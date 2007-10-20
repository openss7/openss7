/*
 ****************************************************************
 *                                                              *
 *  HULA project						*
 *                                             			*
 *  program:  	tsapunitdata.c                                  *
 *  author:   	kurt dobbins                                    *
 *  date:     	12/88                                           *
 *                                                              *
 *  This program implements the connectionless tsap unitdata    *
 *  interface.  It is designed to provide generic datagram      *
 *  transport service over multiple connectionless transports.  *
 *                                  				*
 *  entry points:                   				*
 *                                  				*
 *	TUnitDataListen (ta, qos, td)	 	                *
 *	TuSave (sd, vecp, vex, td)				*
 *	TUnitDataBind (sd, calling, called, qos, td)            *
 *	TUnitDataUnbind (sd)                    		*
 *	TUnitDataRequest (calling, called, qos, data)           *
 * 	TUnitDataWrite (sd, uv)                                 *
 *      TUnitDataRead (sd, tud, secs)	                        *
 *      TSelectUnitDataMask (sd, mask, nfds)                    *
 *	UNITDATAser (sig, code, sc)                             *
 *	TUnitDataWakeUp (tb, td)                                *
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
#include <signal.h>
#include "tpkt.h"
#include "tsap.h"
#include "tusap.h"
#include "isoservent.h"
#include "tailor.h"
#include "internet.h"
#include "uderrors.h"


struct TSAPaddr *newtuaddr ();


#define	selmask(fd,m,n) \
{ \
    FD_SET (fd, &(m)); \
    if ((fd) >= (n)) \
	(n) = (fd) + 1; \
}


#ifdef HULA


/*    STATIC DATA */


#ifdef WITHOUTCONS

/*
 *  Only allocate these if built
 *  WITHOUT the connection-oriented service.
 */

static int once_only = 0;
static struct tsapblk tsapque;
static struct tsapblk *THead = &tsapque;

#endif

#ifndef	SIGPOLL
static int TPid = NOTOK;
#endif





/*  */

/*   
 ****************************************************************
 *								*
 *  TUnitDataListen	                                        *
 *  								*
 *  This routine creates the "listen" socket for transport      *
 *  addresses that support datagrams. 				*
 *								*
 *  returns:	socket descriptor				*
 *								*
 ****************************************************************
 */

int	TUnitDataListen (listen, qos, td)

struct TSAPaddr *listen;
struct QOStype  *qos;
struct TSAPdisconnect *td;

{

int      result;
register int n = listen -> ta_naddr - 1;
register struct NSAPaddr *na = listen -> ta_addrs;
register struct NSAPaddr *la;
register struct tsapblk *tb;


#ifdef HULADEBUG
	printf ("\n     in TUnitDataListen \n");
#endif


    /*
     *  Check for missing parms.
     */

    missing_udP (listen);
    missing_udP (td);

    /*
     *  Allocate a new tsap block for this listen.
     */

    if ((tb = newtublk ()) == NULL)
	return tusaplose (td, 
			  DR_CONGEST, 
			  NULLCP, 
			  TuErrString(UDERR_NO_MEMORY) );

    /*
     *  Find the type of transport service to use.
     *  The idea is that if the listen address has 
     *  multiple network addresses, we will setup 
     *  a listen on each one.
     */

    for (; n >= 0; na++, n--)
	{
	switch (na -> na_type)
	    {
	    /*
  	     *  For now, we only support UDP (which is the default service
 	     *  if no addresses are supplied. 
             */
	    case NA_TCP:
	    		
		udpinit (tb);

   	    	/*
		 *  Now finish setting up the control block.
     		 */

    		tb -> tb_srcref = htons ((u_short) (getpid () & 0xffff));
    		tb -> tb_dstref = htons ((u_short) 0);

    		/*  
     		 *  Set the network address to be only the one we care about.
     		 */

#if FALSE
    		tb -> tb_initiating = *newtuaddr (calling, la);	
    		tb -> tb_responding = *newtuaddr (called, na);	
#endif

    		if (qos)
		tb -> tb_qos = *qos;	/* struct copy */

		/*
		 *  Allocate the local socket to listen on.
		 */

		if ( (result = (*tb -> tb_UnitDataStart) (tb,
							 na,
							 NULL,
							 TUNITDATA_START,
							 td)
			 ) != OK)
		    {
		    continue;
		    }
		break;

	    default:
		return tusaplose ( td, 
				  DR_PARAMETER,
				  NULLCP,  
				  TuErrString(UDERR_NSAP_NOT_SUPPORTED) );
	    }

	break;
        }   /* end of for number of net addresses */


