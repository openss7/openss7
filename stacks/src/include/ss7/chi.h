/*****************************************************************************

 @(#) $Id: chi.h,v 0.9 2004/01/17 08:08:11 brian Exp $

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
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2004/01/17 08:08:11 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SS7_CHI_H__
#define __SS7_CHI_H__

#ident "@(#) $Name:  $($Revision: 0.9 $) Copyright (c) 1997-2002 OpenSS7 Corporation."

typedef long ch_long;
typedef ulong ch_ulong;
typedef ushort ch_ushort;
typedef uchar ch_uchar;

#define CH_INFO_REQ		 1UL
#define CH_OPTMGMT_REQ		 2UL
#define CH_ATTACH_REQ		 3UL
#define CH_ENABLE_REQ		 4UL
#define CH_CONNECT_REQ		 5UL
#define CH_DATA_REQ		 6UL
#define CH_DISCONNECT_REQ	 7UL
#define CH_DISABLE_REQ		 8UL
#define CH_DETACH_REQ		 9UL

#define CH_INFO_ACK		10UL
#define CH_OPTMGMT_ACK		11UL
#define CH_OK_ACK		12UL
#define CH_ERROR_ACK		13UL
#define CH_ENABLE_CON		14UL
#define CH_CONNECT_CON		15UL
#define CH_DATA_IND		16UL
#define CH_DISCONNECT_IND	17UL
#define CH_DISCONNECT_CON	18UL
#define CH_DISABLE_IND		19UL
#define CH_DISABLE_CON		20UL
#define CH_EVENT_IND		21UL

/*
 *  CH STATES
 */
#define CHS_UNINIT		-2UL
#define CHS_UNUSABLE		-1UL
#define CHS_DETACHED		 0UL
#define CHS_WACK_AREQ		 1UL
#define CHS_WACK_UREQ		 2UL
#define CHS_ATTACHED		 3UL
#define CHS_WACK_EREQ		 4UL
#define CHS_WCON_EREQ		 5UL
#define CHS_WACK_RREQ		 6UL
#define CHS_WCON_RREQ		 7UL
#define CHS_ENABLED		 8UL
#define CHS_WACK_CREQ		 9UL
#define CHS_WCON_CREQ		10UL
#define CHS_WACK_DREQ		11UL
#define CHS_WCON_DREQ		12UL
#define CHS_CONNECTED		13UL

/*
 *  CH PROTOCOL PRIMITIVES
 */

/*
 *  CH_INFO_REQ
 *  -------------------------------------------------------------------------
 */
typedef struct CH_info_req {
	ch_ulong ch_primitive;		/* always CH_INFO_REQ */
} CH_info_req_t;

/*
 *  CH_INFO_ACK
 *  -------------------------------------------------------------------------
 *  Indicates to the channel user requested information concerning the channel
 *  provider and the attached channel (if any).
 */
typedef struct CH_info_ack {
	ch_ulong ch_primitive;		/* always CH_INFO_ACK */
	ch_ulong ch_addr_length;	/* channel address length */
	ch_ulong ch_addr_offset;	/* channel address offset */
	ch_ulong ch_parm_length;	/* channel paramters length */
	ch_ulong ch_parm_offset;	/* channel paramters offset */
	ch_ulong ch_prov_flags;		/* provider options flags */
	ch_ulong ch_style;		/* provider style */
	ch_ulong ch_version;		/* channel interface version */
	ch_ulong ch_state;		/* channel state */
} CH_info_ack_t;

#define CH_STYLE1	0x0	/* does not perform attach */
#define CH_STYLE2	0x1	/* does perform attach */

#define CH_VERSION_1_0	0x10	/* version 1.0 of interface */
#define CH_VERSION	CH_VERSION_1_0

#define CH_PARMS_CIRCUIT	0x01	/* parms structure type */
typedef struct CH_parms_circuit {
	ch_ulong cp_type;		/* always CH_PARMS_CIRCUIT */
	ch_ulong cp_block_size;		/* data block size (bits) */
	ch_ulong cp_encoding;		/* encoding */
	ch_ulong cp_sample_size;	/* sample size (bits) */
	ch_ulong cp_rate;		/* clock rate (Hz) */
	ch_ulong cp_tx_channels;	/* number of tx channels */
	ch_ulong cp_rx_channels;	/* number of rx channels */
	ch_ulong cp_opt_flags;		/* options flags */
} CH_parms_circuit_t;

