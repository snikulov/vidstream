#ifndef TRANSPORT_HPP__
#define TRANSPORT_HPP__

#include <types.hpp>
#include <ecc/ecc.h>
#include <nn.h>
#include <pubsub.h>

namespace vidstream {
    class transport
    {
    public:
#if defined(BUILD_FOR_LINUX)
        transport(const std::string& url, boost::shared_ptr<ecc> ecc)
            : url_(url), ecc_(ecc)
#else
        transport(const std::string& url)
            : url_(url)
#endif
        {
            std::string err_msg("Error: ");
            socket_   = nn_socket(AF_SP, NN_PUB);
            if (socket_ < 0)
            {
                throw std::runtime_error(err_msg + nn_strerror(nn_errno()));
            }
            endpoint_ = nn_bind(socket_, url_.c_str());
            if (endpoint_ < 0)
            {
                throw std::runtime_error(err_msg + nn_strerror(nn_errno()));
            }
        }

        ~transport()
        {
            if (endpoint_ >= 0 && socket_ >= 0)
            {
                nn_shutdown(socket_, endpoint_);
                endpoint_ = -1;
            }

            if (socket_ >= 0)
            {
                nn_close(socket_);
                socket_ = -1;
            }
        }

        void send_jpeg(jpeg_data_t data, jpeg_rst_idxs_t idxs)
        {
            std::vector<std::size_t>& ridx = *idxs;
            std::vector<unsigned char>& rdata = *data;

            const std::string mstart("jpegstart");
            const std::string mend("jpegend");

// send start file marker
            send_block(mstart.c_str(), mstart.size());

// send rst blocks
            for(size_t i = 1; i < ridx.size(); i++ )
            {
                size_t blk_size = ridx[i] - ridx[i-1];
                const char * p_data = reinterpret_cast<const char*>(&(rdata[ridx[i-1]]));
                send_block(p_data, blk_size);
            }
// send end file marker
            send_block(mend.c_str(), mend.size());
        }

    private:

        void send_block(const char* data, size_t len)
        {
            const char* buf = data;
            size_t buf_len = len;
#if defined(BUILD_FOR_LINUX)
            if (ecc_)
            {
                buf = ecc_->encode(data, len, buf_len);
            }
#endif
            int bytes = nn_send(socket_, buf, buf_len, NN_DONTWAIT);
            if(bytes < 0)
            {
                std::cout << "Error: " << nn_strerror(nn_errno()) << std::endl;
            }

#if defined(BUILD_FOR_LINUX)
            if (ecc_)
            {
                free(buf);
            }
#endif
        }

        /* data */
        std::string url_;
#if defined(BUILD_FOR_LINUX)
        boost::shared_ptr<ecc> ecc_;
#endif
        int socket_;
        int endpoint_;

    };
} /* namespace vidstream */

#endif  // TRANSPORT_HPP__
