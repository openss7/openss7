/************************************************************************
*                           cmn_err                                     *
*************************************************************************
*									*
* This is the UNIX common-error message routine.			*
*									*
* For now we will simply turn this into a printk().  Later we can	*
* add some putbuf code.							*
*									*
* Author:	David Grothe	<dave@gcom.com>				*
*									*
* Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>		*
*									*
************************************************************************/
/*
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Cambridge,
 * MA 02139, USA.
 * 
 */

#ident "@(#) LiS cmn_err.c 2.10 8/14/02 20:17:44 "

#include <sys/strport.h>
#include <sys/cmn_err.h>
#include <stdarg.h>
#include <sys/osif.h>

/*
 * Check to see if the symbols for kernel message level have
 * been defined.  If so, use them, if not use null string
 * defaults.  In the case that we are compiled into the Linux
 * kernel this will have the effect of mapping cmn_err message
 * levels to Linux kernel message priorities.  For other
 * environments this will not apply.
 *
 * If we are in Solaris compatible mode then omit the kernel
 * message identifiers.  In Solaris mode the newline goes at the
 * end rather than at the beginning so the kernel flags might
 * be misinterpreted.
 *
 * For SVR4 the newline goes first so it is safe to interpolate the
 * kernel message type tags.
 */

#if defined(SOLARIS_STYLE_CMN_ERR)	/* config option */

#define	PRE_NL		""
#undef  KERN_NOTICE
#define	KERN_NOTICE	""
#undef  KERN_WARNING
#define	KERN_WARNING	""
#undef  KERN_EMERG
#define	KERN_EMERG	""

#else					/* SVR4 style cmn_err */

#define	PRE_NL		"\n"
#ifndef KERN_NOTICE
#define	KERN_NOTICE	""
#endif
#ifndef KERN_WARNING
#define	KERN_WARNING	""
#endif
#ifndef KERN_EMERG
#define	KERN_EMERG	""
#endif
#ifndef KERN_INFO
#define	KERN_INFO	""
#endif

#endif

lis_print_trace_t      lis_print_trace;

char	    lis_cmn_err_buf[4096];
#define	buf lis_cmn_err_buf

void	lis_cmn_err(int err_lvl, char *fmt, ...)
{
    extern int	 vsprintf (char *, const char *, va_list);
    va_list	 args;
    char	*p ;

    switch (err_lvl)
    {
    case CE_CONT:		/* continue printing */
	buf[0] = 0 ;
	break ;
    case CE_NOTE:		/* NOTICE */
	strcpy(buf, PRE_NL KERN_NOTICE "NOTICE: ") ;
	break ;
    case CE_WARN:		/* WARNING */
	strcpy(buf, PRE_NL KERN_WARNING "WARNING: ") ;
	break ;
    case CE_PANIC:		/* PANIC */
	strcpy(buf, PRE_NL KERN_EMERG "PANIC: ") ;
	break ;
    default:
	sprintf(buf, "\n" KERN_NOTICE
		     "cmn_err:  Called with invalid arguments "
		     "(0x%lx, 0x%lx)\n",
		     (long) err_lvl, (long) fmt) ;
	if ( lis_print_trace )
	    (*lis_print_trace) (buf);
	return ;
    }

    for (p = buf; *p; p++) ;		/* find end of string */

    va_start (args, fmt);
    vsprintf (p, fmt, args);
    va_end (args);

#if defined(SOLARIS_STYLE_CMN_ERR)	/* config option */
    if (err_lvl != CE_CONT)
    {
	for (; *p; p++) ;		/* find end of string */
	*p++ = '\n' ;			/* add newline at end */
	*p   = 0 ;
    }
#endif

    if (err_lvl == CE_PANIC)
	PANIC(buf);
    else
	if ( lis_print_trace )
	    (*lis_print_trace) (buf);

} /* lis_cmn_err */
