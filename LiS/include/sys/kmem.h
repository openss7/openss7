/*
 * This file provides some prototypes and defines for SVR4 compatibility
 *
 * Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Cambridge,
 * MA 02139, USA.
 * 
 */


#ident "@(#) LiS kmem.h 2.2 4/2/01 19:31:34 "

#ifdef __KERNEL__

#ifndef _STRPORT_H
#include <sys/strport.h>
#endif

#ifdef QNX
void * port_sbrk(int siz);
void   port_tok_free(char *ptr, int siz);

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
#define KM_SLEEP        0       /* can block for memory; success guaranteed */
#define KM_NOSLEEP      1       /* cannot block for memory; may fail */


#endif				/* __KERNEL__ */
