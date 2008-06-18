/*****************************************************************************

 @(#) $RCSfile: dlaix.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-06-13 06:43:57 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2008-06-13 06:43:57 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: dlaix.c,v $
 Revision 0.9.2.1  2008-06-13 06:43:57  brian
 - added files

 *****************************************************************************/

#ident "@(#) $RCSfile: dlaix.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-06-13 06:43:57 $"

static char const ident[] = "$RCSfile: dlaix.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-06-13 06:43:57 $";

/*
 * This is a DLPI module that is pushed over a CD-HDLC stream to form
 * AIXlink/X.25 style DLPI stream per AIXLink/X.25 documentation.
 */

#define _MPS_SOURCE
#include <sys/os7/compat.h>
#include <sys/strsun.h>
#include <sys/cdi.h>
#include <sys/dlpi.h>

#ifndef CD_ATTACH_PENDING
#define CD_ATTACH_PENDING (CD_XRAY+1)
#endif
#ifndef CD_DETACH_PENDING
#define CD_DETACH_PENDING (CD_XRAY+2)
#endif

#define DLAIX_DESCRIP	"LAPB OVER HDLC MODULE FOR LINUX FAST-STREAMS"
#define DLAIX_EXTRA	"Part of the OpenSS7 X.25 Stack for Linux Fast-STREAMS"
#define DLAIX_COPYRIGHT	"Copyright (c) 1997-2008  OpenSS7 Corporation.  All Rights Reserved."
#define DLAIX_REVISION	"OpenSS7 $RCSfile: dlaix.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-06-13 06:43:57 $"
#define DLAIX_DEVICE	"SVR 4.2MP LAPB over HDLC Module (LAPB) for X.25"
#define DLAIX_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define DLAIX_LICENSE	"GPL"
#define DLAIX_BANNER	DLAIX_DESCRIP	"\n" \
			DLAIX_EXTRA	"\n" \
			DLAIX_COPYRIGHT	"\n" \
			DLAIX_REVISION	"\n" \
			DLAIX_DEVICE	"\n" \
			DLAIX_CONTACT	"\n"
#define DLAIX_SPLASH	DLAIX_DESCRIP	" - " \
			DLAIX_REVISION

#ifndef CONFIG_STREAMS_DLAIX_NAME
#error "CONFIG_STREAMS_DLAIX_NAME must be defined."
#endif				/* CONFIG_STREAMS_DLAIX_NAME */
#ifndef CONFIG_STREAMS_DLAIX_MODID
#error "CONFIG_STREAMS_DLAIX_MODID must be defined."
#endif				/* CONFIG_STREAMS_DLAIX_MODID */

#ifdef LINUX
#ifdef MODULE
MODULE_AUTHOR(DLAIX_CONTACT);
MODULE_DESCRIPTION(DLAIX_DESCRIP);
MODULE_SUPPORTED_DEVICE(DLAIX_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(DLAIX_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-dlaix");
MODULE_ALIAS("streams-module-dlaix");
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_DLAIX_MODID));
#endif				/* MODULE_ALIAS */
#endif				/* MODULE */
#endif				/* LINUX */

#ifndef DLAIX_MOD_NAME
#define DLAIX_MOD_NAME		CONFIG_STREAMS_DLAIX_NAME
#endif				/* DLAIX_MOD_NAME */
#ifndef DLAIX_MOD_ID
#define DLAIX_MOD_ID		CONFIG_STREAMS_DLAIX_MODID
#endif				/* DLAIX_MOD_ID */

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS DEFINITIONS
 *
 * --------------------------------------------------------------------------
 */

#define MOD_ID		DLAIX_MOD_ID
#define MOD_NAME	DLAIX_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	DLAIX_BANNER
#else				/* MODULE */
#define MOD_BANNER	DLAIX_SPLASH
#endif				/* MODULE */

