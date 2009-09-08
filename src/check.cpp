/** \file check.cpp
 * These functions implement the semantic checking phase of the compiler.
 */

#include "ast.h"

#include <cassert>
#include <map>
using namespace std;

/** Check whether a type is some kind of integer. */
bool isIntType(Node type)
{
   return
      type == BaseNode::theByteNode ||
      type == BaseNode::theUnsignedByteNode ||
      type == BaseNode::theIntegerNode ||
      type == BaseNode::theUnsignedIntegerNode;
}

/** Merge two sets of nodes. */
set<Node> merge(const set<Node> & a, const set<Node> & b)
{
   set<Node> u;
   for (set<Node>::const_iterator i = a.begin(); i != a.end(); ++i)
      u.insert(*i);
   for (set<Node>::const_iterator i = b.begin(); i != b.end(); ++i)
      u.insert(*i);
   return u;
}

/** Report an error if a name has been defined previously.
  * Otherwise, enter it into the current names list.
  */
void checkName(CheckData & cd, string name, Errpos ep)
{
   if (cd.varNames.find(name) != cd.varNames.end())
      Error() << "Name '" << name << "' has already been defined in this scope." << ep << REPORT;
   cd.varNames.insert(name);
}

/** Dummy function for checking type equality. */
bool sameType(Node lhs, Node rhs)
{

   Node lhsType = lhs->kind() == NAME_NODE ? lhs->getValue() : lhs;
   Node rhsType = rhs->kind() == NAME_NODE ? rhs->getValue() : rhs;

   if (  lhsType->isMapType() &&                 // lhsType->kind() == MAP_TYPE_NODE &&
         rhsType->isMapType() &&                 //rhsType->kind() == MAP_TYPE_NODE &&
         sameType(lhsType->getDomainType(), rhsType->getDomainType()) &&
         sameType(lhsType->getRangeType(), rhsType->getRangeType()) )
      return true;

   if (  lhsType->isArrayType() &&
         rhsType->isArrayType() )
      return true;

   return lhsType == rhsType;
}

FuncDef *getConversion(Node fromType, Node toType)
{

//   fromType->showDebug("conversion from"); // 090817
//   toType->showDebug("to"); // 090817

   fromType = fromType->kind() == NAME_NODE ? fromType->getValue() : fromType;
   toType = toType->kind() == NAME_NODE ? toType->getValue() : toType;

//   fromType->showDebug("conversion from");
//   toType->showDebug("to");

   // No conversion needed for equal types.
   if (sameType(fromType, toType))
      return funIdentity;

   // Enumeration can be converted to Integer.
   if (  fromType->isEnumType() &&
         (toType == BaseNode::theIntegerNode || toType == BaseNode::theUnsignedIntegerNode))
      return funIdentity;

   // Enumeration can be converted to Text.
   if (fromType->isEnumType() && toType == BaseNode::theTextNode)
      return funEnumText;

   // No conversion allowed between distinct enumeration types.
   if (fromType->isEnumType() && toType->isEnumType())
      return funBottom;

   // Look for a conversion function.
   for (FuncIter it = funcDefs.begin(); it != funcDefs.end(); ++it)
   {
      if (  (*it)->isConversion() &&
            sameType((*it)->paramType(), fromType) &&
            sameType((*it)->resultType(), toType) )
         return *it;
   }

   // No conversion possible.
   return funBottom;
}

/** A Text Type is either Char or Text. */
bool isTextType(Node type)
{
   return
      type == BaseNode::theCharNode ||
      type == BaseNode::theTextNode;
}

/** A numeric type is an enumeration, a Byte or an Integer. */
bool isNumericType(Node type)
{
   return
      type->kind() == ENUM_TYPE_NODE ||
      type == BaseNode::theByteNode ||
      type == BaseNode::theUnsignedByteNode ||
      type == BaseNode::theIntegerNode ||
      type == BaseNode::theUnsignedIntegerNode ||
      type == BaseNode::theFloatNode ||
      type == BaseNode::theDecimalNode;
}

/** Return the smallest type that includes the two given types.
 *  The result is sometimes inaccurate: e.g., unsigned Integer
 *  and Integer give Integer. */
Node joinTypes(Node lhs, Node rhs)
{
   if (lhs == BaseNode::theBoolNode && rhs == BaseNode::theBoolNode)
      return BaseNode::theBoolNode;
   else if (isTextType(lhs) && isTextType(rhs))
   {
      if (lhs == BaseNode::theCharNode && rhs == BaseNode::theCharNode)
         return BaseNode::theCharNode;
      else
         return BaseNode::theTextNode;
   }
   else if (isNumericType(lhs) && isNumericType(rhs))
   {
      if (lhs == BaseNode::theFloatNode || rhs == BaseNode::theFloatNode)
         return BaseNode::theFloatNode;
      if (lhs == BaseNode::theDecimalNode || rhs == BaseNode::theDecimalNode)
         return BaseNode::theDecimalNode;
      else
         return BaseNode::theIntegerNode;
   }
   else
      return BaseNode::theVoidNode;
}

