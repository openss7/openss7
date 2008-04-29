/*****************************************************************************

 @(#) $RCSfile: sua_asp.h,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2008-04-29 01:52:21 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

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

 Last Modified $Date: 2008-04-29 01:52:21 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sua_asp.h,v $
 Revision 0.9.2.4  2008-04-29 01:52:21  brian
 - updated headers for release

 Revision 0.9.2.3  2007/08/14 08:33:54  brian
 - GPLv3 header update

 Revision 0.9.2.2  2007/06/17 02:00:50  brian
 - updates for release, remove any later language

 *****************************************************************************/

#ifndef __SUA_ASP_H__
#define __SUA_ASP_H__

#ident "@(#) $RCSfile: sua_asp.h,v $ $Name:  $($Revision: 0.9.2.4 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

#include "../ua/ua_asp.h"	/* UA --> UA Common Messages */
#include "sua_msg.h"		/* SUA specific messages */

/*
 *  =========================================================================
 *
 *  SUA Peer Messages
 *
 *  =========================================================================
 *  These inline functions build SUA peer messages.
 */
/*
 *  UA_SNMM_DUNA
 *  ------------------------------------------------------------------------
 *  There are two versions here: N-STATE and N-PCSTATE depending on whether
 *  SSN is supplied or not.
 */
static inline mblk_t *
sua_duna(uint *rcs, size_t rc_num, uint pc, uint *ssn, uint smi, caddr_t inf_ptr, size_t inf_len)
{
	mblk_t *mp, *np;
	size_t rlen = rc_num ? UA_PHDR_SIZE + (rc_num << 2) : 0;
	size_t dlen = UA_PHDR_SIZE + sizeof(uint32_t) + UA_SIZE(SUA_SPARM_PC)
	    + ssn ? UA_SIZE(SUA_SPARM_SSN) : 0;
	size_t ilen = inf_ptr ? UA_PHDR_SIZE + inf_len : 0;
	size_t plen = UA_PAD4(ilen) - ilen;
	size_t mlen = UA_MHDR_SIZE + rlen + dlen + UA_PAD4(ilen);

	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		*((uint32_t *) mp->b_wptr)++ = UA_SNMM_DUNA;
		*((uint32_t *) mp->b_wptr)++ = htonl(mlen);
		*((uint32_t *) mp->b_wptr)++ = UA_PHDR(UA_PARM_RC, rlen);
		while (rc_num--)
			*((uint32_t *) mp->b_wptr)++ = htonl(*rcs++);
		*((uint32_t *) mp->b_wptr)++ = UA_PHDR(SUA_PARM_DEST_ADDR, dlen);
		*((uint16_t *) mp->b_wptr)++ = __constant_htons(SUA_RI_PC_SSN);
		*((uint16_t *) mp->b_wptr)++ = htons(SUA_AI_PC | (ssn ? SUA_AI_SSN : 0));
		*((uint32_t *) mp->b_wptr)++ = SUA_SPARM_PC;
		*((uint32_t *) mp->b_wptr)++ = htonl(pc);
		if (ssn) {
			*((uint32_t *) mp->b_wptr)++ = SUA_SPARM_SSN;
			*((uint32_t *) mp->b_wptr)++ = htonl((smi << 16) | *ssn);
		}
		if (inf_ptr) {
			*((uint32_t *) mp->b_wptr)++ = UA_PHDR(UA_PARM_INFO, ilen);
			bcopy(inf_ptr, mp->b_wptr, inf_len);
			mp->b_wptr += inf_len;
			bzero(mp->b_wptr, plen);
			mp->b_wptr += plen;
		}
		if ((np = n_exdata_req(0, N_QOS_PPI_SUA, 0, mp)))
			return (np);
		freeb(mp);
	}
	return (NULL);
}

/*
 *  UA_SNMM_DAVA
 *  ------------------------------------------------------------------------
 *  There are two versions here: N-STATE and N-PCSTATE depending on whether
 *  SSN is supplied or not.
 */
