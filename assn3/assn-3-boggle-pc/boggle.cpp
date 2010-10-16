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
    int row, col, ordinal;

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
    Set<string> foundWordsOtherPlayer;

public:
    UserState() {
        score = 0;
    }

    void AddToPath(Location& location) {
        currentPath.add(location);
        orderedPath.add(location);
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

    void AddFoundWord(string word) {
        if (!foundWordsOtherPlayer.contains(word)) {
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

Set<Location> GetValidPositions(const Grid<string>& board, const Location& currentGridLoc, const Set<Location>& currentPath);
void FindWords(string prefix, Grid<string>& board, Set<Location>& validPositions, UserState& userState);
void FindWords(Grid<string>& board, UserState& userState);

void FindWords(Grid<string>& board, UserState& userState) {
    for (int row = 0; row < board.numRows(); row++) {
        for (int col = 0; col < board.numCols(); col++) {
            Location startLocation(row, col);
            Set<Location> startPath;
            startPath.add(startLocation);
            userState.ClearCurrentPath();
            FindWords("", board, startPath, userState);
        }
    }
}

void FindWords(string prefix, Grid<string>& board, Set<Location>& validPositions, UserState& userState) {
    static Lexicon lexicon("lexicon.dat");
    //no more valid positions to check
    //if (validPositions.isEmpty()) 
    //    return;    

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
            FindWords(candidate, board, nextLetterPositions, userState);
        }
    }
    //dead-end...  roll back our path by one
    userState.RollbackCurrentPath();
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

int OrderedLocationComparator(Location locOne, Location locTwo) {
    int diff = locOne.ordinal - locTwo.ordinal;
    if (diff == 0)
        return 0;
    return diff/abs(diff);
}

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
    UserState userState;

    Grid<string> board(4, 4);

    board[0][0] = "C";
    board[0][1] = "A";
    board[0][2] = "R";
    board[0][3] = "E";

    board[1][0] = "O";
    board[1][1] = "Z";
    board[1][2] = "O";
    board[1][3] = "I";

    board[2][0] = "D";
    board[2][1] = "F";
    board[2][2] = "L";
    board[2][3] = "Qu";

    board[3][0] = "A";
    board[3][1] = "E";
    board[3][2] = "P";
    board[3][3] = "T";

    FindWords(board, userState);

    cout<< userState.GetScore() << endl;

}

int main() {
    TestFindWord();
    return 0;
}