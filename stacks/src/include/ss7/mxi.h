/*****************************************************************************

 @(#) $Id: mxi.h,v 0.9.2.3 2006/09/18 13:52:33 brian Exp $

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

#ifndef __SS7_MXI_H__
#define __SS7_MXI_H__

#ident "@(#) $RCSfile: mxi.h,v $ $Name:  $($Revision: 0.9.2.3 $) Copyright (c) 2001-2004  OpenSS7 Corporation"

/* This file can be processed by doxygen(1). */

#define MX_INFO_REQ		 1UL
#define MX_OPTMGMT_REQ		 2UL
#define MX_ATTACH_REQ		 3UL
#define MX_ENABLE_REQ		 4UL
#define MX_CONNECT_REQ		 5UL
#define MX_DATA_REQ		 6UL
#define MX_DISCONNECT_REQ	 7UL
#define MX_DISABLE_REQ		 8UL
#define MX_DETACH_REQ		 9UL

#define MX_INFO_ACK		10UL
#define MX_OPTMGMT_ACK		11UL
#define MX_OK_ACK		12UL
#define MX_ERROR_ACK		13UL
#define MX_ENABLE_CON		14UL
#define MX_CONNECT_CON		15UL
#define MX_DATA_IND		16UL
#define MX_DISCONNECT_IND	17UL
#define MX_DISCONNECT_CON	18UL
#define MX_DISABLE_IND		19UL
#define MX_DISABLE_CON		20UL

/*
 *  MX STATES
 */
#define MXS_UNINIT		-2UL
#define MXS_UNUSABLE		-1UL
#define MXS_DETACHED		 0UL
#define MXS_WACK_AREQ		 1UL
#define MXS_WACK_UREQ		 2UL
#define MXS_ATTACHED		 3UL
#define MXS_WACK_EREQ		 4UL
#define MXS_WCON_EREQ		 5UL
#define MXS_WACK_RREQ		 6UL
#define MXS_WCON_RREQ		 7UL
#define MXS_ENABLED		 8UL
#define MXS_WACK_CREQ		 9UL
#define MXS_WCON_CREQ		10UL
#define MXS_WACK_DREQ		11UL
#define MXS_WCON_DREQ		12UL
#define MXS_CONNECTED		13UL

/*
 *  MX STATE FLAGS
 */
#define MXSF_UNINIT		(1<<MXS_UNINIT)
#define MXSF_UNUSABLE		(1<<MXS_UNUSABLE)
#define MXSF_DETACHED		(1<<MXS_DETACHED)
#define MXSF_WACK_AREQ		(1<<MXS_WACK_AREQ)
#define MXSF_WACK_UREQ		(1<<MXS_WACK_UREQ)
#define MXSF_ATTACHED		(1<<MXS_ATTACHED)
#define MXSF_WACK_EREQ		(1<<MXS_WACK_EREQ)
#define MXSF_WCON_EREQ		(1<<MXS_WCON_EREQ)
#define MXSF_WACK_RREQ		(1<<MXS_WACK_RREQ)
#define MXSF_WCON_RREQ		(1<<MXS_WCON_RREQ)
#define MXSF_ENABLED		(1<<MXS_ENABLED)
#define MXSF_WACK_CREQ		(1<<MXS_WACK_CREQ)
#define MXSF_WCON_CREQ		(1<<MXS_WCON_CREQ)
#define MXSF_WACK_DREQ		(1<<MXS_WACK_DREQ)
#define MXSF_WCON_DREQ		(1<<MXS_WCON_DREQ)
#define MXSF_CONNECTED		(1<<MXS_CONNECTED)

/*
 *  MX PROTOCOL PRIMITIVES
 */

/*
 *  MX_INFO_REQ
 *  -------------------------------------------------------------------------
 */
typedef struct MX_info_req {
	ulong mx_primitive;		/* always MX_INFO_REQ */
} MX_info_req_t;

/*
 *  MX_INFO_ACK
 *  -------------------------------------------------------------------------
 *  Indicates to the multiplex user requested information concerning the
 *  multiplex provider and the attached multiplex (if any).
 */
typedef struct MX_info_ack {
	ulong mx_primitive;		/* always MX_INFO_ACK */
	ulong mx_addr_length;		/* channel address length */
	ulong mx_addr_offset;		/* channel address offset */
	ulong mx_parm_length;		/* channel paramters length */
	ulong mx_parm_offset;		/* channel paramters offset */
	ulong mx_prov_flags;		/* provider options flags */
	ulong mx_style;			/* provider style */
	ulong mx_version;		/* channel interface version */
} MX_info_ack_t;

#define MX_STYLE1	0x0	/* does not perform attach */
#define MX_STYLE2	0x1	/* does perform attach */

