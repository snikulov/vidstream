#ifndef TRANSPORT_HPP__
#define TRANSPORT_HPP__

#include <types.hpp>
#include <ecc/ecc.h>
#include <nn.h>
//#include <pubsub.h>
#include <pipeline.h>

namespace vidstream {

    typedef enum
    {
        TRANSPORT_SEND = NN_PUSH,
        TRANSPORT_RECEIVE = NN_PULL
    }transport_t;

    class transport
    {
    public:
#if defined(BUILD_FOR_LINUX)
        transport(transport_t type, const std::string& url, boost::shared_ptr<ecc> ecc)
            : type_(type), url_(url), ecc_(ecc), socket_(-1), endpoint_(-1)
#else
        transport(transport_t type, const std::string& url)
            : type_(type), url_(url), socket_(-1), endpoint_(-1)
#endif
        {
            std::string err_msg("Error: ");
            socket_   = nn_socket(AF_SP, type);
            if (socket_ < 0)
            {
                throw std::runtime_error(err_msg + nn_strerror(nn_errno()));
            }
            if (type_ == TRANSPORT_RECEIVE)
            {
                endpoint_ = nn_bind(socket_, url_.c_str());
            }
            else if (type == TRANSPORT_SEND)
            {
                endpoint_ = nn_connect(socket_, url_.c_str());
            }
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

        // utility function
        void send_jpeg(jpeg_data_t data, jpeg_rst_idxs_t idxs)
        {
            std::vector<std::size_t>& ridx = *idxs;
            std::vector<unsigned char>& rdata = *data;
#if 0
            std::cout << "send jpeg data size="
                << rdata.size() << " rst_count="
                << ridx.size() << std::endl;

#endif
            const std::string mstart("jpegstart");
            const std::string mend("jpegend");
// send start file marker
            send(mstart.c_str(), mstart.size());

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
                send(p_data, blk_size);
            }
// send end file marker
            send(mend.c_str(), mend.size());
        }


        void send(const char* data, size_t len)
        {
            const char* buf = data;
            size_t buf_len = len;
#if defined(BUILD_FOR_LINUX)
            if (ecc_)
            {
                buf = ecc_->encode(data, len, buf_len);
            }
#endif
            int bytes = nn_send(socket_, buf, buf_len, 0);
            if(bytes < 0)
            {
                // std::cout << "Error: " << nn_strerror(nn_errno()) << std::endl;
            }

#if defined(BUILD_FOR_LINUX)
            if (ecc_)
            {
                free(const_cast<char*>(buf));
            }
#endif
        }

        bool receive(std::vector<unsigned char>& out)
        {
            bool is_error = false;
            int ret_size = 0;
            char * buf = NULL;
            out.clear();

            ret_size = nn_recv(socket_, &buf, NN_MSG, 0);
            out.insert(out.end(), buf, buf+ret_size);
            nn_freemsg(buf);
#if defined(BUILD_FOR_LINUX)
            if (ecc_)
            {
                char * decoded = NULL;
                size_t d_len = 0;
                std::vector<char> v;
                decoded = ecc_->decode(&out[0], out.size(), d_len, v, is_error);

                out.clear();
                out.insert(out.end(), decoded, decoded+d_len);
                free(decoded);
            }
#endif
            return is_error;
        }
    private:
        /* data */
        transport_t type_;
        std::string url_;
#if defined(BUILD_FOR_LINUX)
        boost::shared_ptr<ecc> ecc_;
#endif
        int socket_;
        int endpoint_;

    };
} /* namespace vidstream */

#endif  // TRANSPORT_HPP__
