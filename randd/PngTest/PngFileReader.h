//
//  PngFileReader.h
//  PngTest
//
//  Created by John McCaughey on 1/8/14.
//  Copyright (c) 2014 John McCaughey. All rights reserved.
//

#ifndef __PngTest__PngFileReader__
#define __PngTest__PngFileReader__

extern "C" {
#include "/opt/local/include/png.h"
}

#include <stdlib.h>
    
class PngFileReader{

public:
    PngFileReader(const char* path);
    unsigned int getWidth();
    unsigned int getHeight();
    unsigned char* getBuffer();
    unsigned char getColorType();
    int getBytesPerPixel();
    unsigned char getBitDepth();
private:
    unsigned char* buffer;
    unsigned char header[8];
    png_byte color_type;
    png_byte bit_depth;
    
    png_structp png_ptr;
    png_infop info_ptr;
    unsigned int width;
    unsigned int height;
    
    
};


#endif /* defined(__PngTest__PngFileReader__) */
