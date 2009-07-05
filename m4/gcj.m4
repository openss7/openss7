# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: gcj.m4,v $ $Name:  $($Revision: 1.1.2.4 $) $Date: 2009-07-05 12:04:27 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
# Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>
#
# All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU Affero General Public License as published by the Free
# Software Foundation; version 3 of the License.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
# details.
#
# You should have received a copy of the GNU Affero General Public License along
# with this program.  If not, see <http://www.gnu.org/licenses/>, or write to
# the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#
# -----------------------------------------------------------------------------
#
# U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
# behalf of the U.S. Government ("Government"), the following provisions apply
# to you.  If the Software is supplied by the Department of Defense ("DoD"), it
# is classified as "Commercial Computer Software" under paragraph 252.227-7014
# of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
# successor regulations) and the Government is acquiring only the license rights
# granted herein (the license rights customarily provided to non-Government
# users).  If the Software is supplied to any unit or agency of the Government
# other than DoD, it is classified as "Restricted Computer Software" and the
# Government's rights in the Software are defined in paragraph 52.227-19 of the
# Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
# the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
# (or any successor regulations).
#
# -----------------------------------------------------------------------------
#
# Commercial licensing and support of this software is available from OpenSS7
# Corporation at a fee.  See http://www.openss7.com/
#
# -----------------------------------------------------------------------------
#
# Last Modified $Date: 2009-07-05 12:04:27 $ by $Author: brian $
#
# =============================================================================

## ----------------------- ##
## 1. Language selection.  ##
## ----------------------- ##

# ----------------------- #
# 1a. The Java language.  #
# ----------------------- #

# AC_LANG(Java)
# -------------
m4_define([AC_LANG(Java)],
[ac_ext=java
ac_cpp=
ac_compile='$GCJ -c $GCJFLAGS conftest.$ac_ext >&AS_MESSAGE_LOG_FD'
ac_link='$GCJ -o conftest$ac_exeext $GCJFLAGS $LDFLAGS conftest.$ac_ext $LIBS >&AS_MESSAGE_LOG_FD'
ac_compiler_gnu=yes
])

# AC_LANG_JAVA
# ------------
AU_DEFUN([AC_LANG_JAVA], [AC_LANG(Java)])

# _AC_LANG_ABBREV(Java)
# ---------------------
m4_define([_AC_LANG_ABBREV(Java)], [gcj])

# _AC_LANG_PREFIX(Java)
# ---------------------
m4_define([_AC_LANG_PREFIX(Java)], [GCJ])

## ------------------------ ##
## 2. Producting programs.  ##
## ------------------------ ##

# ------------------ #
# 2a. Java sources.  #
# ------------------ #

# AC_LANG_SOURCE(Java)(BODY)
# --------------------------
m4_define([AC_LANG_SOURCE(Java)],
[// Java language source.
package conftest;
$1])

# AC_LANG_PROGRAM(Java)([PROLOGUE], [BODY])
# -----------------------------------------
m4_define([AC_LANG_PROGRAM(Java)],
[$1

public class conftest {
    public static void main(String[] args) {
$2
  ;
    }
}])

# AC_LANG_CALL(Java)(PROLOGUE, FUNCTION)
# --------------------------------------
m4_define([AC_LANG_CALL(Java)],
[AC_LANG_PROGRAM([$1
m4_if([$2], [main], ,
int $2 ();]), [return $2 ();])])

# AC_LANG_FUNC_LINK_TRY(Java)(FUNCTION)
# -------------------------------------
m4_define([AC_LANG_FUNC_LINK_TRY(Java)],
[AC_LANG_PROGRAM(
[// no imports

int $1 ();
], [return $1 ();])])

# AC_LANG_BOOL_COMPILE_TRY(Java)(PROLOGUE, EXPRESSION)
# ----------------------------------------------------
m4_define([AC_LANG_BOOL_COMPILE_TRY(Java)],
[AC_LANG_PROGRAM([$1], [static int test_array @<:@1 - 2 * ((($2) != 0) ? 1 : 0)@:>@:
test_array @<:@0@:>@ = 0
])])

