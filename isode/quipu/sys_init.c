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

/* sys_init.c - System tailoring initialisation */

#ifndef lint
static char *rcsid =
    "Header: /xtel/isode/isode/quipu/RCS/sys_init.c,v 9.0 1992/06/16 12:34:01 isode Rel";
#endif

/*
 * Header: /xtel/isode/isode/quipu/RCS/sys_init.c,v 9.0 1992/06/16 12:34:01 isode Rel
 *
 *
 * Log: sys_init.c,v
 * Revision 9.0  1992/06/16  12:34:01  isode
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

#include <stdio.h>
#include "quipu/oid.h"
#include "quipu/config.h"
#include "tailor.h"
#include "logger.h"
#include "usr.dirent.h"
#include <sys/stat.h>

extern char *dsaoidtable;
extern LLog *log_dsap;
extern time_t cache_timeout;
extern time_t retry_timeout;
extern time_t slave_timeout;
extern char *edbtmp_path;
extern char *treedir;

extern int errno;

static int rmFiles();

dsa_sys_init(acptr, avptr)
	int *acptr;
	char ***avptr;
{
	char *name;
	char **ptr;
	int cnt;
	extern int parse_line;
	extern char dsa_mode;

#ifdef  TURBO_DISK
	extern char quipu_faststart;

	quipu_faststart = 1;
#endif

	parse_line = 0;		/* stop 'line 1:' being printed in tailor file errors */
	dsa_mode = 1;

	name = **avptr;

	DLOG(log_dsap, LLOG_TRACE, ("Initialisation"));

	cnt = *acptr;
	ptr = *avptr;
	dsa_tai_args(acptr, avptr);

	if (dsa_tai_init(name) != OK)
		fatal(-43, "Tailoring failed");

	dsa_tai_args(&cnt, &ptr);	/* second call IS needed !!! */

	DLOG(log_dsap, LLOG_TRACE, ("Loading oid table (%s)", dsaoidtable));

	if (load_oid_table(dsaoidtable) == NOTOK)
		fatal(-43, "Can't load oid tables");

	if (retry_timeout == (time_t) 0)
		retry_timeout = cache_timeout;

	if (slave_timeout == (time_t) 0)
		slave_timeout = cache_timeout;

}

void
mk_dsa_tmp_dir()
{
	struct stat statbuf;
	char edbtmp_buf[BUFSIZ];
	char err_buf[BUFSIZ];

	(void) sprintf(edbtmp_buf, "%stmp", treedir);

	(void) strcat(edbtmp_buf, "/");
	edbtmp_path = strdup(edbtmp_buf);
	edbtmp_buf[strlen(edbtmp_path) - 1] = 0;	/* remove "/" */

	if ((stat(edbtmp_buf, &statbuf) == OK)
	    && ((statbuf.st_mode & S_IFMT) == S_IFDIR)) {
		/* tmpdir exists - clean it */
		struct dirent **namelist;

		(void) _scandir(edbtmp_buf, &namelist, rmFiles, NULLIFP);
		if (namelist)
			free((char *) namelist);

	} else if (mkdir(edbtmp_buf, 0700) != 0) {
		(void) sprintf(err_buf, "Can't create tmp directory: %s (%d)", edbtmp_path, errno);
		fatal(-43, err_buf);
	}
}

static int
rmFiles(entry)
	struct dirent *entry;
{
	char cbuf[BUFSIZ];

	if (*entry->d_name == '.' && (strcmp(entry->d_name, ".") == 0)
	    || (strcmp(entry->d_name, "..") == 0))
		return 0;

	(void) strcpy(cbuf, edbtmp_path);
	(void) strcat(cbuf, entry->d_name);

	if (unlink(cbuf) == NOTOK) {
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("remove failure \"%s\")", cbuf, errno));
		return 0;
	}

	return 0;
}
