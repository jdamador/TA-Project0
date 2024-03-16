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
GtkWidget *dfa_table;
GtkWidget *grid_layout;
GtkWidget *scrolled_layout;

// Buttons objects to handle the disable/enable event.
GtkWidget *evaluate;
GtkWidget *evaluate_strip;

// Automaton variable settings;
gint n_states;
gint m_alphabet;
gchar alphabet[1024];

// DFA table arrays;
GtkWidget ***transitions;
GtkWidget **state_names;
GtkWidget **alphabet_element;
GtkWidget **acceptance_checkboxes;

// Simple definition of methods, this allow the program to call the methods in wherever part of the program without having problems of hierarchy.
void end_clicked_event(GtkButton *b);
void settings_clicked_event(GtkButton *b);
void evaluate_clicked_event(GtkButton *b);
void eval_strip_clicked_event(GtkButton *b);
void activation_handler(int option);
void generate_dfa_settings_table();
void alphabet_symbol_changed_event(GtkEntry *entry, gpointer typed_data);
void custom_name_changed_event(GtkEntry *entry, gpointer typed_data);
void transition_changed_event(GtkEntry *entry, gpointer typed_data);
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
    dfa_table = GTK_WIDGET(gtk_builder_get_object(ui_builder, "dfa_table"));
    scrolled_layout = GTK_WIDGET(gtk_builder_get_object(ui_builder, "scrolled_layout"));

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
    // TODO: create a clear function to free memory.

    // TODO: set here the code to create the table base on N states and M alphabet members.

    // Get n_states and m_alphabet_elements from spin buttons.
    n_states = (int)gtk_spin_button_get_value(GTK_SPIN_BUTTON(gtk_builder_get_object(ui_builder, "sp_n_states")));
    m_alphabet = (int)gtk_spin_button_get_value(GTK_SPIN_BUTTON(gtk_builder_get_object(ui_builder, "sp_m_alphabet")));

    // call generate_dfa_settings_table to create the table.
    generate_dfa_settings_table();
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
    // TODO: set here the code to create the DFA.
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
    // TODO: set here the code to evaluate a new strip.
}

/*
 * Generate DFA Settings Table: Generate the table with allow to configure the DFA.
 *
 * Parameters:
 *   void
 *
 * Output:
 *   void.
 */
