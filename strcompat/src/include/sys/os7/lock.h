/*****************************************************************************

 @(#) $Id: lock.h,v 0.9.2.3 2005/05/14 08:26:12 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (C) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

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

 Last Modified $Date: 2005/05/14 08:26:12 $ by $Author: brian $

 *****************************************************************************/

#ifndef __OS7_LOCK_H__
#define __OS7_LOCK_H__

STATIC int ss7_osrv(queue_t *);
STATIC int ss7_isrv(queue_t *);

STATIC INLINE int
ss7_trylockq(queue_t *q)
{
	int res;
	str_t *s = STR_PRIV(q);
	if (!(res = spin_trylock(&s->qlock))) {
		if (q == s->iq)
			s->iwait = q;
		if (q == s->oq)
			s->owait = q;
	}
	return (res);
}
STATIC INLINE void
ss7_unlockq(queue_t *q)
{
	str_t *s = STR_PRIV(q);
	spin_unlock(&s->qlock);
	if (s->iwait) {
		if (s->iwait->q_qinfo && s->iwait->q_qinfo->qi_srvp)
			qenable(xchg(&s->iwait, NULL));
		else
			ss7_isrv(xchg(&s->iwait, NULL));
	}
	if (s->owait) {
		if (s->owait->q_qinfo && s->owait->q_qinfo->qi_srvp)
			qenable(xchg(&s->owait, NULL));
		else
			ss7_osrv(xchg(&s->iwait, NULL));
	}
}

#endif				/* __OS7_LOCK_H__ */
