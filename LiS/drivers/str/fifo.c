/*
 *  fifo.c - FIFO pseudo-driver
 *
 *  This pseudo-driver is used for FIFOs and pipes.  Note that the queue
 *  pairs for this pseudo-driver become the stream head; thus the use of
 *  strrput and strwsrv in the qinit structures.
 *
 *  Since Linux has FIFOs and pipes implemented in the kernel (and they
 *  monopolize the S_IFIFO file type), this pseudo-driver manages FIFOs
 *  as character minor devices.  Since Linux also limits the number of
 *  minor devices per major number to 256, this pseudo-driver manages
 *  multiple major numbers in order to support more than 256 minor devices.
 *  They are list-managed, and so can be allocated/freed in effectively
 *  constant time.  The open()/close() entry points in this file serve
 *  primarily to support the FIFO minor device pool; the remainder of
 *  the necessary support for FIFOs and pipes is in the stream head code.
 *
 *  Minor device 0 for each major is a clonable; it will never be actually
 *  used as an open stream.  Thus, 255 minor numbers are available per
 *  major number.
 *
 *  Copyright (C) 2000  John A. Boyd Jr.  protologos, LLC
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 * 
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 * 
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the
 *  Free Software Foundation, Inc., 59 Temple Place - Suite 330, Cambridge,
 *  MA 02139, USA.
 */

#ident "@(#) LiS fifo.c 1.10 12/15/02"

#ifdef MODULE
#  if defined(LINUX) && defined(__KERNEL__)
#    ifdef MODVERSIONS
#     ifdef LISMODVERS
#      include <sys/modversions.h>	/* /usr/src/LiS/include/sys */
#     else
#      include <linux/modversions.h>
#     endif
#    endif
#    include <linux/module.h>
#  else
#    error This can only be a module in the Linux kernel environment
#  endif
#else
#  define MOD_INC_USE_COUNT  do {} while (0)
#  define MOD_DEC_USE_COUNT  do {} while (0)
#endif

#include <sys/LiS/config.h>

#include <sys/stream.h>
#include <sys/stropts.h>
#include <sys/errno.h>
#include <sys/cmn_err.h>

#include <sys/LiS/head.h>    /* for lis_strrput/lis_strwsrv */
#include <sys/lislocks.h>	/* semaphores */

#include <linux/list.h>
#include <sys/osif.h>

/*
 *  Some configuration sanity checks
 */
#ifndef FIFO_
#error Not configured
#endif

#ifndef FIFO__ID
#define FIFO__ID 0xfef0
#endif

#ifndef FIFO__CMAJORS
#error The number of major numbers must be defined
#endif

#ifndef FIFO__UNITS
#define FIFO__UNITS 256
#endif

#define  MOD_ID   FIFO__ID
#define  MOD_NAME "fifo"

/*
 *  per-minor device (i.e., per-open) info
 */
typedef struct _fifo_dev {
    short index;              /* self-index */
    short flags;              /* see below  */
    struct list_head list;    /* for free/used lists */
} fifo_dev_t;

#define FIFO_CLONABLE    0x01
#define FIFO_ALLOCATED   0x02

int fifo_units = FIFO__UNITS;     /* total number of units (i.e., minors) */

fifo_dev_t fifo_dev[FIFO__UNITS];  /* elems 0 mod minor() not used */

static LIST_HEAD(free_fifos);
static int num_free_fifos = 0;
#if 0
static LIST_HEAD(used_fifos);
static int num_used_fifos = 0;
#endif

extern int fifo__0_majors[];          /* in streams.o via modconf */

static lis_semaphore_t fifo_sem;  /* for allocating minor numbers */

static int fifo_initialized = 0;

/*
 *  function prototypes
 */
static int  fifo_open(queue_t *, dev_t*, int, int, cred_t *);
static int  fifo_close(queue_t *, int, cred_t *);

/*
 *  module structure
 */
