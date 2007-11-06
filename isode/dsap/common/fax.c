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

/* fax.c - facsimileTelephoneNumber attribute */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/dsap/common/RCS/fax.c,v 9.0 1992/06/16 12:12:39 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/dsap/common/RCS/fax.c,v 9.0 1992/06/16 12:12:39 isode Rel
 *
 *
 * Log: fax.c,v
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
	SYNTAX:
		fax ::= <printablestring> [ <parameters> ]
		parameters ::= <parm> | <parm> $ <parameters>
		<parm> ::= "twoDimensional" | "fineResolution"
				| "unlimitedLength" | "b4Length" | "a3Width"
				| "b4Width" | "uncompressed"

	EXAMPLE:
		123-4567 $ twoDimensional
*/

/* LINTLIBRARY */

#include "quipu/util.h"
#include "quipu/entry.h"
#include "quipu/syntaxes.h"

extern LLog *log_dsap;

/*  */

struct pair {
	char *p_name;
	int p_value;
};

static struct pair pairs[] = {
	"twoDimensional", 8,
	"fineResolution", 9,
	"unlimitedLength", 20,
	"b4Length", 21,
	"a3Width", 22,
	"b4Width", 23,
	"uncompressed", 30,

	NULL
};

/*  */

static
fax_free(f)
	register struct fax *f;
{
	free(f->number);

	if (f->bits)
		pe_free(f->bits);

	free((char *) f);
}

/*  */

static struct fax *
fax_cpy(a)
	register struct fax *a;
{
	register struct fax *f;

	f = (struct fax *) smalloc(sizeof *f);

	f->number = strdup(a->number);
	f->bits = a->bits ? pe_cpy(a->bits) : NULLPE;

	return f;
}

/*  */

static int
fax_cmp(a, b)
	register struct fax *a;
	register struct fax *b;
{
	int i;

	if (a == (struct fax *) NULL)
		return (b ? -1 : 0);
	else if (b == (struct fax *) NULL)
		return 1;

	if (i = telcmp(a->number, b->number))
		return i;

	return pe_cmp(a->bits, b->bits);
}

/*  */

static
fax_print(ps, f, format)
	register PS ps;
	register struct fax *f;
	int format;
{
	register int i;
	register struct pair *p;
	register PE pe;

	if (format == READOUT) {
		ps_printf(ps, "%s", f->number);

		if ((pe = f->bits) && (i = pe->pe_nbits) > 0) {
			char *cp = " {";

			while (i-- >= 0)
				if (bit_test(pe, i) > OK) {
					for (p = pairs; p->p_name; p++)
						if (p->p_value == i)
							break;
					if (p->p_name)
						ps_printf(ps, "%s %s", cp, p->p_name);
					else
						ps_printf(ps, "%s %d", cp, i);
					cp = ",";
				}

			if (*cp == ',')
				ps_print(ps, " }");
		}
	} else {
		ps_printf(ps, "%s", f->number);

		if ((pe = f->bits) && (i = pe->pe_nbits) > 0) {
			char *cp = " $";

			while (i-- >= 0)
				if (bit_test(pe, i) > OK) {
					for (p = pairs; p->p_name; p++)
						if (p->p_value == i)
							break;
					if (p->p_name)
						ps_printf(ps, "%s %s", cp, p->p_name);
					else
						ps_printf(ps, "%s %d", cp, i);
					cp = "";
				}
		}
	}
}

/*  */

static struct fax *
str2fax(str)
	register char *str;
{
	int value;
	register char *ptr, **ap;
	char *vec[NVEC + 1];
	register struct fax *f;
	register struct pair *p;

	f = (struct fax *) smalloc(sizeof *f);

	if (ptr = index(str, '$'))
		*ptr = NULL;
	if ((int) strlen(str) > UB_TELEPHONE_NUMBER) {
		parse_error("fax phone number too big", NULLCP);
		free((char *) f);
		return ((struct fax *) NULL);
	}

	FAST_TIDY(str);
	f->number = strdup(str);
	f->bits = NULLPE;

	if (!ptr)
		return f;

	*ptr++ = '$';
	ptr = strdup(ptr);

	bzero((char *) vec, sizeof vec);
	(void) str2vec(ptr, vec);

	for (ap = vec; *ap; ap++) {
		if (sscanf(*ap, "%d", &value) == 1 && value >= 0)
			goto got_value;

		for (p = pairs; p->p_name; p++)
			if (lexequ(p->p_name, *ap) == 0)
				break;
		if (!p->p_name) {
			parse_error("unknown G3fax non-basic parameter: '%s'", *ap);

		      you_lose:;
			free(ptr);
			free(f->number);
			if (f->bits)
				pe_free(f->bits);
			free((char *) f);

			return ((struct fax *) NULL);
		}
		value = p->p_value;

	      got_value:;
		if ((f->bits == NULLPE
		     && (f->bits = prim2bit(pe_alloc(PE_CLASS_UNIV, PE_FORM_PRIM, PE_PRIM_BITS)))
		     == NULLPE)
		    || bit_on(f->bits, value) == NOTOK) {
		      no_allocate:;
			parse_error("unable to allocate G3fax non-basic parameter", NULLCP);
			goto you_lose;
		}
	}

	if (bit2prim(f->bits) == NULLPE)
		goto no_allocate;

	free(ptr);

	return f;
}

/*  */

static PE
fax_enc(f)
	struct fax *f;
{
	PE pe = NULLPE;

	f->fax_bits = bitstr2strb(f->bits, &f->fax_len);

	(void) encode_SA_FacsimileTelephoneNumber(&pe, 0, 0, NULLCP, f);

	if (f->fax_bits)
		free(f->fax_bits);

	return pe;
}

/*  */

static struct fax *
fax_dec(pe)
	PE pe;
{
	struct fax *f;

	if (decode_SA_FacsimileTelephoneNumber(pe, 1, NULLIP, NULLVP, &f)
	    == NOTOK) {
		return ((struct fax *) NULL);
	}

	if ((int) strlen(f->number) > UB_TELEPHONE_NUMBER) {
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("fax number string too big"));
		return ((struct fax *) NULL);
	}

	if (f->fax_bits) {
		f->bits = strb2bitstr(f->fax_bits, f->fax_len, PE_CLASS_UNIV, PE_PRIM_BITS);

		free(f->fax_bits);
	}

	return f;
}

/*  */

fax_syntax()
{
	(void) add_attribute_syntax("FacsimileTelephoneNumber",
				    (IFP) fax_enc, (IFP) fax_dec,
				    (IFP) str2fax, fax_print,
				    (IFP) fax_cpy, fax_cmp, fax_free, NULLCP, NULLIFP, TRUE);
}
