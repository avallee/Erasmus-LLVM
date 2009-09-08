/** \file gen.cpp
 * These functions implement the first part of code generation.
 * An instance of GenData is passed around the AST to give nodes
 * information about code addresses, etc.
 *
 * \todo Combine \a gen and \a genBlocks functions, after doing some
 * dependency analysis to make sure that this would work.
 */

#include "ast.h"
#include "utilities.h"

/** Unique labels for switch cases in generated code. */
int blockNumber = 0;

/** Unique Number for variable. */
int varCounter = 0;

/** Unique number for process type. */
int typeCounter = 0;

/** Number for protocol field: unique within protocol. */
int fieldCounter = 0;

/** Number for select statement: unique within process. */
int selCounter = 0;

/** Add a block to the blocks list and set its transfer addresses.
 *  \param blocks is the list of blocks.
 *  \param endLabel is the transfer label for the current block.
 *  \param startLabel is the case label for the new block.
 *         It is usually the same as EndLabel.
 *  \param altLabel, if present, is the alternative label for
 *         the end of the current block.  It is used to generate
 *         code of the form pc = condition ? startLabel : altLabel.
 */
void addBlock(BlockList & blocks, int endLabel, int startLabel, int altLabel = -1)
{
   blocks.back()->transfer = endLabel;
   assert(altLabel);
   if (altLabel > 0)
   {
      blocks.back()->writeTransfer = false;
      blocks.back()->altTransfer = altLabel;
   }
   blocks.push_back(new BasicBlock(startLabel));
}

/** Constructor for the GenData object that is passed around the
 *  abstract syntax tree during the first phase of code generation.
 */
GenData::GenData() :
      selNum(-1), loopEnd(-1), ifEnd(-1), testGuard(-1),
      execBranch(-1), selectStart(-1), selectEnd(-1), seqIndex(-1)
{}

/** Set values that are needed for code generation, such as transfer addresses.
  * \param gd contains data related to code generation.
  */
void BaseNode::gen(GenData gd)
{}

/** Construct basic blocks and move code (in the form of AST nodes) into them. */
void BaseNode::genBlocks(BlockList & blocks, bool storeBlock)
{}

void ProgramNode::gen(GenData gd)
{
   for (ListIter it = nodes.begin(); it != nodes.end(); ++it)
      (*it)->gen(gd);
}

void ProgramNode::genBlocks(BlockList & blocks, bool storeBlock)
{
   for (ListIter it = nodes.begin(); it != nodes.end(); ++it)
      (*it)->genBlocks(blocks);
}

void InstanceNode::gen(GenData gd)
{
   name->gen(gd);
   for (ListIter it = args.begin(); it != args.end(); ++it)
      (*it)->gen(gd);
}

void InstanceNode::genBlocks(BlockList & blocks, bool storeBlock)
{
   if (blocks.size() > 0)
      blocks.back()->add(this);
}

void ProcedureNode::gen(GenData gd)
{
   typeNum = ++typeCounter;
   start = ++blockNumber;
   for (ListIter it = params.begin(); it != params.end(); ++it)
      (*it)->gen(gd);
   seq->gen(gd);
}

void ProcedureNode::genBlocks(BlockList &, bool storeBlock)
{
   selCounter = 0;
   BasicBlock *pb = new BasicBlock(start);
   pb->setClosure(this);
   blocks.push_back(pb);
   for (ListIter it = params.begin(); it != params.end(); ++it)
      (*it)->genBlocks(blocks);
   seq->genBlocks(blocks);
   blocks.back()->writeTransfer = false;
   blocks.back()->setUnlock();
   optimize(blocks);
}

void ProcessNode::gen(GenData gd)
{
   evmBlockNum = gd.evmBlockNumber;
   typeNum = ++typeCounter;
   start = ++blockNumber;
   for (ListIter it = params.begin(); it != params.end(); ++it)
      (*it)->gen(gd);
   seq->gen(gd);
}

void ProcessNode::genBlocks(BlockList &, bool storeBlock)
{
   selCounter = 0;
   BasicBlock *pb = new BasicBlock(start);
   pb->setClosure(this);
   blocks.push_back(pb);
   for (ListIter it = params.begin(); it != params.end(); ++it)
      (*it)->genBlocks(blocks);
   seq->genBlocks(blocks);
   blocks.back()->add(new RemoveNode);
   blocks.back()->writeTransfer = false;
   blocks.back()->setUnlock();
   optimize(blocks);
}

