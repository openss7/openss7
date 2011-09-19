# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: gcj.m4,v $ $Name:  $($Revision: 1.1.2.10 $) $Date: 2011-05-31 09:46:01 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2008-2011  Monavacon Limited <http://www.monavacon.com/>
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
# Last Modified $Date: 2011-05-31 09:46:01 $ by $Author: brian $
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
AC_LANG_DEFINE([Java], [gcj], [GCJ], [GCJ], [],
[ac_ext=java
ac_cpp=
ac_compile='$GCJ -c $GCJFLAGS conftest.$ac_ext >&AS_MESSAGE_LOG_FD'
ac_link='$GCJ -o conftest$ac_exeext $GCJFLAGS $LDFLAGS conftest.$ac_ext $LIBS >&AS_MESSAGE_LOG_FD'
ac_compiler_gnu=yes
])

# AC_LANG_JAVA
# ------------
AU_DEFUN([AC_LANG_JAVA], [AC_LANG(Java)])

## ------------------------ ##
## 2. Producting programs.  ##
## ------------------------ ##

# ------------------ #
# 2a. Java sources.  #
# ------------------ #

# AC_LANG_CONFTEST(Java)(BODY)
# ----------------------------
m4_define([AC_LANG_CONFTEST(Java)],
[cat >conftest.$ac_ext << _ACEOF
// Java language source.
package conftest;
$1
_ACEOF])

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
    fi
fi
if test -z "$GCJ" ; then
    AC_CHECK_PROG(GCJ, gcj, gcj, , , /usr/ucb/gcj)
fi
if test -z "$GCJ"; then
    AC_CHECK_TOOLS(GCJ, cl.exe)
fi
])

test -z "$GCJ" && AC_MSG_FAILURE([no acceptable GCJ compiler found in \$PATH])

if test :${GCJ:-gcj} != :gcj; then
    PACKAGE_RPMOPTIONS="GCJ=\"$GCJ\"${PACKAGE_RPMOPTIONS:+ $PACKAGE_RPMOPTIONS}"
    PACKAGE_DEBOPTIONS="GCJ=\"$GCJ\"${PACKAGE_DEBOPTIONS:+ $PACKAGE_DEBOPTIONS}"
    ac_configure_args="GCJ=\"$GCJ\"${ac_configure_args:+ $ac_configure_args}"
fi

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
fi
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
fi
if test :${GCJH:-gcjh} != :gcjh; then
    PACKAGE_RPMOPTIONS="GCJH=\"$GCJH\"${PACKAGE_RPMOPTIONS:+ $PACKAGE_RPMOPTIONS}"
    PACKAGE_DEBOPTIONS="GCJH=\"$GCJH\"${PACKAGE_DEBOPTIONS:+ $PACKAGE_DEBOPTIONS}"
    ac_configure_args="GCJH=\"$GCJH\"${ac_configure_args:+ $ac_configure_args}"
