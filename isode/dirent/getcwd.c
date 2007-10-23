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
	getcwd -- get current working directory name (POSIX and SVID compatible)

	last edit:	21-Sep-1987	D A Gwyn

	This public-domain getcwd() routine can be used to replace the UNIX
	System V library routine (which uses popen() to capture the output of
	the "pwd" command).  Once that is done, "pwd" can be reimplemented as
	just puts(getcwd()).

	This implementation depends on every directory having entries for
	"." and "..".  It also depends on the internals of the <dirent.h>
	data structures to some degree.

	I considered using chdir() to ascend the hierarchy, followed by a
	final chdir() to the path being returned by getcwd() to restore the
	location, but decided that error recovery was too difficult that way.
	The algorithm I settled on was inspired by my rewrite of the "pwd"
	utility, combined with the dotdots[] array trick from the SVR2 shell.
*/

#include	<sys/types.h>
#include	<sys/stat.h>
#include	"usr.dirent.h"
#include	<errno.h>
#include	"general.h"

extern int fstat(), stat();

extern int errno;			/* normally done by <errno.h> */

#ifndef NULL
#define	NULL	0		/* amorphous null pointer constant */
#endif

#ifndef NAME_MAX
#define	NAME_MAX	255	/* maximum directory entry size */
#endif

char *
getcwd(buf, size)			/* returns pointer to CWD pathname */
	char *buf;			/* where to put name (NULL to malloc) */
	size_t size;			/* size of buf[] or malloc()ed memory */
{
	static char dotdots[] =
	    "../../../../../../../../../../../../../../../../../../../../../../../../../..";
	char *dotdot;			/* -> dotdots[.], right to left */
	DIR *dirp;			/* -> parent directory stream */
	struct dirent *dir;		/* -> directory entry */
	struct stat stat1, stat2;	/* info from stat() */
	struct stat *d = &stat1;	/* -> info about "." */
	struct stat *dd = &stat2;	/* -> info about ".." */
	register char *buffer;		/* local copy of buf, or malloc()ed */
	char *bufend;			/* -> buffer[size] */
	register char *endp;		/* -> end of reversed string */
	register char *dname;		/* entry name ("" for root) */
	int serrno = errno;		/* save entry errno */

	if (size == 0) {
		errno = EINVAL;	/* invalid argument */
		return NULL;
	}

	if ((buffer = buf) == NULL	/* wants us to malloc() the string */
	    && (buffer = (char *) malloc((unsigned) size)) == NULL) {
		errno = ENOMEM;	/* cannot malloc() specified size */
		return NULL;
	}

	if (stat(".", dd) != 0)	/* prime the pump */
		goto error;	/* errno already set */

	endp = buffer;		/* initially, empty string */
	bufend = &buffer[size];

	for (dotdot = &dotdots[sizeof(dotdots)]; dotdot != dotdots;) {
		dotdot -= 3;	/* include one more "/.." section */
		/* (first time is actually "..") */

		/* swap stat() info buffers */
		{
			register struct stat *temp = d;

			d = dd;	/* new current dir is old parent dir */
			dd = temp;
		}

		if ((dirp = opendir(dotdot)) == NULL)	/* new parent */
			goto error;	/* errno already set */

		if (fstat(dirp->dd_fd, dd) != 0) {
			serrno = errno;	/* set by fstat() */
			(void) closedir(dirp);
			errno = serrno;	/* in case closedir() clobbered it */
			goto error;
		}

		if (d->st_dev == dd->st_dev) {	/* not crossing a mount point */
			if (d->st_ino == dd->st_ino) {	/* root directory */
				dname = "";
				goto append;
			}

			do
				if ((dir = readdir(dirp)) == NULL) {
					(void) closedir(dirp);
					errno = ENOENT;	/* missing entry */
					goto error;
				}
			while (dir->d_ino != d->st_ino) ;
		} else {	/* crossing a mount point */
			struct stat t;	/* info re. test entry */
			char name[sizeof(dotdots) + 1 + NAME_MAX];

			(void) strcpy(name, dotdot);
			dname = &name[strlen(name)];
			*dname++ = '/';

			do {
				if ((dir = readdir(dirp)) == NULL) {
					(void) closedir(dirp);
					errno = ENOENT;	/* missing entry */
					goto error;
				}

				(void) strcpy(dname, dir->d_name);
				/* must fit if NAME_MAX is not a lie */
			}
			while (stat(name, &t) != 0
			       || t.st_ino != d->st_ino || t.st_dev != d->st_dev);
		}

		dname = dir->d_name;

		/* append "/" and reversed dname string onto buffer */
	      append:
		if (endp != buffer	/* avoid trailing / in final name */
		    || dname[0] == '\0'	/* but allow "/" when CWD is root */
		    )
			*endp++ = '/';

		{
			register char *app;	/* traverses dname string */

			for (app = dname; *app != '\0'; ++app) ;

			if (app - dname >= bufend - endp) {
				(void) closedir(dirp);
				errno = ERANGE;	/* won't fit allotted space */
				goto error;
			}

			while (app != dname)
				*endp++ = *--app;
		}

		(void) closedir(dirp);

		if (dname[0] == '\0') {	/* reached root; wrap it up */
			register char *startp;	/* -> buffer[.] */

			*endp = '\0';	/* plant null terminator */

			/* straighten out reversed pathname string */
			for (startp = buffer; --endp > startp; ++startp) {
				char temp = *endp;

				*endp = *startp;
				*startp = temp;
			}

			errno = serrno;	/* restore entry errno */
			return buffer;
		}
	}

	errno = ENOMEM;		/* actually, algorithm failure */

      error:
	if (buf == NULL)
		free(buffer);

	return NULL;
}
