/*
 *  Screen.cpp
 *  CoreScraper
 *
 *  Created by John McCaughey on 8/24/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#include "Screen.h"

Screen::Screen(){
	mouseX = -1;
	mouseY = -1;
	CGLPixelFormatAttribute attributes[] = {
		kCGLPFAFullScreen,   
		kCGLPFADisplayMask, 
		(CGLPixelFormatAttribute)CGDisplayIDToOpenGLDisplayMask(0),
		(CGLPixelFormatAttribute) 0
	};    
	CGLPixelFormatObj pix;
	CGLError errorCode;
	long int num; // stores the number of possible pixel formats
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
	width = CGDisplayPixelsWide(0);
	height = CGDisplayPixelsHigh(0);
	// TODO: subscribe to screen config change events
	int bufferSize = width*height*4;
	images[0] = malloc(bufferSize);
	images[1] = malloc(bufferSize);
	//bottomUpPixels = malloc(bufferSize);
	
	// [mGLContext setFullScreen];
	errorCode = CGLSetFullScreen(context);
	if(errorCode != 0){
		CFShow(CFSTR("error CGLSetFullScreen\n"));
	}
}

Screen::~Screen(){
	CGLSetCurrentContext( NULL );
	CGLDestroyContext( context );
}

void * Screen::getCurrentBuffer(){
	return images[cursor];
}
void * Screen::getPreviousBuffer(){
	return images[cursor == 1 ? 0 : 1];
}

void Screen::doScreenShot(){
	if(cursor == 1){
		cursor = 0;
	}else{
		cursor = 1;
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
				 //images[cursor]
				 bottomUpPixels
				 );
				 
	glPopClientAttrib();	
    //Check for OpenGL errors
    GLenum theError = GL_NO_ERROR;
    theError = glGetError();
	if(GL_NO_ERROR == theError){
		CFShow(CFSTR("no error\n"));
	}else{
		CFShow(CFSTR("scrape error\n")); 
		printf("error: %d\n", theError);
	}	
	reflectPixels();
}

void Screen::reflectPixels(){
	for(int i = 0; i < height; i+++){
		memcpy(images[cursor] + ((height-i) - 1), bottomUpPixels + (i*width*4), width * 4);
	}
}

int Screen::updateMousePosition(int* x, int* y){
	CGEventRef event = CGEventCreate(nil);
	CGPoint loc = CGEventGetLocation(event);
	*x = loc.x;
	*y = loc.y;
	CFRelease(event);
	if(*x != mouseX || *y != mouseY){
		mouseX = *x;
		mouseY = *y;
		return 1;
	}
	return 0;
}