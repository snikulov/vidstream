#include "restartblock.h"

#include <stdio.h>

RestartBlock::RestartBlock() :
    pushbacks_cnt(0)
{
    data.resize(info_len);
}

RestartBlock::RestartBlock(uint8_t *ptr, size_t size) :
    pushbacks_cnt(0)
{
    data.resize(info_len);
    data.reserve(info_len + size);
    for (size_t i = 0; i < size; i++) {
        data.push_back(ptr[i]);
    }
}

void RestartBlock::set_info(uint8_t frame_number, uint16_t rst_block_number, uint16_t length)
{
    data[0] = frame_number;
    data[1] = rst_block_number / 0xFF;
    data[2] = rst_block_number % 0xFF;
    data[3] = length / 0xFF;
    data[4] = length % 0xFF;
}
