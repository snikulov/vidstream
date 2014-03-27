#ifndef THREAD_ENCODE_H
#define THREAD_ENCODE_H

#include <QThread>

#include "ecc.h"
#include "statcollector.h"

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
