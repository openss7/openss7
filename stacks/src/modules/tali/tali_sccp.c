/*****************************************************************************

 @(#) $RCSfile: tali_sccp.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2004/08/26 23:38:13 $

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

 Last Modified $Date: 2004/08/26 23:38:13 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: tali_sccp.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2004/08/26 23:38:13 $"

static char const ident[] =
    "$RCSfile: tali_sccp.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2004/08/26 23:38:13 $";

#include "compat.h"

#include "tali.h"
#include "tali_data.h"
#include "tali_msgs.h"

/*
 *  =========================================================================
 *
 *  STREAMS Message Handling
 *
 *  =========================================================================
 *
 *  Management Message Handling
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  -------------------------------------------------------------------------
 *
 *  SCCP User (SCCPU) Stream Message Handling
 *
 *  -------------------------------------------------------------------------
 *
 *  SCCP User --> TALI Primitives
 *  -----------------------------------
 *  This is the SCCPU upper write routines.  This takes SCCP user primitives
 *  and converts them to SCCP M_DATA and M_CTL messages, if required.  If the
 *  provider is a local SG, then the primitives are forwarded as is.
 *
 *  These are primitives send from a local SCCP user to the TALI layer on the
 *  upper mux stream.  This is part of the TALI at an IP node.  Each message
 *  is converted from an SCCP user request or response primitive to a TALI
 *  message.  These functions are pure translation only.  When they return
 *  zero, they return the SCCP primitive translated in 'mpp'.  When they
 *  return non-zero, they return an error and 'mpp' is untouched.
 */

static int sccp_u_info_req(queue_t * q, mblk_t * mp)
{
}
static int sccp_u_bind_req(queue_t * q, mblk_t * mp)
{
}
static int sccp_u_unbind_req(queue_t * q, mblk_t * mp)
{
}
static int sccp_u_unitdata_req(queue_t * q, mblk_t * mp)
{
}
static int sccp_u_optmgmt_req(queue_t * q, mblk_t * mp)
{
}

static int sccp_u_w_ioctl(queue_t * q, mblk_t * mp)
{
}
static int sccp_u_w_data(queue_t * q, mblk_t * mp)
{
	return (-EOPNOTSUPP);
}
static int sccp_u_w_proto(queue_t * q, mblk_t * mp)
{
	switch (*((long *) mp->b_wptr)) {
	case N_INFO_REQ:
		return sccp_u_info_req(q, mp);
	case N_BIND_REQ:
		return sccp_u_bind_req(q, mp);
	case N_UNBIND_REQ:
		return sccp_u_unbind_req(q, mp);
	case N_UNITDATA_REQ:
		return sccp_u_unitdata_req(q, mp);
	case N_OPTMGMT_REQ:
		return sccp_u_optmgmt_req(q, mp);
	}
	return (-EOPNOTSUPP);
}
static int sccp_u_w_pcproto(queue_t * q, mblk_t * mp)
{
	return sccp_u_w_proto(q, mp);
}
static int sccp_u_w_prim(queue_t * q, mblk_t * mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return sccp_u_w_data(q, mp);
	case M_PROTO:
		return sccp_u_w_proto(q, mp);
	case M_PCPROTO:
		return sccp_u_w_pcproto(q, mp);
	case M_IOCTL:
		return sccp_u_w_ioctl(q, mp);
	case M_FLUSH:
		return tali_w_flush(q, mp);
	}
	return (-EOPNOTSUPP);
}

/*
 *  TALI --> SCCP User Primitives
 *  -----------------------------------
 */
static int sccp_u_r_data(queue_t * q, mblk_t * mp)
{
	return sccp_r_msg(q, mp);
}
static int sccp_u_r_ctl(queue_t * q, mblk_t * mp)
{
	return sccp_r_msg(q, mp);
}
static int sccp_u_r_prim(queue_t * q, mblk_t * mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return sccp_u_r_data(q, mp);
	case M_CTL:
		return sccp_u_r_ctl(q, mp);
	case M_FLUSH:
		return tali_r_flush(q, mp);
	}
	return (5);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SCCP Provider (SCCPP) Stream Message Handling
 *
 *  -------------------------------------------------------------------------
 */
