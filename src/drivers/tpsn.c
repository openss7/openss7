/*****************************************************************************

 @(#) File: src/drivers/tpsn.c

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

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
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 *****************************************************************************/

static char const ident[] = "src/drivers/tpsn.c (" PACKAGE_ENVR ") " PACKAGE_DATE;

/*
 * This file provides an integrated driver for ISO/OSI X.224/X.234  Connection-mode and
 * connectionless-mode transport protocol running over a ISO/IEC 8802 LAN with subnetwork-only
 * addressing and a null network layer as a non-multiplexing pseudo-device driver.  The driver
 * provides a Transport Provider Interface (TPI) interface and can be used directly with the XTI/TLI
 * library.  No configuration is necessary to use the driver and associated devices: the Linux
 * network device independent subsystem is used internally instead of linking network drivers and
 * datalink drivers.
 *
 * The TPSN driver can support null network layer (inactive subset) subnetwork interconnection for
 * CLNP. ISO-IP and ISO-UDP in accordance with RFC 1070 operation is also supported.
 */

//#define _DEBUG 1
//#undef _DEBUG

#define _SVR4_SOURCE	1
#define _MPS_SOURCE	1

#include <sys/os7/compat.h>
#include <sys/strsun.h>

#include <linux/bitops.h>

#define t_tst_bit(nr,addr)	test_bit(nr,addr)
#define t_set_bit(nr,addr)	__set_bit(nr,addr)
#define t_clr_bit(nr,addr)	__clear_bit(nr,addr)

#include <linux/interrupt.h>
#ifdef HAVE_KINC_ASM_SOFTIRQ_H
#include <asm/softirq.h>	/* for start_bh_atomic, end_bh_atomic */
#endif
#include <linux/random.h>	/* for secure_tcp_sequence_number */
#ifdef HAVE_KINC_LINUX_RCUPDATE_H
#include <linux/rcupdate.h>
#endif

#ifdef HAVE_KINC_LINUX_BRLOCK_H
#include <linux/brlock.h>
#endif

#include <linux/udp.h>

#include <linux/net.h>
#include <linux/in.h>
#include <linux/un.h>
#include <linux/ip.h>

#undef ASSERT

#include <net/sock.h>
#include <net/ip.h>
#include <net/icmp.h>
#include <net/route.h>
#include <net/inet_ecn.h>
#include <net/snmp.h>

#include <net/udp.h>
#include <net/llc.h>
#include <net/llc_pdu.h>

#ifdef HAVE_KINC_NET_DST_H
#include <net/dst.h>
#endif

#include <net/protocol.h>

#include <linux/skbuff.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/inetdevice.h>

#include "net_hooks.h"

#if 0
/* Turn on some tracing and debugging. */
#undef ensure
#undef assure

#define ensure __ensure
#define assure __assure
#endif

#if defined HAVE_TIHDR_H
#   include <tihdr.h>
#else
#   include <sys/tihdr.h>
#endif

#include <sys/xti.h>
#include <sys/xti_ip.h>
#include <sys/xti_udp.h>
#include <sys/xti_osi.h>

#define T_ALLLEVELS -1

#define LINUX_2_4 1

#define	TP_DESCRIP	"OSI Transport Provider for Subnetworks (TPSN) STREAMS Driver"
#define TP_EXTRA	"Part of the OpenSS7 OSI Stack for Linux Fast-STREAMS"
#define TP_COPYRIGHT	"Copyright (c) 2008-2015  Monavacon Ltd.  All Rights Reserved."
#define TP_REVISION	"OpenSS7 src/drivers/tpsn.c (" PACKAGE_ENVR ") " PACKAGE_DATE
#define TP_DEVICE	"SVR 4.2 MP STREAMS TPI OSI Transport for Subnetworks Driver"
#define TP_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define TP_LICENSE	"GPL"
#define TP_BANNER	TP_DESCRIP	"\n" \
			TP_EXTRA	"\n" \
			TP_COPYRIGHT	"\n" \
			TP_DEVICE	"\n" \
			TP_CONTACT
#define TP_SPLASH	TP_DESCRIP	" - " \
			TP_REVISION

#ifdef LINUX
MODULE_AUTHOR(TP_CONTACT);
MODULE_DESCRIPTION(TP_DESCRIP);
MODULE_SUPPORTED_DEVICE(TP_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(TP_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-tpsn");
#endif
#ifdef MODULE_VERSION
MODULE_VERSION(PACKAGE_ENVR);
#endif
#endif

#define TP_DRV_ID	CONFIG_STREAMS_TPSN_MODID
#define TP_MOD_ID	CONFIG_STREAMS_TPSN_MODID
#define TP_DRV_NAME	CONFIG_STREAMS_TPSN_NAME
#define TP_MOD_NAME	CONFIG_STREAMS_TPSN_NAME
#define TP_CMAJORS	CONFIG_STREAMS_TPSN_NMAJOR
#define TP_CMAJOR_0	CONFIG_STREAMS_TPSN_MAJOR
#define TP_UNITS	CONFIG_STREAMS_TPSN_NMINORS

#ifdef LINUX
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_TPSN_MODID));
MODULE_ALIAS("streams-driver-tpsn");
MODULE_ALIAS("streams-module-tpsn");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_TPSN_MAJOR));
MODULE_ALIAS("/dev/streams/tpsn");
MODULE_ALIAS("/dev/streams/tpsn/*");
MODULE_ALIAS("/dev/streams/clone/tpsn");
MODULE_ALIAS("char-major-" __stringify(CONFIG_STREAMS_CLONE_MAJOR) "-" __stringify(TP_CMAJOR_0));
MODULE_ALIAS("/dev/tpsn");
//MODULE_ALIAS("devname:tpsn");
MODULE_ALIAS("/dev/clts");
MODULE_ALIAS("/dev/cots");
#endif				/* MOUDLE_ALIAS */
#endif				/* LINUX */

#ifdef _OPTIMIZE_SPEED
#define INETLOG(tp, level, flags, fmt, ...) \
	do { } while (0)