static inline mblk_t *
sua_dava(uint *rcs, size_t rc_num, uint pc, uint *ssn, uint smi, caddr_t inf_ptr, size_t inf_len)
{
	mblk_t *mp, *np;
	size_t rlen = rc_num ? UA_PHDR_SIZE + (rc_num << 2) : 0;
	size_t dlen = UA_PHDR_SIZE + sizeof(uint32_t) + UA_SIZE(SUA_SPARM_PC)
	    + ssn ? UA_SIZE(SUA_SPARM_SSN) : 0;
	size_t ilen = inf_ptr ? UA_PHDR_SIZE + inf_len : 0;
	size_t plen = UA_PAD4(ilen) - ilen;
	size_t mlen = UA_MHDR_SIZE + rlen + dlen + UA_PAD4(ilen);

	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		*((uint32_t *) mp->b_wptr)++ = UA_SNMM_DAVA;
		*((uint32_t *) mp->b_wptr)++ = htonl(mlen);
		*((uint32_t *) mp->b_wptr)++ = UA_PHDR(UA_PARM_RC, rlen);
		while (rc_num--)
			*((uint32_t *) mp->b_wptr)++ = htonl(*rcs++);
		*((uint32_t *) mp->b_wptr)++ = UA_PHDR(SUA_PARM_DEST_ADDR, dlen);
		*((uint16_t *) mp->b_wptr)++ = __constant_htons(SUA_RI_PC_SSN);
		*((uint16_t *) mp->b_wptr)++ = htons(SUA_AI_PC | (ssn ? SUA_AI_SSN : 0));
		*((uint32_t *) mp->b_wptr)++ = SUA_SPARM_PC;
		*((uint32_t *) mp->b_wptr)++ = htonl(pc);
		if (ssn) {
			*((uint32_t *) mp->b_wptr)++ = SUA_SPARM_SSN;
			*((uint32_t *) mp->b_wptr)++ = htonl((smi << 16) | *ssn);
		}
		if (inf_ptr) {
			*((uint32_t *) mp->b_wptr)++ = UA_PHDR(UA_PARM_INFO, ilen);
			bcopy(inf_ptr, mp->b_wptr, inf_len);
			mp->b_wptr += inf_len;
			bzero(mp->b_wptr, plen);
			mp->b_wptr += plen;
		}
		if ((np = n_exdata_req(0, N_QOS_PPI_SUA, 0, mp)))
			return (np);
		freeb(mp);
	}
	return (NULL);
}

/*
 *  UA_SNMM_DAUD
 *  ------------------------------------------------------------------------
 *  There are two versions here: N-STATE and N-PCSTATE depending on whether
 *  SSN is supplied or not.
 */
static inline mblk_t *
sua_daud(uint *rcs, size_t rc_num, uint pc, uint *ssn, uint smi, caddr_t inf_ptr, size_t inf_len)
{
	mblk_t *mp, *np;
	size_t rlen = rc_num ? UA_PHDR_SIZE + (rc_num << 2) : 0;
	size_t dlen = UA_PHDR_SIZE + sizeof(uint32_t) + UA_SIZE(SUA_SPARM_PC)
	    + ssn ? UA_SIZE(SUA_SPARM_SSN) : 0;
	size_t ilen = inf_ptr ? UA_PHDR_SIZE + inf_len : 0;
	size_t plen = UA_PAD4(ilen) - ilen;
	size_t mlen = UA_MHDR_SIZE + rlen + dlen + UA_PAD4(ilen);

	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		*((uint32_t *) mp->b_wptr)++ = UA_SNMM_DAUD;
		*((uint32_t *) mp->b_wptr)++ = htonl(mlen);
		*((uint32_t *) mp->b_wptr)++ = UA_PHDR(UA_PARM_RC, rlen);
		while (rc_num--)
			*((uint32_t *) mp->b_wptr)++ = htonl(*rcs++);
		*((uint32_t *) mp->b_wptr)++ = UA_PHDR(SUA_PARM_DEST_ADDR, dlen);
		*((uint16_t *) mp->b_wptr)++ = __constant_htons(SUA_RI_PC_SSN);
		*((uint16_t *) mp->b_wptr)++ = htons(SUA_AI_PC | (ssn ? SUA_AI_SSN : 0));
		*((uint32_t *) mp->b_wptr)++ = SUA_SPARM_PC;
		*((uint32_t *) mp->b_wptr)++ = htonl(pc);
		if (ssn) {
			*((uint32_t *) mp->b_wptr)++ = SUA_SPARM_SSN;
			*((uint32_t *) mp->b_wptr)++ = htonl((smi << 16) | *ssn);
		}
		if (inf_ptr) {
			*((uint32_t *) mp->b_wptr)++ = UA_PHDR(UA_PARM_INFO, ilen);
			bcopy(inf_ptr, mp->b_wptr, inf_len);
			mp->b_wptr += inf_len;
			bzero(mp->b_wptr, plen);
			mp->b_wptr += plen;
		}
		if ((np = n_exdata_req(0, N_QOS_PPI_SUA, 0, mp)))
			return (np);
		freeb(mp);
	}
	return (NULL);
}

/*
 *  UA_SNMM_SCON
 *  ------------------------------------------------------------------------
 *  There are two versions here: N-STATE and N-PCSTATE depending on whether
 *  SSN is supplied or not.
 */
