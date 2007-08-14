/*****************************************************************************

 @(#) $Id: ua_as_lm.h,v 0.9.2.4 2007/08/14 08:34:06 brian Exp $

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

 $Log: ua_as_lm.h,v $
 Revision 0.9.2.4  2007/08/14 08:34:06  brian
 - GPLv3 header update

 Revision 0.9.2.3  2007/02/14 14:09:00  brian
 - broad changes updating support for SS7 MTP and M3UA

 Revision 0.9.2.2  2007/02/10 22:32:15  brian
 - working up sigtran drivers

 Revision 1.1.2.1  2007/02/03 03:05:18  brian
 - added new files

 *****************************************************************************/

#ifndef __SYS_UA_AS_LM_H__
#define __SYS_UA_AS_LM_H__

#define UA_USER_REQ	(('U'<<8)+ 0)	/* User management request */
#define UA_ATTACH_REQ	(('U'<<8)+ 1)	/* Attach user request */
#define UA_DETACH_REQ	(('U'<<8)+ 2)	/* Detach user request */
#define UA_SCTP_REQ	(('U'<<8)+ 3)	/* SCTP management request */
#define UA_LINK_REQ	(('U'<<8)+ 4)	/* Link SCTP request */
#define UA_UNLINK_REQ	(('U'<<8)+ 5)	/* Unlink SCTP request */
#define UA_ASPUP_REQ	(('U'<<8)+ 6)	/* ASP Up request */
#define UA_ASPDN_REQ	(('U'<<8)+ 7)	/* ASP Down request */
#define UA_HBEAT_REQ	(('U'<<8)+ 8)	/* Heartbeat request */
#define UA_INFO_REQ	(('U'<<8)+ 9)	/* ASP information request */
#define UA_ASPAC_REQ	(('U'<<8)+10)	/* ASP Active request */
#define UA_ASPIA_REQ	(('U'<<8)+11)	/* ASP Inactive request */
#define UA_ASINFO_REQ	(('U'<<8)+12)	/* AS information request */
#define UA_REG_REQ	(('U'<<8)+13)	/* Registration request */
#define UA_DEREG_REQ	(('U'<<8)+14)	/* Deregistration request */

#define UA_USER_IND	(('U'<<8)+15)	/* User managment indication */
#define UA_ATTACH_ACK	(('U'<<8)+16)	/* Attach user acknowledgement */
#define UA_SCTP_IND	(('U'<<8)+17)	/* SCTP management indication */
#define UA_LINK_ACK	(('U'<<8)+18)	/* Link SCTP acknowledgement */
#define UA_OK_ACK	(('U'<<8)+19)	/* Successful receipt acknowledgement */
#define UA_ERROR_ACK	(('U'<<8)+20)	/* Error acknowlegment */
#define UA_ASPUP_CON	(('U'<<8)+21)	/* ASP Up confirmation */
#define UA_ASPUP_IND	(('U'<<8)+22)	/* ASP Up indication */
#define UA_ASPDN_CON	(('U'<<8)+23)	/* ASP Down confirmation */
#define UA_ASPDN_IND	(('U'<<8)+24)	/* ASP Down indication */
#define UA_HBEAT_CON	(('U'<<8)+25)	/* Heartbeat confirmation */
#define UA_HBEAT_IND	(('U'<<8)+26)	/* Heartbeat indication */
#define UA_INFO_ACK	(('U'<<8)+27)	/* ASP information acknowledgement */
#define UA_ASPAC_CON	(('U'<<8)+28)	/* ASP Active confirmation */
#define UA_ASPAC_IND	(('U'<<8)+29)	/* ASP Active indication */
#define UA_ASPIA_CON	(('U'<<8)+30)	/* ASP Inactive confirmation */
#define UA_ASPIA_IND	(('U'<<8)+31)	/* ASP Inactive indication */
#define UA_ASINFO_ACK	(('U'<<8)+32)	/* AS information acknowledgement */
#define UA_REG_RSP	(('U'<<8)+33)	/* Registration response */
#define UA_DEREG_RSP	(('U'<<8)+34)	/* Deregistration response */
#define UA_ERROR_CON	(('U'<<8)+35)	/* Error confirmation */
#define UA_ERROR_IND	(('U'<<8)+36)	/* Error indication */
#define UA_NOTIFY_IND	(('U'<<8)+37)	/* Notification */

/* ASP States */
#define ASP_DOWN	0	/* ASP is down */
#define ASP_WACK_ASPUP	1	/* ASP is coming up */
#define ASP_WACK_ASPDN	2	/* ASP is going down */
#define ASP_UP		3	/* ASP is up */

