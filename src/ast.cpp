/** \file ast.cpp
 *
 * This file contains constructors for AST nodes and some odds and ends.
 */

#include "ast.h"

/** Static member used to assign a unique number to each AST node. */
int BaseNode::nodeCount = 0;

// Nodes used by the compiler as basic type descriptors.
Node BaseNode::theVoidNode            = new VoidTypeNode();
Node BaseNode::theEnumNode            = new EnumTypeNode();
Node BaseNode::theBoolNode            = new BoolTypeNode();
Node BaseNode::theByteNode            = new ByteTypeNode();
Node BaseNode::theUnsignedByteNode    = new ByteTypeNode(true);
Node BaseNode::theIntegerNode         = new IntegerTypeNode();
Node BaseNode::theUnsignedIntegerNode = new IntegerTypeNode(true);
Node BaseNode::theFloatNode           = new FloatTypeNode();
Node BaseNode::theDecimalNode         = new DecimalTypeNode();
Node BaseNode::theCharNode            = new CharTypeNode();
Node BaseNode::theTextNode            = new TextTypeNode();
Node BaseNode::theArrayNode           = new ArrayTypeNode(Errpos(), BaseNode::theVoidNode, 0, 0);
Node BaseNode::theInputFileNode       = new FileTypeNode(FILE_INPUT);
Node BaseNode::theOutputFileNode      = new FileTypeNode(FILE_OUTPUT);

//Node BaseNode::theSignalNode  = new SignalTypeNode();

// BaseNode functions

/** Construct a base node for the AST.
  * \param ep gives the source code coordinates for the node.
  * \param kv is the kind of node.
  */
BaseNode::BaseNode(Errpos ep, NodeKind kv)
      : ep(ep), kv(kv), defChain(0), nodeNum(nodeCount++)
{}

// Non-virtual functions

/** Return the kind of this node. */
NodeKind BaseNode::kind() const
{
   return kv;
}

/** Return the sequence number of this node. */
int BaseNode::num() const
{
   return nodeNum;
}

/** Return the definition chain pointer of this node. */
Node BaseNode::getDefChain() const
{
   return defChain;
}

/** Set the definition chain pointer for this node. */
void BaseNode::setDefChain(Node pdc)
{
   defChain = pdc;
}

// Generate a fatal error so that the debugger dumps the stack.
void BaseNode::emergencyStop(string msg, Errpos ep) const
{
   cerr << "Compiler error: " << msg << ".  Sorry!\n" << ep;
   show(cerr, 4);
   Node p = 0;
   p->show(cerr);
}

/** Return the position for errors. */
Errpos BaseNode::getPos() const
{
   return ep;
}

/** Show node with message for debugging purposes. */
void BaseNode::showDebug(string msg) const
{
   cerr << endl << msg;
   show(cerr, 2);
   cerr << endl;
}

/** Show source code line.  Used for EVM code generation. */
void BaseNode::showLine(ostream & os)
{
   string line = string(ep.pLine);
//   os << "line " << ep.lineNum << ' ' << line.length() << ' ' << line << endl;
}

// Constructors for other node types.

ProgramNode::ProgramNode(Errpos ep, List nodes)
      : BaseNode(ep, PROGRAM_NODE), nodes(nodes)
{}

InstanceNode::InstanceNode(Errpos ep, Node name, List args, bool topLevel)
      : BaseNode(ep, INSTANCE_NODE), name(name), args(args),
      topLevel(topLevel)
{}

RemoveNode::RemoveNode() : BaseNode(Errpos(), REMOVE_NODE)
{}

ProcessNode::ProcessNode(Errpos ep, List params, Node seq)
      : BaseNode(ep, PROCESS_NODE), params(params), seq(seq), typeNum(0)
{}

CppNode::CppNode(Errpos ep, List params, Node type)
      : BaseNode(ep, CPP_NODE), params(params), type(type)
{}

ProcedureNode::ProcedureNode(Errpos ep, List params, Node seq)
      : BaseNode(ep, PROCEDURE_NODE), params(params), seq(seq)
{}

ProtocolNode::ProtocolNode(Errpos ep, Node expr)
      : BaseNode(ep, PROTOCOL_NODE), expr(expr), numFields(0), plts(0)
{}

DefNode::DefNode(Errpos ep, Node name, Node value, bool incremental)
      : BaseNode(ep, DEF_NODE), incremental(incremental),
      name(name), value(value)
{}

CellNode::CellNode(Errpos ep, List params, List instances)
      : BaseNode(ep, CELL_NODE), params(params), instances(instances)
{}

SequenceNode::SequenceNode(Errpos ep, List stmts)
      : BaseNode(ep, SEQUENCE_NODE), stmts(stmts)
{}

