/************************************************************************
*                       STREAMS Debugging Aids                          *
*************************************************************************
*									*
* Author:	David Grothe	<dave@gcom.com>				*
*									*
* Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>		*
*									*
************************************************************************/
/*
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

#ident "@(#) LiS strdbg.c 2.23 12/15/02 18:00:04 "

#include <sys/stream.h>
#include <sys/poll.h>
#include <sys/lislocks.h>

#include <stdarg.h>
#include <sys/osif.h>
#if defined(LINUX)			/* Linux kernel version */
#include <sys/lismem.h>
#endif

/************************************************************************
*                          Memory Links                                 *
*************************************************************************
*									*
* We keep track of who allocates messages and other areas of memory.	*
* The routine lis_print_mem will print out ownership information about	*
* all of the memory allocated by streams.				*
*									*
************************************************************************/

typedef struct mem_link
{
    struct mem_link	*next ;
    struct mem_link	*prev ;

    long		 size ;
    char		 file_name[32] ;
    int	 		 line_nr ;

} mem_link_t ;

lis_spin_lock_t	 lis_mem_lock ;
long		 lis_mem_alloced ;	/* keep track of allocated memory */
long		 lis_max_mem ;		/* maximum to allocate */

mem_link_t	 lis_mem_head = {&lis_mem_head, &lis_mem_head} ;

/*
 * Allocations larger than this allocate pages instead of calling
 * kmalloc.
 */
#define	MAX_KMALLOC_SIZE	(120000 - sizeof(mem_link_t) - sizeof(long) \
				        - sizeof(void *) \
				)


#ifndef DEBUG_MASK
#define	DEBUG_MASK	LIS_DEBUG_SAFE_BIT
#endif
#ifndef DEBUG_MASK2
#define	DEBUG_MASK2	0
#endif
unsigned long	lis_debug_mask = DEBUG_MASK;	/* debug mask */
unsigned long	lis_debug_mask2 = DEBUG_MASK2;	/* debug mask */

/************************************************************************
*                          Print Buffer                                 *
*************************************************************************
*									*
* This is a mechanism whereby printf-like data can be concatenated in	*
* a large buffer and periodically flushed to the screen.  This allows	*
* for debugging printfs that do not overly slow down execution, and can	*
* thus be used to catch timing-dependent bugs.				*
*									*
************************************************************************/

char			 lis_print_buffer[LIS_PRINT_BUFFER_SIZE] ;
char			*lis_nxt_print_ptr = lis_print_buffer ;
static lis_spin_lock_t	 lis_print_buffer_lock ;
static int		 lock_initialized ;

void	lis_bprintf(char *fmt, ...)
{
    extern char	 lis_cmn_err_buf[];
    extern int	 vsprintf (char *, const char *, va_list);
    va_list	 args;
    int		 nbytes ;

    if (!lock_initialized)
    {
	lis_spin_lock_init(&lis_print_buffer_lock, "PrntBfr") ;
	lock_initialized = 1 ;
    }

    lis_spin_lock(&lis_print_buffer_lock) ;

    va_start (args, fmt);
    vsprintf (lis_cmn_err_buf, fmt, args);
    va_end (args);
    nbytes = strlen(lis_cmn_err_buf) ;

    if (&lis_print_buffer[LIS_PRINT_BUFFER_SIZE] - lis_nxt_print_ptr - 50
								       > nbytes)
    {					/* will fit in buffer */
	strcpy(lis_nxt_print_ptr, lis_cmn_err_buf) ;
	lis_nxt_print_ptr += nbytes ;
    }
    else				/* nearing end of buffer */
	strcpy(lis_nxt_print_ptr, "\n...Truncated...\n") ;

    lis_spin_unlock(&lis_print_buffer_lock) ;
}

void	lis_flush_print_buffer(void)
{
    char	*p ;
    char	 c ;
    int		 nbytes ;
    int		 printed ;

    if (!lock_initialized)
    {
	lis_spin_lock_init(&lis_print_buffer_lock, "PrntBfr") ;
	lock_initialized = 1 ;
    }

    lis_spin_lock(&lis_print_buffer_lock) ;
    printed = 1000 ;			/* kernel's buf is 1024, leave room */
    for (nbytes = strlen(lis_print_buffer), p = lis_print_buffer;
	 nbytes > 0;
	 nbytes -= printed, p += printed)
    {
	if (nbytes < 1000) printed = nbytes ;
	c = p[printed] ;
	p[printed] = 0 ;
	printk("%s", p) ;
	p[printed] = c ;
    }

    lis_nxt_print_ptr = lis_print_buffer ;
    *lis_nxt_print_ptr = 0 ;
    lis_spin_unlock(&lis_print_buffer_lock) ;
}