static inline mblk_t *
sua_scon(uint *rcs, size_t rc_num, uint pc, uint *ssn,
	 uint smi, uint cong, caddr_t inf_ptr, size_t inf_len)
{
	mblk_t *mp, *np;
	size_t rlen = rc_num ? UA_PHDR_SIZE + (rc_num << 2) : 0;
	size_t dlen = UA_PHDR_SIZE + sizeof(uint32_t) + UA_SIZE(SUA_SPARM_PC)
	    + ssn ? UA_SIZE(SUA_SPARM_SSN) : 0;
	size_t ilen = inf_ptr ? UA_PHDR_SIZE + inf_len : 0;
	size_t plen = UA_PAD4(ilen) - ilen;
	size_t mlen = UA_MHDR_SIZE + rlen + dlen + UA_SIZE(UA_PARM_CONG_LEVEL)
	    + UA_PAD4(ilen);

	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		*((uint32_t *) mp->b_wptr)++ = UA_SNMM_SCON;
		*((uint32_t *) mp->b_wptr)++ = htonl(mlen);
		*((uint32_t *) mp->b_wptr)++ = UA_PHDR(UA_PARM_RC, rlen);
		while (rc_num--)
			*((uint32_t *) mp->b_wptr)++ = htonl(*rcs++);
		*((uint32_t *) mp->b_wptr)++ = UA_PHDR(SUA_PARM_DEST_ADDR, dlen);
		*((uint16_t *) mp->b_wptr)++ = __constant_htons(SUA_RI_PC_SSN);
		*((uint16_t *) mp->b_wptr)++ = htons(SUA_AI_PC | (ssn ? SUA_AI_SSN : 0));
		*((uint32_t *) mp->b_wptr)++ = SUA_SPARM_PC;
		*((uint32_t *) mp->b_wptr)++ = htonl(pc);
		if (ssn) {
			*((uint32_t *) mp->b_wptr)++ = SUA_SPARM_SSN;
			*((uint32_t *) mp->b_wptr)++ = htonl((smi << 16) | *ssn);
		}
		*((uint32_t *) mp->b_wptr)++ = UA_PARM_CONG_LEVEL;
		*((uint32_t *) mp->b_wptr)++ = htonl(cong);
		if (inf_ptr) {
			*((uint32_t *) mp->b_wptr)++ = UA_PHDR(UA_PARM_INFO, ilen);
			bcopy(inf_ptr, mp->b_wptr, inf_len);
			mp->b_wptr += inf_len;
			bzero(mp->b_wptr, plen);
			mp->b_wptr += plen;
		}
		if ((np = n_exdata_req(0, N_QOS_PPI_SUA, 0, mp)))
			return (np);
		freeb(mp);
	}
	return (NULL);
}

/*
 *  UA_SNMM_DRST
 *  ------------------------------------------------------------------------
 *  There are two versions here: N-COORD and N-PCSTATE depending on whether
 *  SSN is supplied or not.
 *
 *  FIXME: straighten out N-COORD.
 */
static inline mblk_t *
sua_drst(uint *rcs, size_t rc_num, uint pc, uint *ssn, uint smi, caddr_t inf_ptr, size_t inf_len)
{
	mblk_t *mp, *np;
	size_t rlen = rc_num ? UA_PHDR_SIZE + (rc_num << 2) : 0;
	size_t dlen = UA_PHDR_SIZE + sizeof(uint32_t) + UA_SIZE(SUA_SPARM_PC)
	    + ssn ? UA_SIZE(SUA_SPARM_SSN) : 0;
	size_t ilen = inf_ptr ? UA_PHDR_SIZE + inf_len : 0;
	size_t plen = UA_PAD4(ilen) - ilen;
	size_t mlen = UA_MHDR_SIZE + rlen + dlen + UA_PAD4(ilen);

	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		*((uint32_t *) mp->b_wptr)++ = UA_SNMM_DRST;
		*((uint32_t *) mp->b_wptr)++ = htonl(mlen);
		*((uint32_t *) mp->b_wptr)++ = UA_PHDR(UA_PARM_RC, rlen);
		while (rc_num--)
			*((uint32_t *) mp->b_wptr)++ = htonl(*rcs++);
		*((uint32_t *) mp->b_wptr)++ = UA_PHDR(SUA_PARM_DEST_ADDR, dlen);
		*((uint16_t *) mp->b_wptr)++ = __constant_htons(SUA_RI_PC_SSN);
		*((uint16_t *) mp->b_wptr)++ = htons(SUA_AI_PC | (ssn ? SUA_AI_SSN : 0));
		*((uint32_t *) mp->b_wptr)++ = SUA_SPARM_PC;
		*((uint32_t *) mp->b_wptr)++ = htonl(pc);
		if (ssn) {
			*((uint32_t *) mp->b_wptr)++ = SUA_SPARM_SSN;
			*((uint32_t *) mp->b_wptr)++ = htonl((smi << 16) | *ssn);
		}
		if (inf_ptr) {
			*((uint32_t *) mp->b_wptr)++ = UA_PHDR(UA_PARM_INFO, ilen);
			bcopy(inf_ptr, mp->b_wptr, inf_len);
			mp->b_wptr += inf_len;
			bzero(mp->b_wptr, plen);
			mp->b_wptr += plen;
		}
		if ((np = n_exdata_req(0, N_QOS_PPI_SUA, 0, mp)))
			return (np);
		freeb(mp);
	}
	return (NULL);
}

/*
 *  UA_RKMM_REG_REQ
 *  ------------------------------------------------------------------------
 */
