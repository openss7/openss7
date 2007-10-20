/*
 ****************************************************************
 *                                                              *
 *  HULA project - connectionless ISODE                         *
 *                                             			*
 *  module:  	acsapunitdat.c                                  *
 *  author:   	Bill Haggerty                                   *
 *  date:     	4/89                                            *
 *                                                              *
 *  This code implements ACSE for the A-UNIT-DATA service.      *
 *                                  				*
 *  entry points:                   				*
 *                                  				*
 * 	AcUnitDataRequest ( context, callingtitle, calledtitle, *
 *		 	    callingaddr, calledaddr, ctxlist,   * 
 * 			    data, ndata, qos, aci )             *
 * 	AcUnitDataBind ( sd, binding, context, callingtitle,    *
 *			 calledtitle, callingaddr, calledaddr,  *
 *			 ctxlist, qos, aci )                    *
 * 	AcUnitDataRebind ( sd, calledtitle, calledaddr, aci )   *
 *      AcUnitDataWrite ( sd, data, ndata, aci )                *
 * 	AcUnitDataRead ( sd, acs, secs, aci )                   *
 * 	AcUnitDataUnbind (sd, aci)                              *
 * 	AcuSave (sd, vecp, vec, aci)                            *
 *                                                              *
 *  internal routines:                                          *
 *                                                              *
 * 	titles2block ( callingtitle, calledtitle, acb, aci )    *
 * 	titles2pdu ( callingtitle, calledtitle, pdu )           *
 * 	ctx2block ( ctxlist, acb, ppcx, aci )                   *
 * 	validaudtctx ( ctx, acb, ps, aci )                      *
 * 	pdu2start ( pdu, acs )                                  *
 *								*
 ****************************************************************
 *								*
 *			     NOTICE		   		*
 *								*
 *    Use of this module is subject to the restrictions of the  *
 *    ISODE license agreement.					*
 *								*    
 ****************************************************************
 */

/* modified from ISODE's acsapinitiat.c and acsaprespond.c */
/* acsapinitiat.c - ACPM: initiator */

#ifndef	lint
static char *rcsid = "$Header: /f/iso/acsap/RCS/acsapinitiat.c,v 5.0 88/07/21 14:21:35 mrose Rel $";
#endif


/* LINTLIBRARY */

#include <stdio.h>
#include <signal.h>
#include "ACS-types.h"
#define	ACSE
#include "acupkt.h"
#include "acusap.h"
#include "isoservent.h"
#include "tailor.h"


#define	BER		"asn.1 basic encoding"
#define NULLACC		((struct AcSAPconnect *)0)




/*---------------------------------------------------------------------------*/
/*    A-UNIT-DATA.REQUEST */
/*      opens and closes a socket for each request */
/*---------------------------------------------------------------------------*/
int	AcUnitDataRequest ( context, callingtitle, calledtitle,
		 	    callingaddr, calledaddr, ctxlist,
			    data, ndata, qos, aci )
/*---------------------------------------------------------------------------*/
OID	context;
AEI	callingtitle,
	calledtitle;
struct PSAPaddr *callingaddr,
		*calledaddr;
struct PSAPctxlist *ctxlist;
PE     *data;
int	ndata;
struct  QOStype *qos;
struct  AcSAPindication *aci;
{
    SBV     smask;
    int     result;
    PE	    pe;
    register struct assocblk *acb;
    struct PSAPcontext *pp;
    struct PSAPindication pis;
    register struct PSAPindication *pi = &pis;
    register struct PSAPabort *pa = &pi -> pi_abort;
    register struct type_ACS_AUDT__apdu *pdu;

    isodetailor ("acsap");

    missingP (context);
#ifdef	notdef
    missingP (callingtitle);
    missingP (calledtitle);
#endif

    missingP (data);
    toomuchP (data, ndata, NACDATA, "user");
    if ( ndata <= 0 )
	(void) acusaplose (aci, ACS_PARAMETER, NULLCP,
			 "illegal number of ASDUs (%d)", ndata );
    missingP (aci);


    smask = sigioblock ();

