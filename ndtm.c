#include "ndtm.h"
//--------------------------------------------------------State Functions
void save_state(State *s, char tape[MAX_INPUT], int state, int pos){
	s->state = state;
	strcpy(s->tape, tape);
	s->pos = pos;
} //save to a state object

int get_state(State *s){
	return s->state;
} //returns the state of state struct

int get_pos(State *s){
	return s->pos;
}//get current position

char* get_tape(State *s){
	return s->tape;
}//get tape

int change_tape(State *s, Transition *t){
	int pass = 1;
	char str[MAX_INPUT];
	strcpy(str, s->tape);
	if(t->write != '_') str[s->pos] = t->write;
	strcpy(s->tape, str);
	s->state = t->to;
	if(t->move == 'R'){
		s->pos++;
	}else if(t->move == 'L'){
		s->pos--;
	}
	if(s->pos < 0 || s->pos > sizeof(s->tape)){
		pass = 0;
	}
	return pass;
} //write the new tape in position and change the index using the move direction and move to next state

void print_state(State s){
	printf("Tape: %s\n", s.tape);
	printf("State: %d\nPosition: %d\n", s.state, s.pos);
} //print node


//--------------------------------------------------------Stack Functions
void make_stack(Stack *s){
	s->capacity = INITIAL_QUEUE_SIZE;
	s->state_info = (State*)malloc(s->capacity*sizeof(State));
	memset(s->state_info, 0, s->capacity*sizeof(State));
	s->front = -1;
}//initialize stack

int is_empty(Stack *s){
	if(s->front == -1) return 1;
	return 0;
} //check if the stack is empty

int is_full(Stack *s){
	if(s->front >= s->capacity) return 1;
	return 0;
}//check if the stack is full

void push(Stack *s, State n){
	s->front++;
	if(is_full(s)){
		s->capacity = s->capacity * QUEUE_MULTIPLIER;
		s->state_info = realloc(s->state_info, s->capacity*sizeof(State));
	}
	s->state_info[s->front] = n;
}//push the state in the stack

State pop(Stack *s){
	if(is_empty(s)){
		State n;
		save_state(&n, "", -1, -1);
		return n;
	}
	State n = s->state_info[s->front];
	s->front--;
	return n;
} //pop the top state in the stack

void free_stack(Stack *s){
	free(s->state_info);
} //free the stack contents

//--------------------------------------------------------Transition functions

void save_transition(Transition *t, int from, char read, char write, char move, int to){
	t->from = from;
	t->read = read;
	t->write = write;
	t->move = move;
	t->to = to;
}//initialize transition

int get_from(Transition *t){
	return t->from;
}//get from
char get_read(Transition *t){
	return t->read;
}
char get_move(Transition *t){
	return t->move;
}//get move

void print_transition(Transition *t){
	printf("From state %d, read %c, write %c, move %c, to state %d\n", t->from, t->read, t->write, t->move, t->to);
}//Prints the transition

//--------------------------------------------------------Linked List Functions

void create_list(LinkedList *list){
	list->head = -1;
	list->contents = NULL;
	list->count = 0;
}//initialize linked list

void insert_head(LinkedList *list, int head){
	list->head = head;
}//insert head state

void add_content(LinkedList *list, Transition *t){
	list->count++;
	if(list->count == 1){
		list->contents = malloc(sizeof(LinkedList));
		memset(list->contents, 0, sizeof(LinkedList));
	}else{
		list->contents = realloc(list->contents, list->count*sizeof(LinkedList));
	}
	list->contents[list->count-1] = *t;
}//adds a transition to the linked list

int get_head(LinkedList* list){
	return list->head;
}//returns the state head

Transition* get_contents(LinkedList* list){
	return list->contents;
}//returns an array of transitions

void free_list(LinkedList* list){
	for(int i = 0; i < list->count; i++){
		free(&list->contents[i]);
	}
	free(list->contents);
}//free the linked list

//--------------------------------------------------------Turing Machine Functions

void start_tm(TM *tm){
	tm->transition_list = NULL;
	tm->accepted_states = NULL;
	tm->count_transition = 0;
	tm->count_accepted = 0;
	tm->max_steps = MAX_STEPS;
} //initialize turing machine transitions

int in_transition_list(TM *tm, int state){
	for(int i = 0; i<tm->count_transition; i++){
		if(get_head(&tm->transition_list[i]) == state){
			return i;
		}
	}
	return -1;
} //checks if a state exists in the transition list

