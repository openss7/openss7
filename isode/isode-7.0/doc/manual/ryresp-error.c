...

void	ros_adios (rop, event)
register struct RoSAPpreject *rop;
char   *event;
{
    ros_advise (rop, event);

    longjmp (toplevel, NOTOK);
}


void	ros_advise (rop, event)
register struct RoSAPpreject *rop;
char   *event;
{
    char    buffer[BUFSIZ];

    if (rop -> rop_cc > 0)
	(void) sprintf (buffer, "[%s] %*.*s", RoErrString (rop -> rop_reason),
		rop -> rop_cc, rop -> rop_cc, rop -> rop_data);
    else
	(void) sprintf (buffer, "[%s]", RoErrString (rop -> rop_reason));

    advise (LLOG_EXCEPTIONS, NULLCP, "%s: %s", event, buffer);
}


void	acs_advise (aca, event)
register struct AcSAPabort *aca;
char   *event;
{
    char    buffer[BUFSIZ];

    if (aca -> aca_cc > 0)
	(void) sprintf (buffer, "[%s] %*.*s",
		AcErrString (aca -> aca_reason),
		aca -> aca_cc, aca -> aca_cc, aca -> aca_data);
    else
	(void) sprintf (buffer, "[%s]", AcErrString (aca -> aca_reason));

    advise (LLOG_EXCEPTIONS, NULLCP, "%s: %s (source %d)", event, buffer,
		aca -> aca_source);
}


#ifndef	lint
void	adios (va_alist)
va_dcl
{
    va_list ap;

    va_start (ap);
    
    _ll_log (pgm_log, LLOG_FATAL, ap);

    va_end (ap);

    _exit (1);
}
#else
/* VARARGS2 */

void	adios (what, fmt)
char   *what,
       *fmt;
{
    adios (what, fmt);
}
#endif


#ifndef	lint
void	advise (va_alist)
va_dcl
{
    int	    code;
    va_list ap;

    va_start (ap);

    code = va_arg (ap, int);

    _ll_log (pgm_log, code, ap);

    va_end (ap);
}
#else
/* VARARGS3 */

void	advise (code, what, fmt)
char   *what,
       *fmt;
int	code;
{
    advise (code, what, fmt);
}
#endif
