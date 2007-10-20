/* ts2tli.c - TPM: TLI OSI TP interface */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/tsap/RCS/ts2tli.c,v 9.0 1992/06/16 12:40:39 isode Rel $";
#endif

/*
 * This module written by X-Tel Serviecs Ltd, UK. 
 * for Boldon James Limited UK. There donation
 * to the ISODE tree is gratefully received.
 *
 * ICL options later added by X-Tel Serviecs Ltd, UK.
 */

/* 
 * $Header: /xtel/isode/isode/tsap/RCS/ts2tli.c,v 9.0 1992/06/16 12:40:39 isode Rel $
 *
 *
 * $Log: ts2tli.c,v $
 * Revision 9.0  1992/06/16  12:40:39  isode
 * Release 8.0
 *
 */

/* LINTLIBRARY */

#include <stdio.h>
#include "tpkt.h"

#ifdef	TP4
#include "tp4.h"
#endif

#ifdef	TLI_TP
#include <errno.h>
#include <fcntl.h>
#include "tailor.h"
#include "sys/file.h"

/*
 *  The TLI does not support the functionality of multi-buffer transfers
 *  in a single call (like writev(2)).  Thus because of the overhead of a
 *  system call it may be better to do a local copy of the bits of a
 *  TSDU before doing a t_snd().  Setting TSDU_COPY_LIMIT determines
 *  how much of a TSDU may be copied in order to reduce the number of
 *  system calls.
 *
 *  Measurments on one system suggest that the break-even point is
 *  about 10kb.
 *
 *  Some implementations also generate a single TPDU (with all the
 *  associated overhead) for each t_snd() call and in that case then
 *  setting this may have an advantage regardless of the system-call
 *  overhead factor.
 *
 *  Override this with a setting in h/config.h.
 */
#ifndef TSDU_COPY_LIMIT
#define TSDU_COPY_LIMIT 0
#endif

extern char *tli_cots_dev;
extern char *tli_clts_dev;

#ifdef ICL_TLI		/* TLI X25 on ICL DRS6000 */

extern char *icl_tli_initiator;
extern char *icl_tli_responder;

/* Address format characters */
#define ICL_TSEL_MARKER		0x80
#define ICL_NSAP_MARKER		0x81
#define ICL_DTE_MARKER		0x82
#define ICL_CUG_MARKER		0x83
#define ICL_PORT_MARKER		0x84

#define ICL_TSEL_LEN		0x21
#define ICL_NSAP_LEN		0x15
#define ICL_DTE_LEN		0x09

#define ADD_DTE2NSAP	/* Should not have to do this - should be tli's job */

#endif

/*
 * Better to do segmenting across the syscall interface than in Session.
 * Make this as big as you dare, but remember all your receive buffers
 * will be this big too.
 */
#define	MAXTP4		8192

/* why do we need this ? */
#define	TP4SLOP		  12	/* estimate of largest DT PCI */

extern int  errno;
extern int t_nerr, t_errno;
extern char *t_errlist[];

#define TLI_CTX_CALL 	0
#define TLI_CTX_LISTEN	1

/* Options structure used by Concurrent RTnet-OSI R02 product */
#ifdef RTnet_R02
struct opts {
    int size;
#   define OPT_SIZE_DEFAULT 0
    unsigned char class;
#   define OPT_TP0_ALTERNATE	0x80
#   define OPT_TP4		0x40
#   define OPT_TP2		0x20
#   define OPT_TP2_NO_FLOW	0x10
#   define OPT_MASTER_PRIORITY	0x08
#   define OPT_EXPEDITED	0x01
#   define OPT_CLASS_DEFAULT (OPT_TP4 | OPT_TP0_ALTERNATE)
    struct {
	unsigned char secondary_priority:4;
	unsigned char traffic_class:3;
	unsigned char teletex:1;
    } qos;
};
#define OPTS_SIZE 6
#endif

/*    Utility Routines */

static char *sys_terrname (te)
int te;
{
    static char tbuf[32];

    if (te > 0 && te <= t_nerr)
	return t_errlist[te];
    (void) sprintf (tbuf, "Terrno %d", te);
    return tbuf;
}

static tli_lose (td, fd, reason, str)
struct TSAPdisconnect *td;
int fd, reason;
char *str;
{
    int eindex = errno;
    int tindex = t_errno;
    (void) tsaplose (td, reason, sys_terrname (t_errno), str);
    if (fd != NOTOK)
	(void) t_close (fd);
    errno = eindex;
    t_errno = tindex;
    return NOTOK;
}

#ifdef RTnet_R02
static int tp4err2gen(err)
int err;
{
    int ret;

    switch (err & 0xff) {
    case 0xfc: ret = DR_CONGEST; break;
    case 0xf9: ret = DR_PROTOCOL; break;
    case 0xf6: ret = DR_REFUSED; break;
    case 0xf3: ret = DR_ADDRESS; break;
    case 0xf0: ret = DR_PROTOCOL; break;
    case 0xff: ret = DR_UNKNOWN; break;
    case 0x00: ret = DR_NORMAL; break;
    default: ret = err;
    }
    DLOG (tsap_log, LLOG_DEBUG, ("tp4err2gen(0x%x): return 0x%x", err, ret));
    return ret;
}
#define tp4err_def
#endif

#ifndef tp4err_def
#define tp4err2gen(err) (err)
#endif

