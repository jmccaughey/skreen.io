//
//  main.cpp
//  PngMemoryTest
//
//  Created by John McCaughey on 2/14/14.
//  Copyright (c) 2014 John McCaughey. All rights reserved.
//

#include "PngCompressor.h"
#include "PngFileReader.h"

int total = 0;

void user_write_data(png_structp png_ptr,
                     png_bytep data, png_size_t length){
    printf("got write data: %zu bytes\n", length);
    unsigned char* out = (unsigned char*)png_get_io_ptr(png_ptr);
    for(int i = 0; i < length; i++){
        out[i+total] = data[i];
    }
    total += length;
}

void user_flush_data(png_structp png_ptr){
    printf("got flush data\n");
}

//#define WIDTH 800
//#define HEIGHT 600

int main(int argc, const char * argv[]){
    
    PngFileReader* pngFileReader = new PngFileReader(
        "/Users/jmccaughey/Desktop/xcode.gray.png");
    printf("width:%d x height:%d\n", pngFileReader->getWidth(), pngFileReader->getHeight());
    unsigned int width = pngFileReader->getWidth();
    unsigned int height = pngFileReader->getHeight();
    unsigned char type = pngFileReader->getColorType();
    
    unsigned char* bytes = pngFileReader->getBuffer();
    unsigned int outBufferLength = width*height*10;
    unsigned char* outBytes = (unsigned char*)malloc(outBufferLength);

    PngCompressor* pngCompressor = new PngCompressor(width, height,
                    type == PNG_COLOR_TYPE_GRAY);
    total = pngCompressor->compress(bytes, outBytes, outBufferLength);
  
    FILE* fp = fopen("/Users/jmccaughey/Desktop/test2.png", "wb");
    for(int i = 0; i < total; i++){
        fputc(outBytes[i], fp);
    }
    fclose(fp);
    free(outBytes);
    delete pngCompressor;
    delete pngFileReader;
}
