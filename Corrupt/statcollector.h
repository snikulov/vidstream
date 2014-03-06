#ifndef STATCOLLECTOR_H
#define STATCOLLECTOR_H

#include <string>
#include <cstddef>
#include <cstdio>
#include <ctime>

class StatCollector
{
public:
    enum Timers { TIMER_FRAME,
                  TIMER_FILEIO,
                  TIMER_AVI,
                  TIMER_ENCODE,
                  TIMER_DECODE,
                  TIMER_JPEG_CREATE,
                  TIMER_JPEG_READ,
                  TIMER_INTERLACE,
                  TIMER_SCALING
                };
    StatCollector();
    void Reset();
    void AddPacket(unsigned long long size, bool decoded_ok);
    void AddBchPkg(unsigned long long cnt, unsigned long long failed_cnt);
    void StartFrame();
    void FinishFrame();
    void ResetFileIOTimer();
    void StartTimer(Timers id);
    void StopTimer(Timers id);
    std::string GetStats();
    std::string GetPacketSizeStats();
    std::string GetFrameSizeStats();
    std::string GetErrorStats();
    std::string GetTimerStats();
private:
    enum { TIMER_CNT = 9};
    std::string desc[TIMER_CNT] = { "frame processing time: ",
                                    "file i/o time: ",
                                    "video decoding time: ",
                                    "encoding time: ",
                                    "decoding time: ",
                                    "JPEG creation time: ",
                                    "JPEG reading time: ",
                                    "interlace composition time: ",
                                    "scaling time: "
                                  };
    unsigned long long total_packet_size;
    unsigned long long total_packet_cnt;
    unsigned long long failed_packet_cnt;
    unsigned long long cur_frame_size;
    unsigned long long total_frame_size;
    unsigned long long total_frame_cnt;
    unsigned long long last_packet_size;
    unsigned long long last_frame_size;

    unsigned long long total_bch_pkg_cnt;
    unsigned long long failed_bch_pkg_cnt;
    clock_t total_time[TIMER_CNT];
    clock_t last_time[TIMER_CNT];
    clock_t started_time[TIMER_CNT];

    StatCollector(const StatCollector &T);
};

#endif // STATCOLLECTOR_H
