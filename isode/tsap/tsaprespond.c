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

/* tsaprespond.c - TPM: responder */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/tsap/RCS/tsaprespond.c,v 9.0 1992/06/16 12:40:39 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/tsap/RCS/tsaprespond.c,v 9.0 1992/06/16 12:40:39 isode Rel
 *
 *
 * Log: tsaprespond.c,v
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
#include "tpkt.h"
#include "tailor.h"

#ifdef X25
#include "x25.h"
#endif

/*    T-CONNECT.INDICATION */

int
TInit(vecp, vec, ts, td)
	register int vecp;
	register char **vec;
	register struct TSAPstart *ts;
	register struct TSAPdisconnect *td;
{
	register struct tsapblk *tb;

	isodetailor(NULLCP, 0);

	if (vecp < 3)
		return tsaplose(td, DR_PARAMETER, NULLCP, "bad initialization vector");
	missingP(vec);
	missingP(ts);
	missingP(td);

	if ((tb = newtblk()) == NULL)
		return tsaplose(td, DR_CONGEST, NULLCP, "out of memory");

	vec += vecp - 2;
	switch (*vec[0]) {
	case NT_TCP:
#ifdef	TCP
		if (tcprestore(tb, vec[0] + 1, td) == NOTOK)
			goto out;
		break;
#else
		goto not_supported;
#endif

	case NT_X25:
#ifdef	X25
		if (x25restore(tb, vec[0] + 1, td) == NOTOK)
			goto out;
		break;
#else
		goto not_supported;
#endif

	case NT_X2584:
#ifdef AEF_NSAP
		if (x25nsaprestore(tb, vec[0] + 1, td) == NOTOK)
			goto out;
		break;
#else
		goto not_supported;
#endif

	case NT_BSD:
#ifdef	BSD_TP4
		if (tp4restore(tb, vec[0] + 1, td) == NOTOK)
			goto out;
		break;
#else
		goto not_supported;
#endif

	case NT_SUN:
#ifdef	SUN_TP4
		if (tp4restore(tb, vec[0] + 1, td) == NOTOK)
			goto out;
		break;
#else
		goto not_supported;
#endif
	case NT_TLI:
#ifdef TLI_TP
		if (tp4restore(tb, vec[0] + 1, td) == NOTOK)
			goto out;
		break;
#else
		goto not_supported;
#endif
	case NT_XTI:
#ifdef XTI_TP
		if (tp4restore(tb, vec[0] + 1, td) == NOTOK)
			goto out;
		break;
#else
		goto not_supported;
#endif

	default:
		(void) tsaplose(td, DR_PARAMETER, NULLCP,
				"unknown network type: 0x%x (%c)", *vec[0], *vec[0]);
		goto out;
	}
	bzero(vec[0], strlen(vec[0]));

	if ((*tb->tb_startPfnx) (tb, vec[1], ts, td) == NOTOK)
		goto out;
	bzero(vec[1], strlen(vec[1]));

	*vec = NULL;

	return OK;

      not_supported:;
	(void) tsaplose(td, DR_PARAMETER, NULLCP,
			"not configured for network type: 0x%x (%c)", *vec[0], *vec[0]);

      out:;
	freetblk(tb);

	return NOTOK;
}

/*    T-CONNECT.RESPONSE */

int
TConnResponse(sd, responding, expedited, data, cc, qos, td)
	int sd;
	register struct TSAPaddr *responding;
	int expedited, cc;
	char *data;
	struct QOStype *qos;
	register struct TSAPdisconnect *td;
{
	int result;
	register struct tsapblk *tb;
	struct tsapADDR tas;

	if ((tb = findtblk(sd)) == NULL || (tb->tb_flags & TB_CONN))
		return tsaplose(td, DR_PARAMETER, NULLCP, "invalid transport descriptor");
#ifdef	notdef
	missingP(responding);
#endif
	if (responding) {
		copyTSAPaddrY(responding, &tas);
		if (bcmp((char *) &tb->tb_responding, (char *) &tas, sizeof tas))
			tb->tb_responding = tas;	/* struct copy */
		else
			responding = NULLTA;
	}
	if (expedited && !(tb->tb_flags & TB_EXPD))
		return tsaplose(td, DR_PARAMETER, NULLCP, "expedited service not available");
	toomuchP(data, cc, TC_SIZE, "initial");
#ifdef	notdef
	missingP(qos);
#endif
	missingP(td);

	if (!expedited)
		tb->tb_flags &= ~TB_EXPD;

	if ((result = (*tb->tb_acceptPfnx) (tb, responding ? 1 : 0, data, cc, qos, td)) == NOTOK)
		freetblk(tb);
#ifdef	X25
	else if (tb->tb_flags & TB_X25)
		LLOG(x25_log, LLOG_NOTICE,
		     ("connection %d from %s", sd, na2str(&tb->tb_initiating.ta_addr)));
#endif

	return result;
}
