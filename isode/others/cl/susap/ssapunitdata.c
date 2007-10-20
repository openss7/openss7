/*
 ****************************************************************
 *                                                              *
 *  HULA project						*
 *                                             			*
 *  program:  	ssapunitdata.c                                  *
 *  author:   	kurt dobbins                                    *
 *  date:     	3/89                                            *
 *                                                              *
 *  This program implements the connectionless ssap unitdata    *
 *  interface.  It implements the ISO 9548 "Connectionless-Mode *
 *  Session Protocol".						*
 *                                  				*
 *  entry points:                   				*
 *                                  				*
 *	SUnitDataBind (sd, calling, called, qos)                *
 *	SUnitDataUnbind (sd)                    		*
 *	SuSave (sd, vecp, vec, si)				*
 *	SUnitDataRequest (calling, called, qos, data, cc, si)   *
 * 	SUnitDataWrite (sd, data, cc, si)                       *
 * 	SUnitDataWriteV (sd, uv)                                *
 *      SUnitDataRead (sd, sud, secs)	                        *
 *      x SSelectUnitDataMask (sd, mask, nfds)                  *
 *	x TUNITDATAser (sig, code, sc)                          *
 *	x SUnitDataWakeUp (tb, td)                              *
 *								*
 *	x = not implemented					*
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



/* LINTLIBRARY */

#include <stdio.h>
#include <signal.h>
#include "spkt.h"
#include "ssap.h"
#include "susap.h"
#include "tusap.h"
#include "uderrors.h"
#include "tailor.h"



#ifdef HULA



/*    DATA */




#define SSAP_NOT_ASYNC 	0
#define SSAP_ASYNC 	1



int	TUNITDATAser ();



/*   
 ****************************************************************
 *								*
 *  SUnitDataBind	                                        *
 *  								*
 *  This routine binds the local address to a socket.           *
 *  Optionally it allows a remote address to be bound as an     *
 *  address pair.  If the remote address is specified, it is    *
 *  considered a permanent binding; otherwise, the bind to a    *
 *  remote address can be done anytime which essentially allows *
 *  dynamic binding (and rebinding) to remote addresses.  But   *
 *  no matter what, the local address must always be specified. *
 *								*
 *  It allocates and initializes a ssap control block for the   *
 *  socket and calls the tsap to initialize unitdata service.	*
 * 								*
 *  This routine must be called prior to using the SUnitData    *
 *  interface for subsequent SUniteWrite requests.  If this     *
 *  isn't called, then SUnitDataRequest must be called for each *
 *  unit data request.						*
 *								*
 *  Note:  BIND has write sematics only.                        *
 *								*
 *								*
 *  returns:	transport descriptor				*
 *              updated si struct if any error (NOTOK) returned *
 *								*
 ****************************************************************
 */

int	SUnitDataBind (sd, local, remote, qos, si)

int    			sd;
struct SSAPaddr 	*local;
struct SSAPaddr 	*remote;
struct QOStype  	*qos;
struct SSAPindication 	*si;

{

    int     fd;
    SBV     smask;
    int     result;
    register struct ssapblk *sb;
    struct  TSAPdisconnect  td;

#ifdef HULADEBUG
	printf ("\n     in SUnitDataBind \n");
#endif

    isodetailor ("ssap");

/*
 *  Check for missing parameters.
 */

    missing_udP (si);
    if (!local && !remote)
	return susaplose (si,
			  SC_PARAMETER,
		  	  NULLCP,
			  SuErrString (UDERR_MISSING_PARAMETER));
			  		

/*
 *  Check if we need to create a new socket or just
 *  reuse (essentially "rebind") the current socket 
 *  for the specified address pair.
 */

