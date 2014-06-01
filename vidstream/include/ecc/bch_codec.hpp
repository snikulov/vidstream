#ifndef BCH_CODEC_HPP__
#define BCH_CODEC_HPP__

#include "cfg/cfg_notify.hpp"
#if defined(BUILD_FOR_LINUX)
#include <ecc/ecc.h>
#else
    // fake class
    class ecc
    {
    public:
        ecc() {}
        ~ecc() {}
    };
#endif

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

    std::vector<unsigned char> encode(const std::vector<unsigned char>& in)
    {
#if !defined(BUILD_FOR_LINUX)
        return in;
#else
        if (!codec_)
        {
            return in;
        }
        // process data
        size_t olen = 0;
        char * buf = codec_->encode(reinterpret_cast<const char*>(&in[0]), in.size(), olen);
        std::vector<unsigned char> out(buf, buf+olen);
        free(buf);
        return out;
#endif
    }

    std::vector<unsigned char> encode(const char* pch, size_t len)
    {
        std::vector<unsigned char> in(pch, pch+len);
        return encode(in);
    }

    std::vector<unsigned char> encode(const std::string& str)
    {
        std::vector<unsigned char> in(str.begin(), str.end());
        return encode(in);
    }

    std::vector<unsigned char> decode(const std::vector<unsigned char>& in
            ,std::vector<char> &successful, bool &decoded_ok)
    {
#if !defined(BUILD_FOR_LINUX)
        decoded_ok = true;
        return in;
#else
        if (!codec_)
        {
            decoded_ok = true;
            return in;
        }
        // process data
        size_t olen = 0;
        char * buf = decode(reinterpret_cast<const char*>(&in[0])
                ,in.size(), olen, successful, decoded_ok);
        std::vector<unsigned char> out(buf, buf+olen);
        free(buf);
        return out;
#endif
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

