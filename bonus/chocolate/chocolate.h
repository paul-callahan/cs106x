/**
* File: chocolate.h
* ---------------------
* Header file for Solving the n x m chocolate piece problem.

* Author: Paul Callahan
* For: CS106X bonus problem
* Date: October 24th, 2010
*/

#ifndef _chocolate_h
#define _chocolate_h

#include <vector>
#include "strutils.h"

const string INPUT_FILENAME = "sharing-chocolate.in";

/* enum: BreakageDirection
* -----------------------------
* enum representing the orientation of a break in the 
* chocolate bar.  i.e. Horizontal or vertical.
*/

enum BreakageDirection {
  Horizontal,
  Vertical,
};

/* Class: Piece
* -----------------------------
* Represents a piece of chocolate made up of segments.
* A piece can be broken horizontally or vertically
* along segments to make 2 new pieces.
*/
class Piece {
public:

  /* Constructor: 
  * -----------------------------
  *
  */
  Piece(): rows(-1), cols(-1){};

  /* Constructor: 
  * -----------------------------
  * Constructs a piece with the given number of rows
  * and columns.
  */
  Piece(int rows, int cols): rows(rows), cols(cols) {};

  /* Method: size
  * -----------------------------
  * returns the size of this piece (rows x cols)
  */
  int size() const { return rows * cols; };

  /* Method: numCols
  * -----------------------------
  * returns the number of columns this piece has
  */
  int numCols() const { return cols; };

  /* Method: numRows
  * -----------------------------
  * returns the number of rows this piece has
  */
  int numRows() const { return rows; };

  /* Method: breakPiece
  * --piecesOut[] - out array for pieces 
  * --dir - direction of the break - horizontal or vertical
  * --breakLoc - location of the break.  see below
  * -----------------------------
  * Breaks the piece into two pieces.  breakLoc represents where to break
  * the piece.  If breaking horizontally, 1 means break the piece between 
  * the 0th row and the 1st row.
  * No bounds checking is done.  Maybe add this as a todo.
  */
  void BreakPiece(Piece piecesOut[], BreakageDirection dir, int breakLoc) const;

  string toString() const;

  bool operator==(const Piece& other) const;
  bool operator<(const Piece& other) const;


private:
  int cols;  //number of columns this piece has
  int rows;  //number of rows this piece has
};

struct Memoizer {
  multimap<vector<int>, Piece> solutionMap;
  multimap<vector<int>, Piece> noSolutionMap;
};

/* Function: CanBreak
* -----------------------------
* Can a piece be broken at a given location in a given direction?
* probably should be a member of Piece.
*/
bool CanBreak(Piece& piece, BreakageDirection dir, int breakLoc);


/* Function: TryCombination
--shareList - list of target piece sizes that we want to break
*      the chocolate piece into
--piece - the piece we're trying to break into some number of pieces
* -----------------------------
* divide the list of share sizes into all possible subsets of size two.
* then for each, break the piece at all possible breakage locations.
* bail out and backtrack if a particular combination will not work.
* returns true if a piece can be broken up to match the share sizes in
* shareList.
*/
bool TryCombination(vector<int>& shareList, const Piece& piece,
                    Memoizer& memoizer);

/* Function: TryCombination
--piece - the piece we're trying to break into some number of pieces
--subshares1 - one subset of the list of share sizes we're trying to find
--subshares1 - the other subset of the list of share sizes we're trying to find
--numberOfBreaks - number of breaks possible for a given breakage direction
--breakDir - the direction we want to break the piece in
* -----------------------------
* overloaded helper for above TryCombination function.
*/
bool TryCombination(const Piece& piece, vector<int>& subshares1, vector<int>& subshares2, 
                    int numberOfBreaks, BreakageDirection breakDir, 
                    Memoizer& memoizer);


/* Function: FillVector
* -----------------------------
*  Use a bitmask to pick items from the source vector and put them into
* a destination vector.
*  For instance, if the bitmask is 100101, then put the 1st, 3nd and 6th
* item from the source vector into the destination vector.
* I stole the idea for this from Jon Bentley's "Programming Pearls"
*/
void FillVector(vector<int>& source, vector<int>& dest, int bitmask);


bool HasBeenSolved(const Piece& piece, vector<int>& shareList, Memoizer& memoizer);

bool HasNoSolution(const Piece& piece, vector<int>& shareList, Memoizer& memoizer);

bool IsInMultimap(const Piece& piece, vector<int>& shareList, multimap<vector<int>, Piece>& map);

int SumShareSizes(vector<int>& shareList);

/* Function: ReadDataLine
* -----------------------------
*  Read a line of space-separated digits from an ifstream
*  parse it and put it into a vector.
*
*/
int ReadDataLine(ifstream& fileInputStream, vector<int>& outData);

string ToString(const vector<int> v) {
  string out("[");
  for (int i = 0; i < v.size(); i++) {
    out.append(IntegerToString(v[i]));
    if (i < v.size() -1)
      out.append(",");
  }
  out.append("]");
  return out;
};

#endif