void insert_transition(TM *tm, Transition *t){
	int index = in_transition_list(tm, get_from(t));
	if(index == -1){
		tm->count_transition++;
		if(tm->count_transition == 1){
			tm->transition_list = (LinkedList*)malloc(sizeof(LinkedList));
			memset(tm->transition_list, 0, sizeof(LinkedList));
		}else{
			tm->transition_list = realloc(tm->transition_list, tm->count_transition*sizeof(LinkedList));
		}
		LinkedList temp;
		create_list(&temp);
		insert_head(&temp, get_from(t));
		add_content(&temp, t);
		tm->transition_list[tm->count_transition-1] = temp;
	}else{
		add_content(&tm->transition_list[index], t);
	}
}//inserts a transition to the turing machine list

void insert_acceptance(TM *tm, int state){
	tm->count_accepted++;
	if(tm->count_accepted == 1){
		tm->accepted_states = (int*)malloc(sizeof(int));
	}else{
		tm->accepted_states = realloc(tm->accepted_states, tm->count_accepted*sizeof(int));
	}
	tm->accepted_states[tm->count_accepted-1] = state;
} //inserts an acceptance state

void edit_max_steps(TM *tm, int max){
	tm->max_steps = max;
} //edit the max amount of steps

LinkedList* get_transition_at_state(TM *tm, int state){
	for(int i = 0; i<tm->count_transition; i++){
		if(get_head(&tm->transition_list[i]) == state){
			return &tm->transition_list[i];
		}
	}
	return NULL;
}//returns the transitions from the given state

int check_accepted(TM *tm, State state){
	for(int i = 0; i < tm->count_accepted; i++){
		if(tm->accepted_states[i] == get_state(&state)){
			//printf("%d, %d\n", tm->accepted_states[i], state);
			return 1;
		}
	}
	return 0;
}//check if the state is an accepted state

void free_tm(TM *tm){
	for(int i = 0; i<tm->count_transition; i++){
		free_list(&tm->transition_list[i]);
	}
	free(tm->transition_list);
	for(int i = 0; i<tm->count_accepted; i++){
		free(&tm->accepted_states[i]);
	}
	free(tm->accepted_states);
} //free the turing machine

void print_all_transitions(TM *tm){
	printf("All transitions:\n");
	for(int i = 0; i < tm->count_transition; i++){
		LinkedList *list = &tm->transition_list[i];
		Transition* t = get_contents(&tm->transition_list[i]);
		for(int j = 0; j<list->count; j++){
			print_transition(&t[j]);
		}
	}
} //prints all transition states

void print_all_acceptance_states(TM *tm){
	printf("All acceptance states:\n");
	for(int i = 0; i<tm->count_accepted; i++){
		printf("%d\n", tm->accepted_states[i]);
	}
}//prints all acceptance states

void print_max_steps(TM *tm){
	printf("Maximum Steps:\n");
	printf("%d\n", tm->max_steps);
}//prints the max steps

//--------------------------------------------------------DFS Functions
int check_validity(State *state, char tape[MAX_INPUT], Transition *t){
	if(get_from(t) == get_state(state) && (get_read(t) == tape[get_pos(state)] || get_read(t) == '_')){
		char _tape[MAX_INPUT];
		strcpy(_tape, tape);
		//printf("%c, %d, %d\n", get_move(t), (sizeof(_tape)/sizeof(char)), get_pos(state));
		//printf("%c\n", get_move(t));
		if(get_move(t) == 'R' && tape[get_pos(state)+1] != '\0') return 1;
		if(get_move(t) == 'L' && get_pos(state) > -1) return 1;
		if(get_move(t) == 'S') return 1;
	}
	return 0;
}//check if the transition fits the rules of the language

void dfs_run(TM *tm, Stack *stack, int step){
	char result = 'U';
	while(!is_empty(stack) && step < tm-> max_steps){
		//printf("\n\n\npass\n\n\n");
		State curr = pop(stack);
		//printf("POPPED:\n");
		//print_state(curr);
		char *tape = get_tape(&curr);
		LinkedList* list = get_transition_at_state(tm, get_state(&curr));
		if(list == NULL){
			int accept = check_accepted(tm, curr);
			if(accept == 1){
				result = '1';
				break;
			}else if(accept == 0){
				result = '0';
			}
		}else{
			Transition* branches = get_contents(list);
			for(int t = 0; t<list->count; t++){
				State _curr = curr;
				int valid = check_validity(&_curr, tape, &branches[t]);
				if(valid == 1){
					//printf("Current Transition\n");
					//print_transition(&branches[t]);
					if(step>=tm->max_steps) break;
					step++;
					int pass = change_tape(&_curr, &branches[t]);
					if(pass){
						push(stack, _curr);
						//printf("PUSHED\n");
					}
				}
			}
		}
		
		if(is_empty(stack)){
			//printf("\n\nFINAL\n");
			//printf("Stack is empty\n");
			//printf("Steps: %d\n", step);
			//printf("0\n\n");
			//printf("0");
			result = '0';
			break;
		}
		if(step>=tm->max_steps) break;
		step++;
	}
	//fprintf(stdout, "\nAFTER WHILE\n\n");
	//if(step >= tm->max_steps){
		//printf("\n\nFINAL\n");
		//printf("STEP: %d\n", step);
		//printf("-U\n\n");//return U
	//	printf("U");
	//	return;
	//}
	
	//printf("\n\nFINAL\n");
	//printf("STEP: %d\n");
	//printf("%c\n\n", result);
	printf("%c", result);
	
}//pops to state then checks for all valid states to traverse

