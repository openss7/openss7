/************************************************************************
*                        LiS Memory Interface                           *
*************************************************************************
*									*
* This interface was created to insulate the Linux STREAMS programmer	*
* from the constantly changing memory allocation interface provided by	*
* the Linux kernel.							*
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

#ident "@(#) LiS lismem.c 1.17 09/07/04"

#include <sys/stream.h>		/* gets all the right LiS stuff included */
#include <sys/lismem.h>		/* LiS mem header file */
#include <sys/osif.h>		/* LiS kernel interface */
#include <sys/lislocks.h>	/* for spin locks */

#ifdef POISON_MEM
#define CACHE_OPTS	(SLAB_POISON | SLAB_RED_ZONE)
#else
#define CACHE_OPTS	0
#endif

/************************************************************************
*                         Cache Allocation                              *
*************************************************************************
*									*
* These sizes are chosen to accomodate stream head structures, mblks	*
* and queue structures.							*
*									*
************************************************************************/


typedef struct
{
    const char		 *name ;
    kmem_cache_t	 *cache_struct ;
    int			  size ;
    int			  kflags ;

} lis_slab_table_t ;

#define	SIZE1		200		/* numeric form */
#define	SIZE2		512
#define SIZE3		1500
#define SIZE_MAX	SIZE3
#define	SIZE1S		"200"		/* string form */
#define	SIZE2S		"512"
#define SIZE3S		"1500"

lis_slab_table_t lis_slab_table[] =
{
    {"LiS-Atomic-"SIZE1S, NULL, SIZE1, GFP_ATOMIC},
    {"LiS-Atomic-"SIZE2S, NULL, SIZE2, GFP_ATOMIC},
    {"LiS-Atomic-"SIZE3S, NULL, SIZE3, GFP_ATOMIC},

    {"LiS-Kernel-"SIZE1S, NULL, SIZE1, GFP_KERNEL},
    {"LiS-Kernel-"SIZE2S, NULL, SIZE2, GFP_KERNEL},
    {"LiS-Kernel-"SIZE3S, NULL, SIZE3, GFP_KERNEL},

    {"LiS-DMA-"SIZE1S,    NULL, SIZE1, GFP_DMA},
    {"LiS-DMA-"SIZE2S,    NULL, SIZE2, GFP_DMA},
    {"LiS-DMA-"SIZE3S,    NULL, SIZE3, GFP_DMA},

    {NULL,                NULL, 0,     0}
} ;


/************************************************************************
*                         lismem_init                                   *
*************************************************************************
*									*
* Self-initialization for memory functions.				*
*									*
************************************************************************/

#define chk_init	if (!lismem_init_flag) lis_mem_init()

static int  		lismem_init_flag ;
static lis_spin_lock_t	contig_lock ;		/* protect the table */

void lis_mem_init(void)
{
    lis_spin_lock_init(&contig_lock, "LiS-Page-Alloc");

    {
	lis_slab_table_t	*p ;

	for (p = lis_slab_table; p->name != NULL; p++)
	{
	    p->cache_struct = kmem_cache_create(p->name, p->size, 0,
				SLAB_HWCACHE_ALIGN | CACHE_OPTS, NULL,NULL);
	}
    }

    lismem_init_flag = 1 ;
}

void lis_mem_terminate(void)
{
    lis_slab_table_t	*p ;

    for (p = lis_slab_table; p->name != NULL; p++)
    {
	if (p->cache_struct == NULL) continue ;
	kmem_cache_destroy(p->cache_struct) ;
	p->cache_struct = NULL ;
    }
}

/************************************************************************
*                          Page Allocation                              *
*************************************************************************
*									*
* Page allocation is the way to ensure that you get memory that		*
* occupies physically contiguous locations.  We offer a simplified	*
* interface to the kernel's page allocator here.  The kernel's page	*
* allocation routines are awkward to use in that you have to pass	*
* the number of pages to de-allocate to their "free pages" routine.	*
*									*
* We keep a separate table of allocated pages.  This allows us to return*
* the entire page to the caller.					*
*									*
************************************************************************/

typedef struct contig_memlink
{				/* 32 bytes long */
    struct contig_memlink *link;/* to next entry */
    void	*page_addr ;	/* addr of allocated page */
    int		 size ;		/* original size in bytes */
    int		 order ;	/* "order" param for freeing */
    char	*file ;		/* who allocated */
    int		 line ;
    int		 boundary ;	/* page boundary */

} contig_memlink_t ;

static contig_memlink_t	*contig_mem_head ;
static unsigned long	contig_bytes ;		/* total allocated */

