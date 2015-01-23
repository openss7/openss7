/*****************************************************************************

 @(#) File: src/drviers/mtp_mux.c

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

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

static char const ident[] = "File: " __FILE__ "  Version: " PACKAGE_ENVR "  Date: " PACKAGE_DATE;

#define _SVR4_SOURCE	1
#define _MPS_SOURCE	1

#include <sys/os7/compat.h>

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/mtpi.h>
#include <ss7/mtpi_ioctl.h>

#include <ss7/mtp_mux.h>

#define MTP_MUX_DESCRIP		"MTP-MUX SS7/MTP (Message Transfer Part) STREAMS Multiplexing Driver"
#define MTP_MUX_EXTRA		"Part of the OpenSS7 SS7 Stack for Linux Fast-STREAMS"
#define MTP_MUX_REVISION	"OpenSS7 File: " __FILE__ "  Version: " PACKAGE_ENVR "  Date: " PACKAGE_DATE
#define MTP_MUX_COPYRIGHT	"Copyright (c) 2008-2015  Monavacon Limited.  All Rights Reserved."
#define MTP_MUX_DEVICE		"Supports the OpenSS7 MTP and M3UA drivers."
#define MTP_MUX_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define MTP_MUX_LICENSE		"GPL"
#define MTP_MUX_BANNER		MTP_MUX_DESCRIP		"\n" \
				MTP_MUX_EXTRA		"\n" \
				MTP_MUX_REVISION	"\n" \
				MTP_MUX_COPYRIGHT	"\n" \
				MTP_MUX_DEVICE		"\n" \
				MTP_MUX_CONTACT		"\n"
#define MTP_MUX_SPLASH		MTP_MUX_DEVICE		" - " \
				MTP_MUX_REVISION

#ifdef LINUX
MODULE_AUTHOR(MTP_MUX_CONTACT);
MODULE_DESCRIPTION(MTP_MUX_DESCRIP);
MODULE_SUPPORTED_DEVICE(MTP_MUX_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(MTP_MUX_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-mtp-mux");
#endif				/* MODULE_ALIAS */
#ifdef MODULE_VERSION
MODULE_VERSION(PACKAGE_ENVR);
#endif				/* MODULE_VERSION */
#endif				/* LINUX */

#define MTP_MUX_DRV_ID		CONFIG_STREAMS_MTP_MUX_MODID
#define MTP_MUX_DRV_NAME	CONFIG_STREAMS_MTP_MUX_NAME
#define MTP_MUX_CMAJORS		CONFIG_STREAMS_MTP_MUX_NMAJORS
#define MTP_MUX_CMAJOR_0	CONFIG_STREAMS_MTP_MUX_MAJOR
#define MTP_MUX_UNITS		CONFIG_STREAMS_MTP_MUX_NMINORES

/*
 * LOGGING
 * --------------------------------------------------------------------------
 */

#define STRLOGNO	0
#define STRLOGIO	1
#define STRLOGST	2
#define STRLOGTO	3
#define STRLOGRX	4
#define STRLOGTX	5
#define STRLOGTE	6
#define STRLOGDA	7

static inline fastcall const char *
lmi_primname(const mtp_ulong prim)
{
	switch (prim) {
	case LMI_INFO_REQ:
		return ("LMI_INFO_REQ");
	case LMI_ATTACH_REQ:
		return ("LMI_ATTACH_REQ");
	case LMI_DETACH_REQ:
		return ("LMI_DETACH_REQ");
	case LMI_ENABLE_REQ:
		return ("LMI_ENABLE_REQ");
	case LMI_DISABLE_REQ:
		return ("LMI_DISABLE_REQ");
	case LMI_OPTMGMT_REQ:
		return ("LMI_OPTMGMT_REQ");
	case LMI_INFO_ACK:
		return ("LMI_INFO_ACK");
	case LMI_OK_ACK:
		return ("LMI_OK_ACK");
	case LMI_ERROR_ACK:
		return ("LMI_ERROR_ACK");
	case LMI_ENABLE_CON:
		return ("LMI_ENABLE_CON");
	case LMI_DISABLE_CON:
		return ("LMI_DISABLE_CON");
	case LMI_OPTMGMT_ACK:
		return ("LMI_OPTMGMT_ACK");
	case LMI_ERROR_IND:
		return ("LMI_ERROR_IND");
	case LMI_STATS_IND:
		return ("LMI_STATS_IND");
	case LMI_EVENT_IND:
		return ("LMI_EVENT_IND");
	default:
		return ("(unknown)");
	}
}

static inline fastcall const char *
mtp_primname(const mtp_ulong prim)
{
	switch (prim) {
	case MTP_BIND_REQ:
		return ("MTP_BIND_REQ");
	case MTP_UNBIND_REQ:
		return ("MTP_UNBIND_REQ");
	case MTP_CONN_REQ:
		return ("MTP_CONN_REQ");
	case MTP_DISCON_REQ:
		return ("MTP_DISCON_REQ");
	case MTP_ADDR_REQ:
		return ("MTP_ADDR_REQ");
	case MTP_INFO_REQ:
		return ("MTP_INFO_REQ");
	case MTP_OPTMGMT_REQ:
		return ("MTP_OPTMGMT_REQ");
	case MTP_TRANSFER_REQ:
		return ("MTP_TRANSFER_REQ");
	case MTP_OK_ACK:
		return ("MTP_OK_ACK");
	case MTP_ERROR_ACK:
		return ("MTP_ERROR_ACK");
	case MTP_BIND_ACK:
		return ("MTP_BIND_ACK");
	case MTP_ADDR_ACK:
		return ("MTP_ADDR_ACK");
	case MTP_INFO_ACK:
		return ("MTP_INFO_ACK");
	case MTP_OPTMGMT_ACK:
		return ("MTP_OPTMGMT_ACK");
	case MTP_TRANSFER_IND:
		return ("MTP_TRANSFER_IND");
	case MTP_PAUSE_IND:
		return ("MTP_PAUSE_IND");
	case MTP_RESUME_IND:
		return ("MTP_RESUME_IND");
	case MTP_STATUS_IND:
		return ("MTP_STATUS_IND");
	case MTP_RESTART_BEGINS_IND:
		return ("MTP_RESTART_BEGINS_IND");
	case MTP_RESTART_COMPLETE_IND:
		return ("MTP_RESTART_COMPLETE_IND");
	default:
		return ("(unknown)");
	}
}

static inline fastcall const char *
lmi_statename(const mtp_ulong state)
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

static inline fastcall const char *
mtp_statename(const mtp_ulong state)
{
	switch (state) {
	case MTPS_UNBND:
		return ("MTPS_UNBND");
	case MTPS_WACK_BREQ:
		return ("MTPS_WACK_BREQ");
	case MTPS_IDLE:
		return ("MTPS_IDLE");
	case MTPS_WACK_CREQ:
		return ("MTPS_WACK_CREQ");
	case MTPS_WCON_CREQ:
		return ("MTPS_WCON_CREQ");
	case MTPS_CONNECTED:
		return ("MTPS_CONNECTED");
	case MTPS_WACK_UREQ:
		return ("MTPS_WACK_UREQ");
	case MTPS_WACK_DREQ6:
		return ("MTPS_WACK_DREQ6");
	case MTPS_WACK_DREQ9:
		return ("MTPS_WACK_DREQ9");
	case MTPS_WACK_OPTREQ:
		return ("MTPS_WACK_OPTREQ");
	case MTPS_WREQ_ORDREL:
		return ("MTPS_WREQ_ORDREL");
	case MTPS_WIND_ORDREL:
		return ("MTPS_WIND_ORDREL");
	case MTPS_WRES_CIND:
		return ("MTPS_WRES_CIND");
	case MTPS_UNUSABLE:
		return ("MTPS_UNUSABLE");
	default:
		return ("(unknown)");
	}
}

