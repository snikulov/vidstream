#include "thread_packetize.h"

#include <cassert>
#include <cmath>
#include <fstream>

#include <QDebug>

#include <threaded_coder.h>
#include <boost/interprocess/ipc/message_queue.hpp>

#include "interlace.h"
#include "split.h"


uint32_t RSTCount;
uint32_t MaxLenRST;
uint32_t RstStart, PacketLen;
uint32_t BufSize, OutBufSize;
int32_t  PrevRSTLen,  cur_rst_len;
uint32_t PackStartRSTNum,CurGrp, LenPos;
int32_t  LastGRP ;
char Packet[1000];
char TmpRST[1000];
uint32_t TmpLen ;
uint32_t MaxPacketSize = 100;
int32_t  MaxLenRSTInGRP = 8;
int32_t  MaxGRP = 15;
int32_t  SubPacketLength = 20;
bool BlockOK ;
int32_t  LastMarkerPosition ;
int32_t  LastMarkerRSTNum   ;
uint32_t PacketStartRSTNum  ;
uint32_t getPos ;
uint32_t pLen;
uint32_t GrpCount, RstLen ;
uint32_t RSTNum ;
char PacketByte;
uint32_t NextGroupPos, NextGroupStartNum;
uint32_t RSTStartNum;
bool FirstInSub;
uint32_t IDD;

void  PutToPacket(char b)
{
    Packet[PacketLen] = b;
    PacketLen++;
    if ((PacketLen % SubPacketLength)  == 0)
    {
            LastMarkerPosition = PacketLen;
            PacketLen += 2;
            FirstInSub = true;
      }
}

void InitRSTPack(int StartNum)
{
    FirstInSub = false; //First frp in pack not need
    PackStartRSTNum=StartNum;
    PrevRSTLen = 0;
    PacketLen =0;
    LastGRP =0;
    LenPos = 7;
    Packet[0] = 0; //signature
    Packet[1] = 1; //signature
    Packet[2] = 2; //signature
    Packet[3] = 3; //signature
    Packet[4] = StartNum / 256 ;
    Packet[5] = StartNum % 256 ;
    Packet[6] = 0 ; //reserv for len
    PacketStartRSTNum = RSTCount;
    LastMarkerPosition = -1;
    LastMarkerRSTNum   = -1;
    OutBufSize += 7;
    PacketLen += 7;
}
void FlushGroup()
{

    if (LastGRP > 0)
    {
        if (PrevRSTLen>MaxLenRSTInGRP )
        {
            Packet[LenPos] = PrevRSTLen | 0x80;
        }  else {
            Packet[LenPos] = ((PrevRSTLen - 1) << 4) + LastGRP;
        } ;
        if (FirstInSub)
        {
            Packet[LastMarkerPosition] = RSTCount - PacketStartRSTNum;
            Packet[LastMarkerPosition+1] = LenPos - LastMarkerPosition;
            FirstInSub = false;
        };
    };
    PrevRSTLen = 0;
    LastGRP = 0;
    LenPos = PacketLen;
};

void AddRSTToGroup()
{
    int i1;

    if (LastGRP == 0)   {

        LenPos = PacketLen;
        PutToPacket(0);
        LastMarkerRSTNum = RSTCount;
    };

    LastGRP++;
    for (i1=0 ; i1<= cur_rst_len-1; i1++ ) {

        //###    PutToPacket(Buf[RSTPos+i1]);
    };
    if (LastGRP == MaxGRP )
    {
        FlushGroup();
    };
};

void SendPacket(){
};

void AddRSTBlock()
{

    bool ChangeGroup;
    if ((PacketLen + cur_rst_len) > MaxPacketSize)
    {
        FlushGroup();
        Packet[6]=PacketLen;
        SendPacket();
        InitRSTPack(RSTCount);
    };
    ChangeGroup =((cur_rst_len != PrevRSTLen))  || ( cur_rst_len > MaxLenRSTInGRP);
    if (ChangeGroup)
    {
        FlushGroup();
    }
    AddRSTToGroup();
    PrevRSTLen = cur_rst_len;
};





namespace bipc = boost::interprocess;

PacketizerThread::PacketizerThread(const uint8_t *buffer, const size_t buffer_size,
                                   uint8_t frame_number, size_t restart_block_cnt,
                                   StatCollector &stat,
                                   const InterlaceControl &interlace) :
    buffer(buffer),
    buffer_size(buffer_size),
    frame_number(frame_number),
    restart_block_cnt(restart_block_cnt),
    stat(stat),
    interlace(interlace)
{
}

void PacketizerThread::TransmitBlock(RestartBlock& block,
                                    bipc::message_queue &mq)
{
    //send_data msg;
    //msg.in_buff_lnt = block.raw_length();
    //memcpy(msg.in_buff, block.raw_ptr(), block.raw_length());
    mq.send(block.raw_ptr(), block.raw_length(), 0);
}

void PacketizerThread::run()
{
    bipc::message_queue mq(bipc::open_or_create, TO_ENCODE_MSG, NUM_OF_PKGS, PKG_MAX_SIZE);
    // c - current char, p - previous char
    uint8_t p = 0, c;
    uint16_t rst_cnt = 0;
    RestartBlock block;
    for (size_t i = 0; i < buffer_size; i++) {
        c = buffer[i];
        // section can only contain FF 00 and RST markers
        if (p == 0xFF) { // && (c == 0x00 || is_rst(c))) {
            if (is_rst(c)) {
                // send buffer
                if (interlace_refresh_block(rst_cnt, interlace)) {
                    block.set_info(frame_number, rst_cnt, block.pushbacks_count());
                    TransmitBlock(block, mq);
                }
                block.clear();
                rst_cnt++;
            } else { // {p, c} = FF00; ignoring 00
                block.push_back(p);
            }
        } else { // p != 0xFF
            if (c != 0xFF) { // certainly not a marker
                block.push_back(c);
            }
        }
        p = c;
    }

    if (block.pushbacks_count() > 0) {
        block.set_info(frame_number, rst_cnt,block.pushbacks_count());
        TransmitBlock(block, mq);
    }
}

