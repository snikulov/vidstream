#ifndef _ECC_H_
#define _ECC_H_

#include <stdint.h>
#include <iostream>
#include "bch.h"
using namespace std;

class ecc{
    public:
        ecc(float ecc2data = 1.0);
        ecc(uint8_t m, uint8_t t);
            // ecc2data = 0.2  гарантированно исправляется 1% ошибка и меньше
            // ecc2data = 1.0 гарантированно исправляется 3.5% ошибка и меньше
            // ecc2data = 2.0 гарантированно исправляется 5% ошибка и меньше
            // ecc2data = 3.0 при 10% ошибок около 12% байт остаются неправильными

        ~ecc();
        void* encode(void* in_data, size_t in_data_len, size_t &out_data_len);
        //кодирует данные, возвращает указатель на закодированный массив и его размер помещает в out_data_len
        void* decode(void* in_data, size_t in_data_len, size_t &out_data_len);
        //раскодирует данные и исправляет по возможности ошибки, возвращает указатель на раскодированный массив и его размер помещает в out_data_len
        //
        //in_data - указатель на входные данные, 
        //in_data_len - размер входных данных, 
        //out_data_len - в эту переменную будет помещен размер выходных данных
        void decode(void* in_data, size_t in_data_len, void* out_data, size_t &out_data_len);
        void encode(void* in_data, size_t in_data_len, void* out_data, size_t &out_data_len);        
        int get_enc_size(size_t in_data_len);// размер буфера для кодирования    в него in_data_len байт
        int get_dec_size(size_t in_data_len);// размер буфера для раскодирования в него in_data_len байт
    private:
        void ecc_init(uint8_t m, uint8_t t);
        bch_control *pbch;
        unsigned int pkg_len;
        unsigned int ecc_len;
        unsigned int data_len;
        uint8_t *data_buff;
        uint8_t *ecc_buff;
        unsigned int *errloc;
};


#endif