static int sccp_l_w_data(queue_t * q, mblk_t * mp)
{
	return sccp_w_msg(q, mp);
}
static int sccp_l_w_ctl(queue_t * q, mblk_t * mp)
{
	return sccp_w_msg(q, mp);
}
static int sccp_l_w_error(queue_t * q, mblk_t * mp)
{
}
static int sccp_l_w_hangup(queue_t * q, mblk_t * mp)
{
}
static int sccp_l_w_prim(queue_t * q, mblk_t * mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return sccp_l_w_data(q, mp);
	case M_CTL:
		return sccp_l_w_ctl(q, mp);
	case M_ERROR:
		return sccp_l_w_error(q, mp);
	case M_HANGUP:
		return sccp_l_w_hangup(q, mp);
	case M_FLUSH:
		return tali_w_flush(q, mp);
	}
}

/*
 *  SCCP Provider --> TALI Primitives
 *  -----------------------------------
 *  This is the SCCP lower read routines.  This takes SCCP primitives and
 *  converts them to TALI M_DATA and M_CTL messages, if required.  If the user
 *  is a local IP, then the primitives are forwarded as is.
 */
static int sccp_l_info_ack(queue_t * q, mblk_t ** mpp)
{
}
static int sccp_l_bind_ack(queue_t * q, mblk_t ** mpp)
{
}
static int sccp_l_error_ack(queue_t * q, mblk_t ** mpp)
{
}
static int sccp_l_ok_ack(queue_t * q, mblk_t ** mpp)
{
}
static int sccp_l_unitdata_ind(queue_t * q, mblk_t ** mpp)
{
}
static int sccp_l_uderror_ind(queue_t * q, mblk_t ** mpp)
{
}
static int sccp_l_r_proto(queue_t * q, mblk_t * mp)
{
	switch (*((long *) mp->b_rptr)) {
	case N_INFO_ACK:
		return sccp_l_info_ack(q, mp);
	case N_BIND_ACK:
		return sccp_l_bind_ack(q, mp);
	case N_ERROR_ACK:
		return sccp_l_error_ack(q, mp);
	case N_OK_ACK:
		return sccp_l_ok_ack(q, mp);
	case N_UNITDATA_IND:
		return sccp_l_unitdata_ind(q, mp);
	case N_UDERROR_IND:
		return sccp_l_uderror_ind(q, mp);
	}
	return (-ENOTSUPP);
}
static int sccp_l_r_pcproto(queue_t * q, mblk_t * mp)
{
	return sccp_l_r_proto(q, mp);
}
static int sccp_l_r_error(queue_t * q, mblk_t * mp)
{
}
static int sccp_l_r_hangup(queue_t * q, mblk_t * mp)
{
}
static int sccp_l_r_prim(queue_t * q, mblk_t * mp)
{
	switch (mp->b_datap->db_type) {
	case M_PROTO:
		return sccp_l_r_proto(q, mp);
	case M_PCPROTO:
		return sccp_l_r_pcproto(q, mp);
	case M_ERROR:
		return sccp_l_r_error(q, mp);
	case M_HANGUP:
		return sccp_l_r_hangup(q, mp);
	case M_FLUSH:
		return tali_r_flush(q, mp);
	}
	return (-EOPNOTSUPP);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  TCP (SCTP) TLI Transport Message Handling
 *
 *  -------------------------------------------------------------------------
 *
 *  TALI --> TCP (SCTP) TLI Transport Primitives
 *  -------------------------------------------------------------------------
 */
static int tali_x_w_data(queue_t * q, mblk_t * mp)
{
}
static int tali_x_w_ctl(queue_t * q, mblk_t * mp)
{
}
static int tali_x_w_error(queue_t * q, mblk_t * mp)
{
}
static int tali_x_w_hangup(queue_t * q, mblk_t * mp)
{
}
static int tali_x_w_prim(queue_t * q, mblk_t * mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return tali_x_w_data(q, mp);
	case M_CTL:
		return tali_x_w_ctl(q, mp);
	case M_ERROR:
		return tali_x_w_error(q, mp);
	case M_HANGUP:
		return tali_x_w_hangup(q, mp);
	case M_FLUSH:
		return tali_w_flush(q, mp);
	}
	return (-EOPNOTSUPP);
}

/*
 *  TCP (SCTP) TLI Transport -- SCCP Primitives
 *  -------------------------------------------------------------------------
 */
