#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <jpeglib.h>

//  gcc grab_monitor_zero.c -ljpeg -lX11
//  gcc grab_monitor_zero.c -I/home/john/libjpeg-turbo/ -I/home/john/libjpeg-turbo/build/ \
//          -L/home/john/libjpeg-turbo/build/.libs/ -ljpeg -lX11 -lXrandr

int writeJpeg(char * data, int width, int height, char* path);

int main(int argc, char** argv) {
    if(argc < 2){
        printf("usage: %s <dir>\n", argv[0]);
        return 1;
    }
    mkdir(argv[1], 0777);
    char path[512];
    Display *display;
    int screen;
    Window root;
    display = XOpenDisplay(0);
    screen = DefaultScreen(display);
    root = RootWindow(display, screen);
    
    XRRScreenResources *resources;
    resources = XRRGetScreenResourcesCurrent (display, root);
    printf("%d monitors found\n", resources->noutput);
    
    int monitorIndex = 0;
    XRROutputInfo *output = 0;
    for(monitorIndex = 0; monitorIndex < resources->noutput; monitorIndex++){
        output = XRRGetOutputInfo (display, resources, resources->outputs[monitorIndex]);
        if(output->crtc > 0){
            break;
        }
        XRRFreeOutputInfo (output);
    }

    XRRCrtcInfo *crtc = XRRGetCrtcInfo (display, resources, output->crtc);
    
    int width = crtc->width;
    int height = crtc->height;
    
    printf("monitor %d is %d x %d\n", monitorIndex, width, height);
    XRRFreeCrtcInfo (crtc);
    XRRFreeOutputInfo (output);
    
    XImage *imgs[2];
    imgs[0] = 0;
    imgs[1] = 0;

    for(int i = 0; i < 20; i++){
        int index = i % 2;
        int other = index == 0 ? 1 : 0;
        //printf("index: %d other: %d\n", index, other);
        bool changed = false;
        if(imgs[index] == 0){
            imgs[index] = XGetImage(display, root, 0, 0, width, height, XAllPlanes(),
                    ZPixmap);
        }else{
            XGetSubImage(display, root, 0, 0, width, height, XAllPlanes(),
                        ZPixmap, imgs[index], 0, 0);
        }
        if(imgs[other] == 0){
            changed = true;
        }else{
            //changed = memcmp(imgs[index]->data, imgs[other]->data, imgs[index]->width * imgs[index]->height * 4);
        }
        if(changed){
            sprintf(path, "%s/sshot_%d.jpg", argv[1], i);
            //printf("writing changed screen\n");
            //int result = writeJpeg(imgs[index]->data, imgs[index]->width, imgs[index]->height, path);
        }
        //getchar();
    }
    if (imgs[0] != NULL) {
         XDestroyImage(imgs[0]);
    }
    if (imgs[1] != NULL) {
         XDestroyImage(imgs[1]);
    }
    XRRFreeScreenResources(resources);
    XCloseDisplay(display);
    return 0;
}

int writeJpeg(char * data, int width, int height, char* path){
    FILE* outfile = fopen(path, "wb");

     if (!outfile) {
         return 1;
     }
     struct jpeg_compress_struct cinfo;
     struct jpeg_error_mgr jerr;

     cinfo.err = jpeg_std_error(&jerr);
     jpeg_create_compress(&cinfo);
     jpeg_stdio_dest(&cinfo, outfile);

     cinfo.image_width = width;
     cinfo.image_height = height;
     cinfo.input_components = 4;
     cinfo.in_color_space = JCS_EXT_BGRA;

     jpeg_set_defaults(&cinfo);
     /*set the quality [0..100]  */
     jpeg_set_quality(&cinfo, 88, 1);
     jpeg_start_compress(&cinfo, 1);

     JSAMPROW row_pointer; /* pointer to a single row */
     char * buffer = data;
     while (cinfo.next_scanline < cinfo.image_height) {
         row_pointer = (JSAMPROW) & buffer[cinfo.next_scanline * (4)
                 * width];
         jpeg_write_scanlines(&cinfo, &row_pointer, 1);
     }
     jpeg_finish_compress(&cinfo);
     jpeg_destroy_compress(&cinfo);
     fclose(outfile);
}