## -------------------------------------------- ##
## 3. Looking for Compilers and Preprocessors.  ##
## -------------------------------------------- ##

# ----------------------- #
# 3a. The Java compiler.  #
# ----------------------- #

# AC_LANG_PREPROC(Java)
# ---------------------
# Find the proprocessor.
m4_define([AC_LANG_PREPROC(Java)],
[AC_REQUIRE([AC_PROG_CPP])])

# AC_LANG_COMPILER(Java)
# ----------------------
# Find the Java compiler.
m4_define([AC_LANG_COMPILER(Java)],
[AC_REQUIRE([AC_PROG_GCJ])])

# AC_PROG_GCJ([COMPILER ...])
# ---------------------------
# COMPILER ... is a space separated list of Java compilers to
# serach for.  This just gives the user an opportunity to
# specify an alternative search list for the Java compiler.
AN_MAKEVAR([GCJ], [AC_PROG_GCJ])
AN_MAKEVAR([gcj], [AC_PROG_GCJ])
AN_PROGRAM([gcj], [AC_PROG_GCJ])
AC_DEFUN([AC_PROG_GCJ],
[AC_LANG_PUSH(Java)dnl
AC_ARG_VAR([GCJ], [Java compiler command])dnl
AC_ARG_VAR([GCJFLAGS], [Java compiler flags])dnl
_AC_ARG_VAR_LDFLAGS()dnl
_AC_ARG_VAR_LIBS()dnl
_AC_ARG_VAR_CPPFLAGS()dnl
m4_ifval([$1],
      [AC_CHECK_TOOLS(GCJ, [$1])],
[AC_CHECK_TOOLS(GCJ, [gcj3 gcj])
if test -z "$GCJ"; then
    dnl Here we want:
    dnl   AC_CHECK_TOOL(GCJ, gcj)
    dnl but without the check for a tool without the prefix.
    dnl Until the check is removed from there, copy the code:
    if test -n "$ac_tool_prefix"; then
	AC_CHECK_PROGS(GCJ, [${ac_tool_prefix}gcj3 ${ac_tool_prefix}gcj], [${ac_tool_prefix}gcj])
    fi # gcj.m4 181
fi # gcj.m4 182
if test -z "$GCJ" ; then
    AC_CHECK_PROG(GCJ, gcj, gcj, , , /usr/ucb/gcj)
fi # gcj.m4 185
if test -z "$GCJ"; then
    AC_CHECK_TOOLS(GCJ, cl.exe)
fi # gcj.m4 188
])

test -z "$GCJ" && AC_MSG_FAILURE([no acceptable GCJ compiler found in \$PATH])

# Provide some information about the compiler
_AS_ECHO_LOG([checking for _AC_LANG compiler version])
set X $ac_compile
ac_compiler=$[2]
_AC_DO([$ac_compiler --version >&AS_MESSAGE_LOG_FD])
_AC_DO([$ac_compiler -v >&AS_MESSAGE_LOG_FD])
_AC_DO([$ac_compiler -V >&AS_MESSAGE_LOG_FD])

m4_expand_once([_AC_COMPILER_EXEEXT])[]dnl
m4_expand_once([_AC_COMPILER_OBJEXT])[]dnl
_AC_LANG_COMPILER_GNU
_AC_PROG_GCJ_G
AC_LANG_POP(Java)dnl
])