static int	alloc_page_tbl(void)
{
    contig_memlink_t	*pg ;
    contig_memlink_t	*p ;
    int			 nentries ;

    pg = (contig_memlink_t *) __get_free_page(GFP_KERNEL) ;
    if (pg == NULL) 
	return(-1) ;

    memset(pg, 0, PAGE_SIZE) ;
    for (p = pg, nentries = PAGE_SIZE/sizeof(*p); nentries > 0; nentries--, p++)
    {
	p->link = p + 1 ;
    }

    /*
     * Link the last entry in this page to the head entry of the previous page.
     * Then make this page the new head of the list.
     */
    pg->boundary = 1 ;			/* 1st entry is on page boundary */
    lis_spin_lock(&contig_lock) ;
    (p-1)->link = contig_mem_head ;	/* last entry in page */
    contig_mem_head = pg ;
    lis_spin_unlock(&contig_lock) ;
    return(0) ;
}

static contig_memlink_t *find_entry(void *addr)
{
    contig_memlink_t	*mp ;

    for (mp = contig_mem_head; mp != NULL; mp = mp->link)
    {
	if (mp->page_addr == addr) return(mp) ;
    }

    return(NULL) ;			/* could not find it */
}

void    * _RP lis_get_free_pages_fcn(int nbytes, int class, char *file, int line)
{
    void		*a ;
    int			 order ;
    int			 bytes ;
    contig_memlink_t	*mp ;

    chk_init ;

    for (order = 0; (bytes = (1 << order) * PAGE_SIZE) < nbytes; order++) ;

    lis_spin_lock(&contig_lock) ;
    mp = find_entry(NULL) ;		/* find available entry */
    if (mp == NULL)
    {
	lis_spin_unlock(&contig_lock) ;
	if (alloc_page_tbl() < 0)
	    return(NULL) ;		/* can't track allocated page */

	lis_spin_lock(&contig_lock) ;
	mp = find_entry(NULL) ;		/* should be one now */
	if (mp == NULL)			/* busted code */
	{
	    lis_spin_unlock(&contig_lock) ;
	    printk("lis_get_free_pages: called from %s #%d: "
		    "bug in allocation table code\n",
		    file, line) ;
	    return(NULL) ;
	}
    }

    mp->page_addr = (void *) 0xffffffff ;/* guard from find_entry */
    lis_spin_unlock(&contig_lock) ;

    a =  (void *) __get_free_pages(class, order) ;	/* kernel routine */
    if (a == NULL)
    {
	printk("lis_get_free_pages: called from %s #%d: "
		"nbytes=%d order=%d total=%lu allocation failed\n",
		file, line,
		nbytes, order, contig_bytes) ;
	return(NULL) ;
    }

    contig_bytes += bytes ;		/* # bytes allocated */

    mp->size         = nbytes ;
    mp->order        = order ;
    mp->file         = file ;
    mp->line         = line ;
    mp->page_addr    = a ;

    return(a) ;

} /* lis_get_free_pages_fcn */

void    * _RP lis_get_free_pages_atomic_fcn(int nbytes, char *file, int line)
{
    return(lis_get_free_pages_fcn(nbytes, GFP_ATOMIC, file, line)) ;
}

void    * _RP lis_get_free_pages_kernel_fcn(int nbytes, char *file, int line)
{
    return(lis_get_free_pages_fcn(nbytes, GFP_KERNEL, file, line)) ;
}

void     * _RP lis_free_pages_fcn(void *ptr, char *file, int line)
{
    contig_memlink_t	*mp ;
    void		*addr ;
    int			 order ;

    chk_init ;

    if (ptr == NULL)
	return(NULL) ;

    lis_spin_lock(&contig_lock) ;
    mp = find_entry(ptr) ;
    if (mp == NULL)			/* have no record of this page */
    {
	printk("lis_free_pages: called from %s #%d: "
	       "cannot find page info for address 0x%lx\n",
	       file, line, (long) ptr) ;
	lis_spin_unlock(&contig_lock) ;
	return(NULL) ;
    }

    addr = mp->page_addr ;
    order = mp->order ;
    contig_bytes -= (1 << mp->order) * PAGE_SIZE ;
    mp->page_addr = NULL ;

    lis_spin_unlock(&contig_lock) ;

    free_pages((unsigned long) addr, order) ; /* kernel routine */

    return(NULL) ;

} /* lis_free_pages_fcn */

