/*****************************************************************************

 @(#) $RCSfile: ua_msg.h,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2008-04-29 01:52:22 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

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

 Last Modified $Date: 2008-04-29 01:52:22 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: ua_msg.h,v $
 Revision 0.9.2.5  2008-04-29 01:52:22  brian
 - updated headers for release

 Revision 0.9.2.4  2007/08/14 08:34:01  brian
 - GPLv3 header update

 Revision 0.9.2.3  2007/02/14 14:08:59  brian
 - broad changes updating support for SS7 MTP and M3UA

 Revision 0.9.2.2  2007/02/10 22:32:13  brian
 - working up sigtran drivers

 Revision 1.1.2.1  2007/02/03 03:05:16  brian
 - added new files

 *****************************************************************************/

#ifndef __LOCAL_UA_MSG_H__
#define __LOCAL_UA_MSG_H__

#ident "@(#) $RCSfile: ua_msg.h,v $ $Name:  $($Revision: 0.9.2.5 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

/*
 *  This file is usable both by kernel modules and user space programs.  The
 *  OpenSS7 Project uses this file from user space primarily in test programs.
 */

/*
 *  UA Message Definitions
 *  =========================================================================
 */

#ifndef UA_PROTOCOL
#define UA_PROTOCOL UA_PROTOCOL_ALL
#endif				/* UA_PROTOCOL */

#ifndef UA_VERSION
#define UA_VERSION	1
#endif				/* UA_VERSION */

#define UA_PROTOCOL_ALL		0
#define UA_PROTOCOL_IUA		1
#define UA_PROTOCOL_DUA		2
#define UA_PROTOCOL_V5UA	3
#define UA_PROTOCOL_GUA		4
#define UA_PROTOCOL_M2UA	5
#define UA_PROTOCOL_M3UA	6
#define UA_PROTOCOL_SUA		7
#define UA_PROTOCOL_TUA		8
#define UA_PROTOCOL_ISUA	9

#define UA_USE_PROTOCOL(x) ((UA_PROTOCOL == UA_PROTOCOL_ALL) || (UA_PROTOCOL == (x)))

#define UA_PROFILE(level,version,standard) ((level<<16)|(version<<8)|(standard<<0))

#define UA_PROFILE_LEVEL(profile) ((profile>>16)&0xff)
#define UA_PROFILE_VERSION(profile) ((profile>>8)&0xff)
#define UA_PROFILE_STANDARD(profile) ((profile>>0)&0xff)

/* *INDENT-OFF* */
#define UA_PROFILE_NONE		UA_PROFILE(0,                0,             0)
#define UA_PROFILE_RFC3057	UA_PROFILE(UA_PROTOCOL_IUA,  IUA_VERSION,   1)	/* IUA RFC 3057 */
#define UA_PROFILE_RFC3331	UA_PROFILE(UA_PROTOCOL_M2UA, M2UA_VERSION,  2)	/* M2UA RFC 3331 */
#define UA_PROFILE_RFC3332	UA_PROFILE(UA_PROTOCOL_M3UA, M3UA_VERSION,  3)	/* M3UA RFC 3332 */
#define UA_PROFILE_RFC3868	UA_PROFILE(UA_PROTOCOL_SUA,  SUA_VERSION,   4)	/* SUA RFC 3868 */
#define UA_PROFILE_RFC4233	UA_PROFILE(UA_PROTOCOL_IUA,  IUA_VERSION,   5)	/* IUA RFC 4133 */
#define UA_PROFILE_RFC4666	UA_PROFILE(UA_PROTOCOL_M3UA, M3UA_VERSION,  6)	/* M3UA RFC 4666 */
#define UA_PROFILE_TS102141	UA_PROFILE(UA_PROTOCOL_M2UA, M2UA_VERSION,  7)	/* M2UA TS 102 141 */
#define UA_PROFILE_TS102142	UA_PROFILE(UA_PROTOCOL_M3UA, M3UA_VERSION,  8)	/* M3UA TS 102 142 */
#define UA_PROFILE_TS102143	UA_PROFILE(UA_PROTOCOL_SUA,  SUA_VERSION,   9)	/* SUA TS 102 143 */
#define UA_PROFILE_SUABIS	UA_PROFILE(UA_PROTOCOL_SUA,  SUA_VERSION,  10)	/* draft-ietf-sigtran-sua-00.txt */
#define UA_PROFILE_TUA05	UA_PROFILE(UA_PROTOCOL_TUA,  TUA_VERSION,  11)	/* draft-bidulock-sigtran-tua-05.txt */
#define UA_PROFILE_ISUA04	UA_PROFILE(UA_PROTOCOL_ISUA, ISUA_VERSION, 12)	/* draft-bidulock-sigtran-isua-04.txt */
#define UA_PROFILE_DEFAULT	UA_PROFILE_RFC3332
/* *INDENT-ON* */

#define UA_PAD4(__len) (((__len)+3)&~0x3)
#define UA_MHDR(__version, __spare, __class, __type) \
	(__constant_htonl(((__version)<<24)|((__spare)<<16)|((__class)<<8)|(__type)))

#define UA_MSG_VERS(__hdr) ((ntohl(__hdr)>>24)&0xff)
#define UA_MSG_CLAS(__hdr) ((ntohl(__hdr)>> 8)&0xff)
#define UA_MSG_TYPE(__hdr) ((ntohl(__hdr)>> 0)&0xff)

/*
 *  MESSAGE CLASSES:-
 */
#define UA_CLASS_MGMT	0x00	/* UA Management (MGMT) Message */
#define UA_CLASS_XFER	0x01	/* M3UA Data transfer message */
#define UA_CLASS_SNMM	0x02	/* Signalling Network Mgmt (SNM) Messages */
#define UA_CLASS_ASPS	0x03	/* ASP State Maintenance (ASPSM) Messages */
#define UA_CLASS_ASPT	0x04	/* ASP Traffic Maintenance (ASPTM) Messages */
#define UA_CLASS_QPTM	0x05	/* Q.921 User Part Messages */
#define UA_CLASS_MAUP	0x06	/* M2UA Messages */
#define UA_CLASS_CNLS	0x07	/* SUA Connectionless Messages */
#define UA_CLASS_CONS	0x08	/* SUA Connection Oriented Messages */
#define UA_CLASS_RKMM	0x09	/* Routing Key Management Messages */
#define UA_CLASS_TDHM	0x0a	/* TUA Dialog Handling Mesages */
#define UA_CLASS_TCHM	0x0b	/* TUA Component Handling Messages */
#define UA_CLASS_ISCP	0x0a	/* ISUA Call Processing Messages */
#define UA_CLASS_ISCS	0x0b	/* ISUA Circuit Supervision Messages */

/*
 *  MESSAGES DEFINED IN EACH CLASS:-
 */
#define UA_MGMT_ERR		UA_MHDR(UA_VERSION, 0, UA_CLASS_MGMT, 0x00)
#define UA_MGMT_NTFY		UA_MHDR(UA_VERSION, 0, UA_CLASS_MGMT, 0x01)
#define UA_MGMT_LAST		0x01

#define UA_XFER_DATA		UA_MHDR(UA_VERSION, 0, UA_CLASS_XFER, 0x01)

#define UA_SNMM_DUNA		UA_MHDR(UA_VERSION, 0, UA_CLASS_SNMM, 0x01)
#define UA_SNMM_DAVA		UA_MHDR(UA_VERSION, 0, UA_CLASS_SNMM, 0x02)
#define UA_SNMM_DAUD		UA_MHDR(UA_VERSION, 0, UA_CLASS_SNMM, 0x03)
#define UA_SNMM_SCON		UA_MHDR(UA_VERSION, 0, UA_CLASS_SNMM, 0x04)
#define UA_SNMM_DUPU		UA_MHDR(UA_VERSION, 0, UA_CLASS_SNMM, 0x05)
#define UA_SNMM_DRST		UA_MHDR(UA_VERSION, 0, UA_CLASS_SNMM, 0x06)
#define UA_SNMM_LAST		0x06