static inline mblk_t *
sua_reg_req(...)
{
}

/*
 *  UA_RKMM_REG_RSP
 *  ------------------------------------------------------------------------
 */
static inline mblk_t *
sua_reg_rsp(...)
{
}

/*
 *  UA_RKMM_DEREG_REQ
 *  ------------------------------------------------------------------------
 */
static inline mblk_t *
sua_dereg_req(...)
{
}

/*
 *  UA_RKMM_DEREG_RSP
 *  ------------------------------------------------------------------------
 */
static inline mblk_t *
sua_dereg_rsp(...)
{
}

/*
 *  SUA_CNLS_CLDT
 *  ------------------------------------------------------------------------
 *  TODO: Need to bread out flags for SUA-08 and add a segmentation parameter.
 */
static inline mblk_t *
sua_cldt(uint rc, uint flags, sccp_addr_t * src, sccp_addr_t * dst, mblk_t *dp)
{
	mblk_t *mp, *np;
	const size_t slen = sua_addr_len(src);
	const size_t dlen = sua_addr_len(dst);
	const size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_RC)
	    + UA_SIZE(SUA_PARM_FLAGS)
	    + slen ? UA_PHDR_SIZE + slen : 0 + dlen ? UA_PHDR_SIZE + dlen : 0 + UA_PHDR_SIZE;

	if ((mp = allocb(mlen, BPRI_MED))) {
		const size_t blen = msgdsize(dp);

		mp->b_datap->db_type = M_DATA;
		*((uint32_t *) mp->b_wptr)++ = SUA_CNLS_CLDT;
		*((uint32_t *) mp->b_wptr)++ = htonl(mlen + blen);
		*((uint32_t *) mp->b_wptr)++ = UA_PARM_RC;
		*((uint32_t *) mp->b_wptr)++ = htonl(rc);
		*((uint32_t *) mp->b_wptr)++ = SUA_PARM_FLAGS;
		*((uint32_t *) mp->b_wptr)++ = htonl(flags);
		*((uint32_t *) mp->b_wptr)++ = UA_PHDR(SUA_PARM_SRCE_ADDR, slen);
		sua_pack_addr(&mp->b_wptr, src);
		*((uint32_t *) mp->b_wptr)++ = UA_PHDR(SUA_PARM_DEST_ADDR, dlen);
		sua_pack_addr(&mp->b_wptr, dst);
		*((uint32_t *) mp->b_wptr)++ = UA_PHDR(UA_PARM_DATA, blen);
		*((uint32_t *) mp->b_wptr)++ = UA_PHDR(SUA_PARM_DATA, blen);
		mp->b_cont = dp;
		if ((np = n_data_req(N_RC_FLAG, N_QOS_PPI_SUA, (rc << 8) | sls, mp)))
			return (np);
		freeb(mp);
	}
	return (NULL);
}

/*
 *  SUA_CNLS_CLDR
 *  ------------------------------------------------------------------------
 *  TODO: Need to bread out flags for SUA-08 and add a segmentation parameter.
 */
static inline mblk_t *
sua_cldr(uint rc, uint flags, uint cause, sccp_addr_t * src, sccp_addr_t * dst, mblk_t *dp)
{
	mblk_t *mp, *np;
	const size_t slen = sua_addr_len(src);
	const size_t dlen = sua_addr_len(dst);
	const size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_RC)
	    + UA_SIZE(SUA_PARM_CAUSE)
	    + slen ? UA_PHDR_SIZE + slen : 0 + dlen ? UA_PHDR_SIZE + dlen : 0 +
	    dp ? UA_PHDR_SIZE : 0;
	if ((mp = allocb(mlen, BPRI_MED))) {
		const size_t blen = msgdsize(dp);

		mp->b_datap->db_type = M_DATA;
		*((uint32_t *) mp->b_wptr)++ = SUA_CLNS_CLDR;
		*((uint32_t *) mp->b_wptr)++ = htonl(mlen + blen);
		*((uint32_t *) mp->b_wptr)++ = UA_PARM_RC;
		*((uint32_t *) mp->b_wptr)++ = htonl(rc);
		*((uint32_t *) mp->b_wptr)++ = SUA_PARM_FLAGS;
		*((uint32_t *) mp->b_wptr)++ = htonl(flags);
		*((uint32_t *) mp->b_wptr)++ = UA_PHDR(SUA_PARM_SRCE_ADDR, slen);
		sua_pack_addr(&mp->b_wptr, src);
		*((uint32_t *) mp->b_wptr)++ = UA_PHDR(SUA_PARM_DEST_ADDR, dlen);
		sua_pack_addr(&mp->b_wptr, dst);
		*((uint32_t *) mp->b_wptr)++ = UA_PHDR(SUA_PARM_DATA, blen);
		if (dp)
			*((uint32_t *) mp->b_wptr)++ = UA_PHDR(SUA_PARM_DATA, blen);
		mp->b_cont = dp;
		if ((np = n_data_req(N_RC_FLAG, N_QOS_PPI_SUA, (rc << 8) | sls, mp)))
			return (np);
		freeb(mp);
	}
	return (NULL);
}

