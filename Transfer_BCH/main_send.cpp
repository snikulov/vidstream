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
int main(int argc, char *argv[]){

    //Using:
    //send IP_ADDRESS PORT_NUMBER
    //

    message_queue input_que(open_or_create, TO_SEND_MSG, NUM_OF_PKGS, PKG_MAX_SIZE);

    send_data msg;
    size_t buff_len = sizeof(msg);
    transport T;

//    union{
        char loc_buff[PKG_MAX_SIZE + 4]; // 4 bytes for start key
//	uint32_t key;
//    };

    uint32_t* pkey = (uint32_t*)loc_buff;

    memset((void*)loc_buff,0,PKG_MAX_SIZE);
    *pkey = PKG_START_KEY;
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
	    memset((void*)(loc_buff + 4), 0, PKG_MAX_SIZE - 4); // loc_buff+4 чтобы не стереть ключ вначале
            memcpy(loc_buff + 4, msg.in_buff, loc_size);
	    loc_size += 4;
        }
	
	
        if(loc_size){
            T.send(argv[1], atoi(argv[2]), loc_buff, loc_size); //отсылаем пакет
            cout<< "send one pkg.\n"; 
        }
    }

    return 0;
}

