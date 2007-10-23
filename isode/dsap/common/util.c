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

/* util.c - General utility routines */

#ifndef lint
static char *rcsid =
    "Header: /xtel/isode/isode/dsap/common/RCS/util.c,v 9.0 1992/06/16 12:12:39 isode Rel";
#endif

/*
 * Header: /xtel/isode/isode/dsap/common/RCS/util.c,v 9.0 1992/06/16 12:12:39 isode Rel
 *
 *
 * Log: util.c,v
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
#include "quipu/commonarg.h"
#include "quipu/malloc.h"
#include "tsap.h"
#include <varargs.h>

extern LLog *log_dsap;
extern char dsa_mode;

char *
SkipSpace(ptr)
	register char *ptr;
{
	if (ptr == NULLCP)
		return (NULLCP);

	while (isascii(*ptr) && isspace(*ptr))
		ptr++;
	return (ptr);
}

void
StripSpace(b)
	register char *b;

/* copy b to a less spaces and comments */
{
	register char *a;

	if (*b == COMMENT) {
		*b = 0;
		return;
	}

	while (isascii(*b) && (!isspace(*b)) && (*b))
		b++;

	if (!*b)
		return;		/* nothing needs doing */

	a = b;

	for (;;) {
		if (isascii(*b) && isspace(*b))
			b++;
		else if ((!*b) || (*b == COMMENT))
			break;
		else
			*a++ = *b++;
	}

	*a = 0;
}

void
StripSpace2(b)
	register char *b;

/* copy b to a less spaces and comments */
{
	register char *a;

	while (isascii(*b) && (!isspace(*b)) && (*b))
		b++;

	if (!*b)
		return;		/* nothing needs doing */

	a = b;

	for (;;) {
		if (isascii(*b) && isspace(*b))
			b++;
		else if (!*b)
			break;
		else
			*a++ = *b++;
	}

	*a = 0;
}

char *
TidyString2(a)
	register char *a;
{
	register char *b;
	char *c;
	register int i = 0;

	/* removing multiple and trailing spaces */
	c = a, b = a;
	while (*a) {
		if (isascii(*a) && isspace(*a)) {
			*b = ' ';	/* make sure not a tab etc */
			a++;
			while (isascii(*a) && isspace(*a)) {
				a++;
				i = 1;
			}

			if (*a)
				b++;
			else
				break;
		}
		if (i)
			*b = *a;

		a++, b++;
	}

	*b = 0;

	if (*--b == '\n')
		*b-- = 0;

	if (*b == ' ')
		*b = 0;

	return (c);
}

char *
TidyString(a)
	register char *a;
{
	register char *b;
	char *c;
	register int i = 0;

	if (!*a)
		return (a);

	/* remove white space from front of string */
	while (isascii(*a) && isspace(*a))
		a++;

	/* removing multiple and trailing spaces */
	c = a, b = a;
	while (*a) {
		if (isascii(*a) && isspace(*a)) {
			*b = ' ';	/* make sure not a tab etc */
			a++;
			while (isascii(*a) && isspace(*a)) {
				a++;
				i = 1;
			}

			if (*a)
				b++;
			else
				break;
		}
		if (i)
			*b = *a;

		a++, b++;
	}

	*b = 0;

	if (*--b == '\n')
		*b-- = 0;

	if (*b == ' ')
		*b = 0;

	return (c);
}

test_prim_pe(pe, class, id)
	PE pe;
	PElementClass class;
	PElementID id;
{
	if (pe == NULLPE)
		return FALSE;

	if (pe->pe_form != PE_FORM_PRIM) {
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("Primative Attribute Value expected"));
		return FALSE;
	}

	if (PE_ID(pe->pe_class, pe->pe_id) != PE_ID(class, id)) {
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("Bad class/ID in Attribute Value"));
		return FALSE;
	}

	return (TRUE);

}

