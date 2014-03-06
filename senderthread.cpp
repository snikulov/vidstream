#include "senderthread.h"

#include <cassert>
#include <cmath>
#include <fstream>

#include "interlace.h"
#include "split.h"

SenderThread::SenderThread(const uint8_t *buffer, const size_t buffer_size,
                           Transceiver &t, ecc &encoder, uint8_t frame_number,
                           StatCollector &stat,
                           const InterlaceControl &interlace,
                           bool broken_channel) :
    buffer(buffer),
    buffer_size(buffer_size),
    t(t),
    encoder(encoder),
    frame_number(frame_number),
    stat(stat),
    interlace(interlace),
    broken_channel(broken_channel)
{
}

int SenderThread::TransmitBlock(RestartBlock& block, uint8_t frame_number,
                                uint16_t rst_number, uint16_t data_len)
{
    block.set_info(frame_number, rst_number, data_len);

    size_t encoded_len = 0;
    stat.StartTimer(StatCollector::TIMER_ENCODE);
    uint8_t *encoded_ptr = (uint8_t *) encoder.encode((char *) block.raw_ptr(),
                                                      block.raw_length(), encoded_len);
    stat.StopTimer(StatCollector::TIMER_ENCODE);
    //uint8_t *encoded_ptr = block.raw_ptr();
    //encoded_len = block.raw_length();

    size_t res_len = encoded_len + block.get_info_len();
    uint8_t *res_ptr = (uint8_t *) malloc(res_len);
    memcpy(res_ptr, block.raw_ptr(), block.get_info_len());
    if (!broken_channel) {
        memcpy(RestartBlock::get_data_ptr(res_ptr), encoded_ptr, encoded_len);
    } else {
        memset(RestartBlock::get_data_ptr(res_ptr), 0, encoded_len);
    }
    free(encoded_ptr);
    if (!t.Transmit(res_ptr, res_len)) {
        fprintf(stderr, "Failed to send data chunk\n");
        free(res_ptr);
        return 0;
    }
    free(res_ptr);
    return 1;
}

void SenderThread::run()
{
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
                if (interlace_refresh_block(rst_cnt, interlace)
                    && !TransmitBlock(block, frame_number, rst_cnt, block.pushbacks_count())) {
                    // can't send blocks, aborting
                    return;
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
    if (block.pushbacks_count() > 0) {
        if (!TransmitBlock(block, 0, rst_cnt, block.pushbacks_count())) {//block.pushbacks_count())) {
            // a serious error happened, aborting
            return;
        }
    }
}

