/*****************************************************************************

 @(#) $Id: mgi.h,v 0.9.2.2 2005/05/14 08:30:45 brian Exp $

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

 Last Modified $Date: 2005/05/14 08:30:45 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SS7_MG_H__
#define __SS7_MG_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2004  OpenSS7 Corporation"

/* This file can be processed by doxygen(1). */

#define MG_INFO_REQ		 1UL
#define MG_OPTMGMT_REQ		 2UL
#define MG_ATTACH_REQ		 3UL
#define MG_DETACH_REQ		 4UL
#define MG_JOIN_REQ		 5UL
#define MG_ACTION_REQ		 6UL
#define MG_ABORT_REQ		 7UL
#define MG_CONN_REQ		 8UL
#define MG_DATA_REQ		 9UL
#define MG_DISCON_REQ		10UL
#define MG_LEAVE_REQ		11UL
#define MG_NOTIFY_REQ		12UL

#define MG_INFO_ACK		13UL
#define MG_OPTMGMT_ACK		14UL
#define MG_OK_ACK		15UL
#define MG_ERROR_ACK		16UL
#define MG_ATTACH_ACK		17UL
#define MG_JOIN_CON		18UL
#define MG_ACTION_CON		19UL
#define MG_ACTION_IND		20UL
#define MG_CONN_CON		21UL
#define MG_DATA_IND		22UL
#define MG_DISCON_IND		23UL
#define MG_DISCON_CON		24UL
#define MG_LEAVE_IND		25UL
#define MG_LEAVE_CON		26UL
#define MG_NOTIFY_IND		27UL

/*
 *  MG STATES
 */
#define MGS_UNINIT		-2UL
#define MGS_UNUSABLE		-1UL
#define MGS_DETACHED		 0UL
#define MGS_WACK_AREQ		 1UL
#define MGS_WCON_AREQ		 2UL
#define MGS_WACK_UREQ		 3UL
#define MGS_WCON_UREQ		 4UL
#define MGS_ATTACHED		 5UL
#define MGS_WACK_JREQ		 6UL
#define MGS_WCON_JREQ		 7UL
#define MGS_WACK_LREQ		 8UL
#define MGS_WCON_LREQ		 9UL
#define MGS_JOINED		10UL
#define MGS_WACK_CREQ		11UL
#define MGS_WCON_CREQ		12UL
#define MGS_WACK_DREQ		13UL
#define MGS_WCON_DREQ		14UL
#define MGS_CONNECTED		15UL

#define MGSF_UNINIT		0
#define MGSF_UNUSABLE		0
#define MGSF_DETACHED		(1<<MGS_DETACHED)
#define MGSF_WACK_AREQ		(1<<MGS_WACK_AREQ)
#define MGSF_WCON_AREQ		(1<<MGS_WCON_AREQ)
#define MGSF_WACK_UREQ		(1<<MGS_WACK_UREQ)
#define MGSF_WCON_UREQ		(1<<MGS_WCON_UREQ)
#define MGSF_ATTACHED		(1<<MGS_ATTACHED)
#define MGSF_WACK_JREQ		(1<<MGS_WACK_JREQ)
#define MGSF_WCON_JREQ		(1<<MGS_WCON_JREQ)
#define MGSF_WACK_LREQ		(1<<MGS_WACK_LREQ)
#define MGSF_WCON_LREQ		(1<<MGS_WCON_LREQ)
#define MGSF_JOINED		(1<<MGS_JOINED)
#define MGSF_WACK_CREQ		(1<<MGS_WACK_CREQ)
#define MGSF_WCON_CREQ		(1<<MGS_WCON_CREQ)
#define MGSF_WACK_DREQ		(1<<MGS_WACK_DREQ)
#define MGSF_WCON_DREQ		(1<<MGS_WCON_DREQ)
#define MGSF_CONNECTED		(1<<MGS_CONNECTED)

/*
 *  MG PROTOCOL PRIMITIVES
 */

