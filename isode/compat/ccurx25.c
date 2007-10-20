/* ccurx25.c - X.25 abstractions for Concurrent RTnet-X25 */

#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/compat/RCS/ccurx25.c,v 9.0 1992/06/16 12:07:00 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/compat/RCS/ccurx25.c,v 9.0 1992/06/16 12:07:00 isode Rel $
 *
 * Contributed by Alan Young, Concurrent Computer Corporation
 *
 * This interface should need little adaption for use with any generic
 * service supporting the AT&T Network Provider Interface.
 *
 *
 * $Log: ccurx25.c,v $
 * Revision 9.0  1992/06/16  12:07:00  isode
 * Release 8.0
 *
 */

/*
 *                                NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */


/* LINTLIBRARY */

#include <errno.h>
#include <stdio.h>
#include "general.h"
#include "manifest.h"
#include "tailor.h"
#include "tpkt.h"

/*   CCUR RTnet-X25 R01 */

#ifdef  X25
#ifdef  CCUR_X25

#include "x25.h"
#include <stropts.h>
#include <fcntl.h>

typedef union N_indication_t {
    N_bind_ack_t	bind;
    N_conn_con_t	conn_con;
    N_conn_ind_t	conn_ind;
    N_data_req_t	data;
    N_discon_ind_t	disc;
    N_error_ack_t	error;
    N_exdata_ind_t	exdata;
    N_info_ack_t	info;
    N_ok_ack_t		ok;
    N_reset_con_t	reset_con;
    N_reset_ind_t	reset_ind;
} N_indication_t;

#ifndef X25_DEV
#define X25_DEV "/dev/x25piuser"
#endif

#define N_FACILITIES_SIZE 0
#define N_CHUNK_SIZE 1024	/* Cannot write single chunks greater than
				   this and must write a single NSDU in
				   multiples of this, or of the NPDU size
				   (which is always less than or equal).
				   This could be determined by doing an
				   N_INFO_REQ (the NIDU_size field), or the
				   NPDU size could be extracted from the
				   facilities on a connection
				   indication/confirm, but where would we
				   store such information?  We hardly want
				   to do an N_INFO_REQ before every write!
				*/
#define CONTROL_BUF_SIZE 1024

static fd_set	inprogress;


char * npierr2str(n)
int n;
{
    static char buf[20];
    switch (n) {
	case NBADADDR: return "invalid address";
	case NBADOPT: return "invalid options";
	case NACCESS: return "no permission";
	case NBADQOSPARAM:
	case NBADQOSTYPE: return "invalid QOS";
	case NBADTOKEN: return "invalid token";
	case NNOADDR: return "unable to allocate address";
	case NOUTSTATE: return "invalid state";
	case NBADSEQ: return "invalid sequence number";
	case NBADCONNFLAG:
	case NBADFLAG: return "invalid flag";
	case NTOOMUCHCUD:
	case NBADDATA: return "invalid data size";
	case NBADOFFLENGTH:
	case NBADSIZE: return "invalid message size";
	case NFACFAILSPACE:
	case NFACFAILELMT:
	case NFACELMTSIZE:
	case NFACFLDSIZE:
	case NFACTOOBIG: return "invalid facilities";
	case NFACWINSIZE: return "invalid window";
	case NFACPKTSIZE: return "invalid packet size";
	case NBADDESTLENGTH:
	case NBADDESTOFF:
	case NBADRESLENGTH: return "bad address size/offset";
	case NNOLINE: return "line not up";
	case NBOUND: return "address in use";
	case NBADCONINDNUM: return "listen concurrency too large";
	case NBADEXPWIN: return "expedited window closed";
	case NSYSERR: return "";
	case NNOTSUPPORT: return "unsupported primitive";
	default: (void) sprintf(buf, "NPI error %d", n); return buf;
    }
}


