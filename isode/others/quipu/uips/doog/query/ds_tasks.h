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

#ifndef __QUIPU_UIPS_DOOG_QUERY_DS_TASKS_H__
#define __QUIPU_UIPS_DOOG_QUERY_DS_TASKS_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* Header: /xtel/isode/isode/others/quipu/uips/doog/query/RCS/ds_tasks.h,v 9.0 1992/06/16 12:45:27 isode Rel */

#ifndef _query_ds_task_h_
#define _query_ds_task_h_

#include "types.h"
#include "error.h"

#define MAXTASKS 512
#define NO_TASK -1

typedef enum {
	SEARCH_TASK,
	LIST_TASK,
	READ_TASK,
	MODIFY_TASK,
	MODIFY_RDN_TASK,
	ADD_TASK,
	REMOVE_TASK,
	COMPARE_TASK,
	ABANDON_TASK
} task_type;

typedef struct _ds_task {
	task_type type;
	int task_id;
	char *baseobject;
	QCardinal request_id;
	struct _ds_task *next;
} ds_task, *DsTask;

#define NULLDsTask (DsTask) NULL
#define ds_task_alloc() (DsTask) smalloc(sizeof(ds_task))

QE_error_code _task_invoked();
void _task_complete();
DsTask _get_task_of_id();
void abort_task();

#endif				/* _query_ds_task_h_ */

#endif				/* __QUIPU_UIPS_DOOG_QUERY_DS_TASKS_H__ */