#define UA_ASPS_ASPUP_REQ	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPS, 0x01)
#define UA_ASPS_ASPDN_REQ	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPS, 0x02)
#define UA_ASPS_HBEAT_REQ	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPS, 0x03)
#define UA_ASPS_ASPUP_ACK	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPS, 0x04)
#define UA_ASPS_ASPDN_ACK	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPS, 0x05)
#define UA_ASPS_HBEAT_ACK	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPS, 0x06)
#define UA_ASPS_LAST		0x06

#define UA_ASPT_ASPAC_REQ	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPT, 0x01)
#define UA_ASPT_ASPIA_REQ	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPT, 0x02)
#define UA_ASPT_ASPAC_ACK	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPT, 0x03)
#define UA_ASPT_ASPIA_ACK	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPT, 0x04)
#define UA_ASPT_LAST		0x04

#define UA_RKMM_REG_REQ		UA_MHDR(UA_VERSION, 0, UA_CLASS_RKMM, 0x01)
#define UA_RKMM_REG_RSP		UA_MHDR(UA_VERSION, 0, UA_CLASS_RKMM, 0x02)
#define UA_RKMM_DEREG_REQ	UA_MHDR(UA_VERSION, 0, UA_CLASS_RKMM, 0x03)
#define UA_RKMM_DEREG_RSP	UA_MHDR(UA_VERSION, 0, UA_CLASS_RKMM, 0x04)
#define UA_RKMM_LAST		0x04

#define UA_MHDR_SIZE (sizeof(uint32_t)*2)
#define UA_PHDR_SIZE (sizeof(uint32_t))
#define UA_MAUP_SIZE (UA_MHDR_SIZE + UA_PHDR_SIZE + sizeof(uint32_t))

#define UA_TAG_MASK		(htonl(0xffff0000))
#define UA_PTAG(__phdr)		((htonl(__phdr)>>16)&0xffff)
#define UA_PLEN(__phdr)		(htonl(__phdr)&0xffff)
#define UA_SIZE(__phdr)		(htonl(__phdr)&0xffff)
#define UA_TAG(__phdr)		((htonl(__phdr)>>16)&0xffff)
#define UA_PHDR(__phdr, __length) \
	(__constant_htonl(((__phdr)<<16)|((__length)+sizeof(uint32_t))))
#define UA_CONST_PHDR(__phdr, __length) \
	(__constant_htonl(((__phdr)<<16)|((__length)+sizeof(uint32_t))))

/*
 *  COMMON PARAMETERS:-
 *
 *  Common parameters per draft-ietf-sigtran-m2ua-10.txt
 *  Common parameters per draft-ietf-sigtran-m3ua-08.txt
 *  Common parameters per draft-ietf-sigtran-sua-07.txt
 *  Common parameters per rfc3057.txt
 *  -------------------------------------------------------------------
 */
#define UA_PARM_RESERVED	UA_CONST_PHDR(0x0000,0)
#define UA_PARM_IID		UA_CONST_PHDR(0x0001,sizeof(uint32_t))
//#define UA_PARM_IID_RANGE_O   UA_CONST_PHDR(0x0002,0) /* m2ua-10 */
#define UA_PARM_DATA		UA_CONST_PHDR(0x0003,0)	/* sua-07 */
#define UA_PARM_IID_TEXT	UA_CONST_PHDR(0x0003,0)
#define UA_PARM_INFO		UA_CONST_PHDR(0x0004,0)
//#define UA_PARM_APC           UA_CONST_PHDR(0x0005,sizeof(uint32_t))  /* sua-07 */
#define UA_PARM_DLCI		UA_CONST_PHDR(0x0005,sizeof(uint32_t))	/* rfc3057 */
#define UA_PARM_RC		UA_CONST_PHDR(0x0006,sizeof(uint32_t))
#define UA_PARM_DIAG		UA_CONST_PHDR(0x0007,0)
#define UA_PARM_IID_RANGE	UA_CONST_PHDR(0x0008,0)	/* rfc3057, rfc3331 */
#define UA_PARM_HBDATA		UA_CONST_PHDR(0x0009,0)
#define UA_PARM_REASON		UA_CONST_PHDR(0x000a,sizeof(uint32_t))
#define UA_PARM_TMODE		UA_CONST_PHDR(0x000b,sizeof(uint32_t))
#define UA_PARM_ECODE		UA_CONST_PHDR(0x000c,sizeof(uint32_t))
#define UA_PARM_STATUS		UA_CONST_PHDR(0x000d,sizeof(uint32_t))
//#define UA_PARM_ASPID         UA_CONST_PHDR(0x000e,sizeof(uint32_t))
#define UA_PARM_PROT_DATA	UA_CONST_PHDR(0x000e,sizeof(uint32_t))	/* rfc3057, rfc4233 */
#define UA_PARM_CONG_LEVEL	UA_CONST_PHDR(0x000f,sizeof(uint32_t))	/* sua-07 */
#define UA_PARM_REL_REASON	UA_CONST_PHDR(0x000f,sizeof(uint32_t))	/* rfc3057 */
#define UA_PARM_TEI_STATUS	UA_CONST_PHDR(0x0010,sizeof(uint32_t))	/* rfc3057 */
#define UA_PARM_ASPID		UA_CONST_PHDR(0x0011,sizeof(uint32_t))	/* rfc4233 */
#define UA_PARM_APC		UA_CONST_PHDR(0x0012,sizeof(uint32_t))
#define UA_PARM_CORID		UA_CONST_PHDR(0x0013,sizeof(uint32_t))
#define UA_PARM_REG_RESULT	UA_CONST_PHDR(0x0014,sizeof(uint32_t))	/* rfc3868, tua-04 */
#define UA_PARM_DEREG_RESULT	UA_CONST_PHDR(0x0015,sizeof(uint32_t)*4)	/* rfc3868, tua-04 */
#define UA_PARM_REG_STATUS	UA_CONST_PHDR(0x0016,sizeof(uint32_t))	/* rfc3868, tua-04 */
#define UA_PARM_DEREG_STATUS	UA_CONST_PHDR(0x0017,sizeof(uint32_t))	/* rfc3868, tua-04 */
#define UA_PARM_LOC_KEY_ID	UA_CONST_PHDR(0x0018,sizeof(uint32_t))	/* rfc3868, tua-04 */

#define UA_PARM_ASPEXT		UA_CONST_PHDR(0x0011,sizeof(uint32_t))	/* aspext-04 */
#define UA_PARM_PROTO_LIMITS	UA_CONST_PHDR(0x001b,sizeof(uint32_t)*6)	/* sginfo-05 */

/*
 *  Somewhat common field values:
 */
