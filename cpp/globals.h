#ifndef GLOBALS_H
#define GLOBALS_H

#include <pthread.h>
#include <iostream>

// these all sit inside the stack-ptr.cpp
extern int num_threads;
extern int iterations;
extern bool testing;
extern pthread_mutex_t mutex;
extern std::ostream csv;

#endif
