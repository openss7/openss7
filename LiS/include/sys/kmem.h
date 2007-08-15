/*****************************************************************************

 @(#) $Id: kmem.h,v 1.1.1.1.12.4 2007/08/14 10:47:08 brian Exp $

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

 Last Modified $Date: 2007/08/14 10:47:08 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: kmem.h,v $
 Revision 1.1.1.1.12.4  2007/08/14 10:47:08  brian
 - GPLv3 header update

 *****************************************************************************/

/*
 * This file provides some prototypes and defines for SVR4 compatibility
 *
 * Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>
 *
 */

#ifndef __SYS_KMEM_H__
#define __SYS_KMEM_H__

#ident "@(#) $RCSfile: kmem.h,v $ $Name:  $($Revision: 1.1.1.1.12.4 $) $Date: 2007/08/14 10:47:08 $"

#ifdef __KERNEL__

#ifndef _STRPORT_H
#include <sys/strport.h>
#endif

#if 0
void *port_sbrk(int siz);
void port_tok_free(char *ptr, int siz);

#define	kmem_alloc(siz,wait_code)	port_sbrk((siz))
#define	kmem_free(ptr,siz)		port_tok_free((ptr), (siz))
#endif

#define	kmem_alloc(siz,wait_code)	ALLOC(siz)
#define	kmem_zalloc(siz,wait_code)	ZALLOC(siz)
#define	kmem_free(ptr,siz)		FREE(ptr)

/*
 * Wait codes.  These are not used but are included for compatibility
 * with SVR4.
 */
#define KM_SLEEP        0	/* can block for memory; success guaranteed */
#define KM_NOSLEEP      1	/* cannot block for memory; may fail */

#endif				/* __KERNEL__ */

#endif				/* __SYS_KMEM_H__ */
