/*****************************************************************************

 @(#) $RCSfile: streams.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2005/12/09 00:27:54 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com/>
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

 Last Modified $Date: 2005/12/09 00:27:54 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: streams.c,v $
 Revision 0.9.2.2  2005/12/09 00:27:54  brian
 - updates to clock 95% on FC4

 Revision 0.9.2.1  2005/12/05 22:49:05  brian
 - some sneaky tricks for a single compilation unit build to help compiler
   with optimization
 - reworked plumbing read locks to be taken all the time (they are only for
   SMP anyway).
 - got 80% pipe performance @64-byes --with-k-optimize=speed on FC4.

 *****************************************************************************/

#ident "@(#) $RCSfile: streams.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2005/12/09 00:27:54 $"

static char const ident[] = "$RCSfile: streams.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2005/12/09 00:27:54 $";

/* can we just include these into one big compilation unit? */

#define BIG_COMPILE 1
#define BIG_STATIC static
#define BIG_STATIC_INLINE static streams_inline

#ifdef CONFIG_STREAMS_STH
#define BIG_STATIC_STH static
#define BIG_STATIC_INLINE_STH static streams_inline
#else
#define BIG_STATIC_STH
#define BIG_STATIC_INLINE_STH
#endif

#ifdef CONFIG_STREAMS_CLONE
#define BIG_STATIC_CLONE static
#define BIG_STATIC_INLINE_CLONE static streams_inline
#else
#define BIG_STATIC_CLONE
#define BIG_STATIC_INLINE_CLONE
#endif

#define __STRSCHD_EXTERN_INLINE inline
#define __STRUTIL_EXTERN_INLINE inline

#undef ident
#define ident ident_strreg
#include "strreg.c"
#undef ident
#define ident ident_strsysctl
#include "strsysctl.c"
#undef ident
#define ident ident_strsched
#include "strsched.c"
#undef ident
#define ident ident_strutil
#include "strutil.c"
#ifdef CONFIG_STREAMS_STH
#undef ident
#define ident ident_sth
#include "src/modules/sth.c"
#endif
#ifdef CONFIG_STREAMS_CLONE
#undef ident
#define ident ident_clone
#include "src/drivers/clone.c"
#endif
#undef ident
#define ident ident_strsad
#include "strsad.c"
#undef ident
#define ident ident_strprocfs
#include "strprocfs.c"
#undef ident
#define ident ident_strsyms
#include "strsyms.c"
#undef ident
#define ident ident_strmain
#include "strmain.c"
