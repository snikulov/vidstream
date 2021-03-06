#ifndef CORRUPT_INTRO_HPP__
#define CORRUPT_INTRO_HPP__

#include <types.hpp>
#include <vector>

#ifdef BUILD_WITH_ITPP
#include <itpp/itcomm.h>
#endif

#include <cfg/cfg_notify.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/dynamic_bitset.hpp>
#include <boost/random.hpp>

class corrupt_intro : public cfg_notify
{
public:
    corrupt_intro(float e = 0.0f)
        : p_(e), u_(0.0f, 1.0f)
#ifdef BUILD_WITH_ITPP
          , binary_channel_(new itpp::BSC(e))
#endif
    {
    }

    void cfg_changed(const boost::property_tree::ptree& cfg)
    {
        float noise = cfg.get<float>("cfg.error.val");
        boost::mutex::scoped_lock lk(lk_);
        p_ = noise;
#ifdef BUILD_WITH_ITPP
        binary_channel_.reset(new itpp::BSC(p_));
#endif
    }

#ifdef BUILD_WITH_ITPP
    itpp::bvec corrupt(itpp::bvec& signal)
    {
        boost::mutex::scoped_lock lk(lk_);
        return binary_channel_->operator()(signal);
    }
#endif

    std::vector<uint8_t> corrupt(const std::vector<uint8_t>& src)
    {

        boost::mutex::scoped_lock lk(lk_);

        std::vector<uint8_t> res;
        res.reserve(src.size());
        boost::dynamic_bitset<uint8_t> bs(src.begin(), src.end());

        for(size_t i=0; i < bs.size(); ++i)
        {
            if (gen_random_float() <= p_)
            {
                bs.flip(i);
            }
        }

        to_block_range(bs, std::back_inserter(res));

        return res;
    }

private:

    float gen_random_float()
    {
        boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > gen(rng_, u_);
        return gen();
    }

    corrupt_intro(const corrupt_intro&);
    corrupt_intro& operator= (const corrupt_intro&);

    float p_;

    // random number generator
    boost::mt19937 rng_;
    boost::uniform_real<float> u_;

    // BSC channel model from ITPP
    boost::mutex lk_;

#ifdef BUILD_WITH_ITPP
    boost::scoped_ptr<itpp::BSC> binary_channel_;
#endif

};

#endif
