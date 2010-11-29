/*****************************************************************************

 @(#) $Id: strconf.h,v 1.1.2.2 2010-11-28 14:21:53 brian Exp $

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

 Last Modified $Date: 2010-11-28 14:21:53 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: strconf.h,v $
 Revision 1.1.2.2  2010-11-28 14:21:53  brian
 - remove #ident, protect _XOPEN_SOURCE

 Revision 1.1.2.1  2009-06-21 11:26:49  brian
 - added files to new distro

 *****************************************************************************/

#ifndef __SYS_OSF_STRCONF_H__
#define __SYS_OSF_STRCONF_H__

#ifndef __SYS_STRCONF_H__
#warning "Do not include sys/aix/strconf.h directly, include sys/strconf.h instead."
#endif

#ifndef __KERNEL__
#error "Do not use kernel headers for user space programs"
#endif				/* __KERNEL__ */

#ifndef __EXTERN_INLINE
#define __EXTERN_INLINE extern __inline__
#endif

#ifndef __OSF_EXTERN_INLINE
#define __OSF_EXTERN_INLINE __EXTERN_INLINE streamscall
#endif				/* __OSF_EXTERN_INLINE */

#ifndef __OSF_EXTERN
#define __OSF_EXTERN extern streamscall
#endif

#ifndef _OSF_SOURCE
#warning "_OSF_SOURCE not defined but OSF strconf.h included"
#endif

#include <sys/openss7/config.h>

#if defined(CONFIG_STREAMS_COMPAT_OSF) || defined(CONFIG_STREAMS_COMPAT_OSF_MODULE)

#ifndef dev_t
#define dev_t __streams_dev_t
#endif

#if 0				/* we don't support the static configuration */

#define OSF_STREAMS_CONFIG_10   0x04026019
#define OSF_STREAMS_CONFIG_11   0x0503611A

typedef struct str_config {
	uint sc_version;
	uint sc_sa_flags;
	char sc_sa_name[FMNAMESZ + 1];
	dev_t sc_devnum;
} str_config_t;

#endif

#define OSF_STREAMS_10  0x04026019	/* OSF/1.0 */
#define OSF_STREAMS_11  0x0503611B	/* OSF/1.1 */

struct streamadm {
	uint sa_version;
	uint sa_flags;
	char sa_name[FMNAMESZ + 1];
	caddr_t sa_ttys;
	uint sa_sync_level;
	caddr_t sa_sync_info;
};

__OSF_EXTERN dev_t strmod_add(dev_t dev, struct streamtab *st, struct streamadm *sa);
__OSF_EXTERN int strmod_del(dev_t dev, struct streamtab *st, struct streamadm *sa);

#else
#ifdef _OSF_SOURCE
#warning "_OSF_SOURCE defined but not CONFIG_STREAMS_COMPAT_OSF"
#endif
#endif

#endif				/* __SYS_OSF_STRCONF_H__ */
