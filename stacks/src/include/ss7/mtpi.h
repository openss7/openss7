/*****************************************************************************

 @(#) $Id: mtpi.h,v 0.9 2004/01/17 08:08:12 brian Exp $

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

 Last Modified $Date: 2004/01/17 08:08:12 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SS7_MTPI_H__
#define __SS7_MTPI_H__

#ident "@(#) $Name:  $($Revision: 0.9 $) Copyright (c) 1997-2002 OpenSS7 Corporation."

#define MTP_VERSION_1	    0x10
#define MTP_CURRENT_VERSION MTP_VERSION_1

typedef long mtp_long;
typedef unsigned long mtp_ulong;
typedef unsigned short mtp_ushort;
typedef unsigned char mtp_uchar;

#define MTP_BIND_REQ			1	/* Bind to an MTP-SAP */
#define MTP_UNBIND_REQ			2	/* Unbind from an MTP-SAP */
#define MTP_CONN_REQ			3	/* Connect to a remote MTP-SAP */
#define MTP_DISCON_REQ			4	/* Disconnect from a remote MTP-SAP */
#define MTP_ADDR_REQ			5	/* Address service */
#define MTP_INFO_REQ			6	/* Information service */
#define MTP_OPTMGMT_REQ			7	/* Options management service */
#define MTP_TRANSFER_REQ		8	/* MTP data transfer request */

#define MTP_OK_ACK			9	/* Positive acknowledgement */
#define MTP_ERROR_ACK			10	/* Negative acknowledgement */
#define MTP_BIND_ACK			11	/* Bind acknowledgement */
#define MTP_ADDR_ACK			12	/* Address acknowledgement */
#define MTP_INFO_ACK			13	/* Information acknowledgement */
#define MTP_OPTMGMT_ACK			14	/* Options management acknowledgement */
#define MTP_TRANSFER_IND		15	/* MTP data transfer indication */
#define MTP_PAUSE_IND			16	/* MTP pause (stop) indication */
#define MTP_RESUME_IND			17	/* MTP resume (start) indication */
#define MTP_STATUS_IND			18	/* MTP status indication */
#define MTP_RESTART_BEGINS_IND		19	/* MTP restart begins (impl. dep.) */
#define MTP_RESTART_COMPLETE_IND	20	/* MTP restart complete (impl. dep.) */

/*
 *  Interface States
 */
#define MTPS_UNBND			 0UL
#define MTPS_WACK_BREQ			 1UL
#define MTPS_IDLE			 2UL
#define MTPS_WACK_CREQ			 3UL
#define MTPS_WCON_CREQ			 4UL
#define MTPS_CONNECTED			 5UL
#define MTPS_WACK_UREQ			 6UL
#define MTPS_WACK_DREQ6			 7UL
#define MTPS_WACK_DREQ9			 8UL
#define MTPS_WACK_OPTREQ		 9UL
#define MTPS_WREQ_ORDREL		10UL
#define MTPS_WIND_ORDREL		11UL
#define MTPS_WRES_CIND			12UL
#define MTPS_UNUSABLE			0xffffffffUL

#ifndef __HAVE_MTP_ADDR
#ifndef AF_MTP
#define AF_MTP 0
#endif
typedef struct mtp_addr {
	unsigned int family __attribute__ ((packed));
	unsigned short int ni __attribute__ ((packed));	/* network indentifier */
	unsigned short int si __attribute__ ((packed));	/* service indicator */
	unsigned int pc __attribute__ ((packed));	/* point code */
} mtp_addr_t;
#define __HAVE_MTP_ADDR
#endif

/*
 *  MTP_INFO_REQ, M_PROTO
 */
typedef struct MTP_info_req {
	mtp_ulong mtp_primitive;	/* always MTP_INFO_REQ */
} MTP_info_req_t;

/*
 *  MTP_INFO_ACK, M_PCPROTO
 */
