/* tailor.h - ISODE tailoring */

/* 
 * $Header: /f/iso/h/RCS/tailor.h,v 5.0 88/07/21 14:39:37 mrose Rel $
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

#ifndef	_TAILOR_
#define	_TAILOR_

extern char    *isodename;
extern char    *isodepath;

extern char    *isodeconfig;
extern int	isodeconlen;

#ifndef	BSD42
extern	char   *isodelog;
#endif

#define	ISODELOG_NONE		0
#define	ISODELOG_EXCEPTIONS	(1 << 0)	/* log exceptions */
#define	ISODELOG_PDUS		(1 << 1)	/* trace pdus */
#define	ISODELOG_DEBUG		(1 << 2)	/* full debugging */
#define	ISODELOG_MASK		"\020\01EXCEPTIONS\02PDUS\03DEBUG"


void	xsprintf (), _xsprintf ();


extern	int	compatlevel;
extern	char   *compatfile;

extern	int	addrlevel;
extern	char   *addrfile;

extern	int	tsaplevel;
extern	char   *tsapfile;

extern	int	ssaplevel;
extern	char   *ssapfile;

extern	int	psaplevel;
extern	char   *psapfile;

extern	int	psap2level;
extern	char   *psap2file;

extern	int	acsaplevel;
extern	char   *acsapfile;

extern	int	rtsaplevel;
extern	char   *rtsapfile;

extern	int	rosaplevel;
extern	char   *rosapfile;

#ifdef	X25
#ifdef	SUN_X25
extern  u_char  reverse_charge;
extern  u_short recvpktsize;
extern  u_short sendpktsize;
extern  u_char  recvwndsize;
extern  u_char  sendwndsize;
extern  u_char  recvthruput;
extern  u_char  sendthruput;
extern  u_char  cug_req;
extern  u_char  cug_index;
extern  u_char  fast_select_type;
extern  u_char  rpoa_req;
extern  u_short rpoa;
#endif

#if	defined(CAMTEC) || defined(CAMTEC_CCL)
extern char    *x25_lsubaddr;
extern char     x25_lsubprefix;
extern int      x25_lsublen;
extern int      x25_dtelen;
extern int      x25_start_range;
extern int      x25_end_range;
extern char     x25_outgoing_port;
extern int      x25_port;
#endif

#if	defined(UBC_X25) || defined(CAMTEC) || defined(CAMTEC_CCL) || defined(SUN_X25)
extern char    *x25_local_janet_dte;
extern char    *x25_local_pss_dte;
#endif

extern	char   *x25_default_listen;
extern	char   *x25_default_pid;
#endif

#ifdef	BRIDGE_X25
extern char    *x25_bridge_host;

extern char    *x25_bridge_addr;

extern char    *x25_bridge_discrim;

extern	char   *x25_bridge_listen;
extern	char   *x25_bridge_pid;
#endif

#if defined(BRIDGE_X25) || defined(X25)
extern u_short  x25_bridge_port;
#endif

#ifdef CONS
#ifdef CAMTEC_CCL
extern char    *cons_local_address;
extern char    cons_is_ybts;
extern char    *cons_listen_str;
#endif
#endif


#ifdef	DSAP
extern char	dua_enabled;
extern char    *dua_address;
#endif


void	isodetailor ();
char   *isodefile ();

char   *getlocalhost ();

#endif