char *npiev2str(n)
int n;
{
    static char buf[10];

    switch (n) {
    case N_CONN_REQ: return "N_CONN_REQ";
    case N_CONN_RES: return "N_CONN_RES";
    case N_DISCON_REQ: return "N_DISCON_REQ";
    case N_DATA_REQ: return "N_DATA_REQ";
    case N_EXDATA_REQ: return "N_EXDATA_REQ";
    case N_INFO_REQ: return "N_INFO_REQ";
    case N_BIND_REQ: return "N_BIND_REQ";
    case N_UNBIND_REQ: return "N_UNBIND_REQ";
    case N_UNITDATA_REQ: return "N_UNITDATA_REQ";
    case N_OPTMGMT_REQ: return "N_OPTMGMT_REQ";
    case N_CONN_IND: return "N_CONN_IND";
    case N_CONN_CON: return "N_CONN_CON";
    case N_DISCON_IND: return "N_DISCON_IND";
    case N_DATA_IND: return "N_DATA_IND";
    case N_EXDATA_IND: return "N_EXDATA_IND";
    case N_INFO_ACK: return "N_INFO_ACK";
    case N_BIND_ACK: return "N_BIND_ACK";
    case N_ERROR_ACK: return "N_ERROR_ACK";
    case N_OK_ACK: return "N_OK_ACK";
    case N_UNITDATA_IND: return "N_UNITDATA_IND";
    case N_UDERROR_IND: return "N_UDERROR_IND";
    case N_DATACK_REQ: return "N_DATACK_REQ";
    case N_DATACK_IND: return "N_DATACK_IND";
    case N_RESET_REQ: return "N_RESET_REQ";
    case N_RESET_IND: return "N_RESET_IND";
    case N_RESET_RES: return "N_RESET_RES";
    case N_RESET_CON: return "N_RESET_CON";
    case N_UNREADABLE: return "N_UNREADABLE";
    default:
	(void) sprintf(buf, "%d", n);
	return buf;
    }
}


int 	get_prim(fd, control, data, flags, expected, size, close_fd)
int		fd;
struct strbuf	*control, *data;
int *		flags;
int		expected, size, close_fd;
{
    N_error_ack_t * err;
    int res;

    errno = 0;
    if ((res = getmsg (fd, control, data, (*flags=0, flags))) == NOTOK) {
	SLOG (compat_log, LLOG_EXCEPTIONS, "failed",
	    ("getmsg expecting %s", npiev2str(expected)));
	if (close_fd != NOTOK)
	    (void) close(close_fd);
	return NOTOK;
    }
    if (control->len < (int)sizeof(err->PRIM_type)) {
	SLOG (compat_log, LLOG_EXCEPTIONS, "insufficient control data",
	    ("getmsg"));
	if (close_fd != NOTOK)
	    (void) close(close_fd);
	return NOTOK;
    }
    err = (N_error_ack_t *)control->buf;
    if (res & MORECTL) {
	SLOG (compat_log, LLOG_EXCEPTIONS, "too much control data",
	    ("getmsg: %s", npiev2str(err -> PRIM_type)));
	if (close_fd != NOTOK)
	    (void) close(close_fd);
	return NOTOK;
    }
    if (err -> PRIM_type == expected && control->len >= size)
	return OK;
    if (err -> PRIM_type == N_ERROR_ACK
	&& control->len >= (int)sizeof(N_error_ack_t))
    {
	errno = err->UNIX_error;
	SLOG (compat_log, LLOG_EXCEPTIONS,
	    npierr2str(err->NPI_error),
	    ("getmsg (expected %s)", npiev2str(expected)));
	if (close_fd != NOTOK)
	    (void) close(close_fd);
	return NOTOK;
    }
    SLOG (compat_log, LLOG_EXCEPTIONS, "unexpected event",
	    ("getmsg: %s", npiev2str(err -> PRIM_type)));
    if (close_fd != NOTOK)
	(void) close(close_fd);
    return NOTOK;
}


int     start_x25_client (local)
struct NSAPaddr *local;
{
    int     sd;

