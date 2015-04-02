include(ExternalProject)

if (NOT DEPS_LOCATION)
    set(DEPS_LOCATION "${CMAKE_BINARY_DIR}/deps")
endif()

if (TARGET_CROSS)
    ExternalProject_Add(ext.opencv
        DEPENDS envpkg
        GIT_REPOSITORY https://github.com/Itseez/opencv.git
        GIT_TAG "3.0.0-beta"
        CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=${_cross_toolchain_file} -DENABLE_PRECOMPILED_HEADERS=OFF -DCMAKE_INSTALL_PREFIX=${DEPS_LOCATION} -DBUILD_JPEG=OFF -DWITH_CUDA=OFF -DWITH_OPENCL=ON -DWITH_OPENGL=ON -DWITH_GSTREAMER_0_10=ON -DWITH_IPP=OFF -DWITH_TIFF=OFF -DBUILD_opencv_apps=OFF -DBUILD_DOCS=OFF -DBUILD_TESTS=OFF -DBUILD_PERF_TESTS=OFF -DBUILD_opencv_python2=OFF -DBUILD_opencv_python3=OFF
        PREFIX "${DEPS_LOCATION}"
        INSTALL_DIR "${DEPS_LOCATION}"
        )
else()
    ExternalProject_Add(ext.opencv
        DEPENDS envpkg
        GIT_REPOSITORY https://github.com/Itseez/opencv.git
        GIT_TAG "3.0.0-beta"
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${DEPS_LOCATION} -DBUILD_JPEG=OFF -DWITH_CUDA=ON -DWITH_OPENCL=ON -DWITH_OPENGL=ON -DWITH_IPP=ON -DWITH_GTK=ON -DWITH_QT=OFF -DBUILD_opencv_apps=OFF -DBUILD_DOCS=OFF -DBUILD_TESTS=OFF -DBUILD_PERF_TESTS=OFF -DBUILD_opencv_python2=OFF -DBUILD_opencv_python3=OFF

        PREFIX "${DEPS_LOCATION}"
        INSTALL_DIR "${DEPS_LOCATION}"
        )
endif()