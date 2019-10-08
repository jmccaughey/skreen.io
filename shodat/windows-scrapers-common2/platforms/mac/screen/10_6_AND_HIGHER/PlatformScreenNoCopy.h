#pragma once

extern "C" {
#include "libavutil/mathematics.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
}
#include "SwscaleTest/Scaler.h"
#include "ApplicationServices/ApplicationServices.h"

class PlatformScreen{
public:
	PlatformScreen(void);
    int doScreenShot(); // 1 means screen size changed
    void * getCurrentBuffer();
    void * getPreviousBuffer();
    unsigned long getWidth();
    unsigned long getHeight();
    int updateMousePosition(int * x, int * y);
    ~PlatformScreen();

private:
    CFDataRef images[2];
    unsigned char* images_scaled[2];
    Scaler* scaler;
    size_t bytesPerRow;
    size_t cursor;
    void reset();
    unsigned long width;
    unsigned long height;
    int mouseX;
    int mouseY;
    float scaleFactor; // normally 1 but 2 for "Retina" displays
    void clearImages();
    bool OSX_10_6_OR_LESS;
    CGImageRef takeMacScreenshot();
    
};
