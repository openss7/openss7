/*****************************************************************************

 @(#) $Id: isupi.h,v 0.9.2.4 2006/10/31 21:04:37 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
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

 Last Modified $Date: 2006/10/31 21:04:37 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: isupi.h,v $
 Revision 0.9.2.4  2006/10/31 21:04:37  brian
 - changes for 32-bit compatibility and remove HZ dependency

 *****************************************************************************/

#ifndef __SS7_ISUPI_H__
#define __SS7_ISUPI_H__

#ident "@(#) $RCSfile: isupi.h,v $ $Name:  $($Revision: 0.9.2.4 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

/* This file can be processed by doxygen(1). */

/*
 *  ISUP addresss
 */

typedef struct isup_addr {
	cc_ulong scope;			/* the scope of the identifier */
	cc_ulong id;			/* the identifier within the scope */
	cc_ulong cic;			/* circuit identification code within the scope */
} isup_addr_t;

#define ISUP_SCOPE_CT		1	/* circuit scope */
#define ISUP_SCOPE_CG		2	/* circuit group scope */
#define ISUP_SCOPE_TG		3	/* trunk group scope */
#define ISUP_SCOPE_SR		4	/* signalling relation scope */
#define ISUP_SCOPE_SP		5	/* signalling point scope */
#define ISUP_SCOPE_DF		6	/* default scope */
#define ISUP_SCOPE_CIC		7	/* for unidentified cic addresses */

/*
 *  Definitions for CCI for Q.764 Conforming CCS Providers.
 */

enum {
	ISUP_INCOMING_INTERNATIONAL_EXCHANGE = 0x00000001UL,
	ISUP_SUSPEND_NATIONALLY_PERFORMED = 0x00000002UL,
};

enum {
	CMS_IDLE = 0,
	CMS_WCON_BLREQ,
	CMS_WRES_BLIND,
	CMS_WACK_BLRES,
	CMS_WCON_UBREQ,
	CMS_WRES_UBIND,
	CMS_WACK_UBRES,
	CMS_WCON_RESREQ,
	CMS_WRES_RESIND,
	CMS_WACK_RESRES,
	CMS_WCON_QRYREQ,
	CMS_WRES_QRYIND,
	CMS_WACK_QRYRES,
};

enum {
	CKS_IDLE = 0,
	CKS_WIND_CONT,
	CKS_WRES_CONT,
	CKS_WIND_CTEST,
	CKS_WREQ_CTEST,
	CKS_WIND_CCREP,
	CKS_WREQ_CCREP,
	CKS_WCON_RELREQ,
	CKS_WRES_RELIND,
};

/*
 *  Circuit States:
 */
#define CTS_ICC			0x00000010
#define CTS_OGC			0x00000020
#define CTS_COT			0x00000040
#define CTS_LPA			0x00000080
#define CTS_COR			0x00000100
#define CTS_MASK		0x0000000f

#define CTS_DIRECTION(__val)	(__val & (CTS_ICC|CTS_OGC))
#define CTS_CONT_CHECK(__val)	(__val & (CTS_COT|CTS_LPA|CTS_COR))
#define CTS_MESSAGE(__val)	(__val & CTS_MASK)

#define CTS_IDLE		0x00000000
#define CTS_WAIT_IAM		0x00000001
#define CTS_WAIT_CCR		0x00000002
#define CTS_WAIT_LPA		0x00000003
#define CTS_WAIT_SAM		0x00000004
#define CTS_WAIT_ACM		0x00000005
#define CTS_WAIT_ANM		0x00000006
#define CTS_ANSWERED		0x00000007
#define CTS_SUSPENDED		0x00000008
#define CTS_WAIT_RLC		0x00000009
#define CTS_SEND_RLC		0x0000000a