typedef struct MTP_info_ack {
	mtp_ulong mtp_primitive;	/* always MTP_INFO_ACK */
	mtp_ulong mtp_msu_size;		/* maximum MSU size for guaranteed delivery */
	mtp_ulong mtp_addr_size;	/* maximum address size */
	mtp_ulong mtp_addr_length;	/* address length */
	mtp_ulong mtp_addr_offset;	/* address offset */
	mtp_ulong mtp_current_state;	/* current interface state */
	mtp_ulong mtp_serv_type;	/* service type */
	mtp_ulong mtp_version;		/* version of interface */
} MTP_info_ack_t;

#define M_COMS	1		/* Connection-mode MTP service supported */
#define M_CLMS	2		/* Connection-less MTP service supported */

/*
 *  MTP_ADDR_REQ, M_PCPROTO
 */
typedef struct MTP_addr_req {
	mtp_ulong mtp_primitive;	/* always MTP_ADDR_REQ */
} MTP_addr_req_t;

/*
 *  MTP_ADDR_ACK, M_PCPROTO
 */
typedef struct MTP_addr_ack {
	mtp_ulong mtp_primitive;	/* always MTP_ADDR_ACK */
	mtp_ulong mtp_loc_length;	/* length of local MTP address */
	mtp_ulong mtp_loc_offset;	/* offset of local MTP address */
	mtp_ulong mtp_rem_length;	/* length of remote MTP address */
	mtp_ulong mtp_rem_offset;	/* offset of remote MTP address */
} MTP_addr_ack_t;

/*
 *  MTP_BIND_REQ, M_PROTO
 */
typedef struct MTP_bind_req {
	mtp_ulong mtp_primitive;	/* always MTP_BIND_REQ */
	mtp_ulong mtp_addr_length;	/* length of MTP address */
	mtp_ulong mtp_addr_offset;	/* offset of MTP address */
	mtp_ulong mtp_bind_flags;	/* bind flags */
} MTP_bind_req_t;

/*
 *  MTP_BIND_ACK, M_PCPROTO
 */
typedef struct MTP_bind_ack {
	mtp_ulong mtp_primitive;	/* always MTP_BIND_ACK */
	mtp_ulong mtp_addr_length;	/* length of bound MTP address */
	mtp_ulong mtp_addr_offset;	/* offset of bound MTP address */
} MTP_bind_ack_t;

/*
 *  MTP_UNBIND_REQ, M_PROTO
 */
typedef struct MTP_unbind_req {
	mtp_ulong mtp_primitive;	/* always MTP_UNBIND_REQ */
} MTP_unbind_req_t;

/*
 *  MTP_CONN_REQ, M_PROTO
 */
typedef struct MTP_conn_req {
	mtp_ulong mtp_primitive;	/* always MTP_CONN_REQ */
	mtp_ulong mtp_addr_length;	/* length of MTP address to connect */
	mtp_ulong mtp_addr_offset;	/* offset of MTP address to connect */
	mtp_ulong mtp_conn_flags;	/* connect flags */
} MTP_conn_req_t;

/*
 *  MTP_DISCON_REQ, M_PROTO, M_PCPROTO
 */
typedef struct MTP_discon_req {
	mtp_ulong mtp_primitive;	/* always MTP_DISCON_REQ */
} MTP_discon_req_t;

/*
 *  MTP_OPTMGMT_REQ, M_PROTO or M_PCPROTO
 */
typedef struct MTP_optmgmt_req {
	mtp_ulong mtp_primitive;	/* always MTP_OPTMGMT_REQ */
	mtp_ulong mtp_opt_length;	/* length of options */
	mtp_ulong mtp_opt_offset;	/* offset of options */
	mtp_ulong mtp_mgmt_flags;	/* management flags */
} MTP_optmgmt_req_t;

#define MTP_DEFAULT	0UL
#define MTP_CHECK	1UL
#define MTP_NEGOTIATE	2UL
#define MTP_CURRENT	3UL