SkipNode::SkipNode(Errpos ep) : BaseNode(ep, SKIP_NODE)
{}

ExitNode::ExitNode(Errpos ep) : BaseNode(ep, EXIT_NODE)
{}

IfNode::IfNode(Errpos ep, List condPairs, Node alt)
      : BaseNode(ep, IF_NODE), condPairs(condPairs), alt(alt)
{}

CondPairNode::CondPairNode(Errpos ep, Node cond, Node seq)
      : BaseNode(ep, CONDPAIR_NODE), cond(cond), seq(seq), decs(List()), parent(0)
{}

LoopNode::LoopNode(Errpos ep, Node seq) : BaseNode(ep, LOOP_NODE), seq(seq)
{}

ForNode::ForNode(Errpos ep, Node comp, Node seq)
      : BaseNode(ep, FOR_NODE), comp(comp), seq(seq)
{}

AnyNode::AnyNode(Errpos ep, Node comp, Node seq, Node alt)
      : BaseNode(ep, ANY_NODE), comp(comp), seq(seq), alt(alt)
{}

ComprehensionNode::ComprehensionNode(Errpos ep, Node var, Node type,
                                     Node collection, Node pred)
      : BaseNode(ep, COMP_NODE), var(var), type(type),
      iter(new NameNode(var->getPos(), var->getNameString() + "_iter", 0, true)),
      collection(collection), pred(pred), iterType(0)
{}

RangeNode::RangeNode(Errpos ep, Node type, Node start, Node finish,
                     Node step, bool open, bool ascending)
      : BaseNode(ep, RANGE_NODE), type(type), start(start),
      finish(finish), step(step), finishNum(0), stepNum(0),
      open(open), ascending(ascending)
{}

RangeInitNode::RangeInitNode(Errpos ep, string owner, Node type, Node var,
                             Node start, Node finish, Node step,
                             int finishNum, int stepNum)
      : BaseNode(ep, RANGE_INIT_NODE), owner(owner), var(var), type(type),
      start(start), finish(finish), step(step),
      finishNum(finishNum), stepNum(stepNum)
{}

RangeTermNode::RangeTermNode(Errpos ep, string owner, Block bb, Node type, Node var,
                             Node step, int stepNum, Node finish, int finishNum,
                             bool open, bool ascending)
      : BaseNode(ep, RANGE_TERM_NODE), owner(owner), bb(bb), type(type), var(var),
      step(step), stepNum(stepNum), finish(finish), finishNum(finishNum),
      open(open), ascending(ascending)
{}

RangeStepNode::RangeStepNode(Errpos ep, string owner, Node type,
                             Node var, Node step, int stepNum, bool ascending)
      : BaseNode(ep, STEP_NODE), owner(owner), type(type), var(var),
      step(step), stepNum(stepNum), ascending(ascending)
{}

MapSetNode::MapSetNode(Errpos ep, MapSetKind setKind, Node map)
      : BaseNode(ep, MAP_SET_NODE), type(0), setKind(setKind),
      mapKind(NO_MAP_KIND), map(map), iter(0), indexName("")
{}

MapInitNode::MapInitNode(Errpos ep, string owner, Node type, Node var,
                         Node iter, MapKind mapKind, MapSetKind setKind, Node map, string indexName)
      : BaseNode(ep, MAP_INIT_NODE), owner(owner), type(type),
      var(var), iter(iter), mapKind(mapKind), setKind(setKind), map(map), indexName(indexName)
{}

MapTermNode::MapTermNode(Errpos ep, string owner, Block bb,
                         Node type, Node var, Node iter, MapKind mapKind, MapSetKind setKind, Node map, string indexName)
      : BaseNode(ep, MAP_TERM_NODE), owner(owner), bb(bb), type(type), var(var),
      iter(iter), mapKind(mapKind), setKind(setKind), map(map), indexName(indexName)
{}

MapStepNode::MapStepNode(Errpos ep, string owner, Node type, Node var,
                         Node iter, MapKind mapKind, MapSetKind setKind, Node map, string indexName)
      : BaseNode(ep, MAP_STEP_NODE), owner(owner), type(type), var(var),
      iter(iter), mapKind(mapKind), setKind(setKind), map(map), indexName(indexName)
{}

EnumSetNode::EnumSetNode(Errpos ep, Node type)
      : BaseNode(ep, ENUM_SET_NODE), type(type), max(0), owner("")
{}

EnumInitNode::EnumInitNode(Errpos ep, string owner, Node var)
      : BaseNode(ep, ENUM_INIT_NODE), owner(owner), var(var)
{}