# AC_PROG_JAVAC([LIST-OF-COMPILERS])
# ----------------------------------
# LIST-OF-COMPILERS is a space spearated list of Java class
# compilers to search (if not specified, a default list is
# used).  This just gives the user an opportunity to specify an
# alternative search list for the Java class compiler.
# gcj	    - GNU compiler Java front-end
# javac	    - Java SDK class compiler
# ??? others ???
AN_MAKEVAR([JAVAC], [AC_PROG_JAVAC])
AN_PROGRAM([gcj], [AC_PROG_JAVAC])
AN_PROGRAM([javac], [AC_PROG_JAVAC])
AC_DEFUN([AC_PROG_JAVAC],
[AC_REQUIRE([AC_PROG_GCJ])
AC_LANG_PUSH(Java)dnl
AC_ARG_VAR([JAVAC], [Java class compiler])dnl
AC_ARG_VAR([JAVACFLAGS], [Java class compiler flags])dnl
if test -z "$JAVAC"; then
    AC_CHECK_TOOLS(JAVAC,
	[m4_default([$1], [gcj javac])],
	gcj)
fi # gcj.m4 228
# Provide some information about the compiler
_AS_ECHO_LOG([checking for Javac compiler version])
ac_compiler=$JAVAC
_AC_DO([$ac_compiler --version >&AS_MESSAGE_LOG_FD])
_AC_DO([$ac_compiler -version >&AS_MESSAGE_LOG_FD])
_AC_DO([$ac_compiler -v >&AS_MESSAGE_LOG_FD])
_AC_DO([$ac_compiler -V >&AS_MESSAGE_LOG_FD])
AC_LANG_POP(Java)dnl
])

# AC_PROG_GCJH([COMPILER ...])
# ----------------------------
# COMPILER ... is a space speparated list of CNI header
# compilers to search.  This just gives the user an opportunity
# to specify an alternative search list for the CNI header
# compiler.
AN_MAKEVAR([GCJH], [AC_PROG_GCJH])
AN_PROGRAM([gcjh3], [AC_PROG_GCJH])
AN_PROGRAM([gcjh], [AC_PROG_GCJH])
AC_DEFUN([AC_PROG_GCJH],
[AC_LANG_PUSH(Java)dnl
AC_ARG_VAR([GCJH], [Java CNI header command])dnl
AC_ARG_VAR([GCJHFLAGS], [Java CNI header flags])dnl
if test -z "$GCJH"; then
    AC_CHECK_TOOLS(GCJH,
	[m4_default([$1], [gcjh3 gcjh])],
	gcjh)
fi # gcj.m4 255
# Provide some information about the compiler
_AS_ECHO_LOG([checking for Javah compiler version])
ac_compiler=$GCJH
_AC_DO([$ac_compiler --version >&AS_MESSAGE_LOG_FD])
_AC_DO([$ac_compiler -v >&AS_MESSAGE_LOG_FD])
_AC_DO([$ac_compiler -V >&AS_MESSAGE_LOG_FD])
AC_LANG_POP(Java)dnl
])

# AC_PROG_JAVAH([LIST-OF-COMPILERS])
# ----------------------------------
# LIST-OF-COMPILERS is a space separated list of Java JNI
# header compilers to search (if not specified, a default list
# is used).  This just gives the user an opportunity to specify
# an alternative search list for the Java JNI header compiler.
# gcjh	    - GNU JNI header compiler
# gcjnih    - GNU JNI header compiler
# javah	    - Java SDK JNI header compiler
AN_MAKEVAR([JAVAH], [AC_PROG_JAVAH])
AN_PROGRAM([gcjh], [AC_PROG_JAVAH])
AN_PROGRAM([gcjnih], [AC_PROG_JAVAH])
AN_PROGRAM([javah], [AC_PROG_JAVAH])
AC_DEFUN([AC_PROG_JAVAH],
[AC_REQUIRE([AC_PROG_GCJH])
AC_LANG_PUSH(Java)dnl
AC_ARG_VAR([JAVAH], [Java JNI header command])dnl
AC_ARG_VAR([JAVAHFLAGS], [Java JNI header flags])dnl
if test -z "$JAVAH"; then
    AC_CHECK_TOOLS(JAVAH,
	[m4_default([$1], [$GCJH gcjh3 gcjh gcjnih javah])],
	gcjh)
fi # gcj.m4 286
# Provide some information about the compiler
_AS_ECHO_LOG([checking for Javah compiler version])
ac_compiler=$JAVAH
_AC_DO([$ac_compiler --version >&AS_MESSAGE_LOG_FD])
_AC_DO([$ac_compiler -version >&AS_MESSAGE_LOG_FD])
_AC_DO([$ac_compiler -v >&AS_MESSAGE_LOG_FD])
_AC_DO([$ac_compiler -V >&AS_MESSAGE_LOG_FD])
AC_LANG_POP(Java)dnl
])

