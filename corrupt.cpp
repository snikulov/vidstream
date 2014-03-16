#include "corrupt.h"

#include <stdlib.h>
#include <ctime>
#include <cmath>

float Corruptor::check_err(uint8_t *etalon, uint8_t *data, unsigned len){
    float bad_bytes = 0;
    for(unsigned ii = 0; ii < len; ii++)
        if(etalon[ii] != data[ii])
            bad_bytes += 1.0;

    return 100.0* bad_bytes /(float)len;
}

/*unsigned Corruptor::add_err(uint8_t* data, unsigned len, float err_prcnt){
    uint8_t curr_bad_bit = 0;
    unsigned bad_bits = 0;
    unsigned bad_bytes = 0;
    bool wrong_byte;

    srand (time(NULL));
    err_prcnt *= RAND_MAX / 100.0;

    for(unsigned ii = 0; ii< len; ii++){
        wrong_byte = false;
        for(int jj = 0; jj< 8; jj++){
            if (rand() < prob) {
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
*/

unsigned Corruptor::add_err(uint8_t *data, unsigned len, float err_prcnt)
{
    unsigned cnt = 0;
    bool f;
    int prob = err_prcnt;
    for (unsigned i1 = 0; i1 < len; i1++) {
        f = false;
        for (unsigned i2 = 1; i2 != 256; i2 <<= 1) {
            if (prob > (rand() % 100)) {
                data[i1] ^= i2;
                f = true;
            }
        }
        cnt += f;
    }
    return cnt;
}
