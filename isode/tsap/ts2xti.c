/* ts2xli.c - XTI OSI TP interface */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/tsap/RCS/ts2xti.c,v 9.0 1992/06/16 12:40:39 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/tsap/RCS/ts2xti.c,v 9.0 1992/06/16 12:40:39 isode Rel $
 *
 *
 * $Log: ts2xti.c,v $
 * Revision 9.0  1992/06/16  12:40:39  isode
 * Release 8.0
 *
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


/* This should be considered Alpha test code at this stage... */


/* LINTLIBRARY */

#include <stdio.h>
#include "tpkt.h"

#ifdef  TP4
#include "tp4.h"
#endif

#ifdef  XTI_TP
#include <errno.h>
#include <fcntl.h>
#include "tailor.h"

/*
 * Better to do segmenting across the syscall interface than in Session. Make
 * this as big as you dare, but remember all your receive buffers will be
 * this big too. 
 */
#define MAXTP4          8192

/* why do we need this ? */
#define TP4SLOP           12	/* estimate of largest DT PCI */
#define XTIDFLTADDR 1024
#define XTIDFLTOPT 1024
#define XTIDFLTUDATA 1024

extern char *	xti_tp4_ident;
extern char *	xti_tp0_ident;

extern int      errno;
extern int      t_errno;
char           *t_errlist[] = {"Incorrect Address Format",
			       "Incorrect Option Format",
			       "incorrect permissions",
			       "illegal transport fd",
			       "could not allocate address",
			       "out of state ",
			       "bad call sequence number",
			       "system error",
			       "event requires attention",
			       "illegal amount of data",
			       "buffer not large enough",
			       "flow control",
			       "no data",
			       "disconnect indication not found on queue",
			       "unitdata error not found",
			       "bad flags",
			       "no orderly release found on queue",
			       "primitive not supported",
			       "state is in process of changing",
			       "unsupported struct type requested",
			       "invalid transport provider name",
			       "qlen is zero",
			       "address in use"};
#define t_nerr 22

struct td_info {
  struct td_info *next;
  struct t_info   info;
  int             fd;
};
#define NULLTDINFO (struct td_info *)0
static struct td_info *td_head = NULLTDINFO;

static char    *
sys_terrname(te)
  int             te;
{
  static char     tbuf[41];

  if (te > 0 && te <= t_nerr)
    return t_errlist[te];
  (void) sprintf(tbuf, "Terrno %d", te);
  return tbuf;
}

# define tp4err2gen(err) (err)

int
T_getinfo(fd, info)
  int             fd;
  struct t_info  *info;
{
  struct td_info *td_current = td_head;
  while (td_current != NULLTDINFO)
    if (fd == td_current->fd) {
      info->addr = td_current->info.addr;
      info->options = td_current->info.options;
      info->tsdu = td_current->info.tsdu;
      info->etsdu = td_current->info.etsdu;
      info->connect = td_current->info.connect;
      info->discon = td_current->info.discon;
      info->servtype = td_current->info.servtype;
      return OK;
    } else
      td_current = td_current->next;
  t_errno = TBADF;
  return NOTOK;
}

static void
T_swapfd (fd, nfd)
  int             fd;
  int   	  nfd;
{
  struct td_info *td_current = td_head;
  while (td_current != NULLTDINFO)
    if (fd == td_current->fd) {
      td_current->fd = nfd;
      break;
  } else
      td_current = td_current->next;
}

int
T_setinfo(fd, info)
  int             fd;
  struct t_info  *info;
{
  struct td_info *td_current;
  if ((td_current = (struct td_info *) calloc(1,sizeof(struct td_info))) == NULL) {
    LLOG(tsap_log, LLOG_EXCEPTIONS,
	 ("Malloc failed in T_setinfo"));
    return NOTOK;
  }
  td_current->fd = fd;
  td_current->info.addr = info->addr;
  td_current->info.options = info->options;
  td_current->info.tsdu = info->tsdu;
  td_current->info.etsdu = info->etsdu;
  td_current->info.connect = info->connect;
  td_current->info.discon = info->discon;
  td_current->info.servtype = info->servtype;
  td_current->next = td_head;
  td_head = td_current;
  return OK;
}

int
T_delinfo(fd)
  int             fd;
{
  struct td_info *td_early = td_head;
  struct td_info *td_late = td_head;
  if (td_early == NULLTDINFO) {
    LLOG(tsap_log, LLOG_EXCEPTIONS,
	 ("No transport descriptor to delete"));
    return NOTOK;
  }
  if (fd == td_head->fd) {
    td_head = td_head->next;
    free(td_early->info);
    return OK;
  }
  while (td_late != NULLTDINFO) {
    if (fd == td_late->fd) {
      td_early->next = td_late->next;
      free(td_late->info);
      return OK;
    } else {
      td_late = td_late->next;
      td_early = td_late;
    }
  }
  LLOG(tsap_log, LLOG_EXCEPTIONS,
       ("Couldn't find transport descriptor %d to delete", fd));
  return NOTOK;
}


int
T_open(name, oflag, info)
  char           *name;
  int             oflag;
  struct t_info  *info;
{
  int             fd;
  if ((fd = t_open(name, oflag, info)) == -1)
    return NOTOK;
  if (T_setinfo(fd, info) != 0)
    return NOTOK;
  return fd;
}

int
T_accept(fd, resfd, call)
  int             fd;
  int             resfd;
  struct t_call  *call;
{
  if (t_accept(fd, resfd, call) != 0)
    return -1;
  if (resfd != fd) {
    struct t_info   info;
    if (T_getinfo(fd, &info) != 0)
      return -1;
    if (T_setinfo(resfd, &info) != 0)
      return -1;
  } return 0;
}

int
T_close(fd)
  int             fd;
{
  if (T_delinfo(fd) != 0)
    LLOG(tsap_log, LLOG_EXCEPTIONS, ("T_delinfo failed in T_close"));
  return (t_close(fd));
}