void ProtocolNode::gen(GenData gd)
{
   evmBlockNum = gd.evmBlockNumber;
   // Find the highest field counter assigned so far.
   // This avoids duplicate field numbers due to tieing.
   fieldCounter = 0;
   expr->getx(fieldCounter);
//   cerr << "Field counter = " << fieldCounter << endl; // 090207
   if (expr)
      expr->gen(gd);
   numFields = fieldCounter;
//   showDebug("ProtocolNode::gen"); //090207
}

void DefNode::gen(GenData gd)
{
   // The order of these statements is important!
   name->gen(gd);
   gd.entity = name->getNameString();
   gd.evmBlockNumber = name->getEVMBlockNumber();
   value->gen(gd);
}

void DefNode::genBlocks(BlockList & blocks, bool storeBlock)
{
   name->genBlocks(blocks);
   value->genBlocks(blocks);
}

void CellNode::gen(GenData gd)
{
   evmBlockNum = gd.evmBlockNumber;
   for (ListIter it = params.begin(); it != params.end(); ++it)
      (*it)->gen(gd);
   for (ListIter it = instances.begin(); it != instances.end(); ++it)
      (*it)->gen(gd);
}

void CellNode::genBlocks(BlockList & blocks, bool storeBlock)
{}

void SequenceNode::gen(GenData gd)
{
   for (ListIter it = stmts.begin(); it != stmts.end(); ++it)
   {
      (*it)->gen(gd);
      if (gd.seqIndex >= 0)
         ++gd.seqIndex;
   }
}

void SequenceNode::genBlocks(BlockList & blocks, bool storeBlock)
{
   for (ListIter it = stmts.begin(); it != stmts.end(); ++it)
      (*it)->genBlocks(blocks);
}

void ExitNode::gen(GenData gd)
{
   loopEnd = gd.loopEnd;
}

void ExitNode::genBlocks(BlockList & blocks, bool storeBlock)
{
   blocks.back()->transfer = loopEnd;
   blocks.push_back(new BasicBlock());
}

void IfNode::gen(GenData gd)
{
   ifEnd = ++blockNumber;
   gd.ifEnd = ifEnd;
   for (ListIter it = condPairs.begin(); it != condPairs.end(); ++it)
      (*it)->gen(gd);
   alt->gen(gd);
}

void IfNode::genBlocks(BlockList & blocks, bool storeBlock)
{
   for (ListIter it = condPairs.begin(); it != condPairs.end(); ++it)
      (*it)->genBlocks(blocks);
   alt->genBlocks(blocks);
   addBlock(blocks, ifEnd, ifEnd);
}

void CondPairNode::gen(GenData gd)
{
   condTrue = ++blockNumber;
   condFalse = ++blockNumber;
   ifEnd = gd.ifEnd;
   for (ListIter it = decs.begin(); it != decs.end(); ++it)
      (*it)->gen(gd);
   cond->gen(gd);
   seq->gen(gd);
}

void CondPairNode::genBlocks(BlockList & blocks, bool storeBlock)
{
   for (ListIter it = decs.begin(); it != decs.end(); ++it)
      (*it)->genBlocks(blocks);
   cond->genBlocks(blocks, true);
   blocks.back()->add(cond);
   addBlock(blocks, condTrue, condTrue, condFalse);
   seq->genBlocks(blocks);
   addBlock(blocks, ifEnd, condFalse);
}

void LoopNode::gen(GenData gd)
{
   loopStart = ++blockNumber;
   loopEnd = ++blockNumber;
   gd.loopEnd = loopEnd;
   seq->gen(gd);
}

void LoopNode::genBlocks(BlockList & blocks, bool storeBlock)
{
   addBlock(blocks, loopStart, loopStart);
   seq->genBlocks(blocks);
   addBlock(blocks, loopStart, loopEnd);
}

void ForNode::gen(GenData gd)
{
   forMore = ++blockNumber;
   forMatch = ++blockNumber;
   forBody = ++blockNumber;
   forStep = ++blockNumber;
   forEnd = ++blockNumber;

   comp->gen(gd);
   seq->gen(gd);
}

void ForNode::genBlocks(BlockList & blocks, bool storeBlock)
{
   blocks.back()->add(comp->makeInit(0, 0));
   addBlock(blocks, forMore, forMore);
   blocks.back()->add(comp->makeTermTest(blocks.back(), 0, 0));
   addBlock(blocks, forEnd, forMatch, forMatch);
   blocks.back()->add(comp->makeMatchTest(blocks.back()));
   addBlock(blocks, forStep, forBody, forBody);
   seq->genBlocks(blocks);
   addBlock(blocks, forStep, forStep);
   blocks.back()->add(comp->makeStep(0, 0));
   addBlock(blocks, forMore, forEnd);
}

