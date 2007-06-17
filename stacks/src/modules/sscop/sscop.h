/*****************************************************************************

 @(#) $Id: sscop.h,v 0.9.2.3 2007/06/17 01:56:28 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
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

 Last Modified $Date: 2007/06/17 01:56:28 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sscop.h,v $
 Revision 0.9.2.3  2007/06/17 01:56:28  brian
 - updates for release, remove any later language

 *****************************************************************************/

#ifndef __SSCOP_H__
#define __SSCOP_H__

#ident "@(#) $RCSfile: sscop.h,v $ $Name:  $($Revision: 0.9.2.3 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

/*
 *  Signals that are initiated by the SSCF.
 */
#define	AA_ESTABLISH_REQ		N_CONN_REQ
#define AA_ESTABLISH_RES		N_CONN_RES
#define AA_RELEASE_REQ			N_DISCON_REQ
#define AA_DATA_REQ			N_DATA_REQ
#define AA_EXDATA_REQ			N_EXDATA_REQ
#define	AA_INFO_REQ			N_INFO_REQ
#define AA_BIND_REQ			N_BIND_REQ
#define AA_UNBIND_REQ			N_UNBIND_REQ
#define AA_UNITDATA_REQ			N_UNITDATA_REQ
#define MAA_UNITDATA_REQ		N_UNITDATA_REQ
#define AA_OPTMGMT_REQ			N_OPTMGMT_REQ

/*
 *  Signals that are initiated by the SSCOP.
 */
#define AA_ESTABLISH_IND		N_CONN_IND
#define AA_ESTABLISH_CON		N_CONN_CON
#define AA_RELEASE_IND			N_DISCON_IND
#define AA_DATA_IND			N_DATA_IND
#define AA_EXDATA_IND			N_EXDATA_IND
#define AA_INFO_ACK			N_INFO_ACK
#define AA_BIND_ACK			N_BIND_ACK
#define	AA_ERROR_ACK			N_ERROR_ACK
#define AA_OK_ACK			N_OK_ACK
#define AA_UNITDATA_IND			N_UNITDATA_IND
#define MAA_UNITDATA_IND		N_UNITDATA_IND
#define MAA_ERROR_IND			N_UDERROR_IND

#define AA_RESYNC_REQ			N_RESET_REQ
#define AA_RESYNC_IND			N_RESET_IND
#define AA_RESYNC_RES			N_RESET_RES
#define AA_RESYNC_CON			N_RESET_CON

#define AA_RELEASE_CON			10
#define AA_RECOVER_IND			29
#define AA_RECOVER_RES			30
#define AA_RETRIEVE_REQ			31
#define AA_RETRIEVE_IND			32
#define AA_RETRIEVAL_COMPLETE_IND	33

#define MAA_SET_TIMER_REQ		34
#define MAA_ADD_LINK			35
#define MAA_REMOVE_LINK_REQ		36
#define MAA_REMOVE_LINK_IND		37

/*
 *  Initialization events
 */
#define	AE_BIND_REQ	NE_BIND_REQ	/* bind request */
#define AE_UNBIND_REQ	NE_UNBIND_REQ	/* unbind request */
#define AE_OPTMGMT_REQ	NE_OPTMGMT_REQ	/* manage options request */
#define AE_BIND_ACK	NE_BIND_ACK	/* bind acknowledgement */
#define AE_ERROR_ACK	NE_ERROR_ACK	/* error acknoweldgement */
#define AE_OK_ACK1	NE_OK_ACK1	/* ok ack, outcnt == 0 */
#define AE_OK_ACK2	NE_OK_ACK2	/* ok ack, outcnt == 1, q == rq */
#define AE_OK_ACK3	NE_OK_ACK3	/* ok ack, outcnt == 1, q != rq */
#define AE_OK_ACK4	NE_OK_ACK4	/* ok ack, outcnt > 1 */

/*
 *  Connection mode events
 */
