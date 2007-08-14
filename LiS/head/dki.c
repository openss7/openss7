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

#ident "@(#) $RCSfile: dki.c,v $ $Name:  $($Revision: 1.1.1.6.4.4 $) $Date: 2005/12/19 03:22:18 $"

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

/************************************************************************
*                      SVR4 Driver-Kernel Interface                     *
*************************************************************************
*									*
* These routines implement an SVR4 compatible DKI.			*
*									*
* Author:	David Grothe <dave@gcom.com>				*
*									*
* Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>		*
*									*
************************************************************************/

#include <sys/stream.h>
#include <sys/osif.h>

lis_spin_lock_t lis_tlist_lock;

/************************************************************************
*                       SVR4 Compatible timeout                         *
*************************************************************************
*									*
* This implementation of SVR4 compatible timeout functions uses the	*
* Linux timeout mechanism as the internals.				*
*									*
* The technique is to allocate memory that contains a Linux timer	*
* structure and an SVR4-like handle.  We build up a list of these	*
* structures in a linked-list headed by 'lis_tlist_head'.  We never	*
* use handle zero, so that value becomes a marker for an available	*
* entry.  We recycle the list elements as they expire or get untimeout-	*
* ed.									*
*									*
************************************************************************/

typedef struct tlist {
	struct tlist *next;		/* thread of these */
	struct timer_list tl;		/* Linux timer structure */
	int handle;			/* SVR4-style handle */
	timo_fcn_t *fcn;		/* function to call */
	caddr_t arg;			/* function argument */

} tlist_t;

#define THASH		757	/* prime number */

/*
 * head[0] is for free timer structures.  The others head linked
 * lists of timer structures whose handles hash to the same value.
 */
volatile tlist_t *lis_tlist_heads[THASH];

/*
 * The tlist handles roll over at 2^31 and thus are virtually never
 * repeated.  But we check whenever assigning a timer handle to ensure
 * that it is not already in use.  The hash table makes this easy.
 */
volatile int lis_tlist_handle;		/* next handle to use */

/*
 * Timer list manipulations
 *
 * Caller has timer list locked
 */
static INLINE void
enter_timer_in_list(tlist_t * tp)
{
	tlist_t **headp = (tlist_t **) & lis_tlist_heads[tp->handle % THASH];

	tp->next = *headp;
	*headp = tp;
}

static INLINE int
remove_timer_from_list(tlist_t * tp)
{
	tlist_t *t;
	tlist_t *nxt;
	tlist_t **headp = (tlist_t **) & lis_tlist_heads[tp->handle % THASH];

	if ((t = *headp) == NULL) {	/* nothing in that hash list */
		tp->next = NULL;
		return 0;
	}

	if (t == tp) {		/* our entry is first in list */
		*headp = tp->next;
		tp->next = NULL;
		return 1;
	}

	/* Find this entry starting at the 2nd element and on down the list */
	for (nxt = NULL; t->next != NULL; t = nxt) {
		nxt = t->next;
		if (nxt == tp) {
			t->next = tp->next;
			tp->next = NULL;
			return 1;
		}
	}

	tp->next = NULL;	/* ensure null next pointer */
	return 0;
}

static INLINE tlist_t *
alloc_timer(char *file_name, int line_nr)
{
	tlist_t *t;
	tlist_t **headp = (tlist_t **) & lis_tlist_heads[0];

	if ((t = *headp) == NULL)	/* nothing in the hash list */
		t = (tlist_t *) lis_alloc_timer(file_name, line_nr);
	else {
		*headp = t->next;	/* link around 1st element */
		lis_mark_mem(t, file_name, line_nr);
	}

	return (t);		/* NULL if could not allocate */
}

/*
 * This is always the function passed to the kernel.  'arg' is a pointer to
 * the tlist_t for the timeout.
 */
static void
sys_timeout_fcn(ulong arg)
{
	tlist_t *tp = (tlist_t *) arg;
	timo_fcn_t *fcn = (timo_fcn_t *) NULL;
	caddr_t uarg = NULL;
	lis_flags_t psw;

	lis_spin_lock_irqsave(&lis_tlist_lock, &psw);

	if (!remove_timer_from_list(tp)) {
		lis_spin_unlock_irqrestore(&lis_tlist_lock, &psw);
		return;
	}
	if (tp->handle != 0 && tp->fcn != NULL) {
		fcn = tp->fcn;	/* save local copy */
		uarg = tp->arg;
	}

	tp->handle = 0;		/* entry now available */
	tp->fcn = NULL;
	enter_timer_in_list(tp);	/* enter in list 0 (free list) */
	lis_spin_unlock_irqrestore(&lis_tlist_lock, &psw);

	if (fcn != (timo_fcn_t *) NULL)
		fcn(uarg);	/* call fcn while not holding lock */
}