/*
 *  MG_OPTMGMT_REQ
 *  -------------------------------------------------------------------------
 */
typedef struct MG_optmgmt_req {
	ulong mg_primitive;		/* always MG_INFO_REQ */
	ulong mg_opt_length;		/* length of options */
	ulong mg_opt_offset;		/* offset of options */
	ulong mg_mgmt_flags;		/* management flags */
} MG_optmgmt_req_t;

/*
 *  MG_OPTMGMT_ACK
 *  -------------------------------------------------------------------------
 */
typedef struct MG_optmgmt_ack {
	ulong mg_primitive;		/* always MG_INFO_ACK */
	ulong mg_opt_length;		/* length of options */
	ulong mg_opt_offset;		/* offset of options */
	ulong mg_mgmt_flags;		/* management flags */
} MG_optmgmt_ack_t;

#define MG_SUCCESS	0x00	/* indicates successful operation */
#define MG_FAILURE	0x01	/* indicates unsuccessful operation */
#define MG_SET_OPT	0x02	/* set options */
#define MG_GET_OPT	0x04	/* get options */
#define MG_NEGOTIATE	0x08	/* negotiate options */
#define MG_DEFAULT	0x10	/* default options */

typedef struct MG_channel_opt {
	ulong mg_obj_type;		/* always MG_OBJ_TYPE_CH */
	ulong mg_obj_id;		/* channel id */
	ulong ch_type;			/* channel media type */
	ulong ch_flags;			/* channel media options flags */
	ulong ch_block_size;		/* data block size (bits) */
	ulong ch_encoding;		/* encoding */
	ulong ch_sample_size;		/* sample size (bits) */
	ulong ch_rate;			/* clock rate (Hz) */
	ulong ch_tx_channels;		/* number of tx channels */
	ulong ch_rx_channels;		/* number of rx channels */
	ulong ch_opt_flags;		/* options flags */
} MG_channel_opt_t;

typedef struct MG_connleg_opt {
	ulong mg_obj_type;		/* always MG_OBJ_TYPE_LG */
	ulong mg_obj_id;		/* channel id */
	ulong lg_type;			/* conn leg media type */
	ulong lg_flags;			/* conn leg media options flags */
	ulong lg_block_size;		/* data block size (bits) */
	ulong lg_encoding;		/* encoding */
	ulong lg_sample_size;		/* sample size (bits) */
	ulong lg_rate;			/* clock rate (Hz) */
	ulong lg_tx_channels;		/* number of tx channels */
	ulong lg_rx_channels;		/* number of rx channels */
	ulong lg_opt_flags;		/* options flags */
} MG_connleg_opt_t;

typedef struct MG_session_opt {
	ulong mg_obj_type;		/* always MG_OBJ_TYPE_SE */
	ulong mg_obj_id;		/* session id */
	ulong se_type;			/* session media type */
	ulong se_flags;			/* session media options flags */
	ulong se_block_size;		/* data block size (bits) */
	ulong se_encoding;		/* encoding */
	ulong se_sample_size;		/* sample size (bits) */
	ulong se_rate;			/* clock rate (Hz) */
	ulong se_tx_channels;		/* number of tx channels */
	ulong se_rx_channels;		/* number of rx channels */
	ulong se_opt_flags;		/* options flags */
} MG_session_opt_t;

#define SEF_INTERWORKING	0x01	/* encoding interworking */
#define SEF_CONFERENCING	0x02	/* conferencing in effect */
#define SEF_CLEARCHANNEL	0x04	/* clear channel enforced */

typedef union MG_options {
	struct {
		ulong mg_obj_type;	/* object type */
		ulong mg_obj_id;	/* object id */
	} obj;
	struct MG_channel_opt ch;	/* channel options */
	struct MG_connleg_opt lg;	/* conn leg options */
	struct MG_session_opt se;	/* session options */
} MG_options_t;

