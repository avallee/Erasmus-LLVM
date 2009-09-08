/** \file bind.cpp
 *
 * These functions implement the "binding" pass of the compiler,
 * during which names are linked to their definitions.
 */

#include "ast.h"

#include<cassert>

using namespace std;

/** Set \a defChain to to the name list for this node.
 *  The \a defChain is a linked list containing nodes that might contain
 *  names requiring definition or that might contain definitions.
 *  Links are constructed to match scoping rules.
 *  The default action is to do nothing.
 *  Nodes with subtrees invoke this function recursively.
 *  Definition nodes pass on a pointer to themselves.
 *  Name nodes use the pointer to find the matching definition.
 *  \param p points to the definition list for this node to use.
 */
void BaseNode::bind(Node p)
{}

void BaseNode::tryBind(Node p)
{}

void ProgramNode::bind(Node p)
{
   defChain = p;
   for (ListIter it = nodes.begin(); it != nodes.end(); ++it)
   {
      (*it)->bind(p);
      p = &(**it);
   }
}

void InstanceNode::bind(Node p)
{
   defChain = p;
   name->bind(p);
   for (ListIter it = args.begin(); it != args.end(); ++it)
      (*it)->bind(p);
}

void CppNode::bind(Node p)
{
   for (ListIter it = params.begin(); it != params.end(); ++it)
   {
      (*it)->bind(p);
      p = &(**it);
   }
   if (type)
      type->bind(p);
}

void ProcedureNode::bind(Node p)
{
   for (ListIter it = params.begin(); it != params.end(); ++it)
   {
      (*it)->bind(p);
      p = &(**it);
   }
   seq->bind(p);
}

void ProcessNode::bind(Node p)
{
   for (ListIter it = params.begin(); it != params.end(); ++it)
   {
      (*it)->bind(p);
      p = &(**it);
   }
   seq->bind(p);
}

void ProtocolNode::bind(Node p)
{
   if (expr)
      expr->bind(p);
}

/** \todo Postpone setting name in value node. */
void DefNode::bind(Node p)
{
   defChain = p;
   name->bind(this);
   value->bind(this);

   // A cell or closure node contains its own name, set here.
   // Perhaps the name should be set later, when the full name is known.
   value->setName(name->getNameString());
}

void CellNode::bind(Node p)
{
   for (ListIter it = params.begin(); it != params.end(); ++it)
   {
      (*it)->bind(p);
      p = &(**it);
   }
   for (ListIter it = instances.begin(); it != instances.end(); ++it)
   {
      (*it)->bind(p);
      p = &(**it);
   }
}

void SequenceNode::bind(Node p)
{
   // A sequence node must be on the definition chain
   // because it introduces a scope.
   defChain = p;
   p = this;

   for (ListIter it = stmts.begin(); it != stmts.end(); ++it)
   {
      (*it)->bind(p);
      p = &(**it);
   }
}

void ExitNode::bind(Node p)
{
   defChain = p;
}

void IfNode::bind(Node p)
{
   defChain = p;
   for (ListIter it = condPairs.begin(); it != condPairs.end(); ++it)
      (*it)->bind(p);
   alt->bind(p);
}

void CondPairNode::bind(Node p)
{
   defChain = p; // 090227
   p = this;
   for (ListIter it = decs.begin(); it != decs.end(); ++it)
   {
      (*it)->bind(p);
      p = &(**it);
   }

   cond->bind(p);
   seq->bind(p);
}

void LoopNode::bind(Node p)
{
   defChain = p;
   seq->bind(p);
}

void ForNode::bind(Node p)
{
   defChain = p;
   comp->bind(p);
   seq->bind(comp);
}

void AnyNode::bind(Node p)
{
   defChain = p;
   comp->bind(p);
   seq->bind(comp);
   if (!alt->isEmpty())
      alt->bind(p);
}

void ComprehensionNode::bind(Node p)
{
   defChain = p;
   var->bind(this);
   p = this;
   if (type)
      type->bind(p);
   collection->bind(p);
   if (pred)
      pred->bind(p);
}

void RangeNode::bind(Node p)
{
   defChain = p;
   start->bind(p);
   finish->bind(p);
   if (step)
      step->bind(p);
}

void EnumSetNode::bind(Node p)
{
   defChain = p;
   type->bind(p);
}

void MapSetNode::bind(Node p)
{
   defChain = p;
   map->bind(p);
}

void SelectNode::bind(Node p)
{
   defChain = p;
   for (ListIter it = options.begin(); it != options.end(); ++it)
      (*it)->bind(p);
}

void OptionNode::bind(Node p)
{
   defChain = p;
   if (guard)
      guard->bind(p);
   seq->bind(p);
}

void SkipNode::bind(Node p)
{
   defChain = p;
}

