/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

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

 $Log$
 *****************************************************************************/

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

/*
 *  This is the AS side of M2UA implemented as a pushable module that pushes over an SCTP NPI
 *  stream.
 */

#define _LFS_SOURCE 1
#define _DEBUG 1

#include <sys/os7/compat.h>

#include <stdbool.h>

#include <linux/socket.h>
#include <net/ip.h>

#include <sys/npi.h>
#include <sys/npi_sctp.h>

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/sdli.h>
#include <ss7/sdli_ioctl.h>
#include <ss7/sdti.h>
#include <ss7/sdti_ioctl.h>
#include <ss7/sli.h>
#include <ss7/sli_ioctl.h>

#define M2UA_AS_DESCRIP		"M2UA/SCTP SIGNALLING LINK (SL) STREAMS MODULE."
#define M2UA_AS_REVISION	"OpenSS7 $RCSfile$ $Name$($Revision$) $Date$"
#define M2UA_AS_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define M2UA_AS_DEVICE		"Part of the OpenSS7 Stack for Linux Fast STREAMS."
#define M2UA_AS_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define M2UA_AS_LICENSE		"GPL"
#define M2UA_AS_BANNER		M2UA_AS_DESCRIP		"\n" \
				M2UA_AS_REVISION	"\n" \
				M2UA_AS_COPYRIGHT	"\n" \
				M2UA_AS_DEVICE		"\n" \
				M2UA_AS_CONTACT		"\n"
#define M2UA_AS_SPLASH		M2UA_AS_DEVICE		" - " \
				M2UA_AS_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(M2UA_AS_CONTACT);