union CH_parms {
	ch_ulong cp_type;		/* structure type */
	CH_parms_circuit_t circuit;	/* circuit structure */
};

#define CH_PARM_OPT_CLRCH	0x01	/* supports clear channel */

#define CH_ENCODING_NONE	 0
#define CH_ENCODING_CN		 1
#define CH_ENCODING_DVI4	 2
#define CH_ENCODING_FS1015	 3
#define CH_ENCODING_FS1016	 4
#define CH_ENCODING_G711_PCM_A	 5
#define CH_ENCODING_G711_PCM_L	 6
#define CH_ENCODING_G711_PCM_U	 7
#define CH_ENCODING_G721	 8
#define CH_ENCODING_G722	 9
#define CH_ENCODING_G723	10
#define CH_ENCODING_G726	11
#define CH_ENCODING_G728	12
#define CH_ENCODING_G729	13
#define CH_ENCODING_GSM		14
#define CH_ENCODING_GSM_EFR	15
#define CH_ENCODING_GSM_HR	16
#define CH_ENCODING_LPC		17
#define CH_ENCODING_MPA		18
#define CH_ENCODING_QCELP	19
#define CH_ENCODING_RED		20
#define CH_ENCODING_S16_BE	21
#define CH_ENCODING_S16_LE	22
#define CH_ENCODING_S8		23
#define CH_ENCODING_U16_BE	24
#define CH_ENCODING_U16_LE	25
#define CH_ENCODING_U8		26
#define CH_ENCODING_VDVI	27

#define CH_RATE_VARIABLE	0
#define CH_RATE_8000		8000
#define CH_RATE_11025		11025
#define CH_RATE_16000		16000
#define CH_RATE_22050		22050
#define CH_RATE_44100		44100
#define CH_RATE_90000		90000
#define CH_RATE_184000		184000	/* 23B */
#define CH_RATE_192000		192000	/* T1 */
#define CH_RATE_240000		240000	/* 30B */
#define CH_RATE_248000		248000	/* E1 */

/*
 *  CH_OPTMGMT_REQ
 *  -------------------------------------------------------------------------
 */
typedef struct CH_optmgmt_req {
	ch_ulong ch_primitive;		/* always CH_OPTMGMT_REQ */
	ch_ulong ch_opt_length;		/* length of options */
	ch_ulong ch_opt_offset;		/* offset of options */
	ch_ulong ch_mgmt_flags;		/* option flags */
} CH_optmgmt_req_t;

/*
 *  CH_OPTMGMT_ACK
 *  -------------------------------------------------------------------------
 */
typedef struct CH_optmgmt_ack {
	ch_ulong ch_primitive;		/* always CH_OPTMGMT_REQ */
	ch_ulong ch_opt_length;		/* length of options */
	ch_ulong ch_opt_offset;		/* offset of options */
	ch_ulong ch_mgmt_flags;		/* option flags */
} CH_optmgmt_ack_t;

/*
   management flags for CH_OPTMGMT 
 */
#define CH_SET		0x01
#define CH_GET		0x02
#define CH_NEGOTIATE	0x03
#define CH_DEFAULT	0x04

/*
 *  CH_ATTACH_REQ
 *  -------------------------------------------------------------------------
 */
typedef struct CH_attach_req {
	ch_ulong ch_primitive;		/* always CH_ATTACH_REQ */
	ch_ulong ch_addr_length;	/* length of channel address */
	ch_ulong ch_addr_offset;	/* offset of channel address */
	ch_ulong ch_flags;		/* options flags */
} CH_attach_req_t;

/*
 *  CH_DETACH_REQ
 *  -------------------------------------------------------------------------
 */
typedef struct CH_detach_req {
	ch_ulong ch_primitive;		/* always CH_DETACH_REQ */
} CH_detach_req_t;

/*
 *  CH_OK_ACK
 *  -------------------------------------------------------------------------
 */
typedef struct CH_ok_ack {
	ch_ulong ch_primitive;		/* always CH_OK_ACK */
	ch_ulong ch_correct_prim;	/* correct primitive */
	ch_ulong ch_state;		/* resulting state */
} CH_ok_ack_t;