/* AS States */
#define AS_DOWN		0	/* AS is deregistered */
#define AS_WRSP_RREQ	1	/* AS is registering */
#define AS_WRSP_DREQ	2	/* AS is deregistering */
#define AS_INACTIVE	3	/* AS is inactive */
#define AS_WACK_ASPAC	4	/* AS is activating */
#define AS_WACK_ASPIA	5	/* AS is deactivating */
#define AS_ACTIVE	6	/* AS is active */

/* ASP Extension flags */
#define ASPEXT_ASPEXT	(1<< 0)	/* ASPEXT extension */
#define ASPEXT_SGINFO	(1<< 1)	/* SGINFO extension */
#define ASPEXT_LOADSEL	(1<< 2)	/* LOADSEL extension */
#define ASPEXT_LOADGRP	(1<< 3)	/* LOADGRP extension */
#define ASPEXT_CORID	(1<< 4)	/* CORID extension */
#define ASPEXT_REGEXT	(1<< 5)	/* REGEXT extension */
#define ASPEXT_SESSID	(1<< 6)	/* SESSID extension */
#define ASPEXT_DYNAMIC	(1<< 7)	/* DYNAMIC option */
#define ASPEXT_DEIPSP	(1<< 8)	/* DEIPSP option */
#define ASPEXT_ASPCONG	(1<< 9)	/* ASPCONG extension */
#define ASPEXT_TEXTIID	(1<<10)	/* TEXTIID option */

/* UA reason codes */
#define UA_REASON_UNSPECIFIED			0	/* reason is unspecified or unknown */
#define UA_REASON_INVALID_VERSION		1
#define UA_REASON_UNSUPPORTED_MESSAGE_CLASS	2
#define UA_REASON_UNSUPPORTED_MESSAGE_TYPE	3
#define UA_REASON_UNSUPPORTED_TRAFFIC_MODE	4
#define UA_REASON_UNEXPECTED_MESSAGE		5
#define UA_REASON_PROTOCOL_ERROR
#define UA_REASON_UNSUPPORTED_IID_TYPE
#define UA_REASON_INVALID_STREAMS_IDENTIFIER
#define UA_REASON_UNASSIGNED_TEI
#define UA_REASON_UNRECOGNIZED_SAPI
#define UA_REASON_INVALID_TEI_SAPI_COMBINATION
#define UA_REASON_REFUSED_MANAGEMENT_BLOCKING
#define UA_REASON_ASPID_REQUIRED
#define UA_REASON_INVALID_ASPID
#define UA_REASON_ASP_ACTIVE_FOR_IDS
#define UA_REASON_INVALID_PARAMETER_VALUE
#define UA_REASON_PARAMETER_FIELD_ERROR
#define UA_REASON_UNEXPECTED_PARAMETER
#define UA_REASON_DESTINATION_STATUS_UNKNOWN
#define UA_REASON_INVALID_NETWORK_APPERANCE
#define UA_REASON_MISSING_PARAMETER
#define UA_REASON_ROUTING_KEY_CHANGE_REFUSED
#define UA_REASON_INVALID_ROUTING_CONTEXT
#define UA_REASON_NO_CONFIGURED_AS_FOR_ASP
#define UA_REASON_SYBSYSTEM_STATUS_UNKNOWN
#define UA_REASON_DEREG_STATUS_UNKNOWN
#define UA_REASON_DEREG_STATUS_INVALID_ID
#define UA_REASON_DEREG_STATUS_PERMISSION_DENIED
#define UA_REASON_DEREG_STATUS_NOT_REGISTERED
#define UA_REASON_DEREG_STATUS_ID_ACTIVE_FOR_AS
#define UA_REASON_REG_STATUS_UNKNOWN				0x201
#define UA_REASON_REG_STATUS_INVALID_DPC			0x202
#define UA_REASON_REG_STATUS_INVALID_NETWORK_APPEARANCE		0x203
#define UA_REASON_REG_STATUS_INVALID_ROUTING_KEY		0x204
#define UA_REASON_REG_STATUS_PERMISSION_DENIED			0x205
#define UA_REASON_REG_STATUS_CANNOT_SUPPORT_UNIQUE_ROUTING	0x206
#define UA_REASON_REG_STATUS_KEY_NOT_PROVISIONED		0x207
#define UA_REASON_REG_STATUS_INSUFFICIENT_RESOURCES		0x208
#define UA_REASON_REG_STATUS_UNSPPORTED_KEY_PARAMETER_FIELD	0x209
#define UA_REASON_REG_STATUS_INVALID_TRAFFIC_MODE		0x20a
#define UA_REASON_REG_STATUS_KEY_CHANGE_REFUSED			0x20b
#define UA_REASON_REG_STATUS_KEY_ALREADY_REGISTERED		0x20c

