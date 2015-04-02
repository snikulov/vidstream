#include "corrupt_int.h"

#include <cstdlib>
#include <ctime>

#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/math/special_functions/round.hpp>


size_t get_err_count(const std::vector<uint8_t>& etalon, const std::vector<uint8_t>& data)
{
    boost::dynamic_bitset<> eb = to_bitset(etalon);
    boost::dynamic_bitset<> db = to_bitset(data);
    db ^= eb;
    return db.count();
}

double get_err_persent(size_t bits_len, size_t num_err)
{
    return (((double)num_err * 100.) / (double)bits_len);
}

void corruptor::add_error(std::vector<uint8_t>& data, double err_p)
{
    boost::dynamic_bitset<> db = to_bitset(data);
    size_t bitlen = db.size();

    // TODO: make it more precise
    size_t num_errors = boost::math::lround((double)bitlen/100. * err_p);

    boost::random::random_device rng;
    boost::random::uniform_int_distribution<> index_dist(0, bitlen - 1);

    for(size_t i = 0; i < num_errors; ++i)
    {
        db[index_dist(rng)].flip();
    }

    std::vector<uint8_t> res = to_bytes(db);
    data.swap(res);
}

size_t size_in_bits(const std::vector<uint8_t>& data)
{
    return data.size() * sizeof(uint8_t) * CHAR_BIT;
}

boost::dynamic_bitset<> to_bitset(const std::vector<uint8_t>& data)
{
    const size_t bitlen = size_in_bits(data);

    boost::dynamic_bitset<> ret(bitlen);

    for(size_t i=0; i < data.size();++i)
    {
        uint8_t val = data.at(i);
        size_t disp = i * 8;

        ret[disp++] = (val & (1 << 0)) ? true : false;
        ret[disp++] = (val & (1 << 1)) ? true : false;
        ret[disp++] = (val & (1 << 2)) ? true : false;
        ret[disp++] = (val & (1 << 3)) ? true : false;
        ret[disp++] = (val & (1 << 4)) ? true : false;
        ret[disp++] = (val & (1 << 5)) ? true : false;
        ret[disp++] = (val & (1 << 6)) ? true : false;
        ret[disp++] = (val & (1 << 7)) ? true : false;
    }
    return ret;
}

std::vector<uint8_t> to_bytes(const boost::dynamic_bitset<>& bits)
{
    const size_t capa = bits.size()/8 + 1;
    std::vector<uint8_t> ret;
    ret.reserve(capa);

    uint8_t val = 0;

    for(size_t i=0; i < bits.size(); ++i)
    {
        size_t shift = i % 8;
        if (!shift && i > 0)
        {
            ret.push_back(val);
            val = 0;
        }
        val |= (bits[i] ? 1 << shift : 0);
    }

    ret.push_back(val);

    return ret;
}


