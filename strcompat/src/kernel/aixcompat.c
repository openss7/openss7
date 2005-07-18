/*****************************************************************************

 @(#) $RCSfile: aixcompat.c,v $ $Name:  $($Revision: 0.9.2.17 $) $Date: 2005/07/18 12:25:41 $

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

 Last Modified $Date: 2005/07/18 12:25:41 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: aixcompat.c,v $ $Name:  $($Revision: 0.9.2.17 $) $Date: 2005/07/18 12:25:41 $"

static char const ident[] =
    "$RCSfile: aixcompat.c,v $ $Name:  $($Revision: 0.9.2.17 $) $Date: 2005/07/18 12:25:41 $";

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

#define __AIX_EXTERN_INLINE inline

#define _AIX_SOURCE

#include "sys/os7/compat.h"

#define AIXCOMP_DESCRIP		"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define AIXCOMP_COPYRIGHT	"Copyright (c) 1997-2005 OpenSS7 Corporation.  All Rights Reserved."
#define AIXCOMP_REVISION	"LfS $RCSfile: aixcompat.c,v $ $Name:  $($Revision: 0.9.2.17 $) $Date: 2005/07/18 12:25:41 $"
#define AIXCOMP_DEVICE		"AIX 5L Version 5.1 Compatibility"
#define AIXCOMP_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define AIXCOMP_LICENSE		"GPL"
#define AIXCOMP_BANNER		AIXCOMP_DESCRIP		"\n" \
				AIXCOMP_COPYRIGHT	"\n" \
				AIXCOMP_REVISION	"\n" \
				AIXCOMP_DEVICE		"\n" \
				AIXCOMP_CONTACT		"\n"
#define AIXCOMP_SPLASH		AIXCOMP_DEVICE		" - " \
				AIXCOMP_REVISION	"\n"

#ifdef CONFIG_STREAMS_COMPAT_AIX_MODULE
MODULE_AUTHOR(AIXCOMP_CONTACT);
MODULE_DESCRIPTION(AIXCOMP_DESCRIP);
MODULE_SUPPORTED_DEVICE(AIXCOMP_DEVICE);
MODULE_LICENSE(AIXCOMP_LICENSE);
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-aixcompat");
#endif
#endif

/* 
 *  MI_OPEN_COMM, MI_CLOSE_COMM, MI_NEXT_PTR, MI_PREV_PTR and MI_BUFCALL
 *  -------------------------------------------------------------------------
 *  implemented in mspcompat.c
 */

#if LFS
/* 
 *  WANTIO
 *  -------------------------------------------------------------------------
 */
int
wantio(queue_t *q, struct wantio *w)
{
	queue_t *wq = WR(q);
	struct stdata *sd = ((struct queinfo *) (wq - 1))->qu_str;

	if (w) {
		// qget(wq);
		xchg(&sd->sd_directio, w);
	} else {
		xchg(&sd->sd_directio, w);
		// qput(&wq);
	}
	return (0);
}

EXPORT_SYMBOL(wantio);		/* aix/ddi.h */

/* 
 *  WANTMSG
 *  -------------------------------------------------------------------------
 */
__AIX_EXTERN_INLINE int wantmsg(queue_t *q, int (*func) (mblk_t *));

EXPORT_SYMBOL(wantmsg);		/* aix/ddi.h */
#endif

/* 
 *  STR_INSTALL
 *  -------------------------------------------------------------------------
 */