#else
#define INETLOG(tp, level, flags, fmt, ...) \
	mi_strlog(tp->rq, level, flags, fmt, ##__VA_ARGS__)
#endif

#define STRLOGERR	0	/* log INET error information */
#define STRLOGNO	0	/* log INET notice information */
#define STRLOGST	1	/* log INET state transitions */
#define STRLOGTO	2	/* log INET timeouts */
#define STRLOGRX	3	/* log INET primitives received */
#define STRLOGTX	4	/* log INET primitives issued */
#define STRLOGTE	5	/* log INET timer events */
#define STRLOGIO	6	/* log INET additional data */
#define STRLOGDA	7	/* log INET data */

#if 1
#define LOGERR(tp, fmt, ...) INETLOG(tp, STRLOGERR, SL_TRACE | SL_ERROR | SL_CONSOLE, fmt, ##__VA_ARGS__)
#define LOGNO(tp, fmt, ...) INETLOG(tp, STRLOGNO, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGST(tp, fmt, ...) INETLOG(tp, STRLOGST, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGTO(tp, fmt, ...) INETLOG(tp, STRLOGTO, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGRX(tp, fmt, ...) INETLOG(tp, STRLOGRX, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGTX(tp, fmt, ...) INETLOG(tp, STRLOGTX, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGTE(tp, fmt, ...) INETLOG(tp, STRLOGTE, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGIO(tp, fmt, ...) INETLOG(tp, STRLOGIO, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGDA(tp, fmt, ...) INETLOG(tp, STRLOGDA, SL_TRACE, fmt, ##__VA_ARGS__)
#else
#define LOGERR(tp, fmt, ...) INETLOG(tp, STRLOGERR, SL_TRACE | SL_ERROR | SL_CONSOLE, fmt, ##__VA_ARGS__)
#define LOGNO(tp, fmt, ...) INETLOG(tp, STRLOGNO, SL_TRACE | SL_ERROR | SL_CONSOLE, fmt, ##__VA_ARGS__)
#define LOGST(tp, fmt, ...) INETLOG(tp, STRLOGST, SL_TRACE | SL_ERROR | SL_CONSOLE, fmt, ##__VA_ARGS__)
#define LOGTO(tp, fmt, ...) INETLOG(tp, STRLOGTO, SL_TRACE | SL_ERROR | SL_CONSOLE, fmt, ##__VA_ARGS__)
#define LOGRX(tp, fmt, ...) INETLOG(tp, STRLOGRX, SL_TRACE | SL_ERROR | SL_CONSOLE, fmt, ##__VA_ARGS__)
#define LOGTX(tp, fmt, ...) INETLOG(tp, STRLOGTX, SL_TRACE | SL_ERROR | SL_CONSOLE, fmt, ##__VA_ARGS__)
#define LOGTE(tp, fmt, ...) INETLOG(tp, STRLOGTE, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGIO(tp, fmt, ...) INETLOG(tp, STRLOGIO, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGDA(tp, fmt, ...) INETLOG(tp, STRLOGDA, SL_TRACE, fmt, ##__VA_ARGS__)
#endif

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

#define DRV_ID		TP_DRV_ID
#define DRV_NAME	TP_DRV_NAME
#define CMAJORS		TP_CMAJORS
#define CMAJOR_0	TP_CMAJOR_0
#define UNITS		TP_UNITS
#ifdef MODULE
#define DRV_BANNER	TP_BANNER
#else				/* MODULE */
#define DRV_BANNER	TP_SPLASH
#endif				/* MODULE */

static struct module_info tp_rinfo = {
	.mi_idnum = DRV_ID,	/* Module ID number */
	.mi_idname = DRV_NAME,	/* Module name */
	.mi_minpsz = 0,		/* Min packet size accepted */
	.mi_maxpsz = (1 << 16),	/* Max packet size accepted */
	.mi_hiwat = SHEADHIWAT << 5,	/* Hi water mark */
	.mi_lowat = 0,		/* Lo water mark */
};

static struct module_stat tp_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat tp_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static streamscall int tp_qopen(queue_t *, dev_t *, int, int, cred_t *);
static streamscall int tp_qclose(queue_t *, int, cred_t *);

static streamscall int tp_rput(queue_t *, mblk_t *);
static streamscall int tp_rsrv(queue_t *);

static struct qinit tp_rinit = {
	.qi_putp = tp_rput,	/* Read put (msg from below) */
	.qi_srvp = tp_rsrv,	/* Read queue service */
	.qi_qopen = tp_qopen,	/* Each open */
	.qi_qclose = tp_qclose,	/* Last close */
	.qi_minfo = &tp_rinfo,	/* Information */
	.qi_mstat = &tp_rstat,	/* Statistics */
};

static struct module_info tp_winfo = {
	.mi_idnum = DRV_ID,	/* Module ID number */
	.mi_idname = DRV_NAME,	/* Module name */
	.mi_minpsz = 0,		/* Min packet size accepted */
	.mi_maxpsz = (1 << 16),	/* Max packet size accepted */
	.mi_hiwat = (SHEADHIWAT >> 1),	/* Hi water mark */
	.mi_lowat = 0,		/* Lo water mark */
};

static streamscall int tp_wput(queue_t *, mblk_t *);
static streamscall int tp_wsrv(queue_t *);

static struct qinit tp_winit = {
	.qi_putp = tp_wput,	/* Write put (msg from above) */
	.qi_srvp = tp_wsrv,	/* Write queue service */
	.qi_minfo = &tp_winfo,	/* Information */
	.qi_mstat = &tp_wstat,	/* Statistics */
};

MODULE_STATIC struct streamtab tp_info = {
	.st_rdinit = &tp_rinit,	/* Upper read queue */
	.st_wrinit = &tp_winit,	/* Lower read queue */
};

/*
   TLI interface state flags 
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

#if !defined HAVE_KMEMB_STRUCT_SK_BUFF_TRANSPORT_HEADER
#if !defined HAVE_KFUNC_SKB_TRANSPORT_HEADER
static inline unsigned char *
skb_tail_pointer(const struct sk_buff *skb)
{
	return skb->tail;
}

static inline unsigned char *
skb_end_pointer(const struct sk_buff *skb)
{
	return skb->end;
}

static inline unsigned char *
skb_transport_header(const struct sk_buff *skb)
{
	return skb->h.raw;
}

static inline unsigned char *
skb_network_header(const struct sk_buff *skb)
{
	return skb->nh.raw;
}

static inline unsigned char *
skb_mac_header(const struct sk_buff *skb)
{
	return skb->mac.raw;
}

static inline void
skb_reset_tail_pointer(struct sk_buff *skb)
{
	skb->tail = skb->data;
}

static inline void
skb_reset_end_pointer(struct sk_buff *skb)
{
	skb->end = skb->data;
}

static inline void
skb_reset_transport_header(struct sk_buff *skb)
{
	skb->h.raw = skb->data;
}

static inline void
skb_reset_network_header(struct sk_buff *skb)
{
	skb->nh.raw = skb->data;
}

static inline void
skb_reset_mac_header(struct sk_buff *skb)
{
	skb->mac.raw = skb->data;
}

static inline void
skb_set_tail_pointer(struct sk_buff *skb, const int offset)
{
	skb_reset_tail_pointer(skb);
	skb->tail += offset;
}

static inline void
skb_set_transport_header(struct sk_buff *skb, const int offset)
{
	skb_reset_transport_header(skb);
	skb->h.raw += offset;
}

static inline void
skb_set_network_header(struct sk_buff *skb, const int offset)
{
	skb_reset_network_header(skb);
	skb->nh.raw += offset;
}

static inline void
skb_set_mac_header(struct sk_buff *skb, const int offset)
{
	skb_reset_mac_header(skb);
	skb->mac.raw += offset;
}
#endif				/* !defined HAVE_KFUNC_SKB_TRANSPORT_HEADER */
#endif				/* !defined HAVE_KMEMB_STRUCT_SK_BUFF_TRANSPORT_HEADER */

/*
 *  =========================================================================
 *
 *  Predicates for class 4 over CLNS
 *
 *  =========================================================================
 *  The following assumption and notation are used:
 *
 *  a)	local-ref -- The reference (local) of the TC is chosen when sending the CR or when accepting
 *	a CR:
 *	--  remote-ref -- The reference of the remote entity is initially set to zero and
 *	    initialized when processing the CC except if the CC is ignored.
 *	--  SRC-REF -- Designates the corresponding field of the received TPDU.
 *	--  DST-REF -- Deisgnates the corresponding field of the received TPDU.
 *	--  src-ref, dst-ref -- Designates the corresponding fields of the sent TPDU.
 *	--  count -- Designates the number of times a TPDU has been sent (retransmissions).
 *
 *  b)	The data transfer phase is not completely described in the state table but refers to the
 *	main text.
 *
 *  c)	It is assumed that the network services is continuously available.
 *
 *  The operation resulting from signalled inaccessibility of the network services are a local
 *  matter.
 */
#define _TP_PREDICATE_P0	 0	/* T-CONNECT request is acceptable */
#define _TP_PREDICATE_P3	 3	/* local choke */
#define _TP_PREDICATE_P7	 7	/* count = maximum */
#define _TP_PREDICATE_P8	 8	/* acceptable CR-TPDU */
#define _TP_PREDICATE_P9	 9	/* acceptable class 4 CC-TPDU */

/*
 *  =========================================================================
 *
 *  Specific actions
 *
 *  =========================================================================
 */
#define _TP_ACTION_0	 0	/* set reference timer */
#define _TP_ACTION_1	 1	/* count = count + 1 */
#define _TP_ACTION_2	 2	/* count = 0-*/
#define _TP_ACTION_3	 3	/* set retransmission timer */
#define _TP_ACTION_4	 4	/* stop retransmission timer if running */
#define _TP_ACTION_5	 5	/* set window timer */
#define _TP_ACTION_6	 6	/* stop window timer if running */
#define _TP_ACTION_7	 7	/* set inactivity timer */
#define _TP_ACTION_8	 8	/* stop inactivity timer if running */
#define _TP_ACTION_9	 9	/* set initial credit for sending according to the received CR/CC-TPDU */
#define _TP_ACTION_10	10	/* set initial credit for controlling reception according to the sent CR/CC-TPDU */
#define _TP_ACTION_15	15	/* send the DR-TPDU.  This DR-TPDU is sent with src-ref = local-ref
				   and dst-ref = remote-ref (may be zero) */
#define _TP_ACTION_16	16	/* send the DR-TPDU.  The DR-TPDU is sent with src-ref = 0 and
				   dst-ref = remote-ref */
#define _TP_ACTION_17	17	/* send a TPDU accourding to data transfer procedure */
#define _TP_ACTION_20	20	/* store request and exercise flow control to the user */
#define _TP_ACTION_21	21	/* send a DC-TPDU with src-ref equal to zero */
#define _TP_ACTION_22	22	/* send a DC-TPDU except if the SRC-REF field of the received
				   DR-TDPU is equal to zero */
#define _TP_ACTION_23	23	/* send a DR-TPDU with src-ref = local-ref and dst-ref = SRC-REF in CC-TPDU */

/*
 *  =========================================================================
 *
 *  Specific notes
 *
 *  =========================================================================
 */
#define _TP_NOTE_5	 5	/* not a duplicated CR-TPDU; if duplicated, ignore it */
#define _TP_NOTE_7	 7	/* as a local choice it is also possible to apply the following [0], TDISind, REFWAIT */
#define _TP_NOTE_8	 8	/* asociation to this transport connection is done regardless of the
				   SRC-REF field.  If SRC-REF is not zero, a DC-TPDU is sent back.  */
#define _TP_NOTE_9	 9	/* at least an AK-TPDU shall be sent if the transport entity is the
				   initiator in order to ensure that the responder will complete its
				   three-way handshake. */
#define _TP_NOTE_10	10	/* if association has been made, and DST-REF is zero, then the
				   DC-TPDU contains a src-ref field set to zero */
#define _TP_NOTE_11	11	/* if the CLOSING state has been entered, coming from WFCC state,
				   the remote-ref is zero.  The SRC-REF field of the CC-TPDU is
				   ignored (i.e. if the DR-TPDU is retransmitted, it will be with
				   the dst-ref field set to zero). */
#define _TP_NOTE_13	13	/* The DR-TPDU may be either repeated immediately or when T1 will
				   run out */
#define _TP_NOTE_15	15	/* Previously stored T-DATA or T-EXPEDITED-DATA requests are ready
				   for processing according to data transfer procedures. */
#define _TP_NOTE_16	16	/* See data transfer procedures. */

/*
 *  =========================================================================
 *
 *  States:
 *
 *  =========================================================================
 */

#define _TP_STATE_REFWAIT	 0
#define _TP_STATE_CLOSED	 1
#define _TP_STATE_WFCC		 2
#define _TP_STATE_WBCL		 3
#define _TP_STATE_OPEN		 4
#define _TP_STATE_WFTRESP	 5
#define _TP_STATE_AKWAIT	 6
#define _TP_STATE_CLOSING	 7

/*
 *  =========================================================================
 *
 *  TP Private Datastructures
 *
 *  =========================================================================
 */

/*
 *  Option Handling
 */
#define T_SPACE(len) \
	(sizeof(struct t_opthdr) + T_ALIGN(len))

#define T_LENGTH(len) \
	(sizeof(struct t_opthdr) + len)

#define _T_SPACE_SIZEOF(s) \
	T_SPACE(sizeof(s))

#define _T_LENGTH_SIZEOF(s) \
	T_LENGTH(sizeof(s))

struct tp_options {
	unsigned long flags[3];		/* at least 96 flags */
	struct {
		t_uscalar_t xti_debug[4];	/* XTI_DEBUG */
		struct t_linger xti_linger;	/* XTI_LINGER */
		t_uscalar_t xti_rcvbuf;	/* XTI_RCVBUF */
		t_uscalar_t xti_rcvlowat;	/* XTI_RCVLOWAT */
		t_uscalar_t xti_sndbuf;	/* XTI_SNDBUF */
		t_uscalar_t xti_sndlowat;	/* XTI_SNDLOWAT */
	} xti;
	struct {
		struct thrpt tco_throughput;	/* T_TCO_THROUGHPUT */
		struct transdel tco_transdel;	/* T_TCO_TRANSDEL */
		struct rate tco_reserrorrate;	/* T_TCO_RESERRORRATE */
		struct rate tco_transffailprob;	/* T_TCO_TRANSFFAILPROB */
		struct rate tco_estfailprob;	/* T_TCO_ESTFAILPROB */
		struct rate tco_relfailprob;	/* T_TCO_RELFAILPROB */
		struct rate tco_estdelay;	/* T_TCO_ESTDELAY */
		struct rate tco_reldelay;	/* T_TCO_RELDELAY */
		struct rate tco_connresil;	/* T_TCO_CONNRESIL */
		t_uscalar_t tco_protection;	/* T_TCO_PROTECTION */
		t_uscalar_t tco_priority;	/* T_TCO_PRIORITY */
		t_uscalar_t tco_expd;	/* T_TCO_EXPD */

		t_uscalar_t tco_ltpdu;	/* T_TCO_LTPDU */
		t_uscalar_t tco_acktime;	/* T_TCO_ACKTIME */
		t_uscalar_t tco_reastime;	/* T_TCO_REASTIME */
		t_uscalar_t tco_prefclass;	/* T_TCO_PREFCLASS */
		t_uscalar_t tco_altclass1;	/* T_TCO_ALTCLASS1 */
		t_uscalar_t tco_altclass2;	/* T_TCO_ALTCLASS2 */
		t_uscalar_t tco_altclass3;	/* T_TCO_ALTCLASS3 */
		t_uscalar_t tco_altclass4;	/* T_TCO_ALTCLASS4 */
		t_uscalar_t tco_extform;	/* T_TCO_EXTFORM */
		t_uscalar_t tco_flowctrl;	/* T_TCO_FLOWCTRL */
		t_uscalar_t tco_checksum;	/* T_TCO_CHECKSUM */
		t_uscalar_t tco_netexp;	/* T_TCO_NETEXP */
		t_uscalar_t tco_netrecptcf;	/* T_TCO_NETRECPTCF */
		t_uscalar_t tco_selectack;	/* T_TCO_SELECTACK */
		t_uscalar_t tco_requestack;	/* T_TCO_REQUESTACK */
		t_uscalar_t tco_nblkexpdata;	/* T_TCO_NBLKEXPDATA */
	} tco;
	struct {
		struct transdel tcl_transdel;	/* T_TCL_TRANSDEL */
		struct rate tcl_reserrorrate;	/* T_TCL_RESERRORRATE */
		t_uscalar_t tcl_protection;	/* T_TCL_PROTECTION */
		t_uscalar_t tcl_priority;	/* T_TCL_PRIORITY */
		t_uscalar_t tcl_checksum;	/* T_TCL_CHECKSUM */
	} tcl;
	struct {
		unsigned char ip_options[40];	/* T_IP_OPTIONS */
		unsigned char ip_tos;	/* T_IP_TOS */
		unsigned char ip_ttl;	/* T_IP_TTL */
		unsigned int ip_reuseaddr;	/* T_IP_REUSEADDR */
		unsigned int ip_dontroute;	/* T_IP_DONTROUTE */
		unsigned int ip_broadcast;	/* T_IP_BROADCAST */
		uint32_t ip_addr;	/* T_IP_ADDR */
	} ip;
	struct {
		t_uscalar_t udp_checksum;	/* T_UDP_CHECKSUM */
	} udp;
};

struct tp;

struct tp_timer {
	int timer;
	uint count;
	struct tp *tp;
};

/*
 *  Connection-mode Service
 *  -----------------------
 *  The protocol level of all subsequent options is T_ISO_TP.
 *
 *  All options are association-related, that is, they are options with end-to-end significance (see
 *  the Use of Options in XTI).  They may be negotiated in the XTI states T_IDLE and T_INCON, and
 *  are read-only in all other states except T_UNINIT.
 *
 *  Absolute Requirements
 *  ---------------------
 *  For the options in Options for Quality of Service and Expedited Data, the transport user can
 *  indicate whether the request is an absolute requirement or whether a degraded value is
 *  acceptable.  For the QOS options based on struct rate, an absolute requirement is specified via
 *  the field minacceptvalue, if that field is given a value different from T_UNSPEC.  The value
 *  specified for T_TCO_PROTECTION is an absolute requirement if the T_ABSREQ flag is set.  The
 *  values specified for T_TCO_EXPD and T_TCO_PRIORITY are never absolute requirements.
 *
 *  Further Remarks
 *  ---------------
 *  A detailed description of the options for Quality of Service can be found in ISO 8072
 *  specification. The field elements of the structures in use for the option values are
 *  self-explanatory.  Only the following details remain to be explained.
 *
 *  - If these options are returned with t_listen(), their values are related to the incoming
 *    connection and not to the transport endpoint where t_listen() was issued.  To give an example,
 *    the value of T_TCO_PROTECTION is the value sent by the calling transport user, and not the
 *    value currently effective for the endpoint (that could be retrieved by t_optmgmt() with the
 *    flag T_CURRENT set).  The option is not returned at all if the calling user did not specify
 *    it.  An analogous procedure applies for the other options.  See also The Use of Options in
 *    XTI.
 *
 *  - If, in a call to t_accept(), the called transport user tried to negotiate an option of higher
 *    quality than proposed, the option is rejected and the connection establishment fails (see
 *    Responding to a Negotiation Proposal).
 *
 *  - The values of the QOS options T_TCO_THROUGHPUT, T_TCO_TRANSDEL, T_TCO_RESERRRATE,
 *    T_TCO_TRANSFFAILPROB, T_TCO_ESTFAILPROB, T_TCO_RELFAILPROB, T_TCO_ESTDELAY, T_TCO_RELDELAY and
 *    T_TCO_CONNRESIL have a structured format.  A user requesting one of these options might leave
 *    a field of the structure unspecified by setting it to T_UNSPEC.  The transport provider is
 *    then free to select an appropriate value for this field.  The transport provider may return
 *    T_UNSPEC in a field of the structure to the user to indicate that it has not yet decided on a
 *    definite value for this field.
 *
 *    T_UNSPEC is not a legal value for T_TCO_PROTECTION, T_TCO_PRIORITY and T_TCO_EXPD.
 *
 *  - T_TCO_THROUGHPUT and T_TCO_TRANSDEL If avgthrpt (average throughput) is not defined (both
 *    fields set to T_UNSPEC), the transport provider considers that the average throughput has the
 *    same values as the maximum throughput (maxthrpt).  An analogous procedure applies to
 *    T_TCO_TRANSDEL.
 *
 *  - The ISO specification ISO 8073:1986 does not differentiate between average and maximum transit
 *    delay.  Transport providers that support this option adopt the values of the maximum delay as
 *    input for the CR TPDU.
 *
 *  - T_TCO_PROTECTION  This option defines the general level of protection.  The symbolic constants
 *    in the following list are used to specify the required level of protection:
 *
 *    T_NOPROTECT	- No protection feature
 *    T_PASSIVEPROTECT	- Protection against passive monitoring.
 *    T_ACITVEPROTECT	- Protection against modification, replay, addition or deletion.
 *
 *    Both flags T_PASSIVEPROTECT and T_ACTIVEPROTECT may be set simultaneously but are exclusive
 *    with T_NOPROTECT.  If the T_ACTIVEPROTECT or T_PASSIVEPROTECT flags are set, the user may
 *    indicate that this is an absolute requirement by also setting the T_ABSREQ flag.
 *
 *  - T_TCO_PRIORITY  Fiver priority levels are defined by XTI:
 *
 *    T_PRIDFLT		- Lower level.
 *    T_PRILOW		- Low level.
 *    T_PRIMID		- Mid level.
 *    T_PRIHIGH		- High level.
 *    T_PRITOP		- Top level.
 *
 *    The number of priority levels is not defined in ISO 8072:1986.  The parameter only has meaning
 *    in the context of some management entity or structure able to judge relative importance.
 *
 *  - It is recommended that transport users avoid expedited data with an ISO-over-TCP transport
 *    provider, since the RFC 1006 treatment of expedited data does not meet the data reordering
 *    requirements specified in ISO 8072:1986, and may not be supported by the provider.
 *
 *  Management Options
 *  ------------------
 *  These options are parameters of an ISO transport protocol according to ISO 8073:1986.  They are
 *  not included in the ISO transport service definition ISO 8072:1986, but are additionally offered
 *  by XTI.  Transport users wishing to be truly ISO-compliant should thus not adhere to them.
 *  T_TCO_LTPDU is the only management option supported by an ISO-over-TCP transport provider.
 *
 *  Avoid specifying both QOS parameters and management options at the same time.
 *
 *  Absolute Requirements
 *  ---------------------
 *  A request for any of these options is considered an absolute requirement.
 *
 *  Further Remarks
 *  ---------------
 *  - If these options are returned with t_listen() their values are related to the incoming
 *    connection and not to the transport endpoing where t_listen() was issued.  That means that
 *    t_optmgmt() with the flag T_CURRENT set would usually yield a different result (see the Use of
 *    Options in XTI).
 *
 *  - For management options that are subject to perr-to-peer negotiation the following holds: If,
 *    in a call to t_accept(), the called transport user tries to negotiate an option of higher
 *    quality than proposed, the option is rejected and the connection establishment fails (see
 *    Responding to a Negotiation Proposal).
 *
 *  - A connection-mode transport provider may allow  the transport user to select more than one
 *    alternative class.  The transport user may use the options T_TCO_ALLCLASS1, T_TCO_ALLCLASS2,
 *    etc. to detnote the alternatives.  A transport provider only supports an implementation
 *    dependent limit of alternatives and ignores the rest.
 *
 *  - The value T_UNSPEC is legal for all options in Management Options.  It may be set by the user
 *     to indicate that the transport provider is free to choose any appropriate value.  If returned
 *     by the transport provider, it indicates that the transport provider has not yet decided on a
 *     specific value.
 *
 *  - Legal values for the options T_TCO_PREFCLASS, T_TCO_ALTCLASS1, T_TCO_ALTCLASS2,
 *    T_TCO_ALTCLASS23 and T_TCO_ALTCLASS4 are T_CLASS0, T_CLASS1, T_CLASS2, T_CLASS3, T_CLASS4 and
 *    T_UNSPEC.
 *
 *  - If a connection has been established, T_TCO_PREFCLASS will be set to the selected value, and
 *    T_ALTCLASS1 through T_ALTCLASS4 will be set to T_UNSPEC, if these options are supported.
 *
 *  - Warning on the use of T_TCO_LTPDU: Sensible use of this option requires that the application
 *    programmer knows about system internals.  Careless setting of either a lower or a higher value
 *    than the implementation-dependent default may degrade the performance.
 *
 *    Legal values for an ISO transport provider are T_UNSPEC and multiples of 128 up to 128*(232-1)
 *    or the largest multiple of 128 that will fit in a t_uscalar_t.  Values other than powers of 2
 *    between 27 and 213 are only supported by transport providers that conform to the 1992 updated
 *    to ISO 8073.
 *
 *    Legal values for an ISO-over-TCP providers are T_UNSPEC and any power of 2 between 2**7 and
 *    2**11, and 65531.
 *
 *    The action taken by a transport provider is implementation dependent if a value is specified
 *    which is not exactly as defined in ISO 8073:1986 or its addendums.
 *
 *  - The management options are not independent of one another, and not independent of the options
 *    defined in Options for Quality of Service and Expedited Data.  A transport user must take care
 *    not to request conflicting values.  If conflicts are detected at negotiation time, the
 *    negotiation fails according to the rules for absolute requirements (see The Use of Options in
 *    XTI).  Conflicts that cannot be detected at negotiation time will lead to unpredictable
 *    results in the course of communication.  Usually, conflicts are detected at the time the
 *    connection is established.
 *
 *  Some relations that must be obeyed are:
 *
 *  - If T_TCO_EXP is set to T_YES and T_TCO_PREFCLASS is set to T_CLASS2, T_TCO_FLOWCTRL must also
 *    be set to T_YES.
 *
 *  - If T_TCO_PRECLASS is set to T_CLASS0, T_TCO_EXP must be set to T_NO.
 *
 *  - The value in T_TCO_PREFCLASS must not be lower than the value in T_TCO_ALTCLASS1,
 *    T_TCO_ALTCLASS2, and so on.
 *
 *  - Depending on the chosen QOS options, further value conflicts might occur.
 *
 *  Connectionless-mode Service
 *  ----------------------------
 */

#define TP_CMINOR_COTS		0
#define TP_CMINOR_CLTS		1
#define TP_CMINOR_COTS_ISO	2
#define TP_CMINOR_CLTS_ISO	3
#define TP_CMINOR_COTS_IP	4
#define TP_CMINOR_CLTS_IP	5
#define TP_CMINOR_COTS_UDP	6
#define TP_CMINOR_CLTS_UDP	7

struct tp_profile {
	struct {
		uint family;
		uint type;
		uint protocol;
	} prot;
	struct T_info_ack info;
};

#define TP_SNPA_TYPE_WILDCARD	0	/* any address */
#define TP_SNPA_TYPE_MAC	1	/* 6 octet MAC address */
#define TP_SNPA_TYPE_IP4	2	/* 4 octet IPv4 address */
#define TP_SNPA_TYPE_IP4_PORT	3	/* 4 octet IPv4 address + 2 octet port */
#define TP_SNPA_TYPE_IP6	4	/* 16 octet IPv6 address */
#define TP_SNPA_TYPE_IP6_PORT	5	/* 16 octet IPv6 address + 2 octet port */
#define TP_SNPA_TYPE_IFNAME	6	/* up to 15 octet zero terminated name */

struct snpaaddr {
	int type;
	unsigned char addr[16];		/* SNPA (MAC or IP) address */
	unsigned char subnet[2];	/* SNPA subnet or IP port */
	unsigned char lsap;
	struct net_device *dev;
};

struct nsapaddr {
	unsigned char addr[20];
	unsigned int len;
	int grp;

};

struct tsapaddr {
	unsigned char tsel[2];
	struct nsapaddr nsap;
};

struct tp_bhash_bucket;
struct tp_chash_bucket;
struct tp_lhash_bucket;

struct tp_ipv4_daddr {
	uint32_t addr;			/* IP address this destination */
	uint32_t saddr;			/* current source address */
	unsigned char ttl;		/* time to live, this destination */
	unsigned char tos;		/* type of service, this destination */
	unsigned short mtu;		/* maximum transfer unit this destination */
	struct dst_entry *dst;		/* route for this destination */
	int oif;			/* current interface */
};

struct tp_ipv4_saddr {
	uint32_t addr;			/* IP address this source */
};

struct tp_ipv4_baddr {
	uint32_t addr;			/* IP address this bind */
};

struct tp {
	struct tp *tp_parent;		/* pointer to parent or self */
	volatile long tp_rflags;	/* rd queue flags */
	volatile long tp_wflags;	/* wr queue flags */
	queue_t *rq;			/* associated read queue */
	queue_t *wq;			/* associated write queue */
	t_scalar_t oldstate;		/* tpi state history */
	struct tp_profile p;		/* protocol profile */
	cred_t cred;			/* credentials */
	ushort port;			/* port/protocol number */
	struct tsapaddr src;		/* bound address */
	struct tsapaddr dst;		/* connected address */
	struct snpaaddr loc;		/* local SNPA address */
	struct snpaaddr rem;		/* remote SNPA address */
	unsigned short lref;		/* local reference */
	unsigned short rref;		/* remote reference */
	struct tp_options options;	/* protocol options */
	unsigned char _pad[40];		/* pad for options */
	bufq_t conq;			/* connection queue */
	uint conind;			/* number of connection indications */
	t_uscalar_t seq;		/* previous sequence number */
	struct tp *bnext;		/* bind hash linkage */
	struct tp **bprev;		/* bind hash linkage */
	struct tp_bhash_bucket *bindb;	/* bind hash bucket */
	struct tp *cnext;		/* conn hash linkage */
	struct tp **cprev;		/* conn hash linkage */
	struct tp_chash_bucket *connb;	/* conn hash bucket */
	struct tp *lnext;		/* list hash linkage */
	struct tp **lprev;		/* list hash linkage */
	struct tp_lhash_bucket *listb;	/* list hash bucket */
};

struct tp_conind {
	/* first portion must be identical to struct tp */
	struct tp *ci_parent;		/* pointer to parent */
	volatile long ci_rflags;	/* rd queue flags */
	volatile long ci_wflags;	/* wr queue flags */
	t_uscalar_t ci_seq;		/* sequence number */
};

#define PRIV(__q) ((struct tp *)((__q)->q_ptr)

#define xti_default_debug		{ 0, }
#define xti_default_linger		(struct t_linger){T_YES, 120}
#define xti_default_rcvbuf		SK_RMEM_MAX
#define xti_default_rcvlowat		1
#define xti_default_sndbuf		SK_WMEM_MAX
#define xti_default_sndlowat		1

#define t_tco_default_throughput	{{{T_UNSPEC,T_UNSPEC},{T_UNSPEC,T_UNSPEC}},{{T_UNSPEC,T_UNSPEC},{T_UNSPEC,T_UNSPEC}}}
#define t_tco_default_transdel		{{{T_UNSPEC,T_UNSPEC},{T_UNSPEC,T_UNSPEC}},{{T_UNSPEC,T_UNSPEC},{T_UNSPEC,T_UNSPEC}}}
#define t_tco_default_reserrorrate	{T_UNSPEC,T_UNSPEC}
#define t_tco_default_transffailprob	{T_UNSPEC,T_UNSPEC}
#define t_tco_default_estfailprob	{T_UNSPEC,T_UNSPEC}
#define t_tco_default_relfailprob	{T_UNSPEC,T_UNSPEC}
#define t_tco_default_estdelay		{T_UNSPEC,T_UNSPEC}
#define t_tco_default_reldelay		{T_UNSPEC,T_UNSPEC}
#define t_tco_default_connresil		{T_UNSPEC,T_UNSPEC}
#define t_tco_default_protection	T_NOPROTECT
#define t_tco_default_priority		T_PRIDFLT
#define t_tco_default_expd		T_NO

#define t_tco_default_ltpdu		T_UNSPEC
#define t_tco_default_acktime		T_UNSPEC
#define t_tco_default_reastime		T_UNSPEC
#define t_tco_default_prefclass		T_CLASS4
#define t_tco_default_altclass1		T_UNSPEC
#define t_tco_default_altclass2		T_UNSPEC
#define t_tco_default_altclass3		T_UNSPEC
#define t_tco_default_altclass4		T_UNSPEC
#define t_tco_default_extform		T_NO
#define t_tco_default_flowctrl		T_NO
#define t_tco_default_checksum		T_NO
#define t_tco_default_netexp		T_NO
#define t_tco_default_netrecptcf	T_NO

#define t_tcl_default_transdel		{{{T_UNSPEC,T_UNSPEC},{T_UNSPEC,T_UNSPEC}},{{T_UNSPEC,T_UNSPEC},{T_UNSPEC,T_UNSPEC}}}
#define t_tcl_default_reserrorrate	{T_UNSPEC,T_UNSPEC}
#define t_tcl_default_protection	T_NOPROTECT
#define t_tcl_default_priority		T_PRIDFLT
#define t_tcl_default_checksum		T_NO

#define ip_default_options		{ 0, }
#define ip_default_tos			0
#define ip_default_ttl			64
#define ip_default_reuseaddr		T_NO
#define ip_default_dontroute		T_NO
#define ip_default_broadcast		T_NO

#define udp_default_checksum		T_YES

enum {
	_T_BIT_XTI_DEBUG = 0,
	_T_BIT_XTI_LINGER,
	_T_BIT_XTI_RCVBUF,
	_T_BIT_XTI_RCVLOWAT,
	_T_BIT_XTI_SNDBUF,
	_T_BIT_XTI_SNDLOWAT,

	_T_BIT_TCO_THROUGHPUT,
	_T_BIT_TCO_TRANSDEL,
	_T_BIT_TCO_RESERRORRATE,
	_T_BIT_TCO_TRANSFFAILPROB,
	_T_BIT_TCO_ESTFAILPROB,
	_T_BIT_TCO_RELFAILPROB,
	_T_BIT_TCO_ESTDELAY,
	_T_BIT_TCO_RELDELAY,
	_T_BIT_TCO_CONNRESIL,
	_T_BIT_TCO_PROTECTION,
	_T_BIT_TCO_PRIORITY,
	_T_BIT_TCO_EXPD,

	_T_BIT_TCO_LTPDU,
	_T_BIT_TCO_ACKTIME,
	_T_BIT_TCO_REASTIME,
	_T_BIT_TCO_PREFCLASS,
	_T_BIT_TCO_ALTCLASS1,
	_T_BIT_TCO_ALTCLASS2,
	_T_BIT_TCO_ALTCLASS3,
	_T_BIT_TCO_ALTCLASS4,
	_T_BIT_TCO_EXTFORM,
	_T_BIT_TCO_FLOWCTRL,
	_T_BIT_TCO_CHECKSUM,
	_T_BIT_TCO_NETEXP,
	_T_BIT_TCO_NETRECPTCF,
	_T_BIT_TCO_SELECTACK,
	_T_BIT_TCO_REQUESTACK,
	_T_BIT_TCO_NBLKEXPDATA,

	_T_BIT_TCL_TRANSDEL,
	_T_BIT_TCL_RESERRORRATE,
	_T_BIT_TCL_PROTECTION,
	_T_BIT_TCL_PRIORITY,
	_T_BIT_TCL_CHECKSUM,

	_T_BIT_IP_OPTIONS,
	_T_BIT_IP_TOS,
	_T_BIT_IP_TTL,
	_T_BIT_IP_REUSEADDR,
	_T_BIT_IP_DONTROUTE,
	_T_BIT_IP_BROADCAST,
	_T_BIT_IP_ADDR,
	_T_BIT_IP_RETOPTS,

	_T_BIT_UDP_CHECKSUM,
	_T_BIT_LAST,
};

#define _T_FLAG_XTI_DEBUG		(1<<_T_BIT_XTI_DEBUG)
#define _T_FLAG_XTI_LINGER		(1<<_T_BIT_XTI_LINGER)
#define _T_FLAG_XTI_RCVBUF		(1<<_T_BIT_XTI_RCVBUF)
#define _T_FLAG_XTI_RCVLOWAT		(1<<_T_BIT_XTI_RCVLOWAT)
#define _T_FLAG_XTI_SNDBUF		(1<<_T_BIT_XTI_SNDBUF)
#define _T_FLAG_XTI_SNDLOWAT		(1<<_T_BIT_XTI_SNDLOWAT)

#define _T_MASK_XTI			(_T_FLAG_XTI_DEBUG \
					|_T_FLAG_XTI_LINGER \
					|_T_FLAG_XTI_RCVBUF \
					|_T_FLAG_XTI_RCVLOWAT \
					|_T_FLAG_XTI_SNDBUF \
					|_T_FLAG_XTI_SNDLOWAT)

