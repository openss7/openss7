/*****************************************************************************

 @(#) $Id: lm.h,v 0.9.2.4 2007/06/17 01:56:16 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2007/06/17 01:56:16 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: lm.h,v $
 Revision 0.9.2.4  2007/06/17 01:56:16  brian
 - updates for release, remove any later language

 *****************************************************************************/

#ifndef __LMI_LM_H__
#define __LMI_LM_H__

#ident "@(#) $RCSfile: lm.h,v $ $Name:  $($Revision: 0.9.2.4 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

struct lmi;

typedef struct lmi_devops {
	struct lmi *(*attach) (dev_t);
	int (*open) (struct lmi *);
	int (*close) (struct lmi *);
} lmi_devops_t;

typedef struct lmi_lmiops {
	int (*info) (struct lmi *, void **, int *);
	int (*attach) (struct lmi *, void *, int);
	int (*detach) (struct lmi *);
	int (*enable) (struct lmi *);
	int (*disable) (struct lmi *);
	int (*ioctl) (struct lmi *, int, void *);
} lmi_lmiops_t;

typedef struct lmi_ops {
	lmi_devops_t dev;		/* device management */
	lmi_lmiops_t lmi;		/* local management calls */
} lmi_ops_t;

typedef struct lmi_driver {
	struct lmi_driver *next;	/* next driver structure */
	int cmajor;			/* major device number */
	int nminor;			/* number of minor devices */
	struct lmi *list;		/* list of device structures */
	struct streamtab *info;		/* streamtab for this driver */
	struct lmi_ops ops;		/* device operations */
	void *dcalls;			/* module calling driver */
	void *ucalls;			/* driver calling module */
} lmi_driver_t;

typedef struct lmi {
	struct lmi *next;		/* next device in device list */
	struct lmi *module;		/* upper for dev/mux, other for mod */
	void *ucalls;			/* upper module calls */
	void *dcalls;			/* lower device calls */
	struct lmi *device;		/* driver device (lower for mux) */
	lmi_driver_t *driver;		/* driver struct for driver */
	dev_t devnum;			/* driver device (null for module) */
	struct queue *rq;		/* read queue */
	struct queue *wq;		/* write queue */
	spinlock_t lock;		/* spin lock for SMP */
	void *user;			/* user of this structure */
	uint nest;			/* nested locks */
	lmi_ulong state;		/* LM interface state */
	lmi_ulong flags;		/* LM interface flags */
	lmi_ulong ids[0];		/* Protocol structure follows */
} lmi_t;

#define LMI_PRIV(__q) ((lmi_t *)(__q)->q_ptr)
#define lmi_init_lock(__lmi) spin_lock_init(&(__lmi)->lock)
#define lmi_locked(__lmi) ((__lmi)->user)

static inline int lmi_trylock(lmi_t * lmi)
{
	spinlock_t *lock = &lmi->lock;
	spin_lock(lock);
	if (lmi->user && lmi->user != current) {
		spin_unlock(lock);
		return (1);
	}
	lmi->user = current;
	lmi->next++;
	spin_unlock(lock);
	return (0);
}

static inline void lmi_unlock(lmi_t * lmi)
{
	spinlock_t *lock = &lmi->lock;
	spin_lock(lock);
	if (lmi->nest && lmi->user == current) {
		if (!--lmi->nest)
			lmi->user = NULL;
	} else {
		ptrace(("LMI: ERROR: Double unlock, nest = %d\n", lmi->nest));
		lmi->user = NULL;
		lmi->nest = 0;
	}
	spin_unlock(lock);
}

extern int lmi_info(struct lmi *, void **, int *);
extern int lmi_attach(struct lmi *, void *, int);
extern int lmi_detach(struct lmi *);
extern int lmi_enable(struct lmi *);
extern int lmi_disable(struct lmi *);

extern int (*lmi_lmi_ops[5]) (lmi_t *, mblk_t *);

extern lmi_t *lmi_drv_attach(dev_t, lmi_driver_t *, size_t);
extern int lmi_drv_open(lmi_t * lmi);
extern int lmi_drv_close(lmi_t * lmi, lmi_ulong * tids, int ntids, lmi_ulong * bids, int nbids);

extern int lmi_mux_open(queue_t * q, dev_t * devp, int flag, int sflag,
			cred_t * crp, lmi_driver_t * list, size_t size);
extern int lmi_mux_close(queue_t * q, int flag, cred_t * crp,
			 lmi_ulong * tids, int ntids, lmi_ulong * bids, int nbids);

extern int lmi_open(queue_t * q, dev_t * devp, int flag, int sflag,
		    cred_t * crp, lmi_driver_t * list, size_t size);
extern int lmi_close(queue_t * q, int flag, cred_t * crp,
		     lmi_ulong * tids, int ntids, lmi_ulong * bids, int nbids);

extern int lmi_register_driver(lmi_driver_t ** list, major_t cmajor,
			       struct streamtab *strtab, int nminor, char *name,
			       lmi_ops_t * ops, void *dcalls, void *ucalls);
extern int lmi_unregister_driver(lmi_driver_t **, major_t);

#endif				/* __LMI_LM_H__ */
