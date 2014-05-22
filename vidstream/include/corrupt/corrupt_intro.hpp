#ifndef CORRUPT_INTRO_HPP__
#define CORRUPT_INTRO_HPP__

#include <types.hpp>
#include <vector>
#include <corrupt/corrupt.h>

class corrupt_intro : public cfg_notify
{
public:
    corrupt_intro(double e = 0.0)
        : err_val_(e)
    {
    }

    void cfg_changed(const boost::property_tree::ptree& cfg)
    {
        err_val_ = cfg.get<double>("cfg.error.val");
    }

    void corrupt(std::vector<uint8_t>& data)
    {
        corr_.add_error(data, err_val_);
    }

private:

    corrupt_intro(const corrupt_intro&) {}
    corrupt_intro& operator= (const corrupt_intro&) {}

    corruptor corr_;
    double err_val_;
};

#endif
