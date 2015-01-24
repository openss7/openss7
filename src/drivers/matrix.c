/*****************************************************************************

 @(#) File: src/drivers/matrix.c

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

static char const ident[] = "src/drivers/matrix.c (" PACKAGE_ENVR ") " PACKAGE_DATE;

/*
 * This is the MATRIX multiplexing driver.  Its purpose is to allow a single
 * device, /dev/streams/matrix, to represent all multiplex and channel device
 * streams in the system.  The Matrix SNMP (mxMIB) configuration script opens
 * the appropriate device drivers (MX and CH) and links (I_PLINKs) them under
 * this multiplexer to form a single view of the device drivers.  It is then
 * possible to open a single MX or CH upper streams on this multiplexer and
 * either use it directly or link it, with appropriate modules pushed, under
 * another multiplexing device driver.  Examples for MX streams would be
 * linking them under the MG multiplexer or ZAPTEL multiplexer.  Examples for CH
 * stream could be the X.25 PLP with HLDC and LAPB modules pushed, or the MTP
 * driver with SDT and SL moudles pushed.
 *
 * Using MX or CH pipe moudles, it is possible to open a STREAMS based pipe,
 * push the mx-pmod or ch-pmod module on one end of the pipe and then link both
 * ends benath this multiplexer.  This permits testing of the multiplexer as
 * well as providing an internal switching matrix capability.
 *
 * The driver also provides pseudo-digital cross-connect capabilities within
 * lower MX streams, between lower MX streams, between lower CH streams, or
 * between CH streams and MX streams.  This cross-conect can be performe on a
 * dynamic basis (switching), a semi-permanent basis (DCCS), or a permanent
 * basis (channel bank).
 *
 * Lower MX and CH streams are always treated as Style 1 Streams.  That is, if
 * the lower Stream is in fact a Style 2 Stream, it must be attached to a PPA
 * before it is linked beneath the multiplexing driver.  Clone upper CH and MX
 * Streams are always treated as Style 2 Streams.  PPA addressing for the upper
 * CH and MX Streams is performed by using a globally assigned PPA or CLEI that
 * is assigned to a lower Stream once it is linked beneath the multiplexing
 * driver.  Adddresses for CH Streams are derived from the PPA and CLEI for
 * lower Streams and the slot map used in the enable and connect primitives is
 * used to select the channel(s) from the lower MX Stream.  Monitoring can be
 * accomplished, even for pseudo-digital cross-connect channels, by enabling and
 * connecting the upper CH stream in monitoring mode.
 *
 * Whether an upper stream is a CH stream or an MX stream is selected using the
 * minor device number of the opened Stream.  Upper CH and MX streams can be
 * clone streams or non-clone streams.  Clone streams are Style 2 streams that
 * can be attache to any available PPA.  Non-clone streams have a minor device
 * number that maps to a global PPA or CLEI of a lower Stream (the mapping is
 * assigned when the Stream is I_PLINKED beneath the multiplexing driver).
 * These are Style 1 MX or CH streams.  Only individual channel or full span CH
 * streams are available as Style 1 streams.  Fractional CH streams must utilize
 * the clone CH device number and attach to the appropriate fraction of the
 * span.
 *
 * The primary reason that the MX and CH portion of the switching matrix are
 * combined into one multiplexing device driver is to support the switching of
 * multi-rate ISDN and ISUP calls, where multiple 64 kbps channels are ganged
 * together to provide the mulitrate connection.
 *
 * Note that the only time that it is necessary to pass channel data to and from
 * the driver using an upper CH Stream is for data or voice connections that are
 * terminated on the host.  For example, X.25 connections that are terminated on
 * this host open a CH stream and push appropriate HDLC and LAPB modules and
 * link the resulting stream under the X.25 PLP driver.  Similarly, for SS7
 * MTP2, this host opens a CH stream and pushes an SDT and SL module and links
 * the resulting stream under the MTP driver.  A voice example is where a CH
 * stream is opened and attached to a channel and an application plays an
 * announcement or one into the connection.
 *
 * Note that this switching matrix is not capable of performing channel
 * conversion that is not provided by the lower CH or MX streams themselves.
 * All pseudo-digital cross-connect is performed clear channel and consists of
 * simple full or half duplex connections.  At a later date, one exception to
 * this might be the non-intensive mu-law to A-law conversion.
 *
 * To peferom dynamic media conversion, conferencing, or other media conversion
 * functions, MX and CH upper streams on this driver (or their corresponding
 * underlying devices) should be linked beneath the MG driver.  However, static
 * media conversion can also be performed using a module or the device driver
 * directly.  For example, VT over IP or RTP streams can have conversion modules
 * pushed to provide an MX or CH interface that can then be linked directly
 * underneath this multiplexing driver.  For example, the RTP stream then can
 * convert between the codec in use on the RTP stream and either mu-law or A-law
 * PCM presented to this driver as a CH stream.  To facilitate this, the upper
 * management CH stream an pass whatever input-output controls it wishes to a
 * managed lower CH stream.  For example, this could be used to set the codec
 * used by a lower RTP stream before it is enabled.  The same is true for the MX
 * management stream.
 */

#include <sys/os7/compat.h>

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/chi.h>
#include <ss7/chi_ioctl.h>
#include <ss7/mxi.h>
#include <ss7/mxi_ioctl.h>

#define MATRIX_DESCRIP		"MATRIX (MX) STREAMS Multiplexing Driver"
#define MATRIX_EXTRA		"Part of the OpenSS7 VoIP Stack for Linux Fast-STREAMS"
#define MATRIX_REVISION		"OpenSS7 File: " __FILE__ "  Version: " PACKAGE_ENVR "  Date: " PACKAGE_DATE
#define MATRIX_COPYRIGHT	"Copyright (c) 2008-2015  Monavacon Limited.  All Rights Reserved."
#define MATRIX_DEVICE		"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define MATRIX_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define MATRIX_LICENSE		"GPL"
#define MATRIX_BANNER		MATRIX_DESCRIP		"\n" \
				MATRIX_EXTRA		"\n" \
				MATRIX_REVISION		"\n" \
				MATRIX_COPYRIGHT	"\n" \
				MATRIX_DEVICE		"\n" \
				MATRIX_CONTACT		"\n"
#define MATRIX_SPLASH		MATRIX_DESCRIP		" - " \
				MATRIX_REVISION

#if defined LINUX

MODULE_AUTHOR(MATRIX_CONTACT);
MODULE_DESCRIPTION(MATRIX_DESCRIP);
MODULE_SUPPORTED_DEVICE(MATRIX_DEVICE);
#if defined MODULE_LICENSE
MODULE_LICENSE(MATRIX_LICENSE);
#endif				/* defined MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-matrix");
#endif				/* defined MODULE_ALIAS */
#if defined MODULE_VERSION
MODULE_VERSION(PACKAGE_ENVR);
#endif				/* defined MODULE_VERSION */

#endif				/* defined LINUX */

#define MATRIX_DRV_ID		CONFIG_STREAMS_MATRIX_MODID
#define MATRIX_DRV_NAME		CONFIG_STREAMS_MATRIX_NAME
#define MATRIX_CMAJORS		CONFIG_STREAMS_MATRIX_NMAJORS
#define MATRIX_CMAJOR_0		CONFIG_STREAMS_MATRIX_MAJOR
#define MATRIX_UNITS		CONFIG_STREAMS_MATRIX_NMINORS

modID_t modid = MATRIX_DRV_ID;
major_t major = MATRIX_CMAJOR_0;

#if defined LINUX

#if defined module_param
module_param(modid, ushort, 0444);
module_param(major, uint, 0444);
#else				/* defined module_param */
MODULE_PARM(modid, "h");
MODULE_PARM(major, "d");
#endif				/* defined module_param */
MODULE_PARM_DESC(modid, "Module id number for MATRIX driver (0 for allocation).");
MODULE_PARM_DESC(major, "Major device number for MATRIX driver (0 for allocation).");

#if defined MODULE_ALIAS
MODULE_ALIAS("streams-modid-" __stringify(MATRIX_MODID));
MODULE_ALIAS("streams-driver-matrix");
MODULE_ALIAS("streams-major-" __stringify(MATRIX_CMAJOR_0));
MODULE_ALIAS("/dev/streams/matrix");
MODULE_ALIAS("/dev/streams/matrix/mx");
MODULE_ALIAS("/dev/streams/matrix/ch");
MODULE_ALIAS("/dev/streams/matrix/admin");
MODULE_ALIAS("/dev/stremas/clone/matrix");
MODULE_ALIAS("char-major-" __stringify(CONFIG_STREAMS_CLONE_MAJOR) "-" __stringify(MATRIX_CMAJOR_0));
MODULE_ALIAS("/dev/matrix");
//MODULE_ALIAS("devname:matrix");
#endif				/* defined MODULE_ALIAS */

#endif				/* defined LINUX */

#define DRV_ID		MATRIX_DRV_ID
#define DRV_NAME	MATRIX_DRV_NAME
#if defined MODULE
#define DRV_BANNER	MATRIX_BANNER
#else				/* defined MODULE */
#define DRV_BANNER	MATRIX_SPLASH
#endif				/* defined MODULE */

/*
 * PRIVATE STRUCTURES
 */

struct ch;
struct mx;
struct mg;

struct ma_state {
	int state;			/* state */
	uint32_t conn_rx;		/* bitmask of slots connected for RX */
	uint32_t conn_tx;		/* bitmask of slots connected for TX */
	int eventid;
};

struct ch {
	struct ch *next;
	struct ch **prev;
	queue_t *oq;
	queue_t *iq;
	struct ch *ch;
	struct mx *mx;
	struct mg *mg;
	int type;
	struct ma_state state, oldstate;
	/* uncommon fields */
	struct CH_info_ack info;
};

struct mx {
	struct mx *next;
	struct mx **prev;
	queue_t *oq;
	queue_t *iq;
	struct ch *ch;
	struct mx *mx;
	struct mg *mg;
	int type;
	struct ma_state state, oldstate;
	/* uncommon fields */
	struct MX_info_ack info;
};

struct mg {
	struct mg *next;
	struct mg **prev;
	queue_t *oq;
	queue_t *iq;
	struct ch *ch;
	struct mx *mx;
	struct mg *mg;
	int type;
	struct ma_state state, oldstate;
	/* uncommon fields */
};

union ma {
	union ma *next;
	struct {
		union ma *next;
		union ma **prev;
		queue_t *oq;
		queue_t *iq;
		struct ch *ch;
		struct mx *mx;
		struct mg *mg;
		int type;
		struct ma_state state, oldstate;
	} common;
	struct ch ch;
	struct mx mx;
};

#define MA_TYPE_MG	0	/* management type */
#define MA_TYPE_CH	1	/* channel type */
#define MA_TYPE_MX	2	/* multiplex type */

static inline const char *
ch_primname(const ch_ulong prim)
{
	switch (prim) {
	case CH_INFO_REQ:
		return ("CH_INFO_REQ");
	case CH_OPTMGMT_REQ:
		return ("CH_OPTMGMT_REQ");
	case CH_ATTACH_REQ:
		return ("CH_ATTACH_REQ");
	case CH_ENABLE_REQ:
		return ("CH_ENABLE_REQ");
	case CH_CONNECT_REQ:
		return ("CH_CONNECT_REQ");
	case CH_DATA_REQ:
		return ("CH_DATA_REQ");
	case CH_DISCONNECT_REQ:
		return ("CH_DISCONNECT_REQ");
	case CH_DISABLE_REQ:
		return ("CH_DISABLE_REQ");
	case CH_DETACH_REQ:
		return ("CH_DETACH_REQ");
	case CH_INFO_ACK:
		return ("CH_INFO_ACK");
	case CH_OPTMGMT_ACK:
		return ("CH_OPTMGMT_ACK");
	case CH_OK_ACK:
		return ("CH_OK_ACK");
	case CH_ERROR_ACK:
		return ("CH_ERROR_ACK");
	case CH_ENABLE_CON:
		return ("CH_ENABLE_CON");
	case CH_CONNECT_CON:
		return ("CH_CONNECT_CON");
	case CH_DATA_IND:
		return ("CH_DATA_IND");
	case CH_DISCONNECT_IND:
		return ("CH_DISCONNECT_IND");
	case CH_DISCONNECT_CON:
		return ("CH_DISCONNECT_CON");
	case CH_DISABLE_IND:
		return ("CH_DISABLE_IND");
	case CH_DISABLE_CON:
		return ("CH_DISABLE_CON");
	case CH_EVENT_IND:
		return ("CH_EVENT_IND");
	default:
		return ("(unknown)");
	}
}

static inline const char *
mx_primname(const mx_ulong prim)
{
	switch (prim) {
	case MX_INFO_REQ:
		return ("MX_INFO_REQ");
	case MX_OPTMGMT_REQ:
		return ("MX_OPTMGMT_REQ");
	case MX_ATTACH_REQ:
		return ("MX_ATTACH_REQ");
	case MX_ENABLE_REQ:
		return ("MX_ENABLE_REQ");
	case MX_CONNECT_REQ:
		return ("MX_CONNECT_REQ");
	case MX_DATA_REQ:
		return ("MX_DATA_REQ");
	case MX_DISCONNECT_REQ:
		return ("MX_DISCONNECT_REQ");
	case MX_DISABLE_REQ:
		return ("MX_DISABLE_REQ");
	case MX_DETACH_REQ:
		return ("MX_DETACH_REQ");
	case MX_INFO_ACK:
		return ("MX_INFO_ACK");
	case MX_OPTMGMT_ACK:
		return ("MX_OPTMGMT_ACK");
	case MX_OK_ACK:
		return ("MX_OK_ACK");
	case MX_ERROR_ACK:
		return ("MX_ERROR_ACK");
	case MX_ENABLE_CON:
		return ("MX_ENABLE_CON");
	case MX_CONNECT_CON:
		return ("MX_CONNECT_CON");
	case MX_DATA_IND:
		return ("MX_DATA_IND");
	case MX_DISCONNECT_IND:
		return ("MX_DISCONNECT_IND");
	case MX_DISCONNECT_CON:
		return ("MX_DISCONNECT_CON");
	case MX_DISABLE_IND:
		return ("MX_DISABLE_IND");
	case MX_DISABLE_CON:
		return ("MX_DISABLE_CON");
	case MX_EVENT_IND:
		return ("MX_EVENT_IND");
	default:
		return ("(unknown)");
	}
}

static inline const char *
ch_statename(const ch_ulong state)
{
	switch (state) {
	case CHS_UNINIT:
		return ("CHS_UNINIT");
	case CHS_UNUSABLE:
		return ("CHS_UNUSABLE");
	case CHS_DETACHED:
		return ("CHS_DETACHED");
	case CHS_WACK_AREQ:
		return ("CHS_WACK_AREQ");
	case CHS_WACK_UREQ:
		return ("CHS_WACK_UREQ");
	case CHS_ATTACHED:
		return ("CHS_ATTACHED");
	case CHS_WACK_EREQ:
		return ("CHS_WACK_EREQ");
	case CHS_WCON_EREQ:
		return ("CHS_WCON_EREQ");
	case CHS_WACK_RREQ:
		return ("CHS_WACK_RREQ");
	case CHS_WCON_RREQ:
		return ("CHS_WCON_RREQ");
	case CHS_ENABLED:
		return ("CHS_ENABLED");
	case CHS_WACK_CREQ:
		return ("CHS_WACK_CREQ");
	case CHS_WCON_CREQ:
		return ("CHS_WCON_CREQ");
	case CHS_WACK_DREQ:
		return ("CHS_WACK_DREQ");
	case CHS_WCON_DREQ:
		return ("CHS_WCON_DREQ");
	case CHS_CONNECTED:
		return ("CHS_CONNECTED");
	default:
		return ("(unknown)");
	}
}

static inline const char *
mx_statename(const mx_ulong state)
{
	switch (state) {
	case MXS_UNINIT:
		return ("MXS_UNINIT");
	case MXS_UNUSABLE:
		return ("MXS_UNUSABLE");
	case MXS_DETACHED:
		return ("MXS_DETACHED");
	case MXS_WACK_AREQ:
		return ("MXS_WACK_AREQ");
	case MXS_WACK_UREQ:
		return ("MXS_WACK_UREQ");
	case MXS_ATTACHED:
		return ("MXS_ATTACHED");
	case MXS_WACK_EREQ:
		return ("MXS_WACK_EREQ");
	case MXS_WCON_EREQ:
		return ("MXS_WCON_EREQ");
	case MXS_WACK_RREQ:
		return ("MXS_WACK_RREQ");
	case MXS_WCON_RREQ:
		return ("MXS_WCON_RREQ");
	case MXS_ENABLED:
		return ("MXS_ENABLED");
	case MXS_WACK_CREQ:
		return ("MXS_WACK_CREQ");
	case MXS_WCON_CREQ:
		return ("MXS_WCON_CREQ");
	case MXS_WACK_DREQ:
		return ("MXS_WACK_DREQ");
	case MXS_WCON_DREQ:
		return ("MXS_WCON_DREQ");
	case MXS_CONNECTED:
		return ("MXS_CONNECTED");
	default:
		return ("(unknown)");
	}
}

static const char *
ch_errname(const ch_long error)
{
	switch (error) {
	case CHSYSERR:
		return ("CHSYSERR");
	case CHBADADDR:
		return ("CHBADADDR");
	case CHOUTSTATE:
		return ("CHOUTSTATE");
	case CHBADOPT:
		return ("CHBADOPT");
	case CHBADPARM:
		return ("CHBADPARM");
	case CHBADPARMTYPE:
		return ("CHBADPARMTYPE");
	case CHBADFLAG:
		return ("CHBADFLAG");
	case CHBADPRIM:
		return ("CHBADPRIM");
	case CHNOTSUPP:
		return ("CHNOTSUPP");
	case CHBADSLOT:
		return ("CHBADSLOT");
	case -EMSGSIZE:
		return ("EMSGSIZE");
	case -EINVAL:
		return ("EINVAL");
	case -ENOSR:
		return ("ENOSR");
	case -EFAULT:
		return ("EFAULT");
	case -EPROTO:
		return ("EPROTO");
	case -ETIMEDOUT:
		return ("ETIMEDOUT");
	default:
		return ("(unknown)");
	}
}

