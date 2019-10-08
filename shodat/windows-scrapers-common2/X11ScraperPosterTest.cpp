#include "X11ScraperPoster.h"
#include "ScraperPosterMetaData.h"

#include <signal.h>

//john@johnm-laptop:~/shodat/windows-scrapers-common2$ g++ -o x11scraper X11ScraperPosterTest.cpp  X11ScraperPoster.cpp ScraperPosterMetaData.cpp -I../md5/ -I../jpeg_integration2/ -I../libjpeg-turbo/

// g++ -o x11scraper X11ScraperPosterTest.cpp FilePoster.cpp X11Screen.cpp X11ScraperPoster.cpp ScraperPosterMetaData.cpp Region.cpp HashString.cpp Trace.cpp SubtileInfo.cpp ../md5/md5.cpp ../jpeg_integration2/jpeg_mem_dest.cpp ../GifEncoder2/gifencode.c  ../GifEncoder2/GifSpriter.cpp  -I../md5/ -I../jpeg_integration2/ -I../libjpeg-turbo/ -lX11 -lXrandr -ljpeg

/*
 g++ -O2 -o x11scraper X11ScraperPosterTest.cpp FilePoster.cpp X11Screen.cpp X11ScraperPoster.cpp ScraperPosterMetaData.cpp Region.cpp HashString.cpp Trace.cpp SubtileInfo.cpp ../md5/md5.cpp ../jpeg_integration2/jpeg_mem_dest.cpp ../GifEncoder2/gifencode.c  ../GifEncoder2/GifSpriter.cpp  -I../md5/ -I../jpeg_integration2/ -I../libjpeg-turbo/ -lX11 -lXrandr ../libjpeg-turbo/.libs/libjpeg.a 
*/

// john@johnm-laptop:~/shodat/windows-scrapers-common2$ g++ -std=c++0x -D_GLIBCXX_USE_NANOSLEEP -O2 -o x11scraper X11ScraperPosterTest.cpp ../randd/CurlPoster.cpp X11Screen.cpp X11ScraperPoster.cpp ScraperPosterMetaData.cpp Region.cpp HashString.cpp Trace.cpp SubtileInfo.cpp ../md5/md5.cpp ../jpeg_integration2/jpeg_mem_dest.cpp ../GifEncoder2/gifencode.c  ../GifEncoder2/GifSpriter.cpp  ../libjpeg-turbo-linux-build/.libs/libjpeg.a -I../md5/ -I../jpeg_integration2/ -I../libjpeg-turbo-linux-build/ -I../libjpeg-turbo/ -lX11 -lXrandr -lcurl

static bool keepRunning = true;

void intHandler(int dummy=0) {
    keepRunning = false;
}

int main(int argc, char** argv) {
	// usage: host port context ssl blastid blasterid key
	if(argc != 4){
		printf("usage: %s blastid blasterid key\n", argv[0]);
		return 1;
	}
    ScraperPosterMetaData* spmd = new ScraperPosterMetaData("skreen.io", 80, "", false,
            argv[1], argv[2], argv[3]);
    X11ScraperPoster* scraperPoster = new X11ScraperPoster(spmd);
    //int i = 0;
    while(keepRunning){
    	scraperPoster->doScreenScrape();
    	//std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    	usleep(1000*333);
    }
    printf("exited loop\n");
    delete spmd;
    delete scraperPoster;
    return 0;
}
