/*****************************************************************************

 @(#) $RCSfile: safe.c,v $ $Name:  $($Revision: 1.1.1.5.4.3 $) $Date: 2005/12/18 05:41:23 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 2 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 Ave, Cambridge, MA 02139, USA.

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

 Last Modified $Date: 2005/12/18 05:41:23 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: safe.c,v $ $Name:  $($Revision: 1.1.1.5.4.3 $) $Date: 2005/12/18 05:41:23 $"

/*                               -*- Mode: C -*- 
 * safe.c --- stream safe
 * Author          : Graham Wheeler
 * Created On      : Tue May 31 22:25:19 1994
 * Last Modified By: David Grothe
 * RCS Id          : $Id: safe.c,v 1.1.1.5.4.3 2005/12/18 05:41:23 brian Exp $
 * Purpose         : stream safe processing stuff
 * ----------------______________________________________________
 *
 *  Copyright (C) 1995  Graham Wheeler
 *  Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>
 *
 *    You can reach me by email to
 *    gram@aztec.co.za
 *    dave@gcom.com
 */

/*  -------------------------------------------------------------------  */
/*				 Dependencies                            */

#include <sys/stream.h>
#include <sys/osif.h>

/*  -------------------------------------------------------------------  */
/*				  Glob. Vars                             */

/*  -------------------------------------------------------------------  */
/*			   Local functions & macros                      */

#define	LOG(fil, line, msg)	printk("%s: called from file %s #%d\n",  \
					msg, fil, line)

/*  -------------------------------------------------------------------  */
/*			Exported functions & macros                      */

void _RP
lis_safe_noenable(queue_t *q, char *f, int l)
{
	lis_flags_t psw;

	if (!LIS_QMAGIC(q, f, l))
		return;
	LIS_QISRLOCK(q, &psw);
	q->q_flag |= QNOENB;
	LIS_QISRUNLOCK(q, &psw);
}

void _RP
lis_safe_enableok(queue_t *q, char *f, int l)
{
	lis_flags_t psw;

	if (!LIS_QMAGIC(q, f, l))
		return;
	LIS_QISRLOCK(q, &psw);
	q->q_flag &= ~QNOENB;
	LIS_QISRUNLOCK(q, &psw);
}

int _RP
lis_safe_canenable(queue_t *q, char *f, int l)
{
	if (LIS_QMAGIC(q, f, l))
		return !(q->q_flag & QNOENB);

	return 0;
}

queue_t *_RP
lis_safe_OTHERQ(queue_t *q, char *f, int l)
{
	queue_t *oq = NULL;

	if (LIS_QMAGIC(q, f, l)) {
		oq = q->q_other;
		if (LIS_QMAGIC(oq, f, l))
			return (oq);
	}

	return NULL;
}

queue_t *_RP
lis_safe_RD(queue_t *q, char *f, int l)
{
	queue_t *oq = NULL;

	if (LIS_QMAGIC(q, f, l)) {
		if ((q->q_flag & QREADR))
			oq = q;
		else
			oq = q->q_other;

		if (LIS_QMAGIC(oq, f, l))
			return (oq);

	}
	return NULL;
}

queue_t *_RP
lis_safe_WR(queue_t *q, char *f, int l)
{
	queue_t *oq = NULL;

	if (LIS_QMAGIC(q, f, l)) {
		if ((q->q_flag & QREADR))
			oq = q->q_other;
		else
			oq = q;

		if (LIS_QMAGIC(oq, f, l))
			return (oq);
	}

	return NULL;
}

int _RP
lis_safe_SAMESTR(queue_t *q, char *f, int l)
{
	if (LIS_QMAGIC(q, f, l)
	    && q->q_next != NULL && LIS_QMAGIC((q->q_next), f, l)
	    )
		return ((q->q_flag & QREADR) == (q->q_next->q_flag & QREADR));

	return 0;
}

int
lis_safe_do_putmsg(queue_t *q, mblk_t *mp, ulong qflg, int retry, char *f, int l)
{
	lis_flags_t psw;

	qflg |= QOPENING;

      try_again:
	if (mp == NULL || !LIS_QMAGIC(q, f, l)
	    || q->q_qinfo == NULL || q->q_qinfo->qi_putp == NULL) {
		LOG(f, l, "NULL q, mp, q_qinfo or qi_putp in putmsg");
		freemsg(mp);
		return (1);	/* message consumed */
	}

	LIS_QISRLOCK(q, &psw);
	if (q->q_flag & (QPROCSOFF | QFROZEN)) {
		if ((q->q_flag & QPROCSOFF) && SAMESTR(q)) {	/* there is a next queue *//* pass to next queue */
			LIS_QISRUNLOCK(q, &psw);
			q = q->q_next;
			goto try_again;
		}

		lis_defer_msg(q, mp, retry, &psw);	/* put in deferred msg list */
		LIS_QISRUNLOCK(q, &psw);
		return (0);	/* msg deferred */
	}

	if ((q->q_flag & qflg) || q->q_defer_head != NULL) {
		lis_defer_msg(q, mp, retry, &psw);
		LIS_QISRUNLOCK(q, &psw);
		return (0);	/* msg deferred */
	}
	LIS_QISRUNLOCK(q, &psw);

	/* 
	 * Now we are going to call the put procedure
	 */
	if (lis_lockqf(q, f, l) < 0) {
		freemsg(mp);	/* busted */
		return (1);
	}

	(*(q->q_qinfo->qi_putp)) (q, mp);

	lis_unlockqf(q, f, l);
	return (1);		/* msg consumed */
}

void _RP
lis_safe_putmsg(queue_t *q, mblk_t *mp, char *f, int l)
{
	lis_safe_do_putmsg(q, mp, (QDEFERRING | QOPENING), 0, f, l);
}

void _RP
lis_safe_putnext(queue_t *q, mblk_t *mp, char *f, int l)
{
	queue_t *qnxt = NULL;

	if (mp == NULL || !LIS_QMAGIC(q, f, l)
	    || !LIS_QMAGIC((qnxt = q->q_next), f, l)
	    ) {
		LOG(f, l, "NULL q, mp or q_next in putnext");
		freemsg(mp);
		return;
	}

	if (LIS_DEBUG_TRCE_MSG)
		lis_mark_mem(mp, lis_queue_name(qnxt), MEM_MSG);

	if (LIS_DEBUG_PUTNEXT) {
		printk("putnext: %s from \"%s\" to \"%s\" size %d\n", lis_msg_type_name(mp),
		       lis_queue_name(q), lis_queue_name(qnxt), lis_msgsize(mp)
		    );

		if (LIS_DEBUG_ADDRS)
			printk("        q=%lx, mp=%lx, mp->b_rptr=%lx, wptr=%lx\n", (long) q,
			       (long) mp, (long) mp->b_rptr, (long) mp->b_wptr);
	}

	lis_safe_putmsg(qnxt, mp, f, l);
}

void _RP
lis_safe_qreply(queue_t *q, mblk_t *mp, char *f, int l)
{
	if (mp == NULL) {
		LOG(f, l, "NULL msg in qreply");
		return;
	}

	if ((q = LIS_OTHERQ(q)) == NULL) {
		LOG(f, l, "NULL OTHERQ(q) in qreply");
		freemsg(mp);
		return;
	}

	lis_safe_putnext(q, mp, f, l);
}

/*----------------------------------------------------------------------
# Local Variables:      ***
# change-log-default-name: "~/src/prj/streams/src/NOTES" ***
# End: ***
  ----------------------------------------------------------------------*/
