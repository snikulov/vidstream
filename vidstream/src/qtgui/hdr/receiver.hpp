#ifndef RECEIVER_HPP__
#define RECEIVER_HPP__


#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include <vector>
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>

#include <opencv2/opencv.hpp>

#include <types.hpp>
#include <jpeg/jpeg_builder.hpp>
#include <transport/transport.hpp>

#include <ocv/ocv_output.hpp>
#include <corrupt/corrupt_intro.hpp>
#include <ecc/bch_codec.hpp>



using namespace vidstream;

class receiver
{
public:
    receiver(bool& stop, const std::string& url
            , boost::shared_ptr<corrupt_intro> error
            , boost::shared_ptr<ocv_output> win
            , boost::shared_ptr<jpeg_builder> jb
            , boost::shared_ptr<bch_codec> bch
            )
        : stop_(stop), url_(url), waiting_(false), err_(error), win_(win), jb_(jb)
          , ecc_(bch)
    {
    }

    ~receiver()
    {
    }

    void operator()()
    {
        boost::scoped_ptr<transport> rcv(
                new transport(TRANSPORT_PULL, url_)
                );

        jpeg_data_t rcv_buf(new std::vector<unsigned char>);
        const char * mstart = "jpegstart";
        const char * mend = "jpegend";
        unsigned long img_count = 0;
        while(!stop_)
        {
            waiting_ = true;
            std::vector<unsigned char> buf;
            if (stop_) break;
            rcv->receive(buf);

            // introduce error
            if (err_)
            {
                err_->corrupt(buf);
            }

            if (ecc_)
            {
                std::vector<char> good;
                bool is_ok = false;
                buf = ecc_->decode(buf, good, is_ok);
                if (!is_ok)
                {
                    std::cout << "failed to decode buffer" << std::endl;
                }
            }

            waiting_ = false;

            if(buf.end() != std::search(buf.begin(), buf.end(), mstart, mstart+9))
            {
                // jpeg start
                rcv_buf.reset(new std::vector<unsigned char>());
//                std::cout << "started new image data..." << std::endl;
            }
            else if(buf.end() != std::search(buf.begin(), buf.end(), mend, mend+7))
            {
                // jpeg end
                // need to reassemble
                rcv_buf->push_back(0xff);
                rcv_buf->push_back(0xd9);
                img_count++;
                std::vector<size_t> rst_idxs;
                get_all_rst_blocks(*rcv_buf, rst_idxs);
                std::cout << "img_count = "<< img_count << " rst blocks =" << rst_idxs.size() << std::endl;

                jpeg_data_t jpg = jb_->build_jpeg_from_rst(rcv_buf);
//                jbuilder.write(jpg, img_count);
                cv::Mat m = cv::imdecode(cv::Mat(*jpg), 1);
                if (!m.empty())
                {
                    // good frame, need to store data and rst
                    win_->show(m);
                }
                else
                {
                    std::cout << "m is empty" << std::endl;
                }
            }
            else
            {
                rcv_buf->insert(rcv_buf->end(), buf.begin(), buf.end());
            }
        }
    }

    void stop()
    {
        stop_ = true;
        win_->stop();
        if (waiting_)
        {
            nn_term();
        }
    }

private:
    /* data */
    bool& stop_;
    std::string url_;
    bool waiting_;
    boost::shared_ptr<corrupt_intro> err_;
    boost::shared_ptr<ocv_output> win_;
    boost::shared_ptr<jpeg_builder> jb_;
    boost::shared_ptr<bch_codec> ecc_;
};

#endif