    /* should use local address to see if sub-address on local bind
       but cannot bind now as need to know remote address to determin
       which line to use. */

    if (local != NULLNA)
	local -> na_stack = NA_X25, local -> na_community = ts_comm_x25_default;
    if ((sd = open (X25_DEV, O_RDWR)) == NOTOK) {
	SLOG (compat_log, LLOG_EXCEPTIONS, "failed", ("open"));
	return NOTOK; /* Error can be found in errno */
    }

    return sd;
}


int     start_x25_server (local, backlog, opt1, opt2)
struct NSAPaddr *local;
int     backlog,
	opt1,
	opt2;
{
    int     sd;
    int	    flags;
    struct {
	N_bind_req_t	bind;
	CONN_DB		addr;
    } bind;
    char pbuf[CONTROL_BUF_SIZE];
    N_bind_ack_t *	ack;
    struct strbuf control, data;

    if ((sd = open (X25_DEV, O_RDWR)) == NOTOK) {
	SLOG (compat_log, LLOG_EXCEPTIONS, "failed", ("open"));
	return NOTOK; /* Can't get an X.25 socket */
    }

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

    bzero((char *)&bind, sizeof(bind));
    (void) gen2if (local, &bind.addr, ADDR_LISTEN);
    bind.bind.PRIM_type = N_BIND_REQ;
    if (bind.addr.addr.DTE_MAC.lsap_len == 0 && bind.addr.cud.len == 0)
	bind.bind.BIND_flags = DEFAULT_LISTENER;;
    bind.bind.ADDR_length = sizeof(bind.addr.addr);
    bind.bind.ADDR_offset = sizeof(bind.bind);
    bind.bind.CONIND_number = 100; /* something big */
    control.len = sizeof(bind.addr.addr) + sizeof(bind.bind);
    control.buf = (char *)&bind;
    if (data.len = bind.addr.cud.len)
	data.buf = bind.addr.cud.buf;
    DLOG (compat_log, LLOG_DEBUG,
	("bind: dtelen=%d, cudlen=%d%s",
	    bind.addr.addr.DTE_MAC.lsap_len, bind.addr.cud.len,
	    bind.bind.BIND_flags & DEFAULT_LISTENER ? ", default": ""));

    if (putmsg (sd, &control, data.len ? &data : 0, 0) == NOTOK) {
	SLOG (compat_log, LLOG_EXCEPTIONS, "failed", ("bind:putmsg"));
	(void) close(sd);
	return NOTOK;
    }

    control.maxlen = sizeof(pbuf);
    control.buf = pbuf;
    if (get_prim(sd, &control, 0, &flags, N_BIND_ACK,
	    (int)sizeof(N_bind_ack_t), sd) == NOTOK)
	return NOTOK;

    ack = (N_bind_ack_t *)pbuf;
    if (ack->CONIND_number == 0)
    {
	SLOG (compat_log, LLOG_EXCEPTIONS, "unable to listen", ("bind"));
	(void) close(sd);
	return NOTOK;
    }

    /* maybe extract listening address here ?? */
    if (!local->na_dtelen) {
	local->na_dtelen = 1;
	local->na_dte[0] = '0';
    }

    return sd;
}


int     join_x25_client (fd, remote, local)
int     fd;
struct NSAPaddr *remote, *local;
{
    int			flags;
    char		pbuf[CONTROL_BUF_SIZE];
    char		dbuf[128];
    struct {
	N_bind_req_t	bind;
	CONN_DB		addr;
    } bind;
    N_bind_ack_t *	ack;
    N_conn_ind_t	ind;
    N_conn_res_t 	res;
    struct strbuf control, data;
    int     nfd;

