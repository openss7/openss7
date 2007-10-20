...

/*    ERRORS */

#include <varargs.h>


/* VARARGS2 */

void	adios (what, fmt, va_alist)
char   *what,
       *fmt;
va_dcl
{
    advise (what, fmt, va_alist);

    _exit (1);
}


/* VARARGS2 */

void	advise (what, fmt, va_alist)
char   *what,
       *fmt;
va_dcl
{
    char    buffer[BUFSIZ];

    asprintf (buffer, what, fmt, va_alist);

    (void) fflush (stdout);

    fprintf (stderr, "%s: ", myname);
    (void) fputs (buffer, stderr);
    (void) fputc ('\n', stderr);

    (void) fflush (stderr);
}

%}
