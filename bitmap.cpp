#include "bitmap.h"

#include <cstring>
#include <algorithm>

Bitmap::Bitmap(uint8_t data[], const size_t width, const size_t height) :
    own_data(false), width(width), height(height), data(data)
{
}

Bitmap::Bitmap(const size_t width, const size_t height) :
    own_data(true),
    width(width),
    height(height),
    data(new uint8_t[data_size()])
{
}

Bitmap::Bitmap(const Bitmap &other) :
    own_data(true),
    width(other.width),
    height(other.height),
    data(new uint8_t[data_size()])
{
    memcpy(data, other.data, data_size());
}

Bitmap::~Bitmap()
{
    if (own_data) delete[] data;
}

bool Bitmap::ReduceHeight(size_t new_height)
{
    if (new_height > height) {
        return false;
    }
    height = new_height;
    return true;
}

uint8_t inline GetColour(Bitmap &img, int row, int col, int channel)
{
    using std::min;
    using std::max;
    if (row >= 0 && (size_t) row < img.GetHeight() && col >= 0 && (size_t) col < img.GetWidth()) {
        return img.GetPixel(row, col)[channel];
    } else {
        row = min(max(row, 0), (int) img.GetHeight());
        col = min(max(col, 0), (int) img.GetWidth());
        return img.GetPixel(row,col)[channel];
    }
    //return img.GetPixel(min(max(row, 0), (int) img.GetHeight()), min(max(col, 0), (int) img.GetWidth()))[channel];
}

Bitmap* bilinear_resize(Bitmap &img, int newWidth, int newHeight)
{
    int w = newWidth;
    int h = newHeight;
    Bitmap *img2 = new Bitmap(newWidth, newHeight);
    uint8_t * Data = img2->GetData();
    uint8_t * data = img.GetData();
    int a, b, c, d, x, y, index;
    float tx = (float) (img.GetWidth()  - 1) / w;
    float ty = (float) (img.GetHeight() - 1) / h;
    float x_diff, y_diff;
    int i, j, k;

     for(i=0; i<h; i++)
         for(j=0; j<w; j++) {
              x = (int) (tx * j);
              y = (int) (ty * i);

              x_diff = ((tx * j) -x);
              y_diff = ((ty * i) -y );

              index = y * img.GetWidth() * Bitmap::CHANNELS_NUM + x * Bitmap::CHANNELS_NUM;
              a = (int)index;
              b = (int)(index + Bitmap::CHANNELS_NUM);
              c = (int)(index + img.GetWidth() * Bitmap::CHANNELS_NUM);
              d = (int)(index + img.GetWidth() * Bitmap::CHANNELS_NUM + Bitmap::CHANNELS_NUM);

              for(k=0; k<3; k++) {
                  Data[i * img2->GetWidth() * Bitmap::CHANNELS_NUM + j*Bitmap::CHANNELS_NUM + k] =
                  data[a+k]*(1-x_diff)*(1-y_diff)
                  +data[b+k]*(1-y_diff)*(x_diff)
                  +data[c+k]*(y_diff)*(1-x_diff)
                  +data[d+k]*(y_diff)*(x_diff);
              }
         }
     return img2;
}

void swap_regions(Bitmap &img, size_t r1, size_t c1, size_t r2, size_t c2, size_t w, size_t h)
{
    size_t chunk_width = w * 3;
    char* buffer = new char[chunk_width];
    for (size_t i = 0; i < h; i++) {
        memcpy(buffer, img.GetPixel(r1 + i, c1), chunk_width);
        memcpy(img.GetPixel(r1 + i, c1), img.GetPixel(r2 + i, c2), chunk_width);
        memcpy(img.GetPixel(r2 + i, c2), buffer, chunk_width);
    }
    delete[] buffer;
}

void change_order(Bitmap &img, size_t bsize)
{
    for (size_t row = 0; row < img.GetHeight() - bsize; row += 2 * bsize) {
        for (size_t col = 0; col < img.GetWidth() - 4 * bsize; col += 4 * bsize) {
            swap_regions(img, row, col + 2 * bsize, row + bsize, col,
                         2 * bsize, bsize);
        }
    }
}
