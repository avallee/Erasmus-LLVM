/** \file genassem.h */

#ifndef GENASSEM_H
#define GENASSEM_H

#include "types.h"

struct AssemData
{
   int typeCode;
   bool lvalue;
   Node ifParent;Node loopParent;
};

/** Translate MEC condition codes to MEL condition codes.
  * An unfortunate shim that reflects the historical development of MEC/MEL.
  */
int transOps(Operator op);

/** Branch forward if \a expr is true.
  * \param expr does not contain \a and or \a or.
  * \param keys indexes branch instructions to be patched.
  */
void simpleBranchTrue(Node expr, Patches & keys);

/** Branch forward if \a expr is false.
  * \param expr does not contain \a and or \a or.
  * \param keys indexes branch instructions to be patched.
  */
void simpleBranchFalse(Node expr, Patches & keys);

/** Branch forward if \a expr is true.
  * \a expr may containg \a and and \a or subexpressions.
  * \param keys indexes branch instructions to be patched.
  */
void branchTrue(Node expr, Patches & keys);

/** Branch forward if \a expr is false.
  * \a expr may containg \a and and \a or subexpressions.
  * \param keys indexes branch instructions to be patched.
  */
void branchFalse(Node expr, Patches & keys);

void genPushVar(Node expr, int typeCode);

/** Generate a literal with the default value of the given type. */
void genDefault(int typeCode);

/** Generate code for the expression \a expr.
  * \todo Generate additional code to check that the value of the expression
  * conforms to the type \a typeCode.
  */
void rangeCheck(Node expr, int typeCode);

/** Generate code for the given expression. */
void pushExpr(Node expr, AssemData aData);

#endif

