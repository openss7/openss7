/*****************************************************************************

 @(#) $Id: mtp.h,v 0.9.2.2 2005/05/14 08:30:45 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (C) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>

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
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2005/05/14 08:30:45 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SS7_MTP_H__
#define __SS7_MTP_H__

#ident "@(#) $Name:  $($Revision: 0.9.2.2 $) Copyright (c) 1997-2002 OpenSS7 Corporation."

#ifdef __KERNEL__
#include "../../include/npi.h"
#else
#include "<sys/npi.h>"
#endif

/*
 *  These are MTP specific definitions for NPI...
 */

/*
 *  QOS structure types for MTP
 */
#define N_QOS_SEL_MTP		0x0801
#define N_QOS_OPT_SEL_MTP	0x0802
#define N_QOS_RANGE_MTP		0x0803

/*
   QOS for use with N_UNITDATA_REQ, N_UNITDATA_IND, N_UDERROR_IND 
 */
typedef struct {
	np_ulong n_qos_type;		/* always N_QOS_SEL_MTP */
	ulong sls;			/* signalling link selection */
	ulong mp;			/* message priority */
} N_qos_sel_mtp_t;

/*
   QOS for use with N_OPTMGMT_REQ, N_CONN_REQ, N_CONN_RES, N_CONN_IND, N_CONN_CON 
 */
typedef struct {
	np_ulong n_qos_type;		/* always N_QOS_OPT_SEL_MTP */
} N_qos_opt_sel_mtp_t;

/*
   QOS range for use with N_OPTMGMT_REQ 
 */
typedef struct {
	np_ulong n_qos_type;		/* always N_QOS_RANGE_MTP */
	ulong sls_range;		/* signalling link selection */
	ulong mp_range;			/* message priority */
} N_qos_range_mtp_t;

/*
   fix me: make these unique 
 */
#define MTP_DEST_CONGESTED		N_UD_CONGESTION
#define MTP_DEST_PROHIBITED		N_UD_ROUTE_UNAVAIL
#define MTP_DEST_RESTRICTED		N_UD_QOS_UNAVAIL
#define MTP_CLUSTER_PROHIBITED		N_UD_ROUTE_UNAVAIL
#define MTP_CLUSTER_RESTRICTED		N_UD_QOS_UNAVAIL
#define MTP_RESTARTING			N_UD_ROUTE_UNAVAIL
#define MTP_USER_PART_UNKNOWN		N_UD_ROUTE_UNAVAIL
#define MTP_USER_PART_UNEQUIPPED	N_UD_ROUTE_UNAVAIL
#define MTP_USER_PART_UNAVAILABLE	N_UD_ROUTE_UNAVAIL

/*
 *  MTP Address format:
 */
typedef struct mtp_pc {
	ulong ni;			/* network indicator */
	ulong pc;			/* point code */
} mtp_pc_t;

#ifndef N_unitdata_req_t
#define N_unitdata_req_t MTP_unitdata_req_t
/*
 *  Unitdata transfer request.
 */
typedef struct {
	np_ulong PRIM_type;		/* always N_UNITDATA_REQ */
	np_ulong DEST_length;		/* destination address length */
	np_ulong DEST_offset;		/* destination address offset */
	np_ulong RESERVED_field[2];	/* reserved field for DLPI compat */
	np_ulong SRC_length;		/* source address length *//* XXX */
	np_ulong SRC_offset;		/* source address offset *//* XXX */
	np_ulong QOS_length;		/* QOS parameter set length *//* XXX */
	np_ulong QOS_offset;		/* QOS parameter set offset *//* XXX */
} MTP_unitdata_req_t;
#endif

#ifndef N_unitdata_ind_t
#define N_unitdata_ind_t MTP_unitdata_ind_t
/*
 *  Unitdata transfer indication
 */
typedef struct {
	np_ulong PRIM_type;		/* always N_UNITDATA_IND */
	np_ulong SRC_length;		/* source address length */
	np_ulong SRC_offset;		/* source address offset */
	np_ulong DEST_length;		/* destination address length */
	np_ulong DEST_offset;		/* destination address offset */
	np_ulong ERROR_type;		/* return error cause */
	np_ulong QOS_length;		/* QOS parameter set length *//* XXX */
	np_ulong QOS_offset;		/* QOS parameter set offset *//* XXX */
} MTP_unitdata_ind_t;
#endif

#ifndef N_uderror_ind_t
#define N_uderror_ind_t MTP_uderror_ind_t
/*
 *  Unitdata error indication for CLNS services
 */
typedef struct {
	np_ulong PRIM_type;		/* always N_UDERROR_IND */
	np_ulong DEST_length;		/* destination address length */
	np_ulong DEST_offset;		/* destination address offset */
	np_ulong RESERVED_field;	/* reserved for DLPI compatibility */
	np_ulong ERROR_type;		/* error type */
	np_ulong SRC_length;		/* source address length *//* XXX */
	np_ulong SRC_offset;		/* source address offset *//* XXX */
} MTP_uderror_ind_t;
#endif