char           *
T_alloc(fd, struct_type, fields)
  int             fd;
  int             struct_type;
  int             fields;
{
  struct t_info   info;
  struct t_bind  *get_bind;
  struct t_call  *get_call;
  struct t_discon *get_discon;
  if (T_getinfo(fd, &info) != 0)
    return -1;
  if (info.addr == -2)
    info.addr = 0;
  if (info.addr == -1)
    info.addr = XTIDFLTADDR;
  if (info.options == -1)
    info.options = XTIDFLTOPT;
  if (info.options == -2)
    info.options = 0;
  if (info.tsdu == -1)
    info.tsdu = XTIDFLTUDATA;
  if (info.tsdu == -2)
    info.tsdu = 0;
  if (info.etsdu == -1)
    info.etsdu = XTIDFLTUDATA;
  if (info.etsdu == -2)
    info.etsdu = 0;
  switch (struct_type) {
  case T_BIND_STR:
    if ((get_bind = (struct t_bind *) calloc(1,sizeof(struct t_bind))) == NULL)
      break;
    if ((get_bind->addr.buf = (char *) calloc(1,sizeof(char) * info.addr)) == NULL)
      break;
    get_bind->addr.maxlen = info.addr;
    return (char *) get_bind;
  case T_CALL_STR:
    if ((get_call = (struct t_call *) calloc(1,sizeof(struct t_call))) == NULL)
      break;
    if ((get_call->addr.buf = (char *) calloc(1,sizeof(char) * info.addr)) == NULL)
      break;
    get_call->addr.maxlen = info.addr;
    if ((get_call->opt.buf = (char *) calloc(1,sizeof(char) * info.options)) == NULL)
      break;
    get_call->opt.maxlen = info.options;
    if ((get_call->udata.buf = (char *) calloc(1,sizeof(char) * max(info.tsdu, info.etsdu))) == NULL)
      break;
    get_call->udata.maxlen = max(info.tsdu, info.etsdu);
    return (char *) get_call;
  case T_DIS_STR:
    if ((get_discon = (struct t_discon *) calloc(1,sizeof(struct t_discon))) == NULL)
      break;
    if ((get_discon->udata.buf = (char *) calloc(1,sizeof(char) * max(info.tsdu, info.etsdu))) == NULL)
      break;
    get_discon->udata.maxlen = max(info.tsdu, info.etsdu);
    return (char *) get_discon;
  default:
    t_errno = TNOSTRUCTYPE;
    return NOTOK;
  }
  t_errno = TSYSERR;
  return NOTOK;
}

int
T_free(ptr, struct_type)
  char           *ptr;
  int             struct_type;
{
  struct t_bind  *bind_free;
  struct t_call  *call_free;
  struct t_optmgmt *optmgmt_free;
  struct t_discon *discon_free;
  struct t_unitdata *unitdata_free;
  struct t_uderr *uderr_free;
  struct t_info  *info_free;
  switch (struct_type) {
  case T_BIND_STR:
    bind_free = (struct t_bind *) ptr;
    if (bind_free->addr.buf != NULL)
      free(bind_free->addr.buf);
    free(bind_free);
    break;
  case T_CALL_STR:
    call_free = (struct t_call *) ptr;
    if (call_free->addr.buf != NULL)
      free(call_free->addr.buf);
    if (call_free->opt.buf != NULL)
      free(call_free->opt.buf);
    if (call_free->udata.buf != NULL)
      free(call_free->udata.buf);
    free(call_free);
    break;
  case T_OPTMGMT_STR:
    optmgmt_free = (struct t_optmgmt *) ptr;
    if (optmgmt_free->opt.buf != NULL)
      free(optmgmt_free->opt.buf);
    free(optmgmt_free);
    break;
  case T_DIS_STR:
    discon_free = (struct t_discon *) ptr;
    if (discon_free->udata.buf != NULL)
      free(discon_free->udata.buf);
    free(discon_free);
    break;
  case T_UNITDATA_STR:
    unitdata_free = (struct t_unitdata *) ptr;
    if (unitdata_free->addr.buf != NULL)
      free(unitdata_free->addr.buf);
    if (unitdata_free->opt.buf != NULL)
      free(unitdata_free->opt.buf);
    if (unitdata_free->udata.buf != NULL)
      free(unitdata_free->udata.buf);
    free(unitdata_free);
    break;
  case T_UDERROR_STR:
    uderr_free = (struct t_uderr *) ptr;
    if (uderr_free->addr.buf != NULL)
      free(uderr_free->addr.buf);
    if (uderr_free->opt.buf != NULL)
      free(uderr_free->opt.buf);
    free(uderr_free);
    break;
  case T_INFO_STR:
    info_free = (struct t_info *) ptr;
    free(info_free);
  }
  return OK;
}

static
tli_lose(td, fd, reason, str)
  struct TSAPdisconnect *td;
  int             fd, reason;
  char           *str;
{
  int             eindex = errno;
  int             tindex = t_errno;
  (void) tsaplose(td, reason, sys_terrname(t_errno), str);
  if (fd != NOTOK)
    (void) T_close(fd);
  errno = eindex;
  t_errno = tindex;
  return NOTOK;
}

/* open and bind an endpoint */
static int
tp4bind(ta, qlen, td, na)
  struct TSAPaddr *ta;
  int             qlen;
  struct TSAPdisconnect *td;
  struct NSAPaddr *na;
{
  int             fd;
  struct t_info   Info;
  struct t_info  *info = &Info;
  struct NSAPinfo *ni;
  char *xti_class = xti_tp4_ident;
  struct NSAPaddr *naq;

  if (ta && (ta -> ta_naddr > 0)) 
	  naq = ta -> ta_addrs;
  else
	  naq = na;

  if (naq) {
	  struct NSAPaddr sna;

	  if (naq -> na_stack == NA_NSAP) 
		  if (norm2na (naq->na_address, naq->na_addrlen, &sna) == OK)
			  naq = &sna;

	  if (naq -> na_stack == NA_X25)
		  xti_class = xti_tp0_ident;
	  else if (ni = getnsapinfo (naq))
		  xti_class = ni -> is_class;
  }
  
  DLOG(tsap_log, LLOG_TRACE, ("xti_class %s",xti_class));

  if ((fd = T_open(xti_class, O_RDWR, info)) == NOTOK)
    return tli_lose(td, NOTOK, DR_CONGEST, "cannot open TS");

