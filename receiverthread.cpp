#include "receiverthread.h"

#include <QDebug>

#include <boost/interprocess/ipc/message_queue.hpp>

#include "corrupt.h"
#include "membuf.h"
#include "split.h"
#include "queue_params.h"


namespace bipc = boost::interprocess;
namespace btime = boost::posix_time;

char rPacketByte;
char rPacket[1000];
uint32_t rMaxPacketSize = 100;
int32_t  rMaxLenRSTInGRP = 8;
int32_t  rMaxGRP = 15;
int32_t  rSubPacketLength = 20;
int32_t  rplen;
int rGetPos;
uint32_t rNextGroupPos, rNextGroupStartNum;
bool ByteError(int Pos )
{
    return false;
}
bool GetByteFromPacket()
{
  rPacketByte = rPacket[rGetPos];
  return (! ByteError(rGetPos)) ;// ByteError(getPos);

}



void  NextInpPos()

{
    rGetPos++;
    if ((rGetPos % rSubPacketLength) == 0)  {
        if (ByteError(rGetPos) || ByteError(rGetPos+1)) {
            rNextGroupPos = -1;
            rNextGroupStartNum = -1;
        } else {
            rNextGroupPos = rPacket[rGetPos];
            rNextGroupStartNum = rPacket[rGetPos+1];
        };
        rGetPos = rGetPos+2;
    };
};

void GetRSTFromPack()

{

    int32_t   i1,j1;
    int32_t   rRSTLen ;
    int32_t   rRSTNum ;
    int32_t   rLastRSTNum ;
    int32_t   rRSTStartNum;
    int32_t   rGrpCount;
    int32_t   TmpLen;
    char TmpRST[1000];
    bool BlockOK;
    rplen = rPacket[6];
    rRSTStartNum = rPacket[4]*255 + rPacket[5];
    rRSTNum = rRSTStartNum;
    rLastRSTNum = rRSTNum;
    rGetPos = 7;
    rNextGroupPos = 7;
    rNextGroupStartNum = 0;

    while (rGetPos<rplen)  {
        if (not GetByteFromPacket)
        {
            rGetPos = ( 1+(rGetPos / rSubPacketLength) ) * rSubPacketLength;
            while (rGetPos<rplen)
            {
                if (GetByteFromPacket)
                {
                    rRSTNum = rRSTStartNum+rPacketByte;
                    if (GetByteFromPacket())
                    {
                        rGetPos = rPacketByte;
                        break;
                    }
                }
                rGetPos = ( 1+(rGetPos / rSubPacketLength) ) * rSubPacketLength;
            }
            continue;
        }

        if ((rPacketByte & 0x80) != 0)  {

            rGrpCount = 1;
            rRSTLen = rPacket[rGetPos] & 0x7F;
        } else {

            rRSTLen = (rPacketByte >> 4) +1;

            rGrpCount = rPacketByte &0x0F;

        };

        NextInpPos();

        for (i1 = 0; i1<=  rGrpCount -1; i1++)   {
            BlockOK = true;
            TmpLen = 0;
            for ( j1 = 0; j1<= rRSTLen -1; j1++)   {
                if ( ! GetByteFromPacket())
                {
                    BlockOK = false;
                } else {
                    TmpRST[TmpLen] = rPacketByte;
                    TmpLen++;
                };
                NextInpPos();
            };
            if (BlockOK)
            {

                    // put bytes to block  for j1 = 0 to tmplen-1 do PutOut(TmpRst[j1])
            }

                    else
            {
                // skip block

            };

            rRSTNum++;
            rLastRSTNum = rRSTNum;
        };
    }
};





ReceiverThread::ReceiverThread(uint8_t *buffer, char *mask,
                               std::string queue_name,
                               ecc &encoder, BlockHistory &history,
                               size_t restart_block_cnt,
                               StatCollector &stat,
                               float err_percent,
                               bool broken_channel) :
    buffer(buffer),
    mask(mask),
    queue_name(queue_name),
    encoder(encoder),
    history(history),
    restart_block_cnt(restart_block_cnt),
    stat(stat),
    err_percent(err_percent),
    killed(false),
    broken_channel(broken_channel)
{
}

