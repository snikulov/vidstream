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
    size_t recvd = 0;
    unsigned priority;

    while (!killed) {

        input_que.receive(recv_buf.get(), PKG_MAX_SIZE, recvd, priority);

        //T.send(ip.c_str(), port, loc_buff, loc_size); //отсылаем пакет
        output_que.send(recv_buf.get(), recvd, 0);
    }

    return;
}

