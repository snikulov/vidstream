#include "jpeg/jpeg_stream_parser.hpp"

jpeg_stream_parser::jpeg_stream_parser(const std::vector<uint8_t>& mark)
: mark_(mark), cbuff(new boost::circular_buffer< uint8_t >(2*1024*1024))
{
}


jpeg_stream_parser::parse_status_t jpeg_stream_parser::parse()
{
    boost::circular_buffer< uint8_t >::iterator l = std::search(cbuff->begin(), cbuff->end(), mark_.begin(), mark_.end());

    if (l == cbuff->end())
    {
        // no marker
        return need_more_data;
    }

    if (l == cbuff->begin())
    {
        // at the beginning
        cbuff->erase(cbuff->begin(), cbuff->begin() + mark_.size());
        return parse();
    }

    // else we got jpeg between marks
    vidstream::jpeg_data_t jpeg(new std::vector<uint8_t>(cbuff->begin(), l));
    cbuff->erase(cbuff->begin(), l + mark_.size());

    jpegs_.push_back(jpeg);

    // process next data...
    while (jpeg_ready == parse()) ;
        
    return jpeg_ready;
}

jpeg_stream_parser::parse_status_t jpeg_stream_parser::parse(const std::vector<uint8_t>& data)
{
    cbuff->insert(cbuff->begin(), data.begin(), data.end());
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
