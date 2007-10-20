/* tai_init.c - */

#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/tai_init.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/dsap/common/RCS/tai_init.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: tai_init.c,v $
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

extern LLog * log_dsap;
extern  char    *tailfile;

#define MAXTAIARGS      100

extern  LLog    *log_dsap;


dsap_tai_init()
{
	FILE    *fp;
	char    *cp;
	char    buf[BUFSIZ];

	if(!isstr(tailfile))    /* it's compiled in */
		return(OK);

	if( (fp = fopen(cp = isodefile(tailfile, 0), "r")) == (FILE *)NULL) {
		LLOG (log_dsap,LLOG_FATAL,("can't open tailor file '%s'", cp));
		fatal (-1, "Cannot open tailor file");
	}

	while(fgets(buf, sizeof(buf), fp) != NULLCP)
		if ( (*buf != '#') && (*buf != '\n') )
			/* not a comment or blank */
			if (tai_string (buf) == NOTOK) 
				LLOG (log_dsap,LLOG_EXCEPTIONS,("tai_string failed %s",buf));

	
	(void) fclose(fp);
	return OK;
}


tai_string (str)
char * str;
{
	char    *args[MAXTAIARGS];
	char    *p;
	int     ac;

	if( (p = index(str, '\n')) != NULLCP)
		*p = '\0';

	if((ac = sstr2arg(str, MAXTAIARGS, args, " \t,")) == NOTOK) {
		LLOG(log_dsap,LLOG_EXCEPTIONS,("too many tailor parameters"));
		return(NOTOK);
	}
	if(ac <= 1) {
		LLOG (log_dsap,LLOG_EXCEPTIONS,("no option set '%s'",str));
		return (NOTOK);
	}
	return (dsap_tai(ac, args));

}
