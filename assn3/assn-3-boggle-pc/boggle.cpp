/* File: boggle.cpp
* ----------------
* Author: Paul Callahan 
* Grader: Alex Quach
* Assignment #3, boggle
*
* This program creates the word game boggle.
* First, itrompts user to read dice from a file or enter manually,
* Then randomly creates a game board and displays it.
* Then it allows the user to play first.  User enters words 
* he or she finds at the command prompt.
* After the user has played, the computer finds the remaing
* words on the board.
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
#include <iostream>
#include <algorithm>
#include "random.h"

// required word size
const int MIN_WORD_SIZE = 4;
//number of sides on a die
const int NUM_SIDES_ON_DIE = 6;
//board width & height
const int BOARD_SIZE = 4;



/*
* Record: Location
* ----------
* This struct represents a point on the game board.
*
*/
struct Location {
  int row, col;
  Location(int row, int col): row(row), col(col){};
  Location(): row(-1), col(-1){};

  /*
  * Method: operator==
  * ----------------------------
  * Required for using this record in cs106x container classes
  * Returns true if this Location is equal to the given location..
  */
  bool operator==(const Location& other) {
    return other.col == col && other.row == row;
  }

  /*
  * Method: operator<
  * ----------------------------
  * Required for using this record in cs106x container classes
  * A location is greater than another location if
  * its row is bigger, or if the rows are the same,
  * its column is bigger.
  */
  bool operator<(const Location& other) {
    return other.row < row ||
      (other.row == row && other.col < col);
  }
};


/*
* Class: Player
* ----------
* This class represents a player in the game.
* Contains words and the paths of those words this
* player has found, as well as the list of words
* this player is not allowed to use.
*/
class Player {
public:

  //default construtor
  Player(): score(0) { }

  /*
  * Method: AddBlackListedWord
  * ----------------------------
  * Adds specified word to words this player cannot use
  * (i.e. the other player used them)
  */
  void AddBlackListedWord(string word) {
    blackListedWords.add(word);
  }

  /*
  * Method: GetBlackListedWords
  * ----------------------------
  * Returns the set of words this user cannot
  * use.  (i.e. the words the other player has
  * already used.
  */
  Set<string>& GetBlackListedWords() {
    return blackListedWords;
  }

  /*
  * Method: AddToCurrentPath
  * ----------------------------
  * Adds a board Location to the currently tracked word path
  * for this user.
  */
  void AddToCurrentPath(const Location& location) {
    currentWordLetterLocations.add(location);
    currentWordPath.add(location);
  }

  /*
  * Method: GetCurrentPath
  * ----------------------------
  * Gets the currently tracked word path for the current word, 
  * as a set.
  */
  const Set<Location>& GetCurrentPath() {
    return currentWordLetterLocations; 
  }

  /*
  * Method: RollbackCurrentPath
  * ----------------------------
  * Remove the last added word path location from the 
  * word path for the currently tracked word.
  */
  void RollbackCurrentPath() {
    if (currentWordPath.size() > 0) {
      Location lastLocation = currentWordPath[currentWordPath.size()-1];
      currentWordPath.removeAt(currentWordPath.size()-1);
      currentWordLetterLocations.remove(lastLocation);
    }
  }

  /*
  * Method: ClearCurrentPath
  * ----------------------------
  *  Clear out the currently tracked word path.
  */
  void ClearCurrentPath() {
    currentWordLetterLocations.clear();
    currentWordPath.clear();
  }

  /*
  * Method: GetPathForWord
  * ----------------------------
  * Gets the path for a found word, as a vector of elements.
  * This path is in the same order as the spelling of the word.
  */
  Vector<Location> GetPathForWord(const string& word) {
    return wordsToPath.get(word);
  }

  /*
  * Method: GetFoundWords
  * ----------------------------
  * Returns the set of words this user has found on the 
  * boggle board.
  */
  void GetFoundWords(Set<string>& outWordSet) {
    //no map.keySet ?  :-/
    foreach (string word in wordsToPath) {
      outWordSet.add(word);
    }   
  }

