# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: lib32.m4,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-09-02 08:46:30 $
#
# -----------------------------------------------------------------------------
#
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
# Last Modified $Date: 2011-09-02 08:46:30 $ by $Author: brian $
#
# =============================================================================

# =============================================================================
# This file provides 32-bit library versions of the normal autoconf macros.
# They are used for checking 32-bit libraries independent of native (64-bit)
# libraries.
# =============================================================================

# =============================================================================
# AC_CHECK_LIB(LIBRARY, FUNCTION,
#		 [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND],
#		 [OTHER-LIBRARIES], [PROLOGUE])
# -----------------------------------------------------------------------------
# Use cache variable name containing both the library and function name, because
# the test really is for library $1 defining function $2, not just for library
# $1.  Separate tests with the same $1 and different $2s may have different
# results.
#
# Note that using directly AS_VAR_PUSHDEF([ac_Lib], [ac_cv_lib_$1_$2]) is asking
# for troubles, since AC_CHECK_LIB($lib, fun) would give ac_cv_lib_$lib_fun,
# which is definitely not what was meant.  Hence the AS_LITERAL_IF indirection.
#
# FIXME: This macro is extremely suspicious.  It DEFINEs unconditionally,
# whatever the FUNCTION, in addition to not being a *S macro.  Note that the
# cache does depend upon the function we are looking for.
#
# It is on purpose we used `ac_check_lib_save_LIBS' and not just
# `ac_save_LIBS': there are many macros which don't want to see `LIBS' changed
# but still want to use AC_CHECK_LIB, so they save `LIBS'.  And `ac_save_LIBS'
# is too tempting a name, so let's leave them some freedom.
# =============================================================================
AC_DEFUN([AC_CHECK_LIB],
[m4_ifval([$3], , [AH_CHECK_LIB([$1])])dnl
AS_LITERAL_IF([$1],
	      [AS_VAR_PUSHDEF([ac_Lib], [ac_cv_lib_$1_$2])],
	      [AS_VAR_PUSHDEF([ac_Lib], [ac_cv_lib_$1''_$2])])dnl
AC_CACHE_CHECK([for $2 in native -l$1], ac_Lib,
[ac_check_lib_save_LIBS=$LIBS
LIBS="-l$1 $5 $LIBS"
AC_LINK_IFELSE([AC_LANG_CALL([$6], [$2])],
	       [AS_VAR_SET(ac_Lib, yes)],
	       [AS_VAR_SET(ac_Lib, no)])
LIBS=$ac_check_lib_save_LIBS])
AS_IF([test AS_VAR_GET(ac_Lib) = yes],
      [m4_default([$3], [AC_DEFINE_UNQUOTED(AS_TR_CPP(HAVE_LIB$1))
  LIBS="-l$1 $LIBS"
])],
      [$4])dnl
AS_VAR_POPDEF([ac_Lib])dnl
])
# -----------------------------------------------------------------------------
# AC_CHECK_LIB
# =============================================================================

# =============================================================================
# AC_CHECK_LIB32(LIBRARY, FUNCTION,
#		 [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND],
#		 [OTHER-LIBRARIES], [PROLOGUE])
# -----------------------------------------------------------------------------
# Use cache variable name containing both the library and function name, because
# the test really is for library $1 defining function $2, not just for library
# $1.  Separate tests with the same $1 and different $2s may have different
# results.
#
# Note that using directly AS_VAR_PUSHDEF([ac_Lib], [ac_cv_lib32_$1_$2]) is asking
# for troubles, since AC_CHECK_LIB32($lib, fun) would give ac_cv_lib32_$lib_fun,
# which is definitely not what was meant.  Hence the AS_LITERAL_IF indirection.
#
# FIXME: This macro is extremely suspicious.  It DEFINEs unconditionally,
# whatever the FUNCTION, in addition to not being a *S macro.  Note that the
# cache does depend upon the function we are looking for.
#
# It is on purpose we used `ac_check_lib_save_LIBS' and not just
# `ac_save_LIBS': there are many macros which don't want to see `LIBS' changed
# but still want to use AC_CHECK_LIB32, so they save `LIBS'.  And `ac_save_LIBS'
# is too tempting a name, so let's leave them some freedom.
#
# Note that LDFLAGS must be set appropriately by the called (e.g. -m32).
# =============================================================================
AC_DEFUN([AC_CHECK_LIB32],
[m4_ifval([$3], , [AH_CHECK_LIB32([$1])])dnl
AS_LITERAL_IF([$1],
	      [AS_VAR_PUSHDEF([ac_Lib], [ac_cv_lib32_$1_$2])],
	      [AS_VAR_PUSHDEF([ac_Lib], [ac_cv_lib32_$1''_$2])])dnl
AC_CACHE_CHECK([for $2 in 32-bit -l$1], ac_Lib,
[ac_check_lib_save_LIBS=$LIBS
LIBS="-l$1 $5 $LIBS"
AC_LINK_IFELSE([AC_LANG_CALL([$6], [$2])],
	       [AS_VAR_SET(ac_Lib, yes)],
	       [AS_VAR_SET(ac_Lib, no)])
LIBS=$ac_check_lib_save_LIBS])
AS_IF([test AS_VAR_GET(ac_Lib) = yes],
      [m4_default([$3], [AC_DEFINE_UNQUOTED(AS_TR_CPP(HAVE_LIB32$1))
  LIBS="-l$1 $LIBS"
])],
      [$4])dnl
AS_VAR_POPDEF([ac_Lib])dnl
])
# -----------------------------------------------------------------------------
# AC_CHECK_LIB32
# =============================================================================

