#ifndef SENDERTHREAD_H
#define SENDERTHREAD_H

#include <QThread>

#include <boost/interprocess/ipc/message_queue.hpp>

#include "ecc.h"
#include "interlace.h"
#include "restartblock.h"
#include "statcollector.h"

class SenderThread : public QThread
{
public:
    SenderThread(const uint8_t *buffer, const size_t buffer_size,
                 std::string queue_name,
                 ecc &encoder, uint8_t frame_number,
                 StatCollector &stat,
                 const InterlaceControl &interlace);

protected:
    void run();

private:
    int TransmitBlock(RestartBlock& block,
                      boost::interprocess::message_queue &mq);

    const uint8_t *buffer;
    const size_t buffer_size;
    RestartBlock buf;
    std::string queue_name;
    ecc &encoder;
    uint8_t frame_number;
    StatCollector &stat;
    const InterlaceControl &interlace;
};

#endif // SENDERTHREAD_H
