include(ExternalProject)

if (NOT DEPS_LOCATION)
    set(DEPS_LOCATION "${CMAKE_BINARY_DIR}/deps")
endif()

ExternalProject_Add(ext.opencv
    DEPENDS envpkg
    GIT_REPOSITORY https://github.com/Itseez/opencv.git
    GIT_TAG "3.0.0-beta"
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${DEPS_LOCATION} -DBUILD_JPEG=OFF -DWITH_QT=ON -DWITH_OPENCL=ON -DWITH_OPENGL=ON -DJPEG_INCLUDE_DIR=${DEPS_LOCATION}/include
    PREFIX "${DEPS_LOCATION}"
    INSTALL_DIR "${DEPS_LOCATION}"
)