/* open and bind an endpoint */
static int tp4bind(ta, qlen, td, context)
struct TSAPaddr *ta;
int qlen;
struct TSAPdisconnect *td;
int context;
{
    int fd;
    char * tli_dev = tli_clts_dev;
    struct NSAPinfo *ni;
    int ts = nsap_default_stack;

    /* CONS or CLNS ? */
    if (ta && (ta -> ta_naddr > 0)) 
	    if (ni = getnsapinfo (ta -> ta_addrs))
		    ts = ni -> is_stack;
	    
    if (ts == NAS_CONS)
	    tli_dev = tli_cots_dev;

    if ((fd = t_open (tli_dev, O_RDWR, NULL)) == NOTOK)
           return tli_lose (td, NOTOK, DR_CONGEST, "cannot open TS");

    if (ta && (ta -> ta_naddr > 0 || ta -> ta_selectlen > 0)) {
	struct t_bind *bind, *bound;
	struct TSAPaddr bound_ta;

	if ((bind = (struct t_bind *)t_alloc (fd, T_BIND, T_ALL)) == NULL)
	    return tli_lose (td, fd, DR_CONGEST, "can't allocate bind");
	if ((bound = (struct t_bind *)t_alloc (fd, T_BIND, T_ALL)) == NULL) {
	    t_free ((char *)bind, T_BIND);
	    return tli_lose (td, fd, DR_CONGEST, "can't allocate bind");
	}

	if (gen2tp4 (ta, &bind -> addr, context) == NOTOK) {
	    fd = tli_lose (td, fd, DR_ADDRESS, "invalid address");
	    goto out;
	}
	bind->qlen = qlen;

	if (t_bind (fd, bind, bound) == NOTOK) {
	    char buf[TS_SIZE*2+1];

	    fd = tli_lose (td, fd, DR_CONGEST, "unable to bind");
	    buf[explode(buf, bind->addr.buf, bind->addr.len)] = '\0';
	    LLOG (tsap_log, LLOG_EXCEPTIONS,
		("fail to bind %s as %s", taddr2str(ta), buf));
	    goto out;
	}

	/*
	 * Check that we bound where we asked,
	 * we could be given someting else.
	 * Only care about selector
	 */
	(void) tp42gen(&bound_ta, &bound->addr);
	if ((qlen && bound->qlen < 1) ||
	    ta->ta_selectlen != bound_ta.ta_selectlen ||
	    memcmp(ta->ta_selector, bound_ta.ta_selector,
		    bound_ta.ta_selectlen) != 0)
	{
	    char buf[BUFSIZ];

	    t_unbind(fd);
	    fd = tli_lose (td, fd, DR_CONGEST, "address in use");
	    (void) strcpy(buf, taddr2str(ta));
	    LLOG (tsap_log, LLOG_EXCEPTIONS,
		("tried to bind to %s but got %s", buf, taddr2str(&bound_ta)));
	    goto out;
	}
    out:
	t_free ((char *)bind, T_BIND);
	t_free ((char *)bound, T_BIND);
    } else {
	if (t_bind (fd, (struct t_bind *)0, (struct t_bind *)0) == NOTOK) 
	    fd = tli_lose (td, fd, DR_CONGEST, "unable to bind to NULL");
    }

#ifdef DEBUG
    if (ta)
       DLOG (tsap_log, LLOG_TRACE, ("bound %d to %s", fd, taddr2str(ta)));
    else 
       DLOG (tsap_log, LLOG_TRACE, ("bound %d to NULLTA",fd));
#endif

    return fd;
}

/*
 * Check that the endpoint is suitable.
 * WARNING: do not call this after determining if an actual connection
 * 	supports expedited data (or other options).  It is not defined
 *	whether t_getinfo() return defaults or current values once a 
 *	connection is established.
 */
static int tp4info(tb, td)
struct tsapblk *tb;
struct TSAPdisconnect *td;
{
    struct t_info info;
    int	    len;

    if (t_getinfo (tb -> tb_fd, &info) == NOTOK) {
	return tli_lose (td, NOTOK, DR_CONGEST, "t_getinfo");
    }

    DLOG (tsap_log, LLOG_DEBUG,
	("tp4info: addr=%d, opt=%d, tsdu=%d, etsdu=%d, con=%d, dis=%d, serv=%d",
	info.addr, info.options, info.tsdu, info.etsdu,
	info.connect, info.discon, info.servtype));
    switch (len = info.tsdu) {
    case -2:
	return tsaplose (td, DR_PARAMETER, NULLCP, "TSDU data not supported");
    case 0:
	return tsaplose (td, DR_PARAMETER, NULLCP, "TSDU not packetized");
    case -1:
	len = MAXTP4;
	break;
    default:
	break;
    }

    if (info.etsdu == -1 || info.etsdu > 0)
	tb -> tb_flags |= TB_EXPD;
    else
	tb -> tb_flags &= ~TB_EXPD;
    tb -> tb_tpduslop = TP4SLOP;
    tb -> tb_tsdusize = len - tb -> tb_tpduslop;

    return OK;
}

static int tp4getdis(fd, td)
int fd;
struct TSAPdisconnect *td;
{
    struct t_discon *discon;

    discon = (struct t_discon *)t_alloc (fd, T_DIS, T_ALL);
    if (t_rcvdis (fd, discon) == NOTOK) {
	(void) tli_lose (td, NOTOK, DR_NETWORK, "t_discon");
	goto out;
    }
    if (discon != (struct t_discon *)0) {
	td -> td_reason = tp4err2gen(discon -> reason);
	if ((td -> td_cc = discon -> udata.len) > 0) {
	    if (td -> td_cc > TD_SIZE)
		td -> td_cc = TD_SIZE; /* discard extra! */
	    bcopy (discon -> udata.buf, td -> td_data, td -> td_cc);
	}
    } else {
	td -> td_reason = DR_UNKNOWN;
	td -> td_cc = 0;
    }

out:
    if (discon != (struct t_discon *)0)
	t_free ((char *)discon, T_DIS);

    return NOTOK;
}

/*    UPPER HALF */

static int  TConnect (tb, expedited, data, cc, td)
register struct tsapblk *tb;
char    *data;
int	expedited,
	cc;
struct TSAPdisconnect *td;
{
    struct t_call *sndcall;
    struct t_call *rcvcall;
    struct TSAPaddr ta;

    if ((sndcall =
	    (struct t_call *) t_alloc (tb -> tb_fd, T_CALL, T_ALL)) == NULL)
	return tli_lose (td, NOTOK, DR_CONGEST, "t_alloc");
    if ((rcvcall =
	    (struct t_call *) t_alloc (tb -> tb_fd, T_CALL, T_ALL)) == NULL)
    {
	t_free ((char *)sndcall, T_CALL);
	return tli_lose (td, NOTOK, DR_CONGEST, "t_alloc");
    }

#ifdef RTnet_R02
    if (!expedited) { /* expedited negotiated by default */
	struct opts * opts;
	struct t_optmgmt * def;

	if ((def =
	    (struct t_optmgmt *) t_alloc (tb -> tb_fd, T_OPTMGMT, T_ALL))
	    == NULL)
	{
	    t_free ((char *)sndcall, T_CALL);
	    t_free ((char *)rcvcall, T_CALL);
	    return tli_lose (td, NOTOK, DR_CONGEST, "t_alloc");
	}

	def->flags = T_DEFAULT;
	if (t_optmgmt(tb -> tb_fd, def, def) == NOTOK) {
	    t_free ((char *)sndcall, T_CALL);
	    t_free ((char *)rcvcall, T_CALL);
	    t_free ((char *)def, T_OPTMGMT);
	    return tli_lose (td, NOTOK, DR_CONGEST, "t_optmgmt");
	}
	opts = (struct opts *) sndcall -> opt.buf;

	/* first copy in hard-coded defaults */
	opts -> size = OPT_SIZE_DEFAULT;
	opts -> class = OPT_CLASS_DEFAULT;

	/* then replace with defaults from endpoint if returned */
	bcopy(def -> opt.buf, sndcall -> opt.buf, def -> opt.len);

	t_free ((char *)def, T_OPTMGMT);
	if (expedited)
	    opts -> class |= OPT_EXPEDITED;
	else
	    opts -> class &= ~OPT_EXPEDITED;
	sndcall -> opt.len = OPTS_SIZE;
    }
#endif

