#ifndef CFG_SUBSCRIBERS_HPP__
#define CFG_SUBSCRIBERS_HPP__

#include "cfg/cfg_notify.hpp"
#include <boost/thread.hpp>

class cfg_subscribers : private boost::noncopyable
{
public:
    cfg_subscribers() {}
    ~cfg_subscribers() {}

    bool subscribe(cfg_notify* listener)
    {
        boost::mutex::scoped_lock lk(mx_);
        for(size_t i = 0; i < subs_.size(); ++i)
        {
            if (subs_[i] == listener) return false;
        }
        subs_.push_back(listener);
        return true;
    }

    void notify_change(const boost::property_tree::ptree& cfg)
    {
        boost::mutex::scoped_lock lk(mx_);
        for(size_t i = 0; i < subs_.size(); ++i)
        {
            subs_[i]->cfg_changed(cfg);
        }
    }

private:
    /* data */
    std::vector<cfg_notify*> subs_;
    boost::mutex mx_;
};

#endif

