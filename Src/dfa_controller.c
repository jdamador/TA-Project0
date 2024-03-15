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
 * Output: an dfa_execution_history element that contains state (1-accepted, 0-rejected) and a linked list with all of steps
 * followed to complete the DFA execution.
 */

dfa_execution_history dfa_core_execute(const char *row, char *alphabet, int **transition_table, int *list_acceptance_states)
{
    dfa_execution_history result = (dfa_execution_history){.state = 0, .transition_history = NULL};

    /*
    
        Code here.

    */
    return result;
}