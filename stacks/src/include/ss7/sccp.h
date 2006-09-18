/*****************************************************************************

 @(#) $Id: sccp.h,v 0.9.2.3 2006/09/18 13:52:33 brian Exp $

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

 Last Modified $Date: 2006/09/18 13:52:33 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SS7_SCCP_H__
#define __SS7_SCCP_H__

#ident "@(#) $RCSfile: sccp.h,v $ $Name:  $($Revision: 0.9.2.3 $) Copyright (c) 2001-2004  OpenSS7 Corporation"

/* This file can be processed by doxygen(1). */

#ifdef __KERNEL__
#include "../../include/sys/npi.h"
#else
#include <sys/npi.h>
#endif

/*
 *  These are SCCP specific definitions for NPI...
 */

/*
 *  QOS structure types for SCCP
 */
#define N_QOS_SEL_SCCP		0x0201
#define N_QOS_OPT_SEL_SCCP	0x0202
#define N_QOS_RANGE_SCCP	0x0203

typedef struct {
	np_ulong n_qos_type;		/* always N_QOS_SEL_SCCP */
	long protocol_class;		/* protocol class 2 or 3 */
	long option_flags;		/* options flags (return option) *//* XXX */
	long importance;		/* importance */
	long sequence_selection;	/* selected SLS value */
	long message_priority;		/* MTP message priority */
} N_qos_sel_sccp_t;

typedef struct {
	np_ulong n_qos_type;		/* always N_QOS_OPT_SEL_SCCP */
	long protocol_class;		/* protocol class 0 or 1 */
} N_qos_opt_sel_sccp_t;

typedef struct {
	np_ulong n_qos_type;		/* always N_QOS_RANGE_SCCP */
	long protocol_classes;		/* protocol class support range */
	long sequence_selection;	/* SLS range */
} N_qos_range_sccp_t;

/*
 *  Protocol Class bit masks for use with N_QOS_RANGE_SCCP protocol_classes
 */
#define N_QOS_PCLASS_0		0x01
#define N_QOS_PCLASS_1		0x02
#define N_QOS_PCLASS_2		0x04
#define N_QOS_PCLASS_3		0x08
#define N_QOS_PCLASS_ALL	0xff

/*
 *  SCCP Address format:
 */

typedef struct sccp_addr {
	ulong ni;			/* network indicator */
	ulong ri;			/* routing indicator */
	ulong pc;			/* point code (-1 not present) */
	ulong ssn;			/* subsystem number (-1 not present) */
	ulong gtt;			/* type of global title */
	ulong tt;			/* translaction type */
	ulong es;			/* encoding scheme */
	ulong nplan;			/* numbering plan */
	ulong nai;			/* nature of address indicator */
	ulong alen;			/* address length */
	uint8_t addr[0];		/* adress digits */
	/*
	   followed by alen address bytes 
	 */
} sccp_addr_t;

#define SCCP_MAX_ADDR_LENGTH 32

#define SCCP_RI_DPC_SSN	    0	/* route on DPC/SSN */
#define SCCP_RI_GT	    1	/* route on GT */

#define SCCP_GTTYPE_NONE    0	/* no GT */
#define SCCP_GTTYPE_NAI	    1	/* NAI only */
#define SCCP_GTTYPE_TT	    2	/* TT only */
#define SCCP_GTTYPE_NP      3	/* TT, ES, NPLAN */
#define SCCP_GTTYPE_NP_NAI  4	/* TT, ES, NPLAN and NAI */

/*
 *  SCCP Causes
 */
/*
 *  SCCP Release Causes used in N_DISCON_IND
 */
#define SCCP_RELC_END_USER_ORIGINATED				0x1000
#define SCCP_RELC_END_USER_CONGESTION				0x1001
#define SCCP_RELC_END_USER_FAILURE				0x1002
#define SCCP_RELC_SCCP_USER_ORIGINATED				0x1003
#define SCCP_RELC_REMOTE_PROCEDURE_ERROR			0x1004
#define SCCP_RELC_INCONSISTENT_CONNECTION_DATA			0x1005
#define SCCP_RELC_ACCESS_FAILURE				0x1006
#define SCCP_RELC_ACCESS_CONGESTION				0x1007
#define SCCP_RELC_SUBSYSTEM_FAILURE				0x1008
#define SCCP_RELC_SUBSYSTEM_CONGESTION				0x1009
#define SCCP_RELC_MTP_FAILURE					0x100a
#define SCCP_RELC_NETWORK_CONGESTION				0x100b
#define SCCP_RELC_EXPIRATION_OF_RESET_TIMER			0x100c
#define SCCP_RELC_EXPIRATION_OF_RECEIVE_INACTIVITY_TIMER	0x100d
#define SCCP_RELC_RESERVED					0x100e
#define SCCP_RELC_UNQUALIFIED					0x100f
#define SCCP_RELC_SCCP_FAILURE					0x1010
/*
 *  SCCP Return Causes used in N_UDERROR_IND
 */
