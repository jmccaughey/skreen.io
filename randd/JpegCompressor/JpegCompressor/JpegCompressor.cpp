//
//  JpegCompressor.cpp
//  JpegCompressor
//
//  Created by John McCaughey on 2/25/14.
//  Copyright (c) 2014 John McCaughey. All rights reserved.
//

#include "JpegCompressor.h"

unsigned int JpegCompressor::compress(unsigned char* source, unsigned char* dest,
                                      J_COLOR_SPACE colorspace, deskblast::Region* region, unsigned int outBufferSize,
                                      int qualityPercent){
    int jpegSize;
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    my_jpeg_mem_dest(&cinfo, dest, outBufferSize);
    if(colorspace == JCS_GRAYSCALE){
        cinfo.input_components = 1;
    }else if(colorspace == JCS_RGB || colorspace == JCS_EXT_BGR || colorspace == JCS_EXT_RGB){
        cinfo.input_components = 3;
    }else{
        cinfo.input_components = 4;
    }
    cinfo.in_color_space = colorspace;
    cinfo.image_width = region->getWidth();
    cinfo.image_height = region->getHeight();
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, qualityPercent, TRUE);
    jpeg_start_compress(&cinfo, TRUE);
    JSAMPROW row_pointer[1];
    int line = 0;
    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = region->getScanline(cinfo.next_scanline, source);
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
        line++;
    }
    jpeg_finish_compress(&cinfo);
    jpegSize = my_jpeg_mem_size(&cinfo);
    jpeg_destroy_compress(&cinfo);
    return jpegSize;
}

