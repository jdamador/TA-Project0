/*
 * dfa_controller.h
 * Description: this file handle everything related with user interface process.
 * Author: Daniel A, Sebastian G, Josef R, Gerardo G.
 * Date: 2024
 */
#include "dfa_interface.h"
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
GtkWidget *visual_transition_table;
GtkWidget *grid_layout;
GtkWidget *scrolled_layout;

// Buttons objects to handle the disable/enable event.
GtkWidget *evaluate_button;
GtkWidget *evaluate_strip_button;
GtkWidget *evaluate_strip;
GtkWidget *print_button;
// Automaton variable settings;
gint n_states;
gint m_alphabet;
int ready_to_print = 0;

// gchar alphabet[1024];

// DFA visual table arrays;
GtkWidget ***transition_entries;
GtkWidget **state_names_entries;
GtkWidget **alphabet_symbol_entries;
GtkWidget **acceptance_checkboxes;

// DFA data arrays -> here is storage the DFA after evaluate button is pressed.
int **transition_table;
int *acceptance_states;
char *alphabet_symbols;
char **state_names;

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
void clear_memory();
void execute_strip_evaluation(const char *strip2eval, char *alphabet_symbols, int **transition_table, int *acceptance_states, char **state_names);
void fix_ui_transition_table(int ***fixed_table, int **original_table, int n_states, int m_alphabet);
void transition_focus_out(GtkEntry *entry, gpointer typed_data);
gboolean isNumeric(const gchar *text);
void btn_print_clicked_cb(GtkButton *b);
void print_base_packages(FILE *file);
void print_title_section(FILE *file);
void print_main_opening_section(FILE *file);
void print_main_closing_section(FILE *file);
void print_dfa_definition(FILE *file, char **states, int states_num, char *alphabet, int alphabet_size, int **trans_states, int *acceptance);
void print_dfa_graph(FILE *file, char **states, int states_num, char *alphabet, int alphabet_size, int **trans_states, int *acceptance);
void print_accepted_examples(FILE *file, char **states, int states_num, char *alphabet, int alphabet_size, int **trans_states, int *acceptance);
void print_rejected_examples(FILE *file, char **states, int states_num, char *alphabet, int alphabet_size, int **trans_states, int *acceptance);
void print_regex_dfa(FILE *file, char **states, int states_num, char *alphabet, int alphabet_size, int **trans_states, int *acceptance);
void iterate_states(FILE *file, char **states, int states_num);
void print_table(FILE *file, int **trans_states, int alphabet_size, int states_num, char **states, char *alphabet);
void iterate_alphabet(FILE *file, char *alphabet, int alphabet_size);
/*
 * display_ui: handler to start all the screen elements to be displayed.
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
    evaluate_button = GTK_WIDGET(gtk_builder_get_object(ui_builder, "btn_evaluate"));
    evaluate_strip_button = GTK_WIDGET(gtk_builder_get_object(ui_builder, "btn_evaluate_strip"));
    visual_transition_table = GTK_WIDGET(gtk_builder_get_object(ui_builder, "dfa_table"));
    scrolled_layout = GTK_WIDGET(gtk_builder_get_object(ui_builder, "scrolled_layout"));
    evaluate_strip = GTK_WIDGET(gtk_builder_get_object(ui_builder, "evaluate_strip"));
    print_button = GTK_WIDGET(gtk_builder_get_object(ui_builder, "btn_print"));
    // Deactivate the buttons as initial state.
    activation_handler(1);

    // Display the new screen.
    gtk_widget_show_all(window);
    gtk_main();
    return EXIT_SUCCESS;
}

/*
 * activation_handler: this method is in charge of manage which elements will be enable or disable.
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
        gtk_widget_set_sensitive(evaluate_button, 0);
        gtk_widget_set_sensitive(evaluate_strip_button, 0);
        gtk_widget_set_sensitive(evaluate_strip, 0);
        gtk_widget_set_sensitive(print_button, 0);
        break;
    case 2:
        // after settings are made: evaluate button is enable, but evaluate_strip keeps disable.
        gtk_widget_set_sensitive(evaluate_button, 1);
        gtk_widget_set_sensitive(evaluate_strip_button, 0);
        gtk_widget_set_sensitive(evaluate_strip, 0);
        gtk_widget_set_sensitive(print_button, 0);
        break;
    case 3:
        // after evaluate is pressed: DFA is ready so, evaluate button is disable again, and evaluate_strip is enable.
        gtk_widget_set_sensitive(evaluate_button, 0);
        gtk_widget_set_sensitive(evaluate_strip_button, 1);
        gtk_widget_set_sensitive(evaluate_strip, 1);
        gtk_widget_set_sensitive(print_button, 1);
        break;
    }
}

/*
 * evaluate_clicked_event: save the DFA settings and set as ready the DFA.
 *
 * Parameters:
 *   GtkButton: the event generated when the button is clicked.
 *
 * Output:
 *   void.
 */
