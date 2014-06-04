#ifndef JPEG_RCV_STM_HPP__
#define JPEG_RCV_STM_HPP__
#include <vector>
#include <split/split.h>

typedef enum
{
    STM_INITIAL = 0,
    STM_WAIT_HEADER = STM_INITIAL,
    STM_WAIT_RST,
    STM_NO_RST,
    STM_INVALID_RST,
    STM_WAIT_STOP,
    STM_DATA_READY,
    STM_END = STM_DATA_READY
}STM_STATE_T;

class jpeg_rcv_stm
{
public:
    jpeg_rcv_stm(boost::shared_ptr<jpeg_builder> jb, boost::shared_ptr<jpeg_history> jh,
            const std::vector<unsigned char>& beg, const std::vector<unsigned char>& end)
        : jb_(jb), jh_(jh), state_(STM_INITIAL), start_(beg), end_(end)
    {
    }

    ~jpeg_rcv_stm()
    {
    }

    jpeg_data_t get_jpeg()
    {
        jpeg_data_t ret = jb_->build_jpeg_from_rst(rcv_data_);
        change_state(STM_WAIT_HEADER);
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
                else
                {
                    std::cerr << "STM_WAIT_HEADER: start not found " << std::endl;
                }
                break;
            case STM_WAIT_RST:
                {
                    size_t cur_rst = 0;
                    if (is_rstblock(buf, cur_rst))
                    {
                        rst_num_++;
                        collect_rst(buf);
                    }
                    else if(is_end(buf))
                    {
                        change_state(STM_DATA_READY);
                    }
                    else
                    {
                    }
                }
                break;

            case STM_WAIT_STOP:
                if (is_end(buf))
                {
                }
                else
                {
                    std::cerr << "STM_WAIT_STOP: end not found " << std::endl;
                }
                change_state(STM_DATA_READY);
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
        switch(state)
        {
            default:
                break;
        }
    }

    void on_enter(STM_STATE_T state)
    {
        switch(state)
        {
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
        return is_marker(buf, start_);
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
        if(0xFF == buf.at(0) && is_rst(buf.at(1)))
        {
            rst_num = buf[1] & 0x0F;
            return true;
        }
        return false;
    }

    void collect_rst(const std::vector<unsigned char>& buf)
    {
        std::vector<unsigned char>::const_iterator it;
        it = std::find_end(buf.begin(), buf.end(),
                buf.begin(),buf.begin()+1);
        if (it != buf.end() || it != buf.begin())
        {
            rcv_data_->insert(rcv_data_->end(), buf.begin(), it);
        }
        else
        {
            // not found terminator
        }

    }

    bool is_all_rst_received()
    {
        return (rst_num_ >= (jb_->get_rst_num()-1));
    }

    boost::shared_ptr<jpeg_history> jh_;
    boost::shared_ptr<jpeg_builder> jb_;

    STM_STATE_T state_;
    const std::vector<unsigned char>& start_;
    const std::vector<unsigned char>& end_;
    size_t rst_num_;
    jpeg_data_t rcv_data_;
};


#endif /* end of include guard: JPEG_RCV_STM_HPP__ */