  if (ta && (ta->ta_naddr > 0 || ta->ta_selectlen > 0)) {
    struct t_bind  *bind, *bound;
    struct TSAPaddr bound_ta;

    if ((bind = (struct t_bind *) T_alloc(fd, T_BIND_STR, T_ALL)) == NULL)
      return tli_lose(td, fd, DR_CONGEST, "can't allocate bind");
    if ((bound = (struct t_bind *) T_alloc(fd, T_BIND_STR, T_ALL)) == NULL) {
      T_free((char *) bind, T_BIND_STR);
      return tli_lose(td, fd, DR_CONGEST, "can't allocate bind");
    }
    if (gen2tp4(ta, &bind->addr) == NOTOK) {
      fd = tli_lose(td, fd, DR_ADDRESS, "invalid address");
      goto out;
    }
    bind->qlen = qlen;

    if (t_bind(fd, bind, bound) == NOTOK) {
      char            buf[TS_SIZE * 2 + 1];

      fd = tli_lose(td, fd, DR_CONGEST, "unable to bind");
      buf[explode(buf, bind->addr.buf, bind->addr.len)] = '\0';
      LLOG(tsap_log, LLOG_EXCEPTIONS,
	   ("fail to bind %s as %s", taddr2str(ta), buf));
      goto out;
    }
    /*
     * Check that we bound where we asked, we could be given someting else.
     * Only care about selector 
     */
    (void) tp42gen(&bound_ta, &bound->addr);
    if ((qlen && bound->qlen < 1) ||
	ta->ta_selectlen != bound_ta.ta_selectlen ||
	memcmp(ta->ta_selector, bound_ta.ta_selector,
	       bound_ta.ta_selectlen) != 0) {
      char            buf[BUFSIZ];

      t_unbind(fd);
      fd = tli_lose(td, fd, DR_CONGEST, "address in use");
      (void) strcpy(buf, taddr2str(ta));
      LLOG(tsap_log, LLOG_EXCEPTIONS,
	   ("tried to bind to %s but got %s", buf, taddr2str(&bound_ta)));
      goto out;
    }
out:
    T_free((char *) bind, T_BIND_STR);
    T_free((char *) bound, T_BIND_STR);
  } else {
    if (t_bind(fd, (struct t_bind *) 0, (struct t_bind *) 0) == NOTOK)
      fd = tli_lose(td, fd, DR_CONGEST, "unable to bind to NULL");
  }
  if (T_setinfo(fd, info) == NOTOK) {
    LLOG(tsap_log, LLOG_EXCEPTIONS,
	 ("Couldn't set transport descriptor information for %d", fd));
    return NOTOK;
  }
  DLOG(tsap_log, LLOG_TRACE, ("bound %d to %s", fd, taddr2str(ta)));
  return fd;
}

/*
 * Check that the endpoint is suitable. WARNING: do not call this after
 * determining if an actual connection supports expedited data (or other
 * options).  It is not defined whether T_getinfo() return defaults or
 * current values once a connection is established. 
 */
static int
tp4info(tb, td)
  struct tsapblk *tb;
  struct TSAPdisconnect *td;
{
  struct t_info   info;
  int             len;

  if (T_getinfo(tb->tb_fd, &info) == NOTOK) {
    return tli_lose(td, NOTOK, DR_CONGEST, "T_getinfo");
  }
  DLOG(tsap_log, LLOG_DEBUG,
    ("tp4info: addr=%d, opt=%d, tsdu=%d, etsdu=%d, con=%d, dis=%d, serv=%d",
     info.addr, info.options, info.tsdu, info.etsdu,
     info.connect, info.discon, info.servtype));
  switch (len = info.tsdu) {
  case -2:
    return tsaplose(td, DR_PARAMETER, NULLCP, "TSDU data not supported");
  case 0:
    return tsaplose(td, DR_PARAMETER, NULLCP, "TSDU not packetized");
  case -1:
    len = MAXTP4;
    break;
  default:
    break;
  }

  if (info.etsdu == -1 || info.etsdu > 0)
    tb->tb_flags |= TB_EXPD;
  else
    tb->tb_flags &= ~TB_EXPD;
  tb->tb_tpduslop = TP4SLOP;
  tb->tb_tsdusize = len - tb->tb_tpduslop;

  return OK;
}

static int
tp4getdis(fd, td)
  int             fd;
  struct TSAPdisconnect *td;
{
  struct t_discon *discon;

  discon = (struct t_discon *) T_alloc(fd, T_DIS_STR, T_ALL);
  if (t_rcvdis(fd, discon) == NOTOK) {
    (void) tli_lose(td, NOTOK, DR_NETWORK, "t_discon");
    goto out;
  }
  if (discon != (struct t_discon *) 0) {
    td->td_reason = tp4err2gen(discon->reason);
    if ((td->td_cc = discon->udata.len) > 0) {
      if (td->td_cc > TD_SIZE)
	td->td_cc = TD_SIZE;	/* discard extra! */
      bcopy(discon->udata.buf, td->td_data, td->td_cc);
    }
  } else {
    td->td_reason = DR_UNKNOWN;
    td->td_cc = 0;
  }

out:
  if (discon != (struct t_discon *) 0)
    T_free((char *) discon, T_DIS_STR);

  return NOTOK;
}

/*  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * UPPER HALF */

static int
TConnect(tb, expedited, data, cc, td)
  register struct tsapblk *tb;
  char           *data;
  int             expedited, cc;
  struct TSAPdisconnect *td;
{
  struct t_call  *sndcall;
  struct t_call  *rcvcall;
  struct TSAPaddr ta;

  if ((sndcall =
       (struct t_call *) T_alloc(tb->tb_fd, T_CALL_STR, T_ALL)) == NULL)
    return tli_lose(td, NOTOK, DR_CONGEST, "T_alloc");
  if ((rcvcall =
       (struct t_call *) T_alloc(tb->tb_fd, T_CALL_STR, T_ALL)) == NULL) {
    T_free((char *) sndcall, T_CALL_STR);
    return tli_lose(td, NOTOK, DR_CONGEST, "T_alloc");
  }
  copyTSAPaddrX(&tb->tb_responding, &ta);
  if (gen2tp4(&ta, &sndcall->addr) == NOTOK)
    return tli_lose(td, NOTOK, DR_ADDRESS, "invalid address");

  toomuchP(data, cc, sndcall->udata.maxlen, "initial");
  if (data)
    bcopy(data, sndcall->udata.buf, sndcall->udata.len = (unsigned) cc);