fi
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
AN_PROGRAM([gjavah], [AC_PROG_JAVAH])
AN_PROGRAM([javah], [AC_PROG_JAVAH])
AC_DEFUN([AC_PROG_JAVAH],
[AC_REQUIRE([AC_PROG_GCJH])
AC_LANG_PUSH(Java)dnl
AC_ARG_VAR([JAVAH], [Java JNI header command])dnl
AC_ARG_VAR([JAVAHFLAGS], [Java JNI header flags])dnl
if test -z "$JAVAH"; then
    AC_CHECK_TOOLS(JAVAH,
	[m4_default([$1], [$GCJH gcjh3 gcjh gcjnih gjavah javah])],
	gcjh)
fi
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
fi
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
fi
if test :AS_VAR_GET(CacheVar) = :yes
then :;
$2
else :;
$3
fi
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
if test :AS_VAR_GET(CacheVar) = :yes
then :;
$2
else :;
$3
fi
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
    AC_REQUIRE([_OPENSS7_MISSING4])dnl
    tmp_path="${PATH:+$PATH:}/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/X11R6/bin";
dnl
dnl We need a javac compiler or we cannot compile any classes.
dnl
    AC_REQUIRE([AC_PROG_JAVAC])
    AC_ARG_VAR([JAVAC],
	[Java class compiler. @<:@default=[$]GCJ@:>@])
    AC_ARG_VAR([JAVACFLAGS],
	[Java class compiler flags. @<:@default=auto@:>@])
    _BLD_PATH_PROGS([JAVAC], [$GCJ javac], [], [$tmp_path], [dnl
	_BLD_INSTALL_ERROR([JAVAC], [
***
*** Configure could not find the Java class compiler program 'javac'
*** (nor 'gcj').  This program is part of the GNU Compiler Collection,
*** but is not always loaded on recent distributions.  It is also part
*** of most Java SDKs.
*** ], [
*** On RPM based distributions, try 'yum install gcc-java'.
*** On DEB based distributions, try 'aptitude install gcj'.
***
*** Debian 5.0:  'aptitude install gcj'
*** Ubuntu 8.04: 'aptitude install gcj'
*** CentOS 5.x:  'yum install gcc-java'
*** SLES 11:     'zypper install gcc-java'
*** Mandriva 10: 'urpmi gcc-java'], [
***
*** Alternatively, you can specify an equivalent command with the JAVAC
*** environment variable when rerunning configure.
***])])

dnl
dnl We need the gcj-dbtool so that we can create classmap databses and add
dnl native compile java modules to the global classmap database when
dnl installing.  Note that older versions of gcj did not include classmap
dnl database and used the name of the library to determine which library to
dnl laod.  We have appropriate names for this approach on older systems.  If the
dnl tool is not present, we simply indicate to the makefiles not to install
dnl classmap databases.
dnl
    AC_ARG_VAR([GCJDBTOOL],
	[GCJ database tool. @<:@default=gcj-dbtool@:>@])
    _BLD_PATH_PROGS([GCJDBTOOL], [gcj-dbtool], [${am_missing4_run}gcj-dbtool], [$tmp_path], [dnl
	if test :"${USE_MAINTAINER_MODE:-no}" != :no ; then
	    _BLD_INSTALL_WARN([GCJDBTOOL], [
***
*** Configure could not find a suitable GCJ database tool program,
*** 'gcj-dbtool'.  This program is part of the GNU Compiler Collection,
*** but is not always loaded on recent distributions.  Also, older
*** versions of libgcj, such as that distributed with GCC version 2 and
*** early version 3, do not have this tool.  The first clue to this is
*** that GCJ does not support the -indirect-dispatch flag.
*** ], [
*** On RPM based distributions, try 'yum install libgcj'.
*** On DEB based distributions, try 'aptitude install gij'.
*** On SuSE distributions, try 'zypper install libgcj'.
***
*** Debian 5.0:  'aptitude install gij'
*** Ubuntu 8.04: 'aptitude install gij'
*** CentOS 5.x:  'yum install libgcj'
*** SLES 11:     'zypper install gcc-java'
*** Mandriva 10: 'urpmi gcj-tools'], [
***
*** Alternatively, you can specify an equivalent command with the
*** GCJDBTOOL environment variable when running configure.
***])
	fi])
    AM_CONDITIONAL([WITH_GCJDBTOOL], [test :"${ac_cv_path_GCJDBTOOL:-no}" != :no])dnl
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
    _BLD_PATH_PROGS([GCJH], [$GCJH gcjh3 gcjh], [${am_missing4_run}gcjh], [$tmp_path], [dnl
	if test :"${USE_MAINTAINER_MODE:-no}" != :no ; then
	    _BLD_INSTALL_WARN([GCJH], [
***
*** Configure could not find the GNU Java Compiler CNI header generation
*** command, 'gcjh'.  This program is part of the GNU Compiler
*** Collection, but is not always loaded on recent distributions.
*** ], [
*** On RPM based distributions, try 'yum install gcc-java'.
*** On DEB based distributions, try 'aptitude install gcj'.
*** On SUSE distributions, try 'zypper install gcc-java'.
***
*** Debian 5.0:	 'aptitude install gcj'
*** Ubuntu 8.04: 'aptitude install gcj'
*** CentOS 5.x:	 'yum install gcc-java'
*** SLES 11:	 'zypper install gcc-java'
*** Mandriva 10: 'urpmi gcj-tools'
*** RH 7.3:	 'rpm -i gcc-java'], [
***
*** Alternatively, you can specify an equivalent command with the GCJH
*** environment variable when running configure.
***])
	fi])

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
    _BLD_PATH_PROGS([JAVAH], [$GCJH gcjh3 gcjh gcjnih gjavah javah], [${am_missing4_run}gcjh], [$tmp_path], [dnl
	if test :"${USE_MAINTAINER_MODE:-no}" != :no ; then
	    _BLD_INSTALL_WARN([JAVAH], [
***
*** Configure could not find the Java JNI header generation program
*** 'javah' (nor 'gcjh').  This program is part of the GNU Compiler
*** Collection, but is not always loaded on recent distributions.  It is
*** also part of most Java SDKs.
*** ], [
*** On RPM based distributions, try 'yum install gcc-java'.
*** On DEB based distributions, try 'aptitude install gcj'.
*** On SUSE distributions, try 'zypper install gcc-java'.
***
*** Debian 5.0:	 'aptitude install gcj'
*** Ubuntu 8.04: 'aptitude install gcj'
*** CentOS 5.x:	 'yum install gcc-java'
*** SLES 11:	 'zypper install gcc-java'
*** Mandriva 10: 'urpmi gcj-tools'
*** RH 7.3:	 'rpm -i gcc-java'], [
***
*** Alternatively, you can specify an equivalent command with the JAVAH
*** environment variable when running configure.
***])
	fi])

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
    _BLD_PATH_PROGS([JAVADOC], [javadoc gjdoc], [${am_missing4_run}gjdoc], [$tmp_path], [dnl
	if test :"${USE_MAINTAINER_MODE:-no}" != :no ; then
	    _BLD_INSTALL_WARN([JAVADOC], [
***
*** Configure could not find the Java documentation program 'javadoc'
*** (nor 'gjdoc').  This program is part of the GNU Compiler Colleciton,
*** but is not always loaded on recent distributions.  It is also part
*** of most Java SDKs.
*** ], [
*** On RPM based distributions, try 'yum install gcc-java'.
*** On DEB based distributions, try 'aptitude install gcj'.
*** On SUSE, try 'zypper install jdk-1_5_0-ibm-devel'.
***
*** Debian 5.0:	 'aptitude install gjdoc'
*** Ubuntu 8.04: 'aptitude install gjdoc'
*** CentOS 5.x:	 'yum install gjdoc'
*** SLES 10:	 'zypper install java-1_5_0-ibm-devel'
*** SLES 11:     'zypper install gjdoc'
*** Mandriva 10: 'urpmi javadoc'
*** RH 7.3:	 'rpm -i kaffe'], [
***
*** Alternatively, you can specify an equivalent command with the
*** JAVADOC environment variable when running configure.
***])
	fi])
    AC_REQUIRE_SHELL_FN([bld_path_check])dnl
    AC_CACHE_CHECK([for libgcj javadoc directory], [ac_cv_libgcj_doc], [dnl
	eval "gcj_search_path=\"
	    ${DESTDIR}${rootdir}${javadocdir}
	    ${DESTDIR}${rootdir}/usr/share/javadoc
	    ${DESTDIR}${rootdir}${docdir}
	    ${DESTDIR}${rootdir}/usr/share/doc
	    ${DESTDIR}${javadocdir}
	    ${DESTDIR}/usr/share/javadoc
	    ${DESTDIR}${docdir}
	    ${DESTDIR}/usr/share/doc\""
	gcj_search_path=`echo "$gcj_search_path" | sed -e 's,\<NONE\>,'$ac_default_prefix',g;s,//,/,g'`
	AC_MSG_RESULT([searching])
	for gcj_dir in $gcj_search_path ; do
	    if test -d "$gcj_dir" ; then
		gcj_dirs=`find "$gcj_dir" -name 'Class.html' 2>/dev/null | egrep '(gcj|openjdk)' | grep '/java/lang/Class.html' | sed -r -e 's,^(.*)/java/lang/Class.html,\1,' | sort -ru`
		for gcj_sub in $gcj_dirs ; do
		    for gcj_sdir in $gcj_sub $gcj_sub/html ; do
			AC_MSG_CHECKING([for libgcj javadoc directory... $gcj_sdir])
			if test -d "$gcj_sdir" -a \( -r $gcj_sdir/package-list -o -r $gcj_sdir/package-list.gz \) ; then
			    ac_cv_libgcj_doc="$gcj_sdir"
			    AC_MSG_RESULT([yes])
			    break 3
			else
			    AC_MSG_RESULT([no])
			fi
		    done
		done
	    fi
	done
	test -n "$ac_cv_libgcj_doc" || ac_cv_libgcj_doc=no
	AC_MSG_CHECKING([for libgcj javadoc directory])
    ])
    if test :"${ac_cv_libgcj_doc:-no}" = :no ; then
	_BLD_INSTALL_WARN([PACKAGE_LIST], [
***
*** Configure could not find a suitable libgcj javadoc documentation
*** directory.  The libgcj javadoc documentation is part of the 'libgcj'
*** documentation package which is not always loaded on all
*** distributions.  Try:
*** ], [
*** On RPM based distributions, try 'yum install libgcj-doc'.
*** On DEB based distributions, try 'aptitude install libgcj-doc'.
***
*** Debian 5.0:   'aptitude install libgcj-doc'
*** Ubuntu 8.04:  'aptitude install libgcj-doc'
*** CentOS 5.x:   'yum install libgcj-doc'], [
***
*** Repeat after loading the correct package.  Otherwise, java
*** documentation will be linked with the online versions of the
*** classpath documentation instead.  Proceeding with linking javadoc
*** online instead of offline.
*** ])
	gcjdocdir=
    else
	bld_path_check "ac_cv_libgcj_doc" "package-list"
	gcjdocdir="$ac_cv_libgcj_doc"
    fi
    AC_SUBST([gcjdocdir])dnl
    JAVADOCFLAGS=
    if test $(basename "$JAVADOC") = 'gjdoc'; then
	JAVADOCFLAGS="${JAVADOCFLAGS:+$JAVADOCFLAGS }-validhtml"
	JAVADOCFLAGS="${JAVADOCFLAGS:+$JAVADOCFLAGS }-licensetext"
    fi
    if test -n "$gcjdocdir" ; then
	JAVADOCFLAGS="${JAVADOCFLAGS:+$JAVADOCFLAGS }"'-linkoffline http://developer.classpath.org/doc/ $(gcjdocdir)'
    else
	JAVADOCFLAGS="${JAVADOCFLAGS:+$JAVADOCFLAGS }"'-link http://developer.classpath.org/doc/'
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
    fi
    AC_LANG_PUSH([Java])
    JNIFLAGS=`echo " $JNIFLAGS" | sed -r -e 's, -f(no-)?jni,,g'`
    AC_LANG_FLAG([-fjni],
		 [JNIFLAGS="-fjni"],
		 [JNIFLAGS=])
    CNIFLAGS=`echo " $CNIFLAGS" | sed -r -e 's, -f(no-)?indirect-dispatch,,g'`
    AC_LANG_FLAG([-findirect-dispatch],
		 [CNIFLAGS="-findirect-dispatch"],
		 [CNIFLAGS=])
    GCJFLAGS=`echo " $GCJFLAGS" | sed -r -e 's, -fsource=([[^[:space:]]]*),,g'`
    AC_LANG_FLAG([-fsource=1.4],
		 [GCJFLAGS="${GCJFLAGS:+$GCJFLAGS }-fsource=1.4"],
		 [])
    GCJFLAGS=`echo " $GCJFLAGS" | sed -r -e 's, -W(no-)?out-of-date,,g'`
    AC_LANG_FLAG([-Wno-out-of-date],
		 [GCJFLAGS="${GCJFLAGS:+$GCJFLAGS }-Wno-out-of-date"],
		 [GCJFLAGS=`echo " $GCJFLAGS" | sed -r -e 's, -Werror,,g'`])
    GCJFLAGS=`echo " $GCJFLAGS" | sed -r -e 's, -W(no-)?deprecated,,g'`
    AC_LANG_FLAG([-Wno-deprecated],
		 [GCJFLAGS="${GCJFLAGS:+$GCJFLAGS }-Wno-deprecated"],
		 [])
    GCJFLAGS=`echo " $GCJFLAGS" | sed -r -e 's, -W(no-)?serial,,g'`
    AC_LANG_FLAG([-Wno-serial],
		 [GCJFLAGS="${GCJFLAGS:+$GCJFLAGS }-Wno-serial"],
		 [])
    GCJFLAGS=`echo " $GCJFLAGS" | sed -r -e 's, -W(no-)?param-assign,,g'`
    AC_LANG_FLAG([-Wno-param-assign],
		 [GCJFLAGS="${GCJFLAGS:+$GCJFLAGS }-Wno-param-assign"],
		 [])
    # It seems that older GCJ's that cannot recogize this flag have
    # no way of suppressing these warnings and cannot have -Werror set.
    GCJFLAGS=`echo " $GCJFLAGS " | sed -r -e 's, -W(no-)?unused , ,g'`
    AC_LANG_FLAG([-Wno-unused],
		 [GCJFLAGS="${GCJFLAGS:+$GCJFLAGS }-Wno-unused"],
		 [])
    GCJFLAGS=`echo " $GCJFLAGS " | sed -r -e 's, -W(no-)?unused-argument , ,g'`
    AC_LANG_FLAG([-Wno-unused-argument],
		 [GCJFLAGS="${GCJFLAGS:+$GCJFLAGS }-Wno-unused-argument"],
		 [])
    GCJFLAGS=`echo " $GCJFLAGS " | sed -r -e 's, -W(no-)?unused-import , ,g'`
    AC_LANG_FLAG([-Wno-unused-import],
		 [GCJFLAGS="${GCJFLAGS:+$GCJFLAGS }-Wno-unused-import"],
		 [])
    GCJFLAGS=`echo " $GCJFLAGS" | sed -r -e 's, -W(no-)?redundant-modifiers,,g'`
    AC_LANG_FLAG([-Wno-redundant-modifiers],
		 [GCJFLAGS="${GCJFLAGS:+$GCJFLAGS }-Wno-redundant-modifiers"],
		 [])
    GCJFLAGS=`echo " $GCJFLAGS" | sed -r -e 's, -W(no-)?raw,,g'`
    AC_LANG_FLAG([-Wno-raw],
		 [GCJFLAGS="${GCJFLAGS:+$GCJFLAGS }-Wno-raw"],
		 [])
    AC_LANG_POP([Java])
    JNIFLAGS=`echo "$JNIFLAGS" | sed -r -e 's,^[[[:space:]]]*,,;s,[[[:space:]]][$],,'`
    AC_SUBST([JNIFLAGS])dnl
    CNIFLAGS=`echo "$CNIFLAGS" | sed -r -e 's,^[[[:space:]]]*,,;s,[[[:space:]]][$],,'`
    AC_SUBST([CNIFLAGS])dnl
    GCJFLAGS=`echo "$GCJFLAGS" | sed -r -e 's,^[[[:space:]]]*,,;s,[[[:space:]]][$],,'`
])# _GCJ_OPTIONS
# =============================================================================

