/*****************************************************************************

 @(#) $Id$

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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ifndef __SYS_RSW_H__
#define __SYS_RSW_H__

/* This file can be processed by doxygen(1). */

#include <sys/npi.h>

/** @file
  * This file provides a header file for the RTP Switch (RSW) driver.  The RSW
  * driver is responsible for connection and cross-connection of symmetric
  * bidirectional RTP media connections.  The purpose is to multiplex a number
  * of RTP connection legs onto a limited set of Streams.
  */

#define rs_primitive		rs_prim.PRIM_type
#define rs_addr_length		rs_prim.ADDR_length
#define rs_addr_offset		rs_prim.ADDR_offset
#define rs_addr_size		rs_prim.ADDR_size
#define rs_bind_flags		rs_prim.BIND_flags
#define rs_cdata_size		rs_prim.CDATA_size
#define rs_conind_number	rs_prim.CONIND_number
#define rs_conn_flags		rs_prim.CONN_flags
#define rs_correct_prim		rs_prim.CORRECT_prim
#define rs_current_state	rs_prim.CURRENT_state
#define rs_data_xfer_flags	rs_prim.DATA_xfer_flags
#define rs_ddata_size		rs_prim.DDATA_size
#define rs_dest_length		rs_prim.DEST_length
#define rs_dest_offset		rs_prim.DEST_offset
#define rs_discon_reason	rs_prim.DISCON_reason
#define rs_ensdu_size		rs_prim.ENSDU_size
#define rs_error_prim		rs_prim.ERROR_prim
#define rs_error_type		rs_prim.ERROR_type
#define rs_nidu_size		rs_prim.NIDU_size
#define rs_nodu_size		rs_prim.NODU_size
#define rs_npi_error		rs_prim.NPI_error
#define rs_npi_version		rs_prim.NPI_version
#define rs_nsdu_size		rs_prim.NSDU_size
#define rs_options_flags	rs_prim.OPTIONS_flags
#define rs_optmgmt_flags	rs_prim.OPTMGMT_flags
#define rs_protoid_length	rs_prim.PROTOID_length
#define rs_protoid_offset	rs_prim.PROTOID_offset
#define rs_provider_type	rs_prim.PROVIDER_type
#define rs_qos_length		rs_prim.QOS_length
#define rs_qos_offset		rs_prim.QOS_offset
#define rs_qos_range_length	rs_prim.QOS_range_length
#define rs_qos_range_offset	rs_prim.QOS_range_offset
#define rs_reserved_field	rs_prim.RESERVED_field
#define rs_reset_orig		rs_prim.RESET_orig
#define rs_reset_reason		rs_prim.RESET_reason
#define rs_res_length		rs_prim.RES_length
#define rs_res_offset		rs_prim.RES_offset
#define rs_seq_number		rs_prim.SEQ_number
#define rs_serv_type		rs_prim.SERV_type
#define rs_src_length		rs_prim.SRC_length
#define rs_src_offset		rs_prim.SRC_offset
#define rs_token_value		rs_prim.TOKEN_value
#define rs_unix_error		rs_prim.UNIX_error

/*
 *  RS_OPTMGMT_REQ (M_PROTO, M_PCPROTO) - options management
 *  -------------------------------------------------------------------------
 */
typedef struct RS_optmgmt_req {
	np_ulong rs_tpid;
	N_optmgmt_req_t rs_prim;

	np_ulong rs_primitive;		/* always RS_OPTMGMT_REQ */
	np_ulong rs_tpid;		/* termination point id */
	np_ulong rs_opt_length;		/* length of options */
	np_ulong rs_opt_offset;		/* offset of options */
	np_ulong rs_optmgmt_flags;	/* options management flags */
} RS_optmgmt_req_t;

/*
 *  RS_OPTMGMT_ACK (M_PROTO, M_PCPROTO) - options management acknowledgement
 *  -------------------------------------------------------------------------
 */
typedef struct RS_optmgmt_ack {
	np_ulong rs_primitive;		/* always RS_OPTMGMT_ACK */
	np_ulong rs_tpid;		/* termination point id */
	np_ulong rs_opt_length;		/* length of returned options */
	np_ulong rs_opt_offset;		/* offset of returned options */
	np_ulong rs_optmgmt_flags;	/* options management flags */
} RS_optmgmt_ack_t;

/*
 *  RS_OK_ACK (M_PCPROTO) - positive acknowledgement
 *  -------------------------------------------------------------------------
 */
typedef struct RS_ok_ack {
	np_ulong rs_primitive;		/* always RS_OK_ACK */
	np_ulong rs_tpid;		/* termination point id */
	np_ulong rs_correct_prim;	/* correct primitive */
	np_ulong rs_state;		/* resulting state */
} RS_ok_ack_t;

