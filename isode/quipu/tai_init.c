/* tai_init.c - */

#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/quipu/RCS/tai_init.c,v 9.0 1992/06/16 12:34:01 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/quipu/RCS/tai_init.c,v 9.0 1992/06/16 12:34:01 isode Rel $
 *
 *
 * $Log: tai_init.c,v $
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
#include "tailor.h"

extern  char    *dsatailfile;

#define MAXTAIARGS      100

extern  LLog    *log_dsap;


dsa_tai_init(name)
char    *name;
{
	FILE    *fp;
	char    buf[BUFSIZ];
	char   *cp;

	isodetailor (name,0);

	if( (fp = fopen(cp = isodefile(dsatailfile, 0), "r")) == (FILE *)NULL){
		LLOG (log_dsap,LLOG_FATAL, ("Cannot open tailor file '%s'", cp));
		fatal (-46, "Cannot open quiputailor");
	}

	while(fgets(buf, sizeof(buf), fp) != NULLCP)
		if ( (*buf != '#') && (*buf != '\n') )
			/* not a comment or blank */
			if (dsa_tai_string (buf) == NOTOK)
				LLOG (log_dsap,LLOG_EXCEPTIONS,("tai_string failed %s",buf));

	(void) fclose(fp);
	isodexport(NULLCP);
	return OK;
}


dsa_tai_string (str)
char * str;
{
	char    *args[MAXTAIARGS];
	char    *p;
	int     ac;

	if( (p = index(str, '\n')) != NULLCP)
		*p = '\0';

	if((ac = sstr2arg(str, MAXTAIARGS, args, " \t,")) == NOTOK) {
		LLOG (log_dsap,LLOG_EXCEPTIONS,("too many tailor parameters"));
		return(NOTOK);
	}
	if(ac <= 1) {
		LLOG (log_dsap,LLOG_EXCEPTIONS,("no option set",str));
		return (NOTOK);
	}
	return (dsa_sys_tai(ac, args));

}