void	 _RP lis_free_all_pages(void)
{
    contig_memlink_t	*mp ;
    contig_memlink_t	*boundary_ptr = NULL ;
    volatile static int  freeing ;

    chk_init ;

    lis_spin_lock(&contig_lock) ;
    if (!freeing && contig_bytes != 0)
    {
	freeing = 1 ;			/* only one gets to do this */
	lis_spin_unlock(&contig_lock) ;

	/*
	 * Free the data pages allocated to callers.
	 */
	for (mp = contig_mem_head; mp != NULL; mp = mp->link)
	{
	    if (mp->page_addr == NULL || mp->page_addr == (void *)0xffffffff)
		continue ;

	    if (LIS_DEBUG_MEM_LEAK)
		printk("lis_free_all_pages: "
			  "Memory at %lx allocated from %s #%d, "
			  "%d bytes leaked (recovered)\n",
			  (long) mp->page_addr, mp->file, mp->line, mp->size) ;
	    lis_free_pages(mp->page_addr) ;
	}

	/*
	 * Free the pages allocated for keeping track of other pages.
	 */
	for (mp = contig_mem_head; mp != NULL; mp = mp->link)
	{
	    if (!mp->boundary) continue ;

	    /* At a page boundary, remember it for freeing */
	    if (boundary_ptr != NULL)
		free_page((unsigned long) boundary_ptr) ;

	    boundary_ptr = mp ;		/* remember for next time */
	}

	if (boundary_ptr != NULL)	/* catch last page */
	    free_page((unsigned long) boundary_ptr) ;

	contig_mem_head = NULL ;
	freeing = 0 ;
    }
    else
	lis_spin_unlock(&contig_lock) ;

} /* lis_free_all_pages */

/************************************************************************
*                         Kernel Allocators                             *
*************************************************************************
*									*
* lis_kmalloc/lis_kfree are called from lis_malloc in order to actually	*
* obtain the memory from the kernel.  We do this in either of two ways,	*
* either from an lis memory cache or from the general kmalloc.		*
*									*
************************************************************************/

typedef struct
{
    kmem_cache_t	*slab_ptr ;

} mem_hdr_t ;

typedef struct
{
    mem_hdr_t		 mh ;
    char		 padding[LIS_CACHE_BYTES -
	                                (sizeof(mem_hdr_t) % LIS_CACHE_BYTES)];
} mem_hdr_space_t ;

void *lis__kmalloc(int nbytes, int class, int use_cache)
{
    mem_hdr_space_t  	*p ;
    lis_slab_table_t	*tp ;
    kmem_cache_t	*cp = NULL ;

    nbytes += sizeof(mem_hdr_space_t) ;

    if (!use_cache || nbytes > SIZE_MAX)
	p = kmalloc(nbytes, class) ;
    else
    {				/* find appropriate slab */
	for (tp = lis_slab_table; tp->name != NULL; tp++)
	{
	    if (   tp->kflags == class
		&& tp->cache_struct != NULL
		&& nbytes <= tp->size
	       )
		break ;
	}

	if (tp->cache_struct == NULL) return(NULL) ;

	p = kmem_cache_alloc(cp = tp->cache_struct, class) ;
    }

    if (p == NULL) return(NULL) ;

    p->mh.slab_ptr = cp ;
    return((void *)(p+1)) ;
}

void *lis__kfree(void *ptr)
{
    mem_hdr_space_t	 *p ;

    if (ptr == NULL) return(NULL) ;

    p = (mem_hdr_space_t *) ptr ;
    p-- ;

    if (p->mh.slab_ptr == NULL)
	kfree(p) ;
    else
	kmem_cache_free(p->mh.slab_ptr, p) ;

    return(NULL) ;
}


/************************************************************************
*                       Calls to Kernel Routines                        *
*************************************************************************
*									*
* We are really using the LiS memory allocator here so that we can	*
* track file and line number information.				*
*									*
************************************************************************/

void	* _RP lis_alloc_atomic_fcn(int nbytes, char *file, int line)
{
    return(lis_malloc(nbytes, GFP_ATOMIC, 0, file, line)) ;
}

void	* _RP lis_alloc_kernel_fcn(int nbytes, char *file, int line)
{
    return(lis_malloc(nbytes, GFP_KERNEL, 0, file, line)) ;
}

void	* _RP lis_alloc_dma_fcn(int nbytes, char *file, int line)
{
    return(lis_malloc(nbytes, GFP_DMA, 0, file, line)) ;
}

void	* _RP lis_free_mem_fcn(void *mem_area, char *file, int line)
{
    lis_free(mem_area, file, line) ;
    return(NULL) ;
}

