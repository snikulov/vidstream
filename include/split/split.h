#ifndef __SPLIT_H
#define __SPLIT_H

#include <iosfwd>
#include <vector>

enum Markers
{
    SOI = 0xD8,
    SOF = 0xC0,
    DHT = 0xC4,
    DQT = 0xDB,
    DRI = 0xDD,
    SOS = 0xDA,
    RST = 0xD0,
    APP = 0xE0,
    COM = 0xFE,
    EOI = 0xD9
};

enum { MARKER_CNT = 10 };

typedef struct
{
    unsigned char type; // must be one of the enum values
    unsigned char id;   // actual id byte
    int no;             // there are various SOFs, RSTs, and APPs
} Marker;

// if safe = 1, puts SOS header in header file
// otherwise SOS header goes with body
int split_file(std::istream &fin, std::ostream *fhead, std::ostream &fbody, char safe, size_t &restart_count);

int is_rst(int c);



// input const std::vector<unsigned char> - buffer with jpeg data
// do following
// check if it jpeg, find DRI and check it non zero, then first, last RST (our data)
bool get_all_rst_blocks(const std::vector<unsigned char>&, std::vector<std::size_t>&);

bool is_valid_marker(unsigned char c);


#endif
