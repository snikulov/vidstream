#ifndef JPEG_HISTORY_HPP__
#define JPEG_HISTORY_HPP__

#include <types.hpp>
#include <jpeg/jpeg_builder.hpp>

using vidstream::jpeg_builder;
using vidstream::jpeg_data_t;

class jpeg_history
{
public:
    jpeg_history(boost::shared_ptr<jpeg_builder> jb, size_t msize = 3)
        : jb_(jb), max_size_(msize)
    {
    }

    // return good frame
    jpeg_data_t get()
    {
        if (!good_frames_.empty())
        {
            return good_frames_.front();
        }
        return jb_->get_etalon_jpeg();
    }

    void put(jpeg_data_t frame)
    {
        if (good_frames_.size() == max_size_)
        {
            good_frames_.pop();
        }
        good_frames_.push(frame);
    }

    ~jpeg_history()
    {
    }

private:
    /* data */
    boost::shared_ptr<jpeg_builder> jb_;
    std::queue<jpeg_data_t> good_frames_;
    size_t max_size_;
};

#endif /* end of include guard: JPEG_HISTORY_HPP__ */
