#include "thread_decode.h"

#include <errno.h>
#include "ecc.h"
#include "err.h"
#include "pthread.h"
#include "params.h"
#include "transport.h"
#include <sys/types.h>
#include <sys/errno.h>
#include <sys/msg.h>
#include <sys/ipc.h>

#include <boost/interprocess/ipc/message_queue.hpp>

using namespace boost::interprocess;
DecoderThread::DecoderThread(ecc *coder,
                             StatCollector &stat) :
    coder(coder),
    stat(stat),
    killed(false),
   BCHParametersChanged(false)
{ }
void DecoderThread::RecreateCoder(int bch_m, int bch_t)
{
   BCHParametersChanged = true;
   this->bch_m = bch_m;
   this->bch_t = bch_t;
   BCHParametersChanged = true;
}

void DecoderThread::run()
{
    message_queue input_que(open_or_create, TO_DECODE_MSG, NUM_OF_PKGS, PKG_MAX_SIZE);
    message_queue output_que(open_or_create, TO_OUT_MSG, NUM_OF_PKGS, PKG_MAX_SIZE);

    std::unique_ptr<uint8_t[]> recv_buf(new uint8_t[PKG_MAX_SIZE]);
    DecodedBlock send_buf;
    size_t recvd, out_lnt;
    unsigned int priority;
    std::vector<char> decoded;
    int rst_len;
    while (!killed) {
        if (BCHParametersChanged){
            delete coder;
            coder = new ecc(bch_m, bch_t, &stat);
            BCHParametersChanged = false;
        }
        input_que.receive(recv_buf.get(), PKG_MAX_SIZE, recvd, priority);

        char* out_data = (char*)coder->decode((char *)(recv_buf.get()), recvd,
                                             out_lnt, decoded, send_buf.decoded_ok);
        send_buf.data_len = out_lnt;
        memcpy(send_buf.data, out_data, out_lnt);
        free(out_data);
        rst_len = send_buf.data[4]+2;
        stat.AddPacket(rst_len, send_buf.decoded_ok);

        output_que.send(&send_buf, out_lnt, 0);
    }

    return;
}
