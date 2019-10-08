//
//  main.cpp
//  JpegCompressor
//
//  Created by John McCaughey on 2/25/14.
//  Copyright (c) 2014 John McCaughey. All rights reserved.
//

#include "PngFileReader.h"
#include "JpegCompressor.h"

#define QUALITY 40

int main(int argc, const char * argv[]){
    
    std::string source_root("/Users/jmccaughey/Desktop/mix.half");
    std::string source = source_root + ".png";
    PngFileReader* pngFileReader = new PngFileReader(source.c_str());
    printf("width:%d x height:%d\n", pngFileReader->getWidth(),
           pngFileReader->getHeight());
    unsigned int width = pngFileReader->getWidth();
    unsigned int height = pngFileReader->getHeight();
    unsigned char type = pngFileReader->getColorType();
    
    unsigned char* bytes = pngFileReader->getBuffer();
    unsigned int outBufferLength = (width*height)/5;
    printf("out buffer length %d\n", outBufferLength);
    unsigned char* outBytes = (unsigned char*)malloc(outBufferLength);
    //JpegCompressor* compressor = new JpegCompressor();
    deskblast::Region* region = new deskblast::Region(0, 0, width, height, width,
                                    0, pngFileReader->getBytesPerPixel());
    unsigned int length = JpegCompressor::compress(bytes, outBytes,
                type == 0 ? JCS_GRAYSCALE : JCS_EXT_RGB, region, outBufferLength
                                               , QUALITY);
    printf("jpeg size: %d\n", length);
    if(length > 0){
        std::string target(source_root);
        char qualityBuff[20];
        sprintf(qualityBuff, ".%d", QUALITY);
        target += qualityBuff;
        target += ".jpg";
        printf("out: %s\n", target.c_str());
        FILE* fp = fopen(target.c_str(), "wb");
        for(int i = 0; i < length; i++){
            putc(outBytes[i], fp);
        }
        fclose(fp);
    }
    return 0;
}

