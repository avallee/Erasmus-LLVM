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

/** Conditions */
enum { COND_EQ, COND_NE, COND_LT, COND_LE, COND_GT, COND_GE };

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

// Literals

/** Literal bool. */
void literalB(int value);

/** Literal byte. */
void literalO(const char *value);

/** Literal unsigned byte. */
void literalUO(const char *value);

/** Literal integer. */
void literalI(const char *value);

/** Literal unsigned integer. */
void literalUI(const char *value);

/** Literal float. */
void literalF(const char *value);

/** Literal decimal. */
void literalD(const char *value);

/** Literal character. */
void literalC(char value);

/** Literal text. */
void literalT(const char *value);

// Push onto pseudo-stack from offset address

/** Push bool. */
void pushB(int offset);

/** Push byte. */
void pushO(int offset);

/** Push unsigned byte. */
void pushUO(int offset);

/** Push integer. */
void pushI(int offset);

/** Push unsigned integer. */
void pushUI(int offset);

/** Push float. */
void pushF(int offset);

/** Push decimal. */
void pushD(int offset);

/** Push character. */
void pushC(int offset);

/** Push text. */
void pushT(int offset);

// Pop from pseudo-stack to offset address

/** Pop bool. */
void popB(int offset);

/** Pop byte. */
void popO(int offset);

/** Pop unsigned byte. */
void popUO(int offset);

/** Pop integer. */
void popI(int offset);

/** Pop unsigned integer. */
void popUI(int offset);

/** Pop float. */
void popF(int offset);

/** Pop decimal. */
void popD(int offset);

/** Pop character. */
void popC(int offset);

/** Pop text. */
void popT(int offset);

/** Unconditional backward jump to \a target. */
void jumpBackward(void *target);

/** Unconditional forward jump, returning location to plant target. */
void *jumpForward();

/** Forward branch instruction for Boolean.
  * Compares register to 0, does not assign branch label.
  * \param compOp must be COND_EQ or COND_NE.
  * \return address for later patching.
  */
void *branchForwardB(int compOp);

/** Forward branch instruction for Integer.
  * Compares two registers, does not assign branch label.
  * \return address for later patching.
  */
void *branchForwardI(int compOp);

/** Forward branch instruction for unsigned Integer.
  * Compares two registers, does not assign branch label.
  * \return address for later patching.
  */
void *branchForwardUI(int compOp);

/** Forward branch instruction for Float and Decimal.
  * Compares two registers, does not assign branch label.
  * \return address for later patching.
  */
void *branchForwardF(int compOp);

/** Backward branch instruction for Boolean.
  * Compares register to 0, branches to \a target..
  */
void branchBackwardB(int compOp, void* target);

/** Backward branch instruction for Integer.
  * Compares two registers, branches to \a target..
  */
void branchBackwardI(int compOp, void *target);

/** Backward branch instruction for unsigned Integer.
  * Compares two registers, branches to \a target..
  */
void branchBackwardUI(int compOp, void *target);

/** Backward branch instruction for Float and Decimal.
  * Compares two registers, branches to \a target..
  */
void branchBackwardF(int compOp, void *target);


#ifdef __cplusplus
}
#endif

#endif
