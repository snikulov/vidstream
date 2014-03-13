#include "thread_read.h"

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

ReaderThread::ReaderThread(float err_percent,
                           transport &T, Transceiver &tcv,
                           size_t restart_block_cnt, StatCollector &stat) :
    err_percent(err_percent),
    T(T),
    tcv(tcv),
    restart_block_cnt(restart_block_cnt),
    stat(stat)
{ }

void ReaderThread::run()
{

    // Using:
    // read IP_ADDRESS ERRORS_PERCENT

    message_queue output_que(open_or_create, TO_DECODE_MSG, NUM_OF_PKGS, PKG_MAX_SIZE);
    //message_queue input_que(open_or_create, TO_OUT_MSG, NUM_OF_PKGS, PKG_MAX_SIZE);

    send_data msg;

    //transport P;
    //int port = P.get_free_port();
    //cout << "on port: " << port;

    size_t msg_len;

    //transport T(ip.c_str(), port);

    err E;
    int readed;
    cout<< " read started.\n"; 

    for (size_t cnt = 0; cnt < restart_block_cnt; cnt++) {
        msg.in_buff_lnt = DATA_LEN;

        //input_que.try_receive((void*)&msg, buff_len, recvd, priority);
        if(!msg.in_buff_lnt)
            return;

        //msg.in_buff_lnt = readed = T.read(msg.in_buff, msg.in_buff_lnt);
        uint8_t *ptr = tcv.Receive(10, msg.in_buff_lnt);
        if (!ptr) {
            break;
        }
        memcpy(msg.in_buff, ptr, msg.in_buff_lnt);
        readed = msg.in_buff_lnt;


        if (readed > 0) {

            E.add_err((uint8_t*)msg.in_buff, msg.in_buff_lnt, err_percent);

            msg_len = msg.in_buff_lnt + sizeof(size_t);
            output_que.send((void*)&msg, msg_len, 0);
        } else {
            break;
        }
    }

    cout << "read quit\n";
    
    return;
}