  /* use tb_cc to communicate to TRetry if the connect completed here */
  if (t_connect(tb->tb_fd, sndcall, rcvcall) == NOTOK) {
    T_free((char *) rcvcall, T_CALL_STR);
    T_free((char *) sndcall, T_CALL_STR);
    switch (t_errno) {

    case TNODATA:
      tb->tb_cc = -1;
      return CONNECTING_2;

    case TLOOK:
      switch (t_look(tb->tb_fd)) {
      case T_CONNECT:
	tb->tb_cc = -1;
	return CONNECTING_2;

      case T_DISCONNECT:
	return tp4getdis(tb->tb_fd, td);
      }

    case TBADADDR:
      return tli_lose(td, NOTOK, DR_ADDRESS,
		      "unable to establish connection");

    default:
      return tli_lose(td, NOTOK, DR_REFUSED,
		      "unable to establish connection");
    }

  }
  T_free((char *) sndcall, T_CALL_STR);

  (void) tp42gen(&ta, &rcvcall->addr);
  copyTSAPaddrY(&ta, &tb->tb_responding);

  if (cc = rcvcall->udata.len) {
    if (tb->tb_data)
      free(tb->tb_data);
    if ((tb->tb_data = calloc(1,(unsigned) cc)) == NULLCP) {
      T_free((char *) rcvcall, T_CALL_STR);
      (void) tsaplose(td, DR_CONGEST, NULLCP, "out of memory");
    }
    bcopy(rcvcall->udata.buf, tb->tb_data, tb->tb_cc = cc);
  } else
    tb->tb_cc = 0;
  if (rcvcall->opt.len == sizeof(struct isoco_options)) {
    struct isoco_options *isoco_options = (struct isoco_options *) rcvcall->opt.buf;
    if (isoco_options->expd == T_YES)
      tb->tb_flags |= TB_EXPD;
    else
      tb->tb_flags &= ~TB_EXPD;
  }
  T_free((char *) rcvcall, T_CALL_STR);

  return DONE;
}

/*  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static int
TRetry(tb, async, tc, td)
  register struct tsapblk *tb;
  int             async;
  struct TSAPconnect *tc;
  struct TSAPdisconnect *td;
{
  struct t_call  *call;

  if (tb->tb_cc == -1) {	/* call not yet connected */
    struct TSAPaddr ta;

    if (async) {
      switch ((*tb->tb_retryfnx) (tb, td)) {
      case NOTOK:
	goto out;
      case OK:
	return CONNECTING_2;

      case DONE:
	break;
      }
    }
    if ((call = (struct t_call *)
	 T_alloc(tb->tb_fd, T_CALL_STR, T_ALL)) == NULL) {
      (void) tli_lose(td, NOTOK, DR_CONGEST, "T_alloc ");
      goto out;
    }
    if (t_rcvconnect(tb->tb_fd, call) == NOTOK) {
      T_free((char *) call, T_CALL_STR);
      if (t_errno == TNODATA)
	return CONNECTING_2;
      else if (t_errno == TLOOK && t_look(tb->tb_fd) == T_DISCONNECT)
	(void) tp4getdis(tb->tb_fd, td);
      else
	(void) tli_lose(td, NOTOK, DR_REFUSED, "t_rcvconnect");
      goto out;
    }
    if (async) {
      int             flags;
      if ((flags = fcntl(tb->tb_fd, F_GETFL, 0)) == NOTOK) {
	T_free((char *) call, T_CALL_STR);
	(void) tsaplose(td, DR_CONGEST, "failed", "fcntl");
	goto out;
      }
      flags &= ~FNDELAY;
      (void) fcntl(tb->tb_fd, F_SETFL, flags);
    }
    (void) tp42gen(&ta, &call->addr);
    copyTSAPaddrY(&ta, &tb->tb_responding);

    if (call->udata.len > 0)
      bcopy(call->udata.buf, tc->tc_data,
	    tc->tc_cc = (int) call->udata.len);
    if (call->opt.len == sizeof(struct isoco_options)) {
      struct isoco_options *isoco_options = (struct isoco_options *) call->opt.buf;
      if (isoco_options->expd == T_YES)
	tb->tb_flags |= TB_EXPD;
      else
	tb->tb_flags &= ~TB_EXPD;
    }
    T_free((char *) call, T_CALL_STR);
  } else {
    if (tc->tc_cc = tb->tb_cc)
      bcopy(tb->tb_data, tc->tc_data, tc->tc_cc);
  }
  tb->tb_flags |= TB_CONN;

  tc->tc_expedited = (tb->tb_flags & TB_EXPD) ? 1 : 0;
  tc->tc_sd = tb->tb_fd;
  tc->tc_tsdusize = tb->tb_tsdusize;
  copyTSAPaddrX(&tb->tb_responding, &tc->tc_responding);
#ifdef  MGMT
  if (tb->tb_manfnx)
    (*tb->tb_manfnx) (OPREQOUT, tb);
#endif

  return DONE;

out:;
  freetblk(tb);

  return NOTOK;
}

/*  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static int
TStart(tb, cp, ts, td)
  register struct tsapblk *tb;
  char           *cp;
  struct TSAPstart *ts;
  struct TSAPdisconnect *td;
{
  int             i;

  DLOG(tsap_log, LLOG_DEBUG, ("TStart: \"%s\"", cp));

  ts->ts_expedited = (tb->tb_flags & TB_EXPD) ? 1 : 0;
  ts->ts_sd = tb->tb_fd;
  copyTSAPaddrX(&tb->tb_initiating, &ts->ts_calling);
  copyTSAPaddrX(&tb->tb_responding, &ts->ts_called);
  ts->ts_expedited = (tb->tb_flags & TB_EXPD) ? 1 : 0;
  ts->ts_tsdusize = tb->tb_tsdusize;

  if ((i = strlen(cp)) > 0) {
    if (i > 2 * TS_SIZE)
      return tsaplose(td, DR_CONNECT, NULLCP,
		      "too much initial user data");
    ts->ts_cc = implode((u_char *) ts->ts_data, cp, i);
  } else
    ts->ts_cc = 0;

  return OK;
}

/*  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
 * This is a tricky one. We have to pass over a dup'd transport descriptor
 * and here we have to finish this by accepting the call and completing the
 * connection. When started the fd is the descriptor that we are listening
 * on, but by the end it is the one we accepted dup'd into the old slot. 
 */
