/** \file enumerations.cpp
 * String representations for enumerations
 */

#include "enumerations.h"

string opToString(Operator op)
{
   switch (op)
   {
   case LISTOP_BAR:
      return " | ";
   case LISTOP_SEQ:
      return " ; ";

   case BINOP_OR:
      return " || ";
   case BINOP_AND:
      return " && ";
   case BINOP_PLUS:
      return " + ";
   case BINOP_MINUS:
      return " - ";
   case BINOP_MULTIPLY:
      return " * ";
   case BINOP_DIVIDE:
      return " / ";
   case BINOP_MOD:
      return " % ";
   case BINOP_EXTEND:
      return " &= ";
   case BINOP_LT:
      return " < ";
   case BINOP_LE:
      return " <= ";
   case BINOP_GT:
      return " > ";
   case BINOP_GE:
      return " >= ";
   case BINOP_EQ:
      return " == ";
   case BINOP_CAT:
      return " + ";

   case BINOP_NE:
      return " != ";
   case UNOP_MINUS:
      return " - ";
   case UNOP_NOT:
      return " ! ";
   case UNOP_MANY:
      return " * ";
   case UNOP_MORE:
      return " + ";
   case UNOP_OPT:
      return " ? ";
   case UNOP_EXECUTE:
      return " execute ";
   default:
      return "???";
   }
}

string modeToString(Mode mode)
{
   switch (mode)
   {
   case COPY:
      return "copy";
   case SHARE:
      return "share";
   case ALIAS:
      return "alias";
   default:
      return "";
   }
}

string policyToString(Policy policy)
{
   switch (policy)
   {
   case FAIR:
      return "fair";
   case ORDERED:
      return "ordered";
   case RANDOM:
      return "rand";
   default:
      return "";
   }
}

string commToString(CommKind c)
{
   switch (c)
   {
   case NO_COMM:
      return "";
   case SEND:
      return "send";
   case RECEIVE:
      return "receive";
   }
}

string messToString(MessageKind m)
{
   switch (m)
   {
   case NO_FIELD:
      return "";
   case QUERY:
      return "query";
   case REPLY:
      return "reply";
   }
}

string portToString(PortKind p)
{
   switch (p)
   {
   case NO_ROLE:
      return "";
   case CHANNEL:
      return "channel";
   case CLIENT:
      return "client";
   case SERVER:
      return "server";
   }
}

string iterFunToString(IterFun fun)
{
   switch (fun)
   {
   case ITER_START:
      return "start";
   case ITER_FINISH:
      return "finish";
   case ITER_KEY:
      return "key";
   case ITER_VALUE:
      return "value";
   case ITER_STEP:
      return "next";
   }
}

