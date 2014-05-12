#ifndef TRANSPORT_HPP__
#define TRANSPORT_HPP__

#include <types.hpp>
#include <nn.h>
#include <pubsub.h>

namespace vidstream {
    class transport
    {
    public:
        transport(const std::string& url)
            : url_(url)
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
            (void)nn_send(socket_, mstart.c_str(), mstart.size(), NN_DONTWAIT);
// send rst blocks
            for(size_t i = 1; i < ridx.size(); i++ )
            {
                size_t blk_size = ridx[i] - ridx[i-1];
                const char * p_data = reinterpret_cast<const char*>(&(rdata[ridx[i-1]]));
                int bytes = nn_send(socket_, p_data, blk_size, NN_DONTWAIT);
                if(bytes < 0)
                {
                    std::cout << "Error: " << nn_strerror(nn_errno()) << std::endl;
                }
            }
// send end file marker
            (void)nn_send(socket_, mend.c_str(), mend.size(), NN_DONTWAIT);
        }

    private:
        /* data */
        std::string url_;
        int socket_;
        int endpoint_;

    };
} /* namespace vidstream */

#endif  // TRANSPORT_HPP__
