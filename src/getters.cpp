/** \file getters.cpp
 * These functions query AST nodes for particular information.
 */

#include "ast.h"
#include "utilities.h"

#include <cassert>
#include <sstream>
#include <cstdlib>

using namespace std;

//------------------------------------------------getName

Node BaseNode::getName() const
{
   emergencyStop("getName", ep);
   return 0; // for VC7
}

Node InstanceNode::getName() const
{
   return name;
}

//------------------------------------------------getNameString

/** Return a string representation of the node's original name.  */
string BaseNode::getNameString() const
{
   emergencyStop("getNameString", ep);
   return ""; // for VC7
}

string DefNode::getNameString() const
{
   return value->getNameString();
}

string ProcedureNode::getNameString() const
{
   return name;
}

string ProcessNode::getNameString() const
{
   return name;
}

string InstanceNode::getNameString() const
{
   return name->getNameString();
}

string DotNode::getNameString() const
{
   return port->getNameString();
}

string QueryNode::getNameString() const
{
   return port->getNameString();
}

string DecNode::getNameString() const
{
   return name->getNameString();
}

string SubscriptNode::getNameString() const
{
   return base->getNameString();
}

string EnumTypeNode::getNameString() const
{
   return name;
}

string EnumValueNode::getNameString() const
{
   return name;
}

string NameNode::getNameString() const
{
   return value;
}

string CharNode::getNameString() const
{
   return string(1, value);
}

string TextNode::getNameString() const
{
   return value;
}

//------------------------------------------------getType

Node BaseNode::getType() const
{
   emergencyStop("getType()", ep);
   return 0; // for VC7
}

Node DefNode::getType() const
{
   return value->getType();
}

Node ProcedureNode::getType() const
{
   return theVoidNode;
}

Node ProcessNode::getType() const
{
   return theVoidNode;
}

Node CellNode::getType() const
{
   return theVoidNode;
}

Node CppNode::getType() const
{
   if (type)
      return type;
   else
      return theVoidNode;
}

Node ProtocolNode::getType() const
{
   return expr;
}

Node ThreadNode::getType() const
{
   return theVoidNode;
}

Node ConstantNode::getType() const
{
   return type;
}

Node DecNode::getType() const
{
   if (type)
      return type;
   else
      return name->getType();
}

Node DotNode::getType() const
{
   return field->getType();
}

Node QueryNode::getType() const
{
   return theBoolNode;
}

Node ComprehensionNode::getType() const
{
   return type;
}

Node NameNode::getType() const
{
   if (definition)
      return definition->getType();
   else if (type)
      return type;
   else
      return theVoidNode;
}

Node BoolNode::getType() const
{
   return BaseNode::theBoolNode;
}

Node CharNode::getType() const
{
   return BaseNode::theCharNode;
}

Node TextNode::getType() const
{
   return BaseNode::theTextNode;
}

Node NumNode::getType() const
{
   return type;
}

Node UnopNode::getType() const
{
   return type;
}

Node SubscriptNode::getType() const
{
   if (base->getType() == BaseNode::theTextNode)
      return BaseNode::theCharNode;
   else
      return base->getRangeType();
}

Node SubrangeNode::getType() const
{
   return theTextNode;
}

Node EnumTypeNode::getType() const
{
   return const_cast<EnumTypeNode*>(this);
}

Node EnumValueNode::getType() const
{
   return type;
}

Node ArrayTypeNode::getType() const
{
   return const_cast<ArrayTypeNode*>(this);
}

Node MapTypeNode::getType() const
{
   return const_cast<MapTypeNode*>(this);
}

Node IteratorNode::getType() const
{
   switch (fun)
   {
      case ITER_KEY:
         return map->getDomainType();
      case ITER_VALUE:
         return map->getRangeType();
      case ITER_FINISH:
         return theBoolNode;
      default:
         return theVoidNode;
   }
}

Node BinopNode::getType() const
{
   return type;
}

Node CondExprNode::getType() const
{
   return type;
}

Node FunctionNode::getType() const
{
   if (desc)
      return desc->resultType();
   else
      return theVoidNode;
}

Node ThreadParamNode::getType() const
{
   return type;
}

//------------------------------------------------getDomainType

Node BaseNode::getDomainType() const
{
   emergencyStop("getDomainType" , ep);
   return 0; // for VC7
}

Node ComprehensionNode::getDomainType() const
{
   return theVoidNode;
}

Node DefNode::getDomainType() const
{
   return value->getDomainType();
}

Node NameNode::getDomainType() const
{
   if (definition)
      return definition->getDomainType();
   else
      return 0;
}

Node DecNode::getDomainType() const
{
   return type->getDomainType();
}

Node ArrayTypeNode::getDomainType() const
{
   return theIntegerNode;;
}

Node MapTypeNode::getDomainType() const
{
   return domainType;
}

Node EnumTypeNode::getDomainType() const
{
   return const_cast<EnumTypeNode*>(this);
}

