/** \file genassem.cpp
  * Generate calls to Lightning interface
  */

#include "ast.h"
#include "assembler.h"
#include "functions.h"

#include <vector>

using namespace std;

/** The address of the first local variable of a process.
  * Allows for saving 3 registers (12 bytes) and 2 queue pointers (8 bytes).
  */
const int VAR_BASE = -20;

/** This global variable is used to set the address of a process variable
  * relative to the base address of the process.
  */
int offset;

/** This gloabal variable is used to determine whether a name occurs in
  * a left context.
  * \todo Eliminate global lvalue: data shold be in the node.
  */
bool lvalue = false;

/** Translate MEC condition codes to MEL condition codes.
  * An unfortunate shim that reflects the historical development of MEC/MEL.
  */
int transOps(Operator op)
{
   switch (op)
   {
      case BINOP_EQ:
         return COND_EQ;
      case BINOP_NE:
         return COND_NE;
      case BINOP_LT:
         return COND_LT;
      case BINOP_LE:
         return COND_LE;
      case BINOP_GT:
         return COND_GT;
      case BINOP_GE:
         return COND_GE;
      case BINOP_AND:
         return BOOL_AND;
      case BINOP_OR:
         return BOOL_OR;
      case BINOP_PLUS:
         return ARI_PLUS;
      case BINOP_MINUS:
         return ARI_MINUS;
      case BINOP_MULTIPLY:
         return ARI_MULTIPLY;
      case BINOP_DIVIDE:
         return ARI_DIVIDE;
      case BINOP_MOD:
         return ARI_MOD;
      case BINOP_EXTEND:
         return BIN_EXTEND;
      case BINOP_CAT:
         return BIN_CAT;
      default:
         Error() << "Unexpected op code\n" << REPORT;
         return 0;
   }
}

int complement(int op)
{
   switch (op)
   {
      case COND_NE:
         return COND_EQ;
      case COND_EQ:
         return COND_NE;
      case COND_GE:
         return COND_LT;
      case COND_GT:
         return COND_LE;
      case COND_LE:
         return COND_GT;
      case COND_LT:
         return COND_GE;
      default:
         Error() << "Unexpected op code\n" << REPORT;
         return 0;
   }
}

void pushExpr(Node expr)
{
   AssemData aData;
   aData.lvalue = false;
   expr->genAssem();
}

/** Store branch targets at the address in \a keys.
  * This function must be called at the target of the jumps.
  */
void patch(Patches keys)
{
   cerr << "Patching at " << getLabel() << ":";
   for (Patches::iterator i = keys.begin(); i != keys.end(); ++i)
   {
      setLabel(*i);
      cerr << ' ' << *i;
   }
   cerr << endl;
}

/** Branch forward if \a expr is true.
  * \param expr does not contain \a and or \a or.
  * \param keys indexes branch instructions to be patched.
  */
void simpleBranchTrue(Node expr, Patches & keys)
{
   if (expr->kind() == NAME_NODE)
   {
      // Must be a Boolean variable
      push(TYPE_BOOL, expr->getOffset());
      keys.push_back(branchForward(TYPE_BOOL, COND_NE));
   }
   else if (expr->kind() == UNOP_NODE)
   {
      if (expr->getOp() == UNOP_NOT)
         simpleBranchFalse(expr->getArg(), keys);
      else
         Error() << "Operator illegal in context" << expr->getPos() << REPORT;
   }
   else if (expr->kind() == BINOP_NODE)
   {
      pushExpr(expr->getLHS());
      pushExpr(expr->getRHS());
      keys.push_back(branchForward(expr->getTypeCode(), transOps(expr->getOp())));
   }
}

/** Branch forward if \a expr is false.
  * \param expr does not contain \a and or \a or.
  * \param keys indexes branch instructions to be patched.
  */
void simpleBranchFalse(Node expr, Patches & keys)
{
   if (expr->kind() == NAME_NODE)
   {
      // Must be a Boolean variable
      push(TYPE_BOOL, expr->getOffset());
      keys.push_back(branchForward(TYPE_BOOL, COND_EQ));
   }
   else if (expr->kind() == UNOP_NODE)
   {
      if (expr->getOp() == UNOP_NOT)
         simpleBranchTrue(expr->getArg(), keys);
      else
         Error() << "Operator illegal in contenct" << expr->getPos() << REPORT;
   }
   else if (expr->kind() == BINOP_NODE)
  {
      pushExpr(expr->getLHS());
      pushExpr(expr->getRHS());
      int op = complement(transOps(expr->getOp()));
      keys.push_back(branchForward(expr->getTypeCode(), op));
   }
}

