/*****************************************************************************

 @(#) $RCSfile: strsad.c,v $ $Name:  $($Revision: 0.9.2.25 $) $Date: 2005/05/11 20:12:21 $

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
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2005/05/11 20:12:21 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: strsad.c,v $ $Name:  $($Revision: 0.9.2.25 $) $Date: 2005/05/11 20:12:21 $"

static char const ident[] =
    "$RCSfile: strsad.c,v $ $Name:  $($Revision: 0.9.2.25 $) $Date: 2005/05/11 20:12:21 $";

#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>

#include <sys/stream.h>
#include <sys/strconf.h>
#include <sys/strsubr.h>
#include <sys/ddi.h>
#include <sys/sad.h>

#include "sys/config.h"
#include "strsched.h"		/* for ap_alloc/ap_put */
#include "strsad.h"		/* extern verification */

static spinlock_t apush_lock = SPIN_LOCK_UNLOCKED;

static struct apinfo *__autopush_find(struct cdevsw *cdev, unsigned char minor)
{
	struct list_head *pos;
	struct apinfo *api = NULL;
	if (!cdev->d_apush.next)
		INIT_LIST_HEAD(&cdev->d_apush);
	list_for_each(pos, &cdev->d_apush) {
		api = list_entry(pos, struct apinfo, api_more);
		if (minor >= api->api_sap.sap_minor && minor <= api->api_sap.sap_lastminor)
			break;
		api = NULL;
	}
	return (api);
}

static int __autopush_add(struct cdevsw *cdev, struct strapush *sap)
{
	struct apinfo *api;
	int err;
	err = -EEXIST;
	if ((api = __autopush_find(cdev, sap->sap_minor)) != NULL)
		goto error;
	err = -ENOSR;
	if ((api = ap_alloc(sap)) == NULL)
		goto error;
	if (!cdev->d_apush.next)
		INIT_LIST_HEAD(&cdev->d_apush);
	list_add_tail(&api->api_more, &cdev->d_apush);
	return (0);
      error:
	return (err);
}

static int __autopush_del(struct cdevsw *cdev, struct strapush *sap)
{
	struct apinfo *api;
	int err;
	err = -ENODEV;
	if ((api = __autopush_find(cdev, sap->sap_minor)) == NULL)
		goto error;
	err = -EINVAL;
	if (sap->sap_minor != api->api_sap.sap_minor)
		goto error;
	ap_put(api);
	return (0);
      error:
	return (err);
}

struct strapush *autopush_find(dev_t dev)
{
	unsigned long flags;
	struct cdevsw *cdev;
	struct apinfo *api = NULL;
	if ((cdev = cdrv_get(getmajor(dev))) == NULL)
		goto notfound;
	printd(("%s: %s: got driver\n", __FUNCTION__, cdev->d_name));
	spin_lock_irqsave(&apush_lock, flags);
	if ((api = __autopush_find(cdev, getminor(dev))) != NULL)
		ap_get(api);
	spin_unlock_irqrestore(&apush_lock, flags);
	printd(("%s: %s: putting driver\n", __FUNCTION__, cdev->d_name));
	cdrv_put(cdev);
      notfound:
	return ((struct strapush *) api);
}
#if defined CONFIG_STREAMS_SAD_MODULE
EXPORT_SYMBOL_GPL(autopush_find);
#endif

int autopush_add(struct strapush *sap)
{
	unsigned long flags;
	struct cdevsw *cdev;
	int err, k;
	err = -EINVAL;
	switch (sap->sap_cmd) {
	case SAP_ONE:
		sap->sap_lastminor = sap->sap_minor;
	case SAP_RANGE:
		break;
	case SAP_ALL:
		sap->sap_minor = 0;
		sap->sap_lastminor = 255;
		break;
	default:
		goto error;
	}
#ifndef MAX_CHRDEV
	if (sap->sap_major != MAJOR(MKDEV(sap->sap_major, 0)))
#else
	if (sap->sap_major >= MAX_CHRDEV)
#endif
		goto error;
	if (sap->sap_minor > 255)
		goto error;
	if (1 > sap->sap_npush || sap->sap_npush > MAXAPUSH)
		goto error;
	for (k = 0; k < sap->sap_npush; k++) {
		int len = strnlen(sap->sap_list[k], FMNAMESZ + 1);
		if (len == 0 || len == FMNAMESZ + 1)
			goto error;
	}
	err = -ENOSTR;
	if ((cdev = cdev_get(sap->sap_major)) == NULL)
		goto error;
	printd(("%s: %s: got device\n", __FUNCTION__, cdev->d_name));
	spin_lock_irqsave(&apush_lock, flags);
	err = __autopush_add(cdev, sap);
	spin_unlock_irqrestore(&apush_lock, flags);
	printd(("%s: %s: putting device\n", __FUNCTION__, cdev->d_name));
	cdev_put(cdev);
      error:
	return (err);
}
#if defined CONFIG_STREAMS_SAD_MODULE
EXPORT_SYMBOL_GPL(autopush_add);
#endif

int autopush_del(struct strapush *sap)
{
	unsigned long flags;
	struct cdevsw *cdev;
	int err;
	err = -EINVAL;
#ifndef MAX_CHRDEV
	if (sap->sap_major != MAJOR(MKDEV(sap->sap_major, 0)))
#else
	if (sap->sap_major >= MAX_CHRDEV)
#endif
		goto error;
	err = -ENODEV;
	if ((cdev = cdev_get(sap->sap_major)) == NULL)
		goto error;
	printd(("%s: %s: got device\n", __FUNCTION__, cdev->d_name));
	spin_lock_irqsave(&apush_lock, flags);
	err = __autopush_del(cdev, sap);
	spin_unlock_irqrestore(&apush_lock, flags);
	printd(("%s: %s: putting device\n", __FUNCTION__, cdev->d_name));
	cdev_put(cdev);
      error:
	return (err);
}
#if defined CONFIG_STREAMS_SAD_MODULE
EXPORT_SYMBOL_GPL(autopush_del);
#endif

