/*****************************************************************************

 @(#) $Id: lmi.h,v 0.9.2.1 2004/08/21 10:14:39 brian Exp $

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

 Last Modified $Date: 2004/08/21 10:14:39 $ by $Author: brian $

 *****************************************************************************/

#ifndef __LMI_H__
#define __LMI_H__

#ident "@(#) $Name:  $($Revision: 0.9.2.1 $) Copyright (c) 1997-2002 OpenSS7 Corporation."

#define LMI_PROTO_BASE		  0L

#define LMI_DSTR_FIRST		( 1L + LMI_PROTO_BASE )
#define LMI_INFO_REQ		( 1L + LMI_PROTO_BASE )
#define LMI_ATTACH_REQ		( 2L + LMI_PROTO_BASE )
#define LMI_DETACH_REQ		( 3L + LMI_PROTO_BASE )
#define LMI_ENABLE_REQ		( 4L + LMI_PROTO_BASE )
#define LMI_DISABLE_REQ		( 5L + LMI_PROTO_BASE )
#define LMI_OPTMGMT_REQ		( 6L + LMI_PROTO_BASE )
#define LMI_DSTR_LAST		( 6L + LMI_PROTO_BASE )

#define LMI_USTR_LAST		(-1L - LMI_PROTO_BASE )
#define LMI_INFO_ACK		(-1L - LMI_PROTO_BASE )
#define LMI_OK_ACK		(-2L - LMI_PROTO_BASE )
#define LMI_ERROR_ACK		(-3L - LMI_PROTO_BASE )
#define LMI_ENABLE_CON		(-4L - LMI_PROTO_BASE )
#define LMI_DISABLE_CON		(-5L - LMI_PROTO_BASE )
#define LMI_OPTMGMT_ACK		(-6L - LMI_PROTO_BASE )
#define LMI_ERROR_IND		(-7L - LMI_PROTO_BASE )
#define LMI_STATS_IND		(-8L - LMI_PROTO_BASE )
#define LMI_EVENT_IND		(-9L - LMI_PROTO_BASE )
#define LMI_USTR_FIRST		(-9L - LMI_PROTO_BASE )

#define LMI_UNATTACHED		1L	/* No PPA attached, awating LMI_ATTACH_REQ */
#define LMI_ATTACH_PENDING	2L	/* Waiting for attach */
#define LMI_UNUSABLE		3L	/* Device cannot be used, STREAM in hung state */
#define LMI_DISABLED		4L	/* PPA attached, awaiting LMI_ENABLE_REQ */
#define LMI_ENABLE_PENDING	5L	/* Waiting to send LMI_ENABLE_CON */
#define LMI_ENABLED		6L	/* Ready for use, awaiting primtiive exchange */
#define LMI_DISABLE_PENDING	7L	/* Waiting to send LMI_DISABLE_CON */
#define LMI_DETACH_PENDING	8L	/* Waiting for detach */

/*
 *  LMI_ERROR_ACK and LMI_ERROR_IND reason codes
 */
#define LMI_UNSPEC		0x00000000	/* Unknown or unspecified */
#define LMI_BADADDRESS		0x00010000	/* Address was invalid */
#define LMI_BADADDRTYPE		0x00020000	/* Invalid address type */
#define LMI_BADDIAL		0x00030000	/* (not used) */
#define LMI_BADDIALTYPE		0x00040000	/* (not used) */
#define LMI_BADDISPOSAL		0x00050000	/* Invalid disposal parameter */
#define LMI_BADFRAME		0x00060000	/* Defective SDU received */
#define LMI_BADPPA		0x00070000	/* Invalid PPA identifier */
#define LMI_BADPRIM		0x00080000	/* Unregognized primitive */
#define LMI_DISC		0x00090000	/* Disconnected */
#define LMI_EVENT		0x000a0000	/* Protocol-specific event ocurred */
#define LMI_FATALERR		0x000b0000	/* Device has become unusable */
#define LMI_INITFAILED		0x000c0000	/* Link initialization failed */
#define LMI_NOTSUPP		0x000d0000	/* Primitive not supported by this device */
#define LMI_OUTSTATE		0x000e0000	/* Primitive was issued from invalid state */
#define LMI_PROTOSHORT		0x000f0000	/* M_PROTO block too short */
#define LMI_SYSERR		0x00100000	/* UNIX system error */
#define LMI_WRITEFAIL		0x00110000	/* Unitdata request failed */
#define LMI_CRCERR		0x00120000	/* CRC or FCS error */
#define LMI_DLE_EOT		0x00130000	/* DLE EOT detected */
#define LMI_FORMAT		0x00140000	/* Format error detected */
#define LMI_HDLC_ABORT		0x00150000	/* Aborted frame detected */
#define LMI_OVERRUN		0x00160000	/* Input overrun */
#define LMI_TOOSHORT		0x00170000	/* Frame too short */
#define LMI_INCOMPLETE		0x00180000	/* Partial frame received */
#define LMI_BUSY		0x00190000	/* Telephone was busy */
#define LMI_NOANSWER		0x001a0000	/* Connection went unanswered */
#define LMI_CALLREJECT		0x001b0000	/* Connection rejected */
#define LMI_HDLC_IDLE		0x001c0000	/* HDLC line went idle */
#define LMI_HDLC_NOTIDLE	0x001d0000	/* HDLC link no longer idle */
#define LMI_QUIESCENT		0x001e0000	/* Line being reassigned */
#define LMI_RESUMED		0x001f0000	/* Line has been reassigned */
#define LMI_DSRTIMEOUT		0x00200000	/* Did not see DSR in time */
#define LMI_LAN_COLLISIONS	0x00210000	/* LAN excessive collisions */
#define LMI_LAN_REFUSED		0x00220000	/* LAN message refused */
#define LMI_LAN_NOSTATION	0x00230000	/* LAN no such station */
#define LMI_LOSTCTS		0x00240000	/* Lost Clear to Send signal */
#define LMI_DEVERR		0x00250000	/* Start of device-specific error codes */