/*
 *  MTP_OPTMGMT_ACK, M_PCPROTO
 */
typedef struct MTP_optmgmt_ack {
	mtp_ulong mtp_primitive;	/* always MTP_OPTMGMT_ACK */
	mtp_ulong mtp_opt_length;	/* length of options */
	mtp_ulong mtp_opt_offset;	/* offset of options */
	mtp_ulong mtp_mgmt_flags;	/* management flags */
} MTP_optmgmt_ack_t;

/*
 *  MTP_OK, MTP_ERROR, M_PCPROTO
 */
typedef struct MTP_ok_ack {
	mtp_ulong mtp_primitive;	/* always MTP_OK_ACK */
	mtp_ulong mtp_correct_prim;	/* correct primitive */
} MTP_ok_ack_t;

typedef struct MTP_error_ack {
	mtp_ulong mtp_primitive;	/* always MTP_ERROR_ACK */
	mtp_ulong mtp_error_primitive;	/* primitive in error */
	mtp_ulong mtp_mtpi_error;	/* MTP interface error */
	mtp_ulong mtp_unix_error;	/* UNIX error */
} MTP_error_ack_t;

#define MSYSERR		0UL
#define MACCESS		1UL
#define MBADADDR	2UL
#define MNOADDR		3UL
#define MBADPRIM	4UL
#define MOUTSTATE	5UL
#define MNOTSUPP	6UL
#define MBADFLAG	7UL
#define MBADOPT		8UL

/*
 *  MTP_TRANSFER_REQ, M_PROTO
 */
typedef struct MTP_transfer_req {
	mtp_ulong mtp_primitive;	/* always MTP_TRANSFER_REQ */
	mtp_ulong mtp_dest_length;	/* length of destination address */
	mtp_ulong mtp_dest_offset;	/* offset of destination address */
	mtp_ulong mtp_mp;		/* message priority */
	mtp_ulong mtp_sls;		/* signalling link selection */
} MTP_transfer_req_t;

/*
 *  MTP_TRANSFER_IND, M_PROTO
 */
typedef struct MTP_transfer_ind {
	mtp_ulong mtp_primitive;	/* always MTP_TRANSFER_IND */
	mtp_ulong mtp_srce_length;	/* length of source address */
	mtp_ulong mtp_srce_offset;	/* offset of source address */
	mtp_ulong mtp_mp;		/* message priority */
	mtp_ulong mtp_sls;		/* signalling link selection */
} MTP_transfer_ind_t;

/*
 *  MTP_PAUSE_IND, M_PCPROTO
 */
typedef struct MTP_pause_ind {
	mtp_ulong mtp_primitive;	/* always MTP_PAUSE_IND */
	mtp_ulong mtp_addr_length;	/* length of affected MTP address */
	mtp_ulong mtp_addr_offset;	/* offset of affected MTP address */
} MTP_pause_ind_t;

/*
 *  MTP_RESUME_IND, M_PCPROTO
 */
typedef struct MTP_resume_ind {
	mtp_ulong mtp_primitive;	/* always MTP_RESUME_IND */
	mtp_ulong mtp_addr_length;	/* length of affected MTP address */
	mtp_ulong mtp_addr_offset;	/* offset of affected MTP address */
} MTP_resume_ind_t;

/*
 *  MTP_STATUS_IND, M_PCPROTO
 */
typedef struct MTP_status_ind {
	mtp_ulong mtp_primitive;	/* always MTP_STATUS_IND */
	mtp_ulong mtp_addr_length;	/* length of affected MTP address */
	mtp_ulong mtp_addr_offset;	/* offset of affected MTP address */
	mtp_ulong mtp_type;		/* type */
	mtp_ulong mtp_status;		/* status */
} MTP_status_ind_t;

/*
   Type for MTP_STATUS_IND 
 */
#define MTP_STATUS_TYPE_CONG		0x00	/* MTP-STATUS refers to congestion */
#define MTP_STATUS_TYPE_UPU		0x01	/* MTP-STATUS referes to user part unavailability */

