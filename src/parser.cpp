/** \file parser.cpp
 * Implementation of parsing functions.
 */

#include "ast.h"
#include "enumerations.h"
#include "parser.h"
#include "scanner.h"
#include "utilities.h"

#include <cassert>
#include <iostream>
#include <sstream>
#include <list>
#include <string>
#include <stack>

using namespace std;

int Parser::caseTempNum = 0;

Parser::Parser(list<Token> toks) : tokens(toks)
{
   // If the parameter name is changed from 'toks' to 'tokens',
   // the constructor sometimes fails!
   tki = tokens.begin();
}

void Parser::check(TokenKind kind, string message)
{
   if (tki->kind == kind)
      ++tki;
   else
      Error() << "Syntax: " << message << '.' << tki->ep << THROW;
}

bool Parser::match(TokenKind kind)
{
   if (tki->kind == kind)
   {
      ++tki;
      return true;
   }
   else
      return false;
}

void Parser::checkIdentifier(string message)
{
   if (tki->keyword)
      Error() << "Syntax: " << message << "('" << tki->value << "' is a keyword.)" << tki->ep << THROW;
   else
      Error() << "Syntax: " << message << tki->ep << THROW;
}

void Parser::checkNames(List names, Errpos ep)
{
   if (names.size() > 1)
      Error() << "only one target is allowed in this context." << ep << REPORT;
}

Operator Parser::convOp()
{
   Operator op = BINOP_PLUS;
   switch (tki->kind)
   {
      case OP_PLUS_EQ:
         op = BINOP_PLUS;
         break;
      case OP_MINUS_EQ:
         op = BINOP_MINUS;
         break;
      case OP_MUL_EQ:
         op = BINOP_MULTIPLY;
         break;
      case OP_DIV_EQ:
         op = BINOP_DIVIDE;
         break;
      case OP_MOD_EQ:
         op = BINOP_MOD;
         break;
      default:
         Error() << "Syntax: operator expected." << tki->ep << REPORT;
         break;
   }
   ++tki;
   return op;
}

bool Parser::isBasicType()
{
   return
      tki->kind == KW_BOOL ||
      tki->kind == KW_BYTE ||
      tki->kind == KW_INTEGER ||
      tki->kind == KW_DECIMAL ||
      tki->kind == KW_FLOAT ||
      tki->kind == KW_CHAR ||
      tki->kind == KW_TEXT ||
      tki->kind == KW_INPUT_FILE ||
      tki->kind == KW_OUTPUT_FILE ||
      tki->kind == KW_UNSIGNED ||
      tki->kind == KW_VOID;
}

bool Parser::isType()
{
   return
      isBasicType() ||
      tki->kind == IDENVAL ||
      tki->kind == KW_ENUM ||
      tki->kind == OP_LT;
}

bool Parser::isStatement()
{
   return
      tki->kind == IDENVAL ||
      tki->kind == KW_SKIP ||
      tki->kind == KW_EXIT ||
      tki->kind == KW_UNTIL ||
      tki->kind == KW_WHILE ||
      tki->kind == KW_ASSERTFUN ||
      tki->kind == KW_FILE_CLOSE ||
      tki->kind == KW_FILE_WRITE ||
      tki->kind == KW_IF ||
      tki->kind == KW_CASES ||
      tki->kind == KW_LOOP ||
      tki->kind == KW_FOR ||
      tki->kind == KW_ANY ||
      tki->kind == KW_SELECT ||
      tki->kind == KW_LOOPSELECT ||
      tki->kind == KW_START;
}

bool Parser::isSlot()
{
   return
      tki->kind == IDENVAL ||
      tki->kind == LC ||
      tki->kind == KW_ALIAS; // 080517
}

bool Parser::isExpression()
{
   return
      tki->kind == KW_TRUE ||
      tki->kind == KW_FALSE ||
      tki->kind == KW_EXECUTE ||
      tki->kind == CHARVAL ||
      tki->kind == TEXTVAL ||
      tki->kind == INTVAL ||
      tki->kind == DECIMALVAL ||
      tki->kind == IDENVAL ||
      tki->kind == LP ||
      tki->kind == KW_NOT ||
      tki->kind == OP_MINUS ||
      tki->kind == OP_SHARP ||
      tki->kind == PERC_PERC ||
      tki->kind == KW_BOOLFUN ||
      tki->kind == KW_CHARFUN ||
      tki->kind == KW_INTFUN ||
      tki->kind == KW_FLOATFUN ||
      tki->kind == KW_TEXTFUN ||
      tki->kind == KW_FORMATFUN ||
      tki->kind == KW_RANDOMFUN ||
      tki->kind == KW_FILE_OPEN ||
      tki->kind == KW_FILE_CLOSE  ||
      tki->kind == KW_FILE_OK  ||
      tki->kind == KW_FILE_EOF  ||
      tki->kind == KW_FILE_READ  ||
      tki->kind == KW_FILE_WRITE;
}

bool Parser::isArgument()
{
   return
      tki->kind == KW_TRUE ||
      tki->kind == KW_FALSE ||
      tki->kind == CHARVAL ||
      tki->kind == TEXTVAL ||
      tki->kind == INTVAL ||
      tki->kind == DECIMALVAL ||
      tki->kind == IDENVAL ||
      tki->kind == LP;
}