  /*
  * Method: AddFoundWord
  * ----------------------------
  * Adds a word to the set of found words.
  */
  void AddFoundWord(string word) {
    if (!blackListedWords.contains(word)) {
      wordsToPath.put(word, currentWordPath);
      score += word.size() - MIN_WORD_SIZE + 1;
      //cout << word << PathToString(currentWordPath) << endl;
    }
  }

  /*
  * Method: GetScore
  * ----------------------------
  *  Returns the current score for this user.
  */
  int GetScore() { 
    return score; 
  }

private:
  //score
  int score;
  // map of found words to vector of loctions (the path)
  Map<Vector<Location> > wordsToPath;
  // currently tracked word path
  Vector<Location> currentWordPath;
  // set of locations for the currently tracked word
  Set<Location> currentWordLetterLocations;
  // set of words is user cannot play
  Set<string> blackListedWords;
};


/*
* Class: GameState
* ----------
* This class represents the state of a game.  Contains
* player objects and the board object.
*/
class GameState {

public:

  //default constructor - sizes board and sets human as 
  // current player.
  GameState(): currentPlayer(humanPlayer) {
    board.resize(BOARD_SIZE, BOARD_SIZE);
  }

  /*
  * Method: ComputersTurn
  * ----------------------------
  * Sets the player currently playing to the computer
  */
  void ComputersTurn() {
    humanPlayer.GetFoundWords(computerPlayer.GetBlackListedWords());
    currentPlayer = computerPlayer;
  }

  /*
  * Method: GetCurrentPlayer
  * ----------------------------
  * Returns the player object currently playing.
  */
  Player& GetCurrentPlayer() {
    return currentPlayer;
  }

  // game board
  Grid<char> board;

private:
  // human player object
  Player humanPlayer;
  // computer player object
  Player computerPlayer;
  // player currently playing
  Player& currentPlayer;
};

///////////////////////////////////////////////////////////////////////////////
//   find and validate word methods
///////////////////////////////////////////////////////////////////////////////

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
* returns true if the word is valid.
*/ 
bool ValidateWord(string word, GameState& gameState);


/* 
* Function: ValidateWord
* --fragment: a portion of the word being validated
* --lastLetterLoc: location of the last found letter
* --gameState: object containing board and the players
* -------------------
* Recursively attempts find the letters that comprise the word
* on the board such that the letters are adjacent on the board.
* 
* Starts with the first letter of the word fragment, finds it,
* then calls itself with the next word fragment with is the current
* fragment minus the first letter.
* Returns true if it gets down to an empty string.
* Returns false if it can't find a valid path.
* 
* 
*/ 
bool ValidateWord(string fragment, 
                  const Location& lastLetterLoc, 
                  GameState& gameState);

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

///////////////////////////////////////////////////////////////////////////////
//   Input and Message Methods
///////////////////////////////////////////////////////////////////////////////

/* 
* Function: YesNoPrompt
* --message: message to print before prompting for input
* -------------------
* Prints the specified message and prompts the user for "yes" or "no".
* Keeps asking until one of those is entered.
* returns true if yes, false if no.
* case insensitive.
*/ 
bool YesNoPrompt(string message);

/* 
* Function: GiveInstructions
* -------------------
* Prints out the canned game instructions text.
*/ 
void GiveInstructions();

/* 
* Function: Welcome
* -------------------
* Prints out the canned welcome text supplied with this file.
*/ 
void Welcome();

/* 
* Function: ReadDiceFromUser
* --boardSize: size of boggle board
* --diceList: vector populated with dice strings
* -------------------
* Reads the boggle dice from user input.  The user enters
* boardSize squared lines.  Each line contains the same 
* strings as the dice file specified below.
*/ 
void ReadDiceFromUser(int boardSize, Vector<string>& diceList);

/* 
* Function: ReadDiceFile
* --boardSize: size of boggle board
* --diceList: vector populated with dice strings
* -------------------
* Reads the boggle dice from a file.  The file name is based on the
* number of dice the board holds (boardSize squared).
* The dice file is boardSize squared lines long.  Each line has six
* characters in a string, representing a side of each die.
*/ 
void ReadDiceFile(int boardSize, Vector<string>& diceList);

