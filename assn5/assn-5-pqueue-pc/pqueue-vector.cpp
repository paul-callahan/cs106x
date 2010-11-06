/**
* File: pqueue-vector.cpp
* ---------------------
* Vector-backed priority queue subclass.
*
* Author: Paul Callahan
* For: CS106X Assignment 5, Priority Queues
* Date: Nov 5th, 2010
*/

#include "pqueue-vector.h"

VectorPQueue::VectorPQueue() {}
VectorPQueue::~VectorPQueue() {}


int VectorPQueue::findIndexOfMin() {
  if (isEmpty())
    Error("Attempting to find min on an empty Priority Queue");
  int indexOfMin = 0;
  string minString = strings[0];
  for (int i = 0; i < strings.size(); i++) {
    if (strings[i] < minString) {
      minString = strings[i];
      indexOfMin = i;
    }
  }
  return indexOfMin;
}

string VectorPQueue::peek() { 
  return strings[findIndexOfMin()];
}


string VectorPQueue::extractMin() {
  int indexOfMin = findIndexOfMin();
  string minString = strings[indexOfMin];
  strings.erase(strings.begin() + indexOfMin);
  logSize--;
  return minString;
}

void VectorPQueue::enqueue(string elem) {
  logSize++;
  strings.push_back(elem);
}

VectorPQueue *VectorPQueue::merge(VectorPQueue *one, VectorPQueue *two) {
  VectorPQueue* newQueue = new VectorPQueue();
  newQueue->strings.insert( newQueue->strings.end(), one->strings.begin(), one->strings.end() );
  newQueue->strings.insert( newQueue->strings.end(), two->strings.begin(), two->strings.end() );
  newQueue->logSize = one->size() + two->size();
  return newQueue;
}