void evaluate_clicked_event(GtkButton *b)
{
    // Free all array elements;
    free(state_names);
    free(alphabet_symbols);
    free(acceptance_states);
    free(transition_table);

    // Create a matrix of size n_states x m_alphabet.
    transition_table = (int **)malloc(n_states * sizeof(int *));
    for (int i = 0; i < n_states; i++)
    {
        transition_table[i] = (int *)malloc(m_alphabet * sizeof(int));
    }

    // Run through all transitions and get the values to create the transition table.
    for (int i = 0; i < n_states; i++)
    {
        for (int j = 0; j < m_alphabet; j++)
        {
            GtkWidget *entry = transition_entries[i][j];
            const gchar *text = gtk_entry_get_text(GTK_ENTRY(entry));
            int value = -1;

            if (strcmp(text, "-") == 0)
            {
                value = -1;
            }
            else
            {
                value = atoi(text);
            }

            transition_table[i][j] = value;
        }
    }

    // Run through the acceptance array to get if checkboxes are checked or not.
    acceptance_states = (int *)malloc(n_states * sizeof(int));
    for (int i = 0; i < n_states; i++)
    {
        GtkWidget *checkbox = acceptance_checkboxes[i];
        gboolean is_active = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbox));
        acceptance_states[i] = is_active ? 1 : 0;
    }

    // Run through the alphabet elements to get the symbols.
    alphabet_symbols = (char *)malloc(m_alphabet * sizeof(char));
    for (int j = 0; j < m_alphabet; j++)
    {
        const char *text = gtk_entry_get_text(GTK_ENTRY(alphabet_symbol_entries[j]));
        char cstr[2];
        cstr[0] = text[0];
        cstr[1] = '\0';
        alphabet_symbols[j] = cstr[0];
    }

    // Run through all state_names to get the state names.
    state_names = (char **)malloc(n_states * sizeof(char *));
    for (int j = 0; j < n_states; j++)
    {
        const gchar *text = gtk_entry_get_text(GTK_ENTRY(state_names_entries[j]));

        state_names[j] = NULL;
        if (strlen(text) > 0)
        {
            state_names[j] = g_strdup(text);
        }
        else
        {
            state_names[j] = g_strdup_printf("%d", j + 1);
        }
    }

    // Manage which elements must be disable.
    activation_handler(3);
}

/*
 * eval_strip_clicked_event: handler of events when the button to eval a strip is clicked.
 *
 * Parameters:
 *   GtkButton: the event generated when the button is clicked.
 *
 * Output:
 *   void.
 */
void eval_strip_clicked_event(GtkButton *b)
{
    const gchar *strip2eval = gtk_entry_get_text(GTK_ENTRY(evaluate_strip));
    execute_strip_evaluation(strip2eval, alphabet_symbols, transition_table, acceptance_states, state_names);
}

/*
 * generate_dfa_settings_table: Generate the table with allow to configure the DFA.
 *
 * Parameters:
 *   void.
 *
 * Output:
 *   void.
 */
void generate_dfa_settings_table()
{
    // Activate the evaluate button.
    activation_handler(2);

    // Clear the table and its container.
    gtk_container_remove(GTK_CONTAINER(visual_transition_table), grid_layout);
    // clear_memory();

    // Allocate memory for the arrays to create the table.
    transition_entries = (GtkWidget ***)malloc(n_states * sizeof(GtkWidget **));
    state_names_entries = (GtkWidget **)malloc(n_states * sizeof(GtkWidget *));
    alphabet_symbol_entries = (GtkWidget **)malloc(m_alphabet * sizeof(GtkWidget *));
    acceptance_checkboxes = (GtkWidget **)malloc(n_states * sizeof(GtkWidget *));

    // For each state, we need to create m_alphabet columns. So we allocate memory for that.
    for (int i = 0; i < n_states; i++)
    {
        transition_entries[i] = (GtkWidget **)malloc(m_alphabet * sizeof(GtkWidget *));
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
        alphabet_symbol_entries[i] = new_alphabet_entry;
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
        g_signal_connect(custom_name, "changed", G_CALLBACK(custom_name_changed_event), GINT_TO_POINTER(10));
        gtk_grid_attach(GTK_GRID(grid_layout), custom_name, 2, i, 1, 1);
        state_names_entries[i - 1] = custom_name;

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
            // g_signal_connect(new_transition, "changed", G_CALLBACK(transition_changed_event), NULL);
            g_signal_connect(new_transition, "focus-out-event", G_CALLBACK(transition_focus_out), GINT_TO_POINTER(10));
            gtk_grid_attach(GTK_GRID(grid_layout), new_transition, j + 3, i, 1, 1);
            transition_entries[i - 1][j] = new_transition;
        }
    }

    // Add the grid_layout to the screen.
    gtk_container_add(GTK_CONTAINER(visual_transition_table), grid_layout);
    gtk_container_add(GTK_CONTAINER(scrolled_layout), visual_transition_table);
    gtk_widget_show_all(window);
}

