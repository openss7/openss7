/*****************************************************************************

 @(#) $RCSfile: streams.c,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:37:16 $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
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

 Last Modified $Date: 2009-06-21 11:37:16 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: streams.c,v $
 Revision 1.1.2.1  2009-06-21 11:37:16  brian
 - added files to new distro

 *****************************************************************************/

#ident "@(#) $RCSfile: streams.c,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:37:16 $"

static char const ident[] = "$RCSfile: streams.c,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:37:16 $";

/* can we just include these into one big compilation unit? */

#define BIG_COMPILE 1
#define BIG_STATIC STATIC
#define BIG_STATIC_INLINE STATIC streams_inline

#define BIG_COMPILE_STREAMS 1

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