#define SCCP_RETC_NO_ADDRESS_TYPE_TRANSLATION			0x2000
#define	SCCP_RETC_NO_ADDRESS_TRANSLATION			0x2001
#define SCCP_RETC_SUBSYSTEM_CONGESTION				0x2002
#define SCCP_RETC_SUBSYSTEM_FAILURE				0x2003
#define SCCP_RETC_UNEQUIPPED_USER				0x2004
#define SCCP_RETC_MTP_FAILURE					0x2005
#define SCCP_RETC_NETWORK_CONGESTION				0x2006
#define SCCP_RETC_UNQUALIFIED					0x2007
#define SCCP_RETC_MESSAGE_TRANSPORT_ERROR			0x2008
#define SCCP_RETC_LOCAL_PROCESSING_ERROR			0x2009
#define SCCP_RETC_NO_REASSEMBLY_AT_DESTINATION			0x200a
#define SCCP_RETC_SCCP_FAILURE					0x200b
#define SCCP_RETC_SCCP_HOP_COUNTER_VIOLATION			0x200c
#define SCCP_RETC_SEGMENTATION_NOT_SUPPORTED			0x200d
#define SCCP_RETC_SEGMENTATION_FAILURE				0x200e

#define SCCP_RETC_MESSAGE_CHANGE_FAILURE			0x20f7
#define SCCP_RETC_INVALID_INS_ROUTING_REQUEST			0x20f8
#define SCCP_RETC_INVALID_INSI_ROUTING_REQUEST			0x20f9
#define SCCP_RETC_UNAUTHORIZED_MESSAGE				0x20fa
#define SCCP_RETC_MESSAGE_INCOMPATIBILITY			0x20fb
#define SCCP_RETC_CANNOT_PERFORM_ISNI_CONSTRAINED_ROUTING	0x20fc
#define SCCP_RETC_REDUNDANT_ISNI_CONSTRAINED_ROUTING_INFO	0x20fd
#define SCCP_RETC_UNABLE_TO_PERFORM_ISNI_IDENTIFICATION		0x20fe
/*
 *  SCCP Reset Causes used in N_RESET_IND/REQ
 */
#define SCCP_RESC_END_USER_ORIGINATED				0x3000
#define SCCP_RESC_SCCP_USER_ORIGINATED				0x3001
#define SCCP_RESC_MESSAGE_OUT_OF_ORDER_INCORRECT_PS		0x3002
#define SCCP_RESC_MESSAGE_OUT_OF_ORDER_INCORRECT_PR		0x3003
#define SCCP_RESC_REMOTE_PROC_ERROR_MESSAGE_OUT_OF_WINDOW	0x3004
#define SCCP_RESC_REMOTE_PROC_ERROR_INCORRECT_PS_AFTER_INIT	0x3005
#define SCCP_RESC_REMOTE_PROC_ERROR_GENERAL			0x3006
#define SCCP_RESC_REMOTE_END_USER_OPERATIONAL			0x3007
#define SCCP_RESC_NETWORK_OPERATIONAL				0x3008
#define SCCP_RESC_ACCESS_OPERATIONAL				0x3009
#define SCCP_RESC_NETWORK_CONGESTION				0x300a
#define SCCP_RESC_NOT_OBTAINABLE				0x300b
#define SCCP_RESC_UNQUALIFIED					0x300c
/*
 *  SCCP Error Causes
 */
#define SCCP_ERRC_LRN_MISMATCH_UNASSIGNED_DEST_LRN		0x4000
#define SCCP_ERRC_LRN_MISMATCH_INCONSISTENT_SOURCE_LRN		0x4001
#define SCCP_ERRC_POINT_CODE_MISMATCH				0x4002
#define SCCP_ERRC_SERVICE_CLASS_MISMATCH			0x4003
#define SCCP_ERRC_UNQUALIFIED					0x4004
/*
 *  SCCP Refusal Causes used in N_DISCON_IND
 */