#define   UA_ECODE_INVALID_VERSION		(0x01)
#define   UA_ECODE_INVALID_IID			(0x02)
#define   UA_ECODE_INVALID_RC			(0x02)
#define   UA_ECODE_UNSUPPORTED_MESSAGE_CLASS	(0x03)
#define   UA_ECODE_UNSUPPORTED_MESSAGE_TYPE	(0x04)
#define   UA_ECODE_UNSUPPORTED_TRAFFIC_MODE	(0x05)
#define   UA_ECODE_UNEXPECTED_MESSAGE		(0x06)
#define   UA_ECODE_PROTOCOL_ERROR		(0x07)
#define   UA_ECODE_UNSUPPORTED_IID_TYPE		(0x08)
#define   UA_ECODE_INVALID_STREAM_IDENTIFIER	(0x09)
#if UA_USE_PROTOCOL(UA_PROTOCOL_IUA)
#define  IUA_ECODE_UNASSIGNED_TEI		(0x0a)
#define  IUA_ECODE_UNRECOGNIZED_SAPI		(0x0b)
#define  IUA_ECODE_INVALID_TEI_SAPI_COMBINATION	(0x0c)
#endif				/* UA_USE_PROTOCOL(UA_PROTOCOL_IUA) */
#define   UA_ECODE_REFUSED_MANAGEMENT_BLOCKING	(0x0d)
#define   UA_ECODE_ASPID_REQUIRED		(0x0e)
#define   UA_ECODE_INVALID_ASPID		(0x0f)
#if UA_USE_PROTOCOL(UA_PROTOCOL_M2UA)
#define M2UA_ECODE_ASP_ACTIVE_FOR_IIDS		(0x10)	/* not rfc4233 */
#endif				/* UA_USE_PROTOCOL(UA_PROTOCOL_M2UA) */
#define   UA_ECODE_INVALID_PARAMETER_VALUE	(0x11)	/* not rfc4233 */
#define   UA_ECODE_PARAMETER_FIELD_ERROR	(0x12)	/* not rfc4233 */
#define   UA_ECODE_UNEXPECTED_PARAMETER		(0x13)	/* not rfc4233 */
#define   UA_ECODE_DESTINATION_STATUS_UNKNOWN	(0x14)	/* not rfc4233 */
#define   UA_ECODE_INVALID_NETWORK_APPEARANCE	(0x15)	/* not rfc4233 */
#define   UA_ECODE_MISSING_PARAMETER		(0x16)	/* not rfc4233 */
#define   UA_ECODE_ROUTING_KEY_CHANGE_REFUSED	(0x17)	/* not rfc4233 */
#define   UA_ECODE_INVALID_ROUTING_CONTEXT	(0x19)	/* not rfc4233 */
#define   UA_ECODE_NO_CONFIGURED_AS_FOR_ASP	(0x1a)	/* not rfc4233 */
#define   UA_ECODE_SUBSYSTEM_STATUS_UNKNOWN	(0x1b)	/* not rfc4233 */

#define UA_STATUS_AS_DOWN			(0x00010001)
#define UA_STATUS_AS_INACTIVE			(0x00010002)
#define UA_STATUS_AS_ACTIVE			(0x00010003)
#define UA_STATUS_AS_PENDING			(0x00010004)
#define UA_STATUS_AS_INSUFFICIENT_ASPS		(0x00020001)
#define UA_STATUS_ALTERNATE_ASP_ACTIVE		(0x00020002)
#define UA_STATUS_ASP_FAILURE			(0x00020003)
#define UA_STATUS_AS_MINIMUM_ASPS		(0x00020004)

#ifndef UA_TMODE_OVERRIDE
#define UA_TMODE_OVERRIDE			(0x1)
#define UA_TMODE_LOADSHARE			(0x2)
#define UA_TMODE_BROADCAST			(0x3)
#endif				/* UA_TMODE_OVERRIDE */
#define UA_TMODE_SB_OVERRIDE			(0x4)
#define UA_TMODE_SB_LOADSHARE			(0x5)
#define UA_TMODE_SB_BROADCAST			(0x6)

#define UA_RESULT_SUCCESS			(0x00)
#define UA_RESULT_FAILURE			(0x01)

#define UA_REG_STATUS_SUCCESS			(0x0)
#define UA_REG_STATUS_UNKNOWN			(0x1)
#define UA_REG_STATUS_INVALID_SDLI		(0x2)	/* rfc3331 only */
#define UA_REG_STATUS_INVALID_DEST_ADDRESS	(0x2)	/* rfc3868 only */
#define UA_REG_STATUS_INVALID_DPC		(0x2)	/* rfc3332, rfc4666 only */
#define UA_REG_STATUS_INVALID_SDTI		(0x3)	/* rfc3331 only */
#define UA_REG_STATUS_INVALID_NA		(0x3)	/* rfc3332, rfc3868, rfc4666 only */
#define UA_REG_STATUS_INVALID_KEY		(0x4)
#define UA_REG_STATUS_PERMISSION_DENIED		(0x5)
#define UA_REG_STATUS_OVERLAPPING_KEY		(0x6)
#define UA_REG_STATUS_KEY_NOT_PROVISIONED	(0x7)
#define UA_REG_STATUS_INSUFFICIENT_RESOURCES	(0x8)
#define UA_REG_STATUS_UNSUPPORTED_KEY_FIELD	(0x9)	/* rfc3332, rfc3868, rfc4666, not rfc3331 */
#define UA_REG_STATUS_INVALID_TRAFFIC_MODE	(0xa)	/* rfc3332, rfc3868, rfc4666, not rfc3331 */
#define UA_REG_STATUS_KEY_CHANGE_REFUSED	(0xb)	/* rfc4666, rfc3868, not rfc3331, rfc3332 */
#define UA_REG_STATUS_KEY_ALREADY_REGISTERED	(0xc)	/* rfc4666 only */

#define UA_DEREG_STATUS_SUCCESS			(0x0)
#define UA_DEREG_STATUS_UNKNOWN			(0x1)
#define UA_DEREG_STATUS_INVALID_ID		(0x2)
#define UA_DEREG_STATUS_PERMISSION_DENIED	(0x3)
#define UA_DEREG_STATUS_NOT_REGISTERED		(0x4)
#define UA_DEREG_STATUS_ID_ACTIVE_FOR_AS	(0x5)	/* but not m2ua (rfc3331) */

#define UA_ASPEXT_NONE				(0x0)
#define UA_ASPEXT_SGINFO			(0x1)
#define UA_ASPEXT_LOADSEL			(0x2)
#define UA_ASPEXT_LOADGRP			(0x3)
#define UA_ASPEXT_CORID				(0x4)
#define UA_ASPEXT_REGEXT			(0x5)
#define UA_ASPEXT_SESSID			(0x6)
#define UA_ASPEXT_DYNAMIC			(0x7)
#define UA_ASPEXT_DEIPSP			(0x8)
#define UA_ASPEXT_ASPCONG			(0x9)
#define UA_ASPEXT_TEXTIID			(0xa)

/*
 *  IUA-Specific Values
 *  -------------------------------------------------------------------
 */
#ifndef IUA_PORT
#define IUA_PORT	9900	/* IUA well-known port number */
#endif				/* IUA_PORT */

#ifndef IUA_PPI
#define IUA_PPI		1	/* IUA SCTP Payload Protocol Identifier */
#endif				/* IUA_PPI */

#define IUA_VERSION	1	/* RFC 3057, RFC 4233 */

#if UA_USE_PROTOCOL(UA_PROTOCOL_IUA)

/*
 *  IUA-Specific Messages: per RFC 3057 and RFC 4233
 *  -------------------------------------------------------------------
 */
#define IUA_MGMT_TEIS_REQ	UA_MHDR(UA_VERSION, 0, UA_CLASS_MGMT, 0x02)
#define IUA_MGMT_TEIS_CON	UA_MHDR(UA_VERSION, 0, UA_CLASS_MGMT, 0x03)
#define IUA_MGMT_TEIS_IND	UA_MHDR(UA_VERSION, 0, UA_CLASS_MGMT, 0x04)
#define IUA_MGMT_TEIQ_REQ	UA_MHDR(UA_VERSION, 0, UA_CLASS_MGMT, 0x05)

