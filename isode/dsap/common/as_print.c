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

#ifndef lint
static char *rcsid =
    "Header: /xtel/isode/isode/dsap/common/RCS/as_print.c,v 9.0 1992/06/16 12:12:39 isode Rel";
#endif

/*
 * Header: /xtel/isode/isode/dsap/common/RCS/as_print.c,v 9.0 1992/06/16 12:12:39 isode Rel
 *
 *
 * Log: as_print.c,v
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

#include "quipu/util.h"
#include "quipu/attrvalue.h"

extern AttributeType last_at;
extern int avs_count;
short inherit_sntx = -1;
extern char dsa_mode;

as_comp_print(ps, as, format)
	PS ps;
	Attr_Sequence as;
	int format;
{
	AV_Sequence avs;
	char buffer[LINESIZE];
	extern int oidformat;
	char *ptr;

	last_at = as->attr_type;
	avs_count = 1;

	if (format == READOUT) {
		(void) sprintf(buffer, "%s", attr2name(as->attr_type, oidformat));
		ptr = buffer;
	} else
		ptr = attr2name_aux(as->attr_type);

	if (split_attr(as))
		if (as->attr_value == NULLAV)
			if (format != READOUT)
				ps_printf(ps, "%s=\n", ptr);
			else
				ps_printf(ps, "%-21s - NO VALUE\n", ptr);
		else {
			if (format != READOUT)
				ps_printf(ps, "%s= ", ptr);
			for (avs = as->attr_value; avs != NULLAV;
			     avs = avs->avseq_next, avs_count++) {
				if (format == READOUT)
					ps_printf(ps, "%-21s - ", ptr);
				avs_comp_print(ps, avs, format);
				if ((format != READOUT) && (avs->avseq_next))
					if (!dsa_mode || (as->attr_type->oa_syntax == inherit_sntx))
						ps_printf(ps, "\n%s= ", ptr);
					else
						ps_print(ps, " &\\\n\t");
				else
					ps_print(ps, "\n");
			}
	} else {
		if (format == READOUT)
			ps_printf(ps, "%-21s - ", ptr);
		else
			ps_printf(ps, "%s= ", ptr);
		avs_print(ps, as->attr_value, format);
	}

	avs_count = 1;
}

as_print(ps, as, format)
	Attr_Sequence as;
	PS ps;
	int format;
{
	register Attr_Sequence eptr;

	for (eptr = as; eptr != NULLATTR; eptr = eptr->attr_link)
		as_comp_print(ps, eptr, format);

}
