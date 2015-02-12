#ifndef CONFIG_IFACE_H
#define CONFIG_IFACE_H

#include <boost/shared_ptr.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

typedef boost::shared_ptr<boost::property_tree::ptree> cfg_ptr_t;

// read config file
// true - ok
// false - otherwise
bool read_config_file(const std::string & fname, cfg_ptr_t cfg);

void init_config_defaults(cfg_ptr_t cfg);


#endif /* end of include guard: CONFIG_IFACE_H */