#define IUA_QPTM_DATA_REQ	UA_MHDR(UA_VERSION, 0, UA_CLASS_QPTM, 0x01)
#define IUA_QPTM_DATA_IND	UA_MHDR(UA_VERSION, 0, UA_CLASS_QPTM, 0x02)
#define IUA_QPTM_UDAT_REQ	UA_MHDR(UA_VERSION, 0, UA_CLASS_QPTM, 0x03)
#define IUA_QPTM_UDAT_IND	UA_MHDR(UA_VERSION, 0, UA_CLASS_QPTM, 0x04)
#define IUA_QPTM_ESTB_REQ	UA_MHDR(UA_VERSION, 0, UA_CLASS_QPTM, 0x05)
#define IUA_QPTM_ESTB_CON	UA_MHDR(UA_VERSION, 0, UA_CLASS_QPTM, 0x06)
#define IUA_QPTM_ESTB_IND	UA_MHDR(UA_VERSION, 0, UA_CLASS_QPTM, 0x07)
#define IUA_QPTM_RELS_REQ	UA_MHDR(UA_VERSION, 0, UA_CLASS_QPTM, 0x08)
#define IUA_QPTM_RELS_CON	UA_MHDR(UA_VERSION, 0, UA_CLASS_QPTM, 0x09)
#define IUA_QPTM_RELS_IND	UA_MHDR(UA_VERSION, 0, UA_CLASS_QPTM, 0x10)

#endif				/* UA_USE_PROTOCOL(UA_PROTOCOL_IUA) */

/*
 *  M2UA-Specific Values
 *  -------------------------------------------------------------------
 */
#ifndef M2UA_PORT
#define M2UA_PORT	2904	/* M2UA well-known port number */
#endif				/* M2UA_PORT */

#ifndef M2UA_PPI
#define M2UA_PPI	2	/* M2UA SCTP Payload Protocol Identifier */
#endif				/* M2UA_PPI */

#define M2UA_VERSION	1	/* RFC 3331 */

#if UA_USE_PROTOCOL(UA_PROTOCOL_M2UA)

/*
 *  M2UA-Specific Messages: per draft-ietf-sigtran-m2ua-10.txt
 *  -------------------------------------------------------------------
 */
#define M2UA_MAUP_DATA		UA_MHDR(UA_VERSION, 0, UA_CLASS_MAUP, 0x01)
#define M2UA_MAUP_ESTAB_REQ	UA_MHDR(UA_VERSION, 0, UA_CLASS_MAUP, 0x02)
#define M2UA_MAUP_ESTAB_CON	UA_MHDR(UA_VERSION, 0, UA_CLASS_MAUP, 0x03)
#define M2UA_MAUP_REL_REQ	UA_MHDR(UA_VERSION, 0, UA_CLASS_MAUP, 0x04)
#define M2UA_MAUP_REL_CON	UA_MHDR(UA_VERSION, 0, UA_CLASS_MAUP, 0x05)
#define M2UA_MAUP_REL_IND	UA_MHDR(UA_VERSION, 0, UA_CLASS_MAUP, 0x06)
#define M2UA_MAUP_STATE_REQ	UA_MHDR(UA_VERSION, 0, UA_CLASS_MAUP, 0x07)
#define M2UA_MAUP_STATE_CON	UA_MHDR(UA_VERSION, 0, UA_CLASS_MAUP, 0x08)
#define M2UA_MAUP_STATE_IND	UA_MHDR(UA_VERSION, 0, UA_CLASS_MAUP, 0x09)
#define M2UA_MAUP_RETR_REQ	UA_MHDR(UA_VERSION, 0, UA_CLASS_MAUP, 0x0a)
#define M2UA_MAUP_RETR_CON	UA_MHDR(UA_VERSION, 0, UA_CLASS_MAUP, 0x0b)
#define M2UA_MAUP_RETR_IND	UA_MHDR(UA_VERSION, 0, UA_CLASS_MAUP, 0x0c)
#define M2UA_MAUP_RETR_COMP_IND	UA_MHDR(UA_VERSION, 0, UA_CLASS_MAUP, 0x0d)
#define M2UA_MAUP_CONG_IND	UA_MHDR(UA_VERSION, 0, UA_CLASS_MAUP, 0x0e)
#define M2UA_MAUP_DATA_ACK	UA_MHDR(UA_VERSION, 0, UA_CLASS_MAUP, 0x0f)
#define M2UA_MAUP_LAST		0x0f

/*
 *  M2UA-Specific Parameters: per draft-ietf-sigtran-m2ua-10.txt
 *  -------------------------------------------------------------------
 */
#define M2UA_PARM_DATA1		UA_CONST_PHDR(0x0300,0)	/* rfc3331 */
#define M2UA_PARM_DATA2		UA_CONST_PHDR(0x0301,0)	/* rfc3331 */
#define M2UA_PARM_STATE_REQUEST	UA_CONST_PHDR(0x0302,sizeof(uint32_t))	/* rfc3331 */
#define M2UA_PARM_STATE_EVENT	UA_CONST_PHDR(0x0303,sizeof(uint32_t))	/* rfc3331 */
#define M2UA_PARM_CONG_STATUS	UA_CONST_PHDR(0x0304,sizeof(uint32_t))	/* rfc3331 */
#define M2UA_PARM_DISC_STATUS	UA_CONST_PHDR(0x0305,sizeof(uint32_t))	/* rfc3331 */
#define M2UA_PARM_ACTION	UA_CONST_PHDR(0x0306,sizeof(uint32_t))	/* rfc3331 */
#define M2UA_PARM_SEQNO		UA_CONST_PHDR(0x0307,sizeof(uint32_t))	/* rfc3331 */
#define M2UA_PARM_RETR_RESULT	UA_CONST_PHDR(0x0308,sizeof(uint32_t))	/* rfc3331 */
#define M2UA_PARM_LINK_KEY	UA_CONST_PHDR(0x0309,sizeof(uint32_t)*6)	/* rfc3331 */
#define M2UA_PARM_LOC_KEY_ID	UA_CONST_PHDR(0x030a,sizeof(uint32_t))	/* rfc3331 */
#define M2UA_PARM_SDTI		UA_CONST_PHDR(0x030b,sizeof(uint32_t))	/* rfc3331 */
#define M2UA_PARM_SDLI		UA_CONST_PHDR(0x030c,sizeof(uint32_t))	/* rfc3331 */
#define M2UA_PARM_REG_RESULT	UA_CONST_PHDR(0x030d,sizeof(uint32_t))	/* rfc3331 */
#define M2UA_PARM_REG_STATUS	UA_CONST_PHDR(0x030e,sizeof(uint32_t))	/* rfc3331 */
#define M2UA_PARM_DEREG_RESULT	UA_CONST_PHDR(0x030f,sizeof(uint32_t)*4)	/* rfc3331 */
#define M2UA_PARM_DEREG_STATUS	UA_CONST_PHDR(0x0310,sizeof(uint32_t))	/* rfc3331 */
/* Note RFC 3331 uses the common corelation id parameter */
#define M2UA_PARM_CORR_ID	UA_CONST_PHDR(0x0311,sizeof(uint32_t))	/* m2ua-10, not rfc3331 */
#define M2UA_PARM_CORR_ID_ACK	UA_CONST_PHDR(0x0312,sizeof(uint32_t))	/* m2ua-10, not rfc3331 */

#define M2UA_ACTION_RTRV_BSN		(0x01)
#define M2UA_ACTION_RTRV_MSGS		(0x02)

