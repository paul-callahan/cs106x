/**
* pqueue-heap.h
* priority-queue
*
* Created by Jerry Cain on 10/23/10.
* Copyright 2010 Stanford University. All rights reserved.
*/

#ifndef __binary_heap_pqueue_cs106__
#define __binary_heap_pqueue_cs106__

#include "pqueue.h"

class HeapPQueue : public PQueue {
public:
  HeapPQueue()  { init(1000); };
  HeapPQueue(int size) { init(size); };
  ~HeapPQueue();

  static HeapPQueue *merge(HeapPQueue *one, HeapPQueue *two);

  void enqueue(string elem);
  string extractMin();
  string peek();

private:
  int heapAllocated;
  string* heap;

  /*
  * Function: swap
  * -----------------------------
  *  Swaps two positions within the string array.
  */
  void swap(int posx, int posy);

  /*
  * Function: siftDown
  * -----------------------------
  * Pushes the element at "nodeToShift" down the heap tree to its proper 
  * location in the heap.
  */
  void siftDown(int nodeToSift, int length);

  /*
  * method: siftUp
  * -----------------------------
  * Pushes the element at "nodeToShift" up the heap tree to its proper 
  * location in the heap.
  */
  void siftUp(int nodeToSift);

  /*
  * Function: heapify
  * -----------------------------
  * Repeatedly sifts-down the elements in the vector until
  * they obey the heap property.
  */
  void heapify();

  /*
  * Function: checkAlloc
  * -----------------------------
  * Ensures there is enough space in the heap array for enqueuing
  * If more space is needed, it doubles the allocated size of the
  * heap array.
  */
  void checkAlloc();

  /*
  * Function: init
  * -----------------------------
  * creates initial heap array.
  */
  void init(int size);


};

#endif