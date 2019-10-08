//
//  PngCompressor.cpp
//  PngMemoryTest
//
//  Created by John McCaughey on 2/20/14.
//  Copyright (c) 2014 John McCaughey. All rights reserved.
//

#include "PngCompressor.h"

PngCompressor::PngCompressor(unsigned int width, unsigned int height, bool grayscale){
    this->width = width;
    this->height = height;
    this->grayscale = grayscale;
}
unsigned int PngCompressor::compress(unsigned char * source, unsigned char * dest, unsigned int destLength){
    this->outPtr = dest;
    this->destLength = destLength;
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    
    if (!png_ptr)
        return -1;
    
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
        return -1;
    
    if (setjmp(png_jmpbuf(png_ptr)))
        return -1;
    
    png_set_write_fn(png_ptr,
                     this, handleWrite,
                     user_flush_data);
    
    if (setjmp(png_jmpbuf(png_ptr)))
        return -1;
    png_set_IHDR(png_ptr, info_ptr, width, height,
                 8,
                 grayscale ? PNG_COLOR_TYPE_GRAY : PNG_COLOR_TYPE_RGB,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_BASE);

    png_set_compression_level(png_ptr, 4);
    
    png_write_info(png_ptr, info_ptr);
    if (setjmp(png_jmpbuf(png_ptr))){
        printf("error writing data\n");
        return -1;
    }
    int bytesScale = grayscale ? 1 : 3;
    for(int i = 0; i < height; i++){
        png_write_row(png_ptr, &source[i * width * bytesScale]);
    }
    if (setjmp(png_jmpbuf(png_ptr)))
        return -1;
    png_write_end(png_ptr, NULL);
    return destCursor;
}

void PngCompressor::doWrite(png_structp png_ptr,
                            png_bytep data, png_size_t length){
    printf("do write %zu\n", length);
    if((length + destCursor) < this->destLength){
        memcpy(this->outPtr + destCursor, data, length);
        destCursor += length;
    }else{
        png_error(png_ptr, "insufficient buffer length");
    }
}

void PngCompressor::handleWrite(png_structp png_ptr,
                                       png_bytep data, png_size_t length){

    PngCompressor* compressor = (PngCompressor*)png_get_io_ptr(png_ptr);
    compressor->doWrite(png_ptr, data, length);
}