#define M2UA_STATUS_LPO_SET		(0x00)
#define M2UA_STATUS_LPO_CLEAR		(0x01)
#define M2UA_STATUS_EMER_SET		(0x02)
#define M2UA_STATUS_EMER_CLEAR		(0x03)
#define M2UA_STATUS_FLUSH_BUFFERS	(0x04)
#define M2UA_STATUS_CONTINUE		(0x05)
#define M2UA_STATUS_CLEAR_RTB		(0x06)
#define M2UA_STATUS_AUDIT		(0x07)
#define M2UA_STATUS_CONG_CLEAR		(0x08)
#define M2UA_STATUS_CONG_ACCEPT		(0x09)
#define M2UA_STATUS_CONG_DISCARD	(0x0a)

#define M2UA_EVENT_RPO_ENTER		(0x01)
#define M2UA_EVENT_RPO_EXIT		(0x02)
#define M2UA_EVENT_LPO_ENTER		(0x03)
#define M2UA_EVENT_LPO_EXIT		(0x04)

#define M2UA_LEVEL_NONE			(0x00)
#define M2UA_LEVEL_1			(0x01)
#define M2UA_LEVEL_2			(0x02)
#define M2UA_LEVEL_3			(0x03)
#define M2UA_LEVEL_4			(0x04)	/* big argument */

#endif

/*
 *  M3UA-Specific Values
 *  -------------------------------------------------------------------
 */
#ifndef M3UA_PORT
#define M3UA_PORT	2905	/* M3UA well-known port number */
#endif				/* M3UA_PORT */

#ifndef M3UA_PPI
#define M3UA_PPI	3	/* M3UA SCTP Payload Protocol Identifier */
#endif				/* M3UA_PPI */

#define M3UA_VERSION	1	/* RFC 3332, RFC 4666 */

#if UA_USE_PROTOCOL(UA_PROTOCOL_M3UA)

/*
 *  M3UA-Specific Messages: per RFC 3332/RFC 4666
 *  -------------------------------------------------------------------
 */

/*
 *  M3UA-Specific Parameters: per RFC 3332/RFC 4666
 *  -------------------------------------------------------------------
 */

#define M3UA_PARM_NTWK_APP	UA_CONST_PHDR(0x0200,sizeof(uint32_t))
#define M3UA_PARM_PROT_DATA1	UA_CONST_PHDR(0x0201,0)
#define M3UA_PARM_PROT_DATA2	UA_CONST_PHDR(0x0202,0)
#define M3UA_PARM_AFFECT_DEST	UA_CONST_PHDR(0x0203,sizeof(uint32_t))
#define M3UA_PARM_USER_CAUSE	UA_CONST_PHDR(0x0204,sizeof(uint32_t))
#define M3UA_PARM_CONG_IND	UA_CONST_PHDR(0x0205,sizeof(uint32_t))
#define M3UA_PARM_CONCERN_DEST	UA_CONST_PHDR(0x0206,sizeof(uint32_t))
#define M3UA_PARM_ROUTING_KEY	UA_CONST_PHDR(0x0207,0)
#define M3UA_PARM_REG_RESULT	UA_CONST_PHDR(0x0208,sizeof(uint32_t))
#define M3UA_PARM_DEREG_RESULT	UA_CONST_PHDR(0x0209,sizeof(uint32_t)*4)
#define M3UA_PARM_LOC_KEY_ID	UA_CONST_PHDR(0x020a,sizeof(uint32_t))
#define M3UA_PARM_DPC		UA_CONST_PHDR(0x020b,0)
#define M3UA_PARM_SI		UA_CONST_PHDR(0x020c,0)
#define M3UA_PARM_SSN		UA_CONST_PHDR(0x020d,0)
#define M3UA_PARM_OPC		UA_CONST_PHDR(0x020e,0)
#define M3UA_PARM_CIC		UA_CONST_PHDR(0x020f,0)
#define M3UA_PARM_PROT_DATA3	UA_CONST_PHDR(0x0210,0)	/* proposed */
#define M3UA_PARM_REG_STATUS	UA_CONST_PHDR(0x0212,sizeof(uint32_t))
#define M3UA_PARM_DEREG_STATUS	UA_CONST_PHDR(0x0213,sizeof(uint32_t))

#endif				/* UA_USE_PROTOCOL(UA_PROTOCOL_M3UA) */

/*
 *  SUA-Specific Values
 *  -------------------------------------------------------------------
 */
#ifndef SUA_PORT
#define SUA_PORT	14001	/* SUA well-known port number */
#endif				/* SUA_PORT */

#ifndef SUA_PPI
#define SUA_PPI		4	/* SUA SCTP Payload Protocol Identifier */
#endif				/* SUA_PPI */

#define SUA_VERSION	1	/* RFC 3868 */

#if UA_USE_PROTOCOL(UA_PROTOCOL_SUA)

/*
 *  SUA-Specific Messages: per RFC 3868
 *  -------------------------------------------------------------------
 */

#define SUA_CLNS_CLDT		UA_MHDR(UA_VERSION, 0, UA_CLASS_CNLS, 0x01)
#define SUA_CLNS_CLDR		UA_MHDR(UA_VERSION, 0, UA_CLASS_CNLS, 0x02)

#define SUA_CONS_CORE		UA_MHDR(UA_VERSION, 0, UA_CLASS_CONS, 0x01)
#define SUA_CONS_COAK		UA_MHDR(UA_VERSION, 0, UA_CLASS_CONS, 0x02)
#define SUA_CONS_COREF		UA_MHDR(UA_VERSION, 0, UA_CLASS_CONS, 0x03)
#define SUA_CONS_RELRE		UA_MHDR(UA_VERSION, 0, UA_CLASS_CONS, 0x04)
#define SUA_CONS_RELCO		UA_MHDR(UA_VERSION, 0, UA_CLASS_CONS, 0x05)
#define SUA_CONS_RESCO		UA_MHDR(UA_VERSION, 0, UA_CLASS_CONS, 0x06)
#define SUA_CONS_RESRE		UA_MHDR(UA_VERSION, 0, UA_CLASS_CONS, 0x07)
#define SUA_CONS_CODT		UA_MHDR(UA_VERSION, 0, UA_CLASS_CONS, 0x08)
#define SUA_CONS_CODA		UA_MHDR(UA_VERSION, 0, UA_CLASS_CONS, 0x09)
#define SUA_CONS_COERR		UA_MHDR(UA_VERSION, 0, UA_CLASS_CONS, 0x10)
#define SUA_CONS_COIT		UA_MHDR(UA_VERSION, 0, UA_CLASS_CONS, 0x11)

/*
 *  SUA-Specific Parameters: per RFC 3868
 *  -------------------------------------------------------------------
 */