void AnyNode::gen(GenData gd)
{
   anyMore = ++blockNumber;
   anyFound = ++blockNumber;
   anyStep = ++blockNumber;
   anyBody = ++blockNumber;
   anyElse = ++blockNumber;
   anyEnd = ++blockNumber;

   comp->gen(gd);
   seq->gen(gd);
   alt->gen(gd);
}

void AnyNode::genBlocks(BlockList & blocks, bool storeBlock)
{
   blocks.back()->add(comp->makeInit(0, 0));
   addBlock(blocks, anyMore, anyMore);
   blocks.back()->add(comp->makeTermTest(blocks.back(), 0, 0));
   addBlock(blocks, anyElse, anyFound, anyFound);
   blocks.back()->add(comp->makeMatchTest(blocks.back()));
   addBlock(blocks, anyStep, anyStep, anyBody);
   blocks.back()->add(comp->makeStep(0, 0));
   addBlock(blocks, anyMore, anyBody);
   seq->genBlocks(blocks);
   addBlock(blocks, anyEnd, anyElse);
   alt->genBlocks(blocks);
   addBlock(blocks, anyEnd, anyEnd);
}

void ComprehensionNode::gen(GenData gd)
{
   var->gen(gd);
   iter->gen(gd);
   if (type)
      type->gen(gd);
   collection->gen(gd);
   if (pred)
      pred->gen(gd);
}

void ComprehensionNode::genBlocks(BlockList & blocks, bool storeBlock)
{
}

void RangeNode::gen(GenData gd)
{
   finishNum = ++blockNumber; // varCounter++;
   stepNum = ++blockNumber; // varCounter++;

   owner = gd.entity;
   start->gen(gd);
   finish->gen(gd);
   if (step)
      step->gen(gd);
}

void EnumSetNode::gen(GenData gd)
{
   owner = gd.entity;
}

void EnumSetNode::genBlocks(BlockList & blocks, bool storeBlock)
{
}

void RangeNode::genBlocks(BlockList & blocks, bool storeBlock)
{
}

void MapSetNode::gen(GenData gd)
{
   owner = gd.entity;
   indexName = "index" + str(++blockNumber);
   map->gen(gd);
}

void MapSetNode::genBlocks(BlockList & blocks, bool storeBlock)
{
}

void SelectNode::gen(GenData gd)
{
   owner = gd.entity;

   selNum = ++selCounter;
   gd.selNum = selNum;

   selectStart = ++blockNumber;
   gd.selectStart = selectStart;

   selectEnd = ++blockNumber;
   gd.selectEnd = selectEnd;

   numBranches = 0;
   for (ListIter it = options.begin(); it != options.end(); ++it)
   {
      ++numBranches;
      gd.testGuard = ++blockNumber;
      gd.execBranch = ++blockNumber;
      (*it)->gen(gd);
   }
}

void SelectNode::genBlocks(BlockList & blocks, bool storeBlock)
{
   addBlock(blocks, selectStart, selectStart);
   blocks.back()->add(this);
   blocks.back()->writeTransfer = false;
   for (ListIter it = options.begin(); it != options.end(); ++it)
      (*it)->genBlocks(blocks);
   addBlock(blocks, selectEnd, selectEnd);
}

void OptionNode::gen(GenData gd)
{
   owner = gd.entity;

   selNum = gd.selNum;
   selectStart = gd.selectStart;
   selectEnd = gd.selectEnd;
   testGuard = gd.testGuard;
   execBranch = gd.execBranch;
   if (guard)
      guard->gen(gd);
   gd.seqIndex = 0; // Enables first statement of option to be identified
   seq->gen(gd);
}

void OptionNode::genBlocks(BlockList & blocks, bool storeBlock)
{
   blocks.push_back(new BasicBlock(testGuard));
   blocks.back()->writeTransfer = false;
   blocks.back()->add(this);
   blocks.push_back(new BasicBlock(execBranch));
   seq->genBlocks(blocks);
   blocks.back()->transfer = selectEnd;
}

void DotNode::gen(GenData gd)
{
   port->gen(gd);
   field->gen(gd);
}

void QueryNode::gen(GenData gd)
{
   port->gen(gd);
   field->gen(gd);
}

void QueryNode::genBlocks(BlockList & blocks, bool storeBlock)
{
   blocks.back()->add(this);
}