    /* get connection indication */
    control.maxlen = sizeof(pbuf);
    control.buf = pbuf;
    data.maxlen = sizeof(dbuf);
    data.buf = dbuf;
    if (get_prim(fd, &control, &data, &flags, N_CONN_IND,
	    (int)sizeof(N_conn_ind_t), NOTOK) == NOTOK)
	return NOTOK;
    ind = *((N_conn_ind_t *)pbuf);
    /* extract calling address */
    if (ind.SRC_length) {
	CONN_DB	addr;

	addr.addr = *((N_X25_addr_t *)&pbuf[ind.SRC_offset]);
	addr.cud.len = 0;
	(void) if2gen (remote, &addr, ADDR_REMOTE);
    } else {
	bzero(remote, sizeof(remote));
	remote -> na_stack = NA_X25;
	remote -> na_community = ts_comm_x25_default;
    }
    if (ind.DEST_length) {
	CONN_DB	addr;

	addr.addr = *((N_X25_addr_t *)&pbuf[ind.DEST_offset]);
	(void) if2gen (local, &addr, ADDR_LOCAL);
    }

    /* get & bind fd to accept on */
    if((nfd = start_x25_client(NULLNA)) == NOTOK)
	return NOTOK;
    bzero((char *)&bind, sizeof(bind));
    bind.bind.PRIM_type = N_BIND_REQ;
    bind.bind.ADDR_length = sizeof(bind.addr.addr);
    bind.bind.ADDR_offset = sizeof(bind.bind);
    bind.bind.CONIND_number = 1;
    bind.bind.BIND_flags = TOKEN_REQUEST;
    control.len = sizeof(bind.bind) + sizeof(bind.addr.addr);
    control.buf = (char *)&bind;
    if (putmsg (nfd, &control, 0, 0) == NOTOK) {
	SLOG (compat_log, LLOG_EXCEPTIONS, "failed", ("bind:putmsg"));
	(void) close(nfd);
	return NOTOK;
    }
    control.maxlen = sizeof(pbuf);
    control.buf = pbuf;
    if (get_prim(nfd, &control, 0, &flags, N_BIND_ACK,
	    (int)sizeof(N_bind_ack_t), nfd) == NOTOK)
	return NOTOK;
    ack = (N_bind_ack_t *)pbuf;

    /* accept connection */
    bzero((char *)&res, sizeof(res));
    res.PRIM_type = N_CONN_RES;
    res.TOKEN_value = ack->TOKEN_value;
    res.SEQ_number = ind.SEQ_number;
    control.len = sizeof(res);
    control.buf = (char *)&res;
    if (putmsg (fd, &control, 0, 0) == NOTOK) {
	SLOG (compat_log, LLOG_EXCEPTIONS, "failed", ("accept:putmsg"));
	(void) close(nfd);
	return NOTOK;
    }
    control.maxlen = sizeof(pbuf);
    control.buf = pbuf;
    if (get_prim(fd, &control, 0, &flags, N_OK_ACK,
	    (int)sizeof(N_ok_ack_t), nfd) == NOTOK)
	return NOTOK;
    return nfd;
}


int     join_x25_server (fd, remote)
int     fd;
struct NSAPaddr *remote;
{
    struct {
	N_conn_req_t req;
	CONN_DB addr;
    }			con;
    struct {
	N_bind_req_t bind;
	CONN_DB	addr;
    }			bind;
    N_bind_ack_t *	ack;
    char		pbuf[CONTROL_BUF_SIZE];
    N_indication_t *	result;
    struct strbuf	control, data;
    int			async, flags, res;
    int *		ip;
    char **		lp;
    char *		line;	/* line name */

    if (FD_ISSET (fd, &inprogress)) {
	FD_CLR (fd, &inprogress);
	goto retry;
    }
	
    if (remote == NULLNA || remote -> na_stack != NA_X25)
    {
	SLOG (compat_log, LLOG_EXCEPTIONS, NULLCP,
	      ("Invalid type na %d", remote->na_stack));
	return NOTOK;
    }

    /* determine line and bind */
    /*
     * This could be expanded to do the bind and con-req inside
     * the loop so as to cope with the possibility of a line being
     * down when multiple lines are connected to the same
     * subnet.  The tailor code (which eliminates duplicate lines
     * for a subnet from this list) would also need to be changed.
     */
    for (ip = x25_communities, lp = x25_lines; *ip; ip++, lp++)
	if (*ip == remote -> na_community)
	    break;
    if (!*ip)
	line = x25_default_line;
    else
	line = *lp;
    if (line == NULLCP) {
	SLOG (compat_log, LLOG_EXCEPTIONS, NULLCP,
	  ("No line for community %d & no default", remote -> na_community));
	return NOTOK;
    }

    if ((async = fcntl (fd, F_GETFL, 0)) == NOTOK)
	return NOTOK;
    if ((async & O_NDELAY) && fcntl (fd, F_SETFL, async & ~O_NDELAY) == NOTOK)
	return NOTOK;

    bzero((char *)&bind, sizeof(bind));
    bind.bind.PRIM_type = N_BIND_REQ;
    bind.bind.ADDR_length = sizeof(bind.addr.addr);
    bind.bind.ADDR_offset = sizeof(bind.bind);
    bind.bind.CONIND_number = 0;
    strncpy(bind.addr.addr.sn_id, line, SNIDSIZE);
    bind.addr.addr.aflags = 1;
    control.len = sizeof(bind.addr.addr) + sizeof(bind.bind);
    control.buf = (char *)&bind;

    if (putmsg (fd, &control, 0, 0) == NOTOK) {
	SLOG (compat_log, LLOG_EXCEPTIONS, "failed", ("bind:putmsg"));
	return NOTOK;
    }

    control.maxlen = sizeof(pbuf);
    control.buf = pbuf;
    if (get_prim(fd, &control, 0, &flags, N_BIND_ACK,
	    (int)sizeof(N_bind_ack_t), NOTOK) == NOTOK)
	return NOTOK;


    bzero((char *)&con, sizeof(con));
    (void) gen2if (remote, &con.addr, ADDR_REMOTE);
    con.req.PRIM_type = N_CONN_REQ;
    con.req.DEST_length = sizeof(con.addr.addr);
    con.req.DEST_offset = sizeof(con.req);
    control.len = sizeof(con.req) + sizeof(con.addr.addr);
    control.buf = (char *)&con;;
    data.len = con.addr.cud.len;
    data.buf = con.addr.cud.buf;
    if (putmsg (fd, &control, data.len ? &data : 0, 0) == NOTOK) {
	SLOG (compat_log, LLOG_EXCEPTIONS, "failed", ("connect:putmsg"));
	return NOTOK;
    }

    if ((async & O_NDELAY) && fcntl (fd, F_SETFL, async) == NOTOK)
	return NOTOK;

retry:
    control.maxlen = sizeof(pbuf);
    control.buf = pbuf;
    errno = ECONNREFUSED; /* just in case */
    if ((res = getmsg (fd, &control, 0, (flags=0, &flags))) == NOTOK
	    || (res & MORECTL)
	    || control.len < (int)sizeof(result->bind.PRIM_type))
    {
	if (res == NOTOK && errno == EAGAIN) {
	    FD_SET (fd, &inprogress);
	    errno = EINPROGRESS;
	    return NOTOK;
	}
	SLOG (compat_log, LLOG_EXCEPTIONS, "failed", ("getmsg:con-cnf"));
	return NOTOK;
    }
    result = (N_indication_t *)pbuf;
    switch(result->bind.PRIM_type) {

    case N_CONN_CON:
	return OK;

    case N_DISCON_IND:
        elucidate_x25_err(1 << RECV_DIAG | 1 << DIAG_TYPE,
			    (char *)&result->disc.DISCON_reason + 2);
	return(NOTOK);

    case N_ERROR_ACK:
	errno = result->error.UNIX_error;
	SLOG (compat_log, LLOG_EXCEPTIONS,
	    npierr2str(result->error.NPI_error), ("getmsg:concnf"));
	return NOTOK;

    default:
	SLOG (compat_log, LLOG_EXCEPTIONS, "failed",
		("getmsg got %d", result->bind.PRIM_type));
	return NOTOK;
    }
}


