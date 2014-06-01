#ifndef BCH_CODEC_HPP__
#define BCH_CODEC_HPP__

#include "cfg/cfg_notify.hpp"
#include <ecc/ecc.h>

class bch_codec : public cfg_notify
{
public:
    bch_codec(int m = 0, int t = 0)
        : m_(m), t_(t)
    {
        codec_.reset(
#if defined(BUILD_FOR_LINUX)
                new ecc(m_, t_)
#endif
                );
    }

    ~bch_codec()
    {
    }

    std::vector<unsigned char> encode(const std::string& str)
    {
        std::vector<unsigned char> in(str.begin(), str.end());
        return encode(in);
    }

    std::vector<unsigned char> encode(const std::vector<unsigned char>& in)
    {
        if (!codec_)
        {
            return in;
        }
        // process data
    }
    std::vector<unsigned char> decode(const std::vector<unsigned char>& in
            ,std::vector<char> &successful, bool &decoded_ok)
    {
        if (!codec_)
        {
            decoded_ok = true;
            return in;
        }
        // process data
    }

    void cfg_changed(const boost::property_tree::ptree& cfg)
    {
        int m = cfg.get<int>("cfg.bch.m");
        int t = cfg.get<int>("cfg.bch.t");
        if (m_ != m || t_ != t)
        {
            // re-init bch TODO: add lock on this operation
            codec_.reset(
#if defined(BUILD_FOR_LINUX)
                    new ecc(m_, t_)
#endif
                    );
        }
    }

private:
    int m_;
    int t_;
    boost::scoped_ptr<ecc> codec_;
};

#endif