/** Coerce 'expr' from type 'fromType' to type 'toType'. */
void coerce(Node fromType, Node toType, Node & expr)
{

//   expr->showDebug("coerce"); // 090817
//   fromType->showDebug("from"); // 090817
//   toType->showDebug("to"); // 090817

   if (expr->kind() == NUM_NODE && expr->getType() == BaseNode::theIntegerNode)
   {
      int val = expr->getIntVal();
      if (toType == BaseNode::theUnsignedIntegerNode && val < 0)
      {
         Error() << "Literal cannot be converted to unsigned Integer." << expr->getPos() << REPORT;
         return;
      }
      else if (toType == BaseNode::theByteNode && (val < -128 || val > 127))
      {
         Error() << "Literal cannot be converted to Byte." << expr->getPos() << REPORT;
         return;
      }
      else if (toType == BaseNode::theUnsignedByteNode && (val < 0 || val > 255))
      {
         Error() << "Literal cannot be converted to Unsigned Byte." << expr->getPos() << REPORT;
         return;
      }
   }

   if (expr->kind() == CHAR_NODE && toType == BaseNode::theTextNode)
   {
      expr = new TextNode(expr->getPos(), expr->getConstValue());
      return;
   }


   // Integer can be converted to Enumeration.
   if (isIntType(fromType) && toType->isEnumType())
   {
      List args;
      args.push_back(toType);
      args.push_back(expr);
      expr = new FunctionNode(expr->getPos(), funIntegerEnum, args);
      return;
   }

   FuncDef *fun = getConversion(fromType, toType);
   if (fun == funIdentity)
      return;
   if (fun == funBottom)
   {
      Error() << "Expression cannot be coerced to required type." << expr->getPos() << REPORT;
      return;
   }
   List args;
   args.push_back(expr);
   expr = new FunctionNode(expr->getPos(), fun, args);
}

/** Check that a sequence conforms to a protocol. */
void checkProtocol(Node portDec, Node closure, Node seq, PortKind pk, bool warnings)
{
   // Construct the LTS for the sequence.
   clearNextState();
   LTS *seqLTS = seq->processGraph(portDec, -1);
   seqLTS->collapse();

   // Get the protocol and LTS for the port.
   Node prot = portDec->getProtocol();
   LTS *protLTS = prot->getLTS();

   switch (pk)
   {
      case CLIENT:
         if (warnings &&  ! satisfies(protLTS, seqLTS))
         {
            cerr << "Warning: protocol '" << portDec->getNameString() <<
            "' does not satisfy client '" << closure->getNameString() <<
            "'.\nProtocol: " << protLTS << "\nCode:     " << seqLTS << "\n";
            cerr << endl;
         }
         break;
      case SERVER:
         if (warnings &&  ! satisfies(seqLTS, protLTS))
         {
            cerr << "Warning: server '" << closure->getNameString() <<
            "' does not satisfy protocol '" << portDec->getNameString() <<
            "'.\nCode:     " << seqLTS << "\nProtocol: " << protLTS << "\n";
            cerr << endl;
         }
         break;
      default:
         cerr << "Warning: port should indicate client or server." << portDec->getPos();
         break;
   }
}

/** Structure used to check port usage in a CellNode. */
struct Entry
{
   Entry(bool param, PortKind portDir, Errpos ep)
         : param(param), portDir(portDir), ep(ep), valid(true), numServers(0), numClients(0)
   {}

   Entry() : param(false), portDir(NO_ROLE), valid(false), numServers(0), numClients(0)
   {}

   /** True if this port is a parameter of the instance. */
   bool param;

   /** True if this entry is really a port. */
   bool valid;

   /** Declared direction of port. */
   PortKind portDir;

   /** Position in source text corresponding to this entry. */
   Errpos ep;

   /** Number of 'provides' uses. */
   int numServers;

   /** Number of 'needs' uses. */
   int numClients;
};

typedef map<string, Entry> PortTable;

/** Add an entry to a list of port entries. */
void addPort(PortTable & ports, Node p, bool param)
{
   Errpos ep = p->getPos();
   switch (p->kind())
   {
      case DEC_NODE:
         if (p->isPort())
         {
            string name = p->getNameString();
            PortKind portDir = p->getPortKind();
            ports.insert(make_pair(name, Entry(param, portDir, ep)));
         }
         break;

      case INSTANCE_NODE:
         {
            List args = p->getArgList();
            Node name = p->getName();
            Node def = name->getDefinition();
            Node val = def->getValue();
            List params = val->getParamList();
            ListIter ita = args.begin();
            ListIter itp = params.begin();
            while (ita != args.end() && itp != params.end())
            {
               if ((*ita)->kind() == NAME_NODE)
               {
                  string name = (*ita)->getNameString();
                  if ((*itp)->kind() == DEC_NODE && (*itp)->isPort())
                  {
                     PortKind portDir = (*itp)->getPortKind();
                     switch (portDir)
                     {
                        case SERVER:
                           ++(ports[name].numServers);
                           break;
                        case CLIENT:
                           ++(ports[name].numClients);
                           break;
                     }
                  }
                  else
                     ports[name].valid = false;
               }
               ++ita;
               ++itp;
            }
         }
         break;

      default:
         Error() << "Internal error: strange declaration in InstanceNode.\n" << REPORT;
         break;
   }
}

