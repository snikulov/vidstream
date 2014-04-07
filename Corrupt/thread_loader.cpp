#include "thread_loader.h"

#include <exception>
#include <QImage>
#include <QCoreApplication>

#include "avhandler.h"
#include "bitmap.h"
#include "jpegops.h"
#include "membuf.h"
#include "split.h"
#include "thread_packetize.h"

#define SETTINGS_FILE "settings.conf"

LoaderThread::LoaderThread(StatCollector &stat, size_t &rst_count,
                       std::unique_ptr<uint8_t[]> &res_buffer,
                       JpegHeader& jpeg_header) :
    image_width(1280),
    image_height(720),
    scaled_width(image_width / 2),
    scaled_height(image_height / 2),
    transmit_restart_count(rst_count),
    image_size(50000),
    image_buffer_size(image_size),
    body_size(0),
    jpeg_header(jpeg_header),
    hdr_buf_initialized(false),
    reorder_blocks(false),
    grayscale(false),
    res_raster(new Bitmap(image_width, image_height)),
    src_raster(new Bitmap(image_width, image_height)),
    body_buffer(new uint8_t[image_buffer_size]),
    res_buffer(res_buffer),
    interlace_blocks(new InterlaceControl(settings.block_num,
                                          settings.block_denom)),
    stat(stat)
{
    try {
        LoadSettingsFromFile(SETTINGS_FILE, stored_settings[0],
                                            stored_settings[1]);
        settings = stored_settings[0];
    } catch(...) {
    }
}

bool LoaderThread::loadImageFile()
{
    // fetch raster data from avi
    QImage image;
    stat.StartTimer(StatCollector::TIMER_AVI);
    if (!AVHandler::Instance()->ReadFrame2QImage(0, image)) {
        return false;
    }
    stat.StopTimer(StatCollector::TIMER_AVI);

    // frame resolution differs from bitmaps size
    if (static_cast<size_t>(image.width()) != image_width ||
        static_cast<size_t>(image.height()) != image_height) {
        // loader is unable to warn receiver of non-standard resolution
        throw std::runtime_error("Invalid resolution");
    }

    // convert QImage into RGB888 buffer

    if (image.format() != QImage::Format_RGB888) {
        // slow copying
        for (size_t i = 0; i < image_height; i++) {
            for (size_t j = 0; j < image_width; j++) {
                src_raster->GetPixel(i, j)[0] = qRed  (image.pixel(j, i));
                src_raster->GetPixel(i, j)[1] = qGreen(image.pixel(j, i));
                src_raster->GetPixel(i, j)[2] = qBlue (image.pixel(j, i));
            }
        }
    } else {
        // Format_RGB888, can be copied directly
        for (size_t i = 0; i < image_height; i++) {
            memcpy(src_raster->GetPixel(i, 0),
                   image.scanLine(i), src_raster->GetRowStride());
        }
    }

    image.detach();

    // scale bitmap, write into dst
    stat.StartTimer(StatCollector::TIMER_SCALING);
    std::unique_ptr<Bitmap> scaled_raster(bilinear_resize(*src_raster, scaled_width, scaled_height));
    stat.StopTimer(StatCollector::TIMER_SCALING);

    // create JPEG from scaled_raster, write to temp file
    // temp is a JPEG file containing the image to be transmitted

    std::string tempname = "temp";

    stat.StartTimer(StatCollector::TIMER_JPEG_CREATE);
    if (reorder_blocks && grayscale && settings.rst_block_size == 4) {
        change_order(*scaled_raster, 8);
    }
    write_JPEG_file(scaled_raster->GetData(),
                    scaled_raster->GetWidth(), scaled_raster->GetHeight(),
                    tempname.c_str(), settings.lum_quality, settings.chrom_quality,
                    settings.rst_block_size, grayscale);
    stat.StopTimer(StatCollector::TIMER_JPEG_CREATE);

    // now read JPEG from temp file

    if (fin.is_open()) {
        fin.close();
    }
    fin.open(tempname, std::ios::binary);
    if (!fin) {
        throw std::runtime_error("Failed to load image");
    }

    // get image size
    fin.seekg(0, std::ios::end);
    image_size = fin.tellg();
    fin.seekg(0);

    // resize buffers if image doesn't fit in them
    if (image_size > image_buffer_size) {
        try {
            throw std::runtime_error("JPEG size larger than buffer size");
        } catch (const std::exception &e) {
            qDebug() << "Exception occurred: " << e.what();
            throw;
            //return false;
        }

        hdr_buf_initialized = false;
    }
    membuf sbuf_body((char *) body_buffer.get(), image_size);
    std::ostream fbdy(&sbuf_body);

#ifdef GENERATE_HEADER
    std::unique_ptr<std::ostream> fhdr;
    std::unique_ptr<membuf> sbuf_res;
    //hdr_buf_initialized = false;
    if (!hdr_buf_initialized) {
        sbuf_res = std::unique_ptr<membuf>
                   (new membuf((char *) jpeg_header.data, image_size));
        fhdr = std::unique_ptr<std::ostream> (new std::ostream(sbuf_res.get()));
    }
    stat.StartTimer(StatCollector::TIMER_FILEIO);
    size_t temp_restart_count = 0;
    if (!split_file(fin, fhdr.get(), fbdy, 1, temp_restart_count)) {
        throw std::runtime_error("Image parsing error");
    }
    if (transmit_restart_count != temp_restart_count &&
        transmit_restart_count != 0) {
        //throw std::runtime_error("RST count changed");
    }
    transmit_restart_count = temp_restart_count;

    //history.resize(transmit_restart_count);

    stat.StopTimer(StatCollector::TIMER_FILEIO);
    if (!hdr_buf_initialized) {
        jpeg_header.size = sbuf_res->written();
        hdr_buf_initialized = true;
    }
#else
    if (!hdr_buf_initialized) {
        std::ifstream fhdr("header", std::ios::binary);
        fhdr.seekg(0, std::ios::end);
        jpeg_header.size = fhdr.tellg();
        if (jpeg_header.size > image_size) {
            qDebug() << "Error: header file is too large";
            throw std::length_error("header file is too large");
            return false;
        }
        fhdr.seekg(0, std::ios::beg);
        fhdr.read((char *) jpeg_header.data(), jpeg_header.size);
    }
    stat.StartTimer(StatCollector::TIMER_FILEIO);
    if (!split_file(fin, NULL, fbdy, 1, transmit_restart_count)) {
        ui->image_corrupt->setText("Error occurred while parsing image");
        return false;
    }

    stat.StopTimer(StatCollector::TIMER_FILEIO);
#endif
    body_size = sbuf_body.written();

    return true;
}

void LoaderThread::corruptImage(uint8_t frame_number)
{
    if (!fin.is_open()) {
        throw std::logic_error("Image not loaded");
        return;
    }
    PacketizerThread packetizer(body_buffer.get(), body_size,
                                frame_number, transmit_restart_count,
                                stat, *interlace_blocks);
    packetizer.start();
    packetizer.wait();
    //packetizer.run();
    //while (packetizer.isRunning()) {
    //    usleep(1000);
    //    QCoreApplication::processEvents();
    //}
}

void LoaderThread::run()
{
    unsigned cur = 0;
    while (1) {
        cur++;
        if (!loadImageFile()) {
            break;
        }
        corruptImage(cur);
    }
}
