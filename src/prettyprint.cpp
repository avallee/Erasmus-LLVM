/** \file prettyprint.cpp
* These functions are used to display AST nodes in a style
* that resembles the original source code.
*/

#include "ast.h"
#include "utilities.h"

void ProgramNode::prettyPrint(ostream & os, int indent) const
{
   for (ListIter it = nodes.begin(); it != nodes.end(); ++it)
      (*it)->prettyPrint(os, indent);
}

void InstanceNode::prettyPrint(ostream & os, int indent) const
{
   name->prettyPrint(os);
   os << '(';
   bool moreThanOne = false;
   for (ListIter it = args.begin(); it != args.end(); ++it)
   {
      if (moreThanOne)
         os << ", ";
      moreThanOne = true;
      (*it)->prettyPrint(os, indent + 2);
   }
   os << ')';
}

void RemoveNode::prettyPrint(ostream & os, int indent) const
{}

void ProcessNode::prettyPrint(ostream & os, int indent) const
{
   os << name << " process ";
   bool moreThanOne = false;
   for (ListIter it = params.begin(); it != params.end(); ++it)
   {
      if (moreThanOne)
         os << "; ";
      moreThanOne = true;
      (*it)->prettyPrint(os);
   }
   os << " |";
   seq->prettyPrint(os, indent + 2);
   os << "\nend";
}

void CppNode::prettyPrint(ostream & os, int indent) const
{
   bool moreThanOne = false;
   for (ListIter it = params.begin(); it != params.end(); ++it)
   {
      if (moreThanOne)
         os << "; ";
      moreThanOne = true;
      (*it)->prettyPrint(os);
   }
   if (type)
   {
      os << " -> ";
      type->prettyPrint(os, indent + 2);
      os << "end\n";
   }
}

void ProcedureNode::prettyPrint(ostream & os, int indent) const
{
   os << name << "procedure ";
   bool moreThanOne = false;
   for (ListIter it = params.begin(); it != params.end(); ++it)
   {
      if (moreThanOne)
         os << "; ";
      moreThanOne = true;
      (*it)->prettyPrint(os);
   }
   os << " |";
   seq->prettyPrint(os, indent + 2);
   os << "\nend";
}

void ProtocolNode::prettyPrint(ostream & os, int indent) const
{
   os << "[ ";
   if (expr)
      expr->prettyPrint(os);
   os << " ]";
}

void DefNode::prettyPrint(ostream & os, int indent) const
{
   name->prettyPrint(os);
   os << " = ";
   value->prettyPrint(os, indent + 2);
   os << "\n";
}

void CellNode::prettyPrint(ostream & os, int indent) const
{
   os << "cell ";
   if (params.size() > 0)
   {
      bool moreThanOne = false;
      for (ListIter it = params.begin(); it != params.end(); ++it)
      {
         if (moreThanOne)
            os << "; ";
         moreThanOne = true;
         (*it)->prettyPrint(os, indent + 2);
      }
      os << " | ";
   }
   bool moreThanOne = false;
   for (ListIter it = instances.begin(); it != instances.end(); ++it)
   {
      if (moreThanOne)
         os << "; ";
      moreThanOne = true;
      (*it)->prettyPrint(os, indent + 2);
   }
   os << "\nend";
}

void SequenceNode::prettyPrint(ostream & os, int indent) const
{
   for (ListIter it = stmts.begin(); it != stmts.end(); ++it)
   {
      os << endl << setw(indent) << ' ';
      (*it)->prettyPrint(os, indent);
   }
}

void SkipNode::prettyPrint(ostream & os, int indent) const
{
   os << "skip";
}

void ExitNode::prettyPrint(ostream & os, int indent) const
{
   os << "exit";
}

void IfNode::prettyPrint(ostream  & os, int indent) const
{
   bool moreThanOne = false;
   for (ListIter it = condPairs.begin(); it != condPairs.end(); ++it)
   {
      if (moreThanOne)
         os << "\n" << setw(indent) << ' ' << "else ";
      moreThanOne = true;
      (*it)->prettyPrint(os, indent + 2);
   }
   if (!alt->isEmpty())
   {
      os << "\n" << setw(indent) << ' ' << "else ";
      alt->prettyPrint(os, indent + 2);
   }
}

