/* internet.c - TCP/IP abstractions for TLI */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/compat/RCS/internet_tli.c,v 9.0 1992/06/16 12:07:00 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/compat/RCS/internet_tli.c,v 9.0 1992/06/16 12:07:00 isode Rel $
 *
 *
 * $Log: internet_tli.c,v $
 * Revision 9.0  1992/06/16  12:07:00  isode
 * Release 8.0
 *
 */

/*
 * This module contributed by X-Tel Services from work done on behalf
 * of Boldon-James LTD, UK. There contribution to ISODE of this work
 * is gratefully acknowledged.
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


/* LINTLIBRARY */

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include "general.h"
#include "manifest.h"
#include "tailor.h"

/*  */

#ifdef	TCP
#include "internet.h"


extern int  errno;
extern int t_nerr, t_errno;
extern char *t_errlist[];


#ifdef	TLI_TCP

static fd_set	inprogress;

#include <tiuser.h>
#ifndef DEVTLI
#define DEVTLI	"/dev/tcp"
#endif /* DEVTLI */
#include <fcntl.h>

static char *sys_terrname (te)
int te;
{
    static char tbuf[32];

    if (te > 0 && te <= t_nerr)
	return t_errlist[te];
    (void) sprintf (tbuf, "Terrno %d", te);
    return tbuf;
}

static tli_lose (fd, str)
int fd;
char *str;
{
    int eindex = errno;
    int tindex = t_errno;
    SLOG (compat_log, LLOG_EXCEPTIONS, sys_terrname (t_errno),
			  ("%s failed:", str));
    if (fd != NOTOK)
	(void) close_tcp_socket (fd);
    errno = eindex;
    t_errno = tindex;
    return NOTOK;
}

static int tligetdis(fd)
int fd;
{
    struct t_discon *discon;

    discon = (struct t_discon *)t_alloc (fd, T_DIS, T_ALL);
    if (t_rcvdis (fd, discon) == NOTOK) {
	(void) tli_lose (NOTOK, "t_discon");
	goto out;
    }
out:
    if (discon != (struct t_discon *)0)
	t_free ((char *)discon, T_DIS);

    return NOTOK;
}

int	start_tcp_client (sock, priv)
struct sockaddr_in *sock;
int	priv;
{
    register int    port;
    int	    sd;

    if ((sd = t_open (DEVTLI, O_RDWR, NULL)) == NOTOK)
	return tli_lose (NOTOK, "t_open");
    FD_CLR (sd, &inprogress);

    if (sock == NULL) {
	    if (t_bind (sd, NULL, NULL) == NOTOK)
		return tli_lose (sd, "t_bind");
    }
    else {
	struct t_bind *bind, *bound;

	if ((bind = (struct t_bind *)t_alloc (sd, T_BIND, T_ALL)) == NULL)
	    return tli_lose (sd, "t_alloc");
	if ((bound = (struct t_bind *)t_alloc (sd, T_BIND, T_ALL)) == NULL)
	{
	    (void) tli_lose (sd, "t_alloc");
	    t_free(bind);
	    return NOTOK;
	}
	bind->qlen = 0;
	bcopy((char *)sock, bind->addr.buf, bind->addr.len = sizeof(*sock));

	for (port = IPPORT_RESERVED - priv;; priv ? port-- : port++) {
	    ((struct sockaddr_in *)bind->addr.buf)->sin_port =
		htons ((u_short) port);

	    if (t_bind (sd, bind, bound) == OK) {

		/* if we asked for a priv port we had better have got one */
		if (priv &&
		    ((struct sockaddr_in *)bound->addr.buf)->sin_port
						    >= IPPORT_RESERVED)
		{
		    if (port >= IPPORT_RESERVED / 2) {
			if (t_unbind(sd) == NOTOK) {
			    sd = tli_lose (sd, "t_bind");
			} else
			    continue;
		    } else {
			errno = EADDRINUSE;
			SLOG (compat_log, LLOG_EXCEPTIONS, "failed", ("bind"));
			t_close(sd);
			sd = NOTOK;
		    }
		}
	    } else
		sd = tli_lose (sd, "t_bind");
	    break;
	}
	(void)t_free(bind);
	(void)t_free(bound);
    }

    /* WARNING: This option-setting code may be implementation specific */
    {
	struct opthdr * opts;
	struct t_optmgmt * def;

	if ((def = (struct t_optmgmt *)t_alloc(sd, T_OPTMGMT, T_ALL)) == NULL)
	    SLOG (compat_log, LLOG_EXCEPTIONS, "failed", ("optmgmt"));
	else {
	    opts = (struct opthdr *) def -> opt.buf;
	    opts -> level = SOL_SOCKET;
	    opts -> name = SO_KEEPALIVE;
	    opts -> len = 0;
	    def->flags = T_NEGOTIATE;
	    def->opt.len = sizeof(*opts);
	    if (t_optmgmt(sd, def, def) == NOTOK)
		SLOG (compat_log, LLOG_EXCEPTIONS,
		    sys_terrname (t_errno), ("set SO_KEEPALIVE"));
	    t_free ((char *)def, T_OPTMGMT);
	}
    }

    return sd;
}

