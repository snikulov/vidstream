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
                    // and put block to queue
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






DecoderThread::DecoderThread(ecc &coder,
                             StatCollector &stat) :
    coder(coder),
    stat(stat),
    killed(false)
{ }

void DecoderThread::run()
{
    message_queue input_que(open_or_create, TO_DECODE_MSG, NUM_OF_PKGS, PKG_MAX_SIZE);
    message_queue output_que(open_or_create, TO_OUT_MSG, NUM_OF_PKGS, PKG_MAX_SIZE);

    std::unique_ptr<uint8_t[]> recv_buf(new uint8_t[PKG_MAX_SIZE]);
    //std::unique_ptr<uint8_t[]> send_buf(new uint8_t[PKG_MAX_SIZE]);
    DecodedBlock send_buf;
    size_t recvd, out_lnt;
    unsigned int priority;
    std::vector<char> decoded;

    while (!killed) {

        input_que.receive(recv_buf.get(), PKG_MAX_SIZE, recvd, priority);

        char* out_data = (char*)coder.decode((char *)(recv_buf.get()), recvd,
                                             out_lnt, send_buf.decoded_ok);
        //stat.AddPacket(recvd, send_buf.decoded_ok);
        send_buf.data_len = out_lnt;
        memcpy(send_buf.data, out_data, out_lnt);
        free(out_data);

        output_que.send(&send_buf, out_lnt, 0);
    }

    return;
}