/** Match one argument to the corresponding parameter. */
void matchArg(Node & arg, Node par)
{
   Node argType = arg->getType();
   Node parType = par->getType();
   if (argType->isPort() && parType->isPort())
   {
      PortKind argPK = arg->getPortKind();
      Node argProt = argType->getProtocol();

      PortKind parPK = par->getPortKind();
      Node parProt = parType->getProtocol();

      bool protsat = true;
      if (parPK == SERVER && ! satisfies(parProt->getLTS(), argProt->getLTS()))
      {
         Error() << "Parameter protocol" << par->getPos() << "does not satisfy argument protocol"  << arg->getPos() << REPORT;
         protsat = false;
      }
      else if (parPK == CLIENT  && ! satisfies(argProt->getLTS(), parProt->getLTS()))
      {
         Error() << "Argument protocol" << arg->getPos() << "does not satisfy parameter protocol" << par->getPos() << REPORT;
         protsat = false;
      }
      if (protsat)
      {
         set<Node> fields = merge(argProt->getFieldDecs(), parProt->getFieldDecs());
         for (set<Node>::iterator i = fields.begin(); i != fields.end(); ++i)
            for (set<Node>::iterator j = i; j != fields.end(); ++j)
               if (i != j && (*i)->getNameString() == (*j)->getNameString())
               {
//                  cerr << "Tieing " << (*i)->getNameString() << " and " << (*j)->getNameString() << endl; // 090207
                  Node t = (*i)->getTie();
                  (*i)->setTie((*j)->getTie());
                  (*j)->setTie(t);
               }
      }
   }
   else if (argType->isPort())
      Error() << "Argument is a port ..." << arg->getPos() << "... but parameter is not." << par->getPos() << REPORT;
   else if (parType->isPort())
      Error() << "Parameter is a port ..." << par->getPos() << "... but argument is not." << arg->getPos() << REPORT;
   else if (par->isAlias())
   {
      if ( ! sameType(argType, parType))
         Error() << "Argument ..." << arg->getPos() << "... does not match parameter." << par->getPos() << REPORT;
      else if (arg->kind() == NAME_NODE)
      {
         if (arg->isConstant())             // 090228
            Error() << "A constant ..." << arg->getPos() << "... cannot be passed as an alias" << par->getPos() << REPORT;
         if ( ! arg->isAlias())             // 081206
            arg->setPassByReference();
      }
      else
         Error() << "Argument must be a name ..." << arg->getPos() << "... to match aliased parameter" << par->getPos() << REPORT;
   }
   else
      coerce(argType, parType, arg);
}

//   {
//      FuncDef *fun = getConversion(argType, parType);
//      if (fun == funBottom)
//         Error() << "Argument ..." << arg->getPos() << "... does not match parameter." << par->getPos() << REPORT;
//      else
//         coerce(fun, arg);
//   }


/** Match arguments and parameters in an instance node. */
void matchArgList(List & args, List params, Errpos ep)
{
   List::iterator ita = args.begin();
   List::iterator itp = params.begin();
   if (args.size() < params.size())
      Error() << "Too few arguments." << ep << REPORT;
   else if (args.size() > params.size())
      Error() << "Too many arguments." << (*ita)->getPos() << REPORT;
   else
      while (ita != args.end())
         matchArg(*ita++, *itp++);
}

/** Constructor for the CheckData object that is passed around the
 *  abstract syntax tree during semantic checking.
 */
CheckData::CheckData(bool showWarnings)
      : showWarnings(showWarnings), type(BaseNode::BaseNode::theVoidNode),
      withinLoop(false), withinProtocol(false), withinParamList(false),
      withinGuard(false)
{}

/** "Infinity" for conversion count. */
const int MAX_CONV = 100000;

/** Return the the conversions needed for a function call. */
int countConversions(List params, List args)
{
   assert(params.size() == args.size());
   int nc = 0;
   ListIter ip = params.begin();
   ListIter ia = args.begin();
   while (ip != params.end())
   {
      FuncDef *conv = getConversion((*ia)->getType(), *ip);

//      090701
//      if (*ia) (*ia)->showDebug("arg");
//      if (*ip) (*ip)->showDebug("par");
//      if (conv)
//         cerr << conv;
//      else
//         cerr << "No function\n";

      if (conv == funBottom)
         return MAX_CONV;
      else if (conv != funIdentity)
         ++nc;
      ++ip;
      ++ia;
   }
   return nc;
}

/** Incorporate conversions into argumentst. */
void doConversions(List params, List & args)
{
   assert(params.size() == args.size());
   ListIter ip = params.begin();
   List::iterator ia = args.begin();
   while (ip != params.end())
   {
//      FuncDef *conv = getConversion((*ia)->getType(), *ip);
//      assert(conv);
//      coerce(conv, *ia);
//(*ia)->getType()->showDebug("ia");
//(*ip)->showDebug("ip");
      coerce((*ia)->getType(), *ip, *ia);
      ++ip;
      ++ia;
   }
}

/** Semantic checking: default is to do nothing.
  * \param cd contains data for checking
  */
void BaseNode::check(CheckData & cd)
{}

void ProgramNode::check(CheckData & cd)
{
   for (ListIter it = nodes.begin(); it != nodes.end(); ++it)
      (*it)->check(cd);
}

void DefNode::check(CheckData & cd)
{
   name->check(cd);
   value->check(cd);
   if (incremental)
      cerr << "Warning: '+=' is not implemented yet; '=' assumed." << ep;
}

void InstanceNode::check(CheckData & cd)
{
   name->check(cd);
   for (ListIter it = args.begin(); it != args.end(); ++it)
      (*it)->check(cd);

   if (topLevel && args.size() > 0)
      Error() << "A top level instance cannot have arguments." << (*args.begin())->getPos() << REPORT;

   if (name->kind() == NAME_NODE)
   {
      Node def = name->getDefinition();
      Node val = def->getValue();
      if ( val->kind() == PROCESS_NODE ||
            val->kind() == CELL_NODE ||
            val->kind() == PROCEDURE_NODE ||
            val->kind() == CPP_NODE )
         matchArgList(args, val->getParamList(), ep);
      else
         Error() << "Invocation does not match declaration." << name->getPos() << REPORT;
   }
   else
      Error() << "Simple name required here." << name->getPos() << REPORT;
}

void CppNode::check(CheckData & cd)
{
   StringSet outer = cd.varNames;
   cd.varNames = StringSet();
   cd.withinParamList = true;
   for (ListIter it = params.begin(); it != params.end(); ++it)
      (*it)->check(cd);
   cd.withinParamList = false;
   for (ListIter it = params.begin(); it != params.end(); ++it)
   {
      if ((*it)->kind() == DEC_NODE)
      {
         (*it)->setParameter();
         if ( (*it)->isPort())
            Error() << "A port cannot be passed to a C++ function." << (*it)->getPos() << REPORT;
         if ((*it)->isAlias())
            (*it)->setReference();
      }
   }
   cd.varNames = outer;
   if (type)
      type->check(cd);
}

