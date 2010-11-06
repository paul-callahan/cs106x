/**
* File: pqueue-linked-list.cpp
* ---------------------
* sorted linked list-backed priority queue subclass.
*
* Author: Paul Callahan
* For: CS106X Assignment 5, Priority Queues
* Date: Nov 5th, 2010
*/
#include "pqueue-linked-list.h"

// constructor is inlined.
//LinkedListPQueue::LinkedListPQueue() {}

LinkedListPQueue::~LinkedListPQueue() {
  node* currNode = head;
  while (currNode) {
    node* me = currNode;
    currNode = currNode->next;
    delete me;
  }
}

void LinkedListPQueue::checkEmpty() {
   if (isEmpty()) {
    Error("Attempt to peek/extract on an empty priority queue");
  }
}

string LinkedListPQueue::peek() { 
  checkEmpty();
  return head->value;
}

string LinkedListPQueue::extractMin() {
  checkEmpty();
  node* minNode = head;
  head = head->next;
  if (head != NULL)  {
    head->prev = NULL;
  }
  string value = minNode->value;
  delete minNode;
  logSize--;
  return value;
}

void LinkedListPQueue::enqueue(string elem) {

  // cases
  // 1. head is null, so new node because the head
  // 2. list has one node
  // 2a.  new elem value is greater than head value, so new node becomes tail
  // 2b.  new elem value is less than head value, so new node becomes head
  // 3. new elem value is greater than all node values, this is same as 2a.
  // 4.  new elem value goes in the middle someplace.
  // 5. new elem value is less than all node values, this is the same as 2b

  logSize++;

  node* newNode = new node;
  newNode->value = elem;
  newNode->next = NULL;
  newNode->prev = NULL;

  node* currNode = head;
  node* nodeBeforeCurr = NULL; // so I don't have to keep a tail ptr

  while (currNode) {
    // we have gone one past
    if (currNode->value > elem) {
      // head is bigger than elem
      if (currNode == head) {
        head = newNode;
      } else {
        currNode->prev->next = newNode;
      }
      newNode->next = currNode; // ok if currNode is head
      newNode->prev = currNode->prev; // ok if currNode is head/ currNode->prev = null.
      currNode->prev = newNode;
      return;
    }
    nodeBeforeCurr = currNode;
    currNode = currNode->next;
  }
  
  // ran off the end.

  // well, there was really no end.  list was empty
  if (head == NULL) {
    head = newNode;
  }

  // or newNode becomes the new last node.
  if (nodeBeforeCurr != NULL) {
    nodeBeforeCurr->next = newNode;
    newNode->prev = nodeBeforeCurr;
  }


  /*
  while (currNode != NULL && currNode->value < elem) {
    currNode = currNode->next;
  }
  //if currNode is head, then  currNode-> prev would be null.
  newNode->next = currNode;



  if (currNode != NULL) {
    if (currNode->prev != NULL) {
       currNode->prev->next = newNode;
       newNode->prev = currNode->prev;
    } else {
      head = newNode;
    }
  } else {  // currnode is null
    head = newNode;
  }

  */
  
}

void LinkedListPQueue::appendAndAdvance(node*& currTail, node*& sourceNode) {
  sourceNode->prev = currTail;
  if (currTail != NULL)
    currTail->next = sourceNode;
  currTail = sourceNode;
  // currTail->next = NULL;
  sourceNode = sourceNode->next;  
}

LinkedListPQueue *LinkedListPQueue::merge(LinkedListPQueue *one, LinkedListPQueue *two) {
  LinkedListPQueue* newQueue = new LinkedListPQueue;
  newQueue->logSize = one->size() + two->size();
  node* currOne = one->head;
  node* currTwo = two->head;
  node* currNew = newQueue->head;

  while(currOne && currTwo) {
    if (currOne->value < currTwo->value) {
      appendAndAdvance(currNew, currOne);
    } else {
      appendAndAdvance(currNew, currTwo);
    }
    if (!newQueue->head) {
      newQueue->head = currNew;
    }
  }

  while (currOne) {
    appendAndAdvance(currNew, currOne);
  }
  while (currTwo) {
    appendAndAdvance(currNew, currTwo);
  }
  // don't taze my nodes, bro
  // one and two's head still points at nodes we purloined for the new node.
  // if one or two's destructor gets called, that's going to delete at least
  // some of the new nodes.  To prevent this pwnage, we null out the head ptr
  // of one and two.
  one->head = NULL;
  two->head = NULL;

  // currNew is the tail, make sure it is not pointing off anywhere.
  currNew->next = NULL;

	return newQueue;
}