void ConstantNode::gen(GenData gd)
{
   evmBlockNum = ++blockNumber;
   name->gen(gd);
   type->gen(gd);
   value->gen(gd);
}

void ConstantNode::genBlocks(BlockList & blocks, bool storeBlock)
{
}

void NumNode::gen(GenData gd)
{
   evmBlockNum = ++blockNumber;
}

void CharNode::gen(GenData gd)
{
   evmBlockNum = ++blockNumber;
}

void TextNode::gen(GenData gd)
{
   evmBlockNum = ++blockNumber;
}

void DecNode::gen(GenData gd)
{
   name->gen(gd);
   if (type)
      type->gen(gd);
   if (value)
      value->gen(gd);
//   evmBlockNum = ++blockNumber;
   transfer = ++blockNumber;
   tempnum = ++blockNumber; // ++varCounter;
   branch = gd.seqIndex == 0;
}

void DecNode::genBlocks(BlockList & blocks, bool storeBlock)
{
   Node port = 0;
   int fieldNum = 0;
   string bufferName = "";
   FileMode mode = SYS_NULL;
   if (name->kind() == DOT_NODE)
   {
      port = name->getPort();
      fieldNum = name->getFieldNum();
      bufferName = name->getPortBufferName();
      mode = name->checkSysIO();
      // name->showDebug("Name '" + bufferName + "'"); // 080916
   }
   else if (value && value->kind() == DOT_NODE)
   {
      port = value->getPort();
      fieldNum = value->getFieldNum();
      bufferName = value->getPortBufferName();
      mode = value->checkSysIO();
      // value->showDebug("Value '" + bufferName + "'"); // 080916
   }
   string tempName = "temp" + str(tempnum);


   enum
   {
      ASSIGNMENT, SENDING, RECEIVING, QUERY
   }
   kind = ASSIGNMENT;
   bool signal = false;
   if (name->kind() == DOT_NODE)
   {
      if (value)
         kind = SENDING;
      else
      {
         // DotNode and no value => signal
         PortKind portDir = name->getPortKind();
         switch (portDir)
         {
            case SERVER:
               kind = RECEIVING;
               break;
            case CLIENT:
               kind = SENDING;
               break;
            default:
               Error() << "Port kind = " << portToString(portDir) << ep << REPORT;
               break;
         }
         signal = true;
      }
   }
   else if (value && value->kind() == DOT_NODE)
      kind = RECEIVING;
   else if (value && value->kind() == QUERY_NODE)
      kind = QUERY;

   switch (kind)
   {
      case ASSIGNMENT:
         blocks.back()->add(this);
         break;

      case SENDING:
         if (branch)
         {
            // Select branch
            blocks.back()->add(new SendOptionNode(ep, value, port, fieldNum, bufferName));
            blocks.back()->setUnlock();
            addBlock(blocks, transfer, transfer);
         }
         else
         {
            blocks.back()->add(new SendNode(ep, value, port, tempName, bufferName, fieldNum, mode));
            if ( ! (mode == SYS_OUT || mode == SYS_ERR) )
            {
               blocks.back()->setUnlock();
               addBlock(blocks, transfer, transfer);
            }
         }
         break;

      case RECEIVING:
         if (branch)
         {
            // Select branch
            blocks.back()->add(new ReceiveOptionNode(ep, name, port, type, fieldNum,
                               bufferName, signal));
            blocks.back()->setUnlock();
            addBlock(blocks, transfer, transfer);
         }
         else if (mode == SYS_IN)
            blocks.back()->add(new ReceiveNode(ep, name, port, type, bufferName,
                                               signal, fieldNum, mode));
         else
         {
            blocks.back()->add(new ReceiveNode(ep, name, port, type, bufferName,
                                               signal, fieldNum, mode));
            blocks.back()->setUnlock();
            addBlock(blocks, transfer, transfer);
         }
         break;

      case QUERY:
         blocks.back()->add(value);
         blocks.back()->setUnlock();
         addBlock(blocks, transfer, transfer);
         blocks.back()->add(new QueryNode(
            value->getPos(),
            value->getPort(),
            value->getField(),
            name,
            2));
         break;

   }
}

void EnumTypeNode::genBlocks(BlockList & blocks, bool storeBlock)
{
   name = "enum_values_" + str(++blockNumber);
   for (ListIter it = values.begin(); it != values.end(); ++it)
      (*it)->genBlocks(blocks, storeBlock);
}

