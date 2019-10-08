#pragma once

#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

class PlatformScreen{
public:
	PlatformScreen(void);
    void doScreenShot();
    void * getCurrentBuffer();
    void * getPreviousBuffer();
    int getWidth(){ return width; }
    int getHeight(){ return height; }
    bool updateMousePosition(int & x, int & y);
    ~PlatformScreen();

private:
    XImage *imgs[2];
    int cursor;
    int init();
    Display *display;
    Window root;
    int width;
    int height;
    int mouseX;
    int mouseY;
};
