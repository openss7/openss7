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

#ifndef __QUIPU_UIPS_DOOG_QUERY_MODIFY_H__
#define __QUIPU_UIPS_DOOG_QUERY_MODIFY_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

#ifndef _query_modify_h_
#define _query_modify_h_

#include "types.h"
#include "error.h"
#include "sequence.h"
#include "read.h"
#include "attrt_list.h"

/*
 * 	Structures and data types for `modify entry' request.
 */

typedef enum {
	new,
	unchanged,
	deleted,
	updated
} modValState;

typedef struct _modify_value {
	char *value;
	char *new_value;

	modValState status;

	struct _modify_value *next;
} modify_value, *modifyValue;

#define NULLModifyValue (modifyValue) NULL
#define modify_value_alloc() (modifyValue) smalloc(sizeof(modify_value))

typedef struct _modify_attr {
	char *attr_name;
	AttributeType attr_type;

	QBool to_modify;

	modifyValue values;

	struct _modify_attr *next;
} modify_attr, *modifyAttr;

#define NULLModifyAttr (modifyAttr) NULL
#define modify_attr_alloc() (modifyAttr) smalloc(sizeof(modify_attr))

 /* Modify result structure */

typedef struct _modify_result {
	QBool was_successful;
	errorList errors;
} modify_result, *modifyResult;

#define NULLModifyResult (modifyResult) NULL
#define modify_result_alloc() (modifyResult) smalloc(sizeof(modify_result))

 /* Modify request record */

typedef struct _modify_rec {
	QCardinal request_id;
	char *base_object;
	int task_id;
	modifyResult result;
} modify_rec, *modifyRec;

#define NULLModifyRec (modifyRec) NULL
#define modify_rec_alloc() (modifyRec) smalloc(sizeof(modify_rec))

/*
 *	Structures and data types for `make entry template' requests.
 */

/*
 *	Make template request result structure
 */

typedef struct _make_template_result {
	char *base_object;

	modifyAttr must_request;
	modifyAttr may_request;

	modifyAttr must_exc;
	modifyAttr may_exc;

	modifyAttr object_class;

	errorList errors;
} make_template_result, *makeTemplateResult;

#define NULLTemplateResult (makeTemplateResult) NULL
#define template_result_alloc() (makeTemplateResult) \
                                smalloc(sizeof(make_template_result))

/*
 *	Make template request record
 */

typedef struct _make_template_rec {
	char *base_object;
	modifyAttr include_attrs;

	QCardinal request_id;
	int task_id;

	makeTemplateResult result;

	errorList errors;
} make_template_rec, *makeTemplateRec;

#define NULLTemplateRec (makeTemplateRec) NULL
#define template_rec_alloc() (makeTemplateRec) \
  			      smalloc(sizeof(make_template_rec))

/*
 *	Public procedures
 */

QE_error_code do_make_template();
makeTemplateResult get_make_template_result();

errorList do_modify();
modifyResult get_modify_result();

QE_error_code do_change_name();
void get_change_name_result();

QE_error_code do_add_entry();
void get_add_entry_result();

QE_error_code do_delete_entry();
void get_delete_entry_result();

void free_mod_attr_list(), free_mod_val_list(), free_make_template_result(), free_modify_result();

void free_make_template_rec(), modify_rec_free();

request_state process_modify_ds_result(), process_modify_ds_error();
request_state process_template_ds_result(), process_template_ds_error();

#endif

#endif				/* __QUIPU_UIPS_DOOG_QUERY_MODIFY_H__ */
