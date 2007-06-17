/*****************************************************************************

 @(#) $Id: m3ua_msg.h,v 0.9.2.2 2007/06/17 02:00:50 brian Exp $

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

 Last Modified $Date: 2007/06/17 02:00:50 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: m3ua_msg.h,v $
 Revision 0.9.2.2  2007/06/17 02:00:50  brian
 - updates for release, remove any later language

 *****************************************************************************/

#ifndef __M3UA_MSG_H__
#define __M3UA_MSG_H__

#ident "@(#) $RCSfile: m3ua_msg.h,v $ $Name:  $($Revision: 0.9.2.2 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

#include "../ua/ua_msg.h"
/*
 *  =========================================================================
 *
 *  M3UA MESSAGE DEFINITIONS:-
 *
 *  =========================================================================
 *
 *  M3UA-Specific Messages: per draft-ietf-sigtran-m3ua-08.txt
 *  -------------------------------------------------------------------
 */
#define M3UA_XFER_DATA		UA_MHDR(1, 0, UA_CLASS_XFER, 0x01)
#define M3UA_XFER_LAST		0x01

/*
 *  M3UA-Specific Parameters: per draft-ietf-sigtran-m3ua-08.txt
 *  -------------------------------------------------------------------
 */
#define M3UA_PARM_NTWK_APP	UA_PHDR(0x0200,sizeof(uint32_t))
#define M3UA_PARM_PROT_DATA1	UA_PHDR(0x0201,0)
#define M3UA_PARM_PROT_DATA2	UA_PHDR(0x0202,0)
#define M3UA_PARM_AFFECT_DEST	UA_PHDR(0x0203,sizeof(uint32_t))
#define M3UA_PARM_USER_CAUSE	UA_PHDR(0x0204,sizeof(uint32_t))
#define M3UA_PARM_CONG_IND	UA_PHDR(0x0205,sizeof(uint32_t))
#define M3UA_PARM_CONCERN_DEST	UA_PHDR(0x0206,sizeof(uint32_t))
#define M3UA_PARM_ROUTING_KEY	UA_PHDR(0x0207,0)
#define M3UA_PARM_REG_RESULT	UA_PHDR(0x0208,sizeof(uint32_t))
#define M3UA_PARM_DEREG_RESULT	UA_PHDR(0x0209,sizeof(uint32_t))
#define M3UA_PARM_LOC_KEY_ID	UA_PHDR(0x020a,sizeof(uint32_t))
#define M3UA_PARM_DPC		UA_PHDR(0x020b,0)
#define M3UA_PARM_SI		UA_PHDR(0x020c,0)
#define M3UA_PARM_SSN		UA_PHDR(0x020d,0)
#define M3UA_PARM_OPC		UA_PHDR(0x020e,0)
#define M3UA_PARM_CIC		UA_PHDR(0x020f,0)
#define M3UA_PARM_PROT_DATA3	UA_PHDR(0x0210,0)	/* proposed */

#define M3UA_PARM_BASE 0x0200
typedef struct m3ua_parms {
	ua_parms_t common;
	parm_t ntwk_app;
	parm_t prot_data1;
	parm_t prot_data2;
	parm_t affect_dest;
	parm_t user_cause;
	parm_t cong_ind;
	parm_t concern_dest;
	parm_t routing_key;
	parm_t reg_result;
	parm_t dereg_result;
	parm_t loc_key_id;
	parm_t dpc;
	parm_t si;
	parm_t ssn;
	parm_t opc;
	parm_t cic;
#define M3UA_PARM_MAX 15
} m3ua_parms_t;

extern int m3ua_decode_parms(mblk_t *, m3ua_parms_t *);

#endif				/* __M3UA_MSG_H__ */
