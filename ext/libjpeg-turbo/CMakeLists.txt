include(ExternalProject)

if (NOT DEPS_LOCATION)
    set(DEPS_LOCATION "${CMAKE_BINARY_DIR}/deps")
endif()

if (TARGET_CROSS)
  ExternalProject_Add(ext.libjpeg-turbo
    GIT_REPOSITORY https://github.com/libjpeg-turbo/libjpeg-turbo.git
    GIT_TAG "2.0.4"
    PREFIX "${DEPS_LOCATION}"
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${DEPS_LOCATION}
    )
else()
  ExternalProject_Add(ext.libjpeg-turbo
    GIT_REPOSITORY https://github.com/libjpeg-turbo/libjpeg-turbo.git
    GIT_TAG "2.0.4"
    PREFIX "${DEPS_LOCATION}"
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${DEPS_LOCATION}
    )
endif()
