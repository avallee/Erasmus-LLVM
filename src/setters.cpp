/** \file setters.cpp
  * Functions that set attributes in AST nodes.
  */

#include "ast.h"

//--------------------------------------------------------- setDefining

/** Set defining occurrence flag to true in a NameNode. */
void BaseNode::setDefining(bool def)
{
   emergencyStop("setDefining", ep);
}

void DotNode::setDefining(bool def)
{
   field->setDefining(def);
}

void QueryNode::setDefining(bool def)
{
   field->setDefining(def);
}

void SubscriptNode::setDefining(bool def)
{
   base->setDefining(def);
}

void NameNode::setDefining(bool def)
{
   definingOccurrence = def;
}

//--------------------------------------------------------- setStartAddress

/** Set start address for closure node. */
void BaseNode::setStartAddress(int s)
{
   emergencyStop("setStartAddress", ep);
}

void ProcessNode::setStartAddress(int s)
{
   start = s;
}

//--------------------------------------------------------- setName

void BaseNode::setName(string n)
{}

void ProcedureNode::setName(string n)
{
   name = n;
}

void ProcessNode::setName(string n)
{
   name = n;
}

void CellNode::setName(string n)
{
   name = n;
}

void ThreadNode::setName(string n)
{
   name = n;
}

//--------------------------------------------------------- setReference

void BaseNode::setReference()
{}

void DecNode::setReference()
{
   reference = true;
}

//--------------------------------------------------------- setParameter

void BaseNode::setParameter()
{}

void DecNode::setParameter()
{
   parameter = true;
}

//--------------------------------------------------------- setTie

void BaseNode::setTie(Node t)
{}

void DecNode::setTie(Node t)
{
   name->setTie(t);
}

void NameNode::setTie(Node t)
{
   tie = t;
}

//--------------------------------------------------------- setFieldNum

void BaseNode::setFieldNum(int fn)
{}

void NameNode::setFieldNum(int fn)
{
   fieldNum = fn;
}

//--------------------------------------------------------- setOffset

void BaseNode::setOffset(int os)
{}

void NameNode::setOffset(int os)
{
   offset = os;
}

//--------------------------------------------------------- setType

void BaseNode::setType(Node newType)
{}

void EnumValueNode::setType(Node newType)
{
   type = newType;
}

void NameNode::setType(Node newType)
{
   type = newType;
}


//--------------------------------------------------------- setPassByReference

void BaseNode::setPassByReference()
{}

void NameNode::setPassByReference()
{
   passByReference = true;
}

//---------------------------------------------------------------------- Negate

void BaseNode::negate()
{
   emergencyStop("BaseNode::negate", ep);
}

void NumNode::negate()
{
   value = string("-") + value;
}

//---------------------------------------------------------------------- setLHS
//
//void BaseNode::setLHS(Node name)
//{
//   emergencyStop("BaseNode::setLHS", ep);
//}
//
//void QueryNode::setLHS(Node name)
//{
//   lhs = name;
//}

