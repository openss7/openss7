/*****************************************************************************

 @(#) $Id$

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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ifndef __ISODE_QUIPU_NRS_INFO_H__
#define __ISODE_QUIPU_NRS_INFO_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* nrs_info.h - attribute structure for representing NRS information */

/* 
 * Header: /xtel/isode/isode/h/quipu/RCS/nrs_info.h,v 9.0 1992/06/16 12:23:11 isode Rel
 *
 *
 * Log: nrs_info.h,v
 * Revision 9.0  1992/06/16  12:23:11  isode
 * Release 8.0
 *
 */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */

#ifndef QUIPUNRSINFO
#define QUIPUNRSINFO

#include "psap.h"

struct str_seq {
	char *ss_str;
	struct str_seq *ss_next;
};

/* Defined values for contexts */
#define	NRS_Context_UNKNOWN	-1
#define	NRS_Context_X29	0
#define	NRS_Context_TS29	1
#define	NRS_Context_NIFTP	2
#define	NRS_Context_MAIL_NIFTP	3
#define	NRS_Context_NOT_USED	4
#define	NRS_Context_MAIL_TELEX	5
#define	NRS_Context_JTMP	6
#define	NRS_Context_JTMP_FILES	7
#define	NRS_Context_JTMP_REG	8
#define	NRS_Context_YBTS_NODE	9
#define	NRS_Context_YBTS	10
#define	NRS_Context_FTAM	11
#define	NRS_Context_JTM	12
#define	NRS_Context_JTM_REG	13
#define	NRS_Context_VT	14
#define	NRS_Context_MOTIS	15

/* Defined values for address space identifiers */
#define	NRS_Address_Space_Id_PSS	0
#define	NRS_Address_Space_Id_JANET	1
#define	NRS_Address_Space_Id_TELEX	2
#define	NRS_Address_Space_Id_OSI_CONS	3

struct addr_info {
	int addr_info_type;
#define	ADDR_INFO_DTE_ONLY			1
#define	ADDR_INFO_DTE_APPLIC_INFO		2
#define	ADDR_INFO_DTE_CUDF			3
#define	ADDR_INFO_DTE_CUDF_APPLIC_INFO		4
#define	ADDR_INFO_DTE_YBTS			5
#define	ADDR_INFO_DTE_YBTS_APPLIC_INFO		6
#define	ADDR_INFO_DTE_YBTS_APPLIC_RELAY		7
#define	ADDR_INFO_NONE_NEEDED			8
#define	ADDR_INFO_OSI_ADDRESSING		9
#define	ADDR_INFO_OSI_NSAP_ONLY			10
#define	ADDR_INFO_OSI_NSAP_APPLIC_INFO		11
#define	ADDR_INFO_OSI_NSAP_APPLIC_RELAY		12
#define	ADDR_INFO_DTE_YBTS_OSI_ADDRESSING	13

	char *dte_number;
	char *cudf;
	char *ybts_string;
	char *nsap;
	char *tselector;
	char *sselector;
	char *pselector;
	PE place_holder;
	PE application_title;
	PE per_app_context_info;
	struct str_seq *applic_info;
	struct str_seq *applic_relay;
};

struct nrs_routes {
	PE cost;
	struct addr_info *addr_info;
	struct nrs_routes *next;
};

struct nrs_info {
	int context;
	int addr_sp_id;
	struct nrs_routes *routes;
};

#endif

#endif				/* __ISODE_QUIPU_NRS_INFO_H__ */