#ifdef __KERNEL__
/*
 *  N_UNITDATA_REQ   8 - Connection-less data send request
 *  ---------------------------------------------------------------
 */
#ifndef n_unitdata_req
#define n_unitdata_req mtp_n_unitdata_req
static inline mblk_t *
mtp_n_unitdata_req(caddr_t dst_ptr, size_t dst_len,
		   caddr_t src_ptr, size_t src_len, caddr_t qos_ptr, size_t qos_len, mblk_t *dp)
{
	mblk_t *mp;
	N_unitdata_req_t *p;
	if ((mp = allocb(sizeof(*p) + dst_len + src_len + qos_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (N_unitdata_req_t *) mp->b_wptr;
		p->PRIM_type = N_UNITDATA_REQ;
		p->DEST_length = dst_len;
		p->DEST_offset = dst_len ? sizeof(*p) : 0;
		p->SRC_length = src_len;
		p->SRC_offset = src_len ? sizeof(*p) + dst_len : 0;
		p->QOS_length = qos_len;
		p->QOS_offset = qos_len ? sizeof(*p) + dst_len + src_len : 0;
		p->RESERVED_field[0] = 0;
		p->RESERVED_field[1] = 0;
		mp->b_wptr += sizeof(*p);
		bcopy(dst_ptr, mp->b_wptr, dst_len);
		mp->b_wptr += dst_len;
		bcopy(src_ptr, mp->b_wptr, src_len);
		mp->b_wptr += src_len;
		bcopy(qos_ptr, mp->b_wptr, qos_len);
		mp->b_wptr += qos_len;
		mp->b_cont = dp;
	}
	return (mp);
}
#endif
/*
 *  N_UNITDATA_IND  20 - Connection-less data receive indication
 *  ---------------------------------------------------------------
 */
#ifndef n_unitdata_ind
#define n_unitdata_ind mtp_n_unitdata_ind
static inline mblk_t *
mtp_n_unitdata_ind(caddr_t dst_ptr, size_t dst_len,
		   caddr_t src_ptr, size_t src_len, caddr_t qos_ptr, size_t qos_len, mblk_t *dp)
{
	mblk_t *mp;
	N_unitdata_ind_t *p;
	if ((mp = allocb(sizeof(*p) + dst_len + src_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (N_unitdata_ind_t *) mp->b_wptr;
		p->PRIM_type = N_UNITDATA_IND;
		p->DEST_length = dst_len;
		p->DEST_offset = dst_len ? sizeof(*p) : 0;
		p->SRC_length = src_len;
		p->SRC_offset = src_len ? sizeof(*p) + dst_len : 0;
		p->QOS_length = qos_len;
		p->QOS_offset = qos_len ? sizeof(*p) + dst_len + src_len : 0;
		p->ERROR_type = 0;
		mp->b_wptr += sizeof(*p);
		bcopy(dst_ptr, mp->b_wptr, dst_len);
		mp->b_wptr += dst_len;
		bcopy(src_ptr, mp->b_wptr, src_len);
		mp->b_wptr += src_len;
		bcopy(qos_ptr, mp->b_wptr, qos_len);
		mp->b_wptr += qos_len;
		mp->b_cont = dp;
	}
	return (mp);
}
#endif
/*
 *  N_UDERROR_IND   21 - UNITDATA Error Indication
 *  ---------------------------------------------------------------
 */
#ifndef n_uderror_ind
#define n_uderror_ind mtp_n_uderror_ind
static inline mblk_t *
mtp_n_uderror_ind(uint etype,
		  caddr_t dst_ptr, size_t dst_len, caddr_t src_ptr, size_t src_len, mblk_t *dp)
{
	mblk_t *mp;
	N_uderror_ind_t *p;
	if ((mp = allocb(sizeof(*p) + dst_len + src_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (N_uderror_ind_t *) mp->b_wptr;
		p->PRIM_type = N_UDERROR_IND;
		p->ERROR_type = etype;
		p->DEST_length = dst_len;
		p->DEST_offset = dst_len ? sizeof(*p) : 0;
		p->SRC_length = src_len;
		p->SRC_offset = src_len ? sizeof(*p) + dst_len : 0;
		p->RESERVED_field = 0;
		mp->b_wptr += sizeof(*p);
		bcopy(dst_ptr, mp->b_wptr, dst_len);
		mp->b_wptr += dst_len;
		bcopy(src_ptr, mp->b_wptr, src_len);
		mp->b_wptr += src_len;
		mp->b_cont = dp;
	}
	return (mp);
}
#endif
#endif				/* __KERNEL__ */

#endif				/* __SS7_MTP_H__ */