/*
 *  UA_USER_REQ - Request managemnt of a USER stream (M_PROTO)
 *  -------------------------------------------------------------------------
 */
struct UA_user_req {
	int PRIM_type;			/* always UA_USER_REQ */
	int USER_id;			/* USR identifier (0 for default) */
	/* followed by USER primitive */
};

/*
 *  UA_USER_IND - Indicate management of a USER stream (M_PROTO)
 *  -------------------------------------------------------------------------
 */
struct UA_user_ind {
	int PRIM_type;			/* always UA_USER_IND */
	int USER_id;			/* USR identifier */
	/* followed by USER primitive */
};

/*
 *  UA_ATTACH_REQ - Link a Stream to an ASP/SGP/AS (M_PROTO)
 *  -------------------------------------------------------------------------
 *  Link an Stream to an SGP/ASP/AS.  USER_id is the multiplex identifier
 *  returned by the I_LINK(7) or I_PLINK(7) input-output control, or for an AS
 *  may be the device number of a User Stream.
 */
struct UA_attach_req {
	int PRIM_type;			/* always UA_ATTACH_REQ */
	int USER_id;			/* User Stream identifier */
	uint ASP_id;			/* ASP identifier (0 for default/assignment) */
	uint SGP_id;			/* SGP identifier (0 for default/assignment) */
	uint RC_iid;			/* RC of IID */
};

/*
 *  UA_ATTACH_ACK	- Acknowledge attachment of a Stream (M_PCPROTO)
 *  -------------------------------------------------------------------------
 */
struct UA_attach_ack {
	int PRIM_type;			/* always UA_ATTACH_ACK */
	int USER_id;			/* User Stream identifier */
	uint ASP_id;			/* ASP identifier (assigned) */
	uint SGP_id;			/* SGP identifier (assigned) */
	uint RC_iid;			/* RC of IID (assigned) */
};

/*
 *  UA_DETACH_REQ - Detach a Stream from an ASP/SGP/AS (M_PROTO)
 *  -------------------------------------------------------------------------
 */
struct UA_detach_req {
	int PRIM_type;			/* always UA_DETACH_REQ */
	int USER_id;			/* User Stream identifier (0 for default) */
};

/*
 *  UA_SCTP_REQ - Request management of an SCTP association (M_PROTO/M_PCPROTO)
 *  -------------------------------------------------------------------------
 */
struct UA_sctp_req {
	int PRIM_type;			/* always UA_SCTP_REQ */
	int ASSOC_id;			/* MUX identifier (0 for default) */
	/* followed by TPI primitive */
};

/*
 *  UA_SCTP_IND - Indicate management of an SCTP association (M_PROTO/M_PCPROTO)
 *  -------------------------------------------------------------------------
 */
struct UA_sctp_ind {
	int PRIM_type;			/* always UA_SCTP_IND */
	int ASSOC_id;			/* MUX identifier (actual) */
	/* followed by TPI primitive */
};

/*
 *  UA_LINK_REQ - Link a Stream to an ASP/SGP/AS (M_PROTO)
 *  -------------------------------------------------------------------------
 *  Link an Stream to an SGP/ASP/AS.  ASSOC_id is the multiplex identifier
 *  returned by the I_LINK(7) or I_PLINK(7) input-output control, or for an AS
 *  may be the device number of a User Stream.
 */
struct UA_link_req {
	int PRIM_type;			/* always UA_LINK_REQ */
	int ASSOC_id;			/* SCTP association identifier */
	uint ASP_id;			/* ASP identifier (0 for default/assignment) */
	uint SGP_id;			/* SGP identifier (0 for default/assignment) */
};

/*
 *  UA_LINK_ACK	- Acknowledge linkage of a Stream (M_PCPROTO)
 *  -------------------------------------------------------------------------
 */
struct UA_link_ack {
	int PRIM_type;			/* always UA_LINK_ACK */
	int ASSOC_id;			/* SCTP association identifier */
	uint ASP_id;			/* ASP identifier (assigned) */
	uint SGP_id;			/* SGP identifier (assigned) */
};

/*
 *  UA_UNLINK_REQ - Unlink a Stream from an ASP/SGP/AS (M_PROTO)
 *  -------------------------------------------------------------------------
 */
struct UA_unlink_req {
	int PRIM_type;			/* always UA_UNLINK_REQ */
	int ASSOC_id;			/* SCTP association identifier (0 for default) */
};

