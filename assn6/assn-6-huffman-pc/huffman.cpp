/**
* File: huffman.cpp 
* -------------------
* Huffman main module.
*/

#include "genlib.h"
#include <iostream>
#include <algorithm>
#include "pqueue.h"

#include "simpio.h"

#include "encoding.h"
#include "bstream.h"
#include "debugstream.h"

string toUpper(std::string &str) {
  std::transform(str.begin(), str.end(), str.begin(), toupper);
  return str;
}

string PromptAndValidateInputFile(ibstream& input, string msg) {
  while (true) {
    cout << msg;
    string filename = GetLine();
    input.open(filename.c_str());
    if (input.fail()) {
      cout << "Cannot open \"" << filename << "\".  Try again." <<  endl;
      input.clear();
    } else {
      return filename;
    }
  }
}

void PromptAndValidateOutputFile(dobstream& output, string inputFile, string msg) {
  while (true) {
    cout << msg;
    string filename = GetLine();
    output.open(filename.c_str());
    if (output.fail() || inputFile == filename) {
      cout << "Cannot use \"" << filename << "\".  Try again." <<  endl;
      output.clear();
    } else {
      break;
    }
  }
}

void DoCompress() {
  ibstream input;
  dobstream output;
  string inputFile = PromptAndValidateInputFile(input, "Please enter the filename to compress: ");
  PromptAndValidateOutputFile(output, inputFile,
    "Please enter compressed filename: ");

  Encoding encoding;
  encoding.compress(input, output);

  int percent = (int) (((double)output.size() / (double)input.size()) * 100.0);
  cout << endl;
  cout << "File " << inputFile << " compressed to " << IntegerToString(percent)
       << "% of original." << endl << endl;
  
}

void DoDecompress() {
  ibstream input;
  dobstream output;
  string inputFile = PromptAndValidateInputFile(input, "Please enter the filename to decompress: ");
  PromptAndValidateOutputFile(output, inputFile,
    "Please enter uncompressed filename: ");
  Encoding encoding;
  encoding.decompress(input, output);
}

void MainPrompt() {

  while(true) {
    cout << "Your options:" << endl;
    cout << "\t\tC) Compress a file" << endl;
    cout << "\t\tD) Decompress a file" << endl;
    cout << "\t\tS) Run self-test" << endl;
    cout << "\t\tQ) Quit" << endl;
    cout << "Enter choice: ";
    string input = GetLine();
    toUpper(input);
    switch (input[0]) {
      case 'C':
        DoCompress();
        break;
      case 'D':
        DoDecompress();
        break;
      case 'S':
        Encoding::selfTest("test1.txt");
        break;
      case 'Q':
        return;
      default:
        cout << "That was not a valid choice. Please try again.";
    }
   
  }
}

int main()
{

  cout << "Welcome to CS106 Shrink-It(tm)!" << endl;
  cout << "This program uses the Huffman coding algorithm for compression." << endl;
  cout << "Any file can be compressed by this method, often with substantial" << endl;
  cout << "savings. Decompression will faithfully reproduce the original." << endl << endl;

  MainPrompt();

  cout << endl << "Good bye!" << endl;
  return 0;
}

