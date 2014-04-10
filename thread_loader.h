#ifndef JPEGLOADER_H
#define JPEGLOADER_H

#include <cstdint>
#include <cstddef>
#include <fstream>
#include <memory>
#include <QThread>
#include <opencv2/opencv.hpp>

#include "bitmap.h"
#include "interlace.h"
#include "jpegops.h"
#include "settings.h"
#include "statcollector.h"

class LoaderThread : public QThread
{
    Q_OBJECT

public:
    constexpr static size_t MAX_IMAGE_SIZE = 50000;

    LoaderThread(StatCollector &stat, size_t &rst_cout,
                 JpegInfo &jpeg_info);
    bool loadImageFile();
    void corruptImage(uint8_t frame_number);
    void Kill() { killed = true; }
    void SetGrayscale(bool f) { grayscale = f; hdr_buf_initialized = false; }
protected:
    void run();
private:

    bool killed;

    std::ifstream fin;

    size_t image_width, image_height;
    size_t scaled_width, scaled_height;
    size_t &transmit_restart_count;
    size_t image_buffer_size;
    size_t body_size;
    JpegInfo &jpeg_info;
    bool hdr_buf_initialized;
    bool reorder_blocks;
    bool grayscale;
    Settings settings;
    Settings stored_settings[2];
    cv::Mat res_raster;
    cv::Mat src_raster;
    std::unique_ptr<uint8_t[]> body_buffer;
    std::unique_ptr<InterlaceControl> interlace_blocks;

    StatCollector &stat;
};

#endif // JPEGLOADER_H
