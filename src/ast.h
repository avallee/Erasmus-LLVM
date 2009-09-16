/** \file ast.h
 * Declaration of AST node classes.
 */

#ifndef AST_H
#define AST_H

#include "basicblocks.h"
#include "enumerations.h"
#include "error.h"
#include "functions.h"
#include "types.h"
#include "typecodes.h"
#include "lts.h"

#include "genassem.h" // Lightning

#include <set>
#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert> // patch says #include <assert.h>

using namespace std;

// Refer to types.h for definitions of Node, List, etc.

const string BASIO_SYS = "sys";
const string BASIO_INP = "inp";
const string BASIO_OUT = "out";
const string BASIO_ERR = "err";

/** An instance is passed around the AST for semantic checking. */
class CheckData
{
public:
   /** Construct a \a CheckData object with default fields. */
   CheckData(bool showWarnings);

   /** The set of names already defined in the current scope.  This set is
     * used to check name declarations.  A declaration is legal only if the
     * name declared is not a member of varNames.  At the beginning of a new
     * scope, the od value of varNames is 'pushed and the current value is
     * made empty.  At the end of the scope, the old value is 'popped'.
     */
   StringSet varNames;

   /** The type of this node. */
   Node type;

   /** The iterator type for a for/any loop with a map. */
   Node iterType;

   /** The index type for a for/any loop with an array or text. */
   Node indexType;

   /** True when we are inside a loop. */
   bool withinLoop;

   /** True if we are inside a protocol expression. */
   bool withinProtocol;

   /** True if we are inside a guard. */
   bool withinGuard;

   /** True if we are inside a parameter list. */
   bool withinParamList;

   /** The set of field names in a protocol. */
   StringSet protNames;

   /** Show warning messages. */
   bool showWarnings;
};

/** An instance is passed around the AST during code generation. */
class GenData
{
public:
   /** Construct a \a GenData object with default fields. */
   GenData();

   /** Name of the most recent definition. */
   string entity;

   /** Unique number for select statement. */
   int selNum;

   /** Address of end of loop. */
   int loopEnd;

   /** Address of end of if statement. */
   int ifEnd;

   /** Address of start of guard test. */
   int testGuard;

   /** Address of start of option sequence. */
   int execBranch;

   /** Address of start of select statement. */
   int selectStart;

   /** Address of end of select statement. */
   int selectEnd;

   /** Index of statement in select option sequence.
     * The value is usually -1.  It is set to 0 for the
     * first statement of an option in a select statement,
     * and incremented for subsequent statements.
     * This allows DecNode::gen to recognize the first
     * statement of an option. */
   int seqIndex;

   /** EVM block number obtained from the last name node. */
   int evmBlockNumber;
};

/** Abstract base class for nodes in the abstract syntax tree (AST). */
class BaseNode
{
public:

//---------------------------------------------------------------Class functions

   /** Unique node representing the type \a Void. */
   static Node theVoidNode;

   /** Unique node representing all enumerated types. */
   static Node theEnumNode;

   /** Unique node representing the type \a Signal. */
   //static Node theSignalNode;

   /** Unique node representing the type \a Bool. */
   static Node theBoolNode;

   /** Unique node representing the type \a Byte. */
   static Node theByteNode;

   /** Unique node representing the type \a unsigned Byte. */
   static Node theUnsignedByteNode;

   /** Unique node representing the type input file. */
   static Node theInputFileNode;

   /** Unique node representing the type output file. */
   static Node theOutputFileNode;

   /** Unique node representing the type \a Integer. */
   static Node theIntegerNode;

   /** Unique node representing the type \a unsigned Integer. */
   static Node theUnsignedIntegerNode;

   /** Unique node representing the type \a Float. */
   static Node theFloatNode;

   /** Unique node representing the type \a Decimal. */
   static Node theDecimalNode;

   /** Unique node representing the type \a Char. */
   static Node theCharNode;

   /** Unique node representing the type \a Text. */
   static Node theTextNode;

   /** Unique node representing array types. */
   static Node theArrayNode;

//----------------------------------------------------------Base class functions

   /** Construct a base node with source code coordinates and node kind. */
   BaseNode(Errpos ep, NodeKind kv);

   /** Virtual destructor. */
   virtual ~BaseNode() {}

   /**  Set the definition chain pointer. */
   void setDefChain(Node pdc);

   /**  Write a description of the \a BaseNode component
    *   of this node to stream \a os.    */
   void showBase(ostream & os, int level) const;

   /**  Write a description of this node with a message to \a cerr. */
   void showDebug(string msg) const;

   /**  Display a message and a node, then terminate the program.
     *  When the program is run in debug mode,
     * this forces gdb to give a stack trace.
     */
   void emergencyStop(string msg, Errpos ep) const;

   /**  Return the sequence number of this node. */
   int num() const;

   /**  Return the definition chain pointer of this node. */
   Node getDefChain() const;

   /**  Return the kind of this node. */
   NodeKind kind() const;

   /**  Return the source code coordinates of this node for diagnostics. */
   Errpos getPos() const;

   /** Display the source code line corresponding to this node. */
   void showLine(ostream & os);

//-------------------------------------------------------------Virtual functions

   /** Write a description of this node to the stream \a os */
   virtual void show(ostream & os, int level = 0) const = 0;

   /** Write basic blocks to an output stream. */
   virtual void showBB(ostream & os) const;

   /** Write source code corresponding to this node to stream \a os */
   virtual void prettyPrint(ostream & os, int level = 0) const = 0;

   /** Bind names in this node to their definitions, using the definition chain. */
   virtual void bind(Node p);

   /** Another version of bind that does not report errors.
    *  This is used for function names. */
   virtual void tryBind(Node p);

   /** Check semantics for this node. */
   virtual void check(CheckData & cd);

   /** Prepare this node for code generation. */
   virtual void gen(GenData gd);

   /** Store information about this node in a linear list of basic blocks. */
   virtual void genBlocks(BlockList & blocks, bool storeBlock = false);

   /** Write parts of the source code for this node to the  stream \a code.*/
   virtual void writeParts(ostream & code, WriteMode wm);

   /** Write executable statements for this node to the stream \a code */
   virtual void write(ostream & code);

   /** Mark this node as \a defined. */
   virtual void setDefining(bool def = true);

   /** Set the start address (i.e., switch label) for this node. */
   virtual void setStartAddress(int s);

   /** Set the name associated with this node to \a n. */
   virtual void setName(string n);

   /** Mark this node as a reference variable. */
   virtual void setReference();

   /** Mark this node as a parameter. */
   virtual void setParameter();

   /** Tie this node to another node.
     * If nodes are tied, they use the same communication offset.
     */
   virtual void setTie(Node t);

   /** Set the field number of this node.  Field numbers identify
     * the role of a name in a protocol.  */
   virtual void setFieldNum(int fn);

   /** Set the offset of a name for Lightning assembly. */
   virtual void setOffset(int os);

   /** Get the largest field number in a protocol expression. */
   virtual void getx(int & fn) const;

   /** Set the type for this node. */
   virtual void setType(Node type);

   /** Mark a name as aliased. */
   virtual void setPassByReference();

   /** Write the 'exists' check for a SubscriptNode. */
   virtual void writeExistCheck(ostream & code) {}

   /** Return input and output parameters of a Thread. */
   virtual void getThreadParams(List & inpars, List & outpars);

   /** Start a thread (used only in ThreadCallNode). */
   virtual void startThread(BlockList & blocks) {}

   /** Stop a thread (used only in ThreadCallNode). */
   virtual void stopThread(BlockList & blocks) {}

   /** Prepare for Lightning assembly. */
   virtual void prepAssem(AssemData aData);

   /** Perform Lightning assembly. */
   virtual void genAssem();

   /** Add a Lightning jump instruction address. */
   virtual void addJumps(Patches keys) { }

   /** Write the AST for this node. */
   virtual bool drawAST(ostream & os, set<int> & nodeNums, int level);

   /** Return \a true if this node is a \a Bool type descriptor. */
   virtual bool isBoolType() const;

   /** Return \a true if this node is a numeric
     * (\a Byte, \a Integer, \a Float, \a Decimal) type descriptor.
     */
   virtual bool isNumericType() const;

   /** Return \a true if this node is an array type. */
   virtual bool isArrayType() const;

   /** Return \a true if this node is a map type. */
   virtual bool isMapType() const;

   /** Return \a true if this node is an enumeration type. */
   virtual bool isEnumType() const;

   /** Return \a true if this node is a map iterator type. */
   virtual bool isIterType() const;

   /** Return \a true if this is an empty sequence node.  */
   virtual bool isEmpty() const;

   /** Return \a true if this is a reply node
     * (flagged with a caret in the source code).
     */
   virtual bool isReply() const;

   /** Return \a true if this name is a parameter of a cell or process. */
   virtual bool isReference() const;

   /** Return \a true if this node is a variable of map type. */
   virtual bool isMap() const;

   /** Return \a true if this variable is a port. */
   virtual bool isPort() const;

   /** Return \a true if this variable was declared as a constant. */
   virtual bool isConstant() const;

   /** Return \a true if this variable was declared as a pervasive constant. */
   virtual bool isPervasive() const;

   /** Return \a true if this variable is a value from an enumeration. */
   virtual bool isEnumVal() const;

   /** Return \a true if this variable can be assigned a new value. */
   virtual bool assignable() const;

   /** Return \a true if this is a map iterator. */
   virtual bool isIterator() const;

   /** Return \a true if a NameNode is aliased. */
   virtual bool isAlias() const;

   /** Return true if a name should be passed by reference. */
   virtual bool isPassByReference() const;

   /** Return the sequence number of this variable.
     * Sequence numbers are sued for disambiguation.
     */
   virtual int getVarNum() const;

   /** Return the field number (that is,
     * position in the protocol) of a variable.
     */
   virtual int getFieldNum() const;