void dfs(TM *tm, char tape[MAX_INPUT]){
		//printf("New tape\n");
		Stack stack;
		make_stack(&stack);
		State init;
		int step = 0;
		save_state(&init, tape, 0, 0);
		push(&stack, init);
		//fprintf(stdout, "\n\n\nBEFORE DFS_RUN\n\n\n");
		dfs_run(tm, &stack, step);
		//fprintf(stdout, "\nAFTER DFS_RUN\n\n");
		free_stack(&stack);
} //setups the dfs

//--------------------------------------------------------Input Verification functions
void tr_start(char *s){
	if(s == NULL || strstr(s, "tr") == NULL){
		fprintf(stderr, "Invalid input. Does not begin with 'tr'. Starts with %s.\n", s);
		exit(0);
	}
}//checks if the line begins with "tr". Ends if not.

void check_movement(char move){
	if(move != 'L' && move != 'S' && move != 'R'){
		fprintf(stderr, "Invalid input. Error in format. Movement input is %c.\n", move);
		exit(0);
	}
}//checks the format if the movement has a correct format. Ends if not.


//--------------------------------------------------------Misc Functions
void tokenize(char line[MAX_INPUT], int *from, char *read, char *write, char *move, int *to){
	char* token;
	char token2;
	token = strtok(line, " ");
	*from = atoi(token);
	token = strtok(NULL, " ");
	*read = token[0];
	token = strtok(NULL, " ");
	*write = token[0];
	token = strtok(NULL, " ");
	*move = token[0];
	token = strtok(NULL, " ");
	*to = atoi(token);
}//splits the line input using spaces, and saves the values to corresponding variables

int main(int argc, char *argv[]){
	char line[MAX_INPUT]; //line buffer
	char* newline;
	line[0] = '\0';
	fgets(line, sizeof(line), stdin); //gets input from cmd
	if ((newline=strchr(line, '\n')) != NULL){ //checks if there's a newline and replaces with a null character
		*newline = '\0';
	}

	tr_start(line); //checks if the line begins with "tr". Ends if not.

	TM tm;
	start_tm(&tm);

	while(strstr(line, "acc") == NULL){ //continue to look for patterns in the stdin for states until the acceptance tag is reached. End when it reaches.
		int to, from;
		char read, write, move;
		int buff = 0;

		line[0] = '\0';
		fgets(line, sizeof(line), stdin);
		if ((newline=strchr(line, '\n')) != NULL){
			*newline = '\0';
		}

		if(buff == 0 && strstr(line, "acc") == NULL){
			tokenize(line, &from, &read, &write, &move, &to); //splits the line input using spaces, and saves the values to corresponding variables
			check_movement(move); //checks the format if the movement has a correct format. Ends if not.

			Transition *curr = (Transition*)malloc(sizeof(Transition));
			save_transition(curr, from, read, write, move, to);
			insert_transition(&tm, curr);
			buff++;
		}
		continue;
	}
	//print_all_transitions(&tm);

	while(strstr(line, "max") == NULL){ //continue to look for acceptance states until the max tag is reached. End when it reaches.
		int accepted_state;

		line[0] - '\0';
		fgets(line, sizeof(line), stdin);
		if ((newline=strchr(line, '\n')) != NULL){
			*newline = '\0';
		}
		if(strstr(line, "max") == NULL){
			accepted_state = atoi(line);
			insert_acceptance(&tm, accepted_state);
		}
	}

	line[0] - '\0';
	fgets(line, sizeof(line), stdin);
	if ((newline=strchr(line, '\n')) != NULL){
		*newline = '\0';
	}
	int max_steps = atoi(line); //gets the max amount of steps
	edit_max_steps(&tm, max_steps);

	while(1){
		int c = getchar();
		if(c == EOF){ //checks if end of file
			break;
		}
		ungetc(c, stdin);

		line[0] - '\0';
		fgets(line, sizeof(line), stdin); //gets the tape list
		if ((newline=strchr(line, '\n')) != NULL){
			*newline = '\0';
		}
		if(strstr(line, "run") != NULL){ //until the max tag is reached. End when it reaches.
			continue;
		}

		//fprintf(stdout, "%s\n", line);

		dfs(&tm, line); //perform depth first search
	}

	free_tm(&tm);
}