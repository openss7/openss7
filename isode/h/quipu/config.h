/*****************************************************************************

 @(#) $Id$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ifndef __ISODE_QUIPU_CONFIG_H__
#define __ISODE_QUIPU_CONFIG_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* config.h - compile time configuration parameters */

/* 
 * Header: /xtel/isode/isode/h/quipu/RCS/config.h,v 9.0 1992/06/16 12:23:11 isode Rel
 *
 *
 * Log: config.h,v
 * Revision 9.0  1992/06/16  12:23:11  isode
 * Release 8.0
 *
 */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */

/* 
 * OPTIONS:-
 *
 * #define PDU_DUMP
 * 		If this is defined, and "dish" is invoke with
 *		dish -pdu foobar
 *		Then a directory "foobar" will be created, and 
 *		it will contain logs of all the X500 PDUs sent
 *
 * #define NO_STATS
 * 		If defined, the QUIPU will NOT produce statistical
 *		logs of both the DSA and DUA.
 *
 * #define CHECK_FILE_ATTRIBUTES
 * 		If and EDB entry contains a FILE attribute, check that
 *		the corresponding file exists
 *
 * #define QUIPU_MALLOC	
 * 		Use a version of malloc optimised for the memory
 * 		resident Quipu DSA database.
 *
 * #define TURBO_DISK
 *		Store EDB files in gdbm files instead of plain text files.
 *		This makes modifies of entries in large EDB files much
 *		faster.  See the ../../quipu/turbo directory for tools to
 *		help in converting your EDB files.
 *
 * #define TURBO_INDEX
 *		Enable code to build and search database indexes for
 *		selected attributes (see tailor file options optimize_attr,
 *		index_subtree, and index_siblings).
 *		This can cut the search	time for very large databases.
 *
 * #define SOUNDEX_PREFIX
 *		Consider soundex prefixes as matches.  For example, make
 *		"fred" match "frederick".  #defining this option gives
 *		approximate matching behavior the same as in version 6.0.
 *
 * #define STRICT_X500
 *		Enforce X.500 more strictly than "normal".
 *		Useful for conformance testing, but not "real users".
 *
 * #define HAVE_PROTECTED
 *		If defined, enable use of protectedPassword attribute.
 *
 * #define QUIPU_CONSOLE
 *		Undocumented pre-alpha test feature.
 *
 */

# ifndef QUIPU_CONFIG
# define QUIPU_CONFIG

#ifndef	USE_BUILTIN_OIDS
#define	USE_BUILTIN_OIDS	1
#endif

# define PDU_DUMP
# define QUIPU_MALLOC
# define TURBO_INDEX
# define SOUNDEX_PREFIX
# define HAVE_PROTECTED

# endif

#endif				/* __ISODE_QUIPU_CONFIG_H__ */
