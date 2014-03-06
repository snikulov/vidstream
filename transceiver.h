#ifndef TRANSCEIVER_H
#define TRANSCEIVER_H

#include <vector>
#include <QMutex>

class Transceiver
{
public:
    Transceiver();
    ~Transceiver();
    size_t GetFrameSize() const;
    int Transmit(const uint8_t *data, size_t size);
    uint8_t* Receive(int timeout, size_t &size);
private:
    QMutex buffer_read;
    QMutex buffer_write;
    size_t buffer_size;
    uint8_t *buffer;

    // forbid auto-cast of timeout parameter
    template <typename T>
    uint8_t* Receive(T timeout, size_t &size);
};

#endif // TRANSCEIVER_H