EnumTermNode::EnumTermNode(Errpos ep, string owner, Block bb, Node var, Node max)
      : BaseNode(ep, ENUM_TERM_NODE), owner(owner), bb(bb), var(var), max(max)
{}

EnumStepNode::EnumStepNode(Errpos ep, string owner, Node var)
      : BaseNode(ep, MAP_STEP_NODE), owner(owner), var(var)
{}

MatchNode::MatchNode(Errpos ep, Block bb, Node pred)
      : BaseNode(ep, MATCH_NODE), bb(bb), pred(pred)
{}

SelectNode::SelectNode(Errpos ep, Policy policy, List options)
      : BaseNode(ep, SELECT_NODE), policy(policy), options(options),
      selNum(0), numBranches(0), selectStart(-1), selectEnd(-1), owner("")
      //firstGuard(-1),
{}

OptionNode::OptionNode(Errpos ep, Policy policy, Node guard, Node seq)
      : BaseNode(ep, OPTION_NODE), policy(policy), guard(guard), seq(seq),
      testGuard(-1), execBranch(-1), selNum(0), owner(""),
      selectStart(-1), selectEnd(-1)
{}

DotNode::DotNode(Errpos ep, Node port, Node field)
      : BaseNode(ep, DOT_NODE), port(port), field(field)
{}

QueryNode::QueryNode(Errpos ep, Node port, Node field, Node name, int phase)
      : BaseNode(ep, QUERY_NODE), port(port), field(field), name(name), phase(phase)
{}

BoolNode::BoolNode(Errpos ep, bool value)
      : BaseNode(ep, BOOL_NODE), value(value), bb(0),
      evmBlockNum(0), noBlockYet(true)
{}

CharNode::CharNode(Errpos ep, char value)
      : BaseNode(ep, CHAR_NODE), value(value),
      evmBlockNum(0), noBlockYet(true)
{}

TextNode::TextNode(Errpos ep, string value)
      : BaseNode(ep, TEXT_NODE), value(value),
      evmBlockNum(0), noBlockYet(true)
{}

NumNode::NumNode(Errpos ep, Node type, string value)
      : BaseNode(ep, NUM_NODE), type(type), value(value),
      evmBlockNum(0), noBlockYet(true)
{}

ListopNode::ListopNode(Errpos ep, Operator op, List args) :
      BaseNode(ep, LISTOP_NODE), op(op), args(args)
{}

BinopNode::BinopNode(Errpos ep, Operator op, Node lhs, Node rhs) :
      BaseNode(ep, BINOP_NODE), op(op), lhs(lhs), rhs(rhs), type(0), bb(0)
{}

CondExprNode::CondExprNode(Errpos ep, Node lhs, Node pred, Node rhs) :
   BaseNode(ep, CONDEXPR_NODE), lhs(lhs), pred(pred), rhs(rhs), type(0), bb(0)
{}

UnopNode::UnopNode(Errpos ep, Operator op, Node operand)
      : BaseNode(ep, UNOP_NODE), op(op), operand(operand),
      type(0), bb(0)
{}

SubscriptNode::SubscriptNode(Errpos ep, Node base, Node sub, bool lvalue) :
      BaseNode(ep, SUBSCRIPT_NODE), base(base), sub(sub), lvalue(lvalue)
{}

SubrangeNode::SubrangeNode(Errpos ep, Node base,
                           Node sub1, Node sub2, bool lvalue)
      :      BaseNode(ep, SUBRANGE_NODE), base(base), sub1(sub1),
      sub2(sub2), lvalue(lvalue)
{}

IteratorNode::IteratorNode(Errpos ep, Node map, IterFun fun) :
      BaseNode(ep, ITERATOR_NODE), map(map), fun(fun), bb(0), owner("")
{}

ConstantNode::ConstantNode(Errpos ep, Node name, Node type,
                           Node value, bool pervasive)
      : BaseNode(ep, CONSTANT_NODE), name(name), type(type),
      value(value), pervasive(pervasive), evmBlockNum(0)
{}

DecNode::DecNode(Errpos ep, Node name, Node type, Node value,
                 PortKind portDir, MessageKind mk, bool generated,
                 bool constant, bool move, bool alias) :
      BaseNode(ep, DEC_NODE),
      name(name),
      type(type),
      value(value),
      portDir(portDir),
      mk(mk),
      ck(NO_COMM),
      generated(generated),
      constant(constant),
      move(move),
      alias(alias),
      reference(false),
      parameter(false),
      branch(false),
      tempnum(0),
      transfer(0),
      selectEnd(0)
{}

EnumTypeNode::EnumTypeNode(Errpos ep, List values)
      : BaseNode(ep, ENUM_TYPE_NODE), values(values), name("")
{}

