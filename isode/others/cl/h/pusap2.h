/* HULA */
/* pusap2.h - include file for P-UNIT-DATA presentation users  */
/* subject to license agreement for ISODE */

#ifndef	_PUSAP2_
#define	_PUSAP2_

#include "psap.h"

#ifndef	_ISOADDRS_
#include "isoaddrs.h"		/* definitions of ISO addresses */
#endif



/*  */


#define	NPDATA		25	/* arbitrary */

struct PSAPcontext {		/* presentation context */
    int	    pc_id;		/* identifier */

    OID	    pc_asn;		/* abstract syntax name */

    OID	    pc_atn;		/* abstract transfer name */
				/*   NULLOID if provider should handle it */
    
    int	    pc_result;		/* same codes as pc_result below */
};

struct PSAPctxlist {		/* list of presentation contexts */
    int	    pc_nctx;		/* number of contexts */

#define	NPCTX	10		/* arbitrary */
    struct PSAPcontext pc_ctx[NPCTX];
};
#define	NULLPC	((struct PSAPctxlist *) 0)



struct PuSAPstart {		/* P-UNIT-DATA.INDICATION */
    int	    ps_sd;		/* PRESENTATION descriptor */

    struct PSAPaddr ps_calling;	/* address of peer calling */
    struct PSAPaddr ps_called;	/* address of peer called */

    struct PSAPctxlist ps_ctxlist;  /* list of P-UNIT-DATA contexts */

    int	    ps_ssdusize;	/* largest atomic SSDU */
    int	    ps_sversion;	/* session service version number */
    
    struct QOStype ps_qos;	/* quality of service */
    
    int	    ps_result;		/* result */

#define	PC_ACCEPT	(-1)
#define	PC_REJECTED	0	/* Rejected by peer */
				/* Provider-reason */
#define	PC_NOTSPECIFIED	1	/* Reason not specified */
#define	PC_CONGEST	2	/* Temporary congestion */
#define	PC_EXCEEDED	3	/* Local limit exceeded */
#define	PC_ADDRESS	4	/* Called presentation address unknown */
#define	PC_VERSION	5	/* Protocol version not supported */
#define	PC_DEFAULT	6	/* Default context not supported */
#define	PC_READABLE	7	/* User-data not readable */
#define	PC_AVAILABLE	8	/* No PSAP available */
				/* Abort-reason */
#define	PC_UNRECOGNIZED	9	/* Unrecognized PPDU */
#define	PC_UNEXPECTED	10	/* Unexpected PPDU */
#define	PC_SSPRIMITIVE	11	/* Unexpected session service primitive */
#define	PC_PPPARAM1	12	/* Unrecognized PPDU parameter */
#define	PC_PPPARAM2	13	/* Unexpected PPDU parameter */
#define	PC_INVALID	14	/* Invalid PPDU parameter value */
				/* Provider-reason */
#define	PC_ABSTRACT	15	/* Abstract syntax not supported */
#define	PC_TRANSFER	16	/* Proposed transfer syntaxes not supported */
#define	PC_DCSLIMIT	17	/* Local limit on DCS exceeded */
				/* begin UNOFFICIAL */
#define	PC_REFUSED	18	/* Connect request refused on this network
				   connection */
#define	PC_SESSION	19	/* Session disconnect */
#define	PC_PROTOCOL	20	/* Protocol error */
#define	PC_ABORTED	21	/* Peer aborted connection */
#define	PC_PARAMETER	22	/* Invalid parameter */
#define	PC_OPERATION	23	/* Invalid operation */
#define	PC_TIMER	24	/* Timer expired */
#define	PC_WAITING	25	/* Indications waiting */
				/* end UNOFFICIAL */

#define	PC_FATAL(r)	((r) < PC_PARAMETER)
#define	PC_OFFICIAL(r)	((r) < PC_REFUSED)
				/* initial data from peer */

    int	    ps_ninfo;		/*   number of elements */
    PE	    ps_info[NPDATA];	/*   data */
};
#define	PUSFREE(ps) \
{ \
    register int PSI; \
 \
    for (PSI = (ps) -> ps_ctxlist.pc_nctx - 1; PSI >= 0; PSI--) { \
	oid_free ((ps) -> ps_ctxlist.pc_ctx[PSI].pc_asn); \
	oid_free ((ps) -> ps_ctxlist.pc_ctx[PSI].pc_atn); \
	(ps) -> ps_ctxlist.pc_ctx[PSI].pc_asn = \
	    (ps) -> ps_ctxlist.pc_ctx[PSI].pc_atn = NULLOID; \
    } \
    (ps) -> ps_ctxlist.pc_nctx = 0; \
    for (PSI = (ps) -> ps_ninfo - 1; PSI >= 0; PSI--) \
	if ((ps) -> ps_info[PSI]) \
	    pe_free ((ps) -> ps_info[PSI]), (ps) -> ps_info[PSI] = NULLPE; \
    (ps) -> ps_ninfo = 0; \
}


struct PSAPabort {		/* P-{U,P}-ABORT.INDICATION */
    int	    pa_peer;		/* T   = P-U-ABORT.INDICATION:
					    pa_info/pa_ninfo is meaningful
				   NIL = P-P-ABORT.INDICATION:
					    pa_reason/pa_ppdu is meaningful,
					    pa_data contains diagnostics */

    int	    pa_reason;		/* same codes as pc_result */

				/* abort information from peer */
    int	    pa_ninfo;		/*   number of elements */
    PE	    pa_info[NPDATA];	/*   data */

				/* diagnostics from provider */
#define	PA_SIZE		512
    int	    pa_cc;		/*   length */
    char    pa_data[PA_SIZE];	/*   data */
};
#define	PAFREE(pa) \
{ \
    register int PAI; \
 \
    for (PAI = (pa) -> pa_ninfo - 1; PAI >= 0; PAI--) \
	if ((pa) -> pa_info[PAI]) \
	    pe_free ((pa) -> pa_info[PAI]), (pa) -> pa_info[PAI] = NULLPE; \
    (pa) -> pa_ninfo = 0; \
}


struct PSAPindication {
    int	    pi_type;		/* union for compatiblity with connections */
#define	PI_ABORT	0x06
    union {
	struct PSAPabort pi_un_abort;
    }	pi_un;
#define	pi_abort	pi_un.pi_un_abort
};


/*  */

extern char *psap2version;

char   *PuErrString ();		/* return PSAP error code in string form */

int	PUnitDataRequest ();	/* P-UNIT-DATA.REQUEST */
int	PUnitDataBind ();	/* set P-UNIT-DATA local binding */
int	PUnitDataRebind ();	/* reset local binding called PSAPaddr */
int	PUnitDataWrite ();	/* P-UNIT-DATA.REQUEST on local binding */
int	PUnitDataRead ();	/* P-UNIT-DATA.INDICATION on local binding */
int	PUnitDataUnbind ();	/* release P-UNIT-DATA local binding */
int	PuSave ();	        /* save TPDU for PUDT indication on binding */

#endif