# _AC_PROG_GCJ_G
# --------------
# Check whether -g works, even if GCJFLAGS is set, in case the package
# plays around with GCJFLAGS (such as to build both debuggin and normal
# versions of a library), tasteless as that idea is.
# Don't consider -g to work if it generates warnings when plan compiles don't.
m4_define([_AC_PROG_GCJ_G],
[ac_test_GCJFLAGS=${GCJFLAGS+set}
ac_save_GCJFLAGS=$GCJFLAGS
AC_CACHE_CHECK([whether $GCJ accepts -g], [ac_cv_prog_gcj_g],
    [ac_save_gcj_werror_flag=$ac_gcj_werror_flag
     ac_gcj_werror_flag=yes
     ac_cv_prog_gcj_g=no
     GCJFLAGS="-g"
     _AC_COMPILE_IFELSE([AC_LANG_PROGRAM()],
       [ac_cv_prog_gcj_g=yes],
       [GCJFLAGS=""
	_AC_COMPILE_IFELSE([AC_LANG_PROGRAM()],
	  [],
	  [ac_gcj_werror_flag=$ac_save_gcj_werror_flag
	   GCJFLAGS="-g"
	   _AC_COMPILE_IFELSE([AC_LANG_PROGRAM()],
	     [ac_cv_prog_gcj_g=yes])])])
     ac_gcj_werror_flag=$ac_save_gcj_werror_flag])
if test "$ac_test_GCJFLAGS" = set; then
    GCJFLAGS=$ac_save_GCJFLAGS
elif test $ac_cv_prog_gcj_g = yes; then
    GCJFLAGS="-g -O2"
else
    GCJFLAGS="-O2"
fi # gcj.m4 327
])

# AC_LANG_FLAG(Java)(FLAG, [IF-WORKS], [IF-DOES-NOT-WORK])
# --------------------------------------------------------
# Test whether a flag works on its own.  Consider the flag to not work
# if it generates warnings when plain compiles do not.
m4_define([AC_LANG_FLAG(Java)],
[ac_test_GCJFLAGS=${GCJFLAGS+set}
ac_save_GCJFLAGS=$GCJFLAGS
AS_VAR_PUSHDEF([CacheVar], [AS_TR_SH(ac_cv_prog_gcj[]$1)])dnl
AC_CACHE_CHECK([whether $GCJ accepts $1], CacheVar,
    [ac_save_gcj_werror_flag=$ac_gcj_werror_flag
     ac_gcj_werror_flag=yes
     AS_VAR_SET([CacheVar], [no])
     GCJFLAGS="$1"
     _AC_COMPILE_IFELSE([AC_LANG_PROGRAM()],
	[AS_VAR_SET([CacheVar], [yes])],
	[GCJFLAGS=""
	_AC_COMPILE_IFELSE([AC_LANG_PROGRAM()],
	    [],
	    [ac_gcj_werror_flag=$ac_save_gcj_werror_flag
	     GCJFLAGS="$1"
	     _AC_COMPILE_IFELSE([AC_LANG_PROGRAM()],
		[AS_VAR_SET([CacheVar], [yes])])])])
     ac_gcj_werror_flag=$ac_save_gcj_werror_flag])
if test "$ac_test_GCJFLAGS" = set; then
    GCJFLAGS=$ac_save_GCJFLAGS
fi # gcj.m4 355
if test :AS_VAR_GET([CacheVar]) = :yes
then :;
$2
else :;
$3
fi # gcj.m4 361
AS_VAR_POPDEF([CacheVar])dnl
])