    if (sd < 0)
        {	
	/*
 	 *  Allocate a new ssap block for this calling/called address pair.
 	 */
#ifdef HULADEBUG
	printf ("\n     allocating new ssap block \n");
#endif

    	if ( (sb = newsublk () ) == NULL)
	    return susaplose (si,
			      SC_CONGEST,
			      NULLCP,
			      SuErrString (UDERR_NO_MEMORY));
	}
    else
	{
	/* 
 	 *  Find the correct session block and set the signal mask.
 	 */ 

    	if ((sb = findsublk (sd)) == NULL)
	    {

	    /*
             *  Check for special case where a server has set up a 
	     *  listen socket and is now binding the address so the 
	     *  socket exists but no ssap block has been set up.
	     */

	    if (local)
	        {
#ifdef HULADEBUG
	        printf ("\n     allocating new ssap block for listen socket\n");
#endif
    	        if ( (sb = newsublk () ) == NULL)
	            return susaplose (si, 
				      SC_CONGEST, 
				      NULLCP, 
				      SuErrString(UDERR_NO_MEMORY));
	        }
	    else
	        {
	        (void) sigsetmask (smask); 
	  	return susaplose (si,
				  SC_PARAMETER,
				  NULLCP,
				  SuErrString(UDERR_INVALID_SESSION_DESC)); 
		}
	    }

#ifdef HULADEBUG
	printf ("\n     re-binding on current ssap block \n");
#endif
	}


    /*
     *  Set up the addresses if not specified.
     */
   
    if (local == NULLSA)
        {
	static struct SSAPaddr salocal;

	local = &salocal;
	bzero ((char *) local, sizeof *local);
	}

    if (remote == NULLSA)
        {
	static struct SSAPaddr saremote;

	remote = &saremote;
	bzero ((char *) remote, sizeof *remote);
	}

    if (local -> sa_selectlen == 0) 
        {
	local -> sa_port = htons((u_short)(0x8000 | (getpid() & 0x7fff)));
	local -> sa_selectlen = sizeof local -> sa_port;
	}
    	
/*
 *  Now bind (or re-bind) the address pair and get back a socket descriptor.
 */

    fd = TUnitDataBind (sd, &local -> sa_addr, &remote -> sa_addr, qos, &td);

    if (fd == NOTOK)
	{
	freesublk (sb);
	return ts2suslose (si, "TUnitDataBind", &td);
	}

/*
 *  Now finish setting up the control block.
 */

    sb -> sb_fd = fd;

    sb -> sb_version = SB_VRSN1_CLNS;	/* version 1 connectionless */

    sb -> sb_flags = SB_CLNS;		/* connectionless service */


#if FALSE
    sb -> sb_tsdu_us = 0;
#endif

    sb -> sb_initiating = *local;	/* strcut copy */
    
    sb -> sb_responding = *remote;	/* struct copy */

#if FALSE
    if (qos)
	sb -> sb_qos = *qos;	
#endif

    return fd;

}



  

/*  */

/*
 ****************************************************************
 *								*
 *  SUnitDataUnbind	                                        *
 *  								*
 *  This routine releases datagram service for the session      *
 *  provider.  It frees the ssap block for the socket and then  *
 *  closes the socket. 						*
 *								*
 *  returns:	OK, NOTOK					*
 *								*
 ****************************************************************
 */

int     SUnitDataUnbind (sd, si)

int	sd;
struct  SSAPindication *si;

{

register struct ssapblk *sb;
int	 result;
SBV      smask;			 	/* signal save mask */
struct  TSAPdisconnect  td;


#ifdef HULADEBUG
	printf ("\n     in SUnitDataUnbind \n");
#endif


/* 
 *  Find the correct session block and set the signal mask.
 */ 

    ssap_udPsig (sb, sd);

/*
 *  Close the transport socket and free its resources.
 */

    if ( (result = TUnitDataUnbind (sb -> sb_fd, &td)) != OK )
	return NOTOK;
    
/*
 *  Now free the ssapblk.
 */

#ifdef HULADEBUG
	printf ("\n     freeing the ssap block \n");
#endif
	
    freesublk (sb);

    return OK; 

}

/*  */

/*   
 ****************************************************************
 *								*
 *  SuSave		                                        *
 *  								*
 *  This routine sets up a control block path and socket for    *
 *  datagram that has arrived on the server listen socket.      *
 *								*
 *  returns:	socket descriptor from TuSave			*
 *								*
 ****************************************************************
 */

int	SuSave (sd, vecp, vec, si)

int	 	 sd;
register int     vecp;
register char  **vec;
register struct  SSAPindication *si;

