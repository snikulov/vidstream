//
// Sample generic monitor queue for 1:1 data sink
//
#ifndef MONITOR_QUEUE_HPP__
#define MONITOR_QUEUE_HPP__

#include <queue>
#include <boost/thread.hpp>
#include <boost/noncopyable.hpp>

template<typename element>
class monitor_queue : private boost::noncopyable
{
    public:
        monitor_queue(std::size_t max_size = 100)
            : max_size_(max_size)
        {
        }

        void enqueue(element d) {
            boost::mutex::scoped_lock lock(mx_);
            if (q_.size() < max_size_)
            {
                q_.push(d);
            } // else silently skip for now
            cond_.notify_one();
        }

        element dequeue()
        {
            boost::mutex::scoped_lock lock(mx_);
            if(q_.empty()) {
                cond_.wait(lock); // unlock mutex and wait for data
            }
            element ret_val(q_.front());
            q_.pop();
            return ret_val;
        }

        bool empty() const
        {
            boost::mutex::scoped_lock lock(mx_);
            return q_.empty();
        }

    private:
        std::size_t               max_size_;
        boost::mutex              mx_;
        boost::condition_variable cond_;
        std::queue<element>       q_;

};
#endif


