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

#ifndef __SNMP_VIEW_G_H__
#define __SNMP_VIEW_G_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* view-g.h - VIEW group */

/* 
 * Header: /xtel/isode/isode/snmp/RCS/view-g.h,v 9.0 1992/06/16 12:38:11 isode Rel
 *
 *
 * Log: view-g.h,v
 * Revision 9.0  1992/06/16  12:38:11  isode
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

#include "isoaddrs.h"
#include "internet.h"
#include "psap.h"

/* VIEWS */

#define	inSubtree(tree,object) \
    	((tree) -> oid_nelem <= (object) -> oid_nelem \
	     && bcmp ((char *) (tree) -> oid_elements, \
		      (char *) (object) -> oid_elements, \
		      (tree) -> oid_nelem \
		          * sizeof ((tree) -> oid_elements[0])) == 0)

struct subtree {
	struct subtree *s_forw;		/* doubly-linked list */
	struct subtree *s_back;		/* doubly-linked list */

	OID s_subtree;			/* subtree */
};

struct view {
	struct view *v_forw;		/* doubly-linked list */
	struct view *v_back;		/* .. */

	OID v_name;			/* view name */
	u_long v_mask;			/* view mask */

	struct subtree v_subtree;	/* list of subtrees */

	struct qbuf *v_community;	/* for proxy, traps... */
	struct sockaddr v_sa;

	unsigned int *v_instance;	/* object instance */
	int v_insize;			/* .. */
};

extern struct view *VHead;

/* COMMUNITIES */

struct community {
	struct community *c_forw;	/* doubly-linked list */
	struct community *c_back;	/* .. */

	char *c_name;			/* community name */
	struct NSAPaddr c_addr;		/* network address */

	int c_permission;		/* same as ot_access */
#define	OT_YYY	0x08

	OID c_vu;			/* associated view */
	struct view *c_view;		/* .. */

	unsigned int *c_instance;	/* object instance */
	int c_insize;			/* .. */
	struct community *c_next;	/* next in lexi-order */
};

extern struct community *CHead;

/* TRAPS */

struct trap {
	struct trap *t_forw;		/* doubly-linked list */
	struct trap *t_back;		/* .. */

	char *t_name;			/* trap name */

	struct view t_vu;		/* associated view */
	struct view *t_view;		/* .. */

	u_long t_generics;		/* generic traps enabled */

	unsigned int *t_instance;	/* object instance */
	int t_insize;			/* .. */
};

extern struct trap *UHead;

#endif				/* __SNMP_VIEW_G_H__ */
