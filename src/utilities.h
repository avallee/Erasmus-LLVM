/** \file utilities.h */

#ifndef UTILITIES_H
#define UTILITIES_H

/** Yucch!  Macros!  These are used in write.cpp and basicblocks.cpp
 * to format the output code.
 */

#define TAB0 code
#define TAB1 code << "   "
#define TAB2 code << "      "
#define TAB3 code << "         "
#define TAB4 code << "            "
#define TAB5 code << "               "
#define TAB6 code << "                  "

#include <cstdlib>
#include <string>

using namespace std;

/** \addtogroup utils */
//\{

/** Return the string with quotes around it. */
string quoteString(string s);

/** Return a character in quotes with explicit escape characters. */
string chr(char c);

/** Return string with escape characters. */
string str(string s);

/** Convert integer to string. */
string str(int n);

/** Show character with escaped characters. */
void showChar(ostream & os, char c, bool withConstructor = false);

/** Show string with escaped characters. */
void showString(ostream & os, string s, bool withConstructor = false);

/** Convert date to civilized format. */
string today();

//\}

#endif
