/* Chris Nutter
 * Austin Kim
 *
 * Programming Assignment 3
 * CPSC 351-04
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

// mutex
pthread_mutex_t mutex;

// Linked list node
typedef int value_t;
typedef struct Node
{
	value_t data;
	struct Node *next;
} StackNode;

// Stack function declarations
void push (value_t v, StackNode **top);
value_t pop (StackNode **top);
int is_empty(StackNode *top);

/* -------------------------------------------------------------------------- */

// packed argument for thread starters
typedef struct 
{
	StackNode ** top;
	int thread_num;
} Package;

// assignment function decls
void push_m (value_t v, StackNode **top, int thread_num);
value_t pop_m (StackNode **top, int thread_num);
void print_stack(StackNode * top);
void write_row(int idx, char * action);
void write_header();
void * testStack(void * arg);
void handle_args(int, char **);

// globals
int NUM_THREADS = 200;
int ITERATIONS = 500;
static int testing = 0;

FILE * csv;
int main(int argc, char ** argv)
{
	handle_args(argc, argv);


	StackNode *top = NULL;
	pthread_mutex_init(&mutex, NULL);

	pthread_t workers[NUM_THREADS];
	pthread_attr_t attrs[NUM_THREADS];

	if (testing == 1) 
	{
		csv = fopen("log.csv", "w");
		write_header();
	}

	// create args
	Package * packages = (Package*) malloc(sizeof(Package) * NUM_THREADS);
	for (int i = 0; i < NUM_THREADS; i++)
	{
		packages[i].top = &top;
		packages[i].thread_num = i;
	}

	// create threads
	for (int i = 0; i < NUM_THREADS; i++)
	{
		pthread_attr_init(attrs + i);
		pthread_create(workers + i, attrs + i, testStack, &packages[i]);
	}

	// wait for threads
	for (int i = 0; i < NUM_THREADS; i++)
	{
		pthread_join(workers[i], NULL);
	}

	pthread_mutex_destroy(&mutex);
	
	if (testing == 1) fclose(csv);

	// cleanup args
	free(packages);

	return 0;
}

// Stack function definitions
void push(value_t v, StackNode **top)
{
	StackNode * new_node = malloc(sizeof(StackNode));

	new_node->data = v;

	pthread_mutex_lock(&mutex);
	new_node->next = *top;
	*top		   = new_node;
	pthread_mutex_unlock(&mutex);
}

void push_m(value_t v, StackNode **top, int thread_num)
{
	StackNode * new_node = malloc(sizeof(StackNode));

	new_node->data = v;

	pthread_mutex_lock(&mutex);
	new_node->next = *top;
	*top		   = new_node;
	if (thread_num >= 0 && testing == 1) write_row(thread_num, "push");
	pthread_mutex_unlock(&mutex);
}

value_t pop(StackNode **top)
{
	pthread_mutex_lock(&mutex);
	if (is_empty(*top)) 
	{
		printf("stack empty!\n");
		return (value_t)0;
	}

	value_t	 data  = (*top)->data;
	StackNode * temp  = *top;
	*top			  = (*top)->next;

	free(temp);
	pthread_mutex_unlock(&mutex);
	return data;
}
value_t pop_m(StackNode **top, int thread_num)
{
	pthread_mutex_lock(&mutex);
	if (is_empty(*top)) 
	{
		printf("stack empty!\n");
		return (value_t)0;
	}

	value_t	 data  = (*top)->data;
	StackNode * temp  = *top;
	*top			  = (*top)->next;

	free(temp);
	if (thread_num >= 0 && testing == 1) write_row(thread_num, "POP");
	pthread_mutex_unlock(&mutex);
	return data;
}

int is_empty(StackNode *top) 
{
	if (top == NULL) return 1;
	else			 return 0;
}


void print_stack(StackNode * top)
{
	printf("Current stack: ");
	while (top != NULL)
	{
		printf("%d ", top->data);
		top = top->next;
	}
	printf("\n");
}


void * testStack(void * arg)
{
	Package * packed = (Package *)arg;
	StackNode ** top = packed->top;
	int thread_num = packed->thread_num;

	for (value_t i = 0; i < ITERATIONS; i++)
	{
		if (testing == 1) 
		{
			push_m(i, top, thread_num);
			pop_m(top, thread_num);
			push_m(i, top, thread_num);
			pop_m(top, thread_num);
			push_m(i, top, thread_num);
			pop_m(top, thread_num);
		}
		else
		{
			push(thread_num, top);
			pop(top);
			push(thread_num, top);
			pop(top);
			push(thread_num, top);
			pop(top);
		}
	}
	pthread_exit(0);
}

void write_row(int idx, char * action)
{
	for (int i = 0; i < idx; i++)
	{
		fprintf(csv, ",");
	}
	fprintf(csv, "%s", action);
	for (int i = idx; i < NUM_THREADS; i++)
	{
		fprintf(csv, ",");
	}
	fprintf(csv, "\n");
}

void write_header()
{
	fprintf(csv, "%s", "thread_0"); 
	for (int i = 1; i < NUM_THREADS; i++)
	{
		fprintf(csv, ",thread_%d", i);
	}
	fprintf(csv, "\n");
}

void handle_args(int argc, char ** argv)
{
	if (argc > 2) 
	{
		printf("%s: too many arguments\n", argv[0]);
		exit(1);
	}
	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
		{
			printf("Usage: %s [OPTION]\n"
					"A thread-safe stack in action.\n"
					"Running '%s' WITHOUT '-d, --demo' will spawn 200 threads as per homework specs.\n\n"
					"Optional arguments:\n"
					"  -d, --demo    Run this program in demo mode with a small number of threads.\n" 
					"                 This mode writes a timeline of events to 'log.csv' in the enclosing dir.\n"
					"  -f, --force   Run all 200 threads, 500 iterations, AND generate the 'log.csv' file.\n"
					"                 That's at least 120MB of pure text. PLEASE think twice before doing this.\n"
					"  -n, --normal  Run this program in normal mode (no 'log.csv'). This is the default behavior.\n"
					"  -h, --help    Display this help message\n",
					argv[0], argv[0]);
			exit(0);
		}
		else if (strcmp(argv[i], "--demo") == 0 || strcmp(argv[i], "-d") == 0)
		{
			testing = 1;
			NUM_THREADS = 10;
			ITERATIONS = 5;
		}
		else if (strcmp(argv[i], "--force") == 0 || strcmp(argv[i], "-f") == 0)
		{
			testing = 1;
		}
		else if (strcmp(argv[i], "--normal") == 0 || strcmp(argv[i], "-n") == 0)
		{
			// do nothing: default
		}
		else
		{
			printf("%s: unrecognized argument '%s'\n", argv[0], argv[i]);
			exit(1);
		}
	}
}
