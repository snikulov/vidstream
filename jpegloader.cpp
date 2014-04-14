#include "jpegloader.h"

#include <exception>
#include <QImage>

#include "avhandler.h"
#include "bitmap.h"
#include "jpegops.h"
#include "membuf.h"
#include "split.h"
#include "thread_packetize.h"

#define SETTINGS_FILE "settings.conf"

JpegLoader::JpegLoader(StatCollector &stat) :
    image_width(1280),
    image_height(720),
    scaled_width(image_width / 2),
    scaled_height(image_height / 2),
    image_size(50000),
    image_buffer_size(image_size),
    head_size(0),
    body_size(0),
    hdr_buf_initialized(false),
    reorder_blocks(false),
    grayscale(false),
    res_raster(new Bitmap(image_width, image_height)),
    body_buffer(new uint8_t[image_buffer_size]),
    src_buffer(new uint8_t[image_width * image_height *
                           Bitmap::CHANNELS_NUM]),
    res_buffer(new uint8_t[2 * image_buffer_size]),
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

bool JpegLoader::loadImageFile()
{
    // fetch raster data from avi
    size_t input_width, input_height;
    QImage image;
    stat.StartTimer(StatCollector::TIMER_AVI);
    if (!AVHandler::Instance()->ReadFrame2QImage(0, image)) {
        return false;
    }
    stat.StopTimer(StatCollector::TIMER_AVI);

    input_width = image.width();
    input_height = image.height();

    // frame resolution differs from bitmaps size
    if (input_width != image_width || input_height != image_height) {
        // loader is unable to warn receiver of non-standard resolution
        throw std::runtime_error("Invalid resolution");
    }

    // convert QImage into RGB888 buffer

    if (image.format() != QImage::Format_RGB888) {
        // slow copying
        int cur = 0;
        for (size_t i = 0; i < input_height; i++) {
            for (size_t j = 0; j < input_width; j++) {
                src_buffer[cur] =     qRed  (image.pixel(j, i));
                src_buffer[cur + 1] = qGreen(image.pixel(j, i));
                src_buffer[cur + 2] = qBlue (image.pixel(j, i));
                cur += 3;
            }
        }
    } else {
        // Format_RGB888, can be copied directly
        for (size_t i = 0; i < input_height; i++) {
            memcpy(src_buffer.get() + i * image_width * Bitmap::CHANNELS_NUM,
                   image.scanLine(i), image_width * Bitmap::CHANNELS_NUM);
        }
    }

    image.detach();

    // orig_raster - unscaled
    // src_raster - scaled bitmap
    Bitmap orig_raster(src_buffer.get(), input_width, input_height);
    stat.StartTimer(StatCollector::TIMER_SCALING);
    std::unique_ptr<Bitmap> dst(bilinear_resize(orig_raster, scaled_width, scaled_height));
    stat.StopTimer(StatCollector::TIMER_SCALING);

    // create JPEG from dst_buffer, write to temp file
    // temp is a JPEG file containing half the image that will be transmitted

    std::string tempname = "temp";

    stat.StartTimer(StatCollector::TIMER_JPEG_CREATE);
    if (reorder_blocks && grayscale && settings.rst_block_size == 4) {
        change_order(*dst, 8);
    }
    write_JPEG_file(dst->GetData(), dst->GetWidth(), dst->GetHeight(),
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
                   (new membuf((char *) res_buffer.get(), image_size));
        fhdr = std::unique_ptr<std::ostream> (new std::ostream(sbuf_res.get()));
    }
    stat.StartTimer(StatCollector::TIMER_FILEIO);
    size_t temp_restart_count;
    if (!split_file(fin, fhdr.get(), fbdy, 1, temp_restart_count)) {
        throw std::runtime_error("Image parsing error");
    }
    if (transmit_restart_count != temp_restart_count) {
        throw std::runtime_error("RST count changed");
    }
    transmit_restart_count = temp_restart_count;

    //history.resize(transmit_restart_count);

    stat.StopTimer(StatCollector::TIMER_FILEIO);
    if (!hdr_buf_initialized) {
        head_size = sbuf_res->written();
        hdr_buf_initialized = true;
    }
#else
    if (!hdr_buf_initialized) {
        std::ifstream fhdr("header", std::ios::binary);
        fhdr.seekg(0, std::ios::end);
        head_size = fhdr.tellg();
        if (head_size > image_size) {
            qDebug() << "Error: header file is too large";
            throw std::length_error("header file is too large");
            return false;
        }
        fhdr.seekg(0, std::ios::beg);
        fhdr.read((char *) res_buffer.get(), head_size);
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

void JpegLoader::corruptImage(uint8_t frame_number)
{
    if (!fin.is_open()) {
        throw std::logic_error("Image not loaded");
        return;
    }
    // each byte of mask corresponds to a whole RestartBlock
    //std::unique_ptr<char[]> mask(new char[MAX_RESTART_BLOCKS]);
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
