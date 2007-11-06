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

/* rtsaplose.c - RTPM: you lose */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/rtsap/RCS/rtsaplose.c,v 9.0 1992/06/16 12:37:45 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/rtsap/RCS/rtsaplose.c,v 9.0 1992/06/16 12:37:45 isode Rel
 *
 *
 * Log: rtsaplose.c,v
 * Revision 9.0  1992/06/16  12:37:45  isode
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
#include <varargs.h>
#include "rtpkt.h"
#include "tailor.h"

#ifndef	lint
static int _rtsaplose();
#endif

/*  */

#ifndef	lint
int
rtpktlose(va_alist)
	va_dcl
{
	int reason, result, value;
	register struct assocblk *acb;
	register struct RtSAPindication *rti;
	register struct RtSAPabort *rta;
	va_list ap;

	va_start(ap);

	acb = va_arg(ap, struct assocblk *);
	rti = va_arg(ap, struct RtSAPindication *);
	reason = va_arg(ap, int);

	result = _rtsaplose(rti, reason, ap);

	va_end(ap);

	if ((rta = &rti->rti_abort)->rta_cc > 0) {
		SLOG(rtsap_log, LLOG_EXCEPTIONS, NULLCP,
		     ("rtpktlose [%s] %*.*s", RtErrString(rta->rta_reason),
		      rta->rta_cc, rta->rta_cc, rta->rta_data));
	} else
		SLOG(rtsap_log, LLOG_EXCEPTIONS, NULLCP,
		     ("rtpktlose [%s]", RtErrString(rta->rta_reason)));

	if (acb == NULLACB || acb->acb_fd == NOTOK || acb->acb_rtpktlose == NULLIFP)
		return result;

	switch (reason) {
	case RTS_PROTOCOL:
		value = ABORT_PROTO;
		break;

	case RTS_CONGEST:
		value = ABORT_TMP;
		break;

	default:
		value = ABORT_LSP;
		break;
	}

	(*acb->acb_rtpktlose) (acb, value);

	return result;
}
#else
/* VARARGS5 */

int
rtpktlose(acb, rti, reason, what, fmt)
	struct assocblk *acb;
	struct RtSAPindication *rti;
	int reason;
	char *what, *fmt;
{
	return rtpktlose(acb, rti, reason, what, fmt);
}
#endif

/*  */

#ifndef	lint
int
rtsaplose(va_alist)
	va_dcl
{
	int reason, result;
	struct RtSAPindication *rti;
	va_list ap;

	va_start(ap);

	rti = va_arg(ap, struct RtSAPindication *);
	reason = va_arg(ap, int);

	result = _rtsaplose(rti, reason, ap);

	va_end(ap);

	return result;
}
#else
/* VARARGS4 */

int
rtsaplose(rti, reason, what, fmt)
	struct RtSAPindication *rti;
	int reason;
	char *what, *fmt;
{
	return rtsaplose(rti, reason, what, fmt);
}
#endif

/*  */

#ifndef	lint
static int
_rtsaplose(rti, reason, ap)		/* what, fmt, args ... */
	register struct RtSAPindication *rti;
	int reason;
	va_list ap;
{
	register char *bp;
	char buffer[BUFSIZ];
	register struct RtSAPabort *rta;

	if (rti) {
		bzero((char *) rti, sizeof *rti);
		rti->rti_type = RTI_ABORT;
		rta = &rti->rti_abort;

		xsprintf(bp = buffer, ap);
		bp += strlen(bp);

		rta->rta_peer = 0;
		rta->rta_reason = reason;
		copyRtSAPdata(buffer, bp - buffer, rta);
	}

	return NOTOK;
}
#endif
