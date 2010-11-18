
#include "debugstream.h"
#include "strutils.h"

void dobstream::open(string filename) {
  obstream::open(filename.c_str());
  if (debug) {
    string debugfile = filename + "_debug.html";
    humanReadableOstream.open(debugfile.c_str());
    humanReadableOstream << "<html><body><table border=1><tr>" << endl;
  }
}

void dobstream::close() {
  obstream::close();
  if (debug) {
    humanReadableOstream << "</table></body></html>" << endl;
    humanReadableOstream.close();
  }
}


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

