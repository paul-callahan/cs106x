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

/*
 * Constructor: 
 * -----------------------------
 *  see declaration
 */
BinomialHeapPQueue::BinomialHeapPQueue(){ 
  init();
}

/*
 * Constructor: 
 * -----------------------------
 *  see declaration
 */
BinomialHeapPQueue::BinomialHeapPQueue(int size){ 
  init();
  logSize = size;
  heapRoots.reserve(size + 1);
}

/*
 * Constructor: 
 * -----------------------------
 *  see declaration
 */
void BinomialHeapPQueue::init(){ 
  tmpQueue = NULL;
  singletonQueue = NULL;
  remainingChildrenQueue = NULL;
}

/*
 * Destructor: 
 * -----------------------------
 *  see declaration
 */

BinomialHeapPQueue::~BinomialHeapPQueue() {
  int deleted = deleteElements();
  if (tmpQueue)
    delete tmpQueue;
  if (singletonQueue)
    delete singletonQueue;
  if (remainingChildrenQueue)
    delete remainingChildrenQueue;
  //cout << IntegerToString(deleted) << " elements deleted." << endl;
}


/*
 * Method: peek
 * -----------------------------
 *  see declaration
 */
string BinomialHeapPQueue::peek() { 
  return heapRoots[findIndexOfMinimum()]->value;
}

/*
 * Method: extractMin
 * -----------------------------
 *  see declaration
 */
string BinomialHeapPQueue::extractMin() {
  int index = findIndexOfMinimum();
  node* minHeap = heapRoots[index];
  // take it out of the list of heaps
  heapRoots[index] = NULL;

  //lazy construct this temp queue.
  if (!remainingChildrenQueue) 
    remainingChildrenQueue = new BinomialHeapPQueue;

  // take the children from the min heap and put them in their 
  //own queue
  for (int i = 0; i < minHeap->children.size(); i++) {
    node* childRoot = minHeap->children[i];
    int order = childRoot->children.size();
    //pad the list of heaps with nulls
    if (i != order +1) {
      remainingChildrenQueue->heapRoots.
        insert(remainingChildrenQueue->heapRoots.end(), order - i + 1, NULL);
    }
    remainingChildrenQueue->heapRoots[order] = childRoot;
  }

  merge(remainingChildrenQueue);

  string value = minHeap->value;
  delete minHeap;
  logSize--;
  return value;
}

/*
 * Method: enqueue
 * -----------------------------
 *  see declaration
 */
void BinomialHeapPQueue::enqueue(string elem) {
  //only place we construct new nodes
  node* singletonHeap = new node;
  singletonHeap->value = elem;

  //lazy construct singleton queue
  if (!singletonQueue)
    singletonQueue = new BinomialHeapPQueue();
  singletonQueue->heapRoots.push_back(singletonHeap);
  merge(singletonQueue); 
  singletonQueue->heapRoots.clear();
  logSize++;
}

/*
 * Method: merge
 * -----------------------------
 *  see declaration
 */
void BinomialHeapPQueue::merge(BinomialHeapPQueue *other) {
  if (!tmpQueue)
    tmpQueue = new BinomialHeapPQueue();

  merge(this, other, tmpQueue);
  copyHeapRoots(*tmpQueue, *this);
  tmpQueue->heapRoots.clear();  //all the allocated memory is in this.heapRoots now
}

/*
 * Method: merge
 * -----------------------------
 *  see declaration
 */
BinomialHeapPQueue *BinomialHeapPQueue::merge(BinomialHeapPQueue *one, BinomialHeapPQueue *two) {
  BinomialHeapPQueue* newQueue = new BinomialHeapPQueue(one->size() + two->size());
  return merge(one, two, newQueue);
}

/*
 * Method: merge
 * -----------------------------
 *  see declaration
 */
BinomialHeapPQueue *BinomialHeapPQueue::merge(BinomialHeapPQueue *one, BinomialHeapPQueue *two,
                                              BinomialHeapPQueue* newQueue) {
  vector<node*> smallerHeapList = (one->size() <= two->size()) ? one->heapRoots : two->heapRoots;
  vector<node*> largerHeapList  = (one->size()  > two->size()) ? one->heapRoots : two->heapRoots;


  node* carry = NULL;
  // "add" up all the heaps in the smaller sized list w/ the bigger list
  for (int i = 0; i < smallerHeapList.size(); i++) {
    node* tmpCarry;

    node* heapSum = add(smallerHeapList[i], carry, tmpCarry);
    heapSum = add(heapSum, largerHeapList[i], carry);

    if (tmpCarry)
      carry = tmpCarry;

    newQueue->heapRoots.push_back(heapSum);
  }

  // "add" any left over carry with the balance of the items in the bigger list
  for (int i = smallerHeapList.size(); i < largerHeapList.size(); i++) {
    node* heapSum = add(carry, largerHeapList[i], carry);
    newQueue->heapRoots.push_back(heapSum);
  }

  //one last carry "bit"
  if (carry) {
    newQueue->heapRoots.push_back(carry);
  }
  // we moved all the nodes over, so top referencing them here
  one->heapRoots.clear();
  two->heapRoots.clear();

  return newQueue;
}

/*
 * Method: add
 * -----------------------------
 *  see declaration
 */
BinomialHeapPQueue::node* BinomialHeapPQueue::add(node* operandOne, node* operandTwo, node*& carryOut) {
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

/*
 * Method: findIndexOfMinimum
 * -----------------------------
 *  see declaration
 */
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

/*
 * Method: findIndexOfFirstNonNull
 * -----------------------------
 *  see declaration
 */
int BinomialHeapPQueue::findIndexOfFirstNonNull() {
  for (int i = 0; i < heapRoots.size(); i++) {
    if (heapRoots[i])
      return i;
  }
  return -1;
}

/*
 * Method: copyHeapRoots
 * -----------------------------
 *  see declaration
 */
void BinomialHeapPQueue::copyHeapRoots(BinomialHeapPQueue& src, BinomialHeapPQueue& dest) {
  dest.heapRoots.resize(src.heapRoots.size());
  copy(src.heapRoots.begin(), src.heapRoots.end(), dest.heapRoots.begin());
  // is this slower?
  //dest.heapRoots.insert(dest.heapRoots.begin(), src.heapRoots.begin(), src.heapRoots.end());
}

/*
 * Method: deleteElements
 * -----------------------------
 *  see declaration
 */
int BinomialHeapPQueue::deleteElements() {
  int count = 0;
  for (int i = 0; i < heapRoots.size(); i++) {
    if (heapRoots[i])
      count += deleteElements(heapRoots[i]);
  }
  return count;
}

/*
 * Method: deleteElements
 * -----------------------------
 *  see declaration
 */
int BinomialHeapPQueue::deleteElements(node* node) {
  if (!node) {
    return 0;
  }
  int count = 1;
  for (int i = 0; i < node->children.size(); i++) {
    count += deleteElements(node->children[i]);
  }
  delete node;
  return count;
}

