/** \file basicblocks.h
 *
 * Basic blocks are used to store lists of AST nodes prior to final
 * code generation.  They are not true "basic blocks" because they
 * may have multiple exit points.
 */

#ifndef BASICBLOCKS_H
#define BASICBLOCKS_H

#include "types.h"

#include <iostream>

using namespace std;

class EVMData;

void optimize(BlockList & blocks);

class BasicBlock
{
public:
   BasicBlock(int start = -1, int transfer = 0);

   /** Add a statement to the statement list. */
   void add(Node st);

   /** Set pointer to closure node. */
   void setClosure(Node p)
   {
      closure = p;
   }

   /** Set flag to unlock. */
   void setUnlock()
   {
      unlock = true;
   }

   /** Generate EVM code for this block. */
   // void writeEVM(ostream & evm, EVMData & ed);

   /** Use 'show' to write the nodes in this block. */
   void showBB(ostream & os) const;

   /** Nodes for code in this block. */
   List stmts;

   /** Start address for block (i.e., its case label).
    *  A negative value indicates an invalid address
    *  and thus an unreachable block.
    */
   int start;

   /** Flag set if we must write the address of the successor block. */
   bool writeTransfer;

   /** Transfer address for block (i.e., the successor block). */
   int transfer;

   /** Second transfer address for a block containing a choice. */
   int altTransfer;

   /** Pointer to closure node if this is the first block of a closure. */
   Node closure;

   /** Flag indicating that 'unlock' is required. */
   bool unlock;

   friend std::ostream & operator<<(std::ostream & os, Block bb);
};

#endif