void ProcedureNode::check(CheckData & cd)
{
   StringSet outer = cd.varNames;
   cd.varNames = StringSet();
   cd.withinParamList = true;
   for (ListIter it = params.begin(); it != params.end(); ++it)
      (*it)->check(cd);
   cd.withinParamList = false;
   seq->check(cd);
   for (ListIter it = params.begin(); it != params.end(); ++it)
   {
      if ((*it)->kind() == DEC_NODE)
      {
         (*it)->setParameter();
         if ( (*it)->isPort())
            Error() << "Procedures cannot have ports as parameters." << (*it)->getPos() << REPORT;
         if ((*it)->isAlias())
            (*it)->setReference();
      }
   }
   cd.varNames = outer;
}

void ProcessNode::check(CheckData & cd)
{
   StringSet outer = cd.varNames;
   cd.varNames = StringSet();
   cd.withinParamList = true;
   for (ListIter it = params.begin(); it != params.end(); ++it)
      (*it)->check(cd);
   cd.withinParamList = false;
   seq->check(cd);
   for (ListIter it = params.begin(); it != params.end(); ++it)
   {
      if ((*it)->kind() == DEC_NODE)
      {
         (*it)->setParameter();
         if ( (*it)->isPort())
            checkProtocol((*it), this, seq, (*it)->getPortKind(), cd.showWarnings);
         if ((*it)->isAlias())
            (*it)->setReference();
      }
   }
   cd.varNames = outer;
}

void CellNode::check(CheckData & cd)
{
   StringSet outer = cd.varNames;
   cd.varNames = StringSet();
   cd.withinParamList = true;
   for (ListIter it = params.begin(); it != params.end(); ++it)
      (*it)->check(cd);
   for (ListIter it = params.begin(); it != params.end(); ++it)
      if ((*it)->kind() == DEC_NODE)
         (*it)->setReference();
   cd.withinParamList = false;

   for (ListIter it = instances.begin(); it != instances.end(); ++it)
      (*it)->check(cd);

   PortTable ports;
   for (ListIter it = params.begin(); it != params.end(); ++it)
      addPort(ports, *it, true);
   for (ListIter it = instances.begin(); it != instances.end(); ++it)
      addPort(ports, *it, false);
   for (PortTable::const_iterator it = ports.begin(); it != ports.end(); ++it)
   {
      Entry ent = it->second;
      if (ent.valid)
      {
         if (ent.param)
            {}
         else
         {
            if ( ! (ent.portDir == NO_ROLE || ent.portDir == CHANNEL) )
               Error() << "Port variable must not have a direction." << ent.ep << REPORT;
            if (ent.numServers != 1)
               Error() << "A channel must be connected to exactly one server." << ent.ep << REPORT;
            if (ent.numClients != 1)
               Error() << "A channel must be connected to exactly one client." << ent.ep << REPORT;
         }
      }
   }
   cd.varNames = outer;
}

void ProtocolNode::check(CheckData & cd)
{
   cd.withinProtocol = true;

   cd.protNames = StringSet();
   if (expr)
      expr->check(cd);

   // Build LTS
   clearNextState();
   plts = expr->protocolGraph(fieldDecs);
   plts->collapse();

   cd.withinProtocol = false;
}

void SequenceNode::check(CheckData & cd)
{
   StringSet outer = cd.varNames;
   cd.varNames = StringSet();
   for (ListIter it = stmts.begin(); it != stmts.end(); ++it)
      (*it)->check(cd);
   cd.type = BaseNode::theVoidNode;
   cd.varNames = outer;
}

void ExitNode::check(CheckData & cd)
{
   if (!cd.withinLoop)
      Error() << "'exit' can be used only within the body of a loop." << ep << REPORT;
}

void IfNode::check(CheckData & cd)
{
   for (ListIter it = condPairs.begin(); it != condPairs.end(); ++it)
      (*it)->check(cd);
   alt->check(cd);
   cd.type = BaseNode::theVoidNode;
}

void CondPairNode::check(CheckData & cd)
{
   for (ListIter it = decs.begin(); it != decs.end(); ++it) // 090227
      (*it)->check(cd);
   cond->check(cd);
//   if (cond->kind() == DOT_NODE)
//      Error() << "Conditions are not allowed to communicate." << cond->getPos() << REPORT;
   if (!cd.type || cd.type != theBoolNode)
      Error() << "Boolean expression required after 'if' or 'elif'." << cond->getPos() << REPORT;
   seq->check(cd);
}

void LoopNode::check(CheckData & cd)
{
   bool oldWithin = cd.withinLoop;
   cd.withinLoop = true;
   seq->check(cd);
   cd.withinLoop = oldWithin;
   cd.type = BaseNode::theVoidNode;
}

void ForNode::check(CheckData & cd)
{
   StringSet outer = cd.varNames;
   cd.varNames = StringSet();
   comp->check(cd);
   seq->check(cd);
   cd.varNames = outer;
}

void AnyNode::check(CheckData & cd)
{
   StringSet outer = cd.varNames;
   comp->check(cd);
   seq->check(cd);
   if (!alt->isEmpty())
      alt->check(cd);
   cd.varNames = outer;
}

void ComprehensionNode::check(CheckData & cd)
{
   var->check(cd);
   if (type)
      type->check(cd);
   else
      type = theIntegerNode;

   // If the programmer has not declared the type of the loop variable,
   // we infer it while checking the collection and store it in the
   // loop variable declaration.
   cd.indexType = theVoidNode;
   collection->check(cd);
   if (cd.indexType != theVoidNode)
   {
      type = cd.indexType;
      var->setType(type);
   }
   iterType = cd.iterType;

   if (pred)
      pred->check(cd);
}

