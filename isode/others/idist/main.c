/* main.c -- driver for the client program */

/*
 * $Header: /xtel/isode/isode/others/idist/RCS/main.c,v 9.0 1992/06/16 12:42:00 isode Rel $
 *
 * Major changes to this file have been the ripping out of the server
 * code. This is entirely the client part now. Also changed are the
 * argument parsing stuff and a few bits and pieces.
 *
 * Julian Onions <jpo@cs.nott.ac.uk>
 * Nottingham University Computer Science.
 *
 *
 * $Log: main.c,v $
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
char copyright[] =
"@(#) Copyright (c) 1983 Regents of the University of California.\n\
 All rights reserved.\n";
#endif

#ifndef lint
static char sccsid[] = "@(#)main.c      5.5 (Berkeley) 6/1/90";
static char *rcsid = "$Header: /xtel/isode/isode/others/idist/RCS/main.c,v 9.0 1992/06/16 12:42:00 isode Rel $";
#endif

#include "defs.h"

#define NHOSTS 100

/*
 * Remote distribution program.
 *
 * -u option (controlled by "#ifdef UW") to update only (don't install
 *     the file, assuming the workstation owner doesn't want that file)
 */

char	*distfile = NULL;
char	utmpfile[] = "/tmp/idistXXXXXX";
char	*tmpname = &utmpfile[5];

int	debug;		/* debugging flag */
int	nflag;		/* NOP flag, just print commands without executing */
int	qflag;		/* Quiet. Don't print messages */
int	options;	/* global options */
int	iamremote;	/* act as remote server for transfering files */

FILE	*fin = NULL;	/* input file pointer */
char	*host;		/* host name */
int	nerrs;		/* number of errors while sending/receiving */
char	user[10];	/* user's name */
char	homedir[128];	/* user's home directory */
int	userid;		/* user's user ID */
int	groupid;	/* user's group ID */

struct	passwd *pw;	/* pointer to static area used by getpwent */
struct	group *gr;	/* pointer to static area used by getgrent */
char	*myname = "idist";

main(argc, argv)
	int argc;
	char *argv[];
{
	int cmdargs = 0;
	char *dhosts[NHOSTS], **hp = dhosts;
	extern	int optind;
	extern	char *optarg;
	int	opt;

	if (myname = rindex (argv[0], '/'))
		myname++;
	if (myname == NULL || *myname == NULL)
		myname = argv[0];

	isodetailor (myname, 1);

	pw = getpwuid(userid = getuid());
	if (pw == NULL)
		adios (NULLCP, "Who are you?");

	(void) strcpy(user, pw->pw_name);
	(void) strcpy(homedir, pw->pw_dir);
	groupid = pw->pw_gid;
	host = getlocalhost ();

	while ((opt = getopt (argc, argv, "f:m:d:DcnqbuRvwyhiQ")) != EOF)
		switch (opt) {
		    case 'f':
			distfile = optarg;
			if (distfile[0] == '-' && distfile[1] == '\0')
				fin = stdin;
			break;

		    case 'm':
			if (hp >= &dhosts[NHOSTS-2])
				adios (NULLCP, "too many destination hosts");
			*hp++ = optarg;
			break;

		    case 'd':
			define(optarg);
			break;

		    case 'D':
			debug++;
			break;

		    case 'c':
			cmdargs++;
			break;

		    case 'n':
			if (options & VERIFY) {
				advise (NULLCP, "-n overrides -v");
				options &= ~VERIFY;
			}
			nflag++;
			break;

		    case 'q':
			qflag++;
			break;

		    case 'b':
			options |= COMPARE;
			break;
#ifdef UW
		    case 'u':
			options |= NOINSTALL;
			break;
#endif UW
		    case 'R':
			options |= REMOVE;
			break;

		    case 'v':
			if (nflag) {
				advise (NULLCP, "-n overrides -v");
				break;
			}
			options |= VERIFY;
			break;

		    case 'w':
			options |= WHOLE;
			break;

		    case 'y':
			options |= YOUNGER;
			break;

		    case 'h':
			options |= FOLLOW;
			break;

		    case 'i':
			options |= IGNLNKS;
			break;

		    case 'Q':
			options |= QUERYM;
			break;

		    default:
			usage();
			break;
		}
	*hp = NULL;
	argc -= optind;
	argv += optind;

	(void) mktemp(utmpfile);

	if (cmdargs)
		docmdargs(argc, argv);
	else {
		if (fin == NULL) {
			if(distfile == NULL) {
				if((fin = fopen("distfile","r")) == NULL)
					fin = fopen("Distfile", "r");
			} else
				fin = fopen(distfile, "r");
			if(fin == NULL) {
				adios (distfile, "Can't open file");
			}
		}
		(void) yyparse();
		if (nerrs == 0)
			docmds(dhosts, argc, argv);
	}

	return(nerrs != 0);
}

usage()
{
	advise (NULLCP,
		"Usage: %s [-nqbhirvwyD] [-f distfile] [-d var=value] [-m host] [file ...]\n",
		myname);
	adios(NULLCP, "or: %s [-nqbhirvwyD] -c source [...] machine[:dest]\n",
	      myname);
}

/*
 * rcp like interface for distributing files.
 */
docmdargs(nargs, args)
	int nargs;
	char *args[];
{
	register struct namelist *nl, *prev;
	register char *cp;
	struct namelist *files, *hosts;
	struct subcmd *cmds;
	char *dest;
	static struct namelist tnl = { NULL, NULL };
	int i;

	if (nargs < 2)
		usage();

	prev = NULL;
	for (i = 0; i < nargs - 1; i++) {
		nl = makenl(args[i]);
		if (prev == NULL)
			files = prev = nl;
		else {
			prev->n_next = nl;
			prev = nl;
		}
	}

	cp = args[i];
	if ((dest = index(cp, ':')) != NULL)
		*dest++ = '\0';
	tnl.n_name = cp;
	hosts = expand(&tnl, E_ALL);
	if (nerrs)
		exit(1);

	if (dest == NULL || *dest == '\0')
		cmds = NULL;
	else {
		cmds = makesubcmd(INSTALL);
		cmds->sc_options = options;
		cmds->sc_name = dest;
	}

	if (debug) {
		(void) printf("docmdargs()\nfiles = ");
		prnames(files);
		(void) printf("hosts = ");
		prnames(hosts);
	}
	insert((char *)NULL, files, hosts, cmds);
	docmds((char **)NULL, 0, (char **)NULL);
}

/*
 * Print a list of NAME blocks (mostly for debugging).
 */
prnames(nl)
	register struct namelist *nl;
{
	(void) printf("( ");
	while (nl != NULL) {
		(void) printf("%s ", nl->n_name);
		nl = nl->n_next;
	}
	(void) printf(")\n");
}
