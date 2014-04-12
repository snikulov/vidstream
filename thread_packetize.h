#ifndef SENDERTHREAD_H
#define SENDERTHREAD_H

#include <QThread>
#include <boost/interprocess/ipc/message_queue.hpp>

#include "ecc.h"
#include "interlace.h"
#include "restartblock.h"
#include "statcollector.h"

namespace bipc = boost::interprocess;

class PacketizerThread : public QThread
{
public:
    PacketizerThread(const uint8_t *buffer, const size_t buffer_size,
                     uint8_t frame_number, size_t restart_block_cnt,
                     StatCollector &stat,
                     const InterlaceControl &interlace);

protected:
    void run();

private:
    void TransmitBlock(RestartBlock& block, bipc::message_queue &mq);
    void AddRSTBlock(bipc::message_queue &mq);
    const uint8_t *buffer;
    const size_t buffer_size;
    RestartBlock buf;
    uint8_t frame_number;
    size_t restart_block_cnt;
    StatCollector &stat;
    const InterlaceControl &interlace;
};

#endif // SENDERTHREAD_H
