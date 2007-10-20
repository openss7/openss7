
/*
 ****************************************************************
 *                                                              *
 *  ISODECL  -  HULA project for connectionless ISODE           *
 *                                             			*
 *  module:  	psapunitdata.c                                  *
 *  author:   	Bill Haggerty                                   *
 *  date:     	4/89                                            *
 *                                                              *
 *  This code implements the P-UNIT-DATA service.               *
 *                                  				*
 *  entry points:                   				*
 *                                  				*
 * 	PUnitDataRequest (calling, called, ctxlist,		*
 *				 data, ndata, qos, pi)          *
 * 	PUnitDataBind ( sd, calling, called, ctxlist, qos, pi)  *
 * 	PUnitDataRebind ( sd, called, pi )			*
 * 	PUnitDataWrite ( sd, data, ndata, pi )			*
 * 	PUnitDataRead ( sd, ps, secs, pi )			*
 * 	PUnitDataUnbind ( sd, pi )				*
 * 	PuSave ( sd, vecp, vec, pi )				*
 *                                                              *
 *  internal routines:                                          *
 *                                                              *
 * 	addrs2block ( callingaddr, calledaddr, pb, pi )		*
 * 	contexts2block ( contexts, nctx, pb, pi )		*
 * 	contexts2pdu ( pb, pdu )				*
 * 	pdu2contexts ( pb, ctxdeflist, ctxlist )		*
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

/* HULA P-UNIT-DATA Service */
/* psapunitdata.c - PPM: initiator and responder */
/* stolen from psapinitiate.c in ISODE's /psap2 library */


/* LINTLIBRARY */

#include <stdio.h>
#include <varargs.h>
#include <signal.h>
#include "PS-types.h"
#include "pupkt.h"
#include "isoservent.h"
#include "tailor.h"


/*---------------------------------------------------------------------------*/
/* ^L   P-UNIT-DATA.REQUEST */
/*      opens and closes a socket for each request */
/*---------------------------------------------------------------------------*/
int	PUnitDataRequest (calling, called, ctxlist, data, ndata, qos, pi)
/*---------------------------------------------------------------------------*/
struct	PSAPaddr *calling,
		*called;
struct	PSAPctxlist *ctxlist;
int	ndata;
PE	*data;
struct	QOStype *qos;
struct	PSAPindication *pi;
{
    SBV     smask;
    int     result;
    int	    i, len;
    PE	    pe;
    register struct psapblk *pb;
    struct SSAPindication sis;
    register struct SSAPindication *si = &sis;
    register struct SSAPabort *sa = &si -> si_abort;
    register struct type_PS_UD__type *pdu;
    register struct type_PS_User__data *info;

    isodetailor ("pusap2");

#ifdef notdef
    missingP (calling);
#endif
    missingP (called);

    if (ctxlist && ctxlist -> pc_nctx > NPCTX)
	return pusaplose (pi, PC_PARAMETER, NULLCP,
		"only %d presentation contexts supported", NPCTX);

/* let session provider catch errors in session parameters: qos  */

    toomuchP (data, ndata, NPDATA, "initial");
    missingP (pi);

    smask = sigioblock ();

    if ((pb = newpublk ()) == NULLPB) {
        (void) sigiomask (smask);
        return pusaplose (pi, PC_CONGEST, NULLCP, "out of memory");
	}

    if (called -> pa_selectlen > 0) {
	if (calling == NULLPA) {
	    static struct PSAPaddr pas;
	    calling = &pas;
	    bzero ((char *) calling, sizeof *calling);
	}

	if (calling -> pa_selectlen == 0) {
	    calling -> pa_port =
			    htons ((u_short) (0x8000 | (getpid () & 0x7fff)));
	    calling -> pa_selectlen = sizeof calling -> pa_port;
	}
    }

/*  Start building ppdu */

    pe = NULLPE;
    if ((pdu = (struct type_PS_UD__type *) calloc (1, sizeof *pdu)) == NULL)
	goto no_mem;

