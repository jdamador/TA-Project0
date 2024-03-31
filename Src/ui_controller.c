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

// Automaton variable settings;
gint n_states;
gint m_alphabet;

// Validation variables
int alphabet_error = 0;
int states_name_error = 0;

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
void clean_textView_object();
void clean_evaluate_strip_object();
void enable_evalute_button();

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
    evaluate_button = GTK_WIDGET(gtk_builder_get_object(ui_builder, "btn_evaluate"));
    evaluate_strip_button = GTK_WIDGET(gtk_builder_get_object(ui_builder, "btn_evaluate_strip"));
    visual_transition_table = GTK_WIDGET(gtk_builder_get_object(ui_builder, "dfa_table"));
    scrolled_layout = GTK_WIDGET(gtk_builder_get_object(ui_builder, "scrolled_layout"));
    evaluate_strip = GTK_WIDGET(gtk_builder_get_object(ui_builder, "evaluate_strip"));

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
        gtk_widget_set_sensitive(evaluate_button, 0);
        gtk_widget_set_sensitive(evaluate_strip_button, 0);
        gtk_widget_set_sensitive(evaluate_strip, 0);
        break;
    case 2:
        // after settings are made: evaluate button is enable, but evaluate_strip keeps disable.
        gtk_widget_set_sensitive(evaluate_button, 1);
        gtk_widget_set_sensitive(evaluate_strip_button, 0);
        gtk_widget_set_sensitive(evaluate_strip, 0);
        break;
    case 3:
        // after evaluate is pressed: DFA is ready so, evaluate button is disable again, and evaluate_strip is enable.
        gtk_widget_set_sensitive(evaluate_button, 0);
        gtk_widget_set_sensitive(evaluate_strip_button, 1);
        gtk_widget_set_sensitive(evaluate_strip, 1);
        break;
    }
}

/*
 * Clean testView Event: clear the text in the textView output result.
 *
 * Parameters:
 *   void
 *
 * Output:
 *   void.
 */

void clean_textView_object() {
    // Print automaton transition history in GTK Text View.
    GtkTextView *textView = GTK_TEXT_VIEW(gtk_builder_get_object(ui_builder, "textview"));
    GtkTextBuffer *textViewBuffer = gtk_text_view_get_buffer(textView);

    // wrap text around textView
    gtk_text_view_set_wrap_mode(textView, GTK_WRAP_WORD_CHAR);

    gtk_text_buffer_set_text(textViewBuffer, "", -1);
}

/*
 * Clean testView Event: clear the text in the textView output result.
 *
 * Parameters:
 *   void
 *
 * Output:
 *   void.
 */

void clean_evaluate_strip_object() {
    // Print automaton transition history in GTK Text View.
    gtk_entry_set_text(GTK_ENTRY(evaluate_strip), "");
}

