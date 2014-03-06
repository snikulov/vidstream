#ifndef SENDERTHREAD_H
#define SENDERTHREAD_H

#include <QThread>

#include "ecc.h"
#include "interlace.h"
#include "restartblock.h"
#include "transceiver.h"
#include "statcollector.h"

class SenderThread : public QThread
{
public:
    SenderThread(const uint8_t *buffer, const size_t buffer_size,
                 Transceiver &t, ecc &encoder, uint8_t frame_number,
                 StatCollector &stat,
                 const InterlaceControl &interlace,
                 bool broken_channel = false);

protected:
    void run();

private:
    int TransmitBlock(RestartBlock& block, uint8_t frame_number,
                                    uint16_t rst_number, uint16_t data_len);

    const uint8_t *buffer;
    const size_t buffer_size;
    RestartBlock buf;
    Transceiver &t;
    ecc &encoder;
    uint8_t frame_number;
    StatCollector &stat;
    const InterlaceControl &interlace;
    bool broken_channel;
};

#endif // SENDERTHREAD_H
