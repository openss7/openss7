/*****************************************************************************

 @(#) $RCSfile: osfcompat.c,v $ $Name:  $($Revision: 0.9.2.18 $) $Date: 2006/06/22 13:11:33 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2006/06/22 13:11:33 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: osfcompat.c,v $ $Name:  $($Revision: 0.9.2.18 $) $Date: 2006/06/22 13:11:33 $"

static char const ident[] =
    "$RCSfile: osfcompat.c,v $ $Name:  $($Revision: 0.9.2.18 $) $Date: 2006/06/22 13:11:33 $";

/* 
 *  This is my solution for those who don't want to inline GPL'ed functions or
 *  who don't use optimizations when compiling or specifies
 *  -fnoinline-functions or something of the like.  This file implements all
 *  of the extern inlines from the header files by just including the header
 *  files with the functions declared 'inline' instead of 'extern inline'.
 *
 *  There are implemented here in a separate object, out of the way of the
 *  modules that don't use them.
 */

#define __OSF_EXTERN_INLINE INLINE streamscall

#define _OSF_SOURCE

#include "sys/os7/compat.h"

#define OSFCOMP_DESCRIP		"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define OSFCOMP_COPYRIGHT	"Copyright (c) 1997-2005 OpenSS7 Corporation.  All Rights Reserved."
#define OSFCOMP_REVISION	"LfS $RCSfile: osfcompat.c,v $ $Name:  $($Revision: 0.9.2.18 $) $Date: 2006/06/22 13:11:33 $"
#define OSFCOMP_DEVICE		"OSF/1.2 Compatibility"
#define OSFCOMP_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define OSFCOMP_LICENSE		"GPL"
#define OSFCOMP_BANNER		OSFCOMP_DESCRIP		"\n" \
				OSFCOMP_COPYRIGHT	"\n" \
				OSFCOMP_REVISION	"\n" \
				OSFCOMP_DEVICE		"\n" \
				OSFCOMP_CONTACT		"\n"
#define OSFCOMP_SPLASH		OSFCOMP_DEVICE		" - " \
				OSFCOMP_REVISION	"\n"

#ifdef CONFIG_STREAMS_COMPAT_OSF_MODULE
MODULE_AUTHOR(OSFCOMP_CONTACT);
MODULE_DESCRIPTION(OSFCOMP_DESCRIP);
MODULE_SUPPORTED_DEVICE(OSFCOMP_DEVICE);
MODULE_LICENSE(OSFCOMP_LICENSE);
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-osfcompat");
#endif
#endif

__OSF_EXTERN_INLINE void puthere(queue_t *q, mblk_t *mp);

EXPORT_SYMBOL_NOVERS(puthere);
__OSF_EXTERN_INLINE time_t lbolt(void);

EXPORT_SYMBOL_NOVERS(lbolt);
__OSF_EXTERN_INLINE time_t time(void);

EXPORT_SYMBOL_NOVERS(time);

struct str_comm {
	struct str_comm **prev;		/* must be first */
	struct str_comm *next;
	dev_t dev;			/* device number (or NODEV for modules) */
	size_t size;			/* size of private data */
	char priv[0];			/* followed by private data */
};

static spinlock_t str_list_lock = SPIN_LOCK_UNLOCKED;
static struct str_comm *str_list = NULL;

/* 
 *  STREAMS_OPEN_COMM
 *  -------------------------------------------------------------------------
 *  for V4 open
 */