#define _T_FLAG_TCO_THROUGHPUT		(1<<_T_BIT_TCO_THROUGHPUT)
#define _T_FLAG_TCO_TRANSDEL		(1<<_T_BIT_TCO_TRANSDEL)
#define _T_FLAG_TCO_RESERRORRATE	(1<<_T_BIT_TCO_RESERRORRATE)
#define _T_FLAG_TCO_TRANSFFAILPROB	(1<<_T_BIT_TCO_TRANSFFAILPROB)
#define _T_FLAG_TCO_ESTFAILPROB		(1<<_T_BIT_TCO_ESTFAILPROB)
#define _T_FLAG_TCO_RELFAILPROB		(1<<_T_BIT_TCO_RELFAILPROB)
#define _T_FLAG_TCO_ESTDELAY		(1<<_T_BIT_TCO_ESTDELAY)
#define _T_FLAG_TCO_RELDELAY		(1<<_T_BIT_TCO_RELDELAY)
#define _T_FLAG_TCO_CONNRESIL		(1<<_T_BIT_TCO_CONNRESIL)
#define _T_FLAG_TCO_PROTECTION		(1<<_T_BIT_TCO_PROTECTION)
#define _T_FLAG_TCO_PRIORITY		(1<<_T_BIT_TCO_PRIORITY)
#define _T_FLAG_TCO_EXPD		(1<<_T_BIT_TCO_EXPD)

#define _T_MASK_TCO_QOS			(_T_FLAG_TCO_THROUGHPUT \
					|_T_FLAG_TCO_TRANSDEL \
					|_T_FLAG_TCO_RESERRORRATE \
					|_T_FLAG_TCO_TRANSFFAILPROB \
					|_T_FLAG_TCO_ESTFAILPROB \
					|_T_FLAG_TCO_RELFAILPROB \
					|_T_FLAG_TCO_ESTDELAY \
					|_T_FLAG_TCO_RELDELAY \
					|_T_FLAG_TCO_CONNRESIL \
					|_T_FLAG_TCO_PROTECTION \
					|_T_FLAG_TCO_PRIORITY \
					|_T_FLAG_TCO_EXPD)

#define _T_FLAG_TCO_LTPDU		(1<<_T_BIT_TCO_LTPDU)
#define _T_FLAG_TCO_ACKTIME		(1<<_T_BIT_TCO_ACKTIME)
#define _T_FLAG_TCO_REASTIME		(1<<_T_BIT_TCO_REASTIME)
#define _T_FLAG_TCO_EXTFORM		(1<<_T_BIT_TCO_EXTFORM)
#define _T_FLAG_TCO_FLOWCTRL		(1<<_T_BIT_TCO_FLOWCTRL)
#define _T_FLAG_TCO_CHECKSUM		(1<<_T_BIT_TCO_CHECKSUM)
#define _T_FLAG_TCO_NETEXP		(1<<_T_BIT_TCO_NETEXP)
#define _T_FLAG_TCO_NETRECPTCF		(1<<_T_BIT_TCO_NETRECPTCF)
#define _T_FLAG_TCO_PREFCLASS		(1<<_T_BIT_TCO_PREFCLASS)
#define _T_FLAG_TCO_ALTCLASS1		(1<<_T_BIT_TCO_ALTCLASS1)
#define _T_FLAG_TCO_ALTCLASS2		(1<<_T_BIT_TCO_ALTCLASS2)
#define _T_FLAG_TCO_ALTCLASS3		(1<<_T_BIT_TCO_ALTCLASS3)
#define _T_FLAG_TCO_ALTCLASS4		(1<<_T_BIT_TCO_ALTCLASS4)
#define _T_FLAG_TCO_SELECTACK		(1<<_T_BIT_TCO_SELECTACK)
#define _T_FLAG_TCO_REQUESTACK		(1<<_T_BIT_TCO_REQUESTACK)
#define _T_FLAG_TCO_NBLKEXPDATA		(1<<_T_BIT_TCO_NBLKEXPDATA)

#define _T_MASK_TCO_MGMT		(_T_FLAG_TCO_LTPDU \
					|_T_FLAG_TCO_ACKTIME \
					|_T_FLAG_TCO_REASTIME \
					|_T_FLAG_TCO_EXTFORM \
					|_T_FLAG_TCO_FLOWCTRL \
					|_T_FLAG_TCO_CHECKSUM \
					|_T_FLAG_TCO_NETEXP \
					|_T_FLAG_TCO_NETRECPTCF \
					|_T_FLAG_TCO_PREFCLASS \
					|_T_FLAG_TCO_ALTCLASS1 \
					|_T_FLAG_TCO_ALTCLASS2 \
					|_T_FLAG_TCO_ALTCLASS3 \
					|_T_FLAG_TCO_ALTCLASS4 \
					|_T_FLAG_TCO_SELECTACK \
					|_T_FLAG_TCO_REQUESTACK \
					|_T_FLAG_TCO_NBLKEXPDATA)

#define _T_MASK_TCO			(_T_MASK_TCO_QOS|_T_MASK_TCO_MGMT)

#define _T_FLAG_TCL_TRANSDEL		(1<<_T_BIT_TCL_TRANSDEL)
#define _T_FLAG_TCL_RESERRORRATE	(1<<_T_BIT_TCL_RESERRORRATE)
#define _T_FLAG_TCL_PROTECTION		(1<<_T_BIT_TCL_PROTECTION)
#define _T_FLAG_TCL_PRIORITY		(1<<_T_BIT_TCL_PRIORITY)
#define _T_FLAG_TCL_CHECKSUM		(1<<_T_BIT_TCL_CHECKSUM)

#define _T_MASK_TCL			(_T_FLAG_TCL_TRANSDEL \
					|_T_FLAG_TCL_RESERRORRATE \
					|_T_FLAG_TCL_PROTECTION \
					|_T_FLAG_TCL_PRIORITY \
					|_T_FLAG_TCL_CHECKSUM)