    copyTSAPaddrX(&tb -> tb_responding, &ta);
    if (gen2tp4(&ta, &sndcall -> addr, TLI_CTX_CALL) == NOTOK)
	return tli_lose (td, NOTOK, DR_ADDRESS, "invalid address");

    toomuchP(data, cc, sndcall -> udata.maxlen, "initial");
    if (data)
	bcopy (data, sndcall -> udata.buf, sndcall -> udata.len = (unsigned)cc);


    /* use tb_cc to communicate to TRetry if the connect completed here */
    if (t_connect (tb -> tb_fd, sndcall, rcvcall) == NOTOK) {
	t_free ((char *)rcvcall, T_CALL);
	t_free ((char *)sndcall, T_CALL);
	switch (t_errno) {
	
	case TNODATA:
	    tb->tb_cc = -1;
	    return CONNECTING_2;

	case TLOOK:
	    switch (t_look(tb -> tb_fd)) {
	    case T_CONNECT:
		tb->tb_cc = -1;
		return CONNECTING_2;

	    case T_DISCONNECT:
		return tp4getdis(tb -> tb_fd, td);
	    }

	case TBADADDR:
	    return tli_lose (td, NOTOK, DR_ADDRESS,
			     "unable to establish connection");

	default:
	    return tli_lose (td, NOTOK, DR_REFUSED,
			     "unable to establish connection");
	}

    }

    t_free ((char *)sndcall, T_CALL);

    (void) tp42gen (&ta, &rcvcall -> addr);
    copyTSAPaddrY(&ta, &tb -> tb_responding);

    if (cc = rcvcall->udata.len) {
	if (tb->tb_data)
	    free (tb -> tb_data);
	if ((tb -> tb_data = malloc ((unsigned) cc)) == NULLCP) {
	    t_free ((char *)rcvcall, T_CALL);
	    (void) tsaplose (td, DR_CONGEST, NULLCP, "out of memory");
	}
	bcopy (rcvcall->udata.buf, tb->tb_data, tb->tb_cc = cc);
    } else
	tb->tb_cc = 0;
#ifdef RTnet_R02
    if (rcvcall -> opt.len >= OPTS_SIZE) {
	struct opts * opts = (struct opts *) rcvcall -> opt.buf;
	if (opts -> class & OPT_EXPEDITED)
	    tb -> tb_flags |= TB_EXPD;
	else
	    tb -> tb_flags &= ~TB_EXPD;
    } else
	/* assume no expedited */
	tb -> tb_flags &= ~TB_EXPD;
#else
    /* assume no expedited */
    tb -> tb_flags &= ~TB_EXPD;
#endif
    t_free ((char *)rcvcall, T_CALL);
	
    return DONE;
}

/*  */

static int  TRetry (tb, async, tc, td)
register struct tsapblk *tb;
int	async;
struct TSAPconnect *tc;
struct TSAPdisconnect *td;
{
    struct t_call *call;

    if (tb->tb_cc == -1) { /* call not yet connected */
	struct TSAPaddr ta;

	if (async && tb -> tb_retryfnx) {
	    switch ((*tb -> tb_retryfnx) (tb, td)) {
	    case NOTOK:
		goto out;
	    case OK:
		return CONNECTING_2;

	    case DONE:
		break;
	    }
	}

	if ((call = (struct t_call *)
	    t_alloc (tb -> tb_fd, T_CALL, T_ALL)) == NULL)
	{
	    (void) tli_lose (td, NOTOK, DR_CONGEST, "t_alloc ");
	    goto out;
	}

	if (t_rcvconnect (tb -> tb_fd, call) == NOTOK) {
	    t_free ((char *)call, T_CALL);
	    if (t_errno == TNODATA)
		return CONNECTING_2;
	    else if (t_errno == TLOOK && t_look(tb -> tb_fd) == T_DISCONNECT)
		(void) tp4getdis(tb -> tb_fd, td);
	    else 
		(void) tli_lose (td, NOTOK, DR_REFUSED, "t_rcvconnect");
	    goto out;
	}

	if (async) {
	    int flags;
	    if ((flags = fcntl (tb -> tb_fd, F_GETFL, 0)) == NOTOK) {
		t_free ((char *)call, T_CALL);
		(void) tsaplose (td, DR_CONGEST, "failed", "fcntl");
		goto out;
	    }
	    
	    flags &= ~FNDELAY;
	    (void) fcntl (tb -> tb_fd, F_SETFL, flags);
	}

	(void) tp42gen (&ta, &call -> addr);
	copyTSAPaddrY(&ta, &tb -> tb_responding);

	if (call -> udata.len > 0)
	    bcopy (call -> udata.buf, tc -> tc_data,
		   (int)(tc -> tc_cc = call->udata.len));
#ifdef RTnet_R02
	if (call -> opt.len == OPTS_SIZE) {
	    struct opts * opts = (struct opts *) call -> opt.buf;
	    if (opts -> class & OPT_EXPEDITED)
		tb -> tb_flags |= TB_EXPD;
	    else
		tb -> tb_flags &= ~TB_EXPD;
	}
#else
	tb -> tb_flags &= ~TB_EXPD;
#endif
	t_free ((char *)call, T_CALL);
    } else {
	if (tc -> tc_cc = tb -> tb_cc)
	    bcopy (tb -> tb_data, tc -> tc_data, tc -> tc_cc);
    }
    tb -> tb_flags |= TB_CONN;

    tc -> tc_expedited = (tb -> tb_flags & TB_EXPD) ? 1 : 0;
    tc -> tc_sd = tb -> tb_fd;
    tc -> tc_tsdusize = tb -> tb_tsdusize;
    copyTSAPaddrX(&tb -> tb_responding, &tc -> tc_responding);
#ifdef  MGMT
    if (tb -> tb_manfnx)
	(*tb -> tb_manfnx) (OPREQOUT, tb);
#endif

    return DONE;

out: ;
    freetblk (tb);

    return NOTOK;
}

/*  */

static int  TStart (tb, cp, ts, td)
register struct tsapblk *tb;
char   *cp;
struct TSAPstart *ts;
struct TSAPdisconnect *td;
{
    int	    i;

    DLOG (tsap_log, LLOG_DEBUG, ("TStart: \"%s\"", cp));
    
    ts -> ts_expedited = (tb -> tb_flags & TB_EXPD) ? 1 : 0;
    ts -> ts_sd = tb -> tb_fd;
    copyTSAPaddrX(&tb -> tb_initiating, &ts -> ts_calling);
    copyTSAPaddrX(&tb -> tb_responding, &ts -> ts_called);
    ts -> ts_expedited = (tb -> tb_flags & TB_EXPD) ? 1 : 0;
    ts -> ts_tsdusize = tb -> tb_tsdusize;

    if ( cp && ((i = strlen (cp)) > 0)) {
	if (i > 2 * TS_SIZE)
	    return tsaplose (td, DR_CONNECT, NULLCP,
			     "too much initial user data");
	ts -> ts_cc = implode ((u_char *) ts -> ts_data, cp, i);
    } else
	ts -> ts_cc = 0;