   /** Return the number of fields in a protocol. */
   virtual int getNumFields() const;

   /** Get block number for EVM code. */
   virtual int getEVMBlockNumber() const;

   /** Return the offset of a name for Lightning assembly. */
   virtual int getOffset() const;

   /** Get size of type for Lightning assembly. */
   virtual int getSize() const
   {
      return 0;
   }

   /** Get low bound of array. */
   virtual Node getLo() const;

   /** Get upper bound of array. */
   virtual Node getHi() const;

   /** Get left operand of binary expression. */
   virtual Node getLHS() const
   {
      return 0;
   }

   /** Get right operand of binary expression. */
   virtual Node getRHS() const
   {
      return 0;
   }

   /** Return the internal buffer name associated with a node. */
   virtual string getPortBufferName() const;

   /** Return the string used to access
     * the 'other' process in a communication.
     */
   virtual string getCommunicator() const;

   /** Get the 'name' field of an identifier or value of a TextNode. */
   virtual string getNameString() const;

   /** Get the string that represents the C type of a variable.
     * This should be a C++ name, such as \a int. */
   virtual string getCTypeString() const;

   /** Get the string that represents the Erasmus type of a variable.
     * This should be an Erasmus type name, such as \a Integer. */
   virtual string getEType() const;

   /** Get the form of a variable name used for code generation. */
   virtual string getFullName(bool withpointer = false) const;

   /** Return the owner (usually the enclosing process) of a vvariable name. */
   virtual string getOwner() const;

   /** Return the C++ string used to check the queue for a particular field. */
   virtual string getQueueTest() const;

   /** Return the EVM type code of a type node. */
   virtual string getEVMTypeCode() const;

   /** Get the name and kind of the loop variable of a map in a for/any loop. */
//   virtual void getLoopVarName(Node var, MapSetKind & msKind, string & name) const;
   virtual string getLoopVar(Node var = 0) const;

   /** Get the nane and kind of the iterator variable of a map in a for/any loop. */
   virtual void getIterator(Node iter, MapSetKind & msKind, Node & it) const;

   /** Return the value of a constant as a string. */
   virtual string getConstValue() const;

   /** Return the function code of a function node. */
   virtual string getFun() const;

   /** Return the string corresponding to a null value for a type. */
   virtual string getDefaultValue() const
   {
      return "";
   }

   /** Use the definition chain to find the definition of a name. */
   virtual Node lookUp(string value, Errpos ep);

   /** Return the definition of a name. */
   virtual Node getDefinition() const;

   /** Return the protocol associated with a communication. */
   virtual Node getProtocol();

   /** Return the value field of a definition. */
   virtual Node getValue() const;

   /** Return the type associated with a node. */
   virtual Node getType() const;

   /** Return the port associated with a node. */
   virtual Node getPort() const;

   /** Return the name of a definition or declaration node. */
   virtual Node getName() const;

   /** Return the parameter with the given index. */
   virtual Node getParam(int index) const;

   /** Return the field of a protocol. */
   virtual Node getField() const;

   /** Return the operand of a unary operator node. */
   virtual Node getArg() const;

   /** Return the node tied to this one, if any. */
   virtual Node getTie() const;

   /** Return the domain type of a map type. */
   virtual Node getDomainType() const;

   /** Return the range type of a map type. */
   virtual Node getRangeType() const;

   /** Return a node for the initialize and test part of a for/any loop. */
   virtual Node makeInit(Node var, Node iter);

   /** Return a node for the termination test of a for/any loop. */
   virtual Node makeTermTest(Block bb, Node var, Node iter);

   /** Return a node for the match test of a for/any loop. */
   virtual Node makeMatchTest(Block bb);

   /** Return a node for the step part of a for/any loop. */
   virtual Node makeStep(Node var, Node iter);

   /** Return the number of elements of an enumeration as an Integer node. */
   virtual Node getEnumSize() const;

   /** Return the set of fields of a protocol. */
   virtual set<Node> getFieldDecs() const;


   /** Return the argument list of an invocation. */
   virtual List getArgList() const;

   /** Return the parameter list of a cell or process. */
   virtual List getParamList() const;

   /** Take communication components out of an expression
    *  and replace them by temporary variables.
    */
   virtual List extract(int & tempNum);

   /** Get the operator of a unary or binary expression. */
   virtual Operator getOp() const;

   /** Get the map kind (INDEXED, ARRAY, TEXT) of a collection. */
   virtual MapKind getMapKind() const;

   /** Get the map set kind (DOMAIN, RANGE, PAIR) of a collection. */
   virtual MapSetKind getMapSetKind() const;

   /** Get the direction (server/client) of the field of a protocol. */
   virtual PortKind getPortKind(int slotNum = 0) const;

   /** Get the message kind (NO_FIELD | QUERY | REPLY) of a declaration. */
   virtual MessageKind getMessKind() const;

   /** Recognize special names such as \a sys, \a in, \a out, \a err. */
   virtual FileMode checkSysIO() const;

   /** Construct the LTS for a protocol and return it.
     * Also construct the set of field declarations for the protocl.
     * \param fieldDecs is the resulting set.
     */
   virtual LTS *protocolGraph(set<Node> & fieldDecs);

   /** Construct the LTS for a process with respect to a protocol
     * and return it.  */
   virtual LTS *processGraph(Node portDec, int loopEnd);

   /** Return the LTS for a protocol or sequence. */
   virtual LTS *getLTS() const;

   /** Return the type code of a type descriptor node. */
   virtual int getTypeCode() const
   {
      return TYPE_VOID;
   };

   /** Value of NumNode. */
   virtual int getIntVal() const;

   /** Value of NumNode. */
   virtual double getDecVal() const;

   /** Negate the value in a NumNode. */
   virtual void negate();

//   /** Set LHS name in a QueryNode. */
//   virtual void setLHS(Node name);

   /** Override for displaying nodes. */
   friend ostream & operator<<(ostream & os, const BaseNode *p);

protected:

   /** Unique sequence number of node. */
   int nodeNum;

   /** Object relating this node to its origin in the source code. */
   Errpos ep;

   /** Pointer to name definition chain. */
   Node defChain;

private:

   /** Kind of the node. */
   NodeKind kv;

   /** Counter for nodes: updated by base class constructor only. */
   static int nodeCount;
};

/** Root node for a complete program. */
class ProgramNode : public BaseNode
{
public:
   ProgramNode(Errpos ep, List nodes);
   void bind(Node p);
   void check(CheckData & cd);
   void gen(GenData gd);
   void genBlocks(BlockList & blocks, bool storeBlock = false);
   void write(ostream & code);
   void writeParts(ostream & code, WriteMode wm);
   List extract(int & tempNum);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   void showBB(ostream & os) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
private:

   /** List of definitions and a cell instantiation. */
   List nodes;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** An instance of a cell or closure. */
class InstanceNode : public BaseNode
{
public:
   InstanceNode(Errpos ep, Node name, List args, bool topLevel = false);
   void bind(Node p);
   void check(CheckData & cd);
   void gen(GenData gd);
   void genBlocks(BlockList & blocks, bool storeBlock = false);
   void write(ostream & code);
   void writeParts(ostream & code, WriteMode wm);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
   Node lookUp(string value, Errpos ep);
   Node getName() const;
   List getArgList() const;
   List extract(int & tempNum);
   string getNameString() const;
   string getFullName(bool withpointer = false) const;
private:

   /** Type of cell. */
   Node name;

   /** Ports and variables to be used by the instance. */
   List args;

   /** This is a top-level instance. */
   bool topLevel;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** Declaration for a C++ function or procedure that is defined externally. */
class CppNode : public BaseNode
{
public:
   CppNode(Errpos ep, List params, Node type);
   void bind(Node p);
   void check(CheckData & cd);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   Node getType() const;
   List getParamList() const;
private:

   /** Parameters of function or procedure. */
   List params;

   /** Return type, 0 for a procedure. */
   Node type;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** Body of a procedure with port and variable parameters. */
class ProcedureNode : public BaseNode
{
public:
   ProcedureNode(Errpos ep, List slots, Node seq);
   void bind(Node p);
   void check(CheckData & cd);
   void gen(GenData gd);
   void genBlocks(BlockList & blocks, bool storeBlock = false);
   void write(ostream & code);
   void writeParts(ostream & code, WriteMode wm);
   void setName(string n);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   void showBB(ostream & os) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
   Node getType() const;
   Node getParam(int index) const;
   List getParamList() const;
   List extract(int & tempNum);
   string getNameString() const;
   PortKind getPortKind(int slotNum = 0) const;
private:

   /** Ports and variables to be used by the procedure. */
   List params;

   /** Statements of the process. */
   Node seq;

   /** Start address. */
   int start;

   /** Unique type number. */
   int typeNum;

   /** Name of this process. */
   string name;

   /** Basic blocks for code. */
   BlockList blocks;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** Body of a closure with port and variable parameters. */
class ProcessNode : public BaseNode
{
public:
   ProcessNode(Errpos ep, List slots, Node seq);
   void bind(Node p);
   void check(CheckData & cd);
   void gen(GenData gd);
   void genBlocks(BlockList & blocks, bool storeBlock = false);
   void write(ostream & code);
   void writeParts(ostream & code, WriteMode wm);
   void setName(string n);
   void setStartAddress(int s);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   void showBB(ostream & os) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
   int getEVMBlockNumber() const;
   Node getParam(int index) const;
   Node getType() const;
   List getParamList() const;
   List extract(int & tempNum);
   string getNameString() const;
   PortKind getPortKind(int slotNum = 0) const;
private:

   /** Ports and variables to be used by the process. */
   List params;

   /** Statements of the process. */
   Node seq;

   /** Start address. */
   int start;

   /** Unique type number. */
   int typeNum;

   /** Name of this process. */
   string name;

   /** Basic blocks for code. */
   BlockList blocks;

