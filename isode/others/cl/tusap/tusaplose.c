/* tusaplose.c - TPM: you lose */

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
#include <varargs.h>
#include "tpkt.h"

/*  */

#ifndef	lint
int	tusaplose (va_alist)
va_dcl
{
    int	    reason,
    	    result;
    struct TSAPdisconnect *td;
    va_list ap;

    va_start (ap);

    td = va_arg (ap, struct TSAPdisconnect *);
    reason = va_arg (ap, int);

    result = _tusaplose (td, reason, ap);

    va_end (ap);

    return result;

}
#else
/* VARARGS */

int	tusaplose (td, reason, what, fmt)
struct TSAPdisconnect *td;
int	reason;
char   *what,
       *fmt;
{
    return tusaplose (td, reason, what, fmt);
}
#endif

/*  */

#ifndef	lint
static int  _tusaplose (td, reason, ap)	/* what, fmt, args ... */
register struct TSAPdisconnect *td;
int     reason;
va_list	ap;
{
    register char  *bp;
    char    buffer[BUFSIZ];

    if (td) {
	bzero ((char *) td, sizeof *td);

	asprintf (bp = buffer, ap);
	bp += strlen (bp);

	td -> td_reason = reason;
	copyTSAPdata (buffer, bp - buffer, td);
    }

    return NOTOK;
}
#endif
