/*****************************************************************************

 @(#) $Id: cci.h,v 0.9.2.6 2008-04-29 07:10:42 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation; version 3 of the License.

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

 Last Modified $Date: 2008-04-29 07:10:42 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: cci.h,v $
 Revision 0.9.2.6  2008-04-29 07:10:42  brian
 - updating headers for release

 Revision 0.9.2.5  2007/08/13 19:28:11  brian
 - updated headers

 Revision 0.9.2.4  2006/10/31 21:04:37  brian
 - changes for 32-bit compatibility and remove HZ dependency

 *****************************************************************************/

#ifndef __CCI_H__
#define __CCI_H__

#ident "@(#) $RCSfile: cci.h,v $ $Name:  $($Revision: 0.9.2.6 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

/* This file can be processed by doxygen(1). */

typedef lmi_long cc_long;
typedef lmi_ulong cc_ulong;
typedef lmi_ushort cc_ushort;
typedef lmi_uchar cc_uchar;

#define CC_INFO_REQ		0
#define CC_OPTMGMT_REQ		1
#define CC_BIND_REQ		2
#define CC_UNBIND_REQ		3
#define CC_ADDR_REQ		4
#define CC_SETUP_REQ		5
#define CC_MORE_INFO_REQ	6	/* ISDN only */
#define CC_INFORMATION_REQ	7
#define CC_CONT_CHECK_REQ	8	/* ISUP only */
#define CC_CONT_TEST_REQ	9	/* ISUP only */
#define CC_CONT_REPORT_REQ	10	/* ISUP only */
#define CC_SETUP_RES		11
#define CC_PROCEEDING_REQ	12
#define CC_ALERTING_REQ		13
#define CC_PROGRESS_REQ		14
#define CC_IBI_REQ		15	/* (same as CC_DISCONNECT_REQ in ISDN) */
#define CC_DISCONNECT_REQ	15
#define CC_CONNECT_REQ		16
#define CC_SETUP_COMPLETE_REQ	17	/* ISDN only */
#define CC_FORWXFER_REQ		18	/* ISUP only */
#define CC_SUSPEND_REQ		19
#define CC_SUSPEND_RES		20	/* ISDN only */
#define CC_SUSPEND_REJECT_REQ	21	/* ISDN only */
#define CC_RESUME_REQ		22
#define CC_RESUME_RES		23	/* ISDN only */
#define CC_RESUME_REJECT_REQ	24	/* ISDN only */
#define CC_REJECT_REQ		25	/* ISDN only */
#define CC_RELEASE_REQ		26
#define CC_RELEASE_RES		27	/* ISUP only */
#define CC_NOTIFY_REQ		28	/* ISDN only */
#define CC_RESTART_REQ		29	/* ISDN only */
#define CC_RESET_REQ		30	/* ISUP only */
#define CC_RESET_RES		31	/* ISUP only */
#define CC_BLOCKING_REQ		32	/* ISUP only */
#define CC_BLOCKING_RES		33	/* ISUP only */
#define CC_UNBLOCKING_REQ	34	/* ISUP only */
#define CC_UNBLOCKING_RES	35	/* ISUP only */
#define CC_QUERY_REQ		36	/* ISUP only */
#define CC_QUERY_RES		37	/* ISUP only */
#define CC_STOP_REQ		38	/* ISUP only */

#define CC_OK_ACK		64
#define CC_ERROR_ACK		65
#define CC_INFO_ACK		66
#define CC_BIND_ACK		67
#define CC_OPTMGMT_ACK		68
#define CC_ADDR_ACK		69
#define CC_CALL_REATTEMPT_IND	70	/* ISUP only */
#define CC_SETUP_IND		71	/* recv IAM */
#define CC_MORE_INFO_IND	72	/* ISDN only */
#define CC_INFORMATION_IND	73	/* recv SAM */
#define CC_CONT_CHECK_IND	74	/* ISUP only */
#define CC_CONT_TEST_IND	75	/* ISUP only */
#define CC_CONT_REPORT_IND	76	/* ISUP only */
#define CC_SETUP_CON		77
#define CC_PROCEEDING_IND	78	/* recv ACM w/ no indication if proceeding not sent before */
#define CC_ALERTING_IND		79	/* recv ACM w/ subscriber free indication */
#define CC_PROGRESS_IND		80	/* recv ACM w/ no indication and ATP parameter and call
					   proceeding sent */
#define CC_IBI_IND		81	/* recv ACM or CPG w/ inband info (same as
					   CC_DISCONNECT_IND in ISDN) */
