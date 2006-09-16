/*****************************************************************************

 @(#) $Id$

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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ifndef __SYS_SOCKPATH_H__
#define __SYS_SOCKPATH_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2006 OpenSS7 Corporation."

#define SOCK2PATH		"/etc/sock2path"
#define	SOCKPATH		"SOCKPATH"

struct sockpath {
	int sp_domain;
	int sp_type;
	int sp_protocol;
	char *sp_path;
};

#ifdef _REENTRANT
extern int *__sperror(void);
#define sp_error (*(__sperror()))
extern char *__sperrbuf(void);
#define sp_errbuf (*(__sperrbuf()))
#else
#warning Compiled without _REENTRANT defined!
extern int sp_error;
extern char *sp_errbuf;
#endif

/* error codes */
#define SP_NOERROR		0
#define SP_NOMEM		1
#define SP_NOSET		2
#define SP_OPENFAIL		3
#define SP_BADLINE		4
#define SP_NOTFOUND		5
#define SP_NOMOREENTRIES	6
#define SP_ERROR_MAX		7

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