    if ((acb = newacublk ()) == NULL) {
        (void) sigiomask (smask);
	return acusaplose (aci, ACS_CONGEST, NULLCP, "out of memory");
	}

    pe = NULLPE;
    if ((pdu = (struct type_ACS_AUDT__apdu *) calloc (1, sizeof *pdu))
	    == NULL) {
	acusaplose (aci, ACS_CONGEST, NULLCP, "out of memory");
	goto no_good;
        }

    pdu -> application__context__name = context;

    (void) titles2pdu ( callingtitle, calledtitle, pdu );

/*  Unfortunately info2_apdu() below only does fully encoded data */
/*  Will change to allow simple encoding later */

    if (data && ndata > 0
	     && (pdu -> user__information = info2_apdu (acb, aci, data, ndata))
			== NULL)
	goto no_good;

    if ( encode_ACS_AUDT__apdu (&pe, 1, 0, NULLCP, pdu) == NOTOK ) {
	(void) acusaplose (aci, ACS_CONGEST, NULLCP, "error encoding PDU: %s",
			  PY_pepy);
	goto no_good;
        }


    pp = NULLPC;
    if ( ctx2block (ctxlist, acb, &pp, aci) == NOTOK ) /* set AUDT p-context */
        goto no_good;
    pe -> pe_context = acb -> acb_id;   


#ifdef	DEBUG
    if (acsaplevel & ISODELOG_PDUS) ACU_print (pe, "AUDT-apdu", 0);
#endif


    result = PUnitDataRequest (callingaddr, calledaddr,
                               ctxlist, &pe, 1, qos, pi);

    if (pp) {    /* created an AUDT context for PUnitDataRequest, so free it */ 
        ctxlist -> pc_nctx--;
	pp -> pc_id = 0;
        oid_free (pp -> pc_asn);
        if (pp -> pc_atn)
            oid_free (pp -> pc_atn);
        pp -> pc_asn = pp -> pc_atn = NULLOID;
        }

    if ( result == NOTOK) {
        (void) ps2aculose (NULLACB, aci, "PUnitDataRequest", pa);
	goto no_good;
        }
    goto out;


no_good: ;
    result = NOTOK;
out: ;
    if (pdu) {
	if (pdu -> user__information)
	    free_ACS_Association__information (pdu -> user__information);
	free ((char *) pdu);
    }
    if (pe)
	pe_free (pe);
    freeacublk (acb);
    (void) sigiomask (smask);
    return result;
}




/*---------------------------------------------------------------------------*/
/*    set a local binding for AcUnitDataWrite() and AcUnitDataRead()       */ 
/*      a socket is created and bound if sd is NOTOK on input                */
/*---------------------------------------------------------------------------*/
int	AcUnitDataBind ( sd, binding, context, callingtitle, calledtitle,
			     callingaddr, calledaddr, ctxlist, qos, aci )
/*---------------------------------------------------------------------------*/
int	sd;
int	binding;
OID	context;
AEI	callingtitle,
	calledtitle;
struct  PSAPaddr *callingaddr,
 		*calledaddr;
struct  PSAPctxlist *ctxlist;
struct  QOStype *qos;
struct  AcSAPindication *aci;
{
    SBV     smask;
    int     result;
    register int    i;
    register struct assocblk *acb;
    struct PSAPcontext *pp;
    struct PSAPindication pis;
    register struct PSAPindication *pi = &pis;
    register struct PSAPabort *pa = &pi -> pi_abort;

    isodetailor ("acsap");

    missingP (context);
    missingP (aci);

/* binding is static by default */
/* but if dynamic calledaddr and calledtitle may be reset, AcUnitDataRebind */

    if ( binding == BIND_DYNAMIC )
        { missingP (callingaddr); }
    else
        { missingP (calledaddr); }

    smask = sigioblock ();

/*  Bind remembers the input parameters, not the processed AUDT PE */
/*  necessary to unhook the AUDT PE from user space. */


