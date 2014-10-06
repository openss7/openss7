/*****************************************************************************

 @(#) $Id: sua_send.h,v 1.1.2.2 2010-11-28 14:21:37 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2010  Monavacon Limited <http://www.monavacon.com/>
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

 Last Modified $Date: 2010-11-28 14:21:37 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sua_send.h,v $
 Revision 1.1.2.2  2010-11-28 14:21:37  brian
 - remove #ident, protect _XOPEN_SOURCE

 Revision 1.1.2.1  2009-06-21 11:20:55  brian
 - added files to new distro

 *****************************************************************************/

#ifndef __SUA_SEND_H__
#define __SUA_SEND_H__

static inline mblk_t *
sua_get_data_msg(void)
{
	mblk_t mp;
	N_data_ind_t *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		p = (N_data_ind_t *) mp->b_wptr;
		mp->b_datap->db_type = M_PROTO;
		p->PRIM_type = N_DATA_REQ;
		p->DATA_xfer_flags = N_RC_FLAG;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  CLDT - Connectionless Data Transfer
 *  -------------------------------------------------------------------------
 *  FIXME: I should convert the addresses from the primitive.
 */
static inline int
send_cldt(q, prim, src_ptr, src_len, dst_ptr, dst_len)
	queue_t *q;
	mblk_t *prim;
	caddr_t src_ptr;
	size_t src_len;
	caddr_t dst_ptr;
	size_t dst_len;
{
	int err;
	mblk_t *mp, *tp, dp = prim->b_cont;
	sccp_t *sp = ((pp_t *) q->q_ptr)->u.sccp;
	size_t mlen =
	    SUA_MHDR_SIZE +
	    SUA_SIZE(SUA_PARM_NTWK_APP) +
	    SUA_SIZE(SUA_PARM_FLAGS) +
	    SUA_SIZE(SUA_PARM_SRCE_ADDR) +
	    SUA_SIZE(SUA_PARM_DEST_ADDR) + PAD4(dst_len) + dp ? SUA_SIZE(SUA_PARM_DATA) : 0;
	size_t dlen = msgdsize(dp);

	if (!sccp_as_canput(q))
		return (-EBUSY);
	if ((tp = sua_get_data_msg())) {
		if ((mp = allocb(mlen, BPRI_MED))) {
			mp->b_datap->db_type = M_DATA;
			*((uint32_t *) mp->b_wptr)++ = SUA_CNLS_CLDT;
			*((uint32_t *) mp->b_wptr)++ = htonl(mlen + dlen);
			*((uint32_t *) mp->b_wptr)++ = SUA_PARM_NTWK_APP;
			*((uint32_t *) mp->b_wptr)++ = __htonl(sp->na);
			*((uint32_t *) mp->b_wptr)++ = SUA_PARM_FLAGS;
			*((uint32_t *) mp->b_wptr)++ = __htonl(sp->flags);
			*((uint32_t *) mp->b_wptr)++ = SUA_PHDR(SUA_PARM_SRCE_ADDR, PAD4(src_len));
			bcopy(src_ptr, mp->b_wptr, src_len);
			mp->b_wptr += PAD4(src_len);
			*((uint32_t *) mp->b_wptr)++ = SUA_PHDR(SUA_PARM_DEST_ADDR < PAD4(src_len));
			bcopy(src_ptr, mp->b_wptr, src_len);
			mp->b_wptr += PAD4(src_len);
			*((uint32_t *) mp->b_wptr)++ = SUA_PARM_DATA;
			mp->b_cont = dp;
			tp->b_cont = mp;
			sccp_as_putq(q, tp);
			freeb(prim);
			return (0);
		}
		freeb(tp);
	}
	return (-ENOBUFS);
}

/*
 *  CLDR - Connectionless Data Response
 *  -------------------------------------------------------------------------
 *  FIXME: I should convert the addresses from the primitive.  The
 *  N_UDERROR_IND does not contain the source address.  The source address is
 *  the address to which the stream is bound in PC+SSN format.  The ERROR_type
 *  needs to be converted to the SCCP_Cause.
 */
static inline int
send_cldr(queue_t *q, mblk_t *prim)
{
	int err;
	mblk_t *mp, *tp, *dp = prim->b_cont;
	sccp_t *sp = ((pp_t *) q->q_ptr)->u.sccp;
	N_uderror_ind_t *p = (N_uderror_ind_t *) prim->b_rptr;
	size_t mlen =
	    SUA_MHDR_SIZE +
	    SUA_SIZE(SUA_PARM_NTWK_APP) +
	    SUA_SIZE(SUA_PARM_FLAGS) +
	    SUA_SIZE(SUA_PARM_SRCE_ADDR) +
	    SUA_SIZE(SUA_PARM_DEST_ADDR) + PAD4(dst_len) + dp ? SUA_SIZE(SUA_PARM_DATA) : 0;
	size_t dlen = msgdsize(dp);

	if (!sccp_as_canput(q))
		return (-EBUSY);
	if ((tp = sua_get_data())) {
		if ((mp = allocb(mlen, BPRI_MED))) {
			mp->b_datap->db_type = M_DATA;
			*((uint32_t *) mp->b_wptr)++ = SUA_CNLS_CLDR;
			*((uint32_t *) mp->b_wptr)++ = htonl(mlen + dlen);
			*((uint32_t *) mp->b_wptr)++ = SUA_PARM_NTWK_APP;
			*((uint32_t *) mp->b_wptr)++ = __htonl(sp->na);
			*((uint32_t *) mp->b_wptr)++ = SUA_PARM_FLAGS;
			*((uint32_t *) mp->b_wptr)++ = __htonl(sp->flags);
			*((uint32_t *) mp->b_wptr)++ = SUA_PARM_CAUSE;
			*((uint32_t *) mp->b_wptr)++ = __htonl(p->ERROR_type & 0x0000ffff);
			/* we always use our bound address as source address */
			*((uint32_t *) mp->b_wptr)++ = SUA_PARM_SRCE_ADDR;
			*((uint16_t *) mp->b_wptr)++ = __constant_htons(SUA_RI_PC_SSN);
			*((uint16_t *) mp->b_wptr)++ = __constant_htons(SUA_AI_PC | SUA_AI_SSN);
			*((uint32_t *) mp->b_wptr)++ = SUA_SPARM_SSN;
			*((uint32_t *) mp->b_wptr)++ = htonl(sp->ssn);
			*((uint32_t *) mp->b_wptr)++ = SUA_SPARM_PC;
			*((uint32_t *) mp->b_wptr)++ = htonl(sp->pc);
			*((uint32_t *) mp->b_wptr)++ = SUA_PHDR(SUA_PARM_DEST_ADDR < PAD4(dst_len));
			/* FIXME: pull dest address out of primtive */
			if (dp) {
				*((uint32_t *) mp->b_wptr)++ = SUA_PARM_DATA;
				mp->b_cont = dp;
			}
			tp->b_cont = mp;
			sccp_as_putq(q, tp);
			freeb(prim);
			return (0);
		}
		freeb(tp);
	}
	return (-ENOBUFS);
}

/*
 *  CORE - Connection Request
 *  -------------------------------------------------------------------------
 */
static inline int
send_core(queue_t *q, mblk_t *prim)
{
	int err;
	mblk_t *mp, *tp, *dp = prim->b_cont;
	sccp_t *sp = ((pp_t *) q->q_ptr)->u.sccp;
	N_conn_ind_t *p = (N_conn_ind_t *) prim->b_rptr;
	size_t mlen =
	    SUA_MHDR_SIZE +
	    SUA_SIZE(SUA_PARM_NTWK_APP) +
	    SUA_SIZE(SUA_PARM_FLAGS) +
	    SUA_SIZE(SUA_PARM_SRN) +
	    SUA_SIZE(SUA_PARM_SRCE_ADDR) +
	    SUA_SIZE(SUA_PARM_DEST_ADDR) + PAD4(dst_len) + dp ? SUA_SIZE(SUA_PARM_DATA) : 0;
	size_t dlen = msgdsize(dp);

}

/*
 *  COAK - Connection Acknowledgement 
 *  -------------------------------------------------------------------------
 */
/*
 *  COREF - Connection Refused
 *  -------------------------------------------------------------------------
 */
/*
 *  RELRE - Release Request
 *  -------------------------------------------------------------------------
 */
/*
 *  RELCO - Release Complete
 *  -------------------------------------------------------------------------
 */
/*
 *  RESCO - Reset Confirm
 *  -------------------------------------------------------------------------
 */
/*
 *  RESRE - Reset Request
 *  -------------------------------------------------------------------------
 */
/*
 *  CODT - Connection Oriented Data Transfer
 *  -------------------------------------------------------------------------
 */
/*
 *  CODA - Connection Oriented Data Ackowledgement
 *  -------------------------------------------------------------------------
 */
/*
 *  COERR - Connection Oriented Error
 *  -------------------------------------------------------------------------
 */
/*
 *  COIT - Inactivity Test
 *  -------------------------------------------------------------------------
 */
/*
 *  DUNA - Destination Unavailable
 *  -------------------------------------------------------------------------
 */
/*
 *  DAVA - Destination Available
 *  -------------------------------------------------------------------------
 */
/*
 *  DAUD - Destination Statue Audit
 *  -------------------------------------------------------------------------
 */
/*
 *  SCON - SS7 Network Congestion
 *  -------------------------------------------------------------------------
 */

#endif				/* __SUA_SEND_H__ */
