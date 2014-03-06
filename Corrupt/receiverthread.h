#ifndef RECEIVERTHREAD_H
#define RECEIVERTHREAD_H

#include <QThread>

#include <utility>
#include <vector>

#include "ecc.h"
#include "restartblock.h"
#include "statcollector.h"
#include "transceiver.h"

typedef std::vector<RestartBlock> BlockHistory;

class ReceiverThread : public QThread
{
public:
    ReceiverThread(uint8_t *buffer, char *mask,
                   Transceiver &t, ecc &encoder, BlockHistory &history,
                   size_t restart_block_cnt,
                   StatCollector &stat,
                   float err_percent);

    void Kill()       { killed = true; }
    void ResetState() { killed = false; }
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

    bool killed;

    void ComposeJpeg();
};

#endif // RECEIVERTHREAD_H
