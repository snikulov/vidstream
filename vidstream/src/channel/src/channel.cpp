#include "channel_int.hpp"

channel::channel()
{
    is_running_ = true;
    wt_ = boost::thread(boost::bind(&channel::worker, this));
}

channel::~channel()
{
    is_running_ = false;
    cond_.notify_one();
    wt_.join();
}

void channel::worker()
{
    while (is_running_)
    {
        {
            boost::mutex::scoped_lock lock(mx_);
            if (data_.empty())
            {
                cond_.wait(lock);
            }
        }
        // else send out data with some encoding on bytes
    }
}

void channel::put(const std::vector<uint8_t>& src)
{
    boost::mutex::scoped_lock lock(mx_);
    data_.insert(data_.end(), src.begin(), src.end());
    cond_.notify_one();
}

void channel::get(std::vector<uint8_t>& dst)
{
    boost::mutex::scoped_lock lock(mx_);
    dst.clear();
    dst.reserve(data_.size());
    dst.insert(dst.begin(), data_.begin(), data_.end());
}