#define CC_DISCONNECT_IND	81
#define CC_CONNECT_IND		82
#define CC_SETUP_COMPLETE_IND	83	/* ISDN only */
#define CC_FORWXFER_IND		84	/* ISUP only */
#define CC_SUSPEND_IND		85
#define CC_SUSPEND_CON		86	/* ISDN only */
#define CC_SUSPEND_REJECT_IND	87	/* ISDN only */
#define CC_RESUME_IND		88
#define CC_RESUME_CON		89	/* ISDN only */
#define CC_RESUME_REJECT_IND	90	/* ISDN only */
#define CC_REJECT_IND		91	/* ISDN only */
#define CC_CALL_FAILURE_IND	92	/* ISUP only (ERROR_IND?) */
#define CC_RELEASE_IND		93
#define CC_RELEASE_CON		94
#define CC_NOTIFY_IND		95	/* ISDN only */
#define CC_RESTART_CON		96	/* ISDN only */
#define CC_STATUS_IND		97	/* ISDN only */
#define CC_ERROR_IND		98	/* ISDN only (CALL_FAILURE_IND?) */
#define CC_DATALINK_FAILURE_IND	99	/* ISDN only */
#define CC_INFO_TIMEOUT_IND	100
#define CC_RESET_IND		101	/* ISUP only */
#define CC_RESET_CON		102	/* ISUP only */
#define CC_BLOCKING_IND		103	/* ISUP only */
#define CC_BLOCKING_CON		104	/* ISUP only */
#define CC_UNBLOCKING_IND	105	/* ISUP only */
#define CC_UNBLOCKING_CON	106	/* ISUP only */
#define CC_QUERY_IND		107	/* ISUP only */
#define CC_QUERY_CON		108	/* ISUP only */
#define CC_STOP_IND		109	/* ISUP only */
#define CC_MAINT_IND		110	/* ISUP only */
#define CC_START_RESET_IND	111	/* ISUP only */

/*
 *  Interface state
 */
enum {
	CCS_UNBND,
	CCS_IDLE,
	CCS_WIND_SETUP,
	CCS_WREQ_SETUP,
	CCS_WREQ_MORE,
	CCS_WIND_MORE,
	CCS_WREQ_INFO,
	CCS_WIND_INFO,
	CCS_WACK_INFO,
	CCS_WCON_SREQ,
	CCS_WRES_SIND,
	CCS_WREQ_CCREP,
	CCS_WIND_CCREP,
	CCS_WREQ_PROCEED,
	CCS_WIND_PROCEED,
	CCS_WACK_PROCEED,
	CCS_WREQ_ALERTING,
	CCS_WIND_ALERTING,
	CCS_WACK_ALERTING,
	CCS_WREQ_PROGRESS,
	CCS_WIND_PROGRESS,
	CCS_WACK_PROGRESS,
	CCS_WREQ_IBI,
	CCS_WIND_IBI,
	CCS_WACK_IBI,
	CCS_WREQ_CONNECT,
	CCS_WIND_CONNECT,
	CCS_WCON_CREQ,
	CCS_WACK_FORWXFER,
	CCS_WCON_SUSREQ,
	CCS_CONNECTED,
	CCS_SUSPENDED,
	CCS_WIND_RELEASE,
	CCS_WCON_RELREQ,
	CCS_WRES_RELIND,
	CCS_UNUSABLE,
};

typedef struct CC_ok_ack {
	cc_ulong cc_primitive;		/* always CC_OK_ACK */
	cc_ulong cc_correct_prim;	/* primitive being acknowledged */
	cc_ulong cc_state;		/* current state */
	cc_ulong cc_call_ref;		/* call reference */
} CC_ok_ack_t;

typedef struct CC_error_ack {
	cc_ulong cc_primitive;		/* always CC_ERROR_ACK */
	cc_ulong cc_error_primitive;	/* primitive in error */
	cc_ulong cc_error_type;		/* CCI error code */
	cc_ulong cc_unix_error;		/* UNIX system error code */
	cc_ulong cc_state;		/* current state */
	cc_ulong cc_call_ref;		/* call reference */
} CC_error_ack_t;

enum {
	CCSYSERR = 0,
	CCOUTSTATE,
	CCBADADDR,
	CCBADDIGS,
	CCBADOPT,
	CCNOADDR,
	CCADDRBUSY,
	CCBADCLR,
	CCBADTOK,
	CCBADFLAG,
	CCNOTSUPP,
	CCBADPRIM,
	CCACCESS,
};

typedef struct CC_info_req {
	cc_ulong cc_primitive;		/* always CC_INFO_REQ */
} CC_info_req_t;

typedef struct CC_info_ack {
	cc_ulong cc_primitive;		/* always CC_INFO_ACK */
	/* FIXME ... more ...  */
} CC_info_ack_t;

typedef struct CC_bind_req {
	cc_ulong cc_primitive;		/* always CC_BIND_REQ */
	cc_ulong cc_addr_length;	/* length of address */
	cc_ulong cc_addr_offset;	/* offset of address */
	cc_ulong cc_setup_ind;		/* req # of setup inds to be queued */
	cc_ulong cc_bind_flags;		/* bind options flags */
} CC_bind_req_t;

/*
   Flags associated with CC_BIND_REQ 
 */