int autopush_vml(struct str_mlist *smp, int nmods)
{
	int rtn = 0, k;
	for (k = 0; k < nmods; k++, smp++) {
		int len;
		struct fmodsw *fmod;
		len = strnlen(smp->l_name, FMNAMESZ + 1);
		if (len == 0 || len == FMNAMESZ + 1)
			goto einval;
		if ((fmod = fmod_find(smp->l_name)) != NULL) {
			printd(("%s: %s: got module\n", __FUNCTION__, fmod->f_name));
			printd(("%s: %s: putting module\n", __FUNCTION__, fmod->f_name));
			fmod_put(fmod);
		} else
			rtn = 1;
	}
	return (rtn);
      einval:
	return (-EINVAL);
}
#if defined CONFIG_STREAMS_SAD_MODULE
EXPORT_SYMBOL_GPL(autopush_vml);
#endif

#if defined CONFIG_STREAMS_COMPAT_LIS || defined CONFIG_STREAMS_COMPAT_LIS_MODULE
int apush_set(struct strapush *sap)
{
	if (sap != NULL) {
		switch (sap->sap_cmd) {
		case SAP_CLEAR:
			return autopush_del(sap);
		case SAP_ONE:
		case SAP_RANGE:
		case SAP_ALL:
			return autopush_add(sap);
		}
	}
	return (-EINVAL);
}
#if defined CONFIG_STREAMS_COMPAT_LIS_MODULE
EXPORT_SYMBOL_GPL(apush_set);
#endif
#endif

#if defined CONFIG_STREAMS_COMPAT_LIS || defined CONFIG_STREAMS_COMPAT_LIS_MODULE
int apush_get(struct strapush *sap)
{
	struct strapush *ap;
	if (sap != NULL) {
		dev_t dev;
#ifndef MAX_CHRDEV
		if (sap->sap_major != MAJOR(MKDEV(sap->sap_major, 0)))
#else
		if (sap->sap_major >= MAX_CHRDEV)
#endif
			goto einval;
		if (sap->sap_minor > 255)
			goto einval;
		dev = makedevice(sap->sap_major, sap->sap_minor);
		ap = autopush_find(dev);
		if (!ap)
			goto enodev;
		*sap = *ap;
		ap_put((struct apinfo *) ap);
		return (0);
	}
      einval:
	return (-EINVAL);
      enodev:
	return (-ENODEV);
}
#if defined CONFIG_STREAMS_COMPAT_LIS_MODULE
EXPORT_SYMBOL_GPL(apush_get);	/* strconf.h  LiS specific */
#endif
#endif

#if defined CONFIG_STREAMS_COMPAT_LIS || defined CONFIG_STREAMS_COMPAT_LIS_MODULE
int apush_vml(struct str_list *slp)
{
	return autopush_vml(slp->sl_modlist, slp->sl_nmods);
}
#if defined CONFIG_STREAMS_COMPAT_LIS_MODULE
EXPORT_SYMBOL_GPL(apush_vml);
#endif
#endif

/**
 *  autopush: - perform autopush operations on a newly opened stream
 *  @sd: newly opened stream head
 *  @cdev: character device switch table entry (driver) for the stream
 *  @oflag: open flags
 *  @sflag: stream flag (%MODOPEN or %CLONEOPEN or %DRVOPEN)
 *  @crp: pointer to user credentials structure
 */
int autopush(struct stdata *sd, struct cdevsw *cdev, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	struct apinfo *api;
	int err;
	if ((api = (typeof(api)) autopush_find(*devp)) != NULL) {
		int k;
		for (k = 0; k < MAX_APUSH; k++) {
			struct fmodsw *fmod;
			dev_t dev;
			if (api->api_sap.sap_list[k][0] == 0)
				break;
			if (!(fmod = fmod_find(api->api_sap.sap_list[k]))) {
				err = -EIO;
				goto abort_autopush;
			}
			printd(("%s: %s: found module\n", __FUNCTION__, fmod->f_name));
			dev = *devp;	/* don't change dev nr */
			if (fmod->f_str == NULL) {
				printd(("%s: %s: putting module\n", __FUNCTION__, fmod->f_name));
				fmod_put(fmod);
				err = -EIO;
				goto abort_autopush;
			}
			if ((err = qattach(sd, fmod, &dev, oflag, sflag, crp))) {
				printd(("%s: %s: putting module\n", __FUNCTION__, fmod->f_name));
				fmod_put(fmod);
				goto abort_autopush;
			}
			atomic_inc(&fmod->f_count);
		}
	}
	return (0);
      abort_autopush:
	{
		/* detach everything, including the driver */
		queue_t *wq = sd->sd_wq;
		if (wq)
			while (wq->q_next && SAMESTR(wq))
				qdetach(wq->q_next - 1, oflag, crp);
		return (err);
	}
}
#if defined CONFIG_STREAMS_STH_MODULE || \
    defined CONFIG_STREAMS_FIFO_MODULE || \
    defined CONFIG_STREAMS_PIPE_MODULE || \
    defined CONFIG_STREAMS_SOCK_MODULE
EXPORT_SYMBOL_GPL(autopush);
#endif