Node TextTypeNode::getDomainType() const
{
   return theIntegerNode;
}

Node TextNode::getDomainType() const
{
   return theIntegerNode;
}

Node SubscriptNode::getDomainType() const
{
   Node type = this->getType();
   return type->getRangeType();
}

Node IterTypeNode::getDomainType() const
{
   return domainType;
}

//------------------------------------------------getRangeType

Node BaseNode::getRangeType() const
{
   emergencyStop("getRangeType" , ep);
   return 0; // for VC7
}

Node ComprehensionNode::getRangeType() const
{
   return theVoidNode;
}

Node DefNode::getRangeType() const
{
   return value->getRangeType();
}

Node NameNode::getRangeType() const
{
   if (definition)
      return definition->getRangeType();
   else
      return 0;
}

Node DecNode::getRangeType() const
{
   return type->getRangeType();
}

Node SubscriptNode::getRangeType() const
{
   Node type = this->getType();
   return type->getRangeType();
}

Node MapTypeNode::getRangeType() const
{
   return rangeType;
}

Node EnumTypeNode::getRangeType() const
{
   return theVoidNode;
}

Node TextTypeNode::getRangeType() const
{
   return theCharNode;
}

Node TextNode::getRangeType() const
{
   return theCharNode;
}

Node ArrayTypeNode::getRangeType() const
{
   return rangeType;
}

Node IterTypeNode::getRangeType() const
{
   return rangeType;
}

//------------------------------------------------getCTypeString

/** Return a string representation of the node's type.  */
string BaseNode::getCTypeString() const
{
   emergencyStop("getCTypeString", ep);
   return ""; // for VC7
}

string ProtocolNode::getCTypeString() const
{
   return "Channel *";
}

string DefNode::getCTypeString() const
{
   return value->getCTypeString();
}

string ConstantNode::getCTypeString() const
{
   return type->getCTypeString();
}

string DecNode::getCTypeString() const
{
   if (type)
      return type->getCTypeString();
   else
      return "";
}

string ComprehensionNode::getCTypeString() const
{
   if (type)
      return type->getCTypeString();
   else
      return "";
}

string DotNode::getCTypeString() const
{
   return field->getCTypeString();
}

string QueryNode::getCTypeString() const
{
   return field->getCTypeString();
}

string NameNode::getCTypeString() const
{
   if (definition)
      return definition->getCTypeString();
   else
      emergencyStop("NameNode::getCTypeString", ep);
   return ""; // for VC7
}

string BinopNode::getCTypeString() const
{
   return type->getCTypeString();
}

string CondExprNode::getCTypeString() const
{
   return type->getCTypeString();
}

string UnopNode::getCTypeString() const
{
   return type->getCTypeString();
}

string SubscriptNode::getCTypeString() const
{
   if (base->getType() == BaseNode::theTextNode)
      return "char";
   else
      return base->getCTypeString();
}

string SubrangeNode::getCTypeString() const
{
   return "string";
}

string VoidTypeNode::getCTypeString() const
{
   return "void";
}

string BoolTypeNode::getCTypeString() const
{
   return "bool";
}

string ByteTypeNode::getCTypeString() const
{
   return unSigned ? "unsigned char" : "char";
}

string IntegerTypeNode::getCTypeString() const
{
   return unSigned ? "unsigned int" : "int";
}

string FileTypeNode::getCTypeString() const
{
   switch (mode)
   {
      case FILE_INPUT:
         return "ifstream";
      case FILE_OUTPUT:
         return "ofstream";
   }
   return "";
}

string FloatTypeNode::getCTypeString() const
{
   return "double";
}

string DecimalTypeNode::getCTypeString() const
{
   return "double";
}

string CharTypeNode::getCTypeString() const
{
   return "char";
}

string TextTypeNode::getCTypeString() const
{
   return "string";
}

string EnumTypeNode::getCTypeString() const
{
   return "int";
}

string EnumValueNode::getCTypeString() const
{
   return "int";
}

string ArrayTypeNode::getCTypeString() const
{
   return "Array<" + rangeType->getCTypeString() + ">";
}

string MapTypeNode::getCTypeString() const
{
   return "map<" +
          domainType->getCTypeString() + "," +
          rangeType->getCTypeString() +
          (rangeType->isMap() ? " >" : ">");
}

string BoolNode::getCTypeString() const
{
   return "bool";
}

string CharNode::getCTypeString() const
{
   return "char";
}

string TextNode::getCTypeString() const
{
   return "string";
}

string NumNode::getCTypeString() const
{
   if (type == BaseNode::theIntegerNode)
      return "int";
   else if (type == BaseNode::theFloatNode)
      return "double";
   else if (type == BaseNode::theDecimalNode)
      return "double";
   else if (type == BaseNode::theCharNode)
      return "char";
   else if (type == BaseNode::theTextNode)
      return "string";
   else
      return "void"; // for VC7
}

