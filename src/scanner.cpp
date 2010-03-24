/** \file scanner.cpp
 * Implementaion of scnner.
 */

#include "error.h"
#include "scanner.h"

#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

Scanner::Scanner()
{
   // Types
   keywords["Bool"]        = KW_BOOL;
   keywords["Boolean"]     = KW_BOOL;
   keywords["Byte"]        = KW_BYTE;
   keywords["Decimal"]     = KW_DECIMAL;
   keywords["enum"]        = KW_ENUM;
   keywords["enumeration"] = KW_ENUM;
   keywords["Float"]       = KW_FLOAT;
   keywords["Int"]         = KW_INTEGER;
   keywords["Integer"]     = KW_INTEGER;
   keywords["Char"]        = KW_CHAR;
   keywords["Character"]   = KW_CHAR;
   keywords["Text"]        = KW_TEXT;
   keywords["InputFile"]   = KW_INPUT_FILE;
   keywords["OutputFile"]  = KW_OUTPUT_FILE;
   keywords["Void"]        = KW_VOID;
   keywords["unsigned"]    = KW_UNSIGNED;

   // Keywords
   keywords["alias"]      = KW_ALIAS;
   keywords["and"]        = KW_AND;
   keywords["any"]        = KW_ANY;

   keywords["cases"]      = KW_CASES;
   keywords["cpp"]        = KW_CPP;
   keywords["cell"]       = KW_CELL;
   keywords["copy"]       = KW_COPY;

   keywords["div"]        = KW_DIV;
   keywords["do"]         = KW_DO;
   keywords["domain"]     = KW_DOMAIN;

   keywords["elif"]       = KW_ELIF;
   keywords["else"]       = KW_ELSE;
   keywords["end"]        = KW_END;
   keywords["execute"]    = KW_EXECUTE;
   keywords["exit"]       = KW_EXIT;

   keywords["fair"]       = KW_FAIR;
   keywords["false"]      = KW_FALSE;
   keywords["for"]        = KW_FOR;

   keywords["if"]         = KW_IF;
   keywords["import"]     = KW_IMPORT;
   keywords["in"]         = KW_IN;
   keywords["indexes"]    = KW_INDEXES;

   keywords["loop"]       = KW_LOOP;
   keywords["loopselect"] = KW_LOOPSELECT;

   keywords["mod"]        = KW_MOD;

   keywords["not"]        = KW_NOT;

   keywords["or"]         = KW_OR;
   keywords["ordered"]    = KW_ORDERED;

   keywords["procedure"]  = KW_PROCEDURE;
   keywords["process"]    = KW_PROCESS;
   keywords["protocol"]   = KW_PROTOCOL;

   keywords["random"]     = KW_RANDOM;
   keywords["range"]      = KW_RANGE;
   keywords["rem"]        = KW_REM;

   keywords["select"]     = KW_SELECT;
   keywords["share"]      = KW_SHARE;
   keywords["skip"]       = KW_SKIP;
   keywords["start"]      = KW_START;
   keywords["step"]       = KW_STEP;
   keywords["such"]       = KW_SUCH;

   keywords["that"]       = KW_THAT;
   keywords["then"]       = KW_THEN;
   keywords["thread"]     = KW_THREAD;
   keywords["to"]         = KW_TO;
   keywords["true"]       = KW_TRUE;

   keywords["until"]      = KW_UNTIL;

   keywords["while"]      = KW_WHILE;
}

Scanner::~Scanner()
{
   for (vector<char*>::iterator it= source.begin(); it != source.end(); ++it)
      delete *it;
}

/** Read characters from an input stream, stopping at terminator
 *  Non-graphic characters are encoded with escapes.
 * \param pc points to the ' or " at the start of the string.
 * \param term is the terminating character, ' or ".
 * \param line points to the beginning of the current line,
 *        and is used for reporting illegal characters.
 */
string readChars(char * & pc, char term, char *line)
{
   string result;
   while (true)
   {
      ++pc;
      if (*pc == term)
         return result;
      else if (*pc == '\0')
      {
         cerr << "Warning: line break in character or text literal.\n" << line;
         return result;
      }
      else if (*pc == '\\')
      {
         ++pc;
         switch (*pc)
         {
            case 'n':
               result += '\n';
               break;
            case 't':
               result += '\t';
               break;
            case '\'':
               result += '\'';
               break;
            case '\"':
               result += '\"';
               break;
            case '\\':
               result += '\\';
               break;
            default:
               cerr << "Warning: illegal character following \\.\n" << line;
         }
      }
      else
         result += *pc;
   }
}

/** Scan a file and generate a list of tokens.
 * \param filename is the name of the file to read from.
 * \param tokens is a list to which tokens will be appended.
 * \throw If the file cannot be opened, report an error and throw an exception.
 */