/** Branch forward if \a expr is true.
  * \a expr may containg \a and and \a or subexpressions.
  * \param keys indexes branch instructions to be patched.
  */
void branchTrue(Node expr, Patches & keys)
{
   if (expr->kind() == BINOP_NODE)
   {
      if (expr->getOp() == BINOP_AND)
      {
         Patches localKeys;
         branchFalse(expr->getLHS(), localKeys);
         branchTrue(expr->getRHS(), keys);
         patch(localKeys);
      }
      else if (expr->getOp() == BINOP_OR)
      {
         branchTrue(expr->getLHS(), keys);
         branchTrue(expr->getRHS(), keys);
      }
      else
         simpleBranchTrue(expr, keys);
   }
   else
      simpleBranchTrue(expr, keys);
}

/** Branch forwrd if \a expr is false.
  * \a expr may containg \a and and \a or subexpressions.
  * \param keys indexes branch instructions to be patched.
  */
void branchFalse(Node expr, Patches & keys)
{
   if (expr->kind() == BINOP_NODE)
   {
      if (expr->getOp() == BINOP_AND)
      {
         branchFalse(expr->getLHS(), keys);
         branchFalse(expr->getRHS(), keys);
      }
      else if (expr->getOp() == BINOP_OR)
      {
         Patches localKeys;
         branchTrue(expr->getLHS(), localKeys);
         branchFalse(expr->getRHS(), keys);
         patch(localKeys);
      }
      else
         simpleBranchFalse(expr, keys);
   }
   else
      simpleBranchFalse(expr, keys);
}

/** Generate a literal with the defaul value of the given type. */
void genDefault(int typeCode)
{
   switch (typeCode)
   {
      case TYPE_BOOL:
      case TYPE_BYTE:
      case TYPE_UNS_BYTE:
      case TYPE_INT:
      case TYPE_UNS_INT:
      case TYPE_DEC:
      case TYPE_FLO:
         literal(typeCode, "0");
         break;
      case TYPE_CHAR:
         literal(TYPE_CHAR, " ");
         break;
      case TYPE_TEXT:
      case TYPE_ARR:
      case TYPE_MAP:
      default:
         Error() << "genDefault type code " << typeCode << "\n" << REPORT;
         break;
   }
}

/** Generate code for the expression \a expr.
  * \todo Generate additional code to check that the value of the expression
  * conforms to the type \a typeCode.
  */
void rangeCheck(Node expr, int typeCode)
{
   expr->genAssem();
}


// Member functions


void BaseNode::prepAssem(AssemData aData)
{
   emergencyStop("prepAssem", ep);
}

void BaseNode::genAssem()
{
   emergencyStop("genAssem", ep);
}

void ProgramNode::prepAssem(AssemData aData)
{
   for (ListIter it = nodes.begin(); it != nodes.end(); ++it)
      (*it)->prepAssem(aData);
}

void ProgramNode::genAssem()
{
   cout << "Code generation\n";
   prolog();
   for (ListIter it = nodes.begin(); it != nodes.end(); ++it)
      (*it)->genAssem();
   epilog();
   showCode();
}

void DefNode::prepAssem(AssemData aData)
{
   name->prepAssem(aData);
   value->prepAssem(aData);
}

void DefNode::genAssem()
{
   name->genAssem();
   value->genAssem();
}

void CellNode::prepAssem(AssemData aData)
{
   for (ListIter it = params.begin(); it != params.end(); ++it)
      (*it)->prepAssem(aData);
   for (ListIter it = instances.begin(); it != instances.end(); ++it)
      (*it)->prepAssem(aData);
}

void CellNode::genAssem()
{
   for (ListIter it = params.begin(); it != params.end(); ++it)
      (*it)->genAssem();
   for (ListIter it = instances.begin(); it != instances.end(); ++it)
      (*it)->genAssem();
}

void ProcessNode::prepAssem(AssemData aData)
{
   offset = VAR_BASE;
   for (ListIter it = params.begin(); it != params.end(); ++it)
      (*it)->prepAssem(aData);
   seq->prepAssem(aData);
}

