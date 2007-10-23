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

/* daplose.c - DAP: Support for directory protocol mappings */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/dsap/net/RCS/daplose.c,v 9.0 1992/06/16 12:14:05 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/dsap/net/RCS/daplose.c,v 9.0 1992/06/16 12:14:05 isode Rel
 *
 *
 * Log: daplose.c,v
 * Revision 9.0  1992/06/16  12:14:05  isode
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
#include "tailor.h"
#include "quipu/dap2.h"

#ifndef	lint
static int _daplose();
static int _dapreject();
#endif

/*  */

#ifndef	lint
int
daplose(va_alist)
	va_dcl
{
	int reason, result;
	struct DAPindication *di;
	va_list ap;

	va_start(ap);

	di = va_arg(ap, struct DAPindication *);
	reason = va_arg(ap, int);

	result = _daplose(di, reason, ap);

	va_end(ap);

	return result;
}
#else
/* VARARGS4 */

int
daplose(di, reason, what, fmt)
	struct DAPindication *di;
	int reason;
	char *what, *fmt;
{
	return daplose(di, reason, what, fmt);
}
#endif

/*  */

#ifndef	lint
static int
_daplose(di, reason, ap)		/* what, fmt, args ... */
	register struct DAPindication *di;
	int reason;
	va_list ap;
{
	register char *bp;
	char buffer[BUFSIZ];
	struct DAPabort *da;

	if (di) {
		bzero((char *) di, sizeof *di);
		di->di_type = DI_ABORT;
		da = &(di->di_abort);
		da->da_reason = reason;

		asprintf(bp = buffer, ap);
		bp += strlen(bp);

		copyDAPdata(buffer, bp - buffer, da);
	}

	return NOTOK;
}
#endif

#ifndef	lint
int
dapreject(va_alist)
	va_dcl
{
	int reason, id, result;
	struct DAPindication *di;
	va_list ap;

	va_start(ap);

	di = va_arg(ap, struct DAPindication *);
	reason = va_arg(ap, int);
	id = va_arg(ap, int);

	result = _dapreject(di, reason, id, ap);

	va_end(ap);

	return result;
}
#else
/* VARARGS4 */

int
dapreject(di, reason, id, what, fmt)
	struct DAPindication *di;
	int reason;
	int id;
	char *what, *fmt;
{
	return dapreject(di, reason, id, what, fmt);
}
#endif

/*  */

#ifndef	lint
static int
_dapreject(di, reason, id, ap)		/* what, fmt, args ... */
	register struct DAPindication *di;
	int reason;
	int id;
	va_list ap;
{
	register char *bp;
	char buffer[BUFSIZ];
	struct DAPpreject *dp;

	if (di) {
		bzero((char *) di, sizeof *di);
		di->di_type = DI_PREJECT;
		dp = &(di->di_preject);
		dp->dp_id = id;
		dp->dp_reason = reason;

		asprintf(bp = buffer, ap);
		bp += strlen(bp);

		copyDAPdata(buffer, bp - buffer, dp);
	}

	return (NOTOK);
}
#endif
