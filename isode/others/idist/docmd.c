/* docmd.c -- driver for the whole thing */

/*
 * $Header: /xtel/isode/isode/others/idist/RCS/docmd.c,v 9.0 1992/06/16 14:38:53 isode Rel $
 *
 * The major alterations to this file are the replacing of the
 * connection stuff with the ISODE functions necessary. These new
 * functions appear in a new file.
 *
 * Julian Onions <jpo@cs.nott.ac.uk>
 * Nottingham University, Computer Science.
 * 
 *
 * $Log: docmd.c,v $
 * Revision 9.0  1992/06/16  14:38:53  isode
 * Release 8.0
 *
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
static char *rcsid = "$Header: /xtel/isode/isode/others/idist/RCS/docmd.c,v 9.0 1992/06/16 14:38:53 isode Rel $";
static char sccsid[] = "@(#)docmd.c	5.6 (Berkeley) 6/1/90";
#endif

#include "defs.h"
#include <setjmp.h>
#include "sys.file.h"
#include <signal.h>

FILE	*lfp;			/* log file for recording files updated */
struct	subcmd *subcmds;	/* list of sub-commands for current cmd */
jmp_buf	env;

SFD	cleanup();
SFD	lostconn();

/*
 * Do the commands in cmds (initialized by yyparse).
 */
docmds(dhosts, argc, argv)
	char **dhosts;
	int argc;
	char **argv;
{
	register struct cmd *c;
	register struct namelist *f;
	register char **cpp;
	extern struct cmd *cmds;

	(void) signal(SIGHUP, cleanup);
	(void) signal(SIGINT, cleanup);
	(void) signal(SIGQUIT, cleanup);
	(void) signal(SIGTERM, cleanup);

	for (c = cmds; c != NULL; c = c->c_next) {
		if (dhosts != NULL && *dhosts != NULL) {
			for (cpp = dhosts; *cpp; cpp++)
				if (strcmp(c->c_name, *cpp) == 0)
					goto fndhost;
			continue;
		}
	fndhost:
		if (argc) {
			for (cpp = argv; *cpp; cpp++) {
				if (c->c_label != NULL &&
				    strcmp(c->c_label, *cpp) == 0) {
					cpp = NULL;
					goto found;
				}
				for (f = c->c_files; f != NULL; f = f->n_next)
					if (strcmp(f->n_name, *cpp) == 0)
						goto found;
			}
			continue;
		} else
			cpp = NULL;
	found:
		switch (c->c_type) {
		case ARROW:
			doarrow(cpp, c->c_files, c->c_name, c->c_cmds);
			break;
		case DCOLON:
			dodcolon(cpp, c->c_files, c->c_name, c->c_cmds);
			break;
		default:
			adios (NULLCP, "illegal command type %d\n", c->c_type);
		}
	}
	closeconn();
}

/*
 * Process commands for sending files to other machines.
 */
doarrow(filev, files, rhost, scmds)
	char **filev;
	struct namelist *files;
	char *rhost;
	struct subcmd *scmds;
{
	register struct namelist *f;
	register struct subcmd *sc;
	register char **cpp;
	int n, ddir, opts = options;

	if (debug)
		(void) printf("doarrow(%x, %s, %x)\n", files, rhost, scmds);

	if (files == NULL) {
		advise(NULLCP, "no files to be updated");
		return;
	}

