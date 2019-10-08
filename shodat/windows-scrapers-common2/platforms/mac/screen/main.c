#include <CoreFoundation/CoreFoundation.h>
#include <OpenGL/CGLTypes.h>
#include <GLUT/glut.h>
#include "/opt/libjpeg-turbo/include/jpeglib.h"

int mainOld (int argc, const char * argv[]) {
    CGLContextObj context;
  
  
  CGLPixelFormatAttribute attributes[] = {
    kCGLPFAFullScreen,   
    kCGLPFADisplayMask, 
    CGDisplayIDToOpenGLDisplayMask(0),
    (CGLPixelFormatAttribute) 0
  };
    
  CGLPixelFormatObj pix;
  CGLError errorCode;
  int num; // stores the number of possible pixel formats
  errorCode = CGLChoosePixelFormat( attributes, &pix, &num );
  if(errorCode != 0){
    CFShow(CFSTR("error\n"));
  }
  // add error checking here
  errorCode = CGLCreateContext( pix, NULL, &context ); // second parameter can be another context for object sharing
  // add error checking here
  if(errorCode != 0){
    CFShow(CFSTR("error\n"));
  }
  CGLDestroyPixelFormat( pix );
 
  errorCode = CGLSetCurrentContext( context );
  if(errorCode != 0){
    CFShow(CFSTR("error\n"));
  }
  int width = 1280;
  int height = 1024;
  int bufferSize = width*height*4;
  void * bytes = malloc(bufferSize);
  
  // [mGLContext setFullScreen];
  errorCode = CGLSetFullScreen(context);
  if(errorCode != 0){
    CFShow(CFSTR("error\n"));
  }
  
  glReadBuffer(GL_FRONT);
    
    //Read OpenGL context pixels directly.

    // For extra safety, save & restore OpenGL states that are changed
    glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);
    
    glPixelStorei(GL_PACK_ALIGNMENT, 4); /* Force 4-byte alignment */
    glPixelStorei(GL_PACK_ROW_LENGTH, 0);
    glPixelStorei(GL_PACK_SKIP_ROWS, 0);
    glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
    
    //Read a block of pixels from the frame buffer
    glReadPixels(0, 0, width, height, GL_BGRA, 
    /*
    IMPORTANT
    
    For the pixel data format and type parameters you should *always* specify:
    
        format: GL_BGRA
        type: GL_UNSIGNED_INT_8_8_8_8_REV
    
    because this is the native format of the GPU for both PPC and Intel and will 
    give you the best performance. Any deviation from this format will not give 
    you optimal performance!
    
    BACKGROUND
    
    When using GL_UNSIGNED_INT_8_8_8_8_REV, the OpenGL implementation 
    expects to find data in byte order ARGB on big-endian systems, but BGRA on 
    little-endian systems. Because there is no explicit way in OpenGL to specify 
    a byte order of ARGB with 32-bit or 16-bit packed pixels (which are common
    image formats on Macintosh PowerPC computers), many applications specify 
    GL_BGRA with GL_UNSIGNED_INT_8_8_8_8_REV. This practice works on a 
    big-endian system such as PowerPC, but the format is interpreted differently 
    on a little-endian system, and causes images to be rendered with incorrect colors.

    To prevent images from being rendered incorrectly by this application on little 
    endian systems, you must specify the ordering of the data (big/little endian)
    when creating Quartz bitmap contexts using the CGBitmapContextCreate function. 
    See the createRGBImageFromBufferData: method in the Buffer.m source file for
    the details.

    Also, if you need to reverse endianness, consider using vImage after the read. See: 

    http://developer.apple.com/documentation/Performance/Conceptual/vImage/
    
    */
            GL_UNSIGNED_INT_8_8_8_8_REV,
            bytes);



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
  cinfo.image_width = width; 	/* image width and height, in pixels */
  cinfo.image_height = height;
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
  //jpeg_set_quality(&cinfo, 70, TRUE /* limit to baseline-JPEG values */);

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
  row_stride = width * 4;	/* JSAMPLEs per row in image_buffer */

  while (cinfo.next_scanline < cinfo.image_height) {
    /* jpeg_write_scanlines expects an array of pointers to scanlines.
     * Here the array is only one element long, but you could pass
     * more than one scanline at a time if that's more convenient.
     */
    row_pointer[0] = & bytes[(cinfo.image_height - (1+cinfo.next_scanline)) * row_stride];
    (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }

  /* Step 6: Finish compression */

  jpeg_finish_compress(&cinfo);
  /* After finish_compress, we can close the output file. */
  fclose(outfile);

  /* Step 7: release JPEG compression object */

  /* This is an important step since it will release a good deal of memory. */
  jpeg_destroy_compress(&cinfo);

    glPopClientAttrib();

    //Check for OpenGL errors
    GLenum theError = GL_NO_ERROR;
    theError = glGetError();
	if(GL_NO_ERROR == theError){
	  CFShow(CFSTR("no error\n"));
	}else{
	  CFShow(CFSTR("error\n")); 
	}
   
  
  CGLSetCurrentContext( NULL );
  CGLDestroyContext( context );
  
  
    //CFShow(CFSTR("Hello, World!\n"));
    return 0;
}
