#include "corrupt_int.h"

#include <cstdlib>
#include <ctime>

#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/math/special_functions/round.hpp>

corruptor::corruptor()
{
    srand (time(NULL));
}

corruptor::corruptor(const corruptor& rhs)
{
    srand (time(NULL));
}

corruptor& corruptor::operator= (const corruptor& rhs)
{
    srand (time(NULL));
    return *this;
}


float corruptor::check_err(uint8_t *etalon, uint8_t *data, unsigned len)
{
    float bad_bytes = 0;
    for(unsigned ii = 0; ii < len; ii++)
        if(etalon[ii] != data[ii])
            bad_bytes += 1.0;

    return 100.0* bad_bytes /(float)len;
}

void corruptor::add_err(uint8_t *data, unsigned len, float err_prcnt)
{
    int prob = err_prcnt;
    if (!prob)
    {
        return;
    }
    for (unsigned i1 = 0; i1 < len; i1++)
    {
        for (unsigned i2 = 1; i2 != 256; i2 <<= 1)
        {
            if (prob > (rand() % 100))
            {
                data[i1] ^= i2;
            }
        }
    }
}

double corruptor::check_error(const std::vector<uint8_t>& etalon, const std::vector<uint8_t>& data)
{
    boost::dynamic_bitset<> eb = to_bitset(etalon);
    boost::dynamic_bitset<> db = to_bitset(data);
    db ^= eb;
    size_t corrupted = db.count();
    return (((double)corrupted * 100.0)/(double)eb.size());
}

void corruptor::add_error(std::vector<uint8_t>& data, double err_p)
{
    boost::dynamic_bitset<> db = to_bitset(data);
    size_t bitlen = db.size();

    // TODO: make it more precise
    size_t num_errors = boost::math::lround(bitlen/100 * err_p);

    boost::random::random_device rng;
    boost::random::uniform_int_distribution<> index_dist(0, bitlen - 1);

    for(size_t i = 0; i < num_errors; ++i)
    {
        db[index_dist(rng)].flip();
    }

    std::vector<uint8_t> res = to_bytes(db);
    data.swap(res);
}

boost::dynamic_bitset<> corruptor::to_bitset(const std::vector<uint8_t>& data)
{
    const size_t bitlen = data.size() * (sizeof(uint8_t) * 8);
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

std::vector<uint8_t> corruptor::to_bytes(const boost::dynamic_bitset<>& bits)
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