string FunctionNode::getCTypeString() const
{
   if (desc)
      return desc->resultType()->getCTypeString();
   else
      return "";
}

string ThreadParamNode::getCTypeString() const
{
   return type->getCTypeString();
}

//------------------------------------------------getFullname

/** The 'full name' of an object is its source-code name decorated,
  * usually with a serial number, to ensure uniqueness.
  * \param withpointer indicates that the name should be prefixed with "_ptr->".
  */
string BaseNode::getFullName(bool withpointer) const
{
   emergencyStop("getFullName", ep);
   return ""; // for VC7
}

string InstanceNode::getFullName(bool withpointer) const
{
   return name->getFullName();
}

string DecNode::getFullName(bool withpointer) const
{
   return name->getFullName();
}

string SubscriptNode::getFullName(bool withpointer) const
{
   ostringstream os;
   os << base->getFullName(withpointer) << '[';
   //sub->write(os);
   os << ']';
   return os.str();
}

string ThreadParamNode::getFullName(bool withpointer) const
{
   return name->getFullName(withpointer);
}

string NameNode::getFullName(bool withpointer) const
{
   return value + str(varNum);
}

//------------------------------------------------getEType

string BaseNode::getEType() const
{
   emergencyStop("getEType", ep);
   return "undefined";
}

string VoidTypeNode::getEType() const
{
   return "Void";
}

string EnumTypeNode::getEType() const
{
   ostringstream os;
   if (values.size() ==0)
      os << "Enum";
   else
      os << '<' << values[0]->getNameString() << "...>";
   return os.str();
}

string BoolTypeNode::getEType() const
{
   return "Bool";
}

string ByteTypeNode::getEType() const
{
   return unSigned ? "unsigned Byte" : "Byte";
}

string IntegerTypeNode::getEType() const
{
   return unSigned ? "unsigned Integer" : "Integer";
}

string FloatTypeNode::getEType() const
{
   return "Float";
}

string DecimalTypeNode::getEType() const
{
   return "Decimal";
}

string CharTypeNode::getEType() const
{
   return "Char";
}

string TextTypeNode::getEType() const
{
   return "Text";
}

string FileTypeNode::getEType() const
{
   switch (mode)
   {
      case FILE_INPUT:
         return "InFile";
      case FILE_OUTPUT:
         return "OutFile";
   }
}

string ArrayTypeNode::getEType() const
{
   return "Array";
}

string DefNode::getEType() const
{
   return value->getEType();
}

string ProtocolNode::getEType() const
{
   ostringstream os;
   prettyPrint(os, 0);
   return os.str();
}

string NameNode::getEType() const
{
   return definition ? definition->getEType() : "undefined";
}


//------------------------------------------------getPortBuffername

/** Return the name of the buffer in a Port object
  * corresponding to a given type.
  */
string BaseNode::getPortBufferName() const
{
   emergencyStop("getPortBufferName", ep);
   return ""; // for VC7
}

string DefNode::getPortBufferName() const
{
   return (value->kind() == ENUM_TYPE_NODE)
          ? "pInt" // Values of an enumeration are passed as integers
          : value->getPortBufferName();

//   if (value->kind() == ENUM_TYPE_NODE)
//      return "pInt"; // Values of an enumeration are passed as integers
//   else
//      return value->getPortBufferName(); // 080916
}

string DotNode::getPortBufferName() const
{
   return field->getPortBufferName();
}

string QueryNode::getPortBufferName() const
{
   return field->getPortBufferName();
}

string DecNode::getPortBufferName() const
{
   if (type)
   {
      if (type->isPort())
         return "ppChannel";
      else
         return type->getPortBufferName();
   }
   else
      return "";
}

string ArrayTypeNode::getPortBufferName() const
{
   return ""; // Port name cannot be constructed dynamically.
}

string BoolTypeNode::getPortBufferName() const
{
   return "pBool";
}

string ByteTypeNode::getPortBufferName() const
{
   return (unSigned ? "pUnsignedByte" : "pByte");
}

string FileTypeNode::getPortBufferName() const
{
   return "pFile";
}

string CharTypeNode::getPortBufferName() const
{
   return "pChar";
}

string IntegerTypeNode::getPortBufferName() const
{
   return (unSigned ? "pUnsignedInt" : "pInt");
}

string FloatTypeNode::getPortBufferName() const
{
   return "pDouble";
}

string DecimalTypeNode::getPortBufferName() const
{
   return "pDouble";
}

string TextTypeNode::getPortBufferName() const
{
   return "pString";
}

string ProtocolNode::getPortBufferName() const
{
   return "ppChannel";
}

string NameNode::getPortBufferName() const
{
   if (definition)
      return definition->getPortBufferName();
   else
      return "";
}

//------------------------------------------------getDefinition

