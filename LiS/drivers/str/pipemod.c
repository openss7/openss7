/*
 *  pipemod.c - "pipemod" module
 *
 *  pipemod's only function is to facilitate flush handling at the
 *  midpoint of pipes by switching the sense of the flush flags in
 *  M_FLUSH messages.  In this implementation, we also do flush
 *  handling as well (since this is a module, and modules are expected
 *  to do flush handling...).
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

#ident "@(#) LiS pipemod.c 1.6 12/15/02"

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
#include <sys/osif.h>

/*
 *  Some configuration sanity checks
 */
#ifndef PIPEMOD_
#error Not configured
#endif

#ifndef PIPEMOD__ID
#define PIPEMOD__ID 0x8888
#endif

#define  MOD_ID   PIPEMOD__ID
#define  MOD_NAME "pipemod"

/*
 *  function prototypes
 */
static int  pipemod_open(queue_t *, dev_t*, int, int, cred_t *);
static int  pipemod_close(queue_t *, int, cred_t *);
static int  pipemod_put(queue_t *, mblk_t *);

static void flush_module(queue_t *, mblk_t *);

/*
 *  module structure
 */
static struct module_info pipemod_minfo =
{ 
    MOD_ID,            /* id */
    MOD_NAME,          /* name */
    0,                 /* min packet size accepted */
    INFPSZ,            /* max packet size accepted */
    10240L,            /* highwater mark */
    512L               /* lowwater mark */
};

static struct qinit pipemod_rinit = {
    pipemod_put,       /* put */
    NULL,              /* service */
    pipemod_open,      /* open */
    pipemod_close,     /* close */
    NULL,              /* admin */
    &pipemod_minfo,    /* info */
    NULL               /* stat */
};

static struct qinit pipemod_winit = { 
    pipemod_put,       /* put */
    NULL,              /* service */
    NULL,              /* open */
    NULL,              /* close */
    NULL,              /* admin */
    &pipemod_minfo,    /* info */
    NULL               /* stat */
};

struct streamtab pipemod_info =
{
    &pipemod_rinit,        /* read queue init */
    &pipemod_winit,        /* write queue init */
    NULL,                  /* lower mux read queue init */
    NULL                   /* lower mux write queue init */
};

/*
 *  open
 */
static int pipemod_open( q, devp, flag, sflag, credp )
queue_t *q;
dev_t *devp;
int flag, sflag;
cred_t *credp;
{
#ifdef PIPE_DEBUG
    cmn_err( CE_CONT,
	     "%s_open( 0x%p, 0x%x, 0x%x, 0x%x, ... )\n",
	     MOD_NAME, q, *devp, flag, sflag );
#endif

    if (!(((int)RD(q)->q_ptr)++))  MOD_INC_USE_COUNT;
    WR(q)->q_ptr = RD(q)->q_ptr;

    qprocson(q);

    return 0;	/* success */
}

/*
 *  close
 */
static int pipemod_close( q, flag, credp )
queue_t *q;
int flag;
cred_t *credp;
{
#ifdef PIPE_DEBUG
    cmn_err( CE_CONT,
	     "%s_close( 0x%p, 0x%x, ... )\n", MOD_NAME, q, flag );
#endif
    
    RD(q)->q_ptr = WR(q)->q_ptr = NULL;
    MOD_DEC_USE_COUNT;
    qprocsoff(q);

    return 0;	/* success */
}
 
/*
 *  canonical module flush handling - this could be used in any module
 */
static void flush_module( q, mp )
queue_t *q;
mblk_t *mp;
{
    queue_t *rq = RD(q), *wq = WR(q);

    if (*(mp->b_rptr) & FLUSHW) {
	if (*(mp->b_rptr) & FLUSHBAND)
	    flushband( wq, FLUSHDATA, *(mp->b_rptr + 1) );
	else
	    flushq( wq, FLUSHDATA );
    }
    if (*(mp->b_rptr) & FLUSHR) {
	if (*(mp->b_rptr) & FLUSHBAND)
	    flushband( rq, FLUSHDATA, *(mp->b_rptr + 1) );
	else
	    flushq( rq, FLUSHDATA );
    }
    if (!SAMESTR(q)) {
	char flush = *(mp->b_rptr) & FLUSHRW;
	
	switch (flush & FLUSHRW) {
	case FLUSHR:
	case FLUSHW:
	    *(mp->b_rptr) = (flush & ~FLUSHRW) | (flush ^ FLUSHRW);
	default:
	    break;
	}
    }
    putnext( q, mp );
}
/*
 *  put
 */
static int pipemod_put( q, mp )
queue_t *q;
mblk_t *mp;
{
#ifdef PIPE_DEBUG
    cmn_err( CE_CONT,
	     "%s_wput( 0x%08x, 0x%08x )\n", MOD_NAME, q, mp );
#endif

    switch (mp->b_datap->db_type) {
    case M_FLUSH:
	flush_module( q, mp );
	break;
    default:
	putnext( q, mp );
	break;
    }
    return(0) ;
}

#ifdef MODULE

/*
 *  Linux loadable module interface
 */

int init_module(void)
{
    int ret = lis_register_strmod( &pipemod_info, MOD_NAME );
    if (ret < 0) {
	cmn_err( CE_CONT,
		 "%s - unable to register module.\n",
		 MOD_NAME );
	return ret;
    }

    return 0;
}

void cleanup_module(void)
{
    if (lis_unregister_strmod(&pipemod_info) < 0)
	cmn_err( CE_CONT,
		 "%s - unable to unregister module.\n",
		 MOD_NAME );
    return;
}

#endif

#if defined(LINUX)			/* linux kernel */
#if defined(MODULE_LICENSE)
MODULE_LICENSE("GPL and additional rights");
#endif
#if defined(MODULE_AUTHOR)
MODULE_AUTHOR("John A. Boyd Jr");
#endif
#if defined(MODULE_DESCRIPTION)
MODULE_DESCRIPTION("STREAMS Pipe Module");
#endif
#endif
