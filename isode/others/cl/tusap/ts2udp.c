/*
 ****************************************************************
 *                                                              *
 *  HULA project						*
 *                                             			*
 *  program:  	ts2udp.c                                        *
 *  author:   	kurt dobbins                                    *
 *  date:     	1/89                                            *
 *                                                              *
 *  This program contains the routines to provide datagram      *
 *  transport service over UDP/IP.  They drive the EXCELAN 205T *
 *  card using DATAGRAM sockets.  Datagrams over udp provide    *
 *  message transfer with non-confirmed service and no          *
 *  reliabilty of delivery.                                     *
 *                                  				*
 *  entry points:                   				*
 *                                  				*
 *      udpinit (tb)	 					*
 *      udp_open (tb, local, remote, option)			*
 *	udp_start_client (sock, opt1, opt2)			*
 *	udp_join_server (sd, sock, opt1, opt2)			*
 *	udp_read_socket (fd, q)					*
 *	udp_write_socket (fd, data, len)			*
 *	udp_selecrt_socket()					*
 *	udp_close (fd)	 					*
 *								*
 ****************************************************************
 */


/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */



#include <errno.h>
#include <stdio.h>
#include "general.h"
#include "manifest.h"
#include "tpkt.h"
#include "tsap.h"
#include "uderrors.h"



/*  */

#ifdef	HULA
#ifdef  UDP

#include "internet.h"


extern int  errno;

/*  */

/* this structure is allocated for each socket device */

struct udpconn
    {
    int	    		udp_parent;
    struct sockaddr_in 	udp_peer;
    struct qbuf 	udp_queue;
    };


static int	maxpeers = 0;
static struct udpconn *peers = NULL;

 

/*  */

/* 
 **********************************************************
 *                                                        *
 *  udpinit                                               *
 *                                                        *
 *  This routine inializes the udp transport block.       *
 *                                                        *
 **********************************************************
 */

int	udpinit (tb)

register struct tsapblk	*tb;

{

#ifdef HULADEBUG
    printf ("\n          in udpinit \n");
#endif


    tb -> tb_flags |= (TB_CLNS | TB_UDP);

    tb -> tb_tsdusize = MAXUDP;

    /*
     *  Set the UNITDATA service entry points.
     */

    tb -> tb_UnitDataStart = udp_open;
    tb -> tb_UnitDataRead = udp_read_socket;
    tb -> tb_UnitDataWrite = udp_write_socket;
    tb -> tb_UnitDataIndication = NULLIFP;
    tb -> tb_UnitDataSelect = NULLIFP;
    tb -> tb_UnitDataClose = udp_close;


}



/*  */

/* 
 **********************************************************
 *                                                        *
 *  udp_open		                                  *
 *                                                        *
 *  This routine sets up the client or server socket for  *
 *  the remote address.  Client sockets are bound to      *
 *  remote address whereas the server sockets are not.    *
 *                                                        *
 *  returns:  OK, NOTOK		                          *
 *            also updates the tsapblk fd for the socket  *
 **********************************************************
 */

int 	udp_open (tb, local, remote, option, td)

register struct tsapblk *tb;
struct NSAPaddr *local,
		*remote;
int		option;
struct TSAPdisconnect *td;

{