#define CC_DEFAULT_LISTENER		0x000000001UL
#define CC_TOKEN_REQUEST		0x000000002UL
#define CC_MANAGEMENT			0x000000004UL
#define CC_TEST				0x000000008UL
#define CC_MAINTENANCE			0x000000010UL
#define CC_MONITOR			0x000000020UL

typedef struct CC_bind_ack {
	cc_ulong cc_primitive;		/* always CC_BIND_ACK */
	cc_ulong cc_addr_length;	/* length of address */
	cc_ulong cc_addr_offset;	/* offset of address */
	cc_ulong cc_setup_ind;		/* setup indications */
	cc_ulong cc_token_value;	/* setup response token value */
} CC_bind_ack_t;

typedef struct CC_unbind_req {
	cc_ulong cc_primitive;		/* always CC_UNBIND_REQ */
} CC_unbind_req_t;

typedef struct CC_addr_req {
	cc_ulong cc_primitive;		/* always CC_ADDR_REQ */
	cc_ulong cc_call_ref;		/* call reference */
} CC_addr_req_t;

typedef struct CC_addr_ack {
	cc_ulong cc_primitive;		/* always CC_ADDR_ACK */
	cc_ulong cc_bind_length;	/* length of bound address */
	cc_ulong cc_bind_offset;	/* offset of bound address */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_conn_length;	/* length of connected address */
	cc_ulong cc_conn_offset;	/* offset of connected address */
} CC_addr_ack_t;

typedef struct CC_optmgmt_req {
	cc_ulong cc_primitive;		/* always CC_OPTMGMT_REQ */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_opt_length;		/* length of option values */
	cc_ulong cc_opt_offset;		/* offset of option values */
	cc_ulong cc_opt_flags;		/* option flags */
} CC_optmgmt_req_t;

typedef struct CC_optmgmt_ack {
	cc_ulong cc_primitive;		/* always CC_OPTMGMT_ACK */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_opt_length;		/* length of option values */
	cc_ulong cc_opt_offset;		/* offset of option values */
	cc_ulong cc_opt_flags;		/* option flags */
} CC_optmgmt_ack_t;

typedef struct CC_setup_req {
	cc_ulong cc_primitive;		/* always CC_SETUP_REQ */
	cc_ulong cc_user_ref;		/* user call reference */
	cc_ulong cc_call_type;		/* call type */
	cc_ulong cc_call_flags;		/* call flags */
	cc_ulong cc_cdpn_length;	/* called party number length */
	cc_ulong cc_cdpn_offset;	/* called party number offset */
	cc_ulong cc_opt_length;		/* optional parameters length */
	cc_ulong cc_opt_offset;		/* optional parameters offset */
	cc_ulong cc_addr_length;	/* connect to address length */
	cc_ulong cc_addr_offset;	/* connect to address offset */
} CC_setup_req_t;

typedef struct CC_call_reattempt_ind {
	cc_ulong cc_primitive;		/* always CC_CALL_REATTEMPT_IND */
	cc_ulong cc_user_ref;		/* user call reference */
	cc_ulong cc_reason;		/* reason for reattempt */
} CC_call_reattempt_ind_t;

typedef struct CC_setup_ind {
	cc_ulong cc_primitive;		/* always CC_SETUP_IND */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_call_type;		/* call type */
	cc_ulong cc_call_flags;		/* call flags */
	cc_ulong cc_cdpn_length;	/* called party number length */
	cc_ulong cc_cdpn_offset;	/* called party number offset */
	cc_ulong cc_opt_length;		/* optional parameters length */
	cc_ulong cc_opt_offset;		/* optional parameters offset */
	cc_ulong cc_addr_length;	/* connecting address length */
	cc_ulong cc_addr_offset;	/* connecting address offset */
} CC_setup_ind_t;

typedef struct CC_setup_res {
	cc_ulong cc_primitive;		/* always CC_SETUP_RES */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_token_value;	/* call response token value */
} CC_setup_res_t;

typedef struct CC_setup_con {
	cc_ulong cc_primitive;		/* always CC_SETUP_CON */
	cc_ulong cc_user_ref;		/* user call reference */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_addr_length;	/* connecting address length */
	cc_ulong cc_addr_offset;	/* connecting address offset */
} CC_setup_con_t;

typedef struct CC_cont_check_req {
	cc_ulong cc_primitive;		/* always CC_CONT_CHECK_REQ */
	cc_ulong cc_addr_length;	/* adress length */
	cc_ulong cc_addr_offset;	/* adress offset */
} CC_cont_check_req_t;

typedef struct CC_cont_check_ind {
	cc_ulong cc_primitive;		/* always CC_CONT_CHECK_IND */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_addr_length;	/* adress length */
	cc_ulong cc_addr_offset;	/* adress offset */
} CC_cont_check_ind_t;

typedef struct CC_cont_test_req {
	cc_ulong cc_primitive;		/* always CC_CONT_TEST_REQ */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_token_value;	/* token value */
} CC_cont_test_req_t;

typedef struct CC_cont_test_ind {
	cc_ulong cc_primitive;		/* always CC_CONT_TEST_IND */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_addr_length;	/* adress length */
	cc_ulong cc_addr_offset;	/* adress offset */
} CC_cont_test_ind_t;

