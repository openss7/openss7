/*****************************************************************************

 @(#) $Id: sl.h,v 0.9 2004/01/17 08:08:12 brian Exp $

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

#ifndef __SS7_SL_H__
#define __SS7_SL_H__

#ident "@(#) $Name:  $($Revision: 0.9 $) Copyright (c) 1997-2002 OpenSS7 Corporation."

#define SL_INFO_REQ			   1	/* Information request */
#define SL_ATTACH_REQ			   2	/* Attach request */
#define SL_DETACH_REQ			   3	/* Detach request */
#define SL_ENABLE_REQ			   4	/* Enable request */
#define SL_DISABLE_REQ			   5	/* Disable request */
#define SL_OPTMGMT_REQ			   6	/* Options mangement request */
#define SL_NOTIFY_REQ			   7	/* Notifications request */
#define SL_PDU_REQ			   8	/* Message transfer request */
#define SL_EMERGENCY_REQ		   9	/* Emergency begins */
#define SL_EMERGENCY_CEASES_REQ		  10	/* Emergency ceases */
#define SL_START_REQ			  11	/* Start */
#define SL_STOP_REQ			  12	/* Stop */
#define SL_RETRIEVE_BSNT_REQ		  13	/* Retrieve BSNT value */
#define SL_RETRIEVAL_REQUEST_AND_FSNC_REQ 14	/* Retrieve messages from FSNC */
#define SL_RESUME_REQ			  15	/* Resume request */
#define SL_CLEAR_BUFFERS_REQ		  16	/* Clear buffers */
#define SL_CLEAR_RTB_REQ		  17	/* Clear retransmit buffer */
#define SL_LOCAL_PROCESSOR_OUTAGE_REQ	  18	/* local processor outage begins */
#define SL_CONGESTION_DISCARD_REQ	  19	/* receive congestion discard */
#define SL_CONGESTION_ACCEPT_REQ	  20	/* receive congestion accept */
#define SL_NO_CONGESTION_REQ		  21	/* receive congestion ceased */
#define SL_POWER_ON_REQ			  22	/* power on reset */

#define SL_INFO_ACK			  25	/* Information acknowledgement */
#define SL_OK_ACK			  26	/* Success acknowledgement */
#define SL_ERROR_ACK			  27	/* Error acknowledgement */
#define SL_ENABLE_CON			  28	/* Enable confirmation */
#define SL_DISABLE_CON			  29	/* Disable confirmation */
#define SL_OPTMGMT_ACK			  30	/* Options management reply */
#define SL_NOTIFY_IND			  31	/* Notification indications */
#define SL_ERROR_IND			  32	/* Error indication */
#define SL_STATS_IND			  33	/* Statistics report */
#define SL_EVENT_IND			  34	/* Event indication */
#define SL_PDU_IND			  35	/* Received message indication */
#define SL_LINK_CONGESTED_IND		  36	/* Link congested */
#define SL_LINK_CONGESTION_CEASED_IND	  37	/* Link cnogestion ceased */
#define SL_RETRIEVED_MESSAGE_IND	  38	/* Retrieved message indication */
#define SL_RETRIEVAL_COMPLETED_IND	  39	/* Retrieval confirmation */
#define SL_RB_CLEARED_IND		  40	/* Receive buffer cleared ind */
#define SL_BSNT_IND			  41	/* Retrieved BSNT indication */
#define SL_IN_SERVICE_IND		  42	/* In service indication */
#define SL_OUT_OF_SERVICE_IND		  43	/* Out of service indication */
#define SL_REMOTE_PROCESSOR_OUTAGE_IND	  44	/* Remote processor out */
#define SL_REMOTE_PROCESSOR_RECOVERED_IND 45	/* Remote processor recovered */
#define SL_RTB_CLEARED_IND		  46	/* Retransmit buffer cleared ind */
#define SL_RETRIEVAL_NOT_POSSIBLE_IND	  47	/* Retrieval not possible ind */
#define SL_BSNT_NOT_RETRIEVABLE_IND	  48	/* BSNT not retrievable ind */

/*
 *  States
 */