    return OK;
}

/*  */

/*
 * This is a tricky one. We have to pass over a dup'd transport descriptor
 * and here we have to finish this by accepting the call and completing
 * the connection. When started the fd is the descriptor that we are listening
 * on, but by the end it is the one we accepted dup'd into the old slot.
 * ISODE doesn't quite fit the TLI model here....
 */
/* ARGSUSED */

static int  TAccept (tb, responding, data, cc, qos, td)
register struct tsapblk *tb;
char   *data;
int	responding,
	cc;
struct QOStype *qos;
struct TSAPdisconnect *td;
{
    struct t_call *call;
    int result;
    
    /* we are going to accept on a new endpoint */
    if ((result = tp4bind ((struct TSAPaddr *)0, 0, td, TLI_CTX_LISTEN)) == NOTOK)
	goto reject;

    if ((call = (struct t_call *)t_alloc(tb->tb_fd, T_CALL, T_ALL)) == NULL) {
	(void) tli_lose (td, result, DR_CONGEST, "t_alloc");
	goto reject;
    }

    if (data)			/* user data? */ 
	bcopy (data, call -> udata.buf, call -> udata.len = (unsigned)cc);
    call -> sequence = tb -> tb_seq;

    DLOG (tsap_log, LLOG_TRACE,
	("t_accept(%d, %d): seq=%d", tb -> tb_fd, result, call -> sequence));
    if (t_accept (tb -> tb_fd, result, call) == NOTOK) {
	DLOG (tsap_log, LLOG_DEBUG,
	    ("t_accept fail: t_errno=%d, t_look=%d",
	    t_errno, t_look(tb->tb_fd)));
	if (t_errno == TLOOK && t_look(tb -> tb_fd) == T_DISCONNECT)
	    (void) tp4getdis(tb -> tb_fd, td);
	else
	    (void) tli_lose (td, result, DR_CONGEST, "t_accept");
	t_free ((char *)call, T_CALL);
	return NOTOK;
    }
    t_free ((char *)call, T_CALL);

    /* OK - we have the new call - lets pretend we are back where we were...*/
    (void) t_close (tb -> tb_fd); /* finished with this - get rid of it! */
    (void) dup2 (result, tb -> tb_fd);
    (void) t_close(result);
    (void) t_sync(tb -> tb_fd);

    /* Phew - we now have everything as it should be - I hope */

    tb -> tb_flags |= TB_CONN;
#ifdef  MGMT
    if (tb -> tb_manfnx)
	(*tb -> tb_manfnx) (OPREQIN, tb);
#endif

    return OK;

reject:
    {
	/* since we might be here beacuse we could not t_alloc */
	struct t_call dis;

	bzero((char *)&dis, sizeof dis);
	dis.sequence = tb->tb_seq;
	(void) t_snddis(tb->tb_fd, &dis);
	return NOTOK;
    }
}

/*  */

static int  TWrite (tb, uv, expedited, td)
register struct tsapblk *tb;
register struct udvec *uv;
int	expedited;
struct TSAPdisconnect *td;
{
    register int cc;
    int	async;
    int	    flags;
#ifdef	MGMT
    int	    dlen;
#endif
    register char *bp;
    register struct qbuf *qb;
    register struct udvec *xv;

    flags = expedited ? T_EXPEDITED : 0;
    
#ifdef	MGMT
    dlen = 0;
#endif

    if (!expedited && (tb -> tb_flags & TB_QWRITES)) {
	int	nc;

	cc = 0;
	for (xv = uv; xv -> uv_base; xv++)
	    cc += xv -> uv_len;
#ifdef	MGMT
	dlen = cc;
#endif

	if ((qb = (struct qbuf *) malloc (sizeof *qb + (unsigned) cc))
		== NULL) {
	    (void) tsaplose (td, DR_CONGEST, NULLCP,
		 "unable to malloc %d octets for pseudo-writev, failing...",
		 cc);
	    freetblk (tb);

	    return NOTOK;
	}
	qb -> qb_forw = qb -> qb_back = qb;
	qb -> qb_data = qb -> qb_base, qb -> qb_len = cc;

	bp = qb -> qb_data;
	for (xv = uv; xv -> uv_base; xv++) {
	    bcopy (xv -> uv_base, bp, xv -> uv_len);
	    bp += xv -> uv_len;
	}

	if (tb -> tb_qwrites.qb_forw != &tb -> tb_qwrites) {
	    nc = 0;
	    goto insert;
	}

	if ((async = fcntl (tb -> tb_fd, F_GETFL, 0)) != NOTOK)
	    (void) fcntl (tb -> tb_fd, F_SETFL, async | FNDELAY);

	nc = t_snd (tb -> tb_fd, qb -> qb_data, qb -> qb_len, 0);
	DLOG (tsap_log, LLOG_TRACE,
	    ("t_snd(fd=%d, buf=0x%x, len=%d, NO_MORE) ret=%d",
	    tb -> tb_fd, qb -> qb_data, qb -> qb_len, nc));


	if (async != NOTOK)
	    (void) fcntl (tb -> tb_fd, F_SETFL, async);

	if (nc != cc) {
	    if (nc == NOTOK) {
		if (t_errno == TLOOK && t_look(tb -> tb_fd) == T_DISCONNECT) {
		    (void) tp4getdis(tb -> tb_fd, td);
		    goto losing;
		} else if (t_errno != TFLOW) {
		    (void) tsaplose (td, DR_CONGEST, "failed", "sendmsg");
		    goto losing;
		}

		nc = 0;
	    }
	    if ((*tb -> tb_queuePfnx) (tb, 1, td) == NOTOK)
		goto losing;

	    qb -> qb_data += nc, qb -> qb_len -= nc;
insert: ;
	    insque (qb, tb -> tb_qwrites.qb_back);
	    DLOG (tsap_log, LLOG_TRACE,
		  ("queueing blocked write of %d of %d octets", nc, cc));
	}
	else
	    free ((char *) qb);
	goto done;

losing: ;
	free ((char *) qb);
	freetblk (tb);

	return NOTOK;
    }