/*
 *  CH_ERROR_ACK
 *  -------------------------------------------------------------------------
 */
typedef struct CH_error_ack {
	ch_ulong ch_primitive;		/* always CH_ERROR_ACK */
	ch_ulong ch_error_primitive;	/* primitive in error */
	ch_ulong ch_error_type;		/* CHI error */
	ch_ulong ch_unix_error;		/* UNIX error */
	ch_ulong ch_state;		/* resulting state */
} CH_error_ack_t;

/*
   error types 
 */
#define CHSYSERR	 0	/* UNIX system error */
#define CHBADADDR	 1	/* Bad address format or content */
#define CHOUTSTATE	 2	/* Interface out of state */
#define CHBADOPT	 3	/* Bad options format or content */
#define CHBADPARM	 4	/* Bad parameter format or content */
#define CHBADPARMTYPE	 5	/* Bad paramater structure type */
#define CHBADFLAG	 6	/* Bad flag */
#define CHBADPRIM	 7	/* Bad primitive */
#define CHNOTSUPP	 8	/* Primitive not supported */

/*
 *  CH_ENABLE_REQ
 *  -------------------------------------------------------------------------
 */
typedef struct CH_enable_req {
	ch_ulong ch_primitive;		/* always CH_ENABLE_REQ */
} CH_enable_req_t;

/*
 *  CH_ENABLE_CON
 *  -------------------------------------------------------------------------
 */
typedef struct CH_enable_con {
	ch_ulong ch_primitive;		/* always CH_ENABLE_CON */
} CH_enable_con_t;

/*
 *  CH_DISABLE_REQ
 *  -------------------------------------------------------------------------
 */
typedef struct CH_disable_req {
	ch_ulong ch_primitive;		/* always CH_DISABLE_REQ */
} CH_disable_req_t;

/*
 *  CH_DISABLE_IND
 *  -------------------------------------------------------------------------
 */
typedef struct CH_disable_ind {
	ch_ulong ch_primitive;		/* always CH_DISABLE_IND */
	ch_ulong ch_cause;		/* cause for disable */
} CH_disable_ind_t;

/*
 *  CH_DISABLE_CON
 *  -------------------------------------------------------------------------
 */
typedef struct CH_disable_con {
	ch_ulong ch_primitive;		/* always CH_DISABLE_CON */
} CH_disable_con_t;

/*
 *  CH_DATA_REQ
 *  -------------------------------------------------------------------------
 */
typedef struct CH_data_req {
	ch_ulong ch_primitive;		/* always CH_DATA_REQ */
	ch_ulong ch_slot;		/* slot within channel */
} CH_data_req_t;

/*
 *  CH_DATA_IND
 *  -------------------------------------------------------------------------
 */
typedef struct CH_data_ind {
	ch_ulong ch_primitive;		/* always CH_DATA_IND */
	ch_ulong ch_slot;		/* slot within channel */
} CH_data_ind_t;

/*
 *  CH_CONNECT_REQ
 *  -------------------------------------------------------------------------
 */
typedef struct CH_connect_req {
	ch_ulong ch_primitive;		/* always CH_CONNECT_REQ */
	ch_ulong ch_conn_flags;		/* direction to connect */
	ch_ulong ch_slot;		/* slot within channel */
} CH_connect_req_t;

/*
   connect flags 
 */
#define CHF_RX_DIR	0x01
#define CHF_TX_DIR	0x02
#define CHF_BOTH_DIR	(CHF_RX_DIR|CHF_TX_DIR)

/*
 *  CH_CONNECT_CON
 *  -------------------------------------------------------------------------
 */
typedef struct CH_connect_con {
	ch_ulong ch_primitive;		/* always CH_CONNECT_CON */
	ch_ulong ch_conn_flags;		/* direction connected */
	ch_ulong ch_slot;		/* slot within channel */
} CH_connect_con_t;

/*
 *  CH_DISCONNECT_REQ
 *  -------------------------------------------------------------------------
 */
typedef struct CH_disconnect_req {
	ch_ulong ch_primitive;		/* always CH_DISCONNECT_REQ */
	ch_ulong ch_conn_flags;		/* direction to disconnect */
	ch_ulong ch_slot;		/* slot within channel */
} CH_disconnect_req_t;

