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

#ifndef __IDIST_DEFS_H__
#define __IDIST_DEFS_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* defs.h - general definitions for idist stuff */

/*
 * Header: /xtel/isode/isode/others/idist/RCS/defs.h,v 9.0 1992/06/16 12:42:00 isode Rel
 *
 * General definitions used in the server & client parts of the idist
 * updating tools. This file not changed much from the original UCB rdist one.
 *
 * Julian Onions <jpo@cs.nott.ac.uk>
 * Nottingham University Computer Science.
 * 
 *
 * Log: defs.h,v 
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
#endif				/* UW */
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

struct namelist {			/* for making lists of strings */
	char *n_name;
	struct namelist *n_next;
};

struct subcmd {
	short sc_type;			/* type - INSTALL,NOTIFY,EXCEPT,SPECIAL */
	short sc_options;
	char *sc_name;
	struct namelist *sc_args;
	struct subcmd *sc_next;
};

struct cmd {
	int c_type;			/* type - ARROW,DCOLON */
	char *c_name;			/* hostname or time stamp file name */
	char *c_label;			/* label for partial update */
	struct namelist *c_files;
	struct subcmd *c_cmds;
	struct cmd *c_next;
};

struct linkbuf {
	ino_t inum;
	dev_t devnum;
	int count;
	char *pathname;
	char *target;
	struct linkbuf *nextp;
};

extern int debug;			/* debugging flag */
extern int nflag;			/* NOP flag, don't execute commands */
extern int qflag;			/* Quiet. don't print messages */
extern int options;			/* global options */

extern int nerrs;			/* number of errors seen */
extern int iamremote;			/* acting as remote server */
extern char utmpfile[];			/* file name for logging changes */
extern struct linkbuf *ihead;		/* list of files with more than one link */
extern struct passwd *pw;		/* pointer to static area used by getpwent */
extern struct group *gr;		/* pointer to static area used by getgrent */
extern char *host;			/* host name of master copy */
extern char homedir[];			/* home directory of current user */
extern char user[];			/* the users name */
extern int errno;			/* system error number */
extern char *sys_errlist[];
extern char *myname;

char *makestr();
struct namelist *makenl();
struct subcmd *makesubcmd();
struct namelist *lookup();
struct namelist *expand();
char *exptilde();

void adios(), advise();

#endif				/* __IDIST_DEFS_H__ */