/*
   Status for MTP_STATUS_IND, with MTP_STATUS_TYPE_UPU 
 */
#define MTP_STATUS_UPU_UNKNOWN		0x01	/* User part unavailable: unknown */
#define MTP_STATUS_UPU_UNEQUIPPED	0x02	/* User part unavailable: unequipped remote user. */
#define MTP_STATUS_UPU_INACCESSIBLE	0x03	/* User part unavailable: inaccessible remote user. 
						 */

/*
   Status for MTP_STATUS_IND, with MTP_STATUS_TYPE_CONG 
 */
#define MTP_STATUS_CONGESTION_LEVEL0	0x00	/* Signalling network congestion level 0 */
#define MTP_STATUS_CONGESTION_LEVEL1	0x01	/* Signalling network congestion level 1 */
#define MTP_STATUS_CONGESTION_LEVEL2	0x02	/* Signalling network congestion level 2 */
#define MTP_STATUS_CONGESTION_LEVEL3	0x03	/* Signalling network congestion level 3 */
#define MTP_STATUS_CONGESTION		0x04	/* Signalling network congestion */

/*
 *  MTP_RESTART_BEGINS_IND, M_PCPROTO
 */
typedef struct MTP_restart_begins_ind {
	mtp_ulong mtp_primitive;	/* always MTP_RESTART_BEGINS_IND */
} MTP_restart_begins_ind_t;

/*
 *  MTP_RESTART_COMPLETE_IND, M_PCPROTO
 */
typedef struct MTP_restart_complete_ind {
	mtp_ulong mtp_primitive;	/* always MTP_RESTART_COMPLETE_IND */
} MTP_restart_complete_ind_t;

union MTP_primitives {
	mtp_ulong mtp_primitive;
	MTP_info_req_t info_req;
	MTP_info_ack_t info_ack;
	MTP_addr_req_t addr_req;
	MTP_addr_ack_t addr_ack;
	MTP_bind_req_t bind_req;
	MTP_bind_ack_t bind_ack;
	MTP_unbind_req_t unbind_req;
	MTP_conn_req_t conn_req;
	MTP_discon_req_t discon_req;
	MTP_optmgmt_req_t optmgmt_req;
	MTP_optmgmt_ack_t optmgmt_ack;
	MTP_ok_ack_t ok_ack;
	MTP_error_ack_t error_ack;
	MTP_transfer_req_t transfer_req;
	MTP_transfer_ind_t transfer_ind;
	MTP_pause_ind_t pause_ind;
	MTP_resume_ind_t resume_ind;
	MTP_status_ind_t status_ind;
	MTP_restart_complete_ind_t restart_complete_ind;
};

typedef struct {
	mtp_ulong mtp_affected_dpc;
} mtp_pause_ind_t;

typedef struct {
	mtp_ulong mtp_affected_dpc;
} mtp_resume_ind_t;

/*
 *  8.1 Transfer
 *
 *  The primitive "MTP-TRANSFER" is used between level 4 and level 3 (SMH) to
 *  provide the MTP message transfer service.
 */

/*
 *  8.2 Pause
 *
 *  The primitive "MTP-PAUSE" indicates to "Users" the total inability of
 *  providing the MTP service to the specified destination (see 7.2.6).
 *
 *  NOTE -  The signalling point is inacessible via the MTP.  The MTP will
 *  determine when the signalling point is again acessible and send MTP-RESUME
 *  indication.  The user should wait for such an indication and, meanwhile is
 *  not allowed to send messages on that signalling point.  If the remote peer
 *  user is thought to be unavailable, that condition may be maintained or
 *  cancelled at the local user's discretion.
 */