    int     fd;
    struct sockaddr_in  lo_socket,
                        in_socket;
    register struct sockaddr_in *lsock = &lo_socket,
                                *isock = &in_socket;
    register struct hostent *hp;
    register struct servent *sp;

/*
 *  Check if we are creating a new socket or just rebinding
 *  a new remote addr on an existing socket.
 */

if (option == TUNITDATA_START)
    {

    /*
     *  Setup the local socket.  The host internet address must be zero.
     */

    bzero ((char *) lsock, sizeof *lsock);


    if (local && local -> na_domain[0])
	{

	if ((hp = gethostbystring (local -> na_domain)) == NULL)
	    tusaplose (td, DR_ADDRESS, NULLCP, TuErrString(UDERR_LHOST_UNKNOWN));

        if ( (remote && remote -> na_domain[0]) &&
	     (lsock -> sin_family = hp -> h_addrtype) != isock -> sin_family)
	    tusaplose (td, DR_ADDRESS, NULLCP, TuErrString(UDERR_ADDR_FAMILY_MISMATCH));

	bcopy (hp -> h_addr, (char *) &lsock -> sin_addr, hp -> h_length);

        }
#if FALSE
    else
	lsock = NULL;
#endif

    /*  
     *  Create the local socket.  Use the specified port 
     *  if it is there.
     */

    if (local && local -> na_port != 0)
	{
	lsock -> sin_port = local -> na_port;
	lsock -> sin_family = AF_INET;
	}
    else
	lsock -> sin_family = AF_INET;

    errno = 0;

    if ((fd = udp_start_client (lsock, 0, 0)) == NOTOK)
	tusaplose (td, DR_OPERATION, NULLCP,TuErrString(UDERR_START_CLIENT_FAILED));
	    
#ifdef HULADEBUG
    printf ("\n          socket = %d \n", fd);
#endif

    /* 
     *  Update the tsap block with the socket descriptor.
     */

    tb -> tb_fd = fd;

    } /* end TUNITDATA_START */


/*
 *  If the remote host was specified, BIND the address pair.
 *  Otherwise, we will have to use 'sendto' semantics on
 *  on each datagram send later on.
 */

    if (remote && remote -> na_domain[0])
	{

	/* 
	 *  Setup the remote socket address.
	 */

	bzero ((char *) isock, sizeof *isock);

    	/*
     	 *  If the remote port is not specified, try to
     	 *  default to the port for the tsap daemon.
     	 */ 

    	if (remote -> na_port == 0)
	    {
	    if ((sp = getservbyname ("tsap", "tcp")) == NULL)
	        tusaplose (td, DR_ADDRESS, NULLCP, TuErrString(UDERR_TSAP_SERVICE_UNKNOWN));

	    isock -> sin_port = sp -> s_port;

#ifdef HULADEBUG
        printf ("\n          defaulting REMOTE port to tsap service\n");
#endif

            }
        else
	    isock -> sin_port = remote -> na_port;

        /* 
         *  Lookup the remote host.
         */

        if ((hp = gethostbystring (remote -> na_domain)) == NULL)
	    tusaplose (td, DR_ADDRESS, NULLCP, TuErrString(UDERR_RHOST_UNKNOWN));

        (void) strncpy (remote -> na_domain, 
		        hp -> h_name, 
		        sizeof remote -> na_domain);

        isock -> sin_family = hp -> h_addrtype;

        bcopy (hp -> h_addr, (char *) &isock -> sin_addr, hp -> h_length);

	/*
         *  Now do the actual bind of the remote addr to the socket.
	 */

        if ((udp_join_server (tb -> tb_fd, isock, 0, 0)) == NOTOK)
	    tusaplose (td, DR_OPERATION, NULLCP, TuErrString(UDERR_JOIN_SRVR_FAILED));

	}

    return OK;

}




/*  */

/* 
 **********************************************************
 *                                                        *
 *  udp start client                                      *
 *                                                        *
 *  This routine creates the initial DATAGRAM socket for  *
 *  server code and allocates the udp conn structs to     *
 *  track logical connections over the socket devices.    *
 *                                                        *
 *  returns:  socket descriptor                           *
 *                                                        *
 **********************************************************
 */

#ifdef EXOS

int	udp_start_client (sock, opt1, opt2)

struct 	sockaddr_in *sock;
int	opt1,
	opt2;