# AC_LANG_FLAG(Javah)(FLAG, [IF-WORKS], [IF-NOT-WORKS]
# ----------------------------------------------------
# Test whether a flag works on its own.  Consider the flag to not work
# if it generates warnings when plain compiles do not.
m4_define([AC_LANG_FLAG(Javah)],
[ac_test_JAVAHFLAGS=${JAVAHFLAGS+set}
ac_save_JAVAHFLAGS=$JAVAHFLAGS
AS_VAR_PUSHDEF([CacheVar], [AS_TR_SH(ac_cv_prog_javah[]$1)])dnl
AC_CACHE_CHECK([whether $JAVAH accepts $1], CacheVar,
    [ac_save_javah_werror_flag=$ac_javah_werror_flag
     ac_javah_werror_flag=yes
     AS_VAR_SET([CacheVar], [no])
     JAVAHFLAGS="$1"
     _AC_COMPILE_IFELSE([AC_LANG_PROGRAM()],
	[AS_VAR_SET([CacheVar], [yes])],
	[JAVAHFLAGS=""
	_AC_COMPILE_IFELSE([AC_LANG_PROGRAM()],
	    [],
	    [ac_javah_werror_flag=$ac_save_javah_werror_flag
	     JAVAHFLAGS="$1"
	     _AC_COMPILE_IFELSE([AC_LANG_PROGRAM()],
		[AS_VAR_SET([CacheVar], [yes])])])])
     ac_javah_werror_flag=$ac_save_javah_werror_flag])
if test "$ac_test_JAVAHFLAGS" = set; then
    JAVAHFLAGS=$ac_save_JAVAHFLAGS
fi
if test :AS_VAR_GET([CacheVar]) = :yes
then :;
$2
else :;
$3
fi # gcj.m4 396
AS_VAR_POPDEF([CacheVar])dnl
])

## ---------------------- ##
## Dependencies for JAVAC ##
## ---------------------- ##

# _AM_DEPDENDENCIES(JAVAC)
# ------------------------
m4_define([_AM_DEPENDENCIES(JAVAC)],
[AC_REQUIRE([AM_SET_DEPDIR])dnl
AC_REQUIRE([AM_OUTPUT_DEPENDENCY_COMMANDS])dnl
AC_REQUIRE([AM_MAKE_INCLUDE])dnl
AC_REQUIRE([AM_DEP_TRACK])dnl
AC_CACHE_CHECK([dependency style of $GCJ],
	       [am_cv_javac_dependencies_compiler_type],
[if test -z "$AMDEP_TRUE" && test -f "$am_depcomp"; then
    mkdir conftest.dir
    cp "$am_depcomp" conftest.dir
    cd conftest.dir
    mkdir sub
    am_cv_javac_dependencies_compiler_type=none
    # FIXME put tests here
    cd ..
    rm -rf conftest.dir
else
    am_cv_javac_dependencies_compiler_type=none
fi
])
AC_SUBST([JAVACDEPMODE], [depmode=$am_cv_javac_dependencies_compiler_type])
AM_CONDITIONAL([am__fastdepJAVAC], [
    test "x$enable_dependency_trackint" != xno \
    && test "$am_cv_javac_dependencies_compiler_type" = gcc3])
])


# =============================================================================
# _GCJ_CONFIG
# -----------------------------------------------------------------------------
AC_DEFUN([_GCJ_CONFIG], [dnl
    _GCJ_TOOLS
    _GCJ_OPTIONS
    _GCJ_SETUP
    _GCJ_OUTPUT
])# _GCJ_CONFIG
# =============================================================================