/*
 *  SUA_CONS_CORE
 *  ------------------------------------------------------------------------
 *  TODO: Need to bread out flags for SUA-08 and treat protocol class 3.
 */
static inline mblk_t *
sua_core(uint rc, uint flags, uint srn, sccp_addr_t * dst,
	 sccp_addr_t * src, uint *credit, mblk_t *dp)
{
	mblk_t *mp, *np;
	const size_t slen = src ? sua_addr_len(src) : 0;
	const size_t dlen = dst ? sua_addr_len(dst) : 0;
	const size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_RC)
	    + UA_SIZE(SUA_PARM_FLAGS)
	    + UA_SIZE(SUA_PARM_SRN)
	    + dlen ? UA_PHDR_SIZE + dlen : 0
	    + slen ? UA_PHDR_SIZE + slen : 0 + credit ? UA_SIZE(SUA_PARM_CREDIT) : 0 +
	    dp ? UA_PHDR_SIZE : 0;
	if ((mp = allocb(mlen, BPRI_MED))) {
		const size_t blen = dp ? msgdsize(dp) : 0;

		mp->b_datap->db_type = M_DATA;
		*((uint32_t *) mp->b_wptr)++ = SUA_CONS_CORE;
		*((uint32_t *) mp->b_wptr)++ = htonl(mlen + blen);
		*((uint32_t *) mp->b_wptr)++ = UA_PARM_RC;
		*((uint32_t *) mp->b_wptr)++ = htonl(rc);
		*((uint32_t *) mp->b_wptr)++ = SUA_PARM_FLAGS;
		*((uint32_t *) mp->b_wptr)++ = htonl(flags);
		*((uint32_t *) mp->b_wptr)++ = SUA_PARM_SRN;
		*((uint32_t *) mp->b_wptr)++ = htonl(srn);
		if (dst) {
			*((uint32_t *) mp->b_wptr)++ = UA_PHDR(SUA_PARM_DEST_ADDR, dlen);
			sua_pack_addr(&mp->b_wptr, dst);
		}
		if (src) {
			*((uint32_t *) mp->b_wptr)++ = UA_PHDR(SUA_PARM_SRCE_ADDR, slen);
			sua_pack_addr(&mp->b_wptr, src);
		}
		if (credit) {
			*((uint32_t *) mp->b_wptr)++ = SUA_PARM_CREDIT;
			*((uint32_t *) mp->b_wptr)++ = htonl(*credit);
		}
		if (dp)
			*((uint32_t *) mp->b_wptr)++ = UA_PHDR(SUA_PARM_DATA, blen);
		mp->b_cont = dp;
		if ((np = n_data_req(N_RC_FLAG, N_QOS_PPI_SUA, (rc << 8) | sls, mp)))
			return (np);
		freeb(mp);
	}
	return (NULL);
}

/*
 *  SUA_CONS_COAK
 *  ------------------------------------------------------------------------
 */
static inline mblk_t *
sua_coak(uint rc, uint flags, uint drn, uint srn, uint *credit, sccp_addr_t * dst, mblk_t *dp)
{
	mblk_t *mp, *np;
	const size_t dlen = dst ? sua_addr_len(dst) : 0;
	const size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_RC)
	    + UA_SIZE(SUA_PARM_FLAGS)
	    + UA_SIZE(SUA_PARM_DRN)
	    + UA_SIZE(SUA_PARM_SRN)
	    + UA_SIZE(SUA_PARM_CREDIT)
	    + dlen ? UA_PHDR_SIZE + dlen : 0 + dp ? UA_PHDR_SIZE : 0;

	if ((mp = allocb(mlen, BPRI_MED))) {
		const size_t blen = dp ? msgdsize(dp) : 0;

		mp->b_datap->db_type = M_DATA;
		*((uint32_t *) mp->b_wptr)++ = SUA_CONS_COAK;
		*((uint32_t *) mp->b_wptr)++ = htonl(mlen + blen);
		*((uint32_t *) mp->b_wptr)++ = UA_PARM_RC;
		*((uint32_t *) mp->b_wptr)++ = htonl(rc);
		*((uint32_t *) mp->b_wptr)++ = SUA_PARM_FLAGS;
		*((uint32_t *) mp->b_wptr)++ = htonl(flags);
		*((uint32_t *) mp->b_wptr)++ = SUA_PARM_DRN;
		*((uint32_t *) mp->b_wptr)++ = htonl(drn);
		*((uint32_t *) mp->b_wptr)++ = SUA_PARM_SRN;
		*((uint32_t *) mp->b_wptr)++ = htonl(srn);
		if (credit) {
			*((uint32_t *) mp->b_wptr)++ = SUA_PARM_CREDIT;
			*((uint32_t *) mp->b_wptr)++ = htonl(*credit);
		}
		if (dst) {
			*((uint32_t *) mp->b_wptr)++ = UA_PDHR(SUA_PARM_DEST_ADDR, dlen);
			sua_pack_addr(&mp->b_wptr, dst);
		}
		if (dp)
			*((uint32_t *) mp->b_wptr)++ = UA_PHDR(SUA_PARM_DATA, blen);
		mp->b_cont = dp;
		if ((np = n_data_req(N_RC_FLAG, N_QOS_PPI_SUA, (rc << 8) | sls, mp)))
			return (np);
		freeb(mp);
	}
	return (NULL);
}