/*
 * execute_strip_evaluation: handle to start the strip evaluation over the input text.
 *
 * Parameters:
 *  strip2eval: the character strip to be evaluated.
 *  alphabet_symbols: a list with the alphabet symbols.
 *  transition_table:  a matrix n x m with the transtions states.
 *  acceptance_states:  a list with the 1 or 0 if the state is for acceptance.
 *  state_names: a list with the states names, state num if there wanst nothing typed by the user.
 *
 * Output:
 *   void.
 */
void execute_strip_evaluation(const char *strip2eval, char *alphabet_symbols, int **transition_table, int *acceptance_states, char **state_names)
{
    // Fix the positions associated to the visual elements in a logic matrix n_states x m_alphabet.
    int **fixed_transition_table = NULL;
    fix_ui_transition_table(&fixed_transition_table, transition_table, n_states, m_alphabet);

    // Call the method to solve the DFA for this specific strip.
    dfa_execution_history dfa_history = solve_dfa(strip2eval, alphabet_symbols, fixed_transition_table, acceptance_states);

    // Base in the DFA execution history if returns 1 is approved, if not is rejected.
    if (dfa_history.is_accepted == 1)
    {
        GdkRGBA new_color;
        gdk_rgba_parse(&new_color, "green");
        gtk_widget_override_background_color(GTK_WIDGET(gtk_builder_get_object(ui_builder, "final_result")), GTK_STATE_NORMAL, &new_color);
        gtk_label_set_text(GTK_LABEL(GTK_WIDGET(gtk_builder_get_object(ui_builder, "final_result"))), (const gchar *)"Approved");
    }
    else
    {
        GdkRGBA new_color;
        gdk_rgba_parse(&new_color, "red");
        gtk_widget_override_background_color(GTK_WIDGET(gtk_builder_get_object(ui_builder, "final_result")), GTK_STATE_NORMAL, &new_color);
        gtk_label_set_text(GTK_LABEL(GTK_WIDGET(gtk_builder_get_object(ui_builder, "final_result"))), (const gchar *)"Rejected");
    }

    // Print automaton transition history in GTK Text View.
    GtkTextView *textView = GTK_TEXT_VIEW(gtk_builder_get_object(ui_builder, "textview"));
    GtkTextBuffer *textViewBuffer = gtk_text_view_get_buffer(textView);
    // wrap text around textView
    gtk_text_view_set_wrap_mode(textView, GTK_WRAP_WORD_CHAR);
    // clear buffer
    gtk_text_buffer_set_text(textViewBuffer, "", -1);

    // Starting state
    char state_str[100];
    snprintf(state_str, sizeof(state_str), "%s", state_names[0]);
    gtk_text_buffer_insert_at_cursor(textViewBuffer, "Initial state: ", -1);
    gtk_text_buffer_insert_at_cursor(textViewBuffer, state_str, -1);

    if (dfa_history.transition_history != NULL)
    {
        dfa_transitions *current_transition = dfa_history.transition_history;
        char char_to_print = current_transition->symbol;
        while (current_transition != NULL)
        {
            // transition to -1
            if (current_transition->to == -1)
            {
                gtk_text_buffer_insert_at_cursor(textViewBuffer, "\nSymbol ", -1);
                char_to_print = current_transition->symbol;
                snprintf(state_str, sizeof(state_str), "%c", char_to_print);
                gtk_text_buffer_insert_at_cursor(textViewBuffer, state_str, -1);
                gtk_text_buffer_insert_at_cursor(textViewBuffer, "\nThere were no further valid transitions", -1);
                break;
            }
            else
            {
                // print transition
                // symbol
                gtk_text_buffer_insert_at_cursor(textViewBuffer, "\nSymbol \"", -1);
                char_to_print = current_transition->symbol;
                snprintf(state_str, sizeof(state_str), "%c", char_to_print);
                gtk_text_buffer_insert_at_cursor(textViewBuffer, state_str, -1);
                gtk_text_buffer_insert_at_cursor(textViewBuffer, "\" transition from state: ", -1);
                // from
                snprintf(state_str, sizeof(state_str), "%s", state_names[current_transition->from]);
                gtk_text_buffer_insert_at_cursor(textViewBuffer, state_str, -1);
                // to
                snprintf(state_str, sizeof(state_str), "%s", state_names[current_transition->to]);
                gtk_text_buffer_insert_at_cursor(textViewBuffer, " to state: ", -1);
                gtk_text_buffer_insert_at_cursor(textViewBuffer, state_str, -1);
                // go to next transition in history
                current_transition = current_transition->next;
            }
        }
        // no transitions
    }
    else
    {
        gtk_text_buffer_insert_at_cursor(textViewBuffer, "\nThere were no transitions", -1);
    }

    // Free memory to avoid segmentation fault.
    for (size_t i = 0; i < n_states; i++)
    {
        free(fixed_transition_table[i]);
    }
    free(fixed_transition_table);
}

