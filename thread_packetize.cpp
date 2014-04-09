#include "thread_packetize.h"

#include <cassert>
#include <cmath>
#include <fstream>

#include <QDebug>

#include <threaded_coder.h>
#include <boost/interprocess/ipc/message_queue.hpp>

#include "interlace.h"
#include "split.h"
#include <stdio.h>

uint32_t RSTCount;
uint32_t MaxLenRST;
int32_t RSTStart, PacketLen;
uint32_t BufSize, OutBufSize;
int32_t  PrevRSTLen,  cur_rst_len;
uint32_t PackStartRSTNum,CurGrp, LenPos;
uint32_t CurRSTNum, CurFrameNum;
int32_t  LastGRP ;
char     Packet[1000];
uint8_t TmpRST[1000];
uint16_t TmpRSTLen;

uint32_t TmpLen ;
int32_t MaxPacketSize = 50;
int32_t  MaxLenRSTInGRP = 8;
int32_t  MaxGRP = 15;
int32_t  SubPacketLength = 20;
int32_t  PacketHeaderLen = 7;
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
bool FirstInSub, FirstPacket, PacketReadyForSend, FirstPackOfFrame;
uint32_t IDD;
uint8_t PackFrame;
bool LogFirstEnter = true;
char* LogFileName = "##packetize.log";

void log(char* logstr){
    FILE* logfile;
    if (LogFirstEnter){
        remove(LogFileName);
        LogFirstEnter = false;
    }
    logfile = fopen(LogFileName, "a");
    fprintf(logfile,"%s \n",logstr);
    fclose(logfile);
}

void xlog(u_int8_t lstr)
{
    FILE* logfile;
    if (LogFirstEnter){
        remove(LogFileName);
        LogFirstEnter = false;
    }
    logfile = fopen(LogFileName, "a");
    fprintf(logfile,"%X ",lstr);
    fclose(logfile);
}
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

void InitRSTPack()
{
    using std::cout;
       using std::flush;
    log( "Init Pack ");
    FirstInSub = false; //First frp in pack not need
    PacketReadyForSend = false;
    PrevRSTLen = 0;
    PacketLen =0;
    LastGRP =0;
    LenPos = 7;
    Packet[0] = 0; //signature
    Packet[1] = 1; //signature
    Packet[2] = 2; //signature
    Packet[3] = 3; //signature
    Packet[4] = CurRSTNum / 256 ; // Number of start RST block in pack
    Packet[5] = CurRSTNum % 256 ; //
    Packet[6] = 0 ; //reserv for len
    Packet[7] = CurFrameNum; // frame number
    PacketStartRSTNum = CurRSTNum;
    LastMarkerPosition = -1;
    LastMarkerRSTNum   = -1;
    PacketLen += PacketHeaderLen+1;
}
void FlushGroup()
{
    using std::cout;
       using std::flush;
    log("Flush group ");

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
    using std::cout;
       using std::flush;
    log("Add RST To group");
    if (LastGRP == 0)   {
        LenPos = PacketLen;
        PutToPacket(0);
        LastMarkerRSTNum = RSTCount;
    };
    LastGRP++;
    for (i1=0 ; i1<= cur_rst_len-1; i1++ ) {
        PutToPacket(TmpRST[i1]);
    };
    if (LastGRP == MaxGRP )
    {
        FlushGroup();
    };
};

void SendPacket(){
    if (PacketLen>PacketHeaderLen){
        PacketReadyForSend = true;
//        mq.send(block.raw_ptr(), block.raw_length(), 0);
    }
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
void PacketizerThread::AddRSTBlock(bipc::message_queue &mq)
{
    using std::cout;
       using std::flush;
    char logstr[1000];
    sprintf(logstr,"ADD RST Frame=%d  RST=%d  Len=%d " ,CurFrameNum, CurRSTNum, cur_rst_len);
    log(logstr);
    bool ChangeGroup;
    if ((PacketLen + cur_rst_len) > MaxPacketSize)
    {
        FlushGroup();
        Packet[6]=PacketLen;
        log("Send packet");
        for (int i1=0; i1< PacketLen;i1++)
        {
            xlog(Packet[i1]);
        }
        log("");
        mq.send(&Packet, PacketLen, 0);
        InitRSTPack();
    };
    ChangeGroup =((cur_rst_len != PrevRSTLen))  || ( cur_rst_len > MaxLenRSTInGRP);
    if (ChangeGroup)
    {
        FlushGroup();
    }
    AddRSTToGroup();
    PrevRSTLen = cur_rst_len;
};



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
    FirstPackOfFrame = true;
    for (size_t i = 0; i < buffer_size; i++) {
        c = buffer[i];
        // section can only contain FF 00 and RST markers
        if (p == 0xFF) { // && (c == 0x00 || is_rst(c))) {
            if (is_rst(c)) {
                // send buffer
                if (interlace_refresh_block(rst_cnt, interlace)) {
                    block.set_info(frame_number, rst_cnt, block.pushbacks_count());
                    //ss TransmitBlock(block, mq); //SS
                    CurRSTNum = rst_cnt;
                    CurFrameNum = frame_number;
                    cur_rst_len = block.data_length();
                    size_t len = block.data_length();
                    uint8_t *data = block.data_ptr();
                                // copy block contents to TmpRS
                    for (unsigned TmpLen = 0; TmpLen < len; TmpLen++) {
                        TmpRST[TmpLen] = data[TmpLen];
                    }
                    if (FirstPackOfFrame){
                        InitRSTPack();
                        FirstPackOfFrame = false;
                    }
                    AddRSTBlock(mq); //ss

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
//          TransmitBlock(block, mq); //SS
        CurRSTNum = rst_cnt;
        CurFrameNum = frame_number;
        cur_rst_len = block.data_length();
        size_t len = block.data_length();
        uint8_t *data = block.data_ptr();
        // copy block contents to TmpRS
        for (unsigned TmpLen = 0; TmpLen < len; TmpLen++) {
            TmpRST[TmpLen] = data[TmpLen];
        }
         AddRSTBlock(mq);//SS
    }
    FlushGroup();
    if (PacketLen>PacketHeaderLen+1){
        mq.send(&Packet[0], 100, 0);
    }
}

