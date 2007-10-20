/* udptest.c - test out -ltsap for unit data service over UDP */

#ifndef	lint
static char *rcsid = "$Header: /f/iso/tsap/RCS/tsaptest.c,v 5.0 88/07/21 15:00:04 mrose Rel $";
#endif


/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */


#include <stdio.h>
#include <ctype.h>
#include "tpkt.h"
#include "tsap.h"
#include "tusap.h"
#include "isoservent.h"
#include "isoaddrs.h"
#include "internet.h"

/*  */

char *gets (), *calloc ();

printsrv();
printtaddr();


#define CLIENT_MODE  	0
#define SERVER_MODE  	1


 
/*  */

/* ARGSUSED */

int main(argc, argv, envp)
int argc;
char **argv,
     **envp;
{
    struct tsapkt *t;                           /* Working packet pointer */
    char sendbuf[1024];			        /* send buffer */
    char recvbuf[1024];			        /* recv buffer */
    char buffer[80],                            /* Working input buffer */
         *bptr;                                 /* Pointer to our buffer */
    struct udvec uvs[2];
    register struct udvec *uv;
    int  cc, result, data, mode;
    u_short  pattern;
    register struct tsapblk *tb;
    struct TSAPunitdata tunitdata;
    struct TSAPunitdata *tud = &tunitdata;
    int	sd;
    struct TSAPaddr   *ta;
    struct isoservent *is;
    register int    n = ta -> ta_naddr - 1;
    register struct NSAPaddr   *na = ta -> ta_addrs;
    struct TSAPdisconnect	td;


#define	NTADDRS		FD_SETSIZE

static struct TSAPaddr *tz;
static struct TSAPaddr  tas[NTADDRS];
    int	    port;
    struct NSAPaddr *tcp_na;
    struct servent *sp;



    printf ("\n UDP test driver for TUNITDATA\n");
    printf ("\n running on host \"%s\"\n", TLocalHostName ());

    t = NULL;
    bptr = buffer;
    *bptr = 'h';

/*
 *  Init the send buffer to a continous pattern.
 */

 for (cc = pattern = 0; cc < sizeof(sendbuf); cc++, pattern++)
    sendbuf[cc] = pattern;

/*
 *  Setup the calling and called network addresses.
 */

    if ((is = getisoserventbyname("udptest", "tsap")) == NULL)
	{
	printf ("\nfailed to lookup iso service \n");
	exit (1);
	}
	
    if (is)
        printsrv (is);
    
    if ((ta = is2taddr("loopback", "udptest", is)) == NULLTA)
	{
        printf ("\n tsap address translation failed \n");
	exit(1);
	}
    if (ta)
        printtaddr(ta);
    

    while (1) {
        switch (toupper(*bptr)) {
            case 'H':
                printf("\n\nCommands are:\n");
		printf("  T - Toggle mode (default is client)\n");
		printf("  B - Bind service\n");
		printf("  U - Unbind service\n");
		printf("  L - Looptest - incremental send \n");
                printf("  S - Send a datagram\n");
		printf("  R - Receive a datagram\n");
                break;
            case 'Q':
                printf("\nUDP tester terminated by user\n");
                exit(0);
	    case 'T':
	       
		printf ("\n Toggle mode \n");

		printf ("\n Current mode is ");	
		if (mode == SERVER_MODE)
		    {
		    printf ("SERVER\n");
		    mode = CLIENT_MODE;
		    printf ("\n Changing to CLIENT\n");
		    }
		else
		    {
		    printf ("CLIENT\n");
		    mode = SERVER_MODE;
		    printf ("\n Changing to SERVER\n");
		    }
		break;

	    case 'B':
		printf ("\n Binding the service\n");
#ifdef UDP
		if (mode == CLIENT_MODE)
       		    sd = TUnitDataBind (NOTOK, NULL, ta, NULL, &td);
		else
		    {
	    	    if ((sp = getservbyname ("tsap", "tcp")) == NULL)
	                printf ("\ntcp/tsap: unknown service");
		
		    tz = tas;
    	    	    tcp_na = tz -> ta_addrs;
    	    	    tcp_na -> na_type = NA_TCP;
    	    	    tcp_na -> na_domain[0] = NULL;
    	    	    tcp_na -> na_port = sp -> s_port;
	            tz -> ta_naddr = 1;
		    
       		    sd = TUnitDataListen (tcp_na, NULL);
		    }

	    	if (sd == NOTOK)
		    printf ("\n Bind unit data service failed\n");
    		else
		    printf ("\n application bound to socket = %d \n", sd);

#endif
		break;

	    case 'U':
		printf ("\n Unbinding the service\n");
		    
       		TUnitDataUnbind (sd);

		break;

	    case 'S':
	       
		printf ("\n Send a datagram \n");
		printf ("\n sending %d bytes of user data\n", cc);
		printf ("\n here are the first 25 bytes \n");

			
		for (data = 0; data < 25; data++)
		    printf (" %x ", sendbuf[data]);
		
		uv = &uvs[0];
    		uv -> uv_base = &sendbuf[0];
		uv -> uv_len = 1024;
		uv++;
		uv -> uv_len = 0;
		uv -> uv_base = NULL;

		result = TUnitDataWrite (sd, uvs); 

		if (result != OK)
		    printf ("\n datagram write failed\n");
		else   
		    printf ("\n datagram write successful\n");
		break;

            case 'R':
		printf ("\n Read datagram on socket %d \n", sd);
		
		cc = TUnitDataRead (sd, tud, 60);

		if (cc != NOTOK)
		    printf ("\n received %d bytes \n", cc);
				
		break;

            case '\0':
                break;
            default:
                if (*bptr != '\0')
                    printf("** Unrecognized command\n");
                break;
        }
        printf("\nCommand: ");
        (void)fflush(stdout);
        bptr = gets(buffer);
	if (feof (stdin))
	    exit (1);
        while (isspace(*bptr) && (*bptr != '\0')) ++bptr;
    }
}



