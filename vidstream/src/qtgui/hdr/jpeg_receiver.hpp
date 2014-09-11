#ifndef JPEG_RECEIVER_HPP__
#define JPEG_RECEIVER_HPP__


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
#include <jpeg/jpeg_transport.hpp>
#include <jpeg/jpeg_history.hpp>
#include <jpeg/jpeg_rcv_stm.hpp>
#include <transport/transport.hpp>

#include <ocv/ocv_output.hpp>
#include <corrupt/corrupt_intro.hpp>
#include <ecc/bch_codec.hpp>



using namespace vidstream;

int MyErrorHandler(int status, const char* func_name, const char* err_msg, const char* file_name, int line, void*)
{
    std::cerr << "Woohoo, my own custom error handler" << std::endl;
    return 0;
} 

class jpeg_receiver
{
public:
    jpeg_receiver(bool& stop, const std::string& url
            , boost::shared_ptr<corrupt_intro> error
            , boost::shared_ptr<jpeg_builder> jb
            , boost::shared_ptr<bch_codec> bch
            )
        : stop_(stop), url_(url), waiting_(false), err_(error), jb_(jb)
          , ecc_(bch)
    {
      cv::namedWindow("received");
    }

    ~jpeg_receiver()
    {
    }

    void operator()()
    {

        boost::shared_ptr<jpeg_history> history(new jpeg_history(jb_));

        boost::scoped_ptr<transport> rcv(
                new transport(TRANSPORT_PULL, url_)
                );

        jpeg_transport jt;
        const std::vector<unsigned char>& s_mark = jt.start_mark();
        const std::vector<unsigned char>& e_mark = jt.end_mark();
        jpeg_rcv_stm stm(jb_, history, s_mark, e_mark);

        jpeg_data_t rcv_buf(new std::vector<unsigned char>);
        unsigned long img_count = 0;
        size_t rst_num = 0;
        while(!stop_)
        {
            waiting_ = true;
            std::vector<unsigned char> buf;
            if (stop_) break;
            int bytes = rcv->receive(buf);

            if (bytes < 0)
            {
                // no data available - try again
                continue;
            }
            // introduce error
            if (err_)
            {
                err_->corrupt(buf);
            }
            if (ecc_)
            {
                std::vector<char> good;
                bool is_ok = false;
                std::vector<unsigned char> dec = ecc_->decode(buf, good, is_ok);
		// hack TODO: try to decode everytime
		is_ok = true;
                if (is_ok)
                {
                    buf.swap(dec);
                }
                else
                {
                    std::cout << "failed to decode buffer" << std::endl;

                    // TODO: try recovery mode here...
                    continue;
                }
            }
            waiting_ = false;

            stm.process(buf);
            
            if (stm.has_data())
            {
                jpeg_data_t jpg = stm.get_jpeg();
                jpeg_rst_idxs_t rsts = jb_->rst_idxs(jpg);
                std::cerr << "stm jpeg: rst count=" << std::dec << rsts->size() << std::endl;
                std::vector<unsigned char>& rjpg = *jpg;
                for(size_t i = 0; i < rsts->size(); ++i)
                {
                    size_t idx = (*rsts)[i];
                    unsigned char val1 = rjpg[idx];
                    unsigned char val2 = rjpg[idx+1];
                    if (val1 != 0xff || ((val2 & 0x0f) != (i % 8)))
                    {
                        std::cerr << "i=" << i
                            << " rst=0x" << std::hex
                            << int(val1) << int(val2) << std::endl;
                    }
                }
		// write rebuilt frame
		static int nn = 0;
                jb_->write(jpg,nn);
                
		// TODO: NEED TO INVESTIGATE!!!
                //cvSetErrMode(CV_ErrModeParent);
		//cvRedirectError(MyErrorHandler);
		//cv::Mat imgbuf = cv::Mat(1, jpg->size(), CV_8U, &((*jpg)[0]));
		//cv::Mat m = cv::imdecode(imgbuf, CV_LOAD_IMAGE_COLOR);
		
                //cv::Mat m = cv::imdecode(cv::Mat(*jpg), CV_LOAD_IMAGE_COLOR);
		cv::Mat m = cv::imread("img00000000.jpg", cv::IMREAD_COLOR);
		
                if (!m.empty())
                {
                    std::cerr << "new frame" << std::endl;

		  // probably good frame, store history
                    history->put(jpg);
                    cv::imshow("received", m);
                    cv::waitKey(10);
                }
            }
        }
    }

    void stop()
    {
        stop_ = true;
    }

private:
    /* data */
    bool& stop_;
    std::string url_;
    bool waiting_;
    boost::shared_ptr<corrupt_intro> err_;
    boost::shared_ptr<jpeg_builder> jb_;
    boost::shared_ptr<bch_codec> ecc_;
};

#endif

