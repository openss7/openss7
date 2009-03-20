/*****************************************************************************

 @(#) $Id: xmap_gsm_gc.h,v 0.9.2.2 2009-03-20 18:27:40 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
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

 Last Modified $Date: 2009-03-20 18:27:40 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: xmap_gsm_gc.h,v $
 Revision 0.9.2.2  2009-03-20 18:27:40  brian
 - documentation and headers

 Revision 0.9.2.1  2009-03-13 11:20:25  brian
 - doc and header updates

 *****************************************************************************/

#ifndef __XMAP_GSM_GC_H__
#define __XMAP_GSM_GC_H__

#ident "@(#) $RCSfile: xmap_gsm_gc.h,v $ $Name:  $($Revision: 0.9.2.2 $) Copyright (c) 2008-2009 Monavacon Limited."

#include <xom.h>
#include <xmap.h>
#include <xmap_gsm.h>

/*
 * { iso(1) org(3) dod(6) internet(1) private(4) enterprises(1) openss7(29591)
 *   xom-packages(1) xmap(1) gsm(2) gsm-gc(6) }
 */
#define OMP_O_MAP_GSM_GC_PKG \
    "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x01\x02\x06"

/* Intermediate object identifier macro: */
#define mapP_gsm_gc(X) (OMP_O_MAP_GSM_GC_PKG# #X)

/* This application context is used between anchor MSC and relay MSC for group
 * call and broadcast call procedures. */
#define OMP_O_MAP_GROUP_CALL_CONTROL_CONTEXT			mapP_acId(\x1F)
#define OMP_O_MAP_GROUP_CALL_CONTROL_CONTEXT_V3			mapP_acId(\x1F\x03)

/* This application context is used between group call serving MSC and visited
 * MSC for group call and broadcast call procedures. */
#define OMP_O_MAP_GROUP_CALL_INFO_RET_CONTROL_CONTEXT		mapP_acId(\x2D)
#define OMP_O_MAP_GROUP_CALL_INFO_RET_CONTROL_CONTEXT_V3	mapP_acId(\x2D\x03)

/* OM class names (prefixed MAP_C_) */
#define OMP_O_MAP_C_FORWARD_GROUP_CALL_SIGNALLING_ARG	mapP_gsm_gc(\x87\x69)	/* 1001 */
#define OMP_O_MAP_C_PREPARE_GROUP_CALL_ARG		mapP_gsm_gc(\x87\x6A)	/* 1002 */
#define OMP_O_MAP_C_PREPARE_GROUP_CALL_RES		mapP_gsm_gc(\x87\x6B)	/* 1003 */
#define OMP_O_MAP_C_PROCESS_GROUP_CALL_SIGNALLING_ARG	mapP_gsm_gc(\x87\x6C)	/* 1004 */
#define OMP_O_MAP_C_SEND_GROUP_CALL_END_SIGNAL_ARG	mapP_gsm_gc(\x87\x6D)	/* 1005 */
#define OMP_O_MAP_C_SEND_GROUP_CALL_END_SIGNAL_RES	mapP_gsm_gc(\x87\x6E)	/* 1006 */
#define OMP_O_MAP_C_SEND_GROUP_CALL_INFO_ARG		mapP_gsm_gc(\x87\x6F)	/* 1007 */
#define OMP_O_MAP_C_SEND_GROUP_CALL_INFO_RES		mapP_gsm_gc(\x87\x70)	/* 1008 */
#define OMP_O_MAP_C_STATE_ATTRIBUTES			mapP_gsm_gc(\x87\x71)	/* 1009 */

/* OM attribute names (prefixed MAP_) */
#define MAP_A_ANCHOR_MSC_ADDRESS			((OM_type)8004)
#define MAP_A_CALL_ORIGINATOR				((OM_type)8006)
#define MAP_A_CELL_ID					((OM_type)8007)
#define MAP_A_CIPHERING_ALGORITHM			((OM_type)8008)
#define MAP_A_CODEC_INFO				((OM_type)8010)
#define MAP_A_DOWNLINK_ATTACHED				((OM_type)8011)
#define MAP_A_DUAL_COMMUNICATION			((OM_type)8012)
#define MAP_A_EMERGENCY_MODE_RESET_COMMAND_FLAG		((OM_type)8013)
#define MAP_A_GROUP_CALL_NUMBER				((OM_type)8016)
#define MAP_A_GROUP_KEY					((OM_type)8018)
#define MAP_A_GROUP_KEY_NUMBER_VK_ID			((OM_type)8019)
#define MAP_A_PRIORITY					((OM_type)8022)
#define MAP_A_RELEASE_GROUP_CALL			((OM_type)8023)
#define MAP_A_SM_RP_UI					((OM_type)8025)
#define MAP_A_STATE_ATTRIBUTES				((OM_type)8026)
#define MAP_A_TALKER_CHANNEL_PARAMETER			((OM_type)8027)
#define MAP_A_TALKER_PRIORITY				((OM_type)8028)
#define MAP_A_TMSI					((OM_type)8030)
#define MAP_A_UPLINK_ATTACHED				((OM_type)8031)
#define MAP_A_UPLINK_FREE				((OM_type)8032)
#define MAP_A_UPLINK_REJECT_COMMAND			((OM_type)8033)
#define MAP_A_UPLINK_RELEASE_COMMAND			((OM_type)8034)
#define MAP_A_UPLINK_RELEASE_INDICATION			((OM_type)8035)
#define MAP_A_UPLINK_REQUEST				((OM_type)8036)
#define MAP_A_UPLINK_REQUEST_ACK			((OM_type)8037)
#define MAP_A_UPLINK_SEIZED_COMMAND			((OM_type)8038)
#define MAP_A_VSTK					((OM_type)8039)
#define MAP_A_VSTK_RAND					((OM_type)8040)

