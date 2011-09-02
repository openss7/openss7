/*****************************************************************************

 @(#) $RCSfile: atm.c,v $ $Name:  $($Revision: 1.1.2.3 $) $Date: 2011-09-02 08:46:32 $

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

 Last Modified $Date: 2011-09-02 08:46:32 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: atm.c,v $
 Revision 1.1.2.3  2011-09-02 08:46:32  brian
 - sync up lots of repo and build changes from git

 Revision 1.1.2.2  2010-11-28 14:21:31  brian
 - remove #ident, protect _XOPEN_SOURCE

 Revision 1.1.2.1  2009-06-21 11:20:44  brian
 - added files to new distro

 *****************************************************************************/

static char const ident[] = "$RCSfile: atm.c,v $ $Name:  $($Revision: 1.1.2.3 $) $Date: 2011-09-02 08:46:32 $";

/*
 * This is an ATM multiplexing driver.  The driver links CDI Streams beneath the multiplexing driver
 * that consists of a steady stream of ATM 53 octet cells (see the PHYS module and the MATRIX
 * driver).  The upper service boundary provides an ATM interface.  Upper streams can be bound to a
 * PPA (ATM interface) and VC (Virtual Channel ATM SAP) within the interface.  This level is the ATM
 * layer and deals solely with multiplexing and demultiplexing ATM VCs.  Cells are passed to and
 * accepted from the upper Stream attached to a VC and multiplexed onto the ATM PHYS.  The upper
 * Streams do no contain any ATM cell headers and only contain the ATM cell payload.
 *
 * For the upper interface boundary, see DLPI.  For the lower interface boundary, see CDI.
 */

#define _SVR4_SOURCE	1
#define _MPS_SOURCE	1

#include <sys/os7/compat.h>
#include <sys/strsun.h>
#include <sys/strconf.h>

#include <sys/cdi.h>
#include <sys/dlpi.h>
#include <sys/atm_dlpi.h>

#define ATM_DESCRIP	"ATM STREAMS MULTIPLEXING DRIVER."
#define ATM_REVISION	"OpenSS7 $RCSfile: atm.c,v $ $Name:  $($Revision: 1.1.2.3 $) $Date: 2011-09-02 08:46:32 $"
#define ATM_COPYRIGHT	"Copyright (c) 2008-2011  Monavacon Limited.  All Rights Reserved."
#define ATM_DEVICE	"Provides OpenSS7 ATM I.432.X Pseudo-Device Driver."
#define ATM_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define ATM_LICENSE	"GPL"
#define ATM_BANNER	ATM_DESCRIP	"\n" \
			ATM_REVISION	"\n" \
			ATM_COPYRIGHT	"\n" \
			ATM_DEVICE	"\n" \
			ATM_CONTACT	"\n"
#define ATM_SPLASH	ATM_DEVICE	" - " \
			ATM_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(ATM_CONTACT);
MODULE_DESCRIPTION(ATM_DESCRIP);
MODULE_SUPPORTED_DEVICE(ATM_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(ATM_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-atm");
#endif				/* MODULE_ALIAS */
#ifdef MODULE_VERSION
MODULE_VERSION(PACKAGE_ENVR);
#endif				/* MODULE_VERSION */
#endif				/* LINUX */

#define ATM_DRV_ID	CONFIG_STREAMS_ATM_MODID
#define ATM_DRV_NAME	CONFIG_STREAMS_ATM_NAME
#define ATM_CMAJOR_0	CONFIG_STREAMS_ATM_MAJOR
#define ATM_CMAJORS	CONFIG_STREAMS_ATM_NMAJORS
#define ATM_UNITS	CONFIG_STREAMS_ATM_NMINORS

#define DRV_ID		ATM_DRV_ID
#define DRV_NAME	ATM_DRV_NAME
#define CMAJOR_0	ATM_CMAJOR_0
#define CMAJORS		ATM_CMAJORS
#define UNITS		ATM_UNITS
#ifdef MODULE
#define DRV_BANNER	ATM_BANNER
#else				/* MODULE */
#define DRV_BANNER	ATM_SPLASH
#endif				/* MODULE */


/*
 * PRIVATE STRUCTURES
 */

struct state {
	uint32_t state;
	uint32_t pending;
};

struct dl;
struct cd;

struct dl {
	struct cd *cd;
	struct dl *next;
	struct dl **prev;
	queue_t *oq;
	queue_t *iq;
	struct state state, oldstate;
	dl_info_ack_t info;
};

rwlock_t atm_lock = RW_LOCK_UNLOCKED;
caddr_t atm_opens;

#define DL_PRIV(q) (struct dl *)(q->q_ptr)

struct cd {
	struct dl *dl;
	queue_t *oq;
	queue_t *iq;
	struct state state, oldstate;
	cd_info_ack_t info;
};

caddr_t atm_links;

#define CD_PRIV(q) (struct cd *)(q->q_ptr)


/*
 * PRINTING THINGS
 */

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
		return ("DL_PRIM_INVAL");
	}
}

