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
    "Header: /xtel/isode/isode/others/quipu/uips/doog/query/RCS/ds_tasks.c,v 9.0 1992/06/16 12:45:27 isode Rel";
#endif

/* Header: /xtel/isode/isode/others/quipu/uips/doog/query/RCS/ds_tasks.c,v 9.0 1992/06/16 12:45:27 isode Rel */

/*
 * Log: ds_tasks.c,v
 * Revision 9.0  1992/06/16  12:45:27  isode
 * Release 8.0
 *
 */

/*****************************************************************************

  ds_tasks.c - Manage X500 requests.

*****************************************************************************/

#include "types.h"
#include "util.h"
#include "ds_tasks.h"
#include "error.h"

#include "quipu/common.h"
#include "quipu/util.h"
#include "quipu/dap2.h"
#include "quipu/abandon.h"

/* DsTask tasks_outstanding[MAXTASKS]; */
DsTask live_task_list = NULLDsTask;
QCardinal live_task_count;
int next_task_id = 0;

/* Private functions. */
static void ds_task_free();

#ifndef NO_STATS
extern LLog *log_stat;
#endif

/* 
 * - _task_invoked() -
 * Add invoked task to task list and pass back task identifier.
 *
 */
QE_error_code
_task_invoked(type, baseobject, request_id, task_id_ptr)
	task_type type;
	char *baseobject;
	QCardinal request_id;
	int *task_id_ptr;
{
	DsTask task_rec;

	if (live_task_count >= MAXTASKS)
		return QERR_internal_limit_reached;

	task_rec = ds_task_alloc();
	task_rec->next = live_task_list;

	task_rec->type = type;
	task_rec->task_id = *task_id_ptr = next_task_id++;

	task_rec->baseobject = copy_string(baseobject);
	task_rec->request_id = request_id;

	live_task_count++;
	live_task_list = task_rec;

	return QERR_ok;
}				/* _task_invoked */

/*
 * - _task_complete() -
 *  A task has been completed. Remove it from the outstanding list.
 *
 */
void
_task_complete(task_id)
	int task_id;
{
	register DsTask prev_rec = NULLDsTask, task_rec = live_task_list;

	while (task_rec != NULLDsTask && task_rec->task_id != task_id) {
		prev_rec = task_rec;
		task_rec = task_rec->next;
	}

	if (task_rec != NULLDsTask) {
		if (prev_rec != NULLDsTask)
			prev_rec->next = task_rec->next;
		else
			live_task_list = task_rec->next;

		ds_task_free(task_rec);
		live_task_count--;
	}
}				/* _task_complete */

/*
 * - _get_task_of_id() - 
 * Find task_rec of given id and return a pointer to it.
 *
 */
DsTask
_get_task_of_id(task_id)
	int task_id;
{
	register DsTask task_rec = live_task_list;

	while (task_rec != NULLDsTask && task_rec->task_id != task_id)
		task_rec = task_rec->next;

	return task_rec;
}				/* _get_task_of_id */

/*
 * - abort_task() -
 * Abort an X.500 request
 *
 */
void
abort_task(task_id)
	int task_id;
{
	struct ds_abandon_arg arg;
	struct DAPindication di;

	extern int dsap_ad;

	_task_complete(task_id);

	arg.aba_invokeid = task_id;

	(void) DapAbandon(dsap_ad, next_task_id++, &arg, &di, ROS_ASYNC);

#ifndef NO_STATS
	LLOG(log_stat, LLOG_NOTICE, ("ABANDONING task %d", task_id));
#endif
}				/* abort_task */

/*
 * - ds_task_free() -
 * Free a ds_task structure.
 *
 */
static void
ds_task_free(task)
	DsTask task;
{
	if (task == NULLDsTask)
		return;

	if (task->baseobject != NULLCP)
		(void) free(task->baseobject);

	(void) free((char *) task);
}				/* ds_task_free */
