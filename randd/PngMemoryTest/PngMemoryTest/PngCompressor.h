//
//  PngCompressor.h
//  PngMemoryTest
//
//  Created by John McCaughey on 2/20/14.
//  Copyright (c) 2014 John McCaughey. All rights reserved.
//

#ifndef __PngMemoryTest__PngCompressor__
#define __PngMemoryTest__PngCompressor__

#include <iostream>

extern "C" {
#include "/opt/local/include/png.h"
}


class PngCompressor{
    
public:
    PngCompressor(unsigned int width, unsigned int height, bool grayscale);
    unsigned int compress(unsigned char * source, unsigned char * dest, unsigned int destLength);
    static void handleWrite(png_structp png_ptr,
                       png_bytep data, png_size_t length);
    static void user_flush_data(png_structp png_ptr){
        printf("got flush data\n");
    }
private:
    unsigned int width;
    unsigned int height;
    bool grayscale;
    int bytesPerPixel;
    unsigned char* outPtr;
    unsigned int destLength;
    unsigned int destCursor;
    void doWrite(png_structp png_ptr, png_bytep data, png_size_t length);
    
};

#endif /* defined(__PngMemoryTest__PngCompressor__) */
