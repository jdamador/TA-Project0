#ifndef DFA_INTERFACE
#define DFA_INTERFACE

typedef int (*symbol2table)(char);

// This element is used to manage the transition table..
struct dfa_transition {
    char symbol;
    int from;
    int to;
    struct dfa_transition *next;
};

typedef struct dfa_transition dfa_transitions;

// This element is used to manage the final result after execute a DFA.
typedef struct{
    int is_accepted; // 1: approved | 0: rejected.
    dfa_transitions *transition_history;
} dfa_execution_history;


dfa_execution_history solve_dfa(const char *strip2eval, char *alphabet, int **tanstition_table, int *list_acceptance_states);

#endif
