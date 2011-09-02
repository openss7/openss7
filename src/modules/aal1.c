/*****************************************************************************

 @(#) $RCSfile: aal1.c,v $ $Name:  $($Revision: 1.1.2.3 $) $Date: 2011-09-02 08:46:51 $

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

 Last Modified $Date: 2011-09-02 08:46:51 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: aal1.c,v $
 Revision 1.1.2.3  2011-09-02 08:46:51  brian
 - sync up lots of repo and build changes from git

 Revision 1.1.2.2  2010-11-28 14:22:00  brian
 - remove #ident, protect _XOPEN_SOURCE

 Revision 1.1.2.1  2009-06-21 11:40:29  brian
 - added files to new distro

 *****************************************************************************/

static char const ident[] = "$RCSfile: aal1.c,v $ $Name:  $($Revision: 1.1.2.3 $) $Date: 2011-09-02 08:46:51 $";

/*
 * This is an ATM Adaptation Layer (AAL) Type 1 (AAL1) for passing packets of fixed length across
 * the ATM network.  This is a pushable STREAMS module that may be pushed overa n ATM VC Stream to
 * effect the AAL1 service.  For the upper interface boundary, see MXI.  For the lower interface
 * boundary, see DLPI.
 */

#define _SVR4_SOURCE	1
#define _MPS_SOURCE	1

#include <sys/os7/compat.h>
#include <sys/strsun.h>
#include <sys/strconf.h>

#include <sys/mxi.h>
#include <sys/dlpi.h>

#define AAL1_DESCRIP	"MTP3B-AAL1 STREAMS MODULE."
#define AAL1_REVISION	"OpenSS7 $RCSfile: aal1.c,v $ $Name:  $($Revision: 1.1.2.3 $) $Date: 2011-09-02 08:46:51 $"
#define AAL1_COPYRIGHT	"Copyright (c) 2008-2011  Monavacon Limited.  All Rights Reserved."
#define AAL1_DEVICE	"Provides OpenSS7 MTP3B-I.432.3-AAL1 module."
#define AAL1_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define AAL1_LICENSE	"GPL"
#define AAL1_BANNER	AAL1_DESCRIP	"\n" \
			AAL1_REVISION	"\n" \
			AAL1_COPYRIGHT	"\n" \
			AAL1_DEVICE	"\n" \
			AAL1_CONTACT	"\n"
#define AAL1_SPLASH	AAL1_DEVICE	" - " \
			AAL1_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(AAL1_CONTACT);