static inline fastcall const char *
cd_primname(const cd_ulong prim)
{
	switch (prim) {
	case CD_INFO_REQ:
		return ("CD_INFO_REQ");
	case CD_INFO_ACK:
		return ("CD_INFO_ACK");
	case CD_ATTACH_REQ:
		return ("CD_ATTACH_REQ");
	case CD_DETACH_REQ:
		return ("CD_DETACH_REQ");
	case CD_ENABLE_REQ:
		return ("CD_ENABLE_REQ");
	case CD_DISABLE_REQ:
		return ("CD_DISABLE_REQ");
	case CD_OK_ACK:
		return ("CD_OK_ACK");
	case CD_ERROR_ACK:
		return ("CD_ERROR_ACK");
	case CD_ENABLE_CON:
		return ("CD_ENABLE_CON");
	case CD_DISABLE_CON:
		return ("CD_DISABLE_CON");
	case CD_ERROR_IND:
		return ("CD_ERROR_IND");
	case CD_ALLOW_INPUT_REQ:
		return ("CD_ALLOW_INPUT_REQ");
	case CD_READ_REQ:
		return ("CD_READ_REQ");
	case CD_UNITDATA_REQ:
		return ("CD_UNITDATA_REQ");
	case CD_WRITE_READ_REQ:
		return ("CD_WRITE_READ_REQ");
	case CD_UNITDATA_ACK:
		return ("CD_UNITDATA_ACK");
	case CD_UNITDATA_IND:
		return ("CD_UNITDATA_IND");
	case CD_HALT_INPUT_REQ:
		return ("CD_HALT_INPUT_REQ");
	case CD_ABORT_OUTPUT_REQ:
		return ("CD_ABORT_OUTPUT_REQ");
	case CD_MUX_NAME_REQ:
		return ("CD_MUX_NAME_REQ");
	case CD_BAD_FRAME_IND:
		return ("CD_BAD_FRAME_IND");
	case CD_MODEM_SIG_REQ:
		return ("CD_MODEM_SIG_REQ");
	case CD_MODEM_SIG_IND:
		return ("CD_MODEM_SIG_IND");
	case CD_MODEM_SIG_POLL:
		return ("CD_MODEM_SIG_POLL");
	default:
		return ("(unknown)");
	}
}

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
cd_statename(const cd_ulong state)
{
	switch (state) {
	case CD_UNATTACHED:
		return ("CD_UNATTACHED");
	case CD_UNUSABLE:
		return ("CD_UNUSABLE");
	case CD_DISABLED:
		return ("CD_DISABLED");
	case CD_ENABLE_PENDING:
		return ("CD_ENABLE_PENDING");
	case CD_ENABLED:
		return ("CD_ENABLED");
	case CD_READ_ACTIVE:
		return ("CD_READ_ACTIVE");
	case CD_INPUT_ALLOWED:
		return ("CD_INPUT_ALLOWED");
	case CD_DISABLE_PENDING:
		return ("CD_DISABLE_PENDING");
	case CD_OUTPUT_ACTIVE:
		return ("CD_OUTPUT_ACTIVE");
	case CD_XRAY:
		return ("CD_XRAY");
	case CD_NOT_AUTH:
		return ("CD_NOT_AUTH");
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
cd_errname(const cd_long error)
{
	switch (error) {
	case CD_BADADDRESS:
		return ("CD_BADADDRESS");
	case CD_BADADDRTYPE:
		return ("CD_BADADDRTYPE");
	case CD_BADDIAL:
		return ("CD_BADDIAL");
	case CD_BADDIALTYPE:
		return ("CD_BADDIALTYPE");
	case CD_BADDISPOSAL:
		return ("CD_BADDISPOSAL");
	case CD_BADFRAME:
		return ("CD_BADFRAME");
	case CD_BADPPA:
		return ("CD_BADPPA");
	case CD_BADPRIM:
		return ("CD_BADPRIM");
	case CD_DISC:
		return ("CD_DISC");
	case CD_EVENT:
		return ("CD_EVENT");
	case CD_FATALERR:
		return ("CD_FATALERR");
	case CD_INITFAILED:
		return ("CD_INITFAILED");
	case CD_NOTSUPP:
		return ("CD_NOTSUPP");
	case CD_OUTSTATE:
		return ("CD_OUTSTATE");
	case CD_PROTOSHORT:
		return ("CD_PROTOSHORT");
	case CD_READTIMEOUT:
		return ("CD_READTIMEOUT");
	case CD_SYSERR:
		return ("CD_SYSERR");
	case CD_WRITEFAIL:
		return ("CD_WRITEFAIL");
	default:
		return ("(unknown)");
	}
}

static inline fastcall const char *
dl_strerror(const dl_ulong error)
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
		return ("Automatic handling of XID and TEST response not supported");
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
		return ("unknown error");
	}
}

static inline fastcall const char *
cd_strerror(const cd_ulong error)
{
	switch (error) {
	case CD_BADADDRESS:
		return ("Address was invalid");
	case CD_BADADDRTYPE:
		return ("Invalid address type");
	case CD_BADDIAL:
		return ("Dial information was invalid");
	case CD_BADDIALTYPE:
		return ("Invalid dial information type");
	case CD_BADDISPOSAL:
		return ("Invalid disposal parameter");
	case CD_BADFRAME:
		return ("Defective SDU received");
	case CD_BADPPA:
		return ("Invalid PPA identifier");
	case CD_BADPRIM:
		return ("Unrecognized primitive");
	case CD_DISC:
		return ("Disconnected");
	case CD_EVENT:
		return ("Protocol-specific event occurred");
	case CD_FATALERR:
		return ("Device has become unusable");
	case CD_INITFAILED:
		return ("Line initialization failed");
	case CD_NOTSUPP:
		return ("Primitive not supported by this device");
	case CD_OUTSTATE:
		return ("Primitive was issued from an invalid state");
	case CD_PROTOSHORT:
		return ("M_PROTO block too short");
	case CD_READTIMEOUT:
		return ("Read request timed out before data arrived");
	case CD_SYSERR:
		return ("UNIX system error");
	case CD_WRITEFAIL:
		return ("Unitdata request failed");
	default:
		return ("unknown error");
	}
}

/*
 * STATE TRANSITIONS
 */

