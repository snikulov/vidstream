#ifndef JPEG_TRANSPORT_HPP__
#define JPEG_TRANSPORT_HPP__

#include <types.hpp>
#include <transport/transport.hpp>

#include <channel/out_channel.hpp>

namespace vidstream {

class jpeg_transport
{
public:
    jpeg_transport()
    {
#if 0
        start_mark_.push_back(0xff);
        start_mark_.push_back(0xd8);
        end_mark_.push_back(0xff);
        end_mark_.push_back(0xd9);
#endif
        start_mark_.push_back(0xff);
        start_mark_.push_back(0xff);
        start_mark_.push_back(0xff);
        start_mark_.push_back(0xff);
    }

    ~jpeg_transport()
    {
    }


    size_t send_jpeg(jpeg_data_t data, jpeg_rst_idxs_t idxs
        , boost::shared_ptr<out_channel> outsink)
    {
        std::vector<std::size_t>& ridx = *idxs;
        std::vector<unsigned char>& rdata = *data;

        std::vector<uint8_t> outdata;
        outdata.reserve(rdata.size());

        outdata.insert(outdata.end(), start_mark_.begin(), start_mark_.end());
        outdata.insert(outdata.end(), rdata.begin()+ridx[0], rdata.begin()+ridx[ridx.size()-1]);
        outdata.insert(outdata.end(), start_mark_.begin(), start_mark_.end());

        outsink->put(outdata);

        // here the actual data which will be sent over the wire
        return outdata.size();
    }



    const std::vector<unsigned char>& start_mark() const
    {
        return start_mark_;
    }
    const std::vector<unsigned char>& end_mark() const
    {
        return end_mark_;
    }

private:
    std::vector<unsigned char> start_mark_;
    std::vector<unsigned char> end_mark_;
};

} /* namespace vidstream */

#endif



