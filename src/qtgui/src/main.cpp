#include "mainvideoiowindow.h"
#include <QApplication>

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/loglevel.h>
#include <log4cplus/configurator.h>

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


    QApplication a(argc, argv);

    MainVideoIoWindow w;
    w.show();

    return a.exec();
}
