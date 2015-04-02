#ifndef CONFIG_IFACE_H
#define CONFIG_IFACE_H

#include <boost/shared_ptr.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <utility>

typedef boost::shared_ptr<boost::property_tree::ptree> cfg_ptr_t;

typedef struct
{
    int one;
    int two;
} param_pair_t;

// x, y
static const param_pair_t res_modes[] =
{
    {1280,800}
    ,{1024,780}
    ,{640,480}
    ,{320,240}
};
const size_t RES_MODE_MAX = sizeof(res_modes)/sizeof(res_modes[0]);


// m, t
static const param_pair_t bch_modes[] =
{
    {0,0}
    ,{5,3}
    ,{5,4}
    ,{7,5}
};
const size_t BCH_PRESET_MAX = sizeof(bch_modes)/sizeof(bch_modes[0]);

typedef std::vector< std::pair<int, int> > int_pair_t;

struct profile
{
    profile()
        : name_("")
          , bw_(0)
          , fps_(0)
          , jpeg_quality_(0)
          , bch_idx_(0)
          , res_idx_(0)
    {
    }

    profile(const std::string& name, int bw = 0, int fps = 0, int jpgq = 0
            , int bchidx = 0, int residx = 0)
        : name_(name), bw_(bw), fps_(fps), jpeg_quality_(jpgq)
          , bch_idx_(bchidx), res_idx_(residx)
    {
    }

    profile(const profile& p)
         : name_(p.name_), bw_(p.bw_), fps_(p.fps_), jpeg_quality_(p.jpeg_quality_)
          , bch_idx_(p.bch_idx_), res_idx_(p.res_idx_)
    {
    }

    profile& operator=(const profile& rhs)
    {
        name_ = rhs.name_;
        bw_ = rhs.bw_;
        fps_ = rhs.fps_;
        jpeg_quality_ = rhs.jpeg_quality_;
        bch_idx_ = rhs.bch_idx_;
        res_idx_ = rhs.res_idx_;

        return *this;
    }

    // compare two profiles without taking name in count
    bool operator==(const profile& rhs) const
    {
        return (bw_ == rhs.bw_
                && fps_ == rhs.fps_
                && jpeg_quality_ == rhs.jpeg_quality_
                && bch_idx_ == rhs.bch_idx_
                && res_idx_ == rhs.res_idx_);
    }

    ~profile()
    {
    }

    std::string name_;
    int bw_;
    int fps_;
    int jpeg_quality_;
    int bch_idx_;
    int res_idx_;
};

typedef std::vector<profile> profile_list_t;

static const std::string CONFIG_FILE_NAME("settings.json");
static const std::string PROPERTIES_FILE_NAME("profiles.json");

// read config file
// true - ok
// false - otherwise
bool read_config_file(cfg_ptr_t cfg,
        const std::string & fn);

bool write_config_file(const cfg_ptr_t cfg,
        const std::string & fn);

void init_config_defaults(cfg_ptr_t cfg);

boost::shared_ptr<profile_list_t> get_profile_list(const cfg_ptr_t pfiles);

boost::shared_ptr<int_pair_t> get_resolution_list();

boost::shared_ptr<int_pair_t> get_bch_mode_list();

int find_index(const boost::shared_ptr<int_pair_t> lst, const std::pair<int, int>& val);
int find_index(const boost::shared_ptr<profile_list_t> lst, const profile& p);

#endif /* end of include guard: CONFIG_IFACE_H */
