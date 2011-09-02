/*****************************************************************************

 @(#) $RCSfile: rtp_ch.c,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-09-02 08:46:54 $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2011  Monavacon Limited <http://www.monavacon.com/>
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

 -----------------------------------------------------------------------------

 Last Modified $Date: 2011-09-02 08:46:54 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: rtp_ch.c,v $
 Revision 1.1.2.2  2011-09-02 08:46:54  brian
 - sync up lots of repo and build changes from git

 Revision 1.1.2.1  2011-01-11 23:40:11  brian
 - added new files

 *****************************************************************************/

static char const ident[] = "$RCSfile: rtp_ch.c,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-09-02 08:46:54 $";

/*
 * This is the RTP-CH module.  This is a pushable STREAMS module that can be
 * pushed over an RTP multi-Stream to provide for transcoding and protocol
 * conversion between the RTP module and a multi-Stream Channel Interface (CHI)
 * user above.  The intended CHI user is the MG multiplexing driver.
 *
 * The RTP to Channel conversion supports transcoding.  This module is intended
 * to be pushed over an RTP module.  It supports a multi-Stream Channel
 * Interface (CHI) at the upper interface and an RTP interface using the
 * multi-Stream Network Provider Interface (NPI) at the lower interface.  The
 * purpose of this module is to adapt between an MG driver lower multiplex at
 * the upper interface and a multi-Stream RTP Switch driver at the lower
 * interface.
 *
 * The module expects fully terminated (de-jittered and RTCP terminated) RTP
 * input at the lower interface and upacks and transcodes the AVT profile
 * payload into channel samples that are PCM A-law, mu-law or linear (signed
 * 16-bit).  The module expects a steady stream of PCM A-law, mu-law or linear
 * (signed 16-bit) samples on the upper interface which it will encode in the
 * appropriate format for output on the lower interface.  The simplest instance
 * of the RTP-CH module simply packs and unpacks PCM mu-law samples to and from
 * G.711 RTP packets.
 *
 * For efficiency on large scale systems, one RTP-CH instance should be created
 * for each coding format.  This is so that STREAMS queuing advantages can be
 * fully realized and instruction caches will be kept hot for transcoding blocks
 * of packets.
 */

#define _SVR4_SOURCE
#define _MPS_SOURCE
#define _SUN_SOURCE

#include <sys/os7/compat.h>

#include <sys/npi.h>
#include <sys/npi_ip.h>
#include <sys/npi_udp.h>

#include <sys/chi.h>
#include <sys/chi_ioctl.h>

#define RTPCH_DESCRIP	"RTP-CH STREAMS MODULE."
#define RTPCH_REVISION	"OpenSS7 $RCSfile: rtp_ch.c,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-09-02 08:46:54 $"
#define RTPCH_COPYRIGHT	"Copyright (c) 2008-2011  Monavacon Limited.  All Rights Reserved."
#define RTPCH_DEVICE	"Provides OpenSS7 RTP to CH module."
#define RTPCH_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define RTPCH_LICENSE	"GPL"
#define RTPCH_BANNER	RTPCH_DESCRIP	"\n" \
			RTPCH_REVISION	"\n" \
			RTPCH_COPYRIGHT	"\n" \
			RTPCH_DEVICE	"\n" \
			RTPCH_CONTACT	"\n"
#define RTPCH_SPLASH	RTPCH_DEVICE	" - " \
			RTPCH_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(RTPCH_CONTACT);
MODULE_DESCRIPTION(RTPCH_DESCRIP);
MODULE_SUPPORTED_DEVICE(RTPCH_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(RTPCH_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-rtp-ch");
#endif				/* MODULE_ALIAS */
#ifdef MODULE_VERSION
MODULE_VERSION(PACKAGE_ENVR);
#endif				/* MODULE_VERSION */
#endif				/* LINUX */

#define RTPCH_MOD_ID	CONFIG_STREAMS_RTPCH_MODID
#define RTPCH_MOD_NAME	CONFIG_STREAMS_RTPCH_NAME

#ifndef RTPCH_MOD_NAME
#define RTPCH_MOD_NAME	"rtp-ch"
#endif				/* RTPCH_MOD_NAME */

#ifndef RTPCH_MOD_ID
#define RTPCH_MOD_ID	0
#endif				/* RTPCH_MOD_ID */

/*
 *  -------------------------------------------------------------------------
 *
 *  STREAMS DEFINITIONS
 *
 *  -------------------------------------------------------------------------
 */

#define MOD_ID		RTPCH_MOD_ID
#define MOD_NAME	RTPCH_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	RTPCH_BANNER
#else				/* MODULE */
#define MOD_BANNER	RTPCH_SPLASH
#endif				/* MODULE */

static struct module_info rtpch_minfo = {
	.mi_idnum = MOD_ID,	/* Module ID number */
	.mi_idname = MOD_NAME,	/* Module name */
	.mi_minpsz = STRMINPSZ,	/* Min packet size accepted */
	.mi_maxpsz = STRMAXPSZ,	/* Max packet size accepted */
	.mi_hiwat = STRHIGH,	/* Hi water mark */
	.mi_lowat = STRLOW,	/* Lo water mark */
};

static struct module_stat rtpch_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat rtpch_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

/*
 *  -------------------------------------------------------------------------
 *
 *  PRIVATE STRUCTURE
 *
 *  -------------------------------------------------------------------------
 */

#define CHANNEL_MAGIC	0xdeadbeeffeeddaadUL

/* Channel structure */
struct ch {
	unsigned long magic;		/* set to CHANNEL_MAGIC */
	int ch_state;			/* state of the CHI interface */
	unsigned long ch_tag;		/* CHS-User tag */
	int np_state;			/* state of the NPI interface */
	unsigned long np_tag;		/* NS-Provider tag */
	int flags;			/* General purppose flags */
};

/* Private structure. */
struct rc {
	queue_t *wq;
	queue_t *rq;
	struct ch oldstate;
	struct CH_info_ack info;
	struct CH_parms_circuit parms;
	mblk_t *tick;
	unsigned long interval;
};

/*
 *  -------------------------------------------------------------------------
 *
 *  DEBUGGING
 *
 *  -------------------------------------------------------------------------
 */

#define STRLOGERR	0	/* log error information */
#define STRLOGNO	0	/* log notice information */
#define STRLOGST	1	/* log state transitions */
#define STRLOGTO	2	/* log timeouts */
#define STRLOGRX	3	/* log primitives received */
#define STRLOGTX	4	/* log primitives issued */
#define STRLOGTE	5	/* log timer events */
#define STRLOGIO	6	/* log additional data */
#define STRLOGDA	7	/* log data */

/** rc_errname: display UNIX error name
  * @errno: the error to display
  */
static const char *
rc_errname(int errno)
{
	/* FIXME: remove the ones that are not used in this file. */
	switch (errno) {
	case EDEADLK:
		return ("EDEADLK");
	case ENAMETOOLONG:
		return ("ENAMETOOLONG");
	case ENOLCK:
		return ("ENOLCK");
	case ENOSYS:
		return ("ENOSYS");
	case ENOTEMPTY:
		return ("ENOTEMPTY");
	case ELOOP:
		return ("ELOOP");
	case EWOULDBLOCK:
		return ("EWOULDBLOCK");
	case ENOMSG:
		return ("ENOMSG");
	case EIDRM:
		return ("EIDRM");
	case ECHRNG:
		return ("ECHRNG");
	case EL2NSYNC:
		return ("EL2NSYNC");
	case EL3HLT:
		return ("EL3HLT");
	case EL3RST:
		return ("EL3RST");
	case ELNRNG:
		return ("ELNRNG");
	case EUNATCH:
		return ("EUNATCH");
	case ENOCSI:
		return ("ENOCSI");
	case EL2HLT:
		return ("EL2HLT");
	case EBADE:
		return ("EBADE");
	case EBADR:
		return ("EBADR");
	case EXFULL:
		return ("EXFULL");
	case ENOANO:
		return ("ENOANO");
	case EBADRQC:
		return ("EBADRQC");
	case EBADSLT:
		return ("EBADSLT");
	case EBFONT:
		return ("EBFONT");
	case ENOSTR:
		return ("ENOSTR");
	case ENODATA:
		return ("ENODATA");
	case ETIME:
		return ("ETIME");
	case ENOSR:
		return ("ENOSR");
	case ENONET:
		return ("ENONET");
	case ENOPKG:
		return ("ENOPKG");
	case EREMOTE:
		return ("EREMOTE");
	case ENOLINK:
		return ("ENOLINK");
	case EADV:
		return ("EADV");
	case ESRMNT:
		return ("ESRMNT");
	case ECOMM:
		return ("ECOMM");
	case EPROTO:
		return ("EPROTO");
	case EMULTIHOP:
		return ("EMULTIHOP");
	case EDOTDOT:
		return ("EDOTDOT");
	case EBADMSG:
		return ("EBADMSG");
	case EOVERFLOW:
		return ("EOVERFLOW");
	case ENOTUNIQ:
		return ("ENOTUNIQ");
	case EBADFD:
		return ("EBADFD");
	case EREMCHG:
		return ("EREMCHG");
	case ELIBACC:
		return ("ELIBACC");
	case ELIBBAD:
		return ("ELIBBAD");
	case ELIBSCN:
		return ("ELIBSCN");
	case ELIBMAX:
		return ("ELIBMAX");
	case ELIBEXEC:
		return ("ELIBEXEC");
	case EILSEQ:
		return ("EILSEQ");
	case ERESTART:
		return ("ERESTART");
	case ESTRPIPE:
		return ("ESTRPIPE");
	case EUSERS:
		return ("EUSERS");
	case ENOTSOCK:
		return ("ENOTSOCK");
	case EDESTADDRREQ:
		return ("EDESTADDRREQ");
	case EMSGSIZE:
		return ("EMSGSIZE");
	case EPROTOTYPE:
		return ("EPROTOTYPE");
	case ENOPROTOOPT:
		return ("ENOPROTOOPT");
	case EPROTONOSUPPORT:
		return ("EPROTONOSUPPORT");
	case ESOCKTNOSUPPORT:
		return ("ESOCKTNOSUPPORT");
	case EOPNOTSUPP:
		return ("EOPNOTSUPP");
	case EPFNOSUPPORT:
		return ("EPFNOSUPPORT");
	case EAFNOSUPPORT:
		return ("EAFNOSUPPORT");
	case EADDRINUSE:
		return ("EADDRINUSE");
	case EADDRNOTAVAIL:
		return ("EADDRNOTAVAIL");
	case ENETDOWN:
		return ("ENETDOWN");
	case ENETUNREACH:
		return ("ENETUNREACH");
	case ENETRESET:
		return ("ENETRESET");
	case ECONNABORTED:
		return ("ECONNABORTED");
	case ECONNRESET:
		return ("ECONNRESET");
	case ENOBUFS:
		return ("ENOBUFS");
	case EISCONN:
		return ("EISCONN");
	case ENOTCONN:
		return ("ENOTCONN");
	case ESHUTDOWN:
		return ("ESHUTDOWN");
	case ETOOMANYREFS:
		return ("ETOOMANYREFS");
	case ETIMEDOUT:
		return ("ETIMEDOUT");
	case ECONNREFUSED:
		return ("ECONNREFUSED");
	case EHOSTDOWN:
		return ("EHOSTDOWN");
	case EHOSTUNREACH:
		return ("EHOSTUNREACH");
	case EALREADY:
		return ("EALREADY");
	case EINPROGRESS:
		return ("EINPROGRESS");
	case ESTALE:
		return ("ESTALE");
	case EUCLEAN:
		return ("EUCLEAN");
	case ENOTNAM:
		return ("ENOTNAM");
	case ENAVAIL:
		return ("ENAVAIL");
	case EISNAM:
		return ("EISNAM");
	case EREMOTEIO:
		return ("EREMOTEIO");
	case EDQUOT:
		return ("EDQUOT");
	case ENOMEDIUM:
		return ("ENOMEDIUM");
	case EMEDIUMTYPE:
		return ("EMEDIUMTYPE");
	case ECANCELED:
		return ("ECANCELED");
	default:
		return ("(unknown errno)");
	}
}

/** ch_iocname: display CH ioctl command name
  * @cmd: ioctl command
  */
static const char *
ch_iocname(int cmd)
{
	switch (_IOC_NR(cmd)) {
	case _IOC_NR(CH_IOCGCONFIG):
		return ("CH_IOCGCONFIG");
	case _IOC_NR(CH_IOCSCONFIG):
		return ("CH_IOCSCONFIG");
	case _IOC_NR(CH_IOCTCONFIG):
		return ("CH_IOCTCONFIG");
	case _IOC_NR(CH_IOCCCONFIG):
		return ("CH_IOCCCONFIG");
	case _IOC_NR(CH_IOCGSTATEM):
		return ("CH_IOCGSTATEM");
	case _IOC_NR(CH_IOCCMRESET):
		return ("CH_IOCCMRESET");
	case _IOC_NR(CH_IOCGSTATSP):
		return ("CH_IOCGSTATSP");
	case _IOC_NR(CH_IOCSSTATSP):
		return ("CH_IOCSSTATSP");
	case _IOC_NR(CH_IOCGSTATS):
		return ("CH_IOCGSTATS");
	case _IOC_NR(CH_IOCCSTATS):
		return ("CH_IOCCSTATS");
	case _IOC_NR(CH_IOCGNOTIFY):
		return ("CH_IOCGNOTIFY");
	case _IOC_NR(CH_IOCSNOTIFY):
		return ("CH_IOCSNOTIFY");
	case _IOC_NR(CH_IOCCNOTIFY):
		return ("CH_IOCCNOTIFY");
	case _IOC_NR(CH_IOCCMGMT):
		return ("CH_IOCCMGMT");
	default:
		return ("(unknown ioctl)");
	}
}

/** ch_primname: display CHI primitive name
  * @prim: the primitive to display
  */
static const char *
ch_primname(ch_ulong prim)
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
		return ("(unknown primitive)");
	}
}

/** ch_statename: display CHI state name
  * @state: the state value to display
  */
static const char *
ch_statename(ch_ulong state)
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
		return ("CHS_????");
	}
}

/** ch_errname: display CHI error name
  * @error: the error to display
  */
static const char *
ch_errname(int error)
{
	if (error < 0)
		return rc_errname(-error);
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
	default:
		return ("(unknown error)");
	}
}

/** np_primname: display NPI primitive name
  * @prim: the primitive to display
  */
static const char *
np_primname(np_ulong prim)
{
	switch (prim) {
	case N_CONN_REQ:
		return ("N_CONN_REQ");
	case N_CONN_RES:
		return ("N_CONN_RES");
	case N_DISCON_REQ:
		return ("N_DISCON_REQ");
	case N_DATA_REQ:
		return ("N_DATA_REQ");
	case N_EXDATA_REQ:
		return ("N_EXDATA_REQ");
	case N_INFO_REQ:
		return ("N_INFO_REQ");
	case N_BIND_REQ:
		return ("N_BIND_REQ");
	case N_UNBIND_REQ:
		return ("N_UNBIND_REQ");
	case N_UNITDATA_REQ:
		return ("N_UNITDATA_REQ");
	case N_OPTMGMT_REQ:
		return ("N_OPTMGMT_REQ");
	case N_CONN_IND:
		return ("N_CONN_IND");
	case N_CONN_CON:
		return ("N_CONN_CON");
	case N_DISCON_IND:
		return ("N_DISCON_IND");
	case N_DATA_IND:
		return ("N_DATA_IND");
	case N_EXDATA_IND:
		return ("N_EXDATA_IND");
	case N_INFO_ACK:
		return ("N_INFO_ACK");
	case N_BIND_ACK:
		return ("N_BIND_ACK");
	case N_ERROR_ACK:
		return ("N_ERROR_ACK");
	case N_OK_ACK:
		return ("N_OK_ACK");
	case N_UNITDATA_IND:
		return ("N_UNITDATA_IND");
	case N_UDERROR_IND:
		return ("N_UDERROR_IND");
	case N_DATACK_REQ:
		return ("N_DATACK_REQ");
	case N_DATACK_IND:
		return ("N_DATACK_IND");
	case N_RESET_REQ:
		return ("N_RESET_REQ");
	case N_RESET_IND:
		return ("N_RESET_IND");
	case N_RESET_RES:
		return ("N_RESET_RES");
	case N_RESET_CON:
		return ("N_RESET_CON");
	default:
		return ("(unknown primitive)");
	}
}

/** np_statename: display NPI state name
  * @state: the state value to display
  */
static const char *
np_statename(long state)
{
	switch (state) {
	case NS_UNBND:
		return ("NS_UNBND");
	case NS_WACK_BREQ:
		return ("NS_WACK_BREQ");
	case NS_WACK_UREQ:
		return ("NS_WACK_UREQ");
	case NS_IDLE:
		return ("NS_IDLE");
	case NS_WACK_OPTREQ:
		return ("NS_WACK_OPTREQ");
	case NS_WACK_RRES:
		return ("NS_WACK_RRES");
	case NS_WCON_CREQ:
		return ("NS_WCON_CREQ");
	case NS_WRES_CIND:
		return ("NS_WRES_CIND");
	case NS_WACK_CRES:
		return ("NS_WACK_CRES");
	case NS_DATA_XFER:
		return ("NS_DATA_XFER");
	case NS_WCON_RREQ:
		return ("NS_WCON_RREQ");
	case NS_WRES_RIND:
		return ("NS_WRES_RIND");
	case NS_WACK_DREQ6:
		return ("NS_WACK_DREQ6");
	case NS_WACK_DREQ7:
		return ("NS_WACK_DREQ7");
	case NS_WACK_DREQ9:
		return ("NS_WACK_DREQ9");
	case NS_WACK_DREQ10:
		return ("NS_WACK_DREQ10");
	case NS_WACK_DREQ11:
		return ("NS_WACK_DREQ11");
	case NS_NOSTATES:
		return ("NS_NOSTATES");
	default:
		return ("NS_????");
	}
}

/** np_errname: display NPI error name
  * @error: the error to display
  */
static const char *
np_errname(int error)
{
	if (error < 0)
		return rc_errname(-error);

	switch (error) {
	case NBADADDR:
		return ("NBADADDR");
	case NBADOPT:
		return ("NBADOPT");
	case NACCESS:
		return ("NACCESS");
	case NNOADDR:
		return ("NNOADDR");
	case NOUTSTATE:
		return ("NOUTSTATE");
	case NBADSEQ:
		return ("NBADSEQ");
	case NSYSERR:
		return ("NSYSERR");
	case NBADDATA:
		return ("NBADDATA");
	case NBADFLAG:
		return ("NBADFLAG");
	case NNOTSUPPORT:
		return ("NNOTSUPPORT");
	case NBOUND:
		return ("NBOUND");
	case NBADQOSPARAM:
		return ("NBADQOSPARAM");
	case NBADQOSTYPE:
		return ("NBADQOSTYPE");
	case NBADTOKEN:
		return ("NBADTOKEN");
	case NNOPROTOID:
		return ("NNOPROTOID");
	default:
		return ("(unknown error)");
	}
}

/** ch_get_state: - get CHI state from channel structure
  * @ch: channel structure
  */
static int
ch_get_state(struct ch *ch)
{
	return (ch->ch_state);
}

/** ch_set_state: - set CHI state in channel structure
  * @q: active queue
  * @ch: channel structure
  */
static int
ch_set_state(queue_t *q, struct ch *ch, int newstate)
{
	int oldstate = ch_get_state(ch);

	if (newstate != oldstate) {
		ch->ch_state = newstate;
		mi_strlog(q, STRLOGST, SL_TRACE, "%s <- %s", ch_statename(newstate),
			  ch_statename(oldstate));
	}
	return (newstate);
}

/** ch_save_state: - save the CHI state
  * @priv: private structure (locked)
  * @ch: channel structure
  */
static int
ch_save_state(struct rc *priv, struct ch *ch)
{
	return (priv->oldstate.ch_state = ch_get_state(ch));
}

/** ch_restore_state: - restore the previoius CHI state
  * @priv: private structure (locked)
  * @ch: channel structure
  */
static int
ch_restore_state(struct rc *priv, struct ch *ch)
{
	return (ch->ch_state = priv->oldstate.ch_state);
}

/** np_get_state: - get NPI state from channel structure
  * @ch: channel structure
  */
static int
np_get_state(struct ch *ch)
{
	return (ch->np_state);
}

/** np_set_state: - set NPI state in channel structure
  * @q: active queue
  * @ch: channel structure
  */
