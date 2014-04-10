#include "interlace.h"

#include <stdexcept>

InterlaceControl::InterlaceControl(size_t num, size_t denom) :
    num(num),
    denom(denom),
    cur(0)
{
    if (num > denom) {
        throw std::invalid_argument("Interlace numerator larger than denominator");
    }
}

bool InterlaceControl::ContainsIndex(size_t ind) const
{
    size_t mod = ind % denom;
    if (cur + num <= denom) {
        return (mod >= cur && mod < cur + num);
    } else {
        return (mod >= cur || mod < (cur + num) % denom);
    }
}

size_t InterlaceControl::NextIndex(size_t ind) const
{
    ind++;
    while (!ContainsIndex(ind)) {
        ind++;
    }
    return ind;
}

Bitmap* interlace_split_rows(const Bitmap &src, const InterlaceControl &interlace)
{
    size_t num = interlace.GetNum(), denom = interlace.GetDenom();
    size_t new_height = (src.GetHeight() * num) / denom;
    if ((src.GetHeight() * num) % denom != 0) {
        new_height++;
    }
    Bitmap *res = new Bitmap(src.GetWidth(), new_height);
    // this bitmap may be larger than needed
    size_t i, dsti = 0;
    for (i = interlace.FirstIndex(); i < src.GetHeight();
         i = interlace.NextIndex(i)) {
        memcpy(res->GetPixel(dsti, 0),
               src.GetPixel(i, 0),
               src.GetWidth() * Bitmap::CHANNELS_NUM);
        dsti++;
    }
    res->ReduceHeight(dsti);
    return res;
}

void interlace_merge_rows(const Bitmap &src, Bitmap &dest,
                          const InterlaceControl &interlace)
{
    size_t i = interlace.FirstIndex();
    for (size_t srci = 0; srci < src.GetHeight(); srci++) {
        memcpy(dest.GetPixel(i, 0),
               src.GetPixel(srci, 0),
               src.GetWidth() * Bitmap::CHANNELS_NUM);
        i = interlace.NextIndex(i);
    }
}

bool interlace_refresh_block(size_t block_cur,
                             const InterlaceControl &interlace)
{
    return interlace.ContainsIndex(block_cur);
}