/*
 * Find the timer entry associated with the given handle.
 *
 * Caller has timer list locked
 */
static INLINE tlist_t *
find_timer_by_handle(int handle)
{
	tlist_t *tp = NULL;
	tlist_t **headp = (tlist_t **) & lis_tlist_heads[handle % THASH];

	for (tp = *headp; tp != NULL; tp = tp->next) {
		if (tp->handle == handle)
			return (tp);
	}

	return (NULL);
}

/*
 * Allocate an unused timer handle.  If there is a timer structure in
 * the list that is available return a pointer to it, else null.
 *
 * In any event return a unique handle.
 *
 * Caller has locked the timer list.
 */
static INLINE int
alloc_handle(void)
{
	tlist_t *tp = NULL;
	int handle;

	while (1) {
		do {		/* next handle */
			handle = (++lis_tlist_handle) & 0x7FFFFFFF;
		}
		while ((handle % THASH) == 0);	/* prevent use of handle 0 */

		tp = find_timer_by_handle(handle);
		if (tp == NULL)	/* handle available */
			break;
	}

	return (handle);
}

/*
 * "timeout" is #defined to be this function in dki.h.
 */
toid_t _RP
lis_timeout_fcn(timo_fcn_t *timo_fcn, caddr_t arg, long ticks, char *file_name, int line_nr)
{
	tlist_t *tp;
	int handle;
	lis_flags_t psw;
	static tlist_t z;

	lis_spin_lock_irqsave(&lis_tlist_lock, &psw);

	tp = alloc_timer(file_name, line_nr);	/* available timer struct */
	if (tp == NULL) {	/* must allocate a new one */
		lis_spin_unlock_irqrestore(&lis_tlist_lock, &psw);
		return (0);	/* no memory for timer */
	}

	handle = alloc_handle();
	*tp = z;		/* zero out structure */
	tp->handle = handle;
	tp->fcn = timo_fcn;
	tp->arg = arg;

	enter_timer_in_list(tp);	/* hashed by handle */
	lis_spin_unlock_irqrestore(&lis_tlist_lock, &psw);
	/* start system timer */
	lis_tmout(&tp->tl, sys_timeout_fcn, (long) tp, ticks);

	return (handle);	/* return handle */

}				/* lis_timeout_fcn */

toid_t _RP
lis_untimeout(toid_t id)
{
	tlist_t *tp;
	lis_flags_t psw;

	lis_spin_lock_irqsave(&lis_tlist_lock, &psw);

	tp = find_timer_by_handle(id);
	if (tp != NULL) {
		lis_mark_mem(tp, "unused-timer", MEM_TIMER);
		remove_timer_from_list(tp);
		lis_untmout(&tp->tl);	/* stop system timer */
		tp->handle = 0;	/* make available */
		enter_timer_in_list(tp);	/* list 0 is free list */
	}

	lis_spin_unlock_irqrestore(&lis_tlist_lock, &psw);
	return (0);		/* no rtn value specified */

}				/* lis_untimeout */

void
lis_terminate_dki(void)
{
	lis_flags_t psw;
	int i;

	/* 
	 *  In case of buggy drivers, timeouts could still happen.
	 *  Better be on the safe side.
	 */
	lis_spin_lock_irqsave(&lis_tlist_lock, &psw);

	for (i = 0; i < THASH; i++) {
		while (lis_tlist_heads[i] != NULL) {
			tlist_t *tp = (tlist_t *) lis_tlist_heads[i];

			lis_untmout(&tp->tl);
			remove_timer_from_list(tp);
			lis_free_timer(tp);
		}
	}

	lis_spin_unlock_irqrestore(&lis_tlist_lock, &psw);
	lis_terminate_timers();	/* an mdep routine */
}

void
lis_initialize_dki(void)
{
	lis_init_timers(sizeof(tlist_t));	/* an mdep routine */
}
