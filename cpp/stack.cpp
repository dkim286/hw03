#include "stack.h"
#include "globals.h"
#include <iostream>

Node::Node(int n, Node* next) : data(n), next_ptr(next)
{
}

int Node::getValue() const
{
	return data;
}
Node * Node::getNext() const
{
	return next_ptr;
}

void Node::setValue(int n)
{
	data = n;
}

void Node::setNext(Node * ptr)
{
	next_ptr = ptr;
}


Stack::Stack() : top_ptr(nullptr), count(0)
{
}

Stack::~Stack()
{
	while (top_ptr)
	{
		Node * temp = top_ptr->getNext();
		delete top_ptr;
		top_ptr = temp;
	}
}
	
Node * Stack::top()
{
	return top_ptr;
}
int Stack::pop(int proc)
{
	pthread_mutex_lock(&mutex);

	if (isEmpty())
	{
		std::cout 
			<< "stack is empty! (if testing, this shouldn't happen)" 
			<< std::endl;
		pthread_mutex_unlock(&mutex);
		return 0;
	}

	int result = top_ptr->getValue();
	Node * temp = top_ptr->getNext();
	delete top_ptr;
	top_ptr = temp;
	count--;

	if (proc >= 0)
	{
		write_row(proc, "POP");
	}

	pthread_mutex_unlock(&mutex);

	temp = nullptr;
	return result;
}

void Stack::push(int data, int proc)
{
	pthread_mutex_lock(&mutex);

	Node * newNode = new Node(data, top_ptr);
	top_ptr = newNode;
	count++;

	if (proc >= 0)
	{
		write_row(proc, "push");
	}

	pthread_mutex_unlock(&mutex);

	newNode = nullptr;
}
bool Stack::isEmpty()
{
	return top_ptr == nullptr;
}

void Stack::print()
{
	std::cout << "Current stack: ";
	Node * ptr = top_ptr;
	while (ptr)
	{
		std::cout << ptr->getValue() << " ";
	}
	std::cout << std::endl;
}

void write_row(int proc, const std::string & action)
{
	for (int i = 0; i < proc; i++)
	{
		csv << ",";
	}
	csv << action;
	for (int i = proc; i < num_threads - 1; i++)
	{
		csv << ",";
	}
	csv << std::endl;
}

