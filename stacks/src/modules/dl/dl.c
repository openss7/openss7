/*****************************************************************************

 @(#) $RCSfile: dl.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/08/21 10:14:42 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
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

 Last Modified $Date: 2004/08/21 10:14:42 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: dl.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/08/21 10:14:42 $"

static char const ident[] = "$RCSfile: dl.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/08/21 10:14:42 $";

#include <linux/config.h>
#include <linux/version.h>
#ifdef MODVERSIONS
#include <linux/modversions.h>
#endif
#include <linux/module.h>

#include <sys/stream.h>
#include <sys/cmn_err.h>
#include <sys/dki.h>
#include <sys/ddi.h>
#include <sys/osif.h>

#include "debug.h"

#define DL_DESCRIP	"Data Link (DL) STREAMS MULTIPLEXING DRIVER ($Revision: 0.9.2.1 $)"
#define DL_REVISION	"OpenSS7 $RCSfile: dl.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/08/21 10:14:42 $"
#define DL_COPYRIGHT	"Copyright (c) 1997-2003  OpenSS7 Corporation.  All Rights Reserved."
#define DL_DEVICE	"OpenSS7 CDI Devices."
#define DL_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define DL_LICENSE	"GPL"
#define DL_BANNER	DL_DESCRIP	"\n" \
			DL_REVISION	"\n" \
			DL_COPYRIGHT	"\n" \
			DL_DEVICE	"\n" \
			DL_CONTACT

MODULE_AUTHOR(DL_CONTACT);
MODULE_DESCRIPTION(DL_DESCRIP);
MODULE_SUPPORTED_DEVICE(DL_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(DL_LICENSE);
#endif

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

STATIC struct module_info dl_xinfo = {
	mi_idnum:DL_DRV_ID,		/* Module ID number */
	mi_idname:DL_DRV_NAME,		/* Module ID name */
	mi_minpsz:(0),			/* Min packet size accepted */
	mi_maxpsz:INFPSZ,		/* Max packet size accepted */
	mi_hiwat:(1),			/* Hi water mark */
	mi_lowat:(0),			/* Lo water mark */
};

STATIC int dl_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC int dl_close(queue_t *, int, cred_t *);

STATIC struct qinit dl_xinit = {
	qi_qopen:dl_open,		/* Each open */
	qi_qclose:dl_close,		/* Last close */
	qi_minfo:&dl_xinfo,		/* Information */
};

STATIC struct streamtab dl_info = {
	st_rdinit:&dl_xinit,		/* Upper read queue */
	st_wrinit:&dl_xinit,		/* Upper write queue */
};

/*
 *  =========================================================================
 *
 *  OPEN AND CLOSE
 *
 *  =========================================================================
 */
STATIC char drvname[LIS_NAMESZ + 9];
STATIC int dl_majors[256];
STATIC const char *dl_modules[256] = {
	"dl",
	"dl-dua",
	"dl-gr303",
	"dl-gr303ua",
	"dl-gsma",
	"dl-h225",
	"dl-iua",
	"dl-lapb",
	"dl-lapd",
	"dl-lapf",
	"dl-v52",
	"dl-v5ua",
};

/*
 *  =========================================================================
 *
 *  OPEN AND CLOSE
 *
 *  =========================================================================
 */
STATIC int
dl_find_strdev(const char *devname)
{
	int i;
	for (i = 0; i < MAX_STRDEV; i++)
		if (lis_fstr_sw[i].f_str)
			if (strncmp(devname, lis_fstr_sw[i].f_name, LIS_NAMESZ) == 0)
				return (i);
	return (-1);
}

/*
 *  Open
 *  -------------------------------------------------------------------------
 *  This open function automatically loads the appropriate kernel modules and
 *  calls the appropriate driver for the minor device number opened.  Note
 *  that we do an lis_setq and call the actual driver's open function, so we
 *  should never return here.
 */
STATIC int
dl_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	uchar cmajor;
	uchar cminor = getminor(*devp);
	const char *devname;
	struct streamtab *stab = NULL;
	int err;
	if (cminor == 0)
		return (ENOENT);	/* would loop */
	if ((cmajor = dl_majors[cminor])) {
		if (!(stab = lis_find_strdev(dl_majors[cminor])))
			cmajor = dl_majors[cminor] = 0;
		else if (strncmp(dl_modules[cminor], lis_fstr_sw[cmajor].f_name, LIS_NAMESZ) != 0) {
			/*
			   name changed 
			 */
			stab = NULL;
			cmajor = dl_majors[cminor] = 0;
		}
	}
	if (!cmajor) {
		if (!(devname = dl_modules[cminor]))
			return (ENOENT);
		if ((err = dl_find_strdev(devname)) < 0) {
			sprintf(drvname, "streams-%s", devname);
			if ((err = request_module(drvname)))
				return (err < 0 ? -err : err);
			if ((err = dl_find_strdev(devname)) < 0)
				return (ENOENT);
		}
		cmajor = err;
	}
	if (!(stab = lis_find_strdev(cmajor)))
		return (ENOENT);
	lis_setq(q, stab->st_rdinit, stab->st_wrinit);
	if (!q->q_qinfo->qi_qopen) {
		swerr();
		return (EIO);
	}
	return (*q->q_qinfo->qi_qopen) (q, devp, flag, CLONEOPEN, crp);
}

/*
 *  Close
 *  -------------------------------------------------------------------------
 */
STATIC int
dl_close(queue_t *q, int flag, cred_t *crp)
{
	(void) q;
	(void) flag;
	(void) crp;
	swerr();
	return (EIO);
}

/*
 *  =========================================================================
 *
 *  LIS MODULE INITIALIZATION
 *
 *  =========================================================================
 */
#define MODULE_STATIC STATIC
STATIC int dl_initialized = 0;
MODULE_STATIC void
dl_init(void)
{
	int rtn;
	unless(dl_initialized, return);
	cmn_err(CE_NOTE, DL_BANNER);	/* console splash */
	if ((rtn = lis_register_strdev(dl_majors[0], &dl_info, 255, DL_DRV_NAME)) <= 0) {
		cmn_err(CE_PANIC, "%s: Cannot register major %d", DL_DRV_NAME, dl_majors[0]);
		dl_initialized = rtn;
		return;
	}
	dl_majors[0] = rtn;
	LIS_DEVFLAGS(rtn) |= LIS_MODFLG_CLONE;
	dl_initialized = 1;
	return;
}
MODULE_STATIC void
dl_terminate(void)
{
	int rtn;
	if (dl_majors[0]) {
		if ((rtn = lis_unregister_strdev(dl_majors[0])))
			cmn_err(CE_PANIC, "%s: Cannot unregister major %d\n", DL_DRV_NAME,
				dl_majors[0]);
		dl_majors[0] = 0;
	}
	return;
}

/*
 *  =========================================================================
 *
 *  LINUX MODULE INITIALIZATION
 *
 *  =========================================================================
 */
int
init_module(void)
{
	dl_init();
	if (dl_initialized < 0)
		return dl_initialized;
	return (0);
}

void
cleanup_module(void)
{
	dl_terminate();
}
