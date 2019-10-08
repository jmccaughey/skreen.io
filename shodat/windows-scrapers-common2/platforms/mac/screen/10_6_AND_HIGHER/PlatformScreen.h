#pragma once

#include "ApplicationServices/ApplicationServices.h"

class PlatformScreen{
public:
	PlatformScreen(void);
    void doScreenShot();
    void * getCurrentBuffer();
    void * getPreviousBuffer();
    int getWidth();
    int getHeight();
    int updateMousePosition(int * x, int * y);
    ~PlatformScreen();

private:
    void *images[2];
    int bytesPerRow;
    int cursor;
    void reset();
    int width;
    int height;
    int mouseX;
    int mouseY;
    float scaleFactor; // normally 1 but 2 for "Retina" displays
    
};
