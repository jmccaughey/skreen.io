#include <stdio.h>

#include "X11Screen.h"

X11Screen::X11Screen(void) {
    imgs[0] = 0;
    imgs[1] = 0;
    display = 0;
    width = 0;
    height = 0;
    mouseX = 0;
    mouseY = 0;
    cursor = 1;
    init();
}

X11Screen::~X11Screen() {
    if (imgs[0] != NULL) {
        XDestroyImage( imgs[0]);
    }
    if (imgs[1] != NULL) {
        XDestroyImage( imgs[1]);
    }
    XCloseDisplay( display);
}

void X11Screen::doScreenShot() {
	printf("X11Screen::doScreenShot\n");
    if (display == 0) {
    	printf("X11Screen::doScreenShot(calling init())\n");
        init();
    }
    if (cursor == 1) {
        cursor = 0;
    } else {
        cursor = 1;
    }
    if (imgs[cursor] == NULL) {
    	printf("X11Screen::doScreenShot(xgetimage)\n");
        imgs[cursor] = XGetImage(display, root, 0, 0, width, height,
                XAllPlanes(), ZPixmap);
    } else {
    	printf("X11Screen::doScreenShot(xgetsubimage)\n");
        XGetSubImage(display, root, 0, 0, width, height, XAllPlanes(), ZPixmap,
                imgs[cursor], 0, 0);
    }
    printf("leaving X11Screen::doScreenShot\n");
}
bool X11Screen::updateMousePosition(int & x, int & y){
	Window window_returned;
	int root_x, root_y, win_x, win_y;
	unsigned int mask_return;
	bool result = XQueryPointer(display, root, &window_returned,
                &window_returned, &root_x, &root_y, &win_x, &win_y,
                &mask_return);
    if(result){
    	if(root_x != mouseX || root_y != mouseY){
    		printf("mouse position changed\n");
    		mouseX = root_x; mouseY = root_y;
    		x = mouseX; y = mouseY;
    		return true;
    	}
    }else{
    	printf("problem getting mouse position\n");
    }
    return false;
}
void * X11Screen::getCurrentBuffer() {
    return imgs[cursor]->data;
}
void * X11Screen::getPreviousBuffer() {
    return imgs[cursor == 1 ? 0 : 1]->data;
}

int X11Screen::init() {
    printf("X11Screen::init\n");
    display = XOpenDisplay(0);
    int screen = DefaultScreen(display);
    root = RootWindow(display, screen);
    XRRScreenResources *resources = XRRGetScreenResourcesCurrent(display, root);
    printf("%d monitors found\n", resources->noutput);

    int monitorIndex = 0;
    XRROutputInfo *output = 0;
    for (monitorIndex = 0; monitorIndex < resources->noutput; monitorIndex++) {
        output = XRRGetOutputInfo(display, resources,
                resources->outputs[monitorIndex]);
        if (output->crtc > 0) {
            break;
        }
        XRRFreeOutputInfo(output);
        output = 0;
    }

    XRRCrtcInfo *crtc = XRRGetCrtcInfo(display, resources, output->crtc);

    width = crtc->width;
    height = crtc->height;

    printf("monitor %d is %d x %d\n", monitorIndex, width, height);
    XRRFreeCrtcInfo(crtc);
    if (output != 0) {
        XRRFreeOutputInfo(output);
    }
    XRRFreeScreenResources(resources);
    resources = NULL;
    return 0;
}