{

    register struct ssapblk *sb;
    struct   TSAPdisconnect tds;
    struct   TSAPdisconnect *td = &tds;
    int	     result;	     
    SBV      smask;			 	/* signal save mask */


    missing_udP (vec);
    missing_udP (si);


    /*
     *  Check if we need to create a new socket or just
     *  reuse an already bound socket.
     */

    if (sd < 0)
        {	
	/*
 	 *  Allocate a new ssap block for this calling/called address pair.
 	 */

    	if ( (sb = newsublk () ) == NULL)
	    return susaplose (si,
			      SC_CONGEST,
			      NULLCP,
			      SuErrString (UDERR_NO_MEMORY));
	}
    else
	{
	/* 
 	 *  Find the correct session block and set the signal mask.
 	 */ 

    	if ((sb = findsublk (sd)) == NULL)
	    {

	    (void) sigsetmask (smask); 
	     return susaplose (si,
			       SC_PARAMETER,
			       NULLCP,
			       SuErrString(UDERR_INVALID_SESSION_DESC)); 
	    }

	}


    /*  
     *  Now pass thru the request to TSAP.
     */

    if ( (result = TuSave (sd, vecp, vec, td) ) == NOTOK)
	return ts2suslose ( si, "TuSave", td);

    return result;

}



/*  */
 
/*   
 ****************************************************************
 *								*
 *  SUnitDataRequest                                            *
 *  								*
 *  This routine implements the S-UNITDATA.REQUEST primitive    *
 *  for writing data thru a datagram session service.  This     *
 *  routine is used for unitdata service to remote addresses    *
 *  that havn't been binded.  This routine blocks until the     *
 *  data is written.						*
 *  								*
 ****************************************************************
 */

int     SUnitDataRequest (calling, called, data, cc, qos, si)

struct SSAPaddr *calling,
		*called;
register char	*data;
int		cc;
struct QOStype  *qos;
struct  SSAPindication *si;


{

int	sd;


#ifdef HULADEBUG
	printf ("\n     in SUnitDataRequest \n");
#endif


/*
 *  Create the socket on the fly.  We do the bind here because
 *  tsap data writes expect iovecs instead of char data ptr.
 */

    sd = SUnitDataBind (NOTOK, calling, called, qos, si);

    if (sd == NOTOK)
	return NOTOK;

/*  
 *  Now do the S_UNITDATA send.
 */

    if (SUnitDataWrite (sd, data, cc, si) == NOTOK)
	return NOTOK;

/*
 *  Unbind the socket.
 */

    SUnitDataUnbind (sd, si);

    return OK;

}




/*  */

/*
 ****************************************************************
 *                                                              *
 *  SUnitDataWrite 						* 
 *      							*
 *  This routine implements the ssap unit write interface for   *
 *  writing user data over transport datagram service where     *
 *  the datagram service has already been established and the   *
 *  remote address has been binded on the socket.               *
 *								*
 *  This routine blocks until completion.			*
 *								*
 ****************************************************************
 */

int     SUnitDataWrite (sd, data, cc, si)

int 	 	sd;
register char	*data;
int		cc;
struct  SSAPindication *si;

{

int 	 n, len, j;
SBV      smask;			 	/* signal save mask */
int      result; 		 	/* write result     */
register struct ssapblk *sb;	 	/* ssap ctl blk ptr */

register struct ssapkt *s;
struct TSAPdisconnect   tds;
register struct TSAPdisconnect *td = &tds;
struct udvec vvs[3];
register struct udvec  *vv, *xv;


#ifdef HULADEBUG
	printf ("\n     in SUnitDataWrite \n");
	printf ("\n     writing on socket %d \n", sd);
#endif

    /*
     * 	Check for missing parameters.
     */

    missing_udP (data);
    missing_udP (si);

    /*
     *  Check user data.
     */

    if ( (cc <= 0) || (cc > UD_MAX_DATA) )
	return susaplose (si,
			  SC_PARAMETER,
			  NULLCP,
		  	  SuErrString(UDERR_ILLEGAL_UD_SIZE));

    /* 
     *  Block any signals while we do the write.
     */

    smask = sigioblock ();