static struct module_info fifo_minfo =
{ 
    MOD_ID,            /* id */
    MOD_NAME,          /* name */
    0,                 /* min packet size accepted */
    INFPSZ,            /* max packet size accepted */
    10240L,            /* highwater mark */
    512L               /* lowwater mark */
};

static struct qinit fifo_rinit = {
    lis_strrput,       /* put */
    lis_strrsrv,       /* service */
    fifo_open,         /* open */
    fifo_close,        /* close */
    NULL,              /* admin */
    &fifo_minfo,       /* info */
    NULL               /* stat */
};

static struct qinit fifo_winit = { 
    NULL,              /* put */
    lis_strwsrv,       /* service */
    NULL,              /* open */
    NULL,              /* close */
    NULL,              /* admin */
    &fifo_minfo,       /* info */
    NULL               /* stat */
};

struct streamtab fifo_info =
{
    &fifo_rinit,       /* read queue init */
    &fifo_winit,       /* write queue init */
    NULL,              /* lower mux read queue init */
    NULL               /* lower mux write queue init */
};

static inline int fifo_major_index( int maj )
{
    int idx;

    for (idx = 0;  idx < FIFO__CMAJORS;  idx++)
	if (fifo__0_majors[idx] == maj)
	    return(idx);

    return(-ENOENT);
}


static inline int fifo_unit_index( dev_t dev )
{
    int idx = fifo_major_index(MAJOR(dev));
    dev_t maxminor_dev = MKDEV(0,0x0000ffff) ;

    if (idx < 0)
	return(idx);

    idx = idx * (MINOR(maxminor_dev)+1) + MINOR(dev) ;

    if (idx >= fifo_units)
	return(-ENXIO);

    return(idx);
}

static dev_t fifo_mkdev(int dev_major, int unit_index)
{
    dev_t maxminor_dev = MKDEV(0,0x0000ffff) ;
    int   maxminor = MINOR(maxminor_dev)+1 ;

    return(MKDEV(dev_major + (unit_index / maxminor),
					    unit_index % maxminor)) ;
}


static int fifo_alloc( int idx )
{
    fifo_dev_t *fdp;

    if (idx > fifo_units)
	return -ENXIO;
    if (list_empty(&free_fifos))
	return -EMFILE;

    if (idx < 0) {
	struct list_head *e = free_fifos.next;
	fdp = list_entry( e, fifo_dev_t, list );
    } else
	fdp = &(fifo_dev[idx]);

    if (fdp->flags & FIFO_ALLOCATED)
	return -EBUSY;

    fdp->flags |= FIFO_ALLOCATED;

    list_del(&(fdp->list));  num_free_fifos--;
#if 0
    list_add( &(fdp->list), &(used_fifos) );  num_used_fifos++;
#endif

    return fdp->index;
}


static void fifo_free( int idx )
{
    fifo_dev_t *fdp;

    if (idx < 0 || idx >= fifo_units)  return;

    fdp = &(fifo_dev[idx]);

    if (!(fdp->flags & FIFO_ALLOCATED))  return;

    fdp->flags &= ~FIFO_ALLOCATED;
#if 0
    list_del(&(fdp->list));  num_used_fifos--;
#endif
    list_add( &(fdp->list), free_fifos.prev );  num_free_fifos++;
}

/*
 *  open
 */