/*
 *  UA_ASPUP_REQ - Request that ASP be brought to the ASP Up state (M_PROTO)
 *  -------------------------------------------------------------------------
 *  Requests that an ASP be brought up.  ASP_id can be zero for the ASP
 *  associated with the issuing Stream.  SGP_id can be zero to request that the
 *  ASP be brought up for all SGP asociated with the ASP.
 *
 *  PROTOCOL_version specifies the protocol version to be used by the ASP.
 *  ASPID can be set to a non-zero value to specify to the SGP the identity of
 *  the ASP.  OPT_length and OPT_offset specify UA-specific options to include
 *  when bringing the ASP up.
 *
 *  This primitive change the target state of the specified ASP-SGP relations.
 *  Because the default target state is ASP_DOWN, this primitive must be used to
 *  establish and ASP-SGP relation.
 *
 *  This primitive can only be issued by a control Stream.
 */
struct UA_aspup_req {
	int PRIM_type;			/* always UA_ASPUP_REQ */
	uint ASP_id;			/* ASP identifier (0 for default) */
	uint SGP_id;			/* SGP identifier (0 for default) */
	uint ASPID;			/* SGP ASP identity (external) */
	uint REQUEST_id;		/* confirmation correlation */
	uint PROTOCOL_version;		/* ASP protocol version */
	uint ASP_extensions;		/* ASP supported extensions */
	uint OPT_length;		/* length of ASP options */
	uint OPT_offset;		/* offset of ASP options */
};

/*
 *  UA_ASPUP_CON - Indicate ASP in ASP Up state (M_PROTO)
 *  -------------------------------------------------------------------------
 *  Confirms that an ASP-SGP relation has come Up.  ASP up confirmations are
 *  provided to the current control Stream (if any).  PROTOCOL_version,
 *  ASP_extensions, OPT_length and OPT_offset contains UA-specific options reported by
 *  the SGP.
 */
struct UA_aspup_con {
	int PRIM_type;			/* always UA_ASPUP_CON */
	uint ASP_id;			/* ASP identifier (0 for default) */
	uint SGP_id;			/* SGP identifier (0 for all) */
	uint REQUEST_id;		/* confirmation correlation */
	uint PROTOCOL_version;		/* SGP protocol version */
	uint ASP_extensions;		/* SGP supported extensions */
	uint OPT_length;		/* length of SG options */
	uint OPT_offset;		/* offset of SG options */
};

/*
 *  UA_ASPUP_IND - Indicate ASP in ASP Up state (M_PROTO)
 *  -------------------------------------------------------------------------
 *  Indicates that an ASP-SGP relation has come Up.  ASP up indications are
 *  provided to the current control Stream (if any).  PROTOCOL_version,
 *  ASP_extensions, OPT_length and OPT_offset contains UA-specific options reported by
 *  the SGP.
 */
struct UA_aspup_ind {
	int PRIM_type;			/* always UA_ASPUP_IND */
	uint ASP_id;			/* ASP identifier (0 for default) */
	uint SGP_id;			/* SGP identifier (0 for all) */
	uint PROTOCOL_version;		/* SGP protocol version */
	uint ASP_extensions;		/* SGP supported extensions */
	uint OPT_length;		/* length of SG options */
	uint OPT_offset;		/* offset of SG options */
};

/*
 *  UA_ASPDN_REQ - Request that ASP be brought to the ASP Down state (M_PROTO)
 *  -------------------------------------------------------------------------
 *  Request that an ASP be brought down.  ASP_id can be zero for the ASP
 *  associated with the issuing Stream.  SGP_id can be zero to request that the
 *  ASP be brought down for all SGP associated with the ASP.
 *
 *  This primitive changes the target state of the specified ASP-SGP relations.
 *  Because the default target state is ASP_DOWN, this primitive is only
 *  necessary once an ASP has been brought up with an UA_ASPUP_REQ.
 *
 *
 */
struct UA_aspdn_req {
	int PRIM_type;			/* always UA_ASPDN_REQ */
	uint ASP_id;			/* ASP identifier (0 for default) */
	uint SGP_id;			/* SGP identifier (0 for all) */
	uint REQUEST_id;		/* Confirmation correlation */
	uint REASON;			/* non-zero for management blocking */
};

/*
 *  UA_ASPDN_CON - Confirm ASP in ASP Down state (M_PROTO)
 *  -------------------------------------------------------------------------
 *  Confirms that an ASP is down.  ASP down confirmations are provided to the
 *  current control Stream (if any).  Indications to the local User Part is
 *  given using User Part specific local mangement primitives.
 */