void ProcessNode::genAssem()
{
   void *start = getLabel();
   for (ListIter it = params.begin(); it != params.end(); ++it)
      (*it)->genAssem();
   seq->genAssem();
   void *end = getLabel();
   cerr <<
   "Process " << name <<
   " from " << start <<
   " to " << end <<
   " (" << int(end)-int(start) << " bytes).\n";
}

void InstanceNode::prepAssem(AssemData aData)
{
   for (ListIter it = args.begin(); it != args.end(); ++it)
      (*it)->prepAssem(aData);
}

void InstanceNode::genAssem()
{
   for (ListIter it = args.begin(); it != args.end(); ++it)
      (*it)->genAssem();
}

void SequenceNode::prepAssem(AssemData aData)
{
   for (ListIter it = stmts.begin(); it != stmts.end(); ++it)
      (*it)->prepAssem(aData);
}

void SequenceNode::genAssem()
{
   for (ListIter it = stmts.begin(); it != stmts.end(); ++it)
      (*it)->genAssem();
}

void DecNode::prepAssem(AssemData aData)
{
   name->prepAssem(aData);
   if (value)
      value->prepAssem(aData);
   if (type)
   {
      name->setOffset(offset);
      cerr << left << setw(12) << name->getNameString() <<
      right << setw(3) << type->getSize() <<
      hex << setw(10) << offset << endl;
      offset -= type->getSize();
      /** \todo Improve alignment code. */
      while (offset % 4 != 0)
         --offset;
   }
}

void DecNode::genAssem()
{
   // Todo: use a 'move' when RHS is a literal.
   int typeCode = getTypeCode();
   if (value)
      value->genAssem();
   else
      genDefault(typeCode);
   lvalue = true;
   name->genAssem();
   lvalue = false;
}

void BoolNode::prepAssem(AssemData aData)
{
}

void BoolNode::genAssem()
{
   literal(TYPE_BOOL, value ? "1" : "0");
}

void CharNode::prepAssem(AssemData aData)
{
}

void CharNode::genAssem()
{
   literal(TYPE_CHAR, string(1, value).c_str());
}

void NumNode::prepAssem(AssemData aData)
{
}

void NumNode::genAssem()
{
   literal(getTypeCode(), value.c_str());
}

void BinopNode::prepAssem(AssemData aData)
{
   lhs->prepAssem(aData);
   rhs->prepAssem(aData);
}

void BinopNode::genAssem()
{
   lhs->genAssem();
   rhs->genAssem();
   assBinop(getTypeCode(), transOps(getOp()));
}

void FunctionNode::prepAssem(AssemData aData)
{
   for (ListIter it = args.begin(); it != args.end(); ++it)
      (*it)->prepAssem(aData);
}

void FunctionNode::genAssem()
{
   switch (desc->getCode())
   {
         // Functions that require no action.
      case NO_OP:
      case O2I:
      case C2I:
      case UO2UI:
         args[0]->genAssem();
         break;

         // Assertsions
      case ASSERT_1:
         break;
      case ASSERT_2:
         break;

         // Functions returning Boolean
      case T2B:
         genDefault(TYPE_BOOL);
         break;

         // Functions returning Char
      case ENCODE:
      case T2C:
         genDefault(TYPE_CHAR);
         break;

         // Functions returning Byte
      case UO2O:
      case I2O:
      case UI2O:
         rangeCheck(args[0], TYPE_BYTE);
         break;

         // Functions returning unsigned Byte
      case O2UO:
      case I2UO:
      case UI2UO:
         rangeCheck(args[0], TYPE_UNS_BYTE);
         break;

         // Functions returning Integer
      case UI2I:
      case UO2I:
         rangeCheck(args[0], TYPE_INT);
         break;

      case DECODE:
      case T2I:
      case FLOAT_FLOOR:
      case FLOAT_ROUND:
      case FLOAT_CEILING:
      case DEC_FLOOR:
      case DEC_ROUND:
      case DEC_CEILING:
      case LENT:
      case LENA:
      case RND:
         genDefault(TYPE_INT);
         break;

         // Functions returning unsigned Integer
      case O2UI:
      case I2UI:
         rangeCheck(args[0], TYPE_UNS_INT);
         break;

         // Functions returning Float
      case I2F:
      case UI2F:
      case O2F:
      case UO2F:
      case D2F:
      case T2F:
         genDefault(TYPE_FLO);
         break;

         // Functions returning Decimal
      case I2D:
      case UI2D:
      case O2D:
      case UO2D:
      case F2D:
      case T2D:
      case B2T:
         genDefault(TYPE_DEC);
         break;

         // Functions returning Text
      case UO2T:
      case O2T:
      case I2T:
      case UI2T:
      case D2T:
      case F2T:
      case C2T:
      case FMT_BW:
      case FMT_IW:
      case FMT_UIW:
      case FMT_DW:
      case FMT_FW:
      case FMT_CW:
      case FMT_TW:
      case FMT_DW_P:
      case FMT_FW_P:
         genDefault(TYPE_TEXT);
         break;

         // File functions
      case FRO:
         break;
      case FWO:
         break;
      case FRC:
         break;
      case FWC:
         break;
      case FROK:
         break;
      case FWOK:
         break;
      case FREOF:
         break;
      case FREAD:
         break;
      case FWRITE:
         break;

      default:
         break;
   }
}