#define AE_CONN_REQ	NE_CONN_REQ	/* connect request */
#define AE_CONN_RES	NE_CONN_RES	/* connect response */
#define AE_DISCON_REQ	NE_DISCON_REQ	/* disconnect request */
#define AE_DATA_REQ	NE_DATA_REQ	/* data request */
#define AE_EXDATA_REQ	NE_EXDATA_REQ	/* expedited data request */
#define AE_CONN_IND	NE_CONN_IND	/* connect indication */
#define AE_CONN_CON	NE_CONN_CON	/* connect confirm */
#define AE_DATA_IND	NE_DATA_IND	/* data indication */
#define AE_EXDATA_IND	NE_EXDATA_IND	/* expedited data indication */
#define AE_DISCON_IND1	NE_DISCON_IND1	/* disconnect indication, outcnt == 0 */
#define AE_DISCON_IND2	NE_DISCON_IND2	/* disconnect indication, outcnt == 1 */
#define AE_DISCON_IND3	NE_DISCON_IND3	/* disconnect indication, outcnt > 1 */
#define AE_PASS_CON	NE_PASS_CON	/* pass connection */
#define AE_RESYNC_REQ	NE_RESET_REQ	/* reset request */
#define AE_RESYNC_RES	NE_RESET_RES	/* reset response */
#define AE_DATACK_REQ	NE_DATACK_REQ	/* data acknowledgement request */
#define AE_DATACK_IND	NE_DATACK_IND	/* data acknowledgement indication */
#define AE_RESYNC_IND	NE_RESET_IND	/* reset indication */
#define AE_RESYNC_CON	NE_RESET_CON	/* reset confirm */

/*
 *  Connection-less events
 */
#define AE_UNITDATA_REQ	NE_UNITDATA_REQ	/* unitdata request */
#define AE_UNITDATA_IND NE_UNITDATA_IND	/* unitdata indication */
#define AE_UDERROR_IND	NE_UDERROR_IND	/* unitdata error indication */

#define AE_NOEVENTS	NE_NOEVENTS

/*
 *  AA interface states
 */
#define AS_UNBND	NS_UNBND	/* SSCF not bound to network address */
#define AS_WACK_BREQ	NS_WACK_BREQ	/* Waiting acknowledgement of AA_BIND_REQ */
#define AS_WACK_UREQ	NS_WACK_UREQ	/* Waiting acknolwedgement of AA_UNBIND_REQ */
#define AS_IDLE		NS_IDLE	/* Idle, no connection */
#define AS_WACK_OPTREQ	NS_WACK_OPTREQ	/* Waiting acknowledgement of AA_OPTMGMT_REQ */
#define AS_WACK_RRES	NS_WACK_RRES	/* Waiting acknowledgement of AA_RESYNC_REQ */
#define AS_WCON_CREQ	NS_WCON_CREQ	/* Waiting confirmation of AA_CONN_REQ */
#define AS_WRES_CIND	NS_WRES_CIND	/* Waiting response of AA_CONN_IND */
#define AS_WACK_CRES	NS_WACK_CRES	/* Waiting acknowledgement of N_CONN_RES */
#define AS_DATA_XFER	NS_DATA_XFER	/* Connection-mode data transfer */
#define AS_WCON_RREQ	NS_WCON_RREQ	/* Waiting confirmation of N_RESYNC_REQ */
#define AS_WRES_RIND	NS_WRES_RIND	/* Waiting response of N_RESYNC_IND */
#define AS_WACK_DREQ6	NS_WACK_DREQ6	/* Waiting ack of N_DISCON_REQ */
#define AS_WACK_DREQ7	NS_WACK_DREQ7	/* Waiting ack of N_DISCON_REQ */
#define AS_WACK_DREQ9	NS_WACK_DREQ9	/* Waiting ack of N_DISCON_REQ */
#define AS_WACK_DREQ10	NS_WACK_DREQ10	/* Waiting ack of N_DISCON_REQ */
#define AS_WACK_DREQ11	NS_WACK_DREQ11	/* Waiting ack of N_DISCON_REQ */

#define AS_NOSTATES	NS_NOSTATES	/* No states */

/*
 *  AA_ERROR_ACK error return code values
 */
