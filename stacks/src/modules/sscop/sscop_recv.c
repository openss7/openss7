/*****************************************************************************

 @(#) $RCSfile: sscop_recv.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/08/21 10:14:59 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2002  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@dallas.net>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

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
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2004/08/21 10:14:59 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: sscop_recv.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/08/21 10:14:59 $"

static char const ident[] =
    "$RCSfile: sscop_recv.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/08/21 10:14:59 $";

/*
 *  =========================================================================
 *
 *  SSCOP Peer --> SSCOP Primitives (Received Messages)
 *
 *  =========================================================================
 */
/*
 *  RECV BGN.indication N(W), N(SQ), N(S), SSCOP-UU
 *  -------------------------------------------------------------------------
 */
static int sscop_recv_bgn(q, pdu)
	const queue_t *q;
	const mblk_t *pdu;
{
	mblk_t *mp;
	struct sscop_bgn *m = (struct sscop_bgn *) (pdu->b_cont->b_wptr - sizeof(*m));
}

/*
 *  RECV BGAK.indication N(W), N(SQ), N(S), SSCOP-UU
 *  -------------------------------------------------------------------------
 */
static int sscop_recv_bgak(q, pdu)
	const queue_t *q;
	const mblk_t *pdu;
{
	mblk_t *mp;
	struct sscop_bgak *m = (struct sscop_bgak *) (pdu->b_cont->b_wptr - sizeof(*m));
}

/*
 *  RECV BGREJ.indication SSCOP-UU
 *  -------------------------------------------------------------------------
 */
static int sscop_recv_bgrej(q, pdu)
	const queue_t *q;
	const mblk_t *pdu;
{
	mblk_t *mp;
	struct sscop_bgrej *m = (struct sscop_bgrej *) (pdu->b_cont->b_wptr - sizeof(*m));
}

/*
 *  RECV END.indication [src] SSCOP-UU
 *  -------------------------------------------------------------------------
 */
static int sscop_recv_end(q, pdu)
	const queue_t *q;
	const mblk_t *pdu;
{
	mblk_t *mp;
	struct sscop_end *m = (struct sscop_end *) (pdu->b_cont->b_wptr - sizeof(*m));
}

/*
 *  RECV ENDAK.indication ()
 *  -------------------------------------------------------------------------
 */
static int sscop_recv_endak(q, pdu)
	const queue_t *q;
	const mblk_t *pdu;
{
	mblk_t *mp;
	struct sscop_endak *m = (struct sscop_endak *) (pdu->b_cont->b_wptr - sizeof(*m));
}

/*
 *  RECV RS.indication N(W), N(SQ), N(S), SSCOP-UU
 *  -------------------------------------------------------------------------
 */
static int sscop_recv_rs(q, pdu)
	const queue_t *q;
	const mblk_t *pdu;
{
	mblk_t *mp;
	struct sscop_rs *m = (struct sscop_rs *) (pdu->b_cont->b_wptr - sizeof(*m));
}

/*
 *  RECV RSAK.indication N(W), N(SQ), N(S)
 *  -------------------------------------------------------------------------
 */
static int sscop_recv_rsak(q, pdu)
	const queue_t *q;
	const mblk_t *pdu;
{
	mblk_t *mp;
	struct sscop_rsak *m = (struct sscop_rsak *) (pdu->b_cont->b_wptr - sizeof(*m));
}

/*
 *  RECV ER.indication N(W), N(SQ), N(S)
 *  -------------------------------------------------------------------------
 */
static int sscop_recv_er(q, pdu)
	const queue_t *q;
	const mblk_t *pdu;
{
	mblk_t *mp;
	struct sscop_er *m = (struct sscop_er *) (pdu->b_cont->b_wptr - sizeof(*m));
}

/*
 *  RECV ERAK.indication N(W), N(SQ), N(S)
 *  -------------------------------------------------------------------------
 */
static int sscop_recv_erak(q, pdu)
	const queue_t *q;
	const mblk_t *pdu;
{
	mblk_t *mp;
	struct sscop_erak *m = (struct sscop_erak *) (pdu->b_cont->b_wptr - sizeof(*m));
}

/*
 *  RECV SD.indication N(S), OOS, MU
 *  -------------------------------------------------------------------------
 */
