#ifndef THREAD_DECODE_H
#define THREAD_DECODE_H

#include <QThread>

#include "ecc.h"
#include "statcollector.h"

class DecoderThread : public QThread
{
public:
    DecoderThread(ecc &coder, size_t restart_block_cnt, StatCollector &stat);

    void Kill() { killed = true; }
protected:
    void run();
private:

    ecc &coder;
    size_t restart_block_cnt;
    StatCollector &stat;

    bool killed;
};

#endif // THREAD_DECODE_H
