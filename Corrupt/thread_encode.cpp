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

        char* out_data = (char*)coder.encode((char *)(recv_buf.get()), recvd, out_lnt);
        memcpy(send_buf.get(), out_data, out_lnt);
        free(out_data);

        output_que.send(send_buf.get(), out_lnt, 0);
    }

    return;
}