    if (tb -> tb_fd == NOTOK)
	{
	freetublk (tb);
	return NOTOK;
	}

   return (tb -> tb_fd);

}






/*  */

/*   
 ****************************************************************
 *								*
 *  TUnitDataBind	                                        *
 *  								*
 *  This routine sets up datagram service for the transport.    *
 *  It allocates and initializes a tsap control block for the   *
 *  calling/called address pair and calls the transport to      *
 *  initialize unitdata service.  This routine drives the       *
 *  underlying transport service to create a socket and bind    *
 *  the remote address.						*
 *								*
 *  If the remote address is specified, it is considered a      *
 *  permanent binding; otherwise, the bind to a remote address  *
 *  can be done anytime which essentially allows dynamic        *
 *  binding (and rebinding) to remote addresses.                *
 * 								*
 *  This routine must be called prior to using the TUnitData    *
 *  interface for subsequent TUniteWrite requests.  If this     *
 *  isn't called, then TUnitDataRequest must be called for each *
 *  unit data request.						*
 *								*
 *								*
 *  Note:  BIND has write sematics only.                        *
 *								*
 *								*
 *  returns:	socket descriptor				*
 *              updated ti struct if any error (NOTOK) returned *
 *								*
 ****************************************************************
 */

int	TUnitDataBind (sd, calling, called, qos, td)

int			sd;
struct TSAPaddr 	*calling,
			*called;
struct QOStype  	*qos;
struct TSAPdisconnect 	*td;


{

int      result;
register int n; 
register struct NSAPaddr *na;
register int l;
register struct NSAPaddr *la;
register struct tsapblk *tb;
int	 option;
SBV      smask;			 	/* signal save mask */


#ifdef HULADEBUG
	printf ("\n     in TUnitDataBind \n");
#endif

    isodetailor ("tsap");

    /*
     *  Check for missing parms.
     */

    missing_udP (td);

    /*
     *  Check if we need to create a new socket or just
     *  reuse (essentially "rebind") the current socket 
     *  for the specified address pair.
     */

    if (sd < 0)
        {	
	/*
 	 *  Allocate a new tsap block for this calling/called address pair.
 	 */
#ifdef HULADEBUG
	printf ("\n     allocating new tsap block \n");
#endif

    	if ( (tb = newtublk () ) == NULL)
	    return tusaplose (td, 
			      DR_CONGEST, 
			      NULLCP, 
			      TuErrString(UDERR_NO_MEMORY) );

	option = TUNITDATA_START;

	}
    else
	{
	/* 
 	 *  Find the correct tsap block and set the signal mask.
 	 */ 

#ifdef HULADEBUG
	printf ("\n     re-binding on current tsap block \n");
#endif

	/*  
         *  There had better be a remote addr to bind to.
         */

	if (called == NULLTA)
	    return tusaplose (td, 
			      DR_PARAMETER, 
			      NULLCP, 
			      TuErrString(UDERR_NO_REMOTE_ADDR) );

        tsap_udPsig (tb, sd);

	option = TUNITDATA_BIND;

	}


    if (calling == NULLTA)
        {
	static struct TSAPaddr tas_calling;

	/* 
	 *   Zero-fill the calling address.
	 */

	calling = &tas_calling;
	bzero ((char *) calling, sizeof *calling);
        }

    if (called == NULLTA)
        {
	static struct TSAPaddr tas_called;

	/* 
	 *   Zero-fill the called address.
	 */

	called = &tas_called;
	bzero ((char *) called, sizeof *called);
        }

    if (called -> ta_selectlen > 0 && calling -> ta_selectlen == 0)
        {
	calling -> ta_port = htons ((u_short) (0x8000 | (getpid () & 0x7fff)));
	calling -> ta_selectlen = sizeof calling -> ta_port;
        }


    /*
     *  Set the network address from the called address if it is specified
     *  otherwise default the address to the calling (so we can init the 
     *  transport that it supports).
     */

    if (n = (called -> ta_naddr - 1) < 0)
        {
        /*
         *  No called network address.  
	 *  Default it to be the local if it exists.
         */

        if (n = (calling -> ta_naddr - 1) < 0)
	    {
 	    freetublk (tb);
	    return tusaplose ( td, 
			       DR_PARAMETER, 
			       NULLCP, 
			       TuErrString(UDERR_NO_NSAP_ADDRS) );
	    }
        else
	    {
	    /*  
             *  Set the default addr type so we can continue.
	     */
	    called -> ta_addrs[0].na_type = calling -> ta_addrs[0].na_type;
	    }
        }

