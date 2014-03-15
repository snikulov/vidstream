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
#include <stdlib.h>   
#include <time.h>      
#include <bshift.h>      

using namespace boost::interprocess;
//==============================================================================================================
int main(int argc, char *argv[]){

    // Using:
    // read IP_ADDRESS ERRORS_PERCENT

    message_queue output_que(open_or_create, TO_DECODE_MSG, NUM_OF_PKGS, PKG_MAX_SIZE);
    message_queue input_que(open_or_create, TO_OUT_MSG, NUM_OF_PKGS, PKG_MAX_SIZE);

    send_data msg;

    srand (time(NULL));
    uint8_t shift = 0;

    transport P;
    int udp_port = P.get_free_port();
    cout << "on port: " << udp_port;

    size_t msg_len;
    size_t buff_len = sizeof(msg);

    transport T(argv[1], udp_port);

    err E;
    size_t recvd, out_lnt;
    unsigned priority;
    int readed;

    uint32_t key = __builtin_bswap32(PKG_START_KEY);

    srand (time(NULL));
    cout<< " read started.\n"; 

    uint32_t loc_size = PKG_MAX_SIZE + 9;
    uint8_t loc_buff[PKG_MAX_SIZE + 9]; // PKG_MAX_SIZE bytes for data, 4 bytes for start key and 5 bytes for "random" data shift

    while(1){
        msg.in_buff_lnt = DATA_LEN;

        input_que.try_receive((void*)&msg, buff_len, recvd, priority);
        if(!msg.in_buff_lnt)
            return 0;

	memset((void*)loc_buff, 0, loc_size);
	readed = T.read((char*)loc_buff, loc_size);
	
        if( readed > 4){
		cout<< readed << " bytes read.\n"; 
		
	        shift = rand() % 40; //случайный сдвиг от 0 до 40 бит
        	shift_array(loc_buff, readed, shift, SHIFT_RIGTH);
		
		cout<< (int)shift << " bits shifted.\n"; 
		shift = shift/8 + 1;

		E.add_err(loc_buff, readed + shift, atof(argv[2]));

		shift = find_shift(key, loc_buff, 10);
		cout<< (int)shift << " bits shift found.\n"; 
        	shift_array(loc_buff, readed, shift, SHIFT_LEFT);
		
	        msg.in_buff_lnt = readed - 4;
		memcpy((void*)msg.in_buff, (void*)(loc_buff + 4), msg.in_buff_lnt);
		msg_len = msg.in_buff_lnt + sizeof(size_t);
		output_que.send((void*)&msg, msg_len, 0);
	}
    }
    
    return 0;
}

