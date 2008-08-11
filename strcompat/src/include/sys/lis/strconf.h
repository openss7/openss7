/*****************************************************************************

 @(#) $Id: strconf.h,v 0.9.2.21 2008-08-11 22:23:16 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

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

 Last Modified $Date: 2008-08-11 22:23:16 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: strconf.h,v $
 Revision 0.9.2.21  2008-08-11 22:23:16  brian
 - rationalization of header files

 Revision 0.9.2.20  2008-04-28 16:47:09  brian
 - updates for release

 Revision 0.9.2.19  2007/08/12 15:51:06  brian
 - header and extern updates, GPLv3, 3 new lock functions

 Revision 0.9.2.18  2007/03/25 00:52:33  brian
 - synchronization updates

 Revision 0.9.2.17  2006/12/08 05:08:11  brian
 - some rework resulting from testing and inspection

 Revision 0.9.2.16  2006/11/03 10:39:19  brian
 - updated headers, correction to mi_timer_expiry type

 *****************************************************************************/

#ifndef __SYS_LIS_STRCONF_H__
#define __SYS_LIS_STRCONF_H__

#ident "@(#) $RCSfile: strconf.h,v $ $Name:  $($Revision: 0.9.2.21 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

#ifndef __SYS_STRCONF_H__
#warning "Do not include sys/aix/strconf.h directly, include sys/strconf.h instead."
#endif

#ifndef __KERNEL__
#error "Do not use kernel headers for user space programs"
#endif				/* __KERNEL__ */

#ifndef __depr
#if __GNUC__ >= 3
#define __depr __attribute__ ((__deprecated__))
#else
#define __depr
#endif
#endif

#undef _RP
#define _RP streamscall __depr

#ifndef __EXTERN_INLINE
#define __EXTERN_INLINE extern __inline__
#endif

#ifndef __LIS_EXTERN_INLINE
#define __LIS_EXTERN_INLINE __EXTERN_INLINE streamscall
#endif				/* __LIS_EXTERN_INLINE */

#ifndef __LIS_EXTERN
#define __LIS_EXTERN extern streamscall
#endif				/* __AIX_EXTERN_INLINE */

#ifndef _LIS_SOURCE
#warning "_LIS_SOURCE not defined but LIS strconf.h included"
#define _LIS_SOURCE 1
#endif

#include <sys/strcompat/config.h>

#if defined(CONFIG_STREAMS_COMPAT_LIS) || defined(CONFIG_STREAMS_COMPAT_LIS_MODULE)

#ifndef dev_t
#define dev_t __streams_dev_t
#endif

typedef struct lis_strrecvfd {
	union {
		struct file *fp;
		int fd;
	} f;
	uid_t uid;
	gid_t gid;
	struct {
		struct file *fp;
		struct stdata *hd;
	} r;
} strrecvfd_t;

#define LIS_MAXAPUSH	8
#define LIS_FMNAMESZ	8
struct lis_strapush {
	unsigned sap_cmd;
	unsigned long sap_major;
	unsigned long sap_minor;
	unsigned long sap_lastminor;
	unsigned int sap_npush;
	char sap_list[LIS_MAXAPUSH][LIS_FMNAMESZ + 1];
};

extern int streamscall lis_register_strdev(major_t major, struct streamtab *strtab, int nminor,
				   const char *name);
extern int streamscall lis_unregister_strdev(major_t major);
extern modID_t streamscall lis_register_strmod(struct streamtab *strtab, const char *name);
extern int streamscall lis_unregister_strmod(struct streamtab *strtab);

#define LIS_QLOCK_NONE		0
#define LIS_QLOCK_QUEUE		1
#define LIS_QLOCK_QUEUE_PAIR	2
#define LIS_QLOCK_GLOBAL	3

extern int _RP lis_register_module_qlock_option(modID_t id, int qlock_option);
extern int _RP lis_register_driver_qlock_option(major_t major, int qlock_option);

extern int _RP lis_apush_get(struct lis_strapush *ap);
extern int _RP lis_apush_set(struct lis_strapush *ap);
extern int _RP lis_apush_vml(struct str_list *slp);

extern int _RP lis_clone_major(void);

#else
#ifdef _LIS_SOURCE
#warning "_LIS_SOURCE defined but not CONFIG_STREAMS_COMPAT_LIS"
#endif
#endif				/* CONFIG_STREAMS_COMPAT_LIS */

#endif				/* __SYS_LIS_STRCONF_H__ */