#define CTS_ICC_WAIT_COT_CCR	( CTS_ICC | CTS_COT | CTS_WAIT_CCR )
#define CTS_OGC_WAIT_COT_CCR	( CTS_OGC | CTS_COT | CTS_WAIT_CCR )
#define CTS_ICC_WAIT_LPA_CCR	( CTS_ICC | CTS_LPA | CTS_WAIT_CCR )
#define CTS_OGC_WAIT_LPA_CCR	( CTS_OGC | CTS_LPA | CTS_WAIT_CCR )
#define CTS_ICC_WAIT_CCR	( CTS_ICC | CTS_WAIT_CCR )
#define CTS_OGC_WAIT_CCR	( CTS_OGC | CTS_WAIT_CCR )
#define CTS_ICC_WAIT_COR_SAM	( CTS_ICC | CTS_COR | CTS_WAIT_SAM )
#define CTS_OGC_WAIT_COR_SAM	( CTS_OGC | CTS_COR | CTS_WAIT_SAM )
#define CTS_ICC_WAIT_COT_SAM	( CTS_ICC | CTS_COT | CTS_WAIT_SAM )
#define CTS_OGC_WAIT_COT_SAM	( CTS_OGC | CTS_COT | CTS_WAIT_SAM )
#define CTS_ICC_WAIT_LPA_SAM	( CTS_ICC | CTS_LPA | CTS_WAIT_SAM )
#define CTS_OGC_WAIT_LPA_SAM	( CTS_OGC | CTS_LPA | CTS_WAIT_SAM )
#define CTS_ICC_WAIT_SAM	( CTS_ICC | CTS_WAIT_SAM )
#define CTS_OGC_WAIT_SAM	( CTS_OGC | CTS_WAIT_SAM )
#define CTS_ICC_WAIT_COR_ACM	( CTS_ICC | CTS_COR | CTS_WAIT_ACM )
#define CTS_OGC_WAIT_COR_ACM	( CTS_OGC | CTS_COR | CTS_WAIT_ACM )
#define CTS_ICC_WAIT_COT_ACM	( CTS_ICC | CTS_COT | CTS_WAIT_ACM )
#define CTS_OGC_WAIT_COT_ACM	( CTS_OGC | CTS_COT | CTS_WAIT_ACM )
#define CTS_ICC_WAIT_LPA_ACM	( CTS_ICC | CTS_LPA | CTS_WAIT_ACM )
#define CTS_OGC_WAIT_LPA_ACM	( CTS_OGC | CTS_LPA | CTS_WAIT_ACM )
#define CTS_ICC_WAIT_ACM	( CTS_ICC | CTS_WAIT_ACM )
#define CTS_OGC_WAIT_ACM	( CTS_OGC | CTS_WAIT_ACM )
#define CTS_ICC_WAIT_ANM	( CTS_ICC | CTS_WAIT_ANM )
#define CTS_OGC_WAIT_ANM	( CTS_OGC | CTS_WAIT_ANM )
#define CTS_ICC_ANSWERED	( CTS_ICC | CTS_ANSWERED )
#define CTS_OGC_ANSWERED	( CTS_OGC | CTS_ANSWERED )
#define CTS_ICC_SUSPENDED	( CTS_ICC | CTS_SUSPENDED )
#define CTS_OGC_SUSPENDED	( CTS_OGC | CTS_SUSPENDED )
#define CTS_ICC_WAIT_RLC	( CTS_ICC | CTS_WAIT_RLC )
#define CTS_OGC_WAIT_RLC	( CTS_OGC | CTS_WAIT_RLC )
#define CTS_ICC_SEND_RLC	( CTS_ICC | CTS_SEND_RLC )
#define CTS_OGC_SEND_RLC	( CTS_OGC | CTS_SEND_RLC )

/*
 *  Circuit, Group and MTP Flags
 */
