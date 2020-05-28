#include "mainwindow.h"
#include <QApplication>

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/loglevel.h>
#include <log4cplus/configurator.h>

#include <boost/program_options.hpp>
#include <iostream>

int main(int argc, char *argv[])
{
    try
    {
        log4cplus::PropertyConfigurator::doConfigure("log4cplus.properties");
    }
    catch(const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        exit(0);
    }

    namespace po = boost::program_options;
    po::options_description desc("All options");
    desc.add_options()
        ("settings,s", "enable/disable settings dialog")
        ("help,?", "show help");
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        // print help and exit
        std::cout << desc << "\n";
        return 1;
    }

    bool conf_enable = false;
    if (vm.count("settings"))
    {
        conf_enable = true;
    }

    QApplication a(argc, argv);
    MainWindow w(conf_enable);
    w.show();

    return a.exec();
}
