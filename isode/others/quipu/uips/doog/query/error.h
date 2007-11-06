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

#ifndef __QUIPU_UIPS_DOOG_QUERY_ERROR_H__
#define __QUIPU_UIPS_DOOG_QUERY_ERROR_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* Header: /xtel/isode/isode/others/quipu/uips/doog/query/RCS/error.h,v 9.0 1992/06/16 12:45:27 isode Rel */

#ifndef _query_error_h_
#define _query_error_h_

typedef enum {
	QERR_ok,
	QERR_succeeded,
	QERR_request_failed,
	QERR_referral_error,
	QERR_bad_name,
	QERR_internal_limit_reached,
	QERR_bad_value_syntax,
	QERR_time_limit_reached,
	QERR_size_limit_reached,
	QERR_admin_limit_reached,
	QERR_local_error,
	QERR_remote_error,
	QERR_no_such_attr,
	QERR_bad_attr_syntax,
	QERR_no_such_object,
	QERR_alias_problem,
	QERR_security_error,
	QERR_service_busy,
	QERR_service_unavailable,
	QERR_chaining_required,
	QERR_unable_to_proceed,
	QERR_loop_detect,
	QERR_ext_unavailable,
	QERR_out_of_scope,
	QERR_dit_error,
	QERR_naming_violation,
	QERR_oc_violation,
	QERR_not_on_nonleaf,
	QERR_not_on_rdn,
	QERR_already_exists,
	QERR_affects_mult_dsas,
	QERR_no_oc_mods,
	QERR_no_such_op,
	QERR_too_late,
	QERR_cannot_abandon,
	QERR_nothing_found,
	QERR_internal,
	QERR_no_mods_supplied,
	QERR_null
} QE_error_code;

typedef struct _QE_error {
	QE_error_code error;
	char *error_message;
} QE_error;

typedef struct _error_list {
	char *baseobject;
	QE_error_code error_type;
	char *ds_message;
	struct _error_list *next;
} error_list, *errorList;

#define NULLError (errorList) NULL
#define error_alloc() (errorList) smalloc(sizeof(error_list))

void error_list_free();
errorList error_list_copy();

void add_error_to_request_rec();
char *ds_error_message();
QE_error_code get_log_error_type();

#endif				/* _query_error_h_ */

#endif				/* __QUIPU_UIPS_DOOG_QUERY_ERROR_H__ */
