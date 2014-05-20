#ifndef TRANSPORT_HPP__
#define TRANSPORT_HPP__

#include <types.hpp>
#include <ecc/ecc.h>

#include <nanopp/nn.hpp>

namespace vidstream {

    typedef enum
    {
        TRANSPORT_PUSH = NN_PUSH,
        TRANSPORT_PULL = NN_PULL,
        TRANSPORT_REQ = NN_REQ,
        TRANSPORT_REP = NN_REP
    }transport_t;

    class transport
    {
    public:
#if defined(BUILD_FOR_LINUX)
        transport(transport_t type, const std::string& url, boost::shared_ptr<ecc> ecc)
            : type_(type), url_(url), ecc_(ecc), socket_(AF_SP, type_)
        {
            init_socket();
        }
#endif
        transport(transport_t type, const std::string& url)
            : type_(type), url_(url), socket_(AF_SP, type_)
        {
            init_socket();
        }

        ~transport()
        {
        }

        // utility function
        int send_jpeg(jpeg_data_t data, jpeg_rst_idxs_t idxs)
        {
            int res = 0;
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
            res = send(mstart.c_str(), mstart.size());
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

                res = send(p_data, blk_size);
                if (res == -1)
                {
                    return res;
                }

            }
// send end file marker
            return send(mend.c_str(), mend.size());
        }

        int send(const std::string& data)
        {
            return send(data.c_str(), data.size());
        }

        int send(const char* data, size_t len)
        {
            int bytes = 0;
            const char* buf = data;
            size_t buf_len = len;
#if defined(BUILD_FOR_LINUX)
            if (ecc_)
            {
                buf = ecc_->encode(data, len, buf_len);
            }
#endif
            bytes = socket_.send(buf, buf_len, 0);

#if defined(BUILD_FOR_LINUX)
            if (ecc_)
            {
                free(const_cast<char*>(buf));
            }
#endif
            return bytes;
        }


        int receive(std::vector<unsigned char>& out)
        {
            int bytes = 0;
            int ret_size = 0;
            char * buf = NULL;
            out.clear();

            bytes = socket_.recv(&buf, NN_MSG, 0);

            if (bytes != -1)
            {
                out.insert(out.end(), buf, buf+bytes);
                nn::freemsg(buf);

    #if defined(BUILD_FOR_LINUX)
                if (ecc_)
                {
                    char * decoded = NULL;
                    size_t d_len = 0;
                    bool is_error = false;
                    std::vector<char> v;
                    decoded = ecc_->decode(&out[0], out.size(), d_len, v, is_error);

                    out.clear();
                    out.insert(out.end(), decoded, decoded+d_len);
                    free(decoded);
                }
    #endif
            }

            return bytes;
        }
    private:

        void init_socket()
        {
            int opt = 250; // ms
            socket_.setsockopt(NN_SOL_SOCKET, NN_SNDTIMEO, &opt, sizeof (opt));
            socket_.setsockopt(NN_SOL_SOCKET, NN_RCVTIMEO, &opt, sizeof (opt));

            if (type_ == TRANSPORT_PULL || type_ == TRANSPORT_REP)
            {
                socket_.bind(url_.c_str());
            }
            else if (type_ == TRANSPORT_PUSH || type_ == TRANSPORT_REQ)
            {
                socket_.connect(url_.c_str());
            }
        }

        /* data */
        transport_t type_;
        std::string url_;
#if defined(BUILD_FOR_LINUX)
        boost::shared_ptr<ecc> ecc_;
#endif
        nn::socket socket_;
    };
} /* namespace vidstream */

#endif  // TRANSPORT_HPP__
