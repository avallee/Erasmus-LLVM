/** \file assembler.h
  * This is the interface between the Erasmus code generator
  * and the Lightning assembler.  The functions are declared
  * as "C" functions because all Lightning-related code is
  * compiled as C code.
  */

#ifndef FIREWALL_H
#define FIREWALL_H

#include "typecodes.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ADDR_SIZE     sizeof(void*)
#define BOOL_SIZE     sizeof(int)
#define CHAR_SIZE     sizeof(char)
#define TEXT_SIZE     sizeof(void*)
#define BYTE_SIZE     sizeof(char)
#define UNS_BYTE_SIZE sizeof(unsigned char)
#define INT_SIZE      sizeof(int)
#define UNS_INT_SIZE  sizeof(unsigned int)
#define DECIMAL_SIZE  sizeof(double)
#define FLOAT_SIZE    sizeof(double)
#define ARRAY_SIZE    ADDR_SIZE
#define MAP_SIZE      ADDR_SIZE
#define FILE_SIZE     ADDR_SIZE
#define VOID_SIZE     0

/** Binary operators.
  * These are typed in the front-end, but C does not allow
    named enumeration types. */
enum {
   // Boolean
   BOOL_OR, BOOL_AND,

   // Conditional
   COND_EQ, COND_NE, COND_LT, COND_LE, COND_GT, COND_GE,

   // Arithmetic
   ARI_PLUS, ARI_MINUS, ARI_MULTIPLY, ARI_DIVIDE, ARI_MOD,

   // Other
   BIN_EXTEND, BIN_CAT
};

/** Show the sizes of the types defined by Lightning. */
void showSizes();

/** Return the address of the next instruction to be generated. */
void *getLabel();

/** Set the label of a branch instruction.
  * A branch requiring a forward reference returns an address to itself, \a b.
  * When the branch target is reached, \a setLabel(b) is called to patch the address.
  */
void setLabel(void *branchIns);

// The next three functions must be called in this order: prolog - epilog - execute.

/** Code to be generated before anything else happens. */
void prolog();

/** Code to be generated when we are all finished. */
void epilog();

/** Execute the generated code. */
void execute();

/** Display the assembly code in mnemonic form. */
void showCode();

/** Push literal onto pseudo-stack. */
void literal(int typeCode, const char *value);

/** Push onto pseudo-stack from offset address. */
void push(int typeCode, int offset);

/** Pop from pseudo-stack to offset address. */
void pop(int typeCode, int offset);

/** Binary operator */
void assBinop(int typeCode, int opCode);

/** Unconditional backward jump to \a target. */
void jumpBackward(void *target);

/** Unconditional forward jump, returning location to plant target. */
void *jumpForward();

/** Forward branch.
  * Compares register to 0, does not assign branch label.
  * \param compOp must be COND_EQ or COND_NE.
  * \return address for later patching.
  */
void *branchForward(int typeCode, int compOp);

/** Backward branch.
  * Compares register to 0, branches to \a target..
  */
void branchBackward(int typeCode, int compOp, void* target);

#ifdef __cplusplus
}
#endif

#endif