/*
 *  8.3 Resume
 *
 *  The primitive MTP-RESUME indications to the "User" the ability of
 *  providing the MTP service to the specified destination (See 7.2.6)
 *
 *  This primitive corresponds to the destination accessible state as defined
 *  in Recommendation Q.704.
 *
 *  NOTE -  When the MTP-RESUME indicaiton is given to each user, the MTP does
 *          not know whether the remote peer user is available.  This is the
 *          responsibility of each user.
 */

/*
 *  8.4 Status
 *
 *  The primitive "MTP-STATUS" indicates to the "Users" the partial inability
 *  of providing the MTP service to the specified destination.  The primitive
 *  is also used to indicate to a User that a remote corresponding User is
 *  unavailable and the cause for unavailability (see 11.2.7/Q.704).
 *
 *  In the case of national option with congestion priorities or multiple
 *  signalling link congestion states without prioritites as in Recommendation
 *  Q.704 are implemented, this "MTP-STATUS" primitive is also used to
 *  indicate a change of congestion level.
 *
 *  This primitive corresponds to the destination congested/User Part
 *  unavailable states as defined in Recommendation Q.704.
 *
 *  NOTE -  In the case of remote user unavailability, the user is responsible
 *          for determining the availability of this peer user.  The user is
 *          cautioned not to send normal traffic to the peer user because,
 *          while such peer is unavailable, no message will be delivered but
 *          each will result in a repeated "MTP-STATUS" indication.  The MTP
 *          will not send any further indications about the unavailability or
 *          availability of this peer user unless the local user continues to
 *          send messages to the peer user.
 */

/*
 *  8.5 Restart
 *
 *  When the MTP restart procedure is terminated, the MTP indicates the end of
 *  MTP restart to all local MTP Users showing each signalling point's
 *  accessibility or inaccessibility.  The means of doing this is
 *  implementation dependent (see 9/Q.704).
 */

/*
 *  MTP_STATUS_IND, M_PROTO or M_PCPROTO
 */
typedef struct {
	mtp_ulong mtp_affected_dpc;
	mtp_uchar mtp_cause;
	mtp_uchar mtp_level;
} mtp_status_ind_t;

typedef struct {
	mtp_ulong dpc;
	mtp_ulong opc;
	mtp_ulong sls;
} mtp_rl_t;

typedef struct {
	mtp_uchar si;
	mtp_uchar mp;
	mtp_uchar ni;
	mtp_rl_t rl;
} mtp_hdr_t;

typedef struct {
	mtp_uchar si;
	mtp_uchar mp;
	mtp_uchar ni;
	mtp_rl_t rl;
	mtp_uchar h0;
	mtp_uchar h1;
} mtp_msu_t;

typedef struct {
	mtp_long mtp_primitive;		/* MTP_MSU_REQ, MTP_MSU_IND */
	mtp_ulong mtp_signal;		/* MTP_SIGNAL_COO */
	mtp_msu_t mtp_msg;
	mtp_ulong mtp_slc;
	mtp_ulong mtp_fsnc;
} mtp_signal_coo_t;

typedef struct {
	mtp_long mtp_primitive;		/* MTP_MSU_REQ, MTP_MSU_IND */
	mtp_ulong mtp_signal;		/* MTP_SIGNAL_COA */
	mtp_msu_t mtp_msg;
} mtp_signal_coa_t;

typedef struct {
	mtp_long mtp_primitive;		/* MTP_MSU_REQ, MTP_MSU_IND */
	mtp_ulong mtp_signal;		/* MTP_SIGNAL_CBD */
	mtp_msu_t mtp_msg;
} mtp_signal_cbd_t;

typedef struct {
	mtp_long mtp_primitive;		/* MTP_MSU_REQ, MTP_MSU_IND */
	mtp_ulong mtp_signal;		/* MTP_SIGNAL_CBA */
	mtp_msu_t mtp_msg;
} mtp_signal_cba_t;

typedef struct {
	mtp_long mtp_primitive;		/* MTP_MSU_REQ, MTP_MSU_IND */
	mtp_ulong mtp_signal;		/* MTP_SIGNAL_ECO */
	mtp_msu_t mtp_msg;
} mtp_signal_eco_t;

