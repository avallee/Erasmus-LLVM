/** \file basicblocks.cpp */

#include "basicblocks.h"
#include "ast.h"
#include "utilities.h"

#include <iostream>

using namespace std;

BasicBlock::BasicBlock(int start, int transfer)
      : start(start), transfer(transfer), altTransfer(0),
      writeTransfer(true), closure(0), unlock(false) //, lock(false)
{
}

void BasicBlock::add(Node st)
{
   stmts.push_back(st);
}

/** Perform simple tests to remove redundant blocks. */
void optimize(BlockList & blocks)
{
   int changes = 1;
   while (changes > 0)
   {
      changes = 0;
      BlockIter it = blocks.begin();
      while (it != blocks.end())
      {
         // Negative start indicates block is unreachable.
         if ((*it)->start < 0)
         {
            it = blocks.erase(it);
            ++changes;
         }

         // A block can be removed if it has no statements
         // and does not change locking status.
         else if ((*it)->stmts.size() == 0 && !(*it)->unlock)
         {
            int start = (*it)->start;

//            cerr << "\nRemoving " << start << endl;

            Node closure = (*it)->closure;
            int transfer = (*it)->transfer;
            for (BlockIter ib = blocks.begin(); ib != blocks.end(); ++ib)
            {
               if ((*ib)->start == transfer && closure !=0)
                  (*ib)->closure = closure;
               if ((*ib)->transfer == start)
                  (*ib)->transfer = transfer;
               if ((*ib)->altTransfer == start)
                  (*ib)->altTransfer = transfer;
            }
            it = blocks.erase(it);
            ++changes;
         }
         else
            ++it;
      }

      // Build a set of all labels in use
      set<int> labels;
      for (BlockIter it = blocks.begin(); it != blocks.end(); ++it)
      {
         labels.insert((*it)->transfer);
         labels.insert((*it)->altTransfer);

         // Include addresses of option nodes
         for (ListIter is = (*it)->stmts.begin(); is != (*it)->stmts.end(); ++is)
         {
            NodeKind k = (*is)->kind();
            if (k == OPTION_NODE || k == RECEIVE_OPTION_NODE || k == SEND_OPTION_NODE)
               labels.insert((*it)->start);
         }
      }

//      cerr << "Labels: ";
//      for (set<int>::iterator it = labels.begin(); it != labels.end(); ++it)
//         cerr << ' ' << *it;
//      cerr << endl;

      it = blocks.begin();
      while (it != blocks.end())
      {
         if ((*it)->closure == 0 && labels.find((*it)->start) == labels.end())
         {
            it = blocks.erase(it);
            ++changes;
         }
         else
            ++it;
      }
   }
}

/** Write a block to the generated code stream. */
ostream & operator<<(ostream & code, Block bb)
{
   TAB2 << "case " << bb->start << ":\n";
   TAB2 << "{\n";
   for (ListIter it = bb->stmts.begin(); it != bb->stmts.end(); ++it)
   {
      if ((*it)->kind() == BINOP_NODE && (*it)->getOp() == BINOP_EXTEND)
      {
         // Kludge to ensure that 'a &e x &e y' is printed correctly.
         TAB3;
         (*it)->write(code);
         code << ";\n";
      }
      else
         (*it)->write(code);
   }

   if (bb->writeTransfer)
      TAB3 << "pc = " << bb->transfer << ";\n";
   if (bb->unlock)
      TAB3 << "return;\n";
   else
      TAB3 << "break;\n";
   TAB2 << "}\n";
   return code;
}

void BasicBlock::showBB(ostream & os) const
{
   os << "\n\nBlock " << start << setw(5) << transfer;
   if (altTransfer > 0)
      os << setw(5) << altTransfer;
   for (ListIter it = stmts.begin(); it != stmts.end(); ++it)
      (*it)->show(os, 2);
}