// Program -> {
//              'import' { Name },
//            | 'pervasive' Name ':' Type '=' Expression
//            | Name (
//                     '=' (
//                           BasicType
//                         | Type
//                         | '[' Protocol ']'
//                         | '{' Closure '}'
//                         )
//                   |
//                     '+=' (
//                            Name
//                          | '(' Cell ')'
//                          )
//                   |
//                     '(' { Expression }, ')'
//                   )
//            };
Node Parser::parseProgram()
{
   Errpos epstart = tki->ep;
   List nodes;
   while (tki->kind != END_OF_FILE)
   {
      if (tki->kind == KW_IMPORT)
      {
         // Imports are processed elsewhere, ignored here.
         ++tki;
         while (tki->kind == IDENVAL)
         {
            parseName(false);
            if (tki->kind == COMMA)
               ++tki;
            else if (tki->kind == SEMICOLON)
            {
               ++tki;
               break;
            }
            else
               break;
         }
      }

      else if (tki->kind == IDENVAL)
      {
         Node name = parseName(true);
         switch (tki->kind)
         {
               // Name : Type = Expression
            case COLON:
               {
                  ++tki;
                  Errpos ep = tki->ep;
                  Node type = parseType();
                  check(OP_EQ, "'=' expected or error in definition");
                  Node value = parseExpression();
                  nodes.push_back(new ConstantNode(ep, name, type, value, true));
               }
               break;

               // Name = Declaration
            case OP_EQ:
               ++tki;
               if (isType())
                  nodes.push_back(new DefNode(tki->ep, name, parseType()));
               else
               {
                  Errpos ep = tki->ep;
                  switch (tki->kind)
                  {
                     case LB:
                     case KW_PROTOCOL:
                        nodes.push_back(new DefNode(ep, name, parseProtocol()));
                        break;

                     case LP:
                     case KW_CELL:
                        nodes.push_back(new DefNode(ep, name, parseCell()));
                        break;

                     case LC:
                     case KW_PROCESS:
                        nodes.push_back(new DefNode(ep, name, parseProcess()));
                        break;

                     case KW_THREAD:
                        nodes.push_back(new DefNode(ep, name, parseThread()));
                        break;

                     case KW_CPP:
                        nodes.push_back(new DefNode(ep, name, parseCppDec()));
                        break;

                     case KW_PROCEDURE:
                        nodes.push_back(new DefNode(ep, name, parseProcedure()));
                        break;

                     default:
                        Error() << "Syntax: missing ':' or error after '='." << ep << REPORT;
                        break;
                  }
               }
               break;

               // Name += Declaration
            case OP_PLUS_EQ:
               {
                  ++tki;
                  Errpos ep = tki->ep;
                  switch (tki->kind)
                  {
                     case IDENVAL:
                        nodes.push_back(new DefNode(ep, name, parseName(false), true));
                        break;

                     case LP:
                     case KW_CELL:
                        nodes.push_back(new DefNode(ep, name, parseCell(), true));
                        break;

                     default:
                        Error() << "Syntax: bad syntax after '+=' in top-level definition." << ep << REPORT;
                        break;
                  }
               }
               break;

               // Top-level instance
            case LP:
               {
                  ++tki;
                  Errpos ep = tki->ep;
                  name->setDefining(false);
                  List args;
                  while (isExpression())
                  {
                     args.push_back(parseExpression());
                     if (tki->kind == COMMA)
                        ++tki;
                  }
                  check(RP, "expression or ')' expected");
                  nodes.push_back(new InstanceNode(ep, name, args, true));
               }
               break;

            default:
               Error() << "Syntax: illegal declaration." << tki->ep << THROW;
               break;
         }
      }
      else
         checkIdentifier("Unexpected token in top-level definition ");

      if (tki->kind == SEMICOLON)
         ++tki;
   }
   return new ProgramNode(epstart, nodes);
}

// BasicType -> 'Bool' | 'Integer' | 'Decimal' | 'Float' | 'Text' .
Node Parser::parseBasicType()
{
   Node result = 0;
   switch (tki->kind)
   {
      case KW_BOOL:
         result =  BaseNode::theBoolNode;
         ++tki;
         break;

      case KW_BYTE:
         result =  BaseNode::theByteNode;
         ++tki;
         break;

      case KW_INTEGER:
         result =  BaseNode::theIntegerNode;
         ++tki;
         break;

      case KW_UNSIGNED:
         ++tki;
         if (tki->kind == KW_BYTE)
            result = BaseNode::theUnsignedByteNode;
         else if (tki->kind == KW_INTEGER)
            result = BaseNode::theUnsignedIntegerNode;
         else
         {
            Error() << "Type cannot be qualified with 'unsigned'." << tki->ep << REPORT;
            result = BaseNode::theIntegerNode;
         }
         ++tki;
         break;

      case KW_INPUT_FILE:
         result =  BaseNode::theInputFileNode;
         ++tki;
         break;

      case KW_OUTPUT_FILE:
         result =  BaseNode::theOutputFileNode;
         ++tki;
         break;

      case KW_FLOAT:
         result =  BaseNode::theFloatNode;
         ++tki;
         break;

      case KW_DECIMAL:
         result =  BaseNode::theDecimalNode;
         ++tki;
         break;

      case KW_CHAR:
         result =  BaseNode::theCharNode;
         ++tki;
         break;

      case KW_TEXT:
         result =  BaseNode::theTextNode;
         ++tki;
         break;

      case KW_VOID:
         result =  BaseNode::theVoidNode;
         ++tki;
         break;
   }
   assert(result);
   return result;
}

// Type -> '<' { Name }, '>'
//      |  ( BasicType | Name ) [ '[' Expr [ ',' Expr ']' ] | "indexes" Type ]
Node Parser::parseType()
{
   Errpos ep = tki->ep;
   if (tki->kind == KW_ENUM || tki->kind == OP_LT)
   {
      // Enumerated type
      TokenKind opener = tki->kind;
      ++tki;
      List values;
      int position = 0;
      while (tki->kind == IDENVAL)
      {
         values.push_back(new EnumValueNode(tki->ep, tki->value, position));
         ++tki;
         ++position;
         if (tki->kind == COMMA)
            ++tki;
      }
      if (opener == KW_ENUM)
         check(KW_END, "error in enumerated type definition");
      else
         check(OP_GT, "error in enumerated type definition");
      return new EnumTypeNode(ep, values);
   }
   else if (isBasicType() || tki->kind == IDENVAL)
   {
      Node baseType = isBasicType() ? parseBasicType() : parseName(false);
      if (tki->kind == KW_INDEXES)
      {
         // Indexed type
         ++tki;
         PortKind portDir = NO_ROLE;
         if (match(OP_PLUS))
            portDir = SERVER;
         else if (match(OP_MINUS))
            portDir = CLIENT;
         return new MapTypeNode(ep, baseType, parseType(), portDir);
      }
      else if (tki->kind == LB)
      {
         // Array
         ++tki;
         Node lo = new NumNode(ep, BaseNode::theIntegerNode, "0");
         Node hi = new NumNode(ep, BaseNode::theIntegerNode, "0");
         if (isExpression())
         {
            hi = parseExpression();
            if (tki->kind == COMMA)
            {
               ++tki;
               lo = hi;
               hi = parseExpression();
            }
         }
         check(RB, "error in array declaration.  Missing ']'?");
         return new ArrayTypeNode(ep, baseType, lo, hi);
      }
      else
         return baseType;
   }
   else
      Error() << "Syntax: error in type definition." << tki->ep << REPORT;
}


