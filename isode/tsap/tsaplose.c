/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, version 3 of the license.

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

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

/* tsaplose.c - TPM: you lose */

/* 
 * Header: /xtel/isode/isode/tsap/RCS/tsaplose.c,v 9.0 1992/06/16 12:40:39 isode Rel
 *
 *
 * Log: tsaplose.c,v
 * Revision 9.0  1992/06/16  12:40:39  isode
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

/* LINTLIBRARY */

#include <stdio.h>
#ifdef HAVE_VARARGS_H
#include <varargs.h>
#else
#include <stdarg.h>
#endif
#include "tpkt.h"
#include "mpkt.h"
#include "tailor.h"
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef	LPP
#undef	MGMT
#endif

#ifndef	lint
static int _tsaplose();
#endif

#ifdef HAVE_VARARGS_H
#ifndef	lint
int
tpktlose(va_alist)
	va_dcl
{
	int reason, result;
	register struct tsapblk *tb;
	struct TSAPdisconnect tds;
	register struct TSAPdisconnect *td;
	va_list ap;

	va_start(ap);

	tb = va_arg(ap, struct tsapblk *);

	td = va_arg(ap, struct TSAPdisconnect *);

	if (td == NULL)
		td = &tds;

	reason = va_arg(ap, int);

	result = _tsaplose(td, reason, ap);

	va_end(ap);

	if (td->td_cc > 0) {
		SLOG(tsap_log, LLOG_EXCEPTIONS, NULLCP,
		     ("tpktlose [%s] %*.*s", TErrString(td->td_reason), td->td_cc,
		      td->td_cc, td->td_data));
	} else
		SLOG(tsap_log, LLOG_EXCEPTIONS, NULLCP,
		     ("tpktlose [%s]", TErrString(td->td_reason)));

#ifdef  MGMT
	if (tb->tb_manfnx)
		switch (reason) {
		case DR_REMOTE:
		case DR_CONGEST:
			(*tb->tb_manfnx) (CONGEST, tb);
			break;

		case DR_PROTOCOL:
		case DR_MISMATCH:
			(*tb->tb_manfnx) (PROTERR, tb);
			break;

		case DR_SESSION:
		case DR_ADDRESS:
		case DR_CONNECT:
		case DR_DUPLICATE:
		case DR_OVERFLOW:
		case DR_REFUSED:
			(*tb->tb_manfnx) (CONFIGBAD, tb);
			break;

		default:
			(*tb->tb_manfnx) (OPREQINBAD, tb);
		}
#endif

	(*tb->tb_losePfnx) (tb, reason, td);

	return result;
}
#else
/* VARARGS5 */

int
tpktlose(tb, td, reason, what, fmt)
	struct tsapblk *tb;
	struct TSAPdisconnect *td;
	int reason;
	char *what, *fmt;
{
	return tpktlose(tb, td, reason, what, fmt);
}
#endif
#else				/* HAVE_VARARGS_H */
int
tpktlose(struct tsapblk *tb, struct TSAPdisconnect *td, int reason, const char *what,
	 const char *fmt, ...)
{
	int result;
	struct TSAPdisconnect tds;
	va_list ap;

	va_start(ap, fmt);

	if (td == NULL)
		td = &tds;

	result = _tsaplose(td, reason, what, fmt, ap);

	va_end(ap);

	if (td->td_cc > 0) {
		SLOG(tsap_log, LLOG_EXCEPTIONS, NULLCP,
		     ("tpktlose [%s] %*.*s", TErrString(td->td_reason), td->td_cc,
		      td->td_cc, td->td_data));
	} else
		SLOG(tsap_log, LLOG_EXCEPTIONS, NULLCP,
		     ("tpktlose [%s]", TErrString(td->td_reason)));

#ifdef  MGMT
	if (tb->tb_manfnx)
		switch (reason) {
		case DR_REMOTE:
		case DR_CONGEST:
			(*tb->tb_manfnx) (CONGEST, tb);
			break;

		case DR_PROTOCOL:
		case DR_MISMATCH:
			(*tb->tb_manfnx) (PROTERR, tb);
			break;

		case DR_SESSION:
		case DR_ADDRESS:
		case DR_CONNECT:
		case DR_DUPLICATE:
		case DR_OVERFLOW:
		case DR_REFUSED:
			(*tb->tb_manfnx) (CONFIGBAD, tb);
			break;

		default:
			(*tb->tb_manfnx) (OPREQINBAD, tb);
		}
#endif

	(*tb->tb_losePfnx) (tb, reason, td);

	return result;
}
#endif				/* HAVE_VARARGS_H */

#ifdef HAVE_VARARGS_H
#ifndef	lint
int
tsaplose(va_alist)
	va_dcl
{
	int reason, result;
	struct TSAPdisconnect *td;
	va_list ap;

	va_start(ap);

	td = va_arg(ap, struct TSAPdisconnect *);
	reason = va_arg(ap, int);

	result = _tsaplose(td, reason, ap);

	va_end(ap);

	return result;

}
#else
/* VARARGS4 */

int
tsaplose(td, reason, what, fmt)
	struct TSAPdisconnect *td;
	int reason;
	char *what, *fmt;
{
	return tsaplose(td, reason, what, fmt);
}
#endif

#ifndef	lint
static int
_tsaplose(td, reason, ap)		/* what, fmt, args ... */
	register struct TSAPdisconnect *td;
	int reason;
	va_list ap;
{
	register char *bp;
	char buffer[BUFSIZ];

	if (td) {
		bzero((char *) td, sizeof *td);

		xsprintf(bp = buffer, ap);
		bp += strlen(bp);

		td->td_reason = reason;
		copyTSAPdata(buffer, bp - buffer, td);
	}

	return NOTOK;
}
#endif
#else				/* HAVE_VARARGS_H */
static int
_tsaplose(struct TSAPdisconnect *td, int reason, const char *what, const char *fmt, va_list ap)
{
	register char *bp;
	char buffer[BUFSIZ];

	if (td) {
		bzero((char *) td, sizeof *td);

		xsprintf((bp = buffer), what, fmt, ap);
		bp += strlen(bp);

		td->td_reason = reason;
		copyTSAPdata(buffer, bp - buffer, td);
	}
	return NOTOK;
}

int
tsaplose(struct TSAPdisconnect *td, int reason, const char *what, const char *fmt, ...)
{
	va_list ap;
	int result;

	va_start(ap, fmt);
	result = _tsaplose(td, reason, what, fmt, ap);
	va_end(ap);
	return (result);
}
#endif				/* HAVE_VARARGS_H */
