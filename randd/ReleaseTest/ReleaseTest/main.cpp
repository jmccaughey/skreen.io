//
//  main.cpp
//  ReleaseTest
//
//  Created by John McCaughey on 10/4/13.
//  Copyright (c) 2013 John McCaughey. All rights reserved.
//
#include "ApplicationServices/ApplicationServices.h"
#include <iostream>

int main(int argc, const char * argv[])
{

    // if screen size has changed, call reset
    
    unsigned char* buff = (unsigned char*)malloc(2880*1800*4);
    
    for(int i = 0; i < 1; i++){
	CGImageRef screenShot = CGDisplayCreateImage(kCGDirectMainDisplay);
	unsigned long imageWidth = CGImageGetWidth(screenShot);
    unsigned long imageHeight = CGImageGetHeight(screenShot);
    
    //bytesPerRow = CGImageGetBytesPerRow(screenShot);
    /*
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
    */
    CGDataProviderRef provider = CGImageGetDataProvider(screenShot);
    CFDataRef dataRef = CGDataProviderCopyData(provider);
    size_t sourceLength = CFDataGetLength(dataRef);
    printf("source length: %zu\n", sourceLength);
    //printf("expected size: %d\n", width*height*4);
    uint8_t* pixels = (uint8_t*)CFDataGetBytePtr(dataRef);
    memcpy(buff, pixels, sourceLength);
        //CFRange range = CFRangeMake(0, CFDataGetLength(dataRef));
        //CFDataGetBytes(dataRef, range, buff);
  
        CGImageRelease(screenShot);
        printf("returned form releasing image\n");

        
        CFRelease(dataRef);
        printf("returned form releasing dataref\n");
        
        //CGDataProviderRelease(provider);
        //printf("returned form releasing provider\n");

        //CFRelease(dataRef);
        //printf("returned form releasing dataref\n");
        

  
    //CFRelease(provider);
    //printf("returned form releasing provider\n");
    
    }
    free(buff);
    return 0;
}

