/*
 ****************************************************************
 *                                                              *
 *  HULA project						*
 *                                             			*
 *  program:  	cltp.c		                                *
 *  author:   	kurt dobbins                                    *
 *  date:     	5/89                                            *
 *                                                              *
 *  This program implements the ISO 8602 Connectionless-Mode 	*
 *  Transport Protocol.  It is designed to provide  	 	*
 *  connectionless service over connectionless-mode network	*
 *  service.  There are no provisions for running over a        *
 *  connection-oriented network service.		        *
 *                                  				*
 *  entry points:                   				*
 *                                  				*
 * 	T_UnitDataWrite (tb, uv, hptr, td)                      *
 *      T_UnitDataRead (tb, tud, td)	                        *
 *								*
 ****************************************************************
 *								*
 *								*
 *			     NOTICE		   		*
 *								*
 *    Acquisition, use, and distribution of this module and     *
 *    related materials are subject to the restrictions of a    *
 *    license agreement.					*
 *								*    
 *    This software is for prototype purposes only.		*
 *								*
 ****************************************************************
 */


#include <stdio.h>
#include "tpkt.h"


/*  */

/* returns ptr to header and header len */

int  T_UnitDataWrite (tb, uv, hptr, td)

register struct tsapblk *tb;
register struct udvec *uv;
char	              **hptr;
struct TSAPdisconnect *td;
{

    char   *vptr,
           *liptr;

    int	   n;


    /*
     *  Format the ISO T-UNITDATA header
     */

 	if ((vptr =  malloc ((unsigned) (3 + 7 + 
			   (2 + tb -> tb_initiating.ta_selectlen)
 			 + (2 + tb -> tb_initiating.ta_selectlen) 
			 + 3))) == NULL)
	    return DR_CONGEST;

	*hptr = vptr;
	liptr = vptr++;
        *vptr++ = TPDU_UD;
	*liptr = 1;

 	if (tb -> tb_initiating.ta_selectlen > 0) 
	    {
	    *vptr++ = VDAT_TSAP_CLI;
	    *vptr++ = tb -> tb_initiating.ta_selectlen;
	    bcopy (tb -> tb_initiating.ta_selector, 
		   vptr, 
		   tb -> tb_initiating.ta_selectlen);
	    vptr += tb -> tb_initiating.ta_selectlen;
	    *liptr += 2 + tb -> tb_initiating.ta_selectlen;
	    }

	if (tb -> tb_responding.ta_selectlen > 0) 
	    {
	    *vptr++ = VDAT_TSAP_SRV;
	    *vptr++ = tb -> tb_responding.ta_selectlen;
	    bcopy (tb -> tb_responding.ta_selector, 
		   vptr, 
                   tb -> tb_responding.ta_selectlen);
	    vptr += tb -> tb_responding.ta_selectlen;
	    *liptr += 2 + tb -> tb_responding.ta_selectlen;
	    }


	/*
	 *  LI field doesn't include itself so bump up the actual
	 */

        return ( (*liptr) + 1 );
}






/*  */

/* returns header length */

int  T_UnitDataRead (tb, tud, td)

register struct tsapblk *tb;
register struct TSAPunitdata *tud;
struct TSAPdisconnect *td;

{
    int		code;
    char	*vptr;			/* to variable data */
    int	   	len, vlen, savelen;


    vptr = (char*) tud -> tud_qbuf.qb_data;     

    /* 
     *  Get LI.
     */

    vlen = *vptr;
    savelen = vlen;

    /*
     *  Check for valid unitdata TPDU.
     */

    vptr++;

    if (*vptr++ != TPDU_UD)
	return DR_PROTOCOL;

    len = 0;
    vlen--;

    for (; vlen > 0; vptr += len, vlen -= len)
	{
	int	ilen;

	if (vlen < 2)
	    return DR_LENGTH;
   	
	code = *vptr++ & 0xff;
	len = *vptr++ & 0xff;
	if ((vlen -= 2) < len)
	    return NOTOK;

	switch (code) 
	    {
	    case VDAT_TSAP_SRV:
	        if ((ilen = len) > sizeof tud -> tud_called)
		    ilen = sizeof tud -> tud_called;
		    bcopy (vptr, 
			   &tud -> tud_called, 
			   ilen);
		 break;

	     case VDAT_TSAP_CLI:
	         if ((ilen = len) > sizeof tud -> tud_calling)
		     ilen = sizeof tud -> tud_calling;
		     bcopy (vptr, 
			    &tud -> tud_calling,
			    ilen);
		  break;

		case VDAT_OPTIONS:
		case VDAT_ALTERNATE:
		case VDAT_SIZE:
		case VDAT_VRSN:
		case VDAT_SECURITY:
		case VDAT_CHECKSUM:
		case VDAT_ACKTIME:
		case VDAT_THROUGHPUT:
		case VDAT_ERRORATE:
		case VDAT_PRIORITY:
		case VDAT_DELAY:
		case VDAT_TTR:
		    break;

		default: 
		    return NOTOK;

	   }

    }

    /*
     * Update the pointer to the user data.
     */

    tud -> tud_qbuf.qb_data  = (char*) vptr;

   /*
    *  LI field doesn't include itself so bump up the actual
    *  length in savelen to return.
    */

    return (savelen + 1);

}



