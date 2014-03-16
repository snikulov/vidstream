#include "thread_send.h"

#include <errno.h>
#include "ecc.h"
#include "err.h"
#include "pthread.h"
#include "threaded_coder.h"
#include "transceiver.h"
#include "transport.h"
#include <sys/types.h>
#include <sys/errno.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <boost/interprocess/ipc/message_queue.hpp>

using namespace boost::interprocess;

SenderThread::SenderThread(const char *ip, unsigned port,
                           transport &T, Transceiver &tcv,
                           size_t restart_block_cnt, StatCollector &stat) :
    ip(ip),
    port(port),
    T(T),
    tcv(tcv),
    restart_block_cnt(restart_block_cnt),
    stat(stat)
{ }

void SenderThread::run()
{

    message_queue input_que(open_or_create, TO_SEND_MSG, NUM_OF_PKGS, PKG_MAX_SIZE);
    message_queue output_que(open_or_create, TO_READ_MSG, NUM_OF_PKGS, PKG_MAX_SIZE);

    std::unique_ptr<uint8_t[]> recv_buf(new uint8_t[PKG_MAX_SIZE]);
    size_t recvd = 0;
    unsigned priority;


    cout<< "send started.\n"; 

    for (size_t cnt = 0; cnt < restart_block_cnt; cnt++) {

        input_que.receive(recv_buf.get(), PKG_MAX_SIZE, recvd, priority);
        //boost::posix_time::ptime timeout = boost::posix_time::microsec_clock::universal_time() + boost::posix_time::milliseconds(100);
        //if (!input_que.timed_receive((void*)&msg, buff_len, recvd, priority,
        //                             timeout)) {
        //    return;
        //}

        //T.send(ip.c_str(), port, loc_buff, loc_size); //отсылаем пакет
        output_que.send(recv_buf.get(), PKG_MAX_SIZE, 0);
    }

    cout << "send quit\n";

    return;
}

