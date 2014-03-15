#ifndef _BSHIFT_H_
#define _BSHIFT_H_
#include <string.h>
#include <stdlib.h>

#define SHIFT_RIGTH true
#define SHIFT_LEFT false

uint8_t calc_ham_dist(uint32_t first32, uint32_t key32);
int find_shift(uint32_t key32, uint8_t* inbuff, uint32_t check_len);
bool shift_array(uint8_t* buff, uint32_t buff_len, int shift, bool shift_dirctn);

#endif
