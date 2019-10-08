//
//  PngFileWriter.h
//  PngTest
//
//  Created by John McCaughey on 1/9/14.
//  Copyright (c) 2014 John McCaughey. All rights reserved.
//

#ifndef __PngTest__PngFileWriter__
#define __PngTest__PngFileWriter__

#include <iostream>

extern "C" {
#include "/opt/local/include/png.h"
}

#include <stdlib.h>


class PngFileWriter{
public:
    PngFileWriter(char* path, unsigned char * bytes, unsigned int width,
                  unsigned int height, unsigned char colorType,
                  unsigned char bitDepth);
private:
    
};

#endif /* defined(__PngTest__PngFileWriter__) */
