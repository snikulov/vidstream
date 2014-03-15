#include <errno.h>
#include "ecc.h"
#include "err.h"
#include "bshift.h"
#include "pthread.h"
#include "threaded_coder.h"
#include  "transport.h"
#include  <sys/types.h>
#include <sys/errno.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <boost/interprocess/ipc/message_queue.hpp>

using namespace boost::interprocess;
//==============================================================================================================
int main(int argc, char *argv[]){
    
    message_queue output_que(open_or_create, TO_ENCODE_MSG, NUM_OF_PKGS, PKG_MAX_SIZE);
    message_queue input_que(open_or_create, TO_OUT_MSG, NUM_OF_PKGS, PKG_MAX_SIZE);

    send_data msg;

    msg.in_buff_lnt = (DATA_LEN - 8) / 4; // because data : ecc ratio == 1:3

    for(int ii=0; ii < msg.in_buff_lnt; ii++)
        msg.in_buff[ii] = ii;
    
    size_t msg_len = msg.in_buff_lnt + sizeof(size_t);

    output_que.send((void*)&msg, msg_len, 0);
    msg_len = sizeof(msg);

    size_t recvd;
    unsigned priority;

    input_que.receive((void*)&msg, msg_len, recvd, priority);

    cout << "\nreceived: ";

    for(int ii=0; ii < msg.in_buff_lnt; ii++)
        cout << (int)msg.in_buff[ii] << " ";
    
    cout << "\nmask: ";    

    for(int ii = msg.in_buff_lnt; ii < msg.in_buff_lnt * 2; ii++)
        cout << (int)msg.in_buff[ii] << " ";


    msg.in_buff_lnt = 0; //сигнал остановки
    msg_len = sizeof(size_t);

    message_queue enc_que(open_or_create, TO_ENCODE_MSG, NUM_OF_PKGS, PKG_MAX_SIZE);
    enc_que.send((void*)&msg, msg_len, 0);

    message_queue send_que(open_or_create, TO_SEND_MSG, NUM_OF_PKGS, PKG_MAX_SIZE);
    send_que.send((void*)&msg, msg_len, 0);

    message_queue read_que(open_or_create, TO_READ_MSG, NUM_OF_PKGS, PKG_MAX_SIZE);
    read_que.send((void*)&msg, msg_len, 0);

    message_queue dec_que(open_or_create, TO_DECODE_MSG, NUM_OF_PKGS, PKG_MAX_SIZE);
    dec_que.send((void*)&msg, msg_len, 0);

    message_queue out_que(open_or_create, TO_OUT_MSG, NUM_OF_PKGS, PKG_MAX_SIZE);
    out_que.send((void*)&msg, msg_len, 0);

    message_queue::remove(TO_READ_MSG);
    message_queue::remove(TO_SEND_MSG);
    message_queue::remove(TO_ENCODE_MSG);
    message_queue::remove(TO_OUT_MSG);
    message_queue::remove(TO_DECODE_MSG);

    return 0;
}

