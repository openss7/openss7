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

#ifndef __ISODE_QUIPU_UTIL_H__
#define __ISODE_QUIPU_UTIL_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* util.h - various useful utility definitions */

/*
 * Header: /xtel/isode/isode/h/quipu/RCS/util.h,v 9.0 1992/06/16 12:23:11 isode Rel
 *
 *
 * Log: util.h,v
 * Revision 9.0  1992/06/16  12:23:11  isode
 * Release 8.0
 *
 */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */

#ifndef _DIDUTIL_

#define _DIDUTIL_

#ifndef _H_UTIL			/* PP interwork */

#include <errno.h>
#include <stdio.h>		/* minus the ctype stuff */
#include <ctype.h>
#include <setjmp.h>
#include "manifest.h"
#include "logger.h"

#endif

#ifndef       _GENERAL_
#include "general.h"
#endif

#include "quipu/config.h"

#ifndef _H_UTIL			/* PP interwork */

/* some common logical values */

#ifndef TRUE
#define TRUE    1
#endif
#ifndef FALSE
#define FALSE   0
#endif
#ifndef MAYBE
#define MAYBE   1
#endif

/* stdio extensions */

#ifndef lowtoup
#define lowtoup(chr) (islower(chr)?toupper(chr):chr)
#endif
#ifndef uptolow
#define uptolow(chr) (isupper(chr)?tolower(chr):chr)
#endif
#ifndef MIN
#define MIN(a,b) (( (b) < (a) ) ? (b) : (a) )
#endif
#ifndef MAX
#define MAX(a,b) (( (b) > (a) ) ? (b) : (a) )
#endif
#if	!defined(MAXINT) && !defined(__linux__)
#define MAXINT (~(1 << ((sizeof(int) * 8) - 1)))
#endif

#define isstr(ptr) ((ptr) != 0 && *(ptr) != '\0')
#define isnull(chr) ((chr) == '\0')
#define isnumber(c) ((c) >= '0' && (c) <= '9')

/*
 * provide a timeout facility
 */

extern jmp_buf _timeobuf;

#define quipu_timeout(val)    (setjmp(_timeobuf) ? 1 : (_timeout(val), 0))

/*
 * some common extensions
 */
#define LINESIZE 1024		/* what we are prepared to consider a line length */
#define FILNSIZE 256		/* max filename length */
#define LOTS    1024		/* a max sort of thing */
#define MAXFILENAMELEN 15	/* size of largest fine name allowed */

# define        MAXFORK 10	/* no. of times to try a fork() */

#define _H_UTIL

#endif
#endif

#endif				/* __ISODE_QUIPU_UTIL_H__ */
