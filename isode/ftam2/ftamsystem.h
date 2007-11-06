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

#ifndef __FTAM2_FTAMSYSTEM_H__
#define __FTAM2_FTAMSYSTEM_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* ftamsystem.h - include file for FTAM responder */

/* 
 * Header: /xtel/isode/isode/ftam2/RCS/ftamsystem.h,v 9.0 1992/06/16 12:15:43 isode Rel
 *
 *
 * Log: ftamsystem.h,v
 * Revision 9.0  1992/06/16  12:15:43  isode
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

#include <errno.h>
#include "ftamsbr.h"
#include "logger.h"

#define	SCPYN(a,b)	strncpy ((a), (b), sizeof (a))

/* SERVER */

extern int ftamfd;

extern int cflag;
extern int debug;
extern char *myname;

void ftam_adios(), ftam_advise(), ftam_diag();

void adios(), advise();

/* UNIX DATA */

extern int myuid;

extern int myhomelen;
extern char myhome[];

extern dev_t null_dev;
extern ino_t null_ino;

/* VFS DATA */

#define	NMAX	8		/* too painful to get right! */

#ifndef	NGROUPS
#define	NACCT	32
#else
#define	NACCT	(NGROUPS + 20)
#endif

extern struct vfsmap vfs[];		/* ordering affects default action in st2vfs() put
					   preferential entries towards the end */
#define	VFS_UBF	0		/* offset to FTAM-3 */
#define	VFS_UTF	1		/* ..  FTAM-1 */
#define	VFS_FDF	2		/* ..  NIST-9 */
#define VFS_FDF_OLD 3		/* ..  NBS-9 with old OID (prov.) */

/* REGIME DATA */

extern int level;
extern int ftam_class;
extern int units;
extern int attrs;
extern int fadusize;

/* ACTIVITY DATA */

extern int myfd;
extern char *myfile;
extern struct stat myst;
extern int statok;

extern struct vfsmap *myvf;		/* active contents type */
extern caddr_t myparam;			/* .. */

extern int myaccess;			/* current access request */

extern char *initiator;			/* current initiator identity */

extern struct FADUidentity mylocation;	/* current location */

extern int mymode;			/* current processing mode */
extern int myoperation;			/* .. */

#ifdef	notdef
extern AEI mycalling;			/* current calling AET */
extern AEI myresponding;		/* current responding AET */
#endif

extern char *account;			/* current account */
extern int mygid;			/* "inner" account */

extern int mylock;			/* current concurrency control */
extern struct FTAMconcurrency myconctl;	/* .. */

extern int mylockstyle;			/* current locking style */

extern int mycontext;			/* current access context */
extern int mylevel;			/* .. */

#ifndef	SYS5
#define	unlock()	if (mylock) (void) flock (myfd, LOCK_UN); else
#else
#define	unlock() \
    if (mylock) { \
	struct flock fs; \
 \
	fs.l_type = F_UNLCK; \
	fs.l_whence = L_SET; \
	fs.l_start = fs.l_len = 0; \
	(void) fcntl (myfd, F_SETLK, &fs); \
    } \
    else
#endif

extern int errno;

#ifdef	BRIDGE
/* FTP interface routines and variables */

extern char *ftp_error;

int ftp_exits(), ftp_delete(), ftp_mkdir(), ftp_rename(), ftp_type(),
ftp_write(), ftp_append(), ftp_read(), ftp_ls(), ftp_login(), ftp_quit(), ftp_abort(), ftp_reply();
#endif

#endif				/* __FTAM2_FTAMSYSTEM_H__ */