    /*
     *  Now find the type of transport service to use.
     *  The idea is that if the called service address has 
     *  multiple network addresses, we will use the address 
     *  that matches our local (calling) address type.
     *  If the called service address has no network address,
     *  then we will default to the local network address.
     */


    for (na = called -> ta_addrs; n >= 0; na++, n--)
	{
	for (l = calling -> ta_naddr - 1, la = calling -> ta_addrs;
		l >= 0;
		la++, l--)
	    if (la -> na_type == na -> na_type)
		break;

	if (l < 0)
	    la = NULLNA;

	switch (na -> na_type)
	    {
	    /*
  	     *  For now, we only support UDP (which is the default service
 	     *  if no addresses are supplied. 
             */

	    case NA_TCP:
	    		
		udpinit (tb);

   	    	/*
		 *  Now finish setting up the control block.
     		 */

    		tb -> tb_srcref = htons ((u_short) (getpid () & 0xffff));
    		tb -> tb_dstref = htons ((u_short) 0);

    		/*  
     		 *  Set the network address to be only the one we care about.
     		 */

    		tb -> tb_initiating = *newtuaddr (calling, la);	
    		tb -> tb_responding = *newtuaddr (called, na);	

    		if (qos)
		tb -> tb_qos = *qos;	/* struct copy */

		/*
		 *  Allocate the socket and BIND it to the address pair.
		 */

		if ((result = (*tb -> tb_UnitDataStart) (tb,
							 la,
							 na,
							 option),
							 td) != OK)
		    {
		    continue;
		    }
		break;

	    default:
		return tusaplose ( td,
				   DR_ADDRESS,
				   NULLCP, 
				   TuErrString(UDERR_NSAP_NOT_SUPPORTED) );
	    }

	break;

        }   /* end of for number of net addresses */


    if (tb -> tb_fd == NOTOK)
	{
	freetublk (tb);
	return NOTOK;
	}

   return (tb -> tb_fd);

}





/*  */

/*   
 ****************************************************************
 *								*
 *  TUnitDataUnbind	                                        *
 *  								*
 *  This routine releases datagram service for the transport.   *
 *  Itfrees the tsap block for the socket and then closes the   *
 *  soeckt.							*
 *								*
 *  returns:	OK, NOTOK					*
 *								*
 ****************************************************************
 */

int	TUnitDataUnbind (sd, td)


int	sd;
struct TSAPdisconnect *td;

{

register struct tsapblk *tb;
int	 result;
SBV      smask;			 	/* signal save mask */

#ifdef HULADEBUG
	printf ("\n     in TUnitDataUnbind \n");
#endif


/* 
 *  Find the correct transport block and set the signal mask.
 */ 

    tsap_udPsig (tb, sd);

/*
 *  Close the transport socket and free its resources.
 */

#ifdef HULADEBUG
	printf ("\n     freeing the tsap block \n");
#endif
	
    freetublk (tb);

    return OK; 

}



/*  */

/*   
 ****************************************************************
 *								*
 *  TuSave		                                        *
 *  								*
 *  This routine sets up a control block path and socket for    *
 *  datagram that has arrived on the server listen socket.  It  *
 *  saves the datagram in the control block to return it on the *
 *  first read.  This is required since we want to bind and     *
 *  write a server response on a new socket without tying up    *
 *  the common listen socket.                                   *
 *								*
 *  returns:	socket descriptor 				*
 *								*
 ****************************************************************
 */

int	TuSave (sd, vecp, vec, td)

int	 	 sd;
register int     vecp;
register char  **vec;
register struct  TSAPdisconnect *td;

