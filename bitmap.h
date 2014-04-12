#ifndef BITMAP_H
#define BITMAP_H

#include <cstdint>
#include <memory>
#include <cstring>

class Bitmap
{
public:
    static constexpr size_t CHANNELS_NUM = 3;
    Bitmap(uint8_t data[], size_t width, size_t height);
    Bitmap(size_t width, size_t height);
    Bitmap(const Bitmap &other);
    Bitmap(Bitmap &&other);
    ~Bitmap();

    uint8_t *GetPixel(size_t row, size_t col) {
        // calling const GetPixel() here
        return const_cast<uint8_t *>(static_cast<const Bitmap *>(this)->GetPixel(row, col));
    }
    const uint8_t *GetPixel(size_t row, size_t col) const {
        return &data[row * width * CHANNELS_NUM + col * CHANNELS_NUM];
    }
    uint8_t GetColour(size_t row, size_t col, size_t channel) const {
        return GetPixel(row, col)[channel];
    }

    uint8_t *GetData()             { return data;                 }
    const uint8_t *GetData() const { return data;                 }
    size_t GetWidth()        const { return width;                }
    size_t GetHeight()       const { return height;               }
    size_t GetRowStride()    const { return width * CHANNELS_NUM; }

    bool ReduceHeight(size_t new_height);
    void Clear()       {
        memset(data, 0, width * height * CHANNELS_NUM);
    }

private:
    bool own_data;
    size_t width, height;
    uint8_t *data;

    size_t data_size() const { return width * height * CHANNELS_NUM; }
};

Bitmap* bilinear_resize(Bitmap &img, int newWidth, int newHeight);

void change_order(Bitmap &img, size_t block_size_in_pixels);

#endif // BITMAP_H
