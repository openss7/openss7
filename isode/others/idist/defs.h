/* defs.h - general definitions for idist stuff */

/*
 * $Header: /xtel/isode/isode/others/idist/RCS/defs.h,v 9.0 1992/06/16 12:42:00 isode Rel $
 *
 * General definitions used in the server & client parts of the idist
 * updating tools. This file not changed much from the original UCB rdist one.
 *
 * Julian Onions <jpo@cs.nott.ac.uk>
 * Nottingham University Computer Science.
 * 
 *
 * $Log: defs.h,v $
 * Revision 9.0  1992/06/16  12:42:00  isode
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

#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include <sys/param.h>
#include "usr.dirent.h"
#include <sys/stat.h>
#include <sys/time.h>
#include <internet.h>
#include "manifest.h"
#include "tailor.h"
#include "general.h"

/*
 * The version number should be changed whenever the protocol changes.
 */
#define VERSION	 1

#define	MAILCMD	 "/usr/lib/sendmail -oi -t"

	/* defines for yacc */
#define EQUAL	1
#define LP	2
#define RP	3
#define SM	4
#define ARROW	5
#define COLON	6
#define DCOLON	7
#define NAME	8
#define STRING	9
#define INSTALL	10
#define NOTIFY	11
#define EXCEPT	12
#define PATTERN	13
#define SPECIAL	14
#define OPTION	15

	/* lexical definitions */
#define	QUOTE 	0200		/* used internally for quoted characters */
#define	TRIM	0177		/* Mask to strip quote bit */

	/* table sizes */
#define HASHSIZE	1021
#define INMAX	3500

	/* option flags */
#define VERIFY	0x1
#define WHOLE	0x2
#define YOUNGER	0x4
#define COMPARE	0x8
#define REMOVE	0x10
#define FOLLOW	0x20
#define IGNLNKS	0x40
#ifdef UW
#define NOINSTALL	0x80
#endif UW
#define QUERYM	0x100

	/* expand type definitions */
#define E_VARS	0x1
#define E_SHELL	0x2
#define E_TILDE	0x4
#define E_ALL	0x7

	/* actions for lookup() */
#define LOOKUP	0
#define INSERT	1
#define REPLACE	2

#define ISDIR(m) (((m) & S_IFMT) == S_IFDIR)

#define ALLOC(x) (struct x *) malloc(sizeof(struct x))

#ifndef OK
#define OK	0
#define NOTOK	(-1)
#endif

#if defined(_AIX) && defined(n_name)
#undef n_name
#endif

struct namelist {	/* for making lists of strings */
	char	*n_name;
	struct	namelist *n_next;
};

struct subcmd {
	short	sc_type;	/* type - INSTALL,NOTIFY,EXCEPT,SPECIAL */
	short	sc_options;
	char	*sc_name;
	struct	namelist *sc_args;
	struct	subcmd *sc_next;
};

struct cmd {
	int	c_type;		/* type - ARROW,DCOLON */
	char	*c_name;	/* hostname or time stamp file name */
	char	*c_label;	/* label for partial update */
	struct	namelist *c_files;
	struct	subcmd *c_cmds;
	struct	cmd *c_next;
};

struct linkbuf {
	ino_t	inum;
	dev_t	devnum;
	int	count;
	char	*pathname;
	char	*target;
	struct	linkbuf *nextp;
};

extern int debug;		/* debugging flag */
extern int nflag;		/* NOP flag, don't execute commands */
extern int qflag;		/* Quiet. don't print messages */
extern int options;		/* global options */

extern int nerrs;		/* number of errors seen */
extern int iamremote;		/* acting as remote server */
extern char utmpfile[];		/* file name for logging changes */
extern struct linkbuf *ihead;	/* list of files with more than one link */
extern struct passwd *pw;	/* pointer to static area used by getpwent */
extern struct group *gr;	/* pointer to static area used by getgrent */
extern char *host;		/* host name of master copy */
extern char homedir[];		/* home directory of current user */
extern char user[];		/* the users name */
extern int errno;		/* system error number */
extern char *sys_errlist[];
extern char *myname;

char *makestr();
struct namelist *makenl();
struct subcmd *makesubcmd();
struct namelist *lookup();
struct namelist *expand();
char *exptilde();

void	adios (), advise ();
