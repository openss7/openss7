/*****************************************************************************

 @(#) sad.h,v 1.1.2.5 2003/10/20 11:23:02 brian Exp

 -----------------------------------------------------------------------------

 Copyright (C) 2001-2003  OpenSS7 Corporation <http://www.openss7.com>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

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
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified 2003/10/20 11:23:02 by brian

 *****************************************************************************/

#ifndef __SYS_SAD_H__
#define __SYS_SAD_H__

#define SAD_IOC_MAGIC	'D'
#define SAD_SAP		((SAD_IOC_MAGIC << 8) | 0x01)	/* set autopush */
#define SAD_GAP		((SAD_IOC_MAGIC << 8) | 0x02)	/* get autopush */
#define SAD_VML		((SAD_IOC_MAGIC << 8) | 0x03)	/* validate modules */

#ifndef MAX_APUSH
#define MAX_APUSH 8
#endif

#define MAXAPUSH MAX_APUSH

struct strapush {
	int sap_cmd;
	long sap_major;
	long sap_minor;
	long sap_lastminor;
	uint sap_npush;
	char sap_list[MAXAPUSH][FMNAMESZ + 1];
	char sap_module[FMNAMESZ + 1];
};

#define SAP_CLEAR	0x00		/* clear entry */
#define SAP_ONE		0x01		/* add entry for one minor */
#define SAP_RANGE	0x02		/* add entry for range of minors */
#define SAP_ALL		0x03		/* add etnry for all minors */
#define SAP_CLONE	0x04		/* mark clonable minor device */

#endif				/* __SYS_SAD_H__ */