    /* 
     *  Find the correct session block and set the signal mask.
     */ 

    ssap_udPsig (sb, sd);
    
    /*
     *  Init our working udvec struct.
     */

    vv = vvs;
    vvs[0].uv_base = vvs[1].uv_base = vvs[2].uv_base = NULL;

    /*
     *  Allocate a new UNITDATA spkt for this datagram message.
     */

#ifdef HULADEBUG
	printf ("\n     allocating new SPKT \n");
#endif

    if ((s = newspkt (SPDU_UD)) == NULL)
        return susaplose (si,
			  SC_CONGEST,
			  NULLCP,
			  SuErrString(UDERR_NO_MEMORY));

    /*
     *  Set the parameter field values in the spkt for encoding.
     */

    s -> s_mask |= (SMASK_SPDU_UD | SMASK_UD_VERSION);

    s -> s_ud_version = sb -> sb_version;

    if (sb -> sb_initiating.sa_selectlen > 0)
        {
	s -> s_mask |= SMASK_UD_CALLING;
	bcopy (sb -> sb_initiating.sa_selector, s -> s_ud_calling,
	        s -> s_ud_callinglen = sb -> sb_initiating.sa_selectlen);
	}
    if (sb -> sb_responding.sa_selectlen > 0)
 	{
	s -> s_mask |= SMASK_UD_CALLED;
	bcopy (sb -> sb_responding.sa_selector, s -> s_ud_called,
		s -> s_ud_calledlen = sb -> sb_responding.sa_selectlen);
    	}

#ifdef HULADEBUG	
	spkt2text (stdout, s, NULL);
#endif

    /*
     *  Format the session protocol data unit.
     *  The encode routine will put the SPDU header in the base
     *  portion of the work udvec[0] passed to it.
     */

#ifdef HULADEBUG	
	printf ("\n     formatting the unitdata SPDU \n");
#endif

     if (spkt2tsdu (s, &vv -> uv_base, &vv -> uv_len) == NOTOK)
        {
	susaplose (si, 
		   SC_PROTOCOL,
		   NULLCP,
		   SuErrString (UDERR_ENCODE_UDSPDU_FAILED));
	freespkt (s);
	return NOTOK;
	}

#ifdef HULADEBUG
	for (j = 0; j < vv->uv_len; j++)
	    printf ( " %x ", *(vv->uv_base + j) );
#endif		



    /*
     *  Now copy the user data to the work udvec.  The SPDU UD header
     *  was encoded and put as the 1st base element so put the user
     *  data as the second element.
     */

    xv = ++vv;

    xv -> uv_base = data;
    xv -> uv_len = cc;

    freespkt (s);

    s = NULL;

   /*
    *  Do the unit data write over the TSAP unitdata service.
    */

#ifdef HULADEBUG
	printf ("\n     calling the TSAP unitdata write \n");
#endif

    if ( (result = TUnitDataWrite (sb -> sb_fd, vvs, td)) == NOTOK)
        ts2suslose (si, "TUnitDataWrite", td);

    /*  
     *  Free the encoded header.
     */
	
    free (vvs[0].uv_base);

    /*
    *  Restore the mask.
    */

    (void) sigiomask (smask);

    if (result == NOTOK)
    	return NOTOK;
    else
        return OK;


}





/*  */


#define	NSPUV	12	/* really should be MSG_MAXIOVLEN - 4 */


/*
 ****************************************************************
 *                                                              *
 *  SUnitDataWriteV 						* 
 *      							*
 *  This routine implements the ssap unit write interface for   *
 *  writing user vextors over transport datagram service where  *
 *  the datagram service has already been established and the   *
 *  remote address has been binded on the socket.               *
 *								*
 *  This routine blocks until completion.			*
 *								*
 ****************************************************************
 */

int     SUnitDataWriteV (sd, uv, si)

int 	 sd;
register struct udvec	*uv;