typedef struct CC_cont_report_req {
	cc_ulong cc_primitive;		/* always CC_CONT_REPORT_REQ */
	cc_ulong cc_user_ref;		/* user call reference */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_result;		/* result of continuity check */
} CC_cont_report_req_t;

typedef struct CC_cont_report_ind {
	cc_ulong cc_primitive;		/* always CC_CONT_REPORT_IND */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_result;		/* result of continuity check */
} CC_cont_report_ind_t;

typedef struct CC_more_info_req {
	cc_ulong cc_primitive;		/* always CC_MORE_INFO_REQ */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_opt_length;		/* optional parameter length */
	cc_ulong cc_opt_offset;		/* optional parameter offset */
} CC_more_info_req_t;

typedef struct CC_more_info_ind {
	cc_ulong cc_primitive;		/* always CC_MORE_INFO_IND */
	cc_ulong cc_user_ref;		/* user call reference */
	cc_ulong cc_opt_length;		/* optional parameter length */
	cc_ulong cc_opt_offset;		/* optional parameter offset */
} CC_more_info_ind_t;

typedef struct CC_information_req {
	cc_ulong cc_primitive;		/* always CC_INFORMATION_REQ */
	cc_ulong cc_user_ref;		/* call reference */
	cc_ulong cc_subn_length;	/* subsequent number length */
	cc_ulong cc_subn_offset;	/* subsequent number offset */
	cc_ulong cc_opt_length;		/* optional parameter length */
	cc_ulong cc_opt_offset;		/* optional parameter offset */
} CC_information_req_t;

typedef struct CC_information_ind {
	cc_ulong cc_primitive;		/* always CC_INFORMATION_IND */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_subn_length;	/* subsequent number length */
	cc_ulong cc_subn_offset;	/* subsequent number offset */
	cc_ulong cc_opt_length;		/* optional parameter length */
	cc_ulong cc_opt_offset;		/* optional parameter offset */
} CC_information_ind_t;

typedef struct CC_info_timeout_ind {
	cc_ulong cc_primitive;		/* always CC_INFO_TIMEOUT_IND */
	cc_ulong cc_call_ref;		/* call reference */
} CC_info_timeout_ind_t;

typedef struct CC_proceeding_req {
	cc_ulong cc_primitive;		/* always CC_PROCEEDING_REQ */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_flags;		/* proceeding flags */
	cc_ulong cc_opt_length;		/* optional parameter length */
	cc_ulong cc_opt_offset;		/* optional parameter offset */
} CC_proceeding_req_t;

typedef struct CC_proceeding_ind {
	cc_ulong cc_primitive;		/* always CC_PROCEEDING_IND */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_flags;		/* proceeding flags */
	cc_ulong cc_opt_length;		/* optional parameter length */
	cc_ulong cc_opt_offset;		/* optional parameter offset */
} CC_proceeding_ind_t;

typedef struct CC_alerting_req {
	cc_ulong cc_primitive;		/* always CC_ALERTING_REQ */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_flags;		/* alerting flags */
	cc_ulong cc_opt_length;		/* optional parameter length */
	cc_ulong cc_opt_offset;		/* optional parameter offset */
} CC_alerting_req_t;

typedef struct CC_alerting_ind {
	cc_ulong cc_primitive;		/* always CC_ALERTING_IND */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_flags;		/* alerting flags */
	cc_ulong cc_opt_length;		/* optional parameter length */
	cc_ulong cc_opt_offset;		/* optional parameter offset */
} CC_alerting_ind_t;

typedef struct CC_progress_req {
	cc_ulong cc_primitive;		/* always CC_PROGRESS_REQ */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_event;		/* progress event */
	cc_ulong cc_flags;		/* progress flags */
	cc_ulong cc_opt_length;		/* optional parameter length */
	cc_ulong cc_opt_offset;		/* optional parameter offset */
} CC_progress_req_t;

typedef struct CC_progress_ind {
	cc_ulong cc_primitive;		/* always CC_PROGRESS_IND */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_event;		/* progress event */
	cc_ulong cc_flags;		/* progress flags */
	cc_ulong cc_opt_length;		/* optional parameter length */
	cc_ulong cc_opt_offset;		/* optional parameter offset */
} CC_progress_ind_t;

typedef struct CC_ibi_req {
	cc_ulong cc_primitive;		/* always CC_IBI_REQ */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_flags;		/* ibi flags */
	cc_ulong cc_opt_length;		/* optional parameter length */
	cc_ulong cc_opt_offset;		/* optional parameter offset */
} CC_ibi_req_t;

typedef struct CC_ibi_ind {
	cc_ulong cc_primitive;		/* always CC_IBI_IND */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_flags;		/* ibi flags */
	cc_ulong cc_opt_length;		/* optional parameter length */
	cc_ulong cc_opt_offset;		/* optional parameter offset */
} CC_ibi_ind_t;

