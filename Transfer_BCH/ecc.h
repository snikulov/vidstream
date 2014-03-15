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
            // ecc2data = 0.2  �������������� ������������ 1% ������ � ������
            // ecc2data = 1.0 �������������� ������������ 3.5% ������ � ������
            // ecc2data = 2.0 �������������� ������������ 5% ������ � ������
            // ecc2data = 3.0 ��� 10% ������ ����� 12% ���� �������� �������������

        ~ecc();
        void* encode(void* in_data, size_t in_data_len, size_t &out_data_len);
        //�������� ������, ���������� ��������� �� �������������� ������ � ��� ������ �������� � out_data_len
        void* decode(void* in_data, size_t in_data_len, size_t &out_data_len);
        //����������� ������ � ���������� �� ����������� ������, ���������� ��������� �� ��������������� ������ � ��� ������ �������� � out_data_len
        //
        //in_data - ��������� �� ������� ������, 
        //in_data_len - ������ ������� ������, 
        //out_data_len - � ��� ���������� ����� ������� ������ �������� ������
        void decode(void* in_data, size_t in_data_len, void* out_data, size_t &out_data_len);
        void encode(void* in_data, size_t in_data_len, void* out_data, size_t &out_data_len);        
        int get_enc_size(size_t in_data_len);// ������ ������ ��� �����������    � ���� in_data_len ����
        int get_dec_size(size_t in_data_len);// ������ ������ ��� �������������� � ���� in_data_len ����
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