static struct module_info dl_minfo = {
	.mi_idnum = MOD_ID,
	.mi_idname = MOD_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat dl_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat dl_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

/*
 * --------------------------------------------------------------------------
 *
 * PRIVATE STRUCTURE
 *
 * --------------------------------------------------------------------------
 */

struct priv *priv;
struct dl *dl;
struct cd *cd;

/* Upper interface structure. */
struct dl {
	struct priv *priv;
	struct cd *cd;
	queue_t *oq;
	dl_ulong state;
	dl_ulong oldstate;
	dl_ulong sap;
	struct {
		dl_info_ack_t info;
	} proto;
};

/* Lower interface structure. */
struct cd {
	struct priv *priv;
	struct dl *dl;
	queue_t *oq;
	cd_ulong state;
	cd_ulong oldstate;
	struct {
		cd_info_ack_t info;
	} proto;
};

struct priv {
	struct dl dlpi;
	struct cd cdi;
};

#define PRIV(q) ((struct priv *)q->q_ptr)
#define DL_PRIV(q) (&PRIV(q)->dlpi)
#define CD_PRIV(q) (&PRIV(q)->cdi)

#define STRLOGIO    0		/* input-output controls */
#define STRLOGST    1		/* state transitions */
#define STRLOGTO    2		/* timeouts */
#define STRLOGRX    3		/* primitives received */
#define STRLOGTX    4		/* primitives issued */
#define STRLOGTE    5		/* timer events */
#define STRLOGDA    6		/* data */

static inline const char *
dl_iocname(int cmd)
{
	switch (cmd) {
	default:
		return ("(unknown)");
	}
}
static inline const char *
dl_primname(dl_ulong prim)
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
static inline const char *
dl_statename(dl_ulong state)
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
static inline const char *
dl_errname(dl_long error)
{
	if (error < 0)
		return ("DL_SYSERR");
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
static inline const char *
dl_strerror(dl_long error)
{
	if (error < 0)
		return ("UNIX system error occurred.");
	switch (error) {
	case DL_ACCESS:
		return ("Improper permissions for request.");
	case DL_BADADDR:
		return ("DLSAP addr in improper format or invalid.");
	case DL_BADCORR:
		return ("Seq number not from outstand DL_CONN_IND.");
	case DL_BADDATA:
		return ("User data exceeded provider limit.");
	case DL_BADPPA:
		return ("Specified PPA was invalid.");
	case DL_BADPRIM:
		return ("Primitive received not known by provider.");
	case DL_BADQOSPARAM:
		return ("QOS parameters contained invalid values.");
	case DL_BADQOSTYPE:
		return ("QOS structure type is unknown/unsupported.");
	case DL_BADSAP:
		return ("Bad LSAP selector.");
	case DL_BADTOKEN:
		return ("Token used not an active stream.");
	case DL_BOUND:
		return ("Attempted second bind with dl_max_conind.");
	case DL_INITFAILED:
		return ("Physical Link initialization failed.");
	case DL_NOADDR:
		return ("Provider couldn't allocate alt. address.");
	case DL_NOTINIT:
		return ("Physical Link not initialized.");
	case DL_OUTSTATE:
		return ("Primitive issued in improper state.");
	case DL_SYSERR:
		return ("UNIX system error occurred.");
	case DL_UNSUPPORTED:
		return ("Requested serv. not supplied by provider.");
	case DL_UNDELIVERABLE:
		return ("Previous data unit could not be delivered.");
	case DL_NOTSUPPORTED:
		return ("Primitive is known but not supported.");
	case DL_TOOMANY:
		return ("Limit exceeded.");
	case DL_NOTENAB:
		return ("Promiscuous mode not enabled.");
	case DL_BUSY:
		return ("Other streams for PPA in post-attached state.");
	case DL_NOAUTO:
		return ("Automatic handling of XID and TEST response not supported. .");
	case DL_NOXIDAUTO:
		return ("Automatic handling of XID not supported.");
	case DL_NOTESTAUTO:
		return ("Automatic handling of TEST not supported.");
	case DL_XIDAUTO:
		return ("Automatic handling of XID response.");
	case DL_TESTAUTO:
		return ("Automatic handling of TEST response.");
	case DL_PENDING:
		return ("pending outstanding connect indications.");
	default:
		return ("(unknown)");
	}
}

static inline const char *
cd_iocname(int cmd)
{
	switch (cmd) {
	default:
		return ("(unknown)");
	}
}
static inline const char *
cd_primname(cd_ulong prim)
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
static inline const char *
cd_statename(cd_ulong state)
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
static inline const char *
cd_errname(cd_long error)
{
	if (error < 0)
		return ("CD_SYSERR");
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
static inline const char *
cd_strerror(cd_long error)
{
	if (error < 0)
		return ("UNIX system error.");
	switch (error) {
	case CD_BADADDRESS:
		return ("Address was invalid.");
	case CD_BADADDRTYPE:
		return ("Invalid address type.");
	case CD_BADDIAL:
		return ("Dial information was invalid.");
	case CD_BADDIALTYPE:
		return ("Invalid dial information type.");
	case CD_BADDISPOSAL:
		return ("Invalid disposal parameter.");
	case CD_BADFRAME:
		return ("Defective SDU received.");
	case CD_BADPPA:
		return ("Invalid PPA identifier.");
	case CD_BADPRIM:
		return ("Unrecognized primitive.");
	case CD_DISC:
		return ("Disconnected.");
	case CD_EVENT:
		return ("Protocol-specific event occurred.");
	case CD_FATALERR:
		return ("Device has become unusable.");
	case CD_INITFAILED:
		return ("Line initialization failed.");
	case CD_NOTSUPP:
		return ("Primitive not supported by this device.");
	case CD_OUTSTATE:
		return ("Primitive was issued from an invalid state.");
	case CD_PROTOSHORT:
		return ("M_PROTO block too short.");
	case CD_READTIMEOUT:
		return ("Read request timed out before data arrived.");
	case CD_SYSERR:
		return ("UNIX system error.");
	case CD_WRITEFAIL:
		return ("Unitdata request failed.");
	default:
		return ("(unknown)");
	}
}

static dl_ulong
dl_get_state(struct dl *dl)
{
	return (dl->state);
}

static dl_ulong
dl_set_state(struct dl *dl, dl_ulong newstate)
{
	dl_ulong oldstate = dl->state;

	if (newstate != oldstate) {
		dl->state = newstate;
		dl->proto.info.dl_current_state = newstate;
		mi_strlog(dl->oq, STRLOGST, SL_TRACE, "%s <- %s", dl_statename(newstate),
			  dl_statename(oldstate));
	}
	return (newstate);
}

static dl_ulong
dl_save_state(struct dl *dl)
{
	return ((dl->oldstate = dl_get_state(dl)));
}

static dl_ulong
dl_restore_state(struct dl *dl)
{
	return (dl_set_state(dl, dl->oldstate));
}

static cd_ulong
cd_get_state(struct cd *cd)
{
	return (cd->state);
}

static cd_ulong
cd_set_state(struct cd *cd, cd_ulong newstate)
{
	cd_ulong oldstate = cd->state;

	if (newstate != oldstate) {
		cd->state = newstate;
		cd->proto.info.cd_state = newstate;
		mi_strlog(cd->oq, STRLOGST, SL_TRACE, "%s <- %s", cd_statename(newstate),
			  cd_statename(oldstate));
	}
	return (newstate);
}

static cd_ulong
cd_save_state(struct cd *cd)
{
	return ((cd->oldstate = cd_get_state(cd)));
}

static cd_ulong
cd_restore_state(struct cd *cd)
{
	return (cd_set_state(cd, cd->oldstate));
}

/*
 * --------------------------------------------------------------------------
 *
 * DLPI PROVIDER TO DLPI USER ISSUED PRIMITIVES
 *
 * --------------------------------------------------------------------------
 */

static fastcall noinline __unlikely int
m_error(struct dl *dl, queue_t *q, mblk_t *msg, int rerr, int werr)
{
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, 2, BPRI_MED)))) {
		DB_TYPE(mp) = M_ERROR;
		*mp->b_wptr++ = rerr;
		*mp->b_wptr++ = werr;
		freemsg(msg);
		mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- M_ERROR");
		putnext(dl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

static fastcall noinline __unlikely int
m_error_reply(struct dl *dl, queue_t *q, mblk_t *msg, int error)
{
	switch (error) {
	case 0:
	case -ENOBUFS:
	case -EAGAIN:
	case -EDEADLK:
	case -EBUSY:
	case -ENOMEM:
		return (error);
	}
	if (error > 0)
		return m_error(dl, q, msg, EPROTO, EPROTO);
	return m_error(dl, q, msg, -error, -error);
}

static fastcall inline __hot_read int
dl_data_ind(struct dl *dl, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	if (pcmsg(DB_TYPE(dp)) || bcanputnext(dl->oq, dp->b_band)) {
		mi_strlog(dl->oq, STRLOGDA, SL_TRACE, "<- M_DATA");
		if (msg && msg->b_cont == dp)
			msg->b_cont = NULL;
		freemsg(msg);
		putnext(dl->oq, dp);
		return (0);
	}
	return (-EBUSY);
}
static fastcall noinline __unlikely int
dl_info_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_info_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_INFO_ACK;
		p->dl_max_sdu = dl->proto.info.dl_max_sdu;
		p->dl_addr_length = dl->proto.info.dl_addr_length;
		p->dl_mac_type = dl->proto.info.dl_mac_type;
		p->dl_reserved = dl->proto.info.dl_reserved;
		p->dl_current_state = dl->proto.info.dl_current_state;
		p->dl_sap_length = dl->proto.info.dl_sap_length;
		p->dl_service_mode = dl->proto.info.dl_service_mode;
		p->dl_qos_length = dl->proto.info.dl_qos_length;
		p->dl_qos_offset = dl->proto.info.dl_qos_offset;
		p->dl_qos_range_length = dl->proto.info.dl_qos_range_length;
		p->dl_qos_range_offset = dl->proto.info.dl_qos_range_offset;
		p->dl_provider_style = dl->proto.info.dl_provider_style;
		p->dl_addr_offset = dl->proto.info.dl_addr_offset;
		p->dl_version = dl->proto.info.dl_version;
		p->dl_brdcst_addr_length = dl->proto.info.dl_brdcst_addr_length;
		p->dl_brdcst_addr_offset = dl->proto.info.dl_brdcst_addr_offset;
		p->dl_growth = dl->proto.info.dl_growth;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_INFO_ACK");
		putnext(dl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
dl_bind_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_bind_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_BIND_ACK;
		p->dl_sap = dl->sap;
		p->dl_addr_length = 0;
		p->dl_addr_offset = sizeof(*p);
		p->dl_max_conind = 0;
		p->dl_xidtest_flg = 0;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		dl_set_state(dl, DL_IDLE);
		mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_BIND_ACK");
		putnext(dl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
dl_ok_ack(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong prim)
{
	dl_ok_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_OK_ACK;
		p->dl_correct_primitive = prim;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		switch (dl_get_state(dl)) {
		case DL_UNBIND_PENDING:
			dl_set_state(dl, DL_UNBOUND);
			break;
		case DL_RESET_RES_PENDING:
			dl_set_state(dl, DL_DATAXFER);
			break;
		}
		mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_OK_ACK");
		putnext(dl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
dl_error_ack(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong prim, dl_long error)
{
	dl_error_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_ERROR_ACK;
		p->dl_error_primitive = prim;
		p->dl_errno = error < 0 ? DL_SYSERR : 0;
		p->dl_unix_errno = error < 0 ? -error : 0;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		switch (dl_get_state(dl)) {
		case DL_BIND_PENDING:
			dl_set_state(dl, DL_UNBOUND);
			break;
		case DL_UNBIND_PENDING:
			dl_set_state(dl, DL_IDLE);
			break;
		case DL_OUTCON_PENDING:
			dl_set_state(dl, DL_IDLE);
			break;
		case DL_DISCON8_PENDING:
			dl_set_state(dl, DL_OUTCON_PENDING);
			break;
		case DL_DISCON9_PENDING:
			dl_set_state(dl, DL_INCON_PENDING);
			break;
		case DL_DISCON11_PENDING:
			dl_set_state(dl, DL_DATAXFER);
			break;
		case DL_DISCON12_PENDING:
			dl_set_state(dl, DL_USER_RESET_PENDING);
			break;
		case DL_DISCON13_PENDING:
			dl_set_state(dl, DL_PROV_RESET_PENDING);
			break;
		case DL_RESET_RES_PENDING:
			dl_set_state(dl, DL_PROV_RESET_PENDING);
			break;
		case DL_USER_RESET_PENDING:
			dl_set_state(dl, DL_DATAXFER);
			break;
		}
		mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_ERROR_ACK [%s,%s] %s",
			  dl_primname(prim), dl_errname(error), dl_strerror(error));
		putnext(dl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
dl_error_reply(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong prim, dl_long error)
{
	switch (error) {
	case 0:
	case -ENOBUFS:
	case -EBUSY:
	case -EDEADLK:
	case -ENOMEM:
	case -EAGAIN:
	case -ENOSR:
		return (error);
	}
	return dl_error_ack(dl, q, msg, prim, error);
}
static fastcall noinline __unlikely int
dl_connect_con(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_connect_con_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_CONNECT_CON;
			p->dl_resp_addr_length = 0;
			p->dl_resp_addr_offset = sizeof(*p);
			p->dl_qos_length = 0;
			p->dl_qos_offset = sizeof(*p);
			p->dl_growth = 0;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			dl_set_state(dl, DL_DATAXFER);
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_CONNECT_CON");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-ENOBUFS);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
dl_token_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_token_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_TOKEN_ACK;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			dl_set_state(dl, dl_get_state(dl));
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_TOKEN_ACK");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-ENOBUFS);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
dl_disconnect_ind(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong orig, dl_ulong reason)
{
	dl_disconnect_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_DISCONNECT_IND;
			p->dl_originator = orig;
			p->dl_reason = reason;
			p->dl_correlation = 0;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			dl_set_state(dl, DL_IDLE);
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_DISCONNECT_IND");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-ENOBUFS);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
dl_reset_ind(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong orig, dl_ulong reason)
{
	dl_reset_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_RESET_IND;
			p->dl_originator = orig;
			p->dl_reason = reason;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			dl_set_state(dl, DL_PROV_RESET_PENDING);
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_RESET_IND");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-ENOBUFS);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
dl_reset_con(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_reset_con_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_RESET_CON;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			dl_set_state(dl, DL_DATAXFER);
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_RESET_CON");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-ENOBUFS);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
dl_get_statistics_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_get_statistics_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_GET_STATISTICS_ACK;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			dl_set_state(dl, dl_get_state(dl));
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_GET_STATISTICS_ACK");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-ENOBUFS);
	}
	return (-ENOBUFS);
}

/*
 * --------------------------------------------------------------------------
 *
 * CDI USER TO CDI PROVIDER ISSUED PRIMITIVES
 *
 * --------------------------------------------------------------------------
 */

static fastcall noinline __unlikely int
cd_attach_req(struct cd *cd, queue_t *q, mblk_t *msg, cd_ulong ppa)
{
	cd_attach_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_ATTACH_REQ;
			p->cd_ppa = ppa;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			cd_set_state(cd, CD_ATTACH_PENDING);
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "CD_ATTACH_REQ ->");
			putnext(cd->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-ENOBUFS);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_detach_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_detach_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_DETACH_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		cd_set_state(cd, CD_DETACH_PENDING);
		mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "CD_DETACH_REQ ->");
		putnext(cd->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_enable_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_enable_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_ENABLE_REQ;
			p->cd_dial_type = CD_NODIAL;
			p->cd_dial_length = 0;
			p->cd_dial_offset = sizeof(*p);
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			cd_set_state(cd, CD_ENABLE_PENDING);
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "CD_ENABLE_REQ ->");
			putnext(cd->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-ENOBUFS);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_disable_req(struct cd *cd, queue_t *q, mblk_t *msg, dl_ulong disposal)
{
	cd_disable_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_DISABLE_REQ;
			p->cd_disposal = disposal;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			cd_set_state(cd, CD_DISABLE_PENDING);
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "CD_DISABLE_REQ ->");
			putnext(cd->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-ENOBUFS);
	}
	return (-ENOBUFS);
}
static fastcall inline __hot_out int
cd_unitdata_req(struct cd *cd, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	cd_unitdata_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(cd->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_UNITDATA_REQ;
			p->cd_addr_type = CD_IMPLICIT;
			p->cd_priority = 0;
			p->cd_dest_addr_length = 0;
			p->cd_dest_addr_offset = sizeof(*p);
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(cd->oq, STRLOGDA, SL_TRACE, "CD_UNITDATA_REQ ->");
			putnext(cd->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-ENOBUFS);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
cd_abort_output_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_abort_output_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_ABORT_OUTPUT_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "CD_ABORT_OUTPUT_REQ ->");
		putnext(cd->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 * --------------------------------------------------------------------------
 *
 * DLPI USER TO DLPI PROVIDER RECEIVED PRIMITIVES
 *
 * --------------------------------------------------------------------------
 */

static fastcall inline __hot_write int
dl_data_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return cd_unitdata_req(dl->cd, q, NULL, mp);
}
static fastcall noinline __unlikely int
dl_info_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_info_ack(dl, q, mp);
}
static fastcall noinline __unlikely int
dl_attach_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_reply(dl, q, mp, DL_ATTACH_REQ, DL_NOTSUPPORTED);
}
static fastcall noinline __unlikely int
dl_detach_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_reply(dl, q, mp, DL_DETACH_REQ, DL_NOTSUPPORTED);
}

/**
 * dl_bind_req: - precess DL_BIND_REQ primitive
 * @dl: data link private structure
 * @q: active queue (upper write queue)
 * @mp: the DL_BIND_REQ primitive
 *
 * AIXLink/X.25 uses a specialized SAP: byte 1 is 1 for master or 2 for slave,
 * byte 2 is the logical line number, byte 3 and 4 are zero.  This bind
 * operation needs to perform both an attach and an enable.
 */
static fastcall noinline __unlikely int
dl_bind_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_bind_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (dl_get_state(dl) != DL_UNBOUND)
		goto outstate;
	if (p->dl_service_mode != DL_CODLS)
		goto unsupported;
	if (p->dl_xidtest_flg != 0)
		goto noauto;
	if (p->dl_sap & 0x0000ffff)
		goto badsap;
	switch ((p->dl_sap >> 24) & 0xff) {
	case 0:
	case 1:
		break;
	default:
		goto badsap;
	}
	dl->sap = p->dl_sap;
	dl_set_state(dl, DL_BIND_PENDING);
	return cd_attach_req(dl->cd, q, mp, ((p->dl_sap >> 16) & 0x00ff));
      badsap:
	err = DL_BADSAP;
	goto error;
      noauto:
	err = DL_NOAUTO;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_BIND_REQ, err);
}
static fastcall noinline __unlikely int
dl_unbind_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	int err;

	if (dl_get_state(dl) != DL_IDLE)
		goto outstate;
	dl_set_state(dl, DL_UNBIND_PENDING);
	switch (cd_get_state(dl->cd)) {
	case CD_UNATTACHED:
		return dl_ok_ack(dl, q, mp, DL_UNBIND_REQ);
	case CD_DISABLED:
		return cd_detach_req(dl->cd, q, mp);
	default:
		return cd_disable_req(dl->cd, q, mp, CD_FLUSH);
	}
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_UNBIND_REQ, err);
}
static fastcall noinline __unlikely int
dl_subs_bind_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_reply(dl, q, mp, DL_SUBS_BIND_REQ, DL_NOTSUPPORTED);
}
static fastcall noinline __unlikely int
dl_subs_unbind_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_reply(dl, q, mp, DL_SUBS_UNBIND_REQ, DL_NOTSUPPORTED);
}
static fastcall noinline __unlikely int
dl_enabmulti_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_reply(dl, q, mp, DL_ENABMULTI_REQ, DL_NOTSUPPORTED);
}
static fastcall noinline __unlikely int
dl_disabmulti_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_reply(dl, q, mp, DL_DISABMULTI_REQ, DL_NOTSUPPORTED);
}
static fastcall noinline __unlikely int
dl_promiscon_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_reply(dl, q, mp, DL_PROMISCON_REQ, DL_NOTSUPPORTED);
}
static fastcall noinline __unlikely int
dl_promiscoff_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_reply(dl, q, mp, DL_PROMISCOFF_REQ, DL_NOTSUPPORTED);
}
static fastcall noinline __unlikely int
dl_unitdata_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_reply(dl, q, mp, DL_UNITDATA_REQ, DL_UNSUPPORTED);
}
static fastcall noinline __unlikely int
dl_udqos_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_reply(dl, q, mp, DL_UDQOS_REQ, DL_UNSUPPORTED);
}
static fastcall noinline __unlikely int
dl_connect_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_connect_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->dl_dest_addr_offset, p->dl_dest_addr_length))
		goto badaddr;
	if (!MBLKIN(mp, p->dl_qos_offset, p->dl_qos_length))
		goto badqos;
	if (p->dl_dest_addr_length)
		goto badaddr;
	if (p->dl_qos_length)
		goto badqos;
	if (dl_get_state(dl) != DL_IDLE)
		goto outstate;
	dl_set_state(dl, DL_OUTCON_PENDING);
	return cd_enable_req(dl->cd, q, mp);
      badqos:
	err = DL_BADQOSPARAM;
	goto error;
      badaddr:
	err = DL_BADADDR;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_CONNECT_REQ, err);
}
static fastcall noinline __unlikely int
dl_connect_res(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_reply(dl, q, mp, DL_CONNECT_RES, DL_NOTSUPPORTED);
}
static fastcall noinline __unlikely int
dl_token_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_token_ack(dl, q, mp);
}
static fastcall noinline __unlikely int
dl_disconnect_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_disconnect_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	switch (dl_get_state(dl)) {
	case DL_IDLE:
		goto discard;
	case DL_OUTCON_PENDING:
		dl_set_state(dl, DL_DISCON8_PENDING);
		break;
	case DL_INCON_PENDING:
		dl_set_state(dl, DL_DISCON9_PENDING);
		break;
	case DL_DATAXFER:
		dl_set_state(dl, DL_DISCON11_PENDING);
		break;
	case DL_USER_RESET_PENDING:
		dl_set_state(dl, DL_DISCON12_PENDING);
		break;
	case DL_PROV_RESET_PENDING:
		dl_set_state(dl, DL_DISCON13_PENDING);
		break;
	default:
		goto outstate;
	}
	return cd_disable_req(dl->cd, q, mp, CD_FLUSH);
      outstate:
	err = DL_OUTSTATE;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_TOKEN_REQ, err);
      discard:
	freemsg(mp);
	return (0);
}
static fastcall noinline __unlikely int
dl_reset_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	int err;

	if (dl_get_state(dl) == DL_IDLE)
		goto discard;
	if (dl_get_state(dl) != DL_DATAXFER)
		goto outstate;
	dl_set_state(dl, DL_USER_RESET_PENDING);
	return cd_abort_output_req(dl->cd, q, mp);
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_RESET_REQ, err);
      discard:
	freemsg(mp);
	return (0);
}
static fastcall noinline __unlikely int
dl_reset_res(struct dl *dl, queue_t *q, mblk_t *mp)
{
	int err;

	if (dl_get_state(dl) == DL_IDLE)
		goto discard;
	if (dl_get_state(dl) != DL_PROV_RESET_PENDING)
		goto outstate;
	dl_set_state(dl, DL_RESET_RES_PENDING);
	return cd_abort_output_req(dl->cd, q, mp);
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_RESET_RES, err);
      discard:
	freemsg(mp);
	return (0);
}
static fastcall noinline __unlikely int
dl_data_ack_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_reply(dl, q, mp, DL_DATA_ACK_REQ, DL_UNSUPPORTED);
}
static fastcall noinline __unlikely int
dl_reply_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_reply(dl, q, mp, DL_REPLY_REQ, DL_UNSUPPORTED);
}
static fastcall noinline __unlikely int
dl_reply_update_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_reply(dl, q, mp, DL_REPLY_UPDATE_REQ, DL_UNSUPPORTED);
}
static fastcall noinline __unlikely int
dl_xid_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_reply(dl, q, mp, DL_XID_REQ, DL_NOTSUPPORTED);
}
static fastcall noinline __unlikely int
dl_xid_res(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_reply(dl, q, mp, DL_XID_RES, DL_NOTSUPPORTED);
}
static fastcall noinline __unlikely int
dl_test_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_reply(dl, q, mp, DL_TEST_REQ, DL_NOTSUPPORTED);
}
static fastcall noinline __unlikely int
dl_test_res(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_reply(dl, q, mp, DL_TEST_RES, DL_NOTSUPPORTED);
}
static fastcall noinline __unlikely int
dl_phys_addr_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_reply(dl, q, mp, DL_PHYS_ADDR_REQ, DL_NOTSUPPORTED);
}
static fastcall noinline __unlikely int
dl_set_phys_addr_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_reply(dl, q, mp, DL_SET_PHYS_ADDR_REQ, DL_NOTSUPPORTED);
}
static fastcall noinline __unlikely int
dl_get_statistics_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_get_statistics_ack(dl, q, mp);
}
static fastcall noinline __unlikely int
dl_wrongway_ind(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_ulong *p = (typeof(p)) mp->b_rptr;

	return dl_error_reply(dl, q, mp, *p, DL_BADPRIM);
}
static fastcall noinline __unlikely int
dl_other_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_ulong *p = (typeof(p)) mp->b_rptr;

	return dl_error_reply(dl, q, mp, *p, DL_BADPRIM);
}