void DotNode::bind(Node p)
{
   port->bind(p);
   Node portDec = port->getDefinition();
   if (portDec == 0)
      Error() << "Port '" << port->getNameString() <<
      "' is either undefined or cannot be used in this context." << ep << REPORT;
   else if (portDec->isPort())
      field->bind(portDec->getProtocol());
   else
      Error() << "Invalid port." << ep << REPORT;
}

void QueryNode::bind(Node p)
{
   port->bind(p);
   Node portDec = port->getDefinition();
   if (portDec == 0)
      Error() << "Port '" << port->getNameString() <<
      "' is either undefined or cannot be used in this context." << ep << REPORT;
   else if (portDec->isPort())
      field->bind(portDec->getProtocol());
   else
      Error() << "Invalid port." << ep << REPORT;
}

void ListopNode::bind(Node p)
{
   for (ListIter it = args.begin(); it != args.end(); ++it)
      (*it)->bind(p);
}

void BinopNode::bind(Node p)
{
   defChain = p; // 090701
   lhs->bind(p);
   rhs->bind(p);
}

void CondExprNode::bind(Node p)
{
   lhs->bind(p);
   pred->bind(p);
   rhs->bind(p);
}

void UnopNode::bind(Node p)
{
   operand->bind(p);
}

void SubscriptNode::bind(Node p)
{
   base->bind(p);
   sub->bind(p);
}

void SubrangeNode::bind(Node p)
{
   base->bind(p);
   sub1->bind(p);
   sub2->bind(p);
}

void IteratorNode::bind(Node p)
{
   defChain = p;
   map->bind(p);
}

void ConstantNode::bind(Node p)
{
   defChain = p;
   assert(name->kind() == NAME_NODE);
   name->bind(this);
   type->bind(p);
   value->bind(p);
}

void DecNode::bind(Node p)
{
   defChain = p;
   if (type)
      name->bind(this);
   else
      name->bind(p);
   if (type)
      type->bind(p);
   if (value)
      value->bind(p);
}

void EnumTypeNode::bind(Node p)
{
   defChain = p;
   for (ListIter it = values.begin(); it != values.end(); ++it)
      (*it)->bind(this);
}

void EnumValueNode::bind(Node p)
{
   defChain = p;
}

void ArrayTypeNode::bind(Node p)
{
   rangeType->bind(p);
   lo->bind(p);
   hi->bind(p);
}


void MapTypeNode::bind(Node p)
{
   domainType->bind(p);
   rangeType->bind(p);
}

void FunctionNode::bind(Node p)
{
   defChain = p;

   // Try to bind the name, but no error if undefined. */
   name->tryBind(p);

   for (ListIter it = args.begin(); it != args.end(); ++it)
      (*it)->bind(p);
}

void NameNode::bind(Node p)
{
   // Don't bind a defining occurrence
   if (definingOccurrence)
      return;

   // Look for the name in definitions on the defChain.
   defChain = p;
   Node q = 0;
   while (true)
   {
      if (p == 0)
         // Name undefined: throw an exception to avoid messed-up AST.
         Error() << " '" << value << "' is either not defined or cannot be used in this context." << ep << THROW;
      q = p->lookUp(value, ep);
      if (q)
      {
         definition = q; // Bind name to its definition.
         break;
      }
      else
         p = p->getDefChain();
   }
}

void NameNode::tryBind(Node p)
{
   // Don't bind a defining occurrence
   if (definingOccurrence)
      return;

   // Look for the name in definitions on the defChain.
   defChain = p;
   Node q = 0;
   while (true)
   {
      if (p == 0)
         // Name undefined but we don't care.
         return;
      q = p->lookUp(value, ep);
      if (q)
      {
         definition = q; // Bind name to its definition.
         break;
      }
      else
         p = p->getDefChain();
   }
}

void ThreadNode::bind(Node p)
{
   defChain = p;
   for (ListIter it = inputs.begin(); it != inputs.end(); ++it)
   {
      (*it)->bind(p);
      p = &(**it);
   }
   for (ListIter it = outputs.begin(); it != outputs.end(); ++it)
   {
      (*it)->bind(p);
      p = &(**it);
   }
   seq->bind(p);
}

void ThreadParamNode::bind(Node p)
{
   defChain = p;
   name->bind(this);
   type->bind(p);
}

void StartNode::bind(Node p)
{
   defChain = p;
   for (ListIter it = calls.begin(); it != calls.end(); ++it)
      (*it)->bind(p);
   seq->bind(p);
}

void ThreadCallNode::bind(Node p)
{
   name->bind(p);
   for (ListIter it = inputs.begin(); it != inputs.end(); ++it)
      (*it)->bind(p);
   for (ListIter it = outputs.begin(); it != outputs.end(); ++it)
      (*it)->bind(p);
}