// Type -> '<' { Name }, '>'
//      |  ( BasicType | Name ) [ "indexes" Type]
//Node Parser::parseType()
//{
//   Errpos ep = tki->ep;
//   Node result = 0;
//   if (tki->kind == OP_LT)
//   {
//      // Enumerated type
//      ++tki;
//      List values;
//      int position = 0;
//      while (tki->kind == IDENVAL)
//      {
//         values.push_back(new EnumValueNode(tki->ep, tki->value, position));
//         ++tki;
//         ++position;
//         if (tki->kind == COMMA)
//            ++tki;
//      }
//      check(OP_GT, "error in enumerated type definition");
//      result = new EnumTypeNode(ep, values);
//   }
//   else
//   {
//      if (isBasicType())
//      {
//         // Basic type
//         result = parseBasicType();
//         if (tki->kind == LB)
//         {
//            // Array
//            ++tki;
//            int hi = 0;
//            if (tki->kind == INTVAL)
//            {
//               istringstream is(tki->value);
//               is >> hi;
//            }
//            else
//               Error() << "Array dimension should be an integer." << tki->ep << REPORT;
//            ++tki;
//            check(RB, "error in array declaration.  Missing ']'?");
//            result = new ArrayTypeNode(ep, result, 0, hi);
//         }
//      }
//      else if (tki->kind == IDENVAL)
//         // Name of type defined previously
//         result = parseName(false);
//      else
//         Error() << "Syntax: bad type." << tki->ep << REPORT;
//
//      if (tki->kind == KW_INDEXES)
//      {
//         // Indexed type
//         ++tki;
//         PortKind portDir = NO_ROLE;
//         if (match(OP_PLUS))
//            portDir = SERVER;
//         else if (match(OP_MINUS))
//            portDir = CLIENT;
//         result = new MapTypeNode(ep, result, parseType(), portDir);
//      }
//   }
//   return result;
//}

// Cell -> '(' Params '|' Instances ')'
//      |  '(' Instances ')'
Node Parser::parseCell()
{
   bool keywordSyntax;
   if (tki->kind == LP)
      keywordSyntax = false;
   else if (tki->kind == KW_CELL)
      keywordSyntax = true;
   else
      Error() << "Syntax: cell expected." << tki->ep << REPORT;
   Errpos ep = tki->ep;
   ++tki;
   List params;
   List instances = parseSlots();
   if (tki->kind == BAR)
   {
      ++tki;
      params = instances;
      instances = parseSlots();
   }
   if (keywordSyntax)
      check(KW_END, "error in cell declaration or missing 'end'");
   else
      check(RP, "error in cell declaration or missing '}'");
   return new CellNode(ep, params, instances);
}

// Slots -> [ 'alias' ] { SlotDeclaration };
List Parser::parseSlots()
{
   List slots;
   while (isSlot())
   {
      bool alias = false;
      if (tki->kind == KW_ALIAS)
      {
         alias = true;
         ++tki;
      }
      List decs = parseSlotDeclaration(alias);
      slots.insert(slots.end(), decs.begin(), decs.end());
      if (tki->kind == SEMICOLON)
         ++tki;
   }
   return slots;
}

// SlotDeclaration -> Declaration | Process
List Parser::parseSlotDeclaration(bool alias)
{
   List result;
   switch (tki->kind)
   {
      case IDENVAL:
         result = parseDeclaration(true, NO_FIELD, alias);
         break;

      case LC:
      case KW_PROCESS:
         result.push_back(parseProcess());
         break;

      default:
         Error() << "Syntax: declaration expected." << tki->ep << REPORT;
   }
   return result;
}

