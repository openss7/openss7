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

/* dg2ps.c - datagram-backed abstraction for PStreams */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/psap/RCS/dg2ps.c,v 9.0 1992/06/16 12:25:44 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/psap/RCS/dg2ps.c,v 9.0 1992/06/16 12:25:44 isode Rel
 *
 *
 * Log: dg2ps.c,v
 * Revision 9.0  1992/06/16  12:25:44  isode
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
#include "psap.h"

struct ps_dg {
	int ps_fd;
	int ps_maxsize;

	struct ps_inout {
		struct qbuf *pio_qb;
		char *pio_ptr;
		int pio_cnt;

		IFP pio_fnx;
	} ps_input, ps_output;

	IFP ps_check;
};

extern IFP set_check_fd();

/*  */

static int
dg_prime(ps, waiting)
	register PS ps;
	int waiting;
{
	struct qbuf *qb;
	register struct ps_dg *pt = (struct ps_dg *) ps->ps_addr;
	register struct ps_inout *pi = &pt->ps_input;

	switch (waiting) {
	case 0:
		if (pi->pio_cnt > 0)
			return OK;
		break;

	case 1:
	default:
		return (pi->pio_cnt > 0 ? DONE : OK);

	case -1:
		if (pi->pio_cnt <= 0)
			return OK;
		break;
	}

	if (pi->pio_qb != NULL) {
		qb_free(pi->pio_qb);
		pi->pio_qb = NULL;
	}
	pi->pio_cnt = 0;

	if (waiting < 0)
		return ps_seterr(ps, PS_ERR_EXTRA, NOTOK);

	if ((*pi->pio_fnx) (pt->ps_fd, &qb) == NOTOK)
		return ps_seterr(ps, PS_ERR_IO, NOTOK);

	if (pi->pio_qb = qb)
		pi->pio_ptr = qb->qb_data, pi->pio_cnt = qb->qb_len;
	else
		pi->pio_ptr = NULL, pi->pio_cnt = 0;

	return OK;
}

/* ARGSUSED */

static int
dg_read(ps, data, n, in_line)
	register PS ps;
	PElementData data;
	PElementLen n;
	int in_line;
{
	int cc;
	register struct ps_dg *pt = (struct ps_dg *) ps->ps_addr;
	register struct ps_inout *pi = &pt->ps_input;

	if ((cc = pi->pio_cnt) <= 0)
		return 0;
	if (cc > n)
		cc = n;

	bcopy(pi->pio_ptr, (char *) data, cc);
	pi->pio_ptr += cc, pi->pio_cnt -= cc;

	return cc;
}

/* ARGSUSED */

static int
dg_write(ps, data, n, in_line)
	register PS ps;
	PElementData data;
	PElementLen n;
	int in_line;
{
	register struct ps_dg *pt = (struct ps_dg *) ps->ps_addr;
	register struct ps_inout *po = &pt->ps_output;

	if (po->pio_cnt < n)
		return 0;

	bcopy((char *) data, po->pio_ptr, n);
	po->pio_ptr += n, po->pio_cnt -= n;

	return n;
}

static int
dg_flush(ps)
	register PS ps;
{
	register struct ps_dg *pt = (struct ps_dg *) ps->ps_addr;
	register struct ps_inout *po = &pt->ps_output;
	register struct qbuf *qb = po->pio_qb;

	qb->qb_len = po->pio_ptr - qb->qb_data;
	if ((*po->pio_fnx) (pt->ps_fd, qb) != qb->qb_len)
		return ps_seterr(ps, PS_ERR_IO, NOTOK);

	po->pio_ptr = qb->qb_data, po->pio_cnt = pt->ps_maxsize;

	return OK;
}

static int
dg_close(ps)
	register PS ps;
{
	register struct ps_dg *pt = (struct ps_dg *) ps->ps_addr;

	if (pt == NULL)
		return OK;

	if (pt->ps_input.pio_qb)
		qb_free(pt->ps_input.pio_qb);
	if (pt->ps_output.pio_qb)
		qb_free(pt->ps_output.pio_qb);

	(void) set_check_fd(pt->ps_fd, NULLIFP, NULLCP);

	free((char *) pt);

	return OK;
}

static int
dg_check(fd, data)
	int fd;
	caddr_t data;
{
	int n;
	PS ps = (PS) data;
	register struct ps_dg *pt = (struct ps_dg *) ps->ps_addr;

	if (pt->ps_check && (n = (*pt->ps_check) (fd)))
		return n;

	return (ps_prime(ps, 1) > 0 ? DONE : OK);
}

/*  */

int
dg_open(ps)
	register PS ps;
{
	ps->ps_primeP = dg_prime;
	ps->ps_readP = dg_read;
	ps->ps_writeP = dg_write;
	ps->ps_flushP = dg_flush;
	ps->ps_closeP = dg_close;

	return OK;
}

int
dg_setup(ps, fd, size, rfx, wfx, cfx)
	register PS ps;
	int fd, size;
	IFP rfx, wfx, cfx;
{
	register struct ps_dg *pt;
	register struct ps_inout *po;
	register struct qbuf *qb;

	if ((pt = (struct ps_dg *) calloc(1, sizeof *pt)) == NULL)
		return ps_seterr(ps, PS_ERR_NMEM, NOTOK);
	ps->ps_addr = (caddr_t) pt;

	pt->ps_fd = fd;
	pt->ps_maxsize = size;

	if ((qb = (struct qbuf *) malloc(sizeof *qb + (unsigned) pt->ps_maxsize))
	    == NULL)
		return ps_seterr(ps, PS_ERR_NMEM, NOTOK);
	qb->qb_forw = qb->qb_back = qb;
	qb->qb_len = 0;
	qb->qb_data = qb->qb_base;

	po = &pt->ps_output;
	po->pio_qb = qb;
	po->pio_ptr = qb->qb_data, po->pio_cnt = pt->ps_maxsize;
	if ((pt->ps_input.pio_fnx = rfx) == NULLIFP || (po->pio_fnx = wfx) == NULLIFP)
		return ps_seterr(ps, PS_ERR_XXX, NOTOK);

	pt->ps_check = cfx;
	(void) set_check_fd(fd, dg_check, (caddr_t) ps);

	return OK;
}
