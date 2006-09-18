/*****************************************************************************

 @(#) $Id: sockpath.h,v 0.9.2.2 2006/09/18 13:52:42 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 2 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 Ave, Cambridge, MA 02139, USA.

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

 Last Modified $Date: 2006/09/18 13:52:42 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sockpath.h,v $
 Revision 0.9.2.2  2006/09/18 13:52:42  brian
 - added doxygen markers to sources

 Revision 0.9.2.1  2006/09/18 00:10:35  brian
 - added libsocket source files and manuals

 *****************************************************************************/

#ifndef __SYS_SOCKPATH_H__
#define __SYS_SOCKPATH_H__

#ident "@(#) $RCSfile: sockpath.h,v $ $Name:  $($Revision: 0.9.2.2 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

/* This file can be processed with doxygen(1). */

#define SOCK2PATH		"/etc/sock2path"
#define	SOCKPATH		"SOCKPATH"

/**
 * @file
 *
 * This file contains definitions for use with the
 * <a href="http://www.openss7.org/man2html?sockpath(5)">sockpath</a>(5)
 * commands.
 */

/**
 * @struct src/include/sys/sockpath.h <sockdb.h>
 * @brief Primary sockpath(5) database entry structure representation.
 */
struct sockpath {
	int sp_domain;	    /**< Socket domain as argument to socket(3) call. */
	int sp_type;	    /**< Socket type as argument to socket(3) call. */
	int sp_protocol;    /**< Socket protocol as argument to socket(3) call. */
	char *sp_path;	    /**< Path to STREAMS(9) device. */
};

#ifdef _REENTRANT
extern int *__sperror(void);
#define sp_error (*(__sperror()))
extern char *__sperrbuf(void);
#define sp_errbuf (*(__sperrbuf()))
#else
#error Compiled without _REENTRANT defined!
//extern int sp_error;
//extern char *sp_errbuf;
#endif

/** @brief Error codes returned by sockpath(5) functions. */
enum {
	SP_NOERROR,		/**< No error was returned. */
	SP_NOMEM,		/**< Memory allocation failed. */
	SP_NOSET,		/**< No setsockpath() before getsockpath(). */
	SP_OPENFAIL,		/**< Open of the /etc/sock2path file failed. */
	SP_BADLINE,		/**< Syntax error exists in /etc/sock2path file. */
	SP_NOTFOUND,		/**< Entry specified to getsockpathent() not found. */
	SP_NOMOREENTRIES,	/**< No more entries exist in /etc/sock2path. */
	SP_ERROR_MAX		/**< The current maximum error number value. */
};

#define SP_NOERROR		SP_NOERROR
#define SP_NOMEM		SP_NOMEM
#define SP_NOSET		SP_NOSET
#define SP_OPENFAIL		SP_OPENFAIL
#define SP_BADLINE		SP_BADLINE
#define SP_NOTFOUND		SP_NOTFOUND
#define SP_NOMOREENTRIES	SP_NOMOREENTRIES
#define SP_ERROR_MAX		SP_ERROR_MAX

#ifdef __BEGIN_DECLS
/* *INDENT-OFF* */
__BEGIN_DECLS
/* *INDENT-ON* */
#endif

extern void *setsockpath(void);
extern struct sockpath *getsockpath(void *handle);
extern struct sockpath *getsockpathent(int domain, int type, int protocol);
extern void freesockpathent(struct sockpath *sockpath);
extern int endsockpath(void *handle);

extern void sp_perror(const char *msg);
extern char *sp_sperror(void);

#ifdef __END_DECLS
/* *INDENT-OFF* */
__END_DECLS
/* *INDENT-ON* */
#endif

#endif				/* __SYS_SOCKPATH_H__ */
