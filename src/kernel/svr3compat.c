/*****************************************************************************

 @(#) $RCSfile: svr3compat.c,v $ $Name:  $($Revision: 1.1.2.3 $) $Date: 2011-09-02 08:46:51 $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2011  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

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

 Last Modified $Date: 2011-09-02 08:46:51 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: svr3compat.c,v $
 Revision 1.1.2.3  2011-09-02 08:46:51  brian
 - sync up lots of repo and build changes from git

 Revision 1.1.2.2  2010-11-28 14:21:57  brian
 - remove #ident, protect _XOPEN_SOURCE

 Revision 1.1.2.1  2009-06-21 11:37:17  brian
 - added files to new distro

 *****************************************************************************/

static char const ident[] = "$RCSfile: svr3compat.c,v $ $Name:  $($Revision: 1.1.2.3 $) $Date: 2011-09-02 08:46:51 $";

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

#define __SVR3_EXTERN_INLINE __inline__ streamscall __unlikely
#define __SVR3_EXTERN streamscall

#define _SVR3_SOURCE

#include "sys/os7/compat.h"

#define SVR3COMP_DESCRIP	"UNIX(R) SVR 3.2 Compatibility for Linux Fast-STREAMS"
#define SVR3COMP_EXTRA		"Part of UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define SVR3COMP_COPYRIGHT	"Copyright (c) 2008-2011  Monavacon Limited.  All Rights Reserved."
#define SVR3COMP_REVISION	"LfS $RCSfile: svr3compat.c,v $ $Name:  $($Revision: 1.1.2.3 $) $Date: 2011-09-02 08:46:51 $"
#define SVR3COMP_DEVICE		"UNIX(R) SVR 3.2 Compatibility"
#define SVR3COMP_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SVR3COMP_LICENSE	"GPL"
#define SVR3COMP_BANNER		SVR3COMP_DESCRIP	"\n" \
				SVR3COMP_EXTRA		"\n" \
				SVR3COMP_COPYRIGHT	"\n" \
				SVR3COMP_REVISION	"\n" \
				SVR3COMP_DEVICE		"\n" \
				SVR3COMP_CONTACT	"\n"
#define SVR3COMP_SPLASH		SVR3COMP_DEVICE		" - " \
				SVR3COMP_REVISION	"\n"

#ifdef CONFIG_STREAMS_COMPAT_SVR3_MODULE
MODULE_AUTHOR(SVR3COMP_CONTACT);
MODULE_DESCRIPTION(SVR3COMP_DESCRIP);
MODULE_SUPPORTED_DEVICE(SVR3COMP_DEVICE);
MODULE_LICENSE(SVR3COMP_LICENSE);
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-svr3compat");
#endif
#ifdef MODULE_VERSION
MODULE_VERSION(PACKAGE_ENVR);
#endif
#endif

__SVR3_EXTERN_INLINE major_t emajor(dev_t dev);

EXPORT_SYMBOL(emajor);		/* uw7/ddi.h */
__SVR3_EXTERN_INLINE minor_t eminor(dev_t dev);

EXPORT_SYMBOL(eminor);		/* uw7/ddi.h */

__SVR3_EXTERN_INLINE mblk_t *alloc_proto(size_t psize, size_t bsize, int type, uint bpri);

EXPORT_SYMBOL(alloc_proto);	/* svr3/ddi.h */

#ifdef CONFIG_STREAMS_COMPAT_SVR3_MODULE
static
#endif
int __init
svr3comp_init(void)
{
#ifdef CONFIG_STREAMS_COMPAT_SVR3_MODULE
	printk(KERN_INFO SVR3COMP_BANNER);
#else
	printk(KERN_INFO SVR3COMP_SPLASH);
#endif
	return (0);
}

#ifdef CONFIG_STREAMS_COMPAT_SVR3_MODULE
static
#endif
void __exit
svr3comp_exit(void)
{
	return;
}

#ifdef CONFIG_STREAMS_COMPAT_SVR3_MODULE
module_init(svr3comp_init);
module_exit(svr3comp_exit);
#endif