   /** Block number for EVM code. */
   int evmBlockNum;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** The root of a protocol expression tree. */
class ProtocolNode : public BaseNode
{
public:
   ProtocolNode(Errpos ep, Node expr);
   void bind(Node p);
   void check(CheckData & cd);
   void gen(GenData gd);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
   bool isPort() const;
   int getNumFields() const;
   int getEVMBlockNumber() const;
   string getCTypeString() const;
   string getEType() const;
   string getPortBufferName() const;
   string getEVMTypeCode() const;
   Node getType() const;
   Node lookUp(string value, Errpos ep);
   Node getProtocol();
   List getParamList() const;
   set<Node> getFieldDecs() const;
   LTS *getLTS() const;

private:

   /** Expression defining the protocol. */
   Node expr;

   /** Number of fields in the protocol. */
   int numFields;

   /** LTS for this protocol (built by check()). */
   LTS *plts;

   /** Pointers to the message fields of this protocol. */
   set<Node> fieldDecs;

   /** Block number for EVM code. */
   int evmBlockNum;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** A node containing a name and its definition.
  * \todo Decide how to enter an incremental definition
  * into the symbol tree.
  */
class DefNode : public BaseNode
{
public:
   DefNode(Errpos ep, Node name, Node value, bool incremental = false);
   void bind(Node p);
   void check(CheckData & cd);
   void gen(GenData gd);
   void genBlocks(BlockList & blocks, bool storeBlock = false);
   void write(ostream & code);
   void writeParts(ostream & code, WriteMode wm);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   void showBB(ostream & os) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
   bool isMap() const;
   bool isMapType() const;
   bool isArrayType() const;
   bool isPort() const;
   bool isConstant() const;
   bool isEnumType() const;
   int getVarNum() const;
   int getNumFields() const;
   int getEVMBlockNumber() const;
   List extract(int & tempNum);
   Node lookUp(string value, Errpos ep);
   Node getValue() const;
   Node getType() const;
   Node getProtocol();
   Node getDomainType() const;
   Node getRangeType() const;
   Node getEnumSize() const;
   string getOwner() const;
   string getCTypeString() const;
   string getEType() const;
   string getEVMTypeCode() const;
   string getNameString() const;
   PortKind getPortKind(int slotNum = 0) const;
   string getPortBufferName() const;

private:

   /** True if this is an incremental (+=) definition. */
   bool incremental;

   /** Name of the object being defined. */
   Node name;

   /** An expression defining a type, protocol, etc. */
   Node value;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** Definition of a cell with port and variable parameters. */
class CellNode : public BaseNode
{
public:
   CellNode(Errpos ep, List params, List instances);
   void bind(Node p);
   void check(CheckData & cd);
   void gen(GenData gd);
   void genBlocks(BlockList & blocks, bool storeBlock = false);
   void write(ostream & code);
   void writeParts(ostream & code, WriteMode wm);
   void setName(string n);
   int getEVMBlockNumber() const;
   Node getType() const;
   List extract(int & tempNum);
   List getParamList() const;
   PortKind getPortKind(int slotNum = 0) const;
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
private:

   /** Port and variable parameters. */
   List params;

   /** Private declarations, instances of cells and closures. */
   List instances;

   /** Name given in definition. */
   string name;

   /** Block number for EVM code. */
   int evmBlockNum;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** A sequence (list) of statements. */
class SequenceNode : public BaseNode
{
public:
   SequenceNode(Errpos ep, List stmts);
   void bind(Node p);
   void check(CheckData & cd);
   void gen(GenData gd);
   void genBlocks(BlockList & blocks, bool storeBlock = false);
   void write(ostream & code);
   void writeParts(ostream & code, WriteMode wm);
   Node lookUp(string value, Errpos ep);
   List extract(int & tempNum);
   bool isEmpty() const;
   string getQueueTest() const;
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
   LTS *processGraph(Node portDec, int loopEnd);
private:

   /** The statements of the sequence. */
   List stmts;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** Skip statement. */
class SkipNode : public BaseNode
{
public:
   SkipNode(Errpos ep);
   void bind(Node p);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   Node lookUp(string value, Errpos ep);
   bool drawAST(ostream & os, set<int> & nodeNums, int level);

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** Exit statement. */
class ExitNode : public BaseNode
{
public:
   ExitNode(Errpos ep);
   void bind(Node p);
   void check(CheckData & cd);
   Node lookUp(string value, Errpos ep);
   void gen(GenData gd);
   void genBlocks(BlockList & blocks, bool storeBlock = false);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
   LTS *processGraph(Node portDec, int loopEnd);

private:

   /** Address of loop end. */
   int loopEnd;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
private:
   /** Pointer to parent IfNode, set by \a genass(). */
   Node parent;
};

/** Root of an if statement: points to list of if/then pairs
  * and an else part.
  */
class IfNode : public BaseNode
{
public:
   IfNode(Errpos ep, List condPairs, Node alt);
   void bind(Node p);
   void check(CheckData & cd);
   void gen(GenData gd);
   void genBlocks(BlockList & blocks, bool storeBlock = false);
   void write(ostream & code);
   List extract(int & tempNum);
   void writeParts(ostream & code, WriteMode wm);
   Node lookUp(string value, Errpos ep);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream  & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
   LTS *processGraph(Node portDec, int loopEnd);
private:

   /** A list of condition/action pairs. */
   List condPairs;

   /** The sequence following else.  Non-null, but maybe empty list. */
   Node alt;

   /** Address for end of statement. */
   int ifEnd;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
   void addJumps(Patches keys)
   {
      for (Patches::iterator i = keys.begin(); i != keys.end(); ++i)
         jumps.push_back(*i);
   }
private:
   vector<void*> jumps;
};

/** An 'if/then pair' consisting of a condition and a sequence. */
class CondPairNode : public BaseNode
{
public:
   CondPairNode(Errpos ep, Node cond, Node seq);
   void bind(Node p);
   void check(CheckData & cd);
   void gen(GenData gd);
   void genBlocks(BlockList & blocks, bool storeBlock = false);
   void write(ostream & code);
   Node lookUp(string value, Errpos ep);
   List extract(int & tempNum);
   void writeParts(ostream & code, WriteMode wm);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
   LTS *processGraph(Node portDec, int loopEnd);

private:

   /** Declarations inserted by extract if the condition communicates. */
   List decs;

   /** Boolean expression. */
   Node cond;

   /** Action sequence. */
   Node seq;

   /** Address for true branch. */
   int condTrue;

   /** Address for false branch. */
   int condFalse;

   /** Address for end of if statement. */
   int ifEnd;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
private:
   /** Pointer to parent IfNode, set by \a genass(). */
   Node parent;
};

/** Loop statement. */
class LoopNode : public BaseNode
{
public:
   LoopNode(Errpos ep, Node seq);
   void bind(Node p);
   void check(CheckData & cd);
   void gen(GenData gd);
   void genBlocks(BlockList & blocks, bool storeBlock = false);
   void write(ostream & code);
   List extract(int & tempNum);
   void writeParts(ostream & code, WriteMode wm);
   Node lookUp(string value, Errpos ep);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
   LTS *processGraph(Node portDec, int loopEnd);
private:

   /** Sequence of statements in the loop. */
   Node seq;

   /** Address of start of loop. */
   int loopStart;

   /** Address of end of loop. */
   int loopEnd;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
   void addJumps(Patches keys)
   {
      for (Patches::iterator i = keys.begin(); i != keys.end(); ++i)
         jumps.push_back(*i);
   }
private:
   vector<void*> jumps;
};

/** For statement */
class ForNode : public BaseNode
{
public:
   ForNode(Errpos ep, Node comp, Node seq);
   Node lookUp(string value, Errpos ep);
   List extract(int & tempNum);
   void bind(Node p);
   void check(CheckData & cd);
   void gen(GenData gd);
   void genBlocks(BlockList & blocks, bool storeBlock = false);
   void writeParts(ostream & code, WriteMode wm);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
private:

   /**  Set comprehension. */
   Node comp;

   /**  Loop body. */
   Node seq;

   /** Block number for testing whether there are more elements. */
   int forMore;

   /** Block number for the 'such that' clause, if present. */
   int forMatch;

   /** Block number for the body of the loop. */
   int forBody;

   /** Block number for stepping to the next element. */
   int forStep;

   /** Block number for the next statement. */
   int forEnd;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** Any statement */
class AnyNode : public BaseNode
{
public:
   AnyNode(Errpos ep, Node comp, Node seq, Node alt);
   Node lookUp(string value, Errpos ep);
   List extract(int & tempNum);
   void bind(Node p);
   void check(CheckData & cd);
   void gen(GenData gd);
   void genBlocks(BlockList & blocks, bool storeBlock = false);
   void writeParts(ostream & code, WriteMode wm);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
private:

   /**  Set comprehension. */
   Node comp;

   /**  Loop body. */
   Node seq;

   /**  Optional else part. */
   Node alt;

   /** Block number for testing whether there are more elements. */
   int anyMore;

   /** Block number for testing the 'such that' clause, if any. */
   int anyFound;

   /** Block number for the body of the loop. */
   int anyBody;

   /** Block number for stepping to the next element. */
   int anyStep;

   /** Block number for the 'else' clause, if any. */
   int anyElse;

   /** Block number for the next statement. */
   int anyEnd;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** Set comprehension */
class ComprehensionNode : public BaseNode
{
public:
   ComprehensionNode(Errpos ep, Node var, Node type, Node collection, Node pred);
   Node lookUp(string value, Errpos ep);
   Node getType() const;
   Node makeInit(Node var, Node iter);
   Node makeTermTest(Block bb, Node var, Node iter);
   Node makeMatchTest(Block bb);
   Node makeStep(Node var, Node iter);
   string getOwner() const;
   string getCTypeString() const;
   string getLoopVar(Node var = 0) const;
   void getIterator(Node iter, MapSetKind & msKind, Node & it) const;
   int getVarNum() const;
   int getEVMBlockNumber() const;
   bool isConstant() const;
   bool assignable() const;
   bool isIterator() const;
   void bind(Node p);
   void check(CheckData & cd);
   void gen(GenData gd);
   void genBlocks(BlockList & blocks, bool storeBlock = false);
   void writeParts(ostream & code, WriteMode wm);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
   Node getDomainType() const;
   Node getRangeType() const;

private:

