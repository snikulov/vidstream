#include "thread_encode.h"

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

EncoderThread::EncoderThread(ecc &coder, size_t restart_block_cnt,
                             StatCollector &stat) :
    coder(coder),
    restart_block_cnt(restart_block_cnt),
    stat(stat)
{ }

void EncoderThread::run()
{

    message_queue input_que(open_or_create, TO_ENCODE_MSG, NUM_OF_PKGS, PKG_MAX_SIZE);
    message_queue output_que(open_or_create, TO_SEND_MSG, NUM_OF_PKGS, PKG_MAX_SIZE);

    send_data msg;
    size_t buff_len = sizeof(msg);
    cout<< "encode started.\n"; 

    size_t recvd, msg_len, out_lnt;
    unsigned priority;

    for (size_t cnt = 0; cnt < restart_block_cnt; cnt++) {

        boost::posix_time::ptime timeout = boost::posix_time::microsec_clock::universal_time() + boost::posix_time::milliseconds(100);
        if (!input_que.timed_receive((void*)&msg, buff_len, recvd, priority, timeout)) {
            return;
        }

        if(!msg.in_buff_lnt) {
            return;
        }

        msg_len = msg.in_buff_lnt + sizeof(size_t);// in_buff_lnt can be less than DATA_LNT

        if(recvd == msg_len){

            char* out_data = (char*)coder.encode(msg.in_buff, msg.in_buff_lnt, out_lnt);
            memcpy(msg.in_buff, out_data, out_lnt);// here  out_lnt == DATA_LEN if coder create parameter is 3
                                                   // and can be less for enother coder create parameter
            free(out_data);
            msg.in_buff_lnt = out_lnt;
            msg_len = out_lnt + sizeof(size_t);

            output_que.send((void*)&msg, msg_len, 0);
        }
        else
            cout<< "encode: wrong pkg.\n" << "recv = " << recvd << ", msg_len = " << msg_len << "\n";
    }

    cout << "encode quit\n";

    return;
}


