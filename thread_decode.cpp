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

u_int8_t rPacketByte;
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
char* rLogFileName = "##unpack.log";
char rlogstr[1000];
bool rLogFirstEnter = true;
bool rRawLogFirstEnter = true;

void rlog(char* lstr)
{
    FILE* logfile;
    if (rLogFirstEnter){
        remove(rLogFileName);
        rLogFirstEnter = false;
    }
    logfile = fopen(rLogFileName, "a");
    fprintf(logfile,"%s \n",lstr);
    fclose(logfile);
}
char* rRawFileName = "##rawdata.log";
void rrawlog(const void *lstr, int len)
{
    FILE* logfile;
    if (rRawLogFirstEnter){
        remove(rRawFileName);
        rRawLogFirstEnter = false;
    }
    logfile = fopen(rRawFileName, "a");
    fwrite(lstr,len,1,logfile);
    fclose(logfile);
}


void rxlog(u_int8_t lstr)
{
    FILE* logfile;
    if (rLogFirstEnter){
        remove(rLogFileName);
        rLogFirstEnter = false;
    }
    logfile = fopen(rLogFileName, "a");
    fprintf(logfile,"%X ",lstr);
    fclose(logfile);
}
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
            sprintf(rlogstr,"\nCheck point %d ERR\n",rGetPos);
            rlog(rlogstr);

        } else {
            rNextGroupPos = rPacket[rGetPos];
            rNextGroupStartNum = rPacket[rGetPos+1];
            sprintf(rlogstr,"\nCheck point %d NextGRP Pos=%d NextGRP RST NUM=%d\n", rGetPos, rNextGroupPos, rNextGroupStartNum);
            rlog(rlogstr);
        };
        rGetPos = rGetPos+2;
    };
};

 void DecoderThread::GetRSTFromPack(bipc::message_queue &output_que)

{



    int32_t   i1,j1;
    int32_t   rRSTLen ;
    int32_t   rRSTNum ;
    int32_t   rFrameNum ;
    int32_t   rLastRSTNum ;
    int32_t   rRSTStartNum;
    int32_t   rGrpCount;
    int32_t   TmpLen;
    rGetPos = 0;
    rlog("check packet header");
    for (i1 = 0; i1<=rPacketHeaderLen; i1++){
        if (not GetByteFromPacket()) {
            return;
        }
    }
    rlog("packet header ok");
    bool BlockOK;
    rplen = rPacket[6];
    rFrameNum = rPacket[7];
    rRSTStartNum = rPacket[4]*255 + rPacket[5];
    rRSTNum = rRSTStartNum;

    rLastRSTNum = rRSTNum;
    rGetPos = rPacketHeaderLen+1;
    rNextGroupPos = -1;
    rNextGroupStartNum = 0;
    sprintf(rlogstr,"Packet Len=%d Frame%d Start RST num=%d  ", rplen, rFrameNum, rRSTStartNum);
    rlog(rlogstr);
    while (rGetPos<rplen)  {
        if (not GetByteFromPacket())
        {
            sprintf(rlogstr,"err input byte pos=%d", rGetPos);
            rlog(rlogstr);
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
        sprintf(rlogstr,"Group pos=%d %.2x ", rGetPos, rPacketByte);
        rlog(rlogstr);
        if ((rPacketByte & 0x80) != 0)  {
            rGrpCount = 1;
            rRSTLen = rPacket[rGetPos] & 0x7F;
        } else {
            rRSTLen = (rPacketByte >> 4) +1;
            rGrpCount = rPacketByte &0x0F;
        };
        NextInpPos();
        sprintf(rlogstr,"Group. Count=%d Len=%d ", rGrpCount, rRSTLen);
        rlog(rlogstr);

        for (i1 = 0; i1<=  rGrpCount -1; i1++)   {
            BlockOK = true;
            for (int itmp=0; itmp< 100; itmp++)
            {
                rTmpRST[itmp]= 0;
            }
            TmpLen = 5;
            for ( j1 = 0; j1<= rRSTLen -1; j1++)   {
                if ( !GetByteFromPacket())
                {
                    BlockOK = false;
                } else {
                    rxlog(rPacketByte);
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
                rTmpRST[3] = (TmpLen-5) / 256;
                rTmpRST[4] = (TmpLen-5) % 256 ;
                sprintf(rlogstr, "RST to queue Frame=%d RSTNum=%d RSTLen%d", rFrameNum,  rRSTNum, TmpLen);
                rlog(rlogstr);
                for (int i1=0; i1< TmpLen;i1++){
                    rxlog(rTmpRST[i1]);
                }
                rlog("");
                output_que.send(&rTmpRST, 100, 0);

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
        sprintf(rlogstr,"Receive New paket len=%d",out_lnt);
        rlog(rlogstr);
        for (int i1=0; i1< out_lnt; i1++)
        {
            rxlog(out_data[i1]);
        }
        rlog("");
        rPacket = out_data;
        GetRSTFromPack(output_que); //SS
        send_buf.data_len = out_lnt;

        memcpy(send_buf.data, out_data, out_lnt);
        int rawlen=out_data[4]+5;
//        cout <<"\n rawlen="<<rawlen<<flush;
//        rrawlog(out_data,rawlen);
        free(out_data);

//ss        output_que.send(&send_buf, out_lnt, 0); //SS
    }

    return;
}
