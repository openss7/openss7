/*****************************************************************************

 @(#) $Id: mtp3b.h,v 0.9.2.6 2008-04-29 07:10:44 brian Exp $

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

 Last Modified $Date: 2008-04-29 07:10:44 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: mtp3b.h,v $
 Revision 0.9.2.6  2008-04-29 07:10:44  brian
 - updating headers for release

 Revision 0.9.2.5  2007/08/14 12:17:10  brian
 - GPLv3 header updates

 Revision 0.9.2.4  2007/02/13 14:05:28  brian
 - corrected ulong and long for 32-bit compat

 *****************************************************************************/

#ifndef __SS7_MTP3B_H__
#define __SS7_MTP3B_H__

#ident "@(#) $RCSfile: mtp3b.h,v $ $Name:  $($Revision: 0.9.2.6 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

/* This file can be processed by doxygen(1). */

#define AAL_INFO_REQ			 0	/* Information request */
#define AAL_INFO_ACK			 1	/* Information acknowledgement */
#define AAL_ATTACH_REQ			 2	/* Attach a PPA */
#define AAL_DETACH_REQ			 3	/* Detach a PPA */
#define AAL_ENABLE_REQ			 4	/* Prepare a device */
#define AAL_DISABLE_REQ			 5	/* Disable a device */
#define AAL_OK_ACK			 6	/* Success acknowledgement */
#define AAL_ERROR_ACK			 7	/* Error acknowledgement */
#define AAL_ENABLE_CON			 8	/* Enable confirmation */
#define AAL_DISABLE_CON			 9	/* Disable confirmation */
#define AAL_ERROR_IND			10	/* Error indication */

#define AAL_MESSAGE_FOR_TRANSMISSION	 0
#define AAL_RECEIVED_MESSAGE		 1
#define AAL_LINK_CONGESTED		 2
#define AAL_LINK_CONGESTION_CEASED	 3
#define AAL_EMERGENCY			 4
#define AAL_EMERGENCY_CEASES		 5
#define AAL_STOP			 6
#define AAL_START			 7
#define AAL_IN_SERVICE			 8
#define AAL_OUT_OF_SERVICE		 9
#define AAL_RETRIEVE_BSNT		10
#define AAL_RETRIEVAL_REQUEST_AND_FSNC	11
#define AAL_RETRIEVED_MESSAGES		12
#define AAL_RETRIEVAL_COMPLETE		13
#define AAL_BSNT			14
#define AAL_FLUSH_BUFFERS		15
#define AAL_BSNT_NOT_RETRIEVABLE	16

/*
 *  AAL_MESSAGE_FOR_TRANSMISSION, one M_PROTO block followed by one or more
 *  M_DATA blocks
 */
typedef struct AAL_message_for_transmission {
	sl_ulong PRIM_type;
} AAL_message_for_transmission_t;

/*
 *  AAL_RECEIVED_MESSAGE, one M_PROTO block followed by one or more M_DATA
 *  blocks
 */
typedef struct AAL_received_message {
	sl_ulong PRIM_type;
} AAL_received_message_t;

/*
 *  AAL_LINK_CONGESTED, one M_PCPROTO block
 */
typedef struct AAL_link_congested {
	sl_ulong PRIM_type;
	sl_ulong CONGESTION_status;
	sl_ulong DISCARD_status;
} AAL_link_congested_t;

/*
 *  AAL_LINK_CONGESTION_CEASE, one M_PCPROTO block
 */
typedef struct AAL_link_congestion_ceased {
	sl_ulong PRIM_type;
} AAL_link_congestion_ceased_t;

/*
 *  AAL_EMERGENCY, one M_PCPROTO block
 */
typedef struct AAL_emergency {
	sl_ulong PRIM_type;
} AAL_emergency_t;

/*
 *  AAL_EMERGENCY_CEASES, one M_PCPROTO block
 */
typedef struct AAL_emergency_ceases {
	sl_ulong PRIM_type;
} AAL_emergency_ceases_t;

/*
 *  AAL_STOP, one M_PROTO block
 */
typedef struct AAL_stop {
	sl_ulong PRIM_type;
} AAL_stop_t;

/*
 *  AAL_START, one M_PROTO block
 */
typedef struct AAL_start {
	sl_ulong PRIM_type;
} AAL_start_t;

/*
 *  AAL_IN_SERVICE, one M_PROTO block
 */
typedef struct AAL_in_service {
	sl_ulong PRIM_type;
} AAL_in_service_t;

/*
 *  AAL_OUT_OF_SERVICE, one M_PROTO block
 */
typedef struct AAL_out_of_service {
	sl_ulong PRIM_type;
} AAL_out_of_service_t;

/*
 *  AAL_RETRIEVE_BSNT, one M_PCPROTO block
 */
typedef struct AAL_retrieve_bsnt {
	sl_ulong PRIM_type;
} AAL_retrieve_bsnt_t;

/*
 *  AAL_RETRIEVAL_REQUEST_AND_FSNC, one M_PCPROTO block
 */
typedef struct AAL_retrieval_request_and_fsnc {
	sl_ulong PRIM_type;
	sl_ulong FSNC;
} AAL_retrieval_request_and_fsnc_t;

/*
 *  AAL_RETIREVED_MESSAGES, one M_PROTO block followed by zero or more M_DATA
 *  blocks
 */
typedef struct AAL_retrieved_messages {
	sl_ulong PRIM_type;
} AAL_retrieved_messages_t;

/*
 *  AAL_RETIREVAL_COMPLETE, one M_PROTO block
 */
typedef struct AAL_retrieval_complete {
	sl_ulong PRIM_type;
} AAL_retrieval_complete_t;

/*
 *  AAL_BSNT, one M_PCPROTO block
 */
typedef struct AAL_bsnt {
	sl_ulong PRIM_type;
	sl_ulong BSNT;
} AAL_bsnt;

/*
 *  AAL_BSNT_NOT_RETRIEVABLE, one M_PCPROTO block
 */
typedef struct AAL_bsnt {
	sl_ulong PRIM_type;
} AAL_bsnt;

#define AALS_OUT_OF_SERVICE	0
#define AALS_ALIGNMENT		1
#define AALS_IN_SERVICE		2
#define AALS_PROVING		3
#define AALS_ALIGNED_READY	4

#endif				/* __SS7_MTP3B_H__ */
