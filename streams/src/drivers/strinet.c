/*****************************************************************************

 @(#) $RCSfile: strinet.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2004/04/22 12:08:32 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 Ave, Cambridge, MA 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2004/04/22 12:08:32 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: strinet.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2004/04/22 12:08:32 $"

static char const ident[] = "$RCSfile: strinet.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2004/04/22 12:08:32 $";

/* 
 *  This driver provides the functionality of IP (Internet Protocol) over a
 *  connectionless network provider (NPI).  It provides a STREAMS-based
 *  encapsulation of the Linux IP stack.
 */

#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/modversions.h>

#include <linux/net.h>
#include <linux/in.h>
#include <linux/ip.h>

#include <net/sock.h>
#include <net/tcp.h>

#ifndef __GENKSYMS__
#include <sys/modversions.h>
#endif

#include <sys/cmn_err.h>
#include <sys/stropts.h>
#include <sys/stream.h>
#include <sys/ddi.h>

#include <sys/tihdr.h>
#include <xti_inet.h>

#if defined(CONFIG_SCTP)||defined(CONFIG_SCTP_MODULE)
#include <netinet/sctp.h>
#endif

#include "strdebug.h"

#define	INET_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define INET_COPYRIGHT	"Copyright (c) 1997-2003 OpenSS7 Corporation.  All Rights Reserved."
#define INET_REVISION	"LfS $RCSFile$ $Name:  $($Revision: 0.9.2.5 $) $Date: 2004/04/22 12:08:32 $"
#define INET_DEVICE	"SVR 4.2 XTIOS (XTI Over Sockets) for NET4"
#define INET_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define INET_LICENSE	"GPL"
#define INET_BANNER	INET_DESCRIP	"\n" \
			INET_COPYRIGHT	"\n" \
			INET_REVISION	"\n" \
			INET_DEVICE	"\n" \
			INET_CONTACT	"\n"
#define INET_SPLASH	INET_DEVICE	" - " \
			INET_REVISION	"\n"

MODULE_AUTHOR(INET_CONTACT);
MODULE_DESCRIPTION(INET_DESCRIP);
MODULE_SUPPORTED_DEVICE(INET_DEVICE);
MODULE_LICENSE(INET_LICENSE);

#ifndef CONFIG_STREAMS_INET_MAJOR
#error CONFIG_STREAMS_INET_MAJOR must be defined
#endif
#ifndef CONFIG_STREAMS_INET_NAME
#error CONFIG_STREAMS_INET_NAME must be defined
#endif
#ifndef CONFIG_STREAMS_INET_MODID
#error CONFIG_STREAMS_INET_MODID must be defined
#endif
#define IP_CMINOR	32
#define ICMP_CMINOR	33
#define GGP_CMINOR	34
#define IPIP_CMINOR	35
#define TCP_CMINOR	36
#define EGP_CMINOR	37
#define PUP_CMINOR	38
#define UDP_CMINOR	39
#define IDP_CMINOR	40
#define RAWIP_CMINOR	41
#define FREE_CMINOR	50

#ifndef CONFIG_STREAMS_INET_NMAJORS
#define CONFIG_STREAMS_INET_NMAJORS 4
#define CONFIG_STREAMS_INET_NMINORS 256
#endif

/* 
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */
STATIC struct module_info inet_minfo = {
      mi_idnum:CONFIG_STREAM_INET_MODID, /* Module ID number */
      mi_idname:CONFIG_STREAMS_INET_NAME, /* Module name */
      mi_minpsz:0,		/* Min packet size accepted */
      mi_maxpsz:INFPSZ,		/* Max packet size accepted */
      mi_hiwat:1 << 15,		/* Hi water mark */
      mi_lowat:1 << 10		/* Lo water mark */
};

STATIC int inet_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC int inet_close(queue_t *, int, cred_t *);

STATIC int inet_rput(queue_t *, mblk_t *);
STATIC int inet_rsrv(queue_t *);

STATIC struct qinit inet_rinit = {
      qi_putp:inet_rput,	/* Read put (msg from below) */
      qi_srvp:inet_rsrv,	/* Read queue service */
      qi_qopen:inet_open,	/* Each open */
      qi_qclose:inet_close,	/* Last close */
      qi_minfo:&inet_minfo,	/* Information */
};

STATIC int inet_wput(queue_t *, mblk_t *);
STATIC int inet_wsrv(queue_t *);

STATIC struct qinit inet_winit = {
      qi_putp:inet_wput,	/* Write put (msg from above) */
      qi_srvp:inet_wsrv,	/* Write queue service */
      qi_minfo:&inet_minfo,	/* Information */
};

STATIC struct streamtab inet_info = {
      st_rdinit:&inet_rinit,	/* Upper read queue */
      st_wrinit:&inet_winit,	/* Lower read queue */
};

/* 
 *  Queue put and service return values
 */
#define QR_DONE		0
#define QR_ABSORBED	1
#define QR_TRIMMED	2
#define QR_LOOP		3
#define QR_PASSALONG	4
#define QR_PASSFLOW	5
#define QR_DISABLE	6
#define QR_STRIP	7

/* 
 *  TLI interface state flags
 */
#define TSF_UNBND	( 1 << TS_UNBND		)
#define TSF_WACK_BREQ	( 1 << TS_WACK_BREQ	)
#define TSF_WACK_UREQ	( 1 << TS_WACK_UREQ	)
#define TSF_IDLE	( 1 << TS_IDLE		)
#ifdef TS_WACK_OPTREQ
#define TSF_WACK_OPTREQ	( 1 << TS_WACK_OPTREQ	)
#endif
#define TSF_WACK_CREQ	( 1 << TS_WACK_CREQ	)
#define TSF_WCON_CREQ	( 1 << TS_WCON_CREQ	)
#define TSF_WRES_CIND	( 1 << TS_WRES_CIND	)
#define TSF_WACK_CRES	( 1 << TS_WACK_CRES	)
#define TSF_DATA_XFER	( 1 << TS_DATA_XFER	)
#define TSF_WIND_ORDREL	( 1 << TS_WIND_ORDREL	)
#define TSF_WREQ_ORDREL	( 1 << TS_WREQ_ORDREL	)
#define TSF_WACK_DREQ6	( 1 << TS_WACK_DREQ6	)
#define TSF_WACK_DREQ7	( 1 << TS_WACK_DREQ7	)
#define TSF_WACK_DREQ9	( 1 << TS_WACK_DREQ9	)
#define TSF_WACK_DREQ10	( 1 << TS_WACK_DREQ10	)
#define TSF_WACK_DREQ11	( 1 << TS_WACK_DREQ11	)
#define TSF_NOSTATES	( 1 << TS_NOSTATES	)
#define TSM_WACK_DREQ	(TSF_WACK_DREQ6 \
			|TSF_WACK_DREQ7 \
			|TSF_WACK_DREQ9 \
			|TSF_WACK_DREQ10 \
			|TSF_WACK_DREQ11)
#define TSM_LISTEN	(TSF_IDLE \
			|TSF_WRES_CIND)
#define TSM_CONNECTED	(TSF_WCON_CREQ\
			|TSF_WRES_CIND\
			|TSF_DATA_XFER\
			|TSF_WIND_ORDREL\
			|TSF_WREQ_ORDREL)
#define TSM_DISCONN	(TSF_IDLE\
			|TSF_UNBND)
#define TSM_INDATA	(TSF_DATA_XFER\
			|TSF_WIND_ORDREL)
#define TSM_OUTDATA	(TSF_DATA_XFER\
			|TSF_WREQ_ORDREL)
#ifndef T_PROVIDER
#define T_PROVIDER  0
#define T_USER	    1
#endif

/* 
 *  Socket state masks
 */

/* 
 *  TCP state masks
 */
#define TCPM_CLOSING	(TCPF_CLOSE\
			|TCPF_TIME_WAIT\
			|TCPF_CLOSE_WAIT)
#define TCPM_CONNIND	(TCPF_SYN_RECV\
			|TCPF_ESTABLISHED\
			|TCPF_LISTEN)

/* 
 *  =========================================================================
 *
 *  IP Private Datastructures
 *
 *  =========================================================================
 */

typedef struct sockaddr_in inet_addr_t;

typedef struct inet_dflt {
	struct ip_options opt;		/* T_IP_OPTIONS */
	ulong tos;			/* T_IP_TOS */
	ulong ttl;			/* T_IP_TTL */
	ulong reuse;			/* T_IP_REUSE_ADDR */
	ulong norte;			/* T_IP_DONTROUTE */
	ulong bcast;			/* T_IP_BROADCAST */
	ulong nodly;			/* T_TCP_NODELAY, T_SCTP_NODELAY */
	ulong mss;			/* T_TCP_MAXSEG */
	ulong alive;			/* T_TCP_KEEPALIVE */
	ulong csum;			/* T_UDP_CHECKSUM */
#if defined(CONFIG_SCTP)||defined(CONFIG_SCTP_MODULE)
	ulong cork;			/* T_SCTP_CORK */
	ulong ppi;			/* T_SCTP_PPI */
	ulong sid;			/* T_SCTP_SID */
	ulong ssn;			/* T_SCTP_SSN */
	ulong tsn;			/* T_SCTP_TSN */
	ulong recvopt;			/* T_SCTP_RECVOPT */
	ulong clife;			/* T_SCTP_COOKIE_LIFE */
	ulong sack;			/* T_SCTP_SACK_DELAY */
	ulong prtx;			/* T_SCTP_PATH_MAX_RETRANS */
	ulong artx;			/* T_SCTP_ASSOC_MAX_RETRANS */
	ulong irtx;			/* T_SCTP_MAX_INIT_RETRIES */
	ulong hitvl;			/* T_SCTP_HEARTBEAT_ITVL */
	ulong rtoinit;			/* T_SCTP_RTO_INITIAL */
	ulong rtomin;			/* T_SCTP_RTO_MIN */
	ulong rtomax;			/* T_SCTP_RTO_MAX */
	ulong ostr;			/* T_SCTP_OSTREAMS */
	ulong istr;			/* T_SCTP_ISTREAMS */
	ulong cinc;			/* T_SCTP_COOKIE_INC */
	ulong titvl;			/* T_SCTP_THROTTLE_ITVL */
	ulong mac;			/* T_SCTP_MAC_TYPE */
#endif
} inet_dflt_t;

typedef struct inet_profile {
	struct {
		uint family;
		uint type;
		uint protocol;
	} prot;
	struct T_info_ack info;
} inet_profile_t;

typedef struct inet_protocol {
	HEAD_DECLARATION (struct inet_protocol);
	int ps_family;
	int ps_type;
	int ps_protocol;
	dev_t ps_dev;
	int ps_flags;
} inet_protocol_t;

struct inet_protocol *inet_protosw = NULL;

typedef struct inet {
	struct inet *next;		/* list of all IP-Users */
	struct inet **prev;		/* list of all IP-Users */
	size_t refcnt;			/* structure reference count */
	ushort cmajor;			/* major device number */
	ushort cminor;			/* minor device number */
	queue_t *rq;			/* associated read queue */
	queue_t *wq;			/* associated write queue */
	cred_t cred;			/* credientials */
	uint rbid;			/* RD buffer call id */
	uint wbid;			/* WR buffer call id */
	int ioc_state;			/* transparent ioctl state */
	ushort port;			/* port/protocol number */
	int tcp_state;			/* tcp state history */
	inet_profile_t p;		/* protocol profile */
	struct {
		void (*state_change) (struct sock *);
		void (*data_ready) (struct sock *, int);
		void (*write_space) (struct sock *);
		void (*error_report) (struct sock *);
	} cb_save;			/* call back holder */
	inet_addr_t src;		/* bound address */
	inet_addr_t dst;		/* connected address */
	inet_dflt_t options;		/* protocol options */
	unsigned char _pad[40];		/* pad for options */
	bufq_t conq;			/* connection queue */
	uint conind;			/* number of connection indications */
	struct socket *sock;		/* socket pointer */
} inet_t;

#define PRIV(__q) ((inet_t *)((__q)->q_ptr))
#define SOCK_PRIV(__sk) ((inet_t *)(__sk)->user_data)

typedef struct inet_opts {
	uint flags;			/* success flags */
	struct ip_options *opt;		/* T_IP_OPTIONS */
	ulong *tos;			/* T_IP_TOS */
	ulong *ttl;			/* T_IP_TTL */
	ulong *reuse;			/* T_IP_REUSE_ADDR */
	ulong *norte;			/* T_IP_DONTROUTE */
	ulong *bcast;			/* T_IP_BROADCAST */
	ulong *nodly;			/* T_TCP_NODELAY, T_SCTP_NODELAY */
	ulong *mss;			/* T_TCP_MAXSEG */
	ulong *alive;			/* T_TCP_KEEPALIVE */
	ulong *csum;			/* T_UDP_CHECKSUM */
	ulong *ropt;
	struct in_pktinfo *pkinfo;
#if defined(CONFIG_SCTP)||defined(CONFIG_SCTP_MODULE)
	ulong *cork;			/* T_SCTP_CORK */
	ulong *ppi;			/* T_SCTP_PPI */
	ulong *sid;			/* T_SCTP_SID */
	ulong *ssn;			/* T_SCTP_SSN */
	ulong *tsn;			/* T_SCTP_TSN */
	ulong *recvopt;			/* T_SCTP_RECVOPT */
	ulong *clife;			/* T_SCTP_COOKIE_LIFE */
	ulong *sack;			/* T_SCTP_SACK_DELAY */
	ulong *prtx;			/* T_SCTP_PATH_MAX_RETRANS */
	ulong *artx;			/* T_SCTP_ASSOC_MAX_RETRANS */
	ulong *irtx;			/* T_SCTP_MAX_INIT_RETRIES */
	ulong *hitvl;			/* T_SCTP_HEARTBEAT_ITVL */
	ulong *rtoinit;			/* T_SCTP_RTO_INITIAL */
	ulong *rtomin;			/* T_SCTP_RTO_MIN */
	ulong *rtomax;			/* T_SCTP_RTO_MAX */
	ulong *ostr;			/* T_SCTP_OSTREAMS */
	ulong *istr;			/* T_SCTP_ISTREAMS */
	ulong *cinc;			/* T_SCTP_COOKIE_INC */
	ulong *titvl;			/* T_SCTP_THROTTLE_ITVL */
	ulong *mac;			/* T_SCTP_MAC_TYPE */
	struct t_sctp_hb *hb;		/* T_SCTP_HB */
	struct t_sctp_rto *rto;		/* T_SCTP_RTO */
	struct t_sctp_status *stat;	/* T_SCTP_STATUS */
	ulong *debug;			/* T_SCTP_DEBUG */
#endif
} inet_opts_t;

#define ip_default_opt		{ 0, }
#define ip_default_tos		1
#define ip_default_ttl		64
#define ip_default_reuse	1
#define ip_default_norte	1
#define ip_default_bcast	1
#define tcp_default_nodly	1
#define tcp_default_mss		576
#define tcp_default_alive	1
#define udp_default_csum	0

#define TF_IP_OPTIONS			(1<< 0)
#define TF_IP_TOS			(1<< 1)
#define TF_IP_TTL			(1<< 2)
#define TF_IP_REUSEADDR			(1<< 3)
#define TF_IP_DONTROUTE			(1<< 4)
#define TF_IP_BROADCAST			(1<< 5)
#define TF_IP_PKTINFO			(1<< 6)
#define TF_IP_RETOPTS			(1<< 7)

#define TF_TCP_NODELAY			(1<< 8)
#define TF_TCP_MAXSEG			(1<< 9)
#define TF_TCP_KEEPALIVE		(1<<10)
#define TF_UDP_CHECKSUM			(1<<11)

#if defined(CONFIG_SCTP)||defined(CONFIG_SCTP_MODULE)
#define TF_SCTP_NODELAY			TF_TCP_NODELAY
#define TF_SCTP_MAXSEG			TF_TCP_MAXSEG
#define TF_SCTP_CORK			(1<<10)
#define TF_SCTP_PPI			(1<<11)
#define TF_SCTP_SID			(1<<12)
#define TF_SCTP_SSN			(1<<13)
#define TF_SCTP_TSN			(1<<14)
#define TF_SCTP_RECVOPT			(1<<15)
#define TF_SCTP_COOKIE_LIFE		(1<<16)
#define TF_SCTP_SACK_DELAY		(1<<17)
#define TF_SCTP_MAX_RETRANS		(1<<18)
#define TF_SCTP_ASSOC_MAX_RETRANS	(1<<19)
#define TF_SCTP_MAX_INIT_RETRIES	(1<<20)
#define TF_SCTP_HEARTBEAT_ITVL		(1<<21)
#define TF_SCTP_RTO_INITIAL		(1<<22)
#define TF_SCTP_RTO_MIN			(1<<23)
#define TF_SCTP_RTO_MAX			(1<<24)
#define TF_SCTP_OSTREAMS		(1<<25)
#define TF_SCTP_ISTREAMS		(1<<26)
#define TF_SCTP_COOKIE_INC		(1<<27)
#define TF_SCTP_THROTTLE_ITVL		(1<<28)
#define TF_SCTP_MAC_TYPE		(1<<29)
#define TF_SCTP_HB			(1<<30)
#define TF_SCTP_RTO			(1<<31)
#define TF_SCTP_STATUS			(1<< 6)	/* FIXME */
#define TF_SCTP_DEBUG			(1<< 7)	/* FIXME */
#endif

#define TM_OPT_SENDMSG	(	TF_IP_OPTIONS	\
			|	TF_IP_TOS	\
			|	TF_IP_TTL	\
			|	TF_IP_DONTROUTE	\
			|	TF_IP_PKTINFO	\
			)

#define _T_CHECK		1
#define _T_NEGOTIATE		2
#define _T_DEFAULT		3
#define _T_CURRENT		4

#define TF_CHECK		(1<<_T_CHECK)
#define TF_NEGOTIATE		(1<<_T_NEGOTIATE)
#define TF_DEFAULT		(1<<_T_DEFAULT)
#define TF_CURRENT		(1<<_T_CURRENT)

typedef struct inet_event {
	struct sock *sk;		/* sock (child) for event */
	int state;			/* state at time of event */
} inet_event_t;

STATIC lis_spin_lock_t inet_lock;	/* protects inet_opens lists */
STATIC inet_t *inet_opens = NULL;

#if 0

/* 
 * for later when we support default destinations and default listeners 
 */
STATIC inet_t *inet_dflt_dest = NULL;
STATIC inet_t *inet_dflt_lstn = NULL;
#endif

/* 
 *  =========================================================================
 *
 *  Socket Callbacks
 *
 *  =========================================================================
 */
STATIC void inet_state_change(struct sock *sk);
STATIC void inet_write_space(struct sock *sk);
STATIC void inet_error_report(struct sock *sk);
STATIC void inet_data_ready(struct sock *sk, int len);
STATIC void
inet_socket_put(struct socket *sock)
{
	struct sock *sk;
	if (sock) {
		if ((sk = sock->sk)) {
			write_lock(&sk->callback_lock);
			{
				inet_t *inet;
				if ((inet = SOCK_PRIV(sk))) {
					SOCK_PRIV(sk) = NULL;
					inet->refcnt--;
					sk->state_change = inet->cb_save.state_change;
					sk->data_ready = inet->cb_save.data_ready;
					sk->write_space = inet->cb_save.write_space;
					sk->error_report = inet->cb_save.error_report;
				}
			}
			write_unlock(&sock->sk->callback_lock);
		}
		sock_release(sock);
	}
}
STATIC void
inet_socket_get(struct socket *sock, inet_t * inet)
{
	struct sock *sk;
	if (sock && (sk = sock->sk)) {
		write_lock(&sock->sk->callback_lock);
		{
			SOCK_PRIV(sk) = inet;
			inet->refcnt++;
			inet->cb_save.state_change = sk->state_change;
			inet->cb_save.data_ready = sk->data_ready;
			inet->cb_save.write_space = sk->write_space;
			inet->cb_save.error_report = sk->error_report;
			sk->state_change = inet_state_change;
			sk->data_ready = inet_data_ready;
			sk->write_space = inet_write_space;
			sk->error_report = inet_error_report;
			sk->protinfo.af_inet.recverr = 1;
			inet->tcp_state = sk->state;	/* initialized tcp state */
		}
		write_unlock(&sock->sk->callback_lock);
	}
}

/* 
 *  =========================================================================
 *
 *  Buffer Allocation
 *
 *  =========================================================================
 */
/* 
 *  BUFSRV calls service routine
 *  -------------------------------------------------------------------------
 */
STATIC void
inet_bufsrv(long data)
{
	queue_t *q = (queue_t *) data;
	if (q) {
		inet_t *inet = PRIV(q);
		if (q == inet->rq)
			if (inet->rbid) {
				inet->rbid = 0;
				inet->refcnt--;
			}
		if (q == inet->wq)
			if (inet->wbid) {
				inet->wbid = 0;
				inet->refcnt--;
			}
		qenable(q);
	}
}

/* 
 *  UNBUFCALL
 *  -------------------------------------------------------------------------
 */
STATIC void
inet_unbufcall(queue_t *q)
{
	inet_t *inet = PRIV(q);
	if (inet->rbid) {
		unbufcall(xchg(&inet->rbid, 0));
		inet->refcnt--;
	}
	if (inet->wbid) {
		unbufcall(xchg(&inet->wbid, 0));
		inet->refcnt--;
	}
}

/* 
 *  ALLOCB
 *  -------------------------------------------------------------------------
 */
STATIC mblk_t *
inet_allocb(queue_t *q, size_t size, int prior)
{
	mblk_t *mp;
	if ((mp = allocb(size, prior)))
		return (mp);
	else {
		inet_t *inet = PRIV(q);
		if (q == inet->rq) {
			if (!inet->rbid) {
				inet->rbid = bufcall(size, prior, &inet_bufsrv, (long) q);
				inet->refcnt++;
			}
			return (NULL);
		}
		if (q == inet->wq) {
			if (!inet->wbid) {
				inet->wbid = bufcall(size, prior, &inet_bufsrv, (long) q);
				inet->refcnt++;
			}
			return (NULL);
		}
	}
	swerr();
	return (NULL);
}

#if 0
/* 
 *  ESBALLOC
 *  -------------------------------------------------------------------------
 */
STATIC mblk_t *
inet_esballoc(queue_t *q, unsigned char *base, size_t size, int prior, frtn_t *frtn)
{
	mblk_t *mp;
	if ((mp = esballoc(base, size, prior, frtn)))
		return (mp);
	else {
		inet_t *inet = PRIV(q);
		if (q == inet->rq) {
			if (!inet->rbid) {
				inet->rbid = esbbcall(prior, &inet_bufsrv, (long) q);
				inet->refcnt++;
			}
			return (NULL);
		}
		if (q == inet->wq) {
			if (!inet->wbid) {
				inet->wbid = esbbcall(prior, &inet_bufsrv, (long) q);
				inet->refcnt++;
			}
			return (NULL);
		}
		swerr();
		return (NULL);
	}
}
#endif

/* 
 *  =========================================================================
 *
 *  OPTION Handling
 *
 *  =========================================================================
 */
#define _T_ALIGN_SIZEOF(s) \
	((sizeof((s)) + _T_ALIGN_SIZE - 1) & ~(_T_ALIGN_SIZE - 1))
