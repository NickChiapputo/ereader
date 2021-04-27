#ifndef QUEUE_H_
#define QUEUE_H_

typedef struct Vertex
{
	int label;			// Node label (e.g., 1, 2, 3, etc.)
	int edges;			// Number of adjacent nodes
	int visited;		// Set to 0 if not visited or 1 if visited during BFS
	int * adjList;		// List of adjacent nodes

	/* Add other parameters as necessary */
} Vertex;

typedef struct Node
{
	void * data;		// Data contained in this node
	struct Node * next;	// Points to node behind current node
	struct Node * prev; // Points to node in front of current node
} Node;

typedef struct Queue
{
	Node * head;		// Node at front of queue
	Node * tail;		// Node at back of queue
} Queue;

void enqueue( Queue *, Node * );
Node * dequeue( Queue * );
Node * peek( Queue * );

#endif