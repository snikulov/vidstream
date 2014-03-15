#include "ecc.h"

ecc::ecc(uint8_t m, uint8_t t){

    ecc_init(m,t);
}
//============================================================================================================================
void ecc::ecc_init(uint8_t m, uint8_t t){
    pkg_len = (1 << m) / 8;
    ecc_len = m*t/8;
    if( (m*t) % 8 )
        ecc_len += 1;
    data_len = pkg_len - ecc_len;

    cout<<" data:ecc ratio = 1:"<< (float)ecc_len / (float)data_len << " | ecc:data ratio = 1:"<< (float)data_len / (float)ecc_len<< endl;

    pbch = init_bch(m, t, 0);

    if(pbch)
        cout << "init ok." << endl;
    else{
        cout << "init failed." << endl;
        return;
    }

//    if(data_buff) free(data_buff);
//    if(errloc) free(errloc);

    data_buff = (uint8_t*)calloc(pkg_len, 1);
    ecc_buff = data_buff + data_len;
    errloc = (unsigned int*)calloc(pkg_len, sizeof(unsigned int));

}
//============================================================================================================================
ecc::ecc(float ecc2data){
        

    int m = 13; // ecc2data == 1
    int t = 105;


    if(ecc2data == 0.2 ){ // гарантированно исправляется 1% ошибка и меньше
        m = 13;
        t = 105;
    }

    if(ecc2data == 1 ){ // гарантированно исправляется 3.5% ошибка и меньше
        m = 10;
        t = 50;
    }

    if(ecc2data == 2 ){ // гарантированно исправляется 5% ошибка и меньше
        m = 10;
        t = 68;
    }

    if(ecc2data == 3){ // при 10% ошибок около 12% байт остаются неправильными
        m = 5;
        t = 4;
    }
    ecc_init(m,t);

}
//============================================================================================================================
ecc::~ecc(){
    free(data_buff);
    free(errloc);
}
//============================================================================================================================
int ecc::get_dec_size(size_t in_data_len){
    return data_len * (in_data_len / pkg_len);
}
//============================================================================================================================
void ecc::decode(void* in_data, size_t in_data_len, void* out_data, size_t &out_data_len){
    unsigned data_blocks = in_data_len / pkg_len;
    void *data_out = out_data;
    for(int curr_blk = 0; curr_blk < data_blocks; curr_blk++){
        memcpy(data_buff, in_data + pkg_len * curr_blk,  pkg_len);
        int errors = decode_bch(pbch, data_buff, data_len, ecc_buff, 0, 0, errloc);

        if (errors > 0)
             for (int ii = 0; ii < errors; ii++)
                 if (errloc[ii] < 8*data_len)
                     data_buff[errloc[ii]/8] ^= 1 << (errloc[ii] & 7);

        memcpy(data_out + data_len*curr_blk, data_buff, data_len);
    }

    out_data_len = data_blocks * data_len;
    //return data_out;
}
//==================================================================
int ecc::get_enc_size(size_t in_data_len){
    unsigned data_blocks = in_data_len / data_len;
    uint8_t is_tail = 0;
    if(in_data_len % data_len)
        is_tail = 1;

    return (data_blocks + is_tail) * pkg_len;
}
//==================================================================    
void ecc::encode(void* in_data, size_t in_data_len, void* out_data, size_t &out_data_len){

    unsigned data_blocks = in_data_len / data_len;
    uint8_t is_tail = 0;
    if(in_data_len % data_len)
        is_tail = 1;

    if((data_blocks + is_tail) * pkg_len > out_data_len){
        out_data_len = 0;
        return;
    }

    void *data_pkg = out_data;
    for(int curr_blk = 0; curr_blk < data_blocks; curr_blk++){
        memcpy(data_buff, in_data + data_len*curr_blk, data_len);
        memset((void*)ecc_buff,0,ecc_len);
        memset((void*)errloc, 0, pkg_len*sizeof(unsigned int));
        encode_bch(pbch, data_buff, data_len, ecc_buff);
        memcpy(data_pkg + pkg_len * curr_blk, data_buff,  pkg_len);
    }

    if(is_tail){
        memset((void*)data_buff,0,pkg_len);        
        memcpy(data_buff, in_data + data_len * (in_data_len / data_len), in_data_len % data_len);
        memset((void*)errloc, 0, pkg_len*sizeof(unsigned int));

        encode_bch(pbch, data_buff, data_len, ecc_buff);
        memcpy(data_pkg + pkg_len * (in_data_len / data_len), data_buff,  pkg_len);
    }

    out_data_len = (data_blocks + is_tail) * pkg_len;
    //return data_pkg;


}
//==================================================================
void* ecc::encode(void* in_data, size_t in_data_len, size_t &out_data_len){

    unsigned data_blocks = in_data_len / data_len;
    uint8_t is_tail = 0;
    if(in_data_len % data_len)
        is_tail = 1;

    void *data_pkg = calloc((data_blocks + is_tail) * pkg_len, 1);
    for(int curr_blk = 0; curr_blk < data_blocks; curr_blk++){
        memcpy(data_buff, in_data + data_len*curr_blk, data_len);
        memset((void*)ecc_buff,0,ecc_len);
        memset((void*)errloc, 0, pkg_len*sizeof(unsigned int));
        encode_bch(pbch, data_buff, data_len, ecc_buff);
        memcpy(data_pkg + pkg_len * curr_blk, data_buff,  pkg_len);
    }

    if(is_tail){
        memset((void*)data_buff,0,pkg_len);        
        memcpy(data_buff, in_data + data_len * (in_data_len / data_len), in_data_len % data_len);
        memset((void*)errloc, 0, pkg_len*sizeof(unsigned int));

        encode_bch(pbch, data_buff, data_len, ecc_buff);
        memcpy(data_pkg + pkg_len * (in_data_len / data_len), data_buff,  pkg_len);
    }

    out_data_len = (data_blocks + is_tail) * pkg_len;
    return data_pkg;
}

//==================================================================
void* ecc::decode(void* in_data, size_t in_data_len, size_t &out_data_len){
    unsigned data_blocks = in_data_len / pkg_len;
    void *data_out = calloc(data_blocks * 2, data_len);
    char* err_out = (char*)data_out + data_blocks * data_len;
//    memset(err_out, 0, data_blocks * data_len);

    for(int curr_blk = 0; curr_blk < data_blocks; curr_blk++){
        memcpy(data_buff, in_data + pkg_len * curr_blk,  pkg_len);
        int errors = decode_bch(pbch, data_buff, data_len, ecc_buff, 0, 0, errloc);

        if (errors > 0)
             for (int ii = 0; ii < errors; ii++)
                 if (errloc[ii] < 8*data_len)
                     data_buff[errloc[ii]/8] ^= 1 << (errloc[ii] & 7);

        if (errors < 0)
            memset(err_out + data_len*curr_blk, 1, data_len);

        memcpy(data_out + data_len*curr_blk, data_buff, data_len);
    }

    out_data_len = data_blocks * data_len;
    return data_out;
}

