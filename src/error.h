/** \file error.h
 * Exception handling.
 */

#ifndef ERROR_H
#define ERROR_H

#include <iostream>
#include <iomanip>
#include <sstream>
#include <memory>
#include <stdexcept>

using namespace std;

/** An instance records the position of a character in the source text. */
struct Errpos
{
   /** Construct a position object for reporting an error.
    * \param pLine points to the source code line.
    * \param lineNum is the number of the source code line.
    * \param charPos is the position of the character on the line.
    */
   Errpos(char* pLine = 0, int lineNum = 0, int charPos = 0)
         : pLine(pLine), lineNum(lineNum), charPos(charPos)
   {}

   /** The destructor doesn't do anything special. */
   ~Errpos()
   {
      // pLine should not be deleted when a position object is deleted.
   }

   /** Write an error position, displaying the whole line and a caret. */
   friend ostream & operator<<(ostream & os, const Errpos & ep)
   {
      if (ep.pLine)
      {
         os << '\n' << setw(4) << ep.lineNum << ": " << ep.pLine << "\n      ";
         for (int i = 0; i < ep.charPos; ++i)
            os << ' ';
         os << "^\n";
      }
      else
         os << endl;
      return os;
   }

   /** Pointer to source line. */
   char *pLine;

   /** Number of source line. */
   int lineNum;

   /** Position of character in source line. */
   int charPos;
};

/** An instance is used to determine how an error is handled.
 *  The options are to display the error, or to throw an exception.
 */
class ErrorReporter
{
public:

   /** Reporting modes. */
   enum REP_KIND { REPORT_ERROR, THROW_ERROR };

   /** Construct an instance with a particular kind. */
   ErrorReporter(REP_KIND kind) : kind(kind) {}

   /** Compare two instances using their kinds as the criterion for equality. */
   bool operator==(const ErrorReporter & other) const
   {
      return kind == other.kind;
   }
private:

   REP_KIND kind;
};

/** The object used to indicate that an error should be reported. */
const ErrorReporter REPORT = ErrorReporter(ErrorReporter::REPORT_ERROR);

/** The object used to indicate that an error should be thrown. */
const ErrorReporter THROW = ErrorReporter(ErrorReporter::THROW_ERROR);

/** An instance is an object that accumulates error information.
 *  Usage:  Error() << Message_1 << ... << Message_n << Action.
 *  Each message is an object that has an overload for ostream & operator<<.
 *  Action is either:
 *    'REPORT' to send an error message to cerr, or
 *    'THROW' to throw an exception that displays the message and aborts.
 */
class Error
{
public:

   /** Write an error component of any type. */
   template<typename T> Error & operator<<(const T & t)
   {
      os << t;
      return *this;
   }

   /** Report or throw an error. */
   Error & operator<<(const ErrorReporter & rep);

private:

   ostringstream os;
};

/** \return the number of errors found. */
int errorCount();

#endif
