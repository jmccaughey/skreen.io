//
//  PngFileReader.cpp
//  PngTest
//
//  Created by John McCaughey on 1/8/14.
//  Copyright (c) 2014 John McCaughey. All rights reserved.
//

#include "PngFileReader.h"
//#include <stdio.h>

PngFileReader::PngFileReader(const char* path){
    
    FILE *fp = fopen(path, "rb");
    if(!fp){
        return;
    }
    fread(header, 1, 8, fp);
    if (::png_sig_cmp(header, 0, 8)){
        return;
    }
    
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    
    if (!png_ptr)
        throw;//abort_("[read_png_file] png_create_read_struct failed");
    
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
        throw;//abort_("[read_png_file] png_create_info_struct failed");
    
    if (setjmp(png_jmpbuf(png_ptr)))
        throw;//abort_("[read_png_file] Error during init_io");
    
    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);
    
    png_read_info(png_ptr, info_ptr);
    
    width = png_get_image_width(png_ptr, info_ptr);
    height = png_get_image_height(png_ptr, info_ptr);
    color_type = png_get_color_type(png_ptr, info_ptr);
    bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    unsigned int rowSizeInBytes = png_get_rowbytes(png_ptr, info_ptr);
    
    png_set_interlace_handling(png_ptr);
    png_read_update_info(png_ptr, info_ptr);
    
    /* read file */
    if (setjmp(png_jmpbuf(png_ptr)))
        throw;//abort_("[read_png_file] Error during read_image");
    
    buffer = (uint8_t*) malloc(sizeof(uint8_t) * height * rowSizeInBytes);
    printf("height: %d\n", height);
    
    // TODO: single malloc for buffer
    //uint8_t* buffer =
    
//    for (y=0; y<height; y++)
//        row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(png_ptr,info_ptr));
//    
//    png_read_image(png_ptr, row_pointers);
//
    for(int i = 0; i < height; i++){
        png_read_row(png_ptr, buffer + (i * rowSizeInBytes), NULL);
    }
    fclose(fp);
}
unsigned int PngFileReader::getWidth(){ return width; }
unsigned int PngFileReader::getHeight(){ return height; }
unsigned char* PngFileReader::getBuffer(){ return this->buffer; }
unsigned char PngFileReader::getBitDepth(){ return this->bit_depth; }
unsigned char PngFileReader::getColorType(){ return this->color_type; }
int PngFileReader::getBytesPerPixel(){ return
    this->color_type == PNG_COLOR_TYPE_GRAY ? 1 : this->color_type == PNG_COLOR_TYPE_RGB
    ? 3 : 4;
}

