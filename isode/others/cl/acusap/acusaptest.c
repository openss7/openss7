/*
 ****************************************************************
 *                                                              *
 *  HULA project - connectionless ISODE                         *
 *                                             			*
 *  module:  	acusaptest.c                                    *
 *  author:   	Bill Haggerty, Kurt Dobbins                     *
 *  date:     	4/89                                            *
 *                                                              *
 *  This code implements a test driver for A-UNIT-DATA service. *
 *                                  				*
 ****************************************************************
 *								*
 *			     NOTICE		   		*
 *								*
 *    Use of this module is subject to the restrictions of the  *
 *    ISODE license agreement.					*
 *								*    
 ****************************************************************
 */


#include <stdio.h>
#include <ctype.h>
#define  ACSE
#include "acupkt.h"
#include "isoservent.h"
#include "isoaddrs.h"
#include "internet.h"
#include "ADD-types.h"
#include "varargs.h"

#include "ssap.h"
#include "susap.h"


static char *myhost = "loopback";
static char *myservice = "acusaptest";
static char *mycontext = "acusaptest context";
static char *mypci = "acusaptest pci";

/*  */

char *gets (), *calloc ();

static	int	mode;
#define CLIENT_MODE  	0
#define SERVER_MODE  	1

static    struct QOStype qos;
static    struct PSAPaddr *pa;
static    struct PSAPaddr *cpa;
static    struct AEInfo clientaei;
static    AEI	    aei;
static    AEI	    pclientaei;
static    OID	    ctx;
static    OID	    pci;
static    struct PSAPctxlist pcs;
static    struct PSAPctxlist *pc = &pcs;

static    struct AcSAPindication  acis;
static    struct AcSAPindication *aci = &acis;
static    struct AcSAPabort *aca = &acis.aci_abort;

static    int	csd, ssd;
static	  PE	cpe;
static	  PE	spe;
static    struct  AcuSAPstart	cacs, sacs;
static    struct  AcuSAPstart	*pcacs = &cacs;
static    struct  AcuSAPstart	*psacs = &sacs;

static	  struct  type_ADD_Addends  Addends;
static	  struct  type_ADD_Addends  *pAddends = &Addends;
static	  struct  type_ADD_Sum  Sum;
static	  struct  type_ADD_Sum  *pSum = &Sum;

void acs_adios();
void acs_advise();
void adios();
void advise();
 
/*  */

/* ARGSUSED */

