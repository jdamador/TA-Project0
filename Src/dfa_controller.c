/*
 * dfa_controller.c
 * Description: this file is in charge of manage the DFA structure.
 * Author: Daniel A, Sebastian G, Josef R, Gerardo G.
 * Date: 2024
 */

#include "dfa_interface.h"
#include <stddef.h>
#include <stdlib.h>

/*
 * This method is the handler for DFA core code.
 * Input: row to eval, alphabet (Σ), transition table for all states, list acceptance states.
 * Output: an dfa_execution_history element that contains state (1-accepted, 0-rejected) and a linked list with all the steps
 * followed to complete the DFA execution.
 */

dfa_execution_history dfa_core_execute(const char *row, char *alphabet, int **transition_table, int *list_acceptance_states)
{
  //
  dfa_t *current_transition = (dfa_t *)calloc(1, sizeof(struct dfa_transition));
  // dfa_transition *next_transition = (dfa_transition *)calloc(1, sizeof(dfa_transition));
  dfa_t*last_transition = current_transition;
  dfa_execution_history result = (dfa_execution_history){.state = 0,
                                                         .transition_history = current_transition};

  int input_index = 0;
  int state_index = 0;
  int alphabet_index = 0;
  //int acceptance_index = 0;

  // Iterate over input string
  while (row[input_index] != '\0')
  {
    // Update current transition data
    current_transition->symbol = row[input_index];
    current_transition->from = state_index;

    // Find index alphabet position of the input char
    while (alphabet_index != -1 || row[input_index] != alphabet[alphabet_index])
    {
      alphabet_index++;
      if (alphabet[alphabet_index] == '\0')
      {
        alphabet_index = -1;
      }
    }

    // Move to new state
    if (alphabet_index != -1)
    {
      state_index = transition_table[state_index][alphabet_index];
      current_transition->to = state_index;

      // Check acceptance
      // result.state = list_acceptance_states[state_index];
      //} else {
      // result.state = 0;
    }

    alphabet_index = 0;
    current_transition->next = (struct dfa_transition *)calloc(1, sizeof(struct dfa_transition));
    last_transition = current_transition;
    current_transition = current_transition->next;
  }
  // free memory
  last_transition->next = NULL;
  free(current_transition);

  // Check acceptance
  result.state = list_acceptance_states[state_index];
  return result;
}
