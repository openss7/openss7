/************************************************************************
*                      Miniature Multiplexor                            *
*************************************************************************
*									*
* This  driver is a small multiplexor whose purpose is to facilitate	*
* testing of the I_LINK and I_UNLINK code in STREAMS.			*
*									*
* Author:	David Grothe <dave@gcom.com>				*
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

#ident "@(#) LiS minimux.c 2.10 09/13/04 10:12:31 "

#include <sys/stream.h>
#include <sys/cmn_err.h>
#include <sys/LiS/minimux.h>
#include <sys/osif.h>


/************************************************************************
*                             Macros                                    *
************************************************************************/

#ifndef canputnext
#define	canputnext(q)		canput((q)->q_next)
#endif



/************************************************************************
*                          Table Structure                              *
*************************************************************************
*									*
* As such, this multiplexor connects an upper stream with a lower	*
* stream such that messages arriving in a downstream direction at	*
* upper stream "A" are routed to lower stream "B".  Upstream messages	*
* arriving at "B" will be routed to "A" by default but all this		*
* can be changed via ioctls so as to make the topologies interesting	*
* and complicated (for testing, of course).				*
*									*
* list		We thread all entries together.				*
*									*
* outq		The STREAMS queue that flows out of the multiplexor.	*
*		If this is a lower entry then this will be the lower	*
*		write queue.  If it is an upper entry then this will	*
*		be the upper read queue.				*
*									*
* othermux	Pointer to the mux table entry for the other stream	*
*		to which this stream is connected.			*
*									*
* ctl_strm	This field is filled in for lowers.  It points to the	*
*		mux table entry for the control stream on which the	*
*		I_LINK arrived.						*
*									*
* muxid		The l_index carried in the I_LINK for lower entries.	*
*									*
* minor_dev	Minor device number for upper entries.			*
*									*
************************************************************************/
typedef struct mux_tbl
{
    struct mux_tbl	*list ;
    queue_t		*outq ;
    struct mux_tbl	*othermux ;
    struct mux_tbl	*ctl_strm ;
    int			 muxid ;
    int			 minor_dev ;

} mux_tbl_t ;


/************************************************************************
*                              Storage                                  *
************************************************************************/

mux_tbl_t	*mux_head ;		/* head of mux tbl entry thread */

#define	MAXMUX		100		/* max number of minors open */
int		 mux_minors[MAXMUX] ;	/* index by minor, in-use flags */

/************************************************************************
*                         Streamtab Structure                           *
*************************************************************************
*									*
* The following is the streamtab structure for the multiplexor.		*
*									*
************************************************************************/
static struct module_info mux_minfo =
{
  0,				/* id */
  "mini-mux",			/* name */
  0,				/* min packet size accepted */
  INFPSZ,			/* max packet size accepted */
  10240L,			/* high water mark */
  512L				/* low water mark */
};

/*
 * Driver entry points -- upper
 */
static int   _RP mux_open  (queue_t *,dev_t*,int,int, cred_t *);
static int   _RP mux_close (queue_t *, int, cred_t *);
static int   _RP mux_wput  (queue_t *, mblk_t *);
static int   _RP mux_rsrv  (queue_t *);
/*
 * Driver entry points -- lower
 */
static int   _RP mux_lrput (queue_t *, mblk_t *);
static int   _RP mux_lwsrv (queue_t *);

/*
 * qinit structures (rd and wr side, upper) 
 */
static struct qinit mux_rinit =
{
  NULL,				/* put */       
  mux_rsrv,			/* service  */  
  mux_open,			/* open */      
  mux_close,			/* close */     
  NULL,				/* admin */     
  &mux_minfo,			/* info */      
  NULL				/* stat */      
};

static struct qinit mux_winit =
{
  mux_wput,                    /* put */       
  NULL, 			/* service  */  
  NULL, 			/* open */      
  NULL, 			/* close */     
  NULL, 			/* admin */     
  &mux_minfo, 			/* info */      
  NULL				/* stat */      
};

/*
 * qinit structures (rd and wr side, lower) 
 */
static struct module_info mux_lminfo =
{
  0,				/* id */
  "mini-mux-lwr",		/* name */
  0,				/* min packet size accepted */
  INFPSZ,			/* max packet size accepted */
  10240L,			/* high water mark */
  512L				/* low water mark */
};

static struct qinit mux_lrinit =
{
  mux_lrput,			/* put */       
  NULL,				/* service  */  
  NULL,				/* open */      
  NULL,				/* close */     
  NULL,				/* admin */     
  &mux_lminfo,			/* info */      
  NULL				/* stat */      
};

static struct qinit mux_lwinit =
{
  NULL,   	                /* put */       
  mux_lwsrv, 			/* service  */  
  NULL, 			/* open */      
  NULL, 			/* close */     
  NULL, 			/* admin */     
  &mux_lminfo, 			/* info */      
  NULL				/* stat */      
};

/* streamtab for the loopback driver.
 */
struct streamtab mux_info =
{
  &mux_rinit,			/* read queue */
  &mux_winit,			/* write queue */
  &mux_lrinit,			/* mux read queue  */
  &mux_lwinit			/* mux write queue */
};

/************************************************************************
*                               new_mux                                 *
*************************************************************************
*									*
* Allocate a new mux structure and link it into the master list.	*
*									*
************************************************************************/
static mux_tbl_t *
new_mux(void)
{
    mux_tbl_t	*muxp ;

    muxp = (mux_tbl_t *) ALLOC(sizeof(*muxp)) ;
    if (muxp == NULL)
	return(NULL) ;

    memset(muxp, 0, sizeof(*muxp)) ;
    muxp->list		= mux_head ;
    mux_head		= muxp ;

    return(muxp) ;

} /* new_mux */

/************************************************************************
*                          mux_open                                     *
*************************************************************************
*									*
* Open routine for the multiplexor.					*
*									*
************************************************************************/
static int _RP
mux_open (queue_t *q, dev_t *devp, int flag, int sflag, cred_t *credp)
{
    mux_tbl_t	*muxp;
    int  	 dev ;

    if (sflag == CLONEOPEN)
    {
	for (dev = 1; dev < MAXMUX; dev++)
	{
	    if (mux_minors[dev] == 0)
		break;
	}
    }
    else
	dev = MINOR (*devp);

    if (dev >= MAXMUX)
	return (OPENFAIL);

    mux_minors[dev] = 1 ;			/* minor in use */
    *devp = makedevice(getmajor(*devp), dev) ;

    if (q->q_ptr != NULL)			/* already open */
	return(0);				/* success (2nd open) */

    muxp = new_mux() ;
    if (muxp == NULL)
	return(ENOMEM) ;

    muxp->outq		= RD(q) ;		/* upstream read queue */
    muxp->minor_dev	= dev ;
    RD(q)->q_ptr	= muxp ;
    WR(q)->q_ptr	= muxp ;

    return(0);					/* success */

} /* mux_open */

/************************************************************************
*                             mux_ioctl                                 *
*************************************************************************
*									*
* Handle ioctl from upstream.						*
*									*
* For I_LINK the linkage is set up between the lower queue and the	*
* queue on which the ioctl arrives.  These two queues then relay	*
* messages upstream and downstream to each other.			*
*									*
************************************************************************/
void	mux_ioctl(mux_tbl_t *muxp, mblk_t *mp)
{
    struct iocblk	*iocb;
    struct linkblk	*lk ;
    int			 error;
    int			 rtn_count = 0 ;
    int			 uplink = 0 ;

    iocb = (struct iocblk *) mp->b_rptr;

    switch (iocb->ioc_cmd)
    {
    case I_LINK:
    case I_PLINK:
	{
	    mux_tbl_t	*lwr ;

	    if (mp->b_cont == NULL)		/* no linkblk */
	    {
		error = EINVAL;
		goto iocnak ;
	    }

	    lk = (struct linkblk *) mp->b_cont->b_rptr ;
	    lwr = new_mux() ;
	    if (lwr == NULL)
	    {
		error = ENOMEM ;
		goto iocnak ;
	    }

	    lwr->ctl_strm	 = muxp ;	    /* remember control strm */
	    lwr->othermux	 = muxp ;	    /* lower & upper connected */
	    muxp->othermux	 = lwr ;
	    lwr->muxid		 = lk->l_index ;    /* muxid of lower */
	    lwr->outq		 = lk->l_qbot ;	    /* lower write queue */
	    RD(lwr->outq)->q_ptr = lwr ;	    /* hook queues to tbl */
	    WR(lwr->outq)->q_ptr = lwr ;
	    qenable(RD(lwr->outq)) ;		    /* run service procedure */
	    cmn_err(CE_CONT, "mux_ioctl: I_LINK dev %u to muxid %u\n",
			     muxp->minor_dev, lk->l_index) ;
	}
	break ;

    case I_UNLINK:
    case I_PUNLINK:
	{
	    mux_tbl_t	*lwr ;
	    mux_tbl_t	*prev ;

	    if (mp->b_cont == NULL)		/* no linkblk */
	    {
		error = EINVAL;
		goto iocnak ;
	    }

	    /*
	     * Find entry with given muxid.  Assume that muxids are
	     * unique only within a given control stream.
	     */
	    lk = (struct linkblk *) mp->b_cont->b_rptr ;
	    prev = NULL ;
	    for (lwr = mux_head; lwr != NULL; lwr = lwr->list)
	    {
		if (   lwr->ctl_strm == muxp		/* this is ctl strm */
		    && lwr->muxid == lk->l_index	/* and this muxid */
		   )
		{
		    break ;				/* that's the entry */
		}

		prev = lwr ;
	    }

	    if (lwr == NULL)			/* not found */
	    {
		cmn_err(CE_CONT, "mux_ioctl: I_UNLINK muxid %u: unknown muxid\n",
				lk->l_index) ;
		error = EINVAL ;
		goto iocnak ;
	    }

	    cmn_err(CE_CONT, "mux_ioctl: I_UNLINK minor=%u -> muxid=%u\n",
			     muxp->minor_dev, lk->l_index) ;
	    if (prev == NULL)			/* 1st in list */
		mux_head = lwr->list ;		/* link around it */
	    else				/* middle of list */
		prev->list = lwr->list ;	/* link around it */

	    if (lwr->othermux != NULL && lwr->othermux->othermux == lwr)
		lwr->othermux->othermux = NULL ;/* detach from lower */

	    if (muxp->othermux == lwr)
		muxp->othermux	 = NULL ;	/* detach from lower */

	    RD(lwr->outq)->q_ptr = NULL ;	/* clobber q ptr */
	    WR(lwr->outq)->q_ptr = NULL ;
	    FREE(lwr) ;				/* free structure */
	}
	break ;

    case MINIMUX_UP:				/* point lower to upper */
	uplink = 1 ;	
    case MINIMUX_DOWN:				/* point upper to lower */
	{
	    mux_tbl_t	*lwr ;
	    int		 l_index ;

	    if (iocb->ioc_count != sizeof(int))
	    {
		error = EINVAL ;
		goto iocnak ;
	    }

	    l_index = *( (int *) mp->b_cont->b_rptr ) ;
	    for (lwr = mux_head; lwr != NULL; lwr = lwr->list)
	    {
		if (lwr->ctl_strm == muxp && lwr->muxid == l_index) break ;
	    }

	    if (lwr == NULL)			/* not found */
	    {
		cmn_err(CE_CONT,
			"mux_ioctl: MINIMUX_DOWN muxid %u: unknown muxid\n",
				l_index) ;
		error = EINVAL ;
		goto iocnak ;
	    }

	    if (uplink)				/* point lwr to upper */
		lwr->othermux = muxp ;		/* lwr point up at us */
	    else				/* point upper to lower */
		muxp->othermux = lwr ;		/* we point down to lower */
	    					/* this lower does not    */
	    					/* necessarily point back */
	    					/* up to us.		  */
	}
	break ;

    default:
        error = EINVAL;
    iocnak:
	mp->b_datap->db_type = M_IOCNAK;
	iocb->ioc_error = error;
	iocb->ioc_count = rtn_count;
	putnext (muxp->outq, mp);		/* send back upstream */
	return ;
    }				/* end of switch on ioc_cmd */

    mp->b_datap->db_type = M_IOCACK;
    iocb->ioc_count = rtn_count;
    putnext (muxp->outq, mp);

} /* mux_ioctl */

/************************************************************************
*                             mux_wput                                  *
*************************************************************************
*									*
* Upper wput routine.							*
*									*
************************************************************************/
static int _RP
mux_wput (queue_t *q, mblk_t *mp)
{
    queue_t	*fwdq ;
    mux_tbl_t	*muxp ;

    if (q->q_ptr == NULL)
    {
	freemsg(mp) ;
	return(0) ;
    }

    muxp = q->q_ptr ;
    if (muxp->othermux != NULL)		/* linked to downstream queue */
	fwdq = muxp->othermux->outq ;	/* forward downstream */
    else				/* no downstream linkage */
	fwdq = muxp->outq ;		/* forwared back upstream */

    switch (mp->b_datap->db_type)
    {
    case M_DATA:
    case M_PROTO:
    case M_PCPROTO:
	if (canputnext(fwdq))		/* check flow control */
	    putnext(fwdq, mp) ;
	else
	    putqf(fwdq, mp) ;
	break ;

    case M_IOCTL:
	mux_ioctl(muxp, mp) ;
	break ;

    case M_FLUSH:			/* flush upper queue */
	if (*mp->b_rptr & FLUSHW)
	    flushq (q, FLUSHDATA);

	if (muxp->othermux != NULL && muxp->othermux->outq != NULL)
	{				/* if connected, pass downstream */
	    flushq (muxp->othermux->outq, FLUSHDATA);	/* lwr write q */
	    putnext(muxp->othermux->outq, mp);
	}
	else				/* not connected */
	if (*mp->b_rptr & FLUSHR)
	{
	    flushq (RD(q), FLUSHDATA);
	    *mp->b_rptr &= ~FLUSHW;
	    qreply (q, mp);		/* reply as lowest driver */
	}
	else
	    freemsg (mp);

	break;
    }

    return(0) ;

} /* mux_wput  */


/************************************************************************
*                        mux_messenger_service                          *
*************************************************************************
*									*
* Service a queue by attempting to forward messages subject to flow	*
* control constraints.							*
*									*
************************************************************************/
static void
mux_messenger_service(queue_t *q)
{
    mux_tbl_t	*muxp ;
    mblk_t	*mp;

    muxp = (mux_tbl_t *) q->q_ptr;

    while ((mp = getq(q)) != NULL)
    {
	if (   mp->b_datap->db_type <= QPCTL
	    && !canputnext(q)
	   )
	{
	  putbqf(q, mp);
	  break;
	}

	putnext(q, mp);
    }

} /* mux_messenger_service */

#if 0
/************************************************************************
*                              mux_wsrv                                 *
*************************************************************************
*									*
* Upper write service routine.	Unused.					*
*									*
************************************************************************/
static int _RP
mux_wsrv (queue_t *q)
{
    return(0) ;
} /* mux_wsrv */
#endif


/************************************************************************
*                             mux_rsrv                                  *
*************************************************************************
*									*
* Upper read service procedure.						*
*									*
************************************************************************/
static int _RP
mux_rsrv (queue_t *q)
{
    mux_messenger_service(q) ;
    return(0) ;

} /* mux_rsrv  */