MODULE_DESCRIPTION(AAL1_DESCRIP);
MODULE_SUPPORTED_DEVICE(AAL1_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(AAL1_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-aal1");
#endif				/* MODULE_ALIAS */
#ifdef MODULE_VERSION
MODULE_VERSION(PACKAGE_ENVR);
#endif				/* MODULE_VERSION */
#endif				/* LINUX */

#define AAL1_MOD_ID	CONFIG_STREAMS_AAL1_MODID
#define AAL1_MOD_NAME	CONFIG_STREAMS_AAL1_NAME

#define MOD_ID		AAL1_MOD_ID
#define MOD_NAME	AAL1_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	AAL1_BANNER
#else				/* MODULE */
#define MOD_BANNER	AAL1_SPLASH
#endif				/* MODULE */

/*
 * PRIVATE STRUCTURE
 */

struct state {
	uint32_t state;
	uint32_t pending;
};

struct mx;
struct dl;

struct mx {
	struct dl *dl;
	queue_t *oq;
	queue_t *iq;
	struct state state, oldstate;
	struct MX_info_ack info;
	uint32_t ppa;
};

struct dl {
	struct mx *mx;
	queue_t *iq;
	queue_t *oq;
	struct state state, oldstate;
	dl_info_ack_t info;
	uint32_t ppa;
	mblk_t *dmsg;
	size_t dlen;
};

struct priv {
	queue_t *rq;
	queue_t *wq;
	struct mx mx;
	struct dl dl;
};

static rwlock_t a1_lock = RW_LOCK_UNLOCKED;
static caddr_t a1_opens;

/*
 * PRINTING STUFF
 */

/*
 * Printing primitives
 */

/* Upper */
static inline fastcall const char *
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

/* Lower */
static inline fastcall const char *
dl_primname(const dl_ulong prim)
{
	switch (prim) {
	case DL_INFO_REQ:
		return ("DL_INFO_REQ");
	case DL_INFO_ACK:
		return ("DL_INFO_ACK");
	case DL_ATTACH_REQ:
		return ("DL_ATTACH_REQ");
	case DL_DETACH_REQ:
		return ("DL_DETACH_REQ");
	case DL_BIND_REQ:
		return ("DL_BIND_REQ");
	case DL_BIND_ACK:
		return ("DL_BIND_ACK");
	case DL_UNBIND_REQ:
		return ("DL_UNBIND_REQ");
	case DL_OK_ACK:
		return ("DL_OK_ACK");
	case DL_ERROR_ACK:
		return ("DL_ERROR_ACK");
	case DL_SUBS_BIND_REQ:
		return ("DL_SUBS_BIND_REQ");
	case DL_SUBS_BIND_ACK:
		return ("DL_SUBS_BIND_ACK");
	case DL_SUBS_UNBIND_REQ:
		return ("DL_SUBS_UNBIND_REQ");
	case DL_ENABMULTI_REQ:
		return ("DL_ENABMULTI_REQ");
	case DL_DISABMULTI_REQ:
		return ("DL_DISABMULTI_REQ");
	case DL_PROMISCON_REQ:
		return ("DL_PROMISCON_REQ");
	case DL_PROMISCOFF_REQ:
		return ("DL_PROMISCOFF_REQ");
	case DL_UNITDATA_REQ:
		return ("DL_UNITDATA_REQ");
	case DL_UNITDATA_IND:
		return ("DL_UNITDATA_IND");
	case DL_UDERROR_IND:
		return ("DL_UDERROR_IND");
	case DL_UDQOS_REQ:
		return ("DL_UDQOS_REQ");
	case DL_CONNECT_REQ:
		return ("DL_CONNECT_REQ");
	case DL_CONNECT_IND:
		return ("DL_CONNECT_IND");
	case DL_CONNECT_RES:
		return ("DL_CONNECT_RES");
	case DL_CONNECT_CON:
		return ("DL_CONNECT_CON");
	case DL_TOKEN_REQ:
		return ("DL_TOKEN_REQ");
	case DL_TOKEN_ACK:
		return ("DL_TOKEN_ACK");
	case DL_DISCONNECT_REQ:
		return ("DL_DISCONNECT_REQ");
	case DL_DISCONNECT_IND:
		return ("DL_DISCONNECT_IND");
	case DL_RESET_REQ:
		return ("DL_RESET_REQ");
	case DL_RESET_IND:
		return ("DL_RESET_IND");
	case DL_RESET_RES:
		return ("DL_RESET_RES");
	case DL_RESET_CON:
		return ("DL_RESET_CON");
	case DL_DATA_ACK_REQ:
		return ("DL_DATA_ACK_REQ");
	case DL_DATA_ACK_IND:
		return ("DL_DATA_ACK_IND");
	case DL_DATA_ACK_STATUS_IND:
		return ("DL_DATA_ACK_STATUS_IND");
	case DL_REPLY_REQ:
		return ("DL_REPLY_REQ");
	case DL_REPLY_IND:
		return ("DL_REPLY_IND");
	case DL_REPLY_STATUS_IND:
		return ("DL_REPLY_STATUS_IND");
	case DL_REPLY_UPDATE_REQ:
		return ("DL_REPLY_UPDATE_REQ");
	case DL_REPLY_UPDATE_STATUS_IND:
		return ("DL_REPLY_UPDATE_STATUS_IND");
	case DL_XID_REQ:
		return ("DL_XID_REQ");
	case DL_XID_IND:
		return ("DL_XID_IND");
	case DL_XID_RES:
		return ("DL_XID_RES");
	case DL_XID_CON:
		return ("DL_XID_CON");
	case DL_TEST_REQ:
		return ("DL_TEST_REQ");
	case DL_TEST_IND:
		return ("DL_TEST_IND");
	case DL_TEST_RES:
		return ("DL_TEST_RES");
	case DL_TEST_CON:
		return ("DL_TEST_CON");
	case DL_PHYS_ADDR_REQ:
		return ("DL_PHYS_ADDR_REQ");
	case DL_PHYS_ADDR_ACK:
		return ("DL_PHYS_ADDR_ACK");
	case DL_SET_PHYS_ADDR_REQ:
		return ("DL_SET_PHYS_ADDR_REQ");
	case DL_GET_STATISTICS_REQ:
		return ("DL_GET_STATISTICS_REQ");
	case DL_GET_STATISTICS_ACK:
		return ("DL_GET_STATISTICS_ACK");
	default:
		return ("(unknown)");
	}
}

/*
 * Printing States
 */

/* Upper */
static inline fastcall const char *
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

/* Lower */
static inline fastcall const char *
dl_statename(const dl_ulong state)
{
	switch (state) {
	case DL_UNATTACHED:
		return ("DL_UNATTACHED");
	case DL_ATTACH_PENDING:
		return ("DL_ATTACH_PENDING");
	case DL_DETACH_PENDING:
		return ("DL_DETACH_PENDING");
	case DL_UNBOUND:
		return ("DL_UNBOUND");
	case DL_BIND_PENDING:
		return ("DL_BIND_PENDING");
	case DL_UNBIND_PENDING:
		return ("DL_UNBIND_PENDING");
	case DL_IDLE:
		return ("DL_IDLE");
	case DL_UDQOS_PENDING:
		return ("DL_UDQOS_PENDING");
	case DL_OUTCON_PENDING:
		return ("DL_OUTCON_PENDING");
	case DL_INCON_PENDING:
		return ("DL_INCON_PENDING");
	case DL_CONN_RES_PENDING:
		return ("DL_CONN_RES_PENDING");
	case DL_DATAXFER:
		return ("DL_DATAXFER");
	case DL_USER_RESET_PENDING:
		return ("DL_USER_RESET_PENDING");
	case DL_PROV_RESET_PENDING:
		return ("DL_PROV_RESET_PENDING");
	case DL_RESET_RES_PENDING:
		return ("DL_RESET_RES_PENDING");
	case DL_DISCON8_PENDING:
		return ("DL_DISCON8_PENDING");
	case DL_DISCON9_PENDING:
		return ("DL_DISCON9_PENDING");
	case DL_DISCON11_PENDING:
		return ("DL_DISCON11_PENDING");
	case DL_DISCON12_PENDING:
		return ("DL_DISCON12_PENDING");
	case DL_DISCON13_PENDING:
		return ("DL_DISCON13_PENDING");
	case DL_SUBS_BIND_PND:
		return ("DL_SUBS_BIND_PND");
	case DL_SUBS_UNBIND_PND:
		return ("DL_SUBS_UNBIND_PND");
	default:
		return ("(unknown)");
	}
}

static inline fastcall const char *
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
	default:
		return ("(unknown)");
	}
}

static inline fastcall const char *
dl_errname(const dl_long error)
{
	switch (error) {
	case DL_ACCESS:
		return ("DL_ACCESS");
	case DL_BADADDR:
		return ("DL_BADADDR");
	case DL_BADCORR:
		return ("DL_BADCORR");
	case DL_BADDATA:
		return ("DL_BADDATA");
	case DL_BADPPA:
		return ("DL_BADPPA");
	case DL_BADPRIM:
		return ("DL_BADPRIM");
	case DL_BADQOSPARAM:
		return ("DL_BADQOSPARAM");
	case DL_BADQOSTYPE:
		return ("DL_BADQOSTYPE");
	case DL_BADSAP:
		return ("DL_BADSAP");
	case DL_BADTOKEN:
		return ("DL_BADTOKEN");
	case DL_BOUND:
		return ("DL_BOUND");
	case DL_INITFAILED:
		return ("DL_INITFAILED");
	case DL_NOADDR:
		return ("DL_NOADDR");
	case DL_NOTINIT:
		return ("DL_NOTINIT");
	case DL_OUTSTATE:
		return ("DL_OUTSTATE");
	case DL_SYSERR:
		return ("DL_SYSERR");
	case DL_UNSUPPORTED:
		return ("DL_UNSUPPORTED");
	case DL_UNDELIVERABLE:
		return ("DL_UNDELIVERABLE");
	case DL_NOTSUPPORTED:
		return ("DL_NOTSUPPORTED");
	case DL_TOOMANY:
		return ("DL_TOOMANY");
	case DL_NOTENAB:
		return ("DL_NOTENAB");
	case DL_BUSY:
		return ("DL_BUSY");
	case DL_NOAUTO:
		return ("DL_NOAUTO");
	case DL_NOXIDAUTO:
		return ("DL_NOXIDAUTO");
	case DL_NOTESTAUTO:
		return ("DL_NOTESTAUTO");
	case DL_XIDAUTO:
		return ("DL_XIDAUTO");
	case DL_TESTAUTO:
		return ("DL_TESTAUTO");
	case DL_PENDING:
		return ("DL_PENDING");
	default:
		return ("(unknown)");
	}
}