static int
np_set_state(queue_t *q, struct ch *ch, int newstate)
{
	int oldstate = np_get_state(ch);

	if (newstate != oldstate) {
		ch->np_state = newstate;
		mi_strlog(q, STRLOGST, SL_TRACE, "%s <- %s", np_statename(newstate),
			  np_statename(oldstate));
	}
	return (newstate);
}

/** np_save_state: - save the NPI state
  * @priv: private structure (locked)
  * @ch: channel structure
  */
static int
np_save_state(struct rc *priv, struct ch *ch)
{
	return (priv->oldstate.np_state = np_get_state(ch));
}

/** np_restore_state: - resotre the NPI state
  * @priv: private structure (locked)
  * @ch: channel structure
  */
static int
np_restore_state(struct rc *priv, struct ch *ch)
{
	return (ch->np_state = priv->oldstate.np_state);
}

/**
  * Care must be taken because @ch might be NULL for CH_ATTACH_REQ primitives.
  */
static void
rc_save_total_state(struct rc *priv, struct ch *ch)
{
	if (likely(ch != NULL))
		priv->oldstate = *ch;
}

/**
  * Care must be taken because @ch might be NULL for CH_ATTACH_REQ primitives.
  */
static void
rc_restore_total_state(struct rc *priv, struct ch *ch)
{
	if (likely(ch != NULL))
		*ch = priv->oldstate;
}

/*
 *  -------------------------------------------------------------------------
 *
 *  CH-Provider to CH-User Primitives
 *
 *  -------------------------------------------------------------------------
 */

/** ch_error_ack: - issue CH_ERROR_ACK primitive
  * @q: active queue
  * @msg: message to free upon success
  * @rc: private structure (locked)
  * @ch: channel structure (may be NULL)
  * @prim: primitive in error
  * @error: error number
  *
  * Only called by ch_error_reply(), but then, it can be invoked by the handling procedure for just
  * about any CH-primitive passed down through the module.  State is restored by falling back to the
  * last checkpoint state.
  *
  * Note that the @ch argument is NULL when we are replying with an error to a CH_ATTACH_REQ.  In
  * this case, however, the @msg argument _is_ the CH_ATTACH_REQ and the CHS-User tag can be
  * obtained from the @msg.  In this case @prim is always CH_ATTACH_REQ.
  *
  * Note that the @ch argument may also be NULL when we could not decypher the provider tag.  In
  * this case we use the error CHBADSLOT.  The CHS-User tag is unknown in this case, so we will
  * return the bad NS-Provider tag that the CHS-User placed in the message instead.
  */
static int
ch_error_ack(queue_t *q, mblk_t *msg, struct rc *rc, struct ch *ch, int prim, int error)
{
	CH_error_ack_t *p;
	mblk_t *mp;
	int state;

	if (unlikely(!(mp = mi_allocb(q, sizeof(ch->ch_tag) + sizeof(*p), BPRI_MED))))
		goto enobufs;

	DB_TYPE(mp) = M_PCPROTO;

	*(typeof(ch->ch_tag) *) mp->b_wptr = ch ? ch->ch_tag : *(typeof(ch->ch_tag) *) msg->b_rptr;
	mp->b_wptr += sizeof(ch->ch_tag);

	p = (typeof(p)) mp->b_wptr;
	p->ch_primitive = CH_ERROR_ACK;
	p->ch_error_primitive = prim;
	p->ch_error_type = error < 0 ? CHSYSERR : error;
	p->ch_unix_error = error < 0 ? -error : 0;
	p->ch_state = ch ? ch_restore_state(rc, ch) : CHS_UNINIT;
	mp->b_wptr += sizeof(*p);

	/* When channels error back to the detached state, they have reached the end of their life
	   cycle and need to be destroyed. */
	if (ch) {
		switch (ch_get_state(ch)) {
		case CHS_UNINIT:
		case CHS_UNUSABLE:
		case CHS_DETACHED:
			ch_free(ch);
			break;
		}
	}

	freemsg(msg);
	mi_strlog(rc->rq, STRLOGTX, SL_TRACE, "<- CH_ERROR_ACK");
	putnext(rc->rq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
}

/** ch_error_reply: - reply with an error
  * @q: active queue
  * @msg: message to free upon success
  * @rc: private structure (locked)
  * @ch: channel structure (might be NULL)
  * @prim: primitive in error
  * @error: error number
  *
  * This is essentially the same as ch_error_ack() except that typical queue return codes are
  * filtered and returned directly.  ch_error_reply() and a ch_error_ack() can be invoked from any
  * CH-primitive being issued in the downward direction.
  */
static int
ch_error_reply(queue_t *q, mblk_t *msg, struct rc *rc, struct ch *ch, int prim, int error)
{
	if (unlikely(error <= 0)) {
		switch (-error) {
		case 0:
		case EBUSY:
		case ENOBUFS:
		case ENOMEM:
		case EAGAIN:
		case EDEADLK:
			return (error);
		default:
			break;
		}
	}
	return ch_error_ack(q, msg, rc, ch, prim, error);
}

/** ch_ok_ack: - issue CH_OK_ACK primitive
  * @q: active queue
  * @msg: message to free upon success
  * @rc: private structure (locked)
  * @ch: channel structure
  * @prim: correct primitive
  */
static int
ch_ok_ack(queue_t *q, mblk_t *msg, struct rc *rc, struct ch *ch, int prim)
{
	CH_ok_ack_t *p;
	mblk_t *mp;
	int state;

	if (unlikely(!(mp = mi_allocb(q, sizeof(ch->ch_tag) + sizeof(*p), BPRI_MED))))
		goto enobufs;

	switch ((state = ch_get_state(ch))) {
	case CHS_WACK_AREQ:
		state = CHS_ATTACHED;
		break;
	case CHS_WACK_UREQ:
		state = CHS_DETACHED;
		break;
	case CHS_WACK_EREQ:
		state = CHS_ENABLED;
		break;
	case CHS_WACK_RREQ:
		state = CHS_ATTACHED;
		break;
	case CHS_WACK_CREQ:
		state = CHS_CONNECTED;
		break;
	case CHS_WACK_DREQ:
		state = CHS_ENABLED;
		break;
	}

	DB_TYPE(mp) = M_PCPROTO;

	*(typeof(ch->ch_tag) *) mp->b_wptr = ch->ch_tag;
	mp->b_wptr += sizeof(ch->ch_tag);

	p = (typeof(p)) mp->b_wptr;
	p->ch_primitive = CH_OK_ACK;
	p->ch_correct_prim = prim;
	p->ch_state = state;
	mp->b_wptr += sizeof(*p);

	/* When channels change to the detached state, they have reached the end of their life
	   cycle and need to be destroyed. */
	switch (ch_set_state(rc->rq, ch, state)) {
	case CHS_UNINIT:
	case CHS_UNUSABLE:
	case CHS_DETACHED:
		ch_free(ch);
		break;
	}

	freemsg(msg);
	mi_strlog(rc->rq, STRLOGTX, SL_TRACE, "<- CH_OK_ACK");
	putnext(rc->rq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
}

/** ch_attach_ack: - issue a CH_ATTACH_ACK primitive
  * @q: active queue
  * @msg: message to free upon success
  * @rc: private structure (locked)
  * @ch: channel structure
  * @flags: attach flags
  * @add_ptr: attached address pointer
  * @add_len: attached address length
  *
  * The CH_ATTACH_ACK is issued to the upper layer whenever a valid N_BIND_ACK
  * is received from the lower layer.
  */
static int
ch_attach_ack(queue_t *q, mblk_t *msg, struct rc *rc, struct ch *ch, int flags, caddr_t add_ptr,
	      size_t add_len)
{
	CH_attach_ack_t *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, 2 * sizeof(ch->ch_tag) + sizeof(*p) + add_len, BPRI_MED))))
		goto enobufs;

	DB_TYPE(mp) = M_PCPROTO;

	*(typeof(ch->ch_tag) *) mp->b_wptr = ch->ch_tag;
	mp->b_wptr += sizeof(ch->ch_tag);

	p = (typeof(p)) mp->b - wptr;
	p->ch_primitive = CH_ATTACH_ACK;
	p->ch_addr_length = add_len;
	p->ch_addr_offset = 2 * sizeof(ch->ch_tag) + sizeof(*p);
	p->ch_flags = flags;
	mp->b_wptr += sizeof(*p);

	*(typeof(ch->ch_tag) *) mp->b_wptr = (typeof(ch->ch_tag)) ch;
	mp->b_wptr += sizeof(ch->ch_tag);

	bcopy(mp->b_wptr, add_ptr, add_len);
	mp->b_wptr += add_len;

	freemsg(msg);
	/* The CH_ATTACH_ACK is issued to the upper layer whenever a valid N_BIND_ACK is received
	   from the lower layer. */
	np_set_state(rc->rq, ch, NS_IDLE);
	ch_set_state(rc->rq, ch, CHS_ATTACHED);
	mi_strlog(rc->rq, STRLOGTX, SL_TRACE, "<- CH_ATTACH_ACK");
	putnext(rc->rq, mp);
	return (0);

      enobufs:
	ch_free(ch);
	return (-ENOBUFS);
      enomem:
	return (-ENOMEM);
}

/** ch_info_ack: - issue CH_INFO_ACK primitive
  * @q: active queue
  * @msg: message to free upon success
  * @rc: private structure (locked)
  * @ch: channel structure
  * @add_ptr: address pointer
  * @add_len: address length
  * @par_ptr: parameters pointer
  * @par_len: parameters length
  */
static int
ch_info_ack(queue_t *q, mblk_t *msg, struct rc *rc, struct ch *ch, caddr_t add_ptr, size_t add_len,
	    caddr_t par_ptr, size_t par_len)
{
	CH_info_ack_t *p;
	mblk_t *mp;

	if (unlikely
	    (!(mp = mi_allocb(q, sizeof(ch->ch_tag) + sizeof(*p) + add_len + par_len, BPRI_MED))))
		goto enobufs;

	DB_TYPE(mp) = M_PCPROTO;

	*(typeof(ch->ch_tag) *) mp->b_wptr = ch ? ch->ch_tag : (typeof(ch->ch_tag)) 0;
	mp->b_wptr += sizeof(ch->ch_tag);

	p = (typeof(p)) mp->b_wptr;
	p->ch_primitive = CH_INFO_ACK;
	p->ch_addr_length = add_len;
	p->ch_addr_offset = sizeof(ch->ch_tag) + sizeof(*p);
	p->ch_parm_length = par_len;
	p->ch_parm_offset = p->ch_addr_offset + add_len;
	p->ch_prov_flags = rc->info.ch_prov_flags;
	p->ch_prov_class = rc->info.ch_prov_class;	/* well, ... CH_RTPCLASS? */
	p->ch_style = rc->info.ch_style;	/* multi-stream */
	p->ch_version = rc->info.ch_version;
	p->ch_state = ch ? ch_get_state(ch) : CHS_UNINIT;
	mp->b_wptr += sizeof(*p);

	bcopy(add_ptr, mp->b_wptr, add_len);
	mp->b_wptr += add_len;
	bcopy(par_ptr, mp->b_wptr, par_len);
	mp->b_wptr += par_len;

	freemsg(msg);
	mi_strlog(rc->rq, STRLOGTX, SL_TRACE, "<- CH_INFO_ACK");
	putnext(rc->rq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
}

/** ch_optmgmt_ack: - issue CH_OPTMGMT_ACK primitive
  * @q: active queue
  * @msg: message to free upon success
  * @rc: private structure (locked)
  * @ch: channel structure
  * @flags: management flags
  * @opt_ptr: options pointer
  * @opt_len: options length
  */
static int
ch_optmgmt_ack(queue_t *q, mblk_t *msg, struct rc *rc, struct ch *ch, int flags, caddr_t opt_ptr,
	       size_t opt_len)
{
	CH_optmgmt_ack_t *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(ch->ch_tag) + sizeof(*p) + opt_len, BPRI_MED))))
		goto enobufs;

	DB_TYPE(mp) = M_PCPROTO;

	*(typeof(ch->ch_tag) *) mp->b_wptr = ch ? ch->ch_tag : (typeof(ch->ch_tag)) 0;
	mp->b_wptr += sizeof(ch->ch_tag);

	p = (typeof(p)) mp->b_wptr;
	p->ch_primitive = CH_OPTMGMT_ACK;
	p->ch_opt_length = opt_len;
	p->ch_opt_offset = sizeof(ch->ch_tag) + sizeof(*p);
	p->ch_mgmt_flags = flags;
	mp->b_wptr += sizeof(*p);

	bcopy(opt_ptr, mp->b_wptr, opt_len);
	mp->b_wptr += opt_len;

	freemsg(msg);
	mi_strlog(rc->rq, STRLOGTX, SL_TRACE, "<- CH_OPTMGMT_ACK");
	putnext(rc->rq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
}

/** ch_enable_con: - issue CH_ENABLE_CON primitive
  * @q: active queue
  * @msg: message to free upon success
  * @rc: private structure (locked)
  * @ch: channel structure
  *
  * A note on bands.  Signaling messages are band 1 for normal calls and band 3
  * for emergency calls.
  */
static int
ch_enable_con(queue_t *q, mblk_t *msg, struct rc *rc, struct ch *ch)
{
	CH_enable_con_t *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(ch->ch_tag) + sizeof(*p), BPRI_MED))))
		goto enobufs;

	if (unlikely(!bcanputnext(rc->rq, band)))
		goto ebusy;

	DB_TYPE(mp) = M_PROTO;
	mp->b_band = band;

	*(typeof(ch->ch_tag) *) mp->b_wptr = ch ? ch->ch_tag : (typeof(ch->ch_tag)) 0;
	mp->b_wptr += sizeof(ch->ch_tag);

	p = (typeof(p)) mp->b_wptr;
	p->ch_primitive = CH_ENABLE_CON;
	mp->b_wptr += sizeof(*p);

	freemsg(msg);
	ch_set_state(rc->rq, ch, CHS_ENABLED);
	mi_strlog(rc->rq, STRLOGTX, SL_TRACE, "<- CH_ENABLE_CON");
	putnext(rc->rq, mp);
	return (0);

      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/** ch_connect_con: - issue CH_CONNECT_CON primitive
  * @q: active queue
  * @msg: message to free upon success
  * @rc: private structure (locked)
  * @ch: channel structure
  * @flags: connect flags
  *
  * A note on bands.  Signaling messages are band 1 for normal calls and band 3
  * for emergency calls.
  */
static int
ch_connect_con(queue_t *q, mblk_t *msg, struct rc *rc, struct ch *ch, int flags)
{
	CH_connect_con_t *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(ch->ch_tag) + sizeof(*p), BPRI_MED))))
		goto enobufs;

	if (unlikely(!bcanputnext(rc->rq, band)))
		goto ebusy;

	DB_TYPE(mp) = M_PROTO;
	mp->b_band = band;

	*(typeof(ch->ch_tag) *) mp->b_wptr = ch ? ch->ch_tag : (typeof(ch->ch_tag)) 0;
	mp->b_wptr += sizeof(ch->ch_tag);

	p = (typeof(p)) mp->b_wptr;
	p->ch_primitive = CH_CONNECT_CON;
	p->ch_conn_flags = flags;
	p->ch_slot = 0;
	mp->b_wptr += sizeof(*p);

	freemsg(msg);
	ch_set_state(rc->rq, ch, CHS_CONNECTED);
	mi_strlog(rc->rq, STRLOGTX, SL_TRACE, "<- CH_CONNECT_CON");
	putnext(rc->rq, mp);
	return (0);

      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/** ch_data_ind: - issue CH_DATA_IND primitive
  * @q: active queue
  * @msg: message to free upon success
  * @rc: private structure (locked)
  * @ch: channel structure
  * @dp: data to send
  */
