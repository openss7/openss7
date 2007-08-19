/*****************************************************************************

 @(#) $RCSfile: m2pa_sl.c,v $ $Name:  $($Revision: 0.9.2.20 $) $Date: 2007/08/19 11:48:34 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

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

 -----------------------------------------------------------------------------

 Last Modified $Date: 2007/08/19 11:48:34 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: m2pa_sl.c,v $
 Revision 0.9.2.20  2007/08/19 11:48:34  brian
 - move stdbool.h, bison changes

 Revision 0.9.2.19  2007/08/15 05:14:10  brian
 - GPLv3 updates

 Revision 0.9.2.18  2007/08/12 16:15:35  brian
 -

 Revision 0.9.2.17  2007/07/14 01:33:43  brian
 - make license explicit, add documentation

 Revision 0.9.2.16  2007/07/04 08:32:14  brian
 - doc updates and m2pa correction

 Revision 0.9.2.15  2007/05/18 12:24:02  brian
 - indentation

 Revision 0.9.2.14  2007/05/18 12:15:32  brian
 - careful not to flush timers

 Revision 0.9.2.13  2007/05/18 12:01:57  brian
 - trace logging and service procedures for m2pa

 Revision 0.9.2.12  2007/05/17 22:55:37  brian
 - use mi_timer requeue to requeue mi timers

 Revision 0.9.2.11  2007/03/25 18:59:06  brian
 - changes to support 2.6.20-1.2307.fc5 kernel

 Revision 0.9.2.10  2007/03/09 04:29:17  brian
 - never use bottom half locking

 Revision 0.9.2.9  2007/03/09 04:12:36  brian
 - fixed initial timeout value bug

 Revision 0.9.2.8  2007/01/26 21:54:36  brian
 - working up AS drivers and docs

 Revision 0.9.2.7  2006/12/18 08:51:36  brian
 - corections from testing, resolve device numbering

 Revision 0.9.2.6  2006/12/06 11:31:25  brian
 - rationalized to x400p driver and test program

 Revision 0.9.2.5  2006/11/03 11:08:46  brian
 - 32-bit compatibility testsuite passes

 Revision 0.9.2.4  2006/11/02 12:54:42  brian
 - major corrections and updates to M2PA module and tests cases

 Revision 0.9.2.3  2006/10/31 20:34:21  brian
 - more buildup of test suite

 *****************************************************************************/

#ident "@(#) $RCSfile: m2pa_sl.c,v $ $Name:  $($Revision: 0.9.2.20 $) $Date: 2007/08/19 11:48:34 $"

static char const ident[] =
    "$RCSfile: m2pa_sl.c,v $ $Name:  $($Revision: 0.9.2.20 $) $Date: 2007/08/19 11:48:34 $";

#define _LFS_SOURCE	1
#define _SVR4_SOURCE	1
#define _MPS_SOURCE	1
#define _SUN_SOURCE	1

//#define _DEBUG 1
#undef _DEBUG

#include <sys/os7/compat.h>

#undef mi_timer
#define mi_timer mi_timer_MAC

#include <linux/socket.h>
#include <net/ip.h>

#include <sys/npi.h>
#include <sys/npi_sctp.h>

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/sdli.h>
#include <ss7/sdli_ioctl.h>
#include <ss7/sdti.h>
#include <ss7/sdti_ioctl.h>
#include <ss7/sli.h>
#include <ss7/sli_ioctl.h>

#define M2PA_SL_DESCRIP		"M2PA/SCTP SIGNALLING LINK (SL) STREAMS MODULE."
#define M2PA_SL_REVISION	"OpenSS7 $RCSfile: m2pa_sl.c,v $ $Name:  $($Revision: 0.9.2.20 $) $Date: 2007/08/19 11:48:34 $"
#define M2PA_SL_COPYRIGHT	"Copyright (c) 1997-2007 OpenSS7 Corporation.  All Rights Reserved."
#define M2PA_SL_DEVICE		"Part of the OpenSS7 Stack for Linux Fast STREAMS."
#define M2PA_SL_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define M2PA_SL_LICENSE		"GPL"
#define M2PA_SL_BANNER		M2PA_SL_DESCRIP		"\n" \
				M2PA_SL_REVISION	"\n" \
				M2PA_SL_COPYRIGHT	"\n" \
				M2PA_SL_DEVICE		"\n" \
				M2PA_SL_CONTACT		"\n"
#define M2PA_SL_SPLASH		M2PA_SL_DEVICE		" - " \
				M2PA_SL_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(M2PA_SL_CONTACT);