/*
 * fix_ui_transition_table: this method is in charge of fix the positions relative from screen layout to the logic matrix.
 *
 * Parameters:
 *   fixed_table: is a pointer that storage the fixed table.
 *   original_table: the variable that storage the table which need to be fixed.
 *   n_states: num of states for this automaton.
 *   m_alphabet: num of symbols of the alphabet for this automaton.
 * Output:
 *   void
 */
void fix_ui_transition_table(int ***fixed_table, int **original_table, int n_states, int m_alphabet)
{
    *fixed_table = (int **)malloc(sizeof(int *) * n_states);

    for (size_t i = 0; i < n_states; i++)
    {
        (*fixed_table)[i] = malloc(sizeof(int) * m_alphabet);
    }
    for (size_t i = 0; i < n_states; i++)
    {
        for (size_t j = 0; j < m_alphabet; j++)
        {
            if (original_table[i][j] == -1 || original_table[i][j] > n_states)
            {
                (*fixed_table)[i][j] = -1;
            }
            else
            {
                (*fixed_table)[i][j] = original_table[i][j] - 1;
            }
        }
    }
}

/*
 * custom_name_changed_event: this method is added to every custom_name entry, so when it change, sent a event.
 *
 * Parameters:
 *   entry: the entry who generated the event.
 *   typed_data: the data typed in the entry.
 * Output:
 *   void.
 */
void custom_name_changed_event(GtkEntry *entry, gpointer typed_data)
{
    const gchar *input_text = gtk_entry_get_text(GTK_ENTRY(entry));
    if (strlen(input_text) > 20)
    {
        gchar *cutted_input_text = g_strndup(input_text, 20);
        gtk_entry_set_text(entry, cutted_input_text);
        g_free(cutted_input_text);
    }
}

/*
 * alphabet_symbol_changed_event: this method is added to every alphabet_symbol entry, so when it change, sent a event.
 *
 * Parameters:
 *   entry: the entry who generated the event.
 *   typed_data: the data typed in the entry.
 * Output:
 *   void.
 */
void alphabet_symbol_changed_event(GtkEntry *entry, gpointer typed_data)
{
    const gchar *text = gtk_entry_get_text(entry);

    for (int j = 0; j < m_alphabet; j++)
    {
        // Get the text from the GtkEntry widget
        const char *other_text = gtk_entry_get_text(GTK_ENTRY(alphabet_symbol_entries[j]));
        if (text[0] == other_text[0] && GTK_ENTRY(alphabet_symbol_entries[j]) != entry)
        {
            gtk_entry_set_text(entry, "");
        }
    }

    // Check if the entered text is more than one character long
    if (strlen(text) > 1)
    {
        // Remove extra characters, keeping only the first character
        gchar first_char[2];
        first_char[0] = text[0];
        first_char[1] = '\0';

        gtk_entry_set_text(entry, first_char);
    }
}

/*
 * custom_name_changed_event: this method is added to every transition entry, so when it change, sent a event.
 *
 * Parameters:
 *   entry: the entry who generated the event.
 *   typed_data: the data typed in the entry.
 * Output:
 *   void.
 */
void transition_changed_event(GtkEntry *entry, gpointer typed_data)
{

    const gchar *input_text = gtk_entry_get_text(entry);

    if (strlen(input_text) != 1 && input_text[0] != '-')
    {
    }

    // if (strlen(input_text) > 4)
    // {
    //     gtk_entry_set_text(entry, "-");
    // }

    // glong int_value = g_ascii_strtoll(input_text, NULL, 10);
    // if(int_value > n_states ){
    //     gtk_entry_set_text(entry, "-");
    // }

    // if (strlen(input_text) == 0)
    // {
    //     gtk_entry_set_text(entry, "-");
    // }
}
void transition_focus_out(GtkEntry *entry, gpointer typed_data)
{

    const gchar *input_text = gtk_entry_get_text(entry);

    if (strlen(input_text) == 0)
    {
        gtk_entry_set_text(entry, "-");
    }
    else
    {
        if (strlen(input_text) == 1 && input_text[0] != '-' && !isNumeric(input_text))
        {
            gtk_entry_set_text(entry, "-");
        }
        else
        {
            if (!isNumeric(input_text))
            {
                gtk_entry_set_text(entry, "-");
            }
            else
            {
                glong int_value = g_ascii_strtoll(input_text, NULL, 10);
                if (int_value > n_states || int_value < 1)
                {
                    gtk_entry_set_text(entry, "-");
                }
            }
        }
    }
}

/*
 * custom_name_changed_event: check if an input text is numeric.
 *
 * Parameters:
 *   text: text to check if is numeric
 *
 * Output:
 *   gboolean: True or False.
 */