#define CCTF_LOC_M_BLOCKED		0x00000001UL
#define CCTF_REM_M_BLOCKED		0x00000002UL
#define CCTF_LOC_H_BLOCKED		0x00000004UL
#define CCTF_REM_H_BLOCKED		0x00000008UL
#define CCTF_LOC_M_BLOCK_PENDING	0x00000010UL
#define CCTF_REM_M_BLOCK_PENDING	0x00000020UL
#define CCTF_LOC_H_BLOCK_PENDING	0x00000040UL
#define CCTF_REM_H_BLOCK_PENDING	0x00000080UL
#define CCTF_LOC_M_UNBLOCK_PENDING	0x00000100UL
#define CCTF_REM_M_UNBLOCK_PENDING	0x00000200UL
#define CCTF_LOC_H_UNBLOCK_PENDING	0x00000400UL
#define CCTF_REM_H_UNBLOCK_PENDING	0x00000800UL
#define CCTF_LOC_RESET_PENDING		0x00001000UL
#define CCTF_REM_RESET_PENDING		0x00002000UL
#define CCTF_LOC_QUERY_PENDING		0x00004000UL
#define CCTF_REM_QUERY_PENDING		0x00008000UL
#define CCTF_ORIG_SUSPENDED		0x00010000UL
#define CCTF_TERM_SUSPENDED		0x00020000UL
#define CCTF_UPT_PENDING		0x00040000UL
#define CCTF_LOC_S_BLOCKED		0x00080000UL
#define CCTF_LOC_G_BLOCK_PENDING	0x00100000UL
#define CCTF_REM_G_BLOCK_PENDING	0x00200000UL
#define CCTF_LOC_G_UNBLOCK_PENDING	0x00400000UL
#define CCTF_REM_G_UNBLOCK_PENDING	0x00800000UL
#define CCTF_COR_PENDING		0x01000000UL
#define CCTF_COT_PENDING		0x02000000UL
#define CCTF_LPA_PENDING		0x04000000UL

#define CCTM_OUT_OF_SERVICE	( \
					CCTF_LOC_S_BLOCKED | \
					CCTF_REM_M_BLOCKED | \
					CCTF_REM_H_BLOCKED | \
					CCTF_REM_M_BLOCK_PENDING | \
					CCTF_REM_H_BLOCK_PENDING | \
					CCTF_REM_G_BLOCK_PENDING | \
					CCTF_LOC_RESET_PENDING | \
					CCTF_REM_RESET_PENDING | \
					0 \
				)

#define CCTM_CONT_CHECK		( \
					CCTF_COR_PENDING | \
					CCTF_COT_PENDING | \
					CCTF_LPA_PENDING | \
					0 \
				)

/*
   Cause values for CC_CALL_REATTEMPT_IND 
 */
/*
   Cause values -- Q.764 conforming 
 */
#define ISUP_REATTEMPT_DUAL_SIEZURE		1UL
#define ISUP_REATTEMPT_RESET			2UL
#define ISUP_REATTEMPT_BLOCKING			3UL
#define ISUP_REATTEMPT_T24_TIMEOUT		4UL
#define ISUP_REATTEMPT_UNEXPECTED		5UL
#define ISUP_REATTEMPT_COT_FAILURE		6UL
#define ISUP_REATTEMPT_CIRCUIT_BUSY		7UL

/*
   Call types for CC_SETUP_REQ and CC_SETUP_IND 
 */
/*
   Call types -- Q.764 Conforming 
 */
#define ISUP_CALL_TYPE_SPEECH			0x00000000UL
#define ISUP_CALL_TYPE_64KBS_UNRESTRICTED	0x00000002UL
#define ISUP_CALL_TYPE_3_1kHZ_AUDIO		0x00000003UL
#define ISUP_CALL_TYPE_64KBS_PREFERRED		0x00000006UL
#define ISUP_CALL_TYPE_2x64KBS_UNRESTRICTED	0x00000007UL
#define ISUP_CALL_TYPE_384KBS_UNRESTRICTED	0x00000008UL
#define ISUP_CALL_TYPE_1536KBS_UNRESTRICTED	0x00000009UL
#define ISUP_CALL_TYPE_1920KBS_UNRESTRICTED	0x0000000aUL
/*
   Call flags for CC_SETUP_REQ and CC_SETUP_IND 
 */
