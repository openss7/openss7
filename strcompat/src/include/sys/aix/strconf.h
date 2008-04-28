/*****************************************************************************

 @(#) $Id: strconf.h,v 0.9.2.14 2008-04-28 16:47:08 brian Exp $

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

 Last Modified $Date: 2008-04-28 16:47:08 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: strconf.h,v $
 Revision 0.9.2.14  2008-04-28 16:47:08  brian
 - updates for release

 Revision 0.9.2.13  2007/08/12 15:51:00  brian
 - header and extern updates, GPLv3, 3 new lock functions

 Revision 0.9.2.12  2006/12/08 05:08:04  brian
 - some rework resulting from testing and inspection

 Revision 0.9.2.11  2006/11/03 10:39:15  brian
 - updated headers, correction to mi_timer_expiry type

 *****************************************************************************/

#ifndef __SYS_AIX_STRCONF_H__
#define __SYS_AIX_STRCONF_H__

#ident "@(#) $RCSfile: strconf.h,v $ $Name:  $($Revision: 0.9.2.14 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

#ifndef __SYS_STRCONF_H__
#warning "Do not include sys/aix/strconf.h directly, include sys/strconf.h instead."
#endif

#ifndef __KERNEL__
#error "Do not use kernel headers for user space programs"
#endif				/* __KERNEL__ */

#ifndef __EXTERN_INLINE
#define __EXTERN_INLINE extern __inline__
#endif

#ifndef __AIX_EXTERN_INLINE
#define __AIX_EXTERN_INLINE __EXTERN_INLINE streamscall
#endif				/* __EXTERN_INLINE */

#ifndef __AIX_EXTERN
#define __AIX_EXTERN extern streamscall
#endif				/* __AIX_EXTERN_INLINE */

#ifndef _AIX_SOURCE
#warning "_AIX_SOURCE not defined but AIX strconf.h included"
#endif

#include <sys/strcompat/config.h>

#if defined(CONFIG_STREAMS_COMPAT_AIX) || defined(CONFIG_STREAMS_COMPAT_AIX_MODULE)

#ifndef dev_t
#define dev_t __streams_dev_t
#endif

typedef struct {
	char *sc_name;
	struct streamtab *sc_str;
	int sc_open_stylesc_flags;
	int sc_major;
	int sc_sqlevel;
	caddr_t sc_sqinfo;
} strconf_t;

__AIX_EXTERN int str_install_AIX(int cmd, strconf_t * conf);

#define STR_LOAD_DEV	1	/* add a device */
#define STR_UNLOAD_DEV	2	/* remove a device */
#define STR_LOAD_MOD	3	/* load a module */
#define STR_UNLOAD_MOD	4	/* unload a module */

#define STR_OLD_OPEN	0x00000000	/* SVR3 open semantics */
#define STR_NEW_OPEN	0x00000100	/* SVR4 open semantics */

#define STR_QSAFETY     0x00000200	/* Module needs safe callbacks */
#define STR_MPSAFE	0x00000400	/* Module is MP safe */
#define STR_PERSTREAM	0x00000800	/* Module has per-stream sync */
#define STR_Q_NOTTOSPEC	0x00001000	/* Module runs under process context */
#define STR_64BIT	0x00002000	/* 64-bit capable */
#define STR_NEWCLONING	0x00004000	/* Module does cloning without CLONEOPEN */

#ifndef str_install
#define str_install(__cmd, __conf) str_install_AIX(__cmd, __conf)
#endif

#else
#ifdef _AIX_SOURCE
#warning "_AIX_SOURCE defined but not CONFIG_STREAMS_COMPAT_AIX"
#endif
#endif

#endif				/* __SYS_AIX_STRCONF_H__ */