STATIC size_t
inet_opts_size(inet_t * inet, inet_opts_t * ops)
{
	size_t len = 0;
	const size_t hlen = sizeof(struct t_opthdr);	/* 32 bytes */
	if (ops) {
		if (ops->opt)
			len += hlen + _T_ALIGN_SIZEOF(*(ops->opt));
		if (ops->tos)
			len += hlen + _T_ALIGN_SIZEOF(*(ops->tos));
		if (ops->ttl)
			len += hlen + _T_ALIGN_SIZEOF(*(ops->ttl));
		if (ops->reuse)
			len += hlen + _T_ALIGN_SIZEOF(*(ops->reuse));
		if (ops->norte)
			len += hlen + _T_ALIGN_SIZEOF(*(ops->norte));
		if (ops->bcast)
			len += hlen + _T_ALIGN_SIZEOF(*(ops->bcast));
		if (inet->p.prot.protocol == T_INET_TCP) {
			if (ops->nodly)
				len += hlen + _T_ALIGN_SIZEOF(*(ops->nodly));
			if (ops->mss)
				len += hlen + _T_ALIGN_SIZEOF(*(ops->mss));
			if (ops->alive)
				len += hlen + _T_ALIGN_SIZEOF(*(ops->alive));
		}
		if (inet->p.prot.protocol == T_INET_UDP) {
			if (ops->csum)
				len += hlen + _T_ALIGN_SIZEOF(*(ops->csum));
		}
#if defined(CONFIG_SCTP)||defined(CONFIG_SCTP_MODULE)
		if (inet->p.prot.protocol == T_INET_SCTP) {
			if (ops->nodly)
				len += hlen + _T_ALIGN_SIZEOF(*(ops->nodly));
			if (ops->cork)
				len += hlen + _T_ALIGN_SIZEOF(*(ops->cork));
			if (ops->mss)
				len += hlen + _T_ALIGN_SIZEOF(*(ops->mss));
			if (ops->ppi)
				len += hlen + _T_ALIGN_SIZEOF(*(ops->ppi));
			if (ops->sid)
				len += hlen + _T_ALIGN_SIZEOF(*(ops->sid));
			if (ops->ssn)
				len += hlen + _T_ALIGN_SIZEOF(*(ops->ssn));
			if (ops->tsn)
				len += hlen + _T_ALIGN_SIZEOF(*(ops->tsn));
			if (ops->recvopt)
				len += hlen + _T_ALIGN_SIZEOF(*(ops->recvopt));
			if (ops->clife)
				len += hlen + _T_ALIGN_SIZEOF(*(ops->clife));
			if (ops->sack)
				len += hlen + _T_ALIGN_SIZEOF(*(ops->sack));
			if (ops->prtx)
				len += hlen + _T_ALIGN_SIZEOF(*(ops->prtx));
			if (ops->artx)
				len += hlen + _T_ALIGN_SIZEOF(*(ops->artx));
			if (ops->irtx)
				len += hlen + _T_ALIGN_SIZEOF(*(ops->irtx));
			if (ops->hitvl)
				len += hlen + _T_ALIGN_SIZEOF(*(ops->hitvl));
			if (ops->rtoinit)
				len += hlen + _T_ALIGN_SIZEOF(*(ops->rtoinit));
			if (ops->rtomin)
				len += hlen + _T_ALIGN_SIZEOF(*(ops->rtomin));
			if (ops->rtomax)
				len += hlen + _T_ALIGN_SIZEOF(*(ops->rtomax));
			if (ops->ostr)
				len += hlen + _T_ALIGN_SIZEOF(*(ops->ostr));
			if (ops->istr)
				len += hlen + _T_ALIGN_SIZEOF(*(ops->istr));
			if (ops->cinc)
				len += hlen + _T_ALIGN_SIZEOF(*(ops->cinc));
			if (ops->titvl)
				len += hlen + _T_ALIGN_SIZEOF(*(ops->titvl));
			if (ops->mac)
				len += hlen + _T_ALIGN_SIZEOF(*(ops->mac));
			if (ops->hb)
				len += hlen + _T_ALIGN_SIZEOF(*(ops->hb));
			if (ops->rto)
				len += hlen + _T_ALIGN_SIZEOF(*(ops->rto));
			if (ops->stat)
				len += hlen + _T_ALIGN_SIZEOF(*(ops->stat));	/* FIXME add more */
			if (ops->debug)
				len += hlen + _T_ALIGN_SIZEOF(*(ops->debug));
		}
#endif
	}
	return (len);
}
STATIC void
inet_build_opts(inet_t * inet, inet_opts_t * ops, unsigned char **p)
{
	struct t_opthdr *oh;
	const size_t hlen = sizeof(struct t_opthdr);
	const size_t olen = hlen + sizeof(t_scalar_t);
	if (ops) {
		if (ops->opt) {
			oh = ((typeof(oh)) * p)++;
			oh->len = hlen + sizeof(struct ip_options);
			oh->level = T_INET_IP;
			oh->name = T_IP_OPTIONS;
			oh->status = (ops->flags & TF_IP_OPTIONS) ? T_SUCCESS : T_FAILURE;
			*((struct ip_options *) *p) = *(ops->opt);
			*p += _T_ALIGN_SIZEOF(*ops->opt);
		}
		if (ops->tos) {
			oh = ((typeof(oh)) * p)++;
			oh->len = olen;
			oh->level = T_INET_IP;
			oh->name = T_IP_TOS;
			oh->status = (ops->flags & TF_IP_TOS) ? T_SUCCESS : T_FAILURE;
			*((t_scalar_t *) * p) = *(ops->tos);
			*p += _T_ALIGN_SIZEOF(*ops->tos);
		}
		if (ops->ttl) {
			oh = ((typeof(oh)) * p)++;
			oh->len = olen;
			oh->level = T_INET_IP;
			oh->name = T_IP_TTL;
			oh->status = (ops->flags & TF_IP_TTL) ? T_SUCCESS : T_FAILURE;
			*((t_scalar_t *) * p) = *(ops->ttl);
			*p += _T_ALIGN_SIZEOF(*ops->ttl);
		}
		if (ops->reuse) {
			oh = ((typeof(oh)) * p)++;
			oh->len = olen;
			oh->level = T_INET_IP;
			oh->name = T_IP_REUSEADDR;
			oh->status = (ops->flags & TF_IP_REUSEADDR) ? T_SUCCESS : T_FAILURE;
			*((t_scalar_t *) * p) = *(ops->reuse);
			*p += _T_ALIGN_SIZEOF(*ops->reuse);
		}
		if (ops->norte) {
			oh = ((typeof(oh)) * p)++;
			oh->len = olen;
			oh->level = T_INET_IP;
			oh->name = T_IP_DONTROUTE;
			oh->status = (ops->flags & TF_IP_DONTROUTE) ? T_SUCCESS : T_FAILURE;
			*((t_scalar_t *) * p) = *(ops->norte);
			*p += _T_ALIGN_SIZEOF(*ops->norte);
		}
		if (ops->bcast) {
			oh = ((typeof(oh)) * p)++;
			oh->len = olen;
			oh->level = T_INET_IP;
			oh->name = T_IP_BROADCAST;
			oh->status = (ops->flags & TF_IP_BROADCAST) ? T_SUCCESS : T_FAILURE;
			*((t_scalar_t *) * p) = *(ops->bcast);
			*p += _T_ALIGN_SIZEOF(*ops->bcast);
		}
		if (inet->p.prot.protocol == T_INET_TCP) {
			if (ops->nodly) {
				oh = ((typeof(oh)) * p)++;
				oh->len = olen;
				oh->level = T_INET_TCP;
				oh->name = T_TCP_NODELAY;
				oh->status = (ops->flags & TF_TCP_NODELAY) ? T_SUCCESS : T_FAILURE;
				*((t_scalar_t *) * p) = *(ops->nodly);
				*p += _T_ALIGN_SIZEOF(*ops->nodly);
			}
			if (ops->mss) {
				oh = ((typeof(oh)) * p)++;
				oh->len = olen;
				oh->level = T_INET_TCP;
				oh->name = T_TCP_MAXSEG;
				oh->status = (ops->flags & TF_TCP_MAXSEG) ? T_SUCCESS : T_FAILURE;
				*((t_scalar_t *) * p) = *(ops->mss);
				*p += _T_ALIGN_SIZEOF(*ops->mss);
			}
			if (ops->alive) {
				oh = ((typeof(oh)) * p)++;
				oh->len = olen;
				oh->level = T_INET_TCP;
				oh->name = T_TCP_KEEPALIVE;
				oh->status =
				    (ops->flags & TF_TCP_KEEPALIVE) ? T_SUCCESS : T_FAILURE;
				*((t_scalar_t *) * p) = *(ops->alive);
				*p += _T_ALIGN_SIZEOF(*ops->alive);
			}
		}
		if (inet->p.prot.protocol == T_INET_UDP) {
			if (ops->csum) {
				oh = ((typeof(oh)) * p)++;
				oh->len = olen;
				oh->level = T_INET_UDP;
				oh->name = T_UDP_CHECKSUM;
				oh->status = (ops->flags & TF_UDP_CHECKSUM) ? T_SUCCESS : T_FAILURE;
				*((t_scalar_t *) * p) = *(ops->csum);
				*p += _T_ALIGN_SIZEOF(*ops->csum);
			}
		}
	}
}
STATIC int
inet_parse_opts(inet_t * inet, inet_opts_t * ops, unsigned char *op, size_t len)
{
	struct t_opthdr *oh;
	for (oh = _T_OPT_FIRSTHDR_OFS(op, len, 0); oh; oh = _T_OPT_NEXTHDR_OFS(op, len, oh, 0)) {
		switch (oh->level) {
		case T_INET_IP:
			switch (oh->name) {
			case T_IP_OPTIONS:
				ops->opt = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->flags |= TF_IP_OPTIONS;
				continue;
			case T_IP_TOS:
				ops->tos = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->flags |= TF_IP_TOS;
				continue;
			case T_IP_TTL:
				ops->ttl = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->flags |= TF_IP_TTL;
				continue;
			case T_IP_REUSEADDR:
				ops->reuse = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->flags |= TF_IP_REUSEADDR;
				continue;
			case T_IP_DONTROUTE:
				ops->norte = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->flags |= TF_IP_DONTROUTE;
				continue;
			case T_IP_BROADCAST:
				ops->bcast = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->flags |= TF_IP_BROADCAST;
				continue;
			}
			break;
		case T_INET_TCP:
			if (inet->p.prot.protocol == T_INET_TCP)
				switch (oh->name) {
				case T_TCP_NODELAY:
					ops->nodly = (void *) _T_OPT_DATA_OFS(oh, 0);
					ops->flags |= TF_TCP_NODELAY;
					continue;
				case T_TCP_MAXSEG:
					ops->mss = (void *) _T_OPT_DATA_OFS(oh, 0);
					ops->flags |= TF_TCP_MAXSEG;
					continue;
				case T_TCP_KEEPALIVE:
					ops->alive = (void *) _T_OPT_DATA_OFS(oh, 0);
					ops->flags |= TF_TCP_KEEPALIVE;
					continue;
				}
			break;
		case T_INET_UDP:
			if (inet->p.prot.protocol == T_INET_UDP)
				switch (oh->name) {
				case T_UDP_CHECKSUM:
					ops->csum = (void *) _T_OPT_DATA_OFS(oh, 0);
					ops->flags |= TF_UDP_CHECKSUM;
					continue;
				}
			break;
#if defined(CONFIG_SCTP)||defined(CONFIG_SCTP_MODULE)
		case T_INET_SCTP:
			if (inet->p.prot.protocol == T_INET_SCTP)
				switch (oh->name) {
				case T_SCTP_NODELAY:
					ops->nodly = (void *) _T_OPT_DATA_OFS(oh, 0);
					ops->flags |= TF_SCTP_NODELAY;
					continue;
				case T_SCTP_CORK:
					ops->cork = (void *) _T_OPT_DATA_OFS(oh, 0);
					ops->flags |= TF_SCTP_CORK;
					continue;
				case T_SCTP_PPI:
					ops->ppi = (void *) _T_OPT_DATA_OFS(oh, 0);
					ops->flags |= TF_SCTP_PPI;
					continue;
				case T_SCTP_SID:
					ops->sid = (void *) _T_OPT_DATA_OFS(oh, 0);
					ops->flags |= TF_SCTP_SID;
					continue;
				case T_SCTP_SSN:
					ops->ssn = (void *) _T_OPT_DATA_OFS(oh, 0);
					ops->flags |= TF_SCTP_SSN;
					continue;
				case T_SCTP_TSN:
					ops->tsn = (void *) _T_OPT_DATA_OFS(oh, 0);
					ops->flags |= TF_SCTP_TSN;
					continue;
				case T_SCTP_RECVOPT:
					ops->recvopt = (void *) _T_OPT_DATA_OFS(oh, 0);
					ops->flags |= TF_SCTP_RECVOPT;
					continue;
				case T_SCTP_COOKIE_LIFE:
					ops->clife = (void *) _T_OPT_DATA_OFS(oh, 0);
					ops->flags |= TF_SCTP_COOKIE_LIFE;
					continue;
				case T_SCTP_SACK_DELAY:
					ops->sack = (void *) _T_OPT_DATA_OFS(oh, 0);
					ops->flags |= TF_SCTP_SACK_DELAY;
					continue;
				case T_SCTP_PATH_MAX_RETRANS:
					ops->prtx = (void *) _T_OPT_DATA_OFS(oh, 0);
					ops->flags |= TF_SCTP_MAX_RETRANS;
					continue;
				case T_SCTP_ASSOC_MAX_RETRANS:
					ops->artx = (void *) _T_OPT_DATA_OFS(oh, 0);
					ops->flags |= TF_SCTP_ASSOC_MAX_RETRANS;
					continue;
				case T_SCTP_MAX_INIT_RETRIES:
					ops->irtx = (void *) _T_OPT_DATA_OFS(oh, 0);
					ops->flags |= TF_SCTP_MAX_INIT_RETRIES;
					continue;
				case T_SCTP_HEARTBEAT_ITVL:
					ops->hitvl = (void *) _T_OPT_DATA_OFS(oh, 0);
					ops->flags |= TF_SCTP_HEARTBEAT_ITVL;
					continue;
				case T_SCTP_RTO_INITIAL:
					ops->rtoinit = (void *) _T_OPT_DATA_OFS(oh, 0);
					ops->flags |= TF_SCTP_RTO_INITIAL;
					continue;
				case T_SCTP_RTO_MIN:
					ops->rtomin = (void *) _T_OPT_DATA_OFS(oh, 0);
					ops->flags |= TF_SCTP_RTO_MIN;
					continue;
				case T_SCTP_RTO_MAX:
					ops->rtomax = (void *) _T_OPT_DATA_OFS(oh, 0);
					ops->flags |= TF_SCTP_RTO_MAX;
					continue;
				case T_SCTP_OSTREAMS:
					ops->ostr = (void *) _T_OPT_DATA_OFS(oh, 0);
					ops->flags |= TF_SCTP_OSTREAMS;
					continue;
				case T_SCTP_ISTREAMS:
					ops->istr = (void *) _T_OPT_DATA_OFS(oh, 0);
					ops->flags |= TF_SCTP_ISTREAMS;
					continue;
				case T_SCTP_COOKIE_INC:
					ops->cinc = (void *) _T_OPT_DATA_OFS(oh, 0);
					ops->flags |= TF_SCTP_COOKIE_INC;
					continue;
				case T_SCTP_THROTTLE_ITVL:
					ops->titvl = (void *) _T_OPT_DATA_OFS(oh, 0);
					ops->flags |= TF_SCTP_THROTTLE_ITVL;
					continue;
				case T_SCTP_MAC_TYPE:
					ops->mac = (void *) _T_OPT_DATA_OFS(oh, 0);
					ops->flags |= TF_SCTP_MAC_TYPE;
					continue;
				case T_SCTP_HB:
					ops->hb = (void *) _T_OPT_DATA_OFS(oh, 0);
					ops->flags |= TF_SCTP_HB;
					continue;
				case T_SCTP_RTO:
					ops->rto = (void *) _T_OPT_DATA_OFS(oh, 0);
					ops->flags |= TF_SCTP_RTO;
					continue;
				case T_SCTP_MAXSEG:
					ops->mss = (void *) _T_OPT_DATA_OFS(oh, 0);
					ops->flags |= TF_SCTP_MAXSEG;
					continue;
				case T_SCTP_STATUS:
					ops->stat = (void *) _T_OPT_DATA_OFS(oh, 0);
					ops->flags |= TF_SCTP_STATUS;
					continue;
				case T_SCTP_DEBUG:
					ops->debug = (void *) _T_OPT_DATA_OFS(oh, 0);
					ops->flags |= TF_SCTP_DEBUG;
					continue;
				}
			break;
#endif
		}
	}
	if (oh)
		return (TBADOPT);
	return (0);
}

STATIC size_t
inet_cmsg_size(inet_opts_t * ops, int flags)
{
	size_t len = 0;
	const size_t hlen = CMSG_ALIGN(sizeof(struct cmsghdr));
	if (ops) {
		if (ops->opt && flags & TF_IP_OPTIONS)
			len += hlen + CMSG_ALIGN(sizeof(*(ops->opt)));
		if (ops->tos && flags & TF_IP_TOS)
			len += hlen + CMSG_ALIGN(sizeof(*(ops->tos)));
		if (ops->ttl && flags & TF_IP_TTL)
			len += hlen + CMSG_ALIGN(sizeof(*(ops->ttl)));
		if (ops->reuse && flags & TF_IP_REUSEADDR)
			len += hlen + CMSG_ALIGN(sizeof(*(ops->reuse)));
		if (ops->norte && flags & TF_IP_DONTROUTE)
			len += hlen + CMSG_ALIGN(sizeof(*(ops->norte)));
		if (ops->bcast && flags & TF_IP_BROADCAST)
			len += hlen + CMSG_ALIGN(sizeof(*(ops->bcast)));
		if (ops->nodly && flags & TF_TCP_NODELAY)
			len += hlen + CMSG_ALIGN(sizeof(*(ops->nodly)));
		if (ops->mss && flags & TF_TCP_MAXSEG)
			len += hlen + CMSG_ALIGN(sizeof(*(ops->mss)));
		if (ops->alive && flags & TF_TCP_KEEPALIVE)
			len += hlen + CMSG_ALIGN(sizeof(*(ops->alive)));
#ifdef UDP_CHECKSUM
		if (ops->csum && flags & TF_UDP_CHECKSUM)
			len += hlen + CMSG_ALIGN(sizeof(*(ops->csum)));
#endif
#if defined(CONFIG_SCTP)||defined(CONFIG_SCTP_MODULE)
#endif
	}
	return (len);
}
STATIC void
inet_build_cmsg(inet_opts_t * ops, struct msghdr *msg, int flags)
{
	struct cmsghdr *ch;
	unsigned char *p = msg->msg_control;
	const size_t hlen = CMSG_ALIGN(sizeof(struct cmsghdr));
	if (ops) {
		if (ops->opt && flags & TF_IP_OPTIONS) {
			ch = ((typeof(ch)) p)++;
			ch->cmsg_len = hlen + CMSG_ALIGN(sizeof(*ops->opt));
			ch->cmsg_level = SOL_IP;
			ch->cmsg_type = IP_OPTIONS;
			*((typeof(ops->opt)) p) = *(ops->opt);
			p += CMSG_ALIGN(sizeof(*ops->opt));
		}
		if (ops->tos && flags & TF_IP_TOS) {
			ch = ((typeof(ch)) p)++;
			ch->cmsg_len = hlen + CMSG_ALIGN(sizeof(*ops->tos));
			ch->cmsg_level = SOL_IP;
			ch->cmsg_type = IP_TOS;
			*((typeof(ops->tos)) p) = *(ops->tos);
			p += CMSG_ALIGN(sizeof(*ops->tos));
		}
		if (ops->ttl && flags & TF_IP_TTL) {
			ch = ((typeof(ch)) p)++;
			ch->cmsg_len = hlen + CMSG_ALIGN(sizeof(*ops->ttl));
			ch->cmsg_level = SOL_IP;
			ch->cmsg_type = IP_TTL;
			*((typeof(ops->ttl)) p) = *(ops->ttl);
			p += CMSG_ALIGN(sizeof(*ops->ttl));
		}
		if (ops->reuse && flags & TF_IP_REUSEADDR) {
			ch = ((typeof(ch)) p)++;
			ch->cmsg_len = hlen + CMSG_ALIGN(sizeof(*ops->reuse));
			ch->cmsg_level = SOL_SOCKET;
			ch->cmsg_type = SO_REUSEADDR;
			*((typeof(ops->reuse)) p) = *(ops->reuse);
			p += CMSG_ALIGN(sizeof(*ops->reuse));
		}
		if (ops->norte && flags & TF_IP_DONTROUTE) {
			ch = ((typeof(ch)) p)++;
			ch->cmsg_len = hlen + CMSG_ALIGN(sizeof(*ops->norte));
			ch->cmsg_level = SOL_SOCKET;
			ch->cmsg_type = SO_DONTROUTE;
			*((typeof(ops->norte)) p) = *(ops->norte);
			p += CMSG_ALIGN(sizeof(*ops->norte));
		}
		if (ops->bcast && flags & TF_IP_BROADCAST) {
			ch = ((typeof(ch)) p)++;
			ch->cmsg_len = hlen + CMSG_ALIGN(sizeof(*ops->bcast));
			ch->cmsg_level = SOL_SOCKET;
			ch->cmsg_type = SO_BROADCAST;
			*((typeof(ops->bcast)) p) = *(ops->bcast);
			p += CMSG_ALIGN(sizeof(*ops->bcast));
		}
		if (ops->nodly && flags & TF_TCP_NODELAY) {
			ch = ((typeof(ch)) p)++;
			ch->cmsg_len = hlen + CMSG_ALIGN(sizeof(*ops->nodly));
			ch->cmsg_level = SOL_TCP;
			ch->cmsg_type = TCP_NODELAY;
			*((typeof(ops->nodly)) p) = *(ops->nodly);
			p += CMSG_ALIGN(sizeof(*ops->nodly));
		}
		if (ops->mss && flags & TF_TCP_MAXSEG) {
			ch = ((typeof(ch)) p)++;
			ch->cmsg_len = hlen + CMSG_ALIGN(sizeof(*ops->mss));
			ch->cmsg_level = SOL_TCP;
			ch->cmsg_type = TCP_MAXSEG;
			*((typeof(ops->mss)) p) = *(ops->mss);
			p += CMSG_ALIGN(sizeof(*ops->mss));
		}
		if (ops->alive && flags & TF_TCP_KEEPALIVE) {
			ch = ((typeof(ch)) p)++;
			ch->cmsg_len = hlen + CMSG_ALIGN(sizeof(*ops->alive));
			ch->cmsg_level = SOL_SOCKET;
			ch->cmsg_type = SO_KEEPALIVE;
			*((typeof(ops->alive)) p) = *(ops->alive);
			p += CMSG_ALIGN(sizeof(*ops->alive));
		}
#ifdef UDP_CHECKSUM
		if (ops->csum && flags & TF_UDP_CHECKSUM) {
			ch = ((typeof(ch)) p)++;
			ch->cmsg_len = hlen + CMSG_ALIGN(sizeof(*ops->csum));
			ch->cmsg_level = SOL_UDP;
			ch->cmsg_type = UDP_CHECKSUM;
			*((typeof(ops->csum)) p) = *(ops->csum);
			p += CMSG_ALIGN(sizeof(*ops->csum));
		}
#endif
	}
}
STATIC int
inet_parse_cmsg(struct msghdr *msg, inet_opts_t * opts)
{
	struct cmsghdr *cmsg;
	for (cmsg = CMSG_FIRSTHDR(msg); cmsg; cmsg = CMSG_NXTHDR(msg, cmsg)) {
		if (cmsg->cmsg_len < sizeof(struct cmsghdr)
		    || (unsigned long) (((char *) cmsg - (char *) msg->msg_control)
					+ cmsg->cmsg_len) > msg->msg_controllen) {
			return -EINVAL;
		}
		switch (cmsg->cmsg_level) {
		case SOL_IP:
			switch (cmsg->cmsg_type) {
			case IP_OPTIONS:
				if (cmsg->cmsg_len <= 40) {
					opts->opt = (typeof(opts->opt))
					    CMSG_DATA(cmsg);
					opts->flags |= TF_IP_OPTIONS;
				}
				break;
			case IP_TTL:
				if (cmsg->cmsg_len == sizeof(*opts->ttl)) {
					opts->ttl = (typeof(opts->ttl))
					    CMSG_DATA(cmsg);
					opts->flags |= TF_IP_TTL;
				}
				break;
			case IP_TOS:
				if (cmsg->cmsg_len == sizeof(*opts->tos)) {
					opts->tos = (typeof(opts->tos))
					    CMSG_DATA(cmsg);
					opts->flags |= TF_IP_TOS;
				}
				break;
			case IP_RETOPTS:
				if (cmsg->cmsg_len == sizeof(*opts->ropt)) {
					opts->ropt = (typeof(opts->ropt))
					    CMSG_DATA(cmsg);
					opts->flags |= TF_IP_RETOPTS;
				}
				break;
			case IP_PKTINFO:
				if (cmsg->cmsg_len == sizeof(*opts->pkinfo)) {
					opts->pkinfo = (typeof(opts->pkinfo))
					    CMSG_DATA(cmsg);
					opts->flags |= TF_IP_PKTINFO;
				}
				break;
			case IP_HDRINCL:
			case IP_ROUTER_ALERT:
			case IP_RECVOPTS:
			case IP_PKTOPTIONS:
			case IP_MTU_DISCOVER:
			case IP_RECVERR:
			case IP_RECVTTL:
			case IP_RECVTOS:
			case IP_MTU:
				break;
			}
			break;
		case SOL_TCP:
			switch (cmsg->cmsg_type) {
			case TCP_NODELAY:
			case TCP_MAXSEG:
			case TCP_CORK:
			case TCP_KEEPIDLE:
			case TCP_KEEPINTVL:
			case TCP_KEEPCNT:
			case TCP_SYNCNT:
			case TCP_LINGER2:
			case TCP_DEFER_ACCEPT:
			case TCP_WINDOW_CLAMP:
			case TCP_INFO:
			case TCP_QUICKACK:
				break;
			}
			break;
		case SOL_UDP:
			switch (cmsg->cmsg_type) {
			}
			break;
#if defined(CONFIG_SCTP)||defined(CONFIG_SCTP_MODULE)
		case SOL_SCTP:
			switch (cmsg->cmsg_type) {
			case SCTP_NODELAY:
			case SCTP_MAXSEG:
			case SCTP_CORK:
			case SCTP_RECVSID:
			case SCTP_RECVPPI:
			case SCTP_RECVTSN:
			case SCTP_SID:
			case SCTP_PPI:
			case SCTP_SSN:
			case SCTP_TSN:
			case SCTP_HB:
			case SCTP_RTO:
			case SCTP_COOKIE_LIFE:
			case SCTP_SACK_DELAY:
			case SCTP_PATH_MAX_RETRANS:
			case SCTP_ASSOC_MAX_RETRANS:
			case SCTP_MAX_INIT_RETRIES:
			case SCTP_HEARTBEAT_ITVL:
			case SCTP_RTO_INITIAL:
			case SCTP_RTO_MIN:
			case SCTP_RTO_MAX:
			case SCTP_OSTREAMS:
			case SCTP_ISTREAMS:
			case SCTP_COOKIE_INC:
			case SCTP_THROTTLE_ITVL:
			case SCTP_MAC_TYPE:
			case SCTP_CKSUM_TYPE:
			case SCTP_DEBUG_OPTIONS:
			case SCTP_STATUS:
			case SCTP_ALI:
			case SCTP_PR:
			case SCTP_DISPOSITION:
			case SCTP_LIFETIME:
			case SCTP_ADD:
			case SCTP_ADD_IP:
			case SCTP_DEL_IP:
			case SCTP_SET:
			case SCTP_SET_IP:
			case SCTP_ECN:
			case SCTP_MAX_BURST:
				break;
			}
			break;
#endif
		default:
			break;
		}
	}
	return (0);
}

