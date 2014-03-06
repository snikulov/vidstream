#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "settingsdialog.h"

#include "avhandler.h"
#include "interlace.h"
#include "jpegops.h"
#include "membuf.h"
#include "receiverthread.h"
#include "senderthread.h"
#include "split.h"
#include "transceiver.h"

#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <stdexcept>
#include <QByteArray>
#include <QDebug>
#include <QFile>
#include <QImage>
#include <QPixmap>
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    image_width(1280),
    image_height(720),
    scaled_width(image_width / 2),
    scaled_height(image_height / 2),
    running(false),
    video_opened(false),
    cur_mode(0),
    head_size(0),
    image_buffer_size(0),
    hdr_buf_initialized(false),
    recv_raster(scaled_width, scaled_height),
    res_raster(image_width, image_height),
    enc_s(new ecc(settings.bch_m, settings.bch_t)),
    enc_r(new ecc(settings.bch_m, settings.bch_t)),
    history(MAX_RESTART_BLOCKS),
    interlace_rows  (new InterlaceControl(settings.row_num,
                                          settings.row_denom)),
    interlace_blocks(new InterlaceControl(settings.block_num,
                                          settings.block_denom))
{
    try {
        src_buffer  = std::unique_ptr<uint8_t[]>
                (new uint8_t[image_width * image_height * Bitmap::CHANNELS_NUM]);
        dst_buffer  = std::unique_ptr<uint8_t[]>
                (new uint8_t[image_width * image_height * Bitmap::CHANNELS_NUM]);
        recv_buffer = std::unique_ptr<uint8_t[]>
                (new uint8_t[image_width * image_height * Bitmap::CHANNELS_NUM]);
    } catch (const std::bad_alloc &e) {
        qDebug() << "Allocation failed at startup: " << e.what();
        throw;
    }
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    QDir folder(QDir::currentPath()+"/res_frames");
    if (!folder.exists()) {
        folder.mkdir(QDir::currentPath()+"/res_frames");
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    exit(0);
}

void MainWindow::on_startButton_clicked()
{
    if (!video_opened) {
        // doing nothing
        QMessageBox::warning(this, "", "No video file opened");
        return;
    }
    if (!running) {
        running = true;
        ui->startButton->setText("Pause");
        for (size_t i = 0; i < history.size(); i++) {
            history[i].clear();
        }
        processFrames(256);
    } else {
        running = false;
        ui->startButton->setText("Continue");
    }
}

bool MainWindow::SetJpegQuality(int q)
{
    if (q > 0 && q <= 100) {
        settings.jpeg_quality = q;
        return true;
    } else {
        return false;
    }
}

void MainWindow::GetBchParams(int &bch_m, int &bch_t) const
{
    bch_m = settings.bch_m;
    bch_t = settings.bch_t;
}

// this function isn't executed simultaneously
// with corruptImage() or loadImageFile()
// it runs when QCoreApplication::processEvents is called in processFrames()
// i.e. Sender/Receiver threads aren't running
bool MainWindow::SetBchParams(int bch_m, int bch_t)
{
    try {
        enc_s = std::unique_ptr<ecc> (new ecc(bch_m, bch_t));
        enc_r = std::unique_ptr<ecc> (new ecc(bch_m, bch_t));
    } catch (...) { // failed to initialize bch
        return false;
    }
    settings.bch_m = bch_m;
    settings.bch_t = bch_t;
    return true;
}

void MainWindow::GetRowInterlace(size_t &num, size_t &denom) const
{
    num = interlace_rows->GetNum();
    denom = interlace_rows->GetDenom();
}

bool MainWindow::SetRowInterlace(size_t num, size_t denom)
{
    try {
        interlace_rows = std::unique_ptr<InterlaceControl>
                         (new InterlaceControl(num, denom));
    } catch (const std::invalid_argument &e) {
        qDebug() << "Failed to create InterlaceControl: " << e.what();
        return false;
    }
    settings.row_num = num;
    settings.row_denom = denom;
    return true;
}

void MainWindow::GetBlockInterlace(size_t &num, size_t &denom) const
{
    num = interlace_blocks->GetNum();
    denom = interlace_blocks->GetDenom();
}

bool MainWindow::SetBlockInterlace(size_t num, size_t denom)
{
    try {
        interlace_blocks = std::unique_ptr<InterlaceControl>
                           (new InterlaceControl(num, denom));
    } catch (const std::invalid_argument &e) {
        qDebug() << "Failed to create InterlaceControl: " << e.what();
        return false;
    }
    settings.block_num = num;
    settings.block_denom = denom;
    return true;
}

Settings MainWindow::GetSettings() const
{
    return Settings(settings.jpeg_quality,
                    settings.bch_m, settings.bch_t,
                    interlace_rows->GetNum(),
                    interlace_rows->GetDenom(),
                    interlace_blocks->GetNum(),
                    interlace_blocks->GetDenom());
}

int MainWindow::SetSettings(const Settings &new_s)
{
    if (settings.jpeg_quality != new_s.jpeg_quality &&
        !SetJpegQuality(new_s.jpeg_quality)) {
        return false;
    }
    settings.jpeg_quality = new_s.jpeg_quality;
    if ((settings.bch_m != new_s.bch_m  ||
         settings.bch_t != new_s.bch_t) &&
        !SetBchParams(new_s.bch_m, new_s.bch_t)) {
        return false;
    }
    settings.bch_m = new_s.bch_m;
    settings.bch_t = new_s.bch_t;
    if ((settings.row_num != new_s.row_num ||
         settings.row_denom != new_s.row_denom) &&
        !SetRowInterlace(new_s.row_num, new_s.row_denom)) {
        return false;
    }
    settings.row_num = new_s.row_num;
    settings.row_denom = new_s.row_denom;
    if ((settings.block_num != new_s.block_num ||
         settings.block_denom != new_s.block_denom) &&
        !SetBlockInterlace(new_s.block_num, new_s.block_denom)) {
        return false;
    }
    settings.block_num = new_s.block_num;
    settings.block_denom = new_s.block_denom;
    return true;
}

bool MainWindow::SwitchMode()
{
    // first remember the settings for this mode
    stored_settings[cur_mode] = settings;
    // now apply settings for the new mode
    cur_mode = 1 - cur_mode;
    return SetSettings(stored_settings[cur_mode]);
}

void MainWindow::processFrames(unsigned frame_count)
{
    clock_t st = clock();
    filename = "frame";
    std::string in_prefix  = std::string("src_frames/");
    std::string out_prefix = std::string("res_frames/")
                 /*  + std::to_string(ui->errorSlider->value())  */
                     + "out_";
    unsigned cur = 0;
    recv_raster.Clear();
    while (running) {
        ui->infoLabel->setText(QString::fromStdString(stat.GetStats()));
        stat.Reset();
        stat.StartTimer(StatCollector::TIMER_FRAME);
        st = clock();
        if (!loadImageFile()) {
            break;
        }
        qDebug() << cur << " frames processed";
        char buf[5];
        snprintf(buf, 5, "%03d", cur + 1);
        corruptImage(ui->errorSpinBox->value(),
                     out_prefix + filename + buf + ".jpg",
                     cur);
        cur = (cur + 1) % frame_count;
        interlace_rows->Advance();
        interlace_blocks->Advance();
        stat.StopTimer(StatCollector::TIMER_FRAME);
        qDebug("time elapsed: %.5f s\n", (clock() - st) / (1.0 * CLOCKS_PER_SEC));
        QCoreApplication::processEvents();
    }
}

bool MainWindow::loadImageFile()
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

    // if frame resolution is too large, abort
    if (input_width > image_width || input_height > image_height) {
        qDebug() << "Error: resolution too large";
        ui->image_corrupt->setText("Error: resolution too large");
        ui->image_corrupt->repaint();
        QCoreApplication::processEvents();
        return false;
    }

    image_width = input_width;
    image_height = input_height;
    scaled_width = image_width / 2;
    scaled_height = image_height / 2;

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
    std::unique_ptr<Bitmap> src_raster(bilinear_resize(orig_raster, scaled_width, scaled_height));
    stat.StopTimer(StatCollector::TIMER_SCALING);

    // select rows to transmit
    //qDebug() << "calling interlace_split_rows from pid " << getpid() << ", tid " << syscall(SYS_gettid);
    std::unique_ptr<Bitmap> dst(interlace_split_rows(*src_raster,
                                                     *interlace_rows));
    // create JPEG from dst_buffer, write to temp file
    // temp is a JPEG file containing half the image that will be transmitted
    stat.StartTimer(StatCollector::TIMER_JPEG_CREATE);
    write_JPEG_file(dst->GetData(), dst->GetWidth(), dst->GetHeight(),
                    "temp", settings.jpeg_quality);
    stat.StopTimer(StatCollector::TIMER_JPEG_CREATE);

    // now read JPEG from temp file

    if (fin.is_open()) {
        fin.close();
    }
    fin.open("temp", std::ios::binary);
    if (!fin) {
        ui->image_corrupt->setText("Failed to load image.");
        ui->image_corrupt->repaint();
        QCoreApplication::processEvents();
        return false;
    }

    // get image size
    fin.seekg(0, std::ios::end);
    image_size = fin.tellg();
    fin.seekg(0);

    // resize buffers if image doesn't fit in them
    if (image_size > image_buffer_size) {
        try {
            res_buffer  = std::unique_ptr<uint8_t[]> (new uint8_t[2 * image_size]);
            body_buffer = std::unique_ptr<uint8_t[]> (new uint8_t[image_size]);
        } catch (const std::bad_alloc &e) {
            qDebug() << "Allocation failed: " << e.what();
            ui->image_corrupt->setText("Not enough memory.\n");
            image_buffer_size = 0;
            return false;
        }
        hdr_buf_initialized = false;
        image_buffer_size = image_size;
    }
    membuf sbuf_body((char *) body_buffer.get(), image_size);
    std::ostream fbdy(&sbuf_body);

#ifdef GENERATE_HEADER
    std::unique_ptr<std::ostream> fhdr;
    std::unique_ptr<membuf> sbuf_res;
    hdr_buf_initialized = false;
    if (!hdr_buf_initialized) {
        sbuf_res = std::unique_ptr<membuf>
                   (new membuf((char *) res_buffer.get(), image_size));
        fhdr = std::unique_ptr<std::ostream> (new std::ostream(sbuf_res.get()));
    }
    stat.StartTimer(StatCollector::TIMER_FILEIO);
    if (!split_file(fin, fhdr.get(), fbdy, 1, transmit_restart_count)) {
        ui->image_corrupt->setText("Error occurred while parsing image");
        return false;
    }

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

void MainWindow::corruptImage(float err_percent, const std::string &out_filename,
                              uint8_t frame_number)
{
    if (!fin.is_open()) {
        ui->image_corrupt->setText("Image not loaded");
        return;
    }
    // each byte of mask corresponds to a whole RestartBlock
    // Restart block occupies 16x16 pixels after 2v2c compression
    // there are 3600 blocks in a 1280x720 image
    std::unique_ptr<char[]> mask(new char[MAX_RESTART_BLOCKS]);
    SenderThread sender(body_buffer.get(), body_size,
                        t, *enc_s.get(), frame_number,
                        stat,
                        *interlace_blocks);
    ReceiverThread receiver(res_buffer.get() + head_size, mask.get(), // write into body
                            t, *enc_r.get(), history, transmit_restart_count,
                            stat, err_percent);
    sender.start();
    receiver.start();
    while (sender.isRunning() || receiver.isRunning()) {
        usleep(1000);
    }
    try {
        size_t input_width, input_height;
        stat.StartTimer(StatCollector::TIMER_JPEG_READ);
        read_JPEG_mem(recv_buffer.get(), input_width, input_height, res_buffer.get(), image_size);
        stat.StopTimer(StatCollector::TIMER_JPEG_READ);
        stat.StartTimer(StatCollector::TIMER_INTERLACE);
        // insert received lines into recv_raster
        interlace_merge_rows(Bitmap(recv_buffer.get(),input_width, input_height),
                             recv_raster, *interlace_rows);
        // restore broken pixels from neighboring lines
        //for (size_t i = interlace_rows->FirstIndex(), srci = 0;
        //     srci < input_height;
        //     i = interlace_rows->NextIndex(i), srci++) {
        //    for (size_t j = 0; j < input_width; j++) {
        //        int rst_index = (j / 16) + (srci / 16) * (input_width / 16);
        //        if (!mask[rst_index] && i) {
        //            int neighrow = i % 2 ? i - 1 : i + 1;
        //            memcpy(recv_raster.GetPixel(i, j), recv_raster.GetPixel(neighrow, j), 3);
        //            //recv_raster[i * image_width * 3 + j * 3] = 255;
        //        }
        //    }
        //}
        stat.StopTimer(StatCollector::TIMER_INTERLACE);
        stat.StartTimer(StatCollector::TIMER_SCALING);
        // resize bitmap back to full size
        std::unique_ptr<Bitmap> res_raster(bilinear_resize(recv_raster, image_width, image_height));
        stat.StopTimer(StatCollector::TIMER_SCALING);
        QImage image(res_raster->GetData(), res_raster->GetWidth(), res_raster->GetHeight(),
                     QImage::Format_RGB888);
        ui->image_corrupt->setPixmap(QPixmap::fromImage(image));
        ui->image_corrupt->repaint();
        stat.StartTimer(StatCollector::TIMER_FILEIO);
        write_JPEG_file(res_raster->GetData(), res_raster->GetWidth(), res_raster->GetHeight(),
                        out_filename.c_str(), settings.jpeg_quality);
        stat.StopTimer(StatCollector::TIMER_FILEIO);
    } catch(...) {
        qDebug() << "Failed to decode/draw image";
        ui->image_corrupt->setText("Failed to decode/draw image");
        ui->image_corrupt->repaint();
        throw;
    }
}

void MainWindow::on_settingsButton_clicked()
{
    if (running) {
        ui->startButton->click();
    }
    SettingsDialog *dialog = new SettingsDialog(this);
    dialog->show();
}

void MainWindow::on_openButton_clicked()
{
    video_opened = false;
    QString filename = QFileDialog::getOpenFileName(this, tr("Open File"), ".", tr("Any files (*.*);;mp4 video (*.mp4);;avi video (*.avi)"));
    int ret;
    if ((ret = AVHandler::Instance()->open_input_file(filename.toAscii().constData()))) {
        qDebug() << "Failed to open file " << filename;
        qDebug() << "Error code " << ret;
        ui->image_corrupt->setText("Failed to open video file");
    } else {
        video_opened = true;
    }
}