    if ( sd == NOTOK ) {
	if ((acb = newacublk ()) == NULL) {
            (void) sigiomask (smask);
	    return acusaplose (aci, ACS_CONGEST, NULLCP, "out of memory");
       	    } 		
	acb -> acb_fd = sd;
	}
    else { 
        if ((acb = findacublk (sd)) == NULL || !(acb -> acb_flags & ACB_AUDT)) {
            (void) sigiomask (smask);
	    return acusaplose (aci, ACS_PARAMETER, NULLCP,
		"invalid association descriptor");
	    }
	}

    acb -> acb_binding = ((binding == BIND_DYNAMIC) ? binding : BIND_STATIC);

    if ((acb -> acb_context = oid_cpy (context)) == NULLOID) {
	(void) acusaplose (aci, ACS_CONGEST, NULLCP, NULLCP);
	goto no_good;
        }


    if ( titles2block ( callingtitle, calledtitle, acb, aci ) == NOTOK ) 
        goto no_good;


        {
        pp = NULLPC;
	if ( ctx2block ( ctxlist, acb, &pp, aci ) == NOTOK ) 
            goto no_good;

        result = PUnitDataBind ( acb -> acb_fd, callingaddr, calledaddr,
				 ctxlist, qos, pi );

        if (pp) {    /* free part of created AUDT context */ 
            ctxlist -> pc_nctx--;
	    pp -> pc_id = 0;
            if (pp -> pc_atn)
                oid_free (pp -> pc_atn);
            pp -> pc_asn = pp -> pc_atn = NULLOID;
            }
        }


    if ( result == NOTOK ) {
        (void) ps2aculose (NULLACB, aci, "PUnitDataBind", pa);
        goto no_good;
        }	

    acb -> acb_fd = result;
/*  acb -> acb_uabort = PUnitDataUnbind; ? */

/*  set flags - for connectionless both sides of binding "association" */
/*                  should be able to initiate operations */
    acb -> acb_flags |= ACB_AUDT;
    acb -> acb_flags |= ACB_ACS;  /* may be necessary to fake out rosap */
    acb -> acb_flags |= ACB_INIT; /* may be necessary to fake out rosap */

    (void) sigiomask (smask);
    return result;


no_good: ;
    freeacublk (acb);
    (void) sigiomask (smask);
    return NOTOK;
}


/*---------------------------------------------------------------------------*/
/*    reset a local binding for new called addr and called title           */ 
/*---------------------------------------------------------------------------*/
int	AcUnitDataRebind ( sd, calledtitle, calledaddr, aci )
/*---------------------------------------------------------------------------*/
int	sd;
AEI	calledtitle;
struct  PSAPaddr *calledaddr;
struct  AcSAPindication *aci;
{
    SBV     smask;
    int     result;
    register struct assocblk *acb;
    struct PSAPindication pis;
    register struct PSAPindication *pi = &pis;
    register struct PSAPabort *pa = &pi -> pi_abort;

    missingP (calledaddr);
    missingP (aci);

    smask = sigioblock ();

    if ((acb = findacublk (sd)) == NULL || !(acb -> acb_flags & ACB_AUDT)) {
        acusaplose (aci, ACS_PARAMETER, NULLCP,
	    "invalid association descriptor");
	goto no_good;
        }

    if ( acb -> acb_binding != BIND_DYNAMIC ) {
        acusaplose (aci, ACS_PARAMETER, NULLCP,"association cannot be rebound");
	goto no_good;
        }

/*  free any old called title */
    if ( acb -> acb_calledtitle ) {
        AEIFREE ( acb -> acb_calledtitle ); 
        free ( acb -> acb_callingtitle );  
	}

    if ( titles2block ( acb -> acb_callingtitle, calledtitle, acb, aci )
	    == NOTOK ) 
        goto no_good;

    if ( PUnitDataRebind ( acb -> acb_fd, calledaddr, pi) == NOTOK ) {
	(void) ps2aculose (NULLACB, aci, "PUnitDataRebind", pa);
	goto no_good;
	}
    (void) sigiomask (smask);
    return OK;


no_good: ;
    (void) sigiomask (smask);
    return NOTOK;
}




