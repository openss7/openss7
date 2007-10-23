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

/* docstore.c - documentStore syntax */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/dsap/common/RCS/docstore.c,v 9.0 1992/06/16 12:12:39 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/dsap/common/RCS/docstore.c,v 9.0 1992/06/16 12:12:39 isode Rel
 *
 *
 * Log: docstore.c,v
 * Revision 9.0  1992/06/16  12:12:39  isode
 * Release 8.0
 *
 */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */

/*
	documentStoreSyntax ATTRIBUTE-SYNTAX
	    SEQUENCE {
	        method
		    INTEGER { ftp(0), ftam(1) },

	        textEncodedHostName
		    IA5String,

	        directoryName[0]
		    IA5String
		    OPTIONAL,

	        fileFsName
		    IA5String
	    }
	    MATCHES FOR EQUALITY

	SYNTAX:
		documentStore ::= [ "ftp" | "ftam" ]
					`$' <IA5String>
				      [ `$' <IA5String> ]
					`$' <IA5String>

	EXAMPLE:
		ftp $ nic.ddn.mil $ rfc: $ %s.txt
*/

/* LINTLIBRARY */

#include "quipu/util.h"
#include "quipu/entry.h"
#include "quipu/syntaxes.h"

static
documentStore_free(a)
	struct documentStore *a;
{
	if (!a)
		return;

	if (a->ds_host)
		free(a->ds_host);
	if (a->ds_dir)
		free(a->ds_dir);
	if (a->ds_file)
		free(a->ds_file);

	free((char *) a);
}

static struct documentStore *
documentStore_cpy(a)
	struct documentStore *a;
{
	struct documentStore *b;

	b = (struct documentStore *) smalloc(sizeof *b);
	bzero((char *) b, sizeof *b);

	b->ds_method = a->ds_method;
	b->ds_host = strdup(a->ds_host);
	if (a->ds_dir)
		b->ds_dir = strdup(a->ds_dir);
	b->ds_file = strdup(a->ds_file);

	return b;
}

static
documentStore_cmp(a, b)
	struct documentStore *a;
	struct documentStore *b;
{
	int res;

	if (!a)
		return (b ? (-1) : 0);
	else if (!b)
		return 1;

	if (res = b->ds_method - a->ds_method)
		return (res > 0 ? 1 : -1);
	return lexequ(a->ds_host, b->ds_host);
}

static
documentStore_print(ps, a, format)
	PS ps;
	struct documentStore *a;
	int format;
{
	if (format == READOUT) {
		ps_printf(ps, "use %s to %s and get %s",
			  a->ds_method ? "ftam" : "ftp", a->ds_host, a->ds_file);
		if (a->ds_dir)
			ps_printf(ps, " (look under %s)", a->ds_dir);
	} else
		ps_printf(ps, "%s $ %s $ %s $ %s",
			  a->ds_method ? "ftam" : "ftp", a->ds_host,
			  a->ds_dir ? a->ds_dir : "", a->ds_file);
}

static struct documentStore *
str2documentStore(str)
	char *str;
{
	int method;
	char *d1, *d2, *d3, *ptr;
	struct documentStore *a;

	if ((d1 = index(str, '$')) == NULL || (d2 = index(d1 + 1, '$')) == NULL) {
		parse_error("seperator missing in documentStore '%s'", str);
		return NULL;
	}
	*d1 = NULL;

	for (ptr = str + strlen(str) - 1; ptr >= str && isspace(*ptr); ptr--)
		continue;
	*++ptr = NULL;

	if (lexequ(str, "ftp") == 0)
		method = DS_FTP;
	else if (lexequ(str, "ftam") == 0)
		method = DS_FTAM;
	else
		method = DS_UNK;

	*d1 = '$';
	if (ptr != d1)
		*ptr = ' ';

	if (method == DS_UNK) {
		parse_error("unknown method in documentStore '%s'", str);
		return NULL;
	}

	a = (struct documentStore *) smalloc(sizeof *a);
	bzero((char *) a, sizeof *a);
	a->ds_method = method;

	*d2 = NULL;
	a->ds_host = strdup(SkipSpace(d1 + 1));
	*d2 = '$';

	if (d3 = index(d2 + 1, '$')) {
		*d3 = NULL;
		a->ds_dir = strdup(SkipSpace(d2 + 1));
		*d3 = '$';

		a->ds_file = strdup(SkipSpace(d3 + 1));
	} else
		a->ds_file = strdup(SkipSpace(d2 + 1));

	d1 = a->ds_host;
	for (ptr = d1 + strlen(d1) - 1; ptr >= d1 && isspace(*ptr); ptr--)
		continue;
	*++ptr = NULL;

	if (d1 = a->ds_dir) {
		for (ptr = d1 + strlen(d1) - 1; ptr >= d1 && isspace(*ptr); ptr--)
			continue;
		*++ptr = NULL;
	}

	d1 = a->ds_file;
	for (ptr = d1 + strlen(d1) - 1; ptr >= d1 && isspace(*ptr); ptr--)
		continue;
	*++ptr = NULL;

	return a;
}

static PE
documentStore_enc(a)
	struct documentStore *a;
{
	PE pe;

	(void) encode_Thorn_DocumentStoreSyntax(&pe, 0, 0, NULLCP, a);

	return pe;
}

static struct documentStore *
documentStore_dec(pe)
	PE pe;
{
	struct documentStore *a;

	if (decode_Thorn_DocumentStoreSyntax(pe, 1, NULLIP, NULLVP, &a) == NOTOK)
		return NULL;

	return a;
}

documentStore_syntax()
{
	(void) add_attribute_syntax("documentStoreSyntax",
				    (IFP) documentStore_enc,
				    (IFP) documentStore_dec,
				    (IFP) str2documentStore,
				    documentStore_print,
				    (IFP) documentStore_cpy,
				    documentStore_cmp, documentStore_free, NULLCP, NULLIFP, TRUE);
}