   /**  Loop variable. */
   Node var;

   /**  Type of loop variable - may be 0. */
   Node type;

   /** Iterator variable. */
   Node iter;

   /** Type of iterator variable. */
   Node iterType;

   /**  Expression defining set to be considered. */
   Node collection;

   /**  Predicate for filtering the set. */
   Node pred;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** Set corresponding to a range of values */
class RangeNode : public BaseNode
{
public:
   RangeNode(Errpos ep, Node type, Node start, Node finish, Node step, bool open, bool ascending);
   Node lookUp(string value, Errpos ep);
   Node makeInit(Node var, Node iter);
   Node makeTermTest(Block bb, Node var, Node iter);
   Node makeStep(Node var, Node iter);
   bool assignable() const;
   void bind(Node p);
   void check(CheckData & cd);
   void gen(GenData gd);
   void genBlocks(BlockList & blocks, bool storeBlock = false);
   void writeParts(ostream & code, WriteMode wm);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
private:

   /** Type of loop variable.  Assume int if null. */
   Node type;

   /**  Initial value of loop variable. */
   Node start;

   /**  Limit for loop variable. */
   Node finish;

   /**  Increment for loop variable; defaults to 1 (never NULL). */
   Node step;

   /** Range is 'open', i.e., [m,n). */
   bool open;

   /** Counter is ascending. */
   bool ascending;

   /** Name of the containing closure. */
   string owner;

   /** Unique index for limit. */
   int finishNum;

   /** Unique index for step. */
   int stepNum;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** Initialization for range in for/any loop.
  * Generated by compiler.
  */
class RangeInitNode : public BaseNode
{
public:
   RangeInitNode(Errpos ep, string owner, Node type, Node var, Node start,
                 Node finish, Node step, int finishNum, int stepNum);
   void write(ostream & code);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);

private:

   /** Type of loop variable. */
   Node type;

   /** Loop variable. */
   Node var;

   /**  Initial value of loop variable. */
   Node start;

   /**  Limit for loop variable. */
   Node finish;

   /** Index for finish. */
   int finishNum;

   /**  Increment for loop variable; defaults to 1. */
   Node step;

   /** Index for step. */
   int stepNum;

   /** Name of the containing closure. */
   string owner;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** Termination test for range: generated by compiler. */
class RangeTermNode : public BaseNode
{
public:
   RangeTermNode(Errpos ep, string owner, Block bb, Node type, Node var,
                 Node step, int stepNum, Node finish,
                 int finishNum, bool open, bool ascending);
   void write(ostream & code);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);

private:

   /** Basic block that owns this node. */
   Block bb;

   /** Type of the loop variable. */
   Node type;

   /** Loop variable. */
   Node var;

   /**  Increment for loop variable; defaults to 1. */
   Node step;

   /** The range is open (or closed). */
   bool open;

   /** The loop variable is increasing (or decreasing). */
   bool ascending;

   /** Index for step. */
   int stepNum;

   /**  Increment for loop variable; defaults to 1. */
   Node finish;

   /** Index for step. */
   int finishNum;

   /** Name of the containing closure. */
   string owner;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** Step node for range: generated by compiler. */
class RangeStepNode : public BaseNode
{
public:
   RangeStepNode(Errpos ep, string owner, Node type, Node var, Node step,
                 int stepNum, bool ascending);
   void write(ostream & code);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);

private:

   /** Type of the loop variable. */
   Node type;

   /** Loop variable. */
   Node var;

   /**  Increment for loop variable; defaults to 1. */
   Node step;

   /** Loop variable increases. */
   bool ascending;

   /** Index for step. */
   int stepNum;

   /** Name of the containing closure. */
   string owner;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** Set corresponding to map variable */
class MapSetNode : public BaseNode
{
public:
   MapSetNode(Errpos ep, MapSetKind setKind, Node map);
   Node lookUp(string value, Errpos ep);
   Node makeInit(Node var, Node iter);
   Node makeTermTest(Block bb, Node var, Node iter);
   Node makeStep(Node var, Node iter);
   bool assignable() const;
   bool isIterator() const;
//   void getLoopVarName(Node var, MapSetKind & msKind, string & name) const;
   string getLoopVar(Node var = 0) const;
   void getIterator(Node iter, MapSetKind & msKind, Node & it) const;
   void bind(Node p);
   void check(CheckData & cd);
   void gen(GenData gd);
   void genBlocks(BlockList & blocks, bool storeBlock = false);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   void writeParts(ostream & code, WriteMode wm);
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
   MapKind getMapKind() const;
   MapSetKind getMapSetKind() const;
private:

   /**  Specifies elements from the map: RANGE, DOMAIN, or PAIR. */
   MapSetKind setKind;

   /** Specifies the map type: INDEXED, ARRAY, TEXT. */
   MapKind mapKind;

   /**  Map value (i.e., value of an indexed type). */
   Node map;

   /** Type of loop variable. */
   Node type;

   /** Iterator. */
   Node iter;

   /** Name of the containing closure. */
   string owner;

   /** Name of the index variable, required for RANGE loops. */
   string indexName;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** Initialization for map in for/any loop.
  * Generated by compiler.
  */
class MapInitNode : public BaseNode
{
public:
   MapInitNode(Errpos ep, string owner, Node type, Node var, Node iter,
               MapKind mapKind, MapSetKind setKind, Node map, string indexName);
   void write(ostream & code);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);

private:

   /** Loop variable. */
   Node var;

   /** Iterator. */
   Node iter;

   /** Type of loop variable. */
   Node type;

   /** Specifies the type of mapping: INDEXED, ARRAY, TEXT. */
   MapKind mapKind;

   /**  Specifies elements from the map: RANGE, DOMAIN, or PAIR. */
   MapSetKind setKind;

   /**  Map value (i.e., value of an indexed type). */
   Node map;

   /** Name of the containing closure. */
   string owner;

   /** Name of the loop index variable, if needed. */
   string indexName;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** Termination test for map: generated by compiler. */
class MapTermNode : public BaseNode
{
public:
   MapTermNode(Errpos ep, string owner, Block bb, Node type, Node var,
               Node iter, MapKind mapKind, MapSetKind setKind, Node map, string indexName);
   void write(ostream & code);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);

private:

   /** Basic block that owns this node. */
   Block bb;

   /** Loop variable. */
   Node var;

   /** Iterator. */
   Node iter;

   /** Type of loop variable. */
   Node type;

   /** Specifies the type of mapping: INDEXED, ARRAY, TEXT. */
   MapKind mapKind;

   /**  Specifies elements from the map: RANGE, DOMAIN, or PAIR. */
   MapSetKind setKind;

   /**  Map value (i.e., value of an indexed type). */
   Node map;

   /** Name of the containing closure. */
   string owner;

   /** Name of the loop index variable, if needed. */
   string indexName;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** Step node for map: generated by compiler. */
class MapStepNode : public BaseNode
{
public:
   MapStepNode(Errpos ep, string owner, Node type, Node var, Node iter,
               MapKind mapKind, MapSetKind setKind, Node map, string indexName);
   void write(ostream & code);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);

private:

   /** Loop variable. */
   Node var;

   /** Iterator. */
   Node iter;

   /** Type of loop variable. */
   Node type;

   /** Specifies the type of mapping: INDEXED, ARRAY, TEXT. */
   MapKind mapKind;

   /**  Specifies elements from the map: RANGE, DOMAIN, or PAIR. */
   MapSetKind setKind;

   /**  Map value (i.e., value of an indexed type). */
   Node map;

   /** Name of the containing closure. */
   string owner;

   /** Name of the loop index variable, if needed. */
   string indexName;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** Set corresponding to enumeration type. */
class EnumSetNode : public BaseNode
{
public:
   EnumSetNode(Errpos ep, Node type);
   Node lookUp(string value, Errpos ep);
   Node makeInit(Node var, Node iter);
   Node makeTermTest(Block bb, Node var, Node iter);
   Node makeStep(Node var, Node iter);
   bool assignable() const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
   void bind(Node p);
   void check(CheckData & cd);
   void gen(GenData gd);
   void genBlocks(BlockList & blocks, bool storeBlock = false);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   void writeParts(ostream & code, WriteMode wm);
private:

   /**  The enumeration type to be traversed. */
   Node type;

   /** Number of elements in the enumeration. */
   Node max;

   /** Name of the containing closure. */
   string owner;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** Initialization for enumeration in for/any loop.
  * Generated by compiler.
  */
class EnumInitNode : public BaseNode
{
public:
   EnumInitNode(Errpos ep, string owner, Node var);
   void write(ostream & code);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);

private:

   /** Loop variable. */
   Node var;

   /** Name of the containing closure. */
   string owner;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** Step for enumeration in for/any loop.
  * Generated by compiler.
  */
class EnumStepNode : public BaseNode
{
public:
   EnumStepNode(Errpos ep, string owner, Node var);
   void write(ostream & code);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);

private:

   /** Loop variable. */
   Node var;

   /** Name of the containing closure. */
   string owner;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** Termination for enumeration in for/any loop.
  * Generated by compiler.
  */
class EnumTermNode : public BaseNode
{
public:
   EnumTermNode(Errpos ep, string owner, Block bb, Node var, Node max);
   void write(ostream & code);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);

private:

   /** Basic block that owns this node. */
   Block bb;

   /** Loop variable. */
   Node var;

   /** Number of elements in enumeration. */
   Node max;