#define AABADADDR	NBADADDR	/* Incorrect address format/illegal address info */
#define AABADOPT	NBADOPT	/* Options in incorrect format or contain illegal information */
#define AAACCESS	NACCESS	/* User did not have proper permissions */
#define AANOADDR	NNOADDR	/* SSCOP could not allocate address */
#define AAOUTSTATE	NOUTSTATE	/* Primitive was issued in wrong sequence */
#define AABADSEQ	NBADSEQ	/* Sequence number in primitive was incorrect/illegal */
#define AASYSERR	NSYSERR	/* UNIX system error */
#define AABADDATA	NBADDATA	/* User data spec. outside range supported by SSCOP */
#define AABADFLAG	NBADFLAG	/* Flags in primitive were illegal/incorrect */
#define AANOTSUPPORT	NNOTSUPPORT	/* Primitive type is not supported by SSCOP */
#define AABOUND		NBOUND	/* Illegal second attempt to bind listener or default listener */
#define AABADQOSPARAM	NBADQOSPARAM	/* QOS values sepcified are out of range */
#define AABADQOSTYPE	NBADQOSTYPE	/* QOS struct type specified is not supported */
#define AABADTOKEN	NBADTOKEN	/* Token used is not associated with an open stream */

/*
 *  MAA_ERROR_IND reason codes
 */
#define AA_ERR_UNDEFINED	N_UD_UNDEFINED	/* no reason specified */
#define AA_ERR_TD_EXCEEDED	N_UD_TD_EXCEEDED	/* transit delay exceeded */
#define AA_ERR_CONGESTION	N_UD_CONGESTION	/* SSCOP congestion */
#define AA_ERR_QOS_UNAVAIL	N_UD_QOS_UNAVAIL	/* requested QOS/service characteristic
							   unavailable */
#define AA_ERR_LIFE_EXCEEDED	N_UD_LIFE_EXCEEDED	/* NSDU Lifetime exceeded */
#define AA_ERR_ROUTE_UNAVAIL	N_UD_ROUTE_UNAVAIL	/* suitable route unavailable */

/*
 *  Originator for resyncs and disconnects
 */
#define AA_PROVIDER		0x0100	/* provider originated reset/disconnect */
#define AA_USER			0x0101	/* user originated reset/disconnect */
#define AA_UNDEFINED		0x0102	/* reset/disconnect originator undefined */

/*
 *  Disconnect reasons when the originator is AA_PROVIDER
 */
#define AA_DISC_P		0x0300	/* Disconnection-permanent condition */
#define AA_DISC_T		0x0301	/* Disconnection-transient condition */
#define AA_REJ_NSAP_UNKNOWN	0x0302	/* Connection rejection NSAP address unknown (permanent
					   condition) */
#define AA_REJ_NSAP_UNREACH_P	0x0303	/* Connection rejection NSAP unreachable permanent
					   condition */
#define AA_REJ_NSAP_UNREACH_T	0x0304	/* Connection rejection NSAP unreachable temporary
					   condition */

/*
 *  Diconnect reasons when the originator is AA_USER
 */
#define	AA_DISC_NORMAL		0x0400	/* Disconnection-normal condition */
#define	AA_DISC_ABNORMAL	0x0401	/* Disconnection-abnormal condition */
#define	AA_REJ_P		0x0402	/* Connection rejection-permanent condition */
#define	AA_REJ_T		0x0403	/* Connection rejection-temporary condition */
#define	AA_REJ_INCOMPAT_INFO	0x0406	/* Connection rejection-incompatible info */

/*
 *  Disconnect reasons when the originator is AA_PROVIDER or AA_USER
 */
#define AA_REJ_QOS_UNAVAIL_P	0x0305	/* Connection rejection-QOS unavailable (permanent
					   condition) */
#define AA_REJ_QOS_UNAVAIL_T	0x0306	/* Connection rejection-QOS unavailable (transient
					   condition) */
#define AA_REJ_UNSPECIFIED	0x0307	/* Connection rejection-reason unspecified */

/*
 *  Reset reasons when originator is AA_PROVIDER
 */
#define AA_CONGESTION		0x0500	/* Reset due to congestion */
#define AA_RESET_UNSPECIFIED	0x0501	/* Reset-reason unspecified */

/*
 *  Reset reasons when originator is AA_USER
 */
#define AA_USER_RESYNC		0x0600	/* Reset due to user resynchronization */

/*
 *  CONN_flags definition (used in AA_establish_req, AA_establish_ind,
 *  AA_establish_res, and AA_establish_con primitives)
 */
#define AA_REC_CONF_OPT		0x00000001L	/* Receipt confirmation Selection and Support */
#define AA_EX_DATA_OPT		0x00000002L	/* Expedited Data Selection and Support */
#define AA_BUFFER_RELEASE_OPT	0x00000008L	/* Buffer Release Selection and Support */

