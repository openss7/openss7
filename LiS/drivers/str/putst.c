/************************************************************************
*                          Pullup Test Driver				*
*************************************************************************
*									*
* Author:	David Grothe <dave@gcom.com>				*
*									*
* Copyright (C) 2002  David Grothe, Gcom, Inc <dave@gcom.com>		*
*									*
* This driver runs a test of the LiS pullupmsg routine when its open	*
* routine gets called.  It is linked in with the user mode LiS only	*
* for testing.								*
*									*
* I am also expropriating this driver for a few other message function	*
* tests.								*
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

#ident "@(#) LiS putst.c 1.5 09/13/04"

#include <sys/stream.h>
#include <sys/cmn_err.h>
#if defined(LINUX)		/* Linux kernel compile */
#include <linux/string.h>
#else				/* user mode compile */
#include <string.h>
#endif
#include <sys/osif.h>

static struct module_info putst_minfo =
{
  0,				/* id */
  "putst",			/* name */
  0,				/* min packet size accepted */
  0,				/* max packet size accepted */
  0,				/* high water mark */
  0				/* low water mark */
};

static int   _RP putst_open  (queue_t *, dev_t*, int, int, cred_t *);
static int   _RP putst_close (queue_t *, int, cred_t *);
static int       putst_test (void) ;

/* qinit structures (rd and wr side) 
 */
static struct qinit putst_rinit =
{
  NULL,				/* put */       
  NULL,				/* service  */  
  putst_open,			/* open */      
  putst_close,			/* close */     
  NULL,				/* admin */     
  &putst_minfo,		/* info */      
  NULL				/* stat */      
};

static struct qinit putst_winit =
{
  NULL, 	                /* put */       
  NULL, 			/* service  */  
  NULL, 			/* open */      
  NULL, 			/* close */     
  NULL, 			/* admin */     
  &putst_minfo, 		/* info */      
  NULL				/* stat */      
};

/* streamtab for the printk driver.
 */
struct streamtab putst_info =
{
  &putst_rinit,		/* read queue */
  &putst_winit,		/* write queue */
  NULL,				/* mux read queue  */
  NULL				/* mux write queue */
};


/*
 * Open routine grants all opens
 */
static int _RP
putst_open (queue_t *q, dev_t *devp, int flag, int sflag, cred_t *credp)
{
    (void) q ;					/* compiler happiness */
    (void) devp ;				/* compiler happiness */
    (void) flag ;				/* compiler happiness */
    (void) sflag ;				/* compiler happiness */
    (void) credp ;				/* compiler happiness */

    return(putst_test()) ;

} /* putst_open */


static int _RP
putst_close (queue_t *q, int dummy, cred_t *credp)
{
    (void) q ;					/* compiler happiness */
    (void) dummy ;
    (void) credp ;

    return(0) ;
}