FileTypeNode::FileTypeNode(FileMode mode)
      : BaseNode(Errpos(), FILE_TYPE_NODE), mode(mode)
{}

EnumValueNode::EnumValueNode(Errpos ep, string name, int varNum)
      : BaseNode(ep, ENUM_VALUE_NODE), name(name), varNum(varNum), type(0), evmBlockNum(0)
{}

ArrayTypeNode::ArrayTypeNode(Errpos ep, Node rangeType, Node lo, Node hi)
  : BaseNode(ep, ARRAY_TYPE_NODE), rangeType(rangeType), lo(lo), hi(hi) {}

MapTypeNode::MapTypeNode(Errpos ep, Node domainType,
                         Node rangeType, PortKind portDir)
      : BaseNode(ep, MAP_TYPE_NODE), domainType(domainType),
      rangeType(rangeType), portDir(portDir)
{}

IterTypeNode::IterTypeNode(Errpos ep, Node domainType, Node rangeType)
      : BaseNode(ep, ITER_TYPE_NODE), domainType(domainType),
      rangeType(rangeType)
{}

FunctionNode::FunctionNode(Errpos ep, Node name, List args)
      : BaseNode(ep, FUNC_NODE), name(name), desc(0), args(args), bb(0)
{}

FunctionNode::FunctionNode(Errpos ep, FuncDef *desc, List args)
      : BaseNode(ep, FUNC_NODE), name(0), desc(desc), args(args), bb(0)
{}

NameNode::NameNode(Errpos ep, string value,
                   Node definition, bool definingOccurrence)
      : BaseNode(ep, NAME_NODE), value(value),
      definition(definition), definingOccurrence(definingOccurrence),
      varNum(0), fieldNum(-1), bb(0), tie(this), passByReference(false),
      type(0), evmBlockNum(-1), offset(0)
{}

SendNode::SendNode(Errpos ep, Node rhs, Node port, string tempName,
                   string bufferName, int fieldNum, FileMode mode)
      : BaseNode(ep, SEND_NODE), rhs(rhs), port(port), tempName(tempName),
      bufferName(bufferName), fieldNum(fieldNum), mode(mode)
{}

SendOptionNode::SendOptionNode(Errpos ep, Node rhs, Node port,
                               int fieldNum, string bufferName)
      : BaseNode(ep, SEND_OPTION_NODE), rhs(rhs), port(port),
      bufferName(bufferName), fieldNum(fieldNum)
{}

ReceiveNode::ReceiveNode(Errpos ep, Node lhs, Node port, Node type,
                         string bufferName, bool signal,
                         int fieldNum, FileMode mode)
      : BaseNode(ep, RECEIVE_NODE), lhs(lhs), port(port), type(type),
      bufferName(bufferName), signal(signal), fieldNum(fieldNum), mode(mode)
{}

ReceiveOptionNode::ReceiveOptionNode(Errpos ep, Node lhs, Node port, Node type,
                                     int fieldNum, string bufferName,
                                     bool signal)
      : BaseNode(ep, RECEIVE_OPTION_NODE), lhs(lhs), port(port), type(type),
      fieldNum(fieldNum), bufferName(bufferName), signal(signal)
{}

ThreadNode::ThreadNode(Errpos ep, Node port, List inputs, List outputs, Node seq)
      : BaseNode(ep, THREAD_NODE), name(""), port(port), inputs(inputs), outputs(outputs),
      seq(seq), start(0), numFields(0)
{}

ThreadParamNode::ThreadParamNode(Errpos ep, Node name, PortKind portDir, Node type, Node port, bool input)
      : BaseNode(ep, THREAD_PARAM_NODE), name(name), portDir(portDir), type(type), port(port), input(input),
      owner(""), transfer(0)
{}

StartNode::StartNode(Errpos ep, List calls, Node seq)
      : BaseNode(ep, START_NODE), calls(calls), seq(seq)
{}

ThreadCallNode::ThreadCallNode(Errpos ep, Node name, List inputs, List outputs)
      : BaseNode(ep, THREAD_CALL_NODE), name(name),
      inputs(inputs), outputs(outputs), numFields(0),
      inputTransfer(-1), outputTransfer(-1),
      startTransfer(-1), stopTransfer(-1)
{}

ThreadStartNode::ThreadStartNode(Errpos ep, Node name, Node chName, int numFields)
: BaseNode(ep, THREAD_START_NODE), name(name), chName(chName), numFields(numFields)
{}

ThreadStopNode::ThreadStopNode(Errpos ep, Node name, Node chName)
: BaseNode(ep, THREAD_STOP_NODE), name(name), chName(chName)
{}
