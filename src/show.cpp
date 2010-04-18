/** \file show.cpp
* Functions used to display AST nodes.
*/

#include "ast.h"
#include "utilities.h"

/** Redirect operator<< to appropriate node type.
  * \param os is the output stream.
  * \param p points to the node to be written.
  */
ostream & operator<<(ostream & os, const BaseNode *p)
{
   p->show(os);
   return os;
}

/** Report base node features of node.
  * \param os is the output stream.
  * \param level is the indentation level (depth in the tree).
  */
void BaseNode::showBase(ostream & os, int level) const
{
   os << endl;
   if (ep.lineNum > 0)
      os << setw(3) << ep.lineNum;
   else
      os << "   ";
   os << "  " << setw(level) << nodeNum << ' ' << nodeToString(kv) << ' ';
   if (defChain)
      os << "->" << defChain->nodeNum;
}

void ProgramNode::show(ostream & os, int level) const
{
   showBase(os, level);
   for (ListIter it = nodes.begin(); it != nodes.end(); ++it)
      (*it)->show(os, level + 2);
}

void InstanceNode::show(ostream & os, int level) const
{
   showBase(os, level);
   name->show(os, level + 2);
   for (ListIter it = args.begin(); it != args.end(); ++it)
      (*it)->show(os, level + 2);
}

void RemoveNode::show(ostream & os, int level) const
{
   showBase(os, level);
}

void CppNode::show(ostream & os, int level) const
{
   showBase(os, level);
   for (ListIter it = params.begin(); it != params.end(); ++it)
      (*it)->show(os, level + 2);
   if (type)
      type->show(os, level + 2);
}

void ProcedureNode::show(ostream & os, int level) const
{
   showBase(os, level);
   os << " '" << name << "' " << typeNum;
   for (ListIter it = params.begin(); it != params.end(); ++it)
      (*it)->show(os, level + 2);
   seq->show(os, level + 2);
}

void ProcessNode::show(ostream & os, int level) const
{
   showBase(os, level);
   os << " '" << name << "' " << typeNum;
   for (ListIter it = params.begin(); it != params.end(); ++it)
      (*it)->show(os, level + 2);
   seq->show(os, level + 2);
}

void ProtocolNode::show(ostream & os, int level) const
{
   showBase(os, level);
   os << "nf=" << numFields;
   if (expr)
      expr->show(os, level + 2);
   if (plts)
      os << "\n     " << setw(level) << ' ' << "LTS: " << plts;
}

void DefNode::show(ostream & os, int level) const
{
   showBase(os, level);
   os << ' ' << (incremental ? " += " : " = ");
   name->show(os, level + 2);
   value->show(os, level + 2);
}

void CellNode::show(ostream & os, int level) const
{
   showBase(os, level);
   for (ListIter it = params.begin(); it != params.end(); ++it)
      (*it)->show(os, level + 2);
   for (ListIter it = instances.begin(); it != instances.end(); ++it)
      (*it)->show(os, level + 2);
}

void SequenceNode::show(ostream & os, int level) const
{
   showBase(os, level);
   for (ListIter it = stmts.begin(); it != stmts.end(); ++it)
      (*it)->show(os, level + 2);
}

void SkipNode::show(ostream & os, int level) const
{
   showBase(os, level);
}

void ExitNode::show(ostream & os, int level) const
{
   showBase(os, level);
}

void IfNode::show(ostream  & os, int level) const
{
   showBase(os, level);
   for (ListIter it = condPairs.begin(); it != condPairs.end(); ++it)
      (*it)->show(os, level + 2);
   alt->show(os, level + 2);
}

void CondPairNode::show(ostream & os, int level) const
{
   showBase(os, level);
   for (ListIter it = decs.begin(); it != decs.end(); ++it)
      (*it)->show(os, level + 2);
   cond->show(os, level + 2);
   seq->show(os, level + 2);
}

void LoopNode::show(ostream & os, int level) const
{
   showBase(os, level);
   seq->show(os, level + 2);
}

void ForNode::show(ostream & os, int level) const
{
   showBase(os, level);
   comp->show(os, level + 2);
   seq->show(os, level + 2);
}

void AnyNode::show(ostream & os, int level) const
{
   showBase(os, level);
   comp->show(os, level + 2);
   seq->show(os, level + 2);
   if (alt)
      alt->show(os, level + 2);
}

