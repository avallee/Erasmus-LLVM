/** \file token.h
 * Declaration of class Token and supporting data.
 */

#ifndef TOKEN_H
#define TOKEN_H

#include "error.h"

#include <string>
#include <glibmm/ustring.h>

using namespace std;

/** \addtogroup enums */
//\{

enum TokenKind
{
   // Tokens with information
   IDENVAL,
//   BYTEVAL,
   INTVAL,
   DECIMALVAL,
//   FLOATVAL,
   CHARVAL,
   TEXTVAL,

   // Structures
   KW_PROTOCOL,
   KW_CELL,
   KW_CPP,
   KW_PROCESS,
   KW_PROCEDURE,

   // Type names
   KW_BOOL,
   KW_BYTE,
   KW_DECIMAL,
   KW_ENUM,
   KW_FLOAT,
   KW_INTEGER,
   KW_CHAR,
   KW_TEXT,
   KW_INPUT_FILE,
   KW_OUTPUT_FILE,
   KW_UNSIGNED,
   KW_VOID,

   // Built-in functions
   KW_ASSERTFUN,
   KW_BOOLFUN,
   KW_CHARFUN,
   KW_INTFUN,
   KW_FLOATFUN,
   KW_TEXTFUN,
   KW_FORMATFUN,
   KW_RANDOMFUN,

   // File Functions
   KW_FILE_OPEN,
   KW_FILE_CLOSE,
   KW_FILE_OK,
   KW_FILE_EOF,
   KW_FILE_READ,
   KW_FILE_WRITE,

   // Other keywords
   KW_ALIAS,   KW_AND,     KW_ANY,
   KW_CASES,   KW_COPY,
   KW_DIV,     KW_DO,      KW_DOMAIN,
   KW_ELIF,    KW_ELSE,    KW_END,  KW_EXECUTE,  KW_EXIT,
   KW_FAIR,    KW_FALSE,   KW_FOR,
   KW_IF,      KW_IMPORT,  KW_IN,   KW_INDEXES,
   KW_LOOP,    KW_LOOPSELECT,
   KW_MOD,
   KW_NOT,
   KW_OR,      KW_ORDERED,
   KW_RANDOM,   KW_RANGE,  KW_REM,
   KW_SELECT,   KW_SHARE,  KW_SKIP,    KW_START,   KW_STEP,   KW_SUCH,
   KW_THAT,     KW_THEN,   KW_THREAD,  KW_TO,      KW_TRUE,
   KW_UNTIL,
   KW_WHILE,

   // Operators
   OP_EQ,       // =
   OP_NE,       // ~= or != or <>
   OP_LT,       // <
   OP_LE,       // <=
   OP_GT,       // >
   OP_GE,       // >=
   OP_ASSIGN,   // :=
   OP_MOVE,     // <- 090226
   OP_AND_EQ,   // &=
   OP_PLUS,     // +
   OP_PLUS_EQ,  // +=
   OP_MINUS,    // -
   OP_MINUS_EQ, // -=
   OP_MUL,      // *
   OP_MUL_EQ,   // *=
   OP_DIV,      // /
   OP_DIV_EQ,   // /=
   OP_MOD,      // %
   OP_MOD_EQ,   // %=
   PERC_PERC,   // %%
   OP_DOTS,     // .. 071108
   OP_SHARP,    // #  071108
   OP_CAT,      // // 090201

   // Symbols
   LB,          // [
   RB,          // ]
   LC,          // {
   RC,          // }
   LP,          // (
   RP,          // )
   AT,          // @
   BAR,         // |
   ARROW,       // ->
   CARET,       // ^
   DOT,         // .
   COMMA,       // ,
   COLON,       // :
   SEMICOLON,   // ;
   QUEST_MARK,  // ?

   // End of file marker
   END_OF_FILE,

   // Dummy token used as a marker, terminator, etc.
   END_MARKER
};

//\}

/** An instance is a token created by the scanner. */
class Token
{
public:

   /** Default constructor constructs a dummy (useless) token.
    * All data members are public, but should only be used by scanner methods.
    */
   Token(Errpos ep, TokenKind kind = END_MARKER, Glib::ustring value = "", bool keyword = false)
         : ep(ep), kind(kind), value(value), keyword(keyword)
   {
   }

   /** Write token description to stream:
       used by the parser for reporting errors. */
   friend ostream & operator<<(ostream & os, const Token & tok)
   {
      return os << tok.ep;
   }

   /** Position in the input file. */
   Errpos ep;

   /** The kind of token. */
   TokenKind kind;

   /** The string actually read by the scanner. */
    Glib::ustring value;

   /** The token is a keyword. */
   bool keyword;
};

#endif
