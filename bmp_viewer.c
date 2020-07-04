#define _GNU_SOURCE
#include<stdio.h>
#include<gmodule.h>
#include<gtk/gtk.h>
#include"bmp_reader.h"

/* The following struct is used to store a filename and a window instance */
typedef struct window_app_t {
    GtkWidget* window;
    GtkApplication* app;
} window_app_t;

gboolean draw_callback (GtkWidget *widget, cairo_t *cr, gpointer data) {
  char *filename = data;
  bmp_file_t bmp_file = parse_bmp_file(filename);
  guint width, height;

  width = gtk_widget_get_allocated_width(widget);
  height = gtk_widget_get_allocated_height(widget);
  
  int rgb_id = 0;
  
  uint8_t red, green, blue;
  for(int i=0; i<bmp_file.num_pixels * 3; i++){
      
      if(rgb_id == 0){
          blue = bmp_file.pixel_data[i];
          rgb_id++;
      }
      else if(rgb_id == 1){
          green = bmp_file.pixel_data[i];
          rgb_id++;
      }
      else if(rgb_id == 2){
          red = bmp_file.pixel_data[i];
          rgb_id = 0;
          GdkRGBA my_colour =  {.red = (float)red / 255,
                                .green = (float)green / 255,
                                .blue = (float)blue / 255,
                                .alpha=1.0};
                                
          gdk_cairo_set_source_rgba(cr, &my_colour);   
          
          float i_adjusted = (float) i / 3.0;
          uint32_t x = (uint32_t) i_adjusted % bmp_file.width;
          uint32_t y = i_adjusted / bmp_file.width;
        
          cairo_rectangle(cr, x, bmp_file.height - y, 1, 1);
          cairo_fill(cr);
      }

  }

  printf("%d %d %d\n", width, height, bmp_file.width);

  return FALSE;
}

static void create_new_wave_window(GtkApplication* app, char* filename) {
  GtkWidget *window_new;
  GtkWidget *vbox;

  GtkWidget *instruction_label;
  GtkWidget *drawing_area;

  window_new = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW (window_new), filename);
  gtk_window_set_default_size(GTK_WINDOW (window_new), 300, 100);
  gtk_container_set_border_width(GTK_CONTAINER(window_new), 5);

  vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_container_add(GTK_CONTAINER(window_new), vbox);
  
  bmp_file_t bmp_file = parse_bmp_file(filename);
  if(bmp_file.error){
    printf("Error: Parsing .wav file failed\n");
    return;
  }
  
  drawing_area = gtk_drawing_area_new();
  gtk_widget_set_size_request(drawing_area, bmp_file.width, bmp_file.height);
  g_signal_connect(G_OBJECT(drawing_area), "draw", G_CALLBACK(draw_callback), filename);
  
  gtk_box_pack_start(GTK_BOX(vbox), drawing_area, TRUE, TRUE, 0);



  char *sample_label;
  asprintf(&sample_label,
           " Viewing file: %s \n Image width: %d \n Image height: %d \n Bit depth: %d bits \n",
           filename,
           bmp_file.width,
           bmp_file.height,
           bmp_file.bits_per_pixel);

  instruction_label = gtk_label_new(sample_label);
  gtk_box_pack_start(GTK_BOX(vbox), instruction_label, TRUE, TRUE, 0);

  gtk_widget_show_all(window_new);
}

static void open_file_dialog(GtkWidget* button, gpointer user_data){
  window_app_t *wa = user_data;
  GtkWindow *window = GTK_WINDOW(wa->window);
  GtkApplication *app = wa->app;
  GtkWidget *dialog;
  GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
  gint res;
  dialog = gtk_file_chooser_dialog_new("Open File",
                                      window,
                                      action,
                                      "_Cancel_",
                                      GTK_RESPONSE_CANCEL,
                                      "_Open_",
                                      GTK_RESPONSE_ACCEPT,
                                      NULL);

  res = gtk_dialog_run(GTK_DIALOG(dialog));
  char* filename;
  if (res == GTK_RESPONSE_ACCEPT) {
      GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
      filename = gtk_file_chooser_get_filename(chooser);
      create_new_wave_window(app, filename);
  }

  gtk_widget_destroy(dialog);
}


static void activate (GtkApplication* app, gpointer user_data) {
  GtkWidget *window;
  GtkWidget *vbox;

  GtkWidget *button;
  GtkWidget *instruction_label;
  GtkWidget *button_box;

  window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW (window), "BMPsee");
  gtk_window_set_default_size(GTK_WINDOW (window), 200, 200);
  gtk_container_set_border_width(GTK_CONTAINER(window), 5);

  vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
  gtk_container_add(GTK_CONTAINER(window), vbox);

  window_app_t *wa = g_slice_new(window_app_t); /*creates an efficient memory slice*/
  wa->window = window;
  wa->app = app;

  instruction_label = gtk_label_new(
                                    "Directions: \n\
Chose a valid .bmp file and a new window showing the image will be shown.\n\
Any number of .bmp files may be opened simultaneously.\n\n\
If there is a parse error, no new window will be created.");
  gtk_box_pack_start(GTK_BOX(vbox), instruction_label, TRUE, TRUE, 0);

  button_box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_container_add(GTK_CONTAINER(vbox), button_box);

  button = gtk_button_new_with_label("Choose file");
  g_signal_connect(button, "clicked", G_CALLBACK(open_file_dialog), wa);
  gtk_container_add(GTK_CONTAINER(button_box), button);

  gtk_widget_show_all(window);
}

int main(int argc, char **argv){
  GtkApplication *app;

  app = gtk_application_new("org.sina.bmpsee", G_APPLICATION_FLAGS_NONE);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  int status = g_application_run(G_APPLICATION(app), argc, argv);// Wait for destroy of all windows
  printf("%d\n", status);

  g_object_unref(app);
}