static int fifo_open( queue_t *q, dev_t *devp,
		      int flag, int sflag,
		      cred_t *credp )
{
    int m;
    int dev_major ;

#ifdef FIFO_DEBUG
    cmn_err( CE_CONT,
	     "%s_open( 0x%p, 0x%x, 0x%x, 0x%x, ... )\n",
	     MOD_NAME, q, *devp, flag, sflag );
#endif

    lis_down(&fifo_sem);

    /*
     *  we allow a clone open both ways: 0 minors are clonables
     */
    m = (sflag == CLONEOPEN ? 0 : fifo_unit_index(*devp));
    dev_major = MAJOR(*devp) ;

    if (m < 0) {
	lis_up(&fifo_sem);
	return(m);     /* m is an error code */
    }

    if (fifo_dev[m].flags & FIFO_CLONABLE)  {
	/*
	 *  select any unused unit
	 */
	if ((m = fifo_alloc(-1)) < 0)  {
	    lis_up(&fifo_sem);
	    return(m);   /* m is an error code */
	}
    } else {
	/*
	 *  select the requested unit if unused
	 */
	if (!(q->q_ptr)) {
	    if ((m = fifo_alloc(m)) < 0) {
		lis_up(&fifo_sem);
		return(m);
	    }
	}
    }

    if (!q->q_ptr) {
	RD(q)->q_ptr = WR(q)->q_ptr = (void *) &(fifo_dev[m]);

	/*
	 *  simulate a FIFO by linking WR(q) -> RD(q);  the head might
	 *  also do this, but just in case...
	 */
	WR(q)->q_next = RD(q);

	qprocson(q);

	MOD_INC_USE_COUNT;
    }

    *devp = fifo_mkdev(dev_major, m) ;

    lis_up(&fifo_sem);

#ifdef FIFO_DEBUG
    cmn_err( CE_CONT,
	     "%s_open( 0x%p, 0x%x, ... ) unit %d OK\n",
	     MOD_NAME, q, *devp, m );
#endif

    return 0;	/* OK */
}

/*
 *  close
 */
static int fifo_close( queue_t *q, int flag, cred_t *credp )
{
    fifo_dev_t *fdp = (fifo_dev_t *) q->q_ptr;

#ifdef FIFO_DEBUG
    cmn_err( CE_CONT,
	     "%s_close( 0x%p, 0x%x, ... ) unit %d\n",
	     MOD_NAME, q, flag, fdp->index );
#endif

    lis_down(&fifo_sem);
    fifo_free( fdp->index );

    /*
     *  undo FIFO/pipe queue linkage
     */
    if (WR(q)->q_next) {
	if (!SAMESTR(WR(q)) && (WR(WR(q)->q_next)->q_next == RD(q))) {
	    /*
	     *  pipe linkage - unlink other side - we need to do this
	     *  so that SAMESTR works on the second stream after the
	     *  first stream's queues are deallocated
	     */
	    WR(WR(q)->q_next)->q_next = NULL;
	}
	WR(q)->q_next = NULL;
    }
    RD(q)->q_ptr = WR(q)->q_ptr = NULL;

    lis_up(&fifo_sem);

    qprocsoff(q);

    MOD_DEC_USE_COUNT;

    return 0;	/* success */
}
 
#if 0
/*
 *  wput
 */
static void fifo_wput( queue_t *q, mblk_t *mp )
{
    fifo_dev_t *fdp = (fifo_dev_t *) q->q_ptr;

#ifdef FIFO_DEBUG
    cmn_err( CE_CONT,
	     "%s_wput( 0x%08x, 0x%08x ) unit %d\n", MOD_NAME,
	     q, mp, fdp->index );
#endif

    /*
     *  this is a driver - we should NAK IOCTLs we don't handle
     *  (which means all of them)
     */
    switch (mp->b_datap->db_type) {
    case M_IOCTL:
	mp->b_datap->db_type = M_IOCNAK;
	qreply( q, mp );
	break;
    default:
	putnext( q, mp );   /* this goes to our RD(q) */
	break;
    }
}

/*
 *  wsrv
 */
static void fifo_wsrv( queue_t *q )
{
    fifo_dev_t *fdp = (fifo_dev_t *) q->q_ptr;
    mblk_t *mp;

#ifdef FIFO_DEBUG
    cmn_err( CE_CONT,
	     "%s_wsrv( 0x%08x ) unit %d\n", MOD_NAME,
	     q, fdp->index );
#endif

    while ((mp = getq(q))) {
	/*
	 *  this is a driver - we should NAK IOCTLs we don't handle
	 *  (which means all of them)
	 */
	switch (mp->b_datap->db_type) {
	case M_IOCTL:
	    mp->b_datap->db_type = M_IOCNAK;
	    qreply( q, mp );
	    break;
	default:
	    putnext( q, mp );  /* this goes to our RD(q) */
	    break;
	}
    }
}

