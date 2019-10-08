#pragma once
//#ifndef SCREEN_H
//#define SCREEN_H

#include "OpenGL/OpenGL.h"
#include <GLUT/glut.h>
#include "ApplicationServices/ApplicationServices.h"

class PlatformScreen{
public:
	PlatformScreen(void);
    void doScreenShot();
    void * getCurrentBuffer();
    void * getPreviousBuffer();
    int getWidth(){ return width; }
    int getHeight(){ return height; }
    int updateMousePosition(int * x, int * y);
    ~PlatformScreen();

private:
    void *images[2];
	void * bottomUpPixels;
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

//#endif /* SCREEN_H */

