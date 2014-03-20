#include "restartblock.h"

#include <stdio.h>
#include <cassert>

RestartBlock::RestartBlock() :
    pushbacks_cnt(0)
{
    data.resize(info_len);
}

RestartBlock::RestartBlock(uint8_t *ptr, size_t size) :
    pushbacks_cnt(0)
{
    data.resize(info_len + size);
    for (size_t i = 0; i < size; i++) {
        data[info_len + i] = ptr[i];
    }
}

RestartBlock &RestartBlock::operator=(const RestartBlock &other)
{
    pushbacks_cnt = other.pushbacks_cnt;
    data = other.data;
    return *this;
}

RestartBlock &RestartBlock::operator=(RestartBlock &&other)
{
    pushbacks_cnt = other.pushbacks_cnt;
    data = std::move(other.data);
    return *this;
}

void RestartBlock::clear()
{
    data.clear();
    data.resize(info_len);
    pushbacks_cnt = 0;
}

void RestartBlock::set_info(uint8_t frame_number, uint16_t rst_block_number, uint16_t length)
{
    data[0] = frame_number;
    data[1] = rst_block_number / 0xFF;
    data[2] = rst_block_number % 0xFF;
    data[3] = length / 0xFF;
    data[4] = length % 0xFF;
}
