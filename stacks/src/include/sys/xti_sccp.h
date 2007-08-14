/*****************************************************************************

 @(#) $Id: xti_sccp.h,v 0.9.2.7 2007/08/14 12:17:13 brian Exp $

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

 Last Modified $Date: 2007/08/14 12:17:13 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: xti_sccp.h,v $
 Revision 0.9.2.7  2007/08/14 12:17:13  brian
 - GPLv3 header updates

 Revision 0.9.2.6  2007/02/13 14:05:30  brian
 - corrected ulong and long for 32-bit compat

 Revision 0.9.2.5  2006/09/25 12:10:09  brian
 - updated and doxygenified headers

 *****************************************************************************/

#ifndef _SYS_XTI_SCCP_H
#define _SYS_XTI_SCCP_H

#ident "@(#) $RCSfile: xti_sccp.h,v $ $Name:  $($Revision: 0.9.2.7 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* This file can be processed with doxygen(1). */

/** @addtogroup xnet
  * @{ */

/** @file
  * XTI SCCP-Specific header file.
  *
  * This file contains specific SCCP option and management parameters.  */

#ifndef t_uscalar_t
#define t_uscalar_t	u_int32_t
#define t_scalar_t	int32_t
#endif

#ifdef SCCP_MAX_ADDR_LENGTH
#undef SCCP_MAX_ADDR_LENGTH
#endif
#define SCCP_MAX_ADDR_LENGTH	32

/** SCCP Address structure. */
typedef struct sockaddr_sccp {
	sa_family_t sccp_family;	/**< Address family. */
	uint8_t sccp_ni;		/**< Network indicator. */
	uint8_t sccp_ri;		/**< Routing indicator. */
	uint32_t sccp_pc;		/**< Point code. */
	uint8_t sccp_ssn;		/**< Subsystem number (0 not present). */
	uint8_t sccp_gtt;		/**< Global title translation type. */
	uint8_t sccp_nai;		/**< Nature of address indicator. */
	uint8_t sccp_es;		/**< Encoding scheme. */
	uint8_t sccp_nplan;		/**< Numbering plan. */
	uint8_t sccp_tt;		/**< Translation type. */
	uint8_t sccp_alen;		/**< Address length. */
	uint8_t sccp_addr[SCCP_MAX_ADDR_LENGTH]; /**< Address digits. */
} t_sccp_addr_t;

/** @name Routing Indicators
  * @{ */
#define SCCP_RI_DPC_SSN		 0	/**< Route on DPC/SSN. */
#define SCCP_RI_GT		 1	/**< Route on GT. */
/** @} */

/** @name Subsystem Numbers
  * @{ */
#define SCCP_SSN_UNKNOWN	 0	/**< Subsystem unknown. */
#define SCCP_SSN_SCMG		 1	/**< SCCP management. */
#define SCCP_SSN_RESERVED	 2	/**< Reserved. */
#define SCCP_SSN_ISUP		 3	/**< ISUP subsystem. */
#define SCCP_SSN_OMAP		 4	/**< OMAP subsystem. */
#define SCCP_SSN_MAP		 5	/**< MAP subsystem. */
#define SCCP_SSN_HLR		 6	/**< HLR subsystem. */
#define SCCP_SSN_VLR		 7	/**< VLR subsystem. */
#define SCCP_SSN_MSC		 8	/**< MSC subsystem. */
#define SCCP_SSN_EIC		 9	/**< EIC subsystem. */
#define SCCP_SSN_AUC		10	/**< AUC subsystem. */
#define SCCP_SSN_ISDN_SS	11	/**< ISDN Special Services subsystem. */
#define SCCP_SSN_RESERVED2	12	/**< Reserved. */
#define SCCP_SSN_BISDN		13	/**< Broadband ISDN subsystem. */
#define SCCP_SSN_TC_TEST	14	/**< Transaction Capabilities Test subsystem. */
/** @} */

/** @name Global Title Types
  * @{ */
#define SCCP_GTTTYPE_NONE	 0	/**< No GT. */
#define SCCP_GTTTYPE_NAI	 1	/**< NAI only. */
#define SCCP_GTTTYPE_TT		 2	/**< TT only. */
#define SCCP_GTTTYPE_NP		 3	/**< TT, ES, NPLAN. */
#define SCCP_GTTTYPE_NP_NAI	 4	/**< TT, ES, NPLAN and NAI. */
/** @} */