void EnumSetNode::check(CheckData & cd)
{
   cd.iterType = 0;
   if (type->isEnumType())
      max = type->getEnumSize();
   else
      Error() << "Enumeration type expected" << type->getPos() << REPORT;
}

void RangeNode::check(CheckData & cd)
{
   cd.iterType = 0;
   Node varType = cd.type;

   start->check(cd);
   Node startType = cd.type;

   if (varType == theVoidNode)
   {
      // Loop variable type was not declared: use start type.
      varType = startType;
      cd.indexType = startType;
   }
   else
      coerce(startType, varType, start);
//   {
//      FuncDef *fun = getConversion(startType, varType);
//      if (fun == funBottom)
//         Error() << "Initial value has wrong type." << start->getPos() << REPORT;
//      else
//         coerce(fun, start);
//   }

   finish->check(cd);
   coerce(cd.type, varType, finish);

//   FuncDef *fun = getConversion(cd.type, varType);
//   if (fun == funBottom)
//      Error() << "List value has wrong type." << finish->getPos() << REPORT;
//   else
//      coerce(fun, finish);

   if (step)
   {
      step->check(cd);
      coerce(cd.type, varType, step);
   }
}

//      FuncDef *fun = getConversion(cd.type, varType);
//      if (fun == funBottom)
//         Error() << "Step value has wrong type." << step->getPos() << REPORT;
//      else
//         coerce(fun, step);

void MapSetNode::check(CheckData & cd)
{
   if (setKind == MAPSET_PAIR)
      // This should not occur because the parser does not create this code.
      Error() << "Pair matching not implemented yet." << ep << REPORT;

   type = cd.type;
   map->check(cd);
   Node mapType = cd.type;
   cd.iterType = mapType;

   if (mapType->isMapType())
      mapKind = INDEXED;
   else if (mapType->isArrayType())
      mapKind = ARRAY;
   else if (mapType == theTextNode)
      mapKind = TEXT;
   else
      Error() << "Map expression has incorrect type." << map->getPos() << REPORT;

//   mapType->showDebug("map type");
//   type->showDebug("type");
//   mapType->getRangeType()->showDebug("range type");

   if (map->kind() == FUNC_NODE)
      Error() << "Function calls in this context are not implemented yet." << map->getPos() << REPORT;

   switch (setKind)
   {
      case MAPSET_DOMAIN:
         {
            Node domainType = mapType->getDomainType();
            if (type == theVoidNode)
            {
               type = domainType;
               cd.indexType = domainType;
            }
            else if (!sameType(type, domainType))
               Error() << "The type of the loop variable is not the domain type." << ep << REPORT;
         }
         break;

      case MAPSET_RANGE:
         {
            Node rangeType = mapType->getRangeType();
            if (type == theVoidNode)
            {
               type = rangeType;
               cd.indexType = rangeType;
            }
            else if (!sameType(type, mapType->getRangeType()))
               Error() << "The type of the loop variable is not the range type." << ep << REPORT;
         }
         break;
   }
}

void SelectNode::check(CheckData & cd)
{
   if (options.size() == 0)
      Error() << "Select statement must have at least one branch." << ep << REPORT;
   for (ListIter it = options.begin(); it != options.end(); ++it)
      (*it)->check(cd);
   cd.type = BaseNode::theVoidNode;
}

void OptionNode::check(CheckData & cd)
{
   if (guard)
   {
      cd.withinGuard = true;
      guard->check(cd);
      cd.withinGuard = false;
   }
   if (seq->isEmpty())
      Error() << "Select option must have at least one statement." << ep << REPORT;
   else
      seq->check(cd);
}

void DotNode::check(CheckData & cd)
{
   if (cd.withinGuard)
      Error() << "A guard must not communicate." << ep << REPORT;
   Node type = port->getType();
   if (!type->isPort())
      Error() << "Port name required for communication." << ep << REPORT;
   field->check(cd);
}

void QueryNode::check(CheckData & cd)
{
   if (cd.withinGuard)
      Error() << "A guard must not communicate." << ep << REPORT;
   Node type = port->getType();
   if (!type->isPort())
      Error() << "Port name required for communication." << ep << REPORT;
   field->check(cd);
   cd.type = BaseNode::theBoolNode;
}

void BoolNode::check(CheckData & cd)
{
   cd.type = BaseNode::theBoolNode;
}

void CharNode::check(CheckData & cd)
{
   cd.type = BaseNode::theCharNode;
}

void TextNode::check(CheckData & cd)
{
   cd.type = BaseNode::theTextNode;
}

void NumNode::check(CheckData & cd)
{
   if (  type == BaseNode::theByteNode ||
         type == BaseNode::theUnsignedByteNode ||
         type == BaseNode::theIntegerNode ||
         type == BaseNode::theUnsignedIntegerNode ||
         type == BaseNode::theFloatNode ||
         type == BaseNode::theDecimalNode )
      cd.type = type;
   else
      emergencyStop("numeric node has wrong type.", ep);
}

void ListopNode::check(CheckData & cd)
{
   for (ListIter it = args.begin(); it != args.end(); ++it)
      (*it)->check(cd);
}