int
streams_open_comm(unsigned int size, queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	struct str_comm *sp, **spp = &str_list;
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);

	if (q->q_ptr != NULL)
		return (0);	/* already open */
	if (sflag == MODOPEN) {
		if (!WR(q)->q_next)
			return (EIO);
	} else {
		if (WR(q)->q_next && SAMESTR(q))
			return (EIO);
	}
	if ((sp = kmem_alloc(sizeof(*sp) + size, KM_NOSLEEP)))	/* we could probably sleep */
		return (ENOMEM);
	bzero(sp, sizeof(*sp) + size);
	spin_lock(&str_list_lock);
	switch (sflag) {
	case CLONEOPEN:
		/* first clone minor (above 5 per AIX docs), but the caller can start wherever they 
		   want above that */
		if (cminor <= 5)
			cminor = 5 + 1;
		/* fall through */
	default:
	case DRVOPEN:
	{
		major_t dmajor = cmajor;

		for (; *(spp) && (dmajor = getmajor((*spp)->dev)) < cmajor; spp = &(*spp)->next) ;
		for (; *(spp) && dmajor == getmajor((*spp)->dev) &&
		     getminor(makedevice(0, cminor)) != 0; spp = &(*spp)->next, cminor++) {
			minor_t dminor = getminor((*spp)->dev);

			if (cminor < dminor)
				break;
			if (cminor == dminor)
				if (sflag != CLONEOPEN) {
					spin_unlock(&str_list_lock);
					kmem_free(sp, sizeof(*sp) + size);
					return (ENXIO);
				}
		}
		if (getminor(makedevice(0, cminor)) == 0) {	/* no minors left */
			spin_unlock(&str_list_lock);
			kmem_free(sp, sizeof(*sp) + size);
			return (EBUSY);
		}
		sp->dev = makedevice(cmajor, cminor);
		break;
	}
	case MODOPEN:
	{
		/* just push modules on list with no device */
#ifdef NODEV
		sp->dev = NODEV;
#else
		sp->dev = 0;
#endif
		break;
	}
	}
	sp->size = size;
	if ((sp->next = *spp))
		sp->next->prev = &sp;
	sp->prev = spp;
	*spp = sp;
	q->q_ptr = OTHERQ(q)->q_ptr = (sp + 1);
	spin_unlock(&str_list_lock);
	return (0);
}

EXPORT_SYMBOL_NOVERS(streams_open_comm);

/* 
 *  STREAMS_OPEN_OCOMM
 *  -------------------------------------------------------------------------
 *  for V3 open
 */
int
streams_open_ocomm(dev_t dev, unsigned int size, queue_t *q, dev_t *devp, int oflag, int sflag,
		   cred_t *crp)
{
	dev_t mydev = dev;
	int err;

	if ((err = streams_open_comm(size, q, &mydev, oflag, sflag, crp)))
		return (err);
	*devp = mydev;
	return (0);
}

EXPORT_SYMBOL_NOVERS(streams_open_ocomm);

/* 
 *  STREAMS_CLOSE_COMM
 *  -------------------------------------------------------------------------
 *  for both V3 and V4 close
 */
int
streams_close_comm(queue_t *q, int oflag, cred_t *crp)
{
	spin_lock(&str_list_lock);
	if (q->q_ptr) {
		struct str_comm *sp = ((struct str_comm *) q->q_ptr) - 1;
		size_t size = sp->size;

		/* found it */
		if ((*sp->prev = sp->next))
			sp->next->prev = sp->prev;
		sp->next = NULL;
		sp->prev = &sp->next;
		sp->size = 0;
		q->q_ptr = OTHERQ(q)->q_ptr = NULL;
		(void) size;	/* LiS doesn't actually use this */
		kmem_free(sp, sizeof(*sp) + size);
	}
	spin_unlock(&str_list_lock);
	return (0);
}

EXPORT_SYMBOL_NOVERS(streams_close_comm);

#ifndef NODEV
#define NODEV (makedevice(0, 0))
#endif
/*
 *  STRMOD_ADD
 *  -------------------------------------------------------------------------
 */
