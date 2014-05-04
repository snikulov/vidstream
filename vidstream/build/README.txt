
Externals:

Will be installed to /usr/local

ext/ffmpeg - required for opencv to play files (if not can be dropped)
 - build
# ./configure --enable-gpl --enable-shared --enable-nonfree
# make
# sudo make install

ext/libjpeg-turbo - for some jpeg manipulation, will be used in opencv
 - build
# autoreconf -fiv
# ./configure --with-jpeg8 --prefix=/usr/local
# make
# sudo make install

ext/opencv - for video capture and image manipulation
- build
# cmake <path to opencv source> -DBUILD_JPEG=OFF -DCMAKE_INSTALL_PREFIX=/usr/local -DJPEG_INCLUDE_DIR=/usr/local -DJPEG_LIBRARY=/usr/local/lib64/libjpeg.so.8
# make
# sudo make install
