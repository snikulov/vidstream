#ifndef RECEIVERTHREAD_H
#define RECEIVERTHREAD_H

#include <QThread>
#include <QMutex>

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
    HistoryElement &operator=(RestartBlock &&rst) {
        b = std::move(rst);
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
    Q_OBJECT

public:
    ReassemblerThread(uint8_t *buffer,
                   BlockHistory &history,
                   size_t &rst_block_count,
                   QMutex &history_mutex,
                   StatCollector &stat,
                   bool broken_channel);

    void Kill() { killed = true; }

    static constexpr uint8_t MAX_HISTORY_DIFF = 5;
signals:
    void frameReady();
protected:
    void run();
private:

    uint8_t *buffer;
    //char *mask;
    BlockHistory &history;
    size_t &rst_block_count;
    QMutex &history_mutex;
    StatCollector &stat;

    bool broken_channel;

    bool killed;
};

void ComposeJpeg(uint8_t *buffer, BlockHistory &history, size_t rst_block_count);

#endif // RECEIVERTHREAD_H