void CondPairNode::prettyPrint(ostream & os, int indent) const
{
   for (ListIter it = decs.begin(); it != decs.end(); ++it)
      (*it)->prettyPrint(os, indent);
   os << "if ";
   cond->prettyPrint(os);
   os << " then";
   seq->prettyPrint(os, indent + 2);
}

void LoopNode::prettyPrint(ostream & os, int indent) const
{
   os << "loop";
   seq->prettyPrint(os, indent + 2);
   os << endl << setw(indent) << ' ';
}

void ForNode::prettyPrint(ostream & os, int indent) const
{
   os << "for ";
   comp->prettyPrint(os, indent + 2);
   os << " do ";
   seq->prettyPrint(os, indent + 2);
}

void AnyNode::prettyPrint(ostream & os, int indent) const
{
   os << "any ";
   comp->prettyPrint(os, indent + 2);
   os << " do ";
   seq->prettyPrint(os, indent + 2);
   if (!alt->isEmpty())
   {
      os << "\n" << setw(indent) << ' ' << "else ";
      alt->prettyPrint(os, indent + 2);
   }
}

void ComprehensionNode::prettyPrint(ostream & os, int indent) const
{
   var->prettyPrint(os, indent + 2);
   if (type)
   {
      os << " : ";
      type->prettyPrint(os, indent + 2);
   }
   os << " in ";
   collection->prettyPrint(os, indent + 2);
   if (pred)
   {
      os << " such that ";
      pred->prettyPrint(os, indent + 2);
   }
}

void RangeNode::prettyPrint(ostream & os, int indent) const
{
   start->prettyPrint(os, indent + 2);
   os << " to ";
   finish->prettyPrint(os, indent + 2);
   if (step)
   {
      os << " step ";
      step->prettyPrint(os, indent + 2);
   }
}

void EnumSetNode::prettyPrint(ostream & os, int indent) const
{
   type->prettyPrint(os, indent + 2);
}

void RangeInitNode::prettyPrint(ostream & os, int indent) const
{
   start->prettyPrint(os, indent + 2);
   os << " to ";
   finish->prettyPrint(os, indent + 2);
   if (step)
   {
      os << " step ";
      step->prettyPrint(os, indent + 2);
   }
}

void EnumInitNode::prettyPrint(ostream & os, int indent) const
{
   var->prettyPrint(os, indent + 2);
}

void MapInitNode::prettyPrint(ostream & os, int indent) const
{
}

void RangeTermNode::prettyPrint(ostream & os, int indent) const
{
   var->prettyPrint(os, indent + 2);
   if (type)
      type->prettyPrint(os, indent + 2);
   step->prettyPrint(os, indent + 2);
   finish->prettyPrint(os, indent + 2);
}

void EnumTermNode::prettyPrint(ostream & os, int indent) const
{
   var->prettyPrint(os, indent + 2);
   max->prettyPrint(os, indent + 2);
}

void MapTermNode::prettyPrint(ostream & os, int indent) const
{
}

void MatchNode::prettyPrint(ostream & os, int indent) const
{
   pred->prettyPrint(os, indent + 2);
}

void RangeStepNode::prettyPrint(ostream & os, int indent) const
{
   var->prettyPrint(os, indent + 2);
   os << " += ";
   step->prettyPrint(os, indent + 2);
}

void EnumStepNode::prettyPrint(ostream & os, int indent) const
{
   os << "++";
   var->prettyPrint(os, indent + 2);
}

void MapStepNode::prettyPrint(ostream & os, int indent) const
{
}

void MapSetNode::prettyPrint(ostream & os, int indent) const
{
   switch (setKind)
   {
      case MAPSET_DOMAIN:
         os << "domain ";
         break;
      case MAPSET_RANGE:
         os << "range ";
         break;
   }
   map->prettyPrint(os, indent + 2);
}

void SelectNode::prettyPrint(ostream & os, int indent) const
{
   os << "select";
   for (ListIter it = options.begin(); it != options.end(); ++it)
      (*it)->prettyPrint(os, indent + 2);
}