static const t_uscalar_t tp_space[_T_BIT_LAST] = {
	/* *INDENT-OFF* */
	[_T_BIT_XTI_DEBUG		] = T_SPACE(4 * sizeof(t_uscalar_t)),
	[_T_BIT_XTI_LINGER		] = _T_SPACE_SIZEOF(struct t_linger),
	[_T_BIT_XTI_RCVBUF		] = _T_SPACE_SIZEOF(t_uscalar_t),
	[_T_BIT_XTI_RCVLOWAT		] = _T_SPACE_SIZEOF(t_uscalar_t),
	[_T_BIT_XTI_SNDBUF		] = _T_SPACE_SIZEOF(t_uscalar_t),
	[_T_BIT_XTI_SNDLOWAT		] = _T_SPACE_SIZEOF(t_uscalar_t),

	[_T_BIT_TCO_THROUGHPUT		] = _T_SPACE_SIZEOF(struct thrpt),
	[_T_BIT_TCO_TRANSDEL		] = _T_SPACE_SIZEOF(struct transdel),
	[_T_BIT_TCO_RESERRORRATE	] = _T_SPACE_SIZEOF(struct rate),
	[_T_BIT_TCO_TRANSFFAILPROB	] = _T_SPACE_SIZEOF(struct rate),
	[_T_BIT_TCO_ESTFAILPROB		] = _T_SPACE_SIZEOF(struct rate),
	[_T_BIT_TCO_RELFAILPROB		] = _T_SPACE_SIZEOF(struct rate),
	[_T_BIT_TCO_ESTDELAY		] = _T_SPACE_SIZEOF(struct rate),
	[_T_BIT_TCO_RELDELAY		] = _T_SPACE_SIZEOF(struct rate),
	[_T_BIT_TCO_CONNRESIL		] = _T_SPACE_SIZEOF(struct rate),
	[_T_BIT_TCO_PROTECTION		] = _T_SPACE_SIZEOF(t_uscalar_t),
	[_T_BIT_TCO_PRIORITY		] = _T_SPACE_SIZEOF(t_uscalar_t),
	[_T_BIT_TCO_EXPD		] = _T_SPACE_SIZEOF(t_uscalar_t),

	[_T_BIT_TCO_LTPDU		] = _T_SPACE_SIZEOF(t_uscalar_t),
	[_T_BIT_TCO_ACKTIME		] = _T_SPACE_SIZEOF(t_uscalar_t),
	[_T_BIT_TCO_REASTIME		] = _T_SPACE_SIZEOF(t_uscalar_t),
	[_T_BIT_TCO_PREFCLASS		] = _T_SPACE_SIZEOF(t_uscalar_t),
	[_T_BIT_TCO_ALTCLASS1		] = _T_SPACE_SIZEOF(t_uscalar_t),
	[_T_BIT_TCO_ALTCLASS2		] = _T_SPACE_SIZEOF(t_uscalar_t),
	[_T_BIT_TCO_ALTCLASS3		] = _T_SPACE_SIZEOF(t_uscalar_t),
	[_T_BIT_TCO_ALTCLASS4		] = _T_SPACE_SIZEOF(t_uscalar_t),
	[_T_BIT_TCO_EXTFORM		] = _T_SPACE_SIZEOF(t_uscalar_t),
	[_T_BIT_TCO_FLOWCTRL		] = _T_SPACE_SIZEOF(t_uscalar_t),
	[_T_BIT_TCO_CHECKSUM		] = _T_SPACE_SIZEOF(t_uscalar_t),
	[_T_BIT_TCO_NETEXP		] = _T_SPACE_SIZEOF(t_uscalar_t),
	[_T_BIT_TCO_NETRECPTCF		] = _T_SPACE_SIZEOF(t_uscalar_t),
	[_T_BIT_TCO_SELECTACK		] = _T_SPACE_SIZEOF(t_uscalar_t),

	[_T_BIT_TCL_TRANSDEL		] = _T_SPACE_SIZEOF(struct transdel),
	[_T_BIT_TCL_RESERRORRATE	] = _T_SPACE_SIZEOF(struct rate),
	[_T_BIT_TCL_PROTECTION		] = _T_SPACE_SIZEOF(t_uscalar_t),
	[_T_BIT_TCL_PRIORITY		] = _T_SPACE_SIZEOF(t_uscalar_t),
	[_T_BIT_TCL_CHECKSUM		] = _T_SPACE_SIZEOF(t_uscalar_t),
	/* *INDENT-ON* */
};

#if	defined DEFINE_RWLOCK
static DEFINE_RWLOCK(tp_lock);
#elif	defined __RW_LOCK_UNLOCKED
static rwlock_t tp_lock = __RW_LOCK_UNLOCKED(tp_lock);	/* protects tp_opens lists */
#elif	defined RW_LOCK_UNLOCKED
static rwlock_t tp_lock = RW_LOCK_UNLOCKED;	/* protects tp_opens lists */
#else
#error cannot initialize read-write locks
#endif
static caddr_t tp_opens = NULL;

/*
 *  Bind buckets, caches and hashes.
 */
struct tp_ipv4_bind_bucket {
	struct tp_bind_bucket *next;	/* linkage of bind buckets for hash slot */
	struct tp_bind_bucket **prev;	/* linkage of bind buckets for hash slot */
	unsigned char proto;		/* IP protocol identifier or LSAP */
	unsigned short port;		/* port number (host order) */
	struct tp *owners;		/* list of owners of this protocol/port combination */
	struct tp *dflt;		/* default listeners/destinations for this protocol */
};

struct tp_conn_bucket {
	struct tp_conn_bucket *next;	/* linkage of conn buckets for hash slot */
	struct tp_conn_bucket **prev;	/* linkage of conn buckets for hash slot */
	unsigned char proto;		/* IP protocol identifier or LSAP */
	unsigned short sport;		/* source port number (network order) */
	unsigned short dport;		/* destination port number (network order) */
	struct tp *owners;		/* list of owners of this protocol/sport/dport combination */
};

struct tp_bhash_bucket {
	rwlock_t lock;
	struct tp *list;
};

struct tp_chash_bucket {
	rwlock_t lock;
	struct tp *list;
};

STATIC struct tp_bhash_bucket *tp_bhash;
STATIC size_t tp_bhash_size = 0;
STATIC size_t tp_bhash_order = 0;

STATIC struct tp_chash_bucket *tp_chash;
STATIC size_t tp_chash_size = 0;
STATIC size_t tp_chash_order = 0;

STATIC INLINE fastcall __hot_in int
tp_bhashfn(unsigned char proto, unsigned short bport)
{
	return ((tp_bhash_size - 1) & (proto + bport));
}

STATIC INLINE fastcall __unlikely int
tp_chashfn(unsigned char proto, unsigned short sport, unsigned short dport)
{
	return ((tp_chash_size - 1) & (proto + sport + dport));
}

#if	defined DEFINE_RWLOCK
STATIC DEFINE_RWLOCK(tp_hash_lock);
STATIC DEFINE_RWLOCK(tp_prot_lock);
#elif	defined __RW_LOCK_UNLOCKED
STATIC rwlock_t tp_hash_lock = __RW_LOCK_UNLOCKED(tp_hash_lock);
STATIC rwlock_t tp_prot_lock = __RW_LOCK_UNLOCKED(tp_prot_lock);
#elif	defined RW_LOCK_UNLOCKED
STATIC rwlock_t tp_hash_lock = RW_LOCK_UNLOCKED;
STATIC rwlock_t tp_prot_lock = RW_LOCK_UNLOCKED;
#else
#error cannot initialize read-write locks
#endif

#ifdef LINUX
#if defined HAVE_KTYPE_STRUCT_NET_PROTOCOL
struct inet_protocol {
	struct net_protocol proto;
	struct net_protocol *next;
	struct module *kmod;
};
#endif				/* defined HAVE_KTYPE_STRUCT_NET_PROTCCOL */
#endif				/* LINUX */

struct tp_prot_bucket {
	unsigned char proto;		/* protocol number */
	int refs;			/* reference count */
	int corefs;			/* N_CONS references */
	int clrefs;			/* N_CLNS references */
	struct inet_protocol prot;	/* Linux registration structure */
};
STATIC struct tp_prot_bucket *tp_prots[256];

STATIC kmem_cachep_t tp_bind_cachep;
STATIC kmem_cachep_t tp_prot_cachep;

/*
 *  =========================================================================
 *
 *  Private Structure cache
 *
 *  =========================================================================
 */
static kmem_cachep_t tp_priv_cachep = NULL;

/**
 * tp_init_caches: - initialize caches
 *
 * Returns zero (0) on success or a negative error code on failure.
 */
static int
tp_init_caches(void)
{
	if (!tp_priv_cachep
	    && !(tp_priv_cachep =
		 kmem_create_cache("tp_priv_cachep", mi_open_size(sizeof(struct tp)), 0, SLAB_HWCACHE_ALIGN,
				   NULL, NULL)
	    )) {
		cmn_err(CE_PANIC, "%s: Cannot allocate tp_priv_cachep", __FUNCTION__);
		return (-ENOMEM);
	} else
		cmn_err(CE_DEBUG, "%s: initialized driver private structure cache", DRV_NAME);
	return (0);
}

/**
 * tp_term_caches: - terminate caches
 *
 * Returns zero (0) on success or a negative error code on failure.
 */
static int
tp_term_caches(void)
{
	if (tp_priv_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(tp_priv_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy tp_priv_cachep", __FUNCTION__);
			return (-EBUSY);
		} else
			cmn_err(CE_DEBUG, "%s: destroyed tp_priv_cachep", DRV_NAME);
#else
		kmem_cache_destroy(tp_priv_cachep);
#endif
	}
	return (0);
}

/**
 * tp_trylock: - try to lock a Stream queue pair
 * @q: the queue pair to lock
 *
 * Because we lock connecting, listening and accepting Streams, and listening streams can be waiting
 * on accepting streams, we must ensure that the other stream cannot close and detach from its queue
 * pair at the same time that we are locking or unlocking.  Therefore, hold the master list
 * reader-writer lock while locking or unlocking.
 */
static inline fastcall struct tp *
tp_trylock(queue_t *q)
{
	struct tp *tp;

	read_lock(&tp_lock);
	tp = (struct tp *) mi_trylock(q);
	read_unlock(&tp_lock);
	return (tp);
}

/**
 * tp_unlock: - unlock a Stream queue pair
 * @q: the queue pair to unlock
 *
 * Because we lock connecting, listening and accepting Streams, and listening streams can be waiting
 * on accepting streams, we must ensure that the other stream cannot close and detach from its queue
 * pair at the same time that we are locking or unlocking.  Therefore, hold the master list
 * reader-writer lock while locking or unlocking.
 */
static inline fastcall void
tp_unlock(struct tp *tp)
{
	read_lock(&tp_lock);
	mi_unlock((caddr_t) tp);
	read_unlock(&tp_lock);
}

/*
 *  =========================================================================
 *
 *  IP Local Management
 *
 *  =========================================================================
 */
/*
 *  IP subsystem management
 */
#ifdef LINUX
/**
 * tp_v4_steal - steal a socket buffer
 * @skb: socket buffer to steal
 *
 * In the 2.4 packet handler, if the packet is for us, steal the packet by overwritting the protocol
 * and returning.  THis is only done for normal packets and not error packets (thst do not need to
 * be stolen).  In the 2.4 handler loop, iph->protocol is examined on each iteration, permitting us
 * to steal the packet by overwritting the protocol number.
 *
 * In the 2.6 packet handler, if the packet is not for us, steal the packet by simply not passing it
 * to the next handler.
 */
STATIC INLINE fastcall __hot_in void
tp_v4_steal(struct sk_buff *skb)
{
#ifdef HAVE_KFUNC_NF_RESET
	nf_reset(skb);
#endif
#ifdef HAVE_KTYPE_STRUCT_INET_PROTOCOL
	skb->nh.iph->protocol = 255;
	skb->protocol = 255;
#endif				/* HAVE_KTYPE_STRUCT_INET_PROTOCOL */
}

#if defined HAVE_KTYPE_STRUCT_NET_PROTOCOL
#define mynet_protocol net_protocol
#endif				/* defined HAVE_KTYPE_STRUCT_NET_PROTOCOL */
#if defined HAVE_KTYPE_STRUCT_INET_PROTOCOL
#define mynet_protocol inet_protocol
#endif				/* defined HAVE_KTYPE_STRUCT_INET_PROTOCOL */

struct ipnet_protocol {
	struct mynet_protocol *proto;
	struct mynet_protocol *next;
	struct module *kmod;
};

struct ipnet_protocols {
	struct ipnet_protocol tp4;
	struct ipnet_protocol iso;
	struct ipnet_protocol udp;
};

STATIC struct ipnet_protocols tp_protos;

/**
 * tp_take_protocol: - initialize network protocol override
 * @pp: our static protocol structure
 * @proto: the protocol to register or override
 *
 * This is the network protocol override function.
 *
 * This is complicated because we hack the inet protocol tables.  If no other protocol was
 * previously registered, this reduces to inet_add_protocol().  If there is a protocol previously
 * registered, we take a reference on the kernel module owning the entry, if possible, and replace
 * the entry with our own, saving a pointer to the previous entry for passing sk_buffs along that we
 * are not interested in.  Taking a module reference is particularly for things like SCTP, where
 * unloading the module after protocol override would otherwise break things horribly.  Takeing the
 * reference keeps the module from unloading (this works for OpenSS7 SCTP as well as lksctp).
 */
STATIC __unlikely int
tp_take_protocol(struct ipnet_protocol *pp, unsigned char proto)
{
	struct mynet_protocol **ppp;
	int hash = proto & (MAX_INET_PROTOS - 1);

	ppp = &inet_protosp[hash];
	{
		net_protocol_lock();
#ifdef HAVE_OLD_STYLE_INET_PROTOCOL
		while (*ppp && (*ppp)->protocol != proto)
			ppp = &(*ppp)->next;
#endif				/* HAVE_OLD_STYLE_INET_PROTOCOL */
		if (*ppp != NULL) {
#ifdef HAVE_KMEMB_STRUCT_INET_PROTOCOL_COPY
			/* can only override last entry */
			if ((*ppp)->copy != 0) {
				__ptrace(("Cannot override copy entry\n"));
				net_protocol_unlock();
				return (-EBUSY);
			}
#endif				/* HAVE_KMEMB_STRUCT_INET_PROTOCOL_COPY */
			if ((pp->kmod = streams_module_address((ulong) *ppp))
			    && pp->kmod != THIS_MODULE) {
				if (!try_module_get(pp->kmod)) {
					__ptrace(("Cannot acquire module\n"));
					net_protocol_unlock();
					return (-EDEADLK);
				}
			}
#if defined HAVE_KMEMB_STRUCT_NET_PROTOCOL_NEXT || defined HAVE_KMEMB_STRUCT_INET_PROTOCOL_NEXT
			pp->proto->next = (*ppp)->next;
#endif
		}
		pp->next = xchg(ppp, pp->proto);
		net_protocol_unlock();
	}
	return (0);
}

/**
 * tp_give_protocol: - terminate network protocol override
 * @pp: our static protocol structure
 * @proto: network protocol to terminate
 *
 * This is the network protocol restoration function.
 *
 * This is complicated and brittle.  The module stuff here is just for ourselves (other kernel
 * modules pulling the same trick) as Linux IP protocols are normally kernel resident.  If a
 * protocol was previously registered, restore the protocol's entry and drop the reference to its
 * owning kernel module.  If there was no protocol previously registered, this reduces to
 * inet_del_protocol().
 */
STATIC __unlikely void
tp_give_protocol(struct ipnet_protocol *pp, unsigned char proto)
{
	struct mynet_protocol **ppp;
	int hash = proto & (MAX_INET_PROTOS - 1);

	ppp = &inet_protosp[hash];
	{
		net_protocol_lock();
#ifdef HAVE_OLD_STYLE_INET_PROTOCOL
		while (*ppp && *ppp != pp->proto)
			ppp = &(*ppp)->next;
		if (pp->next)
			pp->next->next = pp->proto->next;
#endif				/* HAVE_OLD_STYLE_INET_PROTOCOL */
		__assert(*ppp == pp->proto);
		*ppp = pp->next;
		net_protocol_unlock();
	}
	if (pp->next != NULL && pp->kmod != NULL && pp->kmod != THIS_MODULE)
		module_put(pp->kmod);
	return;
}

/**
 * tp_v4_rcv_next: - pass a socket buffer to the next handler
 * @next: next protocol structure
 * @skb: the socket buffer
 */
STATIC INLINE fastcall __hot_in int
tp_v4_rcv_next(struct mynet_protocol *next, struct sk_buff *skb)
{
	if (next != NULL) {
		next->handler(skb);
		return (1);
	}
	kfree_skb(skb);
	return (0);
}

/**
 * tp_v4_err_next: - pass a socket buffer to the next error handler
 * @next: next protocol structure
 * @skb: the socket buffer
 * @info: ICMP information
 */
STATIC INLINE fastcall __hot_in void
tp_v4_err_next(struct mynet_protocol *next, struct sk_buff *skb, __u32 info)
{
	if (next != NULL)
		next->err_handler(skb, info);
	return;
}

/**
 * tp_init_nproto - initialize network protocol override
 * @proto: the protocol to register or override
 *
 * This is the network protocol override function.
 *
 * This is complicated because we hack the inet protocol tables.  If no other protocol was
 * previously registered, this reduces to inet_add_protocol().  If there is a protocol previously
 * registered, we take a reference on the kernel module owning the entry, if possible, and replace
 * the entry with our own, saving a pointer to the previous entry for passing sk_bufs along that we
 * are not interested in.  Taking a module reference is particularly for things like SCTP, where
 * unloading the module after protocol override would break things horribly.  Taking the reference
 * keeps the module from unloading (this works for OpenSS7 SCTP as well as lksctp).
 */
STATIC INLINE fastcall __unlikely struct tp_prot_bucket *
tp_init_nproto(unsigned char proto, unsigned int type)
{
#if 0
	struct tp_prot_bucket *pb;
	struct ipnet_protocol *pp;
	struct mynet_protocol **ppp;
	int hash = proto & (MAX_INET_PROTOS - 1);

	write_lock_bh(&tp_prot_lock);
	if ((pb = tp_prots[proto]) != NULL) {
		pb->refs++;
		switch (type) {
		case T_COTS:
		case T_COTS_ORD:
			++pb->corefs;
			break;
		case T_CLTS:
			++pb->clrefs;
			break;
		default:
			swerr();
			break;
		}
	} else if ((pb = kmem_cache_alloc(tp_udp_prot_cachep, GFP_ATOMIC))) {
		bzero(pb, sizeof(*pb));
		pb->refs = 1;
		switch (type) {
		case T_COTS:
		case T_COTS_ORD:
			pb->corefs = 1;
			break;
		case T_CLTS:
			pb->clrefs = 1;
			break;
		default:
			swerr();
			break;
		}
		pp = &pb->prot;
#ifdef HAVE_KMEMB_STRUCT_INET_PROTOCOL_PROTOCOL
		pp->proto.protocol = proto;
		pp->proto.name = "streams-udp";
#endif
#if defined HAVE_KTYPE_STRUCT_NET_PROTOCOL_PROTO
		pp->proto.proto = proto;
#endif				/* defined HAVE_KTYPE_STRUCT_NET_PROTOCOL_PROTO */
#if defined HAVE_KMEMB_STRUCT_NET_PROTOCOL_NO_POLICY || defined HAVE_KMEMB_STRUCT_INET_PROTOCOL_NO_POLICY
		pp->proto.no_policy = 1;
#endif
		pp->proto.handler = &tp_v4_rcv;
		pp->proto.err_handler = &tp_v4_err;
		ppp = &inet_protosp[hash];

		{
			net_protocol_lock();
#ifdef HAVE_OLD_STYLE_INET_PROTOCOL
			while (*ppp && (*ppp)->protocol != proto)
				ppp = &(*ppp)->next;
#endif				/* HAVE_OLD_STYLE_INET_PROTOCOL */
			if (*ppp != NULL) {
#ifdef HAVE_KMEMB_STRUCT_INET_PROTOCOL_COPY
				/* can only override last entry */
				if ((*ppp)->copy != 0) {
					__ptrace(("Cannot override copy entry\n"));
					net_protocol_unlock();
					write_unlock_bh(&tp_prot_lock);
					kmem_cache_free(tp_udp_prot_cachep, pb);
					return (NULL);
				}
#endif				/* HAVE_KMEMB_STRUCT_INET_PROTOCOL_COPY */
				if ((pp->kmod = streams_module_address((ulong) *ppp))
				    && pp->kmod != THIS_MODULE) {
					if (!try_module_get(pp->kmod)) {
						__ptrace(("Cannot acquire module\n"));
						net_protocol_unlock();
						write_unlock_bh(&tp_prot_lock);
						kmem_cache_free(tp_udp_prot_cachep, pb);
						return (NULL);
					}
				}
#if defined HAVE_KMEMB_STRUCT_NET_PROTOCOL_NEXT || defined HAVE_KMEMB_STRUCT_INET_PROTOCOL_NEXT
				pp->proto.next = (*ppp)->next;
#endif
			}
			pp->next = xchg(ppp, &pp->proto);
			net_protocol_unlock();
		}
		/* link into hash slot */
		tp_prots[proto] = pb;
	}
	write_unlock_bh(&tp_prot_lock);
	return (pb);
#endif
	return (NULL);
}

/**
 * tp_term_nproto - terminate network protocol override
 * @proto: network protocol to terminate
 *
 * This is the network protocol restoration function.
 *
 * This is complicated and brittle.  The module stuff here is just for ourselves (other kernel
 * modules pulling the same trick) as Linux IP protocols are normally kernel resident.  If a
 * protocol was previously registered, restore the protocol's entry and drop the reference to its
 * owning kernel module.  If there was no protocol previously registered, this reduces to
 * inet_del_protocol().
 */
STATIC INLINE fastcall __unlikely void
tp_term_nproto(unsigned char proto, unsigned int type)
{
#if 0
	struct tp_prot_bucket *pb;

	write_lock_bh(&tp_prot_lock);
	if ((pb = tp_prots[proto]) != NULL) {
		switch (type) {
		case T_COTS:
		case T_COTS_ORD:
			assure(pb->corefs > 0);
			--pb->corefs;
			break;
		case T_CLTS:
			assure(pb->clrefs > 0);
			--pb->clrefs;
			break;
		default:
			swerr();
			break;
		}
		if (--pb->refs == 0) {
			struct ipnet_protocol *pp = &pb->prot;
			struct mynet_protocol **ppp;
			int hash = proto & (MAX_INET_PROTOS - 1);

			ppp = &inet_protosp[hash];
			{
				net_protocol_lock();
#ifdef HAVE_OLD_STYLE_INET_PROTOCOL
				while (*ppp && *ppp != &pp->proto)
					ppp = &(*ppp)->next;
				if (pp->next)
					pp->next->next = pp->proto.next;
#endif				/* HAVE_OLD_STYLE_INET_PROTOCOL */
				__assert(*ppp == &pp->proto);
				*ppp = pp->next;
				net_protocol_unlock();
			}
			if (pp->next != NULL && pp->kmod != NULL && pp->kmod != THIS_MODULE)
				module_put(pp->kmod);
			/* unlink from hash slot */
			tp_prots[proto] = NULL;

			kmem_cache_free(tp_udp_prot_cachep, pb);
		}
	}
	write_unlock_bh(&tp_prot_lock);
#endif
}
#endif				/* LINUX */

/**
 *  tp_bind_prot -  bind a protocol
 *  @proto:	    protocol number to bind
 *
 *  NOTICES: Notes about registration.  Older 2.4 kernels will allow you to register whatever inet
 *  protocols you want on top of any existing protocol.  This is good.  2.6 kernels, on the other
 *  hand, do not allow registration of inet protocols over existing inet protocols.  We rip symbols
 *  on 2.6 and put special code in the handler to give us effectively the old 2.4 approach.
 *  This is also detectable by the fact that inet_add_protocol() returns void on 2.4 and int on 2.6.
 *
 *  Issues with the 2.4 approach to registration is that the ip_input function passes a cloned skb
 *  to each protocol registered.  We don't want to do that.  If the message is for us, we want to
 *  process it without passing it to others.
 *
 *  Issues with the 2.6 approach to registration is that the ip_input function passes the skb to
 *  only one function.  We don't want that either.  If the message is not for us, we want to pass it
 *  to the next protocol module.
 */
STATIC INLINE fastcall int
tp_bind_prot(unsigned char proto, unsigned int type)
{
	struct tp_prot_bucket *pb;

	if ((pb = tp_init_nproto(proto, type)))
		return (0);
	return (-ENOMEM);
}

/**
 *  tp_unbind_prot - unbind a protocol
 *  @proto:	    protocol number to unbind
 */
STATIC INLINE fastcall void
tp_unbind_prot(unsigned char proto, unsigned int type)
{
	tp_term_nproto(proto, type);
}


/*
 *  =========================================================================
 *
 *  STATE Changes
 *
 *  =========================================================================
 */

#if !defined _OPTIMIZE_SPEED
/**
 * tpi_statename: - name TPI state
 * @state: state to name
 * Returns the name of the state or "(unknown)".
 */
static const char *
tpi_statename(t_scalar_t state)
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

/**
 * tpi_primname: - name TPI primitive
 * @prim: the primitive to name
 * Returns the name of the primitive or "(unknown)".
 */
static const char *
tpi_primname(t_scalar_t prim)
{
	switch (prim) {
	case T_CONN_REQ:
		return ("T_CONN_REQ");
	case T_CONN_RES:
		return ("T_CONN_RES");
	case T_DISCON_REQ:
		return ("T_DISCON_REQ");
	case T_DATA_REQ:
		return ("T_DATA_REQ");
	case T_EXDATA_REQ:
		return ("T_EXDATA_REQ");
	case T_INFO_REQ:
		return ("T_INFO_REQ");
	case T_BIND_REQ:
		return ("T_BIND_REQ");
	case T_UNBIND_REQ:
		return ("T_UNBIND_REQ");
	case T_UNITDATA_REQ:
		return ("T_UNITDATA_REQ");
	case T_OPTMGMT_REQ:
		return ("T_OPTMGMT_REQ");
	case T_ORDREL_REQ:
		return ("T_ORDREL_REQ");
	case T_OPTDATA_REQ:
		return ("T_OPTDATA_REQ");
	case T_ADDR_REQ:
		return ("T_ADDR_REQ");
	case T_CAPABILITY_REQ:
		return ("T_CAPABILITY_REQ");
	case T_CONN_IND:
		return ("T_CONN_IND");
	case T_CONN_CON:
		return ("T_CONN_CON");
	case T_DISCON_IND:
		return ("T_DISCON_IND");
	case T_DATA_IND:
		return ("T_DATA_IND");
	case T_EXDATA_IND:
		return ("T_EXDATA_IND");
	case T_INFO_ACK:
		return ("T_INFO_ACK");
	case T_BIND_ACK:
		return ("T_BIND_ACK");
	case T_ERROR_ACK:
		return ("T_ERROR_ACK");
	case T_OK_ACK:
		return ("T_OK_ACK");
	case T_UNITDATA_IND:
		return ("T_UNITDATA_IND");
	case T_UDERROR_IND:
		return ("T_UDERROR_IND");
	case T_OPTMGMT_ACK:
		return ("T_OPTMGMT_ACK");
	case T_ORDREL_IND:
		return ("T_ORDREL_IND");
	case T_OPTDATA_IND:
		return ("T_OPTDATA_IND");
	case T_ADDR_ACK:
		return ("T_ADDR_ACK");
	case T_CAPABILITY_ACK:
		return ("T_CAPABILITY_ACK");
	default:
		return ("????");
	}
}
#endif				/* !defined _OPTIMIZE_SPEED */

/**
 * tp_set_state: - set TPI state of private structure
 * @tp: private structure (locked) for which to set state
 * @state: the state to set
 */
STATIC INLINE fastcall __hot void
tp_set_state(struct tp *tp, t_scalar_t state)
{
	LOGST(tp, "%s <- %s", tpi_statename(state), tpi_statename(tp->p.info.CURRENT_state));
	tp->p.info.CURRENT_state = state;
}

/**
 * tp_get_state: - get TPI state of private strcutrue
 * @tp: private structure (locked) for which to get state
 * @state: the state to get
 */
STATIC INLINE fastcall __hot t_scalar_t
tp_get_state(struct tp *tp)
{
	return (tp->p.info.CURRENT_state);
}

STATIC INLINE fastcall t_scalar_t
tp_chk_state(struct tp *tp, t_scalar_t mask)
{
	return (((1 << tp_get_state(tp)) & (mask)) != 0);
}

STATIC INLINE fastcall t_scalar_t
tp_not_state(struct tp *tp, t_scalar_t mask)
{
	return (((1 << tp_get_state(tp)) & (mask)) == 0);
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
 *  M_PROTO, M_PCPROTO Handling
 *
 *  -------------------------------------------------------------------------
 */
/**
 * tp_w_proto_return: - handle M_PROTO and M_PCPROTO processing return value
 * @mp: the M_PROTO or M_PCPROTO message
 * @rtn: the return value
 *
 * Some M_PROTO and M_PCPROTO processing functions return an error just to cause the primary state
 * of the endpoint to be restored.  These return values are processed here.
 */
noinline fastcall int
tp_w_proto_return(mblk_t *mp, int rtn)
{
	switch (-rtn) {
	case EBUSY:
	case EAGAIN:
	case ENOMEM:
	case ENOBUFS:
	case EDEADLK:
		return (rtn);
	default:
		freemsg(mp);
	case 0:
		return (0);
	}
}

/**
 * tp_w_prim_put: - put a TPI primitive
 * @q: active queue (write queue)
 * @mp: the primitive
 *
 * This is the fast path for the TPI write put procedure.  Data is simply queued by returning
 * -%EAGAIN.  This provides better flow control and scheduling for maximum throughput.
 */
static inline streamscall __hot_put int
tp_w_prim_put(queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_PROTO)) {
	case M_PROTO:
	case M_PCPROTO:
		break;
		// return tp_w_proto(q, mp);
	case M_DATA:
		break;
		// return tp_w_data(q, mp);
	case M_FLUSH:
		break;
		// return tp_w_flush(q, mp);
	case M_IOCTL:
	case M_IOCDATA:
		break;
		// return tp_w_ioctl(q, mp);
	}
	// LOGERR(PRIV(q), "SWERR: %s %s: %d", __FUNCTION__, __FILE__, __LINE__);
	freemsg(mp);
	return (0);
}

/**
 * tp_w_prim_srv: = service a TPI primitive
 * @tp: private structure (locked)
 * @q: active queue (write queue)
 * @mp: the primitive
 *
 * This is the fast path for the TPI write service procedure.  Data is processed.  Processing data
 * from the write service procedure provies better flow control and scheduling for maximum
 * throughput and minimum latency.
 */
static inline streamscall __hot_out int
tp_w_prim_srv(struct tp *tp, queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_PROTO)) {
	case M_PROTO:
	case M_PCPROTO:
		break;
		// return __tp_w_proto(tp, q, mp);
	case M_DATA:
		break;
		// return __tp_w_data(tp, q, mp);
	case M_FLUSH:
		break;
		// return tp_w_flush(q, mp);
	case M_IOCTL:
	case M_IOCDATA:
		break;
		// return tp_w_ioctl(q, mp);
	}
	LOGERR(tp, "SWERR: %s %s: %d", __FUNCTION__, __FILE__, __LINE__);
	freemsg(mp);
	return (0);
}

/*
 *  READ SIDE wakeup procedure (called on each service procedure exit)
 *  -------------------------------------------------------------------------
 */

/*
 *  =========================================================================
 *
 *  QUEUE PUT and SERVICE routines
 *
 *  =========================================================================
 */

/**
 * tp_rput: - read put procedure
 * @q: active queue (read queue)
 * @mp: message to put
 *
 * This is a canonical put procedure for the read queue.  Messages placed on the read
 * queue either come from this module internally (e.g. timers), the IP or LLC
 * sublayer or when TPSN is pushed as a module from the driver.  Never call put()
 * from soft interrupt.  In tp_v4_rcv() and tp_v4_err() messages are placed directly
 * on the queue with putq().
 */
static streamscall __hot_in int
tp_rput(queue_t *q, mblk_t *mp)
{
	if (unlikely(!putq(q, mp))) {
		mp->b_band = 0;
		putq(q, mp);	/* must succeed */
	}
	return (0);
}

/**
 * tp_r_prim_srv: - read primitive handling
 * @tp: private structure (locked)
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static inline streamscall __hot_in int
tp_r_prim_srv(struct tp *tp, queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_DATA)) {
	case M_DATA:
		break;
		// return __tp_recv_msg(tp, q, mp);
	case M_SIG:
	case M_PCSIG:
		break;
		// return __tp_r_sig(tp, q, mp);
	case M_CTL:
		break;
		// return __tp_recv_err(tp, q, mp);
	case M_FLUSH:
		break;
		// return tp_r_flush(q, mp);
	default:
		break;
		// return tp_r_other(q, mp);
	}
	return (-1);
}

/**
 * tp_rsrv: - read service procedure
 * @q: active queue (read queue)
 *
 * This is a canonical service procedure for the read queue.  Messages on the read
 * queue either come from this module internally, the IP or LLC sublayer or when TPSN
 * is pushed as a module from the driver.  The service procedure takes private
 * structure locks once for the entire loop for speed.
 */
streamscall __hot_out int
tp_rsrv(queue_t *q)
{
#if 0
	struct tp *tp;

	if (likely((tp = tp_trylock(q)) != NULL)) {
		mblk_t *mp;

		__tp_deferred_timers(tp);

		if (likely((mp = getq(q)) != NULL)) {
			__tp_cleanup_read(tp);
			do {
				if (unlikely(tp_r_prim_srv(tp, q, mp))) {
					if (unlikely(!putbq(q, mp))) {
						mp->b_band = 0;	/* must succeed */
						putbq(q, mp);
					}
					LOGRX(tp, "read queue stalled");
					__tp_transmit_wakeup(tp);
					break;
				}
			} while (likely((mp = getq(q)) != NULL));
		}
		__tp_cleanup_read(tp);
		__tp_transmit_wakeup(tp);
		tp_unlock(tp);
	}
#endif
	return (0);
}

/**
 * tp_wput: - write put procedure
 * @q: active queue (write queue)
 * @mp: message to put
 *
 * This is a canoncial put procedure for write.  Locking is performed by the individual message
 * handling procedures.
 */
static streamscall __hot_write int
tp_wput(queue_t *q, mblk_t *mp)
{
	if (unlikely(DB_TYPE(mp) < QPCTL && (q->q_first || (q->q_flag & QSVCBUSY)))
	    || unlikely(tp_w_prim_put(q, mp))) {
		tp_wstat.ms_acnt++;
		if (unlikely(!putq(q, mp))) {
			mp->b_band = 0;
			putq(q, mp);	/* must succeed */
		}
	}
	return (0);
}

/**
 * tp_wsrv: - write service procedure
 * @q: active queue (write queue)
 *
 * This is a canonical service procedure for write.  Locking is performed outside the loop so that
 * locks do not need to be released and acquired with each loop.  Note that the wakeup function must
 * also be executed with the private structure locked.
 */
