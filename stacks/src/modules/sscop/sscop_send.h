/*****************************************************************************

 @(#) $Id: sscop_send.h,v 0.9.2.5 2008-04-29 07:11:13 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

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

 Last Modified $Date: 2008-04-29 07:11:13 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sscop_send.h,v $
 Revision 0.9.2.5  2008-04-29 07:11:13  brian
 - updating headers for release

 Revision 0.9.2.4  2007/08/14 12:18:51  brian
 - GPLv3 header updates

 Revision 0.9.2.3  2007/06/17 01:56:29  brian
 - updates for release, remove any later language

 *****************************************************************************/

#ifndef __SSCOP_SEND_H__
#define __SSCOP_SEND_H__

#ident "@(#) $RCSfile: sscop_send.h,v $ $Name:  $($Revision: 0.9.2.5 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

/*
 *  =========================================================================
 *
 *  SSCOP --> SSCOP Peer Primitives (Send Messages)
 *
 *  =========================================================================
 */
/*
 *  SEND BGN.request N(W), N(SQ), N(S), SSCOP-UU
 *  -------------------------------------------------------------------------
 */
static __inline__ int
sscop_send_bgn(sp, uu_ptr, uu_len)
	const struct sscop *sp;
	const caddr_t uu_ptr;
	const size_t uu_len;
{
	mblk_t *mp;
	const size_t plen = (4 - (uu_len & 0x3)) & 0x3;
	const size_t blen = 4 + uu_len + plen;
	const uint32_t ptype = (plen << 6) | SSCOP_BGN;

	if ((mp = allocb(blen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		bcopy(uu_ptr, mp->b_wptr, uu_len);
		mp->b_wptr += uu_len;
		bzero(mp->b_wptr, plen);
		mp->b_wptr += plen;
		*((uint32_t *) mp->b_wptr)++ = (((plen << 6) | SSCOP_BGN) << 24) | ns;
		sscop_send_msg(sp, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  SEND BGAK.request N(W), N(SQ), N(S), SSCOP-UU
 *  -------------------------------------------------------------------------
 */
static __inline__ int
sscop_send_bgak(sp, uu_ptr, uu_len)
	const struct sscop *sp;
	const caddr_t uu_ptr;
	const size_t uu_len;
{
	mblk_t *mp;
	struct sscop_bgak *m;
	const size_t plen = (4 - (uu_len & 0x3)) & 0x3;
	const size_t blen = sizeof(*m) + uu_len + plen;
	const u8 ptype = (plen << 6) | SSCOP_BGAK;

	if ((mp = allocb(blen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		bcopy(uu_ptr, mp->b_wptr, uu_len);
		mp->b_wptr += uu_len;
		bzero(mp->b_wptr, plen);
		mp->b_wptr += plen;

		m = (struct sscop_bgak *) mp->b_wptr;
		/* 
		 *  TODO: more
		 */
		mp->b_wptr += sizeof(*m);

		sscop_send_msg(sp, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  SEND BGREJ.request SSCOP-UU
 *  -------------------------------------------------------------------------
 */
static __inline__ int
sscop_send_bgrej(sp, uu_ptr, uu_len)
	const struct sscop *sp;
	const caddr_t uu_ptr;
	const size_t uu_len;
{
	mblk_t *mp;
	struct sscop_bgrej *m;
	const size_t plen = (4 - (uu_len & 0x3)) & 0x3;
	const size_t blen = sizeof(*m) + uu_len + plen;
	const u8 ptype = (plen << 6) | SSCOP_BGREJ;

	if ((mp = allocb(blen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		bcopy(uu_ptr, mp->b_wptr, uu_len);
		mp->b_wptr += uu_len;
		bzero(mp->b_wptr, plen);
		mp->b_wptr += plen;
		m = (struct sscop_bgrej *) mp->b_wptr;
		/* 
		 *  TODO: more
		 */
		mp->b_wptr += sizeof(*m);
		sscop_send_msg(sp, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  SEND END.request [src] SSCOP-UU
 *  -------------------------------------------------------------------------
 */
static __inline__ int
sscop_send_end(sp, uu_ptr, uu_len)
	const struct sscop *sp;
	const caddr_t uu_ptr;
	const size_t uu_len;
{
	mblk_t *mp;
	struct sscop_end *m;
	const size_t plen = (4 - (uu_len & 0x3)) & 0x3;
	const size_t blen = sizeof(*m) + uu_len + plen;
	const u8 ptype = (plen << 6) | SSCOP_END;

	if ((mp = allocb(blen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		bcopy(uu_ptr, mp->b_wptr, uu_len);
		mp->b_wptr += uu_len;
		bzero(mp->b_wptr, plen);
		mp->b_wptr += plen;
		m = (struct sscop_end *) mp->b_wptr;
		/* 
		 *  TODO: more
		 */
		mp->b_wptr += sizeof(*m);
		sscop_send_msg(sp, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  SEND ENDAK.request ()
 *  -------------------------------------------------------------------------
 */
static __inline__ int
sscop_send_endak(sp, uu_ptr, uu_len)
	const struct sscop *sp;
	const caddr_t uu_ptr;
	const size_t uu_len;
{
	mblk_t *mp;
	struct sscop_endak *m;
	const size_t plen = (4 - (uu_len & 0x3)) & 0x3;
	const size_t blen = sizeof(*m) + uu_len + plen;
	const u8 ptype = (plen << 6) | SSCOP_ENDAK;

	if ((mp = allocb(blen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		bcopy(uu_ptr, mp->b_wptr, uu_len);
		mp->b_wptr += uu_len;
		bzero(mp->b_wptr, plen);
		mp->b_wptr += plen;
		m = (struct sscop_endak *) mp->b_wptr;
		/* 
		 *  TODO: more
		 */
		mp->b_wptr += sizeof(*m);
		sscop_send_msg(sp, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  SEND RS.request N(W), N(SQ), N(S), SSCOP-UU
 *  -------------------------------------------------------------------------
 */
static __inline__ int
sscop_send_rs(sp, uu_ptr, uu_len)
	const struct sscop *sp;
	const caddr_t uu_ptr;
	const size_t uu_len;
{
	mblk_t *mp;
	struct sscop_rs *m;
	const size_t plen = (4 - (uu_len & 0x3)) & 0x3;
	const size_t blen = sizeof(*m) + uu_len + plen;
	const u8 ptype = (plen << 6) | SSCOP_RS;

	if ((mp = allocb(blen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		bcopy(uu_ptr, mp->b_wptr, uu_len);
		mp->b_wptr += uu_len;
		bzero(mp->b_wptr, plen);
		mp->b_wptr += plen;
		m = (struct sscop_rs *) mp->b_wptr;
		/* 
		 *  TODO: more
		 */
		mp->b_wptr += sizeof(*m);
		sscop_send_msg(sp, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  SEND RSAK.request N(W), N(SQ), N(S)
 *  -------------------------------------------------------------------------
 */
static __inline__ int
sscop_send_rsak(sp, uu_ptr, uu_len)
	const struct sscop *sp;
	const caddr_t uu_ptr;
	const size_t uu_len;
{
	mblk_t *mp;
	struct sscop_rsak *m;
	const size_t plen = (4 - (uu_len & 0x3)) & 0x3;
	const size_t blen = sizeof(*m) + uu_len + plen;
	const u8 ptype = (plen << 6) | SSCOP_RSAK;

	if ((mp = allocb(blen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		bcopy(uu_ptr, mp->b_wptr, uu_len);
		mp->b_wptr += uu_len;
		bzero(mp->b_wptr, plen);
		mp->b_wptr += plen;
		m = (struct sscop_rsak *) mp->b_wptr;
		/* 
		 *  TODO: more
		 */
		mp->b_wptr += sizeof(*m);
		sscop_send_msg(sp, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  SEND ER.request N(W), N(SQ), N(S)
 *  -------------------------------------------------------------------------
 */
static __inline__ int
sscop_send_er(sp, uu_ptr, uu_len)
	const struct sscop *sp;
	const caddr_t uu_ptr;
	const size_t uu_len;
{
	mblk_t *mp;
	struct sscop_er *m;
	const size_t plen = (4 - (uu_len & 0x3)) & 0x3;
	const size_t blen = sizeof(*m) + uu_len + plen;
	const u8 ptype = (plen << 6) | SSCOP_ER;

	if ((mp = allocb(blen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		bcopy(uu_ptr, mp->b_wptr, uu_len);
		mp->b_wptr += uu_len;
		bzero(mp->b_wptr, plen);
		mp->b_wptr += plen;
		m = (struct sscop_er *) mp->b_wptr;
		/* 
		 *  TODO: more
		 */
		mp->b_wptr += sizeof(*m);
		sscop_send_msg(sp, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  SEND ERAK.request N(W), N(SQ), N(S)
 *  -------------------------------------------------------------------------
 */
static __inline__ int
sscop_send_erak(sp, uu_ptr, uu_len)
	const struct sscop *sp;
	const caddr_t uu_ptr;
	const size_t uu_len;
{
	mblk_t *mp;
	struct sscop_erak *m;
	const size_t plen = (4 - (uu_len & 0x3)) & 0x3;
	const size_t blen = sizeof(*m) + uu_len + plen;
	const u8 ptype = (plen << 6) | SSCOP_ERAK;

	if ((mp = allocb(blen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		bcopy(uu_ptr, mp->b_wptr, uu_len);
		mp->b_wptr += uu_len;
		bzero(mp->b_wptr, plen);
		mp->b_wptr += plen;
		m = (struct sscop_erak *) mp->b_wptr;
		/* 
		 *  TODO: more
		 */
		mp->b_wptr += sizeof(*m);
		sscop_send_msg(sp, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  SEND SD.request N(S), OOS, MU
 *  -------------------------------------------------------------------------
 */
static __inline__ int
sscop_send_sd(sp, uu_ptr, uu_len)
	const struct sscop *sp;
	const caddr_t uu_ptr;
	const size_t uu_len;
{
	mblk_t *mp;
	const size_t plen = (4 - (uu_len & 0x3)) & 0x3;
	const size_t blen = 4 + uu_len + plen;
	const u8 ptype = (plen << 6) | SSCOP_SD;

	if ((mp = allocb(blen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		bcopy(uu_ptr, mp->b_wptr, uu_len);
		mp->b_wptr += uu_len;
		bzero(mp->b_wptr, plen);
		mp->b_wptr += plen;
		*((uint32_t *) mp->b_wptr)++ =
		    htonl((((plen << 6) | SSCOP_SD) << 24) | (sp->ns & 0xffffff));
		sscop_send_msg(sp, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  SEND POLL.request N(S), N(PS), N(SQ)
 *  -------------------------------------------------------------------------
 */
static __inline__ int
sscop_send_poll(sp)
	const struct sscop *sp;
{
	mblk_t *mp;
	static const size_t blen = 8;

	if ((mp = allocb(blen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		*((uint32_t *) mp->b_wptr)++ = htonl(sp->nps & 0xffffff);
		*((uint32_t *) mp->b_wptr)++ = htonl((SSCOP_POLL << 24) | (sp->ns & 0xffffff));
		sscop_send_msg(sp, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  SEND STAT.indicaiton N(R), N(MR), N(PS), N(SQ), N(SS), [list]
 *  -------------------------------------------------------------------------
 */
static __inline__ int
sscop_send_stat(sp)
	const struct sscop *sp;
{
	mblk_t *mp;
	const size_t blen = 12 + 4 * lnum;

	if ((mp = allocb(blen, BPRI_MED))) {
		int i;

		mp->b_datap->db_type = M_DATA;
		for (i = 0; i < lnum; i++)
			*((uint32_t *) mp->b_wptr)++ = htonl(sp->le[i] & 0xffffff);
		*((uint32_t *) mp->b_wptr)++ = htonl(sp->nps & 0xffffff);
		*((uint32_t *) mp->b_wptr)++ = htonl(sp->nmr & 0xffffff);
		*((uint32_t *) mp->b_wptr)++ = htonl((SSCOP_STAT << 24) | (sp->nr & 0xffffff));
		sscop_send_msg(sp, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  SEND USTAT.request N(R), N(MR), N(PS), N(SQ), N(SS), [list]
 *  -------------------------------------------------------------------------
 */
static __inline__ int
sscop_send_ustat(sp)
	const struct sscop *sp;
{
	mblk_t *mp;
	static const size_t blen = 16;

	if ((mp = allocb(blen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		bzero(mp->b_wptr, plen);
		mp->b_wptr += plen;
		*((uint32_t *) mp->b_wptr)++ = htonl(sp->le[0] & 0xffffff);
		*((uint32_t *) mp->b_wptr)++ = htonl(sp->le[1] & 0xffffff);
		*((uint32_t *) mp->b_wptr)++ = htonl(sp->nmr & 0xffffff);
		*((uint32_t *) mp->b_wptr)++ = htonl((SSCOP_USTAT << 24) | (sp->nr & 0xffffff));
		m = (struct sscop_ustat *) mp->b_wptr;
		/* 
		 *  TODO: more
		 */
		mp->b_wptr += sizeof(*m);
		sscop_send_msg(sp, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  SEND UD.request MU
 *  -------------------------------------------------------------------------
 */
static __inline__ int
sscop_send_ud(sp, uu_ptr, uu_len)
	const struct sscop *sp;
	const caddr_t uu_ptr;
	const size_t uu_len;
{
	mblk_t *mp;
	struct sscop_ud *m;
	const size_t plen = (4 - (uu_len & 0x3)) & 0x3;
	const size_t blen = sizeof(*m) + uu_len + plen;
	const u8 ptype = (plen << 6) | SSCOP_UD;

	if ((mp = allocb(blen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		bcopy(uu_ptr, mp->b_wptr, uu_len);
		mp->b_wptr += uu_len;
		bzero(mp->b_wptr, plen);
		mp->b_wptr += plen;
		m = (struct sscop_ud *) mp->b_wptr;
		/* 
		 *  TODO: more
		 */
		mp->b_wptr += sizeof(*m);
		sscop_send_msg(sp, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  SEND MD.request MU
 *  -------------------------------------------------------------------------
 */
static __inline__ int
sscop_send_md(sp, uu_ptr, uu_len)
	const struct sscop *sp;
	const caddr_t uu_ptr;
	const size_t uu_len;
{
	mblk_t *mp;
	struct sscop_md *m;
	const size_t plen = (4 - (uu_len & 0x3)) & 0x3;
	const size_t blen = sizeof(*m) + uu_len + plen;
	const u8 ptype = (plen << 6) | SSCOP_MD;

	if ((mp = allocb(blen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		bcopy(uu_ptr, mp->b_wptr, uu_len);
		mp->b_wptr += uu_len;
		bzero(mp->b_wptr, plen);
		mp->b_wptr += plen;
		m = (struct sscop_md *) mp->b_wptr;
		/* 
		 *  TODO: more
		 */
		mp->b_wptr += sizeof(*m);
		sscop_send_msg(sp, mp);
		return (0);
	}
	return (-ENOBUFS);
}

#endif				/* __SSCOP_SEND_H__ */
