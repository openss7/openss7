/*****************************************************************************

 @(#) $Id: sad.h,v 0.9.2.12 2006/03/10 07:24:14 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>

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

 Last Modified $Date: 2006/03/10 07:24:14 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SYS_STREAMS_SAD_H__
#define __SYS_STREAMS_SAD_H__

#ident "@(#) $RCSfile: sad.h,v $ $Name:  $($Revision: 0.9.2.12 $) $Date: 2006/03/10 07:24:14 $"

#ifndef __SYS_SAD_H__
#warning "Do no include sys/streams/sad.h directly, include sys/sad.h instead."
#endif

#define SAD_IOC_MAGIC	'D'	/* Note: OSF/1 1.2 uses 'A' instead instead of 'D' */

#define SAD_SAP		((SAD_IOC_MAGIC << 8) | 0x01)	/* set autopush */
#define SAD_GAP		((SAD_IOC_MAGIC << 8) | 0x02)	/* get autopush */
#define SAD_VML		((SAD_IOC_MAGIC << 8) | 0x03)	/* validate modules */

#define SAD_SAP_SOL	((SAD_IOC_MAGIC << 8) | 0x17)	/* set autopush (anchor) */
#define SAD_GAP_SOL	((SAD_IOC_MAGIC << 8) | 0x18)	/* get autopush (anchor) */

#ifndef MAX_APUSH
#define MAX_APUSH 8
#endif

#define MAXAPUSH MAX_APUSH

#ifdef __LP64__
struct strapush32 {
	int32_t sap_cmd;
	int32_t sap_major;
	int32_t sap_minor;
	int32_t sap_lastminor;
	u_int32_t sap_npush;
	char sap_list[MAXAPUSH][FMNAMESZ + 1];
	/* Solaris adds sap_anchor which is the integer anchor position */
	int32_t sap_anchor;
	/* This one is Linux Fast-STREAMS specific */
	char sap_module[FMNAMESZ + 1];	/* This is mine. */
};
#endif				/* __LP64__ */
struct strapush {
	int sap_cmd;
	long sap_major;
	long sap_minor;
	long sap_lastminor;
	uint sap_npush;
	char sap_list[MAXAPUSH][FMNAMESZ + 1];
	/* Solaris adds sap_anchor which is the integer anchor position */
	int sap_anchor;
	/* This one is Linux Fast-STREAMS specific */
	char sap_module[FMNAMESZ + 1];	/* This is mine. */
};

#define SAP_CLEAR	0x00	/* clear entry */
#define SAP_ONE		0x01	/* add entry for one minor */
#define SAP_RANGE	0x02	/* add entry for range of minors */
#define SAP_ALL		0x03	/* add etnry for all minors */
#define SAP_CLONE	0x04	/* mark clonable minor device */

#endif				/* __SYS_STREAMS_SAD_H__ */