/*
   Call flags -- Q.764 Conforming 
 */
#define ISUP_NCI_ONE_SATELLITE_CCT		0x00000001UL
#define ISUP_NCI_TWO_SATELLITE_CCT		0x00000002UL
#define ISUP_NCI_SATELLITE_MASK			0x00000003UL
#define ISUP_NCI_CONT_CHECK_REQUIRED		0x00000004UL
#define ISUP_NCI_CONT_CHECK_PREVIOUS		0x00000008UL
#define ISUP_NCI_CONT_CHECK_MASK		0x0000000cUL
#define ISUP_NCI_OG_ECHO_CONTROL_DEVICE		0x00000010UL
/*
   Call flags for CC_SETUP_REQ and CC_SETUP_IND 
 */
/*
   Call flags -- Q.764 Conforming 
 */
#define ISUP_FCI_INTERNATIONAL_CALL		0x00000100UL
#define ISUP_FCI_PASS_ALONG_E2E_METHOD_AVAIL	0x00000200UL
#define ISUP_FCI_SCCP_E2E_METHOD_AVAILABLE	0x00000400UL
#define ISUP_FCI_INTERWORKING_ENCOUNTERED	0x00000800UL
#define ISUP_FCI_E2E_INFORMATION_AVAILABLE	0x00001000UL
#define ISUP_FCI_ISDN_USER_PART_ALL_THE_WAY	0x00002000UL
#define ISUP_FCI_ISDN_USER_PART_NOT_REQUIRED	0x00004000UL
#define ISUP_FCI_ISDN_USER_PART_REQUIRED	0x00008000UL
#define ISUP_FCI_ORIGINATING_ACCESS_ISDN	0x00010000UL
#define ISUP_FCI_SCCP_CLNS_METHOD_AVAILABLE	0x00020000UL
#define ISUP_FCI_SCCP_CONS_METHOD_AVAILABLE	0x00040000UL
/*
   Call flags for CC_SETUP_REQ and CC_SETUP_IND 
 */
/*
   Call flags -- Q.764 Conforming 
 */
#define ISUP_CPC_MASK				0xff000000UL
#define ISUP_CPC_UNKNOWN			0x00000000UL
#define ISUP_CPC_OPERATOR_FRENCH		0x01000000UL
#define ISUP_CPC_OPERATOR_ENGLISH		0x02000000UL
#define ISUP_CPC_OPERATOR_GERMAN		0x03000000UL
#define ISUP_CPC_OPERATOR_RUSSIAN		0x04000000UL
#define ISUP_CPC_OPERATOR_SPANISH		0x05000000UL
#define ISUP_CPC_OPERATOR_LANGUAGE_6		0x06000000UL
#define ISUP_CPC_OPERATOR_LANGUAGE_7		0x07000000UL
#define ISUP_CPC_OPERATOR_LANGUAGE_8		0x08000000UL
#define ISUP_CPC_OPERATOR_CODE_9		0x09000000UL
#define ISUP_CPC_SUBSCRIBER_ORDINARY		0x0a000000UL
#define ISUP_CPC_SUBSCRIBER_PRIORITY		0x0b000000UL
#define ISUP_CPC_VOICE_BAND_DATA		0x0c000000UL
#define ISUP_CPC_TEST_CALL			0x0d000000UL
#define ISUP_CPC_SPARE				0x0e000000UL
#define ISUP_CPC_PAYPHONE			0x0f000000UL

/*
   Flags for CC_CONT_REPORT_REQ and CC_CONT_REPORT_IND 
 */
/*
   Flags -- Q.764 Conforming 
 */
#define ISUP_COT_FAILURE			0x00000000UL
#define ISUP_COT_SUCCESS			0x00000001UL