/* ARGSUSED */

static int
TAccept(tb, responding, data, cc, qos, td)
  register struct tsapblk *tb;
  char           *data;
  int             responding, cc;
  struct QOStype *qos;
  struct TSAPdisconnect *td;
{
  struct t_call  *call;
  int             result;

  /* we are going to accept on a new endpoint */
  if ((result = tp4bind((struct TSAPaddr *) 0, 0, td, NULLNA)) == NOTOK)
    goto reject;

  if ((call = (struct t_call *) T_alloc(tb->tb_fd, T_CALL_STR, T_ALL)) == NULL) {
    (void) tli_lose(td, result, DR_CONGEST, "T_alloc");
    goto reject;
  }
  if (data)			/* user data? */
    bcopy(data, call->udata.buf, call->udata.len = (unsigned) cc);
  call->sequence = tb->tb_dstref;

  DLOG(tsap_log, LLOG_TRACE,
       ("T_accept(%d, %d): seq=%d", tb->tb_fd, result, call->sequence));
  if (T_accept(tb->tb_fd, result, call) == NOTOK) {
    DLOG(tsap_log, LLOG_DEBUG,
	 ("T_accept fail: t_errno=%d, t_look=%d",
	  t_errno, t_look(tb->tb_fd)));
    if (t_errno == TLOOK && t_look(tb->tb_fd) == T_DISCONNECT)
      (void) tp4getdis(tb->tb_fd, td);
    else
      (void) tli_lose(td, result, DR_CONGEST, "T_accept");
    T_free((char *) call, T_CALL_STR);
    return NOTOK;
  }
  T_free((char *) call, T_CALL_STR);

  /*
   * OK - we have the new call - lets pretend we are back where we were... 
   */
  (void) T_close(tb->tb_fd);	/* finished with this - get rid of it! */
  (void) dup2(result, tb->tb_fd);
  (void) T_close(result);
  (void) t_sync(tb->tb_fd);

  T_swapfd (result, tb->tb_fd);

  /* Phew - we now have everything as it should be - I hope */

  tb->tb_flags |= TB_CONN;
#ifdef  MGMT
  if (tb->tb_manfnx)
    (*tb->tb_manfnx) (OPREQIN, tb);
#endif

  return OK;

reject:
  {
    /* since we might be here beacuse we could not T_alloc */
    struct t_call   dis;

    bzero((char *) &dis, sizeof dis);
    dis.sequence = tb->tb_dstref;
    (void) t_snddis(tb->tb_fd, &dis);
    return NOTOK;
  }
}

/*  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static int
TWrite(tb, uv, expedited, td)
  register struct tsapblk *tb;
  register struct udvec *uv;
  int             expedited;
  struct TSAPdisconnect *td;
{
  register int    cc;
  int             async;
  int             flags;
#ifdef  MGMT
  int             dlen;
#endif
  register char  *bp;
  register struct qbuf *qb;

  flags = expedited ? T_EXPEDITED : 0;

#ifdef  MGMT
  dlen = 0;
#endif

  if (!expedited && (tb->tb_flags & TB_QWRITES)) {
    int             nc;
    struct udvec   *xv;

    cc = 0;
    for (xv = uv; xv->uv_base; xv++)
      cc += xv->uv_len;
#ifdef  MGMT
    dlen = cc;
#endif

    if ((qb = (struct qbuf *) calloc(1,sizeof *qb + (unsigned) cc))
	== NULL) {
      (void) tsaplose(td, DR_CONGEST, NULLCP,
		 "unable to malloc %d octets for pseudo-writev, failing...",
		      cc);
      freetblk(tb);

      return NOTOK;
    }
    qb->qb_forw = qb->qb_back = qb;
    qb->qb_data = qb->qb_base, qb->qb_len = cc;

    bp = qb->qb_data;
    for (xv = uv; xv->uv_base; xv++) {
      bcopy(xv->uv_base, bp, xv->uv_len);
      bp += xv->uv_len;
    }

    if (tb->tb_qwrites.qb_forw != &tb->tb_qwrites) {
      nc = 0;
      goto insert;
    }
    if ((async = fcntl(tb->tb_fd, F_GETFL, 0)) != NOTOK)
      (void) fcntl(tb->tb_fd, F_SETFL, async | FNDELAY);

    nc = t_snd(tb->tb_fd, qb->qb_data, qb->qb_len, 0);
    DLOG(tsap_log, LLOG_TRACE,
	 ("t_snd(fd=%d, buf=0x%x, len=%d, NO_MORE) ret=%d",
	  tb->tb_fd, qb->qb_data, qb->qb_len, nc));


    if (async != NOTOK)
      (void) fcntl(tb->tb_fd, F_SETFL, async);

    if (nc != cc) {
      if (nc == NOTOK) {
	if (t_errno == TLOOK && t_look(tb->tb_fd) == T_DISCONNECT) {
	  (void) tp4getdis(tb->tb_fd, td);
	  goto losing;
	} else if (t_errno != TFLOW) {
	  (void) tsaplose(td, DR_CONGEST, "failed", "sendmsg");
	  goto losing;
	}
	nc = 0;
      }
      if ((*tb->tb_queuePfnx) (tb, 1, td) == NOTOK)
	goto losing;

      qb->qb_data += nc, qb->qb_len -= nc;
  insert:;
      insque(qb, tb->tb_qwrites.qb_back);
      DLOG(tsap_log, LLOG_TRACE,
	   ("queueing blocked write of %d of %d octets", nc, cc));
    } else
      free((char *) qb);
    goto done;

losing:;
    free((char *) qb);
    freetblk(tb);

    return NOTOK;
  }
  for (; uv->uv_base; uv++) {
    register int    more;
#ifdef  MGMT
    dlen += uv->uv_len;
#endif
    /* not sure why one cannot set the more bit on expedited */
    more = (!(expedited || (uv + 1)->uv_base == NULL));
    DLOG(tsap_log, LLOG_TRACE,
	 ("t_snd(fd=%d, buf=0x%x, len=%d, %s%s)",
	  tb->tb_fd, uv->uv_base, uv->uv_len,
	  (expedited ? "EXPEDITED, " : ""),
	  (more ? "MORE" : "NO_MORE")));
    if (t_snd(tb->tb_fd, uv->uv_base, uv->uv_len,
	      flags | (more ? T_MORE : 0)) == NOTOK) {
      if (t_errno == TLOOK && t_look(tb->tb_fd) == T_DISCONNECT)
	(void) tp4getdis(tb->tb_fd, td);
      else
	(void) tsaplose(td, DR_CONGEST, "failed", "sendmsg");
      freetblk(tb);

      return NOTOK;
    }
  }

