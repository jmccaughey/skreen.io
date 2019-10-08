//
//  main.cpp
//  ScreenTest
//
//  Created by John McCaughey on 1/6/14.
//  Copyright (c) 2014 John McCaughey. All rights reserved.
//

#include <iostream>
#include "ApplicationServices/ApplicationServices.h"

int main(int argc, const char * argv[])
{

    CGImageRef screenShot = CGDisplayCreateImage(kCGDirectMainDisplay);
	unsigned long imageWidth = CGImageGetWidth(screenShot);
    unsigned long imageHeight = CGImageGetHeight(screenShot);
    unsigned long bytesPerRow = CGImageGetBytesPerRow(screenShot);
    CGImageRelease(screenShot);
    unsigned long width = CGDisplayPixelsWide(0);
	unsigned long height = CGDisplayPixelsHigh(0);
    printf("virtual: %lux%lu\n", width, height);
//    printf("pixelsHigh: %lu\n", height);
    printf("real: %lux%lu\n", imageWidth, imageHeight);
//    printf("imageWidth %lu\n", imageWidth);
	double scaleFactor = imageWidth / width;
    // TODO: if scalefactor != 1, allocate images_scaled, scaler
    printf("scale: %f\n", scaleFactor);
    if(scaleFactor != 1){
        //scaler = new Scaler(imageWidth, imageHeight, false);
        //images_scaled[0] = (unsigned char*)malloc(scaler->getScaledBufferSize());
        //images_scaled[1] = (unsigned char*)malloc(scaler->getScaledBufferSize());
    }else{
        //scaler = NULL;
    }
    width = imageWidth;
    height = imageHeight;
    //cursor = 0;
    printf("leaving reset\n");
    return 0;
}

