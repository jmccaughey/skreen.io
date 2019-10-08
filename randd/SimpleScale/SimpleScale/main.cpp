//
//  main.cpp
//  SimpleScale
//
//  Created by John McCaughey on 10/9/13.
//  Copyright (c) 2013 John McCaughey. All rights reserved.
//

#include <iostream>
#include "ApplicationServices/ApplicationServices.h"
#include "/opt/libjpeg-turbo/include/jpeglib.h"


#pragma mark Basic Profiling Tools
// Set to 1 to enable basic profiling. Profiling information is logged to console.
#ifndef PROFILE_WINDOW_GRAB
#define PROFILE_WINDOW_GRAB 1
#endif

#if PROFILE_WINDOW_GRAB
#define StopwatchStart() AbsoluteTime start = UpTime()
#define Profile(img) CFRelease(CGDataProviderCopyData(CGImageGetDataProvider(img)))
#define StopwatchEnd(caption) do { Duration time = AbsoluteDeltaToDuration(UpTime(), start); double timef = time < 0 ? time / -1000.0 : time / 1000.0; printf("%s Time Taken: %f milliseconds\n", caption, timef); } while(0)
#define StopwatchEnd2(caption, start) do { Duration time = AbsoluteDeltaToDuration(UpTime(), start); double timef = time < 0 ? time / -1000.0 : time / 1000.0; printf("%s Time Taken: %f milliseconds\n", caption, timef); } while(0)
#else
#define StopwatchStart()
#define Profile(img)
#define StopwatchEnd(caption)
#endif


int main(int argc, const char * argv[])
{

    CGImageRef screenShot = CGDisplayCreateImage(kCGDirectMainDisplay);
	unsigned long imageWidth = CGImageGetWidth(screenShot);
    unsigned long imageHeight = CGImageGetHeight(screenShot);
    size_t bytesPerRow = CGImageGetBytesPerRow(screenShot);
    AbsoluteTime start = UpTime();//StopwatchStart();
    CFDataRef data = CGDataProviderCopyData(CGImageGetDataProvider(screenShot));
    StopwatchEnd2("copy data", start);
    size_t sourceLength = CFDataGetLength(data);
    printf("source length: %zu\n", sourceLength);
//    printf("expected size: %d\n", width*height*4);
    const unsigned char * x = CFDataGetBytePtr(data);
    
    unsigned char * scaled = (unsigned char*)malloc(imageHeight * imageWidth);
    //StopwatchStart();
    start = UpTime();
    int si = 0;
    for(int r = 0; r < imageHeight; r += 2){ // rows
        //printf("starting row\n");
        for(int p = 0; p < imageWidth; p += 2){
            // average color components from neighboring 4 pixels
            unsigned long c = (r * bytesPerRow) + (p * 4);
            
            scaled[si++] = (x[c] + x[c+4] + x[c+bytesPerRow] + x[c+bytesPerRow+4]) / 4;
            scaled[si++] = (x[c+1] + x[c+5] + x[c+bytesPerRow+1] + x[c+bytesPerRow+5]) / 4;
            scaled[si++] = (x[c+2] + x[c+6] + x[c+bytesPerRow+2] + x[c+bytesPerRow+6]) / 4;
            scaled[si++] = (x[c+3] + x[c+7] + x[c+bytesPerRow+3] + x[c+bytesPerRow+7]) / 4;
            
            //int redTotal = pData[(r * bytesPerRow) + ]
            /*
            if(r < 100){
                printf("row: %d cursor: %d blue: %d green: %d red: %d other: %d\n",
                   r, c, x[c], x[c+1], x[c+2], x[c+3]);
                printf("row: %d cursor: %d nlue: %d nreen: %d ned: %d other: %d\n",
                       r, c, nb, ng, nr, na);
            }
             */
        }
        
    }
    StopwatchEnd2("scaling image", start);
    // write jpeg file
    
    start = UpTime();
    //return 0;
    
    
    struct jpeg_compress_struct cinfo;
    /* This struct represents a JPEG error handler.  It is declared separately
     * because applications often want to supply a specialized error handler
     * (see the second half of this file for an example).  But here we just
     * take the easy way out and use the standard error handler, which will
     * print a message on stderr and call exit() if compression fails.
     * Note that this struct must live as long as the main JPEG parameter
     * struct, to avoid dangling-pointer problems.
     */
    struct jpeg_error_mgr jerr;
    /* More stuff */
    FILE * outfile;		/* target file */
    JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */
    int row_stride;		/* physical row width in image buffer */
    
    /* Step 1: allocate and initialize JPEG compression object */
    
    /* We have to set up the error handler first, in case the initialization
     * step fails.  (Unlikely, but it could happen if you are out of memory.)
     * This routine fills in the contents of struct jerr, and returns jerr's
     * address which we place into the link field in cinfo.
     */
    cinfo.err = jpeg_std_error(&jerr);
    /* Now we can initialize the JPEG compression object. */
    jpeg_create_compress(&cinfo);
    
    /* Step 2: specify data destination (eg, a file) */
    /* Note: steps 2 and 3 can be done in either order. */
    
    /* Here we use the library-supplied code to send compressed data to a
     * stdio stream.  You can also write your own code to do something else.
     * VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
     * requires it in order to write binary files.
     */
    if ((outfile = fopen("test_scrape.jpg", "wb")) == NULL) {
        fprintf(stderr, "can't open %s\n", "test_scrape.jpg");
        exit(1);
    }
    jpeg_stdio_dest(&cinfo, outfile);
    
    /* Step 3: set parameters for compression */
    
    /* First we supply a description of the input image.
     * Four fields of the cinfo struct must be filled in:
     */
    cinfo.image_width = imageWidth / 2; 	/* image width and height, in pixels */
    cinfo.image_height = imageHeight /2;
    cinfo.input_components = 4;		/* # of color components per pixel */
    cinfo.in_color_space = JCS_EXT_BGRX; 	/* colorspace of input image */
    /* Now use the library's routine to set default compression parameters.
     * (You must set at least cinfo.in_color_space before calling this,
     * since the defaults depend on the source color space.)
     */
    jpeg_set_defaults(&cinfo);
    /* Now you can set any non-default parameters you wish to.
     * Here we just illustrate the use of quality (quantization table) scaling:
     */
    jpeg_set_quality(&cinfo, 60, TRUE /* limit to baseline-JPEG values */);
    
    /* Step 4: Start compressor */
    
    /* TRUE ensures that we will write a complete interchange-JPEG file.
     * Pass TRUE unless you are very sure of what you're doing.
     */
    jpeg_start_compress(&cinfo, TRUE);
    
    /* Step 5: while (scan lines remain to be written) */
    /*           jpeg_write_scanlines(...); */
    
    /* Here we use the library's state variable cinfo.next_scanline as the
     * loop counter, so that we don't have to keep track ourselves.
     * To keep things simple, we pass one scanline per call; you can pass
     * more if you wish, though.
     */
    row_stride = imageWidth * 2;	/* JSAMPLEs per row in image_buffer */
    
    while (cinfo.next_scanline < cinfo.image_height) {
        /* jpeg_write_scanlines expects an array of pointers to scanlines.
         * Here the array is only one element long, but you could pass
         * more than one scanline at a time if that's more convenient.
         */
        row_pointer[0] = &scaled[cinfo.next_scanline * row_stride];
        (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }
    
    /* Step 6: Finish compression */
    
    jpeg_finish_compress(&cinfo);
    StopwatchEnd2("jpeg compress", start);
    /* After finish_compress, we can close the output file. */
    fclose(outfile);
    
    
    
    return 0;
}

