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

#ifndef __ISODE_QUIPU_TURBO_H__
#define __ISODE_QUIPU_TURBO_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* turbo.h - your comments here */

/* 
 * Header: /xtel/isode/isode/h/quipu/RCS/turbo.h,v 9.0 1992/06/16 12:23:11 isode Rel
 *
 *
 * Log: turbo.h,v
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

#ifndef QUIPUTURBO
#define QUIPUTURBO

#include "quipu/config.h"
#include "quipu/name.h"

/*
 * this structure represents a generic avl tree node.
 */

typedef struct avlnode {
	caddr_t avl_data;
	char avl_bf;
	struct avlnode *avl_left;
	struct avlnode *avl_right;
} Avlnode;

#define NULLAVL	((Avlnode *) NULL)

/* balance factor values */
#define LH 	-1
#define EH 	0
#define RH 	1

/* avl routines */
#define avl_getone(x)	(x == 0 ? 0 : (x)->avl_data)
#define avl_onenode(x)	(x == 0 || ((x)->avl_left == 0 && (x)->avl_right == 0))
extern int avl_insert();
extern caddr_t avl_delete();
extern caddr_t avl_find();
extern caddr_t avl_getfirst();
extern caddr_t avl_getnext();
extern int avl_dup_error();
extern int avl_apply();

/* apply traversal types */
#define AVL_PREORDER	1
#define AVL_INORDER	2
#define AVL_POSTORDER	3
/* what apply returns if it ran out of nodes */
#define AVL_NOMORE	-6

#ifdef TURBO_INDEX

/*
 * this structure represents an attribute index.  the index is composed
 * of the attribute type, a count of the number of different values in
 * the tree, and a pointer to the root of the tree of attribute values.
 * these nodes are linked together (one node for each attribute type being
 * optimized) in an avl tree in each index node.  there is also
 * a tree and count for soundex values of the attribute.  and now there's
 * also a tree and count for reversed values of the attribute (for final
 * substring queries).
 */

typedef struct index {
	/* entry associated with this index */
	/* sibling => parent */
	DN i_dn;			/* subtree => base */

	/* for subtree index: descendants */
	/* not held locally */
	struct entry **i_nonleafkids;

	/* for both: aliases that escape */
	/* the scope of the index */
	struct entry **i_nonlocalaliases;

	AttributeType i_attr;		/* the attribute type */
	int i_count;			/* number of entries in this tree */
	int i_rcount;			/* number of ents in reverse tree */
	int i_scount;			/* number of ents in soundex tree */
	Avlnode *i_root;		/* tree of values */
	Avlnode *i_rroot;		/* tree of reverse values */
	Avlnode *i_sroot;		/* tree of soundex values */
} Index;

#define NULLINDEX	((Index *) 0)

typedef struct {
	struct entry *ep_entry;
	int ep_count;
} eptr_node;

typedef struct index_node {
	caddr_t in_value;
	struct entry **in_entries;
	int in_num;
	int in_max;
} Index_node;

#define NULLINDEXNODE	((Index_node *) 0)

#define get_subtree_index(x) \
	((Index *) avl_find( subtree_index, (caddr_t) (x), idn_cmp ))

#define get_sibling_index(x) \
	((Index *) avl_find( sibling_index, (caddr_t) (x), idn_cmp ))

#endif				/* TURBO_INDEX */
#endif				/* QUIPUTURBO */

#endif				/* __ISODE_QUIPU_TURBO_H__ */
