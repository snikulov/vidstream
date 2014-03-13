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
int process_read(int argc, char *argv[]){

    // Using:
    // read IP_ADDRESS ERRORS_PERCENT

    message_queue output_que(open_or_create, TO_DECODE_MSG, NUM_OF_PKGS, PKG_MAX_SIZE);
    message_queue input_que(open_or_create, TO_OUT_MSG, NUM_OF_PKGS, PKG_MAX_SIZE);

    send_data msg;

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
    cout<< " read started.\n"; 

    while(1){
        msg.in_buff_lnt = DATA_LEN;

        input_que.try_receive((void*)&msg, buff_len, recvd, priority);
        if(!msg.in_buff_lnt)
            return 0;

	msg.in_buff_lnt = readed = T.read(msg.in_buff, msg.in_buff_lnt);

        if( readed > 0){
		cout<< readed << " bytes read.\n"; 
		
		E.add_err((uint8_t*)msg.in_buff, msg.in_buff_lnt, atof(argv[2]));

		msg_len = msg.in_buff_lnt + sizeof(size_t);
		output_que.send((void*)&msg, msg_len, 0);
	}
    }
    
    return 0;
}

