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
//==============================================================================================================
int process_decode(int argc, char *argv[])
{
    message_queue input_que(open_or_create, TO_DECODE_MSG, NUM_OF_PKGS, PKG_MAX_SIZE);
    message_queue output_que(open_or_create, TO_OUT_MSG, NUM_OF_PKGS, PKG_MAX_SIZE);

    send_data msg;
    size_t buff_len = sizeof(msg);
    size_t recvd, msg_len, out_lnt;

    ecc coder(5, 4);
    unsigned int priority;

    cout<< "decode started.\n"; 

    while(1){

        input_que.receive((void*)&msg, buff_len, recvd, priority);
        cout<< "decode one pkg.\n";

        if(!msg.in_buff_lnt)
            return 0;

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

    return 0;
}
