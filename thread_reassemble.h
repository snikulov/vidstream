#ifndef RECEIVERTHREAD_H
#define RECEIVERTHREAD_H

#include <QThread>

#include <utility>
#include <vector>

#include "ecc.h"
#include "restartblock.h"
#include "statcollector.h"
#include "transceiver.h"

class HistoryElement
{
public:
    HistoryElement() : age(-1) { }
    HistoryElement(const RestartBlock &rst) : b(rst), age(0) { }
    HistoryElement &operator=(const RestartBlock &rst) {
        b = rst;
        age = 0;
        return *this;
    }
    void clear() {
        age = -1; // maximum size_t value
        b.clear();
    }
    size_t get_age() const { return age;               }
    void increase_age()    { if (age + 1 > age) age++; }
    RestartBlock& get_b()  { return b;                 }
private:
    RestartBlock b;
    size_t age;
};

typedef std::vector<HistoryElement> BlockHistory;

class ReassemblerThread : public QThread
{
public:
    ReassemblerThread(uint8_t *buffer, char *mask,
                   Transceiver &t, ecc &encoder, BlockHistory &history,
                   size_t restart_block_cnt,
                   StatCollector &stat,
                   float err_percent,
                   bool broken_channel);

protected:
    void run();
private:
    static constexpr uint8_t MAX_HISTORY_DIFF = 5;

    uint8_t *buffer;
    char *mask;
    Transceiver &t;
    ecc &encoder;
    BlockHistory &history;
    size_t restart_block_cnt;
    StatCollector &stat;
    float err_percent;

    bool broken_channel;

    void ComposeJpeg();
};

#endif // RECEIVERTHREAD_H
