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

const int MIN_WORD_SIZE = 4;


class Location {
public:
  // I wanted to make these const so a Location object would be
  // totally immutable, but was thwarted by the operator=
  // There must be a trick I'm not seeing.
  int row, col;

  Location(int row, int col): row(row), col(col){};

  Location():row(-1), col(-1){};

  bool operator==(const Location& other) {
    return other.col == col && other.row == row;
  }

  bool operator<(const Location& other) {
    return other.row < row ||
      (other.row == row && other.col < col);
  }

  Location& operator=(const Location& rhs) {
    if (this != &rhs) {
      row = rhs.row;
      col = rhs.col;
    }
    return *this;
  }
};

string PathToString(Vector<Location>& path);

class UserState {
  int score;
  Map<Vector<Location>> wordsToPath;
  Set<Location> currentPath;
  Vector<Location> orderedPath;
  Set<string> otherPlayersWords;

public:
  UserState() {
    score = 0;
  }

  void AddToPath(Location& location) {
    currentPath.add(location);
    orderedPath.add(location);
  }

  void AddBlackListedWord(string word) {
    otherPlayersWords.add(word);
  }

  void RollbackCurrentPath() {
    if (orderedPath.size() > 0) {
      Location lastLocation = orderedPath[orderedPath.size()-1];
      orderedPath.removeAt(orderedPath.size()-1);
      currentPath.remove(lastLocation);
    }
  }
  void ClearCurrentPath() {
    currentPath.clear();
    orderedPath.clear();
  }

  void GetFoundWords(Set<string>& outWordSet) {
    //no map.keySet ?  :-/
      foreach (string word in wordsToPath) {
        outWordSet.add(word);
     }   
  }

  void AddFoundWord(string word) {
    if (!otherPlayersWords.contains(word)) {
      wordsToPath.put(word, orderedPath);
      score += word.size() - MIN_WORD_SIZE + 1;
      cout << word << PathToString(orderedPath) <<    endl;
    }
  }

  int GetScore() { 
    return score; 
  }
  const Set<Location>& GetCurrentPath() {
    return currentPath; 
  }
};

class GameState {
  UserState humanPlayer;
  UserState computerPlayer;
  UserState& currentPlayer;

public:
  Grid<string> board;
  GameState(): currentPlayer(humanPlayer) {
    board.resize(4, 4);
  }

  void ComputersTurn() {
    //TODO: make this work
    //humanPlayer.GetFoundWords(computerPlayer.otherPlayersWords);    
    currentPlayer = computerPlayer;
  }

  UserState& GetCurrentPlayer() {
    return currentPlayer;
  }
};

Set<Location> GetValidPositions(const Grid<string>& board, const Location& currentGridLoc, const Set<Location>& currentPath);

void FindWords(string prefix, GameState& gameState, Set<Location>& validPositions);
void FindWords(GameState& gameState);

bool ValidateWord(string fragment, Location lastLetterLoc, GameState& gameState);
bool IsEqual(string boardLetter, char wordLetter);


Lexicon& GetLexicon() {
  static Lexicon lexicon("lexicon.dat");
  return lexicon;
}

void FindWords(GameState& gameState) {
  Grid<string>& board = gameState.board;

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
  Grid<string>& board = gameState.board;
  UserState& userState = gameState.GetCurrentPlayer();

  foreach (Location currentLetterPosition in validPositions) {
    string candidate = prefix + board[currentLetterPosition.row][currentLetterPosition.col];
    //cout << "--" << candidate << PathToString(userState.orderedPath)  << endl;
    if (lexicon.containsPrefix(candidate)) {
      //is the candidate a word?
      if (candidate.size() >= MIN_WORD_SIZE && lexicon.containsWord(candidate)) {
        //duplicate words get overwritten.
        userState.AddFoundWord(candidate);
      }
      userState.AddToPath(currentLetterPosition);
      Set<Location> nextLetterPositions = GetValidPositions(board, currentLetterPosition, userState.GetCurrentPath());
      FindWords(candidate, gameState, nextLetterPositions);
    }
  }
  //dead-end...  roll back our path by one
  userState.RollbackCurrentPath();
}


bool ValidateWord(string word, GameState& gameState) {
  Lexicon& lexicon = GetLexicon();
  if (lexicon.containsWord(word)) {
    for (int row = 0; row < gameState.board.numRows(); row++) {
      for (int col = 0; col < gameState.board.numCols(); col++) {
        if (IsEqual(gameState.board[row][col], word[0])) {
          if (ValidateWord(word.substr(1), Location(row, col), gameState))
            return true;
        }
      }
    }
  }
  return false;
}


bool ValidateWord(string fragment, Location lastLetterLoc, GameState& gameState) {
  if (fragment.size() == 0) 
    return true;

  gameState.GetCurrentPlayer().AddToPath(lastLetterLoc);
  Set<Location> currentWordPath = gameState.GetCurrentPlayer().GetCurrentPath();

  Set<Location> adjacentLetters = GetValidPositions(gameState.board, lastLetterLoc, currentWordPath);
  foreach (Location letterLoc in adjacentLetters) {
    if (IsEqual(gameState.board[letterLoc.row][letterLoc.col], fragment[0])) {
      if (ValidateWord(fragment.substr(1), letterLoc, gameState))
        return true;
    }
  }
  gameState.GetCurrentPlayer().ClearCurrentPath();
  return false;
}

bool IsEqual(string boardLetter, char wordLetter) {
  //TODO: do something to handle "Qu"
  string wordLetterStr;
  wordLetterStr += wordLetter;
  return boardLetter == wordLetterStr;

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


Set<Location> GetValidPositions(const Grid<string>& board, const Location& currentGridLoc, const Set<Location>& currentPath) {
  Set<Location> nextValidPositions;
  for (int dRow = -1; dRow <= 1; dRow++) {
    for (int dCol = -1; dCol <= 1; dCol++) {
      int nRow = currentGridLoc.row + dRow;
      int nCol = currentGridLoc.col + dCol;
      if (nRow >= 0 && nCol >= 0 &&
        !(nRow == currentGridLoc.row && nCol == currentGridLoc.col) &&
        nRow < ((Grid<string>)board).numRows() && nCol < ((Grid<string>)board).numCols()) {
          Location newLocation(nRow, nCol);
          if (!((Set<Location>)currentPath).contains(newLocation)) {
            nextValidPositions.add(newLocation);
          }
      }
    }
  }
  //copy whole set?  yuck
  return nextValidPositions;
}

void TestFindWord() {
  GameState gameState;





  gameState.board[0][0] = "C";
  gameState.board[0][1] = "A";
  gameState.board[0][2] = "R";
  gameState.board[0][3] = "E";

  gameState.board[1][0] = "O";
  gameState.board[1][1] = "Z";
  gameState.board[1][2] = "O";
  gameState.board[1][3] = "I";

  gameState.board[2][0] = "D";
  gameState.board[2][1] = "F";
  gameState.board[2][2] = "L";
  gameState.board[2][3] = "Qu";

  gameState.board[3][0] = "A";
  gameState.board[3][1] = "E";
  gameState.board[3][2] = "P";
  gameState.board[3][3] = "T";

  bool found = ValidateWord("CARE", gameState);

  gameState.ComputersTurn();
  FindWords(gameState);

  cout<< gameState.GetCurrentPlayer().GetScore() << endl;

}

int main() {
  TestFindWord();
  return 0;
}