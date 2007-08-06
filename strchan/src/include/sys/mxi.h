/*****************************************************************************

 @(#) $Id: mxi.h,v 0.9.2.6 2007/08/06 04:44:04 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
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

 Last Modified $Date: 2007/08/06 04:44:04 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: mxi.h,v $
 Revision 0.9.2.6  2007/08/06 04:44:04  brian
 - rework of pipe-based emulation modules

 Revision 0.9.2.5  2007/07/14 01:35:35  brian
 - make license explicit, add documentation

 Revision 0.9.2.4  2006/12/06 11:26:11  brian
 - current development updates

 Revision 0.9.2.3  2006/11/30 13:05:26  brian
 - checking in working copies

 Revision 0.9.2.2  2006/11/27 11:41:58  brian
 - updated CH and MX headers to interface version 1.1

 Revision 0.9.2.1  2006/10/14 06:37:28  brian
 - added manpages, module, drivers, headers from strss7 package

 *****************************************************************************/

#ifndef __SYS_MXI_H__
#define __SYS_MXI_H__

#ident "@(#) $RCSfile: mxi.h,v $ $Name:  $($Revision: 0.9.2.6 $) Copyright (c) 2001-2006 OpenSS7 Corporation"

/* This file can be processed by doxygen(1). */

typedef int32_t mx_long;
typedef uint32_t mx_ulong;
typedef uint16_t mx_ushort;
typedef uint8_t mx_uchar;

#define MX_INFO_REQ		 1U
#define MX_OPTMGMT_REQ		 2U
#define MX_ATTACH_REQ		 3U
#define MX_ENABLE_REQ		 4U
#define MX_CONNECT_REQ		 5U
#define MX_DATA_REQ		 6U
#define MX_DISCONNECT_REQ	 7U
#define MX_DISABLE_REQ		 8U
#define MX_DETACH_REQ		 9U

#define MX_INFO_ACK		10U
#define MX_OPTMGMT_ACK		11U
#define MX_OK_ACK		12U
#define MX_ERROR_ACK		13U
#define MX_ENABLE_CON		14U
#define MX_CONNECT_CON		15U
#define MX_DATA_IND		16U
#define MX_DISCONNECT_IND	17U
#define MX_DISCONNECT_CON	18U
#define MX_DISABLE_IND		19U
#define MX_DISABLE_CON		20U
#define MX_EVENT_IND		21U

/*
 *  MX STATES
 */
#define MXS_UNINIT		-2U
#define MXS_UNUSABLE		-1U
#define MXS_DETACHED		 0U
#define MXS_WACK_AREQ		 1U
#define MXS_WACK_UREQ		 2U
#define MXS_ATTACHED		 3U
#define MXS_WACK_EREQ		 4U
#define MXS_WCON_EREQ		 5U
#define MXS_WACK_RREQ		 6U
#define MXS_WCON_RREQ		 7U
#define MXS_ENABLED		 8U
#define MXS_WACK_CREQ		 9U
#define MXS_WCON_CREQ		10U
#define MXS_WACK_DREQ		11U
#define MXS_WCON_DREQ		12U
#define MXS_CONNECTED		13U

/*
 *  MX STATE FLAGS
 */
#define MXSF_UNINIT		(1<<(2+MXS_UNINIT))
#define MXSF_UNUSABLE		(1<<(2+MXS_UNUSABLE))
#define MXSF_DETACHED		(1<<(2+MXS_DETACHED))
#define MXSF_WACK_AREQ		(1<<(2+MXS_WACK_AREQ))
#define MXSF_WACK_UREQ		(1<<(2+MXS_WACK_UREQ))
#define MXSF_ATTACHED		(1<<(2+MXS_ATTACHED))
#define MXSF_WACK_EREQ		(1<<(2+MXS_WACK_EREQ))
#define MXSF_WCON_EREQ		(1<<(2+MXS_WCON_EREQ))
#define MXSF_WACK_RREQ		(1<<(2+MXS_WACK_RREQ))
#define MXSF_WCON_RREQ		(1<<(2+MXS_WCON_RREQ))
#define MXSF_ENABLED		(1<<(2+MXS_ENABLED))
#define MXSF_WACK_CREQ		(1<<(2+MXS_WACK_CREQ)
#define MXSF_WCON_CREQ		(1<<(2+MXS_WCON_CREQ))
#define MXSF_WACK_DREQ		(1<<(2+MXS_WACK_DREQ))
#define MXSF_WCON_DREQ		(1<<(2+MXS_WCON_DREQ))
#define MXSF_CONNECTED		(1<<(2+MXS_CONNECTED))