{

    register struct tsapblk *tb;
    char     domain[NASIZE];
    register struct NSAPaddr *na;
    int      fd;
    int	     result;	     
    SBV      smask;			 	/* signal save mask */


    if (vecp < 3)
	return tusaplose (td, DR_PARAMETER, NULLCP, 
			  TuErrString(UDERR_BAD_INIT_VECTOR));

    missing_udP (vec);
    missing_udP (td);


    /*
     *  Check if we need to create a new socket or just
     *  reuse an already bound socket.
     */

    if (sd < 0)
        {
		
	/*
 	 *  Allocate a new tsap block and create a new socket.
 	 */
    	if ( (tb = newtublk () ) == NULL)
	    return tusaplose (td, 
			      DR_CONGEST, 
			      NULLCP, 
			      TuErrString(UDERR_NO_MEMORY) );

    	/*
     	 *  Now parse the vector for the args.
     	 *      vec[0] = ptr to buffer containing:
     	 *			network type
     	 *			socket fd
     	 *			remote host name (sock addr)
     	 *	    vec[1] = TPDU
     	 */

    	vec += vecp - 2;

    	switch (*vec[0]) 
	    {
	    case NT_TCP:
	    case NT_UDP:
#ifdef	UDP
    	    if (sscanf((vec[0]+1), "%d %s %s", &fd, na->na_domain, domain) != 3)
     	        return tusaplose (td, DR_PARAMETER, NULLCP,
		    	         TuErrString(UDERR_BAD_INIT_VECTOR));

    	    /*
     	     *  Create the temp socket.  We don't need addresses since
             *  the remote address will be specifeied when the service binds.
     	     */

	    if ((result = (*tb -> tb_UnitDataStart) (tb,
	  					     NULL,
						     NULL,
						     TUNITDATA_START),
						     td) != OK)

#endif
	    default:
	        (void) tusaplose (td, DR_ADDRESS, NULLCP,
	               "unknown network type: 0x%x (%c)", *vec[0], *vec[0]);
	        freetblk (tb);
		return NOTOK;
            }

	}
    else
	{
	/* 
 	 *  Find the correct tsap block and set the signal mask.
 	 */ 

        tsap_udPsig (tb, sd);

	}

    /*
     *  Now save the TPDU in the control block for the first read.
     */

    tb -> tb_holding_tpdu = 1;

    bcopy ( vec[1], &tb -> tb_hold_tpdu, strlen (vec[1]) ); 
        
   
    bzero (vec[0], strlen (vec[0]));

    bzero (vec[1], strlen (vec[1]));

    *vec = NULL;

    return (tb -> tb_fd);

}




/*  */
 
/*   
 ****************************************************************
 *								*
 *  TUnitDataRequest                                            *
 *  								*
 *  This routine implements the T-UNITDATA.REQUEST primitive    *
 *  for writing data to a datagram transport service.  This     *
 *  routine is used for unitdata service to remote addresses    *
 *  that havn't been binded.  This routine blocks until the     *
 *  data is written.						*
 *  								*
 ****************************************************************
 */

int     TUnitDataRequest (calling, called, qos, uv, td)

struct TSAPaddr *calling,
		*called;
struct QOStype  *qos;
register struct udvec	*uv;
struct TSAPdisconnect   *td;



{

int	sd;


#ifdef HULADEBUG
	printf ("\n     in TUnitDataRequest \n");
#endif


/*
 *  Create the socket on the fly.
 */

    sd = TUnitDataBind (calling, called, qos, td);

    if (sd == NOTOK)
	return NOTOK;

/*  
 *  Now do the datagram send.
 */


    if ( TUnitDataWrite (sd, uv, td) == NOTOK)
	return NOTOK;

/*
 *  Unbind the socket.
 */

    TUnitDataUnbind (sd, td);

    return OK;

}




 
/*  */

/*
 ****************************************************************
 *                                                              *
 *  TUnitDataWrite 						* 
 *      							*
 *  This routine implements the tsap unit write interface for   *
 *  writing user data over transport datagram service where     *
 *  the datagram service has already been established and the   *
 *  remote address has been binded on the socket.               *
 *								*
 *  This routine blocks until completion.			*
 *								*
 ****************************************************************
 */

int     TUnitDataWrite (sd, uv, td)

int 	 sd;
register struct udvec	*uv;
register struct TSAPdisconnect td;