{

int 	 n, cc, len, j;
SBV      smask;			 	/* signal save mask */
int      result; 		 	/* write result     */
register struct ssapblk *sb;	 	/* ssap ctl blk ptr */

register struct ssapkt *s;
struct TSAPdisconnect   tds;
register struct TSAPdisconnect *td = &tds;
struct udvec vvs[NSPUV];
register struct udvec  *vv, *xv;


#ifdef HULADEBUG
	printf ("\n     in SUnitDataWriteV \n");
	printf ("\n     writing on socket %d \n", sd);
#endif

    /*
     * 	Check for missing parameters.
     */

    missing_udP (uv);
    missing_udP (uv -> uv_base);

    /*
     *  Check user data.
     */

    n = 0;
    for (vv = uv; vv -> uv_base; vv++)
	n += vv -> uv_len;

#ifdef HULADEBUG
	printf ("\n     len of data in vectors = %d \n", n);
#endif

    if ( (n <= 0) || (n > UD_MAX_DATA) )
	return susaplose (si,
			  SC_PARAMETER,
			  NULLCP,
			  SuErrString(UDERR_ILLEGAL_UD_SIZE));


    /* 
     *  Block any signals while we do the write.
     */

    smask = sigioblock ();

    /* 
     *  Find the correct session block and set the signal mask.
     */ 

    ssap_udPsig (sb, sd);
    
    /*
     *  Init our working udvec struct.
     */

    vv = vvs;
    vvs[0].uv_base = vvs[1].uv_base = NULL;

    /*
     *  Allocate a new UNITDATA spkt for this datagram message.
     */

#ifdef HULADEBUG
	printf ("\n     allocating new SPKT \n");
#endif

    if ((s = newspkt (SPDU_UD)) == NULL)
        return susaplose (si,
			  SC_CONGEST,
			  NULLCP,
			  SuErrString(UDERR_NO_MEMORY));

    /*
     *  Set the parameter field values in the spkt for encoding.
     */

    s -> s_mask |= (SMASK_SPDU_UD | SMASK_UD_VERSION);

    s -> s_ud_version = sb -> sb_version;

    if (sb -> sb_initiating.sa_selectlen > 0)
        {
	s -> s_mask |= SMASK_UD_CALLING;
	bcopy (sb -> sb_initiating.sa_selector, s -> s_ud_calling,
	        s -> s_ud_callinglen = sb -> sb_initiating.sa_selectlen);
	}
    if (sb -> sb_responding.sa_selectlen > 0)
 	{
	s -> s_mask |= SMASK_UD_CALLED;
	bcopy (sb -> sb_responding.sa_selector, s -> s_ud_called,
		s -> s_ud_calledlen = sb -> sb_responding.sa_selectlen);
    	}

#ifdef HULADEBUG	
	spkt2text (stdout, s, NULL);
#endif

    /*
     *  Format the session protocol data unit.
     *  The encode routine will put the SPDU header in the base
     *  portion of the work udvec[0] passed to it.
     */

#ifdef HULADEBUG	
	printf ("\n     formatting the unitdata SPDU \n");
#endif

     if (spkt2tsdu (s, &vv -> uv_base, &vv -> uv_len) == NOTOK)
        {
	susaplose (si,
		   SC_PROTOCOL,
		   NULLCP,
		   SuErrString(UDERR_ENCODE_UDSPDU_FAILED));
	freespkt (s);
	return NOTOK;
	}

#ifdef HULADEBUG
	for (j = 0; j < vv->uv_len; j++)
	    printf ( " %x ", *(vv->uv_base + j) );
#endif		



    /*
     *  Now copy the user udvec to the work udvec.  The SPDU UD header
     *  was encoded and put as the 1st base element so start the copy 
     *  from there. Note:  we just copy the pointers to the user info.
     */

    xv = ++vv;

#ifdef HULADEBUG
	printf ("\n     header len in [1] = %d \n", xv->uv_len);
#endif	

    len = n;

    for (vv = uv; vv -> uv_base; vv++, xv++)
	{
	/*
	 *  Copy the user base to the work base.
	 */
	xv -> uv_base = vv -> uv_base;
        xv -> uv_len  = vv -> uv_len;
	len -= vv -> uv_len;
#ifdef HULADEBUG
	printf ("\n     in copy...len = %d \n", xv->uv_len);
#endif	
	}

    if (len > 0)
	{
	susaplose (si,
		   SC_PARAMETER,
		   NULLCP,
		   SuErrString(UDERR_TOO_MANY_VECTORS));
	freespkt (s);
        if (vvs[0].uv_base)
	    free (vvs[0].uv_base);
	return NOTOK;
	}
	
    /*
     *  Set the last base to NULL (by convention to signal end).
     */

    xv -> uv_base = NULL;
    xv -> uv_len = 0;

    freespkt (s);

    s = NULL;

   /*
    *  Do the unit data write over the TSAP unitdata service.
    */

#ifdef HULADEBUG
	printf ("\n     calling the TSAP unitdata write \n");
	for (j = 0; j < vvs[0].uv_len; j++)
	    printf ( " %x ", *((vvs[0].uv_base) + j) );
	for (j = 0; j < 25; j++)
	    printf ( " %x ", *((vvs[1].uv_base) + j) );
#endif

    if ( (result = TUnitDataWrite (sb -> sb_fd, vvs, td)) == NOTOK)
        ts2suslose (si, "TUnitDataWrite", td);

    /*  
     *  Free the encoded header.
     */
	
    free (vvs[0].uv_base);

    /*
    *  Restore the mask.
    */

    (void) sigiomask (smask);

    if (result == NOTOK)
    	return NOTOK;
    else
        return OK;


}




