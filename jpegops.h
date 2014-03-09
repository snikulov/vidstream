#ifndef JPEGOPS_H
#define JPEGOPS_H

#include <cstddef>

// (JSAMPLE *) image_buffer points to large array of R,G,B-order data

int read_JPEG_file(void *image_buffer,
                   size_t &image_width, size_t &image_height,
                   const char * filename);

int read_JPEG_mem(void *image_buffer,
                  size_t &image_width, size_t &image_height,
                  void *src, unsigned long bufsize);

void write_JPEG_file(void *image_buffer,
                     size_t width, size_t height,
                     const char * filename, int quality,
                      bool grayscale = false);

void write_JPEG_mem(void *image_buffer,
                    size_t width, size_t height,
                    void **dest, unsigned long *bufsize,
                    int quality,
                    bool grayscale = false);

#endif // JPEGOPS_H