/*
   Flags for CC_PROCEEDING, CC_ALERTING, CC_PROGRESS, CC_IBI 
 */
/*
   Flags -- Q.764 Conforming 
 */
#define ISUP_BCI_NO_CHARGE			0x00000001UL
#define ISUP_BCI_CHARGE				0x00000002UL
#define ISUP_BCI_CHARGE_MASK			0x00000003UL
#define ISUP_BCI_SUBSCRIBER_FREE		0x00000004UL
#define ISUP_BCI_CONNECT_FREE			0x00000008UL
#define ISUP_BCI_CPS_MASK			0x0000000cUL
#define ISUP_BCI_ORDINARY_SUBSCRIBER		0x00000010UL
#define ISUP_BCI_PAYPHONE			0x00000020UL
#define ISUP_BCI_CPI_MASK			0x00000030UL
#define ISUP_BCI_PASS_ALONG_E2E_METHOD_AVAIL	0x00000040UL
#define ISUP_BCI_SCCP_E2E_METHOD_AVAILABLE	0x00000080UL
#define ISUP_BCI_E2E_MASK			0x000000c0UL
#define ISUP_BCI_INTERWORKING_ENCOUNTERED	0x00000100UL
#define ISUP_BCI_E2E_INFORMATION_AVAILABLE	0x00000200UL
#define ISUP_BCI_ISDN_USER_PART_ALL_THE_WAY	0x00000400UL
#define ISUP_BCI_HOLDING_REQUESTED		0x00000800UL
#define ISUP_BCI_TERMINATING_ACCESS_ISDN	0x00001000UL
#define ISUP_BCI_IC_ECHO_CONTROL_DEVICE		0x00002000UL
#define ISUP_BCI_SCCP_CLNS_METHOD_AVAILABLE	0x00004000UL
#define ISUP_BCI_SCCP_CONS_METHOD_AVAILABLE	0x00008000UL
#define ISUP_BCI_SCCP_METHOD_MASK		0x0000c000UL
#define ISUP_OBCI_INBAND_INFORMATION_AVAILABLE	0x00010000UL
#define ISUP_OBCI_CALL_DIVERSION_MAY_OCCUR	0x00020000UL
#define ISUP_OBCI_ADDITIONAL_INFO_IN_SEG	0x00040000UL
#define ISUP_OBCI_MLPP_USER			0x00080000UL

/*
   Events for CC_PROGRESS_REQ and CC_PROGRESS_IND 
 */
/*
   Events -- Q.764 Conforming 
 */
#define ISUP_EVNT_PRES_RESTRICT			0x80
#define ISUP_EVNT_ALERTING			0x01	/* alerting */
#define ISUP_EVNT_PROGRESS			0x02	/* progress */
#define ISUP_EVNT_IBI				0x03	/* in-band info or approp pattern avail */
#define ISUP_EVNT_CFB				0x04	/* call forwarded busy */
#define ISUP_EVNT_CFNA				0x05	/* call forwarded no reply */
#define ISUP_EVNT_CFU				0x06	/* call forwarded unconditional */
#define ISUP_EVNT_MASK				0x7f

/*
   Cause values CC_CALL_FAILURE_IND -- Q.764 Conforming 
 */
#define ISUP_CALL_FAILURE_COT_FAILURE		 1UL
#define	ISUP_CALL_FAILURE_RESET			 2UL
#define ISUP_CALL_FAILURE_RECV_RLC		 3UL
#define ISUP_CALL_FAILURE_BLOCKING		 4UL
#define ISUP_CALL_FAILURE_T2_TIMEOUT		 5UL
#define ISUP_CALL_FAILURE_T3_TIMEOUT		 6UL
#define ISUP_CALL_FAILURE_T6_TIMEOUT		 7UL
#define ISUP_CALL_FAILURE_T7_TIMEOUT		 8UL
#define ISUP_CALL_FAILURE_T8_TIMEOUT		 9UL
#define ISUP_CALL_FAILURE_T9_TIMEOUT		10UL
#define ISUP_CALL_FAILURE_T35_TIMEOUT		11UL
#define ISUP_CALL_FAILURE_T38_TIMEOUT		12UL
#define ISUP_CALL_FAILURE_CIRCUIT_BUSY		13UL

