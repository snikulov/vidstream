#ifndef INTERLACE_H
#define INTERLACE_H

#include "bitmap.h"

class InterlaceControl
{
public:
    InterlaceControl(size_t num, size_t denom);
    bool ContainsIndex(size_t ind) const;
    size_t FirstIndex() const { return cur;                }
    size_t NextIndex(size_t ind) const;
    void Advance()            { cur = (cur + num) % denom; }
    void Reset()              { cur = 0;                   }

    size_t GetNum() const     { return num;                }
    size_t GetDenom() const   { return denom;              }
private:
    size_t num, denom;
    size_t cur;
};

Bitmap* interlace_split_rows(const Bitmap &src, const InterlaceControl &interlace);
void interlace_merge_rows(const Bitmap &rows, Bitmap &dest,
                          const InterlaceControl &interlace);

bool interlace_refresh_block(size_t block_cur,
                             const InterlaceControl &interlace);

#endif // INTERLACE_H
