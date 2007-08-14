/*****************************************************************************

 @(#) $Id: sua_msg.h,v 0.9.2.4 2007/08/14 12:18:52 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

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

 Last Modified $Date: 2007/08/14 12:18:52 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sua_msg.h,v $
 Revision 0.9.2.4  2007/08/14 12:18:52  brian
 - GPLv3 header updates

 Revision 0.9.2.3  2007/06/17 01:56:31  brian
 - updates for release, remove any later language

 *****************************************************************************/

#ifndef __SUA_MSG_H__
#define __SUA_MSG_H__

#ident "@(#) $RCSfile: sua_msg.h,v $ $Name:  $($Revision: 0.9.2.4 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

#include "../ua/ua_msg.h"
/*
 *  =========================================================================
 *
 *  SUA MESSAGE DEFINITIONS:-
 *
 *  =========================================================================
 *
 *  SUA-Specific Messages: per draft-ietf-sigtran-sua-07.txt
 *  -------------------------------------------------------------------
 */
#define SUA_CNLS_CLDT		UA_MHDR(1, 0, UA_CLASS_CNLS, 0x01)
#define SUA_CNLS_CLDR		UA_MHDR(1, 0, UA_CLASS_CNLS, 0x02)
#define SUA_CNLS_LAST		0x02

#define SUA_CONS_CORE		UA_MHDR(1, 0, UA_CLASS_CONS, 0x01)
#define SUA_CONS_COAK		UA_MHDR(1, 0, UA_CLASS_CONS, 0x02)
#define SUA_CONS_COREF		UA_MHDR(1, 0, UA_CLASS_CONS, 0x03)
#define SUA_CONS_RELRE		UA_MHDR(1, 0, UA_CLASS_CONS, 0x04)
#define SUA_CONS_RELCO		UA_MHDR(1, 0, UA_CLASS_CONS, 0x05)
#define SUA_CONS_RESCO		UA_MHDR(1, 0, UA_CLASS_CONS, 0x06)
#define SUA_CONS_RESRE		UA_MHDR(1, 0, UA_CLASS_CONS, 0x07)
#define SUA_CONS_CODT		UA_MHDR(1, 0, UA_CLASS_CONS, 0x08)
#define SUA_CONS_CODA		UA_MHDR(1, 0, UA_CLASS_CONS, 0x09)
#define SUA_CONS_COERR		UA_MHDR(1, 0, UA_CLASS_CONS, 0x0a)
#define SUA_CONS_COIT		UA_MHDR(1, 0, UA_CLASS_CONS, 0x0b)
#define SUA_CONS_LAST		0x0b
/*
 *  SUA-Specific Parameters: per draft-ietf-sigtran-sua-07.txt
 *  -------------------------------------------------------------------
 */
#define SUA_PARM_FLAGS		UA_PHDR(0x0101,sizeof(uint32_t))
#define SUA_PARM_SRCE_ADDR	UA_PHDR(0x0102,sizeof(uint32_t)*5)	/* XXX */
#define SUA_PARM_DEST_ADDR	UA_PHDR(0x0103,0)
#define SUA_PARM_SRN		UA_PHDR(0x0104,sizeof(uint32_t))
#define SUA_PARM_DRN		UA_PHDR(0x0105,sizeof(uint32_t))
#define SUA_PARM_CAUSE		UA_PHDR(0x0106,sizeof(uint32_t))
#define SUA_PARM_SEQ_NUM	UA_PHDR(0x0107,sizeof(uint32_t))
#define SUA_PARM_RX_SEQ_NUM	UA_PHDR(0x0108,sizeof(uint32_t))
#define SUA_PARM_ASP_CAPS	UA_PHDR(0x0109,sizeof(uint32_t))
#define SUA_PARM_CREDIT		UA_PHDR(0x010a,sizeof(uint32_t))
#define SUA_PARM_RESERVED1	UA_PHDR(0x010b,0)
#define SUA_PARM_SMI_SUBSYS	UA_PHDR(0x010c,sizeof(uint32_t))

#define SUA_SPARM_GT		UA_PHDR(0x8001,0)
#define SUA_SPARM_PC		UA_PHDR(0x8002,sizeof(uint32_t))
#define SUA_SPARM_SSN		UA_PHDR(0x8003,sizeof(uint32_t))
#define SUA_SPARM_IPV4_ADDR	UA_PHDR(0x8004,sizeof(uint32_t))
#define SUA_SPARM_HOSTNAME	UA_PHDR(0x8005,0)
#define SUA_SPARM_IPV6_ADDR	UA_PHDR(0x8006,sizeof(uint32_t)*4)

/*
 *  Routing Indicator used with SUA_PARM_SRCE_ADDR and SUA_PARM_DEST_ADDR.
 */
#define SUA_RI_GT	0x01
#define SUA_RI_PC_SSN	0x02
#define SUA_RI_HOSTNAME	0x03
#define SUA_RI_SSN_IP	0x04
/*
 *  Address Indicator flags used with addresses.
 */
#define SUA_AI_SSN	0x01
#define SUA_AI_PC	0x02
#define SUA_AI_GT	0x04

#define SUA_SPARM_BASE 0x0801
typedef struct sua_sparms {
	parm_t gt;
	parm_t pc;
	parm_t ssn;
	parm_t ipv4_addr;
	parm_t hostname;
	parm_t ipv6_addr;
#define SUA_SPARM_MAX 5
} sua_sparms_t;

#define SUA_PARM_BASE 0x0101
typedef struct sua_parms {
	ua_parms_t common;
	parm_t flags;
	parm_t srce_addr;
	parm_t dest_addr;
	parm_t srn;
	parm_t drn;
	parm_t cause;
	parm_t seq_num;
	parm_t rx_seq_num;
	parm_t asp_caps;
	parm_t credit;
	parm_t reserved1;
	parm_t smi_subsys;
#define SUA_PARM_MAX 12
	sua_sparms_t srce;
	sua_sparms_t dest;
} sua_parms_t;

#endif				/* __SUA_MSG_H__ */