    if (calling
	    && calling -> pa_selectlen > 0
	    && (pdu -> calling = str2qb (calling -> pa_selector,
					    calling -> pa_selectlen, 1))
		    == NULL)
	goto no_mem;
    if (called -> pa_selectlen > 0
	    && (pdu -> called = str2qb (called -> pa_selector,
					    called -> pa_selectlen, 1))
		    == NULL)
	goto no_mem;

/*  Ignore this comment for initial testing !!!! */
/*  We do need to specify a "default" context for code that exists */

/*  ? Since there is no default context carried in the ppdu we skip the */
/*  following validation of ASN.1/BER for the default context */

    if ((pb -> pb_asn = ode2oid (DFLT_ASN)) == NULLOID)  {
	(void) pusaplose (pi, PC_ABSTRACT, NULLCP, "%s: unknown", DFLT_ASN);
	goto no_good;
    }
    if ((pb -> pb_asn = oid_cpy (pb -> pb_asn)) == NULLOID) {
	pusaplose (pi, PC_CONGEST, NULLCP, "out of memory");
	goto no_good;
    }
    if ((pb -> pb_atn = ode2oid (DFLT_ATN)) == NULLOID)  {
	(void) pusaplose (pi, PC_TRANSFER, NULLCP, "%s: unknown", DFLT_ATN);
	goto no_good;
    }
    if ((pb -> pb_atn = oid_cpy (pb -> pb_atn)) == NULLOID) {
	pusaplose (pi, PC_CONGEST, NULLCP, "out of memory");
	goto no_good;
    }


    if (ctxlist && ctxlist -> pc_nctx > 0) {

	if ( contexts2block ( ctxlist -> pc_ctx, ctxlist -> pc_nctx, pb, pi )
		 == NOTOK )
	    goto no_good;   

	if ( contexts2pdu ( pb, pdu ) == NOTOK )
	    goto no_mem;
	}


    if (data && ndata > 0
	     && (pdu -> user__data = info2_ppdu (pb, pi, data, ndata,
						  PPDU_UD)) == NULL)
	goto no_good;   


    pe = NULLPE;
    if (encode_PS_UD__type (&pe, 1, 0, NULLCP, pdu) == NOTOK) {
       (void) pusaplose (pi, PC_CONGEST, NULLCP, "error encoding PDU: %s",
            PY_pepy);
        goto no_good;
        }		


#ifdef	DEBUG
    if (psap2level & ISODELOG_PDUS)
	PS_print (pe, "UD-type", 0, print_PS_UD__type);
#endif


    if (pe2ssdu (pe, &pb -> pb_retry, &len) == NOTOK)
	goto no_mem;

    free_PS_UD__type (pdu);
    pdu = NULL;

    pe_free (pe);
    pe = NULLPE;

    if ( (result = SUnitDataRequest ( calling ? &calling -> pa_addr : NULLSA,
	    &called -> pa_addr, pb -> pb_retry, len, qos, si))  == NOTOK) {
	(void) ss2pulose (NULLPB, pi, "SUnitDataRequest", sa);
    }
    goto out;


no_mem: ;
    (void) pusaplose (pi, PC_CONGEST, NULLCP, "out of memory");
no_good: ;
    result = NOTOK;
    if (pe) pe_free (pe);
    if (pdu) free_PS_UD__type (pdu);
out: ;
    if ( pb -> pb_retry ) free ( pb -> pb_retry );    /* ??? */   
    freepublk (pb); 
    (void) sigiomask (smask);
    return result;
}


/*---------------------------------------------------------------------------*/
/* ^L   BIND  ...  for P-UNIT-DATA.REQUEST and P-UNIT-DATA.INDICATION */
/*      opens a socket for later, multiple reads and writes */
/*---------------------------------------------------------------------------*/
int	PUnitDataBind ( sd, calling, called, ctxlist, qos, pi)
/*---------------------------------------------------------------------------*/
int	sd;
struct	PSAPaddr *calling,
		*called;
