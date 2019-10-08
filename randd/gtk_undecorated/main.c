#include <gtk/gtk.h>


static gboolean on_button_press (GtkWidget* widget,
  GdkEventButton * event, GdkWindowEdge edge){
  printf("on_button_press\n");
  if (event->type == GDK_BUTTON_PRESS) {
    if (event->button == 1) {
      gtk_window_begin_move_drag(GTK_WINDOW(gtk_widget_get_toplevel(widget)),
          event->button,
      event->x_root,
      event->y_root,
      event->time);
    }
  }
  return FALSE;
}

//static void destroyWindow( GtkWidget *widget,
//                     gpointer   data ){
//    printf("destroyWindow handler\n");
//    gtk_widget_destroy(widget);
//}

static void handleStopButton(GtkWidget *widget,
        gpointer   data ){
    printf("handleStopButton\n");
    gtk_widget_destroy(widget);
}

static gboolean delete_event( GtkWidget *widget,
                              GdkEvent  *event,
                              gpointer   data ){
    printf("delete_event handler\n");
    // TODO: this does not get called when the browser closes
    // or navigates away from the plugin host page
    return false;
}

static void destroy( GtkWidget *widget,
                     gpointer   data ){
    printf("destroy handler\n");
    gtk_main_quit ();
}

int main(int argc, char** argv){

    GtkWidget *window;
    GtkWidget *button;
    GtkWidget *label;
    GtkWidget *frame;
    GtkWidget *hbox;
      gtk_init (&argc, &argv);
      hbox = gtk_hbox_new(FALSE, 2);
      window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
      button = gtk_button_new_with_label ("stop");
      label = gtk_label_new ("showing");
      frame = gtk_frame_new(NULL);
      gtk_frame_set_shadow_type((GtkFrame*)frame, GTK_SHADOW_OUT);
      gtk_container_add (GTK_CONTAINER (hbox), label);

      gtk_container_add(GTK_CONTAINER(hbox), button);

      gtk_widget_show (label);
      gtk_container_add (GTK_CONTAINER (frame), hbox);
      gtk_container_add (GTK_CONTAINER (window), frame);
      gtk_widget_show (button);
      //gtk_button_set_relief((GtkButton*)button, GTK_RELIEF_HALF);
      gtk_widget_show (hbox);
      gtk_widget_show (frame);
      gtk_window_set_decorated((GtkWindow*)window, 0);
//      gtk_window_set_has_frame((GtkWindow*)window, 1);
      gtk_window_set_title((GtkWindow*)window, "shodat");
      gtk_window_resize((GtkWindow*)window, 100, 30);
      gtk_window_set_modal((GtkWindow*)window, 1);
//      g_signal_connect(window, "expose_event",
//                    G_CALLBACK (exposeEventHandler), (gpointer)pluginInstance);
      gtk_widget_show(window);
      gtk_window_resize((GtkWindow*)window, 100, 30);
      //gtk_window_set_position((GtkWindow*)window, GTK_WIN_POS_CENTER);
      gtk_window_set_resizable((GtkWindow*)window, 0);
      gtk_container_set_border_width (GTK_CONTAINER (window), 1);
      gtk_window_move((GtkWindow*)window, 500, 0);
      gtk_window_set_keep_above ((GtkWindow*)window, 1);

      g_signal_connect_swapped(button, "clicked",
                    G_CALLBACK (gtk_widget_destroy), window);
      g_signal_connect (window, "delete-event",
                    G_CALLBACK (delete_event), NULL);
      g_signal_connect (window, "destroy",
                    G_CALLBACK (destroy), NULL);
      gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);

        g_signal_connect(G_OBJECT(window), "button-press-event",
            G_CALLBACK(on_button_press), NULL);

      printf("about to call gtk_main()\n");
      gtk_main();
      printf("...returned from gtk_main()\n");
}
