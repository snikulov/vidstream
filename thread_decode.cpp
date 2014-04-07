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
namespace bipc = boost::interprocess;

char rPacketByte;
uint8_t* rPacket;
uint32_t rMaxPacketSize = 100;
int32_t  rMaxLenRSTInGRP = 8;
int32_t  rMaxGRP = 15;
int32_t  rSubPacketLength = 20;
int32_t  rPacketHeaderLen = 7;
uint8_t  rTmpRST[1000];
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

 void DecoderThread::GetRSTFromPack(bipc::message_queue &output_que)

{

  using std::cout;
     using std::flush;


    int32_t   i1,j1;
    int32_t   rRSTLen ;
    int32_t   rRSTNum ;
    int32_t   rFrameNum ;
    int32_t   rLastRSTNum ;
    int32_t   rRSTStartNum;
    int32_t   rGrpCount;
    int32_t   TmpLen;
    rGetPos = 0;
    cout <<" check packet header \n" << flush;
    for (i1 = 0; i1<=rPacketHeaderLen; i1++){
        if (not GetByteFromPacket()) {
            return;
        }
    }
    cout <<"  packet header ok\n" << flush;

    bool BlockOK;
    rplen = rPacket[6];
    rFrameNum = rPacket[7];
    rRSTStartNum = rPacket[4]*255 + rPacket[5];
    rRSTNum = rRSTStartNum;

    rLastRSTNum = rRSTNum;
    rGetPos = rPacketHeaderLen+1;
    rNextGroupPos = -1;
    rNextGroupStartNum = 0;
    cout <<"\n Rec Packet Len="<< rplen<< " Frame="<<rFrameNum <<" RST Num="<< rRSTStartNum<< " \n" << flush;

    while (rGetPos<rplen)  {
        if (not GetByteFromPacket())
        {
            rGetPos = ( 1+(rGetPos / rSubPacketLength) ) * rSubPacketLength;
            while (rGetPos<rplen)
            {
                if (GetByteFromPacket())
                {
                    rRSTNum = rRSTStartNum+rPacketByte;
                    NextInpPos();
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
            TmpLen = 5;
            for ( j1 = 0; j1<= rRSTLen -1; j1++)   {
                if ( ! GetByteFromPacket())
                {
                    BlockOK = false;
                } else {
                    rTmpRST[TmpLen] = rPacketByte;
                    TmpLen++;
                };
                NextInpPos();
            };
            if (BlockOK)
            {


                    // put bytes to block
                rTmpRST[0] = rFrameNum;
                rTmpRST[1] = rRSTNum / 256;
                rTmpRST[2] = rRSTNum % 256;
                rTmpRST[3] = TmpLen / 256;
                rTmpRST[4] = TmpLen % 256 ;
                output_que.send(&rTmpRST, TmpLen, 0);
                cout <<"RECRST Frame="<<rFrameNum<<" RST="<<rRSTNum<<"\n"<<" RSTLen="<<TmpLen<<  flush;

                //for (int j1 = 0; j1 <  TmpLen-1; j1++){
                 //   PutOut(rTmpRST[j1])

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

    using std::cout;
       using std::flush;

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

        uint8_t* out_data = (uint8_t*)coder.decode((char *)(recv_buf.get()), recvd,
                                             out_lnt, send_buf.decoded_ok);
        //stat.AddPacket(recvd, send_buf.decoded_ok);
        cout <<" GetRST From PAck \n" <<flush;

        rPacket = out_data;
        GetRSTFromPack(output_que);
//        send_buf.data_len = out_lnt;
//
//        memcpy(send_buf.data, out_data, out_lnt);
//          output_que.send(&send_buf, out_lnt, 0);
        free(out_data);

//        output_que.send(&send_buf, out_lnt, 0);
    }

    return;
}
