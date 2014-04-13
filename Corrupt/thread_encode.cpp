#include "thread_encode.h"

#include "ecc.h"
#include "err.h"
#include "pthread.h"
#include "threaded_coder.h"
#include "transport.h"

#include <errno.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <boost/interprocess/ipc/message_queue.hpp>

#include <QDebug>

uint32_t StartTime = 0;
uint64_t SendBytes = 0;
struct timespec start_time, cur_time;
uint32_t ChannelSpeed = 1000000000/8;

int64_t diff_time(struct timespec *timeA_p, struct timespec *timeB_p)
{
    clock_gettime(CLOCK_MONOTONIC, timeA_p);
    return (((timeA_p->tv_sec * 1000000000) + timeA_p->tv_nsec) -
           ((timeB_p->tv_sec * 1000000000) + timeB_p->tv_nsec)) /1000 ;
}

void shape_channel()
{

    if (  diff_time(&cur_time,&start_time)  > 0 ) {

       while  ( SendBytes*1000000/diff_time(&cur_time,&start_time) > ChannelSpeed)
       {
          usleep(1000);
       }

    }
}

using namespace boost::interprocess;

EncoderThread::EncoderThread(ecc &coder,
                             StatCollector &stat) :
    coder(coder),
    stat(stat),
    killed(false)
{ }

void EncoderThread::run()
{

    message_queue input_que(open_or_create, TO_ENCODE_MSG, NUM_OF_PKGS, PKG_MAX_SIZE);
    message_queue output_que(open_or_create, TO_SEND_MSG, NUM_OF_PKGS, PKG_MAX_SIZE);

    std::unique_ptr<uint8_t[]> recv_buf(new uint8_t[PKG_MAX_SIZE]);
    std::unique_ptr<uint8_t[]> send_buf(new uint8_t[PKG_MAX_SIZE]);
    size_t recvd, out_lnt;
    unsigned priority;

    while (!killed) {

        input_que.receive(recv_buf.get(), PKG_MAX_SIZE, recvd, priority);
        //boost::posix_time::ptime timeout = boost::posix_time::microsec_clock::universal_time() + boost::posix_time::milliseconds(100);
        //if (!input_que.timed_receive(buf.get(), PKG_MAX_SIZE, recvd, priority, timeout)) {
        //    break;
        //}

        stat.StartTimer(StatCollector::TIMER_ENCODE);
        char* out_data = (char*)coder.encode((char *)(recv_buf.get()), recvd, out_lnt);
        stat.StopTimer(StatCollector::TIMER_ENCODE);
        memcpy(send_buf.get(), out_data, out_lnt);
        free(out_data);

        // Set start time at first enter
        if (StartTime == 0) {
            clock_gettime(CLOCK_MONOTONIC, &start_time);
            StartTime = 1;
        }
        SendBytes = SendBytes + out_lnt;
        shape_channel();

        if (diff_time(&cur_time,&start_time) >1) {
        //    cout << ChannelSpeed <<" SendBytes= "<< SendBytes << " Diff Time= " << diff_time(&cur_time,&start_time)<< " speed= " << (SendBytes*1000000/(diff_time(&cur_time,&start_time) ))<< "\n";
        }
        stat.AddPacket(out_lnt, true);
        output_que.send(send_buf.get(), out_lnt, 0);
    }

    return;
}


