#include <stdlib.h> // Defines NULL
#include <stdio.h>

#include "queue.h"

void enqueue( Queue * q, Node * n )
{
	// If the new node is the only node in the queue, then set it
	// as the head and tail. Also set prev and next pointers to null
	// for the new node.
	if( q -> tail == NULL || q -> head == NULL )
	{
		q -> head = n;
		q -> tail = n;

		n -> next = NULL;
		n -> prev = NULL;

		return;
	}

	// Set the current tail's next node to point to the new node
	( q -> tail ) -> next = n;

	// Set the new node's previous node to point to the previous tail
	n -> prev = q -> tail;

	// Set the new node's next node to NULL since it is then edn
	n -> next = NULL;

	// Set the new node as the tail
	q -> tail = n;
}


Node * dequeue( Queue * q )
{
	// Get node from the head of the queue
	Node * n = q -> head;

	// Set the head to the next node
	q -> head = n -> next;

	// If the queue wasn't empty, set the new head's previous node to point to NULL since it is now the front node
	if( q -> head != NULL )
		( q -> head ) -> prev = NULL;

	return n;
}


Node * peek( Queue * q )
{
	return q -> head;
}
