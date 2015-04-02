#ifndef FRAME_HPP__
#define FRAME_HPP__
#include <boost/shared_ptr.hpp>

namespace cv
{
class Mat;
} /* namespace cv */

namespace vidstream
{

typedef boost::shared_ptr<cv::Mat> camera_frame_t;

} /* namespace vidstream */

#endif // FRAME_HPP__
