#include "thread_decode.h"

#include <errno.h>
#include "ecc.h"
#include "err.h"
#include "pthread.h"
#include "threaded_coder.h"
#include "transport.h"
#include <sys/types.h>
#include <sys/errno.h>
#include <sys/msg.h>
#include <sys/ipc.h>

#include <boost/interprocess/ipc/message_queue.hpp>

using namespace boost::interprocess;

DecoderThread::DecoderThread(ecc &coder, size_t restart_block_cnt,
                             StatCollector &stat) :
    coder(coder),
    restart_block_cnt(restart_block_cnt),
    stat(stat)
{ }

void DecoderThread::run()
{
    message_queue input_que(open_or_create, TO_DECODE_MSG, NUM_OF_PKGS, PKG_MAX_SIZE);
    message_queue output_que(open_or_create, TO_OUT_MSG, NUM_OF_PKGS, PKG_MAX_SIZE);

    send_data msg;
    size_t buff_len = sizeof(msg);
    size_t recvd, msg_len, out_lnt;

    unsigned int priority;

    cout<< "decode started.\n"; 

    for (size_t cnt = 0; cnt < restart_block_cnt; cnt++) {

        boost::posix_time::ptime timeout = boost::posix_time::microsec_clock::universal_time() + boost::posix_time::milliseconds(100);
        if (!input_que.timed_receive((void*)&msg, buff_len, recvd, priority, timeout)) {
            return;
        }

        if(!msg.in_buff_lnt)
            return;

        msg_len = msg.in_buff_lnt + sizeof(size_t);

        if(recvd == msg_len){
            char* out_data = (char*)coder.decode(msg.in_buff, msg.in_buff_lnt, out_lnt);

            memcpy(msg.in_buff, out_data, out_lnt*2);
            msg.in_buff_lnt = out_lnt;
            free(out_data);

            msg.in_buff_lnt = out_lnt;
            msg_len = out_lnt*2 + sizeof(size_t);

            output_que.send((void*)&msg, msg_len, 0);
        }
        else
            cout<< "wrong pkg.\n";
    }

    cout << "decode quit\n";

    return;
}
