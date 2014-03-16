#include "thread_decode.h"

#include <errno.h>
#include "ecc.h"
#include "err.h"
#include "pthread.h"
#include "threaded_coder.h"
#include "transport.h"
#include <sys/types.h>
#include <sys/errno.h>
#include <sys/msg.h>
#include <sys/ipc.h>

#include <boost/interprocess/ipc/message_queue.hpp>

using namespace boost::interprocess;

DecoderThread::DecoderThread(ecc &coder, size_t restart_block_cnt,
                             StatCollector &stat) :
    coder(coder),
    restart_block_cnt(restart_block_cnt),
    stat(stat)
{ }

void DecoderThread::run()
{
    message_queue input_que(open_or_create, TO_DECODE_MSG, NUM_OF_PKGS, PKG_MAX_SIZE);
    message_queue output_que(open_or_create, TO_OUT_MSG, NUM_OF_PKGS, PKG_MAX_SIZE);

    std::unique_ptr<uint8_t[]> recv_buf(new uint8_t[PKG_MAX_SIZE]);
    std::unique_ptr<uint8_t[]> send_buf(new uint8_t[PKG_MAX_SIZE]);
    size_t recvd, out_lnt;
    unsigned int priority;

    cout<< "decode started.\n";

    for (size_t cnt = 0; cnt < restart_block_cnt; cnt++) {

        input_que.receive(recv_buf.get(), PKG_MAX_SIZE, recvd, priority);
        //boost::posix_time::ptime timeout = boost::posix_time::microsec_clock::universal_time() + boost::posix_time::milliseconds(100);
        //if (!input_que.timed_receive((void*)&msg, buff_len, recvd, priority, timeout)) {
        //    return;
        //}

        char* out_data = (char*)coder.decode((char *)(recv_buf.get()), recvd, out_lnt);
        memcpy(send_buf.get(), out_data, out_lnt);
        free(out_data);

        output_que.send(send_buf.get(), out_lnt, 0);
    }

    cout << "decode quit\n";

    return;
}
