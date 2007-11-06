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

#ifndef __RFA_RFAINFO_H__
#define __RFA_RFAINFO_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/*
 * RFA - Remote File Access
 *
 * Access and Management for a partial file system tree that exists
 * at two sites either as master files or slave files
 *
 * rfainfo.h : data structures to represent content of ".rfainfo" files
 *             and stat info of files
 *
 * Contributed by Oliver Wenzel, GMD Berlin, 1990
 *
 * Header: /xtel/isode/isode/others/rfa/RCS/rfainfo.h,v 9.0 1992/06/16 12:47:25 isode Rel
 *
 * Log: rfainfo.h,v
 * Revision 9.0  1992/06/16  12:47:25  isode
 * Release 8.0
 *
 */

/*
 *                              NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */

#include <sys/types.h>

struct RfaInfo {
	char *ri_filename;
	int ri_status;
	time_t ri_lastChange;		/* time when status change was done */
	char *ri_lckname;		/* name of user that locked file */
	time_t ri_lcksince;		/* time when file has been locked */

	char *ri_owner;			/* file info as per stat(2) */
	char *ri_group;
	int ri_mode;
	int ri_size;
	time_t ri_modTime;
	time_t ri_accTime;
	char *ri_lnkName;

	struct RfaInfo *ri_next;
};

#define RI_STATUS(s)		((s) & 07)
#define SET_STATUS(v, s)	(v)=(((v) & ~07) | (s))
#define	RI_UNREGISTERED		0
#define	IS_UNREGISTERED(s)	(RI_STATUS(s) == RI_UNREGISTERED)
#define	RI_MASTER		1
#define	IS_MASTER(s)		(RI_STATUS(s) == RI_MASTER)
#define RI_SLAVE		2
#define	IS_SLAVE(s)		(RI_STATUS(s) == RI_SLAVE)

#define RI_LOCKINFO(s)		((s) & 070)
#define SET_LOCKINFO(v, s)	(v)=(((v) & ~070) | (s))
#define	RI_LOCKED		010
#define	IS_LOCKED(s)		(RI_LOCKINFO(s) == RI_LOCKED)
#define RI_UNLOCKED		020
#define	IS_UNLOCKED(s)		(RI_LOCKINFO(s) == RI_UNLOCKED)

#define RI_TRANSFER(s)		((s) & 0700)
#define SET_TRANSFER(v, s)	(v)=(((v) & ~0700) | (s))
#define RI_TR_AUTO		0100
#define	IS_TR_AUTO(s)		(RI_TRANSFER(s) == RI_TR_AUTO)
#define RI_TR_REQ		0200
#define	IS_TR_REQ(s)		(RI_TRANSFER(s) == RI_TR_REQ)

extern int getLockedRfaInfoList(), getRfaInfoList(), putRfaInfoList();
extern void remRfaInfo();
extern struct RfaInfo *mallocRfaInfo(), *findRfaInfo(), *extractRfaInfo();
extern int str2status();
extern char *status2str(), *status2sstr();

extern struct RfaInfo *fi2rfa();
extern struct type_RFA_FileInfoList *rfa2fil();
extern struct type_RFA_FileInfo *rfa2fi();

#endif				/* __RFA_RFAINFO_H__ */