/** Return the definition of a NameNode. */
Node BaseNode::getDefinition() const
{
   emergencyStop("getDefinition", ep);
   return 0; // for VC7
}

Node SubscriptNode::getDefinition() const
{
   return base->getDefinition();
}

Node NameNode::getDefinition() const
{
   return definition;
}

//------------------------------------------------getCommunicator

/** Return the "other end" of a communication.
  * A server port returns "client" and vice versa.
  */
string BaseNode::getCommunicator() const
{
   emergencyStop("getCommunicator", ep);
   return ""; // for VC7
}

string NameNode::getCommunicator() const
{
   return definition->getCommunicator();
}

//------------------------------------------------getProtocol

/** Return the protocol field of a PortDecNode. */
Node BaseNode::getProtocol()
{
   emergencyStop("getProtocol", ep);
   return 0; // for VC7
}

Node DefNode::getProtocol()
{
   return value->getProtocol();
}

Node DotNode::getProtocol()
{
   return field->getProtocol();
}

Node QueryNode::getProtocol()
{
   return field->getProtocol();
}

Node ProtocolNode::getProtocol()
{
   return this;
}

Node DecNode::getProtocol()
{
   if (type)
      return type->getProtocol();
   else
      return 0;
}

Node ArrayTypeNode::getProtocol()
{
   return 0;
}

Node MapTypeNode::getProtocol()
{
   return rangeType->getProtocol();
}

Node NameNode::getProtocol()
{
   if (definition)
      return definition->getProtocol();
   else
   {
      emergencyStop("NameNode::getProtocol", ep);
      return 0; // for VC7
   }
}

Node ThreadParamNode::getProtocol()
{
   return type->getProtocol();
}

//------------------------------------------------getValue

/** Return the value field of a DefNode. */
Node BaseNode::getValue() const
{
   emergencyStop("getValue", ep);
   return 0; // for VC7
}

Node DefNode::getValue() const
{
   return value;
}

Node DecNode::getValue() const
{
   return 0;
}

Node NameNode::getValue() const
{
   if (definition)
      return definition->getValue();
   else
   {
      emergencyStop("NameNode::getValue", ep);
      return 0; // for VC7
   }
}

//------------------------------------------------getMessKind

/** Get message kind (NO_FIELD | QUERY | REPLY). */
MessageKind BaseNode::getMessKind() const
{
   emergencyStop("getMessKind", ep);
   return NO_FIELD;
}

MessageKind DecNode::getMessKind() const
{
   return mk;
}

MessageKind DotNode::getMessKind() const
{
   return field->getMessKind();
}

MessageKind QueryNode::getMessKind() const
{
   return field->getMessKind();
}

MessageKind NameNode::getMessKind() const
{
   if (definition)
      return definition->getMessKind();
   else
      return NO_FIELD;
}

//------------------------------------------------getPortKind

/** Get port kind (NO_ROLE | CHANNEL | CLIENT | SERVER).
  * \param slotNum is the number of a parameter of a cell or process.
  */
PortKind BaseNode::getPortKind(int slotNum) const
{
   //emergencyStop("getPortKind", ep);
   return NO_ROLE;
}

PortKind ProcedureNode::getPortKind(int slotNum) const
{
   if (slotNum < params.size())
      return params[slotNum]->getPortKind();
   else
   {
      cerr << "Compiler error in ProcessNode::getPortKind\n";
      return NO_ROLE;
   }
}

PortKind ProcessNode::getPortKind(int slotNum) const
{
   if (slotNum < params.size())
      return params[slotNum]->getPortKind();
   else
   {
      cerr << "Compiler error in ProcessNode::getPortKind\n";
      return NO_ROLE;
   }
}

PortKind CellNode::getPortKind(int slotNum) const
{
   if (slotNum < params.size())
      return params[slotNum]->getPortKind();
   else
   {
      cerr << "Compiler error in ProcessNode::getPortKind\n";
      return NO_ROLE;
   }
}

PortKind DefNode::getPortKind(int slotNum) const
{
   return value->getPortKind(slotNum);
}

PortKind DotNode::getPortKind(int slotNum) const
{
   //showDebug("Dot " + portToString(port->getPortKind(slotNum)));
   return port->getPortKind(slotNum);
}

PortKind QueryNode::getPortKind(int slotNum) const
{
   return port->getPortKind(slotNum);
}

PortKind SubscriptNode::getPortKind(int slotNum) const
{
   //showDebug("Sub " + portToString(base->getPortKind(slotNum)));
   return base->getPortKind(slotNum);
}

PortKind DecNode::getPortKind(int slotNum) const
{
   PortKind pk;
   if (portDir == CLIENT || portDir == SERVER)
      pk = portDir;
   else if (type)
      pk = type->getPortKind();
   else
      pk = NO_ROLE;
   //showDebug("Dec " + portToString(pk));
   return pk;
}

PortKind MapTypeNode::getPortKind(int slotNum) const
{
   //showDebug("MapType " + portToString(portDir));
   return portDir;
}