/*
 *  Q.850 Cause Values
 */
/*
   Normal class 
 */
#define CC_CAUS_UNALLOCATED_NUMBER		  1	/* Unallocated (unassigned) number */
#define CC_CAUS_NO_ROUTE_TO_TRANSIT_NETWORK	  2	/* No route to specified transit network */
#define CC_CAUS_NO_ROUTE_TO_DESTINATION		  3	/* No route to destination */
#define CC_CAUS_SEND_SPECIAL_INFO_TONE		  4	/* Send special information tone */
#define CC_CAUS_MISDIALLED_TRUNK_PREFIX		  5	/* Misdialled trunk prefix */
#define CC_CAUS_PREEMPTION			  8	/* Preemption */
#define CC_CAUS_PREEMPTION_CCT_RESERVED		  9	/* Preemption - circuit reserved for reuse */
#define CC_CAUS_NORMAL_CALL_CLEARING		 16	/* Normal call clearing */
#define CC_CAUS_USER_BUSY			 17	/* User busy */
#define CC_CAUS_NO_USER_RESPONDING		 18	/* No user responding */
#define CC_CAUS_NO_ANSWER			 19	/* No answer from user (user alerted) */
#define CC_CAUS_SUBSCRIBER_ABSENT		 20	/* Subscriber absent */
#define CC_CAUS_CALL_REJECTED			 21	/* Call rejected */
#define CC_CAUS_NUMBER_CHANGED			 22	/* Number changed */
#define CC_CAUS_REDIRECT			 23	/* Redirect to new destination */
#define CC_CAUS_OUT_OF_ORDER			 27	/* Desitination out of order */
#define CC_CAUS_ADDRESS_INCOMPLETE		 28	/* Invalid number format (address
							   incomplete) */
#define CC_CAUS_FACILITY_REJECTED		 29	/* Facility rejected */
#define CC_CAUS_NORMAL_UNSPECIFIED		 31	/* Normal unspecified */
/*
   Resource Unavailable Class 
 */
#define CC_CAUS_NO_CCT_AVAILABLE		 34	/* No circuit/channel available */
#define CC_CAUS_NETWORK_OUT_OF_ORDER		 38	/* Network out of order */
#define CC_CAUS_TEMPORARY_FAILURE		 41	/* Temporary failure */
#define CC_CAUS_SWITCHING_EQUIP_CONGESTION	 42	/* Switching equipment congestion */
#define CC_CAUS_ACCESS_INFO_DISCARDED		 43	/* Access information discarded */
#define CC_CAUS_REQUESTED_CCT_UNAVAILABLE	 44	/* Requested circuit/channel not available */
#define CC_CAUS_PRECEDENCE_CALL_BLOCKED		 46	/* Precedence call blocked */
#define CC_CAUS_RESOURCE_UNAVAILABLE		 47	/* Resource unavailable, unspecified */
/*
   Service or Option Unavaialble Class 
 */
#define CC_CAUS_NOT_SUBSCRIBED			 50	/* Requested facility not subscribed */
#define CC_CAUS_OGC_BARRED_WITHIN_CUG		 53	/* Outgoing calls barred within CUG */
#define CC_CAUS_ICC_BARRED WITHIN_CUG		 55	/* Incoming calls barred within CUG */
#define CC_CAUS_BC_NOT_AUTHORIZED		 57	/* Bearer capability not authorized */
#define CC_CAUS_BC_NOT_AVAILABLE		 58	/* Bearer capability not presently
							   available */