/*
 * --------------------------------------------------------------------------
 *
 * CDI PROVIDER TO CDI USER RECEIVED PRIMITIVES
 *
 * --------------------------------------------------------------------------
 */

static fastcall noinline __unlikely int
cd_info_ack(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_info_ack_t *p = (typeof(p)) mp->b_rptr;

	if (cd->proto.info.cd_primitive == 0) {
		cd->proto.info.cd_primitive = p->cd_primitive;
		cd->proto.info.cd_state = p->cd_state;
		if (p->cd_state != CD_UNATTACHED)
			goto error;
		cd->proto.info.cd_max_sdu = p->cd_max_sdu;
		cd->dl->proto.info.dl_max_sdu = p->cd_max_sdu;
		cd->proto.info.cd_min_sdu = p->cd_min_sdu;
		cd->dl->proto.info.dl_min_sdu = p->cd_min_sdu;
		cd->proto.info.cd_class = p->cd_class;
		if (p->cd_class != CD_HDLC)
			goto error;
		cd->proto.info.cd_duplex = p->cd_duplex;
		if (p->cd_duplex != CD_FULLDUPLEX)
			goto error;
		cd->proto.info.cd_output_style = p->cd_output_style;
		if (p->cd_output_style != CD_UNACKEDOUTPUT)
			goto error;
		cd->proto.info.cd_features = p->cd_features;
		if (!(p->cd_output_style & CD_AUTOALLOW))
			goto error;
		cd->proto.info.cd_addr_length = p->cd_addr_length;
		if (p->cd_addr_length != 0)
			goto error;
		cd->proto.info.cd_ppa_style = p->cd_ppa_style;
		if (p->cd_ppa_style != CD_STYLE2)
			goto error;
	}
	freemsg(mp);
	return (0);
      error:
	return m_error_reply(cd->dl, q, mp, -EFAULT);
}
static fastcall noinline __unlikely int
cd_ok_ack(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_ok_ack_t *p = (typeof(p)) mp->b_rptr;

	switch (cd_get_state(cd)) {
	case CD_ATTACH_PENDING:
		cd_set_state(cd, CD_DISABLED);
		return dl_bind_ack(cd->dl, q, mp);
	case CD_DETACH_PENDING:
		cd_set_state(cd, CD_UNATTACHED);
		return dl_ok_ack(cd->dl, q, mp, DL_UNBIND_REQ);
	default:
		switch (p->cd_correct_primitive) {
		case CD_ABORT_OUTPUT_REQ:
			switch (dl_get_state(cd->dl)) {
			case DL_RESET_RES_PENDING:
				return dl_ok_ack(cd->dl, q, mp, DL_RESET_RES);
			case DL_USER_RESET_PENDING:
				return dl_reset_con(cd->dl, q, mp);
			}
		}
	}
	return dl_ok_ack(cd->dl, q, mp, 0);
}
dl_long dlerror(cd_error_ack_t *p)
{
	switch (p->cd_errno) {
	case 0:
		return (0);
	case CD_BADADDRESS:
		return (DL_BADADDR);
	case CD_BADADDRTYPE:
		return (DL_BADADDR);
	case CD_BADDIAL:
		return (DL_BADADDR);
	case CD_BADDIALTYPE:
		return (DL_BADADDR);
	case CD_BADDISPOSAL:
		return (DL_BADQOSPARAM);
	case CD_BADFRAME:
		return (DL_BADDATA);
	case CD_BADPPA:
		return (DL_BADPPA);
	case CD_BADPRIM:
		return (DL_BADPRIM);
	case CD_DISC:
		return (DL_NOTINIT);
	case CD_EVENT:
		return (-EPIPE);
	case CD_FATALERR:
		return (-EPROTO);
	case CD_INITFAILED:
		return (DL_INITFAILED);
	case CD_NOTSUPP:
		return (DL_NOTSUPPORTED);
	case CD_OUTSTATE:
		return (DL_OUTSTATE);
	case CD_PROTOSHORT:
		return (-EFAULT);
	case CD_READTIMEOUT:
		return (-ETIMEDOUT);
	case CD_SYSERR:
		return (-p->cd_explanation);
	case CD_WRITEFAIL:
		return (DL_UNDELIVERABLE);
	default:
		return (-EFAULT);
	}
}
static fastcall noinline __unlikely int
cd_error_ack(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_error_ack_t *p = (typeof(p)) mp->b_rptr;

	switch (cd_get_state(cd)) {
	case CD_ATTACH_PENDING:
		cd_set_state(cd, CD_UNATTACHED);
		return dl_error_ack(cd->dl, q, mp, DL_BIND_REQ, dlerror(p));
	case CD_DETACH_PENDING:
		cd_set_state(cd, CD_DISABLED);
		return dl_error_ack(cd->dl, q, mp, DL_UNBIND_REQ, dlerror(p));
	case CD_ENABLE_PENDING:
		cd_set_state(cd, CD_DISABLED);
		return dl_error_ack(cd->dl, q, mp, DL_CONNECT_REQ, dlerror(p));
	case CD_DISABLE_PENDING:
		cd_set_state(cd, CD_ENABLED);
		return dl_error_ack(cd->dl, q, mp, DL_DISCONNECT_REQ, dlerror(p));
	default:
		switch (p->cd_error_primitive) {
		case CD_ABORT_OUTPUT_REQ:
			switch (dl_get_state(cd->dl)) {
			case DL_RESET_RES_PENDING:
				return dl_error_ack(cd->dl, q, mp, DL_RESET_RES, dlerror(p));
			case DL_USER_RESET_PENDING:
				return dl_error_ack(cd->dl, q, mp, DL_RESET_REQ, dlerror(p));
			}
		}
	}
	return dl_error_ack(cd->dl, q, mp, 0, dlerror(p));
}
static fastcall noinline __unlikely int
cd_enable_con(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_enable_con_t *p = (typeof(p)) mp->b_rptr;

	cd_set_state(cd, CD_ENABLED);
	return dl_connect_con(cd->dl, q, mp);
}
static fastcall noinline __unlikely int
cd_disable_con(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_disable_con_t *p = (typeof(p)) mp->b_rptr;

	cd_set_state(cd, CD_DISABLED);
	return dl_ok_ack(cd->dl, q, mp, DL_DISCONNECT_REQ);
}
static fastcall noinline __unlikely int
cd_error_ind(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_error_ind_t *p = (typeof(p)) mp->b_rptr;

	switch (p->cd_state) {
	case CD_UNATTACHED:
	case CD_UNUSABLE:
		cd_set_state(cd, p->cd_state);
		return m_error_reply(cd->dl, q, mp, -EPROTO);
	case CD_DISABLED:
	case CD_DISABLE_PENDING:
		cd_set_state(cd, p->cd_state);
		return dl_disconnect_ind(cd->dl, q, mp, DL_PROVIDER, DL_DISC_ABNORMAL_CONDITION);
	case CD_ENABLE_PENDING:
	case CD_ENABLED:
	case CD_READ_ACTIVE:
	case CD_INPUT_ALLOWED:
	case CD_OUTPUT_ACTIVE:
		cd_set_state(cd, p->cd_state);
		return dl_reset_ind(cd->dl, q, mp, DL_PROVIDER, DL_RESET_LINK_ERROR);
	default:
		freemsg(mp);
		return (0);
	}
}
static fastcall noinline __unlikely int
cd_unitdata_ack(struct cd *cd, queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}
static fastcall inline __hot_in int
cd_unitdata_ind(struct cd *cd, queue_t *q, mblk_t *mp)
{
	return dl_data_ind(cd->dl, q, mp, mp->b_cont);
}
static fastcall noinline __unlikely int
cd_bad_frame_ind(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_bad_frame_ind_t *p = (typeof(p)) mp->b_rptr;

	switch (p->cd_state) {
	case CD_UNATTACHED:
	case CD_UNUSABLE:
		cd_set_state(cd, p->cd_state);
		return m_error_reply(cd->dl, q, mp, -EPROTO);
	case CD_DISABLED:
	case CD_DISABLE_PENDING:
		cd_set_state(cd, p->cd_state);
		return dl_disconnect_ind(cd->dl, q, mp, DL_PROVIDER, DL_DISC_ABNORMAL_CONDITION);
	case CD_ENABLE_PENDING:
	case CD_ENABLED:
	case CD_READ_ACTIVE:
	case CD_INPUT_ALLOWED:
	case CD_OUTPUT_ACTIVE:
		cd_set_state(cd, p->cd_state);
		return dl_reset_ind(cd->dl, q, mp, DL_PROVIDER, DL_RESET_LINK_ERROR);
	default:
		freemsg(mp);
		return (0);
	}
}
static fastcall noinline __unlikely int
cd_modem_sig_ind(struct cd *cd, queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}
static fastcall noinline __unlikely int
cd_wrongway_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_ulong *p = (typeof(p)) mp->b_rptr;

	mi_strlog(q, STRLOGRX, SL_TRACE, "%s: primitive on read queue", cd_primname(*p));
	freemsg(mp);
	return (0);
}
static fastcall noinline __unlikely int
cd_other_ind(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_ulong *p = (typeof(p)) mp->b_rptr;

	mi_strlog(q, STRLOGRX, SL_TRACE, "%s: primitive on read queue", cd_primname(*p));
	freemsg(mp);
	return (0);
}

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS MESSAGE HANDLING
 *
 * -------------------------------------------------------------------------
 */
static fastcall noinline __unlikely int
dl_m_data(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);	/* already locked */

	mi_strlog(q, STRLOGDA, SL_TRACE, "-> M_DATA");
	return dl_data_req(dl, q, mp);
}
static fastcall noinline __unlikely int
dl_m_proto(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_ulong prim;
	int rtn = -EFAULT;

	if (!MBLKIN(mp, 0, sizeof(prim)))
		goto done;
	prim = *(dl_ulong *) mp->b_rptr;
	dl_save_state(dl);
	mi_strlog(q, STRLOGRX, SL_TRACE, "-> %s", dl_primname(prim));
	switch (prim) {
	case DL_INFO_REQ:
		rtn = dl_info_req(dl, q, mp);
		break;
	case DL_ATTACH_REQ:
		rtn = dl_attach_req(dl, q, mp);
		break;
	case DL_DETACH_REQ:
		rtn = dl_detach_req(dl, q, mp);
		break;
	case DL_BIND_REQ:
		rtn = dl_bind_req(dl, q, mp);
		break;
	case DL_UNBIND_REQ:
		rtn = dl_unbind_req(dl, q, mp);
		break;
	case DL_SUBS_BIND_REQ:
		rtn = dl_subs_bind_req(dl, q, mp);
		break;
	case DL_SUBS_UNBIND_REQ:
		rtn = dl_subs_unbind_req(dl, q, mp);
		break;
	case DL_ENABMULTI_REQ:
		rtn = dl_enabmulti_req(dl, q, mp);
		break;
	case DL_DISABMULTI_REQ:
		rtn = dl_disabmulti_req(dl, q, mp);
		break;
	case DL_PROMISCON_REQ:
		rtn = dl_promiscon_req(dl, q, mp);
		break;
	case DL_PROMISCOFF_REQ:
		rtn = dl_promiscoff_req(dl, q, mp);
		break;
	case DL_UNITDATA_REQ:
		rtn = dl_unitdata_req(dl, q, mp);
		break;
	case DL_UDQOS_REQ:
		rtn = dl_udqos_req(dl, q, mp);
		break;
	case DL_CONNECT_REQ:
		rtn = dl_connect_req(dl, q, mp);
		break;
	case DL_CONNECT_RES:
		rtn = dl_connect_res(dl, q, mp);
		break;
	case DL_TOKEN_REQ:
		rtn = dl_token_req(dl, q, mp);
		break;
	case DL_DISCONNECT_REQ:
		rtn = dl_disconnect_req(dl, q, mp);
		break;
	case DL_RESET_REQ:
		rtn = dl_reset_req(dl, q, mp);
		break;
	case DL_RESET_RES:
		rtn = dl_reset_res(dl, q, mp);
		break;
	case DL_DATA_ACK_REQ:
		rtn = dl_data_ack_req(dl, q, mp);
		break;
	case DL_REPLY_REQ:
		rtn = dl_reply_req(dl, q, mp);
		break;
	case DL_REPLY_UPDATE_REQ:
		rtn = dl_reply_update_req(dl, q, mp);
		break;
	case DL_XID_REQ:
		rtn = dl_xid_req(dl, q, mp);
		break;
	case DL_XID_RES:
		rtn = dl_xid_res(dl, q, mp);
		break;
	case DL_TEST_REQ:
		rtn = dl_test_req(dl, q, mp);
		break;
	case DL_TEST_RES:
		rtn = dl_test_res(dl, q, mp);
		break;
	case DL_PHYS_ADDR_REQ:
		rtn = dl_phys_addr_req(dl, q, mp);
		break;
	case DL_SET_PHYS_ADDR_REQ:
		rtn = dl_set_phys_addr_req(dl, q, mp);
		break;
	case DL_GET_STATISTICS_REQ:
		rtn = dl_get_statistics_req(dl, q, mp);
		break;
	case DL_INFO_ACK:
	case DL_BIND_ACK:
	case DL_OK_ACK:
	case DL_ERROR_ACK:
	case DL_SUBS_BIND_ACK:
	case DL_TOKEN_ACK:
	case DL_PHYS_ADDR_ACK:
	case DL_GET_STATISTICS_ACK:
	case DL_UNITDATA_IND:
	case DL_UDERROR_IND:
	case DL_CONNECT_IND:
	case DL_CONNECT_CON:
	case DL_DISCONNECT_IND:
	case DL_RESET_IND:
	case DL_RESET_CON:
	case DL_DATA_ACK_IND:
	case DL_DATA_ACK_STATUS_IND:
	case DL_REPLY_IND:
	case DL_REPLY_STATUS_IND:
	case DL_REPLY_UPDATE_STATUS_IND:
	case DL_XID_IND:
	case DL_XID_CON:
	case DL_TEST_IND:
	case DL_TEST_CON:
		rtn = dl_wrongway_ind(dl, q, mp);
		break;
	default:
		rtn = dl_other_req(dl, q, mp);
		break;
	}
      done:
	if (rtn)
		dl_restore_state(dl);
	return (0);
}
static fastcall noinline __unlikely int
dl_m_ctl(queue_t *q, mblk_t *mp)
{
	/* No safe path for now... */
	return dl_m_proto(q, mp);
}
static fastcall noinline __unlikely int
dl_m_sig(queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}
static fastcall noinline __unlikely int
dl_m_rse(queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}
static fastcall noinline __unlikely int
dl_m_flush(queue_t *q, mblk_t *mp)
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
static fastcall noinline __unlikely int
dl_m_ioctl(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}
static fastcall noinline __unlikely int
dl_m_iocdata(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}
static fastcall noinline __unlikely int
dl_m_read(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}
static fastcall noinline __unlikely int
dl_m_other(queue_t *q, mblk_t *mp)
{
	mi_strlog(q, 0, SL_ERROR, "%s; bad message type on write queue %d\n", __FUNCTION__,
		  (int) DB_TYPE(mp));
	putnext(q, mp);
	return (0);
}