{

int 	 n, cc, hlen;
SBV      smask;			 	/* signal save mask */
int      result; 		 	/* write result     */
register struct udvec	*vv;   		/* udvec            */
register struct tsapblk *tb;	 	/* transport blk ptr*/
char	 *data, *buffer, *hptr;	        /* data buffer ptr  */



#ifdef HULADEBUG
	printf ("\n     in TUnitDataWrite \n");
	printf ("\n     writing on socket %d \n", sd);
#endif

/*
 *  Check for missing parameters.
 */

    missing_udP (uv);
    missing_udP (uv -> uv_base);

    /*
     *  Check user data.
     */

    cc = 0;
    for (vv = uv; vv -> uv_base; vv++)
	{
#ifdef HULADEBUG
	printf ("\n     scanning the udvec - len = %d ", vv->uv_len);
#endif
	cc += vv -> uv_len;
	}

    if (n <= 0)
	return tusaplose ( td,
			   DR_PARAMETER,
			   NULLCP,
			   TuErrString(UDERR_ILLEGAL_UD_SIZE) );

    /* 
     *  Block any signals while we do the write.
     */

    smask = sigioblock ();

/* 
 *  Find the correct transport block and set the signal mask.
 */ 

    tsap_udPsig (tb, sd);

/*
 *  Add the ISO T-UNITDATA header.  For prototype purposes we
 *  do this as a subroutine call.  Ideally, it should be 
 *  implemented as a bona-fide transport service.
 */

   hlen = 0;

   hlen = T_UnitDataWrite (tb, uv, &hptr, td);
 
    
/*
 *  Now check if we have to allocate a contiguous buffer 
 *  for the buffer since datagrams must be a complete msg.
 *  If the len in the first udvec is not the total len, it
 *  means that the data spans multiple buffers.
 */

/*
 *  NOTE:  FOR ISO HEADER, WE WILL ALWAYS SPAN BUFFERS
 */

    if ( (cc != uv -> uv_len) || (hlen > 0) )
        {

#ifdef HULADEBUG
	printf ("\n     data spans multiple buffers - alloc buf %d \n", cc);
#endif

	/*
	 *  Allocate the buffer.
         */

	buffer = (char *) calloc (1, cc + hlen);

        if (buffer == NULL)
	    return tusaplose ( td, 
			       DR_CONGEST,
			       NULLCP,
			       TuErrString(UDERR_NO_MEMORY) );

	/*
 	 *  Save the buffer ptr.
	 */

	data = buffer;

	/*
         *  Copy the header first.
	 */

	if (hlen > 0)
	    {
	    bcopy (hptr, buffer, hlen);
	    buffer += hlen;
	    }
	
  	/*
         *  Now copy all the non-contiguous buffers into the 
	 *  contiguous buffer we just allocated.
         */

   	for (vv = uv; vv -> uv_base; vv++)
	    {
	    bcopy (vv -> uv_base, buffer, vv -> uv_len);
	    buffer += vv -> uv_len;
	    }

	}   /* end if non-contiguous user data */

    else
	{

	data = uv -> uv_base;

#ifdef HULADEBUG
	printf ("\n     data is contiguous - %d bytes \n", cc);
#endif
	}


/*
 *  Do the unit data write over the real transport service.
 *  The service entry point is in the transport block service
 *  area.  It returns the number of bytes written.
 */

#ifdef HULADEBUG
	printf ("\n     Tunitdata ... \n");
	for (n=0; n < min(cc,256); n++)
		printf (" %x", *(data+n) );
#endif
	


    if (*tb -> tb_UnitDataWrite)
    	result = (*tb -> tb_UnitDataWrite) (sd, data, cc, td);
    else
	result = 0;


    if (data == uv -> uv_base)
        free ((char *) data);


/*
 *  Restore the mask.
 */

    (void) sigiomask (smask);

/* 
 *  And return the result from the write.  To succeed, it 
 *  must have written the entire length cc since datagrams
 *  cannot be partially sent.
 */

    if (result == cc)
    	return OK;
    else
	return NOTOK;

}







/*  */ 

/*
 ****************************************************************
 *								*
 *  TUnitDataRead						*
 *  								*
 *  This routine reads datagram messages from the transport     *
 *  unit data service.  It blocks until data is read or secs    *
 *  expires.  If secs is 0, it blocks indefinitely.		*
 *								*
 *  returns:  # of bytes read					*
 *								*
 ****************************************************************
 */

int     TUnitDataRead (sd, tud, secs, td)

int	sd;
register struct TSAPunitdata *tud;
int	secs;
struct TSAPdisconnect	     *td;

{

    int	     cc, hlen;
    SBV      smask;			 	/* signal save mask */
    register struct tsapblk *tb;
    register struct qbuf    *qb;
    struct sockaddr_in 	    socket;	 
    register struct hostent *hp;
    register struct NSAPaddr *na;



#ifdef HULADEBUG
	int	j;
	printf ("\n     in TUnitDataRead \n");
#endif

/*
 *  Check for missing parameters.
 */

    missing_udP (tud);

/*
 *  Block any incomming signals while we do the read.
 */

    smask = sigioblock ();

/*
 *  Find the current transport block and set the signal mask.
 */

