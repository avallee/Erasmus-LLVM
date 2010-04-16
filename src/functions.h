/** \file functions.h
 * Not used in current version.
 */

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <iostream>

using namespace std;

#include "types.h"

enum FuncCodes {
   NO_OP, ASSERT_1, ASSERT_2, T2B, DECODE, ENCODE, RND, EXISTS,
   T2C,
   I2O, UI2O, UO2O,
   O2UO, I2UO, UI2UO,
   UI2I, O2I, UO2I, C2I, T2I, FLOAT_FLOOR, FLOAT_ROUND, FLOAT_CEILING,
   DEC_FLOOR, DEC_ROUND, DEC_CEILING,
   O2UI, UO2UI, I2UI,
   I2F, UI2F, O2F, UO2F, D2F, T2F,
   I2D, UI2D, O2D, UO2D, F2D, T2D,
   B2T, UO2T, O2T, I2T, UI2T, D2T, F2T, C2T,
   FMT_BW, FMT_IW, FMT_UIW, FMT_DW, FMT_FW, FMT_CW, FMT_TW,
   FMT_DW_P, FMT_FW_P,
   FRO, FWO, FRC, FWC, FROK, FWOK, FREOF, FREAD, FWRITE, LENT, LENA
};

class FuncDef
{
public:
   FuncDef(string userName, string cppName, FuncCodes code, Node res, bool conv)
   : userName(userName), cppName(cppName), code(code), res(res), conv(conv)
   {}

   void addParam(Node parType)
   {
      paramTypes.push_back(parType);
   }

   string getUserName()
   {
      return userName;
   }

   string getCppName()
   {
      return cppName;
   }

   FuncCodes getCode()
   {
      return code;
   }

   List & getParams()
   {
      return paramTypes;
   }

   int numParams()
   {
      return paramTypes.size();
   }

   Node paramType()
   {
      return paramTypes[0];
   }

   Node resultType()
   {
      return res;
   }

   bool isConversion()
   {
      return conv;
   }

   void texout(ostream & os); // Write descriptor in Latex format

   friend ostream & operator<<(ostream & os, FuncDef *pfd);

private:
   string userName;
   string cppName;
   FuncCodes code;
   vector<Node> paramTypes;
   Node res;
   bool conv;
};

extern vector<FuncDef*> funcDefs;
extern FuncDef *funIdentity;
extern FuncDef *funBottom;
extern FuncDef *funEnumText;
extern FuncDef *funIntegerEnum;

void buildFuncDefs(vector<FuncDef*> & funcDefs);

void showFuncDefs(const char *coerceFileName, const char *funFileName, vector<FuncDef*> fds);

#endif