/************************************************************************
*                             mux_close                                 *
*************************************************************************
*									*
* Close routine.							*
*									*
************************************************************************/
static int _RP
mux_close (queue_t *q, int dummy, cred_t *credp)
{
    mux_tbl_t	*muxp ;
    mux_tbl_t	*prev ;

    if (q->q_ptr == NULL) return(0) ;

    prev = NULL ;
    for (muxp = mux_head; muxp != NULL; muxp = muxp->list)
    {
	if (muxp == (mux_tbl_t *) q->q_ptr) break ;	/* found it */
	prev = muxp ;
    }

    if (muxp == NULL) return(0) ;			/* not found */

    cmn_err(CE_CONT, "mux_close: minor=%d", muxp->minor_dev) ;
    if (muxp->othermux != NULL)
	cmn_err(CE_CONT, " -> muxid=%d\n", muxp->othermux->muxid) ;
    else
	cmn_err(CE_CONT, " no lower\n") ;

    if (prev == NULL)			/* 1st in list */
	mux_head = muxp->list ;		/* link around it */
    else				/* middle of list */
	prev->list = muxp->list ;	/* link around it */

    if (muxp->othermux != NULL && muxp->othermux->othermux == muxp)
	muxp->othermux->othermux = NULL ;/* detach from upper */
					 /* maybe orphans the lower */

    RD(muxp->outq)->q_ptr = NULL ;	/* clobber q ptr (cheap insurance) */
    WR(muxp->outq)->q_ptr = NULL ;
    q->q_ptr		  = NULL ;
    mux_minors[muxp->minor_dev] = 0 ;	/* make minor available */

    FREE(muxp) ;			/* free structure */

    return 0 ;
} /* mux_close */

/************************************************************************
*                           mux_lrput                                   *
*************************************************************************
*									*
* Lower read put procedure.  Receives messages from driver below.	*
*									*
************************************************************************/
static int _RP
mux_lrput (queue_t *q, mblk_t *mp)
{
    queue_t	*fwdq ;
    mux_tbl_t	*muxp ;

    if (q->q_ptr == NULL)
    {
	freemsg(mp) ;
	return(0) ;
    }

    muxp = q->q_ptr ;
    if (muxp->othermux != NULL)		/* linked to upstream queue */
	fwdq = muxp->othermux->outq ;	/* forward upstream */
    else				/* no upstream linkage */
	fwdq = muxp->outq ;		/* forwared back downstream */

    switch (mp->b_datap->db_type)
    {
    case M_DATA:
    case M_PROTO:
    case M_PCPROTO:
	if (canputnext(fwdq))		/* check flow control */
	    putnext(fwdq, mp) ;
	else
	    putqf(fwdq, mp) ;
	break ;

    case M_IOCTL:
	freemsg(mp) ;			/* ioctl from below */
	break ;

    case M_FLUSH:			/* flush lower queue */
	if (*mp->b_rptr & FLUSHR)
	    flushq (q, FLUSHDATA);

	if (muxp->othermux != NULL && muxp->othermux->outq != NULL)
	{				/* if connected, pass upstream */
	    flushq (muxp->othermux->outq, FLUSHDATA);	/* upper rd queue */
	    putnext(muxp->othermux->outq, mp);
	}
	else				/* not connected */
	if (*mp->b_rptr & FLUSHW)
	{
	    flushq (WR(q), FLUSHDATA);
	    *mp->b_rptr &= ~FLUSHR;
	    qreply (q, mp);		/* reply as highest driver */
	}
	else
	    freemsg (mp);

	break;

    default:
	freemsg(mp) ;			/* drop unknown messages */
	break ;
    }

    return(0) ;

} /* mux_lrput  */

/************************************************************************
*                              mux_lwsrv                                *
*************************************************************************
*									*
* Lower write service routine.						*
*									*
************************************************************************/
static int _RP
mux_lwsrv (queue_t *q)
{
    mux_messenger_service(q) ;
    return(0) ;

} /* mux_lwsrv */


#if 0
/************************************************************************
*                             mux_lrsrv                                 *
*************************************************************************
*									*
* Lower read service procedure.  Unused.				*
*									*
************************************************************************/
static int _RP
mux_lrsrv (queue_t *q)
{
    return(0) ;
} /* mux_lrsrv  */
#endif

