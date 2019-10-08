//
//  main.cpp
//  SwscaleTest
//
//  Created by John McCaughey on 12/27/13.
//  Copyright (c) 2013 John McCaughey. All rights reserved.
//

#include <stdio.h>
#include "Scaler.h"

extern "C" {
#include "libavutil/mathematics.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "/opt/libjpeg-turbo/include/jpeglib.h"
}

static const int BEFORE_WIDTH = 2880;
static const int BEFORE_HEIGHT = 1800;
static int AFTER_WIDTH = BEFORE_WIDTH / 2;
static int AFTER_HEIGHT = BEFORE_HEIGHT / 2;

#define PIXEL_FORMAT PIX_FMT_BGRA

void write_JPEG_file (char * filename, int quality, int image_width,
                      int image_height, unsigned char *image_buffer)
{
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE * outfile;                /* target file */
    JSAMPROW row_pointer[1];        /* pointer to JSAMPLE row[s] */
    int row_stride;                /* physical row width in image buffer */
    
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    
    if ((outfile = ::fopen(filename, "wb")) == NULL) {
        fprintf(stderr, "can't open %s\n", filename);
        exit(1);
    }
    jpeg_stdio_dest(&cinfo, outfile);
    
    cinfo.image_width = image_width;         /* image width and height, in pixels */
    cinfo.image_height = image_height;
    cinfo.input_components = 3;                /* # of color components per pixel */
    cinfo.in_color_space = JCS_RGB;         /* colorspace of input image */
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);
    jpeg_start_compress(&cinfo, TRUE);
    row_stride = image_width * 3;        /* JSAMPLEs per row in image_buffer */
    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = & image_buffer[cinfo.next_scanline * row_stride];
        (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }
    jpeg_finish_compress(&cinfo);
    fclose(outfile);
    jpeg_destroy_compress(&cinfo);
}

int main(int argc, const char * argv[]){
    /*
    struct SwsContext *resize;
    resize = ::sws_getContext(BEFORE_WIDTH, BEFORE_HEIGHT, PIXEL_FORMAT, AFTER_WIDTH, AFTER_HEIGHT, PIXEL_FORMAT,
//                            SWS_BICUBIC // prettier
                            SWS_BILINEAR // faster
                            ,
                            NULL, NULL, NULL);
    AVFrame* frame1 = avcodec_alloc_frame(); // this is your original frame
    AVFrame* frame2 = avcodec_alloc_frame();
    int num_bytes = avpicture_get_size(PIXEL_FORMAT, AFTER_WIDTH, AFTER_HEIGHT);
    int num_bytes1 = avpicture_get_size(PIXEL_FORMAT, BEFORE_WIDTH, BEFORE_HEIGHT);
    uint8_t* frame2_buffer = (uint8_t *)av_malloc(num_bytes*sizeof(uint8_t));
    uint8_t* frame1_buffer = (uint8_t *)av_malloc(num_bytes1*sizeof(uint8_t));
    for(int i = 0; i < num_bytes1; i++){
        if(i % 4 == 0){
            frame1_buffer[i] = 200;
        }else{
            frame1_buffer[i] = 0;
        }
    }
    avpicture_fill((AVPicture*)frame2, frame2_buffer, PIXEL_FORMAT, AFTER_WIDTH, AFTER_HEIGHT);
    avpicture_fill((AVPicture*)frame1, frame1_buffer, PIXEL_FORMAT, BEFORE_WIDTH, BEFORE_HEIGHT);
    sws_scale(resize, frame1->data, frame1->linesize, 0, BEFORE_HEIGHT, frame2->data, frame2->linesize);
   */
     printf("swscale\n");

    Scaler* scaler = new Scaler(BEFORE_WIDTH, BEFORE_HEIGHT, false);
    
    int num_bytes_before = avpicture_get_size(PIXEL_FORMAT, BEFORE_WIDTH, BEFORE_HEIGHT);
    int num_bytes_after = avpicture_get_size(PIXEL_FORMAT, AFTER_WIDTH, AFTER_HEIGHT);

    uint8_t* before_buffer = (uint8_t *)av_malloc(num_bytes_before*sizeof(uint8_t));
    uint8_t* after_buffer = (uint8_t *)av_malloc(num_bytes_after*sizeof(uint8_t));
    for(int i = 0; i < num_bytes_before; i++){
        if(i % 4 == 0){
            before_buffer[i] = 200;
        }else{
            before_buffer[i] = 0;
        }
    }

    scaler->scale(before_buffer, after_buffer);
    delete scaler;
    //write_JPEG_file("/Users/jmccaughey/out_before.jpeg", 90, BEFORE_WIDTH, BEFORE_HEIGHT, before_buffer);
    //write_JPEG_file("/Users/jmccaughey/out.jpeg", 90, AFTER_WIDTH, AFTER_HEIGHT, after_buffer);
    return 0;
}