int     read_x25_socket (fd, buffer, len)
int     fd, len;
char    *buffer;
{
    N_indication_t *	result;
    char pbuf[CONTROL_BUF_SIZE];
    struct strbuf control, data;
    int			async = NOTOK, flags, count = 0, more;


    data.buf = buffer;
    data.maxlen = len;
    control.maxlen = sizeof(pbuf);
    control.buf = pbuf;
    do {
	if ((more = getmsg (fd, &control, &data, (flags=0, &flags))) == NOTOK
	    || (more & MORECTL))
	{
	    if (more == NOTOK && errno == EAGAIN) {
		if (count) {
		    /* better get whole NSDU now */
		    if ((async = fcntl (fd, F_GETFL, 0)) == NOTOK ||
			    fcntl (fd, F_SETFL, async & ~O_NDELAY) == NOTOK)
			return NOTOK;
		    continue;
		} else
		    break;
	    }
	    SLOG (compat_log, LLOG_EXCEPTIONS, "failed", ("getmsg"));
	    return NOTOK;
	}
	DLOG (compat_log, LLOG_DEBUG,
	    ("got ctl.len=%d, data.len=%d, flags=0x%x, more=0x%x",
	     control.len, data.len, flags, more));
	if (data.len == -1)
	    data.len = 0;
	if (control.len >= (int)sizeof(result->bind.PRIM_type)) {
	    result = (N_indication_t *)pbuf;
	    switch(result->bind.PRIM_type) {

	    case N_DATA_IND:
		/* check Q-bit ? */
		if (result->data.DATA_xfer_flags & N_MORE_DATA_FLAG)
		    more |= MOREDATA;
		break;

	    case N_DISCON_IND:
		elucidate_x25_err(1 << RECV_DIAG | 1 << DIAG_TYPE,
				    (char *)&result->disc.DISCON_reason + 2);
		return(NOTOK);

	    case N_ERROR_ACK:
		errno = result->error.UNIX_error;
		SLOG (compat_log, LLOG_EXCEPTIONS,
		    npierr2str(result->error.NPI_error), ("getmsg:concnf"));
		return NOTOK;

	    default:
		SLOG (compat_log, LLOG_EXCEPTIONS, "failed",
			("getmsg got %d", result->bind.PRIM_type));
		return NOTOK;
	    }
	}

	count += data.len;
	data.buf += data.len;
	data.maxlen -= data.len;

    } while (data.maxlen > 0 && (more & MOREDATA));
    DLOG (compat_log, LLOG_DEBUG,
	    ("X25 read, total %d/%d", count, data.maxlen));

    if (async != NOTOK && fcntl (fd, F_SETFL, async) == NOTOK)
	return NOTOK;
    return count;
}


int     write_x25_socket (fd, buffer, len)
int     fd, len;
char    *buffer;
{
    N_data_req_t	req;
    struct strbuf control, data;
    int			total;

    data.buf = buffer;
    control.len = sizeof(req);
    control.buf = (char *)&req;

    req.PRIM_type = N_DATA_REQ;
    total = 0;

    do {
	data.len = len > N_CHUNK_SIZE ? N_CHUNK_SIZE : len;
	req.DATA_xfer_flags = len > N_CHUNK_SIZE ? N_MORE_DATA_FLAG : 0;

	if (putmsg(fd,  &control, &data, 0) == NOTOK) {
	    if (errno == EAGAIN)
		break;
	    return NOTOK;
	}
	len -= data.len;
	data.buf += data.len;
	total += data.len;

    } while (len > 0);
    DLOG (compat_log, LLOG_DEBUG, ("X25 write, total %d/%d", total, len));
    return total;
}

int close_x25_socket(fd)
int fd;
{
    FD_CLR (fd, &inprogress);
    return close(fd);
}

#else
int _ccurx25_stub () {}
#endif  /* CCUR_X25 */
int _ccurx25_stub2 () {}
#endif  /* X25 */
