#ifndef THREAD_ENCODE_H
#define THREAD_ENCODE_H

#include <QThread>

#include "ecc.h"
#include "statcollector.h"

extern uint32_t StartTime;
extern uint64_t SendBytes;
extern struct timespec start_time, cur_time;
extern uint32_t ChannelSpeed;

class EncoderThread : public QThread
{
public:
    EncoderThread(ecc &coder, StatCollector &stat);

    void Kill() { killed = true; }
protected:
    void run();
private:

    ecc &coder;
    StatCollector &stat;

    bool killed;
};

#endif // THREAD_ENCODE_H
