/** \file enumerations.h
 * Declarations of enumerations.
 */

#ifndef ENUMERATIONS_H
#define ENUMERATIONS_H

#include <string>

using namespace std;

/** \defgroup enums Enumerations */
//\{

/** Operators */
enum Operator
{
    LISTOP_BAR,
    LISTOP_SEQ,

    BINOP_OR,
    BINOP_AND,
    BINOP_PLUS,
    BINOP_MINUS,
    BINOP_MULTIPLY,
    BINOP_DIVIDE,
    BINOP_MOD,
    BINOP_LT,
    BINOP_LE,
    BINOP_GT,
    BINOP_GE,
    BINOP_EQ,
    BINOP_NE,
    BINOP_EXTEND,
    BINOP_CAT,

    UNOP_MINUS,
    UNOP_NOT,
    UNOP_MANY,
    UNOP_MORE,
    UNOP_OPT,
    UNOP_EXECUTE
};

/** Modes for assignment */
enum Mode
{
    DEFAULT_MODE,
    COPY,
    SHARE,
    ALIAS
};

/** Policies for select statement. */
enum Policy
{
    DEFAULT_POLICY,
    FAIR,
    ORDERED,
    RANDOM
};

/** Communication mode for a declaration or assignment. */
enum CommKind
{
    NO_COMM,
    SEND,
    RECEIVE
};

/** Fields of a protocol. */
enum MessageKind
{
    NO_FIELD,
    QUERY,
    REPLY
};

/** Varieties of a port */
enum PortKind
{
    NO_ROLE,
    CHANNEL,
    CLIENT,
    SERVER
};

/** Node kinds for abstract syntax tree. */
enum NodeKind
{
    ANY_NODE,
    ARRAY_TYPE_NODE,

    BASE_NODE,
    BINOP_NODE,
    BOOL_NODE,
    BOOL_TYPE_NODE,
    BYTE_TYPE_NODE,

    CELL_NODE,
    CHAR_NODE,
    CHAR_TYPE_NODE,
    COMP_NODE,
    CONDEXPR_NODE,
    CONDPAIR_NODE,
    CONSTANT_NODE,
    CPP_NODE,

    DECIMAL_TYPE_NODE,
    DEC_NODE,      // 080226
    DEF_NODE,
    DOT_NODE,

    ENUM_TYPE_NODE,
    ENUM_VALUE_NODE,
    ENUM_SET_NODE,
    ENUM_INIT_NODE,
    ENUM_TERM_NODE,
    ENUM_STEP_NODE,
    EXIT_NODE,

    FILE_TYPE_NODE,
    FLOAT_TYPE_NODE,
    FOR_NODE,
    FUNC_NODE,

    IF_NODE,
    INSTANCE_NODE,
    INTEGER_TYPE_NODE,
    ITERATOR_NODE,
    ITER_TYPE_NODE,

    LISTOP_NODE,
    LOOP_NODE,

    MAP_INIT_NODE,
    MAP_SET_NODE,
    MAP_STEP_NODE,
    MAP_TERM_NODE,
    MAP_TYPE_NODE,
    MATCH_NODE,

    NAME_NODE,
    NUM_NODE,

    OPTION_NODE,

    THREAD_PARAM_NODE,
    PROCEDURE_NODE,
    PROCESS_NODE,
    PROGRAM_NODE,
    PROTOCOL_NODE,

    QUERY_NODE,

    RANGE_INIT_NODE,
    RANGE_TERM_NODE,
    RANGE_NODE,
    RECEIVE_NODE,
    RECEIVE_OPTION_NODE,
    REGION_NODE,
    REMOVE_NODE,

    SELECT_NODE,
    SEND_NODE,
    SEND_OPTION_NODE,
    SEQUENCE_NODE,
    SKIP_NODE,
    START_NODE,
    STEP_NODE,
    SUBRANGE_NODE,
    SUBSCRIPT_NODE,
    SUBSCRIPT_ASSIGNMENT_NODE,

    TEXT_NODE,
    TEXT_TYPE_NODE,
    THREAD_NODE,
    THREAD_CALL_NODE,
    THREAD_START_NODE,
    THREAD_STOP_NODE,

    UNOP_NODE,

    VOID_TYPE_NODE,
};

/** Determine which part of the generated code to write. */
enum WriteMode
{
    CLOSURE_DECLARATION,
    CELL_DECLARATION,
    ACTION_BODIES,
    CONSTANT_DECLARATION,
    LOCAL_DECLARATION,
    INITIALIZER,
    ARRAY_INITIALIZER,
    CREATE_PROCESSES,
    CONVERT_POINTERS,
    VARDEC_ASS
};

/** File and system, input and output. */
enum FileMode
{
    SYS_NULL,
    SYS_IN,
    SYS_OUT,
    SYS_ERR,
    FILE_INPUT,
    FILE_OUTPUT
};

/** Iterator functions */
enum IterFun
{
    ITER_START,
    ITER_FINISH,
    ITER_KEY,
    ITER_VALUE,
    ITER_STEP
};

/** Kinds of set for any and for statements. */
enum SetKind
{
    SET_INTERVAL,
    SET_PAIR,
    SET_DOMAIN,
    SET_RANGE
};

/** Kinds of mapping type for \a for and \a any statements. */
enum MapKind
{
    NO_MAP_KIND,
    INDEXED,
    ARRAY,
    TEXT
};

/** Kinds of map set for \a for and \a any statements. */
enum MapSetKind
{
    MAPSET_UNASSIGNED,
    MAPSET_DOMAIN,
    MAPSET_RANGE,
    MAPSET_PAIR
};

//\}

/** \defgroup utils Utility functions */
//\{

/** Convert operator to the appropriate C++ string */
string opToString(Operator op);

/** Convert mode to string */
string modeToString(Mode mode);

/** Convert policy to string */
string policyToString(Policy policy);

/** Convert communication mode to string */
string commToString(CommKind c);

/** Convert message kind to string */
string messToString(MessageKind m);

/** Convert port kind to string */
string portToString(PortKind p);

/** Convert AST node kind to string */
string nodeToString(NodeKind k);

/** Convert IterFun to string */
string iterFunToString(IterFun fun);

/** Test whether an operator is a Boolean operator (OR|AND). */
bool isBoolOp(Operator op);

/** Test whether an operator is an equality comparison. */
bool isEqualOp(Operator op);

/** Test whether an operator is a comparison other than EQ or NE. */
bool isCompOp(Operator op);

/** Test whether a binary operator is numeric. */
bool isNumericOp(Operator op);

/** Convert MapKind to string */
string mapKindToString(MapKind kind);

/** Convert MapSetKind to string */
string mapSetKindToString(MapSetKind kind);

//\}

#endif