///////////////////////////////////////////////////////////////////////////////
//   Graphics Methods
///////////////////////////////////////////////////////////////////////////////

/* 
* Function: LabelAllCubes
* --board:  boggle game board grid.
* -------------------
* Draw all the letters in the board object on the display.
*/ 
void LabelAllCubes(Grid<char>& board);

/* 
* Function: FlashWordPath
* --wordPath: path of locations that traces out word on board
* --letterDelay: time in seconds to pause between highlighting a letter
* --wordDelay: time in seconds to leave word highlighted
* -------------------
* Highlights a word on the display.  Incrementally highlights each letter
* then after wordDelay seconds, unhighlights the entire word.
*/ 
void FlashWordPath(Vector<Location>& wordPath, 
                   double letterDelay, double wordDelay);

///////////////////////////////////////////////////////////////////////////////
//   Game Play Methods
///////////////////////////////////////////////////////////////////////////////

/* 
* Function: HumanPlays
* --gameState: object containing board and the players
* -------------------
* Human plays, entering words in from the command prompt.
*/
void HumanPlays(GameState& gameState);

/* 
* Function: ComputerPlays
* --gameState: object containing board and the players
* -------------------
* Computer takes its turn, finding all the words the pathetic human
* misses.
*/
void ComputerPlays(GameState& gameState);


///////////////////////////////////////////////////////////////////////////////
//   Utility methods
///////////////////////////////////////////////////////////////////////////////

/* 
* Function: PopulateBoard
* -------------------
* Randomly populates the game board.  For each board cell, a random "die" 
* is selected from the diceList.  The die is just a string where each
* represents a side of the die.  That die is removed from the diceList
* so it does not get reused.  Then a random side of the die is randomly 
* selected and assigned to the board cell.
* 
*/ 
void PopulateBoard(Grid<char>& board, Vector<string>& diceList);

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
* --c: char that will be appened
* --useQu: Controls Q behavior
* -------------------
* Appends the specified character to the string prefix.
* If useQu is true and the character is a Q, then QU will
* be appended.
* 
*/ 
string AppendCharacter(string prefix, char c, bool useQu);

/* 
* Function: DecrementWord
* --fragment: string being shrunk
* --useQu: Controls Q behavior
* -------------------
* Returns in effect fragment.substr(1), unless the useQu is set to true
* and fragment[0] is Q or q, in which case it will perform fragment.substr(2);
* 
*/ 
string DecrementWord(string fragment, bool useQu);

/* 
* Function: Is1stLetterEqual
* --letter: char being compared
* --wordFrag: string whose 1st letter is being compared
* --useQu: Controls Q behavior
* -------------------
* This function compares a char to the first letter of a string.
* If the useQu flag is set to true, a char Q will be treated as "QU" and
* QU will be compared against the first two letters of the string.
* 
*/ 
bool Is1stLetterEqual(char letter, string wordFrag, bool useQu);



///////////////////////////////////////////////////////////////////////////////
//   find and validate word methods
//////////////////////////////////////////////////////////////////////////////

void FindWords(GameState& gameState) {
  Grid<char>& board = gameState.board;

  for (int row = 0; row < board.numRows(); row++) {
    for (int col = 0; col < board.numCols(); col++) {
      Location startLocation(row, col);// = new Location(row, col);
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
        //try a letter location that is the same as the first letter of word
        if (Is1stLetterEqual(gameState.board[row][col], word, true)) {
          string nextFragment = DecrementWord(word, true);
          if (ValidateWord(nextFragment, Location(row, col), gameState)) {
            //we found a path, so the word is valid.
            gameState.GetCurrentPlayer().AddFoundWord(word);
            gameState.GetCurrentPlayer().ClearCurrentPath();
            return true;
          }
        }
      }
    }
  }
  //word not found
  gameState.GetCurrentPlayer().ClearCurrentPath();
  return false;
}

