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
* This library is free software; you can redistribute it and/or		*
* modify it under the terms of the GNU Library General Public		*
* License as published by the Free Software Foundation; either		*
* version 2 of the License, or (at your option) any later version.	*
* 									*
* This library is distributed in the hope that it will be useful,	*
* but WITHOUT ANY WARRANTY; without even the implied warranty of	*
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU	*
* Library General Public License for more details.			*
* 									*
* You should have received a copy of the GNU Library General Public	*
* License along with this library; if not, write to the			*
* Free Software Foundation, Inc., 59 Temple Place - Suite 330,		*
* Cambridge, MA 02139, USA.						*
*									*
************************************************************************/

#ifndef SYS_LISMEM_H
#define SYS_LISMEM_H	1

#ident "@(#) LiS lismem.h 1.5 08/24/04"

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
void	*lis_alloc_atomic_fcn(int nbytes, char *file, int line) _RP;
void	*lis_alloc_kernel_fcn(int nbytes, char *file, int line) _RP;
void	*lis_alloc_dma_fcn(int nbytes, char *file, int line) _RP;
void	*lis_free_mem_fcn(void *mem_area, char *file, int line) _RP;
void	*lis_get_free_pages_fcn(int nbytes, int class, char *file, int line)_RP;
void	*lis_get_free_pages_atomic_fcn(int nbytes, char *file, int line) _RP;
void	*lis_get_free_pages_kernel_fcn(int nbytes, char *file, int line) _RP;
void	*lis_free_pages_fcn(void *ptr, char *file, int line) _RP;
void	 lis_free_all_pages(void) _RP;	/* internal cleanup routine */

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
