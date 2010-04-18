/** \file typecodes.h */

/** This file is part of the Lightning interface. */

#ifndef TYPECODES_H
#define TYPECODES_H

/** \defgroup enums Enumerations */
//\{

/** Erasmus type codes.
  * This declaration is read as "C" and therefore does not define
  * a type for the enumeration.
  */
enum
{
   TYPE_VOID,
   TYPE_BOOL,
   TYPE_CHAR,
   TYPE_TEXT,
   TYPE_BYTE,
   TYPE_UNS_BYTE,
   TYPE_INT,
   TYPE_UNS_INT,
   TYPE_DEC,
   TYPE_FLO,
   TYPE_ARR,
   TYPE_MAP
};

//\}

#endif