struct UA_aspdn_con {
	int PRIM_type;			/* always UA_ASPDN_CON */
	uint ASP_id;			/* ASP identifier (0 for default) */
	uint SGP_id;			/* SGP identifier (0 for default) */
	uint REQUEST_id;		/* Confirmation correlation */
};

/*
 *  UA_ASPDN_IND - Indicate ASP in ASP Down state (M_PROTO)
 *  -------------------------------------------------------------------------
 *  Indicates that an ASP is down.  ASP down indications are provided to the
 *  current control Stream (if any).  Indications to the local User Part is
 *  given using User Part specific local mangement primitives.
 *
 *  ORIGIN indicates whether the ASP going down was originated by the local user
 *  or by the remote provider, or whether the origin was unknown.  REASON
 *  indicates the reason for the ASP going down (e.g. failed ASP Up attempt,
 *  association failure, unsolicited ASP Down Ack, management blocking, etc.)
 */
struct UA_aspdn_ind {
	int PRIM_type;			/* always UA_ASPDN_IND */
	uint ASP_id;			/* ASP identifier (0 for default) */
	uint SGP_id;			/* SGP identifier (0 for default) */
	uint ORIGIN;			/* origin */
	uint REASON;			/* reason */
};

/* UA origins */
#define UA_ORIGIN_UNSPEC	0	/* origin is unspecified or unknown */
#define UA_ORIGIN_USER		1	/* origin is the user */
#define UA_ORIGIN_PROVIDER	2	/* origin is the provider */

#define UA_ASPDN_REASON_UNKNOWN		0	/* Unknown, unspecified */
#define UA_ASPDN_REASON_ASPDN_ACK	1	/* Unsolicited ASP Down Ack */
#define UA_ASPDN_REASON_DISCONNECT	2	/* SCTP disconnect */
#define UA_ASPDN_REASON_RESTART		3	/* SCTP restart */

/*
 *  UA_HBEAT_REQ - Request a Hearbeat (M_PROTO)
 *  -------------------------------------------------------------------------
 */
struct UA_hbeat_req {
	int PRIM_type;			/* always UA_HBEAT_REQ */
	uint ASP_id;			/* ASP identifier (0 for default) */
	uint SGP_id;			/* SGP identifier (0 for default) */
	uint AS_id;			/* AS identifier (0 for none) */
	uint REQUEST_id;		/* Confirmation correlation */
};

/*
 *  UA_HBEAT_CON - Acknowledge a Heartbeat (M_PROTO)
 *  -------------------------------------------------------------------------
 */
struct UA_hbeat_con {
	int PRIM_type;			/* always UA_HBEAT_CON */
	uint ASP_id;			/* ASP identifier (0 for default) */
	uint SGP_id;			/* SGP identifier (0 for default) */
	uint AS_id;			/* AS identifier (0 for none) */
	uint REQUEST_id;		/* Confirmation correlation */
};

/*
 *  UA_HBEAT_IND - Acknowledge a Heartbeat (M_PROTO)
 *  -------------------------------------------------------------------------
 */
struct UA_hbeat_ind {
	int PRIM_type;			/* always UA_HBEAT_IND */
	uint ASP_id;			/* ASP identifier */
	uint SGP_id;			/* SGP identifier (0 for none) */
	uint AS_id;			/* AS identifier (0 for none) */
};

/*
 *  UA_INFO_REQ - Request ASP information (M_PCPROTO)
 *  -------------------------------------------------------------------------
 *  Request information about an ASP.  ASP_id can be set to zero (0) for the
 *  default ASP associated with the control Stream.  SGP_id can be set to zero
 *  (0) to obtain overall information about the ASP.  SGP_id can be set to a
 *  specific SGP_id to obtain information about the state of the ASP for the
 *  specified SGP.
 */
struct UA_info_req {
	int PRIM_type;			/* always UA_INFO_REQ */
	uint ASP_id;			/* ASP identifier (0 for default) */
	uint SGP_id;			/* SGP identifier (0 for overall) */
};

/*
 *  UA_INFO_ACK - Acknowledge ASP information (M_PCPROTO)
 *  -------------------------------------------------------------------------
 *  Acknowledges and provides requested information.  When SGP_id is zero (0),
 *  the CURRENT_state represents the overall state of the ASP with the SG.  When
 *  SGP_id is non-zero, CURRENT_state represents the state of the ASP for the
 *  specified SGP.  SGP_id_length and SGP_id_offset provide a list of currently
 *  assigned SGP_id's associated with this ASP.  AS_id_length and AS_id_offset
 *  provide a list of AS_id's associated with this ASP.
 */
