#ifndef JPEG_TRANSPORT_HPP__
#define JPEG_TRANSPORT_HPP__

#include <types.hpp>
#include <transport/transport.hpp>
#include <ecc/bch_codec.hpp>

namespace vidstream {

class jpeg_transport
{
public:
    static int send_jpeg(jpeg_data_t data, jpeg_rst_idxs_t idxs
            ,boost::shared_ptr<transport> trans
            ,boost::shared_ptr<bch_codec> codec
            )
    {
        int res = 0;
        std::vector<std::size_t>& ridx = *idxs;
        std::vector<unsigned char>& rdata = *data;

        const std::string mstart("jpegstart");
        const std::string mend("jpegend");
        std::vector<unsigned char> out = codec->encode(mstart);
        res = trans->send(out);
// send start file marker
        if (res == -1)
        {
            return res;
        }

// send rst blocks
        size_t ridx_len = ridx.size()-1;
        size_t i = 0;
        size_t blk_size = 0;
        size_t blk_idx = 0;
        const char * p_data = 0;
        for(; i < ridx_len; i++ )
        {
            blk_size = ridx.at(i+1) - ridx.at(i);
            blk_idx = ridx.at(i);
            p_data = reinterpret_cast<const char*>(&(rdata[blk_idx]));

            ;
            res = trans->send(codec->encode(p_data, blk_size));
            if (res == -1)
            {
                return res;
            }

        }
// send end file marker
        out = codec->encode(mend);
        res = trans->send(out);
        return res;
    }


private:
};

} /* namespace vidstream */

#endif