#define SUA_PARM_FLAGS		UA_CONST_PHDR(0x0101,sizeof(uint32_t))
#define SUA_PARM_HOP_COUNTER	UA_CONST_PHDR(0x0101,sizeof(uint32_t))	/* rfc3868 */
#define SUA_PARM_SRCE_ADDR	UA_CONST_PHDR(0x0102,sizeof(uint32_t)*5)	/* XXX */
#define SUA_PARM_DEST_ADDR	UA_CONST_PHDR(0x0103,0)
#define SUA_PARM_SRN		UA_CONST_PHDR(0x0104,sizeof(uint32_t))
#define SUA_PARM_DRN		UA_CONST_PHDR(0x0105,sizeof(uint32_t))
/* FIXME: Which one is it? */
#define SUA_PARM_CAUSE		UA_CONST_PHDR(0x0106,sizeof(uint32_t))
#define SUA_PARM_SEQ_NUM	UA_CONST_PHDR(0x0107,sizeof(uint32_t))
#define SUA_PARM_RX_SEQ_NUM	UA_CONST_PHDR(0x0108,sizeof(uint32_t))
#define SUA_PARM_ASP_CAPS	UA_CONST_PHDR(0x0109,sizeof(uint32_t))
#define SUA_PARM_CREDIT		UA_CONST_PHDR(0x010a,sizeof(uint32_t))
#define SUA_PARM_RESERVED1	UA_CONST_PHDR(0x010b,0)
#define SUA_PARM_DATA		UA_CONST_PHDR(0x010b,0)	/* rfc3868 */
#define SUA_PARM_SMI_SUBSYS	UA_CONST_PHDR(0x010c,sizeof(uint32_t))
/* FIXME: Which one is it? */
//#define SUA_PARM_CAUSE                UA_CONST_PHDR(0x010c,sizeof(uint32_t))
#define SUA_PARM_NTWK_APP	UA_CONST_PHDR(0x010d,sizeof(uint32_t))	/* rfc3868 */
#define SUA_PARM_ROUTING_KEY	UA_CONST_PHDR(0x010e,sizeof(uint32_t))	/* rfc3868 */
#define SUA_PARM_DRN_LABEL	UA_CONST_PHDR(0x010f,sizeof(uint32_t))	/* rfc3868 */
#define SUA_PARM_TID_LABEL	UA_CONST_PHDR(0x0110,sizeof(uint32_t))	/* rfc3868 */
#define SUA_PARM_ADDR_RANGE	UA_CONST_PHDR(0x0111,sizeof(uint32_t))	/* rfc3868 */
#define SUA_PARM_SMI		UA_CONST_PHDR(0x0112,sizeof(uint32_t))
#define SUA_PARM_IMPORTANCE	UA_CONST_PHDR(0x0113,sizeof(uint32_t))	/* rfc3868 */
#define SUA_PARM_MSG_PRIORITY	UA_CONST_PHDR(0x0114,sizeof(uint32_t))	/* rfc3868 */
#define SUA_PARM_PROTO_CLASS	UA_CONST_PHDR(0x0115,sizeof(uint32_t))	/* rfc3868 */
#define SUA_PARM_SEQ_CONTROL	UA_CONST_PHDR(0x0116,sizeof(uint32_t))	/* rfc3868 */
#define SUA_PARM_SEGMENTATION	UA_CONST_PHDR(0x0117,sizeof(uint32_t)	/* rfc3868 */
#define SUA_PARM_CONG		UA_CONST_PHDR(0x0118,sizeof(uint32_t))

#define SUA_SPARM_GT		UA_CONST_PHDR(0x8001,0)
#define SUA_SPARM_PC		UA_CONST_PHDR(0x8002,sizeof(uint32_t))
#define SUA_SPARM_SSN		UA_CONST_PHDR(0x8003,sizeof(uint32_t))
#define SUA_SPARM_IPV4_ADDR	UA_CONST_PHDR(0x8004,sizeof(uint32_t))
#define SUA_SPARM_HOSTNAME	UA_CONST_PHDR(0x8005,0)
#define SUA_SPARM_IPV6_ADDR	UA_CONST_PHDR(0x8006,sizeof(uint32_t)*4)

#endif				/* UA_USE_PROTOCOL(UA_PROTOCOL_SUA) */

/*
 *  TUA-Specific Values
 *  -------------------------------------------------------------------
 */
#ifndef TUA_PORT
#define TUA_PORT	14001	/* TUA well-known port number */
#endif				/* TUA_PORT */

#ifndef TUA_PPI
#define TUA_PPI		5	/* TUA SCTP Payload Protocol Identifier */
#endif				/* TUA_PPI */

#define TUA_VERSION	1	/* draft-bidulock-sigtran-tua-04.txt */

#if UA_USE_PROTOCOL(UA_PROTOCOL_TUA)

/*
 *  TUA-Specific Messages: per draft-bidulock-sigtran-tua-04.txt
 *  -------------------------------------------------------------------
 */

#define TUA_TDHM_TUNI		UA_MHDR(UA_VERSION, 0, UA_CLASS_TDHM, 0x01)
#define TUA_TDHM_TQRY		UA_MHDR(UA_VERSION, 0, UA_CLASS_TDHM, 0x02)
#define TUA_TDHM_TCNV		UA_MHDR(UA_VERSION, 0, UA_CLASS_TDHM, 0x03)
#define TUA_TDHM_TRSP		UA_MHDR(UA_VERSION, 0, UA_CLASS_TDHM, 0x04)
#define TUA_TDHM_TUAB		UA_MHDR(UA_VERSION, 0, UA_CLASS_TDHM, 0x05)
#define TUA_TDHM_TPAB		UA_MHDR(UA_VERSION, 0, UA_CLASS_TDHM, 0x06)
#define TUA_TDHM_TNOT		UA_MHDR(UA_VERSION, 0, UA_CLASS_TDHM, 0x07)

#define TUA_TCHM_CINV		UA_MHDR(UA_VERSION, 0, UA_CLASS_TCHM, 0x01)
#define TUA_TCHM_CRES		UA_MHDR(UA_VERSION, 0, UA_CLASS_TCHM, 0x02)
#define TUA_TCHM_CERR		UA_MHDR(UA_VERSION, 0, UA_CLASS_TCHM, 0x03)
#define TUA_TCHM_CREJ		UA_MHDR(UA_VERSION, 0, UA_CLASS_TCHM, 0x04)
#define TUA_TCHM_CCAN		UA_MHDR(UA_VERSION, 0, UA_CLASS_TCHM, 0x05)

/*
 *  TUA-Specific Parameters: per draft-bidulock-sigtran-tua-04.txt
 *  -------------------------------------------------------------------
 */

#define TUA_PARM_DIALOGUE_ID		UA_CONST_PHDR(0x0401,sizeof(uint32_t))
#define TUA_PARM_DIALOGUE_FLAGS		UA_CONST_PHDR(0x0402,sizeof(uint32_t))
#define TUA_PARM_QOS			UA_CONST_PHDR(0x0403,sizeof(uint32_t))
#define TUA_PARM_DEST_ADDR		UA_CONST_PHDR(0x0404,0)
#define TUA_PARM_ORIG_ADDR		UA_CONST_PHDR(0x0405,0)
#define TUA_PARM_APPL_CTXT		UA_CONST_PHDR(0x0406,0)
#define TUA_PARM_USER_INFO		UA_CONST_PHDR(0x0407,0)
#define TUA_PARM_SECU_CTXT		UA_CONST_PHDR(0x0408,0)
#define TUA_PARM_CONFIDENT		UA_CONST_PHDR(0x0409,0)
#define TUA_PARM_TERM			UA_CONST_PHDR(0x040a,sizeof(uint32_t))
#define TUA_PARM_REPT_CAUS		UA_CONST_PHDR(0x040c,sizeof(uint32_t))
#define TUA_PARM_ABORT_REASON		UA_CONST_PHDR(0x040d,sizeof(uint32_t))
#define TUA_PARM_COMPONENTS		UA_CONST_PHDR(0x040e,sizeof(uint32_t))
#define TUA_PARM_COMPONENT		UA_CONST_PHDR(0x040f,sizeof(uint32_t))
#define TUA_PARM_TRANSACTION_ID		UA_CONST_PHDR(0x0410,sizeof(uint32_t))

#define TUA_PARM_INVK_ID		UA_CONST_PHDR(0x0411,sizeof(uint32_t))
#define TUA_PARM_CORR_ID		UA_CONST_PHDR(0x0412,sizeof(uint32_t))
#define TUA_PARM_LAST_COMP		UA_CONST_PHDR(0x0413,sizeof(uint32_t))
#define TUA_PARM_OPCODE			UA_CONST_PHDR(0x0414,sizeof(uint32_t)*2)
#define TUA_PARM_PARMS			UA_CONST_PHDR(0x0415,0)
#define TUA_PARM_ERROR			UA_CONST_PHDR(0x0416,sizeof(uint32_t))
#define TUA_PARM_PBCODE			UA_CONST_PHDR(0x0417,sizeof(uint32_t))
#define TUA_PARM_TIMEOUT		UA_CONST_PHDR(0x0418,sizeof(uint32_t))