PortKind ThreadParamNode::getPortKind(int slotNum) const
{
   return portDir;
}

PortKind NameNode::getPortKind(int slotNum) const
{
   return definition ? definition->getPortKind(slotNum) : NO_ROLE;
//   if (definition)
//   {
//      PortKind portDir = definition->getPortKind(slotNum);
//      //showDebug("Name " + portToString(portDir));
//      return portDir;
//   }
//   emergencyStop("NameNode::getPortKind", ep);
//   return NO_ROLE;
}

//------------------------------------------------getOwner

string BaseNode::getOwner() const
{
   emergencyStop("getOwner", ep);
   return ""; // for VC7
}

string DefNode::getOwner() const
{
   return name->getOwner();
}

string ComprehensionNode::getOwner() const
{
   return var->getOwner();
}

string SubscriptNode::getOwner() const
{
   return base->getOwner();
}

string ConstantNode::getOwner() const
{
   return name->getOwner();
}

string DecNode::getOwner() const
{
   return name->getOwner();
}

string EnumValueNode::getOwner() const
{
   return "";
}

string NameNode::getOwner() const
{
   return owner;
}

string ThreadParamNode::getOwner() const
{
   return owner;
}

//------------------------------------------------getOffset

int BaseNode::getOffset() const
{
   emergencyStop("getOffset", ep);
}

int DecNode::getOffset() const
{
   return name->getOffset();
}

int NameNode::getOffset() const
{
   if (definingOccurrence)
      return offset;
   else if (definition)
      return definition->getOffset();
   else
      return 0;
}

//------------------------------------------------getFieldNum

int BaseNode::getFieldNum() const
{
   emergencyStop("getFieldNum", ep);
   return 0; // for VC7
}

int DecNode::getFieldNum() const
{
   return name->getFieldNum();
}

int DotNode::getFieldNum() const
{
   return field->getFieldNum();
}

int QueryNode::getFieldNum() const
{
   return field->getFieldNum();
}

int FunctionNode::getFieldNum() const
{
   return args[0]->getFieldNum();
}

int NameNode::getFieldNum() const
{
   if (definingOccurrence)
      return fieldNum;
   else if (definition)
      return definition->getFieldNum();
   else
      return 0;
}

//------------------------------------------------getPort

Node BaseNode::getPort() const
{
   emergencyStop("getPort", ep);
   return 0; // for VC7
}

Node DotNode::getPort() const
{
   return port;
}

Node QueryNode::getPort() const
{
   return port;
}

Node FunctionNode::getPort() const
{
   return args[0]->getPort();
}

Node NameNode::getPort() const
{
   if (definition)
      return definition->getPort();
   else
      return 0;
}

//------------------------------------------------getField

Node BaseNode::getField() const
{
   emergencyStop("getField", ep);
   return 0; // for VC7
}

Node DotNode::getField() const
{
   return field;
}

Node QueryNode::getField() const
{
   return field;
}

Node NameNode::getField() const
{
   if (definition)
      return definition->getField();
   emergencyStop("getField", ep);
   return 0; // for VC7
}

//------------------------------------------------getArg

Node BaseNode::getArg() const
{
   emergencyStop("getArg", ep);
   return 0; // for VC7
}

Node UnopNode::getArg() const
{
   return operand;
}

//------------------------------------------------getArgList

List BaseNode::getArgList() const
{
   emergencyStop("getArgList", ep);
   return List(); // for VC7
}

List InstanceNode::getArgList() const
{
   return args;
}

List ListopNode::getArgList() const
{
   return args;
}

//------------------------------------------------getParam

Node BaseNode::getParam(int index) const
{
   emergencyStop("getParam", ep);
   return 0; // for VC7
}

Node ProcedureNode::getParam(int index) const
{
   return params[index];
}

Node ProcessNode::getParam(int index) const
{
   return params[index];
}

//------------------------------------------------getParamList

List BaseNode::getParamList() const
{
   emergencyStop("getParamList", ep);
   return List(); // for VC7
}

List ProtocolNode::getParamList() const
{
   Error() << "Protocol used in an invalid context." << getPos() << REPORT;
   return List();
}

List CellNode::getParamList() const
{
   return params;
}

List CppNode::getParamList() const
{
   return params;
}

List ProcedureNode::getParamList() const
{
   return params;
}

List ProcessNode::getParamList() const
{
   return params;
}

//------------------------------------------------getVarNum

int BaseNode::getVarNum() const
{
   emergencyStop("getVarNum", ep);
   return 0; // for VC7
}

int DefNode::getVarNum() const
{
   return name->getVarNum();
}

int ComprehensionNode::getVarNum() const
{
   return var->getVarNum();
}

int ConstantNode::getVarNum() const
{
   return name->getVarNum();
}

int DecNode::getVarNum() const
{
   return name->getVarNum();
}

