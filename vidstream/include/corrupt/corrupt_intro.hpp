#ifndef CORRUPT_INTRO_HPP__
#define CORRUPT_INTRO_HPP__

#include <types.hpp>
#include <vector>
#include <corrupt/corrupt.h>

#include <itpp/itcomm.h>

class corrupt_intro : public cfg_notify
{
public:
    corrupt_intro(double e = 0.0)
        : binary_channel_(new itpp::BSC(e))
    {
    }

    void cfg_changed(const boost::property_tree::ptree& cfg)
    {
        double noise = cfg.get<double>("cfg.error.val");
        boost::mutex::scoped_lock lk(lk_);
        binary_channel_.reset(new itpp::BSC(noise));
    }

    itpp::bvec corrupt(itpp::bvec& signal)
    {
        boost::mutex::scoped_lock lk(lk_);
        return binary_channel_->operator()(signal);
    }

private:

    corrupt_intro(const corrupt_intro&) {}
    corrupt_intro& operator= (const corrupt_intro&) { return *this; }

    // BSC channel model from ITPP
    boost::mutex lk_;
    boost::scoped_ptr<itpp::BSC> binary_channel_;

};

#endif