static inline fastcall dl_ulong
dl_get_state(struct dl *dl)
{
	return dl->state.state;
}
static inline fastcall dl_ulong
dl_set_state(struct dl *dl, dl_ulong newstate)
{
	dl_ulong oldstate = dl_get_state(dl);

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

static inline fastcall cd_ulong
cd_get_state(struct cd *cd)
{
	return cd->state.state;
}
static inline fastcall cd_ulong
cd_set_state(struct cd *cd, cd_ulong newstate)
{
	cd_ulong oldstate = cd_get_state(cd);

	if (likely(oldstate != newstate)) {
		mi_strlog(cd->iq, STRLOGST, SL_TRACE, "%s <- %s", cd_statename(newstate),
			  cd_statename(oldstate));
		cd->state.state = newstate;
		cd->info.cd_state = newstate;
	}
	return (newstate);
}
static inline fastcall cd_ulong
cd_save_state(struct cd *cd)
{
	cd->oldstate = cd->state;
	return (cd->state.state);
}
static inline fastcall cd_ulong
cd_restore_state(struct cd *cd)
{
	cd->state = cd->oldstate;
	return (cd->info.cd_state = cd->state.state);
}

/*
 * MESSAGE LOGGING
 */

static inline fastcall void
atm_stripmsg(mblk_t *msg, mblk_t *dp)
{
	mblk_t *b, *b_next = msg;

	while ((b = b_next) && b != dp) {
		b_next = b->b_next;
		freeb(b);
	}
	return;
}

noinline fastcall int
dl_txprim(struct dl *dl, queue_t *q, mblk_t *msg, mblk_t *mp, dl_ulong prim)
{
	if (likely(pcmsg(DB_TYPE(mp)) || bcanputnext(dl->oq, mp->b_band))) {
		mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- %s", dl_primname(prim));
		atm_stripmsg(msg, mp->b_cont);
		putnext(dl->oq, mp);
		return (0);
	}
	freeb(mp);
	return (-EBUSY);
}

noinline fastcall int
cd_txprim(struct cd *cd, queue_t *q, mblk_t *msg, mblk_t *mp, cd_ulong prim)
{
	if (likely(pcmsg(DB_TYPE(mp)) || bcanputnext(cd->oq, mp->b_band))) {
		mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "%s ->", cd_primname(prim));
		atm_stripmsg(msg, mp->b_cont);
		putnext(cd->oq, mp);
		return (0);
	}
	freeb(mp);
	return (-EBUSY);
}

noinline fastcall int
dl_rxprim(struct dl *dl, queue_t *q, mblk_t *mp, dl_ulong prim)
{
	return mi_strlog(dl->iq, STRLOGRX, SL_TRACE, "-> %s", dl_primname(prim));
}

noinline fastcall int
cd_rxprim(struct cd *cd, queue_t *q, mblk_t *mp, cd_ulong prim)
{
	return mi_strlog(cd->iq, STRLOGRX, SL_TRACE, "%s <-", cd_primname(prim));
}

/*
 * DL PROVIDER TO DL USER PRIMITIVES
 */

static inline fastcall int
dl_info_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_info_ack_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_INFO_ACK;
		p->dl_max_sdu = 65535;
		p->dl_min_sdu = 1;
		p->dl_addr_length = dl->info.dl_addr_length;
		p->dl_mac_type = DL_ATM;
		p->dl_sap_length = dl->info.dl_sap_length;
		p->dl_service_mode = DL_CODLS;
		p->dl_qos_length = dl->info.dl_qos_length;
		p->dl_qos_range_length = dl->info.dl_qos_range_length;
		p->dl_provider_style = DL_STYLE2;
		p->dl_addr_offset = sizeof(*p);
		p->dl_version = DL_CURRENT_VERSION;
		p->dl_brdcst_addr_length = 0;
		p->dl_brdcst_addr_offset = 0;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_INFO_ACK);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_bind_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_bind_ack_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_BIND_ACK;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_BIND_ACK);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_ok_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_ok_ack_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_OK_ACK;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_OK_ACK);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_error_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_error_ack_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_ERROR_ACK;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_ERROR_ACK);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_subs_bind_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_subs_bind_ack_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_SUBS_BIND_ACK;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_SUBS_BIND_ACK);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_unitdata_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_unitdata_ind_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_UNITDATA_IND;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_UNITDATA_IND);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_uderror_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_uderror_ind_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_UDERROR_IND;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_UDERROR_IND);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_connect_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_connect_ind_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_CONNECT_IND;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_CONNECT_IND);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_connect_con(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_connect_con_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_CONNECT_CON;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_CONNECT_CON);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_token_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_token_ack_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_TOKEN_ACK;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_TOKEN_ACK);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_disconnect_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_disconnect_ind_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_DISCONNECT_IND;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_DISCONNECT_IND);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_reset_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_reset_ind_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_RESET_IND;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_RESET_IND);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_reset_con(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_reset_con_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_RESET_CON;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_RESET_CON);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_data_ack_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_data_ack_ind_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_DATA_ACK_IND;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_DATA_ACK_IND);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_data_ack_status_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_data_ack_status_ind_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_DATA_ACK_STATUS_IND;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_DATA_ACK_STATUS_IND);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_reply_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_reply_ind_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_REPLY_IND;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_REPLY_IND);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_reply_status_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_reply_status_ind_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_REPLY_STATUS_IND;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_REPLY_STATUS_IND);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_reply_update_status_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_reply_update_status_ind_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_REPLY_UPDATE_STATUS_IND;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_REPLY_UPDATE_STATUS_IND);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_xid_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_xid_ind_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_XID_IND;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_XID_IND);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_xid_con(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_xid_con_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_XID_CON;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_XID_CON);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_test_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_test_ind_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_TEST_IND;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_TEST_IND);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_test_con(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_test_con_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_TEST_CON;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_TEST_CON);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_phys_addr_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_phys_addr_ack_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_PHYS_ADDR_ACK;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_PHYS_ADDR_ACK);
	}
	return (-ENOBUFS);
}
static inline fastcall int
dl_get_statistics_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_get_statistics_ack_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_GET_STATISTICS_ACK;
		mp->b_wptr += sizeof(*p);
		return dl_txprim(dl, q, msg, mp, DL_GET_STATISTICS_ACK);
	}
	return (-ENOBUFS);
}

