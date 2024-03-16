/*
 * dfa_controller.h
 * Description: this file handle everything related with user interface process.
 * Author: Daniel A, Sebastian G, Josef R, Gerardo G.
 * Date: 2024
 */

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

// UI global objects to create visual elements.
GtkBuilder *ui_builder;
GtkWidget *window;

// Buttons objects to handle the disable/enable event.
GtkWidget *evaluate;    
GtkWidget *evaluate_strip;





// Simple definition of methods, this allow the program to call the methods in wherever part of the program without having problems of hierarchy.
void end_clicked_event(GtkButton *b);
void settings_clicked_event(GtkButton *b);
void evaluate_clicked_event(GtkButton *b);
void eval_strip_clicked_event(GtkButton *b);
void activation_handler(int option);
/*
 * Display_UI: handler to start all the screen elements to be displayed.
 *
 * Parameters:
 *   argc, argv: the arguments set by terminal input when the program is executed.
 *
 * Output:
 *   a state that show if the program run without any errors.
 */
int display_ui(int argc, char *argv[])
{
    // Define the start elements of the screen.
    gtk_init(&argc, &argv);
    ui_builder = gtk_builder_new();

    // Build with the glade file the new interface.
    gtk_builder_add_from_file(ui_builder, "Src/ui_new_design.glade", NULL);

    // Create the new visual screen and set the buttons events (close, minimize, expand)
    window = GTK_WIDGET(gtk_builder_get_object(ui_builder, "window"));
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create decorators.
    gtk_window_set_title(GTK_WINDOW(window), "Automata and Formal Language Theory");
    gtk_builder_connect_signals(ui_builder, NULL);

    // Get visual elements to manipulate by code.
    evaluate = GTK_WIDGET(gtk_builder_get_object(ui_builder, "btn_evaluate"));
    evaluate_strip = GTK_WIDGET(gtk_builder_get_object(ui_builder, "btn_evaluate_strip"));
    
    // Deactivate the buttons as initial state.
    activation_handler(1);
    // Display the new screen.
    gtk_widget_show_all(window);
    gtk_main();
    return EXIT_SUCCESS;
}

/*
 * Activation Handler: this method is in charge of manage which elements will be enable or disable.
 *
 * Parameters:
 *   option: which situation is currently happening.
 *
 * Output:
 *   void.
 */
void activation_handler(int option)
{
    switch (option)
    {
    case 1:
        // Start option: evaluate button and evaluate_strip are disable.
        gtk_widget_set_sensitive(evaluate, 0);
        gtk_widget_set_sensitive(evaluate_strip, 0);
        break;
    case 2:
        // after settings are made: evaluate button is enable, but evaluate_strip keeps disable.
        gtk_widget_set_sensitive(evaluate, 1);
        gtk_widget_set_sensitive(evaluate_strip, 0);
        break;
    case 3:
        // after evaluate is pressed: DFA is ready so, evaluate button is disable again, and evaluate_strip is enable.
        gtk_widget_set_sensitive(evaluate, 0);
        gtk_widget_set_sensitive(evaluate_strip, 1);
        break;
    }
}

/*
 * End Clicked Event: handler the end event, just finish the program.
 *
 * Parameters:
 *   GtkButton: the event generated when the button is clicked.
 *
 * Output:
 *   void.
 */
void end_clicked_event(GtkButton *b)
{
    exit(1);
}

/*
 * Settings Clicked Event: handler of events when the button to SET the settings is clicked.
 *
 * Parameters:
 *   GtkButton: the event generated when the button is clicked.
 *
 * Output:
 *   void.
 */
void settings_clicked_event(GtkButton *b)
{
    // TODO: set here the code to create the table base on N states and M alphabet members.
}

/*
 * Evaluate Clicked Event: save the DFA settings and set as ready the DFA.
 *
 * Parameters:
 *   GtkButton: the event generated when the button is clicked.
 *
 * Output:
 *   void.
 */
void evaluate_clicked_event(GtkButton *b)
{
}

/*
 * Eval Strip Clicked Event: handler of events when the button to eval a strip is clicked.
 *
 * Parameters:
 *   GtkButton: the event generated when the button is clicked.
 *
 * Output:
 *   void.
 */
void eval_strip_clicked_event(GtkButton *b)
{
}