{
    register int    port;
    int      sd;
    int	     i;
    register struct hostent *hp;
    register struct udpconn *up;
			    



#ifdef HULADEBUG
    printf ("\n          in start udp client \n");
#endif

/*
 *  Initialize the queue of udp conn structs.  One exists for
 *  each socket device.  It will be stored as an array indexed
 *  by socket descriptor.
 */

    if (peers == NULL)
        {
        maxpeers = getdtablesize ();

#ifdef HULADEBUG
    	printf ("\n          allocating the peers array \n");
	printf ("\n          maxpeers = %d \n ", maxpeers);
#endif

	peers = (struct udpconn *) calloc ( (unsigned) maxpeers,
					    sizeof(struct udpconn) );

	if (peers == NULL)
	    return NOTOK;


#ifdef HULADEBUG
    	printf ("\n          initializing the peers array \n");
#endif

	for (i=0; i < maxpeers; i++)
            {
	    peers[i].udp_parent = NOTOK;
	    peers[i].udp_queue.qb_forw = peers[i].udp_queue.qb_back =
		&peers[i].udp_queue;
	    }

        }


/*
 *  Create the local DATAGRAM socket.  Implies UDP/IP protocol
 *  stack.  Remember, the socket call justs creates an endpoint 
 *  for communication for the specified port.
 */

#ifdef HULADEBUG
    printf ("\n          formatting the local DATAGRAM socket \n");
#endif

    if (sock -> sin_addr.s_addr == 0) 
	{
#ifdef HULADEBUG
    printf ("\n          get local host name  \n");
#endif
	if ((hp = gethostbyname ("localhost")) == NULL)
	    {
	    errno = EADDRNOTAVAIL;
	    return NOTOK;
	    }
	
	sock -> sin_family = AF_INET;

#if FALSE
	sock -> sin_family = hp -> h_addrtype;

	bcopy (hp -> h_addr, (char *) &sock -> sin_addr, hp -> h_length);
#endif
	
	}


    if (sock -> sin_port != 0)
	{
	/*  
	 *  Create the local socket for a spcific port.
	 */

#ifdef HULADEBUG
    	    printf ("\n          creating the local DATAGRAM socket \n");
	    printf ("\n          family = %d", sock -> sin_family);
	    printf ("\n          port = %d", ntohs(sock -> sin_port));
	    printf ("\n          addr = %d %d %d %d", sock -> sin_addr.s_net,
	    			                      sock -> sin_addr.s_host,
	  				              sock -> sin_addr.s_lh,
					              sock -> sin_addr.s_impno);
#endif

	if ((sd = socket (SOCK_DGRAM,
			  (struct sockproto *) 0,
			  (struct sockaddr *) sock,
			  SO_LARGE)) != NOTOK)
	/*
 	 *  Save the socket descriptor in the peers array and
 	 *  return the socket descriptor for the local socket.
 	 */

    	   return (peers[sd].udp_parent = sd);
	}
    else
	{

	/* 
 	 *  Generate a unique source port number.
 	 */

#ifdef HULADEBUG
        printf ("\n          generate unique port # \n");
#endif

    
        for (port = IPPORT_RESERVED ;; port++)
 	    {

	    sock -> sin_port = htons ((u_short) port);

#ifdef HULADEBUG
    	    printf ("\n          creating the local DATAGRAM socket \n");
	    printf ("\n          family = %d", sock -> sin_family);
	    printf ("\n          port = %d", ntohs(sock -> sin_port));
	    printf ("\n          addr = %d %d %d %d", sock -> sin_addr.s_net,
	    			                      sock -> sin_addr.s_host,
	  				              sock -> sin_addr.s_lh,
					              sock -> sin_addr.s_impno);
#endif


	    if ((sd = socket (SOCK_DGRAM,
			      (struct sockproto *) 0,
			      (struct sockaddr *) sock,
			      SO_LARGE)) != NOTOK)
	    /*
 	     *  Save the socket descriptor in the peers array and
 	     *  return the socket descriptor for the local socket.
 	     */

    	        return (peers[sd].udp_parent = sd);

	    switch (errno)
	        {
	        case EADDRINUSE: 
   	            continue;

	        case EADDRNOTAVAIL: 
                case EAFNOSUPPORT: 
	        case ENOBUFS: 
	        case EPROTONOSUPPORT: 
	        default: 
#ifdef EXOS
 	            return NOTOK;

		}  /* end switch */
	
#else	        
		    return NOTOK;

	        } /* end switch */
#endif

	    }   /* end for port number */

	}   /* end if port specified */	

}




/*  */

