/*****************************************************************************

 @(#) $RCSfile: tua_msg.h,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2008-04-29 01:52:22 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

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

 Last Modified $Date: 2008-04-29 01:52:22 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: tua_msg.h,v $
 Revision 0.9.2.4  2008-04-29 01:52:22  brian
 - updated headers for release

 Revision 0.9.2.3  2007/08/14 08:33:55  brian
 - GPLv3 header update

 Revision 0.9.2.2  2007/06/17 02:00:51  brian
 - updates for release, remove any later language

 *****************************************************************************/

#ifndef __TUA_MSG_H__
#define __TUA_MSG_H__

#ident "@(#) $RCSfile: tua_msg.h,v $ $Name:  $($Revision: 0.9.2.4 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

/*
 *  =========================================================================
 *
 *  TUA MESSAGE DEFINITIONS:-
 *
 *  =========================================================================
 *
 *  TUA-Specific Messages: per draft-hou-sigtran-tua-00.txt
 *  -------------------------------------------------------------------
 */
#define TUA_TDHM_UNI		UA_MHDR(1, 0, UA_CLASS_TDHM, 0x00)
#define TUA_TDHM_BEG		UA_MHDR(1, 0, UA_CLASS_TDHM, 0x01)
#define TUA_TDHM_CON		UA_MHDR(1, 0, UA_CLASS_TDHM, 0x02)
#define TUA_TDHM_END		UA_MHDR(1, 0, UA_CLASS_TDHM, 0x03)
#define TUA_TDHM_U_ABT		UA_MHDR(1, 0, UA_CLASS_TDHM, 0x04)
#define TUA_TDHM_P_ABT		UA_MHDR(1, 0, UA_CLASS_TDHM, 0x05)
#define TUA_TDHM_NOT		UA_MHDR(1, 0, UA_CLASS_TDHM, 0x06)
#define TUA_TDHM_LAST		0x06

#define TUA_TCHM_IVK		UA_MHDR(1, 0, UA_CLASS_TCHM, 0x00)
#define TUA_TCHM_RES		UA_MHDR(1, 0, UA_CLASS_TCHM, 0x01)
#define TUA_TCHM_U_ERR		UA_MHDR(1, 0, UA_CLASS_TCHM, 0x02)
#define TUA_TCHM_REJ		UA_MHDR(1, 0, UA_CLASS_TCHM, 0x03)
#define TUA_TCHM_LAST		0x03
/*
 *  TUA-Specific Parameters: per draft-hou-sigtran-tua-00.txt
 *  -------------------------------------------------------------------
 */
#define TUA_PARM_QOS		UA_PHDR(0x0101,sizeof(uint32_t))
#define TUA_PARM_DEST_ADDR	UA_PHDR(0x0102,0)
#define TUA_PARM_ORIG_ADDR	UA_PHDR(0x0103,0)
#define TUA_PARM_APPL_CTXT	UA_PHDR(0x0104,0)
#define TUA_PARM_USER_INFO	UA_PHDR(0x0105,0)
#define TUA_PARM_COMP_PRES	UA_PHDR(0x0106,sizeof(uint32_t))
#define TUA_PARM_TERM		UA_PHDR(0x0107,sizeof(uint32_t))
#define TUA_PARM_P_ABORT	UA_PHDR(0x0108,sizeof(uint32_t))
#define TUA_PARM_REPT_CAUS	UA_PHDR(0x0109,sizeof(uint32_t))

#define TUA_PARM_INVK_ID	UA_PHDR(0x0201,sizeof(uint32_t))
#define TUA_PARM_LAST_COMP	UA_PHDR(0x0202,sizeof(uint32_t))
#define TUA_PARM_TIMEOUT	UA_PHDR(0x0203,sizeof(uint32_t))
#define TUA_PARM_OPCODE		UA_PHDR(0x0204,sizeof(uint32_t)*2)
#define TUA_PARM_PARMS		UA_PHDR(0x0205,0)
#define TUA_PARM_ERROR		UA_PHDR(0x0206,sizeof(uint32_t))
#define TUA_PARM_PBCODE		UA_PHDR(0x0207,sizeof(uint32_t))
#define TUA_PARM_CORR_ID	UA_PHDR(0x0208,sizeof(uint32_t))

#define TUA_PARM_SECU_CTXT	UA_PHDR(0x0301,0)
#define TUA_PARM_CONFIDENT	UA_PHDR(0x0302,0)
#define TUA_PARM_UABT_INFO	UA_PHDR(0x0303,0)

#define TUA_PARM_ABCODE		UA_PHDR(0x0401,sizeof(uint32_t))
#define TUA_PARM_CLASS		UA_PHDR(0x0402,sizeof(uint32_t))

#define TUA_PARM_CNV_TYPE	UA_PHDR(0x0503,sizeof(uint32_t))
#define TUA_PARM_INV_TYPE	UA_PHDR(0x0504,sizeof(uint32_t))
#define TUA_PARM_REJ_TYPE	UA_PHDR(0x0505,sizeof(uint32_t))
#define TUA_PARM_QRY_TYPE	UA_PHDR(0x0506,sizeof(uint32_t))

#define TUA_DPARM_BASE 0x0101
typedef struct tua_dparms {
	parm_t qos;
	parm_t dest_addr;
	parm_t orig_addr;
	parm_t appl_ctxt;
	parm_t user_info;
	parm_t comp_pres;
	parm_t term;
	parm_t p_abort;
	parm_t rept_caus;
#define TUA_DPARM_MAX  8
} tua_dparms_t;

#define TUA_CPARM_BASE 0x0201
typedef struct tua_cparms {
	parm_t invk_id;
	parm_t last_comp;
	parm_t timeout;
	parm_t opcode;
	parm_t parms;
	parm_t error;
	parm_t pbcode;
	parm_t corr_id;
#define TUA_CPARM_MAX  7
} tua_cparms_t;

#define TUA_IPARM_BASE 0x0301
typedef struct tua_iparms {
	parm_t secu_ctxt;
	parm_t confident;
	parm_t uabt_info;
#define TUA_IPARM_MAX  2
} tua_iparms_t;

#define TUA_APARM_BASE 0x0401
parm_t abcode;
parm_t clas;
typedef struct tua_aparms {
#define TUA_APARM_MAX  1
} tua_aparms_t;

#define TUA_XPARM_BASE 0x0503
parm_t cnv_type;
parm_t inv_type;
parm_t rej_type;
parm_t qry_type;
typedef struct tua_xparms {
#define TUA_XPARM_MAX  3
} tua_xparms_t;

typedef struct sua_parms {
	ua_parms_t common;
	sua_dparms_t dparms;
	sua_cparms_t cparms;
	sua_iparms_t iparms;
	sua_aparms_t aparms;
	sua_xparms_t xparms;
} sua_parms_t;

#endif				/* __TUA_MSG_H__ */