typedef struct CC_connect_req {
	cc_ulong cc_primitive;		/* always CC_CONNECT_REQ */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_flags;		/* connect flags */
	cc_ulong cc_opt_length;		/* optional parameter length */
	cc_ulong cc_opt_offset;		/* optional parameter offset */
} CC_connect_req_t;

typedef struct CC_connect_ind {
	cc_ulong cc_primitive;		/* always CC_CONNECT_IND */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_flags;		/* connect flags */
	cc_ulong cc_opt_length;		/* optional parameter length */
	cc_ulong cc_opt_offset;		/* optional parameter offset */
} CC_connect_ind_t;

typedef struct CC_setup_complete_req {
	cc_ulong cc_primitive;		/* always CC_SETUP_COMPLETE_REQ */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_opt_length;		/* optional parameter length */
	cc_ulong cc_opt_offset;		/* optional parameter offset */
} CC_setup_complete_req_t;

typedef struct CC_setup_complete_ind {
	cc_ulong cc_primitive;		/* always CC_SETUP_COMPLETE_IND */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_opt_length;		/* optional parameter length */
	cc_ulong cc_opt_offset;		/* optional parameter offset */
} CC_setup_complete_ind_t;

typedef struct CC_forwxfer_req {
	cc_ulong cc_primitive;		/* always CC_FORWXFER_REQ */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_opt_length;		/* optional parameter length */
	cc_ulong cc_opt_offset;		/* optional parameter offset */
} CC_forwxfer_req_t;

typedef struct CC_forwxfer_ind {
	cc_ulong cc_primitive;		/* always CC_FORWXFER_IND */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_opt_length;		/* optional parameter length */
	cc_ulong cc_opt_offset;		/* optional parameter offset */
} CC_forwxfer_ind_t;

typedef struct CC_suspend_req {
	cc_ulong cc_primitive;		/* always CC_SUSPEND_REQ */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_flags;		/* suspend flags */
	cc_ulong cc_opt_length;		/* optional parameter length */
	cc_ulong cc_opt_offset;		/* optional parameter offset */
} CC_suspend_req_t;

typedef struct CC_suspend_ind {
	cc_ulong cc_primitive;		/* always CC_SUSPEND_IND */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_flags;		/* suspend flags */
	cc_ulong cc_opt_length;		/* optional parameter length */
	cc_ulong cc_opt_offset;		/* optional parameter offset */
} CC_suspend_ind_t;

typedef struct CC_suspend_res {
	cc_ulong cc_primitive;		/* always CC_SUSPEND_RES */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_opt_length;		/* optional parameter length */
	cc_ulong cc_opt_offset;		/* optional parameter offset */
} CC_suspend_res_t;

typedef struct CC_suspend_con {
	cc_ulong cc_primitive;		/* always CC_SUSPEND_CON */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_opt_length;		/* optional parameter length */
	cc_ulong cc_opt_offset;		/* optional parameter offset */
} CC_suspend_con_t;

typedef struct CC_suspend_reject_req {
	cc_ulong cc_primitive;		/* always CC_SUSPEND_REJECT_REQ */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_cause;		/* cause value */
	cc_ulong cc_opt_length;		/* optional parameter length */
	cc_ulong cc_opt_offset;		/* optional parameter offset */
} CC_suspend_reject_req_t;

typedef struct CC_suspend_reject_ind {
	cc_ulong cc_primitive;		/* always CC_SUSPEND_REJECT_IND */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_cause;		/* cause value */
	cc_ulong cc_opt_length;		/* optional parameter length */
	cc_ulong cc_opt_offset;		/* optional parameter offset */
} CC_suspend_reject_ind_t;

typedef struct CC_resume_req {
	cc_ulong cc_primitive;		/* always CC_RESUME_REQ */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_flags;		/* suspend flags */
	cc_ulong cc_opt_length;		/* optional parameter length */
	cc_ulong cc_opt_offset;		/* optional parameter offset */
} CC_resume_req_t;

typedef struct CC_resume_ind {
	cc_ulong cc_primitive;		/* always CC_RESUME_IND */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_flags;		/* suspend flags */
	cc_ulong cc_opt_length;		/* optional parameter length */
	cc_ulong cc_opt_offset;		/* optional parameter offset */
} CC_resume_ind_t;

typedef struct CC_resume_res {
	cc_ulong cc_primitive;		/* always CC_RESUME_RES */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_opt_length;		/* optional parameter length */
	cc_ulong cc_opt_offset;		/* optional parameter offset */
} CC_resume_res_t;

typedef struct CC_resume_con {
	cc_ulong cc_primitive;		/* always CC_RESUME_CON */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_opt_length;		/* optional parameter length */
	cc_ulong cc_opt_offset;		/* optional parameter offset */
} CC_resume_con_t;

typedef struct CC_resume_reject_req {
	cc_ulong cc_primitive;		/* always CC_RESUME_REJECT_REQ */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_cause;		/* cause value */
	cc_ulong cc_opt_length;		/* optional parameter length */
	cc_ulong cc_opt_offset;		/* optional parameter offset */
} CC_resume_reject_req_t;

