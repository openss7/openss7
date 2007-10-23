/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, version 3 of the license.

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

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

/*
	opendir -- open a directory stream

	last edit:	16-Jun-1987	D A Gwyn
*/

#include	<sys/errno.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	"usr.dirent.h"

#ifndef	GETDENTS
#ifdef BSD_SYSV
#define open	_open		/* avoid emulation overhead */
#endif

typedef char *pointer;			/* (void *) if you have it */

extern void free();
extern pointer malloc();
extern int open(), close(), fstat();

extern int errno;

#ifndef NULL
#define	NULL	0
#endif

#ifndef O_RDONLY
#define	O_RDONLY	0
#endif

#ifndef S_ISDIR			/* macro to test for directory file */
#define	S_ISDIR( mode )		(((mode) & S_IFMT) == S_IFDIR)
#endif

DIR *
opendir(dirname)
	char *dirname;			/* name of directory */
{
	register DIR *dirp;		/* -> malloc'ed storage */
	register int fd;		/* file descriptor for read */
	struct stat sbuf;		/* result of fstat() */

	if ((fd = open(dirname, O_RDONLY)) < 0)
		return NULL;	/* errno set by open() */

	if (fstat(fd, &sbuf) != 0 || !S_ISDIR(sbuf.st_mode)) {
		(void) close(fd);
		errno = ENOTDIR;
		return NULL;	/* not a directory */
	}

	if ((dirp = (DIR *) malloc(sizeof(DIR))) == NULL
	    || (dirp->dd_buf = (char *) malloc((unsigned) DIRBUF)) == NULL) {
		register int serrno = errno;

		/* errno set to ENOMEM by sbrk() */

		if (dirp != NULL)
			free((pointer) dirp);

		(void) close(fd);
		errno = serrno;
		return NULL;	/* not enough memory */
	}

	dirp->dd_fd = fd;
	dirp->dd_loc = dirp->dd_size = 0;	/* refill needed */

	return dirp;
}
#else
int
_opendir_stub()
{;
}
#endif
