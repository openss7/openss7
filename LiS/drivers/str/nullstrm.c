/*                               -*- Mode: C -*- 
 * nullstrm.c --- Streams NULL driver
 * Author          : David Grothe
 * Created On      : Fri Apr 25 15:57:15 CDT 1997
 * Last Modified By: David Grothe
 * RCS Id          : 
 * Purpose         : data source/sink
 * ----------------______________________________________________
 *
 *    Copyright (C) 1997  David Grothe, Gcom, Inc
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
 *    gram@aztec.co.za, nemo@ordago.uc3m.es
 */

#ident "@(#) LiS nullstrm.c 2.3 09/13/04 10:12:31 "

/*  -------------------------------------------------------------------  */


#if 0
#if defined(LINUX)
#   include <linux/types.h>
#   include <linux/ioctl.h>
#else
#   include <sys/types.h>
#   include <sys/ioctl.h>
#endif
#endif

#include <sys/stream.h>
#include <sys/stropts.h>


/*  -------------------------------------------------------------------  */
/*			  Module definition structs                      */

/* Module info for the loopback driver
 */
static struct module_info null_minfo =
{
  0,				/* id */
  "null",			/* name */
  0,				/* min packet size accepted */
  INFPSZ,			/* max packet size accepted */
  10240L,			/* high water mark */
  512L				/* low water mark */
};

/* These are the entry points to the driver: open, close, write side put and
 * service procedures and read side service procedure.
 */
static int   _RP null_open  (queue_t *,dev_t*,int,int, cred_t *);
static int   _RP null_close (queue_t *, int, cred_t *);
static int   _RP null_wput  (queue_t *, mblk_t *);
static int   _RP null_rsrv  (queue_t *);

/* qinit structures (rd and wr side) 
 */
static struct qinit null_rinit =
{
  NULL,				/* put */       
  null_rsrv,			/* service  */  
  null_open,			/* open */      
  null_close,			/* close */     
  NULL,				/* admin */     
  &null_minfo,			/* info */      
  NULL				/* stat */      
};

static struct qinit null_winit =
{
  null_wput,                    /* put */       
  NULL, 			/* service  */  
  NULL, 			/* open */      
  NULL, 			/* close */     
  NULL, 			/* admin */     
  &null_minfo, 			/* info */      
  NULL				/* stat */      
};

/* streamtab for the loopback driver.
 */
struct streamtab null_info =
{
  &null_rinit,			/* read queue */
  &null_winit,			/* write queue */
  NULL,				/* mux read queue  */
  NULL				/* mux write queue */
};

/*  -------------------------------------------------------------------  */
/*			    Module implementation                        */

#include <sys/LiS/nullstrm.h>

/*  -------------------------------------------------------------------  */

#define NDEVS 8			/* number of clone devs supported */

struct null			/* info for each null device */
{
  queue_t	*rdq;
  queue_t	*wrq;
  int		 read_size ;	/* nr bytes in upstream messages */
  int		 timr_hndl ;
  int		 minor_nr ;
};

/* Arry.  w/ opened devices info & number of openened devices
 */
static struct null null_devs[NDEVS];
static int null_cnt = NDEVS;

/*  -------------------------------------------------------------------  */


static int _RP
null_open (queue_t *q, dev_t *devp, int flag, int sflag, cred_t *credp)
{
  struct null *null;
  dev_t		dev ;

  if (sflag == CLONEOPEN) {
    for (dev = 1; dev < null_cnt; dev++) {
      if (null_devs[dev].rdq == NULL)
	break;
    }
  } else
    dev = minor (*devp);

  if (dev >= null_cnt)
    return OPENFAIL;

  *devp = makedevice(major(*devp), dev) ;
  if (q->q_ptr)
    return 0;					/* success */

  null = &null_devs[dev];
  WR (q)->q_ptr = (char *) null;
  q->q_ptr = (char *) null;
  null->wrq = WR (q);
  null->rdq = RD (q);
  null->read_size = 10 ;		/* default size for upstrm data */
  null->minor_nr = dev ;
  qenable(null->rdq) ;			/* start sending messages upstream */

#ifdef STREAMS_DEBUG
  printk("nullstr: Opened %x\n",dev);
#endif

  return 0;					/* success */
}