/* 
 **********************************************************
 *                                                        *
 *  udp join server	                                  *
 *                                                        *
 *  This routine does the actual binding of remote socket.*
 *                                                        *
 *  returns:  OK, NOTOK		                          *
 *                                                        *
 **********************************************************
 */

int	udp_join_server (sd, sock, opt1, opt2)

int	sd;
struct 	sockaddr_in *sock;
int	opt1,
	opt2;

{


/*
 *  Bind the socket to the socket name.  The socket name is
 *  made up of a 32-bit internet address and a 16-bit port number.
 *  This is done for the specified port sock -> sin_port.
 */

#ifdef HULADEBUG
    printf ("\n          binding the socket to remote address %d %d %d %d ",
		         sock -> sin_addr.s_net, 
		         sock -> sin_addr.s_host,
		         sock -> sin_addr.s_lh,
		         sock -> sin_addr.s_impno);		
    printf (" and port %d \n", ntohs (sock -> sin_port) );			
#endif

    errno = 0;

    if (sock -> sin_port != 0)
        {
	if (connect (sd, (struct sockaddr *) sock) != NOTOK)
	    return OK;

        switch (errno)
	    { 
	    case EISCONN:
		/*  
		 *  Trying to rebind the existing socket is ok
		 *  since it may be a server rebinding to another
		 *  request from the previously-bound remote socket.
		 */
   	        return OK;
	    default:
		(void) udp_close (sd);
		return NOTOK;
	    }
        }

    return NOTOK;


}

#endif /* if EXOS */


#if FALSE

/*  */
 
/*
 **********************************************************
 *                                                        *
 *  join udp aux                                          *
 *                                                        *
 **********************************************************
 */

int 	join_udp_aux (fd, sock, newfd)

int	fd,
	newfd;
struct sockaddr_in *sock;

{
    int	    nfds,
	    sd;
    fd_set  ifds;
    register struct qbuf *qb;
    register struct udpconn *up;

    if (fd < 0 || fd >= maxpeers || peers[fd].udp_parent != fd) {
	errno = EINVAL;
	return NOTOK;
    }

    if (newfd) {
	FD_ZERO (&ifds);

	nfds = fd + 1;
	FD_SET (fd, &ifds);
	if (udp_select_socket (nfds, &ifds, NULLFD, NULLFD, OK) == NOTOK)
	    return NOTOK;

	up = &peers[fd];
	if ((qb = up -> udp_queue.qb_forw) == &up -> udp_queue) {
	    errno = EWOULDBLOCK;
	    return NOTOK;
	}

	if ((sd = dup (fd)) == NOTOK)
	    return NOTOK;

	up = &peers[sd];
	*sock = *((struct sockaddr_in *) qb -> qb_base);    /* struct copy */
	remque (qb);
	insque (qb, up -> udp_queue.qb_back);
    }
    else
	up = &peers[sd = fd];

    up -> udp_parent = fd;
    bcopy ((char *) sock, (char *) &up -> udp_peer, sizeof up -> udp_peer);

    return (newfd ? sd : OK);
}

#endif  /* if FALSE */



/*  */

/* 
 **********************************************************
 *                                                        *
 *  udp read socket                                       *
 *                                                        *
 *  This routine reads datagram messages on the DATAGRAM  *
 *  socket running over UDP/IP.  The data is read into a  *
 *  buffer and queued on the qbuf struct passed on input. *
 *                                                        *
 *  returns:  # of bytes read                             *
 *                                                        *
 **********************************************************
 */
 
int	udp_read_socket (fd, q, secs, fromsock, td)

int	    	fd;
struct qbuf 	*q;
int	    	secs;
struct sockaddr_in 	*fromsock;
struct TSAPdisconnect   *td;


{
    int		cc;
    int	    	nfds;
    fd_set  	ifds,
	     	mask;
    struct qbuf qbuff;
    register struct qbuf 	*qb = &qbuff;
    register struct udpconn 	*up;
    struct sockaddr_in 		*sock;
    char 			*rbufptr;
    int 			i;


#ifdef HULADEBUG
    		printf ("\n          in udp read socket \n");
#endif


/*
 *  Check for valid socket descriptor and set the udpconn struct ptr.
 */

