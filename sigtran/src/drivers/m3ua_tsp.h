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

 $Log: m3ua_tsp.h,v $
 Revision 0.9.2.1  2006/10/17 11:55:42  brian
 - copied files into new packages from strss7 package

 Revision 0.9.2.1  2004/08/21 10:14:45  brian
 - Force checkin on branch.

 Revision 0.9  2004/01/17 08:20:21  brian
 - Added files for 0.9 baseline autoconf release.

 Revision 0.8.2.1  2002/10/18 03:27:43  brian
 Indentation changes only.

 Revision 0.8  2002/04/02 08:20:42  brian
 Started Linux 2.4 development branch.

 Revision 0.7  2001/07/29 10:46:23  brian
 Split up M3UA files.

 *****************************************************************************/

#ifndef __M3UA_TSP_H__
#define __M3UA_TSP_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

#include "../../include/tli.h"

/*
 *  =========================================================================
 *
 *  M3UA --> TSP (Transport Service Provider)
 *
 *  =========================================================================
 *  
 *  M3UA state machine and layer management use these inlines to generate
 *  primitives to be sent to the Transport Servicei Provider (TSP) for
 *  underlying transports to the ASP based on the XNS/TPI.
 */
/*
 *  T_INFO_REQ
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *t_info_req(void)
{
	mblk_t *mp;
	struct T_info_req *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (struct T_info_req *) mp->b_wptr;
		p->PRIM_type = T_INFO_REQ;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  T_ADDR_REQ
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *t_addr_req(void)
{
	mblk_t *mp;
	struct T_addr_req *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (struct T_addr_req *) mp->b_wptr;
		p->PRIM_type = T_ADDR_REQ;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  T_OPTMGMT_REQ
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *t_optmgmt_req(opt_ptr, opt_len, flags)
	const caddr_t opt_ptr;
	const size_t opt_len;
	const uint flags;
{
	mblk_t *mp;
	struct T_optmgmt_req *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (struct T_optmgmt_req_t *) mp->b_wptr;
		p->PRIM_type = T_OPTMGMT_REQ;
		p->OPT_length = opt_len;
		p->OPT_offset = opt_len ? sizeof(*p) : 0;
		p->MGMT_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
	}
	return (mp);
}

/*
 *  T_BIND_REQ
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *t_bind_req(add_ptr, add_len, cons)
	const caddr_t add_ptr;
	const size_t add_len;
	const uint cons;
{
	mblk_t *mp;
	struct T_bind_req *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (struct T_bind_req *) mp->b_wptr;
		p->PRIM_type = T_BIND_REQ;
		p->ADDR_length = add_len;
		p->ADDR_offset = add_len ? sizeof(*p) : 0;
		p->CONIND_number = cons;
		mp->b_wptr += sizeof(*p);
		bcopy(add_ptr, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
	}
	return (mp);
}

/*
 *  T_UNBIND_REQ
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *t_unbind_req(void)
{
	mblk_t *mp;
	struct T_unbind_req *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (struct T_unbind_req *) mp->b_wptr;
		p->PRIM_type = T_UNBIND_REQ;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  T_CONN_REQ
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *t_conn_req(dst_ptr, dst_len, opt_ptr, opt_len)
	const caddr_t dst_ptr;
	const size_t dst_len;
	const caddr_t opt_ptr;
	const caddr_t opt_len;
{
	mblk_t *mp;
	struct T_conn_req *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (struct T_conn_req *) mp->b_wptr;
		p->PRIM_type = T_CONN_REQ;
		p->DEST_length = dst_len;
		p->DEST_offset = dst_len ? sizeof(*p) : 0;
		p->OPT_length = opt_len;
		p->OPT_offset = opt_len ? sizeof(*p) + dst_len : 0;
		mp->b_wptr += sizeof(*p);
		bcopy(dst_ptr, mp->b_wptr, dst_len);
		mp->b_wptr += dst_len;
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
	}
	return (mp);
}

/*
 *  T_CONN_RES
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *t_conn_res(aq, opt_ptr, opt_len, seq)
	const queue_t *aq;
	const caddr_t opt_ptr;
	const size_t opt_len;
	const uint seq;
{
	mblk_t *mp;
	struct T_conn_res *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (struct T_conn_res *) mp->b_wptr;
		p->PRIM_type = T_CONN_RES;
		p->ACCEPTOR_id = (t_scalar_t) aq;
		p->OPT_length = opt_len;
		p->OPT_offset = opt_len ? sizeof(*p) : 0;
		p->SEQ_number = seq;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  T_DATA_REQ
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *t_data_req(flag)
	const uint flag;
{
	mblk_t *mp;
	struct T_data_req *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (struct T_data_req *) mp->b_wptr;
		p->PRIM_type = T_DATA_REQ;
		p->MORE_flag = flag;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  T_EXDATA_REQ
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *t_exdata_req(flag)
	const uint flag;
{
	mblk_t *mp;
	struct T_exdata_req *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (struct T_exdata_req *) mp->b_wptr;
		p->PRIM_type = T_EXDATA_REQ;
		p->MORE_flag = flag;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  T_OPTDATA_REQ
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *t_optdata_req(flag, opt_ptr, opt_len)
	const uint flag;
	const caddr_t opt_ptr;
	const size_t opt_len;
{
	mblk_t *mp;
	struct T_optdata_req *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (struct T_optdata_req *) mp->b_wptr;
		p->PRIM_type = T_OPTDATA_REQ;
		p->DATA_flag = flag;
		p->OPT_length = opt_len;
		p->OPT_offset = opt_len ? sizeof(*p) : 0;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  T_ORDREL_REQ
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *t_ordrel_req(void)
{
	mblk_t *mp;
	struct T_ordrel_req *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (struct T_ordrel_req *) mp->b_wptr;
		p->PRIM_type = T_ORDREL_REQ;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  T_DISCON_REQ
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *t_discon_req(void)
{
	mblk_t *mp;
	struct T_discon_req *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (struct T_discon_req *) mp->b_wptr;
		p->PRIM_type = T_DISCON_REQ;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

#endif				/* __M3UA_TSP_H__ */