static inline fastcall const char *
lmi_errname(const mtp_long error)
{
	switch (error) {
	case LMI_UNSPEC:
		return ("LMI_UNSPEC");
	case LMI_BADADDRESS:
		return ("LMI_BADADDRESS");
	case LMI_BADADDRTYPE:
		return ("LMI_BADADDRTYPE");
	case LMI_BADDIAL:
		return ("LMI_BADDIAL");
	case LMI_BADDIALTYPE:
		return ("LMI_BADDIALTYPE");
	case LMI_BADDISPOSAL:
		return ("LMI_BADDISPOSAL");
	case LMI_BADFRAME:
		return ("LMI_BADFRAME");
	case LMI_BADPPA:
		return ("LMI_BADPPA");
	case LMI_BADPRIM:
		return ("LMI_BADPRIM");
	case LMI_DISC:
		return ("LMI_DISC");
	case LMI_EVENT:
		return ("LMI_EVENT");
	case LMI_FATALERR:
		return ("LMI_FATALERR");
	case LMI_INITFAILED:
		return ("LMI_INITFAILED");
	case LMI_NOTSUPP:
		return ("LMI_NOTSUPP");
	case LMI_OUTSTATE:
		return ("LMI_OUTSTATE");
	case LMI_PROTOSHORT:
		return ("LMI_PROTOSHORT");
	case LMI_SYSERR:
		return ("LMI_SYSERR");
	case LMI_WRITEFAIL:
		return ("LMI_WRITEFAIL");
	case LMI_CRCERR:
		return ("LMI_CRCERR");
	case LMI_DLE_EOT:
		return ("LMI_DLE_EOT");
	case LMI_FORMAT:
		return ("LMI_FORMAT");
	case LMI_HDLC_ABORT:
		return ("LMI_HDLC_ABORT");
	case LMI_OVERRUN:
		return ("LMI_OVERRUN");
	case LMI_TOOSHORT:
		return ("LMI_TOOSHORT");
	case LMI_INCOMPLETE:
		return ("LMI_INCOMPLETE");
	case LMI_BUSY:
		return ("LMI_BUSY");
	case LMI_NOANSWER:
		return ("LMI_NOANSWER");
	case LMI_CALLREJECT:
		return ("LMI_CALLREJECT");
	case LMI_HDLC_IDLE:
		return ("LMI_HDLC_IDLE");
	case LMI_HDLC_NOTIDLE:
		return ("LMI_HDLC_NOTIDLE");
	case LMI_QUIESCENT:
		return ("LMI_QUIESCENT");
	case LMI_RESUMED:
		return ("LMI_RESUMED");
	case LMI_DSRTIMEOUT:
		return ("LMI_DSRTIMEOUT");
	case LMI_LAN_COLLISIONS:
		return ("LMI_LAN_COLLISIONS");
	case LMI_LAN_REFUSED:
		return ("LMI_LAN_REFUSED");
	case LMI_LAN_NOSTATION:
		return ("LMI_LAN_NOSTATION");
	case LMI_LOSTCTS:
		return ("LMI_LOSTCTS");
	case LMI_DEVERR:
		return ("LMI_DEVERR");
	default:
		if (error < 0)
			return ("MTPSYSERR");
		return ("(unknown)");
	}
}

static inline fastcall const char *
mtp_errname(const mtp_long error)
{
	switch (error) {
	case MSYSERR:
		return ("MSYSERR");
	case MACCESS:
		return ("MACCESS");
	case MBADADDR:
		return ("MBADADDR");
	case MNOADDR:
		return ("MNOADDR");
	case MBADPRIM:
		return ("MBADPRIM");
	case MOUTSTATE:
		return ("MOUTSTATE");
	case MNOTSUPP:
		return ("MNOTSUPP");
	case MBADFLAG:
		return ("MBADFLAG");
	case MBADOPT:
		return ("MBADOPT");
	default:
		if (error < 0)
			return ("MTPSYSERR");
		return ("(unknown)");
	}
}

static inline fastcall const char *
lmi_strerror(const mtp_long error)
{
	switch (error) {
	case LMI_UNSPEC:
		return ("Unknown or unspecified");
	case LMI_BADADDRESS:
		return ("Address was invalid");
	case LMI_BADADDRTYPE:
		return ("Invalid address type");
	case LMI_BADDIAL:
		return ("(not used)");
	case LMI_BADDIALTYPE:
		return ("(not used)");
	case LMI_BADDISPOSAL:
		return ("Invalid disposal parameter");
	case LMI_BADFRAME:
		return ("Defective SDU received");
	case LMI_BADPPA:
		return ("Invalid PPA identifier");
	case LMI_BADPRIM:
		return ("Unregognized primitive");
	case LMI_DISC:
		return ("Disconnected");
	case LMI_EVENT:
		return ("Protocol-specific event ocurred");
	case LMI_FATALERR:
		return ("Device has become unusable");
	case LMI_INITFAILED:
		return ("Link initialization failed");
	case LMI_NOTSUPP:
		return ("Primitive not supported by this device");
	case LMI_OUTSTATE:
		return ("Primitive was issued from invalid state");
	case LMI_PROTOSHORT:
		return ("M_PROTO block too short");
	case LMI_SYSERR:
		return ("UNIX system error");
	case LMI_WRITEFAIL:
		return ("Unitdata request failed");
	case LMI_CRCERR:
		return ("CRC or FCS error");
	case LMI_DLE_EOT:
		return ("DLE EOT detected");
	case LMI_FORMAT:
		return ("Format error detected");
	case LMI_HDLC_ABORT:
		return ("Aborted frame detected");
	case LMI_OVERRUN:
		return ("Input overrun");
	case LMI_TOOSHORT:
		return ("Frame too short");
	case LMI_INCOMPLETE:
		return ("Partial frame received");
	case LMI_BUSY:
		return ("Telephone was busy");
	case LMI_NOANSWER:
		return ("Connection went unanswered");
	case LMI_CALLREJECT:
		return ("Connection rejected");
	case LMI_HDLC_IDLE:
		return ("HDLC line went idle");
	case LMI_HDLC_NOTIDLE:
		return ("HDLC link no longer idle");
	case LMI_QUIESCENT:
		return ("Line being reassigned");
	case LMI_RESUMED:
		return ("Line has been reassigned");
	case LMI_DSRTIMEOUT:
		return ("Did not see DSR in time");
	case LMI_LAN_COLLISIONS:
		return ("LAN excessive collisions");
	case LMI_LAN_REFUSED:
		return ("LAN message refused");
	case LMI_LAN_NOSTATION:
		return ("LAN no such station");
	case LMI_LOSTCTS:
		return ("Lost Clear to Send signal");
	case LMI_DEVERR:
		return ("Start of device-specific error codes");
	default:
		if (error < 0)
			return ("UNIX system error");
		return ("unknown error");
	}
}

static inline fastcall const char *
mtp_strerror(const mtp_long error)
{
	switch (error) {
	case MSYSERR:
		return ("UNIX system error");
	case MACCESS:
		return ("Access denied");
	case MBADADDR:
		return ("Bad address");
	case MNOADDR:
		return ("No address");
	case MBADPRIM:
		return ("Bad primitive");
	case MOUTSTATE:
		return ("Out of state");
	case MNOTSUPP:
		return ("Not supported");
	case MBADFLAG:
		return ("Bad flag");
	case MBADOPT:
		return ("Bad option");
	default:
		if (error < 0)
			return ("UNIX system error");
		return ("unknown error");
	}
}

/*
 * PRIVATE STRUCTURE
 * --------------------------------------------------------------------------
 */

struct mtp {
	queue_t *oq;
	struct mtp *other;
	struct mtp *monitor;
	struct mtp *admin;
	cred_t cred;
	struct mtpmux_ppa ppa;
};

#define MTP_PRIV(q) ((struct mtp *)q->q_ptr)

static struct mtp *mtp_ctrl = NULL; /* layer management control stream */

static caddr_t mtp_opens = NULL;
static caddr_t mtp_links = NULL;

static void
mtpu_init_priv(int unit, queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *credp)
{
}
static void
mtpu_term_priv(queue_t *q)
{
}
static void
mtpp_init_priv(int unit, queue_t *q, int index, cred_t *credp)
{
}
static void
mtpp_term_priv(queue_t *q)
{
}

/*
 * MTP MONITORING
 * --------------------------------------------------------------------------
 */

/**
 * mtp_monitor: - monitor MTP providers
 * @mtp: private structure (locked)
 * @q: active queue
 * @msg: the message to monitor
 * @mtpmon: private structure for upper monitoring stream
 */