static inline fastcall const char *
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
		return ("Bad paramater structure type");
	case MXBADFLAG:
		return ("Bad flag");
	case MXBADPRIM:
		return ("Bad primitive");
	case MXNOTSUPP:
		return ("Primitive not supported");
	case MXBADSLOT:
		return ("Bad multplex slot");
	default:
		return ("Unknown error");
	}
}

static inline fastcall const char *
dl_strerror(const dl_long error)
{
	switch (error) {
	case DL_ACCESS:
		return ("Improper permissions for request");
	case DL_BADADDR:
		return ("DLSAP addr in improper format or invalid");
	case DL_BADCORR:
		return ("Seq number not from outstand DL_CONN_IND");
	case DL_BADDATA:
		return ("User data exceeded provider limit");
	case DL_BADPPA:
		return ("Specified PPA was invalid");
	case DL_BADPRIM:
		return ("Primitive received not known by provider");
	case DL_BADQOSPARAM:
		return ("QOS parameters contained invalid values");
	case DL_BADQOSTYPE:
		return ("QOS structure type is unknown/unsupported");
	case DL_BADSAP:
		return ("Bad LSAP selector");
	case DL_BADTOKEN:
		return ("Token used not an active stream");
	case DL_BOUND:
		return ("Attempted second bind with dl_max_conind");
	case DL_INITFAILED:
		return ("Physical Link initialization failed");
	case DL_NOADDR:
		return ("Provider couldn't allocate alt. address");
	case DL_NOTINIT:
		return ("Physical Link not initialized");
	case DL_OUTSTATE:
		return ("Primitive issued in improper state");
	case DL_SYSERR:
		return ("UNIX system error occurred");
	case DL_UNSUPPORTED:
		return ("Requested serv. not supplied by provider");
	case DL_UNDELIVERABLE:
		return ("Previous data unit could not be delivered");
	case DL_NOTSUPPORTED:
		return ("Primitive is known but not supported");
	case DL_TOOMANY:
		return ("Limit exceeded");
	case DL_NOTENAB:
		return ("Promiscuous mode not enabled");
	case DL_BUSY:
		return ("Other streams for PPA in post-attached state");
	case DL_NOAUTO:
		return ("Automatic handling of XID and TEST response not supported. ");
	case DL_NOXIDAUTO:
		return ("Automatic handling of XID not supported");
	case DL_NOTESTAUTO:
		return ("Automatic handling of TEST not supported");
	case DL_XIDAUTO:
		return ("Automatic handling of XID response");
	case DL_TESTAUTO:
		return ("Automatic handling of TEST response");
	case DL_PENDING:
		return ("pending outstanding connect indications");
	default:
		return ("Unknown error");
	}
}

/*
 * STATE CHANGES
 */

static inline fastcall mx_ulong
mx_get_state(struct mx *mx)
{
	return mx->state.state;
}
static inline fastcall mx_ulong
mx_set_state(struct mx *mx, const mx_ulong newstate)
{
	const mx_ulong oldstate = mx_get_state(mx);

	if (likely(oldstate != newstate)) {
		mi_strlog(mx->iq, STRLOGST, SL_TRACE, "%s <- %s", mx_statename(newstate),
			  mx_statename(oldstate));
		mx->state.state = newstate;
		mx->info.mx_state = newstate;
	}
	return (newstate);
}
static inline fastcall mx_ulong
mx_save_state(struct mx *mx)
{
	mx->oldstate = mx->state;
	return (mx->state.state);
}
static inline fastcall mx_ulong
mx_restore_state(struct mx *mx)
{
	mx->state = mx->oldstate;
	return (mx->info.mx_state = mx->state.state);
}
static inline fastcall mx_ulong
mx_set_pending(struct mx *mx, const mx_ulong pending)
{
	return (mx->state.pending = pending);
}
static inline fastcall mx_ulong
mx_get_pending(struct mx *mx)
{
	return mx->state.pending;
}
static inline fastcall mx_ulong
mx_clear_pending(struct mx *mx)
{
	const mx_ulong pending = mx->state.pending;

	mx->state.pending = -1U;
	return (pending);
}

static inline fastcall dl_ulong
dl_get_state(struct dl *dl)
{
	return dl->state.state;
}
static inline fastcall dl_ulong
dl_set_state(struct dl *dl, const dl_ulong newstate)
{
	const dl_ulong oldstate = dl_get_state(dl);

	if (likely(oldstate != newstate)) {
		mi_strlog(dl->iq, STRLOGST, SL_TRACE, "%s <- %s", dl_statename(newstate),
			  dl_statename(oldstate));
		dl->state.state = newstate;
		dl->info.dl_current_state = newstate;
	}
	return (newstate);
}
static inline fastcall dl_ulong
dl_save_state(struct dl *dl)
{
	dl->oldstate = dl->state;
	return (dl->state.state);
}
static inline fastcall dl_ulong
dl_restore_state(struct dl *dl)
{
	dl->state = dl->oldstate;
	return (dl->info.dl_current_state = dl->state.state);
}
static inline fastcall dl_ulong
dl_set_pending(struct dl *dl, const dl_ulong pending)
{
	return (dl->state.pending = pending);
}
static inline fastcall dl_ulong
dl_get_pending(struct dl *dl)
{
	return dl->state.pending;
}
static inline fastcall dl_ulong
dl_clear_pending(struct dl *dl)
{
	const dl_ulong pending = dl->state.pending;

	dl->state.pending = -1U;
	return (pending);
}

