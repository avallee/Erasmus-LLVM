# ===========================================================================
#                http://autoconf-archive.cryp.to/ax_llvm.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_LLVM([llvm-libs])
#
# DESCRIPTION
#
#   Test for the existance of llvm, and make sure that it can be linked with
#   the llvm-libs argument that is passed on to llvm-config i.e.:
#
#      llvm --libs <llvm-libs>
#
#   llvm-config will also include any libraries that are depended apon.
#
# LICENSE
#
#   Copyright (c) 2008 Andy Kitchen <agimbleinthewabe@gmail.com>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.

# LLVM_PROG_LLVM_CONFIG([MIN-VERSION])
AC_DEFUN([LLVM_PROG_LLVM_CONFIG], [
AC_ARG_VAR([LLVM_CONFIG],[path to llvm-config utility])
AS_IF([test "x$ac_cv_env_LLVM_CONFIG_set" != "xset"],
    [AC_PATH_PROG([LLVM_CONFIG], [llvm-config])])
AS_IF([test -n "$LLVM_CONFIG" && test -r "$LLVM_CONFIG"],
    [
    _llvm_min_version=m4_default([$1], [2.6])
    AC_MSG_CHECKING([that llvm-config is at least version $_llvm_min_version])
    AS_VERSION_COMPARE([$("$LLVM_CONFIG" --version)], ["$_llvm_min_version"],
        [
        AC_MSG_RESULT([no])
        LLVM_CONFIG=""
        ],
        [AC_MSG_RESULT([yes])],
        [AC_MSG_RESULT([yes])])
    ])
])# LLVM_PROG_LLVM_CONFIG


m4_define([_LLVM_ENV],[LLVM_[]m4_toupper([$1])[]])
m4_define([_LLVM_CV_VAR],[llvm_cv_[]_LLVM_ENV([$1])[]])

# _LLVM_CONFIG([COMMAND], [COMPONENTS])
# --------------------------------------
m4_define([_LLVM_CONFIG],
[
    AS_IF([test -n "$LLVM_CONFIG" && test -r "$LLVM_CONFIG"],
        [AS_IF([test -n "$_LLVM_ENV([$1])"],
            [_LLVM_CV_VAR([$1])="$_LLVM_ENV([$1])"],
            [AS_IF([AC_RUN_LOG([$LLVM_CONFIG --[]m4_tolower([$1]) [$2] >/dev/null])],
                [_LLVM_CV_VAR([$1])=$($LLVM_CONFIG --[]m4_tolower([$1]) [$2] 2>/dev/null)],
                [llvm_failed=yes])]
            )],
        [llvm_failed=untried])
])#_LLVM_CONFIG

# USE_LLVM([COMPONENT], [ACTION-IF-FOUND],
# [ACTION-IF-NOT-FOUND])
# -----------------------------------------------
AC_DEFUN([AX_USE_LLVM], [
AC_REQUIRE([LLVM_PROG_LLVM_CONFIG])dnl
AC_ARG_VAR([LLVM_CPPFLAGS], [LLVM preprocessor flags, overriding llvm-config])dnl
AC_ARG_VAR([LLVM_CFLAGS],   [LLVM C compiler flags, overriding llvm-config])dnl
AC_ARG_VAR([LLVM_CXXFLAGS], [LLVM C++ compiler flags, overriding llvm-config])dnl
AC_ARG_VAR([LLVM_LDFLAGS],  [LLVM linker flags, e.g. -L/usr/lib/llvm, overriding llvm-config])dnl
AC_ARG_VAR([LLVM_LIBS],     [LLVM library flags, e.g -lllvm, overriding llvm-config])dnl

llvm_failed=no
AC_MSG_CHECKING([for llvm[]m4_ifblank([$1],,[(components: $1)]) compiler flags])
m4_foreach_w(
    [_llvm_command], [cppflags cflags cxxflags ldflags libs], 
    [_LLVM_CONFIG(_llvm_command, m4_ifnblank([$1],[$1]))])

m4_define([_LLVM_TEXT],
[Alternatively, you may set the environment variable LLVM_CFLAGS and others to
avoid calling llvm-config.])

AS_IF([test "x$llvm_failed" = "xyes"],
#Process errors due to llvm-config not running correctly --wrongflags
    [m4_ifblank([$3],[AC_MSG_ERROR(
[
m4_ifnblank([$1],[Error finding components $1
])
Consider checking which components are being checked for.

_LLVM_TEXT])],
        [AC_MSG_RESULT([no])
        $3])],
    [test "x$llvm_failed" = "xuntried"],
#Process errors due to llvm-config not existing
    [m4_ifblank([$3],
        [AC_MSG_FAILURE(dnl
[
The llvm-config script could not be found. Make sure if is in your path or set
the LLVM_CONFIG environment variable to the full path to llvm-config.

_LLVM_TEXT

To get llvm, see <http://http://llvm.org/>.])],
        [AC_MSG_RESULT([no])
        $3])],
    [AC_MSG_RESULT([yes])])
# End flag discovery phase
m4_foreach_w(
    [_llvm_command], [cppflags cflags cxxflags ldflags libs], 
    [_LLVM_ENV(_llvm_command)=$_LLVM_CV_VAR(_llvm_command);])
# Test linking.
AC_REQUIRE([AC_PROG_CXX])
    CXXFLAGS_SAVED="$CXXFLAGS"
    CXXFLAGS="$CXXFLAGS $LLVM_CXXFLAGS"
    export CXXFLAGS

    LDFLAGS_SAVED="$LDFLAGS"
    LDFLAGS="$LDFLAGS $LLVM_LDFLAGS"
    export LDFLAGS

    LIBS_SAVED="$LIBS"
    LIBS="$LIBS $LLVM_LIBS"
    export LIBS
    AC_CACHE_CHECK(can compile with and link with llvm[]m4_if([$1],[],,([$1])),
        ax_cv_llvm,
        [AC_LANG_PUSH([C++])
        AC_LINK_IFELSE(
            AC_LANG_PROGRAM(
                [[namespace llvm{class LLVMContext;extern LLVMContext &getGlobalContext();}]],
                [[llvm::LLVMContext &C = llvm::getGlobalContext()]]),
            ax_cv_llvm=yes, ax_cv_llvm=no)
        AC_LANG_POP([C++])
        ])
    CXXFLAGS="$CXXFLAGS_SAVED"
    LDFLAGS="$LDFLAGS_SAVED"
    LIBS="$LIBS_SAVED"
    AS_IF([test "x$ax_cv_llvm" = "xyes"], [succeeded=yes])
    AS_IF([test "x$succeeded" = "xyes"],
            [AC_DEFINE([HAVE_LLVM],,[define if the llvm library is available])
            m4_ifnblank([$2],[$2])],
			[m4_ifblank([$3],[AC_MSG_ERROR(dnl
[
Could not detect the llvm libraries make sure that llvm-config is on your path.])],
                [$3])])
])dnl
