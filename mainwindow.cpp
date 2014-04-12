#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "settingsdialog.h"

#include "avhandler.h"
#include "interlace.h"
#include "jpegops.h"
#include "membuf.h"
#include "split.h"
#include "thread_packetize.h"
#include "thread_encode.h"
#include "thread_send.h"
#include "thread_read.h"
#include "thread_decode.h"
#include "thread_reassemble.h"
#include "threaded_coder.h"
#include "transport.h"

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
#include <opencv2/opencv.hpp>

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
    broken_channel(false),
    image_buffer_size(50000),
    res_buffer(new uint8_t[2 * image_buffer_size]),
    jpeg_info(res_buffer.get()),
    hdr_buf_initialized(false),
    recv_raster(new Bitmap(scaled_width, scaled_height)),
    enc_s(new ecc(settings.bch_m, settings.bch_t, &stat)),
    enc_r(new ecc(settings.bch_m, settings.bch_t, &stat)),
    history(MAX_RESTART_BLOCKS),
    port(transport().get_free_port()),
    sender_tp(),
    //reader_tp("127.0.0.1", port),
    reader_tp(),
    encoder(new EncoderThread(*enc_s, stat)),
    sender(new SenderThread("127.0.0.1", port, sender_tp, stat)),
    reader(new ReaderThread(0.0, reader_tp, stat)),
    decoder(new DecoderThread(*enc_r, stat)),
    reassembler(new ReassemblerThread(history, transmit_restart_count,
                                      history_mutex, stat, false)),
    interlace_rows  (new InterlaceControl(settings.row_num,
                                          settings.row_denom)),
    interlace_blocks(new InterlaceControl(settings.block_num,
                                          settings.block_denom))
{
    try {
        recv_buffer = std::unique_ptr<uint8_t[]>
                (new uint8_t[image_width * image_height * Bitmap::CHANNELS_NUM]);
    } catch (const std::bad_alloc &e) {
        qDebug() << "Allocation failed at startup: " << e.what();
        throw;
    }
    ui->setupUi(this);
    ui->reorderCheckBox->setDisabled(true);
    this->setAttribute(Qt::WA_DeleteOnClose);
    QDir folder(QDir::currentPath()+"/res_frames");
    if (!folder.exists()) {
        folder.mkdir(QDir::currentPath()+"/res_frames");
        system("rm res_frames/*");
    }
    system("rm res_frames/*");

    try {
        LoadSettingsFromFile("settings.conf", stored_settings[0],
                                              stored_settings[1]);
        settings = stored_settings[0];
    } catch(...) {
        SaveSettings();
    }

    // queues created by a dead process may hang
    boost::interprocess::message_queue::remove(TO_READ_MSG);
    boost::interprocess::message_queue::remove(TO_SEND_MSG);
    boost::interprocess::message_queue::remove(TO_ENCODE_MSG);
    boost::interprocess::message_queue::remove(TO_DECODE_MSG);
    boost::interprocess::message_queue::remove(TO_OUT_MSG);

    connect(reassembler.get(), SIGNAL(frameReady()), this, SLOT(drawImage()),
            Qt::DirectConnection);
            //Qt::DirectConnection);
    //connect(reassembler.get(), SIGNAL(frameReady()), qApp, SLOT(aboutQt()));

    decoder->start();
    reader->start();
    sender->start();
    encoder->start();
    reassembler->start();
}

MainWindow::~MainWindow()
{
    delete ui;
    exit(0);
}

bool MainWindow::SetJpegQuality(int lum, int chrom)
{
    if ((lum > 0 && lum <= 100) ||
        (chrom > 0 && chrom <= 100)) {
        settings.lum_quality = lum;
        settings.chrom_quality = chrom;
        hdr_buf_initialized = false;
        return true;
    } else {
        return false;
    }
}

bool MainWindow::SetBlockSize(size_t rst_block_size)
{
    if (rst_block_size < 1) {
        return false;
    }
    settings.rst_block_size = rst_block_size;
    hdr_buf_initialized = false;
    return true;
}

void MainWindow::GetBchParams(int &bch_m, int &bch_t) const
{
    bch_m = settings.bch_m;
    bch_t = settings.bch_t;
}

bool MainWindow::SetBchParams(int bch_m, int bch_t)
{
    try {
        ecc a(bch_m, bch_t, NULL);
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
    return Settings(settings.lum_quality,
                    settings.chrom_quality,
                    settings.bch_m, settings.bch_t,
                    interlace_rows->GetNum(),
                    interlace_rows->GetDenom(),
                    interlace_blocks->GetNum(),
                    interlace_blocks->GetDenom(),
                    settings.rst_block_size);
}

int MainWindow::SetSettings(const Settings &new_s)
{
    if ((settings.lum_quality != new_s.lum_quality ||
         settings.chrom_quality != new_s.chrom_quality) &&
        !SetJpegQuality(new_s.lum_quality, new_s.chrom_quality)) {
        return false;
    }
    settings.lum_quality = new_s.lum_quality;
    settings.chrom_quality = new_s.chrom_quality;
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
    settings.rst_block_size = new_s.rst_block_size;
    return true;
}

void MainWindow::GetScalingResolution(size_t &w, size_t &h) const
{
    w = scaled_width;
    h = scaled_height;
}

bool MainWindow::SetScalingResolution(size_t w, size_t h)
{
    scaled_width = w;
    scaled_height = h;
    recv_raster = std::unique_ptr<Bitmap>(new Bitmap(scaled_width, scaled_height));
    return true;
}

bool MainWindow::SwitchMode()
{
    // save current settings
    stored_settings[cur_mode] = settings;
    // now apply new settings
    cur_mode = 1 - cur_mode;
    return SetSettings(stored_settings[cur_mode]);
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
        ui->recordButton->setEnabled(false);
        ui->startButton->setText("Pause");
        loader->start();
        loader->SetGrayscale(ui->grayscaleCheckBox->isChecked());
        //processFrames(256);
    } else {
        loader->Kill();
        loader->wait();
        running = false;
        ui->recordButton->setEnabled(true);
        ui->startButton->setText("Continue");
    }
    SendBytes = 0;
    StartTime = 0;
}

