/*****************************************************************************

 @(#) $Id: $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>

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

 Last Modified $Date: $ by $Author: $

 *****************************************************************************/

/*
 * This file provides some prototypes and defines for SVR4 compatibility
 *
 * Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>
 *
 */

#ifndef __SYS_KMEM_H__
#define __SYS_KMEM_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

#ifdef __KERNEL__

#ifndef _STRPORT_H
#include <sys/strport.h>
#endif

#ifdef QNX
void *port_sbrk(int siz);
void port_tok_free(char *ptr, int siz);

#define	kmem_alloc(siz,wait_code)	port_sbrk((siz))
#define	kmem_free(ptr,siz)		port_tok_free((ptr), (siz))
#else
#define	kmem_alloc(siz,wait_code)	ALLOC(siz)
#define	kmem_zalloc(siz,wait_code)	ZALLOC(siz)
#define	kmem_free(ptr,siz)		FREE(ptr)
#endif

/*
 * Wait codes.  These are not used but are included for compatibility
 * with SVR4.
 */
#define KM_SLEEP        0	/* can block for memory; success guaranteed */
#define KM_NOSLEEP      1	/* cannot block for memory; may fail */

#endif				/* __KERNEL__ */

#endif				/* __SYS_KMEM_H__ */