struct UA_info_ack {
	int PRIM_type;			/* always UA_INFO_ACK */
	uint ASP_id;			/* ASP identifier */
	uint SGP_id;			/* SGP identifier (0 for overall) */
	uint PROTOCOL_version;		/* ASP protocol version */
	uint ASPID;			/* ASP identifier (UA) */
	uint CURRENT_state;		/* ASP state */
	uint SGP_id_length;		/* length of SGP identifiers */
	uint SGP_id_offset;		/* offset of SGP identifiers */
	uint AS_id_length;		/* length of AS identifiers */
	uint AS_id_offset;		/* offset of AS identifiers */
};

/*
 *  UA_ASPAC_REQ - Request AS activation. (M_PROTO)
 *  -------------------------------------------------------------------------
 *  Requests activation of an AS.  ASP_id can be zero for the ASP associated
 *  with the issuing Stream.  SGP_id can be zero to request that the AS be
 *  activated in all SGP associated with the ASP.  AS_id can be zero for the AS
 *  associated with the issuing Stream.  TRAFFIC_mode can be set to zero (0) for
 *  the SG-default traffic mode.
 *
 *  Activation of AS is normally performed by User Parts only.  This primitive
 *  changes the target state of the specified AS-SGP relations.  Because the
 *  default target state is AS_ACTIVE, this primitive will only affect the
 *  target state if a previous UA_ASPIA_REQ was issued for the AS.
 *
 *  This primitive can only be issued by a control Stream.
 */
struct UA_aspac_req {
	int PRIM_type;			/* always UA_ASPAC_REQ */
	uint AS_id;			/* AS identifier (0 for default) */
	uint REQUEST_id;		/* Confirmation correlation */
	uint TRAFFIC_mode;		/* Traffic mode for AS */
	uint LOAD_group;		/* Load group */
	uint LOAD_selector;		/* Load selector */
};

/*
 *  UA_ASPAC_CON - Confirm AS activation. (M_PROTO)
 *  -------------------------------------------------------------------------
 *  Confirms activation of an AS.  AS activation indications are provided to
 *  the current control Stream (if any).  Indications to the local User Part is
 *  given using User Part specific local management primitives.
 */
struct UA_aspac_con {
	int PRIM_type;			/* always UA_ASPAC_CON */
	uint AS_id;			/* AS identifier (actual) */
	uint REQUEST_id;		/* Confirmation correlation */
	uint TRAFFIC_mode;		/* Traffic mode for AS */
	uint LOAD_group;		/* Load group */
	uint LOAD_selector;		/* Load selector */
};

/*
 *  UA_ASPAC_IND - Indicate AS activation. (M_PROTO)
 *  -------------------------------------------------------------------------
 *  Indicates activation of an AS.  AS activation indications are provided to
 *  the current control Stream (if any).  Indications to the local User Part is
 *  given using User Part specific local management primitives.
 */
struct UA_aspac_ind {
	int PRIM_type;			/* always UA_ASPAC_IND */
	uint AS_id;			/* AS identifier (actual) */
	uint TRAFFIC_mode;		/* Traffic mode for AS */
	uint LOAD_group;		/* Load group */
	uint LOAD_selector;		/* Load selector */
};

/*
 *  UA_ASPIA_REQ - Request AS deactivation. (M_PROTO)
 *  -------------------------------------------------------------------------
 *  Requests deactivation of the AS.  ASP_id can be zero for the ASP associated
 *  with the issuing Stream.  SGP_id can be zero to request that the AS be
 *  deactivated in all SGP associated with the ASP.  AS_id can be zero for the
 *  AS(es) associated with the issuing Stream.
 *
 *  Deactivation of AS is normally performed by User Parts only.  This primitive
 *  changes the target state of the specified AS-SGP relations.
 */
struct UA_aspia_req {
	int PRIM_type;			/* always UA_ASPIA_REQ */
	uint AS_id;			/* AS identifier */
	uint REQUEST_id;		/* Confirmation correlation */
	uint REASON;			/* non-zero for management blocking */
};

/*
 *  UA_ASPIA_CON - Confirm AS deactivation. (M_PROTO)
 *  -------------------------------------------------------------------------
 *  Confirms deactivation of an AS.  AS deactivation confirmations are provided
 *  the current control Stream (if any).  Indications to the local User Part is
 *  given using User Part specific local management primitives.
 */
struct UA_aspia_con {
	int PRIM_type;			/* always UA_ASPIA_CON */
	uint AS_id;			/* AS identifier */
	uint REQUEST_id;		/* Confirmation correlation */
};

