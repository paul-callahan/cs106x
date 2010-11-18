/*
* File: encoding.h
* ----------------------------------------------------------
* Defines a public interface for the Encoding class.
* An Encoding object impelements compression/decompression.
*
* Author: Paul Callahan 
* Grader: Alex Quach
* Assignment #6, Huffman Encoding
*/
#ifndef _encoding_h
#define _encoding_h

#include "genlib.h"
#include "bstream.h"
#include "debugstream.h"
#include <map>

// 9 bits to represent the end of file
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

  /*
  * Member function: match
  * Usage: Encoding::match(in, out);
  * -------------------------------------
  * Compares to files and attempts to determine if they are the same
  * or different.  Outputs the first byte position where they differ.
  * Returns true if the same or false if different.
  */
  static bool match(ibstream& streamOne, ibstream& streamTwo);

  /*
  * Member function: selfTest
  * Usage: Encoding::selfTest(someFile);
  * -------------------------------------
  * Compresses then decompresses the given file.  Then runs match on 
  * the output file to verify it is the same as the input.  Assumes the
  * file sourceFile exists.
  */
  static bool selfTest(string sourceFile);

private:

  /*
  * Record: node
  * ------------------------
  * Represents a single node in the huffman coding tree.  Left nodes
  * represent a bit path of zero, right nodes a path of one.
  * The leaf nodes contain a byte/character which the bit patterns map
  * to.
  */
  struct node {
    node() : byte(-1), left(NULL), right(NULL) {};
    node(int byte) : byte(byte), left(NULL), right(NULL) {};
    // recursively disposes of the tree.
    ~node() { delete left; delete right; };
    // this is the character/byte
    int byte;
    node* left;
    node* right;
    // checks to see if this node is a leaf/
    bool isLeaf() { return left == NULL && right == NULL; };
  };

  // root node of the encoding table
  node* mEncodingTableRoot;

  // map for quick lookup of bit patterns for chars/byts
  map<int, string> mByteLookup;

  /*
  * Private Member function: writeEncodingTable
  * -------------------------------------
  * Writes the encoding table/tree to the output stream.  
  * The tree is serialized as a pre-order traversal of
  * the encoding tree.  Inner nodes are stored as zeros and
  * leaf nodes are stored as ones with the node's byte/character
  * stored as at least two byes, to accomodate the pseudo EOF character
  */
  void writeEncodingTable(dobstream& output);

  /*
  * Private Member function: writeEncodingTable
  * -------------------------------------
  * Recursive helper for serializing the encoding tree/table.
  * See description of the single-argument version.
  */
  void writeEncodingTable(node& node, dobstream& output);

  /*
  * Private Member function: readEncodingTable
  * -------------------------------------
  * Deserializes the pre-order representation of the encoding tree and
  * assigns it to the mEncodingTableRoot member.
  */
  void readEncodingTable(ibstream& input);

  /*
  * Private Member function: readEncodingNode
  * -------------------------------------
  * Recursive helper for deserializing the encoding tree.
  * See description of the single-argument version.
  */
  node* readEncodingNode(ibstream& input);

  /*
  * Private Member function: writeCompressedFileBody
  * -------------------------------------
  * Writes the compressed file contents to the output stream.
  * Input stream is the uncompressed source file.
  */
  void writeCompressedFileBody(ibstream& input, dobstream& output);

  /*
  * Private Member function: writeDecompressedFile
  * -------------------------------------
  * Writes the decompressed file to the output stream, from the
  * compressed input stream.
  */
  void writeDecompressedFile(ibstream& input, dobstream& output);


  /*
  * Private Member function: getBitPatternForByte
  * -------------------------------------
  *  Looks up the bit pattern for a given byte/character from the
  * byteLookup dictionary.  Throws an Error if there is no bit pattern
  * for the given byte/character.
  */
  string getBitPatternForByte(int byte);


  /*
  * Private Member function: checkStreamState
  * -------------------------------------
  * Validates that the given stream state is good.
  * throws an Error if the stream is in a bad() state.
  */
  void checkStreamState(ios_base& stream);

  /*
  * Private Member function: createLookupPaths
  * -------------------------------------
  * recursively builds bit patterns from root-to-leaf paths in
  * the encoding tree.
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
  * Private Member function: buildHuffmanTree
  * -------------------------------------
  * Builds the encoding tree out of a map of byte frequencies.
  * iterates over the map, puts the bytes into a priority queue with the
  * frequencies as weights, and then runs the huffman algorithm to find
  * the optimal encoding tree.
  */
  void buildHuffmanTree(map<int, int>& frequencyMap);

};

#endif