# =============================================================================
# _GCJ_TOOLS
# -----------------------------------------------------------------------------
AC_DEFUN([_GCJ_TOOLS], [dnl
    AC_REQUIRE([_OPENSS7_MISSING4])
    gcj_tmp="${PATH:+$PATH:}/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/X11R6/bin";
dnl
dnl We need a javac compiler or we cannot compile any classes.
dnl
    AC_REQUIRE([AC_PROG_JAVAC])
    AC_ARG_VAR([JAVAC],
	[Java class compiler. @<:@default=[$]GCJ@:>@])
    AC_ARG_VAR([JAVACFLAGS],
	[Java class compiler flags. @<:@default=auto@:>@])
    AC_PATH_PROGS([JAVAC], [$GCJ javac], [],
	[$gcj_tmp])
    if test -z "$JAVAC"; then
	AC_MSG_ERROR([
***
*** Configure could not find the Java class compiler program 'javac'
*** (nor 'gcj').  This program is part of the GNU Compiler Collection,
*** but is not always loaded on recent distributions.  It is also part
*** of most Java SDKs.
***
*** On RPM based distributions, try 'yum install gcc-java'.
*** On DEB based distributions, try 'apt-get install gcj'.
***
*** Alternatively, you can specify an equivalent command with the JAVAC
*** environment variable when rerunning configure.
***])
    fi

dnl
dnl We need the gcj-dbtool so that we can create classmap databses and add
dnl native compile java modules to the global classmap database when
dnl installing.
dnl
    AC_ARG_VAR([GCJDBTOOL],
	[GCJ database tool. @<:@default=gcj-dbtool@:>@])
    AC_PATH_PROGS([GCJDBTOOL], [gcj-dbtool], [],
	[$gcj_tmp])
    if test -z "$GCJDBTOOL"; then
	GCJDBTOOL="${am_missing4_run}gcj-dbtool"
	if test :"${USE_MAINTAINER_MODE:-no}" != :no
	then
	    AC_MSG_WARN([
***
*** Configure could not find a suitable GCJ database tool program, 'gcj-dbtool'.
*** This program is part of the GNU Compiler Collection, but is not always
*** loaded on recent distributions.  Also, older versions of libgcj, such as
*** that distributed with GCC version 2 and early version 3, do not have this
*** tool.  The first clue to this is that GCJ does not support the
*** -indirect-dispatch flag.
***
*** On RPM based distributions, try 'yum install libgcj'.
*** On DEB based distributions, try 'apt-get install gij'.
*** On SuSE distributions, try 'zypper install libgcj'.
***
*** Alternatively, you can specify an equivalent command with the GCJDBTOOL
*** environment variable when running configure.
***
*** Debian 4.0:  'apt-get install gij'
*** Ubuntu 8.04: 'apt-get install gij'
*** CentOS 5.x:  'yum install libgcj'
*** SLES 10:     'zypper install libgcj'
***])
	fi
    fi

dnl
dnl Note that we distribute the CNI header files.  Therefore, this tool should
dnl only get invoked should the CNI header files need to be updated.  A
dnl workaround for a missing tool is to simply touch the header files that
dnl were distributed if they exist.  If the header files do not exist, then an
dnl error will result.  See the missing4 script.
dnl
    AC_REQUIRE([AC_PROG_GCJH])
    AC_ARG_VAR([GCJH],
	[Java CNI header command. @<:@default=gcjh@:>@])
    AC_ARG_VAR([GCJHFLAGS],
	[Java CNI header command flags. @<:@default=auto@:>@])
    AC_PATH_PROGS([GCJH], [gcjh3 gcjh], [],
	[$gcj_tmp])
    if test :"${GCJH:-no}" = :no ; then
	GCJH="${am_missing4_run}gcjh"
	if test :"${USE_MAINTAINER_MODE:-no}" != :no
	then
	    AC_MSG_WARN([
***
*** Configure could not find the GNU Java Compiler CNI header generation
*** command, 'gcjh'.  This program is part of the GNU Compiler Collection, but
*** is not always loaded on recent distributions.
***
*** On RPM based distributions, try 'yum install gcc-java'.
*** On DEB based distributions, try 'apt-get install gcj'.
*** On SUSE distributions, try 'zypper install gcc-java'.
***
*** Alternatively, you can specify an equivalent command with the GCJH
*** environment variable when running configure.
***
*** Debian 4.0:	 'apt-get install gcj'
*** Ubuntu 8.04: 'apt-get install gcj'
*** CentOS 5.x:	 'yum install gcc-java'
*** SLES 10:	 'zypper install gcc-java'
*** RH 7.3:	 'rpm -i gcc-java'
***])
	fi
    fi

dnl
dnl Note that we distribute the JNI header files.  Therefore, this tool should
dnl only get invoked should the JNI header files need to be updated.  A
dnl workaround for a missing tool is to simply touch the header files that
dnl were distributed if they exist.  If the header files do not exist, then an
dnl error will result.  See the missing4 script.
dnl
    AC_REQUIRE([AC_PROG_JAVAH])
    AC_ARG_VAR([JAVAH],
	[Java JNI header command. @<:@default=gcjh@:>@])
    AC_ARG_VAR([JAVAHFLAGS],
	[Java JNI header command flags. @<:@default=auto@:>@])
    AC_PATH_PROGS([JAVAH], [gcjh3 gcjh javah], [],
	[$gcj_tmp])
    if test :"${JAVAH:-no}" = :no ; then
	JAVAH="${am_missing4_run}gcjh"
	if test :"${USE_MAINTAINER_MODE:-no}" != :no
	then
	    AC_MSG_WARN([
***
*** Configure could not find the Java JNI header generation program 'javah' (nor
*** 'gcjh').  This program is part of the GNU Compiler Collection, but is not
*** always loaded on recent distributions.  It is also part of most Java SDKs.
***
*** On RPM based distributions, try 'yum install gcc-java'.
*** On DEB based distributions, try 'apt-get install gcj'.
*** On SUSE distributions, try 'zypper install gcc-java'.
***
*** Alternatively, you can specify an equivalent command with the JAVAH
*** environment variable when running configure.
***
*** Debian 4.0:	 'apt-get install gcj'
*** Ubuntu 8.04: 'apt-get install gcj'
*** CentOS 5.x:	 'yum install gcc-java'
*** SLES 10:	 'zypper install gcc-java'
*** RH 7.3:	 'rpm -i gcc-java'
***])
	fi
    fi

dnl
dnl Note that we now distribute the prebuilt javadoc documentation.  This is
dnl due to SLES 10 that is completely missing the gjdoc tool and has no
dnl package for it.  A workaround for a missing tool is to simply touch the
dnl index file if it exists.  If the index file does not exist (e.g., it was
dnl removed with a distclean), an error will result.  See the missing4 script.
dnl
    AC_ARG_VAR([JAVADOC],
	[Java documentation doclet. @<:@default=gjdoc@:>@])
    AC_ARG_VAR([JAVADOCFLAGS],
	[Java documentation flags. @<:@default=auto@:>@])
    AC_PATH_PROGS([JAVADOC], [gjdoc javadoc], [],
	[$gcj_tmp])
    if test :"${JAVADOC:-no}" = :no ; then
	JAVADOC="${am_missing4_run}gjdoc"
	if test :"${USE_MAINTAINER_MODE:-no}" != :no
	then
	    AC_MSG_WARN([
***
*** Configure could not find the Java documentation program 'javadoc' (nor
*** 'gjdoc').  This program is part of the GNU Compiler Colleciton, but is not
*** always loaded on recent distributions.  It is also part of most Java SDKs.
***
*** On RPM based distributions, try 'yum install gcc-java'.
*** On DEB based distributions, try 'apt-get install gcj'.
*** On SUSE, try 'zypper install jdk-1_5_0-ibm-devel'.
***
*** Alternatively, you can specify an equivalent command with the JAVADOC
*** environment variable when running configure.
***
*** Debian 4.0:	 'apt-get install gjdoc'
*** Ubuntu 8.04: 'apt-get install gjdoc'
*** CentOS 5.x:	 'yum install gjdoc'
*** SLES 10:	 'zypper install java-1_5_0-ibm-devel'
*** RH 7.3:	 'rpm -i kaffe'
***])
	fi
    fi
    JAVADOCFLAGS=
    if test $(basename "$JAVADOC") = 'gjdoc'; then
	JAVADOCFLAGS="${JAVADOCFLAGS:+$JAVADOCFLAGS }-validhtml"
	JAVADOCFLAGS="${JAVADOCFLAGS:+$JAVADOCFLAGS }-licensetext"
    fi
])# _GCJ_TOOLS
# =============================================================================