/*
 * MESSAGE LOGGING
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

static inline fastcall void
a1_stripmsg(mblk_t *msg, mblk_t *dp)
{
	mblk_t *b, *b_next = msg;

	while ((b = b_next) && b != dp) {
		b_next = b->b_next;
		freeb(b);
	}
	return;
}

noinline fastcall int
mx_txprim(struct mx *mx, queue_t *q, mblk_t *msg, mblk_t *mp, mx_ulong prim)
{
	if (likely(pcmsg(DB_TYPE(mp)) || bcanputnext(mx->oq, mp->b_band))) {
		int level;

		switch (__builtin_expect(prim, MX_DATA_REQ)) {
		case MX_DATA_REQ:
			level = STRLOGDA;
			break;
		default:
			level = STRLOGTX;
			break;
		}
		mi_strlog(mx->oq, level, SL_TRACE, "<- %s", mx_primname(prim));
		a1_stripmsg(msg, mp->b_cont);
		putnext(mx->oq, mp);
		return (0);
	}
	freeb(mp);
	return (-EBUSY);
}

noinline fastcall int
dl_txprim(struct dl *dl, queue_t *q, mblk_t *msg, mblk_t *mp, dl_ulong prim)
{
	if (likely(pcmsg(DB_TYPE(mp)) || bcanputnext(dl->oq, mp->b_band))) {
		mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "%s ->", dl_primname(prim));
		a1_stripmsg(msg, mp->b_cont);
		putnext(dl->oq, mp);
		return (0);
	}
	freeb(mp);
	return (-EBUSY);
}

noinline fastcall void
mx_rxprim(struct mx *mx, queue_t *q, mblk_t *mp, mx_ulong prim)
{
	int level;

	switch (__builtin_expect(prim, MX_DATA_IND)) {
	case MX_DATA_IND:
		level = STRLOGDA;
		break;
	default:
		level = STRLOGRX;
		break;
	}
	mi_strlog(mx->iq, level, SL_TRACE, "-> %s", mx_primname(prim));
	return;
}

noinline fastcall void
dl_rxprim(struct dl *dl, queue_t *q, mblk_t *mp, dl_ulong prim)
{
	mi_strlog(dl->oq, STRLOGRX, SL_TRACE, "%s <-", dl_primname(prim));
	return;
}

static inline fastcall int
m_error(queue_t *q, mblk_t *msg, int rerror, int werror)
{
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, 2, BPRI_MED)))) {
		DB_TYPE(mp) = M_ERROR;
		mp->b_wptr[0] = rerror;
		mp->b_wptr[1] = werror;
		mp->b_wptr += 2;
		mi_strlog(q, STRLOGERR, SL_ERROR | SL_TRACE, "<- M_ERROR");
		freemsg(msg);
		qreply(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 * MX PROVIDER TO MX USER PRIMITIVES
 */

static inline fastcall int
mx_info_ack(struct mx *mx, queue_t *q, mblk_t *msg)
{
	struct MX_info_ack *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_INFO_ACK;
		mp->b_wptr += sizeof(*p);
		return mx_txprim(mx, q, msg, mp, MX_INFO_ACK);
	}
	return (-ENOBUFS);
}
static inline fastcall int
mx_optmgmt_ack(struct mx *mx, queue_t *q, mblk_t *msg)
{
	struct MX_optmgmt_ack *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_OPTMGMT_ACK;
		mp->b_wptr += sizeof(*p);
		return mx_txprim(mx, q, msg, mp, MX_OPTMGMT_ACK);
	}
	return (-ENOBUFS);
}
static inline fastcall int
mx_ok_ack(struct mx *mx, queue_t *q, mblk_t *msg)
{
	struct MX_ok_ack *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_OK_ACK;
		mp->b_wptr += sizeof(*p);
		return mx_txprim(mx, q, msg, mp, MX_OK_ACK);
	}
	return (-ENOBUFS);
}
static inline fastcall int
mx_error_ack(struct mx *mx, queue_t *q, mblk_t *msg, mx_ulong prim, mx_long error)
{
	struct MX_error_ack *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_ERROR_ACK;
		p->mx_error_primitive = prim;
		p->mx_error_type = error < 0 ? MXSYSERR : error;
		p->mx_unix_error = error < 0 ? -error : 0;
		mp->b_wptr += sizeof(*p);
		return mx_txprim(mx, q, msg, mp, MX_ERROR_ACK);
	}
	return (-ENOBUFS);
}
noinline fastcall __unlikely int
mx_reply_error(struct mx *mx, queue_t *q, mblk_t *msg, mx_ulong prim, mx_long error)
{
	switch (error) {
	case 0:
		freemsg(msg);
		return (0);
	case -EAGAIN:
	case -ENOBUFS:
	case -ENOMEM:
	case -EDEADLK:
	case -EBUSY:
		return (error);
	}
	switch (prim) {
	case MX_DATA_REQ:
		goto merror;
	default:
		goto errorack;
	}
      errorack:
	mi_strlog(mx->oq, STRLOGNO, SL_TRACE, "ERROR: %s: (%s) %s", mx_primname(prim),
		  mx_errname(error), mx_strerror(error));
	return mx_error_ack(mx, q, msg, prim, error);
      merror:
	return m_error(q, msg, EPROTO, EPROTO);
}
static inline fastcall int
mx_enable_con(struct mx *mx, queue_t *q, mblk_t *msg)
{
	struct MX_enable_con *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_ENABLE_CON;
		mp->b_wptr += sizeof(*p);
		return mx_txprim(mx, q, msg, mp, MX_ENABLE_CON);
	}
	return (-ENOBUFS);
}
static inline fastcall int
mx_connect_con(struct mx *mx, queue_t *q, mblk_t *msg)
{
	struct MX_connect_con *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_CONNECT_CON;
		mp->b_wptr += sizeof(*p);
		return mx_txprim(mx, q, msg, mp, MX_CONNECT_CON);
	}
	return (-ENOBUFS);
}
static inline fastcall int
mx_data_ind(struct mx *mx, queue_t *q, mblk_t *msg)
{
	struct MX_data_ind *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_DATA_IND;
		mp->b_wptr += sizeof(*p);
		return mx_txprim(mx, q, msg, mp, MX_DATA_IND);
	}
	return (-ENOBUFS);
}
static inline fastcall int
mx_disconnect_ind(struct mx *mx, queue_t *q, mblk_t *msg)
{
	struct MX_disconnect_ind *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_DISCONNECT_IND;
		mp->b_wptr += sizeof(*p);
		return mx_txprim(mx, q, msg, mp, MX_DISCONNECT_IND);
	}
	return (-ENOBUFS);
}
static inline fastcall int
mx_disconnect_con(struct mx *mx, queue_t *q, mblk_t *msg)
{
	struct MX_disconnect_con *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_DISCONNECT_CON;
		mp->b_wptr += sizeof(*p);
		return mx_txprim(mx, q, msg, mp, MX_DISCONNECT_CON);
	}
	return (-ENOBUFS);
}
static inline fastcall int
mx_disable_ind(struct mx *mx, queue_t *q, mblk_t *msg)
{
	struct MX_disable_ind *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_DISABLE_IND;
		mp->b_wptr += sizeof(*p);
		return mx_txprim(mx, q, msg, mp, MX_DISABLE_IND);
	}
	return (-ENOBUFS);
}
static inline fastcall int
mx_disable_con(struct mx *mx, queue_t *q, mblk_t *msg)
{
	struct MX_disable_con *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_DISABLE_CON;
		mp->b_wptr += sizeof(*p);
		return mx_txprim(mx, q, msg, mp, MX_DISABLE_CON);
	}
	return (-ENOBUFS);
}
static inline fastcall int
mx_event_ind(struct mx *mx, queue_t *q, mblk_t *msg)
{
	struct MX_event_ind *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_EVENT_IND;
		mp->b_wptr += sizeof(*p);
		return mx_txprim(mx, q, msg, mp, MX_EVENT_IND);
	}
	return (-ENOBUFS);
}

/*
 * DL USER TO DL PROVIDER PRIMITIVES
 */

static inline fastcall int
dl_info_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_info_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_INFO_REQ;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_INFO_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_attach_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_attach_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_ATTACH_REQ;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_ATTACH_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_detach_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_detach_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_DETACH_REQ;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_DETACH_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_bind_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_bind_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_BIND_REQ;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_BIND_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_unbind_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_unbind_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_UNBIND_REQ;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_UNBIND_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_subs_bind_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_subs_bind_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_SUBS_BIND_REQ;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_SUBS_BIND_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_subs_unbind_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_subs_unbind_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_SUBS_UNBIND_REQ;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_SUBS_UNBIND_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_enabmulti_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_enabmulti_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_ENABMULTI_REQ;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_ENABMULTI_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_disabmulti_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_disabmulti_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_DISABMULTI_REQ;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_DISABMULTI_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_promiscon_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_promiscon_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_PROMISCON_REQ;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_PROMISCON_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_promiscoff_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_promiscoff_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_PROMISCOFF_REQ;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_PROMISCOFF_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_unitdata_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_unitdata_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_UNITDATA_REQ;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_UNITDATA_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_udqos_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_udqos_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_UDQOS_REQ;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_UDQOS_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_connect_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_connect_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_CONNECT_REQ;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_CONNECT_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_connect_res(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_connect_res_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_CONNECT_RES;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_CONNECT_RES);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_token_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_token_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_TOKEN_REQ;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_TOKEN_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_disconnect_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_disconnect_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_DISCONNECT_REQ;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_DISCONNECT_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_reset_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_reset_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_RESET_REQ;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_RESET_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_reset_res(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_reset_res_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_RESET_RES;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_RESET_RES);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_data_ack_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_data_ack_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_DATA_ACK_REQ;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_DATA_ACK_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_reply_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_reply_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_REPLY_REQ;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_REPLY_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_reply_update_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_reply_update_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_REPLY_UPDATE_REQ;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_REPLY_UPDATE_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_xid_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_xid_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_XID_REQ;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_XID_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_xid_res(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_xid_res_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_XID_RES;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_XID_RES);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_test_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_test_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_TEST_REQ;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_TEST_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_test_res(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_test_res_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_TEST_RES;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_TEST_RES);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_phys_addr_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_phys_addr_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_PHYS_ADDR_REQ;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_PHYS_ADDR_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_set_phys_addr_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_set_phys_addr_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_SET_PHYS_ADDR_REQ;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_SET_PHYS_ADDR_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_get_statistics_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_get_statistics_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_GET_STATISTICS_REQ;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_GET_STATISTICS_REQ);
	}
	return (-ENOBUFS);
}

noinline fastcall __unlikely int
dl_reply_error(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong prim, dl_long error)
{
	switch (error) {
	case 0:
		freemsg(msg);
		return (0);
	case -EAGAIN:
	case -ENOBUFS:
	case -ENOMEM:
	case -EDEADLK:
	case -EBUSY:
		return (error);
	}
	mi_strlog(dl->oq, STRLOGNO, SL_TRACE, "ERROR: %s: (%s) %s", dl_primname(prim),
		  dl_errname(error), dl_strerror(error));
	freemsg(msg);
	return (0);
}

/*
 * MX USER TO MX PROVIDER PRIMITIVES
 */

