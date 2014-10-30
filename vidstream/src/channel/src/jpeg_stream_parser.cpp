#include "jpeg/jpeg_stream_parser.hpp"

jpeg_stream_parser::jpeg_stream_parser(const std::vector<uint8_t>& mark)
: mark_(mark)
{
}


jpeg_stream_parser::parse_status_t jpeg_stream_parser::parse()
{
    std::deque<uint8_t>::iterator l = std::search(workq_.begin(), workq_.end(), mark_.begin(), mark_.end());

    if (l == workq_.end())
    {
        // no marker
        return need_more_data;
    }

    if (l == workq_.begin())
    {
        // at the beginning
        workq_.erase(workq_.begin(), workq_.begin() + mark_.size());
        return parse();
    }

    // else we got jpeg between marks
    vidstream::jpeg_data_t jpeg(new std::vector<uint8_t>(workq_.begin(), l));
    workq_.erase(workq_.begin(), l + mark_.size());

    jpegs_.push_back(jpeg);

    // process next data...
    while (jpeg_ready == parse()) ;
        
    return jpeg_ready;
}

jpeg_stream_parser::parse_status_t jpeg_stream_parser::parse(const std::vector<uint8_t>& data)
{
    workq_.insert(workq_.end(), data.begin(), data.end());
    return parse();
}

vidstream::jpeg_data_t jpeg_stream_parser::get_jpeg()
{
    vidstream::jpeg_data_t ret_val;

    if (!jpegs_.empty())
    {
        ret_val = jpegs_.front();
        jpegs_.pop_front();
    }
    return ret_val;
}