/*
 *  SUA_CONS_COREF
 *  ------------------------------------------------------------------------
 */
static inline mblk_t *
sua_coref(uint rc, uint drn, uint cause, sccp_addr_t * dst, mblk_t *dp)
{
	mblk_t *mp, *np;
	const size_t dlen = dst ? sua_addr_len(dst) : 0;
	const size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_RC)
	    + UA_SIZE(SUA_PARM_DRN)
	    + UA_SIZE(SUA_PARM_CAUSE)
	    + dlen ? UA_PHDR_SIZE + dlen : 0 + dp ? UA_PDHR_SIZE : 0;

	if ((mp = allocb(mlen, BPRI_MED))) {
		const size_t blen = dp ? msgdsize(dp) : 0;

		mp->b_datap->db_type = M_DATA;
		*((uint32_t *) mp->b_wptr)++ = SUA_CONS_COREF;
		*((uint32_t *) mp->b_wptr)++ = htonl(mlen + blen);
		*((uint32_t *) mp->b_wptr)++ = UA_PARM_RC;
		*((uint32_t *) mp->b_wptr)++ = htonl(rc);
		*((uint32_t *) mp->b_wptr)++ = SUA_PARM_DRN;
		*((uint32_t *) mp->b_wptr)++ = htonl(drn);
		*((uint32_t *) mp->b_wptr)++ = SUA_PARM_CAUSE;
		*((uint32_t *) mp->b_wptr)++ = htonl(cause);
		if (dst) {
			*((uint32_t *) mp->b_wptr)++ = UA_PDHR(SUA_PARM_DEST_ADDR, dlen);
			sua_pack_addr(&mp->b_wptr, dst);
		}
		if (dp)
			*((uint32_t *) mp->b_wptr)++ = UA_PHDR(SUA_PARM_DATA, blen);
		mp->b_cont = dp;
		if ((np = n_data_req(N_RC_FLAG, N_QOS_PPI_SUA, (rc << 8) | sls, mp)))
			return (np);
		freeb(mp);
	}
	return (NULL);
}

/*
 *  SUA_CONS_RELRE
 *  ------------------------------------------------------------------------
 */
static inline mblk_t *
sua_relre(uint rc, uint drn, uint srn, uint cause, uint flags, mblk_t *dp)
{
	mblk_t *mp, *np;
	const size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_RC)
	    + UA_SIZE(SUA_PARM_DRN)
	    + UA_SIZE(SUA_PARM_SRN)
	    + UA_SIZE(SUA_PARM_CAUSE)
	    + UA_SIZE(SUA_PARM_FLAGS)
	    + dp ? UA_PHDR_SIZE : 0;

	if ((mp = allocb(mlen, BPRI_MED))) {
		const size_t blen = dp ? msgdsize(dp) : 0;

		mp->b_datap->db_type = M_DATA;
		*((uint32_t *) mp->b_wptr)++ = SUA_CONS_RELRE;
		*((uint32_t *) mp->b_wptr)++ = htonl(mlen + blen);
		*((uint32_t *) mp->b_wptr)++ = UA_PARM_RC;
		*((uint32_t *) mp->b_wptr)++ = htonl(rc);
		*((uint32_t *) mp->b_wptr)++ = SUA_PARM_DRN;
		*((uint32_t *) mp->b_wptr)++ = htonl(drn);
		*((uint32_t *) mp->b_wptr)++ = SUA_PARM_SRN;
		*((uint32_t *) mp->b_wptr)++ = htonl(srn);
		*((uint32_t *) mp->b_wptr)++ = SUA_PARM_CAUSE;
		*((uint32_t *) mp->b_wptr)++ = htonl(cause);
		*((uint32_t *) mp->b_wptr)++ = SUA_PARM_FLAGS;
		*((uint32_t *) mp->b_wptr)++ = htonl(flags);
		if (dp)
			*((uint32_t *) mp->b_wptr)++ = UA_PHDR(SUA_PARM_DATA, blen);
		mp->b_cont = dp;
		if ((np = n_data_req(N_RC_FLAG, N_QOS_PPI_SUA, (rc << 8) | sls, mp)))
			return (np);
		freeb(mp);
	}
	return (NULL);
}

/*
 *  SUA_CONS_RELCO
 *  ------------------------------------------------------------------------
 */
