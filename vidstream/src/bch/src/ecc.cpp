#include <cstring>

#include <ecc/ecc.h>

#include "bch.h"


using namespace std;

ecc::ecc(uint8_t m, uint8_t t) :
    data_buff(NULL), errloc(NULL)
{

    ecc_init(m,t);
}
//============================================================================================================================
void ecc::ecc_init(uint8_t m, uint8_t t){
    pkg_len = (1 << m) / 8;
    ecc_len = m*t/8;
    if( (m*t) % 8 )
        ecc_len += 1;
    data_len = pkg_len - ecc_len;

    if (data_len == 0 || pkg_len == 0 || ecc_len == 0) {
        // will cause floating point exception in encode() and decode()
        //qCritical() << "Incorrect BCH parameters";
        throw("Incorrect BCH parameters");
    }

    //cout<<" data:ecc ratio = 1:"<< (float)ecc_len / (float)data_len << " | ecc:data ratio = 1:"<< (float)data_len / (float)ecc_len<< endl;

    pbch = init_bch(m, t, 0);

    if(!pbch) {
        //qCritical() << "BCH init failed." << endl;
        throw("BCH init failed");
        //return;
    }

    if(data_buff) free(data_buff);
    if(errloc) free(errloc);

    data_buff = (uint8_t*)calloc(pkg_len, 1);
    ecc_buff = data_buff + data_len;
    errloc = (unsigned int*)calloc(pkg_len, sizeof(unsigned int));

}

//============================================================================================================================
ecc::~ecc(){
    if (pbch) {
        free_bch(pbch);
    }
    if (data_buff) {
        free(data_buff);
    }
    if (errloc) {
        free(errloc);
    }
}

//==================================================================
char* ecc::encode(const char* in_data, size_t in_data_len, size_t &out_data_len){

    unsigned data_blocks = in_data_len / data_len;
    uint8_t is_tail = 0;
    if(in_data_len % data_len)
        is_tail = 1;

    char *data_pkg = (char *) calloc((data_blocks + is_tail) * pkg_len, 1);
    for(unsigned curr_blk = 0; curr_blk < data_blocks; curr_blk++){
        memcpy(data_buff, in_data + data_len*curr_blk, data_len);
        memset(ecc_buff,0,ecc_len);
        memset(errloc, 0, pkg_len*sizeof(unsigned int));
        encode_bch(pbch, data_buff, data_len, ecc_buff);
        memcpy(data_pkg + pkg_len * curr_blk, data_buff,  pkg_len);
    }
    if(is_tail){
        memset(data_buff,0,pkg_len);
        memcpy(data_buff, in_data + data_len * (in_data_len / data_len), in_data_len % data_len);
        memset(errloc, 0, pkg_len*sizeof(unsigned int));

        encode_bch(pbch, data_buff, data_len, ecc_buff);
        memcpy(data_pkg + pkg_len * (in_data_len / data_len), data_buff,  pkg_len);
    }

    out_data_len = (data_blocks + is_tail) * pkg_len;
    //fprintf(stderr, "out_data_len = %lu, in_data_len = %lu, "
    //        "overhead = %0.2f%%, pkg_len = %u\n",
    //        out_data_len, in_data_len,
    //        (out_data_len - in_data_len) / (0.01 * in_data_len), pkg_len);
    return data_pkg;
}

//==================================================================
char* ecc::decode(const char* in_data, size_t in_data_len, size_t &out_data_len,
                  std::vector<char> &successful, bool &decoded_ok){
    unsigned data_blocks = in_data_len / pkg_len;
    char *data_out = (char *) calloc(data_blocks, data_len);
    if (successful.size() < data_blocks * data_len) {
        successful.resize(data_blocks * data_len);
    }
    unsigned failed_cnt = 0;
    decoded_ok = true;
    for(unsigned curr_blk = 0; curr_blk < data_blocks; curr_blk++){
        memcpy(data_buff, in_data + pkg_len * curr_blk,  pkg_len);
        int errors = decode_bch(pbch, data_buff, data_len, ecc_buff, 0, 0, errloc);

        if (errors > 0) {
             for (int ii = 0; ii < errors; ii++)
                 if (errloc[ii] < 8*data_len)
                     data_buff[errloc[ii]/8] ^= 1 << (errloc[ii] & 7);
        }
        if (errors < 0) {
            // failed to decode a block
            // set all its bytes to false
            memset(&successful[data_len * curr_blk], 0, data_len);
            decoded_ok = false;
            failed_cnt++;
        }

        memcpy(data_out + data_len*curr_blk, data_buff, data_len);
    }

    // update error statistics
#if 0
    if (stat) {
        stat->AddBchPkg(data_blocks, failed_cnt);
    }
#endif

    out_data_len = data_blocks * data_len;
    return data_out;
}
