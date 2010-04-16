#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.64])
AC_INIT([mE Micro-Erasmus Compiler], [0.1], [grogono@encs.concordia.ca])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])
AC_CONFIG_SRCDIR([src/mec.cpp])
AC_CONFIG_HEADERS([src/config.h])

m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES])

# Checks for programs.
AC_PROG_CXX([llvm-g++ g++])
AC_PROG_CC([llvm-gcc gcc])

# Checks for libraries.
AC_LANG([C++])
PKG_CHECK_MODULES([DEPS], [glib-2.0 glibmm-2.4])
AC_SUBST([DEPS_CFLAGS])
AC_SUBST([DEPS_LIBS])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for library functions.

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
