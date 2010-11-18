/* File: encoding.cpp
* -----------------
* Encoding class implementation.
*
* Author: Paul Callahan 
* Grader: Alex Quach
* Assignment #6, Huffman Encoding
*/

#include <iostream>
#include "encoding.h"
#include "bstream.h"
#include "pqueue.h"
#include <map>

/*
* Member function: compress
* Usage: encoding.compress(in, out);
* ----------------------------------
* Reads the contents of the input stream and writes a compressed
* version to the output stream. The ibstream is assumed to be
* open and ready for reading, the obstream open and ready for writing.
* The boolean result indicates success or failure.
*/
bool Encoding::compress(ibstream &input, dobstream &output) {
  checkStreamState(input);
  checkStreamState(output);

  buildEncodingForInput(input);
  createLookupPaths(*mEncodingTableRoot, string());
  writeEncodingTable(output);
  writeCompressedFileBody(input, output);
  // failures are thrown as Errors.
  return true;
}

/*
* Member function: deccompress
* Usage: encoding.decompress(in, out);
* -------------------------------------
* Reads the compressed input stream and writes a uncompressed
* version to the output stream. The ibstream is assumed to be
* open and ready for reading, the obstream open and ready for writing.
* The boolean result indicates success or failure.
*/
bool Encoding::decompress(ibstream &input, dobstream &output) {
  checkStreamState(input);
  checkStreamState(output);
  readEncodingTable(input);
  createLookupPaths(*mEncodingTableRoot, string());
  writeDecompressedFile(input, output);
  // failures are thrown as Errors.
  return true;
}

/*
* Private Member function: createLookupPaths
* -------------------------------------
* recursively builds bit patterns from root-to-leaf paths in
* the encoding tree.
*/
void Encoding::createLookupPaths(node& node, string bitPattern) {
  if (node.isLeaf()) {
    mByteLookup.insert(pair<int, string>(node.byte, bitPattern));
  } else {
    createLookupPaths(*node.left, bitPattern + '0');
    createLookupPaths(*node.right, bitPattern + '1');
  }
}

/*
* Private Member function: writeEncodingTable
* -------------------------------------
* Writes the encoding table/tree to the output stream.  
* The tree is serialized as a pre-order traversal of
* the encoding tree.  Inner nodes are stored as zeros and
* leaf nodes are stored as ones with the node's byte/character
* stored as at least two byes, to accomodate the pseudo EOF character
*/
void Encoding::writeEncodingTable(dobstream& output) {
  writeEncodingTable(*mEncodingTableRoot, output);
  output.flush();
}

/*
* Private Member function: writeEncodingTable
* -------------------------------------
* Recursive helper for serializing the encoding tree/table.
* See description of the single-argument version.
*/
void Encoding::writeEncodingTable(node& node, dobstream& output) {
  //base case
  if (node.isLeaf()) {
    output.writebit(1);
    //output << (short) node.byte;
    output.writeAsShort(node.byte);
    //output.put(node.byte);
    return;
  }
  output.writebit(0);
  writeEncodingTable(*node.left, output);
  writeEncodingTable(*node.right, output);
}

/*
* Private Member function: readEncodingTable
* -------------------------------------
* Deserializes the pre-order representation of the encoding tree and
* assigns it to the mEncodingTableRoot member.
*/
void Encoding::readEncodingTable(ibstream& input) {
  input.rewind();
  mEncodingTableRoot = readEncodingNode(input);
}

/*
* Private Member function: readEncodingNode
* -------------------------------------
* Recursive helper for deserializing the encoding tree.
* See description of the single-argument version.
*/
Encoding::node* Encoding::readEncodingNode(ibstream& input) {
  int bit = input.readbit();
  if (bit) {
    //int byte = input.get();
    short byte;
    input.operator >>(byte);
    return new node(byte);
  }
  node* n = new node;
  n->left = readEncodingNode(input);
  n->right = readEncodingNode(input);
  return n;
}

/*
* Member function: buildEncodingForInput
* Usage: encoding.buildEncodingForInput(in);
* -------------------------------------
* Makes a pass over the input, records the "byte" alphabet and the 
* frequencies for the alphabet.  Then builds the encoding table
* for the alphabet.
*/
void Encoding::buildEncodingForInput(ibstream& input) {
  input.rewind();
  checkStreamState(input);

  //maps bytes found to frequency in file
  map<int, int> freqMap;

  while(input.good()) {
    int byte = input.get();
    if (input.eof())
      break;
    //if byte exists in map, increment its frequency, otherwise insert it
    map<int,int>::iterator it = freqMap.find(byte);
    if (it != freqMap.end()) {
      it->second++;  //hey, I didn't name this.
    } else {
      freqMap.insert(pair<int,int>(byte, 1));
    }
  }

  buildHuffmanTree(freqMap);

}

