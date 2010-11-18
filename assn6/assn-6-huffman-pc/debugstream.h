/*
* File: debugstream.h
* ----------------------------------------------------------
* Defines the public interface for the dobstream class.  
* The class writes up to two files at once - on a binary output 
* stream, and the other a human-readable HTML output stream for
* debugging purposes.
* It subclasses obstream.
*
* Author: Paul Callahan 
* Grader: Alex Quach
* Assignment #6, Huffman Encoding
*/
#ifndef _debugstream_h
#define _debugstream_h


#include "bstream.h"

/*
* Class: dobstream
* ------------------------
* The class writes up to two files at once - on a binary output 
* stream, and the other a human-readable HTML output stream for
* debugging purposes.
* It subclasses obstream.
*
* Author: Paul Callahan 
* Grader: Alex Quach
* Assignment #6, Huffman Encoding
*/

class dobstream : public obstream {

public:
  /*
  * Constructor: 
  * Usage: dobstream(true)
  * ----------------------------------
  * Constructs a new dobstream.  If debug is true, then 
  * the HTML stream will be used as well.
  */
  dobstream(bool debug) : debug(debug) {};

  /*
  * Constructor: 
  * Usage: dobstream()
  * ----------------------------------
  * Constructs a new dobstream.  Debug is set to false.
  * no HTML debugging output.
  */
  dobstream() : debug(false) {};

  /*
  * Member function: open
  * Usage: dobstream.open
  * ----------------------------------
  * Opens binary stream.  
  * If debug is true, opens html stream with filename + ".html"
  */
  void open(string filename);

  /*
  * Member function: close
  * Usage: dobstream.close
  * ----------------------------------
  * closes binary stream.
  * If debug is true, closes HTML stream, too.
  */
  void close();

  /*
  * Member function: writebit
  * Usage: dobstream.writebit(1)
  * ----------------------------------
  * writes a bit to the binary stream
  * If debug is true, a table cell with the bit is added to the HTML stream
  */
  void writebit(int bit);

  /*
  * Member function: writebits
  * Usage: dobstream.writebits(bits, byte)
  * ----------------------------------
  * writes the given bit pattern to the binary stream.
  * If debug is true, the string is written to an HTML table cell
  */
  void writebits(string bits, int origByte);

  /*
  * Member function: put
  * Usage: dobstream.put(somebyte)
  * ----------------------------------
  * Puts a single byte to the binary stream.
  * If debug is true, the byte is writen to an HTML table cell
  */
  void put(int byte);

  /*
  * Member function: writeAsShort
  * Usage: dobstream.writeAsShort(somebyte)
  * ----------------------------------
  * Writes byte as 2byte short.  To accomodate the 9-bit EOF char.
  * If debug is true, it is also written to the HTML stream.
  * Note: this is the only way I could get this to work. << failed for some
  * reason.
  */
  void writeAsShort(int byte);



private:
  // output stream to write optional HTML to.
  ofstream humanReadableOstream;

  // if true, we write to HTML in parallel
  bool debug;

  // internal counter for formatting HTML table
  int written;

};


#endif