#define TUA_PARM_SSN			UA_CONST_PHDR(0x0419,sizeof(uint32_t))
#define TUA_PARM_SMI			UA_CONST_PHDR(0x041a,sizeof(uint32_t))
#define TUA_PARM_CONG			UA_CONST_PHDR(0x041b,sizeof(uint32_t))
#define TUA_PARM_CAUSE			UA_CONST_PHDR(0x041c,sizeof(uint32_t))
#define TUA_PARM_NTWK_APP		UA_CONST_PHDR(0x041d,sizeof(uint32_t))
#define TUA_PARM_ROUTING_KEY		UA_CONST_PHDR(0x041e,sizeof(uint32_t))
#define TUA_PARM_ADDR_RANGE		UA_CONST_PHDR(0x041f,sizeof(uint32_t))
#define TUA_PARM_DEST_TID		UA_CONST_PHDR(0x0420,sizeof(uint32_t))
#define TUA_PARM_ORIG_TID		UA_CONST_PHDR(0x0421,sizeof(uint32_t))
#define TUA_PARM_TID_RANGE		UA_CONST_PHDR(0x0422,sizeof(uint32_t))
#define TUA_PARM_GLOBAL_TITLE		UA_CONST_PHDR(0x0423,sizeof(uint32_t))
#define TUA_PARM_POINT_CODE		UA_CONST_PHDR(0x0424,sizeof(uint32_t))

#if 0
/* old values from draft-hou-sigtran-tua-00.txt */
#define TUA_PARM_QOS			UA_CONST_PHDR(0x0101,sizeof(uint32_t))
#define TUA_PARM_DEST_ADDR		UA_CONST_PHDR(0x0102,0)
#define TUA_PARM_ORIG_ADDR		UA_CONST_PHDR(0x0103,0)
#define TUA_PARM_APPL_CTXT		UA_CONST_PHDR(0x0104,0)
#define TUA_PARM_USER_INFO		UA_CONST_PHDR(0x0105,0)
#define TUA_PARM_COMP_PRES		UA_CONST_PHDR(0x0106,sizeof(uint32_t))
#define TUA_PARM_TERM			UA_CONST_PHDR(0x0107,sizeof(uint32_t))
#define TUA_PARM_P_ABORT		UA_CONST_PHDR(0x0108,sizeof(uint32_t))
#define TUA_PARM_REPT_CAUS		UA_CONST_PHDR(0x0109,sizeof(uint32_t))

#define TUA_PARM_INVK_ID		UA_CONST_PHDR(0x0201,sizeof(uint32_t))
#define TUA_PARM_LAST_COMP		UA_CONST_PHDR(0x0202,sizeof(uint32_t))
#define TUA_PARM_TIMEOUT		UA_CONST_PHDR(0x0203,sizeof(uint32_t))
#define TUA_PARM_OPCODE			UA_CONST_PHDR(0x0204,sizeof(uint32_t)*2)
#define TUA_PARM_PARMS			UA_CONST_PHDR(0x0205,0)
#define TUA_PARM_ERROR			UA_CONST_PHDR(0x0206,sizeof(uint32_t))
#define TUA_PARM_PBCODE			UA_CONST_PHDR(0x0207,sizeof(uint32_t))
#define TUA_PARM_CORR_ID		UA_CONST_PHDR(0x0208,sizeof(uint32_t))

#define TUA_PARM_SECU_CTXT		UA_CONST_PHDR(0x0301,0)
#define TUA_PARM_CONFIDENT		UA_CONST_PHDR(0x0302,0)
#define TUA_PARM_UABT_INFO		UA_CONST_PHDR(0x0303,0)

#define TUA_PARM_ABCODE			UA_CONST_PHDR(0x0401,sizeof(uint32_t))
#define TUA_PARM_CLASS			UA_CONST_PHDR(0x0402,sizeof(uint32_t))

#define TUA_PARM_CNV_TYPE		UA_CONST_PHDR(0x0503,sizeof(uint32_t))
#define TUA_PARM_INV_TYPE		UA_CONST_PHDR(0x0504,sizeof(uint32_t))
#define TUA_PARM_REJ_TYPE		UA_CONST_PHDR(0x0505,sizeof(uint32_t))
#define TUA_PARM_QRY_TYPE		UA_CONST_PHDR(0x0506,sizeof(uint32_t))
#endif

#endif				/* UA_USE_PROTOCOL(UA_PROTOCOL_TUA) */

/*
 *  ISUA-Specific Values
 *  -------------------------------------------------------------------------
 */
#ifndef ISUA_PORT
#define ISUA_PORT	14001	/* ISUA well-known port number */
#endif				/* ISUA_PORT */

#ifndef ISUA_PPI
#define ISUA_PPI	6	/* ISUA SCTP Payload Protocol Identifier */
#endif				/* ISUA_PPI */

#define ISUA_VERSION	1	/* draft-bidulock-sigtran-isua-03.txt */

#if UA_USE_PROTOCOL(UA_PROTOCOL_ISUA)

/*
 *  ISUA-Specific Messages: per draft-bidulock-sigtran-isua-03.txt
 *  -------------------------------------------------------------------------
 */

/* Call procesing messages */
#define ISUA_ISCP_CSET		UA_MHDR(UA_VERSION, 0, UA_CLASS_ISCP, 0x01)
#define ISUA_ISCP_CMOR		UA_MHDR(UA_VERSION, 0, UA_CLASS_ISCP, 0x02)
#define ISUA_ISCP_CTOT		UA_MHDR(UA_VERSION, 0, UA_CLASS_ISCP, 0x03)
#define ISUA_ISCP_CINF		UA_MHDR(UA_VERSION, 0, UA_CLASS_ISCP, 0x04)
#define ISUA_ISCP_CPRO		UA_MHDR(UA_VERSION, 0, UA_CLASS_ISCP, 0x05)
#define ISUA_ISCP_CALR		UA_MHDR(UA_VERSION, 0, UA_CLASS_ISCP, 0x06)
#define ISUA_ISCP_CPRG		UA_MHDR(UA_VERSION, 0, UA_CLASS_ISCP, 0x07)
#define ISUA_ISCP_CCON		UA_MHDR(UA_VERSION, 0, UA_CLASS_ISCP, 0x08)
#define ISUA_ISCP_CSUS		UA_MHDR(UA_VERSION, 0, UA_CLASS_ISCP, 0x09)
#define ISUA_ISCP_CRES		UA_MHDR(UA_VERSION, 0, UA_CLASS_ISCP, 0x0a)
#define ISUA_ISCP_CREA		UA_MHDR(UA_VERSION, 0, UA_CLASS_ISCP, 0x0b)
#define ISUA_ISCP_CERR		UA_MHDR(UA_VERSION, 0, UA_CLASS_ISCP, 0x0c)
#define ISUA_ISCP_CIBI		UA_MHDR(UA_VERSION, 0, UA_CLASS_ISCP, 0x0d)
#define ISUA_ISCP_CREL		UA_MHDR(UA_VERSION, 0, UA_CLASS_ISCP, 0x0e)
#define ISUA_ISCP_CRLC		UA_MHDR(UA_VERSION, 0, UA_CLASS_ISCP, 0x0f)

