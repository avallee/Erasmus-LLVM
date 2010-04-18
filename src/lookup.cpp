/** \file lookup.cpp */

#include "ast.h"

/** If the node has a name with string value, return a pointer to it.
  * This function works differently for variables names defined in
  * a definition or declaration (which consists of a name/value pair)
  * and protocols (where the field declaration may be nested in an
  * expression).
  */
Node BaseNode::lookUp(string value, Errpos ep)
{
   showDebug("Sorry!  Compiler error in lookUp.");
   Error() << "'" << value << "' undefined in this context." << ep << REPORT;
}

Node InstanceNode::lookUp(string value, Errpos ep)
{
   return defChain->lookUp(value, ep);
}

Node ProtocolNode::lookUp(string value, Errpos ep)
{
   return expr->lookUp(value, ep);
}

Node DefNode::lookUp(string str, Errpos ep)
{
   if (name->getNameString() == str)
      return this;

   // The next step is needed for definitions that contain names,
   // e.g. enumerations
   else if (value->kind() == ENUM_TYPE_NODE)
      return value->lookUp(str, ep);
   else
      return 0;
}

Node SequenceNode::lookUp(string value, Errpos ep)
{
   assert(defChain != this);
   return defChain->lookUp(value, ep);
}

Node ExitNode::lookUp(string value, Errpos ep)
{
   return defChain->lookUp(value, ep);
}

Node IfNode::lookUp(string value, Errpos ep)
{
   return defChain->lookUp(value, ep);
}

Node CondPairNode::lookUp(string value, Errpos ep)
{
   return defChain->lookUp(value, ep);
}

Node LoopNode::lookUp(string value, Errpos ep)
{
   return defChain->lookUp(value, ep);
}

Node ForNode::lookUp(string value, Errpos ep)
{
      return defChain->lookUp(value, ep);
}

Node AnyNode::lookUp(string value, Errpos ep)
{
      return defChain->lookUp(value, ep);
}

Node ComprehensionNode::lookUp(string value, Errpos ep)
{
   if (var->getNameString() == value)
      return this;
   else
      return defChain->lookUp(value, ep);
}

Node RangeNode::lookUp(string value, Errpos ep)
{
   return defChain->lookUp(value, ep);
}

Node EnumSetNode::lookUp(string value, Errpos ep)
{
   return defChain->lookUp(value, ep);
}

Node MapSetNode::lookUp(string value, Errpos ep)
{
   return defChain->lookUp(value, ep);
}

Node SelectNode::lookUp(string value, Errpos ep)
{
   return defChain->lookUp(value, ep);
}

Node FunctionNode::lookUp(string value, Errpos ep)
{
   return defChain->lookUp(value, ep);
}

Node ListopNode::lookUp(string value, Errpos ep)
{
   for (ListIter it = args.begin(); it != args.end(); ++it)
   {
      Node lm = (*it)->lookUp(value, ep);
      if (lm)
         return lm;
   }
   return 0;
}

Node SkipNode::lookUp(string value, Errpos ep)
{
   return defChain->lookUp(value, ep);
}

Node BinopNode::lookUp(string value, Errpos ep)
{
   return defChain->lookUp(value, ep); // 090701 Required because a &= e is a statement.
//   Node lm = lhs->lookUp(value, ep);
//   return lm ? lm :  rhs->lookUp(value, ep);
}

Node CondExprNode::lookUp(string value, Errpos ep)
{
   return 0;
}

Node SubscriptNode::lookUp(string value, Errpos ep)
{
   return 0;
}

Node SubrangeNode::lookUp(string value, Errpos ep)
{
   return 0;
}

Node EnumTypeNode::lookUp(string value, Errpos ep)
{
   for (ListIter it = values.begin(); it != values.end(); ++it)
   {
      Node p = (*it)->lookUp(value, ep);
      if (p)
         return p;
   }
   return 0;
}

Node EnumValueNode::lookUp(string value, Errpos ep)
{
   return (value == name) ? this : 0;
}

Node IteratorNode::lookUp(string value, Errpos ep)
{
   return 0;
}

Node UnopNode::lookUp(string value, Errpos ep)
{
   return operand->lookUp(value, ep);
}

Node ConstantNode::lookUp(string value, Errpos ep)
{
   return (name->getNameString() == value) ? this : 0;
}

Node DecNode::lookUp(string value, Errpos ep)
{
   return (
             (type || mk == QUERY || mk == REPLY || generated) &&
             name->getNameString() == value) ? this : 0;
}

Node ThreadParamNode::lookUp(string value, Errpos ep)
{
   return name->getNameString() == value ? this : 0;
}

Node StartNode::lookUp(string value, Errpos ep)
{
   return defChain->lookUp(value, ep);
}