static int
ch_data_ind(queue_t *q, mblk_t *msg, struct rc *rc, struct ch *ch, mblk_t *dp)
{
	CH_data_ind_t *p;
	mblk_t *mp;
	int band;

	if (unlikely(!(mp = mi_allocb(q, sizeof(ch->ch_tag) + sizeof(*p), BPRI_MED))))
		goto enobufs;

	/* Data messages are band 0 for normal calls and band 2 for emergency calls. */
	band = ch ? ((ch->flags & PRIORITY_CALL) ? 2 : 0) : 0;

	if (unlikely(!bcanputnext(rc->rq, band)))
		goto ebusy;

	DB_TYPE(mp) = M_PROTO;
	mp->b_band = band;

	*(typeof(ch->ch_tag) *) mp->b_wptr = ch ? ch->ch_tag : (typeof(ch->ch_tag)) 0;
	mp->b_wptr += sizeof(ch->ch_tag);

	p = (typeof(p)) mp->b_wptr;
	p->ch_primitive = CH_DATA_IND;
	p->ch_slot = 0;
	mp->b_wptr += sizeof(*p);

	mp->b_cont = dp;
	if (msg == dp)
		msg = NULL;
	if (msg && msg->b_cont = dp)
		msg->b_cont = NULL;

	freemsg(msg);
	mi_strlog(rc->rq, STRLOGDA, SL_TRACE, "<- CH_DATA_IND");
	putnext(rc->rq, mp);
	return (0);

      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/** ch_disconnect_ind: - issue CH_DISCONNECT_IND primitive
  * @q: active queue
  * @msg: message to free upon success
  * @rc: private structure (locked)
  * @ch: channel structure
  * @flags: disconnect flags
  * @cause: cause for disconnect
  */
static int
ch_disconnect_ind(queue_t *q, mblk_t *msg, struct rc *rc, struct ch *ch, int flags, int cause)
{
	CH_disconnect_ind_t *p;
	mblk_t *mp;
	int band;

	if (unlikely(!(mp = mi_allocb(q, sizeof(ch->ch_tag) + sizeof(*p), BPRI_MED))))
		goto enobufs;

	/* Signaling messages are band 1 for normal calls and band 3 for priority calls. */
	band = ch ? ((ch->flags & PRIORITY_CALL) ? 3 : 1) : 1;

	if (unlikely(!bcanputnext(rc->rq, band)))
		goto ebusy;

	DB_TYPE(mp) = M_PROTO;
	mp->b_band = band;

	*(typeof(ch->ch_tag) *) mp->b_wptr = ch ? ch->ch_tag : (typeof(ch->ch_tag)) 0;
	mp->b_wptr += sizeof(ch->ch_tag);

	p = (typeof(p)) mp->b_wptr;
	p->ch_primitive = CH_DISCONNECT_IND;
	p->ch_conn_flags = flags;
	p->ch_cause = cause;
	p->ch_slot = 0;
	mp->b_wptr += sizeof(*p);

	freemsg(msg);
	ch_set_state(rc->rq, ch, CHS_ENABLED);
	mi_strlog(rc->rq, STRLOGTX, SL_TRACE, "<- CH_DISCONNECT_IND");
	putnext(rc->rq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/** ch_disconnect_con: - issue CH_DISCONNECT_CON primitive
  * @q: active queue
  * @msg: message to free upon success
  * @rc: private structure (locked)
  * @ch: channel structure
  * @flags: disconnect flags
  */
static int
ch_disconnect_con(queue_t *q, mblk_t *msg, struct rc *rc, struct ch *ch, int flags)
{
	CH_disconnect_con_t *p;
	mblk_t *mp;
	int band;

	if (unlikely(!(mp = mi_allocb(q, sizeof(ch->ch_tag) + sizeof(*p), BPRI_MED))))
		goto enobufs;

	/* Signaling messages are band 1 for normal calls and band 3 for priority calls. */
	band = ch ? ((ch->flags & PRIORITY_CALL) ? 3 : 1) : 1;

	if (unlikely(!bcanputnext(rc->rq, band)))
		goto ebusy;

	DB_TYPE(mp) = M_PROTO;
	mp->b_band = band;

	*(typeof(ch->ch_tag) *) mp->b_wptr = ch ? ch->ch_tag : (typeof(ch->ch_tag)) 0;
	mp->b_wptr += sizeof(ch->ch_tag);

	p = (typeof(p)) mp->b_wptr;
	p->ch_primitive = CH_DISCONNECT_CON;
	p->ch_conn_flags = flags;
	p->ch_slot = 0;
	mp->b_wptr += sizeof(*p);

	freemsg(msg);
	ch_set_state(rc->rq, ch, CHS_ENABLED);
	mi_strlog(rc->rq, STRLOGTX, SL_TRACE, "<- CH_DISCONNECT_CON");
	putnext(rc->rq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/** ch_disable_ind: - issue CH_DISABLE_IND primitive
  * @q: active queue
  * @msg: message to free upon success
  * @rc: private structure (locked)
  * @ch: channel structure
  */
static int
ch_disable_ind(queue_t *q, mblk_t *msg, struct rc *rc, struct ch *ch)
{
	CH_disable_ind_t *p;
	mblk_t *mp;
	int band;

	if (unlikely(!(mp = mi_allocb(q, sizeof(ch->ch_tag) + sizeof(*p), BPRI_MED))))
		goto enobufs;

	/* Signaling messages are band 1 for normal calls and band 3 for priority calls. */
	band = ch ? ((ch->flags & PRIORITY_CALL) ? 3 : 1) : 1;

	if (unlikely(!bcanputnext(rc->rq, band)))
		goto ebusy;

	DB_TYPE(mp) = M_PROTO;
	mp->b_band = band;

	*(typeof(ch->ch_tag) *) mp->b_wptr = ch ? ch->ch_tag : (typeof(ch->ch_tag)) 0;
	mp->b_wptr += sizeof(ch->ch_tag);

	p = (typeof(p)) mp->b_wptr;
	p->ch_primitive = CH_DISABLE_IND;
	p->ch_cause = cause;
	mp->b_wptr += sizeof(*p);

	freemsg(msg);
	ch_set_state(rc->rq, ch, CHS_ATTACHED);
	mi_strlog(rc->rq, STRLOGTX, SL_TRACE, "<- CH_DISABLE_IND");
	putnext(rc->rq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);

}

/** ch_disable_con: - issue CH_DISABLE_CON primitive
  * @q: active queue
  * @msg: message to free upon success
  * @rc: private structure (locked)
  * @ch: channel structure
  */
static int
ch_disable_con(queue_t *q, mblk_t *msg, struct rc *rc, struct ch *ch)
{
	CH_disable_con_t *p;
	mblk_t *p;
	int band;

	if (unlikely(!(mp = mi_allocb(q, sizeof(ch->ch_tag) + sizeof(*p), BPRI_MED))))
		goto enobufs;

	/* Signaling messages are band 1 for normal calls and band 3 for priority calls. */
	band = ch ? ((ch->flags & PRIORITY_CALL) ? 3 : 1) : 1;

	if (unlikely(!bcanputnext(rc->rq, band)))
		goto ebusy;

	DB_TYPE(mp) = M_PROTO;
	mp->b_band = band;

	*(typeof(ch->ch_tag) *) mp->b_wptr = ch ? ch->ch_tag : (typeof(ch->ch_tag)) 0;
	mp->b_wptr += sizeof(ch->ch_tag);

	p = (typeof(p)) mp->b_wptr;
	p->ch_primitive = CH_DISABLE_CON;
	mp->b_wptr += sizeof(*p);

	freemsg(msg);
	ch_set_state(rc->rq, ch, CHS_ATTACHED);
	mi_strlog(rc->rq, STRLOGTX, SL_TRACE, "<- CH_DISABLE_CON");
	putnext(rc->rq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/** ch_event_ind: - issue CH_EVENT_IND primitive
  * @q: active queue
  * @msg: message to free upon success
  * @rc: private structure (locked)
  * @ch: channel structure
  * @event: event
  */
static int
ch_event_ind(queue_t *q, mblk_t *msg, struct rc *rc, struct ch *ch, int event)
{
	CH_event_ind_t *p;
	mblk_t *mp;
	int band;

	if (unlikely(!(mp = mi_alocb(q, sizeof(ch->ch_tag) + sizeof(*p), BPRI_MED))))
		goto enobufs;

	/* Signaling messages are band 1 for normal calls and band 3 for priority calls. */
	band = ch ? ((ch->flags & PRIORITY_CALL) ? 3 : 1) : 1;

	if (unlikely(!bcanputnext(rc->rq, band)))
		goto ebusy;

	DB_TYPE(mp) = M_PROTO;
	mp->b_band = band;

	*(typeof(ch->ch_tag) *) mp->b_wptr = ch ? ch->ch_tag : (typeof(ch->ch_tag)) 0;
	mp->b_wptr += sizeof(ch->ch_tag);

	p = (typeof(p)) mp->b_wptr;
	p->ch_primitive = CH_EVENT_IND;
	p->ch_event = event;
	p->ch_slot = 0;
	mp->b_wptr += sizeof(*p);

	freemsg(msg);
	mi_strlog(rc->rq, STRLOGTX, SL_TRACE, "<- CH_EVENT_IND");
	putnext(rc->rq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  NS-User to NS-Provider Primitives
 *
 *  -------------------------------------------------------------------------
 */

/** n_error_discard: - discard erroneous primitive
  * @q: active queue
  * @msg: message to free upon success
  * @rc: private structure (locked)
  * @ch: channel structure (might be NULL)
  * @prim: primitive in error
  * @error: error number
  *
  * Only called by n_error_reply(), but then, it can be invoked by the handling procedure for just
  * about any N-primitive received from beneath the module.  State is restored by falling back to
  * the last checkpoint state.
  *
  * Note that the @ch argument is NULL when we could not decypher out N-User tag.  In this case the
  * N-User tag that was issued by the N-provider is present in @msg.
  *
  * The purpose of this function is mostly for logging software fault conditions as any time this
  * function is invoked represents a software fault.
  */
static int
n_error_discard(queue_t *q, mblk_t *msg, struct rc *rc, struct ch *ch, int prim, int error)
{
	/* Restore the total state here because __rc_r_proto() will not restore the state as we are 
	   going to return (0) here. */
	rc_restore_total_state(rc, ch);
	freemsg(msg);
	mi_strlog(rc->wq, STRLOGERR, SL_ERROR, "%s (%s)", np_primname(prim), np_errname(error));
	return (0);
}

/** n_error_reply: - reply with an error
  * @q: active queue
  * @msg: message to free upon success
  * @rc: private structure (locked)
  * @ch: channel structure (might be NULL)
  * @prim: primitive in error
  * @error: error number
  *
  * This is essentially the same as discarding the message except that typical queue return codes
  * are filtered and returned directly.  n_error_reply() and n_error_discard() can be invoked from
  * any N-primitive being received in the upward direction.
  */
static int
n_error_reply(queue_t *q, mblk_t *msg, struct rc *rc, struct ch *ch, int prim, int error)
{
	if (unlikely(error <= 0)) {
		switch (-error) {
		case 0:
		case EBUSY:
		case ENOBUFS:
		case ENOMEM:
		case EAGAIN:
		case EDEADLK:
			return (error);
		default:
			break;
		}
	}
	return n_error_discard(q, msg, rc, ch, prim, error);
}

/** n_conn_req: = issue N_CONN_REQ primitive
  * @q: active queue
  * @msg: message to free upon success
  * @rc: private structure (locked)
  * @ch: channel structure
  * @flags: connect flags
  * @dst_ptr: destination address pointer
  * @dst_len: destination address length
  * @qos_ptr: quality of service parameter pointer
  * @qos_len: quality of service parameter length
  */
static int
n_conn_req(queue_t *q, mblk_t *msg, struct rc *rc, struct ch *ch, int flags, caddr_t dst_ptr,
	   size_t dst_len, caddr_t qos_ptr, size_t qos_len)
{
	N_conn_req_t *p;
	mblk_t *mp;
	int band;
	size_t offs = sizeof(ch->np_tag) + sizeof(*p);
	size_t mlen = offs + dst_len + qos_len;

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;

	/* Signaling messages are band 1 for normal calls and band 3 for priority calls. */
	band = ch ? ((ch->flags & PRIORITY_CALL) ? 3 : 1) : 1;

	if (unlikely(!bcanputnext(rc->wq, band)))
		goto ebusy;

	DB_TYPE(mp) = M_PROTO;
	mp->b_band = band;

	*(typeof(ch->np_tag) *) mp->b_wptr = ch ? ch->np_tag : (typeof(ch->np_tag)) 0;
	mp->b_wptr += sizeof(ch->np_tag);

	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_CONN_REQ;
	p->DEST_length = dst_len;
	p->DEST_offset = offs;
	p->CONN_flags = flags;
	p->QOS_length = qos_len;
	p->QOS_offset = offs + dst_len;
	mp->b_wptr += sizeof(*p);

	bcopy(dst_ptr, mp->b_wptr, dst_len);
	mp->b_wptr += dst_len;
	bcopy(qos_ptr, mp->b_wptr, qos_len);
	mp->b_wptr += qos_len;

	freemsg(msg);
	ch_set_state(rc->wq, ch, CHS_WACK_EREQ);
	np_set_state(rc->wq, ch, NS_WCON_CREQ);
	mi_strlog(rc->wq, STRLOGTX, SL_TRACE, "N_CONN_REQ ->");
	putnext(rc->wq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/** n_conn_res: = issue N_CONN_REQ primitive
  * @q: active queue
  * @msg: message to free upon success
  * @rc: private structure (locked)
  * @ch: channel structure
  */
static int
n_conn_res(queue_t *q, mblk_t *msg, struct rc *rc, struct ch *ch)
{
	N_conn_res_t *p;
	mblk_t *mp;
	int band;

	if (unlikely
	    (!(mp = mi_allocb(q, sizeof(ch->np_tag) + sizeof(*p) + res_len + qos_len, BPRI_MED))))
		goto enobufs;

	/* Signaling messages are band 1 for normal calls and band 3 for priority calls. */
	band = ch ? ((ch->flags & PRIORITY_CALL) ? 3 : 1) : 1;

	if (unlikely(!bcanputnext(rc->wq, band)))
		goto ebusy;

	DB_TYPE(mp) = M_PROTO;
	mp->b_band = band;

	*(typeof(ch->np_tag) *) mp->b_wptr = ch ? ch->np_tag : (typeof(ch->np_tag)) 0;
	mp->b_wptr += sizeof(ch->np_tag);

	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_CONN_RES;
	p->TOKEN_value = token;
	p->RES_length = res_len;
	p->RES_offset = sizeof(ch->np_tag) + sizeof(*p);
	p->SEQ_number = seq;
	p->CONN_flags = flags;
	p->QOS_length = qos_len;
	p->QOS_offset = p->RES_offset + res_len;
	mp->b_wptr += sizeof(*p);

	bcopy(res_ptr, mp->b_wptr, res_len);
	mp->b_wptr += res_len;
	bcopy(qos_ptr, mp->b_wptr, qos_len);
	mp->b_wptr += qos_len;

	freemsg(msg);
	np_set_state(rc->wq, ch, NS_WACK_CRES);
	mi_strlog(rc->wq, STRLOGTX, SL_TRACE, "N_CONN_RES ->");
	putnext(rc->wq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/** n_discon_req: = issue N_DISCON_REQ primitive
  * @q: active queue
  * @msg: message to free upon success
  * @rc: private structure (locked)
  * @ch: channel structure
  * @reason: disconnect reason
  * @seq: sequence number
  * @res_ptr: responding address pointer
  * @res_len: responding address length
  */
static int
n_discon_req(queue_t *q, mblk_t *msg, struct rc *rc, struct ch *ch, np_ulong reason, np_ulong seq,
	     caddr_t res_ptr, size_t res_len)
{
	N_discon_req_t *p;
	mblk_t *mp;
	int band, state;

	if (unlikely(!(mp = mi_allocb(q, sizeof(ch->np_tag) + sizeof(*p) + res_len, BPRI_MED))))
		goto enobufs;

	/* Signaling messages are band 1 for normal calls and band 3 for priority calls. */
	band = ch ? ((ch->flags & PRIORITY_CALL) ? 3 : 1) : 1;

	if (unlikely(!bcanputnext(rc->wq, band)))
		goto ebusy;

	DB_TYPE(mp) = M_PROTO;
	mp->b_band = band;

	*(typeof(ch->np_tag) *) mp->b_wptr = ch ? ch->np_tag : (typeof(ch->np_tag)) 0;
	mp->b_wptr += sizeof(ch->np_tag);

	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_DISCON_REQ;
	p->DISCON_reason = reason;
	p->RES_length = res_len;
	p->RES_offset = sizeof(ch->np_tag) + sizeof(*p);
	p->SEQ_number = seq;
	mp->b_wptr += sizeof(*p);

	bcopy(res_ptr, mp->b_wptr, res_len);
	mp->b_wptr += res_len;

	freemsg(msg);
	switch ((state = np_get_state(ch))) {
	case NS_WCON_CREQ:
		state = NS_WACK_DREQ6;
		break;
	case NS_WRES_CIND:
		state = NS_WACK_DREQ7;
		break;
	case NS_DATA_XFER:
	default:
		state = NS_WACK_DREQ9;
		break;
	case NS_WCON_RREQ:
		state = NS_WACK_DREQ10;
		break;
	case NS_WRES_RIND:
		state = NS_WACK_DREQ11;
		break;
	}
	np_set_state(rc->wq, ch, state);
	mi_strlog(rc->wq, STRLOGTX, SL_TRACE, "N_DISCON_REQ ->");
	putnext(rc->wq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/** n_data_req: = issue N_DATA_REQ primitive
  * @q: active queue
  * @msg: message to free upon success
  * @rc: private structure (locked)
  * @ch: channel structure
  */
static int
n_data_req(queue_t *q, mblk_t *msg, struct rc *rc, struct ch *ch, int flags, mblk_t *dp)
{
	N_data_req_t *p;
	mblk_t *mp;
	int band;

	/* Save the caller the necessity of checking the NPI state.  When in the wrong state, we
	   will simply discard the message. */
	switch (np_get_state(ch)) {
	case NS_DATA_XFER:
	case NS_WCON_RREQ:
	case NS_WRES_RIND:
		break;
	default:
		goto discard;
	}

	if (unlikely(!(mp = mi_allocb(q, sizeof(ch->np_tag) + sizeof(*p), BPRI_MED))))
		goto enobufs;

	/* Data messages are band 0 for normal calls and band 2 for priority calls */
	band = ch ? ((ch->flags & PRIORITY_CALL) ? 2 : 0) : 0;

	if (unlikely(!bcanputnext(rc->wq, band)))
		goto ebusy;

	DB_TYPE(mp) = M_PROTO;
	mp->b_band = band;

	*(typeof(ch->np_tag) *) mp->b_wptr = ch ? ch->np_tag : (typeof(ch->np_tag)) 0;
	mp->b_wptr += sizeof(ch->np_tag);

	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_DATA_REQ;
	p->DATA_xfer_flags = flags;
	mp->b_wptr += sizeof(*p);

	mp->b_cont = dp;
	if (msg == dp)
		msg = NULL;
	if (msg && msg->b_cont == dp)
		msg->b_cont = NULL;

	freemsg(msg);
	mi_strlog(rc->wq, STRLOGDA, SL_TRACE, "N_DATA_REQ ->");
	putnext(rc->wq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
      discard:
	if (msg == dp)
		msg = NULL;
	if (msg && msg->b_cont == dp)
		msg->b_cont = NULL;
	freemsg(msg);
	freemsg(dp);
	return (0);
}

/** n_exdata_req: = issue N_EXDATA_REQ primitive
  * @q: active queue
  * @msg: message to free upon success
  * @rc: private structure (locked)
  * @ch: channel structure
  */
static int
n_exdata_req(queue_t *q, mblk_t *msg, struct rc *rc, struct ch *ch)
{
	N_exdata_req_t *p;
	mblk_t *mp;
	int band;

	/* Save the caller the necessity of checking the NPI state.  When in the wrong state, we
	   will simply discard the message. */
	switch (np_get_state(ch)) {
	case NS_DATA_XFER:
	case NS_WCON_RREQ:
	case NS_WRES_RIND:
		break;
	default:
		goto discard;
	}

	if (unlikely(!(mp = mi_allocb(q, sizeof(ch->np_tag) + sizeof(*p), BPRI_MED))))
		goto enobufs;

	/* Expedited data messages are band 1 for normal calls and band 3 for priority calls */
	band = ch ? ((ch->flags & PRIORITY_CALL) ? 3 : 1) : 1;

	if (unlikely(!bcanputnext(rc->wq, band)))
		goto ebusy;

	DB_TYPE(mp) = M_PROTO;
	mp->b_band = band;

	*(typeof(ch->np_tag) *) mp->b_wptr = ch ? ch->np_tag : (typeof(ch->np_tag)) 0;
	mp->b_wptr += sizeof(ch->np_tag);

	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_EXDATA_REQ;
	mp->b_wptr += sizeof(*p);

	mp->b_cont = dp;
	if (msg == dp)
		msg = NULL;
	if (msg && msg->b_cont == dp)
		msg->b_cont = NULL;

	freemsg(msg);
	mi_strlog(rc->wq, STRLOGDA, SL_TRACE, "N_EXDATA_REQ ->");
	putnext(rc->wq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
      discard:
	if (msg == dp)
		msg = NULL;
	if (msg && msg->b_cont == dp)
		msg->b_cont = NULL;
	freemsg(msg);
	freemsg(dp);
	return (0);
}

/** n_info_req: = issue N_INFO_REQ primitive
  * @q: active queue
  * @msg: message to free upon success
  * @rc: private structure (locked)
  * @ch: channel structure
  *
  * Note that we only issue information requests in response to an information
  * request from above, soe we always issue these primitives as M_PCPROTO
  * because we want an immediate response.  This saves us from saving addresses
  * in our own structures.
  */
static int
n_info_req(queue_t *q, mblk_t *msg, struct rc *rc, struct ch *ch)
{
	N_info_req_t *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(ch->np_tag) + sizeof(*p), BPRI_MED))))
		goto enobufs;

	DB_TYPE(mp) = M_PCPROTO;

	*(typeof(ch->np_tag) *) mp->b_wptr = ch ? ch->np_tag : (typeof(ch->np_tag)) 0;
	mp->b_wptr += sizeof(ch->np_tag);

	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_INFO_REQ;
	mp->b_wptr += sizeof(*p);

	freemsg(msg);
	mi_strlog(rc->wq, STRLOGTX, SL_TRACE, "N_INFO_REQ ->");
	putnext(rc->wq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/** n_bind_req: = issue N_BIND_REQ primitive
  * @q: active queue
  * @msg: message to free upon success (must be CH_ATTACH_REQ)
  * @rc: private structure (locked)
  * @flags: bind flags
  * @coninds: number of connection indications
  * @add_ptr: address to bind pointer
  * @add_len: address to bind length
  * @pro_ptr: protocol options to attach pointer
  * @pro_len: protoocl options to attach length
  *
  * Note that this routine is responsible for allocating the channel structure.
  */
static int
n_bind_req(queue_t *q, mblk_t *msg, struct rc *rc, int flags, int coninds, caddr_t add_ptr,
	   size_t add_len, caddr_t pro_ptr, size_t pro_len)
{
	N_bind_req_t *p;
	mblk_t *mp;
	struct ch *ch;
	size_t mlen = sizeof(ch->np_tag) + sizeof(*p) + add_len + pro_len;

	if (unlikely(!(ch = ch_alloc())))
		goto enomem;

	ch->flags = 0;
	ch->ch_tag = *(typeof(ch->ch_tag) *) msg->b_rptr;
	ch->np_tag = 0;		/* not known yet */
	ch->ch_state = CHS_UNINIT;
	ch->np_state = NS_UNBND;

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;

	/* We issue these as priority messages because we want an immediate response. */
	DB_TYPE(mp) = M_PCPROTO;

	/* N_BIND_REQ always has the NS user tag. */
	*(typeof(ch->np_tag) *) mp->b_wptr = (typeof(ch->np_tag)) ch;
	mp->b_wptr += sizeof(ch->np_tag);

	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_BIND_REQ;
	p->ADDR_length = add_len;
	p->ADDR_offset = sizeof(ch->np_tag) + sizeof(*p);
	p->CONIND_number = coninds;
	p->BIND_flags = flags;
	p->PROTOID_length = pro_len;
	p->PROTOID_offset = p->ADDR_offset + add_len;
	mp->b_wptr += sizeof(*p);

	bcopy(add_ptr, mp->b_wptr, add_len);
	mp->b_wptr += add_len;
	bcopy(pro_ptr, mp->b_wptr, pro_len);
	mp->b_wptr += pro_len;

	freemsg(msg);
	ch_set_state(rc->irc, ch, CHS_WACK_AREQ);
	np_set_state(rc->wq, ch, NS_WACK_BREQ);
	mi_strlog(rc->wq, STRLOGTX, SL_TRACE, "N_BIND_REQ ->");
	putnext(rc->wq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/** n_unbind_req: = issue N_UNBIND_REQ primitive
  * @q: active queue
  * @msg: message to free upon success
  * @rc: private structure (locked)
  * @ch: channel structure
  */
static int
n_unbind_req(queue_t *q, mblk_t *msg, struct rc *rc, struct ch *ch)
{
	N_unbind_req_t *p;
	mblk_t *mp;
	int band;

	if (unlikely(!(mp = mi_allocb(q, sizeof(ch->np_tag) + sizeof(*p), BPRI_MED))))
		goto enobufs;

	/* We issue these as priority messages because we want an immediate response. */
	DB_TYPE(mp) = M_PCPROTO;

	*(typeof(ch->np_tag) *) mp->b_wptr = ch ? ch->np_tag : (typeof(ch->np_tag)) 0;
	mp->b_wptr += sizeof(ch->np_tag);

	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_UNBIND_REQ;
	mp->b_wptr += sizeof(*p);

	freemsg(msg);
	np_set_state(rc->wq, ch, NS_WACK_UREQ);
	mi_strlog(rc->wq, STRLOGTX, SL_TRACE, "N_UNBIND_REQ ->");
	putnext(rc->wq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);

}

/** n_unitdata_req: = issue N_UNITDATA_REQ primitive
  * @q: active queue
  * @msg: message to free upon success
  * @rc: private structure (locked)
  * @ch: channel structure
  */
static int
n_unitdata_req(queue_t *q, mblk_t *msg, struct rc *rc, struct ch *ch)
{
	N_unitdata_req_t *p;
	mblk_t *mp;
	int band;

	/* Save the caller the necessity of checking the NPI state.  When in the wrong state we
	   will simply discard the message. */
	switch (np_get_state(ch)) {
	case NS_IDLE:
		break;
	default:
		goto discard;
	}

	if (unlikely(!(mp = mi_allocb(q, sizeof(ch->np_tag) + sizeof(*p) + dst_len, BPRI_MED))))
		goto enobufs;

	/* Data messages are band 0 for normal calls and band 2 for priority calls */
	band = ch ? ((ch->flags & PRIORITY_CALL) ? 2 : 0) : 0;

	if (unlikely(!bcanputnext(rc->wq, band)))
		goto ebusy;

	DB_TYPE(mp) = M_PROTO;
	mp->b_band = band;

	*(typeof(ch->np_tag) *) mp->b_wptr = ch ? ch->np_tag : (typeof(ch->np_tag)) 0;
	mp->b_wptr += sizeof(ch->np_tag);

	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_UNITDATA_REQ;
	p->DEST_length = dst_len;
	p->DEST_offset = sizeof(ch->np_tag) + sizeof(*p);
	p->RESERVED_field[0] = 0;
	p->RESERVED_field[1] = 0;
	mp->b_wptr += sizeof(*p);

	bcopy(dst_ptr, mp->b_wptr, dst_len);
	mp->b_wptr += dst_len;

	mp->b_cont = dp;
	if (msg == dp)
		msg = NULL;
	if (msg && msg->b_cont == dp)
		msg->b_cont = NULL;

	freemsg(msg);
	mi_strlog(rc->wq, STRLOGDA, SL_TRACE, "N_UNITDATA_REQ ->");
	putnext(rc->wq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
      discard:
	if (msg == dp)
		msg = NULL;
	if (msg && msg->b_cont == dp)
		msg->b_cond = NULL;
	freemsg(msg);
	freemsg(dp);
	return (0);
}

/** n_optmgmt_req: = issue N_OPTMGMT_REQ primitive
  * @q: active queue
  * @msg: message to free upon success
  * @rc: private structure (locked)
  * @ch: channel structure
  */
static int
n_optmgmt_req(queue_t *q, mblk_t *msg, struct rc *rc, struct ch *ch)
{
	N_optmgmt_req_t *p;
	mblk_t *mp;

	if (!(mp = mi_allocb(q, sizeof(ch->np_tag) + sizeof(*p) + qos_len, BPRI_MED)))
		goto enobufs;

	DB_TYPE(mp) = M_PCPROTO;

	*(typeof(ch->np_tag) *) mp->b_wptr = ch ? ch->np_tag : (typeof(ch->np_tag)) 0;
	mp->b_wptr += sizeof(ch->np_tag);

	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_OPTMGMT_REQ;
	p->QOS_length = qos_len;
	p->QOS_offset = sizeof(ch->np_tag) + sizeof(*p);
	p->OPTMGMT_flags = flags;
	mp->b_wptr += sizeof(*p);

	bcopy(qos_ptr, mp->b_wptr, qos_len);
	mp->b_wptr += qos_len;

	freemsg(msg);
	if (np_get_state(ch) == NS_IDLE)
		np_set_state(rc->wq, ch, NS_WACK_OPTREQ);
	mi_strlog(rc->wq, STRLOGTX, SL_TRACE, "N_OPTMGMT_REQ ->");
	putnext(rc->wq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/** n_datack_req: = issue N_DATACK_REQ primitive
  * @q: active queue
  * @msg: message to free upon success
  * @rc: private structure (locked)
  * @ch: channel structure
  */
static int
n_datack_req(queue_t *q, mblk_t *msg, struct rc *rc, struct ch *ch)
{
	N_datack_req_t *p;
	mblk_t *mp;
	int band;

	/* Save the caller the necessity of checking the NPI state.  When in the wrong state, we
	   will simply discard the message. */
	switch (np_get_state(ch)) {
	case NS_DATA_XFER:
	case NS_WCON_RREQ:
	case NS_WRES_RIND:
		break;
	default:
		goto discard;
	}

	if (unlikely(!(mp = mi_allocb(q, sizeof(ch->np_tag) + sizeof(*p), BPRI_MED))))
		goto enobufs;

	/* Data messages are band 0 for normal calls and band 2 for priority calls. */
	band = ch ? ((ch->flags & PRIORITY_CALL) ? 2 : 0) : 0;

	if (unlikely(!bcanputnext(rc->wq, band)))
		goto ebusy;

	DB_TYPE(mp) = M_PROTO;
	mp->b_band = band;

	*(typeof(ch->np_tag) *) mp->b_wptr = ch ? ch->np_tag : (typeof(ch->np_tag)) 0;
	mp->b_wptr += sizeof(ch->np_tag);

	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_DATACK_REQ;
	mp->b_wptr += sizeof(*p);

	freemsg(msg);
	mi_strlog(rc->wq, STRLOGDA, SL_TRACE, "N_DATACK_REQ ->");
	putnext(rc->wq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
      discard:
	freemsg(msg);
	return (0);
}

/** n_reset_req: = issue N_RESET_RES primitive
  * @q: active queue
  * @msg: message to free upon success
  * @rc: private structure (locked)
  * @ch: channel structure
  */
static int
n_reset_req(queue_t *q, mblk_t *msg, struct rc *rc, struct ch *ch)
{
	N_reset_req_t *p;
	mblk_t *mp;
	int band;

	if (unlikely(!(mp = mi_allocb(q, sizeof(ch->np_tag) + sizeof(*p), BPRI_MED))))
		goto enobufs;

	/* Data messages are band 0 for normal calls and band 2 for priority calls */
	band = ch ? ((ch->flags & PRIORITY_CALL) ? 2 : 0) : 0;

	if (unlikely(!bcanputnext(rc->wq, band)))
		goto ebusy;

	DB_TYPE(mp) = M_PROTO;
	mp->b_band = band;

	*(typeof(ch->np_tag) *) mp->b_wptr = ch ? ch->np_tag : (typeof(ch->np_tag)) 0;
	mp->b_wptr += sizeof(ch->np_tag);

	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_RESET_REQ;
	p->RESET_reason = reason;
	mp->b_wptr += sizeof(*p);

	freemsg(msg);
	np_set_state(rc->wq, ch, NS_WCON_RREQ);
	mi_strlog(rc->wq, STRLOGDA, SL_TRACE, "N_RESET_REQ ->");
	putnext(rc->wq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/** n_reset_res: = issue N_RESET_RES primitive
  * @q: active queue
  * @msg: message to free upon success
  * @rc: private structure (locked)
  * @ch: channel structure
  */
static int
n_reset_res(queue_t *q, mblk_t *msg, struct rc *rc, struct ch *ch)
{
	N_reset_res_t *p;
	mblk_t *mp;
	int band;

	if (unlikely(!(mp = mi_allocb(q, sizeof(ch->np_tag) + sizeof(*p), BPRI_MED))))
		goto enobufs;

	/* Data messages are band 0 for normal calls and band 2 for priority calls */
	band = ch ? ((ch->flags & PRIORITY_CALL) ? 2 : 0) : 0;

	if (unlikely(!bcanputnext(rc->wq, band)))
		goto ebusy;

	DB_TYPE(mp) = M_PROTO;
	mp->b_band = band;

	*(typeof(ch->np_tag) *) mp->b_wptr = ch ? ch->np_tag : (typeof(ch->np_tag)) 0;
	mp->b_wptr += sizeof(ch->np_tag);

	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_RESET_RES;
	mp->b_wptr += sizeof(*p);

	freemsg(msg);
	np_set_state(rc->wq, ch, NS_WACK_RRES);
	mi_strlog(rc->wq, STRLOGDA, SL_TRACE, "N_RESET_RES ->");
	putnext(rc->wq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  CH-User to CH-Provider Primitives
 *
 *  -------------------------------------------------------------------------
 */

/** ch_info_req: - process CH_INFO_REQ primitive
  * @q: active queue
  * @mp: the CH_INFO_REQ primitive
  * @rc: private structure (locked)
  * @ch: channel structure
  */
static int
ch_info_req(queue_t *q, mblk_t *mp, struct rc *rc, struct ch *ch)
{
	/* Addresses are provided per channel. */
	if (likely(ch != NULL))
		/* We don't keep addresses, but they are available from the lower level driver.  So 
		   issue an N_INFO_REQ to retrieve them and when they are available responsd to the 
		   CH_INFO_REQ. */
		return n_info_req(q, mp, rc, ch);

	return ch_info_ack(rc, ch, q, mp, NULL, 0, (caddr_t) &rc->parms, sizeof(rc->parms));
}

/** ch_optmgmt_req: - process CH_OPTMGMT_REQ primitive
  * @q: active queue
  * @mp: the CH_OPTMGMT_REQ primitive
  * @rc: private structure (locked)
  * @ch: channel structure
  */
static int
ch_optmgmt_req(queue_t *q, mblk_t *mp, struct rc *rc, struct ch *ch)
{
}

/** ch_attach_req: - process CH_ATTACH_REQ primitive
  * @q: active queue
  * @mp: the CH_ATTACH_REQ primitive
  * @rc: private structure (locked)
  * @ch: channel structure (always NULL)
  *
  * Note that the CH_ATTACH_REQ always creates a new channel structure.  The tag on the primitive is
  * the CHS-User ID for the channel.  The CH_ATTACH_ACK tag will contain the CHS-User ID for the
  * channel prepended, but will also contain the CHS-Provider ID for the channel appended.
  *
  * The CH_ATTACH_REQ results in the issuing of a N_BIND_REQ primitve to the lower layer.
  *
  * Note that the @ch argument is always NULL coming into this function.  It is the n_bind_req()
  * function that actually allocates the @ch structure.  When an error occurs, ch_error_reply()
  * knows that if the @ch argument is NULL, that the CHS-User tag can be extracted from the @msg
  * argument.
  *
  * There are two types of attaches that can be performed.  One permits received packets from any
  * source to be delivered on the channel.  The other does not permit any received packets to be
  * delivered: a connection must first be formed.  The difference between the two is distringuished
  * by attach flags in the ch_flags member. The first type cannot be connected in the transmit
  * direction until after a full connect has been performed.
  */
static int
ch_attach_req(queue_t *q, mblk_t *mp, struct rc *rc, struct ch *ch)
{
	CH_attach_req_t *p;
	int err;
	caddr_t add_ptr = NULL;
	size_t add_len = 0;

	if (!MBLKIN(mp, sizeof(ch->ch_tag), sizeof(*p)))
		goto emsgsize;

	p = (typeof(p)) (mp->b_rptr + sizeof(ch->ch_tag));

	if (!MBLKIN(mp, p->ch_addr_offset, p->ch_addr_length))
		goto emsgsize;

	/* perhaps we should let the lower layer decide whether the address is valid or not */
	if (p->ch_addr_length != sizeof(ch->addr))
		goto badaddr;

	add_ptr = mp->b_rptr + p->ch_addr_offset;
	add_len = p->ch_addr_length;

	return n_bind_req(q, mp, rc, 0, 0, add_ptr, add_len, NULL, 0);
      enomem:
	err = -ENOMEM;
	goto error;
      badaddr:
	err = CHBADADDR;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return ch_error_reply(q, mp, rc, ch, CH_ATTACH_REQ, err);
}

/** ch_enable_req: - process CH_ENABLE_REQ primitive
  * @rc: private structure (locked)
  * @ch: channel structure
  * @q: active queue
  * @mp: the CH_ENABLE_REQ primitive
  */
static int
ch_enable_req(queue_t *q, mblk_t *mp, struct rc *rc, struct ch *ch)
{
	CH_enable_req_t *p;
	int err;
	caddr_t dst_ptr = NULL;
	size_t dst_len = 0;

	if (ch_get_state(ch) != CHS_ATTACHED)
		goto outstate;
	if (!MBLKIN(mp, sizeof(ch->ch_tag), sizeof(*p)))
		goto emsgsize;
	p = (typeof(p)) (mp->b_rptr + sizeof(ch->ch_tag));

	if (!MBLKIN(mp, p->ch_addr_offset, p->ch_addr_length))
		goto emgsize;
	if (p->ch_addr_length) {
		dst_ptr = mp->b_rptr + p->ch_addr_offset;
		dst_len = p->ch_addr_length;
	}
	return n_conn_req(q, mp, rc, ch, 0, dst_ptr, dst_len, NULL, 0);
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      outstate:
	err = CHOUTSTATE;
	goto error;
      error:
	return n_error_reply(q, mp, rc, ch, CH_ENABLE_REQ, err);

}

/** ch_connect_req: - process CH_CONNECT_REQ primitive
  * @rc: private structure (locked)
  * @ch: channel structure
  * @q: active queue
  * @mp: the CH_CONNECT_REQ primitive
  *
  * There are two types of RTP media streams: one that is fully specified, has both local and remote
  * addresses, and one that is only partially specified, has only local address specified.  This
  * first form (fully specified) can be connected in either or both directions.  The second form
  * (partialy specified) can only be connected in both directions, but can only direct packets in
  * the transmit direction once it is fully specified.
  *
  * A partially specified RTP media stream can be modified into a fully specified RTP media stream
  * using the CH_OPTMGMT_REQ primitive.
  */
static int
ch_connect_req(queue_t *q, mblk_t *mp, struct rc *rc, struct ch *ch)
{
	CH_connect_req_t *p;
	mblk_t *mp;
	int err, flags;

	if (!MBLKIN(mp, sizeof(ch->ch_tag), sizeof(*p)))
		goto emsgsize;
	p = (typeof(p)) (mp->b_rptr + sizeof(ch->ch_tag));
	if ((flags = p->ch_conn_flags) & ~CHF_BOTH_DIR)
		goto badflag;
	switch (ch_get_state(ch)) {
	case CHS_ENABLED:
	case CHS_CONNECTED:
		break;
	default:
		goto outstate;
	}
	ch->flags |= flags;
	flags = ch->flags & CHF_BOTH_DIR;
	ch_set_state(q, ch, CHS_WCON_CREQ);
	return ch_connect_con(q, mp, rc, ch, flags);
      outstate:
	err = CHOUTSTATE;
	goto error;
      badflag:
	err = CHBADFLAG;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return ch_error_reply(q, mp, rc, ch, CH_CONNECT_REQ, err);
}

/** ch_write: - write data
  * @q: active queue
  * @mp: the M_DATA message
  * @rc: private structure (locked)
  * @ch: channel structure
  */
static int
ch_write(queue_t *q, mblk_t *mp, struct rc *rc, struct ch *ch)
{
	int err;

	switch (__builtin_expect((ch_get_state(ch), CHS_CONNECTED))) {
	case CHS_CONNECTED:
		break;
	case CHS_WACK_DREQ:
	case CHS_ENABLED:
	case CHS_WACK_UREQ:
		goto discard;
	default:
		goto outstate;
	}
	if (unlikely(!(ch->flags & CHF_TX_DIR)))
		goto discard;
	/* FIXME: this is not correct yet.  What this does is simply forward the data upstream.  It 
	   has to be manipulated first.  Also, it would be pointless to convert the payload only to 
	   find that stream is flow controlled or that a message block cannot be allocated.  We
	   should adjust ch_data_ind to return the M_PROTO portion of the message block without the 
	   data attached. */
	if (!bcanputnext(rc->wq, mp->b_band))
		return (-EBUSY);
	*(typeof(ch->np_tag) *) mp->b_rptr = ch->np_tag;
	mi_strlog(rc->wq, STRLOGDA, SL_TRACE, "M_DATA ->");
	putnext(rc->wq, mp);
	return (0);
      outstate:
	err = CHOUTSTATE;
	goto error;
      error:
	return ch_reply_error(q, mp, rc, ch, CH_DATA_REQ, err);
      discard:
	freemsg(mp);
	return (0);
}

/** ch_data_req: - process CH_DATA_REQ primitive
  * @q: active queue
  * @mp: the CH_DATA_REQ primitive
  * @rc: private structure (locked)
  * @ch: channel structure
  *
  * We can receive data requests for channels that are fully specified (have local and remote
  * addresses).  The channel must be connected in the transmit direction or we will simply discard
  * them.
  */
static int
ch_data_req(queue_t *q, mblk_t *mp, struct rc *rc, struct ch *ch)
{
	CH_data_req_t *p;
	int err;

	if (unlikely(!MBLKIN(mp, sizeof(ch->ch_tag), sizeof(*p))))
		goto emsgsize;
	switch (__builtin_expect((ch_get_state(ch), CHS_CONNECTED))) {
	case CHS_CONNECTED:
		break;
	case CHS_WACK_DREQ:
	case CHS_ENABLED:
	case CHS_WACK_UREQ:
		goto discard;
	default:
		goto outstate;
	}
	if (unlikely(!(ch->flags & CHF_TX_DIR)))
		goto discard;
	/* FIXME: this is not correct yet.  What this does is simply forward the data downstream.
	   It has to be manipulated first.  Also, it would be pointless to convert the payload only 
	   to find that stream is flow controlled or that a message block cannot be allocated We
	   should adjust n_data_req to return the M_PROTO portion of the message block without the
	   data attached. */
	return n_data_req(q, mp, rc, ch, 0, mp->b_cont);
      outstate:
	err = CHOUTSTATE;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return ch_error_reply(q, mp, rc, ch, CH_DATA_REQ, err);
      discard:
	freemsg(mp);
	return (0);
}

/** ch_disconnect_req: - process CH_DISCONNECT_REQ primitive
  * @q: active queue
  * @mp: the CH_DISCONNECT_REQ primitive
  * @rc: private structure (locked)
  * @ch: channel structure
  */
static int
ch_disconnect_req(queue_t *q, mblk_t *mp, struct rc *rc, struct ch *ch)
{
}

/** ch_disable_req: - process CH_DISABLE_REQ primitive
  * @q: active queue
  * @mp: the CH_DISABLE_REQ primitive
  * @rc: private structure (locked)
  * @ch: channel structure
  */
static int
ch_disable_req(queue_t *q, mblk_t *mp, struct rc *rc, struct ch *ch)
{
}

/** ch_detach_req: - process CH_DETACH_REQ primitive
  * @q: active queue
  * @mp: the CH_DETACH_REQ primitive
  * @rc: private structure (locked)
  * @ch: channel structure
  */
static int
ch_detach_req(queue_t *q, mblk_t *mp, struct rc *rc, struct ch *ch)
{
}

/*
 *  -------------------------------------------------------------------------
 *
 *  NS-Provider to NS-User Primitives
 *
 *  -------------------------------------------------------------------------
 */

/** n_conn_ind: - process N_CONN_IND primitive
  * @q: active queue
  * @mp: the N_CONN_IND primitive
  * @rc: private structure (locked)
  * @ch: channel structure
  *
  * We don't normally permit N_CONN_IND and they shouldn't be delivered to us.  The proper response
  * is to send an N_DISCON_REQ in response.
  */
static int
n_conn_ind(queue_t *q, mblk_t *mp, struct rc *rc, struct ch *ch)
{
	N_conn_ind_t *p;
	int err;
	caddr_t res_ptr = NULL;
	size_t res_len = 0;

	if (unlikely(!MBLKIN(mp, sizeof(ch->np_tag), sizeof(*p))))
		goto emsgsize;

	p = (typeof(p)) (mp->b_rptr + sizeof(ch->np_tag));

	if (unlikely(!MBLKIN(mp, p->DEST_offset, p->DEST_length)))
		goto emsgsize;
	if (p->DEST_length) {
		res_ptr = mp->b_rptr + p->DEST_offset;
		res_len = p->DEST_length;
	}
	np_set_state(rc->wq, ch, NS_WRES_CIND);
	return n_discon_req(q, mp, rc, ch, N_REJ_P, p->SEQ_number, res_ptr, res_len);

      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return n_error_reply(q, mp, rc, ch, N_CONN_IND, err);
}

/** n_conn_con: - process N_CONN_CON primitive
  * @q: active queue
  * @mp: the N_CONN_CON primitive
  * @rc: private structure (locked)
  * @ch: channel structure
  *
  * The N_CONN_CON primitive maps directly into the CH_ENABLE_CON primitive.
  */
static int
n_conn_con(queue_t *q, mblk_t *mp, struct rc *rc, struct ch *ch)
{
	np_set_state(rc->wq, ch, NS_DATA_XFER);
	return ch_enable_con(q, mp, rc, ch);
}

/** n_discon_ind: - process N_DISCON_IND primitive
  * @q: active queue
  * @mp: the N_DISCON_IND primitive
  * @rc: private structure (locked)
  * @ch: channel structure
  *
  * Autonomous disconnect.  This does not happen for the RTP drivers used below this module, so this
  * function is moot.  Nevertheless and autonomous disconnect maps into a CH_DISCONNECT_IND and a
  * CH_DISABLE_IND.  The channel stays attached.
  */
static int
n_discon_ind(queue_t *q, mblk_t *mp, struct rc *rc, struct ch *ch)
{
	int err;

	switch (ch_get_state(ch)) {
	case CHS_CONNECTED:
		if ((err = ch_disconnect_ind(q, NULL, rc, ch)))
			return (err);
		/* fall through */
	case CHS_ENABLED:
		if ((err = ch_disable_ind(q, NULL, rc, ch)))
			return (err);
		break;
	default:
		goto outstate;
	}
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return n_error_reply(q, mp, rc, ch, N_DISCON_IND, err);
}

/** n_read: - read data
  * @q: active queue
  * @mp: the M_DATA message
  * @rc: private structure (locked)
  * @ch: channel structure
  */
static int
n_read(queue_t *q, mblk_t *mp, struct rc *rc, struct ch *ch)
{
	int err;

	switch (__builtin_expect((np_get_state(ch), NS_DATA_XFER))) {
	case NS_DATA_XFER:
	case NS_WRES_RIND:
	case NS_WACK_RRES:
	case NS_WCON_RREQ:
	case NS_WACK_DREQ9:
	case NS_WACK_DREQ10:
	case NS_WACK_DREQ11:
		break;
	case NS_IDLE:
		goto discard;
	default:
		goto outstate;
	}
	/* one of the following is redundant */
	if (unlikely(ch_get_state(ch) != CHS_CONNECTED))
		goto discard;
	if (unlikely(!(ch->flags & CHF_RX_DIR)))
		goto discard;
	/* FIXME: this is not correct yet.  What this does is simply forward the data upstream.  It 
	   has to be manipulated first.  Also, it would be pointless to convert the payload only to 
	   find that stream is flow controlled or that a message block cannot be allocated.  We
	   should adjust ch_data_ind to return the M_PROTO portion of the message block without the 
	   data attached. */
	if (!bcanputnext(rc->rq, mp->b_band))
		return (-EBUSY);
	*(typeof(ch->ch_tag) *) mp->b_rptr = ch->ch_tag;
	mi_strlog(rc->rq, STRLOGDA, SL_TRACE, "<- M_DATA");
	putnext(rc->rq, mp);
	return (0);
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return n_reply_error(q, mp, rc, ch, N_DATA_IND, err);
      discard:
	freemsg(mp);
	return (0);
}

/** n_data_ind: - process N_DATA_IND primitive
  * @q: active queue
  * @mp: the N_DATA_IND primitive
  * @rc: private structure (locked)
  * @ch: channel structure
  *
  * We can received data indications for channels that are fully specified (have local and remote
  * addresses).  The channel must be connected in the receive direction or we will simply discard
  * them.
  */
static int
n_data_ind(queue_t *q, mblk_t *mp, struct rc *rc, struct ch *ch)
{
	N_data_ind_t *p;
	int err;

	if (unlikely(!MBLKIN(mp, sizeof(ch->np_tag), sizeof(*p))))
		goto emsgsize;
	switch (__builtin_expect((np_get_state(ch), NS_DATA_XFER))) {
	case NS_DATA_XFER:
	case NS_WRES_RIND:
	case NS_WACK_RRES:
	case NS_WCON_RREQ:
	case NS_WACK_DREQ9:
	case NS_WACK_DREQ10:
	case NS_WACK_DREQ11:
		break;
	case NS_IDLE:
		goto discard;
	default:
		goto outstate;
	}
	/* one of the following is redundant */
	if (unlikely(ch_get_state(ch) != CHS_CONNECTED))
		goto discard;
	if (unlikely(!(ch->flags & CHF_RX_DIR)))
		goto discard;
	/* FIXME: this is not correct yet.  What this does is simply forward the data upstream.  It 
	   has to be manipulated first.  Also, it would be pointless to convert the payload only to 
	   find that stream is flow controlled or that a message block cannot be allocated.  We
	   should adjust ch_data_ind to return the M_PROTO portion of the message block without the 
	   data attached. */
	return ch_data_ind(q, mp, rc, ch, mp->b_cont);
      outstate:
	err = NOUTSTATE;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return n_reply_error(q, mp, rc, ch, N_DATA_IND, err);
      discard:
	freemsg(mp);
	return (0);
}

/** n_exdata_ind: - process N_EXDATA_IND primitive
  * @q: active queue
  * @mp: the N_EXDATA_IND primitive
  * @rc: private structure (locked)
  * @ch: channel structure
  *
  * We can receive expedited data indications for channels that are fully specified (have local and
  * remote addresses).  The channel must be connected in the receive direction or we will simply
  * discard them.
  */
static int
n_exdata_ind(queue_t *q, mblk_t *mp, struct rc *rc, struct ch *ch)
{
	N_exdata_ind_t *p;
	int err;

	if (unlikely(!MBLKIN(mp, sizeof(ch->np_tag), sizeof(*p))))
		goto emsgsize;
	switch (__builtin_expect((np_get_state(ch), NS_DATA_XFER))) {
	case NS_DATA_XFER:
	case NS_WRES_RIND:
	case NS_WACK_RRES:
	case NS_WCON_RREQ:
	case NS_WACK_DREQ9:
	case NS_WACK_DREQ10:
	case NS_WACK_DREQ11:
		break;
	case NS_IDLE:
		goto discard;
	default:
		goto outstate;
	}
	/* one of the following is redundant */
	if (unlikely(ch_get_state(ch) != CHS_CONNECTED))
		goto discard;
	if (unlikely(!(ch->flags & CHF_RX_DIR)))
		goto discard;
	/* FIXME: this is not correct yet.  What this does is simply forward the data upstream.  It 
	   has to be manipulated first.  Also, it would be pointless to convert the payload only to 
	   find that stream is flow controlled or that a message block cannot be allocated.  We
	   should adjust ch_data_ind to return the M_PROTO portion of the message block without the 
	   data attached. */
	return ch_data_ind(q, mp, rc, ch, mp->b_cont);
      outstate:
	err = NOUTSTATE;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return n_reply_error(q, mp, rc, ch, N_DATA_IND, err);
      discard:
	freemsg(mp);
	return (0);
}

/** n_info_ack: - process N_INFO_ACK primitive
  * @q: active queue
  * @mp: the N_INFO_ACK primitive
  * @rc: private structure (locked)
  * @ch: channel structure
  *
  * An N_INFO_ACK primitive maps directly into a CH_INFO_ACK primitive.  The reason we generate the
  * N_INFO_REQ in the first place is because we want to leave the address maintained by the lower
  * level (so we don't duplicate 64,000 of them)!  Circuit parameters, on the other hand, are
  * maintained by the Stream, once for all multi-Streams.
  */
static int
n_info_ack(queue_t *q, mblk_t *mp, struct rc *rc, struct ch *ch)
{
	N_info_ack_t *p;
	int err;
	caddr_t add_ptr = NULL, par_ptr = (caddr_t) &rc->parms;
	size_t add_len = 0, par_len = sizeof(rc->parms);

	if (unlikely(!MBLKIN(mp, sizeof(ch->np_tag), sizeof(*p))))
		goto emsgsize;

	p = (typeof(p)) (mp->b_rptr + sizeof(ch->np_tag));

	if (unlikely(!MBLKIN(mp, p->ADDR_offset, p->ADDR_length)))
		goto emsgsize;
	if (likely(p->ADDR_length != 0)) {
		add_ptr = mp->b_rptr + p->ADDR_offset;
		add_len = p->ADDR_length;
	}
#if 0
	if (unlikely(!MBLKIN(mp, p->QOS_offset, p->QOS_length)))
		goto emsgsize;
	if (unlikely(!MBLKIN(mp, p->QOS_range_offset, p->QOS_range_length)))
		goto emsgsize;
#endif
	return ch_info_ack(q, mp, rc, ch, add_ptr, add_len, par_ptr, par_len);
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return n_reply_error(q, mp, rc, ch, N_INFO_ACK, err);
}

/** n_bind_ack: - process N_BIND_ACK primitive
  * @q: active queue
  * @mp: the N_BIND_ACK primitive
  * @rc: private structure (locked)
  * @ch: channel structure
  *
  * Note that the N_BIND_ACK is a little different when doing multi-Streams.  The tag prepended to
  * the primitive is the NS-User tag (which is a pointer to the ch structure) and the NS-Provider
  * tag is appended to the primitive.  This NS-Provider tag must be saved and prepended to
  * subsequent NS-Uer issued primitives for the same virtual interface.
  */
static int
n_bind_ack(queue_t *q, mblk_t *mp, struct rc *rc, struct ch *ch)
{
	N_bind_ack_t *p;
	int err;
	caddr_t add_ptr = NULL;
	size_t add_len = 0;

	if (unlikely((np_get_state(ch) != NS_WACK_BREQ)))
		goto outstate;
	if (unlikely(!MBLKIN(mp, sizeof(ch->np_tag), sizeof(*p))))
		goto emsgsize;
	p = (typeof(p)) (mp->b_rptr + sizeof(ch->np_tag));
	if (p->ADDR_length) {
		/* sanity check address */
		if (unlikely(!MBLKIN(mp, p->ADDR_offset, p->ADDR_length)))
			goto emsgsize;
		if (p->ADDR_offset < sizeof(*p) + 2 * sizeof(ch->np_tag))
			goto einval;
		/* We do not save the address as we can request it at any time with N_INFO_REQ, but 
		   they are passed upward in the CH_ATTACH_ACK. */

		add_ptr = mp->b_rptr + p->ADDR_offset;
		add_len = p->ADDR_length;
	}
	if (p->PROTOID_length) {
		/* sanity check protocol ids */
		if (unlikely(!MBLKIN(mp, p->PROTOID_offset, p->PROTOID_length)))
			goto emsgsize;
		if (p->PROTOID_offset < sizeof(*p) + 2 * sizeof(ch->np_tag))
			goto einval;
		/* We do not save the protocol ids as we can request it at any time with
		   N_INFO_REQ. */
	}
	return ch_attach_ack(q, mp, rc, ch, 0, add_ptr, add_len);
      einval:
	err = -EINVAL;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return n_reply_error(q, mp, rc, ch, N_BIND_ACK, err);
}

/** n_error_ack: - process N_ERROR_ACK primitive
  * @q: active queue
  * @mp: the N_ERROR_ACK primitive
  * @rc: private structure (locked)
  * @ch: channel structure
  *
  * There are two ways of approaching state machine interworking.
  *
  * 1. One is to terminate both interfaces using a separate state vairable for CHI and NPI and
  *    tracking each interface.  This causes problems in errors situations where the states do not
  *    syncrhonize either because of user intput or as result of errors in interpretation of the
  *    state machines.
  *
  * 2. The second is to ignore states and map each and every received CHI primitive on an issued NPI
  *    primtive, and map each and every received NPI primitive onto a CHI primitive.  In this case,
  *    differences between user input and NS-Provider state will be handled automagically.  This is
  *    the preferred approach.
  */
static int
n_error_ack(queue_t *q, mblk_t *mp, struct rc *rc, struct ch *ch)
{
	N_error_ack_t *p;
	mblk_t *mp;
	int state, error, errno, prim;

	if (unlikely(!MBLKIN(mp, sizeof(ch->np_tag), sizeof(*p))))
		got emsgsize;

	p = (typeof(p)) (mp->b_rptr + sizeof(ch->np_tag));

	if (p->NPI_error != NOUTSTATE) {
	} else {
		/* This is an out of state error.  Based on the primitive in which it is in
		   response, issue the equivalent error to the CHI interface. */
		switch (p->ERROR_prim) {
		case N_CONN_REQ:
		case N_CONN_RES:
		case N_DISCON_REQ:
		case N_DATA_REQ:
		case N_EXDATA_REQ:
		case N_INFO_REQ:
		case N_BIND_REQ:
		case N_UNBIND_REQ:
		case N_UNITDATA_REQ:
		case N_OPTMGMT_REQ:
		case N_DATACK_REQ:
		case N_RESET_REQ:
		case N_RESET_RES:
		default:
			/* nonsense */
		}
	}

	/* There is a one-to-one mapping between any NPI primitive in error and any CHI primitive
	   that was in error. */
	switch (p->ERROR_prim) {
	case N_CONN_REQ:
		prim = CH_ENABLE_REQ;
		break;
	case N_DISCON_REQ:
		prim = CH_DISABLE_REQ;
		break;
	case N_DATA_REQ:
		prim = CH_DATA_REQ;
		break;
	case N_EXDATA_REQ:
		prim = CH_DATA_REQ;
		break;
	case N_INFO_REQ:
		prim = CH_INFO_REQ;
		break;
	case N_BIND_REQ:
		prim = CH_ATTACH_REQ;
		break;
	case N_UNBIND_REQ:
		prim = CH_DETACH_REQ;
		break;
	case N_UNITDATA_REQ:
		prim = CH_DATA_REQ;
		break;
	case N_OPTMGMT_REQ:
		prim = CH_OPTMGMT_REQ;
		break;
	case N_CONN_RES:	/* no corresponding CHI primitive */
	case N_DATACK_REQ:	/* no corresponding CHI primitive */
	case N_RESET_REQ:	/* no corresponding CHI primitive */
	case N_RESET_RES:	/* no corresponding CHI primitive */
		prim = -1;
		break;
	}

	switch ((state = np_get_state(ch))) {
	case NS_WCON_CREQ:
		if (p->ERROR_prim != N_CONN_REQ) ;
		if (p->NPI_error != NOUTSTATE)
			state = NS_IDLE;
		break;
	case NS_WACK_CRES:
		if (p->ERROR_prim != N_CONN_RES) ;
		if (p->NPI_error != NOUTSTATE)
			state = NS_WRES_CIND;
		break;
	case NS_WACK_BREQ:
		if (p->ERROR_prim != N_BIND_REQ) ;
		if (p->NPI_error != NOUTSTATE)
			state = NS_UNBND;
		break;
	case NS_WACK_UREQ:
		if (p->ERROR_prim != N_UNBIND_REQ) ;
		if (p->NPI_error != NOUTSTATE)
			state = NS_IDLE;
		break;
	case NS_WACK_OPTREQ:
		if (p->ERROR_prim != N_OPTMGMT_REQ) ;
		if (p->NPI_error != NOUTSTATE)
			state = NS_IDLE;
		break;
	case NS_WACK_RRES:
		if (p->ERROR_prim != N_RESET_RES) ;
		if (p->NPI_error != NOUTSTATE)
			state = NS_WRES_RIND;
		break;
	case NS_WCON_RREQ:
		if (p->ERROR_prim != N_RESET_REQ) ;
		if (p->NPI_error != NOUTSTATE)
			state = NS_DATA_XFER;
		break;
	case NS_WACK_DREQ6:
		if (p->ERROR_prim != N_DISCON_REQ) ;
		if (p->NPI_error != NOUTSTATE)
			state = NS_WCON_CREQ;
		break;
	case NS_WACK_DREQ7:
		if (p->ERROR_prim != N_DISCON_REQ) ;
		if (p->NPI_error != NOUTSTATE)
			state = NS_WRES_CIND;
		break;
	case NS_WACK_DREQ9:
		if (p->ERROR_prim != N_DISCON_REQ) ;
		if (p->NPI_error != NOUTSTATE)
			state = NS_DATA_XFER;
		break;
	case NS_WACK_DREQ10:
		if (p->ERROR_prim != N_DISCON_REQ) ;
		if (p->NPI_error != NOUTSTATE)
			state = NS_WCON_RREQ;
		break;
	case NS_WACK_DREQ11:
		if (p->ERROR_prim != N_DISCON_REQ) ;
		if (p->NPI_error != NOUTSTATE)
			state = NS_WRES_RIND;
		break;
	case NS_DATA_XFER:
		if (p->ERROR_prim != N_DATACK_REQ) ;
		break;
	default:
		if (p->ERROR_prim != N_INFO_REQ && p->ERROR_prim != N_OPTMGMT_REQ) ;
		break;
	}

	/* This is how we translate error codes. */
	switch (p->NPI_error) {
	case 0:
		error = 0;
		errno = 0;
		break;
	case NBADADDR:
		error = CHBADADDR;
		errno = EADDRNOTAVAIL;
		break;
	case NBADOPT:
		error = CHBADOPT;
		errno = EOPNOTSUPP;
		break;
	case NACCESS:
		error = CHSYSERR;
		errno = EPERM;
		break;
	case NNOADDR:
		error = CHBADADDR;
		errno = EDESTADDRREQ;
		break;
	case NOUTSTATE:
		error = CHOUTSTATE;
		errno = EPROTO;
		break;
	case NBADSEQ:
		error = CHBADSLOT;
		errno = EINVAL;
		break;
	case NSYSERR:
		error = CHSYSERR;
		errno = p->UNIX_error;
		break;
	case NBADDATA:
		/* This valud shouldn't happen, NPI only returns NBADDATA in special circumstances
		   that would represent a software error in this module or the next. */
		error = CHSYSERR;
		errno = EMSGSIZE;
	case NNOTSUPPORT:
		error = CHNOTSUPP;
		errno = EOPNOTSUPP;
		break;
	case NBOUND:
		error = CHOUTSTATE;
		errno = EADDRINUSE;
		break;
	case NBADQOSPARAM:
		error = CHBADPARM;
		errno = EOPNOTSUPP;
		break;
	case NBADQOSTYPE:
		error = CHBADPARMTYPE;
		errno = EOPNOTSUPP;
		break;
	case NBADTOKEN:
		error = CHBADSLOT;
		errno = EINVAL;
		break;
	case NNOPROTOID:
		error = CHBADADDR;
		errno = EDESTADDRREQ;
		break;
	default:
		error = CHSYSERR;
		errno = p->UNIX_error;
		if (errno == 0)
			errno = EINVAL;
		break;
	}
}

/** n_ok_ack: - process N_OK_ACK primitive
  * @q: active queue
  * @mp: the N_OK_ACK primitive
  * @rc: private structure (locked)
  * @ch: channel structure
  *
  * There are several instances where an N_OK_ACK maps directly into an equivalent CHI primitive.
  */
static int
n_ok_ack(queue_t *q, mblk_t *mp, struct rc *rc, struct ch *ch)
{
	N_ok_ack_t *p;
	int err, state;

	if (unlikely(!MBLKIN(mp, sizeof(ch->np_tag), sizeof(*p))))
		goto emsgsize;
	p = (typeof(p)) (mp->b_rptr + sizeof(ch->np_tag));

	switch (__builtin_expect(p->CORRECT_prim, N_RESET_RES)) {
	case N_RESET_RES:
		/* completion of reset indication procedure */
		np_set_state(rc->wq, ch, NS_DATA_XFER);
		freemsg(mp);
		return (0);
	case N_CONN_RES:
		/* completion of connect indication procedure */
		np_set_state(rc->wq, ch, NS_DATA_XFER);
		freemsg(mp);
		return (0);
	case N_DISCON_REQ:
		/* completion of disconnect request procedure */
		np_set_state(rc->wq, ch, NS_IDLE);
		return ch_disable_con(q, mp, rc, ch);
	case N_UNBIND_REQ:
	case N_OPTMGMT_REQ:
	case N_CONN_REQ:
	case N_RESET_REQ:
	case N_DATA_REQ:
	case N_EXDATA_REQ:
	case N_INFO_REQ:
	case N_BIND_REQ:
	case N_UNITDATA_REQ:
	case N_DATACK_REQ:
		goto eproto;
	default:
		/* nonsense */
		goto notsupport;
	}
      eproto:
	err = -EPROTO;
	goto error;
      notsupport:
	err = NNOTSUPPORT;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return n_reply_error(q, mp, rc, ch, N_OK_ACK, err);
}

/** n_unitdata_ind: - process N_UNITDATA_IND primitive
  * @q: active queue
  * @mp: the N_UNITDATA_IND primitive
  * @rc: private structure (locked)
  * @ch: channel structure
  *
  * We can receive unitdata indications for channels that are partially specified (only have a local
  * address bound) and are permitted to receive packets.  They must also be connected in the receive
  * direction, otherwise, we will simply discard them.
  */
static int
n_unitdata_ind(queue_t *q, mblk_t *mp, struct rc *rc, struct ch *ch)
{
	N_unitdata_ind_t *p;
	int err;

	if (unlikely(!MBLKIN(mp, sizeof(ch->ch_tag), sizeof(*p))))
		goto emsgsize;
	if (unlikely(np_get_state(ch) != NS_IDLE))
		goto outstate;
	/* one of the following is redundant */
	if (unlikely(ch_get_state(ch) != CHS_CONNECTED))
		goto discard;
	if (unlikely(!(ch->flags & CHF_RX_DIR)))
		goto discard;
	/* FIXME: this is not correct yet.  What this does is simply forward the data upstream.  It 
	   has to be manipulated first.  Also, it would be pointless to convert the payload only to 
	   find that stream is flow controlled or that a message block cannot be allocated.  We
	   should adjust ch_data_ind to return the M_PROTO portion of the message block without the 
	   data attached. */
	return ch_data_ind(q, mp, rc, ch, mp->b_cont);
      outstate:
	err = NOUTSTATE;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return n_reply_error(q, mp, rc, ch, N_UNITDATA_IND, err);
      discard:
	freemsg(mp);
	return (0);
}

/** n_uderror_ind: - process N_UDERROR_IND primitive
  * @q: active queue
  * @mp: the N_UDERROR_IND primitive
  * @rc: private structure (locked)
  * @ch: channel structure
  *
  * Unitdata error indications are generated by the underlying driver when ICMP errors are received
  * that belong to the connection.  Note that we do not expect to receive N_UDERROR_IND primitives
  * for partially specified connections because we never transmit anything.  So this function is
  * moot.
  */
static int
n_uderror_ind(queue_t *q, mblk_t *mp, struct rc *rc, struct ch *ch)
{
	N_uderror_ind_t *p;
	int err;

	if (!MBLKIN(mp, sizeof(ch->np_tag), sizeof(*p)))
		goto emsgsize;
	return ch_event_ind(rc, ch, q, mp, event);
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	freemsg(mp);
	mi_strlog(rc->wq, STRLOGDA, SL_ERROR, "Bad N_UDERROR_IND! err = %d", err);
	return (0);
}

/** n_datack_ind: - process N_DATACK_IND primitive
  * @q: active queue
  * @mp: the N_DATACK_IND primitive
  * @rc: private structure (locked)
  * @ch: channel structure
  *
  * We never ask for receipt acknowledgement, but it is something we could do in the future.  We
  * could use acknowledgements to provide separate flow controls for each of the virtual channels in
  * the multi-stream.  For example, if, instead of consulting STREAM flow control, we set a maximum
  * number of outstanding messages and set receipt confirmation on each packet.  If the lower level
  * driver then sends us a N_DATACK_IND for each packet it receives and sends off, then we can use
  * the N_DATACK_IND to clock out another packet.  However, we do not do this yet, so this function
  * is moot.
  */
static int
n_datack_ind(queue_t *q, mblk_t *mp, struct rc *rc, struct ch *ch)
{
	return n_reply_error(q, mp, rc, ch, N_DATACK_IND, NNOTSUPPORT);
}

/** n_reset_ind: - process N_RESET_IND primitive
  * @q: active queue
  * @mp: the N_RESET_IND primitive
  * @rc: private structure (locked)
  * @ch: channel structure
  *
  * The RTP drivers only send us N_RESET_IND when it wants to report an ICMP error.  So, be careful,
  * these might come quite quickly.  We must respond to each with a N_RESET_RES so that the state
  * machine of the NS-Provider can clear.
  */
static int
n_reset_ind(queue_t *q, mblk_t *mp, struct rc *rc, struct ch *ch)
{
	N_reset_ind_t *p;
	N_reset_res_t *r;
	mblk_t *rp;
	int err;

	if (!MBLKIN(mp, sizeof(ch->ch_tag), sizeof(*p)))
		goto emsgsize;
	switch (__builtin_expect(np_get_state(ch), NS_DATA_XFER)) {
	case NS_DATA_XFER:
	case NS_WRES_RIND:
		break;
	case NS_IDLE:
		goto discard;
	default:
		goto outstate;
	}

	if (unlikely(!(rp = mi_allocb(q, sizeof(ch->ch_tag) + sizeof(*r), BPRI_MED))))
		goto enobufs;

	if ((err = ch_event_ind(q, mp, rc, ch, event)))
		goto free_error;

	np_set_state(rc->wq, ch, NS_WRES_RIND);

	DB_TYPE(rp) = M_PCPROTO;

	r = (typeof(p)) rp->b_wptr;
	r->PRIM_type = N_RESET_RES;
	rp->b_wptr += sizeof(*r);

	np_set_state(rc->wq, ch, NS_WACK_RRES);
	mi_strlog(rc->wq, STRLOGDA, SL_TRACE, "N_RESET_RES ->");
	putnext(rc->wq, rp);
	return (0);
      enobufs:
	err = -ENOBUFS;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      free_error:
	freeb(rp);
      error:
	return n_error_reply(q, mp, rc, ch, N_RESET_IND, err);
}

/** n_reset_con: - process N_RESET_CON primitive
  * @q: active queue
  * @mp: the N_RESET_CON primitive
  * @rc: private structure (locked)
  * @ch: channel structure
  *
  * We frankly do not expect to receive confirmations of reset because we never issue N_RESET_REQ
  * primitives.  So, this function is moot and will only occur when there is a software fault.
  */
static int
n_reset_con(queue_t *q, mblk_t *mp, struct rc *rc, struct ch *ch)
{
	N_reset_con_t *p;
	int err;

	if (unlikely(!MBLKIN(mp, sizeof(ch->ch_tag), sizeof(*p))))
		goto emsgsize;
	if (unlikely(np_get_state(ch) != NS_WCON_RREQ))
		goto outstate;
	np_set_state(q, ch, NS_DATA_XFER);
	freemsg(mp);
	return (0);
      outstate:
	err = NOUTSTATE;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return n_error_reply(q, mp, rc, ch, N_RESET_CON, err);
}

/** n_other_ind: - process unknown NPI indication primitives
  * @q: active queue (read queue)
  * @mp: the unknown primitive
  * @rc: private structure (locked)
  * @ch: channel structure
  */
static int
n_other_ind(queue_t *q, mblk_t *mp, struct rc *rc, struct ch *ch)
{
	np_ulong *p;
	int err, prim;

	prim = *(typeof(p) *) (mp->b_rptr + sizeof(ch->ch_tag));

	/* Rather redundant because we always check this in the calling function. */
	if (unlikely(!MBLKIN(mp, sizeof(ch->ch_taq), sizeof(*p))))
		goto emsgsize;
	goto notsupport;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      notsupport:
	err = NNOTSUPPORT;
	goto error;
      error:
	return n_error_reply(q, mp, rc, ch, prim, err);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  WAKEUPS
 *
 *  -------------------------------------------------------------------------
 */

/** rc_w_wakeup: - write queue wakeup
  * @q: write queue
  * @rc: private structure (locked)
  *
  * This is a wakeup procedure.  The wakeup procedure is invoked once for each
  * run of the service procedure.
  */
static noinline fastcall void
rc_w_wakeup(queue_t *q, struct rc *rc)
{
	return;
}

/** rc_r_wakeup: - read queue wakeup
  * @q: read queue
  * @rc: private structure (locked)
  *
  * This is a wakeup procedure.  The wakeup procedure is invoked once for each
  * run of the service procedure.
  */
static noinline fastcall void
rc_r_wakeup(queue_t *q, struct rc *rc)
{
	return;
}

/*
 *  -------------------------------------------------------------------------
 *
 *  INPUT-OUTPUT CONTROLS
 *
 *  -------------------------------------------------------------------------
 */

/** rc_testconfig: - test RTP-CH configuration for validity
  * @rc: private structure (locked)
  * @arg: configuration to test
  */
static int
rc_testconfig(struct rc *rc, struct ch_config *arg)
{
	int err = 0;

	/* FIXME: test arg */
	return (err);
}

/** rc_setnotify: - set RTP-CH notification bits
  * @rc: private structure (locked)
  * @arg: notification arguments
  */
static int
rc_setnotify(struct rc *rc, struct ch_notify *arg)
{
	if (arg->events & ~(CH_EVT_ALL))
		return (EINVAL);
	arg->events = arg->events | rc->rc.notify.events;
	return (0);
}

/** rc_clrnotify: - clear RTP-CH notification bits
  * @rc: private structure (locked)
  * @arg: notification arguments
  */
static int
rc_clrnotify(struct rc *rc, struct ch_notify *arg)
{
	if (arg->events & ~(CH_EVT_ALL))
		return (EINVAL);
	arg->events = ~arg->events & rc->rc.notify.events;
	return (0);
}

/** rc_manage: - perform RTP-CH management action
  * @rc: private structure (locked)
  * @arg: management arguments
  */
static int
rc_manage(struct rc *rc, struct ch_mgmt *arg)
{
	switch (arg->cmd) {
	case CH_MGMT_RESET:
		/* FIXME: reset */
		break;
	default:
		return (EINVAL);
	}
	return (0);
}

/** rc_ioctl: - process RTP-CH M_IOCTL message
  * @q: active queue
  * @mp: the M_IOCTL message
  * @rc: private structure (locked)
  *
  * This is step 1 of the RTP-CH input-output control operation.  Step 1 consists of a copyin
  * operation for SET operation and a copyout operation for GET operations.
  *
  * Note that even though this is a multi-Stream module, only the default object is addressed.  We
  * do not collect statistics nor control configuration for the individual channels in the
  * multi-stream.
  */
static int
rc_ioctl(queue_t *q, mblk_t *mp, struct rc *rc)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	mblk_t *bp;
	int size = -1;
	int err = 0;

	mi_strlog(rc->wq, STRLOGIO, SL_TRACE, "-> M_IOCTL(%s)", ch_iocname(ioc->ioc_cmd));

	rc_save_total_state(rc);

	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(CH_IOCGCONFIG):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(rc->rc.config), false)))
			goto enobufs;
		bcopy(&rc->rc.config, bp->b_rptr, sizeof(rc->rc.config));
		break;
	case _IOC_NR(CH_IOCSCONFIG):
	case _IOC_NR(CH_IOCTCONFIG):
	case _IOC_NR(CH_IOCCCONFIG):
		size = sizeof(rc->rc.config);
		break;
	case _IOC_NR(CH_IOCGSTATEM):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(rc->rc.statem), false)))
			goto enobufs;
		bcopy(&rc->rc.statem, bp->b_rptr, sizeof(rc->rc.statem));
		break;
	case _IOC_NR(CH_IOCCMRESET):
		/* FIXME: reset the state machine */
		break;
	case _IOC_NR(CH_IOCGSTATSP):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(rc->rc.statsp), false)))
			goto enobufs;
		bcopy(&rc->rc.statsp, bp->b_rptr, sizeof(rc->rc.statsp));
		break;
	case _IOC_NR(CH_IOCSSTATSP):
		size = sizeof(rc->rc.statsp);
		break;
	case _IOC_NR(CH_IOCGSTATS):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(rc->rc.stats), false)))
			goto enobufs;
		bcopy(&rc->rc.stats, bp->b_rptr, sizeof(rc->rc.stats));
		break;
	case _IOC_NR(CH_IOCCSTATS):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(rc->rc.stats), false)))
			goto enobufs;
		bcopy(&rc->rc.stats, bp->b_rptr, sizeof(rc->rc.stats));
		bzero(&rc->rc.stats, sizeof(rc->rc.stats));
		break;
	case _IOC_NR(CH_IOCGNOTIFY):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(rc->rc.notify), false)))
			goto enobufs;
		bcopy(&rc->rc.notify, bp->b_rptr, sizeof(rc->rc.notify));
		break;
	default:
		err = EOPNOTSUPP;
		break;
	      enobufs:
		err = ENOBUFS;
		break;
	}
	if (err < 0) {
		rc_restore_total_state(rc);
		return (err);
	}
	if (err < 0)
		mi_copy_done(q, mp, err);
	if (err == 0) {
		if (size == 0)
			mi_copy_done(q, mp, 0);
		else if (size == -1)
			mi_copyout(q, mp);
		else
			mi_copyin(q, mp, NULL, size);
	}
	return (0);
}

/** rc_copyin: - process RTP-CH M_IOCDATA message
  * @q: active queue
  * @mp: the M_IOCDATA message
  * @rc: private structure (locked)
  * @dp: the data part
  *
  * This is step number 2 for SET operations.  This is the result of the implicit or explicit copyin
  * operation.  When can now perform sets and commits.  All SET operations must also include a last
  * copyout step that copies out the information actually set.
  */
static int
rc_copyin(queue_t *q, mblk_t *mp, struct rc *rc)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	int err = 0;
	mblk_t *bp;

	if (!(bp = mi_copyout_alloc(q, mp, NULL, MBLKL(dp), false)))
		goto enobufs;
	bcopy(dp->b_rptr, bp->b_rptr, MBLKL(dp));
	mi_strlog(rc->wq, STRLOGIO, SL_TRACE, "-> M_IOCDATA(%s)", ch_iocname(cp->cp_cmd));
	rc_save_total_state(rc);

	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(CH_IOCSCONFIG):
		if ((err = rc_testconfig(rc, (struct ch_config *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &rc->rc.config, sizeof(rc->rc.config));
		break;
	case _IOC_NR(CH_IOCTCONFIG):
		err = rc_testconfig(rc, (struct ch_config *) bp->b_rptr);
		break;
	case _IOC_NR(CH_IOCCCONFIG):
		if ((err = rc_testconfig(rc, (struct ch_config *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &rc->rc.config, sizeof(rc->rc.config));
		break;
	case _IOC_NR(CH_IOCSSTATSP):
		bcopy(bp->b_rptr, &rc->rc.statsp, sizeof(rc->rc.statsp));
		break;
	case _IOC_NR(CH_IOCSNOTIFY):
		if ((err = rc_setnotify(rc, (struct ch_notify *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &rc->rc.notify, sizeof(rc->rc.notify));
		break;
	case _IOC_NR(CH_IOCCNOTIFY):
		if ((err = rc_clrnotify(rc, (struct ch_notify *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &rc->rc.notify, sizeof(rc->rc.notify));
		break;
	case _IOC_NR(CH_IOCCMGMT):
		err = rc_manage(rc, (struct ch_mgmt *) bp->b_rptr);
		break;
	default:
		err = EPROTO;
		break;
	      enobufs:
		err = ENOBUFS;
		break;
	}
	if (err < 0) {
		rc_restore_total_state(rc);
		return (err);
	}
	if (err > 0)
		mi_copy_done(q, mp, err);
	if (err == 0)
		mi_copyout(q, mp);
	return (0);
}

/** rc_copyout: - process RTP-CH M_IOCDATA message
  * @rc: private structure (locked)
  * @q: active queue
  * @mp: the M_IOCDATA message
  * @dp: data part
  *
  * This is the final step that is a simple copy-done operation.
  */
static int
rc_copyout(queue_t *q, mblk_t *mp, struct rc *rc)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;

	mi_strlog(rc->wq, STRLOGIO, SL_TRACE, "-> M_IOCDATA(%s)", ch_iocname(cp->cp_cmd));
	mi_copyout(q, mp);
	return (0);
}

static int
rc_do_ioctl(queue_t *q, mblk_t *mp, struct rc *rc)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*ioc))) || unlikely(mp->b_cont = NULL)) {
		mi_copy_done(q, mp, EFAULT);
		return (0);
	}
	switch (_IOC_TYPE(ioc->ioc_cmd)) {
	case CH_IOC_MAGIC:
		return rc_ioctl(q, mp, rc);
	default:
		if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
			putnext(q, mp);
			return (0);
		}
		return (-EBUSY);
	}
}

static int
rc_do_copyin(queue_t *q, mblk_t *mp, struct rc *rc, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*cp))) || unlikely(mp->b_cont == NULL)) {
		mi_copy_done(q, mp, EFAULT);
		return (0);
	}
	switch (_IOC_TYPE(cp->cp_cmd)) {
	case CH_IOC_MAGIC:
		return rc_copyin(q, mp, rc, dp);
	default:
		if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
			putnext(q, mp);
			return (0);
		}
		return (-EBUSY);
	}
}

static int
rc_do_copyout(queue_t *q, mblk_t *mp, struct rc *rc, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*cp))) || unlikely(mp->b_cond == NULL)) {
		mi_copy_done(q, mp, EFAULT);
		return (0);
	}
	switch (_IOC_TYPE(cp->cp_cmd)) {
	case CH_IOC_MAGIC:
		return rc_copyout(q, mp, rc, dp);
	default:
		if (pcbmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
			putnext(q, mp);
			return (0);
		}
		return (-EBUSY);
	}
}