/*
 *  This flag is used with the OPTIONS_flags field of the AA_info_ack as well
 *  as the OPTMGMT_flags of the AA_optmgmt_req primtives
 */
#define AA_DEFAULT_RC_SEL	0x00000004L	/* Indicates if default receipt confirmation */

/*
 *  BIND_flags (used with AA_bind_req primitive)
 */
#define AA_DEFAULT_LISTENER	0x00000001L	/* this stream is the default listener */
#define AA_TOKEN_REQUEST	0x00000002L	/* "token" should be assigned to the stream */

typedef N_info_req_t AA_info_req_t;
typedef N_info_ack_t AA_info_ack_t;
/*
 *  Service types supported by the AA_PROVIDER
 */
#define AA_CONS		N_CONS	/* Connection-mode network service provided */
#define AA_CLNS		N_CLNS	/* Connection-less network service provided */
#define AA_CONS_RTRV	3	/* Connection-mode network service w/ buffer retrieval */
/*
 *  Valid provider types
 */
#define AA_SNICFP	N_SNICFP
#define AA_SUBNET	N_SUBNET

typedef N_bind_req_t AA_bind_req_t;
typedef N_bind_ack_t AA_bind_ack_t;
typedef N_unbind_req_t AA_unbind_req_t;
typedef N_optmgmt_req_t AA_optmgmt_req_t;
typedef N_error_ack_t AA_error_ack_t;
typedef N_ok_ack_t AA_ok_ack_t;
typedef N_conn_req_t AA_establish_req_t;
typedef N_conn_ind_t AA_establish_ind_t;
typedef N_conn_res_t AA_establish_res_t;
typedef N_conn_con_t AA_establish_con_t;

typedef N_data_req_t AA_data_req_t;

typedef struct {
	np_ulong PRIM_type;
	np_ulong DATA_xfer_flags;
	np_ulong SEQUENCE_number;
} AA_data_ind_t;
/*
 *  DATA_xfer_flags for use with AA_data_req and AA_data_ind
 */
#define AA_MORE_DATA_FLAG   N_MORE_DATA_FLAG	/* next AIDU is part of this ASDU */
#define AA_RC_FLAG	    N_RC_FLAG	/* receipt confirmation required */
#define AA_OUT_OF_SEQ	    0x00000004L	/* out of sequence data */
typedef N_datack_req_t AA_datack_req_t;
typedef N_datack_ind_t AA_datack_ind_t;
typedef N_exdata_req_t AA_exdata_ind_t;
typedef N_reset_req_t AA_resync_req_t;
typedef N_reset_ind_t AA_resync_ind_t;
typedef N_reset_res_t AA_resync_res_t;
typedef N_reset_con_t AA_resync_con_t;
typedef N_discon_req_t AA_release_req_t;
typedef N_discon_ind_t AA_release_ind_t;
typedef N_discon_req_t AA_release_con_t;

typedef N_reset_ind_t AA_recover_ind_t;
typedef N_reset_res_t AA_recover_res_t;
typedef N_unitdata_req_t AA_unitdata_req_t;
typedef N_unitdata_ind_t AA_unitdata_ind_t;
typedef N_unitdata_req_t MAA_unitdata_req_t;
typedef N_unitdata_ind_t MAA_unitdata_ind_t;
typedef N_uderror_ind_t MAA_error_ind_t;	/* might need error count */

typedef struct {
	ulong PRIM_type;
	ulong RETRIEVAL_number;
} AA_retrieve_req_t;

typedef struct {
	ulong PRIM_type;
} AA_retrieve_ind_t;

typedef struct {
	ulong PRIM_type;
} AA_retrieval_complete_ind_t;

typedef struct {
	ulong PRIM_type;
	ulong TIMEOUT_value;
} MAA_set_timer_req_t;

typedef struct {
	ulong PRIM_type;
	ulong LINK_id;
} MAA_add_link_t;

typedef struct {
	ulong PRIM_type;
	ulong LINK_id;
} MAA_remove_link_req_t;

typedef struct {
	ulong PRIM_type;
	ulong LINK_id;
} MAA_remove_link_ind_t;

#endif				/* __SSCOP_H__ */
