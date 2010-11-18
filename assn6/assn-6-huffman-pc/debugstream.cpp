/* File: debugstream.cpp
* -----------------
* dobstream class implementation.
* a binary stream with a debugging output stream in HTML.
*
* Author: Paul Callahan 
* Grader: Alex Quach
* Assignment #6, Huffman Encoding
*/

#include "debugstream.h"
#include "strutils.h"

/*
* Member function: open
* Usage: dobstream.open
* ----------------------------------
* Opens binary stream.  
* If debug is true, opens html stream with filename + ".html"
*/
void dobstream::open(string filename) {
  obstream::open(filename.c_str());
  if (debug) {
    string debugfile = filename + "_debug.html";
    humanReadableOstream.open(debugfile.c_str());
    humanReadableOstream << "<html><body><table border=1><tr>" << endl;
  }
}

/*
* Member function: close
* Usage: dobstream.close
* ----------------------------------
* closes binary stream.
* If debug is true, closes HTML stream, too.
*/
void dobstream::close() {
  obstream::close();
  if (debug) {
    humanReadableOstream << "</table></body></html>" << endl;
    humanReadableOstream.close();
  }
}

/*
* Member function: put
* Usage: dobstream.put(somebyte)
* ----------------------------------
* Puts a single byte to the binary stream.
* If debug is true, the byte is writen to an HTML table cell
*/
void dobstream::put(int byte) {
  written++;
  obstream::put(byte);
  if (debug) {
    humanReadableOstream << "<td>" << IntegerToString(byte) << " / " << (char)byte << "</td>";
    if (written % 20 == 0) {
      humanReadableOstream << endl << "</tr>" << endl;
    }
  }
}

/*
* Member function: writebit
* Usage: dobstream.writebit(1)
* ----------------------------------
* writes a bit to the binary stream
* If debug is true, a table cell with the bit is added to the HTML stream
*/
void dobstream::writebit(int bit) {
  written++;
  obstream::writebit(bit);
  if (debug) {
    humanReadableOstream << "<td>" << IntegerToString(bit) << "</td>";
    if (written % 20 == 0) {
      humanReadableOstream << endl << "</tr>" << endl;
    }
  }
}

/*
* Member function: writebits
* Usage: dobstream.writebits(bits, byte)
* ----------------------------------
* writes the given bit pattern to the binary stream.
* If debug is true, the string is written to an HTML table cell
*/
void dobstream::writebits(string bitPattern, int originalByte) {
  written++;
  for (int i = 0; i < bitPattern.size(); i++) {
    obstream::writebit(bitPattern[i] == '1' ? 1 : 0);
  }
  if (debug) {
    humanReadableOstream << "<td>" << bitPattern << "/" << (char) originalByte << "</td>";
    if (written % 20 == 0) {
      humanReadableOstream << endl << "</tr>" << endl;
    }
  }
}

/*
* Member function: writeAsShort
* Usage: dobstream.writeAsShort(somebyte)
* ----------------------------------
* Writes byte as 2byte short.  To accomodate the 9-bit EOF char.
* If debug is true, it is also written to the HTML stream.
* Note: this is the only way I could get this to work. << failed for some
* reason.
*/
void dobstream::writeAsShort(int byte) {
  written++;
  obstream::operator <<((short)byte);
  if (debug) {
    humanReadableOstream << "<td>" << IntegerToString(byte) << " / " << (char)byte << "-s</td>";
    if (written % 20 == 0) {
      humanReadableOstream << endl << "</tr>" << endl;
    }
  }  
}