   /** Name of the containing closure. */
   string owner;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** Match test node: generated by compiler.
  * pred is the predicate following 'such that' in a for/any loop.
  */
class MatchNode : public BaseNode
{
public:
   MatchNode(Errpos ep, Block bb, Node pred);
   void write(ostream & code);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);

private:

   /** Basic block that owns this node. */
   Block bb;

   /** Predicate to select eligible elements. */
   Node pred;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** Select statement: defines policy, points to options. */
class SelectNode : public BaseNode
{
public:
   SelectNode(Errpos ep, Policy policy, List options);
   void bind(Node p);
   void check(CheckData & cd);
   void gen(GenData gd);
   void genBlocks(BlockList & blocks, bool storeBlock = false);
   List extract(int & tempNum);
   Node lookUp(string value, Errpos ep);
   void write(ostream & code);
   void writeParts(ostream & code, WriteMode wm);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
   LTS *processGraph(Node portDec, int loopEnd);
private:

   /** Fair/Ordered/Random. */
   Policy policy;

   /** List of branches. */
   List options;

   /** Number of branches. */
   int numBranches;

   /** Unique ID for this statement. */
   int selNum;

   /** Address for start of select statement. */
   int selectStart;

   /** Address for first guard test. */
   //int firstGuard;

   /** Address for end of select statement. */
   int selectEnd;

   /** Name of the containing closure. */
   string owner;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** One branch of a select statement. */
class OptionNode : public BaseNode
{
public:
   OptionNode(Errpos ep, Policy policy, Node guard, Node seq);
   void bind(Node p);
   void check(CheckData & cd);
   void gen(GenData gd);
   List extract(int & tempNum);
   void genBlocks(BlockList & blocks, bool storeBlock = false);
   void write(ostream & code);
   void writeParts(ostream & code, WriteMode wm);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
   LTS *processGraph(Node portDec, int loopEnd);
private:

   /** Fair/Ordered/Random. */
   Policy policy;

   /** Boolean expression - maybe 0. */
   Node guard;

   /** Actions following guard. */
   Node seq;

   /** Unique number of the select statement. */
   int selNum;

   /** Address to test guard. */
   int testGuard;

   /** Address to execute branch. */
   int execBranch;

   /** Address for start of select statement. */
   int selectStart;

   /** Address of end of select statement. */
   int selectEnd;

   /** Name of the containing closure. */
   string owner;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** An expression p.f, where p is a port name
  * and f is a field of p's protocol.
  */
class DotNode : public BaseNode
{
public:
   DotNode(Errpos ep, Node port, Node field);
   void bind(Node p);
   void check(CheckData & cd);
   void gen(GenData gd);
   void setDefining(bool def);
   FileMode checkSysIO() const;
   string getNameString() const;
   string getCTypeString() const;
   string getPortBufferName() const;
   PortKind getPortKind(int slotNum = 0) const;
   MessageKind getMessKind() const;
   int getFieldNum() const;
   Node getProtocol();
   Node getPort() const;
   Node getField() const;
   Node getType() const;
   bool isConstant() const;
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
   LTS *processGraph(Node portDec, int loopEnd);
private:

   /** The 'p' in 'p.f'. */
   Node port;

   /** The 'f' in 'p.f'. */
   Node field;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** An expression p?f, where p is a port name
  * and f is a field of p's protocol.
  */
class QueryNode : public BaseNode
{
public:
   QueryNode(Errpos ep, Node port, Node field, Node name, int phase);
   void bind(Node p);
   void check(CheckData & cd);
   void gen(GenData gd);
   void genBlocks(BlockList & blocks, bool storeBlock = false);
   void setDefining(bool def);
   FileMode checkSysIO() const;
   string getNameString() const;
   string getCTypeString() const;
   string getPortBufferName() const;
   PortKind getPortKind(int slotNum = 0) const;
   MessageKind getMessKind() const;
   int getFieldNum() const;
   Node getProtocol();
   Node getPort() const;
   Node getField() const;
   Node getType() const;
   bool isConstant() const;
   void write(ostream & code);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
   LTS *processGraph(Node portDec, int loopEnd);
private:

   /** The 'p' in 'p?f'. */
   Node port;

   /** The 'f' in 'p?f'. */
   Node field;

   /** The variable name to which p?f gets assigned. */
   Node name;

   /** Determines how to generate code for a QueryNode. */
   int phase;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** Node for pervasive constant definitions. */
class ConstantNode : public BaseNode
{
public:
   ConstantNode(Errpos ep, Node name, Node type, Node value, bool pervasive);
   void bind(Node p);
   void check(CheckData & cd);
   void gen(GenData gd);
   void genBlocks(BlockList & blocks, bool storeBlock = false);
   void writeParts(ostream & code, WriteMode wm);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
   bool isPervasive() const;
   bool isConstant() const;
   int getVarNum() const;
   int getEVMBlockNumber() const;
   string getOwner() const;
   string getCTypeString() const;
   Node lookUp(string value, Errpos ep);
   Node getType() const;

private:

   /** The name being declared - always present. */
   Node name;

   /** The type - always present. */
   Node type;

   /** The value - always present. */
   Node value;

   /** \a True if this is a pervasive constant. */
   bool pervasive;

   /** Block number for EVM code. */
   int evmBlockNum;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** Node used for constant, variable, and port declarations, and
  * also for assignments.
  * This node replaces VarDecNode and PortDecNode. 080226
  */
class DecNode : public BaseNode
{
public:
   DecNode(Errpos ep, Node name, Node type, Node value,
           PortKind portDir, MessageKind mk, bool generated, bool constant, bool move, bool alias);
   void bind(Node p);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
   void check(CheckData & cd);
   void gen(GenData gd);
   void genBlocks(BlockList & blocks, bool storeBlock = false);
   void setReference();
   void setParameter();
   void setTie(Node t);
   void write(ostream & code);
   void writeParts(ostream & code, WriteMode wm);
   void getx(int & fn) const;
   bool isReply() const;
   bool isReference() const;
   bool isMap() const;
   bool isPort() const;
   bool isConstant() const;
   bool isEnumVal() const;
   bool isAlias() const;
   int getFieldNum() const;
   int getVarNum() const;
   int getEVMBlockNumber() const;
   Node lookUp(string value, Errpos ep);
   Node getType() const;
   Node getProtocol();
   Node getValue() const;
   Node getTie() const;
   Node getDomainType() const;
   Node getRangeType() const;
   List extract(int & tempNum);
   string getNameString() const;
   string getCTypeString() const;
   string getEVMTypeCode() const;
   string getFullName(bool withPointer) const;
   string getOwner() const;
   string getPortBufferName() const;
   string getQueueTest() const;
   PortKind getPortKind(int slotNum) const;
   MessageKind getMessKind() const;
   LTS *protocolGraph(set<Node> & fieldDecs);
   LTS *processGraph(Node portDec, int loopEnd);

private:

   /** The declared name, or LHS of an assignment. */
   Node name;

   /** The type of a variable, or protocol of a port.
    * This node is the defining instance of the name iff type is non-null.
    */
   Node type;

   /** The initial value of a declaration, or RHS of an assignment. */
   Node value;

   /** The port kind is one of NO_ROLE, CHANNEL, CLIENT, or SERVER.
    * It is set by the constructor.
    */
   PortKind portDir;

   /** The message kind is one of NO_FIELD, QUERY, REPLY.
    * It is set by the constructor.
    */
   MessageKind mk;

   /** The communication is one of NO_COMM, SEND, or RECEIVE.
    * It is set by DecNode::check()
    */
   CommKind ck;

   /** This is a reference variable. */
   bool reference;

   /** This is a process or cell parameter. */
   bool parameter;

   /** This declaration was generated by the compiler. */
   bool generated;

   /** This is the declaration of a constant. */
   bool constant;

   /** The declaration has the form 'x : T <- y'. */
   bool move;

   /** The declaration is preced by 'alias'. */
   bool alias;

   /** The is the first statement of a select branch. */
   bool branch;

   /** The unique number of a temporary variable introduced by the compiler. */
   int tempnum;

   /** The transfer address for a successful test. */
   int transfer;

   /** The transfer address for the end of a select statement. */
   int selectEnd;

//   /** Block number for EVM code. */
//   int evmBlockNum;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
   int getOffset() const;
   int getTypeCode() const
   {
      return type ? type->getTypeCode() : name->getTypeCode();
   }
};

/** Boolean literal. */
class BoolNode : public BaseNode
{
public:
   BoolNode(Errpos ep, bool value);
   void check(CheckData & cd);
   void write(ostream & code);
   void genBlocks(BlockList & blocks, bool storeBlock = false);
   Node getType() const;
   string getCTypeString() const;
   string getConstValue() const;
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
private:

   /** Value of Boolean constant. */
   bool value;

   /** Basic block for this node: set only if the expression
     * must be written in a special way. */
   Block bb;

   /** Block number for EVM code. */
   int evmBlockNum;

   /** This literal does not yet have a data block in the EVM code. */
   bool noBlockYet;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
   int getTypeCode() const
   {
      return TYPE_BOOL;
   }
};

/** Character literal. */
class CharNode : public BaseNode
{
public:
   CharNode(Errpos ep, char value);
   void check(CheckData & cd);
   void gen(GenData gd);
   void write(ostream & code);
   void writeParts(ostream & code, WriteMode wm);
   Node getType() const;
   string getCTypeString() const;
   string getNameString() const;
   string getConstValue() const;
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
private:

   /** Value of Char constant. */
   char value;

   /** Block number for EVM code. */
   int evmBlockNum;

   /** This literal does not yet have a data block in the EVM code. */
   bool noBlockYet;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** Text literal. */
class TextNode : public BaseNode
{
public:
   TextNode(Errpos ep, string value);
   void check(CheckData & cd);
   void gen(GenData gd);
   void write(ostream & code);
   void writeParts(ostream & code, WriteMode wm);
   Node getType() const;
   Node getDomainType() const;
   Node getRangeType() const;
   string getCTypeString() const;
   string getNameString() const;
   string getConstValue() const;
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
private:

