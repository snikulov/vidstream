#ifndef BCH_CODEC_KERNEL_HPP__
#define BCH_CODEC_KERNEL_HPP__

#include <ecc/ecc.h>

class bch_codec_kernel : public abstract_ecc_codec
{
    public:
        explicit bch_codec_kernel(int n, int t)
        {
            codec_.reset(new ecc(n, t));
        }

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


    private:
        boost::scoped_ptr<ecc> codec_;
};



#endif /* end of include guard: BCH_CODEC_KERNEL_HPP__ */
