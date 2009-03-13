/*****************************************************************************

 @(#) $Id: xmap_gsm_om.h,v 0.9.2.1 2009-03-13 11:20:25 brian Exp $

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

 Last Modified $Date: 2009-03-13 11:20:25 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: xmap_gsm_om.h,v $
 Revision 0.9.2.1  2009-03-13 11:20:25  brian
 - doc and header updates

 *****************************************************************************/

#ifndef __XMAP_GSM_OM_H__
#define __XMAP_GSM_OM_H__

#ident "@(#) $RCSfile: xmap_gsm_om.h,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) Copyright (c) 2008-2009 Monavacon Limited."

/*
 * { iso(1) org(3) dod(6) internet(1) private(4) enterprises(1) openss7(29591)
 *   xom-packages(1) xmap(1) gsm(2) gsm-om(4) }
 */
#define OMP_O_MAP_GSM_OM_PKG \
    "\x2b\x06\x01\x04\x01\x81\xe7\x17\x01\x01\x02\x04"

/* Intermediate object identifier macro: */
#define mapP_gsm_om(X) (OMP_O_MAP_GSM_OM_PKG# #X)

/* OM class names (prefixed MAP_C_) */
/* Service-Error parameter classes: */
#define OMP_O_MAP_C_ACTIVATE_TRACE_MODE_ARG			mapP_gsm_om(\x9F\x21) /* 4001 */
#define OMP_O_MAP_C_ACTIVATE_TRACE_MODE_RES			mapP_gsm_om(\x9F\x22) /* 4002 */
#define OMP_O_MAP_C_DEACTIVATE_TRACE_MODE_ARG			mapP_gsm_om(\x9F\x23) /* 4003 */
#define OMP_O_MAP_C_DEACTIVATE_TRACE_MODE_RES			mapP_gsm_om(\x9F\x24) /* 4004 */
#define OMP_O_MAP_C_SEND_IMSI_ARG				mapP_gsm_om(\x9F\x25) /* 4005 */
#define OMP_O_MAP_C_SEND_IMSI_RES				mapP_gsm_om(\x9F\x26) /* 4006 */
#define OMP_O_MAP_C_TRACE_DEPTH_LIST				mapP_gsm_om(\x9F\x27) /* 4007 */
#define OMP_O_MAP_C_TRACE_EVENT_LIST				mapP_gsm_om(\x9F\x28) /* 4008 */
#define OMP_O_MAP_C_TRACE_PROPAGATION_LIST			mapP_gsm_om(\x9F\x29) /* 4009 */
#define OMP_O_MAP_C_TRACING_BUFFER_FULL_PARAM			mapP_gsm_om(\x9F\x2A) /* 4010 */

/* OM attribute names (prefixed MAP_) */
#define MAP_BMSC_LIST						((OM_type)4000)
#define MAP_BMSC_TRACE_DEPTH					((OM_type)4001)
#define MAP_GGSN_LIST						((OM_type)4002)
#define MAP_GGSN_TRACE_DEPTH					((OM_type)4003)
#define MAP_MGW_INTERFACE_LIST					((OM_type)4004)
#define MAP_MGW_LIST						((OM_type)4005)
#define MAP_MGW_TRACE_DEPTH					((OM_type)4006)
#define MAP_MSC_S_EVENT_LIST					((OM_type)4007)
#define MAP_MSC_S_INTERFACE_LIST				((OM_type)4008)
#define MAP_MSC_S_LIST						((OM_type)4009)
#define MAP_MSC_S_TRACE_DEPTH					((OM_type)4010)
#define MAP_OMC_ID						((OM_type)4011)
#define MAP_RNC_INTERFACE_LIST					((OM_type)4012)
#define MAP_RNC_TRACE_DEPTH					((OM_type)4013)
#define MAP_SGSN_LIST						((OM_type)4014)
#define MAP_SGSN_TRACE_DEPTH					((OM_type)4015)
#define MAP_TRACE_DEPTH_LIST					((OM_type)4016)
#define MAP_TRACE_EVENT_LIST					((OM_type)4017)
#define MAP_TRACE_INTERFACE_LIST				((OM_type)4018)
#define MAP_TRACE_NE_TYPE_LIST					((OM_type)4019)
#define MAP_TRACE_RECORDING_SESSION_REFERENCE			((OM_type)4020)
#define MAP_TRACE_REFERENCE2					((OM_type)4021)
#define MAP_TRACE_REFERENCE					((OM_type)4022)
#define MAP_TRACE_SUPPORT_INDICATOR				((OM_type)4023)
#define MAP_TRACE_TYPE						((OM_type)4024)


/* Service-Error problem values: */
#define MAP_E_TRACING_BUFFER_FULL				mapP_problem(40)

