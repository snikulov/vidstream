#ifndef BCH_CODEC_NONE_HPP__
#define BCH_CODEC_NONE_HPP__

#include <channel/ecc_codec.hpp>

class bch_codec_none : public abstract_ecc_codec
{
    public:
        bch_codec_none()
        {
        }

        bool encode(const std::vector<uint8_t>& src, std::vector<uint8_t>& dst) const
        {
            dst = src;
            return true;
        }

        bool encode(uint8_t src, std::vector<uint8_t>& dst) const
        {
            std::vector<uint8_t> tmp;
            tmp.push_back(src);
            dst.swap(tmp);
            return true;
        }

        bool decode(const std::vector<uint8_t>& src, std::vector<uint8_t>& dst) const
        {
            dst = src;
            return true;
        }

        double get_encode_coef()
        {
            return 1.0;
        }
};


#endif /* end of include guard: BCH_CODEC_NONE_HPP__ */