/* Circuit supervision messages */
#define ISUA_ISCS_CCNT		UA_MHDR(UA_VERSION, 0, UA_CLASS_ISCS, 0x01)
#define ISUA_ISCS_CLBK		UA_MHDR(UA_VERSION, 0, UA_CLASS_ISCS, 0x02)
#define ISUA_ISCS_CREP		UA_MHDR(UA_VERSION, 0, UA_CLASS_ISCS, 0x03)
#define ISUA_ISCS_CRSC		UA_MHDR(UA_VERSION, 0, UA_CLASS_ISCS, 0x04)
#define ISUA_ISCS_CRSA		UA_MHDR(UA_VERSION, 0, UA_CLASS_ISCS, 0x05)
#define ISUA_ISCS_CBLO		UA_MHDR(UA_VERSION, 0, UA_CLASS_ISCS, 0x06)
#define ISUA_ISCS_CBLA		UA_MHDR(UA_VERSION, 0, UA_CLASS_ISCS, 0x07)
#define ISUA_ISCS_CUBL		UA_MHDR(UA_VERSION, 0, UA_CLASS_ISCS, 0x08)
#define ISUA_ISCS_CUBA		UA_MHDR(UA_VERSION, 0, UA_CLASS_ISCS, 0x09)
#define ISUA_ISCS_CQRY		UA_MHDR(UA_VERSION, 0, UA_CLASS_ISCS, 0x0a)
#define ISUA_ISCS_CQRA		UA_MHDR(UA_VERSION, 0, UA_CLASS_ISCS, 0x0b)

/*
 *  ISUA-Specific Parameters: per draft-bidulock-sigtran-isua-03.txt
 *  -------------------------------------------------------------------------
 */

#endif				/* UA_USE_PROTOCOL(UA_PROTOCOL_ISUA) */

static struct ua_profile {
	uchar protocol;			/* locally defined protocol level */
	uchar version;			/* UA message header version */
	ushort port;			/* SCTP well-known port number, host byte order */
	uint ppi;			/* SCTP payload protocol identifier */
	uint optmask;			/* UA options mask */
} ua_profiles[] = {
	/* *INDENT-OFF* */
	[UA_PROFILE_NONE]	= { UA_PROTOCOL_M3UA, M3UA_VERSION, M3UA_PORT, M3UA_PPI, -1U, },
	[UA_PROFILE_RFC3057]	= { UA_PROTOCOL_IUA,   IUA_VERSION,  IUA_PORT,  IUA_PPI, -1U, },
	[UA_PROFILE_RFC3331]	= { UA_PROTOCOL_M2UA, M2UA_VERSION, M2UA_PORT, M2UA_PPI, -1U, },
	[UA_PROFILE_RFC3332]	= { UA_PROTOCOL_M3UA, M3UA_VERSION, M3UA_PORT, M3UA_PPI, -1U, },
	[UA_PROFILE_RFC3868]	= { UA_PROTOCOL_SUA,   SUA_VERSION,  SUA_PORT,  SUA_PPI, -1U, },
	[UA_PROFILE_RFC4233]	= { UA_PROTOCOL_IUA,   IUA_VERSION,  IUA_PORT,  IUA_PPI, -1U, },
	[UA_PROFILE_RFC4666]	= { UA_PROTOCOL_M3UA, M3UA_VERSION, M3UA_PORT, M3UA_PPI, -1U, },
	[UA_PROFILE_TS102141]	= { UA_PROTOCOL_M2UA, M2UA_VERSION, M2UA_PORT, M2UA_PPI, -1U, },
	[UA_PROFILE_TS102142]	= { UA_PROTOCOL_M3UA, M3UA_VERSION, M3UA_PORT, M3UA_PPI, -1U, },
	[UA_PROFILE_TS102143]	= { UA_PROTOCOL_SUA,   SUA_VERSION,  SUA_PORT,  SUA_PPI, -1U, },
	[UA_PROFILE_SUABIS]	= { UA_PROTOCOL_SUA,   SUA_VERSION,  SUA_PORT,  SUA_PPI, -1U, },
	[UA_PROFILE_TUA05]	= { UA_PROTOCOL_TUA,   TUA_VERSION,  TUA_PORT,  TUA_PPI, -1U, },
	[UA_PROFILE_ISUA04]	= { UA_PROTOCOL_ISUA, ISUA_VERSION, ISUA_PORT, ISUA_PPI, -1U, },
	/* *INDENT-ON* */
};

struct ua_parm {
	uint32_t tag;			/* tag for parameter */
	union {
		uchar *cp;		/* pointer to parameter value field */
		uint32_t *wp;		/* pointer to parameter value field */
	};
	size_t len;			/* length of parameter value field */
	uint32_t val;			/* value of first 4 bytes (host order) */
};

/**
 * ua_dec_parm: - extract parameter from message or field.
 * @beg: beginning of field or message
 * @end: end of field or message
 * @parm: structure to hold result
 * @tags: array of tags to match
 * @num: number of tags in array
 *
 * If the parameter does not exist in the field or message it returns false; otherwise true, and
 * sets the parameter values if parm is non-NULL.
 *
 * A couple of problems with this function: it does not check if UA_SIZE is zero or less than
 * UA_PHDR_SIZE, both of which are syntax errors (well, a parameter of length zero and tag value
 * zero can be considered excessive padding).  It does not check if UA_SIZE for a given parameter
 * exceeds the given length, which is another syntax error.  The function should return an integer
 * value, zero on success, and an informative negative error number on failure.
 *
 * OTOH, do we really care if there is a syntax error in the message, provided that the necessary
 * information can be obtained?  No, probably not.  We can be forgiving.
 */
static inline bool
ua_dec_parm_any(uchar *beg, uchar *end, struct ua_parm *parm, uint32_t *tags, uint num)
{
	register uint32_t *p;
	int plen, i;

	(void) ua_profiles;
	for (p = (uint32_t *) beg;
	     (uchar *) (p + 1) <= end && (plen = UA_SIZE(*p)) >= UA_PHDR_SIZE;
	     p = (uint32_t *) ((uchar *) p + UA_PAD4(plen))) {
		for (i = 0; i < num; i++) {
			if (UA_TAG(p[0]) == UA_TAG(tags[i])) {
				if (parm) {
					parm->tag = tags[i];
					parm->wp = (p + 1);
					if ((parm->len = plen - UA_PHDR_SIZE) >= 4)
						parm->val = ntohl(p[1]);
					else
						parm->val = 0;
				}
				return (true);
			}
		}
	}
	return (false);
}

/**
 * ua_dec_parm_any_next: - extract next parameter from message or field.
 * @end: end of field or message
 * @parm: structure to hold result (as passed to ua_dec_parm)
 * @tags: array of tags to match
 * @num: number of tags in array
 */
static inline bool
ua_dec_parm_any_next(uchar *end, struct ua_parm *parm, uint32_t *tags, uint num)
{
	return ua_dec_parm_any(parm->cp + UA_PAD4(parm->len), end, parm, tags, num);
}

/**
 * ua_dec_parm: - extract parameter from message or field.
 * @beg: beginning of field or message
 * @end: end of field or message
 * @parm: structure to hold result
 * @tag: parameter tag value
 */
static inline bool
ua_dec_parm(uchar *beg, uchar *end, struct ua_parm *parm, uint32_t tag)
{
	return ua_dec_parm_any(beg, end, parm, &tag, 1);
}

/**
 * ua_dec_parm_next: - extract next parameter from message or field.
 * @end: end of field or message
 * @parm: structure to hold result (as passed to ua_dec_parm)
 */
static inline bool
ua_dec_parm_next(uchar *end, struct ua_parm *parm)
{
	return ua_dec_parm(parm->cp + UA_PAD4(parm->len), end, parm, parm->tag);
}

#endif				/* __LOCAL_UA_MSG_H__ */