/*
 *  UA_ASPIA_IND - Indicate AS deactivation. (M_PROTO)
 *  -------------------------------------------------------------------------
 *  Indicates deactivation of an AS.  AS deactivation indications are provided
 *  the current control Stream (if any).  Indications to the local User Part is
 *  given using User Part specific local management primitives.
 *
 *  ORIGIN indicates whether the deactivation was originated by the local user
 *  or by the remote provider, or whether the origin was unknown.  REASON
 *  indicates the reason for deactivation (e.g. failed activation attempt,
 *  unsolicited deactivation, management blocking, etc.).
 */
struct UA_aspia_ind {
	int PRIM_type;			/* always UA_ASPIA_IND */
	uint AS_id;			/* AS identifier */
	uint ORIGIN;			/* origin of deactivation */
	uint REASON;			/* reason for deactivation */
};

/*
 *  UA_ASINFO_REQ - Request AS information. (M_PCPROTO)
 *  -------------------------------------------------------------------------
 *  Requests information about an AS.  ASP_id can be set to zero (0) for the
 *  default ASP associated with the control Stream.  When SGP_id is zero (0),
 *  information about the overall state of the AS is requested.  When SGP_id is
 *  non-zero, information about the state of the AS for the specified SGP is
 *  requested.  (When there is only one SGP, as for M2UA, the overall state and
 *  SGP-specific state are the same).  AS_id specifies the AS for which to
 *  obtain information.  AS_id can be set to zero (0) for the default AS
 *  associated with the issuing Stream.
 *
 *  If the CURRENT_state is AS_DOWN or AS_WACK_RREQ, then the AS_id is the
 *  registration request identifier?
 */
struct UA_asinfo_req {
	int PRIM_type;			/* always UA_ASINFO_REQ */
	uint AS_id;			/* AS identifier (0 for default) */
};

/*
 *  UA_ASINFO_ACK - Acknowledge AS information. (M_PCPROTO)
 *  -------------------------------------------------------------------------
 *  Acknowledges and provides requested information.  When SGP_id is zero (0),
 *  the CURRENT_state represents the overall state of the AS with the SG.  When
 *  SGP_id is non-zero, CURRENT_state represents the state of the AS for the
 *  specified SGP.
 */
struct UA_asinfo_ack {
	int PRIM_type;			/* always UA_ASINFO_ACK */
	uint AS_id;			/* AS identifier (actual) */
	uint TRAFFIC_mode;		/* Traffic mode */
	uint CURRENT_state;		/* Current AS state */
};

/*
 *  UA_REG_REQ - Register a Key with an SGP. (M_PROTO)
 *  -------------------------------------------------------------------------
 *  Request that a Link/Routing Key be registered with an SGP.  ASP_id can be
 *  set to zero (0) for the default ASP associated with the control Stream.
 *  SGP_id specifies the SGP with which to register and can be zero (0) if there
 *  is only one SGP associated with the ASP.  REQUEST_id is local identifier
 *  used by the control Stream to correlate the response.  KEY_length and
 *  KEY_offset specify the Link/Routing Key to register.  Each UA has its own
 *  key format.  Multiple regsitration requests can be pending.  Only a control
 *  Stream can issue a registration request.
 */
struct UA_reg_req {
	int PRIM_type;			/* always UA_ASINFO_REQ */
	uint ASP_id;			/* ASP identifier (0 for default) */
	uint SGP_id;			/* SGP identifier (0 for default) */
	uint REQUEST_id;		/* Request identifier */
	uint RC_iid;			/* RC or IID (non-zero for change) */
	uint KEY_length;		/* Link/Routing Key length */
	uint KEY_offset;		/* Link/Routing Key offset */
};

/*
 *  UA_REG_RSP - Registration status. (M_PCPROTO)
 *  -------------------------------------------------------------------------
 *  Provides registration status.  STATUS provides the registration status (same
 *  number as defined by the UA).
 */
struct UA_reg_rsp {
	int PRIM_type;			/* always UA_ASINFO_REQ */
	uint ASP_id;			/* ASP identifier */
	uint SGP_id;			/* SGP identifier */
	uint REQUEST_id;		/* Request identifier */
	uint RC_iid;			/* RC of IID (0 on failure) */
	uint STATUS;			/* Regsistration Status */
};