void EnumValueNode::genBlocks(BlockList & blocks, bool storeBlock)
{
   evmBlockNum = ++blockNumber;
}

void BoolNode::genBlocks(BlockList & blocks, bool storeBlock)
{
   if (storeBlock)
      bb = blocks.back();
}

void ListopNode::gen(GenData gd)
{
   for (ListIter it = args.begin(); it != args.end(); ++it)
      (*it)->gen(gd);
}

void ListopNode::genBlocks(BlockList & blocks, bool storeBlock)
{
   for (ListIter it = args.begin(); it != args.end(); ++it)
      (*it)->genBlocks(blocks);
}

void BinopNode::gen(GenData gd)
{
   lhs->gen(gd);
   rhs->gen(gd);
}

void BinopNode::genBlocks(BlockList & blocks, bool storeBlock)
{
   if (op == BINOP_EXTEND)
      blocks.back()->add(this);
   else
   {
      if (storeBlock)
         bb = blocks.back();
      lhs->genBlocks(blocks);
      rhs->genBlocks(blocks);
   }
}

void CondExprNode::gen(GenData gd)
{
   lhs->gen(gd);
   pred->gen(gd);
   rhs->gen(gd);
}

void CondExprNode::genBlocks(BlockList & blocks, bool storeBlock)
{
   if (storeBlock)
      bb = blocks.back();
   lhs->genBlocks(blocks);
   pred->genBlocks(blocks);
   rhs->genBlocks(blocks);
}

void UnopNode::gen(GenData gd)
{
   operand->gen(gd);
}

void UnopNode::genBlocks(BlockList & blocks, bool storeBlock)
{
   operand->genBlocks(blocks);
   if (  storeBlock &&
         op == UNOP_NOT &&
         type == theBoolNode)
      bb = blocks.back();
}

void SubscriptNode::gen(GenData gd)
{
   base->gen(gd);
   sub->gen(gd);
}

void SubscriptNode::genBlocks(BlockList & blocks, bool storeBlock)
{}

void SubrangeNode::gen(GenData gd)
{
   base->gen(gd);
   sub1->gen(gd);
   sub2->gen(gd);
}

void SubrangeNode::genBlocks(BlockList & blocks, bool storeBlock)
{}

void IteratorNode::gen(GenData gd)
{
   owner = gd.entity;
   map->gen(gd);
}

void IteratorNode::genBlocks(BlockList & blocks, bool storeBlock)
{
   switch (fun)
   {
      case ITER_START:
      case ITER_STEP:
         blocks.back()->add(this);
         break;
      case ITER_FINISH:
      case ITER_KEY:
      case ITER_VALUE:
         if (storeBlock)
            bb = blocks.back();
         break;
   }
}

void FunctionNode::gen(GenData gd)
{
   for (ListIter it = args.begin(); it != args.end(); ++it)
      (*it)->gen(gd);
}

void FunctionNode::genBlocks(BlockList & blocks, bool storeBlock)
{
   if (storeBlock)
      bb = blocks.back();

   // The function is actually a procedure, not part of an expression.
   string fun = getFun();
   if ( fun == "assert" ||
         fun == "file_close" ||
         fun == "file_write" )
      blocks.back()->add(this);

   for (ListIter it = args.begin(); it != args.end(); ++it)
      (*it)->genBlocks(blocks);
}

void ArrayTypeNode::gen(GenData gd)
{
   lo->gen(gd);
   hi->gen(gd);
}

void NameNode::gen(GenData gd)
{
   if (definingOccurrence)
   {
      evmBlockNum = ++blockNumber;
      varNum = ++blockNumber; // ++varCounter;
      owner = gd.entity;
      if (fieldNum < 0)
      {
         fieldNum = fieldCounter++;
//         cerr << value << " #" << fieldNum << endl; // 090207
         Node p = tie;
         while (p != this)
         {
            p->setFieldNum(fieldNum);
            p = p->getTie();
         }
      }
   }
   else if (definition)
   {
      owner = definition->getOwner();
      varNum = definition->getVarNum();
   }
}

void NameNode::genBlocks(BlockList & blocks, bool storeBlock)
{
   // Store a pointer to the current block in case this name
   // is used as a Boolean expression in a decision block.
   if (storeBlock && getType() == theBoolNode)
      bb = blocks.back();
}

void ThreadNode::gen(GenData gd)
{
   start = ++blockNumber;
   fieldCounter = 0;
   for (ListIter it = inputs.begin(); it != inputs.end(); ++it)
      (*it)->gen(gd);
   for (ListIter it = outputs.begin(); it != outputs.end(); ++it)
      (*it)->gen(gd);
   numFields = fieldCounter;
   seq->gen(gd);
}

