#include <gtk/gtk.h>
#include <jpeglib.h>

/*
  compile with something like:
  gcc sshot2.c -o sshot2 `pkg-config --cflags --libs gtk+-2.0` -ljpeg
*/


int main(int argc, char *argv[]) {
    GdkWindow *window_root;
    GdkPixbuf *screenshot;
    gtk_init(&argc, &argv);

    GdkScreen *screen = gdk_screen_get_default();
    GdkRectangle rect;

    gdk_screen_get_monitor_geometry(screen, 0, &rect);
    window_root = gdk_get_default_root_window();

    screenshot = gdk_pixbuf_get_from_drawable(NULL, window_root, NULL, 0, 0, 0,
            0, rect.width, rect.height);

    FILE* outfile = fopen("test.jpeg", "wb");
    if (!outfile) {
        exit(1);
    }

    char* buffer = gdk_pixbuf_get_pixels(screenshot);
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, outfile);

    cinfo.image_width = rect.width;
    cinfo.image_height = rect.height;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults(&cinfo);
    /*set the quality [0..100]  */
    jpeg_set_quality(&cinfo, 85, 1);
    jpeg_start_compress(&cinfo, 1);

    JSAMPROW row_pointer; /* pointer to a single row */

    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer = (JSAMPROW) & buffer[cinfo.next_scanline * 3 * rect.width];
        jpeg_write_scanlines(&cinfo, &row_pointer, 1);
    }
    jpeg_finish_compress(&cinfo);
    return 0;
}
