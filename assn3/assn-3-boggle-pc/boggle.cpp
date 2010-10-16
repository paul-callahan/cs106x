/* File: boggle.cpp
* ----------------
* Author:
* Date:
* Section leader:
*/

#include "genlib.h"
#include "gboggle.h"
#include "sound.h"
#include "extgraph.h"
#include "simpio.h"
#include "lexicon.h"
#include "grid.h"
#include "map.h"
#include "vector.h"
#include "disallowcopy.h"
#include <iostream>
#include <algorithm>

const int MIN_WORD_SIZE = 4;


struct Location {
  int row, col;
  Location(int row, int col): row(row), col(col){};
  Location(): row(-1), col(-1){};

  bool operator==(const Location& other) {
    return other.col == col && other.row == row;
  }

  bool operator<(const Location& other) {
    return other.row < row ||
      (other.row == row && other.col < col);
  }
};



string PathToString(Vector<Location>& path);

class Player {
public:
  Player(): score(0) { }

  void AddToCurrentPath(Location& location) {
    currentWordLetterLocations.add(location);
    currentWordPath.add(location);
  }

  void AddBlackListedWord(string word) {
    otherPlayersWords.add(word);
  }

  Set<string>& GetBlackListedWords() {
    return otherPlayersWords;
  }

  const Set<Location>& GetCurrentPath() {
    return currentWordLetterLocations; 
  }

  void RollbackCurrentPath() {
    if (currentWordPath.size() > 0) {
      Location lastLocation = currentWordPath[currentWordPath.size()-1];
      currentWordPath.removeAt(currentWordPath.size()-1);
      currentWordLetterLocations.remove(lastLocation);
    }
  }
  void ClearCurrentPath() {
    currentWordLetterLocations.clear();
    currentWordPath.clear();
  }

  void GetFoundWords(Set<string>& outWordSet) {
    //no map.keySet ?  :-/
      foreach (string word in wordsToPath) {
        outWordSet.add(word);
     }   
  }

  void AddFoundWord(string word) {
    if (!otherPlayersWords.contains(word)) {
      wordsToPath.put(word, currentWordPath);
      score += word.size() - MIN_WORD_SIZE + 1;
      cout << word << PathToString(currentWordPath) <<    endl;
    }
  }

  int GetScore() { 
    return score; 
  }

private:
  int score;
  Map<Vector<Location> > wordsToPath;
  Set<Location> currentWordLetterLocations;
  Vector<Location> currentWordPath;
  Set<string> otherPlayersWords;
};

class GameState {

public:
  Grid<char> board;
  int recursiveCalls;
  GameState(): currentPlayer(humanPlayer) {
    board.resize(4, 4);
  }

  void ComputersTurn() {
    humanPlayer.GetFoundWords(computerPlayer.GetBlackListedWords());
    currentPlayer = computerPlayer;
  }

  Player& GetCurrentPlayer() {
    return currentPlayer;
  }

  Player humanPlayer;
  Player computerPlayer;
  Player& currentPlayer;
};

/* 
 * Function: GetValidPositions
 * -------------------
 * For a give location on the board, find all the *valid* adjacent letter 
 * locations and put them in the validPositionsOut Set.  A valid position 
 * is one that is not currently in use for the current word.
 */ 
void GetValidPositions(GameState& gameState, 
                       const Location& boardLoc, 
                       Set<Location>& validPositionsOut);

/* 
 * Function: FindWords
 * --gameState: object containing board and the players
 * -------------------
 * Find all the valid words for the current player (typically the computer 
 * player).  Valid words are words that are equal or greater in length to the
 * minimum word size (4) and have not been found by the other player already
 * 
 * This calls into the overloaded recursion helper function FindWords below
 * with an empty word prefix string and the current board starting point as
 * initial values.
 */ 
void FindWords(GameState& gameState);

/* 
 * Function: FindWords
 * --prefix: - word being built uo
 * --gameState: object containing board and the players
 * --validAdjacentPositions: locations letters adjacent to the current letter
 * -------------------
 * Recursively finds valid words on the board.  Each call checks if any of the
 * surrounding adjacent letters (validAdjacentPositions), form the beginings of
 * a word.  If not, the function backtracks out of the recursion one level and
 * checks the next adjacent letter.
 *
 * The location path on the board of the current word being formed is tracked in
 * the Player object inside the game state object.
 */ 
void FindWords(string prefix, 
               GameState& gameState, 
               Set<Location>& validAdjacentPositions);

/* 
 * Function: ValidateWord
 * --gameState: object containing board and the players
 * -------------------
 * Validates that a word is valid and exists on the board.
 * The word must be of minimum length (4), and use a path of
 * adjacent letters without using the same letter for a word.
 * 
 * Calls into the ValidateWord recursion helper below.
 */ 
bool ValidateWord(string word, GameState& gameState);


/* 
 * Function: ValidateWord
 * --fragment: a portion of the word being validated
 * --lastLetterLoc: location of the last found letter
 * --gameState: object containing board and the players
 * -------------------
 * 
 * 
 * 
 */ 
bool ValidateWord(string fragment, 
                  Location lastLetterLoc, 
                  GameState& gameState);

