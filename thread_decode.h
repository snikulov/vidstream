#ifndef THREAD_DECODE_H
#define THREAD_DECODE_H

#include <QThread>

#include "ecc.h"
#include "statcollector.h"
#include "threaded_coder.h"
#include <boost/interprocess/ipc/message_queue.hpp>
namespace bipc = boost::interprocess;
struct DecodedBlock
{
    int data_len;
    std::vector<char> decoded_ok;
    uint8_t data[PKG_MAX_SIZE];
};

class DecoderThread : public QThread
{
public:
    DecoderThread(ecc &coder, StatCollector &stat);
    void GetRSTFromPack(bipc::message_queue &output_que);
    void Kill() { killed = true; }
protected:
    void run();
private:

    ecc &coder;
    StatCollector &stat;

    bool killed;
};

#endif // THREAD_DECODE_H