/** __rc_w_ioctl: - process M_IOCTL message
  * @q: active queue
  * @mp: the M_IOCTL message
  */
static noinline fastcall int
__rc_w_ioctl(queue_t *q, mblk_t *mp, struct rc *rc)
{
	int err;

	err = rc_do_ioctl(q, mp, rc);
	if (err <= 0)
		return (err);
	mi_copy_done(q, mp, err);
	return (0);
}

static noinline fastcall int
__rc_w_iocdata(queue_t *q, mblk_t *mp, struct rc *rc)
{
	mblk_t *dp;
	int err;

	switch (mi_copy_state(q, mp, &dp)) {
	case -1:
		err = 0;
		break;
	case MI_COPY_CASE(MI_COPY_IN, 1):
		err = rc_do_copyin(q, mp, rc, dp);
		break;
	case MI_COPY_CASE(MI_COPY_OUT, 1):
		err = rc_do_copyout(q, mp, rc, dp);
		break;
	default:
		err = EPROTO;
		break;
	}
	if (err <= 0)
		return (err);
	mi_copy_done(q, mp, err);
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  STREAMS MESSAGE HANDLING
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  -------------------------------------------------------------------------
 *
 *  M_(PC)PROTO MESSAGE Handling
 *
 *  -------------------------------------------------------------------------
 */

/** __rc_w_proto_err: - process mangled packet
  * @q: active queue (write queue)
  * @mp: the CHS-User issued primitive
  * @rc: private structure (locked)
  *
  * This is really really really bad.  The primitive is not even large enough to
  * contain the primitive type.  Perhaps we can figure out to which channel it
  * belongs, perhaps we cannot.
  */
static noinline fastcall __unlikely int
__rc_w_proto_err(queue_t *q, mblk_t *mp, struct rc *rc)
{
	struct ch *ch;

	if (MBLKIN(mp, 0, sizeof(ch->ch_tag))) {
		/* Attempt to extract pointer at least. */
		/* FIXME: lookup pointer */
		/* We don't even know what primitive it is... */
		return ch_error_reply(q, mp, rc, ch, -3, -EMSGSIZE);
	}
	freemsg(mp);
	mi_strlog(q, STRLOGERR, SL_ERROR, "Primitve too short, %ld bytes!", (long) MBLKL(mp));
	return (0);
}

/** __rc_r_proto_err: - process mangled packet
  * @q: active queue (read queue)
  * @mp: the NS-Provider issued primitive
  * @rc: private structure (locked)
  *
  * This is really really really bad.  The primitive is not even large enough to
  * contain the primitive type.  Perhaps we can figure out to which channel it
  * belongs, perhaps we cannot.
  */
static noinline fastcall __unlikely int
__rc_r_proto_err(queue_t *q, mblk_t *mp, struct rc *rc)
{
	struct ch *ch;

	if (MBLKIN(mp, 0, sizeof(ch->np_tag))) {
		/* Attempt to extract pointer at least. */
		/* FIXME: lookup pointer */
		/* We don't even know what primitive it is... */
	}
	freemsg(mp);
	mi_strlog(q, STRLOGERR, SL_ERROR, "Primitive too short, %ld bytes!", (long) MBLKL(mp));
	return (0);
}

/** ch_tag_lookup: - lookup multi-stream tag for CHI interface
  * @q: upper write queue (not currently used)
  * @mp: the message
  * @rc: private structure (locked) (not currently used)
  * @prim: the CHI primitive
  * @errp: a pointer to an integer error number
  *
  * This is the first jab at implementing the lookup of the tag.  We trust the tag given to use
  * (somewhat -- probably too much) and use the magic number in the structure to determine validity.
  * This is not foolproof and one should not let user processes access this interface.  The @q and
  * @rc pointers are not used... yet.  For more certain validation one could maintain the addresses
  * of valid cache pages in a page table and check that the upper portion of the address at least
  * matches one of the valid cache pages.  It might be possible to ask the Linux kernel cache
  * mechnism whether the address is valid.
  *
  * Note that CH_ATTACH_REQ has a CHS-User tag in the first position instead of the assigned
  * CHS-Provider tag, so we always return NULL in this case.  Also, NULL is a valid tag value for
  * CH_INFO_REQ and CH_OPTMGMT_REQ.
  */
static inline fastcall __hot_in struct ch *
ch_tag_lookup(queue_t *q, mblk_t *mp, struct rc *rc, int prim, int *errp)
{
	struct ch *ch;
	unsigned long tag;

	if (unlikely(prim == CH_ATTACH_REQ))
		goto null;
	tag = *(typeof(tag) *) mp->b_rptr;
	if ((tag == 0)
	    && ((1 << prim) & ((1 << CH_INFO_REQ) | (1 << CH_OPTMGMT_REQ) | (1 << CH_ATTACH_REQ))))
		goto null;
	if (unlikely(tag < 100000))
		goto error;
	if (unlikely((tag & 0x03) != 0))
		goto error;
	ch = (typeof(ch)) tag;
	if (ch->magic != CHANNEL_MAGIC)
		goto error;
	return (ch);
      error:
	*errp = CHBADSLOT;
      null:
	return (NULL);
}

/** __rc_w_proto: - process a CHS-User to CHS-Provider primitive
  * @q: active queue (write queue)
  * @mp: the CHS-User issued primitive
  * @rc: private structure (locked)
  *
  * CHS-User primitives have an unsigned long tag number ahead of the primitive proper.  The
  * unsigned long tag number is the ch pointer for messages continuing a channel interface during
  * its life cycle.  For CH_ATTACH_REQ primitives, however, the tag is the CHS-User tag that is to
  * be associated with a newly created channel.  All CHS-Provider issued primitives have the
  * CHS-User tag attached.  A CH_OK_ACK primitive acknowledging a CH_ATTACH_REQ has an additional
  * tag following the primitive structure which is the CHS-Provider tag.  All CHS-User issued
  * primitives continuing the interface (i.e. other than the CH_ATTACH_REQ) must have the
  * CHS-Provider tag attached by the CHS-User.
  *
  * If we cannot find the channel, then the channel identifier is incorrect.  We indicate this by
  * returning the same channel identifier and issuing a specific error indication, [CHBADSLOT].
  *
  * Note: when optimizing for speed we don't ever want to trace data messages.
  */
static noinline fastcall int
__rc_w_proto(queue_t *q, mblk_t *mp, struct rc *rc)
{
	ch_ulong prim;
	struct ch *ch = NULL;
	int err = 0;

	if (unlikely(!MBLKIN(mp, sizeof(ch->ch_tag), sizeof(ch_ulong))))
		return __rc_w_proto_err(q, mp, rc);

	prim = *(ch_ulong *) (mp->b_rptr + sizeof(ch->ch_tag));

	if (unlikely((ch = ch_tag_lookup(q, mp, rc, prim, &err)) == NULL && err != 0))
		/* Note that ch_error_reply will tag the message with the tag from @mp (in this
		   case the erroneous NS-User tag) because @ch is NULL. */
		return ch_error_reply(q, mp, rc, ch, prim, CHBADSLOT);

	/* Note that if @ch is NULL, this is a no-op. */
	rc_save_total_state(rc, ch);

	switch (__builtin_expect(prim, CH_DATA_REQ)) {
	case CH_DATA_REQ:
#ifndef _OPTIMIZE_SPEED
		mi_strlog(q, STRLOGDA, SL_TRACE, "<- %s", ch_primname(prim));
#endif				/* _OPTIMIZE_SPEED */
		break;
	default:
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- %s", ch_primname(prim));
		break;
	}
	switch (__builtin_expect(prim, CH_DATA_REQ)) {
	case CH_DATA_REQ:
		err = ch_data_req(q, mp, rc, ch);
		break;
	case CH_INFO_REQ:
		err = ch_info_req(q, mp, rc, ch);	/* Note that @ch can be #NULL here. */
		break;
	case CH_OPTMGMT_REQ:
		err = ch_optmgmt_req(q, mp, rc, ch);	/* Note that @ch can be #NULL here. */
		break;
	case CH_ATTACH_REQ:
		err = ch_attach_req(q, mp, rc, ch);	/* Note that @ch is #NULL here. */
		break;
	case CH_ENABLE_REQ:
		err = ch_enable_req(q, mp, rc, ch);
		break;
	case CH_CONNECT_REQ:
		err = ch_connect_req(q, mp, rc, ch);
		break;
	case CH_DISCONNECT_REQ:
		err = ch_disconnect_req(q, mp, rc, ch);
		break;
	case CH_DISABLE_REQ:
		err = ch_disable_req(q, mp, rc, ch);
		break;
	case CH_DETACH_REQ:
		err = ch_detach_req(q, mp, rc, ch);
		break;
	default:
		err = ch_other_req(q, mp, rc, ch);
		break;
	}
	if (err) {
		/* When an error is returned, we are going to place the message (back) on the queue 
		   and wait for the service procedure to run (again).  Therefore, we must restore
		   the previous state of the channel so that it does not conflict on the subsequent 
		   pass through the handling function.  Note that if @ch is NULL, this is a no-op. */
		rc_restore_total_state(rc, ch);
	}
	return (err);
}

/** np_tag_lookup: - lookup multi-stream tag for NPI interface
  * @q: lower read queue (not currently used)
  * @mp: the message
  * @rc: private structure (locked) (not currently used)
  * @prim: the NPI primitive
  * @errp: a pointer to an integer error number
  *
  * This is the first jab at implementing the lookup of the tag.  We trust the tag given to use
  * (somewhat -- probably too much) and use the magic number in the structure to determine validity.
  * This is not foolproof and one should not let user processes access this interface.  The @q and
  * @rc pointers are not used... yet.  For more certain validation one could maintain the addresses
  * of valid cache pages in a page table and check that the upper portion of the address at least
  * matches one of the valid cache pages.  It might be possible to ask the Linux kernel cache
  * mechnism whether the address is valid.
  */
static inline fastcall __hot_out struct ch *
np_tag_lookup(queue_t *q, mblk_t *mp, struct rc *rc, int prim, int *errp)
{
	struct ch *ch;
	unsigned long tag;

	tag = *(typeof(tag) *) mp->b_rptr;
	if ((tag == 0)
	    && ((1 << prim) & ((1 << N_INFO_ACK) | (1 << N_OK_ACK) | (1 << N_ERROR_ACK))))
		goto null;
	if (unlikely(tag < 100000))
		goto error;
	if (unlikely((tag & 0x03) != 0))
		goto error;
	ch = (typeof(ch)) tag;
	if (ch->magic != CHANNEL_MAGIC)
		goto error;
	return (ch);
      error:
	*errp = NBADTOKEN;
      null:
	return (NULL);
}

/** __rc_r_proto: - process a NS-Provider to NS-User primitive
  * @q: active queue (read queue)
  * @mp: the NS-Provider issued primitive
  * @rc: private structure (locked)
  *
  * NS-Provider primitives have an unsigned long tag number ahread of the primitive proper.  The
  * unsigned long tag number is the ch pointer for messages continuing a channel interface during
  * its life cycle.  For N_BIND_REQ primitives, however, the tag is the NS-User tag that is to be
  * associated with a newly created channel.  For N_BIND_ACK primitive acknowledging a N_BIND_REQ,
  * the tag delivered is the NS-User tag, however, an additional NS-Provider tag is appended to the
  * N_BIND_ACK.  All NS-Provider issued primitives have the NS-User tag prepended.  All NS-User
  * issued primitives (except the N_BIND_REQ) have the NS-Provider tag prepended.
  */
static noinline fastcall int
__rc_r_proto(queue_t *q, mblk_t *mp, struct rc *rc)
{
	np_ulong prim;
	struct ch *ch = NULL;
	int err = 0;

	if (unlikely(!MBLKIN(mp, sizeof(ch->np_tag), sizeof(np_ulong))))
		return __rc_r_proto_err(q, mp, rc);

	prim = *(np_ulong *) (mp->b_rptr + sizeof(ch->np_tag));

	if (unlikely((ch = np_tag_lookup(q, mp, rc, prim)) == NULL && err != 0))
		return n_error_reply(q, mp, rc, tag, prim, err);

	/* Note that if @ch is NULL, this is a no-op. */
	rc_save_total_state(rc, ch);

	switch (__builtin_expect(prim, N_DATA_IND)) {
	case N_DATA_IND:
	case N_RESET_IND:
	case N_EXDATA_IND:
	case N_UNITDATA_IND:
	case N_UDERROR_IND:
		/* Note: when optimizing for speed we don't ever want to trace data messages. */
#ifndef _OPTIMIZE_SPEED
		mi_strlog(q, STRLOGDA, SL_TRACE, "<- %s", np_primname(prim));
#endif				/* _OPTIMIZE_SPEED */
		break;
	default:
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- %s", np_primname(prim));
		break;
	}

	switch (__builtin_expect(prim, N_DATA_IND)) {
	case N_DATA_IND:
		err = n_data_ind(q, mp, rc, ch);
		break;
	case N_RESET_IND:
		err = n_reset_ind(q, mp, rc, ch);
		break;
	case N_EXDATA_IND:
		err = n_exdata_ind(q, mp, rc, ch);
		break;
	case N_UNITDATA_IND:
		err = n_unitdata_ind(q, mp, rc, ch);
		break;
	case N_UDERROR_IND:
		err = n_uderror_ind(q, mp, rc, ch);
		break;
	case N_CONN_IND:
		err = n_conn_ind(q, mp, rc, ch);
		break;
	case N_CONN_CON:
		err = n_conn_con(q, mp, rc, ch);
		break;
	case N_DISCON_IND:
		err = n_discon_ind(q, mp, rc, ch);
		break;
	case N_INFO_ACK:
		err = n_info_ack(q, mp, rc, ch);
		break;
	case N_BIND_ACK:
		err = n_bind_ack(q, mp, rc, ch);
		break;
	case N_ERROR_ACK:
		err = n_error_ack(q, mp, rc, ch);
		break;
	case N_OK_ACK:
		err = n_ok_ack(q, mp, rc, ch);
		break;
	case N_DATACK_IND:
		err = n_datack_ind(q, mp, rc, ch);
		break;
	case N_RESET_CON:
		err = n_reset_con(q, mp, rc, ch);
		break;
	default:
		err = n_other_ind(q, mp, rc, ch);
		break;
	}
	if (err) {
		/* When an error is returned, we are going to place the message (back) on the queue 
		   and wait for the service procedure to run (again).  Therefore, we must restore
		   the previous state of the channel so that it does not conflict on the subsequent 
		   pass through the handling function.  Note that if @ch is NULL, this is a no-op. */
		rc_restore_total_state(rc, ch);
	}
	return (err);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_(PC)SIG MESSAGE Handling
 *
 *  -------------------------------------------------------------------------
 */

/** __rc_w_sig: - process M_(PC)SIG message on write queue
  * @q: write queue
  * @mp: the M_(PC)SIG message
  * @rc: private structure (locked)
  *
  * M_PCSIG messages are used for mi_timer's, but we don't have any just yet.
  */
static noinline fastcall int
__rc_w_sig(queue_t *q, mblk_t *mp, struct rc *rc)
{
	/* don't need timers */
	mi_strlog(q, STRLOGRX, SL_TRACE, "M_(PC)SIG on write queue!");
	if (mi_timer_valid(mp)) {
		/* do nothing */
	}
	return (0);
}

/** __rc_r_sig: - process M_(PC)SIG message on read queue
  * @q: read queue
  * @mp: the M_(PC)SIG message
  * @rc: private structure (locked)
  *
  * M_PCSIG messages are used for mi_timer's, but we don't have any just yet.
  */
static noinline fastcall int
__rc_r_sig(queue_t *q, mblk_t *mp, struct rc *rc)
{
	/* don't need timers */
	mi_strlog(q, STRLOGRX, SL_TRACE, "M_(PC)SIG on read queue!");
	if (mi_timer_valid(mp)) {
		/* do nothing */
	}
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_(PC)CTL MESSAGE Handling
 *
 *  -------------------------------------------------------------------------
 */

/** __rc_w_ctl: - process M_(PC)CTL message on write queue
  * @q: write queue
  * @mp: the M_(PC)CTL message
  * @rc: private structure (locked)
  *
  * M_CTL/M_PCCTL messages are used for intermodule controls (similar to input-output controls or
  * M_PROTO/M_PCPROTO but without the Stream head.  We use intermodule controls to permit the
  * multi-stream approach where multiple virtual channel connections are performed over a single
  * stream.
  */
static noinline fastcall int
__rc_w_ctl(queue_t *q, mblk_t *mp, struct rc *rc)
{
	ch_ulong prim;
	struct ch *ch = NULL;
	int err = 0;

	if (unlikely(!MBLKIN(mp, sizeof(ch), sizeof(prim))))
		return __rc_w_ctl_err(q, mp, rc);

	prim = *(typeof(prim) *) (mp->b_rptr + sizeof(ch));

	if (unlikely(!(ch = ch_tag_lookup(q, mp, rc, prim, &err)) && err))
		return chm_error_reply(q, mp, rc, ch, prim CHBADSLOT);

	/* NOte that if @ch is NULL, this is a no-op. */
	rc_save_total_state(rc, ch);

	if (likely(prim == CH_DATA_REQ)) {
#ifndef _OPTIMIZE_SPEED
		mi_strlog(q, STRLOGDA, SL_TRACE, "<- %s", ch_primname(prim));
#endif	/* _OPTIMIZE_SPEED */
	} else {
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- %s", ch_primname(prim));
	}
	switch (__builtin_expect(prim, CH_DATA_REQ)) {
	case CH_DATA_REQ:
		err = chm_data_req(q, mp, rc, ch);
		break;
	case CH_INFO_REQ:
		err = chm_info_req(q, mp, rc, ch);	/* Note that @ch can be #NULL. */
		break;
	case CH_OPTMGMT_REQ:
		err = chm_optmgmt_req(q, mp, rc, ch);	/* Note that @ch can be #NULL. */
		break;
	case CH_ATTACH_REQ:
		err = chm_attach_req(q, mp, rc, ch);	/* Note that @ch is #NULL. */
		break;
	case CH_ENABLE_REQ:
		err = chm_enable_req(q, mp, rc, ch);
		break;
	case CH_CONNECT_REQ:
		err = chm_connect_req(q, mp, rc, ch);
		break;
	case CH_DISCONNECT_REQ:
		err = chm_disconnect_req(q, mp, rc, ch);
		break;
	case CH_DISABLE_REQ:
		err = chm_disable_req(q, mp, rc, ch);
		break;
	case CH_DETACH_REQ:
		err = chm_detach_req(q, mp, rc, ch);
		break;
	default:
		err = chm_other_Req(q, mp, rc, ch);
		break;
	}
	if (err)
		rc_restore_total_state(rc, ch);
	return (err);
}

/** __rc_r_ctl: - process M_(PC)CTL message on read queue
  * @q: read queue
  * @mp: the M_(PC)CTL message
  * @rc: private structure (locked)
  *
  * M_CTL/M_PCCTL messages are used for intermodule controls (similar to input-output controls but
  * without the Stream head.  We don't define any intermodule controls yet, so complain.
  */
static noinline fastcall int
__rc_r_ctl(queue_t *q, mblk_t *mp, struct rc *rc)
{
	/* don't have any intermodule controls yet */
	mi_strlog(q, STRLOGRX, SL_TRACE, "M_(PC)CTL on read queue!");
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_(PC)RSE MESSAGE Handling
 *
 *  -------------------------------------------------------------------------
 */

/** __rc_w_rse: - process M_(PC)RSE message on write queue
  * @q: write queue
  * @mp: the M_(PC)RSE message
  * @rc: private structure (locked)
  *
  * M_RSE/M_PCRSE messages are used for intramodule special purposes.  We don't define any
  * intramodule features requiring these messages yet, so complain.
  */
static noinline fastcall int
__rc_w_rse(queue_t *q, mblk_t *mp, struct rc *rc)
{
	/* don't have any reserved messages yet */
	mi_strlog(q, STRLOGERR, SL_ERROR, "M_(PC)RSE on write queue!");
	freemsg(mp);
	return (0);
}

/** __rc_w_rse: - process M_(PC)RSE message on read queue
  * @q: read queue
  * @mp: the M_(PC)RSE message
  * @rc: private structure (locked)
  *
  * M_RSE/M_PCRSE messages are used for intramodule special purposes.  We don't define any
  * intramodule features requiring these messages yet, so complain.
  */
static noinline fastcall int
__rc_r_rse(queue_t *q, mblk_t *mp, struct rc *rc)
{
	/* don't have any reserved messages yet */
	mi_strlog(q, STRLOGERR, SL_ERROR, "M_(PC)RSE on read queue!");
	freemsg(mp);
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_(HP)DATA MESSAGE Handling
 *
 *  -------------------------------------------------------------------------
 */

/** __rc_w_data: - process M_(HP)DATA message on write queue
  * @q: write queue
  * @mp: the M_(HP)DATA message
  * @rc: private structure (locked)
  *
  * Note that we really don't need to support M_DATA; however, for multi-Stream operation, expect
  * the CHS-Provider tag to be prepended to the message.
  */
static noinline fastcall int
__rc_w_data(queue_t *q, mblk_t *mp, struct rc *rc)
{
	struct ch *ch = NULL;
	int err = 0;

	if (!MBLKIN(mp, 0, sizeof(ch->ch_tag)))
		if (!mp->b_cont || !pullupmsg(mp, sizeof(ch->ch_tag)))
			return __rc_w_data_err(q, mp, rc);

	if (unlikely((ch = ch_tag_lookup(q, mp, rc, CH_DATA_REQ, &err)) == NULL && err != 0))
		return ch_error_reply(q, mp, rc, ch, CH_DATA_REQ, CHBADSLOT);

	return ch_write(q, mp, rc, ch);
}

/** __rc_r_data: - process M_(HP)DATA message on read queue
  * @q: read queue
  * @mp: the M_(HP)DATA message
  * @rc: private structure (locked)
  *
  * Note that we really don't need to support M_DATA; however, for multi-Stream operation, expect
  * the CHS-Provider tag to be prepended to the message.
  */
static noinline fastcall int
__rc_r_data(queue_t *q, mblk_t *mp, struct rc *rc)
{
	struct ch *ch = NULL;
	int err = 0;

	if (!MBLKIN(mp, 0, sizeof(ch->np_tag)))
		if (!mp->b_cont || !pullupmsg(mp, sizeof(ch->np_tag)))
			return __rc_r_data_err(q, mp, rc);

	if (unlikely((ch = np_tag_lookup(q, mp, rc, N_DATA_IND, &err)) == NULL && err != 0))
		return np_error_reply(q, mp, rc, ch, N_DATA_IND, NBADTOKEN);

	return n_read(q, mp, rc, ch);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  OTHER MESSAGE Handling
 *
 *  -------------------------------------------------------------------------
 */

/** rc_w_other: - process other message
  * @q: active queue (write queue)
  * @mp: the other message
  *
  * This function handles unknown or unrecognized messages.  Note that the
  * private structure does not require locking when calling this function.
  */
static noinline fastcall int
rc_w_other(queue_t *q, mblk_t *mp)
{
	register int type = DB_TYPE(mp);

	mi_strlog(q, STRLOGRX, SL_ERROR, "Unexpected message type %d received.", type);
	if (likely(pcmsg(type) || bcanputnext(q, mp->b_band))) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/** rc_r_other: - process other message
  * @q: active queue (read queue)
  * @mp: the other message
  *
  * This function handles unknown or unrecognized messages, M_COPYIN, M_COPYOUT,
  * M_IOCACK, M_IOCNAK, M_ERROR, M_HANGUP and M_UNHANGUP.  These are priority
  * communications with the Stream head, but are processed from the service
  * procedure, so they are all unexpected.  These priority messages should be
  * passed immediately from the put procedure.  Note that the private structure
  * does not require locking when calling this function.
  */
static noinline fastcall int
rc_r_other(queue_t *q, mblk_t *mp)
{
	register int type = DB_TYPE(mp);

	mi_strlog(q, STRLOGRX, SL_ERROR, "Unexpected message type %d received.", type);
	if (likely(pcmsg(type) || bcanputnext(q, mp->b_band))) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_FLUSH Handling
 *
 *  -------------------------------------------------------------------------
 */

/** rc_w_flush: - process M_FLUSH message
  * @q: active queue (write queue)
  * @mp: the M_FLUSH message
  *
  * Avoid having to push pipemod.  When we are the bottommost module over a pipe
  * twist then perform the actions of pipemod.  This means that the RTP-CH
  * module must be pushed over the same side of a pipe as pipemod, if pipemod is
  * pushed at all.  This is so that both don't fight each other at either end of
  * the pipe.  Note that the private structure does not require locking when
  * calling this function.
  */
static noinline fastcall int
rc_w_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHW) {
		if (unlikely(mp->b_rptr[0] & FLUSHBAND))
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHALL);
	}
	/* test for pipe twist beneath us */
	if (unlikely(!SAMESTR(q))) {
		switch (mp->b_rptr[0] & FLUSHW) {
		case FLUSHW:
			mp->b_rptr[0] &= ~FLUSHW;
			mp->b_rptr[0] |= FLUSHR;
			break;
		case FLUSHR:
			mp->b_rptr[0] &= ~FLUSHR;
			mp->b_rptr[0] |= FLUSHW;
			break;
		}
	}
	putnext(q, mp);
	return (0);
}

/** rc_r_flush: - process M_FLUSH message
  * @q: active queue (read queue)
  * @mp: the M_FLUSH message
  *
  * Avoid having to push pipemod.  When we are the bottommost module over a pipe
  * twist then perform the actions of pipemod.  This means that the RTP-CH
  * module must be pushed over the same side of a pipe as pipemod, if pipemod is
  * pushed at all.  This is so that both don't fight each other at either end of
  * the pipe.  Note that the private structure does not require locking when
  * calling this function.
  */
static noinline fastcall int
rc_r_flush(queue_t *q, mblk_t *mp)
{
	/* test for pipe twist beneath us */
	if (unlikely(!SAMESTR(_WR(q)))) {
		switch (mp->b_rptr[0] & FLUSHW) {
		case FLUSHW:
			mp->b_rptr[0] &= ~FLUSHW;
			mp->b_rptr[0] |= FLUSHR;
			break;
		case FLUSHR:
			mp->b_rptr[0] &= ~FLUSHR;
			mp->b_rptr[0] |= FLUSHW;
			break;
		}
	}
	if (mp->b_rptr[0] & FLUSHR) {
		if (unlikely(mp->b_rptr[0] & FLUSHBAND))
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHALL);
	}
	putnext(q, mp);
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  STREAMS MESSAGE DISTRIBUTION
 *
 *  -------------------------------------------------------------------------
 */

static inline fastcall int
rc_w_put_locked(queue_t *q, mblk_t *mp)
{
	struct rc *rc;

	if (likely((rc = (typeof(rc)) mi_trylock(q)) != NULL)) {
		int err;

		switch (__builtin_expect(DB_TYPE(mp), M_PCPROTO)) {
		case M_PCPROTO:
			err = __rc_w_proto(q, mp, rc);
			break;
		case M_PCSIG:
			err = __rc_w_sig(q, mp, rc);
			break;
		case M_PCCTL:
			err = __rc_w_ctl(q, mp, rc);
			break;
		case M_PCRSE:
			err = __rc_w_rse(q, mp, rc);
			break;
		case M_HPDATA:
			err = __rc_w_data(q, mp, rc);
			break;
		case M_IOCTL:
			/* M_IOCTL is the only normal priority message that we attempt to process
			   immediately. */
			err = __rc_w_ioctl(q, mp, rc);
			break;
		case M_IOCDATA:
			err = __rc_w_iocdata(q, mp, rc);
			break;
		default:
			err = -EFAULT;
			break;
		}
		mi_unlock((caddr_t) rc);
		return (err);
	}
	return (-EDEADLK);
}

static inline fastcall int
rc_r_put_locked(queue_t *q, mblk_t *mp)
{
	struct rc *rc;

	if (likely((rc = (typeof(rc)) mi_trylock(q)) != NULL)) {
		int err;

		switch (__builtin_expect(DB_TYPE(mp), M_PCPROTO)) {
		case M_PCPROTO:
			err = __rc_r_proto(q, mp, rc);
			break;
		case M_PCSIG:
			err = __rc_r_sig(q, mp, rc);
			break;
		case M_PCCTL:
			err = __rc_r_ctl(q, mp, rc);
			break;
		case M_PCRSE:
			err = __rc_r_rse(q, mp, rc);
			break;
		case M_HPDATA:
			err = __rc_r_data(q, mp, rc);
			break;
		default:
			err = -EFAULT;
			break;
		}
		mi_unlock((caddr_t) rc);
		return (err);
	}
	return (-EDEADLK);
}

/** rc_w_put_slow: - process a message for the write queue
  * @q: write queue
  * @mp: the message
  *
  * This is the slow path for processing messages, it should consider all
  * message types.  Note that the private structure is unlocked at this point.
  */
static noinline fastcall int
rc_w_put_slow(queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_PCPROTO)) {
	case M_PCPROTO:
	case M_PCSIG:
	case M_PCCTL:
	case M_PCRSE:
	case M_HPDATA:
	case M_IOCTL:
	case M_IOCDATA:
		return rc_w_put_locked(q, mp);
	case M_FLUSH:
		return rc_w_flush(q, mp);
	default:
		if (likely(pcmsg(DB_TYPE(mp))))
			return rc_w_other(q, mp);
		/* fall through */
		/* The following should never get here because the rc_w_put() function should have
		   stripped them off and already returned -EAGAIN for them. */
	case M_PROTO:		/* should never happen */
	case M_SIG:		/* should never happen */
	case M_CTL:		/* should never happen */
	case M_RSE:		/* should never happen */
	case M_DATA:		/* should never happen */
		return (-EAGAIN);
	}
}

/** rc_r_put_slow: - process a message for the read queue
  * @q: read queue
  * @mp: the message
  *
  * This is the slow path for processing messages, it should consider all
  * message types.  Note that the private structure is unlocked at this point.
  */
static noinline fastcall int
rc_r_put_slow(queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_PCPROTO)) {
	case M_PCPROTO:
	case M_PCSIG:
	case M_PCCTL:
	case M_PCRSE:
	case M_HPDATA:
		return rc_r_put_locked(q, mp);
	case M_FLUSH:
		return rc_r_flush(q, mp);
	case M_COPYIN:
	case M_COPYOUT:
	case M_IOCACK:
	case M_IOCNAK:
	case M_ERROR:
	case M_HANGUP:
	case M_UNHANGUP:
		/* These are all passed through because they are all priority messages.  We might
		   want to record the information passing by, but that is it. */
	default:
		if (likely(pcmsg(DB_TYPE(pm))))
			return rc_r_other(q, mp);
		/* fall through */
		/* The following should never get here because the rc_r_put() function should have
		   stripped them off and already returned -EAGAIN for them. */
	case M_PROTO:		/* should never happen */
	case M_SIG:		/* should never happen */
	case M_CTL:		/* should never happen */
	case M_RSE:		/* should never happen */
	case M_DATA:		/* should never happen */
		return (-EAGAIN);

	}
}

/** rc_w_srv: - process a message from the write queue
  * @rc: private structure (locked)
  * @q: write queue
  * @mp: the message
  */
static inline fastcall __hot_in int
rc_w_srv(struct rc *rc, queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_PROTO)) {
	case M_PROTO:
	case M_PCPROTO:
		return __rc_w_proto(q, mp, rc);
	case M_SIG:
	case M_PCSIG:
		return __rc_w_sig(q, mp, rc);
	case M_CTL:
	case M_PCCTL:
		return __rc_w_ctl(q, mp, rc);
	case M_RSE:
	case M_PCRSE:
		return __rc_w_rse(q, mp, rc);
	case M_DATA:
	case M_HPDATA:
		return __rc_w_data(q, mp, rc);
	case M_IOCTL:
		return __rc_w_ioctl(q, mp, rc);
	default:
		return rc_w_other(q, mp);
	case M_IOCDATA:	/* shouldn't happen */
		return __rc_w_iocdata(q, mp, rc);
	case M_FLUSH:		/* shouldn't happen */
		return rc_w_flush(q, mp);
	}
	return rc_w_srv_slow(rc, q, mp);
}

