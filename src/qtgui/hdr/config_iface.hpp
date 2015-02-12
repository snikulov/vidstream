#ifndef CONFIG_IFACE_H
#define CONFIG_IFACE_H

#include <boost/shared_ptr.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

typedef boost::shared_ptr<boost::property_tree::ptree> cfg_ptr_t;

static const std::string CONFIG_FILE_NAME("settings.json");

// read config file
// true - ok
// false - otherwise
bool read_config_file(cfg_ptr_t cfg,
        const std::string & fn = CONFIG_FILE_NAME);
bool write_config_file(cfg_ptr_t cfg,
        const std::string & fn = CONFIG_FILE_NAME);

void init_config_defaults(cfg_ptr_t cfg);



#endif /* end of include guard: CONFIG_IFACE_H */
