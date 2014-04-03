#include "thread_read.h"

#include "ecc.h"
//#include "err.h"
#include "corrupt.h"
#include "pthread.h"
#include "threaded_coder.h"
#include "transport.h"

#include <errno.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <boost/interprocess/ipc/message_queue.hpp>

#include <iostream>

#include <QDebug>

using namespace boost::interprocess;

ReaderThread::ReaderThread(float err_percent,
                           transport &T,
                           size_t restart_block_cnt, StatCollector &stat) :
    err_percent(err_percent),
    T(T),
    restart_block_cnt(restart_block_cnt),
    stat(stat)
{ }

void ReaderThread::run()
{
    using std::cout;

    // Using:
    // read IP_ADDRESS ERRORS_PERCENT

    message_queue output_que(open_or_create, TO_DECODE_MSG, NUM_OF_PKGS, PKG_MAX_SIZE);
    message_queue input_que(open_or_create, TO_READ_MSG, NUM_OF_PKGS, PKG_MAX_SIZE);

    std::unique_ptr<uint8_t[]> recv_buf(new uint8_t[PKG_MAX_SIZE]);
    size_t recvd = 0;
    unsigned priority;

    //transport P;
    //int port = P.get_free_port();
    //cout << "on port: " << port;

    //transport T(ip.c_str(), port);

    cout<< " read started.\n"; 

    while (1) {
        //msg.in_buff_lnt = readed = T.read(msg.in_buff, msg.in_buff_lnt);
        input_que.receive(recv_buf.get(), PKG_MAX_SIZE, recvd, priority);

        Corruptor::add_err(recv_buf.get(), recvd, err_percent);

        output_que.send(recv_buf.get(), recvd, 0);
    }

    cout << "read quit\n";
    
    return;
}

