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

/* Integer registers. */

#define MAXREGS 6

/** Register names. */
int regs[MAXREGS] =
{
   JIT_R0, JIT_R1, JIT_R2, JIT_V0, JIT_V1, JIT_V2
};

/** We use the registers as a stack. \a sp is the stack pointer. */
int sp;

/* Floating-point regsiters. */

#define MAXFPREGS 6

/** Register names. */
int fpRegs[MAXFPREGS] =
{
   JIT_FPR0, JIT_FPR1, JIT_FPR2, JIT_FPR3, JIT_FPR4, JIT_FPR5
};

/* Stack pointer for floating-point registers. */
int fpsp;

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

/** Return the integer register to be used when "pushing" the stack. */
int pushReg()
{
//   printf("push %d\n", sp);
   if (sp >= MAXREGS - 1)
   {
      printf("Integer register stack overflow!\n");
      return regs[0];
   }
   else
      return regs[sp++];
}

/** Return the integer register to be used when "popping" the stack. */
int popReg()
{
//   printf("pop %d\n", sp);
   if (sp <= 0)
   {
      printf("Integer register stack underflow at %x\n", jit_get_ip());
      return regs[0];
   }
   else
      return regs[--sp];
}

/** Return the floating-point register to be used when "pushing" the stack. */
int pushFPReg()
{
   printf("Push FP %d at %x\n", fpsp, jit_get_ip().ptr);
   if (fpsp >= MAXFPREGS - 1)
   {
      printf("Floating-point register stack overflow!\n");
      return fpRegs[0];
   }
   else
      return fpRegs[fpsp++];
}

/** Return the floating-point register to be used when "popping" the stack. */
int popFPReg()
{
   printf("Pop FP %d at %x\n", fpsp, jit_get_ip().ptr);
   if (fpsp <= 0)
   {
      printf("Floating-point register stack underflow at %x\n", jit_get_ip());
      return fpRegs[0];
   }
   else
      return fpRegs[--fpsp];
}

/* General purpose functions */