# =============================================================================
# AC_CHECK_LIB_BROKEN(LIBRARY, FUNCTION,
#                     [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND],
#                     [OTHER-LIBRARIES], [PROLOGUE])
# -----------------------------------------------------------------------------
# SLES 10 has some broken native libraries.  These libraries do not include the
# base SONAME in the library and do not install the proper symbolic links in
# /lib or /usr/lib, therefore, when the check fails, it is necessary to go
# looking for the library and include it directly.
# =============================================================================
AC_DEFUN([AC_CHECK_LIB_BROKEN],
[m4_ifval([$3], , [AH_CHECK_LIB([$1])])dnl
AS_LITERAL_IF([$1],
	      [AS_VAR_PUSHDEF([ac_Lib], [ac_cv_lib_$1_$2])
	       AS_VAR_PUSHDEF([ac_Flg], [ac_cv_flg_$1_$2])],
	      [AS_VAR_PUSHDEF([ac_Lib], [ac_cv_lib_$1''_$2]),
	       AS_VAR_PUSHDEF([ac_Flg], [ac_cv_flg_$1''_$2])])dnl
AC_CACHE_CHECK([for $2 in native -l$1], ac_Lib,
[ac_check_lib_save_LIBS=$LIBS
LIBS="-l$1 $5 $LIBS"
AC_LINK_IFELSE([AC_LANG_CALL([$6], [$2])],
	       [AS_VAR_SET(ac_Flg, "-l$1")
		AS_VAR_SET(ac_Lib, yes)],
[AS_VAR_SET(ac_Flg, no)
 AS_VAR_SET(ac_Lib, no)
 AC_MSG_RESULT([searching])
 eval "ac_search_path=\"
    ${syslibdir}
    ${libdir}\""
 ac_search_path=`echo "$ac_search_path" | sed -e "s,\<NONE\>,$ac_default_prefix,g;s,//,/,g" | awk '{if(!([$]0 in seen)){print[$]0;seen[[$ 0]]=1}}'`
 for ac_dir in $ac_search_path ; do
     for ac_lib in $ac_dir/lib$1.so.* ; do
	 if test -r $ac_lib ; then
	     AC_MSG_CHECKING([for broken native lib$1 $ac_lib])
	     LIBS=$ac_check_lib_save_LIBS
	     LIBS="$ac_lib $5 $LIBS"
	     AC_LINK_IFELSE([AC_LANG_CALL([$6], [$2])],
			    [AS_VAR_SET(ac_Flg, $ac_lib)
			     AS_VAR_SET(ac_Lib, yes)],
			    [AS_VAR_SET(ac_Flg, no)
			     AS_VAR_SET(ac_Lib, no)])
	     AC_MSG_RESULT([AS_VAR_GET(ac_Flg)])
	     AS_IF([test AS_VAR_GET(ac_Lib) = yes], [break])
	 fi
     done
 done
 AC_MSG_CHECKING([for $2 in native lib$1])
])
LIBS=$ac_check_lib_save_LIBS])
AS_IF([test AS_VAR_GET(ac_Lib) = yes],
      [m4_default([$3], [AC_DEFINE_UNQUOTED(AS_TR_CPP(HAVE_LIB$1))
    LIBS=AS_VAR_GET(ac_Flg)" $LIBS"
])],
      [$4])dnl
AS_VAR_POPDEF([ac_Lib])dnl
AS_VAR_POPDEF([ac_Flg])dnl
])
# -----------------------------------------------------------------------------
# AC_CHECK_LIB_BROKEN
# =============================================================================