/** rc_r_srv: - process a message from the read queue
  * @q: read queue
  * @mp: the message
  * @rc: private structure (locked)
  */
static inline fastcall __hot_out int
rc_r_srv(queue_t *q, mblk_t *mp, struct rc *rc)
{
	switch (__builtin_expect(DB_TYPE(mp), M_PROTO)) {
	case M_PROTO:
	case M_PCPROTO:
		return __rc_r_proto(q, mp, rc);
	case M_SIG:
	case M_PCSIG:
		return __rc_r_sig(q, mp, rc);
	case M_CTL:
	case M_PCCTL:
		return __rc_r_ctl(q, mp, rc);
	case M_RSE:
	case M_PCRSE:
		return __rc_r_rse(q, mp, rc);
	case M_DATA:
	case M_HPDATA:
		return __rc_r_data(q, mp, rc);
	case M_COPYIN:		/* should never happen */
	case M_COPYOUT:	/* should never happen */
	case M_IOCACK:		/* should never happen */
	case M_IOCNAK:		/* should never happen */
	case M_ERROR:		/* should never happen */
	case M_HANGUP:		/* should never happen */
	case M_UNHANGUP:	/* should never happen */
	default:
		return rc_r_other(q, mp);
	case M_FLUSH:		/* should never happen */
		return rc_r_flush(q, mp);
	}
}

