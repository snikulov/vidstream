#ifndef STAT_DATA_H__
#define STAT_DATA_H__

typedef struct
{
    unsigned cam_fps_;
    unsigned process_fps_;
    double f_process_time_;
    double f_send_time_;
}stat_data_t;

#endif /* end of include guard: STAT_DATA_H__ */
