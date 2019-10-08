#include <X11/Xlib.h>
#include <stdio.h>
#include <jpeglib.h>

//  gcc grab.c -ljpeg -lX11

int main() {
    Display *display;
    int screen;
    Window root;
    display = XOpenDisplay(0);
    screen = DefaultScreen(display);
    root = RootWindow(display, screen);
    XImage *img = XGetImage(display, root, 0, 0, 1200, 200, XAllPlanes(),
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
        fclose(outfile);
        XDestroyImage(img);
    }
    XCloseDisplay(display);
    return 0;
}
