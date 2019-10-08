#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#include <stdio.h>
#include <jpeglib.h>

//  gcc grab_monitor_zero.c -ljpeg -lX11
//  gcc grab_monitor_zero.c -I/home/john/libjpeg-turbo/ -I/home/john/libjpeg-turbo/build/ \
//          -L/home/john/libjpeg-turbo/build/.libs/ -ljpeg -lX11 -lXrandr

int main() {

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
    
    // TODO: XGetSubImage might be more efficient
    XImage *img = XGetImage(display, root, 0, 0, width, height, XAllPlanes(),
            ZPixmap);

    if (img != NULL) {
        printf(
                "XImage width: %d height: %d bytes_per_line: %d bits_per_pixel: %d\n",
                img->width, img->height, img->bytes_per_line,
                img->bits_per_pixel);

        FILE* outfile = fopen("test.jpeg", "wb");

        if (!outfile) {
            exit(1);
        }
        struct jpeg_compress_struct cinfo;
        struct jpeg_error_mgr jerr;

        cinfo.err = jpeg_std_error(&jerr);
        jpeg_create_compress(&cinfo);
        jpeg_stdio_dest(&cinfo, outfile);

        cinfo.image_width = img->width;
        cinfo.image_height = img->height;
        cinfo.input_components = 4;
        cinfo.in_color_space = JCS_EXT_BGRA;

        jpeg_set_defaults(&cinfo);
        /*set the quality [0..100]  */
        jpeg_set_quality(&cinfo, 88, 1);
        jpeg_start_compress(&cinfo, 1);

        JSAMPROW row_pointer; /* pointer to a single row */
        char * buffer = img->data;
        while (cinfo.next_scanline < cinfo.image_height) {
            row_pointer = (JSAMPROW) & buffer[cinfo.next_scanline * (4)
                    * img->width];
            jpeg_write_scanlines(&cinfo, &row_pointer, 1);
        }
        jpeg_finish_compress(&cinfo);
        jpeg_destroy_compress(&cinfo);
        fclose(outfile);
        XDestroyImage(img);
    }
    XRRFreeScreenResources(resources);
    XCloseDisplay(display);
    return 0;
}
