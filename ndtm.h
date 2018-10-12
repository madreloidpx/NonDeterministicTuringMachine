#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT 2000
#define MAX_STEPS 8000
#define INITIAL_QUEUE_SIZE 50000
#define QUEUE_MULTIPLIER 10

typedef struct {
	int state;
	char tape[MAX_INPUT];
	int pos;
} State; //State struct

typedef struct{
	State *state_info;
	int capacity;
	int front;
} Stack; //Stack struct

typedef struct{
	int from, to;
	char read, write, move;
} Transition; //Transition struct

typedef struct{
	int head;
	Transition* contents;
	int count;
}LinkedList; //Linked List struct

typedef struct{
	LinkedList* transition_list;
	int * accepted_states;
	int count_transition;
	int count_accepted;
	int capacity_transition;
	int capacity_accepted;
	int max_steps;
}TM; //Turing machine transition list struct

void save_state(State *s, char tape[MAX_INPUT], int state, int pos);
int get_state(State *s);
int get_pos(State *s);
char* get_tape(State *s);
int change_tape(State *s, Transition *t);
void print_state(State s);

void make_stack(Stack *s);
int is_empty(Stack *s);
int is_full(Stack *s);
void push(Stack *s, State n);
State pop(Stack *s);
void free_stack(Stack *s);

void save_transition(Transition *t, int from, char read, char write, char move, int to);
int get_from(Transition *t);
char get_read(Transition *t);
void print_transition(Transition *t);

void create_list(LinkedList *list);
void insert_head(LinkedList *list, int head);
void add_content(LinkedList *list, Transition *t);
int get_head(LinkedList* list);
Transition* get_contents(LinkedList* list);
void free_list(LinkedList* list);

void start_tm(TM *tm);
int in_transition_list(TM *tm, int state);
void insert_transition(TM *tm, Transition *t);
void insert_acceptance(TM *tm, int state);
void edit_max_steps(TM *tm, int max);
LinkedList* get_transition_at_state(TM *tm, int state);
int check_accepted(TM *tm, State state);
void free_tm(TM *tm);
void print_all_transitions(TM *tm);
void print_all_acceptance_states(TM *tm);
void print_max_steps(TM *tm);
int check_validity(State *state, char tape[MAX_INPUT], Transition *t);
void dfs_run(TM *tm, Stack *state, int step);
void dfs(TM *tm, char tape[MAX_INPUT]);
void tr_start(char *s);
void check_movement(char move);
void tokenize(char line[MAX_INPUT], int *from, char *read, char *write, char *move, int *to);