    while (uv -> uv_base != NULLCP)  {
	int more, ret;
	char *d;

	/* see if it worth coalescing the bits */
	for (d = uv -> uv_base, cc = 0, xv = uv;
	     xv -> uv_base != NULLCP;
	     xv++)
	{
	    /* not sure why one cannot set the more bit on expedited */
	    if (expedited || (cc + xv -> uv_len) <= TSDU_COPY_LIMIT)
		cc += xv -> uv_len;
	    else
		break;
	}
	/* xv now points to first unused bit */
	if (cc > uv -> uv_len) { /* used more than one bit */
	    d = malloc(cc);
	    for (bp = d; uv < xv; uv++) {
		bcopy(uv -> uv_base, bp, uv -> uv_len);
		bp += uv -> uv_len;
	    }
	} else {
	    cc = uv -> uv_len;
	    uv++;
	    bp = NULLCP;		/* marker to know not to free 'd' */
	}
	/* uv now points to first unused bit */

#ifdef	MGMT
	dlen += cc;
#endif
	more = (uv -> uv_base != NULLCP);
	DLOG (tsap_log, LLOG_TRACE,
	    ("t_snd(fd=%d, buf=0x%x, len=%d, %s%s)",
		tb -> tb_fd, d, cc,
		(expedited ? "EXPEDITED, " : ""),
		(more ? "MORE" : "NO_MORE")));
	ret = t_snd (tb -> tb_fd, d, cc, flags | (more ? T_MORE : 0));
	if (bp != NULLCP) /* NB. bp is a market to free 'd'! */
	    free(d);
	if (ret == NOTOK) {
	    if (t_errno == TLOOK && t_look(tb -> tb_fd) == T_DISCONNECT)
		(void) tp4getdis(tb -> tb_fd, td);
	    else
		(void) tsaplose (td, DR_CONGEST, "failed", "sendmsg");
	    freetblk (tb);

	    return NOTOK;
	}
    }

done: ;
#ifdef  MGMT
    if (tb -> tb_manfnx)
	(*tb -> tb_manfnx) (USERDT, tb, dlen);
#endif

    return OK;
}

/*  */

static int  TDrain (tb, td)
register struct tsapblk *tb;
struct TSAPdisconnect *td;
{
    int	    nc,
	    onoff,
	    result;
    register struct qbuf *qb;

    if ((onoff = fcntl (tb -> tb_fd, F_GETFL, 0)) != NOTOK)
	(void) fcntl (tb -> tb_fd, F_SETFL, onoff | FNDELAY);


    while ((qb = tb -> tb_qwrites.qb_forw) != &tb -> tb_qwrites) {

	if ((nc = t_snd (tb -> tb_fd, qb -> qb_data, qb -> qb_len, 0)) !=
	    qb -> qb_len) {
	    if (nc == NOTOK) {
		if (t_errno == TLOOK && t_look(tb -> tb_fd) == T_DISCONNECT) {
		    result = tp4getdis(tb -> tb_fd, td);
		    goto out;
		} else if (t_errno != TFLOW) {
		    result = tsaplose (td, DR_NETWORK, "failed",
				      "write to network");
		    goto out;
		}

		nc = 0;
	    }

	    qb -> qb_data += nc, qb -> qb_len -= nc;
	    DLOG (tsap_log, LLOG_TRACE,
		  ("wrote %d of %d octets from blocked write", nc,
		   qb -> qb_len));

	    result = OK;
	    goto out;
	}

	DLOG (tsap_log, LLOG_TRACE,
	      ("finished blocked write of %d octets", qb -> qb_len));
	remque (qb);
	free ((char *) qb);
    }
    result = DONE;

 out: ;
    if (onoff != NOTOK)
	(void) fcntl (tb -> tb_fd, F_SETFL, onoff);

    return result;
}

/*  */

/* ARGSUSED */

static int  TRead (tb, tx, td, async, oob)
register struct tsapblk *tb;
register struct TSAPdata *tx;
struct TSAPdisconnect *td;
int	async,
	oob;
{
    int	    cc;
    register struct qbuf *qb;
    static struct qbuf *spare_qb = 0;
    int	flags;

    bzero ((char *) tx, sizeof *tx);
    tx -> tx_qbuf.qb_forw = tx -> tx_qbuf.qb_back = &tx -> tx_qbuf;

    for (;;) {
	qb = NULL;
	if (spare_qb) {
	    if (spare_qb -> qb_len >= tb -> tb_tsdusize)
		qb = spare_qb;
	    else
		free ((char *)spare_qb);
	    spare_qb = NULL;
	}
	if (qb == NULL && (qb = (struct qbuf *)
	    malloc ((unsigned) (sizeof *qb + tb -> tb_tsdusize))) == NULL)
	{
	    (void) tsaplose (td, DR_CONGEST, NULLCP, NULLCP);
	    break;
	} else
	    qb -> qb_len = tb -> tb_tsdusize;
	qb -> qb_data = qb -> qb_base;

	if ((cc = t_rcv(tb -> tb_fd, qb -> qb_data, qb -> qb_len, &flags))
	    == NOTOK) {
	    switch (t_errno) {
	    case TNODATA:
		td -> td_cc = 0;
		break;
	    case TLOOK:
		switch (t_look (tb -> tb_fd)) {
		case T_DISCONNECT:
		    (void) tp4getdis(tb -> tb_fd, td);
		    break;
		default:
		    (void) tsaplose (td, DR_CONGEST, NULLCP,
				     "unexpected event %d",
				     t_look(tb -> tb_fd));
		    break;
		}
		goto out;
	    default:
		(void) tli_lose (td, NOTOK, DR_CONGEST, "t_rcv");
		break;
	    }
	    break;
	}
	DLOG (tsap_log, LLOG_DEBUG,
	    ("t_rcv(fd=%d, buf=0x%x, len=%d, flags=0x%x) ret=%d",
	    tb -> tb_fd, qb -> qb_data, qb -> qb_len, flags, cc));
	if (flags & T_EXPEDITED) {
	    if (cc > 0) {
		register struct qbuf *qb2 = tx -> tx_qbuf.qb_back;

		/* assume ETSDU will always be less than MAXTP4 */
		if (qb2 != &tx->tx_qbuf) {
		    bcopy(qb -> qb_data, qb2 -> qb_len + qb2 -> qb_data, cc);
		    tx -> tx_cc = (qb2 -> qb_len += cc);
		    (spare_qb = qb) -> qb_len = tb -> tb_tsdusize;
		} else {
		    insque (qb, qb2);
		    tx -> tx_cc = (qb -> qb_len = cc);
		}
	    } else
		(spare_qb = qb) -> qb_len = tb -> tb_tsdusize;

	    /* would need a pretty wierd implementation, but possible */
	    if (flags & T_MORE)
		continue;
	    tx -> tx_expedited = 1;

	    return OK;
	} else
	    tx -> tx_expedited = 0;

	tb -> tb_len += (qb -> qb_len = cc);
	if (cc > 0) {
	    register struct qbuf *qb2 = tb -> tb_qbuf.qb_back;

	    if (qb2 != &tb->tb_qbuf && qb2->qb_len + cc <= tb->tb_tsdusize) {
		bcopy(qb -> qb_data, qb2 -> qb_len + qb2 -> qb_data, cc);
		qb2 -> qb_len += cc;
		(spare_qb = qb) -> qb_len = tb -> tb_tsdusize;
	    } else
		insque (qb, qb2);
	} else
	    (spare_qb = qb) -> qb_len = tb -> tb_tsdusize;

#ifdef	MGMT
	if (tb -> tb_manfnx)
	    (*tb -> tb_manfnx) (USERDR, tb, tb -> tb_len);
#endif
	if (flags & T_MORE) {
	    if (async)
		return DONE;
	    continue;
	}
	if (tb -> tb_qbuf.qb_forw != &tb -> tb_qbuf) {
	    tx -> tx_qbuf = tb -> tb_qbuf; /* struct copy */
	    tx -> tx_qbuf.qb_forw -> qb_back =
		tx -> tx_qbuf.qb_back -> qb_forw = &tx -> tx_qbuf;
	    tx -> tx_cc = tb -> tb_len;
	    tb -> tb_qbuf.qb_forw =
		tb -> tb_qbuf.qb_back = &tb -> tb_qbuf;
	    tb -> tb_len = 0;
	}

	return OK;
    }
 out:;
    if (qb)
	free ((char *) qb);

    freetblk (tb);

    return NOTOK;
}