typedef struct CC_resume_reject_ind {
	cc_ulong cc_primitive;		/* always CC_RESUME_REJECT_IND */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_cause;		/* cause value */
	cc_ulong cc_opt_length;		/* optional parameter length */
	cc_ulong cc_opt_offset;		/* optional parameter offset */
} CC_resume_reject_ind_t;

typedef struct CC_reject_req {
	cc_ulong cc_primitive;		/* always CC_REJECT_REQ */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_cause;		/* cause value */
	cc_ulong cc_opt_length;		/* optional parameter length */
	cc_ulong cc_opt_offset;		/* optional parameter offset */
} CC_reject_req_t;

typedef struct CC_reject_ind {
	cc_ulong cc_primitive;		/* always CC_REJECT_IND */
	cc_ulong cc_user_ref;		/* user call reference */
	cc_ulong cc_cause;		/* cause value */
	cc_ulong cc_opt_length;		/* optional parameter length */
	cc_ulong cc_opt_offset;		/* optional parameter offset */
} CC_reject_ind_t;

typedef struct CC_error_ind {
	cc_ulong cc_primitive;		/* always CC_ERROR_IND */
	cc_ulong cc_call_ref;		/* call reference */
} CC_error_ind_t;

typedef struct CC_call_failure_ind {
	cc_ulong cc_primitive;		/* always CC_CALL_FAILURE_IND */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_reason;		/* reason for failure */
	cc_ulong cc_cause;		/* cause to use in release */
} CC_call_failure_ind_t;

typedef struct CC_disconnect_req {
	cc_ulong cc_primitive;		/* always CC_DISCONNECT_REQ */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_cause;		/* cause value */
	cc_ulong cc_opt_length;		/* optional parameter length */
	cc_ulong cc_opt_offset;		/* optional parameter offset */
} CC_disconnect_req_t;

typedef struct CC_disconnect_ind {
	cc_ulong cc_primitive;		/* always CC_DISCONNECT_IND */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_cause;		/* cause value */
	cc_ulong cc_opt_length;		/* optional parameter length */
	cc_ulong cc_opt_offset;		/* optional parameter offset */
} CC_disconnect_ind_t;

typedef struct CC_release_req {
	cc_ulong cc_primitive;		/* always CC_RELEASE_REQ */
	cc_ulong cc_user_ref;		/* user call reference */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_cause;		/* cause value */
	cc_ulong cc_opt_length;		/* optional parameter length */
	cc_ulong cc_opt_offset;		/* optional parameter offset */
} CC_release_req_t;

typedef struct CC_release_ind {
	cc_ulong cc_primitive;		/* always CC_RELEASE_IND */
	cc_ulong cc_user_ref;		/* user call reference */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_cause;		/* cause value */
	cc_ulong cc_opt_length;		/* optional parameter length */
	cc_ulong cc_opt_offset;		/* optional parameter offset */
} CC_release_ind_t;

typedef struct CC_release_res {
	cc_ulong cc_primitive;		/* always CC_RELEASE_RES */
	cc_ulong cc_user_ref;		/* user call reference */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_opt_length;		/* optional parameter length */
	cc_ulong cc_opt_offset;		/* optional parameter offset */
} CC_release_res_t;

typedef struct CC_release_con {
	cc_ulong cc_primitive;		/* always CC_RELEASE_CON */
	cc_ulong cc_user_ref;		/* user call reference */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_opt_length;		/* optional parameter length */
	cc_ulong cc_opt_offset;		/* optional parameter offset */
} CC_release_con_t;

typedef struct CC_notify_req {
	cc_ulong cc_primitive;		/* always CC_NOTIFY_REQ */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_opt_length;		/* optional parameter length */
	cc_ulong cc_opt_offset;		/* optional parameter offset */
} CC_notify_req_t;

typedef struct CC_notify_ind {
	cc_ulong cc_primitive;		/* always CC_NOTIFY_IND */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_opt_length;		/* optional parameter length */
	cc_ulong cc_opt_offset;		/* optional parameter offset */
} CC_notify_ind_t;

typedef struct CC_restart_req {
	cc_ulong cc_primitive;		/* always CC_RESTART_REQ */
	cc_ulong cc_flags;		/* restart flags */
	cc_ulong cc_addr_length;	/* adddress length */
	cc_ulong cc_addr_offset;	/* adddress offset */
} CC_restart_req_t;

typedef struct CC_restart_con {
	cc_ulong cc_primitive;		/* always CC_RESTART_CON */
	cc_ulong cc_flags;		/* restart flags */
	cc_ulong cc_addr_length;	/* adddress length */
	cc_ulong cc_addr_offset;	/* adddress offset */
} CC_restart_con_t;