   /** Value of Text constant. */
   string value;

   /** Block number for EVM code. */
   int evmBlockNum;

   /** This literal does not yet have a data block in the EVM code. */
   bool noBlockYet;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** Numeric literal: may be Integer, Float, or Decimal. */
class NumNode : public BaseNode
{
public:
   NumNode(Errpos ep, Node type, string value);
   void check(CheckData & cd);
   void write(ostream & code);
   void writeParts(ostream & code, WriteMode wm);
   void gen(GenData gd);
   void negate();
   Node getType() const;
   string getCTypeString() const;
   string getConstValue() const;
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
   int getIntVal() const;
   double getDecVal() const;
private:

   /** The type of a NumNode is either theIntegerNode if the value has no
     * decimal point and no exponent, or theDecimalNode if the value has
     * either a decimal point or an exponent.
     */
   Node type;

   /** Value of Integer constant. */
   string value;

   /** Block number for EVM code. */
   int evmBlockNum;

   /** This literal does not yet have a data block in the EVM code. */
   bool noBlockYet;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
   int getTypeCode() const
   {
      return type->getTypeCode();
   }
};

/** Root of a list operator expression.
  * Protocols use list operators.
  */
class ListopNode : public BaseNode
{
public:
   ListopNode(Errpos ep, Operator op, List args);
   void bind(Node p);
   void check(CheckData & cd);
   void gen(GenData gd);
   void genBlocks(BlockList & blocks, bool storeBlock = false);
   void prettyPrint(ostream & os, int level = 0) const;
   void getx(int & fn) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
   Node lookUp(string value, Errpos ep);
   List getArgList() const;
   Operator getOp() const;
   LTS *protocolGraph(set<Node> & fieldDecs);

private:

   /** The list operator */
   Operator op;

   /** The operands */
   List args;


// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** Root of a binary operator expression. */
class BinopNode : public BaseNode
{
public:
   BinopNode(Errpos ep, Operator op, Node lhs, Node rhs);
   void bind(Node p);
   void check(CheckData & cd);
   void gen(GenData gd);
   void genBlocks(BlockList & blocks, bool storeBlock = false);
   void writeParts(ostream & code, WriteMode wm);
   void write(ostream & code);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   void getx(int & fn) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
   string getCTypeString() const;
   Node lookUp(string value, Errpos ep);
   Node getType() const;
   List extract(int & tempNum);
   Operator getOp() const;
   LTS *processGraph(Node portDec, int loopEnd);

private:

   /** The binary operator. */
   Operator op;

   /** Left operand. */
   Node lhs;

   /** Right operand. */
   Node rhs;

   /** Type of expression. */
   Node type;

   /** Pointer to the basic block containing this node.
     * It is used only if special transfer addresses are
     * required when the block is written. */
   Block bb;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
   Node getLHS() const
   {
      return lhs;
   }
   Node getRHS() const
   {
      return rhs;
   }
   int getTypeCode() const
   {
      // Return the type code of the operands, not Boolean. */
      return lhs->getTypeCode();
   }
};

/** Root of a conditional expression, 'x if p else y. */
class CondExprNode : public BaseNode
{
public:
   CondExprNode(Errpos ep, Node lhs, Node pred, Node rhs);
   void bind(Node p);
   void check(CheckData & cd);
   void gen(GenData gd);
   void genBlocks(BlockList & blocks, bool storeBlock = false);
   void writeParts(ostream & code, WriteMode wm);
   void write(ostream & code);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
   string getCTypeString() const;
   Node lookUp(string value, Errpos ep);
   Node getType() const;
   List extract(int & tempNum);
   LTS *processGraph(Node portDec, int loopEnd);

private:

   /** Left operand. */
   Node lhs;

   /** Predicate. */
   Node pred;

   /** Right operand. */
   Node rhs;

   /** Type of expression. */
   Node type;

   /** Pointer to the basic block containing this node.
     * It is used only if special transfer addresses are
     * required when the block is written. */
   Block bb;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** Root of a unary operator expression/ */
class UnopNode : public BaseNode
{
public:
   UnopNode(Errpos ep, Operator op, Node operand);
   void bind(Node p);
   void check(CheckData & cd);
   void gen(GenData gd);
   void genBlocks(BlockList & blocks, bool storeBlock = false);
   void write(ostream & code);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   void getx(int & fn) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
   string getCTypeString() const;
   Node getType() const;
   Node getArg() const;
   Node lookUp(string value, Errpos ep);
   List extract(int & tempNum);
   Operator getOp() const;
   LTS *protocolGraph(set<Node> & fieldDecs);
   LTS *processGraph(Node portDec, int loopEnd);

private:

   /** The unary operator. */
   Operator op;

   /** The single operand. */
   Node operand;

   /** Type of operand. */
   Node type;

   /** Pointer to the basic block containing this node.
     * It is used only if special transfer addresses are
     * required when the block is written. */
   Block bb;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** Subscript expression */
class SubscriptNode : public BaseNode
{
public:
   SubscriptNode(Errpos ep, Node base, Node sub, bool lvalue);
   void bind(Node p);
   void check(CheckData & cd);
   void gen(GenData gd);
   void genBlocks(BlockList & blocks, bool storeBlock = false);
   void write(ostream & code);
   void setDefining(bool def);
   void writeExistCheck(ostream & code); // patch
   bool isPort() const;
   bool isConstant() const;
   PortKind getPortKind(int slotNum) const;
   Node getType() const;
   Node getRangeType() const;
   Node getDomainType() const;
   Node getDefinition() const;
   List extract(int & tempNum);
   Node lookUp(string value, Errpos ep);
   string getCTypeString() const;
   string getNameString() const;
   string getFullName(bool withPointer) const;
   string getOwner() const;
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
   LTS *processGraph(Node portDec, int loopEnd);

private:

   /** Indexed variable name */
   Node base;

   /** Subscript */
   Node sub;

   /** The expression is used as an lvalue. */
   bool lvalue;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** Subrange expression: a[i..j] */
class SubrangeNode : public BaseNode
{
public:
   SubrangeNode(Errpos ep, Node base, Node sub1, Node sub2, bool lvalue);
   void bind(Node p);
   void check(CheckData & cd);
   void gen(GenData gd);
   void genBlocks(BlockList & blocks, bool storeBlock = false);
   void write(ostream & code);
   Node getType() const;
   List extract(int & tempNum);
   Node lookUp(string value, Errpos ep);
   string getCTypeString() const;
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
private:

   /** Indexed variable name */
   Node base;

   /** First subscript */
   Node sub1;

   /** Second subscript */
   Node sub2;

   /** The expression is used as an lvalue. */
   bool lvalue;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** Iterator statements and expressions */
class IteratorNode : public BaseNode
{
public:
   IteratorNode(Errpos ep, Node map, IterFun fun);
   void bind(Node p);
   void check(CheckData & cd);
   List extract(int & tempNum);
   Node lookUp(string value, Errpos ep);
   Node getType() const;
   void gen(GenData gd);
   void genBlocks(BlockList & blocks, bool storeBlock = false);
   void write(ostream & code);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
private:

   /** The identifier node for the map. */
   Node map;

   /** The iterator function (START, FINISH, VALUE, STEP). */
   IterFun fun;

   /** Pointer to the basic block containing this node.
     * It is used only if special transfer addresses are
     * required when the block is written. */
   Block bb;

