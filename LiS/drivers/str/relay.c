/*                               -*- Mode: C -*- 
 * relay.c --- A simple relay pushable module
 * Author          : Dave Grothe
 * Created On      : Dec 30, 1995
 * Last Modified By: Dave Grothe
 * RCS Id          : $Id: relay.c,v 1.1 1996/01/07 20:49:08 dave Exp $
 * Purpose         : relay messages just to test pushable modules
 * ----------------______________________________________________
 *
 *    Copyright (C) 1995  David Grothe <dave@gcom.com>
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
 */

#ident "@(#) LiS relay.c 2.3 3/19/01 22:03:39 "

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
#include <sys/osif.h>

/*  -------------------------------------------------------------------  */
/*			  Module definition structs                      */

/* Module info for the relay module
 */
static struct module_info relay_minfo =
{
  0,				/* id */
  "relay",			/* name */
  0,				/* min packet size accepted */
  INFPSZ,			/* max packet size accepted */
  10240L,			/* high water mark */
  512L				/* low water mark */
};

static struct module_info relay2_minfo =
{
  0,				/* id */
  "relay2",			/* name */
  0,				/* min packet size accepted */
  INFPSZ,			/* max packet size accepted */
  10240L,			/* high water mark */
  512L				/* low water mark */
};

/* These are the entry points to the driver: open, close, write side put and
 * service procedures and read side service procedure.
 */
static int   relay_open  (queue_t *,dev_t*,int,int, cred_t *);
static int   relay_close (queue_t *, int, cred_t *);
static int   relay_wput  (queue_t *, mblk_t *);
static int   relay_rput  (queue_t *, mblk_t *);
#if 0
static int   relay_wsrv  (queue_t *);
static int   relay_rsrv  (queue_t *);
#endif

/* qinit structures (rd and wr side) 
 */
static struct qinit relay_rinit =
{
  relay_rput,			/* put */       
  NULL,				/* service  */  
  relay_open,			/* open */      
  relay_close,			/* close */     
  NULL,				/* admin */     
  &relay_minfo,			/* info */      
  NULL				/* stat */      
};

static struct qinit relay_winit =
{
  relay_wput,                    /* put */       
  NULL, 			/* service  */  
  NULL, 			/* open */      
  NULL, 			/* close */     
  NULL, 			/* admin */     
  &relay_minfo, 			/* info */      
  NULL				/* stat */      
};

/* streamtab for the loopback driver.
 */
struct streamtab relay_info =
{
  &relay_rinit,			/* read queue */
  &relay_winit,			/* write queue */
  NULL,				/* mux read queue  */
  NULL				/* mux write queue */
};

static struct qinit relay2_rinit =
{
  relay_rput,			/* put */       
  NULL,				/* service  */  
  relay_open,			/* open */      
  relay_close,			/* close */     
  NULL,				/* admin */     
  &relay2_minfo,		/* info */      
  NULL				/* stat */      
};

static struct qinit relay2_winit =
{
  relay_wput,                   /* put */       
  NULL, 			/* service  */  
  NULL, 			/* open */      
  NULL, 			/* close */     
  NULL, 			/* admin */     
  &relay2_minfo, 		/* info */      
  NULL				/* stat */      
};

/* streamtab for the loopback driver.
 */
struct streamtab relay2_info =
{
  &relay2_rinit,		/* read queue */
  &relay2_winit,		/* write queue */
  NULL,				/* mux read queue  */
  NULL				/* mux write queue */
};
/*  -------------------------------------------------------------------  */
/*			    Module implementation                        */
/*  -------------------------------------------------------------------  */

/*  -------------------------------------------------------------------  */
/*				relay_open				 */
/*  -------------------------------------------------------------------  */
static int
relay_open (queue_t *q, dev_t *devp, int flag, int sflag, cred_t *credp)
{
  if (sflag == CLONEOPEN)
      return(EINVAL) ;

  return 0;					/* success */

}

/*  -------------------------------------------------------------------  */
/*				relay_wput				 */
/*  -------------------------------------------------------------------  */

static int
relay_wput (queue_t *q, mblk_t *mp)
{
    putnext(q, mp) ;			/* relay downstream */
    return(0) ;
}

/*  -------------------------------------------------------------------  */
/*				relay_rput				 */
/*  -------------------------------------------------------------------  */

static int
relay_rput (queue_t *q, mblk_t *mp)
{
    putnext(q, mp) ;			/* relay upstream */
    return(0) ;
}

#if 0
/*  -------------------------------------------------------------------  */
/*				relay_wsrv				 */
/*  -------------------------------------------------------------------  */

static int
relay_wsrv (queue_t *q)
{
    /* not used */
    return(0) ;
}
#endif

#if 0
/*  -------------------------------------------------------------------  */
/*				relay_rsrv				 */
/*  -------------------------------------------------------------------  */

static int
relay_rsrv (queue_t *q)
{
    /* not used */
    return(0) ;
}
#endif

/*  -------------------------------------------------------------------  */
/*				relay_close				 */
/*  -------------------------------------------------------------------  */

static int
relay_close (queue_t *q, int dummy, cred_t *credp)
{
    return 0;
}
