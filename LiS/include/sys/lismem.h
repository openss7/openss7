/*****************************************************************************

 @(#) $Id: lismem.h,v 1.1.1.2.4.4 2005/12/18 05:41:24 brian Exp $

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

 Last Modified $Date: 2005/12/18 05:41:24 $ by $Author: brian $

 *****************************************************************************/

/************************************************************************
*                        LiS Memory Interface                           *
*************************************************************************
*									*
* This is an interface to the kernel's memory allocation routines.	*
* The idea is that STREAMS drivers that use these routines to allocate	*
* memory can be insulated in changes to the parameters in the kernel's	*
* allocation routines.  These parameters DID change from the 2.2 kernel	*
* to the 2.4 kernel.							*
*									*
*    Copyright (C) 2000  David Grothe, Gcom, Inc <dave@gcom.com>	*
*									*
************************************************************************/

#ifndef SYS_LISMEM_H
#define SYS_LISMEM_H	1

#ident "@(#) $RCSfile: lismem.h,v $ $Name:  $($Revision: 1.1.1.2.4.4 $) $Date: 2005/12/18 05:41:24 $"

#include <sys/LiS/genconf.h>

/*
 * These are the functions that to the work.  Do not call them
 * directly unless you are cascading file and line number information
 * from your own interface routines.
 *
 * Note that lis_free_mem returns a void *.  This is always a NULL pointer
 * value for the convenience of constructs such as ptr = lis_free_mem(ptr).
 * Also, the mem_area argument to lis_free_mem can be NULL.
 */
void *_RP lis_alloc_atomic_fcn(int nbytes, char *file, int line);
void *_RP lis_alloc_kernel_fcn(int nbytes, char *file, int line);
void *_RP lis_alloc_dma_fcn(int nbytes, char *file, int line);
void *_RP lis_free_mem_fcn(void *mem_area, char *file, int line);
void *_RP lis_get_free_pages_fcn(int nbytes, int class, char *file, int line);
void *_RP lis_get_free_pages_atomic_fcn(int nbytes, char *file, int line);
void *_RP lis_get_free_pages_kernel_fcn(int nbytes, char *file, int line);
void *_RP lis_free_pages_fcn(void *ptr, char *file, int line);

#ifdef __LIS_INTERNAL__
void lis_free_all_pages(void);		/* internal cleanup routine */
#endif

/*
 * These are the simplified routines that the STREAMS drivers call.
 * We avoid using the values of the kernel's memory class parameters
 * at the calling site so that only LiS needs to be compiled in the
 * context of the new kernel.
 */
#define	lis_alloc_atomic(n)	lis_alloc_atomic_fcn((n), __FILE__, __LINE__)
#define	lis_alloc_kernel(n)	lis_alloc_kernel_fcn((n), __FILE__, __LINE__)
#define	lis_alloc_dma(n)	lis_alloc_dma_fcn((n), __FILE__, __LINE__)
#define	lis_free_mem(p)		lis_free_mem_fcn((p), __FILE__, __LINE__)
#define	lis_get_free_pages(n)	lis_get_free_pages_kernel_fcn((n), \
							__FILE__, __LINE__)
#define	lis_get_free_pages_kernel(n)	lis_get_free_pages_kernel_fcn((n), \
							__FILE__, __LINE__)
#define	lis_get_free_pages_atomic(n)	lis_get_free_pages_atomic_fcn((n), \
							__FILE__, __LINE__)
#define	lis_free_pages(p)	lis_free_pages_fcn((p), __FILE__, __LINE__)

#endif				/* SYS_LISPCI_H from top of file */