done:;
#ifdef  MGMT
  if (tb->tb_manfnx)
    (*tb->tb_manfnx) (USERDT, tb, dlen);
#endif

  return OK;
}

/*  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static int
TDrain(tb, td)
  register struct tsapblk *tb;
  struct TSAPdisconnect *td;
{
  int             nc, onoff, result;
  register struct qbuf *qb;

  if ((onoff = fcntl(tb->tb_fd, F_GETFL, 0)) != NOTOK)
    (void) fcntl(tb->tb_fd, F_SETFL, onoff | FNDELAY);


  while ((qb = tb->tb_qwrites.qb_forw) != &tb->tb_qwrites) {

    if ((nc = t_snd(tb->tb_fd, qb->qb_data, qb->qb_len, 0)) !=
	qb->qb_len) {
      if (nc == NOTOK) {
	if (t_errno == TLOOK && t_look(tb->tb_fd) == T_DISCONNECT) {
	  result = tp4getdis(tb->tb_fd, td);
	  goto out;
	} else if (t_errno != TFLOW) {
	  result = tsaplose(td, DR_NETWORK, "failed",
			    "write to network");
	  goto out;
	}
	nc = 0;
      }
      qb->qb_data += nc, qb->qb_len -= nc;
      DLOG(tsap_log, LLOG_TRACE,
	   ("wrote %d of %d octets from blocked write", nc,
	    qb->qb_len));

      result = OK;
      goto out;
    }
    DLOG(tsap_log, LLOG_TRACE,
	 ("finished blocked write of %d octets", qb->qb_len));
    remque(qb);
    free((char *) qb);
  }
  result = DONE;

out:;
  if (onoff != NOTOK)
    (void) fcntl(tb->tb_fd, F_SETFL, onoff);

  return result;
}

/*  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* ARGSUSED */

static int
TRead(tb, tx, td, async, oob)
  register struct tsapblk *tb;
  register struct TSAPdata *tx;
  struct TSAPdisconnect *td;
  int             async, oob;
{
  int             cc;
  register struct qbuf *qb;
  static struct qbuf *spare_qb = 0;
  int             flags;

  bzero((char *) tx, sizeof *tx);
  tx->tx_qbuf.qb_forw = tx->tx_qbuf.qb_back = &tx->tx_qbuf;

  for (;;) {
    qb = NULL;
    if (spare_qb) {
      if (spare_qb->qb_len >= tb->tb_tsdusize)
	qb = spare_qb;
      else
	free((char *) spare_qb);
      spare_qb = NULL;
    }
    if (qb == NULL && (qb = (struct qbuf *)
	       calloc(1,(unsigned) (sizeof *qb + tb->tb_tsdusize))) == NULL) {
      (void) tsaplose(td, DR_CONGEST, NULLCP, NULLCP);
      break;
    } else
      qb->qb_len = tb->tb_tsdusize;
    qb->qb_data = qb->qb_base;

    if ((cc = t_rcv(tb->tb_fd, qb->qb_data, qb->qb_len, &flags))
	== NOTOK) {
      switch (t_errno) {
      case TNODATA:
	td->td_cc = 0;
	break;
      case TLOOK:
	switch (t_look(tb->tb_fd)) {
	case T_DISCONNECT:
	  (void) tp4getdis(tb->tb_fd, td);
	  break;
	default:
	  (void) tsaplose(td, DR_CONGEST, NULLCP,
			  "unexpected event %d",
			  t_look(tb->tb_fd));
	  break;
	}
	goto out;
      default:
	(void) tli_lose(td, NOTOK, DR_CONGEST, "t_rcv");
	break;
      }
      break;
    }
    DLOG(tsap_log, LLOG_DEBUG,
	 ("t_rcv(fd=%d, buf=0x%x, len=%d, flags=0x%x) ret=%d",
	  tb->tb_fd, qb->qb_data, qb->qb_len, flags, cc));
    if (flags & T_EXPEDITED) {
      if (cc > 0) {
	register struct qbuf *qb2 = tx->tx_qbuf.qb_back;

	/*
	 * assume ETSDU will always be less than MAXTP4 
	 */
	if (qb2 != &tx->tx_qbuf) {
	  bcopy(qb->qb_data, qb2->qb_len + qb2->qb_data, cc);
	  tx->tx_cc = (qb2->qb_len += cc);
	  (spare_qb = qb)->qb_len = tb->tb_tsdusize;
	} else {
	  insque(qb, qb2);
	  tx->tx_cc = (qb->qb_len = cc);
	}
      } else
	(spare_qb = qb)->qb_len = tb->tb_tsdusize;

      /*
       * would need a pretty wierd implementation, but possible 
       */
      if (flags & T_MORE)
	continue;
      tx->tx_expedited = 1;

      return OK;
    } else
      tx->tx_expedited = 0;

    tb->tb_len += (qb->qb_len = cc);
    if (cc > 0) {
      register struct qbuf *qb2 = tb->tb_qbuf.qb_back;

      if (qb2 != &tb->tb_qbuf && qb2->qb_len + cc <= tb->tb_tsdusize) {
	bcopy(qb->qb_data, qb2->qb_len + qb2->qb_data, cc);
	qb2->qb_len += cc;
	(spare_qb = qb)->qb_len = tb->tb_tsdusize;
      } else
	insque(qb, qb2);
    } else
      (spare_qb = qb)->qb_len = tb->tb_tsdusize;

#ifdef  MGMT
    if (tb->tb_manfnx)
      (*tb->tb_manfnx) (USERDR, tb, tb->tb_len);
#endif
    if (flags & T_MORE) {
      if (async)
	return DONE;
      continue;
    }
    if (tb->tb_qbuf.qb_forw != &tb->tb_qbuf) {
      tx->tx_qbuf = tb->tb_qbuf;/* struct copy */
      tx->tx_qbuf.qb_forw->qb_back =
	tx->tx_qbuf.qb_back->qb_forw = &tx->tx_qbuf;
      tx->tx_cc = tb->tb_len;
      tb->tb_qbuf.qb_forw =
	tb->tb_qbuf.qb_back = &tb->tb_qbuf;
      tb->tb_len = 0;
    }
    return OK;
  }
out:;
  if (qb)
    free((char *) qb);

  freetblk(tb);

  return NOTOK;
}