void ComprehensionNode::show(ostream & os, int level) const
{
   showBase(os, level);
   var->show(os, level + 2);
   if (type)
      type->show(os, level + 2);
   if (iterType)
      iterType->show(os, level + 2);
   if (iter)
      iter->show(os, level + 2);
   collection->show(os, level + 2);
   if (pred)
      pred->show(os, level + 2);
}

void RangeNode::show(ostream & os, int level) const
{
   showBase(os, level);
   os << " f=" << finishNum << " s=" << stepNum;
   start->show(os, level + 2);
   finish->show(os, level + 2);
   if (step)
      step->show(os, level + 2);
}

void EnumSetNode::show(ostream & os, int level) const
{
   showBase(os, level);
   type->show(os, level + 2);
}

void MapSetNode::show(ostream & os, int level) const
{
   showBase(os, level);
   if (iter) iter->show(os, level + 2);
   if (map) map->show(os, level + 2);
   if (type) type->show(os, level + 2);
}

void RangeInitNode::show(ostream & os, int level) const
{
   showBase(os, level);
   var->show(os, level + 2);
   start->show(os, level + 2);
   finish->show(os, level + 2);
   if (step)
      step->show(os, level + 2);
}

void EnumInitNode::show(ostream & os, int level) const
{
   showBase(os, level);
   var->show(os, level + 2);
}

void MapInitNode::show(ostream & os, int level) const
{
   showBase(os, level);
   var->show(os, level + 2);
   iter->show(os, level + 2);
   if (type)
      type->show(os, level + 2);
   map->show(os, level + 2);
}

void RangeTermNode::show(ostream & os, int level) const
{
   os << (open ? " open" : " closed");
   os << (ascending ? " ascending" : " descending");
   showBase(os, level);
   var->show(os, level + 2);
   if (step)
      step->show(os, level + 2);
   finish->show(os, level + 2);
}

void EnumTermNode::show(ostream & os, int level) const
{
   showBase(os, level);
   var->show(os, level + 2);
   max->show(os, level + 2);
}

void MapTermNode::show(ostream & os, int level) const
{
   showBase(os, level);
   var->show(os, level + 2);
   iter->show(os, level + 2);
   if (type)
      type->show(os, level + 2);
   map->show(os, level + 2);
}

void MatchNode::show(ostream & os, int level) const
{
   showBase(os, level);
   if (pred)
      pred->show(os, level + 2);
}

void RangeStepNode::show(ostream & os, int level) const
{
   showBase(os, level);
   os << (ascending ? " ascending" : " descending");
   var->show(os, level + 2);
   if (step)
      step->show(os, level + 2);
}

void EnumStepNode::show(ostream & os, int level) const
{
   showBase(os, level);
   var->show(os, level + 2);
}

void MapStepNode::show(ostream & os, int level) const
{
   showBase(os, level);
   var->show(os, level + 2);
   iter->show(os, level + 2);
   if (type)
      type->show(os, level + 2);
   map->show(os, level + 2);
}

void SelectNode::show(ostream & os, int level) const
{
   showBase(os, level);
   os << policyToString(policy);
   for (ListIter it = options.begin(); it != options.end(); ++it)
      (*it)->show(os, level + 2);
}

void OptionNode::show(ostream & os, int level) const
{
   showBase(os, level);
   if (guard)
      guard->show(os, level + 2);
   seq->show(os, level + 2);
}

void SendNode::show(ostream & os, int level) const
{
   showBase(os, level);
   os << ' ' << tempName << ' ' << bufferName << ' ' << fieldNum;
   if (rhs)
      rhs->show(os, level + 2);
   port->show(os, level + 2);
}

void SendOptionNode::show(ostream & os, int level) const
{
   showBase(os, level);
   os << ' ' << tempName << ' ' << bufferName << ' ' << fieldNum;
   rhs->show(os, level + 2);
   port->show(os, level + 2);
}

void ReceiveNode::show(ostream & os, int level) const
{
   showBase(os, level);
   os << ' ' << bufferName;
   lhs->show(os, level + 2);
   port->show(os, level + 2);
   if (type)
      type->show(os, level + 2);
}

void ReceiveOptionNode::show(ostream & os, int level) const
{
   showBase(os, level);
   os << ' ' << bufferName << ' ' << fieldNum;
   lhs->show(os, level + 2);
   port->show(os, level + 2);
}