/*  */

static int  TDisconnect (tb, data, cc, td)
register struct tsapblk *tb;
char   *data;
int	cc;
struct TSAPdisconnect *td;
{
    int	    result = OK;
    struct t_call *call;

    if ((call = (struct t_call *)t_alloc (tb -> tb_fd, T_CALL, T_ALL)) == NULL)
	result = tli_lose (td, NOTOK, DR_CONGEST, "t_alloc");
    else {
	if (cc)
	    bcopy (data, call -> udata.buf, call -> udata.len = (unsigned)cc);
	call -> sequence = tb -> tb_flags & TB_CONN ? -1 : tb -> tb_seq;
	if (t_snddis (tb -> tb_fd, call) == NOTOK)
	    result = tli_lose (td, NOTOK, DR_CONGEST, "t_snddis");
	t_free ((char *)call, T_CALL);
    }
    freetblk (tb);

    return result;
}
/*  */

/* ARGSUSED */

static int  TLose (tb, reason, td)
register struct tsapblk *tb;
int	reason;
struct TSAPdisconnect *td;
{
    struct t_call *call;

    SLOG (tsap_log, LLOG_EXCEPTIONS, NULLCP, ("TPM error %d", reason));
    
    if ((call = (struct t_call *)t_alloc (tb -> tb_fd, T_CALL, T_ALL)) == NULL)
	SLOG (tsap_log, LLOG_EXCEPTIONS, "TLose",
              ("unable to allocate"));
    else {
	call -> sequence = tb -> tb_flags & TB_CONN ? -1 : tb -> tb_seq;
	if (t_snddis (tb -> tb_fd, call) == NOTOK)
	     SLOG (tsap_log, LLOG_EXCEPTIONS, "t_snddis",
		   ("unable to send"));

	t_free ((char *)call, T_CALL);
    }
}

/*    LOWER HALF */

/* ARGSUSED */

int	tp4open (tb, local_ta, local_na, remote_ta, remote_na, td, async)
register struct tsapblk *tb;
struct TSAPaddr *local_ta,
		*remote_ta;
struct NSAPaddr *local_na,
		*remote_na;
struct TSAPdisconnect *td;
int	async;
{
    int	    fd,
	    onoff;
    struct TSAPaddr tzs;
    register struct TSAPaddr *tz = &tzs;
    register struct NSAPaddr *nz = tz -> ta_addrs;

    bzero ((char *) tz, sizeof *tz);
    if (local_ta)
	*tz = *local_ta;	/* struct copy */
    if (local_na) {
	*nz = *local_na;	/* struct copy */
	tz -> ta_naddr = 1;
    } 

#ifdef NOT_ANY_MORE
    else {
	struct TSAPaddr *ntz;
	ntz = str2taddr ("NSAP=38826110000210010100599000");
					/* should get from isotailor */
	*nz = *ntz -> ta_addrs;             /* struct copy */
    }
#endif

    if ((fd = tp4bind(tz, 0, td, TLI_CTX_CALL)) == NOTOK)
	return NOTOK;
    tb -> tb_fd = fd;

    if (tp4info(tb, td) == NOTOK) {
	t_close(fd);
	tb -> tb_fd = NOTOK;
	return NOTOK;
    }

    (void) tp4init (tb);

    if (async)
	if ((onoff = fcntl (fd, F_GETFL, 0)) != NOTOK)
	    (void) fcntl (fd, F_SETFL, onoff | FNDELAY);

    return (async ? OK : DONE);
}

/*  */

/* ARGSUSED */

static int  retry_tp4_socket (tb, td)
register struct tsapblk *tb;
struct TSAPdisconnect *td;
{
    fd_set  mask;

    FD_ZERO (&mask);
    FD_SET (tb -> tb_fd, &mask);
    if (xselect (tb -> tb_fd + 1, &mask, NULLFD, NULLFD, 0) < 1)
	return OK;

    return DONE;
}



/*  */

/* ARGSUSED */

/*
 * Save string format:
 *	"fd.connection-sequence-number.expedited calling-address called"
 *	better not get any spaces in result of taddr2str().
 */

char   *tp4save (fd, seq, exp, calling_ta, called_ta, td)
int fd, seq, exp;
struct tsapADDR *calling_ta, *called_ta;
struct TSAPdisconnect *td;
{
    struct TSAPaddr calling, called;
    static char buffer[BUFSIZ];

    copyTSAPaddrX(called_ta, &called);
    copyTSAPaddrX(calling_ta, &calling);
    (void) sprintf (buffer, "%c%d.%d.%d %s %s", NT_TLI, fd, seq, exp,
	taddr2str(&calling), taddr2str(&called));
    return buffer;
}

/*  */

int	tp4restore (tb, buffer, td)
register struct tsapblk *tb;
char   *buffer;
struct TSAPdisconnect *td;
{
    int	    fd, exp;
    char calling_buf[BUFSIZ];
    char called_buf[BUFSIZ];

    DLOG (tsap_log, LLOG_DEBUG, ("tp4restore: \"%s\"", buffer));
    if (sscanf (buffer, "%d.%d.%d %s %s",
	    &fd, &tb -> tb_seq, &exp,
	    calling_buf, called_buf) != 5 || fd < 0)
	return tsaplose (td, DR_PARAMETER, NULLCP,
			"bad initialization vector \"%s\"", buffer);

    tb -> tb_fd = fd;

#ifdef DEBUG

    /* loopback HACK */

    if ( strcmp (calling_buf,"NULLPA") == 0)
	    bcopy (called_buf, calling_buf, strlen (called_buf));

#endif

    copyTSAPaddrY(str2taddr(calling_buf), &tb -> tb_initiating);
    copyTSAPaddrY(str2taddr(called_buf), &tb -> tb_responding);
    t_sync (fd);
    if (tp4info(tb, td) == NOTOK) {
	return NOTOK;
    }
    if (exp)
	tb -> tb_flags |= TB_EXPD;
    else
	tb -> tb_flags &= ~TB_EXPD;
    (void) tp4init (tb);
    return OK;
}