# =============================================================================
# _GCJ_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_GCJ_OPTIONS], [dnl
    case "${with_optimize:-auto}" in
	(size)
	    ;;
	(speed)
	    ;;
	(normal)
	    ;;
	(quick)
	    ;;
    esac
    if test :"${USE_MAINTAINER_MODE:-no}" != :no
    then
	GCJFLAGS=`echo " $GCJFLAGS" | sed -e 's, -Wall,,g'`
	GCJFLAGS="${GCJFLAGS:+$GCJFLAGS }-Wall"
	GCJFLAGS=`echo " $GCJFLAGS" | sed -r -e 's, -W(no-)?error,,g'`
	GCJFLAGS="${GCJFLAGS:+$GCJFLAGS }-Werror"
    fi # gcj.m4 630
    AC_LANG_PUSH([Java])
    JNIFLAGS=`echo " $JNIFLAGS" | sed -r -e 's, -f(no-)?jni,,g'`
    AC_LANG_FLAG([-fjni],
		 [JNIFLAGS="-fjni"],
		 [JNIFLAGS=])
    CNIFLAGS=`echo " $CNIFLAGS" | sed -r -e 's, -f(no-)?indirect-dispatch,,g'`
    AC_LANG_FLAG([-findirect-dispatch],
		 [CNIFLAGS="-findirect-dispatch"],
		 [CNIFLAGS=])
    CGJFLAGS=`echo " $GCJFLAGS" | sed -r -e 's, -W(no-)?out-of-date,,g'`
    AC_LANG_FLAG([-Wno-out-of-date],
		 [GCJFLAGS="${GCJFLAGS:+$GCJFLAGS }-Wno-out-of-date"],
		 [GCJFLAGS=`echo " $GCJFLAGS" | sed -r -e 's, -Werror,,g'`])
    CGJFLAGS=`echo " $GCJFLAGS" | sed -r -e 's, -W(no-)?deprecated,,g'`
    AC_LANG_FLAG([-Wno-deprecated],
		 [GCJFLAGS="${GCJFLAGS:+$GCJFLAGS }-Wno-deprecated"],
		 [])
    # It seems that older GCJ's that cannot recogize this flag have
    # no way of suppressing these warnings and cannot have -Werror set.
    CGJFLAGS=`echo " $GCJFLAGS" | sed -r -e 's, -W(no-)?unused,,g'`
    AC_LANG_FLAG([-Wno-unused],
		 [GCJFLAGS="${GCJFLAGS:+$GCJFLAGS }-Wno-unused"],
		 [])
    CGJFLAGS=`echo " $GCJFLAGS" | sed -r -e 's, -W(no-)?redundant-modifiers,,g'`
    AC_LANG_FLAG([-Wno-redundant-modifiers],
		 [GCJFLAGS="${GCJFLAGS:+$GCJFLAGS }-Wno-redundant-modifiers"],
		 [])
    AC_LANG_POP([Java])
    AC_SUBST([JNIFLAGS])dnl
    AC_SUBST([CNIFLAGS])dnl
])# _GCJ_OPTIONS
# =============================================================================

# =============================================================================
# _GCJ_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_GCJ_SETUP], [dnl
])# _GCJ_SETUP
# =============================================================================

# =============================================================================
# _GCJ_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_GCJ_OUTPUT], [dnl
])# _GCJ_OUTPUT
# =============================================================================

# =============================================================================
# _GCJ_XXX
# -----------------------------------------------------------------------------
AC_DEFUN([_GCJ_XXX], [dnl
])# _GCJ_XXX
# =============================================================================

# =============================================================================
#
# $Log: gcj.m4,v $
# Revision 1.1.2.4  2009-07-05 12:04:27  brian
# - updates for release builds
#
# Revision 1.1.2.3  2009-07-04 03:51:33  brian
# - updates for release
#
# Revision 1.1.2.2  2009-06-29 07:35:38  brian
# - improvements to build process
#
# Revision 1.1.2.1  2009-06-21 11:06:04  brian
# - added files to new distro
#
# =============================================================================
# 
# Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
# Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn

