/* isoaddrs.h - ISO addressing */

/* 
 * $Header: /f/iso/h/RCS/isoaddrs.h,v 5.0 88/07/21 14:39:01 mrose Rel $
 *
 *
 * $Log$
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

#ifndef	_ISOADDRS_
#define	_ISOADDRS_

#ifndef	_MANIFEST_
#include "manifest.h"
#endif
#ifndef	_GENERAL_
#include "general.h"
#endif

/*  */

#ifdef	NULLPE
typedef struct AEInfo {		/* "real" directory services! */
    PE	    aei_ap_title;
    PE	    aei_ae_qualifier;

    int	    aei_ap_id;
    int	    aei_ae_id;

    int	    aei_flags;
#define	AEI_NULL	0x00
#define	AEI_AP_ID	0x01
#define	AEI_AE_ID	0x02
}	AEInfo, *AEI;
#define	NULLAEI		((AEI) 0)
#define	AEIFREE(aei) { \
    if ((aei) -> aei_ap_title) \
	pe_free ((aei) -> aei_ap_title), \
	    (aei) -> aei_ap_title = NULLPE; \
    if ((aei) -> aei_ae_qualifier) \
	pe_free ((aei) -> aei_ae_qualifier), \
	    (aei) -> aei_ae_qualifier = NULLPE; \
}

AEI	str2aei ();

char   *sprintaei ();
#endif

/*  */

struct NSAPaddr {
    long     na_type;	/* this structure shouldn't have holes in it */
#define	NA_NSAP	0
#define	NA_TCP	1
#define	NA_X25	2
#define	NA_BRG	3

#define	NASIZE	64
    union {
	struct na_nsap {		/* real network service */
	    char    na_nsap_address[NASIZE];
	    char    na_nsap_addrlen;
	}               un_na_nsap;

	struct na_tcp {			/* emulation via RFC1006 */
	    char    na_tcp_domain[NASIZE];/* e.g., nrtc.northrop.com */
	    u_short na_tcp_port;	/* non-standard TCP port */
	    u_short na_tcp_tset;	/* transport set */
#define	NA_TSET_TCP	0x0001		/*   .. TCP */
#define	NA_TSET_UDP	0x0002	        /*   .. UDP */
	}               un_na_tcp;

	struct na_x25 {			/* X.25 (assume single subnet) */

#define	NSAP_DTELEN	36
	    char    na_x25_dte[NSAP_DTELEN + 1]; /* Numeric DTE + Link */
	    char    na_x25_dtelen;	/* number of digits used */

/* Conventionally, the PID sits at the first head bytes of user data and so
 * should probably not be mention specially. A macros might do it, if
 * necessary.
 */

#define	NPSIZE	4
	    char    na_x25_pid[NPSIZE];	/* X.25 protocol id */
	    char    na_x25_pidlen;	/*   .. */

#define	CUDFSIZE 16
	    char    na_x25_cudf[CUDFSIZE];/* call user data field */
	    char    na_x25_cudflen;	/* .. */
/*
 * X25 Facilities field. 
 */
#define	FACSIZE	6
	    char    na_x25_fac[FACSIZE];	/* X.25 facilities */
	    char    na_x25_faclen;		/*   .. */
	}               un_na_x25;
    }               na_un;

#define	na_address	na_un.un_na_nsap.na_nsap_address
#define	na_addrlen	na_un.un_na_nsap.na_nsap_addrlen

#define	na_domain	na_un.un_na_tcp.na_tcp_domain
#define	na_port		na_un.un_na_tcp.na_tcp_port
#define	na_tset		na_un.un_na_tcp.na_tcp_tset

#define	na_dte		na_un.un_na_x25.na_x25_dte
#define	na_dtelen	na_un.un_na_x25.na_x25_dtelen
#define	na_pid		na_un.un_na_x25.na_x25_pid
#define	na_pidlen	na_un.un_na_x25.na_x25_pidlen
#define	na_cudf		na_un.un_na_x25.na_x25_cudf
#define	na_cudflen	na_un.un_na_x25.na_x25_cudflen
#define	na_fac		na_un.un_na_x25.na_x25_fac
#define	na_faclen	na_un.un_na_x25.na_x25_faclen
};
#define	NULLNA			((struct NSAPaddr *) 0)


struct TSAPaddr {
#define	NTADDR	3
    struct NSAPaddr ta_addrs[NTADDR];	/* choice of network addresses */
    int     ta_naddr;

#define	TSSIZE	64			/* would prefer 16, but ICL transport
					   wants 32, and SEK likes 64... */
    int	    ta_selectlen;

    union {				/* TSAP selector */
	char    ta_un_selector[TSSIZE];

	u_short ta_un_port;
    }               un_ta;
#define	ta_selector	un_ta.ta_un_selector
#define	ta_port		un_ta.ta_un_port
};
#define	NULLTA			((struct TSAPaddr *) 0)


struct SSAPaddr {
    struct TSAPaddr sa_addr;		/* transport address */

#define	SSSIZE	64			/* would prefer 16... */
    int	    sa_selectlen;

    union {				/* SSAP selector */
	char    sa_un_selector[SSSIZE];

	u_short sa_un_port;
    }               un_sa;
#define	sa_selector	un_sa.sa_un_selector
#define	sa_port		un_sa.sa_un_port
};
#define	NULLSA			((struct SSAPaddr *) 0)


struct PSAPaddr {
    struct SSAPaddr pa_addr;		/* session address */

#define	PSSIZE	64			/* would prefer 16... */
    int	    pa_selectlen;

    union {				/* PSAP selector */
	char    pa_un_selector[PSSIZE];

	u_short pa_un_port;
    }               un_pa;
#define	pa_selector	un_pa.pa_un_selector
#define	pa_port		un_pa.pa_un_port
};
#define	NULLPA			((struct PSAPaddr *) 0)

struct PSAPaddr *aei2addr ();	/* application entity title to PSAPaddr */

/*  */

#ifdef	NULLPE
struct dse_service {
    int	    dse_id;

    char   *dse_designator;
    char   *dse_qualifier;
};

struct dse_entity {
    int	    dse_id;

    AEInfo    dse_aei;

    struct PSAPaddr dse_addr;
};

char   *alias2name ();
#endif

#ifdef	NULLOID
struct isoentity {		/* for stub directory service */
    OIDentifier ie_identifier;
    char    *ie_descriptor;

    struct PSAPaddr ie_addr;
};

int	setisoentity (), endisoentity ();

struct isoentity *getisoentity ();

AEI	oid2aei ();
#endif


				/* old-style */
struct PSAPaddr *is2paddr ();	/* service entry to PSAPaddr */
struct SSAPaddr *is2saddr ();	/* service entry to SSAPaddr */
struct TSAPaddr *is2taddr ();	/* service entry to TSAPaddr */


struct PSAPaddr *asn2paddr ();	/* ASN.1 value to PSAPaddr */
struct TSAPaddr *asn2taddr ();	/* ASN.1 value to TSAPaddr */

char   *paddr2asn ();		/* PSAPaddr to ASN.1 value */
char   *taddr2asn ();		/* TSAPaddr to ASN.1 value */

/*  */

struct nsap_entry {
    char   *ns_service;

    IFP	    ns_parse;
};

extern struct nsap_entry _nsap_entries[];


struct PSAPaddr *str2paddr ();  /* string encoding to PSAPaddr */
struct TSAPaddr *str2taddr ();  /* string encoding to TSAPaddr */

char   *paddr2str ();		/* PSAPaddr to string encoding */
char   *taddr2str ();		/* TSAPaddr to string encoding */

char   *na2str ();		/* pretty-print NSAPaddr */

/*  */

int	isodeserver ();		/* generic server dispatch */

/*  */

/* all of this really should be in "isoqos.h" ... */
   
/* ISODE doesn't use this... yet */

struct QOStype {
    int	    qos_reliability;	/* "reliability" element */
#define	HIGH_QUALITY	0
#define	LOW_QUALITY	1
};
#define	NULLQOS	((struct QOStype *) 0)

#endif