#define MG_SE_OPT_AUTO_GAIN_CONTROL	0x01	/* perform Automatic Gain Control */
#define MG_SE_OPT_LIMITING		0x02	/* perform limiting */
#define MG_SE_OPT_COMPRESSION		0x04	/* perform compression */

/*
 *  MG_INFO_REQ
 *  -------------------------------------------------------------------------
 *  Requests information about the MG stream including its current channel
 *  configuration.
 */
typedef struct MG_info_req {
	ulong mg_primitive;		/* always MG_INFO_REQ */
} MG_info_req_t;

/*
 *  MG_INFO_ACK
 *  -------------------------------------------------------------------------
 *  Provides information about the MG stream and provide including the current
 *  channel configuration.
 */
typedef struct MG_info_ack {
	ulong mg_primitive;		/* always MG_INFO_ACK */
	ulong mg_se_id;			/* session id */
	ulong mg_opt_length;		/* channel options length */
	ulong mg_opt_offset;		/* channel options offset */
	ulong mg_prov_flags;		/* provider options flags */
	ulong mg_style;			/* provider style */
	ulong mg_version;		/* provider version */
} MG_info_ack_t;

#define MG_STYLE1	0x0	/* does not perform attach */
#define MG_STYLE2	0x1	/* does perform attach */

#define MG_VERSION_1_0	0x10	/* version 1.0 of interface */
#define MG_VERSION	MG_VERSION_1_0

/*
 *  MG_ATTACH_REQ
 *  -------------------------------------------------------------------------
 *  Requests that the specified slot on the requesting stream (mg_mx_id == 0)
 *  or specified multiplex (mg_mx_id != 0) be associated with a newly created
 *  channel with specified (mg_ch_id non-zero) or provider assigned (mg_ch_id
 *  zero) channel id.
 *
 *  This primitive is acknowledged with the MG_ATTACH_ACK.
 *
 *  If the requesting stream is closed, all channels attached to the
 *  requesting stream will be detached.  Only channels that are associated
 *  with the requesting stream need be assigned in this fashion.
 */
typedef struct MG_attach_req {
	ulong mg_primitive;		/* always MG_ATTACH_REQ */
	ulong mg_mx_id;			/* multiplex id (or 0 for requesting stream) */
	ulong mg_mx_slot;		/* multiplex slot number */
	ulong mg_ch_id;			/* channel id (or 0 for provider assignment) */
	ulong mg_ch_type;		/* type of channel */
} MG_attach_req_t;

/*
 *  MG_ATTACH_ACK
 *  -------------------------------------------------------------------------
 *  Acknowledges an MG_ATTACH_REQ.  Returns the channel id.
 */
typedef struct MG_attach_ack {
	ulong mg_primitive;		/* always MG_ATTACH_ACK */
	ulong mg_mx_id;			/* multiplex id (or 0 for requesting stream) */
	ulong mg_mx_slot;		/* multiplex slot number */
	ulong mg_ch_id;			/* channel id assignment */
} MG_attach_ack_t;

/*
 *  MG_DETACH_REQ
 *  -------------------------------------------------------------------------
 *  Requests that the specified requesting stream channel be detached.  This
 *  primitive is acknowledged with the MG_OK_ACK.
 */
typedef struct MG_detach_req {
	ulong mg_primitive;		/* always MG_DETACH_REQ */
	ulong mg_ch_id;			/* channel id */
} MG_detach_req_t;

/*
 *  MG_JOIN_REQ
 *  -------------------------------------------------------------------------
 *  Requests that the specified channels be joined to the specified session
 *  with newly created termination point with id specified by the caller
 *  (mg_tp_id non-zero) or assigned by the provider (mg_tp_id zero).  If the
 *  specified session id is zero then a new session will be created.  If the
 *  specified session id exists, the existing session will be joined.  If the
 *  specified session id is non-zero but does not exist, one will be created
 *  wtih the specified id.  All channels must be successfully enabled before
 *  this primitive will be confirmed.  This primtive is confirmed with the
 *  MG_JOIN_CON primitive.  It is refused with the MG_ERROR_ACK or
 *  MG_LEAVE_IND primitive.
 */