/* 
 * Function: Is1stLetterEqual
 * -------------------
 * 
 * 
 * 
 */ 
bool Is1stLetterEqual(char boardLetter, 
                      string wordFrag, 
                      bool useQu);

/* 
 * Function: GetLexicon
 * -------------------
 * Returns a reference to a static Lexicon object.
 * Used for checking the validity of words and word-fragments.
 * 
 */ 
Lexicon& GetLexicon();

/* 
 * Function: IsQ
 * --c: char being tested
 * -------------------
 * returns true if the char passed is a 'Q'.
 */ 
bool IsQ(char c);

/* 
 * Function: toUpper
 *--str: string to be transformed.
 * -------------------
 * Changes a string reference to all upper case.
 * also returns the same reference.
 */ 
string toUpper(std::string &str);

/* 
 * Function: AppendCharacter
 * --prefix: string that will be appened to
 * --nextLetter: char that will be appened
 * --useQu: Controls Q behavior
 * -------------------
 * 
 * 
 * 
 */ 
string BuildWord(string prefix, char nextLetter, bool useQu);

/* 
 * Function: DecrementWord
 * -------------------
 * Returns in effect fragment.substr(1), unless the useQu is set to true
 * and fragment[0] is Q or q, in which case it will perform fragment.substr(2);
 * 
 */ 
string DecrementWord(string fragment, bool useQu);

/* 
 * Function: Is1stLetterEqual
 * -------------------
 * This function compares a char to the first letter of a string.
 * If the useQu flag is set to true, a char Q will be treated as "QU" and
 * QU will be compared against the first two letters of the string.
 * 
 */ 
bool Is1stLetterEqual(char boardLetter, string wordFrag, bool useQu);


Lexicon& GetLexicon() {
  static Lexicon lexicon("lexicon.dat");
  return lexicon;
}

bool IsQ(char c) {
  return c == 'Q' || c == 'q';
}

// I stole this toLower/Upper func from a c++ forum.
// http://www.cplusplus.com/forum/general/837/#msg2843
string toUpper(std::string &str) {
	std::transform(str.begin(), str.end(), str.begin(), std::toupper);
  return str;
}

string AppendCharacter(string prefix, char nextLetter, bool useQu) {
  if (useQu) {
    if (IsQ(nextLetter))
      return prefix + "QU";
  }
  return prefix + nextLetter;
}

string DecrementWord(string fragment, bool useQu) {
  if (useQu) 
    if (IsQ(fragment[0]))
      return fragment.substr(2);
  return fragment.substr(1);
}

bool Is1stLetterEqual(char boardLetter, string wordFrag, bool useQu) {
  //TODO: do something to handle "Qu"
  if (useQu)
    if (IsQ(boardLetter)) {
      string first = wordFrag.substr(0, 2);
      return  first == "QU";
    }
  return boardLetter == wordFrag[0];
}

///////////////////////////////////////////////////////////////////////////////
//   find and validate word methods
//////////////////////////////////////////////////////////////////////////////

void FindWords(GameState& gameState) {
  Grid<char>& board = gameState.board;

  for (int row = 0; row < board.numRows(); row++) {
    for (int col = 0; col < board.numCols(); col++) {
      Location startLocation(row, col);
      Set<Location> startPath;
      startPath.add(startLocation);
      gameState.GetCurrentPlayer().ClearCurrentPath();
      FindWords("", gameState, startPath);
    }
  }
}

void FindWords(string prefix, GameState& gameState, Set<Location>& validPositions) {
  Lexicon& lexicon = GetLexicon();
  Grid<char>& board = gameState.board;
  Player& player = gameState.GetCurrentPlayer();

  foreach (Location currentLetterPosition in validPositions) {
    char boardLetter = board[currentLetterPosition.row][currentLetterPosition.col];
    string candidate = AppendCharacter(prefix, boardLetter, true);
    //cout << "--" << candidate << PathToString(userState.currentWordPath)  << endl;
    if (lexicon.containsPrefix(candidate)) {
      //is the candidate a word?
      if (candidate.size() >= MIN_WORD_SIZE && lexicon.containsWord(candidate)) {
        //duplicate words get overwritten.
        player.AddFoundWord(candidate);
      }
      player.AddToCurrentPath(currentLetterPosition);
      Set<Location> nextLetterPositions;
      GetValidPositions(gameState, currentLetterPosition, nextLetterPositions);
      FindWords(candidate, gameState, nextLetterPositions);
    }
  }
  //dead-end...  roll back our path by one
  player.RollbackCurrentPath();
}


bool ValidateWord(string word, GameState& gameState) {
  Lexicon& lexicon = GetLexicon();
  toUpper(word);
  if (lexicon.containsWord(word)) {
    for (int row = 0; row < gameState.board.numRows(); row++) {
      for (int col = 0; col < gameState.board.numCols(); col++) {
        if (Is1stLetterEqual(gameState.board[row][col], word, true)) {
          string nextFragment = DecrementWord(word, true);
          if (ValidateWord(nextFragment, Location(row, col), gameState)) {
            gameState.GetCurrentPlayer().AddFoundWord(word);
            gameState.GetCurrentPlayer().ClearCurrentPath();
            return true;
          }
        }
      }
    }
  }
  return false;
}


