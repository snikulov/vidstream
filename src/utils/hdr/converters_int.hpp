#ifndef CONVERTERS_INT_HPP__
#define CONVERTERS_INT_HPP__

#include <boost/cstdint.hpp>
#include <boost/dynamic_bitset.hpp>

#ifdef BUILD_WITH_ITPP
#include <itpp/itcomm.h>
#endif

namespace vidstream
{

#ifdef BUILD_WITH_ITPP
    void to_itppbvec(const boost::dynamic_bitset<uint8_t>& bdb, itpp::bvec& ib);
    void to_dynbitset(const itpp::bvec& ib, boost::dynamic_bitset<uint8_t>& bdb);
    void to_vector(const itpp::bvec& ib, std::vector<uint8_t>& data);
#endif

    void to_vector(const boost::dynamic_bitset<boost::uint8_t>& bdb,
           std::vector<boost::uint8_t>& data);


}

#endif /* end of include guard: CONVERTERS_INT_HPP__ */