/*
 *  CH_DISCONNECT_IND
 *  -------------------------------------------------------------------------
 */
typedef struct CH_disconnect_ind {
	ch_ulong ch_primitive;		/* always CH_DISCONNECT_IND */
	ch_ulong ch_conn_flags;		/* direction disconnected */
	ch_ulong ch_cause;		/* cause for disconnection */
	ch_ulong ch_slot;		/* slot within channel */
} CH_disconnect_ind_t;

/*
 *  CH_DISCONNECT_CON
 *  -------------------------------------------------------------------------
 */
typedef struct CH_disconnect_con {
	ch_ulong ch_primitive;		/* always CH_DISCONNECT_CON */
	ch_ulong ch_conn_flags;		/* direction disconnected */
	ch_ulong ch_slot;		/* slot within channel */
} CH_disconnect_con_t;

/*
 *  CH_EVENT_IND
 *  -------------------------------------------------------------------------
 */
typedef struct CH_event_ind {
	ch_ulong ch_primitive;		/* always CH_EVENT_IND */
	ch_ulong ch_event;		/* event */
	ch_ulong ch_slot;		/* slot within channel for event */
} CH_event_ind_t;

#define CH_EVT_DCD_ASSERT		 0
#define CH_EVT_DCD_DEASSERT		 1
#define CH_EVT_DSR_ASSERT		 2
#define CH_EVT_DSR_DEASSERT		 3
#define CH_EVT_DTR_ASSERT		 4
#define CH_EVT_DTR_DEASSERT		 5
#define CH_EVT_RTS_ASSERT		 6
#define CH_EVT_RTS_DEASSERT		 7
#define CH_EVT_CTS_ASSERT		 8
#define CH_EVT_CTS_DEASSERT		 9
#define CH_EVT_RI_ASSERT		10
#define CH_EVT_RI_DEASSERT		11
#define CH_EVT_YEL_ALARM		12
#define CH_EVT_BLU_ALARM		13
#define CH_EVT_RED_ALARM		14
#define CH_EVT_NO_ALARM			15

#define CHF_EVT_DCD_ASSERT		(1 <<  0)
#define CHF_EVT_DCD_DEASSERT		(1 <<  1)
#define CHF_EVT_DSR_ASSERT		(1 <<  2)
#define CHF_EVT_DSR_DEASSERT		(1 <<  3)
#define CHF_EVT_DTR_ASSERT		(1 <<  4)
#define CHF_EVT_DTR_DEASSERT		(1 <<  5)
#define CHF_EVT_RTS_ASSERT		(1 <<  6)
#define CHF_EVT_RTS_DEASSERT		(1 <<  7)
#define CHF_EVT_CTS_ASSERT		(1 <<  8)
#define CHF_EVT_CTS_DEASSERT		(1 <<  9)
#define CHF_EVT_RI_ASSERT		(1 << 10)
#define CHF_EVT_RI_DEASSERT		(1 << 11)
#define CHF_EVT_YEL_ALARM		(1 << 12)
#define CHF_EVT_BLU_ALARM		(1 << 13)
#define CHF_EVT_RED_ALARM		(1 << 14)
#define CHF_EVT_NO_ALARM		(1 << 15)

#define CHF_EVT_DCD_CHANGE		(CHF_EVT_DCD_ASSERT|CHF_EVT_DCD_DEASSERT)
#define CHF_EVT_DSR_CHANGE		(CHF_EVT_DSR_ASSERT|CHF_EVT_DSR_DEASSERT)
#define CHF_EVT_DTR_CHANGE		(CHF_EVT_DTR_ASSERT|CHF_EVT_DTR_DEASSERT)
#define CHF_EVT_RTS_CHANGE		(CHF_EVT_RTS_ASSERT|CHF_EVT_RTS_DEASSERT)
#define CHF_EVT_CTS_CHANGE		(CHF_EVT_CTS_ASSERT|CHF_EVT_CTS_DEASSERT)
#define CHF_EVT_RI_CHANGE		(CHF_EVT_RI_ASSERT|CHF_EVT_RI_DEASSERT)

#endif				/* __SS7_CHI_H__ */