static streamscall __hot_out int
tp_wsrv(queue_t *q)
{
#if 0
	struct tp *tp;

	if (likely((tp = tp_trylock(q)) != NULL)) {
		mblk_t *mp;

		while (likely((mp = getq(q)) != NULL)) {
			if (unlikely(tp_w_prim_srv(tp, q, mp))) {
				if (unlikely(!putbq(q, mp))) {
					mp->b_band = 0;	/* must succeed */
					putbq(q, mp);
				}
				LOGTX(tp, "write queue stalled");
				break;
			}
		}
		__tp_transmit_wakeup(tp);
		tp_unlock(tp);
	}
#endif
	return (0);
}

/*
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 */

unsigned short modid = DRV_ID;

major_t major = CMAJOR_0;

static const struct tp_profile tp_profiles[] = {
	{{0x00, TP_CMINOR_COTS, 0},
	 {T_INFO_ACK, 1024, 1024, 1024, 1024, 20, T_INFINITE, 1024, T_COTS, TS_UNBND,
	  XPG4_1 | T_SNDZERO}},
	{{0x00, TP_CMINOR_CLTS, 0},
	 {T_INFO_ACK, 1024, 1024, T_INVALID, T_INVALID, 20, T_INFINITE, 1024, T_CLTS, TS_UNBND,
	  XPG4_1 | T_SNDZERO}},
	{{0x49, TP_CMINOR_COTS_ISO, T_ISO_TP},
	 {T_INFO_ACK, 1024, 1024, 1024, 1024, 20, T_INFINITE, 1024, T_COTS, TS_UNBND,
	  XPG4_1 | T_SNDZERO}},
	{{0x49, TP_CMINOR_CLTS_ISO, T_ISO_TP},
	 {T_INFO_ACK, 1024, 1024, T_INVALID, T_INVALID, 20, T_INFINITE, 1024, T_CLTS, TS_UNBND,
	  XPG4_1 | T_SNDZERO}},
	{{0x47, TP_CMINOR_COTS_IP, T_INET_IP},
	 {T_INFO_ACK, 65515, 65515, 65515, 65515, 20, T_INFINITE, 1024, T_COTS, TS_UNBND,
	  XPG4_1 | T_SNDZERO}},
	{{0x47, TP_CMINOR_CLTS_IP, T_INET_IP},
	 {T_INFO_ACK, 65515, 65515, T_INVALID, T_INVALID, 20, T_INFINITE, 1024, T_CLTS, TS_UNBND,
	  XPG4_1 | T_SNDZERO}},
	{{0x47, TP_CMINOR_COTS_UDP, T_INET_UDP},
	 {T_INFO_ACK, 65507, 65507, 65507, 65507, 20, T_INFINITE, 1024, T_COTS, TS_UNBND,
	  XPG4_1 | T_SNDZERO}},
	{{0x47, TP_CMINOR_CLTS_UDP, T_INET_UDP},
	 {T_INFO_ACK, 65507, 65507, T_INVALID, T_INVALID, 20, T_INFINITE, 1024, T_CLTS, TS_UNBND,
	  XPG4_1 | T_SNDZERO}}
};

/**
 * tp_alloc_qbands: - pre-allocate queue bands for queue pair
 * @q: queue for which to allocate queue bands
 */
static fastcall __unlikely int
tp_alloc_qbands(queue_t *q, int cminor)
{
	int err;
	psw_t pl;

	pl = freezestr(q);
	{
		/* Pre-allocate queue band structures on the read side. */
		if ((err = strqset(q, QHIWAT, 1, STRHIGH))) {
			strlog(major, cminor, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
			       "ERROR: could not allocate queue band 1 structure, err = %d", err);
		} else if ((err = strqset(q, QHIWAT, 2, STRHIGH))) {
			strlog(major, cminor, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
			       "ERROR: could not allocate queue band 2 structure, err = %d", err);
		}
	}
	unfreezestr(q, pl);
	return (err);
}

/**
 * tp_qopen: - STREAMS open procedure
 * @q: newly created queue pair
 * @devp: pointer to device number of driver
 * @oflags: open flags
 * @sflag: STREAMS flag
 * @crp: credentials of opening process
 */
static streamscall int
tp_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	(void) tp_alloc_qbands;
	(void) tp_info;
	(void) tp_lock;
	(void) tp_opens;
	(void) tp_bhash;
	(void) tp_bhash_size;
	(void) tp_bhash_order;
	(void) tp_chash;
	(void) tp_chash_size;
	(void) tp_chash_order;
	(void) tp_hash_lock;
	(void) tp_prot_lock;
	(void) tp_prots;
	(void) tp_bind_cachep;
	(void) tp_prot_cachep;
	(void) tpi_primname;
	return (ENXIO);
}

/**
 * tp_qclose: - STREAMS close procedure
 * @q: queue pair
 * @oflags: file open flags
 * @crp: credentials of closing process
 */
static streamscall int
tp_qclose(queue_t *q, int oflags, cred_t *crp)
{
	return (0);
}

/*
 *  =========================================================================
 *
 *  Bottom-end receive functions
 *
 *  =========================================================================
 */

struct tp4hdr {
	unsigned char len;
	unsigned char type;
	unsigned short dref;
	unsigned short sref;
	unsigned char tpdu[0];
};

STATIC int tp_tp4_v4_rcv(struct sk_buff *skb);
STATIC void tp_tp4_v4_err(struct sk_buff *skb, uint32_t info);
STATIC int tp_iso_v4_rcv(struct sk_buff *skb);
STATIC void tp_iso_v4_err(struct sk_buff *skb, uint32_t info);
STATIC __hot_in int tp_udp_v4_rcv(struct sk_buff *skb);
STATIC __unlikely void tp_udp_v4_err(struct sk_buff *skb, u32 info);

#ifdef HAVE_KMEM_STRUCT_INET_PROTOCOL_PROTOCOL
STATIC struct inet_protocol tp_tp4_protocol = {
	.handler = tp_tp4_v4_rcv,	/* ISO-TP4 data handler */
	.err_handler = tp_tp4_v4_err,	/* ISO-TP4 error control */
	.protocol = 29,		/* ISO-TP4 protocol ID */
	.name = "ISO-TP4",
};

STATIC struct inet_protocol tp_iso_protocol = {
	.handler = tp_iso_v4_rcv,	/* ISO-IP data handler */
	.err_handler = tp_iso_v4_err,	/* ISO-IP error control */
	.protocol = 80,		/* ISO-IP protocol ID */
	.name = "ISO-IP",
};

STATIC struct inet_protocol tp_udp_protocol = {
	.handler = tp_udp_v4_rcv,	/* ISO-UDP data handler */
	.err_handler = tp_udp_v4_err,	/* ISO-UDP error control */
	.protocol = IPPROTO_UDP,	/* ISO-UDP protocol ID */
	.name = "ISO-UDP",
};
#endif				/* HAVE_KMEM_STRUCT_INET_PROTOCOL_PROTOCOL */

#ifdef HAVE_KMEMB_STRUCT_INET_PROTOCOL_NO_POLICY
STATIC struct inet_protocol tp_tp4_protocol = {
	.handler = tp_tp4_v4_rcv,	/* ISO-TP4 data handler */
	.err_handler = tp_tp4_v4_err,	/* ISO-TP4 error control */
	.no_policy = 1,
};

STATIC struct inet_protocol tp_iso_protocol = {
	.handler = tp_iso_v4_rcv,	/* ISO-IP data handler */
	.err_handler = tp_iso_v4_err,	/* ISO-IP error control */
	.no_policy = 1,
};

STATIC struct inet_protocol tp_udp_protocol = {
	.handler = tp_udp_v4_rcv,	/* ISO-UDP data handler */
	.err_handler = tp_udp_v4_err,	/* ISO-UDP error control */
	.no_policy = 1,
};
#endif				/* HAVE_KMEMB_STRUCT_INET_PROTOCOL_NO_POLICY */

#ifdef HAVE_KMEMB_STRUCT_NET_PROTOCOL_NO_POLICY
STATIC struct net_protocol tp_tp4_protocol = {
	.handler = tp_tp4_v4_rcv,	/* ISO-TP4 data handler */
	.err_handler = tp_tp4_v4_err,	/* ISO-TP4 error control */
	.no_policy = 1,
};

STATIC struct net_protocol tp_iso_protocol = {
	.handler = tp_iso_v4_rcv,	/* ISO-IP data handler */
	.err_handler = tp_iso_v4_err,	/* ISO-IP error control */
	.no_policy = 1,
};

STATIC struct net_protocol tp_udp_protocol = {
	.handler = tp_udp_v4_rcv,	/* ISO-UDP data handler */
	.err_handler = tp_udp_v4_err,	/* ISO-UDP error control */
	.no_policy = 1,
};
#endif				/* HAVE_KMEMB_STRUCT_NET_PROTOCOL_NO_POLICY */

STATIC struct ipnet_protocols tp_protos = {
	.tp4 = {
		.proto = &tp_tp4_protocol,
		.next = NULL,
		.kmod = NULL,
		},
	.iso = {
		.proto = &tp_iso_protocol,
		.next = NULL,
		.kmod = NULL,
		},
	.udp = {
		.proto = &tp_udp_protocol,
		.next = NULL,
		.kmod = NULL,
		},
};

static inline void
tp_put(struct tp *tp)
{
}

STATIC streamscall __hot_get void
tp_free(caddr_t data)
{
	struct sk_buff *skb = (typeof(skb)) data;

	dassert(skb != NULL);
	kfree_skb(skb);
	return;
}

/**
 * tp_tp4_v4_rcv: - receive an ISO-TP4 message
 * @skb: the message
 *
 * This is the received frame handler for ISO-TP4.  All packets received by IPv4 with the protocol
 * 29 (ISO-TP4) will arrive here first.
 */
STATIC int
tp_tp4_v4_rcv(struct sk_buff *skb)
{
	mblk_t *mp;
	struct tp *tp;
	struct tp4hdr *th;

#ifdef HAVE_KFUNC_NF_RESET
	nf_reset(skb);
#endif
	if (skb->pkt_type != PACKET_HOST)
		goto bad_pkt_type;
	/* For now...  We should actually place non-linear fragments into seperate mblks and pass them up as
	   a chain, or deal with non-linear sk_buffs directly.  As it winds up, the netfilter hooks linearize 
	   anyway. */
#ifdef HAVE_KFUNC_SKB_LINEARIZE_1_ARG
	if (skb_is_nonlinear(skb) && skb_linearize(skb) != 0)
		goto linear_fail;
#else				/* HAVE_KFUNC_SKB_LINEARIZE_1_ARG */
	if (skb_is_nonlinear(skb) && skb_linearize(skb, GFP_ATOMIC) != 0)
		goto linear_fail;
#endif				/* HAVE_KFUNC_SKB_LINEARIZE_1_ARG */
	/* pull up the ip header */
	th = (typeof(th)) skb_transport_header(skb);
	if (th->len < 6)
		goto too_small;
	if (!(mp = skballoc(skb, BPRI_MED)))
		goto no_buffers;
	// mp->b_rptr = skb->data;
	// mp->b_wptr = mp->b_rptr + skb->len;
	DB_BASE(mp) = skb_network_header(skb);	/* important */
	DB_LIM(mp) = mp->b_wptr;
	mp->b_datap->db_size = DB_LIM(mp) - DB_BASE(mp);
	/* we do the lookup before the checksum */
	{
		struct iphdr *iph = (typeof(iph)) skb_network_header(skb);

		printd(("%s: mp %p lookup of stream\n", __FUNCTION__, mp));
		// if (!(tp = tp_lookup(th, iph->daddr, iph->saddr)))
			goto no_stream;
	}
	/* perform the stream-specific checksum */
	// FIXME;
	skb->dev = NULL;
	if (!tp->rq || !canput(tp->rq))
		goto flow_controlled;
	putq(tp->rq, mp);	/* must succeed, band 0 */
	/* all done */
	printd(("%s: mp %p put to stream %p\n", __FUNCTION__, mp, tp));
	tp_put(tp);
	return (0);
#if 0
      bad_checksum:
	ptrace(("ERROR: Bad checksum\n"));
	goto free_it;
#endif
      no_stream:
	ptrace(("ERROR: No stream\n"));
	icmp_send(skb, ICMP_DEST_UNREACH, ICMP_PORT_UNREACH, 0);
	// tp_rcv_ootb(mp);
	goto free_it;
      free_it:
	ptrace(("ERROR: Discarding message\n"));
	/* free skb in sockets, free mp in streams */
	freemsg(mp);
	return (0);
      too_small:
	ptrace(("ERROR: Packet too small\n"));
	goto discard_it;
      linear_fail:
	ptrace(("ERROR: Could not linearize skb\n"));
	goto discard_it;
      bad_pkt_type:
	ptrace(("ERROR: Packet not PACKET_HOST\n"));
	goto discard_it;
      discard_it:
	/* Discard frame silently. */
	ptrace(("ERROR: Discarding frame silently\n"));
	kfree_skb(skb);
	return (0);
      flow_controlled:
	tp_put(tp);
	ptrace(("ERROR: Flow Controlled\n"));
	goto free_it;
      no_buffers:
	ptrace(("ERROR: Could not allocate mblk\n"));
	goto discard_it;
}

/**
 * tp_tp4_v4_err: - receive ICMP error message
 * @skb: the message
 * @info: the ICMP error information
 *
 * This is the error handler for ISO-TP4.  We have received an ICMP error for the protocol number.
 * THis routine is called by the ICMP module when it gets some sort of error condition.  If err < 0
 * then the stream should be errored (M_ERROR) and the error returned to the peer.  If err > 0 it is
 * just the icmp type << 8 | icmp code.  After adjustment header points to the first 8 bytes of the
 * TP4 header.  We need to find the appropriate remote reference.
 *
 * Because we don't want any races where, we place a M_CTL message (in band 1) on the read queue of
 * the stream to which the message applies.  This distinguishes it from M_DATA messages.  It is
 * processed within the stream with queues locked  by tp_recv_err when the M_CTL message is dequeued
 * and processed.  We have to copy the information because the skb will go away after this call
 * returns.  Because we check for flow control, this approach is also more resilient against ICMP
 * flooding attacks.
 *
 * TP4 headers on sent messages are as follows:
 *
 *	Octet 1	- length of header excluding this octet
 *	Octet 2 - message type (and credit)
 *	Octet 3 - hi order DREF (zero on CR)
 *	Octet 4 - lo order DREF (zero on CR)
 *	Octet 5 - hi order SREF
 *	Octet 6 - lo order SREF
 */
STATIC void
tp_tp4_v4_err(struct sk_buff *skb, uint32_t info)
{
	struct tp *tp;
	struct iphdr *iph = (struct iphdr *) skb->data;

	if (skb->len < (iph->ihl << 2) + 1 + sizeof(struct tp4hdr))
		goto drop;
	printd(("%s: %s: error packet received %p\n", DRV_NAME, __FUNCTION__, skb));
	/* Note: use returned IP header and possibly payload for lookup */
	// if ((tp = tp_tp4_lookup_icmp(iph, skb->len)) == NULL)
		goto no_stream;
	if (tp_get_state(tp) == TS_UNBND)
		goto closed;
	{
		mblk_t *mp;
		queue_t *q;
		size_t plen = skb->len + (skb->data - skb_network_header(skb));

		/* Create and queue a specialized M_CTL message to the Stream's read queue for further
		   processing.  The Stream will convert this message into a T_UDERROR_IND or T_DISCON_IND
		   message and pass it along. */
		if ((mp = allocb(plen, BPRI_MED)) == NULL)
			goto no_buffers;
		/* check flow control only after we have a buffer */
		if ((q = tp->rq) == NULL || !bcanput(q, 1))
			goto flow_controlled;
		mp->b_datap->db_type = M_CTL;
		mp->b_band = 1;
		bcopy(skb_network_header(skb), mp->b_wptr, plen);
		mp->b_wptr += plen;
		put(q, mp);
		goto discard_put;
	      flow_controlled:
		tp_rstat.ms_ccnt++;
		ptrace(("ERROR: stream is flow controlled\n"));
		freeb(mp);
		goto discard_put;
	}
      discard_put:
	/* release reference from lookup */
	if (tp)
		tp_put(tp);
	tp_v4_err_next(tp_protos.tp4.next, skb, info);	/* anyway */
	return;
      no_buffers:
	ptrace(("ERROR: could not allocate buffer\n"));
	goto discard_put;
      closed:
	ptrace(("ERROR: ICMP for closed stream\n"));
	goto discard_put;
      no_stream:
	ptrace(("ERROR: could not find stream for ICMP message\n"));
	tp_v4_err_next(tp_protos.tp4.next, skb, info);
	goto drop;
      drop:
#ifdef HAVE_KINC_LINUX_SNMP_H
#ifdef HAVE_ICMP_INC_STATS_BH_2_ARGS
	ICMP_INC_STATS_BH(dev_net(skb->dev), ICMP_MIB_INERRORS);
#else
	ICMP_INC_STATS_BH(ICMP_MIB_INERRORS);
#endif
#else
	ICMP_INC_STATS_BH(IcmpInErrors);
#endif
	return;
}

