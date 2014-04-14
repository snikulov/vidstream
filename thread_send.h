#ifndef THREAD_SEND_H
#define THREAD_SEND_H

#include <string>
#include <QThread>

#include "statcollector.h"
#include "transport.h"

class SenderThread : public QThread
{
public:
    SenderThread(const char *ip, unsigned port,
                 transport &T,
                 StatCollector &stat);

    void Kill() { killed = true; }
protected:
    void run();
private:
    std::string ip;
    unsigned port;
    transport &T;
    StatCollector &stat;
    bool killed;
};

#endif // THREAD_SEND_H