// Declaration -> { Qualifiedname },
//                (   ':' ( '+' | '-' ) Type ( '=' | ':=' ) Expr
//                  | ':=' Expr
//                  | ( '+=' | '-=' | '*=' | '/=' | '%=' | '&=' ) Expr
//                  | '(' Expression $ ',' ')'
//                )
List Parser::parseDeclaration(bool defining, MessageKind mk, bool alias)
{
   if (tki->kind != IDENVAL)
      Error() << "Parser error: identifier expected." << tki->ep << THROW;
   Errpos ep = tki->ep;
   List names = parseQualifiedNameList(defining, true);
   List result;
   switch (tki->kind)
   {
      case COLON: // Variable declaration
         {
            ++tki;
            PortKind portDir = NO_ROLE;
            if (tki->kind == OP_PLUS)
            {
               portDir = SERVER;
               ++tki;
            }
            else if (tki->kind == OP_MINUS)
            {
               portDir = CLIENT;
               ++tki;
            }
            Node type = parseType();
            Node value = 0;
            bool constant = false;
            bool move = false;
            if (match(OP_ASSIGN))
               value = parseExpression();
            else if (match(OP_EQ))
            {
               value = parseExpression();
               constant = true;
            }
            else if (match(OP_MOVE))
            {
               value = parseExpression();
               move = true;
            }
            else
            {
               Token p = *tki;
               if (
                  match(OP_PLUS_EQ) ||
                  match(OP_MINUS_EQ) ||
                  match(OP_MUL_EQ) ||
                  match(OP_DIV_EQ) ||
                  match(OP_MOD_EQ) )
               {
                  Error() << "Syntax: operator '" << p.value << "' is not allowed in this context." << p.ep << REPORT;
                  ++tki;
               }
            }
            for (ListIter it = names.begin(); it != names.end(); ++it)
            {
               if ((*it)->kind() != NAME_NODE)
                  Error() << "Syntax: qualifed name cannot be declared." << (*it)->getPos() << REPORT;
               else
               {
                  (*it)->setDefining(true);
                  result.push_back(new DecNode(ep, *it, type, value, portDir, mk, false, constant, move, alias));
               }
            }
            break;
         }

      case OP_ASSIGN: // Assignment
         {
            ++tki;
            Node value = parseExpression();
            for (ListIter it = names.begin(); it != names.end(); ++it)
            {
               (*it)->setDefining(false);
               result.push_back(new DecNode(ep, *it, 0, value, NO_ROLE, NO_FIELD, false, false, false, false));
            }
            break;
         }

      case OP_MOVE: // Move (<-)
         {
            ++tki;
            Node value = parseExpression();
            for (ListIter it = names.begin(); it != names.end(); ++it)
            {
               (*it)->setDefining(false);
               result.push_back(new DecNode(ep, *it, 0, value, NO_ROLE, NO_FIELD, false, false, true, false));
            }
            break;
         }

      case OP_PLUS_EQ:
      case OP_MINUS_EQ:
      case OP_MUL_EQ:
      case OP_DIV_EQ:
      case OP_MOD_EQ:
         {
            Operator op = convOp();
            Node rhs = parseExpression();
            for (ListIter it = names.begin(); it != names.end(); ++it)
            {
               (*it)->setDefining(false);
               Node bin = new BinopNode(ep, op, *it, rhs);
               result.push_back(new DecNode(ep, *it, 0, bin, NO_ROLE, NO_FIELD, false, false, false, false));
            }
            break;
         }

      case OP_AND_EQ:
         {
            checkNames(names, names[0]->getPos());
            Node expr = names[0];
            expr->setDefining(false); // 090701
            while (tki->kind == OP_AND_EQ)
            {
               ++tki;
               expr = new BinopNode(ep, BINOP_EXTEND, expr, parseExpression());
            }
            result.push_back(expr);
            break;
         }

//            Node rhs = parseExpression();
//
//            // 081129
//            while (tki->kind == OP_AND_EQ)
//            {
//               ++tki;
//               rhs = new BinopNode(ep, BINOP_EXTEND, rhs, parseExpression());
//            }
//
//            for (ListIter it = names.begin(); it != names.end(); ++it)
//            {
//               (*it)->setDefining(false);
//               result.push_back(new BinopNode(ep, BINOP_EXTEND, *it, rhs));
//            }
//            break;
//         }

      case LP: // Instance of cell or process
         {
            ++tki;
            List args;
            while (isExpression())
            {
               args.push_back(parseExpression());
               if (tki->kind == COMMA)
                  ++tki;
            }
            if (tki->kind == ARROW)
            {
               // Thread invocation
               ++tki;
               List outputs = parseQualifiedNameList(false, true);
               check(RP, "error in arguments of thread invokcation or missing ')'");
               List calls;
               calls.push_back(new ThreadCallNode(ep, names[0], args, outputs));
               Node seq = new SequenceNode(ep, List());
               result.push_back(new StartNode(ep, calls, seq));
            }
            else
            {
               check(RP, "expression or ')' expected");
               for (ListIter it = names.begin(); it != names.end(); ++it)
               {
                  (*it)->setDefining(false);
                  result.push_back(new InstanceNode(ep, *it, args));
               }
               checkNames(names, ep);
            }
            break;
         }

      case AT: // Iteration statement
         {
            ++tki;
            if (tki->kind != IDENVAL)
               Error() << "Syntax: @ should be followed by 'begin' or 'next'." << tki->ep << REPORT;
            IterFun fun = ITER_START;
            if (tki->value == "begin")
               fun = ITER_START;
            else if (tki->value == "next")
               fun = ITER_STEP;
            else
               Error() << "Syntax: @ should be followed by 'begin' or 'next'." << ep << REPORT;
            ++tki;
            for (ListIter it = names.begin(); it != names.end(); ++it)
               result.push_back(new IteratorNode(ep, *it, fun));
            checkNames(names, ep);
            break;
         }

      default:
         // This is used when a signal is declared in a protocol
         // or used in a statement.
         for (ListIter it = names.begin(); it != names.end(); ++it)
            result.push_back(new DecNode(ep, *it, 0, 0, NO_ROLE, mk, false, false, false, false));
         checkNames(names, ep);
         break;
   }
   return result;
}

// Protocol -> Iden | '[' ( ProtocolSeqence $ '|' ) ']'
Node Parser::parseProtocol()
{
   Errpos ep = tki->ep;
   if (tki->kind == IDENVAL)
      return parseName(false);
   else if (tki->kind == LB)
   {
      ++tki;
      Node prot = parseProtocolAlternative();
      check(RB, "error in protocol: ']' expected");
      return new ProtocolNode(ep, prot);
   }
   else if (tki->kind == KW_PROTOCOL)
   {
      ++tki;
      Node prot = parseProtocolAlternative();
      check(KW_END, "error in protocol: 'end' expected");
      return new ProtocolNode(ep, prot);
   }
   else
      Error() << "Syntax: protocol expected." << ep << REPORT;
}

// ProtocolAlternative -> ProtocolSequence $ '|'
Node Parser::parseProtocolAlternative()
{
   Errpos ep = tki->ep;
   Node seq = parseProtocolSequence();
   if (tki->kind == BAR)
   {
      List args;
      args.push_back(seq);
      while (tki->kind == BAR)
      {
         ++tki;
         args.push_back(parseProtocolSequence());
      }
      return new ListopNode(ep, LISTOP_BAR, args);
   }
   else
      return seq;
}

// ProtocolSequence -> ProtcolFactor $ ';'
Node Parser::parseProtocolSequence()
{
   Errpos ep = tki->ep;
   Node fac = parseProtocolFactor();
   if (tki->kind == SEMICOLON)
   {
      List args;
      args.push_back(fac);
      while (tki->kind == SEMICOLON)
      {
         ++tki;
         args.push_back(parseProtocolFactor());
      }
      return new ListopNode(ep, LISTOP_SEQ, args);
   }
   else
      return fac;
}

// ProtocolFactor -> [ '^' ] Declaration
//                |  '*' ProtocolFactor
//                |  '+' ProtocolFactor
//                |  '?' ProtocolFactor
//                |  '('  ProtocolAlternative ')'
Node Parser::parseProtocolFactor()
{
   Node result = 0;
   Errpos ep = tki->ep;
   switch (tki->kind)
   {
      case IDENVAL:
         {
            List names = parseDeclaration(true, QUERY, false);
            checkNames(names, ep);
            result = names.front();
         }
         break;

      case CARET:
         {
            ++tki;
            List names = parseDeclaration(true, REPLY, false);
            checkNames(names, ep);
            result = names.front();
         }
         break;

      case OP_MUL:
         ++tki;
         result = new UnopNode(tki->ep, UNOP_MANY, parseProtocolFactor());
         break;

      case OP_PLUS:
         ++tki;
         result = new UnopNode(tki->ep, UNOP_MORE, parseProtocolFactor());
         break;

      case QUEST_MARK:
         ++tki;
         result = new UnopNode(tki->ep, UNOP_OPT, parseProtocolFactor());
         break;

      case LP:
         ++tki;
         result = parseProtocolAlternative();
         check(RP, "error in protocol: ')' expected");
         break;

      default:
         checkIdentifier("error in protocol");
   }
   return result;
}

