#ifndef THREAD_DECODE_H
#define THREAD_DECODE_H

#include <QThread>

#include "ecc.h"
#include "statcollector.h"
#include "params.h"

struct DecodedBlock
{
    int data_len;
    bool decoded_ok;
    uint8_t data[PKG_MAX_SIZE];
};

class DecoderThread : public QThread
{
public:
    DecoderThread(ecc *coder, StatCollector &stat);
    void RecreateCoder(int bch_t, int bch_m);
    void Kill() { killed = true; }
protected:
    void run();
private:
    int bch_m, bch_t;
    bool BCHParametersChanged;
    ecc *coder;
    StatCollector &stat;

    bool killed;
};

#endif // THREAD_DECODE_H