void OptionNode::prettyPrint(ostream & os, int indent) const
{
   os << endl << setw(indent) << ' ';
   os << '|';
   if (guard)
      guard->prettyPrint(os, indent + 2);
   os << '|';
   seq->prettyPrint(os, indent + 2);
}

void SendNode::prettyPrint(ostream & os, int indent) const
{}

void SendOptionNode::prettyPrint(ostream & os, int indent) const
{}

void ReceiveNode::prettyPrint(ostream & os, int indent) const
{}

//void ReceivePrepareNode::prettyPrint(ostream & os, int indent) const
//   {}

void ReceiveOptionNode::prettyPrint(ostream & os, int indent) const
{}

void ConstantNode::prettyPrint(ostream & os, int indent) const
{
   name->prettyPrint(os);
   os << ": ";
   type->prettyPrint(os);
   os << " = ";
   value->prettyPrint(os);
   os << endl;
}

void DecNode::prettyPrint(ostream & os, int indent) const
{
   name->prettyPrint(os);
   if (type)
   {
      os << ": ";
      switch (portDir)
      {
         case CLIENT:
            os << '-';
            break;
         case SERVER:
            os << '+';
            break;
      }
      type->prettyPrint(os);
   }
   if (value)
   {
      os << (constant ? " = " : " := ");
      value->prettyPrint(os);
   }
}

void DotNode::prettyPrint(ostream & os, int indent) const
{
   port->prettyPrint(os);
   os << '.';
   field->prettyPrint(os);
}

void QueryNode::prettyPrint(ostream & os, int indent) const
{
   port->prettyPrint(os);
   os << '?';
   field->prettyPrint(os);
}

void BoolNode::prettyPrint(ostream & os, int indent) const
{
   os << (value ? "true" : "false");
}

void CharNode::prettyPrint(ostream & os, int indent) const
{
   showChar(os, value);
}

void TextNode::prettyPrint(ostream & os, int indent) const
{
   showString(os, value);
}

void NumNode::prettyPrint(ostream & os, int indent) const
{
   os << value;
}

void ListopNode::prettyPrint(ostream & os, int indent) const
{
   os << '(';
   bool moreThanOne = false;
   for (ListIter it = args.begin(); it != args.end(); ++it)
   {
      if (moreThanOne)
         os << opToString(op);
      (*it)->prettyPrint(os);
      moreThanOne = true;
   }
   os << ')';
}

void BinopNode::prettyPrint(ostream & os, int indent) const
{
   os << '(';
   lhs->prettyPrint(os);
   os << ')' + opToString(op) + '(';
   rhs->prettyPrint(os);
   os << ')';
}

void CondExprNode::prettyPrint(ostream & os, int indent) const
{
   lhs->prettyPrint(os);
   os << " if ";
   pred->prettyPrint(os);
   os << " else ";
   rhs->prettyPrint(os);
}

void UnopNode::prettyPrint(ostream & os, int indent) const
{
   os << opToString(op) << '(';
   operand->prettyPrint(os);
   os << ')';
}

void SubscriptNode::prettyPrint(ostream & os, int indent) const
{
   base->prettyPrint(os);
   os << '[';
   sub->prettyPrint(os);
   os << ']';
}

void SubrangeNode::prettyPrint(ostream & os, int indent) const
{
   base->prettyPrint(os);
   os << '[';
   sub1->prettyPrint(os);
   os << "..";
   sub2->prettyPrint(os);
   os << ']';
}

void IteratorNode::prettyPrint(ostream & os, int indent) const
{
   map->prettyPrint(os);
   os << " @ " << iterFunToString(fun);
}

void VoidTypeNode::prettyPrint(ostream & os, int indent) const
{
   os << "void";
}

void BoolTypeNode::prettyPrint(ostream & os, int indent) const
{
   os << "Bool";
}

void ByteTypeNode::prettyPrint(ostream & os, int indent) const
{
   os << "Byte";
}

void FileTypeNode::prettyPrint(ostream & os, int indent) const
{
   switch (mode)
   {
      case FILE_INPUT:
         os << "inputFile";
         break;
      case FILE_OUTPUT:
         os << "outputFile";
         break;
      default:
         os << "???file";
         break;
   }
}

void IntegerTypeNode::prettyPrint(ostream & os, int indent) const
{
   os << "int";
}