inline btime::ptime timeout(int ms) {
    return btime::microsec_clock::universal_time() + btime::milliseconds(ms);
}

void ReceiverThread::run()
{
    bipc::message_queue mq(bipc::open_or_create, queue_name.c_str(),
                           package_num, package_max_size);
    uint8_t *ptr;
    size_t recv_buf_size = package_max_size;
    std::unique_ptr<uint8_t[]> recv(new uint8_t[recv_buf_size]);
    size_t recv_size, decoded_size;
    unsigned priority;
    stat.StartFrame();
    // receive a block if it comes before the timeout of 10 ms elapses
    // put all received and successfully decoded blocks in history
    while (!killed && mq.timed_receive(recv.get(), recv_buf_size, recv_size, priority,
           timeout(10))) {
           // (recv = t.Receive(10, recv_size))) {
        uint8_t *encoded_ptr = recv.get();
        size_t encoded_len = recv_size;

        // add errors to the whole RestartBlock in recv buffer
        if (broken_channel) {
            memset(encoded_ptr, 0, encoded_len);
        } else {
            Corruptor::add_err(encoded_ptr, encoded_len, err_percent);
        }

        bool decoded_ok;
        stat.StartTimer(StatCollector::TIMER_DECODE);
        ptr = (uint8_t *) encoder.decode((char *) encoded_ptr, encoded_len, decoded_size,
                                         decoded_ok);
        stat.StopTimer(StatCollector::TIMER_DECODE);
        stat.AddPacket(encoded_len, decoded_ok);

        if (RestartBlock::get_rst_block_number(ptr) > history.size() ||
            RestartBlock::get_data_length(ptr) > decoded_size - RestartBlock::get_info_len()) {
            free(ptr);
            continue;
        }
        HistoryElement &hsblock = history[RestartBlock::get_rst_block_number(ptr)];
        mask[RestartBlock::get_rst_block_number(ptr)] = hsblock.get_age() <= MAX_HISTORY_DIFF;

        //replace hsblock with new block if new block is good or hsblock is old
        if (decoded_ok || hsblock.get_age() > MAX_HISTORY_DIFF) {
            hsblock = RestartBlock(RestartBlock::get_data_ptr(ptr),
                                   RestartBlock::get_data_length(ptr));
            hsblock.get_b().set_info(RestartBlock::get_frame_number(ptr),
                                     RestartBlock::get_rst_block_number(ptr),
                                     RestartBlock::get_data_length(ptr));
            mask[RestartBlock::get_rst_block_number(ptr)] = true;
        }
        free(ptr);
    }

    ComposeJpeg();
    stat.FinishFrame();
}

void ReceiverThread::ComposeJpeg()
{
    uint8_t *base = buffer;
    for (size_t cur_iteration = 0; cur_iteration < restart_block_cnt;
         ) {

        // add RST marker before each block, except the first one
        if (cur_iteration) {
            *base = 0xFF;
            base++;
            *base = Markers::RST + (cur_iteration - 1) % 8;
            base++;
        }

        HistoryElement &hsblock = history[cur_iteration];
        if (hsblock.get_age() > MAX_HISTORY_DIFF) {
            hsblock.clear();
        }
        hsblock.increase_age();

        // skip the info part
        size_t len = hsblock.get_b().data_length();
        // encoded data contains block header as well
        uint8_t *data = hsblock.get_b().data_ptr();
        // copy hsblock contents to output buffer
        // restore the 0x00s after 0xFF
        for (unsigned i = 0; i < len; i++) {
            *base = data[i];
            base++;
            if (data[i] == 0xFF) {
                *base = 0x00;
                base++;
            }
        }
        cur_iteration++;
    }
    *base = 0xFF;
    base++;
    *base = 0xD9;
}