bool ValidateWord(string fragment, const Location& lastLetterLoc, GameState& gameState) {
  // base case - we've whittled the string down to nothing after finding all the
  // letters
  if (fragment.size() == 0) {
    gameState.GetCurrentPlayer().AddToCurrentPath(lastLetterLoc);
    return true;
  }

  gameState.GetCurrentPlayer().AddToCurrentPath(lastLetterLoc);
  Set<Location> currentWordPath = gameState.GetCurrentPlayer().GetCurrentPath();
  // populate this set with valid adjacent letter locations
  Set<Location> adjacentLetters;
  GetValidPositions(gameState, lastLetterLoc, adjacentLetters);
  //try each adjacent letter
  foreach (Location letterLoc in adjacentLetters) {
    if (Is1stLetterEqual(gameState.board[letterLoc.row][letterLoc.col], fragment, true)) {
      if (ValidateWord(DecrementWord(fragment, true), letterLoc, gameState))
        return true;
    }
  }
  //no path found, so bail
  gameState.GetCurrentPlayer().ClearCurrentPath();
  return false;
}

void GetValidPositions(GameState& gameState, 
                       const Location& currentGridLoc, 
                       Set<Location>& validPositionsOut) {
   Set<Location> currentWordPath = gameState.GetCurrentPlayer().GetCurrentPath();
   for (int dRow = -1; dRow <= 1; dRow++) {
     for (int dCol = -1; dCol <= 1; dCol++) {
       int nRow = currentGridLoc.row + dRow;
       int nCol = currentGridLoc.col + dCol;
       //on-grid (positive) cells
       if (nRow >= 0 && nCol >= 0 ) {
         //not our own cell 
         if (!(nRow == currentGridLoc.row && nCol == currentGridLoc.col)) {
           // cells that are within the outer bounds of the gi
           if (nRow < gameState.board.numRows() && nCol < gameState.board.numCols()) {
             Location newLocation(nRow, nCol);
             // not a location that has been used for this word already
             if (!currentWordPath.contains(newLocation)) {
               validPositionsOut.add(newLocation);
             }
           }
         }
       }
     }
   }
}

///////////////////////////////////////////////////////////////////////////////
//   Input and Message Methods
///////////////////////////////////////////////////////////////////////////////

bool YesNoPrompt(string message) {
  cout << message << " ";
  while (true) {
    string input = GetLine();
    toUpper(input);
    if (input == "YES")
      return true;
    else if (input == "NO")
      return false;
    cout << endl << "Please answer yes or no: ";
  }
}