    tsap_udPsig (tb, sd);

/*
 *  Check if we are holding a tpdu that hasn't been read yet.
 *  This would happen with dynamic service invocation on the 
 *  tsap unitdata daemon side of things where it calls the 
 *  the save function on unitdata indications and then spawns
 *  the service.
 */

    if (tb -> tb_holding_tpdu)
	{
	/*
         *  Copy the held one into the real one.
	 */	

	bcopy ( &tb -> tb_hold_tpdu, tud, sizeof(tb -> tb_hold_tpdu) );

	tb -> tb_holding_tpdu = 0;

	return (tud -> tud_cc);
	}
	
/*
 *  Read the datagram socket for data.  It returns number of bytes read.
 */

    tud -> tud_qbuf.qb_forw = tud -> tud_qbuf.qb_back = &tud -> tud_qbuf;

    cc = (*tb -> tb_UnitDataRead) (sd, &tud -> tud_qbuf, secs, &socket, td);

    if (cc < 1)
	return 0;

#ifdef HULADEBUG
	printf ("\n     from tb_UnitDataRead %d bytes \n", cc);
	for (j=0; j<25; j++)
	    printf (" %x ", *((tud -> tud_qbuf.qb_data) + j) );
#endif


/*
 *  Format the tsap unit data structure.
 */

    tud -> tud_sd = sd;

    bzero ( &tud -> tud_calling, sizeof tud -> tud_calling);
    bzero ( &tud -> tud_called, sizeof tud -> tud_called);

    na = tud -> tud_calling.ta_addrs;
  
    na -> na_type = NA_TCP;

    hp = gethostbyaddr ((char *) &socket.sin_addr, 
				 sizeof socket.sin_addr,
	    			 socket.sin_family);
    (void) strcpy ( na-> na_domain, hp ? hp -> h_name : 
		    inet_ntoa (socket.sin_addr));

    na -> na_port = socket.sin_port;
    na -> na_tset = NA_TSET_UDP;

    tud -> tud_calling.ta_naddr = 1;   
    tud -> tud_calling.ta_selectlen = 0;


    tud -> tud_cc = cc;
    tud -> tud_base = tud -> tud_qbuf.qb_data;


   /*  
    *  Strip off the ISO T-UNITDATA header.  For prototype purposes
    *  this is done as a simple subroutine call.  Ideally this should
    *  be implemented as a bona-fide transport service.
    */

    hlen = T_UnitDataRead (tb, tud, td);
	
    return (cc - hlen);

}


#if FALSE

/*  */ 

/*
 ****************************************************************
 *								*
 *  Set Asynchronous Unit Data Event Indications		*
 *								*
 *  This routine sets the mask for handling unit data           *
 *  indications asynchromously.					*
 *								*
 ****************************************************************
 */

int	TSetUnitDataIndications (sd, data)

int	sd;
IFP	data;

{

    SBV	    smask;
    int     result;
    register struct tsapblk *tb;

/*
 *  Check for missing parm.
 */
   
    missingP (data);

/*
 *  Block any incomming signals while we do the read.
 */

    smask = sigioblock ();

/*
 *  Find the current transport block and set the signal mask.
 */

    tsapPsig (tb, sd);

/*
 *  Set the N+1 service routine to call with inbound data
 *  and set the flags for asynchronous service.
 */

    if (tb -> tb_UnitDataIndication == data)
	tb -> tb_flags |= TB_ASYN;
    else
	tb -> tb_flags &= ~TB_ASYN;

/*
 *  Now setup the signal event handler for inbound data.
 */

    result = TUnitDataWakeUp (tb);

    (void) sigiomask (smask);

    return result;
}


/*  */


/* 
 ****************************************************************
 *  map transport descriptors for select()
 ****************************************************************
 */

int	TSelectUnitDataMask (sd, mask, nfds)
int	sd;
fd_set *mask;
int    *nfds;

{

    SBV     smask;
    register struct tsapblk *tb;

/*
 *  Check for missing parameters.
 */

    missingP (mask);
    missingP (nfds);

/*
 *  Block any incomming signals while we set the mask.
 */
 
    smask = sigioblock ();

/*  
 *  Find the transport block for this socket descriptor.
 */

    if ((tb = findtublk (sd)) == NULL)
	{
	(void) sigiomask (smask);
	return tusaplose ( td,
			   DR_PARAMETER,
			   NULLCP,
			   TuErrString(UDERR_INVALID_XPORT) );
        }

/*
 *  Now set the select mask.
 */

    selmask (tb -> tb_fd, *mask, *nfds);


    (void) sigiomask (smask);

    return OK;

}


