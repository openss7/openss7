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

/* str2tpkt.c - read/write a TPDU thru a string */

/* 
 * Header: /xtel/isode/isode/tsap/RCS/str2tpkt.c,v 9.0 1992/06/16 12:40:39 isode Rel
 *
 *
 * Log: str2tpkt.c,v
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
#ifdef HAVE_STRING_H
#include <string.h>
#endif

static int readfnx(), getfnx(), writefnx(), putfnx();

char *
tpkt2str(t)
	struct tsapkt *t;
{
	int cc;
	char packet[BUFSIZ];
	static char buffer[2 * sizeof packet + 1];

	(void) writefnx((struct tsapblk *) NOTOK, packet, 0);
	if (tpkt2fd((struct tsapblk *) 0, t, putfnx) == NOTOK)
		return NULLCP;
	cc = writefnx((struct tsapblk *) NOTOK, NULLCP, 0);
	if (t->t_qbuf) {
		bcopy(t->t_qbuf->qb_data, packet + cc, t->t_qbuf->qb_len);
		cc += t->t_qbuf->qb_len;
	}
	buffer[explode(buffer, (u_char *) packet, cc)] = '\0';
	DLOG(tsap_log, LLOG_PDUS, ("write %d bytes, \"%s\"", strlen(buffer), buffer));
	return buffer;
}

struct tsapkt *
str2tpkt(buffer)
	char *buffer;
{
	char packet[BUFSIZ];
	register struct tsapkt *t;

	DLOG(tsap_log, LLOG_PDUS, ("read %d bytes, \"%s\"", strlen(buffer), buffer));
	(void) getfnx(NOTOK, NULLPKT, packet, implode((u_char *) packet, buffer, strlen(buffer)));
	t = fd2tpkt(0, getfnx, readfnx);
	return t;
}

static int
getfnx(fd, t, buffer, n)
	int fd;
	register struct tsapkt *t;
	char *buffer;
	int n;
{
	static int cc;

	if (fd == NOTOK) {
		(void) readfnx(NOTOK, buffer, cc = n);
		return OK;
	}
	t->t_length = cc + sizeof t->t_pkthdr;
	t->t_vrsn = TPKT_VRSN;
	if (readfnx(fd, (char *) &t->t_li, sizeof t->t_li)
	    != sizeof t->t_li)
		return DR_LENGTH;
	if (readfnx(fd, (char *) &t->t_code, sizeof t->t_code)
	    != sizeof t->t_code)
		return DR_LENGTH;
	return OK;
}

static int
readfnx(fd, buffer, n)
	int fd, n;
	char *buffer;
{
	register int i;
	static int cc;
	static char *bp;

	if (fd == NOTOK) {
		bp = buffer, cc = n;
		return OK;
	}
	if ((i = min(cc, n)) > 0) {
		bcopy(bp, buffer, n);
		bp += i, cc -= i;
	}
	return i;
}

static int
putfnx(tb, t, cp, n)
	struct tsapblk *tb;
	register struct tsapkt *t;
	char *cp;
	int n;
{
	register int cc;
	register struct udvec *uv;

	cc = sizeof t->t_li;
	if (writefnx(tb, (char *) &t->t_li, cc) != cc)
		return NOTOK;
	if (writefnx(tb, (char *) &t->t_code, sizeof t->t_code)
	    != sizeof t->t_code)
		return NOTOK;
	cc += sizeof t->t_code;
	if (writefnx(tb, cp, n) != n)
		return NOTOK;
	cc += n;
	if (t->t_vdata && writefnx(tb, t->t_vdata, t->t_vlen) != t->t_vlen)
		return NOTOK;
	cc += t->t_vlen;
	for (uv = t->t_udvec; uv->uv_base; uv++) {
		if (writefnx(tb, uv->uv_base, uv->uv_len) != uv->uv_len)
			return NOTOK;
		cc += uv->uv_len;
	}
	return cc;
}

static int
writefnx(tb, buffer, n)
	struct tsapblk *tb;
	int n;
	char *buffer;
{
	static int cc;
	static char *bp;

	if (tb) {
		if (buffer == NULLCP)
			return cc;
		bp = buffer, cc = 0;
		return OK;
	}
	bcopy(buffer, bp, n);
	bp += n, cc += n;
	return n;
}