struct	PSAPctxlist *ctxlist;
struct	QOStype *qos;
struct	PSAPindication *pi;
{
    SBV     smask;
    int     result;
    int	    i, len;
    PE	    pe;
    register struct psapblk *pb;
    struct SSAPindication sis;
    register struct SSAPindication *si = &sis;
    register struct SSAPabort *sa = &si -> si_abort;
    register struct type_PS_UD__type *pdu;
    register struct type_PS_User__data *info;

    isodetailor ("pusap2");

    if ( calling == NULLPA && called == NULLPA )
	return pusaplose (pi, PC_PARAMETER, NULLCP,
		"either calling or called address must be present");

    if (ctxlist && ctxlist -> pc_nctx > NPCTX)
	return pusaplose (pi, PC_PARAMETER, NULLCP,
		"only %d presentation contexts supported", NPCTX);

/* let session provider catch errors in session parameters: qos  */

    smask = sigioblock ();

    if (called && called -> pa_selectlen > 0) {
	if (calling == NULLPA) {
	    static struct PSAPaddr pas;
	    calling = &pas;
	    bzero ((char *) calling, sizeof *calling);
	}

	if (calling -> pa_selectlen == 0) {
	    calling -> pa_port =
			    htons ((u_short) (0x8000 | (getpid () & 0x7fff)));
	    calling -> pa_selectlen = sizeof calling -> pa_port;
	}
    }
 
    if ( sd == NOTOK ) {
        if ((pb = newpublk ()) == NULLPB) {
            (void) sigiomask (smask);
	    return pusaplose (pi, PC_CONGEST, NULLCP, "out of memory");
	    }
	}
    else  
        if ((pb = findpublk (sd)) == NULLPB || !(pb -> pb_flags & PB_PUDT)) {
            (void) sigiomask (smask);
	    return pusaplose (pi, PC_PARAMETER, NULLCP,
		"invalid presentation descriptor");
	    }

    if ( addrs2block ( calling, called, pb, pi ) == NOTOK )
	    goto no_good;


/*  Ignore this comment for initial testing !!!! */
/*  We do need to specify a "default" context for code that exists */

/*  ? Since there is no default context carried in the ppdu we skip the */
/*  following validation of ASN.1/BER for the default context */

    if ((pb -> pb_asn = ode2oid (DFLT_ASN)) == NULLOID)  {
	(void) pusaplose (pi, PC_ABSTRACT, NULLCP, "%s: unknown", DFLT_ASN);
	goto no_good;
    }
    if ((pb -> pb_asn = oid_cpy (pb -> pb_asn)) == NULLOID) {
	pusaplose (pi, PC_CONGEST, NULLCP, "out of memory");
	goto no_good;
    }
    if ((pb -> pb_atn = ode2oid (DFLT_ATN)) == NULLOID)  {
	(void) pusaplose (pi, PC_TRANSFER, NULLCP, "%s: unknown", DFLT_ATN);
	goto no_good;
    }
    if ((pb -> pb_atn = oid_cpy (pb -> pb_atn)) == NULLOID) {
	pusaplose (pi, PC_CONGEST, NULLCP, "out of memory");
	goto no_good;
    }


    if (ctxlist && ctxlist -> pc_nctx > 0) 
	if ( contexts2block ( ctxlist -> pc_ctx, ctxlist -> pc_nctx, pb, pi )
		== NOTOK )
	    goto no_good;

    if ( (result = SUnitDataBind ( pb -> pb_fd, &calling -> pa_addr,
				   called ? &called -> pa_addr : NULLSA,
				   qos, si )) == NOTOK) {
	(void) ss2pulose (NULLPB, pi, "SUnitDataBind", sa);
	goto no_good;
    }

    pb -> pb_fd = result;
    pb -> pb_flags |= PB_PUDT;
    (void) sigiomask (smask);
    return result;


no_good: ;
    freepublk (pb); 
    (void) sigiomask (smask);
    return NOTOK;
}


