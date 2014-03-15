#include "stdint.h"
#include "bshift.h"
#include <iostream>


//=============================================================================
int find_shift(uint32_t key32, uint8_t* inbuff, uint32_t check_len){
// !!!!!!!!     ATTENTION       !!!!!!!!!!
// check_len - how much bytes will be tested for key. Must be less than "buffer lenght - 8".

    uint8_t* buff = inbuff;
    uint64_t first64 = *( (uint64_t*)buff );
    uint64_t key64 = key32;

    first64 = __builtin_bswap64( first64 );

    int shift = 0;
    int out_shift = -1;
    int min_ham_dist = 16;//50% coincedense
    int ham_dist;

    while(min_ham_dist){
        ham_dist = calc_ham_dist(first64 >> 32, key64);

        if(ham_dist < min_ham_dist){
            min_ham_dist = ham_dist;
            out_shift = shift;
	    if(ham_dist == 0)
		return out_shift;	    
        }

        first64 <<= 1;
        shift += 1;

        if( !(shift%32) ){

            first64 = *( (uint64_t*)(buff + shift/8 ) ) ;
    	    first64 = __builtin_bswap64( first64 );

            if( shift/8 >= check_len )
                return out_shift;
        }
    }

    return out_shift;
}

//=============================================================================
uint8_t calc_ham_dist(uint32_t first32, uint32_t key32){

    uint8_t dist = 0;

    first32 ^= key32;

    for(int ii=0; ii < 32; ii++)
        dist += (first32 >> ii) & 1;

    return dist;
}
//=============================================================================
bool shift_array(uint8_t* buff, uint32_t buff_len, int shift, bool shift_dirctn){ 
// !!!!!!!!     ATTENTION       !!!!!!!!!!
// shift is a bit shift, not byte
// buff_len is a size of shifted area only
// real buffer size must be "buf_len + shift/8 + 1" for right shift >> and "buf_len + shift/8" for left shift <<
// don't forget give a place for shifted bits !!!! 

    if( (shift < 0) || (buff_len < 2) )
        return false;
    
    uint32_t byte_shift = shift / 8;
    shift %= 8;
   
    uint8_t back_shift = 8 - shift;
    uint8_t tmps,tmpn;

    if(shift_dirctn == SHIFT_RIGTH){
	tmpn = buff[0] >> shift;	
	tmps = shift;
	shift = back_shift;
	back_shift = tmps;
    }
	
    for(int ii = 0; ii < buff_len; ii++){
        buff[ii] = buff[ii] << shift;
        buff[ii] |= buff[ii + 1] >> back_shift;
    }

    if(shift_dirctn == SHIFT_RIGTH){
    	for(int ii = buff_len; ii != -1 ; ii--)
            buff[ii+1] = buff[ii];
	buff[0] = tmpn;
    }
    
    if(byte_shift){
        if(shift_dirctn == SHIFT_RIGTH){

    	    for(int ii = buff_len; ii != -1 ; ii--)
                buff[ii + byte_shift] = buff[ii];

    	    for(int ii = 0; ii < byte_shift ; ii++)
		buff[ii] = 0;

	}else{
    	    for(int ii = 0; ii < buff_len; ii++)
                buff[ii] = buff[ii + byte_shift];
        }
    }

    return true;
}
//=============================================================================