#define SL_UNATTACHED		 0
#define SL_ATTACH_PENDING	 1
#define SL_DISABLED		 2
#define SL_ENABLE_PENDING	 3
#define SL_ENABLED		 4
#define SL_STARTING		 5
#define SL_IN_SERVICE		 6
#define SL_PROCESSOR_OUTAGE	 7
#define SL_OUT_OF_SERVICE	 8
#define SL_RETRIEVING_MSUS	 9
#define SL_DISABLE_PENDING	10
#define SL_DETACH_PENDING	11
#define SL_UNUSABLE		12

#define SL_BADADDRESS   0x01	/* Address was invalid */
#define SL_BADADDRTYPE  0x02	/* Invalid address type */
#define SL_BADDIAL      0x03	/* Dial information was invalid */
#define SL_BADDIALTYPE  0x04	/* Invalid dial information type */
#define SL_BADDISPOSAL  0x05	/* Invalid disposal parameter */
#define SL_BADFRAME     0x06	/* Defective SDU received */
#define SL_BADPPA       0x07	/* Invalid PPA identifier */
#define SL_BADPRIM      0x08	/* Unrecognized primitive */
#define SL_DISC 	0x09	/* Disconnected */
#define SL_EVENT        0x0a	/* Protocol-specific event occurred */
#define SL_FATALERR     0x0b	/* Device has become unusable */
#define SL_INITFAILED   0x0c	/* Line initialization failed */
#define SL_NOTSUPP      0x0d	/* Primitive not supported by this device */
#define SL_OUTSTATE     0x0e	/* Primitive was issued from an invalid state */
#define SL_PROTOSHORT   0x0f	/* M_PROTO block too short */
#define SL_READTIMEOUT  0x10	/* Read request timed out before data arrived */
#define SL_SYSERR       0x11	/* UNIX system error */
#define SL_WRITEFAIL    0x12	/* Unitdata request failed */

/*
 *  SL_INFO_REQ                        1 - Information request
 */
typedef struct {
	ulong sl_primitive;		/* always SL_INFO_REQ */
} sl_info_req_t;

/*
 *  SL_ATTACH_REQ                      2 - Attach request
 */
typedef struct {
	ulong sl_primitive;		/* always SL_ATTACH_REQ */
	/*
	   followed by ppa 
	 */
} sl_attach_req_t;

/*
 *  SL_DETACH_REQ                      3 - Detach request
 */
typedef struct {
	ulong sl_primitive;		/* always SL_DETACH_REQ */
} sl_detach_req_t;

/*
 *  SL_ENABLE_REQ                      4 - Enable request
 */
typedef struct {
	ulong sl_primitive;		/* always SL_ENABLE_REQ */
} sl_enable_req_t;

/*
 *  SL_DISABLE_REQ                     5 - Disable request
 */
typedef struct {
	ulong sl_primitive;		/* always SL_DISABLE_REQ */
} sl_disable_req_t;

/*
 *  SL_OPTMGMT_REQ                     6 - Options mangement request
 */
typedef struct {
	ulong sl_primitive;		/* always SL_OPTMGMT_REQ */
	ulong sl_opt_length;		/* options length */
	ulong sl_opt_offset;		/* options offset */
	ulong sl_mgmt_flags;		/* management flags */
} sl_optmgmt_req_t;

/*
 *  SL_NOTIFY_REQ                      7 - Notifications request
 */
typedef struct {
	ulong sl_primitive;		/* always SL_NOTIFY_REQ */
} sl_notify_req_t;

/*
 *  SL_PDU_REQ                         8 - Message transfer request
 */
typedef struct {
	ulong sl_primitive;		/* always SL_PDU_REQ */
} sl_pdu_req_t;

/*
 *  SL_EMERGENCY_REQ                   9 - Emergency begins
 */
typedef struct {
	ulong sl_primitive;		/* always SL_EMERGENCY_REQ */
} sl_emergency_req_t;

/*
 *  SL_EMERGENCY_CEASES_REQ           10 - Emergency ceases
 */
typedef struct {
	ulong sl_primitive;		/* always SL_EMERGENCY_CEASES_REQ */
} sl_emergency_ceases_req_t;

