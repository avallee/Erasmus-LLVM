/** \file functions.cpp
 * Not used in current version.
 */

#include "ast.h"
#include "functions.h"

#include <cassert>
#include <map>
#include <fstream>
using namespace std;

/** Convert a name to 'keyword' font and escape underlines. */
string showName(string name)
{
   string result = "\\kw{";
   for (size_t i = 0; i < name.size(); ++i)
   {
      char c = name[i];
      if (c == '#' || c == '_')
         result += "\\";
      result += c;
   }
   return result + "}";
}

/** Write a description of a function in Latex table format. */
void FuncDef::texout(ostream & os)
{
   if (isConversion())
   {
      Node par = paramTypes[0];
      Node res = res;
      if (par != res)
      {
         os << left << setw(24) << showName(par->getEType()) << right <<
         " & \\rightarrow & " << showName(res->getEType()) << "\\\\\n";
      }
   }
   else
   {
      os << left << setw(24) << showName(userName) << right << " & : & ";
      bool moreThanOne = false;
      for (vector<Node>::const_iterator it = paramTypes.begin(); it != paramTypes.end(); ++it)
      {
         if (moreThanOne)
            os << ",\\; ";
         moreThanOne = true;
         os << showName((*it)->getEType());
      }
      os << " \\rightarrow " << showName(res->getEType()) << "\\\\\n";
   }
}

ostream & operator<<(ostream & os, FuncDef* pfd)
{
   os << (pfd->conv ? "Conversion " : "Function ") << pfd->userName << " (" << pfd->cppName << "). Args:";
   for (vector<Node>::const_iterator it = pfd->paramTypes.begin(); it != pfd->paramTypes.end(); ++it)
      os << ' ' << (*it)->getEType();
   return os;
}

/** Write descriptions of all functions to the given output stream. */
void showFuncDefs(char *coerceFileName, char *funFileName, vector<FuncDef*> fds)
{
   ofstream cos(coerceFileName);
   cos << "\\begin{eqnarray*}\n";
   for (vector<FuncDef*>::const_iterator it = fds.begin() + 1; it != fds.end(); ++it)
      if ((*it)->isConversion())
         (*it)->texout(cos);
   cos << "\\end{eqnarray*}\n";
   cos.close();

   ofstream fns(funFileName);
   fns << "\\begin{eqnarray*}\n";
   for (vector<FuncDef*>::const_iterator it = fds.begin() + 1; it != fds.end(); ++it)
      if ( ! (*it)->isConversion())
         (*it)->texout(fns);
   fns << "\\end{eqnarray*}\n";
   fns.close();
}

