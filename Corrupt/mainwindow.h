#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "bitmap.h"
#include "ecc.h"
#include "interlace.h"
#include "thread_loader.h"
#include "jpegops.h"
#include "settings.h"
#include "thread_packetize.h"
#include "thread_encode.h"
#include "thread_send.h"
#include "thread_read.h"
#include "thread_decode.h"
#include "thread_reassemble.h"

#include <fstream>
#include <memory>
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void GetJpegQuality(int &lum, int &chrom) const
    {
        lum = settings.lum_quality;
        chrom = settings.chrom_quality;
    }
    void SetChannelState(bool ok) { broken_channel = !ok;   }
    bool GetChannelState() const  { return !broken_channel; }
    bool SetJpegQuality(int lum, int chrom);

    size_t GetBlockSize() const { return settings.rst_block_size; }
    bool SetBlockSize(size_t rst_block_size);
    void GetBchParams(int &bch_m, int &bch_t) const;
    bool SetBchParams(int bch_m, int bch_t);
    void GetRowInterlace(size_t &num, size_t &denom) const;
    bool SetRowInterlace(size_t num, size_t denom);
    void GetBlockInterlace(size_t &num, size_t &denom) const;
    bool SetBlockInterlace(size_t num, size_t denom);
    Settings GetSettings() const;
    int SetSettings(const Settings &new_s);

    void GetScalingResolution(size_t &w, size_t &h) const;
    bool SetScalingResolution(size_t w, size_t h);

    bool SwitchMode();
    size_t GetMode() const { return cur_mode; }

    void SaveSettings();

private slots:
    void drawImage();

    void on_startButton_clicked();

    void on_openButton_clicked();

   // void on_mode1_radioButton_clicked(bool checked);

   // void on_mode2_radioButton_clicked(bool checked);

   // void on_breakChannelCheckBox_toggled(bool checked);

   // void on_grayscaleCheckBox_clicked(bool checked);

   // void on_reorderCheckBox_toggled(bool checked);

    void on_bandwidthSpinBox_valueChanged(int arg1);

    void on_errorSpinBox_valueChanged(int arg1);

    void on_recordButton_clicked();

    void on_grayscaleCheckBox_clicked(bool checked);

    void on_breakChannelCheckBox_clicked(bool checked);

    void on_settingsButton_clicked();

    void on_grayscaleCheckBox_clicked();

    void on_grayscaleCheckBox_stateChanged(int arg1);

    void on_bandwidthSpinBox_valueChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;

    // block count with no downsampling on 1280x720 is 14400
    static constexpr size_t MAX_RESTART_BLOCKS = 15000;

    static constexpr char const *output_queue_name = "transceiver_queue";
    static constexpr char const *input_queue_name = "transceiver_queue";

    size_t image_width;
    size_t image_height;
    size_t scaled_width;
    size_t scaled_height;

    size_t transmit_restart_count; // stores the total number of RST blocks

    bool running;
    bool video_opened;

    size_t cur_mode;
    Settings settings;
    Settings stored_settings[2];

    bool broken_channel;

    std::string filename;
    std::string out_filename;

    std::ifstream fin;
    size_t image_size;
    size_t body_size;
    size_t image_buffer_size;
    std::unique_ptr<uint8_t[]> recv_buffer;
    std::unique_ptr<uint8_t[]> res_buffer;
    JpegInfo jpeg_info;
    bool hdr_buf_initialized;

    std::unique_ptr<Bitmap> recv_raster;
    std::unique_ptr<Bitmap> res_raster;
    std::unique_ptr<ecc> enc_s, enc_r;
    BlockHistory history;
    QMutex history_mutex;
    StatCollector stat;

    unsigned port;
    transport sender_tp, reader_tp;

    std::unique_ptr<LoaderThread> loader;
    std::unique_ptr<EncoderThread> encoder;
    std::unique_ptr<SenderThread>  sender;
    std::unique_ptr<ReaderThread>  reader;
    std::unique_ptr<DecoderThread> decoder;
    std::unique_ptr<ReassemblerThread> reassembler;

    std::unique_ptr<InterlaceControl> interlace_rows, interlace_blocks;

    void displayStatistics();
};

#endif // MAINWINDOW_H
