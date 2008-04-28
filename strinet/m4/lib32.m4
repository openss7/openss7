# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: lib32.m4,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.3 $) $Date: 2008-04-28 09:41:03 $
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
# Last Modified $Date: 2008-04-28 09:41:03 $ by $Author: brian $
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
# AH_CHECK_LIB32(LIBNAME)
# -----------------------------------------------------------------------------
m4_define([AH_CHECK_LIB32],
[AH_TEMPLATE(AS_TR_CPP(HAVE_LIB32$1),
	     [Define to 1 if you have the 32-bit `]$1[' library (-l]$1[).])])
# =============================================================================

# =============================================================================
#
# $Log: lib32.m4,v $
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