string nodeToString(NodeKind k)
{
   switch (k)
   {
   case ANY_NODE:
      return "Any";
   case ARRAY_TYPE_NODE:
      return "Array Type";

   case BASE_NODE:
      return "Base";
   case BINOP_NODE:
      return "Binop";
   case BOOL_NODE:
      return "Bool";
   case BOOL_TYPE_NODE:
      return "Bool Type";
   case BYTE_TYPE_NODE:
      return "Byte Type";

   case CPP_NODE:
      return "cpp";
   case CELL_NODE:
      return "Cell";
   case CHAR_NODE:
      return "Char";
   case CHAR_TYPE_NODE:
      return "Char Type";
   case PROCEDURE_NODE:
      return "Procedure";
   case PROCESS_NODE:
      return "Process";
   case COMP_NODE:
      return "Comprehension";
   case CONDPAIR_NODE:
      return "CondPair";
   case CONSTANT_NODE:
      return "Constant";

   case DEC_NODE:
      return "Dec";
   case DECIMAL_TYPE_NODE:
      return "Decimal Type";
   case DEF_NODE:
      return "Def";
   case DOT_NODE:
      return "Dot";

   case ENUM_TYPE_NODE:
      return "Enum Type";
   case ENUM_VALUE_NODE:
      return "Enum Value";
   case ENUM_SET_NODE:
      return "Enum Set";
   case ENUM_INIT_NODE:
      return "Enum Init";
   case ENUM_TERM_NODE:
      return "Enum Term";
   case ENUM_STEP_NODE:
      return "Enum Step";
   case EXIT_NODE:
      return "Exit";

   case FILE_TYPE_NODE:
      return "File Type";
   case FLOAT_TYPE_NODE:
      return "Float Type";
   case FOR_NODE:
      return "For";
   case FUNC_NODE:
      return "Function";

   case IF_NODE:
      return "If";
   case INSTANCE_NODE:
      return "Instance";
   case INTEGER_TYPE_NODE:
      return "Integer Type";
   case ITERATOR_NODE:
      return "Iterator";
   case ITER_TYPE_NODE:
      return "Iter Type";

   case LISTOP_NODE:
      return "Listop";
   case LOOP_NODE:
      return "Loop";

   case MAP_INIT_NODE:
      return "Map_Init";
   case MAP_SET_NODE:
      return "Map_Set";
   case MAP_STEP_NODE:
      return "Map_Step";
   case MAP_TERM_NODE:
      return "Map_Term";
   case MAP_TYPE_NODE:
      return "Map Type";
   case MATCH_NODE:
      return "Match";

   case NAME_NODE:
      return "Name";
   case NUM_NODE:
      return "Num";

   case OPTION_NODE:
      return "Option";

   case THREAD_PARAM_NODE:
      return "Parameter";
   case PROGRAM_NODE:
      return "Program";
   case PROTOCOL_NODE:
      return "Protocol";

   case QUERY_NODE:
      return "Query";

   case RANGE_NODE:
      return "Range";
   case RANGE_INIT_NODE:
      return "Range Init";
   case RANGE_TERM_NODE:
      return "Range Term";
   case RECEIVE_NODE:
      return "Receive";
   case RECEIVE_OPTION_NODE:
      return "Receive Option";
   case REMOVE_NODE:
      return "Remove";

   case SELECT_NODE:
      return "Select";
   case SEND_NODE:
      return "Send";
   case SEND_OPTION_NODE:
      return "Send Option";
   case SEQUENCE_NODE:
      return "Seq";
   case SKIP_NODE:
      return "Skip";
   case START_NODE:
      return "Start";
   case STEP_NODE:
      return "Step";
   case SUBRANGE_NODE:
      return "Subrange";
   case SUBSCRIPT_NODE:
      return "Subscript";
   case SUBSCRIPT_ASSIGNMENT_NODE:
      return "Subscript Assignment";

   case TEXT_NODE:
      return "Text";
   case TEXT_TYPE_NODE:
      return "Text Type";
   case THREAD_NODE:
      return "Thread";
   case THREAD_CALL_NODE:
      return "Thread Call";
   case THREAD_START_NODE:
      return "Thread Start";
   case THREAD_STOP_NODE:
      return "Thread Stop";

   case UNOP_NODE:
      return "Unop";

   case VOID_TYPE_NODE:
      return "Void Type";

   default:
      return "Unknown node type";
   }
}

bool isBoolOp(Operator op)
{
   return
      op == BINOP_AND ||
      op == BINOP_OR;
}

bool isEqualOp(Operator op)
{
   return
      op == BINOP_EQ ||
      op == BINOP_NE;
}

bool isCompOp(Operator op)
{
   return
      op == BINOP_LT ||
      op == BINOP_LE ||
      op == BINOP_GT ||
      op == BINOP_GE;
}

bool isNumericOp(Operator op)
{
   return
      op == BINOP_PLUS ||
      op == BINOP_MINUS ||
      op == BINOP_MULTIPLY ||
      op == BINOP_DIVIDE ||
      op == BINOP_MOD;
}

string mapKindToString(MapKind kind)
{
   switch (kind)
   {
      case NO_MAP_KIND:
         return "no kind";
      case INDEXED:
         return "indexed";
      case ARRAY:
         return "array";
      case TEXT:
         return "text";
      default:
         return "wrong value";
   }
}

string mapSetKindToString(MapSetKind kind)
{
   switch (kind)
   {
      case MAPSET_UNASSIGNED:
         return "unassigned";
      case MAPSET_DOMAIN:
         return "domain";
      case MAPSET_RANGE:
         return "range";
      case MAPSET_PAIR:
         return "pair";
      default:
         return "wrong value";
   }
}


