/*****************************************************************************

 @(#) File: src/drivers/tpsn.c

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2019  Monavacon Limited <http://www.monavacon.com/>
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
#define _SUN_SOURCE	1
//#define _SUN_SOURCE   1

#include <sys/os7/compat.h>
// #include <sys/mps/ddi.h>
// #include <sys/strsun.h>

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
#define TP_COPYRIGHT	"Copyright (c) 2008-2019  Monavacon Ltd.  All Rights Reserved."
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
#define _TP_ACTION_2	 2	/* count = 0- */
#define _TP_ACTION_3	 3	/* set retransmission timer */
#define _TP_ACTION_4	 4	/* stop retransmission timer if running */
#define _TP_ACTION_5	 5	/* set window timer */
#define _TP_ACTION_6	 6	/* stop window timer if running */
#define _TP_ACTION_7	 7	/* set inactivity timer */
#define _TP_ACTION_8	 8	/* stop inactivity timer if running */
#define _TP_ACTION_9	 9	/* set initial credit for sending according to the received CR/CC-TPDU */
#define _TP_ACTION_10	10	/* set initial credit for controlling reception according to the sent
				   CR/CC-TPDU */
#define _TP_ACTION_15	15	/* send the DR-TPDU.  This DR-TPDU is sent with src-ref = local-ref and
				   dst-ref = remote-ref (may be zero) */
#define _TP_ACTION_16	16	/* send the DR-TPDU.  The DR-TPDU is sent with src-ref = 0 and dst-ref =
				   remote-ref */
#define _TP_ACTION_17	17	/* send a TPDU accourding to data transfer procedure */
#define _TP_ACTION_20	20	/* store request and exercise flow control to the user */
#define _TP_ACTION_21	21	/* send a DC-TPDU with src-ref equal to zero */
#define _TP_ACTION_22	22	/* send a DC-TPDU except if the SRC-REF field of the received DR-TDPU is
				   equal to zero */
#define _TP_ACTION_23	23	/* send a DR-TPDU with src-ref = local-ref and dst-ref = SRC-REF in CC-TPDU */

/*
 *  =========================================================================
 *
 *  Specific notes
 *
 *  =========================================================================
 */
#define _TP_NOTE_5	 5	/* not a duplicated CR-TPDU; if duplicated, ignore it */
#define _TP_NOTE_7	 7	/* as a local choice it is also possible to apply the following [0], TDISind, 
				   REFWAIT */
#define _TP_NOTE_8	 8	/* asociation to this transport connection is done regardless of the SRC-REF
				   field.  If SRC-REF is not zero, a DC-TPDU is sent back.  */
#define _TP_NOTE_9	 9	/* at least an AK-TPDU shall be sent if the transport entity is the initiator 
				   in order to ensure that the responder will complete its three-way
				   handshake. */
#define _TP_NOTE_10	10	/* if association has been made, and DST-REF is zero, then the DC-TPDU
				   contains a src-ref field set to zero */
#define _TP_NOTE_11	11	/* if the CLOSING state has been entered, coming from WFCC state, the
				   remote-ref is zero.  The SRC-REF field of the CC-TPDU is ignored (i.e. if
				   the DR-TPDU is retransmitted, it will be with the dst-ref field set to
				   zero). */
#define _TP_NOTE_13	13	/* The DR-TPDU may be either repeated immediately or when T1 will run out */
#define _TP_NOTE_15	15	/* Previously stored T-DATA or T-EXPEDITED-DATA requests are ready for
				   processing according to data transfer procedures. */
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

#define FIRST_CMINOR		0
#define TP_CMINOR_COTS		0
#define TP_CMINOR_CLTS		1
#define TP_CMINOR_COTS_ISO	2
#define TP_CMINOR_CLTS_ISO	3
#define TP_CMINOR_COTS_IP	4
#define TP_CMINOR_CLTS_IP	5
#define TP_CMINOR_COTS_UDP	6
#define TP_CMINOR_CLTS_UDP	7
#define LAST_CMINOR		7
#define FREE_CMINOR		8

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
	struct {
		struct tp_options req;	/* protocol options */
		struct tp_options res;	/* protocol options */
	} options;
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
	struct {
		mblk_t *t1;
		mblk_t *t2;
		mblk_t *t3;
		mblk_t *t4;
	} timers;
	struct {
		uint32_t t1;
		uint32_t t2;
		uint32_t t3;
		uint32_t t4;
	} config;
	int lasterror;
	int pstate;
	int flags;
	int count;
	int maximum;
	int credit;
	int opts;
	int addopts;
	int inactivity;
	int sackblks;
	int rnr;
	int rssn;
	uint16_t sref;
	uint16_t dref;
	mblk_t *sent;
	int local_choke;
};

#define _TP_RESIDERRRATE    0	/* FIXME */
#define _TP_MAX_THROUGHPUT  0	/* FIXME */
#define _TP_AVG_THROUGHPUT  0	/* FIXME */

struct tp_conind {
	/* first portion must be identical to struct tp */
	struct tp *ci_parent;		/* pointer to parent */
	volatile long ci_rflags;	/* rd queue flags */
	volatile long ci_wflags;	/* wr queue flags */
	t_uscalar_t ci_seq;		/* sequence number */
};

#define PRIV(__q) ((struct tp *)((__q)->q_ptr))

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
tp_bhashfn(unsigned short bsel)
{
	return ((tp_bhash_size - 1) & (bsel));
}

STATIC INLINE fastcall __unlikely int
tp_chashfn(unsigned short sref, unsigned short dref)
{
	return ((tp_chash_size - 1) & (sref + dref));
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

static void
tp_priv_init(struct tp *tp)
{
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
 *  OPTION Handling
 *
 *  =========================================================================
 */
static struct tp_options tp_defaults = {
	{0,}
	,
	{
	 xti_default_debug,
	 xti_default_linger,
	 xti_default_rcvbuf,
	 xti_default_rcvlowat,
	 xti_default_sndbuf,
	 xti_default_sndlowat,
	 }
	,
	{
	 t_tco_default_throughput,
	 t_tco_default_transdel,
	 t_tco_default_reserrorrate,
	 t_tco_default_transffailprob,
	 t_tco_default_estfailprob,
	 t_tco_default_relfailprob,
	 t_tco_default_estdelay,
	 t_tco_default_reldelay,
	 t_tco_default_connresil,
	 t_tco_default_protection,
	 t_tco_default_priority,
	 t_tco_default_expd,

	 t_tco_default_ltpdu,
	 t_tco_default_acktime,
	 t_tco_default_reastime,
	 t_tco_default_prefclass,
	 t_tco_default_altclass1,
	 t_tco_default_altclass2,
	 t_tco_default_altclass3,
	 t_tco_default_altclass4,
	 t_tco_default_extform,
	 t_tco_default_flowctrl,
	 t_tco_default_checksum,
	 t_tco_default_netexp,
	 t_tco_default_netrecptcf,
	 }
	,
	{
	 t_tcl_default_transdel,
	 t_tcl_default_reserrorrate,
	 t_tcl_default_protection,
	 t_tcl_default_priority,
	 t_tcl_default_checksum,
	 }
	,
	{
	 ip_default_options,
	 ip_default_tos,
	 ip_default_ttl,
	 ip_default_reuseaddr,
	 ip_default_dontroute,
	 ip_default_broadcast,
	 }
	,
	{
	 udp_default_checksum,
	 }
};

#define t_defaults tp_defaults

STATIC size_t
tp_size_opts(ulong *flags)
{
	int bit;
	size_t size;

	for (size = 0, bit = 0; bit < _T_BIT_LAST; bit++)
		if (t_tst_bit(bit, flags))
			size += tp_space[bit];
	return (size);
}

/**
 * tp_size_conn_opts: - size connection indication or confirmation options
 * @tp: private structure (locked)
 *
 * Return an option buffer size for a connection indication or confirmation.  Options without
 * end-to-end significance are not indicated and are only confirmed if requested.  Options with
 * end-to-end significance are always both indicated and confirmed.  For this to work for connection
 * indications, all request options flags must be cleared to zero.
 */
STATIC int
tp_size_conn_opts(struct tp *tp)
{
	unsigned long flags[3] = { 0, 0, 0 };
	int byte;

	for (byte = 0; byte < 3; byte++)
		flags[byte] = tp->options.req.flags[byte] | tp->options.res.flags[byte];
	return tp_size_opts(flags);
}

/**
 * t_build_conn_con_opts - Build connection confirmation options
 * @tp: transport endpoint
 * @rem: remote options (always provided)
 * @loc: local options (only if this is a confirmation)
 * @op: output buffer pointer
 * @olen: output buffer len
 *
 * These are options with end-to-end significance plus any options without end-to-end significance
 * that were requested for negotiation in the connection request.  For a connection indication, this
 * is only options with end-to-end significance.  For this to work with connection indications, all
 * options request flags must be set to zero.  The return value is the resulting size of the options
 * buffer, or a negative error number on software fault.
 *
 * The t_connect() or t_rcvconnect() functions return the values of all options with end-to-end
 * significance that were received with the connection response and the negotiated values of those
 * options without end-to-end significance that had been specified on input.  However, options
 * specified on input with t_connect() call that are not supported or refer to an unknown option
 * level are discarded and not returned on output.
 *
 * The status field of each option returned with t_connect() or t_rcvconnect() indicates if the
 * proposed value (T_SUCCESS) or a degraded value (T_PARTSUCCESS) has been negotiated.  The status
 * field of received ancillary information (for example, T_IP options) that is not subject to
 * negotiation is always set to T_SUCCESS.
 *
 * If this is a connection indication, @rem is the options passed by the remote end.
 */
static int
t_build_conn_opts(struct tp *tp, struct tp_options *rem, struct tp_options *rsp, unsigned char *op,
		  size_t olen, int indication)
{
	struct t_opthdr *oh;
	unsigned long flags[3] = { 0, }, toggles[3] = {
	0,};

	if (op == NULL || olen == 0)
		return (0);
	if (!tp)
		goto eproto;
	else {
		int i;

		for (i = 0; i < 3; i++) {
			flags[i] = rem->flags[i] | rsp->flags[i];
			toggles[i] = rem->flags[i] ^ rsp->flags[i];
		}
	}
	oh = _T_OPT_FIRSTHDR_OFS(op, olen, 0);
	if (t_tst_bit(_T_BIT_XTI_DEBUG, flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->xti.xti_debug);
		oh->level = XTI_GENERIC;
		oh->name = XTI_DEBUG;
		oh->status = T_SUCCESS;
		/* No end-to-end significance */
		/* FIXME: range check parameter */
		bcopy(rem->xti.xti_debug, T_OPT_DATA(oh), sizeof(rem->xti.xti_debug));
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_XTI_LINGER, flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->xti.xti_linger);
		oh->level = XTI_GENERIC;
		oh->name = XTI_LINGER;
		oh->status = T_SUCCESS;
		/* No end-to-end significance */
		/* FIXME: range check parameter */
		*((struct t_linger *) T_OPT_DATA(oh)) = rem->xti.xti_linger;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_XTI_RCVBUF, flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->xti.xti_rcvbuf);
		oh->level = XTI_GENERIC;
		oh->name = XTI_RCVBUF;
		oh->status = T_SUCCESS;
		/* No end-to-end significance */
		/* FIXME: range check parameter */
		*((t_uscalar_t *) T_OPT_DATA(oh)) = rem->xti.xti_rcvbuf;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_XTI_RCVLOWAT, flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->xti.xti_rcvlowat);
		oh->level = XTI_GENERIC;
		oh->name = XTI_RCVLOWAT;
		oh->status = T_SUCCESS;
		/* No end-to-end significance */
		/* FIXME: range check parameter */
		*((t_uscalar_t *) T_OPT_DATA(oh)) = rem->xti.xti_rcvlowat;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_XTI_SNDBUF, flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->xti.xti_sndbuf);
		oh->level = XTI_GENERIC;
		oh->name = XTI_SNDBUF;
		oh->status = T_SUCCESS;
		/* No end-to-end significance */
		/* FIXME: range check parameter */
		*((t_uscalar_t *) T_OPT_DATA(oh)) = rem->xti.xti_sndbuf;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_XTI_SNDLOWAT, flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->xti.xti_sndlowat);
		oh->level = XTI_GENERIC;
		oh->name = XTI_SNDLOWAT;
		oh->status = T_SUCCESS;
		/* No end-to-end significance */
		if (rem->xti.xti_sndlowat != 1) {
			if (rem->xti.xti_sndlowat != T_UNSPEC && rem->xti.xti_sndlowat < 1)
				oh->status = T_PARTSUCCESS;
			rem->xti.xti_sndlowat = 1;
		}
		*((t_uscalar_t *) T_OPT_DATA(oh)) = rem->xti.xti_sndlowat;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	switch (tp->p.info.SERV_type) {
#define toggled 0
	case T_COTS:
		if (t_tst_bit(_T_BIT_TCO_THROUGHPUT, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_throughput);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_THROUGHPUT;
			oh->status = T_SUCCESS;
			/* End-to-end significance. */
			/* Check requested but not responded, responded without request */
			if (t_tst_bit(_T_BIT_TCO_THROUGHPUT, toggled))
				oh->status = T_FAILURE;
			/* Check if we got downgraded. */
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_throughput)) T_OPT_DATA(oh)) = rem->tco.tco_throughput;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_TRANSDEL, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_transdel);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_TRANSDEL;
			oh->status = T_SUCCESS;
			if (t_tst_bit(_T_BIT_TCO_TRANSDEL, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_transdel)) T_OPT_DATA(oh)) = rem->tco.tco_transdel;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_RESERRORRATE, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_reserrorrate);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_RESERRORRATE;
			oh->status = T_SUCCESS;
			if (t_tst_bit(_T_BIT_TCO_RESERRORRATE, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_reserrorrate)) T_OPT_DATA(oh)) = rem->tco.tco_reserrorrate;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_TRANSFFAILPROB, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_transffailprob);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_TRANSFFAILPROB;
			oh->status = T_SUCCESS;
			if (t_tst_bit(_T_BIT_TCO_TRANSFFAILPROB, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_transffailprob)) T_OPT_DATA(oh)) =
			    rem->tco.tco_transffailprob;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_ESTFAILPROB, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_estfailprob);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_ESTFAILPROB;
			oh->status = T_SUCCESS;
			if (t_tst_bit(_T_BIT_TCO_ESTFAILPROB, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_estfailprob)) T_OPT_DATA(oh)) = rem->tco.tco_estfailprob;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_RELFAILPROB, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_relfailprob);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_RELFAILPROB;
			oh->status = T_SUCCESS;
			if (t_tst_bit(_T_BIT_TCO_RELFAILPROB, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_relfailprob)) T_OPT_DATA(oh)) = rem->tco.tco_relfailprob;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_ESTDELAY, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_estdelay);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_ESTDELAY;
			oh->status = T_SUCCESS;
			if (t_tst_bit(_T_BIT_TCO_ESTDELAY, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_estdelay)) T_OPT_DATA(oh)) = rem->tco.tco_estdelay;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_RELDELAY, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_reldelay);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_RELDELAY;
			oh->status = T_SUCCESS;
			if (t_tst_bit(_T_BIT_TCO_RELDELAY, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_reldelay)) T_OPT_DATA(oh)) = rem->tco.tco_reldelay;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_CONNRESIL, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_connresil);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_CONNRESIL;
			oh->status = T_SUCCESS;
			if (t_tst_bit(_T_BIT_TCO_CONNRESIL, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_connresil)) T_OPT_DATA(oh)) = rem->tco.tco_connresil;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_PROTECTION, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_protection);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_PROTECTION;
			oh->status = T_SUCCESS;
			if (t_tst_bit(_T_BIT_TCO_PROTECTION, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_protection)) T_OPT_DATA(oh)) = rem->tco.tco_protection;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_PRIORITY, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_priority);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_PRIORITY;
			oh->status = T_SUCCESS;
			if (t_tst_bit(_T_BIT_TCO_PRIORITY, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_priority)) T_OPT_DATA(oh)) = rem->tco.tco_priority;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_EXPD, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_expd);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_EXPD;
			oh->status = T_SUCCESS;
			if (t_tst_bit(_T_BIT_TCO_EXPD, toggled))
				oh->status = T_FAILURE;
			if (rem->tco.tco_expd != T_UNSPEC) {
			}
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_expd)) T_OPT_DATA(oh)) = rem->tco.tco_expd;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_LTPDU, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_ltpdu);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_LTPDU;
			oh->status = T_SUCCESS;
			if (t_tst_bit(_T_BIT_TCO_LTPDU, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_ltpdu)) T_OPT_DATA(oh)) = rem->tco.tco_ltpdu;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_ACKTIME, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_acktime);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_ACKTIME;
			oh->status = T_SUCCESS;
			if (t_tst_bit(_T_BIT_TCO_ACKTIME, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_acktime)) T_OPT_DATA(oh)) = rem->tco.tco_acktime;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_REASTIME, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_reastime);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_REASTIME;
			oh->status = T_SUCCESS;
			if (t_tst_bit(_T_BIT_TCO_REASTIME, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_reastime)) T_OPT_DATA(oh)) = rem->tco.tco_reastime;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_EXTFORM, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_extform);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_EXTFORM;
			oh->status = T_SUCCESS;
			if (t_tst_bit(_T_BIT_TCO_EXTFORM, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_extform)) T_OPT_DATA(oh)) = rem->tco.tco_extform;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_FLOWCTRL, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_flowctrl);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_FLOWCTRL;
			oh->status = T_SUCCESS;
			if (t_tst_bit(_T_BIT_TCO_FLOWCTRL, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_flowctrl)) T_OPT_DATA(oh)) = rem->tco.tco_flowctrl;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_CHECKSUM, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_checksum);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_CHECKSUM;
			oh->status = T_SUCCESS;
			if (t_tst_bit(_T_BIT_TCO_CHECKSUM, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_checksum)) T_OPT_DATA(oh)) = rem->tco.tco_checksum;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_NETEXP, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_netexp);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_NETEXP;
			oh->status = T_SUCCESS;
			if (t_tst_bit(_T_BIT_TCO_NETEXP, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_netexp)) T_OPT_DATA(oh)) = rem->tco.tco_netexp;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_NETRECPTCF, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_netrecptcf);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_NETRECPTCF;
			oh->status = T_SUCCESS;
			if (t_tst_bit(_T_BIT_TCO_NETRECPTCF, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_netrecptcf)) T_OPT_DATA(oh)) = rem->tco.tco_netrecptcf;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_SELECTACK, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_selectack);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_SELECTACK;
			oh->status = T_SUCCESS;
			if (t_tst_bit(_T_BIT_TCO_SELECTACK, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_selectack)) T_OPT_DATA(oh)) = rem->tco.tco_selectack;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_REQUESTACK, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_requestack);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_REQUESTACK;
			oh->status = T_SUCCESS;
			if (t_tst_bit(_T_BIT_TCO_REQUESTACK, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_requestack)) T_OPT_DATA(oh)) = rem->tco.tco_requestack;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_NBLKEXPDATA, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_nblkexpdata);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_NBLKEXPDATA;
			oh->status = T_SUCCESS;
			if (t_tst_bit(_T_BIT_TCO_NBLKEXPDATA, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_nblkexpdata)) T_OPT_DATA(oh)) = rem->tco.tco_nblkexpdata;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_PREFCLASS, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_prefclass);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_PREFCLASS;
			oh->status = T_SUCCESS;
			if (t_tst_bit(_T_BIT_TCO_PREFCLASS, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_prefclass)) T_OPT_DATA(oh)) = rem->tco.tco_prefclass;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_ALTCLASS1, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_altclass1);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_ALTCLASS1;
			oh->status = T_SUCCESS;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_altclass1)) T_OPT_DATA(oh)) = rem->tco.tco_altclass1;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_ALTCLASS2, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_altclass2);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_ALTCLASS2;
			oh->status = T_SUCCESS;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_altclass2)) T_OPT_DATA(oh)) = rem->tco.tco_altclass2;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_ALTCLASS3, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_altclass3);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_ALTCLASS3;
			oh->status = T_SUCCESS;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_altclass3)) T_OPT_DATA(oh)) = rem->tco.tco_altclass3;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_ALTCLASS4, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.tco_altclass4);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_ALTCLASS4;
			oh->status = T_SUCCESS;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_altclass4)) T_OPT_DATA(oh)) = rem->tco.tco_altclass4;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		break;
	case T_CLTS:
		if (t_tst_bit(_T_BIT_TCL_TRANSDEL, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tcl.tcl_transdel);
			oh->level = T_ISO_TP;
			oh->name = T_TCL_TRANSDEL;
			oh->status = T_SUCCESS;
			if (t_tst_bit(_T_BIT_TCL_TRANSDEL, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tcl.tcl_transdel)) T_OPT_DATA(oh)) = rem->tcl.tcl_transdel;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCL_RESERRORRATE, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tcl.tcl_reserrorrate);
			oh->level = T_ISO_TP;
			oh->name = T_TCL_RESERRORRATE;
			oh->status = T_SUCCESS;
			if (t_tst_bit(_T_BIT_TCL_RESERRORRATE, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tcl.tcl_reserrorrate)) T_OPT_DATA(oh)) = rem->tcl.tcl_reserrorrate;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCL_PROTECTION, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tcl.tcl_protection);
			oh->level = T_ISO_TP;
			oh->name = T_TCL_PROTECTION;
			oh->status = T_SUCCESS;
			if (t_tst_bit(_T_BIT_TCL_PROTECTION, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tcl.tcl_protection)) T_OPT_DATA(oh)) = rem->tcl.tcl_protection;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCL_PRIORITY, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tcl.tcl_priority);
			oh->level = T_ISO_TP;
			oh->name = T_TCL_PRIORITY;
			oh->status = T_SUCCESS;
			if (t_tst_bit(_T_BIT_TCL_PRIORITY, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tcl.tcl_priority)) T_OPT_DATA(oh)) = rem->tcl.tcl_priority;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCL_CHECKSUM, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tcl.tcl_checksum);
			oh->level = T_ISO_TP;
			oh->name = T_TCL_CHECKSUM;
			oh->status = T_SUCCESS;
			if (t_tst_bit(_T_BIT_TCL_CHECKSUM, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tcl.tcl_checksum)) T_OPT_DATA(oh)) = rem->tcl.tcl_checksum;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		break;
	}
	switch (tp->p.prot.type) {
	case TP_CMINOR_COTS:
	case TP_CMINOR_CLTS:
	case TP_CMINOR_COTS_UDP:
	case TP_CMINOR_CLTS_UDP:
		if (t_tst_bit(_T_BIT_UDP_CHECKSUM, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

			oh->level = T_INET_UDP;
			oh->name = T_UDP_CHECKSUM;
			oh->status = T_SUCCESS;
			/* End-to-end significance */
			if (t_tst_bit(_T_BIT_UDP_CHECKSUM, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((t_uscalar_t *) T_OPT_DATA(oh)) = rem->udp.udp_checksum;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		/* fall through */
	case TP_CMINOR_COTS_IP:
	case TP_CMINOR_CLTS_IP:
		if (t_tst_bit(_T_BIT_IP_OPTIONS, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->ip.ip_options);
			oh->level = T_INET_IP;
			oh->name = T_IP_OPTIONS;
			oh->status = T_SUCCESS;
			/* End-to-end significance */
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_IP_TOS, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->ip.ip_tos);
			oh->level = T_INET_IP;
			oh->name = T_IP_TOS;
			oh->status = T_SUCCESS;
			/* End-to-end significance */
			*((unsigned char *) T_OPT_DATA(oh)) = rem->ip.ip_tos;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_IP_TTL, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->ip.ip_ttl);
			oh->level = T_INET_IP;
			oh->name = T_IP_TTL;
			oh->status = T_SUCCESS;
			/* No end-to-end significance */
			/* FIXME: check validity of requested option */
			*((unsigned char *) T_OPT_DATA(oh)) = rem->ip.ip_ttl;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_IP_REUSEADDR, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(unsigned int);

			oh->level = T_INET_IP;
			oh->name = T_IP_REUSEADDR;
			oh->status = T_SUCCESS;
			/* No end-to-end significance */
			/* FIXME: check validity of requested option */
			*((unsigned int *) T_OPT_DATA(oh)) = rem->ip.ip_reuseaddr;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_IP_DONTROUTE, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(unsigned int);

			oh->level = T_INET_IP;
			oh->name = T_IP_DONTROUTE;
			oh->status = T_SUCCESS;
			/* No end-to-end significance */
			/* FIXME: check validity of requested option */
			*((unsigned int *) T_OPT_DATA(oh)) = rem->ip.ip_dontroute;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_IP_BROADCAST, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(unsigned int);

			oh->level = T_INET_IP;
			oh->name = T_IP_BROADCAST;
			oh->status = T_SUCCESS;
			/* No end-to-end significance */
			/* FIXME: check validity of requested option */
			*((unsigned int *) T_OPT_DATA(oh)) = rem->ip.ip_broadcast;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_IP_ADDR, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(uint32_t);

			oh->level = T_INET_IP;
			oh->name = T_IP_ADDR;
			oh->status = T_SUCCESS;
			/* No end-to-end significance */
			/* FIXME: check validity of requested option */
			*((uint32_t *) T_OPT_DATA(oh)) = rem->ip.ip_addr;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		break;
	}
	/* expect oh to be NULL (filled buffer) */
	assure(oh == NULL);
	return (olen);
	// return ((unsigned char *) oh - op); /* return actual length */
      eproto:
	LOGERR(tp, "SWERR: %s %s:%d", __FUNCTION__, __FILE__, __LINE__);
	return (-EPROTO);
      efault:
	LOGERR(tp, "SWERR: %s %s:%d", __FUNCTION__, __FILE__, __LINE__);
	return (-EFAULT);
}

#ifdef __LP64__
#undef MAX_SCHEDULE_TIMEOUT
#define MAX_SCHEDULE_TIMEOUT INT_MAX
#endif

/**
 * t_set_options: - set options on new stream
 * @tp: private structure (locked)
 *
 * This is used after accepting a new stream.  It is used to negotiate the options
 * applied to the responding stream in the connection response to the newly
 * accepted stream.  All options of interest have their flags set and the
 * appropriate option values set.
 */
static int
t_set_options(struct tp *tp)
{
	if (t_tst_bit(_T_BIT_XTI_DEBUG, tp->options.res.flags)) {
		/* absolute */
	}
	if (t_tst_bit(_T_BIT_XTI_LINGER, tp->options.res.flags)) {
		struct t_linger *valp = &tp->options.res.xti.xti_linger;

		if (valp->l_onoff == T_NO)
			valp->l_linger = T_UNSPEC;
		else {
			if (valp->l_linger == T_UNSPEC)
				valp->l_linger = t_defaults.xti.xti_linger.l_linger;
			if (valp->l_linger == T_INFINITE)
				valp->l_linger = MAX_SCHEDULE_TIMEOUT / HZ;
			if (valp->l_linger >= MAX_SCHEDULE_TIMEOUT / HZ)
				valp->l_linger = MAX_SCHEDULE_TIMEOUT / HZ;
		}
	}
	if (t_tst_bit(_T_BIT_XTI_RCVBUF, tp->options.res.flags)) {
		t_uscalar_t *valp = &tp->options.res.xti.xti_rcvbuf;

		if (*valp > sysctl_rmem_max)
			*valp = sysctl_rmem_max;
		if (*valp < SOCK_MIN_RCVBUF / 2)
			*valp = SOCK_MIN_RCVBUF / 2;
	}
	if (t_tst_bit(_T_BIT_XTI_RCVLOWAT, tp->options.res.flags)) {
		t_uscalar_t *valp = &tp->options.res.xti.xti_rcvlowat;

		if (*valp < 1)
			*valp = 1;
		if (*valp > INT_MAX)
			*valp = INT_MAX;
	}
	if (t_tst_bit(_T_BIT_XTI_SNDBUF, tp->options.res.flags)) {
		t_uscalar_t *valp = &tp->options.res.xti.xti_sndbuf;

		if (*valp > sysctl_wmem_max)
			*valp = sysctl_wmem_max;
		if (*valp < SOCK_MIN_SNDBUF / 2)
			*valp = SOCK_MIN_SNDBUF / 2;
	}
	if (t_tst_bit(_T_BIT_XTI_SNDLOWAT, tp->options.res.flags)) {
		t_uscalar_t *valp = &tp->options.res.xti.xti_sndlowat;

		if (*valp < 1)
			*valp = 1;
		if (*valp > 1)
			*valp = 1;
	}
	if (tp->p.prot.family == PF_INET) {
		if (t_tst_bit(_T_BIT_IP_OPTIONS, tp->options.res.flags)) {
			unsigned char *valp = tp->options.res.ip.ip_options;

			(void) valp;	/* FIXME */
		}
		if (t_tst_bit(_T_BIT_IP_TOS, tp->options.res.flags)) {
			unsigned char *valp = &tp->options.res.ip.ip_tos;

			(void) valp;	/* FIXME */

		}
		if (t_tst_bit(_T_BIT_IP_TTL, tp->options.res.flags)) {
			unsigned char *valp = &tp->options.res.ip.ip_ttl;

			if (*valp < 1)
				*valp = 1;

			(void) valp;	/* FIXME */
		}
		if (t_tst_bit(_T_BIT_IP_REUSEADDR, tp->options.res.flags)) {
			unsigned int *valp = &tp->options.res.ip.ip_reuseaddr;

			(void) valp;	/* FIXME */

		}
		if (t_tst_bit(_T_BIT_IP_DONTROUTE, tp->options.res.flags)) {
			unsigned int *valp = &tp->options.res.ip.ip_dontroute;

			(void) valp;	/* FIXME */

		}
		if (t_tst_bit(_T_BIT_IP_BROADCAST, tp->options.res.flags)) {
			unsigned int *valp = &tp->options.res.ip.ip_broadcast;

			(void) valp;	/* FIXME */

		}
		if (t_tst_bit(_T_BIT_IP_ADDR, tp->options.res.flags)) {
			uint32_t *valp = &tp->options.res.ip.ip_addr;

			(void) valp;	/* FIXME */

		}
		switch (tp->p.prot.protocol) {
		case T_INET_UDP:
			if (t_tst_bit(_T_BIT_UDP_CHECKSUM, tp->options.res.flags)) {
				t_uscalar_t *valp = &tp->options.res.udp.udp_checksum;

				(void) valp;	/* FIXME */

			}
			break;
		}
	}
	return (0);
}

/**
 * t_parse_conn_opts: - parse connection request or response options
 * @tp: private structure (locked)
 * @ip: input options pointer
 * @ilen: input options length
 * @request: non-zero, request; zero, response
 *
 * Only legal options can be negotiated; illegal options cause failure.  An option is illegal if the
 * following applies: 1) the length specified in t_opthdr.len exceeds the remaining size of the
 * option buffer (counted from the beginning of the option); 2) the option value is illegal: the
 * legal values are defined for each option.  If an illegal option is passed to XTI, the following
 * will happen: ... if an illegal option is passed to t_accept() or t_connect() then either the
 * function failes with t_errno set to [TBADOPT] or the connection establishment fails at a later
 * stage, depending on when the implementation detects the illegal option. ...
 *
 * If the tansport user passes multiple optiohs in one call and one of them is illegal, the call
 * fails as described above.  It is, however, possible that some or even all of the smbmitted legal
 * options were successfully negotiated.  The transport user can check the current status by a call
 * to t_optmgmt() with the T_CURRENT flag set.
 *
 * Specifying an option level unknown to the transport provider does not fail in calls to
 * t_connect(), t_accept() or t_sndudata(); the option is discarded in these cases.  The function
 * t_optmgmt() fails with [TBADOPT].
 *
 * Specifying an option name that is unknown to or not supported by the protocol selected by the
 * option level does not cause failure.  The option is discarded in calles to t_connect(),
 * t_accept() or t_sndudata().  The function t_optmgmt() returns T_NOTSUPPORT in the status field of
 * the option.
 *
 * If a transport user requests negotiation of a read-only option, or a non-privileged user requests
 * illegal access to a privileged option, the following outcomes are possible: ... 2) if negotiation
 * of a read-only option is required, t_accept() or t_connect() either fail with [TACCES], or the
 * connection establishment aborts and a T_DISCONNECT event occurs.  If the connection aborts, a
 * synchronous call to t_connect() failes with [TLOOK].  It depdends on timing an implementation
 * conditions whether a t_accept() call still succeeds or failes with [TLOOK].  If a privileged
 * option is illegally requested, the option is quietly ignored.  (A non-privileged user shall not
 * be able to select an option which is privileged or unsupported.)
 *
 * If multiple options are submitted to t_connect(), t_accept() or t_sndudata() and a read-only
 * option is rejected, the connection or the datagram transmission fails as described.  Options that
 * could be successfully negotiated before the erroneous option was processed retain their
 * negotiated values.  There is no roll-back mechanmism.
 */
static int
t_parse_conn_opts(struct tp *tp, const unsigned char *ip, size_t ilen, int request)
{
	struct t_opthdr *ih;

	/* clear flags, these flags will be used when sending a connection confirmation to determine which
	   options to include in the confirmation. */
	bzero(tp->options.req.flags, sizeof(tp->options.req.flags));
	if (ip == NULL || ilen == 0)
		return (0);
	/* For each option recognized, we test the requested value for legallity, and then set the requested
	   value in the stream's option buffer and mark the option requested in the options flags.  If it is
	   a request (and not a response), we negotiate the value to the underlying.  socket.  Once the
	   protocol has completed remote negotiation, we will determine whether the negotiation was
	   successful or partially successful.  See t_build_conn_opts(). */
	/* For connection responses, test the legality of each option and mark the option in the options
	   flags.  We do not negotiate to the socket because the final socket is not present. t_set_options() 
	   will read the flags and negotiate to the final socket after the connection has been accepted. */
	for (ih = _T_OPT_FIRSTHDR_OFS(ip, ilen, 0); ih; ih = _T_OPT_NEXTHDR_OFS(ip, ilen, ih, 0)) {
		if (ih->len < sizeof(*ih))
			goto einval;
		switch (ih->level) {
		case XTI_GENERIC:
			switch (ih->name) {
			case XTI_DEBUG:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (((ih->len - sizeof(*ih)) % sizeof(t_uscalar_t)) != 0)
					goto einval;
				if (ih->len >= sizeof(*ih) + 4 * sizeof(t_uscalar_t))
					tp->options.req.xti.xti_debug[3] = valp[3];
				else
					tp->options.req.xti.xti_debug[3] = 0;
				if (ih->len >= sizeof(*ih) + 3 * sizeof(t_uscalar_t))
					tp->options.req.xti.xti_debug[2] = valp[2];
				else
					tp->options.req.xti.xti_debug[2] = 0;
				if (ih->len >= sizeof(*ih) + 2 * sizeof(t_uscalar_t))
					tp->options.req.xti.xti_debug[1] = valp[1];
				else
					tp->options.req.xti.xti_debug[1] = 0;
				if (ih->len >= sizeof(*ih) + 1 * sizeof(t_uscalar_t))
					tp->options.req.xti.xti_debug[0] = valp[0];
				else
					tp->options.req.xti.xti_debug[1] = 0;
				t_set_bit(_T_BIT_XTI_DEBUG, tp->options.req.flags);
				continue;
			}
			case XTI_LINGER:
			{
				struct t_linger *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				if (valp->l_onoff != T_NO && valp->l_onoff != T_YES)
					goto einval;
				if (valp->l_linger != T_INFINITE && valp->l_linger != T_UNSPEC
				    && valp->l_linger < 0)
					goto einval;
				tp->options.req.xti.xti_linger = *valp;
				t_set_bit(_T_BIT_XTI_LINGER, tp->options.req.flags);
				if (!request)
					continue;
				if (valp->l_onoff == T_NO)
					valp->l_linger = T_UNSPEC;
				else {
					if (valp->l_linger == T_UNSPEC)
						valp->l_linger = t_defaults.xti.xti_linger.l_linger;
					if (valp->l_linger == T_INFINITE)
						valp->l_linger = MAX_SCHEDULE_TIMEOUT / HZ;
					if (valp->l_linger >= MAX_SCHEDULE_TIMEOUT / HZ)
						valp->l_linger = MAX_SCHEDULE_TIMEOUT / HZ;
				}
				continue;
			}
			case XTI_RCVBUF:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				tp->options.req.xti.xti_rcvbuf = *valp;
				t_set_bit(_T_BIT_XTI_RCVBUF, tp->options.req.flags);
				if (!request)
					continue;
				if (*valp > sysctl_rmem_max)
					*valp = sysctl_rmem_max;
				if (*valp < SOCK_MIN_RCVBUF / 2)
					*valp = SOCK_MIN_RCVBUF / 2;
				continue;
			}
			case XTI_RCVLOWAT:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				tp->options.req.xti.xti_rcvlowat = *valp;
				t_set_bit(_T_BIT_XTI_RCVLOWAT, tp->options.req.flags);
				if (!request)
					continue;
				if (*valp < 1)
					*valp = 1;
				if (*valp > INT_MAX)
					*valp = INT_MAX;
				continue;
			}
			case XTI_SNDBUF:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				tp->options.req.xti.xti_sndbuf = *valp;
				t_set_bit(_T_BIT_XTI_SNDBUF, tp->options.req.flags);
				if (!request)
					continue;
				if (*valp > sysctl_wmem_max)
					*valp = sysctl_wmem_max;
				if (*valp < SOCK_MIN_SNDBUF / 2)
					*valp = SOCK_MIN_SNDBUF / 2;
				continue;
			}
			case XTI_SNDLOWAT:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				tp->options.req.xti.xti_sndlowat = *valp;
				t_set_bit(_T_BIT_XTI_SNDLOWAT, tp->options.req.flags);
				if (!request)
					continue;
				if (*valp < 1)
					*valp = 1;
				if (*valp > 1)
					*valp = 1;
				continue;
			}
			}
			continue;
		case T_ISO_TP:
			switch (tp->p.info.SERV_type) {
			case T_COTS:
				switch (ih->name) {
				case T_TCO_THROUGHPUT:
				{
					struct thrpt *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					/* FIXME: check values */
					tp->options.req.tco.tco_throughput = *valp;
					t_set_bit(_T_BIT_TCO_THROUGHPUT, tp->options.req.flags);
					if (!request)
						continue;
					continue;
				}
				case T_TCO_TRANSDEL:
				{
					struct transdel *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					/* FIXME: check values */
					tp->options.req.tco.tco_transdel = *valp;
					t_set_bit(_T_BIT_TCO_TRANSDEL, tp->options.req.flags);
					if (!request)
						continue;
					continue;
				}
				case T_TCO_RESERRORRATE:
				{
					struct rate *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					/* FIXME: check values */
					tp->options.req.tco.tco_reserrorrate = *valp;
					t_set_bit(_T_BIT_TCO_RESERRORRATE, tp->options.req.flags);
					if (!request)
						continue;
					continue;
				}
				case T_TCO_TRANSFFAILPROB:
				{
					struct rate *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					/* FIXME: check values */
					tp->options.req.tco.tco_transffailprob = *valp;
					t_set_bit(_T_BIT_TCO_TRANSFFAILPROB, tp->options.req.flags);
					if (!request)
						continue;
					continue;
				}
				case T_TCO_ESTFAILPROB:
				{
					struct rate *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					/* FIXME: check values */
					tp->options.req.tco.tco_estfailprob = *valp;
					t_set_bit(_T_BIT_TCO_ESTFAILPROB, tp->options.req.flags);
					if (!request)
						continue;
					continue;
				}
				case T_TCO_RELFAILPROB:
				{
					struct rate *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					/* FIXME: check values */
					tp->options.req.tco.tco_relfailprob = *valp;
					t_set_bit(_T_BIT_TCO_RELFAILPROB, tp->options.req.flags);
					if (!request)
						continue;
					continue;
				}
				case T_TCO_ESTDELAY:
				{
					struct rate *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					/* FIXME: check values */
					tp->options.req.tco.tco_estdelay = *valp;
					t_set_bit(_T_BIT_TCO_ESTDELAY, tp->options.req.flags);
					if (!request)
						continue;
					continue;
				}
				case T_TCO_RELDELAY:
				{
					struct rate *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					/* FIXME: check values */
					tp->options.req.tco.tco_reldelay = *valp;
					t_set_bit(_T_BIT_TCO_RELDELAY, tp->options.req.flags);
					if (!request)
						continue;
					continue;
				}
				case T_TCO_CONNRESIL:
				{
					struct rate *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					/* FIXME: check values */
					tp->options.req.tco.tco_connresil = *valp;
					t_set_bit(_T_BIT_TCO_CONNRESIL, tp->options.req.flags);
					if (!request)
						continue;
					continue;
				}
				case T_TCO_PROTECTION:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					if (*valp != T_NOPROTECT && *valp != T_PASSIVEPROTECT
					    && *valp != T_ACTIVEPROTECT)
						goto einval;
					if (*valp != T_NOPROTECT)
						goto eacces;
					tp->options.req.tco.tco_protection = *valp;
					t_set_bit(_T_BIT_TCO_PROTECTION, tp->options.req.flags);
					if (!request)
						continue;
					continue;
				}
				case T_TCO_PRIORITY:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					if (*valp != T_PRITOP && *valp != T_PRIHIGH && *valp != T_PRIMID
					    && *valp != T_PRILOW && *valp != T_PRIDFLT)
						goto einval;
					tp->options.req.tco.tco_priority = *valp;
					t_set_bit(_T_BIT_TCO_PRIORITY, tp->options.req.flags);
					if (!request)
						continue;
					continue;
				}
				case T_TCO_EXPD:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					if (*valp != T_YES && *valp != T_NO)
						goto einval;
					tp->options.req.tco.tco_expd = *valp;
					t_set_bit(_T_BIT_TCO_EXPD, tp->options.req.flags);
					if (!request)
						continue;
					continue;
				}
				case T_TCO_LTPDU:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					/* FIXME: check values */
					tp->options.req.tco.tco_ltpdu = *valp;
					t_set_bit(_T_BIT_TCO_LTPDU, tp->options.req.flags);
					if (!request)
						continue;
					continue;
				}
				case T_TCO_ACKTIME:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					/* FIXME: check values */
					tp->options.req.tco.tco_acktime = *valp;
					t_set_bit(_T_BIT_TCO_ACKTIME, tp->options.req.flags);
					if (!request)
						continue;
					if (*valp < 10)
						*valp = 10;
					if (*valp > INT_MAX)
						*valp = INT_MAX;
					continue;
				}
				case T_TCO_REASTIME:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					/* FIXME: check values */
					tp->options.req.tco.tco_reastime = *valp;
					t_set_bit(_T_BIT_TCO_REASTIME, tp->options.req.flags);
					if (!request)
						continue;
					if (*valp < 10)
						*valp = 10;
					if (*valp > INT_MAX)
						*valp = INT_MAX;
					continue;
				}
				case T_TCO_PREFCLASS:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					if (*valp != T_CLASS0 && *valp != T_CLASS1 && *valp != T_CLASS2
					    && *valp != T_CLASS3 && *valp != T_CLASS4)
						goto einval;
					if (*valp != T_CLASS4)
						goto einval;
					tp->options.req.tco.tco_prefclass = *valp;
					t_set_bit(_T_BIT_TCO_PREFCLASS, tp->options.req.flags);
					if (!request)
						continue;
					continue;
				}
				case T_TCO_ALTCLASS1:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					if (*valp != T_UNSPEC && *valp != T_CLASS0 && *valp != T_CLASS1
					    && *valp != T_CLASS2 && *valp != T_CLASS3 && *valp != T_CLASS4)
						goto einval;
					if (*valp != T_UNSPEC)
						goto einval;
					tp->options.req.tco.tco_altclass1 = *valp;
					t_set_bit(_T_BIT_TCO_ALTCLASS1, tp->options.req.flags);
					if (!request)
						continue;
					continue;
				}
				case T_TCO_ALTCLASS2:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					if (*valp != T_UNSPEC && *valp != T_CLASS0 && *valp != T_CLASS1
					    && *valp != T_CLASS2 && *valp != T_CLASS3 && *valp != T_CLASS4)
						goto einval;
					if (*valp != T_UNSPEC)
						goto einval;
					tp->options.req.tco.tco_altclass2 = *valp;
					t_set_bit(_T_BIT_TCO_ALTCLASS2, tp->options.req.flags);
					if (!request)
						continue;
					continue;
				}
				case T_TCO_ALTCLASS3:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					if (*valp != T_UNSPEC && *valp != T_CLASS0 && *valp != T_CLASS1
					    && *valp != T_CLASS2 && *valp != T_CLASS3 && *valp != T_CLASS4)
						goto einval;
					if (*valp != T_UNSPEC)
						goto einval;
					tp->options.req.tco.tco_altclass3 = *valp;
					t_set_bit(_T_BIT_TCO_ALTCLASS3, tp->options.req.flags);
					if (!request)
						continue;
					continue;
				}
				case T_TCO_ALTCLASS4:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					if (*valp != T_UNSPEC && *valp != T_CLASS0 && *valp != T_CLASS1
					    && *valp != T_CLASS2 && *valp != T_CLASS3 && *valp != T_CLASS4)
						goto einval;
					if (*valp != T_UNSPEC)
						goto einval;
					tp->options.req.tco.tco_altclass4 = *valp;
					t_set_bit(_T_BIT_TCO_ALTCLASS4, tp->options.req.flags);
					if (!request)
						continue;
					continue;
				}
				case T_TCO_EXTFORM:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					if (*valp != T_YES && *valp != T_NO && *valp != T_UNSPEC)
						goto einval;
					if (*valp != T_UNSPEC)
						tp->options.req.tco.tco_extform = *valp;
					t_set_bit(_T_BIT_TCO_EXTFORM, tp->options.req.flags);
					if (!request)
						continue;
					continue;
				}
				case T_TCO_FLOWCTRL:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					if (*valp != T_YES && *valp != T_NO && *valp != T_UNSPEC)
						goto einval;
					if (*valp != T_UNSPEC)
						tp->options.req.tco.tco_flowctrl = *valp;
					t_set_bit(_T_BIT_TCO_FLOWCTRL, tp->options.req.flags);
					if (!request)
						continue;
					continue;
				}
				case T_TCO_CHECKSUM:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					if (*valp != T_YES && *valp != T_NO && *valp != T_UNSPEC)
						goto einval;
					if (*valp != T_UNSPEC)
						tp->options.req.tco.tco_checksum = *valp;
					t_set_bit(_T_BIT_TCO_CHECKSUM, tp->options.req.flags);
					if (!request)
						continue;
					continue;
				}
				case T_TCO_NETEXP:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					if (*valp != T_YES && *valp != T_NO && *valp != T_UNSPEC)
						goto einval;
					if (*valp != T_UNSPEC)
						tp->options.req.tco.tco_netexp = *valp;
					t_set_bit(_T_BIT_TCO_NETEXP, tp->options.req.flags);
					if (!request)
						continue;
					continue;
				}
				case T_TCO_NETRECPTCF:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					if (*valp != T_YES && *valp != T_NO && *valp != T_UNSPEC)
						goto einval;
					if (*valp != T_UNSPEC)
						tp->options.req.tco.tco_netrecptcf = *valp;
					t_set_bit(_T_BIT_TCO_NETRECPTCF, tp->options.req.flags);
					if (!request)
						continue;
					continue;
				}
				case T_TCO_SELECTACK:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					if (*valp != T_YES && *valp != T_NO && *valp != T_UNSPEC)
						goto einval;
					if (*valp != T_UNSPEC)
						tp->options.req.tco.tco_selectack = *valp;
					t_set_bit(_T_BIT_TCO_SELECTACK, tp->options.req.flags);
					if (!request)
						continue;
					continue;
				}
				case T_TCO_REQUESTACK:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					if (*valp != T_YES && *valp != T_NO && *valp != T_UNSPEC)
						goto einval;
					if (*valp != T_UNSPEC)
						tp->options.req.tco.tco_requestack = *valp;
					t_set_bit(_T_BIT_TCO_REQUESTACK, tp->options.req.flags);
					if (!request)
						continue;
					continue;
				}
				case T_TCO_NBLKEXPDATA:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					if (*valp != T_YES && *valp != T_NO && *valp != T_UNSPEC)
						goto einval;
					if (*valp != T_UNSPEC)
						tp->options.req.tco.tco_nblkexpdata = *valp;
					t_set_bit(_T_BIT_TCO_NBLKEXPDATA, tp->options.req.flags);
					if (!request)
						continue;
					continue;
				}
				}
				continue;
			case T_CLTS:
				switch (ih->name) {
				case T_TCL_TRANSDEL:
				{
					struct transdel *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					/* FIXME: check values */
					tp->options.req.tcl.tcl_transdel = *valp;
					t_set_bit(_T_BIT_TCL_TRANSDEL, tp->options.req.flags);
					if (!request)
						continue;
					continue;
				}
				case T_TCL_RESERRORRATE:
				{
					struct rate *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					/* FIXME: check values */
					tp->options.req.tcl.tcl_reserrorrate = *valp;
					t_set_bit(_T_BIT_TCL_RESERRORRATE, tp->options.req.flags);
					if (!request)
						continue;
					continue;
				}
				case T_TCL_PROTECTION:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					if (*valp != T_NOPROTECT && *valp != T_PASSIVEPROTECT
					    && *valp != T_ACTIVEPROTECT)
						goto einval;
					if (*valp != T_NOPROTECT)
						goto eacces;
					tp->options.req.tcl.tcl_protection = *valp;
					t_set_bit(_T_BIT_TCL_PROTECTION, tp->options.req.flags);
					if (!request)
						continue;
					continue;
				}
				case T_TCL_PRIORITY:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					if (*valp != T_PRITOP && *valp != T_PRIHIGH && *valp != T_PRIMID
					    && *valp != T_PRILOW && *valp != T_PRIDFLT)
						goto einval;
					tp->options.req.tcl.tcl_priority = *valp;
					t_set_bit(_T_BIT_TCL_PRIORITY, tp->options.req.flags);
					if (!request)
						continue;
					continue;
				}
				case T_TCL_CHECKSUM:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len - sizeof(*ih) != sizeof(*valp))
						goto einval;
					if (*valp != T_YES && *valp != T_NO && *valp != T_UNSPEC)
						goto einval;
					if (*valp != T_UNSPEC)
						tp->options.req.tcl.tcl_checksum = *valp;
					t_set_bit(_T_BIT_TCL_CHECKSUM, tp->options.req.flags);
					if (!request)
						continue;
					continue;
				}
				}
				continue;
			}
			continue;
		case T_INET_IP:
			switch (tp->p.prot.type) {
			case TP_CMINOR_COTS:
			case TP_CMINOR_CLTS:
			case TP_CMINOR_COTS_IP:
			case TP_CMINOR_CLTS_IP:
			case TP_CMINOR_COTS_UDP:
			case TP_CMINOR_CLTS_UDP:
				break;
			default:
				continue;
			}
			switch (ih->name) {
			case T_IP_OPTIONS:
			{
				unsigned char *valp = (typeof(valp)) T_OPT_DATA(ih);

				(void) valp;	/* FIXME */
				t_set_bit(_T_BIT_IP_OPTIONS, tp->options.req.flags);
				continue;
			}
			case T_IP_TOS:
			{
				unsigned char *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				tp->options.req.ip.ip_tos = *valp;
				t_set_bit(_T_BIT_IP_TOS, tp->options.req.flags);
				continue;
			}
			case T_IP_TTL:
			{
				unsigned char *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				tp->options.req.ip.ip_ttl = *valp;
				t_set_bit(_T_BIT_IP_TTL, tp->options.req.flags);
				continue;
			}
			case T_IP_REUSEADDR:
			{
				unsigned int *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				if (*valp != T_NO && *valp != T_YES)
					goto einval;
				tp->options.req.ip.ip_reuseaddr = *valp;
				t_set_bit(_T_BIT_IP_REUSEADDR, tp->options.req.flags);
				continue;
			}
			case T_IP_DONTROUTE:
			{
				unsigned int *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				if (*valp != T_NO && *valp != T_YES)
					goto einval;
				tp->options.req.ip.ip_dontroute = *valp;
				t_set_bit(_T_BIT_IP_DONTROUTE, tp->options.req.flags);
				continue;
			}
			case T_IP_BROADCAST:
			{
				unsigned int *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				if (*valp != T_NO && *valp != T_YES)
					goto einval;
				tp->options.req.ip.ip_broadcast = *valp;
				t_set_bit(_T_BIT_IP_BROADCAST, tp->options.req.flags);
				continue;
			}
			case T_IP_ADDR:
			{
				uint32_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				tp->options.req.ip.ip_addr = *valp;
				t_set_bit(_T_BIT_IP_ADDR, tp->options.req.flags);
				continue;
			}
			}
			continue;
		case T_INET_UDP:
			switch (tp->p.prot.type) {
			case TP_CMINOR_COTS:
			case TP_CMINOR_CLTS:
			case TP_CMINOR_COTS_UDP:
			case TP_CMINOR_CLTS_UDP:
				break;
			default:
				continue;
			}
			switch (ih->name) {
			case T_UDP_CHECKSUM:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				if (*valp != T_NO && *valp != T_YES)
					goto einval;
				tp->options.req.udp.udp_checksum = *valp;
				t_set_bit(_T_BIT_UDP_CHECKSUM, tp->options.req.flags);
				continue;
			}
			}
			continue;
		}
	}
	return (0);
      einval:
	return (-EINVAL);
      eacces:
	return (-EACCES);
#if 0
      eproto:
	LOGERR(tp, "SWERR: %s %s:%d", __FUNCTION__, __FILE__, __LINE__);
	return (-EPROTO);
#endif
}

/**
 * t_size_default_options: - size options
 * @t: private structure (locked)
 * @ip: input options pointer
 * @ilen: input options length
 *
 * Check the validity of the options structure, check for correct size of each supplied option given
 * the option management flag, and return the size required of the acknowledgement options field.
 *
 * Returns the size (positive) of the options necessary to respond to a T_DEFAULT options management
 * request, or an error (negative).
 */
static int
t_size_default_options(const struct tp *t, const unsigned char *ip, size_t ilen)
{
	int olen = 0, optlen;
	const struct t_opthdr *ih;
	struct t_opthdr all;

	if (ip == NULL || ilen == 0) {
		/* For zero-length options fake an option header for all names with all levels */
		all.level = T_ALLLEVELS;
		all.name = T_ALLOPT;
		all.len = sizeof(all);
		all.status = 0;
		ip = (const unsigned char *) &all;
		ilen = sizeof(all);
	}
	for (ih = _T_OPT_FIRSTHDR_OFS(ip, ilen, 0); ih; ih = _T_OPT_NEXTHDR_OFS(ip, ilen, ih, 0)) {
		if (ih->len < sizeof(*ih))
			goto einval;
		if ((unsigned char *) ih + ih->len > ip + ilen)
			goto einval;
		optlen = ih->len - sizeof(*ih);
		(void) optlen;
		switch (ih->level) {
		default:
			goto einval;
		case T_ALLLEVELS:
			if (ih->name != T_ALLOPT)
				goto einval;
			/* fall through */
		case XTI_GENERIC:
			switch (ih->name) {
			default:
				olen += T_SPACE(0);
				continue;
			case T_ALLOPT:
			case XTI_DEBUG:
				olen += _T_SPACE_SIZEOF(t_defaults.xti.xti_debug);
				if (ih->name != T_ALLOPT)
					continue;
				/* fall through */
			case XTI_LINGER:
				olen += _T_SPACE_SIZEOF(t_defaults.xti.xti_linger);
				if (ih->name != T_ALLOPT)
					continue;
				/* fall through */
			case XTI_RCVBUF:
				olen += _T_SPACE_SIZEOF(t_defaults.xti.xti_rcvbuf);
				if (ih->name != T_ALLOPT)
					continue;
				/* fall through */
			case XTI_RCVLOWAT:
				olen += _T_SPACE_SIZEOF(t_defaults.xti.xti_rcvlowat);
				if (ih->name != T_ALLOPT)
					continue;
				/* fall through */
			case XTI_SNDBUF:
				olen += _T_SPACE_SIZEOF(t_defaults.xti.xti_sndbuf);
				if (ih->name != T_ALLOPT)
					continue;
				/* fall through */
			case XTI_SNDLOWAT:
				olen += _T_SPACE_SIZEOF(t_defaults.xti.xti_sndlowat);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
			/* fall through */
		case T_ISO_TP:
			switch (t->p.prot.type) {
			case TP_CMINOR_COTS:
			case TP_CMINOR_CLTS:
			case TP_CMINOR_COTS_ISO:
			case TP_CMINOR_CLTS_ISO:
				switch (t->p.info.SERV_type) {
				case T_COTS:
					switch (ih->name) {
					default:
						olen += T_SPACE(0);
						continue;
					case T_ALLOPT:
					case T_TCO_THROUGHPUT:
						olen += _T_SPACE_SIZEOF(t_defaults.tco.tco_throughput);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_TRANSDEL:
						olen += _T_SPACE_SIZEOF(t_defaults.tco.tco_transdel);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_RESERRORRATE:
						olen += _T_SPACE_SIZEOF(t_defaults.tco.tco_reserrorrate);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_TRANSFFAILPROB:
						olen += _T_SPACE_SIZEOF(t_defaults.tco.tco_transffailprob);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_ESTFAILPROB:
						olen += _T_SPACE_SIZEOF(t_defaults.tco.tco_estfailprob);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_RELFAILPROB:
						olen += _T_SPACE_SIZEOF(t_defaults.tco.tco_relfailprob);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_ESTDELAY:
						olen += _T_SPACE_SIZEOF(t_defaults.tco.tco_estdelay);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_RELDELAY:
						olen += _T_SPACE_SIZEOF(t_defaults.tco.tco_reldelay);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_CONNRESIL:
						olen += _T_SPACE_SIZEOF(t_defaults.tco.tco_connresil);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_PROTECTION:
						olen += _T_SPACE_SIZEOF(t_defaults.tco.tco_protection);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_PRIORITY:
						olen += _T_SPACE_SIZEOF(t_defaults.tco.tco_priority);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_EXPD:
						olen += _T_SPACE_SIZEOF(t_defaults.tco.tco_expd);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_LTPDU:
						olen += _T_SPACE_SIZEOF(t_defaults.tco.tco_ltpdu);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_ACKTIME:
						olen += _T_SPACE_SIZEOF(t_defaults.tco.tco_acktime);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_REASTIME:
						olen += _T_SPACE_SIZEOF(t_defaults.tco.tco_reastime);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_PREFCLASS:
						olen += _T_SPACE_SIZEOF(t_defaults.tco.tco_prefclass);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_ALTCLASS1:
						olen += _T_SPACE_SIZEOF(t_defaults.tco.tco_altclass1);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_ALTCLASS2:
						olen += _T_SPACE_SIZEOF(t_defaults.tco.tco_altclass2);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_ALTCLASS3:
						olen += _T_SPACE_SIZEOF(t_defaults.tco.tco_altclass3);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_ALTCLASS4:
						olen += _T_SPACE_SIZEOF(t_defaults.tco.tco_altclass4);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_EXTFORM:
						olen += _T_SPACE_SIZEOF(t_defaults.tco.tco_extform);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_FLOWCTRL:
						olen += _T_SPACE_SIZEOF(t_defaults.tco.tco_flowctrl);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_CHECKSUM:
						olen += _T_SPACE_SIZEOF(t_defaults.tco.tco_checksum);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_NETEXP:
						olen += _T_SPACE_SIZEOF(t_defaults.tco.tco_netexp);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_NETRECPTCF:
						olen += _T_SPACE_SIZEOF(t_defaults.tco.tco_netrecptcf);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_SELECTACK:
						olen += _T_SPACE_SIZEOF(t_defaults.tco.tco_selectack);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_REQUESTACK:
						olen += _T_SPACE_SIZEOF(t_defaults.tco.tco_requestack);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_NBLKEXPDATA:
						olen += _T_SPACE_SIZEOF(t_defaults.tco.tco_nblkexpdata);
						if (ih->name != T_ALLOPT)
							continue;
					}
					break;
				case T_CLTS:
					switch (ih->name) {
					default:
						olen += T_SPACE(0);
						continue;
					case T_ALLOPT:
					case T_TCL_TRANSDEL:
						olen += _T_SPACE_SIZEOF(t_defaults.tcl.tcl_transdel);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCL_RESERRORRATE:
						olen += _T_SPACE_SIZEOF(t_defaults.tcl.tcl_reserrorrate);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCL_PROTECTION:
						olen += _T_SPACE_SIZEOF(t_defaults.tcl.tcl_protection);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCL_PRIORITY:
						olen += _T_SPACE_SIZEOF(t_defaults.tcl.tcl_priority);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCL_CHECKSUM:
						olen += _T_SPACE_SIZEOF(t_defaults.tcl.tcl_checksum);
						if (ih->name != T_ALLOPT)
							continue;
					}
					break;
				}
				if (ih->level != T_ALLLEVELS)
					continue;
				break;
			default:
				if (ih->level != T_ALLLEVELS)
					goto einval;
				break;
			}
			/* fall through */
		case T_INET_IP:
			switch (t->p.prot.type) {
			case TP_CMINOR_COTS:
			case TP_CMINOR_CLTS:
			case TP_CMINOR_COTS_IP:
			case TP_CMINOR_CLTS_IP:
			case TP_CMINOR_COTS_UDP:
			case TP_CMINOR_CLTS_UDP:
				switch (ih->name) {
				default:
					olen += T_SPACE(0);
					continue;
				case T_ALLOPT:
				case T_IP_OPTIONS:
					/* not supported yet */
					olen += T_SPACE(0);
					if (ih->name != T_ALLOPT)
						continue;
					/* fall through */
				case T_IP_TOS:
					olen += _T_SPACE_SIZEOF(t_defaults.ip.ip_tos);
					if (ih->name != T_ALLOPT)
						continue;
					/* fall through */
				case T_IP_TTL:
					olen += _T_SPACE_SIZEOF(t_defaults.ip.ip_ttl);
					if (ih->name != T_ALLOPT)
						continue;
					/* fall through */
				case T_IP_REUSEADDR:
					olen += _T_SPACE_SIZEOF(t_defaults.ip.ip_reuseaddr);
					if (ih->name != T_ALLOPT)
						continue;
					/* fall through */
				case T_IP_DONTROUTE:
					olen += _T_SPACE_SIZEOF(t_defaults.ip.ip_dontroute);
					if (ih->name != T_ALLOPT)
						continue;
					/* fall through */
				case T_IP_BROADCAST:
					olen += _T_SPACE_SIZEOF(t_defaults.ip.ip_broadcast);
					if (ih->name != T_ALLOPT)
						continue;
					/* fall through */
				case T_IP_ADDR:
					olen += _T_SPACE_SIZEOF(t_defaults.ip.ip_addr);
					if (ih->name != T_ALLOPT)
						continue;
				}
				if (ih->level != T_ALLLEVELS)
					continue;
				break;
			default:
				if (ih->level != T_ALLLEVELS)
					goto einval;
				break;
			}
			/* fall through */
		case T_INET_UDP:
			switch (t->p.prot.type) {
			case TP_CMINOR_COTS:
			case TP_CMINOR_CLTS:
			case TP_CMINOR_COTS_UDP:
			case TP_CMINOR_CLTS_UDP:
				switch (ih->name) {
				default:
					olen += T_SPACE(0);
					continue;
				case T_ALLOPT:
				case T_UDP_CHECKSUM:
					olen += _T_SPACE_SIZEOF(t_defaults.udp.udp_checksum);
					if (ih->name != T_ALLOPT)
						continue;
				}
				if (ih->level != T_ALLLEVELS)
					continue;
				break;
			default:
				if (ih->level != T_ALLLEVELS)
					goto einval;
				break;
			}
		}
	}
	LOGIO(t, "Calculated option output size = %u", olen);
	return (olen);
      einval:
	LOGNO(t, "ERROR: Invalid input options");
	return (-EINVAL);
}

/**
 * t_size_current_options: - size options
 * @tp: private structure (locked)
 * @ip: input options pointer
 * @ilen: input options length
 *
 * Check the validity of the options structure, check for correct size of each supplied option given
 * the option management flag, and return the size required of the acknowledgement options field.
 *
 * Returns the size (positive) of the options necessary to respond to a T_CURRENT options management
 * request, or an error (negative).
 */
static int
t_size_current_options(const struct tp *t, const unsigned char *ip, size_t ilen)
{
	return t_size_default_options(t, ip, ilen);
}

/**
 * t_size_check_options: - size options
 * @tp: private structure (locked)
 * @ip: input options pointer
 * @ilen: input options length
 *
 * Check the validity of the options structure, check for correct size of each supplied option given
 * the option management flag, and return the size required of the acknowledgement options field.
 *
 * Returns the size (positive) of the options necessary to respond to a T_CHECK options management
 * request, or an error (negative).
 */
static int
t_size_check_options(const struct tp *t, const unsigned char *ip, size_t ilen)
{
	int olen = 0, optlen;
	const struct t_opthdr *ih;
	struct t_opthdr all;

	if (ip == NULL || ilen == 0) {
		/* For zero-length options fake an option header for all names with all levels */
		all.level = T_ALLLEVELS;
		all.name = T_ALLOPT;
		all.len = sizeof(all);
		all.status = 0;
		ip = (unsigned char *) &all;
		ilen = sizeof(all);
	}
	for (ih = _T_OPT_FIRSTHDR_OFS(ip, ilen, 0); ih; ih = _T_OPT_NEXTHDR_OFS(ip, ilen, ih, 0)) {
		if (ih->len < sizeof(*ih))
			goto einval;
		if ((unsigned char *) ih + ih->len > ip + ilen)
			goto einval;
		optlen = ih->len - sizeof(*ih);
		switch (ih->level) {
		default:
			goto einval;
		case T_ALLLEVELS:
			if (ih->name != T_ALLOPT)
				goto einval;
			/* fall through */
		case XTI_GENERIC:
			switch (ih->name) {
			default:
				olen += T_SPACE(optlen);
				continue;
			case T_ALLOPT:
			case XTI_DEBUG:
				/* can be any non-zero array of t_uscalar_t */
				if (optlen
				    && ((optlen % sizeof(t_uscalar_t)) != 0
					|| optlen > 4 * sizeof(t_uscalar_t)))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
				/* fall through */
			case XTI_LINGER:
				if (optlen && optlen != sizeof(t->options.req.xti.xti_linger))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
				/* fall through */
			case XTI_RCVBUF:
				if (optlen && optlen != sizeof(t->options.req.xti.xti_rcvbuf))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
				/* fall through */
			case XTI_RCVLOWAT:
				if (optlen && optlen != sizeof(t->options.req.xti.xti_rcvlowat))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
				/* fall through */
			case XTI_SNDBUF:
				if (optlen && optlen != sizeof(t->options.req.xti.xti_sndbuf))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
				/* fall through */
			case XTI_SNDLOWAT:
				if (optlen && optlen != sizeof(t->options.req.xti.xti_sndlowat))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
			/* fall through */
		case T_ISO_TP:
			switch (t->p.prot.type) {
			case TP_CMINOR_COTS:
			case TP_CMINOR_CLTS:
			case TP_CMINOR_COTS_ISO:
			case TP_CMINOR_CLTS_ISO:
				switch (t->p.info.SERV_type) {
				case T_COTS:
					switch (ih->name) {
					default:
						olen += T_SPACE(0);
						continue;
					case T_ALLOPT:
					case T_TCO_THROUGHPUT:
						if (optlen
						    && optlen != sizeof(t->options.req.tco.tco_throughput))
							goto einval;
						olen += T_SPACE(optlen);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_TRANSDEL:
						if (optlen
						    && optlen != sizeof(t->options.req.tco.tco_transdel))
							goto einval;
						olen += T_SPACE(optlen);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_RESERRORRATE:
						if (optlen
						    && optlen != sizeof(t->options.req.tco.tco_reserrorrate))
							goto einval;
						olen += T_SPACE(optlen);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_TRANSFFAILPROB:
						if (optlen
						    && optlen !=
						    sizeof(t->options.req.tco.tco_transffailprob))
							goto einval;
						olen += T_SPACE(optlen);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_ESTFAILPROB:
						if (optlen
						    && optlen != sizeof(t->options.req.tco.tco_estfailprob))
							goto einval;
						olen += T_SPACE(optlen);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_RELFAILPROB:
						if (optlen
						    && optlen != sizeof(t->options.req.tco.tco_relfailprob))
							goto einval;
						olen += T_SPACE(optlen);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_ESTDELAY:
						if (optlen
						    && optlen != sizeof(t->options.req.tco.tco_estdelay))
							goto einval;
						olen += T_SPACE(optlen);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_RELDELAY:
						if (optlen
						    && optlen != sizeof(t->options.req.tco.tco_reldelay))
							goto einval;
						olen += T_SPACE(optlen);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_CONNRESIL:
						if (optlen
						    && optlen != sizeof(t->options.req.tco.tco_connresil))
							goto einval;
						olen += T_SPACE(optlen);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_PROTECTION:
						if (optlen
						    && optlen != sizeof(t->options.req.tco.tco_protection))
							goto einval;
						olen += T_SPACE(optlen);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_PRIORITY:
						if (optlen
						    && optlen != sizeof(t->options.req.tco.tco_priority))
							goto einval;
						olen += T_SPACE(optlen);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_EXPD:
						if (optlen && optlen != sizeof(t->options.req.tco.tco_expd))
							goto einval;
						olen += T_SPACE(optlen);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_LTPDU:
						if (optlen && optlen != sizeof(t->options.req.tco.tco_ltpdu))
							goto einval;
						olen += T_SPACE(optlen);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_ACKTIME:
						if (optlen
						    && optlen != sizeof(t->options.req.tco.tco_acktime))
							goto einval;
						olen += T_SPACE(optlen);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_REASTIME:
						if (optlen
						    && optlen != sizeof(t->options.req.tco.tco_reastime))
							goto einval;
						olen += T_SPACE(optlen);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_PREFCLASS:
						if (optlen
						    && optlen != sizeof(t->options.req.tco.tco_prefclass))
							goto einval;
						olen += T_SPACE(optlen);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_ALTCLASS1:
						if (optlen
						    && optlen != sizeof(t->options.req.tco.tco_altclass1))
							goto einval;
						olen += T_SPACE(optlen);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_ALTCLASS2:
						if (optlen
						    && optlen != sizeof(t->options.req.tco.tco_altclass2))
							goto einval;
						olen += T_SPACE(optlen);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_ALTCLASS3:
						if (optlen
						    && optlen != sizeof(t->options.req.tco.tco_altclass3))
							goto einval;
						olen += T_SPACE(optlen);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_ALTCLASS4:
						if (optlen
						    && optlen != sizeof(t->options.req.tco.tco_altclass4))
							goto einval;
						olen += T_SPACE(optlen);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_EXTFORM:
						if (optlen
						    && optlen != sizeof(t->options.req.tco.tco_extform))
							goto einval;
						olen += T_SPACE(optlen);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_FLOWCTRL:
						if (optlen
						    && optlen != sizeof(t->options.req.tco.tco_flowctrl))
							goto einval;
						olen += T_SPACE(optlen);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_CHECKSUM:
						if (optlen
						    && optlen != sizeof(t->options.req.tco.tco_checksum))
							goto einval;
						olen += T_SPACE(optlen);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_NETEXP:
						if (optlen && optlen != sizeof(t->options.req.tco.tco_netexp))
							goto einval;
						olen += T_SPACE(optlen);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_NETRECPTCF:
						if (optlen
						    && optlen != sizeof(t->options.req.tco.tco_netrecptcf))
							goto einval;
						olen += T_SPACE(optlen);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_SELECTACK:
						if (optlen
						    && optlen != sizeof(t->options.req.tco.tco_selectack))
							goto einval;
						olen += T_SPACE(optlen);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_REQUESTACK:
						if (optlen
						    && optlen != sizeof(t->options.req.tco.tco_requestack))
							goto einval;
						olen += T_SPACE(optlen);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_NBLKEXPDATA:
						if (optlen
						    && optlen != sizeof(t->options.req.tco.tco_nblkexpdata))
							goto einval;
						olen += T_SPACE(optlen);
						if (ih->name != T_ALLOPT)
							continue;
					}
					break;
				case T_CLTS:
					switch (ih->name) {
					default:
						olen += T_SPACE(0);
						continue;
					case T_ALLOPT:
					case T_TCL_TRANSDEL:
						if (optlen
						    && optlen != sizeof(t->options.req.tcl.tcl_transdel))
							goto einval;
						olen += T_SPACE(optlen);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCL_RESERRORRATE:
						if (optlen
						    && optlen != sizeof(t->options.req.tcl.tcl_reserrorrate))
							goto einval;
						olen += T_SPACE(optlen);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCL_PROTECTION:
						if (optlen
						    && optlen != sizeof(t->options.req.tcl.tcl_protection))
							goto einval;
						olen += T_SPACE(optlen);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCL_PRIORITY:
						if (optlen
						    && optlen != sizeof(t->options.req.tcl.tcl_priority))
							goto einval;
						olen += T_SPACE(optlen);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCL_CHECKSUM:
						if (optlen
						    && optlen != sizeof(t->options.req.tcl.tcl_checksum))
							goto einval;
						olen += T_SPACE(optlen);
						if (ih->name != T_ALLOPT)
							continue;
					}
					break;
				}
				if (ih->level != T_ALLLEVELS)
					continue;
				break;
			default:
				if (ih->level != T_ALLLEVELS)
					goto einval;
				break;
			}
			/* fall through */
		case T_INET_IP:
			switch (t->p.prot.type) {
			case TP_CMINOR_COTS:
			case TP_CMINOR_CLTS:
			case TP_CMINOR_COTS_IP:
			case TP_CMINOR_CLTS_IP:
			case TP_CMINOR_COTS_UDP:
			case TP_CMINOR_CLTS_UDP:
				switch (ih->name) {
				default:
					olen += T_SPACE(optlen);
					continue;
				case T_ALLOPT:
				case T_IP_OPTIONS:
					if (optlen && optlen != sizeof(t->options.req.ip.ip_options))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
					/* fall through */
				case T_IP_TOS:
					if (optlen && optlen != sizeof(t->options.req.ip.ip_tos))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
					/* fall through */
				case T_IP_TTL:
					if (optlen && optlen != sizeof(t->options.req.ip.ip_ttl))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
					/* fall through */
				case T_IP_REUSEADDR:
					if (optlen && optlen != sizeof(t->options.req.ip.ip_reuseaddr))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
					/* fall through */
				case T_IP_DONTROUTE:
					if (optlen && optlen != sizeof(t->options.req.ip.ip_dontroute))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
					/* fall through */
				case T_IP_BROADCAST:
					if (optlen && optlen != sizeof(t->options.req.ip.ip_broadcast))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
					/* fall through */
				case T_IP_ADDR:
					if (optlen && optlen != sizeof(t->options.req.ip.ip_addr))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				}
				if (ih->level != T_ALLLEVELS)
					continue;
				break;
			default:
				if (ih->level != T_ALLLEVELS)
					goto einval;
				break;
			}
			/* fall through */
		case T_INET_UDP:
			switch (t->p.prot.type) {
			case TP_CMINOR_COTS:
			case TP_CMINOR_CLTS:
			case TP_CMINOR_COTS_UDP:
			case TP_CMINOR_CLTS_UDP:
				switch (ih->name) {
				default:
					olen += T_SPACE(optlen);
					continue;
				case T_ALLOPT:
				case T_UDP_CHECKSUM:
					if (optlen && optlen != sizeof(t->options.req.udp.udp_checksum))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				}
				if (ih->level != T_ALLLEVELS)
					continue;
				break;
			default:
				if (ih->level != T_ALLLEVELS)
					goto einval;
				break;
			}
		}
	}
	LOGIO(t, "Calculated option output size = %u", olen);
	return (olen);
      einval:
	LOGNO(t, "ERROR: Invalid input options");
	return (-EINVAL);
}

/**
 * t_size_negotiate_options: - size options
 * @tp: private structure (locked)
 * @ip: input options pointer
 * @ilen: input options length
 *
 * Check the validity of the options structure, check for correct size of each supplied option given
 * the option management flag, and return the size required of the acknowledgement options field.
 *
 * Returns the size (positive) of the options necessary to respond to a T_NEGOTIATE options
 * management request, or an error (negative).
 */
static int
t_size_negotiate_options(const struct tp *t, const unsigned char *ip, size_t ilen)
{
	int olen = 0, optlen;
	const struct t_opthdr *ih;
	struct t_opthdr all;

	if (ip == NULL || ilen == 0) {
		/* For zero-length options fake an option header for all names with all levels */
		all.level = T_ALLLEVELS;
		all.name = T_ALLOPT;
		all.len = sizeof(all);
		all.status = 0;
		ip = (unsigned char *) &all;
		ilen = sizeof(all);
	}
	for (ih = _T_OPT_FIRSTHDR_OFS(ip, ilen, 0); ih; ih = _T_OPT_NEXTHDR_OFS(ip, ilen, ih, 0)) {
		if (ih->len < sizeof(*ih))
			goto einval;
		if ((unsigned char *) ih + ih->len > ip + ilen)
			goto einval;
		optlen = ih->len - sizeof(*ih);
		switch (ih->level) {
		default:
			goto einval;
		case T_ALLLEVELS:
			if (ih->name != T_ALLOPT)
				goto einval;
			/* fall through */
		case XTI_GENERIC:
			switch (ih->name) {
			default:
				olen += T_SPACE(optlen);
				continue;
			case T_ALLOPT:
			case XTI_DEBUG:
				if (ih->name != T_ALLOPT
				    && ((optlen % sizeof(t_uscalar_t)) != 0
					|| optlen > 4 * sizeof(t_uscalar_t)))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.req.xti.xti_debug);
				if (ih->name != T_ALLOPT)
					continue;
				/* fall through */
			case XTI_LINGER:
				if (ih->name != T_ALLOPT && optlen != sizeof(t->options.req.xti.xti_linger))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.req.xti.xti_linger);
				if (ih->name != T_ALLOPT)
					continue;
				/* fall through */
			case XTI_RCVBUF:
				if (ih->name != T_ALLOPT && optlen != sizeof(t->options.req.xti.xti_rcvbuf))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.req.xti.xti_rcvbuf);
				if (ih->name != T_ALLOPT)
					continue;
				/* fall through */
			case XTI_RCVLOWAT:
				if (ih->name != T_ALLOPT && optlen != sizeof(t->options.req.xti.xti_rcvlowat))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.req.xti.xti_rcvlowat);
				if (ih->name != T_ALLOPT)
					continue;
				/* fall through */
			case XTI_SNDBUF:
				if (ih->name != T_ALLOPT && optlen != sizeof(t->options.req.xti.xti_sndbuf))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.req.xti.xti_sndbuf);
				if (ih->name != T_ALLOPT)
					continue;
				/* fall through */
			case XTI_SNDLOWAT:
				if (ih->name != T_ALLOPT && optlen != sizeof(t->options.req.xti.xti_sndlowat))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.req.xti.xti_sndlowat);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
			/* fall through */
		case T_ISO_TP:
			switch (t->p.prot.type) {
			case TP_CMINOR_COTS:
			case TP_CMINOR_CLTS:
			case TP_CMINOR_COTS_ISO:
			case TP_CMINOR_CLTS_ISO:
				switch (t->p.info.SERV_type) {
				case T_COTS:
					switch (ih->name) {
					default:
						olen += T_SPACE(optlen);
						continue;
					case T_ALLOPT:
					case T_TCO_THROUGHPUT:
						if (ih->name != T_ALLOPT
						    && optlen != sizeof(t->options.req.tco.tco_throughput))
							goto einval;
						olen += _T_SPACE_SIZEOF(t->options.req.tco.tco_throughput);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_TRANSDEL:
						if (ih->name != T_ALLOPT
						    && optlen != sizeof(t->options.req.tco.tco_transdel))
							goto einval;
						olen += _T_SPACE_SIZEOF(t->options.req.tco.tco_transdel);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_RESERRORRATE:
						if (ih->name != T_ALLOPT
						    && optlen != sizeof(t->options.req.tco.tco_reserrorrate))
							goto einval;
						olen += _T_SPACE_SIZEOF(t->options.req.tco.tco_reserrorrate);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_TRANSFFAILPROB:
						if (ih->name != T_ALLOPT
						    && optlen !=
						    sizeof(t->options.req.tco.tco_transffailprob))
							goto einval;
						olen +=
						    _T_SPACE_SIZEOF(t->options.req.tco.tco_transffailprob);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_ESTFAILPROB:
						if (ih->name != T_ALLOPT
						    && optlen != sizeof(t->options.req.tco.tco_estfailprob))
							goto einval;
						olen += _T_SPACE_SIZEOF(t->options.req.tco.tco_estfailprob);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_RELFAILPROB:
						if (ih->name != T_ALLOPT
						    && optlen != sizeof(t->options.req.tco.tco_relfailprob))
							goto einval;
						olen += _T_SPACE_SIZEOF(t->options.req.tco.tco_relfailprob);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_ESTDELAY:
						if (ih->name != T_ALLOPT
						    && optlen != sizeof(t->options.req.tco.tco_estdelay))
							goto einval;
						olen += _T_SPACE_SIZEOF(t->options.req.tco.tco_estdelay);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_RELDELAY:
						if (ih->name != T_ALLOPT
						    && optlen != sizeof(t->options.req.tco.tco_reldelay))
							goto einval;
						olen += _T_SPACE_SIZEOF(t->options.req.tco.tco_reldelay);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_CONNRESIL:
						if (ih->name != T_ALLOPT
						    && optlen != sizeof(t->options.req.tco.tco_connresil))
							goto einval;
						olen += _T_SPACE_SIZEOF(t->options.req.tco.tco_connresil);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_PROTECTION:
						if (ih->name != T_ALLOPT
						    && optlen != sizeof(t->options.req.tco.tco_protection))
							goto einval;
						olen += _T_SPACE_SIZEOF(t->options.req.tco.tco_protection);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_PRIORITY:
						if (ih->name != T_ALLOPT
						    && optlen != sizeof(t->options.req.tco.tco_priority))
							goto einval;
						olen += _T_SPACE_SIZEOF(t->options.req.tco.tco_priority);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_EXPD:
						if (ih->name != T_ALLOPT
						    && optlen != sizeof(t->options.req.tco.tco_expd))
							goto einval;
						olen += _T_SPACE_SIZEOF(t->options.req.tco.tco_expd);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_LTPDU:
						if (ih->name != T_ALLOPT
						    && optlen != sizeof(t->options.req.tco.tco_ltpdu))
							goto einval;
						olen += _T_SPACE_SIZEOF(t->options.req.tco.tco_ltpdu);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_ACKTIME:
						if (ih->name != T_ALLOPT
						    && optlen != sizeof(t->options.req.tco.tco_acktime))
							goto einval;
						olen += _T_SPACE_SIZEOF(t->options.req.tco.tco_acktime);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_REASTIME:
						if (ih->name != T_ALLOPT
						    && optlen != sizeof(t->options.req.tco.tco_reastime))
							goto einval;
						olen += _T_SPACE_SIZEOF(t->options.req.tco.tco_reastime);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_PREFCLASS:
						if (ih->name != T_ALLOPT
						    && optlen != sizeof(t->options.req.tco.tco_prefclass))
							goto einval;
						olen += _T_SPACE_SIZEOF(t->options.req.tco.tco_prefclass);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_ALTCLASS1:
						if (ih->name != T_ALLOPT
						    && optlen != sizeof(t->options.req.tco.tco_altclass1))
							goto einval;
						olen += _T_SPACE_SIZEOF(t->options.req.tco.tco_altclass1);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_ALTCLASS2:
						if (ih->name != T_ALLOPT
						    && optlen != sizeof(t->options.req.tco.tco_altclass2))
							goto einval;
						olen += _T_SPACE_SIZEOF(t->options.req.tco.tco_altclass2);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_ALTCLASS3:
						if (ih->name != T_ALLOPT
						    && optlen != sizeof(t->options.req.tco.tco_altclass3))
							goto einval;
						olen += _T_SPACE_SIZEOF(t->options.req.tco.tco_altclass3);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_ALTCLASS4:
						if (ih->name != T_ALLOPT
						    && optlen != sizeof(t->options.req.tco.tco_altclass4))
							goto einval;
						olen += _T_SPACE_SIZEOF(t->options.req.tco.tco_altclass4);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_EXTFORM:
						if (ih->name != T_ALLOPT
						    && optlen != sizeof(t->options.req.tco.tco_extform))
							goto einval;
						olen += _T_SPACE_SIZEOF(t->options.req.tco.tco_extform);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_FLOWCTRL:
						if (ih->name != T_ALLOPT
						    && optlen != sizeof(t->options.req.tco.tco_flowctrl))
							goto einval;
						olen += _T_SPACE_SIZEOF(t->options.req.tco.tco_flowctrl);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_CHECKSUM:
						if (ih->name != T_ALLOPT
						    && optlen != sizeof(t->options.req.tco.tco_checksum))
							goto einval;
						olen += _T_SPACE_SIZEOF(t->options.req.tco.tco_checksum);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_NETEXP:
						if (ih->name != T_ALLOPT
						    && optlen != sizeof(t->options.req.tco.tco_netexp))
							goto einval;
						olen += _T_SPACE_SIZEOF(t->options.req.tco.tco_netexp);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_NETRECPTCF:
						if (ih->name != T_ALLOPT
						    && optlen != sizeof(t->options.req.tco.tco_netrecptcf))
							goto einval;
						olen += _T_SPACE_SIZEOF(t->options.req.tco.tco_netrecptcf);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_SELECTACK:
						if (ih->name != T_ALLOPT
						    && optlen != sizeof(t->options.req.tco.tco_selectack))
							goto einval;
						olen += _T_SPACE_SIZEOF(t->options.req.tco.tco_selectack);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_REQUESTACK:
						if (ih->name != T_ALLOPT
						    && optlen != sizeof(t->options.req.tco.tco_requestack))
							goto einval;
						olen += _T_SPACE_SIZEOF(t->options.req.tco.tco_requestack);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCO_NBLKEXPDATA:
						if (ih->name != T_ALLOPT
						    && optlen != sizeof(t->options.req.tco.tco_nblkexpdata))
							goto einval;
						olen += _T_SPACE_SIZEOF(t->options.req.tco.tco_nblkexpdata);
						if (ih->name != T_ALLOPT)
							continue;
					}
					break;
				case T_CLTS:
					switch (ih->name) {
					default:
						olen += T_SPACE(optlen);
						continue;
					case T_ALLOPT:
					case T_TCL_TRANSDEL:
						if (ih->name != T_ALLOPT
						    && optlen != sizeof(t->options.req.tcl.tcl_transdel))
							goto einval;
						olen += _T_SPACE_SIZEOF(t->options.req.tcl.tcl_transdel);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCL_RESERRORRATE:
						if (ih->name != T_ALLOPT
						    && optlen != sizeof(t->options.req.tcl.tcl_reserrorrate))
							goto einval;
						olen += _T_SPACE_SIZEOF(t->options.req.tcl.tcl_reserrorrate);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCL_PROTECTION:
						if (ih->name != T_ALLOPT
						    && optlen != sizeof(t->options.req.tcl.tcl_protection))
							goto einval;
						olen += _T_SPACE_SIZEOF(t->options.req.tcl.tcl_protection);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCL_PRIORITY:
						if (ih->name != T_ALLOPT
						    && optlen != sizeof(t->options.req.tcl.tcl_priority))
							goto einval;
						olen += _T_SPACE_SIZEOF(t->options.req.tcl.tcl_priority);
						if (ih->name != T_ALLOPT)
							continue;
						/* fall through */
					case T_TCL_CHECKSUM:
						if (ih->name != T_ALLOPT
						    && optlen != sizeof(t->options.req.tcl.tcl_checksum))
							goto einval;
						olen += _T_SPACE_SIZEOF(t->options.req.tcl.tcl_checksum);
						if (ih->name != T_ALLOPT)
							continue;
					}
					break;
				}
				if (ih->level != T_ALLLEVELS)
					continue;
				break;
			default:
				if (ih->level != T_ALLLEVELS)
					goto einval;
				break;
			}
			/* fall through */
		case T_INET_IP:
			switch (t->p.prot.type) {
			case TP_CMINOR_COTS:
			case TP_CMINOR_CLTS:
			case TP_CMINOR_COTS_IP:
			case TP_CMINOR_CLTS_IP:
			case TP_CMINOR_COTS_UDP:
			case TP_CMINOR_CLTS_UDP:
				switch (ih->name) {
				default:
					olen += T_SPACE(optlen);
					continue;
				case T_ALLOPT:
				case T_IP_OPTIONS:
					/* If the status is T_SUCCESS, T_FAILURE, T_NOTSUPPORT or T_READONLY, 
					   the returned option value is the same as the one requested on
					   input. */
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
					/* fall through */
				case T_IP_TOS:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.req.ip.ip_tos))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.req.ip.ip_tos);
					if (ih->name != T_ALLOPT)
						continue;
					/* fall through */
				case T_IP_TTL:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.req.ip.ip_ttl))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.req.ip.ip_ttl);
					if (ih->name != T_ALLOPT)
						continue;
					/* fall through */
				case T_IP_REUSEADDR:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.req.ip.ip_reuseaddr))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.req.ip.ip_reuseaddr);
					if (ih->name != T_ALLOPT)
						continue;
					/* fall through */
				case T_IP_DONTROUTE:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.req.ip.ip_dontroute))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.req.ip.ip_dontroute);
					if (ih->name != T_ALLOPT)
						continue;
					/* fall through */
				case T_IP_BROADCAST:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.req.ip.ip_broadcast))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.req.ip.ip_broadcast);
					if (ih->name != T_ALLOPT)
						continue;
					/* fall through */
				case T_IP_ADDR:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.req.ip.ip_addr))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.req.ip.ip_addr);
					if (ih->name != T_ALLOPT)
						continue;
				}
				if (ih->level != T_ALLLEVELS)
					continue;
				break;
			default:
				if (ih->level != T_ALLLEVELS)
					goto einval;
				break;
			}
			/* fall through */
		case T_INET_UDP:
			switch (t->p.prot.type) {
			case TP_CMINOR_COTS:
			case TP_CMINOR_CLTS:
			case TP_CMINOR_COTS_UDP:
			case TP_CMINOR_CLTS_UDP:
				switch (ih->name) {
				default:
					olen += T_SPACE(optlen);
					continue;
				case T_ALLOPT:
				case T_UDP_CHECKSUM:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(t->options.req.udp.udp_checksum))
						goto einval;
					olen += _T_SPACE_SIZEOF(t->options.req.udp.udp_checksum);
					if (ih->name != T_ALLOPT)
						continue;
				}
				if (ih->level != T_ALLLEVELS)
					continue;
				break;
			default:
				if (ih->level != T_ALLLEVELS)
					goto einval;
				break;
			}
		}
	}
	LOGIO(t, "Calculated option output size = %u", olen);
	return (olen);
      einval:
	LOGNO(t, "ERROR: Invalid input options");
	return (-EINVAL);
}

static uint
t_overall_result(t_scalar_t *overall, uint result)
{
	switch (result) {
	case T_NOTSUPPORT:
		if (!(*overall & (T_NOTSUPPORT)))
			*overall = T_NOTSUPPORT;
		break;
	case T_READONLY:
		if (!(*overall & (T_NOTSUPPORT | T_READONLY)))
			*overall = T_READONLY;
		break;
	case T_FAILURE:
		if (!(*overall & (T_NOTSUPPORT | T_READONLY | T_FAILURE)))
			*overall = T_FAILURE;
		break;
	case T_PARTSUCCESS:
		if (!(*overall & (T_NOTSUPPORT | T_READONLY | T_FAILURE | T_PARTSUCCESS)))
			*overall = T_PARTSUCCESS;
		break;
	case T_SUCCESS:
		if (!(*overall & (T_NOTSUPPORT | T_READONLY | T_FAILURE | T_PARTSUCCESS | T_SUCCESS)))
			*overall = T_SUCCESS;
		break;
	}
	return (result);
}

/**
 * t_build_default_options: - build default options
 * @t: private structure (locked)
 * @ip: input options pointer
 * @ilen: input options length
 * @op: output options pointer
 * @olen: output options length (returned)
 *
 * Perform the actions required of T_DEFAULT placing the output in the provided buffer.
 */
static t_scalar_t
t_build_default_options(const struct tp *t, const unsigned char *ip, size_t ilen, unsigned char *op,
			size_t *olen)
{
	t_scalar_t overall = T_SUCCESS;
	const struct t_opthdr *ih;
	struct t_opthdr *oh, all;
	int optlen;

	if (ilen == 0) {
		/* For zero-length options fake an option for all names within all levels. */
		all.level = T_ALLLEVELS;
		all.name = T_ALLOPT;
		all.len = sizeof(all);
		all.status = 0;
		ip = (unsigned char *) &all;
		ilen = sizeof(all);
	}
	for (ih = _T_OPT_FIRSTHDR_OFS(ip, ilen, 0), oh = _T_OPT_FIRSTHDR_OFS(op, *olen, 0); ih && oh;
	     ih = _T_OPT_NEXTHDR_OFS(ip, ilen, ih, 0), oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)) {
		/* don't need to do this, it was done when we sized options */
		if (ih->len < sizeof(*ih))
			goto einval;
		if ((unsigned char *) ih + ih->len > ip + ilen)
			goto einval;
		optlen = ih->len - sizeof(*ih);
		(void) optlen;
		switch (ih->level) {
		default:
			goto einval;
		case T_ALLLEVELS:
			if (ih->name != T_ALLOPT)
				goto einval;
			/* fall through */
		case XTI_GENERIC:
			switch (ih->name) {
			default:
				oh->len = sizeof(*oh);
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				continue;
			case T_ALLOPT:
			case XTI_DEBUG:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.xti.xti_debug);
				oh->level = XTI_GENERIC;
				oh->name = XTI_DEBUG;
				oh->status = T_SUCCESS;
				bcopy(t_defaults.xti.xti_debug, T_OPT_DATA(oh),
				      sizeof(t_defaults.xti.xti_debug));
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
				/* fall through */
			case XTI_LINGER:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.xti.xti_linger);
				oh->level = XTI_GENERIC;
				oh->name = XTI_LINGER;
				oh->status = T_SUCCESS;
				*((struct t_linger *) T_OPT_DATA(oh)) = t_defaults.xti.xti_linger;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
				/* fall through */
			case XTI_RCVBUF:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.xti.xti_rcvbuf);
				oh->level = XTI_GENERIC;
				oh->name = XTI_RCVBUF;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = sysctl_rmem_default;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
				/* fall through */
			case XTI_RCVLOWAT:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.xti.xti_rcvlowat);
				oh->level = XTI_GENERIC;
				oh->name = XTI_RCVLOWAT;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.xti.xti_rcvlowat;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
				/* fall through */
			case XTI_SNDBUF:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.xti.xti_sndbuf);
				oh->level = XTI_GENERIC;
				oh->name = XTI_SNDBUF;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = sysctl_wmem_default;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
				/* fall through */
			case XTI_SNDLOWAT:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.xti.xti_sndlowat);
				oh->level = XTI_GENERIC;
				oh->name = XTI_SNDLOWAT;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.xti.xti_sndlowat;
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
			switch (t->p.prot.type) {
			case TP_CMINOR_COTS:
			case TP_CMINOR_CLTS:
			case TP_CMINOR_COTS_ISO:
			case TP_CMINOR_CLTS_ISO:
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
				break;
			}
			/* fall through */
		case T_ISO_TP:
			switch (t->p.prot.type) {
			case TP_CMINOR_COTS:
			case TP_CMINOR_CLTS:
			case TP_CMINOR_COTS_ISO:
			case TP_CMINOR_CLTS_ISO:
				switch (t->p.info.SERV_type) {
				case T_COTS:
					switch (ih->name) {
					default:
						oh->len = sizeof(*oh);
						oh->level = ih->level;
						oh->name = ih->name;
						oh->status = t_overall_result(&overall, T_NOTSUPPORT);
						continue;
					case T_ALLOPT:
					case T_TCO_THROUGHPUT:
						oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.tco_throughput);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_THROUGHPUT;
						oh->status = T_SUCCESS;
						*(struct thrpt *) T_OPT_DATA(oh) =
						    t_defaults.tco.tco_throughput;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_TRANSDEL:
						oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.tco_transdel);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_TRANSDEL;
						oh->status = T_SUCCESS;
						*(struct transdel *) T_OPT_DATA(oh) =
						    t_defaults.tco.tco_transdel;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_RESERRORRATE:
						oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.tco_reserrorrate);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_RESERRORRATE;
						oh->status = T_SUCCESS;
						*(struct rate *) T_OPT_DATA(oh) =
						    t_defaults.tco.tco_reserrorrate;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_TRANSFFAILPROB:
						oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.tco_transffailprob);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_TRANSFFAILPROB;
						oh->status = T_SUCCESS;
						*(struct rate *) T_OPT_DATA(oh) =
						    t_defaults.tco.tco_transffailprob;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_ESTFAILPROB:
						oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.tco_estfailprob);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_ESTFAILPROB;
						oh->status = T_SUCCESS;
						*(struct rate *) T_OPT_DATA(oh) =
						    t_defaults.tco.tco_estfailprob;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_RELFAILPROB:
						oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.tco_relfailprob);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_RELFAILPROB;
						oh->status = T_SUCCESS;
						*(struct rate *) T_OPT_DATA(oh) =
						    t_defaults.tco.tco_relfailprob;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_ESTDELAY:
						oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.tco_estdelay);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_ESTDELAY;
						oh->status = T_SUCCESS;
						*(struct rate *) T_OPT_DATA(oh) = t_defaults.tco.tco_estdelay;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_RELDELAY:
						oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.tco_reldelay);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_RELDELAY;
						oh->status = T_SUCCESS;
						*(struct rate *) T_OPT_DATA(oh) = t_defaults.tco.tco_reldelay;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_CONNRESIL:
						oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.tco_connresil);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_CONNRESIL;
						oh->status = T_SUCCESS;
						*(struct rate *) T_OPT_DATA(oh) =
						    t_defaults.tco.tco_connresil;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_PROTECTION:
						oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.tco_protection);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_PROTECTION;
						oh->status = T_SUCCESS;
						*(t_uscalar_t *) T_OPT_DATA(oh) =
						    t_defaults.tco.tco_protection;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_PRIORITY:
						oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.tco_priority);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_PRIORITY;
						oh->status = T_SUCCESS;
						*(t_uscalar_t *) T_OPT_DATA(oh) = t_defaults.tco.tco_priority;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_EXPD:
						oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.tco_expd);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_EXPD;
						oh->status = T_SUCCESS;
						*(t_uscalar_t *) T_OPT_DATA(oh) = t_defaults.tco.tco_expd;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_LTPDU:
						oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.tco_ltpdu);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_LTPDU;
						oh->status = T_SUCCESS;
						*(t_uscalar_t *) T_OPT_DATA(oh) = t_defaults.tco.tco_ltpdu;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_ACKTIME:
						oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.tco_acktime);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_ACKTIME;
						oh->status = T_SUCCESS;
						*(t_uscalar_t *) T_OPT_DATA(oh) = t_defaults.tco.tco_acktime;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_REASTIME:
						oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.tco_reastime);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_REASTIME;
						oh->status = T_SUCCESS;
						*(t_uscalar_t *) T_OPT_DATA(oh) = t_defaults.tco.tco_reastime;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_PREFCLASS:
						oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.tco_prefclass);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_PREFCLASS;
						oh->status = T_SUCCESS;
						*(t_uscalar_t *) T_OPT_DATA(oh) =
						    t_defaults.tco.tco_prefclass;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_ALTCLASS1:
						oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.tco_altclass1);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_ALTCLASS1;
						oh->status = T_SUCCESS;
						*(t_uscalar_t *) T_OPT_DATA(oh) =
						    t_defaults.tco.tco_altclass1;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_ALTCLASS2:
						oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.tco_altclass2);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_ALTCLASS2;
						oh->status = T_SUCCESS;
						*(t_uscalar_t *) T_OPT_DATA(oh) =
						    t_defaults.tco.tco_altclass2;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_ALTCLASS3:
						oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.tco_altclass3);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_ALTCLASS3;
						oh->status = T_SUCCESS;
						*(t_uscalar_t *) T_OPT_DATA(oh) =
						    t_defaults.tco.tco_altclass3;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_ALTCLASS4:
						oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.tco_altclass4);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_ALTCLASS4;
						oh->status = T_SUCCESS;
						*(t_uscalar_t *) T_OPT_DATA(oh) =
						    t_defaults.tco.tco_altclass4;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_EXTFORM:
						oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.tco_extform);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_EXTFORM;
						oh->status = T_SUCCESS;
						*(t_uscalar_t *) T_OPT_DATA(oh) = t_defaults.tco.tco_extform;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_FLOWCTRL:
						oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.tco_flowctrl);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_FLOWCTRL;
						oh->status = T_SUCCESS;
						*(t_uscalar_t *) T_OPT_DATA(oh) = t_defaults.tco.tco_flowctrl;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_CHECKSUM:
						oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.tco_checksum);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_CHECKSUM;
						oh->status = T_SUCCESS;
						*(t_uscalar_t *) T_OPT_DATA(oh) = t_defaults.tco.tco_checksum;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_NETEXP:
						oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.tco_netexp);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_NETEXP;
						oh->status = T_SUCCESS;
						*(t_uscalar_t *) T_OPT_DATA(oh) = t_defaults.tco.tco_netexp;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_NETRECPTCF:
						oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.tco_netrecptcf);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_NETRECPTCF;
						oh->status = T_SUCCESS;
						*(t_uscalar_t *) T_OPT_DATA(oh) =
						    t_defaults.tco.tco_netrecptcf;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_SELECTACK:
						oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.tco_selectack);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_SELECTACK;
						oh->status = T_SUCCESS;
						*(t_uscalar_t *) T_OPT_DATA(oh) =
						    t_defaults.tco.tco_selectack;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_REQUESTACK:
						oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.tco_requestack);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_REQUESTACK;
						oh->status = T_SUCCESS;
						*(t_uscalar_t *) T_OPT_DATA(oh) =
						    t_defaults.tco.tco_requestack;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_NBLKEXPDATA:
						oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.tco_nblkexpdata);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_NBLKEXPDATA;
						oh->status = T_SUCCESS;
						*(t_uscalar_t *) T_OPT_DATA(oh) =
						    t_defaults.tco.tco_nblkexpdata;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
					}
					break;
				case T_CLTS:
					switch (ih->name) {
					default:
						oh->len = sizeof(*oh);
						oh->level = ih->level;
						oh->name = ih->name;
						oh->status = t_overall_result(&overall, T_NOTSUPPORT);
						continue;
					case T_ALLOPT:
					case T_TCL_TRANSDEL:
						oh->len = _T_LENGTH_SIZEOF(t_defaults.tcl.tcl_transdel);
						oh->level = T_ISO_TP;
						oh->name = T_TCL_TRANSDEL;
						oh->status = T_SUCCESS;
						*(struct transdel *) T_OPT_DATA(oh) =
						    t_defaults.tcl.tcl_transdel;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCL_RESERRORRATE:
						oh->len = _T_LENGTH_SIZEOF(t_defaults.tcl.tcl_reserrorrate);
						oh->level = T_ISO_TP;
						oh->name = T_TCL_RESERRORRATE;
						oh->status = T_SUCCESS;
						*(struct rate *) T_OPT_DATA(oh) =
						    t_defaults.tcl.tcl_reserrorrate;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCL_PROTECTION:
						oh->len = _T_LENGTH_SIZEOF(t_defaults.tcl.tcl_protection);
						oh->level = T_ISO_TP;
						oh->name = T_TCL_PROTECTION;
						oh->status = T_SUCCESS;
						*(t_uscalar_t *) T_OPT_DATA(oh) =
						    t_defaults.tcl.tcl_protection;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCL_PRIORITY:
						oh->len = _T_LENGTH_SIZEOF(t_defaults.tcl.tcl_priority);
						oh->level = T_ISO_TP;
						oh->name = T_TCL_PRIORITY;
						oh->status = T_SUCCESS;
						*(t_uscalar_t *) T_OPT_DATA(oh) = t_defaults.tcl.tcl_priority;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCL_CHECKSUM:
						oh->len = _T_LENGTH_SIZEOF(t_defaults.tcl.tcl_checksum);
						oh->level = T_ISO_TP;
						oh->name = T_TCL_CHECKSUM;
						oh->status = T_SUCCESS;
						*(t_uscalar_t *) T_OPT_DATA(oh) = t_defaults.tcl.tcl_checksum;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
					}
					break;
				}
				if (ih->level != T_ALLLEVELS)
					continue;
				switch (t->p.prot.type) {
				case TP_CMINOR_COTS:
				case TP_CMINOR_CLTS:
				case TP_CMINOR_COTS_IP:
				case TP_CMINOR_CLTS_IP:
				case TP_CMINOR_COTS_UDP:
				case TP_CMINOR_CLTS_UDP:
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
					break;
				}
				break;
			default:
				if (ih->level != T_ALLLEVELS)
					goto einval;
				break;
			}
			/* fall through */
		case T_INET_IP:
			switch (t->p.prot.type) {
			case TP_CMINOR_COTS:
			case TP_CMINOR_CLTS:
			case TP_CMINOR_COTS_IP:
			case TP_CMINOR_CLTS_IP:
			case TP_CMINOR_COTS_UDP:
			case TP_CMINOR_CLTS_UDP:
				switch (ih->name) {
				default:
					oh->len = sizeof(*oh);
					oh->level = ih->level;
					oh->name = ih->name;
					oh->status = t_overall_result(&overall, T_NOTSUPPORT);
					continue;
				case T_ALLOPT:
				case T_IP_OPTIONS:
					oh->len = sizeof(*oh);
					oh->level = T_INET_IP;
					oh->name = T_IP_OPTIONS;
					oh->status = t_overall_result(&overall, T_NOTSUPPORT);
					/* not supported yet */
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
					/* fall through */
				case T_IP_TOS:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.ip.ip_tos);
					oh->level = T_INET_IP;
					oh->name = T_IP_TOS;
					oh->status = T_SUCCESS;
					*((unsigned char *) T_OPT_DATA(oh)) = t_defaults.ip.ip_tos;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
					/* fall through */
				case T_IP_TTL:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.ip.ip_ttl);
					oh->level = T_INET_IP;
					oh->name = T_IP_TTL;
					oh->status = T_SUCCESS;
					*((unsigned char *) T_OPT_DATA(oh)) = t_defaults.ip.ip_ttl;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
					/* fall through */
				case T_IP_REUSEADDR:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.ip.ip_reuseaddr);
					oh->level = T_INET_IP;
					oh->name = T_IP_REUSEADDR;
					oh->status = T_SUCCESS;
					*((unsigned int *) T_OPT_DATA(oh)) = t_defaults.ip.ip_reuseaddr;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
					/* fall through */
				case T_IP_DONTROUTE:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.ip.ip_dontroute);
					oh->level = T_INET_IP;
					oh->name = T_IP_DONTROUTE;
					oh->status = T_SUCCESS;
					*((unsigned int *) T_OPT_DATA(oh)) = t_defaults.ip.ip_dontroute;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
					/* fall through */
				case T_IP_BROADCAST:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.ip.ip_broadcast);
					oh->level = T_INET_IP;
					oh->name = T_IP_BROADCAST;
					oh->status = T_SUCCESS;
					*((unsigned int *) T_OPT_DATA(oh)) = t_defaults.ip.ip_broadcast;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
					/* fall through */
				case T_IP_ADDR:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.ip.ip_addr);
					oh->level = T_INET_IP;
					oh->name = T_IP_ADDR;
					oh->status = T_SUCCESS;
					*((uint32_t *) T_OPT_DATA(oh)) = t_defaults.ip.ip_addr;
					if (ih->name != T_ALLOPT)
						continue;
				}
				if (ih->level != T_ALLLEVELS)
					continue;
				switch (t->p.prot.type) {
				case TP_CMINOR_COTS:
				case TP_CMINOR_CLTS:
				case TP_CMINOR_COTS_UDP:
				case TP_CMINOR_CLTS_UDP:
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
					break;
				}
				break;
			default:
				if (ih->level != T_ALLLEVELS)
					goto einval;
				break;
			}
			/* fall through */
		case T_INET_UDP:
			switch (t->p.prot.type) {
			case TP_CMINOR_COTS:
			case TP_CMINOR_CLTS:
			case TP_CMINOR_COTS_UDP:
			case TP_CMINOR_CLTS_UDP:
				switch (ih->name) {
				default:
					oh->len = sizeof(*oh);
					oh->level = ih->level;
					oh->name = ih->name;
					oh->status = t_overall_result(&overall, T_NOTSUPPORT);
					continue;
				case T_ALLOPT:
				case T_UDP_CHECKSUM:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.udp.udp_checksum);
					oh->level = T_INET_UDP;
					oh->name = T_UDP_CHECKSUM;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.udp.udp_checksum;
					if (ih->name != T_ALLOPT)
						continue;
				}
				if (ih->level != T_ALLLEVELS)
					continue;
				break;
			default:
				if (ih->level != T_ALLLEVELS)
					goto einval;
				break;
			}
		}
	}
	if (ih && !oh)
		goto efault;
	if (oh)
		*olen = (unsigned char *) oh - op;
	return (overall);
      einval:
	return (-EINVAL);
      efault:
	LOGERR(t, "SWERR: %s %s:%d", __FUNCTION__, __FILE__, __LINE__);
	return (-EFAULT);
}

/**
 * t_build_current_options: - build current options
 * @t: private structure (locked)
 * @ip: input options pointer
 * @ilen: input options length
 * @op: output options pointer
 * @olen: output options length (returned)
 *
 * Perform the actions required of T_CURRENT placing the output in the provided buffer.
 */
static t_scalar_t
t_build_current_options(const struct tp *t, const unsigned char *ip, size_t ilen, unsigned char *op,
			size_t *olen)
{
	t_scalar_t overall = T_SUCCESS;
	const struct t_opthdr *ih;
	struct t_opthdr *oh, all;
	int optlen;

	if (ilen == 0) {
		/* For zero-length options fake an option for all names within all levels. */
		all.level = T_ALLLEVELS;
		all.name = T_ALLOPT;
		all.len = sizeof(all);
		all.status = 0;
		ip = (unsigned char *) &all;
		ilen = sizeof(all);
	}
	for (ih = _T_OPT_FIRSTHDR_OFS(ip, ilen, 0), oh = _T_OPT_FIRSTHDR_OFS(op, *olen, 0); ih && oh;
	     ih = _T_OPT_NEXTHDR_OFS(ip, ilen, ih, 0), oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)) {
		/* don't need to do this, it was done when we sized options */
		if (ih->len < sizeof(*ih))
			goto einval;
		if ((unsigned char *) ih + ih->len > ip + ilen)
			goto einval;
		optlen = ih->len - sizeof(*ih);
		(void) optlen;
		switch (ih->level) {
		default:
			goto einval;
		case T_ALLLEVELS:
			if (ih->name != T_ALLOPT)
				goto einval;
			/* fall through */
		case XTI_GENERIC:
			switch (ih->name) {
			default:
				oh->len = sizeof(*oh);
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				continue;
			case T_ALLOPT:
			case XTI_DEBUG:
				oh->len = _T_LENGTH_SIZEOF(t->options.req.xti.xti_debug);
				oh->level = XTI_GENERIC;
				oh->name = XTI_DEBUG;
				oh->status = T_SUCCESS;
				bcopy(t->options.req.xti.xti_debug, T_OPT_DATA(oh), 4 * sizeof(t_uscalar_t));
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
				/* fall through */
			case XTI_LINGER:
				oh->len = _T_LENGTH_SIZEOF(t->options.req.xti.xti_linger);
				oh->level = XTI_GENERIC;
				oh->name = XTI_LINGER;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((struct t_linger *) T_OPT_DATA(oh)) = t->options.req.xti.xti_linger;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
				/* fall through */
			case XTI_RCVBUF:
				oh->len = _T_LENGTH_SIZEOF(t->options.req.xti.xti_rcvbuf);
				oh->level = XTI_GENERIC;
				oh->name = XTI_RCVBUF;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.req.xti.xti_rcvbuf;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
				/* fall through */
			case XTI_RCVLOWAT:
				oh->len = _T_LENGTH_SIZEOF(t->options.req.xti.xti_rcvlowat);
				oh->level = XTI_GENERIC;
				oh->name = XTI_RCVLOWAT;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.req.xti.xti_rcvlowat;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
				/* fall through */
			case XTI_SNDBUF:
				oh->len = _T_LENGTH_SIZEOF(t->options.req.xti.xti_sndbuf);
				oh->level = XTI_GENERIC;
				oh->name = XTI_SNDBUF;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.req.xti.xti_sndbuf;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
				/* fall through */
			case XTI_SNDLOWAT:
				oh->len = _T_LENGTH_SIZEOF(t->options.req.xti.xti_sndlowat);
				oh->level = XTI_GENERIC;
				oh->name = XTI_SNDLOWAT;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.req.xti.xti_sndlowat;
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
			switch (t->p.prot.type) {
			case TP_CMINOR_COTS:
			case TP_CMINOR_CLTS:
			case TP_CMINOR_COTS_ISO:
			case TP_CMINOR_CLTS_ISO:
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
				break;
			}
			/* fall through */
		case T_ISO_TP:
			switch (t->p.prot.type) {
			case TP_CMINOR_COTS:
			case TP_CMINOR_CLTS:
			case TP_CMINOR_COTS_ISO:
			case TP_CMINOR_CLTS_ISO:
				switch (t->p.info.SERV_type) {
				case T_COTS:
					switch (ih->name) {
					default:
						oh->len = sizeof(*oh);
						oh->level = ih->level;
						oh->name = ih->name;
						oh->status = t_overall_result(&overall, T_NOTSUPPORT);
						continue;
					case T_ALLOPT:
					case T_TCO_THROUGHPUT:
						oh->len = _T_LENGTH_SIZEOF(t->options.req.tco.tco_throughput);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_THROUGHPUT;
						oh->status = T_SUCCESS;
						/* refresh current value */
						*(struct thrpt *) T_OPT_DATA(oh) =
						    t->options.req.tco.tco_throughput;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_TRANSDEL:
						oh->len = _T_LENGTH_SIZEOF(t->options.req.tco.tco_transdel);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_TRANSDEL;
						oh->status = T_SUCCESS;
						/* refresh current value */
						*(struct transdel *) T_OPT_DATA(oh) =
						    t->options.req.tco.tco_transdel;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_RESERRORRATE:
						oh->len =
						    _T_LENGTH_SIZEOF(t->options.req.tco.tco_reserrorrate);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_RESERRORRATE;
						oh->status = T_SUCCESS;
						/* refresh current value */
						*(struct rate *) T_OPT_DATA(oh) =
						    t->options.req.tco.tco_reserrorrate;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_TRANSFFAILPROB:
						oh->len =
						    _T_LENGTH_SIZEOF(t->options.req.tco.tco_transffailprob);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_TRANSFFAILPROB;
						oh->status = T_SUCCESS;
						/* refresh current value */
						*(struct rate *) T_OPT_DATA(oh) =
						    t->options.req.tco.tco_transffailprob;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_ESTFAILPROB:
						oh->len =
						    _T_LENGTH_SIZEOF(t->options.req.tco.tco_estfailprob);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_ESTFAILPROB;
						oh->status = T_SUCCESS;
						/* refresh current value */
						*(struct rate *) T_OPT_DATA(oh) =
						    t->options.req.tco.tco_estfailprob;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_RELFAILPROB:
						oh->len =
						    _T_LENGTH_SIZEOF(t->options.req.tco.tco_relfailprob);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_RELFAILPROB;
						oh->status = T_SUCCESS;
						/* refresh current value */
						*(struct rate *) T_OPT_DATA(oh) =
						    t->options.req.tco.tco_relfailprob;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_ESTDELAY:
						oh->len = _T_LENGTH_SIZEOF(t->options.req.tco.tco_estdelay);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_ESTDELAY;
						oh->status = T_SUCCESS;
						/* refresh current value */
						*(struct rate *) T_OPT_DATA(oh) =
						    t->options.req.tco.tco_estdelay;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_RELDELAY:
						oh->len = _T_LENGTH_SIZEOF(t->options.req.tco.tco_reldelay);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_RELDELAY;
						oh->status = T_SUCCESS;
						/* refresh current value */
						*(struct rate *) T_OPT_DATA(oh) =
						    t->options.req.tco.tco_reldelay;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_CONNRESIL:
						oh->len = _T_LENGTH_SIZEOF(t->options.req.tco.tco_connresil);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_CONNRESIL;
						oh->status = T_SUCCESS;
						/* refresh current value */
						*(struct rate *) T_OPT_DATA(oh) =
						    t->options.req.tco.tco_connresil;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_PROTECTION:
						oh->len = _T_LENGTH_SIZEOF(t->options.req.tco.tco_protection);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_PROTECTION;
						oh->status = T_SUCCESS;
						/* refresh current value */
						*(t_uscalar_t *) T_OPT_DATA(oh) =
						    t->options.req.tco.tco_protection;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_PRIORITY:
						oh->len = _T_LENGTH_SIZEOF(t->options.req.tco.tco_priority);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_PRIORITY;
						oh->status = T_SUCCESS;
						/* refresh current value */
						*(t_uscalar_t *) T_OPT_DATA(oh) =
						    t->options.req.tco.tco_priority;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_EXPD:
						oh->len = _T_LENGTH_SIZEOF(t->options.req.tco.tco_expd);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_EXPD;
						oh->status = T_SUCCESS;
						/* refresh current value */
						*(t_uscalar_t *) T_OPT_DATA(oh) = t->options.req.tco.tco_expd;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_LTPDU:
						oh->len = _T_LENGTH_SIZEOF(t->options.req.tco.tco_ltpdu);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_LTPDU;
						oh->status = T_SUCCESS;
						/* refresh current value */
						*(t_uscalar_t *) T_OPT_DATA(oh) =
						    t->options.req.tco.tco_ltpdu;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_ACKTIME:
						oh->len = _T_LENGTH_SIZEOF(t->options.req.tco.tco_acktime);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_ACKTIME;
						oh->status = T_SUCCESS;
						/* refresh current value */
						*(t_uscalar_t *) T_OPT_DATA(oh) =
						    t->options.req.tco.tco_acktime;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_REASTIME:
						oh->len = _T_LENGTH_SIZEOF(t->options.req.tco.tco_reastime);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_REASTIME;
						oh->status = T_SUCCESS;
						/* refresh current value */
						*(t_uscalar_t *) T_OPT_DATA(oh) =
						    t->options.req.tco.tco_reastime;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_PREFCLASS:
						oh->len = _T_LENGTH_SIZEOF(t->options.req.tco.tco_prefclass);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_PREFCLASS;
						oh->status = T_SUCCESS;
						/* refresh current value */
						*(t_uscalar_t *) T_OPT_DATA(oh) =
						    t->options.req.tco.tco_prefclass;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_ALTCLASS1:
						oh->len = _T_LENGTH_SIZEOF(t->options.req.tco.tco_altclass1);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_ALTCLASS1;
						oh->status = T_SUCCESS;
						/* refresh current value */
						*(t_uscalar_t *) T_OPT_DATA(oh) =
						    t->options.req.tco.tco_altclass1;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_ALTCLASS2:
						oh->len = _T_LENGTH_SIZEOF(t->options.req.tco.tco_altclass2);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_ALTCLASS2;
						oh->status = T_SUCCESS;
						/* refresh current value */
						*(t_uscalar_t *) T_OPT_DATA(oh) =
						    t->options.req.tco.tco_altclass2;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_ALTCLASS3:
						oh->len = _T_LENGTH_SIZEOF(t->options.req.tco.tco_altclass3);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_ALTCLASS3;
						oh->status = T_SUCCESS;
						/* refresh current value */
						*(t_uscalar_t *) T_OPT_DATA(oh) =
						    t->options.req.tco.tco_altclass3;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_ALTCLASS4:
						oh->len = _T_LENGTH_SIZEOF(t->options.req.tco.tco_altclass4);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_ALTCLASS4;
						oh->status = T_SUCCESS;
						/* refresh current value */
						*(t_uscalar_t *) T_OPT_DATA(oh) =
						    t->options.req.tco.tco_altclass4;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_EXTFORM:
						oh->len = _T_LENGTH_SIZEOF(t->options.req.tco.tco_extform);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_EXTFORM;
						oh->status = T_SUCCESS;
						/* refresh current value */
						*(t_uscalar_t *) T_OPT_DATA(oh) =
						    t->options.req.tco.tco_extform;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_FLOWCTRL:
						oh->len = _T_LENGTH_SIZEOF(t->options.req.tco.tco_flowctrl);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_FLOWCTRL;
						oh->status = T_SUCCESS;
						/* refresh current value */
						*(t_uscalar_t *) T_OPT_DATA(oh) =
						    t->options.req.tco.tco_flowctrl;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_CHECKSUM:
						oh->len = _T_LENGTH_SIZEOF(t->options.req.tco.tco_checksum);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_CHECKSUM;
						oh->status = T_SUCCESS;
						/* refresh current value */
						*(t_uscalar_t *) T_OPT_DATA(oh) =
						    t->options.req.tco.tco_checksum;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_NETEXP:
						oh->len = _T_LENGTH_SIZEOF(t->options.req.tco.tco_netexp);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_NETEXP;
						oh->status = T_SUCCESS;
						/* refresh current value */
						*(t_uscalar_t *) T_OPT_DATA(oh) =
						    t->options.req.tco.tco_netexp;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_NETRECPTCF:
						oh->len = _T_LENGTH_SIZEOF(t->options.req.tco.tco_netrecptcf);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_NETRECPTCF;
						oh->status = T_SUCCESS;
						/* refresh current value */
						*(t_uscalar_t *) T_OPT_DATA(oh) =
						    t->options.req.tco.tco_netrecptcf;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_SELECTACK:
						oh->len = _T_LENGTH_SIZEOF(t->options.req.tco.tco_selectack);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_SELECTACK;
						oh->status = T_SUCCESS;
						/* refresh current value */
						*(t_uscalar_t *) T_OPT_DATA(oh) =
						    t->options.req.tco.tco_selectack;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_REQUESTACK:
						oh->len = _T_LENGTH_SIZEOF(t->options.req.tco.tco_requestack);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_REQUESTACK;
						oh->status = T_SUCCESS;
						/* refresh current value */
						*(t_uscalar_t *) T_OPT_DATA(oh) =
						    t->options.req.tco.tco_requestack;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCO_NBLKEXPDATA:
						oh->len =
						    _T_LENGTH_SIZEOF(t->options.req.tco.tco_nblkexpdata);
						oh->level = T_ISO_TP;
						oh->name = T_TCO_NBLKEXPDATA;
						oh->status = T_SUCCESS;
						/* refresh current value */
						*(t_uscalar_t *) T_OPT_DATA(oh) =
						    t->options.req.tco.tco_nblkexpdata;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
					}
					break;
				case T_CLTS:
					switch (ih->name) {
					default:
						oh->len = sizeof(*oh);
						oh->level = ih->level;
						oh->name = ih->name;
						oh->status = t_overall_result(&overall, T_NOTSUPPORT);
						continue;
					case T_ALLOPT:
					case T_TCL_TRANSDEL:
						oh->len = _T_LENGTH_SIZEOF(t->options.req.tcl.tcl_transdel);
						oh->level = T_ISO_TP;
						oh->name = T_TCL_TRANSDEL;
						oh->status = T_SUCCESS;
						/* refresh current value */
						*(struct transdel *) T_OPT_DATA(oh) =
						    t->options.req.tcl.tcl_transdel;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCL_RESERRORRATE:
						oh->len =
						    _T_LENGTH_SIZEOF(t->options.req.tcl.tcl_reserrorrate);
						oh->level = T_ISO_TP;
						oh->name = T_TCL_RESERRORRATE;
						oh->status = T_SUCCESS;
						/* refresh current value */
						*(struct rate *) T_OPT_DATA(oh) =
						    t->options.req.tcl.tcl_reserrorrate;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCL_PROTECTION:
						oh->len = _T_LENGTH_SIZEOF(t->options.req.tcl.tcl_protection);
						oh->level = T_ISO_TP;
						oh->name = T_TCL_PROTECTION;
						oh->status = T_SUCCESS;
						/* refresh current value */
						*(t_uscalar_t *) T_OPT_DATA(oh) =
						    t->options.req.tcl.tcl_protection;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCL_PRIORITY:
						oh->len = _T_LENGTH_SIZEOF(t->options.req.tcl.tcl_priority);
						oh->level = T_ISO_TP;
						oh->name = T_TCL_PRIORITY;
						oh->status = T_SUCCESS;
						/* refresh current value */
						*(t_uscalar_t *) T_OPT_DATA(oh) =
						    t->options.req.tcl.tcl_priority;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
						/* fall through */
					case T_TCL_CHECKSUM:
						oh->len = _T_LENGTH_SIZEOF(t->options.req.tcl.tcl_checksum);
						oh->level = T_ISO_TP;
						oh->name = T_TCL_CHECKSUM;
						oh->status = T_SUCCESS;
						/* refresh current value */
						*(t_uscalar_t *) T_OPT_DATA(oh) =
						    t->options.req.tcl.tcl_checksum;
						if (ih->name != T_ALLOPT)
							continue;
						if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
							goto efault;
					}
					break;
				}
				if (ih->level != T_ALLLEVELS)
					continue;
				switch (t->p.prot.type) {
				case TP_CMINOR_COTS:
				case TP_CMINOR_CLTS:
				case TP_CMINOR_COTS_IP:
				case TP_CMINOR_CLTS_IP:
				case TP_CMINOR_COTS_UDP:
				case TP_CMINOR_CLTS_UDP:
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
					break;
				}
				break;
			default:
				if (ih->level != T_ALLLEVELS)
					goto einval;
				break;
			}
			/* fall through */
		case T_INET_IP:
			switch (t->p.prot.type) {
			case TP_CMINOR_COTS:
			case TP_CMINOR_CLTS:
			case TP_CMINOR_COTS_IP:
			case TP_CMINOR_CLTS_IP:
			case TP_CMINOR_COTS_UDP:
			case TP_CMINOR_CLTS_UDP:
				switch (ih->name) {
				default:
					oh->len = sizeof(*oh);
					oh->level = ih->level;
					oh->name = ih->name;
					oh->status = t_overall_result(&overall, T_NOTSUPPORT);
					continue;
				case T_ALLOPT:
				case T_IP_OPTIONS:
					oh->len = sizeof(*oh);
					oh->level = T_INET_IP;
					oh->name = T_IP_OPTIONS;
					oh->status = t_overall_result(&overall, T_NOTSUPPORT);
					/* not supported yet */
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
					/* fall through */
				case T_IP_TOS:
					oh->len = _T_LENGTH_SIZEOF(t->options.req.ip.ip_tos);
					oh->level = T_INET_IP;
					oh->name = T_IP_TOS;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((unsigned char *) T_OPT_DATA(oh)) = t->options.req.ip.ip_tos;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
					/* fall through */
				case T_IP_TTL:
					oh->len = _T_LENGTH_SIZEOF(t->options.req.ip.ip_ttl);
					oh->level = T_INET_IP;
					oh->name = T_IP_TTL;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((unsigned char *) T_OPT_DATA(oh)) = t->options.req.ip.ip_ttl;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
					/* fall through */
				case T_IP_REUSEADDR:
					oh->len = _T_LENGTH_SIZEOF(t->options.req.ip.ip_reuseaddr);
					oh->level = T_INET_IP;
					oh->name = T_IP_REUSEADDR;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((unsigned int *) T_OPT_DATA(oh)) = t->options.req.ip.ip_reuseaddr;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
					/* fall through */
				case T_IP_DONTROUTE:
					oh->len = _T_LENGTH_SIZEOF(t->options.req.ip.ip_dontroute);
					oh->level = T_INET_IP;
					oh->name = T_IP_DONTROUTE;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((unsigned int *) T_OPT_DATA(oh)) = t->options.req.ip.ip_dontroute;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
					/* fall through */
				case T_IP_BROADCAST:
					oh->len = _T_LENGTH_SIZEOF(t->options.req.ip.ip_broadcast);
					oh->level = T_INET_IP;
					oh->name = T_IP_BROADCAST;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((unsigned int *) T_OPT_DATA(oh)) = t->options.req.ip.ip_broadcast;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
					/* fall through */
				case T_IP_ADDR:
					oh->len = _T_LENGTH_SIZEOF(t->options.req.ip.ip_addr);
					oh->level = T_INET_IP;
					oh->name = T_IP_ADDR;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((uint32_t *) T_OPT_DATA(oh)) = t->options.req.ip.ip_addr;
					if (ih->name != T_ALLOPT)
						continue;
				}
				if (ih->level != T_ALLLEVELS)
					continue;
				switch (t->p.prot.type) {
				case TP_CMINOR_COTS:
				case TP_CMINOR_CLTS:
				case TP_CMINOR_COTS_UDP:
				case TP_CMINOR_CLTS_UDP:
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
					break;
				}
				break;
			default:
				if (ih->level != T_ALLLEVELS)
					goto einval;
				break;
			}
			/* fall through */
		case T_INET_UDP:
			switch (t->p.prot.type) {
			case TP_CMINOR_COTS:
			case TP_CMINOR_CLTS:
			case TP_CMINOR_COTS_UDP:
			case TP_CMINOR_CLTS_UDP:
				switch (ih->name) {
				default:
					oh->len = sizeof(*oh);
					oh->level = ih->level;
					oh->name = ih->name;
					oh->status = t_overall_result(&overall, T_NOTSUPPORT);
					continue;
				case T_ALLOPT:
				case T_UDP_CHECKSUM:
					oh->len = _T_LENGTH_SIZEOF(t->options.req.udp.udp_checksum);
					oh->level = T_INET_UDP;
					oh->name = T_UDP_CHECKSUM;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.req.udp.udp_checksum;
					if (ih->name != T_ALLOPT)
						continue;
				}
				if (ih->level != T_ALLLEVELS)
					continue;
				break;
			default:
				if (ih->level != T_ALLLEVELS)
					goto einval;
				break;
			}
		}
	}
	if (ih && !oh)
		goto efault;
	if (oh)
		*olen = (unsigned char *) oh - op;
	return (overall);
      einval:
	return (-EINVAL);
      efault:
	LOGERR(t, "SWERR: %s %s:%d", __FUNCTION__, __FILE__, __LINE__);
	return (-EFAULT);
}

static t_scalar_t
t_build_check_options(const struct tp *t, const unsigned char *ip, size_t ilen, unsigned char *op,
		      size_t *olen)
{
	return (0);
}

static t_scalar_t
t_build_negotiate_options(struct tp *t, const unsigned char *ip, size_t ilen, unsigned char *op, size_t *olen)
{
	return (0);
}

/**
 * t_build_options: - build options
 * @tp: private structure (locked)
 * @ip: input options pointer
 * @ilen: input options length
 * @op: output options pointer
 * @olen: output options length (returned)
 * @flags: options management flag
 *
 * Perform the actions required of T_DEFAULT, T_CURRENT, T_CHECK or T_NEGOTIARE, placing the output
 * in the provided buffer.
 */
static t_scalar_t
t_build_options(struct tp *t, unsigned char *ip, size_t ilen, unsigned char *op, size_t *olen,
		t_scalar_t flag)
{
	switch (flag) {
	case T_DEFAULT:
		return t_build_default_options(t, ip, ilen, op, olen);
	case T_CURRENT:
		return t_build_current_options(t, ip, ilen, op, olen);
	case T_CHECK:
		return t_build_check_options(t, ip, ilen, op, olen);
	case T_NEGOTIATE:
		return t_build_negotiate_options(t, ip, ilen, op, olen);
	}
	return (-EINVAL);
}

/*
 *  =========================================================================
 *
 *  Address Handling
 *
 *  =========================================================================
 *
 *  Addressing:
 *
 *  ISO:
 *
 *  49			AFI (Local Binary)
 *  --			IDI (NULL)
 *  XXXX		Routing Domain/Area Code (Subnetwork)
 *  HH:HH:HH:HH:HH:HH	MAC Address (first octet 0 for 802.3)
 *  FE			NSel (LSAP)
 *  binary length = 10 octets with NSel
 *
 *  ETSI TS 300 612-2
 *  ISO-IP: (OSINET format NSAP)
 *  47			AFI (ICD Binary)
 *  0004		IDI/ICD (OSINET ICD)
 *  XXXX		OG (Organization Number)
 *  FFFF		SN (Subnetwork) 0xFFFF unspecified subnetwork
 *  HHHHHHHHHHHH	ID (Station Identifier)
 *  FE			Sel (Network Selector)  LSAP?
 *  binary length = 14 octets with NSel
 *
 *  RFC 1069 -IP NSAPs
 *  ISO-IP:
 *
 *  47			AFI (ICD Binary)
 *  0006		IDI/ICD (Internet ICD)
 *  02			DFI Version
 *  0000		Global Area
 *  XXXX		Routing Domain
 *  00000000000000	Padding (7 octets of zeros)
 *  AABBCCDD		IPv4 Address
 *  00			NSel (LSAP)
 *  binary length = 20 octets with NSel
 *
 *  Note: The selector field performs the same function as the user
 *  protocol field in the IP header.  This is necessary because the
 *  ISO protocol considers identification of the user protocol to be
 *  an addressing issue, and therefore does not allow for the user
 *  protocol to be specified in the protocol header independently from
 *  address.
 *
 *  ISO-UDP: ANSI 2/6/1 format (RFC 982)
 *
 *  47			AFI (ICD Binary)
 *  0006		IDI/ICD (Internet ICD)
 *  02			DFI Version
 *  0000		Global Area
 *  XXXX		Routing Domain
 *  000000		Padding
 *  XXXX		Area Code
 *  AABBCCDDPPPP	IPv4 Address and port
 *  00			NSel (LSAP)
 *  binary length = 20 octets with NSel
 *
 *  RFC 1237/RFC 1629 GOSIP 2/ANSI NSAPS
 *
 *  GOSIP 2:
 *  47			AFI (ICD Binary)
 *  0005		IDI/ICD (GSA)
 *  80			DFI Version
 *  XXXXXX		AA Administrative Authority
 *  0000		Rsvd Reserved
 *  XXXX		Routing Domain
 *  0000		Area Identifier
 *  HHHHHHHHHHHH	ID System Identifier
 *  00			NSel (See X.264) [00-NET, 01-X.224, 02-X.234, 03-X.274/X.224, 04-X.274/X.234]
 *
 *  ANSI:
 *  39			AFI (ISO DCC Binary)
 *  0840		US (ANSI)
 *  80			DFI Version
 *  XXXXXX		AA Administrative Authority
 *  0000		Reserved
 *  XXXX		Routing Domain
 *  0000		Area Identifier
 *  HHHHHHHHHHHH	ID System Identifier
 *  00			NSel (See X.264) [00-NET, 01-X.224, 02-X.234, 03-X.274/X.224, 04-X.274/X.234]
 *
 *  RFC 1277 - RFC 1006 NSAPs
 *
 *  54			AFI (F.69 Decimal)
 *  00			F.69 prefix
 *  728722		Hypothetical telex number
 *  03			Prefix (RFC1006)
 *  AAABBBCCCDDD	IPv4 Address (decimal)
 *  PPPPP		Port number (decimal)
 *  TTTTT		Transport selection mask (1 TCP, 2 UDP, how about 4 SCTP)
 *  00			NSel
 *  binary length = 18 with NSel
 *
 *  RFC 4548 - IP NSAPs
 *
 *  35			AFI (IANA ICP Binary)
 *  0001		IPv4 (IPv6 is 0000)
 *  AABBCCDD		IPv4 address
 *  00...		12 octets of zeros
 *  00			NSel (LSAP)
 *  binary length = 20 octets with NSel
 *
 *  RFC 1768 - Multicast NSAPs for CLNS and (ES-IS/IS-IS) extensions
 *
 *  Typical trick: use
 *
 *  49                AFI (Local Binary)
 *  --                IDI (NULL)
 *  XXXX              Routing Domain/Area Code (Subnetwork)
 *  HH:HH:HH:HH:HH:HH MAC Address
 *  00                NSel (00 for NET, FE for ES)
 *  binary length = 10 octets with NSel
 */

/**
 * tp_addr_parse: - extract SNPA address from NSAP address
 * @nsap: pointer to NSAP address
 * @snpa: pointer to SNPA address
 */
static int
tp_addr_parse(struct nsapaddr *nsap, struct snpaaddr *snpa)
{
	if (nsap->len < 3 || nsap->len > 20)
		return (-EFAULT);
	switch (nsap->addr[0]) {
/* RFC 4548 NSAPs */
	case 0x35:		/* ind IANA ICP binary */
	case 0xb9:		/* grp IANA ICP binary */
		if (nsap->len < 20)
			return (-EMSGSIZE);
		if (nsap->addr[1] != 0x00)
			return (-EINVAL);
		switch (nsap->addr[2]) {
		case 0x00:	/* IPv6 Address */
			snpa->type = TP_SNPA_TYPE_IP6;
			bcopy(nsap->addr + 3, snpa->addr, 16);
			snpa->subnet[0] = 0xff;
			snpa->subnet[1] = 0xff;
			snpa->lsap = nsap->addr[19];
			break;
		case 0x01:	/* IPv4 Address */
			snpa->type = TP_SNPA_TYPE_IP4;
			bcopy(nsap->addr + 3, snpa->addr, 4);
			snpa->subnet[0] = 0xff;
			snpa->subnet[1] = 0xff;
			snpa->lsap = nsap->addr[19];
			break;
		}
		break;
/* ANSI 2/6/1 (also ECMA 117) */
	case 0x39:		/* ind ISO DCC binary */
	case 0xbd:		/* grp ISO DCC binary */
		if (nsap->len < 20)
			return (-EMSGSIZE);
		if (nsap->addr[1] != 0x00 || nsap->addr[2] != 0x40)
			return (-EINVAL);
		if (nsap->addr[3] != 0x80)
			return (-EINVAL);
		if (nsap->addr[7] != 0x00 || nsap->addr[8] != 0x00)
			return (-EINVAL);
		snpa->type = TP_SNPA_TYPE_MAC;
		bcopy(nsap->addr + 13, snpa->addr, 6);
		snpa->subnet[0] = nsap->addr[11];
		snpa->subnet[1] = nsap->addr[12];
		snpa->lsap = nsap->addr[19];
		break;
/* Various ICD binary formats */
	case 0x47:		/* ind ICD binary */
	case 0xc5:		/* grp ICD binary */
		if (nsap->len < 20)
			return (-EMSGSIZE);
		if (nsap->addr[1] != 0x00)
			return (-EINVAL);
		switch (nsap->addr[2]) {
		case 0x04:	/* OSINET (ETSI TS 300 612-2 and ISO-IP */
			snpa->type = TP_SNPA_TYPE_MAC;
			bcopy(nsap->addr + 7, snpa->addr, 6);
			snpa->subnet[0] = nsap->addr[5];
			snpa->subnet[1] = nsap->addr[6];
			snpa->lsap = nsap->addr[19];
			/* should be LSAP */
			break;
		case 0x05:	/* GSA-GOSIP 2 */
			snpa->type = TP_SNPA_TYPE_MAC;
			bcopy(nsap->addr + 13, snpa->addr, 6);
			snpa->subnet[0] = nsap->addr[12];
			snpa->subnet[1] = nsap->addr[13];
			snpa->lsap = nsap->addr[19];
			/* 00-NET, 01-X.224, 02-X.234, 03-X.274/224, 04-X.274/234 */
			break;
		case 0x06:	/* DND-Internet */
			switch (nsap->addr[3]) {
			case 0x02:	/* DFI Version 2 */
				if (nsap->addr[13] == 0x00) {
					/* RFC 1069 - IP NSAP */
					snpa->type = TP_SNPA_TYPE_IP4;
					bcopy(nsap->addr + 15, snpa->addr, 4);
					snpa->subnet[0] = 0x00;
					snpa->subnet[1] = 0x00;
				} else {
					/* ISO-IP ANSI 2/6/1 format (RFC 982) */
					snpa->type = TP_SNPA_TYPE_IP4_PORT;
					bcopy(nsap->addr + 13, snpa->addr, 4);
					snpa->subnet[0] = nsap->addr[17];
					snpa->subnet[1] = nsap->addr[18];

				}
				break;
			case 0x03:	/* DFI Version 3 */
				/* RFC 1070 - UDP NSAPs */
				snpa->type = TP_SNPA_TYPE_IP4_PORT;
				bcopy(nsap->addr + 15, snpa->addr, 4);
				snpa->subnet[0] = nsap->addr[11];
				snpa->subnet[1] = nsap->addr[12];
				break;
			default:
				return (-EINVAL);
			}
			snpa->lsap = nsap->addr[19];
			/* 00-NET, 01-X.224, 02-X.234, 03-X.274/224, 04-X.274/234 */
			break;
		default:
			return (-EINVAL);
		}
		break;
	case 0x49:		/* ind Local binary */
	case 0xc7:		/* grp Local binary */
		if (nsap->len < 10)
			return (-EMSGSIZE);
		snpa->type = TP_SNPA_TYPE_MAC;
		bcopy(nsap->addr + 3, snpa->addr, 6);
		snpa->subnet[0] = nsap->addr[1];
		snpa->subnet[1] = nsap->addr[2];
		snpa->lsap = nsap->addr[9];
		break;
	case 0x50:		/* ind Local text: ISO/IEC 646 */
	case 0x51:		/* ind Local text: national character */
		if (nsap->len < 3 || nsap->len > 16)
			return (-EMSGSIZE);
		snpa->type = TP_SNPA_TYPE_IFNAME;
		bcopy(nsap->addr + 1, snpa->addr, nsap->len - 2);
		snpa->addr[nsap->len - 2] = 0x00;	/* zero terminated string */
		snpa->subnet[0] = 0xff;
		snpa->subnet[1] = 0xff;
		snpa->lsap = nsap->addr[nsap->len - 1];
		break;
	case 0x40:		/* ind F.69 decimal */
	case 0x54:		/* ind F.69 decimal */
	case 0xbe:		/* grp F.69 decimal */
	case 0xcc:		/* grp F.69 decimal */
	{
		unsigned short subnet;

		if (nsap->len < 18)
			return (-EMSGSIZE);
		if (nsap->addr[1] != 0x00)
			return (-EINVAL);
		/* hypothetical F.69 prefix */
		if (nsap->addr[2] != 0x72 || nsap->addr[3] != 0x87 || nsap->addr[4] != 0x22)
			return (-EINVAL);
		snpa->type = TP_SNPA_TYPE_IP4_PORT;
		snpa->addr[0] =
		    (((nsap->addr[6] >> 4) & 0x0f) * 100) + (((nsap->addr[6] >> 0) & 0x0f) * 10) +
		    (((nsap->addr[7] >> 4) & 0x0f) * 1);
		snpa->addr[1] =
		    (((nsap->addr[7] >> 0) & 0x0f) * 100) + (((nsap->addr[8] >> 4) & 0x0f) * 10) +
		    (((nsap->addr[8] >> 0) & 0x0f) * 1);
		snpa->addr[2] =
		    (((nsap->addr[9] >> 4) & 0x0f) * 100) + (((nsap->addr[9] >> 0) & 0x0f) * 10) +
		    (((nsap->addr[10] >> 4) & 0x0f) * 1);
		snpa->addr[3] =
		    (((nsap->addr[10] >> 0) & 0x0f) * 100) + (((nsap->addr[11] >> 4) & 0x0f) * 10) +
		    (((nsap->addr[11] >> 0) & 0x0f) * 1);
		subnet =
		    (((nsap->addr[12] >> 4) & 0x0f) * 10000) + (((nsap->addr[12] >> 0) & 0x0f) * 1000) +
		    (((nsap->addr[13] >> 4) & 0x0f) * 100) + (((nsap->addr[13] >> 0) & 0x0f) * 10) +
		    (((nsap->addr[14] >> 4) & 0x0f) * 1);
		snpa->subnet[0] = (subnet >> 8) * 0x00ff;
		snpa->subnet[1] = (subnet >> 0) * 0x00ff;
		switch (nsap->addr[17]) {
		case 0x01:	/* TCP, ipproto = 6 = 0x06 */
			snpa->lsap = 0x06;
			return (-EPFNOSUPPORT);
			break;
		case 0x02:	/* UDP, ipproto = 17 = 0x11 */
			snpa->lsap = 0x11;
			break;
		case 0x04:	/* SCTP, ipproto = 132 = 0x84 */
			snpa->lsap = 0x84;
			return (-EPFNOSUPPORT);
			break;
		default:
			return (-EINVAL);
		}
		break;
	}
	default:
		return (-EAFNOSUPPORT);
	}
	nsap->grp = (nsap->addr[0] > (unsigned char) 0xb0U);
	return (0);
}

/**
 * tp_addr_type: - extract SNPA address
 * @add: on call, pointer to NSAP, on return pointer to SNPA in NSAP
 * @len: on call, length of NSAP, on return length of SNPA in NSAP
 * @grp: on return, set when address is a group address
 * @sub: on return, the subnetwork of the SNPA (0xffff unspecified)
 * @sel: on return, the NSel byte implied or present in the NSAP
 *
 * Extracts and determines the type of an SNPA address found in an NSAP address.  Returns a positive
 * SNAP address type on success and provides a pointer to the SNPA address and its length in the
 * @add and @len locations.  On failure, returns a negative error code and does not alter the
 * locations @add nor @len.
 */
static int
tp_addr_type(caddr_t *add, size_t *len, int *grp, unsigned short *sub, unsigned char *sel)
{
	caddr_t nsap_add, snpa_add = NULL;
	size_t nsap_len, snpa_len = 0;
	unsigned short snpa_sub = 0xffff;
	unsigned char snpa_sel = 0xfe;

	if (add == NULL || len == NULL)
		return (-EFAULT);
	nsap_add = *add;
	nsap_len = *len;
	if (nsap_add == NULL || nsap_len <= 0)
		return (-EFAULT);
	if (nsap_len > 20)
		return (-EMSGSIZE);
	if (nsap_len < 1)
		return (-EINVAL);
	switch ((unsigned char) nsap_add[0]) {
/* RFC 4548 NSAPs */
	case 0x35:		/* ind IANA ICP binary */
	case 0xb9:		/* grp IANA ICP binary */
		if (nsap_len < 20)
			return (-EMSGSIZE);
		if (nsap_add[1] != 0x00)
			return (-EINVAL);
		switch (nsap_add[2]) {
		case 0x00:	/* IPv6 Address */
			snpa_add = nsap_add + 3;
			snpa_len = 16;
			snpa_sub = 0xffff;
			snpa_sel = nsap_add[19];	/* should be ipproto */
			break;
		case 0x01:	/* IPv4 Address */
			snpa_add = nsap_add + 3;
			snpa_len = 4;
			snpa_sub = 0xffff;
			snpa_sel = nsap_add[19];	/* should be ipproto */
			break;
		default:
			return (-EINVAL);
		}
		break;
/* ANSI 2/6/1 (also ECMA 117) */
	case 0x39:		/* ind ISO DCC binary */
	case 0xbd:		/* grp ISO DCC binary */
		if (nsap_len < 20)
			return (-EMSGSIZE);
		if (nsap_add[1] != 0x08 || nsap_add[2] != 0x40)
			return (-EINVAL);
		if (nsap_add[3] != 0x80)
			return (-EINVAL);
		if (nsap_add[7] != 0x00 || nsap_add[8] != 0x00)
			return (-EINVAL);
		snpa_add = nsap_add + 13;
		snpa_len = 6;
		snpa_sub = (nsap_add[11] << 8) | (nsap_add[12]);
		snpa_sel = nsap_add[19];	/* 00-NET, 01-X.224, 02-X.234, 03-X.274/224, 04-X.274/234 */
		break;
/* Various ICD binary formats */
	case 0x47:		/* ind ICD binary */
	case 0xc5:		/* grp ICD binary */
		if (nsap_len < 20)
			return (-EMSGSIZE);
		if (nsap_add[1] != 0x00)
			return (-EINVAL);
		switch (nsap_add[2]) {
		case 0x04:	/* OSINET (ETSI TS 300 612-2 and ISO-IP) */
			snpa_add = nsap_add + 7;
			snpa_len = 6;
			snpa_sub = (nsap_add[5] << 8) | (nsap_add[6]);
			snpa_sel = nsap_add[19];	/* should be LSAP */
			break;
		case 0x05:	/* GSA-GOSIP 2 */
			snpa_add = nsap_add + 13;
			snpa_len = 6;
			snpa_sub = (nsap_add[12] << 8) | (nsap_add[13]);
			snpa_sel = nsap_add[19];	/* 00-NET, 01-X.224, 02-X.234, 03-X.274/224,
							   04-X.274/234 */
			break;
		case 0x06:	/* DND-Internet */
			switch (nsap_add[3]) {
			case 0x02:	/* DFI Version 2 */
				if (nsap_add[13] == 0x00) {
					/* RFC 1069 - IP NSAP */
					snpa_add = nsap_add + 15;
					snpa_len = 4;
					snpa_sub = 0x00;	/* port number in subnet */
				} else {
					/* ISO-IP ANSI 2/6/1 format (RFC 982) */
					snpa_add = nsap_add + 13;
					snpa_len = 4;
					snpa_sub = (nsap_add[17] << 8) | (nsap_add[18]);	/* port
												   number in
												   subnet */
				}
				break;
			case 0x03:	/* DFI Version 3 */
				/* RFC 1070 - UDP NSAPs */
				snpa_add = nsap_add + 15;
				snpa_len = 4;
				snpa_sub = (nsap_add[11] << 8) | (nsap_add[12]);	/* port number in
											   subnet */
				break;
			default:
				return (-EINVAL);

			}
			snpa_sel = nsap_add[19];	/* 00-NET, 01-X.224, 02-X.234, 03-X.274/224,
							   04-X.274/234 */
			break;
		default:
			return (-EINVAL);
		}
		break;
	case 0x49:		/* ind Local binary */
	case 0xc7:		/* grp Local binary */
		if (nsap_len < 10)
			return (-EMSGSIZE);
		snpa_add = nsap_add + 3;
		snpa_len = 6;
		snpa_sub = (nsap_add[1] << 8) | (nsap_add[2]);
		snpa_sel = nsap_add[9];	/* 0x00 for NET, 0xFE for ES */
		break;
	case 0x50:		/* ind Local text: ISO/IEC 646 */
	case 0x51:		/* ind Local text: national character */
		if (nsap_len < 3)
			return (-EMSGSIZE);
		snpa_add = nsap_add + 1;
		snpa_len = nsap_len - 2;
		snpa_sub = 0xffff;
		snpa_sel = nsap_add[nsap_len - 1];
		break;
	case 0x40:		/* ind F.69 decimal */
	case 0x54:		/* ind F.69 decimal */
	case 0xbe:		/* grp F.69 decimal */
	case 0xcc:		/* grp F.69 decimal */
	{
		static char addr[4] = { 0, };

		if (nsap_len < 18)
			return (-EMSGSIZE);
		if (nsap_add[1] != 0x00 || nsap_add[2] != 0x72 || nsap_add[3] != 0x87 || nsap_add[4] != 0x22
		    || nsap_add[5] != 0x03)
			return (-EINVAL);
		addr[0] =
		    (((nsap_add[6] >> 4) & 0x0f) * 100) + (((nsap_add[6] >> 0) & 0x0f) * 10) +
		    (((nsap_add[7] >> 4) & 0x0f) * 1);
		addr[1] =
		    (((nsap_add[7] >> 0) & 0x0f) * 100) + (((nsap_add[8] >> 4) & 0x0f) * 10) +
		    (((nsap_add[8] >> 0) & 0x0f) * 1);
		addr[2] =
		    (((nsap_add[9] >> 4) & 0x0f) * 100) + (((nsap_add[9] >> 0) & 0x0f) * 10) +
		    (((nsap_add[10] >> 4) & 0x0f) * 1);
		addr[3] =
		    (((nsap_add[10] >> 0) & 0x0f) * 100) + (((nsap_add[11] >> 4) & 0x0f) * 10) +
		    (((nsap_add[11] >> 0) & 0x0f) * 1);
		snpa_add = addr;
		snpa_len = 4;
		snpa_sub =
		    (((nsap_add[12] >> 4) & 0x0f) * 10000) + (((nsap_add[12] >> 0) & 0x0f) * 1000) +
		    (((nsap_add[13] >> 4) & 0x0f) * 100) + (((nsap_add[13] >> 0) & 0x0f) * 10) +
		    (((nsap_add[14] >> 4) & 0x0f) * 1);
		switch (nsap_add[17]) {
		case 0x01:	/* TCP, ipproto = 6 = 0x06 */
			snpa_sel = 0x06;
			return (-EPFNOSUPPORT);
			break;
		case 0x02:	/* UDP, ipproto = 17 = 0x11 */
			snpa_sel = 0x11;
			break;
		case 0x04:	/* SCTP, ipproto = 132 = 0x84 */
			snpa_sel = 0x84;
			return (-EPFNOSUPPORT);
			break;
		default:
			return (-EINVAL);
		}
		break;
	}
	default:
		return (-EAFNOSUPPORT);
	}
	*add = snpa_add;
	*len = snpa_len;
	if (grp != NULL)
		*grp = ((unsigned char) nsap_add[0] > (unsigned char) 0xb0U);
	if (sub != NULL)
		*sub = snpa_sub;
	if (sel != NULL)
		*sel = snpa_sel;
	return (snpa_len);
}

/**
 * tp_addr_size: - calculate addresss size
 * @add: address buffer
 * @len: buffer length
 *
 * Calculates and returns the size of the transport address based on the address format indicator
 * (AFI) contained in the address.
 */
static inline fastcall __hot int
tp_addr_size(caddr_t add, size_t len)
{
	if (len < 1)
		return (0);
	switch ((unsigned char) add[0]) {
	case 0x35:		/* ind IANA ICP binary */
	case 0xb9:		/* grp IANA ICP binary */
		if (len < 20)
			return (0);
		if (add[1] != 0x00 || (add[2] != 0x00 && add[2] != 0x01))
			return (0);
		return (20);

	case 0x39:		/* ind ISO DCC binary */
	case 0xbd:		/* grp ISO DCC binary */
		if (len < 20)
			return (0);
		if (add[3] != 0x80)
			return (0);
		return (20);

	case 0x47:		/* ind ICD binary */
	case 0xc5:		/* grp ICD binary */
		if (len < 20)
			return (0);
		if (add[1] != 0x00)
			return (0);
		switch (add[2]) {
		case 0x04:	/* OSINET */
			return (14);
		case 0x05:	/* GSA-GOSIP */
		case 0x06:	/* DND */
			switch (add[3]) {
			case 0x02:	/* DFI version 2 */
			case 0x03:	/* DFI version 3 */
				return (20);
			}
		}
		return (0);
	case 0x49:		/* ind Local binary */
	case 0xc7:		/* grp Local binary */
		if (len < 10)
			return (0);
		return (10);

	case 0x50:		/* ind Local text: ISO/IEC 646 */
	case 0x51:		/* ind Local text: national character */
		if (len < 5)
			return (0);
		return ((int) len);

	case 0x40:		/* ind F.69 decimal */
	case 0x54:		/* ind F.69 decimal */
	case 0xbe:		/* grp F.69 decimal */
	case 0xcc:		/* grp F.69 decimal */
		if (len < 18)
			return (0);
		if (add[1] != 0x00)
			return (0);
		if (add[2] != 0x72 || add[3] != 0x87 || add[4] != 0x22 || add[5] != 0x03)
			return (0);
		if ((add[14] & 0x0f) != 0x00 || add[15] != 0x00
		    || (add[16] != 0x01 && add[16] != 0x02 && add[16] != 0x04))
			return (0);
		return (18);
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
 *  Timers
 *
 *  =========================================================================
 */
enum {
	tall,				/* all timers */
	t1,				/* retransmission timer */
	t2,				/* inactivity timer */
	t3,				/* window timer */
	t4				/* reference timer */
};

static void
tp_timer_stop(struct tp *tp, const uint timer)
{
	int single = 1;

	switch (timer) {
	case tall:
		single = 0;
		/* fall through */
	case t1:
		mi_timer_stop(tp->timers.t1);
		if (single)
			break;
		/* fall through */
	case t2:
		mi_timer_stop(tp->timers.t2);
		if (single)
			break;
		/* fall through */
	case t3:
		mi_timer_stop(tp->timers.t3);
		if (single)
			break;
		/* fall through */
	case t4:
		mi_timer_stop(tp->timers.t4);
		if (single)
			break;
		/* fall through */
		break;
	default:
		LOGERR(tp, "bad timer value");
		break;
	}
}

static void
tp_timer_start(struct tp *tp, queue_t *q, const uint timer)
{
	switch (timer) {
	case t1:
		((struct tp_timer *) tp->timers.t1->b_rptr)->count = 0;
		mi_timer(q, tp->timers.t1, tp->config.t1);
		break;
	case t2:
		((struct tp_timer *) tp->timers.t2->b_rptr)->count = 0;
		mi_timer(q, tp->timers.t2, tp->config.t2);
		break;
	case t3:
		((struct tp_timer *) tp->timers.t3->b_rptr)->count = 0;
		mi_timer(q, tp->timers.t3, tp->config.t3);
		break;
	case t4:
		((struct tp_timer *) tp->timers.t4->b_rptr)->count = 0;
		mi_timer(q, tp->timers.t4, tp->config.t4);
		break;
	default:
		LOGERR(tp, "bad timer value");
		break;
	}
}

/*
 *  =========================================================================
 *
 *  SENDMSG and RECVMSG
 *
 *  =========================================================================
 */

/* Class options */
#define _T_F_CO_RESERVED4	(1<<3)	/* reserved */
#define _T_F_CO_RESERVED3	(1<<2)	/* reserved */
#define _T_F_CO_EXTFORM		(1<<1)	/* extended format */
#define _T_F_CO_FLOWCTRL	(1<<0)	/* explicit flow control */

/* Additional options - XTI doesn't know about these first three */
#define _T_F_AO_NBLKEXPDATA	(1<<6)	/* non-blocking expedited data */
#define _T_F_AO_REQUESTACK	(1<<5)	/* request acknowledgement */
#define _T_F_AO_SELECTACK	(1<<4)	/* selective acknowledgement */
#define _T_F_AO_NETEXP		(1<<3)	/* network expedited data */
#define _T_F_AO_NETRECPTCF	(1<<2)	/* receipt confirmation */
#define _T_F_AO_CHECKSUM	(1<<1)	/* checksum */
#define _T_F_AO_EXPD		(1<<0)	/* transport expedited data */

/* these are really size = (1<<value) */
#define _T_TPDU_SIZE_8192	0x0d
#define _T_TPDU_SIZE_4096	0x0c
#define _T_TPDU_SIZE_2048	0x0b
#define _T_TPDU_SIZE_1024	0x0a
#define _T_TPDU_SIZE_512	0x09
#define _T_TPDU_SIZE_256	0x08
#define _T_TPDU_SIZE_128	0x07

/* disconnect reasons, these should be in TPI header file */
#define _TP_REASON_UNSPECIFIED		0x00
#define _TP_REASON_TSAP_CONGESTION	0x01
#define _TP_REASON_TSAP_NO_SESSION	0x02	/* persistent */
#define _TP_REASON_ADDRESS_UNKNOWN	0x03	/* persistent */
#define _TP_REASON_NORMAL_DISCONNECT	0x80
#define _TP_REASON_REMOTE_CONGESTION	0x81
#define _TP_REASON_NEGOTIATION_FAILED	0x82	/* persistent */
#define _TP_REASON_DUPLICATE_SREF	0x83
#define _TP_REASON_MISMATCH_REFS	0x84
#define _TP_REASON_PROTOCOL_ERROR	0x85
#define _TP_REASON_NOT_USED1		0x86
#define _TP_REASON_REFERENCE_OVERFLOW	0x87
#define _TP_REASON_REFUSED		0x88
#define _TP_REASON_NOT_USED2		0x89
#define _TP_REASON_INVALID_LENGTH	0x8a

/* error causes, these should be in TPI header file */
#define _TP_ERROR_UNSPECIFIED		0x00
#define _TP_ERROR_INVALID_PARM_TYPE	0x01
#define _TP_ERROR_INVALID_TPDU_TYPE	0x02
#define _TP_ERROR_INVALID_PARM_VALUE	0x03

/* message types */
#define	_TP_MT_ED	0x10	/* can contain data */
#define	_TP_MT_EA	0x20	/* no data */
#define _TP_MT_UD	0x40	/* can contain data */
#define	_TP_MT_RJ	0x50	/* no data */
#define	_TP_MT_AK	0x60	/* no data */
#define	_TP_MT_ER	0x70	/* no data */
#define	_TP_MT_DR	0x80	/* can contain data */
#define	_TP_MT_DC	0xc0	/* no data */
#define	_TP_MT_CC	0xd0	/* can contain data */
#define	_TP_MT_CR	0xe0	/* can contain data */
#define	_TP_MT_DT	0xf0	/* can contain data */

#define _TP_PT_INVALID_TPDU	0xc1	/* invalid TPDU */

#define _TP_PT_TPDU_SIZE	0xc0	/* TPDU size */
#define _TP_PT_CGTRANSSEL	0xc1	/* calling transport selector */
#define _TP_PT_CDTRANSSEL	0xc2	/* called transport selector */
#define _TP_PT_CHECKSUM		0xc3	/* checksum */
#define _TP_PT_VERSION		0xc4	/* version number */
#define _TP_PT_PROTECTION	0xc5	/* protection */
#define _TP_PT_ADDOPTIONS	0xc6	/* additional options */
#define _TP_PT_ALTCLASS		0xc7	/* alternative protocol classes */

#define _TP_PT_ACKTIME		0x85	/* acknowledgement time */
#define _TP_PT_RESERRORRATE	0x86	/* residual error rate */
#define _TP_PT_PRIORITY		0x87	/* priority */
#define _TP_PT_TRANSDEL		0x88	/* transit delay */
#define _TP_PT_THROUGHPUT	0x89	/* throughput */
#define _TP_PT_SUBSEQUENCE	0x8a	/* subsequence number */
#define _TP_PT_REASTIME		0x8b	/* reassignment time */
#define _TP_PT_FLOWCTLCF	0x8c	/* flow control confirmation */
#define _TP_PT_SELECTACK	0x8f	/* selective acknowledgement */

#define _TP_PT_ED_TPDU_NR	0x90	/* ED-TPDU-NR */

#define _TP_PT_DIAGNOSTIC	0xe0	/* diagnostic */

#define _TP_PT_PREF_TPDU_SIZE	0xf0	/* preferred TPDU size */
#define _TP_PT_INACTTIME	0xf2	/* inactivity time */

/**
 * tp_add_checksum - complete the checksum
 * @mp: message containing TPDU
 *
 * Add a checksum parameter to the variable part and calculate a checksum for the resulting TPDU.
 * This calculation follows the procedures in X.224 Annex D and X.234 Annex C.
 */
STATIC INLINE __hot void
tp_add_checksum(mblk_t *mp)
{
	int n;
	mblk_t *bp;
	int c0 = 0, c1 = 0, L = 0;

	mp->b_rptr[0] += 4;	/* add 4 to length indicator */
	*mp->b_wptr++ = _TP_PT_CHECKSUM;
	*mp->b_wptr++ = 2;
	n = mp->b_wptr - mp->b_rptr;
	/* zero checksum to begin with */
	*mp->b_wptr++ = 0;
	*mp->b_wptr++ = 0;

	for (bp = mp; bp; bp = bp->b_cont) {
		register unsigned char *p = bp->b_rptr;
		register unsigned char *e = bp->b_wptr;

		if (p < e) {
			L += e - p;
			while (p < e) {
				c0 += *p++;
				c1 += c0;
			}
		}
	}

	mp->b_rptr[n] = -c1 + (L - n) * c0;
	mp->b_rptr[n + 1] = c1 - (L - n + 1) * c0;
	return;
}

/**
 * tp_pack_cr - create a Connection Request (CR) TPDU
 * @tp: sending transport endpoint
 * @q: invoking queue (typically write queue)
 * @dp: user data to include in connect request
 *
 * CDT
 * DST-REF: (set to zero)
 * SRC-REF: (set to assigned source reference)
 * CLASS and OPTIONS:
 * Calling Transport Selector:
 * Called Transport Selector:
 * TPDU size (proposed)
 * preferred maximum TPDU size (proposed)
 * version number
 * protection parameter
 * checksum
 * additional options depending on class
 * alternative protocol class(es)
 * acknowledgement time
 * inactivity time
 * throughput (proposed)
 * residual error rate (proposed)
 * priority (proposed)
 * transit delay (proposed)
 * reassignment time
 * user data
 *
 * A transport connection is established by means of one transport entity (the
 * initiator) transmitting a CR-TPDU to the other transport entity (the
 * responder), which replies with a CC-TPDU.
 *
 * Before sneding the CR-TPDU, the intiator assigns the transport connection
 * being created to one (or more if the splitting procedure is being used)
 * network connection(s).  It is this set of network connections over which the
 * TPDUs are sent.
 *
 *   NOTE 1 - Even if the initiator assigns the transport connection to more
 *   than one network connection, all CR-TPDUs (if repeated) or DR-TPDUs with
 *   DST-REF set to zero which are sent prior to the receipt of the CC-TPDU
 *   shall be sent on the same network connection, unless an N-DISCONNECT
 *   indication is received.  (This is necessary because the remote entity may
 *   not support class 4 and therefore may not recognize splitting.)  If the
 *   initiator has made other assignments, it will use them only after receive
 *   of a class 4 CC-TPDU (see also the splitting procedure 6.23).
 *
 * During this exchange, all information and parameters need for the transport
 * entities to operate shall be exchanged or negotiated.
 *
 *   NOTE 2 - Except in class 4, it is recommended that the initiator start an
 *   optional timer TS1 at the time the CR-TPDU is sent.  This timer should be
 *   stopped when the connection is considered as accepted or refused or
 *   unsuccessful.  If the timer expires, the intiator should reset or
 *   disconnect the network connection, and in classes 1 and 3, freeze the
 *   reference (see 6.18).  For all other transport connection(s) multiplexed
 *   on the same network connection, the procedures for reset or disconnect as
 *   appropriate should be followed.
 *
 * ...
 *
 * The following information is exchanged:
 *
 * a) References - Each transport entity chooses a reference to be used by the
 *    peer entity which is 16-bits long and which is arbitrary under the
 *    following restrictions:
 *
 *    1) it shall not already be in use nor frozen (see 6.18);
 *    2) it shall not be zero.
 *
 *    This mechanism is symmetrical and provide identification of the transport
 *    connection independent of the network connection.  The range of
 *    references used for transport connections, in a given transport entity,
 *    is a local matter.
 *
 * b) Calling, Called and Responding Transport-Selectors (optional) - When
 *    either network address unambiguously defines the transport address, this
 *    information may be omitted.
 *
 * c) Initial credit - Only relevant for classes which include the explicit
 *    flow control function.
 *
 * d) User data - Not available if class 0 is the preferred class (see Note 3).
 *    Up to 32 octets in other classes.
 *
 *    NOTE 3 - If class 0 is a valid response according to Table 3, inclusing
 *    of user data in the CR-TPDU may cause the responding entity to refuse the
 *    connection (for example, if it only supports class 0).
 *
 * e) Acknowledgement time - Only in class 4.
 *
 * f) Checksum paraemter - Only in class 4.
 *
 * g) Protection parameter - This parameter and its semantics are user defined.
 *
 * h) Inactivity time - Only in class 4.  The inactivity time parameter shall
 *    not be included in a CC-TPDU if it was not present in the corresponding
 *    CR-TPDU.
 *
 * The following negotiations take place:
 *
 * i) The initiator shall propose a preferred class and may propose any number
 *    of alternative classes which permit a valid response as defined in Table
 *    3.  The initiator should assume when it sends the CR-TPDU that its
 *    preferred class will be agreed to, and commence the procedures associated
 *    with that class, except that if class 0 or class 1 is an alternative
 *    class, multiplexing shall not commence until a CC-TPDU selecting the use
 *    of classes 2, 3 or 4 has been received.
 *
 *    NOTE 4 - This means, for example, that when the preferred class includes
 *    resynchronization (see 6.14) the resynchronization will occur if a reset
 *    is signalled during connection establishment.
 *
 *    The responder shall select one class defined in Table 3 as a valid
 *    response corresponding to the preferred class and to the class(es), if
 *    any, contained in the alternative class parameter of the CR-TPDU.  It
 *    shall indicate the selected class in the CC-TPDU and shall follow the
 *    procedures for the selected class.
 *
 *    If the preferred class is not selected, then on receipt of the CC-TPDU,
 *    the initiator shall adjust its operation according to the procedures of
 *    the selected class.
 *
 *    NOTE 5 - The valid responses indicated in Table 3 result from both
 *    explicit negotiation, whereby each of the classes proposed is a valid
 *    response, and implement negotiation whereby:
 *    - if class 3 or 4 is proposed, then class 2 is a valid response.
 *    - if class 1 is proposed, then class 0 is a valid response.
 *
 *    NOTE 6 - Negotiation from class 2 to class 1 and from any class to a
 *    higher-numbered class is not valid.
 *
 *    NOTE 7 - Redundant combinations are not a protocol error.
 *
 * j) TPDU size - The initiator may propose a maimum size for TPDUs, and the
 *    responder may accept this value or respond with any value between 128 and
 *    the proposed value in the set of values available [see 13.3.4 b)].
 *
 *    NOTE 8 - The length of the CR-TPDU does not exceed 128 octets (see 13.3).
 *    NOTE 9 - The transport entities may have knowledge, by some local means,
 *    of the maximum available NSDU size.
 *
 * k) Preferred maximum TPDU size - The value of this parameter, multiplied by
 *    128, yields the proposed or accepted maximum TPDU size in octets.  The
 *    initiator may propose a preferred maximum size of TPDUs and the responder
 *    may accept this value or repsond with a smaller value.
 *
 *    NOTE 10 - If this parameter is used in a CR-TPDU without also including
 *    the TPDU size parameter, this will result in a maximum TPDU size of 128
 *    octets being selected if the remote entity does not recognize the
 *    preferred TPDU size parameter.  Therefore, it is reocmmended that both
 *    parameters be included in the CR-TPDU.
 *
 *    If the preferred maximum TPDU size parameter is present in a CR-TPDU the
 *    responder shall either:
 *
 *    - ignore the preferred maximum TPDU size parameter and follow TPDU size
 *      negotiation as defined in 6.5.4 j); or
 *
 *    - use the preferrend maximum TPDU size parameter to determine the maximum
 *      TPDU size requested by the initiator and ignore the TPDU size
 *      parameter.  In this case the responder shall use the preferred maximum
 *      TPDU size parameter in the CC-TPDU and shall not include the TPDU size
 *      parameter in the CC-TPDU.
 *
 *    If the preferred maximum TPDU size parameter is not present in the
 *    CR-TPDU it shall not be included in the corresponding CC-TPDU.  In this
 *    case TPDU size negotiation is as defined in 6.5.4 j).
 *
 * l) Normal or extended format - Either normal or extended format is
 *    available.  When extended is used, this applies to CDT, TPDU-NR,
 *    ED-TPDU-NR, YR-TU-NR and YR-EDTU-NR parameters.
 *
 * m) Checksum selection - This defines whether or not TPDUs of the connection
 *    are to include a checksum.
 *
 * n) Quality of service parameters - This defines the throughput, transit
 *    delay, priority and residual error rate.
 *
 *    NOTE 11 - The transport service defines transit delay as requiring a
 *    previously stated average TSDU size as a basis for any specification.
 *    This protocol as speciied in 13.3.4 m), uses a value at 128 octets.
 *    Conversion to and from specifications based upon some other value is a
 *    local matter.
 *
 * o) The non-use of explicit flow control in class 2.
 *
 * p) The use of network receipt confirmation and network expedited when class
 *    1 is to be used.
 *
 * q) Use of expedited data transfer service - This allows both TS-users to
 *    negotiate the use or non-use of the expedite data transport service as
 *    defined in the transport service (see ITU-T Rec. X.214 | ISO/IEC 8072).
 *
 * r) The use of selective acknowledgement - This allows the transport entities
 *    to decide whether to use procedures that allow acknowledgement of
 *    DT-TPDUs that are received out-of-sequence (only in class 4).
 *
 * s) The use of request acknowledgement - This allows both transport entities
 *    to negotiate the use or non-use of the request acknowledgement facility
 *    specified in 6.13.4.2 (only in classes 1, 3, 4).
 *
 * t) The use of non-blocking expedite data transfer service - This allows both
 *    transport entities to negotiate the use or non-use of the non-blocking
 *    expedited data transfer service (only in class 4).  This option will only
 *    be valid when the option of "use of expedited data transfer service" is
 *    negotiated.
 *
 * The following information is sent only in the CR-TPDU:
 *
 * u) Version number - This defines the version of the transport protocol
 *    standard used for this connection.
 *
 * v) Reassignment time parameter - This indicates the time for which the
 *    intiiator will persist in following the reassignment after failure
 *    procedure.
 *     
 */
STATIC mblk_t *
tp_pack_cr(struct tp *tp, queue_t *q, mblk_t *dp)
{
	register unsigned char *p;
	mblk_t *bp;

	/* 128-32 = 96 octets gives enough room for the entire CR-TPDU */
	if (unlikely((bp = mi_allocb(q, 128, BPRI_MED)) == NULL))
		return (bp);

	bp->b_datap->db_type = M_DATA;
	p = bp->b_wptr;
	*p++ = 0;		/* fix up later */
	*p++ = _TP_MT_CR | tp->credit;
	*p++ = 0;
	*p++ = 0;
	*p++ = tp->sref >> 8;
	*p++ = tp->sref >> 0;
	*p++ = (tp->options.req.tco.tco_prefclass << 4)
	    | (tp->opts & (_T_F_CO_EXTFORM | _T_F_CO_FLOWCTRL));
	{
		*p++ = _TP_PT_CGTRANSSEL;	/* calling transport selector */
		*p++ = sizeof(tp->src.tsel);
		bcopy(tp->src.tsel, p, sizeof(tp->src.tsel));
		p += sizeof(tp->src.tsel);
	}
	{
		*p++ = _TP_PT_CDTRANSSEL;	/* called transport selector */
		*p++ = sizeof(tp->dst.tsel);
		bcopy(tp->dst.tsel, p, sizeof(tp->dst.tsel));
		p += sizeof(tp->dst.tsel);
	}
	if (tp->options.req.tco.tco_ltpdu != T_UNSPEC) {
		int order;
		t_uscalar_t size;

		size = tp->options.req.tco.tco_ltpdu;
		for (order = _T_TPDU_SIZE_8192; order >= _T_TPDU_SIZE_128; order--)
			if ((1 << order) <= size)
				break;
		if (order != _T_TPDU_SIZE_128) {
			*p++ = _TP_PT_TPDU_SIZE;	/* TPDU size */
			*p++ = 1;
			*p++ = order;
		}
		if (!(size >>= 7))
			size = 1;
		*p++ = _TP_PT_PREF_TPDU_SIZE;	/* preferred TPDU size */
		if (size >= 1 << 24) {
			*p++ = 4;
			*p++ = size >> 24;
			*p++ = size >> 16;
			*p++ = size >> 8;
			*p++ = size >> 0;
		} else if (size >= 1 << 16) {
			*p++ = 3;
			*p++ = size >> 16;
			*p++ = size >> 8;
			*p++ = size >> 0;
		} else if (size >= 1 << 8) {
			*p++ = 2;
			*p++ = size >> 8;
			*p++ = size >> 0;
		} else {
			*p++ = 1;
			*p++ = size >> 0;
		}
	}
	if (tp->options.req.tco.tco_prefclass != T_CLASS0) {
		*p++ = _TP_PT_VERSION;	/* version number */
		*p++ = 1;
		*p++ = 1;
		if (t_tst_bit(_T_BIT_TCO_PROTECTION, tp->options.req.flags)
		    && tp->options.req.tco.tco_protection != T_UNSPEC) {
			*p++ = _TP_PT_PROTECTION;	/* protection */
			*p++ = 1;
			*p++ = tp->options.req.tco.tco_protection & ~T_ABSREQ;
		}
		{
			*p++ = _TP_PT_ADDOPTIONS;	/* additional options */
			*p++ = 1;
			*p++ = tp->addopts;
		}
#if 0
		if (tp->network != N_CLNS) {
			int i, altnum = 0;

			for (i = 0; i < 4; i++)
				if ((&tp->options.req.tco.tco_altclass1)[i] != T_UNSPEC)
					altnum++;

			if (altnum) {
				*p++ = _TP_PT_ALTCLASS;	/* alternative protocol classes */
				*p++ = altnum;
				for (i = 0; i < 4; i++) {
					if ((&tp->options.req.tco.tco_altclass1)[i] != T_UNSPEC)
						*p++ = (&tp->options.req.tco.tco_altclass1)[i];
				}
			}
		}
#endif
		if (tp->options.req.tco.tco_prefclass == T_CLASS4) {
			*p++ = _TP_PT_ACKTIME;	/* acknowledgement time */
			*p++ = 2;
			*p++ = tp->options.req.tco.tco_acktime >> 8;
			*p++ = tp->options.req.tco.tco_acktime >> 0;
		}
		if (tp->flags & _TP_MAX_THROUGHPUT) {
			*p++ = _TP_PT_THROUGHPUT;	/* throughput */
			if (!(tp->flags & _TP_AVG_THROUGHPUT)) {
				*p++ = 12;
			} else {
				*p++ = 24;
			}
			*p++ = tp->options.req.tco.tco_throughput.maxthrpt.called.targetvalue >> 16;
			*p++ = tp->options.req.tco.tco_throughput.maxthrpt.called.targetvalue >> 8;
			*p++ = tp->options.req.tco.tco_throughput.maxthrpt.called.targetvalue >> 0;
			*p++ = tp->options.req.tco.tco_throughput.maxthrpt.called.minacceptvalue >> 16;
			*p++ = tp->options.req.tco.tco_throughput.maxthrpt.called.minacceptvalue >> 8;
			*p++ = tp->options.req.tco.tco_throughput.maxthrpt.called.minacceptvalue >> 0;
			*p++ = tp->options.req.tco.tco_throughput.maxthrpt.calling.targetvalue >> 16;
			*p++ = tp->options.req.tco.tco_throughput.maxthrpt.calling.targetvalue >> 8;
			*p++ = tp->options.req.tco.tco_throughput.maxthrpt.calling.targetvalue >> 0;
			*p++ = tp->options.req.tco.tco_throughput.maxthrpt.calling.minacceptvalue >> 16;
			*p++ = tp->options.req.tco.tco_throughput.maxthrpt.calling.minacceptvalue >> 8;
			*p++ = tp->options.req.tco.tco_throughput.maxthrpt.calling.minacceptvalue >> 0;
			if (tp->flags & _TP_AVG_THROUGHPUT) {
				*p++ = tp->options.req.tco.tco_throughput.avgthrpt.called.targetvalue >> 16;
				*p++ = tp->options.req.tco.tco_throughput.avgthrpt.called.targetvalue >> 8;
				*p++ = tp->options.req.tco.tco_throughput.avgthrpt.called.targetvalue >> 0;
				*p++ =
				    tp->options.req.tco.tco_throughput.avgthrpt.called.minacceptvalue >> 16;
				*p++ = tp->options.req.tco.tco_throughput.avgthrpt.called.minacceptvalue >> 8;
				*p++ = tp->options.req.tco.tco_throughput.avgthrpt.called.minacceptvalue >> 0;
				*p++ = tp->options.req.tco.tco_throughput.avgthrpt.calling.targetvalue >> 16;
				*p++ = tp->options.req.tco.tco_throughput.avgthrpt.calling.targetvalue >> 8;
				*p++ = tp->options.req.tco.tco_throughput.avgthrpt.calling.targetvalue >> 0;
				*p++ =
				    tp->options.req.tco.tco_throughput.avgthrpt.calling.minacceptvalue >> 16;
				*p++ =
				    tp->options.req.tco.tco_throughput.avgthrpt.calling.minacceptvalue >> 8;
				*p++ =
				    tp->options.req.tco.tco_throughput.avgthrpt.calling.minacceptvalue >> 0;
			}
		}
		if (tp->flags & _TP_RESIDERRRATE) {
			*p++ = _TP_PT_RESERRORRATE;	/* residual error rate */
			*p++ = 3;
			*p++ = tp->options.req.tco.tco_reserrorrate.targetvalue;
			*p++ = tp->options.req.tco.tco_reserrorrate.minacceptvalue;
			*p++ = tp->options.req.tco.tco_ltpdu;	/* FIXME */
		}
		*p++ = _TP_PT_PRIORITY;	/* priority */
		*p++ = 2;
		*p++ = tp->options.req.tco.tco_priority >> 8;
		*p++ = tp->options.req.tco.tco_priority >> 0;
		*p++ = _TP_PT_TRANSDEL;	/* transit delay */
		*p++ = 8;
		*p++ = tp->options.req.tco.tco_transdel.maxdel.called.targetvalue >> 8;
		*p++ = tp->options.req.tco.tco_transdel.maxdel.called.targetvalue >> 0;
		*p++ = tp->options.req.tco.tco_transdel.maxdel.called.minacceptvalue >> 8;
		*p++ = tp->options.req.tco.tco_transdel.maxdel.called.minacceptvalue >> 0;
		*p++ = tp->options.req.tco.tco_transdel.maxdel.calling.targetvalue >> 8;
		*p++ = tp->options.req.tco.tco_transdel.maxdel.calling.targetvalue >> 0;
		*p++ = tp->options.req.tco.tco_transdel.maxdel.calling.minacceptvalue >> 8;
		*p++ = tp->options.req.tco.tco_transdel.maxdel.calling.minacceptvalue >> 0;
		if (tp->options.req.tco.tco_prefclass >= T_CLASS3) {
			*p++ = _TP_PT_REASTIME;	/* reassignment time */
			*p++ = 2;
			*p++ = tp->options.req.tco.tco_reastime >> 8;
			*p++ = tp->options.req.tco.tco_reastime >> 0;
		}
		if (tp->options.req.tco.tco_prefclass == T_CLASS4) {
			*p++ = _TP_PT_INACTTIME;	/* inactivity timer */
			*p++ = 4;
			*p++ = tp->inactivity >> 24;
			*p++ = tp->inactivity >> 16;
			*p++ = tp->inactivity >> 8;
			*p++ = tp->inactivity >> 0;
		}
	}
	bp->b_rptr[0] = p - bp->b_rptr - 1;	/* set length indicator */
	bp->b_wptr = p;

	bp->b_cont = dp;
	tp_add_checksum(bp);

	return (bp);
}

/**
 * tp_pack_cc - create a Connection Confirmation (CC) TPDU
 * @tp: sending transport endpoint
 * @q: invoking queue
 * @dp: data to include in response
 *
 * CDT
 * DST-REF: (SRC-REF received in CR)
 * SRC-REF: (set to assigned source reference)
 * CLASS and OPTIONS (selected)
 * Calling Transport Selector;
 * Responding Transport Selector;
 * TPDU size (selected)
 * the preferred maximum TPDU size (selected)
 * protection parameter
 * checksum
 * additional option selection (selected)
 * acknowledgement time
 * inactivity time
 * throughput (selected)
 * residual error rate (selected)
 * priority (selected)
 * transit delay (selected)
 * user data
 */
STATIC mblk_t *
tp_pack_cc(struct tp *tp, queue_t *q, mblk_t *dp)
{
	register unsigned char *p;
	mblk_t *bp;

	/* 128-32 = 96 octets gives enough room for the entire CR-TPDU */
	if ((bp = mi_allocb(q, 96, BPRI_MED)) == NULL)
		goto enobufs;
	p = bp->b_wptr;
	*p++ = 0;		/* fix up later */
	*p++ = _TP_MT_CC | tp->credit;
	*p++ = tp->dref >> 8;
	*p++ = tp->dref >> 0;
	*p++ = tp->sref >> 8;
	*p++ = tp->sref >> 0;
	*p++ = (tp->options.res.tco.tco_prefclass << 4)
	    | (tp->opts & (_T_F_CO_EXTFORM | _T_F_CO_FLOWCTRL));
	if (tp->src.tsel[0] > 0) {
		*p++ = _TP_PT_CGTRANSSEL;	/* calling transport selector */
		*p++ = 2;
		bcopy(tp->src.tsel, p, 2);
		p += 2;
	}
	if (tp->dst.tsel[0] > 0) {
		*p++ = _TP_PT_CDTRANSSEL;	/* called transport selector */
		*p++ = 2;
		bcopy(tp->dst.tsel, p, 2);
		p += 2;
	}
	if (tp->options.res.tco.tco_ltpdu != T_UNSPEC) {
		int order;
		t_uscalar_t size;

		size = tp->options.res.tco.tco_ltpdu;
		for (order = _T_TPDU_SIZE_8192; order >= _T_TPDU_SIZE_128; order--)
			if ((1 << order) <= size)
				break;
		if (order != _T_TPDU_SIZE_128) {
			*p++ = _TP_PT_TPDU_SIZE;	/* TPDU size */
			*p++ = 1;
			*p++ = order;
		}
		if (!(size >>= 7))
			size = 1;
		*p++ = _TP_PT_PREF_TPDU_SIZE;	/* preferred TPDU size */
		if (size >= 1 << 24) {
			*p++ = 4;
			*p++ = size >> 24;
			*p++ = size >> 16;
			*p++ = size >> 8;
			*p++ = size >> 0;
		} else if (size >= 1 << 16) {
			*p++ = 3;
			*p++ = size >> 16;
			*p++ = size >> 8;
			*p++ = size >> 0;
		} else if (size >= 1 << 8) {
			*p++ = 2;
			*p++ = size >> 8;
			*p++ = size >> 0;
		} else {
			*p++ = 1;
			*p++ = size >> 0;
		}
	}
	if (tp->options.res.tco.tco_prefclass != T_CLASS0) {
#if 0
		*p++ = _TP_PT_VERSION;	/* version number */
		*p++ = 1;
		*p++ = 1;
#endif
		if (tp->options.res.tco.tco_protection) {
			*p++ = _TP_PT_PROTECTION;	/* protection */
			*p++ = 1;
			*p++ = tp->options.res.tco.tco_protection;
		}
		*p++ = _TP_PT_ADDOPTIONS;	/* additional options */
		*p++ = 1;
		*p++ = tp->addopts;
#if 0
		if (tp->network != N_CLNS) {
			int i, altnum = 0;

			for (i = 0; i < 4; i++)
				if ((&tp->options.res.tco.tco_altclass1)[i] != T_UNSPEC)
					altnum++;

			if (altnum) {
				*p++ = _TP_PT_ALTCLASS;	/* alternative protocol classes */
				*p++ = altnum;
				for (i = 0; i < 4; i++) {
					if ((&tp->options.res.tco.tco_altclass1)[i] != T_UNSPEC)
						*p++ = (&tp->options.res.tco.tco_altclass1)[i];
				}
			}
		}
#endif
		if (tp->options.res.tco.tco_prefclass == T_CLASS4) {
			*p++ = _TP_PT_ACKTIME;	/* acknowledgement time */
			*p++ = 2;
			*p++ = tp->options.res.tco.tco_acktime >> 8;
			*p++ = tp->options.res.tco.tco_acktime >> 0;
		}
		if (tp->flags & _TP_MAX_THROUGHPUT) {
			*p++ = _TP_PT_THROUGHPUT;	/* throughput */
			if (!(tp->flags & _TP_AVG_THROUGHPUT)) {
				*p++ = 12;
			} else {
				*p++ = 24;
			}
			*p++ = tp->options.res.tco.tco_throughput.maxthrpt.called.targetvalue >> 16;
			*p++ = tp->options.res.tco.tco_throughput.maxthrpt.called.targetvalue >> 8;
			*p++ = tp->options.res.tco.tco_throughput.maxthrpt.called.targetvalue >> 0;
			*p++ = tp->options.res.tco.tco_throughput.maxthrpt.called.minacceptvalue >> 16;
			*p++ = tp->options.res.tco.tco_throughput.maxthrpt.called.minacceptvalue >> 8;
			*p++ = tp->options.res.tco.tco_throughput.maxthrpt.called.minacceptvalue >> 0;
			*p++ = tp->options.res.tco.tco_throughput.maxthrpt.calling.targetvalue >> 16;
			*p++ = tp->options.res.tco.tco_throughput.maxthrpt.calling.targetvalue >> 8;
			*p++ = tp->options.res.tco.tco_throughput.maxthrpt.calling.targetvalue >> 0;
			*p++ = tp->options.res.tco.tco_throughput.maxthrpt.calling.minacceptvalue >> 16;
			*p++ = tp->options.res.tco.tco_throughput.maxthrpt.calling.minacceptvalue >> 8;
			*p++ = tp->options.res.tco.tco_throughput.maxthrpt.calling.minacceptvalue >> 0;
			if (tp->flags & _TP_AVG_THROUGHPUT) {
				*p++ = tp->options.res.tco.tco_throughput.avgthrpt.called.targetvalue >> 16;
				*p++ = tp->options.res.tco.tco_throughput.avgthrpt.called.targetvalue >> 8;
				*p++ = tp->options.res.tco.tco_throughput.avgthrpt.called.targetvalue >> 0;
				*p++ =
				    tp->options.res.tco.tco_throughput.avgthrpt.called.minacceptvalue >> 16;
				*p++ = tp->options.res.tco.tco_throughput.avgthrpt.called.minacceptvalue >> 8;
				*p++ = tp->options.res.tco.tco_throughput.avgthrpt.called.minacceptvalue >> 0;
				*p++ = tp->options.res.tco.tco_throughput.avgthrpt.calling.targetvalue >> 16;
				*p++ = tp->options.res.tco.tco_throughput.avgthrpt.calling.targetvalue >> 8;
				*p++ = tp->options.res.tco.tco_throughput.avgthrpt.calling.targetvalue >> 0;
				*p++ =
				    tp->options.res.tco.tco_throughput.avgthrpt.calling.minacceptvalue >> 16;
				*p++ =
				    tp->options.res.tco.tco_throughput.avgthrpt.calling.minacceptvalue >> 8;
				*p++ =
				    tp->options.res.tco.tco_throughput.avgthrpt.calling.minacceptvalue >> 0;
			}
		}
		if (tp->flags & _TP_RESIDERRRATE) {
			*p++ = _TP_PT_RESERRORRATE;	/* residual error rate */
			*p++ = 3;
			*p++ = tp->options.res.tco.tco_reserrorrate.targetvalue;
			*p++ = tp->options.res.tco.tco_reserrorrate.minacceptvalue;
			*p++ = tp->options.res.tco.tco_ltpdu;	/* FIXME */
		}
		*p++ = _TP_PT_PRIORITY;	/* priority */
		*p++ = 2;
		*p++ = tp->options.res.tco.tco_priority >> 8;
		*p++ = tp->options.res.tco.tco_priority >> 0;
		*p++ = _TP_PT_TRANSDEL;	/* transit delay */
		*p++ = 8;
		*p++ = tp->options.res.tco.tco_transdel.maxdel.called.targetvalue >> 8;
		*p++ = tp->options.res.tco.tco_transdel.maxdel.called.targetvalue >> 0;
		*p++ = tp->options.res.tco.tco_transdel.maxdel.called.minacceptvalue >> 8;
		*p++ = tp->options.res.tco.tco_transdel.maxdel.called.minacceptvalue >> 0;
		*p++ = tp->options.res.tco.tco_transdel.maxdel.calling.targetvalue >> 8;
		*p++ = tp->options.res.tco.tco_transdel.maxdel.calling.targetvalue >> 0;
		*p++ = tp->options.res.tco.tco_transdel.maxdel.calling.minacceptvalue >> 8;
		*p++ = tp->options.res.tco.tco_transdel.maxdel.calling.minacceptvalue >> 0;
#if 0
		if (tp->options.res.tco.tco_prefclass >= T_CLASS3) {
			*p++ = _TP_PT_REASTIME;	/* reassignment time */
			*p++ = 2;
			*p++ = tp->options.res.tco.tco_reastime >> 8;
			*p++ = tp->options.res.tco.tco_reastime >> 0;
		}
#endif
		if (tp->options.res.tco.tco_prefclass == T_CLASS4) {
			*p = _TP_PT_INACTTIME;	/* inactivity timer */
			*p++ = 4;
			*p++ = tp->inactivity >> 24;
			*p++ = tp->inactivity >> 16;
			*p++ = tp->inactivity >> 8;
			*p++ = tp->inactivity >> 0;
		}
	}
	bp->b_rptr[0] = p - bp->b_rptr - 1;	/* set length indicator */
	bp->b_wptr = p;

#if 0
	/* FIXME: caller must do this */
	bp->b_cont = dp;
	if (tp->options.res.tco.tco_checksum == T_YES)
		tp_add_checksum(bp);
#endif
      enobufs:
	return (bp);
}

/**
 * tp_pack_dr - Send a Disconnect Request (DR) Message
 * @tp: sending transport endpoint
 * @q: invoking queue
 * @reason: reason for the disconnect
 * @dp: diagnostic message block (disconnect data)
 *
 * Disconnect reasons:
 * 128 + 0:	Normal disconnect initiated by session entity
 * 128 + 1:	Remote transport entity congestion at connect request time
 * 128 + 2:*	Connection negotiation failed [i.e. proposed class(es) not supported].
 * 128 + 3:	Duplicate source reference detected for same pair of NSAPs.
 * 128 + 4:	Mismatched references.
 * 128 + 5:	Protocol error.
 * 128 + 6:	Not used.
 * 128 + 7:	Reference overflow.
 * 128 + 8:	Connection request refused on this network connection.
 * 128 + 9:	Not used.
 * 128 + 10:	Header or parameter length invalid.
 * 0:		Reason not specified.
 * 1:		Congestion at TSAP.
 * 2:*		Session entity not attached to TSAP.
 * 3:*		Address unknown.
 */
STATIC mblk_t *
tp_pack_dr(struct tp *tp, queue_t *q, t_uscalar_t reason, mblk_t *dp)
{
	register unsigned char *p;
	mblk_t *bp;

	struct netbuf diag = {
		.len = dp ? msgsize(dp) : 0,
		.buf = dp ? dp->b_rptr : NULL,
	};

	/* NOTE: the entire size of the DR-TPDU must be within the agree maximum TPDU size or when used in
	   response to a connection request, 128.  Note that the DDATA_size member of the T_info_ack
	   structure should be set accordingly.  The DDATA_size may depend upon the setting of the
	   tco_checksum option, because the checksum consumes an additional 4 octets in the DR-TPDU when
	   used. */
	if (unlikely((bp = mi_allocb(q, 13 + diag.len, BPRI_MED)) == NULL))
		goto enobufs;

	p = bp->b_wptr;
	*p++ = 0;		/* fix up later */
	*p++ = _TP_MT_DR;	/* disconnect request */
	*p++ = tp->dref >> 8;	/* destination reference */
	*p++ = tp->dref >> 0;
	*p++ = tp->sref >> 8;	/* source reference */
	*p++ = tp->sref >> 0;
	*p++ = reason;
	if (diag.len > 0 && diag.buf != NULL) {
		*p++ = _TP_PT_DIAGNOSTIC;	/* diagnostic */
		*p++ = diag.len;
		bcopy(diag.buf, p, diag.len);
		p += diag.len;
	}
	*bp->b_rptr = p - bp->b_rptr - 1;	/* set length indicator */
	bp->b_wptr = p;

#if 0
	/* FIXME: caller must do this */
	bp->b_cont = dp;
	if (tp->options.res.tco.tco_checksum == T_YES)
		tp_add_checksum(bp);
#endif
      enobufs:
	return (bp);

}

static int
tp_send_dr(struct tp *tp, queue_t *q, mblk_t *mp)
{
	/* FIXME FIXME FIXME */
	return (-ENOBUFS);
}

/**
 * tp_pack_dc - Send a Disconnect Confirmation (DC) Message
 * @tp: sending transport endpoint
 * @q: invoking queue
 * @dref: destination reference
 * @sref: source reference
 */
STATIC mblk_t *
tp_pack_dc(struct tp *tp, queue_t *q, uint16_t dref, uint16_t sref)
{
	register unsigned char *p;
	mblk_t *bp;

	if (unlikely((bp = mi_allocb(q, 10, BPRI_MED)) == NULL))
		goto enobufs;

	p = bp->b_wptr;
	*p++ = 0;		/* fix up later */
	*p++ = _TP_MT_DC;	/* disconnect confirm */
	*p++ = dref >> 8;	/* destination reference */
	*p++ = dref >> 0;
	*p++ = sref >> 8;	/* source reference */
	*p++ = sref >> 0;
	bp->b_rptr[0] = p - bp->b_rptr - 1;	/* set length indicator */
	bp->b_wptr = p;

#if 0
	/* FIXME: caller must do this */
	if (tp->options.res.tco.tco_checksum == T_YES)
		tp_add_checksum(bp);
#endif
      enobufs:
	return (bp);
}

/**
 * tp_pack_dt - Send a Data (DT) Message
 * @tp: sending transport endpoint
 * @q: invoking queue
 * @roa: receipt acknowlegement option
 * @eot: end of transmission
 * @nr: sequence number
 * @ednr: expedited data sequence number
 */
STATIC mblk_t *
tp_pack_dt(struct tp *tp, queue_t *q, uint8_t roa, uint8_t eot, uint32_t nr, int ednr, mblk_t *dp)
{
	register unsigned char *p;
	mblk_t *bp;

	if (unlikely((bp = mi_allocb(q, 1, BPRI_MED)) == NULL))	/* FIXME */
		goto enobufs;

	p = bp->b_wptr;
	*p++ = 0;		/* fix up later */
	*p++ = _TP_MT_DT | roa;	/* data */
	switch (tp->options.res.tco.tco_prefclass) {
	case T_CLASS0:
	case T_CLASS1:
		*p++ = (eot ? 0x80 : 0x00) | (nr & 0x7f);
		break;
	case T_CLASS2:
	case T_CLASS3:
	case T_CLASS4:
		*p++ = tp->dref >> 8;
		*p++ = tp->dref >> 0;
		if (!(tp->opts & _T_F_CO_EXTFORM)) {
			*p++ = (eot ? 0x80 : 0x00) | (nr & 0x7f);
		} else {
			*p++ = (eot ? 0x80 : 0x00) | ((nr >> 24) & 0x7f);
			*p++ = nr >> 16;
			*p++ = nr >> 8;
			*p++ = nr >> 0;
		}
		if ((tp->addopts & _T_F_AO_NBLKEXPDATA) && ednr != -1) {
			*p++ = _TP_PT_ED_TPDU_NR;	/* ED-TPDU-NR */
			if (!(tp->opts & _T_F_CO_EXTFORM)) {
				*p++ = 2;
				*p++ = ednr >> 8;
				*p++ = ednr >> 0;
			} else {
				*p++ = 4;
				*p++ = ednr >> 24;
				*p++ = ednr >> 16;
				*p++ = ednr >> 8;
				*p++ = ednr >> 0;
			}
		}
		break;
	}
	bp->b_rptr[0] = p - bp->b_rptr - 1;	/* set length indicator */
	bp->b_wptr = p;

#if 0
	/* FIXME: caller must do this */
	bp->b_cont = dp;
	if (tp->options.res.tco.tco_checksum == T_YES)
		tp_add_checksum(bp);
#endif
      enobufs:
	return (bp);
}

static int
tp_send_dt(struct tp *tp, queue_t *q, mblk_t *mp, int more)
{
	mblk_t *dp = NULL;
	uint8_t roa = 0;		/* FIXME */
	uint8_t nr = 0;			/* FIXME */
	int ednr = 0;			/* FIXME */
	uint8_t eot = more ? 0 : 1;

	if ((dp = tp_pack_dt(tp, q, roa, eot, nr, ednr, mp)) == NULL)
		goto enobufs;
	/* FIXME send of the packet */
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * tp_pack_ed - Send an Expedited Data (ED) Message
 * @tp: sending transport endpoint
 * @q: invoking queue
 * @nr: sequence number
 */
STATIC mblk_t *
tp_pack_ed(struct tp *tp, queue_t *q, uint32_t nr, mblk_t *dp)
{
	register unsigned char *p;
	mblk_t *bp;

	if (unlikely((bp = mi_allocb(q, 0, BPRI_MED)) == NULL))	/* FIXME */
		goto enobufs;

	// bp->b_datap->db_type = M_DATA; /* redundant */
	p = bp->b_wptr;
	*p++ = 0;		/* fix up later */
	*p++ = _TP_MT_ED;	/* expedited data */
	*p++ = tp->dref >> 8;
	*p++ = tp->dref >> 0;
	assure(tp->options.res.tco.tco_prefclass != T_CLASS0);
	if (!(tp->opts & _T_F_CO_EXTFORM)) {
		*p++ = 0x80 | (nr & 0x7f);
	} else {
		*p++ = 0x80 | ((nr >> 24) & 0x7f);
		*p++ = nr >> 16;
		*p++ = nr >> 8;
		*p++ = nr >> 0;
	}
	bp->b_rptr[0] = p - bp->b_rptr - 1;	/* set length indicator */
	bp->b_wptr = p;

#if 0
	/* FIXME: caller must do this */
	bp->b_cont = dp;
	if (tp->options.res.tco.tco_checksum == T_YES)
		tp_add_checksum(bp);
#endif
      enobufs:
	return (bp);
}

static int
tp_send_ed(struct tp *tp, queue_t *q, mblk_t *mp, int more)
{
	mblk_t *dp = NULL;
	uint8_t nr = 0;			/* FIXME */

	if ((dp = tp_pack_ed(tp, q, nr, mp)) == NULL)
		goto enobufs;
	/* FIXME send of the packet */
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * tp_pack_ak - Send an Acknowledgement (AK) Message
 * @tp: sending transport endpoint
 * @q: invoking queue
 * @dref: destination reference
 * @credit:
 * @nr: sequence number
 * @ssn:
 */
STATIC mblk_t *
tp_pack_ak(struct tp *tp, queue_t *q, mblk_t *dp, unsigned short dref, uint16_t credit, uint32_t nr, int ssn)
{
	register unsigned char *p;
	mblk_t *bp;
	int mlen = 5;

	if (tp->options.res.tco.tco_extform == T_YES)
		mlen += 5;
	if (tp->options.res.tco.tco_prefclass == T_CLASS4) {
		if (ssn != 0)
			mlen += 4;
		if (tp->options.res.tco.tco_flowctrl == T_YES)
			mlen += 12;
		if (tp->options.res.tco.tco_selectack == T_YES) {
			if (tp->options.res.tco.tco_extform == T_YES)
				mlen += 1 + (tp->sackblks << 3);
			else
				mlen += 1 + (tp->sackblks << 1);
		}
	}

	if ((bp = mi_allocb(q, mlen, BPRI_MED)) == NULL)
		goto enobufs;

	// bp->b_datap->db_type = M_DATA; /* redundant */
	p = bp->b_wptr;
	*p++ = 0;		/* fix up later */
	if (tp->options.res.tco.tco_extform != T_YES) {
		*p++ = _TP_MT_AK | (credit & 0x0f);	/* acknowledge */
	} else {
		*p++ = _TP_MT_AK;	/* acknowledge */
	}
	*p++ = dref >> 8;
	*p++ = dref >> 0;
	if (tp->options.res.tco.tco_extform != T_YES) {
		*p++ = nr & 0x7f;
	} else {
		*p++ = (nr >> 24) & 0x7f;
		*p++ = nr >> 16;
		*p++ = nr >> 8;
		*p++ = nr >> 0;
		*p++ = credit >> 8;
		*p++ = credit >> 0;
	}
	if (tp->options.res.tco.tco_prefclass == T_CLASS4) {
		if (ssn != 0) {
			*p++ = _TP_PT_SUBSEQUENCE;	/* subsequence number */
			*p++ = 2;
			*p++ = ssn >> 8;
			*p++ = ssn >> 0;
		}
		if (tp->options.res.tco.tco_flowctrl == T_YES) {
			*p++ = _TP_PT_FLOWCTLCF;	/* flow control confirmation */
			*p++ = 8;
			if (tp->options.res.tco.tco_extform != T_YES) {
				*p++ = 0;
				*p++ = 0;
				*p++ = 0;
				*p++ = tp->rnr & 0x7f;
			} else {
				*p++ = (tp->rnr >> 24) & 0x7f;
				*p++ = tp->rnr >> 16;
				*p++ = tp->rnr >> 8;
				*p++ = tp->rnr >> 0;
			}
			*p++ = tp->rssn >> 8;
			*p++ = tp->rssn >> 0;
			if (tp->options.res.tco.tco_extform != T_YES) {
				*p++ = 0;
				*p++ = 0;
				*p++ = 0;
				*p++ = credit & 0x7f;
			} else {
				*p++ = (credit >> 24) & 0x7f;
				*p++ = credit >> 16;
				*p++ = credit >> 8;
				*p++ = credit >> 0;
			}
		}
#if 0
		if (tp->options.res.tco.tco_selectack == T_YES) {
			*p++ = _TP_PT_SELECTACK;	/* selective acknowledgement */
			if (tp->options.res.tco.tco_extform != T_YES) {
				*p++ = tp->sackblks << 1;
				while (0) {	/* FIXME */
					*p++ = blk->beg & 0x7f;
					*p++ = blk->end & 0x7f;
				}
			} else {
				*p++ = tp->sackblks << 3;
				while (0) {	/* FIXME */
					*p++ = (blk->beg >> 24) & 0x7f;
					*p++ = blk->beg >> 16;
					*p++ = blk->beg >> 8;
					*p++ = blk->beg >> 0;
					*p++ = (blk->end >> 24) & 0x7f;
					*p++ = blk->end >> 16;
					*p++ = blk->end >> 8;
					*p++ = blk->end >> 0;
				}
			}
		}
#endif
	}
	bp->b_rptr[0] = p - bp->b_rptr - 1;	/* set length indicator */
	bp->b_wptr = p;

#if 0
	/* FIXME: caller must do this */
	bp->b_cont = dp;
	if (tp->options.res.tco.tco_checksum == T_YES)
		tp_add_checksum(bp);
#endif
      enobufs:
	return (bp);
}

/**
 * tp_pack_ea - Send an Expedited Data Acknowledgement (EA) Message
 * @tp: sending transport endpoint
 * @q: invoking queue
 * @dref: destination reference
 * @nr: sequence number
 * @ssn:
 */
STATIC mblk_t *
tp_pack_ea(struct tp *tp, queue_t *q, unsigned short dref, uint32_t nr, int ssn)
{
	register unsigned char *p;
	mblk_t *bp;
	int mlen = 5;

	if (tp->options.res.tco.tco_extform == T_YES)
		mlen += 3;

	if (unlikely((bp = mi_allocb(q, mlen, BPRI_MED)) == NULL))
		goto enobufs;

	p = bp->b_wptr;
	*p++ = 0;		/* fix up later */
	*p++ = _TP_MT_EA;	/* expedited data acknowledge */
	*p++ = dref >> 8;
	*p++ = dref >> 0;
	if (tp->options.res.tco.tco_extform != T_YES) {
		*p++ = nr & 0x7f;
	} else {
		*p++ = (nr >> 24) & 0x7f;
		*p++ = nr >> 16;
		*p++ = nr >> 8;
		*p++ = nr >> 0;
	}
	bp->b_rptr[0] = p - bp->b_rptr - 1;	/* set length indicator */
	bp->b_wptr = p;

#if 0
	/* FIXME: caller must do this */
	bp->b_cont = NULL;
	if (tp->options.res.tco.tco_checksum == T_YES)
		tp_add_checksum(bp);
#endif
      enobufs:
	return (bp);
}

static int
tp_normal(struct tp *tp)
{
	return (1);
}

/**
 * tp_pack_rj - Send a Reject (RJ) Message
 * @tp: sending transport endpoint
 * @q: invoking queue
 * @nr: sequence number
 * @ssn:
 */
STATIC mblk_t *
tp_pack_rj(struct tp *tp, queue_t *q, mblk_t *dp, uint32_t nr, int ssn)
{
	register unsigned char *p;
	mblk_t *bp;
	int mlen = 5;

	if (tp->options.res.tco.tco_extform == T_YES)
		mlen += 5;

	if (unlikely((bp = mi_allocb(q, mlen, BPRI_MED)) == NULL))
		goto enobufs;

	p = bp->b_wptr;
	*p++ = 0;		/* fix up later */
	if (tp_normal(tp)) {
		*p++ = _TP_MT_RJ | (tp->credit & 0x0f);	/* reject */
	} else {
		*p++ = _TP_MT_RJ;	/* reject */
	}
	*p++ = tp->dref >> 8;
	*p++ = tp->dref >> 0;
	if (tp_normal(tp)) {
		*p++ = nr & 0x7f;
	} else {
		*p++ = (nr >> 24) & 0x7f;
		*p++ = nr >> 16;
		*p++ = nr >> 8;
		*p++ = nr >> 0;
		*p++ = tp->credit >> 8;
		*p++ = tp->credit >> 0;
	}
	bp->b_rptr[0] = p - bp->b_rptr - 1;	/* set length indicator */
	bp->b_wptr = p;

#if 0
	/* FIXME: caller must do this */
	bp->b_cont = dp;
	if (tp->options.res.tco.tco_checksum == T_YES)
		tp_add_checksum(bp);
#endif
      enobufs:
	return (bp);
}

/**
 * tp_pack_er - Create an Error (ER) TPDU
 * @tp: sending transport endpoint
 * @q: invoking queue
 * @cause: reject cause
 */
STATIC mblk_t *
tp_pack_er(struct tp *tp, queue_t *q, mblk_t *dp, uint32_t cause, struct netbuf *tpdu)
{
	register unsigned char *p;
	mblk_t *bp;
	int mlen = 5;

	if (tpdu->len > 0 && tpdu->buf != NULL)
		mlen += 2 + tpdu->len;

	if (unlikely((bp = mi_allocb(q, mlen, BPRI_MED)) == NULL))
		goto enobufs;

	p = bp->b_wptr;
	*p++ = 0;		/* fix up later */
	*p++ = _TP_MT_ER;	/* error */
	*p++ = tp->dref >> 8;
	*p++ = tp->dref >> 0;
	*p++ = cause;
	if (tpdu->len > 0 && tpdu->buf != NULL) {
		*p++ = _TP_PT_INVALID_TPDU;	/* invalid TPDU */
		*p++ = tpdu->len;
		bcopy(tpdu->buf, p, tpdu->len);
		p += tpdu->len;
	}
	*bp->b_rptr = p - bp->b_rptr - 1;	/* set length indicator */
	bp->b_wptr = p;

#if 0
	/* FIXME: caller must do this */
	bp->b_cont = dp;
	if (tp->options.res.tco.tco_checksum == T_YES)
		tp_add_checksum(bp);
#endif
      enobufs:
	return (bp);
}

/**
 * tp_pack_ud - Create a Unit Data (UD) TPDU
 * @tp: sending transport endpoint
 * @q: invoking queue
 */
STATIC INLINE fastcall __hot_put mblk_t *
tp_pack_ud(struct tp *tp, queue_t *q, mblk_t *dp)
{
	register unsigned char *p;
	mblk_t *bp;
	size_t mlen = 2;

	if (tp->dst.tsel[0] > 0)
		mlen += 2 + tp->dst.nsap.len + 2;
	if (tp->src.tsel[0] > 0)
		mlen += 2 + tp->src.nsap.len + 2;

	if (unlikely((bp = mi_allocb(q, mlen, BPRI_MED)) == NULL))
		goto enobufs;

	p = bp->b_wptr;
	*p++ = 0;		/* length indicator - fix up later */
	*p++ = _TP_MT_UD;	/* unitdata */
	if (tp->dst.tsel[0] > 0) {
		*p++ = _TP_PT_CDTRANSSEL;
		*p++ = 2;
		bcopy(tp->dst.tsel, p, 2);
		p += 2;
	}
	if (tp->src.tsel[0] > 0) {
		*p++ = _TP_PT_CGTRANSSEL;
		*p++ = 2;
		bcopy(tp->src.tsel, p, 2);
		p += 2;
	}
	*bp->b_rptr = p - bp->b_rptr - 1;	/* set length indicator */
	bp->b_wptr = p;
#if 0
	/* FIXME: caller must do this */
	bp->b_cont = dp;
	if (tp->options.res.tcl.tcl_checksum == T_YES)
		tp_add_checksum(bp);
#endif
      enobufs:
	return (bp);
}

static int
tp_send_ud(struct tp *tp, queue_t *q, mblk_t *dp, unsigned char *dst, t_uscalar_t dst_len)
{
	/* FIXME FIXME FIXME */
	return (-ENOBUFS);
}

static void
tp_queue_xmit(struct tp *tp, queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[1] == _TP_MT_UD) {
		if (tp->options.res.tcl.tcl_checksum == T_YES)
			tp_add_checksum(mp);
	} else {
		if (tp->options.res.tco.tco_checksum == T_YES)
			tp_add_checksum(mp);
	}

	/* FIXME: actually send the message now... */

	return;
}

/**
 * tp_resend - resend the previously sent message
 * @tp: private structure (locked)
 * @q: active queue
 */
static void
tp_resend(struct tp *tp, queue_t *q)
{
	tp_timer_stop(tp, t1);
	tp_timer_start(tp, q, t1);
	tp->count++;
	tp_queue_xmit(tp, q, tp->sent);
}

/**
 * tp_repeat - repeat sending of message based on retransmission timer
 * @tp: private structure (locked)
 * @q: active queue
 * @mp: message to repeat send
 */
static void
tp_repeat(struct tp *tp, queue_t *q, mblk_t *mp)
{
	freemsg(tp->sent);
	tp->sent = mp;
	tp->count = 0;
	tp_resend(tp, q);
}

/*
 *  =========================================================================
 *
 *  Protocol Actions
 *
 *  =========================================================================
 */
STATIC INLINE streams_fastcall __hot_in struct tp *
tp_lookup_conn(uint16_t dref, uint16_t sref, struct snpaaddr *daddr, struct snpaaddr *saddr)
{
	struct tp *tp;
	struct tp_chash_bucket *hp = &tp_chash[tp_chashfn(dref, sref)];

	read_lock(&hp->lock);
	for (tp = hp->list; tp; tp = tp->cnext) {
		if ((dref == 0 || tp->lref == dref) && (sref == 0 || tp->rref == sref))
			break;
	}
	/* tp_get(tp); */
	read_unlock(&hp->lock);
	return (tp);
}

STATIC INLINE streams_fastcall __hot_in struct tp *
tp_lookup_bind(uint16_t dsel, struct snpaaddr *daddr)
{
	struct tp *result = NULL;
	struct tp_bhash_bucket *hp = &tp_bhash[tp_bhashfn(dsel)];

	read_lock(&hp->lock);
	{
		struct tp *tp;
		int hiscore = 0;

		for (tp = hp->list; tp; tp = tp->bnext) {
			int score = 0;
			t_uscalar_t state;
			uint16_t ssel;
			struct snpaaddr *saddr;

			/* only listening T_COTS(_ORD) Streams and T_CLTS Streams */
			if (tp->conind == 0 && tp->p.info.SERV_type != T_CLTS)
				continue;
			/* only Streams in the (close to) correct state */
			if ((state = tp_get_state(tp)) != TS_IDLE && state != TS_WACK_UREQ)
				continue;
			ssel = *(uint16_t *) tp->src.tsel;
			if (ssel == 0)
				continue;
			if (ssel != dsel)
				continue;
			score++;
			saddr = &tp->loc;
			if (saddr->type != 0) {
				if (saddr->type != daddr->type)
					continue;
				switch (saddr->type) {
				case TP_SNPA_TYPE_MAC:
					if (memcmp(saddr->addr, daddr->addr, 6))
						continue;
					break;
				case TP_SNPA_TYPE_IP4:
					if (memcmp(saddr->addr, daddr->addr, 4))
						continue;
					break;
				case TP_SNPA_TYPE_IP4_PORT:
					if (memcmp(saddr->addr, daddr->addr, 6))
						continue;
					break;
				case TP_SNPA_TYPE_IP6:
					if (memcmp(saddr->addr, daddr->addr, 16))
						continue;
					break;
				case TP_SNPA_TYPE_IP6_PORT:
					if (memcmp(saddr->addr, daddr->addr, 18))
						continue;
					break;
				default:
					continue;
				}
				score++;
				if (score == 2) {
					/* perfect match */
					result = tp;
					break;
				}
				if (score > hiscore) {
					hiscore = score;
					result = tp;
				}
			}
		}
	}
	/* tp_get(result) */
	read_unlock(&hp->lock);
	usual(result);
	return (result);
}

STATIC INLINE streams_fastcall __hot_in struct tp *
tp_lookup(uint16_t dref, uint16_t sref, uint16_t dsel, uint16_t ssel, struct snpaaddr *daddr,
	  struct snpaaddr *saddr)
{
	struct tp *tp;

	if ((tp = tp_lookup_conn(dref, sref, daddr, saddr)))
		return (tp);
	if ((tp = tp_lookup_bind(dsel, daddr)))
		return (tp);
	return (NULL);
}

/**
 * tp_bind - bind a Stream to a TSAP
 * @tp: private structure to bind (locked)
 * @q: active queue in queue pair (write queue)
 * @tsap: TSAP address to bind
 *
 * Bind to a TSEL and possibly an SNPA address.  We basically have to go look in the bind hashes and
 * see if there is any other Stream bound to the same transport selector and subnetwork point of
 * attachment.  If there is (and it is as specific or more specific than the attempted bind
 * address), reject the bind; otherwise, permit it.  The bind hashes are looked up by TSEL to
 * determine the best T_CLTS to deliver a UD or the best T_COTS(_ORD) to deliver a CR.
 */
STATIC int
tp_bind(struct tp *tp, queue_t *q, struct tsapaddr *tsap, struct snpaaddr *ssap)
{
	struct tp_bhash_bucket *hp;
	uint16_t tsel = *(uint16_t *) tsap->tsel;
	struct tp *test;

	hp = &tp_bhash[tp_bhashfn(tsel)];
	write_lock(&hp->lock);
	for (test = hp->list; test; test = test->bnext)
		if (test->loc.type == TP_SNPA_TYPE_WILDCARD || test->loc.type == ssap->type)
			break;
	if (test != NULL) {
		write_unlock(&hp->lock);
		if (ssap->type != TP_SNPA_TYPE_WILDCARD)
			return (TADDRBUSY);
		return (TNOADDR);
	}
	if ((tp->bnext = hp->list))
		tp->bnext->bprev = &tp->bnext;
	tp->bprev = &hp->list;
	hp->list = tp;
	tp->bindb = hp;
	/* tp_get(tp); */
	write_unlock(&hp->lock);
	return (0);
}

STATIC int
tp_listen(struct tp *tp, queue_t *q, t_uscalar_t CONIND_number)
{
	/* FIXME FIXME FIXME */
	return (-ENXIO);
}

/**
 * tp_unbind - unbind a Stream from a TSAP
 * @tp: private structure to bind (locked)
 * @q: active queue in queue pair (write queue)
 *
 * Simply remote the transport Stream from the hashes.  This function can be called whether the
 * Stream is bound or not (and is always called before the private structure is freed).
 */
STATIC int
tp_unbind(struct tp *tp, queue_t *q)
{
	struct tp_bhash_bucket *hp;

	if ((hp = tp->bindb)) {
		write_lock(&hp->lock);
		if ((*tp->bprev = tp->bnext))
			tp->bnext->bprev = tp->bprev;
		tp->bnext = NULL;
		tp->bprev = &tp->bnext;
		tp->bindb = NULL;
		write_unlock(&hp->lock);
		bzero(&tp->src, sizeof(tp->src));
		bzero(&tp->loc, sizeof(tp->loc));
		/* tp_release(&tp); */
	}
	return (0);
}

static uint16_t last_allocated_lref = 0;

/**
 * tp_assign_lref - assign a local reference and place in connection hashes
 * @tp: private structure (locked)
 *
 * Assigns a local reference (if necessary) and places the stream into the
 * connection hashes with the corresponding local reference.
 */
static int
tp_assign_lref(struct tp *tp)
{
	if (tp->lref != 0)
		return (0);
	if ((tp->lref = last_allocated_lref + 1) == 0)
		tp->lref++;
	for (;;) {
		struct tp *up;
		struct tp_chash_bucket *hp;

		hp = &tp_chash[tp_chashfn(tp->lref, 0)];
		write_lock(&hp->lock);
		for (up = hp->list; up; up = up->cnext)
			if (up->lref == tp->lref)
				break;
		if (up == NULL) {
			if ((tp->cnext = hp->list))
				tp->cnext->cprev = &tp->cnext;
			tp->cprev = &hp->list;
			hp->list = tp;
			tp->connb = hp;
			/* tp_get(tp); */
			last_allocated_lref = tp->lref;
			write_unlock(&hp->lock);
			return (0);
		} else {
			tp->lref++;
			if (tp->lref == last_allocated_lref) {
				tp->lref = 0;
				write_unlock(&hp->lock);
				return (TNOADDR);
			}
			write_unlock(&hp->lock);
			continue;
		}
	}
}

/**
 * tp_connect - connect a Stream on a TSAP
 * @tp: private structure (locked)
 * @q: active queue (write queue)
 * @dp: user data to send with CR-TPDU
 *
 * This function simply places the Stream into the connection hashes (assigning a local reference if
 * necessary).  The remote reference should have been zeroed, and the destination TSAP address and
 * local SNPA addresses should have been already populated.
 *
 * NOTE: this function does not actually freeze local references according to spec; however, it does
 * provide loose assurances that local references are not reused within a period of the lifetime of
 * a connection as long as the number of connections is far less than 65,535.
 */
STATIC int
tp_connect(struct tp *tp, queue_t *q, mblk_t *dp)
{
	mblk_t *mp;
	int err;

	if ((err = tp_assign_lref(tp)))
		return (err);

	tp->lasterror = 0;
	if (!(mp = tp_pack_cr(tp, q, dp)))
		return (-ENOBUFS);
	tp_repeat(tp, q, mp);
	tp->pstate = _TP_STATE_WFCC;
	return (0);
}

/**
 * tp_disconnect - disconnect from the connection hashes
 * @tp: private structure (locked)
 *
 * This simply removes the Stream from the connection hashes and zeros the remote reference,
 * destination TSAP address and remote SNPA address.
 */
STATIC int
tp_disconnect(struct tp *tp)
{
	struct tp_chash_bucket *hp;

	if ((hp = tp->connb)) {
		write_lock(&hp->lock);
		if ((*tp->cprev = tp->cnext))
			tp->cnext->cprev = tp->cprev;
		tp->cnext = NULL;
		tp->cprev = &tp->cnext;
		tp->connb = NULL;
		tp->lref = tp->rref = 0;
		bzero(&tp->dst, sizeof(tp->dst));
		bzero(&tp->rem, sizeof(tp->rem));
		/* tp_release(&tp); */
		write_unlock(&hp->lock);
	}
	return (0);
}

/**
 * tp_accept - accept a connection indication
 * @tp: private structure (locked)
 * @q: active queue (write queue)
 * @ap: accepting stream private structure (locked)
 * @cp: connection indication
 *
 * This function absorbs the connection indication and associated data on success;
 * on failure, neither are affected and are preserved.
 *
 * Note that options from the received CR-TPDU must have already been allocated
 * against the accepting stream.
 */
STATIC int
tp_accept(struct tp *tp, queue_t *q, struct tp *ap, mblk_t *cp)
{
	mblk_t *mp, *dp = NULL;
	int err;

	if ((err = tp_assign_lref(ap)))
		return (err);

	if (unlikely((mp = tp_pack_cc(ap, q, dp)) == NULL))
		return (-ENOBUFS);
	tp_repeat(ap, q, mp);
	ap->pstate = _TP_STATE_AKWAIT;
	return (0);
}

/**
 * tp_refuse - refuse a connection indication
 * @tp: private structure (locked)
 * @q: active queue (write queue)
 * @cp: connection indication
 * @dp: diagnostic data
 *
 * This function absorbs both the connection indication and the diagnostic data on
 * success; otherwise, on failure, both are preserved.
 */
STATIC int
tp_refuse(struct tp *tp, queue_t *q, mblk_t *cp, mblk_t *dp)
{
	mblk_t *mp;

	/* first attempt to prepare and send off a connection refusal */
	if (unlikely((mp = tp_pack_dr(tp, q, 128 + 0, dp)) == NULL))
		return (-ENOBUFS);
	tp_queue_xmit(tp, q, mp);
	/* discard connection indication */
	bufq_unlink(&tp->conq, cp);
	freemsg(cp);
	/* restore previous state of the listening Stream */
	if (bufq_length(&tp->conq))
		tp_set_state(tp, TS_WRES_CIND);
	else {
		tp_set_state(tp, TS_IDLE);
		/* make sure any backed up indications are processed */
		qenable(tp->rq);
	}
	return (0);
}

/*
 *  =========================================================================
 *
 *  IP T-Provider --> T-User Primitives (Indication, Confirmation and Ack)
 *
 *  =========================================================================
 */

/**
 * m_flush: - issue an M_FLUSH message upstream
 * @tp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 * @how: how to flush (FLUSHR, FLUSHW, FLUSHBAND)
 * @band: band to flush when FLUSHBAND set
 *
 * This procedure should be flushing the read queue when FLUSHR is set before passing the message
 * upstream; however, there seemed to be some problems in doing so.  Turn this back on once the
 * problems are resolved. 
 */
static int
m_flush(struct tp *tp, queue_t *q, mblk_t *msg, int how, int band)
{
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, 2, BPRI_HI))))
		goto enobufs;
	mp->b_datap->db_type = M_FLUSH;
	mp->b_wptr[0] = how;
	mp->b_wptr[1] = band;
	mp->b_wptr += 2;
#if 0
	/* get things started off right */
	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(tp->rq, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(tp->rq, FLUSHDATA);
	}
#endif
	freemsg(msg);
	LOGTX(tp, "<- M_FLUSH");
	putnext(tp->rq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * m_error: - issue an M_ERROR message upstream
 * @tp: private structure (locked or unlocked)
 * @q: active qeuue
 * @msg: message to free upon success
 * @error: the error number to pass in the message
 *
 * The caller should determine what to do visa-vi the TPI state and the disposition of any attached
 * socket.
 */
static int
m_error(struct tp *tp, queue_t *q, mblk_t *msg, int error)
{
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, 2, BPRI_HI))))
		goto enobufs;
	mp->b_datap->db_type = M_ERROR;
	mp->b_wptr[0] = error < 0 ? -error : error;
	mp->b_wptr[1] = error < 0 ? -error : error;
	mp->b_wptr += 2;
	tp_set_state(tp, tp->oldstate);
	freemsg(msg);
	LOGTX(tp, "<- M_ERROR %d", error);
	putnext(tp->rq, mp);
#if 0
	tp_disconnect(tp);
	tp_set_state(tp, TS_IDLE);
	tp_set_state(tp, TS_UNBND);
#endif
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * m_hangup: - issue an M_HANGUP message upstream
 * @tp: private structure (locked or unlocked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 *
 * The caller should determine what to do visa-vi the TPI state and the disposition of any attached
 * socket.
 */
static inline fastcall __unlikely int
m_hangup(struct tp *tp, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, 0, BPRI_HI))))
		goto enobufs;
	mp->b_datap->db_type = M_HANGUP;
	tp_set_state(tp, tp->oldstate);
	freemsg(msg);
	LOGTX(tp, "<- M_HANGUP");
	putnext(tp->rq, mp);
#if 0
	tp_disconnect(tp);
	tp_set_state(tp, TS_IDLE);
	tp_set_state(tp, TS_UNBND);
#endif
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * m_error_reply: - reply with an M_ERROR or M_HANGUP message
 * @tp: private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @err: error to place in M_ERROR message
 *
 * Generates an M_ERROR or M_HANGUP message upstream to generate a fatal error when required by the
 * TPI specification.  The four errors that require the message to be placed on the message queue
 * (%EBUSY, %ENOBUFS, %EAGAIN, %ENOMEM) are simply returned.  Other errors are either translated to
 * an M_ERROR or M_HANGUP message.  If a message block cannot be allocated -%ENOBUFS is returned,
 * otherwise zero (0) is returned.
 */
noinline fastcall __unlikely int
m_error_reply(struct tp *tp, queue_t *q, mblk_t *msg, int err)
{
	int hangup = 0;
	int error = err;

	if (error > 0)
		error = -error;
	switch (-error) {
	case 0:
	case EBUSY:
	case ENOBUFS:
	case EAGAIN:
	case ENOMEM:
		return (error);
	case EPIPE:
	case ENETDOWN:
	case EHOSTUNREACH:
	case ECONNRESET:
	case ECONNREFUSED:
		hangup = 1;
		error = EPIPE;
		break;
	case EFAULT:
		LOGERR(tp, "%s() fault", __FUNCTION__);
	default:
	case EPROTO:
		err = (err < 0) ? -err : err;
		break;
	}
#if 0
	/* always flush before M_ERROR or M_HANGUP */
	if (m_flush(tp, q, NULL, FLUSHRW, 0) == -ENOBUFS)
		return (-ENOBUFS);
#endif
	if (hangup)
		return m_hangup(tp, q, msg);
	return m_error(tp, q, msg, err);
}

/**
 * t_info_ack: - issue a T_INFO_ACK primitive
 * @tp: private structure (locked)
 * @q: active queue (write queue)
 * @msg: message to free upon success (or NULL)
 */
static fastcall int
t_info_ack(struct tp *tp, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	struct T_info_ack *p;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;

	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	*p = tp->p.info;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	LOGTX(tp, "<- T_INFO_ACK");
	/* qreply(q, mp); */
	putnext(tp->rq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
}

/**
 * t_bind_ack: - issue a T_BIND_ACK primitive
 * @tp: private structure (locked)
 * @q: active queue
 * @ADDR_buffer: bound address pointer
 * @ADDR_length: bound address length
 * @CONIND_number: bound maximum number of outstanding connection indications
 *
 * Note that the bound address must have already been validated and bound to the stream, this
 * function merely performs the state transition and provides the necessary response.
 */
static fastcall int
t_bind_ack(struct tp *tp, queue_t *q, mblk_t *msg, unsigned char *ADDR_buffer, t_uscalar_t
	   ADDR_length, t_uscalar_t CONIND_number)
{
	mblk_t *mp;
	struct T_bind_ack *p;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p) + ADDR_length, BPRI_MED))))
		goto enobufs;

	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_BIND_ACK;
	p->ADDR_length = ADDR_length;
	p->ADDR_offset = ADDR_length ? sizeof(*p) : 0;
	p->CONIND_number = CONIND_number;
	mp->b_wptr += sizeof(*p);
	if (ADDR_length) {
		bcopy(ADDR_buffer, mp->b_wptr, ADDR_length);
		mp->b_wptr += ADDR_length;
	}
	tp_set_state(tp, TS_IDLE);
	freemsg(msg);
	LOGTX(tp, "<- T_BIND_ACK");
	/* qreply(q, mp); */
	putnext(tp->rq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
}

/**
 * t_error_ack: - issue a T_ERROR_ACK primitive
 * @tp: private structure (locked)
 * @q: active queue (write queue)
 * @msg: message to free upon success (or NULL)
 * @prim: primitive in error
 * @error: positive TPI error or negative UNIX error
 */
static int
t_error_ack(struct tp *tp, queue_t *q, mblk_t *msg, t_scalar_t prim, t_scalar_t error)
{
	mblk_t *mp;
	struct T_error_ack *p;

	switch (-error) {
	case EBUSY:
	case EAGAIN:
	case ENOMEM:
	case ENOBUFS:
	case EDEADLK:
		return (error);
	case 0:
		freemsg(msg);
		return (0);
	}
	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;

	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_ERROR_ACK;
	p->ERROR_prim = prim;
	if (error < 0) {
		p->TLI_error = TSYSERR;
		p->UNIX_error = -error;
	} else {
		p->TLI_error = error;
		p->UNIX_error = 0;
	}
	mp->b_wptr += sizeof(*p);
	tp_set_state(tp, tp->oldstate);
	freemsg(msg);
	LOGTX(tp, "<- T_ERROR_ACK");
	/* qreply(q, mp); */
	putnext(tp->rq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
}

/**
 * t_ok_ack: - issue a T_OK_ACK primitive
 * @tp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @prim: correct primitive
 * @cp: connection indication event to accept or disconnect
 * @dp: user data to send in the connection request or response
 * @ap: accepting Stream private structure (also locked)
 *
 * The accepting Stream must be locked before we do all of these perambulations on it.  The
 * accepting Stream must be locked and unlocked by the caller.
 */
STATIC INLINE streams_fastcall __hot_put int
t_ok_ack(struct tp *tp, queue_t *q, mblk_t *msg, t_scalar_t prim, mblk_t *cp, mblk_t *dp, struct tp *ap)
{
	struct T_ok_ack *p;
	int err = 0;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;

	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_OK_ACK;
	p->CORRECT_prim = prim;
	mp->b_wptr += sizeof(*p);
	switch (tp_get_state(tp)) {
	case TS_WACK_UREQ:
		if ((err = tp_unbind(tp, q)))
			goto free_error;
		/* TPI spec says that if the provider must flush both queues before responding with a
		   T_OK_ACK primitive when responding to a T_UNBIND_REQ. This is to flush queued data for
		   connectionless providers. */
		if (unlikely(m_flush(tp, q, NULL, FLUSHRW, 0)))
			goto enobufs;
		tp_set_state(tp, TS_UNBND);
		break;
	case TS_WACK_CREQ:
		if ((err = tp_connect(tp, q, dp)))
			goto free_error;
		tp_set_state(tp, TS_WCON_CREQ);
		break;
	case TS_WACK_CRES:
		ensure(ap && cp, goto free_error);
		/* NOTE: we should suck all of this into tp_accept() */
		ap->oldstate = tp_get_state(ap);
		if ((err = tp_accept(tp, q, ap, cp)))
			goto free_error;
		tp_set_state(ap, TS_DATA_XFER);
		if (ap != tp) {
			/* only change state if not accepting on listening socket */
			if (bufq_length(&tp->conq))
				tp_set_state(tp, TS_WRES_CIND);
			else {
				tp_set_state(tp, TS_IDLE);
				/* make sure any backed up indications are processed */
				qenable(tp->rq);
			}
		}
		/* make sure any data on the socket is delivered */
		// tp_r_recvmsg(ap, ap->rq); /* FIXME */
		break;
	case TS_WACK_DREQ7:
		ensure(cp, goto free_error);
		/* NOTE: we should suck all of this into tp_refuse() */
		if ((err = tp_refuse(tp, q, cp, dp)))
			goto free_error;
		if (bufq_length(&tp->conq))
			tp_set_state(tp, TS_WRES_CIND);
		else {
			tp_set_state(tp, TS_IDLE);
			/* make sure any backed up indications are processed */
			qenable(tp->rq);
		}
		break;
	case TS_WACK_DREQ6:
		/* NOTE: we should suck all of this into tp_disconnect() */
		if ((err = tp_disconnect(tp)))
			goto free_error;
		tp_set_state(tp, TS_IDLE);
		break;
	case TS_WACK_DREQ9:
	case TS_WACK_DREQ10:
	case TS_WACK_DREQ11:
		/* NOTE: we should suck all of this into tp_disconnect() */
		if ((err = tp_disconnect(tp)))
			goto free_error;
		/* TPI spec says that if the interface is in the TS_DATA_XFER, TS_WIND_ORDREL or
		   TS_WACK_ORDREL [sic] state, the stream must be flushed before responding with the T_OK_ACK 
		   primitive. */
		if (unlikely(m_flush(tp, q, NULL, FLUSHRW, 0)))
			goto enobufs;
		tp_set_state(tp, TS_IDLE);
		break;
	default:
		break;
		/* Note: if we are not in a WACK state we simply do not change state. This occurs normally
		   when we are responding to a T_OPTMGMT_REQ in other than the TS_IDLE state. */
	}
	freemsg(msg);
	LOGTX(tp, "<- T_OK_ACK");
	putnext(tp->rq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
      free_error:
	freemsg(mp);
	return t_error_ack(tp, q, msg, prim, err);
}

/**
 * t_conn_ind: - issue a T_CONN_IND primitive
 * @tp: private structure (locked)
 * @q: active queue
 * @cp: the CR-TPDU that created the connection indication
 *
 * This function creates a new connection indication and sequence number from the
 * open request child and generates a connection indication to the TLI user.  Note
 * that the @cp is the CR-TPDU that cause the connection indication: this is an
 * M_DATA message block containing the CR-TPDU with DB_BASE(cp) pointing to the
 * subnetwork header, and cp->b_rptr pointing to the CR-TPDU.
 *
 * The caller must check whether there is an existing connection indication in the
 * queue before calling this function.
 */
static fastcall int
t_conn_ind(struct tp *tp, queue_t *q, mblk_t *cp)
{
	mblk_t *mp;
	struct T_conn_ind *p;
	size_t src_len = tp->p.info.ADDR_size;
	size_t opt_len = tp_size_conn_opts(tp);
	unsigned char *src;
	int err;

	if (unlikely(tp_not_state(tp, TSM_LISTEN)))
		goto discard;
	if (unlikely(bufq_length(&tp->conq) > tp->conind))
		goto eagain;
	if (unlikely(!canputnext(tp->rq)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, sizeof(*p) + src_len + opt_len, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_CONN_IND;
	p->SRC_length = src_len;
	p->SRC_offset = src_len ? sizeof(*p) : 0;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) + src_len : 0;
	p->SEQ_number = (t_uscalar_t) (long) cp;
	mp->b_wptr += sizeof(*p);

	/* place source address from CR-TPDU in connection indication */
	src = cp->b_rptr;	/* FIXME: find address in CR TPDU */
	bcopy(src, mp->b_wptr, src_len);
	mp->b_wptr += src_len;

	/* place options from CR-TPDU in connection indication */
	if (opt_len) {
		if ((err =
		     t_build_conn_opts(tp, &tp->options.req, &tp->options.res, mp->b_wptr, opt_len, 1)) < 0)
			goto badopt;
		mp->b_wptr += opt_len;
	}
	bufq_queue(&tp->conq, cp);
	tp_set_state(tp, TS_WRES_CIND);
	LOGTX(tp, "<- T_CONN_IND");
	putnext(tp->rq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
      ebusy:
	LOGNO(tp, "ERROR: flow controlled");
	return (-EBUSY);
      eagain:
	LOGNO(tp, "ERROR: too many conn inds");
	return (-EAGAIN);
      badopt:
	freemsg(mp);
	LOGNO(tp, "ERROR: option build fault");
	goto discard;
      discard:
	freemsg(cp);
	LOGERR(tp, "%s() in wrong TPI state %d", __FUNCTION__, (int) tp_get_state(tp));
	return (0);
}

/**
 * t_conn_con: - issue a T_CONN_CON primitive
 * @tp: private structure (locked)
 * @q: active queue (read queue only)
 *
 * Issues a connection confirmation primitive to the TPI user as a result of a state change on the
 * underlying socket to the TCP_ESTABLISHED state from the TCP_SYN_SENT state.  Note that the
 * responding address is always the address to which the connection was attempted.  All options with
 * end-to-end significant and options that were specified on connection request are returned on
 * connection confirmation.
 *
 * Note that because only one connect confirmation message is sent on any given Stream, this
 * function does not check flow control.  This means that it is highly unlikely that this message
 * will fail.
 */
static fastcall int
t_conn_con(struct tp *tp, queue_t *q)
{
	mblk_t *mp;
	struct T_conn_con *p;
	struct tsapaddr *res = &tp->dst;
	size_t res_len = tp_addr_size(res->nsap.addr, res->nsap.len);
	size_t opt_len = tp_size_conn_opts(tp);
	int err;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p) + res_len + opt_len, BPRI_MED))))
		goto enobufs;

	mp->b_datap->db_type = M_PROTO;
	mp->b_band = 1;		/* expedite */
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_CONN_CON;
	p->RES_length = res_len;
	p->RES_offset = res_len ? sizeof(*p) : 0;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) + res_len : 0;
	mp->b_wptr += sizeof(*p);
	if (res_len) {
		bcopy(res, mp->b_wptr, res_len);
		mp->b_wptr += res_len;
	}
	if (opt_len) {
		if ((err =
		     t_build_conn_opts(tp, &tp->options.req, &tp->options.res, mp->b_wptr, opt_len, 0)) >= 0)
			mp->b_wptr += opt_len;
		else {
			freemsg(mp);
			LOGNO(tp, "ERROR: option build fault");
			return (0);
		}
	}
	tp_set_state(tp, TS_DATA_XFER);
	LOGTX(tp, "<- T_CONN_CON");
	putnext(tp->rq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
}

/**
 * t_discon_ind: - issue a T_DISCON_IND primitive
 * @tp: private structure (locked)
 * @q: active queue (read queue only)
 * @reason: disconnect reason
 * @cp: connection indication
 *
 * Disconnect indications are issued on normal Streams as follows: (In this case the sequence number
 * is always zero.)
 *
 * TS_WCON_CREQ state:
 *	when the underlying socket transitions to the TCP_TIME_WAIT or TCP_CLOSE state (posthumously
 *	from the TCP_SYN_SENT state).
 * TS_DATA_XFER state:
 *	when the underlying socket transitions to the TCP_TIME_WAIT or TCP_CLOSE state (posthumously
 *	from the TCP_ESTABLISHED state).
 * TS_WIND_ORDREL state:
 *	when the underlying socket transitions to the TCP_TIME_WAIT or TCP_CLOSE state from the
 *	TCP_CLOSE_WAIT state.
 * TS_WREQ_ORDREL state:
 *	when the underlying socket transitions to the TCP_TIME_WAIT or TCP_CLOSE state from the
 *	TCP_ESTABLISHED or TCP_FIN_WAIT1 state.
 *
 * Disconnect indications are issued on listening Streams as follows: (In this case the sequence
 * number is never zero.)
 *
 * TS_WRES_CIND state:
 *	when the child socket transitions to the TCP_TIME_WAIT or TCP_CLOSE state (posthumously from
 *	the TCP_SYN_RECV state).
 *
 */
static int
t_discon_ind(struct tp *tp, queue_t *q, t_scalar_t reason, mblk_t *cp)
{
	mblk_t *mp = NULL;
	struct T_discon_ind *p;
	t_uscalar_t seq;
	int err;

	seq = cp ? ((struct tp_conind *) (cp->b_rptr))->ci_seq : 0;

	switch (tp_get_state(tp)) {
	case TS_DATA_XFER:
	case TS_WIND_ORDREL:
	case TS_WREQ_ORDREL:
		/* TPI spec says that if the interface is in the TS_DATA_XFER, TS_WIND_ORDREL or
		   TS_WACK_ORDREL [sic] state, the stream must be flushed before sending up the T_DISCON_IND
		   primitive. */
		if (unlikely(m_flush(tp, tp->rq, NULL, FLUSHRW, 0)))
			goto enobufs;
		/* fall through */
	case TS_WCON_CREQ:
		assure(cp == NULL);
		break;
	case TS_WRES_CIND:
		/* If we are a listening Stream then the flow of connection indications and disconnect
		   indications must be controlled and flow control must be checked.  For other Streams, only
		   one disconnect indication can be issued for a given Stream and flow control need not be
		   checked. */
		assure(cp != NULL);
		if (unlikely(!canputnext(tp->rq)))
			goto ebusy;
		break;
	default:
		LOGERR(tp, "SWERR: out of state: %s %s:%d", __FUNCTION__, __FILE__, __LINE__);
		/* remove it anyway and feign success */
		if (cp && (err = tp_refuse(tp, q, cp, NULL)))
			goto error;
		return (0);
	}
	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;

	if (unlikely(cp && (err = tp_refuse(tp, q, cp, NULL)))) {
		freemsg(mp);
		goto error;
	}

	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_DISCON_IND;
	p->DISCON_reason = reason;
	p->SEQ_number = seq;
	mp->b_wptr += sizeof(*p);
	tp->lasterror = 0;
	if (!bufq_length(&tp->conq))
		tp_set_state(tp, TS_IDLE);
	else
		tp_set_state(tp, TS_WRES_CIND);
	LOGTX(tp, "<- T_DISCON_IND");
	putnext(tp->rq, mp);
	return (0);

      ebusy:
	LOGNO(tp, "ERROR: Flow controlled");
	err = -EBUSY;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	return (err);
}

/**
 * t_data_ind: - issue a T_DATA_IND primitive
 * @tp: private structure (locked)
 * @q: active queue
 * @MORE_flag: flag indicating more data
 * @dp: associated data
 */
static inline fastcall __hot_in int
t_data_ind(struct tp *tp, queue_t *q, t_uscalar_t MORE_flag, mblk_t *dp)
{
	struct T_data_ind *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		return (-ENOBUFS);

	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_DATA_IND;
	p->MORE_flag = MORE_flag ? T_MORE : 0;
	mp->b_wptr += sizeof(*p);
	dp->b_band = 0;		/* sometimes non-zero */
	dp->b_flag &= ~MSGMARK;	/* sometimes marked */
	mp->b_cont = dp;
	LOGTX(tp, "<- T_DATA_IND");
	/* putnext(q, mp); */
	putnext(tp->rq, mp);
	return (0);
}

/**
 * t_exdata_ind: - issue a T_EXDATA_IND primitive
 * @tp: private structure (locked)
 * @q: active queue
 * @msg: prepared message header
 * @dp: associated data
 */
static inline fastcall __hot_in int
t_exdata_ind(struct tp *tp, queue_t *q, struct msghdr *msg, mblk_t *dp)
{
	mblk_t *mp;
	struct T_exdata_ind *p;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;

	mp->b_datap->db_type = M_PROTO;
	mp->b_band = 1;		/* expedite */
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_EXDATA_IND;
	p->MORE_flag = (msg->msg_flags & MSG_EOR) ? 0 : T_MORE;
	mp->b_wptr += sizeof(*p);
	dp->b_band = 0;		/* sometimes non-zero */
	dp->b_flag &= ~MSGMARK;	/* sometimes marked */
	mp->b_cont = dp;
	LOGTX(tp, "<- T_EXDATA_IND");
	putnext(tp->rq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
}

// static fastcall __hot_in void tp_r_recvmsg(struct tp *tp, queue_t *q);

/**
 * t_unitdata_ind: - issue a T_UNITDATA_IND primitive
 * @tp: private structure (locked)
 * @q; active queue
 * @msg: pre-prepared message header
 * @dp: the data
 *
 * The UD TPDU arrived in the user data field of an N-UNITDATA indication.
 *
 * If a checksum parameter is present in the UD TPDU then a checksum verification
 * will be made of the UD TPDU using the algorithm defined in ITU-T Rec. X.234 (1994
 * E) 6.4.  If the result of the verification is false then the TPDU is discarded.
 * If the result of the verificaiton is true, or if the checksum mechanisms is not
 * used, then the transport entity will construct a T-UNITDATA indication and provide
 * it to the appropriate transport service user.
 *
 * The source network address from the N-UNITDATA indication and the source TSAP-ID
 * from the UD TPDU will be used to determine the source address parameter for the
 * T-UNITDATA indication.
 *
 * The destination network address from the N-UNITDATA indication and the destination
 * TSAP-ID from the UD TPDU will be used to determine the desitnation address
 * parameter for the T-UNITDATA indication.
 *
 * The user data field of the UD TPDU will be mapped to the user data paraemter of
 * the T-UNITDATA indicaiton.
 *
 * The QOS parameter is derived from the a priori knowledge of the QOS available from
 * the association and whether the checksum mechanism was used.
 */
STATIC INLINE fastcall __hot_in int
t_unitdata_ind(struct tp *tp, queue_t *q, struct netbuf *src, struct netbuf *opt, mblk_t *dp)
{
	struct T_unitdata_ind *p;
	mblk_t *mp;

	if (unlikely(!canputnext(tp->rq)))
		return (-EBUSY);
	if (unlikely(!(mp = mi_allocb(q, sizeof(*p) + src->len + opt->len, BPRI_MED))))
		return (-ENOBUFS);
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_UNITDATA_IND;
	p->SRC_length = src->len;
	p->SRC_offset = src->len ? sizeof(*p) : 0;
	p->OPT_length = opt->len;
	p->OPT_offset = opt->len ? sizeof(*p) + src->len : 0;
	mp->b_wptr += sizeof(*p);
	if (src->len) {
		bcopy(src->buf, mp->b_wptr, src->len);
		mp->b_wptr += src->len;
	}
	if (opt->len) {
		bcopy(opt->buf, mp->b_wptr, opt->len);
		mp->b_wptr += opt->len;
	}
	/* trim the header */
	dp->b_rptr += dp->b_rptr[0];
	dp->b_band = 0;
	dp->b_flag &= ~MSGMARK;
	mp->b_cont = dp;
	LOGTX(tp, "<- T_UNITDATA_IND");
	/* putnext(q, mp); */
	putnext(tp->rq, mp);
	return (0);
}

/**
 * t_uderror_ind: - issue a T_UDERROR_IND primitive
 * @tp: private structure (locked)
 * @q: active queue
 * @msg: pre-prepared message header
 * @dp: the data
 *
 * This primitive indicates to the transport user that a datagram with the specified destination
 * address and options produced an error.
 *
 * This is not called, but should be when T_UNITDATA_REQ fails due to options errors or permission.
 */
static inline fastcall __hot_get int
t_uderror_ind(struct tp *tp, queue_t *q, struct msghdr *msg, mblk_t *dp)
{
	mblk_t *mp;
	struct T_uderror_ind *p;
	size_t opt_len = 0;		// tp_opts_size(tp, msg);

	if (unlikely(!bcanputnext(tp->rq, 1)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, sizeof(*p) + msg->msg_namelen + opt_len, BPRI_MED))))
		goto enobufs;

	mp->b_datap->db_type = M_PROTO;
	mp->b_band = 1;		/* XXX move ahead of data indications */
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_UDERROR_IND;
	p->DEST_length = msg->msg_namelen;
	p->DEST_offset = msg->msg_namelen ? sizeof(*p) : 0;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) + msg->msg_namelen : 0;
	p->ERROR_type = 0;
	mp->b_wptr += sizeof(*p);
	if (msg->msg_namelen) {
		bcopy(msg->msg_name, mp->b_wptr, msg->msg_namelen);
		mp->b_wptr += msg->msg_namelen;
	}
	if (opt_len) {
		// tp_opts_build(tp, msg, mp->b_wptr, opt_len, &p->ERROR_type);
		mp->b_wptr += opt_len;
	}
	if (dp) {
		dp->b_band = 0;	/* sometimes non-zero */
		dp->b_flag &= ~MSGMARK;	/* sometimes marked */
		mp->b_cont = dp;
	}
	LOGTX(tp, "<- T_UDERROR_IND");
	putnext(tp->rq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
      ebusy:
	LOGNO(tp, "ERROR: Flow controlled");
	return (-EBUSY);
}

/**
 * t_optmgmt_ack: - issue a T_OPTMGMT_ACK primitive
 * @tp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @flags: management flags
 * @req: pointer to the requested options
 * @req_len: length of the requested options
 * @opt_len: estimate of the length of the reply options
 *
 * Note: opt_len is conservative but might not be actual size of the output options.  This will be
 * adjusted when the option buffer is built.
 */
static fastcall int
t_optmgmt_ack(struct tp *tp, queue_t *q, mblk_t *msg, t_scalar_t flags, unsigned char *req, size_t req_len,
	      size_t opt_len)
{
	mblk_t *mp;
	struct T_optmgmt_ack *p;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p) + opt_len, BPRI_MED))))
		goto enobufs;

	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	if ((flags = t_build_options(tp, req, req_len, mp->b_wptr, &opt_len, flags)) < 0) {
		freemsg(mp);
		return (flags);
	}
	p->PRIM_type = T_OPTMGMT_ACK;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) : 0;
	p->MGMT_flags = flags;
	if (opt_len) {
		mp->b_wptr += opt_len;
	}
#ifdef TS_WACK_OPTREQ
	if (tp_get_state(tp) == TS_WACK_OPTREQ)
		tp_set_state(tp, TS_IDLE);
#endif
	freemsg(msg);
	LOGTX(tp, "<- T_OPTMGMT_ACK");
	putnext(tp->rq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
}

/**
 * t_ordrel_ind: - issue a T_ORDREL_IND primitive
 * @tp: private structure (locked)
 * @q: active queue (read queue only)
 *
 * This primitive is an orderly release indication.  An orderly release indication is set as a
 * result of a state change of the underlying socket from the TCP_ESTABLISHED state to the
 * TCP_CLOSE_WAIT state (half close), or from the TCP_FIN_WAIT2 state to the TCP_TIME_WAIT or
 * TCP_CLOSE state (full close after half close).
 *
 * Note that because only one of these primitives can be sent on a given Stream, flow control is not
 * checked.  This means that it is very unlikely that this message will fail.
 */
static int
t_ordrel_ind(struct tp *tp, queue_t *q)
{
	mblk_t *mp;
	struct T_ordrel_ind *p;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;

	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_ORDREL_IND;
	mp->b_wptr += sizeof(*p);
	switch (tp_get_state(tp)) {
	case TS_DATA_XFER:
		/* In this case the underlying socket has transitioned form the TCP_ESTABLISHED state to the
		   TCP_CLOSE_WAIT state. */
		tp_set_state(tp, TS_WREQ_ORDREL);
		break;
	case TS_WIND_ORDREL:
		/* In this case the underlying socket has transitioned from the TCP_FIN_WAIT2 state to the
		   TCP_TIME_WAIT or TCP_CLOSE state. */
		tp_set_state(tp, TS_IDLE);
		break;
	}
	LOGTX(tp, "<- T_ORDREL_IND");
	putnext(tp->rq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
}

/**
 * t_optdata_ind: - issue a T_OPTDATA_IND primitive
 * @tp: private structure (locked)
 * @q: active queue
 * @msg: pre-prepared message header
 * @dp: the data
 */
static inline fastcall __hot_in int
t_optdata_ind(struct tp *tp, queue_t *q, struct msghdr *msg, mblk_t *dp)
{
	mblk_t *mp;
	struct T_optdata_ind *p;
	size_t opt_len = 0;		// tp_opts_size(tp, msg);

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p) + opt_len, BPRI_MED))))
		goto enobufs;

	mp->b_datap->db_type = M_PROTO;
	mp->b_band = (msg->msg_flags & MSG_OOB) ? 1 : 0;	/* expedite */
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_OPTDATA_IND;
	p->DATA_flag =
	    ((msg->msg_flags & MSG_EOR) ? 0 : T_ODF_MORE) | ((msg->msg_flags & MSG_OOB) ? T_ODF_EX : 0);
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) : 0;
	mp->b_wptr += sizeof(*p);
	if (opt_len) {
		// tp_opts_build(tp, msg, mp->b_wptr, opt_len, NULL);
		mp->b_wptr += opt_len;
	}
	dp->b_band = 0;		/* sometimes non-zero */
	dp->b_flag &= ~MSGMARK;	/* sometimes marked */
	mp->b_cont = dp;
	LOGTX(tp, "<- T_OPTDATA_IND");
	putnext(tp->rq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
}

#ifdef T_ADDR_ACK
/**
 * t_addr_ack: - issue a T_ADDR_ACK primitive
 * @tp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @loc: local address (or NULL)
 * @rem: remote address (or NULL)
 */
static fastcall int
t_addr_ack(struct tp *tp, queue_t *q, mblk_t *msg, struct tsapaddr *loc, struct tsapaddr *rem)
{
	mblk_t *mp;
	struct T_addr_ack *p;
	size_t loc_len = tp_addr_size(loc->nsap.addr, loc->nsap.len);
	size_t rem_len = tp_addr_size(rem->nsap.addr, rem->nsap.len);

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p) + loc_len + rem_len, BPRI_MED))))
		goto enobufs;

	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_ADDR_ACK;
	p->LOCADDR_length = loc_len;
	p->LOCADDR_offset = loc_len ? sizeof(*p) : 0;
	p->REMADDR_length = rem_len;
	p->REMADDR_offset = rem_len ? sizeof(*p) + loc_len : 0;
	mp->b_wptr += sizeof(*p);
	if (loc_len) {
		bcopy(loc, mp->b_wptr, loc_len);
		mp->b_wptr += loc_len;
	}
	if (rem_len) {
		bcopy(rem, mp->b_wptr, rem_len);
		mp->b_wptr += rem_len;
	}
	freemsg(msg);
	LOGTX(tp, "<- T_ADDR_ACK");
	putnext(tp->rq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
}
#endif

#ifdef T_CAPABILITY_ACK
/**
 * t_capability_ack: - issue a T_CAPABILITY_ACK primitive
 * @tp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @caps: capability bits
 * @type: message type (M_PROTO or M_PCPROTO)
 *
 * Note that TPI Revision 2 Draft 2 says that if the T_CAPABILITY_REQ is sent as a M_PCPROTO then
 * the the T_CAPABILITY_ACK must be sent as an M_PCPROTO and that if the T_CAPABILITY_REQ was sent
 * as a M_PROTO, then the T_CAPABILITY_ACK must also be sent as a M_PROTO.
 */
static fastcall int
t_capability_ack(struct tp *tp, queue_t *q, mblk_t *msg, t_uscalar_t caps, int type)
{
	mblk_t *mp;
	struct T_capability_ack *p;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;

	mp->b_datap->db_type = type;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_CAPABILITY_ACK;
	p->CAP_bits1 = caps & (TC1_INFO | TC1_ACCEPTOR_ID);
	p->ACCEPTOR_id = (caps & TC1_ACCEPTOR_ID) ? (t_uscalar_t) (long) tp->rq : 0;
	mp->b_wptr += sizeof(*p);
	if (caps & TC1_INFO)
		p->INFO_ack = tp->p.info;
	else
		bzero(&p->INFO_ack, sizeof(p->INFO_ack));
	freemsg(msg);
	LOGTX(tp, "<- T_CAPABILITY_ACK");
	putnext(tp->rq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
}
#endif

/*
 *  =========================================================================
 *
 *  Received message handling:
 *
 *  =========================================================================
 */

/**
 * tp_tst_checksum - test the checksum
 * @mp: message containin TPDU
 * @offset: offset of variable part within message.
 *
 * Test the checksum * by calculating the checksum for the TPDU.  This calculation
 * follows the procedures in X.224 Annex D.  It is necessary that the TPDU contains
 * a checksum, but not necessary to know where it is located.
 */
STATIC INLINE __hot int
tp_tst_checksum(mblk_t *mp)
{
	mblk_t *bp;
	int c0 = 0, c1 = 0, L = 0;

	for (bp = mp; bp; bp = bp->b_cont) {
		register unsigned char *p = bp->b_rptr;
		register unsigned char *e = bp->b_wptr;

		if (p < e) {
			L += e - p;
			while (p < e) {
				c0 += *p++;
				c1 += c0;
			}
		}
	}
	return ((c1 == 0) && (c0 == 0));
}

static int
tp_cr_check(struct tp *tp, mblk_t *mp)
{
	size_t msize, csize;
	mblk_t *cp;
	pl_t pl;

	msize = DB_BASE(mp) - mp->b_rptr + 4;
	pl = bufq_lock(&tp->conq);
	for (cp = bufq_head(&tp->conq); cp; cp = cp->b_next) {
		csize = DB_BASE(cp) - cp->b_rptr + 4;

		if (csize == msize && memcmp(DB_BASE(mp), DB_BASE(cp), msize) == 0) {
			bufq_unlock(&tp->conq, pl);
			return (0);
		}
	}
	bufq_unlock(&tp->conq, pl);
	return (1);
}

/**
 * tp_recv_cr - a CR is received
 * @tp: private structure (locked)
 * @q: active queue (read queue)
 * @mp: the CR message
 *
 * In state _TP_STATE_CLOSED:
 * If not _TP_PREDICATE_P8 (acceptable CR-TPDU), _TP_ACTION_21 (send a DC-TPDU with
 *	src-ref equal to zero), stay in _TP_STATE_CLOSED.
 * If _TP_PREDICATE_P8 (acceptable CR-TPDU), _TP_ACTION_1 (count = count + 1),
 *	_TP_ACTION_9 (set initial credit for sending according to the received
 *	CR/CC-TPDU), _TP_ACTION_3 (set retransmission timer), generate T-CONNECTION
 *	indication, move to _TP_STATE_WFTRESP, _TP_NOTE_5 (not a duplicate CR-TPDU;
 *	if duplicated, ignore it).
 *
 * In state _TP_STATE_OPEN:
 * _TP_ACTION_8 (stop inactivity timer if running), _TP_ACTION_7 (set inactivity
 *	timer), stay in _TP_STATE_OPEN.
 *
 * In state _TP_STATE_WFTRESP: stay in _TP_STATE_WFTRESP.
 *
 * In state _TP_STATE_AKWAIT: send CC-TPDU, stay in _TP_STATE_AKWAIT.
 *
 * In state _TP_STATE_CLOSING:
 * stay in _TP_STATE_CLOSING, _TP_NOTE_13 (the DR-TPDU may be either repeated
 *	immediately or when T1 will run out).
 */
static int
tp_recv_cr(struct tp *tp, queue_t *q, mblk_t *mp)
{
	register unsigned char *m;
	int hlen, mlen;
	uint16_t dref, sref;
	int li, acceptable = 1, copt, credit, extf;

	/* Note that checksum is always present in CR */
	if (unlikely(!tp_tst_checksum(mp)))
		goto badpdu;

	/* check for duplicate connection request */
	if (unlikely(!tp_cr_check(tp, mp)))
		goto duplicate;

	m = mp->b_rptr;
	mlen = msgsize(mp);
	if (unlikely(mlen < 2))
		goto badpdu;
	hlen = li = m[0];
	if (unlikely(mlen < li))
		goto badpdu;
	if (unlikely(li < 2))
		goto badpdu;
	if ((m[1] & 0xf0) != _TP_MT_CR)
		goto badpdu;
	credit = m[1] & 0x0f;
	if (unlikely(li < 7))
		goto badpdu;
	/* DST-REF must always be zero in CR-TPDU */
	if (unlikely((dref = (m[2] << 8) | m[3]) != 0))
		goto badpdu;
	dref = tp->rref;
	if (unlikely((sref = (m[4] << 8) | m[5]) == 0))
		goto badpdu;
	if (unlikely((copt = m[6] & 0x0f) != T_CLASS4))
		goto badpdu;
	if (unlikely(m[6] & _T_F_CO_RESERVED4))
		goto badpdu;
	if (unlikely(m[6] & _T_F_CO_RESERVED3))
		goto badpdu;
	if (unlikely(m[6] & _T_F_CO_FLOWCTRL))
		goto badpdu;	/* class 2 only */
	extf = (m[6] & _T_F_CO_EXTFORM) ? 1 : 0;
	m += 7;
	hlen -= 7;
	while (hlen > 0) {
		li = m[1];
		switch (m[0]) {
			int val;

		case _TP_PT_CGTRANSSEL:
			if (li >= 2) {
				tp->dst.tsel[0] = m[2];
				tp->dst.tsel[1] = m[3];
			}
			break;
		case _TP_PT_CDTRANSSEL:
			if (li >= 2) {
				tp->src.tsel[0] = m[2];
				tp->src.tsel[1] = m[3];
			}
			break;
		case _TP_PT_TPDU_SIZE:
			if (li != 1)
				goto badpdu;
			val = m[2];
			if (val < 7 || val > 13)
				goto badpdu;
			val = (1 << val);
			break;
		case _TP_PT_PREF_TPDU_SIZE:
			break;
		case _TP_PT_VERSION:
			if (li != 1 || m[2] != 0x1)
				goto badpdu;
			break;
		case _TP_PT_PROTECTION:
			break;
		case _TP_PT_CHECKSUM:
			break;
		default:
			break;
		}
		m += 2 + li;
		hlen -= 2 + li;
	}

	switch (tp->pstate) {
	case _TP_STATE_CLOSED:
		if (!acceptable) {
		} else {
		}
		break;
	case _TP_STATE_OPEN:
		tp_timer_stop(tp, t2);
		tp_timer_start(tp, q, t2);
		break;
	case _TP_STATE_WFTRESP:
		/* We are waiting for a response from the user, this is likely a repeat. */
		break;
	case _TP_STATE_AKWAIT:
		/* We have already received a CR-TPDU to which we have sent a CC-TPDU.  Send the CC-TPDU
		   again and stay in this state. */
		tp_resend(tp, q);
		break;
	case _TP_STATE_CLOSING:
#if 0
		tp_resend(tp, q);
#endif
		break;
	}
	freemsg(mp);
	return (0);

      badpdu:
	LOGRX(tp, "discarding bad CR-TPDU");
	freemsg(mp);
	return (0);
#if 0
      enobufs:
	err = -ENOBUFS;
	goto error;

      error:
	return (err);
#endif
      duplicate:
	LOGRX(tp, "discarding multiple CR-TPDU");
	freemsg(mp);
	return (0);
}

/**
 * tp_recv_cc - a CC is received
 * @tp: private structure (locked)
 * @q: active queue (read queue)
 * @mp: the CC message
 *
 * In state _TP_STATE_REFWAIT: Send DR-TPDU, stay in _TP_STATE_REFWAIT.
 *
 * In state _TP_STATE_CLOSED: stay in _TP_STATE_CLOSED.
 *
 * In state _TP_STATE_WFCC: If _TP_PREDICATE_P9 (acceptable class 4 CC-TPDU), _TP_ACTION_9 (set
 * initial credit for sending according to the received CR/CC-TPDU), _TP_ACTION_2 (count = 0),
 * _TP_ACTION_4 (stop retransmission timer if running), _TP_ACTION_5 (set window timer),
 * _TP_ACTION_7 (set inactivity timer), _TP_ACTION_17 (send a TPDU according to data transfer
 * procedure), generate T-CONNECTION confirmation, _TP_NOTE_9 (at least an AK-TPDU shall be sent if
 * the transport entity is the initiator in order to ensure that the responder will complete its
 * three-way handshake).  If not _TP_PREDICATE_P9 (acceptable class 4 CC-TPDU), _TP_ACTION_4 (stop
 * retransmission timer if running), _TP_ACTION_3 (set retransmission timer), _TP_ACTION_2 (count =
 * 0), _TP_ACTION_1 (count = count + 1), _TP_ACTION_23 (send a DR-TPDU with src-ref = local-ref and
 * dst-ref = SRC-REF in CC-TPDU), generate T-DISCONNECT indiciation, move to _TP_STATE_CLOSING.
 *
 * In state _TP_STATE_WBCL: If _TP_PREDICATE_P9 (acceptable class 4 CC-TPDU), _TP_ACTION_2 (count =
 * 0), _TP_ACTION_4 (stop retransmission timer if running), _TP_ACTION_3 (set retransmission timer),
 * _TP_ACTION_1 (count = count + 1), _TP_ACTION_15 (send the DR-TPDU, this DR-TPDU is sent with
 * src-ref = local-ref and dst-ref = remote-ref (may be zero)), move to _TP_STATE_CLOSING.
 *
 * In state _TP_STATE_OPEN: _TP_ACTION_17 (set a TPDU according to data transfer procedures),
 * _TP_ACTION_8 (stop inactivity timer if running), _TP_ACTION_7 (start inactivity timer),
 * _TP_NOTE_9 (at least an AK-TPDU shall be sent if the transport entity is the initiator in order
 * to ensure that the responder will complete its three-way handshake), stay in _TP_STATE_OPEN.
 *
 * In state _TP_STATE_CLOSING: If _TP_PREDICATE_P9 (acceptable class 4 TC-TPDU), _TP_NOTE_11 (if the
 * CLOSING state has been entered, coming from WFCC state, the remote-ref is zero.  The SRC-REF
 * field of the CC-TPDU is ignored (i.e. if the DR-TPDU is retransmitted, it will be with the
 * dst-ref field set to zero)), stay in state _TP_STATE_CLOSING.
 */
static int
tp_recv_cc(struct tp *tp, queue_t *q, mblk_t *mp)
{
#if 0
	/* FIXME: caller must do this */
	if (tp->options.res.tco.tco_checksum == T_YES) {
		if (!tp_tst_checksum(mp)) {
			freemsg(mp);
			return (0);
		}
	}
#endif
	return (0);
}

/**
 * tp_recv_er - a ER is received
 * @tp: private structure (locked)
 * @q: active queue (read queue)
 * @mp: the ER message
 *
 * In state _TP_STATE_REFWAIT: stay in _TP_STATE_REFWAIT.
 *
 * In state _TP_STATE_CLOSED: stay in _TP_STATE_CLOSED.
 *
 * In state _TP_STATE_WFCC: _TP_ACTION_0 (set reference timer), generate T-DISCONNECT indication,
 * move to _TP_STATE_REFWAIT.
 *
 * In state _TP_STATE_WBCL: _TP_ACTION_0 (set reference timer), move to _TP_STATE_REFWAIT.
 *
 * In state _TP_STATE_OPEN: _TP_ACTION_6 (stop window timer if running), _TP_ACITON_8 (stop
 * inactivit timer if running), _TP_ACTION_4 (stop retransmission timer if running), _TP_ACTION_3
 * (set retransmission timer), _TP_ACTION_2 (count = 0), _TP_ACTION_1 (count = count + 1),
 * _TP_ACTION_15 (send the DR-TPDU, this DR-TPDU is sent with src-ref = local-ref and dst-ref =
 * remote-ref (may be zero)), generate T-DISCONNECT indication, move to _TP_STATE_CLOSING.
 *
 * In state _TP_STATE_AKWAIT: _TP_ACTION_4 (stop retransmission timer if running), _TP_ACTION_3 (set
 * retransmission timer), _TP_ACTION_2 (count = 0), _TP_ACTION_1 (count = count + 1), _TP_ACTION_15
 * (send the DR-TPDU, this DR-TPDU is sent with src-ref = local-ref and dst-ref = remote-ref (may be
 * zero)), generate T-DISCONNECT indication, move to _TP_STATE_CLOSING.
 *
 * In state _TP_STATE_CLOSING: _TP_ACTION_0 (set reference timer), move to _TP_STATE_REFWAIT.
 */
static int
tp_recv_er(struct tp *tp, queue_t *q, mblk_t *mp)
{
#if 0
	/* FIXME: caller must do this */
	if (tp->options.res.tco.tco_checksum == T_YES) {
		if (!tp_tst_checksum(mp)) {
			freemsg(mp);
			return (0);
		}
	}
#endif
	return (0);
}

/**
 * tp_recv_dr - a DR is received
 * @tp: private structure (locked)
 * @q: active queue (read queue)
 * @dp: the DR message
 *
 * In state _TP_STATE_REFWAIT: _TP_ACTION_22 (send a DC-TPDU with src-ref equal to
 * zero), stay in _TP_STATE_REFWAIT.
 *
 * In state _TP_STATE_CLOSED: _TP_ACTION_22 (send a DC-TPDU with src-ref equal to
 * zero), stay in _TP_STATE_CLOSED.
 *
 * In state _TP_STATE_WFCC: _TP_NOTE_8 (association to this transport connection is
 * done regardless of the SRC-REF field; if SRC-REF is not zero, a DC-TPDU is sent
 * back), _TP_ACTION_0 (set reference timer), generate T-DISCONNECT indication,
 * move to _TP_STATE_REFWAIT.
 *
 * In state _TP_STATE_WBCL: _TP_NOTE_8 (association to this transport connection is
 * done regardless of the SRC-REF field; if SRC-REF is not zero, a DC-TPDU is sent
 * back), _TP_ACTION_0 (set reference timer), move to _TP_STATE_REFWAIT.
 *
 * In state _TP_STATE_OPEN: Send DC-TPDU,
 * _TP_NOTE_10 (if association has been made, and DST-REF is zero, then the DC-TPDU
 * contains a src-ref field set to zero),
 * _TP_ACTION_0 (set reference timer),
 * generate T-DISCONNECT indication,
 * move to _TP_STATE_REFWAIT.
 *
 * In state _TP_STATE_WFTRESP: Send DC-TPDU, _TP_NOTE_10 (if association has been
 * made, and DST-REF is zero, then the DC-TPDU contains a src-ref field set to
 * zero), generate T-DISCONNECT indication, move to _TP_STATE_CLOSED.
 *
 * In state _TP_STATE_AKWAIT: Send DC-TPDU, _TP_NOTE_10 (if association has been
 * made, and DST-REF is zero, then the DC-TPDU contains a src-ref field set to
 * zero), _TP_ACTION_0 (set reference timer), generate T-DISCONNECT indication,
 * move to _TP_STATE_REFWAIT
 *
 * In state _TP_STATE_CLOSING: _TP_ACTION_0 (set reference timer), move to
 * _TP_STATE_REFWAIT.
 */
static int
tp_recv_dr(struct tp *tp, queue_t *q, mblk_t *dp)
{
	uint16_t dref, sref;
	mblk_t *mp;
	int err;

#if 0
	/* FIXME: caller must do this */
	if (tp->options.res.tco.tco_checksum == T_YES) {
		if (!tp_tst_checksum(dp)) {
			freemsg(dp);
			return (0);
		}
	}
#endif

	dref = (dp->b_rptr[2] << 8) | dp->b_rptr[3];
	sref = (dp->b_rptr[4] << 8) | dp->b_rptr[5];

	switch (tp->pstate) {
	case _TP_STATE_WFCC:
	case _TP_STATE_WBCL:
		if (sref == 0)
			break;
		goto senddc;
	case _TP_STATE_REFWAIT:
	case _TP_STATE_CLOSED:
		dref = 0;
		goto senddc;
	case _TP_STATE_OPEN:
	case _TP_STATE_AKWAIT:
	case _TP_STATE_WFTRESP:
	      senddc:
		if ((mp = tp_pack_dc(tp, q, sref, dref)) == NULL)
			goto enobufs;
		tp_queue_xmit(tp, q, mp);
		freemsg(mp);
		break;
	}
	switch (tp->pstate) {
	case _TP_STATE_WBCL:
	case _TP_STATE_AKWAIT:
	case _TP_STATE_WFTRESP:
		if ((err = t_discon_ind(tp, q, _TP_REASON_UNSPECIFIED, NULL)))
			goto error;
		break;
	}
	switch (tp->pstate) {
	case _TP_STATE_WFCC:
	case _TP_STATE_WBCL:
	case _TP_STATE_OPEN:
	case _TP_STATE_AKWAIT:
	case _TP_STATE_CLOSING:
		tp_timer_start(tp, q, t4);
		tp->pstate = _TP_STATE_REFWAIT;
		break;
	case _TP_STATE_WFTRESP:
		tp->pstate = _TP_STATE_CLOSED;
		break;
	}
	return (0);
      error:
	return (err);
      enobufs:
	err = -ENOBUFS;
	goto error;
}

/**
 * tp_recv_dc - a DC is received
 * @tp: private structure (locked)
 * @q: active queue (read queue)
 * @mp: the DC message
 *
 * In state _TP_STATE_REFWAIT: stay in _TP_STATE_REFWAIT.
 *
 * In state _TP_STATE_CLOSED: stay in _TP_STATE_CLOSED.
 *
 * In state _TP_STATE_CLOSING: _TP_ACTION_0 (set reference timer), move to
 * _TP_STATE_REFWAIT.
 */
static int
tp_recv_dc(struct tp *tp, queue_t *q, mblk_t *mp)
{
#if 0
	/* FIXME: caller must do this */
	if (tp->options.res.tco.tco_checksum == T_YES) {
		if (!tp_tst_checksum(mp)) {
			freemsg(mp);
			return (0);
		}
	}
#endif
	switch (tp->pstate) {
	case _TP_STATE_REFWAIT:
	case _TP_STATE_CLOSED:
		break;
	case _TP_STATE_CLOSING:
		tp_timer_start(tp, q, t4);
		tp->pstate = _TP_STATE_REFWAIT;
		break;
	default:
		break;
	}
	return (0);
}

/**
 * tp_recv_ea - a EA is received
 * @tp: private structure (locked)
 * @q: active queue (read queue)
 * @mp: the EA message
 *
 * In state _TP_STATE_REFWAIT: stay in _TP_STATE_REFWAIT.
 *
 * In state _TP_STATE_CLOSED: stay in _TP_STATE_CLOSED.
 *
 * In state _TP_STATE_OPEN: _TP_ACTION_8 (stop inactivity timer if running),
 * _TP_ACTION_7 (set inactivity timer), stay in _TP_STATE_OPEN, _TP_NOTE_16 (see
 * data transfer procedures).
 *
 * In state _TP_STATE_CLOSING: stay in _TP_STATE_CLOSING, _TP_NOTE_13 (the DR-TPDU
 * may be either repeated immediately or when T1 will run out).
 */
static int
tp_recv_ea(struct tp *tp, queue_t *q, mblk_t *mp)
{
#if 0
	/* FIXME: caller must do this */
	if (tp->options.res.tco.tco_checksum == T_YES) {
		if (!tp_tst_checksum(mp)) {
			freemsg(mp);
			return (0);
		}
	}
#endif
	return (0);
}

/**
 * tp_recv_dt - a DT is received
 * @tp: private structure (locked)
 * @q: active queue (read queue)
 * @mp: the DT message
 *
 * In state _TP_STATE_REFWAIT: stay in _TP_STATE_REFWAIT.
 *
 * In state _TP_STATE_CLOSED: stay in _TP_STATE_CLOSED.
 *
 * In state _TP_STATE_OPEN: _TP_ACTION_8 (stop inactivity timer if running),
 * _TP_ACTION_7 (set inactivity timer), stay in _TP_STATE_OPEN, _TP_NOTE_16 (see
 * data transfer procedures).
 *
 * In state _TP_STATE_AKWAIT: _TP_ACTION_7 (set inactivity timer), move to
 * _TP_STATE_OPEN, _TP_NOTE_15 (previously stored T-DATA or T-EXPEDITED-DATA
 * requests are ready for processing according to data transfer procedures),
 * _TP_NOTE_16 (see data transfer procedures).
 *
 * In state _TP_STATE_CLOSING: stay in _TP_STATE_CLOSING, _TP_NOTE_13 (the DR-TPDU
 * may be either repeated immediately or when T1 will run out).
 */
static int
tp_recv_dt(struct tp *tp, queue_t *q, mblk_t *mp)
{
	register unsigned char *m;
	int hlen, mlen, dlen;
	int roa, eot;
	uint16_t dref;
	uint32_t seq;

#if 0
	/* FIXME: caller must do this */
	if (tp->options.res.tco.tco_checksum == T_YES) {
		if (!tp_tst_checksum(mp)) {
			freemsg(mp);
			return (0);
		}
	}
#endif
	if (unlikely(tp_get_state(tp) != TS_DATA_XFER))
		goto outstate;

	mlen = msgsize(mp);
	m = mp->b_rptr;
	if (unlikely(mlen < 1))
		goto badpdu;
	hlen = *m++;
	dlen = mlen - hlen;
	hlen--;
	if (unlikely(dlen < 0))
		goto badpdu;
	if (unlikely(mlen < 2))
		goto badpdu;
	if ((*m & 0xfe) != _TP_MT_DT)
		goto badpdu;
	roa = *m++ & 0x01;
	hlen--;
	dref = (m[0] << 8) | m[1];
	m += 2;
	hlen -= 2;
	if (!(tp->opts & _T_F_CO_EXTFORM)) {
		/* short form */
		eot = ((m[0] & 0x80) != 0);
		seq = ((m[0] << 0) & 0x7f);
		m++;
		hlen--;
	} else {
		/* extended form */
		eot = ((m[0] & 0x80) != 0);
		seq = ((m[0] << 24) & 0x7f) | (m[1] << 16) | (m[2] << 8) | m[3];
		m += 4;
		hlen -= 4;
	}

	switch (tp->pstate) {
	case _TP_STATE_REFWAIT:
	case _TP_STATE_CLOSED:
	default:
		freemsg(mp);
		return (0);
	case _TP_STATE_OPEN:
	case _TP_STATE_AKWAIT:
		tp_timer_stop(tp, t2);
		tp_timer_start(tp, q, t2);
		tp->pstate = _TP_STATE_OPEN;
		qenable(tp->rq);
		/* FIXME: perform data transfer procedure */
		break;
	case _TP_STATE_CLOSING:
		freemsg(mp);
		return tp_send_dr(tp, q, NULL);	/* optional */
	}
	return (0);

      badpdu:
	LOGERR(tp, "bad DT-TPDU");
      outstate:
	/* ignore out of state messages */
	freemsg(mp);
	return (0);
}

static int
tp_recv_rj(struct tp *tp, queue_t *q, mblk_t *mp)
{
#if 0
	/* FIXME: caller must do this */
	if (tp->options.res.tco.tco_checksum == T_YES) {
		if (!tp_tst_checksum(mp)) {
			freemsg(mp);
			return (0);
		}
	}
#endif
	return (0);
}

/**
 * tp_recv_ak - a AK is received
 * @tp: private structure (locked)
 * @q: active queue (read queue)
 * @mp: the AK message
 *
 * In state _TP_STATE_REFWAIT: stay in _TP_STATE_REFWAIT.
 *
 * In state _TP_STATE_CLOSED: stay in _TP_STATE_CLOSED.
 *
 * In state _TP_STATE_OPEN: _TP_ACTION_8 (stop inactivity timer if running),
 * _TP_ACTION_7 (set inactivity timer), stay in _TP_STATE_OPEN, _TP_NOTE_16 (see
 * data transfer procedures).
 *
 * In state _TP_STATE_AKWAIT: _TP_ACTION_7 (set inactivity timer), move to
 * _TP_STATE_OPEN, _TP_NOTE_15 (previously stored T-DATA or T-EXPEDITED-DATA
 * requests are ready for processing according to data transfer procedures),
 * _TP_NOTE_16 (see data transfer procedures).
 *
 * In state _TP_STATE_CLOSING: stay in _TP_STATE_CLOSING, _TP_NOTE_13 (the DR-TPDU
 * may be either repeated immediately or when T1 will run out).
 */
static int
tp_recv_ak(struct tp *tp, queue_t *q, mblk_t *mp)
{
#if 0
	/* FIXME: caller must do this */
	if (tp->options.res.tco.tco_checksum == T_YES) {
		if (!tp_tst_checksum(mp)) {
			freemsg(mp);
			return (0);
		}
	}
#endif
	return (0);
}

/**
 * tp_recv_ed - a ED is received
 * @tp: private structure (locked)
 * @q: active queue (read queue)
 * @mp: the ED message
 *
 * In state _TP_STATE_REFWAIT: stay in _TP_STATE_REFWAIT.
 *
 * In state _TP_STATE_CLOSED: stay in _TP_STATE_CLOSED.
 *
 * In state _TP_STATE_OPEN: _TP_ACTION_8 (stop inactivity timer if running),
 * _TP_ACTION_7 (set inactivity timer), stay in _TP_STATE_OPEN, _TP_NOTE_16 (see
 * data transfer procedures).
 *
 * In state _TP_STATE_AKWAIT: _TP_ACTION_7 (set inactivity timer), move to
 * _TP_STATE_OPEN, _TP_NOTE_15 (previously stored T-DATA or T-EXPEDITED-DATA
 * requests are ready for processing according to data transfer procedures),
 * _TP_NOTE_16 (see data transfer procedures).
 *
 * In state _TP_STATE_CLOSING: stay in _TP_STATE_CLOSING, _TP_NOTE_13 (the DR-TPDU
 * may be either repeated immediately or when T1 will run out).
 */
static int
tp_recv_ed(struct tp *tp, queue_t *q, mblk_t *mp)
{
#if 0
	/* FIXME: caller must do this */
	if (tp->options.res.tco.tco_checksum == T_YES) {
		if (!tp_tst_checksum(mp)) {
			freemsg(mp);
			return (0);
		}
	}
#endif
	return (0);
}

/**
 * tp_recv_ud - a UD is received
 * @tp: private structure (locked)
 * @q: active queue (read queue)
 * @mp: the UD message (must be pulled up)
 *
 * Generate T-UNITDATA indication.
 */
static int
tp_recv_ud(struct tp *tp, queue_t *q, mblk_t *mp, struct tsapaddr *saddr)
{
	register unsigned char *m;
	int hlen, plen, mlen, dlen;
	struct netbuf src, opt;

#if 0
	/* FIXME: caller must do this */
	if (tp->options.res.tcl.tcl_checksum == T_YES) {
		if (!tp_tst_checksum(mp)) {
			freemsg(mp);
			return (0);
		}
	}
#endif
	src.buf = (typeof(src.buf)) saddr;
	src.len = saddr->nsap.len + 2;

	opt.buf = NULL;
	opt.len = 0;		/* FIXME: put checksum in here eventually */

	if (unlikely(tp_get_state(tp) != TS_IDLE))
		goto outstate;

	mlen = msgsize(mp);
	m = mp->b_rptr;
	if (unlikely(mlen < 1))
		goto badpdu;
	hlen = *m++;
	dlen = mlen - hlen;
	hlen--;
	if (unlikely(dlen < 0))
		goto badpdu;
	if (unlikely(mlen < 2))
		goto badpdu;
	if (*m++ != _TP_MT_UD)
		goto badpdu;
	hlen--;
	while (hlen > 0) {
		switch (*m++) {
		case _TP_PT_CDTRANSSEL:
			hlen--;
			plen = *m++;
			hlen--;
			if (unlikely(plen > hlen))
				goto badpdu;
			m += plen;
			hlen -= plen;
			break;
		case _TP_PT_CGTRANSSEL:
			hlen--;
			plen = *m++;
			hlen--;
			saddr->tsel[0] = m[0];
			saddr->tsel[1] = m[1];
			if (unlikely(plen > hlen))
				goto badpdu;
			m += plen;
			hlen -= plen;
			break;
		case _TP_PT_CHECKSUM:
			hlen--;
			plen = *m++;
			hlen--;
			if (unlikely(plen > hlen))
				goto badpdu;
			m += plen;
			hlen -= plen;
			break;
		default:
			goto badpdu;
		}
	}
	return t_unitdata_ind(tp, q, &src, &opt, mp);
      badpdu:
	LOGERR(tp, "bad UD-TPDU");
      outstate:
	/* ignore out of state messages */
	freemsg(mp);
	return (0);
#if 0
      enobufs:
	return (-ENOBUFS);
      ebusy:
	return (-EBUSY);
#endif
}

/**
 * t_read - read data message from below
 * @tp: private structure (locked)
 * @q: active queue (read queue)
 * @mp: the data message
 *
 * Messages from below consist of the entire frame that was received from the
 * underlying network provider.  For subnetwork (llc) frames this is the entire
 * ethernet frame.  For ISO-IP and ISO-UDP this is the entire frame without the
 * layer 2 header (IP and possibly UDP).  Frames have already been assigned to the
 * appropriate stream by lower level discriminator and lookup functions.
 *
 * Note that DB_BASE(mp) points to the network header.  For llc, this is the L2
 * header including ethernet addresses.  For ISO-IP and ISO-UDP, this is the IP
 * header.  mp->b_rptr points to the transport header (TPDU).  Frames must be
 * descriminated again by TPDU message type and fed to the appropriate message
 * receive functions.
 */
int
t_read(struct tp *tp, queue_t *q, mblk_t *mp)
{
	switch (mp->b_rptr[1] & 0xf0) {
	case _TP_MT_ED:
		return tp_recv_ed(tp, q, mp);
	case _TP_MT_EA:
		return tp_recv_ea(tp, q, mp);
	case _TP_MT_UD:
		return tp_recv_ud(tp, q, mp, &tp->dst);
	case _TP_MT_RJ:
		return tp_recv_rj(tp, q, mp);
	case _TP_MT_AK:
		return tp_recv_ak(tp, q, mp);
	case _TP_MT_ER:
		return tp_recv_er(tp, q, mp);
	case _TP_MT_DR:
		return tp_recv_dr(tp, q, mp);
	case _TP_MT_DC:
		return tp_recv_dc(tp, q, mp);
	case _TP_MT_CC:
		return tp_recv_cc(tp, q, mp);
	case _TP_MT_CR:
		return tp_recv_cr(tp, q, mp);
	case _TP_MT_DT:
		return tp_recv_dt(tp, q, mp);
	default:
		freemsg(mp);
		return (0);
	}
}

/*
 *  =========================================================================
 *
 *  IP T-User --> T-Provider Primitives (Request and Response)
 *
 *  =========================================================================
 */

/**
 * t_conn_req: - process T_CONN_REQ primitive
 * @tp: private structure (locked)
 * @q: active queue
 * @mp: the T_CONN_REQ primtive
 *
 * If in state _TP_STATE_CLOSED:
 * If _TP_PREDICATE_P0 (T-CONNECT request acceptable) _TP_ACTION_1 (count = count +
 *	1), _TP_ACTION_3 (set retransmission timer), _TP_ACTION_10 (set initial
 *	credit for controlling reception according to the sent CR/CC-TPDU), send
 *	CR-TPDU and move to state _TP_STATE_WFCC.
 * If not _TP_PREDICATE_P0 (T-CONNECT request is acceptable), issue T-DISCONNECT
 *	indication and move to state _TP_STATE_CLOSED.
 */
static fastcall int
t_conn_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	const struct T_conn_req *p = (typeof(p)) mp->b_rptr;
	unsigned char *dst;
	long dst_len;
	int err;

	if (unlikely(tp->p.info.SERV_type == T_CLTS))
		goto notsupport;
	if (unlikely(MBLKL(mp) < sizeof(*p)))
		goto einval;
	if (unlikely(tp_get_state(tp) != TS_IDLE))
		goto outstate;
	if (p->DEST_length && MBLKL(mp) < p->DEST_offset + p->DEST_length)
		goto einval;
	if (p->OPT_length && MBLKL(mp) < p->OPT_offset + p->OPT_length)
		goto einval;
	/* Note: in general, it is not necessary to have a destination address when N_CONS is being used and
	   the number of network connections is only one. */
	if (unlikely(p->DEST_length < 1))
		goto badaddr;

	dst = (typeof(dst)) (mp->b_rptr + p->DEST_offset);
	dst_len = p->DEST_length;

	if (unlikely(!(dst_len = tp_addr_size(dst, dst_len))))
		goto badaddr;
	switch (__builtin_expect(tp->p.prot.protocol, T_ISO_TP)) {
	case T_ISO_TP:
		/* For now we need a local NSAP address that identifies a MAC address or an IETF NSAP address 
		   that identifies an IPv4 address.  We cannot use ISO-UDP from this minor device. */
		break;
	case T_INET_IP:
	case T_INET_UDP:
		/* For now we need an IETF NSAP address that identifies an IPv4 address */
		break;
	default:
		LOGNO(tp, "address format incorrect %u", tp->p.prot.family);
		goto badaddr;
	}
	if (unlikely((err = t_parse_conn_opts(tp, mp->b_rptr + p->OPT_offset, p->OPT_length, 1)) < 0)) {
		switch (-err) {
		case EINVAL:
			goto badopt;
		case EACCES:
			goto acces;
		default:
			goto error;
		}
	}
	// bcopy(tp->dst.addr, dst, dst_len);
	// tp->dst.len = dst_len;
	if (unlikely(! !mp->b_cont)) {
		long mlen, mmax;

		mlen = msgdsize(mp->b_cont);
		if (mlen > 0) {
			/* User data not available in Class 0 */
			if (tp->options.res.tco.tco_prefclass == T_CLASS0)
				goto baddata;
			/* Up to 32 octets in other classes */
			if (mlen > 32)
				goto baddata;
			/* NOTE 3 - If class 0 is a valid response according to Table 3, inclusion of user
			   data in the CR-TPDU may cause the responding entity to refuse the connection (for
			   example, if it only supports class 0). */
		}
		if (((mmax = tp->p.info.CDATA_size) > 0 && mlen > mmax) || mmax == T_INVALID
		    || ((mmax = tp->p.info.TIDU_size) > 0 && mlen > mmax) || mmax == T_INVALID)
			goto baddata;
		/* FIXME need to generate connect with data */
	}
	tp_set_state(tp, TS_WACK_CREQ);
	return t_ok_ack(tp, q, mp, T_CONN_REQ, NULL, NULL, NULL);

      baddata:
	err = TBADDATA;
	LOGNO(tp, "ERROR: bad connection data");
	goto error;
      badopt:
	err = TBADOPT;
	LOGNO(tp, "ERROR: bad options");
	goto error;
      acces:
	err = TACCES;
	LOGNO(tp, "ERROR: no permission for address or option");
	goto error;
      badaddr:
	err = TBADADDR;
	LOGNO(tp, "ERROR: address is unusable");
	goto error;
      einval:
	err = -EINVAL;
	LOGNO(tp, "ERROR: invalid message format");
	goto error;
      outstate:
	err = TOUTSTATE;
	LOGNO(tp, "ERROR: would place i/f out of state");
	goto error;
      notsupport:
	err = TNOTSUPPORT;
	LOGNO(tp, "ERROR: primitive not supported for T_CLTS");
	goto error;
      error:
	return t_error_ack(tp, q, mp, T_CONN_REQ, err);
}

/**
 * t_seq_check: - find a connection indication by sequence number
 * @tp: private structure (locked)
 * @seq: the sequence number to find
 *
 * Takes a sequence number and matches it to a connetion indication.  Note that it is not necessary
 * to separately lock the connection indication queue because it is only accessed with the private
 * structure locked.
 */
static mblk_t *
t_seq_check(struct tp *tp, t_uscalar_t seq)
{
	mblk_t *cp;

	for (cp = bufq_head(&tp->conq); cp && ((struct tp_conind *) cp->b_rptr)->ci_seq != seq;
	     cp = cp->b_next) ;
	usual(cp);
	return (cp);
}

/**
 * t_tok_check: - find an accepting stream by acceptor id
 * @tp: private structure (locked)
 * @q: active queue
 * @tok: the acceptor id
 * @errp: error return pointer
 *
 * Finds the accepting stream that matches the acceptor id and returns NULL if no match is found.
 * If a match is found and it is different than the stream on which the connection response was
 * received, the accepting stream is locked.  If the accepting stream cannot be locked, NULL is
 * returned and -EDEADLK is returned in the error pointer.
 */
static fastcall struct tp *
t_tok_check(struct tp *tp, queue_t *q, t_uscalar_t tok, int *errp)
{
	struct tp *as;

	read_lock(&tp_lock);
	for (as = (struct tp *) mi_first_ptr(&tp_opens); as && (t_uscalar_t) (long) as->rq != tok;
	     as = (struct tp *) mi_next_ptr((caddr_t) as)) ;
	if (as && as != tp)
		if (!mi_acquire((caddr_t) as, q))
			*errp = -EDEADLK;
	read_unlock(&tp_lock);
	usual(as);
	return (as);
}

/**
 * t_conn_res: - process a T_CONN_RES primitive
 * @tp: private structure (locked) of listening Stream
 * @q: active queue
 * @mp: the T_CONN_RES primitive
 *
 * Note that when the accepting Stream is different from the listening Stream, it is necessary to
 * acquire a lock also on the accepting Stream.  It is necessary to release this lock after
 * t_ok_ack() but may be released before t_error_ack().
 *
 * If in state _TP_STATE_WFTRESP:
 * _TP_ACTION_3 (set retransmission timer), _TP_ACTION_2 (count = 0), _TP_ACITON_1
 *	(count = count + 1), _TP_ACTION_10 (set initial credit for sending
 *	according to the received CR/CC-TPDU), Send CC-TPDU, move to state
 *	_TP_STATE_ACKWAIT.
 */
static fastcall int
t_conn_res(struct tp *tp, queue_t *q, mblk_t *mp)
{
	int err = 0;
	mblk_t *cp;
	struct tp *ap;
	const struct T_conn_res *p = (typeof(p)) mp->b_rptr;
	unsigned char *opt;

	if (unlikely(tp->p.info.SERV_type == T_CLTS))
		goto notsupport;
	if (unlikely(MBLKL(mp) < sizeof(*p)))
		goto inval;
	if (unlikely(tp_get_state(tp) != TS_WRES_CIND))
		goto outstate;
	tp_set_state(tp, TS_WACK_CRES);
	if (unlikely(!(cp = t_seq_check(tp, p->SEQ_number))))
		goto badseq;
	if (unlikely(!(ap = t_tok_check(tp, q, p->ACCEPTOR_id, &err)) && err))
		goto edeadlk;
	if (unlikely(!ap || !(ap == tp || ((1 << tp_get_state(ap)) & TSM_DISCONN))))
		goto badf;
	if (unlikely(tp->p.prot.type != ap->p.prot.type))
		goto provmismatch;
	if (unlikely(tp_get_state(ap) == TS_IDLE && ap->conind))
		goto resqlen;
	/* protect at least r00t streams from users */
#ifdef HAVE_KMEMB_STRUCT_CRED_UID_VAL
	if (unlikely(tp->cred.cr_uid.val != 0 && ap->cred.cr_uid.val == 0))
		goto acces;
#else
	if (unlikely(tp->cred.cr_uid != 0 && ap->cred.cr_uid == 0))
		goto acces;
#endif
	opt = mp->b_rptr + p->OPT_offset;
	if (unlikely(p->OPT_length && MBLKL(mp) < p->OPT_offset + p->OPT_length))
		goto badopt;
	if (unlikely(mp->b_cont != NULL)) {
		long mlen, mmax;

		mlen = msgdsize(mp->b_cont);
		if (((mmax = tp->p.info.CDATA_size) > 0 && mlen > mmax) || mmax == T_INVALID
		    || ((mmax = tp->p.info.TIDU_size) > 0 && mlen > mmax) || mmax == T_INVALID)
			goto baddata;
		/* FIXME need to generate connect with data */
	}
	if (unlikely((err = t_parse_conn_opts(ap, opt, p->OPT_length, 0)) < 0)) {
		switch (-err) {
		case EINVAL:
			goto badopt;
		case EACCES:
			goto acces;
		default:
			goto unlock_error;
		}
	}
	/* FIXME: options will be processed on wrong socket!!! When we accept, we will delete this socket and 
	   create another one that was derived from the listening socket.  We need to process the options
	   against that socket, not the placeholder.  One way to fix this is to set flags when we process
	   options against the stream structure and then reprocess those options once the sockets have been
	   swapped.  See t_ok_ack for details. */
	/* FIXME: The accepting socket does not have to be in the bound state. The socket will be autobound
	   to the correct address already. */
	err = t_ok_ack(tp, q, mp, T_CONN_RES, cp, NULL, ap);
	if (ap && ap != tp) {
		read_lock(&tp_lock);
		mi_release((caddr_t) ap);
		read_unlock(&tp_lock);
	}
	return (err);
      baddata:
	err = TBADDATA;
	LOGNO(tp, "ERROR: bad connection data");
	goto unlock_error;
      badopt:
	err = TBADOPT;
	LOGNO(tp, "ERROR: bad options");
	goto unlock_error;
      acces:
	err = TACCES;
	LOGNO(tp, "ERROR: no access to accepting queue");
	goto unlock_error;
      resqlen:
	err = TRESQLEN;
	LOGNO(tp, "ERROR: accepting queue is listening");
	goto unlock_error;
      provmismatch:
	err = TPROVMISMATCH;
	LOGNO(tp, "ERROR: not same transport provider");
	goto unlock_error;
      badf:
	err = TBADF;
	LOGNO(tp, "ERROR: accepting queue id is invalid");
	goto unlock_error;
      edeadlk:
	err = -EDEADLK;
	LOGNO(tp, "ERROR: cannot lock accepting stream");
	goto error;
      badseq:
	err = TBADSEQ;
	LOGNO(tp, "ERROR: sequence number is invalid");
	goto error;
      inval:
	err = -EINVAL;
	LOGNO(tp, "ERROR: invalid primitive format");
	goto error;
      outstate:
	err = TOUTSTATE;
	LOGNO(tp, "ERROR: would place i/f out of state");
	goto error;
      notsupport:
	err = TNOTSUPPORT;
	LOGNO(tp, "ERROR: primitive not supported for T_CLTS");
	goto error;
      unlock_error:
	if (ap && ap != tp) {
		read_lock(&tp_lock);
		mi_release((caddr_t) ap);
		read_unlock(&tp_lock);
	}
	goto error;
      error:
	return t_error_ack(tp, q, mp, T_CONN_RES, err);
}

/*
 *  T_DISCON_REQ         2 - TC disconnection request
 *  -------------------------------------------------------------------
 *
 *  In state _TP_STATE_WFCC:
 *  If _TP_PREDICATE_P3 (local choke), move to state _TP_STATE_WBCL; otherwise,
 *	_TP_ACTION_4 (stop retransmission timer if running), _TP_ACTION_3 (set
 *	retransmission timer), _TP_ACTION_2 (count = 0), _TP_ACTION_1 (count =
 *	count + 1), _TP_ACTION_15 (send DR-TPDU; this DR-TPDU is sent with src-ref
 *	= local-ref and dst-ref = remote-ref (may be zero)), move to state
 *	_TP_STATE_CLOSING.
 *
 *  In state _TP_STATE_OPEN: _TP_ACTION_6 (stop window timer if running),
 *	_TP_ACTION_8 (stop inactivity timer if running), _TP_ACTION_4 (stop
 *	retransmission timer if running), _TP_ACTION_3 (set retransmission timer),
 *	_TP_ACTION_2 (count = 0), _TP_ACTION_1 (count = count + 1), _TP_ACTION_15
 *	(send the DR-TPDU; this DR-TPDU is sent with src-ref = local-ref and
 *	dst-ref = remote-ref (may be zero)), move to state _TP_STATE_CLOSING.
 *
 *  In state _TP_STATE_WFTRESP: _TP_ACTION_16 (send the DR-TPDU; the DR-TPDU is
 *	sent with src-ref = 0 and dst-ref = remote-ref), move to state
 *	_TP_STATE_CLOSED.
 *
 *  In state _TP_STATE_AKWAIT: _TP_ACTION_4 (stop retransmission timer if running),
 *	_TP_ACTION_3 (set retransmission timer), _TP_ACTION_2 (count = 0),
 *	_TP_ACTION_1 (count = count + 1), _TP_ACTION_15 (send the DR-TPDU, this
 *	DR-TPDU is sent with src-ref = local-ref and dst-ref = remote-ref (may be
 *	zero)), move to state _TP_STATE_CLOSING.
 *
 */
static fastcall int
t_discon_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	int err;
	mblk_t *cp = NULL;
	struct T_discon_req *p = (typeof(p)) mp->b_rptr;

	if (unlikely(tp->p.info.SERV_type == T_CLTS))
		goto notsupport;
	if (unlikely(MBLKL(mp) < sizeof(*p)))
		goto einval;
	if (unlikely((1 << tp_get_state(tp)) & ~TSM_CONNECTED))
		goto outstate;
	if (unlikely(! !mp->b_cont)) {
		long mlen, mmax;

		mlen = msgdsize(mp->b_cont);
		if (((mmax = tp->p.info.DDATA_size) > 0 && mlen > mmax) || mmax == T_INVALID
		    || ((mmax = tp->p.info.TIDU_size) > 0 && mlen > mmax) || mmax == T_INVALID)
			goto baddata;
		/* FIXME need to generate disconnect with data */
	}
	switch (__builtin_expect(tp_get_state(tp), TS_DATA_XFER)) {
	case TS_WCON_CREQ:
		tp_set_state(tp, TS_WACK_DREQ6);
		break;
	case TS_WRES_CIND:
		if ((cp = t_seq_check(tp, p->SEQ_number))) {
			tp_set_state(tp, TS_WACK_DREQ7);
			break;
		}
		goto badseq;
	case TS_DATA_XFER:
		tp_set_state(tp, TS_WACK_DREQ9);
		break;
	case TS_WIND_ORDREL:
		tp_set_state(tp, TS_WACK_DREQ10);
		break;
	case TS_WREQ_ORDREL:
		tp_set_state(tp, TS_WACK_DREQ11);
		break;
	}
	return t_ok_ack(tp, q, mp, T_DISCON_REQ, cp, NULL, NULL);

      badseq:
	err = TBADSEQ;
	LOGNO(tp, "ERROR: sequence number is invalid");
	goto error;
      baddata:
	err = TBADDATA;
	LOGNO(tp, "ERROR: bad disconnection data");
	goto error;
      einval:
	err = -EINVAL;
	LOGNO(tp, "ERROR: invalid primitive format");
	goto error;
      outstate:
	err = TOUTSTATE;
	LOGNO(tp, "ERROR: would place i/f out of state");
	goto error;
      notsupport:
	err = TNOTSUPPORT;
	LOGNO(tp, "ERROR: primitive not supported for T_CLTS");
	goto error;
      error:
	return t_error_ack(tp, q, mp, T_DISCON_REQ, err);
}

/*
 *  M_DATA
 *  -------------------------------------------------------------------
 */
static inline fastcall __hot_write int
t_write(struct tp *tp, queue_t *q, mblk_t *mp)
{
	long mlen, mmax;

	if (unlikely(tp->p.info.SERV_type == T_CLTS))
		goto notsupport;
	if (unlikely(tp_get_state(tp) == TS_IDLE))
		goto discard;
	if (unlikely((1 << tp_get_state(tp)) & ~TSM_OUTDATA))
		goto outstate;
	mlen = msgdsize(mp);
	if (((mmax = tp->p.info.TSDU_size) > 0 && mlen > mmax) || mmax == T_INVALID
	    || ((mmax = tp->p.info.TIDU_size) > 0 && mlen > mmax) || mmax == T_INVALID)
		goto emsgsize;

	return tp_send_dt(tp, q, mp, 0);

      emsgsize:
	LOGNO(tp, "ERROR: message too large %ld > %ld", mlen, mmax);
	goto error;
      outstate:
	LOGNO(tp, "ERROR: would place i/f out of staten");
	goto error;
      discard:
	freemsg(mp);
	LOGNO(tp, "ERROR: ignore in idle state");
	return (0);
      notsupport:
	LOGNO(tp, "ERROR: primitive not supported for T_CLTS");
	goto error;
      error:
	return m_error(tp, q, mp, -EPROTO);
}

/*
 *  T_DATA_REQ           3 - Connection-Mode data transfer request
 *  -------------------------------------------------------------------
 *
 *  In state _TP_STATE_OPEN: _TP_NOTE_16 (see data transfer procedures), stay in the _TP_STATE_OPEN
 *  state.
 *
 *  In state _TP_AKWAIT: _TP_ACTION_20 (store reqeust and exercise flow control to the user), stay
 *  in the _TP_STATE_AKWAIT state.
 */
static inline fastcall __hot_out int
t_data_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	long mlen, mmax;
	const struct T_data_req *p = (typeof(p)) mp->b_rptr;

	if (unlikely(tp->p.info.SERV_type == T_CLTS))
		goto notsupport;
	if (unlikely(MBLKL(mp) < sizeof(*p)))
		goto einval;
	if (unlikely(tp_get_state(tp) == TS_IDLE))
		goto discard;
	if (unlikely((1 << tp_get_state(tp)) & ~TSM_OUTDATA))
		goto outstate;
	if (!(mlen = msgdsize(mp)) && !(tp->p.info.PROVIDER_flag & T_SNDZERO))
		goto baddata;
	if (((mmax = tp->p.info.TSDU_size) > 0 && mlen > mmax) || mmax == T_INVALID
	    || ((mmax = tp->p.info.TIDU_size) > 0 && mlen > mmax) || mmax == T_INVALID)
		goto emsgsize;

	return tp_send_dt(tp, q, mp->b_cont, p->MORE_flag & T_MORE);

      emsgsize:
	LOGNO(tp, "ERROR: message too large %ld > %ld", mlen, mmax);
	goto error;
      baddata:
	LOGNO(tp, "ERROR: zero length data");
	goto error;
      outstate:
	LOGNO(tp, "ERROR: would place i/f out of state");
	goto error;
      einval:
	LOGNO(tp, "ERROR: invalid primitive format");
	goto error;
      discard:
	LOGNO(tp, "ERROR: ignore in idle state");
	freemsg(mp);
	return (0);
      notsupport:
	LOGNO(tp, "ERROR: primitive not supported for T_CLTS");
	goto error;
      error:
	return m_error(tp, q, mp, -EPROTO);
}

/*
 *  T_EXDATA_REQ         4 - Expedited data request
 *  -------------------------------------------------------------------
 *
 *  In state _TP_STATE_OPEN: _TP_NOTE_16 (see data transfer procedures), stay in the _TP_STATE_OPEN
 *  state.
 *
 *  In state _TP_AKWAIT: _TP_ACTION_20 (store reqeust and exercise flow control to the user), stay
 *  in the _TP_STATE_AKWAIT state.
 */
static inline fastcall __hot_put int
t_exdata_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	long mlen, mmax;
	const struct T_exdata_req *p = (typeof(p)) mp->b_rptr;

	if (unlikely(tp->p.info.SERV_type == T_CLTS))
		goto notsupport;
	if (unlikely(MBLKL(mp) < sizeof(*p)))
		goto einval;
	if (unlikely(tp_get_state(tp) == TS_IDLE))
		goto discard;
	if (unlikely((1 << tp_get_state(tp)) & ~TSM_OUTDATA))
		goto outstate;
	if (!(mlen = msgdsize(mp)) && !(tp->p.info.PROVIDER_flag & T_SNDZERO))
		goto baddata;
	if (((mmax = tp->p.info.ETSDU_size) > 0 && mlen > mmax) || mmax == T_INVALID
	    || ((mmax = tp->p.info.TIDU_size) > 0 && mlen > mmax) || mmax == T_INVALID)
		goto emsgsize;

	return tp_send_ed(tp, q, mp->b_cont, p->MORE_flag & T_MORE);

      emsgsize:
	LOGNO(tp, "ERROR: message too large %ld > %ld", mlen, mmax);
	goto error;
      baddata:
	LOGNO(tp, "ERROR: zero length data");
	goto error;
      outstate:
	LOGNO(tp, "ERROR: would place i/f out of state");
	goto error;
      einval:
	LOGNO(tp, "ERROR: invalid primitive format");
	goto error;
      discard:
	LOGNO(tp, "ERROR: ignore in idle state");
	freemsg(mp);
	return (0);
      notsupport:
	LOGNO(tp, "ERROR: primitive not supported for T_CLTS");
	goto error;
      error:
	return m_error(tp, q, mp, -EPROTO);
}

/*
 *  T_INFO_REQ           5 - Information Request
 *  -------------------------------------------------------------------
 */
static fastcall int
t_info_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	return t_info_ack(tp, q, mp);
}

/*
 *  T_BIND_REQ           6 - Bind a TS user to a transport address
 *  -------------------------------------------------------------------
 */
static fastcall int
t_bind_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	int err;
	const struct T_bind_req *p = (typeof(p)) mp->b_rptr;

	if (unlikely(MBLKL(mp) < sizeof(*p)))
		goto einval;
	if (unlikely(tp_get_state(tp) != TS_UNBND))
		goto outstate;
	tp_set_state(tp, TS_WACK_BREQ);
#if 0
	/* we are supposed to ignore CONIND_number for T_CLTS according to XTI */
	if (tp->p.info.SERV_type == T_CLTS && p->CONIND_number)
		goto notsupport;
#endif
	if (p->ADDR_length && (MBLKL(mp) < p->ADDR_offset + p->ADDR_length)) {
		LOGNO(tp, "ADDR_offset(%u) or ADDR_length(%u) are incorrect", p->ADDR_offset, p->ADDR_length);
		goto badaddr;
	}
	if (p->ADDR_length < 1 || p->ADDR_length > 22)
		goto badaddr;
	bcopy(mp->b_rptr + p->ADDR_offset, tp->src.tsel, p->ADDR_length);
	tp->src.nsap.len = p->ADDR_length - 2;
	if ((err = tp_addr_parse(&tp->src.nsap, &tp->loc)) < 0)
		goto badaddr;
	switch (__builtin_expect(tp->p.prot.protocol, 0)) {
#if 0
	case 0:		/* not determined yet */
		/* For now we need a local NSAP address that identifies a MAC address or an IETF NSAP address 
		   that identifies an IPv4 address.  We cannot use ISO-UDP from this minor device. */
		break;
#endif
	case T_ISO_TP:
		/* For now we need a local NSAP address that identifies a MAC address */
		switch (tp->loc.type) {
		case TP_SNPA_TYPE_MAC:
		case TP_SNPA_TYPE_IFNAME:
			break;
		default:
			goto noaddr;
		}
		break;
	case T_INET_IP:
		/* For now we need an IETF NSAP address that identifies an IPv4 address */
		switch (tp->loc.type) {
		case TP_SNPA_TYPE_IP4:
		case TP_SNPA_TYPE_IP4_PORT:
			break;
		default:
			goto noaddr;
		}
		break;
	case T_INET_UDP:
		/* For now we need an IETF NSAP address that identifies an IPv4 address */
		switch (tp->loc.type) {
		case TP_SNPA_TYPE_IP4:
		case TP_SNPA_TYPE_IP4_PORT:
			break;
		default:
			goto noaddr;
		}
		break;
	default:
		LOGNO(tp, "address format incorrect %u", tp->p.prot.family);
		goto badaddr;
	}
	if (unlikely((err = tp_bind(tp, q, &tp->src, &tp->loc))))
		goto error;
	tp->conind = 0;
	if (p->CONIND_number && tp->p.info.SERV_type == T_CLTS)
		if ((err = tp_listen(tp, q, p->CONIND_number)))
			goto error;
	return t_bind_ack(tp, q, mp, (unsigned char *) &tp->src,
			  (t_uscalar_t) (sizeof(tp->src.tsel) + tp->src.nsap.len), p->CONIND_number);

#if 0
      acces:
	err = TACCES;
	LOGNO(tp, "ERROR: no permission for address");
	goto error;
#endif
      noaddr:
	err = TNOADDR;
	LOGNO(tp, "ERROR: address could not be assigned");
	goto error;
      badaddr:
	err = TBADADDR;
	LOGNO(tp, "ERROR: address is invalid");
	goto error;
      einval:
	err = -EINVAL;
	LOGNO(tp, "ERROR: invalid primitive format");
	goto error;
      outstate:
	err = TOUTSTATE;
	LOGNO(tp, "ERROR: would place i/f out of state");
	goto error;
#if 0
      notsupport:
	err = TNOTSUPPORT;
	LOGNO(tp, "ERROR: primitive not supported for T_CLTS");
	goto error;
#endif
      error:
	return t_error_ack(tp, q, mp, T_BIND_REQ, err);
}

/**
 * t_unbind_req: - process a T_UNBIND_REQ primitive
 * @tp: private structure (locked)
 * @q: active queue
 * @mp: the T_UNBIND_REQ primitive
 *
 * This procedure unbinds a TS user from a transport address.
 */
static fastcall int
t_unbind_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	if (unlikely(tp_get_state(tp) != TS_IDLE))
		goto outstate;

	tp_set_state(tp, TS_WACK_UREQ);
	return t_ok_ack(tp, q, mp, T_UNBIND_REQ, NULL, NULL, NULL);

      outstate:
	LOGNO(tp, "ERROR: would place i/f out of state");
	return t_error_ack(tp, q, mp, T_UNBIND_REQ, TOUTSTATE);
}

/*
 *  T_UNITDATA_REQ       8 -Unitdata Request 
 *  -------------------------------------------------------------------
 *  The source and destination address parameters of the T-UNITDATA request service primitive are
 *  used to determine the source network address, source TSAP-ID, destination network address, and
 *  destination TSAP-ID.
 *
 *  The quality of tservice parameter in the T-UNITDATA request is used to determine if a checksum
 *  should be included in the unit data UD TPDU.
 *
 *  NOTE -- If the length of the TSDU given in the T-UNITDATA request, plus the PCI of the UD TPDU
 *  exceeds the maximum NSDU size supported by the network service, then the TSDU is discarded and a
 *  local report may be made to the TS-user indicating the inability of the Transport Layer to
 *  provide the service requested.
 *
 *  A UD TPDU is constructed with a checksum parameter (if necessary), a source TSAP-ID, a
 *  desitnation TSAP-ID, and the user data field from the T-UNITDATA request.
 *
 *  An N-UNITDATA request service primitive is issued with the source and destination network
 *  addresses determined above, the quality of service requested and a user field contianing the UD
 *  TPDU.
 *
 *  Each TPDU is transmitted by the use of the connectionless-mode network service over a
 *  pre-existing association between a pair of NSAPs.  The association is considered by transport
 *  entities as permanently established and available.
 *
 *  There is no indication given to transport entities about the ability of the network entity to
 *  fulfull the service requirements given in the N-UNITDATA primitive.  However, it can be a local
 *  matter to make transport entities aware of the availability and caharacteristics (QOS) of
 *  connectionless-mode network services, as the corresponding NSAP associations exist logically  by
 *  the nature of the connectionless-mode network service and may be recognized by network entities.
 */
static inline fastcall __hot_out int
t_unitdata_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	const struct T_unitdata_req *p = (typeof(p)) mp->b_rptr;
	size_t dlen = mp->b_cont ? msgdsize(mp->b_cont) : 0;
	long mmax;
	int err;

	if (unlikely(tp->p.info.SERV_type != T_CLTS))
		goto notsupport;
	if (unlikely(MBLKL(mp) < sizeof(*p)))
		goto einval;
	if (unlikely(dlen == 0 && !(tp->p.info.PROVIDER_flag & T_SNDZERO)))
		goto baddata;
	if (((mmax = tp->p.info.TSDU_size) > 0 && dlen > mmax) || mmax == T_INVALID
	    || ((mmax = tp->p.info.TIDU_size) > 0 && dlen > mmax) || mmax == T_INVALID)
		goto baddata;
	if (unlikely(tp_get_state(tp) != TS_IDLE))
		goto outstate;
	/* FIXME: check address better */
	if ((MBLKL(mp) < p->DEST_offset + p->DEST_length)
	    || (p->DEST_length < 1)
	    || (p->DEST_length < tp_addr_size(mp->b_rptr + p->DEST_offset, p->DEST_length)))
		goto badadd;
	if ((err = tp_send_ud(tp, q, mp->b_cont, mp->b_rptr + p->DEST_offset, p->DEST_length))) {
		/* FIXME: analyze error */
		return (err);
	}
	/* FIXME: we can send uderr for some of these instead of erroring out the entire stream. */
	return (0);
#if 0
      badopt:
	LOGNO(tp, "ERROR: bad options");
	goto error;
      acces:
	LOGNO(tp, "ERROR: no permission to address or options");
	goto error;
#endif
      badadd:
	LOGNO(tp, "ERROR: bad destination address");
	goto error;
      einval:
	LOGNO(tp, "ERROR: invalid primitive");
	goto error;
      outstate:
	LOGNO(tp, "ERROR: would place i/f out of state");
	goto error;
      baddata:
	LOGNO(tp, "ERROR: bad data size");
	goto error;
      notsupport:
	LOGNO(tp, "ERROR: primitive not supported for T_COTS");
	goto error;
      error:
	return m_error(tp, q, mp, -EPROTO);
}

/**
 * t_optmgmt_req: - process T_OPTMGMT_REQ primitive
 * @tp: private structure (locked)
 * @q: active queue
 * @mp: the T_OPTMGMT_REQ message
 *
 * The T_OPTMGMT_REQ is responsible for establishing options while the stream is in the T_IDLE
 * state.  When the stream is bound to a local address using the T_BIND_REQ, the settings of options
 * with end-to-end significance will have an affect on how the driver response to an INIT with
 * INIT-ACK for SCTP.  For example, the bound list of addresses is the list of addresses that will
 * be sent in the INIT-ACK.  The number of inbound streams and outbound streams are the numbers that
 * will be used in the INIT-ACK.
 *
 * Returned Errors:
 *
 * TACCES:
 *	the user did not have proper permissions for the user of the requested options.
 * TBADFLAG:
 *	the flags as specified were incorrect or invalid.
 * TBADOPT:
 *	the options as specified were in an incorrect format, or they contained invalid information.
 * TOUTSTATE:
 *	the primitive would place the transport interface out of state.
 * TNOTSUPPORT:
 *	this prmitive is not supported.
 * TSYSERR:
 *	a system error has occured and the UNIX system error is indicated in the primitive.
 */
static fastcall int
t_optmgmt_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	int err, opt_len;
	const struct T_optmgmt_req *p = (typeof(p)) mp->b_rptr;

	if (unlikely(MBLKL(mp) < sizeof(*p)))
		goto einval;
#ifdef TS_WACK_OPTREQ
	if (unlikely(tp_get_state(tp) == TS_IDLE))
		tp_set_state(tp, TS_WACK_OPTREQ);
#endif
	if (unlikely(p->OPT_length && MBLKL(mp) < p->OPT_offset + p->OPT_length))
		goto badopt;
	switch (p->MGMT_flags) {
	case T_DEFAULT:
		opt_len = t_size_default_options(tp, mp->b_rptr + p->OPT_offset, p->OPT_length);
		break;
	case T_CURRENT:
		opt_len = t_size_current_options(tp, mp->b_rptr + p->OPT_offset, p->OPT_length);
		break;
	case T_CHECK:
		opt_len = t_size_check_options(tp, mp->b_rptr + p->OPT_offset, p->OPT_length);
		break;
	case T_NEGOTIATE:
		opt_len = t_size_negotiate_options(tp, mp->b_rptr + p->OPT_offset, p->OPT_length);
		break;
	default:
		goto badflag;
	}
	if (unlikely(opt_len < 0)) {
		switch (-(err = opt_len)) {
		case EINVAL:
			goto badopt;
		case EACCES:
			goto acces;
		default:
			goto provspec;
		}
	}
	err = t_optmgmt_ack(tp, q, mp, p->MGMT_flags, mp->b_rptr + p->OPT_offset, p->OPT_length, opt_len);
	if (unlikely(err < 0)) {
		switch (-err) {
		case EINVAL:
			goto badopt;
		case EACCES:
			goto acces;
		case ENOBUFS:
		case ENOMEM:
			break;
		default:
			goto provspec;
		}
	}
	return (err);

      provspec:
	err = err;
	LOGNO(tp, "ERROR: provider specific");
	goto error;
      badflag:
	err = TBADFLAG;
	LOGNO(tp, "ERROR: bad options flags");
	goto error;
      acces:
	err = TACCES;
	LOGNO(tp, "ERROR: no permission for option");
	goto error;
      badopt:
	err = TBADOPT;
	LOGNO(tp, "ERROR: bad options");
	goto error;
      einval:
	err = -EINVAL;
	LOGNO(tp, "ERROR: invalid primitive format");
	goto error;
      error:
	return t_error_ack(tp, q, mp, T_OPTMGMT_REQ, err);
}

/*
 *  T_ORDREL_REQ        10 - TS user is finished sending
 *  -------------------------------------------------------------------
 */
static fastcall int
t_ordrel_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	const struct T_ordrel_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(tp->p.info.SERV_type != T_COTS_ORD))
		goto notsupport;
	if (unlikely((1 << tp_get_state(tp)) & ~TSM_OUTDATA))
		goto outstate;
	if (unlikely(! !mp->b_cont)) {
		long mlen, mmax;

		mlen = msgdsize(mp->b_cont);
		if (((mmax = tp->p.info.DDATA_size) > 0 && mlen > mmax) || mmax == T_INVALID
		    || ((mmax = tp->p.info.TIDU_size) > 0 && mlen > mmax) || mmax == T_INVALID)
			goto baddata;
	}
	/* FIXME: generate the disconnect request */
	if ((err = tp_send_dr(tp, q, mp->b_cont))) {
		/* FIXME: analyze error response */
		return (err);
	}
	switch (__builtin_expect(tp_get_state(tp), TS_DATA_XFER)) {
	case TS_DATA_XFER:
		tp_set_state(tp, TS_WIND_ORDREL);
		break;
	case TS_WREQ_ORDREL:
		tp_set_state(tp, TS_IDLE);
		break;
	}
	freemsg(mp);
	return (0);

      baddata:
	LOGNO(tp, "ERROR: bad orderly release data");
	goto error;
      outstate:
	LOGNO(tp, "ERROR: would place i/f out of state");
	goto error;
      notsupport:
	LOGNO(tp, "ERROR: primitive not supported for T_CLTS or T_COTS");
	goto error;
      error:
	tp_set_state(tp, tp->oldstate);
	return m_error(tp, q, mp, -EPROTO);
}

/*
 *  T_OPTDATA_REQ       24 - Data with options request
 *  -------------------------------------------------------------------
 *
 *  In state _TP_STATE_OPEN: _TP_NOTE_16 (see data transfer procedures), stay in the _TP_STATE_OPEN
 *  state.
 *
 *  In state _TP_AKWAIT: _TP_ACTION_20 (store reqeust and exercise flow control to the user), stay
 *  in the _TP_STATE_AKWAIT state.
 */
static inline fastcall __hot_out int
t_optdata_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	long mlen, mmax;
	const struct T_optdata_req *p = (typeof(p)) mp->b_rptr;

	if (unlikely(tp->p.info.SERV_type == T_CLTS))
		goto notsupport;
	if (unlikely(MBLKL(mp) < sizeof(*p)))
		goto einval;
	if (unlikely(tp_get_state(tp) == TS_IDLE))
		goto discard;
	if (unlikely((1 << tp_get_state(tp)) & ~TSM_OUTDATA))
		goto outstate;
	if (!(mlen = msgdsize(mp)) && !(tp->p.info.PROVIDER_flag & T_SNDZERO))
		goto baddata;
	if (likely(!(p->DATA_flag & T_ODF_EX))) {
		if (((mmax = tp->p.info.TSDU_size) > 0 && mlen > mmax) || mmax == T_INVALID
		    || ((mmax = tp->p.info.TIDU_size) > 0 && mlen > mmax) || mmax == T_INVALID)
			goto emsgsize;
	} else {
		if (((mmax = tp->p.info.ETSDU_size) > 0 && mlen > mmax) || mmax == T_INVALID
		    || ((mmax = tp->p.info.TIDU_size) > 0 && mlen > mmax) || mmax == T_INVALID)
			goto emsgsize;
	}
	{
		if (likely(!(p->DATA_flag & T_ODF_EX)))
			return tp_send_dt(tp, q, mp->b_cont, p->DATA_flag & T_ODF_MORE);
		else
			return tp_send_ed(tp, q, mp->b_cont, p->DATA_flag & T_ODF_MORE);
	}
#if 0
      badopt:
	LOGNO(tp, "ERROR: bad options");
	goto error;
      acces:
	LOGNO(tp, "ERROR: no permission to options");
	goto error;
#endif
      emsgsize:
	LOGNO(tp, "ERROR: message too large %ld > %ld", mlen, mmax);
	goto error;
      baddata:
	LOGNO(tp, "ERROR: zero length data");
	goto error;
      outstate:
	LOGNO(tp, "ERROR: would place i/f out of state");
	goto error;
      einval:
	LOGNO(tp, "ERROR: invalid primitive format");
	goto error;
      discard:
	LOGNO(tp, "ERROR: ignore in idle state");
	freemsg(mp);
	return (0);
      notsupport:
	LOGNO(tp, "ERROR: primitive not supported for T_CLTS");
	goto error;
      error:
	return m_error(tp, q, mp, -EPROTO);
}

#ifdef T_ADDR_REQ
/*
 *  T_ADDR_REQ          25 - Address Request
 *  -------------------------------------------------------------------
 */
static fastcall int
t_addr_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct tsapaddr *loc = NULL;
	struct tsapaddr *rem = NULL;

	(void) mp;
	switch (tp_get_state(tp)) {
	case TS_UNBND:
		break;
	case TS_IDLE:
		loc = &tp->src;
		break;
	case TS_WCON_CREQ:
	case TS_DATA_XFER:
	case TS_WIND_ORDREL:
	case TS_WREQ_ORDREL:
		loc = &tp->src;
		rem = &tp->dst;
		break;
	case TS_WRES_CIND:
		rem = &tp->dst;
		break;
	default:
		goto outstate;
	}
	return t_addr_ack(tp, q, mp, loc, rem);
      outstate:
	return t_error_ack(tp, q, mp, T_ADDR_REQ, TOUTSTATE);
}
#endif
#ifdef T_CAPABILITY_REQ
/*
 *  T_CAPABILITY_REQ    ?? - Capability Request
 *  -------------------------------------------------------------------
 */
static fastcall int
t_capability_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	int err;
	struct T_capability_req *p = (typeof(p)) mp->b_rptr;

	if (unlikely(MBLKL(mp) < sizeof(*p)))
		goto einval;
	(void) tp;
	return t_capability_ack(tp, q, mp, p->CAP_bits1, DB_TYPE(mp));

      einval:
	err = -EINVAL;
	LOGNO(tp, "ERROR: invalid message format");
	goto error;
      error:
	return t_error_ack(tp, q, mp, T_CAPABILITY_REQ, err);
}
#endif
/*
 *  Other primitives    XX - other invalid primitives
 *  -------------------------------------------------------------------------
 */
static int
t_other_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	t_scalar_t prim = *((t_scalar_t *) mp->b_rptr);

	return t_error_ack(tp, q, mp, prim, TNOTSUPPORT);
}

/*
 *  =========================================================================
 *
 *  Timers:
 *
 *  =========================================================================
 */

/**
 * tp_t1_timeout - retransmission timer timeout
 * @tp: private structure (locked)
 * @q: active queue (read queue)
 * @bp: timer message block
 *
 * In state _TP_STATE_WFCC:
 * If _TP_PREDICATE_P7 (count = maximum) and _TP_PREDICATE_P3 (local choke),
 *	_TP_ACTION_0 (set reference timer), generate T-DISCONNECT indication, move
 *	to state _TP_STATE_REFWAIT.
 * If _TP_PREDICATE_P7 (count = maximum) and not _TP_PREDICATE_P3 (local choke),
 *	_TP_ACTION_3 (set retransmission timer), _TP_ACTION_2 (count = 0),
 *	_TP_ACTION_1 (count = count + 1), _TP_ACTION_15 (send the DR-TPDU, this
 *	DR-TPDU is sent with src-ref = local-ref and dst-ref = remote-ref (may be
 *	zero)), generate T-DISCONNECT indication, move to state _TP_STATE_CLOSING.
 * If not _TP_PREDICATE_P7 (count = maximum), _TP_ACTION_1 (count = count + 1),
 *	_TP_ACTION_3 (set retransmission timer), send CR, stay in state
 *	_TP_STATE_WFCC.
 *
 * In state _TP_STATE_WBCL:
 * If _TP_PREDICATE_P7 (count = maximum) and _TP_PREDICATE_P3 (local choke),
 *	_TP_ACTION_0 (set reference timer), move to state _TP_STATE_REFWAIT.
 * If _TP_PREDICATE_P7 (count = maximum) and not _TP_PREDICATE_P3 (local choke),
 *	_TP_ACTION_3 (set retransmission timer), _TP_ACTION_P2 (count = 0),
 *	_TP_ACTION_1 (count = count + 1), _TP_ACTION_15 (send the DR-TPDU, this
 *	DR-TPDU is sent with src-ref = local-ref and dst-ref = remote-ref (may be
 *	zero)), move to state _TP_STATE_CLOSING.
 * If not _TP_PREDICATE_P7 (count = maximum), _TP_ACTION_1 (count = count + 1),
 *	_TP_ACTION_3 (set retransmission timer), send CR, move to state
 *	_TP_STATE_WBCL.
 *
 * In state _TP_STATE_OPEN:
 * If _TP_PREDICATE_P7 (count = maximum), _TP_ACTION_6 (stop window timer if
 *	running), _TP_ACTION_8 (stop inactivity timer if running), _TP_ACTION_3
 *	(set retransmission timer), _TP_ACTION_2 (count = 0), _TP_ACTION_1 (count =
 *	count + 1), _TP_ACTION_15 (send the DR-TPDU, this DR-TPDU is sent with
 *	src-ref = local-ref and dst-ref = remote-ref (may be zero)), issue
 *	T-DISCONNECT indication, move to state _TP_STATE_CLOSING.
 * If not _TP_PREDICATE_P7 (count = maximum), _TP_NOTE_16 (see data transfer
 *	procedures).
 *
 * In state _TP_STATE_AKWAIT:
 * If _TP_PREDICATE_P7 (count = maximum), _TP_ACTION_3 (set retransmission timer),
 *	_TP_ACTION_2 (count = 0), _TP_ACTION_1 (count = count + 1), _TP_ACTION_15
 *	(send DR-TPDU, the DR-TPDU is sent with src-ref = local-ref and dst-ref =
 *	remote-ref (may be zero)), generate T-DISCONNECT indication, move to
 *	_TP_STATE_CLOSING.
 * If not _TP_PREDICATE_P7 (count = maximum), _TP_ACTION_1 (count = count + 1),
 *	_TP_ACTION_3 (set retransmission timer), send CC-TPDU, move to
 *	_TP_STATE_AKWAIT.
 *
 * In state _TP_STATE_CLOSING:
 * If _TP_PREDICATE_P7 (count = maximum), _TP_ACTION_0 (set reference timer), move
 *	to _TP_STATE_REFWAIT.
 * If not _TP_PREDICATE_P7 (count = maximum), _TP_ACTION_1 (count = count + 1),
 *	_TP_ACTION_3 (set retransmission timer), _TP_ACTION_15 (send the DR-TPDU,
 *	the DR-TPDU is sent with src-ref = local-ref and dst-ref = remote-ref (may
 *	be zero)), move to _TP_STATE_CLOSING.
 */
static fastcall int
tp_t1_timeout(struct tp *tp, queue_t *q, mblk_t *bp)
{
	int err;

	switch (tp->pstate) {
	case _TP_STATE_WFCC:
		if (tp->count >= tp->maximum) {
			mblk_t *mp;

			if ((err = t_discon_ind(tp, q, _TP_REASON_UNSPECIFIED, NULL)) != 0)
				goto error;
			if (tp->local_choke
			    || unlikely((mp = tp_pack_dr(tp, q, _TP_REASON_UNSPECIFIED, NULL)) == NULL)) {
				tp_timer_start(tp, q, t4);
				tp->pstate = _TP_STATE_REFWAIT;
			} else {
				tp_repeat(tp, q, mp);
				tp->pstate = _TP_STATE_CLOSING;
			}
		} else {
			/* resend the CR-TPDU */
			tp_resend(tp, q);
		}
		break;
	case _TP_STATE_WBCL:
		if (tp->count >= tp->maximum) {
			mblk_t *mp;

			if ((err = t_discon_ind(tp, q, _TP_REASON_UNSPECIFIED, NULL)) != 0)
				goto error;
			if (tp->local_choke
			    || unlikely((mp = tp_pack_dr(tp, q, _TP_REASON_UNSPECIFIED, NULL)) == NULL)) {
				tp_timer_start(tp, q, t4);
				tp->pstate = _TP_STATE_REFWAIT;
			} else {
				tp_repeat(tp, q, mp);
				tp->pstate = _TP_STATE_CLOSING;
			}
		} else {
			/* resend the CR-TPDU */
			tp_resend(tp, q);
		}
		break;
	case _TP_STATE_OPEN:
		if (tp->count >= tp->maximum) {
			mblk_t *mp;

			if (unlikely((mp = tp_pack_dr(tp, q, _TP_REASON_UNSPECIFIED, NULL)) == NULL))
				goto enobufs;
			if ((err = t_discon_ind(tp, q, _TP_REASON_UNSPECIFIED, NULL)) != 0)
				goto error;
			tp_timer_stop(tp, t3);
			tp_timer_stop(tp, t2);
			tp_repeat(tp, q, mp);
			tp->pstate = _TP_STATE_CLOSING;
		} else {
			/* FIXME: see data transfer procedures */
		}
		break;
	case _TP_STATE_AKWAIT:
		if (tp->count >= tp->maximum) {
			mblk_t *mp;

			if (unlikely((mp = tp_pack_dr(tp, q, _TP_REASON_UNSPECIFIED, NULL)) == NULL))
				goto enobufs;
			if ((err = t_discon_ind(tp, q, _TP_REASON_UNSPECIFIED, NULL)) != 0)
				goto error;
			tp_repeat(tp, q, mp);
			tp->pstate = _TP_STATE_CLOSING;
		} else {
			/* resend the CC-TPDU */
			tp_resend(tp, q);
		}
		break;
	case _TP_STATE_CLOSING:
		if (tp->count >= tp->maximum) {
			tp_timer_start(tp, q, t4);
			tp->pstate = _TP_STATE_REFWAIT;
		} else {
			/* resend the DR-TPDU */
			tp_resend(tp, q);
		}
		break;
	}
	return (0);
      error:
	return (err);
      enobufs:
	err = -ENOBUFS;
	goto error;
}

/**
 * tp_t2_timeout - inactivity timer timeout
 * @tp: private structure (locked)
 * @q: active queue (read queue)
 * @bp: timer message block
 *
 * In state _TP_STATE_OPEN: _TP_ACTION_6 (stop window timer if running),
 * _TP_ACTION_4 (stop retransmission timer if running), _TP_ACTION_3 (set
 * retransmission timer), _TP_ACTION_2 (count =
 * 0), _TP_ACTION_1 (count = count + 1), _TP_ACTION_15 (send DR-TPDU, this DR-TPDU
 * is sent with src-ref = local-ref and dst-ref = remote-ref (may be zero)),
 * generate T-DISCONNECT indication, move to _TP_STATE_CLOSING, _TP_NOTE_7 (as a
 * local choice, it is also possible to apply the following: _TP_ACTION_0 (set
 * reference timer), generate T-DISCONNECT indication, move to _TP_STATE_REFWAIT).
 */
static fastcall int
tp_t2_timeout(struct tp *tp, queue_t *q, mblk_t *bp)
{
	int err;

	if (tp->pstate == _TP_STATE_OPEN) {
		mblk_t *mp;

		if (unlikely((err = t_discon_ind(tp, q, _TP_REASON_UNSPECIFIED, NULL)) != 0))
			goto error;
		tp_timer_stop(tp, t3);
		tp_timer_stop(tp, t1);
		if (unlikely((mp = tp_pack_dr(tp, q, _TP_REASON_UNSPECIFIED, NULL)) == NULL)) {
			tp_timer_start(tp, q, t4);
			tp->pstate = _TP_STATE_REFWAIT;
		} else {
			tp_repeat(tp, q, mp);
			tp->pstate = _TP_STATE_CLOSING;
		}
	}
	return (0);
#if 0
      enobufs:
	err = -ENOBUFS;
	goto error;
#endif
      error:
	return (err);
}

/**
 * tp_t3_timeout - window timer timeout
 * @tp: private structure (locked)
 * @q: active queue (read queue)
 * @bp: timer message block
 *
 */
static fastcall int
tp_t3_timeout(struct tp *tp, queue_t *q, mblk_t *bp)
{
	return (-ENOBUFS);
}

/**
 * tp_t4_timeout - reference timer timeout
 * @tp: private structure (locked)
 * @q: active queue (read queue)
 * @bp: timer message block
 *
 * In state _TP_STATE_REFWAIT: move to _TP_STATE_CLOSED.
 *
 * Basically, we remain in the connection hashes when we are in reference wait.
 * When the timeout occurs, remove ourselves from the connection hashes and move to
 * the closed state.
 *
 */
static fastcall int
tp_t4_timeout(struct tp *tp, queue_t *q, mblk_t *bp)
{
	if (tp->pstate == _TP_STATE_REFWAIT) {
		struct tp_chash_bucket *hp;

		if ((hp = tp->connb)) {
			write_lock(&hp->lock);
			if ((*tp->cprev = tp->cnext))
				tp->cnext->cprev = tp->cprev;
			tp->cnext = NULL;
			tp->cprev = &tp->cnext;
			tp->connb = NULL;
			tp->lref = tp->rref = 0;
			bzero(&tp->dst, sizeof(tp->dst));
			bzero(&tp->rem, sizeof(tp->rem));
			/* tp_release(&tp); */
			write_unlock(&hp->lock);
		}
	}
	return (0);
}

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
		/* fall through */
	case 0:
		return (0);
	}
}

/**
 * __ss_w_proto_slow: process M_PROTO or M_PCPROTO primitive
 * @tp: private structure (locked)
 * @q: the active queue
 * @mp: the M_PROTO or M_PCPROTO primitive
 * @prim: primitive
 *
 * This function either returns zero (0) when the message has been consumed, or a negative error
 * number when the message is to be (re)queued.  This function must be called with the private
 * structure locked.
 */
noinline fastcall int
__tp_w_proto_slow(struct tp *tp, queue_t *q, mblk_t *mp, t_scalar_t prim)
{
	int rtn;

	tp->oldstate = tp_get_state(tp);

	switch (prim) {
	case T_DATA_REQ:
	case T_EXDATA_REQ:
	case T_UNITDATA_REQ:
	case T_OPTDATA_REQ:
		LOGDA(tp, "-> %s", tpi_primname(prim));
		break;
	default:
		LOGRX(tp, "-> %s", tpi_primname(prim));
		break;
	}
	switch (prim) {
	case T_CONN_REQ:
		rtn = t_conn_req(tp, q, mp);
		break;
	case T_CONN_RES:
		rtn = t_conn_res(tp, q, mp);
		break;
	case T_DISCON_REQ:
		rtn = t_discon_req(tp, q, mp);
		break;
	case T_DATA_REQ:
		rtn = t_data_req(tp, q, mp);
		break;
	case T_EXDATA_REQ:
		rtn = t_exdata_req(tp, q, mp);
		break;
	case T_INFO_REQ:
		rtn = t_info_req(tp, q, mp);
		break;
	case T_BIND_REQ:
		rtn = t_bind_req(tp, q, mp);
		break;
	case T_UNBIND_REQ:
		rtn = t_unbind_req(tp, q, mp);
		break;
	case T_OPTMGMT_REQ:
		rtn = t_optmgmt_req(tp, q, mp);
		break;
	case T_UNITDATA_REQ:
		rtn = t_unitdata_req(tp, q, mp);
		break;
	case T_ORDREL_REQ:
		rtn = t_ordrel_req(tp, q, mp);
		break;
	case T_OPTDATA_REQ:
		rtn = t_optdata_req(tp, q, mp);
		break;
#ifdef T_ADDR_REQ
	case T_ADDR_REQ:
		rtn = t_addr_req(tp, q, mp);
		break;
#endif
#ifdef T_CAPABILITY_REQ
	case T_CAPABILITY_REQ:
		rtn = t_capability_req(tp, q, mp);
		break;
#endif
	case T_CONN_IND:
	case T_CONN_CON:
	case T_DISCON_IND:
	case T_DATA_IND:
	case T_EXDATA_IND:
	case T_INFO_ACK:
	case T_BIND_ACK:
	case T_ERROR_ACK:
	case T_OK_ACK:
	case T_UNITDATA_IND:
	case T_UDERROR_IND:
	case T_OPTMGMT_ACK:
	case T_ORDREL_IND:
	case T_OPTDATA_IND:
	case T_ADDR_ACK:
#ifdef T_CAPABILITY_ACK
	case T_CAPABILITY_ACK:
#endif
		LOGRX(tp, "%s() replying with error %d", __FUNCTION__, -EPROTO);
		rtn = m_error(tp, q, mp, -EPROTO);
		break;
	default:
		rtn = t_other_req(tp, q, mp);
		break;
	}
	if (rtn < 0)
		tp_set_state(tp, tp->oldstate);
	/* The put and srv procedures do not recognize all errors.  Sometimes we return an error to here just 
	   to restore the previous state. */
	return tp_w_proto_return(mp, rtn);
}

noinline fastcall int
tp_w_proto_slow(queue_t *q, mblk_t *mp, t_scalar_t prim)
{
	struct tp *tp;
	int err;

	if (likely(! !(tp = tp_trylock(q)))) {
		err = __tp_w_proto_slow(tp, q, mp, prim);
		tp_unlock(tp);
	} else
		err = -EDEADLK;
	return (err);

}

/**
 * __tp_w_proto: - process M_PROTO or M_PCPROTO message locked
 * @tp: private structure (locked)
 * @q: active queue (write queue)
 * @mp: the M_PROTO or M_PCPROTO message
 *
 * This locked version is for use by the service procedure that takes private structure locks once
 * for the entire service procedure run.
 */
static inline fastcall __hot_write int
__tp_w_proto(struct tp *tp, queue_t *q, mblk_t *mp)
{
	if (likely(MBLKL(mp) >= sizeof(t_scalar_t))) {
		t_scalar_t prim = *(t_scalar_t *) mp->b_rptr;
		int rtn;

		tp->oldstate = tp_get_state(tp);
		if (likely(prim == T_DATA_REQ)) {
			if (likely((rtn = t_data_req(tp, q, mp)) == 0))
				return (0);
		} else if (likely(prim == T_UNITDATA_REQ)) {
			if (likely((rtn = t_unitdata_req(tp, q, mp)) == 0))
				return (0);
		} else if (likely(prim == T_OPTDATA_REQ)) {
			if (likely((rtn = t_optdata_req(tp, q, mp)) == 0))
				return (0);
		} else if (likely(prim == T_EXDATA_REQ)) {
			if (likely((rtn = t_exdata_req(tp, q, mp)) == 0))
				return (0);
		} else
			return __tp_w_proto_slow(tp, q, mp, prim);
		if (rtn < 0)
			tp_set_state(tp, tp->oldstate);
		return tp_w_proto_return(mp, rtn);
	}
	LOGRX(tp, "%s() replying with error %d", __FUNCTION__, -EPROTO);
	return m_error(tp, q, mp, -EPROTO);
}

/**
 * tp_w_proto: - process M_PROTO or M_PCPROTO message
 * @q: active queue (write queue)
 * @mp: the M_PROTO or M_PCPROTO message
 *
 * This locking version is for use by the put procedure which does not take locks.  For data
 * messages we simply return (-EAGAIN) for performance.
 */
static inline fastcall __hot_write int
tp_w_proto(queue_t *q, mblk_t *mp)
{
	if (likely(MBLKL(mp) >= sizeof(t_scalar_t))) {
		t_scalar_t prim = *(t_scalar_t *) mp->b_rptr;

		if (likely(prim == T_DATA_REQ)) {
		} else if (likely(prim == T_UNITDATA_REQ)) {
		} else if (likely(prim == T_OPTDATA_REQ)) {
		} else if (likely(prim == T_EXDATA_REQ)) {
		} else
			return tp_w_proto_slow(q, mp, prim);
	}
	return (-EAGAIN);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_DATA Handling
 *
 *  -------------------------------------------------------------------------
 */

/**
 * __tp_r_data: - process M_DATA message
 * @tp: private structure (locked)
 * @q: active queue (read queue)
 * @mp: the M_DATA message
 *
 * This function either returns zero (0) when the message is consumed, or a
 * negative error number when the message is to be (re)queued.  This non-locking
 * version is used by the service procedure.
 */
static inline fastcall __hot_read int
__tp_r_data(struct tp *tp, queue_t *q, mblk_t *mp)
{
	return t_read(tp, q, mp);
}

/**
 * __tp_w_data: - process M_DATA message
 * @tp: private structure (locked)
 * @q: active queue (write queue)
 * @mp: the M_DATA message
 *
 * This function either returns zero (0) when the message is consumed, or a negative error number
 * when the message is to be (re)queued.  This non-locking version is used by the service procedure.
 */
static inline fastcall __hot_write int
__tp_w_data(struct tp *tp, queue_t *q, mblk_t *mp)
{
	return t_write(tp, q, mp);
}

/**
 * tp_w_data: - process M_DATA  messages
 * @q: active queue (write queue)
 * @mp: the M_DATA message
 *
 * This function either returns zero (0) when the message is consumed, or a negative error number
 * when the message is to be (re)queued.  This locking version is used by the put procedure.
 */
static inline fastcall __hot_write int
tp_w_data(queue_t *q, mblk_t *mp)
{
	/* Always queue from put procedure for performance. */
	return (-EAGAIN);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_FLUSH Handling
 *
 *  -------------------------------------------------------------------------
 */

/**
 * tp_r_flush: M_FLUSH handling
 * @q: active queue (read queue)
 * @mp: the M_FLUSH message
 *
 * As we are a driver it is not normal to receive a flush message on the read queue (unless we have
 * explicitly place it there); however, at some point this driver might be pushed as a module over
 * an IP or LLC driver stream in which case we will need to perform proper canonical flushing.  The
 * private structure is unlocked at this point.  This funciton always consumes the message and
 * returns 0.
 */
noinline fastcall __unlikely int
tp_r_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHALL);
		else
			flushq(q, FLUSHALL);
		putnext(q, mp);
	} else if (mp->b_rptr[0] & FLUSHW) {
		putnext(q, mp);
	} else
		freemsg(mp);
	return (0);
}

/**
 * tp_w_flush: - canonical driver write flush procedure
 * @q: active queue (write queue)
 * @mp: the flush message
 *
 * This canonical driver write flush procedure flushes the write side queue if requested and cancels
 * write side flush.  If read side flush is also requested, the read side is flushed and the message
 * is passed upwards.  If read side flush is not requested, the message is freed.
 */
noinline fastcall int
tp_w_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		mp->b_rptr[0] &= ~FLUSHW;
	}
	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(OTHERQ(q), mp->b_rptr[1], FLUSHDATA);
		else
			flushq(OTHERQ(q), FLUSHDATA);
		qreply(q, mp);
		return (0);
	}
	freemsg(mp);
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_IOCTL Handling
 *
 *  -------------------------------------------------------------------------
 */

/**
 * tp_w_ioctl: - process M_IOCTL or M_IOCDATA messages
 * @q: active queue (write queue only)
 * @mp: the M_IOCTL or M_IOCDATA message
 *
 * This is canonical driver write processing for M_IOCTL or M_IOCDATA messages for a
 * driver that has no input-output controls of its own.  Messages are simply
 * negatively acknowledged.
 */
noinline fastcall int
tp_w_ioctl(queue_t *q, mblk_t *mp)
{
	mi_copy_done(q, mp, EINVAL);
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_SIG, M_PCSIG Handling
 *
 *  -------------------------------------------------------------------------
 */

static int
do_timeout(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct tp_timer *t = (typeof(t)) mp->b_rptr;

	switch (t->timer) {
	case t1:
		LOGTO(tp, "t1 expiry at %lu", jiffies);
		return tp_t1_timeout(tp, q, mp);
	case t2:
		LOGTO(tp, "t2 expiry at %lu", jiffies);
		return tp_t2_timeout(tp, q, mp);
	case t3:
		LOGTO(tp, "t3 expiry at %lu", jiffies);
		return tp_t3_timeout(tp, q, mp);
	case t4:
		LOGTO(tp, "t4 expiry at %lu", jiffies);
		return tp_t4_timeout(tp, q, mp);
	default:
		return (0);
	}
}

noinline fastcall int
__tp_r_sig(struct tp *tp, queue_t *q, mblk_t *mp)
{
	int err = 0;

	tp->oldstate = tp_get_state(tp);

	if (likely(mi_timer_valid(mp))) {
		if ((err = do_timeout(tp, q, mp)) != 0) {
			tp_set_state(tp, tp->oldstate);
			err = mi_timer_requeue(mp) ? err : 0;
		}
	}
	return (err);
}

noinline fastcall int
tp_r_sig(queue_t *q, mblk_t *mp)
{
	struct tp *tp;
	int err;

	if (likely((tp = tp_trylock(q)) != NULL)) {
		err = __tp_r_sig(tp, q, mp);
		tp_unlock(tp);
	} else
		err = -EDEADLK;
	return (err);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Other Message Handling
 *
 *  -------------------------------------------------------------------------
 */

/**
 * tp_w_other: - process other messages on the driver write queue
 * @q: active queue (write queue only)
 * @mp: the message
 *
 * The only other messages that we expect on the driver write queue is M_FLUSH and M_IOCTL or
 * M_IOCDATA messages.  We process these appropriately.  Any other message is an error and is
 * discarded.
 */
noinline fastcall int
tp_w_other(queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_FLUSH)) {
	case M_FLUSH:
		return tp_w_flush(q, mp);
	case M_IOCTL:
	case M_IOCDATA:
		return tp_w_ioctl(q, mp);
	}
	LOGERR(PRIV(q), "SWERR: %s %s:%d", __FUNCTION__, __FILE__, __LINE__);
	freemsg(mp);
	return (0);
}

noinline fastcall int
tp_r_other(queue_t *q, mblk_t *mp)
{
	LOGERR(PRIV(q), "SWERR: %s %s:%d", __FUNCTION__, __FILE__, __LINE__);
	freemsg(mp);
	return (0);
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
		return tp_w_proto(q, mp);
	case M_DATA:
		return tp_w_data(q, mp);
	case M_FLUSH:
		return tp_w_flush(q, mp);
	case M_IOCTL:
	case M_IOCDATA:
		return tp_w_ioctl(q, mp);
	}
	LOGERR(PRIV(q), "SWERR: %s %s: %d", __FUNCTION__, __FILE__, __LINE__);
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
		return __tp_w_proto(tp, q, mp);
	case M_DATA:
		return __tp_w_data(tp, q, mp);
	case M_FLUSH:
		return tp_w_flush(q, mp);
	case M_IOCTL:
	case M_IOCDATA:
		return tp_w_ioctl(q, mp);
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
STATIC streamscall __hot_in int
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
STATIC inline streamscall __hot_in int
tp_r_prim_srv(struct tp *tp, queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_DATA)) {
	case M_DATA:
		return __tp_r_data(tp, q, mp);
	case M_SIG:
	case M_PCSIG:
		return __tp_r_sig(tp, q, mp);
	case M_CTL:
		return (-EFAULT);
		// FIXME FIXME FIXME
		// return __tp_recv_err(tp, q, mp);
	case M_FLUSH:
		return tp_r_flush(q, mp);
	default:
		return tp_r_other(q, mp);
	}
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
	struct tp *tp;

	if (likely((tp = tp_trylock(q)) != NULL)) {
		mblk_t *mp;

		// __tp_deferred_timers(tp);

		if (likely((mp = getq(q)) != NULL)) {
			// __tp_cleanup_read(tp);
			do {
				if (unlikely(tp_r_prim_srv(tp, q, mp))) {
					if (unlikely(!putbq(q, mp))) {
						mp->b_band = 0;	/* must succeed */
						putbq(q, mp);
					}
					LOGRX(tp, "read queue stalled");
					// __tp_transmit_wakeup(tp);
					break;
				}
			} while (likely((mp = getq(q)) != NULL));
		}
		// __tp_cleanup_read(tp);
		// __tp_transmit_wakeup(tp);
		tp_unlock(tp);
	}
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
		// __tp_transmit_wakeup(tp);
		tp_unlock(tp);
	}
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

	freezestr(q);
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
	unfreezestr(q);
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
	minor_t cminor = getminor(*devp);
	psw_t flags;
	caddr_t ptr;
	struct tp *tp;
	int err;

	if (q->q_ptr)
		return (0);	/* already open */
	if (cminor < FIRST_CMINOR || cminor > LAST_CMINOR)
		return (ENXIO);
	if (!mi_set_sth_lowat(q, 0))
		return (ENOBUFS);
	if (!mi_set_sth_hiwat(q, SHEADHIWAT >> 1))
		return (ENOBUFS);
	if ((err = tp_alloc_qbands(q, cminor)))
		return (err);
	if (!(tp = (struct tp *) (ptr = mi_open_alloc_cache(tp_priv_cachep, GFP_KERNEL))))
		return (ENOMEM);
	{
		struct tp_timer *t;

		bzero(tp, sizeof(*tp));
		tp->tp_parent = tp;
		tp->rq = RD(q);
		tp->wq = WR(q);
		tp->p = tp_profiles[cminor - FIRST_CMINOR];
		tp->cred = *crp;
		bufq_init(&tp->conq);
		if (!(tp->timers.t1 = mi_timer_alloc(sizeof(*t))))
			return (ENOBUFS);
	}
	sflag = CLONEOPEN;
	cminor = FREE_CMINOR;	/* start at the first free minor device number */
	/* The static device range for mi_open_link() is 5 or 10, and tpsn uses 10, so we adjust the minor
	   device number before calling mi_open_link(). */
	*devp = makedevice(getmajor(*devp), cminor);
	write_lock_irqsave(&tp_lock, flags);
	if (mi_acquire_sleep(ptr, &ptr, &tp_lock, &flags) == NULL) {
		err = EINTR;
		goto unlock_free;
	}
	if ((err = mi_open_link(&tp_opens, ptr, devp, oflags, sflag, crp))) {
		mi_release(ptr);
		goto unlock_free;
	}
	mi_attach(q, ptr);
	mi_release(ptr);
	write_unlock_irqrestore(&tp_lock, flags);
	tp_priv_init(tp);
	return (0);
      unlock_free:
	mi_close_free_cache(tp_priv_cachep, ptr);
	write_unlock_irqrestore(&tp_lock, flags);
	return (err);
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
	struct tp *tp = PRIV(q);
	caddr_t ptr = (caddr_t) tp;
	psw_t flags;
	int err;

	write_lock_irqsave(&tp_lock, flags);
	mi_acquire_sleep_nosignal(ptr, &ptr, &tp_lock, &flags);
	qprocsoff(q);
	while (bufq_head(&tp->conq))
		freemsg(__bufq_dequeue(&tp->conq));
	tp->tp_parent = NULL;
	tp->rq = NULL;
	tp->wq = NULL;
	err = mi_close_comm(&tp_opens, q);
	write_unlock_irqrestore(&tp_lock, flags);
	return (err);
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
#if defined HAVE_NET_PROTOCOL_ERR_HANDLER_RETURNS_INT
STATIC int tp_tp4_v4_err(struct sk_buff *skb, uint32_t info);
#else
STATIC void tp_tp4_v4_err(struct sk_buff *skb, uint32_t info);
#endif
STATIC int tp_iso_v4_rcv(struct sk_buff *skb);
#if defined HAVE_NET_PROTOCOL_ERR_HANDLER_RETURNS_INT
STATIC int tp_iso_v4_err(struct sk_buff *skb, uint32_t info);
#else
STATIC void tp_iso_v4_err(struct sk_buff *skb, uint32_t info);
#endif
STATIC __hot_in int tp_udp_v4_rcv(struct sk_buff *skb);
#if defined HAVE_NET_PROTOCOL_ERR_HANDLER_RETURNS_INT
STATIC __unlikely int tp_udp_v4_err(struct sk_buff *skb, u32 info);
#else
STATIC __unlikely void tp_udp_v4_err(struct sk_buff *skb, u32 info);
#endif

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
	struct tp *tp = NULL;
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
		if (!
		    (tp =
		     tp_lookup(tp->dref, tp->sref, *(uint16_t *) tp->src.tsel, *(uint16_t *) tp->dst.tsel,
			       &tp->loc, &tp->rem)))
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
	// tp_rcv_ootb(mp); FIXME FIXME FIXME
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
STATIC
#if defined HAVE_NET_PROTOCOL_ERR_HANDLER_RETURNS_INT
int
#else
void
#endif
tp_tp4_v4_err(struct sk_buff *skb, uint32_t info)
{
	struct tp *tp = NULL;
	struct iphdr *iph = (struct iphdr *) skb->data;

	if (skb->len < (iph->ihl << 2) + 1 + sizeof(struct tp4hdr))
		goto drop;
	printd(("%s: %s: error packet received %p\n", DRV_NAME, __FUNCTION__, skb));
	/* Note: use returned IP header and possibly payload for lookup */
//      if ((tp = tp_tp4_lookup_icmp(iph, skb->len)) == NULL)
//              goto no_stream;
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
#if defined HAVE_NET_PROTOCOL_ERR_HANDLER_RETURNS_INT
	return(0);
#else
	return;
#endif
      no_buffers:
	ptrace(("ERROR: could not allocate buffer\n"));
	goto discard_put;
      closed:
	ptrace(("ERROR: ICMP for closed stream\n"));
	goto discard_put;
#if 0
      no_stream:
	ptrace(("ERROR: could not find stream for ICMP message\n"));
	tp_v4_err_next(tp_protos.tp4.next, skb, info);
	goto drop;
#endif
      drop:
#ifdef HAVE_KINC_LINUX_SNMP_H
#ifndef ICMP_INC_STATS_BH
	__ICMP_INC_STATS(dev_net(skb->dev), ICMP_MIB_INERRORS);
#else
#ifdef HAVE_ICMP_INC_STATS_BH_2_ARGS
	ICMP_INC_STATS_BH(dev_net(skb->dev), ICMP_MIB_INERRORS);
#else
	ICMP_INC_STATS_BH(ICMP_MIB_INERRORS);
#endif
#endif
#else
	ICMP_INC_STATS_BH(IcmpInErrors);
#endif
#if defined HAVE_NET_PROTOCOL_ERR_HANDLER_RETURNS_INT
	return(0);
#else
	return;
#endif
}

STATIC int
tp_iso_v4_rcv(struct sk_buff *skb)
{
	mblk_t *mp;
	struct tp *tp = NULL;
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
		if (!
		    (tp =
		     tp_lookup(tp->dref, tp->sref, *(uint16_t *) tp->src.tsel, *(uint16_t *) tp->dst.tsel,
			       &tp->loc, &tp->rem)))
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
	// tp_rcv_ootb(mp); FIXME FIXME FIXME
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

STATIC
#if defined HAVE_NET_PROTOCOL_ERR_HANDLER_RETURNS_INT
int
#else
void
#endif
tp_iso_v4_err(struct sk_buff *skb, uint32_t info)
{
	struct tp *tp = NULL;
	struct iphdr *iph = (struct iphdr *) skb->data;

	if (skb->len < (iph->ihl << 2) + 1 + sizeof(struct tp4hdr))
		goto drop;
	printd(("%s: %s: error packet received %p\n", DRV_NAME, __FUNCTION__, skb));
	/* Note: use returned IP header and possibly payload for lookup */
//      if ((tp = tp_iso_lookup_icmp(iph, skb->len)) == NULL)
//              goto no_stream;
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
#if defined HAVE_NET_PROTOCOL_ERR_HANDLER_RETURNS_INT
	return(0);
#else
	return;
#endif
      no_buffers:
	ptrace(("ERROR: could not allocate buffer\n"));
	goto discard_put;
      closed:
	ptrace(("ERROR: ICMP for closed stream\n"));
	goto discard_put;
#if 0
      no_stream:
	ptrace(("ERROR: could not find stream for ICMP message\n"));
	tp_v4_err_next(tp_protos.iso.next, skb, info);
	goto drop;
#endif
      drop:
#ifdef HAVE_KINC_LINUX_SNMP_H
#ifndef ICMP_INC_STATS_BH
	__ICMP_INC_STATS(dev_net(skb->dev), ICMP_MIB_INERRORS);
#else
#ifdef HAVE_ICMP_INC_STATS_BH_2_ARGS
	ICMP_INC_STATS_BH(dev_net(skb->dev), ICMP_MIB_INERRORS);
#else
	ICMP_INC_STATS_BH(ICMP_MIB_INERRORS);
#endif
#endif
#else
	ICMP_INC_STATS_BH(IcmpInErrors);
#endif
#if defined HAVE_NET_PROTOCOL_ERR_HANDLER_RETURNS_INT
	return(0);
#else
	return;
#endif
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
//      if (unlikely((tp = tp_udp_lookup(iph, uh, th)) == NULL))
//              goto no_stream;
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
#if 0
      bad_checksum:
//      UDP_INC_STATS_BH(UdpInErrors);
//      IP_INC_STATS_BH(IpInDiscards);
	/* decrement IpInDelivers ??? */
	// goto linear_fail;
#endif
      wrong_protocol:
      no_buffers:
      linear_fail:
	if (tp)
		tp_put(tp);
	kfree_skb(skb);
	return (0);
#if 0
      no_stream:
	ptrace(("ERROR: No stream\n"));
	/* Note, in case there is nobody to pass it to, we have to complete the checksum check before
	   dropping it to handle stats correctly. */
	if (skb->ip_summed != CHECKSUM_UNNECESSARY
	    && (unsigned short) csum_fold(skb_checksum(skb, 0, skb->len, skb->csum)))
		goto bad_checksum;
//      UDP_INC_STATS_BH(UdpNoPorts);   /* should wait... */
#endif
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
STATIC __unlikely
#if defined HAVE_NET_PROTOCOL_ERR_HANDLER_RETURNS_INT
int
#else
void
#endif
tp_udp_v4_err(struct sk_buff *skb, u32 info)
{
	struct tp *tp = NULL;
	struct iphdr *iph = (struct iphdr *) skb->data;

	if (skb->len < (iph->ihl << 2) + sizeof(struct udphdr) + 1 + sizeof(struct tp4hdr))
		goto drop;
	printd(("%s: %s: error packet received %p\n", DRV_NAME, __FUNCTION__, skb));
	/* Note: use returned IP header and possibly payload for lookup */
//      if ((tp = tp_udp_lookup_icmp(iph, skb->len)) == NULL)
//              goto no_stream;
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
#if defined HAVE_NET_PROTOCOL_ERR_HANDLER_RETURNS_INT
	return(0);
#else
	return;
#endif
      no_buffers:
	ptrace(("ERROR: could not allocate buffer\n"));
	goto discard_put;
      closed:
	ptrace(("ERROR: ICMP for closed stream\n"));
	goto discard_put;
#if 0
      no_stream:
	ptrace(("ERROR: could not find stream for ICMP message\n"));
	tp_v4_err_next(tp_protos.udp.next, skb, info);
	goto drop;
#endif
      drop:
#ifdef HAVE_KINC_LINUX_SNMP_H
#ifndef ICMP_INC_STATS_BH
	__ICMP_INC_STATS(dev_net(skb->dev), ICMP_MIB_INERRORS);
#else
#ifdef HAVE_ICMP_INC_STATS_BH_2_ARGS
	ICMP_INC_STATS_BH(dev_net(skb->dev), ICMP_MIB_INERRORS);
#else
	ICMP_INC_STATS_BH(ICMP_MIB_INERRORS);
#endif
#endif
#else
	ICMP_INC_STATS_BH(IcmpInErrors);
#endif
#if defined HAVE_NET_PROTOCOL_ERR_HANDLER_RETURNS_INT
	return(0);
#else
	return;
#endif
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
//      if (unlikely((tp = tp_llc_lookup(uh, &daddr, &saddr)) == NULL))
//              goto no_stream;
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
#if 0
      no_stream:
	ptrace(("ERROR: No stream\n"));
#endif
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
static int
tp_init(void)
{
	int err;

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
	(void) tp_timer_stop;
	(void) tp_timer_start;
	(void) t_set_options;
	(void) tp_addr_type;
	(void) tp_pack_ak;
	(void) tp_pack_rj;
	(void) tp_pack_er;
	(void) t_conn_ind;
	(void) t_conn_con;
	(void) t_ordrel_ind;
	(void) tp_pack_ea;

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
static void
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
