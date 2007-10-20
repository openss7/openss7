/* install.c - installation of files on remote host */

/*
 * $Header: /xtel/isode/isode/others/idist/RCS/install.c,v 9.0 1992/06/16 12:42:00 isode Rel $
 *
 * Installation of files on remote host - the routines here drive the
 * protocol for installation, comparision and deletion etc. The
 * protocol is basically similar to the original rdist, except it is
 * fully client/server in this model with the daemon only responding
 * to requests. In the old version, the two sides occasionally swapped
 * around for some operations. Parts of this file were originally in
 * the file server.c.
 *
 * Julian Onions <jpo@cs.nott.ac.uk>
 * Nottingham University Computer Science
 * 
 *
 * $Log: install.c,v $
 * Revision 9.0  1992/06/16  12:42:00  isode
 * Release 8.0
 *
 */

/*
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that: (1) source distributions retain this entire copyright
 * notice and comment, and (2) distributions including binaries display
 * the following acknowledgement:  ``This product includes software
 * developed by the University of California, Berkeley and its contributors''
 * in the documentation or other materials provided with the distribution
 * and in all advertising materials mentioning features or use of this
 * software. Neither the name of the University nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef lint
static char sccsid[] = "@(#)server.c    5.12 (Berkeley) 6/1/90";
static char rcsid[] = "$Header: /xtel/isode/isode/others/idist/RCS/install.c,v 9.0 1992/06/16 12:42:00 isode Rel $";
#endif

#include "defs.h"
#include "sys.file.h"

struct	linkbuf *ihead;		/* list of files with more than one link */
char	target[BUFSIZ];		/* target/source directory name */
char	basename[BUFSIZ];	/* base of directory operations */
char	*tp;			/* pointer to end of target name */
char	*Tdest;			/* pointer to last T dest*/
int	catname;		/* cat name to target name */
char	*stp[128];		/* stack of saved tp's for directories */
int	oumask;			/* old umask for creating files */
char	tranbuf[1024*8];	/* 8K at a time... */

extern	FILE *lfp;		/* log file for mailing changes */

SFD	cleanup();
struct	linkbuf *savelink();
extern char *getstring ();

/*
 * Update the file(s) if they are different.
 * destdir = 1 if destination should be a directory
 * (i.e., more than one source is being copied to the same destination).
 */
install(src, dest, destdir, opts)
	char *src, *dest;
	int destdir, opts;
{
	char *rname;
	char destcopy[BUFSIZ];

	if (dest == NULL) {
		opts &= ~WHOLE; /* WHOLE mode only useful if renaming */
		dest = src;
	}

	if (nflag || debug) {
		(void) printf("%s%s%s%s%s%s %s %s\n",
		       opts & VERIFY ? "verify":"install",
		       opts & WHOLE ? " -w" : "",
		       opts & YOUNGER ? " -y" : "",
		       opts & COMPARE ? " -b" : "",
		       opts & REMOVE ? " -R" : "",
		       opts & QUERYM ? " -Q" : "", src, dest);
		if (nflag)
			return;
	}

	rname = exptilde(target, src);
	if (rname == NULL)
		return;
	tp = target;
	(void) strcpy (basename, target);
	while (*tp)
		tp++;
	/*
	 * If we are renaming a directory and we want to preserve
	 * the directory heirarchy (-w), we must strip off the leading
	 * directory name and preserve the rest.
	 */
	if (opts & WHOLE) {
		rname = target;
		while (*rname == '/')
			rname++;
		destdir = 1;
	} else {
		rname = rindex(target, '/');
		if (rname == NULL)
			rname = target;
		else
			rname++;
	}
	if (debug)
		(void) printf("target = %s, rname = %s\n", target, rname);
	/*
	 * Pass the destination file/directory name to remote.
	 */
	if (initdir (destdir, dest) < 0)
		return;

	(void) strcpy(destcopy, dest);
	Tdest = destcopy;

	sendf(rname, opts);
	Tdest = 0;
}

#define protoname() (pw ? pw->pw_name : user)
#define protogroup() (gr ? gr->gr_name : group)
/*
 * Transfer the file or directory in target[].
 * rname is the name of the file on the remote host.
 */
