/*****************************************************************************

 @(#) $Id: ua_lm.h,v 0.9.2.3 2007/08/14 08:34:06 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

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

 Last Modified $Date: 2007/08/14 08:34:06 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: ua_lm.h,v $
 Revision 0.9.2.3  2007/08/14 08:34:06  brian
 - GPLv3 header update

 *****************************************************************************/

#ifndef __SS7_UA_LM_H__
#define __SS7_UA_LM_H__

#ident "@(#) $RCSfile: ua_lm.h,v $ $Name:  $($Revision: 0.9.2.3 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

/* This file can be processed by doxygen(1). */

/*
 *  Includes for Layer Management Primitives: used by the UA configuration
 *  daemons and some by the user in Application Server mode.
 */

#define LM_SG_ADD_REQ		51
#define LM_SG_DEL_REQ		52
#define LM_AS_ADD_REQ		53
#define LM_AS_DEL_REQ		54
#define LM_PROC_ADD_REQ		55
#define LM_PROC_DEL_REQ		56
#define LM_LINK_ADD_REQ		57
#define LM_LINK_DEL_REQ		58
#define LM_ROUTE_ADD_REQ	59
#define LM_ROUTE_DEL_REQ	60
#define LM_REG_RES		61
#define LM_REG_REF		62
#define LM_INFO_REQ		63

#define LM_LINK_IND		71	/* something happend on link below */
#define LM_OK_ACK		72
#define LM_ERROR_ACK		73
#define LM_REG_IND		74
#define LM_DEREG_IND		75
#define LM_ERROR_IND		76
#define LM_INFO_ACK		77

/*
 *  LM_INFO_REQ
 *  -------------------------------------------------------------------------
 *  Request information about the SP multiplexor which has been opened.
 */
typedef struct {
	ulong prim;			/* always LM_INFO_REQ */
} lm_info_req_t;

/*
 *  LM_INFO_ACK
 *  -------------------------------------------------------------------------
 *  This returns information about the SP multiplexor which has been opened.
 *  For now it just returns the identifier (clone minor number) of the SP
 *  associated with the multiplexor.  Opening /dev/xxua_asp0X will open
 *  another AS/LM stream for the multiplexor, where xx is the UA protocol and
 *  X is the spid.
 */
typedef struct {
	ulong prim;			/* always LM_INFO_ACK */
	ulong spid;			/* SP Identifier */
} lm_info_ack_t;

/*
 *  Process types:
 */
#define LM_ASP	0x01		/* ASP - Application Server Process proxy */
#define LM_SGP	0x02		/* SGP - Signalling Gateway Process proxy */
#define LM_SPP	0x04		/* SPP - Signalling Point Process proxy */

#define LM_SS7	0x10		/* The ASP/SGP is an SS7 stream */
#define LM_IPC	0x20		/* The ASP/SGP is an IPC stream */
#define LM_UAP	0x40		/* The ASP/SGP/SPP is a UA proxy */

/*
 *  LM_SG_ADD_REQ
 *  -------------------------------------------------------------------------
 *  Requests that the driver add or augment the signalling gateway fro the
 *  protocol on whose user or control queue the request was issued.  The AS
 *  identifier is for further reference.  The layer or local management queue
 *  normally uses this primitive to add a signallingg gateway which does not
 *  yet exist.  This is normally only done on a node acting as an ASP.  The
 *  traffic mode describes the traffic mode to be used between the SGP on the
 *  SG.  For ASP operation, the aspid is used to inidcate the idetnifier that
 *  the ASP will use when it goes up for the given SG.  Only one SP is ever
 *  associated with an SG.
 */
typedef struct {
	ulong prim;			/* always LM_SG_ADD_REQ */
	ulong sgid;			/* SG identifier */
	ulong flags;			/* flags */
	ulong tmode;			/* Traffic Mode */
	ulong prio;			/* priority */
	ulong cost;			/* cost */
	ulong aspid;			/* ASP Identifier */
} lm_sg_add_req_t;

typedef struct {
	ulong prim;			/* always LM_SG_DEL_REQ */
	ulong sgid;			/* SG identifier */
} lm_sg_del_req_t;

#define LM_SG_AUTODEL	0x01	/* the SG will be removed when unused */
#define LM_SG_PERM	0x02	/* the SG will survive loss of lmq */