/*  */ 

/*
 ****************************************************************
 *								*
 *  N-UNITDATA.indication					* 
 *								*
 *  This routines gets the NSAP unit data indications.  This    *
 *  really gets the indications from the Transport Datagram     *
 *  service.  This is for asynchronous interface only.   	*
 *								*
 ****************************************************************
 */


static int  UNITDATAser (sig, code, sc)

int	sig;
long	code;
struct sigcontext *sc;

{

    int     n,
	    nfds,
	    sd;
    fd_set  ifds,
	    mask;
#ifndef	BSDSIGS
    SBV	    smask;
#endif
    IFP	    disc;
    register struct tsapblk *tb,
                           *tb2;
    struct TSAPdata txs;
    register struct TSAPdata   *tx = &txs;

#ifndef	BSDSIGS
    (void) signal (SIGEMT, UNITDATAser);

    smask = sigioblock ();
#endif

    for (;;)
	{
	n = 0;
	FD_ZERO (&ifds);
	for (tb = THead -> tb_forw; tb != THead; tb = tb -> tb_forw)
	    if (tb -> tb_fd != NOTOK && (tb -> tb_flags & TB_ASYN))
		{
		nfds = 0;
		FD_ZERO (&mask);
		selmask (tb -> tb_fd, mask, nfds);
		if ((*tb -> tb_UnitDataSelect) (nfds, &mask, NULLFD, NULLFD, 0)
			> OK)
		    {
		    FD_SET (tb -> tb_fd, &ifds);
		    n++;
		    }
	        }

	if (n == 0)
	    break;

	for (tb = THead -> tb_forw; tb != THead; tb = tb2)
	    {
	    tb2 = tb -> tb_forw;

	    if (FD_ISSET (sd = tb -> tb_fd, &ifds))
		{
		switch ((*tb -> tb_UnitDataRead) (tb, tx))
		    {
		    case NOTOK:
			return tusaplose (td, DR_OPERATION, NULLCP, "Unit Read failed");
			break;

		    case OK: 
			
/*
 *			Call the N+1 data handler.
 */
		        (*tb -> tb_UnitDataIndication) (sd, tx);

			break;

		    case DONE:	/* partially assembled TSDU */
		        break;
		    }
	        }
	   }
    }

/*
 *  Post a signal back to the ISORE helper to handshake and
 *  indicate completion of the data handler.
 */

#ifndef	SIGPOLL
    (void) kill (TPid, SIGEMT);
#endif

#ifndef	BSDSIGS
    (void) sigiomask (smask);
#endif

}



/*  */

#ifndef	SIGPOLL

static int  TUnitDataWakeUp (tb)

register struct tsapblk *tb;

{
    int     i,
            nfds;
    fd_set  mask;
    char    buf1[10],
            buf2[10],
            buf3[10];
    register struct isoservent *is;
    static int  inited = 0;

    if (TPid > OK)
	{
	(void) kill (TPid, SIGTERM);
	TPid = NOTOK;
        }

    nfds = 0;
    FD_ZERO (&mask);
    for (tb = THead -> tb_forw; tb != THead; tb = tb -> tb_forw)
	if (tb -> tb_fd != NOTOK && (tb -> tb_flags & TB_ASYN))
	    selmask (tb -> tb_fd, mask, nfds);

    if (nfds == 0)
	return OK;

    if (nfds > sizeof (int) * 8)
	return tsaplose (td, DR_CONGEST, NULLCP, "you lose");

    if (!inited)
	{
#ifndef	BSDSIGS
	int    smask = sigsetmask (sigblock (0) & ~sigmask (SIGEMT));
#endif

	(void) signal (SIGEMT, UNITDATAser);
#ifndef	BSDSIGS
	(void) sigiomask (smask);
#endif
	inited++;
        }

    if ((is = getisoserventbyname ("udisore", "tsap")) == NULL)
	return tusaplose (td, DR_CONGEST, NULLCP, "ISO service tsap/isore not found");

    (void) sprintf (buf1, "%d", nfds);
    *is -> is_tail++ = buf1;
    (void) sprintf (buf2, "0x%x", mask.fds_bits[0]);
    *is -> is_tail++ = buf2;
    (void) sprintf (buf3, "%d", getpid ());
    *is -> is_tail++ = buf3;
    *is -> is_tail = NULL;

    for (i = 0; i < 5; i++)
	switch (TPid = vfork ())
	    {
	    case NOTOK: 
		continue;

	    case OK: 
		(void) signal (SIGEMT, SIG_DFL);
		(void) execv (*is -> is_vec, is -> is_vec);
		_exit (1);

	    default:
		return OK;
	    }

    return tusaplose (td, DR_CONGEST, NULLCP, "unable to fork");

}
#else
#ifdef	BSDSIGS
#include <fcntl.h>
#include <sys/ioctl.h>
#else
#include <sys/stropts.h>
#endif


