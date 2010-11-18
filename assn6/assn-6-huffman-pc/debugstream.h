/*
* File: encoding.h
* Last modified on Wed May 24 21:32:50 PDT 2006 by jzelenski
* ----------------------------------------------------------
* Defines a suggested public interface for the Encoding class.
* An Encoding object impelements compression/decompression.
* Your class is not required to follow this interface exactly, 
* but it makes a reasonable starting point.
*/
#ifndef _debugstream_h
#define _debugstream_h


#include "bstream.h"

class dobstream : public obstream {

public:
  dobstream(bool debug) : debug(debug) {};
  dobstream() : debug(false) {};
  void open(string filename);
  void close();
  void writebit(int bit);
  void writebits(string bits, int origByte);
  void put(int byte);
  void writeAsShort(int byte);
  void setDebug(bool db) { debug = db; };
  dobstream& operator<< (short& val);

private:
  
  ofstream humanReadableOstream;
  obstream binaryOStream;
  bool debug;
  int written;
  
};


#endif