typedef struct MG_join_req {
	ulong mg_primitive;		/* always MG_JOIN_REQ */
	ulong mg_se_id;			/* session to join (0 to create) */
	ulong mg_tp_id;			/* joined termination (0 for new) */
	ulong mg_channel_length;	/* channel ids that make up termination */
	ulong mg_channel_offset;	/* channel ids that make up termination */
} MG_join_req_t;

/*
 *  MG_JOIN_CON
 *  -------------------------------------------------------------------------
 *  Confirms that the previous join request was successful and the session id
 *  and termination point id of the join.
 */
typedef struct MG_join_con {
	ulong mg_primitive;		/* always MG_JOIN_CON */
	ulong mg_se_id;			/* joined session */
	ulong mg_tp_id;			/* joined termination */
} MG_join_con_t;

/*
 *  MG_ACTION_REQ, M_PROTO w/ 0 or more M_DATA
 *  -------------------------------------------------------------------------
 *  Requests that the action of the requested type be performed against the
 *  specified session and termination point for the requested duration and
 *  with the requested options.
 *
 *  When more data is indicated using the MG_MORE_DATA flag, it indicates that
 *  subsequent MG_ACTION_REQ primitives contain more data for the associated
 *  pattern.  The data is encoded according to the media format of the
 *  requesting stream.
 *
 *  When the requested duration is zero, the action will continue until its
 *  normal termination, or until subsequently aborted.
 *
 *  Actions on terminations which are currently connected in a communications
 *  session will be mixed with the media received from the communications
 *  session and any other actions which are currently being performed on the
 *  termination.
 *
 *  Actions on terminations which are currently disconnected from a
 *  communications session will be be mixed with the media from other actions
 *  on the termination point.
 *
 *  Some actions can only be performed on disconnected termination points
 *  (e.g., MG_ACTION_LOOPBACK, MG_ACTION_TEST_SILENT).
 *
 *  Some actions replace all other actions on the termination point (e.g.,
 *  MG_ACTION_LOOPBACK, MG_ACTION_TEST_SILENT).
 *
 *  Some actions performed on a termination point will be performed on
 *  individual channels that make up the termination point (e.g.
 *  MG_ACTION_LOOPBACK).
 */
typedef struct MG_action_req {
	ulong mg_primitive;		/* always MG_ACTION_REQ */
	ulong mg_action;		/* requested action */
	ulong mg_se_id;			/* session id */
	ulong mg_tp_id;			/* termination id to perform action */
	ulong mg_duration;		/* duration in milliseconds */
	ulong mg_flags;			/* option flags */
} MG_action_req_t;

