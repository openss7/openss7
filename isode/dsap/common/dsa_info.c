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

/* dsa_info.c - DSA Operational Information */

#ifndef lint
static char *rcsid =
    "Header: /xtel/isode/isode/dsap/common/RCS/dsa_info.c,v 9.0 1992/06/16 12:12:39 isode Rel";
#endif

/*
 * Header: /xtel/isode/isode/dsap/common/RCS/dsa_info.c,v 9.0 1992/06/16 12:12:39 isode Rel
 *
 *
 * Log: dsa_info.c,v
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
#include "quipu/entry.h"
#include "quipu/syntaxes.h"
extern LLog *log_dsap;

static
edb_info_free(edb)
	struct edb_info *edb;
{
	if (edb == NULLEDB)
		return;

	dn_free(edb->edb_name);
	dn_free(edb->edb_getfrom);
	dn_seq_free(edb->edb_allowed);
	dn_seq_free(edb->edb_sendto);
	free((char *) edb);
}

static struct edb_info *
edb_info_cpy(a)
	struct edb_info *a;
{
	struct edb_info *result;

	result = edb_info_alloc();
	result->edb_name = dn_cpy(a->edb_name);
	result->edb_getfrom = dn_cpy(a->edb_getfrom);
	result->edb_sendto = dn_seq_cpy(a->edb_sendto);
	result->edb_allowed = dn_seq_cpy(a->edb_allowed);
	return (result);
}

static
edb_info_cmp(a, b)
	struct edb_info *a;
	struct edb_info *b;
{
	int i;

	if (a == NULLEDB)
		return (b == NULLEDB ? 0 : -1);

	if (b == NULLEDB)
		return (1);

	if ((i = dn_cmp(a->edb_name, b->edb_name)) != 0)
		return (i);

	if ((i = dn_cmp(a->edb_getfrom, b->edb_getfrom)) != 0)
		return (i);

	if ((i = dn_seq_cmp(a->edb_sendto, b->edb_sendto)) != 0)
		return (i);

	if ((i = dn_seq_cmp(a->edb_allowed, b->edb_allowed)) != 0)
		return (i);

	return (0);

}

static struct edb_info *
edb_info_decode(pe)
	PE pe;
{
	struct edb_info *a;

	if (decode_Quipu_EDBInfoSyntax(pe, 1, NULLIP, NULLVP, &a) == NOTOK) {
		return (NULLEDB);
	}
	return (a);
}

static
edb_info_print(ps, edb, format)
	PS ps;
	struct edb_info *edb;
	int format;
{
	if (edb == NULLEDB)
		return;

	if (format == READOUT) {
		if (edb->edb_name != NULLDN)
			dn_print(ps, edb->edb_name, EDBOUT);
		else
			ps_print(ps, "ROOT");

		if (edb->edb_getfrom != NULLDN) {
			ps_print(ps, " ( FROM ");
			dn_print(ps, edb->edb_getfrom, EDBOUT);
			ps_print(ps, " )");
			if (edb->edb_allowed != NULLDNSEQ) {
				ps_print(ps, "\n\t\t\t");
				if (edb->edb_name != NULLDN)
					dn_print(ps, edb->edb_name, EDBOUT);
				else
					ps_print(ps, "ROOT");
			}
		}
		if (edb->edb_allowed != NULLDNSEQ) {
			ps_print(ps, " ( TO ");
			dn_seq_print(ps, edb->edb_allowed, READOUT);
			ps_print(ps, " )");
		}
	} else {
		if (edb->edb_name != NULLDN)
			dn_print(ps, edb->edb_name, EDBOUT);
		ps_print(ps, "#");
		dn_print(ps, edb->edb_getfrom, EDBOUT);
		ps_print(ps, "#");
		dn_seq_print(ps, edb->edb_allowed, format);
	}

	if (edb->edb_sendto != NULLDNSEQ)
		DLOG(log_dsap, LLOG_EXCEPTIONS, ("edb_sendto not null"));

}

static struct edb_info *
str2update(str)
	char *str;
{
	register char *ptr;
	char *save, val;
	struct edb_info *ei;

	/* dn # num # dn # dnseq # */
	if ((ptr = index(str, '#')) == 0) {
		parse_error("# missing in update syntax '%s'", str);
		return (NULLEDB);
	}
	ei = edb_info_alloc();
	ei->edb_sendto = NULLDNSEQ;

	/* go for name */
	save = ptr++;
	if (*str == '#')
		ei->edb_name = NULLDN;
	else {
		if (!isspace(*--save))
			save++;
		val = *save;
		*save = 0;

		if ((ei->edb_name = str2dn(str)) == NULLDN) {
			free((char *) ei);
			return (NULLEDB);
		}
		*save = val;
	}

	str = SkipSpace(ptr);
	if ((ptr = index(str, '#')) == 0) {
		parse_error("2nd # missing in update syntax '%s'", str);
		dn_free(ei->edb_name);
		free((char *) ei);
		return (NULLEDB);
	}

	save = ptr++;
	if (*str == '#') {
		ei->edb_getfrom = NULLDN;
	} else {
		if (!isspace(*--save))
			save++;
		val = *save;
		*save = 0;

		if ((ei->edb_getfrom = str2dn(str)) == NULLDN) {
			dn_free(ei->edb_name);
			free((char *) ei);
			return (NULLEDB);
		}
		*save = val;
	}

	/* send to */
	str = SkipSpace(ptr);
	if ((ptr = index(str, '#')) == 0) {
		ptr = 0;
	} else {
		save = ptr++;
		if (!isspace(*--save))
			save++;
		val = *save;
		*save = 0;
	}

	if (*str == 0)
		ei->edb_allowed = NULLDNSEQ;
	else if ((ei->edb_allowed = str2dnseq(str)) == NULLDNSEQ) {
		parse_error("invalid dn '%s'", str);
		dn_free(ei->edb_name);
		dn_free(ei->edb_getfrom);
		free((char *) ei);
		return (NULLEDB);
	}

	if (ptr != 0)
		*save = val;

	return (ei);
}

static PE
edb_info_enc(ei)
	struct edb_info *ei;
{
	PE ret_pe;

	(void) encode_Quipu_EDBInfoSyntax(&ret_pe, 0, 0, NULLCP, ei);
	return (ret_pe);
}

edbinfo_syntax()
{
	(void) add_attribute_syntax("edbinfo",
				    (IFP) edb_info_enc, (IFP) edb_info_decode,
				    (IFP) str2update, edb_info_print,
				    (IFP) edb_info_cpy, edb_info_cmp,
				    edb_info_free, NULLCP, NULLIFP, TRUE);
}