/* 
 *  =========================================================================
 *
 *  OPTIONS Handling
 *
 *  =========================================================================
 */
STATIC inet_dflt_t opt_defaults = {
	ip_default_opt,
	ip_default_tos,
	ip_default_ttl,
	ip_default_reuse,
	ip_default_norte,
	ip_default_bcast,
	tcp_default_nodly,
	tcp_default_mss,
	tcp_default_alive,
	udp_default_csum
};

/* 
 *  CHECK Options
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
inet_opt_check(inet_t * inet, inet_opts_t * opt)
{
	if (opt->flags) {
		opt->flags = 0;
		if (opt->opt)
			opt->flags |= TF_IP_OPTIONS;
		if (opt->tos && !(*(opt->tos) & 0x1e))
			opt->flags |= TF_IP_TOS;
		if (opt->ttl && !(*(opt->ttl) & 0xff))
			opt->flags |= TF_IP_TTL;
		if (opt->reuse)
			opt->flags |= TF_IP_REUSEADDR;
		if (opt->norte)
			opt->flags |= TF_IP_DONTROUTE;
		if (opt->bcast)
			opt->flags |= TF_IP_BROADCAST;
		if (inet->p.prot.protocol == T_INET_TCP) {
			if (opt->nodly)
				opt->flags |= TF_TCP_NODELAY;
			if (opt->mss)
				opt->flags |= TF_TCP_MAXSEG;
			if (opt->alive)
				opt->flags |= TF_TCP_KEEPALIVE;
		}
		if (inet->p.prot.protocol == T_INET_UDP) {
			if (opt->csum)
				opt->flags |= TF_UDP_CHECKSUM;
		}
	}
	return (0);
}

/* 
 *  DEFAULT Options
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
inet_opt_default(inet_t * inet, inet_opts_t * opt)
{
	int flags = opt->flags;
	opt->flags = 0;
	if (!flags || opt->tos) {
		opt->tos = &opt_defaults.tos;
		opt->flags |= TF_IP_TOS;
	}
	if (!flags || opt->ttl) {
		opt->ttl = &opt_defaults.ttl;
		opt->flags |= TF_IP_TTL;
	}
	if (!flags || opt->reuse) {
		opt->reuse = &opt_defaults.reuse;
		opt->flags |= TF_IP_REUSEADDR;
	}
	if (!flags || opt->norte) {
		opt->norte = &opt_defaults.norte;
		opt->flags |= TF_IP_DONTROUTE;
	}
	if (!flags || opt->bcast) {
		opt->bcast = &opt_defaults.bcast;
		opt->flags |= TF_IP_BROADCAST;
	}
	if (inet->p.prot.protocol == T_INET_TCP) {
		if (!flags || opt->nodly) {
			opt->nodly = &opt_defaults.nodly;
			opt->flags |= TF_TCP_NODELAY;
		}
		if (!flags || opt->mss) {
			opt->mss = &opt_defaults.mss;
			opt->flags |= TF_TCP_MAXSEG;
		}
		if (!flags || opt->alive) {
			opt->alive = &opt_defaults.alive;
			opt->flags |= TF_TCP_KEEPALIVE;
		}
	}
	if (inet->p.prot.protocol == T_INET_UDP) {
		if (!flags || opt->csum) {
			opt->csum = &opt_defaults.csum;
			opt->flags |= TF_UDP_CHECKSUM;
		}
	}
	return (0);
}

/* 
 *  NEGOTIATE Options
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
inet_opt_negotiate(inet_t * inet, inet_opts_t * opt)
{
	struct sock *sk = NULL;
	if (inet->sock)
		sk = inet->sock->sk;
	if (opt) {
		opt->flags = 0;
		if (opt->opt) {
			inet->options.opt = *(opt->opt);
			if (sk)
				sk->protinfo.af_inet.opt = &inet->options.opt;
			opt->flags |= TF_IP_OPTIONS;
		}
		if (opt->tos) {
			inet->options.tos = *(opt->tos);
			if (sk)
				sk->protinfo.af_inet.tos = inet->options.tos;
			opt->flags |= TF_IP_TOS;
		}
		if (opt->ttl) {
			inet->options.ttl = *(opt->ttl);
			if (sk)
				sk->protinfo.af_inet.ttl = inet->options.ttl;
			opt->flags |= TF_IP_TTL;
		}
		if (opt->reuse) {
			inet->options.reuse = *(opt->reuse) ? 1 : 0;
			if (sk)
				sk->reuse = inet->options.reuse;
			opt->flags |= TF_IP_REUSEADDR;
		}
		if (opt->norte) {
			inet->options.norte = *(opt->norte) ? 1 : 0;
			if (sk)
				sk->localroute = inet->options.norte;
			opt->flags |= TF_IP_DONTROUTE;
		}
		if (opt->bcast) {
			inet->options.bcast = *(opt->bcast) ? 1 : 0;
			if (sk)
				sk->broadcast = inet->options.bcast;
			opt->flags |= TF_IP_BROADCAST;
		}
		if (opt->nodly) {
			inet->options.nodly = *(opt->nodly) ? 1 : 0;
			if (sk)
				sk->tp_pinfo.af_tcp.nonagle = inet->options.nodly;
			opt->flags |= TF_TCP_NODELAY;
		}
		if (opt->mss) {
			inet->options.mss = *(opt->mss) ? 1 : 0;
			if (sk)
				sk->tp_pinfo.af_tcp.user_mss = inet->options.mss;
			opt->flags |= TF_TCP_MAXSEG;
		}
		if (opt->alive) {
			inet->options.alive = *(opt->alive) ? 1 : 0;
			if (sk)
				sk->keepopen = inet->options.alive;
			opt->flags |= TF_TCP_KEEPALIVE;
		}
		if (opt->csum) {
			inet->options.csum = *(opt->csum) ? 1 : 0;
			// if ( sk ) sk->csum = inet->options.csum;
			opt->flags |= TF_UDP_CHECKSUM;
		}
	}
	return (0);
}

/* 
 *  CURRENT Options
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
inet_opt_current(inet_t * inet, inet_opts_t * opt)
{
	uint flags = opt->flags;
	opt->flags = 0;
	if (!flags || opt->opt) {
		opt->opt = &inet->options.opt;
		opt->flags |= TF_IP_OPTIONS;
	}
	if (!flags || opt->tos) {
		opt->tos = &inet->options.tos;
		opt->flags |= TF_IP_TOS;
	}
	if (!flags || opt->ttl) {
		opt->ttl = &inet->options.ttl;
		opt->flags |= TF_IP_TTL;
	}
	if (!flags || opt->reuse) {
		opt->reuse = &inet->options.reuse;
		opt->flags |= TF_IP_REUSEADDR;
	}
	if (!flags || opt->norte) {
		opt->norte = &inet->options.norte;
		opt->flags |= TF_IP_DONTROUTE;
	}
	if (!flags || opt->bcast) {
		opt->bcast = &inet->options.bcast;
		opt->flags |= TF_IP_BROADCAST;
	}
	if (inet->p.prot.protocol == T_INET_TCP) {
		if (!flags || opt->nodly) {
			opt->nodly = &inet->options.nodly;
			opt->flags |= TF_TCP_NODELAY;
		}
		if (!flags || opt->mss) {
			opt->mss = &inet->options.mss;
			opt->flags |= TF_TCP_MAXSEG;
		}
		if (!flags || opt->alive) {
			opt->alive = &inet->options.alive;
			opt->flags |= TF_TCP_KEEPALIVE;
		}
	}
	if (inet->p.prot.protocol == T_INET_UDP) {
		if (!flags || opt->csum) {
			opt->csum = &inet->options.csum;
			opt->flags |= TF_UDP_CHECKSUM;
		}
	}
	return (0);
}

/* 
 *  =========================================================================
 *
 *  STATE Changes
 *
 *  =========================================================================
 */
#ifdef _DEBUG
STATIC const char *
state_name(long state)
{
	switch (state) {
	case TS_UNBND:
		return ("TS_UNBND");
	case TS_WACK_BREQ:
		return ("TS_WACK_BREQ");
	case TS_WACK_UREQ:
		return ("TS_WACK_UREQ");
	case TS_IDLE:
		return ("TS_IDLE");
	case TS_WACK_OPTREQ:
		return ("TS_WACK_OPTREQ");
	case TS_WACK_CREQ:
		return ("TS_WACK_CREQ");
	case TS_WCON_CREQ:
		return ("TS_WCON_CREQ");
	case TS_WRES_CIND:
		return ("TS_WRES_CIND");
	case TS_WACK_CRES:
		return ("TS_WACK_CRES");
	case TS_DATA_XFER:
		return ("TS_DATA_XFER");
	case TS_WIND_ORDREL:
		return ("TS_WIND_ORDREL");
	case TS_WREQ_ORDREL:
		return ("TS_WREQ_ORDREL");
	case TS_WACK_DREQ6:
		return ("TS_WACK_DREQ6");
	case TS_WACK_DREQ7:
		return ("TS_WACK_DREQ7");
	case TS_WACK_DREQ9:
		return ("TS_WACK_DREQ9");
	case TS_WACK_DREQ10:
		return ("TS_WACK_DREQ10");
	case TS_WACK_DREQ11:
		return ("TS_WACK_DREQ11");
	case TS_NOSTATES:
		return ("TS_NOSTATES");
	default:
		return ("(unknown)");
	}
}
#endif
STATIC void
inet_set_state(inet_t * inet, long state)
{
	inet->p.info.CURRENT_state = state;
}
STATIC long
inet_get_state(inet_t * inet)
{
	return (inet->p.info.CURRENT_state);
}

#ifdef _DEBUG
STATIC const char *
tcp_state_name(int state)
{
	switch (state) {
	case TCP_ESTABLISHED:
		return ("TCP_ESTABLISHED");
	case TCP_SYN_SENT:
		return ("TCP_SYN_SENT");
	case TCP_SYN_RECV:
		return ("TCP_SYN_RECV");
	case TCP_FIN_WAIT1:
		return ("TCP_FIN_WAIT1");
	case TCP_FIN_WAIT2:
		return ("TCP_FIN_WAIT2");
	case TCP_TIME_WAIT:
		return ("TCP_TIME_WAIT");
	case TCP_CLOSE:
		return ("TCP_CLOSE");
	case TCP_CLOSE_WAIT:
		return ("TCP_CLOSE_WAIT");
	case TCP_LAST_ACK:
		return ("TCP_LAST_ACK");
	case TCP_LISTEN:
		return ("TCP_LISTEN");
	case TCP_CLOSING:
		return ("TCP_CLOSING");
	case TCP_MAX_STATES:
		return ("TCP_MAX_STATES");
	default:
		return ("(unknown)");
	}
}
#endif

/* 
 *  ------------------------------------------------------------------------
 *
 *  Socket Calls
 *
 *  ------------------------------------------------------------------------
 *  These are wrappered versions of socket calls.
 */
/* 
 *  SOCKET CREATE
 *  ------------------------------------------------------------------------
 */
STATIC int
inet_socket(inet_t * inet)
{
	int err;
	int family, type, protocol;
	if (!inet)
		return (-EFAULT);
	if (!inet->sock)
		return (-EFAULT);
	family = inet->p.prot.family;
	type = inet->p.prot.type;
	protocol = (inet->p.prot.protocol == IPPROTO_RAW) ? inet->port : inet->p.prot.protocol;
	if (!(err = sock_create(family, type, protocol, &inet->sock))) {
		if (!inet->sock)
			return (-EFAULT);
		if (!inet->sock->sk)
			return (-EFAULT);
		inet->sock->sk->allocation = GFP_ATOMIC;
		inet_socket_get(inet->sock, inet);
		return (0);
	}
	return (err);
}

/* 
 *  SOCKET BIND
 *  ------------------------------------------------------------------------
 */
STATIC int
inet_bind(inet_t * inet, inet_addr_t * add)
{
	int err;
	if (!inet || !inet->sock || !inet->sock->sk || !inet->sock->ops || !inet->sock->ops->bind)
		return (-EFAULT);
	if (!(err = inet->sock->ops->bind(inet->sock, (struct sockaddr *) add, sizeof(*add)))) {
		inet->src = *add;
		return (0);
	}
	return (err);
}

/* 
 *  SOCKET LISTEN
 *  ------------------------------------------------------------------------
 */
STATIC int
inet_listen(inet_t * inet, uint cons)
{
	int err;
	int type;
	if (!inet || !inet->sock || !inet->sock->sk || !inet->sock->ops || !inet->sock->ops->listen)
		return (-EFAULT);
	type = inet->p.prot.type;
	if (type != SOCK_STREAM && type != SOCK_SEQPACKET)
		return (-EFAULT);
	if (!(err = inet->sock->ops->listen(inet->sock, cons))) {
		inet->conind = cons;
		inet->tcp_state = inet->sock->sk->state;
		return (0);
	}
	return (err);
}

/* 
 *  SOCKET ACCEPT
 *  ------------------------------------------------------------------------
 *  Unfortunately, sock->ops->accept will only accept the next sequential
 *  connection indication.  In TLI's case, we want to be able to accept or
 *  release connection indications other than the next sequential indication.
 *  To do this we must muck with TCP's accept queue when the SEQ_number is not
 *  the next in the queue.  To do this we mimic some of the tcp_accept
 *  behavior.
 */
STATIC int
inet_accept(inet_t * inet, struct socket **newsock, mblk_t *cp)
{
	struct socket *sock;
	if (!newsock || !cp || !inet || !inet->sock || !inet->sock->sk || !inet->sock->ops
	    || !inet->sock->ops->accept)
		return (-EFAULT);
	if ((sock = sock_alloc())) {
		struct sock *sk = inet->sock->sk;
		struct tcp_opt *tp = &sk->tp_pinfo.af_tcp;
		struct open_request *req, *req_prev, **reqp;
		struct sock *ask = ((inet_event_t *) cp->b_rptr)->sk;
		sock->type = inet->sock->type;
		sock->ops = inet->sock->ops;
		lock_sock(sk);
		if (tp->accept_queue) {
			/* find connection in queue */
			for (reqp = &tp->accept_queue, req_prev = NULL; *reqp && (*reqp)->sk != ask;
			     req_prev = (*reqp), reqp = &(*reqp)->dl_next) ;
			if ((req = *reqp)) {
				if (!((*reqp) = (*reqp)->dl_next))
					tp->accept_queue_tail = req_prev;
				sk->ack_backlog--;
				tcp_openreq_fastfree(req);
			}
			release_sock(sk);
			lock_sock(ask);
			sock_graft(ask, sock);
			release_sock(ask);
			sock->state = INET_CONNECTED;
			*newsock = sock;
			bufq_unlink(&inet->conq, cp);
			freemsg(cp);
			return (0);
		}
		release_sock(sk);
		inet_socket_put(sock);
		return (-EAGAIN);
	}
	return (-EFAULT);
}

/* 
 *  SOCKET UNBIND
 *  ------------------------------------------------------------------------
 *  There is no good way to unbind and rebind a socket in Linux, so we just
 *  close the entire socket.  The next time we go to bind, we will create
 *  a fresh socket to bind.
 */
STATIC int
inet_unbind(inet_t * inet)
{
	if (!inet || !inet->sock || !inet->sock->sk)
		return (-EFAULT);
	inet_socket_put(xchg(&inet->sock, NULL));
	return (0);
}

/* 
 *  SOCKET CONNECT
 *  ------------------------------------------------------------------------
 */
STATIC int
inet_connect(inet_t * inet, inet_addr_t * dst)
{
	int err;
	if (!inet || !inet->sock || !inet->sock->sk || !inet->sock->ops
	    || !inet->sock->ops->connect)
		return (-EFAULT);
	if ((err =
	     inet->sock->ops->connect(inet->sock, (struct sockaddr *) dst, sizeof(*dst),
				      O_NONBLOCK)) == 0 || err == -EINPROGRESS)
		return (0);
	return (err);
}

/* 
 *  SOCKET SENDMSG
 *  ------------------------------------------------------------------------
 */
STATIC int
inet_sendmsg(inet_t * inet, struct msghdr *msg, int len)
{
	int res;
	if (!inet || !inet->sock || !inet->sock->ops || !inet->sock->ops->sendmsg || !inet->sock->sk
	    || !inet->sock->sk->prot || !inet->sock->sk->prot->sendmsg)
		return (-EFAULT);
	{
		mm_segment_t fs = get_fs();
		set_fs(KERNEL_DS);
		res = sock_sendmsg(inet->sock, msg, len);
		set_fs(fs);
	}
	return (res);
}

/* 
 *  SOCKET RECVMSG
 *  ------------------------------------------------------------------------
 */
STATIC int
inet_recvmsg(inet_t * inet, struct msghdr *msg, int size)
{
	int res;
	int sflags = MSG_DONTWAIT | MSG_NOSIGNAL;
	if (!inet || !inet->sock || !inet->sock->ops || !inet->sock->ops->recvmsg || !inet->sock->sk
	    || !inet->sock->sk->prot || !inet->sock->sk->prot->recmsg)
		return (-EFAULT);
	{
		mm_segment_t fs = get_fs();
		set_fs(KERNEL_DS);
		res = sock_recvmsg(inet->sock, msg, size, sflags);
		set_fs(fs);
	}
	return (res);
}

/* 
 *  SOCKET DISCONNECT
 *  ------------------------------------------------------------------------
 *  Performing a sock_release (inet_socket_put) from the established state does
 *  not affect an abortive release for TCP, but rather, initiates an orderly
 *  shutdown rather than an abortive release.  We can try performing a
 *  protocol disconnect and see if that works better.
 */
STATIC int
inet_disconnect(inet_t * inet)
{
	int err;
	if (!inet || !inet->sock->ops || !inet->sock->ops->connect || !inet->sock->sk
	    || !inet->sock->sk->prot || !inet->sock->sk->prot->disconnect)
		return (-EFAULT);
	if (!(err = inet->sock->sk->prot->disconnect(inet->sock->sk, O_NONBLOCK))) {
		inet->sock->state = INET_UNCONNECTED;
		return (0);
	}
	inet->sock->state = INET_DISCONNECTING;
	return (err);
}

/* 
 *  =========================================================================
 *
 *  IP T-Provider --> T-User Primitives (Indication, Confirmation and Ack)
 *
 *  =========================================================================
 */
/* 
 *  M_ERROR
 *  ---------------------------------------------------------------
 */