int EnumValueNode::getVarNum() const
{
   return varNum;
}

int NameNode::getVarNum() const
{
   return varNum;
}

int ThreadParamNode::getVarNum() const
{
   return name->getVarNum();
}

//------------------------------------------------getOp

Operator BaseNode::getOp() const
{
   emergencyStop("getOp", ep);
   return UNOP_NOT; // for VC7
}

Operator ListopNode::getOp() const
{
   return op;
}

Operator BinopNode::getOp() const
{
   return op;
}

Operator UnopNode::getOp() const
{
   return op;
}

//------------------------------------------------getTie

Node BaseNode::getTie() const
{
   emergencyStop("getTie", ep);
   return 0; // for VC7
}

Node DecNode::getTie() const
{
   return name->getTie();
}

Node NameNode::getTie() const
{
   return tie;
}

//------------------------------------------------getNumFields

int BaseNode::getNumFields() const
{
   emergencyStop("getNumFields" , ep);
   return 0; // for VC7
}

int DefNode::getNumFields() const
{
   return value->getNumFields();
}

int ProtocolNode::getNumFields() const
{
   return numFields;
}

int NameNode::getNumFields() const
{
   return definition ? definition->getNumFields() : 0;
}

//------------------------------------------------getQueueTest

string BaseNode::getQueueTest() const
{
   Error() << "First statement of select branch must send or receive." << ep << REPORT;
   return "";
}

string SequenceNode::getQueueTest() const
{
   return stmts.front()->getQueueTest();
}

/**
 * \todo The code below is repeated in gen.cpp - factor it.
 * \todo Must check for sys.xxx here
 */
string DecNode::getQueueTest() const
{
   enum
   {
      SENDING, RECEIVING, ASSIGNMENT
   }
   kind = ASSIGNMENT;
   bool signal = false;
   Node port = 0;
   string field = "";

   if (name->kind() == DOT_NODE)
   {
      port = name->getPort();
      if (port->getNameString() != BASIO_SYS)
      {
         field = str(name->getFieldNum());
         if (value)
            kind = SENDING;
         else
         {
            // DotNode and no value => signal
            switch (name->getPortKind())
            {
               case SERVER:
                  kind = RECEIVING;
                  break;
               case CLIENT:
                  kind = SENDING;
                  break;
            }
            signal = true;
         }
      }
   }
   else if (value && value->kind() == DOT_NODE)
   {
      port = value->getPort();
      if (port->getNameString() != BASIO_SYS)
      {
         field = str(value->getFieldNum());
         kind = RECEIVING;
      }
   }

   if (kind == ASSIGNMENT)
      Error() << "First statement of select branch must send or receive." << ep << REPORT;
   else if (port)
   {
      string portName = port->getFullName(true);
      return "!" + portName + "->idle() && " + portName + "->check(" + field + ')';
   }
   else
      Error() << "Compiler error in select statement." << ep << THROW;
   return "";

//   assert(kind == ASSIGNMENT || port);
//
//   // Code for new communication model
//   switch (kind)
//   {
//      case SENDING:
//      case RECEIVING:
//      {
//         string portName = port->getFullName(true);
//         return "!" + portName + "->idle() && " + portName + "->check(" + field + ')';
//      }
//      case ASSIGNMENT:
//         Error() << "First statement of select branch must send or receive." << ep << REPORT;
//         break;
//   }
//
//   switch (kind)
//   {
//      case SENDING:
//         return port->getFullName(true) + "->readers[" + field + ']';
//      case RECEIVING:
//         return port->getFullName(true) + "->writers[" + field + ']';
//      case ASSIGNMENT:
//         Error() << "First statement of select branch must send or receive." << ep << REPORT;
//         break;
//   }

   return "";
}

//------------------------------------------------getLTS

LTS *BaseNode::getLTS() const
{
   emergencyStop("getLTS", ep);
   return 0; // for VCS 7
}

LTS *ProtocolNode::getLTS() const
{
   return plts;
}

//------------------------------------------------getFieldDecs

set<Node> BaseNode::getFieldDecs() const
{
   emergencyStop("getFieldDecs", ep);
   return set<Node>(); // for VCS 7
}

set<Node> ProtocolNode::getFieldDecs() const
{
   return fieldDecs;
}

//------------------------------------------------getFun

string BaseNode::getFun() const
{
   // Every node must return a value.
   return "";
}

string FunctionNode::getFun() const
{
   if (name)
      return name->getNameString();
   else
      return desc->getUserName();
}

//-------------------------------------------------------getLoopVar

string BaseNode::getLoopVar(Node var) const
{
   return "";
}

string ComprehensionNode::getLoopVar(Node) const
{
   return collection->getLoopVar(var);
}