/*
* Private Member function: buildHuffmanTree
* -------------------------------------
* Builds the encoding tree out of a map of byte frequencies.
* iterates over the map, puts the bytes into a priority queue with the
* frequencies as weights, and then runs the huffman algorithm to find
* the optimal encoding tree.
*/
void Encoding::buildHuffmanTree(map<int, int>& frequencyMap) {
  PQueue<node*> pq;
  map<int, int>::iterator i = frequencyMap.begin();
  for( ; i != frequencyMap.end(); i++ ) {
    node* n = new node(i->first);
    pq.add(n, i->second);
  }

  //add the pseudo eof terminator
  node* pseudoEOFNode = new node(PSEUDO_EOF);
  pq.add(pseudoEOFNode, 1.0);

  while(pq.size() > 1) {
    double newPriority = pq.peekPriority();
    node* one = pq.extractMin();
    newPriority += pq.peekPriority();
    node* two = pq.extractMin();

    node* newNode = new node();
    newNode->left = one;
    newNode->right = two;
    pq.add(newNode, newPriority);
  }
  mEncodingTableRoot = pq.extractMin();
}
/*
* Private Member function: writeDecompressedFile
* -------------------------------------
* Writes the decompressed file to the output stream, from the
* compressed input stream.
*/
void Encoding::writeDecompressedFile(ibstream& input, dobstream& output) {
  node* currNode = mEncodingTableRoot;
  while(input.good()) {
    int bit = input.readbit();
    if (input.eof())
      break;

    if (bit)
      currNode = currNode->right;
    else
      currNode = currNode->left;

    if (currNode->isLeaf()) {
      //check for EOF
      if (currNode->byte == PSEUDO_EOF)
        break;
      output.put(currNode->byte);
      currNode = mEncodingTableRoot;
    }
  }
  output.flush();
}

/*
* Private Member function: writeCompressedFileBody
* -------------------------------------
* Writes the compressed file contents to the output stream.
* Input stream is the uncompressed source file.
*/
void Encoding::writeCompressedFileBody(ibstream& input, dobstream& output) {
  input.rewind();
  while(input.good()) {
    int byte = input.get();
    if (input.eof())
      break;

    string bitPattern = getBitPatternForByte(byte);
    output.writebits(bitPattern, byte);
  }
  //pseudo EOF terminator
  output.writebits(getBitPatternForByte(PSEUDO_EOF), PSEUDO_EOF);
  output.flush();
}

/*
* Private Member function: getBitPatternForByte
* -------------------------------------
*  Looks up the bit pattern for a given byte/character from the
* byteLookup dictionary.  Throws an Error if there is no bit pattern
* for the given byte/character.
*/
string Encoding::getBitPatternForByte(int byte) {
  map<int,string>::iterator it = mByteLookup.find(byte);
  if (it == mByteLookup.end())
    Error("Could not find bit pattern for byte/char: " + byte);
  return it->second;
}

/*
* Private Member function: checkStreamState
* -------------------------------------
* Validates that the given stream state is good.
* throws an Error if the stream is in a bad() state.
*/
void Encoding::checkStreamState(ios_base& stream) {
  if (stream.bad()) {
    Error("bad stream: " /* + stream.getName ?? */ );
  }
}

/*
* Member function: match
* Usage: Encoding::match(in, out);
* -------------------------------------
* Compares to files and attempts to determine if they are the same
* or different.  Outputs the first byte position where they differ.
* Returns true if the same or false if different.
*/
bool Encoding::match(ibstream& streamOne, ibstream& streamTwo) {
  if (streamOne.size() != streamTwo.size()) {
    cout << "mismatch in file sizes" << endl;
  }
  int byteCount = 0;
  // I realize this .eof() might not get set until the last get() gets called
  while (!streamOne.eof() && !streamTwo.eof()) {
    byteCount++;
    if (streamOne.get() != streamTwo.get()) {
      cout << "mismatch at byte: " << IntegerToString(byteCount) << endl;
      return false;
    }
  }
  return true;
}

/*
* Member function: selfTest
* Usage: Encoding::selfTest(someFile);
* -------------------------------------
* Compresses then decompresses the given file.  Then runs match on 
* the output file to verify it is the same as the input.  Assumes the
* file sourceFile exists.
*/
bool Encoding::selfTest(string sourceFile) {
  ibstream ins;
  ins.open(sourceFile.c_str());

  string shrunkFile = sourceFile + ".shrunk";
  dobstream dos(true);
  dos.open(shrunkFile.c_str());

  Encoding encoding;
  encoding.compress(ins, dos);

  ins.close();
  dos.close();

  ibstream ins2;
  dobstream dos2;
  string resultFile = sourceFile + ".uncompressed";
  ins2.open(shrunkFile.c_str());
  dos2.open(resultFile.c_str());
  Encoding encoding2;
  encoding2.decompress(ins2, dos2);

  ins2.close();
  dos2.close();

  ibstream matchStream1;
  ibstream matchStream2;
  matchStream1.open(sourceFile.c_str());
  matchStream2.open(resultFile.c_str());

  bool doTheyMatch = match(matchStream1, matchStream2);

  cout << "Do files match:  " << (doTheyMatch ? "yes" : "no") << endl << endl;
  return doTheyMatch;

}