STATIC int
m_error(queue_t *q, int error)
{
	inet_t *inet = PRIV(q);
	mblk_t *mp;
	int hangup = 0;
	if (error < 0)
		error = -error;
	switch (error) {
	case EBUSY:
	case ENOBUFS:
	case EAGAIN:
	case ENOMEM:
		return (-error);
	case EPIPE:
	case ENETDOWN:
	case EHOSTUNREACH:
		hangup = 1;
	}
	if ((mp = inet_allocb(q, 2, BPRI_HI))) {
		if (inet->sock)
			inet_socket_put(xchg(&inet->sock, NULL));
		if (hangup) {
			mp->b_datap->db_type = M_HANGUP;
			putnext(inet->rq, mp);
			error = EPIPE;
		} else {
			mp->b_datap->db_type = M_ERROR;
			*(mp->b_wptr)++ = error;
			*(mp->b_wptr)++ = error;
			putnext(inet->rq, mp);
		}
		return (-error);
	}
	rare();
	return (-ENOBUFS);
}

/* 
 *  T_CONN_IND          11 - Connection Indication
 *  ---------------------------------------------------------------
 */
STATIC int
t_conn_ind(queue_t *q, inet_addr_t * src, inet_opts_t * opts, mblk_t *cp)
{
	inet_t *inet = PRIV(q);
	mblk_t *mp;
	struct T_conn_ind *p;
	size_t src_len = src ? sizeof(*src) : 0;
	size_t opt_len = inet_opts_size(inet, opts);
	if (bufq_length(&inet->conq) <= inet->conind) {
		if (canputnext(inet->rq)) {
			if ((mp = inet_allocb(q, sizeof(*p) + src_len + opt_len, BPRI_MED))) {
				mp->b_datap->db_type = M_PROTO;
				p = ((typeof(p)) mp->b_wptr)++;
				p->PRIM_type = T_CONN_IND;
				p->SRC_length = src_len;
				p->SRC_offset = src_len ? sizeof(*p) : 0;
				p->OPT_length = opt_len;
				p->OPT_offset = opt_len ? sizeof(*p) + src_len : 0;
				p->SEQ_number = (ulong) cp;
				if (src_len) {
					bcopy(src, mp->b_wptr, src_len);
					mp->b_wptr += src_len;
				}
				if (opt_len) {
					inet_build_opts(inet, opts, &mp->b_wptr);
				}
				bufq_queue(&inet->conq, cp);
				inet_set_state(inet, TS_WRES_CIND);
				putnext(inet->rq, mp);
				return (QR_ABSORBED);	/* absorbed cp */
			}
			return (-ENOBUFS);
		}
		return (-EBUSY);
	}
	return (-EAGAIN);
}

/* 
 *  T_CONN_CON          12 - Connection Confirmation
 *  ---------------------------------------------------------------
 */