MODULE_DESCRIPTION(M2PA_SL_DESCRIP);
MODULE_SUPPORTED_DEVICE(M2PA_SL_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(M2PA_SL_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-m2pa_sl");
#endif
#endif				/* LINUX */

#ifdef LFS
#define M2PA_SL_MOD_ID		CONFIG_STREAMS_M2PA_SL_MODID
#define M2PA_SL_MOD_NAME	CONFIG_STREAMS_M2PA_SL_NAME
#endif

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

#define MOD_ID		M2PA_SL_MOD_ID
#define MOD_NAME	M2PA_SL_MOD_NAME
#ifdef MODULE
#define MOD_SPLASH	M2PA_SL_BANNER
#else				/* MODULE */
#define MOD_SPLASH	M2PA_SL_SPLASH
#endif				/* MODULE */

STATIC struct module_info sl_minfo = {
	.mi_idnum = MOD_ID,		/* Module ID number */
	.mi_idname = MOD_NAME,		/* Module name */
	.mi_minpsz = 1,			/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,		/* Max packet size accepted */
	.mi_hiwat = (1 << 15),		/* Hi water mark */
	.mi_lowat = (1 << 10),		/* Lo water mark */
};

STATIC streamscall int sl_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC streamscall int sl_close(queue_t *, int, cred_t *);

STATIC struct qinit sl_rinit = {
	.qi_putp = ss7_oput,		/* Read put (msg from below) */
	.qi_srvp = ss7_osrv,		/* Read service */
	.qi_qopen = sl_open,		/* Each open */
	.qi_qclose = sl_close,		/* Last close */
	.qi_minfo = &sl_minfo,		/* Information */
};

STATIC struct qinit sl_winit = {
	.qi_putp = ss7_iput,		/* Write put (msg from above) */
	.qi_srvp = ss7_isrv,		/* Write service */
	.qi_minfo = &sl_minfo,		/* Information */
};

struct streamtab m2pa_slinfo = {
	.st_rdinit = &sl_rinit,		/* Upper read queue */
	.st_wrinit = &sl_winit,		/* Upper write queue */
};

#define M2PALOGST	1	/* log M2PA state transitions */
#define M2PALOGTO	2	/* log M2PA timeouts */
#define M2PALOGRX	3	/* log M2PA primitives received */
#define M2PALOGTX	4	/* log M2PA primitives issued */
#define M2PALOGTE	5	/* log M2PA timer events */
#define M2PALOGNO	6	/* log M2PA additional data */
#define M2PALOGDA	7	/* log M2PA data */

#if 0
#define m2palog(sl, level, flags, fmt, ...) \
	do { } while (0)
#else
#define m2palog(sl, level, flags, fmt, ...) \
	strlog(sl->mid, sl->sid, level, flags, fmt, ##__VA_ARGS__)
#endif

#define m2paloger(sl, fmt, ...) m2palog(sl, 0, SL_TRACE | SL_ERROR | SL_CONSOLE, fmt, ##__VA_ARGS__)
#define m2palogst(sl, fmt, ...) m2palog(sl, M2PALOGST, SL_TRACE, fmt, ##__VA_ARGS__)
#define m2palogto(sl, fmt, ...) m2palog(sl, M2PALOGTO, SL_TRACE, fmt, ##__VA_ARGS__)
#define m2palogrx(sl, fmt, ...) m2palog(sl, M2PALOGRX, SL_TRACE, fmt, ##__VA_ARGS__)
#define m2palogtx(sl, fmt, ...) m2palog(sl, M2PALOGTX, SL_TRACE, fmt, ##__VA_ARGS__)
#define m2palogte(sl, fmt, ...) m2palog(sl, M2PALOGTE, SL_TRACE, fmt, ##__VA_ARGS__)
#define m2palogno(sl, fmt, ...) m2palog(sl, M2PALOGNO, SL_TRACE, fmt, ##__VA_ARGS__)
#define m2palogda(sl, fmt, ...) m2palog(sl, M2PALOGDA, SL_TRACE, fmt, ##__VA_ARGS__)

/*
 *  =========================================================================
 *
 *  M2PA-SL Private Structure
 *
 *  =========================================================================
 */
typedef struct sl {
	STR_DECLARATION (struct sl);	/* stream declaration */
	int mid;			/* module identifier */
	int sid;			/* stream identifier */
	int n_request;			/* npi request */
	int n_state;			/* npi state */
	int coninds;			/* npi connection indications */
	mblk_t *rbuf;			/* reassembly buffer */
	uint rmsu;			/* received unacked msus */
	uint tmsu;			/* transmitted unacked msus */
	uint rack;			/* received acked msus */
	uint tack;			/* transmitted acked msus */
	uint fsnr;			/* received msu seq number */
	uint bsnr;			/* received msu seq number */
	uint fsnx;			/* expected msu seq number */
	uint fsnt;			/* transmitted msu seq number */
	uint back;			/* bad acks */
	uint bbsn;			/* bad bsn */
	uint bfsn;			/* bad fsn */
	lmi_option_t option;		/* protocol and variant options */
	bufq_t rb;			/* receive buffer */
	bufq_t tb;			/* transmission buffer */
	bufq_t rtb;			/* retransmission buffer */
	struct {
		uint flags;		/* overall provider flags */
		uint state;		/* overall provider state */
		sl_timers_t timers;	/* SL protocol timers */
		sl_config_t config;	/* SL configuration options */
		sl_statem_t statem;	/* state machine variables */
		sl_notify_t notify;	/* SL notification options */
		sl_stats_t stats;	/* SL statistics */
		sl_stats_t stamp;	/* SL statistics timestamps */
		sl_stats_t statsp;	/* SL statistics periods */
	} sl;
	struct {
		sdt_timers_t timers;	/* SDT timers */
		sdt_config_t config;	/* SDT configuration options */
	} sdt;
	struct {
		sdl_timers_t timers;	/* SDL timers */
		sdl_config_t config;	/* SDL configuration options */
	} sdl;
	struct {
		N_info_ack_t n;		/* NPI information */
		lmi_info_ack_t lm;	/* LMI information */
	} info;
	size_t loc_len;			/* local address length */
	size_t rem_len;			/* remote address length */
	struct sockaddr_in loc[8];	/* up to 8 local addresses */
	struct sockaddr_in rem[8];	/* up to 8 remote addresses */
} sl_t;

#define SL_PRIV(__q) ((struct sl *)(__q)->q_ptr)

STATIC struct sl *sl_alloc_priv(queue_t *q, struct sl **slp, dev_t *devp, cred_t *crp, int mid,
				int sid);
STATIC void sl_free_priv(queue_t *q);
STATIC struct sl *sl_get(struct sl *sl);
STATIC void sl_put(struct sl *sl);

#if 0
#define NSF_UNBND	(1<<NS_UNBND)
#define NSF_WACK_BREQ	(1<<NS_WACK_BREQ)
#define NSF_WACK_UREQ	(1<<NS_WACK_UREQ)
#define NSF_IDLE	(1<<NS_IDLE)
#define NSF_WACK_OPTREQ	(1<<NS_WACK_OPTREQ)
#define NSF_WACK_RRES	(1<<NS_WACK_RRES)
#define NSF_WCON_CREQ	(1<<NS_WCON_CREQ)
#define NSF_WRES_CIND	(1<<NS_WRES_CIND)
#define NSF_WACK_CRES	(1<<NS_WACK_CRES)
#define NSF_DATA_XFER	(1<<NS_DATA_XFER)
#define NSF_WCON_RREQ	(1<<NS_WCON_RREQ)
#define NSF_WRES_RIND	(1<<NS_WRES_RIND)
#define NSF_WACK_DREQ6	(1<<NS_WACK_DREQ6)
#define NSF_WACK_DREQ7	(1<<NS_WACK_DREQ7)
#define NSF_WACK_DREQ9	(1<<NS_WACK_DREQ9)
#define NSF_WACK_DREQ10	(1<<NS_WACK_DREQ10)
#define NSF_WACK_DREQ11	(1<<NS_WACK_DREQ11)
#endif

#ifdef _DEBUG
const char *
sl_n_state_name(int state)
{
	switch (state) {
	case NS_UNBND:		/* NS user not bound to network address */
		return ("NS_UNBND");
	case NS_WACK_BREQ:	/* Awaiting acknowledgement of N_BIND_REQ */
		return ("NS_WACK_BREQ");
	case NS_WACK_UREQ:	/* Pending acknowledgement for N_UNBIND_REQ */
		return ("NS_WACK_UREQ");
	case NS_IDLE:		/* Idle, no connection */
		return ("NS_IDLE");
	case NS_WACK_OPTREQ:	/* Pending acknowledgement of N_OPTMGMT_REQ */
		return ("NS_WACK_OPTREQ");
	case NS_WACK_RRES:	/* Pending acknowledgement of N_RESET_RES */
		return ("NS_WACK_RRES");
	case NS_WCON_CREQ:	/* Pending confirmation of N_CONN_REQ */
		return ("NS_WCON_CREQ");
	case NS_WRES_CIND:	/* Pending response of N_CONN_REQ */
		return ("NS_WRES_CIND");
	case NS_WACK_CRES:	/* Pending acknowledgement of N_CONN_RES */
		return ("NS_WACK_CRES");
	case NS_DATA_XFER:	/* Connection-mode data transfer */
		return ("NS_DATA_XFER");
	case NS_WCON_RREQ:	/* Pending confirmation of N_RESET_REQ */
		return ("NS_WCON_RREQ");
	case NS_WRES_RIND:	/* Pending response of N_RESET_IND */
		return ("NS_WRES_RIND");
	case NS_WACK_DREQ6:	/* Waiting ack of N_DISCON_REQ */
		return ("NS_WACK_DREQ6");
	case NS_WACK_DREQ7:	/* Waiting ack of N_DISCON_REQ */
		return ("NS_WACK_DREQ7");
	case NS_WACK_DREQ9:	/* Waiting ack of N_DISCON_REQ */
		return ("NS_WACK_DREQ9");
	case NS_WACK_DREQ10:	/* Waiting ack of N_DISCON_REQ */
		return ("NS_WACK_DREQ10");
	case NS_WACK_DREQ11:	/* Waiting ack of N_DISCON_REQ */
		return ("NS_WACK_DREQ11");
	default:
		return ("(unknown)");
	}
}
STATIC int
sl_set_n_state(struct sl *sl, int newstate)
{
	int oldstate = sl->n_state;

	m2palogst(sl, "%s <- %s", sl_n_state_name(oldstate), sl_n_state_name(newstate));
	return ((sl->n_state = newstate));
}
#else				/* _DEBUG */
STATIC int
sl_set_n_state(struct sl *sl, int newstate)
{
	return ((sl->n_state = newstate));
}
#endif				/* _DEBUG */
STATIC INLINE int
sl_get_n_state(struct sl *sl)
{
	return (sl->n_state);
}
STATIC INLINE int
sl_get_n_statef(struct sl *sl)
{
	return ((1 << sl_get_n_state(sl)));
}
STATIC INLINE int
sl_chk_n_state(struct sl *sl, int mask)
{
	return (sl_get_n_statef(sl) & mask);
}
STATIC INLINE int
sl_not_n_state(struct sl *sl, int mask)
{
	return (sl_chk_n_state(sl, ~mask));
}

#define LMF_UNATTACHED	    (1<<LMI_UNATTACHED)
#define LMF_ATTACH_PENDING  (1<<LMI_ATTACH_PENDING)
#define LMF_UNUSABLE	    (1<<LMI_UNUSABLE)
#define LMF_DISABLED	    (1<<LMI_DISABLED)
#define LMF_ENABLE_PENDING  (1<<LMI_ENABLE_PENDING)
#define LMF_ENABLED	    (1<<LMI_ENABLED)
#define LMF_DISABLE_PENDING (1<<LMI_DISABLE_PENDING)
#define LMF_DETACH_PENDING  (1<<LMI_DETACH_PENDING)

#ifdef _DEBUG
const char *
sl_i_state_name(int state)
{
	switch (state) {
	case LMI_UNATTACHED:
		return ("LMI_UNATTACHED");
	case LMI_ATTACH_PENDING:
		return ("LMI_ATTACH_PENDING");
	case LMI_UNUSABLE:
		return ("LMI_UNUSABLE");
	case LMI_DISABLED:
		return ("LMI_DISABLED");
	case LMI_ENABLE_PENDING:
		return ("LMI_ENABLE_PENDING");
	case LMI_ENABLED:
		return ("LMI_ENABLED");
	case LMI_DISABLE_PENDING:
		return ("LMI_DISABLE_PENDING");
	case LMI_DETACH_PENDING:
		return ("LMI_DETACH_PENDING");
	default:
		return ("(unknown)");
	}
}
STATIC int
sl_set_i_state(struct sl *sl, int newstate)
{
	int oldstate = sl->i_state;

	m2palogst(sl, "%s <- %s", sl_i_state_name(newstate), sl_i_state_name(oldstate));
	return ((sl->info.lm.lmi_state = sl->i_state = newstate));
}
#else				/* _DEBUG */
STATIC INLINE int
sl_set_i_state(struct sl *sl, int newstate)
{
	return ((sl->i_state = newstate));
}
#endif				/* _DEBUG */
STATIC INLINE int
sl_get_i_state(struct sl *sl)
{
	return sl->i_state;
}
STATIC INLINE int
sl_get_i_statef(struct sl *sl)
{
	return (1 << sl_get_i_state(sl));
}
STATIC INLINE int
sl_chk_i_state(struct sl *sl, int mask)
{
	return (sl_get_i_statef(sl) & mask);
}
STATIC INLINE int
sl_not_i_state(struct sl *sl, int mask)
{
	return (sl_chk_i_state(sl, ~mask));
}

#define M2PA_VERSION_DRAFT3	0x30
#define M2PA_VERSION_DRAFT3_1	0x31
#define M2PA_VERSION_DRAFT4	0x40
#define M2PA_VERSION_DRAFT4_1	0x41
#define M2PA_VERSION_DRAFT4_9	0x49
#define M2PA_VERSION_DRAFT5	0x50
#define M2PA_VERSION_DRAFT5_1	0x51
#define M2PA_VERSION_DRAFT6	0x60
#define M2PA_VERSION_DRAFT6_1	0x61
#define M2PA_VERSION_DRAFT6_9	0x69
#define M2PA_VERSION_DRAFT10	0xa0
#define M2PA_VERSION_DRAFT11	0xb0
#define M2PA_VERSION_RFC4165	0xc1
#define M2PA_VERSION_DEFAULT	M2PA_VERSION_RFC4165

/*
 *  =========================================================================
 *
 *  M2PA PDU Message Definitions
 *
 *  =========================================================================
 */
#define M2PA_PPI		5
#define M2PA_MESSAGE_CLASS	11
#define M2PA_VERSION		1
#define M2PA_MTYPE_DATA		1
#define M2PA_MTYPE_STATUS	2
#define M2PA_MTYPE_PROVING	3
#define M2PA_MTYPE_ACK		4
#define M2PA_DATA_MESSAGE	\
	__constant_htonl((M2PA_VERSION<<24)|(M2PA_MESSAGE_CLASS<<8)|M2PA_MTYPE_DATA)
#define M2PA_STATUS_MESSAGE	\
	__constant_htonl((M2PA_VERSION<<24)|(M2PA_MESSAGE_CLASS<<8)|M2PA_MTYPE_STATUS)
#define M2PA_PROVING_MESSAGE	\
	__constant_htonl((M2PA_VERSION<<24)|(M2PA_MESSAGE_CLASS<<8)|M2PA_MTYPE_PROVING)
#define M2PA_ACK_MESSAGE	\
	__constant_htonl((M2PA_VERSION<<24)|(M2PA_MESSAGE_CLASS<<8)|M2PA_MTYPE_ACK)
#define M2PA_STATUS_STREAM	0
#define M2PA_DATA_STREAM	1
#define M2PA_STATUS_ALIGNMENT			(__constant_htonl(1))
#define M2PA_STATUS_PROVING_NORMAL		(__constant_htonl(2))
#define M2PA_STATUS_PROVING_EMERGENCY		(__constant_htonl(3))
#define M2PA_STATUS_IN_SERVICE			(__constant_htonl(4))
#define M2PA_STATUS_PROCESSOR_OUTAGE		(__constant_htonl(5))
#define M2PA_STATUS_PROCESSOR_OUTAGE_ENDED	(__constant_htonl(6))
#define M2PA_STATUS_BUSY			(__constant_htonl(7))
#define M2PA_STATUS_BUSY_ENDED			(__constant_htonl(8))
#define	M2PA_STATUS_OUT_OF_SERVICE		(__constant_htonl(9))
#define M2PA_STATUS_ACK				(__constant_htonl(10))

#ifndef PAD4
#define PAD4(__x) (((__x)+0x3)&~0x3)
#endif

/*
 *  =========================================================================
 *
 *  OUTPUT Events
 *
 *  =========================================================================
 */
/*
 *  -------------------------------------------------------------------------
 *
 *  N Provider -> N User Primitives
 *
 *  -------------------------------------------------------------------------
 */
#if 0
/**
 * n_error_ack: - issue an N_ERROR_ACK primitive upstream
 * @sl: private structure
 * @q: active queue
 * @prim: primitive in error
 * @err: positive NPI or negative UNIX error
 */
static int
n_error_ack(struct sl *sl, queue_t *q, int prim, int err)
{
	N_error_ack_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_ERROR_ACK;
		p->ERROR_prim = prim;
		p->NPI_error = err > 0 ? err : NSYSERR;
		p->UNIX_error = err < 0 ? -err : 0;
		mp->b_wptr += sizeof(*p);
		m2palogtx(sl, "<- N_ERROR_ACK");
		/* no state change */
		putnext(sl->oq, mp);
		return (0);
	}
	return (-EBUSY);
}
#endif

static int
n_pass_along(struct sl *sl, queue_t *q, mblk_t *mp, int npistate, int lmistate)
{
	if (mp->b_datap->db_type < QPCTL && !bcanputnext(q, mp->b_band))
		goto busy;
	sl_set_n_state(sl, npistate);
	sl_set_i_state(sl, lmistate);
	sl->n_request = false;
	putnext(q, mp);
	return (QR_ABSORBED);
      busy:
	return (-EBUSY);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SL Provider (M2PA) -> SL User Primitives
 *
 *  -------------------------------------------------------------------------
 */
/**
 * lmi_info_ack: - issue an LMI_INFO_ACK prmitive upstream
 * @sl: private structure
 * @q: active queue
 */
static int
lmi_info_ack(struct sl *sl, queue_t *q)
{
	lmi_info_ack_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p) + sl->loc_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_INFO_ACK;
		p->lmi_version = sl->info.lm.lmi_version;
		p->lmi_state = sl->info.lm.lmi_state;
		p->lmi_max_sdu = sl->info.lm.lmi_max_sdu;
		p->lmi_min_sdu = sl->info.lm.lmi_min_sdu;
		p->lmi_header_len = sl->info.lm.lmi_header_len;
		p->lmi_ppa_style = sl->info.lm.lmi_ppa_style;
		p->lmi_ppa_length = sl->loc_len;
		p->lmi_ppa_offset = sizeof(*p);
		p->lmi_prov_flags = sl->sl.flags;
		p->lmi_prov_state = sl->sl.state;
		mp->b_wptr += sizeof(*p);
		bcopy(&sl->loc, mp->b_wptr, sl->loc_len);
		mp->b_wptr += sl->loc_len;
		m2palogtx(sl, "<- LMI_INFO_ACK");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_ok_ack: - issue an LMI_OK_ACK prmitive upstream
 * @sl: private structure
 * @q: active queue
 * @prim: correct primitive
 */
static int
lmi_ok_ack(struct sl *sl, queue_t *q, lmi_long prim)
{
	lmi_ok_ack_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_OK_ACK;
		p->lmi_correct_primitive = prim;
		switch (sl_get_i_state(sl)) {
		case LMI_ATTACH_PENDING:
			p->lmi_state = LMI_DISABLED;
			sl_set_i_state(sl, LMI_DISABLED);
			sl_set_n_state(sl, NS_IDLE);
			break;
		case LMI_DETACH_PENDING:
			p->lmi_state = LMI_UNATTACHED;
			sl_set_i_state(sl, LMI_UNATTACHED);
			sl_set_n_state(sl, NS_UNBND);
			break;
		default:
			/* default is don't change state */
			p->lmi_state = sl_get_i_state(sl);
			break;
		}
		mp->b_wptr += sizeof(*p);
		m2palogtx(sl, "<- LMI_OK_ACK");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_error_ack: - issue an LMI_ERROR_ACK prmitive upstream
 * @sl: private structure
 * @q: active queue
 * @prim: primitive in error
 * @err: positive LMI error or negative UNIX error
 */
STATIC int
lmi_error_ack(struct sl *sl, queue_t *q, lmi_long prim, lmi_long err)
{
	lmi_error_ack_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_ERROR_ACK;
		p->lmi_errno = err < 0 ? -err : 0;
		p->lmi_reason = err < 0 ? LMI_SYSERR : err;
		p->lmi_error_primitive = prim;
		switch (sl_get_i_state(sl)) {
		case LMI_ATTACH_PENDING:
			p->lmi_state = LMI_UNATTACHED;
			sl_set_i_state(sl, LMI_UNATTACHED);
			sl_set_n_state(sl, NS_UNBND);
			break;
		case LMI_DETACH_PENDING:
		case LMI_ENABLE_PENDING:
			p->lmi_state = LMI_DISABLED;
			sl_set_i_state(sl, LMI_DISABLED);
			sl_set_n_state(sl, NS_IDLE);
			break;
		case LMI_DISABLE_PENDING:
			p->lmi_state = LMI_ENABLED;
			sl_set_i_state(sl, LMI_ENABLED);
			switch (sl_get_n_state(sl)) {
			case NS_WACK_DREQ6:
				sl_set_n_state(sl, NS_WCON_CREQ);
				break;
			case NS_WACK_DREQ7:
				sl_set_n_state(sl, NS_WRES_CIND);
				break;
			case NS_WACK_DREQ9:
				sl_set_n_state(sl, NS_DATA_XFER);
				break;
			case NS_WACK_DREQ10:
				sl_set_n_state(sl, NS_WCON_RREQ);
				break;
			case NS_WACK_DREQ11:
				sl_set_n_state(sl, NS_WRES_RIND);
				break;
			default:
				/* Default is not to change state. */
				m2paloger(sl, "%s() called in incorrect state %d", __FUNCTION__,
					  (int) sl_get_n_state(sl));
				break;
			}
			break;
		}
		mp->b_wptr += sizeof(*p);
		m2palogtx(sl, "<- LMI_ERROR_ACK");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_error_reply: - repy with an LMI_ERROR_ACK prmitive if necessary
 * @sl: private structure
 * @q: active queue
 * @prim: primitive in error
 * @err: positive LMI error or negative UNIX error
 */
static int
lmi_error_reply(struct sl *sl, queue_t *q, lmi_long prim, lmi_long err)
{
	switch (err) {
	case QR_DONE:
	case QR_ABSORBED:
	case QR_TRIMMED:
	case QR_LOOP:
	case QR_PASSALONG:
	case QR_PASSFLOW:
	case QR_DISABLE:
		m2paloger(sl, "Shouldn't pass Q returns to m_error_reply");
	case -EBUSY:
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
		return (err);
	}
	return lmi_error_ack(sl, q, prim, err);
}

/**
 * lmi_enable_con: - issue an LMI_ENABLE_CON primitive upstream
 * @sl: private strucutre
 * @q: active queue
 */
static int
lmi_enable_con(struct sl *sl, queue_t *q)
{
	lmi_enable_con_t *p;
	mblk_t *mp;

	ensure(sl_get_i_state(sl) == LMI_ENABLE_PENDING, return (-EFAULT));
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_ENABLE_CON;
		p->lmi_state = LMI_ENABLED;
		mp->b_wptr += sizeof(*p);
		sl_set_i_state(sl, LMI_ENABLED);
		m2palogtx(sl, "<- LMI_ENABLE_CON");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_disable_con: - issue an LMI_DISABLE_CON primitive upstream
 * @sl: private structure
 * @q: active queue
 */
static int
lmi_disable_con(struct sl *sl, queue_t *q)
{
	lmi_disable_con_t *p;
	mblk_t *mp;

	ensure(sl_get_i_state(sl) == LMI_DISABLE_PENDING, return (-EFAULT));
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_DISABLE_CON;
		p->lmi_state = LMI_DISABLED;
		sl_set_i_state(sl, LMI_DISABLED);
		sl_set_n_state(sl, NS_IDLE);
		mp->b_wptr += sizeof(*p);
		m2palogtx(sl, "<- LMI_DISABLE_CON");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  LMI_OPTMGMT_ACK
 *  ---------------------------------------------
 */
#if 0
STATIC INLINE int
lmi_optmgmt_ack(struct sl *sl, queue_t *q, lmi_ulong flags, void *opt_ptr, size_t opt_len)
{
	mblk_t *mp;
	lmi_optmgmt_ack_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((lmi_optmgmt_ack_t *) mp->b_wptr)++;
		p->lmi_primitive = LMI_OPTMGMT_ACK;
		p->lmi_opt_length = opt_len;
		p->lmi_opt_offset = opt_len ? sizeof(*p) : 0;
		p->lmi_mgmt_flags = flags;
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
		m2palogtx(sl, "<- LMI_OPTMGMT_ACK");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
#endif
/**
 * lmi_error_ind: - issue an LMI_ERROR_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 * @err: postitive LMI or negative UNIX error
 * @fatal: whether error is fatal or not
 */
static int
lmi_error_ind(struct sl *sl, queue_t *q, lmi_long err, int fatal)
{
	mblk_t *mp;
	lmi_error_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_ERROR_IND;
		p->lmi_errno = err < 0 ? -err : 0;
		p->lmi_reason = err < 0 ? LMI_SYSERR : err;
		if (fatal)
			sl_set_i_state(sl, LMI_UNUSABLE);
		p->lmi_state = sl_get_i_state(sl);
		mp->b_wptr += sizeof(*p);
		m2palogtx(sl, "<- LMI_ERROR_IND");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

#if 0
/**
 * lmi_stats_ind: - issue an LMI_STATS_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 * @interval: interval reported
 * @timestamp: timestamp of report
 */
STATIC INLINE int
lmi_stats_ind(struct sl *sl, queue_t *q, lmi_ulong interval, lmi_ulong timestamp)
{
	lmi_stats_ind_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_STATS_IND;
		p->lmi_interval = interval;
		p->lmi_timestamp = timestamp;
		mp->b_wptr += sizeof(*p);
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
#endif

/**
 * lmi_event_ind: - issue an LMI_EVENT_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 * @oid: event id
 * @severity: event severity
 * @inf_ptr: information pointer
 * @inf_len: information length
 */
static int
lmi_event_ind(struct sl *sl, queue_t *q, lmi_ulong oid, lmi_ulong severity, void *inf_ptr,
	      size_t inf_len)
{
	lmi_event_ind_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p) + inf_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_EVENT_IND;
		p->lmi_objectid = oid;
		p->lmi_timestamp = jiffies;
		p->lmi_severity = severity;
		mp->b_wptr += sizeof(*p);
		bcopy(mp->b_wptr, inf_ptr, inf_len);
		mp->b_wptr += inf_len;
		m2palogtx(sl, "<- LMI_EVENT_IND");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_pdu_ind: - issued an SL_PDU_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 * @dp: user data to indicate
 */
static inline fastcall __hot_in int
sl_pdu_ind(struct sl *sl, queue_t *q, mblk_t *dp)
{
	sl_pdu_ind_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_PDU_IND;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = dp;
		m2palogtx(sl, "<- SL_PDU_IND [%d]", (int) msgdsize(mp));
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_link_congested_ind: - issue an SL_LINK_CONGESTED_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 * @cong: congestion status
 * @disc: discard status
 */
static int
sl_link_congested_ind(struct sl *sl, queue_t *q, sl_ulong cong, sl_ulong disc)
{
	sl_link_cong_ind_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_LINK_CONGESTED_IND;
		p->sl_timestamp = jiffies;
		p->sl_cong_status = cong;
		p->sl_disc_status = disc;
		mp->b_wptr += sizeof(*p);
		m2palogtx(sl, "<- SL_LINK_CONGESTED_IND");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_link_congestion_ceased_ind: - issue an SL_LINK_CONGESTION_CEASED_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 * @cong: congestion status
 * @disc: discard status
 */
static int
sl_link_congestion_ceased_ind(struct sl *sl, queue_t *q, sl_ulong cong, sl_ulong disc)
{
	sl_link_cong_ceased_ind_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_LINK_CONGESTION_CEASED_IND;
		p->sl_timestamp = jiffies;
		p->sl_cong_status = cong;
		p->sl_disc_status = disc;
		mp->b_wptr += sizeof(*p);
		m2palogtx(sl, "<- SL_LINK_CONGESTION_CEASED_IND");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_retrieved_message_ind: - issue an SL_RETRIEVED_MESSAGE_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 * @dp: retrieved user data
 */
static int
sl_retrieved_message_ind(struct sl *sl, queue_t *q, mblk_t *dp)
{
	sl_retrieved_msg_ind_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_RETRIEVED_MESSAGE_IND;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = dp;
		m2palogtx(sl, "<- SL_RETRIEVED_MESSAGE_IND");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_retrieval_complete_ind: - issue an SL_RETRIEVAL_COMPLETE_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 */
static int
sl_retrieval_complete_ind(struct sl *sl, queue_t *q)
{
	sl_retrieval_comp_ind_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_RETRIEVAL_COMPLETE_IND;
		mp->b_wptr += sizeof(*p);
		m2palogtx(sl, "<- SL_RETRIEVAL_COMPLETE_IND");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_rb_cleared_ind: - issue an SL_RB_CLEARED_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 */
static int
sl_rb_cleared_ind(struct sl *sl, queue_t *q)
{
	sl_rb_cleared_ind_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_RB_CLEARED_IND;
		mp->b_wptr += sizeof(*p);
		m2palogtx(sl, "<- SL_RB_CLEARED_IND");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_bsnt_ind: - issue an SL_BSNT_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 * @bsnt: value of BSNT to indicate
 */
static int
sl_bsnt_ind(struct sl *sl, queue_t *q, sl_ulong bsnt)
{
	sl_bsnt_ind_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_BSNT_IND;
		p->sl_bsnt = bsnt;
		mp->b_wptr += sizeof(*p);
		m2palogtx(sl, "<- SL_BSNT_IND");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_in_service_ind: - issue an SL_IN_SERVICE_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 */
static int
sl_in_service_ind(struct sl *sl, queue_t *q)
{
	sl_in_service_ind_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_IN_SERVICE_IND;
		m2palogtx(sl, "<- SL_IN_SERVICE_IND");
		mp->b_wptr += sizeof(*p);
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_out_of_service_ind: - issue an SL_OUT_OF_SERVICE_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 * @reason: reason for failure
 */
static int
sl_out_of_service_ind(struct sl *sl, queue_t *q, sl_ulong reason)
{
	sl_out_of_service_ind_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_OUT_OF_SERVICE_IND;
		p->sl_timestamp = jiffies;
		p->sl_reason = reason;
		mp->b_wptr += sizeof(*p);
		m2palogtx(sl, "<- SL_OUT_OF_SERVICE_IND");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_remote_processor_outage_ind: - issue an SL_REMOTE_PROCESSOR_OUTAGE_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 */
static int
sl_remote_processor_outage_ind(struct sl *sl, queue_t *q)
{
	sl_rem_proc_out_ind_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_REMOTE_PROCESSOR_OUTAGE_IND;
		p->sl_timestamp = jiffies;
		mp->b_wptr += sizeof(*p);
		m2palogtx(sl, "<- SL_PROCESSOR_OUTAGE_IND");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_remote_processor_recovered_ind: - issue an SL_REMOTE_PROCESSOR_RECOVERED_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 */
static int
sl_remote_processor_recovered_ind(struct sl *sl, queue_t *q)
{
	sl_rem_proc_recovered_ind_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_REMOTE_PROCESSOR_RECOVERED_IND;
		p->sl_timestamp = jiffies;
		mp->b_wptr += sizeof(*p);
		m2palogtx(sl, "<- SL_REMOTE_PROCESSOR_RECOVERED_IND");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_rtb_cleared_ind: - issue an SL_RTB_CLEARED_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 */
static int
sl_rtb_cleared_ind(struct sl *sl, queue_t *q)
{
	sl_rtb_cleared_ind_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_RTB_CLEARED_IND;
		mp->b_wptr += sizeof(*p);
		m2palogtx(sl, "<- SL_RTB_CLEARED_IND");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_retrieval_not_possible_ind: - issue an SL_RETRIEVAL_NOT_POSSIBLE_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 */
static int
sl_retrieval_not_possible_ind(struct sl *sl, queue_t *q)
{
	sl_retrieval_not_poss_ind_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_RETRIEVAL_NOT_POSSIBLE_IND;
		mp->b_wptr += sizeof(*p);
		m2palogtx(sl, "<- SL_RETRIEVAL_NOT_POSSIBLE_IND");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

#if 0
/**
 * sl_bsnt_not_retrievable_ind: - issue an SL_BSNT_NOT_RETRIEVABLE_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 * @bsnt: BSNT
 */
STATIC INLINE int
sl_bsnt_not_retrievable_ind(struct sl *sl, queue_t *q, sl_ulong bsnt)
{
	sl_bsnt_not_retr_ind_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_BSNT_NOT_RETRIEVABLE_IND;
		p->sl_bsnt = bsnt;
		mp->b_wptr += sizeof(*p);
		m2palogtx(sl, "<- SL_BSNT_NOT_RETRIEVABLE_IND");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
#endif

/*
 *  -------------------------------------------------------------------------
 *
 *  NPI User (M2PA) -> NPI Provider (SCTP) Primitives
 *
 *  -------------------------------------------------------------------------
 */

#if 0
static int
n_pass_down(struct sl *sl, queue_t *q, mblk_t *mp, int npistate, int lmistate)
{
	if (mp->b_datap->db_type < QPCTL && !bcanputnext(q, mp->b_band))
		goto busy;
	sl_set_n_state(sl, npistate);
	sl_set_i_state(sl, lmistate);
	sl->n_request = true;
	putnext(q, mp);
	return (QR_ABSORBED);
      busy:
	return (-EBUSY);
}
#endif

/**
 * n_bind_req: - issue a bind request to fullfill an attach request
 * @sl: private structure
 * @q: active queue
 * @add_ptr: pointer to address to bind
 * @add_len: length of bind address
 */
static int
n_bind_req(struct sl *sl, queue_t *q, caddr_t add_ptr, size_t add_len)
{
	N_bind_req_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_BIND_REQ;
		p->ADDR_length = add_len;
		p->ADDR_offset = add_len ? sizeof(*p) : 0;
		p->CONIND_number = 0;
		p->BIND_flags = TOKEN_REQUEST;
		p->PROTOID_length = 0;
		p->PROTOID_offset = 0;
		mp->b_wptr += sizeof(*p);
		bcopy(add_ptr, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
		sl_set_i_state(sl, LMI_ATTACH_PENDING);
		m2palogtx(sl, "N_BIND_REQ ->");
		putnext(sl->iq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * n_unbind_req: - issue an unbind request to fullfull a detach request
 * @sl: private structure
 * @q: active queue
 */
static int
n_unbind_req(struct sl *sl, queue_t *q)
{
	N_unbind_req_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_UNBIND_REQ;
		mp->b_wptr += sizeof(*p);
		sl_set_i_state(sl, LMI_DETACH_PENDING);
		m2palogtx(sl, "N_UNBIND_REQ ->");
		putnext(sl->iq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * n_conn_req: - issue a connection request to fulfull an enable request
 * @sl: private structure
 * @q: active queue
 * @dst_ptr: destination address
 * @dst_len: destination adresss length
 * @qos_ptr: quality of service parameters
 * @qos_len: quality of service parameters length
 */
static int
n_conn_req(struct sl *sl, queue_t *q, caddr_t dst_ptr, size_t dst_len)
{
	N_qos_sel_conn_sctp_t *n;
	N_conn_req_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p) + dst_len + sizeof(*n), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_CONN_REQ;
		p->DEST_length = dst_len;
		p->DEST_offset = dst_len ? sizeof(*p) : 0;
		p->CONN_flags = REC_CONF_OPT | EX_DATA_OPT;
		p->QOS_length = sizeof(*n);
		p->QOS_offset = sizeof(*p) + dst_len;
		mp->b_wptr += sizeof(*p);
		bcopy(dst_ptr, mp->b_wptr, dst_len);
		mp->b_wptr += dst_len;
		n = (typeof(n)) mp->b_wptr;
		n->n_qos_type = N_QOS_SEL_CONN_SCTP;
		n->i_streams = 2;
		n->o_streams = 2;
		mp->b_wptr += sizeof(*n);
		sl_set_i_state(sl, LMI_ENABLE_PENDING);
		m2palogtx(sl, "N_CONN_REQ ->");
		putnext(sl->iq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * n_discon_req: - issue a disconnection request to fulfill a disable request
 * @sl: private structure
 * @q: active queue
 * @reason: disconnect reason
 */
static int
n_discon_req(struct sl *sl, queue_t *q, np_ulong reason)
{
	N_discon_req_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_DISCON_REQ;
		p->DISCON_reason = reason;
		p->RES_length = 0;
		p->RES_offset = 0;
		p->SEQ_number = 0;
		mp->b_wptr += sizeof(*p);
		sl_set_i_state(sl, LMI_DISABLE_PENDING);
		m2palogtx(sl, "N_DISCON_REQ ->");
		putnext(sl->iq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * n_data_req: - issue a data transfer request
 * @sl: private structure
 * @q: active queue
 * @flags: data transfer flags
 * @qos_ptr: quality of service parameters
 * @qos_len: quality of service parameters length
 * @dp: user data to transfer
 */
static inline fastcall __hot_out int
n_data_req(struct sl *sl, queue_t *q, np_ulong flags, void *qos_ptr, size_t qos_len, mblk_t *dp)
{
	N_data_req_t *p;
	mblk_t *mp;

	if (likely((mp = ss7_allocb(q, sizeof(*p) + qos_len, BPRI_MED)) != NULL)) {
		if (likely(canputnext(sl->iq))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_DATA_REQ;
			p->DATA_xfer_flags = flags;
			mp->b_wptr += sizeof(*p);
			bcopy(qos_ptr, mp->b_wptr, qos_len);
			mp->b_wptr += qos_len;
			mp->b_cont = dp;
			m2palogda(sl, "N_DATA_REQ ->");
			putnext(sl->iq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * n_exdata_req: - issue an expedited data transfer request
 * @sl: private structure
 * @q: active queue
 * @qos_ptr: qualitfy of service parameters
 * @qos_len: qualitfy of service parameters length
 * @dp: user data to transfer
 */
static inline fastcall int
n_exdata_req(struct sl *sl, queue_t *q, void *qos_ptr, size_t qos_len, mblk_t *dp)
{
	N_exdata_req_t *p;
	mblk_t *mp;

	if (likely((mp = ss7_allocb(q, sizeof(*p) + qos_len, BPRI_MED)) != NULL)) {
		mp->b_band = 1;	/* expedite */
		if (likely(bcanputnext(sl->iq, mp->b_band))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_EXDATA_REQ;
			mp->b_wptr += sizeof(*p);
			bcopy(qos_ptr, mp->b_wptr, qos_len);
			mp->b_wptr += qos_len;
			mp->b_cont = dp;
			m2palogda(sl, "N_EXDATA_REQ ->");
			putnext(sl->iq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  =========================================================================
 *
 *  M2PA State Machines
 *
 *  =========================================================================
 */
#define MF_LPO			SLF_LOC_PROC_OUT
#define MF_RPO			SLF_REM_PROC_OUT
#define MF_LOC_INS		SLF_LOC_IN_SERV
#define MF_REM_INS		SLF_REM_IN_SERV
#define MF_LOC_BUSY		SLF_LOC_BUSY
#define MF_REM_BUSY		SLF_REM_BUSY
#define MF_LOC_EMERG		SLF_LOC_EMERG
#define MF_REM_EMERG		SLF_REM_EMERG
#define MF_RECV_MSU		SLF_RECV_MSU
#define MF_SEND_MSU		SLF_SEND_MSU
#define MF_CONG_ACCEPT		SLF_CONG_ACCEPT
#define MF_CONG_DISCARD		SLF_CONG_DISCARD
#define MF_RTB_FULL		SLF_RTB_FULL
#define MF_L3_CONG_DETECT	SLF_L3_CONG_DETECT
#define MF_L2_CONG_DETECT	SLF_L2_CONG_DETECT
#define MF_CONTINUE		SLF_CONTINUE
#define MF_CLEAR_RTB		SLF_CLEAR_RTB
#define MF_NEED_FLUSH		SLF_NEED_FLUSH
#define MF_WAIT_SYNC		SLF_WAIT_SYNC
#define MF_REM_ALN		SLF_REM_ALIGN

#define MS_POWER_OFF		SLS_POWER_OFF
#define MS_OUT_OF_SERVICE	SLS_OUT_OF_SERVICE
#define MS_NOT_ALIGNED		SLS_NOT_ALIGNED
#define MS_ALIGNED		SLS_INITIAL_ALIGNMENT
#define MS_PROVING		SLS_PROVING
#define MS_ALIGNED_NOT_READY	SLS_ALIGNED_NOT_READY
#define MS_ALIGNED_READY	SLS_ALIGNED_READY
#define MS_IN_SERVICE		SLS_IN_SERVICE
#define MS_PROCESSOR_OUTAGE	SLS_PROCESSOR_OUTAGE

#ifdef _DEBUG
const char *
sl_state_name(int state)
{
	switch (state) {
	case MS_POWER_OFF:
		return ("MS_POWER_OFF");
	case MS_OUT_OF_SERVICE:
		return ("MS_OUT_OF_SERVICE");
	case MS_NOT_ALIGNED:
		return ("MS_NOT_ALIGNED");
	case MS_ALIGNED:
		return ("MS_ALIGNED");
	case MS_PROVING:
		return ("MS_PROVING");
	case MS_ALIGNED_NOT_READY:
		return ("MS_ALIGNED_NOT_READY");
	case MS_ALIGNED_READY:
		return ("MS_ALIGNED_READY");
	case MS_IN_SERVICE:
		return ("MS_IN_SERVICE");
	case MS_PROCESSOR_OUTAGE:
		return ("MS_PROCESSOR_OUTAGE");
	default:
		return ("(unknown)");
	}
}
STATIC void
sl_set_state(struct sl *sl, int newstate)
{
	int oldstate = sl->state;

	m2palogst(sl, "%s <- %s", sl_state_name(newstate), sl_state_name(oldstate));
	sl->state = newstate;
}
#else
STATIC INLINE void
sl_set_state(struct sl *sl, int newstate)
{
	sl->state = newstate;
}
#endif
STATIC INLINE int
sl_get_state(struct sl *sl)
{
	return sl->state;
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SL Peer (M2PA) -> SL Peer Sent Messages
 *
 *  -------------------------------------------------------------------------
 */
static const char filler[] = {
	0x6f, 0x43, 0x79, 0x70, 0x69, 0x72, 0x68, 0x67, 0x20, 0x74, 0x63,
	0x28, 0x20, 0x29, 0x39, 0x31, 0x37, 0x39, 0x32, 0x2d, 0x30, 0x30,
	0x20, 0x34, 0x4f, 0x20, 0x65, 0x70, 0x53, 0x6e, 0x37, 0x53, 0x43,
	0x20, 0x72, 0x6f, 0x6f, 0x70, 0x61, 0x72, 0x69, 0x74, 0x6e, 0x6f,
	0x20, 0x2e, 0x41, 0x20, 0x6c, 0x6c, 0x52, 0x20, 0x67, 0x69, 0x74,
	0x68, 0x20, 0x73, 0x65, 0x52, 0x65, 0x73, 0x76, 0x72, 0x64, 0x65,
	0x0a, 0x2e, 0x6e, 0x55, 0x75, 0x61, 0x68, 0x74, 0x72, 0x6f, 0x7a,
	0x69, 0x64, 0x65, 0x64, 0x20, 0x70, 0x75, 0x69, 0x6c, 0x61, 0x63,
	0x69, 0x74, 0x6e, 0x6f, 0x70, 0x20, 0x6f, 0x72, 0x69, 0x68, 0x69,
	0x62, 0x65, 0x74, 0x2e, 0x64, 0x00, 0x0a,
};

/*
 *
 *  M2PA SEND PROVING
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_send_proving(struct sl *sl, queue_t *q, uint32_t status)
{
	int err;
	mblk_t *mp;
	N_qos_sel_data_sctp_t qos = { N_QOS_SEL_DATA_SCTP, M2PA_PPI, M2PA_DATA_STREAM, };

	switch (sl->i_version) {
	case M2PA_VERSION_DRAFT3:
	case M2PA_VERSION_DRAFT3_1:
		if ((mp = ss7_allocb(q, 2 * sizeof(uint32_t) + sizeof(filler), BPRI_MED))) {
			mp->b_datap->db_type = M_DATA;
			*(uint32_t *) mp->b_wptr = M2PA_PROVING_MESSAGE;
			mp->b_wptr += sizeof(uint32_t);
			*(uint32_t *) mp->b_wptr =
			    __constant_htonl(2 * sizeof(uint32_t) + sizeof(filler));
			mp->b_wptr += sizeof(uint32_t);
			bcopy(filler, mp->b_wptr, sizeof(filler));
			mp->b_wptr += sizeof(filler);
			if ((err = n_data_req(sl, q, 0, &qos, sizeof(qos), mp)) >= 0)
				return (0);
			freeb(mp);
			return (err);
		}
		return (-ENOBUFS);
	case M2PA_VERSION_DRAFT4:
	case M2PA_VERSION_DRAFT4_1:
		/* draft-ietf-sigtran-m2pa-04.txt does not have a separate Proving message: it uses 
		   the status proving message for sending proving data will fill bytes. */
		if ((mp = ss7_allocb(q, 4 * sizeof(uint32_t) + sizeof(filler), BPRI_MED))) {
			mp->b_datap->db_type = M_DATA;
			*(uint32_t *) mp->b_wptr = M2PA_STATUS_MESSAGE;
			mp->b_wptr += sizeof(uint32_t);
			*(uint32_t *) mp->b_wptr =
			    __constant_htonl(4 * sizeof(uint32_t) + sizeof(filler));
			mp->b_wptr += sizeof(uint32_t);
			*(uint16_t *) mp->b_wptr = htons(sl->fsnr);
			mp->b_wptr += sizeof(uint16_t);
			*(uint16_t *) mp->b_wptr = htons(sl->fsnt);
			mp->b_wptr += sizeof(uint16_t);
			*(uint32_t *) mp->b_wptr = status;
			mp->b_wptr += sizeof(uint32_t);
			bcopy(filler, mp->b_wptr, sizeof(filler));
			mp->b_wptr += sizeof(filler);
			if ((err = n_data_req(sl, q, 0, &qos, sizeof(qos), mp)) >= 0)
				return (0);
			freeb(mp);
			return (err);
		}
		return (-ENOBUFS);
	case M2PA_VERSION_DRAFT4_9:
	case M2PA_VERSION_DRAFT5:
	case M2PA_VERSION_DRAFT6:
	case M2PA_VERSION_DRAFT6_1:
	default:
		/* It looks like the sequence numbers stuck. */
	case M2PA_VERSION_DRAFT10:
	case M2PA_VERSION_DRAFT11:
	case M2PA_VERSION_RFC4165:
		if ((mp = ss7_allocb(q, 5 * sizeof(uint32_t) + sizeof(filler), BPRI_MED))) {
			mp->b_datap->db_type = M_DATA;
			*(uint32_t *) mp->b_wptr = M2PA_STATUS_MESSAGE;
			mp->b_wptr += sizeof(uint32_t);
			*(uint32_t *) mp->b_wptr =
			    __constant_htonl(5 * sizeof(uint32_t) + sizeof(filler));
			mp->b_wptr += sizeof(uint32_t);
			*(uint32_t *) mp->b_wptr = htonl(sl->fsnr & 0xffffff);
			mp->b_wptr += sizeof(uint32_t);
			*(uint32_t *) mp->b_wptr = htonl(sl->fsnt & 0xffffff);
			mp->b_wptr += sizeof(uint32_t);
			*(uint32_t *) mp->b_wptr = status;
			mp->b_wptr += sizeof(uint32_t);
			bcopy(filler, mp->b_wptr, sizeof(filler));
			mp->b_wptr += sizeof(filler);
			if ((err = n_data_req(sl, q, 0, &qos, sizeof(qos), mp)) >= 0)
				return (0);
			freeb(mp);
			return (err);
		}
		return (-ENOBUFS);
	case M2PA_VERSION_DRAFT6_9:
		/* It is anticipated that draft 7 will not include FSN or BSN in the status
		   messages. */
		if ((mp = ss7_allocb(q, 3 * sizeof(uint32_t) + sizeof(filler), BPRI_MED))) {
			mp->b_datap->db_type = M_DATA;
			*(uint32_t *) mp->b_wptr = M2PA_STATUS_MESSAGE;
			mp->b_wptr += sizeof(uint32_t);
			*(uint32_t *) mp->b_wptr =
			    __constant_htonl(3 * sizeof(uint32_t) + sizeof(filler));
			mp->b_wptr += sizeof(uint32_t);
			*(uint32_t *) mp->b_wptr = status;
			mp->b_wptr += sizeof(uint32_t);
			bcopy(filler, mp->b_wptr, sizeof(filler));
			mp->b_wptr += sizeof(filler);
			if ((err = n_data_req(sl, q, 0, &qos, sizeof(qos), mp)) >= 0)
				return (0);
			freeb(mp);
			return (err);
		}
		return (-ENOBUFS);
	}
}

/*
 *  M2PA SEND STATUS
 *  ---------------------------------------------
 */
STATIC int
sl_send_status(struct sl *sl, queue_t *q, uint32_t status)
{
	int err;
	mblk_t *mp;
	N_qos_sel_data_sctp_t qos = { N_QOS_SEL_DATA_SCTP, M2PA_PPI, M2PA_STATUS_STREAM, };

#ifdef _DEBUG
	switch (status) {
	case M2PA_STATUS_BUSY_ENDED:
		m2palogtx(sl, "BUSY-ENDED ->");
		break;
	case M2PA_STATUS_IN_SERVICE:
		m2palogtx(sl, "IN-SERVICE ->");
		break;
	case M2PA_STATUS_PROCESSOR_OUTAGE_ENDED:
		m2palogtx(sl, "PROCESSOR-ENDED ->");
		break;
	case M2PA_STATUS_OUT_OF_SERVICE:
		m2palogtx(sl, "OUT-OF-SERVICE ->");
		break;
	case M2PA_STATUS_PROCESSOR_OUTAGE:
		m2palogtx(sl, "PROCESSOR-OUTAGE ->");
		break;
	case M2PA_STATUS_BUSY:
		m2palogtx(sl, "BUSY ->");
		break;
	case M2PA_STATUS_ALIGNMENT:
		m2palogtx(sl, "ALIGNMENT ->");
		break;
	case M2PA_STATUS_PROVING_NORMAL:
		m2palogtx(sl, "PROVING-NORMAL ->");
		break;
	case M2PA_STATUS_PROVING_EMERGENCY:
		m2palogtx(sl, "PROVING-EMERGENCY ->");
		break;
	case M2PA_STATUS_ACK:
		m2palogtx(sl, "ACK ->");
		break;
	default:
		m2palogtx(sl, "???? ->");
		break;
	}
#endif
	switch (sl->i_version) {
	case M2PA_VERSION_DRAFT3:
	case M2PA_VERSION_DRAFT3_1:
		if ((mp = ss7_allocb(q, 3 * sizeof(uint32_t), BPRI_MED))) {
			mp->b_datap->db_type = M_DATA;
			*(uint32_t *) mp->b_wptr = M2PA_STATUS_MESSAGE;
			mp->b_wptr += sizeof(uint32_t);
			*(uint32_t *) mp->b_wptr = __constant_htonl(3 * sizeof(uint32_t));
			mp->b_wptr += sizeof(uint32_t);
			*(uint32_t *) mp->b_wptr = status;
			mp->b_wptr += sizeof(uint32_t);
			if ((err = n_data_req(sl, q, 0, &qos, sizeof(qos), mp)) >= 0)
				return (0);
			freeb(mp);
			return (err);
		}
		return (-ENOBUFS);
	case M2PA_VERSION_DRAFT4:
	case M2PA_VERSION_DRAFT4_1:
		/* draft-ietf-sigtran-m2pa-04.txt has sequence numbers in an M2PA-specific header. */
		if ((mp = ss7_allocb(q, 4 * sizeof(uint32_t), BPRI_MED))) {
			mp->b_datap->db_type = M_DATA;
			*(uint32_t *) mp->b_wptr = M2PA_STATUS_MESSAGE;
			mp->b_wptr += sizeof(uint32_t);
			*(uint32_t *) mp->b_wptr = __constant_htonl(4 * sizeof(uint32_t));
			mp->b_wptr += sizeof(uint32_t);
			*(uint16_t *) mp->b_wptr = htons(sl->fsnr);
			mp->b_wptr += sizeof(uint16_t);
			*(uint16_t *) mp->b_wptr = htons(sl->fsnt);
			mp->b_wptr += sizeof(uint16_t);
			*(uint32_t *) mp->b_wptr = status;
			mp->b_wptr += sizeof(uint32_t);
			if ((err = n_data_req(sl, q, 0, &qos, sizeof(qos), mp)) >= 0)
				return (0);
			freeb(mp);
			return (err);
		}
		return (-ENOBUFS);
	case M2PA_VERSION_DRAFT4_9:
	case M2PA_VERSION_DRAFT5:
	case M2PA_VERSION_DRAFT6:
	case M2PA_VERSION_DRAFT6_1:
		if ((mp = ss7_allocb(q, 5 * sizeof(uint32_t), BPRI_MED))) {
			mp->b_datap->db_type = M_DATA;
			*(uint32_t *) mp->b_wptr = M2PA_STATUS_MESSAGE;
			mp->b_wptr += sizeof(uint32_t);
			*(uint32_t *) mp->b_wptr = __constant_htonl(5 * sizeof(uint32_t));
			mp->b_wptr += sizeof(uint32_t);
			*(uint32_t *) mp->b_wptr = htonl(sl->fsnr & 0xffffff);
			mp->b_wptr += sizeof(uint32_t);
			*(uint32_t *) mp->b_wptr = htonl(sl->fsnt & 0xffffff);
			mp->b_wptr += sizeof(uint32_t);
			*(uint32_t *) mp->b_wptr = status;
			mp->b_wptr += sizeof(uint32_t);
			if ((err = n_data_req(sl, q, 0, &qos, sizeof(qos), mp)) >= 0)
				return (0);
			freeb(mp);
			return (err);
		}
		return (-ENOBUFS);
	case M2PA_VERSION_DRAFT6_9:
		/* It is anticipated that draft 7 will not include FSN or BSN in the status
		   messages. */
		if ((mp = ss7_allocb(q, 3 * sizeof(uint32_t), BPRI_MED))) {
			mp->b_datap->db_type = M_DATA;
			*(uint32_t *) mp->b_wptr = M2PA_STATUS_MESSAGE;
			mp->b_wptr += sizeof(uint32_t);
			*(uint32_t *) mp->b_wptr = __constant_htonl(3 * sizeof(uint32_t));
			mp->b_wptr += sizeof(uint32_t);
			*(uint32_t *) mp->b_wptr = status;
			mp->b_wptr += sizeof(uint32_t);
			if ((err = n_data_req(sl, q, 0, &qos, sizeof(qos), mp)) >= 0)
				return (0);
			freeb(mp);
			return (err);
		}
		return (-ENOBUFS);
	default:
	case M2PA_VERSION_DRAFT10:
	case M2PA_VERSION_DRAFT11:
	case M2PA_VERSION_RFC4165:
		if ((mp = ss7_allocb(q, 5 * sizeof(uint32_t), BPRI_MED))) {
			mp->b_datap->db_type = M_DATA;
			*(uint32_t *) mp->b_wptr = M2PA_STATUS_MESSAGE;
			mp->b_wptr += sizeof(uint32_t);
			*(uint32_t *) mp->b_wptr = __constant_htonl(5 * sizeof(uint32_t));
			mp->b_wptr += sizeof(uint32_t);
			*(uint32_t *) mp->b_wptr = htonl(sl->fsnr & 0xffffff);
			mp->b_wptr += sizeof(uint32_t);
			*(uint32_t *) mp->b_wptr = htonl(sl->fsnt & 0xffffff);
			mp->b_wptr += sizeof(uint32_t);
			*(uint32_t *) mp->b_wptr = status;
			mp->b_wptr += sizeof(uint32_t);
			switch (status) {
			case M2PA_STATUS_PROCESSOR_OUTAGE:
			case M2PA_STATUS_PROCESSOR_OUTAGE_ENDED:
				qos.sid = M2PA_DATA_STREAM;
				break;
			case M2PA_STATUS_IN_SERVICE:
				switch (sl_get_state(sl)) {
				case MS_PROCESSOR_OUTAGE:
					/* Should only really be sent on data stream when used for
					   processor outage resynchronization. */
					qos.sid = M2PA_DATA_STREAM;
					break;
				default:
					qos.sid = M2PA_STATUS_STREAM;
					break;
				}
				break;
			default:
				qos.sid = M2PA_STATUS_STREAM;
				break;
			}
			if ((err = n_data_req(sl, q, 0, &qos, sizeof(qos), mp)) >= 0)
				return (0);
			freeb(mp);
			return (err);
		}
		return (-ENOBUFS);
	}
}

/*
 *  M2PA SEND ACK
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_send_ack(struct sl *sl, queue_t *q)
{
	int err;
	mblk_t *mp;
	N_qos_sel_data_sctp_t qos = { N_QOS_SEL_DATA_SCTP, M2PA_PPI, M2PA_DATA_STREAM, };

	if (!sl->rack)
		return (0);
	m2palogda(sl, "ACK ->");
	switch (sl->i_version) {
	case M2PA_VERSION_DRAFT3:
		/* draft-ietf-sigtran-m2pa-03.txt does not have a separate acknowledgement message. 
		   Acknowledgement is done at the SCTP level only. */
		return (0);
	case M2PA_VERSION_DRAFT3_1:
		if ((mp = ss7_allocb(q, 3 * sizeof(uint32_t), BPRI_MED))) {
			mp->b_datap->db_type = M_DATA;
			*(uint32_t *) mp->b_wptr = M2PA_ACK_MESSAGE;
			mp->b_wptr += sizeof(uint32_t);
			*(uint32_t *) mp->b_wptr = __constant_htonl(3 * sizeof(uint32_t));
			mp->b_wptr += sizeof(uint32_t);
			*(uint32_t *) mp->b_wptr = htonl(sl->rack);
			mp->b_wptr += sizeof(uint32_t);
			if ((err = n_exdata_req(sl, q, &qos, sizeof(qos), mp)) >= 0) {
				sl->rack = 0;
				return (0);
			}
			freeb(mp);
			return (err);
		}
		return (-ENOBUFS);
	case M2PA_VERSION_DRAFT4:
		/* draft-ietf-sigtran-m2pa-04.txt again does not have a separate acknowledgement
		   message.  There is no way to acknowledge data without sending status or data in
		   M2PA-04. */
		return (0);
	case M2PA_VERSION_DRAFT4_1:
		/* This extension uses the otherwise useless "In Service" state message from
		   draft-ietf-sigtran-m2pa-04.txt as an acknowledgement message (like a FISU).
		   These messages are sent expedited to speed them up, but that means they may
		   arrive out of order and, therefore, some sequence numbers should be ignored by
		   the receiver. */
		if ((mp = ss7_allocb(q, 4 * sizeof(uint32_t), BPRI_MED))) {
			mp->b_datap->db_type = M_DATA;
			*(uint32_t *) mp->b_wptr = M2PA_STATUS_MESSAGE;
			mp->b_wptr += sizeof(uint32_t);
			*(uint32_t *) mp->b_wptr = __constant_htonl(4 * sizeof(uint32_t));
			mp->b_wptr += sizeof(uint32_t);
			*(uint16_t *) mp->b_wptr = htons(sl->fsnr);
			mp->b_wptr += sizeof(uint16_t);
			*(uint16_t *) mp->b_wptr = htons(sl->fsnt);
			mp->b_wptr += sizeof(uint16_t);
			*(uint32_t *) mp->b_wptr = M2PA_STATUS_ACK;
			mp->b_wptr += sizeof(uint32_t);
			if ((err = n_exdata_req(sl, q, &qos, sizeof(qos), mp)) >= 0) {
				sl->rack = 0;
				return (0);
			}
			freeb(mp);
			return (err);
		}
		return (-ENOBUFS);
	case M2PA_VERSION_DRAFT4_9:
		/* This is the same as extension 4.1, except that it uses the larger 32-bit
		   sequence numbers expected to appear in draft-ietf-sigtran-m2pa-05.txt. */
		if ((mp = ss7_allocb(q, 5 * sizeof(uint32_t), BPRI_MED))) {
			mp->b_datap->db_type = M_DATA;
			*(uint32_t *) mp->b_wptr = M2PA_STATUS_MESSAGE;
			mp->b_wptr += sizeof(uint32_t);
			*(uint32_t *) mp->b_wptr = __constant_htonl(5 * sizeof(uint32_t));
			mp->b_wptr += sizeof(uint32_t);
			*(uint32_t *) mp->b_wptr = htonl(sl->fsnr);
			mp->b_wptr += sizeof(uint32_t);
			*(uint32_t *) mp->b_wptr = htonl(sl->fsnt);
			mp->b_wptr += sizeof(uint32_t);
			*(uint32_t *) mp->b_wptr = M2PA_STATUS_ACK;
			mp->b_wptr += sizeof(uint32_t);
			if ((err = n_exdata_req(sl, q, &qos, sizeof(qos), mp)) >= 0) {
				sl->rack = 0;
				return (0);
			}
			freeb(mp);
			return (err);
		}
		return (-ENOBUFS);
	case M2PA_VERSION_DRAFT5:
	case M2PA_VERSION_DRAFT5_1:
		/* Draft 5 allows acknowledgement with DATA as well as STATUS IN_SERVICE message.
		   Draft 5 incorrectly indicates that the FSN is to be incremented. */
		if (sl->tb.q_msgs && (sl->flags & MF_SEND_MSU) && !(sl->flags & MF_RTB_FULL)) {
			m2palogda(sl, "Delaying ack for piggyback");
			return (QR_DONE);	/* piggyback ack on outgoing message */
		}
		m2palogda(sl, "Sending separate ack");
		if ((mp = ss7_allocb(q, 5 * sizeof(uint32_t), BPRI_MED))) {
			mp->b_datap->db_type = M_DATA;
			*(uint32_t *) mp->b_wptr = M2PA_STATUS_MESSAGE;
			mp->b_wptr += sizeof(uint32_t);
			*(uint32_t *) mp->b_wptr = __constant_htonl(5 * sizeof(uint32_t));
			mp->b_wptr += sizeof(uint32_t);
			*(uint32_t *) mp->b_wptr = htonl(sl->fsnr & 0xffffff);
			mp->b_wptr += sizeof(uint32_t);
			*(uint32_t *) mp->b_wptr = htonl(sl->fsnt & 0xffffff);
			mp->b_wptr += sizeof(uint32_t);
			*(uint32_t *) mp->b_wptr = M2PA_STATUS_IN_SERVICE;
			mp->b_wptr += sizeof(uint32_t);
			if ((err = n_exdata_req(sl, q, &qos, sizeof(qos), mp)) >= 0) {
				sl->rack = 0;
				return (0);
			}
			freeb(mp);
			return (err);
		}
		return (-ENOBUFS);
	case M2PA_VERSION_DRAFT6:
	case M2PA_VERSION_DRAFT6_1:
	case M2PA_VERSION_DRAFT6_9:
		/* draft-ietf-signtran-m2pa-06.txt and the anticipated draft 7 wil acknowledge
		   messages with an empty data message.  Draft 6 incorrectly indicatest that the
		   FSN is to be incremented. */
		if (sl->tb.q_msgs && (sl->flags & MF_SEND_MSU) && !(sl->flags & MF_RTB_FULL)) {
			m2palogda(sl, "Delaying ack for piggyback");
			return (QR_DONE);	/* piggyback ack on outgoing message */
		}
		m2palogda(sl, "Sending separate ack");
		if ((mp = ss7_allocb(q, 4 * sizeof(uint32_t), BPRI_MED))) {
			mp->b_datap->db_type = M_DATA;
			*(uint32_t *) mp->b_wptr = M2PA_DATA_MESSAGE;
			mp->b_wptr += sizeof(uint32_t);
			*(uint32_t *) mp->b_wptr = __constant_htonl(4 * sizeof(uint32_t));
			mp->b_wptr += sizeof(uint32_t);
			*(uint32_t *) mp->b_wptr = htonl(sl->fsnr & 0xffffff);
			mp->b_wptr += sizeof(uint32_t);
			*(uint32_t *) mp->b_wptr = htonl(sl->fsnt & 0xffffff);
			mp->b_wptr += sizeof(uint32_t);
			if ((err = n_data_req(sl, q, 0, &qos, sizeof(qos), mp)) >= 0) {
				sl->rack = 0;
				return (0);
			}
			freemsg(mp);
			return (err);
		}
		return (-ENOBUFS);
	default:
	case M2PA_VERSION_DRAFT10:
	case M2PA_VERSION_DRAFT11:
	case M2PA_VERSION_RFC4165:
		/* Acknowledgements must be sent immediately on the next outgoing message.  We
		   check to see if there is a pending MSU waiting to be sent that will actually be
		   sent during this pass of the state machine.  If there is one, we will piggyback
		   so we return now.  If there is not an MSU that will be sent on this pass, we
		   send the separate ack.  Note that we do not delay whatsoever waiting for an MSU: 
		   either one is ready now or we will generate a separate ack immediately.  Let
		   SCTP worry about Nagling and bundling. */
		if (sl->tb.q_msgs && (sl->flags & MF_SEND_MSU) && !(sl->flags & MF_RTB_FULL)) {
			m2palogda(sl, "Delaying ack for piggyback");
			return (QR_DONE);	/* piggyback ack on outgoing message */
		}
		m2palogda(sl, "Sending separate ack");
		if ((mp = ss7_allocb(q, 4 * sizeof(uint32_t), BPRI_MED))) {
			mp->b_datap->db_type = M_DATA;
			*(uint32_t *) mp->b_wptr = M2PA_DATA_MESSAGE;
			mp->b_wptr += sizeof(uint32_t);
			*(uint32_t *) mp->b_wptr = __constant_htonl(4 * sizeof(uint32_t));
			mp->b_wptr += sizeof(uint32_t);
			*(uint32_t *) mp->b_wptr = htonl(sl->fsnr & 0xffffff);
			mp->b_wptr += sizeof(uint32_t);
			*(uint32_t *) mp->b_wptr = htonl(sl->fsnt & 0xffffff);
			mp->b_wptr += sizeof(uint32_t);
			if ((err = n_data_req(sl, q, 0, &qos, sizeof(qos), mp)) >= 0) {
				sl->rack = 0;
				return (0);
			}
			freemsg(mp);
			return (err);
		}
		return (-ENOBUFS);
	}
}

/*
 *  M2PA SEND DATA
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_send_data(struct sl *sl, queue_t *q, mblk_t *dp)
{
	int err;
	mblk_t *mp, *b;
	int rcpt = sl->i_version < M2PA_VERSION_DRAFT3_1 ? N_RC_FLAG : 0;
	size_t blen, dlen = msgdsize(dp);
	N_qos_sel_data_sctp_t qos = { N_QOS_SEL_DATA_SCTP, M2PA_PPI, M2PA_DATA_STREAM, };

	m2palogda(sl, "DATA ->");
	switch (sl->i_version) {
	case M2PA_VERSION_DRAFT3:
	case M2PA_VERSION_DRAFT3_1:
		if ((mp = ss7_allocb(q, dlen + 2 * sizeof(uint32_t), BPRI_MED))) {
			mp->b_datap->db_type = M_DATA;
			*(uint32_t *) mp->b_wptr = M2PA_DATA_MESSAGE;
			mp->b_wptr += sizeof(uint32_t);
			*(uint32_t *) mp->b_wptr = htonl(dlen + 2 * sizeof(uint32_t));
			mp->b_wptr += sizeof(uint32_t);
			/* copy it in */
			for (b = dp; b; b = b->b_next) {
				if (b->b_wptr <= b->b_rptr)
					continue;
				blen = b->b_wptr - b->b_rptr;
				if (blen > dlen)
					blen = dlen;
				bcopy(b->b_rptr, mp->b_wptr, blen);
				mp->b_wptr += blen;
				dlen -= blen;
			}
			if ((err = n_data_req(sl, q, rcpt, &qos, sizeof(qos), mp)) >= 0)
				return (0);
			freemsg(mp);
			return (err);
		}
		break;
	case M2PA_VERSION_DRAFT4:
	case M2PA_VERSION_DRAFT4_1:
		if ((mp = ss7_allocb(q, dlen + 3 * sizeof(uint32_t), BPRI_MED))) {
			mp->b_datap->db_type = M_DATA;
			*(uint32_t *) mp->b_wptr = M2PA_DATA_MESSAGE;
			mp->b_wptr += sizeof(uint32_t);
			*(uint32_t *) mp->b_wptr = htonl(dlen + 3 * sizeof(uint32_t));
			mp->b_wptr += sizeof(uint32_t);
			*(uint16_t *) mp->b_wptr = htons(sl->fsnr);
			mp->b_wptr += sizeof(uint16_t);
			*(uint16_t *) mp->b_wptr = htons(sl->fsnt + 1);
			mp->b_wptr += sizeof(uint16_t);
			/* copy it in */
			for (b = dp; b; b = b->b_next) {
				if (b->b_wptr <= b->b_rptr)
					continue;
				blen = b->b_wptr - b->b_rptr;
				if (blen > dlen)
					blen = dlen;
				bcopy(b->b_rptr, mp->b_wptr, blen);
				mp->b_wptr += blen;
				dlen -= blen;
			}
			if ((err = n_data_req(sl, q, rcpt, &qos, sizeof(qos), mp)) >= 0)
				return (0);
			freemsg(mp);
			return (err);
		}
		break;
	case M2PA_VERSION_DRAFT4_9:
	case M2PA_VERSION_DRAFT5:
	case M2PA_VERSION_DRAFT6:
	case M2PA_VERSION_DRAFT6_1:
	case M2PA_VERSION_DRAFT6_9:
	default:
	case M2PA_VERSION_DRAFT10:
	case M2PA_VERSION_DRAFT11:
	case M2PA_VERSION_RFC4165:
		if ((mp = ss7_allocb(q, dlen + 4 * sizeof(uint32_t), BPRI_MED))) {
			mp->b_datap->db_type = M_DATA;
			*(uint32_t *) mp->b_wptr = M2PA_DATA_MESSAGE;
			mp->b_wptr += sizeof(uint32_t);
			*(uint32_t *) mp->b_wptr = htonl(dlen + 4 * sizeof(uint32_t));
			mp->b_wptr += sizeof(uint32_t);
			*(uint32_t *) mp->b_wptr = htonl(sl->fsnr & 0xffffff);
			mp->b_wptr += sizeof(uint32_t);
			*(uint32_t *) mp->b_wptr = htonl((sl->fsnt + 1) & 0xffffff);
			mp->b_wptr += sizeof(uint32_t);
			/* copy it in */
			for (b = dp; b; b = b->b_next) {
				if (b->b_wptr <= b->b_rptr)
					continue;
				blen = b->b_wptr - b->b_rptr;
				if (blen > dlen)
					blen = dlen;
				bcopy(b->b_rptr, mp->b_wptr, blen);
				mp->b_wptr += blen;
				dlen -= blen;
			}
			if ((err = n_data_req(sl, q, rcpt, &qos, sizeof(qos), mp)) >= 0)
				return (0);
			freemsg(mp);
			return (err);
		}
		break;
	}
	return (-ENOBUFS);
}

/*
 *  ------------------------------------------------------------------------
 *
 *  Timers
 *
 *  ------------------------------------------------------------------------
 */
enum { tall, t1, t2, t3, t4, t5, t6, t7, t8, t9 };

static noinline fastcall void
sl_stop_timer_t1(struct sl *sl)
{
	m2palogte(sl, "-> T1 STOP <-");
	mi_timer_stop(sl->sl.timers.t1);
}
static noinline fastcall void
sl_start_timer_t1(struct sl *sl)
{
	m2palogte(sl, "-> T1 START <- (%d msec)", (int) sl->sl.config.t1);
	mi_timer(sl->sl.timers.t1, sl->sl.config.t1);
}
static noinline fastcall void
sl_stop_timer_t2(struct sl *sl)
{
	m2palogte(sl, "-> T2 STOP <-");
	mi_timer_stop(sl->sl.timers.t2);
}
static noinline fastcall void
sl_start_timer_t2(struct sl *sl)
{
	m2palogte(sl, "-> T2 START <- (%d msec)", (int) sl->sl.config.t2);
	mi_timer(sl->sl.timers.t2, sl->sl.config.t2);
}
static noinline fastcall void
sl_stop_timer_t3(struct sl *sl)
{
	m2palogte(sl, "-> T3 STOP <-");
	mi_timer_stop(sl->sl.timers.t3);
}
static noinline fastcall void
sl_start_timer_t3(struct sl *sl)
{
	m2palogte(sl, "-> T3 START <- (%d msec)", (int) sl->sl.config.t3);
	mi_timer(sl->sl.timers.t3, sl->sl.config.t3);
}
static noinline fastcall void
sl_stop_timer_t4(struct sl *sl)
{
	m2palogte(sl, "-> T4 STOP <-");
	mi_timer_stop(sl->sl.timers.t4);
}
static noinline fastcall void
sl_start_timer_t4(struct sl *sl)
{
	sl_ulong t4v =
	    (sl->flags & (MF_LOC_EMERG | MF_REM_EMERG)) ? sl->sl.config.t4e : sl->sl.config.t4n;
	m2palogte(sl, "-> T4 START <- (%d msec)", (int) t4v);
	mi_timer(sl->sl.timers.t4, t4v);
}

#if 0
static noinline fastcall void
sl_stop_timer_t5(struct sl *sl)
{
	m2palogte(sl, "-> T5 STOP <-");
	mi_timer_stop(sl->sl.timers.t5);
}
static noinline fastcall void
sl_start_timer_t5(struct sl *sl)
{
	m2palogte(sl, "-> T5 START <- (%d msec)", (int) sl->sl.config.t5);
	mi_timer(sl->sl.timers.t5, sl->sl.config.t5);
}
#endif
static noinline fastcall void
sl_stop_timer_t6(struct sl *sl)
{
	m2palogte(sl, "-> T6 STOP <-");
	mi_timer_stop(sl->sl.timers.t6);
}
static noinline fastcall void
sl_start_timer_t6(struct sl *sl)
{
	m2palogte(sl, "-> T6 START <- (%d msec)", (int) sl->sl.config.t6);
	mi_timer(sl->sl.timers.t6, sl->sl.config.t6);
}
static noinline fastcall void
sl_stop_timer_t7(struct sl *sl)
{
	m2palogte(sl, "-> T7 STOP <-");
	mi_timer_stop(sl->sl.timers.t7);
}
static noinline fastcall void
sl_start_timer_t7(struct sl *sl)
{
	m2palogte(sl, "-> T7 START <- (%d msec)", (int) sl->sl.config.t7);
	mi_timer(sl->sl.timers.t7, sl->sl.config.t7);
}

#if 0
static noinline fastcall void
sl_stop_timer_t8(struct sl *sl)
{
	m2palogte(sl, "-> T8 STOP <-");
	mi_timer_stop(sl->sdt.timers.t8);
}
static noinline fastcall void
sl_start_timer_t8(struct sl *sl)
{
	m2palogte(sl, "-> T8 START <- (%d msec)", (int) sl->sdt.config.t8);
	mi_timer(sl->sdt.timers.t8, sl->sdt.config.t8);
}
#endif
static noinline fastcall void
sl_stop_timer_t9(struct sl *sl)
{
	m2palogte(sl, "-> T9 STOP <-");
	mi_timer_stop(sl->sdl.timers.t9);
}
static noinline fastcall void
sl_start_timer_t9(struct sl *sl)
{
	m2palogte(sl, "-> T9 START <- (%d msec)", (int) 20);
	mi_timer(sl->sdl.timers.t9, 20);
}

STATIC inline fastcall __hot void
sl_timer_stop(struct sl *sl, const uint t)
{
	int single = 1;

	switch (t) {
	case tall:
		single = 0;
		/* fall through */
	case t1:
		sl_stop_timer_t1(sl);
		if (single)
			break;
		/* fall through */
	case t2:
		sl_stop_timer_t2(sl);
		if (single)
			break;
		/* fall through */
	case t3:
		sl_stop_timer_t3(sl);
		if (single)
			break;
		/* fall through */
	case t4:
		sl_stop_timer_t4(sl);
		if (single)
			break;
		/* fall through */
#if 0
	case t5:
		sl_stop_timer_t5(sl);
		if (single)
			break;
		/* fall through */
#endif
	case t6:
		sl_stop_timer_t6(sl);
		if (single)
			break;
		/* fall through */
	case t7:
		sl_stop_timer_t7(sl);
		if (single)
			break;
		/* fall through */
#if 0
	case t8:
		sl_stop_timer_t8(sl);
		if (single)
			break;
		/* fall through */
#endif
	case t9:
		sl_stop_timer_t9(sl);
		if (single)
			break;
		/* fall through */
		break;
	default:
		m2paloger(sl, "%s() called with invalid timer number %d", __FUNCTION__, (int) t);
		break;
	}
}
STATIC inline fastcall __hot void
sl_timer_start(struct sl *sl, const uint t)
{
	sl_timer_stop(sl, t);
	switch (t) {
	case t1:
		sl_start_timer_t1(sl);
		break;
	case t2:
		sl_start_timer_t2(sl);
		break;
	case t3:
		sl_start_timer_t3(sl);
		break;
	case t4:
		sl_start_timer_t4(sl);
		break;
#if 0
	case t5:
		sl_start_timer_t5(sl);
		break;
#endif
	case t6:
		sl_start_timer_t6(sl);
		break;
	case t7:
		sl_start_timer_t7(sl);
		break;
#if 0
	case t8:
		sl_start_timer_t8(sl);
		break;
#endif
	case t9:
		sl_start_timer_t9(sl);
		break;
	default:
		m2paloger(sl, "%s() called with invalid timer number %d", __FUNCTION__, (int) t);
		break;
	}
}

STATIC noinline __unlikely void
sl_free_timers(struct sl *sl)
{
	mblk_t *tp;

	if ((tp = XCHG(&sl->sl.timers.t1, NULL)))
		mi_timer_free(tp);
	if ((tp = XCHG(&sl->sl.timers.t2, NULL)))
		mi_timer_free(tp);
	if ((tp = XCHG(&sl->sl.timers.t3, NULL)))
		mi_timer_free(tp);
	if ((tp = XCHG(&sl->sl.timers.t4, NULL)))
		mi_timer_free(tp);
#if 0
	if ((tp = XCHG(&sl->sl.timers.t5, NULL)))
		mi_timer_free(tp);
#endif
	if ((tp = XCHG(&sl->sl.timers.t6, NULL)))
		mi_timer_free(tp);
	if ((tp = XCHG(&sl->sl.timers.t7, NULL)))
		mi_timer_free(tp);
#if 0
	if ((tp = XCHG(&sl->sdt.timers.t8, NULL)))
		mi_timer_free(tp);
#endif
	if ((tp = XCHG(&sl->sdl.timers.t9, NULL)))
		mi_timer_free(tp);
}

STATIC noinline __unlikely int
sl_alloc_timers(struct sl *sl)
{
	mblk_t *tp;

	/* SDL timer allocation */
	if (!(tp = sl->sdl.timers.t9 = mi_timer_alloc_MAC(sl->oq, sizeof(int))))
		goto enobufs;
	*(int *) tp->b_rptr = t9;
#if 0
	/* SDT timer allocation */
	if (!(tp = sl->sdt.timers.t8 = mi_timer_alloc_MAC(sl->oq, sizeof(int))))
		goto enobufs;
	*(int *) tp->b_rptr = t8;
#endif
	/* SL timer allocation */
	if (!(tp = sl->sl.timers.t7 = mi_timer_alloc_MAC(sl->oq, sizeof(int))))
		goto enobufs;
	*(int *) tp->b_rptr = t7;
	if (!(tp = sl->sl.timers.t6 = mi_timer_alloc_MAC(sl->oq, sizeof(int))))
		goto enobufs;
	*(int *) tp->b_rptr = t6;
#if 0
	if (!(tp = sl->sl.timers.t5 = mi_timer_alloc_MAC(sl->oq, sizeof(int))))
		goto enobufs;
	*(int *) tp->b_rptr = t5;
#endif
	if (!(tp = sl->sl.timers.t4 = mi_timer_alloc_MAC(sl->oq, sizeof(int))))
		goto enobufs;
	*(int *) tp->b_rptr = t4;
	if (!(tp = sl->sl.timers.t3 = mi_timer_alloc_MAC(sl->oq, sizeof(int))))
		goto enobufs;
	*(int *) tp->b_rptr = t3;
	if (!(tp = sl->sl.timers.t2 = mi_timer_alloc_MAC(sl->oq, sizeof(int))))
		goto enobufs;
	*(int *) tp->b_rptr = t2;
	if (!(tp = sl->sl.timers.t1 = mi_timer_alloc_MAC(sl->oq, sizeof(int))))
		goto enobufs;
	*(int *) tp->b_rptr = t1;
	return (0);
      enobufs:
	sl_free_timers(sl);
	return (-ENOBUFS);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Error Monitoring
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE void
sl_suerm_start(struct sl *sl, queue_t *q)
{
	/* SCTP does this for us */
	return;
}

STATIC INLINE void
sl_suerm_stop(struct sl *sl, queue_t *q)
{
	/* SCTP does this for us */
	return;
}

STATIC INLINE void
sl_aerm_start(struct sl *sl, queue_t *q)
{
	m2palogst(sl, "Start Proving...");
#ifndef _DEBUG
	/* strapped out when debugging: too many console messages */
	sl_timer_start(sl, t9);
#endif
}

STATIC INLINE void
sl_aerm_stop(struct sl *sl, queue_t *q)
{
	sl_timer_stop(sl, t9);
	m2palogst(sl, "Stop Proving...");
}

STATIC int
sl_t9_timeout(struct sl *sl, queue_t *q)
{
	int proving = !(sl->option.popt & SS7_POPT_NOPR);

	if (sl_get_state(sl) == MS_PROVING) {
		int err;

		/* We use the t9 tick timer to determine how fast to send proving messages.  We
		   send a proving message once every t9 during the proving period.  To adjust the
		   bandwidth of the proving messages, adjust the t9 timer or the size of the
		   filler. */
		if (proving) {
			if (sl->flags & MF_LOC_EMERG)
				err = sl_send_proving(sl, q, M2PA_STATUS_PROVING_EMERGENCY);
			else
				err = sl_send_proving(sl, q, M2PA_STATUS_PROVING_NORMAL);
			if (err)
				return (err);
			sl_timer_start(sl, t9);
		}
		return (0);
	}
	m2paloger(sl, "Received timeout t9 in uexpected state %d", (int) sl_get_state(sl));
	return (-EPROTO);
}

STATIC int sl_lsc_out_of_service(struct sl *sl, queue_t *q, uint reason);

#if 0
STATIC INLINE int
sl_iac_abort_proving(struct sl *sl, queue_t *q)
{
	int err;

	if (sl_get_state(sl) == MS_PROVING) {
		sl->sl.statem.Cp++;
		sl_timer_stop(sl, t4);
		sl_aerm_stop(sl, q);
		if (sl->sl.statem.Cp == sl->sl.config.M) {
			if (sl->sl.notify.events & SL_EVT_FAIL_ALIGNMENT)
				if ((err =
				     lmi_event_ind(sl, q, SL_EVT_FAIL_ALIGNMENT, 0, NULL, 0)) < 0)
					return (err);
			if ((err =
			     sl_lsc_out_of_service(sl, q, SL_FAIL_ALIGNMENT_NOT_POSSIBLE)) < 0)
				return (err);
			m2palogst(sl, "Link failed: Alignement not possible");
			sl->sl.stats.sl_fail_align_or_proving++;
			return (0);
		}
		sl_timer_start(sl, t4);
		sl_aerm_start(sl, q);
		return (0);
	}
	m2paloger(sl, "Received signal iac-abort-proving in unexpected state %d",
		  (int) sl_get_state(sl));
	return (-EFAULT);
}
#endif

/*
 *  -----------------------------------------------------------------------
 *
 *  STATE MACHINES:- The order of the state machine primitives below may seem
 *  somewhat disorganized at first glance; however, they have been ordered by
 *  dependency because they are all inline functions.  You see, the L2 state
 *  machine does not required multiple threading because there is never a
 *  requirement to invoke the individual state machines concurrently.  This
 *  works out good for the driver, because a primitive action expands inline
 *  to the necessary procedure, while the source still takes the appearance of
 *  the SDL diagrams in the SS7 specification for inspection and debugging.
 *
 *  -----------------------------------------------------------------------
 */
/*
 *  -----------------------------------------------------------------------
 *
 *  Duration Statistics
 *
 *  -----------------------------------------------------------------------
 */
STATIC void
sl_is_stats(struct sl *sl, queue_t *q)
{
	if (sl->sl.stamp.sl_dur_unavail)
		sl->sl.stats.sl_dur_unavail += jiffies - xchg(&sl->sl.stamp.sl_dur_unavail, 0);
	if (sl->sl.stamp.sl_dur_unavail_rpo)
		sl->sl.stats.sl_dur_unavail_rpo +=
		    jiffies - xchg(&sl->sl.stamp.sl_dur_unavail_rpo, 0);
	if (sl->sl.stamp.sl_dur_unavail_failed)
		sl->sl.stats.sl_dur_unavail_failed +=
		    jiffies - xchg(&sl->sl.stamp.sl_dur_unavail_failed, 0);
	sl->sl.stamp.sl_dur_in_service = jiffies;
}
STATIC void
sl_oos_stats(struct sl *sl, queue_t *q)
{
	if (sl->sl.stamp.sl_dur_in_service)
		sl->sl.stats.sl_dur_in_service +=
		    jiffies - xchg(&sl->sl.stamp.sl_dur_in_service, 0);
	if (sl->sl.stamp.sl_dur_unavail_rpo)
		sl->sl.stats.sl_dur_unavail_rpo +=
		    jiffies - xchg(&sl->sl.stamp.sl_dur_unavail_rpo, 0);
	if (sl->sl.stamp.sl_dur_unavail_failed)
		sl->sl.stats.sl_dur_unavail_failed +=
		    jiffies - xchg(&sl->sl.stamp.sl_dur_unavail_failed, 0);
	sl->sl.stamp.sl_dur_unavail = jiffies;
}
STATIC void
sl_rpo_stats(struct sl *sl, queue_t *q)
{
	if (sl->sl.stamp.sl_dur_in_service)
		sl->sl.stats.sl_dur_in_service +=
		    jiffies - xchg(&sl->sl.stamp.sl_dur_in_service, 0);
	sl->sl.stamp.sl_dur_unavail_rpo = jiffies;
}
STATIC void
sl_rpr_stats(struct sl *sl, queue_t *q)
{
	if (sl->sl.stamp.sl_dur_unavail_rpo)
		sl->sl.stats.sl_dur_unavail_rpo +=
		    jiffies - xchg(&sl->sl.stamp.sl_dur_unavail_rpo, 0);
}
STATIC int
sl_lsc_stop(struct sl *sl, queue_t *q)
{
	int err;

	if (sl_get_state(sl) > MS_OUT_OF_SERVICE) {
		if ((err = sl_send_status(sl, q, M2PA_STATUS_OUT_OF_SERVICE)) < 0)
			return (err);
		sl_timer_stop(sl, tall);
		sl_aerm_stop(sl, q);
		sl_suerm_stop(sl, q);
		sl->flags &= ~MF_LPO;
		if (sl->flags & MF_RPO) {
			if (sl->sl.notify.events & SL_EVT_RPO_END)
				if ((err = lmi_event_ind(sl, q, SL_EVT_RPO_END, 0, NULL, 0)) < 0)
					return (err);
			m2palogst(sl, "RPO Ends");
			sl_rpr_stats(sl, q);
			sl->flags &= ~MF_RPO;
		}
		sl->flags &= ~MF_LOC_INS;
		sl->flags &= ~MF_REM_INS;
		sl->flags &= ~MF_LOC_BUSY;
		sl->flags &= ~MF_REM_BUSY;
		sl->flags &= ~MF_LOC_EMERG;
		sl->flags &= ~MF_REM_EMERG;
		sl_oos_stats(sl, q);
		sl_set_state(sl, MS_OUT_OF_SERVICE);
		return (0);
	}
	return (0);
}
STATIC int
sl_lsc_out_of_service(struct sl *sl, queue_t *q, uint reason)
{
	int err;

	if (sl_get_state(sl) > MS_OUT_OF_SERVICE) {
		if ((err = sl_out_of_service_ind(sl, q, reason)) < 0)
			return (err);
		if ((err = sl_lsc_stop(sl, q)) < 0)
			return (err);
		sl->sl.stamp.sl_dur_unavail_failed = jiffies;
		return (0);
	}
	return (0);
}
STATIC INLINE int
sl_lsc_power_on(struct sl *sl, queue_t *q)
{
	int err;

	if (sl_get_state(sl) == MS_POWER_OFF) {
		sl->rmsu = 0;
		sl->rack = 0;
		sl->fsnr = 0xffffff;
		sl->bsnr = 0xffffff;
		sl->fsnx = 0;
		sl->tmsu = 0;
		sl->tack = 0;
		sl->fsnt = 0xffffff;
		sl->back = 0;
		sl->bbsn = 0;
		sl->bfsn = 0;
		sl->flags = 0;
		if ((err = sl_send_status(sl, q, M2PA_STATUS_OUT_OF_SERVICE)) < 0)
			return (err);
		sl_set_state(sl, MS_OUT_OF_SERVICE);
		return (0);
	}
	m2paloger(sl, "Received primitive SL_POWER_ON_REQ in unexpected state %d",
		  (int) sl_get_state(sl));
	return (-EPROTO);
}
STATIC int sl_ready(struct sl *sl, queue_t *q);
STATIC INLINE int
sl_lsc_start(struct sl *sl, queue_t *q)
{
	int err;
	int proving = !(sl->option.popt & SS7_POPT_NOPR);

	if (sl_get_state(sl) == MS_OUT_OF_SERVICE) {
		sl->rmsu = 0;
		sl->rack = 0;
		sl->fsnr = 0xffffff;
		sl->bsnr = 0xffffff;
		sl->fsnx = 0;
		sl->tmsu = 0;
		sl->tack = 0;
		sl->fsnt = 0xffffff;
		sl->back = 0;
		sl->bbsn = 0;
		sl->bfsn = 0;
		// sl->flags = 0;
		if ((err = sl_send_status(sl, q, M2PA_STATUS_ALIGNMENT)) < 0)
			return (err);
		if (!(sl->flags & MF_REM_ALN)) {
			sl_timer_start(sl, t2);
			sl_set_state(sl, MS_NOT_ALIGNED);
			return (QR_DONE);
		} else {
			if (proving) {
				if (sl->flags & MF_LOC_EMERG)
					err = sl_send_proving(sl, q, M2PA_STATUS_PROVING_EMERGENCY);
				else
					err = sl_send_proving(sl, q, M2PA_STATUS_PROVING_NORMAL);
				if (err)
					return (err);
				sl_timer_start(sl, t3);
				sl_set_state(sl, MS_ALIGNED);
				return (QR_DONE);
			}
			return sl_ready(sl, q);
		}
	}
	m2paloger(sl, "Received primitive SL_START_REQ in unexpected state %d",
		  (int) sl_get_state(sl));
	return (-EPROTO);
}
STATIC int
sl_t2_timeout(struct sl *sl, queue_t *q)
{
	int err;

	if (sl_get_state(sl) == MS_NOT_ALIGNED) {
		if (sl->sl.notify.events & SL_EVT_FAIL_ALIGNMENT)
			if ((err = lmi_event_ind(sl, q, SL_EVT_FAIL_ALIGNMENT, 0, NULL, 0)) < 0)
				return (err);
		if ((err = sl_lsc_out_of_service(sl, q, SL_FAIL_ALIGNMENT_NOT_POSSIBLE)) < 0)
			return (err);
		m2palogst(sl, "Link failed: Alignment not possible");
		sl->sl.stats.sl_fail_align_or_proving++;
		return (0);
	}
	m2paloger(sl, "Received timeout t2 in uexpected state %d", (int) sl_get_state(sl));
	return (-EPROTO);
}

STATIC int
sl_ready(struct sl *sl, queue_t *q)
{
	int err;

	sl->flags &= ~MF_LOC_EMERG;
	sl->flags &= ~MF_REM_EMERG;
	sl_suerm_start(sl, NULL);
	sl->flags |= MF_LOC_INS;
	if (sl->flags & MF_LPO) {
		if ((err = sl_send_status(sl, NULL, M2PA_STATUS_PROCESSOR_OUTAGE)) < 0)
			return (err);
		if (sl->flags & MF_REM_INS) {
			if ((err = sl_in_service_ind(sl, NULL)) < 0)
				return (err);
			sl->flags &= ~MF_SEND_MSU;
			sl->flags &= ~MF_RECV_MSU;
			sl_oos_stats(sl, NULL);
			sl_set_state(sl, MS_PROCESSOR_OUTAGE);
		} else {
			sl_timer_start(sl, t1);
			sl_set_state(sl, MS_ALIGNED_NOT_READY);
		}
	} else {
		mblk_t *mp;

		if ((mp = sl->tb.q_head)) {
			if ((err = sl_send_data(sl, NULL, mp)) < 0)
				return (err);
			sl->tmsu++;
			sl->fsnt++;
			sl->fsnt &= 0xffffff;
			bufq_queue(&sl->rtb, bufq_dequeue(&sl->tb));
			sl->tmsu--;
			sl->tack++;
			sl->sl.stats.sl_tran_msus++;
			sl->sl.stats.sl_tran_sio_sif_octets += mp->b_wptr - mp->b_rptr - 1;
			if (sl->rtb.q_msgs >= sl->sl.config.N1
			    || sl->rtb.q_count >= sl->sl.config.N2
			    || sl->tmsu + sl->tack > 0x7ffffff)
				sl->flags |= MF_RTB_FULL;
		} else {
			if ((err = sl_send_status(sl, NULL, M2PA_STATUS_IN_SERVICE)) < 0)
				return (err);
		}
		if (sl->flags & MF_REM_INS) {
			if ((err = sl_in_service_ind(sl, NULL)) < 0)
				return (err);
			sl->flags |= MF_SEND_MSU;
			sl->flags |= MF_RECV_MSU;
			sl_is_stats(sl, NULL);
			sl_set_state(sl, MS_IN_SERVICE);
		} else if (sl->flags & MF_RPO) {
			if ((err = sl_remote_processor_outage_ind(sl, q)) < 0)
				return (err);
			sl->flags &= ~MF_SEND_MSU;
			sl->flags &= ~MF_RECV_MSU;
			sl_set_state(sl, MS_PROCESSOR_OUTAGE);
		} else {
			sl_timer_start(sl, t1);
			sl_set_state(sl, MS_ALIGNED_READY);
		}
	}
	return (0);
}

STATIC INLINE int
sl_lsc_status_alignment(struct sl *sl, queue_t *q)
{
	int err;
	int proving = !(sl->option.popt & SS7_POPT_NOPR);

	sl->flags |= MF_REM_ALN;
	switch (sl_get_state(sl)) {
	case MS_POWER_OFF:
	case MS_OUT_OF_SERVICE:
		return (QR_DONE);
	case MS_NOT_ALIGNED:
		sl_timer_stop(sl, t2);
		if (proving) {
			if (sl->flags & MF_LOC_EMERG)
				err = sl_send_proving(sl, q, M2PA_STATUS_PROVING_EMERGENCY);
			else
				err = sl_send_proving(sl, q, M2PA_STATUS_PROVING_NORMAL);
			if (err)
				return (err);
			sl_timer_start(sl, t3);
			sl_set_state(sl, MS_ALIGNED);
			return (QR_DONE);
		}
	case MS_ALIGNED:
		if (proving)
			return (QR_DONE);
		return sl_ready(sl, q);
	case MS_PROVING:
		sl_aerm_stop(sl, q);
		sl_timer_stop(sl, t4);
		sl_timer_start(sl, t3);
		sl_set_state(sl, MS_ALIGNED);
		return (QR_DONE);
	case MS_ALIGNED_READY:
		if (!proving)
			return (QR_DONE);
		/* fall through */
	default:
		if (sl->sl.notify.events & SL_EVT_FAIL_RECEIVED_SIO)
			if ((err = lmi_event_ind(sl, q, SL_EVT_FAIL_RECEIVED_SIO, 0, NULL, 0)) < 0)
				return (err);
		if ((err = sl_lsc_out_of_service(sl, q, SL_FAIL_RECEIVED_SIO)) < 0)
			return (err);
		m2palogst(sl, "Link failed: Received SIO");
		return (QR_DONE);
	}
}

STATIC INLINE int
sl_lsc_emergency(struct sl *sl, queue_t *q)
{
	int err;
	int proving = !(sl->option.popt & SS7_POPT_NOPR);

	sl->flags |= MF_LOC_EMERG;
	if (proving) {
		switch (sl_get_state(sl)) {
		case MS_PROVING:
			/* For a normal Q.703 link, when we received an emergency signal in the
			   proving state, we would stop sending normal proving messages and being
			   sending emergency proving messages.  The problem is if we have already
			   primed the SCTP transmit queue with normal messages the change would be
			   difficult.  MTP2 normally paces output of SIN so that the switch to SIE
			   can be made at any time.  The only way that we can change mid-burst is
			   to send proving messages in smaller chunks or on some sort of timed
			   basis. */
			if ((err = sl_send_proving(sl, q, M2PA_STATUS_PROVING_EMERGENCY)) < 0)
				return (err);
			sl_timer_stop(sl, t4);
			sl_aerm_stop(sl, q);
			sl_timer_start(sl, t4);
			sl_aerm_start(sl, q);
			return (0);
		case MS_ALIGNED:
			if ((err = sl_send_proving(sl, q, M2PA_STATUS_PROVING_EMERGENCY)) < 0)
				return (err);
			return (0);
		}
	}
	m2paloger(sl, "Received primitive SL_EMERGENCY_REQ in unexpected state %d",
		  (int) sl_get_state(sl));
	return (-EPROTO);
}

STATIC INLINE int
sl_lsc_emergency_ceases(struct sl *sl, queue_t *q)
{
	sl->flags &= ~MF_LOC_EMERG;
	return (0);
}

STATIC INLINE int
sl_lsc_status_proving_normal(struct sl *sl, queue_t *q)
{
	int err;
	int proving = !(sl->option.popt & SS7_POPT_NOPR);

	sl->flags &= ~MF_REM_ALN;
	switch (sl_get_state(sl)) {
	case MS_NOT_ALIGNED:
		sl_timer_stop(sl, t2);
		if (proving) {
			if (sl->flags & MF_LOC_EMERG)
				err = sl_send_proving(sl, q, M2PA_STATUS_PROVING_EMERGENCY);
			else
				err = sl_send_proving(sl, q, M2PA_STATUS_PROVING_NORMAL);
			if (err)
				return (err);
			sl_timer_start(sl, t3);
			sl_set_state(sl, MS_ALIGNED);
		}
		/* fall thru */
	case MS_ALIGNED:
		sl->flags &= ~MF_REM_EMERG;
		if (proving) {
			sl_timer_stop(sl, t3);
			sl_aerm_start(sl, q);
			sl_timer_start(sl, t4);
			sl->sl.statem.Cp = 0;
			sl_set_state(sl, MS_PROVING);
			return (QR_DONE);
		}
		return sl_ready(sl, q);
	case MS_IN_SERVICE:
	case MS_PROCESSOR_OUTAGE:
		if (sl->sl.notify.events & SL_EVT_FAIL_RECEIVED_SIN)
			if ((err = lmi_event_ind(sl, q, SL_EVT_FAIL_RECEIVED_SIN, 0, NULL, 0)) < 0)
				return (err);
		if ((err = sl_lsc_out_of_service(sl, q, SL_FAIL_RECEIVED_SIN)) < 0)
			return (err);
		m2palogst(sl, "Link failed: Received SIN");
		return (QR_DONE);
	}
	m2paloger(sl, "Received status PROVING_NORMAL in unexpected state %d",
		  (int) sl_get_state(sl));
	return (-EPROTO);
}

STATIC INLINE int
sl_lsc_status_proving_emergency(struct sl *sl, queue_t *q)
{
	int err;
	int proving = !(sl->option.popt & SS7_POPT_NOPR);

	sl->flags &= ~MF_REM_ALN;
	switch (sl_get_state(sl)) {
	case MS_NOT_ALIGNED:
		sl_timer_stop(sl, t2);
		if (proving) {
			if (sl->flags & MF_LOC_EMERG)
				err = sl_send_proving(sl, q, M2PA_STATUS_PROVING_EMERGENCY);
			else
				err = sl_send_proving(sl, q, M2PA_STATUS_PROVING_NORMAL);
			if (err)
				return (err);
			sl_timer_start(sl, t3);
			sl_set_state(sl, MS_ALIGNED);
		}
		/* fall thru */
	case MS_ALIGNED:
		if (proving) {
			sl->flags |= MF_REM_EMERG;
			sl_timer_stop(sl, t3);
			sl_aerm_start(sl, q);
			sl_timer_start(sl, t4);
			sl->sl.statem.Cp = 0;
			sl_set_state(sl, MS_PROVING);
			return (QR_DONE);
		}
		return sl_ready(sl, q);
	case MS_PROVING:
		if (sl->flags & MF_REM_EMERG)
			return (QR_DONE);
		if (sl->flags & MF_LOC_EMERG) {
			sl->flags |= MF_REM_EMERG;
			return (QR_DONE);
		}
		sl->flags |= MF_REM_EMERG;
		sl_timer_stop(sl, t4);
		sl_aerm_stop(sl, q);
		sl_aerm_start(sl, q);
		sl_timer_start(sl, t4);
		sl->sl.statem.further_proving = 0;
		return (QR_DONE);
	case MS_IN_SERVICE:
	case MS_PROCESSOR_OUTAGE:
		if (sl->sl.notify.events & SL_EVT_FAIL_RECEIVED_SIE)
			if ((err = lmi_event_ind(sl, q, SL_EVT_FAIL_RECEIVED_SIE, 0, NULL, 0)) < 0)
				return (err);
		if ((err = sl_lsc_out_of_service(sl, q, SL_FAIL_RECEIVED_SIE)) < 0)
			return (err);
		m2palogst(sl, "Link failed: Received SIE");
		return (QR_DONE);
	}
	m2paloger(sl, "Received status PROVING_EMERGENCY in unexpected state %d",
		  (int) sl_get_state(sl));
	return (-EPROTO);
}

STATIC int
sl_t1_timeout(struct sl *sl, queue_t *q)
{
	int err;

	if (sl->sl.notify.events & SL_EVT_FAIL_T1_TIMEOUT)
		if ((err = lmi_event_ind(sl, q, SL_EVT_FAIL_T1_TIMEOUT, 0, NULL, 0)) < 0)
			return (err);
	if ((err = sl_lsc_out_of_service(sl, q, SL_FAIL_T1_TIMEOUT)) < 0)
		return (err);
	m2palogst(sl, "Link failed: T1 Timeout");
	sl->sl.stats.sl_fail_align_or_proving++;
	return (0);
}

STATIC int
sl_t3_timeout(struct sl *sl, queue_t *q)
{
	int err;

	if (sl_get_state(sl) == MS_ALIGNED) {
		if (sl->sl.notify.events & SL_EVT_FAIL_ALIGNMENT)
			if ((err = lmi_event_ind(sl, q, SL_EVT_FAIL_ALIGNMENT, 0, NULL, 0)) < 0)
				return (err);
		if ((err = sl_lsc_out_of_service(sl, q, SL_FAIL_ALIGNMENT_NOT_POSSIBLE)) < 0)
			return (err);
		m2palogst(sl, "Link failed: T3 Timeout");
		sl->sl.stats.sl_fail_align_or_proving++;
		return (0);
	}
	m2paloger(sl, "Received timeout t3 in uexpected state %d", (int) sl_get_state(sl));
	return (-EPROTO);
}

STATIC int
sl_t4_timeout(struct sl *sl, queue_t *q)
{
	int err;

	if (sl_get_state(sl) == MS_PROVING) {
		sl_aerm_stop(sl, q);
		if ((err = sl_ready(sl, q)))
			return (err);
		return (0);
	}
	m2paloger(sl, "Received timeout t4 in unexpected state %d", (int) sl_get_state(sl));
	return (-EPROTO);
}

STATIC void streamscall sl_tx_wakeup(queue_t *q);
STATIC INLINE int
sl_lsc_status_in_service(struct sl *sl, queue_t *q, mblk_t *mp)
{
	int err;
	int proving = !(sl->option.popt & SS7_POPT_NOPR);

	sl->flags &= ~MF_REM_ALN;
	switch (sl_get_state(sl)) {
	case MS_NOT_ALIGNED:
		if (proving)
			break;
		sl_timer_stop(sl, t2);
		sl->flags |= MF_REM_INS;
		return sl_ready(sl, q);
	case MS_ALIGNED:
	case MS_PROVING:
		sl->flags |= MF_REM_INS;
		return (QR_DONE);
	case MS_ALIGNED_NOT_READY:
		if ((err = sl_in_service_ind(sl, q)) < 0)
			return (err);
		sl_timer_stop(sl, t1);
		sl->flags &= ~MF_SEND_MSU;
		sl->flags &= ~MF_RECV_MSU;
		sl_oos_stats(sl, q);
		sl_set_state(sl, MS_PROCESSOR_OUTAGE);
		return (QR_DONE);
	case MS_ALIGNED_READY:
		if ((err = sl_in_service_ind(sl, q)) < 0)
			return (err);
		sl_timer_stop(sl, t1);
		sl->flags |= MF_SEND_MSU;
		sl->flags |= MF_RECV_MSU;
		sl_is_stats(sl, q);
		sl_set_state(sl, MS_IN_SERVICE);
		return (QR_DONE);
	case MS_IN_SERVICE:
	case MS_PROCESSOR_OUTAGE:
		if (sl->i_version >= M2PA_VERSION_DRAFT10) {
			/* In the final, when we receive a READY in the in service or processor
			   outage states, we use the BSN in the message to synchronize when
			   synchronization was required. */
			if (sl->flags & MF_WAIT_SYNC) {
				sl->fsnt = ntohl(*((uint32_t *) mp->b_rptr)) & 0xffffff;
				sl->flags &= ~MF_WAIT_SYNC;
				sl->flags |= MF_SEND_MSU;
				sl_tx_wakeup(q);
			}
		}
	case MS_POWER_OFF:
	case MS_OUT_OF_SERVICE:
		m2paloger(sl, "Received status IN_SERVICE in unusual state %d",
			  (int) sl_get_state(sl));
		return (QR_DONE);
	}
	m2paloger(sl, "Received status IN_SERVICE in unexpected state %d", (int) sl_get_state(sl));
	return (-EPROTO);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  RECEIVE CONGESTION ALGORITHM
 *
 *  --------------------------------------------------------------------------
 *
 *  These congestion functions are implementation dependent.  We should define
 *  a congestion onset level and set congestion accept at that point.  We
 *  should also define a second congestion onset level and set congestion
 *  discard at that point.  For STREAMS, the upstream congestion can be
 *  detected in two ways: 1) canputnext(): is the upstream module flow
 *  controlled; and, 2) canput(): are we flow controlled.  If the upstream
 *  module is flow controlled, then we can accept MSUs and place them on our
 *  own read queue.  If we are flow contolled, then we have no choice but to
 *  discard the message.  In addition, and because upstream message processing
 *  times are likely more sensitive to the number of backlogged messages than
 *  they are to the number of backlogged message octets, we have some
 *  configurable thresholds of backlogging and keep track of backlogged
 *  messages.
 *
 *  --------------------------------------------------------------------------
 */
STATIC INLINE int
sl_rb_congestion_function(struct sl *sl, queue_t *q)
{
	int err;

	if (!(sl->flags & MF_L3_CONG_DETECT)) {
		if (sl->flags & MF_L2_CONG_DETECT) {
			if (sl->rb.q_msgs <= sl->sl.config.rb_abate && canputnext(sl->oq)) {
				if (sl->flags & MF_LOC_BUSY)
					if ((err =
					     sl_send_status(sl, q, M2PA_STATUS_BUSY_ENDED)) < 0)
						return (err);
				sl->flags &= ~MF_LOC_BUSY;
				sl->flags &= ~MF_CONG_DISCARD;
				sl->flags &= ~MF_CONG_ACCEPT;
				sl->rack += xchg(&sl->rmsu, 0);
				sl->flags &= ~MF_L2_CONG_DETECT;
			}
		} else {
			if (sl->rb.q_msgs >= sl->sl.config.rb_discard || !canput(sl->oq)) {
				sl->flags |= MF_CONG_DISCARD;
				if (!(sl->flags & MF_LOC_BUSY)) {
					if ((err = sl_send_status(sl, q, M2PA_STATUS_BUSY)) < 0)
						return (err);
					m2palogda(sl, "Receive congestion: congestion discard");
					m2palogda(sl, "  sl->rb.q_msgs            = %12d",
						  (int) sl->rb.q_msgs);
					m2palogda(sl, "  sl->sl.config.rb_discard = %12d",
						  (int) sl->sl.config.rb_discard);
					m2palogda(sl, "  canput                   = %12s",
						  canput(sl->oq) ? "YES" : "NO");
					sl->sl.stats.sl_sibs_sent++;
					sl->flags |= MF_LOC_BUSY;
				}
				sl->flags |= MF_L2_CONG_DETECT;
			} else {
				if (sl->rb.q_msgs >= sl->sl.config.rb_accept || !canputnext(sl->oq)) {
					sl->flags |= MF_CONG_ACCEPT;
					if (!(sl->flags & MF_LOC_BUSY)) {
						if ((err =
						     sl_send_status(sl, q, M2PA_STATUS_BUSY)) < 0)
							return (err);
						m2palogda(sl,
							  "Receive congestion: congestion accept");
						m2palogda(sl, "  sl->rb.q_msgs            = %12d",
							  (int) sl->rb.q_msgs);
						m2palogda(sl, "  sl->sl.config.rb_accept  = %12d",
							  (int) sl->sl.config.rb_accept);
						m2palogda(sl, "  cantputnext?             = %12s",
							  canputnext(sl->oq) ? "YES" : "NO");
						sl->sl.stats.sl_sibs_sent++;
						sl->flags |= MF_LOC_BUSY;
					}
					sl->flags |= MF_L2_CONG_DETECT;
				}
			}
		}
	}
	return (0);
}

STATIC streamscall void
sl_rx_wakeup(queue_t *q)
{
	struct sl *sl = SL_PRIV(q);
	int err;
	mblk_t *mp;

	ensure(q, return);
	switch (sl_get_state(sl)) {
	default:
		if (sl->flags & MF_RECV_MSU && sl->rb.q_msgs) {
			while ((mp = bufq_dequeue(&sl->rb))) {
				if ((err = sl_pdu_ind(sl, q, mp)) < 0) {
					bufq_queue_head(&sl->rb, mp);
					rare();
					return;
				}
				sl->sl.stats.sl_recv_msus++;
				sl->sl.stats.sl_recv_sio_sif_octets += mp->b_wptr - mp->b_rptr - 1;
				sl->rmsu++;
				sl->fsnr++;
				sl->fsnr &= 0xffffff;
				sl_rb_congestion_function(sl, q);
				if (!(sl->flags & MF_CONG_ACCEPT))
					sl->rack += xchg(&sl->rmsu, 0);
			}
			if (sl->rack)
				sl_send_ack(sl, q);
		}
	case MS_POWER_OFF:
	case MS_OUT_OF_SERVICE:
		return;
	}
}

/*
 *  Sequence number comparisons:
 */
/*
   is s2<=s1<=s3 
 */
#define between32(__s1,__s2,__s3) ((uint32_t)(__s3)-(uint32_t)(__s2)>=(uint32_t)(__s1)-(uint32_t)(__s2))
#define outside32(__s1,__s2,__s3) between32(__s1,__s3,__s2)
#define before32(__s1,__s2) (((int32_t)((uint32_t)(__s1)-(uint32_t)(__s2))<0))
#define after32(__s1,__s2) (((int32_t)((uint32_t)(__s2)-(uint32_t)(__s1))<0))
#define diff32(__s1,__s2) (before32(__s1,__s2) ? \
		((int32_t)((uint32_t)(__s2)-(uint32_t)(__s1))) : \
		((int32_t)((uint32_t)(__s1)-(uint32_t)(__s2))))
/*
   is s2<=s1<=s3 
 */
/* XXX: think these through again */
#define between24(__s1,__s2,__s3) ((((uint32_t)(__s3)-(uint32_t)(__s2))&0xffffff)>=(((uint32_t)(__s1)-(uint32_t)(__s2))&0xffffff))
#define outside24(__s1,__s2,__s3) between24(__s1,__s3,__s2)
#define before24(__s1,__s2) (((uint32_t)(__s1)-(uint32_t)(__s2))&0x800000)
#define after24(__s1,__s2) ((__s1)!=(__s2)&&!before24(__s1,__s2))
#define diff24(__s1,__s2) ((before24(__s1,__s2) ? \
		((int32_t)((uint32_t)(__s2)-(uint32_t)(__s1))) : \
		((int32_t)((uint32_t)(__s1)-(uint32_t)(__s2))))&0xffffff)

/*
   is s2<=s1<=s3 
 */
#define between16(__s1,__s2,__s3) ((uint16_t)(__s3)-(uint16_t)(__s2)>=(uint16_t)(__s1)-(uint16_t)(__s2))
#define outside16(__s1,__s2,__s3) between16(__s1,__s3,__s2)
#define before16(__s1,__s2) (((int16_t)((uint16_t)(__s1)-(uint16_t)(__s2))<0))
#define after16(__s1,__s2) (((int16_t)((uint16_t)(__s2)-(uint16_t)(__s1))<0))
#define diff16(__s1,__s2) (before16(__s1,__s2) ? \
		((int16_t)((uint16_t)(__s2)-(uint16_t)(__s1))) : \
		((int16_t)((uint16_t)(__s1)-(uint16_t)(__s2))))

STATIC int sl_check_congestion(struct sl *sl, queue_t *q);

STATIC INLINE int
sl_txc_datack(struct sl *sl, queue_t *q, uint count)
{
	int err;

	if (!count)
		m2paloger(sl, "ack called with zero count");
	switch (sl_get_state(sl)) {
	case MS_IN_SERVICE:
	case MS_PROCESSOR_OUTAGE:
	case MS_ALIGNED_READY:
	case MS_ALIGNED_NOT_READY:
		while (count--) {
			if (sl->rtb.q_msgs) {
				freemsg(bufq_dequeue(&sl->rtb));
				sl->tack--;
				qenable(sl->iq);
			} else {
				if (sl->back) {
					if (sl->sl.notify.events & SL_EVT_FAIL_ABNORMAL_BSNR)
						if ((err =
						     lmi_event_ind(sl, q, SL_EVT_FAIL_ABNORMAL_BSNR,
								   0, NULL, 0)) < 0)
							return (err);
					if ((err =
					     sl_lsc_out_of_service(sl, q,
								   SL_FAIL_ABNORMAL_BSNR)) < 0)
						return (err);
					m2palogst(sl, "Link failed: Abnormal BSNR");
					return (0);
				}
				m2palogno(sl, "Received bad acknowledgement acks = %d",
					  (int) (count + 1));
				sl->back++;
				return (0);
			}
		}
		sl->back = 0;
		if (sl->rtb.q_count == 0) {
			sl_timer_stop(sl, t7);
			sl_timer_stop(sl, t6);
		} else if (!(sl->flags & (MF_REM_BUSY | MF_RPO)))
			sl_timer_start(sl, t7);
		sl->flags &= ~MF_RTB_FULL;
		sl_check_congestion(sl, q);
		sl_tx_wakeup(q);
		return (0);
	default:
		m2paloger(sl, "Received ACK in unexpected state %d", (int) sl_get_state(sl));
		return (0);
	}
}

STATIC INLINE int
sl_rc_sn_check(struct sl *sl, queue_t *q, mblk_t *mp)
{
	int err;

	switch (sl->i_version) {
	case M2PA_VERSION_DRAFT3:
	case M2PA_VERSION_DRAFT3_1:
		break;
	case M2PA_VERSION_DRAFT4:
	case M2PA_VERSION_DRAFT4_1:
	{
		size_t mlen;

		mlen = mp->b_wptr > mp->b_rptr ? mp->b_wptr - mp->b_rptr : 0;
		if (mlen >= 2 * sizeof(uint16_t)) {
			int msu;
			uint bsnr, fsnr;

			bsnr = ntohs(((uint16_t *) mp->b_rptr)[0]);
			fsnr = ntohs(((uint16_t *) mp->b_rptr)[1]);
			mp->b_rptr += 2 * sizeof(uint16_t);
			mlen -= 2 * sizeof(uint16_t);
			msu = (mlen > sizeof(uint32_t)) ? 0 : 1;
			if ((uint16_t) fsnr != (uint16_t) (sl->fsnx - msu)) {
				if (sl->bfsn) {
					if (sl->sl.notify.events & SL_EVT_FAIL_ABNORMAL_FIBR)
						if ((err =
						     lmi_event_ind(sl, q, SL_EVT_FAIL_ABNORMAL_FIBR,
								   0, NULL, 0)) < 0)
							return (err);
					if ((err =
					     sl_lsc_out_of_service(sl, q,
								   SL_FAIL_ABNORMAL_FIBR)) < 0)
						return (err);
					m2palogst(sl, "Link failed: Abnormal FIBR");
					return (-EPROTO);
				}
				m2palogno(sl, "Received bad fsn = %d, expecting %d", (int) fsnr,
					  (int) (sl->fsnx - msu));
				sl->bfsn++;
				return (-EPROTO);
			} else
				sl->bfsn = 0;
			if (!between16(bsnr, sl->bsnr, sl->fsnt)) {
				if (sl->bbsn) {
					if (sl->sl.notify.events & SL_EVT_FAIL_ABNORMAL_BSNR)
						if ((err =
						     lmi_event_ind(sl, q, SL_EVT_FAIL_ABNORMAL_BSNR,
								   0, NULL, 0)) < 0)
							return (err);
					if ((err =
					     sl_lsc_out_of_service(sl, q,
								   SL_FAIL_ABNORMAL_BSNR)) < 0)
						return (err);
					m2palogst(sl, "Link failed: Abnormal BSNR");
					return (-EPROTO);
				}
				m2palogno(sl, "Received bad bsn = %d, expecting %d", (int) bsnr,
					  (int) sl->fsnt);
				sl->bbsn++;
			} else {
				if (sl->bsnr != bsnr) {
					if ((err =
					     sl_txc_datack(sl, q, diff16(sl->bsnr, bsnr))) < 0)
						return (err);
					sl->bsnr = bsnr;
				}
				sl->bbsn = 0;
			}
			break;
		}
		rare();
		return (-EMSGSIZE);
	}
	case M2PA_VERSION_DRAFT4_9:
	case M2PA_VERSION_DRAFT5:
	case M2PA_VERSION_DRAFT6:
	case M2PA_VERSION_DRAFT6_1:
	case M2PA_VERSION_DRAFT6_9:
	{
		size_t mlen;

		mlen = mp->b_wptr > mp->b_rptr ? mp->b_wptr - mp->b_rptr : 0;
		if (mlen >= 2 * sizeof(uint32_t)) {
			int msu;
			uint bsnr, fsnr;

			bsnr = ntohl(((uint32_t *) mp->b_rptr)[0]) & 0xffffff;
			fsnr = ntohl(((uint32_t *) mp->b_rptr)[1]) & 0xffffff;
			mp->b_rptr += 2 * sizeof(uint32_t);
			mlen -= 2 * sizeof(uint32_t);
			msu = (mlen > sizeof(uint32_t)) ? 0 : 1;
			if ((uint32_t) fsnr != (uint32_t) ((sl->fsnx - msu) & 0xffffff)) {
				if (sl->bfsn) {
					if (sl->sl.notify.events & SL_EVT_FAIL_ABNORMAL_FIBR)
						if ((err =
						     lmi_event_ind(sl, q, SL_EVT_FAIL_ABNORMAL_FIBR,
								   0, NULL, 0)) < 0)
							return (err);
					if ((err =
					     sl_lsc_out_of_service(sl, q,
								   SL_FAIL_ABNORMAL_FIBR)) < 0)
						return (err);
					m2palogst(sl, "Link failed: Abnormal FIBR");
					return (-EPROTO);
				}
				m2palogno(sl, "Received bad fsn = %d, expecting %d", (int) fsnr,
					  (int) ((sl->fsnx - msu) & 0xffffff));
				sl->bfsn++;
				sl->bfsn &= 0xffffff;
				return (-EPROTO);
			} else
				sl->bfsn = 0;
			if (!between24(bsnr, sl->bsnr, sl->fsnt)) {
				if (sl->bbsn) {
					if (sl->sl.notify.events & SL_EVT_FAIL_ABNORMAL_BSNR)
						if ((err =
						     lmi_event_ind(sl, q, SL_EVT_FAIL_ABNORMAL_BSNR,
								   0, NULL, 0)) < 0)
							return (err);
					if ((err =
					     sl_lsc_out_of_service(sl, q,
								   SL_FAIL_ABNORMAL_BSNR)) < 0)
						return (err);
					m2palogst(sl, "Link failed: Abnormal BSNR");
					return (-EPROTO);
				}
				m2palogno(sl, "Received bad bsn = %d, expecting %d", (int) bsnr,
					  (int) sl->fsnt);
				sl->bbsn++;
				sl->bbsn &= 0xffffff;
			} else {
				if (sl->bsnr != bsnr) {
					if ((err =
					     sl_txc_datack(sl, q, diff24(sl->bsnr, bsnr))) < 0)
						return (err);
					sl->bsnr = bsnr;
				}
				sl->bbsn = 0;
			}
			break;
		}
		rare();
		return (-EMSGSIZE);
	}
	default:
	case M2PA_VERSION_DRAFT10:
	{
		size_t mlen = mp->b_wptr > mp->b_rptr ? mp->b_wptr - mp->b_rptr : 0;
		if (mlen >= 2 * sizeof(uint32_t)) {
			int msu;
			uint bsnr, fsnr;

			bsnr = ntohl(((uint32_t *) mp->b_rptr)[0]) & 0xffffff;
			fsnr = ntohl(((uint32_t *) mp->b_rptr)[1]) & 0xffffff;
			mp->b_rptr += 2 * sizeof(uint32_t);
			mlen -= 2 * sizeof(uint32_t);
			msu = (mlen > sizeof(uint32_t)) ? 0 : 1;
			if ((uint32_t) fsnr != (uint32_t) ((sl->fsnx - msu) & 0xffffff)) {
				if (sl->bfsn) {
					if (sl->sl.notify.events & SL_EVT_FAIL_ABNORMAL_FIBR)
						if ((err =
						     lmi_event_ind(sl, q, SL_EVT_FAIL_ABNORMAL_FIBR,
								   0, NULL, 0)) < 0)
							return (err);
					if ((err =
					     sl_lsc_out_of_service(sl, q,
								   SL_FAIL_ABNORMAL_FIBR)) < 0)
						return (err);
					m2palogst(sl, "Link failed: Abnormal FIBR");
					return (-EPROTO);
				}
				m2palogno(sl, "Received bad fsn = %d, expecting %d", (int) fsnr,
					  (int) ((sl->fsnx - msu) & 0xffffff));
				sl->bfsn++;
				sl->bfsn &= 0xffffff;
				return (-EPROTO);
			} else
				sl->bfsn = 0;
			if (!between24(bsnr, sl->bsnr, sl->fsnt)) {
				if (sl->bbsn) {
					if (sl->sl.notify.events & SL_EVT_FAIL_ABNORMAL_BSNR)
						if ((err =
						     lmi_event_ind(sl, q, SL_EVT_FAIL_ABNORMAL_BSNR,
								   0, NULL, 0)) < 0)
							return (err);
					if ((err =
					     sl_lsc_out_of_service(sl, q,
								   SL_FAIL_ABNORMAL_BSNR)) < 0)
						return (err);
					m2palogst(sl, "Link failed: Abnormal BSNR");
					return (-EPROTO);
				}
				m2palogno(sl, "Received bad bsn = %d, expecting %d", (int) bsnr,
					  (int) sl->fsnt);
				sl->bbsn++;
				sl->bbsn &= 0xffffff;
			} else {
				if (sl->bsnr != bsnr) {
					if ((err =
					     sl_txc_datack(sl, q, diff24(sl->bsnr, bsnr))) < 0)
						return (err);
					sl->bsnr = bsnr;
				}
				sl->bbsn = 0;
			}
			break;
		}
		rare();
		return (-EMSGSIZE);
	}
	case M2PA_VERSION_DRAFT11:
	case M2PA_VERSION_RFC4165:
	{
		/* In the final, when we are waiting for synchronization (we have sent
		   PROCESSOR_RECOVERED but have not yet received IN_SERVICE, DATA or DATA-ACK) we
		   do not check the sequence number, but resynchornize to the provided number. */
		size_t mlen;

		mlen = mp->b_wptr > mp->b_rptr ? mp->b_wptr - mp->b_rptr : 0;
		if (mlen >= 2 * sizeof(uint32_t)) {
			int msu;
			uint bsnr, fsnr;

			bsnr = ntohl(((uint32_t *) mp->b_rptr)[0]) & 0xffffff;
			fsnr = ntohl(((uint32_t *) mp->b_rptr)[1]) & 0xffffff;
			mp->b_rptr += 2 * sizeof(uint32_t);
			mlen -= 2 * sizeof(uint32_t);
			msu = (mlen > sizeof(uint32_t)) ? 0 : 1;
			/* In the final we discard out of sequence FSNs but do two out of three on
			   BSNs */
			if ((uint32_t) fsnr != (uint32_t) ((sl->fsnx - msu) & 0xffffff)) {
				m2palogno(sl, "discarding, fsnr = %x, fsnx = %x, msu = %d", fsnr,
					  sl->fsnx, msu);
				return (-EPROTO);
			}
			if (!between24(bsnr, sl->bsnr, sl->fsnt)) {
				if (sl->bbsn) {
					if (sl->sl.notify.events & SL_EVT_FAIL_ABNORMAL_BSNR)
						if ((err =
						     lmi_event_ind(sl, q, SL_EVT_FAIL_ABNORMAL_BSNR,
								   0, NULL, 0)) < 0)
							return (err);
					if ((err =
					     sl_lsc_out_of_service(sl, q,
								   SL_FAIL_ABNORMAL_BSNR)) < 0)
						return (err);
					m2palogst(sl, "Link failed: Abnormal BSNR");
					return (-EPROTO);
				}
				m2palogno(sl, "Received bad bsn = %d, expecting %d", (int) bsnr,
					  (int) sl->fsnt);
				sl->bbsn++;
				sl->bbsn &= 0xffffff;
			} else {
				if (sl->bsnr != bsnr) {
					if ((err =
					     sl_txc_datack(sl, q, diff24(sl->bsnr, bsnr))) < 0)
						return (err);
					sl->bsnr = bsnr;
				}
				sl->bbsn = 0;
			}
			break;
		}
		rare();
		return (-EMSGSIZE);
	}
	}
	return (0);
}

STATIC INLINE int
sl_rc_signal_unit(struct sl *sl, queue_t *q, mblk_t *mp)
{
	int err;

	sl->flags &= ~MF_REM_ALN;
	switch (sl_get_state(sl)) {
	case MS_ALIGNED_NOT_READY:
		if ((err = sl_rc_sn_check(sl, q, mp)) < 0)
			return (err);
		if ((err = sl_in_service_ind(sl, q)) < 0)
			return (err);
		sl_timer_stop(sl, t1);
		sl->flags &= ~MF_SEND_MSU;
		sl->flags &= ~MF_RECV_MSU;
		sl_oos_stats(sl, q);
		sl_set_state(sl, MS_PROCESSOR_OUTAGE);
		goto receive;
	case MS_ALIGNED_READY:
		if ((err = sl_rc_sn_check(sl, q, mp)) < 0)
			return (err);
		if ((err = sl_in_service_ind(sl, q)) < 0)
			return (err);
		sl_timer_stop(sl, t1);
		sl->flags |= MF_SEND_MSU;
		sl->flags |= MF_RECV_MSU;
		sl_is_stats(sl, q);
		sl_set_state(sl, MS_IN_SERVICE);
		goto receive;
	default:
		if ((err = sl_rc_sn_check(sl, q, mp)) < 0)
			return (err);
	      receive:
		if (sl->flags & MF_RECV_MSU) {
			if (!(sl->flags & MF_CONG_DISCARD)) {
				if (mp->b_wptr - mp->b_rptr > sizeof(uint32_t)) {
					bufq_queue(&sl->rb, mp);
					sl_rb_congestion_function(sl, q);
					sl->fsnx++;
					sl->fsnx &= 0xffffff;
					// sl_rx_wakeup(sl->iq);
					return (QR_ABSORBED);
				}
				return (QR_DONE);
			}
			return (-EBUSY);
		}
		rare();
		return (-EPROTO);
	case MS_POWER_OFF:
	case MS_OUT_OF_SERVICE:
		m2paloger(sl, "Received SIGNAL UNIT in unexpected state %d",
			  (int) sl_get_state(sl));
		return (QR_DONE);
	}
}

STATIC streamscall void
sl_tx_wakeup(queue_t *q)
{
	struct sl *sl = SL_PRIV(q);
	int err;
	mblk_t *mp;

	ensure(q, return);
	switch (sl_get_state(sl)) {
	default:
		if (sl->rack)
			if ((err = sl_send_ack(sl, q)) < 0)
				return;
		if (sl->flags & MF_SEND_MSU && sl->tb.q_msgs && !(sl->flags & MF_RTB_FULL)) {
			if (!sl->rtb.q_count) {
				if (sl->flags & MF_REM_BUSY)
					sl_timer_start(sl, t6);
				else
					sl_timer_start(sl, t7);
			}
			while ((mp = sl->tb.q_head)) {
				if ((err = sl_send_data(sl, q, mp)) < 0)
					return;
				sl->tmsu++;
				sl->fsnt++;
				sl->fsnt &= 0xffffff;
				bufq_queue(&sl->rtb, bufq_dequeue(&sl->tb));
				sl->tmsu--;
				sl->tack++;
				sl->sl.stats.sl_tran_msus++;
				sl->sl.stats.sl_tran_sio_sif_octets += mp->b_wptr - mp->b_rptr - 1;
				if (sl->rtb.q_msgs >= sl->sl.config.N1
				    || sl->rtb.q_count >= sl->sl.config.N2
				    || sl->tmsu + sl->tack > 0x7ffffff) {
					sl->flags |= MF_RTB_FULL;
					break;
				}
			}
		}
	case MS_POWER_OFF:
	case MS_OUT_OF_SERVICE:
		return;
	}
}
STATIC INLINE int
sl_lsc_pdu(struct sl *sl, queue_t *q, mblk_t *mp)
{
	bufq_queue(&sl->tb, mp);
	sl_check_congestion(sl, q);
	sl_tx_wakeup(q);
	return (0);
}
STATIC int
sl_t7_timeout(struct sl *sl, queue_t *q)
{
	int err;

	if (sl->sl.notify.events & SL_EVT_FAIL_ACK_TIMEOUT)
		if ((err = lmi_event_ind(sl, q, SL_EVT_FAIL_ACK_TIMEOUT, 0, NULL, 0)) < 0)
			return (err);
	if ((err = sl_lsc_out_of_service(sl, q, SL_FAIL_ACK_TIMEOUT)) < 0)
		return (err);
	m2palogst(sl, "Link failed: T7 Timeout");
	return (0);
}
STATIC INLINE int
sl_lsc_status_busy(struct sl *sl, queue_t *q)
{
	sl->flags &= ~MF_REM_ALN;
	switch (sl_get_state(sl)) {
	case MS_IN_SERVICE:
		if (!(sl->flags & MF_REM_BUSY)) {
			sl->flags |= MF_REM_BUSY;
			if (sl->rtb.q_count)
				sl_timer_start(sl, t6);
			sl_timer_stop(sl, t7);
		}
		return (QR_DONE);
	case MS_PROCESSOR_OUTAGE:
		sl->flags |= MF_REM_BUSY;
		return (QR_DONE);
	case MS_POWER_OFF:
	case MS_OUT_OF_SERVICE:
		return (QR_DONE);
	}
	m2paloger(sl, "Received status BUSY in unexpected state %d", (int) sl_get_state(sl));
	return (-EPROTO);
}
STATIC INLINE int
sl_lsc_congestion_discard(struct sl *sl, queue_t *q)
{
	int err;

	if (!(sl->flags & MF_LOC_BUSY)) {
		if ((err = sl_send_status(sl, q, M2PA_STATUS_BUSY)) < 0)
			return (err);
		sl->sl.stats.sl_sibs_sent++;
		sl->flags |= MF_LOC_BUSY;
	}
	sl->flags |= MF_CONG_DISCARD;
	sl->flags |= MF_L3_CONG_DETECT;
	return (0);
}
STATIC INLINE int
sl_lsc_congestion_accept(struct sl *sl, queue_t *q)
{
	int err;

	if (!(sl->flags & MF_LOC_BUSY)) {
		if ((err = sl_send_status(sl, q, M2PA_STATUS_BUSY)) < 0)
			return (err);
		sl->sl.stats.sl_sibs_sent++;
		sl->flags |= MF_LOC_BUSY;
	}
	sl->flags |= MF_CONG_ACCEPT;
	sl->flags |= MF_L3_CONG_DETECT;
	return (0);
}
STATIC INLINE int
sl_lsc_status_busy_ended(struct sl *sl, queue_t *q)
{
	sl->flags &= ~MF_REM_ALN;
	switch (sl_get_state(sl)) {
	default:
		m2paloger(sl, "Received status BUSY_ENDED in unexpected state %d",
			  (int) sl_get_state(sl));
	case MS_IN_SERVICE:
		sl->flags &= ~MF_REM_BUSY;
		sl_timer_stop(sl, t6);
		if (sl->rtb.q_count)
			sl_timer_start(sl, t7);
		return (QR_DONE);
	case MS_POWER_OFF:
	case MS_OUT_OF_SERVICE:
		return (QR_DONE);
	}
}
STATIC int
sl_t6_timeout(struct sl *sl, queue_t *q)
{
	int err;

	if (sl->sl.notify.events & SL_EVT_FAIL_CONG_TIMEOUT)
		if ((err = lmi_event_ind(sl, q, SL_EVT_FAIL_CONG_TIMEOUT, 0, NULL, 0)) < 0)
			return (err);
	if ((err = sl_lsc_out_of_service(sl, q, SL_FAIL_CONG_TIMEOUT)) < 0)
		return (err);
	m2palogst(sl, "Link failed: T6 Timeout");
	return (0);
}
STATIC INLINE int
sl_lsc_no_congestion(struct sl *sl, queue_t *q)
{
	int err;

	if (sl->flags & MF_LOC_BUSY)
		if ((err = sl_send_status(sl, q, M2PA_STATUS_BUSY_ENDED)) < 0)
			return (err);
	sl->flags &= ~MF_LOC_BUSY;
	sl->flags &= ~MF_CONG_DISCARD;
	sl->flags &= ~MF_CONG_ACCEPT;
	sl->flags &= ~MF_L3_CONG_DETECT;
	sl->rack += xchg(&sl->rmsu, 0);
	return (0);
}

STATIC INLINE int
sl_lsc_status_processor_outage(struct sl *sl, queue_t *q)
{
	int err;
	int proving = !(sl->option.popt & SS7_POPT_NOPR);

	sl->flags &= ~MF_REM_ALN;
	switch (sl_get_state(sl)) {
	case MS_POWER_OFF:
	case MS_OUT_OF_SERVICE:
		/* ignored in these states */
		return (QR_DONE);
	case MS_NOT_ALIGNED:
		if (proving)
			return (QR_DONE);
		sl_timer_stop(sl, t2);
		if (!(sl->flags & MF_RPO)) {
			if (sl->sl.notify.events & SL_EVT_RPO_BEGIN)
				if ((err = lmi_event_ind(sl, q, SL_EVT_RPO_BEGIN, 0, NULL, 0)) < 0)
					return (err);
			sl->flags |= MF_RPO;
			m2palogst(sl, "RPO Begins");
		}
		return sl_ready(sl, q);
	case MS_ALIGNED:
	case MS_PROVING:
		if (!(sl->flags & MF_RPO)) {
			if (sl->sl.notify.events & SL_EVT_RPO_BEGIN)
				if ((err = lmi_event_ind(sl, q, SL_EVT_RPO_BEGIN, 0, NULL, 0)) < 0)
					return (err);
			/* just remember for later */
			sl->flags |= MF_RPO;
			m2palogst(sl, "RPO Begins");
		}
		return (QR_DONE);
	case MS_ALIGNED_READY:
	case MS_ALIGNED_NOT_READY:
		if (!(sl->flags & MF_RPO)) {
			if (sl->sl.notify.events & SL_EVT_RPO_BEGIN)
				if ((err = lmi_event_ind(sl, q, SL_EVT_RPO_BEGIN, 0, NULL, 0)) < 0)
					return (err);
			if ((err = sl_remote_processor_outage_ind(sl, q)) < 0)
				return (err);
			sl->flags |= MF_RPO;
			sl_rpo_stats(sl, q);
			m2palogst(sl, "RPO Begins");
		}
		sl_timer_stop(sl, t1);
		sl_set_state(sl, MS_PROCESSOR_OUTAGE);
		return (QR_DONE);
	case MS_IN_SERVICE:
		if (sl->sl.notify.events & SL_EVT_RPO_BEGIN)
			if ((err = lmi_event_ind(sl, q, SL_EVT_RPO_BEGIN, 0, NULL, 0)) < 0)
				return (err);
		if (!(sl->flags & MF_RPO)) {
			if ((err = sl_remote_processor_outage_ind(sl, q)) < 0)
				return (err);
			sl->flags |= MF_RPO;
		}
		m2palogst(sl, "RPO Begins");
		sl_timer_stop(sl, t7);
		sl_timer_stop(sl, t6);
		sl->flags &= ~MF_SEND_MSU;
		switch (sl->option.pvar) {
		default:
		case SS7_PVAR_ITUT_93:
		case SS7_PVAR_ITUT_96:
			sl->flags &= ~MF_CONTINUE;
			break;
		case SS7_PVAR_ANSI_92:
			sl->rack += xchg(&sl->rmsu, 0);
			fixme(("%s: %p: FIXME: Bad idea\n", MOD_NAME, sl));
			if (sl->flags & MF_LOC_BUSY)
				if ((err = sl_send_status(sl, q, M2PA_STATUS_BUSY_ENDED)) < 0)
					return (err);
			sl->flags &= ~MF_LOC_BUSY;
			break;
		}
		sl_rpo_stats(sl, q);
		sl_set_state(sl, MS_PROCESSOR_OUTAGE);
		return (QR_DONE);
	case MS_PROCESSOR_OUTAGE:
		if (!(sl->flags & MF_RPO)) {
			if (sl->sl.notify.events & SL_EVT_RPO_BEGIN)
				if ((err = lmi_event_ind(sl, q, SL_EVT_RPO_BEGIN, 0, NULL, 0)) < 0)
					return (err);
			if (!(sl->flags & MF_RPO)) {
				if ((err = sl_remote_processor_outage_ind(sl, q)) < 0)
					return (err);
				sl->flags |= MF_RPO;
			}
			m2palogst(sl, "RPO Begins");
		}
		return (QR_DONE);
	}
	m2paloger(sl, "Received status PROCESSOR_OUTAGE in unexpected state %d",
		  (int) sl_get_state(sl));
	return (-EPROTO);
}

STATIC INLINE int
sl_lsc_local_processor_outage(struct sl *sl, queue_t *q)
{
	int err;

	switch (sl_get_state(sl)) {
	case MS_POWER_OFF:
	case MS_OUT_OF_SERVICE:
	case MS_NOT_ALIGNED:
	case MS_ALIGNED:
	case MS_PROVING:
		/* just remember for later */
		sl->flags &= ~MF_WAIT_SYNC;
		sl->flags |= MF_LPO;
		return (0);
	case MS_ALIGNED_READY:
		if ((err = sl_send_status(sl, q, M2PA_STATUS_PROCESSOR_OUTAGE)) < 0)
			return (err);
		sl->flags &= ~MF_WAIT_SYNC;
		sl->flags |= MF_LPO;
		sl_timer_stop(sl, t7);
		sl_timer_stop(sl, t6);
		sl->flags &= ~MF_RECV_MSU;
		sl_set_state(sl, MS_ALIGNED_NOT_READY);
		return (0);
	case MS_IN_SERVICE:
		if ((err = sl_send_status(sl, q, M2PA_STATUS_PROCESSOR_OUTAGE)) < 0)
			return (err);
		sl->flags &= ~MF_WAIT_SYNC;
		sl->flags |= MF_LPO;
		sl_timer_stop(sl, t7);
		sl_timer_stop(sl, t6);
		sl->flags &= ~MF_RECV_MSU;
		switch (sl->option.pvar) {
		default:
		case SS7_PVAR_ITUT_93:
		case SS7_PVAR_ITUT_96:
			sl->flags &= ~MF_CONTINUE;
			break;
		case SS7_PVAR_ANSI_92:
			sl->rack += xchg(&sl->rmsu, 0);
			fixme(("%s: %p: FIXME: Bad idea\n", MOD_NAME, sl));
			if (sl->flags & MF_LOC_BUSY)
				if ((err = sl_send_status(sl, q, M2PA_STATUS_BUSY_ENDED)) < 0)
					return (err);
			sl->flags &= ~MF_LOC_BUSY;
			break;
		}
		sl_rpo_stats(sl, q);
		sl_set_state(sl, MS_PROCESSOR_OUTAGE);
		return (0);
	case MS_PROCESSOR_OUTAGE:
		if (!(sl->flags & MF_LPO))
			if ((err = sl_send_status(sl, q, M2PA_STATUS_PROCESSOR_OUTAGE)) < 0)
				return (err);
		sl->flags &= ~MF_WAIT_SYNC;
		sl->flags |= MF_LPO;
		sl_timer_stop(sl, t7);	/* ??? */
		sl_timer_stop(sl, t6);	/* ??? */
		return (0);
	}
	m2paloger(sl, "Received primitive SL_LOCAL_PROCESSOR_OUTAGE_REQ in unexpected state %d",
		  (int) sl_get_state(sl));
	return (-EPROTO);
}
STATIC INLINE int
sl_lsc_clear_rtb(struct sl *sl, queue_t *q)
{
	int err;

	switch (sl_get_state(sl)) {
	case MS_PROCESSOR_OUTAGE:
		bufq_purge(&sl->rtb);
		sl->tack = 0;
		sl->flags &= ~MF_RTB_FULL;
		sl->flags &= ~MF_CLEAR_RTB;
		if ((err = sl_rtb_cleared_ind(sl, q)) < 0)
			return (err);
		if ((err = sl_check_congestion(sl, q)) < 0)
			return (err);
		sl_timer_stop(sl, t7);
		sl_timer_stop(sl, t6);
		sl->flags &= ~MF_LPO;
		if (sl->flags & MF_RPO) {
			if (sl->sl.notify.events & SL_EVT_RPO_END)
				if ((err = lmi_event_ind(sl, q, SL_EVT_RPO_END, 0, NULL, 0)) < 0)
					return (err);
			m2palogst(sl, "RPO Ends");
			sl_rpr_stats(sl, q);
			sl->flags &= ~MF_RPO;
		}
		sl->flags &= ~MF_SEND_MSU;
		sl->flags |= MF_SEND_MSU;
		if ((err = sl_remote_processor_recovered_ind(sl, q)) < 0)
			return (err);
		sl_is_stats(sl, q);
		sl_set_state(sl, MS_IN_SERVICE);
		return (0);
	}
	m2paloger(sl, "Received primitive SL_CLEAR_RTB_REQ in unexpected state %d",
		  (int) sl_get_state(sl));
	return (-EPROTO);
}
STATIC INLINE int
sl_lsc_clear_buffers(struct sl *sl, queue_t *q)
{
	int err;

	switch (sl_get_state(sl)) {
	case MS_POWER_OFF:
	case MS_OUT_OF_SERVICE:
	case MS_NOT_ALIGNED:
	case MS_ALIGNED:
	case MS_PROVING:
		switch (sl->option.pvar) {
		default:
		case SS7_PVAR_ITUT_93:
		case SS7_PVAR_ITUT_96:
			return (0);
		case SS7_PVAR_ANSI_92:
			if ((err = sl_rtb_cleared_ind(sl, q)) < 0)
				return (err);
			sl->flags &= ~MF_LPO;	/* ??? */
			return (0);
		}
		return (0);
	case MS_ALIGNED_NOT_READY:
		switch (sl->option.pvar) {
		default:
		case SS7_PVAR_ITUT_93:
		case SS7_PVAR_ITUT_96:
			return (0);
		case SS7_PVAR_ANSI_92:
			if ((err = sl_rtb_cleared_ind(sl, q)) < 0)
				return (err);
			sl->flags &= ~MF_LPO;
			sl_timer_stop(sl, t7);	/* ??? */
			sl_timer_stop(sl, t6);	/* ??? */
			sl->flags &= ~MF_SEND_MSU;
			sl_set_state(sl, MS_ALIGNED_READY);
			return (0);
		}
		return (0);
	case MS_PROCESSOR_OUTAGE:
		bufq_purge(&sl->rtb);
		sl->tack = 0;
		bufq_purge(&sl->tb);
		flushq(sl->iq, FLUSHDATA);
		sl_timer_stop(sl, t7);
		sl_timer_stop(sl, t6);
		sl->flags &= ~MF_RTB_FULL;
		sl->flags &= ~MF_CLEAR_RTB;
		if ((err = sl_check_congestion(sl, q)) < 0)
			return (err);
		switch (sl->option.pvar) {
		default:
		case SS7_PVAR_ITUT_93:
		case SS7_PVAR_ITUT_96:
			sl->flags &= ~MF_NEED_FLUSH;
			if (sl->flags & MF_CONTINUE) {
				sl->flags |= MF_NEED_FLUSH;
				break;
			}
			return (0);
		case SS7_PVAR_ANSI_92:
			if (sl->sl.notify.events & SL_EVT_RPO_END)
				if ((err = lmi_event_ind(sl, q, SL_EVT_RPO_END, 0, NULL, 0)) < 0)
					return (err);
			if ((err = sl_rtb_cleared_ind(sl, q)) < 0)
				return (err);
			bufq_purge(&sl->rb);
			flushq(sl->oq, FLUSHDATA);
			if ((err = sl_rb_cleared_ind(sl, q)) < 0)
				return (err);
			if (sl->flags & MF_RPO) {
				if ((err = sl_remote_processor_recovered_ind(sl, q)) < 0)
					return (err);
				sl->flags &= ~MF_RPO;
			}
			m2palogst(sl, "RPO Ends");
			sl_rpr_stats(sl, q);
			break;
		}
		sl->flags |= MF_RECV_MSU;
		sl->flags |= MF_SEND_MSU;
		sl->flags &= ~MF_LPO;
		sl_is_stats(sl, q);
		sl_set_state(sl, MS_IN_SERVICE);
		return (0);
	}
	m2paloger(sl, "Received primitive SL_CLEAR_BUFFERS_REQ in unexpected state %d",
		  (int) sl_get_state(sl));
	return (-EPROTO);
}
STATIC void
sl_lsc_no_processor_outage(struct sl *sl, queue_t *q)
{
	switch (sl_get_state(sl)) {
	case MS_PROCESSOR_OUTAGE:
		sl->flags |= MF_CONTINUE;
		if (!(sl->flags & MF_NEED_FLUSH)) {
			sl->flags &= ~MF_LPO;
			sl->flags |= MF_NEED_FLUSH;
			sl->flags |= MF_RECV_MSU;
			if (!(sl->flags & MF_WAIT_SYNC))
				sl->flags |= MF_SEND_MSU;
			if (sl->rtb.q_count) {
				if (sl->flags & MF_REM_BUSY)
					sl_timer_start(sl, t6);
				else
					sl_timer_start(sl, t7);
			}
			sl_is_stats(sl, q);
			sl_set_state(sl, MS_IN_SERVICE);
			sl_tx_wakeup(q);
		}
	}
}

#if 0
/*
   This was the blue book way of doing things... 
 */
STATIC INLINE int
sl_lsc_continue(struct sl *sl, queue_t *q, mblk_t *mp)
{
	switch (sl_get_state(sl)) {
	case MS_PROCESSOR_OUTAGE:
		if (sl->flags & MF_CONTINUE) {
			sl->flags |= MF_NEED_FLUSH;
			sl->flags |= MF_RECV_MSU;
			sl->flags |= MF_SEND_MSU;
			if (sl->rtb.q_count) {
				if (sl->flags & MF_REM_BUSY)
					sl_timer_start(sl, t6);
				else
					sl_timer_start(sl, t7);
			}
			sl->flags &= ~MF_LPO;
			sl_is_stats(sl, q);
			sl_set_state(sl, MS_IN_SERVICE);
		}
		return (0);
	}
	m2paloger(sl, "Received primitive SL_CONTINUE_REQ in unexpected state %d",
		  (int) sl_get_state(sl));
	return (-EPROTO);
}
#endif

STATIC INLINE int
sl_lsc_status_processor_outage_ended(struct sl *sl, queue_t *q, mblk_t *mp)
{
	int err;

	sl->flags &= ~MF_REM_ALN;
	switch (sl_get_state(sl)) {
	case MS_PROCESSOR_OUTAGE:
		if (sl->flags & MF_RPO) {
			if (sl->i_version >= M2PA_VERSION_DRAFT10) {
				/* always clear the retransmission buffer */
				bufq_purge(&sl->rtb);
				sl->tack = 0;
				sl->flags &= ~MF_RTB_FULL;
				sl->flags &= ~MF_CLEAR_RTB;
				if ((err = sl_check_congestion(sl, q)) < 0)
					return (err);
				/* In the final, when we receive PROCESSOR_RECOVERED, we need to
				   send READY, and also use the BSN from the received message to
				   synchronize FSN. */
				sl->fsnt = ntohl(*((uint32_t *) mp->b_rptr)) & 0xffffff;
				if ((err = sl_send_status(sl, q, M2PA_STATUS_IN_SERVICE)))
					return (err);
				sl->flags &= ~MF_WAIT_SYNC;
			}
			if (sl->sl.notify.events & SL_EVT_RPO_END)
				if ((err = lmi_event_ind(sl, q, SL_EVT_RPO_END, 0, NULL, 0)) < 0)
					return (err);
			if ((err = sl_remote_processor_recovered_ind(sl, q)) < 0)
				return (err);
			sl->flags &= ~MF_RPO;
			m2palogst(sl, "RPO Ends");
		}
		sl_rpr_stats(sl, q);
		switch (sl->option.pvar) {
		default:
		case SS7_PVAR_ITUT_93:
		case SS7_PVAR_ITUT_96:
			/* auto recover */
			if (!(sl->flags & MF_LPO))
				sl_lsc_no_processor_outage(sl, q);
			break;
		case SS7_PVAR_ANSI_92:
			/* leave state latched */
			break;
		}
		return (QR_DONE);
	}
	m2paloger(sl, "Received status PROCESSOR_OUTAGE_ENDED in unexpected state %d",
		  (int) sl_get_state(sl));
	return (-EPROTO);
}

STATIC INLINE int
sl_lsc_resume(struct sl *sl, queue_t *q)
{
	int err;

	switch (sl_get_state(sl)) {
	case MS_POWER_OFF:
	case MS_OUT_OF_SERVICE:
	case MS_NOT_ALIGNED:
	case MS_ALIGNED:
	case MS_PROVING:
		/* remember for later */
		sl->flags &= ~MF_LPO;
		return (0);
	case MS_ALIGNED_READY:
		fixme(("%s: %p: FIXME: This is really an error...\n", MOD_NAME, sl));
		return (0);
	case MS_ALIGNED_NOT_READY:
		sl->flags &= ~MF_LPO;
		switch (sl->option.pvar) {
		case SS7_PVAR_ITUT_96:
			if (!(sl->flags & MF_RPO))
				sl_lsc_no_processor_outage(sl, q);
			sl->flags |= MF_RECV_MSU;	/* ??? */
			break;
		case SS7_PVAR_ANSI_92:
			break;
		default:
			if (!(sl->flags & MF_RPO))
				sl_lsc_no_processor_outage(sl, q);
			break;
		}
		sl_timer_stop(sl, t7);
		sl_timer_stop(sl, t6);
		sl->flags &= ~MF_SEND_MSU;
		sl_set_state(sl, MS_ALIGNED_READY);
		return (0);
	case MS_PROCESSOR_OUTAGE:
		/* This is where the really wierd behavior begins... */
		switch (sl->option.pvar) {
		default:
		case SS7_PVAR_ITUT_93:
		case SS7_PVAR_ITUT_96:
			if (sl->flags & MF_LPO) {
				if ((err =
				     sl_send_status(sl, q, M2PA_STATUS_PROCESSOR_OUTAGE_ENDED)) < 0)
					return (err);
				if (sl->i_version >= M2PA_VERSION_DRAFT10)
					sl->flags |= MF_WAIT_SYNC;
				sl->flags &= ~MF_LPO;
			}
			if (!(sl->flags & MF_RPO))
				sl_lsc_no_processor_outage(sl, q);
			sl->rack += xchg(&sl->rmsu, 0);
			sl->flags &= ~MF_CONG_DISCARD;
			sl->flags &= ~MF_CONG_ACCEPT;
			if (sl->flags & MF_LOC_BUSY)
				if ((err = sl_send_status(sl, q, M2PA_STATUS_BUSY_ENDED)) < 0)
					return (err);
			sl->flags &= ~MF_LOC_BUSY;
			sl_timer_stop(sl, t7);
			sl_timer_stop(sl, t6);
			sl->flags &= ~MF_SEND_MSU;
			return (0);
		case SS7_PVAR_ANSI_92:
			/* XXX do we do this now??? */
			if (sl->flags & MF_LPO) {
				if ((err =
				     sl_send_status(sl, q, M2PA_STATUS_PROCESSOR_OUTAGE_ENDED)) < 0)
					return (err);
				if (sl->i_version >= M2PA_VERSION_DRAFT10)
					sl->flags |= MF_WAIT_SYNC;
				sl->flags &= ~MF_LPO;
			}
			sl->flags |= MF_RECV_MSU;
			if (sl->flags & MF_RPO) {
				sl_timer_stop(sl, t7);
				sl_timer_stop(sl, t6);
				sl->flags &= ~MF_SEND_MSU;
				fixme(("%s: %p: FIXME: We should not do this...\n", MOD_NAME, sl));
				if ((err = sl_remote_processor_outage_ind(sl, q)) < 0)
					return (err);
				return (0);
			}
			sl->flags |= MF_SEND_MSU;
			if (sl->rtb.q_count) {
				if (sl->flags & MF_REM_BUSY)
					sl_timer_start(sl, t6);
				else
					sl_timer_start(sl, t7);
			}
			break;
		}
		sl_is_stats(sl, q);
		sl_set_state(sl, MS_IN_SERVICE);
		return (0);
	}
	m2paloger(sl, "Received primitive SL_RESUME_REQ in unexpected state %d",
		  (int) sl_get_state(sl));
	return (-EPROTO);
}

STATIC INLINE int
sl_lsc_status_out_of_service(struct sl *sl, queue_t *q)
{
	int err;

	sl->flags &= ~MF_REM_ALN;
	switch (sl_get_state(sl)) {
	case MS_ALIGNED_READY:
	case MS_ALIGNED_NOT_READY:
	case MS_IN_SERVICE:
	case MS_PROCESSOR_OUTAGE:
		sl->flags &= ~MF_REM_INS;
		if (sl->sl.notify.events & SL_EVT_FAIL_RECEIVED_SIOS)
			if ((err = lmi_event_ind(sl, q, SL_EVT_FAIL_RECEIVED_SIOS, 0, NULL, 0)) < 0)
				return (err);
		if ((err = sl_lsc_out_of_service(sl, q, SL_FAIL_RECEIVED_SIOS)) < 0)
			return (err);
		m2palogst(sl, "Link failed: Received SIOS");
		return (QR_DONE);
	case MS_ALIGNED:
	case MS_PROVING:
		sl->flags &= ~MF_REM_INS;
		if (sl->sl.notify.events & SL_EVT_FAIL_ALIGNMENT)
			if ((err = lmi_event_ind(sl, q, SL_EVT_FAIL_ALIGNMENT, 0, NULL, 0)) < 0)
				return (err);
		if ((err = sl_lsc_out_of_service(sl, q, SL_FAIL_ALIGNMENT_NOT_POSSIBLE)) < 0)
			return (err);
		m2palogst(sl, "Link failed: Alignment not possible");
		sl->sl.stats.sl_fail_align_or_proving++;
		return (QR_DONE);
	case MS_POWER_OFF:
	case MS_OUT_OF_SERVICE:
	case MS_NOT_ALIGNED:
		return (QR_DONE);
	}
	m2paloger(sl, "Received status OUT_OF_SERVICE in unexpected state %d",
		  (int) sl_get_state(sl));
	return (-EPROTO);
}

STATIC INLINE int
sl_lsc_retrieve_bsnt(struct sl *sl, queue_t *q)
{
	switch (sl_get_state(sl)) {
	case MS_POWER_OFF:
	case MS_OUT_OF_SERVICE:
	case MS_PROCESSOR_OUTAGE:
		sl_bsnt_ind(sl, q, sl->fsnr);
		return (0);
	}
	m2paloger(sl, "Received primitive SL_RETRIEVE_BSNT_REQ in unexpected state %d",
		  (int) sl_get_state(sl));
	return (-EPROTO);
}

STATIC INLINE int
sl_lsc_retrieval_request_and_fsnc(struct sl *sl, queue_t *q, uint fsnc)
{
	int err;
	mblk_t *mp;

	switch (sl_get_state(sl)) {
	case MS_POWER_OFF:
	case MS_OUT_OF_SERVICE:
	case MS_PROCESSOR_OUTAGE:
		/* 
		 *  FIXME: Q.704/5.7.2 states:
		 *
		 *  5.7.2   If a changeover order or acknowledgement
		 *  containing an unreasonable value of the forward sequence
		 *  number is received, no buffer updating or retrieval is
		 *  performed, and new traffic is started on the alternative
		 *  signalling link(s).
		 *
		 *  It will be necessary to check FSNC for "reasonableness"
		 *  here and flush RTB and TB and return retrieval-complete
		 *  indication with a return code of "unreasonable FSNC".
		 */
		fixme(("%s: %p: FIXME: Fix this check...\n", MOD_NAME, sl));
		/* this will pretty much clear the rtb if there is a problem with the FSNC */
		while (sl->rtb.q_msgs && sl->fsnt != fsnc) {
			freemsg(bufq_dequeue(&sl->rtb));
			sl->fsnt--;
			sl->fsnt &= 0xffffff;
		}
		while ((mp = bufq_dequeue(&sl->rtb))) {
			if ((err = sl_retrieved_message_ind(sl, q, mp)) < 0) {
				bufq_queue_head(&sl->rtb, mp);
				return (err);
			}
			sl->flags &= ~MF_RTB_FULL;
		}
		while ((mp = bufq_dequeue(&sl->tb))) {
			if ((err = sl_retrieved_message_ind(sl, q, mp)) < 0) {
				bufq_queue_head(&sl->tb, mp);
				return (err);
			}
		}
		if ((err = sl_retrieval_complete_ind(sl, q)) < 0)
			return (err);
		return (0);
	}
	m2paloger(sl, "Received primitive SL_RETRIEVAL_REQUEST_AND_FSNC_REQ in unexpected state %d",
		  (int) sl_get_state(sl));
	if ((err = sl_retrieval_not_possible_ind(sl, q)) < 0)
		return (err);
	return (-EPROTO);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  TRANSMIT CONGESTION ALGORITHM
 *
 *  -------------------------------------------------------------------------
 *
 *  The transmit congestion algorithm is an implementation dependent algorithm
 *  but is suggested as being based on TB and/or RTB buffer occupancy.  With
 *  STREAMS we can use octet count buffer occupancy over message count
 *  occupancy, because congestion in transmission is more related to octet
 *  count (because it determines transmission latency).
 *
 *  We check the total buffer occupancy and apply the necessary congestion
 *  control signal as per configured abatement, onset and discard thresholds.
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sl_check_congestion(struct sl *sl, queue_t *q)
{
	int err;
	unsigned int occupancy = sl->tb.q_count + sl->rtb.q_count;
	int old_cong_status = sl->sl.statem.cong_status;
	int old_disc_status = sl->sl.statem.disc_status;
	int multi = sl->option.popt & SS7_POPT_MPLEV;

	switch (sl->sl.statem.cong_status) {
	case 0:
		if (occupancy >= sl->sl.config.tb_onset_1) {
			sl->sl.statem.cong_status = 1;
			if (occupancy >= sl->sl.config.tb_discd_1) {
				sl->sl.statem.disc_status = 1;
				if (!multi)
					break;
				if (occupancy >= sl->sl.config.tb_onset_2) {
					sl->sl.statem.cong_status = 2;
					if (occupancy >= sl->sl.config.tb_discd_2) {
						sl->sl.statem.disc_status = 2;
						if (occupancy >= sl->sl.config.tb_onset_3) {
							sl->sl.statem.cong_status = 3;
							if (occupancy >= sl->sl.config.tb_discd_3) {
								sl->sl.statem.disc_status = 3;
							}
						}
					}
				}
			}
		}
		break;
	case 1:
		if (occupancy < sl->sl.config.tb_abate_1) {
			sl->sl.statem.cong_status = 0;
			sl->sl.statem.disc_status = 0;
		} else {
			if (!multi)
				break;
			if (occupancy >= sl->sl.config.tb_onset_2) {
				sl->sl.statem.cong_status = 2;
				if (occupancy >= sl->sl.config.tb_discd_2) {
					sl->sl.statem.disc_status = 2;
					if (occupancy >= sl->sl.config.tb_onset_3) {
						sl->sl.statem.cong_status = 3;
						if (occupancy >= sl->sl.config.tb_discd_3) {
							sl->sl.statem.disc_status = 3;
						}
					}
				}
			}
		}
		break;
	case 2:
		if (!multi) {
			sl->sl.statem.cong_status = 1;
			sl->sl.statem.disc_status = 1;
			break;
		}
		if (occupancy < sl->sl.config.tb_abate_2) {
			sl->sl.statem.cong_status = 1;
			sl->sl.statem.disc_status = 1;
			if (occupancy < sl->sl.config.tb_abate_1) {
				sl->sl.statem.cong_status = 0;
				sl->sl.statem.disc_status = 0;
			}
		} else if (occupancy >= sl->sl.config.tb_onset_3) {
			sl->sl.statem.cong_status = 3;
			if (occupancy >= sl->sl.config.tb_discd_3) {
				sl->sl.statem.disc_status = 3;
			}
		}
		break;
	case 3:
		if (!multi) {
			sl->sl.statem.cong_status = 1;
			sl->sl.statem.disc_status = 1;
			break;
		}
		if (occupancy < sl->sl.config.tb_abate_3) {
			sl->sl.statem.cong_status = 2;
			sl->sl.statem.disc_status = 2;
			if (occupancy < sl->sl.config.tb_abate_2) {
				sl->sl.statem.cong_status = 1;
				sl->sl.statem.disc_status = 1;
				if (occupancy < sl->sl.config.tb_abate_1) {
					sl->sl.statem.cong_status = 0;
					sl->sl.statem.disc_status = 0;
				}
			}
		}
		break;
	}
	if (sl->sl.statem.cong_status != old_cong_status
	    || sl->sl.statem.disc_status != old_disc_status) {
		if (sl->sl.statem.cong_status < old_cong_status)
			sl_link_congestion_ceased_ind(sl, q, sl->sl.statem.cong_status,
						      sl->sl.statem.disc_status);
		else {
			if (sl->sl.statem.cong_status > old_cong_status) {
				if (sl->sl.notify.events & SL_EVT_CONGEST_ONSET_IND
				    && !sl->sl.stats.sl_cong_onset_ind[sl->sl.statem.cong_status]++)
					if ((err =
					     lmi_event_ind(sl, q, SL_EVT_CONGEST_ONSET_IND, 0,
							   &sl->sl.statem.cong_status,
							   sizeof(sl->sl.statem.cong_status))) < 0)
						return (err);
				m2palogda(sl, "Congestion onset: level %d",
					  (int) sl->sl.statem.cong_status);
			} else {
				if (sl->sl.notify.events & SL_EVT_CONGEST_DISCD_IND
				    && !sl->sl.stats.sl_cong_discd_ind[sl->sl.statem.disc_status]++)
					if ((err =
					     lmi_event_ind(sl, q, SL_EVT_CONGEST_DISCD_IND, 0,
							   &sl->sl.statem.disc_status,
							   sizeof(sl->sl.statem.disc_status))) < 0)
						return (err);
				m2palogda(sl, "Congestion discard: level %d",
					  (int) sl->sl.statem.cong_status);
			}
			sl_link_congested_ind(sl, q, sl->sl.statem.cong_status,
					      sl->sl.statem.disc_status);
		}
	}
	return (0);
}

/*
 *  =========================================================================
 *
 *  INPUT Events
 *
 *  =========================================================================
 *  -------------------------------------------------------------------------
 *
 *  SL Peer -> SL Peer (M2PA) Received Messages
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  M2PA RECV ACK
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_recv_datack(struct sl *sl, queue_t *q, mblk_t *mp)
{
	int err;
	uint count;

	sl->flags &= ~MF_REM_ALN;
	switch (sl->i_version) {
	case M2PA_VERSION_DRAFT3_1:
	{
		size_t mlen = mp->b_wptr - mp->b_rptr;
		if (mlen >= sizeof(uint32_t)) {
			count = ntohl(*(uint32_t *) mp->b_wptr);
			mp->b_wptr += sizeof(uint32_t);
			if (count) {
				if ((err = sl_txc_datack(sl, q, count)) < 0)
					return (err);
				return (QR_DONE);
			}
			rare();
			return (-EPROTO);
		}
		rare();
		return (-EMSGSIZE);
	}
	case M2PA_VERSION_DRAFT3:
	case M2PA_VERSION_DRAFT4:
	case M2PA_VERSION_DRAFT4_1:
	case M2PA_VERSION_DRAFT4_9:
	case M2PA_VERSION_DRAFT5:
	case M2PA_VERSION_DRAFT6:
	case M2PA_VERSION_DRAFT6_1:
	case M2PA_VERSION_DRAFT6_9:
	default:
	case M2PA_VERSION_DRAFT10:
	case M2PA_VERSION_DRAFT11:
	case M2PA_VERSION_RFC4165:
		m2paloger(sl, "Invalid message for version");
		return (-EPROTO);
	}
}

/*
 *  M2PA RECV PROVING
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_recv_proving(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl->flags &= ~MF_REM_ALN;
	switch (sl->i_version) {
	case M2PA_VERSION_DRAFT3:
	case M2PA_VERSION_DRAFT3_1:
		/* just ignore proving packets */
		return (QR_DONE);
	case M2PA_VERSION_DRAFT4:
	case M2PA_VERSION_DRAFT4_1:
	case M2PA_VERSION_DRAFT4_9:
	case M2PA_VERSION_DRAFT5:
	case M2PA_VERSION_DRAFT6:
	case M2PA_VERSION_DRAFT6_1:
	case M2PA_VERSION_DRAFT6_9:
	default:
	case M2PA_VERSION_DRAFT10:
	case M2PA_VERSION_DRAFT11:
	case M2PA_VERSION_RFC4165:
		m2paloger(sl, "Invalid message for version");
		return (-EPROTO);
	}
}

/*
 *  M2PA RECV STATUS
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_recv_status(struct sl *sl, queue_t *q, mblk_t *mp)
{
	size_t hlen;			/* M2PA-specific header length */
	size_t mlen = mp->b_wptr > mp->b_rptr ? mp->b_wptr - mp->b_rptr : 0;

	switch (sl->i_version) {
	case M2PA_VERSION_DRAFT3:
	case M2PA_VERSION_DRAFT3_1:
	case M2PA_VERSION_DRAFT6_9:
		hlen = 0;
		break;
	case M2PA_VERSION_DRAFT4:
	case M2PA_VERSION_DRAFT4_1:
		hlen = 2 * sizeof(uint16_t);
		break;
	case M2PA_VERSION_DRAFT4_9:
	case M2PA_VERSION_DRAFT5:
	case M2PA_VERSION_DRAFT6:
	case M2PA_VERSION_DRAFT6_1:
	default:
	case M2PA_VERSION_DRAFT10:
	case M2PA_VERSION_DRAFT11:
	case M2PA_VERSION_RFC4165:
		/* In the final analysis, sequence numbers are included in status messages. */
		hlen = 2 * sizeof(uint32_t);
		break;
	}
	if (mlen >= sizeof(uint32_t)) {
		uint32_t status = *((uint32_t *) (mp->b_rptr + hlen));

		switch (status) {
		case M2PA_STATUS_BUSY_ENDED:
			m2palogrx(sl, "<- BUSY-ENDED");
			return sl_lsc_status_busy_ended(sl, q);
		case M2PA_STATUS_IN_SERVICE:
			m2palogrx(sl, "<- IN-SERVICE");
			return sl_lsc_status_in_service(sl, q, mp);
		case M2PA_STATUS_PROCESSOR_OUTAGE_ENDED:
			m2palogrx(sl, "<- PROCESSOR-ENDED");
			return sl_lsc_status_processor_outage_ended(sl, q, mp);
		case M2PA_STATUS_OUT_OF_SERVICE:
			m2palogrx(sl, "<- OUT-OF-SERVICE");
			return sl_lsc_status_out_of_service(sl, q);
		case M2PA_STATUS_PROCESSOR_OUTAGE:
			m2palogrx(sl, "<- PROCESSOR-OUTAGE");
			return sl_lsc_status_processor_outage(sl, q);
		case M2PA_STATUS_BUSY:
			m2palogrx(sl, "<- BUSY");
			return sl_lsc_status_busy(sl, q);
		case M2PA_STATUS_ALIGNMENT:
			m2palogrx(sl, "<- ALIGNMENT");
			return sl_lsc_status_alignment(sl, q);
		case M2PA_STATUS_PROVING_NORMAL:
			m2palogrx(sl, "<- PROVING-NORMAL");
			return sl_lsc_status_proving_normal(sl, q);
		case M2PA_STATUS_PROVING_EMERGENCY:
			m2palogrx(sl, "<- PROVING-EMERGENCY");
			return sl_lsc_status_proving_emergency(sl, q);
		case M2PA_STATUS_ACK:
			m2palogrx(sl, "<- ACK");
			return sl_rc_signal_unit(sl, q, mp);
		}
		rare();
		return (-EPROTO);
	}
	rare();
	return (-EMSGSIZE);
}

/*
 *  M2PA RECV DATA
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_recv_data(struct sl *sl, queue_t *q, mblk_t *mp)
{
	size_t hlen;			/* M2PA-specific header length */
	size_t rlen;			/* Routing Label length */
	size_t mlen = mp->b_wptr > mp->b_rptr ? mp->b_wptr - mp->b_rptr : 0;

	switch (sl->i_version) {
	case M2PA_VERSION_DRAFT3:
	case M2PA_VERSION_DRAFT3_1:
		hlen = 0;
		break;
	case M2PA_VERSION_DRAFT4:
	case M2PA_VERSION_DRAFT4_1:
		hlen = 2 * sizeof(uint16_t);
		break;
	case M2PA_VERSION_DRAFT4_9:
	case M2PA_VERSION_DRAFT5:
	case M2PA_VERSION_DRAFT6:
	case M2PA_VERSION_DRAFT6_1:
	case M2PA_VERSION_DRAFT6_9:
	default:
	case M2PA_VERSION_DRAFT10:
	case M2PA_VERSION_DRAFT11:
	case M2PA_VERSION_RFC4165:
		/* In the final, sequence numbers are 24 bit values in a 32 bit field. */
		hlen = 2 * sizeof(uint32_t);
		break;
	}
	switch (sl->option.pvar) {
	default:
	case SS7_PVAR_ITUT_88:
	case SS7_PVAR_ITUT_93:
	case SS7_PVAR_ITUT_96:
	case SS7_PVAR_ITUT_00:
	case SS7_PVAR_ETSI_88:
	case SS7_PVAR_ETSI_93:
	case SS7_PVAR_ETSI_96:
	case SS7_PVAR_ETSI_00:
		rlen = 5;
		break;
	case SS7_PVAR_ANSI_88:
	case SS7_PVAR_ANSI_92:
	case SS7_PVAR_ANSI_96:
	case SS7_PVAR_ANSI_00:
	case SS7_PVAR_JTTC_94:
		rlen = 7;
		break;
	}
	switch (sl->i_version) {
	case M2PA_VERSION_DRAFT3:
	case M2PA_VERSION_DRAFT3_1:
	case M2PA_VERSION_DRAFT4:
	case M2PA_VERSION_DRAFT4_1:
	case M2PA_VERSION_DRAFT4_9:
	case M2PA_VERSION_DRAFT5:
		if (mlen > hlen + rlen || mlen == 0) {
			m2palogda(sl, "<- DATA");
			return sl_rc_signal_unit(sl, q, mp);
		}
		break;
	case M2PA_VERSION_DRAFT6:
	case M2PA_VERSION_DRAFT6_1:
		/* draft6 and anticipated draft7 permit acknowledgement using zero payload data
		   messages. */
	case M2PA_VERSION_DRAFT6_9:
	default:
	case M2PA_VERSION_DRAFT10:
	case M2PA_VERSION_DRAFT11:
	case M2PA_VERSION_RFC4165:
		/* In the final, zero-length data messages are used as spearate acknowledgements. */
		if (mlen > hlen + rlen || mlen == hlen || mlen == 0) {
			m2palogda(sl, "<- DATA");
			return sl_rc_signal_unit(sl, q, mp);
		}
		break;
	}
	rare();
	return (-EMSGSIZE);
}

/*
 *  M2PA RECV
 *  ---------------------------------------------
 */
STATIC int
sl_recv_msg(struct sl *sl, queue_t *q, mblk_t *mp)
{
	uint32_t mlen;
	uint32_t type;

	if (mp->b_wptr >= mp->b_rptr + 2 * sizeof(uint32_t)) {
		int err;
		unsigned char *oldp = mp->b_rptr;

		type = *(uint32_t *) mp->b_rptr;
		mp->b_rptr += sizeof(uint32_t);
		switch (type) {
		case M2PA_DATA_MESSAGE:
		case M2PA_STATUS_MESSAGE:
		case M2PA_PROVING_MESSAGE:
		case M2PA_ACK_MESSAGE:
			break;
		default:
			goto eproto;
		}

		mlen = *(uint32_t *) mp->b_rptr;
		mp->b_rptr += sizeof(uint32_t);
		m2palogda(sl, "message length before conversion %d", (int) mlen);
		mlen = ntohl(mlen);
		m2palogda(sl, "message length after conversion %d", (int) mlen);
		m2palogda(sl, "message length with padding %d", (int) PAD4(mlen));

		if (mp->b_wptr - oldp != PAD4(mlen))
			goto emsgsize;

		switch (type) {
		case M2PA_DATA_MESSAGE:
			err = sl_recv_data(sl, q, mp);
			break;
		case M2PA_STATUS_MESSAGE:
			err = sl_recv_status(sl, q, mp);
			break;
		case M2PA_PROVING_MESSAGE:
			err = sl_recv_proving(sl, q, mp);
			break;
		case M2PA_ACK_MESSAGE:
			err = sl_recv_datack(sl, q, mp);
			break;
		default:
			goto eproto;
		}
		if (err < 0)
			mp->b_rptr = oldp;
		return (err);
	      emsgsize:
		m2paloger(sl, "Bad message size %d != %d", (int) (mp->b_wptr - oldp),
			  (int) PAD4(mlen));
		return (-EMSGSIZE);
	}
      eproto:
	rare();
	return (-EPROTO);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  NP User -> NP Provider Primitives
 *
 *  -------------------------------------------------------------------------
 */

#if 0
/**
 * ne_conn_req: - process N_CONN_REQ primitive
 * @sl: private structure
 * @q: active queue (write queue)
 * @mp: the N_CONN_REQ primitive
 */
static int
ne_conn_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (sl_not_n_state(sl, NSF_IDLE))
		goto outstate;
	return n_pass_down(sl, q, mp, NS_WCON_CREQ, LMI_ENABLE_PENDING);
      outstate:
	return n_error_ack(sl, q, N_CONN_REQ, NOUTSTATE);
}

/**
 * ne_conn_res: - process N_CONN_RES primitive
 * @sl: private structure
 * @q: active queue (write queue)
 * @mp: the N_CONN_RES primitive
 */
static int
ne_conn_res(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (sl_not_n_state(sl, NSF_WRES_CIND))
		goto outstate;
	return n_pass_down(sl, q, mp, NS_WACK_CRES, LMI_ENABLE_PENDING);
      outstate:
	return n_error_ack(sl, q, N_CONN_RES, NOUTSTATE);
}

/**
 * ne_discon_req: - process N_DISCON_REQ primitive
 * @sl: private structure
 * @q: active queue (write queue)
 * @mp: the N_DISCON_REQ primitive
 */
static int
ne_discon_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (sl_not_n_state(sl, (NSF_WCON_CREQ | NSF_WRES_CIND | NSF_DATA_XFER |
				NSF_WCON_RREQ | NSF_WRES_RIND)))
		goto outstate;
	switch (sl_get_n_state(sl)) {
	case NS_WCON_CREQ:
		return n_pass_down(sl, q, mp, NS_WACK_DREQ6, LMI_DISABLE_PENDING);
	case NS_WRES_CIND:
		return n_pass_down(sl, q, mp, NS_WACK_DREQ7, LMI_DISABLE_PENDING);
	case NS_DATA_XFER:
		return n_pass_down(sl, q, mp, NS_WACK_DREQ9, LMI_DISABLE_PENDING);
	case NS_WCON_RREQ:
		return n_pass_down(sl, q, mp, NS_WACK_DREQ10, LMI_DISABLE_PENDING);
	case NS_WRES_RIND:
		return n_pass_down(sl, q, mp, NS_WACK_DREQ11, LMI_DISABLE_PENDING);
	}
	goto outstate;
      outstate:
	return n_error_ack(sl, q, N_DISCON_REQ, NOUTSTATE);
}

STATIC int m_error_reply(struct sl *sl, queue_t *q, int err);

/**
 * ne_data_req: - process N_DATA_REQ primitive
 * @sl: private structure
 * @q: active queue (write queue)
 * @mp: the N_DATA_REQ primitive
 */
static int
ne_data_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (sl_chk_n_state(sl, NSF_IDLE))
		goto discard;
	if (sl_not_n_state(sl, NSF_DATA_XFER))
		goto outstate;
	return n_pass_down(sl, q, mp, sl_get_n_state(sl), sl_get_i_state(sl));
      outstate:
	return m_error_reply(sl, q, -EPROTO);
      discard:
	return (QR_DONE);
}

/**
 * ne_exdata_req: - process N_EXDATA_REQ primitive
 * @sl: private structure
 * @q: active queue (write queue)
 * @mp: the N_EXDATA_REQ primitive
 */
static int
ne_exdata_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (sl_chk_n_state(sl, NSF_IDLE))
		goto discard;
	if (sl_not_n_state(sl, NSF_DATA_XFER))
		goto outstate;
	return n_pass_down(sl, q, mp, sl_get_n_state(sl), sl_get_i_state(sl));
      outstate:
	return m_error_reply(sl, q, -EPROTO);
      discard:
	return (QR_DONE);
}

/**
 * ne_info_req: - process N_INFO_REQ primitive
 * @sl: private structure
 * @q: active queue (write queue)
 * @mp: the N_INFO_REQ primitive
 */
static int
ne_info_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	return n_pass_down(sl, q, mp, sl_get_n_state(sl), sl_get_i_state(sl));
}

/**
 * ne_bind_req: - process N_BIND_REQ primitive
 * @sl: private structure
 * @q: active queue (write queue)
 * @mp: the N_BIND_REQ primitive
 */
static int
ne_bind_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (sl_not_n_state(sl, NSF_UNBND))
		goto outstate;
	return n_pass_down(sl, q, mp, NS_WACK_BREQ, LMI_ATTACH_PENDING);
      outstate:
	return n_error_ack(sl, q, N_BIND_REQ, NOUTSTATE);
}

/**
 * ne_unbind_req: - process N_UNBIND_REQ primitive
 * @sl: private structure
 * @q: active queue (write queue)
 * @mp: the N_UNBIND_REQ primitive
 */
static int
ne_unbind_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (sl_not_n_state(sl, NSF_IDLE))
		goto outstate;
	return n_pass_down(sl, q, mp, NS_WACK_UREQ, LMI_DETACH_PENDING);
      outstate:
	return n_error_ack(sl, q, N_UNBIND_REQ, NOUTSTATE);
}

/**
 * ne_unitdata_req: - process N_UNITDATA_REQ primitive
 * @sl: private structure
 * @q: active queue (write queue)
 * @mp: the N_UNITDATA_REQ primitive
 */
static int
ne_unitdata_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (sl_not_n_state(sl, NSF_IDLE))
		goto outstate;
	return n_pass_down(sl, q, mp, sl_get_n_state(sl), sl_get_i_state(sl));
      outstate:
	return m_error_reply(sl, q, -EPROTO);
}

/**
 * ne_optmgmt_req: - process N_OPTMGMT_REQ primitive
 * @sl: private structure
 * @q: active queue (write queue)
 * @mp: the N_OPTMGMT_REQ primitive
 */
static int
ne_optmgmt_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (sl_get_n_state(sl) == NS_IDLE)
		return n_pass_down(sl, q, mp, NS_WACK_OPTREQ, sl_get_i_state(sl));
	return n_pass_down(sl, q, mp, sl_get_n_state(sl), sl_get_i_state(sl));
}

/**
 * ne_datack_req: - process N_DATACK_REQ primitive
 * @sl: private structure
 * @q: active queue (write queue)
 * @mp: the N_DATACK_REQ primitive
 */
static int
ne_datack_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (sl_get_n_state(sl) != NS_IDLE)
		goto outstate;
	return n_pass_down(sl, q, mp, sl_get_n_state(sl), sl_get_i_state(sl));
      outstate:
	return m_error_reply(sl, q, -EPROTO);
}

/**
 * ne_reset_req: - process N_RESET_REQ primitive
 * @sl: private structure
 * @q: active queue (write queue)
 * @mp: the N_RESET_REQ primitive
 */
static int
ne_reset_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (sl_not_n_state(sl, NSF_DATA_XFER))
		goto outstate;
	return n_pass_down(sl, q, mp, NS_WCON_CREQ, LMI_ENABLED);
      outstate:
	return n_error_ack(sl, q, N_RESET_REQ, NOUTSTATE);
}

/**
 * ne_reset_res: - process N_RESET_RES primitive
 * @sl: private structure
 * @q: active queue (write queue)
 * @mp: the N_RESET_RES primitive
 */
static int
ne_reset_res(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (sl_not_n_state(sl, NSF_WRES_RIND))
		goto outstate;
	return n_pass_down(sl, q, mp, NS_WACK_RRES, LMI_ENABLED);
      outstate:
	return n_error_ack(sl, q, N_RESET_RES, NOUTSTATE);
}
#endif

/*
 *  -------------------------------------------------------------------------
 *
 *  SL User -> SL Provider (M2PA) Primitives
 *
 *  -------------------------------------------------------------------------
 */

/**
 * lmi_info_req: - process LMI_INFO_REQ primitive
 * @sl: private structure
 * @q: active queue (write queue)
 * @mp: the LMI_INFO_REQ primitive
 */
static int
lmi_info_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	return lmi_info_ack(sl, q);
}

/**
 * lmi_attach_req: - process LMI_ATTACH_REQ primitive
 * @sl: private structure
 * @q: active queue (write queue)
 * @mp: the LMI_ATTACH_REQ primitive
 *
 * The attach request was orignally unsupported in M2PA but there was a problem with data being
 * delivered to the Stream head when connected before the module was pushed, so attach is now
 * equivalent to a bind.  The PPA is the local sockaddr_in structure to which to bind.
 */
static int
lmi_attach_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_attach_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto protoshort;
	if (sl_get_i_state(sl) != LMI_UNATTACHED)
		goto outstate;
	if (!MBLKIN(mp, p->lmi_ppa_offset, p->lmi_ppa_length))
		goto badppa;
	return n_bind_req(sl, q, mp->b_rptr + p->lmi_ppa_offset, p->lmi_ppa_length);
badppa:
	m2palogrx(sl, "bad PPA");
	err = LMI_BADPPA;
	goto error;
      outstate:
	m2palogrx(sl, "would place interface out of state");
	err = LMI_OUTSTATE;
	goto error;
      protoshort:
	m2palogrx(sl, "M_PROTO block too short");
	err = LMI_PROTOSHORT;
	goto error;
      error:
	return lmi_error_ack(sl, q, LMI_ATTACH_REQ, err);
}

/**
 * lmi_detach_req: - process LMI_DETACH_REQ primitive
 * @sl: private structure
 * @q: active queue (write queue)
 * @mp: the LMI_DETACH_REQ primitive
 *
 * The detach request was originally unsupported in M2PA, but there was a problem with data being
 * delivered to the Stream head when connected before the module was pushed, so detach is now
 * equivalent to an unbind.
 */
static int
lmi_detach_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_detach_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (sl_get_i_state(sl) != LMI_DISABLED)
		goto outstate;
	return n_unbind_req(sl, q);
      outstate:
	m2palogrx(sl, "would place interface out of state");
	err = LMI_OUTSTATE;
	goto error;
      protoshort:
	m2palogrx(sl, "M_PROTO block too short");
	err = LMI_PROTOSHORT;
	goto error;
      error:
	return lmi_error_ack(sl, q, LMI_DETACH_REQ, err);
}

/**
 * lmi_enable_req: - process LMI_ENABLE_REQ primitive
 * @sl: private structure
 * @q: active queue (write queue)
 * @mp: the LMI_ENABLE_REQ primitive
 *
 * The enable request was originally a null operation in M2PA, but there was a problem with data
 * being delivered to the Stream head when connected before the module was pushed, so enable is now
 * equivalent to a connect.
 */
static int
lmi_enable_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_enable_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto protoshort;
	if (sl_get_i_state(sl) != LMI_DISABLED)
		goto outstate;
	if (!MBLKIN(mp, p->lmi_rem_offset, p->lmi_rem_length))
		goto badaddr;
	return n_conn_req(sl, q, (caddr_t) (p + 1), mp->b_wptr - mp->b_rptr - sizeof(*p));
      outstate:
	m2palogrx(sl, "would place interface out of state");
	err = LMI_OUTSTATE;
	goto error;
      badaddr:
	m2palogrx(sl, "bad address");
	err = LMI_BADADDRESS;
	goto error;
      protoshort:
	m2palogrx(sl, "M_PROTO block too short");
	err = LMI_PROTOSHORT;
	goto error;
      error:
	return lmi_error_ack(sl, q, LMI_ENABLE_REQ, err);
}

/**
 * lmi_disable_req: - process LMI_DISABLE_REQ primitive
 * @sl: private structure
 * @q: active queue (write queue)
 * @mp: the LMI_DISABLE_REQ primitive
 *
 * The disable request was originally a null operation in M2PA, but there was a problem with data
 * being delivered to the Stream head when connected before the module was pushed, so disable is now
 * equivalent to a disconnect.
 */
static int
lmi_disable_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_disable_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (sl_not_i_state(sl, (LMF_ENABLED | LMF_ENABLE_PENDING)))
		goto outstate;
	if ((err = sl_lsc_stop(sl, q)) < 0)
		return (err);
	return n_discon_req(sl, q, N_DISC_NORMAL);
      outstate:
	m2palogrx(sl, "would place interface out of state");
	err = LMI_OUTSTATE;
	goto error;
      protoshort:
	m2palogrx(sl, "M_PROTO block too short");
	err = LMI_PROTOSHORT;
	goto error;
      error:
	return lmi_error_reply(sl, q, LMI_DISABLE_REQ, err);
}

/*
 *  LMI_OPTMGMT_REQ
 *  ---------------------------------------------
 */
STATIC INLINE int
lmi_optmgmt_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	int err;
	lmi_optmgmt_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	goto notsupp;
      notsupp:
	m2palogrx(sl, "Primitive is not supported");
	err = LMI_NOTSUPP;
	goto error;
      protoshort:
	m2palogrx(sl, "M_PROTO block too short");
	err = LMI_PROTOSHORT;
	goto error;
      error:
	return lmi_error_ack(sl, q, LMI_OPTMGMT_REQ, err);
}

/*
 *  M_ERROR Reply
 *  ---------------------------------------------
 */
#ifndef abs
#define abs(x) ((x)<0 ? -(x):(x))
#endif
STATIC int
m_error_reply(struct sl *sl, queue_t *q, int err)
{
	mblk_t *mp;

	switch (err) {
	case QR_DONE:
	case QR_ABSORBED:
	case QR_TRIMMED:
	case QR_LOOP:
	case QR_PASSALONG:
	case QR_PASSFLOW:
	case QR_DISABLE:
		m2paloger(sl, "Shouldn't pass Q returns to m_error_reply");
	case -EBUSY:
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
		return (err);
	}
	if ((mp = ss7_allocb(q, 2, BPRI_HI))) {
		mp->b_datap->db_type = M_ERROR;
		*(mp->b_wptr)++ = abs(err);
		*(mp->b_wptr)++ = abs(err);
		m2palogtx(sl, "<- M_ERROR");
		putnext(sl->oq, mp);
		return (0);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SL_PDU_REQ
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_pdu_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	int err;
	mblk_t *dp;
	sl_pdu_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if ((dp = mp->b_cont) == NULL)
		goto badprim;
	if ((err = sl_lsc_pdu(sl, q, dp)) < 0)
		return (err);
	return (QR_TRIMMED);	/* data absorbed */
      badprim:
	m2palogrx(sl, "No M_DATA block");
	err = LMI_BADPRIM;
	goto error;
      protoshort:
	m2palogrx(sl, "M_PROTO block too short");
	err = LMI_PROTOSHORT;
	goto error;
      error:
	return m_error_reply(sl, q, err);
}

/*
 *  SL_EMERGENCY_REQ
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_emergency_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	int err;
	sl_emergency_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	return sl_lsc_emergency(sl, q);
      protoshort:
	m2palogrx(sl, "M_PROTO block too short");
	err = LMI_PROTOSHORT;
	goto error;
      error:
	return m_error_reply(sl, q, err);
}

/*
 *  SL_EMERGENCY_CEASES_REQ
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_emergency_ceases_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	int err;
	sl_emergency_ceases_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	return sl_lsc_emergency_ceases(sl, q);
      protoshort:
	m2palogrx(sl, "M_PROTO block too short");
	err = LMI_PROTOSHORT;
	goto error;
      error:
	return m_error_reply(sl, q, err);
}

/*
 *  SL_START_REQ
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_start_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	int err;
	sl_start_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	return sl_lsc_start(sl, q);
      protoshort:
	m2palogrx(sl, "M_PROTO block too short");
	err = LMI_PROTOSHORT;
	goto error;
      error:
	return m_error_reply(sl, q, err);
}

/*
 *  SL_STOP_REQ
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_stop_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	int err;
	sl_stop_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	return sl_lsc_stop(sl, q);
      protoshort:
	m2palogrx(sl, "M_PROTO block too short");
	err = LMI_PROTOSHORT;
	goto error;
      error:
	return m_error_reply(sl, q, err);
}

/*
 *  SL_RETRIEVE_BSNT_REQ
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_retrieve_bsnt_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	int err;
	sl_retrieve_bsnt_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	return sl_lsc_retrieve_bsnt(sl, q);
      protoshort:
	m2palogrx(sl, "M_PROTO block too short");
	err = LMI_PROTOSHORT;
	goto error;
      error:
	return m_error_reply(sl, q, err);
}

/*
 *  SL_RETRIEVAL_REQUEST_AND_FSNC_REQ
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_retrieval_request_and_fsnc_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	int err;
	sl_retrieval_req_and_fsnc_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	return sl_lsc_retrieval_request_and_fsnc(sl, q, p->sl_fsnc);
      protoshort:
	m2palogrx(sl, "M_PROTO block too short");
	err = LMI_PROTOSHORT;
	goto error;
      error:
	return m_error_reply(sl, q, err);
}

/*
 *  SL_RESUME_REQ
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_resume_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	int err;
	sl_resume_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	return sl_lsc_resume(sl, q);
      protoshort:
	m2palogrx(sl, "M_PROTO block too short");
	err = LMI_PROTOSHORT;
	goto error;
      error:
	return m_error_reply(sl, q, err);
}

/*
 *  SL_CLEAR_BUFFERS_REQ
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_clear_buffers_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	int err;
	sl_clear_buffers_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	return sl_lsc_clear_buffers(sl, q);
      protoshort:
	m2palogrx(sl, "M_PROTO block too short");
	err = LMI_PROTOSHORT;
	goto error;
      error:
	return m_error_reply(sl, q, err);
}

/*
 *  SL_CLEAR_RTB_REQ
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_clear_rtb_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	int err;
	sl_clear_rtb_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	return sl_lsc_clear_rtb(sl, q);
      protoshort:
	m2palogrx(sl, "M_PROTO block too short");
	err = LMI_PROTOSHORT;
	goto error;
      error:
	return m_error_reply(sl, q, err);
}

/*
 *  SL_LOCAL_PROCESSOR_OUTAGE_REQ
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_local_processor_outage_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	int err;
	sl_local_proc_outage_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	return sl_lsc_local_processor_outage(sl, q);
      protoshort:
	m2palogrx(sl, "M_PROTO block too short");
	err = LMI_PROTOSHORT;
	goto error;
      error:
	return m_error_reply(sl, q, err);
}

/*
 *  SL_CONGESTION_DISCARD_REQ
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_congestion_discard_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	int err;
	sl_cong_discard_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	return sl_lsc_congestion_discard(sl, q);
      protoshort:
	m2palogrx(sl, "M_PROTO block too short");
	err = LMI_PROTOSHORT;
	goto error;
      error:
	return m_error_reply(sl, q, err);
}

/*
 *  SL_CONGESTION_ACCEPT_REQ
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_congestion_accept_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	int err;
	sl_cong_accept_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	return sl_lsc_congestion_accept(sl, q);
      protoshort:
	m2palogrx(sl, "M_PROTO block too short");
	err = LMI_PROTOSHORT;
	goto error;
      error:
	return m_error_reply(sl, q, err);
}

/*
 *  SL_NO_CONGESTION_REQ
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_no_congestion_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	int err;
	sl_no_cong_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	return sl_lsc_no_congestion(sl, q);
      protoshort:
	m2palogrx(sl, "M_PROTO block too short");
	err = LMI_PROTOSHORT;
	goto error;
      error:
	return m_error_reply(sl, q, err);
}

/*
 *  SL_POWER_ON_REQ
 *  ---------------------------------------------
 */
STATIC INLINE int
sl_power_on_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	int err;
	sl_power_on_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	return sl_lsc_power_on(sl, q);
      protoshort:
	m2palogrx(sl, "M_PROTO block too short");
	err = LMI_PROTOSHORT;
	goto error;
      error:
	return m_error_reply(sl, q, err);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  NPI Provider -> NPI User (M2PA) Primitives
 *
 *  -------------------------------------------------------------------------
 */

/**
 * ne_bind_ack: - process received N_BIND_ACK primitive
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the N_BIND_ACK primitive
 */
STATIC int
ne_bind_ack(struct sl *sl, queue_t *q, mblk_t *mp)
{
	N_bind_ack_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (sl->n_request) {
		if (sl_get_n_state(sl) != NS_WACK_BREQ)
			goto outstate;
		bcopy(mp->b_rptr + p->ADDR_offset, &sl->loc, p->ADDR_length);
		sl->loc_len = p->ADDR_length;
		return n_pass_along(sl, q, mp, NS_IDLE, LMI_DISABLED);
	} else {
		if (sl_get_i_state(sl) != LMI_ATTACH_PENDING)
			goto outstate;
		bcopy(mp->b_rptr + p->ADDR_offset, &sl->loc, p->ADDR_length);
		sl->loc_len = p->ADDR_length;
		return lmi_ok_ack(sl, q, LMI_ATTACH_REQ);
	}
      protoshort:
	m2paloger(sl, "N_BIND_ACK received too short!");
	return (-EMSGSIZE);
      outstate:
	m2paloger(sl, "N_BIND_ACK received in incorrect state!");
	return (-EPROTO);
}

/**
 * ne_ok_ack: - process received N_OK_ACK primitive
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the N_OK_ACK primitive
 */
STATIC int
ne_ok_ack(struct sl *sl, queue_t *q, mblk_t *mp)
{
	N_ok_ack_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (sl->n_request) {
		switch (p->CORRECT_prim) {
		case N_UNBIND_REQ:
			if (sl_get_n_state(sl) != NS_WACK_UREQ)
				goto outstate;
			return n_pass_along(sl, q, mp, NS_UNBND, LMI_UNATTACHED);
		case N_DISCON_REQ:
			if (sl_not_n_state(sl, (NSF_WACK_DREQ6 | NSF_WACK_DREQ7 |
						NSF_WACK_DREQ9 | NSF_WACK_DREQ10 |
						NSF_WACK_DREQ11)))
				goto outstate;
			return n_pass_along(sl, q, mp, NS_IDLE, LMI_DISABLED);
		case N_OPTMGMT_REQ:
			if (sl_get_n_state(sl) == NS_WACK_OPTREQ)
				return n_pass_along(sl, q, mp, NS_IDLE, LMI_DISABLED);
			return n_pass_along(sl, q, mp, sl_get_n_state(sl), sl_get_i_state(sl));
		case N_CONN_RES:
			if (sl_get_n_state(sl) != NS_WACK_CRES)
				goto outstate;
			/* FIXME: NS_DATA_XFER if accepted on listening stream */
			if (--sl->coninds)
				return n_pass_along(sl, q, mp, NS_WRES_CIND, LMI_ENABLED);
			return n_pass_along(sl, q, mp, NS_IDLE, LMI_ENABLED);
		case N_RESET_RES:
			if (sl_get_n_state(sl) != NS_WACK_RRES)
				goto outstate;
			return n_pass_along(sl, q, mp, NS_DATA_XFER, LMI_ENABLED);
		default:
			m2paloger(sl, "%s() with invalid primitive %d", __FUNCTION__,
				  (int) p->CORRECT_prim);
			return n_pass_along(sl, q, mp, sl_get_n_state(sl), sl_get_i_state(sl));
		}
	} else {
		switch (p->CORRECT_prim) {
		case N_UNBIND_REQ:
			if (sl_get_i_state(sl) != LMI_DETACH_PENDING)
				goto outstate;
			return lmi_ok_ack(sl, q, LMI_DETACH_REQ);
		case N_DISCON_REQ:
			if (sl_get_i_state(sl) != LMI_DISABLE_PENDING)
				goto outstate;
			return lmi_disable_con(sl, q);
		case N_OPTMGMT_REQ:
			return (0);
		case N_CONN_RES:
		case N_RESET_RES:
		default:
			m2paloger(sl, "%s() with invalid primitive %d", __FUNCTION__,
				  (int) p->CORRECT_prim);
			return (0);
		}
	}
      protoshort:
	m2paloger(sl, "N_OK_ACK received too short!");
	return (-EMSGSIZE);
      outstate:
	m2paloger(sl, "Unpexected N_OK_ACK primitive!");
	return (-EPROTO);
}

/**
 * ne_conn_con: - process received N_CONN_CON primitive
 * @q: active queue (read queue)
 * @sl: private structure
 * @mp: the N_CONN_CON primitive
 */
STATIC int
ne_conn_con(struct sl *sl, queue_t *q, mblk_t *mp)
{
	N_conn_con_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (sl->n_request) {
		if (sl_get_n_state(sl) != NS_WCON_CREQ)
			goto outstate;
		return n_pass_along(sl, q, mp, NS_DATA_XFER, LMI_ENABLED);
	} else {
		if (sl_get_i_state(sl) != LMI_ENABLE_PENDING)
			goto outstate;
		return lmi_enable_con(sl, q);
	}
      protoshort:
	m2paloger(sl, "N_CONN_CON received too short!");
	return (-EMSGSIZE);
      outstate:
	m2paloger(sl, "N_CONN_CON primitive unexpected!");
	return (-EPROTO);
}

/**
 * ne_discon_ind: - process received N_DISCON_IND primitive
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the N_DISCON_IND primitive
 *
 * TODO: careful about flushing when sending N_DISCON_REQ.  When we receive a
 * flush from below, we should really not pass it to the Stream head but
 * should loop it back around.  Also when we receive a flush from the Stream
 * head, we should not pass it down, but loop it back to the Stream head, both
 * as though we were a multiplexing driver instead of a module.
 */
STATIC int
ne_discon_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	N_discon_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (p->RES_length != 0 && p->RES_length % sizeof(struct sockaddr_in) != 0)
		goto badaddr;
	if (sl_get_i_state(sl) == LMI_ENABLED) {
		/* double out of service ok, double notify not ok */
		if ((err = sl_lsc_out_of_service(sl, q, SL_FAIL_SUERM_EIM)) < 0)
			return (err);
		m2palogst(sl, "Link failed: SUERM/EIM");
		if (sl->sl.notify.events & SL_EVT_FAIL_SUERM_EIM)
			if ((err = lmi_event_ind(sl, q, SL_EVT_FAIL_SUERM_EIM, 0, NULL, 0)) < 0)
				return (err);
	}
	if (sl->n_request) {
		if (sl_not_n_state(sl, (NSF_WCON_CREQ | NSF_WRES_CIND | NSF_DATA_XFER |
					NSF_WCON_RREQ | NSF_WRES_RIND)))
			goto outstate;
		return n_pass_along(sl, q, mp, NS_IDLE, LMI_DISABLED);
	} else {
		switch (sl_get_i_state(sl)) {
		case LMI_ENABLE_PENDING:
			return lmi_error_ack(sl, q, LMI_ENABLE_REQ, -ECONNABORTED);
		case LMI_ENABLED:
			return lmi_error_ind(sl, q, LMI_DISC, true);
		case LMI_DISABLE_PENDING:
		case LMI_DISABLED:
		case LMI_UNUSABLE:
			/* ignore it, we are already disabled */
			goto discard;
		default:
			goto outstate;
		}
	}
      discard:
	return (QR_DONE);
      protoshort:
	m2paloger(sl, "N_CONN_CON received too short!");
	return (-EMSGSIZE);
      badaddr:
	m2paloger(sl, "N_CONN_CON contained badd address!");
	return (-EINVAL);
      outstate:
	m2paloger(sl, "N_CONN_CON primitive unexpected!");
	return (-EPROTO);
}

/**
 * ne_error_ack: - process received N_ERROR_ACK primitive
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the N_ERROR_ACK primitive
 */
STATIC int
ne_error_ack(struct sl *sl, queue_t *q, mblk_t *mp)
{
	N_error_ack_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (sl->n_request) {
		switch (p->ERROR_prim) {
		case N_BIND_REQ:
			if (sl_get_n_state(sl) != NS_WACK_BREQ)
				goto outstate;
			return n_pass_along(sl, q, mp, NS_UNBND, LMI_UNATTACHED);
		case N_UNBIND_REQ:
			if (sl_get_n_state(sl) != NS_WACK_UREQ)
				goto outstate;
			return n_pass_along(sl, q, mp, NS_IDLE, LMI_DISABLED);
		case N_CONN_REQ:
			if (sl_get_n_state(sl) != NS_WCON_CREQ)
				goto outstate;
			return n_pass_along(sl, q, mp, NS_IDLE, LMI_DISABLED);
		case N_CONN_RES:
			if (sl_get_n_state(sl) != NS_WACK_CRES)
				goto outstate;
			return n_pass_along(sl, q, mp, NS_WRES_CIND, LMI_ENABLE_PENDING);
		case N_RESET_REQ:
			if (sl_get_n_state(sl) != NS_WCON_RREQ)
				goto outstate;
			return n_pass_along(sl, q, mp, NS_DATA_XFER, LMI_ENABLED);
		case N_RESET_RES:
			if (sl_get_n_state(sl) != NS_WRES_RIND)
				goto outstate;
			return n_pass_along(sl, q, mp, NS_DATA_XFER, LMI_ENABLED);
		case N_DISCON_REQ:
			switch (sl_get_n_state(sl)) {
			case NS_WACK_DREQ6:
				return n_pass_along(sl, q, mp, NS_WCON_CREQ, LMI_ENABLED);
			case NS_WACK_DREQ7:
				return n_pass_along(sl, q, mp, NS_WRES_CIND, LMI_ENABLED);
			case NS_WACK_DREQ9:
				return n_pass_along(sl, q, mp, NS_DATA_XFER, LMI_ENABLED);
			case NS_WACK_DREQ10:
				return n_pass_along(sl, q, mp, NS_WCON_RREQ, LMI_ENABLED);
			case NS_WACK_DREQ11:
				return n_pass_along(sl, q, mp, NS_WRES_RIND, LMI_ENABLED);
			default:
				goto outstate;
			}
		default:
			m2paloger(sl, "ne_error_ack() called with invalid primitive %d",
				  (int) p->ERROR_prim);
			return n_pass_along(sl, q, mp, sl_get_n_state(sl), sl_get_i_state(sl));
		}
	} else {
		switch (p->ERROR_prim) {
		case N_BIND_REQ:
			if (sl_get_i_state(sl) != LMI_ATTACH_PENDING)
				goto outstate;
			return lmi_error_ack(sl, q, LMI_ATTACH_REQ, -EFAULT);
		case N_UNBIND_REQ:
			if (sl_get_i_state(sl) != LMI_DETACH_PENDING)
				goto outstate;
			return lmi_error_ack(sl, q, LMI_DETACH_REQ, -EFAULT);
		case N_CONN_REQ:
			if (sl_get_i_state(sl) != LMI_ENABLE_PENDING)
				goto outstate;
			return lmi_error_ack(sl, q, LMI_ENABLE_REQ, -EFAULT);
		case N_DISCON_REQ:
			if (sl_get_i_state(sl) != LMI_DISABLE_PENDING)
				goto outstate;
			return lmi_error_ack(sl, q, LMI_DISABLE_REQ, -EFAULT);
		case N_OPTMGMT_REQ:
			return lmi_error_ack(sl, q, LMI_OPTMGMT_REQ, -EFAULT);
		case N_INFO_REQ:
			return (-EFAULT);
		default:
			m2paloger(sl, "ne_error_ack() called with invalid primitive %d",
				  (int) p->ERROR_prim);
			return (QR_DONE);
		}
	}
      outstate:
	m2paloger(sl, "N_ERROR_ACK received in wrong state!");
	return (-EPROTO);
      protoshort:
	m2paloger(sl, "N_ERROR_ACK received too short!");
	return (-EMSGSIZE);
}

/**
 * ne_info_ack: - process received N_INFO_ACK primitive
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the N_INFO_ACK primitive
 */
STATIC int
ne_info_ack(struct sl *sl, queue_t *q, mblk_t *mp)
{
	N_info_ack_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	sl->info.n = *p;
	if (p->ADDR_length && p->ADDR_offset
	    && mp->b_wptr >= mp->b_rptr + p->ADDR_offset + p->ADDR_length) {
		bcopy(mp->b_rptr + p->ADDR_offset, &sl->loc, p->ADDR_length);
		sl->loc_len = p->ADDR_length;
	}
	/* Translate N information into LMI information */
	if (p->NSDU_size < sl->info.lm.lmi_max_sdu + sl->info.lm.lmi_header_len)
		sl->info.lm.lmi_max_sdu = p->NSDU_size - sl->info.lm.lmi_header_len;
	if (p->NIDU_size < sl->info.lm.lmi_max_sdu)
		sl->info.lm.lmi_max_sdu = p->NIDU_size - sl->info.lm.lmi_header_len;
	if (p->NODU_size < sl->info.lm.lmi_max_sdu)
		sl->info.lm.lmi_max_sdu = p->NODU_size - sl->info.lm.lmi_header_len;
	if (sl->info.lm.lmi_min_sdu > sl->info.lm.lmi_max_sdu)
		sl->info.lm.lmi_min_sdu = sl->info.lm.lmi_max_sdu;
	switch (p->CURRENT_state) {
	case NS_UNBND:
		sl_set_i_state(sl, LMI_UNATTACHED);
		break;
	case NS_WACK_BREQ:
		sl_set_i_state(sl, LMI_ATTACH_PENDING);
		break;
	case NS_WACK_UREQ:
		sl_set_i_state(sl, LMI_DETACH_PENDING);
		break;
	case NS_IDLE:
	case NS_WACK_OPTREQ:
		sl_set_i_state(sl, LMI_DISABLED);
		break;
	case NS_WCON_CREQ:
		sl_set_i_state(sl, LMI_ENABLE_PENDING);
		break;
	case NS_WACK_DREQ6:
	case NS_WACK_DREQ9:
	case NS_WACK_DREQ11:
		sl_set_i_state(sl, LMI_DISABLE_PENDING);
		break;
	case NS_WACK_RRES:
	case NS_DATA_XFER:
	case NS_WRES_RIND:
		sl_set_i_state(sl, LMI_ENABLED);
		break;
	default:
	case NS_WRES_CIND:
	case NS_WACK_DREQ7:
	case NS_WACK_CRES:
	case NS_WCON_RREQ:
	case NS_WACK_DREQ10:
		sl_set_i_state(sl, LMI_UNUSABLE);
		break;
	}
	sl_set_n_state(sl, p->CURRENT_state);
	if (sl->n_request)
		return n_pass_along(sl, q, mp, sl_get_n_state(sl), sl_get_i_state(sl));
	return (QR_DONE);
      protoshort:
	m2paloger(sl, "N_INFO_ACK received too short!");
	return (-EMSGSIZE);
}

/**
 * ne_data_ind_slow: - slow path processing for N_DATA_IND
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the N_DATA_IND primitive
 * 
 * Normally we receive data unfragmented and in a singled M_DATA block.  This slower routine handles
 * the circumstance where we receive fragmented data or data that is chained together in multiple
 * M_DATA blocks.  It also handles various slow path error conditions.
 */
static noinline fastcall __unlikely int
ne_data_ind_slow(struct sl *sl, queue_t *q, mblk_t *mp)
{
	N_data_ind_t *p = (typeof(p)) mp->b_rptr;
	mblk_t *dp, *bp, *newp = NULL;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (sl_not_i_state(sl, LMF_ENABLED))
		goto outstate;
	if ((dp = mp->b_cont) == NULL)
		goto baddata;
	if (dp->b_cont != NULL) {
		/* We have chained M_DATA blocks: pull them up. */
		if (!pullupmsg(dp, -1)) {
			/* normall only fail because of dup'ed blocks */
			if (!(newp = msgpullup(dp, -1))) {
				/* normaly only fail because of no buffer */
				ss7_bufcall(q, xmsgsize(dp), BPRI_MED);
				return (-ENOBUFS);
			}
			dp = newp;
		}
	}
	if (p->DATA_xfer_flags & N_MORE_DATA_FLAG) {
		/* We have a partial delivery.  Chain normal message together.  We might have a
		   problem with messages split over multiple streams? Treat normal and expedited
		   separately. */
		sl->rbuf = linkmsg(sl->rbuf, dp);
	} else {
		bp = linkmsg(XCHG(&sl->rbuf, NULL), dp);
		if ((err = sl_recv_msg(sl, q, bp)) != QR_ABSORBED) {
			if (err < 0) {
				sl->rbuf = unlinkmsg(sl->rbuf, dp);
				if (newp)
					freemsg(newp);
			}
			return (err);
		}
	}
	return (newp ? QR_DONE : QR_TRIMMED);

      protoshort:
	m2paloger(sl, "N_DATA_IND primitive too short!");
	err = -EFAULT;
	goto error;
      outstate:
	m2paloger(sl, "N_DATA_IND received in unexpected state!");
	err = -EFAULT;
	goto error;
      baddata:
	m2paloger(sl, "N_DATA_IND received with no data!");
	err = -EFAULT;
	goto error;
      error:
	return (err);
}

/**
 * ne_data_ind: - fast path processing for N_DATA_IND
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the N_DATA_IND primitive
 */
static inline fastcall __hot_in int
ne_data_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	N_data_ind_t *p = (typeof(p)) mp->b_rptr;
	mblk_t *dp;
	int err;

	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto go_slow;
	if (unlikely(sl_not_i_state(sl, LMF_ENABLED)))
		goto go_slow;
	if (unlikely((dp = mp->b_cont) == NULL))
		goto go_slow;
	if (unlikely(p->DATA_xfer_flags & N_MORE_DATA_FLAG))
		goto go_slow;
	if (unlikely(sl->rbuf != NULL))
		goto go_slow;
	if ((err = sl_recv_msg(sl, q, dp)) == QR_ABSORBED)
		return (QR_TRIMMED);	/* data only */
	return (err);

      go_slow:
	return ne_data_ind_slow(sl, q, mp);

}

/**
 * ne_exdata_ind_slow: - slow path processing for N_EXDATA_IND
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the N_EXDATA_IND primitive
 * 
 * Normally we receive data unfragmented and in a singled M_DATA block.  This slower routine handles
 * the circumstance where we receive fragmented data or data that is chained together in multiple
 * M_DATA blocks.  It also handles other slow path error conditions.
 */
static noinline fastcall __unlikely int
ne_exdata_ind_slow(struct sl *sl, queue_t *q, mblk_t *mp)
{
	N_exdata_ind_t *p = (typeof(p)) mp->b_rptr;
	mblk_t *dp, *newp = NULL;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (sl_get_n_state(sl) != NS_DATA_XFER)
		goto outstate;
	if (sl_get_i_state(sl) != LMI_ENABLED)
		goto outstate;
	if ((dp = mp->b_cont) == NULL)
		goto baddata;
	if (dp->b_cont != NULL) {
		/* We have chaned M_DATA blocks: pull them up. */
		/* normally only fails because of dup'ed blocks */
		if (!pullupmsg(dp, -1)) {
			/* normally only fails because of buffer allocation failure */
			if (!(newp = msgpullup(dp, -1))) {
				ss7_bufcall(q, xmsgsize(dp), BPRI_MED);
				return (-ENOBUFS);
			}
			dp = newp;
		}
	}
	if ((err = sl_recv_msg(sl, q, dp)) == QR_ABSORBED)
		return (newp ? QR_DONE : QR_TRIMMED);
	if (err < 0 && newp)
		freemsg(newp);
	return (err);

      protoshort:
	m2paloger(sl, "N_EXDATA_IND primitive too short!");
	err = -EFAULT;
	goto error;
      outstate:
	m2paloger(sl, "N_EXDATA_IND received in unexpected state!");
	err = -EFAULT;
	goto error;
      baddata:
	m2paloger(sl, "N_EXDATA_IND received with no data!");
	err = -EFAULT;
	goto error;
      error:
	return (err);
}

/**
 * ne_exdata_ind: - fast path processing for N_EXDATA_IND
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the N_EXDATA_IND primitive
 */
static inline fastcall __hot_in int
ne_exdata_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	N_exdata_ind_t *p = (typeof(p)) mp->b_rptr;
	mblk_t *dp;
	int err;

	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto go_slow;
	if (unlikely(sl_get_n_state(sl) != NS_DATA_XFER))
		goto go_slow;
	if (unlikely(sl_get_i_state(sl) != LMI_ENABLED))
		goto go_slow;
	if (unlikely((dp = mp->b_cont) == NULL))
		goto go_slow;
	if (unlikely(dp->b_cont != NULL))
		goto go_slow;

	if ((err = sl_recv_msg(sl, q, dp)) == QR_ABSORBED)
		return (QR_TRIMMED);	/* data only */
	return (err);

      go_slow:
	return ne_exdata_ind_slow(sl, q, mp);
}

/**
 * ne_datack_ind: - processing for N_DATACK_IND
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the N_DATACK_IND primitive
 *
 * For M2PA draft 03 and before, messages were not acknowledged by the M2PA peer with ack messages
 * but an acknowlegement at the SCTP level was considered an acknowledgement for M2PA as well.  When
 * sending in that draft mode, M2PA sets the receive confirmation flag and obtains the SCTP ack
 * indication that is used to clear the retransmission buffer.  Each ack represents the earliest
 * outstanding data message, and only one message.
 */
STATIC INLINE int
ne_datack_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	N_datack_ind_t *p;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (sl_not_i_state(sl, NSF_DATA_XFER))
		goto outstate;
	if (sl->i_version >= M2PA_VERSION_DRAFT3_1)
		goto unexpected;
	if ((err = sl_txc_datack(sl, q, 1)) < 0)
		return (err);
	return (0);
      unexpected:
	m2paloger(sl, "N_DATACK_IND unexpected in any state!");
	err = -EPROTO;
	goto error;
      outstate:
	m2paloger(sl, "N_DATACK_IND received in unexpected state!");
	err = -EPROTO;
	goto error;
      protoshort:
	m2paloger(sl, "N_DATACK_IND too short!");
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * ne_reset_ind: - processing for N_RESET_IND
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the N_RESET_IND primitive
 *
 * NPI SCTP only sends N_RESET_IND when an association has restarted.  In that case, the other end
 * should be sending SIOS very soon, or if it can muddle on from a previous state, then all is well.
 * Otherwise, link semantics and timers should bring the link down if there is a problem.  So, we
 * will ignore this situation for the moment.
 */
static noinline fastcall __unlikely int
ne_reset_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	N_reset_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (sl_not_i_state(sl, (LMF_ENABLED | LMF_UNUSABLE)))
		goto outstate;
	/* just ignore it */
	return (0);
      outstate:
	m2paloger(sl, "N_RESET_IND received in unexpected state!");
	err = -EPROTO;
	goto error;
      protoshort:
	m2paloger(sl, "N_RESET_IND too short!");
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * ne_reset_con: - processing for N_RESET_CON
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the N_RESET_CON primitive
 *
 * There is no mechanism in the SL interface for confirming a reset request, pass the confirmation
 * along.
 */
static noinline fastcall __unlikely int
ne_reset_con(struct sl *sl, queue_t *q, mblk_t *mp)
{
	N_reset_con_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (sl_not_i_state(sl, NSF_WCON_RREQ))
		goto outstate;
	return n_pass_along(sl, q, mp, NS_DATA_XFER, LMI_ENABLED);
      outstate:
	m2paloger(sl, "N_RESET_CON received in unexpected state!");
	err = -EPROTO;
	goto error;
      protoshort:
	m2paloger(sl, "N_RESET_CON too short!");
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * ne_conn_ind: - processing for N_CONN_IND
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the N_CONN_IND primitive
 *
 * There is no mechanism in the SL interface for providing a connection indication, however, if the
 * provider was bound with CONIND_number greater than zero, pass the connection indication primitive
 * along.
 */
static noinline fastcall __unlikely int
ne_conn_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	N_conn_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (sl_not_n_state(sl, (NSF_IDLE | NSF_WRES_CIND)))
		goto outstate;
	sl->coninds++;
	if ((err = n_pass_along(sl, q, mp, NS_WRES_CIND, LMI_ENABLE_PENDING)) != QR_ABSORBED)
		sl->coninds--;
	return (err);
      outstate:
	m2paloger(sl, "N_RESET_CON received in unexpected state!");
	err = -EPROTO;
	goto error;
      protoshort:
	m2paloger(sl, "N_RESET_CON too short!");
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/*
 *  =========================================================================
 *
 *  IO Controls
 *
 *  =========================================================================
 *
 *  SL IO Controls
 *
 *  -------------------------------------------------------------------------
 */
#if 0
STATIC int
sl_test_config(struct sl *sl, sl_config_t * arg)
{
	fixme(("%s: %p: FIXME: write this function\n", MOD_NAME, sl));
	return (-EOPNOTSUPP);
}
STATIC int
sl_commit_config(struct sl *sl, sl_config_t * arg)
{
	fixme(("%s: %p: FIXME: write this function\n", MOD_NAME, sl));
	return (-EOPNOTSUPP);
}
#endif
STATIC int
sl_iocgoptions(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		unsigned long flags;

		spin_lock_irqsave(&sl->lock, flags);
		{
			*arg = sl->option;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_iocsoptions(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		unsigned long flags;

		spin_lock_irqsave(&sl->lock, flags);
		{
			sl->option = *arg;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_iocgconfig(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		sl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		unsigned long flags;

		spin_lock_irqsave(&sl->lock, flags);
		{
			*arg = sl->sl.config;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_iocsconfig(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		sl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		unsigned long flags;

		spin_lock_irqsave(&sl->lock, flags);
		{
			sl->sl.config = *arg;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_ioctconfig(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		sl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		unsigned long flags;

		spin_lock_irqsave(&sl->lock, flags);
		{
			(void) arg;
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_ioccconfig(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		sl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		unsigned long flags;

		spin_lock_irqsave(&sl->lock, flags);
		{
			(void) arg;
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_iocgstatem(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		sl_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		unsigned long flags;

		spin_lock_irqsave(&sl->lock, flags);
		{
			*arg = sl->sl.statem;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_ioccmreset(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		sl_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		unsigned long flags;

		spin_lock_irqsave(&sl->lock, flags);
		{
			bzero(&sl->sl.statem, sizeof(sl->sl.statem));
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_iocgstatsp(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		sl_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		unsigned long flags;

		spin_lock_irqsave(&sl->lock, flags);
		{
			*arg = sl->sl.statsp;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_iocsstatsp(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		sl_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		unsigned long flags;

		spin_lock_irqsave(&sl->lock, flags);
		{
			sl->sl.statsp = *arg;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_iocgstats(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		sl_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		unsigned long flags;

		spin_lock_irqsave(&sl->lock, flags);
		{
			*arg = sl->sl.stats;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_ioccstats(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		sl_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		unsigned long flags;

		spin_lock_irqsave(&sl->lock, flags);
		{
			bzero(&sl->sl.stats, sizeof(sl->sl.stats));
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_iocgnotify(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		sl_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		unsigned long flags;

		spin_lock_irqsave(&sl->lock, flags);
		{
			*arg = sl->sl.notify;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_iocsnotify(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		sl_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		unsigned long flags;

		spin_lock_irqsave(&sl->lock, flags);
		{
			sl->sl.notify.events |= arg->events;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_ioccnotify(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		sl_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		unsigned long flags;

		spin_lock_irqsave(&sl->lock, flags);
		{
			sl->sl.notify.events &= ~(arg->events);
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SDT IO Controls
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sdt_test_config(struct sl *sl, sdt_config_t * arg)
{
	int ret = 0;
	unsigned long flags;

	spin_lock_irqsave(&sl->lock, flags);
	do {
		if (!arg->t8)
			arg->t8 = sl->sdt.config.t8;
		if (!arg->Tin)
			arg->Tin = sl->sdt.config.Tin;
		if (!arg->Tie)
			arg->Tie = sl->sdt.config.Tie;
		if (!arg->T)
			arg->T = sl->sdt.config.T;
		if (!arg->D)
			arg->D = sl->sdt.config.D;
		if (!arg->Te)
			arg->Te = sl->sdt.config.Te;
		if (!arg->De)
			arg->De = sl->sdt.config.De;
		if (!arg->Ue)
			arg->Ue = sl->sdt.config.Ue;
		if (!arg->N)
			arg->N = sl->sdt.config.N;
		if (!arg->m)
			arg->m = sl->sdt.config.m;
		if (!arg->b)
			arg->b = sl->sdt.config.b;
		if (!arg->f)
			arg->f = sl->sdt.config.f;
	} while (0);
	spin_unlock_irqrestore(&sl->lock, flags);
	return (ret);
}
STATIC int
sdt_commit_config(struct sl *sl, sdt_config_t * arg)
{
	unsigned long flags;

	spin_lock_irqsave(&sl->lock, flags);
	{
		sdt_test_config(sl, arg);
		sl->sdt.config = *arg;
	}
	spin_unlock_irqrestore(&sl->lock, flags);
	return (0);
}
STATIC int
sdt_iocgoptions(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		unsigned long flags;

		spin_lock_irqsave(&sl->lock, flags);
		{
			*arg = sl->option;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocsoptions(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		unsigned long flags;

		spin_lock_irqsave(&sl->lock, flags);
		{
			sl->option = *arg;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocgconfig(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		sdt_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		unsigned long flags;

		spin_lock_irqsave(&sl->lock, flags);
		{
			*arg = sl->sdt.config;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocsconfig(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		sdt_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		unsigned long flags;

		spin_lock_irqsave(&sl->lock, flags);
		{
			sl->sdt.config = *arg;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_ioctconfig(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sdt_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		return sdt_test_config(sl, arg);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_ioccconfig(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sdt_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		return sdt_commit_config(sl, arg);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocgstatem(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		sdt_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		unsigned long flags;

		spin_lock_irqsave(&sl->lock, flags);
		{
			(void) arg;
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_ioccmreset(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		sdt_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		unsigned long flags;

		spin_lock_irqsave(&sl->lock, flags);
		{
			(void) arg;
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocgstatsp(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		sdt_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		unsigned long flags;

		spin_lock_irqsave(&sl->lock, flags);
		{
			(void) arg;
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocsstatsp(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		sdt_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		unsigned long flags;

		spin_lock_irqsave(&sl->lock, flags);
		{
			(void) arg;
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocgstats(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		sdt_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		unsigned long flags;

		spin_lock_irqsave(&sl->lock, flags);
		{
			(void) arg;
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_ioccstats(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		sdt_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		unsigned long flags;

		spin_lock_irqsave(&sl->lock, flags);
		{
			(void) arg;
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocgnotify(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		sdt_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		unsigned long flags;

		spin_lock_irqsave(&sl->lock, flags);
		{
			(void) arg;
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocsnotify(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		sdt_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		unsigned long flags;

		spin_lock_irqsave(&sl->lock, flags);
		{
			(void) arg;
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_ioccnotify(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		sdt_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		unsigned long flags;

		spin_lock_irqsave(&sl->lock, flags);
		{
			(void) arg;
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_ioccabort(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		void *arg = (typeof(arg)) mp->b_cont->b_rptr;
		unsigned long flags;

		spin_lock_irqsave(&sl->lock, flags);
		{
			(void) arg;
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SDL IO Controls
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sdl_iocgoptions(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		unsigned long flags;

		spin_lock_irqsave(&sl->lock, flags);
		{
			*arg = sl->option;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_iocsoptions(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		unsigned long flags;

		spin_lock_irqsave(&sl->lock, flags);
		{
			sl->option = *arg;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_iocgconfig(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		sdl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		unsigned long flags;

		spin_lock_irqsave(&sl->lock, flags);
		{
			*arg = sl->sdl.config;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_iocsconfig(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		sdl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		unsigned long flags;

		spin_lock_irqsave(&sl->lock, flags);
		{
			sl->sdl.config = *arg;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_ioctconfig(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		sdl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		unsigned long flags;

		spin_lock_irqsave(&sl->lock, flags);
		{
			(void) arg;
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_ioccconfig(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		sdl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		unsigned long flags;

		spin_lock_irqsave(&sl->lock, flags);
		{
			(void) arg;
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_iocgstatem(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		sdl_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		unsigned long flags;

		spin_lock_irqsave(&sl->lock, flags);
		{
			(void) arg;
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_ioccmreset(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		sdl_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		unsigned long flags;

		spin_lock_irqsave(&sl->lock, flags);
		{
			(void) arg;
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_iocgstatsp(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		sdl_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		unsigned long flags;

		spin_lock_irqsave(&sl->lock, flags);
		{
			(void) arg;
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_iocsstatsp(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		sdl_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		unsigned long flags;

		spin_lock_irqsave(&sl->lock, flags);
		{
			(void) arg;
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_iocgstats(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		sdl_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		unsigned long flags;

		spin_lock_irqsave(&sl->lock, flags);
		{
			(void) arg;
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_ioccstats(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		sdl_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		unsigned long flags;

		spin_lock_irqsave(&sl->lock, flags);
		{
			(void) arg;
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_iocgnotify(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		sdl_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		unsigned long flags;

		spin_lock_irqsave(&sl->lock, flags);
		{
			(void) arg;
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_iocsnotify(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		sdl_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		unsigned long flags;

		spin_lock_irqsave(&sl->lock, flags);
		{
			(void) arg;
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_ioccnotify(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		sdl_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		unsigned long flags;

		spin_lock_irqsave(&sl->lock, flags);
		{
			(void) arg;
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sl->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_ioccdisctx(struct sl *sl, queue_t *q, mblk_t *mp)
{
	int ret = 0;
	unsigned long flags;

	spin_lock_irqsave(&sl->lock, flags);
	{
		ret = -EOPNOTSUPP;
	}
	spin_unlock_irqrestore(&sl->lock, flags);
	return (ret);
}
STATIC int
sdl_ioccconntx(struct sl *sl, queue_t *q, mblk_t *mp)
{
	int ret = 0;
	unsigned long flags;

	spin_lock_irqsave(&sl->lock, flags);
	{
		ret = -EOPNOTSUPP;
	}
	spin_unlock_irqrestore(&sl->lock, flags);
	return (ret);
}

/*
 *  =========================================================================
 *
 *  STREAMS Message Handling
 *
 *  =========================================================================
 *
 *  M_IOCTL Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sl_w_ioctl(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	struct iocblk *iocp = (struct iocblk *) mp->b_rptr;
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	int cmd = iocp->ioc_cmd, count = iocp->ioc_count;
	int type = _IOC_TYPE(cmd), nr = _IOC_NR(cmd), size = _IOC_SIZE(cmd);
	struct linkblk *lp = (struct linkblk *) arg;
	int ret = 0;

	switch (type) {
	case __SID:
	{
		switch (nr) {
		case _IOC_NR(I_STR):
		case _IOC_NR(I_LINK):
		case _IOC_NR(I_PLINK):
		case _IOC_NR(I_UNLINK):
		case _IOC_NR(I_PUNLINK):
			(void) lp;
			m2paloger(sl, "Unsupported STREAMS ioctl %d", (int) nr);
			ret = -EINVAL;
			break;
		default:
			m2paloger(sl, "Unsupported STREAMS ioctl %d", (int) nr);
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	}
	case SL_IOC_MAGIC:
	{
		if (count < size || sl_get_i_state(sl) == LMI_UNUSABLE) {
			m2paloger(sl, "ioctl count = %d, size = %d, state = %d", count, size,
				  sl_get_i_state(sl));
			ret = -EINVAL;
			break;
		}
		switch (nr) {
		case _IOC_NR(SL_IOCGOPTIONS):	/* lmi_option_t */
			ret = sl_iocgoptions(sl, q, mp);
			break;
		case _IOC_NR(SL_IOCSOPTIONS):	/* lmi_option_t */
			ret = sl_iocsoptions(sl, q, mp);
			break;
		case _IOC_NR(SL_IOCGCONFIG):	/* sl_config_t */
			ret = sl_iocgconfig(sl, q, mp);
			break;
		case _IOC_NR(SL_IOCSCONFIG):	/* sl_config_t */
			ret = sl_iocsconfig(sl, q, mp);
			break;
		case _IOC_NR(SL_IOCTCONFIG):	/* sl_config_t */
			ret = sl_ioctconfig(sl, q, mp);
			break;
		case _IOC_NR(SL_IOCCCONFIG):	/* sl_config_t */
			ret = sl_ioccconfig(sl, q, mp);
			break;
		case _IOC_NR(SL_IOCGSTATEM):	/* sl_statem_t */
			ret = sl_iocgstatem(sl, q, mp);
			break;
		case _IOC_NR(SL_IOCCMRESET):	/* sl_statem_t */
			ret = sl_ioccmreset(sl, q, mp);
			break;
		case _IOC_NR(SL_IOCGSTATSP):	/* lmi_sta_t */
			ret = sl_iocgstatsp(sl, q, mp);
			break;
		case _IOC_NR(SL_IOCSSTATSP):	/* lmi_sta_t */
			ret = sl_iocsstatsp(sl, q, mp);
			break;
		case _IOC_NR(SL_IOCGSTATS):	/* sl_stats_t */
			ret = sl_iocgstats(sl, q, mp);
			break;
		case _IOC_NR(SL_IOCCSTATS):	/* sl_stats_t */
			ret = sl_ioccstats(sl, q, mp);
			break;
		case _IOC_NR(SL_IOCGNOTIFY):	/* sl_notify_t */
			ret = sl_iocgnotify(sl, q, mp);
			break;
		case _IOC_NR(SL_IOCSNOTIFY):	/* sl_notify_t */
			ret = sl_iocsnotify(sl, q, mp);
			break;
		case _IOC_NR(SL_IOCCNOTIFY):	/* sl_notify_t */
			ret = sl_ioccnotify(sl, q, mp);
			break;
		default:
			m2paloger(sl, "Unspported SL ioctl %d", (int) nr);
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	}
	case SDT_IOC_MAGIC:
	{
		if (count < size || sl_get_i_state(sl) == LMI_UNUSABLE) {
			m2paloger(sl, "ioctl count = %d, size = %d, state = %d", count, size,
				  sl_get_i_state(sl));
			ret = -EINVAL;
			break;
		}
		switch (nr) {
		case _IOC_NR(SDT_IOCGOPTIONS):	/* lmi_option_t */
			ret = sdt_iocgoptions(sl, q, mp);
			break;
		case _IOC_NR(SDT_IOCSOPTIONS):	/* lmi_option_t */
			ret = sdt_iocsoptions(sl, q, mp);
			break;
		case _IOC_NR(SDT_IOCGCONFIG):	/* sdt_config_t */
			ret = sdt_iocgconfig(sl, q, mp);
			break;
		case _IOC_NR(SDT_IOCSCONFIG):	/* sdt_config_t */
			ret = sdt_iocsconfig(sl, q, mp);
			break;
		case _IOC_NR(SDT_IOCTCONFIG):	/* sdt_config_t */
			ret = sdt_ioctconfig(sl, q, mp);
			break;
		case _IOC_NR(SDT_IOCCCONFIG):	/* sdt_config_t */
			ret = sdt_ioccconfig(sl, q, mp);
			break;
		case _IOC_NR(SDT_IOCGSTATEM):	/* sdt_statem_t */
			ret = sdt_iocgstatem(sl, q, mp);
			break;
		case _IOC_NR(SDT_IOCCMRESET):	/* sdt_statem_t */
			ret = sdt_ioccmreset(sl, q, mp);
			break;
		case _IOC_NR(SDT_IOCGSTATSP):	/* lmi_sta_t */
			ret = sdt_iocgstatsp(sl, q, mp);
			break;
		case _IOC_NR(SDT_IOCSSTATSP):	/* lmi_sta_t */
			ret = sdt_iocsstatsp(sl, q, mp);
			break;
		case _IOC_NR(SDT_IOCGSTATS):	/* sdt_stats_t */
			ret = sdt_iocgstats(sl, q, mp);
			break;
		case _IOC_NR(SDT_IOCCSTATS):	/* sdt_stats_t */
			ret = sdt_ioccstats(sl, q, mp);
			break;
		case _IOC_NR(SDT_IOCGNOTIFY):	/* sdt_notify_t */
			ret = sdt_iocgnotify(sl, q, mp);
			break;
		case _IOC_NR(SDT_IOCSNOTIFY):	/* sdt_notify_t */
			ret = sdt_iocsnotify(sl, q, mp);
			break;
		case _IOC_NR(SDT_IOCCNOTIFY):	/* sdt_notify_t */
			ret = sdt_ioccnotify(sl, q, mp);
			break;
		case _IOC_NR(SDT_IOCCABORT):	/* */
			ret = sdt_ioccabort(sl, q, mp);
			break;
		default:
			m2paloger(sl, "Unspported SDT ioctl %d", (int) nr);
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	}
	case SDL_IOC_MAGIC:
	{
		if (count < size || sl_get_i_state(sl) == LMI_UNUSABLE) {
			m2paloger(sl, "ioctl count = %d, size = %d, state = %d", count, size,
				  sl_get_i_state(sl));
			ret = -EINVAL;
			break;
		}
		switch (nr) {
		case _IOC_NR(SDL_IOCGOPTIONS):	/* lmi_option_t */
			ret = sdl_iocgoptions(sl, q, mp);
			break;
		case _IOC_NR(SDL_IOCSOPTIONS):	/* lmi_option_t */
			ret = sdl_iocsoptions(sl, q, mp);
			break;
		case _IOC_NR(SDL_IOCGCONFIG):	/* sdl_config_t */
			ret = sdl_iocgconfig(sl, q, mp);
			break;
		case _IOC_NR(SDL_IOCSCONFIG):	/* sdl_config_t */
			ret = sdl_iocsconfig(sl, q, mp);
			break;
		case _IOC_NR(SDL_IOCTCONFIG):	/* sdl_config_t */
			ret = sdl_ioctconfig(sl, q, mp);
			break;
		case _IOC_NR(SDL_IOCCCONFIG):	/* sdl_config_t */
			ret = sdl_ioccconfig(sl, q, mp);
			break;
		case _IOC_NR(SDL_IOCGSTATEM):	/* sdl_statem_t */
			ret = sdl_iocgstatem(sl, q, mp);
			break;
		case _IOC_NR(SDL_IOCCMRESET):	/* sdl_statem_t */
			ret = sdl_ioccmreset(sl, q, mp);
			break;
		case _IOC_NR(SDL_IOCGSTATSP):	/* sdl_stats_t */
			ret = sdl_iocgstatsp(sl, q, mp);
			break;
		case _IOC_NR(SDL_IOCSSTATSP):	/* sdl_stats_t */
			ret = sdl_iocsstatsp(sl, q, mp);
			break;
		case _IOC_NR(SDL_IOCGSTATS):	/* sdl_stats_t */
			ret = sdl_iocgstats(sl, q, mp);
			break;
		case _IOC_NR(SDL_IOCCSTATS):	/* sdl_stats_t */
			ret = sdl_ioccstats(sl, q, mp);
			break;
		case _IOC_NR(SDL_IOCGNOTIFY):	/* sdl_notify_t */
			ret = sdl_iocgnotify(sl, q, mp);
			break;
		case _IOC_NR(SDL_IOCSNOTIFY):	/* sdl_notify_t */
			ret = sdl_iocsnotify(sl, q, mp);
			break;
		case _IOC_NR(SDL_IOCCNOTIFY):	/* sdl_notify_t */
			ret = sdl_ioccnotify(sl, q, mp);
			break;
		case _IOC_NR(SDL_IOCCDISCTX):	/* */
			ret = sdl_ioccdisctx(sl, q, mp);
			break;
		case _IOC_NR(SDL_IOCCCONNTX):	/* */
			ret = sdl_ioccconntx(sl, q, mp);
			break;
		default:
			m2paloger(sl, "Unspported SDL ioctl %d", (int) nr);
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	}
	default:
		return (QR_PASSALONG);
	}
	if (ret >= 0) {
		mp->b_datap->db_type = M_IOCACK;
		iocp->ioc_error = 0;
		iocp->ioc_rval = 0;
	} else {
		mp->b_datap->db_type = M_IOCNAK;
		iocp->ioc_error = -ret;
		iocp->ioc_rval = -1;
	}
	qreply(q, mp);
	return (QR_ABSORBED);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_PROTO, M_PCPROTO Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sl_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	sl_ulong prim;
	struct sl *sl = SL_PRIV(q);
	int oldnpistate = sl_get_n_state(sl);
	int oldlmistate = sl_get_i_state(sl);

	/* Fast Path */
	if ((prim = *(sl_ulong *) mp->b_rptr) == SL_PDU_REQ) {
		m2palogda(sl, "-> SL_PDU_REQ");
		if ((rtn = sl_pdu_req(sl, q, mp)) < 0) {
			sl_set_n_state(sl, oldnpistate);
			sl_set_i_state(sl, oldlmistate);
		}
		return (rtn);
	}
	switch (prim) {
#if 0
	case N_CONN_REQ:
		m2palogrx(sl, "-> N_CONN_REQ");
		rtn = ne_conn_req(sl, q, mp);
		break;
	case N_CONN_RES:
		m2palogrx(sl, "-> N_CONN_RES");
		rtn = ne_conn_res(sl, q, mp);
		break;
	case N_DISCON_REQ:
		m2palogrx(sl, "-> N_DISCON_REQ");
		rtn = ne_discon_req(sl, q, mp);
		break;
	case N_DATA_REQ:
		m2palogrx(sl, "-> N_DATA_REQ");
		rtn = ne_data_req(sl, q, mp);
		break;
	case N_EXDATA_REQ:
		m2palogrx(sl, "-> N_EXDATA_REQ");
		rtn = ne_exdata_req(sl, q, mp);
		break;
	case N_INFO_REQ:
		m2palogrx(sl, "-> N_INFO_REQ");
		rtn = ne_info_req(sl, q, mp);
		break;
	case N_BIND_REQ:
		m2palogrx(sl, "-> N_BIND_REQ");
		rtn = ne_bind_req(sl, q, mp);
		break;
	case N_UNBIND_REQ:
		m2palogrx(sl, "-> N_UNBIND_REQ");
		rtn = ne_unbind_req(sl, q, mp);
		break;
	case N_UNITDATA_REQ:
		m2palogrx(sl, "-> N_UNITDATA_REQ");
		rtn = ne_unitdata_req(sl, q, mp);
		break;
	case N_OPTMGMT_REQ:
		m2palogrx(sl, "-> N_OPTMGMT_REQ");
		rtn = ne_optmgmt_req(sl, q, mp);
		break;
	case N_DATACK_REQ:
		m2palogrx(sl, "-> N_DATACK_REQ");
		rtn = ne_datack_req(sl, q, mp);
		break;
	case N_RESET_REQ:
		m2palogrx(sl, "-> N_RESET_REQ");
		rtn = ne_reset_req(sl, q, mp);
		break;
	case N_RESET_RES:
		m2palogrx(sl, "-> N_RESET_RES");
		rtn = ne_reset_res(sl, q, mp);
		break;
#endif
	case LMI_INFO_REQ:
		m2palogrx(sl, "-> LMI_INFO_REQ");
		rtn = lmi_info_req(sl, q, mp);
		break;
	case LMI_ATTACH_REQ:
		m2palogrx(sl, "-> LMI_ATTACH_REQ");
		rtn = lmi_attach_req(sl, q, mp);
		break;
	case LMI_DETACH_REQ:
		m2palogrx(sl, "-> LMI_DETACH_REQ");
		rtn = lmi_detach_req(sl, q, mp);
		break;
	case LMI_ENABLE_REQ:
		m2palogrx(sl, "-> LMI_ENABLE_REQ");
		rtn = lmi_enable_req(sl, q, mp);
		break;
	case LMI_DISABLE_REQ:
		m2palogrx(sl, "-> LMI_DISABLE_REQ");
		rtn = lmi_disable_req(sl, q, mp);
		break;
	case LMI_OPTMGMT_REQ:
		m2palogrx(sl, "-> LMI_OPTMGMT_REQ");
		rtn = lmi_optmgmt_req(sl, q, mp);
		break;
	case SL_PDU_REQ:
		rtn = sl_pdu_req(sl, q, mp);
		m2palogda(sl, "-> SL_PDU_REQ");
		break;
	case SL_EMERGENCY_REQ:
		rtn = sl_emergency_req(sl, q, mp);
		m2palogrx(sl, "-> SL_EMERGENCY_REQ");
		break;
	case SL_EMERGENCY_CEASES_REQ:
		m2palogrx(sl, "-> SL_EMERGENCY_CEASES_REQ");
		rtn = sl_emergency_ceases_req(sl, q, mp);
		break;
	case SL_START_REQ:
		m2palogrx(sl, "-> SL_START_REQ");
		rtn = sl_start_req(sl, q, mp);
		break;
	case SL_STOP_REQ:
		m2palogrx(sl, "-> SL_STOP_REQ");
		rtn = sl_stop_req(sl, q, mp);
		break;
	case SL_RETRIEVE_BSNT_REQ:
		m2palogrx(sl, "-> SL_RETRIEVE_BSNT_REQ");
		rtn = sl_retrieve_bsnt_req(sl, q, mp);
		break;
	case SL_RETRIEVAL_REQUEST_AND_FSNC_REQ:
		m2palogrx(sl, "-> SL_RETRIEVAL_REQUEST_AND_FSNC_REQ");
		rtn = sl_retrieval_request_and_fsnc_req(sl, q, mp);
		break;
	case SL_RESUME_REQ:
		m2palogrx(sl, "-> SL_RESUME_REQ");
		rtn = sl_resume_req(sl, q, mp);
		break;
	case SL_CLEAR_BUFFERS_REQ:
		m2palogrx(sl, "-> SL_CLEAR_BUFFERS_REQ");
		rtn = sl_clear_buffers_req(sl, q, mp);
		break;
	case SL_CLEAR_RTB_REQ:
		m2palogrx(sl, "-> SL_CLEAR_RTB_REQ");
		rtn = sl_clear_rtb_req(sl, q, mp);
		break;
	case SL_LOCAL_PROCESSOR_OUTAGE_REQ:
		m2palogrx(sl, "-> SL_LOCAL_PROCESSOR_OUTAGE_REQ");
		rtn = sl_local_processor_outage_req(sl, q, mp);
		break;
	case SL_CONGESTION_DISCARD_REQ:
		m2palogrx(sl, "-> SL_CONGESTION_DISCARD_REQ");
		rtn = sl_congestion_discard_req(sl, q, mp);
		break;
	case SL_CONGESTION_ACCEPT_REQ:
		m2palogrx(sl, "-> SL_CONGESTION_ACCEPT_REQ");
		rtn = sl_congestion_accept_req(sl, q, mp);
		break;
	case SL_NO_CONGESTION_REQ:
		m2palogrx(sl, "-> SL_NO_CONGESTION_REQ");
		rtn = sl_no_congestion_req(sl, q, mp);
		break;
	case SL_POWER_ON_REQ:
		m2palogrx(sl, "-> SL_POWER_ON_REQ");
		rtn = sl_power_on_req(sl, q, mp);
		break;
	default:
		m2palogrx(sl, "-> (?????)");
		rtn = -EOPNOTSUPP;
		break;
	}
	if (rtn < 0) {
		sl_set_n_state(sl, oldnpistate);
		sl_set_i_state(sl, oldlmistate);
	}
	return (rtn);
}
STATIC int
sl_r_proto(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	int rtn;
	np_ulong prim;

	switch ((prim = *((np_ulong *) mp->b_rptr))) {
	case N_DATA_IND:
		m2palogda(sl, "N_DATA_IND <-");
		rtn = ne_data_ind(sl, q, mp);
		break;
	case N_EXDATA_IND:
		m2palogda(sl, "N_EXDATA_IND <-");
		rtn = ne_exdata_ind(sl, q, mp);
		break;
	case N_DATACK_IND:
		m2palogrx(sl, "N_DATACK_IND <-");
		rtn = ne_datack_ind(sl, q, mp);
		break;
	case N_DISCON_IND:
		m2palogrx(sl, "N_DISCON_IND <-");
		rtn = ne_discon_ind(sl, q, mp);
		break;
	case N_RESET_IND:
		m2palogrx(sl, "N_RESET_IND <-");
		rtn = ne_reset_ind(sl, q, mp);
		break;
	case N_CONN_CON:
		m2palogrx(sl, "N_CONN_CON <-");
		rtn = ne_conn_con(sl, q, mp);
		break;
	case N_INFO_ACK:
		m2palogrx(sl, "N_INFO_ACK <-");
		rtn = ne_info_ack(sl, q, mp);
		break;
	case N_BIND_ACK:
		m2palogrx(sl, "N_BIND_ACK <-");
		rtn = ne_bind_ack(sl, q, mp);
		break;
	case N_OK_ACK:
		m2palogrx(sl, "N_OK_ACK <-");
		rtn = ne_ok_ack(sl, q, mp);
		break;
	case N_ERROR_ACK:
		m2palogrx(sl, "N_ERROR_ACK <-");
		rtn = ne_error_ack(sl, q, mp);
		break;
	case N_CONN_IND:
		m2palogrx(sl, "N_CONN_IND <-");
		rtn = ne_conn_ind(sl, q, mp);
		break;
	case N_RESET_CON:
		m2palogrx(sl, "N_RESET_CON <-");
		rtn = ne_reset_con(sl, q, mp);
		break;
	default:
		m2palogrx(sl, "(????) <-");
		rtn = -EOPNOTSUPP;
		break;
	}
	if (rtn < 0) {
	}
	return (rtn);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_SIG/M_PCSIG Handling
 *
 *  -------------------------------------------------------------------------
 *  One little problem with mi_timers: we can never place them back on the
 *  queue because mi_timer_valid cannot be called twice for the same timer.
 */
STATIC noinline fastcall __unlikely int
sl_r_sig(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	int rtn = QR_ABSORBED;

	if (likely(mi_timer_valid(mp))) {
		switch (*(int *) mp->b_rptr) {
		case t1:
			m2palogto(sl, "-> T1 TIMEOUT <-");
			rtn = sl_t1_timeout(sl, q);
			break;
		case t2:
			m2palogto(sl, "-> T2 TIMEOUT <-");
			rtn = sl_t2_timeout(sl, q);
			break;
		case t3:
			m2palogto(sl, "-> T3 TIMEOUT <-");
			rtn = sl_t3_timeout(sl, q);
			break;
		case t4:
			m2palogto(sl, "-> T4 TIMEOUT <-");
			rtn = sl_t4_timeout(sl, q);
			break;
#if 0
		case t5:
			m2palogto(sl, "-> T5 TIMEOUT <-");
			rtn = sl_t5_timeout(sl, q);
			break;
#endif
		case t6:
			m2palogto(sl, "-> T6 TIMEOUT <-");
			rtn = sl_t6_timeout(sl, q);
			break;
		case t7:
			m2palogto(sl, "-> T7 TIMEOUT <-");
			rtn = sl_t7_timeout(sl, q);
			break;
#if 0
		case t8:
			m2palogto(sl, "-> T8 TIMEOUT <-");
			rtn = sl_t8_timeout(sl, q);
			break;
#endif
		case t9:
			m2palogto(sl, "-> T9 TIMEOUT <-");
			rtn = sl_t9_timeout(sl, q);
			break;
		default:
			m2paloger(sl, "sl_r_sig() called with invalid timer %d",
				  *(int *) mp->b_rptr);
			break;
		}
		if (rtn >= 0)
			rtn = QR_ABSORBED;
		else
			switch (rtn) {
			case -ENOBUFS:
			case -ENOMEM:
			case -EBUSY:
			case -EAGAIN:
				if (mi_timer_requeue(mp))
					break;
				/* fall through */
			default:
				rtn = QR_ABSORBED;
			}
	}
	return (rtn);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_DATA Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sl_w_data(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	int err;

	m2palogda(sl, "-> M_DATA [%d]", (int) msgdsize(mp));
	if ((err = sl_lsc_pdu(sl, q, mp)) < 0)
		return (err);
	return (QR_ABSORBED);	/* data absorbed */
}
STATIC int
sl_r_data(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	int err;

	m2palogda(sl, "M_DATA [%d] <-", (int) msgdsize(mp));
	if ((err = sl_recv_msg(sl, q, mp)) == QR_ABSORBED)
		return (QR_ABSORBED);	/* data absorbed */
	return (err);
}

/*
 *  =========================================================================
 *
 *  PUT and SRV
 *
 *  =========================================================================
 */
STATIC INLINE streamscall int
sl_r_prim(queue_t *q, mblk_t *mp)
{
	/* Fast Path */
	if (mp->b_datap->db_type == M_DATA)
		return sl_r_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return sl_r_data(q, mp);
	case M_SIG:
	case M_PCSIG:
		return sl_r_sig(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return sl_r_proto(q, mp);
	case M_FLUSH:
		return ss7_r_flush(q, mp);
	}
	return (QR_PASSALONG);
}
STATIC INLINE streamscall int
sl_w_prim(queue_t *q, mblk_t *mp)
{
	/* Fast Path */
	if (mp->b_datap->db_type == M_DATA)
		return sl_w_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return sl_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return sl_w_proto(q, mp);
	case M_FLUSH:
		return ss7_w_flush(q, mp);
	case M_IOCTL:
		return sl_w_ioctl(q, mp);
	}
	return (QR_PASSALONG);
}

/*
 *  =========================================================================
 *
 *  Private Structure allocation, deallocation and cache
 *
 *  =========================================================================
 */
kmem_cachep_t sl_priv_cachep = NULL;
STATIC int
sl_init_caches(void)
{
	if (!sl_priv_cachep
	    && !(sl_priv_cachep =
		 kmem_cache_create("sl_priv_cachep", sizeof(struct sl), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: Cannot allocate sl_priv_cachep", __FUNCTION__);
		return (-ENOMEM);
	} else
		printd(("%s: initialized module private structure cache\n", MOD_NAME));
	return (0);
}
STATIC int
sl_term_caches(void)
{
	if (sl_priv_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(sl_priv_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy sl_priv_cachep", __FUNCTION__);
			return (-EBUSY);
		} else
			printd(("%s: destroyed sl_priv_cachep\n", MOD_NAME));
#else
		kmem_cache_destroy(sl_priv_cachep);
#endif
	}
	return (0);
}
STATIC struct sl *
sl_alloc_priv(queue_t *q, struct sl **slp, dev_t *devp, cred_t *crp, int mid, int sid)
{
	struct sl *sl;

	if ((sl = kmem_cache_alloc(sl_priv_cachep, GFP_ATOMIC))) {
		bzero(sl, sizeof(*sl));

		sl->mid = mid;
		sl->sid = sid;

		m2palogno(sl, "allocated module private structure");

		/* intitialize head structure */
		sl->next = NULL;
		sl->prev = &sl->next;
		atomic_set(&sl->refcnt, 0);
		spin_lock_init(&sl->lock);	/* m2pa-priv-lock */
		sl->priv_put = &sl_put;
		sl->priv_get = &sl_get;
		sl->type = 0;
		sl->id = 0;
		sl->state = 0;
		sl->flags = 0;

		sl_get(sl);	/* first get */

		/* initialize stream structure */
		sl->u.dev.cmajor = getmajor(*devp);
		sl->u.dev.cminor = getminor(*devp);
		sl->cred = *crp;
		(sl->oq = RD(q))->q_ptr = sl_get(sl);
		(sl->iq = WR(q))->q_ptr = sl_get(sl);
		sl->i_prim = &sl_w_prim;
		sl->o_prim = &sl_r_prim;
		sl->i_wakeup = &sl_tx_wakeup;
		sl->o_wakeup = &sl_rx_wakeup;
		spin_lock_init(&sl->qlock);	/* m2pa-queue-lock */
		sl->users = 0;
		sl->ibid = 0;
		sl->obid = 0;
		sl->iwait = NULL;
		sl->owait = NULL;
		sl->i_state = LMI_UNUSABLE;
		sl->i_flags = 0;
		sl->i_style = LMI_STYLE2;
		sl->i_version = M2PA_VERSION_DEFAULT;
		sl->i_oldstate = sl->i_state;

		if (sl_alloc_timers(sl)) {
			sl_put(XCHG(&sl->iq->q_ptr, NULL));
			sl_put(XCHG(&sl->oq->q_ptr, NULL));
			sl_put(XCHG(&sl, NULL));
			return (sl);
		}

		/* link structure */
		if ((sl->next = *slp))
			sl->next->prev = &sl->next;
		sl->prev = slp;
		*slp = sl_get(sl);
		m2palogno(sl, "linked module private structure");

		/* intialize sl structure */
		sl->n_state = NS_UNBND;	/* until info ack */
		sl->n_request = 0;
		sl->coninds = 0;
		sl->rbuf = NULL;
		sl->rmsu = 0;
		sl->tmsu = 0;
		sl->rack = 0;
		sl->tack = 0;
		sl->fsnr = 0x00ffffff;	/* depends on draft version */
		sl->bsnr = 0x00ffffff;	/* depends on draft version */
		sl->fsnx = 0x00000000;	/* depends on draft version */
		sl->fsnt = 0x00000000;	/* depends on draft version */
		sl->back = 0;
		sl->bbsn = 0;
		sl->bfsn = 0;

		/* LMI configuraiton defaults */
		sl->option.pvar = SS7_PVAR_ITUT_88;
		sl->option.popt = 0;

		/* buffer queues */
		bufq_init(&sl->rb);
		bufq_init(&sl->tb);
		bufq_init(&sl->rtb);

		/* SDL configuration defaults */
		sl->sdl.config.ifflags = 0;
		sl->sdl.config.iftype = SDL_TYPE_PACKET;
		sl->sdl.config.ifrate = 10000000;
		sl->sdl.config.ifgtype = SDL_GTYPE_SCTP;
		sl->sdl.config.ifgrate = 10000000;
		sl->sdl.config.ifmode = SDL_MODE_PEER;
		sl->sdl.config.ifgmode = SDL_GMODE_NONE;
		sl->sdl.config.ifgcrc = SDL_GCRC_NONE;
		sl->sdl.config.ifclock = SDL_CLOCK_NONE;
		sl->sdl.config.ifcoding = SDL_CODING_NONE;
		sl->sdl.config.ifframing = SDL_FRAMING_NONE;
		/* rest zero */

		/* SDT configuration defaults */
		sl->sdt.config.Tin = 4;
		sl->sdt.config.Tie = 1;
		sl->sdt.config.T = 64;
		sl->sdt.config.D = 256;
		sl->sdt.config.t8 = 100;
		sl->sdt.config.Te = 577169;
		sl->sdt.config.De = 9308000;
		sl->sdt.config.Ue = 144292000;
		sl->sdt.config.N = 16;
		sl->sdt.config.m = 272;
		sl->sdt.config.b = 8;
		sl->sdt.config.f = SDT_FLAGS_ONE;

		/* SL configuration defaults */
		sl->sl.config.t1 = 45 * 1000;
		sl->sl.config.t2 = 5 * 1000;
		sl->sl.config.t2l = 20 * 1000;
		sl->sl.config.t2h = 100 & 1000;
		sl->sl.config.t3 = 1 * 1000;
		sl->sl.config.t4n = 8 * 1000;
		sl->sl.config.t4e = 500;
		sl->sl.config.t5 = 100;
		sl->sl.config.t6 = 4 * 1000;
		sl->sl.config.t7 = 1 * 1000;
		sl->sl.config.rb_abate = 3;
		sl->sl.config.rb_accept = 6;
		sl->sl.config.rb_discard = 9;
		sl->sl.config.tb_abate_1 = 128 * 272;
		sl->sl.config.tb_onset_1 = 256 * 272;
		sl->sl.config.tb_discd_1 = 284 * 272;
		sl->sl.config.tb_abate_2 = 512 * 272;
		sl->sl.config.tb_onset_2 = 640 * 272;
		sl->sl.config.tb_discd_2 = 768 * 272;
		sl->sl.config.tb_abate_3 = 896 * 272;
		sl->sl.config.tb_onset_3 = 1024 * 272;
		sl->sl.config.tb_discd_3 = 1152 * 272;
		sl->sl.config.N1 = 127;
		sl->sl.config.N2 = 8192;
		sl->sl.config.M = 5;

		/* LM information defaults */
		sl->info.lm.lmi_primitive = LMI_INFO_ACK;
		sl->info.lm.lmi_version = LMI_CURRENT_VERSION;
		sl->info.lm.lmi_state = LMI_UNUSABLE;
		sl->info.lm.lmi_max_sdu = 538;
		sl->info.lm.lmi_min_sdu = 1;
		sl->info.lm.lmi_header_len = 10;
		sl->info.lm.lmi_ppa_style = LMI_STYLE2;
		sl->info.lm.lmi_ppa_length = 0;
		sl->info.lm.lmi_ppa_offset = sizeof(sl->info.lm);
		sl->info.lm.lmi_prov_flags = 0;
		sl->info.lm.lmi_prov_state = MS_POWER_OFF;

		m2palogno(sl, "setting module private structure defaults");
	} else
		ptrace(("%s: %p: ERROR: Could not allocate module private structure\n",
			MOD_NAME, sl));
	return (sl);
}
STATIC void
sl_free_priv(queue_t *q)
{
	struct sl *sl = SL_PRIV(q);

	ensure(sl, return);
	{
		ss7_unbufcall((str_t *) sl);
		sl_free_timers(sl);
		bufq_purge(&sl->rb);
		bufq_purge(&sl->tb);
		bufq_purge(&sl->rtb);
		if (sl->rbuf)
			freemsg(xchg(&sl->rbuf, NULL));
		if ((*sl->prev = sl->next))
			sl->next->prev = sl->prev;
		sl->next = NULL;
		sl->prev = &sl->next;
		ensure(atomic_read(&sl->refcnt) > 1, sl_get(sl));
		sl_put(sl);
		flushq(sl->oq, FLUSHDATA);
		flushq(sl->iq, FLUSHDATA);
		sl->oq->q_ptr = NULL;
		sl->oq = NULL;
		ensure(atomic_read(&sl->refcnt) > 1, sl_get(sl));
		sl_put(sl);
		sl->iq->q_ptr = NULL;
		sl->iq = NULL;
		ensure(atomic_read(&sl->refcnt) > 1, sl_get(sl));
		sl_put(sl);
		/* done, check final count */
		if (atomic_read(&sl->refcnt) != 1) {
			m2paloger(sl, "sl lingering reference count = %d",
				  (int) atomic_read(&sl->refcnt));
			atomic_set(&sl->refcnt, 1);
		}
	}
	sl_put(sl);		/* final put */
	return;
}
STATIC struct sl *
sl_get(struct sl *sl)
{
	assure(sl);
	if (sl)
		atomic_inc(&sl->refcnt);
	return (sl);
}
STATIC void
sl_put(struct sl *sl)
{
	assure(sl);
	if (sl && atomic_dec_and_test(&sl->refcnt)) {
		kmem_cache_free(sl_priv_cachep, sl);
		m2palogno(sl, "freed module private structure");
	}
}

/*
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 */
unsigned short modid = MOD_ID;
STATIC struct sl *sl_list = NULL;
STATIC spinlock_t sl_lock = SPIN_LOCK_UNLOCKED;
STATIC int m2pa_instance = 0;
STATIC streamscall int
sl_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	(void) crp;		/* for now */
	if (q->q_ptr != NULL)
		return (0);	/* already open */
	if (sflag == MODOPEN || WR(q)->q_next != NULL) {
		major_t cmajor = getmajor(*devp);
		minor_t cminor = getminor(*devp);
		struct sl *sl;
		mblk_t *mp;
		unsigned long flags;

		if ((mp = allocb(sizeof(N_info_req_t), BPRI_WAITOK)) == NULL)
			return (ENOSR);

		spin_lock_irqsave(&sl_lock, flags);
		/* test for multiple push */
		for (sl = sl_list; sl; sl = sl->next)
			if (sl->u.dev.cmajor == cmajor && sl->u.dev.cminor == cminor) {
				spin_unlock_irqrestore(&sl_lock, flags);
				freemsg(mp);
				return (ENXIO);
			}

		if (!(sl_alloc_priv(q, &sl_list, devp, crp, modid, m2pa_instance + 1))) {
			spin_unlock_irqrestore(&sl_lock, flags);
			freemsg(mp);
			return (ENOMEM);
		}
		m2pa_instance++;
		spin_unlock_irqrestore(&sl_lock, flags);

		mp->b_datap->db_type = M_PCPROTO;
		((N_info_req_t *) mp->b_wptr)->PRIM_type = N_INFO_REQ;
		mp->b_wptr += sizeof(N_info_req_t);

		qprocson(q);
		putnext(WR(q), mp);	/* immediate info request */
		return (0);
	}
	return (EIO);
}
STATIC streamscall int
sl_close(queue_t *q, int flag, cred_t *crp)
{
	unsigned long flags;

	(void) flag;
	(void) crp;

	qprocsoff(q);

	spin_lock_irqsave(&sl_lock, flags);
	sl_free_priv(q);
	spin_unlock_irqrestore(&sl_lock, flags);
	return (0);
}

/*
 *  =========================================================================
 *
 *  Registration and initialization
 *
 *  =========================================================================
 */
#ifdef LINUX
/*
 *  Linux Registration
 *  -------------------------------------------------------------------------
 */

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0444);
#endif
MODULE_PARM_DESC(modid, "Module ID for the M2PA-SL module. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LIS
#define fmodsw _fmodsw
#endif

STATIC struct fmodsw sl_fmod = {
	.f_name = MOD_NAME,
	.f_str = &m2pa_slinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

MODULE_STATIC int __init
m2pa_slinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_SPLASH);	/* console splash */
	if ((err = sl_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", MOD_NAME, err);
		return (err);
	}
	if ((err = register_strmod(&sl_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module, err = %d", MOD_NAME, -err);
		sl_term_caches();
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

MODULE_STATIC void __exit
m2pa_slterminate(void)
{
	int err;

	if ((err = unregister_strmod(&sl_fmod)) < 0)
		cmn_err(CE_WARN, "%s: could not unregister module", MOD_NAME);
	if ((err = sl_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", MOD_NAME);
	return;
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(m2pa_slinit);
module_exit(m2pa_slterminate);

#endif				/* LINUX */