# =============================================================================
# _GCJ_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_GCJ_SETUP], [dnl
    AC_ARG_WITH([libgcj],
	[AS_HELP_STRING([--with-libgcj=HEADERS],
	    [GCJ library header directory @<:@default=search@:>@])],
	[], [with_libgcj=search])
    _BLD_FIND_DIR([libgcj include directory], [gcj_cv_includedir], [
	    ${DESTDIR}${rootdir}${includedir}
	    ${DESTDIR}${rootdir}${oldincludedir}
	    ${DESTDIR}${rootdir}/usr/include
	    ${DESTDIR}${rootdir}/usr/local/include
	    ${DESTDIR}${includedir}
	    ${DESTDIR}${oldincludedir}
	    ${DESTDIR}/usr/include
	    ${DESTDIR}/usr/local/include], [gcj/cni.h], [no], [dnl
	if test ${with_libgcj:-search} != no ; then
	    _BLD_INSTALL_WARN([GCJ_CNI_H], [
***
*** Configure cannot find the libgcj development header gcj/cni.h.  This
*** header is required to compile CNI components for Java.  The
*** gcj/cni.h header is part of the libgcj-devel package on most Linux
*** distributions.  Try:
*** ], [
*** Debian/Ubuntu: 'aptitude install libgcj-devel'
*** Mandriva 2010: 'urpmi libgcj-devel'
*** SuSE 11:       'zypper install libgcj-devel'
*** CentOS/RHEL:   'yum install libgcj10-devel'], [
***
*** Repeat after loading the correct package or by specifying the
*** location of the headers with the --with-libgcj=DIRECTORY option to
*** configure, or by specifying --without-java: continuing under the
*** assumption that the option --without-java was intended.
*** ])
	    PACKAGE_RPMOPTIONS="${PACKAGE_RPMOPTIONS:+$PACKAGE_RPMOPTIONS }--define \"_without_libgcj --without-libgcj\""
	    PACKAGE_DEBOPTIONS="${PACKAGE_DEBOPTIONS:+$PACKAGE_DEBOPTIONS }'--without-libgcj'"
	    ac_configure_args="${ac_configure_args:+$ac_configure_args }'--without-libgcj'"
	    with_libgcj=no
	fi], [], [with_libgcj])
    if test ${gcj_cv_includedir:-no} = no ; then
	gcjincludedir=
    else
	gcjincludedir="$gcj_cv_includedir"
    fi
    AM_CONDITIONAL([WITH_LIBGCJ], [test :"${with_libgcj:-search}" != :no])
    AC_SUBST([gcjincludedir])dnl
    AC_CACHE_CHECK([for libgcj cxxflags], [gcj_cv_cxxflags], [dnl
	if test -n "$gcjincludedir" ; then
	    gcj_cv_cxxflags="-I$gcjincludedir"
	else
	    gcj_cv_cxxflags=
	fi
    ])
    LIBGCJ_CXXFLAGS="$gcj_cv_cxxflags"
    AC_SUBST([LIBGCJ_CXXFLAGS])dnl
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
# Revision 1.1.2.10  2011-05-31 09:46:01  brian
# - new distros
#
# Revision 1.1.2.9  2011-02-07 04:48:32  brian
# - updated configure and build scripts
#
# Revision 1.1.2.8  2011-01-12 03:49:24  brian
# - support for RHEL 6 kernel
#
# Revision 1.1.2.7  2010-11-28 13:55:51  brian
# - update build requirements, proper autoconf functions, build updates
#
# Revision 1.1.2.6  2009-07-21 11:06:13  brian
# - changes from release build
#
# Revision 1.1.2.5  2009-07-13 07:13:27  brian
# - changes for multiple distro build
#
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
# Copyright (c) 2008-2011  Monavacon Limited <http://www.monavacon.com/>
# Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn

