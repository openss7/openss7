/************************************************************************
*                     Multi-Threaded Test Driver                        *
*************************************************************************
*									*
* Used to test the multi-threaded aspects of driver opens.  This is	*
* a separately loadable driver which is only loaded when strtst is run.	*
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

#ident "@(#) LiS mtdrv.c 1.9 09/13/04"

/*  -------------------------------------------------------------------  */

#include <sys/LiS/module.h>	/* must be VERY first include */

#include <sys/stream.h>
#include <sys/stropts.h>
#include <linux/stat.h>		/* for S_IFCHR */
#include <sys/osif.h>

#include <sys/LiS/mtdrv.h>

/************************************************************************
*                        Module Definition Structs                      *
************************************************************************/

static struct module_info mtdrv_minfo = {
    0,				/* id */
    "mtdrv",			/* name */
    0,				/* min packet size accepted */
    INFPSZ,			/* max packet size accepted */
    10240L,			/* high water mark */
    512L			/* low water mark */
};

/* These are the entry points to the driver: open, close, write side put and
 * service procedures and read side service procedure.
 */
static int _RP mtdrv_open(queue_t *, dev_t *, int, int, cred_t *);
static int _RP mtdrv_close(queue_t *, int, cred_t *);
static int _RP mtdrv_wput(queue_t *, mblk_t *);
static int _RP mtdrv_wsrv(queue_t *);
static int _RP mtdrv_rsrv(queue_t *);

/* qinit structures (rd and wr side) 
 */
static struct qinit mtdrv_rinit = {
    NULL,			/* put */
    mtdrv_rsrv,			/* service  */
    mtdrv_open,			/* open */
    mtdrv_close,			/* close */
    NULL,			/* admin */
    &mtdrv_minfo,		/* info */
    NULL			/* stat */
};

static struct qinit mtdrv_winit = {
    mtdrv_wput,			/* put */
    mtdrv_wsrv,			/* service  */
    NULL,			/* open */
    NULL,			/* close */
    NULL,			/* admin */
    &mtdrv_minfo,		/* info */
    NULL			/* stat */
};

/* streamtab for the driver.
 */
struct streamtab mtdrv_info = {
    &mtdrv_rinit,		/* read queue */
    &mtdrv_winit,		/* write queue */
    NULL,			/* mux read queue  */
    NULL			/* mux write queue */
};

/************************************************************************
*                        Global Storage                                 *
************************************************************************/

static int		 next_clone_dev ;
static int		 open_sleep_time ;
static lis_semaphore_t	*open_sleep_sem ;
static int		 mtdrv_major ;		/* from registration */

static lis_semaphore_t	*open_sem ;
#define N_DEVS	20
static char		 devs[N_DEVS] ;

/************************************************************************
*                           Prototypes                                  *
************************************************************************/

static void _RP mtdrv_timeout(caddr_t arg) ;

/************************************************************************
*                          Functions                                    *
************************************************************************/

static void make_node(char *name, int maj, int mnr)
{
    int rtn;

    rtn = lis_unlink(name);
    if (rtn < 0 && rtn != -ENOENT)
    {
	printk("unlink %s: error %d\n", name, rtn);
	return ;
    }

    if (maj == 0)			/* don't make the node */
    {
	printk("unlink %s: OK\n", name);
	return ;
    }

    rtn = lis_mknod(name, 0666 | S_IFCHR, UMKDEV(maj, mnr));
    if (rtn == 0)
	printk("mknod %s: OK\n", name);
    else
	printk("mknod %s: error %d\n", name, rtn);
}

void _RP mtdrv_init(int maj)
{
    make_node("/dev/mtdrv_clone", CLONE__CMAJOR_0, maj);
    make_node("/dev/mtdrv.1", maj, 1);
    make_node("/dev/mtdrv.2", maj, 2);
    make_node("/dev/mtdrv.3", maj, 3);
    make_node("/dev/mtdrv.4", maj, 4);
    open_sleep_sem = lis_sem_alloc(0) ;
    open_sem = lis_sem_alloc(1) ;

}				/* mtdrv_init */

void _RP mtdrv_term(void)
{
    open_sleep_sem = lis_sem_destroy(open_sleep_sem) ;
    open_sem = lis_sem_destroy(open_sem) ;
    make_node("/dev/mtdrv_clone", 0,0);
    make_node("/dev/mtdrv.1", 0,0);
    make_node("/dev/mtdrv.2", 0,0);
    make_node("/dev/mtdrv.3", 0,0);
    make_node("/dev/mtdrv.4", 0,0);

}				/* mtdrv_term */

/*  -------------------------------------------------------------------  */


static int _RP
mtdrv_open(queue_t * q, dev_t * devp, int flag, int sflag, cred_t * credp)
{
    int          dev;

    printk("mtdrv_open: flag=0x%x sflag=0x%x\n", flag, sflag) ;
    lis_down(open_sem) ;
    if (sflag == CLONEOPEN)
    {
	if (next_clone_dev)
	{
	    dev = next_clone_dev ;
	    next_clone_dev = 0 ;
	    if (dev >= sizeof(devs))
	    {
		lis_up(open_sem) ;
		return(-EINVAL) ;
	    }
	}
	else
	{
	    for (dev = 1; dev < sizeof(devs); dev++)
		if (devs[dev] == 0) break ;

	    if (dev >= sizeof(devs))
	    {
		lis_up(open_sem) ;
		return(-EBUSY) ;
	    }

	}
    }
    else
	dev = getminor(*devp);

    MODGET();
    devs[dev]++ ;		/* must be protected by semaphore */

    /*
     * Note: we may be forced by an ioctl to "select" an already open
     * dev as a clone device to test an error condition.  In that case
     * we do not want to set the q_ptr to the devs array if that dev
     * is already in use.  We will get a close call for the erroneous
     * open and we want the pointer to be NULL when close is called.
     */
    if (devs[dev] == 1)		/* first open */
	q->q_ptr = (void *) &devs[dev] ;
    lis_up(open_sem) ;
    *devp = makedevice(getmajor(*devp), dev);

    printk("mtdrv_open: dev=%d\n", dev) ;
    if (open_sleep_time > 0)
    {
	timeout(mtdrv_timeout, (caddr_t) 0, open_sleep_time);
	open_sleep_time = 0 ;
	lis_down(open_sleep_sem) ;
    }

    return 0;			/* success */
}

