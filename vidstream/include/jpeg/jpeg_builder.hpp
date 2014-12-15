#ifndef JPEG_BUILDER_HPP__
#define JPEG_BUILDER_HPP__

//
// Stateless builder for jpeg memory buffer with specific parameters
//
#include <iomanip>
#include <fstream>
#include <sstream>

#include <boost/thread/mutex.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <frame.hpp>
#include <types.hpp>

#include <split/split.h>
#include <cfg/cfg_notify.hpp>

namespace vidstream
{


// TODO: add interface to access jpeg parameters
// - get/set

class jpeg_builder : public cfg_notify
{
public:
    jpeg_builder(int quality=100, int rst_interval=1)
        : quality_(quality), rst_(rst_interval)
        , csize_(cv::Size(640, 480)), bw_(false), num_of_rst_(0)
    {
        params_.push_back(cv::IMWRITE_JPEG_QUALITY);
        params_.push_back(quality_);
        params_.push_back(cv::IMWRITE_JPEG_RST_INTERVAL);
        params_.push_back(rst_);
    }

    ~jpeg_builder()
    {
    }

    jpeg_rst_idxs_t rst_idxs(jpeg_data_t data)
    {
        jpeg_rst_idxs_t ret_val(new std::vector<std::size_t>);
        if (data && !data->empty())
        {
            (void)get_all_rst_blocks(*data, *ret_val);
        }
        return ret_val;
    }

    jpeg_data_t from_cvmat(const camera_frame_t frame)
    {
        jpeg_data_t ret_buf(new std::vector<unsigned char>());
        if (frame && !frame->empty())
        {
            boost::mutex::scoped_lock lk(mx_);
            bool res = cv::imencode(".jpg", *frame, *ret_buf, params_);
            // TODO: need to decide what must I do in this case
        }
        return ret_buf;
    }

    std::vector<int> get_params() const
    {
        boost::mutex::scoped_lock lk(mx_);
        return params_;
    }

    // TODO: should it be in debug only?
    // write to file named "img[num].jpg"
    static void write(const jpeg_data_t data, unsigned long num)
    {
        std::ostringstream fn;
        fn << "img" << std::setfill ('0') << std::setw(8)
           << num << ".jpg";
        std::ofstream of(fn.str().c_str(), std::ios_base::binary);
        const char * p_buf = reinterpret_cast<const char*>(&((*data)[0]));
        of.write(p_buf, data->size()*sizeof(unsigned char));
        of.close();
    }

    static jpeg_data_t read(const std::string& fname)
    {
        jpeg_data_t ret_val;
        std::ifstream in(fname.c_str(), std::ios::binary);
        if (in)
        {
            in.unsetf(std::ios::skipws);
            std::istream_iterator<unsigned char> start(in), end;
            ret_val.reset(new std::vector<unsigned char>(start, end));
        }
        return ret_val;
    }

    jpeg_data_t get_etalon_jpeg()
    {
        camera_frame_t frame;
        {
            boost::mutex::scoped_lock lk(mx_);
            frame.reset(new cv::Mat(csize_, bw_ ? CV_8UC1 : CV_8UC3, cv::Scalar::all(0)));
        }
        return from_cvmat(frame);
    }

    size_t get_rst_num()
    {
        if (!num_of_rst_)
        {
            std::vector<size_t> rst_idx;
            (void)get_all_rst_blocks(*get_etalon_jpeg(), rst_idx);
            num_of_rst_ = rst_idx.size();
        }
        return num_of_rst_;
    }

    jpeg_data_t build_jpeg_from_rst(jpeg_data_t jpeg_rst)
    {
        // create jpeg with current params
        jpeg_data_t dst = get_etalon_jpeg();
        std::vector<size_t> dst_idxs;
        (void)get_all_rst_blocks(*dst, dst_idxs);
        dst->erase(dst->begin()+dst_idxs[0], dst->end());
        dst->insert(dst->end(),jpeg_rst->begin(), jpeg_rst->end());

        // add missing terminator
        dst->push_back(0xFF);
        dst->push_back(0xD9);
        return dst;
    }

    void cfg_changed(const boost::property_tree::ptree& cfg)
    {
        boost::mutex::scoped_lock lk(mx_);

        int w       = cfg.get<int>("cfg.img.width");
        int h       = cfg.get<int>("cfg.img.height");
        bool bw     = cfg.get<bool>("cfg.img.bw");
        int quality = cfg.get<int>("cfg.img.q");
        int rst     = cfg.get<int>("cfg.img.rst");

        cv::Size tmps(w, h);

        if (csize_ != tmps)
        {
            csize_ = tmps;
            num_of_rst_ = 0;
        }

        if (bw_ != bw)
        {
            bw_ = bw;
        }

        update_jpeg_encoder_params(quality, rst);

    }

    int get_quality() const
    {
        boost::mutex::scoped_lock lk(mx_);
        return quality_;
    }

    void set_quality(int quality)
    {
        boost::mutex::scoped_lock lk(mx_);
        update_jpeg_encoder_params(quality, rst_);
    }

private:

    void update_jpeg_encoder_params(int q, int r)
    {

        // in-place changes...
        if (quality_ != q)
        {
            params_[1] = quality_ = q;
        }
        if (rst_ != r)
        {
            params_[3] = rst_     = r;
        }
    }

    /* data */
    int quality_;
    int rst_;
    std::vector<int> params_;
    cv::Size csize_;
    bool bw_;
    mutable boost::mutex mx_;
    size_t num_of_rst_;
};
} /* namespace vidstream */
#endif // JPEG_BUILDER_HPP__
