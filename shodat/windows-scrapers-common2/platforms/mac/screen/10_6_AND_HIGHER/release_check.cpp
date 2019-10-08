#include "PlatformScreen.h"
#include <sys/time.h>
#include <string.h>
#include "ApplicationServices/ApplicationServices.h"


int main(int argc, char** argv){
    printf("starting reset\n");
    void* images[2];
    // capture a screen shot to discover the resolution
    // because in "Retina" case, CGDisplayPixelsWide reports
    // 1/2 actual pixels reported by CGImageGetWidth
    CGImageRef screenShot = CGDisplayCreateImage(kCGDirectMainDisplay);
	unsigned long imageWidth = CGImageGetWidth(screenShot);
    unsigned long imageHeight = CGImageGetHeight(screenShot);
    //bytesPerRow = CGImageGetBytesPerRow(screenShot);
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
	CGImageRef screenShot = CGDisplayCreateImage(kCGDirectMainDisplay);
	unsigned long imageWidth = CGImageGetWidth(screenShot);
    unsigned long imageHeight = CGImageGetHeight(screenShot);
    //bytesPerRow = CGImageGetBytesPerRow(screenShot);
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
    uint8_t* pixels = (uint8_t*)CFDataGetBytePtr(dataRef);
    memcpy(images[cursor], pixels, sourceLength);
    
    //CFRelease(dataRef);
    //printf("returned form releasing dataref\n");
    //CFRelease(provider);
    //printf("returned form releasing provider\n");
    
    CGImageRelease(screenShot);
    printf("returned form releasing image\n");
    
    

}