#ifndef CODEC_INT_HPP__
#define CODEC_INT_HPP__

#include <boost/shared_ptr.hpp>

namespace itpp
{
class Channel_Code;
}

class itpp_codec
{
public:
    itpp_codec(int n_=7, int t = 3);
    std::string encode(uint8_t src);
    uint8_t decode(std::string src);
private:
    int n_;
    int t_;
    boost::shared_ptr<itpp::Channel_Code> codec_;
};


#endif