/*  -------------------------------------------------------------------  */

int	_RP null_timeout(caddr_t arg)
{
    struct null		*null = (struct null *) arg ;

    if (null->rdq != NULL)
    {
	enableok(null->rdq) ;	/* allow qenable to work */
	qenable(null->rdq) ;
    }
    return(0);

} /* null_timeout */

/*  -------------------------------------------------------------------  */
/*
 * null_bufcall
 *
 * Called from the STREAMS bufcall mechanism.
 *
 * The argument is a pointer to a queue to enable.
 */
void _RP null_bufcall(long q_ptr)
{
    enableok((queue_t *) q_ptr) ;	/* allow qenable to work */
    qenable((queue_t *) q_ptr) ;

} /* null_bufcall */


/*  -------------------------------------------------------------------  */

static int _RP
null_wput (queue_t *q, mblk_t *mp)
{
  struct null *null;
  int		rtn_count = 0 ;

  null = (struct null *) q->q_ptr;

  switch (mp->b_datap->db_type) {
  case M_IOCTL:{
    struct iocblk *iocb;
    int error;

    iocb = (struct iocblk *) mp->b_rptr;

    switch (iocb->ioc_cmd) 
    {

    case NULL_READ_SIZE:
	null->read_size = *(int *) mp->b_cont->b_rptr;
	break ;


    case NULL_GET_DEV:				/* get minor number */
	{
	    int		*devp ;

	    if (iocb->ioc_count != sizeof (int))
	    {
		printk ("Expected ioctl len %d, got %d\n",
			 sizeof (int), iocb->ioc_count);

		error = EINVAL;
		goto iocnak;
	    }

	    devp = (int *) mp->b_cont->b_rptr ;
	    *devp = null->minor_nr ;		/* return minor device nr */
	    rtn_count = sizeof(int) ;
	}
	break ;

    default:
      printk ("invalid ioctl %x\n", iocb->ioc_cmd);
      error = EINVAL;
    iocnak:{
	mp->b_datap->db_type = M_IOCNAK;
	iocb->ioc_error = error;
	qreply (q, mp);
      }
      return(0) ;
    }				/* end of switch on ioc_cmd */

    mp->b_datap->db_type = M_IOCACK;
    iocb->ioc_count = rtn_count;
    qreply (q, mp);

    break;
  }
  case M_FLUSH:{
    if (*mp->b_rptr & FLUSHW)
      flushq (q, FLUSHDATA);
    if (*mp->b_rptr & FLUSHR) {
      flushq (RD (q), FLUSHDATA);
      *mp->b_rptr &= ~FLUSHW;
      qreply (q, mp);
    } else
      freemsg (mp);
    break;
  }
  default:
      freemsg (mp);			/* NULL driver action */
      break;
    break;
  
  }
  return(0) ;
}


/*  -------------------------------------------------------------------  */

static int _RP
null_rsrv (queue_t *q)
{
    struct null *null;
    mblk_t	*mp ;

    null = (struct null *) q->q_ptr;
    if (null->rdq == NULL)
	return(0);

    while (   canputnext(null->rdq)
	   && (mp = allocb(null->read_size, BPRI_MED)) != NULL
	  )
    {
	mp->b_wptr += null->read_size ;
	putnext(q, mp) ;
    }
    return(0) ;
}

/*  -------------------------------------------------------------------  */

static int _RP
null_close (queue_t *q, int dummy, cred_t *credp)
{
  struct null *null;

  null = (struct null *) q->q_ptr;
  null->rdq = NULL;
  null->wrq = NULL;

  if (null->timr_hndl != 0)
  {
      untimeout(null->timr_hndl) ;
      null->timr_hndl = 0 ;
  }

  flushq (q, FLUSHALL);
  flushq (WR (q), FLUSHALL);

  return(0);
}

#if 0
void	null_register()
{
    register_strdev(	0,		/* major (allocate for us) */
			&null_info,	/* streamtab entry */
			NDEVS,		/* number of minors */
			"null"		/* name */
		   ) ;
}
#endif