static fastcall noinline __unlikely int
cd_m_data(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}
static fastcall noinline __unlikely int
cd_m_proto(queue_t *q, mblk_t *mp)
{
	struct cd *cd = CD_PRIV(q);	/* already locked */
	cd_ulong prim;
	int rtn = -EFAULT;

	if (!MBLKIN(mp, 0, sizeof(prim)))
		goto done;
	prim = *(cd_ulong *) mp->b_rptr;
	cd_save_state(cd);
	if (likely(prim == CD_UNITDATA_IND)) {
		mi_strlog(q, STRLOGRX, SL_TRACE, "CD_UNITDATA_IND <-");
		rtn = cd_unitdata_ind(cd, q, mp);
		goto done;
	}
	mi_strlog(q, STRLOGRX, SL_TRACE, "%s <-", cd_primname(prim));
	switch (prim) {
	case CD_INFO_ACK:
		rtn = cd_info_ack(cd, q, mp);
		break;
	case CD_OK_ACK:
		rtn = cd_ok_ack(cd, q, mp);
		break;
	case CD_ERROR_ACK:
		rtn = cd_error_ack(cd, q, mp);
		break;
	case CD_ENABLE_CON:
		rtn = cd_enable_con(cd, q, mp);
		break;
	case CD_DISABLE_CON:
		rtn = cd_disable_con(cd, q, mp);
		break;
	case CD_ERROR_IND:
		rtn = cd_error_ind(cd, q, mp);
		break;
	case CD_UNITDATA_ACK:
		rtn = cd_unitdata_ack(cd, q, mp);
		break;
	case CD_UNITDATA_IND:
		rtn = cd_unitdata_ind(cd, q, mp);
		break;
	case CD_BAD_FRAME_IND:
		rtn = cd_bad_frame_ind(cd, q, mp);
		break;
	case CD_MODEM_SIG_IND:
		rtn = cd_modem_sig_ind(cd, q, mp);
		break;
	case CD_INFO_REQ:
	case CD_ATTACH_REQ:
	case CD_DETACH_REQ:
	case CD_ENABLE_REQ:
	case CD_DISABLE_REQ:
	case CD_ALLOW_INPUT_REQ:
	case CD_READ_REQ:
	case CD_UNITDATA_REQ:
	case CD_WRITE_READ_REQ:
	case CD_HALT_INPUT_REQ:
	case CD_ABORT_OUTPUT_REQ:
	case CD_MUX_NAME_REQ:
	case CD_MODEM_SIG_REQ:
	case CD_MODEM_SIG_POLL:
		rtn = cd_wrongway_req(cd, q, mp);
		break;
	default:
		rtn = cd_other_ind(cd, q, mp);
		break;
	}
      done:
	if (rtn)
		cd_restore_state(cd);
	return (0);
}
static fastcall noinline __unlikely int
cd_m_sig(queue_t *q, mblk_t *mp)
{
	if (!MBLKIN(mp, 0, sizeof(int))) {
		if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
			putnext(q, mp);
			return (0);
		}
		return (-EBUSY);
	}
	freemsg(mp);		/* process when we have timers */
	return (0);
}
static fastcall noinline __unlikely int
cd_m_ctl(queue_t *q, mblk_t *mp)
{
	/* Normal path _is_ safe path. */
	return cd_m_proto(q, mp);
}
static fastcall noinline __unlikely int
cd_m_rse(queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}
static fastcall noinline __unlikely int
cd_m_flush(queue_t *q, mblk_t *mp)
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
static fastcall noinline __unlikely int
cd_m_error(queue_t *q, mblk_t *mp)
{
	cd_set_state(cd, CD_NOT_AUTH);
	flushq(q, FLUSHALL);
	flushq(WR(q), FLUSHALL);
	putnext(q, mp);
	return (0);
}
static fastcall noinline __unlikely int
cd_m_hangup(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}
static fastcall noinline __unlikely int
cd_m_setopts(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}
static fastcall noinline __unlikely int
cd_m_copy(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}
static fastcall noinline __unlikely int
cd_m_iocack(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}
static fastcall noinline __unlikely int
cd_m_other(queue_t *q, mblk_t *mp)
{
	mi_strlog(q, 0, SL_ERROR, "%s: unrecognized STREAMS message on read queue, %d",
		  __FUNCTION__, (int) DB_TYPE(mp));
	putnext(q, mp);
	return (0);
}

