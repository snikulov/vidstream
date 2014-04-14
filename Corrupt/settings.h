#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <cstddef>

struct Settings {
    int lum_quality, chrom_quality;
    int bch_m, bch_t;
    size_t row_num, row_denom;
    size_t block_num, block_denom;
    size_t rst_block_size;
    bool   BW;
    int64_t channel_width;


    Settings() :
        lum_quality(20),
        chrom_quality(20),
        bch_m(5), bch_t(4),
        row_num(1), row_denom(1),
        block_num(1), block_denom(1),
        rst_block_size(1),
        BW(false),
        channel_width(10000000)
    { }

    Settings(int lum_quality, int chrom_quality,
             int bch_m, int bch_t,
             size_t row_num, size_t row_denom,
             size_t block_num, size_t block_denom,
             size_t rst_block_size,
             bool BW,
             int64_t channel_width) :
        lum_quality(lum_quality),
        chrom_quality(chrom_quality),
        bch_m(bch_m), bch_t(bch_t),
        row_num(row_num), row_denom(row_denom),
        block_num(block_num), block_denom(block_denom),
        rst_block_size(rst_block_size),
        BW(BW),
        channel_width(channel_width)
    { }
};

void LoadSettingsFromFile(std::string filename, Settings &mode1, Settings &mode2);
void SaveSettingsToFile(std::string filename, const Settings &mode1, const Settings &mode2);

#endif // SETTINGS_H
