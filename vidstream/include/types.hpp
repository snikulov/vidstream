#ifndef VIDSTREAM_TYPES_HPP__
#define VIDSTREAM_TYPES_HPP__

#include <vector>

#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>

namespace vidstream
{
typedef boost::shared_ptr<std::vector<unsigned char> > jpeg_data_t;
typedef boost::shared_ptr<std::vector<std::size_t> >  jpeg_rst_idxs_t;
} /* namespace vidsteram */

using boost::uint8_t;

#endif // VIDSTREAM_TYPES_HPP__
