/*****************************************************************************

 @(#) $Id: sctp.h,v 0.9.2.6 2008-04-28 23:13:26 brian Exp $

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

 Last Modified $Date: 2008-04-28 23:13:26 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sctp.h,v $
 Revision 0.9.2.6  2008-04-28 23:13:26  brian
 - updated headers for release

 Revision 0.9.2.5  2007/08/14 06:22:27  brian
 - GPLv3 header update

 Revision 0.9.2.4  2007/06/17 01:57:13  brian
 - updates for release, remove any later language

 *****************************************************************************/

#ifndef __SS7_SCTP_H__
#define __SS7_SCTP_H__

#ident "@(#) $RCSfile: sctp.h,v $ $Name:  $($Revision: 0.9.2.6 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

#ifdef __KERNEL__
#include "../../include/npi.h"
#else
#include <sys/npi.h>
#endif

/*
 *  These are SCTP specific definitions for NPI...
 */

/*
 *  QOS structure types for SCTP
 */
#define N_QOS_SEL_SCTP		0x0201
#define N_QOS_OPT_SEL_SCTP	0x0202
#define N_QOS_RANGE_SCTP	0x0203
#define N_QOS_STR_SEL_SCTP	0x0204

typedef struct {
	np_ulong n_qos_type;		/* always N_QOS_SEL_SCTP */
	/* 
	 *  FIXME: more...
	 */
} N_qos_sel_sctp_t;

typedef struct {
	np_ulong n_qos_type;		/* always N_QOS_OPT_SEL_SCTP */
	/* 
	 *  FIXME: more...
	 */
} N_qos_opt_sel_sctp_t;

typedef struct {
	np_ulong n_qos_type;		/* always N_QOS_RANGE_SCTP */
	/* 
	 *  FIXME: more...
	 */
} N_qos_range_sctp_t;

typedef struct {
	np_ulong n_qos_type;		/* always N_QOS_STR_SEL_SCTP */
	long ppi;			/* payload protocol identifier */
	long sid;			/* stream id */
	long ssn;			/* strean sequence number */
	long tsn;			/* transmit sequence number */
} N_qos_str_sel_sctp_t;

#define N_QOS_PPI_IUA
#define N_QOS_PPI_M2UA
#define N_QOS_PPI_M3UA
#define N_QOS_PPI_SUA
#define N_QOS_PPI_TUA

/*
 *  NC data transfer request with options.
 */
#define N_data_req_t SCTP_data_req_t
typedef struct {
	np_ulong PRIM_type;		/* always N_DATA_REQ */
	np_ulong DATA_xfer_flags;	/* data transfer flags */
	np_ulong QOS_length;		/* QOS parameter set length */
	np_ulong QOS_offset;		/* QOS parameter set offset */
} SCTP_data_req_t;

/*
 *  NC expedited data request with options
 */
#define N_exdata_req_t SCTP_exdata_req_t
typedef struct {
	np_ulong PRIM_type;		/* always N_EXDATA_REQ */
	np_ulong QOS_length;		/* QOS parameter set length */
	np_ulong QOS_offset;		/* QOS parameter set offset */
} SCTP_exdata_req_t;

/*
 *  NC data acknowledgement request with options
 */
#define N_datack_req_t SCTP_datack_req_t
typedef struct {
	np_ulong PRIM_type;		/* always N_DATACK_REQ */
	np_ulong QOS_length;		/* QOS parameter set length */
	np_ulong QOS_offset;		/* QOS parameter set offset */
} SCTP_datack_req_t
/*
 *  NC data transfer indication with options
 */
#define N_data_ind_t SCTP_data_ind_t
    typedef struct {
	np_ulong PRIM_type;		/* always N_DATA_IND */
	np_ulong DATA_xfer_flags;	/* data transfer flags */
	np_ulong QOS_length;		/* QOS parameter set length */
	np_ulong QOS_offset;		/* QOS parameter set offset */
} SCTP_data_ind_t;

