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
#include <iostream>
#include "strutils.h"

bool debug_x = false;

BinomialHeapPQueue::~BinomialHeapPQueue() {}

string BinomialHeapPQueue::peek() { 

  return heapRoots[findIndexOfMinimum()]->value;
}

string BinomialHeapPQueue::extractMin() {



  int index = findIndexOfMinimum();
  node* minHeap = heapRoots[index];
  // take it out of the list of heaps
  heapRoots[index] = NULL;
  BinomialHeapPQueue remainingChildrenQueue;

  // debug
  int removedNodeCount = countActualElemets(minHeap);
  int count = countActualElemets();
  // debug

  for (int i = 0; i < minHeap->children.size(); i++) {
    node* childRoot = minHeap->children[i];
    int order = childRoot->children.size();
    //pad the list of heaps
    if (i != order +1) {
      remainingChildrenQueue.heapRoots.
        insert(remainingChildrenQueue.heapRoots.end(), order - i + 1, NULL);
    }
    remainingChildrenQueue.heapRoots[order] = childRoot;
  }
  // debug
  int remainChildCount = remainingChildrenQueue.countActualElemets();  
  //debug_x = true;

  BinomialHeapPQueue* tmpHeap = merge(this, &remainingChildrenQueue);
 
  // debug
  int tmpHeapCount = tmpHeap->countActualElemets();

  copyHeapRoots(*tmpHeap, *this);
  tmpHeap->heapRoots.clear();  //all the allocated memory is in this.heapRoots now
  delete tmpHeap;

  string value = minHeap->value;
  delete minHeap;
  logSize--;
  return value;
}

void BinomialHeapPQueue::enqueue(string elem) {
  node* singletonHeap = new node;
  singletonHeap->value = elem;
  BinomialHeapPQueue singletonQueue;
  singletonQueue.heapRoots.push_back(singletonHeap);

  BinomialHeapPQueue* tmpHeap = merge(this, &singletonQueue);
  copyHeapRoots(*tmpHeap, *this);
  tmpHeap->heapRoots.clear();  //all the allocated memory is in this.heapRoots now
  delete tmpHeap;
  logSize++;
}

BinomialHeapPQueue *BinomialHeapPQueue::merge(BinomialHeapPQueue *one, BinomialHeapPQueue *two) {
  BinomialHeapPQueue* newQueue = new BinomialHeapPQueue(one->size() + two->size());

  vector<node*> smallerHeapList = (one->size() <= two->size()) ? one->heapRoots : two->heapRoots;
  vector<node*> largerHeapList  = (one->size()  > two->size()) ? one->heapRoots : two->heapRoots;
    
  int newQueueActCount = 0;
  int smallerLargerC = 0;

  node* carry = NULL;
  for (int i = 0; i < smallerHeapList.size(); i++) {
    node* tmpCarry;

    //debug/////////////////////////////
    int smallerC = 0;
    int largerC = 0;
    int sumC1 = 0;
    int carryC1 = 0;
    int sumC2 = 0;
    int carryC2 = 0;


    node* smaller = smallerHeapList[i];
    node* larger = largerHeapList[i];
    if (debug_x) {
      smallerC = countActualElemets(smaller);
      largerC = countActualElemets(larger);
    }
    //debug; ///////////////////

    node* heapSum = add(smallerHeapList[i], carry, tmpCarry, i);


    if (debug_x) {//////////////////
      sumC1 = countActualElemets(heapSum);
      carryC1 = countActualElemets(carry);
    }
    //////////////////////

    heapSum = add(heapSum, largerHeapList[i], carry, i);

    if (debug_x) {
      sumC2 = countActualElemets(heapSum);
      carryC2 = countActualElemets(carry);
    }

    int s2c2 = sumC2 + carryC2;
    smallerLargerC = smallerC + largerC;

    if (tmpCarry)
      carry = tmpCarry;

    newQueue->heapRoots.push_back(heapSum);
  }

  //debug 
  if (debug_x)
    newQueueActCount = newQueue->countActualElemets();

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

int BinomialHeapPQueue::findIndexOfMinimum() {
  int minIndex = findIndexOfFirstNonNull();
  string minString = heapRoots[minIndex]->value;
  for (int i = minIndex; i < heapRoots.size(); i++) {
    if (heapRoots[i]) {
      if (heapRoots[i]->value < minString) {
        minIndex = i;
        minString = heapRoots[i]->value;
      }
    }
  }
  return minIndex;
}

int BinomialHeapPQueue::findIndexOfFirstNonNull() {
  for (int i = 0; i < heapRoots.size(); i++) {
    if (heapRoots[i])
      return i;
  }
  return -1;
}

void BinomialHeapPQueue::copyHeapRoots(BinomialHeapPQueue& src, BinomialHeapPQueue& dest) {
  dest.heapRoots.clear();
  dest.heapRoots.insert(dest.heapRoots.begin(), src.heapRoots.begin(), src.heapRoots.end());
}

int BinomialHeapPQueue::countActualElemets() {
  if (!debug_x) return 0;
  int count = 0;
  for (int i = 0; i < heapRoots.size(); i++) {
    if (heapRoots[i])
      count += countActualElemets(heapRoots[i]);
  }
  return count;
}

int BinomialHeapPQueue::countActualElemets(node* node) {
  if (!debug_x) return 0;
  if (!node) {
    return 0;
  }

  int count = 1;
  for (int i = 0; i < node->children.size(); i++) {
    count += countActualElemets(node->children[i]);
  }
  return count;
}

int mainX() {
  BinomialHeapPQueue queue;

  queue.enqueue("aaaa");
  queue.enqueue("bbbb");
  queue.enqueue("cccc");
  queue.enqueue("dddd");
  queue.enqueue("eeee");
  queue.enqueue("ffff");
  queue.enqueue("gggg");
  queue.enqueue("hhhh");

  cout << "actual: " << IntegerToString(queue.countActualElemets()) << endl;

  string val = queue.extractMin();
  val = queue.extractMin();
  val = queue.extractMin();
  val = queue.extractMin();
  val = queue.extractMin();
  val = queue.extractMin();
  val = queue.extractMin();
  val = queue.extractMin();

  cout << "actual: " << IntegerToString(queue.countActualElemets()) << endl;


  return 0;
}