/*****************************************************************************

 @(#) $RCSfile: cd_daed.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/08/21 10:14:41 $

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

 Last Modified $Date: 2004/08/21 10:14:41 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: cd_daed.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/08/21 10:14:41 $"

static char const ident[] =
    "$RCSfile: cd_daed.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/08/21 10:14:41 $";

#include <linux/config.h>
#include <linux/version.h>
#ifdef MODVERSIONS
#include <linux/modversions.h>
#endif
#include <linux/module.h>
#include <sys/stream.h>
#include <sys/stropts.h>
#include <sys/cmn_err.h>

#include <sys/cdi.h>
#include <sys/cdi_daed.h>
#include <ss7/chi.h>
#include <ss7/chi_ioctl.h>
#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/hdlc_ioctl.h>

#include "debug.h"
#include "bufq.h"
#include "priv.h"
#include "lock.h"
#include "queue.h"
#include "allocb.h"
#include "timer.h"

#include "cd/cd.h"

#define DAED_DESCRIP	"Q.703/T1.111.3 DAED: (Delimination Alignment and Error Detection) STREAMS MODULE."
#define DAED_REVISION	"OpenSS7 $RCSfile: cd_daed.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/08/21 10:14:41 $"
#define DAED_COPYRIGHT	"Copyright (c) 1997-2003 OpenSS7 Corporation.  All Rights Reserved."
#define DAED_DEVICES	"Supports OpenSS7 Channel Drivers."
#define DAED_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define DAED_LICENSE	"GPL"
#define DAED_BANNER	DAED_DESCRIP	"\n" \
			DAED_REVISION	"\n" \
			DAED_COPYRIGHT	"\n" \
			DAED_DEVICES	"\n" \
			DAED_CONTACT	"\n"

MODULE_AUTHOR(DAED_CONTACT);
MODULE_DESCRIPTION(DAED_DESCRIP);
MODULE_SUPPORTED_DEVICE(DAED_DEVICES);
#ifdef MODULE_LICENSE
MODULE_LICENSE(DAED_LICENSE);
#endif

/*
 *  =======================================================================
 *
 *  STREAMS Definitions
 *
 *  =======================================================================
 */

STATIC struct module_info daed_winfo = {
	mi_idnum:DAED_MOD_ID,		/* Module ID number */
	mi_idname:DAED_MOD_NAME "-wr",	/* Module name */
	mi_minpsz:(1),			/* Min packet size accepted */
	mi_maxpsz:INFPSZ,		/* Max packet size accepted */
	mi_hiwat:(1),			/* Hi water mark */
	mi_lowat:(0),			/* Lo water mark */
};

STATIC struct module_info daed_rinfo = {
	mi_idnum:DAED_MOD_ID,		/* Module ID number */
	mi_idname:DAED_MOD_NAME "-rd",	/* Module name */
	mi_minpsz:(1),			/* Min packet size accepted */
	mi_maxpsz:INFPSZ,		/* Max packet size accepted */
	mi_hiwat:(1),			/* Hi water mark */
	mi_lowat:(0),			/* Lo water mark */
};

STATIC int daed_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC int daed_close(queue_t *, int, cred_t *);

STATIC struct qinit daed_rinit = {
	qi_putp:ss7_oput,		/* Read put (message from below) */
	qi_srvp:ss7_osrv,		/* Read queue service */
	qi_qopen:daed_open,		/* Each open */
	qi_qclose:daed_close,		/* Last close */
	qi_minfo:&daed_rinfo,		/* Information */
};

STATIC struct qinit daed_winit = {
	qi_putp:ss7_iput,		/* Write put (message from above) */
	qi_srvp:ss7_isrv,		/* Write queue service */
	qi_minfo:&daed_winfo,		/* Information */
};

STATIC struct streamtab daed_info = {
	st_rdinit:&daed_rinit,		/* Upper read queue */
	st_wrinit:&daed_winit,		/* Upper write queue */
};

/*
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 */
/*
 *  OPEN
 *  -------------------------------------------------------------------------
 */
STATIC struct str *daed_list = NULL;
STATIC int
daed_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	MOD_INC_USE_COUNT;	/* keep module from unloading in our face */
	if (q->q_ptr != NULL) {
		MOD_DEC_USE_COUNT;
		return (0);	/* already open */
	}
	if (sflag == MODOPEN || WR(q)->q_next != NULL) {
		int cmajor = getmajor(*devp);
		int cminor = getminor(*devp);
		struct str *str;
		/*
		   test for multiple push 
		 */
		for (str = daed_list; str; str = str->next) {
			if (str->u.dev.cmajor == cmajor && str->u.dev.cminor == cminor) {
				MOD_DEC_USE_COUNT;
				return (ENXIO);
			}
		}
		if (!(str = cd_alloc_priv(q, &daed_list, devp, crp, CD_DAED))) {
			MOD_DEC_USE_COUNT;
			return (ENOMEM);
		}
		return (0);
	}
	MOD_DEC_USE_COUNT;
	return (EIO);
}

/*
 *  CLOSE
 *  -------------------------------------------------------------------------
 */
STATIC int
daed_close(queue_t *q, int flag, cred_t *crp)
{
	(void) flag;
	(void) crp;
	cd_free_priv(q);
	MOD_DEC_USE_COUNT;
	return (0);
}

/*
 *  =======================================================================
 *
 *  LiS Module Initialization (For unregistered driver.)
 *
 *  =======================================================================
 */
STATIC int daed_initialized = 0;
STATIC void
daed_init(void)
{
	unless(daed_initialized > 0, return);
	cmn_err(CE_NOTE, DAED_BANNER);	/* console splash */
	if ((daed_initialized = lis_register_strmod(&daed_info, DAED_MOD_NAME)) < 0) {
		cmn_err(CE_WARN, "%s: couldn't register module", DAED_MOD_NAME);
		return;
	}
	daed_initialized = 1;
	return;
}
STATIC void
daed_terminate(void)
{
	ensure(daed_initialized > 0, return);
	if ((daed_initialized = lis_unregister_strmod(&daed_info)) < 0) {
		cmn_err(CE_PANIC, "%s: couldn't unregister module", DAED_MOD_NAME);
		return;
	}
	daed_initialized = 0;
	return;
}

/*
 *  =======================================================================
 *
 *  Kernel Module Initialization
 *
 *  =======================================================================
 */
int
init_module(void)
{
	daed_init();
	if (daed_initialized < 0)
		return daed_initialized;
	return (0);
}

void
cleanup_module(void)
{
	(void) ss7_unbufcall;
	daed_terminate();
}