string MapSetNode::getLoopVar(Node var) const
{
   string iter = var->getFullName(true);
   switch (mapKind)
   {
      case INDEXED:
         switch (setKind)
         {
            case MAPSET_DOMAIN:
               return iter + "->first";
            case MAPSET_RANGE:
               return iter + "->second";
            case MAPSET_PAIR:              // not implemented yet
               return "";
         }
         break;

      case ARRAY:
      case TEXT:
         switch (setKind)
         {
            case MAPSET_DOMAIN:
               return iter;
            case MAPSET_RANGE:
               return map->getFullName() + "[" + indexName + "]";
            case MAPSET_PAIR:
               return "";
         }
         break;
   }
}

string NameNode::getLoopVar(Node var) const
{
   return definition ? definition->getLoopVar(var) : "";
}

//------------------------------------------------getLoopVarName

//void BaseNode::getLoopVarName(Node var, MapSetKind & msKind, string & name) const
//{
//   msKind = MAPSET_UNASSIGNED;
//   name = "";
//}

//void ComprehensionNode::getLoopVarName(Node dummyVar, MapSetKind & msKind, string & name) const
//{
//   collection->getLoopVarName(var, msKind, name);
//}

//void MapSetNode::getLoopVarName(Node var, MapSetKind & msKind, string & name) const
//{
//   msKind = setKind;
//   string iter = var->getFullName(true);
//   switch (mapKind)
//   {
//      case INDEXED:
//         switch (setKind)
//         {
//            case MAPSET_DOMAIN:
//               name = iter + "->first";
//               break;
//            case MAPSET_RANGE:
//               name = iter + "->second";
//               break;
//            case MAPSET_PAIR:
//               name = iter;
//               break;
//         }
//         break;
//
//      case ARRAY:
//      case TEXT:
//         switch (setKind)
//         {
//            case MAPSET_DOMAIN:
//               name = iter;
//               break;
//            case MAPSET_RANGE:
//               name = map->getFullName() + "[" + iter + "]";
//               break;
//            case MAPSET_PAIR:
//               name = iter;
//               break;
//         }
//         break;
//   }
//   cerr << "loop var = " << name << endl;
//
////   switch (setKind)
////   {
////      case MAPSET_DOMAIN:
////         name = iter + "->first";
////         break;
////      case MAPSET_RANGE:
////         name = iter + "->second";
////         break;
////      case MAPSET_PAIR:
////         name = iter;
////         break;
////   }
//}

//void NameNode::getLoopVarName(Node var, MapSetKind & msKind, string & name) const
//{
//   if (definition)
//      definition->getLoopVarName(var, msKind, name);
//   else
//   {
//      msKind = MAPSET_UNASSIGNED;
//      name = "";
//   }
//}

//------------------------------------------------getIterator

void BaseNode::getIterator(Node var, MapSetKind & msKind, Node & it) const
{
   msKind = MAPSET_UNASSIGNED;
   it = 0;
}

void ComprehensionNode::getIterator(Node, MapSetKind & msKind, Node & it) const
{
   collection->getIterator(var, msKind, it);
}

void MapSetNode::getIterator(Node var, MapSetKind & msKind, Node & it) const
{
   msKind = setKind;
   it = iter;
}

void NameNode::getIterator(Node var, MapSetKind & msKind, Node & it) const
{
   if (definition)
      definition->getIterator(var, msKind, it);
   else
   {
      msKind = MAPSET_UNASSIGNED;
      it = 0;
   }
}

//----------------------------------------------------------------getConstValue

string BaseNode::getConstValue() const
{
   cerr << "Warning: unable to create EVM code for this expression." << ep;
   return "0";
}

string BoolNode::getConstValue() const
{
   return value ? "T" : "F";
}

string NumNode::getConstValue() const
{
   return value;
}

string CharNode::getConstValue() const
{
   return string(1, value);
}

string TextNode::getConstValue() const
{
   return value;
}

//----------------------------------------------------------------getEnumSize

Node BaseNode::getEnumSize() const
{
   emergencyStop("getEnumSize", ep);
   return 0;
}

Node DefNode::getEnumSize() const
{
   return value->getEnumSize();
}

Node EnumTypeNode::getEnumSize() const
{
   return new NumNode(Errpos(), theIntegerNode, str(values.size()));
}

Node NameNode::getEnumSize() const
{
   return definition ? definition->getEnumSize() : 0;
}

//----------------------------------------------------------------getThreadParams

void BaseNode::getThreadParams(List &, List &)
{
   emergencyStop("getThreadParams", ep);
}

void ThreadNode::getThreadParams(List & inpars, List & outpars)
{
   inpars = inputs;
   outpars = outputs;
}

//----------------------------------------------------------------getLo

Node BaseNode::getLo() const
{
   return 0;
}

Node ArrayTypeNode::getLo() const
{
   return lo;
}

//----------------------------------------------------------------getHi

Node BaseNode::getHi() const
{
   emergencyStop("getHi", ep);
   return 0;
}

Node ArrayTypeNode::getHi() const
{
   return hi;
}

//--------------------------------------------------------------getEVMTypeCode