static inline mblk_t *
sua_relco(uint rc, uint drn, uint srn)
{
	mblk_t *mp, *np;
	static const size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_RC)
	    + UA_SIZE(SUA_PARM_DRN)
	    + UA_SIZE(SUA_PARM_SRN);

	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		*((uint32_t *) mp->b_wptr)++ = SUA_CONS_RELCO;
		*((uint32_t *) mp->b_wptr)++ = __constant_htonl(mlen);
		*((uint32_t *) mp->b_wptr)++ = UA_PARM_RC;
		*((uint32_t *) mp->b_wptr)++ = htonl(rc);
		*((uint32_t *) mp->b_wptr)++ = SUA_PARM_DRN;
		*((uint32_t *) mp->b_wptr)++ = htonl(drn);
		*((uint32_t *) mp->b_wptr)++ = SUA_PARM_SRN;
		*((uint32_t *) mp->b_wptr)++ = htonl(srn);
		if ((np = n_data_req(N_RC_FLAG, N_QOS_PPI_SUA, (rc << 8) | sls, mp)))
			return (np);
		freeb(mp);
	}
	return (NULL);
}

/*
 *  SUA_CONS_RESCO
 *  ------------------------------------------------------------------------
 */
static inline mblk_t *
sua_resco(uint rc, uint drn, uint srn)
{
	mblk_t *mp, *np;
	static const size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_RC)
	    + UA_SIZE(SUA_PARM_DRN)
	    + UA_SIZE(SUA_PARM_SRN);

	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		*((uint32_t *) mp->b_wptr)++ = SUA_CONS_RESCO;
		*((uint32_t *) mp->b_wptr)++ = __constant_htonl(mlen);
		*((uint32_t *) mp->b_wptr)++ = UA_PARM_RC;
		*((uint32_t *) mp->b_wptr)++ = htonl(rc);
		*((uint32_t *) mp->b_wptr)++ = SUA_PARM_DRN;
		*((uint32_t *) mp->b_wptr)++ = htonl(drn);
		*((uint32_t *) mp->b_wptr)++ = SUA_PARM_SRN;
		*((uint32_t *) mp->b_wptr)++ = htonl(srn);
		if ((np = n_data_req(N_RC_FLAG, N_QOS_PPI_SUA, (rc << 8) | sls, mp)))
			return (np);
		freeb(mp);
	}
	return (NULL);
}

/*
 *  SUA_CONS_RESRE
 *  ------------------------------------------------------------------------
 */
static inline mblk_t *
sua_relre(uint rc, uint drn, uint srn, uint cause)
{
	mblk_t *mp, *np;
	static const size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_RC)
	    + UA_SIZE(SUA_PARM_DRN)
	    + UA_SIZE(SUA_PARM_SRN)
	    + UA_SIZE(SUA_PARM_CAUSE);

	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		*((uint32_t *) mp->b_wptr)++ = SUA_CONS_RELRE;
		*((uint32_t *) mp->b_wptr)++ = __constant_htonl(mlen);
		*((uint32_t *) mp->b_wptr)++ = UA_PARM_RC;
		*((uint32_t *) mp->b_wptr)++ = htonl(rc);
		*((uint32_t *) mp->b_wptr)++ = SUA_PARM_DRN;
		*((uint32_t *) mp->b_wptr)++ = htonl(drn);
		*((uint32_t *) mp->b_wptr)++ = SUA_PARM_SRN;
		*((uint32_t *) mp->b_wptr)++ = htonl(srn);
		*((uint32_t *) mp->b_wptr)++ = SUA_PARM_CAUSE;
		*((uint32_t *) mp->b_wptr)++ = htonl(cause);
		if ((np = n_data_req(N_RC_FLAG, N_QOS_PPI_SUA, (rc << 8) | sls, mp)))
			return (np);
		freeb(mp);
	}
	return (NULL);
}

/*
 *  SUA_CONS_CODT
 *  ------------------------------------------------------------------------
 */
static inline mblk_t *
sua_codt(uint rc, uint seq, uint drn, mblk_t *dp)
{
	mblk_t *mp, *np;
	static const size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_RC)
	    + UA_SIZE(SUA_PARM_SEQ)
	    + UA_SIZE(SUA_PARM_DRN)
	    + UA_PHDR_SIZE;

	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		*((uint32_t *) mp->b_wptr)++ = SUA_CONS_CODT;
		*((uint32_t *) mp->b_wptr)++ = __constant_htonl(mlen);
		*((uint32_t *) mp->b_wptr)++ = UA_PARM_RC;
		*((uint32_t *) mp->b_wptr)++ = htonl(rc);
		*((uint32_t *) mp->b_wptr)++ = SUA_PARM_SEQ;
		*((uint32_t *) mp->b_wptr)++ = htonl(seq);
		*((uint32_t *) mp->b_wptr)++ = SUA_PARM_DRN;
		*((uint32_t *) mp->b_wptr)++ = htonl(drn);
		*((uint32_t *) mp->b_wptr)++ = UA_PARM(SUA_PARM_DATA, msgdsize(dp));
		mp->b_cont = dp;
		if ((np = n_data_req(N_RC_FLAG, N_QOS_PPI_SUA, (rc << 8) | sls, mp)))
			return (np);
		freeb(mp);
	}
	return (NULL);
}