/*
 * CD USER TO CD PROVIDER PRIMITIVES
 */

static inline fastcall int
cd_info_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_info_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_INFO_REQ;
		mp->b_wptr += sizeof(*p);
		return cd_txprim(cd, q, msg, mp, CD_INFO_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
cd_attach_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_attach_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_ATTACH_REQ;
		mp->b_wptr += sizeof(*p);
		return cd_txprim(cd, q, msg, mp, CD_ATTACH_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
cd_detach_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_detach_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_DETACH_REQ;
		mp->b_wptr += sizeof(*p);
		return cd_txprim(cd, q, msg, mp, CD_DETACH_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
cd_enable_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_enable_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_ENABLE_REQ;
		mp->b_wptr += sizeof(*p);
		return cd_txprim(cd, q, msg, mp, CD_ENABLE_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
cd_disable_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_disable_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_DISABLE_REQ;
		mp->b_wptr += sizeof(*p);
		return cd_txprim(cd, q, msg, mp, CD_DISABLE_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
cd_allow_input_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_allow_input_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_ALLOW_INPUT_REQ;
		mp->b_wptr += sizeof(*p);
		return cd_txprim(cd, q, msg, mp, CD_ALLOW_INPUT_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
cd_read_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_read_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_READ_REQ;
		mp->b_wptr += sizeof(*p);
		return cd_txprim(cd, q, msg, mp, CD_READ_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
cd_unitdata_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_unitdata_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_UNITDATA_REQ;
		mp->b_wptr += sizeof(*p);
		return cd_txprim(cd, q, msg, mp, CD_UNITDATA_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
cd_write_read_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_write_read_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_WRITE_READ_REQ;
		mp->b_wptr += sizeof(*p);
		return cd_txprim(cd, q, msg, mp, CD_WRITE_READ_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
cd_halt_input_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_halt_input_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_HALT_INPUT_REQ;
		mp->b_wptr += sizeof(*p);
		return cd_txprim(cd, q, msg, mp, CD_HALT_INPUT_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
cd_abort_output_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_abort_output_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_ABORT_OUTPUT_REQ;
		mp->b_wptr += sizeof(*p);
		return cd_txprim(cd, q, msg, mp, CD_ABORT_OUTPUT_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
cd_mux_name_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_mux_name_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_MUX_NAME_REQ;
		mp->b_wptr += sizeof(*p);
		return cd_txprim(cd, q, msg, mp, CD_MUX_NAME_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
cd_modem_sig_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_modem_sig_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_MODEM_SIG_REQ;
		mp->b_wptr += sizeof(*p);
		return cd_txprim(cd, q, msg, mp, CD_MODEM_SIG_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
cd_modem_sig_poll(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_modem_sig_poll_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_MODEM_SIG_POLL;
		mp->b_wptr += sizeof(*p);
		return cd_txprim(cd, q, msg, mp, CD_MODEM_SIG_POLL);
	}
	return (-ENOBUFS);
}

/*
 * DL USER TO DL PROVIDER PRIMITIVES
 */

noinline fastcall __unlikely int
dl_info_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_info_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto badprim;
      badprim:
	err = DL_BADPRIM;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * dl_attach_req: - process DL_ATTACH_REQ primitive
 * @dl: DL private structure (locked)
 * @q: active queue (upper write queue)
 * @mp: the DL_ATTACH_REQ primitive
 *
 * PPA Access and Control
 *
 * The PPA number that is passed in the DL_ATTACH_REQ primitive should correspond to the ATM port to
 * which the DLS User is requested to be attached.
 *
 * The PPA initialization method is the pre-initialization.  There is no PPA de-initialization.  The
 * pre-initialization and de-initialization are peformed using management tools that are outside the
 * scope of the DLPI Addendum for ATM specification.
 *
 * When PPAs are pre-initialized, DL_CONNECT_REQ can only be successful (SVC DLSAPs) after the ATM
 * network prefix is configured.  This ATM network prefix can be obtained using ILMI (See UNI 3.0/1)
 * or specified as part of the pre-initialization.  At de-initialization time all connections are
 * released (DL_DISCONNECT_IND).  If the ATM network prefix changes, the SVC connections are
 * released (DL_DISCONNECT_IND).
 *
 * If there are several ATM Addresses attached to the port (to the PPA), only one called "primary"
 * ATM Address is returned in the DL_INFO_ACK.  The other addresses are available for use if a peer
 * subsequent bind has been issued (See DLSAP Registration, Multiple ATM Addresses support).
 *
 * If the operating status of the port is OutOfService, all connections are released.  When the
 * operating status of the interface becomes either InService or LoopBack, the configured PVCs are
 * checked against the bound saps and if there is a match DL_CONNECT_IND messages are sent upstream.
 */
noinline fastcall __unlikely int
dl_attach_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_attach_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto badprim;
      badprim:
	err = DL_BADPRIM;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
dl_detach_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_detach_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto badprim;
      badprim:
	err = DL_BADPRIM;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
dl_bind_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_bind_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto badprim;
      badprim:
	err = DL_BADPRIM;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
dl_unbind_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_unbind_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto badprim;
      badprim:
	err = DL_BADPRIM;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
dl_subs_bind_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_subs_bind_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto badprim;
      badprim:
	err = DL_BADPRIM;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
dl_subs_unbind_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_subs_unbind_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto badprim;
      badprim:
	err = DL_BADPRIM;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * dl_connect_req: - process DL_CONNECT_REQ primitive
 * @dl: DL private structure (locked)
 * @q: active queue (upper write queue)
 * @mp: the DL_CONNECT_REQ primitive
 *
 * DLSAP values in DL_CONNECT_REQ
 *
 * The called DLSAP in DL_CONNECT_REQ must follow a number of restrictions.  All the SVEs must be
 * tagged PRESENT or ABSENT.  The ATM_addr and ATM_selector SVEs must be tagged as PRESENT.  The
 * tagging and values of SVEs contained in SAP and subsequent SAP parts of called DLSAP must be
 * consistent with DLSAP registered in DL_BIND_REQ, DL_SUBS_BIND_REQ, that is:
 *
 * - for SVCs: called blli_2, blli_3 and bhli SVE must be equal to those of one of the bound DLSAPs,
 *   and the selector and address must be present,
 *
 * - for PVCs: extended blli_2 must be equal, selector and VPI/VCI must be present.
 *
 * For SVCs, the SETUP PDU will contain IEs indicated by the SVE tagging in called DLSAP.  If
 * several DLSAP were bound to the Stream, several BLLIs are included in the SETUP PDU (See DLSAP
 * Regsitration, Multiple BLLI support).
 *
 * For PVCs, the ATM_addr value will contain the VPI/VCI information enabling Stream attachment to
 * PVC.  The DL_CONNECT_REQ may cause the PVC to be created according to parameters passed in the
 * DL_QoS.  The main operation process is similar to the one described for SVCs.  The PVC does not
 * exist anymore after the disconnection when the DLS User has sent a DL_DISCONNECT_REQ, or after
 * the DLS User has received a DL_DISCONNECT_IND when the port operating status becomes OutOfService
 * (in which case the DLS User has to recreate the PVC, as it would for an SVC).
 *
 * However, as there is no signalling for PVCs, the success of the open of the PVC will depend on
 * existing conditions.
 *
 * - If the PVC does not exist, then it can be created using the information in the DL_QoS, and
 *   attached to the DLS User Stream.
 *
 * - If the PVC exists, it has already been configured by a means outside the scope of DLPPI
 *   Addendum for ATM, then:
 *
 *   - If the DL_QoS is not empty (DL_qos field size is not zero and some parameters tagged as VALID
 *     or DEFAULT), the DL_CONNECT_REQ is refused.
 *
 *   - If the DL_QoS is empyt (DL_qos field size is zero or all parameters tagged as INVALID), the
 *     DL_CONNECT_REQ is processed:
 *
 *     - if the PVC is in use, the DL_CONNECT_REQ is rejected, the PVC cannot be shared by more than
 *       one DLS User Stream.
 *
 *     - if the PVC is not in use (case where the PVC has been pre-configured previously), the PVC
 *       is attached to the DLS User Stream.
 *
 * Information necessary in the DL_QoS for a PVC opened as an SVC:
 *
 *   (v) - used; (x) - not used.
 *
 *   (v) ADL_AAL_Parameters_t AAL_Parameters;
 *   (v) ADL_ATM_Traffic_Descriptor_t ATM_Traffic_Descriptor;
 *   (v) ADL_Broadband_Bearer_Capability_t Broadband_Bearer_Capability;
 *   (x) ADL_BLLI_Parameters_t BLLI_Parameters[3];
 *   (x) ADL_Subaddress_t Called_Party_Subaddrss;
 *   (x) ADL_Calling_Party_Number_t Calling_Party_Number;
 *   (x) ADL_Subaddress_t Calling_Party_Subaddress;
 *   (v) ADL_QOS_t QOS;
 *   (x) ADL_Transit_Network_Selection_t Transit_Network_Selection;
 *
 * For PVCs configured by means beyond the scope of this DLPI Addendum for ATM, the DLS User will
 * receive DL_CONNECT_IND primitives if the port operating status is InService and PVCs are
 * configured or when PVCs are configured and the port operating status becomes InService.  The DLS
 * User will receive a DL_DISCONNECT_IND when the PVC is unconfigured or when the port operating
 * status becomes OutOfService.  This is a convenient way to synchronize the PVC connections with
 * the state of the card and PVC configuration.  For those PVCs, the DL_CONNECT_XYZ primitives must
 * be thought as a way to link an application with the connection rather than exstablishing the
 * connection over the network.
 */
noinline fastcall __unlikely int
dl_connect_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_connect_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto badprim;
      badprim:
	err = DL_BADPRIM;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
dl_connect_res(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_connect_res_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto badprim;
      badprim:
	err = DL_BADPRIM;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
dl_token_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_token_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto badprim;
      badprim:
	err = DL_BADPRIM;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
dl_disconnect_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_disconnect_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto badprim;
      badprim:
	err = DL_BADPRIM;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
dl_phys_addr_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_phys_addr_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto badprim;
      badprim:
	err = DL_BADPRIM;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
dl_set_phys_addr_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_set_phys_addr_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto badprim;
      badprim:
	err = DL_BADPRIM;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
dl_get_statistics_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_get_statistics_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto badprim;
      badprim:
	err = DL_BADPRIM;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/*
 * CD PROVIDER TO CD USER PRIMITIVES
 */

noinline fastcall __unlikely int
cd_info_ack(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_info_ack_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto badprim;
      badprim:
	err = CD_NOTSUPP;
	goto error;
      tooshort:
	err = CD_PROTOSHORT;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
cd_ok_ack(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_ok_ack_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto badprim;
      badprim:
	err = CD_NOTSUPP;
	goto error;
      tooshort:
	err = CD_PROTOSHORT;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
cd_error_ack(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_error_ack_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto badprim;
      badprim:
	err = CD_NOTSUPP;
	goto error;
      tooshort:
	err = CD_PROTOSHORT;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
cd_enable_con(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_enable_con_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto badprim;
      badprim:
	err = CD_NOTSUPP;
	goto error;
      tooshort:
	err = CD_PROTOSHORT;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
cd_disable_con(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_disable_con_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto badprim;
      badprim:
	err = CD_NOTSUPP;
	goto error;
      tooshort:
	err = CD_PROTOSHORT;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
cd_error_ind(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_error_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto badprim;
      badprim:
	err = CD_NOTSUPP;
	goto error;
      tooshort:
	err = CD_PROTOSHORT;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
cd_unitdata_ack(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_unitdata_ack_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto badprim;
      badprim:
	err = CD_NOTSUPP;
	goto error;
      tooshort:
	err = CD_PROTOSHORT;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
cd_unitdata_ind(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_unitdata_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto badprim;
      badprim:
	err = CD_NOTSUPP;
	goto error;
      tooshort:
	err = CD_PROTOSHORT;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
cd_bad_frame_ind(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_bad_frame_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto badprim;
      badprim:
	err = CD_NOTSUPP;
	goto error;
      tooshort:
	err = CD_PROTOSHORT;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
cd_modem_sig_ind(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_modem_sig_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto badprim;
      badprim:
	err = CD_NOTSUPP;
	goto error;
      tooshort:
	err = CD_PROTOSHORT;
	goto error;
      error:
	return (err);
}

/*
 * STREAMS MESSAGE TYPE HANDLING
 */
noinline fastcall __unlikely int
dl_reply_error(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong prim, dl_long error)
{
	/* FIXME */
	freemsg(msg);
	return (0);
}
noinline fastcall __unlikely int
cd_reply_error(struct cd *cd, queue_t *q, mblk_t *msg, cd_ulong prim, cd_long error)
{
	/* FIXME */
	freemsg(msg);
	return (0);
}

noinline fastcall int
dl_w_data(struct dl *dl, queue_t *q, mblk_t *mp)
{
	/* FIXME */
	freemsg(mp);
	return (0);
}
noinline fastcall int
cd_r_data(struct cd *cd, queue_t *q, mblk_t *mp)
{
	/* FIXME */
	freemsg(mp);
	return (0);
}

noinline fastcall int
dl_w_proto(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_ulong prim = -1U;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(prim))))
		goto tooshort;
	prim = *(typeof(prim) *) mp->b_rptr;
	dl_rxprim(dl, q, mp, prim);
	switch (__builtin_expect(prim, DL_UNITDATA_REQ)) {
	case DL_INFO_REQ:
		err = dl_info_req(dl, q, mp);
		break;
	case DL_ATTACH_REQ:
		err = dl_attach_req(dl, q, mp);
		break;
	case DL_DETACH_REQ:
		err = dl_detach_req(dl, q, mp);
		break;
	case DL_BIND_REQ:
		err = dl_bind_req(dl, q, mp);
		break;
	case DL_UNBIND_REQ:
		err = dl_unbind_req(dl, q, mp);
		break;
	case DL_SUBS_BIND_REQ:
		err = dl_subs_bind_req(dl, q, mp);
		break;
	case DL_SUBS_UNBIND_REQ:
		err = dl_subs_unbind_req(dl, q, mp);
		break;
	case DL_ENABMULTI_REQ:
	case DL_DISABMULTI_REQ:
	case DL_PROMISCON_REQ:
	case DL_PROMISCOFF_REQ:
	case DL_UNITDATA_REQ:
	case DL_UDQOS_REQ:
		err = DL_NOTSUPPORTED;
		break;
	case DL_CONNECT_REQ:
		err = dl_connect_req(dl, q, mp);
		break;
	case DL_CONNECT_RES:
		err = dl_connect_res(dl, q, mp);
		break;
	case DL_TOKEN_REQ:
		err = dl_token_req(dl, q, mp);
		break;
	case DL_DISCONNECT_REQ:
		err = dl_disconnect_req(dl, q, mp);
		break;
	case DL_RESET_REQ:
	case DL_RESET_RES:
	case DL_DATA_ACK_REQ:
	case DL_REPLY_REQ:
	case DL_REPLY_UPDATE_REQ:
	case DL_XID_REQ:
	case DL_XID_RES:
	case DL_TEST_REQ:
	case DL_TEST_RES:
		err = DL_NOTSUPPORTED;
		break;
	case DL_PHYS_ADDR_REQ:
		err = dl_phys_addr_req(dl, q, mp);
		break;
	case DL_SET_PHYS_ADDR_REQ:
		err = dl_set_phys_addr_req(dl, q, mp);
		break;
	case DL_GET_STATISTICS_REQ:
		err = dl_get_statistics_req(dl, q, mp);
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
	err = DL_BADPRIM;
	goto out;
}
noinline fastcall int
cd_r_proto(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_ulong prim = -1U;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(prim))))
		goto tooshort;
	prim = *(typeof(prim) *) mp->b_rptr;
	cd_rxprim(cd, q, mp, prim);
	switch (__builtin_expect(prim, CD_UNITDATA_IND)) {
	case CD_INFO_ACK:
		err = cd_info_ack(cd, q, mp);
		break;
	case CD_OK_ACK:
		err = cd_ok_ack(cd, q, mp);
		break;
	case CD_ERROR_ACK:
		err = cd_error_ack(cd, q, mp);
		break;
	case CD_ENABLE_CON:
		err = cd_enable_con(cd, q, mp);
		break;
	case CD_DISABLE_CON:
		err = cd_disable_con(cd, q, mp);
		break;
	case CD_ERROR_IND:
		err = cd_error_ind(cd, q, mp);
		break;
	case CD_UNITDATA_ACK:
		err = cd_unitdata_ack(cd, q, mp);
		break;
	case CD_UNITDATA_IND:
		err = cd_unitdata_ind(cd, q, mp);
		break;
	case CD_BAD_FRAME_IND:
		err = cd_bad_frame_ind(cd, q, mp);
		break;
	case CD_MODEM_SIG_IND:
		err = cd_modem_sig_ind(cd, q, mp);
		break;
	default:
		err = CD_NOTSUPP;
		break;
	}
	if (likely(err == 0))
		return (err);
      out:
	cd_restore_state(cd);
	return cd_reply_error(cd, q, mp, prim, err);
      tooshort:
	err = CD_PROTOSHORT;
	goto out;
}

noinline fastcall int
dl_w_ctl(struct dl *dl, queue_t *q, mblk_t *mp)
{
	/* nothing for now */
	freemsg(mp);
	return (0);
}
noinline fastcall int
cd_r_ctl(struct cd *cd, queue_t *q, mblk_t *mp)
{
	/* nothing for now */
	freemsg(mp);
	return (0);
}

noinline fastcall int
dl_w_ioctl(struct dl *dl, queue_t *q, mblk_t *mp)
{
	/* nothing for now */
	freemsg(mp);
	return (0);
}
noinline fastcall int
cd_r_ioctl(struct cd *cd, queue_t *q, mblk_t *mp)
{
	/* nothing for now */
	freemsg(mp);
	return (0);
}

noinline fastcall int
m_w_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		mp->b_rptr[0] &= ~FLUSHW;
	}
	if (mp->b_rptr[0] & FLUSHR) {
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
		if (!(mp->b_flag & MSGNOLOOP)) {
			mp->b_flag |= MSGNOLOOP;
			qreply(q, mp);
			return (0);
		}
	}
	freemsg(mp);
	return (0);
}

noinline fastcall int
m_r_error(queue_t *q, mblk_t *mp)
{
	/* nothing for now */
	freemsg(mp);
	return (0);
}

noinline fastcall int
m_w_other(queue_t *q, mblk_t *mp)
{
	mi_strlog(q, STRLOGERR, SL_TRACE | SL_ERROR, "invalid message type %x on upper write queue",
		  (int) DB_TYPE(mp));
	freemsg(mp);
	return (0);
}
noinline fastcall int
m_r_other(queue_t *q, mblk_t *mp)
{
	mi_strlog(q, STRLOGERR, SL_TRACE | SL_ERROR, "invalid message type %x on lower read queue",
		  (int) DB_TYPE(mp));
	freemsg(mp);
	return (0);
}

/*
 * STREAMS MESSAGE HANDLING
 */

static fastcall int
dl_wsrv_msg(struct dl *dl, queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_DATA)) {
	case M_DATA:
	case M_HPDATA:
		return dl_w_data(dl, q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return dl_w_proto(dl, q, mp);
	case M_CTL:
	case M_PCCTL:
		return dl_w_ctl(dl, q, mp);
	case M_IOCTL:
	case M_IOCDATA:
		return dl_w_ioctl(dl, q, mp);
	case M_FLUSH:
		return m_w_flush(q, mp);
	default:
		return m_w_other(q, mp);
	}
}

static fastcall int
cd_rsrv_msg(struct cd *cd, queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_DATA)) {
	case M_DATA:
	case M_HPDATA:
		return cd_r_data(cd, q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return cd_r_proto(cd, q, mp);
	case M_CTL:
	case M_PCCTL:
		return cd_r_ctl(cd, q, mp);
	case M_IOCACK:
	case M_IOCNAK:
	case M_COPYIN:
	case M_COPYOUT:
		return cd_r_ioctl(cd, q, mp);
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
dl_wput_msg(queue_t *q, mblk_t *mp)
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
		struct dl *dl;
		int err;

		if (likely(!!(dl = (typeof(dl)) mi_trylock(q)))) {
			switch (__builtin_expect(type, M_PCPROTO)) {
			case M_HPDATA:
				err = dl_w_data(dl, q, mp);
				break;
			case M_PCPROTO:
				err = dl_w_proto(dl, q, mp);
				break;
			case M_PCCTL:
				err = dl_w_ctl(dl, q, mp);
				break;
			case M_IOCDATA:
				err = dl_w_ioctl(dl, q, mp);
				break;
			default:
				err = -EFAULT;
				break;
			}
			mi_unlock((caddr_t) dl);
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
cd_rput_msg(queue_t *q, mblk_t *mp)
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
		struct cd *cd;
		int err;

		if (likely(!!(cd = (typeof(cd)) mi_trylock(q)))) {
			switch (__builtin_expect(type, M_PCPROTO)) {
			case M_HPDATA:
				err = cd_r_data(cd, q, mp);
				break;
			case M_PCPROTO:
				err = cd_r_proto(cd, q, mp);
				break;
			case M_PCCTL:
				err = cd_r_ctl(cd, q, mp);
				break;
			case M_IOCACK:
			case M_IOCNAK:
			case M_COPYIN:
			case M_COPYOUT:
				err = cd_r_ioctl(cd, q, mp);
				break;
			default:
				err = -EFAULT;
				break;
			}
			mi_unlock((caddr_t) cd);
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

/*
 * QUEUE PUT AND SERVICE PROCEDURES
 */

/**
 * dl_wput: - upper write put procedure
 * @q: upper write queue
 * @mp: message to put
 *
 * The upper write put procedure is for DL-Primitives passed from the DL user above to the DL
 * provider.
 */
static streamscall int
dl_wput(queue_t *q, mblk_t *mp)
{
	if (unlikely((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY)))
		     || dl_wput_msg(q, mp))) {
		if (unlikely(!putq(q, mp))) {
			mp->b_band = 0;
			putq(q, mp);
		}
	}
	return (0);
}

/**
 * dl_wsrv: - upper write service procedure
 * @q: upper write queue
 *
 * The upper write queue service procedure is for DL-Primitive queued from the DL user above to the
 * DL provider.
 */
static streamscall int
dl_wsrv(queue_t *q)
{
	struct dl *dl;
	mblk_t *mp;

	if (likely(!!(dl = (typeof(dl)) mi_trylock(q)))) {
		while (likely(!!(mp = getq(q)))) {
			if (unlikely(dl_wsrv_msg(dl, q, mp))) {
				if (unlikely(!putbq(q, mp))) {
					mp->b_band = 0;
					putbq(q, mp);
				}
				break;
			}
		}
		mi_unlock((caddr_t) dl);
	}
	return (0);
}

/**
 * atm_muxrsrv: - upper read service procedure
 * @q: upper read queue
 *
 * This procedure is only invoked when the upper read queue is back-enabled by flow control
 * subsiding on the read side of an upper multiplex stream.  Simply enable across the multiplexer
 * all lower CD Streams feeding this upper DL Stream.
 */
static streamscall int
atm_muxrsrv(queue_t *q)
{
	/* FIXME */
	return (0);
}

/**
 * atm_muxwsrv: - lower write service procedure
 * @q: lower write queue
 *
 * This procedure is only invoked when the lower write queue is back-enabled by flow control
 * subsiding on the write side of a lower multiplex Stream.  Simply enable across the multiplexer
 * all upper DL Streams feeding this lower CD Stream.
 */
static streamscall int
atm_muxwsrv(queue_t *q)
{
	/* FIXME */
	return (0);
}

/**
 * cd_rsrv: - lower read service procedure
 * @q: lower read queue
 */
static streamscall int
cd_rsrv(queue_t *q)
{
	struct cd *cd;
	mblk_t *mp;

	if (likely(!!(cd = (typeof(cd)) mi_trylock(q)))) {
		while (likely(!!(mp = getq(q)))) {
			if (unlikely(cd_rsrv_msg(cd, q, mp))) {
				if (unlikely(!putbq(q, mp))) {
					mp->b_band = 0;
					putbq(q, mp);
				}
				break;
			}
		}
		mi_unlock((caddr_t) cd);
	}
	return (0);
}

/**
 * cd_rput: - lower read put procedure
 * @q: lower read queue
 * @mp: message to put
 */
static streamscall int
cd_rput(queue_t *q, mblk_t *mp)
{
	if (unlikely((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY)))
		     || cd_rput_msg(q, mp))) {
		if (unlikely(!putq(q, mp))) {
			mp->b_band = 0;
			putq(q, mp);
		}
	}
	return (0);
}

/*
 * OPEN AND CLOSE ROUTINES
 */

static streamscall int
atm_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct dl *dl;
	int err;

	if (q->q_ptr)
		return (0);
	if (sflag != CLONEOPEN)
		return (ENXIO);
	write_lock(&atm_lock);
	if ((err = mi_open_comm(&atm_opens, sizeof(*dl), q, devp, oflags, sflag, crp))) {
		write_unlock(&atm_lock);
		return (err);
	}
	dl = (typeof(dl)) q->q_ptr;
	bzero(dl, sizeof(*dl));
	dl->cd = NULL;
	dl->next = NULL;
	dl->prev = &dl->next;
	dl->oq = RD(q);
	dl->iq = WR(q);
	dl->state.state = DL_UNATTACHED;
	dl->oldstate.state = DL_UNATTACHED;
	dl->info.dl_primitive = DL_INFO_ACK;
	dl->info.dl_current_state = DL_UNATTACHED;
	/* FIXME: more */
	write_unlock(&atm_lock);
	return (0);
}

static streamscall int
atm_qclose(queue_t *q, int oflags, cred_t *crp)
{
	qprocsoff(q);
	write_lock(&atm_lock);
	mi_close_comm(&atm_opens, q);
	write_unlock(&atm_lock);
	return (0);
}

static modID_t modid = DRV_ID;
static major_t major = CMAJOR_0;

#ifdef LINUX
#ifdef module_param
module_param(modid, ushort, 0444);
module_param(major, uint, 0444);
#else				/* module_param */
MODULE_PARM(modid, "h");
MODULE_PARM(major, "d");
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for ATM driver.  (0 for allocation.)");
MODULE_PARM_DESC(major, "Major Device Number for ATM driver. (0 for allocation.)");
#endif				/* LINUX */

static struct module_info atm_minfo = {
	.mi_idnum = DRV_ID,
	.mi_idname = DRV_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat atm_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat atm_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static struct qinit atm_rdinit = {
	.qi_putp = NULL,
	.qi_srvp = atm_muxrsrv,
	.qi_qopen = atm_qopen,
	.qi_qclose = atm_qclose,
	.qi_minfo = &atm_minfo,
	.qi_mstat = &atm_rstat,
};

static struct qinit atm_wrinit = {
	.qi_putp = dl_wput,
	.qi_srvp = dl_wsrv,
	.qi_minfo = &atm_minfo,
	.qi_mstat = &atm_wstat,
};

static struct qinit atm_muxrinit = {
	.qi_putp = cd_rput,
	.qi_srvp = cd_rsrv,
	.qi_minfo = &atm_minfo,
	.qi_mstat = &atm_rstat,
};

static struct qinit atm_muxwinit = {
	.qi_putp = NULL,
	.qi_srvp = atm_muxwsrv,
	.qi_minfo = &atm_minfo,
	.qi_mstat = &atm_wstat,
};

struct streamtab atminfo = {
	.st_rdinit = &atm_rdinit,
	.st_wrinit = &atm_wrinit,
	.st_muxrinit = &atm_muxrinit,
	.st_muxwinit = &atm_muxwinit,
};

static struct cdevsw atm_cdev = {
	.d_name = DRV_NAME,
	.d_str = &atminfo,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR,		/* FIXME */
	.d_kmod = THIS_MODULE,
};

static int __init
atminit(void)
{
	int err;

	(void) modid;
	cmn_err(CE_NOTE, DRV_BANNER);
	if ((err = register_strdev(&atm_cdev, major)) < 0) {
		cmn_err(CE_WARN, "%s: could not register driver %d, err = %d\n", DRV_NAME,
			(int) major, -err);
		return (err);
	}
	if (major == 0)
		major = err;
	return (0);
}

static int
atmterminate(void)
{
	int err;

	if ((err = unregister_strdev(&atm_cdev, major)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister driver %d, err = %d\n", DRV_NAME,
			(int) major, -err);
		return (err);
	}
	major = 0;
	return (0);
}

static void __exit
atmexit(void)
{
	atmterminate();
}

#ifdef LINUX
module_init(atminit);
module_exit(atmexit);
#endif				/* LINUX */