static int sscop_recv_sd(q, pdu)
	const queue_t *q;
	const mblk_t *pdu;
{
	mblk_t *mp;
	struct sscop_sd *m = (struct sscop_sd *) (pdu->b_cont->b_wptr - sizeof(*m));
}

/*
 *  RECV POLL.indication N(S), N(PS), N(SQ)
 *  -------------------------------------------------------------------------
 */
static int sscop_recv_poll(q, pdu)
	const queue_t *q;
	const mblk_t *pdu;
{
	mblk_t *mp;
	struct sscop_poll *m = (struct sscop_poll *) (pdu->b_cont->b_wptr - sizeof(*m));
}

/*
 *  RECV STAT.indicaiton N(R), N(MR), N(PS), N(SQ), N(SS), [list]
 *  -------------------------------------------------------------------------
 */
static int sscop_recv_stat(q, pdu)
	const queue_t *q;
	const mblk_t *pdu;
{
	mblk_t *mp;
	struct sscop_stat *m = (struct sscop_stat *) (pdu->b_cont->b_wptr - sizeof(*m));
}

/*
 *  RECV USTAT.indication N(R), N(MR), N(PS), N(SQ), N(SS), [list]
 *  -------------------------------------------------------------------------
 */
static int sscop_recv_ustat(q, pdu)
	const queue_t *q;
	const mblk_t *pdu;
{
	mblk_t *mp;
	struct sscop_ustat *m = (struct sscop_ustat *) (pdu->b_cont->b_wptr - sizeof(*m));
}

/*
 *  RECV UD.indication MU
 *  -------------------------------------------------------------------------
 */
static int sscop_recv_ud(q, pdu)
	const queue_t *q;
	const mblk_t *pdu;
{
	mblk_t *mp;
	struct sscop_ud *m = (struct sscop_ud *) (pdu->b_cont->b_wptr - sizeof(*m));
}

/*
 *  RECV MD.indication MU
 *  -------------------------------------------------------------------------
 */
static int sscop_recv_md(q, pdu)
	const queue_t *q;
	const mblk_t *pdu;
{
	mblk_t *mp;
	struct sscop_md *m = (struct sscop_md *) (pdu->b_cont->b_wptr - sizeof(*m));
}

static int (*sscop_pdu[]) (const queue_t *, const mblk_t *) = {
	sscop_recv_bgn,		/* SSCOP_BGN */
	    sscop_recv_bgak,	/* SSCOP_BGAK */
	    sscop_recv_bgrej,	/* SSCOP_BGREJ */
	    sscop_recv_end,	/* SSCOP_END */
	    sscop_recv_endak,	/* SSCOP_ENDAK */
	    sscop_recv_rs,	/* SSCOP_RS */
	    sscop_recv_rsak,	/* SSCOP_RSAK */
	    sscop_recv_er,	/* SSCOP_ER */
	    sscop_recv_erak,	/* SSCOP_ERAK */
	    sscop_recv_sd,	/* SSCOP_SD */
	    sscop_recv_poll,	/* SSCOP_POLL */
	    sscop_recv_stat,	/* SSCOP_STAT */
	    sscop_recv_ustat,	/* SSCOP_USTAT */
	    sscop_recv_ud,	/* SSCOP_UD */
	    sscop_recv_md	/* SSCOP_MD */
};

/*
 *  RECV SSCOP MESSAGE
 *  -------------------------------------------------------------------------
 *
 */
int sscop_recv_msg(q, mp)
	const queue_t *q;
	const mblk_t *mp;
{
	struct sscop *sp = SSCOP_PRIV(q);
	int err = -EMSGSIZE;
	uint type;

	if (!mp)
		return -EFAULT;

	if (mp->b_cont)
		do {
			type = ((struct sscopchdr *) mp->b_cont->b_rptr)->type & SSCOP_CTYPE_MASK;
			/* 
			 *  TODO:  Do all the chunk size
			 */
			sscop_statistics.SctpStatRecChunks++;

			if (0 < type && type <= SSCOP_SHUTDOWN_COMPLETE)
				err = (*sscop_pdu[type]) (q, mp);
			else
				err = sscop_recv_unrec_ctype(q, mp);
		}
		while (err > 0);
	if (err != -EAGAIN && err != -ENOBUFS)
		freemsg(mp);
	return (err);
}
