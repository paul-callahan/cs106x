/**
 * pqueue-binomial-heap.h
 * priority-queue
 *
 * Created by Jerry Cain on 10/23/10.
 * Copyright 2010 Stanford University. All rights reserved.
 */

#ifndef __binomial_heap_pqueue_cs106__
#define __binomial_heap_pqueue_cs106__

#include "pqueue.h"
#include <vector>

class BinomialHeapPQueue : public PQueue {
public:
  BinomialHeapPQueue() {};
	~BinomialHeapPQueue();
	
  void merge(BinomialHeapPQueue *other);
	static BinomialHeapPQueue *merge(BinomialHeapPQueue *one, BinomialHeapPQueue *two);
  static BinomialHeapPQueue *merge(BinomialHeapPQueue *one, BinomialHeapPQueue *two, 
    BinomialHeapPQueue* newQueue);
	
	void enqueue(string elem);
	string extractMin();
	string peek();


	
private:
  struct node {
    string value;
    vector<node*> children;
  };

  BinomialHeapPQueue(int size);
  vector<node*> heapRoots;

  int findIndexOfFirstNonNull();
  int findIndexOfMinimum();
  void copyHeapRoots(BinomialHeapPQueue& src, BinomialHeapPQueue& dest);

  static node* add(node* operandOne, node* operandTwo, node*& carry, int order);
  
  int BinomialHeapPQueue::deleteElements();
  static int BinomialHeapPQueue::deleteElements(node* n);

  BinomialHeapPQueue& getSingletonQueue(string elem);

	// update the private section with the list of 
	// data members and helper methods needed to implement
	// the binomial-heap-backed version of the PQueue.
};



#endif