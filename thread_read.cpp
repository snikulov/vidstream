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
    size_t recvd = 0;
    unsigned priority;

    int port = 32000;
    int readed;
    char loc_buff[1500]; // 4 bytes for start key
    size_t  loc_size = 0;

    cout << "on port: " << port;

    transport T_inp("127.0.0.1", port);

    while (1) {
        readed = T_inp.read(loc_buff, PKG_MAX_SIZE);
        cout <<"readed=" <<readed<< "\n"<< flush;
//        input_que.receive(recv_buf.get(), PKG_MAX_SIZE, recvd, priority);
        if (readed>0)
        {
                memcpy(recv_buf.get(),loc_buff,readed);
        } else {
            continue;
        }
        recvd = readed;
        if (!broken_channel) {
            Corruptor::add_err(recv_buf.get(), recvd, err_percent);

        } else {
            memset(recv_buf.get(), 0, recvd);
        }

        output_que.send(recv_buf.get(), PKG_MAX_SIZE, 0);
    }

    cout << "read quit\n";
    
    return;
}

