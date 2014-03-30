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