void BinopNode::check(CheckData & cd)
{
   lhs->check(cd);
   Node lt = cd.type;
   rhs->check(cd);
   Node rt = cd.type;
   if (cd.withinProtocol)
      return;

   lt = lt->kind() == NAME_NODE ? lt->getValue() : lt;
   rt = rt->kind() == NAME_NODE ? rt->getValue() : rt;

//   lhs->showDebug("LHS"); // 090817
//   lt->showDebug("type"); // 090817

   if (op == BINOP_EXTEND)
   {
      if (!lt->isArrayType())
         Error() << "Operand should be an array." << lhs->getPos() << REPORT;
      coerce(rt, lt->getRangeType(), rhs);
      type = lt;
      cd.type = type;
      return;
   }

   if (isBoolOp(op))
   {
      if (lt != theBoolNode)
      {
         Error() << "Operand should be Bool." << lhs->getPos() << REPORT;
         return;
      }
      if (rt != theBoolNode)
      {
         Error() << "Operand should be Bool." << rhs->getPos() << REPORT;
         return;
      }
      type = theBoolNode;
      cd.type = type;
      return;
   }

   if (isEqualOp(op))
   {
      Node join = joinTypes(lt, rt);
      if (join == theVoidNode)
         Error() << "Expressions are incomparable." << ep << REPORT;
      else
      {
         coerce(lt, join, lhs);
         coerce(rt, join, rhs);
      }
      type = theBoolNode;
      cd.type = type;
      return;
   }

   if (isCompOp(op))
   {
      Node join = joinTypes(lt, rt);
      if (join == theBoolNode || join == theVoidNode)
         Error() << "Expressions cannot be ordered." << ep << REPORT;
      else
      {
         coerce(lt, join, lhs);
         coerce(rt, join, rhs);
      }
      type = theBoolNode;
      cd.type = type;
      return;
   }

   if (isNumericOp(op))
   {
      // Allow '+' for text concatenation
      if (op != BINOP_PLUS && (isTextType(lt) || isTextType(rt)))
      {
         Error() << "Operands have incorrect types." << ep << REPORT;
         type = theVoidNode;
         cd.type = type;
         return;
      }
      Node join = joinTypes(lt, rt);
      if (join == theVoidNode)
         Error() << "Operands have incorrect types." << ep << REPORT;
      else
      {
         coerce(lt, join, lhs);
         coerce(rt, join, rhs);
      }
      type = join;
      cd.type = type;
      return;
   }

   if (op == BINOP_CAT)
   {
//      lt->showDebug("lt");
//      rt->showDebug("rt");

      coerce(lt, theTextNode, lhs);
      coerce(rt, theTextNode, rhs);
      type = theTextNode;
      cd.type = type;
      return;
   }

   Error() << "Operands have incorrect types." << ep << REPORT;
   type = theVoidNode;
   cd.type = type;
}

void CondExprNode::check(CheckData & cd)
{
   lhs->check(cd);
   Node lhsType = cd.type;
   pred->check(cd);
   Node predType = cd.type;
   rhs->check(cd);
   Node rhsType = cd.type;

   if (predType != theBoolNode)
      Error() << "Predicate type must be Bool." << pred->getPos() << REPORT;

   // \todo: generalize the following to allow for coercions.
   if (sameType(lhsType, rhsType))
   {
      type = lhsType;
      cd.type = type;
   }
   else
      Error() << "Operands of if-else expression must have the same type." << rhs->getPos() << REPORT;
}

void UnopNode::check(CheckData & cd)
{
   operand->check(cd);
   if (cd.withinProtocol)
      return;
   type = cd.type;

   if (op == UNOP_NOT && !cd.type->isBoolType())
      Error() << "'not' requires a Boolean operand." << ep << REPORT;

   if (op == UNOP_MINUS && !cd.type->isNumericType())
      Error() << "'-' requires a numeric operand." << ep << REPORT;

   if (op == UNOP_EXECUTE)
   {
      if ( ! (type->isArrayType() && (type->getRangeType() == theUnsignedByteNode)))
         Error() << "'execute' requires an array of unsigned Bytes as its operand." << ep << REPORT;
      type = theIntegerNode;
      cd.type = type;
   }
}

void SubscriptNode::check(CheckData & cd)
{
   sub->check(cd);
   base->check(cd);

   if (cd.type == BaseNode::theTextNode)
   {
      if ( ! isIntType(sub->getType()))
         Error() << "Subscript must be an integer expression." << sub->getPos() << REPORT;
      cd.type = theCharNode;
   }
   else if (cd.type->isArrayType())
   {
      if ( ! isIntType(sub->getType()))
         Error() << "Subscript must be an integer expression." << sub->getPos() << REPORT;
      cd.type = cd.type->getRangeType();
   }
   else if (cd.type->isMap())
   {
      if ( ! sameType(sub->getType(), cd.type->getDomainType()) )
         Error() << "Subscript has wrong type." << sub->getPos() << REPORT;
      cd.type = cd.type->getRangeType();
   }
   else
      Error() << "Variable cannot be subscripted." << base->getPos() << REPORT;
}

void SubrangeNode::check(CheckData & cd)
{
   sub1->check(cd);
   Node sub1Type = cd.type;
   sub2->check(cd);
   Node sub2Type = cd.type;
   base->check(cd);
   if (lvalue)
      Error() << "Assignment to characters in a text is not implemented." << ep << REPORT;

   if (cd.type == BaseNode::theTextNode)
   {
      if ( ! isIntType(sub1->getType()))
         Error() << "Subscript must be an integer expression." << sub1->getPos() << REPORT;
      if ( ! isIntType(sub2->getType()))
         Error() << "Subscript must be an integer expression." << sub2->getPos() << REPORT;
   }
   else if (cd.type->isMap())
      Error() << "Subrange not allowed for indexed types." << sub2->getPos() << REPORT;
}

void IteratorNode::check(CheckData & cd)
{
   map->check(cd);
   switch (fun)
   {
      case ITER_START:
         cd.type = theVoidNode;
         break;
      case ITER_FINISH:
         cd.type = theBoolNode;
         break;
      case ITER_KEY:
         cd.type = map->getDomainType();
         break;
      case ITER_VALUE:
         cd.type = map->getRangeType();
         break;
      case ITER_STEP:
         cd.type = theVoidNode;
         break;
   }
}