#define CC_CAUS_INCONSISTENCY			 62	/* Inconsistency in designated outgoing
							   access information and subscriber class */
#define CC_CAUS_SERVICE_OPTION_NOT_AVAILABLE	 63	/* Service or option not available,
							   unspecified */
/*
   Service or Option Not Implemented Class 
 */
#define CC_CAUS_BC_NOT_IMPLEMENTED		 65	/* Bearer capability not implemented */
#define CC_CAUS_FACILITY_NOT_IMPLEMENTED	 69	/* Requested facility not implemented */
#define CC_CAUS_RESTRICTED_BC_ONLY		 70	/* Only restricted digital information
							   bearer capability is available */
#define CC_CAUS_SERIVCE_OPTION_NOT_IMPLEMENTED	 79	/* Service or option not implemented,
							   unspecified */
/*
   Invalid Message (e.g., Parameter out of Range) Class 
 */
#define CC_CAUS_USER_NOT_MEMBER_OF_CUG		 87	/* User not member of CUG */
#define CC_CAUS_INCOMPATIBLE_DESTINATION	 88	/* Incompatible destination */
#define CC_CAUS_NON_EXISTENT_CUG		 90	/* Non-existent CUG */
#define CC_CAUS_INVALID_TRANSIT_NTWK_SELECTION	 91	/* Invalid transit network selection */
#define CC_CAUS_INVALID_MESSAGE			 95	/* Invalid message, unspecified */
/*
   Protocol Error (e.g., Unknwon Message) Class 
 */
#define CC_CAUS_MESSAGE_TYPE_NOT_IMPLEMENTED	 97	/* Message typ non-existent or not
							   implemented. */
#define CC_CAUS_PARAMETER_NOT_IMPLEMENTED	 99	/* Information element/Parameter
							   non-existent or not implemented */
#define CC_CAUS_RECOVERY_ON_TIMER_EXPIRY	102	/* Recovery on timer expiry */
#define CC_CAUS_PARAMETER_PASSED_ON		103	/* Parameter non-existent or not
							   implemented - passed on */
#define CC_CAUS_MESSAGE_DISCARDED		110	/* Message with unrecognized parameter
							   discarded */
#define CC_CAUS_PROTOCOL_ERROR			111	/* Protocol error, unspecified */
/*
   Interworking Class 
 */
#define CC_CAUS_INTERWORKING			127	/* Interworking, unspecified */
/*
 *  ANSI Standard Causes
 */
/*
   Normal Class 
 */
#define CC_CAUS_UNALLOCATED_DEST_NUMBER		 23	/* Unallocated destination number */
#define CC_CAUS_UNKNOWN_BUSINESS_GROUP		 24	/* Unknown business group */
#define CC_CAUS_EXCHANGE_ROUTING_ERROR		 25	/* Exchange routing error */
#define CC_CAUS_MISROUTED_CALL_TO_PORTED_NUMBER	 26	/* Misrouted call to a ported number */
#define CC_CAUS_LNP_QOR_NUMBER_NOT_FOUND	 27	/* Number portability Query on Release
							   (QoR) number not found. */
/*
   Resource Unavailable Class 
 */
#define CC_CAUS_RESOURCE_PREEMPTION		 45	/* Preemption. */
#define CC_CAUS_PRECEDENCE_CALL_BLOCKED		 46	/* Precedence call blocked. */
/*
   Service or Option Not Available Class 
 */
#define CC_CAUS_CALL_TYPE_INCOMPATIBLE		 51	/* Call type incompatible with service
							   request */
#define CC_CAUS_GROUP_RESTRICTIONS		 54	/* Call blocked due to group restrictions */

/*
   Management flags -- Q.764 Conforming 
 */
#define ISUP_GROUP				0x00010000UL
#define ISUP_MAINTENANCE_ORIENTED		0x00000000UL
#define ISUP_HARDWARE_FAILURE_ORIENTED		0x00000001UL

