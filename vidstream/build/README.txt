
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