sendf(rname, opts)
	char *rname;
	int opts;
{
	register struct subcmd *sc;
	struct stat stb;
	int sizerr, f, u, len;
	off_t i;
	DIR *d;
	struct dirent *dp;
	char *otp, *cp;
	extern struct subcmd *subcmds;
	char	buf[BUFSIZ];
	static char uname[15], group[15];

	if (debug)
		(void) printf("sendf(%s, %x)\n", rname, opts);

	if (except(target))
		return;
	if ((opts & FOLLOW ? stat(target, &stb) : lstat(target, &stb)) < 0) {
		advise (target, "Can't stat");
		return;
	}
	if ((u = update(rname, opts, &stb)) == 0) {
		if ((stb.st_mode & S_IFMT) == S_IFREG && stb.st_nlink > 1)
			(void) savelink(&stb, opts);
		return;
	}

	if (pw == NULL || pw->pw_uid != stb.st_uid)
		if ((pw = getpwuid(stb.st_uid)) == NULL) {
			log(lfp, "%s: no password entry for uid \n", target);
			pw = NULL;
			(void) sprintf(uname, ":%d", stb.st_uid);
		}
	if (gr == NULL || gr->gr_gid != stb.st_gid)
		if ((gr = getgrgid(stb.st_gid)) == NULL) {
			log(lfp, "%s: no name for group %d\n", target);
			gr = NULL;
			(void) sprintf(group, ":%d", stb.st_gid);
		}
	if (u == 1) {
#ifdef UW
		if (opts & NOINSTALL) {
			log(lfp, "does not exist, did not install: %s\n", target);
			goto dospecial;
		}
#endif UW
		if (opts & VERIFY) {
			log(lfp, "need to install: %s\n", target);
			goto dospecial;
		}
		log(lfp, "installing: %s\n", target);
		opts &= ~(COMPARE|REMOVE);
	}

	switch (stb.st_mode & S_IFMT) {
	case S_IFDIR:
		if ((d = opendir(target)) == NULL) {
			advise (target, "Can't open directory");
			return;
		}
		if (transfer (stb.st_mode & S_IFMT, opts,
			      stb.st_mode & 07777, (off_t)0, (time_t)0,
			      protoname (), protogroup (),
			      rname, "") < 0) {
			(void) closedir (d);
			return;
		}

		if (opts & REMOVE)
			(void) rmchk(opts);

		otp = tp;
		len = tp - target;
		while (dp = readdir(d)) {
			if (!strcmp(dp->d_name, ".") ||
			    !strcmp(dp->d_name, ".."))
				continue;
			if (len + 1 + (int)strlen(dp->d_name) >= BUFSIZ - 1) {
				advise (NULLCP, "%s/%s name too long",
					target, dp->d_name);
				continue;
			}
			tp = otp;
			*tp++ = '/';
			cp = dp->d_name;
			while (*tp++ = *cp++)
				;
			tp--;
			sendf(dp->d_name, opts);
		}
		(void) closedir(d);
		(void) terminate (S_IFDIR, OK);
		tp = otp;
		*tp = '\0';
		return;

	case S_IFLNK:
		if (u != 1)
			opts |= COMPARE;
		if (stb.st_nlink > 1) {
			struct linkbuf *lp;

			if ((lp = savelink(&stb, opts)) != NULL) {
				if (*lp -> target == 0)
					(void) strcpy (buf, lp -> pathname);
				else	(void) sprintf (buf, "%s/%s",
							lp -> target,
							lp -> pathname);
				(void) transfer ((unsigned short)0, opts,
						 (unsigned short)0, (off_t)0,
						 (time_t)0, "", "",
						 rname, buf);
				return;
			}
		}
		sizerr = (readlink(target, buf, BUFSIZ) != stb.st_size);
		if (debug)
			(void) printf("readlink = %.*s\n", stb.st_size, buf);
		if (transfer (stb.st_mode & S_IFMT, opts, stb.st_mode & 07777,
			      stb.st_size, stb.st_mtime,
			      protoname (), protogroup (), rname,
			      buf) < 0)
			return;
		goto done;

	case S_IFREG:
		break;

	default:
		advise (NULLCP, "%s: not a file or directory", target);
		return;
	}

	if (u == 2) {
		if (opts & VERIFY) {
			log(lfp, "need to update: %s\n", target);
			goto dospecial;
		}
		log(lfp, "updating: %s\n", target);
	}

	if (stb.st_nlink > 1) {
		struct linkbuf *lp;

		if ((lp = savelink(&stb, opts)) != NULL) {
			if (*lp -> target == 0)
				(void) strcpy (buf, lp -> pathname);
			else	(void) sprintf (buf, "%s/%s",
						lp -> target,
						lp -> pathname);
			(void) transfer ((unsigned short)0, opts,
					 (unsigned short)0, (off_t)0,
					 (time_t)0, "", "",
					 rname, buf);
			return;
		}
	}

	if ((f = open(target, O_RDONLY, 0)) < 0) {
		advise (target, "Can't open file");
		return;
	}
	if ( transfer ((unsigned short)S_IFREG, opts, stb.st_mode & 07777,
		       stb.st_size,
		       stb.st_mtime, protoname (), protogroup (),
		       rname, "") < 0) {
		(void) close (f);
		return;
	}
	sizerr = 0;
	for (i = 0; i < stb.st_size; i += sizeof tranbuf) {
		int amt = sizeof tranbuf;
		if (i + amt > stb.st_size)
			amt = stb.st_size - i;
		if (sizerr == 0 && read(f, tranbuf, amt) != amt)
			sizerr = 1;
		if (tran_data (tranbuf, amt) == NOTOK)
			break;
	}
	(void) close(f);
	if (sizerr) {
		advise (NULLCP, "%s: file changed size", target);
		if (terminate (S_IFREG, NOTOK) < 0)
			return;
	} else {
		if (terminate (S_IFREG, OK) < 0)
			return;
	}
done:
	if (opts & COMPARE)
		return;
dospecial:
	for (sc = subcmds; sc != NULL; sc = sc->sc_next) {
		if (sc->sc_type != SPECIAL)
			continue;
#ifdef UW
		if (opts & NOINSTALL)  /* don't do specials associated with
					  non-installation notices */
			continue;
#endif UW
		if (sc->sc_args != NULL && !inlist(sc->sc_args, target))
			continue;
		log(lfp, "special \"%s\"\n", sc->sc_name);
		if (opts & VERIFY)
			continue;
		(void) sprintf(buf, "FILE=%s;export FILE;%s",
			       target, sc->sc_name);
		(void) runspecial (buf);
		
	}
}

