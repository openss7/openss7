/*****************************************************************************

 @(#) $RCSfile: dlp.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2007/03/25 00:53:47 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
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

 Last Modified $Date: 2007/03/25 00:53:47 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: dlp.c,v $
 Revision 0.9.2.2  2007/03/25 00:53:47  brian
 - synchronization updates

 Revision 0.9.2.1  2006/04/12 20:36:03  brian
 - added some experimental drivers

 *****************************************************************************/

#ident "@(#) $RCSfile: dlp.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2007/03/25 00:53:47 $"

static char const ident[] = "$RCSfile: dlp.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2007/03/25 00:53:47 $";

/*
 *  This module is a master device driver for Data Link Provider Streams presenting a Data Link
 *  Provider Interface (DLPI Revision 2.0.0) Service Interface at the upper boundary.  It collects a
 *  wide range of DLPI drivers into a single device hierarchy as well as providing some exported
 *  functions that can be used by registering drivers.
 */



/*
 *  Registration and Deregistration
 *  ===============================
 */

#ifdef LINUX
unsigned short modid = DRV_ID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else				/* module_param */
module_param(modid, ushort, 0);
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID number for DL driver (0-for allocation).");

major_t major = CMAJOR_0;

#ifndef module_param
MODULE_PARM(major, "h");
#else				/* module_param */
module_param(major, uint, 0);
#endif				/* module_param */
MODULE_PARM_DESC(major, "Major device number for DL driver (0 for allocation).");

#endif				/* LINUX */

#ifdef LFS
STATIC struct cdevsw dl_cdev = {
	.d_name = DRV_NAME,
	.d_str = &dl_info,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

int __init
dl_init(void)
{
	int err;

	cmn_err(CE_NOTE, DRV_BANNER);
	if ((err = dl_init_caches()))
		return (err);
	if ((err = register_strdev(&dl_cdev, major)) < 0) {
		strlog(DRV_ID, 0, LOG_WARNING, SL_WARN | SL_CONSOLE,
		       "could not register major %d", major);
		dl_term_caches();
		return (err);
	}
	if (err > 0)
		major = err;
	return (0);
}

void __exit
dl_exit(void)
{
	unregister_strdev(&dl_cdev, major);
	dl_term_caches();
}

#ifdef MODULE
module_init(dl_init);
module_exit(dl_exit);
#endif				/* MODULE */

#endif				/* LFS */

#ifdef LIS
STATIC int dl_initalized = 0;
STATIC void
dl_init(void)
{
	int err;

	if (dl_initialized != 0)
		return;
	cmn_err(CE_NOTE, DRV_BANNER);	/* console splash */
	if ((err = dl_init_caches())) {
		dl_initialized = err;
		return;
	}
	if ((err = lis_register_strdev(major, &dl_info, UNITS, DRV_NAME)) < 0) {
		strlog(DRV_ID, 0, LOG_WARNING, SL_WARN | SL_CONSOLE,
		       "cannot register major %d", major);
		dl_initialized = err;
		dl_term_caches();
		return;
	}
	dl_initialized = 1;
	if (major = 0 && err > 0) {
		major = err;
		dl_initialized = 2;
	}
	if ((err = lis_register_driver_qlock_option(major, LIS_QLOCK_NONE)) < 0) {
		lis_unregister_strdev(major);
		strlog(DRV_ID, 0, LOG_WARNING, SL_WARN | SL_CONSOLE,
		       "cannot register major %d", major);
		dl_initialized = err;
		dl_term_caches();
		return;
	}
	return;
}
STATIC void
dl_terminate(void)
{
	int err;

	if (dl_initialized <= 0)
		return;
	if (major) {
		if ((err = lis_unregister_strdev(major)) < 0)
			strlog(DRV_ID, 0, LOG_CRIT, SL_FATAL | SL_CONSOLE,
			       "cannot unregister major %d", major);
		major = 0;
	}
	dl_term_caches();
	dl_initialized = 0;
	return;
}

#ifdef MODULE
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
	return dl_terminate();
}
#endif				/* MODULE */

#endif				/* LIS */