void GiveInstructions() {
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



void ReadDiceFromUser(int boardSize, Vector<string>& diceList) {
  cout << "Enter a 16-character string to identify which letters you want on the cubes." << endl
    << "The first 4 letters are the cubes on the top row from left to right" << endl
    << "next 4 letters are the second row, etc." << endl;
  for (int i = 0; i < (boardSize * boardSize); i++) {
    while (true) {
      cout << "Enter line #" << (i + 1) << ":";
      string input = GetLine();
      toUpper(input);
      if (input.size() == NUM_SIDES_ON_DIE) {
        diceList.add(input);
        break;
      } else {
        cout << "Invalid input" << endl;
      }
    }
  }
}

void ReadDiceFile(int boardSize, Vector<string>& diceList) {
  string fileName = "cubes" + IntegerToString(boardSize * boardSize) + ".txt";
  ifstream fileStream;
  fileStream.open(fileName.c_str());
  if (fileStream.fail()) {
    Error("Could not open file: " + fileName );
  }
  string line;
  while (!fileStream.eof()) {
    getline(fileStream, line);
    if (!line.empty()) {
      if (line.size() != NUM_SIDES_ON_DIE) 
        Error("Invalid line on dice file: " + line);
      diceList.add(line);
    }
  }
  fileStream.close();
  if (diceList.size() != (boardSize*boardSize))
    Error("Incorrect number of dice in file: " + fileName);
}

///////////////////////////////////////////////////////////////////////////////
//   Graphics Methods
///////////////////////////////////////////////////////////////////////////////

void LabelAllCubes(Grid<char>& board) {
  for (int row = 0; row < board.numRows(); row++) {
    for (int col = 0; col < board.numCols(); col++) {
      LabelCube(row, col, board[row][col]);
    }
  }
  UpdateDisplay();
}

void FlashWordPath(Vector<Location>& wordPath, 
                   double letterDelay, double wordDelay) {
   foreach (Location cell in wordPath) {
     HighlightCube(cell.row, cell.col, true);
     UpdateDisplay();
     Pause(letterDelay);
   }
   Pause(wordDelay);
   foreach (Location cell in wordPath) {
     HighlightCube(cell.row, cell.col, false);
   }
   UpdateDisplay();
}

///////////////////////////////////////////////////////////////////////////////
//   Game Play Methods
///////////////////////////////////////////////////////////////////////////////

void HumanPlays(GameState& gameState) {
  while (true) {
    cout << "Enter a word: ";
    string word = GetLine();
    toUpper(word);
    if (word.empty())
      break;
    if (!ValidateWord(word, gameState)) {
      PlayNamedSound("doh.wav");
      cout << "Not a valid word." << endl;
    } else {
      RecordWordForPlayer(word, Human);
      PlayNamedSound("kaching.wav");
      Vector<Location>& wordPath = gameState.GetCurrentPlayer().GetPathForWord(word);
      FlashWordPath(wordPath, 0.2, .5); 
    }
  }
}

void ComputerPlays(GameState& gameState) {
  FindWords(gameState);
  Set<string> foundWordSet;
  gameState.GetCurrentPlayer().GetFoundWords(foundWordSet);
  PlayNamedSound("tweetle.wav");
  //display the words and flash them.
  foreach (string word in foundWordSet) {
    RecordWordForPlayer(word, Computer);
    Vector<Location>& wordPath = gameState.GetCurrentPlayer().GetPathForWord(word);
    FlashWordPath(wordPath, 0.0, .05); 
  }
}

// entry point
int main() {
  Randomize();
  InitGraphics();
  DrawBoard(BOARD_SIZE, BOARD_SIZE);
  Welcome();
  SetSoundOn(YesNoPrompt("Would you like sound?"));
  Vector<string> diceList;
  if (YesNoPrompt("Do you need instructions?"))
    GiveInstructions();
  while (true) {
    if (YesNoPrompt("Do you want to enter your own board?"))
      ReadDiceFromUser(BOARD_SIZE, diceList);
    else
      ReadDiceFile(BOARD_SIZE, diceList);
    GameState gameState;
    PopulateBoard(gameState.board, diceList);
    LabelAllCubes(gameState.board);

    HumanPlays(gameState);
    //computer goes
    gameState.ComputersTurn();
    //computer finds words
    ComputerPlays(gameState);

    if (!YesNoPrompt("Play again?"))
      break;
    InitGraphics();
    DrawBoard(BOARD_SIZE, BOARD_SIZE);
  }
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
//   Utility methods
///////////////////////////////////////////////////////////////////////////////

void PopulateBoard(Grid<char>& board, Vector<string>& diceList) {
  for (int row = 0; row < board.numRows(); row++) {
    for (int col = 0; col < board.numCols(); col++) {
      int dieIndex = RandomInteger(0, diceList.size() -1);
      string die = diceList.getAt(dieIndex);
      diceList.removeAt(dieIndex);
      int sideIndex = RandomInteger(0, NUM_SIDES_ON_DIE -1);
      board[row][col] = die[sideIndex];
    }
  }
}


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
//   Test methods
///////////////////////////////////////////////////////////////////////////////

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

void TestPopulateBoard() {
  Vector<string> diceList;
  ReadDiceFile(4, diceList);
  InitGraphics();
  DrawBoard(4, 4);
  GameState gameState;
  PopulateBoard(gameState.board, diceList);
  LabelAllCubes(gameState.board);
  Vector<Location> wordPath;
  wordPath.add(Location(0, 0));
  wordPath.add(Location(1, 1));
  wordPath.add(Location(2, 2));
  wordPath.add(Location(3, 3));
  FlashWordPath(wordPath, 0.1, 0.25);
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

int mainTest() {
  TestFindWord();
  TestPopulateBoard();
  return 0;
}