/*
 *  NC expedited data indication with options
 */
#define N_exdata_ind_t SCTP_exdata_ind_t
typedef struct {
	np_ulong PRIM_type;		/* always N_EXDATA_REQ */
	np_ulong QOS_length;		/* QOS parameter set length */
	np_ulong QOS_offset;		/* QOS parameter set offset */
} SCTP_exdata_ind_t;

/*
 *  NC data acknowledgement indications with options
 */
#define N_datack_ind_t SCTP_datack_ind_t
typedef struct {
	np_ulong PRIM_type;		/* always N_DATACK_IND */
	np_ulong QOS_length;		/* QOS parameter set length */
	np_ulong QOS_offset;		/* QOS parameter set offset */
} SCTP_datack_ind_t;

#ifdef __KERNEL__
/*
 *  N_DATA_REQ       3 - Connection-Mode data transfer request
 *  ---------------------------------------------------------------
 */
#define n_data_req sctp_data_req
static inline mblk_t *
sctp_data_req(uint flags, uint ppi, uint sid, mblk_t *dp)
{
	mblk_t *mp;
	SCTP_data_req_t *p;
	N_qos_str_sel_sctp_t *q;

	if ((mp = allocb(sizeof(*p) + sizeof(*q), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (SCTP_data_req_t *) mp->b_wptr;
		p->PRIM_type = N_DATA_REQ;
		p->DATA_xfer_flags = flags;
		p->QOS_length = sizeof(*q);
		p->QOS_offset = sizeof(*p);
		mp->b_wptr += sizeof(*p);
		q = (N_qos_str_sel_sctp_t *) mp->b_wptr;
		q->n_qos_type = N_QOS_STR_SEL_SCTP;
		q->ppi = ppi;
		q->sid = sid;
		q->ssn = N_QOS_UNKNOWN;
		q->tsn = N_QOS_UNKNOWN;
		mp->b_wptr += sizeof(*q);
		mp->b_cont = dp;
	}
	return (mp);
}

/*
 *  N_EXDATA_REQ     4 - Expedited data request
 *  ---------------------------------------------------------------
 */
#define n_exdata_req sctp_exdata_req
static inline mblk_t *
sctp_exdata_req(uint ppi, uint sid, mblk_t *dp)
{
	mblk_t *mp;
	SCTP_exdata_req_t *p;
	N_qos_str_sel_sctp_t *q;

	if ((mp = allocb(sizeof(*p) + sizeof(*q), BPRI_MED))) {
		mp->b_band = 254;
		mp->b_datap->db_type = M_PROTO;
		p = (SCTP_exdata_req_t *) mp->b_wptr;
		p->PRIM_type = N_EXDATA_REQ;
		p->QOS_length = sizeof(*q);
		p->QOS_offset = sizeof(*p);
		mp->b_wptr += sizeof(*p);
		q = (N_qos_str_sel_sctp_t *) mp->b_wptr;
		q->n_qos_type = N_QOS_STR_SEL_SCTP;
		q->ppi = ppi;
		q->sid = sid;
		q->ssn = N_QOS_UNKNOWN;
		q->tsn = N_QOS_UNKNOWN;
		mp->b_wptr += sizeof(*q);
		mp->b_cont = dp;
	}
	return (mp);
}

/*
 *  N_DATACK_REQ    23 - Data acknowledgement request
 *  ---------------------------------------------------------------
 */
#define n_datack_req sctp_datack_req
static inline mblk_t *
sctp_datack_req(uint ppi, uint sid, uint ssn, uint tsn)
{
	mblk_t *mp;
	SCTP_datack_req_t *p;
	N_qos_str_sel_sctp_t *q;

	if ((mp = allocb(sizeof(*p) + sizeof(*q), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (SCTP_datack_req_t *) mp->b_wptr;
		p->PRIM_type = N_DATACK_REQ;
		p->QOS_length = sizeof(*q);
		p->QOS_offset = sizeof(*p);
		mp->b_wptr += sizeof(*p);
		q = (N_qos_str_sel_sctp_t *) mp->b_wptr;
		q->n_qos_type = N_QOS_STR_SEL_SCTP;
		q->ppi = ppi;
		q->sid = sid;
		q->ssn = ssn;
		q->tsn = tsn;
		mp->b_wptr += sizeof(*q);
	}
	return (mp);
}

/*
 *  N_DATA_IND      14 - Incoming connection-mode data indication
 *  ---------------------------------------------------------------
 */
#define n_data_ind sctp_data_ind
static inline mblk_t *
sctp_data_ind(uint flags, uint ppi, uint sid, uint ssn, uint tsn, mblk_t *dp)
{
	mblk_t *mp;
	SCTP_data_ind_t *p;
	N_qos_str_sel_sctp_t *q;

	if ((mp = allocb(sizeof(*p) + sizeof(*q), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (SCTP_data_req_t *) mp->b_wptr;
		p->PRIM_type = N_DATA_IND;
		p->DATA_xfer_flags = flags;
		p->QOS_length = sizeof(*q);
		p->QOS_offset = sizeof(*p);
		mp->b_wptr += sizeof(*p);
		q = (N_qos_str_sel_sctp_t *) mp->b_wptr;
		q->n_qos_type = N_QOS_STR_SEL_SCTP;
		q->ppi = ppi;
		q->sid = sid;
		q->ssn = ssn;
		q->tsn = tsn;
		mp->b_wptr += sizeof(*q);
		mp->b_cont = dp;
	}
	return (mp);
}

/*
 *  N_EXDATA_IND    15 - Incoming expedited data indication
 *  ---------------------------------------------------------------
 */
#define n_exdata_ind sctp_exdata_ind
static inline mblk_t *
sctp_exdata_ind(uint ppi, uint sid, uint ssn, uint tsn, mblk_t *dp)
{
	mblk_t *mp;
	SCTP_exdata_ind_t *p;
	N_qos_str_sel_sctp_t *q;

	if ((mp = allocb(sizeof(*p) + sizeof(*q), BPRI_MED))) {
		mp->b_band = 254;
		mp->b_datap->db_type = M_PROTO;
		p = (SCTP_exdata_ind_t *) mp->b_wptr;
		p->PRIM_type = N_EXDATA_IND;
		p->QOS_length = sizeof(*q);
		p->QOS_offset = sizeof(*p);
		mp->b_wptr += sizeof(*p);
		q = (N_qos_str_sel_sctp_t *) mp->b_wptr;
		q->n_qos_type = N_QOS_STR_SEL_SCTP;
		q->ppi = ppi;
		q->sid = sid;
		q->ssn = ssn;
		q->tsn = tsn;
		mp->b_wptr += sizeof(*q);
		mp->b_cont = dp;
	}
	return (mp);
}

/*
 *  N_DATACK_IND    24 - Data acknowledgement indication
 *  ---------------------------------------------------------------
 */
#define n_datack_ind sctp_datack_ind
static inline mblk_t *
sctp_datack_ind(uint ppi, uint sid, uint ssn, uint tsn)
{
	mblk_t *mp;
	SCTP_datack_ind_t *p;
	N_qos_str_sel_sctp_t *q;

	if ((mp = allocb(sizeof(*p) + sizeof(*q), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (SCTP_datack_ind_t *) mp->b_wptr;
		p->PRIM_type = N_DATACK_IND;
		p->QOS_length = sizeof(*q);
		p->QOS_offset = sizeof(*p);
		mp->b_wptr += sizeof(*p);
		q = (N_qos_str_sel_sctp_t *) mp->b_wptr;
		q->n_qos_type = N_QOS_STR_SEL_SCTP;
		q->ppi = ppi;
		q->sid = sid;
		q->ssn = ssn;
		q->tsn = tsn;
		mp->b_wptr += sizeof(*q);
	}
	return (mp);
}
#endif

#endif				/* __SS7_SCTP_H__ */