/*
 *  MX PROTOCOL PRIMITIVES
 */

/*
 *  MX_INFO_REQ
 *  -------------------------------------------------------------------------
 */
typedef struct MX_info_req {
	mx_ulong mx_primitive;		/* always MX_INFO_REQ */
} MX_info_req_t;

/*
 *  MX_INFO_ACK
 *  -------------------------------------------------------------------------
 *  Indicates to the multiplex user requested information concerning the
 *  multiplex provider and the attached multiplex (if any).
 */
typedef struct MX_info_ack {
	mx_ulong mx_primitive;		/* always MX_INFO_ACK */
	mx_ulong mx_addr_length;	/* multiplex address length */
	mx_ulong mx_addr_offset;	/* multiplex address offset */
	mx_ulong mx_parm_length;	/* multiplex paramters length */
	mx_ulong mx_parm_offset;	/* multiplex paramters offset */
	mx_ulong mx_prov_flags;		/* provider options flags */
	mx_ulong mx_prov_class;		/* provider class */
	mx_ulong mx_style;		/* provider style */
	mx_ulong mx_version;		/* multiplex interface version */
	mx_ulong mx_state;		/* multiplex state */
} MX_info_ack_t;

#define MX_CIRCUIT	0x01	/* circuit provider class */

#define MX_STYLE1	0x0	/* does not perform attach */
#define MX_STYLE2	0x1	/* does perform attach */

#define MX_VERSION_1_0	0x10	/* version 1.0 of interface */
#define MX_VERSION_1_1	0x11	/* version 1.1 of interface */
#define MX_VERSION	MX_VERSION_1_1

#define MX_PARMS_CIRCUIT	0x01	/* parms structure type */
typedef struct MX_parms_circuit {
	mx_ulong mp_type;		/* always MX_PARMS_CIRCUIT */
	mx_ulong mp_encoding;		/* encoding */
	mx_ulong mp_block_size;		/* data block size (bits) */
	mx_ulong mp_samples;		/* samples per block */
	mx_ulong mp_sample_size;	/* sample size (bits) */
	mx_ulong mp_rate;		/* channel clock rate (samples/second) */
	mx_ulong mp_tx_channels;	/* number of tx channels */
	mx_ulong mp_rx_channels;	/* number of rx channels */
	mx_ulong mp_opt_flags;		/* options flags */
} MX_parms_circuit_t;