/*---------------------------------------------------------------------------*/
/* ^L   ReBIND  ...  resets called address on binding */
/*---------------------------------------------------------------------------*/
int	PUnitDataRebind ( sd, called, pi)
/*---------------------------------------------------------------------------*/
int	sd;
struct	PSAPaddr *called;
struct	PSAPindication *pi;
{
    SBV     smask;
    int     result;
    register struct psapblk *pb;
    register struct PSAPaddr *calling;
    struct SSAPindication sis;
    register struct SSAPindication *si = &sis;
    register struct SSAPabort *sa = &si -> si_abort;

    missingP (called);
    missingP (pi);

    smask = sigioblock ();

    if ((pb = findpublk (sd)) == NULLPB || !(pb -> pb_flags & PB_PUDT)) {
        (void) sigiomask (smask);
        return pusaplose (pi, PC_PARAMETER, NULLCP,
	    "invalid presentation descriptor");
        }

    calling = pb->pb_calling;

    if (called -> pa_selectlen > 0) {
	if (calling == NULLPA) {
	    static struct PSAPaddr pas;
	    calling = &pas;
	    bzero ((char *) calling, sizeof *calling);
	    }
	if (calling -> pa_selectlen == 0) {
	    calling -> pa_port =
			    htons ((u_short) (0x8000 | (getpid () & 0x7fff)));
	    calling -> pa_selectlen = sizeof calling -> pa_port;
	    }
        }

    if ( addrs2block ( calling, called, pb, pi ) == NOTOK ) {
        (void) sigiomask (smask);
        return NOTOK;
	}

    if ( SUnitDataBind ( pb -> pb_fd, &calling -> pa_addr,
				   &called -> pa_addr, NULL, si ) == NOTOK) {
	(void) ss2pulose (NULLPB, pi, "SUnitDataBind", sa);
        freepublk (pb); 
        (void) sigiomask (smask);
        return NOTOK;
    }

    (void) sigiomask (smask);
    return OK;
}




/*---------------------------------------------------------------------------*/
/*    send P-UNIT-DATA.REQUEST over locally bound association */
/*      socket must have been previously bound by PUnitDataBind() */
/*---------------------------------------------------------------------------*/
int	PUnitDataWrite ( sd, data, ndata, pi )
/*---------------------------------------------------------------------------*/
int	sd;
int	ndata;
PE     *data;
struct  PSAPindication *pi;
{
    SBV     smask;
    int     result;
    int	    i, len;
    PE	    pe;
    register struct psapblk *pb;
    struct SSAPindication sis;
    register struct SSAPindication *si = &sis;
    register struct SSAPabort *sa = &si -> si_abort;
    register struct type_PS_UD__type *pdu;
    register struct type_PS_User__data *info;
    register struct PSAPaddr *calling;
    register struct PSAPaddr *called;


    missingP (data);
    toomuchP (data, ndata, NPDATA, "user");
    if (ndata <= 0)
	return pusaplose (pi, PC_PARAMETER, NULLCP,
		    "illegal number of PSDUs (%d)", ndata);
    missingP (pi);

    smask = sigioblock ();

    if ((pb = findpublk (sd)) == NULLPB || !(pb -> pb_flags & PB_PUDT)) {
        (void) sigiomask (smask);
        return pusaplose (pi, PC_PARAMETER, NULLCP,
	    "invalid presentation descriptor");
        }

/*  Start building ppdu */

    pe = NULLPE;
    if ((pdu = (struct type_PS_UD__type *) calloc (1, sizeof *pdu)) == NULL)
	goto no_mem;

    if ( (calling = pb -> pb_calling)
	    && calling -> pa_selectlen > 0
	    && (pdu -> calling = str2qb (calling -> pa_selector,
					    calling -> pa_selectlen, 1))
		    == NULL)
	goto no_mem;

    if ( (called = pb -> pb_called)
            && called -> pa_selectlen > 0
	    && (pdu -> called = str2qb (called -> pa_selector,
					    called -> pa_selectlen, 1))
		    == NULL)
	goto no_mem;


