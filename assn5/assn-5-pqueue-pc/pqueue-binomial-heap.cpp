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


BinomialHeapPQueue::BinomialHeapPQueue(int size){ 
  logSize = size;
  heapRoots.reserve(size + 1);
}

BinomialHeapPQueue::~BinomialHeapPQueue() {
  int deleted = deleteElements();
  //cout << IntegerToString(deleted) << " elements deleted." << endl;
}



string BinomialHeapPQueue::peek() { 
  return heapRoots[findIndexOfMinimum()]->value;
}

string BinomialHeapPQueue::extractMin() {
  int index = findIndexOfMinimum();
  node* minHeap = heapRoots[index];
  // take it out of the list of heaps
  heapRoots[index] = NULL;
  BinomialHeapPQueue remainingChildrenQueue;

  // take the children from the min heap and put them in their 
  //own queue
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

  BinomialHeapPQueue* tmpHeap = merge(this, &remainingChildrenQueue);
  copyHeapRoots(*tmpHeap, *this);
  tmpHeap->heapRoots.clear();  //all the allocated memory is in this.heapRoots now
  delete tmpHeap;

  string value = minHeap->value;
  delete minHeap;
  logSize--;
  return value;
}

void BinomialHeapPQueue::enqueue(string elem) {
  //node* singletonHeap = new node;
  //singletonHeap->value = elem;
  //BinomialHeapPQueue singletonQueue;
  //singletonQueue.heapRoots.push_back(singletonHeap);
  
  BinomialHeapPQueue singletonQueue = getSingletonQueue(elem);

  BinomialHeapPQueue* tmpHeap = merge(this, &singletonQueue);
  copyHeapRoots(*tmpHeap, *this);
  tmpHeap->heapRoots.clear();  //all the allocated memory is in this.heapRoots now
  delete tmpHeap;

  //merge(&singletonQueue); 

  logSize++;
}

BinomialHeapPQueue& BinomialHeapPQueue::getSingletonQueue(string elem) {
  static BinomialHeapPQueue singletonQueue;
  singletonQueue.heapRoots.clear();
  singletonQueue.heapRoots.reserve(1);
  node* singletonHeap = new node;
  singletonHeap->value = elem;
  singletonQueue.heapRoots.push_back(singletonHeap);
  return singletonQueue;
}

void BinomialHeapPQueue::merge(BinomialHeapPQueue *other) {
  static BinomialHeapPQueue transferHeap;
  transferHeap.heapRoots.reserve(other->size() + size());
  merge(this, other, &transferHeap);
  copyHeapRoots(transferHeap, *this);
}

BinomialHeapPQueue *BinomialHeapPQueue::merge(BinomialHeapPQueue *one, BinomialHeapPQueue *two) {
  BinomialHeapPQueue* newQueue = new BinomialHeapPQueue(one->size() + two->size());
  return merge(one, two, newQueue);
}

BinomialHeapPQueue *BinomialHeapPQueue::merge(BinomialHeapPQueue *one, BinomialHeapPQueue *two,
                                              BinomialHeapPQueue* newQueue) {

  vector<node*> smallerHeapList = (one->size() <= two->size()) ? one->heapRoots : two->heapRoots;
  vector<node*> largerHeapList  = (one->size()  > two->size()) ? one->heapRoots : two->heapRoots;
  

  node* carry = NULL;
  for (int i = 0; i < smallerHeapList.size(); i++) {
    node* tmpCarry;

    node* heapSum = add(smallerHeapList[i], carry, tmpCarry, i);
    heapSum = add(heapSum, largerHeapList[i], carry, i);

    if (tmpCarry)
      carry = tmpCarry;

    newQueue->heapRoots.push_back(heapSum);
  }

  for (int i = smallerHeapList.size(); i < largerHeapList.size(); i++) {
    node* heapSum = add(carry, largerHeapList[i], carry, i);
    newQueue->heapRoots.push_back(heapSum);
  }

  if (carry) {
    newQueue->heapRoots.push_back(carry);
  }
  // we moved all the nodes over, so top referencing them here
  one->heapRoots.clear();
  two->heapRoots.clear();

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
  dest.heapRoots.resize(src.heapRoots.size());
  copy(src.heapRoots.begin(), src.heapRoots.end(), dest.heapRoots.begin());
  // is this slow?
  //dest.heapRoots.insert(dest.heapRoots.begin(), src.heapRoots.begin(), src.heapRoots.end());
}

int BinomialHeapPQueue::deleteElements() {
  int count = 0;
  for (int i = 0; i < heapRoots.size(); i++) {
    if (heapRoots[i])
      count += deleteElements(heapRoots[i]);
  }
  return count;
}

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


  string val = queue.extractMin();
  val = queue.extractMin();
  val = queue.extractMin();
  val = queue.extractMin();
  val = queue.extractMin();
  val = queue.extractMin();
  val = queue.extractMin();
  val = queue.extractMin();




  return 0;
}