Node Parser::parseCppDec()
{
   if (tki->kind != KW_CPP)
      Error() << "Syntax: C++ declartation expected." << tki->ep << REPORT;
   Errpos ep = tki->ep;
   ++tki;
   List params = parseSlots();
   Node type = 0;
   if (tki->kind == ARROW)
   {
      ++tki;
      type = parseType();
   }
   check(KW_END, "error in declaration or missing 'end'");
   return new CppNode(ep, params, type);
}

// Procedure -> 'procedure' Slots '|' Sequence 'end'
Node Parser::parseProcedure()
{
   if (tki->kind != KW_PROCEDURE)
      Error() << "Syntax: procedure expected." << tki->ep << REPORT;
   Errpos ep = tki->ep;
   ++tki;
   List slots = parseSlots();
   Node result;
   if (tki->kind == BAR)
   {
      ++tki;
      result = new ProcedureNode(ep, slots, parseSequence());
   }
   else
      result = new ProcedureNode(ep, slots, new SequenceNode(tki->ep, List()));
   check(KW_END, "error in statement or missing 'end'");
   return result;
}

// Process -> '{' Slots '|' Sequence '}'
//         |  'process' Slots '|' Sequence 'end'
Node Parser::parseProcess()
{
   assert(tki->kind == LC || tki->kind == KW_PROCESS);
   bool keywordSyntax = tki->kind == KW_PROCESS;
   Errpos ep = tki->ep;
   ++tki;
   List slots = parseSlots();
   Node result;
   if (tki->kind == BAR)
   {
      ++tki;
      result = new ProcessNode(ep, slots, parseSequence());
   }
   else
      result = new ProcessNode(ep, slots, new SequenceNode(tki->ep, List()));
   if (keywordSyntax)
      check(KW_END, "'end' or '|' missing, or error in statement");
   else
      check(RC, "'}' or '|' missing, or error in statement");
   return result;
}

// Sequence -> Statement $ ;
Node Parser::parseSequence()
{
   Errpos ep = tki->ep;
   List seq;
   while (isStatement())
   {
      List stmts = parseStatement();
      seq.insert(seq.end(), stmts.begin(), stmts.end());
      match(SEMICOLON);
   }
   return new SequenceNode(ep, seq);
}

// Statement -> 'skip'
//           |  'exit'
//           |  'until' Disjunction
//           |  'while' Disjunction
//           |  'assert' '(' arg ',' arg ')'
//           |  'if' Disjunction 'then' Sequence
//              { 'elif' Disjunction 'then' Sequence }
//              Alternative
//           |  cases ... end
//           |  'loop' Sequence 'end'
//           |  Select
//           |  Declaration
List Parser::parseStatement()
{
   List result;
   switch (tki->kind)
   {

      case KW_SKIP:
         result.push_back(new SkipNode(tki->ep));
         ++tki;
         break;

      case KW_EXIT:
         result.push_back(new ExitNode(tki->ep));
         ++tki;
         break;

      case KW_UNTIL:
         {
            Errpos ep = tki->ep;
            ++tki;
            Node cond = parseExpression();
            List stmts;
            stmts.push_back(new ExitNode(ep));
            Node seq = new SequenceNode(ep, stmts);
            List pairs;
            pairs.push_back(new CondPairNode(ep, cond, seq));
            Node alt = new SequenceNode(ep, List());
            result.push_back(new IfNode(ep, pairs, alt));
            break;
         }

      case KW_WHILE:
         {
            Errpos ep = tki->ep;
            ++tki;
            Node cond = new UnopNode(ep, UNOP_NOT, parseExpression());
            List stmts;
            stmts.push_back(new ExitNode(ep));
            Node seq = new SequenceNode(ep, stmts);
            List pairs;
            pairs.push_back(new CondPairNode(ep, cond, seq));
            Node alt = new SequenceNode(ep, List());
            result.push_back(new IfNode(ep, pairs, alt));
            break;
         }

      case KW_IF:
         {
            Errpos epstart = tki->ep;
            ++tki;
            List condPairs;
            Errpos ep = tki->ep;
            Node cond = parseExpression();
            check(KW_THEN, "bad expression or missing 'then'");
            condPairs.push_back(new CondPairNode(ep, cond, parseSequence()));
            while (match(KW_ELIF))
            {
               ep = tki->ep;
               Node cond = parseExpression();
               check(KW_THEN, "bad expression or missing 'then'");
               condPairs.push_back(new CondPairNode(tki->ep, cond, parseSequence()));
            }
            result.push_back(new IfNode(epstart, condPairs, parseAlternative()));
            break;
         }

      case KW_CASES:
         {
            Errpos epstart = tki->ep;
            ++tki;
            Node lhs = 0;
            if (isExpression())
            {
               Errpos ep = tki->ep;
               Node expr = parseExpression();
               string tempName = "casetemp" + str(++Parser::caseTempNum);
               Node ass = new DecNode(ep, new NameNode(ep, tempName, 0, true), 0, expr,
                                      NO_ROLE, NO_FIELD, true, false, false, false);
               result.push_back(ass);
               lhs = new NameNode(ep, tempName, 0, false);
            }
            List condPairs;
            while (match(BAR))
            {
               Errpos ep = tki->ep;
               Node rhs = 0;
               if (isExpression())
                  rhs = parseExpression();
               Node cond = 0;
               if (rhs)
                  cond = lhs ? new BinopNode(ep, BINOP_EQ, lhs, rhs) : rhs;
               else
                  cond = new BoolNode(Errpos(tki->ep), true);
               check(BAR, "error in guard or '|' missing");
               condPairs.push_back(new CondPairNode(tki->ep, cond, parseSequence()));
            }
            result.push_back(new IfNode(epstart, condPairs, new SequenceNode(epstart, List())));
            check(KW_END, "'end' missing (possibly in an inner scope)");
            break;
         }

      case KW_LOOP:
         {
            Errpos ep = tki->ep;
            ++tki;
            result.push_back(new LoopNode(ep, parseSequence()));
            check(KW_END, "'end' missing (possibly in inner scope)");
            break;
         }

      case KW_FOR:
         {
            Errpos ep = tki->ep;
            ++tki;
            Node comp = parseComprehension();
            check(KW_DO, "'do' expected");
            Node seq = parseSequence();
            result.push_back(new ForNode(ep, comp, seq));
            check(KW_END, "'end' missing (possibly in inner scope)");
            break;
         }

      case KW_ANY:
         {
            Errpos ep = tki->ep;
            ++tki;
            Node comp = parseComprehension();
            check(KW_DO, "'do' expected");
            Node seq = parseSequence();
            Node alt = parseAlternative();
            result.push_back(new AnyNode(ep, comp, seq, alt));
            break;
         }

      case KW_SELECT:
         {
            result.push_back(parseSelect(false));
            break;
         }

      case KW_LOOPSELECT:
         {
            result.push_back(parseSelect(true));
            break;
         }

      case KW_START:
         {
            Errpos ep = tki->ep;
            ++tki;
            List calls;
            while (tki->kind == IDENVAL)
            {
               calls.push_back(parseThreadCall());
               if (tki->kind == SEMICOLON)
                  ++tki;
            }
            check(KW_DO, "'do' expected or error in thread invocation");
            result.push_back(new StartNode(ep, calls, parseSequence()));
            check(KW_END, "'end' missing (possibly in inner scope)");
            break;
         }

      case IDENVAL:
         {
            if (  tki->value == "assert" ||
                  tki->value == "file_close" ||
                  tki->value == "file_write" )
            {
               Node name = new NameNode(tki->ep, tki->value, 0, false);
               Errpos ep = tki->ep;
               ++tki;
               List args;
               if (tki->kind == LP)
               {
                  ++tki;
                  while (isExpression())
                  {
                     args.push_back(parseExpression());
                     if (tki->kind == COMMA)
                        ++tki;
                  }
                  check(RP, "error in assertion or function call or ')' missing");
               }
               else
                  args.push_back(parseFactor());
               result.push_back(new FunctionNode(ep, name, args));
            }
            else
               result = parseDeclaration(false, NO_FIELD, false);
            break;
         }

   }
   return result;
}