/*  -------------------------------------------------------------------  */

static void _RP mtdrv_timeout(caddr_t arg)
{
    lis_up(open_sleep_sem) ;
}				/* mtdrv_timeout */

/*  -------------------------------------------------------------------  */

static void mtdrv_ioctl(queue_t * q, mblk_t * mp)
{
    struct iocblk *iocb;
    int 	   error;
    int		   rtn_count = 0 ;

    iocb = (struct iocblk *) mp->b_rptr;

    /*
     * Syntax switch
     */
    switch (iocb->ioc_cmd)
    {
    case MTDRV_SET_CLONE_DEV:
    case MTDRV_SET_OPEN_SLEEP:
	if (mp->b_cont == NULL)
	{
	    printk("mtdrv_ioctl: integer argument expected\n") ;
	    error = EINVAL ;
	    goto iocnak;
	}
	if (iocb->ioc_count != sizeof(int))
	{
	    printk("mtdrv_ioctl: expected ioctl len %d, got %d\n", sizeof(int),
		   iocb->ioc_count);

	    error = EINVAL;
	    goto iocnak;
	}
	break ;

    default:
	printk("mtdrv_ioctl: Invalid ioctl: %x\n", iocb->ioc_cmd);
	error = EINVAL;
	goto iocnak ;

      iocnak:
	{
	    mp->b_datap->db_type = M_IOCNAK;
	    iocb->ioc_error = error;
	    qreply(q, mp);
	}
	return ;
    }			/* end of switch on ioc_cmd */

    /*
     * Semantics switch
     */
    switch (iocb->ioc_cmd)
    {
    case MTDRV_SET_CLONE_DEV:
	next_clone_dev = *(int *) mp->b_cont->b_rptr;
	break ;
    case MTDRV_SET_OPEN_SLEEP:
	open_sleep_time = *(int *) mp->b_cont->b_rptr;
	break ;

    }			/* end of switch on ioc_cmd */

    mp->b_datap->db_type = M_IOCACK;
    iocb->ioc_count = rtn_count;
    iocb->ioc_error = 0;
    qreply(q, mp);
}

/*  -------------------------------------------------------------------  */


static int _RP mtdrv_wput(queue_t * q, mblk_t * mp)
{

    switch (mp->b_datap->db_type)
    {
    case M_IOCTL:
	mtdrv_ioctl(q, mp) ;
	return(0) ;

    case M_FLUSH:
	if (*mp->b_rptr & FLUSHW)
	    flushq(q, FLUSHDATA);
	if (*mp->b_rptr & FLUSHR)
	{
	    flushq(RD(q), FLUSHDATA);
	    *mp->b_rptr &= ~FLUSHW;
	    qreply(q, mp);
	}
	else
	    freemsg(mp);
	return(0) ;

    default:
	break;
    }

    freemsg(mp) ;
    return (0);
}

/*  -------------------------------------------------------------------  */

static int _RP mtdrv_wsrv(queue_t * q)
{
    return (0);
}

/*  -------------------------------------------------------------------  */

static int _RP mtdrv_rsrv(queue_t * q)
{
    return (0);
}

/*  -------------------------------------------------------------------  */

static int _RP mtdrv_close(queue_t * q, int dummy, cred_t * credp)
{
    char	*devp = (char *) q->q_ptr ;

    while (devp != NULL && *devp)
    {
	MODPUT();
	(*devp)-- ;
    }
    return (0);
}


#ifdef MODULE

#ifdef KERNEL_2_5
int mtdrv_init_module(void)
#else
int init_module(void)
#endif
{
    int ret = lis_register_strdev(0, &mtdrv_info, 10, "mtdrv");
    if (ret < 0)
    {
	printk("mtdrv.init_module: Unable to register driver.\n");
	return ret;
    }
    mtdrv_major = ret ;
    mtdrv_init(mtdrv_major) ;
    return 0;
}

#ifdef KERNEL_2_5
void mtdrv_cleanup_module(void)
#else
void cleanup_module(void)
#endif
{
    mtdrv_term() ;
    if (lis_unregister_strdev(mtdrv_major) < 0)
	printk("mtdrv.cleanup_module: Unable to unregister driver.\n");
    else
	printk("mtdrv.cleanup_module: Unregistered, ready to be unloaded.\n");
    return;
}

#ifdef KERNEL_2_5
module_init(mtdrv_init_module) ;
module_exit(mtdrv_cleanup_module) ;
#endif

#if defined(MODULE_LICENSE)
MODULE_LICENSE("GPL and additional rights");
#endif
#if defined(MODULE_AUTHOR)
MODULE_AUTHOR("David Grothe");
#endif
#if defined(MODULE_DESCRIPTION)
MODULE_DESCRIPTION("multi-threaded open test helper driver");
#endif

#endif
