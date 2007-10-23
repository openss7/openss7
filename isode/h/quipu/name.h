/*****************************************************************************

 @(#) $Id$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 3 of the License.

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

#ifndef __ISODE_QUIPU_NAME_H__
#define __ISODE_QUIPU_NAME_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* name.h - */

/*
 * Header: /xtel/isode/isode/h/quipu/RCS/name.h,v 9.0 1992/06/16 12:23:11 isode Rel
 *
 *
 * Log: name.h,v
 * Revision 9.0  1992/06/16  12:23:11  isode
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

#ifndef QUIPUNAME
#define QUIPUNAME

#include "quipu/attr.h"

typedef struct ava {			/* represents AttributeValueAssertion */
	AttributeType ava_type;
	AttributeValue ava_value;
} ava, AVA;

typedef struct rdncomp {		/* RDN is sequence of attribute value */
	/* assertions */
	/* represents RelativeDistinguishedName */
	attrType rdn_at;
	attrVal rdn_av;
	struct rdncomp *rdn_next;
} rdncomp, *RDN;

#define NULLRDN ((RDN) 0)
#define rdn_comp_alloc()          (RDN) smalloc(sizeof(rdncomp))
RDN rdn_comp_new();
RDN rdn_comp_cpy();
RDN str2rdn();
RDN rdn_cpy();
RDN rdn_merge();

typedef struct dncomp {			/* DN is sequence of RDNs.  */
	/* represents RDNSequence which is */
	/* equivalent to DistinguishedName */
	RDN dn_rdn;
	struct dncomp *dn_parent;
} dncomp, *DN;

#define NULLDN ((DN) 0)

#define dn_comp_alloc()        (DN) smalloc(sizeof(dncomp))
#define dn_comp_print(x,y,z)   if (y!=NULLDN) rdn_print(x,y->dn_rdn,z)
#define dn_comp_fill(x,y)     x -> dn_rdn = y
#define dn_comp_cmp(x,y)      ((rdn_cmp (x->dn_rdn ,y->dn_rdn) == OK) ? OK : NOTOK )

DN dn_comp_new();
DN dn_comp_cpy();
DN dn_cpy();
DN str2dn();

char *dn2str();
char *dn2ufn();
char *dn2rfc();

void dn_print();
void dn_rfc_print();
void ufn_dn_print();
void ufn_rdn_print();

int ufn_dn_print_aux();

extern int ufn_indent;

#endif

#endif				/* __ISODE_QUIPU_NAME_H__ */
