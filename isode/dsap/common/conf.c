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

/* conf.c - DSAP Configuration */

#ifndef lint
static char *rcsid =
    "Header: /xtel/isode/isode/dsap/common/RCS/conf.c,v 9.0 1992/06/16 12:12:39 isode Rel";
#endif

/*
 * Header: /xtel/isode/isode/dsap/common/RCS/conf.c,v 9.0 1992/06/16 12:12:39 isode Rel
 *
 *
 * Log: conf.c,v
 * Revision 9.0  1992/06/16  12:12:39  isode
 * Release 8.0
 *
 */

/*
 *                                NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */

/* LINTLIBRARY */

#include "quipu/util.h"
#include "tailor.h"

int sizelimit = 20;
int timelimit = 30;

char *oidtable = "oidtable";
char *tailfile = "dsaptailor";
char *isotailor = NULLCP;

char *dsa_address = NULLCP;
char *myname = NULLCP;

char *local_dit = NULLCP;		/* the part of the tree local to the users */
char dishinit = FALSE;

int dsap_id,				/* global last id sent */
 dsap_ad;				/* global association descriptor */

time_t cache_timeout = 21600;		/* Keep cache entries for 6 hours */

static LLog _ldsap_log = {
	"dsap.log",
	NULLCP,
	NULLCP,
	LLOG_FATAL | LLOG_EXCEPTIONS,
	LLOG_NONE,
	-1,
	LLOGZER | LLOGCRT | LLOGCLS,
	NOTOK
};

LLog *log_dsap = &_ldsap_log;

#ifndef NO_STATS

static LLog lstat_log = {
	"quipu.log",
	NULLCP,
	NULLCP,
	LLOG_NOTICE | LLOG_TRACE,
	LLOG_NONE,
	-1,
	LLOGCRT | LLOGZER,
	NOTOK
};

LLog *log_stat = &lstat_log;

#endif

int oidformat = 1;			/* oid format, 1=part, 2=full, 3=numeric */

/* a quick def incase quipu/malloc.c is not compiled in !!! */
unsigned mem_heap = 0;

char *dsaoidtable = "oidtable";
char *dsatailfile = "quiputailor";
char *treedir = "quipu-db";
int no_last_mod = FALSE;
char startup_update = FALSE;
int search_level = 2;			/* do NOT allow country level searching */
int read_only = FALSE;			/* Prevent DIT modification */

				/* admin limits */
int admin_size = 100;			/* 100 entries */
time_t admin_time = 120;		/* don't spend more than 2 minutes on a task */
time_t conn_timeout = 300;		/* don't hold an unused connection open for more than 5
					   minutes */
time_t nsap_timeout = 45;		/* after 45 seconds assume nsap has failed */

time_t slave_timeout = 0;		/* Update slaves every 'cache_time' seconds */
time_t retry_timeout = 0;		/* Test DSA for reliability after 'cache_time' seconds */

unsigned watchdog_time = 5 * 60;	/* allow lower layers 5 minutes per *async* operation */
unsigned watchdog_delta = 30;		/* allow 30 second for timeout to propagate */

char dsa_mode = FALSE;

char quipu_faststart = FALSE;