/*  */

int	start_tcp_server (sock, backlog, opt1, opt2)
struct sockaddr_in *sock;
int	backlog,
	opt1,
	opt2;
{
    int     sd, eindex;
    struct t_bind *bind, *bound;

    if ((sd = t_open (DEVTLI, O_RDWR, NULL)) == NOTOK)
	return tli_lose (sd, "t_open");
    FD_CLR (sd, &inprogress);


    /* WARNING: This option-setting code may be implementation specific */
    if (opt1 || opt2) {
	struct opthdr * opts;
	struct t_optmgmt * def;

	if ((def = (struct t_optmgmt *)t_alloc(sd, T_OPTMGMT, T_ALL)) == NULL)
	    SLOG (compat_log, LLOG_EXCEPTIONS, "failed", ("optmgmt"));
	else {
	    opts = (struct opthdr *) def -> opt.buf;
	    if (opt1) {
		opts -> level = SOL_SOCKET;
		opts -> len = 0;
		opts -> name = opt1;
		def->flags = T_NEGOTIATE;
		def->opt.len = sizeof(*opts);
		if (t_optmgmt(sd, def, def) == NOTOK)
		    SLOG (compat_log, LLOG_EXCEPTIONS,
			sys_terrname (t_errno), ("set option %d", opt1));
	    }
	    if (opt2) {
		opts -> level = SOL_SOCKET;
		opts -> len = 0;
		opts -> name = opt2;
		def->flags = T_NEGOTIATE;
		def->opt.len = sizeof(*opts);
		if (t_optmgmt(sd, def, def) == NOTOK)
		    SLOG (compat_log, LLOG_EXCEPTIONS,
			sys_terrname (t_errno), ("set option %d", opt2));
	    }
	    t_free ((char *)def, T_OPTMGMT);
	}
    }

    if ((bind = (struct t_bind *)t_alloc (sd, T_BIND, T_ALL)) == NULL)
	return tli_lose (sd, "t_alloc");
    
    if ((bound = (struct t_bind *)t_alloc (sd, T_BIND, T_ALL)) == NULL) {
	(void) tli_lose (sd, "t_alloc");
	t_free(bind);
	return(NOTOK);
    }
    
    bind -> qlen = 1;
    bcopy ((char *)sock, bind -> addr.buf, bind -> addr.len = sizeof (*sock));
    if (sock -> sin_port == 0)
	bind -> addr.len = 0;

    if (t_bind (sd, bind, bound) == NOTOK)
	sd = tli_lose (sd, "t_bind");
    else if (bound -> qlen < 1 
	|| (sock -> sin_port &&
	    ((struct sockaddr_in *)bound->addr.buf)->sin_port
						!= sock -> sin_port))
    {
	errno = EADDRINUSE;
	SLOG (compat_log, LLOG_EXCEPTIONS, "failed", ("bind"));
	t_close(sd);
	sd = NOTOK;
    } else
	bcopy(bound->addr.buf, (char *)sock, bound -> addr.len);
	
    eindex = errno;
    t_free ((char *)bind);
    t_free ((char *)bound);
    errno = eindex;

    /* OPTIONS */

    return sd;
}