   /** Name of the containing closure. */
   string owner;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** The basic type Void. */
class VoidTypeNode : public BaseNode
{
public:
   VoidTypeNode() : BaseNode(Errpos(), VOID_TYPE_NODE) {}
   void check(CheckData & cd);
   void write(ostream & code);
   void writeParts(ostream & code, WriteMode wm);
   string getCTypeString() const;
   string getEType() const;
   string getEVMTypeCode() const;
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
   string getDefaultValue() const
   {
      return "DEFAULT VALUE OF VOID!";
   }
   Node getType() const
   {
      return theVoidNode;
   }
};

/** The basic type Bool. */
class BoolTypeNode : public BaseNode
{
public:
   BoolTypeNode() : BaseNode(Errpos(), BOOL_TYPE_NODE) {}
   void check(CheckData & cd);
   void write(ostream & code);
   void writeParts(ostream & code, WriteMode wm);
   string getPortBufferName() const;
   bool isBoolType() const;
   string getCTypeString() const;
   string getEType() const;
   string getEVMTypeCode() const;
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
   string getDefaultValue() const
   {
      return "false";
   }
   Node getType() const
   {
      return theBoolNode;
   }
   int getTypeCode() const
   {
      return TYPE_BOOL;
   }

// Lightning related stuff
   int getSize() const;
};

/** The basic type Byte. */
class ByteTypeNode : public BaseNode
{
public:
   ByteTypeNode(bool unSigned = false) : BaseNode(Errpos(), BYTE_TYPE_NODE), unSigned(unSigned) {}
   void check(CheckData & cd);
   void write(ostream & code);
   void writeParts(ostream & code, WriteMode wm);
   string getPortBufferName() const;
   bool isNumericType() const;
   string getCTypeString() const;
   string getEType() const;
   string getEVMTypeCode() const;
   string getDefaultValue() const
   {
      return "0";
   }
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
   Node getType() const
   {
      return theByteNode;
   }
   int getTypeCode() const
   {
      return unSigned ? TYPE_UNS_BYTE : TYPE_BYTE;
   }

private:
   bool unSigned;

// Lightning related stuff
   int getSize() const;
};

/** The basic type Integer. */
class IntegerTypeNode : public BaseNode
{
public:
   IntegerTypeNode(bool unSigned = false) : BaseNode(Errpos(), INTEGER_TYPE_NODE), unSigned(unSigned) {}
   void check(CheckData & cd);
   void write(ostream & code);
   void writeParts(ostream & code, WriteMode wm);
   string getPortBufferName() const;
   bool isNumericType() const;
   string getCTypeString() const;
   string getEType() const;
   string getEVMTypeCode() const;
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
   string getDefaultValue() const
   {
      return "0";
   }
   Node getType() const
   {
      return theIntegerNode;
   }
   int getTypeCode() const
   {
      return unSigned ? TYPE_UNS_INT : TYPE_INT;
   }

private:
   bool unSigned;

// Lightning related stuff
   int getSize() const;
};

/** The basic type Float. */
class FloatTypeNode : public BaseNode
{
public:
   FloatTypeNode() : BaseNode(Errpos(), FLOAT_TYPE_NODE) {}
   void check(CheckData & cd);
   void write(ostream & code);
   void writeParts(ostream & code, WriteMode wm);
   string getPortBufferName() const;
   bool isNumericType() const;
   string getCTypeString() const;
   string getEType() const;
   string getEVMTypeCode() const;
   string getDefaultValue() const
   {
      return "0.0";
   }
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
   Node getType() const
   {
      return theFloatNode;
   }
   int getTypeCode() const
   {
      return TYPE_FLO;
   }

// Lightning related stuff
   int getSize() const;
};

/** The basic type Decimal. */
class DecimalTypeNode : public BaseNode
{
public:
   DecimalTypeNode() : BaseNode(Errpos(), DECIMAL_TYPE_NODE) {}
   void check(CheckData & cd);
   void write(ostream & code);
   void writeParts(ostream & code, WriteMode wm);
   string getPortBufferName() const;
   bool isNumericType() const;
   string getCTypeString() const;
   string getEType() const;
   string getEVMTypeCode() const;
   string getDefaultValue() const
   {
      return "0.0";
   }
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
   Node getType() const
   {
      return theDecimalNode;
   }
   int getTypeCode() const
   {
      return TYPE_DEC;
   }

// Lightning related stuff
   int getSize() const;
};

/** The basic type Char. */
class CharTypeNode : public BaseNode
{
public:
   CharTypeNode() : BaseNode(Errpos(), CHAR_TYPE_NODE) {}
   void check(CheckData & cd);
   void write(ostream & code);
   void writeParts(ostream & code, WriteMode wm);
   string getPortBufferName() const;
   string getCTypeString() const;
   string getEType() const;
   string getEVMTypeCode() const;
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
   string getDefaultValue() const
   {
      return "' '";
   }
   Node getType() const
   {
      return theCharNode;
   }
   int getTypeCode() const
   {
      return TYPE_CHAR;
   }

// Lightning related stuff
   int getSize() const;
};

/** The basic type Text. */
class TextTypeNode : public BaseNode
{
public:
   TextTypeNode() : BaseNode(Errpos(), TEXT_TYPE_NODE) {}
   void check(CheckData & cd);
   void write(ostream & code);
   void writeParts(ostream & code, WriteMode wm);
   string getPortBufferName() const;
   string getCTypeString() const;
   string getEType() const;
   string getEVMTypeCode() const;
   string getDefaultValue() const
   {
      return "\"\"";
   }
   Node getDomainType() const;
   Node getRangeType() const;
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
   Node getType() const
   {
      return theTextNode;
   }
   int getTypeCode() const
   {
      return TYPE_TEXT;
   }

// Lightning related stuff
   int getSize() const;
};

/** Enumerated type */
class EnumTypeNode : public BaseNode
{
public:
   EnumTypeNode() : BaseNode(Errpos(), ENUM_TYPE_NODE) {} // Used for theEnumNode only
   EnumTypeNode(Errpos ep, List values);
   void bind(Node p);
   void check(CheckData & cd);
   void genBlocks(BlockList & blocks, bool storeBlock = false);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   void writeParts(ostream & code, WriteMode wm);
   int getIntVal() const;
   bool isEnumType() const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
   Node lookUp(string value, Errpos ep);
   Node getType() const;
   Node getEnumSize() const;
   Node getDomainType() const;
   Node getRangeType() const;
   string getCTypeString() const;
   string getEType() const;
   string getEVMTypeCode() const;
   string getNameString() const;
   string getDefaultValue() const
   {
      return "0";
   }

private:

   /** Intenal name of the type. */
   string name;

   /** List of the EnumValueNode's of the enumeration. */
   List values;

// Lightning related stuff
   int getSize() const;
};

/** The basic type File. */
class FileTypeNode : public BaseNode
{
public:
   FileTypeNode(FileMode mode);
   void check(CheckData & cd);
   void write(ostream & code);
   void writeParts(ostream & code, WriteMode wm);
   string getPortBufferName() const;
   string getCTypeString() const;
   string getEType() const;
   string getEVMTypeCode() const;
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
   string getDefaultValue() const
   {
      return "0";
   }

private:

   /** Mode is FILE_INPUT or FILE_OUTPUT. */
   FileMode mode;

// Lightning related stuff
   int getSize() const;
};

/** Enumerated value */
class EnumValueNode : public BaseNode
{
public:
   EnumValueNode(Errpos ep, string name, int varNum);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
   void setType(Node type);
   string getNameString() const;
   string getOwner() const;
   string getCTypeString() const;
   void bind(Node p);
   void check(CheckData & cd);
   void genBlocks(BlockList & blocks, bool storeBlock = false);
   Node lookUp(string value, Errpos ep);
   Node getType() const;
   int getVarNum() const;
   int getEVMBlockNumber() const;

private:

   /** The name of the enumerated value. */
   string name;

   /** The position within the definition, numbered from 0. */
   int varNum;

   /** The type refers back to the EnumTypeNode. */
   Node type;

   /** Block number for EVM code. */
   int evmBlockNum;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** Array types. */
class ArrayTypeNode : public BaseNode
{
public:
   ArrayTypeNode(Errpos ep, Node rangeType, Node lo, Node hi);
   void bind(Node p);
   void check(CheckData & cd);
   void gen(GenData gd);
   bool isArrayType() const;
   string getPortBufferName() const;
   string getCTypeString() const;
   string getEType() const;
   string getEVMTypeCode() const;
   Node getLo() const;
   Node getHi() const;
   Node getType() const;
   Node getDomainType() const;
   Node getRangeType() const;
   Node getProtocol();
   List extract(int & tempNum);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
   string getDefaultValue() const
   {
      return getCTypeString() + "()";
   }

private:

   /** The range type of the array. */
   Node rangeType;

   /** Expression for low bound. */
   Node lo;

   /** Expression for high bound. */
   Node hi;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** Map (indexed) types. */
class MapTypeNode : public BaseNode
{
public:
   MapTypeNode(Errpos ep, Node domainType, Node rangeType,
               PortKind portDir = NO_ROLE);
   void bind(Node p);
   void check(CheckData & cd);
   string getCTypeString() const;
   string getEVMTypeCode() const;
   bool isMap() const;
   bool isPort() const;
   bool isMapType() const;
   Node getType() const;
   Node getDomainType() const;
   Node getRangeType() const;
   Node getProtocol();
   PortKind getPortKind(int slotNum) const;
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
   string getDefaultValue() const
   {
      return getCTypeString() + "()";
   }

private:
   Node domainType;
   Node rangeType;
   PortKind portDir;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
   int getSize() const;
};

/** Iterator for traversing maps. */
class IterTypeNode : public BaseNode
{
public:
   IterTypeNode(Errpos ep, Node domainType, Node rangeType);
   bool isIterType() const;
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   Node getDomainType() const;
   Node getRangeType() const;
   string getEVMTypeCode() const;
private:
   Node domainType;
   Node rangeType;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** The root of a function call expression.
  * Either 'name' is a non-empty string and 'desc' == 0,
  * or 'name' == "" and 'desc' points to a descriptor.
  */
class FunctionNode : public BaseNode
{
public:
   FunctionNode(Errpos ep, Node name, List args);
   FunctionNode(Errpos ep, FuncDef *desc, List args);
   void bind(Node p);
   void check(CheckData & cd);
   void gen(GenData gd);
   void genBlocks(BlockList & blocks, bool storeBlock = false);
   void write(ostream & code);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
   int getFieldNum() const;
   string getCTypeString() const;
   string getFun() const;
   Node getType() const;
   Node getPort() const;
   Node lookUp(string value, Errpos ep);
   List extract(int & tempNum);
   LTS *processGraph(Node portDec, int loopEnd);

private:

   /** Name of the function. */
   Node name;

   /** Arguments of function */
   List args;

   /** Function descriptor: set by check(). */
   FuncDef *desc;

   /** Pointer to the basic block containing this node.
     * It is used only if special transfer addresses are
     * required when the block is written. */
   Block bb;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** A defining or defined occurrence of an identifier. */
class NameNode : public BaseNode
{
public:
   NameNode(Errpos ep, string value, Node definition, bool definingOccurrence);
   void setDefining(bool def);
   void bind(Node p);
   void tryBind(Node p);
   void check(CheckData & cd);
   void gen(GenData gd);
   void genBlocks(BlockList & blocks, bool storeBlock = false);
   void writeParts(ostream & code, WriteMode wm);
   void write(ostream & code);
   void setType(Node newType);
   void setTie(Node t);
   void setPassByReference();
   void setFieldNum(int fn);
   void setOffset(int os);
   void getIterator(Node iter, MapSetKind & msKind, Node & it) const;
   void getx(int & fn) const;
   string getLoopVar(Node var = 0) const;
   bool isReply() const;
   bool isReference() const;
   bool isMap() const;
   bool isMapType() const;
   bool isArrayType() const;
   bool isEnumType() const;
   bool isPort() const;
   bool isConstant() const;
   bool assignable() const;
   bool isPervasive() const;
   bool isEnumVal() const;
   bool isIterator() const;
   bool isAlias() const;
   bool isPassByReference() const;
   Node getDomainType() const;
   Node getRangeType() const;
   Node getPort() const;
   int getFieldNum() const;
   int getNumFields() const;
   int getVarNum() const;
   int getEVMBlockNumber() const;
   string getNameString() const;
   string getFullName(bool withpointer = false) const;
   string getOwner() const;
   string getCommunicator() const;
   string getPortBufferName() const;
   string getCTypeString() const;
   string getEType() const;
   string getEVMTypeCode() const;
   Node getDefinition() const;
   Node getType() const;
   Node getProtocol();
   Node getValue() const;
   Node getTie() const;
   Node getField() const;
   Node getEnumSize() const;
   PortKind getPortKind(int slotNum = 0) const;
   MessageKind getMessKind() const;
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
   LTS *processGraph(Node portDec, int loopEnd);

private:

