#ifndef THREAD_ENCODE_H
#define THREAD_ENCODE_H

#include <QThread>

#include "ecc.h"
#include "statcollector.h"

class EncoderThread : public QThread
{
public:
    EncoderThread(ecc &coder, size_t restart_block_cnt, StatCollector &stat);

protected:
    void run();
private:

    ecc &coder;
    size_t restart_block_cnt;
    StatCollector &stat;
};

#endif // THREAD_ENCODE_H
