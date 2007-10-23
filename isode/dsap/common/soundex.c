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

/* soundex.c - compare a filter and attribute value */

#ifndef lint
static char *rcsid =
    "Header: /xtel/isode/isode/dsap/common/RCS/soundex.c,v 9.0 1992/06/16 12:12:39 isode Rel";
#endif

/*
 * Header: /xtel/isode/isode/dsap/common/RCS/soundex.c,v 9.0 1992/06/16 12:12:39 isode Rel
 *
 *
 * Log: soundex.c,v
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

#include <ctype.h>
#include "quipu/util.h"
#include "quipu/attrvalue.h"
#include "quipu/ds_search.h"

extern char chrcnv[];
extern LLog *log_dsap;

#ifndef SOUNDEX_LEN
#define SOUNDEX_LEN	-1
#endif

#define iswordbreak(x)	(!isascii(x) || isspace((unsigned char) (x)) || \
	ispunct((unsigned char) (x)) || \
	isdigit((unsigned char) (x)) || x == '\0')

char *
first_word(ptr)
	char *ptr;
{
	if (ptr == NULLCP)
		return NULLCP;

	while (iswordbreak(*ptr))
		if (*ptr == 0)
			return NULLCP;
		else
			ptr++;

	return (ptr);
}

char *
next_word(ptr)
	register char *ptr;
{
	if (ptr == NULLCP)
		return NULLCP;

	for (;;) {
		if (*ptr == 0)
			return NULLCP;
		else if (iswordbreak(*ptr)) {
			while (*ptr && iswordbreak(*ptr))
				ptr++;
			if (*ptr == 0)
				return NULLCP;
			else
				return (ptr);
		} else
			ptr++;
	}
	/* NOTREACHED */
}

soundex(s, c)
	char *s;
	char **c;
{
	char code, adjacent, ch, *p;
	int i, cmax;

	p = s;
	if (*p == '\0') {
		*c = (char *) malloc(sizeof(char));
		**c = '\0';
		return;
	}

	/* assume at least four...realloc if we need to */
	*c = (char *) malloc(5 * sizeof(char));
	cmax = 4;

	adjacent = '0';
	if (islower(*p))
		(*c)[0] = toupper(*p);
	else
		(*c)[0] = *p;

	(*c)[1] = '\0';
	for (i = 0; i < 99 && (!iswordbreak(*p)); p++) {
		if (islower(*p))
			ch = toupper(*p);
		else
			ch = *p;

		code = '0';

		switch (ch) {
		case 'B':
		case 'F':
		case 'P':
		case 'V':
			code = (adjacent != '1') ? '1' : '0';
			break;
		case 'S':
		case 'C':
		case 'G':
		case 'J':
		case 'K':
		case 'Q':
		case 'X':
		case 'Z':
			code = (adjacent != '2') ? '2' : '0';
			break;
		case 'D':
		case 'T':
			code = (adjacent != '3') ? '3' : '0';
			break;
		case 'L':
			code = (adjacent != '4') ? '4' : '0';
			break;
		case 'M':
		case 'N':
			code = (adjacent != '5') ? '5' : '0';
			break;
		case 'R':
			code = (adjacent != '6') ? '6' : '0';
			break;
		default:
			adjacent = '0';
		}

		if (i == 0) {
			adjacent = code;
			i++;
		} else if (code != '0') {
			if (i == SOUNDEX_LEN)
				break;
			if (i == cmax) {
				*c = (char *) realloc(*c, (unsigned) (2 * cmax * sizeof(char) + 1));
				cmax *= 2;
			}
			adjacent = (*c)[i] = code;
			i++;
		}
	}

	if (i > 0)
		(*c)[i] = '\0';

	return;
}

static char *g_bcode;
static int g_bcodelen;

static
match_word(a)
	char *a;
{
	char *as;
	int cmp;

	soundex(a, &as);

#ifdef SOUNDEX_PREFIX
	cmp = strncmp(as, g_bcode, g_bcodelen);
#else
	cmp = strcmp(as, g_bcode);
#endif
	free(as);
	return (cmp == 0);
}

soundex_cmp(a, b)
	register char *a;
	register char *b;
{
	char result = FALSE;
	register char *ptr;

	for (; a && b; b = next_word(b)) {
		soundex(b, &g_bcode);
		g_bcodelen = strlen(g_bcode);
		for (ptr = first_word(a); ptr; ptr = next_word(ptr)) {
			if (match_word(ptr)) {
				a = next_word(ptr);
				result = TRUE;
				break;
			}
		}
		free(g_bcode);
		if (ptr == NULLCP)
			return FALSE;
	}
	if ((a == NULL) && (b != NULL))
		return FALSE;

	return (result);
}

soundex_match(fitem, avs)
	register struct filter_item *fitem;
	register AV_Sequence avs;
{
	for (; avs != NULLAV; avs = avs->avseq_next)
		if (soundex_cmp
		    ((char *) avs->avseq_av.av_struct, (char *) fitem->UNAVA.ava_value->av_struct))
			return (OK);

	return (NOTOK);
}
