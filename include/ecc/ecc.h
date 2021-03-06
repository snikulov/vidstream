#ifndef _ECC_H_
#define _ECC_H_

#include <boost/cstdint.hpp>
#include <iostream>
#include <vector>

//using namespace std;

// class StatCollector;

struct bch_control;

class ecc
{
    public:
        //ecc(float ecc2data = 1.0);
        ecc(uint8_t m, uint8_t t);

        ~ecc();

        char* encode(const char* in_data, size_t in_data_len, size_t &out_data_len);
        //кодирует данные, возвращает указатель на закодированный массив и его размер помещает в out_data_len
        char* decode(const unsigned char* in_data, size_t in_data_len, size_t &out_data_len,
                     std::vector<char> &successful, bool &decoded_ok);
        //раскодирует данные и исправляет по возможности ошибки, возвращает указатель на раскодированный массив и его размер помещает в out_data_len
        //
        //in_data - указатель на входные данные,
        //in_data_len - размер входных данных,
        //out_data_len - в эту переменную будет помещен размер выходных данных
    private:
        void ecc_init(uint8_t m, uint8_t t);
        uint8_t m_;
        uint8_t t_;
        bch_control *pbch;
        unsigned int pkg_len;
        unsigned int ecc_len;
        unsigned int data_len;
        uint8_t *data_buff;
        uint8_t *ecc_buff;
        unsigned int *errloc;

//        StatCollector *stat;
};


#endif
