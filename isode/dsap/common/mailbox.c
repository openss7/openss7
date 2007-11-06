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

/* mailbox.c - otherMailbox attribute */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/dsap/common/RCS/mailbox.c,v 9.0 1992/06/16 12:12:39 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/dsap/common/RCS/mailbox.c,v 9.0 1992/06/16 12:12:39 isode Rel
 *
 *
 * Log: mailbox.c,v
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
		mailbox ::= <printablestring> '$' <IA5String>
	
	EXAMPLE:
		internet $ quipu-support@cs.ucl.ac.uk
*/

/* LINTLIBRARY */

#include "quipu/util.h"
#include "quipu/entry.h"
#include "quipu/syntaxes.h"

static
mailbox_free(ptr)
	struct mailbox *ptr;
{
	free(ptr->mbox);
	free(ptr->mtype);

	free((char *) ptr);
}

static struct mailbox *
mailbox_cpy(a)
	struct mailbox *a;
{
	struct mailbox *result;

	result = (struct mailbox *) smalloc(sizeof(struct mailbox));
	result->mbox = strdup(a->mbox);
	result->mtype = strdup(a->mtype);
	return (result);
}

static
mailbox_cmp(a, b)
	struct mailbox *a;
	struct mailbox *b;
{
	int res;

	if (a == (struct mailbox *) NULL)
		if (b == (struct mailbox *) NULL)
			return (0);
		else
			return (-1);

	if ((res = lexequ(a->mbox, b->mbox)) != 0)
		return (res);
	if ((res = lexequ(a->mtype, b->mtype)) != 0)
		return (res);
	return (0);
}

static
mailbox_print(ps, mail, format)
	register PS ps;
	struct mailbox *mail;
	int format;
{
	if (format == READOUT)
		ps_printf(ps, "%s: %s", mail->mtype, mail->mbox);
	else
		ps_printf(ps, "%s $ %s", mail->mtype, mail->mbox);
}

static struct mailbox *
str2mailbox(str)
	char *str;
{
	struct mailbox *result;
	char *ptr;
	char *mark = NULLCP;

	if ((ptr = index(str, '$')) == NULLCP) {
		parse_error("seperator missing in mailbox '%s'", str);
		return ((struct mailbox *) NULL);
	}

	result = (struct mailbox *) smalloc(sizeof(struct mailbox));
	*ptr-- = 0;
	if (isspace(*ptr)) {
		*ptr = 0;
		mark = ptr;
	}
	ptr++;
	result->mtype = strdup(str);
	*ptr++ = '$';
	result->mbox = strdup(SkipSpace(ptr));

	if (mark != NULLCP)
		*mark = ' ';

	return (result);
}

static PE
mail_enc(m)
	struct mailbox *m;
{
	PE ret_pe;

	(void) encode_Thorn_MailBox(&ret_pe, 0, 0, NULLCP, m);

	return (ret_pe);
}

static struct mailbox *
mail_dec(pe)
	PE pe;
{
	struct mailbox *m;

	if (decode_Thorn_MailBox(pe, 1, NULLIP, NULLVP, &m) == NOTOK) {
		return ((struct mailbox *) NULL);
	}
	return (m);
}

mailbox_syntax()
{
	(void) add_attribute_syntax("Mailbox",
				    (IFP) mail_enc, (IFP) mail_dec,
				    (IFP) str2mailbox, mailbox_print,
				    (IFP) mailbox_cpy, mailbox_cmp,
				    mailbox_free, NULLCP, NULLIFP, TRUE);
}
