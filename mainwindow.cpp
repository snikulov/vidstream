#include "mainwindow.h"
#include "ui_mainwindow.h"

//#include "settingsdialog.h"

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
    jpeg_header(res_buffer.get()),
    hdr_buf_initialized(false),
    recv_raster(new Bitmap(scaled_width, scaled_height)),
    enc_s(new ecc(settings.bch_m, settings.bch_t, &stat)),
    enc_r(new ecc(settings.bch_m, settings.bch_t, &stat)),
    history(MAX_RESTART_BLOCKS),
    port(transport().get_free_port()),
    sender_tp(),
    //reader_tp("127.0.0.1", port),
    reader_tp(),
    loader(stat, transmit_restart_count, res_buffer, jpeg_header),
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
    }
    try {
        LoadSettingsFromFile("settings.conf", stored_settings[0],
                                              stored_settings[1]);
        settings = stored_settings[0];
    } catch(...) {
    }

    // queues created by a dead process may hang
    boost::interprocess::message_queue::remove(TO_READ_MSG);
    boost::interprocess::message_queue::remove(TO_SEND_MSG);
    boost::interprocess::message_queue::remove(TO_ENCODE_MSG);
    boost::interprocess::message_queue::remove(TO_DECODE_MSG);
    boost::interprocess::message_queue::remove(TO_OUT_MSG);

    connect(reassembler.get(), SIGNAL(frameReady()), this, SLOT(drawImage()));
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
        loader.start();
        //processFrames(256);
    } else {
        loader.terminate();
        running = false;
        ui->startButton->setText("Continue");
    }
    //SendBytes = 0;
    //StartTime = 0;
}

void MainWindow::SaveSettings()
{
    stored_settings[cur_mode] = settings;
    SaveSettingsToFile("settings.conf", stored_settings[0], stored_settings[1]);
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
    recv_raster->Clear();
    while (running) {
        displayStatistics();
        stat.Reset();
        stat.StartTimer(StatCollector::TIMER_FRAME);
        st = clock();
        if (!loadImageFile()) {
            break;
        }
        qDebug() << cur << " frames processed";
        char buf[5];
        snprintf(buf, 5, "%03d", cur + 1);
        out_filename = out_prefix + filename + buf + ".jpg";
        corruptImage(cur);
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
    return loader.loadImageFile();
}

void MainWindow::corruptImage(uint8_t frame_number)
{
    loader.corruptImage(frame_number);
}

void MainWindow::drawImage()
{
    try {
        displayStatistics();
        ComposeJpeg(res_buffer.get() + jpeg_header.size, history, transmit_restart_count);
        size_t input_width, input_height;
        stat.StartTimer(StatCollector::TIMER_JPEG_READ);
        read_JPEG_mem(recv_buffer.get(), input_width, input_height, res_buffer.get(), image_size);
        Bitmap received(recv_buffer.get(), input_width, input_height);
        //if (reorder_blocks && grayscale && settings.rst_block_size == 4) {
        //    change_order(received, 8);
        //}
        stat.StopTimer(StatCollector::TIMER_JPEG_READ);
        stat.StartTimer(StatCollector::TIMER_SCALING);
        // resize bitmap back to full size
        std::unique_ptr<Bitmap> res_raster(bilinear_resize(received, image_width, image_height));
        stat.StopTimer(StatCollector::TIMER_SCALING);
        QImage image(res_raster->GetData(), res_raster->GetWidth(), res_raster->GetHeight(),
                     QImage::Format_RGB888);
        ui->image_corrupt->setPixmap(QPixmap::fromImage(image));
        ui->image_corrupt->repaint();
        static size_t cur_frame = 0;
        cur_frame++;
        char out_filename[64];
        snprintf(out_filename, sizeof(out_filename),
                 "res_frames/frame%03lu.jpg", cur_frame);
        stat.StartTimer(StatCollector::TIMER_FILEIO);
        write_JPEG_file(res_raster->GetData(), res_raster->GetWidth(), res_raster->GetHeight(),
                        out_filename,
                        settings.lum_quality, settings.chrom_quality,
                        settings.rst_block_size, false);
        stat.StopTimer(StatCollector::TIMER_FILEIO);
    } catch(...) {
        qDebug() << "Failed to decode/draw image";
        ui->image_corrupt->setText("Failed to decode/draw image");
        ui->image_corrupt->repaint();
    }
    history_mutex.unlock();
}

void MainWindow::displayStatistics()
{
    //ui->infoLabel1->setText(QString::fromStdString(stat.GetStats()));
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
    }
}

void MainWindow::on_bandwidthSpinBox_valueChanged(int arg1)
{
    //ChannelSpeed = arg1 / 8;
    //SendBytes = 0;
    //StartTime = 0;
}

void MainWindow::on_errorSpinBox_valueChanged(int arg1)
{
    if (reader) {
        reader->SetErrPercent(arg1);
    }
}