/*  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static int
TDisconnect(tb, data, cc, td)
  register struct tsapblk *tb;
  char           *data;
  int             cc;
  struct TSAPdisconnect *td;
{
  int             result = OK;
  struct t_call  *call;

  if ((call = (struct t_call *) T_alloc(tb->tb_fd, T_CALL_STR, T_ALL)) == NULL)
    result = tli_lose(td, NOTOK, DR_CONGEST, "T_alloc");
  else {
    if (cc)
      bcopy(data, call->udata.buf, call->udata.len = (unsigned) cc);
    call->sequence = tb->tb_flags & TB_CONN ? -1 : tb->tb_dstref;
    if (t_snddis(tb->tb_fd, call) == NOTOK)
      result = tli_lose(td, NOTOK, DR_CONGEST, "t_snddis");
    T_free((char *) call, T_CALL_STR);
  }
  freetblk(tb);

  return result;
}
/*  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* ARGSUSED */

static int
TLose(tb, reason, td)
  register struct tsapblk *tb;
  int             reason;
  struct TSAPdisconnect *td;
{
  struct t_call  *call;

  SLOG(tsap_log, LLOG_EXCEPTIONS, NULLCP, ("TPM error %d", reason));

  if ((call = (struct t_call *) T_alloc(tb->tb_fd, T_CALL_STR, T_ALL)) == NULL)
    SLOG(tsap_log, LLOG_EXCEPTIONS, "TLose",
	 ("unable to allocate"));
  else {
    call->sequence = tb->tb_flags & TB_CONN ? -1 : tb->tb_dstref;
    if (t_snddis(tb->tb_fd, call) == NOTOK)
      SLOG(tsap_log, LLOG_EXCEPTIONS, "t_snddis",
	   ("unable to send"));

    T_free((char *) call, T_CALL_STR);
  }
}

/*  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * LOWER HALF */

/* ARGSUSED */

int
tp4open(tb, local_ta, local_na, remote_ta, remote_na, td, async)
  register struct tsapblk *tb;
  struct TSAPaddr *local_ta, *remote_ta;
  struct NSAPaddr *local_na, *remote_na;
  struct TSAPdisconnect *td;
  int             async;
{
  int             fd, onoff;
  struct TSAPaddr tzs;
  register struct TSAPaddr *tz = &tzs;
  register struct NSAPaddr *nz = tz->ta_addrs;

  bzero((char *) tz, sizeof *tz);
  if (local_ta)
    *tz = *local_ta;		/* struct copy */
  if (local_na) {
    *nz = *local_na;		/* struct copy */
    tz->ta_naddr = 1;
  }
  if ((fd = tp4bind(tz, 0, td, remote_na)) == NOTOK)
    return NOTOK;
  tb->tb_fd = fd;
  if (tp4info(tb, td) == NOTOK) {
    T_close(fd);
    tb->tb_fd = NOTOK;
    return NOTOK;
  }
  (void) tp4init(tb);

  if (async)
    if ((onoff = fcntl(fd, F_GETFL, 0)) != NOTOK)
      (void) fcntl(fd, F_SETFL, onoff | FNDELAY);

  return (async ? OK : DONE);
}

/*  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* ARGSUSED */

static int
retry_tp4_socket(tb, td)
  register struct tsapblk *tb;
  struct TSAPdisconnect *td;
{
  fd_set          mask;

  FD_ZERO(&mask);
  FD_SET(tb->tb_fd, &mask);
  if (xselect(tb->tb_fd + 1, &mask, NULLFD, NULLFD, 0) < 1)
    return OK;

  return DONE;
}



/*  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* ARGSUSED */

/*
 * Save string format: "fd.connection-sequence-number.expedited
 * calling-address called" better not get any spaces in result of
 * taddr2str(). 
 */

char           *
tp4save(fd, seq, exp, calling_ta, called_ta, td)
  int             fd, seq, exp;
  struct tsapADDR *calling_ta, *called_ta;
  struct TSAPdisconnect *td;
{
  struct TSAPaddr calling, called;
  static char     buffer[BUFSIZ];
  struct t_info   Info;
  struct t_info  *info = &Info;

  copyTSAPaddrX(called_ta, &called);
  copyTSAPaddrX(calling_ta, &calling);
  if (T_getinfo(fd, info) == NOTOK) {
    LLOG(tsap_log, LLOG_EXCEPTIONS,
	 ("Couldn't find transport descriptor information for %d", fd));
    return NULLCP;
  }
  (void) sprintf(buffer, "%c%d.%d.%d.%ld.%ld.%ld.%ld.%ld.%ld.%ld %s %s", NT_XTI, fd, seq, exp, info->addr, info->options, info->tsdu, info->etsdu, info->connect, info->discon, info->servtype,
		 taddr2str(&calling), taddr2str(&called));
  if (T_delinfo(fd) == NOTOK) {
    LLOG(tsap_log, LLOG_EXCEPTIONS,
	 ("Couldn't delete transport descriptor information for %d", fd));
    return NULLCP;
  }
  return buffer;
}

