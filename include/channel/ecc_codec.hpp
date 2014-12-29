#ifndef ECC_CODEC_HPP__
#define ECC_CODEC_HPP__

#include <vector>

class abstract_ecc_codec
{
    public:
        virtual ~abstract_ecc_codec()
        {}

        virtual bool encode(uint8_t src, std::vector<uint8_t>& dst) const = 0;
        virtual bool encode(const std::vector<uint8_t>& src, std::vector<uint8_t>& dst) const = 0;
        virtual bool decode(const std::vector<uint8_t>& src, std::vector<uint8_t>& dst) const = 0;

        virtual double get_encode_coef() = 0;
};


#endif /* end of include guard: ECC_CODEC_HPP__ */