int main(argc, argv, envp)
int argc;
char **argv,
     **envp;
{
    char buffer[80], *bptr; 

    printf ("\n AcuSAP test driver for AUNITDATA\n");

    bptr = buffer;
    *bptr = 'h';
	
    while (1) {
        switch (toupper(*bptr)) {
            case 'H':
                printf("\n\nCommands are:\n");
		printf("  T - Toggle mode (default is client)\n");
		printf("  B - Bind service\n");
		printf("  U - Unbind service\n");
		printf("  L - Listen on a socket \n");
                printf("  X - Send an AUDT request\n");
                printf("  S - Send an AUDT datagram on binding\n");
		printf("  R - Receive an AUDT datagram on binding\n");
		printf("  V - Validate received application context/title\n");
		printf("  C - Rebind to last read caller\n");
		printf("  Q - Quit\n");
                break;
            case 'Q':
                printf("\nAcuSAP tester terminated by user\n");
                exit(0);
	    case 'T':
		printf ("\n Toggle mode \n");
		togglemode ();
		break;
	    case 'B':
		bind();
		break;
	    case 'U':
		unbind();
		break;
	    case 'L':
		listenup();
		break;
	    case 'X':
		printf ("\n Sending AUDT datagram directly\n");
		audtsendreq();
		break;
	    case 'S':
		printf ("\n Sending AUDT datagram on the binding\n");
		audtsend();
		break;
            case 'R':
		printf ("\n Reading AUDT datagram on the binding\n");
		audtread();
		break;
            case 'C':
		printf ("\n Rebinding to last caller\n");
		audtrebind();
		break;
            case '\0':
                break; /* for empty line */
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


/*-------------------------------------*/
togglemode()
/*-------------------------------------*/
{
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
}


/*-------------------------------------*/
int	bind()
/*-------------------------------------*/
{
/*  set binding variables: title, presentation address, and ctxlist */

    initbindvars();

    {
    if (mode == CLIENT_MODE)
	{
        printf ("\n Binding the client side\n");
	if ((csd = AcUnitDataBind (NOTOK, BIND_STATIC, ctx, NULLAEI, aei,
			            NULLPA, pa, pc, qos, aci) ) == NOTOK )
	    {	
	    printf ("\n Bind unit data service failed\n");
	    acs_adios (aca, "A-UNIT-DATA BIND");
	    }
    	else
	    printf ("\n application bound to socket = %d \n", csd);
	}
    else    /* SERVER_MODE */
	{
        printf ("\n Binding the server side\n");
	if ((ssd = AcUnitDataBind (NOTOK, BIND_DYNAMIC, ctx, aei, NULLAEI,
			            pa, NULLPA, pc, qos, aci) ) == NOTOK )
	    {
	    printf ("\n Bind unit data service failed\n");
	    acs_adios (aca, "A-UNIT-DATA BIND");
	    }
    	else
	    printf ("\n application bound to socket = %d \n", ssd);
	}
    } 
}

/*-------------------------------------*/
initbindvars()
/*-------------------------------------*/
{
/*  set up bind static variables for test */
	
    if ((aei = str2aei (myhost, myservice)) == NULLAEI)
	adios (NULLCP, "%s-%s: unknown application-entity", myhost, myservice);
    if ((pa = aei2addr (aei)) == NULLPA)
	adios (NULLCP, "address translation failed");
    printpaddr ( pa );

    if ((ctx = ode2oid (mycontext)) == NULLOID)
	adios (NULLCP, "%s: unknown object descriptor", mycontext);
    if ((ctx = oid_cpy (ctx)) == NULLOID)
	adios (NULLCP, "out of memory");
    if ((pci = ode2oid (mypci)) == NULLOID)
	adios (NULLCP, "%s: unknown object descriptor", mypci);
    if ((pci = oid_cpy (pci)) == NULLOID)
	adios (NULLCP, "out of memory");
    pc -> pc_nctx = 1;
    pc -> pc_ctx[0].pc_id = 1;
    pc -> pc_ctx[0].pc_asn = pci;
    pc -> pc_ctx[0].pc_atn = NULLOID;
}


/*-------------------------------------*/
int	unbind()
/*-------------------------------------*/
{
    if (mode == CLIENT_MODE)
        {
        printf ("\n Unbinding the client\n");
        if ( AcUnitDataUnbind (csd,aci) == NOTOK )
		printf ("unbind failed");
	else
		printf ( "unbind worked");
        }
    else
        {
        printf ("\n Unbinding the server\n");
        if ( AcUnitDataUnbind (ssd,aci) == NOTOK )
		printf ("unbind failed");
	else
		printf ( "unbind worked");
        }
}

	    
/*-------------------------------------*/
int	listenup()
/*-------------------------------------*/
{
;
/*    struct TSAPaddr   *ta;
    register int    n = ta -> ta_naddr - 1;
    register struct NSAPaddr   *na = ta -> ta_addrs;
static struct TSAPaddr *tz;
static struct TSAPaddr  tas[NTADDRS];
    int	    port;
    struct NSAPaddr *tcp_na;

    if (mode != SERVER_MODE)
	printf ("failed - may only listen on socket in SERVER mode");

    initbindvars();
*/
    /* if ((sp = getservbyname ("tsap", "tcp")) == NULL)
        printf ("failed - \ntcp/tsap: unknown service");
    */
		
/*
    tz = tas;
    tcp_na = tz -> ta_addrs;
    tcp_na -> na_type = NA_TCP;
    tcp_na -> na_domain[0] = NULL;
    tcp_na -> na_port = sp -> s_port;
    tz -> ta_naddr = 1;
	    
    sd = TUnitDataListen (tcp_na, NULL);

    if (sd == NOTOK)
        printf ("\n TNetListen failed\n");
    else
        {
	printf ("\n listening on socket = %d \n", sd);
        printf ("\n              port = %d \n", tcp_na -> na_port);
	}
*/
}

/*-------------------------------------*/
int audtsendreq()
/*-------------------------------------*/
{
/*  first set binding variables: title, presentation address, and ctxlist */
    initbindvars();
    {
    if (mode == CLIENT_MODE)
	{
	Addends.addend1 = 3; Addends.addend2 = 2;
        printf ("\n Client sending pair of addends directly: ");
	printf ("%d, %d\n", Addends.addend1, Addends.addend2);
	if ( encode_ADD_Addends ( &cpe, 1, 0, NULLCP, &Addends ) == NOTOK )
	    {
            printf ("\n encode failed\n");
	    return;
	    }
	cpe->pe_context = 1;
        if ( AcUnitDataRequest ( ctx, NULLAEI, aei, NULLPA, pa,
				 pc, &cpe, 1, qos, aci ) == NOTOK )
	    {
            printf ("\n AcUnitDataRequest failed\n");
            acs_adios (aca, "A-UNIT-DATA REQUEST");
	    }
    	else
	    printf ("\n AUDT sent\n");
	}
    else    /* SERVER_MODE */
	{
        printf ("\n Only supported on client side\n");
	}
    }
}

/*-------------------------------------*/
int audtsend()
/*-------------------------------------*/
{
int	sum, result;
    if (mode == CLIENT_MODE)
	{
	Addends.addend1 = 3; Addends.addend2 = 2;
        printf ("\n Client sending pair of addends: ");
	printf ("%d, %d\n", Addends.addend1, Addends.addend2);
	if ( encode_ADD_Addends ( &cpe, 1, 0, NULLCP, &Addends ) == NOTOK )
	    {
            printf ("\n encode failed\n");
	    return;
	    }
	cpe->pe_context = 1;
	result = AcUnitDataWrite (csd, &cpe, 1, aci );
	}
    else    /* SERVER_MODE */
	{
        printf ("\n Server sending sum = ");
	sum = pAddends->addend1 + pAddends->addend2;
	free_ADD_Addends(pAddends);
	Sum.parm = sum;
        printf ("%d\n", sum);
	if ( encode_ADD_Sum ( &spe, 1, 0, NULLCP, &Sum ) == NOTOK )
	    {
            printf ("\n encode failed\n");
	    return;
	    }
	spe->pe_context = 1;
	result = AcUnitDataWrite (ssd, &spe, 1, aci );
	}

    if ( result == NOTOK )
	{
        printf ("\n AcUnitDataWrite failed\n");
        acs_adios (aca, "A-UNIT-DATA WRITE");
	}
    else   
        printf ("\n write successful\n");
}
		

/*-------------------------------------*/
int audtread()
/*-------------------------------------*/
{
    if (mode == CLIENT_MODE)
	{
        printf ("\n Client reading sum\n");
	if ( AcUnitDataRead (csd, pcacs, 5, aci ) == NOTOK ) goto rfail;
/*
	if ( validbinding ( csd, pcacs ) == NOTOK )
	    printf ("\n validation of context and/or title failed\n");
*/
	if ( decode_ADD_Sum ( cacs.acs_info[0], 1, NULLIP, NULLVP, &pSum )
		== NOTOK )
            printf ("\n read ok but decode failed\n");
	else 
	    printf ( "\n successful: sum = %d\n", pSum->parm );
	ACSFREE(pcacs);
	free_ADD_Sum ( pSum );
	return;
	}
    else    /* SERVER_MODE */
	{
        printf ("\n Server reading addends\n");
	if ( AcUnitDataRead (ssd, psacs, 5, aci ) == NOTOK ) goto rfail;
/*
	if ( validbinding ( ssd, psacs ) == NOTOK )
	    printf ("\n validation of context and/or title failed\n");
*/
	Addends.addend1 = Addends.addend2 = 0;
	if ( decode_ADD_Addends (sacs.acs_info[0], 1, NULLIP, NULLVP, &pAddends)
		== NOTOK )
            printf ("\n read ok but decode failed\n");
	else 
	    printf ( "\n successful: Addends = %d, %d\n",
					 pAddends->addend1, pAddends->addend2 );
        clientaei = psacs -> acs_callingtitle;	/* struct copy */
	ACSFREE(psacs);
	return;
	}
rfail:;
	printf ("\n AcUnitDataRead() failed\n" );
        acs_adios (aca, "A-UNIT-DATA READ");
	return;
}	

/*-------------------------------------*/
int validbinding ( sd, acs )
/*-------------------------------------*/
int sd;
struct  AcuSAPstart	*acs;
{
    struct assocblk *acb;

#define	AEICMP(aei1,aei2) \
    (pe_cmp ((aei1) -> aei_ap_title, (aei2) -> aei_ap_title) \
  || pe_cmp ((aei1) -> aei_ae_qualifier, (aei2) -> aei_ae_qualifier)) 


    if ( (acb = findacublk (sd)) == NULL )
	{
	printf ("\n cannot find assocblk\n");
	return NOTOK;
	}

    if ( acb -> acb_context != acs -> acs_context )
	return NOTOK;

    if ( acb -> acb_callingtitle 
	    && (AEICMP(acb->acb_callingtitle,&acs->acs_calledtitle)) )
        return NOTOK;

    if ( acb -> acb_binding == BIND_STATIC 
            && acb -> acb_calledtitle
	    && AEICMP (acb -> acb_calledtitle, &acs -> acs_callingtitle ))
        return NOTOK;
    return OK;
}


/*-------------------------------------*/
int audtrebind()
/*-------------------------------------*/
{
    if (mode == CLIENT_MODE)
        printf ("\n Only support on server side\n");
    else    /* SERVER_MODE */
	{
        printf ("\n Rebinding server to new called address and title\n");

/* cheating til Kurt gets back 
    if ((pclientaei = str2aei (myhost, "clientacusaptest")) == NULLAEI)
	adios (NULLCP, "%s-%s: unknown application-entity",
		 myhost, "clientacusaptest");
    if ((cpa = aei2addr (pclientaei)) == NULLPA)
	adios (NULLCP, "address translation failed");
    printpaddr ( pa );
    if ( AcUnitDataRebind (ssd, pclientaei, cpa, aci) == NOTOK )
end cheat */

        if ( AcUnitDataRebind (ssd, &clientaei,
				 &psacs->acs_start.ps_calling, aci) == NOTOK )

	    {
	    printf ("\n Rebind unit data service failed\n");
	    acs_adios (aca, "A-UNIT-DATA REBIND");
	    }
    	else
	    printf ("\n application rebound to socket %d \n", ssd);
	}
}
		

/*-------------------------------------*/
static	printsrv (is)
/*-------------------------------------*/
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
/*-------------------------------------*/
static	printpaddr (pa)
/*-------------------------------------*/
register struct PSAPaddr *pa;
{
struct SSAPaddr   *sa = &(pa -> pa_addr);
struct TSAPaddr   *ta = &(sa -> sa_addr);
struct NSAPaddr   *na = ta -> ta_addrs;
int    n = ta -> ta_naddr - 1;

	printf ("ADDR:    PSEL: %s\n",
		sel2str (pa -> pa_selector, pa -> pa_selectlen, 1));

	printf ("ADDR:    SSEL: %s\n",
		sel2str (sa -> sa_selector, sa -> sa_selectlen, 1));

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




void	acs_adios (aca, event)
register struct AcSAPabort *aca;
char   *event;
{
    acs_advise (aca, event);

    _exit (1);
}


void	acs_advise (aca, event)
register struct AcSAPabort *aca;
char   *event;
{
    char    buffer[BUFSIZ];

    if (aca -> aca_cc > 0)
	(void) sprintf (buffer, "[%s] %*.*s",
		AcuErrString (aca -> aca_reason),
		aca -> aca_cc, aca -> aca_cc, aca -> aca_data);
    else
	(void) sprintf (buffer, "[%s]", AcuErrString (aca -> aca_reason));

	advise (NULLCP, "%s: %s (source %d)", event, buffer,
		aca -> aca_source);
}

/*  */

#ifndef	lint
void	_advise ();

void	adios (va_alist)
va_dcl
{
    va_list ap;
    va_start (ap);
    _advise (ap);
    va_end (ap);
    _exit (1);
}
#else
/* VARARGS */
void	adios (what, fmt)
char   *what,
       *fmt;
{
    adios (what, fmt);
}
#endif


#ifndef	lint
void	advise (va_alist)
va_dcl
{
    va_list ap;
    va_start (ap);
    _advise (ap);
    va_end (ap);
}

static void  _advise (ap)
va_list	ap;
{
    char    buffer[BUFSIZ];

    asprintf (buffer, ap);
    (void) fflush (stdout);
    fprintf (stderr, "%s: ", myservice);
    (void) fputs (buffer, stderr);
    (void) fputc ('\n', stderr);
    (void) fflush (stderr);
}
#else
/* VARARGS */

void	advise (what, fmt)
char   *what,
       *fmt;
{
    advise (what, fmt);
}
#endif


#ifndef	lint
void	ryr_advise (va_alist)
va_dcl
{
    char   *what;
    va_list ap;
    va_start (ap);
    _advise (ap);
    va_end (ap);
}
#else
/* VARARGS */

void	ryr_advise (what, fmt)
char   *what,
       *fmt;
{
    ryr_advise (what, fmt);
}
#endif
