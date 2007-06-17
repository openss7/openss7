/*****************************************************************************

 @(#) $Id$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 2 of the License.

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

#ifndef __M3UA_ASP_H__
#define __M3UA_ASP_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

#include "../ua/ua_asp.h"	/* UA --> UA Common Messages */
#include "m3ua_msg.h"		/* M3UA specific messages */

/*
 *  =========================================================================
 *
 *  M3UA Peer Messages
 *
 *  =========================================================================
 *  These inline functions build M3UA peer messages.
 */

/*
 *  UA_SNMM_DUNA
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *m3ua_duna(uint32_t na, uint32_t * apcs, size_t num_apcs,
				caddr_t inf_ptr, size_t inf_len)
{
	mblk_t *mp, *np;
	size_t mlen = UA_MHDR_SIZE +
	    na ? UA_SIZE(M3UA_PARM_NTWK_APP) : 0 +
	    num_apcs ? UA_SIZE(M2UA_AFFECT_DEST) + num_apcs * sizeof(uint32_t) : 0 +
	    inf_len ? UA_PHDR_SIZE + UA_PAD4(inf_len) : 0;
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		*((uint32_t *) mp->b_wptr)++ = UA_SNMM_DUNA;
		*((uint32_t *) mp->b_wptr)++ = htonl(mlen);
		if (na) {
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_NTWK_APP;
			*((uint32_t *) mp->b_wptr)++ = htonl(na);
		}
		if (num_apcs)
			*((uint32_t *) mp->b_wptr)++ =
			    UA_PHDR(M3UA_PARM_AFFECT_DEST, num_apcs * sizeof(uint32_t));
		while (num_apcs--)
			*((uint32_t *) mp->b_wptr)++ = htonl(*apcs++);
		if (inf_len) {
			*((uint32_t *) mp->b_wptr)++ = UA_PHDR(UA_PARM_INFO, inf_len);
			bcopy(inf_ptr, mp->b_wptr, inf_len);
			mp->b_wptr += UA_PAD4(inf_len);
		}
		if ((np = n_exdata_req(0, N_QOS_PPI_M3UA, 0, mp)))
			return (np);
		freeb(mp);
	}
	return (NULL);
}

/*
 *  UA_SNMM_DAVA
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *m3ua_dava(uint32_t na, uint32_t * apcs, size_t num_apcs,
				caddr_t inf_ptr, size_t inf_len)
{
	mblk_t *mp, *np;
	size_t mlen = UA_MHDR_SIZE +
	    na ? UA_SIZE(M3UA_PARM_NTWK_APP) : 0 +
	    num_apcs ? UA_SIZE(M2UA_AFFECT_DEST) + num_apcs * sizeof(uint32_t) : 0 +
	    inf_len ? UA_PHDR_SIZE + UA_PAD4(inf_len) : 0;
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		*((uint32_t *) mp->b_wptr)++ = UA_SNMM_DAVA;
		*((uint32_t *) mp->b_wptr)++ = htonl(mlen);
		if (na) {
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_NTWK_APP;
			*((uint32_t *) mp->b_wptr)++ = htonl(na);
		}
		if (num_apcs)
			*((uint32_t *) mp->b_wptr)++ =
			    UA_PHDR(M3UA_PARM_AFFECT_DEST, num_apcs * sizeof(uint32_t));
		while (num_apcs--)
			*((uint32_t *) mp->b_wptr)++ = htonl(*apcs++);
		if (inf_len) {
			*((uint32_t *) mp->b_wptr)++ = UA_PHDR(UA_PARM_INFO, inf_len);
			bcopy(inf_ptr, mp->b_wptr, inf_len);
			mp->b_wptr += UA_PAD4(inf_len);
		}
		if ((np = n_exdata_req(0, N_QOS_PPI_M3UA, 0, mp)))
			return (np);
		freeb(mp);
	}
	return (NULL);
}

/*
 *  UA_SNMM_DAUD
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *m3ua_daud(uint32_t na, uint32_t * apcs, size_t num_apcs,
				caddr_t inf_ptr, size_t inf_len)
{
	mblk_t *mp, *np;
	size_t mlen = UA_MHDR_SIZE +
	    na ? UA_SIZE(M3UA_PARM_NTWK_APP) : 0 +
	    num_apcs ? UA_SIZE(M2UA_AFFECT_DEST) + num_apcs * sizeof(uint32_t) : 0 +
	    inf_len ? UA_PHDR_SIZE + UA_PAD4(inf_len) : 0;
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		*((uint32_t *) mp->b_wptr)++ = UA_SNMM_DAUD;
		*((uint32_t *) mp->b_wptr)++ = htonl(mlen);
		if (na) {
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_NTWK_APP;
			*((uint32_t *) mp->b_wptr)++ = htonl(na);
		}
		if (num_apcs)
			*((uint32_t *) mp->b_wptr)++ =
			    UA_PHDR(M3UA_PARM_AFFECT_DEST, num_apcs * sizeof(uint32_t));
		while (num_apcs--)
			*((uint32_t *) mp->b_wptr)++ = htonl(*apcs++);
		if (inf_len) {
			*((uint32_t *) mp->b_wptr)++ = UA_PHDR(UA_PARM_INFO, inf_len);
			bcopy(inf_ptr, mp->b_wptr, inf_len);
			mp->b_wptr += UA_PAD4(inf_len);
		}
		if ((np = n_exdata_req(0, N_QOS_PPI_M3UA, 0, mp)))
			return (np);
		freeb(mp);
	}
	return (NULL);
}

/*
 *  UA_SNMM_SCON
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *m3ua_scon(uint32_t na, uint32_t * apcs, size_t num_apcs,
				uint32_t cpc, uint8_t cong_level, caddr_t inf_ptr, size_t inf_len)
{
	mblk_t *mp, *np;
	size_t mlen = UA_MHDR_SIZE +
	    na ? UA_SIZE(M3UA_PARM_NTWK_APP) : 0 +
	    num_apcs ? UA_PHDR_SIZE + num_apcs * sizeof(uint32_t) : 0 +
	    cpc ? UA_SIZE(M3UA_PARM_CONCERN_DEST) : 0 +
	    UA_SIZE(M3UA_PARM_CONG_IND) + inf_len ? UA_PHDR_SIZE + UA_PAD4(inf_len) : 0;
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		*((uint32_t *) mp->b_wptr)++ = UA_SNMM_SCON;
		*((uint32_t *) mp->b_wptr)++ = htonl(mlen);
		if (na) {
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_NTWK_APP;
			*((uint32_t *) mp->b_wptr)++ = htonl(na);
		}
		if (num_apcs)
			*((uint32_t *) mp->b_wptr)++ =
			    UA_PHDR(M3UA_PARM_AFFECT_DEST, num_apcs * sizeof(uint32_t));
		while (num_apcs--)
			*((uint32_t *) mp->b_wptr)++ = htonl(*apcs++);
		if (cpc) {
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_CONCERN_DEST;
			*((uint32_t *) mp->b_wptr)++ = htonl(cpc);
		}
		*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_CONG_IND;
		*((uint32_t *) mp->b_wptr)++ = htonl(cong_level);
		if (inf_len) {
			*((uint32_t *) mp->b_wptr)++ = UA_PHDR(UA_PARM_INFO, inf_len);
			bcopy(inf_ptr, mp->b_wptr, inf_len);
			mp->b_wptr += UA_PAD4(inf_len);
		}
		if ((np = n_exdata_req(0, N_QOS_PPI_M3UA, 0, mp)))
			return (np);
		freeb(mp);
	}
	return (NULL);
}

/*
 *  UA_SNMM_DUPU
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *m3ua_dupu(uint32_t na, uint32_t apc, uint16_t user,
				uint16_t cause, caddr_t inf_ptr, size_t inf_len)
{
	mblk_t *mp, *np;
	size_t mlen = UA_MHDR_SIZE +
	    na ? UA_SIZE(M3UA_PARM_NTWK_APP) : 0 +
	    UA_SIZE(M3UA_PARM_AFFECT_DEST) +
	    UA_SIZE(M3UA_PARM_USER_CAUSE) + inf_len ? UA_PHDR_SIZE + UA_PAD4(inf_len) : 0;
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		*((uint32_t *) mp->b_wptr)++ = UA_SNMM_DRST;
		*((uint32_t *) mp->b_wptr)++ = htonl(mlen);
		if (na) {
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_NTWK_APP;
			*((uint32_t *) mp->b_wptr)++ = htonl(na);
		}
		*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_AFFECT_DEST;
		*((uint32_t *) mp->b_wptr)++ = htonl(apc);
		*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_USER_CAUSE;
		*((uint16_t *) mp->b_wptr)++ = htons(cause);
		*((uint16_t *) mp->b_wptr)++ = htons(user);
		if (inf_len) {
			*((uint32_t *) mp->b_wptr)++ = UA_PHDR(UA_PARM_INFO, inf_len);
			bcopy(inf_ptr, mp->b_wptr, inf_len);
			mp->b_wptr += UA_PAD4(inf_len);
		}
		if ((np = n_exdata_req(0, N_QOS_PPI_M3UA, 0, mp)))
			return (np);
		freeb(mp);
	}
	return (NULL);
}

/*
 *  UA_SNMM_DRST
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *m3ua_drst(uint32_t na, uint32_t * apcs, size_t num_apcs,
				caddr_t inf_ptr, size_t inf_len)
{
	mblk_t *mp, *np;
	size_t mlen = UA_MHDR_SIZE +
	    na ? UA_SIZE(M3UA_PARM_NTWK_APP) : 0 +
	    num_apcs ? UA_SIZE(M2UA_AFFECT_DEST) + num_apcs * sizeof(uint32_t) : 0 +
	    inf_len ? UA_PHDR_SIZE + UA_PAD4(inf_len) : 0;
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		*((uint32_t *) mp->b_wptr)++ = UA_SNMM_DRST;
		*((uint32_t *) mp->b_wptr)++ = htonl(mlen);
		if (na) {
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_NTWK_APP;
			*((uint32_t *) mp->b_wptr)++ = htonl(na);
		}
		if (num_apcs)
			*((uint32_t *) mp->b_wptr)++ =
			    UA_PHDR(M3UA_PARM_AFFECT_DEST, num_apcs * sizeof(uint32_t));
		while (num_apcs--)
			*((uint32_t *) mp->b_wptr)++ = htonl(*apcs++);
		if (inf_len) {
			*((uint32_t *) mp->b_wptr)++ = UA_PHDR(UA_PARM_INFO, inf_len);
			bcopy(inf_ptr, mp->b_wptr, inf_len);
			mp->b_wptr += UA_PAD4(inf_len);
		}
		if ((np = n_exdata_req(0, N_QOS_PPI_M3UA, 0, mp)))
			return (np);
		freeb(mp);
	}
	return (NULL);
}

/*
 *  M3UA_XFER_DATA
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *m3ua_data(uint32_t na, uint32_t rc, uint32_t opc,
				uint32_t dpc, uint8_t si, uint8_t mp, uint16_t sls, mblk_t * dp)
{
	mblk_t *mp, *np;
	size_t dlen = UA_PDHR_SIZE + 3 * sizeof(uint32_t) + msgdsize(dp);
	size_t mlen = UA_MHDR_SIZE +
	    na ? UA_SIZE(M3UA_PARM_NTWK_APP) : 0 + rc ? UA_SIZE(M3UA_PARM_RC) : 0 + UA_PAD4(dlen);
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		*((uint32_t *) mp->b_wptr)++ = M3UA_XFER_DATA;
		*((uint32_t *) mp->b_wptr)++ = htonl(mlen);
		if (na) {
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_NTWK_APP;
			*((uint32_t *) mp->b_wptr)++ = htonl(na);
		}
		if (rc) {
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_RC;
			*((uint32_t *) mp->b_wptr)++ = htonl(na);
		}
		*((uint32_t *) mp->b_wptr)++ = UA_PHDR(M3UA_PROT_DATA3, dlen);
		*((uint32_t *) mp->b_wptr)++ = htonl(opc);
		*((uint32_t *) mp->b_wptr)++ = htonl(dpc);
		*((uint8_t *) mp->b_wptr)++ = si;
		*((uint8_t *) mp->b_wptr)++ = mp;
		*((uint16_t *) mp->b_wptr)++ = htons(sls);
		mp->b_cont = dp;
		dp->b_wptr = UA_PAD4(dp->b_wptr);
		if ((np = n_data_req(N_RC_FLAG, N_QOS_PPI_M3UA, (rc << 8) | sls, mp)))
			return (np);
		freeb(mp);
	}
	return (NULL);
}

#endif				/* __M3UA_ASP_H__ */
