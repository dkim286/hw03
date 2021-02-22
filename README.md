# Programming Assignment  #3 - Chapter 7 Synchronization Thread-Safe Stack 

## Description

A thread-safe version of whatever the assignment was uploaded with. Developed with [Chris](https://github.com/cdnutter).

## Building

If the executable is missing for some reason, run:

```text
$ gcc -pthread stack-ptr.c -o stack-ptr
```

## Usage

```text
$ ./stack-ptr [OPTION]
```

Running it without args outputs nothing useful. It "just werks" as per assignment specification. I think. It's really hard to tell. 

### Optional arguments

* `-d, --demo`
	* Run it with a small number of threads, then outputs a timeline to `log.csv`. 

* `-f, --force`
	* Same as `-d, --demo`, except ALL 200 threads and 500 iterations are run. 

* `-n, --normal`
	* Default program behavior. Added for the sake of documentation.

* `-h, --help`
	* Display something similar to this section on terminal.

