/** \file parser.h
 * Declaration of class \a Parser.
 */

#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"
#include "ast.h"

#include <list>
#include <stack>

using namespace std;

class Token;

/** A parser accepts a list of tokens and builds an abstract syntax tree. */
class Parser
{
public:

   /** Construct a parser.
    * \param tokens provides the input for the parser. */
   Parser(list<Token> tokens);


   /** Check that an expected token is actually there.
    * \param kind is the kind of token expected.
    *        If the next token is of this kind, move to the next token.
    * \param message is the error message,
    *        reported if the next token has a different kind.
    */
   void check(TokenKind kind, string message);


   /** Match the next token.
    * \param kind is the kind of token expected.
    * If the next token is of this kind,
      move to the next token and return \a true.
    * Otherwise, return \a false without advancing.
    */
   bool match(TokenKind kind);

   /** Report a missing identifier, and give a warning about keywords. */
   void checkIdentifier(string msg);

   /** Check that a name list has only one name in it. */
   void checkNames(List names, Errpos ep);

   /** Convert a token to an assignment operator. */
   Operator convOp();

   /** Check whether the current token could start a statement. */
   bool isStatement();

   /** Check whether the current token introduces a basic type. */
   bool isBasicType();

   /** Check whether the current token introduces a type. */
   bool isType();

   /** Check whether the current token introduces a slot. */
   bool isSlot();

   /** Check whether the current token introduces an expression. */
   bool isExpression();

   /** Check whether the current token could introduce
     * the argument list of a function. */
   bool isArgument();

   /** Parse an entire program (list of type definitions). */
   Node parseProgram();

   /** Parse a list of qualified names. */
   List parseQualifiedNameList(bool definition, bool lvalue);

   /** Parse a qualified name (e.g., \a a.b.c.). */
   Node parseQualifiedName(bool definition = false, bool lvalue = false);

   /** Parse a statement - may return more than one statement. */
   List parseStatement();

   /** Parse a \a select or \a loopselect statement.
    * \param loop is \a true for \a loopselect and
    *                \a false for \a select.
    */
   Node parseSelect(const bool loop);

   /** Parse a set comprehension. */
   Node parseComprehension();

   /** Parse a set. */
   Node parseCollection(Node type);

   /** Parse a step in a range descriptor. */
   Node parseStep();

   /** Parse a declaration. */
   List parseDeclaration(bool defining, MessageKind mk, bool alias);

   /** Parse a mode (COPY, SHARE, ALIAS). */
   Mode parseMode();

   /** Parse a policy (FAIR, ORDERED, RANDOM). */
   Policy parsePolicy();

   /** Parse a basic type. */
   Node parseBasicType();

   /** Parse a type (basic type name or cell). */
   Node parseType();

   /** Parse a cell. */
   Node parseCell();

   /** Parse a procedure. */
   Node parseProcedure();

   /** Parse a C++ function declaration. */
   Node parseCppDec();

   /** Parse a process. */
   Node parseProcess();

   /** Parse a protocol. */
   Node parseProtocol();

   /** Parse "A|B" in a protocol. */
   Node parseProtocolAlternative();

   /** Parse "A;B" in a protocol. */
   Node parseProtocolSequence();

   /** Parse a message in a protocol. */
   Node parseProtocolFactor();

   /** Parse a slot declaration - may return more than one declaration.
   /*  \param alias is true if the declaration starts with 'alias'.
   */
   List parseSlotDeclaration(bool alias);

   /** Parse a list of slots. */
   List parseSlots();

   /** Parse a general expression. */
   Node parseExpression();

   /** Parse a 'X if P else Y'. */
   Node parseConditionalExpression();

   /** Parse 'P1 or P2 or etc' */
   Node parseDisjunction();

   /** Parse 'P1 and P2 and etc' */
   Node parseConjunction();

   /** Parse 'E1=E2' or 'E1<>E2' */
   Node parseComparison();

   /** Parse 'E1<E2' or 'E1<=E2' or 'E1>=E2' or 'E1>E2' */
   Node parseOrdering();

   /** Parse 'E1 addop E2 addop etc' where addop is + or - */
   Node parseSum();

   /** Parse 'E1 mulop E2 mulop etc' where mulop is * or / */
   Node parseProduct();

   /** Parse an expression that does not have operators at top level.
    *  There may be operators within parenthesized subexpressions.
    */
   Node parseFactor();

   /** Parse a sequence of statements. */
   Node parseSequence();

   /** Parse " [ 'else' Sequence ] 'end' ". */
   Node parseAlternative();

   /** Parse a name (i.e., identifier). */
   Node parseName(bool definition);

   /** Parse a thread declaration. */
   Node parseThread();

   /** Parse the parameter list of a thread.
    *  \param input distinguishes input/output parameters.
    */
   List parseParamList(Node port, bool input);

   /** Parse a thread invokcation. */
   Node parseThreadCall();

private:

   /** The list of tokens to be parsed. */
   list<Token> tokens;

   /** A global iterator for the token list. */
   list<Token>::const_iterator tki;

   /** Counter for temporary expressions generated for 'cases' statements. */
   static int caseTempNum;
};

#endif