static	printsrv (is)
register struct isoservent *is;
{
    register int    n = is -> is_tail - is -> is_vec - 1;
    register char **ap = is -> is_vec;

    printf ("ENT: \"%s\" PRV: \"%s\" SEL: %s\n",
	    is -> is_entity, is -> is_provider,
	    sel2str (is -> is_selector, is -> is_selectlen, 1));

    for (; n >= 0; ap++, n--)
	printf ("\t%d: \"%s\"\n", ap - is -> is_vec, *ap);
    printf ("\n");
}


/*  */

static	printtaddr (ta)

register struct TSAPaddr *ta;

{

register int    n = ta -> ta_naddr - 1;
register struct NSAPaddr   *na = ta -> ta_addrs;



	printf ("ADDR:    TSEL: %s\n",
		sel2str (ta -> ta_selector, ta -> ta_selectlen, 1));

	for (; n >= 0; na++, n--)
	    {
	    printf ("\t%d: ", ta -> ta_naddr - n - 1);

	    switch (na -> na_type)
	        {
		case NA_NSAP: 
		    printf ("NS %s", na2str (na));
		    break;

		case NA_TCP: 
		    printf ("TCP %s", na2str (na));
		    if (na -> na_port)
			printf ("  Port %d Tset 0x%x",
				(int) ntohs (na -> na_port),
				(int) na -> na_tset);
		    break;

                case NA_X25:
		case NA_BRG:
                    printf ("X.25 %s%s",
			    na -> na_type == NA_BRG ? "(BRIDGE) " : "",
			    na2str (na));
                    if (na -> na_pidlen > 0)
                        printf (" %s" , sel2str (na -> na_pid,
						 (int) na -> na_pidlen, 1));
		    break;

		default: 
		    printf ("0x%x", na -> na_type);
		    break;
	        }
	}

}