static int  TUnitDataWakeUp (tb)

register struct tsapblk *tb;

{
    int	    result;
    static int  inited = 0;

    if (tb -> tb_flags & TB_ASYN)
	{
	if (!inited) {
	    (void) signal (SIGPOLL, UNITDATAser);

	    inited++;
	}

#ifdef	BSDSIGS
	if (fcntl (tb -> tb_fd, F_SETOWN, getpid ()) == NOTOK)
	    return tusaplose (td, DR_CONGEST, "fcntl F_SETOWN");
	if ((result = fcntl (tb -> tb_fd, F_GETFL, 0x00)) == NOTOK)
	    return tusaplose (td, DR_CONGEST, "fcntl F_GETFL");
	result |= FASYNC;
	if (fcntl (tb -> tb_fd, F_SETFL, result) == NOTOK)
	    return tusaplose (td, DR_CONGEST, "fcntl F_SETFL");
#else

/*
	if (ioctl (tb -> tb_fd, I_GETSIG, &result) == NOTOK)
	    result = 0;
	result |= S_INPUT;
	if (ioctl (tb -> tb_fd, I_SETSIG, result) == NOTOK)
	    return tsaplose (td, DR_CONGEST, "failed", "ioctl I_SETSIG 0x%x",
			     result);
 */
	return tusaplose (td, DR_CONGEST,
		     "asynchronous operations not yet supported under SVR3");
#endif
    }
    else {
#ifdef	BSDSIGS
	if ((result = fcntl (tb -> tb_fd, F_GETFL, 0x00)) == NOTOK)
	    return tusaplose (td, DR_CONGEST, "fcntl F_GETFL");
	result &= ~FASYNC;
	if (fcntl (tb -> tb_fd, F_SETFL, result) == NOTOK)
	    return tusaplose (td, DR_CONGEST, "fcntl F_SETFL 0x%x");
#else
	if (ioctl (tb -> tb_fd, I_GETSIG, &result) == NOTOK)
	    return tusaplose (td, DR_CONGEST, "ioctl I_GETSIG");
	result &= ~S_INPUT;
	if (ioctl (tb -> tb_fd, I_SETSIG, result) == NOTOK)
	    return tusaplose (td, DR_CONGEST, "ioctl I_SETSIG ");
#endif
    }

    return OK;
}
#endif



/*  */

int newtuaddr (ta, na, tdest)

register struct TSAPaddr *ta;
register struct NSAPaddr *na;
register struct TSAPaddr *tdest;

{
    register struct NSAPaddr *ndest = tdest -> ta_addrs;

    bzero ((char *) tdest, sizeof(struct TSAPaddr));

    if (tdest -> ta_selectlen = ta -> ta_selectlen)
	bcopy (ta -> ta_selector, tdest -> ta_selector, ta -> ta_selectlen);
    if (na) {
	ndest->na_type = na->na_type;
	switch (na->na_type)
	    {
	    case NA_NSAP:
		bcopy (na->na_address, ndest->na_address, NASIZE);
		ndest->na_addrlen = na->na_addrlen;
		break;
	    case NA_TCP:
		bcopy (na->na_domain, ndest->na_domain, NASIZE);
		ndest->na_port = na->na_port;
		ndest->na_tset = na->na_tset;
		break;
	    default:
		break;
	    }
	tdest -> ta_naddr = 1;
    }

    return OK;
}

#endif /* if FALSE */


/*  */

static struct TSAPaddr *newtuaddr (ta, na)
register struct TSAPaddr *ta;
register struct NSAPaddr *na;
{
    static struct TSAPaddr tzs;
    register struct TSAPaddr *tz = &tzs;
    register struct NSAPaddr *nz = tz -> ta_addrs;

    bzero ((char *) tz, sizeof *tz);

    if (tz -> ta_selectlen = ta -> ta_selectlen)
	bcopy (ta -> ta_selector, tz -> ta_selector, ta -> ta_selectlen);
    if (na) {
	*nz = *na;
	tz -> ta_naddr = 1;
    }

    return tz;
}

#endif /* if HULA */