/************************************************************************
*                         put_file_name                                 *
*************************************************************************
*									*
* Copy the name into the file_name field of the memory link.  If the	*
* name is too long, just take the last 'n' characters of it.		*
*									*
************************************************************************/
static void put_file_name(mem_link_t *p, const char *name)
{
    int		len = strlen(name) ;
    int		index = 0 ;;

    if (len > sizeof(p->file_name) - 1)
	index = len - (sizeof(p->file_name) - 1) ;

    strcpy(p->file_name, &name[index]) ;
}

/************************************************************************
*                            lis_malloc                                 *
*************************************************************************
*									*
* Allocate nbytes of memory and return a pointer to it.			*
*									*
************************************************************************/
void	*lis_malloc(int nbytes, int class, int use_cache,
		    char *file_name, int line_nr)
{
    mem_link_t	*p ;
    long	*lp ;				/* ptr to guard word */
    int		 abytes ;			/* # bytes to allocate */
    int		 tbytes ;			/* total # of bytes */
    int		 psw;

    LisUpCount(MEMALLOCS) ;			/* stats array */

    if (lis_max_mem != 0 && lis_mem_alloced >= lis_max_mem)
    {
	LisUpFailCount(MEMALLOCD) ;		/* stats array */
	LisUpFailCount(MEMALLOCS) ;		/* stats array */
	return(NULL) ;
    }

    /*
     * Round nbytes up to even multiple of 16 bytes.  Then allocate
     * that much space plus room for the header plus room for the
     * guard word at the end of the area.
     */
    abytes = (nbytes + 15) & ~0x0F ;
    tbytes = abytes + sizeof(*p) + sizeof(long) ;
#if defined(LINUX)			/* Linux kernel version */
    if (abytes > MAX_KMALLOC_SIZE)
	p = (mem_link_t *) lis_get_free_pages_fcn(tbytes, class,
						  file_name, line_nr) ;
    else
#endif
	p = (mem_link_t *) KALLOC(tbytes, class, use_cache) ;

    if (p == NULL)
    {
	LisUpFailCount(MEMALLOCD) ;		/* stats array */
	LisUpFailCount(MEMALLOCS) ;		/* stats array */
	return(NULL) ;
    }

    lis_spin_lock_irqsave(&lis_mem_lock, &psw) ;

    lis_mem_alloced += abytes ;			/* keep track of memory */
    LisUpCounter(MEMALLOCD, abytes) ;		/* stats array */

    p->next	  = &lis_mem_head ;		/* we point fwd to head */
    p->prev	  = lis_mem_head.prev ;		/* we point back to old last */
    p->prev->next = p ;				/* elt b4 us points to us */
    lis_mem_head.prev = p ;			/* we are last elt in list */

    p->size	  = abytes ;			/* save # bytes */
    put_file_name(p, file_name) ;		/* copy in filename of caller */
    p->line_nr	  = line_nr;			/* line number called from */

    lp = (long *) (((char *)p) + abytes + sizeof(*p)) ;	/* to guard word */
    *lp = MEM_GUARD ;				/* plant guard word */

    if (LIS_DEBUG_MALLOC)			/* debugging allocations */
    {
	printk("lis_malloc: ") ;
	lis_print_block(p+1) ;
    }

    lis_spin_unlock_irqrestore(&lis_mem_lock, &psw) ;

    if (LIS_DEBUG_MONITOR_MEM)
	lis_check_mem() ;			/* check all mem areas */

    return(p+1) ;				/* rtn area caller can use */

} /* lis_malloc */

/************************************************************************
*                            lis_zmalloc                                *
*************************************************************************
*									*
* Like lis_malloc, only it zeros the memory before returning.		*
*									*
************************************************************************/
void	*lis_zmalloc(int nbytes, int class, char *file_name, int line_nr)
{
    void	*ptr = lis_malloc(nbytes, class, 0, file_name, line_nr) ;

    if (ptr != NULL)
	memset(ptr, 0, nbytes) ;

    return(ptr) ;
}

/************************************************************************
*                          lis_check_guard                              *
*************************************************************************
*									*
* Check the guard word at the end of the area pointed to by 'ptr'.	*
* Return 0 if bad, 1 if good.  If bad, print a message to that effect	*
* with "msg" embedded in it.						*
*									*
* The argument 'ptr' is the pointer to the usable portion of the area,	*
* that is, the same value as returned by lis_malloc.			*
*									*
************************************************************************/
int	lis_check_guard(void *ptr, char *msg)
{
    mem_link_t	*p ;
    long	*lp ;

    if (ptr == NULL) return(1) ;		/* "good" */

    p = (mem_link_t *) ptr ;
    p-- ;					/* go back to link structure */
    lp = (long *) (((char *)p) + p->size + sizeof(*p)) ; /* to guard word */

    if (*lp == MEM_GUARD) return(1) ;		/* good guard word */

    printk("%s: memory area at 0x%lx: guard word = 0x%lx, should be 0x%lx\n",
		msg == NULL ? "lis_check_guard" : msg, 
		(long) ptr, *lp, (long) MEM_GUARD) ;
    lis_print_block(ptr) ;			/* print the hdr info */
    return(0);

} /* lis_check_guard */

/************************************************************************
*                            lis_free                                   *
*************************************************************************
*									*
* Free a block of memory allocated by lis_malloc.			*
*									*
************************************************************************/
void	lis_free(void *ptr, char *file_name, int line_nr)
{
    mem_link_t	*p ;
    int		 psw;
    int		 rslt ;

    if (ptr == NULL) return ;

    lis_spin_lock_irqsave(&lis_mem_lock, &psw) ;

    LisUpCount(MEMFREES) ;			/* stats array */
    if (LIS_DEBUG_MONITOR_MEM)
	rslt = lis_check_mem() ;		/* check all mem areas */
    else
	rslt = lis_check_guard(ptr, file_name) ;/* check the guard word */

    if (rslt == 0)
	LisUpFailCount(MEMFREES) ;		/* stats array */

    p = (mem_link_t *) ptr ;
    p-- ;					/* go back to link structure */

    lis_mem_alloced -= p->size ;		/* keep track of memory */
    LisDownCounter(MEMALLOCD, p->size) ;	/* stats array */

    p->prev->next = p->next ;			/* prev elt links around us */
    p->next->prev = p->prev ;			/* next elt links around us */
    p->next	  = NULL ;			/* clobber our links */
    p->prev	  = NULL ;

    put_file_name(p, file_name) ;		/* copy in filename of caller */
    p->line_nr	  = line_nr;			/* line number called from */

    if (LIS_DEBUG_MALLOC)			/* debugging allocations */
    {
	printk("lis_free: ") ;
	lis_print_block(p+1) ;
    }

    lis_spin_unlock_irqrestore(&lis_mem_lock, &psw) ;

#if defined(LINUX)				/* Linux kernel version */
    if (p->size > MAX_KMALLOC_SIZE)
	lis_free_pages(p) ;			/* pages, not slab */
    else
#endif
	KFREE(p) ;

} /* lis_free */

/************************************************************************
*                         lis_terminate_mem                             *
*************************************************************************
*                                                                      *
* Does the final shutdown of the memory subsystem.                     *
* This will clean up any memory leaks.                                 *
*                                                                      *
************************************************************************/
void   lis_terminate_mem(void)
{
    mem_link_t *p ;
    int		psw ;
    int		nbytes = 0 ;

    lis_spin_lock_irqsave(&lis_mem_lock, &psw) ;
    while (lis_mem_head.next != &lis_mem_head)
    {
	p = lis_mem_head.next ;
	lis_spin_unlock_irqrestore(&lis_mem_lock, &psw) ;
#if 0
        lis_mem_head.next = p->next ;
        p->next->prev = &lis_mem_head ;
#endif
        if (LIS_DEBUG_MEM_LEAK)
	   printk("lis: %ld bytes (at %p) leaked from file %s, line %d\n",
				      p->size, p+1, p->file_name, p->line_nr);
	nbytes += p->size ;
	lis_free(p+1, "lis_terminate_mem", __LINE__) ;
	lis_spin_lock_irqsave(&lis_mem_lock, &psw) ;
    }
    lis_spin_unlock_irqrestore(&lis_mem_lock, &psw) ;

    if (nbytes > 0)
	printk("lis_terminate_mem: %d bytes of leaked memory freed\n", nbytes) ;
}

/************************************************************************
*                         lis_check_mem                                 *
*************************************************************************
*									*
* Run down the list of allocated memory and check all the guard words.	*
*									*
* Because the allocator can be called from interrupt level, this needs	*
* to run with interrupts disabled.					*
*									*
* Return 1 if all areas are OK.  Return 0 if some area is bad.		*
*									*
************************************************************************/
int	lis_check_mem(void)
{
    mem_link_t		*p ;
    int			 rslt = 1 ;
    int		 	 psw;

    lis_spin_lock_irqsave(&lis_mem_lock, &psw) ;
    for (p = lis_mem_head.next; p != &lis_mem_head; p = p->next)
    {
	if (!lis_check_guard(p+1, "lis_check_mem")) rslt = 0 ;
    }

    lis_spin_unlock_irqrestore(&lis_mem_lock, &psw) ;
    return(rslt) ;

} /* lis_check_mem */

/************************************************************************
*                          lis_mark_mem                                 *
*************************************************************************
*									*
* Given a pointer as returned by lis_malloc, remark the owner of the	*
* memory in the links.							*
*									*
************************************************************************/
void	lis_mark_mem(void *ptr, const char *file_name, int line_nr)
{
    mem_link_t	*p ;

    if (ptr == NULL) return ;

    p = (mem_link_t *) ptr ;
    p-- ;					/* go back to link structure */
    put_file_name(p, file_name) ;		/* copy in filename of caller */
    p->line_nr	  = line_nr;			/* line number called from */

} /* lis_mark_mem */

/************************************************************************
*                           lis_print_block                             *
*************************************************************************
*									*
* Print a single block of memory.  The pointer passed in is the		*
* pointer to the user section of the memory.  We back up to the		*
* link field and print it out in decoded form.				*
*									*
************************************************************************/
void	lis_print_block(void *ptr)
{
    mem_link_t		*p ;

    p = ((mem_link_t *) ptr) - 1 ;

    if (p->file_name != NULL)
    {
	switch (p->line_nr)
	{
	case MEM_QUEUE:
	    if (LIS_DEBUG_DMP_QUEUE)
	    {
		queue_t		*q = (queue_t *) (p+1) ;

		lis_print_queue(q) ;	/* the read queue */
		lis_print_queue(q+1) ;	/* the write queue */
	    }
	    else
		printk("queue: \"%s\", size=%ld\n", p->file_name, p->size);
	    break ;
	case MEM_MSG:
	    if (LIS_DEBUG_DMP_MBLK)
		lis_print_msg((mblk_t *)(p+1), p->file_name,
			      (LIS_DEBUG_DMP_DBLK) ? PRINT_DATA_ENTIRE : 0) ;
	    else
		printk("mblk: \"%s\", size=%ld\n", p->file_name, p->size);
	    break ;
	case MEM_STRMHD:
	    if (LIS_DEBUG_ADDRS)		/* printing addresses */
		printk("0x%lx: ", (long) ptr) ;

	    printk("stream-head: \"%s\", size=%ld\n", p->file_name, p->size);
	    break ;
	case MEM_TIMER:
	    if (LIS_DEBUG_ADDRS)		/* printing addresses */
		printk("0x%lx: ", (long) ptr) ;

	    printk("timer: \"%s\", size=%ld\n", p->file_name, p->size);
	    break ;
	case 0:
	    if (LIS_DEBUG_ADDRS)		/* printing addresses */
		printk("0x%lx: ", (long) ptr) ;

	    printk("\"%s\", size=%ld\n", p->file_name, p->size);
	    break ;
	default:
	    if (LIS_DEBUG_ADDRS)		/* printing addresses */
		printk("0x%lx: ", (long) ptr) ;

	    printk("%s #%d, size=%ld\n", p->file_name, p->line_nr, p->size);
	    break ;
	}
    }
    else
	printk("unknown file and line number\n") ;

} /* lis_print_block */

