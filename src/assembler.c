/** \file assembler.c */

#include "lightning.h"
#include "assembler.h"

#include <stdio.h>

/* Lightning jit_ calls are macros.  They may access their arguments more than
 * once.  For this reason, we do not use functions as arugments - especially
 * functions with side-effects!
 */

/* Type program = void -> void). */
typedef void (*Program)(void);

/* Function to invoke generated code. */
Program prog;

/* Storage for generated code. */
char buffer[10000];

#define MAXREGS 6

/** Lightning's registers. */
int regs[MAXREGS] =
{
   JIT_R0, JIT_R1, JIT_R2,
   JIT_V0, JIT_V1, JIT_V2
};

/** We use the registers as a stack. \a sp is the stack pointer. */
int sp;

// Utility functions

/** Show the sizes of the types defined by Lightning. */
void showSizes()
{
   printf("c   %3d\n", sizeof(char));
   printf("uc  %3d\n", sizeof(unsigned char));
   printf("s   %3d\n", sizeof(short));
   printf("us  %3d\n", sizeof(unsigned short));
   printf("i   %3d\n", sizeof(int));
   printf("ui  %3d\n", sizeof(unsigned int));
   printf("l   %3d\n", sizeof(long));
   printf("ul  %3d\n", sizeof(unsigned long));
   printf("f   %3d\n", sizeof(float));
   printf("d   %3d\n", sizeof(double));
   printf("p   %3d\n", sizeof(void*));
}

/** Return the register to be used when "pushing" the stack. */
int pushReg()
{
//   printf("push %d\n", sp);
   if (sp >= MAXREGS - 1)
   {
      printf("Register stack overflow!\n");
      return regs[0];
   }
   else
      return regs[sp++];
}

/** Return the register to be used when "popping" the stack. */
int popReg()
{
//   printf("pop %d\n", sp);
   if (sp <= 0)
   {
      printf("Register stack underflow at %x\n", jit_get_ip());
      return regs[0];
   }
   else
      return regs[--sp];
}

/* General purpose functions */

void *getLabel()
{
   return jit_get_label();
}

void setLabel(void *branchIns)
{
   jit_patch((jit_insn*)branchIns);
}

void prolog()
{
   prog = (Program)(jit_set_ip(buffer).iptr);
   jit_prolog(0);
   sp = 0;
}

void epilog()
{
   jit_ret();
   jit_flush_code(buffer, jit_get_ip().ptr);
}

void execute()
{
   prog();
}

void showCode()
{
   printf("Disassembly (%d bytes):\n", jit_get_ip().ptr - buffer);
   disassemble(stderr, buffer, jit_get_ip().ptr);
}

void literalB(int value)
{
   int reg = pushReg();
   jit_movi_i(reg, value);
}

void literalO(const char *value)
{
   int reg = pushReg();
   jit_movi_i(reg, atoi(value));
}

void literalUO(const char *value)
{
   int reg = pushReg();
   jit_movi_i(reg, atoi(value));
}

void literalI(const char *value)
{
   int reg = pushReg();
   jit_movi_i(reg, atoi(value));
}

void literalUI(const char *value)
{
   int reg = pushReg();
   jit_movi_ui(reg, atoi(value));
}

void literalC(char value)
{
   int reg = pushReg();
   jit_movi_i(reg, (int)value);
}

void literalT(const char *value)
{
}

void literalF(const char *value)
{
}

void literalD(const char *value)
{
}

void pushB(int offset)
{
   int reg = pushReg();
   jit_ldxi_i(reg, JIT_FP, offset);
}

void pushC(int offset)
{
   int reg = pushReg();
   jit_ldxi_ui(reg, JIT_FP, offset);
}

void pushO(int offset)
{
   int reg = pushReg();
   jit_ldxi_c(reg, JIT_FP, offset);
}

void pushUO(int offset)
{
   int reg = pushReg();
   jit_ldxi_uc(reg, JIT_FP, offset);
}

void pushI(int offset)
{
   int reg = pushReg();
   jit_ldxi_i(reg, JIT_FP, offset);
}

void pushUI(int offset)
{
   int reg = pushReg();
   jit_ldxi_ui(reg, JIT_FP, offset);
}

void pushF(int offset) {  }

void pushD(int offset) {  }

void pushT(int offset) {  }


void popB(int offset)
{
   int reg = popReg();
   jit_stxi_i(offset, JIT_FP, reg);
}

void popC(int offset)
{
   int reg = popReg();
   jit_stxi_ui(offset, JIT_FP, reg);
}

void popO(int offset)
{
   int reg = popReg();
   jit_stxi_i(offset, JIT_FP, reg);
//   We would like to use the following, but it doesn't compile.
//   jit_stxi_c(offset, JIT_FP, reg);
}

void popUO(int offset)
{
   int reg = popReg();
   jit_stxi_ui(offset, JIT_FP, reg);
//   We would like to use the following, but it doesn't compile.
//   jit_stxi_uc(offset, JIT_FP, reg);
}

void popI(int offset)
{
   int reg = popReg();
   jit_stxi_i(offset, JIT_FP, reg);
}