#define MG_ACTION_FIRST			 1
#define MG_ACTION_SEND_PATTERN		 1	/* send the provided pattern */
#define MG_ACTION_REPEAT_PATTERN	 2	/* repeat the provided pattern */
#define MG_ACTION_LOOPBACK		 3	/* apply loopback */
#define MG_ACTION_TEST_CONT		 4	/* apply continuity test tone */
#define MG_ACTION_TEST_MILLIWATT	 5	/* apply milliwatt */
#define MG_ACTION_TEST_SILENT		 6	/* apply silent termination */
#define MG_ACTION_TEST_BALANCED		 7	/* apply ballanced termination */
#define MG_ACTION_US_RINGBACK		 8	/* apply US ringback */
#define MG_ACTION_US_BUSY		 9	/* apply US busy */
#define MG_ACTION_US_REORDER		10	/* apply US reorder */
#define MG_ACTION_US_PERM_SIGNAL	11	/* apply US receiver of hook */
#define MG_ACTION_US_BONG		12	/* apply US bong tone */
#define MG_ACTION_EU_RINGBACK		13	/* apply EU ringback */
#define MG_ACTION_EU_BUSY		14	/* apply EU busy */
#define MG_ACTION_EU_REORDER		15	/* apply EU reorder */
#define MG_ACTION_EU_PERM_SIGNAL	16	/* apply EU receiver of hook */
#define MG_ACTION_EU_BONG		17	/* apply EU bong tone */
#define MG_ACTION_MF_0			20	/* apply MF 0 */
#define MG_ACTION_MF_1			21	/* apply MF 1 */
#define MG_ACTION_MF_2			22	/* apply MF 2 */
#define MG_ACTION_MF_3			23	/* apply MF 3 */
#define MG_ACTION_MF_4			24	/* apply MF 4 */
#define MG_ACTION_MF_5			25	/* apply MF 5 */
#define MG_ACTION_MF_6			26	/* apply MF 6 */
#define MG_ACTION_MF_7			27	/* apply MF 7 */
#define MG_ACTION_MF_8			28	/* apply MF 8 */
#define MG_ACTION_MF_9			29	/* apply MF 9 */
#define MG_ACTION_MF_A			30	/* apply MF A */
#define MG_ACTION_MF_B			31	/* apply MF B */
#define MG_ACTION_MF_C			32	/* apply MF C */
#define MG_ACTION_MF_D			33	/* apply MF D */
#define MG_ACTION_MF_E			34	/* apply MF E */
#define MG_ACTION_MF_F			35	/* apply MF F */
#define MG_ACTION_WAIT			36	/* wait for specifie duration */
#define MG_ACTION_LAST			36

#define MG_MORE_DATA			0x01

/*
 *  MG_ACTION_CON, M_PROTO w/ 0 or more M_DATA
 *  -------------------------------------------------------------------------
 *  Confirms that the requested action has begun.  MG_ACTION_REQ which have
 *  the MG_MORE_DATA flag set will not be confirmed until the last
 *  MG_ACTION_REQ has been issued by the MG user. The end of restricted
 *  duration actions will be indicated with MG_ACTION_IND.
 */
typedef struct MG_action_con {
	ulong mg_primitive;		/* always MG_ACTION_CON */
	ulong mg_action;		/* confirmed action */
	ulong mg_se_id;			/* session id */
	ulong mg_tp_id;			/* termination id for action confirmed */
	ulong mg_action_id;		/* action identifier */
} MG_action_con_t;

/*
 *  MG_ACTION_IND, M_PROTO
 *  -------------------------------------------------------------------------
 *  Indicates that the action identified by the indicated action identifier
 *  has completed.
 */
typedef struct MG_action_ind {
	ulong mg_primitive;		/* always MG_ACTION_CON */
	ulong mg_action;		/* completed action */
	ulong mg_se_id;			/* session id */
	ulong mg_tp_id;			/* termination id for action completed */
	ulong mg_action_id;		/* action identifier */
} MG_event_ind_t;

/*
 *  MG_ABORT_REQ, M_PROTO
 *  -------------------------------------------------------------------------
 *  Requests that the specified action be aborted.  This primitive is
 *  confirmed with the MG_OK_ACK primiitve.  If the action identifier is
 *  MG_ACTION_PREVIOUS, this primitive requests that the previously initiated
 *  (unconfirmed) action be aborted.  If the action identifier is zero, this
 *  primitive requests that all actions on the specified termination point be
 *  aborted.
 */
typedef struct MG_abort_req {
	ulong mg_primitive;		/* always MG_ABORT_REQ */
	ulong mg_se_id;			/* session id */
	ulong mg_tp_id;			/* termination id for action to abort */
	ulong mg_action_id;		/* identifier of action to abort */
} MG_abort_req_t;

#define MG_ACTION_PREVIOUS	(-1UL)

/*
 *  MG_CONN_REQ
 *  -------------------------------------------------------------------------
 *  Request that the requested termination point be connected into the
 *  communications session in the directions indicated by mg_conn_flags, with
 *  the digital padding specified and the optional topology description.
 *
 *  If the optional topology description is not included, it is assumed that
 *  the termination point is requested to be connected to all other
 *  participants in the communications session in the directions requested.
 *
 *  If the optional topology description is included, it contains the list of
 *  other termination points in the session to which the the specified
 *  termination point is to be connected in the directions requested.
 */
