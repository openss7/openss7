/*
 *    Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>
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

#ident "@(#) LiS slog.c 2.2 11/27/00 21:16:57 "

#include <stdarg.h>

#include <sys/stream.h>
#include <sys/cmn_err.h>
#include <sys/osif.h>

#ifndef NULL
#define NULL		( (void *) 0 )
#endif

extern       char                    *lis_strm_print_trace_buf;

/************************************************************************
*                            lis_strm_trace                             *
*************************************************************************
*									*
* printf to the debug/testing output "device".  The tracing may be 	*
* disabled on installed systems.  This is not a logging mechanism or	*
* audit trail since the traced events may not be recorded anywhere.	*
*									*
* If you want print trace printout to be compatible with the token	*
* printer, your fmt should begin (not end) with a newline (\n).		*
*									*
************************************************************************/
void
lis_strm_trace(char *fmt, ...)
{
	va_list		arg_ptr ;

	if (lis_print_trace == (lis_print_trace_t) 0)
	    return;

	va_start(arg_ptr, fmt) ;
	vsprintf(lis_strm_print_trace_buf, fmt, arg_ptr) ;
	va_end(arg_ptr) ;

	(*lis_print_trace) (lis_strm_print_trace_buf);

} /* lis_strm_trace */

/************************************************************************
*                          lis_strm_log                                 *
*************************************************************************
*									*
* The intent of this routine is to be used to log events that would	*
* likely be of interest to an administrator.				*
*									*
************************************************************************/
void
lis_strm_log(int log_level, char *fmt, ...)
{
	va_list		arg_ptr ;

	if (lis_print_trace == (lis_print_trace_t) 0)
	    return;

	va_start(arg_ptr, fmt) ;
	vsprintf(lis_strm_print_trace_buf, fmt, arg_ptr) ;
	va_end(arg_ptr) ;

	(*lis_print_trace) (lis_strm_print_trace_buf);

} /* lis_strm_log */