string BaseNode::getEVMTypeCode() const
{
   emergencyStop("getEVMTypeCode", ep);
   return "U";
}

string DefNode::getEVMTypeCode() const
{
   return value->getEVMTypeCode();
}

string ProtocolNode::getEVMTypeCode() const
{
   return "P";
}

string DecNode::getEVMTypeCode() const
{
   if (type)
      return type->getEVMTypeCode();
   else
      return "S";
}

string VoidTypeNode::getEVMTypeCode() const
{
   return "V";
}

string BoolTypeNode::getEVMTypeCode() const
{
   return "B";
}

string ByteTypeNode::getEVMTypeCode() const
{
   return "O";
}

string IntegerTypeNode::getEVMTypeCode() const
{
   return "I";
}

string FloatTypeNode::getEVMTypeCode() const
{
   return "F";
}

string DecimalTypeNode::getEVMTypeCode() const
{
   return "D";
}

string CharTypeNode::getEVMTypeCode() const
{
   return "C";
}

string TextTypeNode::getEVMTypeCode() const
{
   return "T";
}

string EnumTypeNode::getEVMTypeCode() const
{
   return "I";
}

string FileTypeNode::getEVMTypeCode() const
{
   return "F";
}

string ArrayTypeNode::getEVMTypeCode() const
{
   return "A" + getDomainType()->getEVMTypeCode();
}

string MapTypeNode::getEVMTypeCode() const
{
   return "M" + getDomainType()->getEVMTypeCode() + getRangeType()->getEVMTypeCode();
}

string IterTypeNode::getEVMTypeCode() const
{
   return "X" + getDomainType()->getEVMTypeCode() + getRangeType()->getEVMTypeCode();
}

string NameNode::getEVMTypeCode() const
{
   return definition ? definition->getEVMTypeCode() : "U";
}

//----------------------------------------------------------------getEVMBlockNumber

int BaseNode::getEVMBlockNumber() const
{
   emergencyStop("getEVMBlockNumber", ep);
   return 0;
}

int DefNode::getEVMBlockNumber() const
{
   return value->getEVMBlockNumber();
}

int CellNode::getEVMBlockNumber() const
{
   return evmBlockNum;
}

int ProcessNode::getEVMBlockNumber() const
{
   return evmBlockNum;
}

int ProtocolNode::getEVMBlockNumber() const
{
   return evmBlockNum;
}

int DecNode::getEVMBlockNumber() const
{
   return name->getEVMBlockNumber();
}

int ConstantNode::getEVMBlockNumber() const
{
   return evmBlockNum;
}

int EnumValueNode::getEVMBlockNumber() const
{
   return evmBlockNum;
}

int ComprehensionNode::getEVMBlockNumber() const
{
   return var->getEVMBlockNumber();
}

int NameNode::getEVMBlockNumber() const
{
//   if (definition)
//   {
//      int bn = definition->getEVMBlockNumber();
//      definition->showDebug("NameNode " + str(bn));
//   }
   return definition ? definition->getEVMBlockNumber() : evmBlockNum;
}

//----------------------------------------------------------------getMapKind

MapKind BaseNode::getMapKind() const
{
//   emergencyStop("getMapKind", ep);
   return NO_MAP_KIND;
}

MapKind MapSetNode::getMapKind() const
{
   return mapKind;
}

//----------------------------------------------------------------getMapSetKind

MapSetKind BaseNode::getMapSetKind() const
{
//   emergencyStop("getMapSetKind", ep);
   return MAPSET_UNASSIGNED;
}

MapSetKind MapSetNode::getMapSetKind() const
{
   return setKind;
}

//----------------------------------------------------------------getx

void BaseNode::getx(int & fn) const
{
   emergencyStop("BaseNode::getx", ep);
}

void UnopNode::getx(int & fn) const
{
   operand->getx(fn);
}

void BinopNode::getx(int & fn) const
{
   lhs->getx(fn);
   rhs->getx(fn);
}

void ListopNode::getx(int & fn) const
{
   for (ListIter it = args.begin(); it != args.end(); ++it)
      (*it)->getx(fn);
}

void DecNode::getx(int & fn) const
{
   name->getx(fn);
}

void NameNode::getx(int & fn) const
{
   if (fn <= fieldNum)
      fn = fieldNum + 1;
}



//------------------------------------------------------NumNode and EnumTypeNode

int BaseNode::getIntVal() const
{
   emergencyStop("getIntVal", ep);
   return 0;
}

int NumNode::getIntVal() const
{
   assert(type == theIntegerNode);
   return atoi(value.c_str());
}

int EnumTypeNode::getIntVal() const
{
   return values.size();
}

double BaseNode::getDecVal() const
{
   emergencyStop("getDecVal", ep);
   return 0;
}

double NumNode::getDecVal() const
{
   assert(type == theDecimalNode);
   return atof(value.c_str());
}


