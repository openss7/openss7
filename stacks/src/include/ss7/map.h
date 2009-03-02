/*****************************************************************************

 @(#) $Id: map.h,v 0.9.2.1 2009-03-02 05:14:54 brian Exp $

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

 Last Modified $Date: 2009-03-02 05:14:54 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: map.h,v $
 Revision 0.9.2.1  2009-03-02 05:14:54  brian
 - updates

 *****************************************************************************/

#ifndef __SS7_MAP_H__
#define __SS7_MAP_H__

#ident "@(#) $RCSfile: map.h,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) Copyright (c) 2008-2009 Monavacon Limited."

#define MAP_INFO_REQ		0	/* Information request */
#define MAP_BIND_REQ		1	/* Bind to network address */
#define MAP_BIND_ACK
#define MAP_UNBIND_REQ		2	/* Unbind from network address */
#define MAP_OPTMGMT_REQ		5	/* Options management request */
#define MAP_OK_ACK
#define MAP_ERROR_ACK

#define MAP_OPEN_REQ
#define MAP_OPEN_IND
#define MAP_OPEN_RES
#define MAP_OPEN_CON

#define MAP_XXX_REQ
#define MAP_XXX_IND
#define MAP_XXX_RES
#define MAP_XXX_CON

#define MAP_DELIM_REQ
#define MAP_DELIM_IND

#define MAP_CLOSE_REQ
#define MAP_CLOSE_IND
#define MAP_ABORT_IND
#define MAP_UABORT_IND
#define MAP_PABORT_IND
#define MAP_NOTICE_IND

#define MAP_STC1_REQ
#define MAP_STC1_IND
#define MAP_STC2_REQ
#define MAP_STC2_IND
#define MAP_STC3_REQ
#define MAP_STC3_IND
#define MAP_STC4_REQ
#define MAP_STC4_IND

typedef struct MAP_info_req {
	ulong PRIM_type; /* always MAP_INFO_REQ */
} MAP_info_req_t;

typedef struct MAP_info_ack {
	ulong PRIM_type; /* always MAP_INFO_ACK */
	ulong TSDU_size; /* maximum TSDU size */
	ulong ETSDU_size; /* maximum ETSDU size */
	ulong CDATA_size; /* connect data size */
	ulong DDATA_size; /* disconnect data size */
	ulong ADDR_size; /* address size */
	ulong ADDR_length; /* address length */
	ulong ADDR_offset; /* address offset */
	ulong QOS_length; /* default QOS values length */
	ulong QOS_offset; /* default QOS values offset */
	ulong QOS_range_length; /* QOS range length */
	ulong QOS_range_offset; /* QOS range offset */
	ulong OPTIONS_flags; /* bit masking for options */
	ulong TIDU_size; /* transaction interface data size */
	ulong SERV_type; /* service type */
	ulong CURRENT_state; /* current state */
	ulong PROVIDER_type; /* type of provider */
	ulong NODU_size; /* optimal TSDU size */
	ulong PROTOID_length; /* length of bound protocol ids */
	ulong PROTOID_offset; /* offset of bound protocol ids */
	ulong MAPI_version; /* supported MAPI version number */
} MAP_info_ack_t;

typedef struct MAP_bind_req {
	ulong PRIM_type; /* always MAP_BIND_REQ */
	ulong ADDR_length; /* length of protocol address */
	ulong ADDR_offset; /* offset of protocol address */
	ulong DIAIND_number; /* requested number of dialogue indications to be queued */
} MAP_bind_req_t;

typedef struct MAP_BIND_ack {
} MAP_bind_ack_t;

#endif				/* __SS7_MAP_H__ */