MODULE_DESCRIPTION(M2UA_AS_DESCRIP);
MODULE_SUPPORTED_DEVICE(M2UA_AS_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(M2UA_AS_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-m2ua_as");
#endif
#endif				/* LINUX */

#ifdef LFS
#define M2UA_AS_MOD_ID		CONFIG_STREAMS_M2UA_AS_MODID
#define M2UA_AS_MOD_NAME	CONFIG_STREAMS_M2UA_AS_NAME
#endif

#define MOD_ID	    M2UA_AS_MOD_ID
#define MOD_NAME    M2UA_AS_MOD_NAME
#ifdef MODULE
#define MOD_SPLASH  M2UA_AS_BANNER
#else				/* MODULE */
#define MOD_SPLASH  M2UA_AS_SPLASH
#endif				/* MODULE */

STATIC struct module_info sl_minfo = {
	.mi_idnum = MOD_ID,		/* Module ID number */
	.mi_idname = MOD_NAME,		/* Module name */
	.mi_minpsz = STRMINPSZ,		/* Min packet size accepted */
	.mi_maxpsz = STRMAXPSZ,		/* Max packet size accepted */
	.mi_hiwat = STRHIGH,		/* Hi water mark */
	.mi_lowat = STRLOW,		/* Lo water mark */
};

/*
 *  M2UA-AS Private Structure
 */
struct sl {
	STR_DECLARATION (struct sl);	/* stream declaration */
	int n_request;			/* npi request */
	int n_state;			/* npi state */
	int coninds;			/* npi connection indications */
	lmi_option_t option;		/* protocol and variant options */
	struct {
		sl_timers_t timers;	/* SL protocol timers */
		sl_config_t config;	/* SL configuration options */
		sl_statem_t statem;	/* state machine variables */
		sl_notify_t notify;	/* SL notification options */
		sl_stats_t stats;	/* SL statistics */
		sl_stats_t stamp;	/* SL statistics timestamps */
		sl_stats_t statsp;	/* SL statistics periods */
	} sl;
	struct {
		sdt_timers_t timers;	/* SDT timers */
		sdt_config_t config;	/* SDT configuration options */
	} sdt;
	struct {
		sdl_timers_t timers;	/* SDL timers */
		sdl_config_t config;	/* SDL configuration options */
	} sdl;
	struct {
		N_info_ack_t n;		/* NPI information */
		lmi_info_ack_t lm;	/* LMI information */
	} info;
	size_t loc_len;			/* local address length */
	size_t rem_len;			/* remote address length */
	struct sockaddr_in loc[8];	/* up to 8 local addresses */
	struct sockaddr_in rem[8];	/* up to 8 remote addresses */
};

#define SL_PRIV(__q) ((struct sl *)(__q)->q_ptr)

STATIC struct sl *sl_alloc_priv(queue_t *q, struct sl **slp, dev_t *devp, cred_t *crp);
STATIC void sl_free_priv(queue_t *q);
STATIC struct sl *sl_get(struct sl *sl);
STATIC void sl_put(struct sl *sl);

#if 0
#define NSF_UNBND	(1<<NS_UNBND)
#define NSF_WACK_BREQ	(1<<NS_WACK_BREQ)
#define NSF_WACK_UREQ	(1<<NS_WACK_UREQ)
#define NSF_IDLE	(1<<NS_IDLE)
#define NSF_WACK_OPTREQ	(1<<NS_WACK_OPTREQ)
#define NSF_WACK_RRES	(1<<NS_WACK_RRES)
#define NSF_WCON_CREQ	(1<<NS_WCON_CREQ)
#define NSF_WRES_CIND	(1<<NS_WRES_CIND)
#define NSF_WACK_CRES	(1<<NS_WACK_CRES)
#define NSF_DATA_XFER	(1<<NS_DATA_XFER)
#define NSF_WCON_RREQ	(1<<NS_WCON_RREQ)
#define NSF_WRES_RIND	(1<<NS_WRES_RIND)
#define NSF_WACK_DREQ6	(1<<NS_WACK_DREQ6)
#define NSF_WACK_DREQ7	(1<<NS_WACK_DREQ7)
#define NSF_WACK_DREQ9	(1<<NS_WACK_DREQ9)
#define NSF_WACK_DREQ10	(1<<NS_WACK_DREQ10)
#define NSF_WACK_DREQ11	(1<<NS_WACK_DREQ11)
#endif

#ifdef _DEBUG
const char *
sl_n_state_name(int state)
{
	switch (state) {
	case NS_UNBND:		/* NS user not bound to network address */
		return ("NS_UNBND");
	case NS_WACK_BREQ:	/* Awaiting acknowledgement of N_BIND_REQ */
		return ("NS_WACK_BREQ");
	case NS_WACK_UREQ:	/* Pending acknowledgement for N_UNBIND_REQ */
		return ("NS_WACK_UREQ");
	case NS_IDLE:		/* Idle, no connection */
		return ("NS_IDLE");
	case NS_WACK_OPTREQ:	/* Pending acknowledgement of N_OPTMGMT_REQ */
		return ("NS_WACK_OPTREQ");
	case NS_WACK_RRES:	/* Pending acknowledgement of N_RESET_RES */
		return ("NS_WACK_RRES");
	case NS_WCON_CREQ:	/* Pending confirmation of N_CONN_REQ */
		return ("NS_WCON_CREQ");
	case NS_WRES_CIND:	/* Pending response of N_CONN_REQ */
		return ("NS_WRES_CIND");
	case NS_WACK_CRES:	/* Pending acknowledgement of N_CONN_RES */
		return ("NS_WACK_CRES");
	case NS_DATA_XFER:	/* Connection-mode data transfer */
		return ("NS_DATA_XFER");
	case NS_WCON_RREQ:	/* Pending confirmation of N_RESET_REQ */
		return ("NS_WCON_RREQ");
	case NS_WRES_RIND:	/* Pending response of N_RESET_IND */
		return ("NS_WRES_RIND");
	case NS_WACK_DREQ6:	/* Waiting ack of N_DISCON_REQ */
		return ("NS_WACK_DREQ6");
	case NS_WACK_DREQ7:	/* Waiting ack of N_DISCON_REQ */
		return ("NS_WACK_DREQ7");
	case NS_WACK_DREQ9:	/* Waiting ack of N_DISCON_REQ */
		return ("NS_WACK_DREQ9");
	case NS_WACK_DREQ10:	/* Waiting ack of N_DISCON_REQ */
		return ("NS_WACK_DREQ10");
	case NS_WACK_DREQ11:	/* Waiting ack of N_DISCON_REQ */
		return ("NS_WACK_DREQ11");
	default:
		return ("(unknown)");
	}
}
STATIC int
sl_set_n_state(struct sl *sl, int newstate)
{
	int oldstate = sl->n_state;

	printd(("%s: %p: %s <- %s\n", MOD_NAME, sl, sl_n_state_name(oldstate),
		sl_n_state_name(newstate)));
	return ((sl->n_state = newstate));
}
#else				/* _DEBUG */
STATIC int
sl_set_n_state(struct sl *sl, int newstate)
{
	return ((sl->n_state = newstate));
}
#endif				/* _DEBUG */
STATIC INLINE int
sl_get_n_state(struct sl *sl)
{
	return (sl->n_state);
}
STATIC INLINE int
sl_get_n_statef(struct sl *sl)
{
	return ((1 << sl_get_n_state(sl)));
}
STATIC INLINE int
sl_chk_n_state(struct sl *sl, int mask)
{
	return (sl_get_n_statef(sl) & mask);
}
STATIC INLINE int
sl_not_n_state(struct sl *sl, int mask)
{
	return (sl_chk_n_state(sl, ~mask));
}

#define LMF_UNATTACHED	    (1<<LMI_UNATTACHED)
#define LMF_ATTACH_PENDING  (1<<LMI_ATTACH_PENDING)
#define LMF_UNUSABLE	    (1<<LMI_UNUSABLE)
#define LMF_DISABLED	    (1<<LMI_DISABLED)
#define LMF_ENABLE_PENDING  (1<<LMI_ENABLE_PENDING)
#define LMF_ENABLED	    (1<<LMI_ENABLED)
#define LMF_DISABLE_PENDING (1<<LMI_DISABLE_PENDING)
#define LMF_DETACH_PENDING  (1<<LMI_DETACH_PENDING)

#ifdef _DEBUG
const char *
sl_i_state_name(int state)
{
	switch (state) {
	case LMI_UNATTACHED:
		return ("LMI_UNATTACHED");
	case LMI_ATTACH_PENDING:
		return ("LMI_ATTACH_PENDING");
	case LMI_UNUSABLE:
		return ("LMI_UNUSABLE");
	case LMI_DISABLED:
		return ("LMI_DISABLED");
	case LMI_ENABLE_PENDING:
		return ("LMI_ENABLE_PENDING");
	case LMI_ENABLED:
		return ("LMI_ENABLED");
	case LMI_DISABLE_PENDING:
		return ("LMI_DISABLE_PENDING");
	case LMI_DETACH_PENDING:
		return ("LMI_DETACH_PENDING");
	default:
		return ("(unknown)");
	}
}
STATIC int
sl_set_i_state(struct sl *sl, int newstate)
{
	int oldstate = sl->i_state;

	printd(("%s: %p: %s <- %s\n", MOD_NAME, sl, sl_i_state_name(newstate),
		sl_i_state_name(oldstate)));
	return ((sl->info.lm.lmi_state = sl->i_state = newstate));
}
#else				/* _DEBUG */
STATIC INLINE int
sl_set_i_state(struct sl *sl, int newstate)
{
	return ((sl->i_state = newstate));
}
#endif				/* _DEBUG */
STATIC INLINE int
sl_get_i_state(struct sl *sl)
{
	return sl->i_state;
}
STATIC INLINE int
sl_get_i_statef(struct sl *sl)
{
	return (1 << sl_get_i_state(sl));
}
STATIC INLINE int
sl_chk_i_state(struct sl *sl, int mask)
{
	return (sl_get_i_statef(sl) & mask);
}
STATIC INLINE int
sl_not_i_state(struct sl *sl, int mask)
{
	return (sl_chk_i_state(sl, ~mask));
}

/*
 *  M2UA Message Definitions
 */

#define M2UA_PPI    5

#define UA_VERSION  1
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
#define UA_CLASS_Q921	0x05	/* Q.931 User Part Messages */
#define UA_CLASS_MAUP	0x06	/* M2UA Messages */
#define UA_CLASS_CNLS	0x07	/* SUA Connectionless Messages */
#define UA_CLASS_CONS	0x08	/* SUA Connection Oriented Messages */
#define UA_CLASS_RKMM	0x09	/* Routing Key Management Messages */
#define UA_CLASS_TDHM	0x0a	/* TUA Dialog Handling Mesages */
#define UA_CLASS_TCHM	0x0b	/* TUA Component Handling Messages */

/*
 *  MESSAGES DEFINED IN EACH CLASS:-
 */
#define UA_MGMT_ERR		UA_MHDR(UA_VERSION, 0, UA_CLASS_MGMT, 0x00)
#define UA_MGMT_NTFY		UA_MHDR(UA_VERSION, 0, UA_CLASS_MGMT, 0x01)
#define UA_MGMT_LAST		0x01

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

#define UA_TAG_MASK		(__constant_htonl(0xffff0000))
#define UA_PTAG(__phdr)		((htonl(__phdr)>>16)&0xffff)
#define UA_PLEN(__phdr)		(htonl(__phdr)&0xffff)
#define UA_SIZE(__phdr)		(__constant_htonl(__phdr)&0xffff)
#define UA_TAG(__phdr)		((__constant_htonl(__phdr)>>16)&0xffff)
#define UA_PHDR(__phdr, __length) \
	(htonl(((__phdr)<<16)|((__length)+sizeof(uint32_t))))
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
#define UA_PARM_IID_RANGE	UA_CONST_PHDR(0x0002,0)	/* m2ua-10 */
#define UA_PARM_DATA		UA_CONST_PHDR(0x0003,0)	/* sua-07 */
#define UA_PARM_IID_TEXT	UA_CONST_PHDR(0x0003,0)
#define UA_PARM_INFO		UA_CONST_PHDR(0x0004,0)
#define UA_PARM_APC		UA_CONST_PHDR(0x0005,sizeof(uint32_t))	/* sua-07 */
#define UA_PARM_DLCI		UA_CONST_PHDR(0x0005,sizeof(uint32_t))	/* rfc3057 */
#define UA_PARM_RC		UA_CONST_PHDR(0x0006,sizeof(uint32_t))
#define UA_PARM_DIAG		UA_CONST_PHDR(0x0007,0)
#define UA_PARM_IID_RANGE1	UA_CONST_PHDR(0x0008,0)	/* rfc3057 */
#define UA_PARM_HBDATA		UA_CONST_PHDR(0x0009,0)
#define UA_PARM_REASON		UA_CONST_PHDR(0x000a,sizeof(uint32_t))
#define UA_PARM_TMODE		UA_CONST_PHDR(0x000b,sizeof(uint32_t))
#define UA_PARM_ECODE		UA_CONST_PHDR(0x000c,sizeof(uint32_t))
#define UA_PARM_STATUS		UA_CONST_PHDR(0x000d,sizeof(uint32_t))
#define UA_PARM_ASPID		UA_CONST_PHDR(0x000e,sizeof(uint32_t))
#define UA_PARM_PROT_DATA	UA_CONST_PHDR(0x000e,sizeof(uint32_t))	/* rfc3057 */
#define UA_PARM_CONG_LEVEL	UA_CONST_PHDR(0x000f,sizeof(uint32_t))	/* sua-07 */
#define UA_PARM_REL_REASON	UA_CONST_PHDR(0x000f,sizeof(uint32_t))	/* rfc3057 */
#define UA_PARM_TEI_STATUS	UA_CONST_PHDR(0x0010,sizeof(uint32_t))	/* rfc3057 */

/*
 *  Somewhat common field values:
 */
#define   UA_ECODE_INVALID_VERSION		(0x01)
#define   UA_ECODE_INVALID_IID			(0x02)
#define   UA_ECODE_UNSUPPORTED_MESSAGE_CLASS	(0x03)
#define   UA_ECODE_UNSUPPORTED_MESSAGE_TYPE	(0x04)
#define   UA_ECODE_UNSUPPORTED_TRAFFIC_MODE	(0x05)
#define   UA_ECODE_UNEXPECTED_MESSAGE		(0x06)
#define   UA_ECODE_PROTOCOL_ERROR		(0x07)
#define   UA_ECODE_UNSUPPORTED_IID_TYPE		(0x08)
#define   UA_ECODE_INVALID_STREAM_IDENTIFIER	(0x09)
#define  IUA_ECODE_UNASSIGNED_TEI		(0x0a)
#define  IUA_ECODE_UNRECOGNIZED_SAPI		(0x0b)
#define  IUA_ECODE_INVALID_TEI_SAPI_COMBINATION	(0x0c)
#define   UA_ECODE_REFUSED_MANAGEMENT_BLOCKING	(0x0d)
#define   UA_ECODE_ASPID_REQUIRED		(0x0e)
#define   UA_ECODE_INVALID_ASPID		(0x0f)
#define M2UA_ECODE_ASP_ACTIVE_FOR_IIDS		(0x10)
#define   UA_ECODE_INVALID_PARAMETER_VALUE	(0x11)
#define   UA_ECODE_PARAMETER_FIELD_ERROR	(0x12)
#define   UA_ECODE_UNEXPECTED_PARAMETER		(0x13)
#define   UA_ECODE_DESTINATION_STATUS_UNKNOWN	(0x14)
#define   UA_ECODE_INVALID_NETWORK_APPEARANCE	(0x15)
#define   UA_ECODE_MISSING_PARAMETER		(0x16)
#define   UA_ECODE_ROUTING_KEY_CHANGE_REFUSED	(0x17)
#define   UA_ECODE_INVALID_ROUTING_CONTEXT	(0x19)
#define   UA_ECODE_NO_CONFIGURED_AS_FOR_ASP	(0x1a)
#define   UA_ECODE_SUBSYSTEM_STATUS_UNKNOWN	(0x1b)

#define UA_STATUS_AS_DOWN			(0x00010001)
#define UA_STATUS_AS_INACTIVE			(0x00010002)
#define UA_STATUS_AS_ACTIVE			(0x00010003)
#define UA_STATUS_AS_PENDING			(0x00010004)
#define UA_STATUS_AS_INSUFFICIENT_ASPS		(0x00020001)
#define UA_STATUS_ALTERNATE_ASP_ACTIVE		(0x00020002)
#define UA_STATUS_ASP_FAILURE			(0x00020003)
#define UA_STATUS_AS_MINIMUM_ASPS		(0x00020004)

#define UA_TMODE_OVERRIDE			(0x1)
#define UA_TMODE_LOADSHARE			(0x2)
#define UA_TMODE_BROADCAST			(0x3)
#define UA_TMODE_SB_OVERRIDE			(0x4)
#define UA_TMODE_SB_LOADSHARE			(0x5)
#define UA_TMODE_SB_BROADCAST			(0x6)

#define UA_RESULT_SUCCESS			(0x00)
#define UA_RESULT_FAILURE			(0x01)

/*
 *  M2UA-Specific Messages: per draft-ietf-sigtran-m2ua-10.txt
 *  -------------------------------------------------------------------
 */
#define M2UA_MAUP_DATA		UA_MHDR(1, 0, UA_CLASS_MAUP, 0x01)
#define M2UA_MAUP_ESTAB_REQ	UA_MHDR(1, 0, UA_CLASS_MAUP, 0x02)
#define M2UA_MAUP_ESTAB_CON	UA_MHDR(1, 0, UA_CLASS_MAUP, 0x03)
#define M2UA_MAUP_REL_REQ	UA_MHDR(1, 0, UA_CLASS_MAUP, 0x04)
#define M2UA_MAUP_REL_CON	UA_MHDR(1, 0, UA_CLASS_MAUP, 0x05)
#define M2UA_MAUP_REL_IND	UA_MHDR(1, 0, UA_CLASS_MAUP, 0x06)
#define M2UA_MAUP_STATE_REQ	UA_MHDR(1, 0, UA_CLASS_MAUP, 0x07)
#define M2UA_MAUP_STATE_CON	UA_MHDR(1, 0, UA_CLASS_MAUP, 0x08)
#define M2UA_MAUP_STATE_IND	UA_MHDR(1, 0, UA_CLASS_MAUP, 0x09)
#define M2UA_MAUP_RETR_REQ	UA_MHDR(1, 0, UA_CLASS_MAUP, 0x0a)
#define M2UA_MAUP_RETR_CON	UA_MHDR(1, 0, UA_CLASS_MAUP, 0x0b)
#define M2UA_MAUP_RETR_IND	UA_MHDR(1, 0, UA_CLASS_MAUP, 0x0c)
#define M2UA_MAUP_RETR_COMP_IND	UA_MHDR(1, 0, UA_CLASS_MAUP, 0x0d)
#define M2UA_MAUP_CONG_IND	UA_MHDR(1, 0, UA_CLASS_MAUP, 0x0e)
#define M2UA_MAUP_DATA_ACK	UA_MHDR(1, 0, UA_CLASS_MAUP, 0x0f)
#define M2UA_MAUP_LAST		0x0f

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

/*
 *  =========================================================================
 *
 *  OUTPUT Events
 *
 *  =========================================================================
 */
/*
 *  -------------------------------------------------------------------------
 *
 *  N Provider -> N User Primitives
 *
 *  -------------------------------------------------------------------------
 */
#if 0
/**
 * n_error_ack: - issue an N_ERROR_ACK primitive upstream
 * @sl: private structure
 * @q: active queue
 * @prim: primitive in error
 * @err: positive NPI or negative UNIX error
 */
static int
n_error_ack(struct sl *sl, queue_t *q, int prim, int err)
{
	N_error_ack_t *p;
	mblk_t *mp;

	if (likely((mp = ss7_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_ERROR_ACK;
		p->ERROR_prim = prim;
		p->NPI_error = err > 0 ? err : NSYSERR;
		p->UNIX_error = err < 0 ? -err : 0;
		mp->b_wptr += sizeof(*p);
		printd(("%s: %p: <- N_ERROR_ACK\n", MOD_NAME, sl));
		/* no state change */
		putnext(sl->oq, mp);
		return (0);
	}
	return (-EBUSY);
}
#endif

static int
n_pass_along(struct sl *sl, queue_t *q, mblk_t *mp, int npistate, int lmistate)
{
	if (mp->b_datap->db_type < QPCTL && !bcanputnext(q, mp->b_band))
		goto busy;
	sl_set_n_state(sl, npistate);
	sl_set_i_state(sl, lmistate);
	sl->n_request = false;
	putnext(q, mp);
	return (QR_ABSORBED);
      busy:
	return (-EBUSY);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SL Provider (M2PA) -> SL User Primitives
 *
 *  -------------------------------------------------------------------------
 */
/**
 * lmi_info_ack: - issue an LMI_INFO_ACK prmitive upstream
 * @sl: private structure
 * @q: active queue
 */
static int
lmi_info_ack(struct sl *sl, queue_t *q)
{
	lmi_info_ack_t *p;
	mblk_t *mp;

	if (likely((mp = ss7_allocb(q, sizeof(*p) + sl->loc_len, BPRI_MED)) != NULL)) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_INFO_ACK;
		p->lmi_version = sl->info.lm.lmi_version;
		p->lmi_state = sl->info.lm.lmi_state;
		p->lmi_max_sdu = sl->info.lm.lmi_max_sdu;
		p->lmi_min_sdu = sl->info.lm.lmi_min_sdu;
		p->lmi_header_len = sl->info.lm.lmi_header_len;
		p->lmi_ppa_style = sl->info.lm.lmi_ppa_style;
		mp->b_wptr += sizeof(*p);
		bcopy(&sl->loc, mp->b_wptr, sl->loc_len);
		mp->b_wptr += sl->loc_len;
		printd(("%s: %p: <- LMI_INFO_ACK\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_ok_ack: - issue an LMI_OK_ACK prmitive upstream
 * @sl: private structure
 * @q: active queue
 * @prim: correct primitive
 */
static int
lmi_ok_ack(struct sl *sl, queue_t *q, lmi_long prim)
{
	lmi_ok_ack_t *p;
	mblk_t *mp;

	if (likely((mp = ss7_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_OK_ACK;
		p->lmi_correct_primitive = prim;
		switch (sl_get_i_state(sl)) {
		case LMI_ATTACH_PENDING:
			p->lmi_state = LMI_DISABLED;
			sl_set_i_state(sl, LMI_DISABLED);
			sl_set_n_state(sl, NS_IDLE);
			break;
		case LMI_DETACH_PENDING:
			p->lmi_state = LMI_UNATTACHED;
			sl_set_i_state(sl, LMI_UNATTACHED);
			sl_set_n_state(sl, NS_UNBND);
			break;
		default:
			/* default is don't change state */
			p->lmi_state = sl_get_i_state(sl);
			break;
		}
		mp->b_wptr += sizeof(*p);
		printd(("%s: %p: <- LMI_OK_ACK\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_error_ack: - issue an LMI_ERROR_ACK prmitive upstream
 * @sl: private structure
 * @q: active queue
 * @prim: primitive in error
 * @err: positive LMI error or negative UNIX error
 */
STATIC int
lmi_error_ack(struct sl *sl, queue_t *q, lmi_long prim, lmi_long err)
{
	lmi_error_ack_t *p;
	mblk_t *mp;

	if (likely((mp = ss7_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_ERROR_ACK;
		p->lmi_errno = err < 0 ? -err : 0;
		p->lmi_reason = err < 0 ? LMI_SYSERR : err;
		p->lmi_error_primitive = prim;
		switch (sl_get_i_state(sl)) {
		case LMI_ATTACH_PENDING:
			p->lmi_state = LMI_UNATTACHED;
			sl_set_i_state(sl, LMI_UNATTACHED);
			sl_set_n_state(sl, NS_UNBND);
			break;
		case LMI_DETACH_PENDING:
		case LMI_ENABLE_PENDING:
			p->lmi_state = LMI_DISABLED;
			sl_set_i_state(sl, LMI_DISABLED);
			sl_set_n_state(sl, NS_IDLE);
			break;
		case LMI_DISABLE_PENDING:
			p->lmi_state = LMI_ENABLED;
			sl_set_i_state(sl, LMI_ENABLED);
			switch (sl_get_n_state(sl)) {
			case NS_WACK_DREQ6:
				sl_set_n_state(sl, NS_WCON_CREQ);
				break;
			case NS_WACK_DREQ7:
				sl_set_n_state(sl, NS_WRES_CIND);
				break;
			case NS_WACK_DREQ9:
				sl_set_n_state(sl, NS_DATA_XFER);
				break;
			case NS_WACK_DREQ10:
				sl_set_n_state(sl, NS_WCON_RREQ);
				break;
			case NS_WACK_DREQ11:
				sl_set_n_state(sl, NS_WRES_RIND);
				break;
			default:
				/* Default is not to change state. */
				swerr();
				break;
			}
			break;
		}
		mp->b_wptr += sizeof(*p);
		printd(("%s: %p: <- LMI_ERROR_ACK\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_error_reply: - repy with an LMI_ERROR_ACK prmitive if necessary
 * @sl: private structure
 * @q: active queue
 * @prim: primitive in error
 * @err: positive LMI error or negative UNIX error
 */
static int
lmi_error_reply(struct sl *sl, queue_t *q, lmi_long prim, lmi_long err)
{
	switch (err) {
	case QR_DONE:
	case QR_ABSORBED:
	case QR_TRIMMED:
	case QR_LOOP:
	case QR_PASSALONG:
	case QR_PASSFLOW:
	case QR_DISABLE:
		ptrace(("%s: %p: WARNING: Shouldn't pass Q returns to m_error_reply\n",
			MOD_NAME, sl));
	case -EBUSY:
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
		return (err);
	}
	return lmi_error_ack(sl, q, prim, err);
}

/**
 * lmi_enable_con: - issue an LMI_ENABLE_CON primitive upstream
 * @sl: private strucutre
 * @q: active queue
 */
static int
lmi_enable_con(struct sl *sl, queue_t *q)
{
	lmi_enable_con_t *p;
	mblk_t *mp;

	ensure(sl_get_i_state(sl) == LMI_ENABLE_PENDING, return (-EFAULT));
	if (likely((mp = ss7_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_ENABLE_CON;
		p->lmi_state = LMI_ENABLED;
		mp->b_wptr += sizeof(*p);
		sl_set_i_state(sl, LMI_ENABLED);
		printd(("%s: %p: <- LMI_ENABLE_CON\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_disable_con: - issue an LMI_DISABLE_CON primitive upstream
 * @sl: private structure
 * @q: active queue
 */
static int
lmi_disable_con(struct sl *sl, queue_t *q)
{
	lmi_disable_con_t *p;
	mblk_t *mp;

	ensure(sl_get_i_state(sl) == LMI_DISABLE_PENDING, return (-EFAULT));
	if (likely((mp = ss7_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_DISABLE_CON;
		p->lmi_state = LMI_DISABLED;
		sl_set_i_state(sl, LMI_DISABLED);
		sl_set_n_state(sl, NS_IDLE);
		mp->b_wptr += sizeof(*p);
		printd(("%s: %p: <- LMI_DISABLE_CON\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  LMI_OPTMGMT_ACK
 *  ---------------------------------------------
 */
#if 0
STATIC INLINE int
lmi_optmgmt_ack(queue_t *q, struct sl *sl, lmi_ulong flags, void *opt_ptr, size_t opt_len)
{
	mblk_t *mp;
	lmi_optmgmt_ack_t *p;

	if (likely((mp = ss7_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((lmi_optmgmt_ack_t *) mp->b_wptr)++;
		p->lmi_primitive = LMI_OPTMGMT_ACK;
		p->lmi_opt_length = opt_len;
		p->lmi_opt_offset = opt_len ? sizeof(*p) : 0;
		p->lmi_mgmt_flags = flags;
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
		printd(("%s: %p: <- LMI_OPTMGMT_ACK\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
#endif
/**
 * lmi_error_ind: - issue an LMI_ERROR_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 * @err: postitive LMI or negative UNIX error
 * @fatal: whether error is fatal or not
 */
static int
lmi_error_ind(struct sl *sl, queue_t *q, lmi_long err, int fatal)
{
	mblk_t *mp;
	lmi_error_ind_t *p;

	if (likely((mp = ss7_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_ERROR_IND;
		p->lmi_errno = err < 0 ? -err : 0;
		p->lmi_reason = err < 0 ? LMI_SYSERR : err;
		if (fatal)
			sl_set_i_state(sl, LMI_UNUSABLE);
		p->lmi_state = sl_get_i_state(sl);
		mp->b_wptr += sizeof(*p);
		printd(("%s: %p: <- LMI_ERROR_IND\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

#if 0
/**
 * lmi_stats_ind: - issue an LMI_STATS_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 * @interval: interval reported
 * @timestamp: timestamp of report
 */
STATIC INLINE int
lmi_stats_ind(struct sl *sl, queue_t *q, lmi_ulong interval, lmi_ulong timestamp)
{
	lmi_stats_ind_t *p;
	mblk_t *mp;

	if (likely((mp = ss7_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		if (likely(bcanputnext(sl->oq, mp->b_band))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->lmi_primitive = LMI_STATS_IND;
			p->lmi_interval = interval;
			p->lmi_timestamp = timestamp;
			mp->b_wptr += sizeof(*p);
			putnext(sl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
#endif

/**
 * lmi_event_ind: - issue an LMI_EVENT_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 * @oid: event id
 * @severity: event severity
 * @inf_ptr: information pointer
 * @inf_len: information length
 */
static int
lmi_event_ind(struct sl *sl, queue_t *q, lmi_ulong oid, lmi_ulong severity, void *inf_ptr,
	      size_t inf_len)
{
	lmi_event_ind_t *p;
	mblk_t *mp;

	if (likely((mp = ss7_allocb(q, sizeof(*p) + inf_len, BPRI_MED)) != NULL)) {
		if (likely(bcanputnext(sl->oq, mp->b_band))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->lmi_primitive = LMI_EVENT_IND;
			p->lmi_objectid = oid;
			p->lmi_timestamp = jiffies;
			p->lmi_severity = severity;
			mp->b_wptr += sizeof(*p);
			bcopy(mp->b_wptr, inf_ptr, inf_len);
			mp->b_wptr += inf_len;
			printd(("%s: %p: <- LMI_EVENT_IND\n", MOD_NAME, sl));
			putnext(sl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_pdu_ind: - issued an SL_PDU_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 * @dp: user data to indicate
 */
STATIC INLINE fastcall __hot_in int
sl_pdu_ind(struct sl *sl, queue_t *q, mblk_t *dp)
{
	sl_pdu_ind_t *p;
	mblk_t *mp;

	if (likely((mp = ss7_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		if (likely(bcanputnext(sl->oq, mp->b_band))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_PDU_IND;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			printd(("%s: %p: <- SL_PDU_IND [%lu]\n", MOD_NAME, sl,
				(ulong) msgdsize(mp)));
			putnext(sl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_link_congested_ind: - issue an SL_LINK_CONGESTED_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 * @cong: congestion status
 * @disc: discard status
 */
static int
sl_link_congested_ind(struct sl *sl, queue_t *q, sl_ulong cong, sl_ulong disc)
{
	sl_link_cong_ind_t *p;
	mblk_t *mp;

	if (likely((mp = ss7_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		if (likely(bcanputnext(sl->oq, mp->b_band))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_LINK_CONGESTED_IND;
			p->sl_timestamp = jiffies;
			p->sl_cong_status = cong;
			p->sl_disc_status = disc;
			mp->b_wptr += sizeof(*p);
			printd(("%s: %p: <- SL_LINK_CONGESTED_IND\n", MOD_NAME, sl));
			putnext(sl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_link_congestion_ceased_ind: - issue an SL_LINK_CONGESTION_CEASED_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 * @cong: congestion status
 * @disc: discard status
 */
static int
sl_link_congestion_ceased_ind(struct sl *sl, queue_t *q, sl_ulong cong, sl_ulong disc)
{
	sl_link_cong_ceased_ind_t *p;
	mblk_t *mp;

	if (likely((mp = ss7_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		if (likely(bcanputnext(sl->oq, mp->b_band))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_LINK_CONGESTION_CEASED_IND;
			p->sl_timestamp = jiffies;
			p->sl_cong_status = cong;
			p->sl_disc_status = disc;
			mp->b_wptr += sizeof(*p);
			printd(("%s: %p: <- SL_LINK_CONGESTION_CEASED_IND\n", MOD_NAME, sl));
			putnext(sl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_retrieved_message_ind: - issue an SL_RETRIEVED_MESSAGE_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 * @dp: retrieved user data
 */
static int
sl_retrieved_message_ind(struct sl *sl, queue_t *q, mblk_t *dp)
{
	sl_retrieved_msg_ind_t *p;
	mblk_t *mp;

	if (likely((mp = ss7_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		if (likely(bcanputnext(sl->oq, mp->b_band))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_RETRIEVED_MESSAGE_IND;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			printd(("%s: %p: <- SL_RETRIEVED_MESSAGE_IND\n", MOD_NAME, sl));
			putnext(sl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_retrieval_complete_ind: - issue an SL_RETRIEVAL_COMPLETE_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 */
static int
sl_retrieval_complete_ind(struct sl *sl, queue_t *q, mblk_t *dp)
{
	sl_retrieval_comp_ind_t *p;
	mblk_t *mp;

	if (likely((mp = ss7_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_RETRIEVAL_COMPLETE_IND;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = dp;
		printd(("%s: %p: <- SL_RETRIEVAL_COMPLETE_IND\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_rb_cleared_ind: - issue an SL_RB_CLEARED_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 */
static int
sl_rb_cleared_ind(struct sl *sl, queue_t *q)
{
	sl_rb_cleared_ind_t *p;
	mblk_t *mp;

	if (likely((mp = ss7_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		if (likely(bcanputnext(sl->oq, mp->b_band))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_RB_CLEARED_IND;
			mp->b_wptr += sizeof(*p);
			printd(("%s: %p: <- SL_RB_CLEARED_IND\n", MOD_NAME, sl));
			putnext(sl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_bsnt_ind: - issue an SL_BSNT_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 * @bsnt: value of BSNT to indicate
 */
static int
sl_bsnt_ind(struct sl *sl, queue_t *q, sl_ulong bsnt)
{
	sl_bsnt_ind_t *p;
	mblk_t *mp;

	if (likely((mp = ss7_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		if (likely(bcanputnext(sl->oq, mp->b_band))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_BSNT_IND;
			p->sl_bsnt = bsnt;
			mp->b_wptr += sizeof(*p);
			printd(("%s: %p: <- SL_BSNT_IND\n", MOD_NAME, sl));
			putnext(sl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_in_service_ind: - issue an SL_IN_SERVICE_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 */
static int
sl_in_service_ind(struct sl *sl, queue_t *q)
{
	sl_in_service_ind_t *p;
	mblk_t *mp;

	if (likely((mp = ss7_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_IN_SERVICE_IND;
		printd(("%s: %p: <- SL_IN_SERVICE_IND\n", MOD_NAME, sl));
		mp->b_wptr += sizeof(*p);
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_out_of_service_ind: - issue an SL_OUT_OF_SERVICE_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 * @reason: reason for failure
 */
static int
sl_out_of_service_ind(struct sl *sl, queue_t *q, sl_ulong reason)
{
	sl_out_of_service_ind_t *p;
	mblk_t *mp;

	if (likely((mp = ss7_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_OUT_OF_SERVICE_IND;
		p->sl_timestamp = jiffies;
		p->sl_reason = reason;
		mp->b_wptr += sizeof(*p);
		printd(("%s: %p: <- SL_OUT_OF_SERVICE_IND\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_remote_processor_outage_ind: - issue an SL_REMOTE_PROCESSOR_OUTAGE_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 */
static int
sl_remote_processor_outage_ind(struct sl *sl, queue_t *q)
{
	sl_rem_proc_out_ind_t *p;
	mblk_t *mp;

	if (likely((mp = ss7_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		if (likely(bcanputnext(sl->oq, mp->b_band))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_REMOTE_PROCESSOR_OUTAGE_IND;
			p->sl_timestamp = jiffies;
			mp->b_wptr += sizeof(*p);
			printd(("%s: %p: <- SL_PROCESSOR_OUTAGE_IND\n", MOD_NAME, sl));
			putnext(sl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_remote_processor_recovered_ind: - issue an SL_REMOTE_PROCESSOR_RECOVERED_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 */
static int
sl_remote_processor_recovered_ind(struct sl *sl, queue_t *q)
{
	sl_rem_proc_recovered_ind_t *p;
	mblk_t *mp;

	if (likely((mp = ss7_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		if (likely(bcanputnext(sl->oq, mp->b_band))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_REMOTE_PROCESSOR_RECOVERED_IND;
			p->sl_timestamp = jiffies;
			mp->b_wptr += sizeof(*p);
			printd(("%s: %p: <- SL_REMOTE_PROCESSOR_RECOVERED_IND\n", MOD_NAME, sl));
			putnext(sl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_rtb_cleared_ind: - issue an SL_RTB_CLEARED_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 */
static int
sl_rtb_cleared_ind(struct sl *sl, queue_t *q)
{
	sl_rtb_cleared_ind_t *p;
	mblk_t *mp;

	if (likely((mp = ss7_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		if (likely(bcanputnext(sl->oq, mp->b_band))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_RTB_CLEARED_IND;
			mp->b_wptr += sizeof(*p);
			printd(("%s: %p: <- SL_RTB_CLEARED_IND\n", MOD_NAME, sl));
			putnext(sl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_retrieval_not_possible_ind: - issue an SL_RETRIEVAL_NOT_POSSIBLE_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 */
static int
sl_retrieval_not_possible_ind(struct sl *sl, queue_t *q)
{
	sl_retrieval_not_poss_ind_t *p;
	mblk_t *mp;

	if (likely((mp = ss7_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		if (likely(bcanputnext(sl->oq, mp->b_band))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_RETRIEVAL_NOT_POSSIBLE_IND;
			mp->b_wptr += sizeof(*p);
			printd(("%s: %p: <- SL_RETRIEVAL_NOT_POSSIBLE_IND\n", MOD_NAME, sl));
			putnext(sl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

#if 0
/**
 * sl_bsnt_not_retrievable_ind: - issue an SL_BSNT_NOT_RETRIEVABLE_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 * @bsnt: BSNT
 */
STATIC INLINE int
sl_bsnt_not_retrievable_ind(struct sl *sl, queue_t *q, sl_ulong bsnt)
{
	sl_bsnt_not_retr_ind_t *p;
	mblk_t *mp;

	if (likely((mp = ss7_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		if (likely(bcanputnext(sl->oq, mp->b_band))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_BSNT_NOT_RETRIEVABLE_IND;
			p->sl_bsnt = bsnt;
			mp->b_wptr += sizeof(*p);
			printd(("%s: %p: <- SL_BSNT_NOT_RETRIEVABLE_IND\n", MOD_NAME, sl));
			putnext(sl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
#endif

static int
sl_local_processor_outage_ind(struct sl *sl, queue_t *q)
{
	sl_loc_proc_out_ind_t *p;
	mblk_t *mp;

	if (likely((mp = ss7_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		if (likely(bcanputnext(sl->oq, mp->b_band))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_LOCAL_PROCESSOR_OUTAGE_IND;
			p->sl_timestamp = drv_hztomsec(jiffies);
			mp->b_wptr += sizeof(*p);
			printd(("%s: %p: <- SL_LOCAL_PROCESSOR_OUTAGE_IND\n", MOD_NAME, sl));
			putnext(sl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

static int
sl_local_processor_recovered_ind(struct sl *sl, queue_t *q)
{
	sl_loc_proc_recovered_ind_t *p mblk_t *mp;

	if (likely((mp = ss7_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		if (likely(bcanputnext(sl->oq, mp->b_band))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_LOCAL_PROCESSOR_RECOVERED_IND;
			p->sl_timestamp = drv_hztomsec(jiffies);
			mp->b_wptr += sizeof(*p);
			printd(("%s: %p: <- SL_LOCAL_PROCESSOR_RECOVERED_IND\n", MOD_NAME, sl));
			putnext(sl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  NPI User (M2PA) -> NPI Provider (SCTP) Primitives
 *
 *  -------------------------------------------------------------------------
 */

#if 0
static int
n_pass_down(struct sl *sl, queue_t *q, mblk_t *mp, int npistate, int lmistate)
{
	if (mp->b_datap->db_type < QPCTL && !bcanputnext(q, mp->b_band))
		goto busy;
	sl_set_n_state(sl, npistate);
	sl_set_i_state(sl, lmistate);
	sl->n_request = true;
	putnext(q, mp);
	return (QR_ABSORBED);
      busy:
	return (-EBUSY);
}
#endif

/**
 * n_bind_req: - issue a bind request to fullfill an attach request
 * @sl: private structure
 * @q: active queue
 * @add_ptr: pointer to address to bind
 * @add_len: length of bind address
 */
static int
n_bind_req(struct sl *sl, queue_t *q, caddr_t add_ptr, size_t add_len)
{
	N_bind_req_t *p;
	mblk_t *mp;

	if (likely((mp = ss7_allocb(q, sizeof(*p) + add_len, BPRI_MED)) != NULL)) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_BIND_REQ;
		p->ADDR_length = add_len;
		p->ADDR_offset = add_len ? sizeof(*p) : 0;
		p->CONIND_number = 0;
		p->BIND_flags = TOKEN_REQUEST;
		p->PROTOID_length = 0;
		p->PROTOID_offset = 0;
		mp->b_wptr += sizeof(*p);
		bcopy(add_ptr, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
		sl_set_i_state(sl, LMI_ATTACH_PENDING);
		printd(("%s: %p: N_BIND_REQ ->\n", MOD_NAME, sl));
		putnext(sl->iq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * n_unbind_req: - issue an unbind request to fullfull a detach request
 * @sl: private structure
 * @q: active queue
 */
static int
n_unbind_req(struct sl *sl, queue_t *q)
{
	N_unbind_req_t *p;
	mblk_t *mp;

	if (likely((mp = ss7_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_UNBIND_REQ;
		mp->b_wptr += sizeof(*p);
		sl_set_i_state(sl, LMI_DETACH_PENDING);
		printd(("%s: %p: N_UNBIND_REQ ->\n", MOD_NAME, sl));
		putnext(sl->iq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * n_conn_req: - issue a connection request to fulfull an enable request
 * @sl: private structure
 * @q: active queue
 * @dst_ptr: destination address
 * @dst_len: destination adresss length
 * @qos_ptr: quality of service parameters
 * @qos_len: quality of service parameters length
 */
static int
n_conn_req(struct sl *sl, queue_t *q, caddr_t dst_ptr, size_t dst_len)
{
	N_qos_sel_conn_sctp_t *n;
	N_conn_req_t *p;
	mblk_t *mp;

	if (likely((mp = ss7_allocb(q, sizeof(*p) + dst_len + sizeof(*n), BPRI_MED)) != NULL)) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_CONN_REQ;
		p->DEST_length = dst_len;
		p->DEST_offset = dst_len ? sizeof(*p) : 0;
		p->CONN_flags = REC_CONF_OPT | EX_DATA_OPT;
		p->QOS_length = sizeof(*n);
		p->QOS_offset = sizeof(*p) + dst_len;
		mp->b_wptr += sizeof(*p);
		bcopy(dst_ptr, mp->b_wptr, dst_len);
		mp->b_wptr += dst_len;
		n = (typeof(n)) mp->b_wptr;
		n->n_qos_type = N_QOS_SEL_CONN_SCTP;
		n->i_streams = 2;
		n->o_streams = 2;
		mp->b_wptr += sizeof(*n);
		sl_set_i_state(sl, LMI_ENABLE_PENDING);
		printd(("%s: %p: N_CONN_REQ ->\n", MOD_NAME, sl));
		putnext(sl->iq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * n_discon_req: - issue a disconnection request to fulfill a disable request
 * @sl: private structure
 * @q: active queue
 * @reason: disconnect reason
 */
static int
n_discon_req(struct sl *sl, queue_t *q, np_ulong reason)
{
	N_discon_req_t *p;
	mblk_t *mp;

	if (likely((mp = ss7_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		if (likely(bcanputnext(sl->iq, mp->b_band))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_DISCON_REQ;
			p->DISCON_reason = reason;
			p->RES_length = 0;
			p->RES_offset = 0;
			p->SEQ_number = 0;
			mp->b_wptr += sizeof(*p);
			sl_set_i_state(sl, LMI_DISABLE_PENDING);
			printd(("%s: %p: N_DISCON_REQ ->\n", MOD_NAME, sl));
			putnext(sl->iq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * n_data_req: - issue a data transfer request
 * @sl: private structure
 * @q: active queue
 * @flags: data transfer flags
 * @sid: SCTP stream on which to send
 * @dp: user data to transfer
 */
STATIC INLINE fastcall __hot_out int
n_data_req(struct sl *sl, queue_t *q, np_ulong flags, np_ulong sid, mblk_t *dp)
{
	N_qos_sel_data_sctp_t *n;
	N_data_req_t *p;
	mblk_t *mp;

	if (likely((mp = ss7_allocb(q, sizeof(*p) + sizeof(*n), BPRI_MED)) != NULL)) {
		if (likely(bcanputnext(sl->iq, mp->b_band))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_DATA_REQ;
			p->DATA_xfer_flags = flags;
			mp->b_wptr += sizeof(*p);
			n = (typeof(n)) mp->b_wptr;
			n->n_qos_type = N_QOS_SEL_DATA_SCTP;
			n->ppi = M2UA_PPI;
			n->sid = sid;
			n->ssn = 0;
			n->tsn = 0;
			n->more = flags;
			mp->b_wptr += sizeof(*n);
			mp->b_cont = dp;
			printd(("%s: %p: N_DATA_REQ ->\n", MOD_NAME, sl));
			putnext(sl->iq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * n_exdata_req: - issue an expedited data transfer request
 * @sl: private structure
 * @q: active queue
 * @flags: data transfer flags
 * @sid: SCTP stream on which to send
 * @dp: user data to transfer
 */
STATIC INLINE fastcall int
n_exdata_req(struct sl *sl, queue_t *q, np_ulong flags, np_ulong sid, mblk_t *dp)
{
	N_qos_sel_data_sctp_t *n;
	N_exdata_req_t *p;
	mblk_t *mp;

	if (likely((mp = ss7_allocb(q, sizeof(*p) + sizeof(*n), BPRI_MED)) != NULL)) {
		mp->b_band = 1;	/* expedite */
		if (likely(bcanputnext(sl->iq, mp->b_band))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_EXDATA_REQ;
			mp->b_wptr += sizeof(*p);
			n = (typeof(n)) mp->b_wptr;
			n->n_qos_type = N_QOS_SEL_DATA_SCTP;
			n->ppi = M2UA_PPI;
			n->sid = sid;
			n->ssn = 0;
			n->tsn = 0;
			n->more = flags;
			mp->b_wptr += sizeof(*n);
			mp->b_cont = dp;
			printd(("%s: %p: N_EXDATA_REQ ->\n", MOD_NAME, sl));
			putnext(sl->iq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  ==========================================================================
 *
 *  M2UA State Machines
 *
 *  ==========================================================================
 */

/*
 *  ==========================================================================
 *
 *  OUTPUT Events
 *
 *  ==========================================================================
 */
/*
 *  --------------------------------------------------------------------------
 *
 *  SL AS -> SL SG (M2UA) Sent Messages
 *
 *  --------------------------------------------------------------------------
 */
/**
 * sl_send_mgmt_err: - send MGMT ERR message
 * @sl: private structure
 * @q: active queue
 * @ecode: error code
 * @dia_ptr: pointer to diagnostics
 * @dia_len: length of diagnostics
 */
STATIC INLINE __unlikely int
sl_send_mgmt_err(struct sl *sl, queue_t *q, np_ulong ecode, caddr_t dia_ptr, size_t dia_len)
{
	int err;
	mblk_t *mp;
	size_t mlen =
	    UA_MHDR_SIZE + UA_SIZE(UA_PARM_ECODE) + dia_len ? UA_SIZE(UA_PARM_DIAG) +
	    UA_PAD4(dia_len) : 0;

	if (likely((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_MGMT_ERR;
		p[1] = htonl(mlen);
		p[2] = UA_PARM_ECODE;
		p[3] = htonl(ecode);
		p += 4;
		if (dia_len) {
			*p++ = UA_PHDR(UA_PARM_DIAG, dia_len);
			bcopy(dia_ptr, p, dia_len);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(dia_len);
		/* sent unordered and expedited on management stream */
		if ((err = n_exdata_req(sl, q, 0, 0, mp)) == 0)
			return (QR_DONE);
		freeb(mp);
		return (err);

	}
	return (-ENOBUFS);
}

/**
 * sl_send_mgmt_ntfy: - send management notify message
 * @sl: private structure
 * @q: active queue
 * @status: notification status
 * @aspid: ASP identifier, if non-NULL
 * @iid: Interface identifier, if non-NULL
 * @num_iid: number of interface identifiers
 * @inf_ptr: pointer to info
 * @inf_len: length of info
 */
STATIC INLINE int
sl_send_mgmt_ntfy(struct sl *sl, queue_t *q, np_ulong status, np_ulong *aspid, np_ulong *iid,
		  size_t num_iid, caddr_t inf_ptr, size_t inf_len)
{
	int err;
	mblk_t *mp;
	size_t mlen =
	    UA_MHDR_SIZE + UA_SIZE(UA_PARM_STATUS) + aspid ? UA_SIZE(UA_PARM_ASPID) : 0 +
	    num_iid ? UA_PHDR_SIZE + num_iid * sizeof(uint32_t) : 0 + inf_len ? UA_PHDR_SIZE +
	    UA_PAD4(inf_len) : 0;

	if (likely((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_MGMT_NTFY;
		p[1] = htonl(mlen);
		p[2] = UA_PARM_STATUS;
		p[3] = htonl(status);
		p += 4;
		if (aspid) {
			p[0] = UA_PARM_ASPID;
			p[1] = htonl(*aspid);
			p += 2;
		}
		if (num_iid) {
			uint32_t *ip = iid;
			*p++ = UA_PHDR(UA_PARM_IID, num_iid * sizeof(uint32_t));
			while (num_iid--)
				*p++ = htonl(*ip++);
		}
		if (inf_len) {
			*p++ = UA_PHDR(UA_PARM_INFO, inf_len);
			bcopy(inf_ptr, p, inf_len);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(inf_len);
		/* send unordered and expedited on management stream */
		if ((err = n_exdata_req(sl, q, 0, 0, mp)) == 0)
			return (QR_DONE);
		freeb(mp);
		return (err);

	}
	return (-ENOBUFS);
}

/**
 * sl_send_asps_aspup_req: - send ASP Up
 * @sl: private structure
 * @q: active queue
 * @aspid: ASP identifier if not NULL
 * @inf_ptr: Info pointer
 * @inf_len: Info length
 */
STATIC INLINE int
sl_send_asps_aspup_req(struct sl *sl, queue_t *q, np_ulong *aspid, caddr_t inf_ptr, size_t inf_len)
{
	int err;
	mblk_t *mp;
	size_t mlen =
	    UA_MHDR_SIZE + aspid ? UA_SIZE(UA_PARM_ASPID) : 0 + inf_len ? UA_PHDR_SIZE +
	    UA_PAD4(inf_len) : 0;

	if (likely((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPS_ASPDN_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (aspid) {
			p[0] = UA_PARM_ASPID;
			p[1] = htonl(*aspid);
			p += 2;
		}
		if (inf_len) {
			*p++ = UA_PHDR(UA_PARM_INFO, inf_len);
			bcopy(inf_ptr, p, inf_len);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(inf_len);
		/* send unordered and expedited on stream 0 */
		if ((err = n_exdata_req(sl, q, 0, 0, mp)) == 0)
			return (QR_DONE);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * sl_send_asps_aspdn_req: - send ASP Down
 * @sl: private structure
 * @q: active queue
 * @aspid: ASP identifier if not NULL
 * @inf_ptr: Info pointer
 * @inf_len: Info length
 */
STATIC INLINE int
sl_send_asps_aspdn_req(struct sl *sl, queue_t *q, np_ulong *aspid, caddr_t inf_ptr, size_t inf_len)
{
	int err;
	mblk_t *mp;
	size_t mlen =
	    UA_MHDR_SIZE + aspid ? UA_SIZE(UA_PARM_ASPID) : 0 + inf_len ? UA_PHDR_SIZE +
	    UA_PAD4(inf_len) : 0;

	if (likely((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPS_ASPDN_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (aspid) {
			p[0] = UA_PARM_ASPID;
			p[1] = htonl(*aspid);
			p += 2;
		}
		if (inf_len) {
			*p++ = UA_PHDR(UA_PARM_INFO, inf_len);
			bcopy(inf_ptr, p, inf_len);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(inf_len);
		if (likely((err = n_exdata_req(sl, q, 0, 0, mp)) == 0))
			return (QR_DONE);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * sl_send_asps_hbeat_req: - send a BEAT message
 * @sl: private structure
 * @q: active queue
 * @sid: SCTP stream identifier
 * @hbt_ptr: heartbeat info pointer
 * @hbt_len: heartbeat info length
 */
STATIC INLINE int
sl_send_asps_hbeat_req(struct sl *sl, queue_t *q, np_ulong sid, caddr_t hbt_ptr, size_t hbt_len)
{
	int err;
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE + hbt_len ? UA_PHDR_SIZE + UA_PAD4(hbt_len) : 0;

	if (likely((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPS_HBEAT_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (hbt_len) {
			*p++ = UA_PHDR(UA_PARM_HBDATA, hbt_len);
			bcopy(hbt_ptr, p, hbt_len);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(hbt_len);
		/* send ordered on specified stream */
		if (likely((err = n_data_req(sl, q, 0, sid, mp)) == 0))
			return (QR_DONE);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * sl_send_asps_aspup_ack: - send ASP Up Ack
 * @sl: private structure
 * @q: active queue
 * @aspid: ASP Id if not NULL
 * @inf_ptr: INFO pointer
 * @inf_len: INFO length
 */
STATIC INLINE int
sl_send_asps_aspup_ack(struct sl *sl, queue_t *q, np_ulong *aspid, caddr_t inf_ptr, size_t inf_len)
{
	int err;
	mblk_t *mp;
	size_t mlen =
	    UA_MHDR_SIZE + aspid ? UA_SIZE(UA_PARM_ASPID) : 0 + inf_len ? UA_PHDR_SIZE +
	    UA_PAD4(inf_len) : 0;

	if (likely((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPS_ASPUP_ACK;
		p[1] = htonl(mlen);
		p += 2;
		if (aspid) {
			p[0] = UA_PARM_ASPID;
			p[1] = htonl(*aspid);
			p += 2;
		}
		if (inf_len) {
			*p++ = UA_PHDR(UA_PARM_INFO, inf_len);
			bcopy(inf_ptr, p, inf_len);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(inf_len);
		/* send unordered on stream 0 */
		if (likely((err = n_exdata_req(sl, q, 0, 0, mp)) == 0))
			return (QR_DONE);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * sl_send_asps_aspdn_ack: - send ASP Down Ack
 * @sl: private structure
 * @q: active queue
 * @aspid: ASP Id if not NULL
 * @inf_ptr: INFO pointer
 * @inf_len: INFO length
 */
STATIC INLINE int
sl_send_asps_aspdn_ack(struct sl *sl, queue_t *q, np_ulong *aspid, caddr_t inf_ptr, size_t inf_len)
{
	int err;
	mblk_t *mp;
	size_t mlen =
	    UA_MHDR_SIZE + aspid ? UA_SIZE(UA_PARM_ASPID) : 0 + inf_len ? UA_PHDR_SIZE +
	    UA_PAD4(inf_len) : 0;

	if (likely((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPS_ASPDN_ACK;
		p[1] = htonl(mlen);
		p += 2;
		if (aspid) {
			p[0] = UA_PARM_ASPID;
			p[1] = htonl(*aspid);
			p += 2;
		}
		if (inf_len) {
			*p++ = UA_PHDR(UA_PARM_INFO, inf_len);
			bcopy(inf_ptr, p, inf_len);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(inf_len);
		if (likely((err = n_exdata_req(sl, q, 0, 0, mp)) == 0))
			return (QR_DONE);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * sl_send_asps_hbeat_ack: - send a BEAT message
 * @sl: private structure
 * @q: active queue
 * @sid: SCTP stream identifier
 * @hbt_ptr: heartbeat info pointer
 * @hbt_len: heartbeat info length
 */
STATIC INLINE int
sl_send_asps_hbeat_ack(struct sl *sl, queue_t *q, np_ulong sid, caddr_t hbt_ptr, size_t hbt_len)
{
	int err;
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE + hbt_len ? UA_PHDR_SIZE + UA_PAD4(hbt_len) : 0;

	if (likely((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPS_HBEAT_ACK;
		p[1] = htonl(mlen);
		p += 2;
		if (hbt_len) {
			*p++ = UA_PHDR(UA_PARM_HBDATA, hbt_len);
			bcopy(hbt_ptr, p, hbt_len);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(hbt_len);
		/* send ordered on specified stream */
		if (likely((err = n_data_req(sl, q, 0, sid, mp)) == 0))
			return (QR_DONE);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * sl_send_aspt_aspac_req: - send ASP Active
 * @sl: private structure
 * @q: active queue
 * @tmode: traffic mode
 * @iid: Interface Id
 * @num_iid: number of interface ids
 * @inf_ptr: INFO pointer
 * @inf_len: INFO length
 */
STATIC INLINE int
sl_send_aspt_aspac_req(struct sl *sl, queue_t *q, np_ulong tmode, np_ulong *iid, np_ulong num_iid,
		       caddr_t inf_ptr, size_t inf_len)
{
	int err;
	mblk_t *mp;
	size_t mlen =
	    UA_MHDR_SIZE + UA_SIZE(UA_PARM_TMODE) + num_iid ? UA_PHDR_SIZE +
	    num_iid * sizeof(uint32_t) : 0 + inf_len ? UA_PHDR_SIZE + UA_PAD4(inf_len) : 0;

	if (likely((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPT_ASPAC_REQ;
		p[1] = htonl(mlen);
		p[2] = UA_PARM_TMODE;
		p[3] = htonl(tmode);
		p += 4;
		if (num_iid) {
			uint32_t *ip = iid;

			*p++ = UA_PHDR(UA_PARM_IID, num_iid * sizeof(uint32_t));
			while (num_iid--)
				*p++ = htonl(*ip++);
		}
		if (inf_len) {
			*p++ = UA_PHDR(UA_PARM_INFO, inf_len);
			bcopy(inf_ptr, p, inf_len);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(inf_len);
		/* always sent on same stream as data */
		if (likely((err = n_data_req(sl, q, 0, iid ? *iid : 0, mp)) == 0))
			return (QR_DONE);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * sl_send_aspt_aspia_req: - send ASP Inactive
 * @sl: private structure
 * @q: active queue
 * @iid: Interface Id
 * @num_iid: number of interface ids
 * @inf_ptr: INFO pointer
 * @inf_len: INFO length
 */
STATIC INLINE int
sl_send_aspt_aspia_req(struct sl *sl, queue_t *q, np_ulong *iid, np_ulong num_iid,
		       caddr_t inf_ptr, size_t inf_len)
{
	int err;
	mblk_t *mp;
	size_t mlen =
	    UA_MHDR_SIZE + num_iid ? UA_PHDR_SIZE + num_iid * sizeof(uint32_t) : 0 +
	    inf_len ? UA_PHDR_SIZE + UA_PAD4(inf_len) : 0;

	if (likely((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPT_ASPIA_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (num_iid) {
			uint32_t *ip = iid;

			*p++ = UA_PHDR(UA_PARM_IID, num_iid * sizeof(uint32_t));
			while (num_iid--)
				*p++ = htonl(*ip++);
		}
		if (inf_len) {
			*p++ = UA_PHDR(UA_PARM_INFO, inf_len);
			bcopy(inf_ptr, p, inf_len);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(inf_len);
		if (likely((err = n_data_req(sl, q, 0, iid ? *iid : 0, mp)) == 0))
			return (QR_DONE);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * sl_send_aspt_aspac_ack: - send ASP Active Ack
 * @sl: private structure
 * @q: active queue
 * @tmode: traffic mode
 * @iid: interface id
 * @num_iid: number of interface ids
 * @inf_ptr: INFO pointer
 * @inf_len: INFO length
 */
STATIC INLINE int
sl_send_aspt_aspac_ack(struct sl *sl, queue_t *q, np_ulong tmode, np_ulong *iid, size_t num_iid,
		       caddr_t inf_ptr, size_t inf_len)
{
	int err;
	mblk_t *mp;
	size_t mlen =
	    UA_MHDR_SIZE + UA_SIZE(UA_PARM_TMODE) + num_iid ? UA_PHDR_SIZE +
	    num_iid * sizeof(uint32_t) : 0 + inf_len ? UA_PHDR_SIZE + UA_PAD4(inf_len) : 0;

	if (likely((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPT_ASPAC_ACK;
		p[1] = htonl(mlen);
		p[2] = UA_PARM_TMODE;
		p[3] = htonl(tmode);
		p += 4;
		if (num_iid) {
			uint32_t *ip = iid;
			*p++ = UA_PHDR(UA_PARM_IID, num_iid * sizeof(uint32_t));
			while (num_iid--)
				*p++ = htonl(*ip++);
		}
		if (inf_len) {
			*p++ = UA_PHDR(UA_PARM_INFO, inf_len);
			bcopy(inf_ptr, p, inf_len);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(inf_len);
		if (likely((err = n_data_req(sl, q, 0, iid ? *iid : 0, mp)) == 0))
			return (QR_DONE);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * sl_send_aspt_aspia_ack: - send ASP Inactive Ack
 * @sl: private structure
 * @q: active queue
 * @iid: interface id
 * @num_iid: number of interface ids
 * @inf_ptr: INFO pointer
 * @inf_len: INFO length
 */
STATIC INLINE int
sl_send_aspt_aspia_ack(struct sl *sl, queue_t *q, uint32_t *iid, size_t num_iid, caddr_t inf_ptr,
		       size_t inf_len)
{
	int err;
	mblk_t *mp;
	size_t mlen =
	    UA_MHDR_SIZE + num_iid ? UA_PHDR_SIZE + num_iid * sizeof(uint32_t) : 0 +
	    inf_len ? UA_PHDR_SIZE + UA_PAD4(inf_len) : 0;

	if (likely((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPT_ASPIA_ACK;
		p[1] = htonl(mlen);
		p += 2;
		if (num_iid) {
			uint32_t *ip = iid;

			*p++ = UA_PHDR(UA_PARM_IID, num_iid * sizeof(uint32_t));
			while (num_iid--)
				*p++ = htonl(*ip++);
		}
		if (inf_len) {
			*p++ = UA_PHDR(UA_PARM_INFO, inf_len);
			bcopy(inf_ptr, p, inf_len);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(inf_len);

		if (likely((err = n_exdata_req(sl, q, 0, iid ? iid[0] : 0, mp)) == 0))
			return (QR_DONE);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * sl_send_rkmm_reg_req: - send REG REQ
 * @sl: private structure
 * @q: active queue
 * @id: request identifier
 * @sdti: Signalling Data Terminal Identifier
 * @sdli: Signalling Data Link Identifier
 */
STATIC INLINE int
sl_send_rkmm_reg_req(struct sl *sl, queue_t *q, uint32_t id, uint32_t sdti, uint32_t sdli)
{
	int err;
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE + UA_SIZE(M2UA_PARM_LINK_KEY);

	if (likely((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_RKMM_REG_REQ;
		p[1] = __constant_htonl(mlen);
		p[2] = M2UA_PARM_LINK_KEY;
		p[3] = M2UA_PARM_LOC_KEY_ID;
		p[4] = htonl(id);
		p[5] = M2UA_PARM_SDTI;
		p[6] = htonl(sdti);
		p[7] = M2UA_PARM_SDLI;
		p[8] = htonl(sdli);
		p += 9;
		mp->b_wptr = (unsigned char *) p;

		if (likely((err = n_exdata_req(sl, q, 0, 0, mp)) == 0))
			return (QR_DONE);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * sl_send_rkmm_reg_rsp: send REG RSP
 * @sl: private structure
 * @q: active queue
 * @id: request identifier
 * @status: request status
 * @iid: interface id
 */
STATIC INLINE int
sl_send_rkmm_reg_rsp(struct sl *sl, queue_t *q, uint32_t id, uint32_t status, uint32_t iid)
{
	int err;
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE + UA_SIZE(M2UA_PARM_REG_RESULT);

	if (likely((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_RKMM_REG_RSP;
		p[1] = __constant_htonl(mlen);
		p[2] = M2UA_PARM_REG_RESULT;
		p[3] = M2UA_PARM_LOC_KEY_ID;
		p[4] = htonl(id);
		p[5] = M2UA_PARM_REG_STATUS;
		p[6] = htonl(status);
		p[7] = UA_PARM_IID;
		p[8] = status ? 0 : htonl(iid);
		p += 9;
		mp->b_wptr = (unsigned char *) p;

		if (likely((err = n_exdata_req(sl, q, 0, 0, mp)) == 0))
			return (QR_DONE);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * sl_send_maup_data1: - send DATA1
 * @sl: private structure
 * @q: active queue
 * @iid: interface id
 * @dp: user data
 */
STATIC INLINE int
sl_send_maup_data1(struct sl *sl, queue_t *q, uint32_t iid, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	size_t dlen = msgdsize(dp->b_cont);
	size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID) + UA_PHDR_SIZE;

	if (likely((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_DATA;
		p[1] = htonl(mlen + dlen);
		p[2] = UA_PARM_IID;
		p[3] = htonl(iid);
		p[4] = UA_PHDR(M2UA_PARM_DATA1, dlen);
		p += 5;
		mp->b_wptr = (unsigned char *) p;

		mp->b_cont = dp->b_cont;
		if (likely((err = n_data_req(sl, q, 0, iid, mp)) == 0))
			return (QR_TRIMMED);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * sl_send_maup_data2: - send DATA2
 * @sl: private structure
 * @q: active queue
 * @iid: interface id
 * @dp: user data
 */
STATIC INLINE int
sl_send_maup_data2(struct sl *sl, queue_t *q, uint32_t iid, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	size_t dlen = msgdsize(dp->b_cont);
	size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID) + UA_PHDR_SIZE;

	if (likely((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_DATA;
		p[1] = htonl(mlen + dlen);
		p[2] = UA_PARM_IID;
		p[3] = htonl(iid);
		p[4] = UA_PHDR(M2UA_PARM_DATA2, dlen);
		p += 5;
		mp->b_wptr = (unsigned char *) p;

		mp->b_cont = dp->b_cont;
		if (likely((err = n_data_req(sl, q, 0, iid, mp)) == 0))
			return (QR_TRIMMED);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * sl_send_maup_estab_req: - send MAUP Establish Request
 * @sl: private structure
 * @q: active queue
 * @iid: interface id
 */
STATIC INLINE int
sl_send_maup_estab_req(struct sl *sl, queue_t *q, uint32_t iid)
{
	int err;
	mblk_t *mp;
	static const size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID);

	if (likely((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_ESTAB_REQ;
		p[1] = __constant_htonl(mlen);
		p[2] = UA_PARM_IID;
		p[3] = htonl(iid);
		p += 4;
		mp->b_wptr = (unsigned char *) p;

		if (likely((err = n_data_req(sl, q, 0, iid, mp)) == 0))
			return (QR_DONE);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * sl_send_maup_estab_con: - send MAUP Establish Confirmation
 * @sl: private structure
 * @q: active queue
 * @iid: interface id
 */
STATIC INLINE int
sl_send_maup_estab_con(struct sl *sl, queue_t *q, uint32_t iid)
{
	int err;
	mblk_t *mp;
	static const size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID);

	if (likely((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_ESTAB_CON;
		p[1] = __constant_htonl(mlen);
		p[2] = UA_PARM_IID;
		p[3] = htonl(iid);
		p += 4;
		mp->b_wptr = (unsigned char *) p;

		if (likely((err = n_data_req(sl, q, 0, iid, mp)) == 0))
			return (QR_DONE);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * sl_send_maup_rel_req: - send MAUP Release Request
 * @sl: private structure
 * @q: active queue
 * @iid: interface id
 */
STATIC INLINE int
sl_send_maup_rel_req(struct sl *sl, queue_t *q, uint32_t iid)
{
	int err;
	mblk_t *mp;
	static const size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID);

	if (likely((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_REL_REQ;
		p[1] = __constant_htonl(mlen);
		p[2] = UA_PARM_IID;
		p[3] = htonl(iid);
		p += 4;
		mp->b_wptr = (unsigned char *) p;

		if (likely((err = n_data_req(sl, q, 0, iid, mp)) == 0))
			return (QR_DONE);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * sl_send_maup_rel_con: - send MAUP Release Confirmation
 * @sl: private structure
 * @q: active queue
 * @iid: interface id
 */
STATIC INLINE int
sl_send_maup_rel_con(struct sl *sl, queue_t *q, uint32_t iid)
{
	int err;
	mblk_t *mp;
	static const size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID);

	if (likely((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_REL_CON;
		p[1] = __constant_htonl(mlen);
		p[2] = UA_PARM_IID;
		p[3] = htonl(iid);
		p += 4;
		mp->b_wptr = (unsigned char *) p;

		if (likely((err = n_data_req(sl, q, 0, iid, mp)) == 0))
			return (QR_DONE);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * sl_send_maup_rel_ind: - send MAUP Release Indication
 * @sl: private structure
 * @q: active queue
 * @iid: interface id
 */
STATIC INLINE int
sl_send_maup_rel_ind(struct sl *sl, queue_t *q, uint32_t iid)
{
	int err;
	mblk_t *mp;
	static const size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID);

	if (likely((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_REL_IND;
		p[1] = __constant_htonl(mlen);
		p[2] = UA_PARM_IID;
		p[3] = htonl(iid);
		p += 4;
		mp->b_wptr = (unsigned char *) p;

		if (likely((err = n_data_req(sl, q, 0, iid, mp)) == 0))
			return (QR_DONE);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/*
 *  ==========================================================================
 *
 *  INPUT Events
 *
 *  ==========================================================================
 */
/*
 *  --------------------------------------------------------------------------
 *
 *  SL SG -> SL AS (M2UA) Received Messages
 *
 *  --------------------------------------------------------------------------
 */

/**
 * sl_recv_mgmt_err: - process received MGMT ERR message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MGMT ERR message (with header)
 */
static int
sl_recv_mgmt_err(struct sl *sl, queue_t *q, mblk_t *mp)
{
}

/**
 * sl_recv_mgmt_ntfy: - process received MGMT NTFY message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MGMT NTFY message (with header)
 */
static int
sl_recv_mgmt_ntfy(struct sl *sl, queue_t *q, mblk_t *mp)
{
}

/**
 * sl_recv_asps_aspup_req: - process received ASPS ASP Up message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the ASPS ASP Up message (with header)
 */
static int
sl_recv_asps_aspup_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	/* wrong direction */
	return (-EINVAL);
}

/**
 * sl_recv_asps_aspdn_req: - process received ASPS ASP Down message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the ASPS ASP Down message (with header)
 */
static int
sl_recv_asps_aspdn_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	/* wrong direction */
	return (-EINVAL);
}

/**
 * sl_recv_asps_hbeat_req: - process received ASPS BEAT message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the ASPS BEAT message (with header)
 */
static int
sl_recv_asps_hbeat_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
}

/**
 * sl_recv_asps_aspup_ack: - process received ASPS ASP Up Ack message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the ASPS ASP Up Ack message (with header)
 */
static int
sl_recv_asps_aspup_ack(struct sl *sl, queue_t *q, mblk_t *mp)
{
}

/**
 * sl_recv_asps_aspdn_ack: - process received ASPS ASP Down Ack message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the ASPS ASP Down Ack message (with header)
 */
static int
sl_recv_asps_aspdn_ack(struct sl *sl, queue_t *q, mblk_t *mp)
{
}

/**
 * sl_recv_asps_hbeat_ack: - process received ASPSP BEAT Ack message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the ASPSP BEAT Ack message (with header)
 */
static int
sl_recv_asps_hbeat_ack(struct sl *sl, queue_t *q, mblk_t *mp)
{
}

/**
 * sl_recv_aspt_aspac_req: - process received ASPT ASP Active message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the ASPT ASP Active message (with header)
 */
static int
sl_recv_aspt_aspac_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	/* wrong direction */
	return (-EINVAL);
}

/**
 * sl_recv_aspt_aspia_req: - process received ASPT ASP Inactive message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the ASPT ASP Inactive message (with header)
 */
static int
sl_recv_aspt_aspia_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	/* wrong direction */
	return (-EINVAL);
}

/**
 * sl_recv_aspt_aspac_ack: - process received ASPT ASP Active Ack message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the ASPT ASP Active Ack message (with header)
 */
static int
sl_recv_aspt_aspac_ack(struct sl *sl, queue_t *q, mblk_t *mp)
{
}

/**
 * sl_recv_aspt_aspia_ack: - process received ASPT ASP Inactive Ack message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the ASPT ASP Inactive Ack message (with header)
 */
static int
sl_recv_aspt_aspia_ack(struct sl *sl, queue_t *q, mblk_t *mp)
{
}

/**
 * sl_recv_maup_estab_req: - process received MAUP Establish Request message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MAUP Establish Request message (with header)
 */
static int
sl_recv_maup_estab_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	/* wrong direction */
	return (-EINVAL);
}

/**
 * sl_recv_maup_estab_con: - process received MAUP Establish Confirmation message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MAUP Establish Confirmation message (with header)
 *
 * This is simply SL_IN_SERVICE_IND.
 */
static int
sl_recv_maup_estab_con(struct sl *sl, queue_t *q, mblk_t *mp)
{
}

/**
 * sl_recv_maup_rel_req: - process received MAUP Release Request message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MAUP Release Request message (with header)
 */
static int
sl_recv_maup_rel_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	/* wrong direction */
	return (-EINVAL);
}

/**
 * sl_recv_maup_rel_con: - process received MAUP Release Confirmation message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MAUP Release Confirmation message (with header)
 *
 * This is discarded (unnecessary).
 */
static int
sl_recv_maup_rel_con(struct sl *sl, queue_t *q, mblk_t *mp)
{
}

/**
 * sl_recv_maup_rel_ind: - process received MAUP Release Indication message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MAUP Release Indication message (with header)
 *
 * This is simply SL_OUT_OF_SERVICE_IND.
 */
static int
sl_recv_maup_rel_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
}

/**
 * sl_recv_maup_state_req: - process received MAUP State Request message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MAUP State Request message (with header)
 */
static int
sl_recv_maup_state_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	/* wrong direction */
	return (-EINVAL);
}

/**
 * sl_recv_status_lpo_set: - process STATUS_LPO_SET status
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MAUP status message (with headers)
 */
static int
sl_recv_status_lpo_set(struct sl *sl, queue_t *q, mblk_t *mp)
{
	/* ignore */
	return (QR_DONE);
}

/**
 * sl_recv_status_lpo_clear: - process STATUS_LPO_CLEAR status
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MAUP status message (with headers)
 */
static int
sl_recv_status_lpo_clear(struct sl *sl, queue_t *q, mblk_t *mp)
{
	/* ignore */
	return (QR_DONE);
}

/**
 * sl_recv_status_emer_set: - process STATUS_EMER_SET status
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MAUP status message (with headers)
 */
static int
sl_recv_status_emer_set(struct sl *sl, queue_t *q, mblk_t *mp)
{
	/* ignore */
	return (QR_DONE);
}

/**
 * sl_recv_status_emer_clear: - process STATUS_EMER_CLEAR status
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MAUP status message (with headers)
 */
static int
sl_recv_status_emer_clear(struct sl *sl, queue_t *q, mblk_t *mp)
{
	/* ignore */
	return (QR_DONE);
}

/**
 * sl_recv_status_flush_buffers: - process STATUS_FLUSH_BUFFERS status
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MAUP status message (with headers)
 */
static int
sl_recv_status_flush_buffers(struct sl *sl, queue_t *q, mblk_t *mp)
{
	return sl_rb_cleared_ind(sl, q);
}

/**
 * sl_recv_status_continue: - process STATUS_CONTINUE status
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MAUP status message (with headers)
 */
static int
sl_recv_status_continue(struct sl *sl, queue_t *q, mblk_t *mp)
{
	/* ignore */
	return (QR_DONE);
}

/**
 * sl_recv_status_clear_rtb: - process STATUS_CLEAR_RTB status
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MAUP status message (with headers)
 */
static int
sl_recv_status_clear_rtb(struct sl *sl, queue_t *q, mblk_t *mp)
{
	return sl_rtb_cleared_ind(sl, q);
}

/**
 * sl_recv_status_audit: - process STATUS_AUDIT status
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MAUP status message (with headers)
 */
static int
sl_recv_status_audit(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl->audit = 0;
	return (QR_DONE);
}

/**
 * sl_recv_status_cong_clear: - process STATUS_CONG_CLEAR status
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MAUP status message (with headers)
 */
static int
sl_recv_status_cong_clear(struct sl *sl, queue_t *q, mblk_t *mp)
{
	/* ignore */
	return (QR_DONE);
}

/**
 * sl_recv_status_cong_accept: - process STATUS_CONG_ACCEPT status
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MAUP status message (with headers)
 */
static int
sl_recv_status_cong_accept(struct sl *sl, queue_t *q, mblk_t *mp)
{
	/* ignore */
	return (QR_DONE);
}

/**
 * sl_recv_status_cong_discard: - process STATUS_CONG_DISCARD status
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MAUP status message (with headers)
 */
static int
sl_recv_status_cong_discard(struct sl *sl, queue_t *q, mblk_t *mp)
{
	/* ignore */
	return (QR_DONE);
}

/**
 * sl_recv_maup_state_con: - process received MAUP State Confirmation message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MAUP State Confirmation message (with header)
 *
 * These get translated into one of the other status indications.  There are
 * a couple of such commands that are expected to take some time under SS7
 * such as 
 */
static int
sl_recv_maup_state_con(struct sl *sl, queue_t *q, mblk_t *mp)
{
	switch (status) {
	case M2UA_STATUS_LPO_SET:	/* (0x00) */
		return sl_recv_status_lpo_set(sl, q, mp);
	case M2UA_STATUS_LPO_CLEAR:	/* (0x01) */
		return sl_recv_status_lpo_clear(sl, q, mp);
	case M2UA_STATUS_EMER_SET:	/* (0x02) */
		return sl_recv_status_emer_set(sl, q, mp);
	case M2UA_STATUS_EMER_CLEAR:	/* (0x03) */
		return sl_recv_status_emer_clear(sl, q, mp);
	case M2UA_STATUS_FLUSH_BUFFERS:	/* (0x04) */
		return sl_recv_status_flush_buffers(sl, q, mp);
	case M2UA_STATUS_CONTINUE:	/* (0x05) */
		return sl_recv_status_continue(sl, q, mp);
	case M2UA_STATUS_CLEAR_RTB:	/* (0x06) */
		return sl_recv_status_clear_rtb(sl, q, mp);
	case M2UA_STATUS_AUDIT:	/* (0x07) */
		return sl_recv_status_audit(sl, q, mp);
	case M2UA_STATUS_CONG_CLEAR:	/* (0x08) */
		return sl_recv_status_cong_clear(sl, q, mp);
	case M2UA_STATUS_CONG_ACCEPT:	/* (0x09) */
		return sl_recv_status_cong_accept(sl, q, mp);
	case M2UA_STATUS_CONG_DISCARD:	/* (0x0a) */
		return sl_recv_status_cong_discard(sl, q, mp);
	default:
		return (-EOPNOTSUPP);
	}
}

/**
 * sl_recv_event_rpo_enter: - process received EVENT_RPO_ENTER event
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the message containing all headers
 */
static int
sl_recv_event_rpo_enter(struct sl *sl, queue_t *q, mblk_t *mp)
{
	return sl_remote_processor_outage_ind(sl, q);
}

/**
 * sl_recv_event_rpo_enter: - process received EVENT_RPO_ENTER event
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the message containing all headers
 */
static int
sl_recv_event_rpo_exit(struct sl *sl, queue_t *q, mblk_t *mp)
{
	return sl_remote_processor_recovered_ind(sl, q);
}

/**
 * sl_recv_event_rpo_enter: - process received EVENT_RPO_ENTER event
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the message containing all headers
 */
static int
sl_recv_event_lpo_enter(struct sl *sl, queue_t *q, mblk_t *mp)
{
	return sl_local_processor_outage_ind(sl, q);
}

/**
 * sl_recv_event_rpo_enter: - process received EVENT_RPO_ENTER event
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the message containing all headers
 */
static int
sl_recv_event_lpo_exit(struct sl *sl, queue_t *q, mblk_t *mp)
{
	return sl_local_processor_recovered_ind(sl, q);
}

/**
 * sl_recv_maup_state_ind: - process received MAUP State Indication message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MAUP State Indication message (with header)
 */
static int
sl_recv_maup_state_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	switch (event) {
	case M2UA_EVENT_RPO_ENTER:
		return sl_recv_event_rpo_enter(sl, q, mp);
	case M2UA_EVENT_RPO_EXIT:
		return sl_recv_event_rpo_exit(sl, q, mp);
	case M2UA_EVENT_LPO_ENTER:
		return sl_recv_event_lpo_enter(sl, q, mp);
	case M2UA_EVENT_LPO_EXIT:
		return sl_recv_event_lpo_exit(sl, q, mp);
	default:
		return (-EOPNOTSUPP);
	}
}

/**
 * sl_recv_maup_retr_req: - process received MAUP Retrieval Request message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MAUP Retrieval Request message (with header)
 */
static int
sl_recv_maup_retr_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	/* wrong direction */
	return (-EINVAL);
}

/**
 * sl_recv_maup_retr_con: - process received MAUP Retrieval Confirm message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MAUP Retrieval Confirm message (with header)
 */
static int
sl_recv_maup_retr_con(struct sl *sl, queue_t *q, mblk_t *mp)
{
	switch (action) {
	case M2UA_ACTION_RTRV_BSN:
		return sl_bsnt_ind(sl, q, bsnt);
	case M2UA_ACTION_RTRV_MSGS:
		/* ignore */
		return (QR_DONE);
	default:
		return (-EINVAL);
	}
}

/**
 * sl_recv_maup_retr_ind: - process received MAUP Retrieval Indication message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MAUP Retrieval Indication message (with header)
 */
static int
sl_recv_maup_retr_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	int err;
	unsigned char *b_rptr = mp->b_rptr;

	mp->b_rptr += 3 * sizeof(uint32_t);
	if (likely((err = sl_retrieved_message_ind(sl, q, mp)) == 0))
		return (QR_DONE);
	mp->b_rptr = b_rptr;
	return (err);
}

/**
 * sl_recv_maup_retr_comp_ind: - process received MAUP Retrieval Complete Indication message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MAUP Retrieval Complete Indication message (with header)
 */
static int
sl_recv_maup_retr_comp_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	int err;
	unsigned char *b_rptr = mp->b_rptr;

	mp->b_rptr += 3 * sizeof(uint32_t);
	if (likely((err = sl_retrieval_complete_ind(sl, q, mp)) == 0))
		return (QR_DONE);
	mp->b_rptr = b_rptr;
	return (err);
}

/**
 * sl_recv_maup_cong_ind: - process received MAUP Congestion Indication message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MAUP Congestion Indication message (with header)
 */
static int
sl_recv_maup_cong_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (on_the_increase)
		return sl_link_congested_ind(sl, q, cong, disc);
	return sl_link_congestion_ceased_ind(sl, q, cong, disc);
}

/**
 * sl_recv_maup_data: - process received MAUP Data message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MAUP Data message (with header)
 */
static int
sl_recv_maup_data(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (likely((err = sl_pdu_ind(sl, q, mp)) == 0))
		return (QR_DONE);
}

/**
 * sl_recv_maup_data_ack: - process received MAUP Data Ack message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MAUP Data Ack message (with header)
 */
static int
sl_recv_maup_data_ack(struct sl *sl, queue_t *q, mblk_t *mp)
{
}

/**
 * sl_recv_rkmm_reg_req: - process received RKMM REG REQ message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the RKMM REG REQ message (with header)
 */
static int
sl_recv_rkmm_reg_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	/* wrong direction */
	return (-EINVAL);
}

/**
 * sl_recv_rkmm_req_rsp: - process received RKMM REG RSP message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the RKMM REG RSP message (with header)
 */
static int
sl_recv_rkmm_reg_rsp(struct sl *sl, queue_t *q, mblk_t *mp)
{
}

/**
 * sl_recv_rkmm_dereg_req: - process received RKMM DEREG REQ message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the RKMM DEREG REQ message (with header)
 */
static int
sl_recv_rkmm_dereg_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	/* wrong direction */
	return (-EINVAL);
}

/**
 * sl_recv_rkmm_dereg_rsp: - process received RKMM DEREG RSP message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the RKMM DEREG RSP message (with header)
 */
static int
sl_recv_rkmm_dereg_rsp(struct sl *sl, queue_t *q, mblk_t *mp)
{
}

/**
 * sl_recv_msg: - process message received from M2UA peer (SG)
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the message (just the M2UA part)
 */
static int
sl_recv_msg(struct sl *sl, queue_t *q, mblk_t *mp)
{
	register uint32_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + 2 * sizeof(*p))
		return (-EMSGSIZE);
	if (mp->b_wptr < mp->b_wptr + ntohl(p[1]))
		return (-EMSGSIZE);
	switch (UA_MSG_CLAS(p[0])) {
	case UA_CLASS_MGMT:
		switch (UA_MSG_TYPE(p[0])) {
		case UA_MGMT_ERR:
			return sl_recv_mgmt_err(sl, q, mp);
		case UA_MGMT_NTFY:
			return sl_recv_mgmt_ntfy(sl, q, mp);
		}
		return (-EOPNOTSUPP);
	case UA_CLASS_XFER:
		return (-ENOPROTOOPT);
	case UA_CLASS_SNMM:
		return (-ENOPROTOOPT);
	case UA_CLASS_ASPS:
		switch (UA_MSG_TYPE(p[0])) {
		case UA_ASPS_ASPUP_REQ:
			return sl_recv_asps_aspup_req(sl, q, mp);
		case UA_ASPS_ASPDN_REQ:
			return sl_recv_asps_aspdn_req(sl, q, mp);
		case UA_ASPS_HBEAT_REQ:
			return sl_recv_asps_hbeat_req(sl, q, mp);
		case UA_ASPS_ASPUP_ACK:
			return sl_recv_asps_aspup_ack(sl, q, mp);
		case UA_ASPS_ASPDN_ACK:
			return sl_recv_asps_aspdn_ack(sl, q, mp);
		case UA_ASPS_HBEAT_ACK:
			return sl_recv_asps_hbeat_ack(sl, q, mp);
		}
		return (-EOPNOTSUPP);
	case UA_CLASS_ASPT:
		switch (UA_MSG_TYPE(p[0])) {
		case UA_ASPT_ASPAC_REQ:
			return sl_recv_aspt_aspac_req(sl, q, mp);
		case UA_ASPT_ASPIA_REQ:
			return sl_recv_aspt_aspia_req(sl, q, mp);
		case UA_ASPT_ASPAC_ACK:
			return sl_recv_aspt_aspac_ack(sl, q, mp);
		case UA_ASPT_ASPIA_ACK:
			return sl_recv_aspt_aspia_ack(sl, q, mp);
		}
		return (-EOPNOTSUPP);
	case UA_CLASS_Q921:
		return (-ENOPROTOOPT);
	case UA_CLASS_MAUP:
		switch (UA_MSG_TYPE(p[0])) {
		case M2UA_MAUP_ESTAB_REQ:
			return sl_recv_maup_estab_req(sl, q, mp);
		case M2UA_MAUP_ESTAB_CON:
			return sl_recv_maup_estab_con(sl, q, mp);
		case M2UA_MAUP_REL_REQ:
			return sl_recv_maup_rel_req(sl, q, mp);
		case M2UA_MAUP_REL_CON:
			return sl_recv_maup_rel_con(sl, q, mp);
		case M2UA_MAUP_REL_IND:
			return sl_recv_maup_rel_ind(sl, q, mp);
		case M2UA_MAUP_STATE_REQ:
			return sl_recv_maup_state_req(sl, q, mp);
		case M2UA_MAUP_STATE_CON:
			return sl_recv_maup_state_con(sl, q, mp);
		case M2UA_MAUP_STATE_IND:
			return sl_recv_maup_state_ind(sl, q, mp);
		case M2UA_MAUP_RETR_REQ:
			return sl_recv_maup_retr_req(sl, q, mp);
		case M2UA_MAUP_RETR_CON:
			return sl_recv_maup_retr_con(sl, q, mp);
		case M2UA_MAUP_RETR_IND:
			return sl_recv_maup_retr_ind(sl, q, mp);
		case M2UA_MAUP_RETR_COMP_IND:
			return sl_recv_maup_retr_comp_ind(sl, q, mp);
		case M2UA_MAUP_CONG_IND:
			return sl_recv_maup_cong_ind(sl, q, mp);
		case M2UA_MAUP_DATA:
			return sl_recv_maup_data(sl, q, mp);
		case M2UA_MAUP_DATA_ACK:
			return sl_recv_maup_data_ack(sl, q, mp);
		}
		return (-EOPNOTSUPP);
	case UA_CLASS_CNLS:
		return (-ENOPROTOOPT);
	case UA_CLASS_CONS:
		return (-ENOPROTOOPT);
	case UA_CLASS_RKMM:
		switch (UA_MSG_TYPE(p[0])) {
		case UA_RKMM_REG_REQ:
			return sl_recv_rkmm_reg_req(sl, q, mp);
		case UA_RKMM_REG_RSP:
			return sl_recv_rkmm_reg_rsp(sl, q, mp);
		case UA_RKMM_DEREG_REQ:
			return sl_recv_rkmm_dereg_req(sl, q, mp);
		case UA_RKMM_DEREG_RSP:
			return sl_recv_rkmm_dereg_rsp(sl, q, mp);
		}
		return (-EOPNOTSUPP);
	case UA_CLASS_TDHM:
		return (-ENOPROTOOPT);
	case UA_CLASS_TCHM:
		return (-ENOPROTOOPT);
	}
	return (-ENOPROTOOPT);
}