/************************************************************************
*                           lis_print_mem                               *
*************************************************************************
*									*
* Walk the memory link list and print out each element.			*
*									*
************************************************************************/
void	lis_print_mem(void)
{
    mem_link_t		*p ;
    int			 psw ;

    lis_spin_lock_irqsave(&lis_mem_lock, &psw) ;
    for (p = lis_mem_head.next; p != &lis_mem_head; p = p->next)
    {
	if (lis_check_guard(p+1, "lis_print_mem"))
	    lis_print_block(p+1) ;
    }
    lis_spin_unlock_irqrestore(&lis_mem_lock, &psw) ;

} /* lis_print_mem */

/************************************************************************
*                           lis_print_queues                            *
*************************************************************************
*									*
* Find all the queues that are allocated and print them out.		*
*									*
************************************************************************/
void lis_print_queues(void)
{
    mem_link_t		*p ;
    int		 	 psw;

    lis_spin_lock_irqsave(&lis_mem_lock, &psw) ;
    for (p = lis_mem_head.next; p != &lis_mem_head; p = p->next)
    {
	if (   lis_check_guard(p+1, "lis_print_queues")
	    && p->line_nr == MEM_QUEUE)
	{
	    if (LIS_DEBUG_DMP_QUEUE)
	    {
		queue_t		*q = (queue_t *) (p+1) ;

		lis_print_queue(q) ;	/* the read queue */
		lis_print_queue(q+1) ;	/* the write queue */
	    }
	    else
		printk("queue: \"%s\", size=%ld\n", p->file_name, p->size);
	}
    }
    lis_spin_unlock_irqrestore(&lis_mem_lock, &psw) ;

} /* lis_print_queues */

/************************************************************************
*                           lis_strm_name                               *
*************************************************************************
*									*
* Return the name of the stream attached to the stream head or		*
* the string "(unknown)" if there is no name for this stream.		*
* Used for debugging printfs.						*
*									*
************************************************************************/

const char	*lis_strm_name(stdata_t *head)
{
    const char	*name ;

#if 1
    if (head != NULL && head->magic != STDATA_MAGIC)
	return(NULL) ;
    
    if (head->sd_name[0] != 0)
	return(head->sd_name);

    if (   head->sd_strtab != NULL
	&& head->sd_strtab->st_rdinit != NULL
	&& head->sd_strtab->st_rdinit->qi_minfo != NULL
	&& (name = head->sd_strtab->st_rdinit->qi_minfo->mi_idname) != NULL
       )
	return(name);
#else
    if (   head->sd_strtab != NULL
	&& head->sd_strtab->st_rdinit != NULL
	&& head->sd_strtab->st_rdinit->qi_minfo != NULL
	&& (name = head->sd_strtab->st_rdinit->qi_minfo->mi_idname) != NULL
       )
	return(name);
#endif

    return("(unknown)") ;

} /* strm_name */

/************************************************************************
*                     lis_strm_name_from_queue                          *
*************************************************************************
*									*
* Find the stream name associated with the queue.			*
*									*
************************************************************************/
const char	*lis_strm_name_from_queue(queue_t *q)
{
    if (q->q_str != NULL)
	return(lis_strm_name((stdata_t *) q->q_str)) ;

    return(NULL) ;

} /* lis_strm_name_from_queue */

/************************************************************************
*                          lis_queue_name				*
*************************************************************************
*									*
* Return the name of the stream attached to the queue or		*
* the string "(unknown)" if there is no name for this stream.		*
* Used for debugging printfs.						*
*									*
************************************************************************/

const char	*lis_queue_name(queue_t *q)
{
    const char	*name ;

    if (   q == NULL
	|| q->q_magic != Q_MAGIC
       )
	return("(bad)") ;

    if (   q->q_qinfo != NULL
	&& q->q_qinfo->qi_minfo != NULL
	&& (name = q->q_qinfo->qi_minfo->mi_idname) != NULL
       )
	return(name);

    return("(unknown)") ;

} /* lis_queue_name */

/************************************************************************
*                          lis_msg_type_name				*
*************************************************************************
*									*
* Return the ASCII string name of the type of a given message.		*
*									*
************************************************************************/