/*  */ 

/*
 ****************************************************************
 *								*
 *  SUnitDataRead						*
 *  								*
 *  This routine is the high-level routine that reads datagram  *
 *  messages from the tsap unit data service.  It calls the     *
 *  auxilliary routine that does the actual read.               *
 *								*
 *  returns:  OK, NOTOK, DONE 	 				*
 *								*
 ****************************************************************
 */

int	SUnitDataRead (sd, sud, secs, si)

int	sd;
struct  SuSAPstart     *sud;
int	secs;
struct  SSAPindication *si;

{

    SBV	    smask;
    int     result;
    register struct ssapblk *sb;
    register struct TSAPunitdata tx;

#ifdef HULADEBUG
	printf ("\n     in SUnitDataRead \n");
	printf ("\n     reading on socket %d \n", sd);
#endif

    missing_udP (sud);
    missing_udP (si);

    /* 
     *  Block any signals while we do the read.
     */

    smask = sigioblock ();

    /* 
     *  Find the correct session block and set the signal mask.
     */ 

    ssap_udPsig (sb, sd);

    /* 
     *  Do the unitdata read. 
     */
    
    result = SUnitDataReadAux (sb, sud, secs, si, SSAP_NOT_ASYNC, &tx);

    /*
     *  Restore the mask.
     */

    (void) sigiomask (smask);

    /*
     *  Return the result.
     */

    return result;

}





/*  */ 

/*
 ****************************************************************
 *								*
 *  SUnitDataReadAux						*
 *  								*
 *  This routine is the low-level routine that does tha actual  *
 *  unitdata read from the tsap.  It blocks until data is read  *
 *  or secs expires.  If secs is 0, it blocks indefinitely.	*
 *								*
 *  returns:  OK, NOTOK 					*
 *								*
 ****************************************************************
 */

static int  SUnitDataReadAux (sb, sud, secs, si, async, tud)

register struct ssapblk *sb;
register struct SuSAPstart *sud;
int	secs;
struct SSAPindication *si;
int	async;
register struct TSAPunitdata   *tud;


