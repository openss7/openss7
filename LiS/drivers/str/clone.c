/************************************************************************
*                          Clone Driver                                 *
*************************************************************************
*									*
* Author:	David Grothe <dave@gcom.com>				*
*									*
* Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>		*
*									*
* This driver's open routine divides the device number passed in into	*
* the major and minor parts.  It treats the minor number as the major	*
* number of the driver that the user really wants to open.  It then	*
* calls that driver's open routine passing in the target driver's	*
* major and minor zero.  It sets the clone option bit in this open.	*
*									*
* The queue passed to clone is given the attributes of the target	*
* driver rather than of the clone driver.				*
*									*
* When this driver returns from its open routine the dev_t structure	*
* passed to it will no doubt be modified.  The caller, perhaps clear	*
* back in the I/O system, must take care to notice this and allocate	*
* a new inode for this stream.						*
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

#ident "@(#) LiS clone.c 2.8 09/13/04 10:12:30 "

#include <sys/stream.h>
#include <sys/osif.h>

static struct module_info clone_minfo =
{
  0,				/* id */
  "clone",			/* name */
  0,				/* min packet size accepted */
  0,				/* max packet size accepted */
  0,				/* high water mark */
  0				/* low water mark */
};

static int   _RP clone_open  (queue_t *, dev_t*, int, int, cred_t *);
static int   _RP clone_close (queue_t *, int, cred_t *);

/* qinit structures (rd and wr side) 
 */
static struct qinit clone_rinit =
{
  NULL,				/* put */       
  NULL,				/* service  */  
  clone_open,			/* open */      
  clone_close,			/* close */     
  NULL,				/* admin */     
  &clone_minfo,			/* info */      
  NULL				/* stat */      
};

static struct qinit clone_winit =
{
  NULL, 	                   /* put */       
  NULL, 			/* service  */  
  NULL, 			/* open */      
  NULL, 			/* close */     
  NULL, 			/* admin */     
  &clone_minfo, 		/* info */      
  NULL				/* stat */      
};

/* streamtab for the clone driver.
 */
struct streamtab clone_info =
{
  &clone_rinit,			/* read queue */
  &clone_winit,			/* write queue */
  NULL,				/* mux read queue  */
  NULL				/* mux write queue */
};


static int _RP
clone_open (queue_t *q, dev_t *devp, int flag, int sflag, cred_t *credp)
{
    int		 major_dev ;
    int		 rslt ;
    streamtab_t	*st ;

    if (sflag == CLONEOPEN) return(OPENFAIL) ;

    /*
     * Minor passed to us functions as major for the target driver
     */
    major_dev = getminor (*devp);
    st = lis_find_strdev(major_dev) ;
    if (st == NULL) return(ENOENT) ;		/* no such driver */

    *devp = makedevice(major_dev, 0) ;		/* construct new major */

    if (st->st_rdinit->qi_qopen == NULL) return(OPENFAIL) ;
    lis_setq(q, st->st_rdinit, st->st_wrinit) ;	/* xfer queue params */
    rslt = st->st_rdinit->qi_qopen (q, devp, flag, CLONEOPEN, credp) ;
    if (rslt != 0) return(rslt) ;

    return(0) ;					/* success */

} /* clone_open */


static int _RP
clone_close (queue_t *q, int dummy, cred_t *credp)
{
    (void) q ;					/* compiler happiness */
    (void) dummy ;
    (void) credp ;

    return(0) ;
}

void _RP clone_init(void)
{
    /*
     *  indicate that this is a CLONE pseudo device
     */
    LIS_DEVFLAGS(LIS_CLONE) |= LIS_MODFLG_CLONE;
}