void NameNode::prepAssem(AssemData aData)
{
}

void NameNode::genAssem()
{
   int offset = getOffset();
   int typeCode = getTypeCode();

   // Temporary kludge: assume variable => offset > 0
   if (offset == 0)
      return;
   if (lvalue)
      pop(typeCode, offset);
   else
      push(typeCode, offset);
}

void IfNode::prepAssem(AssemData aData)
{
   aData.ifParent = this;
   for (ListIter it = condPairs.begin(); it != condPairs.end(); ++it)
      (*it)->prepAssem(aData);
   alt->prepAssem(aData);
}

void IfNode::genAssem()
{
   for (ListIter it = condPairs.begin(); it != condPairs.end(); ++it)
      (*it)->genAssem();
   alt->genAssem();
   patch(jumps);
}

void CondPairNode::prepAssem(AssemData aData)
{
   parent = aData.ifParent;
   cond->prepAssem(aData);
   seq->prepAssem(aData);
}

void CondPairNode::genAssem()
{
   Patches next;
   branchFalse(cond, next);
   seq->genAssem();
   Patches end;
   end.push_back(jumpForward());
   parent->addJumps(end);
   patch(next);
}

void LoopNode::prepAssem(AssemData aData)
{
   aData.loopParent = this;
   seq->prepAssem(aData);
}

void LoopNode::genAssem()
{
   void *start = getLabel();
   cerr << "Loop " << hex << start << endl;
   seq->genAssem();
   jumpBackward(start);
   patch(jumps);
}

void ExitNode::prepAssem(AssemData aData)
{
   parent = aData.loopParent;
}

void ExitNode::genAssem()
{
   Patches end;
   end.push_back(jumpForward());
   parent->addJumps(end);
}

void UnopNode::prepAssem(AssemData aData)
{
}

void UnopNode::genAssem()
{
   emergencyStop("genAssem", ep);
}








void MapTypeNode::prepAssem(AssemData aData)
{
   emergencyStop("prepAssem", ep);
}

void MapTypeNode::genAssem()
{
   emergencyStop("genAssem", ep);
}

void ArrayTypeNode::prepAssem(AssemData aData)
{
   emergencyStop("prepAssem", ep);
}

void ArrayTypeNode::genAssem()
{
   emergencyStop("genAssem", ep);
}

void ConstantNode::prepAssem(AssemData aData)
{
   emergencyStop("prepAssem", ep);
}

void ConstantNode::genAssem()
{
   emergencyStop("genAssem", ep);
}

void IteratorNode::prepAssem(AssemData aData)
{
   emergencyStop("prepAssem", ep);
}

void IteratorNode::genAssem()
{
   emergencyStop("genAssem", ep);
}

void SubrangeNode::prepAssem(AssemData aData)
{
   emergencyStop("prepAssem", ep);
}

void SubrangeNode::genAssem()
{
   emergencyStop("genAssem", ep);
}

void SubscriptNode::prepAssem(AssemData aData)
{
   emergencyStop("prepAssem", ep);
}

void SubscriptNode::genAssem()
{
   emergencyStop("genAssem", ep);
}

void CondExprNode::prepAssem(AssemData aData)
{
   emergencyStop("prepAssem", ep);
}