noinline fastcall __unlikely int
mx_info_req(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_info_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = MXNOTSUPP;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
mx_optmgmt_req(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_optmgmt_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = MXNOTSUPP;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
mx_attach_req(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_attach_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = MXNOTSUPP;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
mx_enable_req(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_enable_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = MXNOTSUPP;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
mx_connect_req(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_connect_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = MXNOTSUPP;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
static inline fastcall __hot_write int
mx_write(struct mx *mx, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	/* FIXME: for now */
	return DL_NOTSUPPORTED;
}
noinline fastcall __unlikely int
mx_data_req(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_data_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	return mx_write(mx, q, mp, mp->b_cont);
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
mx_disconnect_req(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_disconnect_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = MXNOTSUPP;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
mx_disable_req(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_disable_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = MXNOTSUPP;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
mx_detach_req(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_detach_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = MXNOTSUPP;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/*
 * DL PROVIDER TO DL USER PRIMITIVES
 */

noinline fastcall __unlikely int
dl_info_ack(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_info_ack_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = DL_UNSUPPORTED;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
dl_bind_ack(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_bind_ack_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = DL_UNSUPPORTED;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
dl_ok_ack(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_ok_ack_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = DL_UNSUPPORTED;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
dl_error_ack(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_error_ack_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = DL_UNSUPPORTED;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
dl_subs_bind_ack(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_subs_bind_ack_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = DL_UNSUPPORTED;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
dl_unitdata_ind(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_unitdata_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = DL_UNSUPPORTED;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
dl_uderror_ind(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_uderror_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = DL_UNSUPPORTED;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
dl_connect_ind(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_connect_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = DL_UNSUPPORTED;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
dl_connect_con(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_connect_con_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = DL_UNSUPPORTED;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
dl_token_ack(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_token_ack_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = DL_UNSUPPORTED;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
dl_disconnect_ind(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_disconnect_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = DL_UNSUPPORTED;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
dl_reset_ind(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_reset_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = DL_UNSUPPORTED;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
dl_reset_con(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_reset_con_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = DL_UNSUPPORTED;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
dl_data_ack_ind(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_data_ack_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = DL_UNSUPPORTED;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
dl_data_ack_status_ind(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_data_ack_status_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = DL_UNSUPPORTED;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
dl_reply_ind(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_reply_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = DL_UNSUPPORTED;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
dl_reply_status_ind(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_reply_status_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = DL_UNSUPPORTED;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
dl_reply_update_status_ind(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_reply_update_status_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = DL_UNSUPPORTED;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
dl_xid_ind(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_xid_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = DL_UNSUPPORTED;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
dl_xid_con(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_xid_con_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = DL_UNSUPPORTED;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
dl_test_ind(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_test_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = DL_UNSUPPORTED;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
dl_test_con(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_test_con_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = DL_UNSUPPORTED;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
dl_phys_addr_ack(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_phys_addr_ack_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = DL_UNSUPPORTED;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
dl_get_statistics_ack(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_get_statistics_ack_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = DL_UNSUPPORTED;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

static inline fastcall __hot_read int
dl_read(struct dl *dl, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	/* FIXME: For now */
	return DL_NOTSUPPORTED;
}

/*
 * STREAMS MESSAGE TYPE HANDLING
 */

/**
 * mx_w_data: - process received M_DATA message from above
 * @mx: MX private structure (locked)
 * @q: active queue (upper write queue)
 * @mp: the M_DATA message
 */
static fastcall int
mx_w_data(struct mx *mx, queue_t *q, mblk_t *mp)
{
	int err;

	mx_save_state(mx);
	err = mx_write(mx, q, NULL, mp);
	if (likely(err == 0))
		return (err);
	mx_restore_state(mx);
	return mx_reply_error(mx, q, mp, MX_DATA_REQ, err);
}
static fastcall int
dl_r_data(struct dl *dl, queue_t *q, mblk_t *mp)
{
	int err;

	dl_save_state(dl);
	err = dl_read(dl, q, NULL, mp);
	if (likely(err == 0))
		return (err);
	dl_restore_state(dl);
	return dl_reply_error(dl, q, mp, -1U, err);
}

static fastcall int
mx_w_proto(struct mx *mx, queue_t *q, mblk_t *mp)
{
	mx_ulong prim = -1U;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(prim))))
		goto tooshort;
	prim = *(typeof(prim) *) mp->b_rptr;
	mx_rxprim(mx, q, mp, prim);
	mx_save_state(mx);
	switch (__builtin_expect(prim, MX_DATA_REQ)) {
	case MX_INFO_REQ:
		err = mx_info_req(mx, q, mp);
		break;
	case MX_OPTMGMT_REQ:
		err = mx_optmgmt_req(mx, q, mp);
		break;
	case MX_ATTACH_REQ:
		err = mx_attach_req(mx, q, mp);
		break;
	case MX_ENABLE_REQ:
		err = mx_enable_req(mx, q, mp);
		break;
	case MX_CONNECT_REQ:
		err = mx_connect_req(mx, q, mp);
		break;
	case MX_DATA_REQ:
		err = mx_data_req(mx, q, mp);
		break;
	case MX_DISCONNECT_REQ:
		err = mx_disconnect_req(mx, q, mp);
		break;
	case MX_DISABLE_REQ:
		err = mx_disable_req(mx, q, mp);
		break;
	case MX_DETACH_REQ:
		err = mx_detach_req(mx, q, mp);
		break;
	default:
		err = MXBADPRIM;
		break;
	}
	if (likely(err == 0))
		return (err);
      out:
	mx_restore_state(mx);
	return mx_reply_error(mx, q, mp, prim, err);
      tooshort:
	err = -EMSGSIZE;
	goto out;
}
static fastcall int
dl_r_proto(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_ulong prim = -1U;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(prim))))
		goto tooshort;
	prim = *(typeof(prim) *) mp->b_rptr;
	dl_rxprim(dl, q, mp, prim);
	dl_save_state(dl);
	switch (prim) {
	case DL_INFO_ACK:
		err = dl_info_ack(dl, q, mp);
		break;
	case DL_BIND_ACK:
		err = dl_bind_ack(dl, q, mp);
		break;
	case DL_OK_ACK:
		err = dl_ok_ack(dl, q, mp);
		break;
	case DL_ERROR_ACK:
		err = dl_error_ack(dl, q, mp);
		break;
	case DL_SUBS_BIND_ACK:
		err = dl_subs_bind_ack(dl, q, mp);
		break;
	case DL_UNITDATA_IND:
		err = dl_unitdata_ind(dl, q, mp);
		break;
	case DL_UDERROR_IND:
		err = dl_uderror_ind(dl, q, mp);
		break;
	case DL_CONNECT_IND:
		err = dl_connect_ind(dl, q, mp);
		break;
	case DL_CONNECT_CON:
		err = dl_connect_con(dl, q, mp);
		break;
	case DL_TOKEN_ACK:
		err = dl_token_ack(dl, q, mp);
		break;
	case DL_DISCONNECT_IND:
		err = dl_disconnect_ind(dl, q, mp);
		break;
	case DL_RESET_IND:
		err = dl_reset_ind(dl, q, mp);
		break;
	case DL_RESET_CON:
		err = dl_reset_con(dl, q, mp);
		break;
	case DL_DATA_ACK_IND:
		err = dl_data_ack_ind(dl, q, mp);
		break;
	case DL_DATA_ACK_STATUS_IND:
		err = dl_data_ack_status_ind(dl, q, mp);
		break;
	case DL_REPLY_IND:
		err = dl_reply_ind(dl, q, mp);
		break;
	case DL_REPLY_STATUS_IND:
		err = dl_reply_status_ind(dl, q, mp);
		break;
	case DL_REPLY_UPDATE_STATUS_IND:
		err = dl_reply_update_status_ind(dl, q, mp);
		break;
	case DL_XID_IND:
		err = dl_xid_ind(dl, q, mp);
		break;
	case DL_XID_CON:
		err = dl_xid_con(dl, q, mp);
		break;
	case DL_TEST_IND:
		err = dl_test_ind(dl, q, mp);
		break;
	case DL_TEST_CON:
		err = dl_test_con(dl, q, mp);
		break;
	case DL_PHYS_ADDR_ACK:
		err = dl_phys_addr_ack(dl, q, mp);
		break;
	case DL_GET_STATISTICS_ACK:
		err = dl_get_statistics_ack(dl, q, mp);
		break;
	default:
		err = DL_BADPRIM;
		break;
	}
	if (likely(err == 0))
		return (err);
      out:
	dl_restore_state(dl);
	return dl_reply_error(dl, q, mp, prim, err);
      tooshort:
	err = -EMSGSIZE;
	goto out;
}

static fastcall int
mx_w_ctl(struct mx *mx, queue_t *q, mblk_t *mp)
{
	/* FIXME: for now */
	freemsg(mp);
	return (0);
}
static fastcall int
dl_r_ctl(struct dl *dl, queue_t *q, mblk_t *mp)
{
	/* FIXME: for now */
	freemsg(mp);
	return (0);
}

static fastcall int
mx_w_ioctl(struct mx *mx, queue_t *q, mblk_t *mp)
{
	/* FIXME: for now */
	freemsg(mp);
	return (0);
}
static fastcall int
dl_r_ioctl(struct dl *dl, queue_t *q, mblk_t *mp)
{
	/* FIXME: for now */
	freemsg(mp);
	return (0);
}

static fastcall int
m_w_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
	}
	putnext(q, mp);
	return (0);
}
static fastcall int
m_r_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
	}
	putnext(q, mp);
	return (0);
}
static fastcall int
m_r_error(queue_t *q, mblk_t *mp)
{
	/* FIXME: for now */
	freemsg(mp);
	return (0);
}

static fastcall int
m_w_other(queue_t *q, mblk_t *mp)
{
	/* FIXME: for now */
	freemsg(mp);
	return (0);
}
static fastcall int
m_r_other(queue_t *q, mblk_t *mp)
{
	/* FIXME: for now */
	freemsg(mp);
	return (0);
}

/*
 * STREAMS MESSAGE HANDLING
 */

static fastcall int
mx_wsrv_msg(struct mx *mx, queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_DATA)) {
	case M_DATA:
	case M_HPDATA:
		return mx_w_data(mx, q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return mx_w_proto(mx, q, mp);
	case M_CTL:
	case M_PCCTL:
		return mx_w_ctl(mx, q, mp);
	case M_IOCTL:
	case M_IOCDATA:
		return mx_w_ioctl(mx, q, mp);
	case M_FLUSH:
		return m_w_flush(q, mp);
	default:
		return m_w_other(q, mp);
	}
}
static fastcall int
dl_rsrv_msg(struct dl *dl, queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_DATA)) {
	case M_DATA:
	case M_HPDATA:
		return dl_r_data(dl, q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return dl_r_proto(dl, q, mp);
	case M_CTL:
	case M_PCCTL:
		return dl_r_ctl(dl, q, mp);
	case M_IOCACK:
	case M_IOCNAK:
	case M_COPYIN:
	case M_COPYOUT:
		return dl_r_ioctl(dl, q, mp);
	case M_FLUSH:
		return m_r_flush(q, mp);
	case M_ERROR:
	case M_HANGUP:
		return m_r_error(q, mp);
	default:
		return m_r_other(q, mp);
	}
}

static fastcall int
mx_wput_msg(queue_t *q, mblk_t *mp)
{
	unsigned char type;

	switch (__builtin_expect((type = DB_TYPE(mp)), M_DATA)) {
	case M_DATA:
	case M_PROTO:
	case M_CTL:
	case M_IOCTL:
		return (-EAGAIN);	/* for speed */
	case M_HPDATA:
	case M_PCPROTO:
	case M_PCCTL:
	case M_IOCDATA:
	{
		struct priv *priv;
		int err;

		if (likely(!!(priv = (typeof(priv)) mi_trylock(q)))) {
			switch (__builtin_expect(type, M_PCPROTO)) {
			case M_HPDATA:
				err = mx_w_data(&priv->mx, q, mp);
				break;
			case M_PCPROTO:
				err = mx_w_proto(&priv->mx, q, mp);
				break;
			case M_PCCTL:
				err = mx_w_ctl(&priv->mx, q, mp);
				break;
			case M_IOCDATA:
				err = mx_w_ioctl(&priv->mx, q, mp);
				break;
			default:
				err = -EFAULT;
				break;
			}
			mi_unlock((caddr_t) priv);
		} else
			err = -EDEADLK;
		return (err);
	}
	case M_FLUSH:
		return m_w_flush(q, mp);
	default:
		return m_w_other(q, mp);
	}
}
static fastcall int
dl_rput_msg(queue_t *q, mblk_t *mp)
{
	unsigned char type;

	switch (__builtin_expect((type = DB_TYPE(mp)), M_DATA)) {
	case M_DATA:
	case M_PROTO:
	case M_CTL:
		return (-EAGAIN);	/* for speed */
	case M_HPDATA:
	case M_PCPROTO:
	case M_PCCTL:
	case M_IOCACK:
	case M_IOCNAK:
	case M_COPYIN:
	case M_COPYOUT:
	{
		struct priv *priv;
		int err;

		if (likely(!!(priv = (typeof(priv)) mi_trylock(q)))) {
			switch (__builtin_expect(type, M_PCPROTO)) {
			case M_HPDATA:
				err = dl_r_data(&priv->dl, q, mp);
				break;
			case M_PCPROTO:
				err = dl_r_proto(&priv->dl, q, mp);
				break;
			case M_PCCTL:
				err = dl_r_ctl(&priv->dl, q, mp);
				break;
			case M_IOCACK:
			case M_IOCNAK:
			case M_COPYIN:
			case M_COPYOUT:
				err = dl_r_ioctl(&priv->dl, q, mp);
				break;
			default:
				err = -EFAULT;
				break;
			}
			mi_unlock((caddr_t) priv);
		} else
			err = -EDEADLK;
		return (err);
	}
	case M_FLUSH:
		return m_r_flush(q, mp);
	case M_ERROR:
	case M_HANGUP:
		return m_r_error(q, mp);
	default:
		return m_r_other(q, mp);
	}
}
static streamscall int
mx_wput(queue_t *q, mblk_t *mp)
{
	if (unlikely((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY)))
		     || mx_wput_msg(q, mp))) {
		if (unlikely(!putq(q, mp))) {
			mp->b_band = 0;
			putq(q, mp);
		}
	}
	return (0);
}
static streamscall int
mx_wsrv(queue_t *q)
{
	struct priv *priv;
	mblk_t *mp;

	if (likely(!!(priv = (typeof(priv)) mi_trylock(q)))) {
		while (likely(!!(mp = getq(q)))) {
			if (unlikely(mx_wsrv_msg(&priv->mx, q, mp))) {
				if (unlikely(!putbq(q, mp))) {
					mp->b_band = 0;
					putbq(q, mp);
				}
				break;
			}
		}
		mi_unlock((caddr_t) priv);
	}
	return (0);
}
static streamscall int
dl_rsrv(queue_t *q)
{
	struct priv *priv;
	mblk_t *mp;

	if (likely(!!(priv = (typeof(priv)) mi_trylock(q)))) {
		while (likely(!!(mp = getq(q)))) {
			if (unlikely(dl_rsrv_msg(&priv->dl, q, mp))) {
				if (unlikely(!putbq(q, mp))) {
					mp->b_band = 0;
					putbq(q, mp);
				}
				break;
			}
		}
		mi_unlock((caddr_t) priv);
	}
	return (0);
}
static streamscall int
dl_rput(queue_t *q, mblk_t *mp)
{
	if (unlikely((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY)))
		     || dl_rput_msg(q, mp))) {
		if (unlikely(!putq(q, mp))) {
			mp->b_band = 0;
			putq(q, mp);
		}
	}
	return (0);
}

/**
 * a1_qopen: - STREAMS open routine
 * @q: read queue of opened (pushed) queue pair
 * @devp: pointer to device number of driver
 * @oflags: open flags
 * @sflag: STREAMS flag
 * @crp: credentials of opening (pushing) process
 */
static streamscall int
a1_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	dl_info_req_t *r;
	struct priv *priv;
	mblk_t *rp;
	int err;

	if (q->q_ptr)
		return (0);
	if (sflag != MODOPEN)
		return (ENXIO);
	if (unlikely(!!(rp = allocb(sizeof(*r), BPRI_WAITOK))))
		return (ENOSR);
	write_lock(&a1_lock);
	if ((err = mi_open_comm(&a1_opens, sizeof(*priv), q, devp, oflags, sflag, crp))) {
		write_unlock(&a1_lock);
		freeb(rp);
		return (err);
	}
	priv = (struct priv *) (q->q_ptr);
	bzero(priv, sizeof(*priv));
	priv->rq = RD(q);
	priv->wq = WR(q);
	priv->mx.dl = &priv->dl;
	priv->mx.iq = WR(q);
	priv->mx.oq = RD(q);
	priv->mx.state.state = MXS_UNINIT;
	priv->mx.oldstate.state = MXS_UNINIT;
	priv->mx.info.mx_primitive = MX_INFO_ACK;

	priv->dl.mx = &priv->mx;
	priv->dl.iq = RD(q);
	priv->dl.oq = WR(q);
	priv->dl.state.state = DL_UNATTACHED;
	priv->dl.oldstate.state = DL_UNATTACHED;
	priv->dl.info.dl_primitive = DL_INFO_ACK;

	write_unlock(&a1_lock);
	r = (typeof(r)) rp->b_wptr;
	r->dl_primitive = DL_INFO_REQ;
	rp->b_wptr += sizeof(*r);
	qprocson(q);
	putnext(WR(q), rp);
	return (0);
}

static streamscall int
a1_qclose(queue_t *q, int oflags, cred_t *crp)
{
	qprocsoff(q);
	write_lock(&a1_lock);
	mi_close_comm(&a1_opens, q);
	write_unlock(&a1_lock);
	return (0);
}

static modID_t modid = MOD_ID;

#ifdef LINUX
#ifdef module_param
module_param(modid, ushort, 0444);
#else				/* module_param */
MODULE_PARM(modid, "h");
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for AAL1 module. (0 for allocation.)");
#endif				/* LINUX */

static struct module_info a1_minfo = {
	.mi_idnum = MOD_ID,
	.mi_idname = MOD_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat a1_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat a1_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static struct qinit a1_rdinit = {
	.qi_putp = dl_rput,
	.qi_srvp = dl_rsrv,
	.qi_qopen = a1_qopen,
	.qi_qclose = a1_qclose,
	.qi_minfo = &a1_minfo,
	.qi_mstat = &a1_rstat,
};

static struct qinit a1_wrinit = {
	.qi_putp = mx_wput,
	.qi_srvp = mx_wsrv,
	.qi_minfo = &a1_minfo,
	.qi_mstat = &a1_wstat,
};

struct streamtab aal1info = {
	.st_rdinit = &a1_rdinit,
	.st_wrinit = &a1_wrinit,
};

static struct fmodsw a1_fmod = {
	.f_name = MOD_NAME,
	.f_str = &aal1info,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

static int __init
aal1init(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);
	if ((err = register_strmod(&a1_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module %d, err = %d\n", MOD_NAME,
			(int) modid, -err);
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

static int
aal1terminate(void)
{
	int err;

	if ((err = unregister_strmod(&a1_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister module %d, err = %d\n", MOD_NAME,
			(int) modid, -err);
		return (err);
	}
	modid = 0;
	return (0);
}

static void __exit
aal1exit(void)
{
	aal1terminate();
}

module_init(aal1init);
module_exit(aal1exit);
