/*****************************************************************************

 @(#) $Id: timer.h,v 0.9 2004/01/17 08:07:46 brian Exp $

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
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2004/01/17 08:07:46 $ by $Author: brian $

 *****************************************************************************/

#ifndef __TIMER_H__
#define __TIMER_H__

#define SS7_DECLARE_TIMER(__n,__o,__t,__c) \
STATIC int __o ## _ ## __t ## _timeout(struct __o *); \
STATIC void __o ## _ ## __t ## _expiry(caddr_t data) \
{ \
	ss7_do_timeout(data, # __t, __n, &((struct __o *)data)->timers.__t, \
			(int(*)(struct head *))&__o ## _ ## __t ## _timeout, \
			&__o ## _ ## __t ## _expiry); \
} \
STATIC void __o ## _stop_timer_ ## __t (struct __o * __o) \
{ \
	ss7_stop_timer((struct head *)__o, # __t, __n, &__o->timers.__t); \
} \
STATIC void __o ## _start_timer_ ## __t (struct __o * __o) \
{ \
	ss7_start_timer((struct head *)__o, # __t, __n, &__o->timers.__t, &__o ## _ ## __t ## _expiry, __o->__c.__t); \
} \

/* Why????! */
#undef put

STATIC INLINE void
ss7_do_timeout(caddr_t data, const char *timer, const char *mod, ulong *timeo,
	       int (*to_fnc) (struct head *), void (*exp_func) (caddr_t))
{
	struct head *h = (struct head *) data;
	if (xchg(timeo, 0)) {
		if (lis_spin_trylock(&h->lock)) {
			printd(("%s: %p: %s timeout at %lu\n", mod, h, timer, jiffies));
			switch (to_fnc(h)) {
			default:
			case QR_DONE:
				lis_spin_unlock(&h->lock);
				if (h->put)
					h->put(h);
				else
					printd(("%s: %p: object has no put procedure\n", mod, h));
				return;
			case -ENOMEM:
			case -ENOBUFS:
			case -EBUSY:
			case -EAGAIN:
				break;
			}
			lis_spin_unlock(&h->lock);
		} else
			printd(("%s: %p: %s timeout collision at %lu\n", mod, h, timer, jiffies));
		/*
		   back off timer two ticks 
		 */
		*timeo = timeout(exp_func, data, 2);
	}
}
STATIC INLINE void
ss7_stop_timer(struct head *h, const char *timer, const char *mod, ulong *timeo)
{
	ulong to;
	if ((to = xchg(timeo, 0))) {
		untimeout(to);
		printd(("%s: %p: stopping %s at %lu\n", mod, h, timer, jiffies));
		if (h->put)
			h->put(h);
		else
			printd(("%s: %p: object has no put procedure\n", mod, h));
	}
	return;
}
STATIC INLINE void
ss7_start_timer(struct head *h, const char *timer, const char *mod, ulong *timeo,
		void (*exp_func) (caddr_t), ulong val)
{
	printd(("%s: %p: starting %s %lu ms at %lu\n", mod, h, timer, val * 1000 / HZ, jiffies));
	if (h->get)
		h->get(h);
	else
		printd(("%s: %p: object has no get procedure\n", mod, h));
	*timeo = timeout(exp_func, (caddr_t) h, val);
}

#endif				/* __TIMER_H__ */
