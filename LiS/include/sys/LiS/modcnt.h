/*****************************************************************************

 @(#) $Id$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ifndef LIS_MODCNT_H
#define LIS_MODCNT_H	1

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

/*
 *  This file is intended to address the different situations for which
 *  MODGET/MODPUT (i.e., increment/decrement loadable kernel module use
 *  reference counts) might be needed.
 *
 *  There are basically these kinds of C source files that might have a
 *  need:
 *  - head/linux-mdep.c - includes <linux/module.h> for the head, and
 *    thus effectively defines streams.o as a module.
 *  - head/head.c - doesn't include <linux/module.h>, but will be linked
 *    into the same streams.o object as linux-mdep.c
 *  - drivers/modules also to be linked into streams.o, and will thus not
 *    include <linux/module.h>
 *  - drivers/modules to be separate loadable modules, which will include
 *    <linux/module.h> for themselves
 *
 *  I have decided that the development versions of these routines are not
 *  easily workable.  They had to be defined in linux-mdep.c as callables,
 *  but as such, also had to be inter-module calls, whose names were
 *  exported symbols.  This is somewhat circular, considering that these
 *  routines concern the loading of modules in the first place.  Second,
 *  only the parameter-less MOD_[INC,DEC]_USE_COUNT macros were used, which
 *  makes no sense for counting references from an entirely other module.
 *  So, the development mode routines have been dropped, at least for
 *  now; they may be added back in later.  (JB 05/18/04)
 *
 *  In production mode, separate loadable modules will use the inline
 *  routines here.  Sources like head.c which are to be included in
 *  streams.o without themselves including <linux/module.h> will not be
 *  able to define such inlines, so they will call the ..._local routines,
 *  which linux-mdep.c will also define.  These need not be exported
 *  symbols, since the calls will be within streams.o.
 */

#if defined(_LINUX_MODULE_H)

#ifndef	_STRMDBG_H
#include <sys/LiS/strmdbg.h>	/* for debug bits */
#endif

/* local inlines for separate loadables that have included <linux/module.h> -
 * such sources are self-contained on this issue
 */
static inline void
lis_modget_local(const char *file, int line, const char *fn)
{
#if defined(LINUX) && defined(__KERNEL__) && defined(MODULE)
	if (LIS_DEBUG_REFCNTS)
		printk("lis_modget_local() <\"%s\">++ {%s@%d,%s()}\n", (THIS_MODULE)->name, file,
		       line, fn);

#if defined(KERNEL_2_5)
	try_module_get(THIS_MODULE);
#else
	MOD_INC_USE_COUNT;
#endif
#endif
}

static inline void
lis_modput_local(const char *file, int line, const char *fn)
{
#if defined(LINUX) && defined(__KERNEL__) && defined(MODULE)
	if (LIS_DEBUG_REFCNTS)
		printk("lis_modput_local() <\"%s\">-- {%s@%d,%s()}\n", (THIS_MODULE)->name, file,
		       line, fn);
#if defined(KERNEL_2_5)
	module_put(THIS_MODULE);
#else
	MOD_DEC_USE_COUNT;
#endif
#endif
}

#define MODGET()       lis_modget_local(__FILE__,__LINE__,__FUNCTION__)
#define MODPUT()       lis_modput_local(__FILE__,__LINE__,__FUNCTION__)

#else

/*
 * If not a separate module then use the internal LiS modget/put via
 * the debug versions.  These reside in linux-mdep.c.
 */
#define MODGET()       lis_modget_dbg(__LIS_FILE__,__LINE__,__FUNCTION__)
#define MODPUT()       lis_modput_dbg(__LIS_FILE__,__LINE__,__FUNCTION__)

#endif				/* !_LINUX_MODULE_H */

/* intra-module callables for production mode, for sources not including
 * <linux/module.h>.  these are defined in linux-mdep.c, and only wrap
 * the above ..._local inlines, as a convenience so that <linux/module.h>
 * need not be included to do module use reference counting
 */
#ifdef __LIS_INTERNAL__
extern void lis_modget_dbg(const char *file, int line, const char *fn);
extern void lis_modput_dbg(const char *file, int line, const char *fn);
#endif

#define LIS_MODGET()       lis_modget_dbg(__LIS_FILE__,__LINE__,__FUNCTION__)
#define LIS_MODPUT()       lis_modput_dbg(__LIS_FILE__,__LINE__,__FUNCTION__)

#endif				/* LIS_MODCNT_H */
