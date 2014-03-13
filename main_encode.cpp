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
int process_encode(int argc, char *argv[]){

    message_queue input_que(open_or_create, TO_ENCODE_MSG, NUM_OF_PKGS, PKG_MAX_SIZE);
    message_queue output_que(open_or_create, TO_SEND_MSG, NUM_OF_PKGS, PKG_MAX_SIZE);

    send_data msg;
    size_t buff_len = sizeof(msg);
    ecc coder(5, 4);
    cout<< "encode started.\n"; 

    size_t recvd, msg_len, out_lnt;
    unsigned priority;

    while(1){

        input_que.receive((void*)&msg, buff_len, recvd, priority);
        cout<< "encode one pkg.\n";
        if(!msg.in_buff_lnt)
            return 0;

        msg_len = msg.in_buff_lnt + sizeof(size_t);// in_buff_lnt can be less than DATA_LNT

        if(recvd == msg_len){

            char* out_data = (char*)coder.encode(msg.in_buff, msg.in_buff_lnt, out_lnt);
            memcpy(msg.in_buff, out_data, out_lnt);// here  out_lnt == DATA_LEN if coder create parameter is 3
                                                   // and can be less for enother coder create parameter
            free(out_data);
            msg.in_buff_lnt = out_lnt;
            msg_len = out_lnt + sizeof(size_t);

            output_que.send((void*)&msg, msg_len, 0);
            cout<< "to sent encoded pkg.\n";
        }
        else
            cout<< "wrong pkg.\n";
    }

    return 0;
}


