include(ExternalProject)

if (NOT DEPS_LOCATION)
    set(DEPS_LOCATION "${CMAKE_BINARY_DIR}/deps")
endif()

ExternalProject_Add(ext.ffmpeg
    GIT_REPOSITORY https://github.com/FFmpeg/FFmpeg.git
    GIT_TAG n2.5.2
    PREFIX "${DEPS_LOCATION}"
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND <SOURCE_DIR>/configure --enable-gpl --enable-shared --enable-nonfree --prefix=${DEPS_LOCATION}
    BUILD_COMMAND ${MAKE}
    INSTALL_DIR "${DEPS_LOCATION}"
)