// Select -> [ Policy ]
//           { '|' [ Disjunction ] '|' Sequence }
Node Parser::parseSelect(const bool loop)
{
   Errpos eps = tki->ep;
   ++tki;
   List options;
   Policy pol = parsePolicy();
   while (tki->kind == BAR)
   {
      Errpos ep = tki->ep;
      ++tki;

      // Guard
      Node guard = 0;
      if (isExpression())
         guard = parseExpression();
      check(BAR, "error in guard or '|' missing");
      options.push_back(new OptionNode(ep, pol, guard, parseSequence()));
   }
   check(KW_END, "bad statement or missing 'end'");

   if (loop)
   {
      List seq;
      seq.push_back(new SelectNode(eps, pol, options));
      return new LoopNode(eps, new SequenceNode(eps, seq));
   }
   else
      return new SelectNode(eps, pol, options);
}

// Mode -> 'copy' | 'share' | 'alias' | empty
Mode Parser::parseMode()
{
   switch (tki->kind)
   {
      case KW_COPY:
         ++tki;
         return COPY;

      case KW_ALIAS:
         ++tki;
         return ALIAS;

      case KW_SHARE:
         ++tki;
         return SHARE;

      default:
         return DEFAULT_MODE;
   }
}

// Policy -> 'fair' | 'ordered' | 'random' | empty
Policy Parser::parsePolicy()
{
   switch (tki->kind)
   {
      case KW_FAIR:
         ++tki;
         return FAIR;

      case KW_ORDERED:
         ++tki;
         return ORDERED;

      case KW_RANDOM:
         ++tki;
         return RANDOM;

      default:
         return DEFAULT_POLICY;
   }
}

// Alternative -> [ 'else' Sequence ] 'end' .
Node Parser::parseAlternative()
{
   Node result;
   if (match(KW_ELSE))
      result = parseSequence();
   else
      result = new SequenceNode(tki->ep, List());
   check(KW_END, "bad statement or missing 'end'");
   return result;
}

// Comprehension -> Var [ ':' Type ] 'in' Set [ 'such' 'that' Expr ]
Node Parser::parseComprehension()
{
   Errpos ep = tki->ep;
   Node var = parseName(true);
   Node type = 0;
   if (tki->kind == COLON)
   {
      ++tki;
      type = parseType();
   }
   Node collection = 0;
   if (match(KW_IN))
      collection = parseCollection(type);
   else
      collection = new EnumSetNode(ep, type);
   Node pred = 0;
   Errpos epSuch = tki->ep;
   if (tki->kind == KW_SUCH)
   {
      ++tki;
      if (tki->kind == KW_THAT)
         ++tki;
      else
         cerr << "Warning: 'that' inserted after 'such'." << epSuch;
      pred = parseExpression();
   }
   return new ComprehensionNode(ep, var, type, collection, pred);
}

// Step = '(' [ Expr ] ')'
Node Parser::parseStep()
{
   check(LP, "missing '(' or error in step expression");
   Node step = 0;
   if (isExpression())
      step = parseSum();
   check(RP, "missing ')' or error in step expression");
   return step;
}

// Set -> Expr
//     | ( 'domain' | 'range' ) Expr
//     |  Expr 'to' Expr [ 'step' Expr ]
//     |  Expr '<=' Step ( '<=' | '<' ) Expr
//     |  Expr '>=' Step ( '>=' | '>' ) Expr

Node Parser::parseCollection(Node type)
{
   Errpos ep = tki->ep;
   if (tki->kind == KW_DOMAIN)
   {
      ++tki;
      Node map = parseExpression();
      return new MapSetNode(ep, MAPSET_DOMAIN, map);
   }
   else if (tki->kind == KW_RANGE)
   {
      ++tki;
      Node map = parseExpression();
      return new MapSetNode(ep, MAPSET_RANGE, map);
   }
   else if (isExpression())
   {
      Node start = parseSum();
      if (tki->kind == KW_TO)
      {
         ++tki;
         Node finish = parseSum();
         Node step = 0;
         if (tki->kind == KW_STEP)
         {
            ++tki;
            step = parseSum();
         }
         return new RangeNode(ep, type, start, finish, step, false, true);
      }
      else if (tki->kind == OP_LE)
      {
         ++tki;
         Node step = parseStep();
         bool open;
         if (tki->kind == OP_LE)
         {
            ++tki;
            open = false;
         }
         else if (tki->kind == OP_LT)
         {
            ++tki;
            open = true;
         }
         else
            Error() << "Syntax: '<' or '<=' expected." << tki->ep << REPORT;
         Node finish = parseSum();
         return new RangeNode(ep, type, start, finish, step, open, true);
      }
      else if (tki->kind == OP_GE)
      {
         ++tki;
         Node step = parseStep();
         bool open;
         if (tki->kind == OP_GE)
         {
            ++tki;
            open = false;
         }
         else if (tki->kind == OP_GT)
         {
            ++tki;
            open = true;
         }
         else
            Error() << "Syntax: '<' or '<=' expected." << tki->ep << REPORT;
         Node finish = parseSum();
         return new RangeNode(ep, type, start, finish, step, open, false);
      }
      else
         return new EnumSetNode(ep, start);
   }
   else
      Error() << "Syntax: set expression expected." << tki->ep << REPORT;
}

