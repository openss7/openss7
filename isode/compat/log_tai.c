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

/* log_tai.c - system tailoring routines */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/compat/RCS/log_tai.c,v 9.0 1992/06/16 12:07:00 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/compat/RCS/log_tai.c,v 9.0 1992/06/16 12:07:00 isode Rel
 *
 *
 * Log: log_tai.c,v
 * Revision 9.0  1992/06/16  12:07:00  isode
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

/* LINTLIBRARY */

#include <stdio.h>
#include "cmd_srch.h"
#include "general.h"
#include "manifest.h"
#include "logger.h"

/*  */

#define	LG_FILE         1
#define	LG_SFLAGS       2
#define	LG_DFLAGS       3
#define	LG_LEVEL        4
#define	LG_SIZE         5
#define	LG_DLEVEL       6
#define LG_SSLEVEL	7
#define LG_DSLEVEL	8

static CMD_TABLE log_tbl[] = {
	"FILE", LG_FILE,
	"SFLAGS", LG_SFLAGS,
	"DFLAGS", LG_DFLAGS,
	"LEVEL", LG_LEVEL,
	"SLEVEL", LG_LEVEL,
	"DLEVEL", LG_DLEVEL,
	"SSLEVEL", LG_SSLEVEL,
	"DSLEVEL", LG_DSLEVEL,
	"SIZE", LG_SIZE,
	0, -1,
};

static CMD_TABLE log_lvltbl[] = {
	"NONE", LLOG_NONE,
	"FATAL", LLOG_FATAL,
	"EXCEPTIONS", LLOG_EXCEPTIONS,
	"NOTICE", LLOG_NOTICE,
	"TRACE", LLOG_TRACE,
	"DEBUG", LLOG_DEBUG,
	"PDUS", LLOG_PDUS,
	"ALL", LLOG_ALL,
	0, -1
};

static CMD_TABLE log_flgtbl[] = {
	"CLOSE", LLOGCLS,
	"CREATE", LLOGCRT,
	"ZERO", LLOGZER,
	"TTY", LLOGTTY,
	0, -1
};

/*  */

log_tai(lgptr, av, ac)		/* for now only alter the level - files etc later */
	LLog *lgptr;
	char **av;
	int ac;
{
	register int i;
	register char *p;
	int val;

	for (i = 0; i < ac; i++) {
		if ((p = index(av[i], '=')) == NULLCP)
			continue;
		*p++ = NULL;
		switch (cmd_srch(av[i], log_tbl)) {
		case LG_LEVEL:
			val = cmd_srch(p, log_lvltbl);
			if (val != -1)
				lgptr->ll_events |= val;
			break;
		case LG_DSLEVEL:
			val = cmd_srch(p, log_lvltbl);
			if (val != -1)
				lgptr->ll_syslog &= ~val;
			break;
		case LG_SSLEVEL:
			val = cmd_srch(p, log_lvltbl);
			if (val != -1)
				lgptr->ll_syslog |= val;
			break;
		case LG_DLEVEL:
			val = cmd_srch(p, log_lvltbl);
			if (val != -1)
				lgptr->ll_events &= ~val;
			break;
		case LG_FILE:
			lgptr->ll_file = strdup(p);
			break;
		case LG_SFLAGS:
			val = cmd_srch(p, log_flgtbl);
			if (val != -1)
				lgptr->ll_stat |= val;
			break;
		case LG_DFLAGS:
			val = cmd_srch(p, log_flgtbl);
			if (val != -1)
				lgptr->ll_stat &= ~val;
			break;
		case LG_SIZE:
			lgptr->ll_msize = atoi(p);
			break;
		}
	}
}
