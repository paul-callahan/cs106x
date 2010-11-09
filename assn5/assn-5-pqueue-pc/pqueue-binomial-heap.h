/**
 * pqueue-binomial-heap.h
 * priority-queue
 *
 * Created by Jerry Cain on 10/23/10.
 * Copyright 2010 Stanford University. All rights reserved.
 * Student:  Paul Callahan
 * Grader:   Alex Quach
 */

#ifndef __binomial_heap_pqueue_cs106__
#define __binomial_heap_pqueue_cs106__

#include "pqueue.h"
#include <vector>

class BinomialHeapPQueue : public PQueue {
public:

 /*
  * Constructor: 
  * -----------------------------
  *  Build and initialize a default heap.
  */
  BinomialHeapPQueue();

 /*
  * Constructor: 
  * -----------------------------
  *  Build and initialize a heap that will be size size.
  */
  BinomialHeapPQueue(int size);

 /*
  * Destructor:
  * -----------------------------
  *  delete any remaining nodes and clean up state.
  */
	~BinomialHeapPQueue();
  
 /*
  * Method: merge
  * -----------------------------
  *  Merges an other queue with this one.
  */
  void merge(BinomialHeapPQueue *other); 

 /*
  * Method: merge
  * -----------------------------
  *  Destructively merges two queues together and returns a new queue.
  */
	static BinomialHeapPQueue *merge(BinomialHeapPQueue *one, BinomialHeapPQueue *two);

 /*
  * Method: merge
  * -----------------------------
  *  Destructively merges two queues together and places the results in a preallocated
  *  queue (newQueue).  Also returns newQueue.
  */
  static BinomialHeapPQueue *merge(BinomialHeapPQueue *one, BinomialHeapPQueue *two, 
    BinomialHeapPQueue* newQueue);
	
 /*
  * Method: enqueue
  * -----------------------------
  *  Enqueue a string on to this heap.
  */
	void enqueue(string elem);

 /*
  * Method: extractMin
  * -----------------------------
  *  Extract and return the least string from this heap.
  */
	string extractMin();

 /*
  * Method: peek
  * -----------------------------
  *  Return the least string from this heap without removing it
  */
	string peek();


	
private:
 /*
  * Record: node
  * -----------------------------
  *  Represents a binomial heap node with a value.  Each heap node has a list of child nodes.
  *  The order of this heap node is the size of the child list.
  */
  struct node {
    string value;
    vector<node*> children;
  };

 /*
  * Member: heapRoots
  * -----------------------------
  *  List of root-level binomial heap nodes.  Each root value is the least value in its
  *  entire tree.
  */
  vector<node*> heapRoots;

 /*
  * Member: tmpQueue
  * -----------------------------
  *  A temporary queue that gets reused over and over for enqueue and extractMin operations.
  *  An optimination over reallocating it on the stack it every time. 
  *  (hundreds of thousands of times).
  */
  BinomialHeapPQueue* tmpQueue;

 /*
  * Member: singletonQueue
  * -----------------------------
  *  A temporary queue that gets reused over and over for enqueue operations.
  *  An optimination over reallocating it on the stack it every time. 
  *  (hundreds of thousands of times).
  */
  BinomialHeapPQueue* singletonQueue;

 /*
  * Member: remainingChildrenQueue
  * -----------------------------
  *  A temporary queue that gets reused over and over for extractMin operations.
  *  An optimination over reallocating it on the stack it every time. 
  *  (hundreds of thousands of times).
  */
  BinomialHeapPQueue* remainingChildrenQueue;

 /*
  * Method: init
  * -----------------------------
  *  Initialize state inside this queue object.
  */
  void init();

 /*
  * Method: findIndexOfFirstNonNull
  * -----------------------------
  *  Scans the list of heaps and returns the index of the very first non-null
  *  heap.
  */
  int findIndexOfFirstNonNull();

 /*
  * Method: findIndexOfMinimum
  * -----------------------------
  *  Returns the index of the heap with the least root value of all the heaps.
  */
  int findIndexOfMinimum();

 /*
  * Method: copyHeapRoots
  * -----------------------------
  *  Efficiently (I hope) copies the list of heaps from the src queue to the
  *  dest queue.
  */
  void copyHeapRoots(BinomialHeapPQueue& src, BinomialHeapPQueue& dest);

 /*
  * Method: add
  * -----------------------------
  *  "adds" two heaps together like they are binary numbers and returns the 
  *  result.  Any carry value gets  assigned to the carryOut parameter.
  */
  static node* add(node* operandOne, node* operandTwo, node*& carryOut);
  
 /*
  * Method: deleteElements
  * -----------------------------
  *  Deletes each heap from the list of heaps.
  */
  int deleteElements();

 /*
  * Method: deleteElements
  * -----------------------------
  *  Recursively deletes a heap.
  */
  static int deleteElements(node* n);

};



#endif