void *getLabel()
{
   return (void*)jit_get_label();
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
   fpsp = 0;
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

// Load literals into registers.
void literal(int typeCode, const char *value)
{
   int reg; // Needed because JIT macros use operands more than once.
   switch (typeCode)
   {
      case TYPE_BOOL:
         reg = pushReg();
         if (strcmp(value, "0") == 0)
            jit_movi_i(reg, 0);
         else if (strcmp(value, "1") == 0)
            jit_movi_i(reg, 1);
         else printf("Illegal boolean value\n");
         break;
      case TYPE_CHAR:
         reg = pushReg();
         jit_movi_i(reg, value[0]);
         break;
      case TYPE_BYTE:
         reg = pushReg();
         jit_movi_i(reg, atoi(value));
         break;
      case TYPE_UNS_BYTE:
         reg = pushReg();
         jit_movi_i(reg, atoi(value));
         break;
      case TYPE_INT:
         reg = pushReg();
         jit_movi_i(reg, atoi(value));
         break;
      case TYPE_UNS_INT:
         reg = pushReg();
         jit_movi_i(reg, atoi(value));
         break;
      case TYPE_DEC:
         reg = pushFPReg();
         jit_movi_i(reg, atof(value));
         break;
      case TYPE_FLO:
         reg = pushFPReg();
         jit_movi_i(reg, atof(value));
         break;
      case TYPE_ARR:
      case TYPE_MAP:
      case TYPE_TEXT:
      case TYPE_VOID:
      default:
         printf("Literal %d\n", typeCode);
         break;
   }
}

// Load variables

void push(int typeCode, int offset)
{
   int reg; // Must be used, because JIT arguments may be used more than once.
   switch (typeCode)
   {
      case TYPE_BOOL:
         reg = pushReg();
         jit_ldxi_i(reg, JIT_FP, offset);
         break;
      case TYPE_CHAR:
         reg = pushReg();
         jit_ldxi_ui(reg, JIT_FP, offset);
         break;
      case TYPE_BYTE:
         reg = pushReg();
         jit_ldxi_c(reg, JIT_FP, offset);
         break;
      case TYPE_UNS_BYTE:
         reg = pushReg();
         jit_ldxi_uc(reg, JIT_FP, offset);
         break;
      case TYPE_INT:
         reg = pushReg();
         jit_ldxi_i(reg, JIT_FP, offset);
         break;
      case TYPE_UNS_INT:
         reg = pushReg();
         jit_ldxi_ui(reg, JIT_FP, offset);
         break;
      case TYPE_DEC:
         reg = pushFPReg();
         jit_ldxi_d(reg, JIT_FP, offset);
         break;
      case TYPE_FLO:
         reg = pushFPReg();
         jit_ldxi_d(reg, JIT_FP, offset);
         break;
      case TYPE_TEXT:
      case TYPE_ARR:
      case TYPE_MAP:
      case TYPE_VOID:
      default:
         printf("Pushing %d\n", typeCode);
         break;
   }
}

// jit_stxi_c and jit_stxi_uc do not compile.
void pop(int typeCode, int offset)
{
   int reg; // Must be used, because JIT arguments may be used more than once.
   switch (typeCode)
   {
      case TYPE_BOOL:
         reg = popReg();
         jit_stxi_i(offset, JIT_FP, reg);
         break;
      case TYPE_CHAR:
         reg = popReg();
         jit_stxi_ui(offset, JIT_FP, reg);
         break;
      case TYPE_BYTE:
         reg = popReg();
         jit_stxi_i(offset, JIT_FP, reg);
         break;
      case TYPE_UNS_BYTE:
         reg = popReg();
         jit_stxi_ui(offset, JIT_FP, reg);
         break;
      case TYPE_INT:
         reg = popReg();
         jit_stxi_i(offset, JIT_FP, reg);
         break;
      case TYPE_UNS_INT:
         reg = popReg();
         jit_stxi_ui(offset, JIT_FP, reg);
         break;
      case TYPE_DEC:
         reg = popFPReg();
         jit_stxi_d(offset, JIT_FP, reg);
         break;
      case TYPE_FLO:
         reg = popFPReg();
         jit_stxi_d(offset, JIT_FP, reg);
         break;
      case TYPE_ARR:
      case TYPE_MAP:
      case TYPE_TEXT:
      case TYPE_VOID:
      default:
         printf("Popping %d\n", typeCode);
         break;
   }
}

void assBinop(int typeCode, int opCode)
{
   int r1, r2, r3, fpr1, fpr2, fpr3;
   switch (typeCode)
   {
      case TYPE_UNS_BYTE:
      case TYPE_UNS_INT:
         r3 = popReg();
         r2 = popReg();
         r1 = pushReg();
         switch (opCode)
         {
            case ARI_PLUS:
               jit_addr_ui(r1, r2, r3);
               break;
            case ARI_MINUS:
               jit_subr_ui(r1, r2, r3);
               break;
            case ARI_MULTIPLY:
               jit_mulr_ui(r1, r2, r3);
               break;
            case ARI_DIVIDE:
               jit_divr_ui(r1, r2, r3);
               break;
            case ARI_MOD:
               jit_modr_ui(r1, r2, r3);
               break;
         }
         break;
      case TYPE_BYTE:
      case TYPE_INT:
         r3 = popReg();
         r2 = popReg();
         r1 = pushReg();
         switch (opCode)
         {
            case ARI_PLUS:
               jit_addr_i(r1, r2, r3);
               break;
            case ARI_MINUS:
               jit_subr_i(r1, r2, r3);
               break;
            case ARI_MULTIPLY:
               jit_mulr_i(r1, r2, r3);
               break;
            case ARI_DIVIDE:
               jit_divr_i(r1, r2, r3);
               break;
            case ARI_MOD:
               jit_modr_i(r1, r2, r3);
               break;
         }
         break;
      case TYPE_VOID:
         break;
      case TYPE_BOOL:
         break;
      case TYPE_CHAR:
         break;
      case TYPE_TEXT:
         break;
      case TYPE_DEC:
      case TYPE_FLO:
         fpr3 = popFPReg();
         fpr2 = popFPReg();
         fpr1 = pushFPReg();
         switch (opCode)
         {
            case ARI_PLUS:
               jit_addr_d(fpr1, fpr2, fpr3);
               break;
            case ARI_MINUS:
               jit_subr_d(fpr1, fpr2, fpr3);
               break;
            case ARI_MULTIPLY:
               jit_mulr_d(fpr1, fpr2, fpr3);
               break;
            case ARI_DIVIDE:
               jit_divr_d(fpr1, fpr2, fpr3);
               break;
         }
         break;
      case TYPE_ARR:
         break;
      case TYPE_MAP:
         break;
   }

}

// Unconditional jumps

void jumpBackward(void *target)
{
   jit_jmpi(target);
}

void *jumpForward()
{
   return jit_jmpi(jit_forward());
}

void *branchForward(int typeCode, int compOp)
{
   int reg, r2, r3;
   switch (typeCode)
   {
      case TYPE_BOOL:
         reg = popReg();
         switch (compOp)
         {
            case COND_EQ:
               return jit_beqi_i(jit_forward(), reg, 0);
            case COND_NE:
               return jit_bnei_i(jit_forward(), reg, 0);
         }
         break;

      case TYPE_INT:
         r2 = popReg();
         r3 = popReg();
         switch (compOp)
         {
            case COND_EQ:
               return jit_beqr_i(jit_forward(), r2, r3);
            case COND_NE:
               return jit_bner_i(jit_forward(), r2, r3);
            case COND_LT:
               return jit_bltr_i(jit_forward(), r2, r3);
            case COND_LE:
               return jit_bler_i(jit_forward(), r2, r3);
            case COND_GT:
               return jit_bgtr_i(jit_forward(), r2, r3);
            case COND_GE:
               return jit_bger_i(jit_forward(), r2, r3);
         }
         break;

      case TYPE_UNS_INT:
         r2 = popReg();
         r3 = popReg();
         switch (compOp)
         {
            case COND_EQ:
               return jit_beqr_ui(jit_forward(), r2, r3);
            case COND_NE:
               return jit_bner_ui(jit_forward(), r2, r3);
            case COND_LT:
               return jit_bltr_ui(jit_forward(), r2, r3);
            case COND_LE:
               return jit_bler_ui(jit_forward(), r2, r3);
            case COND_GT:
               return jit_bgtr_ui(jit_forward(), r2, r3);
            case COND_GE:
               return jit_bger_ui(jit_forward(), r2, r3);
         }
         break;

      case TYPE_DEC:
      case TYPE_FLO:
         r2 = popFPReg();
         r3 = popFPReg();
         switch (compOp)
         {
            case COND_EQ:
               return jit_beqr_d(jit_forward(), r2, r3);
            case COND_NE:
               return jit_bner_d(jit_forward(), r2, r3);
            case COND_LT:
               return jit_bltr_d(jit_forward(), r2, r3);
            case COND_LE:
               return jit_bler_d(jit_forward(), r2, r3);
            case COND_GT:
               return jit_bgtr_d(jit_forward(), r2, r3);
            case COND_GE:
               return jit_bger_d(jit_forward(), r2, r3);
         }
         break;

      case TYPE_VOID:
      case TYPE_CHAR:
      case TYPE_TEXT:
      case TYPE_BYTE:
      case TYPE_UNS_BYTE:
      case TYPE_ARR:
      case TYPE_MAP:
      default:
         printf("Branch forward %d\n", typeCode);
         break;
   }
}

void branchBackward(int typeCode, int compOp, void* target)
{
   int reg, r2, r3;
   switch (typeCode)
   {
      case TYPE_BOOL:
         reg = popReg();
         switch (compOp)
         {
            case COND_EQ:
               jit_beqi_i(target, reg, 0);
               break;
            case COND_NE:
               jit_bnei_i(target, reg, 0);
               break;
         }
         break;

      case TYPE_INT:
         r2 = popReg();
         r3 = popReg();
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
         break;

      case TYPE_UNS_INT:
         r2 = popReg();
         r3 = popReg();
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
         break;

      case TYPE_DEC:
      case TYPE_FLO:
         r2 = popFPReg();
         r3 = popFPReg();
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
         break;

      case TYPE_BYTE:
      case TYPE_UNS_BYTE:
      case TYPE_CHAR:
      case TYPE_TEXT:
      case TYPE_ARR:
      case TYPE_MAP:
      case TYPE_VOID:
      default:
         printf("Branch backward %d\n", typeCode);
         break;
   }
}