typedef struct CC_status_ind {
	cc_ulong cc_primitive;		/* always CC_STATUS_IND */
	cc_ulong cc_flags;		/* status flags */
	cc_ulong cc_addr_length;	/* adddress length */
	cc_ulong cc_addr_offset;	/* adddress offset */
} CC_status_ind_t;

typedef struct CC_datalink_failure_ind {
	cc_ulong cc_primitive;		/* always CC_DATALINK_FAILURE_IND */
	cc_ulong cc_user_ref;		/* user call reference */
	cc_ulong cc_call_ref;		/* call reference */
} CC_datalink_failure_ind_t;

typedef struct CC_reset_req {
	cc_ulong cc_primitive;		/* always CC_RESET_REQ */
	cc_ulong cc_flags;		/* reset flags */
	cc_ulong cc_addr_length;	/* address length */
	cc_ulong cc_addr_offset;	/* address offset */
} CC_reset_req_t;

typedef struct CC_reset_ind {
	cc_ulong cc_primitive;		/* always CC_RESET_IND */
	cc_ulong cc_flags;		/* reset flags */
	cc_ulong cc_addr_length;	/* address length */
	cc_ulong cc_addr_offset;	/* address offset */
} CC_reset_ind_t;

typedef struct CC_reset_res {
	cc_ulong cc_primitive;		/* always CC_RESET_RES */
	cc_ulong cc_flags;		/* reset flags */
	cc_ulong cc_addr_length;	/* address length */
	cc_ulong cc_addr_offset;	/* address offset */
} CC_reset_res_t;

typedef struct CC_reset_con {
	cc_ulong cc_primitive;		/* always CC_RESET_CON */
	cc_ulong cc_flags;		/* reset flags */
	cc_ulong cc_addr_length;	/* address length */
	cc_ulong cc_addr_offset;	/* address offset */
} CC_reset_con_t;

typedef struct CC_blocking_req {
	cc_ulong cc_primitive;		/* always CC_BLOCKING_REQ */
	cc_ulong cc_flags;		/* blocking flags */
	cc_ulong cc_addr_length;	/* address length */
	cc_ulong cc_addr_offset;	/* address offset */
} CC_blocking_req_t;

typedef struct CC_blocking_ind {
	cc_ulong cc_primitive;		/* always CC_BLOCKING_IND */
	cc_ulong cc_flags;		/* blocking flags */
	cc_ulong cc_addr_length;	/* address length */
	cc_ulong cc_addr_offset;	/* address offset */
} CC_blocking_ind_t;

typedef struct CC_blocking_res {
	cc_ulong cc_primitive;		/* always CC_BLOCKING_RES */
	cc_ulong cc_flags;		/* blocking flags */
	cc_ulong cc_addr_length;	/* address length */
	cc_ulong cc_addr_offset;	/* address offset */
} CC_blocking_res_t;

typedef struct CC_blocking_con {
	cc_ulong cc_primitive;		/* always CC_BLOCKING_CON */
	cc_ulong cc_flags;		/* blocking flags */
	cc_ulong cc_addr_length;	/* address length */
	cc_ulong cc_addr_offset;	/* address offset */
} CC_blocking_con_t;

typedef struct CC_unblocking_req {
	cc_ulong cc_primitive;		/* always CC_UNBLOCKING_REQ */
	cc_ulong cc_flags;		/* unblocking flags */
	cc_ulong cc_addr_length;	/* address length */
	cc_ulong cc_addr_offset;	/* address offset */
} CC_unblocking_req_t;

typedef struct CC_unblocking_ind {
	cc_ulong cc_primitive;		/* always CC_UNBLOCKING_IND */
	cc_ulong cc_flags;		/* unblocking flags */
	cc_ulong cc_addr_length;	/* address length */
	cc_ulong cc_addr_offset;	/* address offset */
} CC_unblocking_ind_t;

typedef struct CC_unblocking_res {
	cc_ulong cc_primitive;		/* always CC_UNBLOCKING_RES */
	cc_ulong cc_flags;		/* blocking flags */
	cc_ulong cc_addr_length;	/* address length */
	cc_ulong cc_addr_offset;	/* address offset */
} CC_unblocking_res_t;

typedef struct CC_unblocking_con {
	cc_ulong cc_primitive;		/* always CC_UNBLOCKING_CON */
	cc_ulong cc_flags;		/* unblocking flags */
	cc_ulong cc_addr_length;	/* address length */
	cc_ulong cc_addr_offset;	/* address offset */
} CC_unblocking_con_t;

typedef struct CC_query_req {
	cc_ulong cc_primitive;		/* always CC_QUERY_REQ */
	cc_ulong cc_flags;		/* query flags */
	cc_ulong cc_addr_length;	/* address length */
	cc_ulong cc_addr_offset;	/* address offset */
} CC_query_req_t;

typedef struct CC_query_ind {
	cc_ulong cc_primitive;		/* always CC_QUERY_IND */
	cc_ulong cc_flags;		/* query flags */
	cc_ulong cc_addr_length;	/* address length */
	cc_ulong cc_addr_offset;	/* address offset */
} CC_query_ind_t;