	subcmds = scmds;
	ddir = files->n_next != NULL;	/* destination is a directory */
	if (nflag)
		(void) printf("updating host %s\n", rhost);
	else {
		if (setjmp(env))
			goto done;
		(void) signal(SIGPIPE, lostconn);
		if (!makeconn(rhost))
			return;
		if ((lfp = fopen(utmpfile, "w")) == NULL)
			adios (utmpfile, "cannot open file");
	}
	for (f = files; f != NULL; f = f->n_next) {
		if (filev) {
			for (cpp = filev; *cpp; cpp++)
				if (strcmp(f->n_name, *cpp) == 0)
					goto found;
			if (!nflag)
				(void) fclose(lfp);
			continue;
		}
	found:
		n = 0;
		for (sc = scmds; sc != NULL; sc = sc->sc_next) {
			if (sc->sc_type != INSTALL)
				continue;
			n++;
			install(f->n_name, sc->sc_name,
				sc->sc_name == NULL ? 0 : ddir, sc->sc_options);
			opts = sc->sc_options;
		}
		if (n == 0)
			install(f->n_name, (char *)NULL, 0, options);
	}
done:
	if (!nflag) {
		(void) signal(SIGPIPE, cleanup);
		(void) fclose(lfp);
		lfp = NULL;
	}
	for (sc = scmds; sc != NULL; sc = sc->sc_next)
		if (sc->sc_type == NOTIFY)
			notify(utmpfile, rhost, sc->sc_args, 0L);
	if (!nflag) {
		(void) unlink(utmpfile);
		for (; ihead != NULL; ihead = ihead->nextp) {
			free((char *)ihead);
			if ((opts & IGNLNKS) || ihead->count == 0)
				continue;
			log(lfp, "%s: Warning: missing links\n",
				ihead->pathname);
		}
	}
}


/* ARGSUSED */
SFD lostconn(sig)
int sig;
{
	log(lfp, "idist: lost connection\n");
	longjmp(env, 1);
}


time_t	lastmod;
FILE	*tfp;
extern	char target[], *tp;

/*
 * Process commands for comparing files to time stamp files.
 */
dodcolon(filev, files, stamp, scmds)
	char **filev;
	struct namelist *files;
	char *stamp;
	struct subcmd *scmds;
{
	register struct subcmd *sc;
	register struct namelist *f;
	register char **cpp;
	struct timeval tv[2];
	struct timezone tz;
	struct stat stb;

	if (debug)
		(void) printf("dodcolon()\n");

	if (files == NULL) {
		advise (NULLCP, "no files to be updated");
		return;
	}
	if (stat(stamp, &stb) < 0) {
		advise(stamp, "Can't stat");
		return;
	}
	if (debug)
		(void) printf("%s: %d\n", stamp, stb.st_mtime);

	subcmds = scmds;
	lastmod = stb.st_mtime;
	if (nflag || (options & VERIFY))
		tfp = NULL;
	else {
		if ((tfp = fopen(utmpfile, "w")) == NULL) {
			advise (utmpfile, "Can't open file");
			return;
		}
		(void) gettimeofday(&tv[0], &tz);
		tv[1] = tv[0];
		(void) utimes(stamp, tv);
	}

	for (f = files; f != NULL; f = f->n_next) {
		if (filev) {
			for (cpp = filev; *cpp; cpp++)
				if (strcmp(f->n_name, *cpp) == 0)
					goto found;
			continue;
		}
	found:
		tp = NULL;
		cmptime(f->n_name);
	}

	if (tfp != NULL)
		(void) fclose(tfp);
	for (sc = scmds; sc != NULL; sc = sc->sc_next)
		if (sc->sc_type == NOTIFY)
			notify(utmpfile, (char *)NULL, sc->sc_args, lastmod);
	if (!nflag && !(options & VERIFY))
		(void) unlink(utmpfile);
}

/*
 * Compare the mtime of file to the list of time stamps.
 */
cmptime(name)
	char *name;
{
	struct stat stb;

	if (debug)
		(void) printf("cmptime(%s)\n", name);

	if (except(name))
		return;

	if (nflag) {
		(void) printf("comparing dates: %s\n", name);
		return;
	}

	/*
	 * first time cmptime() is called?
	 */
	if (tp == NULL) {
		if (exptilde(target, name) == NULL)
			return;
		tp = name = target;
		while (*tp)
			tp++;
	}
	if (access(name, 4) < 0 || stat(name, &stb) < 0) {
		advise (name, "Can't access");
		return;
	}

	switch (stb.st_mode & S_IFMT) {
	case S_IFREG:
		break;

	case S_IFDIR:
		rcmptime(&stb);
		return;

	default:
		advise(NULLCP, "%s not a plain file", name);
		return;
	}

	if (stb.st_mtime > lastmod)
		log(tfp, "new: %s\n", name);
}

rcmptime(st)
	struct stat *st;
{
	register DIR *d;
	register struct dirent *dp;
	register char *cp;
	char *otp;
	int len;

