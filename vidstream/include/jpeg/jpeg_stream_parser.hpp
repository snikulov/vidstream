#ifndef JPEG_STREAM_PARSER_HPP__
#define JPEG_STREAM_PARSER_HPP__

#include <vector>
#include <deque>

#include <types.hpp>

#include <boost/smart_ptr.hpp>
#include <boost/circular_buffer.hpp>

class jpeg_stream_parser
{
public:
    enum parse_status_t
    {
        need_more_data,
        jpeg_ready,
        overflow
    };

    jpeg_stream_parser(const std::vector<uint8_t>& mark);
    ~jpeg_stream_parser() {}

    parse_status_t parse(const std::vector<uint8_t>& data);
    parse_status_t parse(uint8_t data);
    parse_status_t parse();

    vidstream::jpeg_data_t get_jpeg();

    size_t int_buf_size() const
    {
        return cbuff.size();
    }

    size_t num_jpegs() const
    {
        return jpegs_.size();
    }

private:
    jpeg_stream_parser();

    std::vector<uint8_t> mark_;
    std::deque<uint8_t>  workq_;
    std::deque<vidstream::jpeg_data_t> jpegs_;

    typedef std::deque< uint8_t > cbuff_type;

    // TODO: play with boost::circular_buf 
    cbuff_type cbuff;


};

#endif