typedef struct CC_query_res {
	cc_ulong cc_primitive;		/* always CC_QUERY_RES */
	cc_ulong cc_flags;		/* blocking flags */
	cc_ulong cc_addr_length;	/* address length */
	cc_ulong cc_addr_offset;	/* address offset */
} CC_query_res_t;

typedef struct CC_query_con {
	cc_ulong cc_primitive;		/* always CC_QUERY_CON */
	cc_ulong cc_flags;		/* query flags */
	cc_ulong cc_addr_length;	/* address length */
	cc_ulong cc_addr_offset;	/* address offset */
} CC_query_con_t;

typedef struct CC_maint_ind {
	cc_ulong cc_primitive;		/* always CC_MAINT_IND */
	cc_ulong cc_reason;		/* reason for indication */
	cc_ulong cc_call_ref;		/* call reference */
	cc_ulong cc_addr_length;	/* length of address */
	cc_ulong cc_addr_offset;	/* length of address */
} CC_maint_ind_t;

union CC_primitives {
	cc_ulong cc_primitive;
	CC_ok_ack_t ok_ack;
	CC_error_ack_t error_ack;
	CC_info_req_t info_req;
	CC_info_ack_t info_ack;
	CC_bind_req_t bind_req;
	CC_bind_ack_t bind_ack;
	CC_unbind_req_t unbind_req;
	CC_addr_req_t addr_req;
	CC_addr_ack_t addr_ack;
	CC_optmgmt_req_t optmgmt_req;
	CC_optmgmt_ack_t optmgmt_ack;
	CC_setup_req_t setup_req;
	CC_call_reattempt_ind_t call_reattempt_ind;
	CC_setup_ind_t setup_ind;
	CC_setup_res_t setup_res;
	CC_setup_con_t setup_con;
	CC_cont_check_req_t cont_check_req;
	CC_cont_check_ind_t cont_check_ind;
	CC_cont_test_req_t cont_test_req;
	CC_cont_test_ind_t cont_test_ind;
	CC_cont_report_req_t cont_report_req;
	CC_cont_report_ind_t cont_report_ind;
	CC_more_info_req_t more_info_req;
	CC_more_info_ind_t more_info_ind;
	CC_information_req_t information_req;
	CC_information_ind_t information_ind;
	CC_proceeding_req_t proceeding_req;
	CC_proceeding_ind_t proceeding_ind;
	CC_alerting_req_t alerting_req;
	CC_alerting_ind_t alerting_ind;
	CC_progress_req_t progress_req;
	CC_progress_ind_t progress_ind;
	CC_ibi_req_t ibi_req;
	CC_ibi_ind_t ibi_ind;
	CC_connect_req_t connect_req;
	CC_connect_ind_t connect_ind;
	CC_setup_complete_req_t setup_complete_req;
	CC_setup_complete_ind_t setup_complete_ind;
	CC_forwxfer_req_t forwxfer_req;
	CC_forwxfer_ind_t forwxfer_ind;
	CC_suspend_req_t suspend_req;
	CC_suspend_ind_t suspend_ind;
	CC_suspend_res_t suspend_res;
	CC_suspend_con_t suspend_con;
	CC_suspend_reject_req_t suspend_reject_req;
	CC_suspend_reject_ind_t suspend_reject_ind;
	CC_resume_req_t resume_req;
	CC_resume_ind_t resume_ind;
	CC_resume_res_t resume_res;
	CC_resume_con_t resume_con;
	CC_resume_reject_req_t resume_reject_req;
	CC_resume_reject_ind_t resume_reject_ind;
	CC_reject_req_t reject_req;
	CC_reject_ind_t reject_ind;
	CC_error_ind_t error_ind;
	CC_call_failure_ind_t call_failure_ind;
	CC_disconnect_req_t disconnect_req;
	CC_disconnect_ind_t disconnect_ind;
	CC_release_req_t release_req;
	CC_release_ind_t release_ind;
	CC_release_res_t release_res;
	CC_release_con_t release_con;
	CC_restart_req_t restart_req;
	CC_restart_con_t restart_con;
	CC_status_ind_t status_ind;
	CC_datalink_failure_ind_t datalink_failure_ind;
	CC_reset_req_t reset_req;
	CC_reset_ind_t reset_ind;
	CC_reset_res_t reset_res;
	CC_reset_con_t reset_con;
	CC_blocking_req_t blocking_req;
	CC_blocking_ind_t blocking_ind;
	CC_blocking_res_t blocking_res;
	CC_blocking_con_t blocking_con;
	CC_unblocking_req_t unblocking_req;
	CC_unblocking_ind_t unblocking_ind;
	CC_unblocking_res_t unblocking_res;
	CC_unblocking_con_t unblocking_con;
	CC_query_req_t query_req;
	CC_query_ind_t query_ind;
	CC_query_res_t query_res;
	CC_query_con_t query_con;
	CC_maint_ind_t maint_ind;
};

#endif				/* __CCI_H__ */
