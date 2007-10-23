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

#ifndef __FTAM2_FTAMUSER_H__
#define __FTAM2_FTAMUSER_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* ftamuser.h - include file for interactive FTAM initiator */

/* 
 * Header: /xtel/isode/isode/ftam2/RCS/ftamuser.h,v 9.0 1992/06/16 12:15:43 isode Rel
 *
 *
 * Log: ftamuser.h,v
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

#include "ftamsbr.h"

/* MAIN */

extern int ontty;
extern int interrupted;

void adios(), advise();

#ifndef	BRIDGE
int ask(), getline();
#endif
char *strdup();

/* DATA */

extern int ftamfd;

#ifdef	BRIDGE
extern int dataconn();
#endif

extern char *host;
extern char *user;
extern char *account;

#ifndef	BRIDGE
extern char *userdn;
extern char *storename;
#endif

extern int bell;
extern int concurrency;			/* Olivier Dubois */
extern int debug;
extern int globbing;
extern int hash;
extern int marks;
extern int omode;
extern int query;
extern int runcom;
extern int tmode;
extern int trace;
extern int verbose;
extern int watch;

extern char *myhome;
extern char *myuser;

extern int realstore;

#define	RFS_UNKNOWN	0
#define	RFS_UNIX	1

extern char *rs_unknown;
extern char *rs_support;

extern char *rcwd;

extern struct QOStype myqos;

char *str2file();

/* DISPATCH */

struct dispatch {
	char *ds_name;
	IFP ds_fnx;

	int ds_flags;
#define	DS_NULL		0x00
#define	DS_OPEN		0x01		/* association required */
#define	DS_CLOSE	0x02		/* association avoided */
#define	DS_MODES	0x04		/* class/units meaningful */

	int ds_class;
	int ds_units;

	char *ds_help;
};

struct dispatch *getds();

/* FTAM */

#define	UMASK	"\020\01READ\02WRITE\03ACCESS\04LIMITED\05ENHANCED\06GROUPING\
\07RECOVERY\08RESTART"

extern OID context;
extern int fqos;
extern int ftam_class;
extern int units;
extern int attrs;
extern int fadusize;

extern struct vfsmap vfs[];		/* ordering depends on char *tmodes[] */

#define	VFS_DEF	0		/* try to default it */
#define	VFS_UBF	1		/* offset to unstructured binary file */
#define	VFS_UTF	2		/* ..  unstructured text file */
#define	VFS_FDF	3		/* ..  file directory file */

extern struct vfsmap *myvf;

void ftam_advise(), ftam_chrg(), ftam_diag(), ftam_watch();

/* FILES */

struct filent {
	char *fi_name;
	OID fi_oid;

	char *fi_entry;

	struct filent *fi_next;
};

extern int toomany;

extern int nfilent;
extern struct filent *filents;

int fdffnx();

/* GLOB */

extern int xglobbed;
extern char *globerr;

int blkfree(), blklen();
char **blkcpy();

char *xglob1val();
char **xglob();

extern int errno;
extern char *isodeversion;

#ifdef	BRIDGE
extern char ftam_error[];
#endif

#endif				/* __FTAM2_FTAMUSER_H__ */