/** rc_data_type: - determine whether to queue message type
  * @type: the message type
  *
  * Indicates that normal messages M_PROTO, M_SIG, M_CTL, M_RSE and M_DATA
  * should be queued in the put procedure.  M_IOCTL messages should be processed
  * immediately if possible, and is therefore missing from the set of message
  * that will always be queued.
  */
static inline fastcall int
rc_data_type(int type)
{
	return ((1 << type) &
		((1 << M_PROTO) | (1 << M_SIG) | (1 << M_CTL) | (1 << M_RSE) | (1 << M_DATA)));
}

/** rc_w_put: - process a message for the write queue
  * @q: write queue
  * @mp: the message
  *
  * For performance reasons, all data messages (whether M_DATA or M_PROTO)
  * should be queued (i.e. -EAGAIN returned).  All priority messages should be
  * processed immediately.  Any high flow messages (M_PROTO) should be queued
  * (i.e. -EAGAIN returned).  M_IOCTL messages should be processed immediately,
  * if possible, and is therefore mssing from the set of messages that will
  * always be queued.
  */
static inline fastcall __hot_in int
rc_w_put(queue_t *q, mblk_t *mp)
{
	register int type = DB_TYPE(mp);

	if (likely(!pcmsg(type) && rc_data_type(type)))
		return (-EAGAIN);
	return rc_w_put_slow(q, mp);
}

