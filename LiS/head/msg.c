/*                               -*- Mode: C -*- 
 * msg.c --- streams message handling
 * Author          : Graham Wheeler
 * Created On      : Tue May 31 22:25:19 1994
 * Last Modified By: David Grothe
 * RCS Id          : $Id: msg.c,v 1.6 1996/01/17 18:57:05 dave Exp $
 * Purpose         : provide streams message handling
 *                 : this is the main memory allocation related module.
 * ----------------______________________________________________
 *
 *   Copyright (C) 1995 Graham Wheeler, Francisco J. Ballesteros, 
 *                      Denis Froschauer
 *   Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>
 *
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
 *
 *    You can reach us by email to any of
 *    gram@aztec.co.za, nemo@ordago.uc3m.es, 100741.1151@compuserve.com
 */

#ident "@(#) LiS msg.c 2.19 09/07/04 11:11:22 "

/*
 * The memory allocation mechanism is based on that in SVR4.2.
 * That is, all memory is allocated dynamically, with freed
 * message headers being held on a free list. When kernel memory
 * is low some of these can be reclaimed by calling strgiveback.
 *
 * Message headers are 128 bytes in size. The extra space
 * is used as the data buffer for smallish messages. This
 * scheme means that in most cases, a call to allocb just
 * requires unlinking a message header from the free list
 * and initialising it.
 *
 * This scheme does add some complexity, however, with
 * regard to dupb/dupmsg. In this case the duplicate can
 * have pointers to a data buffer within some other message
 * header. Thus, if a message header is freed, we have to
 * check if its internal data buffer is still in use by
 * someone else, in which case we defer freeing the header;
 * on the other hand, if we are freeing the last reference
 * to some other data buffer in a message header, we have two
 * headers to free. All of this logic is nicely hidden in freeb()
 * (with a little bit of it leaking into pullupmsg).
 *
 * NB: This does rely on the fact that if a message block
 * has a data buffer of FASTBUF or less in size and no special
 * free function (i.e. it wasn't an esballoc), then that data
 * buffer lives internally within some (not necessarily the same)
 * message header, and was *not* allocated elsewhere.
 *
 * To put it another way, if you don't completely understand
 * the memory management scheme, don't fiddle with any of
 * the following code, and don't ever directly modify data
 * block elements like db_base, db_lim and db_size.
 */

/*  -------------------------------------------------------------------  */
/*				 Dependencies                            */

#include <sys/stream.h>
#include <sys/osif.h>

/*  -------------------------------------------------------------------  */
/*			       Symbols & types                           */



/*  -------------------------------------------------------------------  */
/*				  Glob. Vars                             */

lis_spin_lock_t	  lis_msg_lock ;	  /* protects msg list */
volatile struct mdbblock  *lis_mdbfreelist = NULL; /* msg block free list */
int 	     lis_strmsgsz;		  /* maximum stream message size */
lis_atomic_t lis_strcount ;		  /* # bytes allocated to msgs   */
long	     lis_max_msg_mem ;		  /* maximum to allocate */

/*  -------------------------------------------------------------------  */
/*			   Local functions & macros                      */


/* allochdr - allocate a message header. For internal use.
 *
 * Use DBLK_ALLOC to get the memory.  Since a header (mblk) also
 * contains a small data block it must be DMA-able memory.
 */
