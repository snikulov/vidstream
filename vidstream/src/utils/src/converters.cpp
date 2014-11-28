#include <boost/dynamic_bitset.hpp>
#include <itpp/itcomm.h>

namespace vidstream
{
    void to_itppbvec(const boost::dynamic_bitset<uint8_t>& bdb, itpp::bvec& ib)
    {
        size_t len = bdb.size();
        itpp::bvec res(len);

        for(size_t i=0,j = len-1; i < len; ++i, --j)
        {
            res[i] = bdb[j];
        }

        ib = res;
    }

    void to_dynbitset(const itpp::bvec& ib, boost::dynamic_bitset<uint8_t>& bdb)
    {
        size_t len = ib.size();
        boost::dynamic_bitset<uint8_t> res(len);

        for(size_t i = 0, j = len-1; i < len; ++i, --j)
        {
            res[i] = ib[j];
        }
        bdb.swap(res);
    }

    void to_vector(const boost::dynamic_bitset<uint8_t>& bdb, std::vector<uint8_t>& data)
    {
        std::vector<uint8_t> res(bdb.num_blocks());
        to_block_range(bdb, res.begin());
        data.swap(res);
    }

    void to_vector(const itpp::bvec& ib, std::vector<uint8_t>& data)
    {
        boost::dynamic_bitset<uint8_t> dst;
        to_dynbitset(ib, dst);

        std::vector<uint8_t> res;
        to_vector(dst, res);
        data.swap(res);
    }
}

