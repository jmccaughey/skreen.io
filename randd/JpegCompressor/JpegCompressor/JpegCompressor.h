//
//  JpegCompressor.h
//  JpegCompressor
//
//  Created by John McCaughey on 2/25/14.
//  Copyright (c) 2014 John McCaughey. All rights reserved.
//

#ifndef __JpegCompressor__JpegCompressor__
#define __JpegCompressor__JpegCompressor__

#include <iostream>

#include "Region.h"

extern "C" {
#include "jpeglib.h"
#include "jpeg_mem_dest.h"
}

#define DEFAULT_QUALITY 30

class JpegCompressor{
    
public:
    static unsigned int compress(unsigned char* source, unsigned char* dest,
                                 J_COLOR_SPACE, deskblast::Region* region, unsigned int outBufferSize,
                                 int qualityPercent = DEFAULT_QUALITY);
};



#endif /* defined(__JpegCompressor__JpegCompressor__) */