STATIC int
tp_iso_v4_rcv(struct sk_buff *skb)
{
	mblk_t *mp;
	struct tp *tp;
	struct tp4hdr *th;
	unsigned char *nlpid;

#ifdef HAVE_KFUNC_NF_RESET
	nf_reset(skb);
#endif
	if (skb->pkt_type != PACKET_HOST)
		goto bad_pkt_type;
	/* For now...  We should actually place non-linear fragments into seperate mblks and pass them up as
	   a chain, or deal with non-linear sk_buffs directly.  As it winds up, the netfilter hooks linearize 
	   anyway. */
#ifdef HAVE_KFUNC_SKB_LINEARIZE_1_ARG
	if (skb_is_nonlinear(skb) && skb_linearize(skb) != 0)
		goto linear_fail;
#else				/* HAVE_KFUNC_SKB_LINEARIZE_1_ARG */
	if (skb_is_nonlinear(skb) && skb_linearize(skb, GFP_ATOMIC) != 0)
		goto linear_fail;
#endif				/* HAVE_KFUNC_SKB_LINEARIZE_1_ARG */
	/* pull up the ip header */
	nlpid = (typeof(nlpid)) skb_transport_header(skb);
	if (nlpid[0] != 0)
		goto wrong_protocol;
	th = (typeof(th)) (nlpid + 1);
	if (th->len < 6)
		goto too_small;
	if (!(mp = skballoc(skb, BPRI_MED)))
		goto no_buffers;
	// mp->b_rptr = skb->data;
	// mp->b_wptr = mp->b_rptr + skb->len;
	DB_BASE(mp) = skb_network_header(skb);	/* important */
	DB_LIM(mp) = mp->b_wptr;
	mp->b_datap->db_size = DB_LIM(mp) - DB_BASE(mp);
	/* we do the lookup before the checksum */
	{
		struct iphdr *iph = (typeof(iph)) skb_network_header(skb);

		printd(("%s: mp %p lookup of stream\n", __FUNCTION__, mp));
		// if (!(tp = tp_lookup(th, iph->daddr, iph->saddr)))
			goto no_stream;
	}
	/* perform the stream-specific checksum */
	// FIXME;
	skb->dev = NULL;
	if (!tp->rq || !canput(tp->rq))
		goto flow_controlled;
	putq(tp->rq, mp);	/* must succeed, band 0 */
	/* all done */
	printd(("%s: mp %p put to stream %p\n", __FUNCTION__, mp, tp));
	tp_put(tp);
	return (0);
#if 0
      bad_checksum:
	ptrace(("ERROR: Bad checksum\n"));
	goto free_it;
#endif
      no_stream:
	ptrace(("ERROR: No stream\n"));
	icmp_send(skb, ICMP_DEST_UNREACH, ICMP_PORT_UNREACH, 0);
	// tp_rcv_ootb(mp);
	goto free_it;
      free_it:
	ptrace(("ERROR: Discarding message\n"));
	/* free skb in sockets, free mp in streams */
	freemsg(mp);
	return (0);
      too_small:
	ptrace(("ERROR: Packet too small\n"));
	goto discard_it;
      wrong_protocol:
	ptrace(("ERROR: Packet not for INLP\n"));
	goto discard_it;
      linear_fail:
	ptrace(("ERROR: Could not linearize skb\n"));
	goto discard_it;
      bad_pkt_type:
	ptrace(("ERROR: Packet not PACKET_HOST\n"));
	goto discard_it;
      discard_it:
	/* Discard frame silently. */
	ptrace(("ERROR: Discarding frame silently\n"));
	kfree_skb(skb);
	return (0);
      flow_controlled:
	tp_put(tp);
	ptrace(("ERROR: Flow Controlled\n"));
	goto free_it;
      no_buffers:
	ptrace(("ERROR: Could not allocate mblk\n"));
	goto discard_it;
}

STATIC void
tp_iso_v4_err(struct sk_buff *skb, uint32_t info)
{
	struct tp *tp;
	struct iphdr *iph = (struct iphdr *) skb->data;

	if (skb->len < (iph->ihl << 2) + 1 + sizeof(struct tp4hdr))
		goto drop;
	printd(("%s: %s: error packet received %p\n", DRV_NAME, __FUNCTION__, skb));
	/* Note: use returned IP header and possibly payload for lookup */
	// if ((tp = tp_iso_lookup_icmp(iph, skb->len)) == NULL)
		goto no_stream;
	if (tp_get_state(tp) == TS_UNBND)
		goto closed;
	{
		mblk_t *mp;
		queue_t *q;
		size_t plen = skb->len + (skb->data - skb_network_header(skb));

		/* Create and queue a specialized M_CTL message to the Stream's read queue for further
		   processing.  The Stream will convert this message into a T_UDERROR_IND or T_DISCON_IND
		   message and pass it along. */
		if ((mp = allocb(plen, BPRI_MED)) == NULL)
			goto no_buffers;
		/* check flow control only after we have a buffer */
		if ((q = tp->rq) == NULL || !bcanput(q, 1))
			goto flow_controlled;
		mp->b_datap->db_type = M_CTL;
		mp->b_band = 1;
		bcopy(skb_network_header(skb), mp->b_wptr, plen);
		mp->b_wptr += plen;
		put(q, mp);
		goto discard_put;
	      flow_controlled:
		tp_rstat.ms_ccnt++;
		ptrace(("ERROR: stream is flow controlled\n"));
		freeb(mp);
		goto discard_put;
	}
      discard_put:
	/* release reference from lookup */
	if (tp)
		tp_put(tp);
	tp_v4_err_next(tp_protos.iso.next, skb, info);	/* anyway */
	return;
      no_buffers:
	ptrace(("ERROR: could not allocate buffer\n"));
	goto discard_put;
      closed:
	ptrace(("ERROR: ICMP for closed stream\n"));
	goto discard_put;
      no_stream:
	ptrace(("ERROR: could not find stream for ICMP message\n"));
	tp_v4_err_next(tp_protos.iso.next, skb, info);
	goto drop;
      drop:
#ifdef HAVE_KINC_LINUX_SNMP_H
#ifdef HAVE_ICMP_INC_STATS_BH_2_ARGS
	ICMP_INC_STATS_BH(dev_net(skb->dev), ICMP_MIB_INERRORS);
#else
	ICMP_INC_STATS_BH(ICMP_MIB_INERRORS);
#endif
#else
	ICMP_INC_STATS_BH(IcmpInErrors);
#endif
	return;
}

#ifndef CHECKSUM_HW
#define CHECKSUM_HW CHECKSUM_COMPLETE
#endif

/**
 * tp_udp_v4_rcv: - process a received UDP packet
 * @skb: socket buffer containing IP packet
 *
 * This function is a callback function called by the Linux IP code when a packet is delivered to
 * the UDP IP protocol number.  If the destination address is a broadcast or multicast address, pass
 * it for distribution to multiple Streams.  If the destination address is a unicast address, look
 * up the receiving IP Stream based on the protocol number and IP addresses.  If no receiving IP
 * Stream exists for a unicast packet, or if the packet is a broadcast or multicast packet, pass the
 * packet along to the next handler, if any.  If there is no next handler, and the packet was not
 * sent to any Stream, generate an appropriate ICMP error.  If the receiving Stream is flow
 * controlled, simply discard its copy of the IP packet.  Otherwise, generate an (internal) M_DATA
 * message and pass it to the Stream.
 */
STATIC __hot_in int
tp_udp_v4_rcv(struct sk_buff *skb)
{
	struct tp *tp = NULL;
	struct iphdr *iph = (typeof(iph)) skb_network_header(skb);
	struct udphdr *uh = (struct udphdr *) (skb_network_header(skb) + (iph->ihl << 2));
	unsigned char *nlpid;
	struct tp4hdr *th;
	struct rtable *rt;
	ushort ulen;

#ifdef HAVE_KFUNC_NF_RESET
	nf_reset(skb);
#endif
//      IP_INC_STATS_BH(IpInDelivers);  /* should wait... */
	if (unlikely(!pskb_may_pull(skb, sizeof(struct udphdr) + 1 + sizeof(struct tp4hdr))))
		goto too_small;
#if 1
	/* I don't think that ip_rcv will ever give us a packet that is not PACKET_HOST. */
	if (unlikely(skb->pkt_type != PACKET_HOST))
		goto bad_pkt_type;
#endif
	rt = skb_rtable(skb);
	if (rt->rt_flags & (RTCF_BROADCAST | RTCF_MULTICAST))
		/* need to do something about broadcast and multicast */ ;
	_printd(("%s: %s: packet received %p\n", DRV_NAME, __FUNCTION__, skb));
//      UDP_INC_STATS_BH(UdpInDatagrams);
	uh = (typeof(uh)) skb_transport_header(skb);
	ulen = ntohs(uh->len);
	/* sanity check UDP length */
	if (unlikely(ulen > skb->len || ulen < sizeof(struct udphdr) + 1 + sizeof(struct tp4hdr)))
		goto too_small;
	if (unlikely(pskb_trim(skb, ulen)))
		goto too_small;
	/* we do the lookup before the checksum */
	nlpid = (typeof(nlpid)) (uh + 1);
	if (unlikely(*nlpid != 0))
		goto wrong_protocol;
	th = (typeof(th)) (nlpid + 1);
	if (unlikely(th->len < sizeof(struct tp4hdr)))
		goto too_small;
	// if (unlikely((tp = tp_udp_lookup(iph, uh, th)) == NULL))
		goto no_stream;
	/* checksum initialization */
	if (likely(uh->check == 0))
		skb->ip_summed = CHECKSUM_UNNECESSARY;
	else {
		if (skb->ip_summed == CHECKSUM_HW)
			skb->ip_summed = CHECKSUM_UNNECESSARY;
		else if (skb->ip_summed != CHECKSUM_UNNECESSARY)
			skb->csum = csum_tcpudp_nofold(iph->saddr, iph->daddr, ulen, IPPROTO_UDP, 0);
	}
	if (unlikely(skb_is_nonlinear(skb))) {
		_ptrace(("Non-linear sk_buff encountered!\n"));
		goto linear_fail;
	}
	{
		mblk_t *mp;
		frtn_t fr = { &tp_free, (caddr_t) skb };
		size_t plen = skb->len + (skb->data - skb_network_header(skb));

		/* now allocate an mblk */
		if (unlikely((mp = esballoc(skb_network_header(skb), plen, BPRI_MED, &fr)) == NULL))
			goto no_buffers;
		/* tell others it is a socket buffer */
		mp->b_datap->db_flag |= DB_SKBUFF;
		_ptrace(("Allocated external buffer message block %p\n", mp));
		/* check flow control only after we have a buffer */
		if (unlikely(tp->rq == NULL || !canput(tp->rq)))
			goto flow_controlled;
		// mp->b_datap->db_type = M_DATA;
		mp->b_wptr += plen;
		put(tp->rq, mp);
//              UDP_INC_STATS_BH(UdpInDatagrams);
		/* release reference from lookup */
		tp_put(tp);
		return (0);
	      flow_controlled:
		tp_rstat.ms_ccnt++;
		freeb(mp);	/* will take sk_buff with it */
		tp_put(tp);
		return (0);
	}
      bad_checksum:
//      UDP_INC_STATS_BH(UdpInErrors);
//      IP_INC_STATS_BH(IpInDiscards);
	/* decrement IpInDelivers ??? */
	// goto linear_fail;
      wrong_protocol:
      no_buffers:
      linear_fail:
	if (tp)
		tp_put(tp);
	kfree_skb(skb);
	return (0);
      no_stream:
	ptrace(("ERROR: No stream\n"));
	/* Note, in case there is nobody to pass it to, we have to complete the checksum check before
	   dropping it to handle stats correctly. */
	if (skb->ip_summed != CHECKSUM_UNNECESSARY
	    && (unsigned short) csum_fold(skb_checksum(skb, 0, skb->len, skb->csum)))
		goto bad_checksum;
//      UDP_INC_STATS_BH(UdpNoPorts);   /* should wait... */
#if 1
      bad_pkt_type:
#endif
      too_small:
	if (tp_v4_rcv_next(tp_protos.udp.next, skb)) {
		/* TODO: want to generate an ICMP error here */
	}
	return (0);
}

/**
 * tp_udp_v4_err: - process a received ICMP packet
 * @skb: socket buffer containing ICMP packet
 * @info: additional information (unused)
 *
 * This function is a network protocol callback that is invoked when transport specific ICMP errors
 * are received.  The function looks up the Stream and, if found, wraps the packet in an M_CTL
 * message and passes it to the read queue of the Stream.
 *
 * ICMP packet consists of ICMP IP header, ICMP header, IP header of returned packet, and IP payload
 * of returned packet (up to some number of bytes of total payload).  The passed in sk_buff has
 * skb->data pointing to the ICMP payload which is the beginning of the returned IP header.
 * However, we include the entire packet in the message.
 *
 * LOCKING: tp_lock protects the master list and protects from open, close, link and unlink.
 * tp->qlock protects the state of private structure.  tp->refs protects the private structure from
 * being deallocated before locking.
 */
STATIC __unlikely void
tp_udp_v4_err(struct sk_buff *skb, u32 info)
{
	struct tp *tp;
	struct iphdr *iph = (struct iphdr *) skb->data;

	if (skb->len < (iph->ihl << 2) + sizeof(struct udphdr) + 1 + sizeof(struct tp4hdr))
		goto drop;
	printd(("%s: %s: error packet received %p\n", DRV_NAME, __FUNCTION__, skb));
	/* Note: use returned IP header and possibly payload for lookup */
	// if ((tp = tp_udp_lookup_icmp(iph, skb->len)) == NULL)
		goto no_stream;
	if (tp_get_state(tp) == TS_UNBND)
		goto closed;
	{
		mblk_t *mp;
		queue_t *q;
		size_t plen = skb->len + (skb->data - skb_network_header(skb));

		/* Create and queue a specialized M_CTL message to the Stream's read queue for further
		   processing.  The Stream will convert this message into a T_UDERROR_IND or T_DISCON_IND
		   message and pass it along. */
		if ((mp = allocb(plen, BPRI_MED)) == NULL)
			goto no_buffers;
		/* check flow control only after we have a buffer */
		if ((q = tp->rq) == NULL || !bcanput(q, 1))
			goto flow_controlled;
		mp->b_datap->db_type = M_CTL;
		mp->b_band = 1;
		bcopy(skb_network_header(skb), mp->b_wptr, plen);
		mp->b_wptr += plen;
		put(q, mp);
		goto discard_put;
	      flow_controlled:
		tp_rstat.ms_ccnt++;
		ptrace(("ERROR: stream is flow controlled\n"));
		freeb(mp);
		goto discard_put;
	}
      discard_put:
	/* release reference from lookup */
	if (tp)
		tp_put(tp);
	tp_v4_err_next(tp_protos.udp.next, skb, info);	/* anyway */
	return;
      no_buffers:
	ptrace(("ERROR: could not allocate buffer\n"));
	goto discard_put;
      closed:
	ptrace(("ERROR: ICMP for closed stream\n"));
	goto discard_put;
      no_stream:
	ptrace(("ERROR: could not find stream for ICMP message\n"));
	tp_v4_err_next(tp_protos.udp.next, skb, info);
	goto drop;
      drop:
#ifdef HAVE_KINC_LINUX_SNMP_H
#ifdef HAVE_ICMP_INC_STATS_BH_2_ARGS
	ICMP_INC_STATS_BH(dev_net(skb->dev), ICMP_MIB_INERRORS);
#else
	ICMP_INC_STATS_BH(ICMP_MIB_INERRORS);
#endif
#else
	ICMP_INC_STATS_BH(IcmpInErrors);
#endif
	return;
}

/**
 * tp_llc_rcv: - receive an unnumbered LLC class I frame
 * @skb: the frame
 * @dev: the device on which the frame was received
 * @pt: the packet type of the received frame
 *
 * Note that it is not necessary to use the device or packet type.  The packet type will always be
 * 802.2, either for Ethernet (802.3) or Token Ring (802.5).
 *
 * The network_header points to the DSAP/SSAP/UI-CTRL portion of the frame (the 802.2 part and is
 * actually the data link portion).  The transport_header points to the payload of the UI frame (I
 * field) and is in fact the network portion.
 */