#define MX_VERSION_1_0	0x10	/* version 1.0 of interface */
#define MX_VERSION	MX_VERSION_1_0

#define MX_PARMS_CIRCUIT	0x01	/* parms structure type */
typedef struct MX_parms_circuit {
	ulong cp_type;			/* always MX_PARMS_CIRCUIT */
	ulong cp_encoding;		/* encoding */
	ulong cp_block_size;		/* data block size (bits) */
	ulong cp_samples;		/* samples per block */
	ulong cp_sample_size;		/* sample size (bits) */
	ulong cp_rate;			/* clock rate (samples/second) */
	ulong cp_tx_channels;		/* number of tx channels */
	ulong cp_rx_channels;		/* number of rx channels */
	ulong cp_opt_flags;		/* options flags */
} MX_parms_circuit_t;

union MX_parms {
	ulong cp_type;			/* structure type */
	MX_parms_circuit_t circuit;	/* circuit structure */
};

#define MX_PARM_OPT_CLRCH	0x01	/* supports clear channel */

#define MX_ENCODING_NONE	 0
#define MX_ENCODING_CN		 1
#define MX_ENCODING_DVI4	 2
#define MX_ENCODING_FS1015	 3
#define MX_ENCODING_FS1016	 4
#define MX_ENCODING_G711_PCM_A	 5
#define MX_ENCODING_G711_PCM_L	 6
#define MX_ENCODING_G711_PCM_U	 7
#define MX_ENCODING_G721	 8
#define MX_ENCODING_G722	 9
#define MX_ENCODING_G723	10
#define MX_ENCODING_G726	11
#define MX_ENCODING_G728	12
#define MX_ENCODING_G729	13
#define MX_ENCODING_GSM		14
#define MX_ENCODING_GSM_EFR	15
#define MX_ENCODING_GSM_HR	16
#define MX_ENCODING_LPC		17
#define MX_ENCODING_MPA		18
#define MX_ENCODING_QCELP	19
#define MX_ENCODING_RED		20
#define MX_ENCODING_S16_BE	21
#define MX_ENCODING_S16_LE	22
#define MX_ENCODING_S8		23
#define MX_ENCODING_U16_BE	24
#define MX_ENCODING_U16_LE	25
#define MX_ENCODING_U8		26
#define MX_ENCODING_VDVI	27

#define MX_RATE_VARIABLE	0
#define MX_RATE_8000		8000
#define MX_RATE_11025		11025
#define MX_RATE_16000		16000
#define MX_RATE_22050		22050
#define MX_RATE_44100		44100
#define MX_RATE_90000		90000

/*
 *  MX_OPTMGMT_REQ
 *  -------------------------------------------------------------------------
 */
typedef struct MX_optmgmt_req {
	ulong mx_primitive;		/* always MX_OPTMGMT_REQ */
	ulong mx_opt_length;		/* length of options */
	ulong mx_opt_offset;		/* offset of options */
	ulong mx_mgmt_flags;		/* option flags */
} MX_optmgmt_req_t;

/*
 *  MX_OPTMGMT_ACK
 *  -------------------------------------------------------------------------
 */
typedef struct MX_optmgmt_ack {
	ulong mx_primitive;		/* always MX_OPTMGMT_REQ */
	ulong mx_opt_length;		/* length of options */
	ulong mx_opt_offset;		/* offset of options */
	ulong mx_mgmt_flags;		/* option flags */
} MX_optmgmt_ack_t;

/*
   management flags for MX_OPTMGMT 
 */
#define MX_SET_OPT	0x01
#define MX_GET_OPT	0x02
#define MX_NEGOTIATE	0x03
#define MX_DEFAULT	0x04

/*
 *  MX_ATTACH_REQ
 *  -------------------------------------------------------------------------
 */
typedef struct MX_attach_req {
	ulong mx_primitive;		/* always MX_ATTACH_REQ */
	ulong mx_addr_length;		/* length of channel address */
	ulong mx_addr_offset;		/* offset of channel address */
	ulong mx_flags;			/* options flags */
} MX_attach_req_t;

/*
 *  MX_DETACH_REQ
 *  -------------------------------------------------------------------------
 */
typedef struct MX_detach_req {
	ulong mx_primitive;		/* always MX_DETACH_REQ */
} MX_detach_req_t;

/*
 *  MX_OK_ACK
 *  -------------------------------------------------------------------------
 */
typedef struct MX_ok_ack {
	ulong mx_primitive;		/* always MX_OK_ACK */
	ulong mx_correct_prim;		/* correct primitive */
	ulong mx_state;			/* resulting state */
} MX_ok_ack_t;

/*
 *  MX_ERROR_ACK
 *  -------------------------------------------------------------------------
 */