   /** \a True if this is a defining occurrence of the name. */
   bool definingOccurrence;

   /** True if this name must be passed by reference to a cell or closure. */
   bool passByReference;

   /** The actual name, as a string. */
   string value;

   /** Pointer to definition/declaration, added during binding phase. */
   Node definition;

   /** The type of the name.  This is set only if definition = 0. */
   Node type;

   /** Name of the entity that owns this name. */
   string owner;

   /** Sequence number to ensure names are unique. */
   int varNum;

   /** Field number of a message in a protocol. */
   int fieldNum;

   /** Corresponding fields in different protocols must have the same field
     * number.  The constructor sets tie=this.  If this name corresponds with
     * another, \a tie will be set pointing to the other name node,
     * and vice versa. */
   Node tie;

   /** Basic block for this node: set only if the expression
     * must be written in a special way. */
   Block bb;

   /** Block number for EVM code.
     * Valid only for a defining occurrence, otherwise -1.
     */
   int evmBlockNum;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
   int getOffset() const;
   int getTypeCode() const
   {
      if (definingOccurrence)
         return type ? type->getTypeCode() : 0;
      else
         return definition ? definition->getTypeCode() : 0;
   }

private:

   /** Addres relative to stack base for Lightning assembly. */
   int offset;
};

// The following nodes are generated by later phses of the compiler,
// rather than the parser.

/** A node stored in a sequence to indicate that a process has
  * terminated and should be removed from the active process list.
  */
class RemoveNode : public BaseNode
{
public:
   RemoveNode();
   void write(ostream & code);
   void prettyPrint(ostream & os, int indent = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** A node generated to send a message unconditionally. */
class SendNode : public BaseNode
{
public:
   SendNode(Errpos ep, Node rhs, Node port, string tempName,
            string bufferName, int fieldNum, FileMode mode = SYS_NULL);
   void prettyPrint(ostream & os, int indent = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
   void write(ostream & code);

private:

   /** The value to be sent. */
   Node rhs;

   /** The port used to send the value. */
   Node port;

   /** The name of a temporary variable, if needed. */
   string tempName;

   /** The name of the buffer pointer in the port. */
   string bufferName;

   /** The field number in the protocol. */
   int fieldNum;

   /** Mode for system i/o. */
   FileMode mode;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** A node generated to send a message in a select statement. */
class SendOptionNode : public BaseNode
{
public:
   SendOptionNode(Errpos ep, Node rhs, Node port,
                  int fieldNum, string bufferName);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);
   void write(ostream & code);

private:

   /** The value to be sent. */
   Node rhs;

   /** The port through which to send it. */
   Node port;

   /** Name for temporary variable, if required. */
   string tempName;

   /** Name of pointe to buffer in the port. */
   string bufferName;

   /** Field number in protocol. */
   int fieldNum;

   /** Pointer to block for this node, indicating that special
     * transfer addresses are generated when this node is written. */
   BasicBlock *pbb;

   /** Mode for system i/o. */
   FileMode mode;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** A node generated to complete the receive operation. */
class ReceiveNode : public BaseNode
{
public:
   ReceiveNode(Errpos ep, Node lhs, Node port, Node type,
               string bufferName, bool signal = false,
               int fieldNum = 0, FileMode mode = SYS_NULL);
   void write(ostream & code);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);

private:

   /** The value to be received. */
   Node lhs;

   /** The port through which the value arrives. */
   Node port;

   /** The type, if this node originated as a declaration. */
   Node type;

   /** The name of the buffer pointer in the port. */
   string bufferName;

   /** The field number in the protocol. */
   int fieldNum;

   /** True if this is a signal (no data to receive). */
   bool signal;

   /** Mode for system i/o. */
   FileMode mode;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** A node generated to prepare for a receive operation
  * in a select statement.
  */
class ReceiveOptionNode : public BaseNode
{
public:
   ReceiveOptionNode(Errpos ep, Node lhs, Node port, Node type, int fieldNum,
                     string bufferName, bool signal = false);
   void write(ostream & code);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   bool drawAST(ostream & os, set<int> & nodeNums, int level);

private:

   /** The variable that will receive the value. */
   Node lhs;

   /** The port to be used for the transfer. */
   Node port;

   /** The type of data to be transferred. */
   Node type;

   /** The field number in the protocol. */
   int fieldNum;

   /** The name of the buffer pointer in the port. */
   string bufferName;

   /** True if this is a signal (no data to receive). */
   bool signal;

   /** Pointer to block for this node, indicating that special
     * transfer addresses are generated when this node is written. */
   BasicBlock *pbb;

   /** Mode for system i/o. */
   FileMode mode;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** Definition of a thread. */
class ThreadNode : public BaseNode
{
public:
   ThreadNode(Errpos ep, Node port, List inputs, List outputs, Node seq);
   void setName(string n);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   void bind(Node p);
   void check(CheckData & cd);
   void gen(GenData gd);
   void genBlocks(BlockList & blocks, bool storeBlock = false);
   void getThreadParams(List & inpars, List & outpars);
   void write(ostream & code);
   void writeParts(ostream & code, WriteMode wm);
   Node getType() const;
   List extract(int & tempNum);

private:

   /** Name of the thread. */
   string name;

   /** Name of the thread's port. */
   Node port;

   /** Input parameters. */
   List inputs;

   /** Output parameters. */
   List outputs;

   /** Statement sequence. */
   Node seq;

   /** Address of starting block. */
   int start;

   /** Totl number of parameters. */
   int numFields;

   /** Basic blocks for code. */
   BlockList blocks;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** Thread parameter. */
class ThreadParamNode : public BaseNode
{
public:
   ThreadParamNode(Errpos ep, Node name, PortKind portDir, Node type, Node port, bool input);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   void bind(Node p);
   void check(CheckData & cd);
   void gen(GenData gd);
   void genBlocks(BlockList & blocks, bool storeBlock = false);
   void write(ostream & code);
   void writeParts(ostream & code, WriteMode wm);
   bool isConstant() const;
   bool isPort() const;
   int getVarNum() const;
   string getOwner() const;
   string getCTypeString() const;
   string getFullName(bool withpointer = false) const;
   Node lookUp(string value, Errpos ep);
   Node getType() const;
   Node getProtocol();
   PortKind getPortKind(int slotNum = 0) const;

private:

   /** Unqualified name of parameter. */
   Node name;

   /** Direction, if the parameter is a port. */
   PortKind portDir;

   /** Type of parameter. */
   Node type;

   /** Thread's port. */
   Node port;

   /** Tells whether this is an input or an output parameter. */
   bool input;

   /** Transfer address for next block. */
   int transfer;

   /** Name of entity that owns this parameter. */
   string owner;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** Start statement. */
class StartNode : public BaseNode
{
public:
   StartNode(Errpos ep, List calls, Node seq);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   void bind(Node p);
   void check(CheckData & cd);
   void gen(GenData gd);
   void genBlocks(BlockList & blocks, bool storeBlock = false);
   void write(ostream & code);
   void writeParts(ostream & code, WriteMode wm);
   Node lookUp(string value, Errpos ep);
   List extract(int & tempNum);

private:

   /** Thread invocations. */
   List calls;

   /** Statement sequence. */
   Node seq;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** Thread invocation. */
class ThreadCallNode : public BaseNode
{
public:
   ThreadCallNode(Errpos ep, Node name, List inputs, List outputs);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   void bind(Node p);
   void check(CheckData & cd);
   void gen(GenData gd);
   void genBlocks(BlockList & blocks, bool storeBlock = false);
   void startThread(BlockList & blocks);
   void stopThread(BlockList & blocks);
   void write(ostream & code);
   void writeParts(ostream & code, WriteMode wm);
   List extract(int & tempNum);

private:

   /** Name of thread. */
   Node name;

   /** Arguments sent to thread. */
   List inputs;

   /** Results received from thread. */
   List outputs;

   /** Channel name. */
   Node chName;

   /** Number of fields. */
   int numFields;

   /** Block number to start thread. */
   int startTransfer;

   /** Block number to stop transfer. */
   int stopTransfer;

   /** First block number for inputs. */
   int inputTransfer;

   /** First block number for outputs. */
   int outputTransfer;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** Start a thread. */
class ThreadStartNode : public BaseNode
{
public:
   ThreadStartNode(Errpos ep, Node name, Node chName, int numFields);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   void write(ostream & code);

private:

   /** Name of thread to start. */
   Node name;

   /** Name of channel for communication with thread. */
   Node chName;

   /** Number of fields for communication. */
   int numFields;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

/** Stop a thread. */
class ThreadStopNode : public BaseNode
{
public:
   ThreadStopNode(Errpos ep, Node name, Node chName);
   void prettyPrint(ostream & os, int level = 0) const;
   void show(ostream & os, int level = 0) const;
   void write(ostream & code);

private:

   /** Name of thread to stop. */
   Node name;

   /** Name of channel for communication with thread. */
   Node chName;

// Lightning related stuff
public:
   void prepAssem(AssemData aData);
   void genAssem();
};

#endif

