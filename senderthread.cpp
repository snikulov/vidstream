#include "senderthread.h"

#include <cassert>
#include <cmath>
#include <fstream>

#include <QDebug>

#include <boost/interprocess/ipc/message_queue.hpp>

#include "interlace.h"
#include "split.h"
#include "queue_params.h"

namespace bipc = boost::interprocess;

SenderThread::SenderThread(const uint8_t *buffer, const size_t buffer_size,
                           std::string queue_name,
                           ecc &encoder, uint8_t frame_number,
                           StatCollector &stat,
                           const InterlaceControl &interlace) :
    buffer(buffer),
    buffer_size(buffer_size),
    queue_name(queue_name),
    encoder(encoder),
    frame_number(frame_number),
    stat(stat),
    interlace(interlace)
{
}

int SenderThread::TransmitBlock(RestartBlock& block, bipc::message_queue &mq)
{
    size_t encoded_len = 0;
    stat.StartTimer(StatCollector::TIMER_ENCODE);
    uint8_t *encoded_ptr = (uint8_t *) encoder.encode((char *) block.raw_ptr(),
                                                      block.raw_length(), encoded_len);
    stat.StopTimer(StatCollector::TIMER_ENCODE);

    mq.send(encoded_ptr, encoded_len, 0);
    free(encoded_ptr);
    return 1;
}

void SenderThread::run()
{
    bipc::message_queue mq(bipc::open_or_create, queue_name.c_str(),
                           package_num, package_max_size);

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
                    if (!TransmitBlock(block, mq)) {
                        return;
                    }
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

    // send remaining data
    if (block.pushbacks_count() > 0 && interlace_refresh_block(rst_cnt, interlace)) {
        block.set_info(frame_number, rst_cnt, block.pushbacks_count());
        if (!TransmitBlock(block, mq)) {
            return;
        }
    }
}

