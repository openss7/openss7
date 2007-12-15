/*****************************************************************************

 @(#) $RCSfile: streams.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2007/12/15 20:19:55 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2007/12/15 20:19:55 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: streams.c,v $
 Revision 0.9.2.11  2007/12/15 20:19:55  brian
 - updates

 Revision 0.9.2.10  2007/08/13 22:46:17  brian
 - GPLv3 header updates

 Revision 0.9.2.9  2006/07/24 09:01:17  brian
 - results of udp2 optimizations

 Revision 0.9.2.8  2006/06/22 13:11:39  brian
 - more optmization tweaks and fixes

 Revision 0.9.2.7  2006/02/20 10:59:21  brian
 - updated copyright headers on changed files

 Revision 0.9.2.6  2006/02/20 09:44:28  brian
 - gcc 4.0.2 does not like declared but unused static functions

 Revision 0.9.2.5  2005/12/19 12:45:16  brian
 - locking down for release

 Revision 0.9.2.4  2005/12/11 13:06:08  brian
 - corrected normal build error

 Revision 0.9.2.3  2005/12/09 18:01:43  brian
 - profiling copy

 Revision 0.9.2.2  2005/12/09 00:27:54  brian
 - updates to clock 95% on FC4

 Revision 0.9.2.1  2005/12/05 22:49:05  brian
 - some sneaky tricks for a single compilation unit build to help compiler
   with optimization
 - reworked plumbing read locks to be taken all the time (they are only for
   SMP anyway).
 - got 80% pipe performance @64-byes --with-k-optimize=speed on FC4.

 *****************************************************************************/

#ident "@(#) $RCSfile: streams.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2007/12/15 20:19:55 $"

static char const ident[] =
    "$RCSfile: streams.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2007/12/15 20:19:55 $";

/* can we just include these into one big compilation unit? */

#define BIG_COMPILE 1
#define BIG_STATIC STATIC
#define BIG_STATIC_INLINE STATIC streams_inline

#define BIG_COMPILE_STREAMS 1

#ifdef CONFIG_STREAMS_STH
#define BIG_STATIC_STH STATIC
#define BIG_STATIC_INLINE_STH STATIC streams_inline
#endif

#ifdef CONFIG_STREAMS_CLONE
#define BIG_STATIC_CLONE STATIC
#define BIG_STATIC_INLINE_CLONE STATIC streams_inline
#endif

#define __STRSCHD_EXTERN_INLINE	streams_inline streams_fastcall __unlikely
#define __STRUTIL_EXTERN_INLINE streams_inline streams_fastcall __unlikely

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
