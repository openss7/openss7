/*****************************************************************************

 @(#) $Id: isdni.h,v 0.9.2.1 2004/08/21 10:14:39 brian Exp $

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

 Last Modified $Date: 2004/08/21 10:14:39 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SS7_ISDNI_H__
#define __SS7_ISDNI_H__

/*
 *  ISDN address
 */

typedef struct isdn_addr {
	ulong scope;			/* the scope of the identifier */
	ulong id;			/* the identifier within the scope */
	ulong ci;			/* channel identifier within the scope */
} isdn_addr_t;

#define ISDN_SCOPE_CH           1	/* channel scope */
#define ISDN_SCOPE_FG           2	/* facility group scope */
#define ISDN_SCOPE_TG           3	/* transmission group scope */
#define ISDN_SCOPE_EG           4	/* equipment group scope */
#define ISDN_SCOPE_XG           5	/* customer/provider group scope */
#define ISDN_SCOPE_DF           6	/* default scope */

enum {
	U0_NULL,
	U1_CALL_INITIATED,
	U2_OVERLAP_SENDING,
	U3_OUTGOING_CALL_PROCEEDING,
	U4_CALL_DELIVERED,
	U6_CALL_PRESENT,
	U7_CALL_RECEIVED,
	U8_CONNECT_REQUEST,
	U9_INCOMING_CALL_PROCEEDING,
	U10_ACTIVE,
	U11_DISCONNECT_REQUEST,
	U12_DISCONNECT_INDICATION,
	U15_SUSPEND_REQUEST,
	U17_RESUME_REQUEST,
	U19_RELEASE_REQUEST,
	U25_OVERLAP_RECEIVING,
	N0_NULL,
	N1_CALL_INITIATED,
	N2_OVERLAP_SENDING,
	N3_OUTGOING_CALL_PROCEEDING,
	N4_CALL_DELIVERED,
	N6_CALL_PRESENT,
	N7_CALL_RECEIVED,
	N8_CONNECT_REQUEST,
	N9_INCOMING_CALL_PROCEEDING,
	N10_ACTIVE,
	N11_DISCONNECT_REQUEST,
	N12_DISCONNECT_INDICATION,
	N15_SUSPEND_REQUEST,
	N17_RESUME_REQUEST,
	N19_RELEASE_REQUEST,
	N22_CALL_ABORT,
	N25_OVERLAP_RECEIVING,
};

enum {
	CMS_IDLE = 0,
};

#define ISDN_PI_INTERWORKING	    0x0	/* FIXME */

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
#define CC_CAUS_CALL_ABANDONNED			  6	/* Call abandonned */
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
#define CC_CAUS_UNEXPECTED_MESSAGE		 81	/* Unexpected message */
#define CC_CAUS_USER_NOT_MEMBER_OF_CUG		 87	/* User not member of CUG */
#define CC_CAUS_INCOMPATIBLE_DESTINATION	 88	/* Incompatible destination */
#define CC_CAUS_NON_EXISTENT_CUG		 90	/* Non-existent CUG */
#define CC_CAUS_INVALID_TRANSIT_NTWK_SELECTION	 91	/* Invalid transit network selection */
#define CC_CAUS_INVALID_MESSAGE			 95	/* Invalid message, unspecified */
#define CC_CAUS_MISSING_MANDATORY_PARAMETER	 96	/* Invalid message, missing mandatory
							   parameter */
/*
   Protocol Error (e.g., Unknwon Message) Class 
 */
#define CC_CAUS_MESSAGE_TYPE_NOT_IMPLEMENTED	 97	/* Message typ non-existent or not
							   implemented. */
#define CC_CAUS_PARAMETER_NOT_IMPLEMENTED	 99	/* Information element/Parameter
							   non-existent or not implemented */
#define CC_CAUS_INVALID_MANDATORY_PARAMETER	100	/* Invalid mandatory parameter */
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

#endif				/* __SS7_ISDNI_H__ */
