#include "thread_send.h"

#include <errno.h>
#include "ecc.h"
#include "err.h"
#include "pthread.h"
#include "params.h"
#include "transport.h"

#include <sys/types.h>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <QDebug>

char* sLogFileName = "##send.log";
char slogstr[1000];
bool sLogFirstEnter = true;
bool sRawLogFirstEnter = true;

void slog(char* lstr)
{
    FILE* logfile;
    if (sLogFirstEnter){
        remove(sLogFileName);
        sLogFirstEnter = false;
    }
    logfile = fopen(sLogFileName, "a");
    fprintf(logfile,"%s \n",lstr);
    fclose(logfile);
}


using namespace boost::interprocess;

SenderThread::SenderThread(const char *ip, unsigned port,
                           transport &T,
                           StatCollector &stat) :
    ip(ip),
    port(port),
    T(T),
    stat(stat),
    killed(false)
{ }

void SenderThread::run()
{

    message_queue input_que(open_or_create, TO_SEND_MSG, NUM_OF_PKGS, PKG_MAX_SIZE);
    message_queue output_que(open_or_create, TO_READ_MSG, NUM_OF_PKGS, PKG_MAX_SIZE);

    std::unique_ptr<uint8_t[]> recv_buf(new uint8_t[PKG_MAX_SIZE]);
    std::unique_ptr<uint8_t[]> udp_buf(new uint8_t[1500]);

    size_t recvd = 0;
    unsigned priority;
    int RSTinPack = 0;
    while (!killed) {
        input_que.receive(recv_buf.get(), PKG_MAX_SIZE, recvd, priority);

        output_que.send(recv_buf.get(), recvd, 0);

    }

//    transport T_out;

//    while (!killed)
//    {
//        input_que.receive(recv_buf.get(), PKG_MAX_SIZE, recvd, priority);
//        if (RSTinPack<5){
//            for (int i; i<240; i++){
//                udp_buf[RSTinPack*240+i] = recv_buf[i];

//            }
//            RSTinPack++;
//            continue;
//        }
//        RSTinPack = 0;
//        T_out.send("127.0.0.1", 32000,(char*)&udp_buf[0], 1450); //отсылаем пакет
//    }


    return;
}

