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

#ifndef __ISODE_LOGGER_H__
#define __ISODE_LOGGER_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* logger.h - logging routines */

/* 
 * Header: /xtel/isode/isode/h/RCS/logger.h,v 9.0 1992/06/16 12:17:57 isode Rel
 *
 *
 * Log: logger.h,v
 * Revision 9.0  1992/06/16  12:17:57  isode
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

#ifndef	_LOGGER_
#define	_LOGGER_

#include "manifest.h"

typedef struct ll_struct {
	char *ll_file;			/* path name to logging file */

	char *ll_hdr;			/* text to put in opening line */
	char *ll_dhdr;			/* dynamic header - changes */

	int ll_events;			/* interesting events */
#define	LLOG_NONE	0
#define	LLOG_FATAL	0x01		/* fatal errors */
#define	LLOG_EXCEPTIONS	0x02		/* exceptional events */
#define	LLOG_NOTICE	0x04		/* informational notices */
#define	LLOG_PDUS	0x08		/* PDU printing */
#define	LLOG_TRACE	0x10		/* program tracing */
#define	LLOG_DEBUG	0x20		/* full debugging */
#define	LLOG_ALL	0xff
#define	LLOG_MASK \
    "\020\01FATAL\02EXCEPTIONS\03NOTICE\04PDUS\05TRACE\06DEBUG"

	int ll_syslog;			/* interesting events to send to syslog */
	/* takes same values as ll_events */

	int ll_msize;			/* max size for log, in Kbytes */

	int ll_stat;			/* assorted switches */
#define	LLOGNIL		0x00
#define	LLOGCLS		0x01		/* keep log closed, except when writing */
#define	LLOGCRT		0x02		/* create log if necessary */
#define	LLOGZER		0x04		/* truncate log when limits reached */
#define	LLOGERR		0x08		/* log closed due to (soft) error */
#define	LLOGTTY		0x10		/* also log to stderr */
#define	LLOGHDR		0x20		/* static header allocated */
#define	LLOGDHR		0x40		/* dynamic header allocated */

	int ll_fd;			/* file descriptor */
} LLog;

#define	SLOG(lp,event,what,args) \
if (lp -> ll_events & (event)) { \
    (void) ll_log (lp, event, what, "%s", ll_preset args); \
} \
else

#ifndef	LLOG
#define	LLOG(lp,event,args)	SLOG (lp, event, NULLCP, args)
#endif

#ifdef	DEBUG
#define	DLOG(lp,event,args)	SLOG (lp, event, NULLCP, args)
#else
#define	DLOG(lp,event,args)
#endif

#ifdef	DEBUG

#ifdef PEPSY_VERSION

#ifdef __STDC__

#define	PLOGP(lp,args,pe,text,rw) \
    if ((lp) -> ll_events & LLOG_PDUS) { \
	pvpdu (lp, print_##args##_P, pe, text, rw); \
    } \
    else

#define	PLOG(lp,fnx,pe,text,rw)	\
    if ((lp) -> ll_events & LLOG_PDUS) { \
	pvpdu (lp, fnx##_P, pe, text, rw); \
    } \
    else

#else

#define	PLOGP(lp,args,pe,text,rw) \
    if ((lp) -> ll_events & LLOG_PDUS) { \
	pvpdu (lp, print_/**/args/**/_P, pe, text, rw); \
    } \
    else

#define	PLOG(lp,fnx,pe,text,rw)	\
    if ((lp) -> ll_events & LLOG_PDUS) { \
	pvpdu (lp, fnx/**/_P, pe, text, rw); \
    } \
    else

#endif

#else				/* !PEPSY_VERSION */

#define	PLOG(lp,fnx,pe,text,rw)	\
    if ((lp) -> ll_events & LLOG_PDUS) { \
	vpdu (lp, fnx, pe, text, rw); \
    } \
    else

#endif				/* !PEPSY_VERSION */

#ifdef	lint
#undef	PLOGP
#define	pvpdu(lp,cookie,pe,text,rw) \
	_pvpdu(lp, pe, text, rw)
#define	PLOGP(lp,args,pe,text,rw) \
	_pvpdu (lp, pe, text, rw);
#endif

#ifndef PLOGP
#define	PLOGP(lp,args,pe,text,rw) \
    if ((lp) -> ll_events & LLOG_PDUS) { \
	pvpdu (lp, 0, (struct modtype *) 0, pe, text, rw); \
    } \
    else
#endif

#else				/* !DEBUG */
#define	PLOG(lp,fnx,pe,text,rw)
#define	PLOGP(lp,args,pe,text,rw)
#endif

int ll_open();
int ll_log(), _ll_log();
int ll_close();

void ll_hdinit();
void ll_dbinit();

int ll_printf();
int ll_sync();

char *ll_preset();

int ll_check();

int ll_defmhdr();
IFP ll_setmhdr();
#endif

/******************************/
/* should be removed finally: */
/******************************/

#ifdef ULTRIX_X25_DEMSA
/* for Trace Information of module names */
#include <stdio.h>
char our_global_buffer[BUFSIZ];
#endif

#endif				/* __ISODE_LOGGER_H__ */
