#ifndef _ECC_H_
#define _ECC_H_

#include <cstdint>
#include <iostream>
#include <vector>

#include "bch.h"

//using namespace std;

class StatCollector;

class ecc{
    public:
        //ecc(float ecc2data = 1.0);
        ecc(uint8_t m, uint8_t t, StatCollector *stat = NULL);

        ~ecc();
        void* encode(char* in_data, size_t in_data_len, size_t &out_data_len);
        //кодирует данные, возвращает указатель на закодированный массив и его размер помещает в out_data_len
        void* decode(char* in_data, size_t in_data_len, size_t &out_data_len,
                     std::vector<char> &successful, bool &chunk_decoded_ok);
        //раскодирует данные и исправляет по возможности ошибки, возвращает указатель на раскодированный массив и его размер помещает в out_data_len
        //
        //in_data - указатель на входные данные, 
        //in_data_len - размер входных данных, 
        //out_data_len - в эту переменную будет помещен размер выходных данных
    private:
        void ecc_init(uint8_t m, uint8_t t);
        bch_control *pbch;
        unsigned int pkg_len;
        unsigned int ecc_len;
        unsigned int data_len;
        uint8_t *data_buff;
        uint8_t *ecc_buff;
        unsigned int *errloc;

        StatCollector *stat;
};


#endif
