/*
* File: RandomWriter.cpp
* ----------------------
* Name: Paul Callahan (paul.callahan@gmail.com)
* Section Leader Name: 
* Grader: Alex Quach
* This file is the starter project for the random writer problem
* on Assignment #2.
* This program performs Markov analysis on an input file in order to
* output "random" text that appears similar to the input file.
* The user is prompted for an input file and a markov order.
*/

#include "genlib.h"
#include "simpio.h"
#include "random.h"
#include "vector.h"
#include "map.h"
#include "set.h"
#include <iostream>
#include <fstream>
#include <queue>
#include <vector>

/*
IMPORTANT NOTE FOR OPENING FILES:
When opening files, you must append ".txt" to the end of the filename.
For instance, to open "Hamlet" you would do...
infile.open("Hamlet.txt");

*/

const int CHARS_TO_OUTPUT = 2000;

/**
* Function: CountFrequency
* --------------------------
* Counts of a character following a string sequence and stores this
* inside the freqMap Map of vectors.  Also tracks the number of occurences
* of the sequence itself.
*
*/
void CountFrequency(string sequence, char nextChar, Map<vector<char>*>& freqMap, Map<int>& sequenceCounts);

/**
* Function: ReadLinesAndAnalyze
* --------------------------
* Opens a file and performs a markov analysis and places frequency information
* in the freqMap variable.
*/
void ReadLinesAndAnalyze(string fileName, int markovOrder, Map<vector<char>*>& freqMap, Map<int>& sequenceCounts);

/**
* Function: FindMaximumStringFrequency
* --------------------------
* Finds the maximum frequence of a string sequence in the given map.
*
*/
void FindMaximumStringFrequency(Map<int>& strings2Frequency, string& maxStringOut);

/**
* Function: PickNextChar
* --------------------------
* Picks the next character to output based on probability of it showing
* up after a given sequence in the original text.
*/
char PickNextChar(const string& sequence, Map<vector<char>*>& freqMap);

/**
* Function: ReadFileName
* --------------------------
* Reads a file name from the user, then verifies the file can be opened.
*
*/
void ReadFileName(string& fileName);

/**
* Function: 
* --------------------------
* Prompts user to enter the desired level of markov 
* analysis, and verifies the range as correct.
*/
int PromptUserForAnalysisOrder();


int main() {
  Randomize();
  Map<vector<char>*> freqMap;
  Map<int> sequenceCounts;
  string fileName;
  ReadFileName(fileName);
  int markovOrder = PromptUserForAnalysisOrder();
  ReadLinesAndAnalyze(fileName, markovOrder, freqMap, sequenceCounts);

  cout << sequenceCounts.size() << " sequences found." << endl;

  string seed;
  FindMaximumStringFrequency(sequenceCounts, seed);

  int charsWritten = seed.size();
  while(charsWritten < CHARS_TO_OUTPUT ) {
    char nextChar = PickNextChar(seed, freqMap);
    //by chance we hit something in the input string with
    //no chars following it, so we bail, as per instructions.
    if (nextChar == 0)
      break;
    cout << nextChar;
    charsWritten++;
    seed = seed.substr(1) + nextChar;   
  }
  cout << endl;
  return 0;
}

char PickNextChar(const string& sequence, Map<vector<char>*>& freqMap) {
  char nextChar = 0;
  if (freqMap.containsKey(sequence)) {
    vector<char>* charList = freqMap[sequence];
    if (charList != NULL && charList->size() > 0) {
      //this works because chars that show up in the vector more times are more likely
      //to get picked!
      int probalisticChoice = RandomInteger(0, charList->size() -1);
      nextChar =  charList->at(probalisticChoice);
    }
  }
  return nextChar;
}

void ReadLinesAndAnalyze(string fileName, int markovOrder, Map<vector<char>*>& freqMap, Map<int>& sequenceCounts) {
  ifstream fileStream;
  fileStream.open(fileName.c_str());
  if (fileStream.fail()) {
    cout << "Could not open file.  Exiting" << endl;
    return;
  }
  int charCount = 0;
  char* curSeq = new char(markovOrder +1);
  fileStream.get(curSeq, markovOrder +1);
  //if (count < markovOrder) {
  //file read is smaller than the sequence length.
  //  return;
  //}
  string currentSequence(curSeq);
  int ch;
  while((ch = fileStream.get()) != EOF) {
    //cout << currentSequence << ": "<< ch  << endl;
    CountFrequency(currentSequence, ch, freqMap, sequenceCounts);
    //cout << ch ;
    currentSequence = currentSequence.substr(1, markovOrder -1) + (char) ch;
    charCount++;
    if (charCount % 50000 == 0) {
      cout<< charCount << " characters read." << endl;
    }
  }
  fileStream.close();
  //delete curSeq;  //delete the curSeq char blob above
}

void CountFrequency(string sequence, char nextChar, Map<vector<char>*>& freqMap, Map<int>& sequenceCounts) {
  vector<char>* charList;
  if(!freqMap.containsKey(sequence)) {
    charList = new vector<char>;
    freqMap.put(sequence, charList);
  } else {
    charList= freqMap[sequence];
  }
  charList->push_back(nextChar);
  //update the sequence count
  int sequenceCount = 0;
  if (sequenceCounts.containsKey(sequence)) {
    sequenceCount = sequenceCounts[sequence];
  }
  sequenceCounts.put(sequence, sequenceCount +1);
}


void FindMaximumStringFrequency(Map<int>& strings2Frequency, string& maxStringOut) {
  int maxCount = 0;
  foreach (string key in strings2Frequency) {
    if (strings2Frequency[key] > maxCount) {
      maxCount = strings2Frequency[key];
      maxStringOut = key;
    }
  }
}

int PromptUserForAnalysisOrder() {
  while (true) {
    cout << "What order of analysis? (a number from 1 to 10): " ;
    int order = GetInteger();
    if (order < 1 || order > 10)
      cout << "Invalid order."  << endl;
    else 
      return order;
  }

  
}

void ReadFileName(string& fileName) {
  cout << "Enter file name to analyze. " << endl;
  cout << "File: ";
  while (true) {
    string readValue = GetLine();
    if (readValue.size() > 0) {
      ifstream fileStream;
      fileStream.open(readValue.c_str());
      if (fileStream.good()) {
        fileStream.close();
        fileName = readValue;
        break;
      }
      fileStream.close();
    }
    cout << "Invalid file.  You entered: " << readValue << endl;
    cout << "Retry: ";
  }
}