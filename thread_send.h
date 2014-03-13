#ifndef THREAD_SEND_H
#define THREAD_SEND_H

#include <QThread>

#include "statcollector.h"

class SenderThread : public QThread
{
public:
    SenderThread(const char *ip, unsigned port, StatCollector &stat);

protected:
    void run();
private:
    StatCollector &stat;
};

#endif // THREAD_SEND_H