	if (debug)
		(void) printf("rcmptime(%x)\n", st);

	if ((d = opendir(target)) == NULL) {
		advise (target, "can't open directory");
		return;
	}
	otp = tp;
	len = tp - target;
	while (dp = readdir(d)) {
		if (!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, ".."))
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
		cmptime(target);
	}
	(void) closedir(d);
	tp = otp;
	*tp = '\0';
}

/*
 * Notify the list of people the changes that were made.
 * rhost == NULL if we are mailing a list of changes compared to at time
 * stamp file.
 */
notify(file, rhost, to, lmod)
	char *file, *rhost;
	register struct namelist *to;
	time_t lmod;
{
	register int fd, len;
	FILE *pf, *popen();
	struct stat stb;
	char	buf[BUFSIZ];

	if ((options & VERIFY) || to == NULL)
		return;
	if (!qflag) {
		(void) printf("notify ");
		if (rhost)
			(void) printf("@%s ", rhost);
		prnames(to);
	}
	if (nflag)
		return;

	if ((fd = open(file, O_RDONLY, 0)) < 0) {
		advise (file, "Can't open file");
		return;
	}
	if (fstat(fd, &stb) < 0) {
		advise (file, "Can't stat");
		(void) close(fd);
		return;
	}
	if (stb.st_size == 0) {
		(void) close(fd);
		return;
	}
	/*
	 * Create a pipe to mailling program.
	 */
	pf = popen(MAILCMD, "w");
	if (pf == NULL) {
		advise (NULLCP, "notify: \"%s\" failed", MAILCMD);
		(void) close(fd);
		return;
	}
	/*
	 * Output the proper header information.
	 */
	(void) fprintf(pf, "From: idist (Remote ISO distribution program)\n");
	(void) fprintf(pf, "To:");
	if (!any('@', to->n_name) && rhost != NULL)
		(void) fprintf(pf, " %s@%s", to->n_name, rhost);
	else
		(void) fprintf(pf, " %s", to->n_name);
	to = to->n_next;
	while (to != NULL) {
		if (!any('@', to->n_name) && rhost != NULL)
			(void) fprintf(pf, ", %s@%s", to->n_name, rhost);
		else
			(void) fprintf(pf, ", %s", to->n_name);
		to = to->n_next;
	}
	(void) putc('\n', pf);
	if (rhost != NULL)
		(void) fprintf(pf, "Subject: files updated by idist from %s to %s\n",
			host, rhost);
	else
		(void) fprintf(pf, "Subject: files updated after %s\n", ctime(&lmod));
	(void) putc('\n', pf);

	while ((len = read(fd, buf, BUFSIZ)) > 0)
		(void) fwrite(buf, 1, len, pf);
	(void) close(fd);
	(void) pclose(pf);
}

/*
 * Return true if name is in the list.
 */
inlist(list, file)
	struct namelist *list;
	char *file;
{
	register struct namelist *nl;

	for (nl = list; nl != NULL; nl = nl->n_next)
		if (!strcmp(file, nl->n_name))
			return(1);
	return(0);
}

/*
 * Return TRUE if file is in the exception list.
 */
except(file)
	char *file;
{
	register struct	subcmd *sc;
	register struct	namelist *nl;
	char	*p, *re_comp ();

	if (debug)
		(void) printf("except(%s)\n", file);

	for (sc = subcmds; sc != NULL; sc = sc->sc_next) {
		if (sc->sc_type != EXCEPT && sc->sc_type != PATTERN)
			continue;
		for (nl = sc->sc_args; nl != NULL; nl = nl->n_next) {
			if (sc->sc_type == EXCEPT) {
				if (!strcmp(file, nl->n_name))
					return(1);
				continue;
			}
			if (p = re_comp(nl->n_name)) {
				advise (NULLCP, "'%s' - %s", nl -> n_name, p);
				continue;
			}
			if (re_exec(file) > 0)
				return(1);
		}
	}
	return(0);
}

char *
colon(cp)
	register char *cp;
{

	while (*cp) {
		if (*cp == ':')
			return(cp);
		if (*cp == '/')
			return(0);
		cp++;
	}
	return(0);
}