typedef struct {
	mtp_long mtp_primitive;		/* MTP_MSU_REQ, MTP_MSU_IND */
	mtp_ulong mtp_signal;		/* MTP_SIGNAL_ECA */
	mtp_msu_t mtp_msg;
} mtp_signal_eca_t;

typedef struct {
	mtp_long mtp_primitive;		/* MTP_MSU_REQ, MTP_MSU_IND */
	mtp_ulong mtp_signal;		/* MTP_SIGNAL_RCT */
	mtp_msu_t mtp_msg;
} mtp_signal_rct_t;

typedef struct {
	mtp_long mtp_primitive;		/* MTP_MSU_REQ, MTP_MSU_IND */
	mtp_ulong mtp_signal;		/* MTP_SIGNAL_TFC */
	mtp_msu_t mtp_msg;
} mtp_signal_tfc_t;

typedef struct {
	mtp_long mtp_primitive;		/* MTP_MSU_REQ, MTP_MSU_IND */
	mtp_ulong mtp_signal;		/* MTP_SIGNAL_TFP */
	mtp_msu_t mtp_msg;
} mtp_signal_tfp_t;

typedef struct {
	mtp_long mtp_primitive;		/* MTP_MSU_REQ, MTP_MSU_IND */
	mtp_ulong mtp_signal;		/* MTP_SIGNAL_TFR */
	mtp_msu_t mtp_msg;
} mtp_signal_tfr_t;

typedef struct {
	mtp_long mtp_primitive;		/* MTP_MSU_REQ, MTP_MSU_IND */
	mtp_ulong mtp_signal;		/* MTP_SIGNAL_TFA */
	mtp_msu_t mtp_msg;
} mtp_signal_tfa_t;

typedef struct {
	mtp_long mtp_primitive;		/* MTP_MSU_REQ, MTP_MSU_IND */
	mtp_ulong mtp_signal;		/* MTP_SIGNAL_RSP */
	mtp_msu_t mtp_msg;
} mtp_signal_rsp_t;

typedef struct {
	mtp_long mtp_primitive;		/* MTP_MSU_REQ, MTP_MSU_IND */
	mtp_ulong mtp_signal;		/* MTP_SIGNAL_RSR */
	mtp_msu_t mtp_msg;
} mtp_signal_rsr_t;

typedef struct {
	mtp_long mtp_primitive;		/* MTP_MSU_REQ, MTP_MSU_IND */
	mtp_ulong mtp_signal;		/* MTP_SIGNAL_LIN */
	mtp_msu_t mtp_msg;
} mtp_signal_lin_t;

typedef struct {
	mtp_long mtp_primitive;		/* MTP_MSU_REQ, MTP_MSU_IND */
	mtp_ulong mtp_signal;		/* MTP_SIGNAL_LUN */
	mtp_msu_t mtp_msg;
} mtp_signal_lun_t;

typedef struct {
	mtp_long mtp_primitive;		/* MTP_MSU_REQ, MTP_MSU_IND */
	mtp_ulong mtp_signal;		/* MTP_SIGNAL_LIA */
	mtp_msu_t mtp_msg;
} mtp_signal_lia_t;

typedef struct {
	mtp_long mtp_primitive;		/* MTP_MSU_REQ, MTP_MSU_IND */
	mtp_ulong mtp_signal;		/* MTP_SIGNAL_LUA */
	mtp_msu_t mtp_msg;
} mtp_signal_lua_t;

typedef struct {
	mtp_long mtp_primitive;		/* MTP_MSU_REQ, MTP_MSU_IND */
	mtp_ulong mtp_signal;		/* MTP_SIGNAL_LID */
	mtp_msu_t mtp_msg;
} mtp_signal_lid_t;

typedef struct {
	mtp_long mtp_primitive;		/* MTP_MSU_REQ, MTP_MSU_IND */
	mtp_ulong mtp_signal;		/* MTP_SIGNAL_LFU */
	mtp_msu_t mtp_msg;
} mtp_signal_lfu_t;

