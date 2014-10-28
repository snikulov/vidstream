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


    int send_jpeg(jpeg_data_t data, jpeg_rst_idxs_t idxs
        , boost::shared_ptr<out_channel> outsink)
    {
        int res = 0;
        std::vector<std::size_t>& ridx = *idxs;
        std::vector<unsigned char>& rdata = *data;

        outsink->put(start_mark_);

        // send rst blocks
        size_t ridx_len = ridx.size() - 1;
        for (size_t i = 0; i < ridx_len; i++)
        {
            // send only data, without RST mark
            std::vector<unsigned char> rst_blk(&rdata[ridx.at(i) + 2], &rdata[ridx.at(i + 1)]);

            outsink->put(rst_blk);
        }

        return res;
    }

#if 0
    int send_jpeg(jpeg_data_t data, jpeg_rst_idxs_t idxs
            ,boost::shared_ptr<transport> trans
            ,boost::shared_ptr<bch_codec> codec
            )
    {
	trans->start_frame();
        int res = 0;
        std::vector<std::size_t>& ridx = *idxs;
        std::vector<unsigned char>& rdata = *data;

        res = trans->send(codec->encode(start_mark_));
// send start file marker
        if (res == -1)
        {
            return res;
        }

// send rst blocks
        size_t ridx_len = ridx.size()-1;
        for(size_t i = 0; i < ridx_len; i++ )
        {
            // send only data, without RST mark
            std::vector<unsigned char> rst_blk(&rdata[ridx.at(i)+2], &rdata[ridx.at(i+1)]);
            // put terminal RSTn code for end of block indication
//            rst_blk.push_back(rst_blk.at(0));
//            rst_blk.push_back(rst_blk.at(1));

            res = trans->send(codec->encode(rst_blk));

            if (res == -1)
            {
                return res;
            }
        }
// send end file marker
//        res = trans->send(codec->encode(end_mark_));
        return res;
    }
#endif

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



