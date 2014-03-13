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

Bitmap* bicubic_resize(Bitmap &img, int newWidth, int newHeight)
{
        Bitmap *img2 = new Bitmap(newWidth, newHeight);
        //uint8_t * data = img.GetData();
        uint8_t * Data = img2->GetData();
        uint8_t Cc;
        uint8_t C[5] = { 0 };
        uint8_t d0, d2, d3, a0, a1, a2, a3;
        int i, j, k, jj;
        int x, y;
        float dx, dy;
        float tx, ty;

        tx = (float) img.GetWidth()  / newWidth;
        ty = (float) img.GetHeight() / newHeight;

        for (i=0; i < newHeight; i++)
            for (j=0; j < newWidth; j++) {
               x = (int)(tx*j);
               y = (int)(ty*i);

               dx= tx*j - x;
               dy= ty*i - y;

               for (k=0; k < 3; k++) {
                  for (jj=0; jj <= 3; jj++) {
                      const int z = y - 1 + jj;
                      a0 = GetColour(img, z, x, k);
                      d0 = GetColour(img, z, x - 1, k) - a0;
                      d2 = GetColour(img, z, x + 1, k) - a0;
                      d3 = GetColour(img, z, x + 2, k) - a0;

                      //d0 = data[(y-1+jj)*img.GetWidth() * Bitmap::CHANNELS_NUM + (x-1)*Bitmap::CHANNELS_NUM +k] - data[(y-1+jj)*img.GetWidth() * Bitmap::CHANNELS_NUM + (x)*Bitmap::CHANNELS_NUM +k] ;
                      //d2 = data[(y-1+jj)*img.GetWidth() * Bitmap::CHANNELS_NUM + (x+1)*Bitmap::CHANNELS_NUM +k] - data[(y-1+jj)*img.GetWidth() * Bitmap::CHANNELS_NUM + (x)*Bitmap::CHANNELS_NUM +k] ;
                      //d3 = data[(y-1+jj)*img.GetWidth() * Bitmap::CHANNELS_NUM + (x+2)*Bitmap::CHANNELS_NUM +k] - data[(y-1+jj)*img.GetWidth() * Bitmap::CHANNELS_NUM + (x)*Bitmap::CHANNELS_NUM +k] ;
                      //a0 = data[(y-1+jj)*img.GetWidth() * Bitmap::CHANNELS_NUM + (x)*Bitmap::CHANNELS_NUM +k];
                      a1 =  -1.0/3*d0 + d2 -1.0/6*d3;
                      a2 = 1.0/2*d0 + 1.0/2*d2;
                      a3 = -1.0/6*d0 - 1.0/2*d2 + 1.0/6*d3;
                      C[jj] = a0 + a1*dx + a2*dx*dx + a3*dx*dx*dx;

                      d0 = C[0]-C[1];
                      d2 = C[2]-C[1];
                      d3 = C[3]-C[1];
                      a0=C[1];
                      a1 =  -1.0/3*d0 + d2 -1.0/6*d3;
                      a2 = 1.0/2*d0 + 1.0/2*d2;
                      a3 = -1.0/6*d0 - 1.0/2*d2 + 1.0/6*d3;
                      Cc = a0 + a1*dy + a2*dy*dy + a3*dy*dy*dy;
                      // if((int)Cc>255) Cc=255;
                      // if((int)Cc<0)   Cc=0;
                      Data[i * img2->GetWidth() * Bitmap::CHANNELS_NUM + j * Bitmap::CHANNELS_NUM + k] = Cc;
                  }
               }

            }
        return img2;
}