/*
 *  SL_START_REQ                      11 - Start
 */
typedef struct {
	ulong sl_primitive;		/* always SL_START_REQ */
} sl_start_req_t;

/*
 *  SL_STOP_REQ                       12 - Stop
 */
typedef struct {
	ulong sl_primitive;		/* always SL_STOP_REQ */
} sl_stop_req_t;

/*
 *  SL_RETRIEVE_BSNT_REQ              13 - Retrieve BSNT value
 */
typedef struct {
	ulong sl_primitive;		/* always SL_RETRIEVE_BSNT_REQ */
} sl_retrieve_bsnt_req_t;

/*
 *  SL_RETRIEVAL_REQUEST_AND_FSNC_REQ 14 - Retrieve messages from FSNC
 */
typedef struct {
	ulong sl_primitive;		/* always SL_RETRIEVAL_REQUEST_AND_FSNC_REQ */
	ulong sl_fsnc;			/* FSNC value */
} sl_retrieval_request_and_fsnc_req_t;

/*
 *  SL_RESUME_REQ                     15 - Resume request
 */
typedef struct {
	ulong sl_primitive;		/* always SL_RESUME_REQ */
} sl_resume_req_t;

/*
 *  SL_CLEAR_BUFFERS_REQ              16 - Clear buffers
 */
typedef struct {
	ulong sl_primitive;		/* always SL_CLEAR_BUFFERS_REQ */
} sl_clear_buffers_req_t;

/*
 *  SL_CLEAR_RTB_REQ                  17 - Clear retransmit buffer
 */
typedef struct {
	ulong sl_primitive;		/* always SL_CLEAR_RTB_REQ */
} sl_clear_rtb_req_t;

/*
 *  SL_LOCAL_PROCESSOR_OUTAGE_REQ     18 - local processor outage begins
 */
typedef struct {
	ulong sl_primitive;		/* always SL_LOCAL_PROCESSOR_OUTAGE_REQ */
} sl_local_processor_outage_req_t;

/*
 *  SL_CONGESTION_DISCARD_REQ         19 - receive congestion discard
 */
typedef struct {
	ulong sl_primitive;		/* always SL_CONGESTION_DISCARD_REQ */
} sl_congestion_discard_req_t;

/*
 *  SL_CONGESTION_ACCEPT_REQ          20 - receive congestion accept
 */
typedef struct {
	ulong sl_primitive;		/* always SL_CONGESTION_ACCEPT_REQ */
} sl_congestion_accept_req_t;

/*
 *  SL_NO_CONGESTION_REQ              21 - receive congestion ceased
 */
typedef struct {
	ulong sl_primitive;		/* always SL_NO_CONGESTION_REQ */
} sl_no_congestion_req_t;

/*
 *  SL_POWER_ON_REQ                   22 - power on reset
 */
typedef struct {
	ulong sl_primitive;		/* always SL_POWER_ON_REQ */
} sl_power_on_req_t;

/*
 *  SL_INFO_ACK                       25 - Information acknowledgement
 */
typedef struct {
	ulong sl_primitive;		/* always SL_INFO_ACK */
	ulong sl_version;		/* version */
	ulong sl_state;			/* interface state */
	ulong sl_max_sdu;		/* maximum sdu size */
	ulong sl_min_sdu;		/* minimum sdu size */
	ulong sl_header_len;		/* header length to reserve */
	ulong sl_ppa_style;		/* style of PPA address */
	/*
	   followed by PPA address 
	 */
} sl_info_ack_t;

/*
 *  SL_OK_ACK                         26 - Success acknowledgement
 */
typedef struct {
	ulong sl_primitive;		/* always SL_OK_ACK */
	ulong sl_correct_primitive;	/* correct primitive */
} sl_ok_ack_t;

/*
 *  SL_ERROR_ACK                      27 - Error acknowledgement
 */
typedef struct {
	ulong sl_primitive;		/* always SL_ERROR_ACK */
	ulong sl_error_primitive;	/* primitive in error */
	ulong sl_errno;			/* error type */
	ulong sl_unix_error;		/* unix error code */
} sl_error_ack_t;

