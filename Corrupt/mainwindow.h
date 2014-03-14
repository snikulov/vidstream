#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "bitmap.h"
#include "ecc.h"
#include "interlace.h"
#include "receiverthread.h"
#include "transceiver.h"

#include <QMainWindow>
#include <fstream>
#include <memory>

namespace Ui {
class MainWindow;
}

struct Settings {
    int lum_quality, chrom_quality;
    int bch_m, bch_t;
    size_t row_num, row_denom;
    size_t block_num, block_denom;
    size_t rst_block_size;

    Settings() :
        lum_quality(20),
        chrom_quality(20),
        bch_m(5), bch_t(4),
        row_num(1), row_denom(1),
        block_num(1), block_denom(1),
        rst_block_size(1)
    { }

    Settings(int lum_quality, int chrom_quality,
             int bch_m, int bch_t,
             size_t row_num, size_t row_denom,
             size_t block_num, size_t block_denom,
             size_t rst_block_size) :
        lum_quality(lum_quality),
        chrom_quality(chrom_quality),
        bch_m(bch_m), bch_t(bch_t),
        row_num(row_num), row_denom(row_denom),
        block_num(block_num), block_denom(block_denom),
        rst_block_size(rst_block_size)
    { }
};

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

    void SetChannelState(bool ok) { broken_channel = !ok;   }
    bool GetChannelState() const  { return !broken_channel; }

    void GetScalingResolution(size_t &w, size_t &h) const;
    bool SetScalingResolution(size_t w, size_t h);

    bool SwitchMode();
    size_t GetMode() const { return cur_mode; }

private slots:
    void on_settingsButton_clicked();

    void on_startButton_clicked();

    void on_openButton_clicked();

    void on_mode1_radioButton_clicked(bool checked);

    void on_mode2_radioButton_clicked(bool checked);

    void on_breakChannelCheckBox_toggled(bool checked);

    void on_grayscaleCheckBox_clicked(bool checked);

    void on_reorderCheckBox_toggled(bool checked);

private:
    Ui::MainWindow *ui;

    // block count with no downsampling on 1280x720 is 14400
    static constexpr size_t MAX_RESTART_BLOCKS = 15000;

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
    bool grayscale;
    bool reorder_blocks;

    std::string filename;
    std::ifstream fin;
    size_t image_size;
    size_t body_size;
    size_t head_size;
    size_t image_buffer_size;
    std::unique_ptr<uint8_t[]> src_buffer;
    std::unique_ptr<uint8_t[]> dst_buffer;
    std::unique_ptr<uint8_t[]> recv_buffer;
    std::unique_ptr<uint8_t[]> body_buffer;
    std::unique_ptr<uint8_t[]> res_buffer;
    bool hdr_buf_initialized;

    std::unique_ptr<Bitmap> recv_raster;
    std::unique_ptr<Bitmap> res_raster;
    std::unique_ptr<ecc> enc_s, enc_r;
    Transceiver t;
    BlockHistory history;
    StatCollector stat;

    std::unique_ptr<InterlaceControl> interlace_rows, interlace_blocks;

    bool loadImageFile();
    void corruptImage(float err_percent, const std::string &out_filename,
                      uint8_t frame_number);
    void processFrames(unsigned frame_count);
    void displayStatistics();
};

#endif // MAINWINDOW_H
