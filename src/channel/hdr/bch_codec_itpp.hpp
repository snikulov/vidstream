#ifndef BCH_CODEC_ITPP_HPP__
#define BCH_CODEC_ITPP_HPP__

#include <channel/ecc_codec.hpp>
#include <itpp/itcomm.h>
#include <boost/scoped_ptr.hpp>

class bch_codec_itpp : public abstract_ecc_codec
{
    public:

        explicit bch_codec_itpp(int n, int t)
        {
            codec_.reset(new itpp::BCH(n, t));
        }

        ~bch_codec_itpp() {}

        bool encode(const std::vector<uint8_t>& src, std::vector<uint8_t>& dst) const
        {
            return true;
        }

        bool encode(uint8_t src, std::vector<uint8_t>& dst) const
        {
            return true;
        }

        bool decode(const std::vector<uint8_t>& src, std::vector<uint8_t>& dst) const
        {
            return true;
        }

        double get_encode_coef()
        {
            return 1.0;
        }



    private:
        boost::scoped_ptr<itpp::BCH> codec_;
};


#endif /* end of include guard: BCH_CODEC_ITPP_HPP__ */