/*
 * End Clicked Event: handler the end event, just finish the program.
 *La Tortura

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
    // Get n_states and m_alphabet_elements from spin buttons.
    n_states = (int)gtk_spin_button_get_value(GTK_SPIN_BUTTON(gtk_builder_get_object(ui_builder, "sp_n_states")));
    m_alphabet = (int)gtk_spin_button_get_value(GTK_SPIN_BUTTON(gtk_builder_get_object(ui_builder, "sp_m_alphabet")));

    // clear buffer
    clean_textView_object();
    clean_evaluate_strip_object();

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

    // clear buffer
    clean_textView_object();
    clean_evaluate_strip_object();

    // Manage which elements must be disable.
    activation_handler(3);
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
    // clear buffer
    clean_textView_object();

    const gchar *strip2eval = gtk_entry_get_text(GTK_ENTRY(evaluate_strip));
    execute_strip_evaluation(strip2eval, alphabet_symbols, transition_table, acceptance_states, state_names);
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

    // For each alphabet_symbol_entries element set a NULL value
    for (int i = 0; i < m_alphabet; i++)
    {
        alphabet_symbol_entries[i] = NULL;
    }

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
        gtk_entry_set_max_length(GTK_ENTRY(custom_name), 20);                                       // Max length = 20.
        g_signal_connect(custom_name, "changed", G_CALLBACK(custom_name_changed_event), NULL);
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
            g_signal_connect(new_transition, "changed", G_CALLBACK(transition_changed_event), NULL);
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
    const gchar *text = gtk_entry_get_text(entry);

    if (g_utf8_strlen(text, -1) > 20) {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(typed_data),
                                    GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                    GTK_MESSAGE_ERROR,
                                    GTK_BUTTONS_OK,
                                    "The state name cannot be longer than 20 characters!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);

        states_name_error = 1;
    } else {
        states_name_error = 0;

        int counter = 0;

        for (int i = 0; i < n_states; i++) {
            const gchar *text = gtk_entry_get_text(GTK_ENTRY(state_names_entries[i]));
            if (text != NULL && strcmp(text, input_text) == 0) {
                counter++;
            }
        }

        if(counter > 1) {
            GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(typed_data),
                                        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                        GTK_MESSAGE_WARNING,
                                        GTK_BUTTONS_OK,
                                        "The custom name has already been used, please use another one!");
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);

            states_name_error = 1;
        } else {
            states_name_error = 0;
        }
    }

    enable_evalute_button();
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

    // In this way you can get the text to validate if it meets the requirements.

    // And in this way you can set the text in the entry 

     // TODO: validations on state names entries should be here.
    const gchar *input_text = gtk_entry_get_text(entry);
    gchar *trimmed_text = g_strstrip(g_strdup(input_text));
    gint length = g_utf8_strlen(trimmed_text, -1);
    g_free(trimmed_text);

    if (length == 0) {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(typed_data),
                                    GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                    GTK_MESSAGE_WARNING,
                                    GTK_BUTTONS_OK,
                                    "The alphabet value cannot be an empty character!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);

        alphabet_error = 1;
    } else {
        alphabet_error = 0;

        int counter = 0;

        for (int i = 0; i < m_alphabet; i++) {
            const gchar *text = gtk_entry_get_text(GTK_ENTRY(alphabet_symbol_entries[i]));
            if (text != NULL && strcmp(text, input_text) == 0) {
                counter++;
            }
        }

        if(counter > 1) {
            GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(typed_data),
                                        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                        GTK_MESSAGE_WARNING,
                                        GTK_BUTTONS_OK,
                                        "The alphabet value needs to be unique!");
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);

            alphabet_error = 1;
        } else {
            alphabet_error = 0;
        }
    }
    enable_evalute_button();
}

/*
 * Transition Changed Event: this method is added to every transition entry, so when it change, sent a event.
 *
 * Parameters:
 *   alphabet_symbols: all the alphabet symbols (uniques).
 *   transition_table: the table with the transitions between states.
 *   acceptance_states: the list of acceptance states.
 *   state_names: the list of new states names, they are empties if nothing is typed by the user.
 *   typed_data: the data typed in the entry.
 * Output:
 *   void.
 */
void transition_changed_event(GtkEntry *entry, gpointer typed_data)
{
    // TODO: validations on in transitions states entries should be here.

    printf("names:\n");
    for (int i = 0; i < m_alphabet; i++) {
        const gchar *text = gtk_entry_get_text(GTK_ENTRY(alphabet_symbol_entries[i]));
        printf("%s ", text);
        printf("\n");
    }

        /* for (int j = 0; j < m_alphabet; j++) {
            printf("%d ", transition_table[i][j]);
        } */ 
}

void enable_evalute_button() {
    int disabled = alphabet_error || states_name_error;
    if (disabled) {
        gtk_widget_set_sensitive(evaluate_button, 0);
    } else {
        gtk_widget_set_sensitive(evaluate_button, 1);
    }
}

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
   
    if (dfa_history.transition_history != NULL){
        dfa_transitions *current_transition = dfa_history.transition_history; 
        char char_to_print = current_transition->symbol;
    	while (current_transition != NULL){
	    // transition to -1
	    if (current_transition->to == -1) {
                gtk_text_buffer_insert_at_cursor(textViewBuffer, "\nThere were no further valid transitions", -1);
	        break;
	    } else {
    	        // print transition
    	        // symbol
	        gtk_text_buffer_insert_at_cursor(textViewBuffer, "\nSymbol \"", -1);
    	        char_to_print = current_transition->symbol;
    	        snprintf(state_str, sizeof(state_str),"%c", char_to_print);
    	        gtk_text_buffer_insert_at_cursor(textViewBuffer, state_str, -1);
    	        gtk_text_buffer_insert_at_cursor(textViewBuffer, "\" produces a transition from state: ", -1);
	        // from
    	        snprintf(state_str, sizeof(state_str),"%s", state_names[current_transition->from]);
    	        gtk_text_buffer_insert_at_cursor(textViewBuffer, state_str, -1);
	        // to
    	        snprintf(state_str, sizeof(state_str),"%s", state_names[current_transition->to]);
    	        gtk_text_buffer_insert_at_cursor(textViewBuffer, " to state: ", -1);
    	        gtk_text_buffer_insert_at_cursor(textViewBuffer, state_str, -1);
	        // go to next transition in history
	        current_transition = current_transition->next;
	    }
        }
    // no transitions
    } else {
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
 * Fix UI Transition Table: this method is in charge of fix the positions relative from screen layout to the logic matrix.
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
