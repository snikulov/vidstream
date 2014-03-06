#ifndef __CORRUPT_H
#define __CORRUPT_H

#include <cstdint>

class Corruptor
{
public:
    static float check_err(uint8_t *etalon, uint8_t *data, unsigned len);
    static unsigned add_err(uint8_t* data, unsigned len, float err_prcnt);
};

#endif