void Scanner::scanFile(string filename, list<Token> & tokens)
{
   ifstream ifs(filename.c_str());
   if (!ifs)
      Error() << "failed to open input file '" << filename << "'.\n" << THROW;

   const int BUFSIZE = 1000;
   char buffer[BUFSIZE];
   char *pc;
   source.clear(); // Delete stuff from previous file

   while (ifs.getline(buffer, BUFSIZE))
   {
       // Big memory leak here?
       char* pc = new char[strlen(buffer) + 1];
      strcpy(pc, buffer);
      source.push_back(pc);
   }
   ifs.close();

   vector<char*>::const_iterator srcit = source.begin();
   if (srcit == source.end())
      Error() << "input file '" << filename << "' is empty.\n" << THROW;

   pc = *srcit - 1;
   while (true)
   {
      int state = 0;
      do
      {
         ++pc;
         switch (state)
         {
            case 0: // Initial state: no context
               if (*pc == '\0')
               {
                  ++srcit;
                  if (srcit == source.end())
                     return;
                  pc = *srcit - 1;
               }
               else if (*pc == '-')
                  state = 1;
               else if (!isspace(*pc))
                  state = 3;
               break;

            case 1: // Seen '-': might be a comment
               if (*pc == '-')
                  state = 2;
               else
               {
                  --pc;
                  *pc = '-';
                  state = 3;
               }
               break;

            case 2: // Seen '--': read comment
               ++srcit;
               if (srcit == source.end())
                  return;
               pc = *srcit - 1;
               state = 0;
               break;
         }
      }
      while (state < 3)
         ;

      // *pc holds the first character of the next token.
      Errpos ep(*srcit, srcit - source.begin() + 1, pc - *srcit);

      // Digit => numeric literal.
      if (isdigit(*pc))
      {
         string val;
         bool hasPoint = false;
//         bool byte = false;
         bool floating = false;
         bool dots = false;
         while (isdigit(*pc))
            val += *pc++;
         if (*pc == 'e' || *pc == 'E')
         {
            hasPoint = true;
            val += *pc++;
            if (*pc == '+' || *pc == '-')
               val += *pc++;
            while (isdigit(*pc))
               val += *pc++;
         }
         else if (*pc == '.')
         {
            pc++;
            if (*pc == '.')
            {
               dots = true;
               pc++;
            }
            else
            {
               pc--;
               hasPoint = true;
               val += *pc++;
               while (isdigit(*pc))
                  val += *pc++;
               if (*pc == 'e' || *pc == 'E')
               {
                  hasPoint = true;
                  val += *pc++;
                  if (*pc == '+' || *pc == '-')
                     val += *pc++;
                  while (isdigit(*pc))
                     val += *pc++;
               }
            }
         }
         if (floating || hasPoint)
            tokens.push_back(Token(ep, DECIMALVAL, val));
         else
         {
            tokens.push_back(Token(ep, INTVAL, val));
            if (dots)
               tokens.push_back(Token(ep, OP_DOTS, ".."));
         }
         --pc;
      }

//      }
//
//         if (*pc == 'f' || *pc == 'F')
//         {
//            pc++;
//            floating = true;
//         }
//         else if (*pc == 'b' || *pc == 'B')
//         {
//            pc++;
//            byte = true;
//         }
//
//         if (byte)
//         {
//            if (hasPoint)
//            {
//               cerr << "Value is not a byte: stored as Decimal." << ep;
//               tokens.push_back(Token(ep, DECIMALVAL, val));
//            }
//            else
//            {
//               if (atoi(val.c_str()) > 255)
//               {
//                  cerr << "Value is too big to be a byte: stored as Integer." << ep;
//                  tokens.push_back(Token(ep, INTVAL, val));
//               }
//               else
//                  tokens.push_back(Token(ep, BYTEVAL, val));
//            }
//         }
//         else if (floating)
//            tokens.push_back(Token(ep, FLOATVAL, val));
//         else if (hasPoint)
//            tokens.push_back(Token(ep, DECIMALVAL, val));
//         else
//         {
//            tokens.push_back(Token(ep, INTVAL, val));
//            if (dots)
//               tokens.push_back(Token(ep, OP_DOTS, ".."));
//         }
//         --pc;
//      }

      // Alpha => keyword or identifier.
      else if (isalpha(*pc))
      {
         string idval;
         while (isalnum(*pc) || *pc == '_')
            idval += *pc++;
         --pc;
         if (keywords.find(idval) == keywords.end())
            // Not a keyword - must be an identifier
            tokens.push_back(Token(ep, IDENVAL, idval));
         else
         {
            // It's a keyword.
            tokens.push_back(Token(ep, keywords[idval], idval, true));
         }
      }
      else if (*pc == '"')
      {
         string value = readChars(pc, '"', *srcit);
         if (value.length() == 1)
            tokens.push_back(Token(ep, CHARVAL, value));
         else
            tokens.push_back(Token(ep, TEXTVAL, value));
      }
      else if (*pc == '\'')
      {
         string value = readChars(pc, '\'', *srcit);
         if (value.length() == 1)
            tokens.push_back(Token(ep, CHARVAL, value));
         else
            tokens.push_back(Token(ep, TEXTVAL, value));
      }
      else
      {
         switch (*pc)
         {

               // Digraphs

            case '+':
               ++pc;
               if (*pc == '=')
                  tokens.push_back(Token(ep, OP_PLUS_EQ, "+="));
               else
               {
                  tokens.push_back(Token(ep, OP_PLUS, "+"));
                  --pc;
               }
               break;

            case '-':
               ++pc;
               if (*pc == '=')
                  tokens.push_back(Token(ep, OP_MINUS_EQ, "-="));
               else if (*pc == '>')
                  tokens.push_back(Token(ep, ARROW, "->"));  // not used any more
               else
               {
                  tokens.push_back(Token(ep, OP_MINUS, "-"));
                  --pc;
               }
               break;

            case '*':
               ++pc;
               if (*pc == '=')
                  tokens.push_back(Token(ep, OP_MUL_EQ, "*="));
               else
               {
                  tokens.push_back(Token(ep, OP_MUL, "*"));
                  --pc;
               }
               break;

            case '/':
               ++pc;
               if (*pc == '/')
                  tokens.push_back(Token(ep, OP_CAT, "//"));
               else if (*pc == '=')
                  tokens.push_back(Token(ep, OP_DIV_EQ, "/="));
               else
               {
                  tokens.push_back(Token(ep, OP_DIV, "/"));
                  --pc;
               }
               break;

            case '%':
               ++pc;
               if (*pc == '=')
                  tokens.push_back(Token(ep, OP_MOD_EQ, "%="));
               else if (*pc == '%')
                  tokens.push_back(Token(ep, PERC_PERC, "%%"));
               else
               {
                  tokens.push_back(Token(ep, OP_MOD, "%"));
                  --pc;
               }
               break;

            case '<':
               ++pc;
               if (*pc == '=')
                  tokens.push_back(Token(ep, OP_LE, "<="));
               else if (*pc == '>')
                  tokens.push_back(Token(ep, OP_NE, "<>"));
               else if (*pc == '-')
                  tokens.push_back(Token(ep, OP_MOVE, "<-"));
               else
               {
                  tokens.push_back(Token(ep, OP_LT, "<"));
                  --pc;
               }
               break;

            case '>':
               ++pc;
               if (*pc == '=')
                  tokens.push_back(Token(ep, OP_GE, ">="));
               else
               {
                  tokens.push_back(Token(ep, OP_GT, ">"));
                  --pc;
               }
               break;

            case ':':
               ++pc;
               if (*pc == '=')
                  tokens.push_back(Token(ep, OP_ASSIGN, ":="));
               else
               {
                  tokens.push_back(Token(ep, COLON, ":"));
                  --pc;
               }
               break;

            case '~':
               ++pc;
               if (*pc == '=')
                  tokens.push_back(Token(ep, OP_NE, "~="));
               else
                  cerr << "Warning: '~' not followed by '='." << ep;
               break;

            case '!':
               ++pc;
               if (*pc == '=')
                  tokens.push_back(Token(ep, OP_NE, "!="));
               else
                  cerr << "Warning: '!' not followed by '='." << ep;
               break;

            case '.': // 071108
               ++pc;
               if (*pc == '.')
                  tokens.push_back(Token(ep, OP_DOTS, ".."));
               else
               {
                  tokens.push_back(Token(ep, DOT, "."));
                  --pc;
               }
               break;

            case '&': // 081125
               ++pc;
               if (*pc == '=')
                  tokens.push_back(Token(ep, OP_AND_EQ, "&="));
               else
                  cerr << "Warning: '&' not followed by '='." << ep;
               break;

               // Single characters

            case '@':
               tokens.push_back(Token(ep, AT, "@"));
               break;

            case '=':
               tokens.push_back(Token(ep, OP_EQ, "="));
               break;

            case '#': // 071108
               tokens.push_back(Token(ep, OP_SHARP, "#"));
               break;

            case '[':
               tokens.push_back(Token(ep, LB, "["));
               break;

            case ']':
               tokens.push_back(Token(ep, RB, "]"));
               break;

            case '{':
               tokens.push_back(Token(ep, LC, "{"));
               break;

            case '}':
               tokens.push_back(Token(ep, RC, "}"));
               break;

            case '(':
               tokens.push_back(Token(ep, LP, "("));
               break;

            case ')':
               tokens.push_back(Token(ep, RP, ")"));
               break;

            case '|':
               tokens.push_back(Token(ep, BAR, "|"));
               break;

            case '^':
               tokens.push_back(Token(ep, CARET, "^"));
               break;

            case '?':
               tokens.push_back(Token(ep, QUEST_MARK, "?"));
               break;

            case ',':
               tokens.push_back(Token(ep, COMMA, ","));
               break;

            case ';':
               tokens.push_back(Token(ep, SEMICOLON, ";"));
               break;

            default:
               cerr << "Warning: illegal character." << ep;

         }
      }
   }
}