/** rc_r_put: - process a message for the read queue
  * @q: read queue
  * @mp: the message
  *
  * For performance reasons, all data messages (whether M_DATA or M_PROTO)
  * should be queued (i.e. -EAGAIN returned).  All priority messages should be
  * processed immediately.  Any high flow messages (M_PROTO) should be queued
  * (i.e. -EAGAIN returned).
  */
static inline fastcall __hot_out int
rc_r_put(queue_t *q, mblk_t *mp)
{
	register int type = DB_TYPE(mp);

	if (likely(!pcmsg(type) && rc_data_type(type)))
		return (-EAGAIN);
	return rc_r_put_slow(q, mp);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  STREAMS QUEUE PUT AND SERVICE PROCEDURES
 *
 *  -------------------------------------------------------------------------
 */

/** rc_wput: - canonical put procedure
  * @q: active queue (write queue)
  * @mp: message to put
  *
  * Quick canonical put procedure.  To avoid missequencing of message
  * processing, for normal messages, when there are messages on the queue or the
  * service procedure is running concurrently, queue the normal message.  High
  * priority messages can be processed immediately, regardless of whether there
  * are other priority messages on the queue.  In general, if there are no
  * normal messages queued and there is no service procedure running
  * concurrently, some normal messages can be executed immediately.
  *
  * The put processing procedure
  * might return non-zero indicating that the message must be queued.
  */
static streamscall __hot_in int
rc_wput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || rc_w_put(q, mp)) {
		if (!putq(q, mp)) {
			mp->b_band = 0;	/* must succeed */
			putq(q, mp);
		}
	}
	return (0);
}