void popUI(int offset)
{
   int reg = popReg();
   jit_stxi_ui(offset, JIT_FP, reg);
}

void popF(int offset) {  }

void popD(int offset) {  }

void popT(int offset) {  }

// Unconditional jumps

void jumpBackward(void *target)
{
   jit_jmpi((jit_insn)target);
}

void *jumpForward()
{
   return jit_jmpi(jit_forward());
}

// Forward branches

void *branchForwardB(int compOp)
{
   int reg = popReg();
   switch (compOp)
   {
      case COND_EQ:
         return jit_beqi_i(jit_forward(), reg, 0);
         break;
      case COND_NE:
         return jit_bnei_i(jit_forward(), reg, 0);
         break;
   }
}

void *branchForwardI(int compOp)
{
   int r2 = popReg();
   int r3 = popReg();
   switch (compOp)
   {
      case COND_EQ:
         return jit_beqr_i(jit_forward(), r2, r3);
         break;
      case COND_NE:
         return jit_bner_i(jit_forward(), r2, r3);
         break;
      case COND_LT:
         return jit_bltr_i(jit_forward(), r2, r3);
         break;
      case COND_LE:
         return jit_bler_i(jit_forward(), r2, r3);
         break;
      case COND_GT:
         return jit_bgtr_i(jit_forward(), r2, r3);
         break;
      case COND_GE:
         return jit_bger_i(jit_forward(), r2, r3);
         break;
   }
}

void *branchForwardUI(int compOp)
{
   int r2 = popReg();
   int r3 = popReg();
   switch (compOp)
   {
      case COND_EQ:
         return jit_beqr_ui(jit_forward(), r2, r3);
         break;
      case COND_NE:
         return jit_bner_ui(jit_forward(), r2, r3);
         break;
      case COND_LT:
         return jit_bltr_ui(jit_forward(), r2, r3);
         break;
      case COND_LE:
         return jit_bler_ui(jit_forward(), r2, r3);
         break;
      case COND_GT:
         return jit_bgtr_ui(jit_forward(), r2, r3);
         break;
      case COND_GE:
         return jit_bger_ui(jit_forward(), r2, r3);
         break;
   }
}

void *branchForwardF(int compOp)
{
   int r2 = popReg();
   int r3 = popReg();
   switch (compOp)
   {
      case COND_EQ:
         return jit_beqr_f(jit_forward(), r2, r3);
         break;
      case COND_NE:
         return jit_bner_f(jit_forward(), r2, r3);
         break;
      case COND_LT:
         return jit_bltr_f(jit_forward(), r2, r3);
         break;
      case COND_LE:
         return jit_bler_f(jit_forward(), r2, r3);
         break;
      case COND_GT:
         return jit_bgtr_f(jit_forward(), r2, r3);
         break;
      case COND_GE:
         return jit_bger_f(jit_forward(), r2, r3);
         break;
   }
}

// Backward branches

void branchBackwardB(int compOp, void* target)
{
   int reg = popReg();
   switch (compOp)
   {
      case COND_EQ:
         jit_beqi_i(target, reg, 0);
         break;
      case COND_NE:
         jit_bnei_i(target, reg, 0);
         break;
   }
}

void branchBackwardI(int compOp, void* target)
{
   int r2 = popReg();
   int r3 = popReg();
   switch (compOp)
   {
      case COND_EQ:
         jit_beqr_i(target, r2, r3);
         break;
      case COND_NE:
         jit_bner_i(target, r2, r3);
         break;
      case COND_LT:
         jit_bltr_i(target, r2, r3);
         break;
      case COND_LE:
         jit_bler_i(target, r2, r3);
         break;
      case COND_GT:
         jit_bgtr_i(target, r2, r3);
         break;
      case COND_GE:
         jit_bger_i(target, r2, r3);
         break;
   }
}

void branchBackwardUI(int compOp, void* target)
{
   int r2 = popReg();
   int r3 = popReg();
   switch (compOp)
   {
      case COND_EQ:
         jit_beqr_ui(target, r2, r3);
         break;
      case COND_NE:
         jit_bner_ui(target, r2, r3);
         break;
      case COND_LT:
         jit_bltr_ui(target, r2, r3);
         break;
      case COND_LE:
         jit_bler_ui(target, r2, r3);
         break;
      case COND_GT:
         jit_bgtr_ui(target, r2, r3);
         break;
      case COND_GE:
         jit_bger_ui(target, r2, r3);
         break;
   }
}

void branchBackwardF(int compOp, void* target)
{
   int r2 = popReg();
   int r3 = popReg();
   switch (compOp)
   {
      case COND_EQ:
         jit_beqr_f(target, r2, r3);
         break;
      case COND_NE:
         jit_bner_f(target, r2, r3);
         break;
      case COND_LT:
         jit_bltr_f(target, r2, r3);
         break;
      case COND_LE:
         jit_bler_f(target, r2, r3);
         break;
      case COND_GT:
         jit_bgtr_f(target, r2, r3);
         break;
      case COND_GE:
         jit_bger_f(target, r2, r3);
         break;
   }
}

