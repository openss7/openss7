/* tai_args.c - Argument processing routines */

#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/quipu/RCS/tai_args.c,v 9.0 1992/06/16 12:34:01 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/quipu/RCS/tai_args.c,v 9.0 1992/06/16 12:34:01 isode Rel $
 *
 *
 * $Log: tai_args.c,v $
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


#include "quipu/util.h"
#include "quipu/attr.h"
#ifdef	TCP
#include "internet.h"

extern	int	_listen_opts;
#endif

extern char *dsaoidtable,
	    *dsatailfile,
	    *mydsaname,
	    *treedir;

static char        *usage = "[-t <tailor>] [-c <dsa name>] [-T <oidtable>] [-D <directory>]";

extern LLog * log_dsap;

dsa_tai_args (acptr,avptr)
int *acptr;
char ***avptr;
{
char ** av;
register char *cp;
int cnt;
extern char quipu_faststart;

	if (acptr == (int *)NULL)
		return;

	av = *avptr;
	av++, cnt = 1;

	while ((cp = *av) && *cp == '-') {
		switch (*++cp) {
			case 'T': dsaoidtable = *++av;
				  cnt++;
					break;
			case 'D': treedir = *++av;
				  cnt++;
					break;
			case 'c': mydsaname = *++av;
				  cnt++;
					break;
			case 't': dsatailfile = *++av;
				 cnt++;
				 break;

			case 'f': 
			         quipu_faststart = 1;
			         break;

			case 's': 
			         quipu_faststart = 0;
			         break;

			case 'r':
#ifdef	SO_REUSEADDR
				_listen_opts = SO_REUSEADDR;
#endif
				break;

			default:
				LLOG (log_dsap,LLOG_FATAL,("Unknown option\nUsage: %s %s\n",*avptr[0],usage));
				fatal(-46,"Usage...");
		}
		av++;
		cnt++;
	}

	*acptr -= cnt;
	*avptr = av;
}