noinline fastcall int
mtp_monitor(struct mtp *mtp, queue_t *q, mblk_t *msg, struct mtp *mtpmon)
{
	struct mtpmux_mon *mon;
	mblk_t *mp = NULL;
	int err = 0;

	read_lock(&mtp_lock);
	if ((msg->b_flag & 0x8000) != 0)
		goto error;
	if (!(mp = mi_allocb(q, sizeof(*mon), BPRI_MED)))
		goto enobufs;
	DB_TYPE(mp) = DB_TYPE(msg);
	mon = (typeof(mon)) mp->b_rptr;
	mon->mon_ppa.mtpm_index = mtp->other->index;
	mon->mon_ppa.mtpm_ppa = mtp->ppa;
	strncpy(mon->mon_ppa.mtpm_clei, mtp->clei, MTPMUX_CLEI_MAX);
	mon->mon_dir = (q->q_flag & QREADR) ? 0 : 1;
	mon->mon_msg_type = DB_TYPE(msg);
	mon->mon_msg_band = msg->b_band;
	mon->mon_msg_flags = msg->b_flag;
	mp->b_wptr += sizeof(*mon);
	if ((mp->b_cont = dupmsg(msg)) == NULL || !pullupmsg(mp, -1))
		goto enobufs;
	if (!canputnext(mtpmon->oq))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	putnext(mtpmon->oq, mp);
	msg->b_flag |= 0x8000;	/* mark original as copied */
	mp = NULL;
      error:
	if (mp)
		freeb(mp);
	read_unlock(&mtp_lock);
	return (err);
      enobufs:
	err = -ENOBUFS;
	goto error;
      ebusy:
	err = -EBUSY;
	goto error;
}

/**
 * mtp_rxprim: - receive primitive
 * @mtp: private structure (locked)
 * @mp: the primitive
 * @prim: the primitive type
 */
static inline fastcall int
mtp_rxprim(struct mtp *mtp, queue_t *q, mblk_t *mp, const mtp_ulong prim)
{
	const char *fmt;
	int err = 0, level;

	if (mtp->monitor && (err = mtp_monitor(mtp, q, mp, mtp->monitor)))
		goto error;
	switch (__builtin_expect(prim, MTP_TRANSFER_REQ)) {
	case MTP_TRANSFER_REQ:
	case MTP_TRANSFER_IND:
		level = STRLOGDA;
		break;
	default:
		level = STRLOGRX;
		break;
	}
	fmt = (prim < MTP_OK_ACK) ? "-> %s" : "%s <-";
	mi_strlog(q, level, SL_TRACE, fmt, mtp_primname(prim));
      error:
	return (err);
}

/**
 * mtp_txprim: - transmit primitive
 * @mtp: private structure (locked)
 * @msg: message to free upon success (or NULL)
 * @mp: the primitive
 * @prim: the primitive type
 */
static inline fastcall int
mtp_txprim(struct mtp *mtp, queue_t *q, mblk_t *msg, mblk_t *mp, mtp_ulong prim)
{
	const char *fmt;
	int err = 0, level;
	mblk_t *b, *b_next;

	if (unlikely(mtp->monitor && (err = mtp_monitor(mtp, q, mp, mtp->monitor))))
		goto error;
	if (unlikely(!bcanputnext(mtp->oq, mp->b_band)))
		goto ebusy;
	switch (__builtin_expect(prim, MTP_TRANSFER_REQ)) {
	case MTP_TRANSFER_REQ:
	case MTP_TRANSFER_IND:
		level = STRLOGDA;
		break;
	default:
		level = STRLOGTX;
		break;
	}
	fmt = (prim < MTP_OK_ACK) ? "%s ->" : "<- %s";
	mi_strlog(q, level, SL_TRACE, fmt, mtp_primname(prim));
	for (b = b_next = msg; b && b != mp->b_cont; b = b_next) {
		b_next = b->b_next;
		freeb(b);
	}
	putnext(mtp->oq, mp);
      error:
	return (err);
      ebusy:
	err = -EBUSY;
	goto error;
}

/*
 * MTP USER TO MTP PROVIDER ISSUED PRIMITIVES
 * --------------------------------------------------------------------------
 */

/**
 * lmi_info_req: - issue LMI_INFO_REQ primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
noinline static int
lmi_info_req(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
}

/**
 * lmi_attach_req: - issue LMI_ATTACH_REQ primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
noinline static int
lmi_attach_req(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
}

/**
 * lmi_detach_req: - issue LMI_DETACH_REQ primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
noinline static int
lmi_detach_req(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
}

/**
 * lmi_enable_req: - issue LMI_ENABLE_REQ primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
noinline static int
lmi_enable_req(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
}

/**
 * lmi_disable_req: - issue LMI_DISABLE_REQ primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
noinline static int
lmi_disable_req(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
}

/**
 * lmi_optmgmt_req: - issue LMI_OPTMGMT_REQ primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
noinline static int
lmi_optmgmt_req(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
}

/**
 * mtp_bind_req: - issue MTP_BIND_REQ primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
noinline static int
mtp_bind_req(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
}

/**
 * mtp_unbind_req: - issue MTP_UNBIND_REQ primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
noinline static int
mtp_unbind_req(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
}

/**
 * mtp_conn_req: - issue MTP_CONN_REQ primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
noinline static int
mtp_conn_req(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
}

/**
 * mtp_discon_req: - issue MTP_DISCON_REQ primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
noinline static int
mtp_discon_req(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
}

/**
 * mtp_addr_req: - issue MTP_ADDR_REQ primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
noinline static int
mtp_addr_req(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
}

/**
 * mtp_info_req: - issue MTP_INFO_REQ primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
noinline static int
mtp_info_req(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
}

/**
 * mtp_optmgmt_req: - issue MTP_OPTMGMT_REQ primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
noinline static int
mtp_optmgmt_req(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
}

#ifdef MTP_STATUS_REQ
/**
 * mtp_status_req: - issue MTP_STATUS_REQ primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 * @add: address of status (or null)
 * @type: status type
 * @status: status
 */