/*---------------------------------------------------------------------------*/
/*    set off A-UNIT-DATA.REQUEST over locally bound association */
/*      socket must have been previously bound by AcUnitDataBind() */
/*---------------------------------------------------------------------------*/
int	AcUnitDataWrite ( sd, data, ndata, aci )
/*---------------------------------------------------------------------------*/
int	sd;
int	ndata;
PE     *data;
struct  AcSAPindication *aci;
{
    SBV     smask;
    int     result;
    register int    i;
    PE	    pe;
    register struct assocblk *acb;
    register struct PSAPcontext *pp;
    struct PSAPindication pis;
    register struct PSAPindication *pi = &pis;
    register struct PSAPabort *pa = &pi -> pi_abort;
    register struct type_ACS_AUDT__apdu *pdu;


    missingP (data);
    toomuchP (data, ndata, NACDATA, "user");
    if ( ndata <= 0 )
	(void) acusaplose (aci, ACS_PARAMETER, NULLCP,
			 "illegal number of ASDUs (%d)", ndata );

    missingP (aci);

    smask = sigioblock ();

    if ((acb = findacublk (sd)) == NULL || !(acb -> acb_flags & ACB_AUDT)) {
        (void) sigiomask (smask);
	return acusaplose (aci, ACS_PARAMETER, NULLCP,
		"invalid association descriptor");
	}

    if ((pdu = (struct type_ACS_AUDT__apdu *) calloc (1, sizeof *pdu))
	    == NULL) {
	acusaplose (aci, ACS_CONGEST, NULLCP, "out of memory");
	goto no_good;
        }

    pdu -> application__context__name = acb -> acb_context;


    (void) titles2pdu ( acb -> acb_callingtitle,  acb -> acb_calledtitle, pdu );


/*  Unfortunately info2_apdu() below only does fully encoded data */
/*  Will change to allow simple encoding later */

    if ((pdu -> user__information = info2_apdu (acb, aci, data, ndata)) == NULL)
	goto no_good;

    pe = NULLPE;
    if ( encode_ACS_AUDT__apdu (&pe, 1, 0, NULLCP, pdu) == NOTOK ) {
	(void) acusaplose (aci, ACS_CONGEST, NULLCP, "error encoding PDU: %s",
			  PY_pepy);
	goto no_good;
        }

    pe -> pe_context = acb -> acb_id;   
    if ( (result = PUnitDataWrite ( sd, &pe, 1, pi ))  ==  NOTOK ) 
	(void) ps2aculose (NULLACB, aci, "PUnitDataWrite", pa);
    goto out;


no_good: ;
    result = NOTOK;
/* ? should we distinguish fatal vs nonfatal reasons ?
    PUnitDataUnbind ( sd, pi );
    freeacublk (acb);
*/
out: ;
    if (pdu) {
	if (pdu -> user__information)
	    free_ACS_Association__information (pdu -> user__information);
	free ((char *) pdu);
    }
    if (pe)
	pe_free (pe);
    (void) sigiomask (smask);
    return result;
}




/*---------------------------------------------------------------------------*/
/*    implements A-UNIT-DATA.INDICATION on locally bound association */
/*      socket must have been previously bound by AcUnitDataBind() which */
/*      bound both the application context and all valid p-contexts */
/*---------------------------------------------------------------------------*/
int	AcUnitDataRead ( sd, acs, secs, aci )
/*---------------------------------------------------------------------------*/
int	sd;
struct	AcuSAPstart	*acs;
int	secs;
struct  AcSAPindication *aci;
{
    SBV     smask;
    int     result;
    int     ctx;
    register int    i;
    PE	    pe;
    register struct assocblk *acb;
    register struct PuSAPstart *ps;
    struct PSAPindication   pis;
    register struct PSAPindication *pi = &pis;
    register struct PSAPabort  *pa = &pi -> pi_abort;
    struct type_ACS_AUDT__apdu *pdu;


    isodetailor ("acsap");

    missingP (acs);
    missingP (aci);

    smask = sigioblock ();

    if ((acb = findacublk (sd)) == NULL || !(acb -> acb_flags & ACB_AUDT)) {
        (void) sigiomask (smask);
	return acusaplose (aci, ACS_PARAMETER, NULLCP,
		"invalid association descriptor");
	}

