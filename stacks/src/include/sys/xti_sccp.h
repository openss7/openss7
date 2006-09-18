/*****************************************************************************

 @(#) $Id: xti_sccp.h,v 0.9.2.3 2006/03/03 12:06:09 brian Exp $

 -----------------------------------------------------------------------------

     Copyright (C) 1997-2002 OpenSS7 Corporation.  All Rights Reserved.

                                  PUBLIC LICENSE

     This license is provided without fee, provided that the above copy-
     right notice and this public license must be retained on all copies,
     extracts, compilations and derivative works.  Use or distribution of
     this work in a manner that restricts its use except as provided here
     will render this license void.

     The author(s) hereby waive any and all other restrictions in respect
     of their copyright in this software and its associated documentation.
     The authors(s) of this software place in the public domain any novel
     methods or processes which are embodied in this software.

     The author(s) undertook to write it for the sake of the advancement
     of the Arts and Sciences, but it is provided as is, and the author(s)
     will not take any responsibility in it.

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

 Last Modified $Date: 2006/03/03 12:06:09 $ by $Author: brian $

 *****************************************************************************/

#ifndef _SYS_XTI_SCCP_H
#define _SYS_XTI_SCCP_H

#ident "@(#) $Name:  $($Revision: 0.9.2.3 $) Copyright (c) 1997-2002 OpenSS7 Corporation."

/* This file can be processed with doxygen(1). */

#ifndef t_uscalar_t
#define t_uscalar_t	u_int32_t
#define t_scalar_t	int32_t
#endif

#ifdef SCCP_MAX_ADDR_LENGTH
#undef SCCP_MAX_ADDR_LENGTH
#endif
#define SCCP_MAX_ADDR_LENGTH	32

typedef struct sockaddr_sccp {
	sa_family_t sccp_family;		/* Address family */
	uint8_t sccp_ni;			/* Network indicator */
	uint8_t sccp_ri;			/* Routing indicator */
	uint32_t sccp_pc;			/* Point code */
	uint8_t sccp_ssn;			/* Subsystem number (0 not present) */
	uint8_t sccp_gtt;			/* Global title translation type */
	uint8_t sccp_nai;			/* Nature of address indicator */
	uint8_t sccp_es;			/* Encoding scheme */
	uint8_t sccp_nplan;			/* Numbering plan */
	uint8_t sccp_tt;			/* Translation type */
	uint8_t sccp_alen;			/* Address length */
	uint8_t sccp_addr[SCCP_MAX_ADDR_LENGTH];	/* address digits */
} t_sccp_addr_t;

#define SCCP_RI_DPC_SSN		 0	/* route on DPC/SSN */
#define SCCP_RI_GT		 1	/* route on GT */

#define SCCP_SSN_UNKNOWN	 0
#define SCCP_SSN_SCMG		 1
#define SCCP_SSN_RESERVED	 2
#define SCCP_SSN_ISUP		 3
#define SCCP_SSN_OMAP		 4
#define SCCP_SSN_MAP		 5
#define SCCP_SSN_HLR		 6
#define SCCP_SSN_VLR		 7
#define SCCP_SSN_MSC		 8
#define SCCP_SSN_EIC		 9
#define SCCP_SSN_AUC		10
#define SCCP_SSN_ISDN_SS	11
#define SCCP_SSN_RESERVED2	12
#define SCCP_SSN_BISDN		13
#define SCCP_SSN_TC_TEST	14

#define SCCP_GTTTYPE_NONE	 0	/* no GT */
#define SCCP_GTTTYPE_NAI	 1	/* NAI only */
#define SCCP_GTTTYPE_TT		 2	/* TT only */
#define SCCP_GTTTYPE_NP		 3	/* TT, ES, NPLAN */
#define SCCP_GTTTYPE_NP_NAI	 4	/* TT, ES, NPLAN and NAI */

#define SCCP_ES_UNKNOWN		 0
#define SCCP_ES_BCD_ODD		 1
#define SCCP_ES_BCD_EVEN	 2
#define SCCP_ES_NATIONAL	 3

#define SCCP_NPLAN_UNKNOWN	 0
#define SCCP_NPLAN_ISDN		 1
#define SCCP_NPLAN_GENERIC	 2
#define SCCP_NPLAN_DATA		 3
#define SCCP_NPLAN_TELEX	 4
#define SCCP_NPLAN_MARITIME	 5
#define SCCP_NPLAN_LAND_MOBILE	 6
#define SCCP_NPLAN_ISDN_MOBILE	 7
#define SCCP_NPLAN_PRIVATE	 8

#define T_SS7_SCCP		3	/* SCCP level (same as SI value) */

/*
 *  SCCP Transport Provider Options
 */
#define T_SCCP_PVAR		1	/* protocol variant */
#define T_SCCP_MPLEV		2	/* protocol options */
#define T_SCCP_DEBUG		3	/* debug */

#define T_SCCP_CLUSTER		7	/* protocol option */
#define T_SCCP_SEQ_CTRL		8	/* sequence control parameter */
#define T_SCCP_PRIORITY		9	/* message priority */

#define T_SCCP_PCLASS		10	/* protocol class */
#define T_SCCP_IMPORTANCE	11	/* importance */
#define T_SCCP_RET_ERROR	12	/* return on error */

#define T_SCCP_PCLASS_0		0x01
#define T_SCCP_PCLASS_1		0x02
#define T_SCCP_PCLASS_2		0x04
#define T_SCCP_PCLASS_3		0x08
#define T_SCCP_PCLASS_ALL	0x0f

#define T_SCCP_CLUST			(0x0100)
#define T_SCCP_DEST_AVAILABLE		(0x1001)
#define T_SCCP_DEST_PROHIBITED		(0x1002)
#define T_SCCP_DEST_CONGESTED		(0x1003)
#define T_SCCP_CLUS_AVAILABLE		(T_SCCP_DEST_AVAILABLE |T_SCCP_CLUST)
#define T_SCCP_CLUS_PROHIBITED		(T_SCCP_DEST_PROHIBITED|T_SCCP_CLUST)
#define T_SCCP_CLUS_CONGESTED		(T_SCCP_DEST_CONGESTED |T_SCCP_CLUST)
#define T_SCCP_RESTARTING		(0x1004)
#define T_SCCP_USER_PART_UNKNOWN	(0x1005)
#define T_SCCP_USER_PART_UNEQUIPPED	(0x1006)
#define T_SCCP_USER_PART_UNAVAILABLE	(0x1007)

#endif				/* _SYS_XTI_SCCP_H */