noinline static int
mtp_status_req(struct mtp *mtp, queue_t *q, mblk_t *msg, struct strbuf *add, mtp_ulong type,
	       mtp_ulong status)
{
	struct MTP_status_req *p;
	mblk_t *mp = NULL;
	int err;

	const size_t alen = add ? add->len : 0;
	const uchar *aptr = add ? add->buf : NULL;
	const size_t mlen = sizeof(*p) + alen;

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(mtp->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->mtp_primitive = MTP_STATUS_REQ;
	p->mtp_addr_length = alen;
	p->mtp_addr_offset = alen ? sizeof(*p) : 0;
	p->mtp_type = type;
	p->mtp_status = status;
	mp->b_wptr += sizeof(*p);
	bcopy(aptr, mp->b_wptr, alen);
	mp->b_wptr += alen;
	mtp_txprim(mtp, q, msg, mp, MTP_STATUS_REQ);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	if (mp)
		freeb(mp);
	return (err);
}
#endif

/**
 * mtp_transfer_req: - issue MTP_TRANSFER_REQ primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 * @dst: destination address (or NULL)
 * @mp: message priority
 * @sls: signalling link selection
 * @dp: user data
 */
noinline static int
mtp_transfer_req(struct mtp *mtp, queue_t *q, mblk_t *msg, struct strbuf *dst, mtp_ulong mp,
		 mtp_ulong sls, mblk_t *dp)
{
	struct MTP_transfer_req *p;
	mblk_t *mp = NULL;
	int err;

	const size_t dlen = dst ? dst->len : 0;
	const uchar *dptr = dst ? dst->buf : NULL;
	const size_t mlen = sizeof(*p) + dlen;

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(mtp->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->mtp_primitive = MTP_TRANSFER_REQ;
	p->mtp_dest_length = dlen;
	p->mtp_dest_offset = dlen ? sizeof(*p) : 0;
	p->mtp_mp = mp;
	p->mtp_sls = sls;
	mp->b_wptr += sizeof(*p);
	bcopy(dptr, mp->b_wptr, dlen);
	mp->b_wptr += dlen;
	mp->b_cont = dp;
	mtp_txprim(mtp, q, msg, mp, MTP_TRANSFER_REQ);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	if (mp)
		freeb(mp);
	return (err);

}

/*
 * MTP PROVIDER TO MTP USER ISSUED PRIMITIVES
 * --------------------------------------------------------------------------
 */

/**
 * lmi_info_ack: - issue LMI_INFO_ACK primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
noinline static int
lmi_info_ack(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
}

/**
 * lmi_ok_ack: - issue LMI_OK_ACK primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
noinline static int
lmi_ok_ack(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
}

/**
 * lmi_error_ack: - issue LMI_ERROR_ACK primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
noinline static int
lmi_error_ack(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
}

/**
 * lmi_enable_con: - issue LMI_ENABLE_CON primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
noinline static int
lmi_enable_con(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
}

/**
 * lmi_disable_con: - issue LMI_DISABLE_CON primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
noinline static int
lmi_disable_con(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
}

/**
 * lmi_optmgmt_ack: - issue LMI_OPTMGMT_ACK primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
noinline static int
lmi_optmgmt_ack(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
}

/**
 * lmi_error_ind: - issue LMI_ERROR_IND primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
noinline static int
lmi_error_ind(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
}

/**
 * lmi_stats_ind: - issue LMI_STATS_IND primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
noinline static int
lmi_stats_ind(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
}

/**
 * lmi_event_ind: - issue LMI_EVENT_IND primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
noinline static int
lmi_event_ind(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
}

/**
 * mtp_ok_ack: - issue MTP_OK_ACK primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
noinline static int
mtp_ok_ack(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
}

/**
 * mtp_error_ack: - issue MTP_ERROR_ACK primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
noinline static int
mtp_error_ack(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
}

/**
 * mtp_bind_ack: - issue MTP_BIND_ACK primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
noinline static int
mtp_bind_ack(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
}

/**
 * mtp_addr_ack: - issue MTP_ADDR_ACK primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
noinline static int
mtp_addr_ack(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
}

/**
 * mtp_info_ack: - issue MTP_INFO_ACK primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
noinline static int
mtp_info_ack(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
}

/**
 * mtp_optmgmt_ack: - issue MTP_OPTMGMT_ACK primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
noinline static int
mtp_optmgmt_ack(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
}

/**
 * mtp_transfer_ind: - issue MTP_TRANSFER_IND primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
noinline static int
mtp_transfer_ind(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
}

/**
 * mtp_pause_ind: - issue MTP_PAUSE_IND primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
noinline static int
mtp_pause_ind(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
}

/**
 * mtp_resume_ind: - issue MTP_RESUME_IND primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
noinline static int
mtp_resume_ind(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
}

/**
 * mtp_status_ind: - issue MTP_STATUS_IND primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
noinline static int
mtp_status_ind(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
}

/**
 * mtp_restart_begins_ind: - issue MTP_RESTART_BEGINS_IND primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
noinline static int
mtp_restart_begins_ind(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
}

/**
 * mtp_restart_complete_ind: - issue MTP_RESTART_COMPLETE_IND primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
noinline static int
mtp_restart_complete_ind(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
}

/*
 * MTP USER TO MTP PROVIDER PRIMITIVES
 * --------------------------------------------------------------------------
 */

/**
 * l_info_req: - process LMI_INFO_REQ primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @mp: the LMI_INFO_REQ primitive
 */
noinline fastcall int
l_info_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	lmi_info_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * l_attach_req: - process LMI_ATTACH_REQ primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @mp: the LMI_ATTACH_REQ primitive
 *
 * The LMI attach request primitive can be used to attach the upper Stream to
 * a Style II lower device Stream.  The PPA specifies which device to
 * which to attach to a lower stream.  That is, the lower stream selected must
 * be in the unbound state and must have a major device number equal to the
 * specified PPA.  That means that the user can request that the upper Stream
 * be attached to the MTP or M3UA drivers.
 */
noinline fastcall int
l_attach_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	lmi_attach_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * l_detach_req: - process LMI_DETACH_REQ primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @mp: the LMI_DETACH_REQ primitive
 */
noinline fastcall int
l_detach_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	lmi_detach_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * l_enable_req: - process LMI_ENABLE_REQ primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @mp: the LMI_ENABLE_REQ primitive
 */
noinline fastcall int
l_enable_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	lmi_enable_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * l_disable_req: - process LMI_DISABLE_REQ primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @mp: the LMI_DISABLE_REQ primitive
 */
noinline fastcall int
l_disable_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	lmi_disable_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * l_optmgmt_req: - process LMI_OPTMGMT_REQ primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @mp: the LMI_OPTMGMT_REQ primitive
 */
noinline fastcall int
l_optmgmt_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	lmi_optmgmt_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * m_bind_req: - process MTP_BIND_REQ primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @mp: the MTP_BIND_REQ primitive
 *
 * When in the unbound state, the upper Stream is not bound to a lower Stream.
 */
noinline fastcall int
m_bind_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct MTP_bind_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * m_unbind_req: - process MTP_UNBIND_REQ primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @mp: the MTP_UNBIND_REQ primitive
 */
noinline fastcall int
m_unbind_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct MTP_unbind_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * m_conn_req: - process MTP_CONN_REQ primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @mp: the MTP_CONN_REQ primitive
 */
noinline fastcall int
m_conn_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct MTP_conn_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * m_discon_req: - process MTP_DISCON_REQ primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @mp: the MTP_DISCON_REQ primitive
 */
noinline fastcall int
m_discon_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct MTP_discon_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * m_addr_req: - process MTP_ADDR_REQ primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @mp: the MTP_ADDR_REQ primitive
 */
noinline fastcall int
m_addr_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct MTP_addr_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * m_info_req: - process MTP_INFO_REQ primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @mp: the MTP_INFO_REQ primitive
 */
noinline fastcall int
m_info_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct MTP_info_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * m_optmgmt_req: - process MTP_OPTMGMT_REQ primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @mp: the MTP_OPTMGMT_REQ primitive
 */
noinline fastcall int
m_optmgmt_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct MTP_optmgmt_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

#ifdef MTP_STATUS_REQ
/**
 * m_status_req: - process MTP_STATUS_REQ primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @mp: the MTP_STATUS_REQ primitive
 */
noinline fastcall int
m_status_req(struct mtp *mpt, queue_t *q, mblk_t *mp)
{
	struct MTP_status_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
#endif

/**
 * m_transfer_req: - process MTP_TRANSFER_REQ primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @mp: the MTP_TRANSFER_REQ primitive
 */
noinline fastcall int
m_transfer_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct MTP_transfer_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * m_other_req: - process other primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @mp: the other primitive
 */
noinline fastcall int
m_other_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	mtp_ulong *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/*
 * MTP PROVIDER TO MTP USER PRIMITIVES
 * --------------------------------------------------------------------------
 */

/**
 * l_info_ack: - process LMI_INFO_ACK primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @mp: the LMI_INFO_ACK primitive
 */
noinline fastcall int
l_info_ack(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	lmi_info_ack_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * l_ok_ack: - process LMI_OK_ACK primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @mp: the LMI_OK_ACK primitive
 */
noinline fastcall int
l_ok_ack(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	lmi_ok_ack_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * l_error_ack: - process LMI_ERROR_ACK primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @mp: the LMI_ERROR_ACK primitive
 */
noinline fastcall int
l_error_ack(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	lmi_error_ack_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * l_enable_con: - process LMI_ENABLE_CON primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @mp: the LMI_ENABLE_CON primitive
 */
noinline fastcall int
l_enable_con(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	lmi_enable_con_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * l_disable_con: - process LMI_DISABLE_CON primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @mp: the LMI_DISABLE_CON primitive
 */
noinline fastcall int
l_disable_con(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	lmi_disable_con_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * l_optmgmt_ack: - process LMI_OPTMGMT_ACK primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @mp: the LMI_OPTMGMT_ACK primitive
 */
noinline fastcall int
l_optmgmt_ack(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	lmi_optmgmt_ack_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * l_error_ind: - process LMI_ERROR_IND primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @mp: the LMI_ERROR_IND primitive
 */
noinline fastcall int
l_error_ind(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	lmi_error_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * l_stats_ind: - process LMI_STATS_IND primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @mp: the LMI_STATS_IND primitive
 */
noinline fastcall int
l_stats_ind(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	lmi_stats_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * l_event_ind: - process LMI_EVENT_IND primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @mp: the LMI_EVENT_IND primitive
 */
noinline fastcall int
l_event_ind(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	lmi_event_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * m_ok_ack: - process MTP_OK_ACK primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @mp: the MTP_OK_ACK primitive
 */
noinline fastcall int
m_ok_ack(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct MTP_ok_ack *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * m_error_ack: - process MTP_ERROR_ACK primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @mp: the MTP_ERROR_ACK primitive
 */
noinline fastcall int
m_error_ack(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct MTP_error_ack *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * m_bind_ack: - process MTP_BIND_ACK primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @mp: the MTP_BIND_ACK primitive
 */
noinline fastcall int
m_bind_ack(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct MTP_bind_ack *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * m_addr_ack: - process MTP_ADDR_ACK primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @mp: the MTP_ADDR_ACK primitive
 */
noinline fastcall int
m_addr_ack(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct MTP_addr_ack *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * m_info_ack: - process MTP_INFO_ACK primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @mp: the MTP_INFO_ACK primitive
 */
noinline fastcall int
m_info_ack(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct MTP_info_ack *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * m_optmgmt_ack: - process MTP_OPTMGMT_ACK primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @mp: the MTP_OPTMGMT_ACK primitive
 */
noinline fastcall int
m_optmgmt_ack(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct MTP_optmgmt_ack *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * m_transfer_ind: - process MTP_TRANSFER_IND primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @mp: the MTP_TRANSFER_IND primitive
 */
noinline fastcall int
m_transfer_ind(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct MTP_transfer_ind *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * m_pause_ind: - process MTP_PAUSE_IND primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @mp: the MTP_PAUSE_IND primitive
 */
noinline fastcall int
m_pause_ind(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct MTP_pause_ind *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * m_resume_ind: - process MTP_RESUME_IND primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @mp: the MTP_RESUME_IND primitive
 */
noinline fastcall int
m_resume_ind(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct MTP_resume_ind *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * m_status_ind: - process MTP_STATUS_IND primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @mp: the MTP_STATUS_IND primitive
 */
noinline fastcall int
m_status_ind(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct MTP_status_ind *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * m_restart_begins_ind: - process MTP_RESTART_BEGINS_IND primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @mp: the MTP_RESTART_BEGINS_IND primitive
 */
noinline fastcall int
m_restart_begins_ind(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct MTP_restart_begins_ind *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * m_restart_complete_ind: - process MTP_RESTART_COMPLETE_IND primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @mp: the MTP_RESTART_COMPLETE_IND primitive
 */
noinline fastcall int
m_restart_complete_ind(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct MTP_restart_complete_ind *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * m_other_ind: - process other primitive
 * @mtp: private structure (locked)
 * @q: active queue
 * @mp: the other primitive
 */
noinline fastcall int
m_other_ind(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	mtp_ulong *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/*
 * M_PROTO, M_PCPROTO message handling
 * --------------------------------------------------------------------------
 */

static inline fastcall int
mtp_w_proto(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	mtp_ulong prim = *(typeof(prim) *) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(prim))))
		goto tooshort;

	mtp_save_state(mtp);
	mtp_rxprim(mtp, q, mp, prim);

	switch (__builtin_expect(prim < MTP_TRANSFER_REQ)) {
	case LMI_INFO_REQ:
		err = l_info_req(mtp, q, mp);
		break;
	case LMI_ATTACH_REQ:
		err = l_attach_req(mtp, q, mp);
		break;
	case LMI_DETACH_REQ:
		err = l_detach_req(mtp, q, mp);
		break;
	case LMI_ENABLE_REQ:
		err = l_enable_req(mtp, q, mp);
		break;
	case LMI_DISABLE_REQ:
		err = l_disable_req(mtp, q, mp);
		break;
	case LMI_OPTMGMT_REQ:
		err = l_optmgmt_req(mtp, q, mp);
		break;
	case MTP_BIND_REQ:
		err = m_bind_req(mtp, q, mp);
		break;
	case MTP_UNBIND_REQ:
		err = m_unbind_req(mtp, q, mp);
		break;
	case MTP_CONN_REQ:
		err = m_conn_req(mtp, q, mp);
		break;
	case MTP_DISCON_REQ:
		err = m_discon_req(mtp, q, mp);
		break;
	case MTP_ADDR_REQ:
		err = m_addr_req(mtp, q, mp);
		break;
	case MTP_INFO_REQ:
		err = m_info_req(mtp, q, mp);
		break;
	case MTP_OPTMGMT_REQ:
		err = m_optmgmt_req(mtp, q, mp);
		break;
#ifdef MTP_STATUS_REQ
	case MTP_STATUS_REQ:
		err = m_status_req(mtp, q, mp);
		break;
#endif
	case MTP_TRANSFER_REQ:
		err = m_transfer_req(mtp, q, mp);
		break;
	default:
		err = m_other_req(mtp, q, mp);
		break;
	}

      error:
	if (err) {
		mtp_restore_state(mtp);
		return mtp_error_reply(mtp, q, mp, err, prim);
	}
	return (err);
      tooshort:
	err = -EMSGSIZE;
	goto error;
}
static inline fastcall int
mtp_r_proto(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	mtp_ulong prim = *(typeof(prim) *) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(prim))))
		goto tooshort;

	mtp_save_state(mtp);
	mtp_rxprim(mtp, q, mp, prim);

	switch (__builtin_expect(prim, MTP_TRANSFER_IND)) {
	case LMI_INFO_ACK:
		err = l_info_ack(mtp, q, mp);
		break;
	case LMI_OK_ACK:
		err = l_ok_ack(mtp, q, mp);
		break;
	case LMI_ERROR_ACK:
		err = l_error_ack(mtp, q, mp);
		break;
	case LMI_ENABLE_CON:
		err = l_enable_con(mtp, q, mp);
		break;
	case LMI_DISABLE_CON:
		err = l_disable_con(mtp, q, mp);
		break;
	case LMI_OPTMGMT_ACK:
		err = l_optmgmt_ack(mtp, q, mp);
		break;
	case LMI_ERROR_IND:
		err = l_error_ind(mtp, q, mp);
		break;
	case LMI_STATS_IND:
		err = l_stats_ind(mtp, q, mp);
		break;
	case LMI_EVENT_IND:
		err = l_event_ind(mtp, q, mp);
		break;
	case MTP_OK_ACK:
		err = m_ok_ack(mtp, q, mp);
		break;
	case MTP_ERROR_ACK:
		err = m_error_ack(mtp, q, mp);
		break;
	case MTP_BIND_ACK:
		err = m_bind_ack(mtp, q, mp);
		break;
	case MTP_ADDR_ACK:
		err = m_addr_ack(mtp, q, mp);
		break;
	case MTP_INFO_ACK:
		err = m_info_ack(mtp, q, mp);
		break;
	case MTP_OPTMGMT_ACK:
		err = m_optmgmt_ack(mtp, q, mp);
		break;
	case MTP_TRANSFER_IND:
		err = m_transfer_ind(mtp, q, mp);
		break;
	case MTP_PAUSE_IND:
		err = m_pause_ind(mtp, q, mp);
		break;
	case MTP_RESUME_IND:
		err = m_resume_ind(mtp, q, mp);
		break;
	case MTP_STATUS_IND:
		err = m_status_ind(mtp, q, mp);
		break;
	case MTP_RESTART_BEGINS_IND:
		err = m_restart_begins_ind(mtp, q, mp);
		break;
	case MTP_RESTART_COMPLETE_IND:
		err = m_restart_complete_ind(mtp, q, mp);
		break;
	default:
		err = m_other_ind(mtp, q, mp);
		break;
	}

      error:
	if (err) {
		mtp_restore_state(mtp);
		return mtp_error_reply(mtp, q, mp, err, prim);
	}
	return (err);
      tooshort:
	err = -EMSGSIZE;
	goto error;
}

/*
 * M_IOCTL, M_IOCDATA message handling
 * --------------------------------------------------------------------------
 */

noinline fastcall int
mtp_w_ioctl(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
}
noinline fastcall int
mtp_r_ioctl(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
}

/*
 * M_CTL, M_PCCTL message handling
 * --------------------------------------------------------------------------
 */

noinline fastcall int
mtp_w_ctl(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
}
noinline fastcall int
mtp_r_ctl(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
}

/*
 * M_DATA, M_HPDATA message handling
 * --------------------------------------------------------------------------
 */

noinline fastcall int
mtp_w_data(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
}
noinline fastcall int
mtp_r_data(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
}

/*
 * M_FLUSH message handling
 * --------------------------------------------------------------------------
 */

/**
 * m_w_flush: - process M_FLUSH message
 * @q: active queue (upper write queue)
 * @mp: the M_FLUSH message
 *
 * If there is an associated lower queue, pass the M_FLUSH downward, otherwise, turn
 * it back around.  Note that the lower write queue and upper read queue never hold
 * data.
 */
noinline fastcall int
m_w_flush(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp;
	queue_t *oq;

	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHBAND);
	}
	read_lock(&mtp_lock);
	if ((mtp = MTP_PRIV(q)) && mtp->other && (oq = mtp->other->oq)) {
		putnext(oq, mp);
		read_unlock(&mtp_lock);
		return (0);
	}
	read_unlock(&mtp_lock);
	mp->b_rptr[0] &= ~FLUSHW;
	if (mp->b_rptr[0] & FLUSHR)
		qreply(q, mp);
	else
		freemsg(mp);
	return (0);

}

/**
 * m_r_flush: - process M_FLUSH message
 * @q: active queue (lower read queue)
 * @mp: the M_FLUSH message
 *
 * If there is an associated upper queue, pass the M_FLUSH upward, otherwise, turn
 * it back around.  Note that the upper read queue and lower write queue never hold
 * data.
 */
noinline fastcall int
m_r_flush(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp;
	queue_t oq;

	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
	}
	read_lock(&mtp_lock);
	if ((mtp = MTP_PRIV(q)) && mtp->other && (oq = mtp->other->oq)) {
		putnext(oq, mp);
		read_unlock(&mtp_lock);
		return (0);
	}
	read_unlock(&mtp_lock);
	mp->b_rptr[0] &= ~FLUSHR;
	if (mp->b_rptr[0] & FLUSHW)
		qreply(q, mp);
	else
		freemsg(mp);
	return (0);
}

/*
 * M_ERROR message handling
 * --------------------------------------------------------------------------
 */

/**
 * m_r_error: - process M_ERROR message
 * @q: active queue (lower read queue)
 * @mp: the M_ERROR message
 */
noinline fastcall int
m_r_error(queue_t *q, mblk_t *mp)
{
}

/*
 * M_HANGUP message handling
 * --------------------------------------------------------------------------
 */

/**
 * m_r_hangup: - process M_HANGUP message
 * @q: active queue (lower read queue)
 * @mp: the M_HANGUP message
 */
noinline fastcall int
m_r_hangup(queue_t *q, mblk_t *mp)
{
}

/*
 * Other message handling
 * --------------------------------------------------------------------------
 */

/**
 * m_w_other: - process other message
 * @q: active queue (upper write queue)
 * @mp: the other message
 */
noinline fastcall int
m_w_other(queue_t *q, mblk_t *mp)
{
	mi_strlog(q, STRLOGERR, SL_TRACE | SL_ERROR,
		  "unhandled STREAMS message %d on upper write queue", (int) DB_TYPE(mp));
	freemsg(mp);
	return (0);
}

/**
 * m_r_other: - process other message
 * @q: active queue (lower read queue)
 * @mp: the other message
 */
noinline fastcall int
m_r_other(queue_t *q, mblk_t *mp)
{
	mi_strlog(q, STRLOGERR, SL_TRACE | SL_ERROR,
		  "unhandled STREAMS message %d on lower read queue", (int) DB_TYPE(mp));
	freemsg(mp);
	return (0);
}

/*
 * STREAMS MESSAGE HANDLING
 * --------------------------------------------------------------------------
 */

static int
mtp_w_msg(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_PROTO)) {
	case M_PROTO:
	case M_PCPROTO:
		return mtp_w_proto(mtp, q, mp);
	case M_IOCTL:
	case M_IOCDATA:
		return mtp_w_ioctl(mtp, q, mp);
	case M_CTL:
	case M_PCCTL:
		return mtp_w_ctl(mtp, q, mp);
	case M_DATA:
	case M_HPDATA:
		return mtp_w_data(mtp, q, mp);
	case M_FLUSH:
		return m_w_flush(q, mp);
	default:
		return m_w_other(q, mp);
	}
}

static int
mtp_r_msg(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_PROTO:
	case M_PCPROTO:
		return mtp_r_proto(mtp, q, mp);
	case M_IOCACK:
	case M_IOCNAK:
	case M_COPYIN:
	case M_COPYOUT:
		return mtp_r_ioctl(mtp, q, mp);
	case M_CTL:
	case M_PCCTL:
		return mtp_r_ctl(mtp, q, mp);
	case M_DATA:
	case M_HPDATA:
		return mtp_r_data(mtp, q, mp);
	case M_FLUSH:
		return m_r_flush(q, mp);
	case M_ERROR:
		return m_r_error(q, mp);
	case M_HANGUP:
		return m_r_hangup(q, mp);
	default:
		return m_r_other(q, mp);
	}
}

static int
m_w_msg(queue_t *q, mblk_t *mp)
{
	unsigned char type;

	switch (__builtin_expect((type = DB_TYPE(mp)), M_PROTO)) {
	case M_PROTO:
	case M_IOCTL:
	case M_CTL:
	case M_DATA:
		return (-EAGAIN);	/* always queue for performance */
	case M_HPDATA:
	case M_PCPROTO:
	case M_IOCDATA:
	case M_PCCTL:
	{
		struct mtp *mtp;
		int err;

		if (unlikely(!(mtp = (struct mtp *) mi_trylock(q))))
			return (-EDEADLK);
		switch (type) {
		case M_PCPROTO:
			err = mtp_w_proto(mtp, q, mp);
			break;
		case M_IOCDATA:
			err = mtp_w_ioctl(mtp, q, mp);
			break;
		case M_PCCTL:
			err = mtp_w_ctl(mtp, q, mp);
			break;
		case M_HPDATA:
			err = mtp_w_data(mtp, q, mp);
			break;
		}
		mi_unlock((caddr_t) mtp);
		return (err);
	}
	case M_FLUSH:
		return m_w_flush(q, mp);
	default:
		return m_w_other(q, mp);
	}
}

static int
m_r_msg(queue_t *q, mblk_t *mp)
{
	unsigned char type;

	switch (__builtin_expect((type = DB_TYPE(mp)), M_PROTO)) {
	case M_PROTO:
	case M_CTL:
	case M_DATA:
		return (-EAGAIN);	/* always queue for performance */
	case M_HPDATA:
	case M_PCPROTO:
	case M_IOCACK:
	case M_IOCNAK:
	case M_COPYIN:
	case M_COPYOUT:
	case M_PCCTL:
	{
		struct mtp *mtp;
		int err;

		if (unlikely(!(mtp = (struct mtp *) mi_trylock(q))))
			return (-EDEADLK);
		switch (type) {
		case M_PCPROTO:
			err = mtp_r_proto(mtp, q, mp);
		case M_IOCACK:
		case M_IOCNAK:
		case M_COPYIN:
		case M_COPYOUT:
			err = mtp_r_ioctl(mtp, q, mp);
		case M_PCCTL:
			err = mtp_r_ctl(mtp, q, mp);
		case M_HPDATA:
			err = mtp_r_data(mtp, q, mp);
		}
		mi_unlock((caddr_t) mtp);
		return (err);
	}
	case M_FLUSH:
		return m_r_flush(q, mp);
	case M_ERROR:
		return m_r_error(q, mp);
	case M_HANGUP:
		return m_r_hangup(q, mp);
	default:
		return m_r_other(q, mp);
	}
}

/*
 * PUT AND SERVICE PROCEDURES
 * --------------------------------------------------------------------------
 */

static inline fastcall void
mtp_putq(queue_t *q, mblk_t *mp)
{
	if (unlikely(!putq(q, mp))) {
		mp->b_band = 0;
		putq(q, mp);
	}
}

static inline fastcall void
mtp_putbq(queue_t *q, mblk_t *mp)
{
	if (unlikely(!putbq(q, mp))) {
		mp->b_band = 0;
		putbq(q, mp);
	}
}

/**
 * mtp_wput: - MTP user write put procedure
 * @q: upper write queue
 * @mp: message to put
 */
static streamscall int
mtp_wput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flags & QSVCBUSY))) || m_w_msg(q, mp))
		mtp_putq(q, mp);
	return (0);
}

/**
 * mtp_wsrv: - MTP user write service procedure
 * @q: upper write queue
 */
static streamscall int
mtp_wsrv(queue_t *q)
{
	struct mtp *mtp;
	mblk_t *mp;

	if (likely(!!(mtp = (struct mtp *) mi_trylock(q)))) {
		while (likely(!!(mp = getq(q)))) {
			if (unlikely(mtp_w_msg(mtp, q, mp))) {
				mtp_putbq(q, mp);
				break;
			}
		}
		mi_unlock((caddr_t) mtp);
	}
	return (0);
}

/**
 * mtpx_rsrv: - MTP user read service procedure
 * @q: upper read queue
 *
 * The upper read service procedure is only used for flow control across the
 * multiplexing driver.  When the service procedure runs, it enables all feeding
 * lower queues across the multiplexing driver.
 */
static streamscall int
mtpx_rsrv(queue_t *q)
{
	struct mtp *mtp;
	mblk_t *mp;

	if (likely(!!(mtp = (struct mtp *) mi_trylock(q)))) {
		while (likely(!!(mp = getq(q)))) {
			if (unlikely(mtp_r_msg(mtp, q, mp))) {
				mtp_putbq(q, mp);
				break;
			}
		}
		mi_unlock((caddr_t) mtp);
	}
	return (0);
}

/**
 * mtpx_wsrv: - MTP provider write service procedure
 * @q: lower write queue
 *
 * The lower write service procedure is only used to flow control across the
 * multiplexing driver.  When the service procedure runs, it enables all feeding
 * upper queues across the multiplexing driver.
 */
static streamscall int
mtpx_wsrv(queue_t *q)
{
}

/**
 * mtp_rsrv: - MTP provider read service procedure
 * @q: lower read queue
 */
static streamscall int
mtp_rsrv(queue_t *q)
{
}

/**
 * mtp_rput: - MTP provider read put procedure
 * @q: lower read queue
 * @mp: message to put
 */
static streamscall int
mtp_rput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flags & QSVCBUSY))) || m_r_msg(q, mp))
		mtp_putq(q, mp);
	return (0);
}

/*
 * OPEN AND CLOSE ROUTINES
 * --------------------------------------------------------------------------
 */

/**
 * mtp_qopen: - STREAMS open routine
 * @q: read queue of queue pair opened
 * @devp: device number pointer
 * @oflags: open flags
 * @sflag: STREAMS flag
 * @credp: credentials of opening process
 *
 * There are several clone minor device numbers:
 *
 * 0 - /dev/mtp, basic MTP Stream clone minor device
 * 1 - /dev/mtp-npi, basic MTP Stream clone minor device with autopushed MTP-NPI module
 * 2 - /dev/mtp-tpi, basic MTP Stream clone minor device with autopushed MTP-TPI module
 * 3 - /dev/mtp-admin, basic MTP admin minor device
 * 4 - /dev/mtp-stats, basic MTP statistics minor device
 * 5 - /dev/mtp-events, basic MTP events minor device
 * 6 - /dev/mtp-mon, basic MTP xraying minor device
 */
static streamscall int
mtp_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *credp)
{
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);
	int err;

	if (q->q_ptr != NULL)
		return (0);
	if (sflag == MODOPEN || q->q_next != NULL)
		return (ENXIO);

	switch (cminor) {
	case MTP_MINOR:
	case MTP_NPI_MINOR:
	case MTP_TPI_MINOR:
	case MTP_ADM_MINOR:
	case MTP_STA_MINOR:
	case MTP_EVT_MINOR:
	case MTP_MON_MINOR:
		sflag = CLONEOPEN;
		break;
	default:
		return (ENXIO);
	}
	write_lock(&mtp_lock);
	if ((err =
	     mi_open_comm(&mtp_opens, sizeof(struct mtp), q, devp, oflags, sflag, credp))) {
		write_unlock(&mtp_lock);
		return (err);
	}
	mtpu_init_priv(q, devp, oflags, sflag, credp, cminor);
	write_unlock(&mtp_lock);
	qprocson(q);
	return (0);
}

/**
 * mtp_qclose: - STREAMS close routine
 * @q: read queue of queue pair closed
 * @oflags: open flags
 * @credp: credentials of closing process
 */
static streamscall int
mtp_qclose(queue_t *q, int oflags, cred_t *credp)
{
	qprocsoff(q);
	write_lock(&mtp_lock);
	mtpu_term_priv(q);
	mi_close_comm(&mtp_opens, q);
	write_unlock(&mtp_lock);
	return (0);
}

/*
 * STREAMS DEFINITIONS
 * --------------------------------------------------------------------------
 */

static struct module_info mtpu_minfo = {
	.mi_idnum = DRV_ID,
	.mi_idname = DRV_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat mtpu_mstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static struct module_info mtpp_minfo = {
	.mi_idnum = DRV_ID,
	.mi_idname = DRV_NAME "-mux",
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat mtpp_mstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static struct qinit mtp_rdinit = {
	.qi_putp = NULL,	/* it is an error to put to this queue */
	.qi_srvp = mtpx_rsrv,
	.qi_qopen = mtp_qopen,
	.qi_qclose = mtp_qclose,
	.qi_minfo = &mtpu_minfo,
	.qi_mstat = &mtpu_mstat,
};

static struct qinit mtp_wrinit = {
	.qi_putp = mtp_wput,
	.qi_srvp = mtp_wsrv,
	.qi_minfo = &mtpu_minfo,
	.qi_mstat = &mtpu_mstat,
};

static struct qinit mtp_muxrinit = {
	.qi_putp = mtp_rput,
	.qi_srvp = mtp_rsrv,
	.qi_minfo = &mtpp_minfo,
	.qi_mstat = &mtpp_mstat,
};

static struct qinit mtp_muxwinit = {
	.qi_putp = NULL,	/* it is an error to put to this queue */
	.qi_srvp = mtpx_wsrv,
	.qi_minfo = &mtpp_minfo,
	.qi_mstat = &mtpp_mstat,
};

static struct streamtab mtp_muxinfo = {
	.st_rdinit = &mtp_rdinit,
	.st_wrinit = &mtp_wrinit,
	.st_muxrinit = &mtp_muxrinit,
	.st_muxwinit = &mtp_muxwinit,
};

static struct cdevsw mtp_cdev = {
	.d_name = DRV_NAME,
	.d_str = mtp_muxinfo,
	.d_flags = D_MP,
	.d_kmod = THIS_MODULE,
};

/**
 * mtp_node: - regulare MTP device node
 */
static struct devnode mtp_node = {
	.n_name = "mtp",
	.n_flag = D_CLONE,
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

/**
 * mtp_npi_node: - MTP device node with MTP-NPI module autopushed
 */
static struct devnode mtp_npi_node = {
	.n_name = "mtp-npi",
	.n_flag = D_CLONE,
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

/**
 * mtp_tpi_node: - MTP device node with MTP-TPI module autopushed
 */
static struct devnode mtp_tpi_node = {
	.n_name = "mtp-tpi",
	.n_flag = D_CLONE,
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

/**
 * mtp_adm_node: - MTP administration node, used for configuring the driver
 */
static struct devnode mtp_adm_node = {
	.n_name = "mtp-admin",
	.n_flag = D_CLONE,
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

/**
 * mtp_sta_node: - MTP statistics node, used for collecting statistics
 */
static struct devnode mtp_sta_node = {
	.n_name = "mtp-stats",
	.n_flag = D_CLONE,
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

/**
 * mtp_evt_node: - MTP events node, used for collecting events (and alarms)
 */
static struct devnode mtp_evt_node = {
	.n_name = "mtp-events",
	.n_flag = D_CLONE,
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

/**
 * mtp_mon_node: - MTP monitor node, used for xraying MTP users and providers
 */
static struct devnode mtp_mon_node = {
	.n_name = "mtp-mon",
	.n_flag = D_CLONE,
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

static struct strapush mtp_npi_push = {
	.sap_cmd = SAD_ONE,
	.sap_major = DRV_CMAJOR_0,
	.sap_minor = MTP_NPI_MINOR,
	.sap_lastminor = MTP_NPI_MINOR,
	.sap_npush = 1,
	.sap_list = {"mtp-npi",},
	.sap_anchor = 0,
	.sap_module = "mtp-mux",
};

static struct strapush mtp_tpi_push = {
	.sap_cmd = SAD_ONE,
	.sap_major = DRV_CMAJOR_0,
	.sap_minor = MTP_TPI_MINOR,
	.sap_lastminor = MTP_TPI_MINOR,
	.sap_npush = 1,
	.sap_list = {"mtp-tpi",},
	.sap_anchor = 0,
	.sap_module = "mtp-mux",
};

#ifdef module_param
module_param(major, ushort, 0444);
#else				/* module_param */
MODULE_PARM(major, "h");
#endif				/* module_param */
MODULE_PARM_DESC(major, "Major device number for MTP-MUX driver. (0 for allocation.)");

static int __init
mtp_muxinit(void)
{
	int err;

	cmn_err(CE_NOTE, DRV_BANNER);
	if ((err = regsiter_strdev(&mtp_cdev, major)) < 0) {
		cmn_err(CE_WARN, "%s: could not regsiter driver, err = %d\n", DRV_NAME, -err);
		goto no_mtp;
	}
	if (major == 0)
		major = err;
	if (unlikely((err = register_strnod(&mtp_dev, &mtp_node, MTP_MINOR)) < 0)) {
		cmn_err(CE_WARN, "%s: could not register minor %d, err = %d\n", DRV_NAME,
			MTP_MINOR, -err);
		goto no_mtp_node;
	}
	if (unlikely((err = register_strnod(&mtp_dev, &mtp_npi_node, MTP_NPI_MINOR)) < 0)) {
		cmn_err(CE_WARN, "%s: could not register minor %d, err = %d\n", DRV_NAME,
			MTP_NPI_MINOR, -err);
		goto no_mtp_npi_node;
	}
	if (unlikely((err = register_strnod(&mtp_dev, &mtp_tpi_node, MTP_TPI_MINOR)) < 0)) {
		cmn_err(CE_WARN, "%s: could not register minor %d, err = %d\n", DRV_NAME,
			MTP_TPI_MINOR, -err);
		goto no_mtp_tpi_node;
	}
	if (unlikely((err = register_strnod(&mtp_dev, &mtp_adm_node, MTP_ADM_MINOR)) < 0)) {
		cmn_err(CE_WARN, "%s: could not register minor %d, err = %d\n", DRV_NAME,
			MTP_ADM_MINOR, -err);
		goto no_mtp_adm_node;
	}
	if (unlikely((err = register_strnod(&mtp_dev, &mtp_sta_node, MTP_STA_MINOR)) < 0)) {
		cmn_err(CE_WARN, "%s: could not register minor %d, err = %d\n", DRV_NAME,
			MTP_STA_MINOR, -err);
		goto no_mtp_sta_node;
	}
	if (unlikely((err = register_strnod(&mtp_dev, &mtp_evt_node, MTP_EVT_MINOR)) < 0)) {
		cmn_err(CE_WARN, "%s: could not register minor %d, err = %d\n", DRV_NAME,
			MTP_EVT_MINOR, -err);
		goto no_mtp_evt_node;
	}
	if (unlikely((err = register_strnod(&mtp_dev, &mtp_mon_node, MTP_MON_MINOR)) < 0)) {
		cmn_err(CE_WARN, "%s: could not register minor %d, err = %d\n", DRV_NAME,
			MTP_MON_MINOR, -err);
		goto no_mtp_mon_node;
	}
	if (unlikely((err = -autopush_add(&mtp_npi_push)) < 0)) {
		cmn_err(CE_WARN, "%s: could not add autopush %d, err = %d\n", DRV_NAME,
			MTP_NPI_MINOR, -err);
		goto no_mtp_npi_push;
	}
	if (unlikely((err = -autopush_add(&mtp_tpi_push)) < 0)) {
		cmn_err(CE_WARN, "%s: could not add autopush %d, err = %d\n", DRV_NAME,
			MTP_TPI_MINOR, -err);
		goto no_mtp_tpi_push;
	}
	return (0);
	autopush_del(&mtp_tpi_push);
      np_mtp_tpi_push:
	autopush_del(&mtp_npi_push);
      no_mtp_npi_push:
	unregister_strnod(&mtp_cdev, &mtp_mon_node, MTP_MON_MINOR);
      no_mtp_mon_node:
	unregister_strnod(&mtp_cdev, &mtp_evt_node, MTP_EVT_MINOR);
      no_mtp_evt_node:
	unregister_strnod(&mtp_cdev, &mtp_sta_node, MTP_STA_MINOR);
      no_mtp_sta_node:
	unregister_strnod(&mtp_cdev, &mtp_adm_node, MTP_ADM_MINOR);
      no_mtp_adm_node:
	unregister_strnod(&mtp_cdev, &mtp_tpi_node, MTP_TPI_MINOR);
      no_mtp_tpi_node:
	unregister_strnod(&mtp_cdev, &mtp_npi_node, MTP_NPI_MINOR);
      no_mtp_npi_node:
	unregister_strnod(&mtp_cdev, &mtp_node, MTP_MINOR);
      no_mtp_node:
	unregister_strdev(&mtp_cdev, major);
      no_mtp:
	return (err);
}

static void __exit
mtp_muxterminate(void)
{
	int err;

	if (unlikely((err = -autopush_del(&mtp_tpi_push)) < 0))
		cmn_err(CE_WARN, "%s: could not delete autopush %d, err = %d\n", DRV_NAME,
			MTP_TPI_MINOR, -err);
	if (unlikely((err = -autopush_del(&mtp_npi_push)) < 0))
		cmn_err(CE_WARN, "%s: could not delete autopush %d, err = %d\n", DRV_NAME,
			MTP_NPI_MINOR, -err);
	if (unlikely((err = unregister_strnod(&mtp_cdev, &mtp_mon_node, MTP_MON_MINOR)) < 0))
		cmn_err(CE_WARN, "%s: could not unregister minor %d, err = %d\n", DRV_NAME,
			MTP_MON_MINOR, -err);
	if (unlikely((err = unregister_strnod(&mtp_cdev, &mtp_evt_node, MTP_EVT_MINOR)) < 0))
		cmn_err(CE_WARN, "%s: could not unregister minor %d, err = %d\n", DRV_NAME,
			MTP_EVT_MINOR, -err);
	if (unlikely((err = unregister_strnod(&mtp_cdev, &mtp_sta_node, MTP_STA_MINOR)) < 0))
		cmn_err(CE_WARN, "%s: could not unregister minor %d, err = %d\n", DRV_NAME,
			MTP_STA_MINOR, -err);
	if (unlikely((err = unregister_strnod(&mtp_cdev, &mtp_adm_node, MTP_ADM_MINOR)) < 0))
		cmn_err(CE_WARN, "%s: could not unregister minor %d, err = %d\n", DRV_NAME,
			MTP_ADM_MINOR, -err);
	if (unlikely((err = unregister_strnod(&mtp_cdev, &mtp_tpi_node, MTP_TPI_MINOR)) < 0))
		cmn_err(CE_WARN, "%s: could not unregister minor %d, err = %d\n", DRV_NAME,
			MTP_TPI_MINOR, -err);
	if (unlikely((err = unregister_strnod(&mtp_cdev, &mtp_npi_node, MTP_NPI_MINOR)) < 0))
		cmn_err(CE_WARN, "%s: could not unregister minor %d, err = %d\n", DRV_NAME,
			MTP_NPI_MINOR, -err);
	if (unlikely((err = unregister_strnod(&mtp_cdev, &mtp_node, MTP_MINOR)) < 0))
		cmn_err(CE_WARN, "%s: could not unregister minor %d, err = %d\n", DRV_NAME,
			MTP_MINOR, -err);
	if (unlikely((err = unregister_strdev(&mtp_cdev, major))))
		cmn_err(CE_WARN, "%s: could not unregister driver, err = %d\n", DRV_NAME,
			-err);
	return;
}

module_init(mtp_muxinit);
module_exit(mtp_muxterminate);
