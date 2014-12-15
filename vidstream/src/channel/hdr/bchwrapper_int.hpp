#ifndef BCHWRAPPER_INT_HPP__
#define BCHWRAPPER_INT_HPP__

#include "bch_codec_none.hpp"
#include "bch_codec_itpp.hpp"
#include "bch_codec_kernel.hpp"

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/loglevel.h>

#include <boost/shared_ptr.hpp>
#include <itpp/itcomm.h>
#include <channel/ecc_codec.hpp>


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

    bchwrapper(int n, int t, bch_type_t type = BCH_CODEC_KERNEL)
        : n_(n), t_(t), type_(type), log_(log4cplus::Logger::getInstance("bch"))
    {
        init_bch_codec();
    }

    ~bchwrapper(){}

    void change_params(int n, int t)
    {
        LOG4CPLUS_DEBUG(log_, "bch change params: m=" << n << " t=" << t);

        n_ = n;
        t_ = t;

        init_bch_codec();
    }

    boost::shared_ptr<abstract_ecc_codec> get()
    {
        if (!codec_)
        {
            init_bch_codec();
        }
        return codec_;
    }

    double get_encode_coef()
    {
        boost::mutex::scoped_lock lk(mx_);
        return codec_->get_encode_coef();
    }

private:
    bchwrapper()
        : n_(0), t_(0), type_(BCH_CODEC_NONE)
    {
    }


    boost::shared_ptr<abstract_ecc_codec> create_codec(bch_type_t t)
    {
        boost::shared_ptr<abstract_ecc_codec> ret;
        try
        {
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
        }
        catch(...)
        {
            LOG4CPLUS_ERROR(log_, "Unable to create bch codec with m=" << n_ << " and t=" << t_);
            n_ = 0;
            t_ = 0;
            ret.reset(new bch_codec_none());
        }
        return ret;
    }

    void init_bch_codec()
    {
        if (n_ != 0 && t_ != 0)
        {
            LOG4CPLUS_DEBUG(log_, "created bch codec: m=" << n_ << " t=" << t_);
            boost::mutex::scoped_lock lk(mx_);
            codec_ = create_codec(type_);
        }
        else
        {
            LOG4CPLUS_DEBUG(log_, "created empty bch codec: m=" << n_ << " t=" << t_);
            boost::mutex::scoped_lock lk(mx_);
            codec_ = create_codec(BCH_CODEC_NONE);
        }
    }

    int n_;
    int t_;
    bch_type_t type_;

    log4cplus::Logger log_;

    boost::shared_ptr<abstract_ecc_codec> codec_;
    boost::mutex mx_;

};

#endif // BCHWRAPPER_HPP__