static const char *
mx_errname(const mx_long error)
{
	switch (error) {
	case MXSYSERR:
		return ("MXSYSERR");
	case MXBADADDR:
		return ("MXBADADDR");
	case MXOUTSTATE:
		return ("MXOUTSTATE");
	case MXBADOPT:
		return ("MXBADOPT");
	case MXBADPARM:
		return ("MXBADPARM");
	case MXBADPARMTYPE:
		return ("MXBADPARMTYPE");
	case MXBADFLAG:
		return ("MXBADFLAG");
	case MXBADPRIM:
		return ("MXBADPRIM");
	case MXNOTSUPP:
		return ("MXNOTSUPP");
	case MXBADSLOT:
		return ("MXBADSLOT");
	case -EMSGSIZE:
		return ("EMSGSIZE");
	case -EINVAL:
		return ("EINVAL");
	case -ENOSR:
		return ("ENOSR");
	case -EFAULT:
		return ("EFAULT");
	case -EPROTO:
		return ("EPROTO");
	case -ETIMEDOUT:
		return ("ETIMEDOUT");
	default:
		return ("(unknown)");
	}
}

static const char *
ch_strerror(const ch_long error)
{
	switch (error) {
	case CHSYSERR:
		return ("UNIX system error");
	case CHBADADDR:
		return ("Bad address format or content");
	case CHOUTSTATE:
		return ("Interface out of state");
	case CHBADOPT:
		return ("Bad options format or content");
	case CHBADPARM:
		return ("Bad parameter format or content");
	case CHBADPARMTYPE:
		return ("Bad paramter structure type");
	case CHBADFLAG:
		return ("Bad flag");
	case CHBADPRIM:
		return ("Bad primitive");
	case CHNOTSUPP:
		return ("Primitive not supported");
	case CHBADSLOT:
		return ("Bad multiplex slot");
	case -EMSGSIZE:
		return ("Bad message size");
	case -EINVAL:
		return ("Invalid argument");
	case -ENOSR:
		return ("No STREAMS resource");
	case -EFAULT:
		return ("Segmentation fault");
	case -EPROTO:
		return ("Protocol error");
	case -ETIMEDOUT:
		return ("Operation timed out");
	default:
		return ("(unknown)");
	}
}

static const char *
mx_strerror(const mx_long error)
{
	switch (error) {
	case MXSYSERR:
		return ("UNIX system error");
	case MXBADADDR:
		return ("Bad address format or content");
	case MXOUTSTATE:
		return ("Interface out of state");
	case MXBADOPT:
		return ("Bad options format or content");
	case MXBADPARM:
		return ("Bad parameter format or content");
	case MXBADPARMTYPE:
		return ("Bad paramter structure type");
	case MXBADFLAG:
		return ("Bad flag");
	case MXBADPRIM:
		return ("Bad primitive");
	case MXNOTSUPP:
		return ("Primitive not supported");
	case MXBADSLOT:
		return ("Bad multiplex slot");
	case -EMSGSIZE:
		return ("Bad message size");
	case -EINVAL:
		return ("Invalid argument");
	case -ENOSR:
		return ("No STREAMS resource");
	case -EFAULT:
		return ("Segmentation fault");
	case -EPROTO:
		return ("Protocol error");
	case -ETIMEDOUT:
		return ("Operation timed out");
	default:
		return ("(unknown)");
	}
}

/*
 * STATE HANDLING
 */

static inline ch_ulong
ch_get_state(struct ch *ch)
{
	return ch->state.state;
}
static inline ch_ulong
ch_set_state(struct ch *ch, const ch_ulong newstate)
{
	const ch_ulong oldstate = ch_get_state(ch);

	if (newstate != oldstate) {
		ch->state.state = newstate;
		ch->info.CURRENT_state = newstate;
		mi_strlog(ch->iq, STRLOGST, SL_TRACE, "%s <- %s", ch_statename(newstate),
			  ch_statename(oldstate));
	}
	return (newstate);
}
static inline ch_ulong
ch_save_state(struct ch *ch)
{
	ch->oldstate = ch->state;
	return (ch->state.state);
}
static inline ch_ulong
ch_restore_state(struct ch *ch)
{
	ch->state = ch->oldstate;
	return ch_set_state(ch, ch->state.state);
}

static inline mx_ulong
mx_get_state(struct mx *mx)
{
	return mx->state.state;
}
static inline mx_ulong
mx_set_state(struct mx *mx, const mx_ulong newstate)
{
	const mx_ulong oldstate = mx_get_state(mx);

	if (newstate != oldstate) {
		mx->state.state = newstate;
		mx->info.CURRENT_state = newstate;
		mi_strlog(mx->iq, STRLOGST, SL_TRACE, "%s <- %s", mx_statename(newstate),
			  mx_statename(oldstate));
	}
	return (newstate);
}
static inline mx_ulong
mx_save_state(struct mx *mx)
{
	mx->oldstate = mx->state;
	return (mx->state.state);
}
static inline mx_ulong
mx_restore_state(struct mx *mx)
{
	mx->state = mx->oldstate;
	return mx_set_state(mx, mx->state.state);
}

/*
 * MESSAGE LOGGING
 */

static inline fastcall void
ma_stripmsg(mblk_t *msg, mblk_t *mp)
{
	mblk_t *b, *b_next = msg;

	while ((b = b_next) && b != mp->b_cont) {
		b_next = b->b_next;
		freeb(b);
	}
}

static inline fastcall void
ch_txprim(struct ch *ch, queue_t *q, mblk_t *msg, mblk_t *mp, const ch_ulong prim)
{
	switch (__builtin_expect(prim, CH_DATA_REQ)) {
	case CH_DATA_REQ:
	case CH_DATA_IND:
#ifdef _DEBUG
		mi_strlog(ch->oq, STRLOGDA, SL_TRACE, (ch->oq->q_flag & QREADR) ? "<- %s" : "%s ->",
			  ch_primname(prim));
#endif
		break;
	default:
		mi_strlog(ch->oq, STRLOGTX, SL_TRACE, (ch->oq->q_flag & QREADR) ? "<- %s" : "%s ->",
			  ch_primname(prim));
		break;
	}
	ma_stripmsg(msg, mp);
	putnext(ch->oq, mp);
	return;
}

static inline fastcall void
mx_txprim(struct mx *mx, queue_t *q, mblk_t *msg, mblk_t *mp, const mx_ulong prim)
{
	switch (__builtin_expect(prim, MX_DATA_REQ)) {
	case MX_DATA_REQ:
	case MX_DATA_IND:
#ifdef _DEBUG
		mi_strlog(mx->oq, STRLOGDA, SL_TRACE, (mx->oq->q_flag & QREADR) ? "<- %s" : "%s ->",
			  mx_primname(prim));
#endif
		break;
	default:
		mi_strlog(mx->oq, STRLOGTX, SL_TRACE, (mx->oq->q_flag & QREADR) ? "<- %s" : "%s ->",
			  mx_primname(prim));
		break;
	}
	ma_stripmsg(msg, mp);
	putnext(mx->oq, mp);
	return;
}

static inline fastcall void
ch_rxprim(struct ch *ch, queue_t *q, mblk_t *mp, const ch_ulong prim)
{
	switch (__builtin_expect(prim, CH_DATA_IND)) {
	case CH_DATA_REQ:
	case CH_DATA_IND:
#ifdef _DEBUG
		mi_strlog(q, STRLOGDA, SL_TRACE, (q->q_flag & QREADR) ? "%s <-" : "-> %s",
			  ch_primname(prim));
#endif
		break;
	default:
		mi_strlog(q, STRLOGRX, SL_TRACE, (q->q_flag & QREADR) ? "%s <-" : "-> %s",
			  ch_primname(prim));
		break;
	}
	return;
}

static inline fastcall void
mx_rxprim(struct mx *mx, queue_t *q, mblk_t *mp, const mx_ulong prim)
{
	switch (__builtin_expect(prim, MX_DATA_IND)) {
	case MX_DATA_REQ:
	case MX_DATA_IND:
#ifdef _DEBUG
		mi_strlog(q, STRLOGDA, SL_TRACE, (q->q_flag & QREADR) ? "%s <-" : "-> %s",
			  mx_primname(prim));
#endif
		break;
	default:
		mi_strlog(q, STRLOGRX, SL_TRACE, (q->q_flag & QREADR) ? "%s <-" : "-> %s",
			  mx_primname(prim));
		break;
	}
	return;
}

/*
 * CH ISSUED PRIMITIVES
 */

/**
 * ch_info_req: - issue CH_INFO_REQ primitive
 * @ch: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
noinline fastcall int
ch_info_req(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_info_req *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		if (likely(bcanputnext(ch->oq, 0))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->ch_primitive = CH_INFO_REQ;
			mp->b_wptr += sizeof(*p);
			ch_txprim(ch, q, msg, mp, CH_INFO_REQ);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * ch_optmgmt_req: - issue CH_OPTMGMT_REQ primitive
 * @ch: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @optr: options pointer
 * @olen: options length
 * @flags: management flags
 */
