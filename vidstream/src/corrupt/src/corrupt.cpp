#include "corrupt_int.h"

#include <cstdlib>
#include <ctime>

corruptor::corruptor()
{
    srand (time(NULL));
}

corruptor::corruptor(const corruptor& rhs)
{
    srand (time(NULL));
}

corruptor& corruptor::operator= (const corruptor& rhs)
{
    srand (time(NULL));
    return *this;
}


float corruptor::check_err(uint8_t *etalon, uint8_t *data, unsigned len)
{
    float bad_bytes = 0;
    for(unsigned ii = 0; ii < len; ii++)
        if(etalon[ii] != data[ii])
            bad_bytes += 1.0;

    return 100.0* bad_bytes /(float)len;
}

void corruptor::add_err(uint8_t *data, unsigned len, float err_prcnt)
{
    int prob = err_prcnt;
    if (!prob)
    {
        return;
    }
    for (unsigned i1 = 0; i1 < len; i1++)
    {
        for (unsigned i2 = 1; i2 != 256; i2 <<= 1)
        {
            if (prob > (rand() % 100))
            {
                data[i1] ^= i2;
            }
        }
    }
}

double corruptor::check_error(const std::vector<uint8_t>& etalon, const std::vector<uint8_t>& data)
{
    double bad_bytes = 0;
    std::vector<uint8_t>::size_type len = etalon.size();
    for(std::vector<uint8_t>::size_type i = 0; i < len; ++i)
    {
        if(etalon.at(i) != data.at(i))
        {
            bad_bytes += 1.0;
        }
    }
    return 100.0*bad_bytes /(double)len;
}

void  corruptor::add_error(std::vector<uint8_t>& data, double err_p)
{
    int prob = static_cast<int>(err_p);
    if (prob)
    {
        for (std::vector<uint8_t>::size_type i = 0; i < data.size(); ++i)
        {
            for (std::vector<uint8_t>::size_type j = 1; j != 256; j <<= 1)
            {
                if (prob > (rand() % 100))
                {
                    data.at(i) ^= j;
                }
            }
        }
    }
}


