#ifndef THREADED_CODER_H
#define THREADED_CODER_H

#define DATA_LEN             400 // maximum package size
#define NUM_OF_PKGS          1 //0  // maximum packages in interprocess queue
#define PKG_MAX_SIZE         DATA_LEN

#define TO_DECODE_MSG       "TO_DECODE_MSG"
#define TO_OUT_MSG          "TO_OUT_MSG"
#define TO_READ_MSG         "TO_READ_MSG"
#define TO_ADD_ERR_MSG      "TO_ADD_ERR_MSG"
#define TO_ENCODE_MSG       "TO_ENCODE_MSG"
#define TO_SEND_MSG         "TO_SEND_MSG"

//----------------------------------
struct send_data {
    size_t in_buff_lnt;
    char in_buff[DATA_LEN];
};

#endif // THREADED_CODER_H
