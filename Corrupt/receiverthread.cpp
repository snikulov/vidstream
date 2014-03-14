#include "receiverthread.h"

#include "corrupt.h"
#include "membuf.h"
#include "split.h"

#include <QDebug>

ReceiverThread::ReceiverThread(uint8_t *buffer, char *mask,
                               Transceiver &t, ecc &encoder, BlockHistory &history,
                               size_t restart_block_cnt,
                               StatCollector &stat,
                               float err_percent,
                               bool broken_channel) :
    buffer(buffer),
    mask(mask),
    t(t),
    encoder(encoder),
    history(history),
    restart_block_cnt(restart_block_cnt),
    stat(stat),
    err_percent(err_percent),
    killed(false),
    broken_channel(broken_channel)
{
}

void ReceiverThread::run()
{
    uint8_t *recv, *ptr;
    size_t recv_size, decoded_size;
    stat.StartFrame();
    // receive a block if it comes before the timeout of 10 ms elapses
    // put all received and successfully decoded blocks in history
    while (!killed && (recv = t.Receive(10, recv_size))) {
        uint8_t *encoded_ptr = RestartBlock::get_data_ptr(recv);
        size_t encoded_len = recv_size - RestartBlock::get_info_len();

        // add errors to the whole RestartBlock in recv buffer
        if (broken_channel) {
            memset(encoded_ptr, 0, encoded_len);
        } else {
            Corruptor::add_err(encoded_ptr, encoded_len, err_percent);
        }

        bool decoded_ok;
        stat.StartTimer(StatCollector::TIMER_DECODE);
        ptr = (uint8_t *) encoder.decode((char *) encoded_ptr, encoded_len, decoded_size,
                                         decoded_ok);
        stat.StopTimer(StatCollector::TIMER_DECODE);
        stat.AddPacket(encoded_len, decoded_ok);

        if (RestartBlock::get_rst_block_number(ptr) > history.size() ||
            RestartBlock::get_data_length(ptr) > decoded_size - RestartBlock::get_info_len()) {
            free(recv);
            free(ptr);
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
        free(recv);
        free(ptr);
    }

    ComposeJpeg();
    stat.FinishFrame();
}

void ReceiverThread::ComposeJpeg()
{
    uint8_t *base = buffer;
    for (size_t cur_iteration = 0; cur_iteration < restart_block_cnt;
         ) {

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
