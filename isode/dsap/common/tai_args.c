/* tai_args.c - Argument processing routines */

#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/tai_args.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/dsap/common/RCS/tai_args.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: tai_args.c,v $
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
#include "quipu/attr.h"

extern char *oidtable,
	    *tailfile,
	    *myname;

extern LLog *log_dsap;

char        *dsap_usage = "[flags]";
char        *options = "T:t:c:";

tai_args (acptr,avptr)
int *acptr;
char ***avptr;
{
register char ** av;

int cnt;
register char *cp;

	if (acptr == (int *)NULL)
		return;

	if (*acptr <= 1)
		return;

	av = *avptr;
	av++, cnt = 1;

	while ((cp = *av) && *cp == '-') {
		switch (*++cp) {
			case 'T': oidtable = *++av;
				  cnt++;
					break;
			case 'c': myname = *++av;
				  cnt++;
					break;
			case 't': tailfile = *++av;
				  cnt++;
					break;

			default:
				LLOG (log_dsap,LLOG_FATAL,("Usage: %s %s\n",*avptr[0],dsap_usage));
				fatal(-1,"Usage...");
		}
		av++;
		cnt++;
	}

	*acptr -= cnt;
	*avptr = av;
}
