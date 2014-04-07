#include "thread_reassemble.h"

#include "corrupt.h"
#include "membuf.h"
#include "split.h"
#include "thread_decode.h"
#include "threaded_coder.h"

#include <QDebug>
#include <QMessageBox>
#include <boost/interprocess/ipc/message_queue.hpp>

namespace bipc = boost::interprocess;

ReassemblerThread::ReassemblerThread(BlockHistory &history,
                               size_t &rst_block_count,
                               QMutex &history_mutex,
                               StatCollector &stat,
                               bool broken_channel) :
    history(history),
    rst_block_count(rst_block_count),
    history_mutex(history_mutex),
    stat(stat),
    broken_channel(broken_channel),
    killed(false)
{
}

void ReassemblerThread::run()
{
    history_mutex.lock();
    bipc::message_queue mq(bipc::open_or_create, TO_OUT_MSG, NUM_OF_PKGS, PKG_MAX_SIZE);
    DecodedBlock recv_block;
    uint8_t *ptr;
    size_t recv_size, decoded_size;
    uint8_t prev_frame_number = 0;
    size_t time_since_last_frame = 0;
    stat.StartFrame();
    std::unique_ptr<uint8_t[]> recv_buf(new uint8_t[PKG_MAX_SIZE]);
    unsigned priority;
    // receive a block
    // put all received and successfully decoded blocks in history
    while (!killed) {
        mq.receive(&recv_block, PKG_MAX_SIZE, recv_size, priority);
        ptr = recv_block.data;
        decoded_size = recv_block.data_len;
        //bool decoded_ok = recv_block.decoded_ok;
        bool decoded_ok = true;

        if (broken_channel) {
            memset(ptr, 0, decoded_size);
        }

        if (RestartBlock::get_rst_block_number(ptr) > rst_block_count) {
            continue;
        }
        if (RestartBlock::get_data_length(ptr) > decoded_size - RestartBlock::get_info_len()) {
            continue;
        }
        HistoryElement &hsblock = history[RestartBlock::get_rst_block_number(ptr)];
        //mask[RestartBlock::get_rst_block_number(ptr)] = hsblock.get_age() <= MAX_HISTORY_DIFF;

        //replace hsblock with new block if new block is good or hsblock is old
        if (decoded_ok || hsblock.get_age() > MAX_HISTORY_DIFF) {
            hsblock = RestartBlock(RestartBlock::get_data_ptr(ptr),
                                   RestartBlock::get_data_length(ptr));
            hsblock.get_b().set_info(RestartBlock::get_frame_number(ptr),
                                     RestartBlock::get_rst_block_number(ptr),
                                     RestartBlock::get_data_length(ptr));
            //mask[RestartBlock::get_rst_block_number(ptr)] = true;
        }
        time_since_last_frame++;
        if (RestartBlock::get_frame_number(ptr) != prev_frame_number &&
            time_since_last_frame >= rst_block_count / 2) {
            stat.FinishFrame();
            emit frameReady(); // unlocks history_mutex when finishes
            history_mutex.lock();
            stat.StartFrame();
            prev_frame_number = RestartBlock::get_frame_number(ptr);
            time_since_last_frame = 0;
        }
    }
}

void ComposeJpeg(uint8_t *buffer, BlockHistory &history, size_t rst_number)
{
    uint8_t *base = buffer;
    for (size_t cur_iteration = 0; cur_iteration < rst_number;
         cur_iteration++) {
        // add RST marker before each block, except the first one
        if (cur_iteration) {
            *base = 0xFF;
            base++;
            *base = Markers::RST + (cur_iteration - 1) % 8;
            base++;
        }

        HistoryElement &hsblock = history[cur_iteration];
        if (hsblock.get_age() <= ReassemblerThread::MAX_HISTORY_DIFF) {
            hsblock.increase_age();

            // skip the info part
            size_t len = hsblock.get_b().data_length();
            // encoded data contains block header as well
            uint8_t *data = hsblock.get_b().data_ptr();
            // copy hsblock contents to output buffer
            // restore the 0x00s after 0xFF
            for (unsigned i = 0; i < len; i++) {
                *base = data[i];
                base++;
                if (data[i] == 0xFF) {
                    *base = 0x00;
                    base++;
                }
            }
        }
    }
    *base = 0xFF;
    base++;
    *base = 0xD9;
}
