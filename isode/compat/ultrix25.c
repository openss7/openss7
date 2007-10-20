/* ultrix25.c - X.25 abstractions for Ultrix X25 */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/compat/RCS/ultrix25.c,v 9.0 1992/06/16 12:07:00 isode Rel $";
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



/* 
 * $Header: /xtel/isode/isode/compat/RCS/ultrix25.c,v 9.0 1992/06/16 12:07:00 isode Rel $
 *
 * DEMSA X25 contributed by Arnold Krechel, GMD
 *
 * Contributed by George Michaelson, University of Queensland in Australia
 *
 *  -based on the ubcx25.c and sunlink.c modules 
 *   by Julian Onions and John Pavel respectively,
 *
 *   Using the example code for the Ultrix X.25 interface 
 *   written by DEC NAC here in Australia
 *
 *
 * $Log: ultrix25.c,v $
 * Revision 9.0  1992/06/16  12:07:00  isode
 * Release 8.0
 *
 */

/* LINTLIBRARY */

#include <errno.h>
#include <stdio.h>
#include "general.h"
#include "manifest.h"
#include "tailor.h"
#include "tpkt.h"

/*    Ultrix: X25 */

#ifdef  X25
#ifdef  ULTRIX_X25

#ifndef ULTRIX_X25_DEMSA

#include "x25.h"

/* are these needed george? */
#define         X25_MBIT        0x40
#define         X25_QBIT        0x80

/*
 * from examples/socket_incoming.c et al
 */

/*
 * these routines in the Ultrix X.25 library do encoding and decoding
 * of call params. They'll probably be unused, but if I get clever 
 * enough It'd be nice to use them to build up non-standard facilities
 * and other X.25 call stuff.
 *
 * I think they're varargs. should this be ansi-ized at some point???
 */

extern int X25Decode();
extern int X25Encode();

/*
 * Definitions for Call
 */

#define CALLING	0
#define CALLED	1

#define BACKLOG        2
#define MAXMESSAGESIZE   4096

/*
 * global structs used during decoding and encoding of call params.
 * -these are probably way oversize.
 */

static     char	enc_buf[1024];
static     int	enc_buf_len;

/*  */

int     start_x25_client (local)
struct NSAPaddr *local;
{
    int     sd, pgrp;

    if (local != NULLNA)
	local -> na_stack = NA_X25, local -> na_community = ts_comm_x25_default;
    if ((sd = socket (AF_X25, SOCK_SEQPACKET, X25_ACCESS)) == NOTOK) {
	SLOG (compat_log, LLOG_EXCEPTIONS, "failed", ("x25 socket()"));
	return NOTOK; /* Error can be found in errno */
    }

    /*
     * somebody tell me sometime why setting the process group on
     * the socket is such a big deal. This is getting like alchemy
     * with myself doing this 'cos its in the other fellers code...
     * 
     * camtec & ubc does it, sunlink doesn't.
    pgrp = getpid();
    if (ioctl(sd, SIOCSPGRP, &pgrp)) {
	SLOG (compat_log, LLOG_EXCEPTIONS, "failed", ("x25 ioctl(SIOCSPGRP)"));
	return NOTOK; 
    }
     */

    return sd;
}

/*  */

int     start_x25_server (local, backlog, opt1, opt2)
struct NSAPaddr *local;
int     backlog,
	/*
 	 * in Ultrix  X.25 socket functions like turn off/on packet
	 * assembly and call acceptance are done with setsockopt.
	 * it looks like other X.25 socket interfaces are using
	 * "traditional" features like SO_KEEPALIVE, and this
	 * is being passed down in the call from tsap/tsaplisten.c
	 * but I really don't think it applies here.
	 *
	 * thus, the following two arguments are ignored in this
	 * module.
	 */
	opt1,
	opt2;
{
    int     sd, pgrp;
    CONN_DB     zsck;
    CONN_DB     *sck = &zsck;
    sockaddr_x25	addr;

    if ((sd = socket (AF_X25, SOCK_SEQPACKET, X25_ACCESS)) == NOTOK) {
	SLOG (compat_log, LLOG_EXCEPTIONS, "failed", ("x25 socket()"));
	return NOTOK; /* Can't get an X.25 socket */
    }

    /*
     * somebody tell me sometime why setting the process group on
     * the socket is such a big deal. This is getting like alchemy
     * with myself doing this 'cos its in the other fellers code...
     * 
     * camtec & ubc does it, sunlink doesn't.
    pgrp = getpid();
    if (ioctl(sd, SIOCSPGRP, &pgrp)) {
	SLOG (compat_log, LLOG_EXCEPTIONS, "failed", ("x25 ioctl(SIOCSPGRP)"));
	return NOTOK; 
    }
     */

    if (local != NULLNA) {
	local -> na_stack = NA_X25, local -> na_community = ts_comm_x25_default;
	if (local -> na_dtelen == 0) {
	    (void) strcpy (local -> na_dte, x25_local_dte);
	    local -> na_dtelen = strlen(x25_local_dte);
	    if (local -> na_pidlen == 0 && *x25_local_pid)
		local -> na_pidlen =
		    str2sel (x25_local_pid, -1, local -> na_pid, NPSIZE);
	}
    }

    (void) gen2if (local, sck, ADDR_LISTEN);
    /*
     * now munge this into DEC format.
     */
    addr.sx25_family = AF_X25;
    addr.sx25_flags  = 0;
    addr.sx25_namelen = strlen(x25_default_filter);
    strcpy(addr.sx25_name, x25_default_filter);

    if (bind (sd, &addr, sizeof(addr)) == NOTOK) {
	SLOG (compat_log, LLOG_EXCEPTIONS, "failed", ("x25 bind()"));
	(void) close_x25_socket (sd);
	return NOTOK;
    }
    if (listen (sd, backlog) < 0) {
	SLOG (compat_log, LLOG_EXCEPTIONS, "failed", ("x25 listen()"));
	(void) close_x25_socket (sd);
	return NOTOK;
    }
    return sd;
}

/*  */

int     join_x25_client (fd, remote)
int     fd;
struct NSAPaddr *remote;
{
    CONN_DB		zsck;
    CONN_DB		*sck = &zsck;
    sockaddr_x25	filter;
    int			len = sizeof filter;
    int			stat;
    int         	nfd;

    if((nfd = accept (fd, &filter, &len)) == NOTOK) {
	SLOG (compat_log, LLOG_EXCEPTIONS, "failed", ("x25 accept()"));
        if (compat_log -> ll_events & LLOG_EXCEPTIONS)
            (void) log_call_status(fd);      /* decode useful information */
	return NOTOK;
    }
    /*
     * as well as doing a socket level accept, have to accept responsibilty
     * for the X.25 incoming request as well...
     */
    enc_buf_len = sizeof(enc_buf);
    if ((stat = getsockopt(nfd,X25_ACCESS,XSO_TAKECALL,enc_buf,&enc_buf_len) < 0 ) ) {
	SLOG (compat_log, LLOG_EXCEPTIONS, "failed",
              ("Taking inbound X.25 Call"));
        if (compat_log -> ll_events & LLOG_EXCEPTIONS)
            (void) log_call_status(fd);      /* decode useful information */
        return NOTOK;
    }
#ifdef  DEBUG
    if (compat_log -> ll_events & LLOG_DEBUG)
        (void) print_x25_facilities(fd, CALLED, "Effective Called");
#endif
    /*
     * snarf the incoming call details. could permit some local 
     * sanityclaus checks on the X.25 guff but what the hell...
     */

    sck->na_dtelen = sizeof(sck->na_dte);
    if ((stat = X25Decode(enc_buf, enc_buf_len, X25I_CALLINGDTE, sck->na_dte, &(sck->na_dtelen) ) ) <0 ) {
	SLOG (compat_log, LLOG_EXCEPTIONS, "failed",
              ("Getting remote DTE"));
        if (compat_log -> ll_events & LLOG_EXCEPTIONS)
            (void) log_call_status(fd);      /* decode useful information */
        return NOTOK;
    }

    sck->na_cudflen = sizeof(sck->na_cudf);
    if ((stat = X25Decode(enc_buf, enc_buf_len, X25I_USERDATA, sck->na_cudf, &(sck->na_cudflen) ) ) <0 ) {
	SLOG (compat_log, LLOG_EXCEPTIONS, "failed",
              ("Getting remote CUDF"));
        if (compat_log -> ll_events & LLOG_EXCEPTIONS)
            (void) log_call_status(fd);      /* decode useful information */
        return NOTOK;
    }

    (void) if2gen (remote, sck, ADDR_REMOTE);

    /*
     * now send the poor bozo the X.25 acceptance (at last!)
     */
    if (setsockopt(nfd,X25_ACCESS,XSO_ACCEPTCALL,(caddr_t)0, 0) <0 ) {
	SLOG (compat_log, LLOG_EXCEPTIONS, "failed",
              ("Sending Ultrix X.25 Connect Accept"));
        if (compat_log -> ll_events & LLOG_EXCEPTIONS)
            (void) log_call_status(fd);      /* decode useful information */
        return NOTOK;
    }
    return nfd;
}

int     join_x25_server (fd, remote)
int     fd;
struct NSAPaddr *remote;
{
    CONN_DB zsck;
    CONN_DB *sck = &zsck;
    sockaddr_x25 template;

    register int nfd;

    if (remote == NULLNA || remote -> na_stack != NA_X25)
    {
	SLOG (compat_log, LLOG_EXCEPTIONS, "failed",
	      ("Invalid type na%d", remote->na_stack));
	return NOTOK;
    }
    (void) gen2if (remote, sck, ADDR_REMOTE);
    /*
     * now we have to re-map the generic forms of the DTE/CUDF/facil 
     * into DECspeak using the X25Encode() call.
     */
    if ((enc_buf_len = X25Encode (enc_buf, 1024, X25I_NULL)) <0) {
	SLOG (compat_log, LLOG_EXCEPTIONS, "failed",
              ("Initializing Ultrix X.25 Call Template"));
        return NOTOK;
    }
    if ((enc_buf_len = X25Encode (enc_buf, 1024,
              X25I_DTECLASS,
              strlen(x25_default_class), x25_default_class,
	      X25I_CALLEDDTE,
              sck->na_dtelen, sck->na_dte,
              X25I_USERDATA,
              sck->na_cudflen, sck->na_cudf, 
              X25I_NULL)) < 0) {
	SLOG (compat_log, LLOG_EXCEPTIONS, "failed",
              ("Encoding Ultrix X.25 Call Template"));
        return NOTOK;
    }
    if (setsockopt(fd,X25_ACCESS,XSO_SETCONN,enc_buf,enc_buf_len) <0) { 
	SLOG (compat_log, LLOG_EXCEPTIONS, "failed",
              ("Overriding Ultrix X.25 Template Values"));
        return NOTOK;
    }
    template.sx25_family = AF_X25;
    template.sx25_flags  = 0;
    template.sx25_namelen = strlen(x25_default_template);
    strcpy(template.sx25_name, x25_default_template);

    /*
     * poached from sunlink.c
     */
    if ((nfd = connect (fd, &template, sizeof (template))) == NOTOK) {
	SLOG (compat_log, LLOG_EXCEPTIONS, "failed", ("x25 connect()"));
        if (compat_log -> ll_events & LLOG_EXCEPTIONS)
            (void) log_call_status(fd);      /* decode useful information */
	return NOTOK;
   }
#ifdef  DEBUG
    else
        if (compat_log -> ll_events & LLOG_DEBUG)
               (void) log_x25_facilities(fd, CALLING, "Effective Calling");
#endif
    remote = if2gen (remote, sck, ADDR_REMOTE);
    return nfd;
}

close_x25_socket(fd)
int	fd;
{
    struct	X25ClearData	cbuf;
    struct	X25ClearData	*cdata = &cbuf;
    int 			cbl	= sizeof(cbuf);
    int 			stat;

    cdata->clearCause = 0;		/* DTE originated */
    cdata->clearDiagnostic = 0;		/* no additional information */
    cdata->clearDetailsLength = 0;	/* no Clear Details information */

    if ((stat = setsockopt(fd, X25_ACCESS, XSO_CLEARCALL, cdata, cbl)) < 0) {
	if (errno != EACCES)
	    SLOG (compat_log, LLOG_EXCEPTIONS, "failed", 
		("x25 setsockopt(XSO_CLEARCALL)"));
    }

    log_call_status(fd);
    close(fd);
}

log_call_status(fd)
int	fd;
{
	struct	X25PortStatus	sbuf;
	struct	X25PortStatus	*stats = &sbuf;
	int 			sbl	= sizeof(sbuf);
	int			stat;

	/*
	 * get X25PortStatus information
	 */
	if ((stat = getsockopt(fd,X25_ACCESS,XSO_SHOWSTATUS,stats,&sbl) < 0)) {
		SLOG (compat_log, LLOG_EXCEPTIONS, "failed", 
			("x25 getsockopt(XSO_SHOWSTATUS)"));
		return; 
	}
	switch (stats->portState) {
	case	X25S_OPEN:
		SLOG (compat_log, LLOG_DEBUG, NULLCP, 
			("X25S_OPEN: No connection is in Progress."));
		log_call_clear(fd, 0);
		break;

	case	X25S_CLEARED:
		SLOG (compat_log, LLOG_DEBUG, NULLCP, 
			("X25S_CLEARED: The connection has been cleared."));
		log_call_clear(fd, 0);
		break;

	case	X25S_RUNNING:
#ifdef	DEBUG
		SLOG (compat_log, LLOG_DEBUG, NULLCP, 
			("X25S_RUNNING: The connection is still open."));
		log_call_clear(fd, 0);
#endif	DEBUG
		break;

	case X25S_CALLING:         /* Connection in progress       */
		SLOG (compat_log, LLOG_DEBUG, NULLCP, 
			("X25S_CALLING: Connection in progress."));
		log_call_clear(fd, 0);
		break;

	case X25S_CALLED:          /* Call received and taken      */
		SLOG (compat_log, LLOG_DEBUG, NULLCP, 
			("X25S_CALLED: Call received and taken."));
		log_call_clear(fd, 0);
		break;

	case X25S_SYNC:            /* Unconfirmed user reset       */
		SLOG (compat_log, LLOG_DEBUG, NULLCP, 
			("X25S_SYNC: Unconfirmed user reset."));
		log_call_clear(fd, 0);
		break;

	case X25S_UNSYNC:          /* Unconfirmed reset indic      */
		SLOG (compat_log, LLOG_DEBUG, NULLCP, 
			("X25S_UNSYNC: Unconfirmed reset indication."));
		log_call_clear(fd, 0);
		break;

	case X25S_CLEARING:        /* User requested clearing      */
		SLOG (compat_log, LLOG_DEBUG, NULLCP, 
			("X25S_CLEARING: User requested clearing."));
		log_call_clear(fd, 0);
		break;

	case X25S_NOCOMM:          /* No communication with net    */
		SLOG (compat_log, LLOG_DEBUG, NULLCP, 
			("X25S_NOCOMM: No communication with net."));
		log_call_clear(fd, 0);
		break;

	case X25S_CLEARBYDIR:     /* Cleared by directive         */
		SLOG (compat_log, LLOG_DEBUG, NULLCP, 
			("X25S_CLEARBYDIR: Cleared by directive."));
		log_call_clear(fd, 0);
		break;

	case X25S_INCALL:          /* Untaken incoming call        */
		SLOG (compat_log, LLOG_DEBUG, NULLCP, 
			("X25S_INCALL: Untaken incoming call."));
		log_call_clear(fd, 0);
		break;

	default:
		SLOG (compat_log, LLOG_EXCEPTIONS, NULLCP, 
			("unknown return from getsockopt(XSO_SHOWSTATUS)= %d [see /usr/include/netx25/x25.h]", stats->portState));
		SLOG (compat_log, LLOG_DEBUG, NULLCP, 
			("restricted = %d", stats->restrictedInterface));
		SLOG (compat_log, LLOG_DEBUG, NULLCP, 
			("Int Msg Size = %d", stats->interruptMessageSize));
		SLOG (compat_log, LLOG_DEBUG, NULLCP, 
			("Data Msg Size in = %d", stats->dataMessageSizeIn));
		SLOG (compat_log, LLOG_DEBUG, NULLCP, 
			("Data Msg Size out = %d", stats->dataMessageSizeOut));
		SLOG (compat_log, LLOG_DEBUG, NULLCP, 
			("Error BitMap = %2x", stats->errorBitMap));
	}
	return;
}

log_call_clear(fd, type)
int	fd;
int	type;
{
	struct	X25ClearData	cbuf;
	struct	X25ClearData	*cdata = &cbuf;
	int 			cbl	= sizeof(cbuf);
	int			stat;
	int			flags;
	unsigned char		buf[2];
	extern	void		elucidate_x25_err();
	char			dbuf[128];
	int			dlen = sizeof(dbuf);

	/*
	 * get X25ClearData information
	 */
	if ((stat = getsockopt(fd,X25_ACCESS,XSO_CLEARDATA,cdata,&cbl) < 0) &&
	     errno != ENOMSG) {
		SLOG (compat_log, LLOG_EXCEPTIONS, "failed", 
			("x25 getsockopt(XSO_CLEARDATA)"));
		return; 
	}
	if (errno == ENOMSG)
		return;
	/*
	 * set up argbuf to call elucidate_x25_err()
	 */
	flags = 0;
	flags = 1 << RECV_DIAG;		/* we have diagnostics */
	if (type == 0)			/*  diag type (clear/reset) */
	    flags |= 1 << DIAG_TYPE;	/* we have call clear diagnostics */
	buf[0] = cdata->clearCause;
	buf[1] = cdata->clearDiagnostic;
	elucidate_x25_err(flags, buf);

	SLOG (compat_log, LLOG_EXCEPTIONS, NULLCP, 
		("[Clear origin was %s]", 
			cdata->clearOrigin == X25R_ORIGINREMOTE ? "remote" :
			 cdata->clearOrigin == X25R_ORIGINLOCAL ? "local" :
			  "unknown" ));
	SLOG (compat_log, LLOG_EXCEPTIONS, NULLCP, 
		("and %d bytes of info", cdata->clearDetailsLength));

	dlen = sizeof(dbuf);
	if ((stat = X25Decode(cdata->clearDetails, 
			      cdata->clearDetailsLength, 
			      X25I_CHARGEMON, 
			      dbuf, &dlen) ) <0 ) {
	    SLOG (compat_log, LLOG_EXCEPTIONS, "failed",
       		  ("X25Decode(X25I_CHARGEMON)"));
	    return;
    	}
	if (stat > 0) 
	    SLOG (compat_log, LLOG_EXCEPTIONS, NULLCP, 
		("%*s Money Units", stat, dlen, dlen, dbuf));
	dlen = sizeof(dbuf);
	if ((stat = X25Decode(cdata->clearDetails, 
			      cdata->clearDetailsLength, 
			      X25I_CHARGESEG, 
			      dbuf, &dlen) ) <0 ) {
	    SLOG (compat_log, LLOG_EXCEPTIONS, "failed",
       		  ("X25Decode(X25I_CHARGESEG)"));
	    return;
    	}
	if (stat > 0) 
	    SLOG (compat_log, LLOG_EXCEPTIONS, NULLCP, 
		("%*s Segments", dlen, dbuf));
	dlen = sizeof(dbuf);
	if ((stat = X25Decode(cdata->clearDetails, 
			      cdata->clearDetailsLength, 
			      X25I_CHARGETIME, 
			      dbuf, &dlen) ) <0 ) {
	    SLOG (compat_log, LLOG_EXCEPTIONS, "failed",
       		  ("X25Decode(X25I_CHARGETIME)"));
	    return;
    	}
	if (stat > 0) 
	    SLOG (compat_log, LLOG_EXCEPTIONS, NULLCP, 
		("%*s Time Units", dlen, dbuf));
	dlen = sizeof(dbuf);
	if ((stat = X25Decode(cdata->clearDetails, 
			      cdata->clearDetailsLength, 
			      X25I_USERDATA, 
			      dbuf, &dlen) ) <0 ) {
	    SLOG (compat_log, LLOG_EXCEPTIONS, "failed",
       		  ("X25Decode(X25I_USERDATA)"));
	    return;
    	}
	if (stat > 0) 
	    SLOG (compat_log, LLOG_EXCEPTIONS, NULLCP, 
		("%d Bytes User Data", dlen));
	return;
}


#ifdef  DEBUG

static int  log_x25_facilities (fd, coc, caption)
int     fd;
int     coc;
char   *caption;
{
    int	stat;

    enc_buf_len = sizeof(enc_buf);
    if (coc == CALLING) {
        if ((stat = getsockopt(fd,
			       X25_ACCESS,
			       XSO_ACCEPTDETAILS,
			       enc_buf,&enc_buf_len) < 0 ) ) {
	    SLOG (compat_log, LLOG_EXCEPTIONS, "failed", 
			("getsockopt(XSO_ACCEPTDETAILS)"));
    	    return NOTOK;
        }
    }

    print_x25_facilities (fd, coc, caption);

    return OK;
}

/*  */

void	*
epl_prtstr (fmt, val, vallen)
char	*fmt;
char	*val;
int	vallen;
{
	static char	abuf[128];
	static char	tbuf[128];
	char	*c, *d;

	abuf[0] = 0;
	if (vallen > 0) {
	    for (c = val, d = abuf; vallen; c++, vallen--) {
		if (!isprint(*c)) {
		    (void) sprintf(d, " 0x%02x ", *c & 0xff);
		    d += 6;
		} else {
		    (void) sprintf(d, "%c", *c);
		    d++;
		}
	    }
	    *d = 0;
	}
	(void) sprintf(tbuf, fmt, abuf);
	return tbuf;
}

void	*
epl_prtbool (fmt, val, vallen)
char	*fmt;
short	*val;
int	vallen;
{
	static char	*true = "true";
	static char	*false = "false";

	if (*val == 0)
		return (true);
	else
		return (false);
}

void	*
epl_prtint (fmt, val, vallen)
char	*fmt;
short	*val;
int	vallen;
{
	static char	tbuf[128];

	(void) sprintf(tbuf, fmt, *val);
	return tbuf;
}

void	*
epl_prtlst (fmt, val, vallen)
char	*fmt;
short	*val;
int	vallen;
{
	static char	*list = "[LIST]";

	return list;
}

static struct {
	short	code;
	char	type;
#define	EPL_STR		0
#define	EPL_BOOL	1
#define	EPL_INT		2
#define	EPL_LIST	3
	char	*fmt;
} epl_tab[] = {
    X25I_CALLEDEXTISO,	     EPL_STR,	"Address ext for dest (ISO): %s",
    X25I_CALLEDEXTNONISO,    EPL_STR,	"Non-ISO format: %s",
    X25I_CALLINGEXTISO,	     EPL_STR,	"Address ext for target (ISO): %s",
    X25I_CALLINGEXTNONISO,   EPL_STR,	"Non-ISO format: %s",
    X25I_CHARGEMON,	     EPL_STR,	"Call charge in monetary units: %s",
    X25I_CHARGESEG,	     EPL_STR,	"Call charge in segment counts: %s",
    X25I_CHARGETIME,	     EPL_STR,	"Call charge in elapsed time: %s",
    X25I_CHARGEINFO,	     EPL_BOOL,	"Charging information request: %s",
    X25I_CUG,	 	     EPL_STR,	"Closed User Group: %s",
    X25I_ETETRANSITDELAY,    EPL_LIST,	"End-to-end transit delay request: %s",
    X25I_EXPEDITE,	     EPL_BOOL,	"Interrupts allowed: %s",
    X25I_NOEXPEDITE,	     EPL_BOOL,	"Interrupts not allowed: %s",
    X25I_FASTSELECT,	     EPL_BOOL,	"Fast select facility: %s",
    X25I_FASTSELECTRESTR,    EPL_BOOL,	"Fast select restricted response : %s",
    X25I_NONX25FACILITIES,   EPL_STR,	"Non-X.25 facilities: %s",
    X25I_CALLINGSUBADDR,     EPL_STR,	"Calling DTE subaddress: %s",
    X25I_MINTHRUCLSIN,	     EPL_INT,	"Minimum throughput class incoming: %d",
    X25I_MINTHRUCLSOUT,	     EPL_INT,	"Minimum throughput class outgoing: %d",
    X25I_NETUSERID,	     EPL_STR,	"Network-specific user ID: %s",
    X25I_NSAPMAP,	     EPL_BOOL,	"NSAP mapping to DTE: %s",
    X25I_PKTSIZEIN,	     EPL_INT,	"Requested incoming packet size: %d",
    X25I_PKTSIZEOUT,	     EPL_INT,	"Requested outgoing packet size: %d",
    X25I_PRIORITY,	     EPL_STR,	"Connection priority: %s",
    X25I_PROTECTION,	     EPL_STR,	"Protection: %s",
    X25I_CALLINGDTE,	     EPL_STR,	"Calling DTE address: %s",
    X25I_RPOA,	 	     EPL_LIST,	"Specify how call is to be routed: %s",
    X25I_THRUCLSIN,	     EPL_INT,	"Maximum incoming data rate for VC: %d",
    X25I_THRUCLSOUT,	     EPL_INT,	"Maximum outgoing data rate for VC: %d",
    X25I_TRANSITDELAY,	     EPL_INT,	"Actual transit delay for our call: %d",
    X25I_USERDATA,	     EPL_STR,	"User data: %s",
    X25I_WINSIZEIN,	     EPL_INT,	"Window size for incoming call: %d",
    X25I_WINSIZEOUT,	     EPL_INT,	"Window size for outgoing call: %d",
    X25I_DTECLASS,	     EPL_STR,	"DTE class name: %s",
    X25I_TEMPLATE,	     EPL_STR,	"Template for XSO_ACCEPTCALL: %s",
    X25I_BUFFPREALLOC,	     EPL_BOOL,	"Buffer pre-allocation by gateway: %s",
    X25I_CALLEDDTE,	     EPL_STR, 	"Requested DTE address: %s",
    X25I_LOCALDTE,	     EPL_STR,	"DTE receiving incoming call: %s",
    X25I_REDIRECTREASON,     EPL_INT,	"Reason for call redirection: %d",
    0,	 0, 0,
};

print_x25_facilities (fd, coc, caption)
int	fd;
int     coc;
char   *caption;
{
    int		numitems,stat,baud,i,j;
    char	cbuf[128];
    int		cbl = sizeof(cbuf);
    char	*cptr = cbuf;
    short	lbuf[128];
    int		lbl;

    DLOG (compat_log, LLOG_DEBUG, ("%s X.25 Facilities:", caption));

    lbl = sizeof(lbuf);
    if ((numitems = X25GetItemList(enc_buf, enc_buf_len, lbuf, &lbl)) < 0) {
	SLOG (compat_log, LLOG_EXCEPTIONS, "failed",
              ("Getting Returned Facilities List"));
    }
    for (i=0;i < numitems; i++) {
        cbl = sizeof(cbuf);
        if ((stat = X25Decode(enc_buf, enc_buf_len, 
			  lbuf[i], cbuf, &cbl)) < 0) {
	    SLOG (compat_log, LLOG_EXCEPTIONS, "failed",
              ("Getting Facility [%d] = X25I_%d", i, lbuf[i]));
        }
	if (stat > 0)  {
	char	*tptr;
	    for(j=0; epl_tab[j].code != 0 && epl_tab[j].code != lbuf[i]; j++);
	    if (epl_tab[j].code == 0)
                DLOG (compat_log, LLOG_DEBUG, ("unknown facility %d", lbuf[i]));
	    else {
	        switch (epl_tab[j].type) {
	    	case EPL_STR:
		    tptr = epl_prtstr((epl_tab[j].fmt), cptr, cbl);
		    DLOG (compat_log, LLOG_DEBUG, ("%s", tptr));
		    break;
		case EPL_BOOL:
		    tptr = epl_prtbool((epl_tab[j].fmt), (short *)cptr, cbl);
		    DLOG (compat_log, LLOG_DEBUG, ("%s", tptr));
		    break;
		case EPL_INT:
		    tptr = epl_prtint((epl_tab[j].fmt), (short *)cptr, cbl);
		    DLOG (compat_log, LLOG_DEBUG, ("%s", tptr));
		    break;
		case EPL_LIST:
		    tptr = epl_prtlst((epl_tab[j].fmt), (short *)cptr, cbl);
		    DLOG (compat_log, LLOG_DEBUG, ("%s", tptr));
		    break;
		default:
                    DLOG (compat_log, LLOG_DEBUG, 
			  ("unknown type of EPL %d", epl_tab[j].code));
		    break;
	        }
	    }
        }
    }
    return OK;
}
#endif

#else 	/* ULTRIX_X25_DEMSA */

#include <x25.h>
#include <demsa.h>

#include <isoaddrs.h>
#include <logger.h>
#include "/usr/include/x25.h"

extern char * demsa_local_subaddr;
extern char * decnet_destination;

extern t_list_of_conn_desc list_of_conn_desc[our_TABLE_SIZE];

#ifdef ULTRIX_X25_DEMSA_SPAWN_TSAPD
/* ULTRIX_X25_DEMSA_SPAWN_TSAPD  should be removed */
CONN_DB x25_interface;
CONN_DB * p_x25_interface = &x25_interface;
#endif 

/*    4.[23] UNIX: DEC X25 */

#include <sys/uio.h>

#define         X25_MBIT        0x40
#define         X25_QBIT        0x80

#define MAX_RETRY 5

#ifdef DEBUG
char our_buffer[256];
#endif             

#define	TRUE	(1)
#define FALSE	(0)

char * DELIMITER = ":";

X25vc our_get_vci(sd,s)
/* this routine returns NOTOK if it detects an error    */
/* possible errors: 1. sd has not been registered       */
/*                  1. sd doesn't correspond to X25vci  */
int  sd;
char *s;
{
   if( sd >= our_TABLE_SIZE ) {
       SLOG (compat_log, LLOG_EXCEPTIONS, s,
             ("ultrix25.c:sd >= our_TABLE_SIZE"));
       return NOTOK;
   }
   if( sd < NULL ) {
       SLOG (compat_log, LLOG_EXCEPTIONS, s,
             ("ultrix25.c:sd < NULL"));
       return NOTOK;
   }
   if (list_of_conn_desc[sd].TCP_IP_OR_X25_SOCKET==our_X25_SOCKET) {
       return list_of_conn_desc[sd].descriptor;
   }
   else
   {
     SLOG (compat_log, LLOG_EXCEPTIONS, s,
     ("ultrix25.c:list_of_conn_desc[sd].TCP_IP_OR_X25_SOCKET=!our_X25_SOCKET"));
     return NOTOK;
   }
}

compose_text(xudatap,pid,cudf)
char *  xudatap;
char *  pid;
char *  cudf;
{
     strcpy(xudatap,pid);
     strcat(xudatap,DELIMITER);
     strcat(xudatap,cudf);
}


char * Kloppenburgs_x25_ErrorCause( what, vci)
/* only called after clear and reset events */
/* adapted from kloppenburgs's ean x25 access module */
/* rts/netx25/x25int.c */
    int			what;
    X25vc		vci;
  {
    unsigned char	cause[2];
    int			rtn;
    char		msg[6];
    char*		cause_msg;
    char                result[100];

    if ((rtn = X25Info(vci,X25I_Cause,&cause[0],1)) > 0)
	(void) sprintf(msg,"%02x",(int)cause[0]);
    else
      {
	strcpy(msg,"??");
	cause[0] = '?';
      }
    if ((rtn = X25Info(vci,X25I_Diagnostic,&cause[1],1)) > 0)
	(void) sprintf(&msg[2]," %02x",(int)cause[1]);
    if (what == X25_Clear)
      {
	switch ((int)cause[0])
	  {
	    case 000: cause_msg = "DTE originated";		break;
	    case 001: cause_msg = "Number busy";		break;
	    case 011: cause_msg = "Out of order";		break;
	    case 021: cause_msg = "Remote procedure error";	break;
	    case 031: cause_msg = "Reverse charging acceptance not subscribed";
								break;
	    case 041: cause_msg = "Incompatible destination";	break;
	    case 051: cause_msg = "Fast select acceptance not subscribed";
								break;
	    case 003: cause_msg = "Invalid facility request";	break;
	    case 013: cause_msg = "Access barred";		break;
	    case 023: cause_msg = "Local procedure error";	break;
            case 005: cause_msg = "Network congestion";		break;
	    case 015: cause_msg = "Not obtainable";		break;
	    default:  cause_msg = "?";				break;
	  }
      }
    else if (what == X25_Reset)
      {
	switch ((int)cause[0])
	  {
	    case 000: cause_msg = "DTE originated";		break;
	    case 001: cause_msg = "Out of order (PVC)";		break;
	    case 003: cause_msg = "Remote procedure error";	break;
	    case 005: cause_msg = "Local procedure error";	break;
	    case 007: cause_msg = "Network congestion";		break;
	    case 011: cause_msg = "Remote DTE operational (PVC)";	break;
	    case 017: cause_msg = "Network operational (PVC)";	break;
	    case 021: cause_msg = "Incompatible destination";	break;
	    default:  cause_msg = "?";				break;
	  }
      }

    compose_text(result,msg,cause_msg);

    return result;

}


int check_x25_event(event_expected,event_received,vci)
X25vc vci;
int event_expected;
int event_received;
{

switch (event_received)
{
 case X25_Data:
 DLOG (compat_log,LLOG_DEBUG,("check_x25_event: complete unqualified packet sequence"));
 break;

 case X25_DataM:
 DLOG (compat_log,LLOG_DEBUG,("check_x25_event: incomplete unqualified packet sequence"));
 break;

 case X25_QData:
 DLOG (compat_log,LLOG_DEBUG,("check_x25_event: complete qualified packet sequence"));
 break;

 case X25_QDataM:
 DLOG (compat_log,LLOG_DEBUG,("check_x25_event: incomplete qualified packet sequence"));
 break;

 case X25_Reset:
 DLOG (compat_log,LLOG_DEBUG,("check_x25_event: a reset indication packet was received"));
 SLOG (compat_log,LLOG_EXCEPTIONS,
       CAT("check_x25_event: ",Kloppenburgs_x25_ErrorCause( X25_Reset, vci)),
       ("X25_Reset_Event"));
 break;

 case X25_ResetConf:
 DLOG (compat_log,LLOG_DEBUG,("check_x25_event: a previous reset request is acknowlewded"));
 break;

 case X25_Interrupt:
 DLOG (compat_log,LLOG_DEBUG,("check_x25_event: urgent data was received"));
 break;

 case X25_InterruptConf:
 DLOG (compat_log,LLOG_DEBUG,("check_x25_event: a previous interrupt request is confirmed"));
 break;

 case X25_Clear:
 DLOG (compat_log,LLOG_DEBUG,("check_x25_event: a clear indication packet was received"));
 SLOG (compat_log,LLOG_EXCEPTIONS,
       CAT("check_x25_event: ",Kloppenburgs_x25_ErrorCause( X25_Clear, vci)),
       ("X25_Clear_Event"));
 break;

 case X25_ClearConf:
 DLOG (compat_log,LLOG_DEBUG,("check_x25_event: a previous clear request is acknowledged"));
 break;

 case X25_Accepted:
 DLOG (compat_log,LLOG_DEBUG,("check_x25_event: an outstanding call accepted after X25Call"));
 break;

 case X25_CallAvail:
 DLOG (compat_log,LLOG_DEBUG,("check_x25_event: incoming call after X25Listen"));
 break;

 case X25_Call:
 DLOG (compat_log,LLOG_DEBUG,("check_x25_event: incoming call after X25TakeCall"));
 break;

 case X25_CallFail:
 DLOG (compat_log,LLOG_DEBUG,("check_x25_event: failure of VC"));
 break;
}
if (event_expected == event_received)
{
   return TRUE;
}
else
{
   return FALSE;
}

}

#ifdef ULTRIX_X25_DEMSA_SPAWN_TSAPD

/* an incoming call will spawn the process who accepts the call */
/* you will have to change the listeners (DAEMONS)              */

int     start_x25_client (local)
struct NSAPaddr *local;
{
    int     error;
    int     our_sd;
    int     our_count;

    X25vc   vci;
    X25vc   * vcip = &vci;

#ifdef DEBUG
    DLOG (compat_log,LLOG_DEBUG,("--> start_x25_client"));
#endif       

    if ((our_sd = socket (AF_INET, SOCK_STREAM, 0)) == NOTOK) {
        SLOG (compat_log, LLOG_EXCEPTIONS, "failed", ("socket-call"));
        return NOTOK;
    }
    close(our_sd);

    if (local != NULLNA)
  	local->na_stack = NA_X25, local->na_community=ts_comm_x25_default;
    
    error = X25Init(vcip,(unsigned int) NULL);

    if (error < 0) {
       SLOG (compat_log, LLOG_EXCEPTIONS,
       CAT("start_x25_client: ", X25ErrorText(vci,error)), ("X25Init"));
       return NOTOK;
    }

#ifdef DEBUG
    (void) sprintf(our_buffer," vci after start_x25_client  %d ",(int) vci);
    DLOG (compat_log,LLOG_DEBUG,(our_buffer));
#endif       

   if ( X25RegisterVC(vci,rhandler,whandler,our_sd) < NULL ) {
	SLOG (compat_log, LLOG_EXCEPTIONS, "failed", ("X25RegisterVC"));
	return NOTOK;
   }
   if(our_sd>=our_TABLE_SIZE || (our_count=getdtablesize())>=our_TABLE_SIZE){
       SLOG (compat_log, LLOG_EXCEPTIONS, "invalid",
             ("getdtablesize:increase our_TABLE_SIZE in demsb.h"));
      return NOTOK;
    }
    list_of_conn_desc[our_sd].TCP_IP_OR_X25_SOCKET = our_X25_SOCKET;
    list_of_conn_desc[our_sd].descriptor           = vci;

#ifdef DEBUG
    DLOG (compat_log,LLOG_DEBUG,("<--start_x25_client"));
#endif       

  return our_sd;

}

/*  */

int     start_x25_server (local, backlog, opt1, opt2)
struct NSAPaddr *local;
int     backlog,
	opt1,
	opt2;
{
    X25vc   vci;
    X25vc * vcip = &vci;

    int     pgrp;
    int     error;
    int     event;

    int     our_sd;
    int     our_count;

#ifdef DEBUG
    DLOG (compat_log,LLOG_DEBUG,("--> start_x25_server"));
#endif       

    if ((our_sd = socket (AF_INET, SOCK_STREAM, 0)) == NOTOK) {
        SLOG (compat_log, LLOG_EXCEPTIONS, "failed", ("socket-call"));
        return NOTOK;
    }
    close(our_sd);

#ifdef	notyet
#ifdef	BSD43
    int	    onoff;
#endif
#endif

    	if (local != NULLNA) {
	local -> na_stack = NA_X25, local -> na_community = ts_comm_x25_default;
	if (local->na_dtelen == 0) {
	    (void) strcpy (local->na_dte, x25_local_dte);
	    local->na_dtelen = strlen(x25_local_dte);
	    if (local->na_pidlen == 0 && *x25_local_pid)
		local->na_pidlen =
		    str2sel (x25_local_pid, -1, local->na_pid, NPSIZE);
	}
        }


#ifdef DEBUG
    DLOG (compat_log,LLOG_DEBUG,("start_x25_server: X25TakeCall"));
#endif

    error = X25TakeCall(X25_InitCall,vcip);

#ifdef DEBUG
    (void) sprintf(our_buffer," return of X25TakeCall: %d ",error);
    DLOG (compat_log,LLOG_DEBUG,(our_buffer));
    (void) sprintf(our_buffer," vci in start_x25_server  %d ",(int) vci);
    DLOG (compat_log,LLOG_DEBUG,(our_buffer));
#endif       

    if (error < NULL) {
      SLOG (compat_log,LLOG_EXCEPTIONS,
      CAT("start_x25_server:",X25ErrorText(vci,error)),("X25TakeCall"));
      switch (error) { 
        case X25E_BADVCI:
            DLOG (compat_log,LLOG_DEBUG,("BADVCI"));
            break;
        case X25E_NO_LISTEN:
            DLOG (compat_log,LLOG_DEBUG,("NOLISTEN"));
            break;
        case X25E_SOCKOPT:
            DLOG (compat_log,LLOG_DEBUG,("SOCKOPT"));
            break;
        case X25E_ACCEPT:
            DLOG (compat_log,LLOG_DEBUG,("ACCEPT"));
            break;
        case X25E_NO_BUF:
            DLOG (compat_log,LLOG_DEBUG,("NO_BUF"));
            break;
        case X25E_IOCTL:
            DLOG (compat_log,LLOG_DEBUG,("IOCTL"));
            break;
        case  X25E_NOSLOT:
            DLOG (compat_log,LLOG_DEBUG,("NOSLOT"));
            break;
        default:
            break;
    }
#ifdef DEBUG
    DLOG (compat_log,LLOG_DEBUG,("<--1 start_x25_server"));
#endif       
    return NOTOK;
    }

#ifdef DEBUG
    DLOG (compat_log,LLOG_DEBUG,(" x25_start_server: X25ReadEvent"));
#endif

    X25ReadEvent(vci, (char*) NULL, (int) NULL, &event );

if (check_x25_event(X25_Call,event,vci)) 
{
	/* get remote address */

#ifdef DEBUG
       DLOG(compat_log,LLOG_DEBUG,("start_x25_server:X25Info(remote_address)"));
#endif       

	bzero(x25_interface.x25_dte,X25_DTELEN + 1);
	x25_interface.x25_dte[X25_DTELEN]= '\0';
       	error = X25Info(vci,X25I_Remote,x25_interface.x25_dte,X25_DTELEN);

       	if (error < 0) { 
          SLOG (compat_log,LLOG_EXCEPTIONS,
               CAT("start_x25_server:", X25ErrorText(vci,error)),
                  ("X25InfoRemoteAdr"));
#ifdef DEBUG
          DLOG (compat_log,LLOG_DEBUG,("<--2 start_x25_server"));
#endif       
          return NOTOK;
        } 
        else {
#ifdef DEBUG
      	   DLOG (compat_log,LLOG_DEBUG,(x25_interface.x25_dte));
#endif       
           x25_interface.x25_dtelen = error;
        }

        /* get user data field */

#ifdef DEBUG
     DLOG(compat_log,LLOG_DEBUG,("start_x25_server:X25Info(user_data_fields)"));
#endif       

       	bzero(x25_interface.x25_cudf,X25_CUDFSIZE+1);       
       	x25_interface.x25_cudf[X25_CUDFSIZE]='\0';       
       	error = X25Info(vci,X25I_UserData,x25_interface.x25_cudf,X25_CUDFSIZE);

       	if (error < 0) { 
          SLOG (compat_log,LLOG_EXCEPTIONS,
               CAT("start_x25_server: ", X25ErrorText(vci,error)),
                  ("X25InfoCallUserData"));
#ifdef DEBUG
          DLOG (compat_log,LLOG_DEBUG,("<--3 start_x25_server"));
#endif       
          return NOTOK;
   	}
        else {
      	   DLOG (compat_log,LLOG_DEBUG,(x25_interface.x25_cudf));
           x25_interface.x25_cudflen = error;
        }
}	/* if check-Ende ! */
  else {
 	DLOG (compat_log,LLOG_DEBUG,("start_x25_server: X25Clear "));

        error=X25Clear(vci,(unsigned char)"\0",(unsigned char *) NULL, 
                       (int) NULL, (unsigned char *) NULL, (int) NULL );

       if (error < 0) { 
           SLOG (compat_log,LLOG_EXCEPTIONS,
                 CAT("start_x25_server: ",X25ErrorText(vci,error)),
                 ("X25Clear"));
           DLOG(compat_log,LLOG_DEBUG,("<--4 start_x25_server with error"));
           return NOTOK;
        }
     }

#ifdef	notyet		/* not sure if these are supported... */
#ifndef	BSD43
#else
    onoff = 1;
#endif

    DLOG (compat_log,LLOG_DEBUG,("start_x25_server: X25Option 1"));

    if (opt1) error = X25Option(vci,(unsigned int) opt1);
    if (error < 0) { 
      SLOG (compat_log,LLOG_EXCEPTIONS,
      CAT("start_x25_server:",X25ErrorText(vci,error)),("X25Option1"));
      DLOG (compat_log,LLOG_DEBUG,("<--5 start_x25_server with error"));
      return NOTOK;
    }

    DLOG (compat_log,LLOG_DEBUG,("X25_start_server: X25Option 2"));

    if (opt2) error = X25Option(vci,(unsigned int) opt2);
    if (error < 0) { 
      SLOG (compat_log,LLOG_EXCEPTIONS,
      CAT("start_x25_server: ",X25ErrorText(vci,error)),("X25Option2"));
      DLOG (compat_log,LLOG_DEBUG,("<--6 start_x25_server with error"));
      return NOTOK;
    }
#endif

   DLOG (compat_log,LLOG_DEBUG,("ACCEPT IN start_x25_server"));

   error =  X25Accept( vci, (unsigned char *) NULL, (int) NULL,
                       (unsigned char *) NULL, (int) NULL ); 

   if (error < 0) {
      SLOG (compat_log,LLOG_EXCEPTIONS,
      CAT("start_x25_server: ",X25ErrorText(vci,error)),("X25Accept"));
      DLOG (compat_log,LLOG_DEBUG,("<--7 start_x25_server with error"));
      return NOTOK;
   }

  DLOG (compat_log,LLOG_DEBUG,("ACCEPT succeded in start_x25_server"));

#ifdef DEBUG
    (void) sprintf(our_buffer," vci after start_x25_server  %d ",(int) vci);
    DLOG (compat_log,LLOG_DEBUG,(our_buffer));
    (void) sprintf(our_buffer," vci after start_x25_server  %d ",our_sd);
    DLOG (compat_log,LLOG_DEBUG,(our_buffer));
#endif       

   if ( X25RegisterVC(vci,rhandler,whandler,our_sd) < NULL ) {
	SLOG (compat_log, LLOG_EXCEPTIONS, "failed", ("X25RegisterVC"));
	return NOTOK;
   }
   if(our_sd>=our_TABLE_SIZE || (our_count=getdtablesize())>=our_TABLE_SIZE){
       SLOG (compat_log, LLOG_EXCEPTIONS, "invalid",
              ("getdtablesize:increase our_TABLE_SIZE in demsb.h"));
      return NOTOK;
    }
    list_of_conn_desc[our_sd].TCP_IP_OR_X25_SOCKET = our_X25_SOCKET;
    list_of_conn_desc[our_sd].descriptor           = vci;

  DLOG (compat_log,LLOG_DEBUG,("<-- start_x25_server normally"));

  return our_sd;

}

/*  */


int    join_x25_client (sd, rremote)
int    sd;
struct NSAPaddr *rremote;
{
   int error;
   X25vc  vci; 

   DLOG (compat_log,LLOG_DEBUG,("--> join_x25_client"));

   vci =  our_get_vci(sd,"join_x25_client");

#ifdef DEBUG
    (void) sprintf(our_buffer," vci in join_x25_client  %d ",(int) vci);
    DLOG (compat_log,LLOG_DEBUG,(our_buffer));
    (void) sprintf(our_buffer," sd  in join_x25_client  %d ",(int) sd);
    DLOG (compat_log,LLOG_DEBUG,(our_buffer));
#endif       

/* fill call user data field and pid as received in start_x25_server */
/* - ugly but remote was no available in start_x25_server            */

     (void) if2gen (rremote, p_x25_interface, NULL);

     DLOG (compat_log,LLOG_DEBUG,("<-- join_x25_client"));

     return sd;

}

int     join_x25_server (sd, rremote)
int    sd;
struct NSAPaddr *rremote;
{
 
    X25vc   vci;

    int error;
    int event;
    int retry = 0;

    DLOG (compat_log,LLOG_DEBUG,("--> join_x25_server"));

    vci =  our_get_vci(sd,"join_x25_server");

#ifdef DEBUG
    (void) sprintf(our_buffer," vci join_x25_server  %d ",(int) vci);
    DLOG (compat_log,LLOG_DEBUG,(our_buffer));
    (void) sprintf(our_buffer," sd  join_x25_server  %d ",(int) sd );
    DLOG (compat_log,LLOG_DEBUG,(our_buffer));
#endif       

      if (rremote == NULLNA || rremote -> na_stack != NA_X25)
    {
	SLOG (compat_log, LLOG_EXCEPTIONS, NULLCP,
  	      ("Invalid type na%d", rremote->na_stack));
        DLOG (compat_log,LLOG_DEBUG,("<--1 join_x25_server"));
	return NOTOK;
    }

loop:

    retry++;
  
    DLOG (compat_log,LLOG_DEBUG,("join_x25_server: X25Call"));

#ifdef DEBUG

bcopy( rremote->na_dte, our_buffer, (int) rremote->na_dtelen);
(void) sprintf(&our_buffer[(int) rremote->na_dtelen],"=remote dte in join_x25_server");
DLOG (compat_log,LLOG_DEBUG,(our_buffer));

bcopy( rremote->na_fac, our_buffer, (int) rremote->na_faclen);
(void) sprintf(&our_buffer[(int) rremote->na_faclen],"=remote fac in join_x25_server");
DLOG (compat_log,LLOG_DEBUG,(our_buffer));

bcopy( rremote->na_cudf, our_buffer, (int) rremote->na_cudflen);
(void) sprintf(&our_buffer[(int)rremote->na_cudflen],"=remote cud in join_x25_server");
DLOG (compat_log,LLOG_DEBUG,(our_buffer));


bcopy( rremote->na_pid, our_buffer, (int) rremote->na_pidlen);
(void) sprintf(&our_buffer[(int) rremote->na_pidlen],"=remote pid in join_x25_server");
DLOG (compat_log,LLOG_DEBUG,(our_buffer));

#endif       

    x25_interface.x25_cudf[X25_CUDFSIZE]='\0';
    x25_interface.x25_dte[X25_DTELEN]= '\0';

    (void) gen2if (rremote, p_x25_interface, NULL);

    error = X25Call(vci,rremote->na_dte,demsa_local_subaddr,
                        rremote->na_fac,  (int) rremote->na_faclen,
                        x25_interface.x25_cudf,
                        (int) x25_interface.x25_cudflen, NULL );


    if (error < 0) { 
      SLOG (compat_log,LLOG_EXCEPTIONS,
            CAT("join_x25_server: ",X25ErrorText(vci,error)),("X25Call"));
      DLOG (compat_log,LLOG_DEBUG,("<--2 join_x25_server"));
      return NOTOK;
    }

    DLOG (compat_log,LLOG_DEBUG,("join_x25_server: X25ReadEvent"));

    X25ReadEvent(vci, (char*) NULL, (int) NULL, &event );

    if (check_x25_event(X25_Clear,event,vci)) {
      SLOG (compat_log,LLOG_EXCEPTIONS,
             Kloppenburgs_x25_ErrorCause(X25_Clear,vci), 
             ("X25_Clear_Event")); 
        DLOG (compat_log,LLOG_DEBUG,("<--3 join_x25_server"));
        return NOTOK;
    }
    else if (check_x25_event(X25_Accepted,event,vci))
    {
         DLOG (compat_log,LLOG_DEBUG,("<--4 join_x25_server"));
         return sd;
    }
    else /* loop until Accept or Clear event occurs */
      {
         if (retry == MAX_RETRY) return NOTOK;
         goto loop;
      }
    
}
#else 

/* standard  case of running and waiting listener */

int     start_x25_client (local)
struct NSAPaddr *local;
{
/******************************************************************/
/* SOURCE for DAEMON (tsapd not spawned by DECNET object spawner) */
/******************************************************************/
    int     error;
    int     our_sd;
    int     our_count;

    X25vc   vci;
    X25vc   * vcip = &vci;

    DLOG (compat_log,LLOG_DEBUG,("--> start_x25_client"));

    if ((our_sd = socket (AF_INET, SOCK_STREAM, 0)) == NOTOK) {
        SLOG (compat_log, LLOG_EXCEPTIONS, "failed", ("socket-call"));
        return NOTOK;
    }
    close(our_sd);

    if (local != NULLNA)
  	local->na_stack = NA_X25, local->na_community=ts_comm_x25_default;
    
    error = X25Init(vcip,(unsigned int) NULL);

    if (error < 0) {
       SLOG (compat_log, LLOG_EXCEPTIONS,
       CAT("start_x25_client: ", X25ErrorText(vci,error)), ("X25Init"));
       return NOTOK;
    }

#ifdef DEBUG
    (void) sprintf(our_buffer," vci after start_x25_client  %d ",(int) vci);
    DLOG (compat_log,LLOG_DEBUG,(our_buffer));
#endif       

   if ( X25RegisterVC(vci,rhandler,whandler,our_sd) < NULL ) {
	SLOG (compat_log, LLOG_EXCEPTIONS, "failed", ("X25RegisterVC"));
	return NOTOK;
   }
   if(our_sd>=our_TABLE_SIZE || (our_count=getdtablesize())>=our_TABLE_SIZE){
       SLOG (compat_log, LLOG_EXCEPTIONS, "invalid",
             ("getdtablesize:increase our_TABLE_SIZE in demsb.h"));
      return NOTOK;
    }
    list_of_conn_desc[our_sd].TCP_IP_OR_X25_SOCKET = our_X25_SOCKET;
    list_of_conn_desc[our_sd].descriptor           = vci;

  DLOG (compat_log,LLOG_DEBUG,("<--start_x25_client"));

  return our_sd;

}

/*  */

int     start_x25_server (local, backlog, opt1, opt2)
struct NSAPaddr *local;
int     backlog,
	opt1,
	opt2;
{
/******************************************************************/
/* SOURCE for DAEMON (tsapd not spawned by DECNET object spawner) */
/******************************************************************/
    X25vc   vci;
    X25vc * vcip = &vci;

    int     pgrp;
    int     error;
    int     event;

    int     our_sd;
    int     our_count;

    DLOG (compat_log,LLOG_DEBUG,("--> start_x25_server"));

    if ((our_sd = socket (AF_INET, SOCK_STREAM, 0)) == NOTOK) {
        SLOG (compat_log, LLOG_EXCEPTIONS, "failed", ("socket-call"));
        return NOTOK;
    }
    close(our_sd);

#ifdef	notyet
#ifdef	BSD43
    int	    onoff;
#endif
#endif

    	if (local != NULLNA) {
	local -> na_stack = NA_X25, local -> na_community = ts_comm_x25_default;
	if (local->na_dtelen == 0) {
	    (void) strcpy (local->na_dte, x25_local_dte);
	    local->na_dtelen = strlen(x25_local_dte);
	    if (local->na_pidlen == 0 && *x25_local_pid)
		local->na_pidlen =
		    str2sel (x25_local_pid, -1, local->na_pid, NPSIZE);
	}
        }


#ifdef DEBUG
    DLOG (compat_log,LLOG_DEBUG,("start_x25_server: X25TakeCall"));
#endif 

    if (X25Init(vcip,(unsigned int) NULL) < NULL) {
        DLOG (compat_log,LLOG_DEBUG,(CAT("start_x25_server:",
              X25ErrorText(vci,error))));
        return NOTOK;
    }

#ifdef	notyet		/* not sure if these are supported... */
#ifndef	BSD43
#else
    onoff = 1;
#endif

    DLOG (compat_log,LLOG_DEBUG,("X25_start_server: X25Option 1"));

    if (opt1) error = X25Option(vci,(unsigned int) opt1);
    if (error < 0) { 
      SLOG (compat_log,LLOG_EXCEPTIONS,
      CAT("start_x25_server:",X25ErrorText(vci,error)),("X25Option1"));
      DLOG (compat_log,LLOG_DEBUG,("<--5 start_x25_server with error"));
      return NOTOK;
    }

    DLOG (compat_log,LLOG_DEBUG,("X25_start_server: X25Option 2"));

    if (opt2) error = X25Option(vci,(unsigned int) opt2);
    if (error < 0) { 
      SLOG (compat_log,LLOG_EXCEPTIONS,
      CAT("start_x25_server: ",X25ErrorText(vci,error)),("X25Option2"));
      DLOG (compat_log,LLOG_DEBUG,("<--6 start_x25_server with error"));
      return NOTOK;
    }
#endif

#ifdef DEBUG
    (void) sprintf(our_buffer," vci after start_x25_server  %d ",(int) vci);
    DLOG (compat_log,LLOG_DEBUG,(our_buffer));
    (void) sprintf(our_buffer," vci after start_x25_server  %d ",our_sd);
    DLOG (compat_log,LLOG_DEBUG,(our_buffer));
#endif              

   if ( X25RegisterVC(vci,rhandler,whandler,our_sd) < NULL ) {
	SLOG (compat_log, LLOG_EXCEPTIONS, "failed", ("X25RegisterVC"));
	return NOTOK;
   }
   if(our_sd>=our_TABLE_SIZE || (our_count=getdtablesize())>=our_TABLE_SIZE){
       SLOG (compat_log, LLOG_EXCEPTIONS, "invalid",
             ("getdtablesize:increase our_TABLE_SIZE in demsb.h"));
      return NOTOK;
    }
    list_of_conn_desc[our_sd].TCP_IP_OR_X25_SOCKET = our_X25_SOCKET;
    list_of_conn_desc[our_sd].descriptor           = vci;


    if (X25Listen(vci,decnet_destination,backlog) < NULL ) {
        DLOG (compat_log,LLOG_DEBUG,(CAT("start_x25_server:",
              X25ErrorText(vci,error))));
        return NOTOK;
    }

  DLOG (compat_log,LLOG_DEBUG,("<-- start_x25_server normally"));

  return our_sd;

}

/*  */


int    join_x25_client (sd, rremote)
int    sd;
struct NSAPaddr *rremote;
{
/******************************************************************/
/* SOURCE for DAEMON (tsapd not spawned by DECNET object spawner) */
/******************************************************************/
   X25vc   vci_l; 
   X25vc   vci; 
   X25vc * vcip = &vci; 

   int error;
   int our_count,our_sd;
   int event;

   CONN_DB     x25_interface;
   CONN_DB * p_x25_interface = &x25_interface;

   DLOG (compat_log,LLOG_DEBUG,("--> join_x25_client"));

   if ((our_sd = socket (AF_INET, SOCK_STREAM, 0)) == NOTOK) {
        SLOG (compat_log, LLOG_EXCEPTIONS, "failed", ("socket"));
        return NOTOK;
   }
   close(our_sd);

   vci_l =  our_get_vci(sd,"join_x25_client");

#ifdef DEBUG
    (void) sprintf(our_buffer," vci_l in join_x25_client  %d ",(int) vci_l);
    DLOG (compat_log,LLOG_DEBUG,(our_buffer));
    (void) sprintf(our_buffer," sd  in join_x25_client  %d ",(int) sd);
    DLOG (compat_log,LLOG_DEBUG,(our_buffer));
#endif       

/* Call available ? */

X25ReadEvent(vci_l, (char*) NULL, (int) NULL, &event );

if (check_x25_event(X25_CallAvail,event,vci_l))  {

#ifdef DEBUG
       DLOG (compat_log,LLOG_DEBUG,
            ("join_x25_client:Call available so X25TakeCall"));
#endif


    error = X25TakeCall(vci_l,vcip);

#ifdef DEBUG
    (void) sprintf(our_buffer," return of X25TakeCall: %d ",error);
    DLOG (compat_log,LLOG_DEBUG,(our_buffer));
    (void) sprintf(our_buffer," vci in join_x25_client  %d ",(int) vci);
    DLOG (compat_log,LLOG_DEBUG,(our_buffer));
#endif       

    if (error < NULL) {
      SLOG (compat_log,LLOG_EXCEPTIONS,
      CAT("join_x25_client:",X25ErrorText(vci,error)),("X25TakeCall"));
      switch (error) { 
        case X25E_BADVCI:
            DLOG (compat_log,LLOG_DEBUG,("BADVCI"));
            break;
        case X25E_NO_LISTEN:
            DLOG (compat_log,LLOG_DEBUG,("NOLISTEN"));
            break;
        case X25E_SOCKOPT:
            DLOG (compat_log,LLOG_DEBUG,("SOCKOPT"));
            break;
        case X25E_ACCEPT:
            DLOG (compat_log,LLOG_DEBUG,("ACCEPT"));
            break;
        case X25E_NO_BUF:
            DLOG (compat_log,LLOG_DEBUG,("NO_BUF"));
            break;
        case X25E_IOCTL:
            DLOG (compat_log,LLOG_DEBUG,("IOCTL"));
            break;
        case  X25E_NOSLOT:
            DLOG (compat_log,LLOG_DEBUG,("NOSLOT"));
            break;
        default:
            break;
    }
    DLOG (compat_log,LLOG_DEBUG,("<--1 join_x25_client after error"));
    return NOTOK;
    }

#ifdef DEBUG
    DLOG (compat_log,LLOG_DEBUG,("join_x25_client: X25ReadEvent"));
#endif

X25ReadEvent(vci, (char*) NULL, (int) NULL, &event );

#ifdef DEBUG
   DLOG (compat_log,LLOG_DEBUG,("join_x25_client: X25_Call event expected"));
#endif

if (check_x25_event(X25_Call,event,vci)) 
{
	/* get remote address */

        DLOG(compat_log,LLOG_DEBUG,("join_x25_client:X25Info(remote_address)"));

	bzero(x25_interface.x25_dte,X25_DTELEN + 1);
	x25_interface.x25_dte[X25_DTELEN]= '\0';
       	error = X25Info(vci,X25I_Remote,x25_interface.x25_dte,X25_DTELEN);

       	if (error < 0) { 
          SLOG (compat_log,LLOG_EXCEPTIONS,
               CAT("join_x25_client: ", X25ErrorText(vci,error)),
                  ("X25InfoRemoteAdr"));
          DLOG (compat_log,LLOG_DEBUG,("<--2 join_x25_client"));
          return NOTOK;
        } 
        else {
      	   DLOG (compat_log,LLOG_DEBUG,(x25_interface.x25_dte));
           x25_interface.x25_dtelen = error;
        }

        /* get user data field */

        DLOG(compat_log,LLOG_DEBUG,
            ("join_x25_client:X25Info(user_data_fields)"));

       	bzero(x25_interface.x25_cudf,X25_CUDFSIZE+1);       
       	x25_interface.x25_cudf[X25_CUDFSIZE]='\0';       
       	error = X25Info(vci,X25I_UserData,x25_interface.x25_cudf,X25_CUDFSIZE);

       	if (error < 0) { 
          SLOG (compat_log,LLOG_EXCEPTIONS,
               CAT("join_x25_client: ", X25ErrorText(vci,error)),
                  ("X25InfoCallUserData"));
          DLOG (compat_log,LLOG_DEBUG,("<--3 join_x25_client"));
          return NOTOK;
   	}
        else {
      	   DLOG (compat_log,LLOG_DEBUG,(x25_interface.x25_cudf));
           x25_interface.x25_cudflen = error;
        }
}	/* if check-Ende ! */
  else {

 	DLOG (compat_log,LLOG_DEBUG,("join_x25_client: X25Clear )"));

        error=X25Clear(vci,(unsigned char)"\0",(unsigned char *) NULL, 
                       (int) NULL, (unsigned char *) NULL, (int) NULL );

        if (error < 0) { 
           SLOG (compat_log,LLOG_EXCEPTIONS,
                 CAT("join_x25_client: ",X25ErrorText(vci,error)),
                 ("X25Clear"));
        }
        DLOG(compat_log,LLOG_DEBUG,("<--4 join_x25_srv wt. error"));
        return NOTOK;
     }

   DLOG (compat_log,LLOG_DEBUG,("ACCEPT IN join_x25_client"));

   error =  X25Accept( vci, (unsigned char *) NULL, (int) NULL,
                       (unsigned char *) NULL, (int) NULL ); 

   if (error < 0) {
      SLOG (compat_log,LLOG_EXCEPTIONS,
      CAT("join_x25_client: ",X25ErrorText(vci,error)),("X25Accept"));
      DLOG (compat_log,LLOG_DEBUG,("<--1 join_x25_client with error"));
      return NOTOK;
   }

  DLOG (compat_log,LLOG_DEBUG,("ACCEPT succeded in join_x25_client"));

#ifdef DEBUG
    (void) sprintf(our_buffer," vci after join_x25_client  %d ",(int) vci);
    DLOG (compat_log,LLOG_DEBUG,(our_buffer));
    (void) sprintf(our_buffer," vci after join_x25_client  %d ",our_sd);
    DLOG (compat_log,LLOG_DEBUG,(our_buffer));
#endif       

   if ( X25RegisterVC(vci,rhandler,whandler,our_sd) < NULL ) {
	SLOG (compat_log, LLOG_EXCEPTIONS, "failed", ("X25RegisterVC"));
	return NOTOK;
   }
   if(our_sd>=our_TABLE_SIZE || (our_count=getdtablesize())>=our_TABLE_SIZE){
       SLOG (compat_log, LLOG_EXCEPTIONS, "invalid",
             ("getdtablesize:increase our_TABLE_SIZE in demsb.h"));
      return NOTOK;
    }
    list_of_conn_desc[our_sd].TCP_IP_OR_X25_SOCKET = our_X25_SOCKET;
    list_of_conn_desc[our_sd].descriptor           = vci;

/* fill call user data field, pid and remote address */

     (void) if2gen (rremote, p_x25_interface, NULL);

#ifdef DEBUG
    DLOG (compat_log,LLOG_DEBUG,("<-- leave join_x25_client"));
#endif

  return our_sd;

}
else {
#ifdef DEBUG
    DLOG (compat_log,LLOG_DEBUG,("join_x25_client: no Call available"));
#endif

    return NOTOK;
}

}

int     join_x25_server (sd, rremote)
int    sd;
struct NSAPaddr *rremote;
{
/******************************************************************/
/* SOURCE for DAEMON (tsapd not spawned by DECNET object spawner) */
/* (should be the supported version)                              */
/******************************************************************/
    X25vc   vci;

    int error;
    int event;
    int retry = 0;

    CONN_DB     x25_interface;
    CONN_DB * p_x25_interface = &x25_interface;

    DLOG (compat_log,LLOG_DEBUG,("--> join_x25_server"));

    vci =  our_get_vci(sd,"join_x25_server");

#ifdef DEBUG
    (void) sprintf(our_buffer," vci join_x25_server  %d ",(int) vci);
    DLOG (compat_log,LLOG_DEBUG,(our_buffer));
    (void) sprintf(our_buffer," sd  join_x25_server  %d ",(int) sd );
    DLOG (compat_log,LLOG_DEBUG,(our_buffer));
#endif       

      if (rremote == NULLNA || rremote -> na_stack != NA_X25)
    {
	SLOG (compat_log, LLOG_EXCEPTIONS, NULLCP,
  	      ("Invalid type na%d", rremote->na_stack));
        DLOG (compat_log,LLOG_DEBUG,("<--1 join_x25_server"));
	return NOTOK;
    }

loop:

    retry++;
  
    DLOG (compat_log,LLOG_DEBUG,("join_x25_server: X25Call"));

#ifdef DEBUG

bcopy( rremote->na_dte, our_buffer, (int) rremote->na_dtelen);
(void) sprintf(&our_buffer[(int) rremote->na_dtelen],"=remote dte in join_x25_server");
DLOG (compat_log,LLOG_DEBUG,(our_buffer));

bcopy( rremote->na_fac, our_buffer, (int) rremote->na_faclen);
(void) sprintf(&our_buffer[(int) rremote->na_faclen],"=remote fac in join_x25_server");
DLOG (compat_log,LLOG_DEBUG,(our_buffer));

bcopy( rremote->na_cudf, our_buffer, (int) rremote->na_cudflen);
(void) sprintf(&our_buffer[(int)rremote->na_cudflen],"=remote cud in join_x25_server");
DLOG (compat_log,LLOG_DEBUG,(our_buffer));

bcopy( rremote->na_pid, our_buffer, (int) rremote->na_pidlen);
(void) sprintf(&our_buffer[(int) rremote->na_pidlen],"=remote pid in join_x25_server");
DLOG (compat_log,LLOG_DEBUG,(our_buffer));

#endif       

    x25_interface.x25_cudf[X25_CUDFSIZE]='\0';       
    x25_interface.x25_dte[X25_DTELEN]= '\0';

    (void) gen2if (rremote, p_x25_interface, NULL);

    error = X25Call(vci,rremote->na_dte,demsa_local_subaddr,
                        rremote->na_fac,  (int) rremote->na_faclen,
                        x25_interface.x25_cudf, 
                        (int) x25_interface.x25_cudflen, NULL );

    if (error < 0) { 
      SLOG (compat_log,LLOG_EXCEPTIONS,
            CAT("join_x25_server: ",X25ErrorText(vci,error)),("X25Call"));
      DLOG (compat_log,LLOG_DEBUG,("<--2 join_x25_server"));
      return NOTOK;
    }

    DLOG (compat_log,LLOG_DEBUG,("join_x25_server: X25ReadEvent"));

    X25ReadEvent(vci, (char*) NULL, (int) NULL, &event );

    if (check_x25_event(X25_Clear,event,vci)) {
      SLOG (compat_log,LLOG_EXCEPTIONS,
             Kloppenburgs_x25_ErrorCause(X25_Clear,vci), 
             ("X25_Clear_Event")); 
        DLOG (compat_log,LLOG_DEBUG,("<--3 join_x25_server"));
        return NOTOK;
    }
    else if (check_x25_event(X25_Accepted,event,vci))
    {
         DLOG (compat_log,LLOG_DEBUG,("<--4 join_x25_server"));
         return sd;
    }
    else /* loop until Accept or Clear event occurs */
      {
         if (retry == MAX_RETRY) return NOTOK;
         goto loop;
      }
    
}

#endif /* ULTRIX_X25_SPAWN_TSAPD */

int     read_x25_socket (sd, buffer, len)
int     sd;   
int     len;
char    *buffer;
{
X25vc   vci;  
int event, count;
int retry = 0;    


    DLOG (compat_log,LLOG_DEBUG,("--> read x25 socket"));

    vci =  our_get_vci(sd,"read_x25_socket");

#ifdef DEBUG
    (void) sprintf(our_buffer," vci in read_x25_socket  %d ",(int) vci);
    DLOG (compat_log,LLOG_DEBUG,(our_buffer));
    (void) sprintf(our_buffer," sd  in read_x25_socket  %d ",(int) sd );
    DLOG (compat_log,LLOG_DEBUG,(our_buffer));
#endif       

   count = X25ReadEvent( vci, buffer, len, &event );

   DLOG (compat_log, LLOG_DEBUG, ("X25Read, buffer_len %d read %d",count,len));

   if (count < 0) { 
       SLOG (compat_log,LLOG_EXCEPTIONS,
       CAT("read_x25_socket: ",X25ErrorText(vci,count)),("read_x25_socket"));
       return NOTOK;               
   }

   if(!check_x25_event(X25_Data,event,vci)) {
       DLOG (compat_log, LLOG_DEBUG, ("X25_Data event expected: -> error ! "));
       return NOTOK;               
   }

  DLOG (compat_log,LLOG_DEBUG,("<-- read x25 socket"));

  return count;

}


int     write_x25_socket (sd, buffer, len)
int     sd;
int     len;
char    *buffer;
{

int count;
X25vc   vci;

    DLOG (compat_log,LLOG_DEBUG,("--> write x25 socket"));

    vci =  our_get_vci(sd,"write_x25_socket");

#ifdef DEBUG
    (void) sprintf(our_buffer," vci in write_x25_socket  %d ",(int) vci);
    DLOG (compat_log,LLOG_DEBUG,(our_buffer));
    (void) sprintf(our_buffer," sd  in write_x25_socket  %d ",(int) sd );
    DLOG (compat_log,LLOG_DEBUG,(our_buffer));
#endif       

count = X25Send( vci, buffer, len, (int) NULL );

DLOG (compat_log,LLOG_DEBUG,("X25Send buffer_len %d  sent %d",len,count));

if (count < 0) { 
  SLOG (compat_log,LLOG_EXCEPTIONS,
        CAT("write_x25_socket: ",X25ErrorText(vci,count)),
        ("write_x25_socket"));
  DLOG (compat_log,LLOG_DEBUG,("<--1  write x25 socket"));
  return NOTOK;
 }

  DLOG (compat_log,LLOG_DEBUG,("<-- write x25 socket"));

  return count;

}

close_x25_socket (sd)
int     sd;
{
int error = OK;
X25vc   vci;  
   
    DLOG (compat_log,LLOG_DEBUG,("--> close x25 socket"));

    vci =  our_get_vci(sd,"close_x25_socket");
    list_of_conn_desc[sd].TCP_IP_OR_X25_SOCKET = NOTOK;
    list_of_conn_desc[sd].descriptor           = NOTOK;

#ifdef DEBUG
    (void) sprintf(our_buffer," vci in close_x25_socket  %d ",(int) vci);
    DLOG (compat_log,LLOG_DEBUG,(our_buffer));
    (void) sprintf(our_buffer," sd  in close_x25_socket  %d ",(int) sd );
    DLOG (compat_log,LLOG_DEBUG,(our_buffer));
#endif       

    error = X25Finish(vci);

    if (error < 0) { 
      SLOG (compat_log,LLOG_EXCEPTIONS,
            CAT("close_x25_socket: ",X25ErrorText(vci,error)),
                                        ("close_x25_socket"));
     }

     DLOG (compat_log,LLOG_DEBUG,("<--close x25 socket"));

}


#endif 	/* ULTRIX_X25_DEMSA */
#else   /* ULTRIX_X25 */
int     _ultrix25_stub2 () {;}
#endif  /* ULTRIX_X25 */
#else	/* X25 */
int	_ultrix25_stub () {;}
#endif  /* X25 */
