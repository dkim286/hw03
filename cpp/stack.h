#ifndef STACK_H
#define STACK_H

#include <string>

// linked-list node
class Node
{
public:
	Node(int data, Node* next);

	int getValue() const;
	Node * getNext() const;

	void setValue(int);
	void setNext(Node *);

private:
	int data;
	Node * next_ptr;
};

// thread-safe stack
class Stack
{
public:
	Stack();
	~Stack();
	
	Node * top();
	int pop(int = -1);
	void push(int, int = -1);
	bool isEmpty();

	void print();
private:
	Node * top_ptr;
	int count;
};

// helper funcs
void write_row(int, const std::string &);

#endif
