/** \file scanner.h
 * Declaration of class Scanner.
 */

#ifndef SCANNER_H
#define SCANNER_H

#include "token.h"

#include <list>
#include <map>
#include <string>
#include <vector>
#include <glibmm.h>

using namespace std;

/** An instance is an object that can perform
 * lexical analysis of a given input file.
 * The scanner should not be deleted until compilation is complete,
 * because it stores the source text.
 */
class Scanner
{
    public:

        /** Construct an initialized scanner. */
        Scanner();

        /** Delete scanner and source text. */
        ~Scanner();

        /** Scan a file.
         * \param filename is the full name/path of the input file.
         * \param tokens is a list of tokens to which the scanner
         will append all tokens read from the file.
         * \throw an instance of class OldStyleError giving the position
         *        of the error in the source file and a diagnostic.
         */
        void scanFile(string filename, list<Token> & tokens);

    private:

        /** A map giving the kind of token corresponding to each keyword string. */
        map<Glib::ustring, TokenKind> keywords;

        /** The source text of the file.
          Each element points to a line of source code.*/
        vector<Glib::ustring> source;

};

#endif
