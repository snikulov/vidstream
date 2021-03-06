#ifndef BCH_CODEC_KERNEL_HPP__
#define BCH_CODEC_KERNEL_HPP__

#include <boost/scoped_ptr.hpp>
#include <ecc/ecc.h>

class bch_codec_kernel : public abstract_ecc_codec
{
    public:
        explicit bch_codec_kernel(int n, int t)
            : encode_coef_(1.0)
        {
            codec_.reset(new ecc(n, t));
        }

        bool encode(const std::vector<uint8_t>& src, std::vector<uint8_t>& dst) const
        {
            size_t encoded_len = 0;
            size_t orig_len = src.size();

            char * encoded = codec_->encode( reinterpret_cast<const char*>(&src[0]), src.size(), encoded_len );

            std::vector<uint8_t> res(encoded, encoded + encoded_len);
            dst.swap(res);
            free(encoded);

            encode_coef_ = static_cast<double>(encoded_len)/static_cast<double>(orig_len);
            return true;
        }

        bool encode(uint8_t src, std::vector<uint8_t>& dst) const
        {
            std::vector<uint8_t> vsrc;
            vsrc.push_back(src);
            return encode(vsrc, dst);
        }

        bool decode(const std::vector<uint8_t>& src, std::vector<uint8_t>& dst) const
        {
            bool dok = false;
            size_t decoded_len = 0;
            std::vector<char> good_data;
            char * decoded = codec_->decode( &src[0], src.size(), decoded_len, good_data, dok);

            std::vector<uint8_t> res(decoded, decoded + decoded_len);
            dst.swap(res);

            free(decoded);
            return dok;
        }

        double get_encode_coef()
        {
            return encode_coef_;
        }

    private:
        boost::scoped_ptr<ecc> codec_;
        mutable double encode_coef_;
};



#endif /* end of include guard: BCH_CODEC_KERNEL_HPP__ */
