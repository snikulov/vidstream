#include "transceiver.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>

Transceiver::Transceiver() : buffer_size(0), buffer(NULL)
{
    buffer_read.lock();
}

Transceiver::~Transceiver()
{
    if (buffer) free(buffer);
}

size_t Transceiver::GetFrameSize() const
{
    return buffer_size;
}

int Transceiver::Transmit(const uint8_t *data, size_t size)
{
    buffer_write.lock();
    buffer_size = size;
    buffer = (uint8_t *) realloc(buffer, buffer_size);
    if (!buffer) {
        fprintf(stderr, "Transmit: buffer realloc failed\n");
        buffer_size = 0;
        buffer_write.unlock();
        return 0;
    }
    memcpy(buffer, data, buffer_size);
    buffer_read.unlock();
    return 1;
}

uint8_t* Transceiver::Receive(int timeout, size_t &size)
{
    if (!buffer_read.tryLock(timeout ? timeout : -1)) {
        return NULL;
    }
    if (!buffer || !buffer_size) {
        return NULL;
    }
    uint8_t *res = (uint8_t *) malloc(buffer_size);
    if (!res) {
        fprintf(stderr, "Receive: failed to allocate %lu bytes\n", buffer_size);
        buffer_write.unlock();
        return NULL;
    }
    memcpy(res, buffer, buffer_size);
    size = buffer_size;
    buffer_write.unlock();
    return res;
}
