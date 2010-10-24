/**
* File: sorting-lab.cpp
* ---------------------
* Minimal framework needed to start testing.
*/

#include <iostream>
#include "genlib.h"
#include "vector.h"
#include "random.h"
#include <ctime>
#include "simpio.h"
#include <vector>
#include "math.h"


enum BreakageDirection {
  Horizontal,
  Vertical,
};

class Piece {


public:
  Piece(): rows(-1), cols(-1){};
  Piece(int rows, int cols): rows(rows), cols(cols) {};

  int size() { return rows * cols; };
  int cols;
  int rows;
};

void BreakPiece(Piece piecesOut[], Piece thisP, BreakageDirection dir, int breakLoc) {
  switch (dir){
  case Horizontal:
    piecesOut[0] = Piece(breakLoc, thisP.cols);
    piecesOut[1] = Piece(thisP.rows - breakLoc, thisP.cols);
    break;  
  case Vertical:
    piecesOut[0] = Piece(thisP.rows, breakLoc);
    piecesOut[1] = Piece(thisP.rows, thisP.cols - breakLoc);
    break;
  }
}

bool CanBreak(Piece piece, BreakageDirection dir, int breakLoc) {
  switch (dir){
  case Horizontal:
    return piece.cols > breakLoc;
  case Vertical:
    return piece.rows > breakLoc;
  }
}

bool Broken(Piece& piece, int target) {
  //
  if (piece.rows * piece.cols == target)
    return true;
  if (piece.rows * piece.cols < target)
    return false;
  for (int breakLocVert = 1; breakLocVert < piece.rows; breakLocVert++) {
    Piece subPieces[2];
    BreakPiece(subPieces, piece, Vertical, breakLocVert);
    Broken(subPieces[0], target);
    Broken(subPieces[1], target);
  }
  return false; 
}



//record to represent a breakage action.
//
struct Break {
  BreakageDirection dir;
  int breakIndex;
  int pieceRows;
  int pieceCols;
};

void FillVector(vector<int>& source, vector<int>& dest, int bitmask) {
  for (int i = 0; i < source.size(); i++) {
    int shifted = bitmask >> i;
    if (shifted & 1 == 1)
      dest.push_back(source[i]);
  }
}

bool TryCombination(vector<int> shareCombo, Piece piece) {
  //whittled down to a single share, see if it matches the size of the piece.
  if (shareCombo.size() == 1) {
    if (shareCombo[0] == piece.size()) {
      return true;
    } else {
      return false;
    } 
  }
  // break the list of shares, shareCombo, into two lists.
  //do this for every non zero permutation
  for (int subcomboInd = 1; subcomboInd < shareCombo.size(); subcomboInd++) {
    vector<int> subcombo1;
    vector<int> subcombo2;
    FillVector(shareCombo, subcombo1, subcomboInd);  
    FillVector(shareCombo, subcombo2, ~subcomboInd);    
    for (int breakLocVert = 1; breakLocVert < piece.rows; breakLocVert++) {
      Piece subPieces[2];
      BreakPiece(subPieces, piece, Vertical, breakLocVert);    
      if (TryCombination(subcombo1, subPieces[0]) &&
        TryCombination(subcombo2, subPieces[1])) {
          //memoization stuff
          return true;
      }
    }
    //repeat with horizontal breaks
  }
  return false;
}



void PrintVector(vector<int> v) {
  cout << "[";
    for (int i = 0; i < v.size(); i++) {
      cout << v[i] << ",";
    }
    cout << "]" << endl;
}
int main() {

  vector<int> v;
  v.push_back(1);
  v.push_back(2);
  v.push_back(3);

  //111
  int maxbits = (int) pow(2.0, (int)v.size());
  for (int i = 1; i < maxbits; i++) {
    vector<int> dest;
    FillVector(v, dest, i);
    PrintVector(dest);
  }



  return 0;
}