void ConstantNode::show(ostream & os, int level) const
{
   showBase(os, level);
   name->show(os, level + 2);
   type->show(os, level + 2);
   value->show(os, level + 2);
}

void DecNode::show(ostream & os, int level) const
{
   showBase(os, level);
   os << " mk=" << messToString(mk);
   os << " portDir=" << portToString(portDir);
   os << " ck=" << commToString(ck);
   if (alias)
      os << " alias";
   if (reference)
      os << " ref";
   if (generated)
      os << " generated";
   if (parameter)
      os << " parameter";
   if (constant)
      os << " = ";
   else if (move)
      os << " <- ";
   else
      os << " := ";
   name->show(os, level + 2);
   if (type)
      type->show(os, level + 2);
   if (value)
      value->show(os, level + 2);
}

void DotNode::show(ostream & os, int level) const
{
   showBase(os, level);
   port->show(os, level + 2);
   field->show(os, level+ 2);
}

void QueryNode::show(ostream & os, int level) const
{
   showBase(os, level);
   port->show(os, level + 2);
   field->show(os, level+ 2);
}

void BoolNode::show(ostream & os, int level) const
{
   showBase(os, level);
   os << ' ' << (value ? "true " : "false ");
}

void CharNode::show(ostream & os, int level) const
{
   showBase(os, level);
   showChar(os, value);
}

void TextNode::show(ostream & os, int level) const
{
   showBase(os, level);
   showString(os, value);
}

void NumNode::show(ostream & os, int level) const
{
   showBase(os, level);
   os << "evm# " << evmBlockNum;
   string typeName;
   if (type == BaseNode::theByteNode)
      typeName = "Byte";
   if (type == BaseNode::theIntegerNode)
      typeName = "Integer";
   else if (type == BaseNode::theFloatNode)
      typeName = "Float";
   else if (type == BaseNode::theDecimalNode)
      typeName = "Decimal";
   else
      typeName = "???";
   os << ' ' << typeName << ' ' << value;
}

void ListopNode::show(ostream & os, int level) const
{
   showBase(os, level);
   os << quoteString(opToString(op));
   for (ListIter it = args.begin(); it != args.end(); ++it)
      (*it)->show(os, level + 2);
}

void BinopNode::show(ostream & os, int level) const
{
   showBase(os, level);
   os << quoteString(opToString(op));
   if (type)
      type->show(os, level + 2);
   else
      os << " no type";
   lhs->show(os, level + 2);
   rhs->show(os, level + 2);
}

void CondExprNode::show(ostream & os, int level) const
{
   showBase(os, level);
   if (type)
      type->show(os, level + 2);
   lhs->show(os, level + 2);
   pred->show(os, level + 2);
   rhs->show(os, level + 2);
}

void UnopNode::show(ostream & os, int level) const
{
   showBase(os, level);
   os << quoteString(opToString(op));
   if (type)
      type->show(os, level + 2);
   else
      os << " no type";
   operand->show(os, level + 2);
}

void SubscriptNode::show(ostream & os, int level) const
{
   showBase(os, level);
   base->show(os, level+2);
   sub->show(os, level+2);
}

void SubrangeNode::show(ostream & os, int level) const
{
   showBase(os, level);
   base->show(os, level+2);
   sub1->show(os, level+2);
   sub2->show(os, level+2);
}

void IteratorNode::show(ostream & os, int level) const
{
   showBase(os, level);
   os << ' ' << iterFunToString(fun);
   map->show(os, level+2);
}

void VoidTypeNode::show(ostream & os, int level) const
{
   showBase(os, level);
}

void BoolTypeNode::show(ostream & os, int level) const
{
   showBase(os, level);
}

void ByteTypeNode::show(ostream & os, int level) const
{
   showBase(os, level);
   os << (unSigned ? "unsigned " : "signed");
}

void FileTypeNode::show(ostream & os, int level) const
{
   showBase(os, level);
   switch (mode)
   {
      case FILE_INPUT:
         os << "input";
         break;
      case FILE_OUTPUT:
         os << "output";
         break;
      default:
         os << "type = " << int(mode);
         break;
   }
}

void IntegerTypeNode::show(ostream & os, int level) const
{
   showBase(os, level);
   os << (unSigned ? "unsigned " : "signed");
}

void FloatTypeNode::show(ostream & os, int level) const
{
   showBase(os, level);
}

