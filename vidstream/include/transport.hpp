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
        transport(const std::string& url, boost::shared_ptr<ecc> ecc)
            : url_(url), ecc_(ecc)
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

        void send(jpeg_data_t data, jpeg_rst_idxs_t idxs)
        {
            std::vector<std::size_t>& ridx = *idxs;
            std::vector<unsigned char>& rdata = *data;

            const std::string mstart("jpegstart");
            const std::string mend("jpegend");

// send start file marker
            encoded_send(mstart.c_str(), mstart.size());

// send rst blocks
            for(size_t i = 1; i < ridx.size(); i++ )
            {
                size_t blk_size = ridx[i] - ridx[i-1];
                const char * p_data = reinterpret_cast<const char*>(&(rdata[ridx[i-1]]));
                encoded_send(p_data, blk_size);
            }
// send end file marker
            encoded_send(mend.c_str(), mend.size());
        }

    private:

        void encoded_send(const char* data, size_t len)
        {
            char* buf;
            size_t buf_len;

            buf = ecc_->encode(data, len, buf_len);

            int bytes = nn_send(socket_, buf, buf_len, NN_DONTWAIT);
            if(bytes < 0)
            {
                std::cout << "Error: " << nn_strerror(nn_errno()) << std::endl;
            }
            free(buf);

        }

        /* data */
        std::string url_;
        boost::shared_ptr<ecc> ecc_;
        int socket_;
        int endpoint_;

    };
} /* namespace vidstream */

#endif  // TRANSPORT_HPP__