    if ( pb -> pb_ncontext > 0 )
        if ( contexts2pdu ( pb, pdu ) == NOTOK )
            goto no_mem;


    if (data && ndata > 0
	     && (pdu -> user__data = info2_ppdu (pb, pi, data, ndata,
						  PPDU_UD)) == NULL)
	goto no_good;   


    pe = NULLPE;
    if (encode_PS_UD__type (&pe, 1, 0, NULLCP, pdu) == NOTOK) {
       (void) pusaplose (pi, PC_CONGEST, NULLCP, "error encoding PDU: %s",
            PY_pepy);
        goto no_good;
        }		


#ifdef	DEBUG
    if (psap2level & ISODELOG_PDUS)
	PS_print (pe, "UD-type", 0, print_PS_UD__type);
#endif


    if (pe2ssdu (pe, &pb -> pb_retry, &len) == NOTOK)
	goto no_mem;

    free_PS_UD__type (pdu);
    pdu = NULL;

    pe_free (pe);
    pe = NULLPE;

    if ((result = SUnitDataWrite ( sd, pb -> pb_retry, len, si )) == NOTOK ) 
	(void) ss2pulose (NULLPB, pi, "SUnitDataWrite", sa);
    goto out;



no_mem: ;
    (void) pusaplose (pi, PC_CONGEST, NULLCP, "out of memory");
no_good: ;
    result = NOTOK;
    if (pe) pe_free (pe);
    if (pdu) free_PS_UD__type (pdu);
out: ;
    if ( pb -> pb_retry ) free ( pb -> pb_retry );    /* ??? */   
    (void) sigiomask (smask);
    return result;
}





/*---------------------------------------------------------------------------*/
/*    get P-UNIT-DATA.INDICATON over locally bound association */
/*      socket must have been previously bound by PUnitDataBind() */
/*---------------------------------------------------------------------------*/
int	PUnitDataRead ( sd, ps, secs, pi )
/*---------------------------------------------------------------------------*/
int	sd;
struct  PuSAPstart *ps;
int	secs;
struct  PSAPindication *pi;
{
    SBV     smask;
    int     result, i, len;
    PE	    pe = NULLPE;
    char    *base;
    register struct psapblk *pb;
    struct SuSAPstart sss;
    register struct SuSAPstart *ss = &sss;
    struct SSAPindication sis;
    register struct SSAPindication *si = &sis;
    register struct SSAPabort *sa = &si -> si_abort;
    struct type_PS_UD__type *pdu;

    missingP (ps);
    missingP (pi);

    smask = sigioblock ();

    if ((pb = findpublk (sd)) == NULLPB || !(pb -> pb_flags & PB_PUDT)) {
        (void) sigiomask (smask);
        return pusaplose (pi, PC_PARAMETER, NULLCP,
	    "invalid presentation descriptor");
        }

    bzero ((char *) ps, sizeof *ps);
    bzero ((char *) pi, sizeof *pi);
    pdu = NULL;

    if ((result = SUnitDataRead (pb -> pb_fd, ss, secs, si)) == NOTOK) {
        (void) sigiomask (smask);
        if (sa -> sa_reason == SC_TIMER)
            return pusaplose (pi, PC_TIMER, NULLCP, NULLCP);
        return ss2pulose (pb, pi, NULLCP, sa);
	}

    if ((pe = ssdu2pe (ss -> ss_data, ss -> ss_cc, NULLCP, &result))
	    == NULLPE) {
	if (result == PS_ERR_NMEM) goto no_mem;
	(void) pusaplose ( pi, PC_PROTOCOL, NULLCP, "%s", ps_error (result));
	goto no_good;
        }

    SUSFREE (ss);    /* free the ss_data */