typedef struct MG_conn_req {
	ulong mg_primitive;		/* always MG_CONN_REQ */
	ulong mg_se_id;			/* session id */
	ulong mg_tp_id;			/* termination point */
	ulong mg_conn_flags;		/* connection flags */
	ulong mg_padding;		/* digital padding */
	ulong mg_topology_length;	/* length of topology to connect */
	ulong mg_topology_offset;	/* offset of topology to connect */
} MG_conn_req_t;

/*
   connect flags 
 */
#define MGF_IC_DIR	0x01
#define MGF_OG_DIR	0x02
#define MGF_BOTH_DIR	(MGF_IC_DIR|MGF_OG_DIR)

/*
 *  MG_CONN_CON
 *  -------------------------------------------------------------------------
 *  Confirms that the requested connection primitive has been successfully
 *  completed.
 */
typedef struct MG_conn_con {
	ulong mg_primitive;		/* always MG_CONN_CON */
	ulong mg_se_id;			/* session id */
	ulong mg_tp_id;			/* connecting channel id */
} MG_conn_con_t;

/*
 *  MG_DATA_REQ, M_PROTO w/ M_DATA, prefferably just M_DATA.
 *  -------------------------------------------------------------------------
 *  Sends channel data to the session from the requesting MG stream.
 */
typedef struct MG_data_req {
	ulong mg_primitive;		/* always MG_DATA_REQ */
	ulong mg_flags;			/* data flags */
	ulong mg_mx_slot;		/* multiplex slot number */
} MG_data_req_t;

/*
 *  MG_DATA_IND, M_PROTO w/ M_DATA, prefferably just M_DATA.
 *  -------------------------------------------------------------------------
 *  Receives channel data from the session on the MG stream.
 */
typedef struct MG_data_ind {
	ulong mg_primitive;		/* always MG_DATA_IND */
	ulong mg_flags;			/* data flags */
	ulong mg_mx_slot;		/* multiplex slot number */
} MG_data_ind_t;

/*
 *  MG_DISCON_REQ
 *  -------------------------------------------------------------------------
 *  Requests that the termination point be disconnected from the cmmunications
 *  session in the directions indicated by mg_conn_flags, and the optional
 *  topology description.
 *
 *  If the optional topology description is not specified, it is assumed that
 *  the termination point is to be disconnected from all other participants in
 *  the communications session for the directions requested.
 *
 *  If the optional topology description is specified, it contains the list of
 *  other termination points in the session from which the specified
 *  termination point is to be disconnected in the directions requested.
 */
typedef struct MG_discon_req {
	ulong mg_primitive;		/* always MG_DISCON_REQ */
	ulong mg_se_id;			/* session id */
	ulong mg_tp_id;			/* termination point */
	ulong mg_conn_flags;		/* connection flags */
	ulong mg_topology_length;	/* length of topology to disconnect */
	ulong mg_topology_offset;	/* offset of topology to disconnect */
} MG_discon_req_t;

/*
 *  MG_DISCON_IND
 *  -------------------------------------------------------------------------
 *  Indicates that the termination point has been autonomously disconnected
 *  from the indicated communications session in the directions indicated by
 *  mg_conn_flags and with the optional topology description.
 *
 *  If the optional topology description is not indicated, it is assumed that
 *  the termination point has been autonomously disconnected from all other
 *  participants in the communications session for the directions indicated.
 */
typedef struct MG_discon_ind {
	ulong mg_primitive;		/* always MG_DISCON_IND */
	ulong mg_se_id;			/* session id */
	ulong mg_tp_id;			/* disconnecting termination id */
	ulong mg_conn_flags;		/* directions disconnected */
	ulong mg_cause;			/* cause of disconnect */
} MG_discon_ind_t;