struct linkbuf *
savelink(sp, opts)
struct stat *sp;
int	opts;
{
	struct linkbuf *lp;
	extern	char *makestr ();

	for (lp = ihead; lp != NULL; lp = lp->nextp)
		if (lp->inum == sp->st_ino && lp->devnum == sp->st_dev) {
			lp->count--;
			return(lp);
		}
	lp = (struct linkbuf *) malloc(sizeof(*lp));
	if (lp == NULL)
		log(lfp, "out of memory, link information lost\n");
	else {
		lp->nextp = ihead;
		ihead = lp;
		lp->inum = sp->st_ino;
		lp->devnum = sp->st_dev;
		lp->count = sp->st_nlink - 1;
		if (opts & WHOLE)
			lp->pathname = makestr (target);
		else {
			if (strncmp (target, basename, strlen(basename)) == 0)
				lp -> pathname = makestr (target +
							  strlen(basename) + 1);
			else
				lp -> pathname = makestr (target);
		}
				
		if (Tdest)
			lp->target = makestr (Tdest);
		else
			*lp->target = 0;
	}
	return(NULL);
}

update(rname, opts, sp)
	char *rname;
	int opts;
	struct stat *sp;
{
	off_t size;
	time_t mtime;
	unsigned short mode;
	int	retval;

	if (debug) 
		(void) printf("update(%s, %x, %x)\n", rname, opts, sp);

	/*
	 * Check to see if the file exists on the remote machine.
	 */
	switch (retval = rquery (rname, &mtime, &size, &mode)) {
	    case DONE:  /* file doesn't exist so install it */
		if ((opts & QUERYM) && !query ("Install",
					       (int)(sp ->st_mode & S_IFMT),
					       NULLCP))
			return 0;
		return(1);

	    case NOTOK:	/* something went wrong! */
		nerrs++;
		return(0);

	    case OK:
		break;

	    default:
		advise (NULLCP, "update: unexpected response %d", retval);
		return(0);
	}

	if (mode == S_IFDIR)
		return (2);

	if (opts & COMPARE) {
		if ((opts & QUERYM) && !query ("Compare and update", 
					       (int) mode, NULLCP))
			return 0;
		return(3);
	}

	/*
	 * File needs to be updated?
	 */
	if (opts & YOUNGER) {
		if (sp->st_mtime == mtime)
			return(0);
		if (sp->st_mtime < mtime) {
			log(lfp, "Warning: %s: remote copy is newer\n", target);
			return(0);
		}
	} else if (sp->st_mtime == mtime && sp->st_size == size)
		return(0);
	if ((opts & QUERYM) && !query ("Update",
				       (int)(sp -> st_mode & S_IFMT), NULLCP))
		return 0;
	return(2);
}



/*VARARGS2*/
log(fp, fmt, a1, a2, a3)
	FILE *fp;
	char *fmt;
	int a1, a2, a3;
{
	/* Print changes locally if not quiet mode */
	if (!qflag)
		(void) printf(fmt, a1, a2, a3);

	/* Save changes (for mailing) if really updating files */
	if (!(options & VERIFY) && fp != NULL)
		(void) fprintf(fp, fmt, a1, a2, a3);
}

/*
 * Remove temporary files and do any cleanup operations before exiting.
 */
SFD cleanup()
{
	(void) unlink(utmpfile);
	exit(1);
}

query (mess, mode, name)
int	mode;
char	*mess, *name;
{
	char	buf[BUFSIZ];
	char	*cp;

	switch (mode) {
	    case S_IFDIR:
		cp = "directory";
		break;
	    case 0:
	    case S_IFREG:
		cp = "file";
		break;
	    case S_IFLNK:
		cp = "symbolic link";
		break;
	    default:
		cp = "unknown file type";
		break;
	}

	(void) sprintf (buf, "%s %s %s? ", mess, cp,
			name == NULLCP ? target : name);
	for (;;) {
		cp = getstring (buf);
		if (cp == NULLCP)
			continue;
		if (*cp == 'y' || *cp == 'Y' || *cp == 'n' || *cp == 'N')
			break;
	}
	
	if (*cp == 'y' || *cp == 'Y')
		return 1;
	return 0;
}
