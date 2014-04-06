#ifndef JPEGLOADER_H
#define JPEGLOADER_H

#include <cstdint>
#include <cstddef>
#include <fstream>
#include <memory>
#include <QThread>

#include "bitmap.h"
#include "interlace.h"
#include "jpegops.h"
#include "settings.h"
#include "statcollector.h"

class LoaderThread : public QThread
{
    Q_OBJECT

public:
    LoaderThread(StatCollector &stat, size_t &rst_cout,
               std::unique_ptr<uint8_t[]> &res_buffer,
               JpegHeader &jpeg_header);
    bool loadImageFile();
    void corruptImage(uint8_t frame_number);
protected:
    void run();
private:
    std::ifstream fin;

    size_t image_width, image_height;
    size_t scaled_width, scaled_height;
    size_t &transmit_restart_count;
    size_t image_size;
    size_t image_buffer_size;
    size_t body_size;
    JpegHeader &jpeg_header;
    bool hdr_buf_initialized;
    bool reorder_blocks;
    bool grayscale;
    Settings settings;
    Settings stored_settings[2];
    std::unique_ptr<Bitmap> res_raster;
    std::unique_ptr<Bitmap> src_raster;
    std::unique_ptr<uint8_t[]> body_buffer;
    std::unique_ptr<uint8_t[]> &res_buffer;
    std::unique_ptr<InterlaceControl> interlace_blocks;

    StatCollector &stat;
};

#endif // JPEGLOADER_H