#define SCCP_REFC_END_USER_ORIGINATED				0x5000
#define SCCP_REFC_END_USER_CONGESTION				0x5001
#define SCCP_REFC_END_USER_FAILURE				0x5002
#define SCCP_REFC_SCCP_USER_ORIGINATED				0x5003
#define SCCP_REFC_DESTINATION_ADDRESS_UNKNOWN			0x5004
#define SCCP_REFC_DESTINATION_INACCESSIBLE			0x5005
#define SCCP_REFC_NETWORK_RESOURCE_QOS_NOT_AVAIL_PERM		0x5006
#define SCCP_REFC_NETWORK_RESOURCE_QOS_NOT_AVAIL_TRANS		0x5007
#define SCCP_REFC_ACCESS_FAILURE				0x5008
#define SCCP_REFC_ACCESS_CONGESTION				0x5009
#define SCCP_REFC_SUBSYSTEM_FAILURE				0x500a
#define SCCP_REFC_SUBSYSTEM_CONGESTION				0x500b
#define SCCP_REFC_EXPIRATION_OF_NC_ESTAB_TIMER			0x500c
#define SCCP_REFC_INCOMPATIBLE_USER_DATA			0x500d
#define SCCP_REFC_RESERVED					0x500e
#define SCCP_REFC_UNQUALIFIED					0x500f
#define SCCP_REFC_SCCP_HOP_COUNTER_VIOLATION			0x5010
#define SCCP_REFC_SCCP_FAILURE					0x5011
#define SCCP_REFC_NO_ADDRESS_TYPE_TRANSLATION			0x5012
#define SCCP_REFC_UNEQUIPPED_USER				0x5013

/*
 *  Unitdata transfer request.
 */
#define N_unitdata_req_t SCCP_unitdata_req_t
typedef struct {
	np_ulong PRIM_type;		/* always N_UNITDATA_REQ */
	np_ulong DEST_length;		/* destination address length */
	np_ulong DEST_offset;		/* destination address offset */
	np_ulong RESERVED_field[2];	/* reserved field for DLPI compat */
	np_ulong SRC_length;		/* source address length *//* XXX */
	np_ulong SRC_offset;		/* source address offset *//* XXX */
	np_ulong QOS_length;		/* QOS parameter set length *//* XXX */
	np_ulong QOS_offset;		/* QOS parameter set offset *//* XXX */
} SCCP_unitdata_req_t;

/*
 *  Unitdata transfer indication
 */
#define N_unitdata_ind_t SCCP_unitdata_ind_t
typedef struct {
	np_ulong PRIM_type;		/* always N_UNITDATA_IND */
	np_ulong SRC_length;		/* source address length */
	np_ulong SRC_offset;		/* source address offset */
	np_ulong DEST_length;		/* destination address length */
	np_ulong DEST_offset;		/* destination address offset */
	np_ulong ERROR_type;		/* return error cause */
	np_ulong QOS_length;		/* QOS parameter set length *//* XXX */
	np_ulong QOS_offset;		/* QOS parameter set offset *//* XXX */
} SCCP_unitdata_ind_t;

/*
 *  Unitdata error indication for CLNS services
 */
#define N_uderror_ind_t SCCP_uderror_ind_t
typedef struct {
	np_ulong PRIM_type;		/* always N_UDERROR_IND */
	np_ulong DEST_length;		/* destination address length */
	np_ulong DEST_offset;		/* destination address offset */
	np_ulong RESERVED_field;	/* reserved for DLPI compatibility */
	np_ulong ERROR_type;		/* error type */
	np_ulong SRC_length;		/* source address length *//* XXX */
	np_ulong SRC_offset;		/* source address offset *//* XXX */
} SCCP_uderror_ind_t;

#ifdef __KERNEL__
/*
 *  N_UNITDATA_REQ   8 - Connection-less data send request
 *  ---------------------------------------------------------------
 */
#define n_unitdata_req sccp_n_unitdata_req
static inline mblk_t *
sccp_n_unitdata_req(caddr_t dst_ptr, size_t dst_len,
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

/*
 *  N_UNITDATA_IND  20 - Connection-less data receive indication
 *  ---------------------------------------------------------------
 */
#define n_unitdata_ind sccp_n_unitdata_ind
static inline mblk_t *
sccp_n_unitdata_ind(caddr_t dst_ptr, size_t dst_len,
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

/*
 *  N_UDERROR_IND   21 - UNITDATA Error Indication
 *  ---------------------------------------------------------------
 */
#define n_uderror_ind sccp_n_uderror_ind
static inline mblk_t *
sccp_n_uderror_ind(uint etype,
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

#endif				/* __SS7_SCCP_H__ */