/* MAP_T_Trace_Depth: */
typedef enum {
	MAP_T_MINIMUM = 0,
	MAP_T_MEDIUM,
	MAP_T_MAXIMUM,
MAP_T_Trace_Depth;

/* MAP_T_Trace_NE_Type_List: (bits) */
typedef enum {
	MAP_T_TRACE_TYPE_MSC_S = 0, /* TRACE_TYPE for uniqueness */
	MTP_T_TRACE_TYPE_MGW,
	MAP_T_TRACE_TYPE_SGSN,
	MAP_T_TRACE_TYPE_GGSN,
	MAP_T_TRACE_TYPE_RNC,
	MAP_T_TRACE_TYPE_BM_SC,
} MAP_T_Trace_NE_Type_List;

/* MAP_T_MSC_S_Interface_List: (bits) */
typedef enum {
	MAP_T_IF_MSC_A = 0, /* IF_MSC added for uniqueness */
	MAP_T_IF_MSC_IU,
	MAP_T_IF_MSC_MC,
	MAP_T_IF_MSC_MAP_G,
	MAP_T_IF_MSC_MAP_B,
	MAP_T_IF_MSC_MAP_E,
	MAP_T_IF_MSC_MAP_F,
	MAP_T_IF_MSC_CAP,
	MAP_T_IF_MSC_MAP_D,
	MAP_T_IF_MSC_MAP_C,
} MAP_T_MSC_S_Interface_List;

/* MAP_T_MGW_Interface_List: (bits) */
typedef enum {
	MAP_T_IF_MGW_MC = 0, /* IF_MGW added for uniqueness */
	MAP_T_IF_MGW_NB_UP,
	MAP_T_IF_MGW_IU_UP,
} MAP_T_MGW_Interface_List;

/* MAP_T_SGSN_Interface_List: (bits) */
typedef enum {
	MAP_T_IF_SGSN_GB = 0, /* IF_SGSN added for uniqueness */
	MAP_T_IF_SGSN_IU,
	MAP_T_IF_SGSN_GN,
	MAP_T_IF_SGSN_MAP_GR,
	MAP_T_IF_SGSN_MAP_GD,
	MAP_T_IF_SGSN_MAP_GF,
	MAP_T_IF_SGSN_GS,
	MAP_T_IF_SGSN_GE,
} MAP_T_SGSN_Interface_List;

/* MAP_T_GGSN_Interface_List: (bits) */
typedef enum {
	MAP_T_IF_GGSN_GN = 0, /* IF_GGSN added for uniqueness */
	MAP_T_IF_GGSN_GI,
	MAP_T_IF_GGSN_GMB,
} MAP_T_GGSN_Interface_List;

/* MAP_T_RNC_Interface_List: (bits) */
typedef enum {
	MAP_T_IF_RNC_IU = 0, /* IF_RNC added for uniqueness */
	MAP_T_IF_RNC_IUR,
	MAP_T_IF_RNC_IUB,
	MAP_T_IF_RNC_UU,
} MAP_T_RNC_Interface_List;

/* MAP_T_BMSC_Interface_List: (bits) */
typedef enum {
	MAP_T_IF_BMSC_GMB = 0, /* IF_BMSC added for uniqueness */
} MAP_T_BMSC_Interface_List;

/* MAP_T_MSC_S_Event_List: (bits) */
typedef enum {
	MAP_T_EV_MSC_S_MO_MT_CALL = 0, /* EV_MSC_S added for uniqueness */
	MAP_T_EV_MSC_S_MT_SMS,
	MAP_T_EV_MSC_S_LU_IMSI_ATTACH_IMSI_DETACH,
	MAP_T_EV_MSC_S_HANDOVERS,
	MAP_T_EV_MSC_S_SS,
} MAP_T_MSC_S_Event_List;

/* MAP_T_MGW_Event_List: (bits) */
typedef enum {
	MAP_T_EV_MGW_CONTEXT = 0, /* EV_MGW added for uniqueness */
} MAP_T_MGW_Event_List;

/* MAP_T_SGSN_Event_List: (bits) */
typedef enum {
	MAP_T_EV_SGSN_PDP_CONTEXT = 0, /* EV_SGSN added for uniqueness */
	MAP_T_EV_SGSN_MO_MT_SMS,
	MAP_T_EV_SGSN_RAU_GPRS_ATTACH_GPRS_DETACH,
	MAP_T_EV_SGSN_MB_MS_CONTEXT,
} MAP_T_SGSN_Event_List;

/* MAP_T_GGSN_Event_List: (bits) */
typedef enum {
	MAP_T_EV_GGSN_PDP_CONTEXT = 0, /* EV_GGSN added for uniqueness */
	MAP_T_EV_GGSN_MB_MS_CONTEXT,
} MAP_T_GGSN_Event_List;

/* MAP_T_BMSC_Event_List: (bits) */
typedef enum {
	MAP_T_MB_MS_MULTICAST_SERVICE_ACTIVATION = 0,
} MAP_T_BMSC_Event_List;

#endif				/* __XMAP_GSM_OM_H__ */