void ConstantNode::check(CheckData & cd)
{
   name->check(cd);
   type->check(cd);
   Node declaredType = cd.type;
   value->check(cd);
   Node valueType = cd.type;
   coerce(valueType, declaredType, value);
}

void DecNode::check(CheckData & cd)
{
   if (cd.withinProtocol)
   {
      if (type)
      {
         if (type->isMap())
            Error() << "Transfer of maps is not implemented yet." << type->getPos() << REPORT;
         if (type->isArrayType())
            Error() << "Transfer of arrays is not implemented yet." << type->getPos() << REPORT;
      }
      if (value)
         Error() << "Default message values are not allowed." << value->getPos() << REPORT;
      return;
   }

   Node varType = 0;
   if (generated)
   {
      // This node was generated by 'extract' and has no type.
      // We determine the type from the initial value, which must be present.
      assert(!type);
      assert(value);
      value->check(cd);
      type = cd.type;
      varType = cd.type;
      name->check(cd);
   }
   else
   {
      name->check(cd);
      if (type)
      {
         name->setType(type);
         if (type->isPort()  && ! type->isMap())
            cd.type = type->getValue();
         else
            type->check(cd);
      }
      varType = cd.type;
   }

//   showDebug("var type"); // 090817
//   showDebug("Stmt"); // 090817
//   varType->showDebug("var type"); // 090817

   if ( ! varType)
      showDebug("Something wrong: DecNode::check varType == 0");

   if (name->kind() == SUBSCRIPT_NODE && type)
      Error() << "A subscripted variable cannot be declared." << name->getPos() << REPORT;

   // Check message direction for LHS
   if (name->kind() == DOT_NODE && value)
   {
      // sending
      PortKind pk = name->getPortKind();
      MessageKind mk = name->getMessKind();

      //name->showDebug("pk=" + portToString(pk) + "  mk=" + messToString(mk));

      switch (mk)
      {
         case REPLY:
            if (pk == CLIENT)
               Error() << "A client cannot send a reply." << name->getPos() << REPORT;
            break;

         case QUERY:
            if (pk == SERVER)
               Error() << "A server cannot send a query." << name->getPos() << REPORT;
            break;

         default:
            Error() << "Field must be a reply or a query." << name->getPos() << REPORT;
            break;
      }
   }

   // Check message direction for RHS
   if (value && value->kind() == DOT_NODE)
   {
      // receiving
      PortKind pk = value->getPortKind();
      MessageKind mk = value->getMessKind();
      if (pk == CLIENT && mk == QUERY)
         Error() << "A client cannot receive a query." << value->getPos() << REPORT;
      else if (pk == SERVER && mk == REPLY)
         Error() << "A server cannot receive a reply." << value->getPos() << REPORT;
   }

   // Check signals
   if ( ! (value || type) )
   {
      if (name->kind() != DOT_NODE)
         Error() << "Name used without context." << ep << REPORT;
      MessageKind mk = name->getMessKind();
      PortKind pk = name->getPortKind();
      if (pk == CLIENT && mk == QUERY || pk == SERVER && mk == REPLY)
         ck = SEND;
      else if (pk == CLIENT && mk == REPLY || pk == SERVER && mk == QUERY)
         ck = RECEIVE;
      else
         Error() << "Signal direction error." << ep << REPORT;
   }

   // Check that we are not sending a file read result.
   if (name->kind() == DOT_NODE && value && value->getFun() == "file_read")
      Error() << "Sending from file_read is not implemented yet." << value->getPos() << REPORT;

   // Check assignments and initializations with RHS
   if (value)
   {
      if ( ! type && name->isConstant())
         Error() << "Assignment to a constant is not allowed." << name->getPos() << REPORT;
      if (cd.withinParamList)
         Error() << "Parameters cannot have default values." << value->getPos() << REPORT;
      if ( ! name->assignable())
         Error() << "Assignment to this variable is not allowed." << name->getPos() << REPORT;
      if (move && value->kind() != NAME_NODE)
         Error() << "An expression cannot be moved." << value->getPos() << REPORT;

      value->check(cd);
      Node valueType = cd.type;
//      name->showDebug("Name"); // 090817
//      varType->showDebug("lhs"); // 090817
//      valueType->showDebug("rhs"); // 090817
      if (valueType)
         coerce(valueType, varType, value);
   }
}

//      {
//         FuncDef *fun = getConversion(valueType, varType);
//         if (fun == funBottom)
//         {
//            if (type)
//               Error() << "Variable type '" << varType->getEType() << "' cannot accept initial value type '" << valueType->getEType() << "'" << ep << REPORT;
//            else
//               Error() << "Variable type '" << varType->getEType() << "' cannot accept expression type '" << valueType->getEType() << "'" << ep << REPORT;
//         }
//         else if (fun != funIdentity)
//         {
//            if (value->kind() == DOT_NODE)
//               Error() << "Receive with implicit conversion is not allowed." << value->getPos() << REPORT;
//            else
//               coerce(fun, value);
//         }
//      }
//   }
//}

void VoidTypeNode::check(CheckData & cd)
{
   cd.type = this;
}

void BoolTypeNode::check(CheckData & cd)
{
   cd.type = this;
}

void ByteTypeNode::check(CheckData & cd)
{
   cd.type = this;
}

void FileTypeNode::check(CheckData & cd)
{
   cd.type = this;
}

void IntegerTypeNode::check(CheckData & cd)
{
   cd.type = this;
}

void FloatTypeNode::check(CheckData & cd)
{
   cd.type = this;
}

void DecimalTypeNode::check(CheckData & cd)
{
   cd.type = this;
}