    if (fd < 0 ||
        fd >= maxpeers ||
        (up = &peers[fd]) -> udp_parent == NOTOK)
            {
	    errno = EINVAL;
	    return tusaplose (td, DR_PARAMETER, NULLCP, TuErrString(UDERR_INVALID_XPORT_DESC));	    
            }

/*
 *  Set up the file descriptor mask for the select.
 *  Select will tell if the socket is readable if we set the 
 *  read mask.
 */

/* 
 *  Make sure the queue is empty before reading more data.
 */

    if ((qb = up -> udp_queue.qb_forw) == &up -> udp_queue)
        {
	FD_ZERO (&ifds);

/*  
 *  	Set the number of file descriptors we want to check.  Select
 *  	examines the I/O descriptors in the bits 0 thru (nfds - 1)
 *  	so add 1 to get the file descriptor we want.
 */

	nfds = fd + 1;
	FD_SET (fd, &mask);

/*
 *      Select the socket to determine if there is data that
 *	can be read.  The call to udp_select_socket not only
 *      selects the socket but also dequeues any data onto the 
 *      read queue contained in the udpconn structure for the 
 *      active socket.
 */
	for (ifds = mask ; ; ifds = mask)
            {
	    if (udp_select_socket (nfds,	/* # of fd's     */
				   &ifds,	/* & read mask   */
				   NULLFD,      /* & write mask  */
				   NULLFD,      /* & excptn mask */
				   secs)	/* seconds wait   */
 		    == NOTOK)
		return tusaplose (td, DR_OPERATION, NULLCP, NULLCP);
            
	    /*
 	     *  Check if we dequeued any data.
             */

	    if ((qb = up -> udp_queue.qb_forw) != &up -> udp_queue)
		break;

	    return tusaplose (td, DR_TIMER, NULLCP, NULLCP);	    
	    }
        }

/* 
 *  Remove the queue buffer from its queue and return it.
 */

#ifdef HULADEBUG
    		printf ("\n          Removing the queued data from select\n");
#endif

    remque (qb);

    /*
     *  Format the qbuf with the real data info.
     */

    q -> qb_len = qb -> qb_len;
    q -> qb_data = qb -> qb_data;
    q -> qb_base[0] = qb -> qb_base[0];
	

#ifdef HULADEBUG
	printf ("\n qb_data = \n");
	for (cc=0; cc<25; cc++)
	    printf (" %x ", *((qb -> qb_data) + cc) );
	printf ("\n q_data = \n");
	for (cc=0; cc<25; cc++)
	    printf (" %x ", *((q -> qb_data) + cc) );
#endif

   /*  
    *  Copy the fromsocket address saved in the udp peers array.
    */

    bcopy ((char *) &up -> udp_peer, (char *) fromsock, sizeof up -> udp_peer);

    return qb -> qb_len;

}



/*  */

/* 
 **********************************************************
 *                                                        *
 *  udp write socket                                      *
 *                                                        *
 *  This routine writes datagram messages on the DATAGRAM *
 *  socket running over UDP/IP.  The data is written from *
 *  the qbuf buffer using the EXCELAN socket library call *
 *  'send'.                                               *
 *                                                        *
 *  returns:  # of bytes written                          *
 *                                                        *
 **********************************************************
 */

int	udp_write_socket (fd, data, cc, td)

int	fd;
register char *data;
int	cc;
struct TSAPdisconnect *td;

{
    register struct udpconn *up;
int	len;

/*  
 *  Check for valid socket descriptor and set the 
 *  udpconn struct ptr for the socket.
 */

    if (fd < 0 ||
        fd >= maxpeers ||
	(up = &peers[fd]) -> udp_parent == NOTOK
#if FALSE
 ||
	up -> udp_peer.sin_family == 0)
#endif  
    )
	    {
	    errno = EINVAL;
	    return NOTOK;
            }

/*
 *  Issue the write to the socket.  It returns the number
 *  of bytes written.
 */

