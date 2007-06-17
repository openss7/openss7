/*****************************************************************************

 @(#) $Id: tua_asp.h,v 0.9.2.3 2007/06/17 01:56:36 brian Exp $

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

 Last Modified $Date: 2007/06/17 01:56:36 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: tua_asp.h,v $
 Revision 0.9.2.3  2007/06/17 01:56:36  brian
 - updates for release, remove any later language

 *****************************************************************************/

#ifndef __TUA_ASP_H__
#define __TUA_ASP_H__

#ident "@(#) $RCSfile: tua_asp.h,v $ $Name:  $($Revision: 0.9.2.3 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

#include "../ua/ua_asp.h"	/* UA --> UA Common Messages */
#include "tua_msg.h"		/* TUA specific messages */

/*
 *  =========================================================================
 *
 *  TUA Peer Messages
 *
 *  =========================================================================
 *  These inline functions build TUA peer messages.
 */

/*
 *  TUA_TDHM_UNI	0x00
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *tua_uni(uint rc, ...)
{
	mblk_t *mp, *np;
	const size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_RC);
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		*((uint32_t *) mp->b_wptr)++ = TUA_TDHM_UNI;
		*((uint32_t *) mp->b_wptr)++ = htonl(mlen);
		*((uint32_t *) mp->b_wptr)++ = UA_PDHR_RC;
		*((uint32_t *) mp->b_wptr)++ = htonl(rc);
		/* 
		 *  FIXME: more...
		 */
		if ((np = n_data_req(N_RC_FLAG, N_QOS_PPI_TUA, rc, mp)))
			return (np);
		freeb(mp);
	}
	return (NULL);
}

/*
 *  TUA_TDHM_BEG	0x01
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *tua_beg(uint rc, ...)
{
	mblk_t *mp, *np;
	const size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_RC);
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		*((uint32_t *) mp->b_wptr)++ = TUA_TDHM_BEG;
		*((uint32_t *) mp->b_wptr)++ = htonl(mlen);
		*((uint32_t *) mp->b_wptr)++ = UA_PDHR_RC;
		*((uint32_t *) mp->b_wptr)++ = htonl(rc);
		/* 
		 *  FIXME: more...
		 */
		if ((np = n_data_req(N_RC_FLAG, N_QOS_PPI_TUA, rc, mp)))
			return (np);
		freeb(mp);
	}
	return (NULL);
}

/*
 *  TUA_TDHM_CON	0x02
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *tua_con(uint rc, ...)
{
	mblk_t *mp, *np;
	const size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_RC);
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		*((uint32_t *) mp->b_wptr)++ = TUA_TDHM_CON;
		*((uint32_t *) mp->b_wptr)++ = htonl(mlen);
		*((uint32_t *) mp->b_wptr)++ = UA_PDHR_RC;
		*((uint32_t *) mp->b_wptr)++ = htonl(rc);
		/* 
		 *  FIXME: more...
		 */
		if ((np = n_data_req(N_RC_FLAG, N_QOS_PPI_TUA, rc, mp)))
			return (np);
		freeb(mp);
	}
	return (NULL);
}

/*
 *  TUA_TDHM_END	0x03
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *tua_end(uint rc, ...)
{
	mblk_t *mp, *np;
	const size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_RC);
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		*((uint32_t *) mp->b_wptr)++ = TUA_TDHM_END;
		*((uint32_t *) mp->b_wptr)++ = htonl(mlen);
		*((uint32_t *) mp->b_wptr)++ = UA_PDHR_RC;
		*((uint32_t *) mp->b_wptr)++ = htonl(rc);
		/* 
		 *  FIXME: more...
		 */
		if ((np = n_data_req(N_RC_FLAG, N_QOS_PPI_TUA, rc, mp)))
			return (np);
		freeb(mp);
	}
	return (NULL);
}

/*
 *  TUA_TDHM_U_ABT	0x04
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *tua_u_abt(uint rc, ...)
{
	mblk_t *mp, *np;
	const size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_RC);
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		*((uint32_t *) mp->b_wptr)++ = TUA_TDHM_U_ABT;
		*((uint32_t *) mp->b_wptr)++ = htonl(mlen);
		*((uint32_t *) mp->b_wptr)++ = UA_PDHR_RC;
		*((uint32_t *) mp->b_wptr)++ = htonl(rc);
		/* 
		 *  FIXME: more...
		 */
		if ((np = n_data_req(N_RC_FLAG, N_QOS_PPI_TUA, rc, mp)))
			return (np);
		freeb(mp);
	}
	return (NULL);
}