void buildFuncDefs(vector<FuncDef*> & funcDefs)
{

   // Assert ----------------------------------------------------------------------

   FuncDef *funAssert1 = new FuncDef("assert", "assert_1", ASSERT_1, BaseNode::theVoidNode, false);
   funAssert1->addParam(BaseNode::theBoolNode);
   funcDefs.push_back(funAssert1);

   FuncDef *funAssert2 = new FuncDef("assert", "assert_2", ASSERT_2, BaseNode::theVoidNode, false);
   funAssert2->addParam(BaseNode::theBoolNode);
   funAssert2->addParam(BaseNode::theTextNode);
   funcDefs.push_back(funAssert2);

   // Bool-------------------------------------------------------------------------

   FuncDef *funBoolb = new FuncDef("bool", "", NO_OP, BaseNode::theBoolNode, true);
   funBoolb->addParam(BaseNode::theBoolNode);
   funcDefs.push_back(funBoolb);

   FuncDef *funBoolt = new FuncDef("bool", "string2bool", T2B, BaseNode::theBoolNode, false);
   funBoolt->addParam(BaseNode::theTextNode);
   funcDefs.push_back(funBoolt);

   // Char-------------------------------------------------------------------------

   FuncDef *funCharc = new FuncDef("char", "", NO_OP, BaseNode::theCharNode, true);
   funCharc->addParam(BaseNode::theCharNode);
   funcDefs.push_back(funCharc);

   FuncDef *funEncode = new FuncDef("encode", "encode2char", ENCODE, BaseNode::theCharNode, false);
   funEncode->addParam(BaseNode::theIntegerNode);
   funcDefs.push_back(funEncode);

   FuncDef *funChart = new FuncDef("char", "string2char", T2C, BaseNode::theCharNode, true);
   funChart->addParam(BaseNode::theTextNode);
   funcDefs.push_back(funChart);

   // Byte ------------------------------------------------------------------------

   FuncDef *funByteb = new FuncDef("byte", "", NO_OP, BaseNode::theByteNode, true);
   funByteb->addParam(BaseNode::theByteNode);
   funcDefs.push_back(funByteb);

   FuncDef *funBytei = new FuncDef("byte", "int2byte", I2O, BaseNode::theByteNode, true);
   funBytei->addParam(BaseNode::theIntegerNode);
   funcDefs.push_back(funBytei);

   FuncDef *funByteui = new FuncDef("byte", "uint2byte", UI2O, BaseNode::theByteNode, true);
   funByteui->addParam(BaseNode::theUnsignedIntegerNode);
   funcDefs.push_back(funByteui);

   FuncDef *funByteub = new FuncDef("byte", "ubyte2byte", UO2O, BaseNode::theByteNode, true);
   funByteub->addParam(BaseNode::theUnsignedByteNode);
   funcDefs.push_back(funByteub);

   // Unsigned Byte ---------------------------------------------------------------

   FuncDef *funUByteub = new FuncDef("byte", "", NO_OP, BaseNode::theUnsignedByteNode, true);
   funUByteub->addParam(BaseNode::theUnsignedByteNode);
   funcDefs.push_back(funUByteub);

   FuncDef *funUByteb = new FuncDef("byte", "byte2ubyte", I2O, BaseNode::theUnsignedByteNode, true);
   funUByteb->addParam(BaseNode::theByteNode);
   funcDefs.push_back(funUByteb);

   FuncDef *funUBytei = new FuncDef("byte", "int2ubyte", I2UO, BaseNode::theUnsignedByteNode, true);
   funUBytei->addParam(BaseNode::theIntegerNode);
   funcDefs.push_back(funUBytei);

   FuncDef *funUByteui = new FuncDef("byte", "uint2ubyte", UI2UO, BaseNode::theUnsignedByteNode, true);
   funUByteui->addParam(BaseNode::theUnsignedIntegerNode);
   funcDefs.push_back(funUByteui);

   // Integer----------------------------------------------------------------------

   FuncDef *funInti = new FuncDef("int", "", NO_OP, BaseNode::theIntegerNode, true);
   funInti->addParam(BaseNode::theIntegerNode);
   funcDefs.push_back(funInti);

   FuncDef *funIntui = new FuncDef("int", "", UI2I, BaseNode::theIntegerNode, true);
   funIntui->addParam(BaseNode::theUnsignedIntegerNode);
   funcDefs.push_back(funIntui);

   FuncDef *funIntb = new FuncDef("int", "", O2I, BaseNode::theIntegerNode, true);
   funIntb->addParam(BaseNode::theByteNode);
   funcDefs.push_back(funIntb);

   FuncDef *funIntub = new FuncDef("int", "", UO2I, BaseNode::theIntegerNode, true);
   funIntub->addParam(BaseNode::theUnsignedByteNode);
   funcDefs.push_back(funIntub);

   FuncDef *funFloatFloor = new FuncDef("floor", "double2floor", FLOAT_FLOOR, BaseNode::theIntegerNode, false);
   funFloatFloor->addParam(BaseNode::theFloatNode);
   funcDefs.push_back(funFloatFloor);

   FuncDef *funFloatRound = new FuncDef("round", "double2round", FLOAT_ROUND, BaseNode::theIntegerNode, false);
   funFloatRound->addParam(BaseNode::theFloatNode);
   funcDefs.push_back(funFloatRound);

   FuncDef *funFloatCeiling = new FuncDef("ceiling", "double2ceiling", FLOAT_CEILING, BaseNode::theIntegerNode, false);
   funFloatCeiling->addParam(BaseNode::theFloatNode);
   funcDefs.push_back(funFloatCeiling);

   FuncDef *funDecimalFloor = new FuncDef("floor", "double2floor", DEC_FLOOR, BaseNode::theIntegerNode, false);
   funDecimalFloor->addParam(BaseNode::theDecimalNode);
   funcDefs.push_back(funDecimalFloor);

   FuncDef *funDecimalRound = new FuncDef("round", "double2round", DEC_ROUND, BaseNode::theIntegerNode, false);
   funDecimalRound->addParam(BaseNode::theDecimalNode);
   funcDefs.push_back(funDecimalRound);

   FuncDef *funDecimalCeiling = new FuncDef("ceiling", "double2ceiling", DEC_CEILING, BaseNode::theIntegerNode, false);
   funDecimalCeiling->addParam(BaseNode::theDecimalNode);
   funcDefs.push_back(funDecimalCeiling);

   FuncDef *funIntc = new FuncDef("int", "char2int", C2I, BaseNode::theIntegerNode, false);
   funIntc->addParam(BaseNode::theCharNode);
   funcDefs.push_back(funIntc);

   FuncDef *funDecode = new FuncDef("decode", "char2decode", DECODE, BaseNode::theIntegerNode, false);
   funDecode->addParam(BaseNode::theCharNode);
   funcDefs.push_back(funDecode);

   FuncDef *funIntt = new FuncDef("int", "string2int", T2I, BaseNode::theIntegerNode, false);
   funIntt->addParam(BaseNode::theTextNode);
   funcDefs.push_back(funIntt);

   FuncDef *funInte = new FuncDef("int", "", NO_OP, BaseNode::theIntegerNode, true);
   funInte->addParam(BaseNode::theEnumNode);
   funcDefs.push_back(funInte);

   // Unsigned Integer ---------------------------------------------------------------

   FuncDef *funUInteger = new FuncDef("int", "", NO_OP, BaseNode::theUnsignedIntegerNode, true);
   funUInteger->addParam(BaseNode::theUnsignedIntegerNode);
   funcDefs.push_back(funUInteger);

   FuncDef *funUIntegerb = new FuncDef("int", "byte2uint", O2UI, BaseNode::theUnsignedIntegerNode, true);
   funUIntegerb->addParam(BaseNode::theByteNode);
   funcDefs.push_back(funUIntegerb);

   FuncDef *funUIntegerub = new FuncDef("int", "ubyte2uint", UO2UI, BaseNode::theUnsignedIntegerNode, true);
   funUIntegerub->addParam(BaseNode::theUnsignedByteNode);
   funcDefs.push_back(funUIntegerub);

   FuncDef *funUIntegeri = new FuncDef("int", "int2uint", I2UI, BaseNode::theUnsignedIntegerNode, true);
   funUIntegeri->addParam(BaseNode::theIntegerNode);
   funcDefs.push_back(funUIntegeri);

   // Float------------------------------------------------------------------------

   FuncDef *funFloatf = new FuncDef("float", "", NO_OP, BaseNode::theFloatNode, true);
   funFloatf->addParam(BaseNode::theFloatNode);
   funcDefs.push_back(funFloatf);

   FuncDef *funFloati = new FuncDef("float", "double", I2F, BaseNode::theFloatNode, true);
   funFloati->addParam(BaseNode::theIntegerNode);
   funcDefs.push_back(funFloati);

   FuncDef *funFloatui = new FuncDef("float", "double", UI2F, BaseNode::theFloatNode, true);
   funFloatui->addParam(BaseNode::theUnsignedIntegerNode);
   funcDefs.push_back(funFloatui);

   FuncDef *funFloatb = new FuncDef("float", "double", O2F, BaseNode::theFloatNode, true);
   funFloatb->addParam(BaseNode::theByteNode);
   funcDefs.push_back(funFloatb);

   FuncDef *funFloatub = new FuncDef("float", "double", UO2F, BaseNode::theFloatNode, true);
   funFloatub->addParam(BaseNode::theUnsignedByteNode);
   funcDefs.push_back(funFloatub);

   FuncDef *funFloatd = new FuncDef("float", "", D2F, BaseNode::theFloatNode, true);
   funFloatd->addParam(BaseNode::theDecimalNode);
   funcDefs.push_back(funFloatd);

   FuncDef *funFloatt = new FuncDef("float", "string2double", T2F, BaseNode::theFloatNode, false);
   funFloatt->addParam(BaseNode::theTextNode);
   funcDefs.push_back(funFloatt);

   // Decimal----------------------------------------------------------------------

   FuncDef *funDecimald = new FuncDef("decimal", "", NO_OP, BaseNode::theDecimalNode, true);
   funDecimald->addParam(BaseNode::theDecimalNode);
   funcDefs.push_back(funDecimald);

   FuncDef *funDecimalf = new FuncDef("decimal", "", F2D, BaseNode::theDecimalNode, false);
   funDecimalf->addParam(BaseNode::theFloatNode);
   funcDefs.push_back(funDecimalf);

   FuncDef *funDecimalb = new FuncDef("decimal", "double", O2D, BaseNode::theDecimalNode, true);
   funDecimalb->addParam(BaseNode::theByteNode);
   funcDefs.push_back(funDecimalb);

   FuncDef *funDecimalub = new FuncDef("decimal", "double", UO2D, BaseNode::theDecimalNode, true);
   funDecimalub->addParam(BaseNode::theUnsignedByteNode);
   funcDefs.push_back(funDecimalub);

   FuncDef *funDecimali = new FuncDef("decimal", "double", I2D, BaseNode::theDecimalNode, true);
   funDecimali->addParam(BaseNode::theIntegerNode);
   funcDefs.push_back(funDecimali);

   FuncDef *funDecimalui = new FuncDef("decimal", "double", UI2D, BaseNode::theDecimalNode, true);
   funDecimalui->addParam(BaseNode::theUnsignedIntegerNode);
   funcDefs.push_back(funDecimalui);

   FuncDef *funDecimalt = new FuncDef("decimal", "string2double", T2D, BaseNode::theDecimalNode, false);
   funDecimalt->addParam(BaseNode::theTextNode);
   funcDefs.push_back(funDecimalt);

   // Text-------------------------------------------------------------------------

   FuncDef *funTextt = new FuncDef("text", "", NO_OP, BaseNode::theTextNode, true);
   funTextt->addParam(BaseNode::theTextNode);
   funcDefs.push_back(funTextt);

   FuncDef *funTextb = new FuncDef("text", "bool2string1", B2T, BaseNode::theTextNode, true);
   funTextb->addParam(BaseNode::theBoolNode);
   funcDefs.push_back(funTextb);

   FuncDef *funTextou = new FuncDef("text", "ubyte2string1", UO2T, BaseNode::theTextNode, true);
   funTextou->addParam(BaseNode::theUnsignedByteNode);
   funcDefs.push_back(funTextou);

   FuncDef *funTexty = new FuncDef("text", "byte2string1", O2T, BaseNode::theTextNode, true);
   funTexty->addParam(BaseNode::theByteNode);
   funcDefs.push_back(funTexty);

   FuncDef *funTexti = new FuncDef("text", "int2string1", I2T, BaseNode::theTextNode, true);
   funTexti->addParam(BaseNode::theIntegerNode);
   funcDefs.push_back(funTexti);

   FuncDef *funTextui = new FuncDef("text", "uint2string1", UI2T, BaseNode::theTextNode, true);
   funTextui->addParam(BaseNode::theUnsignedIntegerNode);
   funcDefs.push_back(funTextui);

   FuncDef *funTextd = new FuncDef("text", "double2string1", D2T, BaseNode::theTextNode, true);
   funTextd->addParam(BaseNode::theDecimalNode);
   funcDefs.push_back(funTextd);

   FuncDef *funTextf = new FuncDef("text", "double2string1", F2T, BaseNode::theTextNode, true);
   funTextf->addParam(BaseNode::theFloatNode);
   funcDefs.push_back(funTextf);

   FuncDef *funTextc = new FuncDef("text", "char2string1", C2T, BaseNode::theTextNode, true);
   funTextc->addParam(BaseNode::theCharNode);
   funcDefs.push_back(funTextc);

   // Format (width) --------------------------------------------------------------

   FuncDef *funFormatb = new FuncDef("format", "bool2string2", FMT_BW, BaseNode::theTextNode, false);
   funFormatb->addParam(BaseNode::theBoolNode);
   funFormatb->addParam(BaseNode::theIntegerNode);
   funcDefs.push_back(funFormatb);

   FuncDef *funFormati = new FuncDef("format", "int2string2", FMT_IW, BaseNode::theTextNode, false);
   funFormati->addParam(BaseNode::theIntegerNode);
   funFormati->addParam(BaseNode::theIntegerNode);
   funcDefs.push_back(funFormati);

   FuncDef *funFormatui = new FuncDef("format", "uint2string2", FMT_UIW, BaseNode::theTextNode, false);
   funFormatui->addParam(BaseNode::theUnsignedIntegerNode);
   funFormatui->addParam(BaseNode::theIntegerNode);
   funcDefs.push_back(funFormatui);

   FuncDef *funFormatd = new FuncDef("format", "double2string2", FMT_DW, BaseNode::theTextNode, false);
   funFormatd->addParam(BaseNode::theDecimalNode);
   funFormatd->addParam(BaseNode::theIntegerNode);
   funcDefs.push_back(funFormatd);

   FuncDef *funFormatf = new FuncDef("format", "double2string2", FMT_FW, BaseNode::theTextNode, false);
   funFormatf->addParam(BaseNode::theFloatNode);
   funFormatf->addParam(BaseNode::theIntegerNode);
   funcDefs.push_back(funFormatf);

   FuncDef *funFormatc = new FuncDef("format", "char2string2", FMT_CW, BaseNode::theTextNode, false);
   funFormatc->addParam(BaseNode::theCharNode);
   funFormatc->addParam(BaseNode::theIntegerNode);
   funcDefs.push_back(funFormatc);

   FuncDef *funFormatt = new FuncDef("format", "string2string2", FMT_TW, BaseNode::theTextNode, false);
   funFormatt->addParam(BaseNode::theTextNode);
   funFormatt->addParam(BaseNode::theIntegerNode);
   funcDefs.push_back(funFormatt);

   // Format (width and precision)-------------------------------------------------

   FuncDef *funFormatdp = new FuncDef("format", "double2string3", FMT_DW_P, BaseNode::theTextNode, false);
   funFormatdp->addParam(BaseNode::theDecimalNode);
   funFormatdp->addParam(BaseNode::theIntegerNode);
   funFormatdp->addParam(BaseNode::theIntegerNode);
   funcDefs.push_back(funFormatdp);

   FuncDef *funFormatfp = new FuncDef("format", "double2string3", FMT_FW_P, BaseNode::theTextNode, false);
   funFormatfp->addParam(BaseNode::theFloatNode);
   funFormatfp->addParam(BaseNode::theIntegerNode);
   funFormatfp->addParam(BaseNode::theIntegerNode);
   funcDefs.push_back(funFormatfp);

   // Random number----------------------------------------------------------------

   FuncDef *funRandi = new FuncDef("rand", "random", RND, BaseNode::theIntegerNode, false);
   funRandi->addParam(BaseNode::theIntegerNode);
   funcDefs.push_back(funRandi);

   // Files------------------------------------------------------------------------

   FuncDef *funFileOpenr = new FuncDef("file_open_read", "file_open", FRO, BaseNode::theInputFileNode, false);
   funFileOpenr->addParam(BaseNode::theTextNode);
   funcDefs.push_back(funFileOpenr);

   FuncDef *funFileOpenw = new FuncDef("file_open_write", "file_open", FWO, BaseNode::theOutputFileNode, false);
   funFileOpenw->addParam(BaseNode::theTextNode);
   funcDefs.push_back(funFileOpenw);

   FuncDef *funFileCloseIn = new FuncDef("file_close", "file_close", FRC, BaseNode::theVoidNode, false);
   funFileCloseIn->addParam(BaseNode::theInputFileNode);
   funcDefs.push_back(funFileCloseIn);

   FuncDef *funFileCloseOut = new FuncDef("file_close", "file_close", FWC, BaseNode::theVoidNode, false);
   funFileCloseOut->addParam(BaseNode::theOutputFileNode);
   funcDefs.push_back(funFileCloseOut);


   FuncDef *funFileOKIn = new FuncDef("file_ok", "file_ok", FROK, BaseNode::theBoolNode, false);
   funFileOKIn->addParam(BaseNode::theInputFileNode);
   funcDefs.push_back(funFileOKIn);

   FuncDef *funFileOKOut = new FuncDef("file_ok", "file_ok", FWOK, BaseNode::theBoolNode, false);
   funFileOKOut->addParam(BaseNode::theOutputFileNode);
   funcDefs.push_back(funFileOKOut);


   FuncDef *funFileEof = new FuncDef("file_eof", "file_eof", FREOF, BaseNode::theBoolNode, false);
   funFileEof->addParam(BaseNode::theInputFileNode);
   funcDefs.push_back(funFileEof);


   FuncDef *funFileRead = new FuncDef("file_read", "file_read", FREAD, BaseNode::theTextNode, false);
   funFileRead->addParam(BaseNode::theInputFileNode);
   funcDefs.push_back(funFileRead);


   FuncDef *funFileWrite = new FuncDef("file_write", "file_write", FWRITE, BaseNode::theVoidNode, false);
   funFileWrite->addParam(BaseNode::theOutputFileNode);
   funFileWrite->addParam(BaseNode::theTextNode);
   funcDefs.push_back(funFileWrite);

   // Length of Text---------------------------------------------------------------

   FuncDef *funLent = new FuncDef("#", "stringlen", LENT, BaseNode::theIntegerNode, false);
   funLent->addParam(BaseNode::theTextNode);
   funcDefs.push_back(funLent);

// Size of array----------------------------------------------------------------

   FuncDef *funArrSize = new FuncDef("#", "size", LENA, BaseNode::theIntegerNode, false);
   funArrSize->addParam(BaseNode::theArrayNode);
   funcDefs.push_back(funArrSize);

// Special functions - not in the table.
   funIdentity = new FuncDef("id", "", NO_OP, BaseNode::theVoidNode, false);
   funBottom = new FuncDef("bot", "", NO_OP, BaseNode::theVoidNode, false);
   funEnumText = new FuncDef("enumText", "", NO_OP, BaseNode::theVoidNode, false);
   funIntegerEnum = new FuncDef("intEnum", "", NO_OP, BaseNode::theVoidNode, false);
}


// Normal conversions and functions
vector<FuncDef*> funcDefs;

// The identity function, used to mean "no function needed".
FuncDef *funIdentity;

// The bottom function, used to mean "no function available".
FuncDef *funBottom;

// A special function, used internally to generate strings from enumerations.
FuncDef *funEnumText;

// A special function, used internally to generate call to a run-time check function.
FuncDef *funIntegerEnum;



