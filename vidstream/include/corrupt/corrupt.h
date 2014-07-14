#ifndef __CORRUPT_H
#define __CORRUPT_H

#include <types.hpp>
#include <vector>
#include <boost/dynamic_bitset.hpp>

class corruptor
{
public:
    corruptor(){}

    void add_error(std::vector<uint8_t>& data, double persent);

};


double get_err_persent(size_t bits_len, size_t num_err);
size_t get_err_count(const std::vector<uint8_t>& etalon, const std::vector<uint8_t>& data);
size_t size_in_bits(const std::vector<uint8_t>& data);
boost::dynamic_bitset<> to_bitset(const std::vector<uint8_t>& data);
std::vector<uint8_t> to_bytes(const boost::dynamic_bitset<>& bits);

#endif