/*
 *  rput
 */
static void fifo_rput( queue_t *q, mblk_t *mp )
{
    fifo_dev_t *fdp = (fifo_dev_t *) q->q_ptr;

#ifdef FIFO_DEBUG
    cmn_err( CE_CONT,
	     "%s_rput( 0x%08x, 0x%08x ) unit %d\n", MOD_NAME,
	     q, mp, fdp->index );
#endif

    putnext( q, mp );
}

/*
 *  rsrv
 */
static void fifo_rsrv( queue_t *q )
{
    fifo_dev_t *fdp = (fifo_dev_t *) q->q_ptr;
    mblk_t *mp;

#ifdef FIFO_DEBUG
    cmn_err( CE_CONT, "%s_rsrv( 0x%08x ) unit %d\n", MOD_NAME,
	     q, fdp->index );
#endif

    while ((mp = getq(q))) {
	putnext( q, mp );
    }
}
#endif

/*
 *  initialization/termination
 *
 *  These can be invoked one of two ways:
 *   . for modules, it is easiest to call them from [init,cleanup]_module
 *   . otherwise, specify "initialize" and "terminate" in Config
 */

void fifo_init(void)
{
    int i;
    dev_t maxminor_dev = MKDEV(0,0x0000ffff) ;
    int   maxminor = MINOR(maxminor_dev)+1 ;

#ifdef FIFO_DEBUG
    cmn_err( CE_CONT, "%s_init() #%d\n", MOD_NAME, fifo_initialized );
#endif

    /*
     *  only initialize once (per load)...
     */
    if (fifo_initialized++)  return;

    /*
     *  initialize major/minor device data
     */
    for (i = 0;  i < fifo_units;  i++) {
	fifo_dev_t *fdp = &(fifo_dev[i]);
	/*
	 *  initialize per-unit data -
	 *
	 *  we treat the index 'i' as a device number of sorts here; we
	 *  let major() and minor() split it into parts.  This works as
	 *  long as the low x bits are minor number, and the upper y
	 *  bits are major.
	 */
	fdp->index = i;
	if ((i % maxminor) != 0) {
	    fdp->flags = 0;
	    list_add( &(fdp->list), free_fifos.prev );
	    num_free_fifos++;
	}  else {
	    fdp->flags = FIFO_CLONABLE;
	    INIT_LIST_HEAD(&(fdp->list));
	    /*
	     *  indicate that this is a FIFO pseudo device that allows
	     *  minors to reopen
	     */
	    LIS_DEVFLAGS(fifo__0_majors[i/maxminor]) |=
		LIS_MODFLG_FIFO | LIS_MODFLG_REOPEN;
	}
    }
#ifdef FIFO_DEBUG
    cmn_err( CE_CONT, "%s_init() %d units available\n", MOD_NAME,
	     num_free_fifos );
#endif
    
    lis_sem_init( &fifo_sem, 1 );
}

void fifo_term(void)
{
#ifdef FIFO_DEBUG
    cmn_err( CE_CONT, "%s_term() #%d\n", MOD_NAME, fifo_initialized );
#endif

    if (--fifo_initialized)  return;

    SEM_DESTROY(&fifo_sem);
}

#ifdef LINUX
#ifdef MODULE

/*
 *  Linux loadable module interface
 */

int init_module(void)
{
    int ret = lis_register_strdev( FIFO__CMAJOR_0, &fifo_info,
				   fifo_units, MOD_NAME );
    if (ret < 0) {
	cmn_err( CE_CONT,
		 "%s - unable to register driver.\n",
		 MOD_NAME );
	return ret;
    }

    fifo_init();

    return 0;
}

void cleanup_module(void)
{
    fifo_term();

    if (lis_unregister_strdev(FIFO__CMAJOR_0) < 0)
	cmn_err( CE_CONT,
		 "%s - unable to unregister driver.\n",
		 MOD_NAME );
    return;
}

#endif
#endif