const char	*lis_msg_type_name(mblk_t *mp)
{
    if (mp == NULL) return("NULL-MSG") ;

    if (mp->b_datap == NULL) return("NO-DATA-BLK") ;

    switch(lis_btype(mp))
    {
    case M_DATA:	return("M_DATA");
    case M_PROTO:	return("M_PROTO");
    case M_BREAK:	return("M_BREAK");
    case M_CTL:		return("M_CTL");
    case M_DELAY:	return("M_DELAY");
    case M_IOCTL:	return("M_IOCTL");
    case M_PASSFP:	return("M_PASSFP");
    case M_RSE:		return("M_RSE");
    case M_SETOPTS:	return("M_SETOPTS");
    case M_SIG:		return("M_SIG");
    case M_COPYIN:	return("M_COPYIN");
    case M_COPYOUT:	return("M_COPYOUT");
    case M_ERROR:	return("M_ERROR");
    case M_FLUSH:	return("M_FLUSH");
    case M_HANGUP:	return("M_HANGUP");
    case M_IOCACK:	return("M_IOCACK");
    case M_IOCNAK:	return("M_IOCNAK");
    case M_IOCDATA:	return("M_IOCDATA");
    case M_PCPROTO:	return("M_PCPROTO");
    case M_PCRSE:	return("M_PCRSE");
    case M_PCSIG:	return("M_PCSIG");
    case M_READ:	return("M_READ");
    case M_STOP:	return("M_STOP");
    case M_START:	return("M_START");
    case M_STARTI:	return("M_STARTI");
    case M_STOPI:	return("M_STOPI");
    }

    return("UNKNOWN-MSG") ;

} /* lis_msg_type_name */

/************************************************************************
*                          lis_maj_min_name			 	*
*************************************************************************
*									*
* Given a pointer to an stdata structure, return an ASCII form		*
* of the major and minor device numbers associated with it.		*
*									*
* By having a small array of static locations we allow for a limited	*
* amount of nestedness in calling this routine, say from both background*
* and an interrupt routine or three.					*
*									*
************************************************************************/

const char	*lis_maj_min_name(stdata_t *head)
{
    int			 inx ;
    static char		 name[8][32] ;
    static int		 index ;

    if ( head == NULL )
	return("") ;

    inx = index++ & 0x03 ;
    sprintf(name[inx], "(%ld,%ld)",
			(long)STR_MAJOR(head->sd_dev),
			(long)STR_MINOR(head->sd_dev)) ;

    return(name[inx]) ;

} /* lis_maj_min_name */

/************************************************************************
*                           lis_print_queue                             *
*************************************************************************
*									*
* Print out some of the interesting features about a queue.		*
*									*
************************************************************************/
void	lis_print_queue(queue_t *q)
{
    const char	*name ;

    printk("queue \"%s\"", lis_queue_name(q)) ;
    if (LIS_DEBUG_ADDRS)			/* printing addresses */
	printk(" @ 0x%lx: q_ptr=0x%lx", (long)q, (long)q->q_ptr) ;
    printk(" magic=0x%lx", q->q_magic) ;
    if ((name = lis_strm_name_from_queue(q)) != NULL)
	printk(" stream \"%s\"", name) ;
    printk("\n      %s-q", F_ISSET(q->q_flag,QREADR) ? "read" : "write");
    if (q->q_next == NULL)
	printk(" q_next=NULL") ;
    else
	printk(" q_next=\"%s\"", lis_queue_name(q->q_next)) ;

    printk(" q_flag 0x%lx%s ", q->q_flag, q->q_flag != 0 ? ":" : "") ;
    if (F_ISSET(q->q_flag,QENAB)) printk(" QENAB");
    if (F_ISSET(q->q_flag,QWANTR)) printk(" QWANTR");
    if (F_ISSET(q->q_flag,QWANTW)) printk(" QWANTW");
    if (F_ISSET(q->q_flag,QFULL)) printk(" QFULL");
    if (F_ISSET(q->q_flag,QREADR)) printk(" QREADR");
    if (F_ISSET(q->q_flag,QUSE)) printk(" QUSE");
    if (F_ISSET(q->q_flag,QNOENB)) printk(" QNOENB");
    if (F_ISSET(q->q_flag,QBACK)) printk(" QBACK");
    if (F_ISSET(q->q_flag,QRETRY)) printk(" QRETRY");
    if (F_ISSET(q->q_flag,QSCAN)) printk(" QSCAN");
    if (F_ISSET(q->q_flag,QCLOSEWT)) printk(" QCLOSEWT");
    if (F_ISSET(q->q_flag,QCLOSING)) printk(" QCLOSING");
    if (F_ISSET(q->q_flag,QWASFULL)) printk(" QWASFULL");
    printk("\n") ;

    printk("      q_minpsz=%lu ", q->q_minpsz);

    if (q->q_maxpsz == INFPSZ)
	printk("q_maxpsz=INFPSZ ") ;
    else
	printk("q_maxpsz=%lu ", q->q_maxpsz);

    printk("q_hiwat=%lu q_lowat=%lu", q->q_hiwat, q->q_lowat);

    printk(", %d msgs, %ld bytes\n", lis_qsize(q), q->q_count) ;
#if 0
    printk(" %son queue run list,", q->q_link != NULL ? "" : "not ");
    printk(" %son queue scan list\n", q->q_scnxt != NULL ? "" : "not ");
#endif
} /* lis_print_queue */

