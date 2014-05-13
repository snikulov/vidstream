Before you begin:
1. Install git: sudo apt-get install git
2. Do one-time git setup: http://git-scm.com/book/en/Getting-Started-First-Time-Git-Setup
3. Get sources from repository
git clone --recursive <git>
             or
git clone <git>
git submodule update --init --recursive
-------------------------------------------------
Install dependencies for Ubuntu:
1. Install boost libraries (dynamic linking): sudo apt-get install libboost-all-dev
2. Install dependencies for libjpeg-turbo: sudo apt-get install autoconf libtool nasm
3. Install cmake: sudo apt-get install cmake
4. install GTK (for openCV windows support): sudo apt-get install libgtk2.0-dev pkg-config
-------------------------------------------------
Development tools:
0. Installation: sudo apt-get install valgrind

1. valgrind : http://valgrind.org/docs/manual/manual.html
1a. profiling: http://valgrind.org/docs/manual/cl-manual.html
use these options for callgrind: --dump-instr=yes --simulate-cache=yes --collect-jumps=yes
1b. 

2. clang
2a. install clang
2b. scan-build with clang: http://clang-analyzer.llvm.org/scan-build.html#recommended_autoconf
2c. how to build wt=ith clang^
# export CC=/usr/bin/clang
# export CXX=/usr/bin/clang++
# cmake -D_CMAKE_TOOLCHAIN_PREFIX=llvm- ..
or
# cmake -DCMAKE_C_COMPILER=/path/to/clang -DCMAKE_CXX_COMPILER=/path/to/clang++ ..
or can also use ccmake, which provides a curses interface to configure CMake variables for lazy people.

2d. scan-build
# apt-get install clang
# cmake -DCMAKE_C_COMPILER=/usr/share/clang/scan-build/ccc-analyzer ..
# scan-build make


3. IDE:


-------------------------------------------------
Externals:

Will be installed to /usr/local

1. install yasm or use --disable-yasm for ffmpeg in next step
TBD

2. ext/ffmpeg - required for opencv to play files (if not can be dropped)
 - build
# ./configure --enable-gpl --enable-shared --enable-nonfree
# make
# sudo make install

3. ext/libjpeg-turbo - for some jpeg manipulation, will be used in opencv
 - build
# autoreconf -fiv
# ./configure --with-jpeg8 --prefix=/usr/local
# make
# sudo make install

4. ext/opencv - for video capture and image manipulation
- build
For 64 bit system:
# cmake <path to opencv source> -DBUILD_JPEG=OFF -DCMAKE_INSTALL_PREFIX=/usr/local -DJPEG_INCLUDE_DIR=/usr/local -DJPEG_LIBRARY=/usr/local/lib64/libjpeg.so.8
or for 32bit system:
# cmake <path to opencv source> -DBUILD_JPEG=OFF -DCMAKE_INSTALL_PREFIX=/usr/local -DJPEG_INCLUDE_DIR=/usr/local -DJPEG_LIBRARY=/usr/local/lib/libjpeg.so.8
# make
# sudo make install

5. Build system 
# cmake .
# make