    if (decode_PS_UD__type (pe, 1, NULLIP, NULLVP, &pdu) == NOTOK) {
       (void) pusaplose (pi, PC_UNRECOGNIZED, NULLCP, "error decoding PDU: %s",
            PY_pepy);
        goto no_good;
        }		

#ifdef	DEBUG
    if (psap2level & ISODELOG_PDUS)
	PS_print (pe, "UD-type", 1, print_PS_UD__type);
#endif

    pe_free (pe);
    pe = ps -> ps_info[0] = NULLPE;


/*  build PuSAPstart structure */

    ps -> ps_sd = pb -> pb_fd;
    ps -> ps_ssdusize = ss -> ss_ssdusize;
    ps -> ps_sversion = ss -> ss_version; 
    ps -> ps_qos = ss -> ss_qos;    /* struct copy, because its there */ 

    ps -> ps_calling.pa_addr = ss -> ss_calling;    /* struct copy */
    if (pdu -> calling) {
	if ((base = qb2str (pdu -> calling)) == NULLCP)
	    goto no_mem;
	if ((len = pdu -> calling -> qb_len) >
	         sizeof ps -> ps_calling.pa_selector)
	    len = sizeof ps -> ps_calling.pa_selector;
	bcopy (base, ps -> ps_calling.pa_selector,
		ps -> ps_calling.pa_selectlen = len);
	free (base);
    }
    ps -> ps_called.pa_addr = ss -> ss_called;      /* struct copy */
    if (pdu -> called) {
	if ((base = qb2str (pdu -> called)) == NULLCP)
	    goto no_mem;
	if ((len = pdu -> called -> qb_len) >
	         sizeof ps -> ps_called.pa_selector)
	    len = sizeof ps -> ps_called.pa_selector;
	bcopy (base, ps -> ps_called.pa_selector,
		ps -> ps_called.pa_selectlen = len);
	free (base);
    }

    if ( pdu2contexts (pb, pdu -> context__list, &ps -> ps_ctxlist) == NOTOK ) {
        (void) pusaplose (pi, PC_TRANSFER, NULLCP,
			 "transfer syntax not supported");
	goto no_good;
	}
	
    result = ppdu2data (pb, pi, ps, pdu -> user__data);
    goto out;
 
no_mem: ;
    (void) pusaplose (pi, PC_CONGEST, NULLCP, "out of memory");
no_good: ;
    result = NOTOK;
    SUSFREE (ss);
    if (pe) pe_free (pe);
out: ;
    if (pdu) free_PS_UD__type (pdu);
    (void) sigiomask (smask);
    return result;
}



/*---------------------------------------------------------------------------*/
/*    clear local binding for P-UNIT-DATA */
/*---------------------------------------------------------------------------*/
int	PUnitDataUnbind ( sd, pi)
/*---------------------------------------------------------------------------*/
int	sd;
struct  PSAPindication *pi;
{
    SBV     smask;
    int     result;
    register struct psapblk *pb;
    struct SSAPindication sis;
    register struct SSAPindication *si = &sis;
    register struct SSAPabort *sa = &si -> si_abort;

    missingP (pi);
    smask = sigioblock ();

    if ((pb = findpublk (sd)) == NULLPB || !(pb -> pb_flags & PB_PUDT)) {
        (void) sigiomask (smask);
        return pusaplose (pi, PC_PARAMETER, NULLCP,
	    "invalid presentation descriptor");
        }

    if ((result = SUnitDataUnbind (pb -> pb_fd, si))
	    	== NOTOK) {
        (void) ss2pulose (pb, pi, "PUnitDataUnbind", sa);
	if (SC_FATAL (sa -> sa_reason))
	    goto out2;
	else
	    goto out1;
	}
    result = OK;

out2: ;
    freepublk (pb);

out1: ;
    (void) sigiomask (smask);
    return result;
}



/*---------------------------------------------------------------------------*/
/*    save magic args (TPDU) for local P-UNIT-DATA binding                 */
/*---------------------------------------------------------------------------*/
int	PuSave ( sd, vecp, vec, pi )
/*---------------------------------------------------------------------------*/
int	sd;
int	vecp;
char    **vec;
struct  PSAPindication *pi;
{
    int     result;
    register struct psapblk *pb;
    struct SSAPindication sis;
    register struct SSAPindication *si = &sis;
    register struct SSAPabort *sa = &si -> si_abort;