union MX_parms {
	mx_ulong mp_type;		/* structure type */
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
#define MX_RATE_184000		184000	/* 23B */
#define MX_RATE_192000		192000	/* T1 */
#define MX_RATE_240000		240000	/* 30B */
#define MX_RATE_248000		248000	/* E1 */
#define MX_RATE_5376000		5376000	/* T3 */

/*
 *  MX_OPTMGMT_REQ
 *  -------------------------------------------------------------------------
 */
typedef struct MX_optmgmt_req {
	mx_ulong mx_primitive;		/* always MX_OPTMGMT_REQ */
	mx_ulong mx_opt_length;		/* length of options */
	mx_ulong mx_opt_offset;		/* offset of options */
	mx_ulong mx_mgmt_flags;		/* option flags */
} MX_optmgmt_req_t;

/*
 *  MX_OPTMGMT_ACK
 *  -------------------------------------------------------------------------
 */
typedef struct MX_optmgmt_ack {
	mx_ulong mx_primitive;		/* always MX_OPTMGMT_REQ */
	mx_ulong mx_opt_length;		/* length of options */
	mx_ulong mx_opt_offset;		/* offset of options */
	mx_ulong mx_mgmt_flags;		/* option flags */
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
	mx_ulong mx_primitive;		/* always MX_ATTACH_REQ */
	mx_ulong mx_addr_length;	/* length of multiplex address */
	mx_ulong mx_addr_offset;	/* offset of multiplex address */
	mx_ulong mx_flags;		/* options flags */
} MX_attach_req_t;

/*
 *  MX_DETACH_REQ
 *  -------------------------------------------------------------------------
 */
typedef struct MX_detach_req {
	mx_ulong mx_primitive;		/* always MX_DETACH_REQ */
} MX_detach_req_t;

/*
 *  MX_OK_ACK
 *  -------------------------------------------------------------------------
 */
typedef struct MX_ok_ack {
	mx_ulong mx_primitive;		/* always MX_OK_ACK */
	mx_ulong mx_correct_prim;	/* correct primitive */
	mx_ulong mx_state;		/* resulting state */
} MX_ok_ack_t;

/*
 *  MX_ERROR_ACK
 *  -------------------------------------------------------------------------
 */
typedef struct MX_error_ack {
	mx_ulong mx_primitive;		/* always MX_ERROR_ACK */
	mx_ulong mx_error_primitive;	/* primitive in error */
	mx_ulong mx_error_type;		/* MXI error */
	mx_ulong mx_unix_error;		/* UNIX error */
	mx_ulong mx_state;		/* resulting state */
} MX_error_ack_t;

/*
   error types 
 */
#define MXSYSERR	 0	/* UNIX system error */
#define MXBADADDR	 1	/* Bad address format or content */
#define MXOUTSTATE	 2	/* Interface out of state */
#define MXBADOPT	 3	/* Bad options format or content */
#define MXBADPARM	 4	/* Bad parameter format or content */
#define MXBADPARMTYPE	 5	/* Bad paramater structure type */
#define MXBADFLAG	 6	/* Bad flag */
#define MXBADPRIM	 7	/* Bad primitive */
#define MXNOTSUPP	 8	/* Primitive not supported */
#define MXBADSLOT	 9	/* Bad multplex slot */

/*
 *  MX_ENABLE_REQ
 *  -------------------------------------------------------------------------
 */
typedef struct MX_enable_req {
	mx_ulong mx_primitive;		/* always MX_ENABLE_REQ */
} MX_enable_req_t;

/*
 *  MX_ENABLE_CON
 *  -------------------------------------------------------------------------
 */
typedef struct MX_enable_con {
	mx_ulong mx_primitive;		/* always MX_ENABLE_CON */
} MX_enable_con_t;

/*
 *  MX_DISABLE_REQ
 *  -------------------------------------------------------------------------
 */
typedef struct MX_disable_req {
	mx_ulong mx_primitive;		/* always MX_DISABLE_REQ */
} MX_disable_req_t;

/*
 *  MX_DISABLE_IND
 *  -------------------------------------------------------------------------
 */
typedef struct MX_disable_ind {
	mx_ulong mx_primitive;		/* always MX_DISABLE_IND */
	mx_ulong mx_cause;		/* cause for disable */
} MX_disable_ind_t;

/*
 *  MX_DISABLE_CON
 *  -------------------------------------------------------------------------
 */
typedef struct MX_disable_con {
	mx_ulong mx_primitive;		/* always MX_DISABLE_CON */
} MX_disable_con_t;

/*
 *  MX_DATA_REQ
 *  -------------------------------------------------------------------------
 */
typedef struct MX_data_req {
	mx_ulong mx_primitive;		/* always MX_DATA_REQ */
	mx_ulong mx_slot;		/* slot within multiplex */
} MX_data_req_t;

/*
 *  MX_DATA_IND
 *  -------------------------------------------------------------------------
 */
typedef struct MX_data_ind {
	mx_ulong mx_primitive;		/* always MX_DATA_IND */
	mx_ulong mx_slot;		/* slot within multiplex */
} MX_data_ind_t;

/*
 *  MX_CONNECT_REQ
 *  -------------------------------------------------------------------------
 */
typedef struct MX_connect_req {
	mx_ulong mx_primitive;		/* always MX_CONNECT_REQ */
	mx_ulong mx_conn_flags;		/* direction to connect */
	mx_ulong mx_slot;		/* slot within multiplex */
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
	mx_ulong mx_primitive;		/* always MX_CONNECT_CON */
	mx_ulong mx_conn_flags;		/* direction connected */
	mx_ulong mx_slot;		/* slot within multiplex */
} MX_connect_con_t;

/*
 *  MX_DISCONNECT_REQ
 *  -------------------------------------------------------------------------
 */
typedef struct MX_disconnect_req {
	mx_ulong mx_primitive;		/* always MX_DISCONNECT_REQ */
	mx_ulong mx_conn_flags;		/* direction to disconnect */
	mx_ulong mx_slot;		/* slot within multiplex */
} MX_disconnect_req_t;

/*
 *  MX_DISCONNECT_IND
 *  -------------------------------------------------------------------------
 */
typedef struct MX_disconnect_ind {
	mx_ulong mx_primitive;		/* always MX_DISCONNECT_IND */
	mx_ulong mx_conn_flags;		/* direction disconnected */
	mx_ulong mx_cause;		/* cause for disconnection */
	mx_ulong mx_slot;		/* slot within multiplex */
} MX_disconnect_ind_t;

/*
 *  MX_DISCONNECT_CON
 *  -------------------------------------------------------------------------
 */
typedef struct MX_disconnect_con {
	mx_ulong mx_primitive;		/* always MX_DISCONNECT_CON */
	mx_ulong mx_conn_flags;		/* direction disconnected */
	mx_ulong mx_slot;		/* slot within multiplex */
} MX_disconnect_con_t;

/*
 *  MX_EVENT_IND
 *  -------------------------------------------------------------------------
 */
typedef struct MX_event_ind {
	mx_ulong mx_primitive;		/* always MX_EVENT_IND */
	mx_ulong mx_event;		/* event */
	mx_ulong mx_slot;		/* slot within multiplex for event */
} MX_event_ind_t;

#define MX_EVT_DCD_ASSERT		 0
#define MX_EVT_DCD_DEASSERT		 1
#define MX_EVT_DSR_ASSERT		 2
#define MX_EVT_DSR_DEASSERT		 3
#define MX_EVT_DTR_ASSERT		 4
#define MX_EVT_DTR_DEASSERT		 5
#define MX_EVT_RTS_ASSERT		 6
#define MX_EVT_RTS_DEASSERT		 7
#define MX_EVT_CTS_ASSERT		 8
#define MX_EVT_CTS_DEASSERT		 9
#define MX_EVT_RI_ASSERT		10
#define MX_EVT_RI_DEASSERT		11
#define MX_EVT_YEL_ALARM		12
#define MX_EVT_BLU_ALARM		13
#define MX_EVT_RED_ALARM		14
#define MX_EVT_NO_ALARM			15

#define MXF_EVT_DCD_ASSERT		(1 <<  0)
#define MXF_EVT_DCD_DEASSERT		(1 <<  1)
#define MXF_EVT_DSR_ASSERT		(1 <<  2)
#define MXF_EVT_DSR_DEASSERT		(1 <<  3)
#define MXF_EVT_DTR_ASSERT		(1 <<  4)
#define MXF_EVT_DTR_DEASSERT		(1 <<  5)
#define MXF_EVT_RTS_ASSERT		(1 <<  6)
#define MXF_EVT_RTS_DEASSERT		(1 <<  7)
#define MXF_EVT_CTS_ASSERT		(1 <<  8)
#define MXF_EVT_CTS_DEASSERT		(1 <<  9)
#define MXF_EVT_RI_ASSERT		(1 << 10)
#define MXF_EVT_RI_DEASSERT		(1 << 11)
#define MXF_EVT_YEL_ALARM		(1 << 12)
#define MXF_EVT_BLU_ALARM		(1 << 13)
#define MXF_EVT_RED_ALARM		(1 << 14)
#define MXF_EVT_NO_ALARM		(1 << 15)

#define MXF_EVT_DCD_CHANGE		(MXF_EVT_DCD_ASSERT|MXF_EVT_DCD_DEASSERT)
#define MXF_EVT_DSR_CHANGE		(MXF_EVT_DSR_ASSERT|MXF_EVT_DSR_DEASSERT)
#define MXF_EVT_DTR_CHANGE		(MXF_EVT_DTR_ASSERT|MXF_EVT_DTR_DEASSERT)
#define MXF_EVT_RTS_CHANGE		(MXF_EVT_RTS_ASSERT|MXF_EVT_RTS_DEASSERT)
#define MXF_EVT_CTS_CHANGE		(MXF_EVT_CTS_ASSERT|MXF_EVT_CTS_DEASSERT)
#define MXF_EVT_RI_CHANGE		(MXF_EVT_RI_ASSERT|MXF_EVT_RI_DEASSERT)

#endif				/* __SYS_MXI_H__ */