#ifdef HULADEBUG
    printf ("\n          send datagram: ");
    printf (" socket = %d  len = %d  \n", fd, cc);
#endif    

    errno = 0;

    return send (fd,			/* sd       */
		 NULL,		        /* to addr  */
		 data,			/* buffer   */
		 cc);			/* buf size */
		

}



/*  */

int	udp_close (fd)
int	fd;
{
    register struct qbuf *qb,
			 *qp;
    register struct udpconn *up,
			    *vp;

#ifdef HULADEBUG
    printf ("\n          close socket: %d", fd);
#endif    


#if FALSE

    if (fd < 0 || fd >= maxpeers || (up = &peers[fd]) -> udp_parent == NOTOK) {
	errno = EINVAL;
	return NOTOK;
    }

    up -> udp_parent = NOTOK;
    bzero ((char *) &up -> udp_peer, sizeof up -> udp_peer);
    for (qb = up -> udp_queue.qb_forw; qb != &up -> udp_queue; qb = qp) {
	qp = qb -> qb_forw;
	remque (qb);

	free ((char *) qb);
    }
    
    for (vp = (up = peers) + maxpeers; up < vp; up++)
	if (up -> udp_parent == fd)
	    up -> udp_parent = up - peers;
#endif

    return close (fd);
}




/*  */

/* 
 **********************************************************
 *                                                        *
 *  udp select socket                                     *
 *                                                        *
 *  This routine handles doing the select to enable       *
 *  reading data on the socket.                           *
 *                                                        *
 *  If data is readable on the socket, this routine will  *
 *  also do the actual read on the socket and queue the   *
 *  data on the udpconn struct read queue.                *
 * 							  *
 *  synchronous multiplexing:				  *
 *							  *
 *      secs < 0:  block indefinitely			  *
 *      secs = 0:  poll					  *
 *      secs > 0:  wait for secs (in milliseconds)        *
 *                                                        *
 *  returns:  OK, NOTOK                                   *
 *                                                        *
 **********************************************************
 */

int	udp_select_socket (nfds, rfds, wfds, efds, secs)

int	nfds;
fd_set *rfds,
       *wfds,
       *efds;
