/* File: encoding.cpp
* -----------------
* Put your Encoding class implementation here!
*/

#include <iostream>
#include "encoding.h"
#include "bstream.h"
#include "pqueue.h"
#include <map>


bool Encoding::compress(ibstream &input, dobstream &output) {
  checkStreamState(input);
  checkStreamState(output);

  buildEncodingForInput(input);
  createLookupPaths(*mEncodingTableRoot, string());
  writeEncodingTable(output);
  writeCompressedFileBody(input, output);

  /** test code  
  node* tmp = mEncodingTableRoot;

  ibstream ins;
  ins.open(string("foo.cs106x").c_str());
  readEncodingTable(ins);
  map<int, string> tmpLookup = byteLookup;
  byteLookup.clear();
  createLookupPaths(*mEncodingTableRoot, string());

  */

  return true;
}

bool Encoding::decompress(ibstream &input, dobstream &output) {
  checkStreamState(input);
  checkStreamState(output);
  readEncodingTable(input);
  createLookupPaths(*mEncodingTableRoot, string());
  writeDecompressedFile(input, output);
  return true;
}

void Encoding::createLookupPaths(node& node, string bitPattern) {
  if (node.isLeaf()) {
    mByteLookup.insert(pair<int, string>(node.byte, bitPattern));
  } else {
    createLookupPaths(*node.left, bitPattern + '0');
    createLookupPaths(*node.right, bitPattern + '1');
  }
}

void Encoding::writeEncodingTable(dobstream& output) {
  writeEncodingTable(*mEncodingTableRoot, output);
  //output.writebit(1);
  //output.put(PSEUDO_EOF);
  output.flush();
}

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


void Encoding::readEncodingTable(ibstream& input) {
  input.rewind();
  mEncodingTableRoot = readEncodingNode(input);
}

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

string Encoding::getBitPatternForByte(int byte) {
  map<int,string>::iterator it = mByteLookup.find(byte);
  if (it == mByteLookup.end())
    Error("Could not find bit pattern for byte/char: " + byte);
  return it->second;
}

void Encoding::checkStreamState(ios_base& stream) {
  if (stream.bad()) {
    Error("bad stream: " /* + stream.getName ?? */ );
  }
}

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

void Encoding::selfTest(string sourceFile) {
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

}