dev_t
strmod_add(dev_t dev, struct streamtab *st, struct streamadm *sa)
{
#ifdef LIS
	switch (sa->sa_flags & STR_TYPE_MASK) {
	case STR_IS_DEVICE:
	{
		major_t major = getmajor(dev);
		minor_t minor = getminor(dev);
		int result;

		if ((result = lis_register_strdev(major, st, minor, sa->sa_name)) > 0) {
			major = result;
			dev = makedevice(major, minor);
		} else {
			dev = NODEV;
		}
		return (dev);
	}
	case STR_IS_MODULE:
	{
		modID_t modid;
		int result;

		if ((result = lis_register_strmod(st, sa->sa_name)) > 0) {
			modid = result;
			dev = makedevice(modid, 0);
		} else {
			dev = NODEV;
		}
		return (dev);
	}
	default:
		return (NODEV);
	}
#endif
#ifdef LFS
	switch (sa->sa_flags & STR_TYPE_MASK) {
	case STR_IS_DEVICE:
	{
		struct cdevsw *cdev;
		int err;

		if (!(cdev = kmem_zalloc(sizeof(*cdev), KM_NOSLEEP)))
			return (NODEV);
		cdev->d_name = sa->sa_name;
		cdev->d_str = st;
		/* build flags */
		cdev->d_flag = 0;
		if (sa->sa_flags & STR_QSAFETY) {
			cdev->d_flag |= D_SAFE;
		}
		switch ((cdev->d_sqlvl = sa->sa_sync_level)) {
		case SQLVL_NOP:
			cdev->d_flag |= D_MP;
			break;
		case SQLVL_QUEUE:
			cdev->d_flag |= D_MTPERQ;
			break;
		case SQLVL_QUEUEPAIR:
			cdev->d_flag |= D_MTQPAIR;
			break;
		case SQLVL_MODULE:
			cdev->d_flag |= D_MTPERMOD;
			break;
		case SQLVL_ELSEWHERE:
			cdev->d_flag |= D_MTOUTPERIM;
			break;
		case SQLVL_GLOBAL:
			/* can't really support this, but its only used for debug anyway */
			cdev->d_flag &= ~D_MP;
			break;
		case SQLVL_DEFAULT:
			cdev->d_flag |= D_MTPERMOD;
			break;
		}
		if ((err = register_strdev(cdev, getmajor(dev))) < 0) {
			kmem_free(cdev, sizeof(*cdev));
			return (NODEV);
		}
		return (makedevice(err, getminor(dev)));
	}
	case STR_IS_MODULE:
	{
		struct fmodsw *fmod;
		int err;

		if (!(fmod = kmem_zalloc(sizeof(*fmod), KM_NOSLEEP)))
			return (NODEV);
		fmod->f_name = sa->sa_name;
		fmod->f_str = st;
		/* build flags */
		fmod->f_flag = 0;
		if (sa->sa_flags & STR_QSAFETY) {
			fmod->f_flag |= D_SAFE;
		}
		switch ((fmod->f_sqlvl = sa->sa_sync_level)) {
		case SQLVL_NOP:
			fmod->f_flag |= D_MP;
			break;
		case SQLVL_QUEUE:
			fmod->f_flag |= D_MTPERQ;
			break;
		case SQLVL_QUEUEPAIR:
			fmod->f_flag |= D_MTQPAIR;
			break;
		case SQLVL_MODULE:
			fmod->f_flag |= D_MTPERMOD;
			break;
		case SQLVL_ELSEWHERE:
			fmod->f_flag |= D_MTOUTPERIM;
			break;
		case SQLVL_GLOBAL:
			/* can't really support this, but its only used for debug anyway */
			fmod->f_flag &= ~D_MP;
			break;
		case SQLVL_DEFAULT:
			fmod->f_flag |= D_MTPERMOD;
			break;
		}
		if ((err = register_strmod(fmod)) < 0) {
			kmem_free(fmod, sizeof(*fmod));
			return (NODEV);
		}
		return (makedevice(err, 0));
	}
	default:
		return (NODEV);
	}
#endif
}

EXPORT_SYMBOL_NOVERS(strmod_add);

/*
 *  STRMOD_DEL
 *  -------------------------------------------------------------------------
 */
int
strmod_del(dev_t dev, struct streamtab *st, struct streamadm *sa)
{
#ifdef LIS
	switch (sa->sa_flags & STR_TYPE_MASK) {
	case STR_IS_DEVICE:
		return lis_unregister_strdev(getmajor(dev));
	case STR_IS_MODULE:
		return lis_unregister_strmod(st);
	default:
		return (EINVAL);
	}
#endif
#ifdef LFS
	switch (sa->sa_flags & STR_TYPE_MASK) {
	case STR_IS_DEVICE:
	{
		struct cdevsw *cdev;
		int err;

		if ((cdev = cdev_str(st)) == NULL)
			return (ENOENT);
		if ((err = unregister_strdev(cdev, getmajor(dev))) == 0)
			kmem_free(cdev, sizeof(*cdev));
		return (-err);
	}
	case STR_IS_MODULE:
	{
		struct fmodsw *fmod;
		int err;

		if (!(fmod = fmod_str(st)))
			return (ENOENT);
		if ((err = unregister_strmod(fmod)) == 0)
			kmem_free(fmod, sizeof(fmod));
		return (-err);
	}
	default:
		return (EINVAL);
	}
#endif
}

EXPORT_SYMBOL_NOVERS(strmod_del);

#ifdef CONFIG_STREAMS_COMPAT_OSF_MODULE
static
#endif
int __init
osfcomp_init(void)
{
#ifdef CONFIG_STREAMS_COMPAT_OSF_MODULE
	printk(KERN_INFO OSFCOMP_BANNER);
#else
	printk(KERN_INFO OSFCOMP_SPLASH);
#endif
	return (0);
}

#ifdef CONFIG_STREAMS_COMPAT_OSF_MODULE
static
#endif
void __exit
osfcomp_exit(void)
{
	return;
}

#ifdef CONFIG_STREAMS_COMPAT_OSF_MODULE
module_init(osfcomp_init);
module_exit(osfcomp_exit);
#endif
