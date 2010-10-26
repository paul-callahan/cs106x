/**
* File: chocolate.cpp
* ---------------------
* Solve the n x m chocolate piece problem.

* Author: Paul Callahan
* For: CS106X bonus problem
* Date: October 24th, 2010
*/

#include <iostream>
#include <fstream>
#include "genlib.h"
#include "random.h"
#include "simpio.h"
#include "scanner.h"
#include "strutils.h"
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include "math.h"
#include "chocolate.h"

int trycalls_x = 0;

/* Function: main
* -----------------------------
*  read piece sizes and share-size lists from a file sharing-chocolate.in
*  for each piece size and share list, determine if the piece can be
*  broken up into that many subpieces.  print yes or no, accordingly.
*
*/
int main() {
  ifstream fileInputStream;
  fileInputStream.open(INPUT_FILENAME.c_str());
  vector<int> pieceDimensions;
  vector<int> shareList;
  while (ReadDataLine(fileInputStream, pieceDimensions) &&
    ReadDataLine(fileInputStream, shareList)) {
      Memoizer memoizer;
      Piece piece(pieceDimensions[0], pieceDimensions[1]);
      std::sort(shareList.begin(), shareList.end());
      // cout << "piece: " << piece.toString() << " share list: " << ToString(shareList) << endl;
      bool canDivide = TryCombination(shareList, piece, memoizer);
      cout <<  (canDivide ? "yes" : "no") << endl;
      cout << IntegerToString(trycalls_x) << endl;
      trycalls_x = 0;
  }
  
  return 0;
}

/* Method: Piece::BreakPiece
* -----------------------------
*/
void Piece::BreakPiece(Piece piecesOut[], BreakageDirection dir, int breakLoc) const {
  switch (dir){
  case Horizontal:
    piecesOut[0] = Piece(breakLoc, numCols());
    piecesOut[1] = Piece(numRows() - breakLoc, numCols());
    break;  
  case Vertical:
    piecesOut[0] = Piece(numRows(), breakLoc);
    piecesOut[1] = Piece(numRows(), numCols() - breakLoc);
    break;
  }
}

bool Piece::operator==(const Piece& other) const {
  return other.numCols() == numCols() && other.numRows() == numRows();
}

bool Piece::operator<(const Piece& other) const {
  return other.numRows() < numRows() ||
    (other.numRows() == numRows() && other.numCols() < numCols());
}

string Piece::toString() const { 
  string stringOut("[");
  stringOut.append(IntegerToString(numRows()));
  stringOut.append(",");
  stringOut.append(IntegerToString(numCols()));
  stringOut.append("]");
  return stringOut;
}

/* Function: CanBreak
* -----------------------------
* see declaration
*/
bool CanBreak(Piece& piece, BreakageDirection dir, int breakLoc) {
  switch (dir){
  case Horizontal:
    return piece.numCols() > breakLoc;
  case Vertical:
    return piece.numRows() > breakLoc;
  }
  //to shut the compiler up:
  return false;
}

/* Function: FillVector
* -----------------------------
* see declaration
*/
void FillVector(vector<int>& source, vector<int>& dest, int bitmask) {
  for (int i = 0; i < source.size(); i++) {
    int shifted = bitmask >> i;
    if (shifted & 1 == 1)
      dest.push_back(source[i]);
  }
}

/* Function: TryCombination
* -----------------------------
* see declaration
*/
bool TryCombination(vector<int>& shareList, const Piece& piece, Memoizer& memoizer) {
  trycalls_x++;
  //base case
  //whittled down to a single share, see if it matches the size of the piece.
  if (shareList.size() == 1) {
    if (shareList[0] == piece.size()) {
      //cout << "share: " << shareList[0] << " matched" << endl;
      return true;
    } else {
      return false;
    } 
  }
  
  //no way it will work if the sum of the share sizes is not equal to the size of the
  //piece of chocolate.
  if (SumShareSizes(shareList) != piece.size())
    return false;

  // has this subproblem been solved already?
  if (HasBeenSolved(piece, shareList, memoizer))
    return true;

  // has this subproblem been found to have no solution already?
  if (HasNoSolution(piece, shareList, memoizer))
    return false;


  // break the list of shares, shareList, into two lists.
  // do this for every non zero permutation
  int maxbits = (int) pow(2.0, (int)shareList.size());
  for (int bitmask = 1; bitmask < maxbits -1; bitmask++) {
    vector<int> subshares1;
    vector<int> subshares2;
    FillVector(shareList, subshares1, bitmask);  
    FillVector(shareList, subshares2, ~bitmask);

    // cout << "max bits: " << maxbits << "bitmask: " << bitmask << endl;
    // cout << "piece rows: " << piece.numRows() << " piece cols: " << piece.numCols() << endl;
    // PrintVector("sub1:", subshares1);
    // PrintVector("sub2:", subshares2);
    // GetLine();

    // loop over vertical breaks
    if (TryCombination(piece, subshares1, subshares2, piece.numCols(), Vertical, memoizer)) {
      memoizer.solutionMap.insert(pair<vector<int>, Piece>(shareList, piece));
      memoizer.solutionMap.insert(pair<vector<int>, Piece>(shareList, Piece(piece.numCols(), piece.numRows())));
      int working = memoizer.solutionMap.count(shareList);
      return true;
    }
    //repeat with horizontal breaks
    if (TryCombination(piece, subshares1, subshares2, piece.numRows(), Horizontal, memoizer)) {
      memoizer.solutionMap.insert(pair<vector<int>, Piece>(shareList, piece));
      memoizer.solutionMap.insert(pair<vector<int>, Piece>(shareList, Piece(piece.numCols(), piece.numRows())));
      return true;
    }
  }
  memoizer.noSolutionMap.insert(pair<vector<int>, Piece>(shareList, piece));
  memoizer.noSolutionMap.insert(pair<vector<int>, Piece>(shareList, Piece(piece.numCols(), piece.numRows())));
  return false;
}

