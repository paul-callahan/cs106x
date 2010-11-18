/*
* File: encoding.h
* Last modified on Wed May 24 21:32:50 PDT 2006 by jzelenski
* ----------------------------------------------------------
* Defines a public interface for the Encoding class.
* An Encoding object impelements compression/decompression.
*
*/
#ifndef _encoding_h
#define _encoding_h

#include "genlib.h"
#include "bstream.h"
#include "debugstream.h"
#include <map>

const int PSEUDO_EOF = 256;

/*
* Class: Encoding
* ---------------
* Defines object for managing an encoding.
*/
class Encoding {

public:

  /*
  * Constructor: Encoding
  * Usage: Encoding encoding;
  * ------------------------
  * Initializes a new empty encoding.  There are no entries
  * mapping characters to bit patterns or vice versa. 
  */
  Encoding() {} ;


  /*
  * Eestructor: ~Encoding
  * Usage: delete ep;
  * -----------------
  * Frees all storage associated with an encoding.
  */
  ~Encoding() { delete mEncodingTableRoot; };


  /*
  * Member function: compress
  * Usage: encoding.compress(in, out);
  * ----------------------------------
  * Reads the contents of the input stream and writes a compressed
  * version to the output stream. The ibstream is assumed to be
  * open and ready for reading, the obstream open and ready for writing.
  * The boolean result indicates success or failure.
  */
  bool compress(ibstream &inStream, dobstream &outStream);


  /*
  * Member function: deccompress
  * Usage: encoding.decompress(in, out);
  * -------------------------------------
  * Reads the compressed input stream and writes a uncompressed
  * version to the output stream. The ibstream is assumed to be
  * open and ready for reading, the obstream open and ready for writing.
  * The boolean result indicates success or failure.
  */
  bool decompress(ibstream &inStream, dobstream &outStream);


  static bool match(ibstream& streamOne, ibstream& streamTwo);

  static void selfTest(string sourceFile);

private:

  struct node {
    node() : byte(-1), left(NULL), right(NULL) {};
    node(int byte) : byte(byte), left(NULL), right(NULL) {};
    ~node() { delete left; delete right; };
    int byte;
    node* left;
    node* right;
    bool isLeaf() { return left == NULL && right == NULL; };
  };

  node* mEncodingTableRoot;
  map<int, string> mByteLookup;

  /*
  * Private Member function: 
  * -------------------------------------
  *
  *
  */
  void writeEncodingTable(dobstream& output);

  /*
  * Private Member function: 
  * -------------------------------------
  *
  *
  */
  void writeEncodingTable(node& node, dobstream& output);

  /*
  * Private Member function: 
  * -------------------------------------
  *
  *
  */
  void readEncodingTable(ibstream& input);

  /*
  * Private Member function: 
  * -------------------------------------
  *
  *
  */
  node* readEncodingNode(ibstream& input);

  /*
  * Private Member function: 
  * -------------------------------------
  *
  *
  */
  void writeCompressedFileBody(ibstream& input, dobstream& output);

  /*
  * Private Member function: 
  * -------------------------------------
  *
  *
  */
  void writeDecompressedFile(ibstream& input, dobstream& output);


  /*
  * Private Member function: 
  * -------------------------------------
  *
  *
  */
  string getBitPatternForByte(int byte);


  /*
  * Private Member function: 
  * -------------------------------------
  *
  *
  */
  void checkStreamState(ios_base& stream);

  /*
  * Private Member function: 
  * -------------------------------------
  *
  *
  */
  void createLookupPaths(node& node, string bitPattern);

  /*
  * Member function: buildEncodingForInput
  * Usage: encoding.buildEncodingForInput(in);
  * -------------------------------------
  * Makes a pass over the input, records the "byte" alphabet and the 
  * frequencies for the alphabet.  Then builds the encoding table
  * for the alphabet.
  */
  void buildEncodingForInput(ibstream& input);

  /*
  * Private Member function: 
  * -------------------------------------
  *
  *
  */
  void buildHuffmanTree(map<int, int>& frequencyMap);

  

  /* all for you to decide! */

};

#endif
