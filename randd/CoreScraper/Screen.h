#ifndef SCREEN_H
#define SCREEN_H

/*
 *  Screen.h
 *  CoreScraper
 *
 *  Created by John McCaughey on 8/24/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#include "OpenGL/OpenGL.h"
#include <GLUT/glut.h>
#include "ApplicationServices/ApplicationServices.h"

class Screen{
public:
	Screen(void);
    void doScreenShot();
    void * getCurrentBuffer();
    void * getPreviousBuffer();
    int getWidth(){ return width; }
    int getHeight(){ return height; }
    int updateMousePosition(int * x, int * y);
    ~Screen();

private:
	void *bottomUpPixels;
    void *images[2];
	CGLContextObj context;
    int cursor;
    int init();
	void reflectPixels();
    //Display *display;
    //Window root;
    int width;
    int height;
    int mouseX;
    int mouseY;
};

#endif /* SCREEN_H */