/*  */

int	tp4init (tb)
register struct tsapblk *tb;
{

    tb -> tb_connPfnx = TConnect;
    tb -> tb_retryPfnx = TRetry;

    tb -> tb_startPfnx = TStart;
    tb -> tb_acceptPfnx = TAccept;

    tb -> tb_writePfnx = TWrite;
    tb -> tb_readPfnx = TRead;
    tb -> tb_discPfnx = TDisconnect;
    tb -> tb_losePfnx = TLose;

    tb -> tb_drainPfnx = TDrain;

#ifdef  MGMT
    tb -> tb_manfnx = TManGen;
#endif

    tb -> tb_flags &= ~TB_STACKS;
    tb -> tb_flags |= TB_TP4;

    tb -> tb_retryfnx = retry_tp4_socket;

    tb -> tb_closefnx = close_tp4_socket;
    tb -> tb_selectfnx = select_tp4_socket;
}

/*  */

/* ARGSUSED */

int	start_tp4_server (local_ta, backlog, opt1, opt2, td)
struct TSAPaddr *local_ta;
int	backlog,
	opt1,
	opt2;
struct TSAPdisconnect *td;
{
    int	    sd;

    if ((sd = tp4bind (local_ta, 100, td, TLI_CTX_LISTEN)) == NOTOK)
	return NOTOK;

    return sd;
}

/*  */

int	join_tp4_client (fd, remote_ta, ud, ccp, seqp, expdp, td)
int	fd;
struct TSAPaddr *remote_ta;
char	*ud;
int	*ccp;
int	*seqp;
int	*expdp;
struct TSAPdisconnect *td;
{
    struct t_call *call;
    int sd;

    *ccp = 0;
    if ((call = (struct t_call *)t_alloc (fd, T_CALL, T_ALL)) == NULL)
	return tli_lose (td, NOTOK, DR_CONGEST, "t_alloc");

    DLOG (tsap_log, LLOG_DEBUG, ("t_listen: %d", fd));
    if (t_listen (fd, call) == NOTOK) {
	DLOG (tsap_log, LLOG_DEBUG,
	    ("t_listen fail: t_errno=%d, t_look=%d", t_errno, t_look(fd)));
	t_free ((char *)call, T_CALL);
	if (t_errno == TLOOK && t_look(fd) == T_DISCONNECT)
	    return tp4getdis(fd, td);
	else
	    return tli_lose (td, NOTOK, DR_CONGEST, "t_listen");
    }
    (void) tp42gen (remote_ta, &call -> addr);
    *seqp = call -> sequence;
    if ((*ccp = call -> udata.len) > 0)
	bcopy (call -> udata.buf, ud, *ccp);
#ifdef RTnet_R02
    if (call -> opt.len >= OPTS_SIZE) {
	struct opts * opts = (struct opts *) call -> opt.buf;
	*expdp = (opts -> class & OPT_EXPEDITED) ? 1 : 0;
    } else
	*expdp = 0;
#else
    *expdp = 0;
#endif
    t_free ((char *)call, T_CALL);
    sd = dup (fd);
    t_sync(sd);
    DLOG (tsap_log, LLOG_TRACE,
	("t_listen: CON-IND from %s now on %d", taddr2str(remote_ta), sd));
    return sd;
}

/*  */

/*
 *	Default is a pretty common format for TLI addresses:
 *		Tttt...Nnnn...
 *	where T is the Transport Selector length and ttt... is the
 *	selector and N is the NSAP Address length and nnn... is the
 *	address.
 *
 *	Your format may be different - take the ICL_TLI case for example...
 */