typedef signed long lmi_long;
typedef unsigned long lmi_ulong;
typedef unsigned short lmi_ushort;
typedef unsigned char lmi_uchar;

/*
 *  LOCAL MANAGEMENT PRIMITIVES
 */

/*
   LMI_INFO_REQ, M_PROTO or M_PCPROTO 
 */

typedef struct {
	lmi_long lmi_primitive;		/* LMI_INFO_REQ */
} lmi_info_req_t;

/*
   LMI_INFO_ACK, M_PROTO or M_PCPROTO 
 */

typedef struct {
	lmi_long lmi_primitive;		/* LMI_INFO_ACK */
	lmi_ulong lmi_version;
	lmi_ulong lmi_state;
	lmi_ulong lmi_max_sdu;
	lmi_ulong lmi_min_sdu;
	lmi_ulong lmi_header_len;
	lmi_ulong lmi_ppa_style;
	lmi_uchar lmi_ppa_addr[0];
} lmi_info_ack_t;

/*
 *  LMI provider style.
 *
 *  The LMI provider style which determines whether a provider requires an
 *  LMI_ATTACH_REQ to inform the provider which PPA user messages should be
 *  sent/received on.
 */
#define LMI_STYLE1	0x00	/* PPA is implicitly bound by open(2) */
#define LMI_STYLE2	0x01	/* PPA must be explicitly bound via STD_ATTACH_REQ */

/*
   LMI_ATTACH_REQ, M_PROTO or M_PCPROTO 
 */

typedef struct {
	lmi_long lmi_primitive;		/* LMI_ATTACH_REQ */
	lmi_uchar lmi_ppa[0];
} lmi_attach_req_t;

/*
   LMI_DETACH_REQ, M_PROTO or M_PCPROTO 
 */

typedef struct {
	lmi_long lmi_primitive;		/* LMI_DETACH_REQ */
} lmi_detach_req_t;

/*
   LMI_ENABLE_REQ, M_PROTO or M_PCPROTO 
 */

typedef struct {
	lmi_long lmi_primitive;		/* LMI_ENABLE_REQ */
	lmi_uchar lmi_rem[0];
} lmi_enable_req_t;

/*
   LMI_DISABLE_REQ, M_PROTO or M_PCPROTO 
 */

typedef struct {
	lmi_long lmi_primitive;		/* LMI_DISABLE_REQ */
} lmi_disable_req_t;

/*
   LMI_OK_ACK, M_PROTO or M_PCPROTO 
 */

typedef struct {
	lmi_long lmi_primitive;		/* LMI_OK_ACK */
	lmi_long lmi_correct_primitive;
	lmi_ulong lmi_state;
} lmi_ok_ack_t;

/*
   LMI_ERROR_ACK, M_CTL 
 */

typedef struct {
	lmi_long lmi_primitive;		/* LMI_ERROR_ACK */
	lmi_ulong lmi_errno;
	lmi_ulong lmi_reason;
	lmi_long lmi_error_primitive;
	lmi_ulong lmi_state;
} lmi_error_ack_t;

/*
   LMI_ENABLE_CON, M_PROTO or M_PCPROTO 
 */

typedef struct {
	lmi_long lmi_primitive;		/* LMI_ENABLE_CON */
	lmi_ulong lmi_state;
} lmi_enable_con_t;

/*
   LMI_DISABLE_CON, M_PROTO or M_PCPROTO 
 */

typedef struct {
	lmi_long lmi_primitive;		/* LMI_DISABLE_CON */
	lmi_ulong lmi_state;
} lmi_disable_con_t;

/*
   LMI_OPTMGMT_REQ, M_PCPROTO 
 */

