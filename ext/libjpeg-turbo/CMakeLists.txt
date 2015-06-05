include(ExternalProject)

if (NOT DEPS_LOCATION)
    set(DEPS_LOCATION "${CMAKE_BINARY_DIR}/deps")
endif()
if (TARGET_CROSS)
ExternalProject_Add(ext.libjpeg-turbo
    SVN_REPOSITORY http://svn.code.sf.net/p/libjpeg-turbo/code/tags/1.4.0
    PREFIX "${DEPS_LOCATION}"
    CONFIGURE_COMMAND autoreconf --warnings=cross -fiv <SOURCE_DIR> 
        COMMAND <SOURCE_DIR>/configure --with-jpeg8 --prefix=${DEPS_LOCATION} --host=arm-poky-linux-gnueabi
    BUILD_COMMAND ${MAKE}
    BUILD_IN_SOURCE 1
    INSTALL_DIR "${DEPS_LOCATION}"
    )
else()
ExternalProject_Add(ext.libjpeg-turbo
    SVN_REPOSITORY http://svn.code.sf.net/p/libjpeg-turbo/code/tags/1.4.0
    PREFIX "${DEPS_LOCATION}"
    CONFIGURE_COMMAND autoreconf -fiv <SOURCE_DIR>
        COMMAND <SOURCE_DIR>/configure --with-jpeg8 --prefix=${DEPS_LOCATION}
    BUILD_COMMAND ${MAKE}
    BUILD_IN_SOURCE 1
    INSTALL_DIR "${DEPS_LOCATION}"
    )
endif()

