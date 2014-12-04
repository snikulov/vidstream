#ifndef STAT_DATA_H__
#define STAT_DATA_H__

struct stat_data_t
{
    stat_data_t()
        : cam_fps_(0), process_fps_(0)
          , f_process_time_(0.0), f_send_time_(0.0)
          , bytes_sent_(0), frames_sent_(0)
    {}

    ~stat_data_t()
    {}

    unsigned cam_fps_;
    unsigned process_fps_;
    double f_process_time_;
    double f_send_time_;

    unsigned long bytes_sent_;
    unsigned frames_sent_;

    unsigned long tsec_;

    unsigned long frame_size_;
    unsigned long num_rst_;
};

#endif /* end of include guard: STAT_DATA_H__ */