/*
 *  MG_DISCON_CON
 *  -------------------------------------------------------------------------
 *  Confirms that the requested disconnection primitive has been successfully
 *  completed.
 */
typedef struct MG_discon_con {
	ulong mg_primitive;		/* always MG_DISCON_CON */
	ulong mg_se_id;			/* session id */
	ulong mg_tp_id;			/* disconnecting termination id */
} MG_discon_con_t;

/*
 *  MG_LEAVE_REQ
 *  -------------------------------------------------------------------------
 *  Requests that the specified termination point (mg_tp_id non-zero) or all
 *  termination points (mg_tp_id zero) leave the specified communication
 *  session.  Once all termination points leave a communications session, the
 *  communication session is destroyed.
 */
typedef struct MG_leave_req {
	ulong mg_primitive;		/* always MG_LEAVE_REQ */
	ulong mg_se_id;			/* session id */
	ulong mg_tp_id;			/* leaving termination id */
} MG_leave_req_t;

/*
 *  MG_LEAVE_IND
 *  -------------------------------------------------------------------------
 *  Indicates that the termination point has autonomously left the
 *  communications session.
 */
typedef struct MG_leave_ind {
	ulong mg_primitive;		/* always MG_LEAVE_IND */
	ulong mg_se_id;			/* session id */
	ulong mg_tp_id;			/* leaving termination id */
	ulong mg_cause;			/* reason for leaving */
} MG_leave_ind_t;

/*
 *  MG_LEAVE_CON
 *  -------------------------------------------------------------------------
 *  Confirms that the termination point has left the session.  The termination
 *  point identifier is released.
 */
typedef struct MG_leave_con {
	ulong mg_primitive;		/* always MG_LEAVE_CON */
	ulong mg_se_id;			/* session id */
	ulong mg_tp_id;			/* left termination id */
} MG_leave_con_t;

/*
 *  MG_OK_ACK
 *  -------------------------------------------------------------------------
 */
typedef struct MG_ok_ack {
	ulong mg_primitive;		/* always MG_OK_ACK */
	ulong mg_correct_prim;		/* correct primitive */
} MG_ok_ack_t;

/*
 *  MG_ERROR_ACK
 *  -------------------------------------------------------------------------
 */
typedef struct MG_error_ack {
	ulong mg_primitive;		/* always MG_INFO_REQ */
	ulong mg_error_primitive;	/* primitive in error */
	ulong mg_error_type;		/* MG interface error type */
	ulong mg_unix_error;		/* UNIX error */
} MG_error_ack_t;

#define MGSYSERR		 0	/* UNIX system error */
#define MGOUTSTATE		 1	/* Interface out of state */
#define MGBADPRIM		 2	/* Bad primitive */
#define MGNOTSUPP		 3	/* Primitive not supported */
#define MGBADID			 4	/* Bad identifier */
#define MGBADOPTTYPE		 5	/* Bad options structure type */
#define MGBADOPT		 6	/* Bad option format or content */
#define MGBADFLAG		 7	/* Bad flag */
#define MGIDBUSY		 8	/* Object busy */
#define MGBADACT		 9	/* Bad action */

/*
 *  MG_NOTIFY_REQ
 *  -------------------------------------------------------------------------
 */
typedef struct MG_notify_req {
	ulong mg_primitive;		/* always MG_NOTIFY_REQ */
	ulong mg_events;		/* events to notify */
} MG_notify_req_t;

/*
 *  MG_NOTIFY_IND
 *  -------------------------------------------------------------------------
 */
typedef struct MG_notify_ind {
	ulong mg_primitive;		/* always MG_NOTIFY_IND */
	ulong mg_event;			/* event */
	ulong mg_se_id;			/* session id */
	ulong mg_tp_id;			/* termination id */
	ulong mg_ch_id;			/* channel id */
	ulong mg_diag_length;		/* diagnostic length */
	ulong mg_diag_offset;		/* diagnostic offset */
} MG_notify_ind_t;

#endif				/* __SS7_MG_H__ */