    ps = &acs -> acs_start;
    bzero ((char *) acs,  sizeof *acs );

    if ( PUnitDataRead (acb -> acb_fd, ps, secs, pi) == NOTOK ) {
	(void) ps2aculose (NULLACB, aci, "PUnitDataRead", pa);
        return NOTOK;
	}

    pdu = NULL;
    if (ps -> ps_ninfo < 1) {
	(void) acusaplose (aci, ACS_PROTOCOL, NULLCP,
			  "no user-data on P-UNIT-DATA");
	goto no_good;
    }

    pe = ps -> ps_info[0];
    ctx = pe -> pe_context;
    result = decode_ACS_AUDT__apdu (pe, 1, NULLIP, NULLVP, &pdu);

#ifdef	DEBUG
    if (result == OK && (acsaplevel & ISODELOG_PDUS))
	ACU_print (pe, "AUDT-apdu", 1);
#endif

    pe_free (pe);
    pe = ps -> ps_info[0] = NULLPE;
    
    if (result == NOTOK) {
	(void) acusaplose (aci, ACS_PROTOCOL, NULLCP, "%s", PY_pepy);
	goto no_good;
    }

/************/
/*  validate presentation context for AUDT ACSE PCI */
/*  where HULA folded AUDT PCI into abstract syntax for ACSE PCI */
/*  and assure application context is present */  
/************/

    if ( validaudtctx ( ctx, acb, ps, aci ) == NOTOK )
	goto no_good;

    if ( pdu -> application__context__name == NULLOID ) {
	(void) acusaplose (aci, ACS_CONGEST, NULLCP, NULLCP);
	goto no_good;
    }

/************/
/*  now set the AcuSAPstart structure for the indication */
/************/
  
    acs -> acs_sd = acb -> acb_fd;

    (void) pdu2start ( pdu, acs ); /* move context and titles to start struct */

    if (apdu2_info (acb, aci, pdu -> user__information, acs -> acs_info,
		   &acs -> acs_ninfo) == NOTOK)
	goto no_good;

    for (i = ps -> ps_ninfo - 1; i >= 0; i--)
	if (ps -> ps_info[i]) {
	    pe_free (ps -> ps_info[i]);
	    ps -> ps_info[i] = NULL;
	}
    ps -> ps_ninfo = 0;
    free_ACS_AUDT__apdu (pdu);
    return OK;
    

no_good: ;
    if (pdu)
	free_ACS_AUDT__apdu (pdu);
    PUSFREE (ps);
/*  freeacublk (acb); ? */
    return NOTOK;
}



/*---------------------------------------------------------------------------*/
/*    clear local binding for A-UNIT-DATA */
/*      similar to AcUAbortRequest() */ 
/*---------------------------------------------------------------------------*/
int	AcUnitDataUnbind (sd, aci)
/*---------------------------------------------------------------------------*/
int	sd;
struct AcSAPindication *aci;
{
    SBV     smask;
    int     result;
    register struct assocblk  *acb;
    struct PSAPindication pis;
    register struct PSAPindication *pi = &pis;
    register struct PSAPabort  *pa = &pi -> pi_abort;

    missingP (aci);

    smask = sigioblock ();

    if ((acb = findacublk (sd)) == NULL) {
	(void) sigiomask (smask);
	return acusaplose (aci, ACS_PARAMETER, NULLCP,
		"invalid association descriptor");
        }

    if ((result = PUnitDataUnbind (acb -> acb_fd, pi))
	    	== NOTOK) {
	    (void) ps2aculose (acb, aci, "PUnitDataUnbind", pa);
	    if (PC_FATAL (pa -> pa_reason))
		goto out2;
	    else
		goto out1;
	}

    result = OK;

out2: ;
    freeacublk (acb);

out1: ;
    (void) sigiomask (smask);
    return result;
}




