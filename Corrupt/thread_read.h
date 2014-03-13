#ifndef THREAD_READ_H
#define THREAD_READ_H

#include <string>
#include <QThread>

#include "statcollector.h"
#include "transceiver.h"
#include "transport.h"

class ReaderThread : public QThread
{
public:
    ReaderThread(float err_percent, transport &T, Transceiver &tcv,
                 size_t restart_block_cnt, StatCollector &stat);

protected:
    void run();
private:
    float err_percent;
    transport &T;
    Transceiver &tcv;
    size_t restart_block_cnt;
    StatCollector &stat;
};

#endif // THREAD_READ_H
