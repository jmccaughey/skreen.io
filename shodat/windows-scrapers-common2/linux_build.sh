g++ -std=c++0x -D_GLIBCXX_USE_NANOSLEEP -O2 -o x11scraper X11ScraperPosterTest.cpp platforms/linux/poster/PlatformPoster.cpp platforms/linux/screen/PlatformScreen.cpp X11ScraperPoster.cpp ScraperPosterMetaData.cpp Region.cpp HashString.cpp Trace.cpp SubtileInfo.cpp ../md5/md5.cpp ../jpeg_integration2/jpeg_mem_dest.cpp ../GifEncoder2/gifencode.c  ../GifEncoder2/GifSpriter.cpp  ../libjpeg-turbo/linux/.libs/libjpeg.a -I../md5/ -I../jpeg_integration2/ -I../libjpeg-turbo/linux/ -I../libjpeg-turbo/ -I./ -Iplatforms/linux/screen/ -Iplatforms/linux/poster/ -lX11 -lXrandr -lcurl	