int	secs;
{
    register int    fd;
    int	    cc,
	    len,
	    mfds,
	    result;
    fd_set  ifds,
	    jfds;
    register struct qbuf *qb;
    register struct udpconn *up,
			    *vp;
    struct udpconn *wp;
    struct sockaddr_in *sock;
    char   *recvptr;


#ifdef HULADEBUG
    		printf ("\n          in udp select socket \n");
#endif


/*  
 *  Check if the read mask is set.  Then check if any read masks
 *  are set on each socket descriptor in the udpconn array.
 */


    if (rfds) 
        {
	jfds = *rfds;

	if (secs != OK)

#ifdef HULADEBUG
            printf ("\n          checking the read masks of all peers \n");
#endif

	    for (vp = (up = peers) + maxpeers, fd = 0; up < vp; up++, fd++)
		if (up -> udp_parent != NOTOK &&
		    FD_ISSET (fd, &jfds) &&
		    up -> udp_queue.qb_forw != &up -> udp_queue) 
   	                {
			/*
			 *  We found data pending on this socket.
			 */
		        secs = OK;
		        break;
		        }

        }  /* end if rfds */


/*
 *  Now do the real select to enable reading the socket.
 *  The 'selsocket' routine is a front end routine that
 *  implements the semantics of the Berkley select.
 */


#ifdef HULADEBUG
    		printf ("\n          doing the select on the read mask \n");
#endif

    if ((result = selsocket (nfds, rfds, wfds, efds, secs)) == NOTOK
	    || rfds == NULLFD)
	return result;

#ifdef HULADEBUG
    		printf ("\n          return from select # = %d\n", result);
#endif

    ifds = *rfds;

/*
 *  Read the datagrams on each socket descriptor in the udpconn
 *  array that has its read enable bit set.  Each datagram message
 *  is put on the respective read queue for the socket.
 */
 
    if ((mfds = nfds) > maxpeers)
	mfds = maxpeers;

/*
 *  Cycle thru each socket in the peers array.
 */

    for (fd = 0, up = peers; fd < mfds; fd++, up++)
	if (FD_ISSET (fd, &ifds))
	    {

	    if (up -> udp_parent == NOTOK)
		continue;

/*
 *          Allocate a qbuf structure and socket structure and 
 *          data buffer for the max datagram size (8K over EXCELAN).
 *          These are all allocated contiguously.
 */

	    if ( (qb = (struct qbuf *) malloc (sizeof *qb + sizeof *sock) )
		   == NULL)
		return NOTOK;

	    if ( (recvptr = (char *) malloc (8*1024) ) == NULL)
		{
		free ((char *) qb);
		return NOTOK;
		}

/*
 *          Setup the socket and qbuf structures.
 */

	    sock = (struct sockaddr_in *) qb -> qb_base;
	    len = sizeof *sock;
	    qb -> qb_data = recvptr;

/*
 * 	    Do the DATAGRAM read on the socket to dequeue the
 *          data into the qbuf data buffer.
 */

#ifdef HULADEBUG
    		printf ("\n          datagram receive on sock %d \n", fd);
#endif

	    errno = 0;

	    if (( cc = receive  (
				fd,			  /* sd        */
				sock, 			  /* from addr */
			 	qb -> qb_data,	  	  /* buffer    */
				(8*1024)   	  	  /* buf size  */
  				)) == NOTOK)
		{
		free ((char *) qb);
 		return NOTOK;
    	        }

/*
 *          Update the actual count of the bytes read.
 */

#ifdef HULADEBUG
    	    printf ("\n          received datagram - %d bytes long \n", cc);
	printf ("\n data = \n");
	for (len=0; len<25; len++)
	    printf (" %x ", *((qb -> qb_data) + len) );
#endif
	    qb -> qb_len = cc;

/*      
 *          Save the remote socket address in the udpconn struct
 *	    and put the data on its queue.
 */   
	    bcopy ( (char *) sock, (char *) &up -> udp_peer, sizeof *sock);

#ifdef HULADEBUG
    		printf ("\n          queueing data on the active socket\n");
#endif
   	    insque (qb, up -> udp_queue.qb_back);

	    continue;


/*
 *	    If not for the current udpconn struct, then search the 
 *          peers array for the matching socket address and queue
 *          the data on it.
 */

	    for (wp = (vp = peers) + maxpeers; vp < wp; vp++)
		if (vp != up
			&& vp -> udp_parent == up -> udp_parent
		        && bcmp ((char *) &vp -> udp_peer, (char *) sock,
				 sizeof *sock) == 0)
		    break;
	    if (vp >= wp &&
	       (vp = &peers[up -> udp_parent]) -> udp_peer.sin_family != 0)
	           {
		   free ((char *) qb);
		   continue;
	    	   }
#ifdef HULADEBUG
  	    printf ("\n          queuing data on the found socket \n");
#endif

	    insque (qb, vp -> udp_queue.qb_back);


	} /* end if FDSET */

/* 
 *  Now reset all the file descriptors since they get modified.
 */

    for (vp = (up = peers) + maxpeers, fd = 0; up < vp; up++, fd++)
	if (up -> udp_parent != NOTOK && FD_ISSET (fd, &jfds))
	    if (up -> udp_queue.qb_forw != &up -> udp_queue)
		FD_SET (fd, rfds);
	    else
		FD_CLR (fd, rfds);

    result = 0;
    ifds = *rfds;

    if (wfds)
	for (fd = 0; fd < nfds; fd++)
	    if (FD_ISSET (fd, wfds))
		FD_SET (fd, &ifds);

    if (efds)
	for (fd = 0; fd < nfds; fd++)
	    if (FD_ISSET (fd, efds))
		FD_SET (fd, &ifds);

    for (fd = 0; fd < nfds; fd++)
	if (FD_ISSET (fd, &ifds))
	    result++;
/*
 *  Return the count of sockets that were read.
 */

    return result;

}

#endif 	/* if HULA */
#endif  /* if UDP  */
