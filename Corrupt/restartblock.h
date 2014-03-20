#ifndef RESTARTBLOCK_H
#define RESTARTBLOCK_H

#include <cstddef>
#include <cstdint>
#include <vector>

class RestartBlock
{
public:
    RestartBlock();
    RestartBlock(uint8_t *ptr, size_t size);
    RestartBlock(const RestartBlock& other) :
        pushbacks_cnt(other.pushbacks_cnt),
        data(other.data)
    { }
    RestartBlock &operator=(const RestartBlock &other);
    RestartBlock &operator=(RestartBlock &&other);

    void push_back(uint8_t c) { data.push_back(c); pushbacks_cnt++; }
    void clear();
    void set_info(uint8_t frame_number, uint16_t rst_block_number, uint16_t length);

    uint8_t *raw_ptr()               { return data.data();                     }
    uint8_t *data_ptr()              { return get_data_ptr(raw_ptr());         }
    size_t raw_length()              { return data.size();                     }
    size_t data_length()             { return get_data_length(raw_ptr());      }
    uint8_t frame_number()           { return get_frame_number(raw_ptr());     }
    uint16_t rst_block_number()      { return get_rst_block_number(raw_ptr()); }

    uint16_t pushbacks_count() const { return pushbacks_cnt;                   }

    static uint8_t* get_data_ptr(uint8_t *raw_ptr) {
        return raw_ptr + info_len;
    }
    static size_t get_data_length(const uint8_t *raw_ptr) {
        return raw_ptr[3] * 0xFF + raw_ptr[4];
    }
    static uint8_t get_frame_number(const uint8_t *raw_ptr) {
        return *raw_ptr;
    }
    static uint16_t get_rst_block_number(const uint8_t *raw_ptr) {
        return raw_ptr[1] * 0xFF + raw_ptr[2];
    }
    static constexpr size_t get_info_len() {
        return info_len;
    }

private:
    static constexpr size_t info_len = 5;
    uint16_t pushbacks_cnt;
    std::vector<uint8_t> data;
};

#endif // RESTARTBLOCK_H