/*
 *  SL_ENABLE_CON                     28 - Enable confirmation
 */
typedef struct {
	ulong sl_primitive;		/* always SL_ENABLE_CON */
} sl_enable_con_t;

/*
 *  SL_DISABLE_CON                    29 - Disable confirmation
 */
typedef struct {
	ulong sl_primitive;		/* always SL_DISABLE_CON */
} sl_disable_con_t;

/*
 *  SL_OPTMGMT_ACK                    30 - Options management reply
 */
typedef struct {
	ulong sl_primitive;		/* always SL_OPTMGMT_ACK */
	ulong sl_opt_length;		/* options length */
	ulong sl_opt_offset;		/* options offset */
	ulong sl_mgmt_flags;		/* management flags */
} sl_optmgmt_ack_t;

/*
 *  SL_NOTIFY_IND                     31 - Notification indications
 */
typedef struct {
	ulong sl_primitive;		/* always SL_NOTIFY_IND */
	ulong sl_objectid;		/* event object id */
	ulong sl_timestamp;		/* event timestamp */
	ulong sl_severity;		/* event severity */
} sl_notify_ind_t;

/*
 *  SL_ERROR_IND                      32 - Error indication
 */
typedef struct {
	ulong sl_primitive;		/* always SL_ERROR_IND */
} sl_error_ind_t;

/*
 *  SL_STATS_IND                      33 - Statistics report
 */
typedef struct {
	ulong sl_primitive;		/* always SL_STATS_IND */
} sl_stats_ind_t;

/*
 *  SL_EVENT_IND                      34 - Event indication
 */
typedef struct {
	ulong sl_primitive;		/* always SL_EVENT_IND */
} sl_event_ind_t;

/*
 *  SL_PDU_IND                        35 - Received message indication
 */
typedef struct {
	ulong sl_primitive;		/* always SL_PDU_IND */
} sl_pdu_ind_t;

/*
 *  SL_LINK_CONGESTED_IND             36 - Link congested
 */
typedef struct {
	ulong sl_primitive;		/* always SL_LINK_CONGESTED_IND */
	ulong sl_cong_status;		/* congestion status */
	ulong sl_disc_status;		/* discard status */
} sl_link_congested_ind_t;

/*
 *  SL_LINK_CONGESTION_CEASED_IND     37 - Link cnogestion ceased
 */
typedef struct {
	ulong sl_primitive;		/* always SL_LINK_CONGESTION_CEASED_IND */
	ulong sl_cong_status;		/* congestion status */
	ulong sl_disc_status;		/* discard status */
} sl_link_congestion_ceased_ind_t;

/*
 *  SL_RETRIEVED_MESSAGE_IND          38 - Retrieved message indication
 */
typedef struct {
	ulong sl_primitive;		/* always SL_RETRIEVED_MESSAGE_IND */
} sl_retrieved_message_ind_t;

/*
 *  SL_RETRIEVAL_COMPLETED_IND        39 - Retrieval confirmation
 */
typedef struct {
	ulong sl_primitive;		/* always SL_RETRIEVAL_COMPLETED_IND */
} sl_retrieval_completed_ind_t;

/*
 *  SL_RB_CLEARED_IND                 40 - Receive buffer cleared ind
 */
typedef struct {
	ulong sl_primitive;		/* always SL_RB_CLEARED_IND */
} sl_rb_cleared_ind_t;

/*
 *  SL_BSNT_IND                       41 - Retrieved BSNT indication
 */
typedef struct {
	ulong sl_primitive;		/* always SL_BSNT_IND */
	ulong sl_bsnt;			/* returned BSNT value */
} sl_bsnt_ind_t;

/*
 *  SL_IN_SERVICE_IND                 42 - In service indication
 */
typedef struct {
	ulong sl_primitive;		/* always SL_IN_SERVICE_IND */
} sl_in_service_ind_t;

/*
 *  SL_OUT_OF_SERVICE_IND             43 - Out of service indication
 */
typedef struct {
	ulong sl_primitive;		/* always SL_OUT_OF_SERVICE_IND */
	ulong sl_reason;		/* reason for link failure */
} sl_out_of_service_ind_t;