/* 00 - Success */
/* 01 - Unknown */
/* 02 - Invalid Destination Address */
/* 03 - Invalid Newtork Appearance */
/* 04 - Invalid Routing Key */
/* 05 - Permission Denied */
/* 06 - Cannot Support Unique Routing */
/* 07 - Routing Key not Currently Provisioned */
/* 10 - Insufficient Resources */
/* 11 - Unsupported RK parameter field  */
/* 12 - Routing Key Change Refused */

/*
 *  UA_DEREG_REQ - Deregister an AS for an SGP. (M_PROTO)
 *  -------------------------------------------------------------------------
 *  Request that an RC or IID be deregistered with an SGP.  ASP_id can be set to
 *  zero (0) for the default ASP associated with the control Stream.  SGP_id
 *  specifies the SGP with which to deregister and can be zero (0) if there is
 *  only one SGP associated with the ASP.  RC_iid is the RC or IID to deregister
 *  or can be zero (0) for the default AS associated with the Stream.  Only a
 *  control Stream can issue a deregistration request.
 */
struct UA_dereg_req {
	int PRIM_type;			/* always UA_ASINFO_REQ */
	uint ASP_id;			/* ASP identifier (0 for default) */
	uint SGP_id;			/* SGP identifier (0 for default) */
	uint RC_iid;			/* RC of IID (0 for default) */
};

/*
 *  UA_DEREG_RSP - Deregistration status. (M_PROTO)
 *  -------------------------------------------------------------------------
 *  Provides deregistration status.  STATUS provides the deregistration status
 *  (same number as defined by the UA).
 */
struct UA_dereg_rsp {
	int PRIM_type;			/* always UA_ASINFO_REQ */
	uint ASP_id;			/* ASP identifier */
	uint SGP_id;			/* SGP identifier */
	uint RC_iid;			/* RC of IID */
	uint STATUS;			/* Deregsistration Status */
};

/* 00 - Success */
/* 01 - Unknown */
/* 02 - Invalid Routing Context */
/* 03 - Permission Denied */
/* 04 - Not Registered */
/* 05 - Registration Change Refused */

/*
 *  UA_ERROR_CON - Confirm an Error (M_PROTO)
 *  -------------------------------------------------------------------------
 *  Confirms that an unrecoverable error occurred for a request primitive and
 *  that the corresponding confirmation primitive is not forthcoming.
 */
struct UA_error_con {
	int PRIM_type;
	uint ASP_id;			/* AS identifier (0 if default or unknown) */
	uint SGP_id;			/* AS identifier (0 if default or unknown) */
	uint AS_id;			/* AS identifier (0 for none) */
	uint REQUEST_id;
	uint ERROR_prim;
	uint ORIGIN;
	uint REASON;
};

/*
 *  UA_ERROR_IND - Indicate an Error (M_PROTO)
 *  -------------------------------------------------------------------------
 *  Indicates that an error has occured.  No state change in the AS or ASP is
 *  indicated.
 */
struct UA_error_ind {
	int PRIM_type;
	uint ASP_id;			/* AS identifier (0 if default or unknown) */
	uint SGP_id;			/* AS identifier (0 if default or unknown) */
	uint AS_id;			/* AS identifier (0 for none) */
	uint ORIGIN;
	uint REASON;
};

/*
 *  UA_NOTIFY_REQ - Request a Notification (M_PROTO)
 *  -------------------------------------------------------------------------
 *  Requests a notification.
 */
struct UA_notify_req {
	int PRIM_type;			/* always UA_NOTIFY_IND */
	uint ASP_id;			/* AS identifier (0 for all) */
	uint SGP_id;			/* AS identifier (0 for default) */
	uint AS_id;			/* AS identifier (0 for none) */
	uint NOTIFY_type;		/* Notification type (same as UA) */
	uint NOTIFY_status;		/* Notification status (same as UA) */
	uint ASPID;			/* External ASP Identifier (0 for none) */
};

/*
 *  UA_NOTIFY_IND - Indicate a Notification (M_PROTO)
 *  -------------------------------------------------------------------------
 *  Indicates a notification.
 */
struct UA_notify_ind {
	int PRIM_type;			/* always UA_NOTIFY_IND */
	uint ASP_id;			/* AS identifier (0 if default or unknown) */
	uint SGP_id;			/* AS identifier (0 if default or unknown) */
	uint AS_id;			/* AS identifier (0 if none) */
	uint NOTIFY_type;		/* Notification type (same as UA) */
	uint NOTIFY_status;		/* Notification status (same as UA) */
	uint ASPID;			/* External ASP Identifier */
};

#ident "@(#) $RCSfile: ua_as_lm.h,v $ $Name:  $($Revision: 0.9.2.4 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

#endif				/* __SYS_UA_AS_LM_H__ */
