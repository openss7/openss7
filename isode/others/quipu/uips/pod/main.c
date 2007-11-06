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

/* main.c - main routine for pod */

#ifndef lint
static char *rcsid =
    "Header: /xtel/isode/isode/others/quipu/uips/pod/RCS/main.c,v 9.0 1992/06/16 12:44:54 isode Rel";
#endif

/*
 * Header: /xtel/isode/isode/others/quipu/uips/pod/RCS/main.c,v 9.0 1992/06/16 12:44:54 isode Rel
 */

#include "quipu/util.h"
#include "quipu/photo.h"
#include "quipu/common.h"
#include "quipu/entry.h"
#include "quipu/dua.h"
#include "pod.h"

#include <varargs.h>

extern int print_parse_errors;
extern char *oidtable, *tailfile, *myname;
extern char namestr[], passwd[];
extern Widget toplevel, outer;
extern bool testing;
extern str_seq alias_seq, aliased_dn_seq;

int podphoto(), quipu_pe_cmp();
void CreateWidgets(), PodLoop(), displayStartupError();
char *cnnct_bind();
void kill_message(), message();
void put_dn_and_password();

void user_tailor();
void read_bind_args(), read_args(), quit();

void exit();

main(argc, argv)
	unsigned int argc;
	char **argv;
{
	char **null_argv = (char **) NULL;
	char *mess;

	extern int ch_set;

	ch_set = TRUE;

	print_parse_errors = FALSE;
	quipu_syntaxes();

#ifdef USE_PP
	pp_quipu_init(argv[0]);
#endif

	want_oc_hierarchy();

	namestr[0] = '\0';
	passwd[0] = '\0';

	toplevel = XtInitialize("X-Directory", "Pod", NULL, 0, &argc, argv);

	dsap_init((int *) NULL, &null_argv);

#ifdef USE_PP
	pp_quipu_run();
#endif

	read_args(&argc, &argv);
	user_tailor();
	read_bind_args(&argc, &argv);

	CreateWidgets();
	message((Widget) NULL, "Connecting to Directory. Please Wait...");

	if ((mess = cnnct_bind()) != NULLCP) {
		kill_message();
		displayStartupError(mess);
		XtMainLoop();
	}

	set_attribute_syntax(str2syntax("photo"),
			     (IFP) pe_cpy, NULLIFP,
			     NULLIFP, podphoto,
			     (IFP) pe_cpy, quipu_pe_cmp, pe_free, NULLCP, NULLIFP, TRUE);

	kill_message();
	PodLoop();

	return 0;
}

void
read_args(acptr, avptr)
	unsigned int *acptr;
	char ***avptr;
{
	register char *cp;
	char **av;

	if (acptr == (unsigned int *) NULL)
		return;
	if (*acptr <= 1)
		return;

	av = *avptr;
	av++;

	while ((cp = *av) && (*cp == '-')) {
		switch (*++cp) {
		case 'T':
			if (*++av != NULLCP)
				load_oid_table(*av);
			break;
		case 'c':
			if (*++av != NULLCP)
				myname = *av;
			break;
		case 't':
			if (lexequ(*av, "-test") != 0) {
				if (*++av != NULLCP)
					tailfile = *av;
			} else {
				testing = TRUE;
			}
			break;
		default:
			av++;
		}
		av++;
	}
}

void
read_bind_args(acptr, avptr)
	unsigned int *acptr;
	char ***avptr;
{
	register char *cp;
	char **av;
	bool got_user_name = FALSE, got_password = FALSE;

	if (acptr == (unsigned int *) NULL)
		return;
	if (*acptr <= 1)
		return;

	av = *avptr;
	av++;

	while ((cp = *av) && (*cp == '-')) {
		switch (*++cp) {
		case 'u':
			if (*++av != NULLCP) {
				put_dn_and_password(namestr, passwd, *av);
				got_user_name = TRUE;
				if (passwd[0] != '\0')
					got_password = TRUE;
			}
			break;
		case 'p':
			if (*++av != NULLCP) {
				(void) strcpy(passwd, *av);
				got_password = TRUE;
			}
			break;
		default:
			av++;
		}
		av++;
	}
	if (got_user_name == TRUE && got_password == FALSE)
		passwd[0] = '\0';
}

void
quit(sig)
	int sig;
{
	(void) ds_unbind();
	exit(sig);
}

advise(va_alist)
	va_dcl
{
	int code;
	va_list ap;
	extern LLog *log_dsap;

	va_start(ap);

	code = va_arg(ap, int);

	(void) _ll_log(log_dsap, code, ap);

	va_end(ap);
}