noinline fastcall int
ch_optmgmt_req(struct ch *ch, queue_t *q, mblk_t *msg, caddr_t optr, size_t olen, ch_ulong flags)
{
	struct CH_optmgmt_req *p;
	mblk_t *mp;
	const size_t mlen = sizeof(*p) + olen;

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		if (likely(bcanputnext(ch->oq, 0))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->ch_primitive = CH_OPTMGMT_REQ;
			p->ch_opt_length = olen;
			p->ch_opt_offset = olen ? sizeof(*p) : 0;
			p->ch_mgmt_flags = flags;
			mp->b_wptr += sizeof(*p);
			bcopy(optr, mp->b_wptr, p->ch_opt_length);
			mp->b_wptr += p->ch_opt_length;
			ch_txprim(ch, q, msg, mp, CH_OPTMGMT_REQ);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * ch_attach_req: - issue CH_ATTACH_REQ primitive
 * @ch: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @aptr: address pointer
 * @alen: address length
 * @flags: attach flags
 */
noinline fastcall int
ch_attach_req(struct ch *ch, queue_t *q, mblk_t *msg, caddr_t aptr, size_t alen, ch_ulong flags)
{
	struct CH_attach_req *p;
	mblk_t *mp;
	const size_t mlen = sizeof(*p) + alen;

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		if (likely(bcanputnext(ch->oq, 0))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->ch_primitive = CH_ATTACH_REQ;
			p->ch_addr_length = alen;
			p->ch_addr_offset = alen ? sizeof(*p) : 0;
			p->ch_flags = flags;
			mp->b_wptr += sizeof(*p);
			bcopy(aptr, mp->b_wptr, p->ch_addr_length);
			mp->b_wptr += p->ch_addr_length;
			ch_set_state(ch, CHS_WACK_AREQ);
			ch_txprim(ch, q, msg, mp, CH_ATTACH_REQ);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * ch_enable_req: - issue CH_ENABLE_REQ primitive
 * @ch: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
noinline fastcall int
ch_enable_req(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_enable_req *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		if (likely(bcanputnext(ch->oq, 0))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->ch_primitive = CH_ENABLE_REQ;
			mp->b_wptr += sizeof(*p);
			ch_set_state(ch, CHS_WACK_EREQ);
			ch_txprim(ch, q, msg, mp, CH_ENABLE_REQ);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * ch_connect_req: - issue CH_CONNECT_REQ primitive
 * @ch: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @flags: connect flags
 * @slot: multiplex slot
 */
noinline fastcall int
ch_connect_req(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong flags, ch_ulong slot)
{
	struct CH_connect_req *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		if (likely(bcanputnext(ch->oq, 0))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->ch_primitive = CH_CONNECT_REQ;
			p->ch_conn_flags = flags;
			p->ch_slot = slot;
			mp->b_wptr += sizeof(*p);
			ch_set_state(ch, CHS_WACK_CREQ);
			ch_txprim(ch, q, msg, mp, CH_CONNECT_REQ);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * ch_data_req: - issue CH_DATA_REQ primitive
 * @ch: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @slot: multiplex slot
 * @dp: user data
 */
noinline fastcall int
ch_data_req(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong slot, mblk_t *dp)
{
	struct CH_data_req *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		if (likely(bcanputnext(ch->oq, 0))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->ch_primitive = CH_DATA_REQ;
			p->ch_slot = slot;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			ch_txprim(ch, q, msg, mp, CH_DATA_REQ);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * ch_disconnect_req: - issue CH_DISCONNECT_REQ primitive
 * @ch: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @flags: connect flags
 * @slot: multiplex slot
 */
noinline fastcall int
ch_disconnect_req(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong flags, ch_ulong slot)
{
	struct CH_disconnect_req *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		if (likely(bcanputnext(ch->oq, 0))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->ch_primitive = CH_DISCONNECT_REQ;
			p->ch_conn_flags = flags;
			p->ch_slot = slot;
			mp->b_wptr += sizeof(*p);
			ch_set_state(ch, CHS_WACK_DREQ);
			ch_txprim(ch, q, msg, mp, CH_DISCONNECT_REQ);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * ch_disable_req: - issue CH_DISABLE_REQ primitive
 * @ch: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
noinline fastcall int
ch_disable_req(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_disable_req *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		if (likely(bcanputnext(ch->oq, 0))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->ch_primitive = CH_DISABLE_REQ;
			mp->b_wptr += sizeof(*p);
			ch_set_state(ch, CHS_WACK_RREQ);
			ch_txprim(ch, q, msg, mp, CH_DISABLE_REQ);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * ch_detach_req: - issue CH_DETACH_REQ primitive
 * @ch: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
noinline fastcall int
ch_detach_req(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_detach_req *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		if (likely(bcanputnext(ch->oq, 0))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->ch_primitive = CH_DETACH_REQ;
			mp->b_wptr += sizeof(*p);
			ch_set_state(ch, CHS_WACK_UREQ);
			ch_txprim(ch, q, msg, mp, CH_DETACH_REQ);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * ch_info_ack: - issue CH_INFO_ACK primitive
 * @ch: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @aptr: address pointer
 * @alen: address length
 * @pptr: parameters pointer
 * @plen: parameters length
 */
noinline fastcall int
ch_info_ack(struct ch *ch, queue_t *q, mblk_t *msg, caddr_t aptr, size_t alen, caddr_t pptr,
	    size_t plen)
{
	struct CH_info_ack *p;
	mblk_t *mp;
	const size_t mlen = sizeof(*p) + alen + plen;

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_INFO_ACK;
		p->ch_addr_length = alen;
		p->ch_addr_offset = alen ? sizeof(*p) : 0;
		p->ch_parm_length = plen;
		p->ch_parm_offset = plen ? p->ch_addr_offset + p->ch_addr_length : 0;
		p->ch_prov_flags = 0;
		p->ch_prov_class = CH_CIRCUIT;
		p->ch_style = CH_STYLE2;
		p->ch_version = CH_VERSION;
		p->ch_state = ch_get_state(ch);
		mp->b_wptr += sizeof(*p);
		bcopy(aptr, mp->b_wptr, p->ch_addr_length);
		mp->b_wptr += p->ch_addr_length;
		bcopy(pptr, mp->b_wptr, p->ch_parm_length);
		mp->b_wptr += p->ch_parm_length;
		ch_txprim(ch, q, msg, mp, CH_INFO_ACK);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_optmgmt_ack: - issue CH_OPTMGMT_ACK primitive
 * @ch: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @optr: options pointer
 * @olen: options length
 * @flags: management flags
 */
noinline fastcall int
ch_optmgmt_ack(struct ch *ch, queue_t *q, mblk_t *msg, caddr_t optr, size_t olen, ch_ulong flags)
{
	struct CH_optmgmt_ack *p;
	mblk_t *mp;
	const size_t mlen = sizeof(*p) + olen;

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_OPTMGMT_ACK;
		p->ch_opt_length = olen;
		p->ch_opt_offset = olen ? sizeof(*p) : 0;
		p->ch_mgmt_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(optr, mp->b_wptr, p->ch_opt_length);
		mp->b_wptr += p->ch_opt_length;
		ch_txprim(ch, q, msg, mp, CH_OPTMGMT_ACK);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_ok_ack: - issue CH_OK_ACK primitive
 * @ch: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @prim: correct primitive
 */
noinline fastcall int
ch_ok_ack(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong prim)
{
	struct CH_ok_ack *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);
	ch_ulong state;

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		switch ((state = ch_get_state(ch))) {
		case CHS_WACK_UREQ:
			state = CHS_DETACHED;
			break;
		case CHS_WACK_AREQ:
			state = CHS_ATTACHED;
			break;
		case CHS_WACK_RREQ:
			state = CHS_WCON_RREQ;
			break;
		case CHS_WACK_EREQ:
			state = CHS_WCON_EREQ;
			break;
		case CHS_WACK_DREQ:
			state = CHS_WCON_DREQ;
			break;
		case CHS_WACK_CREQ:
			state = CHS_WCON_CREQ;
			break;
		}
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_OK_ACK;
		p->ch_correct_prim = prim;
		p->ch_state = ch_set_state(ch, state);
		mp->b_wptr += sizeof(*p);
		ch_txprim(ch, q, msg, mp, CH_OK_ACK);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_error_ack: - issue CH_ERROR_ACK primitive
 * @ch: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @prim: primitive in error
 * @error: negative UNIX or positive CH error
 */
noinline fastcall int
ch_error_ack(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong prim, ch_long error)
{
	struct CH_error_ack *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);
	ch_ulong state;

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		switch ((state = ch_restore_state(ch))) {
		case CHS_WACK_AREQ:
			state = CHS_DETACHED;
			break;
		case CHS_WACK_UREQ:
		case CHS_WACK_EREQ:
			state = CHS_ATTACHED;
			break;
		case CHS_WACK_RREQ:
		case CHS_WACK_CREQ:
			state = CHS_ENABLED;
			break;
		case CHS_WACK_DREQ:
			state = CHS_CONNECTED;
			break;
		}
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_ERROR_ACK;
		p->ch_error_primitive = prim;
		p->ch_error_type = error < 0 ? CHSYSERR : error;
		p->ch_unix_error = error < 0 ? -error : 0;
		p->ch_state = ch_set - state(ch, state);
		mp->b_wptr += sizeof(*p);
		ch_txprim(ch, q, msg, mp, CH_ERROR_ACK);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_enable_con: - issue CH_ENABLE_CON primitive
 * @ch: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
noinline fastcall int
ch_enable_con(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_enable_con *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		if (likely(bcanputnext(ch->oq, 0))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->ch_primitive = CH_ENABLE_CON;
			mp->b_wptr += sizeof(*p);
			ch_set_state(ch, CHS_ENABLED);
			ch_txprim(ch, q, msg, mp, CH_ENABLE_CON);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * ch_connect_con: - issue CH_CONNECT_CON primitive
 * @ch: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @flags: connect flags
 * @slot: multiplex slot
 */
noinline fastcall int
ch_connect_con(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong flags, ch_ulong slot)
{
	struct CH_connect_con *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		if (likely(bcanputnext(ch->oq, 0))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->ch_primitive = CH_CONNECT_CON;
			p->ch_conn_flags = flags;
			p->ch_slot = slot;
			mp->b_wptr += sizeof(*p);
			if (flags & CHF_RX_DIR)
				ch->state.conn_rx |= slot;
			if (flags & CHF_TX_DIR)
				ch->state.conn_tx |= slot;
			ch_set_state(ch,
				     (ch->state.conn_rx | ch->state.
				      conn_tx) ? CHS_CONNECTED : CHS_ENABLED);
			ch_txprim(ch, q, msg, mp, CH_CONNECT_CON);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * ch_data_ind: - issue CH_DATA_IND primitive
 * @ch: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @slot: multiplex slot
 * @dp: user data
 */
noinline fastcall int
ch_data_ind(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong slot, mblk_t *dp)
{
	struct CH_data_ind *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		if (likely(bcanputnext(ch->oq, 0))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->ch_primitive = CH_DATA_IND;
			p->ch_slot = slot;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			ch_txprim(ch, q, msg, mp, CH_DATA_IND);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * ch_disconnect_ind: - issue CH_DISCONNECT_IND primitive
 * @ch: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @flags: connect flags
 * @cause: disconnect cause
 * @slot: multiplex slot
 */
noinline fastcall int
ch_disconnect_ind(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong flags, ch_long cause,
		  ch_ulong slot)
{
	struct CH_disconnect_ind *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		if (likely(bcanputnext(ch->oq, 0))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->ch_primitive = CH_DISCONNECT_IND;
			p->ch_conn_flags = flags;
			p->ch_cause = cause;
			p->ch_slot = slot;
			mp->b_wptr += sizeof(*p);
			if (flags & CHF_RX_DIR)
				ch->state.conn_rx &= ~slot;
			if (flags & CHF_TX_DIR)
				ch->state.conn_tx &= ~slot;
			ch_set_state(ch,
				     (ch->state.conn_rx | ch->state.
				      conn_tx) ? CHS_CONNECTED : CHS_ENABLED);
			ch_txprim(ch, q, msg, mp, CH_DISCONNECT_IND);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * ch_disconnect_con: - issue CH_DISCONNECT_CON primitive
 * @ch: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @flags: connect flags
 * @slot: multiplex slot
 */
noinline fastcall int
ch_disconnect_con(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong flags, ch_ulong slot)
{
	struct CH_disconnect_con *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		if (likely(bcanputnext(ch->oq, 0))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->ch_primitive = CH_DISCONNECT_CON;
			p->ch_conn_flags = flags;
			p->ch_slot = slot;
			mp->b_wptr += sizeof(*p);
			if (flags & CHF_RX_DIR)
				ch->state.conn_rx &= ~slot;
			if (flags & CHF_TX_DIR)
				ch->state.conn_tx &= ~slot;
			ch_set_state(ch,
				     (ch->state.conn_rx | ch->state.
				      conn_tx) ? CHS_CONNECTED : CHS_ENABLED);
			ch_txprim(ch, q, msg, mp, CH_DISCONNECT_CON);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * ch_disable_ind: - issue CH_DISABLE_IND primitive
 * @ch: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @cause: disable cause
 */
noinline fastcall int
ch_disable_ind(struct ch *ch, queue_t *q, mblk_t *msg, ch_long cause)
{
	struct CH_disable_ind *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		if (likely(bcanputnext(ch->oq, 0))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->ch_primitive = CH_DISABLE_IND;
			p->ch_cause = cause;
			mp->b_wptr += sizeof(*p);
			ch_set_state(ch, CHS_ATTACHED);
			ch_txprim(ch, q, msg, mp, CH_DISABLE_IND);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * ch_disable_con: - issue CH_DISABLE_CON primitive
 * @ch: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
noinline fastcall int
ch_disable_con(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_disable_con *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		if (likely(bcanputnext(ch->oq, 0))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->ch_primitive = CH_DISABLE_CON;
			mp->b_wptr += sizeof(*p);
			ch_set_state(ch, CHS_ATTACHED);
			ch_txprim(ch, q, msg, mp, CH_DISABLE_CON);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * ch_event_ind: - issue CH_EVENT_IND primitive
 * @ch: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @event: event reported
 * @slot: multiplex slot
 */
noinline fastcall int
ch_event_ind(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong event, ch_ulong slot)
{
	struct CH_event_ind *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		if (likely(bcanputnext(ch->oq, 0))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->ch_primitive = CH_EVENT_IND;
			p->ch_event = event;
			p->ch_slot = slot;
			mp->b_wptr += sizeof(*p);
			ch_txprim(ch, q, msg, mp, CH_EVENT_IND);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 * MX ISSUED PRIMITIVES
 */

/**
 * mx_info_req: - issue MX_INFO_REQ primitive
 * @mx: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
noinline fastcall int
mx_info_req(struct mx *mx, queue_t *q, mblk_t *msg)
{
	struct MX_info_req *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		if (likely(bcanputnext(mx->oq, 0))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mx_primitive = MX_INFO_REQ;
			mp->b_wptr += sizeof(*p);
			mx_txprim(mx, q, msg, mp, MX_INFO_REQ);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * mx_optmgmt_req: - issue MX_OPTMGMT_REQ primitive
 * @mx: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @optr: options pointer
 * @olen: options length
 * @flags: management flags
 */
noinline fastcall int
mx_optmgmt_req(struct mx *mx, queue_t *q, mblk_t *msg, caddr_t optr, size_t olen, mx_ulong flags)
{
	struct MX_optmgmt_req *p;
	mblk_t *mp;
	const size_t mlen = sizeof(*p) + olen;

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		if (likely(bcanputnext(mx->oq, 0))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mx_primitive = MX_OPTMGMT_REQ;
			p->mx_opt_length = olen;
			p->mx_opt_offset = olen ? sizeof(*p) : 0;
			p->mx_mgmt_flags = flags;
			mp->b_wptr += sizeof(*p);
			bcopy(optr, mp->b_wptr, p->mx_opt_length);
			mp->b_wptr += p->mx_opt_length;
			mx_txprim(mx, q, msg, mp, MX_OPTMGMT_REQ);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * mx_attach_req: - issue MX_ATTACH_REQ primitive
 * @mx: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @aptr: address pointer
 * @alen: address length
 * @flags: attach flags
 */
noinline fastcall int
mx_attach_req(struct mx *mx, queue_t *q, mblk_t *msg, caddr_t aptr, size_t alen, mx_ulong flags)
{
	struct MX_attach_req *p;
	mblk_t *mp;
	const size_t mlen = sizeof(*p) + alen;

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		if (likely(bcanputnext(mx->oq, 0))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mx_primitive = MX_ATTACH_REQ;
			p->mx_addr_length = alen;
			p->mx_addr_offset = alen ? sizeof(*p) : 0;
			p->mx_flags = flags;
			mp->b_wptr += sizeof(*p);
			bcopy(aptr, mp->b_wptr, p->mx_addr_length);
			mp->b_wptr += p->mx_addr_length;
			mx_set_state(mx, MXS_WACK_AREQ);
			mx_txprim(mx, q, msg, mp, MX_ATTACH_REQ);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * mx_enable_req: - issue MX_ENABLE_REQ primitive
 * @mx: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
noinline fastcall int
mx_enable_req(struct mx *mx, queue_t *q, mblk_t *msg)
{
	struct MX_enable_req *p;
	mblk_t *mp;
	const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		if (likely(bcanputnext(mx->oq, 0))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mx_primitive = MX_ENABLE_REQ;
			mp->b_wptr += sizeof(*p);
			mx_set_state(mx, MXS_WACK_EREQ);
			mx_txprim(mx, q, msg, mp, MX_ENABLE_REQ);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * mx_connect_req: - issue MX_CONNECT_REQ primitive
 * @mx: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @flags: connect flags
 * @slot: multiplex slot
 */
noinline fastcall int
mx_connect_req(struct mx *mx, queue_t *q, mblk_t *msg, mx_ulong flags, mx_ulong slot)
{
	struct MX_connect_req *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		if (likely(bcanputnext(mx->oq, 0))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mx_primitive = MX_CONNECT_REQ;
			p->mx_conn_flags = flags;
			p->mx_slot = slot;
			mp->b_wptr += sizeof(*p);
			mx_set_state(mx, MXS_WACK_CREQ);
			mx_txprim(mx, q, msg, mp, MX_CONNECT_REQ);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * mx_data_req: - issue MX_DATA_REQ primitive
 * @mx: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @slot: multiplex slot
 * @dp: user data
 */
noinline fastcall int
mx_data_req(struct mx *mx, queue_t *q, mblk_t *msg, ch_ulong slot, mblk_t *dp)
{
	struct MX_data_req *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		if (likely(bcanputnext(mx->oq, 0))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mx_primitive = MX_DATA_REQ;
			p->mx_slot = slot;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			mx_txprim(mx, q, msg, mp, MX_DATA_REQ);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * mx_disconnect_req: - issue MX_DISCONNECT_REQ primitive
 * @mx: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @flags: connect flags
 * @slot: multiplex slot
 */
noinline fastcall int
mx_disconnect_req(struct mx *mx, queue_t *q, mblk_t *msg, mx_ulong flags, mx_ulong slot)
{
	struct MX_disconnect_req *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		if (likely(bcanputnext(mx->oq, 0))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mx_primitive = MX_DISCONNECT_REQ;
			p->mx_conn_flags = flags;
			p->mx_slot = slot;
			mp->b_wptr += sizeof(*p);
			mx_set_state(mx, MXS_WACK_DREQ);
			mx_txprim(mx, q, msg, mp, MX_DISCONNECT_REQ);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * mx_disable_req: - issue MX_DISABLE_REQ primitive
 * @mx: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
noinline fastcall int
mx_disable_req(struct mx *mx, queue_t *q, mblk_t *msg)
{
	struct MX_disable_req *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		if (likely(bcanputnext(mx->oq, 0))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mx_primitive = MX_DISABLE_REQ;
			mp->b_wptr += sizeof(*p);
			mx_set_state(mx, MXS_WACK_RREQ);
			mx_txprim(mx, q, msg, mp, MX_DISABLE_REQ);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * mx_detach_req: - issue MX_DETACH_REQ primitive
 * @mx: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
noinline fastcall int
mx_detach_req(struct mx *mx, queue_t *q, mblk_t *msg)
{
	struct MX_detach_req *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		if (likely(bcanputnext(mx->oq, 0))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mx_primitive = MX_DETACH_REQ;
			mp->b_wptr += sizeof(*p);
			mx_set_state(mx, MXS_WACK_UREQ);
			mx_txprim(mx, q, msg, mp, MX_DETACH_REQ);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * mx_info_ack: - issue MX_INFO_ACK primitive
 * @mx: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @aptr: address pointer
 * @alen: address length
 * @pptr: parameters pointer
 * @plen: parameters length
 */
noinline fastcall int
mx_info_ack(struct mx *mx, queue_t *q, mblk_t *msg, caddr_t aptr, size_t alen, caddr_t pptr,
	    size_t plen)
{
	struct MX_info_ack *p;
	mblk_t *mp;
	const size_t mlen = sizeof(*p) + alen + plen;

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_INFO_ACK;
		p->mx_addr_length = alen;
		p->mx_addr_offset = alen ? sizeof(*p) : 0;
		p->mx_parm_length = plen;
		p->mx_parm_offset = plen ? p->mx_addr_offset + p->mx_addr_length : 0;
		p->mx_prov_flags = 0;
		p->mx_prov_class = MX_CIRCUIT;
		p->mx_style = MX_STYLE2;
		p->mx_version = MX_VERSION;
		p->mx_state = mx_get_state(mx);
		mp->b_wptr += sizeof(*p);
		bcopy(aptr, mp->b_wptr, p->mx_addr_length);
		mp->b_wptr += p->mx_addr_length;
		bcopy(pptr, mp->b_wptr, p->mx_parm_length);
		mp->b_wptr += p->mx_parm_length;
		mx_txprim(mx, q, msg, mp, MX_INFO_ACK);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_optmgmt_ack: - issue MX_OPTMGMT_ACK primitive
 * @mx: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @optr: options pointer
 * @olen: options length
 * @flags: management flags
 */
noinline fastcall int
mx_optmgmt_ack(struct mx *mx, queue_t *q, mblk_t *msg, caddr_t optr, size_t olen, mx_ulong flags)
{
	struct MX_optmgmt_ack *p;
	mblk_t *mp;
	const size_t mlen = sizeof(*p) + olen;

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_OPTMGMT_ACK;
		p->mx_opt_length = olen;
		p->mx_opt_offset = olen ? sizeof(*p) : 0;
		p->mx_mgmt_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(optr, mp->b_wptr, p->mx_opt_length);
		mp->b_wptr += p->mx_opt_length;
		mx_txprim(mx, q, msg, mp, MX_OPTMGMT_ACK);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_ok_ack: - issue MX_OK_ACK primitive
 * @mx: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @prim: correct primitive
 */
noinline fastcall int
mx_ok_ack(struct mx *mx, queue_t *q, mblk_t *msg, mx_ulong prim)
{
	struct MX_ok_ack *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);
	mx_ulong state;

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		switch ((state = mx_get_state(ch))) {
		case MXS_WACK_UREQ:
			state = MXS_DETACHED;
			break;
		case MXS_WACK_AREQ:
			state = MXS_ATTACHED;
			break;
		case MXS_WACK_RREQ:
			state = MXS_WCON_RREQ;
			break;
		case MXS_WACK_EREQ:
			state = MXS_WCON_EREQ;
			break;
		case MXS_WACK_DREQ:
			state = MXS_WCON_DREQ;
			break;
		case MXS_WACK_CREQ:
			state = MXS_WCON_CREQ;
			break;
		}
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_OK_ACK;
		p->mx_correct_prim = prim;
		p->mx_state = mx_set_state(mx, state);
		mp->b_wptr += sizeof(*p);
		mx_txprim(mx, q, msg, mp, MX_OK_ACK);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_error_ack: - issue MX_ERROR_ACK primitive
 * @mx: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @prim: primitive in error
 * @error: negative UNIX or positive MX error
 */
noinline fastcall int
mx_error_ack(struct mx *mx, queue_t *q, mblk_t *msg, mx_ulong prim, mx_long error)
{
	struct MX_error_ack *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);
	mx_ulong state;

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		switch ((state = mx_restore_state(mx))) {
		case MXS_WACK_AREQ:
			state = MXS_DETACHED;
			break;
		case MXS_WACK_UREQ:
		case MXS_WACK_EREQ:
			state = MXS_ATTACHED;
			break;
		case MXS_WACK_RREQ:
		case MXS_WACK_CREQ:
			state = MXS_ENABLED;
			break;
		case MXS_WACK_DREQ:
			state = MXS_CONNECTED;
			break;
		}
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_ERROR_ACK;
		p->mx_error_primitive = prim;
		p->mx_error_type = error < 0 ? MXSYSERR : error;
		p->mx_unix_error = error < 0 ? -error : 0;
		p->mx_state = mx_set_state(mx, state);
		mp->b_wptr += sizeof(*p);
		mx_txprim(mx, q, msg, mp, MX_ERROR_ACK);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_enable_con: - issue MX_ENABLE_CON primitive
 * @mx: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
noinline fastcall int
mx_enable_con(struct mx *mx, queue_t *q, mblk_t *msg)
{
	struct MX_enable_con *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		if (likely(bcanputnext(mx->oq, 0))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mx_primitive = MX_ENABLE_CON;
			mp->b_wptr += sizeof(*p);
			mx_set_state(mx, MXS_ENABLED);
			mx_txprim(mx, q, msg, mp, MX_ENABLE_CON);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * mx_connect_con: - issue MX_CONNECT_CON primitive
 * @mx: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @flags: connect flags
 * @slot: multiplex slot
 */
noinline fastcall int
mx_connect_con(struct mx *mx, queue_t *q, mblk_t *msg, mx_ulong flags, mx_ulong slot)
{
	struct MX_connect_con *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		if (likely(bcanputnext(mx->oq, 0))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mx_primitive = MX_CONNECT_CON;
			p->mx_conn_flags = flags;
			p->mx_slot = slot;
			mp->b_wptr += sizeof(*p);
			if (flags & MXF_RX_DIR)
				mx->state.conn_rx |= slot;
			if (flags & MXF_TX_DIR)
				mx->state.conn_tx |= slot;
			mx_set_state(mx,
				     (mx->state.conn_rx | mx->state.
				      conn_tx) ? MXS_CONNECTED : MXS_ENABLED);
			mx_txprim(mx, q, msg, mp, MX_CONNECT_CON);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * mx_data_ind: - issue MX_DATA_IND primitive
 * @mx: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @slot: multiplex slot
 * @dp: user data
 */
noinline fastcall int
mx_data_ind(struct mx *mx, queue_t *q, mblk_t *msg, mx_ulong slot, mblk_t *dp)
{
	struct MX_data_ind *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		if (likely(bcanputnext(mx->oq, 0))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mx_primitive = MX_DATA_IND;
			p->mx_slot = slot;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			mx_txprim(mx, q, msg, mp, MX_DATA_IND);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * mx_disconnect_ind: - issue MX_DISCONNECT_IND primitive
 * @mx: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @flags: connect flags
 * @cause: disconnect cause
 * @slot: multiplex slot
 */
noinline fastcall int
mx_disconnect_ind(struct mx *mx, queue_t *q, mblk_t *msg, mx_ulong flags, mx_long cause,
		  mx_ulong slot)
{
	struct MX_disconnect_ind *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		if (likely(bcanputnext(mx->oq, 0))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mx_primitive = MX_DISCONNECT_IND;
			p->mx_conn_flags = flags;
			p->mx_cause = cause;
			p->mx_slot = slot;
			mp->b_wptr += sizeof(*p);
			if (flags & MXF_RX_DIR)
				mx->state.conn_rx &= ~slot;
			if (flags & MXF_TX_DIR)
				mx->state.conn_tx &= ~slot;
			mx_set_state(mx,
				     (mx->state.conn_rx | mx->state.
				      conn_tx) ? MXS_CONNECTED : MXS_ENABLED);
			mx_txprim(mx, q, msg, mp, MX_DISCONNECT_IND);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * mx_disconnect_con: - issue MX_DISCONNECT_CON primitive
 * @mx: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @flags: connect flags
 * @slot: multiplex slot
 */
noinline fastcall int
mx_disconnect_con(struct mx *mx, queue_t *q, mblk_t *msg, mx_ulong flags, mx_ulong slot)
{
	struct MX_disconnect_con *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		if (likely(bcanputnext(mx->oq, 0))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mx_primitive = MX_DISCONNECT_CON;
			p->mx_conn_flags = flags;
			p->mx_slot = slot;
			mp->b_wptr += sizeof(*p);
			if (flags & MXF_RX_DIR)
				mx->state.conn_rx &= ~slot;
			if (flags & MXF_TX_DIR)
				mx->state.conn_tx &= ~slot;
			mx_set_state(mx,
				     (mx->state.conn_rx | mx->state.
				      conn_tx) ? MXS_CONNECTED : MXS_ENABLED);
			mx_txprim(mx, q, msg, mp, MX_DISCONNECT_CON);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * mx_disable_ind: - issue MX_DISABLE_IND primitive
 * @mx: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @cause: disable cause
 */
noinline fastcall int
mx_disable_ind(struct mx *mx, queue_t *q, mblk_t *msg, mx_long cause)
{
	struct MX_disable_ind *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		if (likely(bcanputnext(mx->oq, 0))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mx_primitive = MX_DISABLE_IND;
			p->mx_cause = cause;
			mp->b_wptr += sizeof(*p);
			mx_set_state(mx, MXS_ATTACHED);
			mx_txprim(mx, q, msg, mp, MX_DISABLE_IND);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * mx_disable_con: - issue MX_DISABLE_CON primitive
 * @mx: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
noinline fastcall int
mx_disable_con(struct mx *mx, queue_t *q, mblk_t *msg)
{
	struct MX_disable_con *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		if (likely(bcanputnext(mx->oq, 0))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mx_primitive = MX_DISABLE_CON;
			mp->b_wptr += sizeof(*p);
			mx_set_state(mx, MXS_ATTACHED);
			mx_txprim(mx, q, msg, mp, MX_DISABLE_CON);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * mx_event_ind: - issue MX_EVENT_IND primitive
 * @mx: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @event: event reported
 * @slot: multiplex slot
 */
noinline fastcall int
mx_event_ind(struct mx *mx, queue_t *q, mblk_t *msg, mx_ulong event, mx_ulong slot)
{
	struct MX_event_ind *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		if (likely(bcanputnext(mx->oq, 0))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mx_primitive = MX_EVENT_IND;
			p->mx_event = event;
			p->mx_slot = slot;
			mp->b_wptr += sizeof(*p);
			mx_txprim(mx, q, msg, mp, MX_EVENT_IND);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 * CH USER TO CH PROVIDER RECEIVED PRIMITIVES
 */

/**
 * ch_w_info_req: - process CH_INFO_REQ primitive
 * @ch: private structure (locked)
 * @q: active queue
 * @mp: the primitive
 */
noinline fastcall int
ch_w_info_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_info_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = CHNOTSUPP;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * ch_w_optmgmt_req: - process CH_OPTMGMT_REQ primitive
 * @ch: private structure (locked)
 * @q: active queue
 * @mp: the primitive
 */
noinline fastcall int
ch_w_optmgmt_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_optmgmt_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = CHNOTSUPP;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * ch_w_attach_req: - process CH_ATTACH_REQ primitive
 * @ch: private structure (locked)
 * @q: active queue
 * @mp: the primitive
 *
 * Address specifications for CH streams are either 16-bit PPAS, or 32-bit PPAs or are a variable
 * length CLEI character string of greater than 4 bytes.  (If the actual CLEI string is less than 4
 * bytes long, the remaining bytes are filled with null characters.)  This procedure looks up the
 * lower Stream using the PPA address or CLEI string and associates the upper Stream with the lower
 * Stream by linking the upper Stream into the lower Stream's private structure's list of upper
 * private structures.
 *
 * Note that the lower stream may be either a CH Stream or an MX Stream.  When it is a CH Stream the
 * mapping of the upper to lower stream is trivial.  When it is an MX Stream, the PPA can specify
 * either the entire MX Stream, a fractional MX Stream (by specifying the primary channel of a
 * pre-configured fraction), or an individual channel (by specifying a specific channel).  When an
 * individual channel is specified, the CH_CONNECT_REQ can still request connection to multiple
 * channels for a multi-rate switched connection.
 */
noinline fastcall int
ch_w_attach_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_attach_req *p = (typeof(p)) mp->b_rptr;
	struct CH_ok_ack *r;
	mblk_t *rp = NULL;
	unsigned long flags;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	if (unlikely(ch->info.ch_style != CH_STYLE2))
		goto notsupp;
	if (unlikely(ch_get_state(ch) != CHS_DETACHED))
		goto outstate;
	if (unlikely(!MBLKIN(mp, p->ch_addr_offset, p->ch_addr_length)))
		goto badaddr;
	if (unlikely(p->ch_addr_length < sizeof(uint32_t) && p->ch_addr_length != sizeof(uint16_t)))
		goto badaddr;
	ch_set_state(ch, CHS_WACK_AREQ);
	if (unlikely(!(rp = mi_allocb(q, sizeof(*r), BPRI_MED))))
		goto enobufs;
	DB_TYPE(rp) = M_PCPROTO;
	r = (typeof(r)) rp->b_wptr;
	r->ch_primitive = CH_OK_ACK;
	r->ch_correct_prim = CH_ATTACH_REQ;
	r->ch_state = CHS_ATTACHED;
	rp->b_wptr += sizeof(*r);
	write_lock_irqsave(&ma_lock, flags);
	{
		union ma *ma;

		if (p->ch_addr_length == sizeof(uint16_t)) {
			uint16_t addr = 0;

			/* address is a 16-bit PPA */
			bcopy(mp->b_rptr + p->ch_addr_length, &addr, sizeof(addr));
			if (unlikely(!(ma = ma_find_addr(addr))))
				goto unlock_badaddr;
		} else if (p->ch_addr_length == sizeof(uint32_t)) {
			uint32_t addr = 0;

			/* address is a 32-bit PPA */
			bcopy(mp->b_rptr + p->ch_addr_length, &addr, sizeof(addr));
			if (unlikely(!(ma = ma_find_addr(addr))))
				goto unlock_badaddr;
		} else {
			caddr_t addr = (caddr_t) (mp->b_rptr + p->ch_addr_offset);

			/* address is a CLEI string */
			if (unlikely(!(ma = ma_find_clei(addr, strnlen(addr, p->ch_addr_length)))))
				goto unlock_badaddr;
		}
		switch (ma->common.type) {
		case MA_TYPE_CH:
		{
			struct ch *bot = &ma->ch;

			if ((ch->next = bot->ch))
				ch->next->prev = &ch->next;
			ch->prev = &bot->ch;
			ch->ch = bot;
			bot->ch = ch;
		}
		case MA_TYPE_MX:
		{
			struct mx *bot = &ma->ch;

			if ((ch->next = bot->ch))
				ch->next->prev = &ch->next;
			ch->prev = &bot->ch;
			ch->mx = bot;
			bot->ch = ch;
		}
		default:
			goto unlock_badaddr;
		}
	}
	write_unlock_irqrestore(&ma_lock, flags);
	ch_set_state(ch, CHS_ATTACHED);
	ch_txprim(ch, q, mp, rp, CH_OK_ACK);
	return (0);
      unlock_badaddr:
	write_unlock_irqrestore(&ma_lock, flags);
	goto badaddr;
      enobufs:
	err = -ENOBUFS;
	goto error;
      badaddr:
	err = CHBADADDR;
	goto error;
      outstate:
	err = CHOUTSTATE;
	goto error;
      notsupp:
	err = CHNOTSUPP;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	freemsg(rp);
	return (err);
}

/**
 * ch_w_enable_req: - process CH_ENABLE_REQ primitive
 * @ch: private structure (locked)
 * @q: active queue
 * @mp: the primitive
 * 
 * If the lower stream is enabled (CHS_ENABLED or better), increment its number of
 * enabled upper streams and simply acknowledge and confirm the enable.  If the
 * lower stream is enabling (CHS_WACK_EREQ), put the message back and wait for our
 * queue to be enabled on the acknowledgement.  If the lower Stream has not been
 * enabled, issue an enable request and put the message back and wait for our queue
 * to be enabled on the acknowledgement.
 */
noinline fastcall int
ch_w_enable_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_enable_req *p = (typeof(p)) mp->b_rptr;
	struct CH_ok_ack *r;
	struct CH_enable_con *c;
	mblk_t *rp = NULL, *cp = NULL;
	union ma *ma;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	if (unlikely(ch_get_state(ch) != CHS_ATTACHED))
		goto outstate;
	ch_set_state(ch, CHS_WACK_EREQ);
	if (unlikely(!(rp = mi_allocb(q, sizeof(*r), BPRI_MED))))
		goto enobufs;
	if (unlikely(!(cp = mi_allocb(q, sizeof(*c), BPRI_MED))))
		goto enobufs;
	DB_TYPE(rp) = M_PCPROTO;
	r = (typeof(r)) rp->b_wptr;
	r->ch_primitive = CH_OK_ACK;
	r->ch_correct_prim = CH_ENABLE_REQ;
	r->ch_state = CHS_WCON_EREQ;
	rp->b_wptr += sizeof(*r);
	DB_TYPE(cp) = M_PROTO;
	c = (typeof(c)) cp->b_wptr;
	c->ch_primitive = CH_ENABLE_CON;
	cp->b_wptr += sizeof(*c);
	read_lock(&ma_lock);
	if ((ma = (typeof(ma)) ch->ch) || (ma = (typeof(ma)) ch->mx)) {
		if (likely(!!(ma = (typeof(ma)) mi_acquire((caddr_t) ma, q)))) {
			switch (ch_get_state(&ma->ch)) {
			case CHS_UNINIT:
			case CHS_UNUSABLE:
			case CHS_DETACHED:
			case CHS_WACK_AREQ:
			case CHS_WACK_UREQ:
				err = -EFAULT;
				break;
			case CHS_ATTACHED:
				/* issue an enable request to the lower stream and wait for
				   acknowledgement and confirmation */
				freemsg(XCHG(&rp, NULL));
				freemsg(XCHG(&cp, NULL));
				if (ma->common.type == MA_TYPE_CH)
					err = ch_enable_req(&ma->ch, q, mp);
				else if (ma->common.type == MA_TYPE_MX)
					err = mx_enable_req(&ma->mx, q, mp);
				else
					err = -EFAULT;
				break;
			case CHS_WACK_EREQ:
			case CHS_WCON_EREQ:
			case CHS_WACK_RREQ:
			case CHS_WCON_RREQ:
				/* simply put the message back and wait for state of lower stream
				   to settle. */
				err = -EAGAIN;
				break;
			case CHS_ENABLED:
			case CHS_WACK_CREQ:
			case CHS_WCON_CREQ:
			case CHS_WACK_DREQ:
			case CHS_WCON_DREQ:
			case CHS_CONNECTED:
				/* lower stream is already enabled, acknowledge and confirm the
				   enable */
				ma->common.enabled++;
				ch_txprim(ch, q, mp, rp, CH_OK_ACK);
				ch_txprim(ch, q, NULL, cp, CH_ENABLE_CON);
				err = 0;
				break;
			}
			mi_release((caddr_t) ma);
		} else
			err = -EDEADLK;
	} else
		err = -EFAULT;
	read_unlock(&ma_lock);
	return (err);
      enobufs:
	err = -ENOBUFS;
	goto error;
      outstate:
	err = CHOUTSTATE;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	freemsg(rp);
	freemsg(cp);
	return (err);
}

/**
 * ch_w_connect_req: - process CH_CONNECT_REQ primitive
 * @ch: private structure (locked)
 * @q: active queue
 * @mp: the primitive
 */
noinline fastcall int
ch_w_connect_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_connect_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	if (unlikely(ch_get_state(ch) != CHS_ENABLED))
		goto outstate;
	goto notsupp;
      notsupp:
	err = CHNOTSUPP;
	goto error;
      outstate:
	err = CHOUTSTATE;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * ch_w_data_req: - process CH_DATA_REQ primitive
 * @ch: private structure (locked)
 * @q: active queue
 * @mp: the primitive
 */
noinline fastcall int
ch_w_data_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_data_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = CHNOTSUPP;
	goto error;
      outstate:
	err = CHOUTSTATE;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * ch_w_disconnect_req: - process CH_DISCONNECT_REQ primitive
 * @ch: private structure (locked)
 * @q: active queue
 * @mp: the primitive
 */
noinline fastcall int
ch_w_disconnect_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_disconnect_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	if (unlikely(ch_get_state(ch) != CHS_CONNECTED))
		goto outstate;
	goto notsupp;
      notsupp:
	err = CHNOTSUPP;
	goto error;
      outstate:
	err = CHOUTSTATE;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * ch_w_disable_req: - process CH_DISABLE_REQ primitive
 * @ch: private structure (locked)
 * @q: active queue
 * @mp: the primitive
 * 
 * If the lower streams has other enabled upper streams, decrement the number of
 * enabled streams and simply acknowledge and confirm the disable.  If the lower
 * stream is in a transitent state, put the message back and wait for the state to
 * stabilize.  If this is the last enabled upper stream, disable the lower stream
 * and await acknowledgement.
 */
noinline fastcall int
ch_w_disable_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_disable_req *p = (typeof(p)) mp->b_rptr;
	struct CH_ok_ack *r;
	struct CH_disable_con *c;
	mblk_t *rp = NULL, *cp = NULL;
	union ma *ma;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	if (unlikely(ch_get_state(ch) != CHS_ENABLED))
		goto outstate;
	ch_set_state(ch, CHS_WACK_RREQ);
	if (unlikely(!(rp = mi_allocb(q, sizeof(*r), BPRI_MED))))
		goto enobufs;
	if (unlikely(!(cp = mi_allocb(q, sizeof(*c), BPRI_MED))))
		goto enobufs;
	DB_TYPE(rp) = M_PCPROTO;
	r = (typeof(r)) rp->b_wptr;
	r->ch_primitive = CH_OK_ACK;
	r->ch_correct_prim = CH_DISABLE_REQ;
	r->ch_state = CHS_WCON_RREQ;
	rp->b_wptr += sizeof(*r);
	DB_TYPE(cp) = M_PROTO;
	c = (typeof(c)) cp->b_wptr;
	c->ch_primitive = CH_DISABLE_CON;
	cp->b_wptr += sizeof(*c);
	read_lock(&ma_lock);
	if ((ma = (typeof(ma)) ch->ch) || (ma = (typeof(ma)) ch->mx)) {
		if (likely(!!(ma = (typeof(ma)) mi_acquire((caddr_t) ma, q)))) {
			switch (ch_get_state(&ma->ch)) {
			case CHS_ENABLED:
				if (ma->common.enabled <= 1) {
					/* issue a disable request to the lower stream and wait for
					   an acknowledgement and confirmation.  Note that the state
					   is now CHS_WACK_RREQ. */
					freemsg(XCHG(&rp, NULL));
					freemsg(XCHG(&cp, NULL));
					if (ma->common.type == MA_TYPE_CH)
						err = ch_disable_req(&ma->ch, q, mp);
					else if (ma->common.type = MA_TYPE_MX)
						err = mx_disable_req(&ma->mx, q, mp);
					else
						err = -EFAULT;
					break;
				}
				/* fall through */
			case CHS_WACK_CREQ:
			case CHS_WCON_CREQ:
			case CHS_WACK_DREQ:
			case CHS_WCON_DREQ:
			case CHS_CONNECTED:
				/* lower stream is still enabled, acknowledge and confirm the
				   disable. */
				--ma->common.enabled;
				ch_txprim(ch, q, mp, rp, CH_OK_ACK);
				ch_txprim(ch, q, NULL, cp, CH_DISABLE_CON);
				err = 0;
				break;
			default:
				err = -EFAULT;
				break;
			}
			mi_release((caddr_t) ma);
		} else
			err = -EDEADLK;
	} else
		err = -EFAULT;
	read_unlock(&ma_lock);
	return (err);
      enobufs:
	err = -ENOBUFS;
	goto error;
      outstate:
	err = CHOUTSTATE;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	freemsg(rp);
	freemsg(cp);
	return (err);
}

/**
 * ch_w_detach_req: - process CH_DETACH_REQ primitive
 * @ch: private structure (locked)
 * @q: active queue
 * @mp: the primitive
 *
 * Simply detach this private structure from whatever lower Stream it was associated with.
 */
noinline fastcall int
ch_w_detach_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_detach_req *p = (typeof(p)) mp->b_rptr;
	struct CH_ok_ack *r;
	mblk_t *rp = NULL;
	unsigned long flags;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	if (unlikely(ch->info.ch_style != CH_STYLE2))
		goto notsupp;
	if (unlikely(ch_get_state(ch) != CHS_ATTACHED))
		goto outstate;
	ch_set_state(ch, CHS_WACK_UREQ);
	if (unlikely(!(rp = mi_allocb(q, sizeof(*r), BPRI_MED))))
		goto enobufs;
	DB_TYPE(rp) = M_PCPROTO;
	r = (typeof(r)) rp->b_wptr;
	r->ch_primitive = CH_OK_ACK;
	r->ch_correct_prim = CH_DETACH_REQ;
	r->ch_state = CHS_DETACHED;
	rp->b_wptr += sizeof(*r);
	write_lock_irqsave(&ma_lock, flags);
	if ((*(ch->prev) = ch->next))
		ch->next->prev = ch->prev;
	ch->next = NULL;
	ch->prev = &ch->next;
	write_unlock_irqrestore(&ma_lock, flags);
	ch_set_state(ch, CHS_DETACHED);
	ch_txprim(ch, q, mp, rp, CH_OK_ACK);
	return (0);
      enobufs:
	err = -ENOBUFS;
	goto error;
      outstate:
	err = CHOUTSTATE;
	goto error;
      notsupp:
	err = CHNOTSUPP;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	freemsg(rp);
	return (err);
}

/**
 * ch_w_other: - process other CH primitive
 * @ch: private structure (locked)
 * @q: active queue
 * @mp: the primitive
 */
noinline fastcall int
ch_w_other(struct ch *ch, queue_t *q, mblk_t *mp)
{
	ch_ulong *p = (typeof(p)) mp->b_rptr;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = CHNOTSUPP;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/*
 * MX USER TO MX PROVIDER RECEIVED PRIMITIVES
 */

/**
 * mx_w_info_req: - process MX_INFO_REQ primitive
 * @mx: private structure (locked)
 * @q: active queue
 * @mp: the primitive
 */
noinline fastcall int
mx_w_info_req(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_info_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = MXNOTSUPP;
	goto error;
      outstate:
	err = MXOUTSTATE;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * mx_w_optmgmt_req: - process MX_OPTMGMT_REQ primitive
 * @mx: private structure (locked)
 * @q: active queue
 * @mp: the primitive
 */
noinline fastcall int
mx_w_optmgmt_req(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_optmgmt_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = MXNOTSUPP;
	goto error;
      outstate:
	err = MXOUTSTATE;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * mx_w_attach_req: - process MX_ATTACH_REQ primitive
 * @mx: private structure (locked)
 * @q: active queue (upper write queue)
 * @mp: the primitive
 *
 * Address specifications for MX streams are either 16-bit PPAS, or 32-bit PPAs or are a variable
 * length CLEI character string of greater than 4 bytes.  (If the actual CLEI string is less than 4
 * bytes long, the remaining bytes are filled with null characters.)  This procedure looks up the
 * lower Stream using the PPA address or CLEI string and associates the upper Stream with the lower
 * Stream by linking the upper Stream into the lower Stream's private structure's list of upper
 * private structures.
 */
noinline fastcall int
mx_w_attach_req(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_attach_req *p = (typeof(p)) mp->b_rptr;
	struct MX_ok_ack *r;
	mblk_t *rp = NULL;
	unsigned long flags;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	if (unlikely(mx->info.mx_style != MX_STYLE2))
		goto notsupp;
	if (unlikely(mx_get_state(mx) != MXS_DETACHED))
		goto outstate;
	if (unlikely(!MBLKIN(mp, p->mx_addr_offset, p->mx_addr_length)))
		goto badaddr;
	if (unlikely(p->mx_addr_length < sizeof(uint32_t) && p->mx_addr_length != sizeof(uint16_t)))
		goto badaddr;
	mx_set_state(mx, MXS_WACK_AREQ);
	if (unlikely(!(rp = mi_allocb(q, sizeof(*r), BPRI_MED))))
		goto enobufs;
	DB_TYPE(rp) = M_PCPROTO;
	r = (typeof(r)) rp->b_wptr;
	r->mx_primitive = MX_OK_ACK;
	r->mx_correct_prim = MX_ATTACH_REQ;
	r->mx_state = MXS_ATTACHED;
	rp->b_wptr += sizeof(*r);
	write_lock_irqsave(&ma_lock, flags);
	{
		union ma *ma;

		if (p->mx_addr_length == sizeof(uint16_t)) {
			uint16_t addr = 0;

			/* address is a 16-bit PPA */
			bcopy(mp->b_rptr + p->mx_addr_length, &addr, sizeof(addr));
			if (unlikely(!(ma = ma_find_addr(addr))))
				goto unlock_badaddr;
		} else if (p->mx_addr_length == sizeof(uint32_t)) {
			uint32_t addr = 0;

			/* address is a 32-bit PPA */
			bcopy(mp->b_rptr + p->mx_addr_length, &addr, sizeof(addr));
			if (unlikely(!(ma = ma_find_addr(addr))))
				goto unlock_badaddr;
		} else {
			caddr_t addr = (caddr_t) (mp->b_rptr + p->mx_addr_offset);

			/* address is a CLEI string */
			if (unlikely(!(ma = ma_find_clei(addr, strnlen(addr, p->mx_addr_length)))))
				goto unlock_badaddr;
		}
		switch (ma->common.type) {
		case MA_TYPE_MX:
		{
			struct mx *bot = &ma->mx;

			if ((mx->next = bot->mx))
				mx->next->prev = &mx->next;
			mx->prev = &bot->mx;
			mx->mx = bot;
			bot->mx = mx;
			break;
		}
		case MA_TYPE_CH:
			/* note that an MX upper stream may not attach to a CH lower stream */
			goto unlock_badaddr;
		default:
			goto unlock_badaddr;
		}
	}
	write_unlock_irqrestore(&ma_lock, flags);
	mx_set_state(mx, MXS_ATTACHED);
	mx_txprim(mx, q, mp, rp, MX_OK_ACK);
	return (0);
      unlock_badaddr:
	write_unlock_irqrestore(&ma_lock, flags);
	goto badaddr;
      enobufs:
	err = -ENOBUFS;
	goto error;
      badaddr:
	err = MXBADADDR;
	goto error;
      outstate:
	err = MXOUTSTATE;
	goto error;
      notsupp:
	err = MXNOTSUPP;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	freemsg(rp);
	return (err);
}

/**
 * mx_w_enable_req: - process MX_ENABLE_REQ primitive
 * @mx: private structure (locked)
 * @q: active queue
 * @mp: the primitive
 *
 * If the lower stream is enabled (MXS_ENABLED or better), increment its number of
 * enabled upper streams and simply acknoweldge and confirm the enable.  If the
 * lower stream is enabling (MXS_WACK_EREQ), put the message back and wait for our
 * queue to be enabled on the acknowledgement.  If the lower Stream has not been
 * enabled, issue an enable request and put the message back and wait for our
 * queue to be enabled on the acknolwedgement.
 */
noinline fastcall int
mx_w_enable_req(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_enable_req *p = (typeof(p)) mp->b_rptr;
	struct MX_ok_ack *r;
	struct MX_enable_con *c;
	mblk_t *rp = NULL, *cp = NULL;
	union ma *ma;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	if (unlikely(mx_get_state(mx) != MXS_ATTACHED))
		goto outstate;
	mx_set_state(mx, MXS_WACK_EREQ);
	if (unlikely(!(rp = mi_allocb(q, sizeof(*r), BPRI_MED))))
		goto enobufs;
	if (unlikely(!(cp = mi_allocb(q, sizeof(*c), BPRI_MED))))
		goto enobufs;
	DB_TYPE(rp) = M_PCPROTO;
	r = (typeof(r)) rp->b_wptr;
	r->mx_primitive = MX_OK_ACK;
	r->mx_correct_prim = MX_ENABLE_REQ;
	r->mx_state = MXS_WCON_EREQ;
	rp->b_wptr += sizeof(*r);
	DB_TYPE(cp) = M_PROTO;
	c = (typeof(c)) cp->b_wptr;
	c->mx_primitive = MX_ENABLE_CON;
	cp->b_wptr += sizeof(*c);
	read_lock(&ma_lock);
	if ((ma = (typeof(ma)) mx->mx)) {
		if (likely(!!(ma = (typeof(ma)) mi_acquire((caddr_t) ma, q)))) {
			switch (mx_get_state(&ma->mx)) {
			case MXS_ATTACHED:
				/* issue an enable request to the lower stream and wait for
				   acknowledgement and confirmation */
				freemsg(XCHG(&rp, NULL));
				freemsg(XCHG(&cp, NULL));
				if (ma->common.type == MA_TYPE_MX)
					err = mx_enable_req(&ma->mx, q, mp);
				else
					err = -EFAULT;
				break;
			case MXS_WACK_EREQ:
			case MXS_WCON_EREQ:
			case MXS_WACK_RREQ:
			case MXS_WCON_RREQ:
				/* simply put the message back and wait for state of lower stream
				   to settle. */
				err = -EAGAIN;
				break;
			case MXS_ENABLED:
			case MXS_WACK_CREQ:
			case MXS_WCON_CREQ:
			case MXS_WACK_DREQ:
			case MXS_WCON_DREQ:
			case MXS_CONNECTED:
				/* lower stream is already enabled, acknowledge and confirm the
				   enable */
				ma->common.enabled++;
				mx_txprim(mx, q, mp, rp, MX_OK_ACK);
				mx_txprim(mx, q, NULL, cp, MX_ENABLE_CON);
				err = 0;
				break;
			default:
				err = -EFAULT;
				break;
			}
			mi_release((caddr_t) ma);
		} else
			err = -EDEADLK;
	} else
		err = -EFAULT;
	read_unlock(&ma_lock);
	return (err);
      enobufs:
	err = -ENOBUFS;
	goto error;
      outstate:
	err = MXOUTSTATE;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	freemsg(rp);
	freemsg(cp);
	return (err);
}

/**
 * mx_w_connect_req: - process MX_CONNECT_REQ primitive
 * @mx: private structure (locked)
 * @q: active queue
 * @mp: the primitive
 */
noinline fastcall int
mx_w_connect_req(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_connect_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	if (unlikely(mx_get_state(mx) != MXS_ENABLED))
		goto outstate;
	goto notsupp;
      notsupp:
	err = MXNOTSUPP;
	goto error;
      outstate:
	err = MXOUTSTATE;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * mx_w_data_req: - process MX_DATA_REQ primitive
 * @mx: private structure (locked)
 * @q: active queue
 * @mp: the primitive
 */
noinline fastcall int
mx_w_data_req(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_data_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = MXNOTSUPP;
	goto error;
      outstate:
	err = MXOUTSTATE;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * mx_w_disconnect_req: - process MX_DISCONNECT_REQ primitive
 * @mx: private structure (locked)
 * @q: active queue
 * @mp: the primitive
 */
noinline fastcall int
mx_w_disconnect_req(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_disconnect_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	if (unlikely(mx_get_state(mx) != MXS_CONNECTED))
		goto outstate;
	goto notsupp;
      notsupp:
	err = MXNOTSUPP;
	goto error;
      outstate:
	err = MXOUTSTATE;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * mx_w_disable_req: - process MX_DISABLE_REQ primitive
 * @mx: private structure (locked)
 * @q: active queue
 * @mp: the primitive
 * 
 * If the lower streams has other enabled upper streams, decrement the number of
 * enabled streams and simply acknowledge and confirm the disable.  If the lower
 * stream is in a transitent state, put the message back and wait for the state to
 * stabilize.  If this is the last enabled upper stream, disable the lower stream
 * and await acknowledgement.
 */
noinline fastcall int
mx_w_disable_req(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_disable_req *p = (typeof(p)) mp->b_rptr;
	struct MX_ok_ack *r;
	struct MX_disable_con *c;
	mblk_t *rp = NULL, *cp = NULL;
	union ma *ma;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	if (unlikely(mx_get_state(mx) != MXS_ENABLED))
		goto outstate;
	mx_set_state(mx, MXS_WACK_RREQ);
	if (unlikely(!(rp = mi_allocb(q, sizeof(*r), BPRI_MED))))
		goto enobufs;
	if (unlikely(!(cp = mi_allocb(q, sizeof(*c), BPRI_MED))))
		goto enobufs;
	DB_TYPE(rp) = M_PCPROTO;
	r = (typeof(r)) rp->b_wptr;
	r->mx_primitive = MX_OK_ACK;
	r->mx_correct_prim = MX_DISABLE_REQ;
	r->mx_state = MXS_WCON_RREQ;
	rp->b_wptr += sizeof(*r);
	DB_TYPE(cp) = M_PROTO;
	c = (typeof(c)) cp->b_wptr;
	c->mx_primitive = MX_DISABLE_CON;
	cp->b_wptr += sizeof(*c);
	read_lock(&ma_lock);
	if ((ma = (typeof(ma)) mx->mx)) {
		if (likely(!!(ma = (typeof(ma)) mi_acquire((caddr_t) ma, q)))) {
			switch (mx_get_state(&ma->mx)) {
			case MXS_ENABLED:
				if (ma->common.enabled <= 1) {
					/* issue a disable request to the lower stream and wait for
					   an acknowledgement and confirmation.  Note that the state
					   is now MXS_WACK_RREQ. */
					freemsg(XCHG(&rp, NULL));
					freemsg(XCHG(&cp, NULL));
					if (ma->common.type = MA_TYPE_MX)
						err = mx_disable_req(&ma->mx, q, mp);
					else
						err = -EFAULT;
					break;
				}
				/* fall through */
			case MXS_WACK_CREQ:
			case MXS_WCON_CREQ:
			case MXS_WACK_DREQ:
			case MXS_WCON_DREQ:
			case MXS_CONNECTED:
				/* lower stream is still enabled, acknowledge and confirm the
				   disable. */
				--ma->common.enabled;
				mx_txprim(mx, q, mp, rp, MX_OK_ACK);
				mx_txprim(mx, q, NULL, cp, MX_DISABLE_CON);
				err = 0;
				break;
			default:
				err = -EFAULT;
				break;
			}
			mi_release((caddr_t) ma);
		} else
			err = -EDEADLK;
	} else
		err = -EFAULT;
	read_unlock(&ma_lock);
	return (err);
      enobufs:
	err = -ENOBUFS;
	goto error;
      outstate:
	err = MXOUTSTATE;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	freemsg(rp);
	freemsg(cp);
	return (err);
}

/**
 * mx_w_detach_req: - process MX_DETACH_REQ primitive
 * @mx: private structure (locked)
 * @q: active queue
 * @mp: the primitive
 *
 * Simply detach this private structure from whatever lower Stream it was associated with.
 */
noinline fastcall int
mx_w_detach_req(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_detach_req *p = (typeof(p)) mp->b_rptr;
	struct MX_ok_ack *r;
	mblk_t *rp = NULL;
	unsigned long flags;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	if (unlikely(mx->info.mx_style != MX_STYLE2))
		goto notsupp;
	if (unlikely(mx_get_state(mx) != MXS_ATTACHED))
		goto outstate;
	mx_set_state(mx, MXS_WACK_UREQ);
	if (unlikely(!(rp = mi_allocb(q, sizeof(*r), BPRI_MED))))
		goto enobufs;
	DB_TYPE(rp) = M_PCPROTO;
	r = (typeof(r)) rp->b_wptr;
	r->mx_primitive = MX_OK_ACK;
	r->mx_correct_prim = MX_DETACH_REQ;
	r->mx_state = MXS_DETACHED;
	rp->b_wptr += sizeof(*r);
	write_lock_irqsave(&ma_lock, flags);
	if ((*(mx->prev) = mx->next))
		mx->next->prev = mx->prev;
	mx->next = NULL;
	mx->prev = &mx->next;
	write_unlock_irqrestore(&ma_lock, flags);
	mx_set_state(mx, MXS_DETACHED);
	mx_txprim(mx, q, mp, rp, MX_OK_ACK);
	return (0);
      enobufs:
	err = -ENOBUFS;
	goto error;
      outstate:
	err = MXOUTSTATE;
	goto error;
      notsupp:
	err = MXNOTSUPP;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	freemsg(rp);
	return (err);
}

/**
 * mx_w_other: - process other MX primitive
 * @mx: private structure (locked)
 * @q: active queue
 * @mp: the primitive
 */
noinline fastcall int
mx_w_other(struct mx *mx, queue_t *q, mblk_t *mp)
{
	mx_ulong *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = MXNOTSUPP;
	goto error;
      outstate:
	err = MXOUTSTATE;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/*
 * CH PROVIDER TO CH USER RECEIVED PRIMITIVES
 */

/**
 * ch_r_info_ack: - process CH_INFO_ACK primitive
 * @ch: private structure (locked)
 * @q: active queue
 * @mp: the primitive
 */
noinline fastcall int
ch_r_info_ack(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_info_ack *p = (typeof(p)) mp->b_rptr;

}

/**
 * ch_r_optmgmt_ack: - process CH_OPTMGMT_ACK primitive
 * @ch: private structure (locked)
 * @q: active queue
 * @mp: the primitive
 */
noinline fastcall int
ch_r_optmgmt_ack(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_optmgmt_ack *p = (typeof(p)) mp->b_rptr;

}

/**
 * ch_r_ok_ack: - process CH_OK_ACK primitive
 * @ch: private structure (locked)
 * @q: active queue
 * @mp: the primitive
 *
 * If any of the associated upper streams are in the WACK state (after locking),
 * issue the appropriate acknolwedgement primitive upstream.  When the
 * acknolwedgement primitive results in a stable state, enable each of the write
 * queues for associated upper streams so that they can now act upon the stable
 * state.
 */
noinline fastcall int
ch_r_ok_ack(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_ok_ack *p = (typeof(p)) mp->b_rptr;
	ch_ulong state;

	switch ((state = ch_get_state(ch))) {
	case CHS_WACK_AREQ:
		state = CHS_ATTACHED;
		break;
	case CHS_WACK_UREQ:
		state = CHS_DETACHED;
		break;
	case CHS_WACK_EREQ:
		state = CHS_WCON_EREQ;
		goto skip_wakeup;
	case CHS_WACK_RREQ:
		state = CHS_WCON_RREQ;
		goto skip_wakeup;
	case CHS_WACK_CREQ:
		state = CHS_WCON_CREQ;
		goto skip_wakeup;
	case CHS_WACK_DREQ:
		state = CHS_WCON_DREQ;
		goto skip_wakeup;
	default:
		goto skip_wakeup;
	}
	read_lock(&ma_lock);
	for (ma = (typeof(ma)) ch->ch; ma; ma = ma->common.next)
		qenable(ma->common.iq);
	read_unlock(&ma_lock);
      skip_wakeup:
	ch_set_state(ch, state);
	freemsg(mp);
	return (0);
}

/**
 * ch_r_error_ack: - process CH_ERROR_ACK primitive
 * @ch: private structure (locked)
 * @q: active queue
 * @mp: the primitive
 *
 * If any of the associated upper streams are a the WACK state (after locking),
 * issue the appropriate error acknolwedgement primitive upstream.  Because the
 * error acknowledgement always results in a stable, state, enable each of the
 * write queues for associated upper streams so that they can now act upon the
 * stable state.
 */
noinline fastcall int
ch_r_error_ack(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_error_ack *p = (typeof(p)) mp->b_rptr;
	ch_ulong state;

	switch ((state = ch_get_state(ch))) {
	case CHS_WACK_AREQ:
		state = CHS_DETACHED;
		break;
	case CHS_WACK_UREQ:
		state = CHS_ATTACHED;
		break;
	case CHS_WACK_EREQ:
		state = CHS_ATTACHED;
		break;
	case CHS_WACK_RREQ:
		state = CHS_ENABLED;
		break;
	case CHS_WACK_CREQ:
		state = CHS_ENABLED;
		break;
	case CHS_WACK_DREQ:
		state = CHS_CONNECTED;
		break;
	default:
		goto skip_wakup;
	}
	read_lock(&ma_lock);
	for (ma = (typeof(ma)) ch->ch; ma; ma = ma->common.next)
		qenable(ma->common.iq);
	read_unlock(&ma_lock);
      skip_wakeup:
	ch_set_state(ch, state);
	freemsg(mp);
	return (0);
}

/**
 * ch_r_enable_con: - process CH_ENABLE_CON primitive
 * @ch: private structure (locked)
 * @q: active queue
 * @mp: the primitive
 *
 * This results in a stable state, so enable all upper streams.
 */
noinline fastcall int
ch_r_enable_con(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_enable_con *p = (typeof(p)) mp->b_rptr;
	union ma *ma;

	read_lock(&ma_lock);
	for (ma = (typeof(ma)) ch->ch; ma; ma = ma->common.next)
		qenable(ma->common.iq);
	read_unlock(&ma_lock);
	ch_set_state(ch, CHS_ENABLED);
	freemsg(mp);
	return (0);
}

/**
 * ch_r_connect_con: - process CH_CONNECT_CON primitive
 * @ch: private structure (locked)
 * @q: active queue
 * @mp: the primitive
 *
 * This results in a stable state, so enable all upper streams.
 */
noinline fastcall int
ch_r_connect_con(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_connect_con *p = (typeof(p)) mp->b_rptr;
	union ma *ma;

	read_lock(&ma_lock);
	for (ma = (typeof(ma)) ch->ch; ma; ma = ma->common.next)
		qenable(ma->common.iq);
	read_unlock(&ma_lock);
	ch_set_state(ch, CHS_CONNECTED);
	if (p->ch_conn_flags & CHF_RX_DIR)
		ch->state.conn_rx |= p->ch_slot;
	if (p->ch_conn_flags & CHF_TX_DIR)
		ch->state.conn_tx |= p->ch_slot;
	ch_set_state(ch, (ch->state.conn_rx | ch->state.conn_tx) ? CHS_CONNECTED : CHS_ENABLED);
	freemsg(mp);
	return (0);
}

/**
 * ch_r_data_ind: - process CH_DATA_IND primitive
 * @ch: private structure (locked)
 * @q: active queue
 * @mp: the primitive
 */
noinline fastcall int
ch_r_data_ind(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_data_ind *p = (typeof(p)) mp->b_rptr;

}

/**
 * ch_r_disconnect_ind: - process CH_DISCONNECT_IND primitive
 * @ch: private structure (locked)
 * @q: active queue
 * @mp: the primitive
 *
 * This results in a stable state, so enable all upper streams.
 */
noinline fastcall int
ch_r_disconnect_ind(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_disconnect_ind *p = (typeof(p)) mp->b_rptr;
	union ma *ma;

	read_lock(&ma_lock);
	for (ma = (typeof(ma)) ch->ch; ma; ma = ma->common.next)
		qenable(ma->common.iq);
	read_unlock(&ma_lock);
	/* FIXME: more has to be done here */
	if (p->ch_conn_flags & CHF_RX_DIR)
		ch->state.conn_rx &= ~p->ch_slot;
	if (p->ch_conn_flags & CHF_TX_DIR)
		ch->state.conn_tx &= ~p->ch_slot;
	ch_set_state(ch, (ch->state.conn_rx | ch->state.conn_tx) ? CHS_CONNECTED : CHS_ENABLED);
	freemsg(mp);
	return (0);
}

/**
 * ch_r_disconnect_con: - process CH_DISCONNECT_CON primitive
 * @ch: private structure (locked)
 * @q: active queue
 * @mp: the primitive
 *
 * This results in a stable state, so enable all upper streams.
 */
noinline fastcall int
ch_r_disconnect_con(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_disconnect_con *p = (typeof(p)) mp->b_rptr;
	union ma *ma;

	read_lock(&ma_lock);
	for (ma = (typeof(ma)) ch->ch; ma; ma = ma->common.next)
		qenable(ma->common.iq);
	read_unlock(&ma_lock);
	/* FIXME: more has to be done here */
	if (p->ch_conn_flags & CHF_RX_DIR)
		ch->state.conn_rx &= ~p->ch_slot;
	if (p->ch_conn_flags & CHF_TX_DIR)
		ch->state.conn_tx &= ~p->ch_slot;
	ch_set_state(ch, (ch->state.conn_rx | ch->state.conn_tx) ? CHS_CONNECTED : CHS_ENABLED);
	freemsg(mp);
	return (0);
}

/**
 * ch_r_disable_ind: - process CH_DISABLE_IND primitive
 * @ch: private structure (locked)
 * @q: active queue
 * @mp: the primitive
 *
 * This results in a stable state, so enable all upper streams.
 */
noinline fastcall int
ch_r_disable_ind(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_disable_ind *p = (typeof(p)) mp->b_rptr;
	union ma *ma;

	read_lock(&ma_lock);
	for (ma = (typeof(ma)) ch->ch; ma; ma = ma->common.next)
		qenable(ma->common.iq);
	read_unlock(&ma_lock);
	ch_set_state(ch, CHS_ATTACHED);
	freemsg(mp);
	return (0);
}

/**
 * ch_r_disable_con: - process CH_DISABLE_CON primitive
 * @ch: private structure (locked)
 * @q: active queue
 * @mp: the primitive
 *
 * This results in a stable state, so enable all upper streams.
 */
noinline fastcall int
ch_r_disable_con(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_disable_con *p = (typeof(p)) mp->b_rptr;
	union ma *ma;

	read_lock(&ma_lock);
	for (ma = (typeof(ma)) ch->ch; ma; ma = ma->common.next)
		qenable(ma->common.iq);
	read_unlock(&ma_lock);
	ch_set_state(ch, CHS_ATTACHED);
	freemsg(mp);
	return (0);
}

/**
 * ch_r_event_ind: - process CH_EVENT_IND primitive
 * @ch: private structure (locked)
 * @q: active queue
 * @mp: the primitive
 *
 * Propagate the event indication to all upper streams interested in the event.
 * Keep track of the event ID so that if we enter with the same message
 * repeatedly, each upper stream is only given one indication.
 */
noinline fastcall int
ch_r_event_ind(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_event_ind *p = (typeof(p)) mp->b_rptr;
	union ma *ma;
	int err;

	ch->state.eventid++;
	read_lock(&ma_lock);
	for (ma = (typeof(ma)) ch->ch; ma; ma = ma->common.next) {
		if (!(ma = (typeof(ma)) mi_acquire((caddr_t) ma, q)))
			goto unlock_edeadlk;
		if (ch_get_state(&ma->ch) < CHS_ENABLED)
			goto next_ma;
		if (!(ma->common.state.conn_rx & p->ch_slot) &&
		    !(ma->common.state.conn_tx & p->ch_slot))
			if (p->ch_slot != 0)
				goto next_ma;
		if (ma->common.state.eventid == ch->state.eventid)
			goto next_ma;
		if ((err = ch_event_ind(&ma->ch, q, NULL, p->ch_event, p->ch_slot)))
			goto release_error;
		ma->common.state.eventid = ch->state.eventid;
	      next_ma:
		mi_release((caddr_t) ma);
	}
	read_unlock(&ma_lock);
	freemsg(mp);
	return (0);
      release_error:
	mi_release((caddr_t) ma);
	read_unlock(&ma_lock);
	return (err);
      unlock_edeadlk:
	read_unlock(&ma_lock);
	return (-EDEADLK);
}

/**
 * ch_r_other: - process other CH primitive
 * @ch: private structure (locked)
 * @q: active queue
 * @mp: the primitive
 */
noinline fastcall int
ch_r_other(struct ch *ch, queue_t *q, mblk_t *mp)
{
	ch_ulong *p = (typeof(p)) mp->b_rptr;

	/* FIXME: report error */
	freemsg(mp);
	return (0);
}

/*
 * MX PROVIDER TO MX USER RECEIVED PRIMITIVES
 */

/**
 * mx_r_info_ack: - process MX_INFO_ACK primitive
 * @mx: private structure (locked)
 * @q: active queue
 * @mp: the primitive
 */
noinline fastcall int
mx_r_info_ack(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_info_ack *p = (typeof(p)) mp->b_rptr;

}

/**
 * mx_r_optmgmt_ack: - process MX_OPTMGMT_ACK primitive
 * @mx: private structure (locked)
 * @q: active queue
 * @mp: the primitive
 */
noinline fastcall int
mx_r_optmgmt_ack(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_optmgmt_ack *p = (typeof(p)) mp->b_rptr;

}

/**
 * mx_r_ok_ack: - process MX_OK_ACK primitive
 * @mx: private structure (locked)
 * @q: active queue
 * @mp: the primitive
 *
 * If any of the associated upper streams are in the WACK state (after locking),
 * issue the appropriate acknolwedgement primitive upstream.  When the
 * acknolwedgement primitive results in a stable state, enable each of the write
 * queues for associated upper streams so that they can now act upon the stable
 * state.
 */
noinline fastcall int
mx_r_ok_ack(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_ok_ack *p = (typeof(p)) mp->b_rptr;
	mx_ulong state;
	union ma *ma;

	switch ((state = mx_get_state(mx))) {
	case MXS_WACK_AREQ:
		state = MXS_ATTACHED;
		break;
	case MXS_WACK_UREQ:
		state = MXS_DETACHED;
		break;
	case MXS_WACK_EREQ:
		state = MXS_WCON_EREQ;
		goto skip_wakeup;
	case MXS_WACK_RREQ:
		state = MXS_WCON_RREQ;
		goto skip_wakeup;
	case MXS_WACK_CREQ:
		state = MXS_WCON_CREQ;
		goto skip_wakeup;
	case MXS_WACK_DREQ:
		state = MXS_WCON_DREQ;
		goto skip_wakeup;
	default:
		goto skip_wakeup;
	}
	read_lock(&ma_lock);
	for (ma = (typeof(ma)) mx->ch; ma; ma = ma->common.next)
		qenable(ma->common.iq);
	for (ma = (typeof(ma)) mx->mx; ma; ma = ma->common.next)
		qenable(ma->common.iq);
	read_unlock(&ma_lock);
      skip_wakeup:
	mx_set_state(mx, state);
	freemsg(mp);
	return (0);
}

/**
 * mx_r_error_ack: - process MX_ERROR_ACK primitive
 * @mx: private structure (locked)
 * @q: active queue
 * @mp: the primitive
 *
 * If any of the associated upper streams are a the WACK state (after locking),
 * issue the appropriate error acknolwedgement primitive upstream.  Because the
 * error acknowledgement always results in a stable, state, enable each of the
 * write queues for associated upper streams so that they can now act upon the
 * stable state.
 */
noinline fastcall int
mx_r_error_ack(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_error_ack *p = (typeof(p)) mp->b_rptr;
	mx_ulong state;
	union ma *ma;

	switch ((state = mx_get_state(mx))) {
	case MXS_WACK_AREQ:
		state = MXS_DETACHED;
		break;
	case MXS_WACK_UREQ:
	case MXS_WACK_EREQ:
		state = MXS_ATTACHED;
		break;
	case MXS_WACK_RREQ:
	case MXS_WACK_CREQ:
		state = MXS_ENABLED;
		break;
	case MXS_WACK_DREQ:
		state = MXS_CONNECTED;
		break;
	default:
		goto skip_wakeup;
	}
	read_lock(&ma_lock);
	for (ma = (typeof(ma)) mx->ch; ma; ma = ma->common.next) {
	}
	for (ma = (typeof(ma)) mx->mx; ma; ma = ma->common.next) {
	}
	read_unlock(&ma_lock);
	mx_set_state(mx, state);
skip_wakeup:

}

/**
 * mx_r_enable_con: - process MX_ENABLE_CON primitive
 * @mx: private structure (locked)
 * @q: active queue
 * @mp: the primitive
 *
 * This results in a stable state, so enable all upper streams.
 */
noinline fastcall int
mx_r_enable_con(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_enable_con *p = (typeof(p)) mp->b_rptr;
	union ma *ma;

	read_lock(&ma_lock);
	for (ma = (typeof(ma)) mx->ch; ma; ma = ma->common.next)
		qenable(ma->common.iq);
	for (ma = (typeof(ma)) mx->mx; ma; ma = ma->common.next)
		qenable(ma->common.iq);
	read_unlock(&ma_lock);
	mx_set_state(mx, MXS_ENABLED);
	freemsg(mp);
	return (0);
}

/**
 * mx_r_connect_con: - process MX_CONNECT_CON primitive
 * @mx: private structure (locked)
 * @q: active queue
 * @mp: the primitive
 *
 * This results in a stable state, so enable all upper streams.
 */
noinline fastcall int
mx_r_connect_con(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_connect_con *p = (typeof(p)) mp->b_rptr;
	union ma *ma;

	read_lock(&ma_lock);
	for (ma = (typeof(ma)) mx->ch; ma; ma = ma->common.next)
		qenable(ma->common.iq);
	for (ma = (typeof(ma)) mx->mx; ma; ma = ma->common.next)
		qenable(ma->common.iq);
	read_unlock(&ma_lock);
	mx_set_state(mx, MXS_CONNECTED);
	freemsg(mp);
	return (0);
}

/**
 * mx_r_data_ind: - process MX_DATA_IND primitive
 * @mx: private structure (locked)
 * @q: active queue
 * @mp: the primitive
 */
noinline fastcall int
mx_r_data_ind(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_data_ind *p = (typeof(p)) mp->b_rptr;

}

/**
 * mx_r_disconnect_ind: - process MX_DISCONNECT_IND primitive
 * @mx: private structure (locked)
 * @q: active queue
 * @mp: the primitive
 *
 * This results in a stable state, so enable all upper streams.
 */
noinline fastcall int
mx_r_disconnect_ind(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_disconnect_ind *p = (typeof(p)) mp->b_rptr;
	union ma *ma;

	read_lock(&ma_lock);
	for (ma = (typeof(ma)) mx->ch; ma; ma = ma->common.next)
		qenable(ma->common.iq);
	for (ma = (typeof(ma)) mx->mx; ma; ma = ma->common.next)
		qenable(ma->common.iq);
	read_unlock(&ma_lock);
	/* FIXME: more to do here */
	if (p->mx_conn_flags & MXF_RX_DIR)
		mx->state.conn_rx &= ~p->mx_slot;
	if (p->mx_conn_flags & MXF_TX_DIR)
		mx->state.conn_tx &= ~p->mx_slot;
	mx_set_state(mx, (mx->state.conn_rx|mx->state.conn_tx) ? MXS_CONNECTED : MXS_ENABLED);
	freemsg(mp);
	return (0);
}

/**
 * mx_r_disconnect_con: - process MX_DISCONNCT_CON primitive
 * @mx: private structure (locked)
 * @q: active queue
 * @mp: the primitive
 *
 * This results in a stable state, so enable all upper streams.
 */
noinline fastcall int
mx_r_disconnect_con(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_disconnect_con *p = (typeof(p)) mp->b_rptr;
	union ma *ma;

	read_lock(&ma_lock);
	for (ma = (typeof(ma)) mx->ch; ma; ma = ma->common.next)
		qenable(ma->common.iq);
	for (ma = (typeof(ma)) mx->mx; ma; ma = ma->common.next)
		qenable(ma->common.iq);
	read_unlock(&ma_lock);
	/* FIXME: more to do here */
	if (p->mx_conn_flags & MXF_RX_DIR)
		mx->state.conn_rx &= ~p->mx_slot;
	if (p->mx_conn_flags & MXF_TX_DIR)
		mx->state.conn_tx &= ~p->mx_slot;
	mx_set_state(mx, (mx->state.conn_rx|mx->state.conn_tx) ? MXS_CONNECTED : MXS_ENABLED);
	freemsg(mp);
	return (0);
}

/**
 * mx_r_disable_ind: - process MX_DISABLE_IND primitive
 * @mx: private structure (locked)
 * @q: active queue
 * @mp: the primitive
 *
 * This results in a stable state, so enable all upper streams.
 */
noinline fastcall int
mx_r_disable_ind(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_disable_ind *p = (typeof(p)) mp->b_rptr;
	union ma *ma;
	mblk_t *mp;

	read_lock(&ma_lock);
	for (ma = (typeof(ma)) mx->ch; ma; ma = ma->common.next)
		qenable(ma->common.iq);
	for (ma = (typeof(ma)) mx->mx; ma; ma = ma->common.next)
		qenable(ma->common.iq);
	read_unlock(&ma_lock);
	mx_set_state(mx, MXS_ATTACHED);
	freemsg(mp);
	return (0);
}

/**
 * mx_r_disable_con: - process MX_DISABLE_CON primitive
 * @mx: private structure (locked)
 * @q: active queue
 * @mp: the primitive
 *
 * This results in a stable state, so enable all upper streams.
 */
noinline fastcall int
mx_r_disable_con(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_disable_con *p = (typeof(p)) mp->b_rptr;
	union ma *ma;

	read_lock(&ma_lock);
	for (ma = (typeof(ma)) mx->ch; ma; ma = ma->common.next)
		qenable(ma->common.iq);
	for (ma = (typeof(ma)) mx->mx; ma; ma = ma->common.next)
		qenable(ma->common.iq);
	read_unlock(&ma_lock);
	mx_set_state(mx, MXS_ATTACHED);
	freemsg(mp);
	return (0);
}

/**
 * mx_r_event_ind: - process MX_EVENT_IND primitive
 * @mx: private structure (locked)
 * @q: active queue
 * @mp: the primitive
 *
 * Propagate the event indication to all upper streams interested in the event.
 * Keep track of the event ID so that if we enter with the same message
 * repeatedly, each upper stream is only given one indication.
 */
noinline fastcall int
mx_r_event_ind(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_event_ind *p = (typeof(p)) mp->b_rptr;
	union ma *ma;
	int err;

	mx->state.eventid++;
	read_lock(&ma_lock);
	for (ma = (typeof(ma)) mx->ch; ma; ma = ma->connect.next) {
		if (!(ma = (typeof(ma)) mi_acquire((caddr_t) ma, q)))
			goto unlock_edeadlk;
		if (ch_get_state(&ma->ch) < CHS_ENABLED)
			goto next_ch;
		if (!(ma->common.state.conn_rx & p->mx_slot) &&
		    !(ma->common.state.conn_tx & p->mx_slot))
			if (p->mx_slot != 0)
				goto next_ch;
		if (ma->common.state.eventid == mx->state.eventid)
			goto next_ch;
		if ((err = ch_event_ind(&ma->ch, q, NULL, p->mx_event, p->mx_slot)))
			goto release_error;
		ma->common.state.eventid = mx->state.eventid;
	      next_ch:
		mi_release((caddr_t) ma);
	}
	for (ma = (typeof(ma)) mx->mx; ma; ma = ma->connect.next) {
		if (!(ma = (typeof(ma)) mi_acquire((caddr_t) ma, q)))
			goto unlock_edeadlk;
		if (mx_get_state(&ma->mx) < MXS_ENABLED)
			goto next_mx;
		if (!(ma->common.state.conn_rx & p->mx_slot) &&
		    !(ma->common.state.conn_tx & p->mx_slot))
			if (p->mx_slot != 0)
				goto next_mx;
		if (ma->common.state.eventid == mx->state.eventid)
			goto next_mx;
		if ((err = mx_event_ind(&ma->mx, q, NULL, p->mx_event, p->mx_slot)))
			goto release_error;
		ma->common.state.eventid = mx->state.eventid;
	      next_mx:
		mi_release((caddr_t) ma);
	}
	read_unlock(&ma_lock);
	freemsg(mp);
	return (0);
      release_error:
	mi_release((caddr_t) ma);
	read_unlock(&ma_lock);
	return (err);
      unlock_edeadlk:
	read_unlock(&ma_lock);
	return (-EDEADLK);
}

/**
 * mx_r_other: - process other MX primitive
 * @mx: private structure (locked)
 * @q: active queue
 * @mp: the primitive
 */
noinline fastcall int
mx_r_other(struct mx *mx, queue_t *q, mblk_t *mp)
{
	mx_ulong *p = (typeof(p)) mp->b_rptr;

	/* FIXME generate error log */
	freemsg(mp);
	return (0);
}

/*
 * M_DATA and M_HPDATA handling
 */

/**
 * m_w_data: - process M_(HP)DATA block
 * @q: active queue (upper write queue)
 * @mp: the M_(HP)DATA message block
 *
 * The upper write queue can be an MX or CH Stream.  M_DATA message blocks presented to the queue
 * represent sample data for the MX or CH.  The process for handling these messages is as follows:
 *
 * Read lock the multiplex.
 *
 * 1. Copy data to the current corresponding lower MX or CH Stream elastic buffer transmission
 *    block, incrementing the sample count and block count if necessary.
 *
 *    When there is no room in the transmit elastic buffer of the lower MX or CH Stream, the
 *    behavior is different for slip or delay sensitive Streams.  Delay sensitive upper Streams mark
 *    a transmit overrun and copy the data to the last transmit elastic buffer block.  Slip
 *    sensitive upper Streams mark themselves as flow controlled and return -EBUSY so that the
 *    buffer is placed back on the queue.  When transmit elastic buffer pressure subsides, the lower
 *    Stream procedure will enable the feeding upper streams that marked themselves for flow
 *    control.
 *
 * Read unlock the multiplex.
 *
 * 2. Release the buffer.
 */
static inline fastcall int
m_w_data(queue_t *q, mblk_t *mp)
{
}

/**
 * ma_w_data: - process M_(HP)DATA block
 * @ma: matrix private structure (locked)
 * @q: active queue (upper write queue)
 * @mp: the M_(HP)DATA message block
 */
static inline fastcall int
ma_w_data(union ma *ma, queue_t *q, mblk_t *mp)
{
}

/**
 * ma_r_data: - process M_(HP)DATA block
 * @ma: matrix private structure (locked)
 * @q: active queue (lower read queue)
 * @mp: the M_(HP)DATA message block
 *
 * The lower read can can either be an MX or a CH Stream.  When M_DATA message blocks are serviced
 * from the queue, they have already had software loopback and pseudo-digital cross-connect already
 * performed.  The only actions left are:
 *
 * 1. Duplicate the message block for each active and connected upper MX Stream (that has not
 *    already been marked with processing this block) and pass the message block to the Stream.
 *
 * 2. Copy the channels specified by each active upper CH Stream (that ahs not already been marked
 *    with processing this block) into a newly allocated message block and pass it to the Stream.
 */
static inline fastcall int
ma_r_data(union ma *ma, queue_t *q, mblk_t *mp)
{
}

/**
 * m_r_data: - process M_(HP)DATA block
 * @q: active queue (lower read queue)
 * @mp: the M_(HP)DATA message block
 * 
 * The lower read queue can either be an MX or a CH Stream.  For either MX or CH Stream, this read
 * put procedure might be invoked by the lower level driver at interrupt.  All timing and
 * synchronization is fed from the read queue put procedure.  Steps are as follows:
 *
 * Write lock the multiplex.
 *
 * 1. Perform software loopback within the same MX or CH.
 *
 * 2. Perform pseudo-digital cross-connection within the same MX or CH.  (This is the same as
 *    software loopback for CH.)
 *
 * 3. Release the transmit block to the driver with esballoc and qreply() unless flow controlled, in
 *    which case discard the transmit block.  Bump the transmit block (head) pointer.
 *
 * 4. Perform pseudo-digital cross-connect.  Take samples from the message block and copy them to
 *    outgoing transmission blocks in elastic buffers on lower CH or MX streams.
 *
 * Write unlock the multiplex.
 *
 * If there are latency sensitive CH or MX Streams that are fed by this lower stream:
 *
 * Read lock the multiplex.
 *
 * 5. Duplicate the message block to any latency sensitive upper MX Streams.  Mark the upper MX
 *    Stream as having processed this received MX or CH block by marking it with the sequence number
 *    associated with the receive block.  Skip slip sensitive upper MX Streams.
 *
 * 6. Demultiplex to the receive elastic buffer for the upper CH Stream.  Pass a freshly esballoc'ed
 *    message block for any latency sensitive upper CH Streams upstream.  Mark the stream as having
 *    processed this received MX or CH block by marking it with the sequence number associated with
 *    the receive block.  Skip slip sensitive upper CH Streams.
 *
 * Read unlock the multiplex.
 *
 * If there are slip sensitive CH or MX Streams that are fed by this lower stream, or if an
 * allocation failure occured on a latency sensitive upper MX Stream:
 *
 * 7. Place block on the queue for later servicing by the read service procedure.
 */
static inline fastcall int
m_r_data(queue_t *q, mblk_t *mp)
{
}

/*
 * M_PROTO and M_PCPROTO handling
 */
noinline fastcall int
ch_w_proto(struct ch *ch, queue_t *q, mblk_t *mp)
{
	ch_ulong prim;

	ch_save_state(ch);

	if (unlikely(!MBLKIN(mp, 0, sizeof(prim))))
		goto tooshort;
	prim = *(typeof(prim) *) mp->b_rptr;
	ch_rxprim(ch, q, mp, prim);
	switch (prim) {
	case CH_INFO_REQ:
		err = ch_w_info_req(ch, q, mp);
		break;
	case CH_OPTMGMT_REQ:
		err = ch_w_optmgmt_req(ch, q, mp);
		break;
	case CH_ATTACH_REQ:
		err = ch_w_attach_req(ch, q, mp);
		break;
	case CH_ENABLE_REQ:
		err = ch_w_enable_req(ch, q, mp);
		break;
	case CH_CONNECT_REQ:
		err = ch_w_connect_req(ch, q, mp);
		break;
	case CH_DATA_REQ:
		err = ch_w_data_req(ch, q, mp);
		break;
	case CH_DISCONNECT_REQ:
		err = ch_w_disconnect_req(ch, q, mp);
		break;
	case CH_DISABLE_REQ:
		err = ch_w_disable_req(ch, q, mp);
		break;
	case CH_DETACH_REQ:
		err = ch_w_detach_req(ch, q, mp);
		break;
	default:
		err = ch_w_other(ch, q, mp);
		break;
	}
	if (err)
		goto error;
	return (0);
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	ch_restore_state(ch);

	return ch_error_reply(ch, q, mp, prim, err);
}
noinline fastcall int
mx_w_proto(struct mx *mx, queue_t *q, mblk_t *mp)
{
	mx_ulong prim;

	mx_save_state(mx);

	if (unlikely(!MBLKIN(mp, 0, sizeof(prim))))
		goto tooshort;
	prim = *(typeof(prim) *) mp->b_rptr;
	mx_rxprim(mx, q, mp, prim);
	switch (prim) {
	case MX_INFO_REQ:
		err = mx_w_info_req(mx, q, mp);
		break;
	case MX_OPTMGMT_REQ:
		err = mx_w_optmgmt_req(mx, q, mp);
		break;
	case MX_ATTACH_REQ:
		err = mx_w_attach_req(mx, q, mp);
		break;
	case MX_ENABLE_REQ:
		err = mx_w_enable_req(mx, q, mp);
		break;
	case MX_CONNECT_REQ:
		err = mx_w_connect_req(mx, q, mp);
		break;
	case MX_DATA_REQ:
		err = mx_w_data_req(mx, q, mp);
		break;
	case MX_DISCONNECT_REQ:
		err = mx_w_disconnect_req(mx, q, mp);
		break;
	case MX_DISABLE_REQ:
		err = mx_w_disable_req(mx, q, mp);
		break;
	case MX_DETACH_REQ:
		err = mx_w_detach_req(mx, q, mp);
		break;
	default:
		err = mx_w_other(mx, q, mp);
		break;
	}
	if (err)
		goto error;
	return (0);
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	ch_restore_state(ch);

	return ch_error_reply(ch, q, mp, prim, err);
}
noinline fastcall int
ch_r_proto(struct ch *ch, queue_t *q, mblk_t *mp)
{
	ch_ulong prim;

	ch_save_state(ch);

	if (unlikely(!MBLKIN(mp, 0, sizeof(prim))))
		goto tooshort;
	prim = *(typeof(prim) *) mp->b_rptr;
	ch_rxprim(ch, q, mp, prim);
	switch (prim) {
	case CH_INFO_ACK:
		err = ch_r_info_ack(ch, q, mp);
		break;
	case CH_OPTMGMT_ACK:
		err = ch_r_optmgmt_ack(ch, q, mp);
		break;
	case CH_OK_ACK:
		err = ch_r_ok_ack(ch, q, mp);
		break;
	case CH_ERROR_ACK:
		err = ch_r_error_ack(ch, q, mp);
		break;
	case CH_ENABLE_CON:
		err = ch_r_enable_con(ch, q, mp);
		break;
	case CH_CONNECT_CON:
		err = ch_r_connect_con(ch, q, mp);
		break;
	case CH_DATA_IND:
		err = ch_r_data_ind(ch, q, mp);
		break;
	case CH_DISCONNECT_IND:
		err = ch_r_disconnect_ind(ch, q, mp);
		break;
	case CH_DISCONNECT_CON:
		err = ch_r_disconnect_con(ch, q, mp);
		break;
	case CH_DISABLE_IND:
		err = ch_r_disable_ind(ch, q, mp);
		break;
	case CH_DISABLE_CON:
		err = ch_r_disable_con(ch, q, mp);
		break;
	case CH_EVENT_IND:
		err = ch_r_event_ind(ch, q, mp);
		break;
	default:
		err = ch_r_other(ch, q, mp);
		break;
	}
	if (err)
		goto error;
	return (0);
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	ch_restore_state(ch);
	freemsg(mp);
	return (0);
}
noinline fastcall int
mx_r_proto(struct mx *mx, queue_t *q, mblk_t *mp)
{
	mx_ulong prim;

	mx_save_state(mx);

	if (unlikely(!MBLKIN(mp, 0, sizeof(prim))))
		goto tooshort;
	prim = *(typeof(prim) *) mp->b_rptr;
	mx_rxprim(mx, q, mp, prim);
	switch (prim) {
	case MX_INFO_ACK:
		err = mx_r_info_ack(mx, q, mp);
		break;
	case MX_OPTMGMT_ACK:
		err = mx_r_optmgmt_ack(mx, q, mp);
		break;
	case MX_OK_ACK:
		err = mx_r_ok_ack(mx, q, mp);
		break;
	case MX_ERROR_ACK:
		err = mx_r_error_ack(mx, q, mp);
		break;
	case MX_ENABLE_CON:
		err = mx_r_enable_con(mx, q, mp);
		break;
	case MX_CONNECT_CON:
		err = mx_r_connect_con(mx, q, mp);
		break;
	case MX_DATA_IND:
		err = mx_r_data_ind(mx, q, mp);
		break;
	case MX_DISCONNECT_IND:
		err = mx_r_disconnect_ind(mx, q, mp);
		break;
	case MX_DISCONNECT_CON:
		err = mx_r_disconnect_con(mx, q, mp);
		break;
	case MX_DISABLE_IND:
		err = mx_r_disable_ind(mx, q, mp);
		break;
	case MX_DISABLE_CON:
		err = mx_r_disable_con(mx, q, mp);
		break;
	case MX_EVENT_IND:
		err = mx_r_event_ind(mx, q, mp);
		break;
	default:
		err = mx_r_other(mx, q, mp);
		break;
	}
	if (err)
		goto error;
	return (0);
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	mx_restore_state(mx);
	freemsg(mp);
	return (0);
}
static inline fastcall int
ma_w_proto(union ma *ma, queue_t *q, mblk_t *mp)
{
	switch (ma->common.type) {
	case MA_TYPE_CH:
		return ch_w_proto(&ma->ch, q, mp);
	case MA_TYPE_MX:
		return mx_w_proto(&ma->mx, q, mp);
	default:
		freemsg(mp);
		return (0);
	}
}
static inline fastcall int
ma_r_proto(union ma *ma, queue_t *q, mblk_t *mp)
{
	switch (ma->common.type) {
	case MA_TYPE_CH:
		return ch_r_proto(&ma->ch, q, mp);
	case MA_TYPE_MX:
		return mx_r_proto(&ma->mx, q, mp);
	default:
		freemsg(mp);
		return (0);
	}
}

#if CH_DATA_REQ != MX_DATA_REQ
#error Code assumes CH_DATA_REQ and MX_DATA_REQ are the same value, but they are not.
#endif

noinline fastcall int
m_w_proto_slow(queue_t *q, mblk_t *mp)
{
	union ma *ma;
	int err;

	if (likely(!!(ma = (typeof(ma)) mi_trylock(q)))) {
		err = ma_w_proto(ma, q, mp);
		mi_unlock((caddr_t) ma);
	} else
		err = -EDEADLK;
	return (err);
}
static inline fastcall int
m_w_proto(queue_t *q, mblk_t *mp)
{
	lmi_ulong prim;
	int err;

	if (likely(MBLKIN(mp, 0, sizeof(prim)))) {
		if (likely((prim = *(typeof(prim) *) mp->b_rptr) == CH_DATA_REQ)) {
			if ((err = m_w_data(q, mp->b_cont)) == 0)
				freeb(mp);
			return (err);
		}
	}
	return m_w_proto_slow(q, mp);
}

#if CH_DATA_IND != MX_DATA_IND
#error Code assumes CH_DATA_IND and MX_DATA_IND are the same value, but they are not.
#endif

noinline fastcall int
m_r_proto_slow(queue_t *q, mblk_t *mp)
{
	union ma *ma;
	int err;

	if (likely(!!(ma = (typeof(ma)) mi_trylock(q)))) {
		err = ma_r_proto(ma, q, mp);
		mi_unlock((caddr_t) ma);
	} else
		err = -EDEADLK;
	return (err);
}
static inline fastcall int
m_r_proto(queue_t *q, mblk_t *mp)
{
	lmi_ulong prim;
	int err;

	if (likely(MBLKIN(mp, 0, sizeof(prim)))) {
		if (likely((prim = *(typeof(prim) *) mp->b_rptr) == CH_DATA_IND)) {
			if ((err = m_r_data(q, mp->b_cont)) == 0)
				freeb(mp);
			return (err);
		}
	}
	return m_r_proto_slow(q, mp);
}

/*
 * M_CTL and M_PCCTL handling
 */
noinline fastcall int
ma_w_ctl(union ma *ma, queue_t *q, mblk_t *mp)
{
	/* Not defined just yet. */
	freemsg(mp);
	return (0);
}
noinline fastcall int
ma_r_ctl(union ma *ma, queue_t *q, mblk_t *mp)
{
	/* Not defined just yet. */
	freemsg(mp);
	return (0);
}

/*
 * M_IOCTL and M_IOCDATA handling
 */
noinline fastcall int
ma_w_ioctl(union ma *ma, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;

	switch (ioc->ioc_cmd) {
	case I_LINK:
		return ma_i_link(ma, q, mp, ioc);
	case I_PLINK:
		return ma_i_plink(ma, q, mp, ioc);
	case I_UNLINK:
		return ma_i_unlink(ma, q, mp, ioc);
	case I_PUNLINK:
		return ma_i_punlink(ma, q, mp, ioc);
	default:
		switch (ma->common.type) {
		case MA_TYPE_CH:
			return ch_w_ioctl(&ma->ch, q, mp, ioc);
		case MX_TYPE_MX:
			return mx_w_ioctl(&ma->mx, q, mp, ioc);
		case MX_TYPE_MG:
			return mg_w_ioctl(&ma->mg, q, mp, ioc);
		default:
			mi_copy_done(q, mp, -EINVAL);
			return (0);
		}
	}
}
noinline fastcall int
ma_r_ioctl(union ma *ma, queue_t *q, mblk_t *mp)
{
	union ma *ctrl;

	read_lock(&ma_lock);
	if ((ctrl = ma->common.ioc)) {
		switch (DB_TYPE(mp)) {
		case M_IOCACK:
		case M_IOCNAK:
			ma->common.ioc = NULL;
			break;
		}
		putnext(ctrl->oq, mp);
	} else
		freemsg(mp);
	read_unlock(&ma_lock);
	return (0);
}

/*
 * M_FLUSH handling
 */
noinline fastcall int
m_w_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_ptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		mp->b_rptr[0] &= ~FLUSHW;
	}
	if (mp->b_rptr[0] & FLUSHR) {
		read_lock(&ma_lock);
		if (ma->common.ch) {
			putnext(ma->common.ch->oq, mp);
			read_unlock(&ma_lock);
			return (0);
		}
		if (ma->common.mx) {
			putnext(ma->common.mx->oq, mp);
			read_unlock(&ma_lock);
			return (0);
		}
		read_unlock(&ma_lock);
		qreply(q, mp);
		return (0);
	}
	freemsg(mp);
	return (0);
}
noinline fastcall int
m_r_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		mp->b_rptr[0] &= ~FLUSHR;
	}
	if (mp->b_rptr[0] & FLUSHW) {
		read_lock(&ma_lock);
		if (ma->common.ch || ma->common.mx) {
			struct ch *ch;
			struct mx *mx;

			for (ch = ma->common.ch; ch; ch = ch->next)
				putctl2(ch->oq, M_FLUSH, mp->b_rptr[0], mp->b_rptr[1]);
			for (mx = ma->common.mx; mx; mx = mx->next)
				putctl2(mx->oq, M_FLUSH, mp->b_rptr[0], mp->b_rptr[1]);
			read_unlock(&ma_lock);
			freemsg(mp);
			return (0);
		}
		read_unlock(&ma_lock);
		if (!(mp->b_band & MSGNOLOOP)) {
			mp->b_band |= MSGNOLOOP;
			qreply(q, mp);
			return (0);
		}
	}
	freemsg(mp);
	return (0);
}

/*
 * M_ERROR and M_HANGUP handling
 */
noinline fastcall int
m_r_error(queue_t *q, mblk_t *mp)
{
	/* FIXME */
	freemsg(mp);
	return (0);
}

/*
 * STREAMS MESSAGE HANDLING
 */

static inline fastcall int
m_wput_msg(queue_t *q, mblk_t *mp)
{
	unsigned char type;
	int err;

	switch ((type = DB_TYPE(mp))) {
	case M_DATA:
		err = m_w_data(q, mp);
		break;
	case M_PROTO:
		err = m_w_proto(q, mp);
		break;
	case M_CTL:
	case M_IOCTL:
		err = -EAGAIN;
		break;
	case M_HPDATA:
	case M_PCPROTO:
	case M_PCCTL:
	case M_IOCDATA:
	{
		union ma *ma;

		if (likely(!!(ma = (typeof(ma)) mi_trylock(q)))) {
			switch (type) {
			case M_HPDATA:
				err = ma_w_data(ma, q, mp);
				break;
			case M_PCPROTO:
				err = ma_w_proto(ma, q, mp);
				break;
			case M_PCCTL:
				err = ma_w_ctl(ma, q, mp);
				break;
			case M_IOCDATA:
				err = ma_w_ioctl(ma, q, mp);
				break;
			}
			mi_unlock((caddr_t) ma);
		} else
			err = -EDEADLK;
		break;
	}
	case M_FLUSH:
		err = m_w_flush(q, mp);
		break;
	default:
		err = m_w_other(q, mp);
		break;
	}
	return (err);
}

static inline fastcall int
ma_wsrv_msg(union ma *ma, queue_t *q, mblk_t *mp)
{
	int err;

	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		err = ma_w_data(ma, q, mp);
		break;
	case M_PROTO:
	case M_PCPROTO:
		err = ma_w_proto(ma, q, mp);
		break;
	case M_CTL:
	case M_PCCTL:
		err = ma_w_ctl(ma, q, mp);
		break;
	case M_IOCTL:
	case M_IOCDATA:
		err = ma_w_ioctl(ma, q, mp);
		break;
	case M_FLUSH:
		/* this should in fact never happen as these are processed by the put procedure */
		err = m_w_flush(q, mp);
		break;
	default:
		/* this should in fact never happen as these are processed by the put procedure */
		err = m_w_other(q, mp);
		break;
	}
	return (err);
}

static inline fastcall int
ma_rsrv_msg(union ma *ma, queue_t *q, mblk_t *mp)
{
	int err;

	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		err = ma_r_data(ma, q, mp);
		break;
	case M_PROTO:
	case M_PCPROTO:
		err = ma_r_proto(ma, q, mp);
		break;
	case M_CTL:
	case M_PCCTL:
		err = ma_r_ctl(ma, q, mp);
		break;
	case M_IOCACK:
	case M_IOCNAK:
	case M_COPYIN:
	case M_COPYOUT:
		err = ma_r_ioctl(ma, q, mp);
		break;
	case M_FLUSH:
		/* this should in fact never happen as these are processed by the put procedure */
		err = m_r_flush(q, mp);
		break;
	case M_ERROR:
	case M_HANGUP:
		/* this should in fact never happen as these are processed by the put procedure */
		err = m_r_error(q, mp);
		break;
	default:
		/* this should in fact never happen as these are processed by the put procedure */
		err = m_r_other(q, mp);
		break;
	}
	return (err);
}

static inline fastcall int
m_rput_msg(queue_t *q, mblk_t *mp)
{
	unsigned char type;
	int err;

	switch ((type = DB_TYPE(mp))) {
	case M_DATA:
		err = m_r_data(q, mp);
		break;
	case M_PROTO:
		err = m_r_proto(q, mp);
		break;
	case M_CTL:
		err = -EAGAIN;
		break;
	case M_HPDATA:
	case M_PCPROTO:
	case M_PCCTL:
	case M_IOCACK:
	case M_IOCNAK:
	case M_COPYIN:
	case M_COPYOUT:
	{
		union ma *ma;

		if (!likely(!!(ma = (typeof(ma)) mi_trylock(q)))) {
			switch (type) {
			case M_HPDATA:
				err = ma_r_data(ma, q, mp);
				break;
			case M_PCPROTO:
				err = ma_r_proto(ma, q, mp);
				break;
			case M_PCCTL:
				err = ma_r_ctl(ma, q, mp);
				break;
			case M_IOCACK:
			case M_IOCNAK:
			case M_COPYIN:
			case M_COPYOUT:
				err = ma_r_ioctl(ma, q, mp);
				break;
			}
			mi_unlock((caddr_t) ma);
		} else
			err = -EDEADLK;
		break;
	}
	case M_FLUSH:
		err = m_r_flush(q, mp);
		break;
	case M_ERROR:
	case M_HANGUP:
		err = m_r_error(q, mp);
		break;
	default:
		err = m_r_other(q, mp);
		break;
	}
	return (err);
}

/*
 * QUEUE PUT AND SERVICE PROCEDURES
 */

/**
 * ma_wput: - matrix upper write put procedure
 * @q: active queue (upper write queue)
 * @mp: the message to put
 *
 * The upper write queue can be an MX or CH Stream.  M_DATA message blocks presented to the queue
 * represent sample data for the MX or CH.  The process for handling these messages is as follows:
 *
 * Read lock the multiplex.
 *
 * 1. Copy data to the current corresponding lower MX or CH Stream elastic buffer transmission
 *    block, incrementing the sample count and block count if necessary.
 *
 * Read unlock the multiplex.
 *
 * 2. Release the buffer.
 */
static streamscall int
ma_wput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || m_wput_msg(q, mp)) {
		if (unlikely(!putq(q, mp))) {
			mp->b_band = 0;
			putq(q, mp);
		}
	}
	return (0);
}

/**
 * ma_wsrv: - matrix upper write service procedure
 * @q: active queue (upper write queue)
 */
static streamscall int
ma_wsrv(queue_t *q)
{
	union ma *ma;
	mblk_t *mp;

	if (likely(!!(ma = (typeof(ma)) mi_trylock(q)))) {
		while (likely(!!(mp = getq(q)))) {
			if (unlikely(ma_wsrv_msg(ma, q, mp))) {
				if (unlikely(!putbq(q, mp))) {
					mp->b_band = 0;
					putbq(q, mp);
				}
				break;
			}
		}
		mi_unlock((caddr_t) ma);
	}
	return (0);
}

static streamscall int
ma_muxrsrv(queue_t *q)
{
}

static streamscall int
ma_muxwsrv(queue_t *q)
{
}

/**
 * ma_rsrv: - matrix lower read service procedure
 * @q: active queue (lower read queue)
 *
 * The lower read can can either be an MX or a CH Stream.  When M_DATA message blocks are serviced
 * from the queue, they have already had software loopback and pseudo-digital cross-connect already
 * performed.  The only actions left are:
 *
 * 1. Duplicate the message block for each active and connected upper MX Stream (that has not
 *    already been marked with processing this block) and pass the message block to the Stream.
 *
 * 2. Copy the channels specified by each active upper CH Stream (that ahs not already been marked
 *    with processing this block) into a newly allocated message block and pass it to the Stream.
 */
static streamscall int
ma_rsrv(queue_t *q)
{
	union ma *ma;
	mblk_t *mp;

	if (likely(!!(ma = (typeof(ma)) mi_trylock(q)))) {
		while (likely(!!(mp = getq(q)))) {
			if (unlikely(ma_rsrv_msg(ma, q, mp))) {
				if (unlikely(!putbq(q, mp))) {
					mp->b_band = 0;
					putbq(q, mp);
				}
				break;
			}
		}
		mi_unlock((caddr_t) ma);
	}
	return (0);
}

/**
 * ma_rput: - matrix lower read put procedure
 * @q: active queue (lower read queue)
 * @mp: the message to put
 *
 * The lower read queue can either be an MX or a CH Stream.  For either MX or CH Stream, this read
 * put procedure might be invoked by the lower level driver at interrupt.  All timing and
 * synchronization is fed from the read queue put procedure.  Steps are as follows:
 *
 * Write lock the multiplex.
 *
 * 1. Perform software loopback within the same MX or CH.
 *
 * 2. Perform pseudo-digital cross-connection within the same MX or CH.  (This is the same as
 *    software loopback for CH.)
 *
 * 3. Release the transmit block to the driver with esballoc and qreply() unless flow controlled, in
 *    which case discard the transmit block.  Bump the transmit block (head) pointer.
 *
 * 4. Perform pseudo-digital cross-connect.  Take samples from the message block and copy them to
 *    outgoing transmission blocks in elastic buffers on lower CH or MX streams.
 *
 * Write unlock the multiplex.
 *
 * If there are latency sensitive CH or MX Streams that are fed by this lower stream:
 *
 * Read lock the multiplex.
 *
 * 5. Duplicate the message block to any latency sensitive upper MX Streams.  Mark the upper MX
 *    Stream as having processed this received MX or CH block by marking it with the sequence number
 *    associated with the receive block.  Skip slip sensitive upper MX Streams.
 *
 * 6. Demultiplex to the receive elastic buffer for the upper CH Stream.  Pass a freshly esballoc'ed
 *    message block for any latency sensitive upper CH Streams upstream.  Mark the stream as having
 *    processed this received MX or CH block by marking it with the sequence number associated with
 *    the receive block.  Skip slip sensitive upper CH Streams.
 *
 * Read unlock the multiplex.
 *
 * If there are slip sensitive CH or MX Streams that are fed by this lower stream, or if an
 * allocation failure occured on a latency sensitive upper MX Stream:
 *
 * 7. Place block on the queue for later servicing by the read service procedure.
 */
static streamscall int
ma_rput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || m_rput_msg(q, mp)) {
		if (unlikely(!putq(q, mp))) {
			mp->b_band = 0;
			putq(q, mp);
		}
	}
	return (0);
}

/*
 * QUEUE OPEN AND CLOSE ROUTINES
 */

static streamscall int
ma_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);
	minor_t bminor = cminor;
	unsigned long flags;
	union ma *ma;
	int err;

	switch (cminor) {
	case MATRIX_CLONE_CH_MINOR:
		bminor = cminor;
		sflag = CLONEOPEN;
		break;
	case MATRIX_CLONE_MX_MINOR:
		bminor = cminor;
		sflag = CLONEOPEN;
		break;
	}
	if (sflag == MODOPEN)
		/* cannot push as module */
		return (ENXIO);
	if (q->q_ptr) {
		if (sflag == CLONEOPEN)
			return (0);
		/* FIXME: This is not a clone device and yet it is already open. Check that the
		   opening process has sufficient credential to open the non-clone matrix device. */
		return (EPERM);
	}
	if (cminor >= MATRIX_CLONE_FREE_MINOR)
		/* Cannot open clone device numbers explicitly. */
		return (ENXIO);
	if (cminor > MATRIX_CLONE_MX_CMINOR)
		/* all others are non-clone devices */
		sflag = DRVOPEN;
	if (sflag == CLONEOPEN) {
		cminor = MATRIX_CLONE_FREE_MINOR;
		*devp = makedevice(cmajor, cminor);
	}
	write_lock_irqsave(&ma_lock, flags);
	if ((err = mi_open_comm(&ma_opens, sizeof(*ma), q, devp, oflags, sflag, crp))) {
		write_unlock_irqrestore(&ma_lock, flags);
		return (err);
	}
	ma = MA_PRIV(q);
	/* initialize matrix private structure */
	ma->common.oq = q;
	ma->common.iq = WR(q);
	/* FIXME: more... */
	write_unlock_irqrestore(&ma_lock, flags);
	qprocson(q);
	return (0);
}

static streamscall int
ma_qclose(queue_t *q, int oflags, cred_t *crp)
{
	unsigned long flags;

	write_lock_irqsave(&ma_lock, flags);
	qprocsoff(q);
	mi_close_comm(&ma_opens, q);
	write_unlock_irqrestore(&ma_lock, flags);
	return (0);
}

/*
 * STREAMS DEFINITIONS
 */

static struct module_info ma_minfo = {
	.mi_idnum = DRV_ID,
	.mi_idname = DRV_NAME,
	.mi_maxpsz = STRMAXPSZ,
	.mi_minpsz = STRMINPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat ma_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat ma_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static struct qinit ma_rdinit = {
	.qi_srvp = ma_muxrsrv,
	.qi_qopen = ma_qopen,
	.qi_qclose = ma_qclose,
	.qi_minfo = &ma_minfo,
	.qi_mstat = &ma_rstat,
};
static struct qinit ma_wrinit = {
	.qi_putp = ma_wput,
	.qi_srvp = ma_wsrv,
	.qi_minfo = &ma_minfo,
	.qi_mstat = &ma_wstat,
};
static struct qinit ma_muxrinit = {
	.qi_putp = ma_rput,
	.qi_srvp = ma_rsrv,
	.qi_minfo = &ma_minfo,
	.qi_mstat = &ma_rstat,
};
static struct qinit ma_muxwinit = {
	.qi_srvp = ma_muxwsrv,
	.qi_minfo = &ma_minfo,
	.qi_mstat = &ma_wstat,
};

MODULE_STATIC struct streamtab matrixinfo = {
	.st_rdinit = &ma_rdinit,
	.st_wrinit = &ma_wrinit,
	.st_muxrinit = &ma_muxrinit,
	.st_muxwinit = &ma_muxwinit,
};

static struct cdevsw ma_cdev = {
	.d_name = DRV_NAME,
	.d_str = &matrixinfo,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

static __init int
matrixinit(void)
{
	int err;

	cmn_err(CE_NOTE, DRV_BANNER);
	if ((err = register_strdev(&ma_cdev, major)) < 0) {
		cmn_err(CE_WARN, "%s: could not register STREAMS device %d, err = %d\n", DRV_NAME,
			(int) major, -err);
		return (err);
	}
	if (major == 0)
		major = err;
	return (0);
}

static int
matrixterminate(void)
{
	int err;

	if ((err = unregister_strdev(&ma_cdev, major)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister STREAMS device %d, err = %d\n", DRV_NAME,
			(int) major, -err);
		return (err);
	}
	major = 0;
	return (0);
}

static __exit void
matrixexit(void)
{
	matrixterminate();
}

module_init(matrixinit);
module_exit(matrixexit);