typedef struct {
	lmi_long lmi_primitive;		/* LMI_OPTMGMT_REQ */
	lmi_ulong lmi_opt_length;
	lmi_ulong lmi_opt_offset;
	lmi_ulong lmi_mgmt_flags;
} lmi_optmgmt_req_t;

/*
   LMI_OPTMGMT_ACK, M_PCPROTO 
 */

typedef struct {
	lmi_long lmi_primitive;		/* LMI_OPMGMT_ACK */
	lmi_ulong lmi_opt_length;
	lmi_ulong lmi_opt_offset;
	lmi_ulong lmi_mgmt_flags;
} lmi_optmgmt_ack_t;

#define LMI_NEGOTIATE		0x0004
#define LMI_CHECK		0x0008
#define LMI_DEFAULT		0x0010
#define LMI_SUCCESS		0x0020
#define LMI_FAILURE		0x0040
#define LMI_CURRENT		0x0080
#define LMI_PARTSUCCESS		0x0100
#define LMI_READONLY		0x0200
#define LMI_NOTSUPPORT		0x0400

/*
   LMI_ERROR_IND, M_PROTO or M_PCPROTO 
 */

typedef struct {
	lmi_long lmi_primitive;		/* LMI_ERROR_IND */
	lmi_ulong lmi_errno;
	lmi_ulong lmi_reason;
	lmi_ulong lmi_state;
} lmi_error_ind_t;

/*
   LMI_STATS_IND, M_PROTO 
 */

typedef struct {
	lmi_long lmi_primitive;		/* LMI_STATS_IND */
	lmi_ulong lmi_interval;
	lmi_ulong lmi_timestamp;
} lmi_stats_ind_t;

/*
   LMI_EVENT_IND, M_PROTO 
 */

typedef struct {
	lmi_long lmi_primitive;		/* LMI_EVENT_IND */
	lmi_ulong lmi_objectid;
	lmi_ulong lmi_timestamp;
	lmi_ulong lmi_severity;
} lmi_event_ind_t;

union LMI_primitive {
	lmi_long lmi_primitive;
	lmi_ok_ack_t ok_ack;
	lmi_error_ack_t error_ack;
	lmi_error_ind_t error_ind;
	lmi_stats_ind_t stats_ind;
	lmi_event_ind_t event_ind;
};

union LMI_primitives {
	lmi_long lmi_primitive;
	lmi_info_req_t info_req;
	lmi_info_ack_t info_ack;
	lmi_attach_req_t attach_req;
	lmi_detach_req_t detach_req;
	lmi_enable_req_t enable_req;
	lmi_disable_req_t disable_req;
	lmi_ok_ack_t ok_ack;
	lmi_error_ack_t error_ack;
	lmi_enable_con_t enable_con;
	lmi_disable_con_t disable_con;
	lmi_error_ind_t error_ind;
	lmi_stats_ind_t stats_ind;
	lmi_event_ind_t event_ind;
};

#define LMI_INFO_REQ_SIZE	sizeof(lmi_info_req_t)
#define LMI_INFO_ACK_SIZE	sizeof(lmi_info_ack_t)
#define LMI_ATTACH_REQ_SIZE	sizeof(lmi_attach_req_t)
#define LMI_DETACH_REQ_SIZE	sizeof(lmi_detach_req_t)
#define LMI_ENABLE_REQ_SIZE	sizeof(lmi_enable_req_t)
#define LMI_DISABLE_REQ_SIZE	sizeof(lmi_disable_req_t)
#define LMI_OK_ACK_SIZE		sizeof(lmi_ok_ack_t)
#define LMI_ERROR_ACK_SIZE	sizeof(lmi_error_ack_t)
#define LMI_ENABLE_CON_SIZE	sizeof(lmi_enable_con_t)
#define LMI_DISABLE_CON_SIZE	sizeof(lmi_disable_con_t)
#define LMI_ERROR_IND_SIZE	sizeof(lmi_error_ind_t)
#define LMI_STATS_IND_SIZE	sizeof(lmi_stats_ind_t)
#define LMI_EVENT_IND_SIZE	sizeof(lmi_event_ind_t)

typedef struct lmi_opthdr {
	lmi_ulong level;
	lmi_ulong name;
	lmi_ulong length;
	lmi_ulong status;
	lmi_uchar value[0];
	/*
	   followed by option value 
	 */
} lmi_opthdr_t;

#define LMI_LEVEL_COMMON	'\0'
#define LMI_LEVEL_SDL		'd'
#define LMI_LEVEL_SDT		't'
#define LMI_LEVEL_SL		'l'
#define LMI_LEVEL_SLS		's'
#define LMI_LEVEL_MTP		'M'
#define LMI_LEVEL_SCCP		'S'
#define LMI_LEVEL_ISUP		'I'
#define LMI_LEVEL_TCAP		'T'

#define LMI_OPT_PROTOCOL	1	/* use struct lmi_option */
#define LMI_OPT_STATISTICS	2	/* use struct lmi_sta */

#endif				/* __LMI_H__ */
