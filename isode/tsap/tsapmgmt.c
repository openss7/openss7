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

/* tsapmgmt.c - management info reporting routines */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/tsap/RCS/tsapmgmt.c,v 9.0 1992/06/16 12:40:39 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/tsap/RCS/tsapmgmt.c,v 9.0 1992/06/16 12:40:39 isode Rel
 *
 *
 * Log: tsapmgmt.c,v
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
#include <varargs.h>
#include "tpkt.h"
#include "mpkt.h"

#ifdef  MGMT
#include "internet.h"

#define LOCALTHLD 128		/* local threshold for reporting (bytes) */

/*  */

static long pid = OK;
static int ManSoc;
static struct MReport TsapInfo;
static struct qbuf data;

/*  */

/* VARARGS2 */

int
TManGen(va_alist)
	va_dcl
{
	int a, result;
	struct TSAPaddr *b;
	unsigned int type;
	register struct tsapblk *tb;
	va_list ap;

	va_start(ap);

	type = va_arg(ap, unsigned int);
	tb = va_arg(ap, struct tsapblk *);

	a = 0, b = NULLTA;
	switch (type) {
	case USERDT:
	case USERDR:
		a = va_arg(ap, int);

		break;

	case STARTLISTEN:
	case ENDLISTEN:
		b = va_arg(ap, struct TSAPaddr *);

		break;

	default:
		break;
	}

	result = TManGenAux(type, tb, a, b);

	va_end(ap);

	return result;
}

/*  */

#define SendMReport()		write_udp_socket (ManSoc, &data)

static int
TManGenAux(type, tb, a, b)
	unsigned int type;
	struct tsapblk *tb;
	int a;
	struct TSAPaddr *b;
{
	if (pid == NOTOK)
		return NOTOK;

	if (pid == OK && ManInit() == NOTOK)
		return (pid = NOTOK);

	switch (TsapInfo.type = type) {
	case USERDT:
		tb->tb_pdus++;
		if ((tb->tb_bytes += a) < LOCALTHLD)
			break;
		TsapInfo.cid = tb->tb_fd;
		TsapInfo.u.gp.a = tb->tb_bytes;
		TsapInfo.u.gp.b = tb->tb_pdus;
		tb->tb_bytes = 0;
		break;

	case USERDR:
		tb->tb_pdur++;
		if ((tb->tb_byter += a) < LOCALTHLD)
			return OK;
		TsapInfo.cid = tb->tb_fd;
		TsapInfo.u.gp.a = tb->tb_byter;
		TsapInfo.u.gp.b = tb->tb_pdur;
		tb->tb_byter = 0;
		break;

	case OPREQIN:
	case OPREQOUT:
		TsapInfo.cid = tb->tb_fd;
		bcopy(tb->tb_responding.ta_selector,
		      TsapInfo.u.taddr.tsel,
		      TsapInfo.u.taddr.tsel_len = tb->tb_responding.ta_selectlen);
		/* struct copy */
		TsapInfo.u.taddr.nsap = tb->tb_responding.ta_addr;
		if (SendMReport() == NOTOK)
			return NOTOK;

		TsapInfo.type = SOURCEADDR;
		bcopy(tb->tb_initiating.ta_selector,
		      TsapInfo.u.taddr.tsel,
		      TsapInfo.u.taddr.tsel_len = tb->tb_initiating.ta_selectlen);
		/* struct copy */
		TsapInfo.u.taddr.nsap = tb->tb_initiating.ta_addr;
		break;

	case DISCREQ:
		TsapInfo.cid = tb->tb_fd;
		TsapInfo.u.gp.a = tb->tb_bytes;
		TsapInfo.u.gp.b = tb->tb_byter;
		TsapInfo.u.gp.c = tb->tb_pdus;
		TsapInfo.u.gp.d = tb->tb_pdur;
		break;

	case PROTERR:
	case CONGEST:
	case CONFIGBAD:
	case OPREQINBAD:
	case OPREQOUTBAD:
		TsapInfo.cid = tb->tb_fd;
		break;

	case STARTLISTEN:
	case ENDLISTEN:
		bcopy(b->ta_selector,
		      TsapInfo.u.taddr.tsel, TsapInfo.u.taddr.tsel_len = b->ta_selectlen);
		TsapInfo.u.taddr.nsap = b->ta_addrs[0];	/* struct copy */
		break;

	default:
		return NOTOK;
	}
	return SendMReport();
}

/*  */

static int
ManInit()
{
	struct sockaddr_in sin;
	register struct sockaddr_in *sock = &sin;
	register struct servent *sp;
	register struct hostent *hp;

	if ((sp = getservbyname("manager", "udp")) == NULL
	    || (hp = gethostbyname("localhost")) == NULL)
		return NOTOK;

	bzero((char *) sock, sizeof *sock);
	sock->sin_family = hp->h_addrtype;
	inaddr_copy(hp, sock);
	if ((ManSoc = start_udp_client(sock, 0, SO_DONTROUTE, 0)) == NOTOK)
		return NOTOK;

	sock->sin_port = sp->s_port;
	if (join_udp_server(ManSoc, sock) == NOTOK)
		return NOTOK;

	TsapInfo.id = pid = getpid();
	data.qb_data = (char *) &TsapInfo;
	data.qb_len = sizeof TsapInfo;

	return OK;
}
#else
int
_tsapmgmt_stub()
{
}
#endif