void CondExprNode::genAssem()
{
   emergencyStop("genAssem", ep);
}

void ListopNode::prepAssem(AssemData aData)
{
   emergencyStop("prepAssem", ep);
}

void ListopNode::genAssem()
{
   emergencyStop("genAssem", ep);
}

void QueryNode::prepAssem(AssemData aData)
{
   emergencyStop("prepAssem", ep);
}

void QueryNode::genAssem()
{
   emergencyStop("genAssem", ep);
}

void DotNode::prepAssem(AssemData aData)
{
   emergencyStop("prepAssem", ep);
}

void DotNode::genAssem()
{
   emergencyStop("genAssem", ep);
}

void SkipNode::prepAssem(AssemData aData)
{
   emergencyStop("prepAssem", ep);
}

void SkipNode::genAssem()
{
   emergencyStop("genAssem", ep);
}

void OptionNode::prepAssem(AssemData aData)
{
   emergencyStop("prepAssem", ep);
}

void OptionNode::genAssem()
{
   emergencyStop("genAssem", ep);
}

void SelectNode::prepAssem(AssemData aData)
{
   emergencyStop("prepAssem", ep);
}

void SelectNode::genAssem()
{
   emergencyStop("genAssem", ep);
}

void ProtocolNode::prepAssem(AssemData aData)
{
   emergencyStop("prepAssem", ep);
}

void ProtocolNode::genAssem()
{
   emergencyStop("genAssem", ep);
}

void ProcedureNode::prepAssem(AssemData aData)
{
   emergencyStop("prepAssem", ep);
}

void ProcedureNode::genAssem()
{
   emergencyStop("genAssem", ep);
}

void CppNode::prepAssem(AssemData aData)
{
   emergencyStop("prepAssem", ep);
}

void CppNode::genAssem()
{
   emergencyStop("genAssem", ep);
}

void TextNode::prepAssem(AssemData aData)
{
   emergencyStop("prepAssem", ep);
}

void TextNode::genAssem()
{
   emergencyStop("genAssem", ep);
}

void MapSetNode::prepAssem(AssemData aData)
{
   emergencyStop("prepAssem", ep);
}

void MapSetNode::genAssem()
{
   emergencyStop("genAssem", ep);
}

void EnumSetNode::prepAssem(AssemData aData)
{
   emergencyStop("prepAssem", ep);
}

void EnumSetNode::genAssem()
{
   emergencyStop("genAssem", ep);
}

void RangeNode::prepAssem(AssemData aData)
{
   emergencyStop("prepAssem", ep);
}

void RangeNode::genAssem()
{
   emergencyStop("genAssem", ep);
}

void ComprehensionNode::prepAssem(AssemData aData)
{
   emergencyStop("prepAssem", ep);
}

void ComprehensionNode::genAssem()
{
   emergencyStop("genAssem", ep);
}

void AnyNode::prepAssem(AssemData aData)
{
   emergencyStop("prepAssem", ep);
}

void AnyNode::genAssem()
{
   emergencyStop("genAssem", ep);
}

void ForNode::prepAssem(AssemData aData)
{
   emergencyStop("prepAssem", ep);
}

void ForNode::genAssem()
{
   emergencyStop("genAssem", ep);
}

void ThreadStopNode::prepAssem(AssemData aData)
{
   emergencyStop("prepAssem", ep);
}

void ThreadStopNode::genAssem()
{
   emergencyStop("genAssem", ep);
}

void ThreadStartNode::prepAssem(AssemData aData)
{
   emergencyStop("prepAssem", ep);
}

void ThreadStartNode::genAssem()
{
   emergencyStop("genAssem", ep);
}

void ThreadCallNode::prepAssem(AssemData aData)
{
   emergencyStop("prepAssem", ep);
}

void ThreadCallNode::genAssem()
{
   emergencyStop("genAssem", ep);
}

void StartNode::prepAssem(AssemData aData)
{
   emergencyStop("prepAssem", ep);
}

void StartNode::genAssem()
{
   emergencyStop("genAssem", ep);
}

void ThreadParamNode::prepAssem(AssemData aData)
{
   emergencyStop("prepAssem", ep);
}

void ThreadParamNode::genAssem()
{
   emergencyStop("genAssem", ep);
}

void EnumValueNode::prepAssem(AssemData aData)
{
   emergencyStop("prepAssem", ep);
}