// Expression = CondExpr $ '//'
Node Parser::parseExpression()
{
   Node result = parseConditionalExpression();
   while (tki->kind == OP_CAT)
   {
      Errpos ep = tki->ep;
      ++tki;
      result = new BinopNode(ep, BINOP_CAT, result, parseConditionalExpression());
   }
   return result;
}

// ConditionalExpression = Disjunction [ 'if' Disjunction 'else' Disjunction ]
Node Parser::parseConditionalExpression()
{
   Errpos ep = tki->ep;
   Node lhs = parseDisjunction();
   if (tki->kind == KW_IF)
   {
      ++tki;
      Node pred = parseDisjunction();
      check(KW_ELSE, "Syntax: error in conditional expression.  Possible missing 'else'.");
      Node rhs = parseExpression();
      return new CondExprNode(ep, lhs, pred, rhs);
   }
   return lhs;
}


// Disjunction -> Conjunction $ 'or'
Node Parser::parseDisjunction()
{
   Node result = parseConjunction();
   while (tki->kind == KW_OR)
   {
      Errpos ep = tki->ep;
      ++tki;
      result = new BinopNode(ep, BINOP_OR, result, parseConjunction());
   }
   return result;
}

// Conjunction -> Comparison $ 'and'
Node Parser::parseConjunction()
{
   Node result = parseComparison();
   while (tki->kind == KW_AND)
   {
      Errpos ep = tki->ep;
      ++tki;
      result = new BinopNode(ep, BINOP_AND, result, parseComparison());
   }
   return result;
}

// Comparison -> Ordering [ '=' | '<>' ] Ordering
Node Parser::parseComparison()
{
   Node result = parseOrdering();
   Errpos ep = tki->ep;
   switch (tki->kind)
   {
      case OP_EQ:
         ++tki;
         result = new BinopNode(ep, BINOP_EQ, result, parseOrdering());
         break;

      case OP_NE:
         ++tki;
         result = new BinopNode(ep, BINOP_NE, result, parseOrdering());
         break;

      default:
         break;
   }
   return result;
}

// Ordering -> Sum [ '<' | '<=' | '>=' | '>' ] Sum
Node Parser::parseOrdering()
{
   Node result = parseSum();
   Errpos ep = tki->ep;
   switch (tki->kind)
   {
      case OP_LT:
         ++tki;
         result = new BinopNode(ep, BINOP_LT, result, parseOrdering());
         break;

      case OP_LE:
         ++tki;
         result = new BinopNode(ep, BINOP_LE, result, parseOrdering());
         break;

      case OP_GT:
         ++tki;
         result = new BinopNode(ep, BINOP_GT, result, parseOrdering());
         break;

      case OP_GE:
         ++tki;
         result = new BinopNode(ep, BINOP_GE, result, parseOrdering());
         break;

      default:
         break;
   }
   return result;
}

// Sum -> Product $ ( '+' | '-' | 'in' )
Node Parser::parseSum()
{
   Node result = parseProduct();
   while (tki->kind == OP_PLUS || tki->kind == OP_MINUS)
   {
      Errpos ep = tki->ep;
      Operator op;
      switch (tki->kind)
      {
         case OP_PLUS:
            op = BINOP_PLUS;
            break;

         case OP_MINUS:
            op = BINOP_MINUS;
            break;
      }
      ++tki;
      result = new BinopNode(ep, op, result, parseProduct());
   }
   return result;
}

// Product -> Factor $ ( '*' | '/' | '%')
Node Parser::parseProduct()
{
   Node result = parseFactor();
   while ( tki->kind == OP_MUL ||
           tki->kind == OP_DIV ||
           tki->kind == KW_DIV ||
           tki->kind == OP_MOD ||
           tki->kind == KW_REM ||
           tki->kind == KW_MOD)
   {
      Errpos ep = tki->ep;
      Operator op;
      switch (tki->kind)
      {
         case OP_MUL:
            op = BINOP_MULTIPLY;
            break;

         case OP_DIV:
         case KW_DIV:
            op = BINOP_DIVIDE;
            break;

         case OP_MOD:
         case KW_MOD:
         case KW_REM:
            op = BINOP_MOD;
            break;
      }
      ++tki;
      result = new BinopNode(ep, op, result, parseFactor());
   }
   return result;
}

