/*****************************************************************************

 @(#) $Id: strconf.h,v 1.1.2.2 2010-11-28 14:21:50 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2010  Monavacon Limited <http://www.monavacon.com/>
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

 Last Modified $Date: 2010-11-28 14:21:50 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: strconf.h,v $
 Revision 1.1.2.2  2010-11-28 14:21:50  brian
 - remove #ident, protect _XOPEN_SOURCE

 Revision 1.1.2.1  2009-06-21 11:26:45  brian
 - added files to new distro

 *****************************************************************************/

#ifndef __SYS_AIX_STRCONF_H__
#define __SYS_AIX_STRCONF_H__

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

#include <sys/openss7/config.h>

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