/* Function: TryCombination
* -----------------------------
* see declaration
*/
bool TryCombination(const Piece& piece, vector<int>& subshares1, vector<int>& subshares2, 
                    int numberOfBreaks, BreakageDirection breakDir,
                    Memoizer& memoizer) {
  //loop over every breakage position for a given breakage direction
  for (int breakLoc = 1; breakLoc < numberOfBreaks; breakLoc++) {
    Piece subPieces[2];
    piece.BreakPiece(subPieces, breakDir, breakLoc);    
    if (TryCombination(subshares1, subPieces[0], memoizer) &&
      TryCombination(subshares2, subPieces[1], memoizer)) {
        //memoization stuff ?
        return true;
    }
  }
  return false;
}

bool HasBeenSolved(const Piece& piece, vector<int>& shareList, Memoizer& memoizer) {
  return IsInMultimap(piece, shareList, memoizer.solutionMap);
}

bool HasNoSolution(const Piece& piece, vector<int>& shareList, Memoizer& memoizer) {
  return IsInMultimap(piece, shareList, memoizer.noSolutionMap);
}

bool IsInMultimap(const Piece& piece, vector<int>& shareList, multimap<vector<int>, Piece>& map) {
  if (map.count(shareList) > 0) {
    for (multimap<vector<int>, Piece>::iterator it = map.lower_bound(shareList);
      it != map.upper_bound(shareList); it++) {
        if (piece == it->second)
          return true;
    }
  }
  return false;
}

int SumShareSizes(vector<int>& shareList) {
  int total = 0;
  for (int i = 0; i < shareList.size(); i++) {
    total += shareList[i];
  }
  return total;
}

/* Function: ReadDataLine
* -----------------------------
* see declaration
*/
int ReadDataLine(ifstream& fileInputStream, vector<int>& outData) {
  outData.clear();
  Scanner scanner;
  scanner.setSpaceOption(Scanner::IgnoreSpaces);
  string dataLine;
  getline(fileInputStream, dataLine);
  if (fileInputStream.fail())
    return false;
  scanner.setInput(dataLine);
  while (scanner.hasMoreTokens()) {
    string toke = scanner.nextToken();
    int val = StringToInteger(toke);
    outData.push_back(val);
  }
  return true;
}




///////////////////////////////////////////////////////////////////////////////
//           Test methods
///////////////////////////////////////////////////////////////////////////////

void PrintVector(string msg, vector<int>& v) {
  cout << msg << "[";
  for (int i = 0; i < v.size(); i++) {
    cout << v[i] << ",";
  }
  cout << "]" << endl;
}

int TestTryCombination() {

  Piece piece(4, 3);
  vector<int> v;
  v.push_back(1);
  v.push_back(2);
  v.push_back(3);
  v.push_back(6);

  Memoizer memoizer;
  multimap<Piece, vector<int> > xxxx;
  bool canBreak = TryCombination(v, piece, memoizer);
  cout << "can break? " << (canBreak ? "yes" : "no") << endl;

  //111
  return 0;
}

int TestSubListPermutions() {

  vector<int> v;
  v.push_back(1);
  v.push_back(2);
  //v.push_back(3);
  //v.push_back(6);


  int maxbits = (int) pow(2.0, (int)v.size());
  for (int i = 1; i < maxbits; i++) {
    vector<int> dest;
    FillVector(v, dest, i);
    PrintVector("a", dest);
    dest.clear();
    FillVector(v, dest, ~i);
    PrintVector("b", dest);
  }

  return 0;
}