/************************************************************************
*                              putst_test                               *
*************************************************************************
*									*
* Test procedure for pullupmsg.						*
*									*
************************************************************************/
static char	*txt1 = "abcdefg" ;
static char	*txt2 = "hijklmnop" ;
static char	*txt3 = "qrstuvwxyz" ;
static char	 mybuf[50] ;
static void _RP esb_free(char *buf)
{
    if (buf == mybuf)
	cmn_err(CE_CONT, "esb_free: passed correct buffer\n") ;
    else
	cmn_err(CE_CONT, "esb_free: passed %p expecting %p\n",
			 buf, mybuf) ;

}
int putst_test(void)
{
    mblk_t	*mp1 ;
    mblk_t	*mp2 ;
    mblk_t	*mp3 ;
    mblk_t	*dup_mp ;
    frtn_t	 esb_rtn ;
    int		 txt1_len = strlen(txt1) ;
    int		 txt2_len = strlen(txt2) ;
    int		 txt3_len = strlen(txt3) ;
    int		 rslt ;

    mp1 = allocb(20, BPRI_MED) ;
    if (mp1 == NULL)
    {
	cmn_err(CE_CONT, "allocb(20) failed\n") ;
	return(ENOMEM) ;
    }

    strcpy(mp1->b_wptr, txt1) ;
    mp1->b_wptr += txt1_len ;

    /***************************/
    cmn_err(CE_CONT, "Buffer before pullupmsg(6) -- "
	    	     "rptr=%p wptr=%p %d bytes\n",
		     mp1->b_rptr, mp1->b_wptr, msgdsize(mp1)) ;
    lis_print_msg(mp1, "", PRINT_DATA_ENTIRE) ;
    rslt = pullupmsg(mp1, 6) ;
    cmn_err(CE_CONT, "Buffer after pullupmsg(6) -- "
	    	     "rptr=%p wptr=%p %d bytes\n",
		     mp1->b_rptr, mp1->b_wptr, msgdsize(mp1)) ;
    lis_print_msg(mp1, "", PRINT_DATA_ENTIRE) ;
    if (rslt)
	cmn_err(CE_CONT, "pullupmsg(\"%s\", 6) OK\n", mp1->b_rptr) ;
    else
    {
	cmn_err(CE_CONT, "pullupmsg(\"%s\", 6) failed\n", mp1->b_rptr) ;
	return(EINVAL) ;
    }

    /***************************/
    rslt = pullupmsg(mp1, 10) ;
    cmn_err(CE_CONT, "Buffer after pullupmsg(10) -- "
	    	     "rptr=%p wptr=%p %d bytes\n",
		     mp1->b_rptr, mp1->b_wptr, msgdsize(mp1)) ;
    lis_print_msg(mp1, "", PRINT_DATA_ENTIRE) ;
    if (rslt)
    {
	cmn_err(CE_CONT, "pullupmsg(\"%s\", 10) unexpected success\n", 
							    mp1->b_rptr) ;
	return(EINVAL) ;
    }
    else
	cmn_err(CE_CONT, "pullupmsg(\"%s\", 10) expected failure\n",
							    mp1->b_rptr) ;

    /***************************/
    mp1->b_rptr++ ;
    cmn_err(CE_CONT, "Buffer before pullupmsg(misaligned, 6) -- "
	    	     "rptr=%p wptr=%p %d bytes\n",
		     mp1->b_rptr, mp1->b_wptr, msgdsize(mp1)) ;
    lis_print_msg(mp1, "", PRINT_DATA_ENTIRE) ;
    rslt = pullupmsg(mp1, 6) ;
    cmn_err(CE_CONT, "Buffer after pullupmsg(misaligned, 6) -- "
	    	     "rptr=%p wptr=%p %d bytes\n",
		     mp1->b_rptr, mp1->b_wptr, msgdsize(mp1)) ;
    lis_print_msg(mp1, "", PRINT_DATA_ENTIRE) ;
    if (rslt)
	cmn_err(CE_CONT, "pullupmsg(\"%s\", 6) OK\n", mp1->b_rptr) ;
    else
    {
	cmn_err(CE_CONT, "pullupmsg(\"%s\", 6) failed\n", mp1->b_rptr) ;
	return(EINVAL) ;
    }

    /***************************/
    mp2 = allocb(21, BPRI_MED) ;
    if (mp2 == NULL)
    {
	cmn_err(CE_CONT, "allocb(21) failed\n") ;
	return(ENOMEM) ;
    }

    strcpy(mp2->b_wptr, txt2) ;
    mp2->b_wptr += txt2_len ;
    mp1->b_cont = mp2 ;
    cmn_err(CE_CONT, "Buffer before pullupmsg(8) -- "
	    	     "rptr=%p wptr=%p %d bytes\n",
		     mp1->b_rptr, mp1->b_wptr, msgdsize(mp1)) ;
    lis_print_msg(mp1, "", PRINT_DATA_ENTIRE) ;
    rslt = pullupmsg(mp1, 8) ;
    cmn_err(CE_CONT, "Buffer after pullupmsg(8) -- "
	    	     "rptr=%p wptr=%p %d bytes\n",
		     mp1->b_rptr, mp1->b_wptr, msgdsize(mp1)) ;
    lis_print_msg(mp1, "", PRINT_DATA_ENTIRE) ;
    if (rslt)
	cmn_err(CE_CONT, "pullupmsg(\"%s\", 8) OK\n", mp1->b_rptr) ;
    else
    {
	cmn_err(CE_CONT, "pullupmsg(\"%s\", 8) failed\n", mp1->b_rptr) ;
	return(EINVAL) ;
    }

    /***************************/
    mp3 = allocb(22, BPRI_MED) ;
    if (mp3 == NULL)
    {
	cmn_err(CE_CONT, "allocb(22) failed\n") ;
	return(ENOMEM) ;
    }

    strcpy(mp3->b_wptr, txt3) ;
    mp3->b_wptr += txt3_len ;
    mp2->b_cont = mp3 ;
    cmn_err(CE_CONT, "Buffer before pullupmsg(21) -- "
	    	     "rptr=%p wptr=%p %d bytes\n",
		     mp1->b_rptr, mp1->b_wptr, msgdsize(mp1)) ;
    lis_print_msg(mp1, "", PRINT_DATA_ENTIRE) ;
    rslt = pullupmsg(mp1, 21) ;
    cmn_err(CE_CONT, "Buffer after pullupmsg(21) -- "
	    	     "rptr=%p wptr=%p %d bytes\n",
		     mp1->b_rptr, mp1->b_wptr, msgdsize(mp1)) ;
    lis_print_msg(mp1, "", PRINT_DATA_ENTIRE) ;
    if (rslt)
	cmn_err(CE_CONT, "pullupmsg(\"%s\", 21) OK\n", mp1->b_rptr) ;
    else
    {
	cmn_err(CE_CONT, "pullupmsg(\"%s\", 21) failed\n", mp1->b_rptr) ;
	return(EINVAL) ;
    }

    /***************************/
    freemsg(mp1) ;
    mp1 = allocb(23, BPRI_MED) ;
    if (mp1 == NULL)
    {
	cmn_err(CE_CONT, "allocb(23) failed\n") ;
	return(ENOMEM) ;
    }

    strcpy(mp1->b_wptr, txt1) ;
    mp1->b_wptr += txt1_len ;

    mp2 = allocb(24, BPRI_MED) ;
    if (mp2 == NULL)
    {
	cmn_err(CE_CONT, "allocb(24) failed\n") ;
	return(ENOMEM) ;
    }

    strcpy(mp2->b_wptr, txt2) ;
    mp2->b_wptr += txt2_len ;
    mp1->b_cont = mp2 ;

    mp3 = allocb(25, BPRI_MED) ;
    if (mp3 == NULL)
    {
	cmn_err(CE_CONT, "allocb(25) failed\n") ;
	return(ENOMEM) ;
    }

    strcpy(mp3->b_wptr, txt3) ;
    mp3->b_wptr += txt3_len ;
    mp2->b_cont = mp3 ;

    dup_mp = dupmsg(mp1) ;

    cmn_err(CE_CONT, "Original buffer before pullupmsg(22) -- "
	    	     "rptr=%p wptr=%p %d bytes\n",
		     mp1->b_rptr, mp1->b_wptr, msgdsize(mp1)) ;
    lis_print_msg(mp1, "", PRINT_DATA_ENTIRE) ;
    cmn_err(CE_CONT, "Dup'd buffer before pullupmsg(22) -- "
	    	     "rptr=%p wptr=%p %d bytes\n",
		     dup_mp->b_rptr, dup_mp->b_wptr, msgdsize(dup_mp)) ;
    lis_print_msg(dup_mp, "", PRINT_DATA_ENTIRE) ;
    rslt = pullupmsg(mp1, 22) ;
    cmn_err(CE_CONT, "Original buffer after pullupmsg(22) -- "
	    	     "rptr=%p wptr=%p %d bytes\n",
		     mp1->b_rptr, mp1->b_wptr, msgdsize(mp1)) ;
    lis_print_msg(mp1, "", PRINT_DATA_ENTIRE) ;
    cmn_err(CE_CONT, "Dup'd buffer after pullupmsg(22) -- "
	    	     "rptr=%p wptr=%p %d bytes\n",
		     dup_mp->b_rptr, dup_mp->b_wptr, msgdsize(dup_mp)) ;
    lis_print_msg(dup_mp, "", PRINT_DATA_ENTIRE) ;
    if (rslt)
	cmn_err(CE_CONT, "pullupmsg(\"%s\", 22) OK\n", mp1->b_rptr) ;
    else
    {
	cmn_err(CE_CONT, "pullupmsg(\"%s\", 22) failed\n", mp1->b_rptr) ;
	return(EINVAL) ;
    }

    freemsg(mp1) ;
    cmn_err(CE_CONT, "Dup'd buffer after free original -- "
	    	     "rptr=%p wptr=%p %d bytes\n",
		     dup_mp->b_rptr, dup_mp->b_wptr, msgdsize(dup_mp)) ;
    lis_print_msg(dup_mp, "", PRINT_DATA_ENTIRE) ;
    freemsg(dup_mp) ;

    /***************************/
    esb_rtn.free_func = esb_free ;
    esb_rtn.free_arg  = mybuf ;
    mp1 = esballoc(mybuf, sizeof(mybuf), BPRI_MED, &esb_rtn) ;
    if (mp1 == NULL)
    {
	cmn_err(CE_CONT, "esballoc failed\n") ;
	return(ENOMEM) ;
    }
    cmn_err(CE_CONT, "esballoc returned mblk ptr - OK\n") ;

    if ((char *)mp1->b_datap->db_base != mybuf)
    {
	cmn_err(CE_CONT, "db_base is %p should be %p\n",
					mp1->b_datap->db_base, mybuf) ;
	return(EINVAL) ;
    }
    cmn_err(CE_CONT, "db_base is OK\n") ;

    if ((char *)mp1->b_datap->db_lim != mybuf + sizeof(mybuf))
    {
	cmn_err(CE_CONT, "db_lim is %p should be %p\n",
				mp1->b_datap->db_lim, mybuf + sizeof(mybuf)) ;
	return(EINVAL) ;
    }
    cmn_err(CE_CONT, "db_lim is OK\n") ;

    if (mp1->b_datap->db_size != sizeof(mybuf))
    {
	cmn_err(CE_CONT, "db_size is %d should be %d\n",
			    mp1->b_datap->db_size, sizeof(mybuf));
	return(EINVAL) ;
    }
    cmn_err(CE_CONT, "db_size is OK\n") ;

    if (mp1->b_datap->db_ref != 1)
    {
	cmn_err(CE_CONT, "db_ref is %d should be 1\n", mp1->b_datap->db_ref);
	return(EINVAL) ;
    }
    cmn_err(CE_CONT, "db_ref is OK\n") ;

    if ((char *)mp1->b_rptr != mybuf)
    {
	cmn_err(CE_CONT, "b_rptr is %p should be %p\n", mp1->b_rptr, mybuf) ;
	return(EINVAL) ;
    }
    cmn_err(CE_CONT, "b_rptr is OK\n") ;

    if ((char *)mp1->b_wptr != mybuf)
    {
	cmn_err(CE_CONT, "b_wptr is %p should be %p\n", mp1->b_wptr, mybuf) ;
	return(EINVAL) ;
    }
    cmn_err(CE_CONT, "b_wptr is OK\n") ;

    freemsg(mp1) ;
    cmn_err(CE_CONT, "esballoc message freed\n") ;

    return(0) ;
}
