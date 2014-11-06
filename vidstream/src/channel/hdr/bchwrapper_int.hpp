#ifndef BCHWRAPPER_INT_HPP__
#define BCHWRAPPER_INT_HPP__

#include <boost/shared_ptr.hpp>
#include <itpp/itcomm.h>

class bchwrapper : private boost::noncopyable
{
public:
    bchwrapper()
        : n_(0), t_(0)
    {
    }

    bchwrapper(int n, int t)
        : n_(n), t_(t)
    {
    }

    ~bchwrapper(){}

    void change_params(int n, int t)
    {
        n_ = n;
        t_ = t;
        // add validation
        init_bch_codec();
    }

    boost::shared_ptr<itpp::Channel_Code> get()
    {
#if defined(CHANNEL_DEBUG)
        std::cerr << "[I] get bch with n_=" << n_ << " t_=" << t_ << std::endl;
#endif
        boost::mutex::scoped_lock lk(mx_);
        if (n_ != 0 && t_ != 0 && !codec_)
        {
            codec_ = boost::shared_ptr<itpp::BCH>(new itpp::BCH(n_, t_));
        }
        return codec_;
    }

private:

    void init_bch_codec()
    {
#if defined(CHANNEL_DEBUG)
        std::cerr << "[I] inited bch with n_=" << n_ << " t_=" << t_ << std::endl;
#endif
        boost::mutex::scoped_lock lk(mx_);
        if (n_ != 0 && t_ != 0)
        {
            codec_ = boost::shared_ptr<itpp::BCH>(new itpp::BCH(n_, t_));
        }
        else
        {
            codec_.reset();
        }
    }

    int n_;
    int t_;

    boost::shared_ptr<itpp::Channel_Code> codec_;
    boost::mutex mx_;

};

#endif // BCHWRAPPER_HPP__

