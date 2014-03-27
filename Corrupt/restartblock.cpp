#include "restartblock.h"

#include <stdio.h>
#include <cassert>
#include <cstring>

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

//RestartBlock &RestartBlock::operator=(const RestartBlock &other)
//{
//    if (this != &other) {
//        pushbacks_cnt = other.pushbacks_cnt;
//        data = other.data; // Invalid delete: address x is 2,704 bytes inside a block of 231,136 alloc'd
//    }
//    return *this;
//}

RestartBlock &RestartBlock::operator=(RestartBlock &&other)
{
    if (this != &other) {
        pushbacks_cnt = other.pushbacks_cnt;
        //data = std::move(other.data);
        //fprintf(stderr, "resizing vector from %lu to %lu\n", data.size(), other.data.size());
        //fflush(stderr);
        data.resize(other.data.size());
        //fprintf(stderr, "ok!\n");
        //fflush(stderr);
        memcpy(data.data(), other.data.data(),
               other.data.size() * sizeof(other.data[0]));
        //for (unsigned i = 0; i < data.size(); i++) {
        //    data[i] = other.data[i];
        //}
    }
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
