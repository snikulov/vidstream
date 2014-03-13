#ifndef THREAD_SEND_H
#define THREAD_SEND_H

#include <string>
#include <QThread>

#include "statcollector.h"
#include "transceiver.h"
#include "transport.h"

class SenderThread : public QThread
{
public:
    SenderThread(const char *ip, unsigned port,
                 transport &T, Transceiver &tcv,
                 size_t restart_block_cnt, StatCollector &stat);

protected:
    void run();
private:
    std::string ip;
    unsigned port;
    transport &T;
    Transceiver &tcv;
    size_t restart_block_cnt;
    StatCollector &stat;
};

#endif // THREAD_SEND_H
