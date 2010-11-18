/**
* File: huffman.cpp 
* -------------------
* Huffman main module.
* 
* Prompts user to compress, decompress or match files.
*
* Author: Paul Callahan 
* Grader: Alex Quach
* Assignment #6, Huffman Encoding
*/

#include "genlib.h"
#include <iostream>
#include <algorithm>
#include "pqueue.h"

#include "simpio.h"

#include "encoding.h"
#include "bstream.h"
#include "debugstream.h"

/*
* Function: DoCompress
* --------------------------
* Prompts user through decompressing a file.
* Then does the decompressing.
*/
void DoCompress();

/*
* Function: DoDecompress
* --------------------------
* Prompts user through decompressing a file.
* Then does the decompressing.
*/
void DoDecompress();

/*
* Function: DoMatch
* --------------------------
* Prompts user checking if two files match.
*/
void DoMatch();

/*
* Function: MainPrompt
* --------------------------
* Main user prompt.  Prompts users whether to compress, decompress, etc.
*/
void MainPrompt();
/*
* Function: toUpper
* --------------------------
* makes a string upper case.
*/
string toUpper(std::string &str);

/*
* Function: PromptAndValidateInputFile
* --------------------------
* Prompts user for an input file, and validates it can be opened.
*/
string PromptAndValidateInputFile(ibstream& input, string msg);

/*
* Function: PromptAndValidateOutputFile
* --------------------------
* Prompts user for an output file and validates it can be opened
* and it is not the same as the input file.
*/
void PromptAndValidateOutputFile(dobstream& output, string inputFile, string msg);


/*
* Function: main
* --------------------------
* entry point.
*/
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

/*
* Function: DoCompress
* --------------------------
* Prompts user through decompressing a file.
* Then does the decompressing.
*/
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

/*
* Function: DoDecompress
* --------------------------
* Prompts user through decompressing a file.
* Then does the decompressing.
*/
void DoDecompress() {
  ibstream input;
  dobstream output;
  string inputFile = PromptAndValidateInputFile(input, "Please enter the filename to decompress: ");
  PromptAndValidateOutputFile(output, inputFile,
    "Please enter uncompressed filename: ");
  Encoding encoding;
  encoding.decompress(input, output);
}

/*
* Function: DoMatch
* --------------------------
* Prompts user checking if two files match.
*/
void DoMatch() {
  ibstream streamOne;
  ibstream streamTwo;

  string inputFileOne = PromptAndValidateInputFile(streamOne, "Please enter the first file: ");
  string inputFileTwo = PromptAndValidateInputFile(streamTwo, "Please enter the second file: ");
  Encoding::match(streamOne, streamTwo);
}

/*
* Function: MainPrompt
* --------------------------
* Main user prompt.  Prompts users whether to compress, decompress, etc.
*/
void MainPrompt() {

  while(true) {
    cout << "Your options:" << endl;
    cout << "\t\tC) Compress a file" << endl;
    cout << "\t\tD) Decompress a file" << endl;
    cout << "\t\tM) Match two files (for checking results)" << endl;
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
        cout << "Testing two files.  Please wait." << endl;
        Encoding::selfTest("smalltest.txt");
        Encoding::selfTest("bigtest.bin");
        break;
      case 'M':
        DoMatch();
        break;
      case 'Q':
        return;
      default:
        cout << "That was not a valid choice. Please try again.";
    }

  }
}

/*
* Function: PromptAndValidateInputFile
* --------------------------
* Prompts user for an input file, and validates it can be opened.
*/
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

/*
* Function: PromptAndValidateOutputFile
* --------------------------
* Prompts user for an output file and validates it can be opened
* and it is not the same as the input file.
*/
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

/*
* Function: toUpper
* --------------------------
* makes a string upper case.
*/
string toUpper(std::string &str) {
  std::transform(str.begin(), str.end(), str.begin(), toupper);
  return str;
}