/*
 *  LM_AS_ADD_REQ
 *  -------------------------------------------------------------------------
 *  Requests that the driver add or augment the application server for the
 *  protocol on whose user or control queue the request was issued.  The AS
 *  identifier is for further reference.  The RC or IID identifier will be
 *  added to the AS if the AS already exists with the specified AS Id.
 *
 *  The layer or local management queue normally uses this primitive to add an
 *  application server which does not yet exist at boot time or in response to
 *  an LM_REG_REQ before issuing an LM_REG_RES.
 *
 *  The optional routing (link) key is only required for statically allocated
 *  Application Servers.  This key is used to bind/connect SS7 provider
 *  streams which support the AS or to generate REG REQ to SGP/SPP IPC and
 *  UA proxies.  I don't really intend to use this function, but it is
 *  there to support all SIGTRAN UA allocation modes.
 *
 *  The ASID is provided to uniquely identify the AS within the system.
 *
 *  The RC is only required for statically allocated AS.  That is, if dynamic
 *  registration is desired, the RC value should be zero.
 *
 *  The SGID is provided to provision an AS on an ASP which is serviced by a
 *  remote SG.
 *
 *  The MUXID is provided to provision an AS on an SGP which is serviced by a
 *  local SS7 provider.  The MUXID is the identifier of the local SS7 provider
 *  stream.
 *
 *  Only one of SGID and MUXID should be zero, the other a specified value.
 */
typedef struct {
	ulong prim;			/* always LM_AS_ADD_REQ */
	ulong asid;			/* Application Server Index */
	ulong flags;			/* Allocation flags */
	ulong rc;			/* RC or IID for the AS */
	ulong tmode;			/* Traffic Mode */
	ulong sgid;			/* SG identifier */
	ulong muxid;			/* multiplexor ID */
	ulong KEY_offset;		/* Routing (Link) Key offset */
	ulong KEY_length;		/* Routing (Link) Key length */
} lm_as_add_req_t;

/*
 *  LM_AS_DEL_REQ
 *  -------------------------------------------------------------------------
 *  Requests that the driver remove the application server for the protocol on
 *  whose local or layer management stream the request was issued.  The AS
 *  identifier must be the same as that provided in the LM_AS_ADD_REQ.
 */
typedef struct {
	ulong prim;			/* always LM_AS_DEL_REQ */
	ulong asid;			/* Application Server Index */
} lm_as_del_req_t;

#define LM_AS_STATIC	(1<<0)	/* AS has static allocation */
#define LM_AS_DYNAMIC	(1<<1)	/* AS has dynamic registration */

/*
 *  LM_PROC_ADD_REQ
 *  -------------------------------------------------------------------------
 *  Requests that the driver add an ASP/SGP/SPP proxy process.  This is for
 *  static configuration of proxy processes only.  Dynamic configuration will
 *  create SGP and SPP proxy processes on demand.
 */
typedef struct {
	ulong prim;			/* always LM_PROC_ADD_REQ */
	ulong type;			/* type of process */
	ulong aspid;			/* process identifier */
	ulong spid;			/* SP to which to add proc */
	ulong load;			/* load selector */
	ulong prio;			/* routing priority */
	ulong cost;			/* routing cost */
} lm_proc_add_req_t;

/*
 *  LM_PROC_DEL_REQ
 *  -------------------------------------------------------------------------
 *  Requests that the driver delete the indicated ASP/SGP/SPP proxy process.
 *  This is for static configuration of proxy processes only.  Dynamic
 *  configuration will delete SGP and SPP proxy processes on demand.
 */
typedef struct {
	ulong prim;			/* always LM_PROC_DEL_REQ */
	ulong type;			/* type of process */
	ulong aspid;			/* process identifier */
	ulong spid;			/* SP from which to del proc */
} lm_proc_del_req_t;

/*
 *  LM_LINK_ADD_REQ
 *  -------------------------------------------------------------------------
 *  Requests that the driver configures and associates the indicated linked
 *  (muxid) stream as the indicated type of proxy and associate it with the
 *  indicated local or remote ASP identifier.  Transport SGP or SPP proxies
 *  must have aspids associated with them.  Transport ASP proxies have may
 *  either have fixed aspids associated with them or may have aspid set to
 *  zero, the aspid to be identified in a subsequent ASPUP message from the
 *  ASP if the ASP requires dynamic registration.  SGP/SPP proxies have their
 *  sgid set to the SG to which they are to be attached.  This field is
 *  ignored for other proxy types.  If the sgid is set to zero, it applies to
 *  the default SG, regardless of the identifier associated with the default
 *  SG.
 */
typedef struct {
	ulong prim;			/* always LM_LINK_ADD_REQ */
	ulong muxid;			/* multiplexor ID */
	ulong type;			/* type of process */
	ulong aspid;			/* process Identifier */
	ulong spid;			/* SP identifier */
} lm_link_add_req_t;

