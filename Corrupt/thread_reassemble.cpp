#include "thread_reassemble.h"

#include "corrupt.h"
#include "membuf.h"
#include "split.h"
#include "threaded_coder.h"

#include <QDebug>
#include <boost/interprocess/ipc/message_queue.hpp>

namespace bipc = boost::interprocess;

ReassemblerThread::ReassemblerThread(uint8_t *buffer, char *mask,
                               BlockHistory &history,
                               size_t restart_block_cnt,
                               StatCollector &stat,
                               bool broken_channel) :
    buffer(buffer),
    mask(mask),
    history(history),
    restart_block_cnt(restart_block_cnt),
    stat(stat),
    broken_channel(broken_channel)
{
}

void ReassemblerThread::run()
{
    bipc::message_queue mq(bipc::open_or_create, TO_OUT_MSG, NUM_OF_PKGS, PKG_MAX_SIZE);
    uint8_t *ptr;
    size_t recv_size, decoded_size;
    stat.StartFrame();
    std::unique_ptr<uint8_t[]> recv_buf(new uint8_t[PKG_MAX_SIZE]);
    unsigned priority;
    // receive a block if it comes before the timeout of 10 ms elapses
    // put all received and successfully decoded blocks in history
    for (size_t cnt = 0; cnt < restart_block_cnt; cnt++) {
        //boost::posix_time::ptime timeout = boost::posix_time::microsec_clock::universal_time() + boost::posix_time::milliseconds(100);
        //if (!mq.timed_receive(recv_buf.get(), PKG_MAX_SIZE, recv_size, priority, timeout)) {
        //    return;
        //}
        mq.receive(recv_buf.get(), PKG_MAX_SIZE, recv_size, priority);
        ptr = recv_buf.get();
        decoded_size = recv_size;
        // TODO: receive decoded_ok from decoder thread
        bool decoded_ok = true;

        // add errors to the whole RestartBlock in recv buffer
        if (broken_channel) {
            memset(ptr, 0, decoded_size);
        }

        if (RestartBlock::get_rst_block_number(ptr) > history.size() ||
            RestartBlock::get_data_length(ptr) > decoded_size - RestartBlock::get_info_len()) {
            continue;
        }
        HistoryElement &hsblock = history[RestartBlock::get_rst_block_number(ptr)];
        mask[RestartBlock::get_rst_block_number(ptr)] = hsblock.get_age() <= MAX_HISTORY_DIFF;

        //replace hsblock with new block if new block is good or hsblock is old
        if (decoded_ok || hsblock.get_age() > MAX_HISTORY_DIFF) {
            hsblock = RestartBlock(RestartBlock::get_data_ptr(ptr),
                                   RestartBlock::get_data_length(ptr));
            hsblock.get_b().set_info(RestartBlock::get_frame_number(ptr),
                                     RestartBlock::get_rst_block_number(ptr),
                                     RestartBlock::get_data_length(ptr));
            mask[RestartBlock::get_rst_block_number(ptr)] = true;
        }
    }

    ComposeJpeg();

    stat.FinishFrame();
}

void ReassemblerThread::ComposeJpeg()
{
    uint8_t *base = buffer;
    for (size_t cur_iteration = 0; cur_iteration < restart_block_cnt; ) {

        // add RST marker before each block, except the first one
        if (cur_iteration) {
            *base = 0xFF;
            base++;
            *base = Markers::RST + (cur_iteration - 1) % 8;
            base++;
        }

        HistoryElement &hsblock = history[cur_iteration];
        if (hsblock.get_age() > MAX_HISTORY_DIFF) {
            hsblock.clear();
        }
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
        cur_iteration++;
    }
    *base = 0xFF;
    base++;
    *base = 0xD9;
}
