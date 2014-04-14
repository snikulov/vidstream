#include "avhandler.h"
#include "ecc.h"
#include "settings.h"
#include "thread_loader.h"
#include "thread_packetize.h"
#include "thread_encode.h"
#include "thread_send.h"
#include "threaded_coder.h"

#include <iostream>
#include <QString>
#include <QFile>
#include <boost/interprocess/ipc/message_queue.hpp>

int main(int argc, char *argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <video file>" << std::endl;
        return -1;
    }
    bool grayscale = false;
    // open video file
    std::string filename = argv[1];
    int ret;
    if ((ret = AVHandler::Instance()->open_input_file(filename.c_str()))) {
        qDebug() << "Failed to open file " << filename.c_str();
        qDebug() << "Error code " << ret;
        return -1;
    }
    // load settings
    Settings stored_settings[2];
    Settings settings;
    try {
        LoadSettingsFromFile("settings.conf", stored_settings[0],
                                              stored_settings[1]);
        settings = stored_settings[0];
    } catch(...) {
        qDebug() << "Failed to load settings";
    }

    // queues created by a dead process may hang
    boost::interprocess::message_queue::remove(TO_READ_MSG);
    boost::interprocess::message_queue::remove(TO_SEND_MSG);
    boost::interprocess::message_queue::remove(TO_ENCODE_MSG);
    boost::interprocess::message_queue::remove(TO_DECODE_MSG);
    boost::interprocess::message_queue::remove(TO_OUT_MSG);

    StatCollector stat;
    // start threads
    int port = 0;
    ecc coder(settings.bch_m, settings.bch_t);
    transport sender_tp;
    SenderThread sender("127.0.0.1", port, sender_tp, stat);
    EncoderThread encoder(coder, stat);
    size_t transmit_restart_count;
    LoaderThread loader(stat, transmit_restart_count);
    loader.SetGrayscale(grayscale);
    sender.start();
    encoder.start();
    loader.start();
    loader.wait();
    encoder.wait();
    sender.wait();
}
