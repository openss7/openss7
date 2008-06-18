/*****************************************************************************

 @(#) $Id: dlpi_aix.h,v 0.9.2.1 2008-06-18 16:43:16 brian Exp $

 -----------------------------------------------------------------------------

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

 Last Modified $Date: 2008-06-18 16:43:16 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: dlpi_aix.h,v $
 Revision 0.9.2.1  2008-06-18 16:43:16  brian
 - added new files for NLI and DLPI

 *****************************************************************************/

#ifndef __SYS_DLPI_AIX_H__
#define __SYS_DLPI_AIX_H__

#ident "@(#) $RCSfile: dlpi_aix.h,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

/*
 * This is a replacement for the AIX Data Link Provider Interface (DLPI)
 * extensions header file.  Information for the definitions in this file were
 * gleened (but not copied) from AIX documentation: see the related manual pages
 * for sources.
 */

#define NS_PROTO_DL_COMPAT	/* AIX 3.2.5 DLPI address format. */
#define NS_PROTO_DL_DONTCARE	/* No addresses in DL_UNITDATA_IND. */

#define DLAIX			(('D'<<8)+64)
#define DL_PKT_FORMAT		(DLAIX|0)
#define DL_INPUT_RESOLVE	(DLAIX|1)
#define DL_OUTPUT_RESOLVE	(DLAIX|2)
#define DL_ROUTE		(DLAIX|3)
#define DL_TUNE_LLC		(DLAIX|4)
#define DL_ZERO_STATS		(DLAIX|5)
#define DL_SET_REMADDR		(DLAIX|6)


/* for use with the DL_TUNE_LLC input-output control */

#define F_LLC_SET	(1UL<<31)
#define F_REPOLL_TIME	(1UL<< 0)
#define F_ACK_TIME	(1UL<< 1)
#define F_INACT_TIME	(1UL<< 2)
#define F_FORCE_TIME	(1UL<< 3)
#define F_MAXIF		(1UL<< 4)
#define F_XMIT_WND	(1UL<< 5)
#define F_MAX_REPOLL	(1UL<< 6)

typedef struct llctune {
	uint32_t flags;			/* flags field */
	/* the following fields and the corresponding flag names are wild guesses */
	uint32_t repoll_time;
	uint32_t ack_time;
	uint32_t inact_time;
	uint32_t force_time;
	uint32_t maxif;
	uint32_t xmit_wnd;
	uint32_t max_repoll;
} llctune_t;

#endif				/* __SYS_DLPI_AIX_H__ */

