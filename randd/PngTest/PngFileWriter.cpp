//
//  PngFileWriter.cpp
//  PngTest
//
//  Created by John McCaughey on 1/9/14.
//  Copyright (c) 2014 John McCaughey. All rights reserved.
//

#include "PngFileWriter.h"

PngFileWriter::PngFileWriter(char* path, unsigned char * bytes,
                             unsigned int width, unsigned int height,
                             unsigned char colorType, unsigned char bitDepth){
    FILE *fp = fopen(path, "wb");
    if (!fp)
        throw;// <#expression#>
        //abort_("[write_png_file] File %s could not be opened for writing", file_name);
    
    
    /* initialize stuff */
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    
    if (!png_ptr)
        throw;//abort_("[write_png_file] png_create_write_struct failed");
    
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
        throw;//abort_("[write_png_file] png_create_info_struct failed");
    
    if (setjmp(png_jmpbuf(png_ptr)))
        throw;//abort_("[write_png_file] Error during init_io");
    
    png_init_io(png_ptr, fp);
    
    
    /* write header */
    if (setjmp(png_jmpbuf(png_ptr)))
        throw;//abort_("[write_png_file] Error during writing header");
    
    png_set_IHDR(png_ptr, info_ptr, width, height,
                 bitDepth, colorType, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    
    png_write_info(png_ptr, info_ptr);
    
    /* write bytes */
    if (setjmp(png_jmpbuf(png_ptr)))
        throw;//abort_("[write_png_file] Error during writing bytes");
    
    //png_write_image(png_ptr, row_pointers);
    int bytesPerPixel = colorType == 0 ? 1 : 3;
    for(int i = 0; i < height; i++){
        png_write_row(png_ptr, &bytes[i * width * bytesPerPixel]);
    }
    
    /* end write */
    if (setjmp(png_jmpbuf(png_ptr)))
        throw;//abort_("[write_png_file] Error during end of write");
    
    png_write_end(png_ptr, NULL);
    
    /* cleanup heap allocation */
//    for (y=0; y<height; y++)
//        free(row_pointers[y]);
//    free(row_pointers);
//    
    fclose(fp);
}
