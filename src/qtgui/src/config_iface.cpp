#include "config_iface.hpp"


#include <boost/foreach.hpp>

boost::shared_ptr<profile_list_t> get_profile_list(const cfg_ptr_t pt)
{
    using namespace boost::property_tree;
    const boost::property_tree::ptree& tree = *pt;

    boost::shared_ptr<profile_list_t> lst(new profile_list_t);

    BOOST_FOREACH( ptree::value_type const& v, tree.get_child("profiles") )
    {
       profile tmp(
               v.second.get<std::string>("name")
               , v.second.get<int>("bw")
               , v.second.get<int>("fps")
               , v.second.get<int>("jpg_quality")
               , v.second.get<int>("bch_idx")
               , v.second.get<int>("res_idx")
               );
       lst->push_back(tmp);
    }

    return lst;
}

static boost::shared_ptr<int_pair_t> get_pair_from_array(const param_pair_t* arr, size_t len)
{
    boost::shared_ptr<int_pair_t> lst(new int_pair_t);
    for(size_t i = 0; i < len; ++i)
    {
        lst->push_back(std::make_pair(arr[i].one, arr[i].two));
    }
    return lst;
}

boost::shared_ptr<int_pair_t> get_resolution_list()
{
    return get_pair_from_array(res_modes, RES_MODE_MAX);
}

boost::shared_ptr<int_pair_t> get_bch_mode_list()
{
    return get_pair_from_array(bch_modes, BCH_PRESET_MAX);
}

int find_index(const boost::shared_ptr<int_pair_t> lst, const std::pair<int, int>& val)
{
    int ret_val = -1;
    if (lst)
    {
        const int_pair_t& rvec = *lst;
        for(int i=0; i < rvec.size(); ++i)
        {
            if (rvec[i] == val)
            {
                return i;
            }
        }
    }
    return ret_val;
}

int find_index(const boost::shared_ptr<profile_list_t> lst, const profile& p)
{
    int ret_val = -1;
    if (lst)
    {
        const profile_list_t& rvec = *lst;
        for(int i=0; i < rvec.size(); ++i)
        {
            if (rvec[i] == p)
            {
                return i;
            }
        }
    }
    return ret_val;
}