#define ISUP_SRIS_MASK				0x3
#define ISUP_SRIS_NETWORK_INITIATED		0x1
#define ISUP_SRIS_USER_INITIATED		0x2

/*
   Maintenance indications -- Q.764 Conforming 
 */
#define ISUP_MAINT_T5_TIMEOUT			 3UL	/* Q.752 12.5 on occrence */
#define ISUP_MAINT_T13_TIMEOUT			 4UL	/* Q.752 12.16 1st and delta */
#define ISUP_MAINT_T15_TIMEOUT			 5UL	/* Q.752 12.17 1st and delta */
#define ISUP_MAINT_T17_TIMEOUT			 6UL	/* Q.752 12.1 1st and delta */
#define ISUP_MAINT_T19_TIMEOUT			 7UL	/* Q.752 12.18 1st and delta */
#define ISUP_MAINT_T21_TIMEOUT			 8UL	/* Q.752 12.19 1st and delta */
#define ISUP_MAINT_T23_TIMEOUT			 9UL	/* Q.752 12.2 1st and delta */
#define ISUP_MAINT_T25_TIMEOUT			10UL
#define ISUP_MAINT_T26_TIMEOUT			11UL
#define ISUP_MAINT_T27_TIMEOUT			12UL
#define ISUP_MAINT_T28_TIMEOUT			13UL
#define ISUP_MAINT_T36_TIMEOUT			14UL
#define ISUP_MAINT_UNEXPECTED_CGBA		15UL	/* Q.752 12.12 1st and delta */
#define ISUP_MAINT_UNEXPECTED_CGUA		16UL	/* Q.752 12.13 1st and delta */
#define ISUP_MAINT_UNEXPECTED_MESSAGE		17UL	/* Q.752 12.21 1st and delta */
#define ISUP_MAINT_UNEQUIPPED_CIC		18UL
#define ISUP_MAINT_SEGMENTATION_DISCARDED	19UL
#define ISUP_MAINT_USER_PART_UNEQUIPPED		20UL
#define ISUP_MAINT_USER_PART_UNAVAILABLE	21UL	/* Q.752 10.1, 10.8 on occrence */
#define ISUP_MAINT_USER_PART_AVAILABLE		22UL	/* Q.752 10.3, 10.9 on occrence */
#define ISUP_MAINT_USER_PART_MAN_MADE_BUSY	23UL	/* Q.752 10.2 on occrence */	/* XXX */
#define ISUP_MAINT_USER_PART_CONGESTED		24UL	/* Q.752 10.5, 10.11 on occrence */
#define ISUP_MAINT_USER_PART_UNCONGESTED	25UL	/* Q.752 10.6, 10.12 on occrence */
#define ISUP_MAINT_MISSING_ACK_IN_CGBA		26UL	/* Q.752 12.8 1st and delta */
#define ISUP_MAINT_MISSING_ACK_IN_CGUA		27UL	/* Q.752 12.9 1st and delta */
#define ISUP_MAINT_ABNORMAL_ACK_IN_CGBA		28UL	/* Q.752 12.10 1st and delta */
#define ISUP_MAINT_ABNORMAL_ACK_IN_CGUA		29UL	/* Q.752 12.11 1st and delta */
#define ISUP_MAINT_UNEXPECTED_BLA		30UL	/* Q.752 12.14 1st and delta */
#define ISUP_MAINT_UNEXPECTED_UBA		31UL	/* Q.752 12.15 1st and delta */
#define ISUP_MAINT_RELEASE_UNREC_INFO		32UL	/* Q.752 12.22 1st and delta */	/* XXX */
#define ISUP_MAINT_RELEASE_FAILURE		33UL	/* Q.752 12.23 1st and delta */	/* XXX */
#define ISUP_MAINT_MESSAGE_FORMAT_ERROR	        34UL	/* Q.752 12.20 1st and delta */	/* XXX */

#endif				/* __SS7_ISUPI_H__ */