gboolean isNumeric(const gchar *text)
{
    gboolean valid = TRUE;
    for (int i = 0; text[i] != '\0'; i++)
    {
        if (!g_ascii_isdigit(text[i]) && text[i] != '-' && text[i] != '.')
        {
            valid = FALSE;
            break;
        }
    }
    return valid;
}

/*
 * end_clicked_event: handler the end event, just finish the program.
 *
 *  * Parameters:
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
 * settings_clicked_event: handler of events when the button to SET the settings is clicked.
 *
 * Parameters:
 *   GtkButton: the event generated when the button is clicked.
 *
 * Output:
 *   void.
 */
void settings_clicked_event(GtkButton *b)
{
    // Get n_states and m_alphabet_elements from spin buttons.
    n_states = (int)gtk_spin_button_get_value(GTK_SPIN_BUTTON(gtk_builder_get_object(ui_builder, "sp_n_states")));
    m_alphabet = (int)gtk_spin_button_get_value(GTK_SPIN_BUTTON(gtk_builder_get_object(ui_builder, "sp_m_alphabet")));

    // call generate_dfa_settings_table to create the table.
    generate_dfa_settings_table();
    ready_to_print = 1;
}

void btn_print_clicked_cb(GtkButton *b)
{

    char **label_array = (char **)malloc(n_states * sizeof(char *));
    for (int j = 0; j < n_states; j++)
    {
        const gchar *text = gtk_entry_get_text(GTK_ENTRY(state_names_entries[j]));

        label_array[j] = NULL;
        if (strlen(text) > 0)
        {
            label_array[j] = g_strdup(text);
        }
        else
        {
            label_array[j] = g_strdup_printf("%d", j + 1);
        }
    }

    char *symbol_array = (char *)malloc(m_alphabet * sizeof(char));
    for (int j = 0; j < m_alphabet; j++)
    {
        const char *text = gtk_entry_get_text(GTK_ENTRY(alphabet_symbol_entries[j]));
        char cstr[2];
        cstr[0] = text[0];
        cstr[1] = '\0';
        symbol_array[j] = cstr[0];
    }

    // Create a 2D array of integers to store the values
    int **trans_states = (int **)malloc(n_states * sizeof(int *));
    for (int i = 0; i < n_states; i++)
    {
        trans_states[i] = (int *)malloc(m_alphabet * sizeof(int));
    }

    // Loop through the GtkWidget ***entries and extract integer values
    for (int i = 0; i < n_states; i++)
    {
        for (int j = 0; j < m_alphabet; j++)
        {

            GtkWidget *entry = transition_entries[i][j];
            const gchar *text = gtk_entry_get_text(GTK_ENTRY(entry));
            int value = -1;

            if (strcmp(text, "-") == 0)
            {
                value = -1;
            }
            else
            {
                value = atoi(text);
            }

            // Store the value in the trans_states
            trans_states[i][j] = value;
        }
    }

    int *acceptance = (int *)malloc(n_states * sizeof(int));
    for (int i = 0; i < n_states; i++)
    {
        GtkWidget *checkbox = acceptance_checkboxes[i];
        gboolean is_active =
            gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbox));
        acceptance[i] = is_active ? 1 : 0;
    }

    FILE *fp = NULL;
    fp = fopen("DFA2PDF.tex", "w");
    print_base_packages(fp);
    print_title_section(fp);
    print_main_opening_section(fp);
    if (ready_to_print)
    {
        print_dfa_definition(fp, label_array, n_states, symbol_array, m_alphabet, trans_states, acceptance);
        print_dfa_graph(fp, label_array, n_states, symbol_array, m_alphabet, trans_states, acceptance);
        print_accepted_examples(fp, label_array, n_states, symbol_array, m_alphabet, trans_states, acceptance);
        print_rejected_examples(fp, label_array, n_states, symbol_array, m_alphabet, trans_states, acceptance);
        print_regex_dfa(fp, label_array, n_states, symbol_array, m_alphabet, trans_states, acceptance);
    }
    else
    {
        fprintf(fp, "Favor ingresar la configuración correcta del DFA, para poder "
                    "construir el PDF con información relevante.");
    }

    print_main_closing_section(fp);
    fclose(fp);
    system("pdflatex DFA2PDF.tex");
    system("evince -s DFA2PDF.pdf");
}

void print_base_packages(FILE *file)
{
    fprintf(file, "\\documentclass{article}\n");
    fprintf(file, "\\usepackage[english]{babel}\n");
    fprintf(file, "\\usepackage[a4paper,top=2cm,bottom=2cm,left=2cm,right=2cm,marginparwidth=1.75cm]{geometry}\n");
    fprintf(file, "\\usepackage{tikz}\n");
    fprintf(file, "\\usetikzlibrary{automata, positioning, arrows}\n");
}

