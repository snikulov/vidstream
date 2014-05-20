#ifndef CFG_NOTIFY_HPP__
#define CFG_NOTIFY_HPP__

class cfg_notify
{
public:
    cfg_notify() {}
    virtual ~cfg_notify() {}

    virtual void cfg_changed(const boost::property_tree::ptree&) = 0;

private:
    /* data */
};
#endif // CFG_NOTIFY_HPP__