/*
 *  Link add flags:
 */
#define LM_DEFAULT_PROTOID	0x00000001	/* use proto as default */

/*
 *  LM_LINK_DEL_REQ
 *  -------------------------------------------------------------------------
 *  Requests that the driver disassociate and possibly free the indicated
 *  linked (muxid) stream from the indicated protocol, process type, and
 *  process.
 */
typedef struct {
	ulong prim;			/* always LM_LINK_DEL_REQ */
	ulong muxid;			/* multiplexor ID */
} lm_link_del_req_t;

/*
 *  LM_ROUTE_ADD_REQ
 *  -------------------------------------------------------------------------
 */
typedef struct {
	ulong prim;			/* always LM_ROUTE_ADD_REQ */
} lm_route_add_req_t;

/*
 *  LM_ROUTE_DEL_REQ
 *  -------------------------------------------------------------------------
 */
typedef struct {
	ulong prim;			/* always LM_ROUTE_DEL_REQ */
} lm_route_del_req_t;

/*
 *  LM_OK_ACK
 *  -------------------------------------------------------------------------
 *  Used by the driver to indicate success of a primitive issued by local or
 *  layer management.
 */
typedef struct {
	ulong prim;			/* always LM_OK_ACK */
	ulong correct_prim;		/* correct primitive */
} lm_ok_ack_t;

/*
 *  LM_ERROR_ACK
 *  -------------------------------------------------------------------------
 *  Used by the driver to indicate failure of a primitive issued by local or
 *  layer management.
 */
typedef struct {
	ulong prim;			/* always LM_ERROR_ACK */
	ulong error_prim;		/* primitive in error */
	ulong lm_error;			/* LM error number */
	ulong unix_error;		/* Unix error code */
} lm_error_ack_t;

#define LMSYSERR    1

/*
 *  LM_REG_IND
 *  -------------------------------------------------------------------------
 *  This indication indicates to management that a registration request has
 *  been received from an ASP with the specified protocol Id and ASP Id and
 *  the indicated routing keys.  If the registration request is acceptable
 *  management responds with a LM_REG_RES indicating the configured
 *  application server by routing context.  If the registration request fails,
 *  management responds with a LM_REG_REF refusing the request.
 */
typedef struct {
	ulong prim;			/* always LM_REG_IND */
	ulong muxid;			/* stream receiving request */
	ulong aspid;			/* ASP generating request */
	ulong load;			/* Load selection of request *//* XXX */
	ulong prio;			/* priority of request *//* XXX */
	ulong KEY_number;		/* routing key number */
	ulong KEY_offset;		/* routing key offset */
	ulong KEY_length;		/* routing key length */
	/* 
	   followed routing keys */
} lm_reg_ind_t;

/*
 *  LM_REG_RES
 *  -------------------------------------------------------------------------
 *  The response tells the multiplexer that layer management has accepted a
 *  registration request and provides the routing context.
 */
typedef struct {
	ulong prim;			/* always LM_REG_RES */
	ulong muxid;			/* stream receiving request */
	ulong aspid;			/* ASP generating request */
	ulong load;			/* Load selection of request *//* XXX */
	ulong prio;			/* priority of request *//* XXX */
	ulong context;			/* RC/IID of application server */
} lm_reg_res_t;

/*
 *  LM_ERROR_IND
 *  -------------------------------------------------------------------------
 *  The driver uses this primitive to indicate a fatal error with respect to a
 *  linked stream which requires that the stream be unlinked.
 */
typedef struct {
	ulong prim;			/* always LM_ERROR_IND */
	ulong muxid;			/* id of linked stream */
	ulong lm_error;			/* LM error number */
	ulong unix_error;		/* Unix error code */
} lm_error_ind_t;

union LM_primitives {
	ulong prim;
	lm_sg_add_req_t sg_add;
	lm_sg_del_req_t sg_del;
	lm_as_add_req_t as_add;
	lm_as_del_req_t as_del;
	lm_proc_add_req_t proc_add;
	lm_proc_del_req_t proc_del;
	lm_link_add_req_t link_add;
	lm_link_del_req_t link_del;
	lm_route_add_req_t route_add;
	lm_route_del_req_t route_del;
	lm_ok_ack_t ok_ack;
	lm_error_ack_t error_ack;
	lm_reg_ind_t reg_ind;
	lm_reg_res_t reg_res;
	lm_error_ind_t error_ind;
	lm_info_req_t info_req;
	lm_info_ack_t info_ack;
};

#endif				/* __SS7_UA_LM_H__ */
