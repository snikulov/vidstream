#ifndef __CORRUPT_H
#define __CORRUPT_H

#include <types.hpp>
#include <vector>
#include <boost/dynamic_bitset.hpp>

class corruptor
{
public:
    corruptor();
    corruptor(const corruptor&);
    corruptor& operator= (const corruptor&);

    float check_err(uint8_t *etalon, uint8_t *data, unsigned len);
    void add_err(uint8_t* data, unsigned len, float err_prcnt);

    double check_error(const std::vector<uint8_t>& etalon, const std::vector<uint8_t>& data);
    void add_error(std::vector<uint8_t>& data, double err_p);

    boost::dynamic_bitset<> to_bitset(const std::vector<uint8_t>& data);
    std::vector<uint8_t> to_bytes(const boost::dynamic_bitset<>& bits);
};

#endif