/** @name Encoding Schemes
  * @{ */
#define SCCP_ES_UNKNOWN		 0	/**< Encoding scheme unknown. */
#define SCCP_ES_BCD_ODD		 1	/**< Encoding Binary Coded Decimal, odd digits. */
#define SCCP_ES_BCD_EVEN	 2	/**< Encoding Binary Coded Decimal, even digits. */
#define SCCP_ES_NATIONAL	 3	/**< Encoding National-Specific. */
/** @} */

/** @name Numbering Plan
  * @{ */
#define SCCP_NPLAN_UNKNOWN	 0	/**< Numbering plan unknown. */
#define SCCP_NPLAN_ISDN		 1	/**< ISDN numbering plan. */
#define SCCP_NPLAN_GENERIC	 2	/**< Generic numbering plan. */
#define SCCP_NPLAN_DATA		 3	/**< Data numbering plan. */
#define SCCP_NPLAN_TELEX	 4	/**< Telex numbering plan. */
#define SCCP_NPLAN_MARITIME	 5	/**< Maritime numbering plan. */
#define SCCP_NPLAN_LAND_MOBILE	 6	/**< Land mobile numbering plan. */
#define SCCP_NPLAN_ISDN_MOBILE	 7	/**< ISDN monile numbering plan. */
#define SCCP_NPLAN_PRIVATE	 8	/**< Private numbering plan. */
/** @} */

/** XTI SCCP Option Level */
#define T_SS7_SCCP		3	/**< SCCP level (same as SI value). */

/** @name SCCP Transport Provider Options
  * @{ */
#define T_SCCP_PVAR		1	/**< Protocol variant. */
#define T_SCCP_MPLEV		2	/**< Protocol options. */
#define T_SCCP_DEBUG		3	/**< Debug. */

#define T_SCCP_CLUSTER		7	/**< Protocol option. */
#define T_SCCP_SEQ_CTRL		8	/**< Sequence control parameter. */
#define T_SCCP_PRIORITY		9	/**< Message priority. */

#define T_SCCP_PCLASS		10	/**< Protocol class. */
#define T_SCCP_IMPORTANCE	11	/**< Importance. */
#define T_SCCP_RET_ERROR	12	/**< Return on error. */
/** @} */

/** @name Protocl Classes
  * For use with T_SCCP_PCLASS.
  * @{ */
#define T_SCCP_PCLASS_0		0x01	/**< Protocol class 0. */
#define T_SCCP_PCLASS_1		0x02	/**< Protocol class 1. */
#define T_SCCP_PCLASS_2		0x04	/**< Protocol class 2. */
#define T_SCCP_PCLASS_3		0x08	/**< Protocol class 3. */
#define T_SCCP_PCLASS_ALL	0x0f	/**< Protocol class any. */
/** @} */

/** @name SCCP Disconnect Reaons and Unidata Errors
  * @{ */
#define T_SCCP_CLUST			(0x0100)	/**< Cluster modifier. */
#define T_SCCP_DEST_AVAILABLE		(0x1001)	/**< Destination available. */
#define T_SCCP_DEST_PROHIBITED		(0x1002)	/**< Destination prohibitied. */
#define T_SCCP_DEST_CONGESTED		(0x1003)	/**< Destination congested. */
#define T_SCCP_CLUS_AVAILABLE		(T_SCCP_DEST_AVAILABLE |T_SCCP_CLUST)	/**< Cluster available. */
#define T_SCCP_CLUS_PROHIBITED		(T_SCCP_DEST_PROHIBITED|T_SCCP_CLUST)	/**< Cluster prohibited. */
#define T_SCCP_CLUS_CONGESTED		(T_SCCP_DEST_CONGESTED |T_SCCP_CLUST)	/**< Cluster congested. */
#define T_SCCP_RESTARTING		(0x1004)	/**< MTP Restart in progress. */
#define T_SCCP_USER_PART_UNKNOWN	(0x1005)	/**< MTP User Part unknown. */
#define T_SCCP_USER_PART_UNEQUIPPED	(0x1006)	/**< MTP User Part unequipped. */
#define T_SCCP_USER_PART_UNAVAILABLE	(0x1007)	/**< MTP User Part unavailable. */
/** @} */

/** @} */

#endif				/* _SYS_XTI_SCCP_H */

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