void ThreadNode::genBlocks(BlockList &, bool storeBlock)
{
   BasicBlock *pb = new BasicBlock(start);
   pb->setClosure(this);
   blocks.push_back(pb);
   for (ListIter it = inputs.begin(); it != inputs.end(); ++it)
      (*it)->genBlocks(blocks);
   seq->genBlocks(blocks);
   for (ListIter it = outputs.begin(); it != outputs.end(); ++it)
      (*it)->genBlocks(blocks);
   blocks.back()->add(new RemoveNode);
   blocks.back()->writeTransfer = false;
   blocks.back()->setUnlock();
   optimize(blocks);
}

void ThreadParamNode::gen(GenData gd)
{
   transfer = ++blockNumber;
   owner = gd.entity;
   name->gen(gd);
   type->gen(gd);
}

void ThreadParamNode::genBlocks(BlockList & blocks, bool storeBlock)
{
   string bufferName = type->getPortBufferName();
   int fieldNum = name->getFieldNum();
   if (input)
   {
      blocks.back()->add(new ReceiveNode(ep, name, port, type, bufferName, false, fieldNum));
      blocks.back()->setUnlock();
      addBlock(blocks, transfer, transfer);
   }
   else
   {
      blocks.back()->add(new SendNode(ep, this, port, "tempName", bufferName, fieldNum));
      blocks.back()->setUnlock();
      addBlock(blocks, transfer, transfer);
   }
}

void StartNode::gen(GenData gd)
{
   cerr << "090816 Generating Start node\n";

   for (ListIter it = calls.begin(); it != calls.end(); ++it)
      (*it)->gen(gd);
   seq->gen(gd);
}

void StartNode::genBlocks(BlockList & blocks, bool storeBlock)
{
   for (ListIter it = calls.begin(); it != calls.end(); ++it)
      (*it)->genBlocks(blocks);
   for (ListIter it = calls.begin(); it != calls.end(); ++it)
      // Create threads and send input arguments
      (*it)->startThread(blocks);
   seq->genBlocks(blocks);
   for (ListIter it = calls.begin(); it != calls.end(); ++it)
      // Receive results and destroy thread
      (*it)->stopThread(blocks);
}

void ThreadCallNode::startThread(BlockList & blocks)
{
   blocks.back()->add(new ThreadStartNode(Errpos(), name, chName, numFields));
   int fieldNum = 0;
   int transfer = inputTransfer;
   for (ListIter it = inputs.begin(); it != inputs.end(); ++it)
   {
      Node type = (*it)->getType();
      string bufferName = type->getPortBufferName();
      blocks.back()->add(new SendNode(ep, *it, chName, "tempName", bufferName, fieldNum));
      blocks.back()->setUnlock();
      addBlock(blocks, transfer, transfer);
      ++fieldNum;
      ++transfer;
   }
}

void ThreadCallNode::stopThread(BlockList & blocks)
{
   int fieldNum = inputs.size();
   int transfer = outputTransfer;
   for (ListIter it = outputs.begin(); it != outputs.end(); ++it)
   {
      Node type = (*it)->getType();
      string bufferName = type->getPortBufferName();
      blocks.back()->add(new ReceiveNode(ep, *it, chName, type, bufferName, false, fieldNum));
      blocks.back()->setUnlock();
      addBlock(blocks, transfer, transfer);
      ++fieldNum;
      ++transfer;
   }
   blocks.back()->add(new ThreadStopNode(Errpos(), name, chName));
}

void ThreadCallNode::gen(GenData gd)
{
   name->gen(gd);
   chName = new NameNode(Errpos(), "ch", 0, true);
   chName->gen(gd);
   numFields = inputs.size() + outputs.size();
   startTransfer = ++blockNumber;
   inputTransfer = blockNumber + 1;
   blockNumber += inputs.size();
   outputTransfer = blockNumber + 1;
   blockNumber += outputs.size();
   stopTransfer = ++blockNumber;
   name->gen(gd);
   for (ListIter it = inputs.begin(); it != inputs.end(); ++it)
      (*it)->gen(gd);
   for (ListIter it = outputs.begin(); it != outputs.end(); ++it)
      (*it)->gen(gd);
}

void ThreadCallNode::genBlocks(BlockList & blocks, bool storeBlock)
{
   blocks.back()->add(this);
}
