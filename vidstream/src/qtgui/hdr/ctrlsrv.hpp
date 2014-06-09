#ifndef CONTROL_SRV_HPP__
#define CONTROL_SRV_HPP__

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <string>
#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/shared_ptr.hpp>

#include "cfg/cfg_subscribers.hpp"

typedef boost::shared_ptr<boost::property_tree::ptree> cfg_ptr_t;

class ctrlsrv
{
public:
    ctrlsrv(Ui::MainWindow &u, cfg_ptr_t cfg, const std::string& url, bool& stop)
        : ui_(u), cfg_(cfg), url_(url), stop_(stop)
    {
    }
    ~ctrlsrv()
    {
    }

    void operator()()
    {
        trans_.reset(new transport(TRANSPORT_REP, url_));
        while(!stop_)
        {
            std::vector<unsigned char> cmd;
            int bytes = trans_->receive(cmd);

            if (bytes >= 0 && !stop_)
            {
                // got command - don't care for now
                // later will use it for statistic
                std::string stat(cmd.begin(), cmd.end());
                update_stat(ui_, stat);

                std::ostringstream out;
                write_json(out, *cfg_, false);
                std::string data(out.str());
                bytes = trans_->send(data);
                if (bytes != data.size())
                {
                    std::cerr << "Error send config" << std::endl;
                }
                else
                {
                    subs_.notify_change(*cfg_);
                }
            }
            else
            {
                // error case
            }
        }
    }

    void stop()
    {
        stop_ = true;
    }

    bool subscribe(cfg_notify* listener)
    {
        return subs_.subscribe(listener);
    }

private:
    /* data */
    Ui::MainWindow &ui_;
    cfg_ptr_t cfg_;
    std::string url_;
    bool& stop_;
    boost::shared_ptr<transport> trans_;
    cfg_subscribers subs_;
};

#endif // CONTROL_SRV_HPP__



