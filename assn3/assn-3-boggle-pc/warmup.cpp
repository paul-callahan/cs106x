/* File: warmup.cpp
* ----------------
* Author: Paul Callahan
* Date: 10/11/2010
* Grader: Alex Quach
* Section leader: Caitlin Colgrove
*
* Put your implementations of the warmup problems in this file.
* 
*/

#include "genlib.h"
#include "sound.h"
#include "extgraph.h"
#include "simpio.h"
#include "lexicon.h"
#include "scanner.h"
#include <iostream>


/*
Within a project there can only be a single 'main()' function.  As such,
in order to test your warmup problem functions, uncomment out 'main()' below, and comment
out 'main()' in boggle.cpp.
*/

const string MATCHED_PARENS = "()";
const string MATCHED_BRACKETS = "[]";
const string MATCHED_BRACES = "{}";

/* 
 * Function: EvalInfixExp
 * -------------------
 * Takes an infix arithmetic expression and computes and returns the results.
 */ 
int EvalInfixExp(string expString);
/* 
 * Function: EvalInfixExp
 * -------------------
 * Recursion helper for EvalInfixExp(string).  
 */ 
int EvalInfixExp(Scanner& scanner);
/* 
 * Function: IsBalanced
 * -------------------
 *  Checks to see if the parens/braces/brackets are balanced in a 
 * given string.  returns true if balanced.
 */ 
bool IsBalanced(string str);
/* 
 * Function: PerformOperation
 * -------------------
 * Performs arithmetic operation on the left and right operands.
 */ 
int PerformOperation(int left, char operat0r, int right);
/* 
 * Function: IsOperator
 * -------------------
 * returns true if char passed in is a + - / or *
 */ 
bool IsOperator(char c);

/* 
 * Function: TestIsBalanced
 * -------------------
 *  Test case for IsBalanced.  Takes the string to check and an expected
 *  value and runs it through IsBalanced and prints the results.
 */ 
void TestIsBalanced(string testStr, bool expectedValue);
/* 
 * Function: TestEvalInfixExp
 * -------------------
 *  Test case for EvalInfixExp.  Takes the string to evaluate and an expected
 *  value and runs it through EvalInfixExp and prints the results.
 */ 
void TestEvalInfixExp(string exp, int expectedValue);



int mainHide(){

  TestEvalInfixExp("(1 + 2)", 3);
  TestEvalInfixExp("(1 + (6* 7))", 3);
  TestEvalInfixExp("4", 4);
  TestEvalInfixExp("(25 - (2 * 10))", 5);

  TestIsBalanced("[(){}]", true);
  TestIsBalanced("[{{{((([])))}}}]", true);
  TestIsBalanced("[{{{((([])))}}}]", true);
  TestIsBalanced("()[{{{((([])))}}}][]", true);
  TestIsBalanced("(([])", false);
  TestIsBalanced(")(", false);
  TestIsBalanced("{(})", false);

  return 0;
}

int EvalInfixExp(string expString) {
  Scanner scanner;
  scanner.setInput(expString);
  return EvalInfixExp(scanner);
}

int EvalInfixExp(Scanner& scanner) {
  char oper = 0;
  int leftOperand = 0;
  int rightOperand = 0;
  while (scanner.hasMoreTokens()) {
    string next = scanner.nextToken();
    if (isdigit(next[0]) && oper == 0)
      leftOperand = StringToInteger(next);
    else if (isdigit(next[0]))
      rightOperand = StringToInteger(next);
    else if (IsOperator(next[0]))
      oper = next[0];
    else if (next == "(")
      rightOperand = EvalInfixExp(scanner);
    else if (next == ")")
      break;
  }
  //base case essentially parses the single number, falls out of the
  //while loop and the single number gets added to 0...
  return PerformOperation(leftOperand, oper, rightOperand);
}

bool IsOperator(char c) {
  return (c > 41 && c < 46);
}

int PerformOperation(int left, char operat0r, int right) {
  switch (operat0r) {
    case 0:
    case '+':
      return left + right;
    case '-':
      return left - right;
    case '*':
      return left * right;
    case '/':
      return left / right;
    default:
      throw ("Invalid operator: " + operat0r);
  }
}

bool IsBalanced(string str) {
  //cout << "->" << str << "<--" << endl;
  //base case.
  if (str.empty()) return true;

  for (int i = 0; i < str.size() -1; i++) {
    string charPair(&str[i], 2);
    if (charPair == MATCHED_BRACES ||
      charPair == MATCHED_BRACKETS ||
      charPair == MATCHED_PARENS) {
        str = str.substr(0, i) + str.substr(i +2);
        return IsBalanced(str);
    }
  }
  //found no matched pairs.
  return false;
}

//This one isn't efficient because it potentially rescans the 
//string up to 3 times.
bool IsBalancedFirstAttempt(string str) {
  cout << "->" << str << "<--" << endl;
  //base case.
  if (str.empty()) return true;

  int foundPos = str.find(MATCHED_BRACES);
  if (foundPos == str.npos) 
    foundPos = str.find(MATCHED_BRACKETS);
  if (foundPos == str.npos)
    foundPos = str.find(MATCHED_PARENS);

  //can't shrink the string down, fail
  if (foundPos == str.npos)
    return false;
  str = str.substr(0, foundPos) + str.substr(foundPos +2);
  return IsBalanced(str);
}

void TestIsBalanced(string testStr, bool expectedValue) {
  bool balanced = IsBalanced(testStr);
  cout << testStr << " balanced? " << balanced 
    <<  " expected: " << expectedValue <<  endl;
  //assert balanced == expectedValue;
}

void TestEvalInfixExp(string exp, int expectedValue) {
  int evaled = EvalInfixExp(exp);
  cout << exp << " eval'd: " << evaled 
    <<  " expected: " << expectedValue <<  endl;
  //assert evaled == expectedValue;
}