#include "thread_packetize.h"

#include <cassert>
#include <cmath>
#include <fstream>

#include <QDebug>

#include <params.h>
#include <boost/interprocess/ipc/message_queue.hpp>

#include "interlace.h"
#include "split.h"

namespace bipc = boost::interprocess;

PacketizerThread::PacketizerThread(const uint8_t *buffer, const size_t buffer_size,
                                   uint8_t frame_number, size_t restart_block_cnt,
                                   StatCollector &stat,
                                   const InterlaceControl &interlace) :
    buffer(buffer),
    buffer_size(buffer_size),
    frame_number(frame_number),
    restart_block_cnt(restart_block_cnt),
    stat(stat),
    interlace(interlace)
{
}

void PacketizerThread::TransmitBlock(RestartBlock& block,
                                    bipc::message_queue &mq)
{
    mq.send(block.raw_ptr(), block.raw_length(), 0);
}

void PacketizerThread::run()
{
    bipc::message_queue mq(bipc::open_or_create, TO_ENCODE_MSG, NUM_OF_PKGS, PKG_MAX_SIZE);
    // c - current char, p - previous char
    uint8_t p = 0, c;
    uint16_t rst_cnt = 0;
    RestartBlock block;
    for (size_t i = 0; i < buffer_size; i++) {
        c = buffer[i];
        // section can only contain FF 00 and RST markers
        if (p == 0xFF) { // && (c == 0x00 || is_rst(c))) {
            if (is_rst(c)) {
                // send buffer
                if (interlace_refresh_block(rst_cnt, interlace)) {
                    block.set_info(frame_number, rst_cnt, block.pushbacks_count());
                    TransmitBlock(block, mq);
                }
                block.clear();
                rst_cnt++;
            } else { // {p, c} = FF00; ignoring 00
                block.push_back(p);
            }
        } else { // p != 0xFF
            if (c != 0xFF) { // certainly not a marker
                block.push_back(c);
            }
        }
        p = c;
    }

    if (block.pushbacks_count() > 0) {
        block.set_info(frame_number, rst_cnt,block.pushbacks_count());
        TransmitBlock(block, mq);
    }
}

