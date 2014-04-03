#include "senderthread.h"

#include <cassert>
#include <cmath>
#include <fstream>
#include <QThread>
#include <QDebug>
#include <iostream>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/date_time/time.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <ctime>
#include "interlace.h"
#include "split.h"
#include "queue_params.h"

uint32_t StartTime = 0;
uint64_t SendBytes = 0;

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
    char Len;
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





struct timespec start_time, cur_time;
uint32_t ChannelSpeed = 10000000/8;

int Base_len = 3;
uint8_t packet[1000];
uint32_t grp_len, grp_count, prev_rst_len, last_grp, pack_size, out_size, loc_rst_count;



void stat_start_frame()
{
    loc_rst_count = 0;
    cur_rst_len = 0;
    prev_rst_len = 0;
    last_grp = 0;
    grp_len = 0;
    grp_count = 0;
    pack_size = 0;
    out_size = 0;
}
void flush_group()
{

}

void stat_flush_pack()
{

    out_size+= 6+(out_size / 100)*6;
}

void  stat_add_rst_out()
{
    loc_rst_count++;
    if( (cur_rst_len > (Base_len + 2)) || (cur_rst_len < Base_len) )
    {
        if (last_grp >0)  {
            // put out last group
            out_size++;
            // if group count > 3 and <7 (if 7 - on prev  step group must be  flushed)
            if (last_grp > 3)  {
                out_size++;
            }
        };
        out_size++;
        last_grp = 0;
    } else  {
        last_grp++;;
        if (last_grp > 7)
        {
            last_grp = 0;
            out_size += 2;
        };
    };
    out_size += cur_rst_len;
}



int64_t diff_time(struct timespec *timeA_p, struct timespec *timeB_p)
{
    clock_gettime(CLOCK_MONOTONIC, timeA_p);
    return (((timeA_p->tv_sec * 1000000000) + timeA_p->tv_nsec) -
           ((timeB_p->tv_sec * 1000000000) + timeB_p->tv_nsec)) /1000 ;
}

void shape_channel()
{

    if (  diff_time(&cur_time,&start_time)  > 0 ) {

       while  ( SendBytes*1000000/diff_time(&cur_time,&start_time) > ChannelSpeed)
       {
          usleep(1000);
       }

    }
}

namespace bipc = boost::interprocess;

SenderThread::SenderThread(const uint8_t *buffer, const size_t buffer_size,
                           std::string queue_name,
                           ecc &encoder, uint8_t frame_number,
                           StatCollector &stat,
                           const InterlaceControl &interlace) :
    buffer(buffer),
    buffer_size(buffer_size),
    queue_name(queue_name),
    encoder(encoder),
    frame_number(frame_number),
    stat(stat),
    interlace(interlace)
{
}

int SenderThread::TransmitBlock(RestartBlock& block, bipc::message_queue &mq)
{
    using std::cout;

    size_t encoded_len = 0;

    stat.StartTimer(StatCollector::TIMER_ENCODE);

    uint8_t *encoded_ptr = (uint8_t *) encoder.encode((char *) block.raw_ptr(),
                                                      block.raw_length(), encoded_len);
    stat.StopTimer(StatCollector::TIMER_ENCODE);


    // Set start time at first enter
    if (StartTime == 0) {
        clock_gettime(CLOCK_MONOTONIC, &start_time);
        StartTime = 1;
    }
    SendBytes = SendBytes + encoded_len;
    shape_channel();

    if (diff_time(&cur_time,&start_time) > 1) {
    //    cout << ChannelSpeed <<" SendBytes= "<< SendBytes << " Diff Time= " << diff_time(&cur_time,&start_time)<< " speed= " << (SendBytes*1000000/(diff_time(&cur_time,&start_time) ))<< "\n";
    }
    mq.send(encoded_ptr, encoded_len, 0);
    free(encoded_ptr);
    return 1;
}

void SenderThread::run()
{
    using std::cout;
    using std::flush;

    bipc::message_queue mq(bipc::open_or_create, queue_name.c_str(),
                           package_num, package_max_size);

    // c - current char, p - previous char
    uint8_t p = 0, c;
    stat_start_frame();
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

                    cur_rst_len = block.data_length();
                    stat_add_rst_out();
                     if (!TransmitBlock(block, mq)) {
                        return;
                    }
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
    stat_flush_pack();
 //   cout << "Frame size = "<< out_size <<" blocks "<< loc_rst_count<<"\n"<< flush;

    // send remaining data
    if (block.pushbacks_count() > 0 && interlace_refresh_block(rst_cnt, interlace)) {
        block.set_info(frame_number, rst_cnt, block.pushbacks_count());
        if (!TransmitBlock(block, mq)) {
            return;
        }
    }
}