void EnumValueNode::genAssem()
{
   emergencyStop("genAssem", ep);
}

void SendOptionNode::prepAssem(AssemData aData)
{
   emergencyStop("prepAssem", ep);
}

void SendOptionNode::genAssem()
{
   emergencyStop("genAssem", ep);
}

void SendNode::prepAssem(AssemData aData)
{
   emergencyStop("prepAssem", ep);
}

void SendNode::genAssem()
{
   emergencyStop("genAssem", ep);
}

void IterTypeNode::prepAssem(AssemData aData)
{
   emergencyStop("prepAssem", ep);
}

void IterTypeNode::genAssem()
{
   emergencyStop("genAssem", ep);
}

void ThreadNode::prepAssem(AssemData aData)
{
   emergencyStop("prepAssem", ep);
}

void ThreadNode::genAssem()
{
   emergencyStop("genAssem", ep);
}

void ReceiveOptionNode::prepAssem(AssemData aData)
{
   emergencyStop("prepAssem", ep);
}

void ReceiveOptionNode::genAssem()
{
   emergencyStop("genAssem", ep);
}

void ReceiveNode::prepAssem(AssemData aData)
{
   emergencyStop("prepAssem", ep);
}

void ReceiveNode::genAssem()
{
   emergencyStop("genAssem", ep);
}

void MatchNode::prepAssem(AssemData aData)
{
   emergencyStop("prepAssem", ep);
}

void MatchNode::genAssem()
{
   emergencyStop("genAssem", ep);
}

void EnumInitNode::prepAssem(AssemData aData)
{
   emergencyStop("prepAssem", ep);
}

void EnumInitNode::genAssem()
{
   emergencyStop("genAssem", ep);
}

void EnumStepNode::prepAssem(AssemData aData)
{
   emergencyStop("prepAssem", ep);
}

void EnumStepNode::genAssem()
{
   emergencyStop("genAssem", ep);
}

void EnumTermNode::prepAssem(AssemData aData)
{
   emergencyStop("prepAssem", ep);
}

void EnumTermNode::genAssem()
{
   emergencyStop("genAssem", ep);
}

void MapStepNode::prepAssem(AssemData aData)
{
   emergencyStop("prepAssem", ep);
}

void MapStepNode::genAssem()
{
   emergencyStop("genAssem", ep);
}

void MapTermNode::prepAssem(AssemData aData)
{
   emergencyStop("prepAssem", ep);
}

void MapTermNode::genAssem()
{
   emergencyStop("genAssem", ep);
}

void MapInitNode::prepAssem(AssemData aData)
{
   emergencyStop("prepAssem", ep);
}

void MapInitNode::genAssem()
{
   emergencyStop("genAssem", ep);
}

void RangeStepNode::prepAssem(AssemData aData)
{
   emergencyStop("prepAssem", ep);
}

void RangeStepNode::genAssem()
{
   emergencyStop("genAssem", ep);
}

void RangeTermNode::prepAssem(AssemData aData)
{
   emergencyStop("prepAssem", ep);
}

void RangeTermNode::genAssem()
{
   emergencyStop("genAssem", ep);
}

void RangeInitNode::prepAssem(AssemData aData)
{
   emergencyStop("prepAssem", ep);
}

void RangeInitNode::genAssem()
{
   emergencyStop("genAssem", ep);
}

void RemoveNode::prepAssem(AssemData aData)
{
   emergencyStop("prepAssem", ep);
}

void RemoveNode::genAssem()
{
   emergencyStop("genAssem", ep);
}


int CharTypeNode::getSize() const
{
   return CHAR_SIZE;
}

int ByteTypeNode::getSize() const
{
   return BYTE_SIZE;
}

int BoolTypeNode::getSize() const
{
   return BOOL_SIZE;
}

int IntegerTypeNode::getSize() const
{
   return INT_SIZE;
}

int FloatTypeNode::getSize() const
{
   return FLOAT_SIZE;
}

int DecimalTypeNode::getSize() const
{
   return DECIMAL_SIZE;
}

int EnumTypeNode::getSize() const
{
   return INT_SIZE;
}

int FileTypeNode::getSize() const
{
   return FILE_SIZE;
}

int MapTypeNode::getSize() const
{
   return MAP_SIZE;
}

int TextTypeNode::getSize() const
{
   return TEXT_SIZE;
}
