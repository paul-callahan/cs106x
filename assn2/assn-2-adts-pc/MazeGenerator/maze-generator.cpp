/**
 * File: maze-generator.cpp
 * ------------------
 * Name: Paul Callahan (paul.callahan@gmail.com)
 * Section Leader Name: 
 * Grader: Alex Quach
 * Assignment #2, part iii.
 * This program creates a maze and displays it based on a
 * simplified Kruzal algorith.  It creates walls for a maze
 * and removes them one by one as long as the wall borders
 * two distinct chambers.
 */
#include "foreach.h"
#include <iostream>
#include "set.h"
#include "vector.h"
#include <map>
#include "genlib.h"
#include "extgraph.h"
#include "simpio.h"
#include "random.h"
#include "maze-types.h"
#include "maze-graphics.h"

//Pause for animating wall removal
const double DRAWING_DELAY = 0.0;

//minimum maze dimension
const int MIN_DIM = 7;

//maximum maze dimension
const int MAX_DIM = 50;

/**
 * Function: 
 * --------------------------
 *  Creates a maze with specified dimension, and displays it
 */
void RunMazeCreation(int dim);

/**
 * Function: IsCellInBounds
 * --------------------------
 *  Checks to see if the coordinates of a cell are inside the bounds 
 *  of the maze.
 *
 */
bool IsCellInBounds(int row, int col, int dim);

/**
 * Function: 
 * --------------------------
 *  Creates and adds two walls for a given cell.  Ensures no out of bounds
 *  walls are created.
 */
void AddWalls(Vector<wall>& wallVector, const cell& currentCell, int row, 
              int col, int dim);

/**
 * Function: 
 * --------------------------
 *  Iterates throw the list of walls and removes them one by one if the wall is not 
 *  within one chamber.
 */
void RemoveWalls(Vector<wall>& wallVector, map<cell, Set<cell> >& cellsToChamberMap);
/**
 * Function: 
 * --------------------------
 *  After two chambers have been merged, this function remaps each cell inside
 *  the "new" chamber to point to the chamber, inside the cellsToChamberMap.
 */
void RemapChamberCells(Set<cell>& chamber, 
                       map<cell, Set<cell> >& cellsToChamberMap);

/**
 * Function: 
 * --------------------------
 *  Randomly suffles the list of walls by swapping each element with a random element
 * in the list.
 *
 */
void ShuffleWalls(Vector<wall>& wallVector);

/**
 * Function: 
 * --------------------------
 *  Asks user for dimension of the maze. 
 *
 */
int PromptUserForDimension();

/**
 * Function: 
 * --------------------------
 *  Debug function to print out a wall.
 *
 */
void PrintWall(const cell& c, const wall& w);


//Make me work!
int main() {
  Randomize();
	InitMazeGraphics();
  while (true) {
    int dim = PromptUserForDimension();
    if (dim == 0) break;
    if (dim < MIN_DIM || dim > MAX_DIM) {
      cout << "Dimension must be between " << MIN_DIM << "and " 
        << MAX_DIM << " inclusive." << endl;    
    } else {
      RunMazeCreation(dim);
    }
  }
	return 0;
}

void RunMazeCreation(int dim) {
  SetMazeDimension(dim);
  DrawMazeBorder();

  Vector<wall> wallVector(4 * dim * dim);
  // cell is the key
  map<cell, Set<cell> > cellsToChamberMap; // stl maps don't take size hints ?

  for (int row = 0; row < dim; row++) {
    for (int col = 0; col < dim; col++) {
      cell currentCell;
      currentCell.row = row;
      currentCell.col = col;
      AddWalls(wallVector, currentCell, row, col, dim);
      Set<cell> chamber;
      chamber.add(currentCell);
      cellsToChamberMap[currentCell] = chamber;
    }
  }
  UpdateDisplay();
  ShuffleWalls(wallVector);
  RemoveWalls(wallVector, cellsToChamberMap);


}

void AddWalls(Vector<wall>& wallVector, const cell& currentCell, int row, 
              int col, int dim) {
  //row and col are the cell's row and col.
  for (int dRow = 0; dRow <= 1; dRow ++) {
    for (int dCol = 0; dCol <= 1; dCol++) {
      if (dRow == 1 && dCol == 1) continue;  //no "corner" cells
      if (dRow == 0 && dCol == 0) continue;  //skip current cell
      //don't even bother adding the wall if one of the cells does not exist.
      if (IsCellInBounds(row + dRow, col + dCol, dim)) {
        cell otherCell;
        otherCell.row = row + dRow;
        otherCell.col = col + dCol;
        wall w;
        w.one = currentCell;
        w.two = otherCell;
        DrawWall(w);
        wallVector.add(w);
      }
    }
  }
}

void RemoveWalls(Vector<wall>& wallVector, map<cell, Set<cell> >& cellsToChamberMap) {
  foreach (wall w in wallVector) {
    //look up the chambers that w.one and w.two are in
    Set<cell> chamber1 = cellsToChamberMap[w.one];
    Set<cell> chamber2 = cellsToChamberMap[w.two];
    //if the wall is within the same chamber, both these chambers
    //will be the same.  = comparison of Sets did not work... 
    //but isSubsetOf did!
    if (!chamber1.isSubsetOf(chamber2))  {
      chamber1.unionWith(chamber2);
      RemapChamberCells(chamber1, cellsToChamberMap);
      RemoveWall(w);
      UpdateDisplay();
      Pause(DRAWING_DELAY);
    }
  }
}

void RemapChamberCells(Set<cell>& chamber, map<cell, Set<cell> >& cellsToChamberMap) {
  foreach (cell c in chamber) {
    cellsToChamberMap[c] = chamber;
  }
}
void ShuffleWalls(Vector<wall>& wallVector) {
  for (int i = 0; i < wallVector.size(); i++) {
    int swapIndex = RandomInteger(0, wallVector.size() -1);
    wall tmpWall = wallVector[i];
    wallVector[i] = wallVector[swapIndex];
    wallVector[swapIndex] = tmpWall;
  }
}

bool IsCellInBounds(int row, int col, int dim) {
  if (row < 0 || row >= dim) return false;
  if (col < 0 || col >= dim) return false;
  return true;
}

int PromptUserForDimension() {
  cout << "Enter desired maze dimension, from 7 to 50, or enter 0 to exit" 
    << endl;;
  return GetInteger();
}


///////////////////////////////////////////////////////////////////////////////
//                           Test Code                                       //
///////////////////////////////////////////////////////////////////////////////


void PrintWall(const cell& c, const wall& w) {
  cout << "cell=[" << c.row << "," << c.col << "]" << "--w.one=[" << w.one.row 
    << "," << w.one.col << "]" << " w.two=[" 
    << w.two.row << "," << w.two.col << "]" << endl; 
}
