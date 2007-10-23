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

#ifndef __QUIPU_UIPS_DOOG_QUERY_DITMODEL_H__
#define __QUIPU_UIPS_DOOG_QUERY_DITMODEL_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

#ifndef _query_dit_model_h_
#define _query_dit_model_h_

#include "types.h"
#include "dn_list.h"
#include "util.h"
#include "sequence.h"

typedef struct _object_type {
	AttributeType object_type;
	QBool use_subtree;
	struct _object_type *next;
} *objectType, *objectTypeList;

#define NULLObjectType (objectType) NULL
#define object_type_alloc() (objectType) smalloc(sizeof(struct _object_type))

typedef struct _dit_relation {
	AttributeType parent_type;
	objectTypeList child_types;

	struct _dit_relation *next;
} dit_relation, *ditRelation;

#define MaxDitTypes = 128;

#define NULLDitRelation (ditRelation) NULL
#define dit_relation_alloc() (ditRelation) smalloc(sizeof(dit_relation))

typedef struct _search_info {
	AttributeType object_type;
	char *type_label;

	objectTypeList search_on_list;

	struct _search_info *next;
} search_info, *searchInfo, *searchInfoList;

#define NULLSearchInfo (searchInfo) NULL
#define search_info_alloc() (searchInfo) smalloc(sizeof(search_info))

typedef struct _ufs_path {
	AttributeType object_type;
	entryList path;

	struct _ufs_path *next;
} ufs_path, *ufsPath;

#define NULLUfsPath (ufsPath) NULL
#define ufs_path_alloc() (ufsPath) smalloc(sizeof(ufs_path))

void set_relation(), set_search_attrs(), set_default_path();
objectTypeList get_child_list(), get_search_attrs();
entryList get_default_path();

QBool is_oc_must(), is_oc_may(), is_search_attr();
char *get_type_label();

#endif

#endif				/* __QUIPU_UIPS_DOOG_QUERY_DITMODEL_H__ */