void print_title_section(FILE *file)
{
    fprintf(file, "\\title{ Tecnológico de Costa Rica \\\\\nEscuela de Ingeniería en Computación\\\\\nTeoría de Automatas y Lenguajes Formales \\\\\nII Sem - 2023 \\\\\nProyecto Programado 1\n}");
    fprintf(file, "\\author{\nDaniel Amador Salas\\\\\n\\texttt{2017022096}\n\\and\nSebastián Francisco Gamboa Chacóns\\\\\n\\texttt{2017142512}\n\\and\nGerardo Gutierrez Quirós\\\\\n\\texttt{2016140286}\n\\and\nJosef Ruzicka González\\\\\n\\texttt{2024800833}\n}\n");
    fprintf(file, "\\date{}\n");
}

void print_main_opening_section(FILE *file)
{
    fprintf(file, "\\begin{document}\n");
    fprintf(file, "\\maketitle\n");
}

void print_main_closing_section(FILE *file)
{
    fprintf(file, "\\end{document}\n");
}

void print_dfa_definition(FILE *file, char **states, int states_num, char *alphabet, int alphabet_size, int **trans_states, int *acceptance)
{
    fprintf(file, "\\section{Definición Formal}\n");
    fprintf(file, "Un DFA se define como el siguiente quinteto:");
    fprintf(file, "\\begin{equation}\n");
    fprintf(file, "M = (Q, \\sum, \\delta, q_0, F)");
    fprintf(file, "\\\n\\end{equation}\n");
    fprintf(file, "\\subsection{Q}\n");
    fprintf(file, "Es el conjunto de estados finitos.\\\\En nuestro ejemplo este conjunto está dado por: \n");
    fprintf(file, "\\begin{equation}\n \\{");
    iterate_states(file, states, states_num);
    fprintf(file, "\\}\n\\end{equation}\n");
    fprintf(file, "\\subsection{$\\sum$}\n");
    fprintf(file, "Es el alfabeto.\\\\En nuestro ejemplo este conjunto está dado por: \n");
    fprintf(file, "\\begin{equation}\n \\{");
    iterate_alphabet(file, alphabet, alphabet_size);
    fprintf(file, "\\}\n\\end{equation}\n");
    fprintf(file, "\\subsection{$\\delta$}\n");
    fprintf(file, "Es la función de transición. La podemos representar como una tabla donde las filas son los estados y las columnas los símbolos del alfabeto.\\\\\n Formalmente se representa como el siguiente producto cartesiano:");
    fprintf(file, "\\begin{equation}\n");
    fprintf(file, "\\delta: Q x \\sum");
    fprintf(file, "\n\\end{equation}\n");
    fprintf(file, "En nuestro ejemplo esta tabla corresponde a: \n");
    print_table(file, trans_states, alphabet_size, states_num, states, alphabet);
    fprintf(file, "\\subsection{$q_0$}\n");
    fprintf(file, "Es el estado inicial.\\\\En nuestro ejemplo el estado inicial es: \\\n %s", states[0]);
    fprintf(file, "\\subsection{F}\n");
    fprintf(file, "Es el conjunto de estados de aceptación.\\\\En nuestro ejemplo este conjunto está dado por: \n");
    fprintf(file, "\\begin{equation}\n \\{");
    for (int i = 0; i < states_num; i++)
    {
        if (acceptance[i])
        {
            fprintf(file, "%s, ", states[i]);
        }
    }
    fprintf(file, "\\}\n\\end{equation}\n");
}

void print_dfa_graph(FILE *file, char **states, int states_num, char *alphabet, int alphabet_size, int **trans_states, int *acceptance)
{
    fprintf(file, "\\section{Grafo del DFA}\n");
    fprintf(file, "\\tikzset{ \n ->, >=stealth, node distance=3cm\n}");
    int i;
    fprintf(file, "\\begin{tikzpicture}\n");
    for (i = 0; i < states_num; i++)
    {
        if (i == 0)
        {
            if (acceptance[i])
            {
                fprintf(file, "\\node[state, initial, accepting] (%i) {$%s$};\n", i, states[i]);
            }
            else
            {
                fprintf(file, "\\node[state, initial] (%i) {$%s$};\n", i, states[i]);
            }
        }
        else
        {
            if (acceptance[i])
            {
                fprintf(file, "\\node[state, accepting, right of=%i] (%i) {$%s$};\n", i - 1, i, states[i]);
            }
            else
            {
                fprintf(file, "\\node[state, , right of=%i] (%i) {$%s$};\n", i - 1, i, states[i]);
            }
        }
    }

    int render_draw = 0;
    for (i = 0; i < states_num; i++)
    {
        for (int j = 0; j < alphabet_size; j++)
        {
            int value = (trans_states[i][j] == -1) ? -1 : (trans_states[i][j] - 1);
            if (value != -1)
            {
                if (!render_draw)
                {
                    fprintf(file, "\\draw ");
                    render_draw = 1;
                }
                if (value == i)
                {
                    fprintf(file, "(%i) edge[loop below] node{%c} (%i) \n", i, alphabet[j], value);
                }
                else
                {
                    int diff = (i < value) ? (value - i) : (i - value);
                    int alphabet_delta = (j < value) ? (value - j) : (j - value);
                    float bend_range = diff + (0.75 * alphabet_delta);
                    int ival = (int)bend_range;
                    int frac = (bend_range - ival) * 100;
                    if (i < j)
                    {
                        fprintf(file, "(%i) edge[bend left=%i.%icm, above] node {%c} (%i)\n", i, ival, frac, alphabet[j], value);
                    }
                    else
                    {
                        fprintf(file, "(%i) edge[bend left=%i.%icm, below] node {%c} (%i)\n", i, ival, frac, alphabet[j], value);
                    }
                }
            }
        }
    }
    if (render_draw)
    {
        fprintf(file, ";\n");
    }
    fprintf(file, "\\end{tikzpicture}\n");
}