static fastcall inline int
dl_msg_put(queue_t *q, mblk_t *mp)
{
	if (likely(DB_TYPE(mp) == M_DATA))
		return (-EAGAIN);	/* fast path for data */
	if (likely(DB_TYPE(mp) == M_PROTO))
		return (-EAGAIN);	/* fast path for data */
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
	case M_PROTO:
	case M_PCPROTO:
	case M_CTL:
	case M_PCCTL:
	case M_SIG:
	case M_PCSIG:
	case M_RSE:
	case M_PCRSE:
		return (-EAGAIN);
	case M_FLUSH:
		return dl_m_flush(q, mp);
	case M_IOCTL:
		return dl_m_ioctl(q, mp);
	case M_IOCDATA:
		return dl_m_iocdata(q, mp);
	case M_READ:
		return dl_m_read(q, mp);
	default:
		return dl_m_other(q, mp);
	}
}
static fastcall noinline int
dl_msg_srv(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);

	if (unlikely(dl->proto.info.dl_primitive == 0))
		return (-EAGAIN);
	if (likely(DB_TYPE(mp) == M_DATA))
		return dl_m_data(q, mp);
	if (likely(DB_TYPE(mp) == M_PROTO))
		return dl_m_proto(q, mp);
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		return dl_m_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return dl_m_proto(q, mp);
	case M_CTL:
	case M_PCCTL:
		return dl_m_ctl(q, mp);
	case M_SIG:
	case M_PCSIG:
		return dl_m_sig(q, mp);
	case M_RSE:
	case M_PCRSE:
		return dl_m_rse(q, mp);
	case M_FLUSH:
		return dl_m_flush(q, mp);
	case M_IOCTL:
		return dl_m_ioctl(q, mp);
	case M_IOCDATA:
		return dl_m_iocdata(q, mp);
	case M_READ:
		return dl_m_read(q, mp);
	default:
		return dl_m_other(q, mp);
	}
}

