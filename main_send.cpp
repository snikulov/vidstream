#include <errno.h>
#include "ecc.h"
#include "err.h"
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
int process_send(int argc, char *argv[]){

    //Using:
    //send IP_ADDRESS PORT_NUMBER
    //

    message_queue input_que(open_or_create, TO_SEND_MSG, NUM_OF_PKGS, PKG_MAX_SIZE);

    send_data msg;
    size_t buff_len = sizeof(msg);
    transport T;
    char loc_buff[DATA_LEN];
    memset((void*)loc_buff,0,DATA_LEN);
    size_t  loc_size = 0;

    size_t recvd = 0;
    unsigned priority;


    cout<< "send started.\n"; 

    while(1){

        //input_que.try_receive((void*)&msg, buff_len, recvd, priority); // посылаем пакет непрерывно до прихода следующего
        input_que.receive((void*)&msg, buff_len, recvd, priority); // посылаем пакет один раз и ждем прихода следующего

        if(recvd){
            if(!msg.in_buff_lnt)
                return 0;

            loc_size = msg.in_buff_lnt;
            memcpy(loc_buff, msg.in_buff, loc_size);
        }
	
	
        if(loc_size){
            T.send(argv[1], atoi(argv[2]), loc_buff, loc_size); //отсылаем пакет
            cout<< "send one pkg.\n"; 
        }
    }

    return 0;
}