    isodetailor ("psap");

    missingP (vec);
    missingP (pi);

/*  assumes here only when process spawned for initial UD indication */
/*  but may precede (sd == NOTOK) or follow call to PUnitDataBind() */

    if ( sd == NOTOK ) {
	if ( (pb = newpublk ()) == NULL )
	    return pusaplose (pi, PC_CONGEST, NULLCP, "out of memory");
	pb->pb_fd = NOTOK;
	}
    else 
        if ( (pb = findpublk (sd)) == NULLPB || !(pb -> pb_flags & PB_PUDT)) 
	    return pusaplose (pi, PC_PARAMETER, NULLCP,
		"invalid presentation descriptor");
	    

    if ( (result = SuSave ( sd, vecp, vec, si) ) == NOTOK) {
        (void) ss2pulose (NULLPB, pi, "PuSave", sa);
        if ( sd == NOTOK ) freepublk (pb);
	return NOTOK;
	}

    pb->pb_fd = result;
    return result;
}


/*---------------------------------------------------------------------------*/
int	addrs2block ( callingaddr, calledaddr, pb, pi )
/*---------------------------------------------------------------------------*/
struct PSAPaddr *callingaddr,
		*calledaddr;
struct psapblk *pb;
struct PSAPindication *pi;
{
register struct PSAPaddr *pa;

    if ( !calledaddr )
	pb -> pb_called = NULLPA;
    else if ( calledaddr != pb -> pb_called ){
        if ( pb -> pb_called ) free (pb -> pb_called);
        if ( (pa = (struct PSAPaddr *) calloc (1, sizeof *calledaddr))
		 == NULLPA ) goto no_mem;
        *pa = *calledaddr;	/* struct copy */
	pb -> pb_called = pa;
	}

    if ( !callingaddr )
	pb -> pb_calling = NULLPA;
    else if ( callingaddr != pb -> pb_calling ){
        if ( pb -> pb_calling ) free (pb -> pb_calling);
        if ( (pa = (struct PSAPaddr *) calloc (1, sizeof *callingaddr))
		 == NULLPA) goto no_mem;
        *pa = *callingaddr;	/* struct copy */
	pb -> pb_calling = pa;
	}
    return OK;
	
no_mem: ;
    pusaplose (pi, PC_CONGEST, NULLCP, "out of memory");
    return NOTOK;
}


/*----------------------------------------------------------------------------*/
int	contexts2block ( contexts, nctx, pb, pi )
/*----------------------------------------------------------------------------*/
struct  PSAPcontext *contexts;
int     nctx;
struct  psapblk *pb;
struct	PSAPindication *pi;
{
    int i;
    register struct PSAPcontext *pp, *qp;

	i = nctx - 1;
	for (pp = contexts, qp = pb -> pb_contexts;
		i >= 0;
		i--, pp++, qp++){
	    if (!((qp -> pc_id = pp -> pc_id) & 01)) {
		(void) pusaplose (pi, PC_PARAMETER, NULLCP,
			    "only odd values allowed for context identifiers");
		goto out2;
	    }

	    if (pp -> pc_asn == NULLOID) {
		(void) pusaplose (pi, PC_PARAMETER, NULLCP,
			    "no abstract syntax name given for context %d",
			    pp -> pc_id);
		goto out2;
	    }
	    if ((qp -> pc_asn = oid_cpy (pp -> pc_asn)) == NULLOID)
		goto no_mem;

	    if (pp -> pc_atn && !atn_is_ok (pb, pp -> pc_atn)) {
		(void) pusaplose (pi, PC_TRANSFER, NULLCP,
			    "unknown transfer syntax name given for context %d",
			    pp -> pc_id);
		goto out2;
	    }
	    if ((qp -> pc_atn = oid_cpy (pp -> pc_atn ? pp -> pc_atn
			: pb -> pb_atn)) == NULLOID)
		goto no_mem;

/*  The following line is superfluous since P-UNIT.DATA is unconfirmed */
	    qp -> pc_result = PC_ACCEPT;

	    pb -> pb_ncontext++;
	}
	return OK;


no_mem: ;
    (void) pusaplose (pi, PC_CONGEST, NULLCP, "out of memory");
out2: ;
    return NOTOK;
}


