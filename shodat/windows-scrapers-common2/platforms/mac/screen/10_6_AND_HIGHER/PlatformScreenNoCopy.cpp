#include "PlatformScreenNoCopy.h"
#include <sys/time.h>
#include <string.h>

PlatformScreen::PlatformScreen(){
	mouseX = -1;
	mouseY = -1;
    width = 0;
    height = 0;
    scaleFactor = -1.0;
    images[0] = NULL;
    images[1] = NULL;
    images_scaled[0] = NULL;
    images_scaled[1] = NULL;
    scaler = NULL;
    if (floor(kCFCoreFoundationVersionNumber) < kCFCoreFoundationVersionNumber10_7) {
        printf("OSX version 10.6 or less\n");
        OSX_10_6_OR_LESS = true;
    }else{
        printf("OSX version 10.7 or higher\n");
        OSX_10_6_OR_LESS = false;
    }
}
PlatformScreen::~PlatformScreen(){
    clearImages();
}
unsigned long PlatformScreen::getWidth(){
    if(width < 1){
        reset();
    }
    return scaler == NULL ? width : width / 2;
}
unsigned long PlatformScreen::getHeight(){
    if(height < 1){
        reset();
    }
    return scaler == NULL ? height : height / 2;
}
CGImageRef PlatformScreen::takeMacScreenshot(){
    // TODO: Hide skreen.io UI using  kCGWindowSharingNone ?
    // https://developer.apple.com/reference/coregraphics/1454852-cgwindowlistcreateimage?language=objc

    CGImageRef cgImageRef;
    if(OSX_10_6_OR_LESS){
        // NOTE: CGWindowListCreateImage will capture _all_ displays, like one big virtual desktop.
        // TODO: figure out which is main display and grab only that
        cgImageRef = CGWindowListCreateImage(CGRectNull, kCGWindowListOptionOnScreenOnly,
                            0, kCGWindowImageBoundsIgnoreFraming);
    }else{
        // NOTE: CGDisplayCreateImage crashes on 10.6
        cgImageRef = CGDisplayCreateImage(kCGDirectMainDisplay);
    }
    if(!cgImageRef){
        printf("got null image\n");
    }
    return cgImageRef;
}
void PlatformScreen::reset(){
    printf("starting reset\n");
    clearImages();
    printf("returned from clearImages\n");
    // capture a screen shot to discover the resolution
    // because in "Retina" case, CGDisplayPixelsWide reports
    // 1/2 actual pixels reported by CGImageGetWidth
    // TODO: Scaler also needed for case of very large screen
    printf("about to takeMacScreenshot\n");
    CGImageRef screenShot = takeMacScreenshot();
    printf("returned from CGDisplayCreateImage\n");
	unsigned long imageWidth = CGImageGetWidth(screenShot);
    printf("returned from CGImageGetWidth\n");
    unsigned long imageHeight = CGImageGetHeight(screenShot);
    printf("returned from CGImageGetHeight\n");
    bytesPerRow = CGImageGetBytesPerRow(screenShot);
    printf("returned from CGImageGetBytesPerRow\n");
    CGImageRelease(screenShot);
    printf("returned from CGImageRelease\n");
    width = CGDisplayPixelsWide(0);
	height = CGDisplayPixelsHigh(0);
	scaleFactor = imageWidth / width;
    if(scaleFactor != 1){
        scaler = new Scaler(imageWidth, imageHeight, false);
        images_scaled[0] = (unsigned char*)malloc(scaler->getScaledBufferSize());
        images_scaled[1] = (unsigned char*)malloc(scaler->getScaledBufferSize());
    }else{
        scaler = NULL;
    }
    width = imageWidth;
    height = imageHeight;
    cursor = 0;
    printf("leaving reset\n");
}
void PlatformScreen::clearImages(){
	if(images[0]){
        CFRelease(images[0]);
        images[0] = NULL;
    }
    if(images[1]){
        CFRelease(images[1]);
        images[1] = NULL;
    }
    if(images_scaled[0]){
        free(images_scaled[0]);
        images_scaled[0] = NULL;
    }
    if(images_scaled[1]){
        free(images_scaled[1]);
        images_scaled[1] = NULL;
    }
    if(scaler){
        delete scaler;
        scaler = NULL;
    }
}

void * PlatformScreen::getCurrentBuffer(){
    if(images[cursor] != NULL && scaler == NULL){
        return (void*)CFDataGetBytePtr(images[cursor]);
    }else if(scaler != NULL){
        return (void*)images_scaled[cursor];
    }
    printf("returning NULL for currentBuffer!!\n");
    return NULL;
}
void * PlatformScreen::getPreviousBuffer(){
    if(images[cursor == 1 ? 0 : 1] != NULL && scaler == NULL){
        return (void*)CFDataGetBytePtr(images[cursor == 1 ? 0 : 1]);
    }else if(scaler != NULL){
        return (void*)images_scaled[cursor == 1 ? 0 : 1];
    }
    printf("returning NULL for previousBuffer!!\n");
    return NULL;
}
int PlatformScreen::doScreenShot(){
    if (scaleFactor < 0 || height == 0 || width == 0) {
        printf("about to call reset\n");
        reset();
        printf("returned from reset\n");
    }
    printf("about to call takeMacScreenshot\n");
    CGImageRef screenShot = takeMacScreenshot();
	unsigned long imageWidth = CGImageGetWidth(screenShot);
    unsigned long imageHeight = CGImageGetHeight(screenShot);
    bytesPerRow = CGImageGetBytesPerRow(screenShot);
    if(width != imageWidth || height != imageHeight){
        // resolution has changed
        printf("resolution changed\n");
        //reset();
        return 1;
    }
    if(cursor == 1){
        printf("setting cursor to 0\n");
        cursor = 0;
    }else{
        printf("setting cursor to 1\n");
        cursor = 1;
    }
    
    if(images[cursor] != NULL){
        printf("CFReleasing %lu\n", cursor);
        CFRelease(images[cursor]);
        images[cursor] = NULL;
    }
    printf("about to CGImageGetDataProvider\n");
    CGDataProviderRef cgDataProviderRef = CGImageGetDataProvider(screenShot);
    if(cgDataProviderRef == NULL){
        printf("got null from CGImageGetDataProvider\n");
    }
    printf("about to CGDataProviderCopyData\n");
    images[cursor] = CGDataProviderCopyData(cgDataProviderRef);
    printf("returned from CGDataProviderCopyData\n");
    //size_t sourceLength = CFDataGetLength(images[cursor]);
    //printf("source length: %zu\n", sourceLength);
    //printf("expected size: %lu\n", width*height*4);
    if(scaler){
        scaler->scale((uint8*)CFDataGetBytePtr(images[cursor]), images_scaled[cursor]);
    }
    CGImageRelease(screenShot);
    //printf("returned form releasing image\n");
    return 0;
}
int PlatformScreen::updateMousePosition(int* x, int* y){
	CGEventRef event = CGEventCreate(nil);
	CGPoint loc = CGEventGetLocation(event);
	*x = loc.x * (scaler == NULL ? scaleFactor : 1);
	*y = loc.y * (scaler == NULL ? scaleFactor : 1);
	CFRelease(event);
	if(*x != mouseX || *y != mouseY){
		mouseX = *x;
		mouseY = *y;
		return 1;
	}
	return 0;
}