/*---------------------------------------------------------------------------*/
/*    save magic args (TPDU) for local A-UNIT-DATA binding                 */
/*---------------------------------------------------------------------------*/
int	AcuSave (sd, vecp, vec, aci)
/*---------------------------------------------------------------------------*/
int	sd;
int	vecp;
char    **vec;
struct AcSAPindication *aci;
{
    int     result;
    register struct assocblk  *acb;
    struct PSAPindication pis;
    register struct PSAPindication *pi = &pis;
    register struct PSAPabort  *pa = &pi -> pi_abort;

    isodetailor ("acsap");

    missingP (vec);
    missingP (aci);

/*  assume here only when process spawned by daemon on initial UD indication */
/*  so no need to hold signals with sigioblock() */

/*  if AcuSave() is called before AcUnitDataBind() (sd==NOTOK) */ 
/*  association descriptor returned is input to AcUnitDataBind() */

    if ( sd == NOTOK ) {
	if ((acb = newacublk ()) == NULL) 
	    return acusaplose (aci, ACS_CONGEST, NULLCP, "out of memory");
	acb -> acb_fd = NOTOK;
	}
    else  
        if ((acb = findacublk (sd)) == NULL || !(acb -> acb_flags & ACB_AUDT)) 
	    return acusaplose (aci, ACS_PARAMETER, NULLCP,
		"invalid association descriptor");
	

    if ( (result = PuSave ( sd, vecp, vec, pi) ) == NOTOK) {
        (void) ps2aculose (NULLACB, aci, "PuSave", pa);
        if ( sd == NOTOK ) freeacublk (acb);
	return NOTOK;
	}

    acb->acb_fd = result;
    return result;
}



/*---------------------------------------------------------------------------*/
int	titles2block ( callingtitle, calledtitle, acb, aci )
/*---------------------------------------------------------------------------*/
AEI	callingtitle,
	calledtitle;
struct assocblk *acb;
struct  AcSAPindication *aci;
{
    register AEI    pi, po;

    if ( !callingtitle )
        acb -> acb_callingtitle = NULLAEI;
    else if ( callingtitle != acb -> acb_callingtitle ) 
        {
        acb -> acb_callingtitle = NULLAEI;
        if ( (po = ( AEI ) calloc (1, sizeof *callingtitle)) == NULLAEI )
	    goto no_mem;
	acb -> acb_callingtitle = po;
	pi = callingtitle;
	if ( pi->aei_ap_title )
	    if ((po-> aei_ap_title = pe_cpy (pi-> aei_ap_title)) == NULLPE )
		goto no_mem;
	if ( pi->aei_ae_qualifier )
	    if ((po-> aei_ae_qualifier = pe_cpy (pi-> aei_ae_qualifier))
		 == NULLPE) goto no_mem;
	po -> aei_ap_id = pi -> aei_ap_id;
	po -> aei_ae_id = pi -> aei_ae_id;
	po -> aei_flags = pi -> aei_flags;
	}

    if ( !calledtitle )
        acb -> acb_calledtitle = NULLAEI;
    else if ( calledtitle != acb -> acb_calledtitle ) 
        {
        if ( (po = ( AEI ) calloc (1, sizeof *calledtitle)) == NULLAEI )
	    goto no_mem;
	acb -> acb_calledtitle = po;
	pi = calledtitle;
	if ( pi->aei_ap_title )
	    if ((po-> aei_ap_title = pe_cpy (pi-> aei_ap_title)) == NULLPE )
		goto no_mem;
	if ( pi->aei_ae_qualifier )
	    if ((po-> aei_ae_qualifier = pe_cpy (pi-> aei_ae_qualifier))
		 == NULLPE) goto no_mem;
	po -> aei_ap_id = pi -> aei_ap_id;
	po -> aei_ae_id = pi -> aei_ae_id;
	po -> aei_flags = pi -> aei_flags;
	}
    return OK;

no_mem: ;
    if ( acb -> acb_callingtitle ) {
        AEIFREE ( acb -> acb_callingtitle ); 
        free ( acb -> acb_callingtitle );  
	}
    if ( acb -> acb_calledtitle ) {
        AEIFREE ( acb -> acb_calledtitle ); 
        free ( acb -> acb_calledtitle );  
        }
    acusaplose (aci, ACS_CONGEST, NULLCP, "out of memory");
    return NOTOK;
}