/*----------------------------------------------------------------------------*/
int	contexts2pdu ( pb, pdu )
/*----------------------------------------------------------------------------*/
struct psapblk *pb;
struct type_PS_UD__type *pdu;
{
    int i;
    register struct PSAPcontext  *qp;
    register struct type_PS_Definition__list *cd, **cp;

    cp = &pdu -> context__list;

    i = pb -> pb_ncontext - 1;

    for ( qp = pb -> pb_contexts; i >= 0; i--, qp++) 
	{
        if ((cd =(struct type_PS_Definition__list *) calloc(1, sizeof *cd))
                == NULL)
	    goto no_mem;

        *cp = cd;
        cp = &cd -> next;

        if ((cd -> element_PS_5 = (struct element_PS_6 *)
	        calloc (1, sizeof *cd -> element_PS_5)) == NULL)
	    goto no_mem;

        cd -> element_PS_5 -> identifier = qp -> pc_id;
        if ((cd -> element_PS_5 -> abstract__syntax = oid_cpy (qp -> pc_asn))
	        == NULLOID
            || (cd -> element_PS_5 -> transfer__syntax__list =
	        (struct element_PS_7 *)calloc (1, sizeof (struct element_PS_7)))
	        == NULL
            || (cd -> element_PS_5 -> transfer__syntax__list ->
	            Transfer__syntax__name = oid_cpy (qp -> pc_atn))
	        == NULL)
	    goto no_mem;
	}

    return OK;

no_mem: ;
    return NOTOK;
}



/*----------------------------------------------------------------------------*/
int	pdu2contexts ( pb, ctxdeflist, ctxlist )
/*----------------------------------------------------------------------------*/
struct psapblk *pb;
struct type_PS_Definition__list *ctxdeflist;
struct PSAPctxlist *ctxlist;
{
    int i, j, result;
    register struct PSAPcontext  *pp, *qp;
    register struct type_PS_Definition__list *lp;

    result = OK;
    i = 0;
    for (lp = ctxdeflist, pp = ctxlist -> pc_ctx;
	    lp;
	    lp = lp -> next, pp++, i++)
        {
	register struct element_PS_6 *pctx = lp -> element_PS_5;
	register struct element_PS_7 *atn;

	pp -> pc_id = pctx -> identifier;
        pp -> pc_asn = pctx -> abstract__syntax;
        pctx -> abstract__syntax = NULLOID;

/*    assure context has been previously bound */

        j = pb -> pb_ncontext;
   	for ( qp = pb -> pb_contexts; j > 0; j--, qp++)  
	    if ( oid_cmp ( pp -> pc_asn, qp -> pc_asn ) == 0 ) { 
	        pp -> pc_result = PC_ACCEPT;
	        break;
		}
	if ( j == 0 )
            pp -> pc_result = PC_REJECTED;

/* PUDT standard is mixed up about the list of transfer syntax names > 1 */
/* but let's process it anyway since thats the ASN.1 for ctxlist */

	for (atn = pctx -> transfer__syntax__list; atn; atn = atn -> next)
	    if (atn_is_ok (pb, atn -> Transfer__syntax__name)) {
		pp -> pc_atn = atn -> Transfer__syntax__name;
		atn -> Transfer__syntax__name = NULLOID;
		break;
		}
	if (atn == NULL) {
	    result = NOTOK;
	    pp -> pc_result = PC_TRANSFER;
	    }
	}
    ctxlist -> pc_nctx = i;
    return result;
}

/* HULA ends */