// Function to generate a random string of specified length using characters from the given array
char *generate_random_string(char *characters, int length, int alphabet_size)
{
    char *random_string = (char *)malloc((length + 1) * sizeof(char)); // Allocate memory for the string
    if (random_string == NULL)
    {
        fprintf(stderr, "Memory allocation failed!\n");
        exit(1);
    }

    // Seed the random number generator
    srand(time(NULL));

    // Generate random string
    for (int i = 0; i < length; i++)
    {
        random_string[i] = characters[rand() % alphabet_size];
    }
    random_string[length] = '\0'; // Null-terminate the string
    return random_string;
}

// Function to check if a string is already in the list of generated strings
int is_string_in_list(char **list, int list_size, char *string)
{
    for (int i = 0; i < list_size; i++)
    {
        if (strcmp(list[i], string) == 0)
        {
            return 1; // String found in the list
        }
    }
    return 0; // String not found in the list
}

void print_accepted_examples(FILE *file, char **states, int states_num, char *alphabet, int alphabet_size, int **trans_states, int *acceptance)
{

    // Start the new section.
    fprintf(file, "\\section{Ejemplos de Hileras Aceptadas}\n");
    fprintf(file, "\\begin{equation}\n\\{\n");
    int max_str_len = 7;

    // Check if there are aceptance states.
    int acceptanceExist = 0;
    for (int i = 0; i < n_states; i++)
    {
        if (acceptance[i] == 1)
        {
            acceptanceExist = 1;
            break;
        }
    }

    if (acceptanceExist == 1)
    {
        // If the first element is a acceptance state we add it to the options.
        if (acceptance[0])
        {
            fprintf(file, "\\epsilon, ");
        }

        int **fixed_table = NULL;
        fix_ui_transition_table(&fixed_table, trans_states, states_num, alphabet_size);

        int num_strings = 5; // Number of random strings to generate
        char *generated_strings[num_strings];
        // Generate and print random strings
        int count = 0;
        while (count < num_strings || count == 300)
        {

            int string_length = rand() % (max_str_len + 1); // Length of each random string
            char *random_string = generate_random_string(alphabet, string_length, alphabet_size);

            dfa_execution_history solved_dfa = solve_dfa(random_string, alphabet, fixed_table, acceptance);
            if (solved_dfa.is_accepted)
            {
                if (count == 0)
                {
                    fprintf(file, "%s,", random_string);
                    generated_strings[count] = random_string;
                    count++;
                }
                else
                {
                    if (!is_string_in_list(generated_strings, count, random_string))
                    {
                        fprintf(file, "%s,", random_string);
                        generated_strings[count] = random_string;
                        count++;
                    }
                    else
                    {
                        free(random_string); // Free the memory if the string is a duplicate
                    }
                }
            }
        }
    }

    fprintf(file, "\\}\n\\end{equation}\n");
}