/*
 *  TUA_TDHM_P_ABT	0x05
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *tua_p_abt(uint rc, ...)
{
	mblk_t *mp, *np;
	const size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_RC);
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		*((uint32_t *) mp->b_wptr)++ = TUA_TDHM_P_ABT;
		*((uint32_t *) mp->b_wptr)++ = htonl(mlen);
		*((uint32_t *) mp->b_wptr)++ = UA_PDHR_RC;
		*((uint32_t *) mp->b_wptr)++ = htonl(rc);
		/* 
		 *  FIXME: more...
		 */
		if ((np = n_data_req(N_RC_FLAG, N_QOS_PPI_TUA, rc, mp)))
			return (np);
		freeb(mp);
	}
	return (NULL);
}

/*
 *  TUA_TDHM_NOT	0x06
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *tua_not(uint rc, ...)
{
	mblk_t *mp, *np;
	const size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_RC);
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		*((uint32_t *) mp->b_wptr)++ = TUA_TDHM_NOT;
		*((uint32_t *) mp->b_wptr)++ = htonl(mlen);
		*((uint32_t *) mp->b_wptr)++ = UA_PDHR_RC;
		*((uint32_t *) mp->b_wptr)++ = htonl(rc);
		/* 
		 *  FIXME: more...
		 */
		if ((np = n_data_req(N_RC_FLAG, N_QOS_PPI_TUA, rc, mp)))
			return (np);
		freeb(mp);
	}
	return (NULL);
}

/*
 *  TUA_TCHM_IVK	0x00
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *tua_ivk(uint rc, ...)
{
	mblk_t *mp, *np;
	const size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_RC);
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		*((uint32_t *) mp->b_wptr)++ = TUA_TDHM_IVK;
		*((uint32_t *) mp->b_wptr)++ = htonl(mlen);
		*((uint32_t *) mp->b_wptr)++ = UA_PDHR_RC;
		*((uint32_t *) mp->b_wptr)++ = htonl(rc);
		/* 
		 *  FIXME: more...
		 */
		if ((np = n_data_req(N_RC_FLAG, N_QOS_PPI_TUA, rc, mp)))
			return (np);
		freeb(mp);
	}
	return (NULL);
}

/*
 *  TUA_TCHM_RES	0x01
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *tua_res(uint rc, ...)
{
	mblk_t *mp, *np;
	const size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_RC);
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		*((uint32_t *) mp->b_wptr)++ = TUA_TDHM_RES;
		*((uint32_t *) mp->b_wptr)++ = htonl(mlen);
		*((uint32_t *) mp->b_wptr)++ = UA_PDHR_RC;
		*((uint32_t *) mp->b_wptr)++ = htonl(rc);
		/* 
		 *  FIXME: more...
		 */
		if ((np = n_data_req(N_RC_FLAG, N_QOS_PPI_TUA, rc, mp)))
			return (np);
		freeb(mp);
	}
	return (NULL);
}

/*
 *  TUA_TCHM_U_ERR	0x02
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *tua_u_err(uint rc, ...)
{
	mblk_t *mp, *np;
	const size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_RC);
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		*((uint32_t *) mp->b_wptr)++ = TUA_TDHM_U_ERR;
		*((uint32_t *) mp->b_wptr)++ = htonl(mlen);
		*((uint32_t *) mp->b_wptr)++ = UA_PDHR_RC;
		*((uint32_t *) mp->b_wptr)++ = htonl(rc);
		/* 
		 *  FIXME: more...
		 */
		if ((np = n_data_req(N_RC_FLAG, N_QOS_PPI_TUA, rc, mp)))
			return (np);
		freeb(mp);
	}
	return (NULL);
}

/*
 *  TUA_TCHM_REJ	0x03
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *tua_rej(uint rc, ...)
{
	mblk_t *mp, *np;
	const size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_RC);
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		*((uint32_t *) mp->b_wptr)++ = TUA_TDHM_REJ;
		*((uint32_t *) mp->b_wptr)++ = htonl(mlen);
		*((uint32_t *) mp->b_wptr)++ = UA_PDHR_RC;
		*((uint32_t *) mp->b_wptr)++ = htonl(rc);
		/* 
		 *  FIXME: more...
		 */
		if ((np = n_data_req(N_RC_FLAG, N_QOS_PPI_TUA, rc, mp)))
			return (np);
		freeb(mp);
	}
	return (NULL);
}

/*
 *  UA_RKMM_REG_REQ
 *  ------------------------------------------------------------------------
 */
static inline mblk_t *tua_reg_req(...)
{
}

/*
 *  UA_RKMM_REG_RSP
 *  ------------------------------------------------------------------------
 */
static inline mblk_t *tua_reg_rsp(...)
{
}

/*
 *  UA_RKMM_DEREG_REQ
 *  ------------------------------------------------------------------------
 */
static inline mblk_t *tua_dereg_req(...)
{
}

/*
 *  UA_RKMM_DEREG_RSP
 *  ------------------------------------------------------------------------
 */
static inline mblk_t *tua_dereg_rsp(...)
{
}

#endif				/* __TUA_ASP_H__ */