/*
 *  RS_ERROR_ACK (M_PCPROTO) - error acknowledgement
 *  -------------------------------------------------------------------------
 */
typedef struct RS_error_ack {
	np_ulong rs_primitive;		/* always RS_ERROR_ACK */
	np_ulong rs_tpid;		/* termination point id */
	np_ulong rs_error_prim;		/* primitive in error */
	np_ulong rs_error;		/* error number */
	np_ulong rs_unix_error;		/* UNIX error code */
	np_ulong rs_state;		/* resulting state */
} RS_error_ack_t;

/*
 *  RS_BIND_REQ (M_PROTO) - bind a termination point
 *  -------------------------------------------------------------------------
 *  Binds an address to a new termination point.
 */
typedef struct RS_bind_req {
	np_ulong rs_primitive;		/* always RS_BIND_REQ */
	np_ulong rs_tpid;		/* termination point id */
	np_ulong rs_addr_length;	/* length of address */
	np_ulong rs_addr_offset;	/* offset of address */
	np_ulong rs_bind_flags;		/* bind flags */
} RS_bind_req_t;

/*
 * RS_BIND_ACK (M_PCPROTO) - acknowledge bind of a termination point
 * --------------------------------------------------------------------------
 */
typedef struct RS_bind_ack {
	np_ulong rs_primitive;		/* always RS_BIND_ACK */
	np_ulong rs_tpid;		/* termination point id */
	np_ulong rs_addr_length;	/* length of address */
	np_ulong rs_addr_offset;	/* offset of address */
	np_ulong rs_bind_flags;		/* # of media streams */
} RS_bind_ackt_t;

/*
 *  RS_CONN_REQ (M_PROTO) - connect a termination point
 *  -------------------------------------------------------------------------
 *  Connect a termination point to a remote address or addresses.
 */
typedef struct RS_conn_req {
	np_ulong rs_primitive;		/* always RS_CONN_REQ */
	np_ulong rs_tpid;		/* termination point id */
	np_ulong rs_dest_length;	/* destination address length */
	np_ulong rs_dest_offset;	/* destination adresss offset */
	np_ulong rs_conn_flags;		/* connection flags */
	np_ulong rs_opt_length;		/* length of options */
	np_ulong rs_opt_length;		/* offset of options */
} RS_conn_req_t;

/*
 *  RS_CONN_CON (M_PROTO) - connection confirmation
 *  -------------------------------------------------------------------------
 */
typedef struct RS_conn_con {
	np_ulong rs_primitive;		/* always RS_CONN_CON */
	np_ulong rs_tpid;		/* termination point id */
	np_ulong rs_res_length;		/* destination address length */
	np_ulong rs_res_offset;		/* destination address offset */
	np_ulong rs_conn_flags;		/* connection flags */
	np_ulong rs_opt_length;		/* length of options */
	np_ulong rs_opt_length;		/* offset of options */
} RS_conn_con_t;

/*
 *  RS_DATA_REQ (M_PROTO) - data transfer request
 *  -------------------------------------------------------------------------
 */
typedef struct RS_data_req {
	np_ulong rs_primitive;		/* always RS_DATA_REQ */
	np_ulong rs_tpid;		/* termination point id */
	np_ulong rs_data_flags;		/* data transfer flags */
} RS_data_req_t;

/*
 *  RS_DATA_IND (M_PROTO) - data transfer indication
 *  -------------------------------------------------------------------------
 */
typedef struct RS_data_ind {
	np_ulong rs_primitive;		/* always RS_DATA_IND */
	np_ulong rs_tpid;		/* termination point id */
	np_ulong rs_data_flags;		/* data transfer flags */
} RS_data_ind_t;

/*
 *  RS_UDERR_IND (M_PROTO) - unitdata error indication
 *  -------------------------------------------------------------------------
 */
typedef struct RS_uderr_ind {
	np_ulong rs_primitive;		/* always RS_UDERR_IND */
	np_ulong rs_tpid;		/* termination point id */
	np_ulong rs_dest_length;	/* destination address length */
	np_ulong rs_dest_offset;	/* destination address offset */
	np_ulong rs_error_type;		/* error */
} RS_uderr_ind_t;

/*
 *  RS_UNBIND_REQ (M_PROTO) - unbind a termination point
 *  -------------------------------------------------------------------------
 *  Unbinds a termination point.
 */
typedef struct RS_unbind_req {
	np_ulong rs_primitive;		/* always RS_UNBIND_REQ */
	np_ulong rs_tpid;		/* termination point id */
} RS_unbind_req_t;

/*
 * RS_DISCON_REQ - connect two termination points
 * --------------------------------------------------------------------------
 */

#endif				/* __SYS_RSW_H__ */
