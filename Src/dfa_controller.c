/*
 * dfa_controller.c
 * Description: this file is in charge of manage the DFA structure.
 * Author: Daniel A, Sebastian G, Josef R, Gerardo G.
 * Date: 2024
 */

#include "dfa_interface.h"
#include <stddef.h>
#include <stdlib.h>

// Simple definition of methods, this allow the program to call the methods in wherever part of the program without having problems of hierarchy.
int decode_symbol(char, char *);

/*
 * Display_UI: This method is the handler for DFA core code.
 *
 * Parameters:
 *   row to eval, alphabet (Σ), transition table for all states, list acceptance states.
 *
 * Output:
 *    an dfa_execution_history element that contains state (1-accepted, 0-rejected) and a linked list with all the steps
 * followed to complete the DFA execution.
 */

dfa_execution_history solve_dfa(const char *strip2eval, char *alphabet, int **transition_table, int *list_acceptance_states)
{
  // First state is always the initial.
  char *strip = (char *)strip2eval;
  int state = 0;

  // Define the current transition state, and the last transition state: Handle FROM - TO.
  dfa_transitions *transition = (dfa_transitions *)calloc(1, sizeof(struct dfa_transition));
  dfa_transitions *last_transition = transition;
  dfa_execution_history result = (dfa_execution_history){.is_accepted = 0, .transition_history = NULL};

  // Run the loop for every strip's symbol.
  while (*strip)
  {
    
    // Assign the fist state.
    if (result.transition_history == NULL)
    {
      result.transition_history = transition;
    }

    // Set a new pointer for the current strip's symbol which is being analyzed.
    char current_symbol = *strip++;
    transition->symbol = current_symbol;
    transition->from = state;

    // We have the symbol of the alphabet, so we need to extract it and replace by the postion instead.
    int symbol_position = decode_symbol(current_symbol, alphabet);

    // If the symbol is -1, it means dont exist. So we define a non existant transition.
    if (symbol_position == -1)
    {
      transition->to = -1;
      transition->next = NULL;

      return result;
    }
    
    // If not is -1, we just add the data to the transition table.
    state = transition_table[state][symbol_position];
    transition->to = state;

    // If the process ended and the last state was -1, it means that ended in a rejected state, so we just set rejected as final result.
    if (state == -1)
    {
      transition->next = NULL;

      return result;
    }
    // If the strip evaluation is not ended, we just move forward to the next element to eval.
    transition->next = (dfa_transitions *)calloc(1, sizeof(dfa_transitions));
    last_transition = transition;
    transition = transition->next;
  }

  last_transition->next = NULL;

  // Free memory to avoid segmentation fault.
  free(transition);

  // At the en we complete the strip eval and set the acceptance state as the final result of the evaluation.
  result.is_accepted = list_acceptance_states[state];

  return result;
}


/*
 * Decode Symbol: This method convert a symbol element into a position in the alphabet array.
 *
 * Parameters:
 *   symbol: symbol to locate.
 *   alphabet: alphabet where the symbol should be located.
 *
 * Output:
 *   the position of the symbol in the alphabet array.
 */

int decode_symbol(char symbol, char *alphabet)
{
  size_t pos = 0;
  while (*alphabet)
  {
    if (symbol == *alphabet++)
    {
      return pos;
    }
    pos++;
  }

  // Symbol does not exist
  return -1;
}