/* ARGSUSED */
int	gen2tp4 (generic, specific, context) /* dependant on Addressing */
struct TSAPaddr *generic;
struct netbuf *specific;
int context;
{
	char *cp;

#ifdef ICL_TLI

	int i;
	struct NSAPaddr *na;
	int space;
	char * prefix;
   	
	space = specific -> maxlen;

	cp = specific -> buf;

#ifdef DEBUG

	/* If NSAP in debug mode, force same naddr */

	if (generic -> ta_naddr > 0) 
		context = TLI_CTX_CALL;

#endif

	if (context == TLI_CTX_LISTEN)
		prefix = icl_tli_responder;
	else
		prefix = icl_tli_initiator;

	i = strlen (prefix);
	space -= i + 1;
	if (space < 0) {
	    out_space:;
		LLOG (tsap_log, LLOG_EXCEPTIONS, ("gen2tp4: out of buffer space"));
		return NOTOK;
	}
    
	(void) strcat (cp, prefix);
	cp += strlen(cp);
	*cp++ = 0x0;

	if (generic -> ta_selectlen) {

		space -= (int) ICL_TSEL_LEN + 1;
		if (space < 0)
			goto out_space;

		*cp++ =	ICL_TSEL_MARKER;
		*cp++ = ICL_TSEL_LEN;
		*cp++ = (char)(generic -> ta_selectlen);

		bcopy (generic -> ta_selector, cp, generic -> ta_selectlen);
		cp += generic -> ta_selectlen;

		for (i = generic -> ta_selectlen + 1 ; i < (int) ICL_TSEL_LEN; i++)
			*cp++ = 0x0;
	}

	if (generic -> ta_naddr > 0) {

		na = generic -> ta_addrs;

#ifndef DEBUG

		/* Useful to have a NSAP in debug mode for loopback test */

		if (context == TLI_CTX_LISTEN) {
			LLOG (tsap_log, LLOG_EXCEPTIONS, ("Can't specify NSAP on listener"));
			return NOTOK;
		}
#endif

		if (na -> na_stack == NA_NSAP) {

			space -= (int) ICL_NSAP_LEN + 1;
			if (space < 0)
				goto out_space;

			*cp++ = ICL_NSAP_MARKER;
			*cp++ = ICL_NSAP_LEN;

			*cp++ = (char)(na -> na_addrlen);
			if (na -> na_addrlen) {
				bcopy (na -> na_address, cp, na -> na_addrlen);
				cp += na -> na_addrlen;
			}

			for (i = na -> na_addrlen +1 ; i < (int) ICL_NSAP_LEN; i++)
				*cp++ = 0x0;


#ifdef ADD_DTE2NSAP

			if (context != TLI_CTX_LISTEN && na) {
				struct NSAPaddr *getisosnpa();
				struct NSAPaddr *dte_nsap;

				/* On failure - carry on, the NSAP may work on its own */

				if ((dte_nsap = getisosnpa (na)) == NULLNA) 
					LLOG (tsap_log, LLOG_EXCEPTIONS, ("ts2tli: Can't find DTE"));
				else if (dte_nsap -> na_stack != NA_X25)
					LLOG (tsap_log, LLOG_EXCEPTIONS, ("ts2tli: No DTE in mapping"));
				else if (dte_nsap -> na_dtelen <= 0)
					LLOG (tsap_log, LLOG_EXCEPTIONS, ("ts2tli: DTE zero length"));
				else {
					int dlen;

					space -= (int) ICL_DTE_LEN + 1;
					if (space < 0)
						goto out_space;

					*cp++ = ICL_DTE_MARKER;
					*cp++ = ICL_DTE_LEN;
					*cp++ = (char)(dte_nsap -> na_dtelen);
					char2bcd (dte_nsap -> na_dte, dte_nsap -> na_dtelen, cp);
					dlen = dte_nsap -> na_dtelen / 2;
					cp += dlen;

					for (i = dlen +1 ; i < (int) ICL_DTE_LEN ; i++)
						*cp++ = 0x0;
				}
			}

#endif

		} else if (na -> na_stack == NA_X25) {

			int dlen;

			space -= (int) ICL_DTE_LEN + 1;
			if (space < 0)
				goto out_space;

			*cp++ = ICL_DTE_MARKER;
			*cp++ = ICL_DTE_LEN;
			*cp++ = (char)(na -> na_dtelen);
			char2bcd (na -> na_dte, na -> na_dtelen, cp);
			dlen = na -> na_dtelen / 2;
			cp += dlen;

			for (i = dlen +1 ; i < (int) ICL_DTE_LEN ; i++)
				*cp++ = 0x0;
		} else {
			LLOG (tsap_log, LLOG_EXCEPTIONS, ("TLI: invalid stack"));
			return NOTOK;
		}
	}
	specific -> len = (unsigned int) (cp - specific -> buf);

#ifdef HEAVY_DEBUG
{
	static char hex[] = "0123456789abcdef";
	char buffer [1024];
	char *p, *s;

	DLOG (tsap_log, LLOG_TRACE, ("maxlen %d, len %d\n",
				     specific -> maxlen,
				     specific -> len));
	p = specific -> buf;
	s = buffer;
	for (i=0; i < specific -> len; i++) {
		*s++ = hex [((*p & 255)/16) % 16];
		*s++ = hex [(*p++ & 255) % 16];
		*s++ = ' ';
	}
	*s = 0x0;

	DLOG (tsap_log, LLOG_TRACE, ("%s",buffer));
}
#endif				/* HEAVY_DEBUG */

	return OK;

#else				/* ICL_TLI */

	cp = specific -> buf;

	specific -> len = 2;	/* minimum */

	*cp++ = (char)(generic -> ta_selectlen);
	if (generic -> ta_selectlen) {
		if ((specific -> len += generic -> ta_selectlen) > specific -> maxlen)
			return NOTOK;
		bcopy (generic -> ta_selector, cp, generic -> ta_selectlen);
		cp += generic -> ta_selectlen;
	}

	if (generic -> ta_naddr > 0) {
		register struct NSAPaddr *na = generic -> ta_addrs;

		*cp++ = (char)(na -> na_addrlen);
		if (na -> na_addrlen) {
			specific -> len += na -> na_addrlen;
			if ((specific -> len += generic -> ta_selectlen) >
			    specific -> maxlen)
				return NOTOK;
			bcopy (na -> na_address, cp, na -> na_addrlen);
			cp += na -> na_addrlen;
		}
	} else
		*cp = '\0';	/* NOTE: put in zero-length NSAP address */

	return OK;

#endif
}

/*  */

int	tp42gen (generic, specific)
struct TSAPaddr *generic;
struct netbuf *specific;
{
	register char *cp;
	register struct NSAPaddr *na = generic -> ta_addrs;

#ifdef ICL_TLI

	int len = 1;
	int flen;
	int alen;
	char sel;
	char *p;
	char got_nsap = 0;

	bzero ((char *) generic, sizeof *generic);
	bzero ((char *) na, sizeof *na);

	if (specific -> len < 2)
		return OK;
	cp = specific -> buf;

	/* Skip to past NULL */
	while (*cp++ != 0x0)
		len++;

	while (len < specific -> len) {
		sel = *cp++;
		len += 2;
		flen = *cp++;
		p = cp;

		switch ((unsigned char)sel) {
		    case ICL_TSEL_MARKER:
			DLOG (tsap_log, LLOG_TRACE, ("tsel supplied"));
			generic -> ta_selectlen = *cp++;
			bcopy (cp, generic -> ta_selector, generic -> ta_selectlen);
			break;
		    case ICL_NSAP_MARKER:
			DLOG (tsap_log, LLOG_TRACE, ("nsap supplied"));
			na -> na_stack = NA_NSAP;
			na -> na_community = ts_comm_nsap_default;
			generic -> ta_naddr++;
			na -> na_addrlen = *cp++;
			bcopy (cp, na -> na_address, na -> na_addrlen);
			got_nsap = 1;
			break;
		    case ICL_DTE_MARKER:
			DLOG (tsap_log, LLOG_TRACE, ("DTE supplied"));
			if (got_nsap)
				break;
			na -> na_stack = NA_X25;
			na -> na_community = ts_comm_x25_default;
			generic -> ta_naddr++;
			na -> na_dtelen = *cp++;
			bcd2char (cp, na -> na_dte, na -> na_dtelen);
			break;
		    case ICL_CUG_MARKER: /* CUG ! */
			DLOG (tsap_log, LLOG_TRACE, ("cug supplied"));
			break;
		    case ICL_PORT_MARKER: /* PORT */
			DLOG (tsap_log, LLOG_TRACE, ("port supplied"));
			break;
		    default:
			/* Does it matter - we've got what we need */
			DLOG (tsap_log, LLOG_NOTICE, ("unknown address selector %x", sel));
			break;
		}
		cp = p + flen;
		len += flen;
	}

#else

	bzero ((char *) generic, sizeof *generic);
	bzero ((char *) na, sizeof *na);

	if (specific -> len < 2)
		return OK;
	cp = specific -> buf;

	if (generic -> ta_selectlen = *cp++) {
		if (specific -> len < generic -> ta_selectlen + 1)
			return NOTOK;
		bcopy(cp, generic -> ta_selector, generic -> ta_selectlen);
		cp += generic -> ta_selectlen;
	}

	if (specific -> len > generic -> ta_selectlen + 1) {
		na -> na_stack = NA_NSAP;
		na -> na_community = ts_comm_nsap_default;
		generic -> ta_naddr++;
		if (na -> na_addrlen = *cp++) {
			if (specific->len < (generic->ta_selectlen + na->na_addrlen + 2))
				return NOTOK;
			bcopy (cp, na -> na_address, na -> na_addrlen);
			cp += na -> na_addrlen;
		}
	}
#endif

	return OK;
}

int close_tp4_socket (fd)
int	fd;
{
    DLOG (tsap_log, LLOG_TRACE, ("close_tp4_socket(%d)", fd));
    return t_close (fd);
}

#else
int	_ts2tli_stub () {;}
#endif
