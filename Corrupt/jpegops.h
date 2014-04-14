#ifndef JPEGOPS_H
#define JPEGOPS_H

#include <cstddef>
#include <cstdint>

struct JpegInfo
{
    size_t header_size;
    size_t file_size;
};

// (JSAMPLE *) image_buffer points to large array of R,G,B-order data

int read_JPEG_file(void *image_buffer,
                   size_t &image_width, size_t &image_height,
                   const char * filename);

int read_JPEG_mem(void *image_buffer,
                  size_t &image_width, size_t &image_height,
                  void *src, unsigned long bufsize);

void write_JPEG_file(void *image_buffer,
                     size_t width, size_t height,
                     const char * filename,
                     int lum_quality, int chrom_quality,
                     size_t rst_block_size,
                     bool grayscale = false);

void write_JPEG_mem(void *image_buffer,
                    size_t width, size_t height,
                    void **dest, unsigned long *bufsize,
                    int lum_quality, int chrom_quality,
                    size_t rst_block_size,
                    bool grayscale = false);

#endif // JPEGOPS_H