STATIC int
t_conn_con(queue_t *q, inet_addr_t * res, inet_opts_t * opts, mblk_t *dp)
{
	inet_t *inet = PRIV(q);
	mblk_t *mp;
	struct T_conn_con *p;
	size_t res_len = res ? sizeof(*res) : 0;
	size_t opt_len = inet_opts_size(inet, opts);
	/* 
	 * this shouldn't happen, we probably shouldn't even check 
	 */
	if (canputnext(inet->rq)) {
		if ((mp = inet_allocb(q, sizeof(*p) + res_len + opt_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = 1;	/* expedite */
			p = ((typeof(p)) mp->b_wptr)++;
			p->PRIM_type = T_CONN_CON;
			p->RES_length = res_len;
			p->RES_offset = res_len ? sizeof(*p) : 0;
			p->OPT_length = opt_len;
			p->OPT_offset = opt_len ? sizeof(*p) + res_len : 0;
			if (res_len) {
				bcopy(res, mp->b_wptr, res_len);
				mp->b_wptr += res_len;
			}
			if (opt_len) {
				inet_build_opts(inet, opts, &mp->b_wptr);
			}
			inet_set_state(inet, TS_DATA_XFER);
			putnext(inet->rq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/* 
 *  T_DISCON_IND        13 - Disconnect Indication
 *  ---------------------------------------------------------------
 */
STATIC mblk_t *
t_seq_find(inet_t * inet, mblk_t *rp)
{
	mblk_t *mp;
	if ((mp = rp)) {
		struct sock *sk = ((inet_event_t *) rp->b_rptr)->sk;
		lis_spin_lock(&inet->conq.q_lock);
		{
			for (mp = bufq_head(&inet->conq);
			     mp && ((inet_event_t *) mp->b_rptr)->sk != sk; mp = mp->b_next) ;
		}
		lis_spin_unlock(&inet->conq.q_lock);
	}
	return (mp);
}
STATIC ulong
t_seq_delete(inet_t * inet, mblk_t *rp)
{
	mblk_t *mp;
	if ((mp = t_seq_find(inet, rp))) {
		struct socket *sock = NULL;
		if (!inet_accept(inet, &sock, rp) && sock)
			sock_release(sock);
		return ((ulong) mp);
	}
	return (0);
}
STATIC int
t_discon_ind(queue_t *q, inet_addr_t * res, uint orig, uint reason, mblk_t *cp, mblk_t *dp)
{
	inet_t *inet = PRIV(q);
	mblk_t *mp;
	struct T_discon_ind *p;
	ulong seq = 0;
	(void) res;
	if (canputnext(inet->rq)) {
		if ((mp = inet_allocb(q, sizeof(*p), BPRI_MED))) {
			if (!cp || (seq = t_seq_delete(inet, cp))) {
				mp->b_datap->db_type = M_PROTO;
				p = ((typeof(p)) mp->b_wptr)++;
				p->PRIM_type = T_DISCON_IND;
				p->DISCON_reason = reason;
				p->SEQ_number = seq;
				if (!bufq_length(&inet->conq))
					inet_set_state(inet, TS_IDLE);
				else
					inet_set_state(inet, TS_WRES_CIND);
				mp->b_cont = dp;
				putnext(inet->rq, mp);
				return (0);
			}
			freemsg(mp);
			return (-EFAULT);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/* 
 *  T_DATA_IND          14 - Data Indication
 *  ---------------------------------------------------------------
 */
STATIC int
t_data_ind(queue_t *q, uint flags, mblk_t *dp)
{
	inet_t *inet = PRIV(q);
	mblk_t *mp;
	struct T_data_ind *p;
	if ((mp = inet_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->PRIM_type = T_DATA_IND;
		p->MORE_flag = flags;
		mp->b_cont = dp;
		putnext(inet->rq, mp);
		return (QR_ABSORBED);
	}
	return (-ENOBUFS);
}

/* 
 *  T_EXDATA_IND        15 - Expedited Data Indication
 *  ---------------------------------------------------------------
 */
STATIC int
t_exdata_ind(queue_t *q, uint flags, mblk_t *dp)
{
	inet_t *inet = PRIV(q);
	mblk_t *mp;
	struct T_exdata_ind *p;
	if ((mp = inet_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		mp->b_band = 1;	/* expedite */
		p = ((typeof(p)) mp->b_wptr)++;
		p->PRIM_type = T_EXDATA_IND;
		p->MORE_flag = flags;
		mp->b_cont = dp;
		putnext(inet->rq, mp);
		return (QR_ABSORBED);
	}
	return (-ENOBUFS);
}

/* 
 *  T_INFO_ACK          16 - Information acknowledgement
 *  ---------------------------------------------------------------
 */
STATIC int
t_info_ack(queue_t *q)
{
	inet_t *inet = PRIV(q);
	mblk_t *mp;
	struct T_info_ack *p;
	if ((mp = inet_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		*p = inet->p.info;
		putnext(inet->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/* 
 *  T_BIND_ACK          17 - Bind Acknowledgement
 *  ---------------------------------------------------------------
 */
STATIC int
t_bind_ack(queue_t *q, inet_addr_t * add, ulong conind)
{
	inet_t *inet = PRIV(q);
	mblk_t *mp;
	struct T_bind_ack *p;
	size_t add_len = add ? sizeof(*add) : 0;
	if ((mp = inet_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->PRIM_type = T_BIND_ACK;
		p->ADDR_length = add_len;
		p->ADDR_offset = add_len ? sizeof(*p) : 0;
		p->CONIND_number = conind;
		if (add_len) {
			bcopy(add, mp->b_wptr, add_len);
			mp->b_wptr += add_len;
		}
		inet_set_state(inet, TS_IDLE);
		putnext(inet->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/* 
 *  T_ERROR_ACK         18 - Error Acknowledgement
 *  -------------------------------------------------------------------------
 */
STATIC int
t_error_ack(queue_t *q, ulong prim, long error)
{
	inet_t *inet = PRIV(q);
	mblk_t *mp;
	struct T_error_ack *p;
	switch (error) {
	case -EBUSY:
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
		seldom();
		return (error);
	case 0:
		never();
		return (error);
	}
	if (!(mp = inet_allocb(q, sizeof(*p), BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = T_ERROR_ACK;
	p->ERROR_prim = prim;
	p->TLI_error = error < 0 ? TSYSERR : error;
	p->UNIX_error = error < 0 ? -error : 0;
	/* 
	 *  This is to only try and get the state correct for putnext.
	 */
	if (error != TOUTSTATE) {
		switch (inet_get_state(inet)) {
#ifdef TS_WACK_OPTREQ
		case TS_WACK_OPTREQ:
			inet_set_state(inet, TS_IDLE);
			break;
#endif
		case TS_WACK_UREQ:
			inet_set_state(inet, TS_IDLE);
			break;
		case TS_WACK_CREQ:
			inet_set_state(inet, TS_IDLE);
			break;
		case TS_WACK_BREQ:
			inet_set_state(inet, TS_UNBND);
			break;
		case TS_WACK_CRES:
			inet_set_state(inet, TS_WRES_CIND);
			break;
		case TS_WACK_DREQ6:
			inet_set_state(inet, TS_WCON_CREQ);
			break;
		case TS_WACK_DREQ7:
			inet_set_state(inet, TS_WRES_CIND);
			break;
		case TS_WACK_DREQ9:
			inet_set_state(inet, TS_DATA_XFER);
			break;
		case TS_WACK_DREQ10:
			inet_set_state(inet, TS_WIND_ORDREL);
			break;
		case TS_WACK_DREQ11:
			inet_set_state(inet, TS_WREQ_ORDREL);
			break;
			/* 
			 *  Note: if we are not in a WACK state we simply do
			 *  not change state.  This occurs normally when we
			 *  are responding to a T_OPTMGMT_REQ in other than
			 *  TS_IDLE state.
			 */
		}
	}
	putnext(inet->rq, mp);
	/* 
	 *  Returning -EPROTO here will make sure that the old state is
	 *  restored correctly.  If we return QR_DONE, then the state will
	 *  never be restored.
	 */
	if (error < 0)
		return (error);
	return (-EPROTO);
      enobufs:
	return (-ENOBUFS);
}

/* 
 *  T_OK_ACK            19 - Success Acknowledgement
 *  -------------------------------------------------------------------------
 */
STATIC int
t_ok_ack(queue_t *q, ulong prim, mblk_t *cp, inet_t * as)
{
	int err = -EFAULT;
	inet_t *inet = PRIV(q);
	mblk_t *mp;
	struct T_ok_ack *p;
	struct socket *sock = NULL;
	if ((mp = inet_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->PRIM_type = T_OK_ACK;
		p->CORRECT_prim = prim;
		switch (inet_get_state(inet)) {
		case TS_WACK_CREQ:
			if ((err = inet_connect(inet, &inet->dst)))
				goto free_error;
			inet_set_state(inet, TS_WCON_CREQ);
			break;
		case TS_WACK_UREQ:
			if ((err = inet_unbind(inet)))
				goto free_error;
			inet_set_state(inet, TS_UNBND);
			break;
		case TS_WACK_CRES:
			if (!as || !cp)
				goto free_error;
			if ((err = inet_accept(inet, &sock, cp)))
				goto free_error;
			if (as->sock)
				inet_socket_put(as->sock);	/* get rid of old socket */
			as->sock = sock;
			inet_socket_get(as->sock, as);
			inet_set_state(as, TS_DATA_XFER);
			if (bufq_length(&inet->conq))
				inet_set_state(inet, TS_WRES_CIND);
			else {
				inet_set_state(inet, TS_IDLE);
				/* make sure any backed up indications are processed */
				qenable(inet->rq);
			}
			break;
		case TS_WACK_DREQ7:
			if (!cp)
				goto free_error;
			if (!(err = inet_accept(inet, &sock, cp)))
				sock_release(sock);
			if (bufq_length(&inet->conq))
				inet_set_state(inet, TS_WRES_CIND);
			else {
				inet_set_state(inet, TS_IDLE);
				/* make sure any backed up indications are processed */
				qenable(inet->rq);
			}
			break;
		case TS_WACK_DREQ6:
		case TS_WACK_DREQ9:
		case TS_WACK_DREQ10:
		case TS_WACK_DREQ11:
			if ((err = inet_disconnect(inet)))
				goto free_error;
			inet_set_state(inet, TS_IDLE);
			break;
		default:
			break;
			/* 
			 *  Note: if we are not in a WACK state we simply do
			 *  not change state.  This occurs normally when we
			 *  are responding to a T_OPTMGMT_REQ in other than
			 *  the TS_IDLE state.
			 */
		}
		putnext(inet->rq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
      free_error:
	freemsg(mp);
	return t_error_ack(q, prim, err);
}

/* 
 *  T_UNIDATA_IND       20 - Unitdata indication
 *  -------------------------------------------------------------------------
 */
STATIC int
t_unitdata_ind(queue_t *q, inet_addr_t * src, inet_opts_t * opts, mblk_t *dp)
{
	inet_t *inet = PRIV(q);
	mblk_t *mp;
	struct T_unitdata_ind *p;
	size_t src_len = src ? sizeof(*src) : 0;
	size_t opt_len = inet_opts_size(inet, opts);
	if ((mp = inet_allocb(q, sizeof(*p) + src_len + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->PRIM_type = T_UNITDATA_IND;
		p->SRC_length = src_len;
		p->SRC_offset = src_len ? sizeof(*p) : 0;
		p->OPT_length = opt_len;
		p->OPT_offset = opt_len ? sizeof(*p) + src_len : 0;
		if (src_len) {
			bcopy(src, mp->b_wptr, src_len);
			mp->b_wptr += src_len;
		}
		if (opt_len) {
			inet_build_opts(inet, opts, &mp->b_wptr);
		}
		mp->b_cont = dp;
		putnext(inet->rq, mp);
		return (QR_ABSORBED);
	}
	return (-ENOBUFS);
}

/* 
 *  T_UDERROR_IND       21 - Unitdata Error Indication
 *  -------------------------------------------------------------------------
 *  This primitive indicates to the transport user that a datagram with the
 *  specified destination address and options produced an error.
 */
STATIC INLINE int
t_uderror_ind(queue_t *q, inet_addr_t * src, inet_addr_t * dst, inet_opts_t * opts, mblk_t *dp,
	      int etype)
{
	inet_t *inet = PRIV(q);
	mblk_t *mp;
	struct T_uderror_ind *p;
	size_t dst_len = dst ? sizeof(*dst) : 0;
	size_t opt_len = inet_opts_size(inet, opts);
	(void) src;
	if (canputnext(inet->rq)) {
		if ((mp = inet_allocb(q, sizeof(*p) + dst_len + opt_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = 2;	/* XXX move ahead of data indications */
			p = ((typeof(p)) mp->b_wptr)++;
			p->PRIM_type = T_UDERROR_IND;
			p->DEST_length = dst_len;
			p->DEST_offset = dst_len ? sizeof(*p) : 0;
			p->OPT_length = opt_len;
			p->OPT_offset = opt_len ? sizeof(*p) + dst_len : 0;
			p->ERROR_type = etype;
			if (dst_len) {
				bcopy(dst, mp->b_wptr, dst_len);
				mp->b_wptr += dst_len;
			}
			if (opt_len) {
				inet_build_opts(inet, opts, &mp->b_wptr);
			}
			putnext(inet->rq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/* 
 *  T_OPTMGMT_ACK       22 - Options Management Acknowledge
 *  -------------------------------------------------------------------------
 */
STATIC int
t_optmgmt_ack(queue_t *q, ulong flags, inet_opts_t * opts)
{
	inet_t *inet = PRIV(q);
	mblk_t *mp;
	struct T_optmgmt_ack *p;
	size_t opt_len = inet_opts_size(inet, opts);
	if ((mp = inet_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->PRIM_type = T_OPTMGMT_ACK;
		p->OPT_length = opt_len;
		p->OPT_offset = opt_len ? sizeof(*p) : 0;
		p->MGMT_flags = flags;
		if (opt_len) {
			inet_build_opts(inet, opts, &mp->b_wptr);
		}
#ifdef TS_WACK_OPTREQ
		if (inet_get_state(inet) == TS_WACK_OPTREQ)
			inet_set_state(inet, TS_IDLE);
#endif
		putnext(inet->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/* 
 *  T_ORDREL_IND        23 - Orderly Release Indication
 *  -------------------------------------------------------------------------
 */
STATIC int
t_ordrel_ind(queue_t *q)
{
	inet_t *inet = PRIV(q);
	mblk_t *mp;
	struct T_ordrel_ind *p;
	if (canputnext(inet->rq)) {
		if ((mp = inet_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->PRIM_type = T_ORDREL_IND;
			switch (inet_get_state(inet)) {
			case TS_DATA_XFER:
				inet_set_state(inet, TS_WREQ_ORDREL);
				break;
			case TS_WIND_ORDREL:
				inet_set_state(inet, TS_IDLE);
				break;
			}
			putnext(inet->rq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/* 
 *  T_OPTDATA_IND       26 - Data with Options Indication
 *  -------------------------------------------------------------------------
 */
STATIC int
t_optdata_ind(queue_t *q, uint flags, inet_opts_t * opts, mblk_t *dp)
{
	inet_t *inet = PRIV(q);
	mblk_t *mp;
	struct T_optdata_ind *p;
	size_t opt_len = inet_opts_size(inet, opts);
	if ((mp = inet_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		mp->b_band = flags & T_ODF_EX ? 1 : 0;	/* expedite */
		p = ((typeof(p)) mp->b_wptr)++;
		p->PRIM_type = T_OPTDATA_IND;
		p->DATA_flag = flags & (T_ODF_MORE | T_ODF_EX);
		p->OPT_length = opt_len;
		p->OPT_offset = opt_len ? sizeof(*p) : 0;
		if (opt_len) {
			inet_build_opts(inet, opts, &mp->b_wptr);
		}
		mp->b_cont = dp;
		putnext(inet->rq, mp);
		return (QR_ABSORBED);
	}
	return (-ENOBUFS);
}

#ifdef T_ADDR_ACK
/* 
 *  T_ADDR_ACK          27 - Address Acknowledgement
 *  -------------------------------------------------------------------------
 */
STATIC int
t_addr_ack(queue_t *q, inet_addr_t * loc, inet_addr_t * rem)
{
	inet_t *inet = PRIV(q);
	mblk_t *mp;
	struct T_addr_ack *p;
	size_t loc_len = loc ? sizeof(*loc) : 0;
	size_t rem_len = rem ? sizeof(*rem) : 0;
	if ((mp = inet_allocb(q, sizeof(*p) + loc_len + rem_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->PRIM_type = T_ADDR_ACK;
		p->LOCADDR_length = loc_len;
		p->LOCADDR_offset = loc_len ? sizeof(*p) : 0;
		p->REMADDR_length = rem_len;
		p->REMADDR_offset = rem_len ? sizeof(*p) + loc_len : 0;
		if (loc_len) {
			bcopy(loc, mp->b_wptr, loc_len);
			mp->b_wptr += loc_len;
		}
		if (rem_len) {
			bcopy(rem, mp->b_wptr, rem_len);
			mp->b_wptr += rem_len;
		}
		putnext(inet->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
#endif
#ifdef T_CAPABILITY_ACK
/* 
 *  T_CAPABILITY_ACK    ?? - Protocol Capability Ack
 *  -------------------------------------------------------------------------
 */
STATIC int
t_capability_ack(queue_t *q, ulong caps)
{
	inet_t *inet = PRIV(q);
	mblk_t *mp;
	struct T_capability_ack *p;
	if ((mp = inet_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->PRIM_type = T_CAPABILITY_ACK;
		p->CAP_bits1 = TC1_INFO;
		p->ACCPETOR_id = (caps & TC1_ACCEPTOR) ? (ulong) inet->rq : 0;
		if (caps & TC1_INFO)
			p->INFO_ack = inet->p.info;
		else
			bzero(&p->INFO_ack, sizeof(p->INFO_ack));
		putnext(inet->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
#endif

/* 
 *  =========================================================================
 *
 *  CONNECTION INNDICATIONS
 *
 *  =========================================================================
 */
/* 
 *  CONIND
 *  -------------------------------------------------------------------------
 *  An unfortunate mis-match between the socket model and the TLI model is
 *  that in TLI it is possible to determine the remote address from a
 *  T_CONN_IND *before* accepting the connection.  Under sockets, the
 *  connection must be accepted before it is possible to determine the peer's
 *  address.  This means that connection must be accepted and then released,
 *  rather than refused.
 */
STATIC int
inet_conn_ind(queue_t *q, mblk_t *cp)
{
	inet_t *inet = PRIV(q);
	struct sock *sk;
	struct inet_event *p = (typeof(p)) cp->b_rptr;
	inet_addr_t dst = { AF_UNSPEC, 0, };
	inet_opts_t opt = { 0, };
	if ((1 << inet_get_state(inet)) & ~TSM_LISTEN)
		goto outstate;
	if (!inet->conind || inet->sock->sk->state != TCP_LISTEN)
		goto nolisten;
	if (cp->b_wptr < cp->b_rptr + sizeof(*p))
		goto einval;
	if (inet->sock->sk == p->sk)
		goto duplicate;
	sk = p->sk;
	dst.sin_family = AF_INET;
	dst.sin_port = sk->dport;
	dst.sin_addr.s_addr = sk->daddr;
	opt.opt = sk->protinfo.af_inet.opt;
	opt.tos = (ulong *) &sk->protinfo.af_inet.tos;
	opt.ttl = (ulong *) &sk->protinfo.af_inet.ttl;
	opt.reuse = (ulong *) &sk->reuse;
	opt.norte = (ulong *) &sk->localroute;
	opt.bcast = (ulong *) &sk->broadcast;
	if (opt.opt)
		opt.flags |= TF_IP_OPTIONS;
	opt.flags |= TF_IP_TOS;
	opt.flags |= TF_IP_TTL;
	opt.flags |= TF_IP_REUSEADDR;
	opt.flags |= TF_IP_DONTROUTE;
	opt.flags |= TF_IP_BROADCAST;
	if (inet->p.prot.protocol == T_INET_TCP) {
		opt.nodly = (ulong *) &sk->tp_pinfo.af_tcp.nonagle;
		opt.mss = (ulong *) &sk->tp_pinfo.af_tcp.user_mss;
		opt.alive = (ulong *) &sk->keepopen;
		opt.flags |= TF_TCP_NODELAY;
		opt.flags |= TF_TCP_MAXSEG;
		opt.flags |= TF_TCP_KEEPALIVE;
	}
	if (inet->p.prot.protocol == T_INET_SCTP) {
		/* 
		 * skip for now 
		 */
	}
	return t_conn_ind(q, &dst, &opt, cp);
      einval:
	swerr();
	return (-EFAULT);
      outstate:
	return (-EPROTO);
      nolisten:
	return (-EPROTO);
      duplicate:
	return (QR_DONE);
}

/* 
 *  =========================================================================
 *
 *  SENDMSG and RECVMSG
 *
 *  =========================================================================
 */
/* 
 *  SENDMSG
 *  -------------------------------------------------------------------------
 *  Convert the mblk to send into an iovec and the options into a control
 *  message and then call sendmsg on the socket with the kernel data segment.
 *  The socket will handle moving data from the mblks.
 */
STATIC int
inet_sock_sendmsg(inet_t * inet, mblk_t *mp, inet_addr_t * dst, int flags, inet_opts_t * opts)
{
	int err = 0;
	int len, sdu, n;
	mblk_t *dp;
	for (len = 0, n = 0, dp = mp; dp; dp = dp->b_cont) {
		if (dp->b_datap->db_type == M_DATA && dp->b_wptr > dp->b_rptr) {
			len += dp->b_wptr - dp->b_rptr;
			n++;
		}
	}
	err = -EPROTO;
	if (!(inet->p.info.PROVIDER_flag & T_SNDZERO) && (!n || !len))
		goto out;
	if ((sdu = (int) inet->p.info.TSDU_size) > 0 && (!n || len > sdu))
		goto out;
	err = -EBUSY;
	if (len > sock_wspace(inet->sock->sk))
		goto out;
	/* 
	 *  This has the ramification that we can never do zero length writes,
	 *  but we have ~T_SNDZERO set anyway.
	 */
	{
		int i;
		struct iovec iov[n];
		struct msghdr msg;
		size_t clen = opts ? inet_cmsg_size(opts, TF_IP_OPTIONS | TF_IP_PKTINFO) : 0;
		unsigned char cbuf[clen > 1024 ? 1024 : clen];
		flags |= (MSG_DONTWAIT | MSG_NOSIGNAL);
		flags |= inet->options.norte ? MSG_DONTROUTE : 0;
		msg.msg_name = dst;
		msg.msg_namelen = dst ? sizeof(*dst) : 0;
		msg.msg_iov = iov;
		msg.msg_iovlen = n;
		msg.msg_control = clen ? cbuf : NULL;
		msg.msg_controllen = clen;
		msg.msg_flags = flags;
		/* 
		 * convert message blocks to an iovec 
		 */
		for (i = 0, dp = mp; dp; dp = dp->b_cont) {
			if (dp->b_datap->db_type == M_DATA && dp->b_wptr > dp->b_rptr) {
				iov[i].iov_base = dp->b_rptr;
				iov[i].iov_len = dp->b_wptr - dp->b_rptr;
				i++;
			}
		}
		if (opts) {
			inet_build_cmsg(opts, &msg, TF_IP_OPTIONS | TF_IP_PKTINFO);
			inet_opt_negotiate(inet, opts);
		}
		err = inet_sendmsg(inet, &msg, len);
	}
	if (err < 0)
		goto out;
	if (!err) {
		err = -EBUSY;
		goto out;
	}
	if (err >= len)
		return (QR_DONE);	/* full write */
	switch (inet->p.prot.type) {	/* partial write */
	case SOCK_DGRAM:
	case SOCK_RAW:
	case SOCK_RDM:
		err = -EMSGSIZE;
		goto out;
	case SOCK_SEQPACKET:
	case SOCK_STREAM:
		adjmsg(mp, err);
		err = -EAGAIN;
		goto out;
	}
	err = -EFAULT;
      out:
	switch (-err) {
	case ENOMEM:
		/* 
		 *  This buffer call is just to kick us.  Because LiS uses kmalloc for
		 *  mblks, if we can allocate an mblk, then another kernel routine can
		 *  allocate that much memory too.
		 */
		if (inet->wbid) {
			unbufcall(xchg(&inet->wbid, 0));
			inet->refcnt--;
		}
		inet->wbid = bufcall(len, BPRI_LO, &inet_bufsrv, (long) inet->wq);
		inet->refcnt++;
	default:
		return m_error(inet->rq, err);
	}
}

/* 
 *  RECVMSG
 *  -------------------------------------------------------------------------
 */
STATIC int
inet_sock_recvmsg(queue_t *q)
{
	inet_t *inet = PRIV(q);
	mblk_t *mp;
	int more, expd, res, oldsize = 0x7fffffff, size = 0;
	struct msghdr msg = { NULL, };
	unsigned char cbuf[128] = { 0, };
	inet_addr_t src = { AF_INET, 0, {0}, };
	struct sk_buff *skb;
	while (canputnext(inet->rq)) {
		switch (inet->p.prot.type) {
		case SOCK_DGRAM:
		case SOCK_RAW:
		case SOCK_RDM:
		case SOCK_STREAM:
		{
			size = (skb = skb_peek(&inet->sock->sk->receive_queue)) ? skb->len : 0;
			break;
		}
		case SOCK_SEQPACKET:
			size = atomic_read(&inet->sock->sk->rmem_alloc);
			break;
		}
		if (!size || size >= oldsize)
			return (QR_DONE);
		oldsize = size;
		if (!(mp = inet_allocb(inet->rq, size, BPRI_MED)))
			return (-ENOBUFS);
		{
			struct iovec iov[1];
			iov[0].iov_base = mp->b_rptr;
			iov[0].iov_len = size;
			if (inet->p.prot.type != SOCK_STREAM) {
				msg.msg_name = &src;
				msg.msg_namelen = sizeof(src);
			} else {
				msg.msg_name = NULL;
				msg.msg_namelen = 0;
			}
			msg.msg_iov = iov;
			msg.msg_iovlen = sizeof(iov) / sizeof(struct iovec);
			msg.msg_control = cbuf;
			msg.msg_controllen = sizeof(cbuf);
			msg.msg_flags = 0;
			if ((res = inet_recvmsg(inet, &msg, size)) < 0) {
				freemsg(mp);
				return (res);
			}
			if (res == 0) {
				freemsg(mp);
				return (QR_DONE);
			}
			mp->b_wptr = mp->b_rptr + res;
		}
#if 0
		if (msg.msg_flags & MSG_TRUNC) {
			freemsg(mp);
			return (-EMSGSIZE);
		}
#endif
		{
			int err;
			inet_opts_t opt = { 0, };
			if (msg.msg_controllen)
				inet_parse_cmsg(&msg, &opt);
			more = (msg.msg_flags & MSG_EOR) ? 0 : T_ODF_MORE;
			expd = (msg.msg_flags & MSG_OOB) ? T_ODF_EX : 0;
			if (inet->p.info.SERV_type == T_CLTS) {
				if ((err = t_unitdata_ind(q, &src, &opt, mp)) != QR_ABSORBED)
					return (err);
			} else if (!opt.flags) {
				if (msg.msg_flags & MSG_OOB) {
					if ((err = t_exdata_ind(q, more, mp)) != QR_ABSORBED)
						return (err);
				} else {
					if ((err = t_data_ind(q, more, mp)) != QR_ABSORBED)
						return (err);
				}
			} else {
				if ((err = t_optdata_ind(q, more | expd, &opt, mp)) != QR_ABSORBED)
					return (err);
			}
		}
	}
	return (-EBUSY);
}

/* 
 *  =========================================================================
 *
 *  STATE MACHINE
 *
 *  =========================================================================
 */
/* 
 *  -------------------------------------------------------------------------
 *
 *  Socket CALLBACKS
 *
 *  -------------------------------------------------------------------------
 */
STATIC void
inet_putctl(inet_t * inet, queue_t *q, int type, void (*func) (long), struct sock *sk)
{
	int flags;
	mblk_t *mp;
	inet_event_t *p;
	if ((mp = allocb(sizeof(*p), BPRI_HI))) {
		mp->b_datap->db_type = type;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sk = sk;
		p->state = sk->state;	/* capture current state */
		putq(q, mp);
		return (void) (0);
	}
	/* 
	 * set up bufcall so we don't lose events 
	 */
	/* 
	 * make sure bufcalls don't happen now 
	 */
	if (q == inet->rq) {
		if (inet->rbid)
			unbufcall(xchg(&inet->rbid, 0));
		inet->rbid = bufcall(FASTBUF, BPRI_HI, func, (long) sk);
	} else if (q == inet->wq) {
		if (inet->wbid)
			unbufcall(xchg(&inet->wbid, 0));
		inet->wbid = bufcall(FASTBUF, BPRI_HI, func, (long) sk);
	}
}

/* 
 *  STATE change
 *  -------------------------------------------------------------------------
 *  We get state changes on sockets that we hold.  We also get state changes
 *  on accepting sockets.
 */
STATIC void
_ss_sock_state_change(long data)
{
	struct sock *sk;
	inet_t *inet;
	if ((sk = (struct sock *) data)) {
		if ((inet = SOCK_PRIV(sk))) {
			read_lock(&sk->callback_lock);
			inet_putctl(inet, inet->rq, M_PCRSE, &_ss_sock_state_change, sk);
			read_unlock(&sk->callback_lock);
		}
	}
}
STATIC void
inet_state_change(struct sock *sk)
{
	if (sk) {
		inet_t *inet;
		if ((inet = SOCK_PRIV(sk))) {
			_ss_sock_state_change((long) sk);
			// inet->cb_save.state_change(sk);
		}
	}
}

/* 
 *  WRITE Available
 *  -------------------------------------------------------------------------
 */
STATIC void
_ss_sock_write_space(long data)
{
	struct sock *sk;
	inet_t *inet;
	if ((sk = (struct sock *) data)) {
		if ((inet = SOCK_PRIV(sk))) {
			read_lock(&sk->callback_lock);
			inet_putctl(inet, inet->wq, M_READ, &_ss_sock_write_space, sk);
			read_unlock(&sk->callback_lock);
		}
	}
}
STATIC void
inet_write_space(struct sock *sk)
{
	if (sk) {
		inet_t *inet;
		if ((inet = SOCK_PRIV(sk))) {
			_ss_sock_write_space((long) sk);
			// inet->cb_save.write_space(sk);
		}
	}
}

/* 
 *  ERROR Available
 *  -------------------------------------------------------------------------
 */
STATIC void
_ss_sock_error_report(long data)
{
	struct sock *sk;
	inet_t *inet;
	if ((sk = (struct sock *) data)) {
		if ((inet = SOCK_PRIV(sk))) {
			read_lock(&sk->callback_lock);
			inet_putctl(inet, inet->rq, M_ERROR, &_ss_sock_error_report, sk);
			read_unlock(&sk->callback_lock);
		}
	}
}
STATIC void
inet_error_report(struct sock *sk)
{
	if (sk) {
		inet_t *inet;
		if ((inet = SOCK_PRIV(sk))) {
			_ss_sock_error_report((long) sk);
			// inet->cb_save.error_report(sk);
		}
	}
}

/* 
 *  READ Available
 *  -------------------------------------------------------------------------
 */
STATIC void
_ss_sock_data_ready(long data)
{
	struct sock *sk;
	inet_t *inet;
	if ((sk = (struct sock *) data)) {
		if ((inet = SOCK_PRIV(sk))) {
			read_lock(&sk->callback_lock);
			inet_putctl(inet, inet->rq, M_READ, &_ss_sock_data_ready, sk);
			read_unlock(&sk->callback_lock);
		}
	}
}
STATIC void
inet_data_ready(struct sock *sk, int len)
{
	if (sk) {
		inet_t *inet;
		if ((inet = SOCK_PRIV(sk))) {
			(void) len;
			_ss_sock_data_ready((long) sk);
			// inet->cb_save.data_ready(sk, len);
		}
	}
}

/* 
 *  =========================================================================
 *
 *  IP T-User --> T-Provider Primitives (Request and Response)
 *
 *  =========================================================================
 *
 *  T_CONN_REQ           0 - TC Request
 *  -------------------------------------------------------------------
 */
STATIC int
t_conn_req(queue_t *q, mblk_t *mp)
{
	int err = -EFAULT;
	inet_t *inet = PRIV(q);
	const struct T_conn_req *p = (typeof(p)) mp->b_rptr;
	if (inet->p.info.SERV_type == T_CLTS)
		goto notsupport;
	if (inet_get_state(inet) != TS_IDLE)
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p)
	    || mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length
	    || mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
		goto einval;
	else {
		inet_addr_t *dst = (typeof(dst)) (mp->b_rptr + p->DEST_offset);
		if (p->DEST_length < sizeof(*dst))
			goto badaddr;
		if (dst->sin_port == 0)
			goto badaddr;
		if (dst->sin_family != inet->p.prot.family)
			goto badaddr;
		if (inet->cred.cr_uid != 0 && htons(dst->sin_port) == IPPROTO_RAW)
			goto acces;
		else {
			unsigned char *opt = mp->b_rptr + p->OPT_offset;
			size_t opt_len = p->OPT_length;
			struct inet_opts opts = { 0L, NULL, };
			if (opt_len && inet_parse_opts(inet, &opts, opt, opt_len))
				goto badopt;
			/* 
			 * TODO: set options first 
			 */
			inet->dst = *dst;
			if (mp->b_cont) {
				putbq(q, mp->b_cont);	/* hold back data */
				mp->b_cont = NULL;	/* abosrbed mp->b_cont */
			}
			inet_set_state(inet, TS_WACK_CREQ);
			return t_ok_ack(q, T_CONN_REQ, NULL, NULL);
		}
	}
      badopt:
	err = TBADOPT;
	goto error;
      acces:
	err = TACCES;
	goto error;
      badaddr:
	err = TBADADDR;
	goto error;
      einval:
	err = -EINVAL;
	goto error;
      outstate:
	err = TOUTSTATE;
	goto error;
      notsupport:
	err = TNOTSUPPORT;
	goto error;
      error:
	return t_error_ack(q, T_CONN_REQ, err);
}

/* 
 *  T_CONN_RES           1 - Accept previous connection indication
 *  -------------------------------------------------------------------
 */
STATIC mblk_t *
t_seq_check(inet_t * inet, ulong seq)
{
	mblk_t *mp;
	lis_spin_lock(&inet->conq.q_lock); {
		for (mp = bufq_head(&inet->conq); mp && mp != (mblk_t *) seq; mp = mp->b_next) ;
	}
	lis_spin_unlock(&inet->conq.q_lock);
	usual(mp);
	return (mp);
}
STATIC inet_t *
t_tok_check(ulong acceptor)
{
	inet_t *as;
	queue_t *aq = (queue_t *) acceptor;
	for (as = inet_opens; as && as->rq != aq; as = as->next) ;
	usual(as);
	return (as);
}
STATIC int
t_conn_res(queue_t *q, mblk_t *mp)
{
	inet_t *inet = PRIV(q);
	int err = 0;
	mblk_t *cp;
	inet_t *as;
	const size_t mlen = mp->b_wptr - mp->b_rptr;
	const struct T_conn_res *p = (typeof(p)) mp->b_rptr;
	struct inet_opts opts = { 0L, NULL, };
	size_t opt_len, opt_off;
	unsigned char *opt;
	if (inet->p.info.SERV_type == T_CLTS)
		goto notsupport;
	if (inet_get_state(inet) != TS_WRES_CIND)
		goto outstate;
	inet_set_state(inet, TS_WACK_CRES);
	if (mlen < sizeof(*p))
		goto inval;
	if ((cp = t_seq_check(inet, p->SEQ_number)) == NULL)
		goto badseq;
	if (((as = t_tok_check(p->ACCEPTOR_id)) == NULL)
	    || !(as == inet || ((1 << inet_get_state(as)) & TSM_DISCONN)))
		goto badf;
	if (inet->p.prot.protocol != as->p.prot.protocol)
		goto provmismatch;
	if (inet_get_state(as) == TS_IDLE && as->conind)
		goto resqlen;
	if (inet->cred.cr_uid != 0 && as->cred.cr_uid == 0)
		goto acces;
	opt_len = p->OPT_length;
	opt_off = p->OPT_offset;
	opt = mp->b_rptr + opt_off;
	if (opt_len
	    && (mlen < opt_off + opt_len || (err = inet_parse_opts(inet, &opts, opt, opt_len))))
		goto badopt;
	if (mp->b_cont != NULL && msgdsize(mp->b_cont))
		goto baddata;
	return t_ok_ack(q, T_CONN_RES, cp, as);
      baddata:
	err = TBADDATA;
	goto error;
      badopt:
	err = TBADOPT;
	goto error;
      acces:
	err = TACCES;
	goto error;
      resqlen:
	err = TRESQLEN;
	goto error;
      provmismatch:
	err = TPROVMISMATCH;
	goto error;
      badf:
	err = TBADF;
	goto error;
      badseq:
	err = TBADSEQ;
	goto error;
      inval:
	err = -EINVAL;
	goto error;
      outstate:
	err = TOUTSTATE;
	goto error;
      notsupport:
	err = TNOTSUPPORT;
	goto error;
      error:
	return t_error_ack(q, T_CONN_RES, err);
}

/* 
 *  T_DISCON_REQ         2 - TC disconnection request
 *  -------------------------------------------------------------------
 */
STATIC int
t_discon_req(queue_t *q, mblk_t *mp)
{
	inet_t *inet = PRIV(q);
	int err;
	mblk_t *cp = NULL;
	struct T_discon_req *p = (typeof(p)) mp->b_rptr;
	if (inet->p.info.SERV_type == T_CLTS)
		goto notsupport;
	if ((1 << inet_get_state(inet)) & ~TSM_CONNECTED)
		goto outstate;
	if (mp->b_wptr - mp->b_rptr < sizeof(*p))
		goto einval;
	switch (inet_get_state(inet)) {
	case TS_WCON_CREQ:
		inet_set_state(inet, TS_WACK_DREQ6);
		break;
	case TS_WRES_CIND:
		if ((cp = t_seq_check(inet, p->SEQ_number))) {
			inet_set_state(inet, TS_WACK_DREQ7);
			break;
		}
		goto badseq;
	case TS_DATA_XFER:
		inet_set_state(inet, TS_WACK_DREQ9);
		break;
	case TS_WIND_ORDREL:
		inet_set_state(inet, TS_WACK_DREQ10);
		break;
	case TS_WREQ_ORDREL:
		inet_set_state(inet, TS_WACK_DREQ11);
		break;
	}
	return t_ok_ack(q, T_DISCON_REQ, cp, NULL);
      badseq:
	err = TBADSEQ;
	goto error;
      einval:
	err = -EINVAL;
	goto error;
      outstate:
	err = TOUTSTATE;
	goto error;
      notsupport:
	err = TNOTSUPPORT;
	goto error;
      error:
	return t_error_ack(q, T_DISCON_REQ, err);
}

/* 
 *  M_DATA
 *  -------------------------------------------------------------------
 */
STATIC int
inet_w_data(queue_t *q, mblk_t *mp)
{
	int flags, mlen, mmax;
	inet_t *inet = PRIV(q);
	if (inet->p.info.SERV_type == T_CLTS)
		goto notsupport;
	if (inet_get_state(inet) == TS_IDLE)
		goto discard;
	if ((1 << inet_get_state(inet)) & ~TSM_OUTDATA)
		goto outstate;
	mlen = msgdsize(mp);
	mmax = inet->p.info.TIDU_size;
	if (mmax < inet->p.info.TSDU_size && inet->p.info.TSDU_size != T_INVALID)
		mmax = inet->p.info.ETSDU_size;
	if (mlen > mmax)
		goto emsgsize;
	flags = (inet->p.prot.type == SOCK_SEQPACKET) ? MSG_EOR : 0;
	return inet_sock_sendmsg(inet, mp, NULL, flags, NULL);
      emsgsize:
	goto error;
      outstate:
	goto error;
      discard:
	return (QR_DONE);
      notsupport:
	goto error;
      error:
	return m_error(q, EPROTO);
}

/* 
 *  T_DATA_REQ           3 - Connection-Mode data transfer request
 *  -------------------------------------------------------------------
 */
STATIC int
t_data_req(queue_t *q, mblk_t *mp)
{
	int flags, mlen, mmax;
	inet_t *inet = PRIV(q);
	const struct T_data_req *p = (typeof(p)) mp->b_rptr;
	if (inet->p.info.SERV_type == T_CLTS)
		goto notsupport;
	if (inet_get_state(inet) == TS_IDLE)
		goto discard;
	if (mp->b_wptr - mp->b_rptr < sizeof(*p))
		goto einval;
	if ((1 << inet_get_state(inet)) & ~TSM_OUTDATA)
		goto outstate;
	mlen = msgdsize(mp);
	mmax = inet->p.info.TIDU_size;
	if (mmax < inet->p.info.TSDU_size && inet->p.info.TSDU_size != T_INVALID)
		mmax = inet->p.info.ETSDU_size;
	if (mlen > mmax)
		goto emsgsize;
	flags = (inet->p.prot.type == SOCK_SEQPACKET && !p->MORE_flag) ? MSG_EOR : 0;
	return inet_sock_sendmsg(inet, mp, NULL, flags, NULL);
      emsgsize:
	goto error;
      outstate:
	goto error;
      einval:
	goto error;
      discard:
	return (QR_DONE);
      notsupport:
	goto error;
      error:
	return m_error(q, EPROTO);
}

/* 
 *  T_EXDATA_REQ         4 - Expedited data request
 *  -------------------------------------------------------------------
 */
STATIC int
t_exdata_req(queue_t *q, mblk_t *mp)
{
	int flags, mlen, mmax;
	inet_t *inet = PRIV(q);
	const struct T_exdata_req *p = (typeof(p)) mp->b_rptr;
	if (inet->p.info.SERV_type == T_CLTS)
		goto notsupport;
	if (inet_get_state(inet) == TS_IDLE)
		goto discard;
	if (mp->b_wptr - mp->b_rptr < sizeof(*p))
		goto einval;
	if ((1 << inet_get_state(inet)) & ~TSM_OUTDATA)
		goto outstate;
	mlen = msgdsize(mp);
	mmax = inet->p.info.TIDU_size;
	if (mmax < inet->p.info.ETSDU_size && inet->p.info.ETSDU_size != T_INVALID)
		mmax = inet->p.info.ETSDU_size;
	if (mlen > mmax)
		goto emsgsize;
	flags = (inet->p.prot.type == SOCK_SEQPACKET && !p->MORE_flag) ? MSG_EOR : 0;
	return inet_sock_sendmsg(inet, mp, NULL, MSG_OOB | flags, NULL);
      emsgsize:
	goto error;
      outstate:
	goto error;
      einval:
	goto error;
      discard:
	return (QR_DONE);
      notsupport:
	goto error;
      error:
	return m_error(q, EPROTO);
}

/* 
 *  T_INFO_REQ           5 - Information Request
 *  -------------------------------------------------------------------
 */
STATIC int
t_info_req(queue_t *q, mblk_t *mp)
{
	inet_t *inet = PRIV(q);
	(void) mp;
	(void) inet;
	return t_info_ack(q);
}

/* 
 *  T_BIND_REQ           6 - Bind a TS user to a transport address
 *  -------------------------------------------------------------------
 */
STATIC int
t_bind_req(queue_t *q, mblk_t *mp)
{
	inet_t *inet = PRIV(q);
	int err;
	const struct T_bind_req *p = (typeof(p)) mp->b_rptr;
	if (inet_get_state(inet) != TS_UNBND)
		goto outstate;
	inet_set_state(inet, TS_WACK_BREQ);
	if (mp->b_wptr - mp->b_rptr < sizeof(*p))
		goto einval;
	if (inet->p.info.SERV_type == T_CLTS && p->CONIND_number)
		goto notsupport;
	if ((mp->b_wptr - mp->b_rptr < p->ADDR_offset + p->ADDR_length)
	    || (p->ADDR_length != sizeof(inet_addr_t)))
		goto badaddr;
	{
		inet_addr_t *add = (typeof(add)) (mp->b_rptr + p->ADDR_offset);
		/* 
		 * we don't allow wildcards yet 
		 */
		if (!add->sin_port)
			goto noaddr;
		if (add->sin_family != inet->p.prot.family)
			goto badaddr;
		if (inet->p.prot.type == SOCK_RAW && inet->cred.cr_uid != 0)
			goto acces;
		inet->port = htons(add->sin_port);
		if ((err = inet_socket(inet)))
			goto error;
		if ((err = inet_bind(inet, add)))
			goto error_close;
		inet->conind = 0;
		if (p->CONIND_number
		    && (inet->p.prot.type == SOCK_STREAM || inet->p.prot.type == SOCK_SEQPACKET))
			if ((err = inet_listen(inet, p->CONIND_number)))
				goto error_close;
		return t_bind_ack(q, &inet->src, p->CONIND_number);
	}
      acces:
	err = TACCES;
	goto error;
      noaddr:
	err = TNOADDR;
	goto error;
      badaddr:
	err = TBADADDR;
	goto error;
      einval:
	err = -EINVAL;
	goto error;
      outstate:
	err = TOUTSTATE;
	goto error;
      notsupport:
	err = TNOTSUPPORT;
	goto error;
      error_close:
	inet_socket_put(xchg(&inet->sock, NULL));
      error:
	return t_error_ack(q, T_BIND_REQ, err);
}

/* 
 *  T_UNBIND_REQ         7 - Unbind TS user from transport address
 *  -------------------------------------------------------------------
 */
STATIC int
t_unbind_req(queue_t *q, mblk_t *mp)
{
	inet_t *inet = PRIV(q);
	if (inet_get_state(inet) != TS_IDLE)
		goto outstate;
	inet_set_state(inet, TS_WACK_UREQ);
	return t_ok_ack(q, T_UNBIND_REQ, NULL, NULL);
      outstate:
	return t_error_ack(q, T_UNBIND_REQ, TOUTSTATE);
}

/* 
 *  T_UNITDATA_REQ       8 -Unitdata Request 
 *  -------------------------------------------------------------------
 */
STATIC int
t_unitdata_req(queue_t *q, mblk_t *mp)
{
	inet_t *inet = PRIV(q);
	size_t mlen = mp->b_wptr - mp->b_rptr;
	size_t dlen = mp->b_cont ? msgdsize(mp->b_cont) : 0;
	const struct T_unitdata_req *p = (typeof(p)) mp->b_rptr;
	if (inet->p.info.SERV_type != T_CLTS)
		goto notsupport;
	if (dlen == 0 && !(inet->p.info.PROVIDER_flag & T_SNDZERO))
		goto baddata;
	if (dlen > inet->p.info.TSDU_size || dlen > inet->p.info.TIDU_size)
		goto baddata;
	if (inet_get_state(inet) != TS_IDLE)
		goto outstate;
	if (mlen < sizeof(*p))
		goto einval;
	if ((mlen < p->DEST_offset + p->DEST_length) || (p->DEST_length != sizeof(inet_addr_t)))
		goto badadd;
	if (inet->p.prot.type == SOCK_RAW && inet->cred.cr_uid != 0)
		goto acces;
	else {
		size_t opt_len = p->OPT_length;
		size_t opt_off = p->OPT_offset;
		unsigned char *opt = mp->b_rptr + opt_off;
		struct inet_opts opts = { 0L, NULL, };
		if (opt_len
		    && (mlen < opt_off + opt_len || inet_parse_opts(inet, &opts, opt, opt_len)))
			goto badopt;
		else {
			int flags = (opts.norte && *(opts.norte)) ? MSG_DONTROUTE : 0;
			inet_addr_t *d = (typeof(d)) (mp->b_rptr + p->DEST_offset);
			return inet_sock_sendmsg(inet, mp, d, flags, &opts);
		}
	}
      badopt:
	goto error;
      acces:
	goto error;
      badadd:
	goto error;
      einval:
	goto error;
      outstate:
	goto error;
      baddata:
	goto error;
      notsupport:
	goto error;
      error:
	return m_error(q, EPROTO);
}

/* 
 *  T_OPTMGMT_REQ        9 - Options management request
 *  -------------------------------------------------------------------
 */
STATIC int
t_optmgmt_req(queue_t *q, mblk_t *mp)
{
	inet_t *inet = PRIV(q);
	int err = 0;
	const size_t mlen = mp->b_wptr - mp->b_rptr;
	const struct T_optmgmt_req *p = (typeof(p)) mp->b_rptr;
	struct inet_opts opts = { 0L, NULL, };
	size_t opt_len, opt_off;
	unsigned char *opt;
#ifdef TS_WACK_OPTREQ
	if (inet_get_state(inet) == TS_IDLE)
		inet_set_state(inet, TS_WACK_OPTREQ);
#endif
	if (mlen < sizeof(*p))
		goto einval;
	opt_len = p->OPT_length;
	opt_off = p->OPT_offset;
	opt = mp->b_rptr + opt_off;
	if (opt_len
	    && (mlen < opt_off + opt_len || (err = inet_parse_opts(inet, &opts, opt, opt_len))))
		goto badopt;
	switch (p->MGMT_flags) {
	case T_CHECK:
		err = inet_opt_check(inet, &opts);
		break;
	case T_NEGOTIATE:
		if (!opts.flags)
			inet_opt_default(inet, &opts);
		else if ((err = inet_opt_check(inet, &opts)))
			break;
		err = inet_opt_negotiate(inet, &opts);
		break;
	case T_DEFAULT:
		err = inet_opt_default(inet, &opts);
		break;
	case T_CURRENT:
		err = inet_opt_current(inet, &opts);
		break;
	default:
		goto badflag;
	}
	if (err)
		goto provspec;
	return t_optmgmt_ack(q, p->MGMT_flags, &opts);
      provspec:
	err = err;
	goto error;
      badflag:
	err = TBADFLAG;
	goto error;
      badopt:
	err = TBADOPT;
	goto error;
      einval:
	err = -EINVAL;
	goto error;
      error:
	return t_error_ack(q, T_OPTMGMT_REQ, err);
}

/* 
 *  T_ORDREL_REQ        10 - TS user is finished sending
 *  -------------------------------------------------------------------
 */
STATIC int
t_ordrel_req(queue_t *q, mblk_t *mp)
{
	inet_t *inet = PRIV(q);
	const struct T_ordrel_req *p = (typeof(p)) mp->b_rptr;
	if (inet->p.info.SERV_type != T_COTS_ORD)
		goto notsupport;
	if ((1 << inet_get_state(inet)) & ~TSM_OUTDATA)
		goto outstate;
	inet->sock->ops->shutdown(inet->sock, SEND_SHUTDOWN);
	switch (inet_get_state(inet)) {
	case TS_DATA_XFER:
		inet_set_state(inet, TS_WIND_ORDREL);
		break;
	case TS_WREQ_ORDREL:
		inet_set_state(inet, TS_IDLE);
		break;
	}
	return (QR_DONE);
      outstate:
	goto error;
      notsupport:
	goto error;
      error:
	return m_error(q, EPROTO);
}

/* 
 *  T_OPTDATA_REQ       24 - Data with options request
 *  -------------------------------------------------------------------
 */
STATIC int
t_optdata_req(queue_t *q, mblk_t *mp)
{
	int err;
	inet_t *inet = PRIV(q);
	const size_t mlen = mp->b_wptr - mp->b_rptr;
	const struct T_optdata_req *p = (typeof(p)) mp->b_rptr;
	if (inet->p.info.SERV_type == T_CLTS)
		goto notsupport;
	if (inet_get_state(inet) == TS_IDLE)
		goto discard;
	if (mlen < sizeof(*p))
		goto einval;
	if ((1 << inet_get_state(inet)) & ~TSM_OUTDATA)
		goto outstate;
	else {
		struct inet_opts opts = { 0L, NULL, };
		size_t opt_len = p->OPT_length;
		size_t opt_off = p->OPT_offset;
		unsigned char *opt = mp->b_rptr + opt_off;
		if (opt_len
		    && (mlen < opt_off + opt_len || inet_parse_opts(inet, &opts, opt, opt_len)))
			goto badopt;
		else {
			int flags = 0;
			if (opts.norte && *(opts.norte))
				flags |= MSG_DONTROUTE;
			if ((p->DATA_flag & T_ODF_EX))
				flags |= MSG_OOB;
			if (!(p->DATA_flag & T_ODF_MORE) && inet->p.prot.type == SOCK_SEQPACKET)
				flags |= MSG_EOR;
			return inet_sock_sendmsg(inet, mp, NULL, flags, &opts);
		}
	}
      badopt:
	err = TBADOPT;
	goto error;
      outstate:
	err = TOUTSTATE;
	goto error;
      einval:
	return m_error(q, EPROTO);
      discard:
	return (QR_DONE);
      notsupport:
	err = TNOTSUPPORT;
	goto error;
      error:
	return t_error_ack(q, T_OPTDATA_REQ, err);
}

#ifdef T_ADDR_REQ
/* 
 *  T_ADDR_REQ          25 - Address Request
 *  -------------------------------------------------------------------
 */
STATIC int
t_addr_req(queue_t *q, mblk_t *mp)
{
	inet_t *inet = PRIV(q);
	(void) mp;
	switch (inet_get_state(inet)) {
	case TS_UNBND:
		return t_addr_ack(q, NULL, NULL);
	case TS_IDLE:
		return t_addr_ack(q, &inet->src, NULL);
	case TS_WCON_CREQ:
	case TS_DATA_XFER:
	case TS_WIND_ORDREL:
	case TS_WREQ_ORDREL:
		return t_addr_ack(q, &inet->src, &inet->dst);
	case TS_WRES_CIND:
		return t_addr_ack(q, NULL, &inet->dst);
	}
	return t_error_ack(q, T_ADDR_REQ, TOUTSTATE);
}
#endif
#ifdef T_CAPABILITY_REQ
/* 
 *  T_CAPABILITY_REQ    ?? - Capability Request
 *  -------------------------------------------------------------------
 */
#endif
/* 
 *  Other primitives    XX - other invalid primitives
 *  -------------------------------------------------------------------------
 */
STATIC int
t_other_req(queue_t *q, mblk_t *mp)
{
	ulong prim = *((ulong *) mp->b_rptr);
	return t_error_ack(q, prim, TNOTSUPPORT);
}

/* 
 *  =========================================================================
 *
 *  IO Controls
 *
 *  =========================================================================
 */
/* 
 *  -------------------------------------------------------------------------
 *
 *  SOCKSYS IO Controls
 *
 *  -------------------------------------------------------------------------
 */

#define INET_IOCS_NONE	0
#define INET_IOCS_1	1
#define INET_IOCS_2	2
#define INET_IOCS_3	3
#define INET_IOCS_4	4
#define INET_IOCS_5	5
#define INET_IOCS_6	6
#define INET_IOCS_7	7
#define INET_IOCS_8	8

/* 
 *  SO_ACCEPT
 *  -----------------------------------
 *  int fd = accept(int fd, struct sockaddr *addr, socklen_t *addrlen)
 */
STATIC int
so_accept(queue_t *q, mblk_t *mp, int *rvp)
{
	inet_t *inet = PRIV(q);
	int err;
	struct copyresp *resp = (typeof(resp)) mp->b_cont;
	struct iocblk *iocp = (typeof(iocp)) mp->b_cont;
	switch (inet->ioc_state) {
	case INET_IOCS_1:
	case INET_IOCS_2:
	case INET_IOCS_3:
	case INET_IOCS_4:
	case INET_IOCS_5:
	case INET_IOCS_6:
	case INET_IOCS_7:
	case INET_IOCS_8:
	}
	*rvp = -1;
	return (-EINVAL);
}

/* 
 *  SO_BIND
 *  -----------------------------------
 */
STATIC int
so_bind(queue_t *q, mblk_t *mp, int *rvp)
{
	inet_t *inet = PRIV(q);
	int err;
	struct copyresp *resp = (typeof(resp)) mp->b_cont;
	struct iocblk *iocp = (typeof(iocp)) mp->b_cont;
	switch (inet->ioc_state) {
	case INET_IOCS_1:
	case INET_IOCS_2:
	case INET_IOCS_3:
	case INET_IOCS_4:
	case INET_IOCS_5:
	case INET_IOCS_6:
	case INET_IOCS_7:
	case INET_IOCS_8:
	}
	*rvp = -1;
	return (-EINVAL);
}

/* 
 *  SO_CONNECT
 *  -----------------------------------
 */
STATIC int
so_connect(queue_t *q, mblk_t *mp, int *rvp)
{
	inet_t *inet = PRIV(q);
	int err;
	struct copyresp *resp = (typeof(resp)) mp->b_cont;
	struct iocblk *iocp = (typeof(iocp)) mp->b_cont;
	switch (inet->ioc_state) {
	case INET_IOCS_1:
	case INET_IOCS_2:
	case INET_IOCS_3:
	case INET_IOCS_4:
	case INET_IOCS_5:
	case INET_IOCS_6:
	case INET_IOCS_7:
	case INET_IOCS_8:
	}
	*rvp = -1;
	return (-EINVAL);
}

/* 
 *  SO_GETPEERNAME
 *  -----------------------------------
 */
STATIC int
so_getpeername(queue_t *q, mblk_t *mp, int *rvp)
{
	inet_t *inet = PRIV(q);
	int err;
	struct copyresp *resp = (typeof(resp)) mp->b_cont;
	struct iocblk *iocp = (typeof(iocp)) mp->b_cont;
	switch (inet->ioc_state) {
	case INET_IOCS_1:
	case INET_IOCS_2:
	case INET_IOCS_3:
	case INET_IOCS_4:
	case INET_IOCS_5:
	case INET_IOCS_6:
	case INET_IOCS_7:
	case INET_IOCS_8:
	}
	*rvp = -1;
	return (-EINVAL);
}

/* 
 *  SO_GETSOCKNAME
 *  -----------------------------------
 */
STATIC int
so_getsockname(queue_t *q, mblk_t *mp, int *rvp)
{
	inet_t *inet = PRIV(q);
	int err;
	struct copyresp *resp = (typeof(resp)) mp->b_cont;
	struct iocblk *iocp = (typeof(iocp)) mp->b_cont;
	switch (inet->ioc_state) {
	case INET_IOCS_1:
	case INET_IOCS_2:
	case INET_IOCS_3:
	case INET_IOCS_4:
	case INET_IOCS_5:
	case INET_IOCS_6:
	case INET_IOCS_7:
	case INET_IOCS_8:
	}
	*rvp = -1;
	return (-EINVAL);
}

/* 
 *  SO_GETSOCKOPT
 *  -----------------------------------
 */
STATIC int
so_getsockopt(queue_t *q, mblk_t *mp, int *rvp)
{
	inet_t *inet = PRIV(q);
	int err;
	struct copyresp *resp = (typeof(resp)) mp->b_cont;
	struct iocblk *iocp = (typeof(iocp)) mp->b_cont;
	switch (inet->ioc_state) {
	case INET_IOCS_1:
	case INET_IOCS_2:
	case INET_IOCS_3:
	case INET_IOCS_4:
	case INET_IOCS_5:
	case INET_IOCS_6:
	case INET_IOCS_7:
	case INET_IOCS_8:
	}
	*rvp = -1;
	return (-EINVAL);
}

/* 
 *  SO_LISTEN
 *  -----------------------------------
 */
STATIC int
so_listen(queue_t *q, mblk_t *mp, int *rvp)
{
	inet_t *inet = PRIV(q);
	int err;
	struct copyresp *resp = (typeof(resp)) mp->b_cont;
	struct iocblk *iocp = (typeof(iocp)) mp->b_cont;
	switch (inet->ioc_state) {
	case INET_IOCS_1:
	case INET_IOCS_2:
	case INET_IOCS_3:
	case INET_IOCS_4:
	case INET_IOCS_5:
	case INET_IOCS_6:
	case INET_IOCS_7:
	case INET_IOCS_8:
	}
	*rvp = -1;
	return (-EINVAL);
}

/* 
 *  SO_RECV
 *  -----------------------------------
 */
STATIC int
so_recv(queue_t *q, mblk_t *mp, int *rvp)
{
	inet_t *inet = PRIV(q);
	int err;
	struct copyresp *resp = (typeof(resp)) mp->b_cont;
	struct iocblk *iocp = (typeof(iocp)) mp->b_cont;
	switch (inet->ioc_state) {
	case INET_IOCS_1:
	case INET_IOCS_2:
	case INET_IOCS_3:
	case INET_IOCS_4:
	case INET_IOCS_5:
	case INET_IOCS_6:
	case INET_IOCS_7:
	case INET_IOCS_8:
	}
	*rvp = -1;
	return (-EINVAL);
}

/* 
 *  SO_RECVFROM
 *  -----------------------------------
 */
STATIC int
so_recvfrom(queue_t *q, mblk_t *mp, int *rvp)
{
	inet_t *inet = PRIV(q);
	int err;
	struct copyresp *resp = (typeof(resp)) mp->b_cont;
	struct iocblk *iocp = (typeof(iocp)) mp->b_cont;
	switch (inet->ioc_state) {
	case INET_IOCS_1:
	case INET_IOCS_2:
	case INET_IOCS_3:
	case INET_IOCS_4:
	case INET_IOCS_5:
	case INET_IOCS_6:
	case INET_IOCS_7:
	case INET_IOCS_8:
	}
	*rvp = -1;
	return (-EINVAL);
}

/* 
 *  SO_SEND
 *  -----------------------------------
 */
STATIC int
so_send(queue_t *q, mblk_t *mp, int *rvp)
{
	inet_t *inet = PRIV(q);
	int err;
	struct copyresp *resp = (typeof(resp)) mp->b_cont;
	struct iocblk *iocp = (typeof(iocp)) mp->b_cont;
	switch (inet->ioc_state) {
	case INET_IOCS_1:
	case INET_IOCS_2:
	case INET_IOCS_3:
	case INET_IOCS_4:
	case INET_IOCS_5:
	case INET_IOCS_6:
	case INET_IOCS_7:
	case INET_IOCS_8:
	}
	*rvp = -1;
	return (-EINVAL);
}

/* 
 *  SO_SENDTO
 *  -----------------------------------
 */
STATIC int
so_sendto(queue_t *q, mblk_t *mp, int *rvp)
{
	inet_t *inet = PRIV(q);
	int err;
	struct copyresp *resp = (typeof(resp)) mp->b_cont;
	struct iocblk *iocp = (typeof(iocp)) mp->b_cont;
	switch (inet->ioc_state) {
	case INET_IOCS_1:
	case INET_IOCS_2:
	case INET_IOCS_3:
	case INET_IOCS_4:
	case INET_IOCS_5:
	case INET_IOCS_6:
	case INET_IOCS_7:
	case INET_IOCS_8:
	}
	*rvp = -1;
	return (-EINVAL);
}

/* 
 *  SO_SETSOCKOPT
 *  -----------------------------------
 */
STATIC int
so_setsockopt(queue_t *q, mblk_t *mp, int *rvp)
{
	inet_t *inet = PRIV(q);
	int err;
	struct copyresp *resp = (typeof(resp)) mp->b_cont;
	struct iocblk *iocp = (typeof(iocp)) mp->b_cont;
	switch (inet->ioc_state) {
	case INET_IOCS_1:
	case INET_IOCS_2:
	case INET_IOCS_3:
	case INET_IOCS_4:
	case INET_IOCS_5:
	case INET_IOCS_6:
	case INET_IOCS_7:
	case INET_IOCS_8:
	}
	*rvp = -1;
	return (-EINVAL);
}

/* 
 *  SO_SHUTDOWN
 *  -----------------------------------
 */
STATIC int
so_shutdown(queue_t *q, mblk_t *mp, int *rvp)
{
	inet_t *inet = PRIV(q);
	int err;
	struct copyresp *resp = (typeof(resp)) mp->b_cont;
	struct iocblk *iocp = (typeof(iocp)) mp->b_cont;
	switch (inet->ioc_state) {
	case INET_IOCS_1:
	case INET_IOCS_2:
	case INET_IOCS_3:
	case INET_IOCS_4:
	case INET_IOCS_5:
	case INET_IOCS_6:
	case INET_IOCS_7:
	case INET_IOCS_8:
	}
	*rvp = -1;
	return (-EINVAL);
}

/* 
 *  SO_SOCKET
 *  -----------------------------------
 */
STATIC int
so_socket(queue_t *q, mblk_t *mp, int *rvp)
{
	inet_t *inet = PRIV(q);
	int err;
	struct copyresp *resp = (typeof(resp)) mp->b_cont;
	struct iocblk *iocp = (typeof(iocp)) mp->b_cont;
	switch (inet->ioc_state) {
	case INET_IOCS_1:
	case INET_IOCS_2:
	case INET_IOCS_3:
	case INET_IOCS_4:
	case INET_IOCS_5:
	case INET_IOCS_6:
	case INET_IOCS_7:
	case INET_IOCS_8:
	}
	*rvp = -1;
	return (-EINVAL);
}

/* 
 *  SO_SELECT
 *  -----------------------------------
 */
STATIC int
so_select(queue_t *q, mblk_t *mp, int *rvp)
{
	inet_t *inet = PRIV(q);
	int err;
	struct copyresp *resp = (typeof(resp)) mp->b_cont;
	struct iocblk *iocp = (typeof(iocp)) mp->b_cont;
	switch (inet->ioc_state) {
	case INET_IOCS_1:
	case INET_IOCS_2:
	case INET_IOCS_3:
	case INET_IOCS_4:
	case INET_IOCS_5:
	case INET_IOCS_6:
	case INET_IOCS_7:
	case INET_IOCS_8:
	}
	*rvp = -1;
	return (-EINVAL);
}

/* 
 *  SO_GETIPDOMAIN
 *  -----------------------------------
 */
STATIC int
so_getipdomain(queue_t *q, mblk_t *mp, int *rvp)
{
	inet_t *inet = PRIV(q);
	int err;
	struct copyresp *resp = (typeof(resp)) mp->b_cont;
	struct iocblk *iocp = (typeof(iocp)) mp->b_cont;
	switch (inet->ioc_state) {
	case INET_IOCS_1:
	case INET_IOCS_2:
	case INET_IOCS_3:
	case INET_IOCS_4:
	case INET_IOCS_5:
	case INET_IOCS_6:
	case INET_IOCS_7:
	case INET_IOCS_8:
	}
	*rvp = -1;
	return (-EINVAL);
}

/* 
 *  SO_SETIPDOMAIN
 *  -----------------------------------
 */
STATIC int
so_setipdomain(queue_t *q, mblk_t *mp, int *rvp)
{
	inet_t *inet = PRIV(q);
	int err;
	struct copyresp *resp = (typeof(resp)) mp->b_cont;
	struct iocblk *iocp = (typeof(iocp)) mp->b_cont;
	switch (inet->ioc_state) {
	case INET_IOCS_1:
	case INET_IOCS_2:
	case INET_IOCS_3:
	case INET_IOCS_4:
	case INET_IOCS_5:
	case INET_IOCS_6:
	case INET_IOCS_7:
	case INET_IOCS_8:
	}
	*rvp = -1;
	return (-EINVAL);
}

/* 
 *  SO_ADJTIME
 *  -----------------------------------
 */
STATIC int
so_adjtime(queue_t *q, mblk_t *mp, int *rvp)
{
	inet_t *inet = PRIV(q);
	int err;
	struct copyresp *resp = (typeof(resp)) mp->b_cont;
	struct iocblk *iocp = (typeof(iocp)) mp->b_cont;
	switch (inet->ioc_state) {
	case INET_IOCS_1:
	case INET_IOCS_2:
	case INET_IOCS_3:
	case INET_IOCS_4:
	case INET_IOCS_5:
	case INET_IOCS_6:
	case INET_IOCS_7:
	case INET_IOCS_8:
	}
	*rvp = -1;
	return (-EINVAL);
}

/* 
 *  SO_SETREUID
 *  -----------------------------------
 */
STATIC int
so_setreuid(queue_t *q, mblk_t *mp, int *rvp)
{
	inet_t *inet = PRIV(q);
	int err;
	struct copyresp *resp = (typeof(resp)) mp->b_cont;
	struct iocblk *iocp = (typeof(iocp)) mp->b_cont;
	switch (inet->ioc_state) {
	case INET_IOCS_1:
	case INET_IOCS_2:
	case INET_IOCS_3:
	case INET_IOCS_4:
	case INET_IOCS_5:
	case INET_IOCS_6:
	case INET_IOCS_7:
	case INET_IOCS_8:
	}
	*rvp = -1;
	return (-EINVAL);
}

/* 
 *  SO_GETREGID
 *  -----------------------------------
 */
STATIC int
so_setregid(queue_t *q, mblk_t *mp, int *rvp)
{
	inet_t *inet = PRIV(q);
	int err;
	struct copyresp *resp = (typeof(resp)) mp->b_cont;
	struct iocblk *iocp = (typeof(iocp)) mp->b_cont;
	switch (inet->ioc_state) {
	case INET_IOCS_1:
	case INET_IOCS_2:
	case INET_IOCS_3:
	case INET_IOCS_4:
	case INET_IOCS_5:
	case INET_IOCS_6:
	case INET_IOCS_7:
	case INET_IOCS_8:
	}
	*rvp = -1;
	return (-EINVAL);
}

/* 
 *  SO_GETTIME
 *  -----------------------------------
 */
STATIC int
so_gettime(queue_t *q, mblk_t *mp, int *rvp)
{
	inet_t *inet = PRIV(q);
	int err;
	struct copyresp *resp = (typeof(resp)) mp->b_cont;
	struct iocblk *iocp = (typeof(iocp)) mp->b_cont;
	switch (inet->ioc_state) {
	case INET_IOCS_1:
	case INET_IOCS_2:
	case INET_IOCS_3:
	case INET_IOCS_4:
	case INET_IOCS_5:
	case INET_IOCS_6:
	case INET_IOCS_7:
	case INET_IOCS_8:
	}
	*rvp = -1;
	return (-EINVAL);
}

/* 
 *  SO_SETTIME
 *  -----------------------------------
 */
STATIC int
so_settime(queue_t *q, mblk_t *mp, int *rvp)
{
	inet_t *inet = PRIV(q);
	int err;
	struct copyresp *resp = (typeof(resp)) mp->b_cont;
	struct iocblk *iocp = (typeof(iocp)) mp->b_cont;
	switch (inet->ioc_state) {
	case INET_IOCS_1:
	case INET_IOCS_2:
	case INET_IOCS_3:
	case INET_IOCS_4:
	case INET_IOCS_5:
	case INET_IOCS_6:
	case INET_IOCS_7:
	case INET_IOCS_8:
	}
	*rvp = -1;
	return (-EINVAL);
}

/* 
 *  SO_GETITIMER
 *  -----------------------------------
 */
STATIC int
so_getitimer(queue_t *q, mblk_t *mp, int *rvp)
{
	inet_t *inet = PRIV(q);
	int err;
	struct copyresp *resp = (typeof(resp)) mp->b_cont;
	struct iocblk *iocp = (typeof(iocp)) mp->b_cont;
	switch (inet->ioc_state) {
	case INET_IOCS_1:
	case INET_IOCS_2:
	case INET_IOCS_3:
	case INET_IOCS_4:
	case INET_IOCS_5:
	case INET_IOCS_6:
	case INET_IOCS_7:
	case INET_IOCS_8:
	}
	*rvp = -1;
	return (-EINVAL);
}

/* 
 *  SO_SETITIMER
 *  -----------------------------------
 */
STATIC int
so_setitimer(queue_t *q, mblk_t *mp, int *rvp)
{
	inet_t *inet = PRIV(q);
	int err;
	struct copyresp *resp = (typeof(resp)) mp->b_cont;
	struct iocblk *iocp = (typeof(iocp)) mp->b_cont;
	switch (inet->ioc_state) {
	case INET_IOCS_1:
	case INET_IOCS_2:
	case INET_IOCS_3:
	case INET_IOCS_4:
	case INET_IOCS_5:
	case INET_IOCS_6:
	case INET_IOCS_7:
	case INET_IOCS_8:
	}
	*rvp = -1;
	return (-EINVAL);
}

/* 
 *  SO_RECVMSG
 *  -----------------------------------
 */
STATIC int
so_recvmsg(queue_t *q, mblk_t *mp, int *rvp)
{
	inet_t *inet = PRIV(q);
	int err;
	struct copyresp *resp = (typeof(resp)) mp->b_cont;
	struct iocblk *iocp = (typeof(iocp)) mp->b_cont;
	switch (inet->ioc_state) {
	case INET_IOCS_1:
	case INET_IOCS_2:
	case INET_IOCS_3:
	case INET_IOCS_4:
	case INET_IOCS_5:
	case INET_IOCS_6:
	case INET_IOCS_7:
	case INET_IOCS_8:
	}
	*rvp = -1;
	return (-EINVAL);
}

/* 
 *  SO_SENDMSG
 *  -----------------------------------
 */
STATIC int
so_sendmsg(queue_t *q, mblk_t *mp, int *rvp)
{
	inet_t *inet = PRIV(q);
	int err;
	struct copyresp *resp = (typeof(resp)) mp->b_cont;
	struct iocblk *iocp = (typeof(iocp)) mp->b_cont;
	switch (inet->ioc_state) {
	case INET_IOCS_1:
	case INET_IOCS_2:
	case INET_IOCS_3:
	case INET_IOCS_4:
	case INET_IOCS_5:
	case INET_IOCS_6:
	case INET_IOCS_7:
	case INET_IOCS_8:
	}
	*rvp = -1;
	return (-EINVAL);
}

/* 
 *  SO_SOCKPAIR
 *  -----------------------------------
 */
STATIC int
so_sockpair(queue_t *q, mblk_t *mp, int *rvp)
{
	inet_t *inet = PRIV(q);
	int err;
	struct copyresp *resp = (typeof(resp)) mp->b_cont;
	struct iocblk *iocp = (typeof(iocp)) mp->b_cont;
	switch (inet->ioc_state) {
	case INET_IOCS_1:
	case INET_IOCS_2:
	case INET_IOCS_3:
	case INET_IOCS_4:
	case INET_IOCS_5:
	case INET_IOCS_6:
	case INET_IOCS_7:
	case INET_IOCS_8:
	}
	*rvp = -1;
	return (-EINVAL);
}

STATIC int
siocsocksys(queue_t *q, mblk_t *mp, int *rvp)
{
	inet_t *inet = PRIV(q);
	switch (inet->ioc_call) {
	case SO_ACCEPT:
		return so_accept(q, mp, rvp);
	case SO_BIND:
		return so_bind(q, mp, rvp);
	case SO_CONNECT:
		return so_connect(q, mp, rvp);
	case SO_GETPEERNAME:
		return so_getpeername(q, mp, rvp);
	case SO_GETSOCKNAME:
		return so_getsockname(q, mp, rvp);
	case SO_GETSOCKOPT:
		return so_getsockopt(q, mp, rvp);
	case SO_LISTEN:
		return so_listen(q, mp, rvp);
	case SO_RECV:
		return so_recv(q, mp, rvp);
	case SO_RECVFROM:
		return so_recvfrom(q, mp, rvp);
	case SO_SEND:
		return so_send(q, mp, rvp);
	case SO_SENDTO:
		return so_sendto(q, mp, rvp);
	case SO_SETSOCKOPT:
		return so_setsockopt(q, mp, rvp);
	case SO_SHUTDOWN:
		return so_shutdown(q, mp, rvp);
	case SO_SOCKET:
		return so_socket(q, mp, rvp);
	case SO_SELECT:
		return so_select(q, mp, rvp);
	case SO_GETIPDOMAIN:
		return so_getipdomain(q, mp, rvp);
	case SO_SETIPDOMAIN:
		return so_setipdomain(q, mp, rvp);
	case SO_ADJTIME:
		return so_adjtime(q, mp, rvp);
	case SO_SETREUID:
		return so_setreuid(q, mp, rvp);
	case SO_GETREGID:
		return so_setregid(q, mp, rvp);
	case SO_GETTIME:
		return so_gettime(q, mp, rvp);
	case SO_SETTIME:
		return so_settime(q, mp, rvp);
	case SO_GETITIMER:
		return so_getitimer(q, mp, rvp);
	case SO_SETITIMER:
		return so_setitimer(q, mp, rvp);
	case SO_RECVMSG:
		return so_recvmsg(q, mp, rvp);
	case SO_SENDMSG:
		return so_sendmsg(q, mp, rvp);
	case SO_SOCKPAIR:
		return so_sockpair(q, mp, rvp);
	}
	*rvp = -1;
	return (-EINVAL);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  SOCKMOD IO Controls
 *
 *  -------------------------------------------------------------------------
 */
/* 
 *  -------------------------------------------------------------------------
 *
 *  TIMOD IO Controls
 *
 *  -------------------------------------------------------------------------
 */
#if 0
STATIC int
ti_getinfo(queue_t *q, mblk_t *mp)
{
}
STATIC int
ti_optmgmt(queue_t *q, mblk_t *mp)
{
}
STATIC int
ti_bind(queue_t *q, mblk_t *mp)
{
}
STATIC int
ti_unbind(queue_t *q, mblk_t *mp)
{
}
STATIC int
ti_getmyname(queue_t *q, mblk_t *mp)
{
}
STATIC int
ti_getpeername(queue_t *q, mblk_t *mp)
{
}
STATIC int
ti_setmyname(queue_t *q, mblk_t *mp)
{
}
STATIC int
ti_setpeername(queue_t *q, mblk_t *mp)
{
}
#endif

/* 
 *  =========================================================================
 *
 *  STREAMS Message Handling
 *
 *  =========================================================================
 */
/* 
 *  -------------------------------------------------------------------------
 *
 *  M_IOCDATA Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
inet_w_iocdata(queue_t *q, mblk_t *mp)
{
	inet_t *inet = PRIV(q);
	struct copyresp *resp = (typeof(resp)) mp->b_rptr;
	struct iocblk *iocp = (typeof(iocp)) mp->b_rptr;
	mblk_t *dp = mp->b_cont;
	int err, ret = -1;
	switch (resp->ioc_cmd) {
	case SIOCSOCKSYS:
		err = siocsocksys(q, mp, &ret);
		break;
	default:
		err = -EINVAL;
		ret = -1;
		break;
	}
	if (err > 0)
		return (err);
	else if (err < 0) {
		switch (err) {
		case -EAGAIN:
		case -ENOBUFS:
		case -ENOMEM:
		case -EBUSY:
			return (err);
		}
		mp->b_datap->db_type = M_IOCACK;
		mp->b_wptr = mp->b_rptr + sizeof(*iocp);
		iocp->ioc_error = 0;
		iocp->ioc_rval = ret;
	} else {
		mp->b_datap->db_type = M_IOCNAK;
		mp->b_wptr = mp->b_rptr + sizeof(*iocp);
		iocp->ioc_error = -err;
		iocp->ioc_rval = ret;
	}
	qreply(q, mp);
	return (QR_ABSORBED);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  M_IOCTL Handling
 *
 *  -------------------------------------------------------------------------
 *  There are a standard set of IOCTLs for TLI.  Check the iBCS package for
 *  the numbering.
 */
STATIC int
inet_w_ioctl(queue_t *q, mblk_t *mp)
{
	inet_t *inet = PRIV(q);
	struct iocblk *iocp = (typeof(iocp)) mp->b_rptr;
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	int cmd = iocp->ioc_cmd, count = iocp->ioc_count;
	int type = _IOC_TYPE(cmd), nr = _IOC_NR(cmd), size = _IOC_SIZE(cmd);
	int err, ret = -1;
	switch (type) {
	case 'I':
	{
		switch (nr) {
		case _IOC_NR(SIOCSOCKSYS):
			err = siocsocksys(q, mp, &ret);
			break;
		case _IOC_NR(SIOCGHIWAT):
			err = siocghiwat(q, mp, &ret);
			break;
		case _IOC_NR(SIOCSHIWAT):
			err = siocshiwat(q, mp, &ret);
			break;
		case _IOC_NR(SIOCGLOWAT):
			err = siocglowat(q, mp, &ret);
			break;
		case _IOC_NR(SIOCSLOWAT):
			err = siocslowat(q, mp, &ret);
			break;
		case _IOC_NR(SIOCATMARK):
		case _IOC_NR(SIOCSPGRP):
		case _IOC_NR(SIOCGPGRP):
		case _IOC_NR(FIONREAD):
		case _IOC_NR(SIOCADDRT):
		case _IOC_NR(SIOCDELRT):
		case _IOC_NR(SIOCSIFADDR):
		case _IOC_NR(SIOCGIFADDR):
		case _IOC_NR(SIOCSIFDSTADDR):
		case _IOC_NR(SIOCGIFDSTADDR):
		case _IOC_NR(SIOCSIFFLAGS):
		case _IOC_NR(SIOCGIFFLAGS):
		case _IOC_NR(SIOCGIFCONF):
		case _IOC_NR(SIOCGIFBRDADDR):
		case _IOC_NR(SIOCSIFBRDADDR):
		case _IOC_NR(SIOCGIFNETMASK):
		case _IOC_NR(SIOCSIFNETMASK):
		case _IOC_NR(SIOCGIFMETRIC):
		case _IOC_NR(SIOCSIFMETRIC):
		case _IOC_NR(SIOCSARP):
		case _IOC_NR(SIOCGARP):
		case _IOC_NR(SIOCGENADDR):
		case _IOC_NR(SIOCSIFMTU):
		case _IOC_NR(SIOCGIFMTU):
		case _IOC_NR(SIOCGETNAME):
		case _IOC_NR(SIOCGETPEER):
		case _IOC_NR(SIOC):
		case _IOC_NR(SIOC):
		case _IOC_NR(SIOC):
		case _IOC_NR(SIOC):
		case _IOC_NR(SIOC):
		case _IOC_NR(SIOCIFDETACH):
		case _IOC_NR(SIOCGETPSTATS):
		case _IOC_NR(SIOCSIFNAME):
		case _IOC_NR(SIOCGIFONEP):
		case _IOC_NR(SIOCGIFONEP):
		default:
			err = -EINVAL;
		}
		break;
	}
	case 'p':
	{
	case _IOC_NR(SIOCXPROTO):
		rtn = inet_term_proto();
		break;
	case _IOC_NR(SIOCPROTO):
	case _IOC_NR(SIOCX25XMT):
	case _IOC_NR(SIOCX25RCV):
	case _IOC_NR(SIOCX25TBL):
	}
	}
#if 0
	/* these are supported by timod which can be pushed */
	switch (nr) {
	case _IOC_NR(TI_GETINFO):
		rtn = ti_getinfo(q, mp);
		break;
	case _IOC_NR(TI_OPTMGMT):
		rtn = ti_optmgmt(q, mp);
		break;
	case _IOC_NR(TI_BIND):
		rtn = ti_bind(q, mp);
		break;
	case _IOC_NR(TI_UNBIND):
		rtn = ti_unbind(q, mp);
		break;
	case _IOC_NR(TI_GETMYNAME):
		rtn = ti_getmyname(q, mp);
		break;
	case _IOC_NR(TI_GETPEERNAME):
		rtn = ti_getpeername(q, mp);
		break;
	case _IOC_NR(TI_SETMYNAME):
		rtn = ti_setmyname(q, mp);
		break;
	case _IOC_NR(TI_SETPEERNAME):
		rtn = ti_setpeername(q, mp);
		break;
	default:
		rtn = -EOPNOTSUPP;
		break;
	}
#endif
	return (-EOPNOTSUPP);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  M_PROTO, M_PCPROTO Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
inet_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	ulong prim;
	inet_t *inet = PRIV(q);
	ulong oldstate = inet_get_state(inet);
	switch ((prim = *((ulong *) mp->b_rptr))) {
	case T_CONN_REQ:
		rtn = t_conn_req(q, mp);
		break;
	case T_CONN_RES:
		rtn = t_conn_res(q, mp);
		break;
	case T_DISCON_REQ:
		rtn = t_discon_req(q, mp);
		break;
	case T_DATA_REQ:
		rtn = t_data_req(q, mp);
		break;
	case T_EXDATA_REQ:
		rtn = t_exdata_req(q, mp);
		break;
	case T_INFO_REQ:
		rtn = t_info_req(q, mp);
		break;
	case T_BIND_REQ:
		rtn = t_bind_req(q, mp);
		break;
	case T_UNBIND_REQ:
		rtn = t_unbind_req(q, mp);
		break;
	case T_OPTMGMT_REQ:
		rtn = t_optmgmt_req(q, mp);
		break;
	case T_UNITDATA_REQ:
		rtn = t_unitdata_req(q, mp);
		break;
	case T_ORDREL_REQ:
		rtn = t_ordrel_req(q, mp);
		break;
	case T_OPTDATA_REQ:
		rtn = t_optdata_req(q, mp);
		break;
#ifdef T_ADDR_REQ
	case T_ADDR_REQ:
		rtn = t_addr_req(q, mp);
		break;
#endif
#ifdef T_CAPABILITY_REQ
	case T_CAPABILITY_REQ:
		rtn = t_capability_req(q, mp);
		break;
#endif
	default:
		rtn = t_other_req(q, mp);
		break;
	}
	if (rtn < 0) {
		rare();
		inet_set_state(inet, oldstate);
	}
	return (rtn);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  M_FLUSH Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
inet_w_flush(queue_t *q, mblk_t *mp)
{
	if (*mp->b_rptr & FLUSHW) {
		if (*mp->b_rptr & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHALL);
		else
			flushq(q, FLUSHALL);
		*mp->b_rptr &= ~FLUSHW;
	}
	if (*mp->b_rptr & FLUSHR) {
		if (*mp->b_rptr & FLUSHBAND)
			flushband(OTHER(q), mp->b_rptr[1], FLUSHALL);
		else
			flushq(OTHER(q), FLUSHALL);
		qreply(q, mp);
		return (QR_ABSORBED);
	}
	return (QR_DONE);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  M_PCRSE Handling
 *
 *  -------------------------------------------------------------------------
 *  On the read queue, an M_PCRSE is used to indicate a state_change callback
 *  on the socket.  This can mean that the socket has completed connecting or
 *  has disconnected, or it may signal a connection indication on a listening
 *  socket.
 *
 *  We always examine both the state of the TPI interface and the state of the
 *  socket to determine which actions need to be performed.  This makes the
 *  state machine interworking more robust.
 *
 *  Note that the sk pointer passed in here is not necessarily the same as
 *  inet->sock->sk, sk may be a child (connection indication) of the primary
 *  socket.
 */
STATIC int
inet_r_pcrse(queue_t *q, mblk_t *mp)
{
	inet_t *inet = PRIV(q);
	inet_event_t *p = (typeof(p)) mp->b_rptr;
	int oldstate = xchg(&inet->tcp_state, p->state);
	if (!inet->sock)
		goto discard;
	switch (inet->p.prot.type) {
	case SOCK_STREAM:
	case SOCK_SEQPACKET:
		switch (inet_get_state(inet)) {
		case TS_WCON_CREQ:
			switch (p->state) {
			case TCP_ESTABLISHED:
				return (t_conn_con(q, &inet->dst, NULL, NULL));
			case TCP_TIME_WAIT:
			case TCP_CLOSE:
				return (t_discon_ind(q, &inet->dst, T_PROVIDER, 0, NULL, NULL));
			}
			break;
		case TS_WIND_ORDREL:
			switch (p->state) {
			case TCP_TIME_WAIT:
			case TCP_CLOSE:
				switch (oldstate) {
				case TCP_FIN_WAIT2:
					return (t_ordrel_ind(q));
				case TCP_ESTABLISHED:
				case TCP_FIN_WAIT1:
					return (t_discon_ind(q, NULL, T_PROVIDER, 0, NULL, NULL));
				}
				break;
			case TCP_FIN_WAIT1:
			case TCP_FIN_WAIT2:
				return (QR_DONE);
			}
			break;
		case TS_DATA_XFER:
			switch (p->state) {
			case TCP_CLOSE_WAIT:
				return (t_ordrel_ind(q));
			case TCP_TIME_WAIT:
			case TCP_CLOSE:
				return (t_discon_ind(q, NULL, T_PROVIDER, 0, NULL, NULL));
			}
			break;
		case TS_WREQ_ORDREL:
			switch (p->state) {
			case TCP_TIME_WAIT:
			case TCP_CLOSE:
				switch (oldstate) {
				case TCP_CLOSE_WAIT:
					return (t_discon_ind(q, NULL, T_PROVIDER, 0, NULL, NULL));
				}
				break;
			}
			break;
		case TS_IDLE:
		case TS_WRES_CIND:
			switch (p->state) {
			case TCP_LAST_ACK:
				switch (oldstate) {
				case TCP_CLOSE_WAIT:
					return (QR_DONE);
				}
				break;
			case TCP_TIME_WAIT:
			case TCP_CLOSE:
				switch (oldstate) {
				case TCP_FIN_WAIT2:
				case TCP_LAST_ACK:
					return (QR_DONE);
				case TCP_LISTEN:
				{
					mblk_t *cp;
					/* state change was on child */
					inet->tcp_state = TCP_LISTEN;
					/* look for the child */
					if ((cp = t_seq_find(inet, mp)))
						return (t_discon_ind
							(q, NULL, T_PROVIDER, 0, cp, NULL));
					return (QR_DONE);
				}
				}
				break;
			case TCP_ESTABLISHED:
				switch (oldstate) {
				case TCP_LISTEN:
					/* state change was on child */
					inet->tcp_state = TCP_LISTEN;
					return (inet_conn_ind(q, mp));
				}
				break;
			}
			break;
		}
		return (-EFAULT);
	case SOCK_DGRAM:
	case SOCK_RAW:
	case SOCK_RDM:
		return (-EFAULT);
	default:
		return (-EFAULT);
	}
      discard:
	return (QR_DONE);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  M_READ Handling
 *
 *  -------------------------------------------------------------------------
 *
 *  M_READ on the RD(q)
 *  -------------------------------------------------------------------------
 *
 *  On the read queue, an M_READ is used to indicate a data_ready callback on
 *  the socket.  This can mean that the socket is now readable (when it was
 *  blocked before), or it may mean that a socket has completed connecting, or
 *  disconnected, or that a connection indication may be availabel on a
 *  listening socket.
 *
 *  We always examine both the state of the TPI interface and the state of the
 *  socket to determine which actions need to be performed.  This makes the
 *  state machine interworking more robust.
 *
 *  Note that the sk pointer passed in here is not necessarily the same as
 *  inet->sock->sk, sk may be a child (connection indication) of the primary
 *  socket.
 */
STATIC int
inet_r_read(queue_t *q, mblk_t *mp)
{
	inet_t *inet = PRIV(q);
	inet_event_t *p = (typeof(p)) mp->b_rptr;
	if (!inet->sock)
		goto discard;
	switch (inet->p.prot.type) {
	case SOCK_STREAM:	/* TCP */
	case SOCK_SEQPACKET:	/* SCTP */
		switch (inet_get_state(inet)) {
		case TS_IDLE:
		case TS_WRES_CIND:
			if (p->state == TCP_LISTEN)
				return (QR_DONE);
		case TS_DATA_XFER:
		case TS_WIND_ORDREL:
		case TS_WREQ_ORDREL:	/* TCP bug I believe */
			return inet_sock_recvmsg(q);
		}
		return (-EFAULT);
	case SOCK_DGRAM:
	case SOCK_RAW:
	case SOCK_RDM:
		switch (inet_get_state(inet)) {
		case TS_IDLE:
			return inet_sock_recvmsg(q);
		}
		return (-EFAULT);
	}
	return (-EFAULT);
      discard:
	return (QR_DONE);
}

/* 
 *  M_READ on the WR(q)
 *  -------------------------------------------------------------------------
 *
 *  On the write queue, an M_READ is used to indicate a write_space callback
 *  on the socket.  This can mean that the socket is now writeable (when it
 *  was blocked before).  This can also mean that a connecting socket has
 *  completed connecting (or has disconnected), or that a listening socket has
 *  a connection indication on the socket.
 *
 *  We always examine both the state of the TPI interface and the state of the
 *  socket to determine which actions need to be performed.  This makes the
 *  state machine interworking more robust.
 *
 *  Note that the sk pointer passed in here is not necessarily the same as
 *  inet->sock->sk, sk may be a child (connection indication) of the primary
 *  socket.
 *
 */
STATIC int
inet_w_read(queue_t *q, mblk_t *mp)
{
	inet_t *inet = PRIV(q);
	inet_event_t *p = (typeof(p)) mp->b_rptr;
	if (!inet->sock)
		goto discard;
	switch (inet->p.prot.type) {
	case SOCK_STREAM:
	case SOCK_SEQPACKET:
		switch (inet_get_state(inet)) {
		case TS_DATA_XFER:
		case TS_WREQ_ORDREL:
			return (QR_DONE);
		}
		return (-EFAULT);
	case SOCK_DGRAM:
	case SOCK_RAW:
	case SOCK_RDM:
		switch (inet_get_state(inet)) {
		case TS_IDLE:
			return (QR_DONE);
		}
		return (-EFAULT);
	}
	return (-EFAULT);
      discard:
	return (QR_DONE);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  M_ERROR Handling
 *
 *  -------------------------------------------------------------------------
 *  On the read queue, an M_ERROR is used to indicate an error_report callback
 *  on the socket.  This can mean that the socket has disconected.  We might
 *  not received any other indication of the error.
 *
 *  We always examine both the state of the TPI interface and the state of the
 *  socket to determine which actions need to be performed.  This makes the
 *  state machine interworking more robust.
 *
 *  Note that the sk pointer passed in here is not necessarily the same as
 *  inet->sock->sk, sk may be a child (connection indication) of the primary
 *  socket.
 */
STATIC int
inet_r_error(queue_t *q, mblk_t *mp)
{
	int err;
	inet_t *inet = PRIV(q);
	inet_event_t *p = (typeof(p)) mp->b_rptr;
	err = sock_error(p->sk);
	if (!inet->sock)
		goto discard;
	switch (inet->p.prot.type) {
	case SOCK_STREAM:
	case SOCK_SEQPACKET:
		switch (inet_get_state(inet)) {
		default:
			fixme(("%s: %p: FIXME: save errors for later\n", CONFIG_STREAMS_INET_NAME,
			       inet));
			return (QR_DONE);
		case TS_IDLE:
			return (QR_DONE);
		}
		return (-EFAULT);
	case SOCK_DGRAM:
	case SOCK_RAW:
	case SOCK_RDM:
		switch (inet_get_state(inet)) {
		case TS_IDLE:
			fixme(("%s: %p: FIXME: generate uderror\n", CONFIG_STREAMS_INET_NAME,
			       inet));
			return (QR_DONE);
		}
		return (-EFAULT);
	}
	return (-EFAULT);
      discard:
	return (QR_DONE);
}

/* 
 *  =========================================================================
 *
 *  QUEUE PUT and SERVICE routines
 *
 *  =========================================================================
 *
 *  WRITE PUT ad SERVICE (Message from above IP-User --> IP-Provider
 *  -------------------------------------------------------------------------
 */
STATIC int
inet_w_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return inet_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return inet_w_proto(q, mp);
	case M_CTL:
	case M_READ:
		return inet_w_read(q, mp);
	case M_FLUSH:
		return inet_w_flush(q, mp);
	case M_IOCTL:
		return inet_w_ioctl(q, mp);
	case M_IOCDATA:
		return inet_w_iocdata(q, mp);
	}
	return (-EOPNOTSUPP);
}

/* 
 *  READ PUT ad SERVICE (Message from below IP-Provider --> IP-User
 *  -------------------------------------------------------------------------
 */
STATIC int
inet_r_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_RSE:
	case M_PCRSE:
		return inet_r_pcrse(q, mp);
	case M_CTL:
	case M_READ:
		return inet_r_read(q, mp);
	case M_ERROR:
		return inet_r_error(q, mp);
	}
	return (QR_PASSFLOW);
}

/* 
 *  =========================================================================
 *
 *  QUEUE PUT and SERVICE routines
 *
 *  =========================================================================
 */

/* 
 *  SRVQ Service Routine
 *  -----------------------------------
 */
STATIC int
inet_srvq(queue_t *q, int (*proc) (queue_t *, mblk_t *))
{
	int rtn = 0;
	mblk_t *mp;
	while ((mp = getq(q))) {
		/* 
		 * Fast Path 
		 */
		if ((rtn = proc(q, mp)) == QR_DONE) {
			freemsg(mp);
			continue;
		}
		switch (rtn) {
		case QR_DONE:
			freemsg(mp);
		case QR_ABSORBED:
			continue;
		case QR_STRIP:
			if (mp->b_cont)
				putbq(q, mp->b_cont);
		case QR_TRIMMED:
			freeb(mp);
			continue;
		case QR_LOOP:
			if (!q->q_next) {
				qreply(q, mp);
				continue;
			}
		case QR_PASSALONG:
			if (q->q_next) {
				putnext(q, mp);
				continue;
			}
			rtn = -EOPNOTSUPP;
		default:
			freemsg(mp);
			continue;
		case QR_DISABLE:
			noenable(q);
			putbq(q, mp);
			rtn = 0;
			break;
		case QR_PASSFLOW:
			if (mp->b_datap->db_type >= QPCTL || canputnext(q)) {
				putnext(q, mp);
				continue;
			}
		case -ENOBUFS:	/* caller must schedule bufcall */
		case -EBUSY:	/* caller must have failed canput */
		case -EAGAIN:	/* caller must re-enable queue */
		case -ENOMEM:	/* caller must re-enable queue */
			if (mp->b_datap->db_type < QPCTL) {
				putbq(q, mp);
				break;
			}
			/* 
			 *  Be careful not to put a priority message
			 *  back on the queue.
			 */
			switch (mp->b_datap->db_type) {
			case M_PCPROTO:
				mp->b_datap->db_type = M_PROTO;
				break;
			case M_PCRSE:
				mp->b_datap->db_type = M_RSE;
				break;
			case M_READ:
				mp->b_datap->db_type = M_CTL;
				break;
			default:
				freemsg(mp);
				continue;
			}
			mp->b_band = 255;
			putq(q, mp);
			break;
		}
		break;
	}
	return (rtn);
}

STATIC int
inet_rsrv(queue_t *q)
{
	return inet_srvq(q, &inet_r_prim);
}

STATIC int
inet_rput(queue_t *q, mblk_t *mp)
{
	putq(q, mp);
	return (0);
}

STATIC int
inet_wput(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_PROTO:
	case M_PCPROTO:
	case M_DATA:
	case M_DELAY:
	case M_READ:
		/* handle rest in service procedure */
		putq(q, mp);
		return (0);
	case M_FLUSH:
		/* always do M_FLUSH in put procedure */
		if (mp->b_rptr[0] & FLUSHW) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(q, mp->b_rptr[1], FLUSHALL);
			else
				flushq(q, FLUSHALL);
			mp->b_rptr[0] &= ~FLUSHW;
		}
		if (mp->b_rptr[0] & FLUSHR) {
			queue_t *rq = RD(q);
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(rq, mp->b_rptr[1], FLUSHALL);
			else
				flushq(rq, FLUSHALL);
			qreply(q, mp);
			return (0);
		}
		break;
	case M_IOCTL:
	case M_IOCDATA:
		break;
	}
	freemsg(mp);
	return (0);
}

STATIC int
inet_wsrv(queue_t *q)
{
	return inet_srvq(q, &inet_w_prim);
}

/* 
 *  =========================================================================
 *
 *  Private Structure allocation, deallocation and cache
 *
 *  =========================================================================
 */
/* initialize the protocol switch table */
STATIC int
inet_init_proto(void)
{
	int ret;
	if ((ret =
	     inet_alloc_proto(PF_INET, SOCK_RAW, IPPROTO_ICMP,
			      makedevice(CONFIG_STREAMS_INET_MAJOR, ICMP_CMINOR), 0) < 0))
		goto error;
	if ((ret =
	     inet_alloc_proto(PF_INET, SOCK_RAW, IPPROTO_IGMP,
			      makedevice(CONFIG_STREAMS_INET_MAJOR, IGMP_CMINOR), 0) < 0))
		goto error;
	if ((ret =
	     inet_alloc_proto(PF_INET, SOCK_RAW, IPPROTO_IPIP,
			      makedevice(CONFIG_STREAMS_INET_MAJOR, IPIP_CMINOR), 0) < 0))
		goto error;
	if ((ret =
	     inet_alloc_proto(PF_INET, SOCK_STREAM, IPPROTO_TCP,
			      makedevice(CONFIG_STREAMS_INET_MAJOR, TCP_CMINOR), 0) < 0))
		goto error;
	if ((ret =
	     inet_alloc_proto(PF_INET, SOCK_RAW, IPPROTO_EGP,
			      makedevice(CONFIG_STREAMS_INET_MAJOR, EGP_CMINOR), 0) < 0))
		goto error;
	if ((ret =
	     inet_alloc_proto(PF_INET, SOCK_RAW, IPPROTO_PUP,
			      makedevice(CONFIG_STREAMS_INET_MAJOR, PUP_CMINOR), 0) < 0))
		goto error;
	if ((ret =
	     inet_alloc_proto(PF_INET, SOCK_DGRAM, IPPROTO_UDP,
			      makedevice(CONFIG_STREAMS_INET_MAJOR, UDP_CMINOR), 0) < 0))
		goto error;
	if ((ret =
	     inet_alloc_proto(PF_INET, SOCK_RAW, IPPROTO_IDP,
			      makedevice(CONFIG_STREAMS_INET_MAJOR, IDP_CMINOR), 0) < 0))
		goto error;
	if ((ret =
	     inet_alloc_proto(PF_INET, SOCK_RAW, IPPROTO_RAW,
			      makedevice(CONFIG_STREAMS_INET_MAJOR, RAWIP_CMINOR), 0) < 0))
		goto error;
#ifdef CONFIG_SCTP
	if ((ret =
	     inet_alloc_proto(PF_INET, SOCK_SEQPACKET, IPPROTO_SCTP,
			      makedevice(CONFIG_STREAMS_INET_MAJOR, SCTP_CMINOR), 0) < 0))
		goto error;
#endif
      error:
	pswerr(("Could not inialize protocol switch table\n"));
	return (-ENOMEM);
}

/* terminate (empty) the protocol switch table */
STATIC int
inet_term_proto(void)
{
	struct inet_protocol *p;
	while ((p = inet_protosw))
		inet_free_proto(p);
	return (0);
}

/* 
 *  =========================================================================
 *
 *  Private Structure allocation, deallocation and cache
 *
 *  =========================================================================
 */
STATIC kmem_cache_t *inet_priv_cachep = NULL;
STATIC int
inet_init_caches(void)
{
	if (!inet_priv_cachep
	    && !(inet_priv_cachep =
		 kmem_cache_create("inet_priv_cachep", sizeof(inet_t), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: Cannot allocate inet_priv_cachep", __FUNCTION__);
		return (-ENOMEM);
	}
	return (0);
}
STATIC void
inet_term_caches(void)
{
	if (inet_priv_cachep) {
		if (kmem_cache_destroy(inet_priv_cachep))
			cmn_err(CE_WARN, "%s: did not destroy inet_priv_cachep", __FUNCTION__);
	}
	return;
}
STATIC inet_t *
inet_alloc_priv(queue_t *q, inet_t ** slp, ushort cmajor, ushort cminor, cred_t *crp,
		const inet_profile_t * prof)
{
	inet_t *inet;
	if ((inet = kmem_cache_alloc(inet_priv_cachep, SLAB_ATOMIC))) {
		bzero(inet, sizeof(*inet));
		inet->cmajor = cmajor;
		inet->cminor = cminor;
		inet->rq = RD(q);
		inet->rq->q_ptr = inet;
		inet->refcnt++;
		inet->wq = WR(q);
		inet->wq->q_ptr = inet;
		inet->refcnt++;
		inet->cred = *crp;
		inet->p = *prof;
		inet->rwait = NULL;
		inet->wwait = NULL;
		inet_set_state(inet, TS_UNBND);
		bufq_init(&inet->conq);
		if ((inet->next = *slp))
			inet->next->prev = &inet->next;
		inet->prev = slp;
		*slp = inet;
		inet->refcnt++;
	}
	return (inet);
}
STATIC void
inet_free_priv(queue_t *q)
{
	inet_t *inet = PRIV(q);
	int flags = 0;
	bufq_purge(&inet->conq);
	if (inet->sock)
		inet_socket_put(inet->sock);
	inet_unbufcall(q);
	if ((*inet->prev = inet->next))
		inet->next->prev = inet->prev;
	inet->next = NULL;
	inet->prev = NULL;
	inet->refcnt--;
	inet->rq->q_ptr = NULL;
	inet->refcnt--;
	inet->wq->q_ptr = NULL;
	inet->refcnt--;
	kmem_cache_free(inet_priv_cachep, inet);
	return;
}

/* 
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 *
 *  OPEN
 *  -------------------------------------------------------------------------
 */
STATIC const inet_profile_t inet_profiles[] = {
	{{PF_INET, SOCK_RAW, IPPROTO_ICMP},
	 {T_INFO_ACK, 0xffff, T_INVALID, T_INVALID, T_INVALID,
	  sizeof(struct sockaddr_in),
	  T_INFINITE, 0xffff, T_CLTS, TS_UNBND, XPG4_1 & ~T_SNDZERO}},
	{{PF_INET, SOCK_RAW, IPPROTO_IGMP},
	 {T_INFO_ACK, 0xffff, T_INVALID, T_INVALID, T_INVALID,
	  sizeof(struct sockaddr_in),
	  T_INFINITE, 0xffff, T_CLTS, TS_UNBND, XPG4_1 & ~T_SNDZERO}},
	{{PF_INET, SOCK_RAW, IPPROTO_IPIP},
	 {T_INFO_ACK, 0xffff, T_INVALID, T_INVALID, T_INVALID,
	  sizeof(struct sockaddr_in),
	  T_INFINITE, 0xffff, T_CLTS, TS_UNBND, XPG4_1 & ~T_SNDZERO}},
	{{PF_INET, SOCK_STREAM, IPPROTO_TCP},
	 {T_INFO_ACK, T_INVALID, 1, T_INVALID, T_INVALID,
	  sizeof(struct sockaddr_in),
	  T_INFINITE, 0xffff, T_COTS_ORD, TS_UNBND, XPG4_1 & ~T_SNDZERO}},
	{{PF_INET, SOCK_RAW, IPPROTO_EGP},
	 {T_INFO_ACK, 0xffff, T_INVALID, T_INVALID, T_INVALID,
	  sizeof(struct sockaddr_in),
	  T_INFINITE, 0xffff, T_CLTS, TS_UNBND, XPG4_1 & ~T_SNDZERO}},
	{{PF_INET, SOCK_RAW, IPPROTO_PUP},
	 {T_INFO_ACK, 0xffff, T_INVALID, T_INVALID, T_INVALID,
	  sizeof(struct sockaddr_in),
	  T_INFINITE, 0xffff, T_CLTS, TS_UNBND, XPG4_1 & ~T_SNDZERO}},
	{{PF_INET, SOCK_DGRAM, IPPROTO_UDP},
	 {T_INFO_ACK, 0xffff, T_INVALID, T_INVALID, T_INVALID,
	  sizeof(struct sockaddr_in),
	  T_INFINITE, 0xffff, T_CLTS, TS_UNBND, XPG4_1 & ~T_SNDZERO}},
	{{PF_INET, SOCK_RAW, IPPROTO_IDP},
	 {T_INFO_ACK, 0xffff, T_INVALID, T_INVALID, T_INVALID,
	  sizeof(struct sockaddr_in),
	  T_INFINITE, 0xffff, T_CLTS, TS_UNBND, XPG4_1 & ~T_SNDZERO}},
	{{PF_INET, SOCK_RAW, IPPROTO_RAW},
	 {T_INFO_ACK, 0xffff, T_INVALID, T_INVALID, T_INVALID,
	  sizeof(struct sockaddr_in),
	  T_INFINITE, 0xffff, T_CLTS, TS_UNBND, XPG4_1 & ~T_SNDZERO}
	 }
};
STATIC int inet_majors[CONFIG_STREAMS_INET_NMAJORS] = { CONFIG_STREAMS_INET_MAJOR, };
STATIC int
inet_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	int flags, mindex = 0;
	int cmajor = getmajor(*devp);
	int cminor = getminor(*devp);
	inet_t *inet, **ipp = &inet_opens;
	const inet_profile_t *prof;
	MOD_INC_USE_COUNT;	/* keep module from unloading in our face */
	if (q->q_ptr != NULL) {
		MOD_DEC_USE_COUNT;
		return (0);	/* already open */
	}
	if (sflag == MODOPEN || WR(q)->q_next) {
		MOD_DEC_USE_COUNT;
		return (EIO);
	}
	if (cmajor != CONFIG_STREAMS_INET_MAJOR || cminor < ICMP_CMINOR || cminor > RAWIP_CMINOR) {
		MOD_DEC_USE_COUNT;
		return (ENXIO);
	}
	prof = &inet_profiles[cminor - ICMP_CMINOR];
	cminor = FREE_CMINOR;
	lis_spin_lock_irqsave(&inet_lock, &flags);
	for (; *ipp; ipp = &(*ipp)->next) {
		if (cmajor != (*ipp)->cmajor)
			break;
		if (cmajor == (*ipp)->cmajor) {
			if (cminor < (*ipp)->cminor)
				break;
			if (cminor == (*ipp)->cminor) {
				if (++cminor >= CONFIG_STREAMS_INET_NMINORS) {
					if (++mindex >= CONFIG_STREAMS_INET_NMAJORS
					    || !(cmajor = inet_majors[mindex]))
						break;
					cminor = 0;
				}
				continue;
			}
		}
	}
	if (mindex >= CONFIG_STREAMS_INET_NMAJORS || !cmajor) {
		lis_spin_unlock_irqrestore(&inet_lock, &flags);
		MOD_DEC_USE_COUNT;
		return (ENXIO);
	}
	*devp = makedevice(cmajor, cminor);
	if (!(inet = inet_alloc_priv(q, ipp, cmajor, cminor, crp, prof))) {
		lis_spin_unlock_irqrestore(&inet_lock, &flags);
		MOD_DEC_USE_COUNT;
		return (ENOMEM);
	}
	lis_spin_unlock_irqrestore(&inet_lock, &flags);
	return (0);
}

/* 
 *  CLOSE
 *  -------------------------------------------------------------------------
 */
STATIC int
inet_close(queue_t *q, int flag, cred_t *crp)
{
	inet_t *inet = PRIV(q);
	int flags;
	(void) flag;
	(void) crp;
	(void) inet;
	lis_spin_lock_irqsave(&inet_lock, &flags);
	inet_free_priv(q);
	lis_spin_unlock_irqrestore(&inet_lock, &flags);
	MOD_DEC_USE_COUNT;
	return (0);
}

/* 
 *  =========================================================================
 *
 *  LiS Module Initialization
 *
 *  =========================================================================
 */
STATIC int inet_initialized = 0;
STATIC void
inet_init(void)
{
	int err, mindex;
#ifdef MODULE
	cmn_err(CE_NOTE, INET_BANNER);	/* log message */
#else
	cmn_err(CE_NOTE, INET_SPLASH);	/* console splash */
#endif
	if ((err = inet_init_caches())) {
		cmn_err(CE_PANIC, "%s: ERROR: Counld not allocated caches",
			CONFIG_STREAMS_INET_NAME);
		inet_initialized = err;
		return;
	}
	for (mindex = 0; mindex < CONFIG_STREAMS_INET_NMAJORS; mindex++) {
		if ((err =
		     lis_register_strdev(inet_majors[mindex], &inet_info,
					 CONFIG_STREAMS_INET_NMINORS,
					 CONFIG_STREAMS_INET_NAME)) < 0) {
			if (!mindex) {
				cmn_err(CE_PANIC, "%s: Can't register 1'st
						major %d", CONFIG_STREAMS_INET_NAME, inet_majors[0]);
				inet_term_caches();
				inet_initialized = err;
				return;
			}
			cmn_err(CE_WARN, "%s: Can't register %d'th major", CONFIG_STREAMS_INET_NAME,
				mindex + 1);
			inet_majors[mindex] = 0;
		} else if (mindex)
			inet_majors[mindex] = err;
	}
	inet_initialized = 1;
	return;
}
STATIC void
inet_terminate(void)
{
	int err, mindex;
	for (mindex = 0; mindex < CONFIG_STREAMS_INET_NMAJORS; mindex++) {
		if (inet_majors[mindex]) {
			if ((err = lis_unregister_strdev(inet_majors[mindex])))
				cmn_err(CE_PANIC, "%s: Can't unregister major %d\n",
					CONFIG_STREAMS_INET_NAME, inet_majors[mindex]);
			if (mindex)
				inet_majors[mindex] = 0;
		}
	}
	inet_term_caches();
	return;
}

/* 
 *  =========================================================================
 *
 *  LINUX MODULE INITIALIZATION
 *
 *  =========================================================================
 */
int
init_module(void)
{
	inet_init();
	if (inet_initialized < 0)
		return inet_initialized;
	return (0);
}

void
cleanup_module(void)
{
	inet_terminate();
}