typedef struct {
	mtp_long mtp_primitive;		/* MTP_MSU_REQ, MTP_MSU_IND */
	mtp_ulong mtp_signal;		/* MTP_SIGNAL_LLT */
	mtp_msu_t mtp_msg;
} mtp_signal_llt_t;

typedef struct {
	mtp_long mtp_primitive;		/* MTP_MSU_REQ, MTP_MSU_IND */
	mtp_ulong mtp_signal;		/* MTP_SIGNAL_LRT */
	mtp_msu_t mtp_msg;
} mtp_signal_lrt_t;

typedef struct {
	mtp_long mtp_primitive;		/* MTP_MSU_REQ, MTP_MSU_IND */
	mtp_ulong mtp_signal;		/* MTP_SIGNAL_TRA */
	mtp_msu_t mtp_msg;
} mtp_signal_tra_t;

typedef struct {
	mtp_long mtp_primitive;		/* MTP_MSU_REQ, MTP_MSU_IND */
	mtp_ulong mtp_signal;		/* MTP_SIGNAL_DLC */
	mtp_msu_t mtp_msg;
} mtp_signal_dlc_t;

typedef struct {
	mtp_long mtp_primitive;		/* MTP_MSU_REQ, MTP_MSU_IND */
	mtp_ulong mtp_signal;		/* MTP_SIGNAL_CSS */
	mtp_msu_t mtp_msg;
} mtp_signal_css_t;

typedef struct {
	mtp_long mtp_primitive;		/* MTP_MSU_REQ, MTP_MSU_IND */
	mtp_ulong mtp_signal;		/* MTP_SIGNAL_CNS */
	mtp_msu_t mtp_msg;
} mtp_signal_cns_t;

typedef struct {
	mtp_long mtp_primitive;		/* MTP_MSU_REQ, MTP_MSU_IND */
	mtp_ulong mtp_signal;		/* MTP_SIGNAL_CNP */
	mtp_msu_t mtp_msg;
} mtp_signal_cnp_t;

typedef struct {
	mtp_long mtp_primitive;		/* MTP_MSU_REQ, MTP_MSU_IND */
	mtp_ulong mtp_signal;		/* MTP_SIGNAL_UPU */
	mtp_msu_t mtp_msg;
} mtp_signal_upu_t;

typedef struct {
	mtp_long mtp_primitive;		/* MTP_MSU_REQ, MTP_MSU_IND */
	mtp_ulong mtp_signal;		/* MTP_SIGNAL_SLTM */
	mtp_msu_t mtp_msg;
} mtp_signal_sltm_t;

typedef struct {
	mtp_long mtp_primitive;		/* MTP_MSU_REQ, MTP_MSU_IND */
	mtp_ulong mtp_signal;		/* MTP_SIGNAL_SLTA */
	mtp_msu_t mtp_msg;
} mtp_signal_slta_t;

typedef struct {
	mtp_long mtp_primitive;		/* MTP_MSU_REQ, MTP_MSU_IND */
	mtp_ulong mtp_signal;		/* MTP_SIGNAL_SSLTM */
	mtp_msu_t mtp_msg;
} mtp_signal_ssltm_t;

typedef struct {
	mtp_long mtp_primitive;		/* MTP_MSU_REQ, MTP_MSU_IND */
	mtp_ulong mtp_signal;		/* MTP_SIGNAL_SSLTA */
	mtp_msu_t mtp_msg;
} mtp_signal_sslta_t;

typedef struct {
	mtp_long mtp_primitive;		/* MTP_MSU_REQ, MTP_MSU_IND */
	mtp_ulong mtp_signal;		/* MTP_SIGNAL_USER */
	mtp_hdr_t mtp_msg;
} mtp_signal_user_t;

