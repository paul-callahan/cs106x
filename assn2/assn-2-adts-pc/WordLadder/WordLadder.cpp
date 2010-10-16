/*
* File: WordLadder.cpp
* --------------------
* Name: Paul Callahan (paul.callahan@gmail.com)
* Section Leader Name: 
* Grader: Alex Quach
* This file is the starter project for the word ladder problem
* on Assignment #2, part ii.
* This program prompts the user for a starting word and an ending
* word and attempts to create a word ladder where each rung in the
* ladder is a word that differs from the previous by one letter.
*/

#include "genlib.h"
#include "lexicon.h"
#include "queue.h"
#include "simpio.h"
#include "vector.h"
#include <iostream>


/**
* Function: RunLadder
* --------------------------
*  Runs the word ladder main function with a starting word
*  and ending word.
*  Returns true if a ladder was found, false otherwise.
*/
bool RunLadder(string& startWord, string& targetWord);
/**
* Function: AddHop
* --------------------------
* Adds new ladders to the end of the queue of ladders by taking the last word
* and changing one of the letters.  The usedWords set is used to track previously
* used words.
*/
void AddHop(Vector<string>& ladder, Queue<Vector<string>>& searchQueue, 
            Set<string>& usedWords);

/**
* Function: PrintLadder
* --------------------------
* Prints out the specified word ladder
*/
void PrintLadder(Vector<string>& ladder);

/**
* Function: PromptUser
* --------------------------
*  Prompts user for a starting and ending word for the word ladder.
*  Gives the user a chance to exit the program.
*/
void PromptUser(string& startWord, string& targetWord);

/**
* Function: IsWordValid
* --------------------------
* Consults the CS106X lexicon to see if a specified word is valid.
*/
bool IsWordValid(const string& word);

/**
* Function: IsWordUsed
* --------------------------
* Checks the specified word against a set of previously used words.
*/
bool IsWordUsed(const string& word, Set<string>& usedWords);

int main() {

  while (true) {
    string startWord;
    string targetWord;
    PromptUser(startWord, targetWord);
    if (startWord.size() == 0)
      break;
    bool result = RunLadder(startWord, targetWord);

    if (!result) {
      cout << "No ladder found." << endl;
    }
  }
  return 0;

}

bool RunLadder(string& startWord, string& targetWord) {
  Vector<string> seedLadder;
  Queue<Vector<string>> searchQueue;
  Set<string> usedWords;
  seedLadder.add(startWord);
  searchQueue.enqueue(seedLadder);
  while (!searchQueue.isEmpty()) {
    Vector<string> ladder = searchQueue.dequeue();
    string targetCandidate = ladder.getAt(ladder.size() -1);
    if (targetCandidate == targetWord) {
      PrintLadder(ladder);
      return true;
    }
    else 
      AddHop(ladder, searchQueue, usedWords);
  }
  return false;
}

bool IsWordValid(const string& word) {
  static Lexicon lex("EnglishWords.dat");
  return lex.containsWord(word);
}

bool IsWordUsed(const string& word, Set<string>& usedWords) {
  if (usedWords.contains(word))
    return true;
  usedWords.add(word);
  return false;
}

void AddHop(Vector<string>& ladder, Queue<Vector<string>>& searchQueue, Set<string>& usedWords) {
  string word = ladder.getAt(ladder.size() -1);
  for (int charPos = 0; charPos < word.size(); charPos++) {
    for (char letter = 'a'; letter  <= 'z'; letter++) {
      string newWord(word);
      if (newWord[charPos] != letter) {
        newWord[charPos] = letter;
        if (IsWordValid(newWord) && !IsWordUsed(newWord, usedWords)) {
          Vector<string> ladderCopy(ladder);
          ladderCopy.add(newWord);
          searchQueue.enqueue(ladderCopy);
        }
      }
    }
  }
}

void PromptUser(string& startWord, string& targetWord) {
  cout << "Enter start word (RETURN to quit):";
  startWord = GetLine();
  if (startWord.size() == 0)
    return;
  while (true) {
    cout << "Enter target word:";
    targetWord = GetLine();
    if (targetWord.size() > 0)
      return;
  }
}

void PrintLadder(Vector<string>& ladder) {
  for (int i = 0; i < ladder.size(); i++) {
    cout << ladder[i];
    if (i < ladder.size() -1)
      cout << "->";
  }
  cout << endl;
}