{
    register struct ssapkt *s;

#ifdef HULADEBUG
	printf ("\n     in SUnitDataReadAux \n");
#endif

    /*
     *  Ready the sap unitdata structure for receive.
     */

    bzero ((char *) sud, sizeof *sud);
    bzero ((char *) si, sizeof *si);

    for ( ; (s = sb2udspkt (sb, si, secs, tud)) != NULL; )
	{
	if (!(s -> s_mask & SMASK_SPDU_UD))
	    break;

	if (sb -> sb_len > 0)
	    {
	    switch (s -> s_code)
	 	{
		case SPDU_UD:
		    if (s -> s_mask & SMASK_SPDU_UD)
			break;
		default:
    		    freespkt (s);
		    return susaplose (si,
				      SC_PROTOCOL,
				      NULLCP,
				      SuErrString(UDERR_UNEXPECTED_SPDU_TYPE));
	        }

#ifdef HULADEBUG
	printf ("\n     got a UNITDATA SPDU \n");
	for (secs=0; secs < 25; secs++)
	    printf (" %x ", *(tud->tud_qbuf.qb_data + secs) );
#endif
	    
	    sb -> sb_code = s -> s_code;

	    /* 
             *  Format the unitdata indication.
	     */

	    sud -> ss_sd = sb -> sb_fd;

	    /*  
             *  Copy the TSAP addresses.
	     */

	    bcopy (&tud -> tud_calling, 
		   &sud -> ss_calling.sa_addr, 
		   sizeof (sud -> ss_calling.sa_addr) );

	    bcopy (&tud -> tud_called,
		   &sud -> ss_called.sa_addr,
		   sizeof (sud -> ss_called.sa_addr) );
			    

	    /*
             *  Copy the SSAP selectors.
	     */

       	    sud -> ss_calling.sa_selectlen = s -> s_ud_callinglen;

	    bcopy (s -> s_ud_calling, 
		   sud -> ss_calling.sa_selector, 		   
		   s -> s_ud_callinglen);

       	    sud -> ss_called.sa_selectlen = s -> s_ud_calledlen;

	    bcopy (s -> s_ud_called,
		   sud -> ss_called.sa_selector,
		   s -> s_ud_calledlen);

	   /*
            *  Copy in the data and misc.
	    */
		
	    sud -> ss_ssdusize = UD_MAX_DATA;
	    sud -> ss_version = s -> s_ud_version;

	    sud -> ss_cc = tud -> tud_qbuf.qb_len;
	    sud -> ss_data = tud -> tud_qbuf.qb_data;
	    sud -> ss_base = tud -> tud_base;

	    sb -> sb_len = 0;

	    freespkt (s);

	    return OK;

	    }  /* end if > 0 */

	   /*
	    *  Check if timeout with no data on read.
	    */

	    if (si -> si_abort.sa_reason == SC_TIMER)	        
	        break;

    	}   /* end for loop */

    if (si -> si_abort.sa_reason == SC_TIMER)
	{
	sud -> ss_cc = 0;
	sud -> ss_data = NULL;
	sud -> ss_base = NULL;
	return OK;
	}

    return NOTOK;
}





#if FALSE

/*    define vectors for INDICATION events */

int	SSetIndications (sd, data, tokens, sync, activity, report, finish,
		abort, si)
int	sd;
IFP	data,	
	tokens,
	sync,
	activity,
	report,
	finish,
	abort;
struct SSAPindication *si;
{
    SBV     smask;
    register struct ssapblk *sb;
    struct TSAPdisconnect   tds;
    register struct TSAPdisconnect *td = &tds;

    if (data || tokens || sync || activity || report || finish || abort) {
	missingP (data);
	missingP (tokens);
	missingP (sync);
	missingP (activity);
	missingP (report);
	missingP (finish);
	missingP (abort);
    }

    smask = sigioblock ();

    ssapPsig (sb, sd);

    if (TSetIndications (sb -> sb_fd, TDATAser, TDISCser, td) == NOTOK)
	if (td -> td_reason == DR_WAITING)
	    return ssaplose (si, SC_WAITING, NULLCP, NULLCP);
	else
	    return ts2sslose (si, "TSetIndications", td);

    if (sb -> sb_DataIndication = data)
	sb -> sb_flags |= SB_ASYN;
    else
	sb -> sb_flags &= ~SB_ASYN;
    sb -> sb_TokenIndication = tokens;
    sb -> sb_SyncIndication = sync;
    sb -> sb_ActivityIndication = activity;
    sb -> sb_ReportIndication = report;
    sb -> sb_ReleaseIndication = finish;
    sb -> sb_AbortIndication = abort;

    (void) sigiomask (smask);

    return OK;
}

#endif





/*  */

struct ssapkt   *sb2udspkt (sb, si, secs, tud)

register struct ssapblk *sb;
register struct SSAPindication *si;
int     secs;
register struct TSAPunitdata   *tud;

