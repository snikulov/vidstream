#ifndef JPEG_RCV_STM_HPP__
#define JPEG_RCV_STM_HPP__
#include <vector>
#include <queue>
#include <split/split.h>
#include <corrupt/corrupt.h>
#include <jpeg/jpeg_history.hpp>

#define MAX_NUM_OF_ERROR 3

typedef enum
{
    STM_INITIAL = 0,
    STM_WAIT_HEADER = STM_INITIAL,
    STM_WAIT_RST,
    STM_END = STM_WAIT_RST
}STM_STATE_T;

class jpeg_rcv_stm
{
public:
    jpeg_rcv_stm(boost::shared_ptr<vidstream::jpeg_builder> jb
        , boost::shared_ptr<jpeg_history> jh
        , const std::vector<unsigned char>& beg, const std::vector<unsigned char>& end)
        : jb_(jb), jh_(jh), state_(STM_INITIAL), start_(beg), end_(end)
    {
    }

    ~jpeg_rcv_stm()
    {
    }
 
    bool has_data()
    {
        return !q_.empty();
    }

    jpeg_data_t get_jpeg()
    {
        
        jpeg_data_t ret;
        if (has_data())
        {
            ret = jb_->build_jpeg_from_rst(q_.front());
            q_.pop();
        }
        return ret;
    }

    STM_STATE_T process(const std::vector<unsigned char>& buf)
    {
        switch(state_)
        {
            case STM_WAIT_HEADER:
                if (is_header(buf))
                {
                    change_state(STM_WAIT_RST);
                }
                break;
            case STM_WAIT_RST:
                if (is_header(buf) && !is_all_rst_received())
                {
                    // possible
                    change_state(STM_WAIT_RST);
                    break;
                }

                collect_rst(buf);
                rst_num_++;

                if (is_all_rst_received())
                {
                    change_state(STM_WAIT_HEADER);
                }

                break;

            default:
                break;
        }
        return state_;
    }

private:
    /* data */
    void on_exit(STM_STATE_T state)
    {

    }

    void on_enter(STM_STATE_T state)
    {
        switch(state)
        {
            case STM_WAIT_HEADER:
                if (rcv_data_)
                {
                    rcv_data_->insert(rcv_data_->end(), end_.begin(), end_.end());
                    q_.push(rcv_data_);
                    rcv_data_.reset();
                }
            break;

            case STM_WAIT_RST:
                rst_num_ = 0;
                rcv_data_.reset(new std::vector<unsigned char>());
                break;
            default:
                break;
        }
    }

    void change_state(STM_STATE_T state)
    {
        on_exit(state_);
        state_ = state;
        on_enter(state_);
    }

    bool is_header(const std::vector<unsigned char>& buf)
    {
        if (start_.size() <= buf.size())
        {
            std::vector<uint8_t> data(buf.begin(), buf.begin()+start_.size());
            if (get_err_count(start_, data) <= MAX_NUM_OF_ERROR)
            {
                return true;
            }
            return false;
        }
        return false;
    }

    bool is_end(const std::vector<unsigned char>& buf)
    {
        return is_marker(buf, end_);
    }

    bool is_marker(const std::vector<unsigned char>& buf, const std::vector<unsigned char>& mark)
    {
        return (buf.end() != std::search(buf.begin(), buf.end(), mark.begin(), mark.end()));
    }

    bool is_rstblock(const std::vector<unsigned char>& buf, size_t& rst_num)
    {
      //  if (get_err_count(start_, data) <= MAX_NUM_OF_ERROR)
        //if(0xFF == buf.at(0) && is_rst(buf.at(1)))
      std::vector<unsigned char> xff;
      xff.push_back(0xff);
      if( (get_err_count(buf, xff) <= MAX_NUM_OF_ERROR) && is_rst(buf.at(1)))
        {
            rst_num = buf[1] & 0x0F;
            return true;
        }
        return false;
    }

    void collect_rst(const std::vector<unsigned char>& buf)
    {
        uint8_t rstnum = 0xd0;
        rstnum |= rst_num_%8;

        rcv_data_->push_back(0xff);
        rcv_data_->push_back(rstnum);

        rcv_data_->insert(rcv_data_->end(), buf.begin(), buf.end());
    }

    bool is_all_rst_received()
    {
        size_t erst = jb_->get_rst_num()-1;
        return !(rst_num_ < erst);
    }

    boost::shared_ptr<jpeg_history> jh_;
    boost::shared_ptr<jpeg_builder> jb_;

    STM_STATE_T state_;
    const std::vector<unsigned char>& start_;
    const std::vector<unsigned char>& end_;
    size_t rst_num_;
    jpeg_data_t rcv_data_;
    std::queue<jpeg_data_t> q_;
};


#endif /* end of include guard: JPEG_RCV_STM_HPP__ */