typedef struct MX_error_ack {
	ulong mx_primitive;		/* always MX_ERROR_ACK */
	ulong mx_error_primitive;	/* primitive in error */
	ulong mx_error_type;		/* MXI error */
	ulong mx_unix_error;		/* UNIX error */
	ulong mx_state;			/* resulting state */
} MX_error_ack_t;

/*
   error types 
 */
#define MXSYSERR		 0	/* UNIX system error */
#define MXBADADDR		 1	/* Bad address format or content */
#define MXOUTSTATE		 2	/* Interface out of state */
#define MXBADOPT		 3	/* Bad options format or content */
#define MXBADPARM		 4	/* Bad parameter format or content */
#define MXBADPARMTYPE		 5	/* Bad paramater structure type */
#define MXBADFLAG		 6	/* Bad flag */
#define MXBADPRIM		 7	/* Bad primitive */
#define MXNOTSUPP		 8	/* Primitive not supported */
#define MXBADSLOT		 9	/* Bad multplex slot */

/*
 *  MX_ENABLE_REQ
 *  -------------------------------------------------------------------------
 */
typedef struct MX_enable_req {
	ulong mx_primitive;		/* always MX_ENABLE_REQ */
} MX_enable_req_t;

/*
 *  MX_ENABLE_CON
 *  -------------------------------------------------------------------------
 */
typedef struct MX_enable_con {
	ulong mx_primitive;		/* always MX_ENABLE_CON */
} MX_enable_con_t;

/*
 *  MX_DISABLE_REQ
 *  -------------------------------------------------------------------------
 */
typedef struct MX_disable_req {
	ulong mx_primitive;		/* always MX_DISABLE_REQ */
} MX_disable_req_t;

/*
 *  MX_DISABLE_IND
 *  -------------------------------------------------------------------------
 */
typedef struct MX_disable_ind {
	ulong mx_primitive;		/* always MX_DISABLE_IND */
	ulong mx_cause;			/* cause for disable */
} MX_disable_ind_t;

/*
 *  MX_DISABLE_CON
 *  -------------------------------------------------------------------------
 */
typedef struct MX_disable_con {
	ulong mx_primitive;		/* always MX_DISABLE_CON */
} MX_disable_con_t;

/*
 *  MX_DATA_REQ
 *  -------------------------------------------------------------------------
 */
typedef struct MX_data_req {
	ulong mx_primitive;		/* always MX_DATA_REQ */
	ulong mx_slot;			/* slot within channel */
} MX_data_req_t;

/*
 *  MX_DATA_IND
 *  -------------------------------------------------------------------------
 */
typedef struct MX_data_ind {
	ulong mx_primitive;		/* always MX_DATA_IND */
	ulong mx_slot;			/* slot within channel */
} MX_data_ind_t;

/*
 *  MX_CONNECT_REQ
 *  -------------------------------------------------------------------------
 */
typedef struct MX_connect_req {
	ulong mx_primitive;		/* always MX_CONNECT_REQ */
	ulong mx_conn_flags;		/* direction to connect */
	ulong mx_slot;			/* slot within channel */
} MX_connect_req_t;

/*
   connect flags 
 */
#define MXF_RX_DIR	0x01
#define MXF_TX_DIR	0x02
#define MXF_BOTH_DIR	(MXF_RX_DIR|MXF_TX_DIR)

/*
 *  MX_CONNECT_CON
 *  -------------------------------------------------------------------------
 */
typedef struct MX_connect_con {
	ulong mx_primitive;		/* always MX_CONNECT_CON */
	ulong mx_conn_flags;		/* direction connected */
	ulong mx_slot;			/* slot within channel */
} MX_connect_con_t;

/*
 *  MX_DISCONNECT_REQ
 *  -------------------------------------------------------------------------
 */
typedef struct MX_disconnect_req {
	ulong mx_primitive;		/* always MX_DISCONNECT_REQ */
	ulong mx_conn_flags;		/* direction to disconnect */
	ulong mx_slot;			/* slot within channel */
} MX_disconnect_req_t;

/*
 *  MX_DISCONNECT_IND
 *  -------------------------------------------------------------------------
 */
typedef struct MX_disconnect_ind {
	ulong mx_primitive;		/* always MX_DISCONNECT_IND */
	ulong mx_conn_flags;		/* direction disconnected */
	ulong mx_cause;			/* cause for disconnection */
	ulong mx_slot;			/* slot within channel */
} MX_disconnect_ind_t;

/*
 *  MX_DISCONNECT_CON
 *  -------------------------------------------------------------------------
 */
typedef struct MX_disconnect_con {
	ulong mx_primitive;		/* always MX_DISCONNECT_CON */
	ulong mx_conn_flags;		/* direction disconnected */
	ulong mx_slot;			/* slot within channel */
} MX_disconnect_con_t;

#endif				/* __SS7_MXI_H__ */