void MainWindow::SaveSettings()
{
    stored_settings[cur_mode] = settings;
    SaveSettingsToFile("settings.conf", stored_settings[0], stored_settings[1]);
}

void MainWindow::drawImage()
{
    try {
        static bool f = false;
        if (f) {
            stat.StopTimer(StatCollector::TIMER_FRAME);
        } else {
            f = true;
        }
        stat.FinishFrame();
        displayStatistics();
        stat.Reset();
        stat.StartFrame();
        stat.StartTimer(StatCollector::TIMER_FRAME);
        //memcpy(res_buffer.get(), jpeg_info.data, jpeg_info.size);
        ComposeJpeg(res_buffer.get() + jpeg_info.header_size,
                    history, transmit_restart_count);
        size_t input_width, input_height;
        stat.StartTimer(StatCollector::TIMER_JPEG_READ);
        if (!read_JPEG_mem(recv_buffer.get(), input_width, input_height,
                           res_buffer.get(), LoaderThread::MAX_IMAGE_SIZE)) {
            throw std::runtime_error("Invalid JPEG");
        }
        cv::Mat received = cv::Mat(input_height, input_width,
                                   CV_8UC3, recv_buffer.get());
        //if (reorder_blocks && grayscale && settings.rst_block_size == 4) {
        //    change_order(received, 8);
        //}
        stat.StopTimer(StatCollector::TIMER_JPEG_READ);
        stat.StartTimer(StatCollector::TIMER_SCALING);
        // resize bitmap back to full size
        cv::Mat res_raster;
        cv::resize(received, res_raster, cv::Size(image_width, image_height),
                   0, 0, cv::INTER_LINEAR);
        stat.StopTimer(StatCollector::TIMER_SCALING);
        QImage image(res_raster.data, res_raster.cols, res_raster.rows,
                     QImage::Format_RGB888);
        ui->image_corrupt->setPixmap(QPixmap::fromImage(image));
        static size_t cur_frame = 0;
        cur_frame++;
        char out_filename[64];
        snprintf(out_filename, sizeof(out_filename),
                 "res_frames/frame%03lu.jpg", cur_frame);
        stat.StartTimer(StatCollector::TIMER_FILEIO);
        write_JPEG_file(res_raster.data, res_raster.cols, res_raster.rows,
                        out_filename,
                        settings.lum_quality, settings.chrom_quality,
                        settings.rst_block_size, false);
        stat.StopTimer(StatCollector::TIMER_FILEIO);
    } catch(...) {
        qDebug() << "Failed to decode/draw image";
        ui->image_corrupt->setText("Failed to decode/draw image");
    }
    history_mutex.unlock();
}

void MainWindow::displayStatistics()
{
    ui->infoLabel1->setText(QString::fromStdString(
                                stat.GetPacketSizeStats() + "\n" +
                                stat.GetFrameSizeStats()  + "\n" +
                                stat.GetErrorStats()      + "\n"));
    ui->infoLabel2->setText(QString::fromStdString(stat.GetTimerStats()));
}

void MainWindow::on_openButton_clicked()
{
    video_opened = false;
    QString filename = QFileDialog::getOpenFileName(this, tr("Open File"), ".", tr("Any files (*.*);;mp4 video (*.mp4);;avi video (*.avi)"));
    int ret;
    if ((ret = AVHandler::Instance()->open_input_file(filename.toStdString().c_str()))) {
        qDebug() << "Failed to open file " << filename;
        qDebug() << "Error code " << ret;
        ui->image_corrupt->setText("Failed to open video file");
    } else {
        video_opened = true;
        loader = std::unique_ptr<LoaderThread>(new LoaderThread(stat, transmit_restart_count,
                                                                jpeg_info));
        loader->SetGrayscale(ui->grayscaleCheckBox->isChecked());
    }
}

void MainWindow::on_bandwidthSpinBox_valueChanged(int arg1)
{
    ChannelSpeed = arg1 / 8;
    SendBytes = 0;
    StartTime = 0;
}

void MainWindow::on_errorSpinBox_valueChanged(int arg1)
{
    if (reader) {
        reader->SetErrPercent(arg1);
    }
}

void MainWindow::on_recordButton_clicked()
{
   if (!running) {
       system("ffmpeg -i res_frames/frame%03d.jpg -c:v huffyuv video.avi");
   }
}

void MainWindow::on_grayscaleCheckBox_clicked(bool checked)
{
   if (loader) {
       loader->SetGrayscale(checked);
   }
}

void MainWindow::on_breakChannelCheckBox_clicked(bool checked)
{
    reader->SetBrokenChannel(checked);
}

void MainWindow::on_settingsButton_clicked()
{
    SettingsDialog *dialog = new SettingsDialog(this);
    dialog->show();
}