void print_rejected_examples(FILE *file, char **states, int states_num, char *alphabet, int alphabet_size, int **trans_states, int *acceptance)
{
    // Start the new section.
    fprintf(file, "\\section{Ejemplos de Hileras Rechazadas}\n");
    fprintf(file, "\\begin{equation}\n\\{\n");
    int max_str_len = 7;

    // If the first element is a acceptance state we add it to the options.
    if (acceptance[0])
    {
        fprintf(file, "\\epsilon, ");
    }

    int **fixed_table = NULL;
    fix_ui_transition_table(&fixed_table, trans_states, states_num, alphabet_size);

    int num_strings = 5; // Number of random strings to generate
    char *generated_strings[num_strings];
    // Generate and print random strings
    int count = 0;
    while (count < num_strings || count == 300)
    {

        int string_length = rand() % (max_str_len + 1); // Length of each random string
        char *random_string = generate_random_string(alphabet, string_length, alphabet_size);

        dfa_execution_history solved_dfa = solve_dfa(random_string, alphabet, fixed_table, acceptance);
        if (!solved_dfa.is_accepted)
        {
            if (count == 0)
            {
                fprintf(file, "%s,", random_string);
                generated_strings[count] = random_string;
                count++;
            }
            else
            {
                if (!is_string_in_list(generated_strings, count, random_string))
                {
                    fprintf(file, "%s,", random_string);
                    generated_strings[count] = random_string;
                    count++;
                }
                else
                {
                    free(random_string); // Free the memory if the string is a duplicate
                }
            }
        }
    }

    fprintf(file, "\\}\n\\end{equation}\n");
}
void print_regex_dfa(FILE *file, char **states, int states_num, char *alphabet, int alphabet_size, int **trans_states, int *acceptance)
{
    // TODO: TEOREMA DE ARDEN

    // State Equations
    fprintf(file, "\\section{ Regex - Teorema de Arden}\n");
    fprintf(file, "\\subsection{ Ecuaciones de Estado}\n");
    int current_char = 0;
    char ***state_equations = (char ***)malloc(100 * sizeof(char **)); // TODO: calloc de un x tamano (states*tamano_max_de cada equacion)
    for (int i = 0; i < n_states; i++)
    {
        state_equations[i] = (char **)malloc(100 * sizeof(char *));
    }

    // e + Bb + Ca
    // B = Aa

    // Traverse states
    for (int e = 0; e < states_num; e++)
    {
        fprintf(file, "\\begin{equation}\n");
        fprintf(file, "%s = ", states[e]);
        current_char = 0;

        if (e == 0)
        {
            fprintf(file, "\\epsilon ");
            state_equations[e][current_char] = g_strdup("e");
            current_char += 1;
        }

        // traverse columns and rows
        for (int i = 0; i < states_num; i++)
        {
            for (int j = 0; j < alphabet_size; j++)
            {

                if (trans_states[i][j] == (e + 1))
                { // E is running through all states, so e is [0-n_states[, where states names are [1-n_states]
                    if (current_char > 0)
                    {
                        fprintf(file, "+ ");
                    }
                    fprintf(file, "%d", (i + 1));
                    state_equations[e][current_char] = states[i];
                    current_char++;
                    fprintf(file, "%c", alphabet[j]);
                    state_equations[e][current_char] = (char *)alphabet[j];
                    current_char++;
                }
            }
        }
        fprintf(file, "\n\\end{equation}\n");
    }
}

void iterate_states(FILE *file, char **states, int states_num)
{
    int index = 0;
    char *state = states[index];
    while (index < states_num)
    {
        if (index + 1 == states_num)
        {
            fprintf(file, "%s", state);
        }
        else
        {
            fprintf(file, "%s, ", state);
        }
        index++;
        state = states[index];
    }
}

void iterate_alphabet(FILE *file, char *alphabet, int alphabet_size)
{
    int index = 0;
    char symbol = alphabet[index];
    while (index < alphabet_size)
    {
        if (index + 1 == alphabet_size)
        {
            fprintf(file, "%c", symbol);
        }
        else
        {
            fprintf(file, "%c, ", symbol);
        }
        index++;
        symbol = alphabet[index];
    }
}

void print_table(FILE *file, int **trans_states, int alphabet_size, int states_num, char **states, char *alphabet)
{
    fprintf(file, "\\begin{center}\n");
    fprintf(file, "\\begin{tabular}{");
    int a_index = 0;
    while (a_index < alphabet_size + 1)
    {
        if (a_index == alphabet_size)
        {
            fprintf(file, "c");
        }
        else
        {
            fprintf(file, "c ");
        }
        a_index++;
    }
    fprintf(file, " }\n");

    for (int j = 0; j < alphabet_size + 1; j++)
    {
        if (j == 0)
        {
            fprintf(file, " & ");
        }
        else
        {
            if (j == alphabet_size)
            {
                fprintf(file, "%c", alphabet[j - 1]);
            }
            else
            {
                fprintf(file, "%c & ", alphabet[j - 1]);
            }
        }
    }
    fprintf(file, "\\\\\n");
    for (int i = 0; i < states_num; i++)
    {
        fprintf(file, "%s & ", states[i]);
        for (int j = 0; j < alphabet_size; j++)
        {
            int value = trans_states[i][j];
            if (j + 1 == alphabet_size)
            {
                if (value == -1)
                {
                    fprintf(file, "-");
                }
                else
                {
                    fprintf(file, "%s", states[value - 1]);
                }
            }
            else
            {
                if (value == -1)
                {
                    fprintf(file, "- & ");
                }
                else
                {
                    fprintf(file, "%s & ", states[value - 1]);
                }
            }
        }
        fprintf(file, "\\\\\n");
    }

    fprintf(file, "\\end{tabular}");
    fprintf(file, "\\end{center}");
}
