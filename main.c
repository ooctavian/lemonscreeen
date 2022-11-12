#include <gtk/gtk.h>
#include <stdlib.h>
GtkWidget *window;
GtkWidget *text;
GHashTable *table;
int ac;
char **av;
char *css = "* { \
all: unset; \
}";
int button_count = 0;

static void
run_command (GtkWidget *widget, gpointer data)
{
  char *p = (char *) data;
  if (p != NULL)
 system((char *) data);
  gtk_widget_destroy (window);
}

static gboolean
keypress_function (GtkWidget *widget, GdkEventKey *event, gpointer data)
{
  char *key = event->string;
  char *command = (char *) g_hash_table_lookup (table, key);
  if (g_strcmp0 (key, "Escape"))
    gtk_widget_destroy (window);
  else if (command != NULL)
    {
      system((char *) command);
      gtk_widget_destroy (window);
    }
  return TRUE;
}

static GtkWidget *
create_button (const char *arg)
{
  char **args = g_strsplit (arg, "|", 5);
  g_hash_table_insert (table, args[0], args[1]);
  GtkWidget *image = gtk_image_new_from_file (args[4]);
  GtkWidget *button = gtk_button_new ();
  gtk_button_set_image (GTK_BUTTON (button), image);
  gchar *label = g_strdup_printf ("b%d", button_count);
  button_count++;
  gtk_widget_set_name (GTK_WIDGET (button), label);
  g_signal_connect (button, "clicked", G_CALLBACK (run_command), args[1]);
  css = g_strdup_printf ("%s\
button#%s:hover {\
	background: %s;\
			}\
button#%s {\
	background: %s;\
			}\
",
                         css, label, args[3], label, args[2]);
  return button;
}

static void
activate (GtkApplication *app, gpointer user_data)
{
  GtkWidget *button_box;
  GtkWidget *layout;
  GtkWidget *hbox, *vbox;
  char *font = "Sans 50";
  char *background = "#1D1F21";
  char *label_background = "#373B41";
  char *buttons_background = "#282A2E";
  char *font_color = "#c5c8c6";
  char *message = "";
  char *button_radius = "0";

  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Exitscreen");
  gtk_widget_set_opacity (GTK_WIDGET (window), 0.0);
  gtk_window_fullscreen (GTK_WINDOW (window));

  layout = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  button_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 20);
  hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 20);
  vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);

  // Parsing command line arguments
  for (int i = 0; i < ac; i++)
    if (av[i][0] == '-')
      switch (av[i][1])
        {
        case 'a':
          gtk_container_add (GTK_CONTAINER (button_box),
                             create_button ((char *) av[++i]));
          break;
        case 'B':
          background = av[++i];
          break;
        case 'b':
          buttons_background = av[++i];
          break;
        case 'T':
          label_background = av[++i];
          break;
        case 'f':
          font = av[++i];
          break;
        case 'F':
          font_color = av[++i];
          break;
        case 't':
          message = av[++i];
          break;
        case 'r':
          button_radius = av[++i];
          break;
        case 's':
          gtk_box_set_spacing (GTK_BOX (button_box), atoi (av[++i]));
          break;
        case 'h':
          g_print ("%s\n", "Usage:");
          g_print ("%s\n", "Flags");
          g_print ("%s\n",
                   " -a key|command|normal_bg|hover_bg|icon_path - add a button");
          g_print ("%s\n", " -B #hex/path - set background color/image");
          g_print ("%s\n", " -b #hex - set buttons' background color");
          g_print ("%s\n", " -T #hex - set text background color");
          g_print ("%s\n", " -F #hex - set text foreground color");
          g_print ("%s\n", " -f Font name size - set text font");
          g_print ("%s\n", " -t string - set text shown above buttons");
          g_print ("%s\n", " -s number - set spacing between buttons");
          g_print ("%s\n", " -r number - set button radius");
          exit (0);
          break;
        default:
          g_print ("Unknown option %c \n", av[i][1]);
          break;
        }
  css = g_strdup_printf ("%s\
			label { \
				font: %s;\
                color: %s;\
				background-color: %s;\
			}\
	box#buttons {\
				background: %s;\
			}\
    button {\
           border-radius: %s;\
}\
",

                         css, font, font_color,
                         label_background, buttons_background, button_radius);
/*   if (background[0] == '/') */
/*     { */
/*       GdkRectangle workarea = { 0 }; */
/*       gdk_monitor_get_workarea ( */
/*           gdk_display_get_primary_monitor (gdk_display_get_default ()), */
/*           &workarea); */
/*       css = g_strdup_printf ("%s\ */
/* window {	  \ */
/* background-image: url(\"%s\");\ */
/* background-repeat: no-repeat; \ */
/* background-position: center; \ */
/* margin:0px;\ */
/* padding:0px;\ */
/* background-size: %upx %upx; \ */
/*        		     } ", */
/*                              css, background, workarea.width, workarea.height); */
/*     } */
/*   else */
/*     { */
      css = g_strdup_printf ("%s\
window {	  \
background-color: %s;\
    }",
                             css, background);
    /* } */
  text = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (text), message);
  gtk_widget_set_name (GTK_WIDGET (text), "text");
  gtk_widget_set_name (GTK_WIDGET (layout), "layout");
  gtk_widget_set_name (GTK_WIDGET (button_box), "buttons");
  gtk_box_set_center_widget (GTK_BOX (hbox), layout);
  gtk_box_set_center_widget (GTK_BOX (vbox), hbox);
  gtk_container_add (GTK_CONTAINER (window), vbox);

  // Apply css using a cssProvider
  GtkCssProvider *cssProvider = gtk_css_provider_new ();
  gtk_css_provider_load_from_data (cssProvider, css, -1, NULL);
  gtk_style_context_add_provider_for_screen (gdk_screen_get_default (),
                                             GTK_STYLE_PROVIDER (cssProvider),
                                             GTK_STYLE_PROVIDER_PRIORITY_USER);
  gtk_container_add (GTK_CONTAINER (layout), text);
  gtk_container_add (GTK_CONTAINER (layout), button_box);
  gtk_widget_add_events (window, GDK_KEY_PRESS_MASK);
  g_signal_connect (G_OBJECT (window), "key_press_event", G_CALLBACK (keypress_function), NULL);
  gtk_widget_show_all (window);
}

int
main (int argc, char **argv)
{
  ac = argc;
  av = argv;
  table = g_hash_table_new (g_str_hash, g_str_equal);
  GtkApplication *app;
  int status;
  app = gtk_application_new ("fuck.this.shit", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), 0, 0);
  g_object_unref (app);
  return status;
}
