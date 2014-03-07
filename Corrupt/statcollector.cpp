#include "statcollector.h"

#include <QDebug>

#include <cstring>

StatCollector::StatCollector()
{
    Reset();
}

void StatCollector::Reset()
{
    total_packet_cnt = 0;
    total_packet_size = 0;
    failed_packet_cnt = 0;
    total_frame_cnt = 0;
    total_frame_size = 0;
    last_packet_size = 0;
    last_frame_size = 0;
    cur_frame_size = 0;
    total_bch_pkg_cnt = 0;
    failed_bch_pkg_cnt = 0;
    memset(total_time, 0, TIMER_CNT * sizeof(total_time[0]));
    memset(last_time,  0, TIMER_CNT * sizeof(last_time [0]));
}

void StatCollector::AddPacket(unsigned long long size, bool decoded_ok)
{
    total_packet_cnt++;
    total_packet_size += size;
    cur_frame_size += size;

    if (!decoded_ok) {
        failed_packet_cnt++;
    }
}

void StatCollector::AddBchPkg(unsigned long long cnt, unsigned long long failed_cnt)
{
    total_bch_pkg_cnt += cnt;
    failed_bch_pkg_cnt += failed_cnt;
}

void StatCollector::StartFrame()
{
    cur_frame_size = 0;
    total_time[TIMER_ENCODE] = 0;
    total_time[TIMER_DECODE] = 0;
}

void StatCollector::FinishFrame()
{
    total_frame_size += cur_frame_size;
    total_frame_cnt++;
    last_frame_size = cur_frame_size;
}

void StatCollector::ResetFileIOTimer()
{
    total_time[TIMER_FILEIO] = 0;
}

void StatCollector::StartTimer(Timers id)
{
    started_time[id] = clock();
}

void StatCollector::StopTimer(Timers id)
{
    last_time[id] = clock() - started_time[id];
    total_time[id] += last_time[id];
}

std::string StatCollector::GetStats() {
    return GetPacketSizeStats() + "\n" +
           GetFrameSizeStats()  + "\n" +
           GetErrorStats()      + "\n" +
           GetTimerStats()      + "\n";
}

std::string StatCollector::GetPacketSizeStats()
{
    if (!total_packet_cnt) {
        return "No packet size data available";
    } else {
        return "Average packet size: " +
                std::to_string((1.0 * total_packet_size) / total_packet_cnt);
    }
}

std::string StatCollector::GetFrameSizeStats()
{
    if (!total_frame_cnt) {
        return "No frame size data available";
    } else {
        float avg_frame = (1.0 * total_frame_size) / total_frame_cnt;
        return "Average frame size: " +
               std::to_string(avg_frame) +
               "\n" +
               "Required bandwith at 25 fps: " +
                std::to_string((avg_frame * 25 * 8) / (1024 * 1024)) +
               " Mbit/s";
    }
}

std::string StatCollector::GetErrorStats()
{
    std::string res;
    if (!total_packet_cnt) {
        res = "No packet error data available\n";
    } else {
        res = "Failed to decode " +
              std::to_string((100.0 * failed_packet_cnt) / total_packet_cnt) +
              "% of packets\n";
    }
    if (!total_bch_pkg_cnt) {
        res += "No package error data available";
    } else {
        res += "Failed to decode " +
               std::to_string((100.0 * failed_bch_pkg_cnt) / total_bch_pkg_cnt) +
               "% of packages";
    }
    return res;
}

std::string StatCollector::GetTimerStats()
{
    std::string res;
    for (int i = 0; i < TIMER_CNT; i++) {
        res += desc[i] + std::to_string(total_time[i] * 1.0 / CLOCKS_PER_SEC);
        if (i != TIMER_FRAME) {
            float percentage = total_time[i] * 100.0 / last_time[TIMER_FRAME];
            res += " (" + std::to_string(percentage) + "%)";
        } else {
            res += " (" + std::to_string(CLOCKS_PER_SEC / (1.0 * total_time[i])) + " fps)";
        }
        res += "\n";
    }
    return res;
}