void CharTypeNode::check(CheckData & cd)
{
   cd.type = this;
}

void TextTypeNode::check(CheckData & cd)
{
   cd.type = this;
}

void EnumTypeNode::check(CheckData & cd)
{
   cd.type = this;
   for (ListIter i = values.begin(); i != values.end(); ++i)
   {
      for (ListIter j = i + 1; j != values.end(); ++j)
         if ((*i)->getNameString() == (*j)->getNameString())
            Error() << "Enumeration name used twice." << (*j)->getPos() << REPORT;
      (*i)->check(cd);
   }
}

void EnumValueNode::check(CheckData & cd)
{
   checkName(cd, name, ep);
   type = cd.type;
}

void ArrayTypeNode::check(CheckData & cd)
{
   rangeType->check(cd);
   lo->check(cd);
   if (cd.type != theIntegerNode)
      Error() << "Lower bound of array must be an integer." << lo->getPos() << REPORT;
   hi->check(cd);
   if (cd.type != theIntegerNode)
      Error() << "Upper bound of array must be an integer." << hi->getPos() << REPORT;
   cd.type = this;
}

void MapTypeNode::check(CheckData & cd)
{
   domainType->check(cd);
   rangeType->check(cd);
}

/** Must choose function that requires fewest conversions. */
void FunctionNode::check(CheckData & cd)
{
   Node givenType = cd.type;
   for (ListIter it = args.begin(); it != args.end(); ++it)
      (*it)->check(cd);

   Node def = name->getDefinition();
   if (def)
   {
      Node val = def->getValue();
      if ( val && val->kind() == CPP_NODE )
      {
         matchArgList(args, val->getParamList(), ep);
         return;
      }
   }

   if (name->getNameString() == "exists")
   {
      if (args.size() == 1)
      {
         if (args[0]->kind() != SUBSCRIPT_NODE)
            Error() << "The argument of 'exists' should be a map element." << REPORT;
      }
      else
         Error() << "'exists' should have exactly one arugment." << REPORT;
      cd.type = theBoolNode;
      desc = new FuncDef("exists", "exists", EXISTS, theBoolNode, false);
      return;
   }

   int minConversions = MAX_CONV;
   FuncDef *bestFunction = 0;
   bool nameFound = false;

   // Find matching functions and count conversions needed.
   for (FuncIter it = funcDefs.begin(); it != funcDefs.end(); ++it)
   {
      if ((*it)->getUserName() == name->getNameString())
      {
         nameFound = true;
         List params = (*it)->getParams();
         if (params.size() == args.size())
         {
            int nc = countConversions(params, args);
            if (minConversions > nc)
            {
               minConversions = nc;
               bestFunction = *it;
            }
         }
      }
   }

   if ( ! nameFound)
   {
      Error() << "There is no function with this name." << ep << REPORT;
      return;
   }

   if (minConversions == MAX_CONV)
   {
      Error() << "The function cannot be applied to the given arguments." << ep << REPORT;
      return;
   }
   desc = bestFunction;
   cd.type = bestFunction->resultType();
   doConversions(bestFunction->getParams(), args);
}

void NameNode::check(CheckData & cd)
{
   cd.type = BaseNode::theVoidNode;
   if (definingOccurrence)
   {
      if (cd.withinProtocol)
      {
         if (cd.protNames.find(value) != cd.protNames.end())
            Error() << "Name '" << value << "' used more than once in protocol." << ep << REPORT;
         else
            cd.protNames.insert(value);
      }
      else
         checkName(cd, value, ep);
   }
   else if (definition)
      cd.type = definition->getType();

//   showDebug("Name"); // 090817
//   cd.type->showDebug("Type"); // 090817
}

void ThreadNode::check(CheckData & cd)
{
   for (ListIter it = inputs.begin(); it != inputs.end(); ++it)
      (*it)->check(cd);
   for (ListIter it = outputs.begin(); it != outputs.end(); ++it)
      (*it)->check(cd);
   seq->check(cd);
}

void ThreadParamNode::check(CheckData & cd)
{
   if (!name->kind() == NAME_NODE)
      Error() << "A thread parameter must be a simple name" << ep << REPORT;
}

void StartNode::check(CheckData & cd)
{
   for (ListIter it = calls.begin(); it != calls.end(); ++it)
      (*it)->check(cd);
   seq->check(cd);
}

void ThreadCallNode::check(CheckData & cd)
{
   for (ListIter it = inputs.begin(); it != inputs.end(); ++it)
      (*it)->check(cd);
   for (ListIter it = outputs.begin(); it != outputs.end(); ++it)
      (*it)->check(cd);
   List inpars, outpars;
   Node def = name->getValue();
   def->getThreadParams(inpars, outpars);

   if (inputs.size() < inpars.size())
      Error() << "Not enough input arguments" << ep << REPORT;
   else if (inputs.size() > inpars.size())
      Error() << "Too many input arguments" << ep << REPORT;
   else
   {
      ListIter args = inputs.begin();
      ListIter pars = inpars.begin();
      while (args != inputs.end())
      {
         if (!sameType((*args)->getType(), (*pars)->getType()))
            Error() << "Argument has wrong type" << (*args)->getPos() << REPORT;
         ++args;
         ++pars;
      }
   }

   if (outputs.size() < outpars.size())
      Error() << "Not enough output arguments" << ep << REPORT;
   else if (outputs.size() > outpars.size())
      Error() << "Too many output arguments" << ep << REPORT;
   else
   {
      ListIter args = outputs.begin();
      ListIter pars = outpars.begin();
      while (args != outputs.end())
      {
         if (!sameType((*args)->getType(), (*pars)->getType()))
            Error() << "Argument has wrong type" << (*args)->getPos() << REPORT;
         ++args;
         ++pars;
      }
   }
}