#ifndef lint
ps_printf(va_alist)
	va_dcl
{
	PS ps;
	extern int std_flush();
	va_list ap;

	va_start(ap);

	ps = va_arg(ap, PS);

#ifdef VSPRINTF

	if (ps->ps_flushP == std_flush) {
		char *fmt;

		fmt = va_arg(ap, char *);

		if (vfprintf((FILE *) ps->ps_addr, fmt, ap) == EOF)
			ps->ps_errno = PS_ERR_IO;

	} else
#endif

	{
		char buffer [8192]; 	/* How big should this go !!! */
		
		_xsprintf (buffer,NULLCP,ap);

		ps_print(ps, buffer);
	}

	va_end(ap);
}
#else

/* VARARGS2 */
ps_printf(ps, fmt)
	PS ps;
	char *fmt;
{
	(void) ps_printf(ps, fmt);
}

#endif

fatal(code, fmt)
	int code;
	char *fmt;
{
	if (dsa_mode)
		log_dsap->ll_syslog = LLOG_FATAL;
	LLOG(log_dsap, LLOG_FATAL, ("Quipu failure (%d): %s", code, fmt));
	stop_listeners();
	(void) fflush(stdout);
	(void) fprintf(stderr, "Quipu failure (%d): %s\n", code, fmt);
	exit(code);
}

static PS ps = NULLPS;

pslog(lp, event, str, func, ptr)
	LLog *lp;
	int event;
	char *str;
	int (*func) ();			/* assumes func (PS ,dataptr,(int) format); */
	caddr_t ptr;
{
	/* log info to pstream */

	if (!(lp->ll_events & event))
		return;

	if (ps == NULL && (((ps = ps_alloc(str_open)) == NULLPS)
			   || str_setup(ps, NULLCP, BUFSIZ, 0) == NOTOK)) {
		if (ps)
			ps_free(ps), ps = NULLPS;

		return;
	}

	(*func) (ps, ptr, EDBOUT);
	ps_print(ps, " ");
	*--ps->ps_ptr = NULL, ps->ps_cnt++;

	LLOG(lp, event, ("%s: %s", str, ps->ps_base));

	ps->ps_cnt = ps->ps_bufsiz;
	ps->ps_ptr = ps->ps_base;
}

stop_listeners()
{
	struct TSAPdisconnect td_s;
	struct TSAPdisconnect *td = &(td_s);

	/* close all listeners */
	(void) TNetClose(NULLTA, td);
}

quipu_pe_cmp(a, b)
	register PE a, b;
{
	char *p, *q;
	register int j, i;

	/* based on ISODE pe_cmp */

	if (a == NULLPE) {
		if (b == NULLPE)
			return (0);
		else
			return (1);
	}
	if (b == NULLPE)
		return (-1);

	if (a->pe_class != b->pe_class)
		if (a->pe_class > b->pe_class)
			return (1);
		else
			return (-1);

	if (a->pe_form != b->pe_form)
		if (a->pe_form > b->pe_form)
			return (1);
		else
			return (-1);

	if (a->pe_id != b->pe_id)
		if (a->pe_id > b->pe_id)
			return (1);
		else
			return (-1);

	switch (a->pe_form) {
	case PE_FORM_ICONS:
		if (a->pe_ilen != a->pe_ilen)
			return (a->pe_ilen > b->pe_ilen ? 1 : -1);
	case PE_FORM_PRIM:
		if ((i = a->pe_len) != b->pe_len)
			if (i > b->pe_len)
				return (1);
			else
				return (-1);

		p = (char *) a->pe_prim;
		q = (char *) b->pe_prim;

		for (j = 0; j < i; j++) {
			if (*p != *q)
				if (*p > *q)
					return (1);
				else
					return (-1);
			p++;
			q++;
		}
		return (0);

	case PE_FORM_CONS:
		for (a = a->pe_cons, b = b->pe_cons; a; a = a->pe_next, b = b->pe_next)
			if ((i = quipu_pe_cmp(a, b)) != 0)
				return (i);
		return (b ? 1 : 0);
	default:
		return (1);

	}
}

IFP acl_fn = NULLIFP;

struct acl_info *
acl_dflt()
{
	if (acl_fn == NULLIFP)
		return ((struct acl_info *) NULL);
	else
		return ((struct acl_info *) (*acl_fn) ());
}