/*---------------------------------------------------------------------------*/
int	titles2pdu ( callingtitle, calledtitle, pdu )
/*---------------------------------------------------------------------------*/
    AEI	    callingtitle,
	    calledtitle;
    struct  type_ACS_AUDT__apdu *pdu;
{
    if (calledtitle) {
	pdu -> called__AP__title = calledtitle -> aei_ap_title;
	pdu -> called__AE__qualifier = calledtitle -> aei_ae_qualifier;
	if (calledtitle -> aei_flags & AEI_AP_ID)
	    pdu -> called__AP__invocation__id =
		(struct type_ACS_AP__invocation__id *)
		    &calledtitle -> aei_ap_id;
	if (calledtitle -> aei_flags & AEI_AE_ID)
	    pdu -> called__AE__invocation__id =
	    	(struct type_ACS_AE__invocation__id *)
	    	    &calledtitle -> aei_ae_id;
    }
    if (callingtitle) {
	pdu -> calling__AP__title = callingtitle -> aei_ap_title;
	pdu -> calling__AE__qualifier = callingtitle -> aei_ae_qualifier;
	if (callingtitle -> aei_flags & AEI_AP_ID)
	    pdu -> calling__AP__invocation__id =
		(struct type_ACS_AP__invocation__id *)
		    &callingtitle -> aei_ap_id;
	if (callingtitle -> aei_flags & AEI_AE_ID)
	    pdu -> calling__AE__invocation__id =
	    	(struct type_ACS_AE__invocation__id *)
	    	    &callingtitle -> aei_ae_id;
    }
}


/*---------------------------------------------------------------------------*/
int	ctx2block ( ctxlist, acb, ppcx, aci )
/*---------------------------------------------------------------------------*/
struct  PSAPctxlist *ctxlist;
struct assocblk *acb;
register struct PSAPcontext **ppcx;
struct  AcSAPindication *aci;
{
    register int ctx;
    register int i;
    register OID oid;
    register struct PSAPcontext *pcx;

    acb -> acb_audtpci = NULLOID;
    if (ctxlist -> pc_nctx >= NPCTX) 
        return acusaplose (aci, ACS_PARAMETER, NULLCP, "too many contexts");
    
/* HULA assumes the OID of the AUDT apdu is the same as other ACSE apdus */
/* but to get the oid for acse pci should no longer be a database read */ 

    if ((oid = ode2oid (AC_ASN)) == NULLOID) 
        return acusaplose (aci, ACS_PARAMETER, NULLCP, "%s: unknown", AC_ASN);

/*  HULA is allowing, at least for now, a default context for AUDT */
    if ( ctxlist -> pc_nctx <= 0 ) {
	acb -> acb_id = PE_DFLT_CTX; 
	return OK;
	}

    i = ctxlist -> pc_nctx - 1, ctx = 1;
    for (pcx = ctxlist -> pc_ctx; i >= 0; i--, pcx++) {
        if (oid_cmp (pcx -> pc_asn, oid) == 0) {
	    acb -> acb_id = pcx -> pc_id;
            acb -> acb_audtpci = oid_cpy ( pcx -> pc_asn );
            return OK;		
            }
        else
	    if (acb -> acb_rosid == PE_DFLT_CTX)
	        acb -> acb_rosid = pcx -> pc_id;
	if (ctx <= pcx -> pc_id)
	    ctx = pcx -> pc_id + 2;
	}

/*  there were context(s) input but not one for AUDT - will create one */

    pcx -> pc_id = ctx;
    if ((pcx -> pc_asn = oid_cpy (oid)) == NULLOID)
	goto no_mem;

    if ((pcx -> pc_atn = ode2oid (BER)) == NULLOID)
	{
        oid_free (pcx -> pc_asn);
        return acusaplose (aci, ACS_PARAMETER, NULLCP, "%s: unknown", BER);
	}

    if ((pcx -> pc_atn = oid_cpy (pcx -> pc_atn)) == NULLOID ) {
        oid_free (pcx -> pc_asn);
        goto no_mem;
	}        

