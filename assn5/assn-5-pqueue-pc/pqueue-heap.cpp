#include "pqueue-heap.h"

void HeapPQueue::init(int size) {
  heapAllocated = size;
  heap = new string[size];
}


HeapPQueue::~HeapPQueue() {
  delete [] heap;
}

string HeapPQueue::peek() { 
  return heap[0];
}

string HeapPQueue::extractMin() {
  string value = peek();
  heap[0] = heap[logSize -1];
  logSize--;
  siftDown(0, logSize);
  return value;
}

void HeapPQueue::enqueue(string elem) {
  checkAlloc();
  heap[logSize] = elem;
  siftUp(logSize);
  logSize++;
}

HeapPQueue *HeapPQueue::merge(HeapPQueue *one, HeapPQueue *two) {
  HeapPQueue* newQueue = new HeapPQueue(one->size() + two->size());

  memcpy(newQueue->heap, one->heap, one->logSize);
  memcpy(newQueue->heap + one->logSize, two->heap, two->logSize);
  newQueue->heapify();
  return newQueue;
}

void HeapPQueue::swap(int posx, int posy) {
  string tmp = heap[posx];
  heap[posx] = heap[posy];
  heap[posy] = tmp;
}


void HeapPQueue::siftDown(int nodeToSift, int length) {
  while (nodeToSift * 2 -1 < length) {
    int leastChild = 2 * nodeToSift + 1;//nodeToSift * 2;
    //find the least child
    if (leastChild + 1 <= length &&
      heap[leastChild + 1] < heap[leastChild]) {
        leastChild += 1;
    }

    if (heap[leastChild] < heap[nodeToSift]) {
      swap(leastChild, nodeToSift);
      nodeToSift = leastChild;
    } else {
      break;
    }
  }
}

void HeapPQueue::siftUp(int nodeToSift) {
  //base case
  if (nodeToSift == 0)
    return;
  int parentNode = (nodeToSift - 1) / 2;
  if (heap[parentNode] > heap[nodeToSift]) {
    swap(parentNode, nodeToSift);
    siftUp(parentNode);
  }

}

void HeapPQueue::heapify() {
  for (int i = logSize / 2; i > 0; i--) {
    siftDown(i, logSize);
  }
}

void HeapPQueue::checkAlloc() {
  if (heapAllocated == logSize) {
    string* oldHeap = heap;
    heap = new string[heapAllocated*2];
    memcpy(heap, oldHeap, heapAllocated);
    heapAllocated *=2;
  }
}