/************************************************************************
*                            lis_print_stream                           *
*************************************************************************
*									*
* Print out the structure of the given stream.  This is not a field-	*
* by-field printing of the stdata structure but a synopsis of the	*
* topology of the stream headed by this structure including its		*
* pushed modules and multiplexor configuration if it is the control	*
* stream of a multiplexor.						*
*									*
************************************************************************/
/*
 * Worker routine for recursive printing
 */
static void strm_print(stdata_t *hd, int indent)
{
    queue_t		*q ;
    stdata_t		*hp ;

#define	prefix	do { int i; for (i=0; i<indent; i+=2) printk(". ") ;} while (0)

    prefix ;

    if (hd->magic != STDATA_MAGIC)
    {
	printk("STREAM head @0x%p: bad magic number 0x%lx, should be 0x%lx\n",
		(long) hd, hd->magic, STDATA_MAGIC) ;
	return ;
    }

    printk("STREAM \"%s\": ", hd->sd_name) ;
    if (hd->sd_mux.mx_index != 0)
	printk("muxid=%u ", hd->sd_mux.mx_index) ;

    printk(" %s", lis_queue_name(q = hd->sd_wq)) ;
    if ( q != NULL )			/* wq is non-NULL (paranoid) */
    {
	int samestr = SAMESTR(q);   /* FIFO or pipe */
	int sameq   = (!samestr && q->q_next == OTHER(q));  /* FIFO */

	if (!sameq)
	    for (q = q->q_next; q && !sameq; q = q->q_next)
	    {
		printk("%s%s",
		       (samestr ? "<==>" : "<=X=>"),
		       lis_queue_name(q)) ;
		samestr = SAMESTR(q);
		sameq   = (!samestr && q->q_next == OTHER(q));
	    }
   }

    printk("\n") ;

    if ( (hp = hd->sd_mux.mx_hd) != NULL)	/* is a ctl stream */
    {
	indent += 2 ;
	prefix ;
	printk("Lower multiplexors:\n") ;
	for (; hp != NULL; hp = hp->sd_mux.mx_next)
	{
	    strm_print(hp, indent+2) ;
	}
    }

} /* strm_print */

#undef prefix

void	lis_print_stream(stdata_t *hd)
{
    strm_print(hd, 0) ;

} /* lis_print_stream */

/************************************************************************
*                         lis_print_data                                *
*************************************************************************
*									*
* Print the data buffer of the associated message.			*
*									*
************************************************************************/

void	lis_print_data(mblk_t *mp, int opt, int cont)
{
    const unsigned char	*p ;
    unsigned char	*start ;
    unsigned char	*end ;
    int			 n ;

    switch (opt)
    {
    case PRINT_DATA_ENTIRE:
	if (mp->b_datap != NULL)
	{
	    start = mp->b_datap->db_base;
	    end	  = mp->b_datap->db_lim;
	}
	else
	{
	    start = NULL;
	    end	  = NULL;
	}
	break ;
    case PRINT_DATA_RDWR:
    default:
	start	= mp->b_rptr ;
	end	= mp->b_wptr ;
	break ;
    }

    if (start > end)
    {
	printk("mblk@%lx: start (%lx) > end (%lx)\n",
				(long)mp, (long)start, (long)end) ;
	return ;
    }

    if (start == NULL || end == NULL)
    {
	printk("mblk@%lx: start (%lx) or end (%lx) NULL\n",
				(long)mp, (long)start, (long)end) ;
	return ;
    }

    printk("    ") ;					/* indent 4 */
    for (p = start, n = 0; p != end; p++, n++)
    {
	if (  n && (n & 0x0F) == 0)			/* printed 16th elt */
	    printk("\n    ") ;				/* indent 4 */

	printk("%c",  (p == mp->b_wptr) ? '>' : ' ') ;
	printk("%c",  (p == mp->b_rptr) ? '<' : ' ') ;
	printk("%02X", *p) ;
    }

    if (n && p == mp->b_wptr)
	printk(">") ;

    printk("\n") ;

} /* lis_print_data */

