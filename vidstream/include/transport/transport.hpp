#ifndef TRANSPORT_HPP__
#define TRANSPORT_HPP__

#include <types.hpp>
#include <corrupt/corrupt_intro.hpp>
#include <nanopp/nn.hpp>

namespace vidstream
{

typedef enum
{
    TRANSPORT_PUSH = NN_PUSH,
    TRANSPORT_PULL = NN_PULL,
    TRANSPORT_REQ = NN_REQ,
    TRANSPORT_REP = NN_REP
} transport_t;

class transport
{
public:
    transport(transport_t type, const std::string& url)
        : type_(type), url_(url), socket_(AF_SP, type_), frame_size_(0)
    {
        init_socket();
    }

    ~transport()
    {
    }

    int start_frame()
    {
      std::cout << "Frame bytes: " << frame_size_ << std::endl;
      frame_size_ = 0;
      return frame_size_;
    }

    int send(const std::vector<unsigned char>& data)
    {
        return send(reinterpret_cast<const char*>(&data[0]), data.size());
    }

    int send(const std::string& data)
    {
        return send(data.c_str(), data.size());
    }

    int send(const char* data, size_t len)
    {
        int bytes = 0;
        const char* buf = data;
        bytes = socket_.send(buf, len, 0);
	frame_size_ += bytes;
        return bytes;
    }


    int receive(std::vector<unsigned char>& out)
    {
        int bytes = 0;
        char * buf = NULL;
        out.clear();

        bytes = socket_.recv(&buf, NN_MSG, 0);

        if (bytes != -1)
        {
            out.insert(out.end(), buf, buf+bytes);
            nn::freemsg(buf);
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
    nn::socket socket_;
    int frame_size_;
};
} /* namespace vidstream */

#endif  // TRANSPORT_HPP__
