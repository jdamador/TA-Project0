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

  dfa_transitions *transition = (dfa_transitions *)calloc(1, sizeof(struct dfa_transition));
  dfa_transitions *last_transition = transition;
  dfa_execution_history result = (dfa_execution_history){.is_accepted = 0, .transition_history = NULL};

  while (*strip)
  {
    if (result.transition_history == NULL)
    {
      result.transition_history = transition;
    }

    char current_symbol = *strip++;
    transition->symbol = current_symbol;
    transition->from = state;

    int symbol_position = decode_symbol(current_symbol, alphabet);

    if (symbol_position == -1)
    {
      transition->to = -1;
      transition->next = NULL;

      return result;
    }

    state = transition_table[state][symbol_position];
    transition->to = state;
    if (state == -1)
    {
      transition->next = NULL;

      return result;
    }

    transition->next = (dfa_transitions *)calloc(1, sizeof(dfa_transitions));
    last_transition = transition;
    transition = transition->next;
  }
  
  last_transition->next = NULL;
  free(transition);

  result.is_accepted = list_acceptance_states[state];

  return result;
}

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