#include "PlatformScreen.h"
#include <sys/time.h>
#include <string.h>

PlatformScreen::PlatformScreen(){
	mouseX = -1;
	mouseY = -1;
    width = -1;
    height = -1;
    scaleFactor = -1.0;
    images[0] = NULL;
    images[1] = NULL;
}
PlatformScreen::~PlatformScreen(){
	free(images[0]);
	free(images[1]);
}
int PlatformScreen::getWidth(){
    if(width == -1){
        reset();
    }
    return width;
}
int PlatformScreen::getHeight(){
    if(height == -1){
        reset();
    }
    return height;
}
void PlatformScreen::reset(){
    printf("starting reset\n");
    // capture a screen shot to discover the resolution
    // because in "Retina" case, CGDisplayPixelsWide reports
    // 1/2 actual pixels reported by CGImageGetWidth
    CGImageRef screenShot = CGDisplayCreateImage(kCGDirectMainDisplay);
	unsigned long imageWidth = CGImageGetWidth(screenShot);
    unsigned long imageHeight = CGImageGetHeight(screenShot);
    bytesPerRow = CGImageGetBytesPerRow(screenShot);
    CGImageRelease(screenShot);
    width = CGDisplayPixelsWide(0);
	height = CGDisplayPixelsHigh(0);
	scaleFactor = imageWidth / width;
    width = imageWidth;
    height = imageHeight;
    int bufferSize = width*height*4;
	if(images[0] != NULL){
        free(images[0]);
        free(images[1]);
    }
    images[0] = malloc(bufferSize);
	images[1] = malloc(bufferSize);
    printf("leaving reset\n");
}
void * PlatformScreen::getCurrentBuffer(){
	return images[cursor];
}
void * PlatformScreen::getPreviousBuffer(){
	return images[cursor == 1 ? 0 : 1];
}
void PlatformScreen::doScreenShot(){
    if (scaleFactor < 0 || height < 0 || width < 0) {
        reset();
    }
    // if screen size has changed, call reset
	CGImageRef screenShot = CGDisplayCreateImage(kCGDirectMainDisplay);
	unsigned long imageWidth = CGImageGetWidth(screenShot);
    unsigned long imageHeight = CGImageGetHeight(screenShot);
    bytesPerRow = CGImageGetBytesPerRow(screenShot);
    if(width != imageWidth || height != imageHeight){
        // resolution has changed
        printf("resolution changed\n");
        reset();
    }
    if(cursor == 1){
		cursor = 0;
	}else{
		cursor = 1;
	}
       
    CGDataProviderRef provider = CGImageGetDataProvider(screenShot);
    CFDataRef dataRef = CGDataProviderCopyData(provider);
    size_t sourceLength = CFDataGetLength(dataRef);
    printf("source length: %zu\n", sourceLength);
    printf("expected size: %d\n", width*height*4);
    
    CFRange range = CFRangeMake(0, sourceLength);
    // TODO FIXME XXX: CGDataProviderCopyData already copied the data -- how to avoid
    // doing it again. Directly accesing the point via CFDataGetBytePtr
    // seemed to cause memory leak
    CFDataGetBytes(dataRef, range, (unsigned char*)(images[cursor]));
    
    //uint8_t* pixels = (uint8_t*)CFDataGetBytePtr(dataRef);
    //memcpy(images[cursor], pixels, sourceLength);
    
    //CFRelease(dataRef);
    //printf("returned form releasing dataref\n");
    //CFRelease(provider);
    //printf("returned form releasing provider\n");
    
    CGImageRelease(screenShot);
    printf("returned form releasing image\n");
    CFRelease(dataRef);
    
	/*
	struct timeval t;
	gettimeofday(&t, NULL);
	long long msecStart = t.tv_sec * 1000 + t.tv_usec / 1000;
	gettimeofday(&t, NULL);
	long total = (t.tv_sec * 1000 + t.tv_usec / 1000) - msecStart;
	printf("scrape took about %d milliseconds\n", (int)total);
    */
    
}
int PlatformScreen::updateMousePosition(int* x, int* y){
	CGEventRef event = CGEventCreate(nil);
	CGPoint loc = CGEventGetLocation(event);
	*x = loc.x * scaleFactor;
	*y = loc.y * scaleFactor;
	CFRelease(event);
	if(*x != mouseX || *y != mouseY){
		mouseX = *x;
		mouseY = *y;
		return 1;
	}
	return 0;
}
