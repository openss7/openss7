/*****************************************************************************

 @(#) $Id: priv.h,v 0.9.2.5 2005/07/18 12:25:40 brian Exp $

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

 Last Modified $Date: 2005/07/18 12:25:40 $ by $Author: brian $

 *****************************************************************************/

#ifndef __OS7_PRIV_H__
#define __OS7_PRIV_H__

/*
 *  =========================================================================
 *
 *  Private Data Structures
 *
 *  =========================================================================
 */

/*
   list linkage 
 */
#define HEAD_DECLARATION(__type) \
	__type *next;				/* link linkage */ \
	__type **prev;				/* link linkage */ \
	atomic_t refcnt;			/* reference count */ \
	spinlock_t lock;			/* structure lock */ \
	void (*priv_put)(__type *);		/* release function */ \
	__type *(*priv_get)(__type *);		/* acquire function */ \
	ulong type;				/* structure type */ \
	ulong id;				/* structure id */ \
	ulong state;				/* state */ \
	ulong flags;				/* flags */ \

typedef struct head {
	HEAD_DECLARATION (struct head);	/* head declaration */
} head_t;

/*
   stream related structures 
 */
#define STR_DECLARATION(__type) \
	HEAD_DECLARATION(__type);		/* link linkage */ \
	union { \
		struct { \
			major_t cmajor;		/* device major */ \
			minor_t cminor;		/* device minor */ \
		} dev; \
		struct { \
			ulong index;		/* linked index */ \
		} mux; \
	} u; \
	cred_t cred;				/* credential */ \
	queue_t *iq;				/* iput queue */ \
	queue_t *oq;				/* oput queue */ \
	int (*i_prim)(queue_t *, mblk_t *);	/* iput function */ \
	int (*o_prim)(queue_t *, mblk_t *);	/* oput function */ \
	void (*i_wakeup)(queue_t *);		/* isrv wakeup function */ \
	void (*o_wakeup)(queue_t *);		/* osrv wakeup function */ \
	spinlock_t qlock;			/* queue lock */ \
	uint ibid;				/* iput bufcall id */ \
	uint obid;				/* oput bufcall id */ \
	queue_t *iwait;				/* iput queue waiting */ \
	queue_t *owait;				/* oput queue waiting */ \
	ulong i_state;				/* interface state */ \
	ulong i_style;				/* interface style */ \
	ulong i_version;			/* interface version */ \
	ulong i_oldstate;			/* previous state */ \

/*
   generic stream structure 
 */
typedef struct str {
	STR_DECLARATION (struct str);	/* stream declaration */
} str_t;

#define STR_PRIV(__q)((str_t *)(__q)->q_ptr)

/*
   generic linkage 
 */
#define SLIST_LINKAGE(__s1, __s2, __s3) \
	struct { \
		struct __s1 *__s1; \
		struct __s2 *next; \
		struct __s2 **prev; \
	} __s3 \

#define SLIST_HEAD(__s1, __s2) \
	struct { \
		size_t numb; \
		struct __s1 *list; \
	} __s2 \

#define SLIST_HASH(__s1, __s2, __n) \
	struct { \
		size_t numb; \
		struct __s1 *hash[__n]; \
	} __s2 \

#endif				/* __OS7_PRIV_H__ */