bool ValidateWord(string fragment, Location lastLetterLoc, GameState& gameState) {
  if (fragment.size() == 0) 
    return true;

  gameState.GetCurrentPlayer().AddToCurrentPath(lastLetterLoc);
  Set<Location> currentWordPath = gameState.GetCurrentPlayer().GetCurrentPath();

  Set<Location> adjacentLetters;
  GetValidPositions(gameState, lastLetterLoc, adjacentLetters);
  foreach (Location letterLoc in adjacentLetters) {
    if (Is1stLetterEqual(gameState.board[letterLoc.row][letterLoc.col], fragment, true)) {
      if (ValidateWord(DecrementWord(fragment, true), letterLoc, gameState))
        return true;
    }
  }
  gameState.GetCurrentPlayer().ClearCurrentPath();
  return false;
}


void GiveInstructions()
{
  cout << endl << "The boggle board is a grid onto which I will randomly distribute" << endl
    << "dice.  These 6-sided dice have letters rather than numbers on the faces, " << endl
    << "creating a grid of letters on which you try to form words.  You go first, " << endl
    << "entering the words you find that are formed by tracing adjoining " << endl
    << "letters.  Two letters adjoin if they are next to each other horizontally, " << endl
    << "vertically, or diagonally. A letter can only be used once in the word. Words" << endl
    << "must be at least 4 letters long and can only be counted once.  You score points" << endl
    << "based on word length, a 4-letter word is worth one, 5-letters two, etc.  After your " << endl
    << "tiny brain is exhausted, I, the brilliant computer, will find all the remaining " << endl
    << "words in the puzzle and double or triple your paltry score." << endl;

  cout << "\nHit return when you're ready...";
  GetLine();
}

void Welcome()
{
  cout << "Welcome!  You're about to play an intense game of mind-numbing Boggle." << endl
    << "The good news is that you might improve your vocabulary a bit.  The" << endl
    << "bad news is that you're probably going to lose miserably to this little" << endl
    << "dictionary-toting hunk of silicon.  If only YOU had a gig of RAM...\n" << endl;
}



int mainX()
{
  InitGraphics();
  Welcome();
  Lexicon lex("lexicon.dat");
  return 0;
}

/**
int OrderedLocationComparator(Location locOne, Location locTwo) {
  int diff = locOne.ordinal - locTwo.ordinal;
  if (diff == 0)
    return 0;
  return diff/abs(diff);
}
*/

/*
int UniqueLocationComparator(Location& locOne, Location& locTwo) {
  if (locOne.row == locTwo.row && locOne.col == locTwo.col)
    return 0;
  int dRow = locOne.row - locTwo.row;
  int dCol = locTwo.row - locTwo.col;
  if (dRow == 0) {
    return dCol/abs(dCol);
  } else {
    return dRow/abs(dRow);
  }
}

*/

string PathToString(Vector<Location>& path) {
  string output(" path:");
  foreach (Location loc in path) {
    output.append("[");
    output.append(IntegerToString(loc.row));
    output.append(",");
    output.append(IntegerToString(loc.col));
    output.append("]->");
  }
  return output;
}


void GetValidPositions(GameState& gameState, 
                       const Location& currentGridLoc, 
                       Set<Location>& validPositionsOut) {
  Set<Location> currentWordPath = gameState.GetCurrentPlayer().GetCurrentPath();
  for (int dRow = -1; dRow <= 1; dRow++) {
    for (int dCol = -1; dCol <= 1; dCol++) {
      int nRow = currentGridLoc.row + dRow;
      int nCol = currentGridLoc.col + dCol;
      if (nRow >= 0 && nCol >= 0 &&
        !(nRow == currentGridLoc.row && nCol == currentGridLoc.col) &&
        nRow < gameState.board.numRows() && nCol < gameState.board.numCols()) {
          Location newLocation(nRow, nCol);
          if (!currentWordPath.contains(newLocation)) {
            validPositionsOut.add(newLocation);
          }
      }
    }
  }
}

void TestFindWord() {
  GameState gameState;

  gameState.board[0][0] = 'C';
  gameState.board[0][1] = 'A';
  gameState.board[0][2] = 'R';
  gameState.board[0][3] = 'E';

  gameState.board[1][0] = 'O';
  gameState.board[1][1] = 'Z';
  gameState.board[1][2] = 'O';
  gameState.board[1][3] = 'I';

  gameState.board[2][0] = 'D';
  gameState.board[2][1] = 'F';
  gameState.board[2][2] = 'L';
  gameState.board[2][3] = 'Q';

  gameState.board[3][0] = 'A';
  gameState.board[3][1] = 'E';
  gameState.board[3][2] = 'P';
  gameState.board[3][3] = 'T';

  bool found = ValidateWord("QUILT", gameState);

  gameState.ComputersTurn();
  FindWords(gameState);

  cout<< gameState.GetCurrentPlayer().GetScore() << endl;

}

int main() {
  TestFindWord();
  return 0;
}