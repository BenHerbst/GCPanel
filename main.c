#include <adwaita.h>
#include <stdlib.h>

// define the applications to search for
// first is application name, second is icon name
char supported_apps[40][20];

void on_application_button_pressed(GtkWidget *widget, int supported_app) {
  char *location[50];
  strcpy(location, "/usr/share/applications/");
  strcat(location, supported_apps[supported_app]);
  strcat(location, ".desktop");

  // check if desktop file exists
  if (access(location, F_OK) == 0) {
    char command[60];
    strcpy(command, "gtk-launch ");
    strcat(command, supported_apps[supported_app]);
    system(command);
  }
}

static void activate_cb ( AdwApplication *app ) {
  AdwHeaderBar *header_bar = adw_header_bar_new();
 
  GtkBox *gtk_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_append(gtk_box, header_bar);

  AdwApplicationWindow *window = adw_application_window_new(app);
  gtk_window_set_title (GTK_WINDOW (window), "GCPanel");
  gtk_window_set_default_size (GTK_WINDOW (window), 700, 500);
  gtk_window_present (GTK_WINDOW (window));
  adw_application_window_set_content(GTK_WINDOW (window), gtk_box);

  GtkFlowBox *flow_box = gtk_flow_box_new();
  gtk_flow_box_set_column_spacing(flow_box, 2);

  // define margins of flow box
  gtk_widget_set_margin_bottom(flow_box, 4);
  gtk_widget_set_margin_top(flow_box, 4);
  gtk_widget_set_margin_start(flow_box, 6);
  gtk_widget_set_margin_end(flow_box, 6);

  for (int supported_app = 0; supported_app < sizeof(supported_apps) / sizeof(*supported_apps); supported_app++) {
    char *location[50];
    strcpy(location, "/usr/share/applications/");
    strcat(location, supported_apps[supported_app]);
    strcat(location, ".desktop");

    if (access(location, F_OK) ==  0) {
      // get icon name from desktop file
      FILE *desktop_file = fopen(location, "r");

      char *icon_name;
      
      char current_char = fgetc(desktop_file);
      int line_count = 0;
      int string_count = 0;

      char desktop_file_array[255][50];

      while (current_char != EOF) {
        if (current_char == '\n') {
          // new line
          string_count = 0;
          line_count++;
        } else {
          // no new line
          desktop_file_array[line_count][string_count] = current_char;
          string_count++;
        }
        current_char = fgetc(desktop_file);
      }
      
      for (int line = 0; line < 255; line++) {
        desktop_file_array[line];
        if (strncmp(desktop_file_array[line], "Icon=", strlen("Icon=")) == 0) {
          // line starts with Icon=
          icon_name = desktop_file_array[line];
          icon_name += 5;
        }
      }

      g_print("%s\n\n", icon_name);

      fclose(desktop_file);

      GtkButton *application_button = gtk_button_new();
      GtkImage *application_icon = gtk_image_new_from_icon_name(icon_name);

      gtk_image_set_icon_size(application_icon, GTK_ICON_SIZE_LARGE);
      gtk_button_set_child(application_button, application_icon);
      gtk_flow_box_append(flow_box, application_button);
      gtk_widget_set_size_request(application_button, 50, 110);

      g_signal_connect(application_button, "clicked", G_CALLBACK(on_application_button_pressed), supported_app);
    }
  }

  gtk_box_append(gtk_box, flow_box);
}

int main ( int argc, char *argv[] ) {
  g_autoptr (AdwApplication) app = NULL;
  
  FILE *supported_apps_file = fopen("supported_apps.txt", "r");
  if (supported_apps_file == NULL) {
    printf("Cannot open file.\n");
    return 1;
  } else {
    // can read file
    char current_char;
    int line_count = 0;
    int string_count = 0;

    current_char = fgetc(supported_apps_file);
    while (current_char != EOF) {
      if ( current_char == '\n') {
        line_count++;
        string_count = 0;
      } else {
        supported_apps[line_count][string_count] = current_char;
        string_count++;
      }

      current_char = fgetc(supported_apps_file);
    }
  }

  fclose(supported_apps_file);

  app = adw_application_new ("org.benherbst", G_APPLICATION_FLAGS_NONE);

  g_signal_connect (app, "activate", G_CALLBACK (activate_cb), NULL);

  return g_application_run (G_APPLICATION (app), argc, argv);
}