static fastcall noinline int
cd_msg_srv(queue_t *q, mblk_t *mp)
{
	if (likely(DB_TYPE(mp) == M_PROTO))
		return cd_m_proto(q, mp);
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		return cd_m_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return cd_m_proto(q, mp);
	case M_SIG:
	case M_PCSIG:
		return cd_m_sig(q, mp);
	case M_CTL:
	case M_PCCTL:
		return cd_m_ctl(q, mp);
	case M_RSE:
	case M_PCRSE:
		return cd_m_rse(q, mp);
	case M_FLUSH:
		return cd_m_flush(q, mp);
	case M_ERROR:
		return cd_m_error(q, mp);
	case M_HANGUP:
	case M_UNHANGUP:
		return cd_m_hangup(q, mp);
	case M_SETOPTS:
	case M_PCSETOPTS:
		return cd_m_setopts(q, mp);
	case M_COPYIN:
	case M_COPYOUT:
		return cd_m_copy(q, mp);
	case M_IOCACK:
	case M_IOCNAK:
		return cd_m_iocack(q, mp);
	default:
		return cd_m_other(q, mp);
	}
}
static fastcall inline int
cd_msg_put(queue_t *q, mblk_t *mp)
{
	if (likely(DB_TYPE(mp) == M_PROTO))
		return (-EAGAIN);	/* fast path for data */
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
	case M_PROTO:
	case M_PCPROTO:
	case M_SIG:
	case M_PCSIG:
	case M_CTL:
	case M_PCCTL:
	case M_RSE:
	case M_PCRSE:
		return (-EAGAIN);	/* handle from service procedure */
	case M_FLUSH:
		return cd_m_flush(q, mp);
	case M_ERROR:
		return cd_m_error(q, mp);
	case M_HANGUP:
	case M_UNHANGUP:
		return cd_m_hangup(q, mp);
	case M_SETOPTS:
	case M_PCSETOPTS:
		return cd_m_setopts(q, mp);
	case M_COPYIN:
	case M_COPYOUT:
		return cd_m_copy(q, mp);
	case M_IOCACK:
	case M_IOCNAK:
		return cd_m_iocack(q, mp);
	default:
		return cd_m_other(q, mp);
	}
}