/*  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int
tp4restore(tb, buffer, td)
  register struct tsapblk *tb;
  char           *buffer;
  struct TSAPdisconnect *td;
{
  int             fd, exp;
  char            calling_buf[BUFSIZ];
  char            called_buf[BUFSIZ];
  struct t_info   Info;
  struct t_info  *info = &Info;

  DLOG(tsap_log, LLOG_DEBUG, ("tp4restore: \"%s\"", buffer));
  if (sscanf(buffer, "%d.%hu.%d.%ld.%ld.%ld.%ld.%ld.%ld.%ld %s %s",
	     &fd, &tb->tb_dstref, &exp, &info->addr, &info->options,
  &info->tsdu, &info->etsdu, &info->connect, &info->discon, &info->servtype,
	     calling_buf, called_buf) != 12 || fd < 0)
    return tsaplose(td, DR_PARAMETER, NULLCP,
		    "bad initialization vector \"%s\"", buffer);

  tb->tb_fd = fd;
  copyTSAPaddrY(str2taddr(calling_buf), &tb->tb_initiating);
  copyTSAPaddrY(str2taddr(called_buf), &tb->tb_responding);
  t_sync(fd);
  if (T_setinfo(fd, info) == NOTOK) {
    LLOG(tsap_log, LLOG_EXCEPTIONS,
	 ("Couldn't set transport descriptor information for %d", fd));
    return NOTOK;
  }
  if (exp)
    tb->tb_flags |= TB_EXPD;
  else
    tb->tb_flags &= ~TB_EXPD;
  (void) tp4init(tb);
  return OK;
}

/*  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int
tp4init(tb)
  register struct tsapblk *tb;
{

  tb->tb_connPfnx = TConnect;
  tb->tb_retryPfnx = TRetry;

  tb->tb_startPfnx = TStart;
  tb->tb_acceptPfnx = TAccept;

  tb->tb_writePfnx = TWrite;
  tb->tb_readPfnx = TRead;
  tb->tb_discPfnx = TDisconnect;
  tb->tb_losePfnx = TLose;

  tb->tb_drainPfnx = TDrain;

#ifdef  MGMT
  tb->tb_manfnx = TManGen;
#endif

  tb->tb_flags &= ~TB_STACKS;
  tb->tb_flags |= TB_TP4;

  tb->tb_retryfnx = retry_tp4_socket;

  tb->tb_closefnx = close_tp4_socket;
  tb->tb_selectfnx = select_tp4_socket;
}

/*  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* ARGSUSED */

int
start_tp4_server(local_ta, backlog, opt1, opt2, td)
  struct TSAPaddr *local_ta;
  int             backlog, opt1, opt2;
  struct TSAPdisconnect *td;
{
  int             sd;

  if ((sd = tp4bind(local_ta, 100, td, NULLNA)) == NOTOK)
    return NOTOK;

  return sd;
}

/*  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int
join_tp4_client(fd, remote_ta, ud, ccp, seqp, expdp, td)
  int             fd;
  struct TSAPaddr *remote_ta;
  char           *ud;
  int            *ccp;
  int            *seqp;
  int            *expdp;
  struct TSAPdisconnect *td;
{
  struct t_call  *call;
  int             sd;

  *ccp = 0;
  if ((call = (struct t_call *) T_alloc(fd, T_CALL_STR, T_ALL)) == NULL)
    return tli_lose(td, NOTOK, DR_CONGEST, "T_alloc");

  DLOG(tsap_log, LLOG_DEBUG, ("t_listen: %d", fd));
  if (t_listen(fd, call) == NOTOK) {
    DLOG(tsap_log, LLOG_DEBUG,
	 ("t_listen fail: t_errno=%d, t_look=%d", t_errno, t_look(fd)));
    T_free((char *) call, T_CALL_STR);
    if (t_errno == TLOOK && t_look(fd) == T_DISCONNECT)
      return tp4getdis(fd, td);
    else
      return tli_lose(td, NOTOK, DR_CONGEST, "t_listen");
  }
  (void) tp42gen(remote_ta, &call->addr);
  *seqp = call->sequence;
  if ((*ccp = call->udata.len) > 0)
    bcopy(call->udata.buf, ud, *ccp);
  if (call->opt.len == sizeof(struct isoco_options)) {
    struct isoco_options *isoco_options = (struct isoco_options *) call->opt.buf;
    *expdp = (isoco_options->expd == T_YES) ? 1 : 0;
  }
  T_free((char *) call, T_CALL_STR);
  sd = dup(fd);
  t_sync(sd);

  if (T_getinfo(fd, &info) != OK)
      return NOTOK;
   if (T_setinfo(sd,&info) != OK)
      return NOTOK;

  DLOG(tsap_log, LLOG_TRACE,
       ("t_listen: CON-IND from %s now on %d", taddr2str(remote_ta), sd));
  return sd;
}

/*  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
 * This implements the XTI format for netbuf used by the IBM AIX 3.1 on 
 * RS6000.  The format may be different for your implementation.
 */

struct xti_netbuf {
  unsigned short int	ts_sellen;
  char			ts_sel[32];
  unsigned char		ts_nsaplen;
  char			ts_nsap[20];
  char			dummy;
};

int
gen2tp4(generic, specific)	/* dependant on Addressing */
  struct TSAPaddr *generic;
  struct netbuf  *specific;
{
  struct xti_netbuf *xtb;

  specific -> len = 55 ;
  xtb = (struct xti_netbuf *)specific -> buf;

  bzero (xtb,sizeof(struct xti_netbuf));

  xtb -> ts_sellen = generic -> ta_selectlen;

  if (generic -> ta_selectlen)
	bcopy (generic -> ta_selector, xtb -> ts_sel, generic -> ta_selectlen);

  if (generic->ta_naddr > 0) {
    struct NSAPaddr *na = generic->ta_addrs;

    na = na2norm (generic->ta_addrs);
    bcopy(na->na_address, xtb->ts_nsap, na->na_addrlen);
    xtb->ts_nsaplen = na->na_addrlen;
  }
    
  return OK;
}

/*  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int
tp42gen(generic, specific)
  struct TSAPaddr *generic;
  struct netbuf  *specific;
{

  register struct NSAPaddr *na = generic->ta_addrs;
  struct xti_netbuf *xtb;

  bzero((char *) generic, sizeof *generic);
  bzero((char *) na, sizeof *na);

  xtb = (struct xti_netbuf *)specific->buf;

  if (generic->ta_selectlen = xtb->ts_sellen) /* assign */

    bcopy(xtb->ts_sel, generic->ta_selector, generic->ta_selectlen);

  na->na_stack = NA_NSAP;
  na->na_community = ts_comm_nsap_default;
  generic->ta_naddr++;
  if (xtb->ts_nsaplen) 
      norm2na (xtb->ts_nsap, xtb->ts_nsaplen, na);

  return OK;

}

int
close_tp4_socket(fd)
  int             fd;
{
  DLOG(tsap_log, LLOG_TRACE, ("close_tp4_socket(%d)", fd));
  return T_close(fd);
}

#else
int
_ts2xti_stub()
{;
}
#endif
