/*
 * dfa_core.h
 * Description: this file handle everything related with user interface process.
 * Author: Daniel A, Sebastian G, Josef R, Gerardo G.
 * Date: 2024
 */

#include "dfa_interface.h"
#include "ui_interface.h"
#include <ctype.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>


GtkBuilder *ui_builder;
GtkWidget *window;


int display_ui(int argc, char *argv[]) {

    gtk_init(&argc, &argv);
    ui_builder = gtk_builder_new();
    gtk_builder_add_from_file(ui_builder, "src/ui_design.glade", NULL);
    window = GTK_WIDGET(gtk_builder_get_object(ui_builder, "window"));

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_window_set_title(GTK_WINDOW(window),"Automata and Formal Language Theory");
    gtk_builder_connect_signals(ui_builder, NULL);
    
    /*
    
        Load UI element here.

    */
    gtk_widget_show_all(window);
    gtk_main();
    return 1;
}