/** rc_wsrv: - canonical service procedure
  * @q: active queue (write queue)
  *
  * Quick canonical service procedure.  For efficiency, lock the private
  * structure before looping through the available messages, processing them one
  * by one.  Also, process a read queue wakeup, once for each service procedure
  * execution regardless of whether there are any messages on the queue.
  */
static streamscall __hot_in int
rc_wsrv(queue_t *q)
{
	struct rc *rc;
	mblk_t *mp;

	if (likely((rc = (typeof(rc)) mi_trylock(q)) != NULL)) {
		while (likely((mp = getq(q)) != NULL)) {
			if (unlikely(rc_w_srv(rc, q, mp) != 0)) {
				putbq(q, mp);
				break;
			}
		}
		rc_w_wakeup(q, rc);
		mi_unlock((caddr_t) rc);
	}
	return (0);
}

/** rc_rsrv: - canonical service procedure
  * @q: active queue (read queue)
  *
  * Quick canonical service procedure.  For efficiency, lock the private
  * structure before looping through the available messages, processing them one
  * by one.  Also, process a read queue wakeup, once for each service procedure
  * execution regardless of whether there are any messages on the queue.
  */
static streamscall __hot_out int
rc_rsrv(queue_t *q)
{
	struct rc *rc;
	mblk_t *mp;

	if (likely((rc = (typeof(rc)) mi_trylock(q)) != NULL)) {
		while (likely((mp = getq(q)) != NULL)) {
			if (unlikely(rc_r_srv(q, mp, rc) != 0)) {
				putbq(q, mp);
				break;
			}
		}
		rc_r_wakeup(q, rc);
		mi_unlock((caddr_t) rc);
	}
	return (0);
}

/** rc_rput: - canonical put procedure
  * @q: active queue (read queue)
  * @mp: message to put
  *
  * Quick canonical put procedure.  To avoid missequencing of message
  * processing, for normal messages, when there are messages on the queue or the
  * service procedure is running concurrently, queue the normal message.  High
  * priority messages can be processed immediately, regardless of whether there
  * are other priority messages on the queue.  The put processing procedure
  * might return non-zero indicating that the message must be queued.
  */
static streamscall __hot_out int
rc_rput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || rc_r_put(q, mp)) {
		if (!putq(q, mp)) {
			mp->b_band = 0;	/* must succeed */
			putq(q, mp);
		}
	}
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  STREAMS QUEUE OPEN AND CLOSE ROUTINES
 *
 *  -------------------------------------------------------------------------
 */

static caddr_t rc_opens = NULL;

/** rc_qopen: - module queue open procedure
  * @q: read queue of queue pair
  * @devp: device number of driver
  * @oflags: flags to open(2) call
  * @sflag: STREAMS flag
  * @crp: credentials of opening process
  */
static streamscall int
rc_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct rc *rc;
	mblk_t *tick;
	int err;

	if (q->q_ptr)
		return (0);	/* already open */
	if ((tick = mi_timer_alloc(0)) == NULL)
		return (ENOBUFS);
	if ((err = mi_open_comm(&rc_opens, sizeof(*rc), q, devp, oflags, sflag, crp))) {
		mi_timer_free(tick);
		return (err);
	}
	rc = PRIV(q);
	bzero(rc, sizeof(*rc));

	/* initialize the structure */
	rc->rq = RD(q);
	rc->wq = WR(q);
	rc->tick = tick;
	rc->interval = 10;	/* milliseconds */

	rc->oldstate.ch_state = CHS_UNINIT;
	rc->oldstate.ch_tag = 0;
	rc->oldstate.np_state = NS_UNBND;
	rc->oldstate.np_tag = 0;
	rc->oldstate.flags = 0;

	rc->info.ch_primitive = CH_INFO_ACK;
	rc->info.ch_addr_length = 0;
	rc->info.ch_addr_offset = 0;
	rc->info.ch_parm_length = 0;
	rc->info.ch_parm_offset = 0;
	rc->info.ch_prov_flags = 0;
	rc->info.ch_prov_class = CH_CIRCUIT;
	rc->info.ch_style = CH_STYLE3;
	rc->info.ch_version = CH_VERSION;
	rc->info.ch_state = CHS_UNINIT;

	rc->parms.cp_type = CH_PARMS_CIRCUIT;
	rc->parms.cp_encoding = CH_ENCODING_G711_PCM_U;
	rc->parms.cp_block_size = 1920;
	rc->parms.cp_samples = 240;
	rc->parms.cp_sample_size = 8;
	rc->parms.cp_rate = 8000;
	rc->parms.cp_tx_channels = 1;
	rc->parms.cp_rx_channels = 1;
	rc->parms.cp_flags = 0;

	qprocson(q);
	return (0);
}

/** rc_qclose: - module queue close procedure
  * @q: queue pair
  * @oflags: flags to open(2) call
  * @crp: credentials of closing process
  */
static streamscall int
rc_qclose(queue_t *q, int oflags, cred_t *crp)
{
	struct rc *rc = PRIV(q);

	qprocsoff(q);
	mi_timer_free(rc->ticks);
	mi_close_comm(&rc_opens, q);
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  REGISTRATION AND INITIALIZATION
 *
 *  -------------------------------------------------------------------------
 */

static struct qinit rc_rinit = {
	.qi_putp = rc_rput,	/* read put (message from below) */
	.qi_srvp = rc_rserv,	/* read queue service */
	.qi_qopen = rc_qopen,	/* each open */
	.qi_qclose = rc_qclose,	/* last close */
	.qi_minfo = &rc_minfo,	/* information */
	.qi_mstat = &rc_rstat,	/* statistics */
};

static struct qinit rc_winit = {
	.qi_putp = rc_wput,	/* write put (message from above) */
	.qi_srvp = rc_wsrv,	/* write queue service */
	.qi_minfo = &rc_minfo,	/* information */
	.qi_mstat = &rc_wstat,	/* statistics */
};

static struct streamtab rtpchinfo = {
	.st_rdinit = &rc_rinit,	/* upper read queue */
	.st_wrinit = &rc_winit,	/* lower write queue */
};

/*
 *  -------------------------------------------------------------------------
 *
 *  LINUX INITIALIZATION
 *
 *  -------------------------------------------------------------------------
 */

#ifdef LINUX
unsigned shrot modid = MOD_ID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else				/* module_param */
module_param(modid, short, 0444);
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for RTP-CH module. (0 for allocation.)");

static struct fmodsw rtpch_fmod = {
	.f_name = MOD_NAME,
	.f_str = &rtpchinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

/** rtpchinit: - initialize RTP-CH
  */
static __init int
rtpchinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);
	if ((err = register_strmod(&rtpch_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module, err = %d", MOD_NAME, err);
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

/** rtpchexit: - terminate RTP-CH
  */
static __exit void
rtpchexit(void)
{
	int err;

	if ((err = unregister_strmod(&rtpch_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister module", MOD_NAME);
		return;
	}
	return;
}

module_init(rtpchinit);
module_exit(rtpchexit);

#endif				/* LINUX */