STATIC __hot_in int
#ifdef HAVE_KMEMB_STRUCT_PACKET_TYPE_FUNC_4_ARGS
tp_llc_rcv(struct sk_buff *skb, struct net_device *dev, struct packet_type *pt, struct net_device *orig_dev)
#else
tp_llc_rcv(struct sk_buff *skb, struct net_device *dev, struct packet_type *pt)
#endif
{
	struct tp *tp = NULL;
	unsigned char *nlpid;
	struct tp4hdr *th;
	struct llc_pdu_un *uh;
	struct llc_addr daddr, saddr;

#ifdef HAVE_KFUNC_NF_RESET
	nf_reset(skb);
#endif
	if (unlikely(!pskb_may_pull(skb, 1 + sizeof(struct tp4hdr))))
		goto too_small;
	if (unlikely(skb->pkt_type != PACKET_HOST))
		goto bad_pkt_type;
	uh = (typeof(uh)) skb_network_header(skb);
	nlpid = (typeof(nlpid)) skb_transport_header(skb);
	if (unlikely(*nlpid != 0))
		goto wrong_protocol;
	th = (typeof(th)) (nlpid + 1);
	if (unlikely(th->len < sizeof(struct tp4hdr)))
		goto too_small;
	if (unlikely(skb->len < 2 + th->len))
		goto too_small;
	llc_pdu_decode_da(skb, daddr.mac);
	llc_pdu_decode_dsap(skb, &daddr.lsap);
	llc_pdu_decode_sa(skb, saddr.mac);
	llc_pdu_decode_ssap(skb, &saddr.lsap);
	// if (unlikely((tp = tp_llc_lookup(uh, &daddr, &saddr)) == NULL))
		goto no_stream;
	if (unlikely(skb_is_nonlinear(skb))) {
		_ptrace(("Non-linear sk_buff encountered!\n"));
		goto linear_fail;
	}
	{
		mblk_t *mp;
		frtn_t fr = { &tp_free, (caddr_t) skb };
		size_t plen = skb->len + (skb->data - skb_network_header(skb));

		/* now allocate an mblk */
		if (unlikely((mp = esballoc(skb_network_header(skb), plen, BPRI_MED, &fr)) == NULL))
			goto no_buffers;
		/* tell others it is a socket buffer */
		mp->b_datap->db_flag |= DB_SKBUFF;
		_ptrace(("Allocated external buffer message block %p\n", mp));
		/* check flow control only after we have a buffer */
		if (unlikely(tp->rq == NULL || !canput(tp->rq)))
			goto flow_controlled;
		// mp->b_datap->db_type = M_DATA;
		mp->b_wptr += plen;
		put(tp->rq, mp);
		/* release reference from lookup */
		tp_put(tp);
		return (0);
	      flow_controlled:
		tp_rstat.ms_ccnt++;
		freeb(mp);	/* will take sk_buff with it */
		tp_put(tp);
		return (0);
	}
      wrong_protocol:
      no_buffers:
      linear_fail:
	if (tp)
		tp_put(tp);
	kfree_skb(skb);
	return (0);
      no_stream:
	ptrace(("ERROR: No stream\n"));
      bad_pkt_type:
      too_small:
	kfree_skb(skb);
	return (0);

}

/*
 *  =========================================================================
 *
 *  Netdevice Notifier
 *
 *  =========================================================================
 */
#ifndef LOOPBACK
#define LOOPBACK(x) ipv4_is_loopback(x)
#endif
#ifndef MULTICAST
#define MULTICAST(x) ipv4_is_multicast(x)
#endif
#ifndef ZERONET
#define ZERONET(x) ipv4_is_zeronet(x)
#endif
#ifndef LOCAL_MCAST
#define LOCAL_MCAST(x) ipv4_is_local_multicast(x)
#endif

STATIC int
tp_notifier(struct notifier_block *self, unsigned long msg, void *data)
{
	struct net_device *dev = (struct net_device *) data;
	struct in_device *in_dev;

#if ( defined HAVE_KFUNC_RCU_READ_LOCK || defined HAVE_KMACRO_RCU_READ_LOCK )
	rcu_read_lock();
#endif
#ifdef HAVE_KFUNC___IN_DEV_GET_RCU
	if (!(in_dev = __in_dev_get_rcu(dev)))
		goto done;
#else
	if (!(in_dev = __in_dev_get(dev)))
		goto done;
#endif
	switch (msg) {
	case NETDEV_UP:
	case NETDEV_REBOOT:
	{
		caddr_t item;

		read_lock(&tp_lock);
		for (item = tp_opens; item; item = mi_next_ptr(item)) {
			struct tp *tp = (typeof(tp)) item;

			if (tp_chk_state(tp, TSF_UNBND | TSF_WACK_BREQ))
				continue;

			switch (tp->loc.type) {
			case TP_SNPA_TYPE_IP4:
			case TP_SNPA_TYPE_IP4_PORT:
			{
				struct in_ifaddr *ifa;

				for (ifa = in_dev->ifa_list; ifa; ifa = ifa->ifa_next) {
					if (LOOPBACK(ifa->ifa_local))
						continue;
					/* check if we bound to ifa_local */
					if (memcmp(tp->loc.addr, (unsigned char *) &ifa->ifa_local, 4) == 0) {
						tp->loc.dev = dev;
						break;
					}
				}
				break;
			}
			case TP_SNPA_TYPE_MAC:
			case TP_SNPA_TYPE_IP6:
			case TP_SNPA_TYPE_IP6_PORT:
			case TP_SNPA_TYPE_IFNAME:
				break;
			default:
				continue;
			}

		}
		read_unlock(&tp_lock);
		break;
	}
	case NETDEV_DOWN:
	case NETDEV_GOING_DOWN:
	{
		caddr_t item;

		read_lock(&tp_lock);
		for (item = tp_opens; item; item = mi_next_ptr(item)) {
			struct tp *tp = (typeof(tp)) item;

			if (tp_chk_state(tp, TSF_UNBND | TSF_WACK_BREQ))
				continue;

			switch (tp->loc.type) {
			case TP_SNPA_TYPE_IP4:
			case TP_SNPA_TYPE_IP4_PORT:
			{
				struct in_ifaddr *ifa;

				for (ifa = in_dev->ifa_list; ifa; ifa = ifa->ifa_next) {
					if (LOOPBACK(ifa->ifa_local))
						continue;
					/* check if we bound to ifa_local */
					if (memcmp(tp->loc.addr, (unsigned char *) &ifa->ifa_local, 4) == 0) {
						tp->loc.dev = NULL;
						break;
					}
				}
				break;
			}
			case TP_SNPA_TYPE_MAC:
			case TP_SNPA_TYPE_IP6:
			case TP_SNPA_TYPE_IP6_PORT:
			case TP_SNPA_TYPE_IFNAME:
				break;
			default:
				continue;
			}

		}
		read_unlock(&tp_lock);
		break;
	}
	case NETDEV_CHANGEADDR:
		/* we should probably do something for this, but I don't know wheterh it is possible to
		   change addresses on an up interface anyway */
	default:
	case NETDEV_CHANGE:
	case NETDEV_REGISTER:
	case NETDEV_UNREGISTER:
	case NETDEV_CHANGEMTU:
	case NETDEV_CHANGENAME:
		break;
	}
      done:
#if ( defined HAVE_KFUNC_RCU_READ_LOCK || defined HAVE_KMACRO_RCU_READ_LOCK )
	rcu_read_unlock();
#endif
	return NOTIFY_DONE;
}

/*
 *  =========================================================================
 *
 *  Registration and initialization
 *
 *  =========================================================================
 */
#ifdef LINUX

STATIC struct notifier_block tp_netdev_notifier = {
	.notifier_call = &tp_notifier,
};

STATIC int
tp_init_notify(void)
{
	register_netdevice_notifier(&tp_netdev_notifier);
	return (0);
}

STATIC int
tp_term_notify(void)
{
	unregister_netdevice_notifier(&tp_netdev_notifier);
	return (0);
}

STATIC int
tp_term_proto(void)
{
	tp_give_protocol(&tp_protos.udp, IPPROTO_UDP);
	tp_give_protocol(&tp_protos.iso, 80);
	tp_give_protocol(&tp_protos.tp4, 29);
	return (0);
}

STATIC int
tp_init_proto(void)
{
	int err;

	if ((err = tp_take_protocol(&tp_protos.tp4, 29)))
		goto no_tp4;
	if ((err = tp_take_protocol(&tp_protos.iso, 80)))
		goto no_iso;
	if ((err = tp_take_protocol(&tp_protos.udp, IPPROTO_UDP)))
		goto no_udp;
	return (0);
      no_udp:
	tp_give_protocol(&tp_protos.iso, 80);
      no_iso:
	tp_give_protocol(&tp_protos.tp4, 29);
      no_tp4:
	return (err);
}

STATIC struct llc_sap *tp_llc_sap = NULL;

STATIC __unlikely int
tp_init_llc(void)
{
	if ((tp_llc_sap = llc_sap_open(0xfe, tp_llc_rcv)) == NULL)
		return (-EBUSY);
	return (0);
}

STATIC __unlikely int
tp_term_llc(void)
{
	if (tp_llc_sap != NULL) {
		llc_sap_close(tp_llc_sap);
		tp_llc_sap = NULL;
		return (0);
	}
	return (-EINVAL);
}

/*
 *  Linux Registration
 *  -------------------------------------------------------------------------
 */

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0444);
#endif
MODULE_PARM_DESC(modid, "Module ID for the TPSN driver. (0 for allocation.)");

#ifndef module_param
MODULE_PARM(major, "h");
#else
module_param(major, int, 0444);
#endif
MODULE_PARM_DESC(major, "Device number for the TPSN driver. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
static struct cdevsw tp_cdev = {
	.d_name = DRV_NAME,
	.d_str = &tp_info,
	.d_flag = D_MP | D_CLONE,
	.d_fop = NULL,
	.d_mode = S_IFCHR | S_IRUGO | S_IWUGO,
	.d_kmod = THIS_MODULE,
};

static struct devnode tp_node_cots = {
	.n_name = "cots",
	.n_flag = D_CLONE,	/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

static struct devnode tp_node_clts = {
	.n_name = "clts",
	.n_flag = D_CLONE,	/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

static struct devnode tp_node_cots_iso = {
	.n_name = "cots-iso",
	.n_flag = D_CLONE,	/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

static struct devnode tp_node_clts_iso = {
	.n_name = "clts-iso",
	.n_flag = D_CLONE,	/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

static struct devnode tp_node_cots_ip = {
	.n_name = "cots-ip",
	.n_flag = D_CLONE,	/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

static struct devnode tp_node_clts_ip = {
	.n_name = "clts-ip",
	.n_flag = D_CLONE,	/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

static struct devnode tp_node_cots_udp = {
	.n_name = "cots-udp",
	.n_flag = D_CLONE,	/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

static struct devnode tp_node_clts_udp = {
	.n_name = "clts-udp",
	.n_flag = D_CLONE,	/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

/**
 * tp_register_strdev: - register STREAMS pseudo-device driver
 * @major: major device number to register (0 for allocation)
 *
 * This function registers the INET pseudo-device driver and a host of minor device nodes associated
 * with the driver.  Should this function fail it returns a negative error number; otherwise, it
 * returns zero.  Only failure to register the major device number is considered an error as minor
 * device nodes in the specfs are optional.
 */
static __unlikely int
tp_register_strdev(major_t major)
{
	int err;

	if ((err = register_strdev(&tp_cdev, major)) < 0)
		return (err);
	if ((err = register_strnod(&tp_cdev, &tp_node_cots, TP_CMINOR_COTS)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not register TP_CMINOR_COTS, err = %d", err);
	if ((err = register_strnod(&tp_cdev, &tp_node_clts, TP_CMINOR_CLTS)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not register TP_CMINOR_CLTS, err = %d", err);
	if ((err = register_strnod(&tp_cdev, &tp_node_cots_iso, TP_CMINOR_COTS_ISO)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not register TP_CMINOR_COTS_ISO, err = %d", err);
	if ((err = register_strnod(&tp_cdev, &tp_node_clts_iso, TP_CMINOR_CLTS_ISO)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not register TP_CMINOR_CLTS_ISO, err = %d", err);
	if ((err = register_strnod(&tp_cdev, &tp_node_cots_ip, TP_CMINOR_COTS_IP)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not register TP_CMINOR_COTS_IP, err = %d", err);
	if ((err = register_strnod(&tp_cdev, &tp_node_clts_ip, TP_CMINOR_CLTS_IP)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not register TP_CMINOR_CLTS_IP, err = %d", err);
	if ((err = register_strnod(&tp_cdev, &tp_node_cots_udp, TP_CMINOR_COTS_UDP)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not register TP_CMINOR_COTS_UDP, err = %d", err);
	if ((err = register_strnod(&tp_cdev, &tp_node_clts_udp, TP_CMINOR_CLTS_UDP)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not register TP_CMINOR_CLTS_UDP, err = %d", err);
	return (0);
}

/**
 * tp_unregister_strdev: - unregister STREAMS pseudo-device driver
 * @major: major device number to unregister
 *
 * The function unregisters the host of minor device nodes and the major deivce node associated with
 * the driver in the reverse order in which they were allocated.  Only deregistration of the major
 * device node is considered fatal as minor device nodes were optional during initialization.
 */
static int
tp_unregister_strdev(major_t major)
{
	int err;

	if ((err = unregister_strnod(&tp_cdev, TP_CMINOR_CLTS_UDP)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not unregister TP_CMINOR_CLTS_UDP, err = %d", err);
	if ((err = unregister_strnod(&tp_cdev, TP_CMINOR_COTS_UDP)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not unregister TP_CMINOR_COTS_UDP, err = %d", err);
	if ((err = unregister_strnod(&tp_cdev, TP_CMINOR_CLTS_IP)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not unregister TP_CMINOR_CLTS_IP, err = %d", err);
	if ((err = unregister_strnod(&tp_cdev, TP_CMINOR_COTS_IP)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not unregister TP_CMINOR_COTS_IP, err = %d", err);
	if ((err = unregister_strnod(&tp_cdev, TP_CMINOR_CLTS_ISO)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not unregister TP_CMINOR_CLTS_ISO, err = %d", err);
	if ((err = unregister_strnod(&tp_cdev, TP_CMINOR_COTS_ISO)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not unregister TP_CMINOR_COTS_ISO, err = %d", err);
	if ((err = unregister_strnod(&tp_cdev, TP_CMINOR_CLTS)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not unregister TP_CMINOR_CLTS, err = %d", err);
	if ((err = unregister_strnod(&tp_cdev, TP_CMINOR_COTS)) < 0)
		strlog(major, 0, 0, SL_TRACE | SL_ERROR | SL_CONSOLE,
		       "Could not unregister TP_CMINOR_COTS, err = %d", err);
	if ((err = unregister_strdev(&tp_cdev, major)) < 0)
		return (err);
	return (0);
}

/**
 * tp_init: - initialize the INET kernel module under Linux
 */
static __init int
tp_init(void)
{
	int err;

	if ((err = tp_register_strdev(major)) < 0) {
		cmn_err(CE_WARN, "%s could not register STREAMS device, err = %d", DRV_NAME, err);
		goto error;
	}
	if (major == 0)
		major = err;
#if 0
	if ((err = tp_init_hashes())) {
		cmn_err(CE_WARN, "%s could not initialize hashes, err = %d", DRV_NAME, err);
		goto no_hashes;
	}
#endif
	if ((err = tp_init_llc())) {
		cmn_err(CE_WARN, "%s could not initialize llc, err = %d", DRV_NAME, err);
		goto no_llc;
	}
	if ((err = tp_init_notify())) {
		cmn_err(CE_WARN, "%s could not initialize notify, err = %d", DRV_NAME, err);
		goto no_notify;
	}
	if ((err = tp_init_proto())) {
		cmn_err(CE_WARN, "%s could not initialize protocols, err = %d", DRV_NAME, err);
		goto no_proto;
	}
	if ((err = tp_init_caches())) {
		cmn_err(CE_WARN, "%s could not initialize caches, err = %d", DRV_NAME, err);
		goto no_caches;
	}
	return (0);
      no_caches:
	tp_term_proto();
      no_proto:
	tp_term_notify();
      no_notify:
#if 0
	tp_term_hashes();
      no_hashes:
#endif
	tp_term_llc();
      no_llc:
	tp_unregister_strdev(major);
      error:
	return (err);
}

/**
 * tp_exit: - remove the INET kernel module under Linux
 */
static __exit void
tp_exit(void)
{
	int err;

	if ((err = tp_term_proto()))
		cmn_err(CE_WARN, "%s could not terminate protocols, err = %d", DRV_NAME, err);
	if ((err = tp_term_notify()))
		cmn_err(CE_WARN, "%s could not terminate notify, err = %d", DRV_NAME, err);
	if ((err = tp_term_llc()))
		cmn_err(CE_WARN, "%s could not terminate llc, err = %d", DRV_NAME, err);
#if 0
	if ((err = tp_term_hashes()))
		cmn_err(CE_WARN, "%s could not terminate hashes, err = %d", DRV_NAME, err);
#endif
	if ((err = tp_term_caches()))
		cmn_err(CE_WARN, "%s could not terminate caches, err = %d", DRV_NAME, err);
	if ((err = tp_unregister_strdev(major)) < 0)
		cmn_err(CE_WARN, "%s could not unregister STREAMS device, err = %d", DRV_NAME, err);
	return;
}

module_init(tp_init);
module_exit(tp_exit);

#endif				/* LINUX */