void generate_dfa_settings_table()
{
    // Activate the evaluate button.
    activation_handler(2);

    // Clear the table and its container.
    gtk_container_remove(GTK_CONTAINER(dfa_table), grid_layout);
    // Allocate memory for the arrays to create the table.
    transitions = (GtkWidget ***)malloc(n_states * sizeof(GtkWidget **));
    state_names = (GtkWidget **)malloc(n_states * sizeof(GtkWidget *));
    alphabet_element = (GtkWidget **)malloc(m_alphabet * sizeof(GtkWidget *));
    acceptance_checkboxes = (GtkWidget **)malloc(n_states * sizeof(GtkWidget *));

    // For each state, we need to create m_alphabet columns. So we allocate memory for that.
    for (int i = 0; i < n_states; i++)
    {
        transitions[i] = (GtkWidget **)malloc(m_alphabet * sizeof(GtkWidget *));
    }

    // Create a new grid layout to set all new elements in the UI.
    grid_layout = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid_layout), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid_layout), 5);

    char current_symbol = 'a'; // This letter is to named the alphabet elements. We start with a-z and then A-Z.

    // For each m_alphabet element we create a new entry.
    for (int i = 0; i < m_alphabet; i++)
    {
        // Create all entries for alphabet symbols.
        GtkWidget *new_alphabet_entry = gtk_entry_new();
        char alphabet_symbol[2] = {current_symbol, '\0'}; // new symbol.
        gtk_entry_set_text(GTK_ENTRY(new_alphabet_entry), alphabet_symbol);
        gtk_entry_set_max_length(GTK_ENTRY(new_alphabet_entry), 1);                                       // Max length = 1.
        g_signal_connect(new_alphabet_entry, "changed", G_CALLBACK(alphabet_symbol_changed_event), NULL); // When entry text changed generate an event.
        gtk_grid_attach(GTK_GRID(grid_layout), new_alphabet_entry, i + 3, 0, 1, 1);
        alphabet_element[i] = new_alphabet_entry;
        current_symbol++;
        if (current_symbol > 'z')
        {
            current_symbol = 'A';
        }
    }

    // For each n_state we need to generate a acceptance box, new state name entry, name label and its transitions entries.
    for (int i = 1; i < n_states + 1; i++)
    {
        // Add acceptance checkboxes.
        GtkWidget *acceptance_checkbox = gtk_check_button_new_with_label("Acceptance");
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(acceptance_checkbox), FALSE); // Uncheck by default.
        gtk_grid_attach(GTK_GRID(grid_layout), acceptance_checkbox, 0, i, 1, 1);
        acceptance_checkboxes[i - 1] = acceptance_checkbox;

        // Add a label with the state name, it starts with 1 - n_states.
        char current_state_name[20]; // Buffer should be maximum for 20 elements.
        sprintf(current_state_name, "%d", i);
        GtkWidget *new_label = gtk_label_new(current_state_name);
        gtk_grid_attach(GTK_GRID(grid_layout), new_label, 1, i, 1, 1);

        // Add new entry to type a custom state name.
        GtkWidget *custom_name = gtk_entry_new();
        gtk_entry_set_placeholder_text(GTK_ENTRY(custom_name), "custom name");
        g_signal_connect(custom_name, "changed", G_CALLBACK(custom_name_changed_event), NULL);
        gtk_grid_attach(GTK_GRID(grid_layout), custom_name, 2, i, 1, 1);
        state_names[i - 1] = custom_name;

        // if i = 1, we know is the first element in the transition table, so we have to mark the elements with a signal.
        if (i == 1)
        {
            GdkRGBA color;
            gdk_rgba_parse(&color, "light green");
            gtk_widget_override_background_color(acceptance_checkbox, GTK_STATE_NORMAL, &color);
            gtk_widget_override_background_color(custom_name, GTK_STATE_NORMAL, &color);
            gtk_widget_override_background_color(new_label, GTK_STATE_NORMAL, &color);
        }

        // Now we generate a new entry for each alphabet element.
        for (int j = 0; j < m_alphabet; j++)
        {
            GtkWidget *new_transition = gtk_entry_new();
            if (i == 1)
            {
                GdkRGBA color;
                gdk_rgba_parse(&color, "light green");
                gtk_widget_override_background_color(acceptance_checkbox, GTK_STATE_NORMAL, &color);
                gtk_widget_override_background_color(new_transition, GTK_STATE_NORMAL, &color);
            }
            gtk_entry_set_text(GTK_ENTRY(new_transition), "-");
            g_signal_connect(new_transition, "changed", G_CALLBACK(transition_changed_event), NULL);
            gtk_grid_attach(GTK_GRID(grid_layout), new_transition, j + 3, i, 1, 1);
            transitions[i - 1][j] = new_transition;
        }
    }

    // Add the grid_layout to the screen.
    gtk_container_add(GTK_CONTAINER(dfa_table), grid_layout);
    gtk_container_add(GTK_CONTAINER(scrolled_layout), dfa_table);
    gtk_widget_show_all(window);
}

/*
 * Custom Name Changed Event: this method is added to every custom_name entry, so when it change, sent a event.
 *
 * Parameters:
 *   entry: the entry who generated the event.
 *   typed_data: the data typed in the entry.
 * Output:
 *   void.
 */
void custom_name_changed_event(GtkEntry *entry, gpointer typed_data)
{
    // TODO: validations on state names entries should be here.
}

/*
 * Alphabet Symbol Changed Event: this method is added to every alphabet_symbol entry, so when it change, sent a event.
 *
 * Parameters:
 *   entry: the entry who generated the event.
 *   typed_data: the data typed in the entry.
 * Output:
 *   void.
 */
void alphabet_symbol_changed_event(GtkEntry *entry, gpointer typed_data)
{
    // TODO: validations on alphabet symbols entries should be here.
}

/*
 * Transition Changed Event: this method is added to every transition entry, so when it change, sent a event.
 *
 * Parameters:
 *   entry: the entry who generated the event.
 *   typed_data: the data typed in the entry.
 * Output:
 *   void.
 */
void transition_changed_event(GtkEntry *entry, gpointer typed_data)
{
    // TODO: validations on in transitions states entries should be here.
}