int
str_install_AIX(int cmd, strconf_t * sc)
{
	if (!sc)
		return (EINVAL);
	switch (cmd) {
	case STR_LOAD_DEV:
#if LIS
	{
		int err;

		if ((err = lis_register_strdev(sc->sc_major, sc->sc_str, 255, sc->sc_name)) > 0)
			sc->sc_major = err;
		return (err < 0 ? -err : 0);
	}
#endif
#if LFS
		{
			struct cdevsw *cdev;
			int err;

#ifdef MAX_CHRDEV
			if (0 >= sc->sc_major || sc->sc_major >= MAX_CHRDEV)
#else
			if (sc->sc_major != MAJOR(MKDEV(sc->sc_major, 0)))
#endif
				return (EINVAL);
			/* We don't do old-style opens */
			if (!(sc->sc_open_stylesc_flags & STR_NEW_OPEN))
				return (ENOSYS);
			if (sc->sc_open_stylesc_flags & STR_Q_NOTTOSPEC) {
				/* Modules can always sleep because we are running at soft IRQ. */
			}
			if (!(cdev = kmem_zalloc(sizeof(*cdev), KM_NOSLEEP)))
				return (ENOMEM);
			cdev->d_name = sc->sc_name;
			cdev->d_str = sc->sc_str;
			/* build flags */
			cdev->d_flag = 0;
			if (sc->sc_open_stylesc_flags & STR_MPSAFE) {
				cdev->d_flag |= D_MP;
			}
			if (sc->sc_open_stylesc_flags & STR_QSAFETY) {
				cdev->d_flag |= D_SAFE;
			}
			if (sc->sc_open_stylesc_flags & STR_PERSTREAM) {
				cdev->d_flag |= D_UP;
			}
			if (sc->sc_open_stylesc_flags & STR_NEWCLONING) {
				cdev->d_flag |= D_CLONE;
			}
			switch ((cdev->d_sqlvl = sc->sc_sqlevel)) {
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
			if ((err = register_strdev(cdev, sc->sc_major)) < 0)
				kmem_free(cdev, sizeof(*cdev));
			return (-err);
		}
#endif
	case STR_UNLOAD_DEV:
#if LIS
	{
		return lis_unregister_strdev(sc->sc_major);
	}
#endif
#if LFS
		{
			struct cdevsw *cdev;
			int err;

			if (0 >= sc->sc_major || sc->sc_major >= MAX_STRDEV)
				return (EINVAL);
			if ((cdev = sdev_get(sc->sc_major)) == NULL)
				return (ENOENT);
			printd(("%s: %s: got device\n", __FUNCTION__, cdev->d_name));
			printd(("%s: %s: putting device\n", __FUNCTION__, cdev->d_name));
			sdev_put(cdev);
			if ((err = unregister_strdev(cdev, sc->sc_major)) == 0)
				kmem_free(cdev, sizeof(*cdev));
			return (-err);
		}
#endif
	case STR_LOAD_MOD:
#if LIS
	{
		int err;

		if ((err = lis_register_strmod(sc->sc_str, sc->sc_name)) > 0)
			sc->sc_major = err;
		return (err < 0 ? -err : 0);
	}
#endif
#if LFS
		{
			struct fmodsw *fmod;
			int err;

			if (!sc->sc_str || !sc->sc_str->st_rdinit
			    || !sc->sc_str->st_rdinit->qi_minfo)
				return (EINVAL);
			if (0 >= sc->sc_major || sc->sc_major >= MAX_STRDEV)
				return (EINVAL);
			/* We don't do old-style opens */
			if (!(sc->sc_open_stylesc_flags & STR_NEW_OPEN))
				return (ENOSYS);
			if (!(fmod = kmem_zalloc(sizeof(*fmod), KM_NOSLEEP)))
				return (ENOMEM);
			fmod->f_name = sc->sc_name;
			fmod->f_str = sc->sc_str;
			/* build flags */
			fmod->f_flag = 0;
			if (sc->sc_open_stylesc_flags & STR_MPSAFE) {
				fmod->f_flag |= D_MP;
			}
			if (sc->sc_open_stylesc_flags & STR_PERSTREAM) {
				fmod->f_flag |= D_UP;
			}
			if (sc->sc_open_stylesc_flags & STR_Q_NOTTOSPEC) {
				/* Modules can always sleep because we are running at soft IRQ;
				   however, it has a horrendous impact.  We just ignore it. */
			}
			if (sc->sc_open_stylesc_flags & STR_QSAFETY) {
				fmod->f_flag |= D_SAFE;
			}
			if (sc->sc_open_stylesc_flags & STR_NEWCLONING) {
				fmod->f_flag |= D_CLONE;
			}
			switch ((fmod->f_sqlvl = sc->sc_sqlevel)) {
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
			if ((err = register_strmod(fmod)) < 0)
				kmem_free(fmod, sizeof(*fmod));
			return (-err);
		}
#endif
	case STR_UNLOAD_MOD:
#if LIS
	{
		return lis_unregister_strmod(sc->sc_str);
	}
#endif
#if LFS
		{
			struct fmodsw *fmod;
			int err;

			if (!(fmod = fmod_str(sc->sc_str)))
				return (ENOENT);
			if ((err = unregister_strmod(fmod)) == 0)
				kmem_free(fmod, sizeof(fmod));
			return (-err);
		}
#endif
	}
	return (EINVAL);
}

EXPORT_SYMBOL(str_install_AIX);	/* strconf.h */

#ifdef CONFIG_STREAMS_COMPAT_AIX_MODULE
static
#endif
int __init
aixcomp_init(void)
{
#ifdef CONFIG_STREAMS_COMPAT_AIX_MODULE
	printk(KERN_INFO AIXCOMP_BANNER);
#else
	printk(KERN_INFO AIXCOMP_SPLASH);
#endif
	return (0);
}

#ifdef CONFIG_STREAMS_COMPAT_AIX_MODULE
static
#endif
void __exit
aixcomp_exit(void)
{
	return;
}

#ifdef CONFIG_STREAMS_COMPAT_AIX_MODULE
module_init(aixcomp_init);
module_exit(aixcomp_exit);
#endif
