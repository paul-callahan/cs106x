/**
* File: pqueue-binomial-heap.cpp
* ---------------------
* binomial heap priority queue subclass.
*
* Author: Paul Callahan
* For: CS106X Assignment 5, Priority Queues
* Date: Nov 7th, 2010
*/

#include "pqueue-binomial-heap.h"

BinomialHeapPQueue::BinomialHeapPQueue() {}
BinomialHeapPQueue::~BinomialHeapPQueue() {}

string BinomialHeapPQueue::peek() { 
	// placeholder so method compiles.. 
	// replace with your own implementation
	return ""; 
}

string BinomialHeapPQueue::extractMin() {
	// placeholder so method compiles.. 
	// replace with your own implementation
	return peek();
}

void BinomialHeapPQueue::enqueue(string elem) {
	// placeholder so method compiles.. 
	// replace with your own implementation
}

BinomialHeapPQueue *BinomialHeapPQueue::merge(BinomialHeapPQueue *one, BinomialHeapPQueue *two) {
  BinomialHeapPQueue* newQueue = new BinomialHeapPQueue(one->size() + two->size());

  vector<node*> smallerHeapList = (one->size() <= two->size()) ? one->heapRoots : two->heapRoots;
  vector<node*> largerHeapList  = (one->size()  > two->size()) ? one->heapRoots : two->heapRoots;

  node* carry = NULL;
  for (int i = 0; i < smallerHeapList.size(); i++) {
    node* heapSum = add(carry, largerHeapList[i], carry, i);
    heapSum = add(smallerHeapList[i], heapSum, carry, i);
    newQueue->heapRoots.push_back(heapSum);
  }

  for (int i = smallerHeapList.size(); i < largerHeapList.size(); i++) {
    node* heapSum = add(carry, largerHeapList[i], carry, i);
    newQueue->heapRoots.push_back(heapSum);
  }

  if (carry) {
    newQueue->heapRoots.push_back(carry);
  }
	return newQueue;
}


BinomialHeapPQueue::node* BinomialHeapPQueue::add(node* operandOne, node* operandTwo, node*& carryOut, int order) {
  if (operandOne && operandTwo) {
    if (operandOne->value < operandTwo->value) {
      operandOne->children.push_back(operandTwo);
      carryOut = operandOne;
    } else {
      operandTwo->children.push_back(operandOne);
      carryOut = operandTwo;
    }
    return NULL;
  }
  carryOut = NULL;

  if (operandOne) {
    return operandOne;
  } else if (operandTwo) {
    return operandTwo;
  } else {
    return NULL;
  }
}