/*
 *  SL_REMOTE_PROCESSOR_OUTAGE_IND    44 - Remote processor out
 */
typedef struct {
	ulong sl_primitive;		/* always SL_REMOTE_PROCESSOR_OUTAGE_IND */
} sl_remote_processor_outage_ind_t;

/*
 *  SL_REMOTE_PROCESSOR_RECOVERED_IND 45 - Remote processor recovered
 */
typedef struct {
	ulong sl_primitive;		/* always SL_REMOTE_PROCESSOR_RECOVERED_IND */
} sl_remote_processor_recovered_ind_t;

/*
 *  SL_RTB_CLEARED_IND                46 - Retransmit buffer cleared ind
 */
typedef struct {
	ulong sl_primitive;		/* always SL_RTB_CLEARED_IND */
} sl_rtb_cleared_ind_t;

/*
 *  SL_RETRIEVAL_NOT_POSSIBLE_IND     47 - Retrieval not possible ind
 */
typedef struct {
	ulong sl_primitive;		/* always SL_RETRIEVAL_NOT_POSSIBLE_IND */
} sl_retrieval_not_possible_ind_t;

/*
 *  SL_BSNT_NOT_RETRIEVABLE_IND       48 - BSNT not retrievable ind
 */
typedef struct {
	ulong sl_primitive;		/* always SL_BSNT_NOT_RETRIEVABLE_IND */
} sl_bsnt_not_retrievable_ind_t;

union SL_primitives {
	ulong sl_primitive;
	sl_info_req_t info_req;
	sl_attach_req_t attach_req;
	sl_detach_req_t detach_req;
	sl_enable_req_t enable_req;
	sl_disable_req_t disable_req;
	sl_optmgmt_req_t optmgmt_req;
	sl_notify_req_t notify_req;
	sl_pdu_req_t pdu_req;
	sl_emergency_req_t emergency_req;
	sl_emergency_ceases_req_t emergency_ceases_req;
	sl_start_req_t start_req;
	sl_stop_req_t stop_req;
	sl_retrieve_bsnt_req_t retrieve_bsnt_req;
	sl_retrieval_request_and_fsnc_req_t retrieval_request_and_fsnc_req;
	sl_resume_req_t resume_req;
	sl_clear_buffers_req_t clear_buffers_req;
	sl_clear_rtb_req_t clear_rtb_req;
	sl_local_processor_outage_req_t local_processor_outage_req;
	sl_congestion_discard_req_t congestion_discard_req;
	sl_congestion_accept_req_t congestion_accept_req;
	sl_no_congestion_req_t no_congestion_req;
	sl_power_on_req_t info_ack;
	sl_ok_ack_t ok_ack;
	sl_error_ack_t error_ack;
	sl_enable_con_t enable_con;
	sl_disable_con_t disable_con;
	sl_optmgmt_ack_t optmgmt_ack;
	sl_notify_ind_t notify_ind;
	sl_error_ind_t error_ind;
	sl_stats_ind_t stats_ind;
	sl_event_ind_t event_ind;
	sl_pdu_ind_t pdu_ind;
	sl_link_congested_ind_t link_congested_ind;
	sl_link_congestion_ceased_ind_t link_congestion_ceased_ind;
	sl_retrieved_message_ind_t retrieved_message_ind;
	sl_retrieval_completed_ind_t retrieval_completed_ind;
	sl_rb_cleared_ind_t rb_cleared_ind;
	sl_bsnt_ind_t bsnt_ind;
	sl_in_service_ind_t in_service_ind;
	sl_out_of_service_ind_t out_of_service_ind;
	sl_remote_processor_outage_ind_t remote_processor_outage_ind;
	sl_remote_processor_recovered_ind_t remote_processor_recovered_ind;
	sl_rtb_cleared_ind_t rtb_cleared_ind;
	sl_retrieval_not_possible_ind_t retrieval_not_possible_ind;
	sl_bsnt_not_retrievable_ind_t bsnt_not_retrievable_ind;
};

#endif				/* __SS7_SL_H__ */
