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
5. Add PPA to install QT5:
# sudo apt-add-repository ppa:ubuntu-sdk-team/ppa
# sudo apt-get update
6. Install QT5: sudo apt-get install qtdeclarative5-dev qtdeclarative5-dev-tools
7. Install boost 1.55
# Install Boost 1.55.0 Ubuntu 12.04

#1
sudo apt-get update
sudo apt-get install build-essential g++ python-dev autotools-dev libicu-dev build-essential libbz2-dev

# Update gcc/g++ - suport: -std=c++0x or -std=gnu++0x or -std=c++11 or -std=gnu++11
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt-get update
sudo apt-get install gcc-4.8
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.8 50
sudo apt-get install g++-4.8
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.8 50

#2
wget -O boost_1_55_0.tar.gz http://sourceforge.net/projects/boost/files/boost/1.55.0/boost_1_55_0.tar.gz/download
tar xzvf boost_1_55_0.tar.gz
cd boost_1_55_0/

#3 --- Add libraries!!!
./bootstrap.sh --with-libraries=system,filesystem,date_time,thread,regex,log,iostreams --prefix=/usr/local

#4
./b2 install
#./b2 --build-type=complete --toolset=gcc threading=multi --layout=tagged install

#5
sudo sh -c 'echo "/usr/local/lib" >> /etc/ld.so.conf.d/local.conf'

#6
sudo ldconfig

#Upgrade Ubuntu 12.04 lts to 14.04
#sudo apt-get install update-manager-core
#sudo vi /etc/update-manager/release-upgrades
#alter prompt=lts
#sudo do-release-upgrade -d
-------------------------------------------------
Development tools:
0. Installation: sudo apt-get install valgrind

1. valgrind : http://valgrind.org/docs/manual/manual.html
G_SLICE=always-malloc G_DEBUG=gc-friendly  valgrind -v --tool=memcheck --leak-check=full --num-callers=40 --log-file=valgrind.log $(which <program>) <arguments>

1a. profiling: http://valgrind.org/docs/manual/cl-manual.html
use these options for callgrind: --dump-instr=yes --simulate-cache=yes --collect-jumps=yes

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
# cmake -DCMAKE_C_COMPILER=/usr/share/clang/scan-build/ccc-analyzer -DCMAKE_CXX_COMPILER=/usr/share/clang/scan-build/ccc-analyzer .
# scan-build make

3. IDE:
TBD

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
1 cd build
# cmake .. -DCMAKE_C_COMPILER=/usr/bin/gcc -DCMAKE_CXX_COMPILER=/usr/bin/g++
# cmake ..
# make

./capture --file=/media/sf_share/test.avi 
