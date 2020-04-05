#include <fstream>
#include <vector>

#include <pthread.h>
#include <string.h>

#include "stack.h"

using namespace std; 

// GLOBALS -------------
pthread_mutex_t mutex;

int num_threads = 200;
int iterations = 500;
bool testing = false;

ofstream csv;
// ---------------------

// packed arg for thread starters
struct Package
{
	Stack * stack;
	int thread_num;
	std::ostream * out;
};

// misc functions
void * testStack(void *);
void handle_args(int, char **);
void write_header(std::ostream &);

string filename = "log.csv";

int main(int argc, char ** argv)
{
	handle_args(argc, argv);
	pthread_mutex_init(&mutex, NULL);

	if (testing)
	{
		csv.open(filename);
		write_header(csv);
	}

	// create args
	vector<Package> packages(num_threads);
	Stack * stack = new Stack();
	for (int i = 0; i < num_threads; i++)
	{
		packages.at(i) = Package{stack, i, &csv};
	}

	// create threads
	vector<pthread_t> workers(num_threads);
	vector<pthread_attr_t> attrs(num_threads);
	for (int i = 0; i < num_threads; i++)
	{
		pthread_attr_init(&attrs.at(i));
		pthread_create(&workers.at(i), &attrs.at(i), testStack, &packages.at(i));
	}

	// wait for threads
	for (int i = 0; i < num_threads; i++)
	{
		pthread_join(workers.at(i), NULL);
	}

	// destroy mutex
	pthread_mutex_destroy(&mutex);

	if (testing) csv.close();

	// cleanup stack
	delete stack;

}

void * testStack(void * arg)
{
	Package * packed = (Package *)arg;
	Stack * stack = packed->stack;
	int thread_num = packed->thread_num;

	for (int i = 0; i < iterations; i++)
	{
		if (testing == 1) 
		{
			stack->push(i, thread_num);
			stack->pop(thread_num);
			stack->push(i, thread_num);
			stack->pop(thread_num);
			stack->push(i, thread_num);
			stack->pop(thread_num);
		}
		else
		{
			stack->push(i);
			stack->pop();
			stack->push(i);
			stack->pop();
			stack->push(i);
			stack->pop();
		}
	}
	pthread_exit(0);
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
			testing = true;
			num_threads = 10;
			iterations = 5;
		}
		else if (strcmp(argv[i], "--force") == 0 || strcmp(argv[i], "-f") == 0)
		{
			testing = true;
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

void write_header(std::ostream & out)
{
	out << "thread_0";
	for (int i = 1; i < num_threads; i++)
	{
		out << ",thread_" << i;
	}
	out << std::endl;
}