#if 0
/* Attributes used defined in <xmap_gsm.h> */
#define MAP_A_ADDITIONAL_INFO				((OM_type)8001)
#define MAP_A_ADDITIONAL_SUBSCRIPTIONS			((OM_type)8002)
#define MAP_A_AN_APDU					((OM_type)8003)
#define MAP_A_ASCI_CALL_REFERENCE			((OM_type)8005)
#define MAP_A_CKSN					((OM_type)8009)
#define MAP_A_EXTENSION					((OM_type)8014)
#define MAP_A_EXTENSION_CONTAINER			((OM_type)8015)
#define MAP_A_GROUP_ID					((OM_type)8017)
#define MAP_A_IMSI					((OM_type)8020)
#define MAP_A_KC					((OM_type)8021)
#define MAP_A_REQUESTED_INFO				((OM_type)8024)
#define MAP_A_TELESERVICE				((OM_type)8029)
#endif

/* OM enumerations (prefixed MAP_T_) */

/* MAP_T_Talker_Priority: */
typedef enum {
	MAP_T_NORMAL = 0,
	MAP_T_PRIVILEGED = 1,
	MAP_T_EMERGENCY = 2,
} MAP_T_Talker_Priority;

/* MAP_T_Requested_Info: */
typedef enum {
	MAP_T_ANCHOR_MSC_ADDRESS_AND_ASCI_CALL_REFERENCE = 0,
	MAP_T_IMSI_AND_ADDITIONAL_INFO_AND_ADDITIONAL_SUBSCRIPTION = 1,
} MAP_T_Requested_Info;

#if 0
/* MAP_T_Additional_Subscriptions: (bits) */
typedef enum {
	MAP_T_PRIVILEGED_UP_LINK_REQUEST = 0,
	MAP_T_EMERGENCY_UP_LINK_REQUEST = 1,
	MAP_T_EMERGENCY_RESET = 2,
} MAP_T_Additional_Subscriptions;
#endif

/* Operation codes (MAP_T_Operation_Code): */
#define MAP_T_PREPARE_GROUP_CALL			39
#define MAP_T_SEND_GROUP_CALL_END_SIGNAL		40
#define MAP_T_FORWARD_GROUP_CALL_SIGNALLING		42
#define MAP_T_PROCESS_GROUP_CALL_SIGNALLING		41
#define MAP_T_SEND_GROUP_CALL_INFO			84

#if 0
/*
 * SERVICE ERRORS
 */
/* Service-Error problem values (MAP_T_User_Error): */
#define MAP_E_NO_GROUP_CALL_NUMBER_AVAILABLE		50

/* Service-Error parameter classes: */
#define OMP_O_MAP_C_NO_GROUP_CALL_NB_PARAM		mapP_gsm_ss(\xCE\x11)	/* 2001 */

/* Service-Error attributes: */

/* Service-Error enumerations: */
#endif

/* Uses:
 *
 * OMP_O_MAP_C_EXTENSION <xmap.h>
 *
 * OMP_O_MAP_C_ACCESS_NETWORK_SIGNAL_INFO <xmap_gsm.h>
 * OMP_O_MAP_C_EXTENSION_CONTAINER <xmap_gsm.h>
 *
 * OMP_O_MAP_C_STATE_ATTRIBUTES (this file)
 *
 * MAP_T_Talker_Priority (this file)
 * MAP_T_Requested_Info (this file)
 * MAP_T_Additional_Subscriptions (this file)
 */

/* Errors:
 *
 * OMP_O_MAP_C_EXTENSION <xmap.h>
 *
 * OMP_O_MAP_C_EXTENSION_CONTAINTER <xmap_gsm.h>
 */

#endif				/* __XMAP_GSM_GC_H__ */