/*  */

int	join_tcp_client (fd, sock)
int	fd;
struct sockaddr_in *sock;
{
    int     eindex,
	    len = sizeof *sock,
	    result = NOTOK;
    struct t_call *call;

    if ((call = (struct t_call *) t_alloc (fd, T_CALL, T_ADDR)) == NULL)
	/* now what do we do ? */
	return tli_lose (NOTOK, "t_alloc");

    if (t_listen(fd, call) == NOTOK) {
	(void) tli_lose (NOTOK, "t_listen");
	if (t_errno == TLOOK && t_look(fd) == T_DISCONNECT)
	    (void) tligetdis(fd);
	goto out;
    }
    bcopy(call->addr.buf, (char *)sock, call->addr.len);
    call->udata.len = call->opt.len = call->addr.len = 0;

    if ((result = t_open (DEVTLI, O_RDWR, NULL)) == NOTOK) {
	(void) tli_lose (NOTOK, "t_open");
	goto disconnect;
    }
    FD_CLR (result, &inprogress);

    if (t_bind (result, NULL, NULL) == NOTOK) {
	result = tli_lose (result, "t_bind");
	goto disconnect;
    }

    if (t_accept (fd, result, call) == NOTOK) {
	result = tli_lose (result, "t_accept");
	if (t_errno == TLOOK && t_look(fd) == T_DISCONNECT)
	    (void) tligetdis(fd);
	else
	    goto disconnect;
    }

out:
    eindex = errno;
    t_free(call);
    errno = eindex;
    return result;

disconnect:
    t_snddis(fd, call);
    goto out;
}

/*  */

int	join_tcp_server (fd, sock)
int	fd;
struct sockaddr_in *sock;
{
    int     eindex,
	    result;
    int retry = 0;
    struct t_call * call;

    if ((call = (struct t_call *) t_alloc (fd, T_CALL, T_ADDR)) == NULL)
	return tli_lose (fd, "t_alloc");

    if (FD_ISSET (fd, &inprogress)) {
	FD_CLR (fd, &inprogress);
	retry = 1;
    } else {
	call -> addr.len = sizeof *sock;
	bcopy ((char *)sock, call -> addr.buf, sizeof *sock);
    }

    if ((retry ? t_rcvconnect(fd, (struct t_call *)0)
	       : t_connect (fd, call, NULL)) == NOTOK)
    {
	(void) tli_lose (NOTOK, retry ? "t_rcvconnect" : "t_connect");
	t_free ((char *)call);
	switch (t_errno) {
	
	case TNODATA:
	    FD_SET (fd, &inprogress);
	    errno = EINPROGRESS;
	    break;

	case TLOOK:
	    switch (t_look(fd)) {
	    case T_CONNECT:
		FD_SET (fd, &inprogress);
		errno = EINPROGRESS;
		break;

	    case T_DISCONNECT:
		(void) tligetdis(fd);
		errno = ECONNREFUSED;
		break;
	    }

	case TBADADDR:
	    errno = EAFNOSUPPORT;
	    break;
	}
	return NOTOK;
    }
    t_free ((char *)call);
    return OK;
}

/*  */
int read_tcp_socket (fd, buffer, len)
int	fd;
char	*buffer;
int	len;
{
	int	n, flags;
	if ((n = t_rcv (fd, buffer, len, &flags)) == NOTOK)
		return tli_lose (NOTOK, "t_rcv");
	return n;
}

int write_tcp_socket (fd, buffer, len)
int	fd;
char	*buffer;
int	len;
{
	int n;

	if ((n = t_snd (fd, buffer, len, 0)) == NOTOK)
		return tli_lose (NOTOK, "t_snd");
	return n;
}

close_tcp_socket (fd)
int	fd;
{
    FD_CLR (fd, &inprogress);
    return (t_close (fd));
}

#endif /* TLI_TCP */

#endif /* TCP */
