include(ExternalProject)

if (NOT DEPS_LOCATION)
    set(DEPS_LOCATION "${CMAKE_BINARY_DIR}/deps")
endif()
if(TARGET_CROSS)
  ExternalProject_Add(ext.nanomsg
    GIT_REPOSITORY https://github.com/nanomsg/nanomsg.git
    GIT_TAG "1.1.5"
    CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=${_cross_toolchain_file} -DCMAKE_INSTALL_PREFIX=${DEPS_LOCATION} -DWITH_UT=OFF
    PREFIX "${DEPS_LOCATION}"
    INSTALL_DIR "${DEPS_LOCATION}"
)
else()
  ExternalProject_Add(ext.nanomsg
    GIT_REPOSITORY https://github.com/nanomsg/nanomsg.git
    GIT_TAG "1.1.5"
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${DEPS_LOCATION} -DWITH_UT=OFF
    PREFIX "${DEPS_LOCATION}"
    INSTALL_DIR "${DEPS_LOCATION}"
    )
endif()
