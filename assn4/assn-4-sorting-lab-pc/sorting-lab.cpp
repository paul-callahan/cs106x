/**
* File: sorting-lab.cpp
* ---------------------
* Minimal framework needed to start testing.
*/

#include <iostream>
#include "genlib.h"
#include "vector.h"
#include "mysterysort.h"
#include "random.h"
#include <ctime>
#include "simpio.h"


void FillVector(Vector<int>& v, int count, bool sorted) {
  for (int i = 0; i < count; i++) {
    if (sorted)
      v.add(i);
    else
      v.add(RandomInteger(1, count));
  }
}

void SortTrial(void (*sortFuncp)(Vector<int>&, double), int inputSize, int numRuns, string functionName, bool sorted) {
  Vector<int> input;
  FillVector(input, inputSize, sorted);
  double start = double(clock()) / CLOCKS_PER_SEC;
  for (int run = 0; run < numRuns; run++) {
    sortFuncp(input, -1);
  }
  double finish = double(clock()) / CLOCKS_PER_SEC;
  double elapsed = finish - start;
  cout << "<tr>" << endl;
  cout << "<th>" << functionName << "</th>" << endl;
  //size
  cout << "<td>" << input.size() << "</td>" << endl;
  //sorted?
  cout << "<td>" << (sorted ? "yes" : "no" ) << "</td>" << endl;
  // number of runs
  cout << "<td>" << numRuns << "</td>" << endl;
  cout << "<td>" << elapsed << "</td>" << endl;
  //time per run
  cout << "<td>" << (elapsed/numRuns) << "</td>" << endl;
  cout << "</tr>" << endl;
}

void HTMLHeader() {

  cout << "<html><body><table>" << endl;
  cout << "<tr>" << endl;
  cout << "<th>Function Name</th>" << endl;
  cout << "<th>Input Size</th>" << endl;
  cout << "<th>Pre-Sorted?</th>" << endl;
  cout << "<th>Number of Runs</th>" << endl;
  cout << "<th>Total Time</th>" << endl;
  cout << "<th>Time per Run</th>" << endl;
  cout << "</tr>" << endl;
}

void HTMLFooter() {

  cout << "</table></body></html>" << endl;

}

void RunBatch(void (*sortFuncp)(Vector<int>&, double), string FunctionName, int start, int end, int step) {


  for (int i = start; i <= end; i+= step) {
    SortTrial(sortFuncp, i, 1000, FunctionName, false);
  }

  for (int i = start; i <= end; i+= step) {
    SortTrial(sortFuncp, i, 1000, FunctionName, true);
  }

}



int main()
{
  Randomize();

  HTMLHeader();
  RunBatch(MysterySort1, "MysterySort1", 1000, 10000, 1000);
  RunBatch(MysterySort2, "MysterySort2", 100, 1000, 100);
  RunBatch(MysterySort3, "MysterySort3", 1000, 10000, 1000);
  RunBatch(MysterySort4, "MysterySort4", 100, 1000, 100);
  RunBatch(MysterySort5, "MysterySort5", 1000, 10000, 1000);
  HTMLFooter();


  return 0;
}