// Factor -> 'true'
//        |  'false'
//        |  'execute' Bytes
//        |  Char
//        |  Text
//        |  Fun  Arg
//        |  Fun '(' Args ')'
//        |  Number
//        |  QualifiedName [ '[' Sub [ '..' Sub ] ']' [ '.' Iden ] ]
//        |  QualifiedName '@' Identifier
//        |  'not' Factor
//        |  '-' Factor
//        |  '(' Expression ')'
Node Parser::parseFactor()
{
   Errpos eps = tki->ep;
//   cerr << "Factor " << eps << endl;
   Node result = 0;
   switch (tki->kind)
   {
      case KW_TRUE:
         result = new BoolNode(Errpos(tki->ep), true);
         ++tki;
         break;

      case KW_FALSE:
         result = new BoolNode(Errpos(tki->ep), false);
         ++tki;
         break;

      case KW_EXECUTE:
         ++tki;
         result = new UnopNode(Errpos(tki->ep), UNOP_EXECUTE, parseFactor());
         break;

      case CHARVAL:
         result = new CharNode(Errpos(tki->ep), tki->value[0]);
         ++tki;
         break;

      case TEXTVAL:
         result = new TextNode(Errpos(tki->ep), tki->value);
         ++tki;
         break;

      case INTVAL:
         result = new NumNode(Errpos(tki->ep), BaseNode::theIntegerNode, tki->value);
         ++tki;
         break;

      case DECIMALVAL:
         result = new NumNode(Errpos(tki->ep), BaseNode::theDecimalNode, tki->value);
         ++tki;
         break;

      case LP:
         ++tki;
         result = parseExpression();
         check(RP, "bad expression or ')' missing");
         break;

      case KW_NOT:
         ++tki;
         result = new UnopNode(Errpos(tki->ep), UNOP_NOT, parseFactor());
         break;

      case OP_MINUS:
         {
            ++tki;
            Node fac = parseFactor();
            if (fac->kind() == NUM_NODE)
            {
               fac->negate();
               result = fac;
            }
            else
               result = new UnopNode(Errpos(tki->ep), UNOP_MINUS, fac);
         }
         break;

      case OP_SHARP:
         {
            Errpos ep = tki->ep;
            ++tki;
            if (isArgument())
            {
               List args;
               if (tki->kind == LP)
               {
                  ++tki;
                  while (isExpression())
                  {
                     args.push_back(parseExpression());
                     if (tki->kind == COMMA)
                        ++tki;
                  }
                  check(RP, "bad argument list or missing ')'");
               }
               else
                  args.push_back(parseFactor());
               result = new FunctionNode(ep, new NameNode(ep, "#", 0, false), args);
            }
            else
               Error() << "Syntax: # requires a right operand." << tki->ep << REPORT;
         }
         break;

      case IDENVAL:
         {
            Errpos ep = tki->ep;
            result = parseQualifiedName();
            if (isArgument())
            {
               List args;
               if (tki->kind == LP)
               {
                  ++tki;
                  while (isExpression())
                  {
                     args.push_back(parseExpression());
                     if (tki->kind == COMMA)
                        ++tki;
                  }
                  check(RP, "bad argument list or missing ')'");
               }
               else
                  args.push_back(parseFactor());
               result = new FunctionNode(ep, result, args);
            }
            else if (tki->kind == AT)
            {
               ++tki;
               if (tki->kind != IDENVAL)
                  Error() << "Syntax: @ should be followed by 'finish', 'key', or 'value'." << tki->ep << REPORT;
               IterFun fun = ITER_FINISH;
               if (tki->value == "finish")
                  fun = ITER_FINISH;
               else if (tki->value == "key")
                  fun = ITER_KEY;
               else if (tki->value == "value")
                  fun = ITER_VALUE;
               else
                  Error() << "Syntax: @ should be followed by 'finish', 'key', or 'value'." << tki->ep << REPORT;
               ++tki;
               result = new IteratorNode(ep, result, fun);
               break;
            }
         }
         break;

      default:
         Error() << "Syntax: bad factor." << eps << REPORT;
         break;
   }
   return result;
}

// QualifiedNameList -> QualifiedName $ ','
List Parser::parseQualifiedNameList(bool definition, bool lvalue)
{
   List result;
   while (tki->kind == IDENVAL)
   {
      result.push_back(parseQualifiedName(definition, lvalue));
      if (tki->kind == COMMA)
         ++tki;
   }
   return result;
}

// CompoundName -> Name { '[' Sum [ '..' Sum ] ']' } [ '.' Iden ]
Node Parser::parseQualifiedName(bool definition, bool lvalue)
{
   Errpos ep = tki->ep;
   Node result = parseName(definition);
   while (match(LB))
   {
      Node sub = parseSum();
      if (match(OP_DOTS))
         result = new SubrangeNode(ep, result, sub, parseSum(), lvalue);
      else
         result = new SubscriptNode(ep, result, sub, lvalue);
      match(RB);
   }
   if (match(DOT))
      result = new DotNode(ep, result, parseName(false));
   else if (match(QUEST_MARK))
      result = new QueryNode(ep, result, parseName(false), 0, 1);
   return result;
}

// Name -> Identifier
Node Parser::parseName(bool definition)
{
   if (tki->kind == IDENVAL)
   {
      Node p = new NameNode(tki->ep, tki->value, 0, definition);
      ++tki;
      return p;
   }
   else
      checkIdentifier("identifier expected");
}

// Thread -> 'thread' Inputs [ '->' Outputs ] '|' Sequence 'end'
Node Parser::parseThread()
{
   assert(tki->kind == KW_THREAD);
   Errpos ep = tki->ep;
   Node port = new NameNode(ep, "ch", 0, false);
   ++tki;
   List inputs = parseParamList(port, true);
   List outputs;
   if (tki->kind == ARROW)
   {
      ++tki;
      outputs = parseParamList(port, false);
   }
   Node result;
   if (tki->kind == BAR)
   {
      ++tki;
      result = new ThreadNode(ep, port, inputs, outputs, parseSequence());
   }
   else
      result =  new ThreadNode(ep, port, inputs, outputs, new SequenceNode(tki->ep, List()));
   check(KW_END, "missing 'end', missing '|', or error in statement");
   return result;
}

// ParamList = { Name ':' Type };
List Parser::parseParamList(Node port, bool input)
{
   List result;
   while (tki->kind == IDENVAL)
   {
      List names = parseQualifiedNameList(true, true);
      check(COLON, "':' expected or error in parameter declaration");
      PortKind portDir = NO_ROLE;
      if (tki->kind == OP_PLUS)
      {
         portDir = SERVER;
         ++tki;
      }
      else if (tki->kind == OP_MINUS)
      {
         portDir = CLIENT;
         ++tki;
      }
      Node type = parseType();
      for (ListIter it = names.begin(); it != names.end(); ++it)
         result.push_back(new ThreadParamNode((*it)->getPos(), *it, portDir, type, port, input));
      if (tki->kind == SEMICOLON)
         ++tki;
   }
   return result;
}

// ThreadCall = Name '(' { Rvalue }, [ ->' { Lvalue }, ')'
Node Parser::parseThreadCall()
{
   assert(tki->kind == IDENVAL);
   Errpos ep = tki->ep;
   Node name = parseName(false);
   check(LP, "'(' expected in thread invocation");
   List inputs;
   while (isExpression())
   {
      inputs.push_back(parseExpression());
      if (tki->kind == COMMA)
         ++tki;
   }
   List outputs;
   if (tki->kind == ARROW)
   {
      ++tki;
      outputs = parseQualifiedNameList(false, true);
   }
   check(RP, "')' expected or error in arguments of thread invocation");
   return new ThreadCallNode(ep, name, inputs, outputs);
}