typedef union {
	mtp_long mtp_primitive;
	mtp_signal_user_t msg;
	mtp_signal_coo_t coo;
	mtp_signal_coa_t coa;
	mtp_signal_cbd_t cbd;
	mtp_signal_cba_t cba;
	mtp_signal_eco_t eco;
	mtp_signal_eca_t eca;
	mtp_signal_rct_t rct;
	mtp_signal_tfc_t tfc;
	mtp_signal_tfp_t tfp;
	mtp_signal_tfr_t tfr;
	mtp_signal_tfa_t tfa;
	mtp_signal_rsp_t rsp;
	mtp_signal_rsr_t rsr;
	mtp_signal_lin_t lin;
	mtp_signal_lun_t lun;
	mtp_signal_lia_t lia;
	mtp_signal_lua_t lua;
	mtp_signal_lid_t lid;
	mtp_signal_lfu_t lfu;
	mtp_signal_llt_t llt;
	mtp_signal_lrt_t lrt;
	mtp_signal_tra_t tra;
	mtp_signal_dlc_t dlc;
	mtp_signal_css_t css;
	mtp_signal_cns_t cns;
	mtp_signal_cnp_t cnp;
	mtp_signal_upu_t upu;
	mtp_signal_sltm_t sltm;
	mtp_signal_slta_t slta;
	mtp_signal_ssltm_t ssltm;
	mtp_signal_sslta_t sslta;
	mtp_signal_user_t user;
} MTP_signals;

/*
 *  MTP_MSU_REQ , M_PROTO or M_PCPROTO (M_DATA)
 */
typedef MTP_signals mtp_msu_req_t;
/*
 *  MTP_MSU_IND , M_PROTO or M_PCPROTO (M_DATA)
 */
typedef MTP_signals mtp_msu_ind_t;

#define MTP_SIGNAL_NONE		 0
#define MTP_SIGNAL_COO		 1	/* STM */
#define MTP_SIGNAL_COA		 2	/* STM */
#define MTP_SIGNAL_CBD		 3	/* STM */
#define MTP_SIGNAL_CBA		 4	/* STM */
#define MTP_SIGNAL_ECO		 5	/* STM */
#define MTP_SIGNAL_ECA		 6	/* STM */
#define MTP_SIGNAL_LIN		14	/* STM */
#define MTP_SIGNAL_LUN		15	/* STM */
#define MTP_SIGNAL_LIA		16	/* STM */
#define MTP_SIGNAL_LUA		17	/* STM */
#define MTP_SIGNAL_LID		18	/* STM */
#define MTP_SIGNAL_LFU		19	/* STM */
#define MTP_SIGNAL_LLT		20	/* STM */
#define MTP_SIGNAL_LRT		21	/* STM */
#define MTP_SIGNAL_TRA		22	/* STM */

#define MTP_SIGNAL_RCT		 7	/* SRM */
#define MTP_SIGNAL_TFC		 8	/* SRM */
#define MTP_SIGNAL_TFP		 9	/* SRM */
#define MTP_SIGNAL_TFR		10	/* SRM */
#define MTP_SIGNAL_TFA		11	/* SRM */
#define MTP_SIGNAL_RSP		12	/* SRM */
#define MTP_SIGNAL_RSR		13	/* SRM */
#define MTP_SIGNAL_UPU		27	/* SRM */

#define MTP_SIGNAL_DLC		23	/* SLM */
#define MTP_SIGNAL_CSS		24	/* SLM */
#define MTP_SIGNAL_CNS		25	/* SLM */
#define MTP_SIGNAL_CNP		26	/* SLM */

#define MTP_SIGNAL_SLTM		28	/* SLTC */
#define MTP_SIGNAL_SLTA		29	/* SLTC */
#define MTP_SIGNAL_SSLTM	30	/* SLTC */
#define MTP_SIGNAL_SSLTA	31	/* SLTC */

#define MTP_SIGNAL_USER		32	/* L4 */

#endif				/* __SS7_MTPI_H__ */