void FloatTypeNode::prettyPrint(ostream & os, int indent) const
{
   os << "double";
}

void DecimalTypeNode::prettyPrint(ostream & os, int indent) const
{
   os << "double";
}

void CharTypeNode::prettyPrint(ostream & os, int indent) const
{
   os << "char";
}

void TextTypeNode::prettyPrint(ostream & os, int indent) const
{
   os << "string";
}

void EnumTypeNode::prettyPrint(ostream & os, int indent) const
{
   os << '<';
   for (ListIter it = values.begin(); it != values.end(); ++it)
   {
      os << ' ';
      (*it)->prettyPrint(os);
   }
   os << " >";
}

void EnumValueNode::prettyPrint(ostream & os, int indent) const
{
   os << name;
}

void ArrayTypeNode::prettyPrint(ostream & os, int indent) const
{
   rangeType->prettyPrint(os, indent);
}

void MapTypeNode::prettyPrint(ostream & os, int indent) const
{
   domainType->prettyPrint(os, indent);
   os << " indexes ";
   switch (portDir)
   {
      case CLIENT:
         os << '-';
         break;
      case SERVER:
         os << '+';
         break;
   }
   rangeType->prettyPrint(os, indent);
}

void IterTypeNode::prettyPrint(ostream & os, int indent) const
{
   domainType->prettyPrint(os, indent);
   os << " indexes ";
   rangeType->prettyPrint(os, indent);
}

void FunctionNode::prettyPrint(ostream & os, int indent) const
{
   os << name->getNameString() << '(';
   bool moreThanOne = false;
   for (ListIter it = args.begin(); it != args.end(); ++it)
   {
      if (moreThanOne)
         os << ", ";
      moreThanOne = true;
      (*it)->prettyPrint(os, indent + 2);
   }
   os << ')';
}

void NameNode::prettyPrint(ostream & os, int indent) const
{
   os << value;
}

void ThreadNode::prettyPrint(ostream & os, int indent) const
{
   os << name << " thread ";
   bool moreThanOne = false;
   for (ListIter it = inputs.begin(); it != inputs.end(); ++it)
   {
      if (moreThanOne)
         os << "; ";
      moreThanOne = true;
      (*it)->prettyPrint(os, indent + 2);
   }
   os << " -> ";
   moreThanOne = false;
   for (ListIter it = outputs.begin(); it != outputs.end(); ++it)
   {
      if (moreThanOne)
         os << "; ";
      moreThanOne = true;
      (*it)->prettyPrint(os, indent + 2);
   }
   os << " | ";
   seq->prettyPrint(os, indent + 2);
   os << "\nend";
}

void ThreadParamNode::prettyPrint(ostream & os, int indent) const
{
   name->prettyPrint(os, indent + 2);
   os << ": ";
   type->prettyPrint(os, indent + 2);
}

void StartNode::prettyPrint(ostream & os, int indent) const
{
   os << "start ";
   bool moreThanOne = false;
   for (ListIter it = calls.begin(); it != calls.end(); ++it)
   {
      if (moreThanOne)
         os << "; ";
      moreThanOne = true;
      (*it)->prettyPrint(os, indent + 2);
   }
   os << " do ";
   seq->prettyPrint(os, indent + 2);
   os << "end";
}

void ThreadCallNode::prettyPrint(ostream & os, int indent) const
{
   name->prettyPrint(os, indent + 2);
   os << "(";
   bool moreThanOne = false;
   for (ListIter it = inputs.begin(); it != inputs.end(); ++it)
   {
      if (moreThanOne)
         os << "; ";
      moreThanOne = true;
      (*it)->prettyPrint(os, indent + 2);
   }
   os << " -> ";
   moreThanOne = false;
   for (ListIter it = outputs.begin(); it != outputs.end(); ++it)
   {
      if (moreThanOne)
         os << "; ";
      moreThanOne = true;
      (*it)->prettyPrint(os, indent + 2);
   }
   os << ")";
}

void ThreadStartNode::prettyPrint(ostream & os, int indent) const
{
   name->prettyPrint(os, indent + 2);
}

void ThreadStopNode::prettyPrint(ostream & os, int indent) const
{
   name->prettyPrint(os, indent + 2);
}