/*
 * --------------------------------------------------------------------------
 *
 * PUT AND SERVICE PROCEDURES
 *
 * --------------------------------------------------------------------------
 */

static streamscall int
dl_wput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || dl_msg_put(q, mp))
		putq(q, mp);
	return (0);
}
static streamscall int
dl_wsrv(queue_t *q)
{
	caddr_t priv;

	if ((priv = mi_trylock(q)) != NULL) {
		mblk_t *mp;

		while ((mp = getq(q))) {
			if (dl_msg_srv(q, mp)) {
				putbq(q, mp);
				break;
			}
		}
		mi_unlock(priv);
	}
	return (0);
}

static streamscall int
cd_rsrv(queue_t *q)
{
	caddr_t priv;

	if ((priv = mi_trylock(q)) != NULL) {
		mblk_t *mp;

		while ((mp = getq(q))) {
			if (cd_msg_srv(q, mp)) {
				putbq(q, mp);
				break;
			}
		}
		mi_unlock(priv);
	}
	return (0);
}
static streamscall int
cd_rput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || cd_msg_put(q, mp))
		putq(q, mp);
	return (0);
}

/*
 * --------------------------------------------------------------------------
 *
 * OPEN AND CLOSE ROUTINES
 *
 * --------------------------------------------------------------------------
 */

caddr_t dl_opens = NULL;

static int
dl_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct priv *p;
	struct dl *dl;
	struct cd *cd;
	dl_info_ack_t *dli;
	cd_info_ack_t *cdi;
	mblk_t *mp;
	int err;

	if (q->q_ptr != NULL)
		return (0);	/* already open */
	if (!(mp = allocb(sizeof(cd_info_req_t), BPRI_WAITOK)))
		return (ENOBUFS);
	if ((err = mi_open_comm(&dl_opens, sizeof(*p), q, devp, oflags, sflag, crp))) {
		freeb(mp);
		return (err);
	}

	p = PRIV(q);
	bzero(p, sizeof(*p));
	dl = &p->dlpi;
	cd = &p->cdi;

	/* initialize private structure */
	dl->priv = p;
	dl->cd = cd;
	dl->oq = q;
	dl->state = DL_UNBOUND;
	dl->oldstate = DL_UNBOUND;

	dli = &dl->proto.info;
	dli->dl_primitive = 0;	/* set to DL_INFO_ACK when initialized */
	dli->dl_max_sdu = 4096;
	dli->dl_min_sdu = 1;
	dli->dl_addr_length = 0;
	dli->dl_mac_type = DL_HDLC;
	dli->dl_reserved = 0;
	dli->dl_current_state = DL_UNBOUND;
	dli->dl_sap_length = 0;
	dli->dl_service_mode = DL_CODLS;
	dli->dl_qos_length = 0;
	dli->dl_qos_offset = 0;
	dli->dl_qos_range_length = 0;
	dli->dl_qos_range_offset = 0;
	dli->dl_provider_style = DL_STYLE1;
	dli->dl_addr_offset = 0;
	dli->dl_version = DL_CURRENT_VERSION;
	dli->dl_brdcst_addr_length = 0;
	dli->dl_brdcst_addr_offset = 0;
	dli->dl_growth = 0;

	cd->priv = p;
	cd->dl = dl;
	cd->oq = WR(q);
	cd->state = CD_UNUSABLE;
	cd->oldstate = CD_UNUSABLE;

	cdi = &cd->proto.info;
	cdi->cd_primitive = 0;	/* set to CD_INFO_ACK when initialized */
	cdi->cd_state = CD_UNUSABLE;
	cdi->cd_max_sdu = STRMAXPSZ;
	cdi->cd_min_sdu = STRMINPSZ;
	cdi->cd_class = CD_HDLC;
	cdi->cd_duplex = CD_FULLDUPLEX;
	cdi->cd_output_style = CD_UNACKEDOUTPUT;
	cdi->cd_features = (CD_CANREAD | CD_AUTOALLOW);
	cdi->cd_addr_length = 0;
	cdi->cd_ppa_style = CD_STYLE2;

	/* Issue immediate info request. */
	DB_TYPE(mp) = M_PCPROTO;
	((cd_info_req_t *) mp->b_wptr)->cd_primitive = CD_INFO_REQ;
	mp->b_wptr += sizeof(cd_info_req_t);

	qprocson(q);
	putnext(q, mp);
	return (0);
}

static int
dl_qclose(queue_t *q, int oflags, cred_t *crp)
{
	struct priv *p = PRIV(q);

	if (p == NULL)
		return (0);
	qprocsoff(q);
	mi_close_comm(&dl_opens, q);
	return (0);
}

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS INITIALIZATION
 *
 * --------------------------------------------------------------------------
 */

static struct qinit dl_rinit = {
	.qi_putp = &cd_rput,
	.qi_srvp = &cd_rsrv,
	.qi_qopen = &dl_qopen,
	.qi_qclose = &dl_qclose,
	.qi_minfo = &dl_minfo,
	.qi_mstat = &dl_rstat,
};

static struct qinit dl_winit = {
	.qi_putp = &dl_wput,
	.qi_srvp = &dl_wsrv,
	.qi_minfo = &dl_minfo,
	.qi_mstat = &dl_wstat,
};

struct streamtab dlaix_info = {
	.st_rdinit = &dl_rinit,
	.st_wrinit = &dl_winit,
};

static modID_t modid = MOD_ID;

/*
 * --------------------------------------------------------------------------
 *
 * LINUX FAST STREAMS REGISTRATION
 *
 * --------------------------------------------------------------------------
 */

#ifdef LINUX

#ifndef module_param
MODULE_PARM(modid, "h");
#else				/* module_param */
module_param(modid, ushort, 0444);
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for LAPB.  (0 for allocation.)");

#ifdef LIS
#define fmodsw _fmodsw
#endif				/* LIS */

static struct fmodsw dl_fmod = {
	.f_name = MOD_NAME,
	.f_str = &dlaix_info,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

static __init int
dlaix_modinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);
	if ((err = register_strmod(&dl_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module %d", MOD_NAME, (int) modid);
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

static __exit void
dlaix_modexit(void)
{
	int err;

	if ((err = unregister_strmod(&dl_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister module, err = %d", MOD_NAME, err);
		return;
	}
	return;
}

module_init(dlaix_modinit);
module_exit(dlaix_modexit);

#endif				/* LINUX */
