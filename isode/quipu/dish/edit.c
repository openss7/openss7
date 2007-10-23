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

/* edit.c - */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/quipu/dish/RCS/edit.c,v 9.0 1992/06/16 12:35:39 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/quipu/dish/RCS/edit.c,v 9.0 1992/06/16 12:35:39 isode Rel
 *
 *
 * Log: edit.c,v
 * Revision 9.0  1992/06/16  12:35:39  isode
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

#include "manifest.h"
#include "quipu/util.h"
#include "psap.h"
#include "tailor.h"
#include "sys.file.h"
#include <sys/stat.h>

extern char fname[];

#define	OPT	(!frompipe || rps -> ps_byteno == 0 ? opt : rps)
#define	RPS	(!frompipe || opt -> ps_byteno == 0 ? rps : opt)
extern char frompipe;
extern PS opt, rps;

editentry(argc, argv)
	int argc;
	char **argv;
{
	char str[LINESIZE];
	char prog[LINESIZE];
	int res;
	extern char inbuf[];
	extern int fd;
	extern char remote_prob;
	extern char dad_flag;

	if (argc != 1) {
		Usage(argv[0]);
		return (NOTOK);
	}

	(void) sprintf(str, "%s %s", _isodefile(isodebinpath, "editentry"), fname);

	if (!frompipe)
		return (system(str) ? NOTOK : OK);

	if (!dad_flag) {
		(void) sprintf(prog, "e%s\n", str);

		send_pipe_aux(prog);

		if ((res = read_pipe_aux(prog, sizeof prog)) < 1) {
			(void) fprintf(stderr, "read failure\n");
			remote_prob = TRUE;
			return (NOTOK);
		} else {
			if ((res == 1) && (*prog == 'e')) {
				remote_prob = FALSE;
				return (NOTOK);	/* remote error - abandon ! */
			}
			if (*fname != '/') {
				char tempbuf[LINESIZE];

				/* relative path... prefix cwd */
				*(prog + res) = 0;
				(void) sprintf(tempbuf, "%s/%s", prog, fname);
				(void) strcpy(fname, tempbuf);
			}
		}
	} else {
#ifndef	SOCKETS
		ps_printf(OPT, "operation not allowed when using directory assistance server!\n");
		return NOTOK;
#else
		int cc, i, j;
		char *cp, *dp;
		FILE *fp;
		struct stat st;
		extern int errno;

		if ((fp = fopen(fname, "r+")) == NULL) {
			ps_printf(OPT, "unable to open %s for rw: %s\n", fname, sys_errname(errno));
			return NOTOK;
		}
		if (fstat(fileno(fp), &st) == NOTOK
		    || (st.st_mode & S_IFMT) != S_IFREG || (cc = st.st_size) == 0) {
			ps_printf(OPT, "%s: not a regular file\n", fname);
		      out:;
			(void) fclose(fp);
			return NOTOK;
		}

		(void) sprintf(prog, "e%d\n", cc);
		send_pipe_aux(prog);

		if ((res = read_pipe_aux(prog, sizeof prog)) < 1) {
			(void) fprintf(stderr, "read failure\n");
			remote_prob = TRUE;
			goto out;
		} else if ((res == 1) && (*prog == 'e')) {
			remote_prob = FALSE;
			goto out;
		}

		if ((cp = malloc((unsigned) (cc))) == NULL) {
			ps_printf(OPT, "out of memory\n");
			goto out;
		}
		for (dp = cp, j = cc; j > 0; dp += i, j -= i)
			switch (i = fread(dp, sizeof *dp, j, fp)) {
			case NOTOK:
				ps_printf(OPT, "error reading %s: %s\n", fname, sys_errname(errno));
				goto out2;

			case OK:
				ps_printf(OPT, "premature eof reading %s\n", fname);
			      out2:;
				free(cp);
				goto out;

			default:
				break;
			}

		send_pipe_aux2(cp, cc);
		free(cp), cp = NULL;

		if ((res = read_pipe_aux2(&cp, &cc)) < 1) {
			(void) ps_printf(OPT, "read failure\n");
			remote_prob = TRUE;
			goto out;
		}
		if (res == 1) {
			if (*cp != 'e')
				(void) ps_printf(OPT, "remote protocol error: %s\n", cp);
			goto out;
		}

		(void) fclose(fp);
		if ((fp = fopen(fname, "w")) == NULL) {
			ps_printf(OPT, "unable to re-open %s for writing: %s\n",
				  fname, sys_errname(errno));
			free(cp);
			return NOTOK;
		}

		if (fwrite(cp, sizeof *cp, cc, fp) == 0) {
			ps_printf(OPT, "error writing %s: %s\n", fname, sys_errname(errno));
			goto out2;
		}

		free(cp);
		(void) fclose(fp);
#endif
	}

	return (OK);
}

get_password(str, buffer)
	char *str;
	char *buffer;
{

	char prog[LINESIZE];
	int res;
	extern char inbuf[];
	extern int fd;
	extern char remote_prob;
	char *getpassword();

	if (frompipe) {
		(void) sprintf(prog, "p%s\n", str);

		send_pipe_aux(prog);

		if ((res = read_pipe_aux(prog, sizeof prog)) < 1) {
			(void) fprintf(stderr, "read failure\n");
			remote_prob = TRUE;
			return;
		} else {
			*(prog + res) = 0;
			(void) strcpy(buffer, prog + 1);
		}
	} else {
		(void) sprintf(buffer, "Enter password for \"%s\": ", str);
		(void) strcpy(buffer, getpassword(buffer));
	}
}

yesno(str)
	char *str;
{
	char prog[LINESIZE];
	extern char inbuf[];
	extern int fd;
	extern char remote_prob;
	char *getpassword();

	if (frompipe) {
		(void) sprintf(prog, "y%s\n", str);

		send_pipe_aux(prog);

		if (read_pipe_aux(prog, sizeof prog) < 1) {
			(void) fprintf(stderr, "read failure\n");
			remote_prob = TRUE;
			return FALSE;
		}
	} else {
		ps_printf(OPT, "%s", str);
		(void) fgets(prog, sizeof prog, stdin);
	}

	switch (prog[0]) {
	case 'y':
		return OK;

	case 'n':
	default:
		return NOTOK;

	case 'N':
		return DONE;
	}
}
