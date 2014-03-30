#ifndef _ERR_H_
#define _ERR_H_

#include <stdint.h>
#include <time.h>
#include <cstdlib>
#include <iostream>
using namespace std;

class err{
    public:
    float check_err(uint8_t *etalon, uint8_t *data, unsigned len);
    float add_err(uint8_t* data, unsigned len, float err_prcnt);
};

#endif