/*
 *  SUA_CONS_CODA
 *  ------------------------------------------------------------------------
 */
static inline mblk_t *
sua_coda(uint rc, uint drn, uint *seq, uint *credit)
{
	mblk_t *mp, *np;
	const size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_RC)
	    + UA_SIZE(SUA_PARM_DRN)
	    + seq ? UA_SIZE(SUA_PARM_SEQ) : 0 + credit ? UA_SIZE(SUA_PARM_CREDIT) : 0;

	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		*((uint32_t *) mp->b_wptr)++ = SUA_CONS_CODA;
		*((uint32_t *) mp->b_wptr)++ = htonl(mlen);
		*((uint32_t *) mp->b_wptr)++ = UA_PARM_RC;
		*((uint32_t *) mp->b_wptr)++ = htonl(rc);
		*((uint32_t *) mp->b_wptr)++ = SUA_PARM_DRN;
		*((uint32_t *) mp->b_wptr)++ = htonl(drn);
		if (seq) {
			*((uint32_t *) mp->b_wptr)++ = SUA_PARM_SEQ;
			*((uint32_t *) mp->b_wptr)++ = htonl(*seq);
		}
		if (credit) {
			*((uint32_t *) mp->b_wptr)++ = SUA_PARM_CREDIT;
			*((uint32_t *) mp->b_wptr)++ = htonl(*credit);
		}
		if ((np = n_data_req(N_RC_FLAG, N_QOS_PPI_SUA, (rc << 8) | sls, mp)))
			return (np);
		freeb(mp);
	}
	return (NULL);
}

/*
 *  SUA_CONS_COERR
 *  ------------------------------------------------------------------------
 */
static inline mblk_t *
sua_coerr(uint rc, uint drn, uint cause)
{
	mblk_t *mp, *np;
	static const size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_RC)
	    + UA_SIZE(SUA_PARM_DRN)
	    + UA_SIZE(SUA_PARM_CAUSE);

	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		*((uint32_t *) mp->b_wptr)++ = SUA_CONS_COERR;
		*((uint32_t *) mp->b_wptr)++ = __constant_htonl(mlen);
		*((uint32_t *) mp->b_wptr)++ = UA_PARM_RC;
		*((uint32_t *) mp->b_wptr)++ = htonl(rc);
		*((uint32_t *) mp->b_wptr)++ = SUA_PARM_DRN;
		*((uint32_t *) mp->b_wptr)++ = htonl(drn);
		*((uint32_t *) mp->b_wptr)++ = SUA_PARM_CAUSE;
		*((uint32_t *) mp->b_wptr)++ = htonl(cause);
		if ((np = n_data_req(N_RC_FLAG, N_QOS_PPI_SUA, (rc << 8) | sls, mp)))
			return (np);
		freeb(mp);
	}
	return (NULL);
}

/*
 *  SUA_CONS_COIT
 *  ------------------------------------------------------------------------
 */
static inline mblk_t *
sua_coit(uint rc, uint flags, uint srn, uint drn, uint seq, uint credit)
{
	mblk_t *mp, *np;
	static const size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_RC)
	    + UA_SIZE(SUA_PARM_FLAGS)
	    + UA_SIZE(SUA_PARM_SRN)
	    + UA_SIZE(SUA_PARM_DRN)
	    + UA_SIZE(SUA_PARM_SEQ)
	    + UA_SIZE(SUA_PARM_CREDIT);

	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		*((uint32_t *) mp->b_wptr)++ = SUA_CONS_COIT;
		*((uint32_t *) mp->b_wptr)++ = __constant_htonl(mlen);
		*((uint32_t *) mp->b_wptr)++ = UA_PARM_RC;
		*((uint32_t *) mp->b_wptr)++ = htonl(rc);
		*((uint32_t *) mp->b_wptr)++ = SUA_PARM_SRN;
		*((uint32_t *) mp->b_wptr)++ = htonl(srn);
		*((uint32_t *) mp->b_wptr)++ = SUA_PARM_DRN;
		*((uint32_t *) mp->b_wptr)++ = htonl(drn);
		*((uint32_t *) mp->b_wptr)++ = SUA_PARM_SEQ;
		*((uint32_t *) mp->b_wptr)++ = htonl(seq);
		*((uint32_t *) mp->b_wptr)++ = SUA_PARM_CREDIT;
		*((uint32_t *) mp->b_wptr)++ = htonl(credit);
		if ((np = n_data_req(N_RC_FLAG, N_QOS_PPI_SUA, (rc << 8) | sls, mp)))
			return (np);
		freeb(mp);
	}
	return (NULL);
}

#endif				/* __SUA_ASP_H__ */
