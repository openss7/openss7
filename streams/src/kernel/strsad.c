/*****************************************************************************

 @(#) strsad.c,v (0.9.2.11) 2003/10/21 21:50:21

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2003  OpenSS7 Corporation <http://www.openss7.com>
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

 Last Modified 2003/10/21 21:50:21 by brian

 *****************************************************************************/

#ident "@(#) strsad.c,v (0.9.2.11) 2003/10/21 21:50:21"

static char const ident[] =
    "strsad.c,v (0.9.2.11) 2003/10/21 21:50:21";

#define __NO_VERSION__

#include <linux/config.h>
#include <linux/version.h>
#ifdef MODVERSIONS
#include <linux/modversions.h>
#endif
#include <linux/module.h>

#include <sys/stropts.h>
#include <sys/stream.h>
#include <sys/strconf.h>
#include <sys/strsubr.h>
#include <sys/ddi.h>
#include <sys/sad.h>

#include "strdebug.h"
#include "strsched.h"		/* for ap_get/ap_put */
#include "strsad.h"		/* extern verification */

static spinlock_t apush_lock = SPIN_LOCK_UNLOCKED;

static struct apinfo *__autopush_find(struct cdevsw *cdev, unsigned char minor)
{
	struct list_head *pos;
	struct apinfo *api = NULL;
	if (cdev->d_apush.next == NULL)
		goto init;
	list_for_each(pos, &cdev->d_apush) {
		api = list_entry(pos, struct apinfo, api_list);
		if (minor >= api->api_sap.sap_minor && minor <= api->api_sap.sap_lastminor)
			break;
		api = NULL;
	}
	return (api);
      init:
	INIT_LIST_HEAD(&cdev->d_apush);
	return (NULL);
}

static int __autopush_add(struct cdevsw *cdev, struct strapush *sap)
{
	struct apinfo *api;
	int err;
	err = -EEXIST;
	if ((api = __autopush_find(cdev, sap->sap_minor)) != NULL)
		goto error;
	err = -ENOSR;
	if ((api = ap_get(sap)) == NULL)
		goto error;
	list_add_tail(&api->api_list, &cdev->d_apush);
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
	if ((cdev = sdev_get(dev)) == NULL)
		goto notfound;
	spin_lock_irqsave(&apush_lock, flags);
	if ((api = __autopush_find(cdev, getminor(dev))) != NULL)
		ap_grab(api);
	spin_unlock_irqrestore(&apush_lock, flags);
	sdev_put(cdev);
      notfound:
	return ((struct strapush *) api);
}

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
	if (sap->sap_major >= MAX_CHRDEV)
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
	if ((cdev = sdev_get(makedevice(sap->sap_major, sap->sap_minor))) == NULL)
		goto error;
	spin_lock_irqsave(&apush_lock, flags);
	err = __autopush_add(cdev, sap);
	spin_unlock_irqrestore(&apush_lock, flags);
	sdev_put(cdev);
      error:
	return (err);
}

int autopush_del(struct strapush *sap)
{
	unsigned long flags;
	struct cdevsw *cdev;
	int err;
	err = -EINVAL;
	if (sap->sap_major >= MAX_CHRDEV)
		goto error;
	err = -ENODEV;
	if ((cdev = sdev_get(makedevice(sap->sap_major, sap->sap_minor))) == NULL)
		goto error;
	spin_lock_irqsave(&apush_lock, flags);
	err = __autopush_del(cdev, sap);
	spin_unlock_irqrestore(&apush_lock, flags);
	sdev_put(cdev);
      error:
	return (err);
}

int autopush_vml(struct str_mlist *smp, int nmods)
{
	int rtn = 0, k;
	for (k = 0; k < nmods; k++, smp++) {
		int len;
		struct fmodsw *fmod;
		len = strnlen(smp->l_name, FMNAMESZ + 1);
		if (len == 0 || len == FMNAMESZ + 1)
			goto einval;
		if ((fmod = smod_get(smp->l_name)) != NULL)
			smod_put(fmod);
		else
			rtn = 1;
	}
	return (rtn);
      einval:
	return (-EINVAL);
}

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

int apush_get(struct strapush *sap)
{
	struct strapush *ap;
	if (sap != NULL) {
		dev_t dev;
		if (sap->sap_major >= MAX_CHRDEV)
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

int apush_vml(struct str_list *slp)
{
	return autopush_vml(slp->sl_modlist, slp->sl_nmods);
}
