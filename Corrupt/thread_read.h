#ifndef THREAD_READ_H
#define THREAD_READ_H

#include <string>
#include <QThread>

#include "statcollector.h"
#include "transport.h"

class ReaderThread : public QThread
{
public:
    ReaderThread(float err_percent, transport &T,
                 StatCollector &stat);

    void SetErrPercent(float p)   { err_percent = p; }
    void SetBrokenChannel(bool f) { broken_channel = f; }
protected:
    void run();
private:
    bool broken_channel;
    float err_percent;
    transport &T;
    StatCollector &stat;
};

#endif // THREAD_READ_H