# =============================================================================
# AC_CHECK_LIB32_BROKEN(LIBRARY, FUNCTION,
#                       [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND],
#                       [OTHER-LIBRARIES], [PROLOGUE])
# -----------------------------------------------------------------------------
# SLES 10 has broken 32-bit libraries on 64-bit architectures.  These libraries
# do not include the base SONAME in the library and do not install the proper
# symbolic links in /lib or /usr/lib, therefore, when the check fails, it is
# necessary to go looking for the library and include it directly.
# =============================================================================
AC_DEFUN([AC_CHECK_LIB32_BROKEN],
[m4_ifval([$3], , [AH_CHECK_LIB32([$1])])dnl
AS_LITERAL_IF([$1],
	      [AS_VAR_PUSHDEF([ac_Lib], [ac_cv_lib32_$1_$2])
	       AS_VAR_PUSHDEF([ac_Flg], [ac_cv_flg32_$1_$2])],
	      [AS_VAR_PUSHDEF([ac_Lib], [ac_cv_lib32_$1''_$2])
	       AS_VAR_PUSHDEF([ac_Flg], [ac_cv_flg32_$1''_$2])])dnl
AC_CACHE_CHECK([for $2 in 32-bit -l$1], ac_Lib,
[ac_check_lib_save_LIBS=$LIBS
LIBS="-l$1 $5 $LIBS"
AC_LINK_IFELSE([AC_LANG_CALL([$6], [$2])],
	       [AS_VAR_SET(ac_Flg, "-l$1")
	        AS_VAR_SET(ac_Lib, yes)],
[AS_VAR_SET(ac_Flg, no)
 AS_VAR_SET(ac_Lib, no)
 AC_MSG_RESULT([searching])
 eval "ac_search_path=\"
    ${syslib32dir}
    ${lib32dir}\""
 ac_search_path=`echo "$ac_search_path" | sed -e "s,\<NONE\>,$ac_default_prefix,g;s,//,/,g" | awk '{if(!([$]0 in seen)){print[$]0;seen[[$ 0]]=1}}'`
 for ac_dir in $ac_search_path ; do
     for ac_lib in $ac_dir/lib$1.so.* ; do
	 if test -r $ac_lib ; then
	     AC_MSG_CHECKING([for broken 32-bit lib$1 $ac_lib])
	     LIBS=$ac_check_lib_save_LIBS
	     LIBS="$ac_lib $5 $LIBS"
	     AC_LINK_IFELSE([AC_LANG_CALL([$6], [$2])],
			    [AS_VAR_SET(ac_Flg, $ac_lib)
			     AS_VAR_SET(ac_Lib, yes)],
			    [AS_VAR_SET(ac_Flg, no)
			     AS_VAR_SET(ac_Lib, no)])
	     AC_MSG_RESULT([AS_VAR_GET(ac_Flg)])
	     AS_IF([test AS_VAR_GET(ac_Lib) = yes], [break])
	 fi
     done
 done
 AC_MSG_CHECKING([for $2 in 32-bit lib$1])
])
LIBS=$ac_check_lib_save_LIBS])
AS_IF([test AS_VAR_GET(ac_Lib) = yes],
      [m4_default([$3], [AC_DEFINE_UNQUOTED(AS_TR_CPP(HAVE_LIB32$1))
    LIBS=AS_VAR_GET(ac_Flg)" $LIBS"
])],
      [$4])dnl
AS_VAR_POPDEF([ac_Lib])dnl
AS_VAR_POPDEF([ac_Flg])dnl
])
# -----------------------------------------------------------------------------
# AC_CHECK_LIB32_BROKEN
# =============================================================================

# =============================================================================
# AH_CHECK_LIB32(LIBNAME)
# -----------------------------------------------------------------------------
m4_define([AH_CHECK_LIB32],
[AH_TEMPLATE(AS_TR_CPP(HAVE_LIB32$1),
	     [Define to 1 if you have the 32-bit `]$1[' library (-l]$1[).])])
# =============================================================================

# =============================================================================
#
# $Log: lib32.m4,v $
# Revision 1.1.2.2  2011-09-02 08:46:30  brian
# - sync up lots of repo and build changes from git
#
# Revision 1.1.2.1  2009-06-21 11:06:04  brian
# - added files to new distro
#
# Revision 0.9.2.3  2008-04-28 09:41:03  brian
# - updated headers for release
#
# Revision 0.9.2.2  2007/10/15 06:47:49  brian
# - update to SNMP agent build
#
# Revision 0.9.2.1  2007/10/13 08:13:17  brian
# - added macros for SNMP agents
#
# =============================================================================
# 
# Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
