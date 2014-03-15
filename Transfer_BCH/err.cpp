#include "err.h"
// ==================================================================
float err::check_err(uint8_t *etalon, uint8_t *data, unsigned len){
    float bad_bytes = 0;
    for(int ii = 0; ii < len; ii++)
        if(etalon[ii] != data[ii])
            bad_bytes += 1.0;

    return 100.0* bad_bytes /(float)len;
}

// ==================================================================
float err::add_err(uint8_t* data, unsigned len, float err_prcnt){
    uint8_t curr_bad_bit = 0;
    unsigned bad_bits = 0;
    unsigned bad_bytes = 0;
    bool wrong_byte;

    srand (time(NULL));

    for(int ii = 0; ii< len; ii++){
        wrong_byte = false;
        for(int jj = 0; jj< 8; jj++){
            if( (rand() *100.0 / RAND_MAX) < err_prcnt){
                curr_bad_bit = 1 << jj;
                bad_bits += 1;
                wrong_byte = true;

                if(data[ii] & curr_bad_bit)
                    data[ii] &= ~curr_bad_bit;
                else
                    data[ii] |= curr_bad_bit;
            }
        }

        if(wrong_byte)
            bad_bytes += 1;
    }

    return bad_bytes;
}