    ctxlist -> pc_nctx++;
    acb -> acb_id = pcx -> pc_id;
    acb -> acb_audtpci = pcx -> pc_asn;
    *ppcx =  pcx;
    return OK;

no_mem: ;
    return acusaplose (aci, ACS_CONGEST, NULLCP, "out of memory");
}



/*---------------------------------------------------------------------------*/
int	validaudtctx ( ctx, acb, ps, aci )
/*---------------------------------------------------------------------------*/
    int		ctx;
    register struct PuSAPstart *ps;
    struct assocblk *acb;
    struct  AcSAPindication *aci;
{
register int i;
register struct PSAPcontext *pp;

/*  checks p-context on AUDT indication equals local AUDT PCI context */
/*  where AUDT PCI = ACSE PCI since HULA put AUDT in with ACSE */
	
    i = ps -> ps_ctxlist.pc_nctx;

    if ( acb -> acb_id == PE_DFLT_CTX ) {
        if ( i == 0 )
	    return OK;
        if ( i > 1 )
	    return acusaplose (aci, ACS_PROTOCOL, NULLCP,
				"contexts not supported");
	}

/*  since only the indirect reference context id was retrieved from the PE, */
/*  must search through the context list on the indication to get direct ref */
 
    for (pp = ps -> ps_ctxlist.pc_ctx; i > 0; i--, pp++) {
	if ( pp -> pc_id == ctx ) break;
	if ( pp -> pc_result == PC_ACCEPT) 
	    acb -> acb_rosid = pp -> pc_id;
	}
		
    if ( i <= 0 ) 
	return acusaplose (aci, ACS_PROTOCOL, NULLCP,
				 "wrong ASN for ACSE");
    if (pp -> pc_result != PC_ACCEPT) 
        return acusaplose (aci, ACS_PROTOCOL, NULLCP,
				"PCI for ACSE not accepted");

    if (oid_cmp (pp -> pc_asn, acb -> acb_audtpci))
	return acusaplose (aci, ACS_PROTOCOL, NULLCP,
				 "wrong ASN for ACSE");
    return OK;
}



/*---------------------------------------------------------------------------*/
int	pdu2start ( pdu, acs )
/*---------------------------------------------------------------------------*/
    struct type_ACS_AUDT__apdu *pdu;
    struct	AcuSAPstart	*acs;
{
    acs -> acs_context = pdu -> application__context__name;
    pdu -> application__context__name = NULLOID;

    acs -> acs_callingtitle.aei_ap_title = pdu -> calling__AP__title;
    pdu-> calling__AP__title = NULLPE;
    acs -> acs_callingtitle.aei_ae_qualifier =
	pdu -> calling__AE__qualifier;
    pdu -> calling__AE__qualifier = NULLPE;
    if (pdu -> calling__AP__invocation__id) {
	acs -> acs_callingtitle.aei_ap_id =
	    pdu -> calling__AP__invocation__id -> parm;
	acs -> acs_callingtitle.aei_flags |= AEI_AP_ID;
    }
    if (pdu -> calling__AE__invocation__id) {
	acs -> acs_callingtitle.aei_ae_id =
	    pdu -> calling__AE__invocation__id -> parm;
	acs -> acs_callingtitle.aei_flags |= AEI_AE_ID;
    }

    acs -> acs_calledtitle.aei_ap_title = pdu -> called__AP__title;
    pdu -> called__AP__title = NULLPE;
    acs -> acs_calledtitle.aei_ae_qualifier =
	pdu -> called__AE__qualifier;
    pdu -> called__AE__qualifier = NULLPE;

    if (pdu -> called__AP__invocation__id) {
	acs -> acs_calledtitle.aei_ap_id =
	    pdu -> called__AP__invocation__id -> parm;
	acs -> acs_calledtitle.aei_flags |= AEI_AP_ID;
    }
    if (pdu -> called__AE__invocation__id) {
	acs -> acs_calledtitle.aei_ae_id =
	    pdu -> called__AE__invocation__id -> parm;
	acs -> acs_calledtitle.aei_flags |= AEI_AE_ID;
    }
}
