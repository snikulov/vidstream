#ifndef BCHWRAPPER_INT_HPP__
#define BCHWRAPPER_INT_HPP__

#include <boost/shared_ptr.hpp>
#include <itpp/itcomm.h>
#include <channel/ecc_codec.hpp>

#include "bch_codec_none.hpp"
#include "bch_codec_itpp.hpp"
#include "bch_codec_kernel.hpp"

class bchwrapper : private boost::noncopyable
{
public:
    typedef enum
    {
        BCH_CODEC_DEFAULT = 0,
        BCH_CODEC_NONE = BCH_CODEC_DEFAULT,
        BCH_CODEC_ITPP,
        BCH_CODEC_KERNEL,
        BCH_CODEC_MAX
    }bch_type_t;

    bchwrapper()
        : n_(0), t_(0), type_(BCH_CODEC_NONE)
    {
    }

    bchwrapper(int n, int t, bch_type_t type = BCH_CODEC_KERNEL)
        : n_(n), t_(t), type_(type)
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

    boost::shared_ptr<abstract_ecc_codec> get()
    {
#if defined(CHANNEL_DEBUG)
        std::cerr << "[I] get bch with n_=" << n_ << " t_=" << t_ << std::endl;
#endif
        if (!codec_)
        {
            init_bch_codec();
        }

        return codec_;
    }

private:

    boost::shared_ptr<abstract_ecc_codec> create_codec(bch_type_t t)
    {
        boost::shared_ptr<abstract_ecc_codec> ret;

        switch(t)
        {
            case BCH_CODEC_ITPP:
                ret.reset(new bch_codec_itpp(n_, t_));
                break;
            case BCH_CODEC_KERNEL:
                ret.reset(new bch_codec_kernel(n_, t_));
                break;
            default:
                ret.reset(new bch_codec_none());
                break;
        }
        return ret;
    }

    void init_bch_codec()
    {
#if defined(CHANNEL_DEBUG)
        std::cerr << "[I] inited bch with n_=" << n_ << " t_=" << t_ << std::endl;
#endif
        boost::mutex::scoped_lock lk(mx_);
        if (n_ != 0 && t_ != 0 && type_ != BCH_CODEC_NONE)
        {
            codec_ = create_codec(type_);
        }
        else
        {
            codec_ = create_codec(BCH_CODEC_NONE);
        }
    }

    int n_;
    int t_;
    bch_type_t type_;

    boost::shared_ptr<abstract_ecc_codec> codec_;
    boost::mutex mx_;

};

#endif // BCHWRAPPER_HPP__

