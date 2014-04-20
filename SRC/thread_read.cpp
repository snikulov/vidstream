#include "thread_read.h"

#include "ecc.h"
//#include "err.h"
#include "corrupt.h"
#include "pthread.h"
#include "params.h"
#include "transport.h"
#include "commn_vt.h"

#include <errno.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <boost/interprocess/ipc/message_queue.hpp>

#include <iostream>

#include <QDebug>

using namespace boost::interprocess;

char* rdLogFileName = "##read.log";
char rdLogstr[1000];
bool rdLogFirstEnter = true;
FILE* rdlogfile;

void rdLog(char* lstr)
{


    if (rdLogFirstEnter){
        remove(rdLogFileName);
        rdLogFirstEnter = false;
        rdlogfile = fopen(rdLogFileName, "a");
    }
    fprintf(rdlogfile,"%s \n",lstr);
}



ReaderThread::ReaderThread(float err_percent,
                           transport &T,
                           StatCollector &stat) :
    broken_channel(false),
    err_percent(err_percent),
    T(T),
    stat(stat)
{ }

void ReaderThread::run()
{
    using std::cout;
    using std::flush;

    // Using:
    // read IP_ADDRESS ERRORS_PERCENT

    message_queue output_que(open_or_create, TO_DECODE_MSG, NUM_OF_PKGS, PKG_MAX_SIZE);
    message_queue input_que(open_or_create, TO_READ_MSG, NUM_OF_PKGS, PKG_MAX_SIZE);

    std::unique_ptr<uint8_t[]> recv_buf(new uint8_t[PKG_MAX_SIZE]);
    uint8_t udp_buf[2500];
    size_t recvd = 0;
    unsigned priority;

    int port = 32000;
    int readed;
    size_t  loc_size = 0;
//    transport T_inp("127.0.0.1", port);
//    while (1) {
//        //change to TCP        input_que.receive(recv_buf.get(), PKG_MAX_SIZE, recvd, priority);
//        readed = T_inp.read((char*)&udp_buf[0], 1500);
//        cout<<"readed "<<readed<<flush;
//        if (readed<=0)
//        {
//            continue;
//        }

//        for (int RSTinPack=0; RSTinPack<5; RSTinPack++){
//            for (int i; i<240; i++){
//                recv_buf[i] = udp_buf[RSTinPack*240+i];
//            }
//            recvd = PKG_MAX_SIZE;
//            if (!broken_channel) {
//                Corruptor::add_err(recv_buf.get(), recvd, err_percent);
//            } else {
//                memset(recv_buf.get(), 0, recvd);
//            }
//            output_que.send(recv_buf.get(), PKG_MAX_SIZE, 0);

//        }
//        recvd = readed;

//    }



    while (1) {
 //       sprintf(rdLogstr,"Normal mode: num in queue %d %d",input_que.get_num_msg(), FlushReadQ);
//        rdLog(rdLogstr);

        if (FlushReadQ){
            while (input_que.get_num_msg()>0){
                sprintf(rdLogstr,"Flush num in queue %d",input_que.get_num_msg());
                rdLog(rdLogstr);
                input_que.receive(recv_buf.get(), PKG_MAX_SIZE, recvd, priority);
                sprintf(rdLogstr,"num in queue %d",input_que.get_num_msg());
                rdLog(rdLogstr);
            }
            FlushReadQ = false;
        }
        //msg.in_buff_lnt = readed = T.read(msg.in_buff, msg.in_buff_lnt);
        input_que.receive(recv_buf.get(), PKG_MAX_SIZE, recvd, priority);

        if (!broken_channel) {
            Corruptor::add_err(recv_buf.get(), recvd, err_percent);
        } else {
            memset(recv_buf.get(), 0, recvd);
        }

        output_que.send(recv_buf.get(), PKG_MAX_SIZE, 0);
    }


    
    return;
}