{
    int     			cc;
    register struct ssapkt   	*s;
    struct TSAPdisconnect  	tds;
    register struct TSAPdisconnect *td = &tds;

    /*
     *  Read the tsap datagram.
     */

#ifdef HULADEBUG
	printf ("\n     calling TUnitDataRead \n");
#endif

    if (TUnitDataRead (sb -> sb_fd, tud, secs, td) == NOTOK)
	{
	if (td -> td_reason == DR_TIMER)	    
            si -> si_abort.sa_reason = SC_TIMER;

	sb -> sb_len = 0;
        return ts2suslose (si, "TUnitDataRead", td);
	}

    /*
     *  Decode the SPDU unitdata from the TSDU.
     */

#ifdef HULADEBUG
	printf ("\n     getting ready to decode the tsdu \n");
#endif

    if ( ((s = udtsdu2spkt (&tud->tud_qbuf, tud->tud_cc) ) == NULL)
	    || s -> s_errno != OK)
	{
	freespkt (s);
	TUDFREE (tud);
	return susaplose (si,
			  SC_PROTOCOL,
			  NULLCP,
			  SuErrString(UDERR_DECODE_UDSPDU_FAILED));
	}

#ifdef HULADEBUG	
    spkt2text (stdout, s, NULL);
#endif

    switch (s -> s_code) 
	{
	case SPDU_UD: 

	    /*
 	     *  Check if data got queued on the qbuf chain.
	     */
	
	    if (tud -> tud_qbuf.qb_forw != &tud -> tud_qbuf)
	        {
#ifdef HULADEBUG
	        printf ("\n     got data in the spkt ... format indication \n");
#endif
		s -> s_qbuf.qb_data = tud -> tud_qbuf.qb_data;
		s -> s_qbuf.qb_forw -> qb_back =
			s -> s_qbuf.qb_back -> qb_forw = &s -> s_qbuf;
		s -> s_qlen = tud -> tud_cc;
	        }

	    sb -> sb_spdu = s;	
	    sb -> sb_len = tud -> tud_cc;
	    return s;

	default:
	    sb -> sb_spdu = NULL;
	    freespkt (s);
	    TUDFREE (tud);
	    return NULL;
        }

}




#if FALSE


/*  */

/*
 ****************************************************************
 *								*
 *  SUnitDataSetIndications					*
 *  								*
 *  This routine is sets the data handler routine to be called  *
 *  on unitdata indications from the transport.			*
 *								*
 *  returns:  OK, NOTOK 					*
 *								*
 ****************************************************************
 */

static int  TUNITDATAser (sd, tud)
int     sd;
register struct TSAPunitdata   *tx;
{
    IFP	    abort;
    register struct ssapblk *sb;
    struct SSAPdata sxs;
    register struct SSAPdata   *sx = &sxs;
    struct SSAPindication   sis;
    register struct SSAPindication *si = &sis;
    register struct SSAPabort  *sa = &si -> si_abort;

    if ((sb = findsublk (sd)) == NULL)
	return;

    abort = sb -> sb_AbortIndication;

    for (;; tx = NULLTX) {
	switch (SReadRequestAux (sb, sx, OK, si, 1, tx)) {
	    case NOTOK: 
		(*abort) (sd, sa);
		return;

	    case OK: 
		(*sb -> sb_DataIndication) (sd, sx);
		break;

	    case DONE: 
		switch (si -> si_type) {
		    case SI_TOKEN: 
			(*sb -> sb_TokenIndication) (sd, &si -> si_token);
			break;

		    case SI_SYNC: 
			(*sb -> sb_SyncIndication) (sd, &si -> si_sync);
			break;

		    case SI_ACTIVITY: 
			(*sb -> sb_ActivityIndication) (sd, &si -> si_activity);
			break;

		    case SI_REPORT: 
			(*sb -> sb_ReportIndication) (sd, &si -> si_report);
			break;

		    case SI_FINISH: 
			(*sb -> sb_ReleaseIndication) (sd, &si -> si_finish);
			break;

		    case SI_DATA: /* partially assembled (T)SSDU */
			break;
		}
		break;
	}

	if (sb -> sb_spdu == NULL)
	    break;
    }
}

#endif







#endif