#if !(defined(LINUX) && defined(USE_KMEM_CACHE))
#if defined(CONFIG_DEV)
static struct mdbblock *
allochdr(char *file_name, int line_nr)
#else
static struct mdbblock * allochdr(void)
#endif
{
#if !defined(CONFIG_DEV)
    static char *file_name = "__FILE__" ;
    static int   line_nr   = 0 ;
#endif
    struct mdbblock *rtn;
    lis_flags_t      psw;
    lis_spin_lock_irqsave(&lis_msg_lock, &psw) ;
    rtn = (struct mdbblock *) lis_mdbfreelist;
    /* Try to get an mdbblock from the free list */
    if (rtn == NULL) /* free list empty; try allocate a new header */
    {
	lis_spin_unlock_irqrestore(&lis_msg_lock, &psw) ;
	if (   lis_max_msg_mem == 0
	    || K_ATOMIC_READ(&lis_strcount) < lis_max_msg_mem
	   )
	    rtn = (struct mdbblock *)
		    DBLK_ALLOC(sizeof(struct mdbblock), file_name, line_nr, 0);
    }
    else
    {
	/* paranoia */
	if (lis_mdbfreelist ==
	    (struct mdbblock *)lis_mdbfreelist->msgblk.m_mblock.b_next)
	{
	    static int		msg_printed ;

	    lis_spin_unlock_irqrestore(&lis_msg_lock, &psw) ;
	    rtn = (struct mdbblock *) NULL;
	    if (!msg_printed++)
	    {
		printk("LiS:allochdr: called from %s #%d\n"
		       "    Buffer at head of free list is linked to itself.\n"
		       "    All future allocb calls will fail\n",
		       file_name, line_nr) ;
	    }
	}
	else
	{
	    /* remove header from free list */
	    lis_mdbfreelist = 
		(struct mdbblock *)lis_mdbfreelist->msgblk.m_mblock.b_next;

	    lis_spin_unlock_irqrestore(&lis_msg_lock, &psw) ;
	    LisDownCount(FREEHDRS);
	    lis_mark_mem(rtn, file_name, line_nr) ;     /* under new ownership*/
	}
    }

    if (rtn)	/* update counts of header allocations */
    {
	LisUpCount(HEADERS);
	rtn->msgblk.m_mblock.b_next = NULL;
    }
    else	/* update counts of header allocation failures */
    {
	LisUpFailCount(HEADERS);
    }

    return rtn;

}/*allochdr*/
#else
/*
 * This case (defined(LINUX) && defined(USE_KMEM_CACHE) is 
 * covered/handled in include/sys/LiS/linux-mdep.h as:
 *
 *  #if defined(CONFIG_DEV)
 *  #define allochdr(a,b) lis_kmem_cache_allochdr() 
 *  #else
 *  #define allochdr() lis_kmem_cache_allochdr()
 *  #endif
 *
 */
#endif
/*  -------------------------------------------------------------------  */

/* freehdr - return a header to the free list. For internal use.
 */

#if !(defined(LINUX) && defined(USE_KMEM_CACHE))
static void
freehdr(mblk_t *bp)
{
    lis_flags_t     psw;

    LisDownCount(HEADERS);

    if (K_ATOMIC_READ(&lis_strstats[FREEHDRS][CURRENT]) >= MAX_MBLKS)
    {
	FREE(bp);
	return ;
    }

    bp->b_cont = NULL ;
    /*
     * If the data block ptr points to the data area of this same
     * mblk then leave it alone for port mortem debugging purposes.
     * If it points outside this area then clear these pointers
     * so that memory printers won't be looking at deallocated memory.
     */
    if (   bp->b_datap == NULL
	|| (bp->b_datap->db_base != (void*) ((struct mdbblock *)bp)->databuf)
       )
    {
	bp->b_datap= NULL ;			/* clear data block ptr */
	bp->b_rptr = NULL ;
	bp->b_wptr = NULL ;
    }

    lis_mark_mem(bp, "free", MEM_MSG) ;		     /* under new ownership */

    lis_spin_lock_irqsave(&lis_msg_lock, &psw) ;
    bp->b_next = (mblk_t *)lis_mdbfreelist;
    lis_mdbfreelist = (struct mdbblock *)bp;
    lis_spin_unlock_irqrestore(&lis_msg_lock, &psw) ;

    LisUpCount(FREEHDRS);

}/*freehdr*/
#else /* !(defined(LINUX) && defined(USE_KMEM_CACHE)) */
/*
 *  This case - LINUX and USE_KMEM_CACHE - is handled/covered 
 *  in linux-mdep.h via #define freehdr(a) lis_msgb_cache_freehdr((a))
 */
#endif /* !(defined(LINUX) && defined(USE_KMEM_CACHE)) */


/*  -------------------------------------------------------------------  */
/* lis_msg_terminate - do the final shutdown of the msg memory subsystem.
 *
 * This will clean up any memory leaks.					*
 */
#if !(defined(LINUX) && defined(USE_KMEM_CACHE))
void
lis_terminate_msg(void)
{
    struct mdbblock *p;

    while (lis_mdbfreelist != NULL)
    {
	p = (struct mdbblock *) lis_mdbfreelist;
	lis_mdbfreelist = 
		(struct mdbblock *)lis_mdbfreelist->msgblk.m_mblock.b_next;
	FREE(p);
    }
}
#else  /* !(defined(LINUX) && defined(USE_KMEM_CACHE)) */
/*
 *  This case - LINUX and USE_KMEM_CACHE - is handled/covered 
 *  in linux-mdep.h via #define 
 */
#endif  /* !(defined(LINUX) && defined(USE_KMEM_CACHE)) */


/*  -------------------------------------------------------------------  */
/* allocdb - allocate a data buffer. For internal use.
 *
 * Use DBLK_ALLOC to allocate the memory.  STREAMS buffers must
 * be DMA-able.
 */
#if defined(CONFIG_DEV)
static char *
allocdb(int size, char *file_name, int line_nr, int flags)
#else
static char *
allocdb(int size, int flags)
#endif
{
#if !defined(CONFIG_DEV)
    static char *file_name = "__FILE__" ;
    static int   line_nr   = 0 ;
#endif
    char *rtn;

    if (   lis_max_msg_mem != 0
	&& K_ATOMIC_READ(&lis_strcount) >= lis_max_msg_mem)
    {
	LisUpFailCount(DATABS);
	return(NULL) ;
    }

    LISASSERT(size > FASTBUF);
    rtn = (char *)DBLK_ALLOC(size, file_name, line_nr, flags);
    if (rtn == NULL)
	LisUpFailCount(DATABS);
    else
    {
	K_ATOMIC_ADD(&lis_strcount, size);	/* total allocated memory */
	LisUpCount(DATABS);
	LisUpCounter(DBLKMEM, size);
    }

    return rtn;
}/*allocdb*/

/*  -------------------------------------------------------------------  */
/* initb: initialise an allocated msg block. For internal use only.
 */

static void
initb(struct mdbblock *blk, char *buff, int size, struct free_rtn *frtn)
{
    static struct mbinfo	mb_zero ;
    static frtn_t		frtn_zero ;

    /* m_func field is unused */
    blk->msgblk = mb_zero ;
    blk->msgblk.m_mblock.b_rptr = (unsigned char *)buff;
    blk->msgblk.m_mblock.b_wptr = (unsigned char *)buff;
    blk->msgblk.m_mblock.b_datap=  (struct datab *)&blk->datblk.d_dblock;
    
    if (frtn != NULL)
    {
	blk->datblk.d_dblock.db_rtn  = *frtn ;	/* save structure */
	blk->datblk.d_dblock.frtnp = &blk->datblk.d_dblock.db_rtn;
    }
    else
    {
	blk->datblk.d_dblock.db_rtn = frtn_zero ;
	blk->datblk.d_dblock.frtnp = NULL ;
    }

    blk->datblk.d_dblock.db_base = (unsigned char *)buff;
    if ((char *) buff != blk->databuf)	/* separate data block */
    {					/* of exact size */
	blk->datblk.d_dblock.db_lim = (unsigned char *)(buff + size);
	blk->datblk.d_dblock.db_size = size;
    }
    else				/* data block w/header */
    {
	blk->datblk.d_dblock.db_lim = (unsigned char *)(buff + FASTBUF);
	blk->datblk.d_dblock.db_size = FASTBUF;
    }

    blk->datblk.d_dblock.db_ref = (uchar)1;
    blk->datblk.d_dblock.db_type = (uchar)M_DATA;
    if (LIS_DEBUG_CLEAN_MSG)
	lis_bzero(blk->datblk.d_dblock.db_base, blk->datblk.d_dblock.db_size) ;
}/*initb*/

/*  -------------------------------------------------------------------  */
/*			Exported functions & macros                      */


/*  -------------------------------------------------------------------  */

/* allocb_physreq: allocate an M_DATA message block of the specified
 *	size and memory constraints. 
 *
 * NOTE:  This routine is not implemented correctly.  physreq_ptr is
 *        supposed to point to a structure that describes the memory
 *        attributes desired.  See SVR4 man pages for details.
 *
 *        To be completed later.
 */

struct msgb * _RP
lis_allocb_physreq(int size, unsigned int priority, void *physreq_ptr,
			char *file_name, int line_nr)
{
    return(lis_allocb(size, priority, file_name, line_nr)) ;

}/*lis_allocb*/

/*  -------------------------------------------------------------------  */

/* allocb: allocate an M_DATA message block of the specified
 *	size. The priority is for compatibility only.
 */

struct msgb * _RP
lis_allocb(int size, unsigned int priority, char *file_name, int line_nr)
{
#if defined(CONFIG_DEV)
    struct mdbblock *rtn = allochdr(file_name, line_nr);
#else
    struct mdbblock *rtn = allochdr();
#endif
    char *buff;
    (void)priority;
    if (rtn == NULL)
	return NULL;
    rtn->msgblk.m_mblock.b_datap = NULL ;     /* in case we need to free it */
    if (size > FASTBUF)
    {
	/* allocate data buffer */
#if defined(CONFIG_DEV)
	if ((buff = allocdb(size, file_name, line_nr, 0)) == NULL)
#else
	if ((buff = allocdb(size, 0)) == NULL)
#endif
	{
	    /* failed; replace header in free list and return */
	    freehdr((mblk_t *)rtn);
	    return NULL;
	}
    }
    else buff = rtn->databuf;

    /* Set up message and data block headers */

    initb(rtn, buff, size, NULL);

    if ( LIS_DEBUG_ALLOC )
    {
	if (LIS_DEBUG_ADDRS)
	  printk("lis_allocb: mp=0x%lx dp=0x%lx(%s) from %s %d\n"
	         "            rptr=0x%lx wptr=0x%lx size=%d\n",
		  (long) rtn,
		  (long) rtn->msgblk.m_mblock.b_datap,
		  (size > FASTBUF ? "extl" : "intl"),
		  file_name, line_nr,
		  (long) ((mblk_t *)rtn)->b_rptr,
		  (long) ((mblk_t *)rtn)->b_wptr,
		  size);
	else
	  printk("lis_allocb: from %s #%d size=%d\n", file_name, line_nr, size);
    }
    return (mblk_t *)rtn;
}/*lis_allocb*/

/*  -------------------------------------------------------------------  */
/* testb: see if an allocation can actually be done.
 */

int _RP
lis_testb(int size, unsigned int priority)
{
#ifdef AVAIL
    int sz = 0;
    (void)priority;
    if (size > FASTBUF)
	sz += size;
    if (lis_mdbfreelist == NULL)
	sz += sizeof(struct mdbblock); /* gotta getta hedda */
    return (sz <= AVAIL());
#else
    /* hack - allocate and free */
    mblk_t *bp = allocb(size, priority);
    return bp ? (lis_freeb(bp), 1) : 0;
#endif
}/*lis_testb*/	

/*  -------------------------------------------------------------------  */
/*
 * esballoc: allocate a message block, using a user-provided data buffer
 *
 * Allocate an mblk with internal dblk and use the db_base of that
 * dblk to point to the user's buffer and to hold the freeinfo stuff.
 */

mblk_t * _RP
lis_esballoc(unsigned char *base, int size, int priority,
	     frtn_t *freeinfo, char *file_name, int line_nr)
{
    struct mdbblock *rtn = NULL;

    (void)priority;
#if defined(CONFIG_DEV)
    if (base && freeinfo && (rtn  = allochdr(file_name, line_nr)) != NULL)
#else
    if (base && freeinfo && (rtn  = allochdr()) != NULL)
#endif
    	initb(rtn, base, size, freeinfo);

    if (rtn == NULL) return(NULL) ;

    if ( LIS_DEBUG_ALLOC )
    {
	if (LIS_DEBUG_ADDRS)
	  printk("lis_esballoc: mp=0x%lx dp=0x%lx(user) from %s %d\n"
	         "              rptr=0x%lx wptr=0x%lx size=%d\n",
		  (long) rtn,
		  (long) rtn->msgblk.m_mblock.b_datap,
		  file_name, line_nr,
		  (long) ((mblk_t *)rtn)->b_rptr,
		  (long) ((mblk_t *)rtn)->b_wptr,
		  size);
	else
	  printk("lis_esballoc: from %s #%d size=%d\n",
					  file_name, line_nr, size);
    }

    return (mblk_t *)rtn;
}/*lis_esballoc*/

/*  -------------------------------------------------------------------  */

/* lis_freedb - Free the data block associated with a message and
 *              possibly the message header as well.
 */
void _RP
lis_freedb(mblk_t *bp, int free_hdr)
{

    if (bp)
    {
	struct datab *dp = bp->b_datap;
	dblk_t       *mydp = (dblk_t *) &((struct mdbblock *)bp)->datblk ;
	char	     *db_base = NULL ;
	int	      locked = 0 ;
	lis_flags_t   psw;

    	if (dp)
	{
	    if ((locked = (dp->db_ref != 1)))
		lis_spin_lock_irqsave(&lis_msg_lock, &psw) ;
	    if (dp -> db_ref == 0)
	    {
		if (locked)
		    lis_spin_unlock_irqrestore(&lis_msg_lock, &psw) ;
		printk ("lis_freeb: mp 0x%lx ref count 0\n", (long) bp);
		return;
	    }

	    /*
	     * dp could point to bp's data block or to an external one
	     * Sometimes the mblk/dblk structure is used just for the
	     * dblk to hold onto an external data buffer.  Sometimes the
	     * data buffer is just after the dblk (smaller than FASTBUF).
	     *
	     * The db_ref count is used to count references to the
	     * data block and also for the mblk header itself.  If we
	     * are called with "free_hdr" set to false then we will
	     * take the buffer freeing action but re-increment the db_ref
	     * because the header is still in use.
	     *
	     * An mblk with an external b_datap needs to have its own
	     * local db_ref decremented if free_hdr is set to true.  In
	     * this case "dp" will point to the external header and
	     * "mydp" will point to the dblk inside the "bp" mblk.
	     */
	    if (--dp->db_ref == 0)
            {
		if (locked)
		    lis_spin_unlock_irqrestore(&lis_msg_lock, &psw) ;
	    	if (dp->frtnp)
		{
		    if ( LIS_DEBUG_FREEMSG && LIS_DEBUG_ADDRS )
		       printk("lis_freeb: (user data blk) dp=0x%lx(extl)"
		              " dp->db_base=0x%lx dp->db_lim=0x%lx"
			      " dp->db_size=%d\n",
			       (long) dp, (long) dp->db_base,
			       (long) dp->db_lim, dp->db_size);

		    /*
		     * esballoc used the dblk inside the mblk to hold
		     * onto the buffer.  The db_base was pointed to the
		     * user's buffer, which we now free.  Setting db_size
		     * to zero makes the remaining mblk look like it has
		     * just an internal data buffer.  This allows it to
		     * be freed normally below.
		     */
	            (*(dp->frtnp->free_func))(dp->frtnp->free_arg);
		    dp->db_size = 0 ;
		    /*
		     * Now that the b_datap has been restored, and the
		     * reference count for it is zero, just keep going
		     * as for a non-esballoc type mblk/dblk.
		     */
		}

		if (dp->db_size > FASTBUF)
	    	{				/* separate data buffer */
		    if ( LIS_DEBUG_FREEMSG && LIS_DEBUG_ADDRS  )
			printk("lis_freeb: (data blk) dp=0x%lx(extl)"
		               " dp->db_base=0x%lx dp->db_lim=0x%lx"
			       " dp->db_size=%d\n",
			       (long) dp, (long) dp->db_base,
			       (long) dp->db_lim, dp->db_size);

	            K_ATOMIC_SUB(&lis_strcount, dp->db_size);
	            LisDownCount(DATABS);
		    LisDownCounter(DBLKMEM, dp->db_size);
		    db_base = dp->db_base ;	/* area to free */
		    dp->db_base = NULL ;	/* squash pointer to it */
	        }

		/*
		 * The data block holding onto this area may not be
		 * in the 'bp' buffer.  If not, then free the msgblk
		 * that contains the data block since its ref count
		 * is now zero.
		 *
		 * We get to the msgblk by starting at the embedded
		 * data block and backing up over the struct mbinfo
		 * that precedes the data block.
		 *
		 * Note that if this was the internal data buffer we
		 * have not actually freed anything at this point.  Unless
		 * we are supposed to free the header we re-increment
		 * the db_ref because the header is still in use.
		 */
		if (dp == mydp)			/* data blk inside hdr */
		{
		    if (!free_hdr)
			dp->db_ref++ ;		/* hdr still in use */
		}
		else
		{				/* data blk external */
		    mblk_t		*dpbp ;

		    dpbp = (mblk_t *) ( ((struct mbinfo *)dp) - 1 ) ;
		    if ( LIS_DEBUG_FREEMSG && LIS_DEBUG_ADDRS )
			printk("lis_freeb: (mblk[dblk]) mp=0x%lx dp=0x%lx\n",
					(long) dpbp, (long) dp);
		    freehdr(dpbp);		/* the hdr containing dblk */
		    bp->b_datap = mydp ;	/* point back to bp's dblk */
		    if (free_hdr && mydp->db_ref > 0)
			--mydp->db_ref ;	/* decr bp's ref cnt */
		}

		/*
		 * Either the data block is internal to the mblk and will
		 * be freed along with the header, or the data area was
		 * external and has already been freed by the code just
		 * above.
		 */

		if (free_hdr && mydp->db_ref == 0)
		{
		    if ( LIS_DEBUG_FREEMSG && LIS_DEBUG_ADDRS  )
			printk("lis_freeb: (mblk) bp=0x%lx dp=0x%lx\n",
			(long) bp, (long) bp->b_datap);

		    freehdr(bp); 	/* free the header */
		}

		if (db_base)		/* have saved data area to free */
		    FREE(db_base);		/* free data area */
		return ;
	    }


	    /* 
	     * Note, still holding the msg_lock.
	     *
	     * If the ref count is non-zero then check to see if the
	     * data block is in the same message as 'bp'.  If not then
	     * it is safe to free 'bp' but not the msg block in which
	     * the data block resides.  That will be done later when
	     * the ref count goes to zero.
	     */
	    if (free_hdr && dp != mydp)	/* external dblk */
	    {
		if (mydp->db_ref > 0 && --mydp->db_ref == 0)
		{
		    if (locked)
			lis_spin_unlock_irqrestore(&lis_msg_lock, &psw) ;
		    if ( LIS_DEBUG_FREEMSG && LIS_DEBUG_ADDRS  )
			printk("lis_freeb: (mblk) bp=0x%lx dp=0x%lx\n",
				(long) bp, (long) bp->b_datap);

		    freehdr(bp);
		    return ;
		}
	    }

	    if (!free_hdr && dp == mydp)/* internal dblk */
		dp->db_ref++ ;		/* hdr still in use */

	    if (locked)
		lis_spin_unlock_irqrestore(&lis_msg_lock, &psw) ;
	    return ;
        }

        /* no data blk ptr, just free hdr */
	if (free_hdr && mydp->db_ref == 1)
	{
	    mydp->db_ref-- ;
	    if ( LIS_DEBUG_FREEMSG && LIS_DEBUG_ADDRS  )
		printk("lis_freeb: (mblk/no-dblk) bp=0x%lx dp=NULL\n",
			(long) bp);

	    freehdr(bp);	 /* no data block; just free the header */
	}
    }

}/*lis_freedb*/

/*  -------------------------------------------------------------------  */

/* freeb - Free data buffer and place message block on free list. Don't
 *      follow the continuation pointer.
 */
void _RP
lis_freeb(mblk_t *bp)
{
    lis_freedb(bp, 1) ;			/* free both hdr and data blk */

}/*lis_freeb*/

/*  -------------------------------------------------------------------  */
/* freemsg - free a whole message
 */
void _RP
lis_freemsg(mblk_t *mp)
{
    while (mp)
    {
	mblk_t *now = mp;
	mp = mp->b_cont;
	lis_freedb(now, 1);
    }
}/*lis_freemsg*/

/*  -------------------------------------------------------------------  */
/* extract values from a char pointer and advance the pointer */
int _RP lis_getint(unsigned char **p)
{
    int result = *((int*)(*p));
    *p += sizeof(int);
    return(result);
}

/*  -------------------------------------------------------------------  */
void _RP lis_putbyte(unsigned char **p, unsigned char byte)
{
    **p = byte;
    ++*p;
}

/*----------------------------------------------------------------------
# Local Variables:      ***
# change-log-default-name: "~/src/prj/streams/src/NOTES" ***
# End: ***
  ----------------------------------------------------------------------*/

