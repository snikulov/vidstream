#ifndef FRAME_HPP__
#define FRAME_HPP__
#include <boost/shared_ptr.hpp>

class cv::Mat;
namespace vidstream
{

typedef boost::shared_ptr<cv::Mat> camera_frame_t;

typedef boost::shared_ptr<std::vector<unsigned char> > jpeg_data_t;
typedef boost::shared_ptr<std::vector<std::size_t> >  jpeg_rst_idxs_t;

} /* namespace vidstream */

#endif // FRAME_HPP__