void DecimalTypeNode::show(ostream & os, int level) const
{
   showBase(os, level);
}

void CharTypeNode::show(ostream & os, int level) const
{
   showBase(os, level);
}

void TextTypeNode::show(ostream & os, int level) const
{
   showBase(os, level);
}

void EnumTypeNode::show(ostream & os, int level) const
{
   showBase(os, level);
   os << ' ' << name;
   for (ListIter it = values.begin(); it != values.end(); ++it)
      (*it)->show(os, level + 2);
}

void EnumValueNode::show(ostream & os, int level) const
{
   showBase(os, level);
   os << ' ' << name << ' ' << varNum << ' ' << evmBlockNum;
}

void MapTypeNode::show(ostream & os, int level) const
{
   showBase(os, level);
   os << ' ' << portToString(portDir);
   domainType->show(os, level+2);
   rangeType->show(os, level+2);
}

void ArrayTypeNode::show(ostream & os, int level) const
{
   showBase(os, level);
   rangeType->show(os, level+2);
   if (lo)
      lo->show(os, level+2);
   if (hi)
      hi->show(os, level+2);
}

void IterTypeNode::show(ostream & os, int level) const
{
   showBase(os, level);
   domainType->show(os, level+2);
   rangeType->show(os, level+2);
}

void FunctionNode::show(ostream & os, int level) const
{
   showBase(os, level);
   os << ' ' << getFun();
   if (desc)
      os << desc;
   for (ListIter it = args.begin(); it != args.end(); ++it)
      (*it)->show(os, level + 2);
}

void NameNode::show(ostream & os, int level) const
{
   showBase(os, level);
   if (definingOccurrence)
      os << "new";
   if (isPervasive())
      os << " pervasive constant";
   if (passByReference)
      os << " pbr";
   if (isAlias())
      os << " alias";
   os << ' ' << value << ' ' << varNum;
   if (owner.size() > 0)
      os << " (" << owner << ")";
   if (fieldNum >= 0)
      os << " field " << fieldNum;
   if (offset > 0)
      os << " offset " << offset;
   if (definition)
      os << " def=" << definition->num();
   else if (!definingOccurrence)
      os << " ***** undefined *****";
   if (type)
      type->show(os, level + 2);
}

void ThreadNode::show(ostream & os, int level) const
{
   showBase(os, level);
   os << name;
   for (ListIter it = inputs.begin(); it != inputs.end(); ++it)
      (*it)->show(os, level + 2);
   for (ListIter it = outputs.begin(); it != outputs.end(); ++it)
      (*it)->show(os, level + 2);
   seq->show(os, level + 2);
}

void ThreadParamNode::show(ostream & os, int level) const
{
   showBase(os, level);
   os << " (" << owner << ")";
   os << (input ? " input" : " output");
   name->show(os, level + 2);
   type->show(os, level + 2);
}

void StartNode::show(ostream & os, int level) const
{
   showBase(os, level);
   for (ListIter it = calls.begin(); it != calls.end(); ++it)
      (*it)->show(os, level + 2);
   seq->show(os, level + 2);

}

void ThreadCallNode::show(ostream & os, int level) const
{
   showBase(os, level);
   name->show(os, level + 2);
   for (ListIter it = inputs.begin(); it != inputs.end(); ++it)
      (*it)->show(os, level + 2);
   for (ListIter it = outputs.begin(); it != outputs.end(); ++it)
      (*it)->show(os, level + 2);
}

void ThreadStartNode::show(ostream & os, int level) const
{
   showBase(os, level);
   name->show(os, level + 2);
}

void ThreadStopNode::show(ostream & os, int level) const
{
   showBase(os, level);
   name->show(os, level + 2);
}

//------------------------------------------------------------------------showBB

void BaseNode::showBB(ostream & os) const
{}

void ProgramNode::showBB(ostream & os) const
{
   for (ListIter it = nodes.begin(); it != nodes.end(); ++it)
      (*it)->showBB(os);
}

void DefNode::showBB(ostream & os) const
{
   value->showBB(os);
}

void ProcedureNode::showBB(ostream & os) const
{
   for (BlockList::const_iterator it = blocks.begin(); it != blocks.end(); ++it)
      (*it)->showBB(os);
}

void ProcessNode::showBB(ostream & os) const
{
   for (BlockList::const_iterator it = blocks.begin(); it != blocks.end(); ++it)
      (*it)->showBB(os);
}
