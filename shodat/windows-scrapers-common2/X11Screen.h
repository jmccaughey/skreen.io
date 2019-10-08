#ifndef X11SCREEN_H
#define X11SCREEN_H

#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

class X11Screen{
public:
	X11Screen(void);
    void doScreenShot();
    void * getCurrentBuffer();
    void * getPreviousBuffer();
    int getWidth(){ return width; }
    int getHeight(){ return height; }
    bool updateMousePosition(int & x, int & y);
    ~X11Screen();

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

#endif /* X11SCREEN_H */