/************************************************************************
*                           lis_print_msg                               *
*************************************************************************
*									*
* Print out an entire message with the given options.			*
*									*
* 'opt' is the print format option, or zero for the short form.		*
*									*
************************************************************************/
void	lis_print_msg(mblk_t *mp, const char *prefix, int opt)
{
    int		nbytes ;
    int		prnt_bfr ;
    int		cont ;

    if (mp == NULL)
    {
	printk("NULL message pointer\n") ;
	return ;
    }

    printk("mblk: \"%s\"", prefix);
    if (LIS_DEBUG_ADDRS)			/* printing addresses */
	printk(" @ 0x%lx: b_next=0x%lx b_prev=0x%lx b_datap=0x%lx",
	      (long)mp, (long)mp->b_next, (long)mp->b_prev, (long)mp->b_datap) ;

    for (cont = 0; mp != NULL; mp = mp->b_cont, cont++)
    {
	nbytes = mp->b_wptr - mp->b_rptr ;
	prnt_bfr = (opt == PRINT_DATA_ENTIRE) || (opt != 0 && nbytes != 0) ;
	if (cont)
	    printk(">>   ") ;

	printk(" %s", lis_msg_type_name(mp)) ;
	printk(" b_band=%d bytes=%d\n", mp->b_band, nbytes) ;

	if (prnt_bfr)
	    lis_print_data(mp, opt, cont) ;
    }

} /* lis_print_msg */

/************************************************************************
*                           lis_poll_events                             *
*************************************************************************
*									*
* Return the ASCII decoding of a poll event mask.			*
*									*
************************************************************************/
char	*lis_poll_events(short events)
{
    static char		 event_names[8][150] ;
    static int		 index ;
    char		*ascii_events ;
    int			 inx ;

    inx = index++ & 0x03 ;
    ascii_events = event_names[inx] ;
    ascii_events[0] = 0 ;

    if (events & POLLIN) strcat(ascii_events, "POLLIN ") ;
    if (events & POLLRDNORM) strcat(ascii_events, "POLLRDNORM ") ;
    if (events & POLLRDBAND) strcat(ascii_events, "POLLRDBAND ") ;
    if (events & POLLPRI) strcat(ascii_events, "POLLPRI ") ;
    if (events & POLLOUT) strcat(ascii_events, "POLLOUT ") ;
    if (events & POLLWRNORM) strcat(ascii_events, "POLLWRNORM ") ;
    if (events & POLLWRBAND) strcat(ascii_events, "POLLWRBAND ") ;
    if (events & POLLMSG) strcat(ascii_events, "POLLMSG ") ;
    if (events & POLLERR) strcat(ascii_events, "POLLERR ") ;
    if (events & POLLHUP) strcat(ascii_events, "POLLHUP ") ;
    if (events & POLLNVAL) strcat(ascii_events, "POLLNVAL ") ;

    if (ascii_events[0] == 0)
	sprintf(ascii_events, "0x%x", events) ;

    return(ascii_events) ;

} /* lis_poll_events */

#if !defined(LIS_SRC)
#define LIS_SRC "/usr/src/LiS"
#endif

const char *lis_basename( const char *filename )
{
    int len = strlen(LIS_SRC);

    /*
     *  poor man's basename() function - this really should be in the
     *  kernel lib...
     */
    if (len && strncmp(LIS_SRC,filename,len) == 0) {
	if (filename[len] == '/')
	    return filename+len+1;
	else
	    return filename+len;
    }
    else
	return filename;
}
