/*****************************************************************************

 @(#) $RCSfile: sscop.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2008-12-07 10:40:24 $

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

 Last Modified $Date: 2008-12-07 10:40:24 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sscop.c,v $
 Revision 0.9.2.2  2008-12-07 10:40:24  brian
 - new stratm package

 Revision 0.9.2.1  2008-12-06 08:20:53  brian
 - added base release files for stratm package

 *****************************************************************************/

#ident "@(#) $RCSfile: sscop.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2008-12-07 10:40:24 $"

static char const ident[] = "$RCSfile: sscop.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2008-12-07 10:40:24 $";

/*
 * This is an SSCOPE module for use with MTP3b.  It can be pushed over an AAL5 Stream to provide the
 * SSCF interface needed by MTP3b.  For the upper interface boundary, see Q.2210 (SSCOPE) and NPI.
 * For the lower interface boundary, see DLPI.
 */

#define _LFS_SOURCE	1
#define _SVR4_SOURCE	1
#define _MPS_SOURCE	1

#include <sys/os7/compat.h>
#include <sys/strsun.h>
#include <sys/strconf.h>

#include <sys/npi.h>
#include <sys/dlpi.h>

#define SSCOP_DESCRIP	"MTP3B-SSCOP STREAMS MODULE."
#define SSCOP_REVISION	"OpenSS7 $RCSfile: sscop.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2008-12-07 10:40:24 $"
#define SSCOP_COPYRIGHT	"Copyright (c) 1997-2008  OpenSS7 Corporation.  All Rights Reserved."
#define SSCOP_DEVICE	"Provides OpenSS7 MTP3B-I.432.3-SSCOP module."
#define SSCOP_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SSCOP_LICENSE	"GPL"
#define SSCOP_BANNER	SSCOP_DESCRIP	"\n" \
			SSCOP_REVISION	"\n" \
			SSCOP_COPYRIGHT	"\n" \
			SSCOP_DEVICE	"\n" \
			SSCOP_CONTACT	"\n"
#define SSCOP_SPLASH	SSCOP_DEVICE	" - " \
			SSCOP_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(SSCOP_CONTACT);
MODULE_DESCRIPTION(SSCOP_DESCRIP);
MODULE_SUPPORTED_DEVICE(SSCOP_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(SSCOP_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-sscop");
#endif				/* MODULE_ALIAS */
#ifdef MODULE_VERSION
MODULE_VERSION(__stringify(PACKAGE_RPMEPOCH) ":" PACKAGE_VERSION "." PACKAGE_RELEASE
	       PACKAGE_PATCHLEVEL "-" PACKAGE_RPMRELEASE PACKAGE_RPMEXTRA2);
#endif				/* MODULE_VERSION */
#endif				/* LINUX */

#ifdef LFS
#define SSCOP_MOD_ID	CONFIG_STREAMS_SSCOP_MODID
#define SSCOP_MOD_NAME	CONFIG_STREAMS_SSCOP_NAME
#endif				/* LFS */

#define MOD_ID		SSCOP_MOD_ID
#define MOD_NAME	SSCOP_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	SSCOP_BANNER
#else				/* MODULE */
#define MOD_BANNER	SSCOP_SPLASH
#endif				/* MODULE */

/*
 * PRIVATE STRUCTURE
 */

struct state {
	uint32_t state;
	uint32_t pending;
};

struct np;
struct dl;

struct np {
	struct dl *dl;
	queue_t *oq;
	queue_t *iq;
	struct state state, oldstate;
	N_info_ack_t info;
	uint32_t ppa;
};

struct dl {
	struct np *np;
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
	struct np np;
	struct dl dl;
};

static rwlock_t sc_lock = RW_LOCK_UNLOCKED;
static caddr_t sc_opens;

/*
 * PRINTING STUFF
 */

/*
 * Printing primitives
 */

/* Upper */
static inline fastcall const char *
np_primname(const np_ulong prim)
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
np_statename(const np_ulong state)
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
np_errname(const np_long error)
{
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
np_strerror(const np_long error)
{
	switch (error) {
	case NBADADDR:
		return ("Incorrect address format/illegal address information");
	case NBADOPT:
		return ("Options in incorrect format or contain illegal information");
	case NACCESS:
		return ("User did not have proper permissions");
	case NNOADDR:
		return ("NS Provider could not allocate address");
	case NOUTSTATE:
		return ("Primitive was issues in wrong sequence");
	case NBADSEQ:
		return ("Sequence number in primitive was incorrect/illegal");
	case NSYSERR:
		return ("UNIX system error occurred");
	case NBADDATA:
		return ("User data spec. outside range supported by NS provider");
	case NBADFLAG:
		return ("Flags specified in primitive were illegal/incorrect");
	case NNOTSUPPORT:
		return ("Primitive type not supported by the NS provider");
	case NBOUND:
		return ("Illegal second attempt to bind listener or default listener");
	case NBADQOSPARAM:
		return ("QOS values specified are outside the range supported by the NS provider");
	case NBADQOSTYPE:
		return ("QOS structure type specified is not supported by the NS provider");
	case NBADTOKEN:
		return ("Token used is not associated with an open stream");
	case NNOPROTOID:
		return ("Protocol id could not be allocated");
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

static inline fastcall np_ulong
np_get_state(struct np *np)
{
	return np->state.state;
}
static inline fastcall np_ulong
np_set_state(struct np *np, const np_ulong newstate)
{
	const np_ulong oldstate = np_get_state(np);

	if (likely(oldstate != newstate)) {
		mi_strlog(np->iq, STRLOGST, SL_TRACE, "%s <- %s", np_statename(newstate),
			  np_statename(oldstate));
		np->state.state = newstate;
		np->info.CURRENT_state = newstate;
	}
	return (newstate);
}
static inline fastcall np_ulong
np_save_state(struct np *np)
{
	np->oldstate = np->state;
	return (np->state.state);
}
static inline fastcall np_ulong
np_restore_state(struct np *np)
{
	np->state = np->oldstate;
	return (np->info.CURRENT_state = np->state.state);
}
static inline fastcall np_ulong
np_set_pending(struct np *np, const np_ulong pending)
{
	return (np->state.pending = pending);
}
static inline fastcall np_ulong
np_get_pending(struct np *np)
{
	return np->state.pending;
}
static inline fastcall np_ulong
np_clear_pending(struct np *np)
{
	const np_ulong pending = np->state.pending;

	np->state.pending = -1U;
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
sc_stripmsg(mblk_t *msg, mblk_t *dp)
{
	mblk_t *b, *b_next = msg;

	while ((b = b_next) && b != dp) {
		b_next = b->b_next;
		freeb(b);
	}
	return;
}

noinline fastcall int
np_txprim(struct np *np, queue_t *q, mblk_t *msg, mblk_t *mp, np_ulong prim)
{
	if (likely(pcmsg(DB_TYPE(mp)) || bcanputnext(np->oq, mp->b_band))) {
		int level;

		switch (__builtin_expect(prim, N_DATA_IND)) {
		case N_DATA_IND:
			level = STRLOGDA;
			break;
		default:
			level = STRLOGTX;
			break;
		}
		mi_strlog(np->oq, level, SL_TRACE, "<- %s", np_primname(prim));
		sc_stripmsg(msg, mp->b_cont);
		putnext(np->oq, mp);
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
		sc_stripmsg(msg, mp->b_cont);
		putnext(dl->oq, mp);
		return (0);
	}
	freeb(mp);
	return (-EBUSY);
}

noinline fastcall void
np_rxprim(struct np *np, queue_t *q, mblk_t *mp, np_ulong prim)
{
	int level;

	switch (__builtin_expect(prim, N_DATA_REQ)) {
	case N_DATA_REQ:
		level = STRLOGDA;
		break;
	default:
		level = STRLOGRX;
		break;
	}
	mi_strlog(np->iq, level, SL_TRACE, "-> %s", np_primname(prim));
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
 * NP USER TO NP PROVIDER PRIMITIVES
 */

static inline fastcall int
n_conn_ind(struct np *np, queue_t *q, mblk_t *msg)
{
	N_conn_ind_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_CONN_IND;
		mp->b_wptr += sizeof(*p);
		return np_txprim(np, q, msg, mp, N_CONN_IND);
	}
	return (-ENOBUFS);
}
static inline fastcall int
n_conn_con(struct np *np, queue_t *q, mblk_t *msg)
{
	N_conn_con_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_CONN_CON;
		mp->b_wptr += sizeof(*p);
		return np_txprim(np, q, msg, mp, N_CONN_CON);
	}
	return (-ENOBUFS);
}
static inline fastcall int
n_discon_ind(struct np *np, queue_t *q, mblk_t *msg)
{
	N_discon_ind_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_DISCON_IND;
		mp->b_wptr += sizeof(*p);
		return np_txprim(np, q, msg, mp, N_DISCON_IND);
	}
	return (-ENOBUFS);
}
static inline fastcall int
n_data_ind(struct np *np, queue_t *q, mblk_t *msg)
{
	N_data_ind_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_DATA_IND;
		mp->b_wptr += sizeof(*p);
		return np_txprim(np, q, msg, mp, N_DATA_IND);
	}
	return (-ENOBUFS);
}
static inline fastcall int
n_exdata_ind(struct np *np, queue_t *q, mblk_t *msg)
{
	N_exdata_ind_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_EXDATA_IND;
		mp->b_wptr += sizeof(*p);
		return np_txprim(np, q, msg, mp, N_EXDATA_IND);
	}
	return (-ENOBUFS);
}
static inline fastcall int
n_info_ack(struct np *np, queue_t *q, mblk_t *msg)
{
	N_info_ack_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_INFO_ACK;
		mp->b_wptr += sizeof(*p);
		return np_txprim(np, q, msg, mp, N_INFO_ACK);
	}
	return (-ENOBUFS);
}
static inline fastcall int
n_bind_ack(struct np *np, queue_t *q, mblk_t *msg)
{
	N_bind_ack_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_BIND_ACK;
		mp->b_wptr += sizeof(*p);
		return np_txprim(np, q, msg, mp, N_BIND_ACK);
	}
	return (-ENOBUFS);
}
static inline fastcall int
n_error_ack(struct np *np, queue_t *q, mblk_t *msg, np_ulong prim, np_long error)
{
	N_error_ack_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_ERROR_ACK;
		p->ERROR_prim = prim;
		p->NPI_error = error < 0 ? NSYSERR : error;
		p->UNIX_error = error < 0 ? -error : 0;
		mp->b_wptr += sizeof(*p);
		return np_txprim(np, q, msg, mp, N_ERROR_ACK);
	}
	return (-ENOBUFS);
}
static inline fastcall int
n_reply_error(struct np *np, queue_t *q, mblk_t *msg, np_ulong prim, np_long error)
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
	case N_DATA_REQ:
	case N_EXDATA_REQ:
		goto merror;
	default:
		goto errorack;
	}
      errorack:
	mi_strlog(np->iq, STRLOGERR, SL_TRACE, "ERROR: %s: (%s) %s", np_primname(prim),
		  np_errname(error), np_strerror(error));
	return n_error_ack(np, q, msg, prim, error);
      merror:
	mi_strlog(np->iq, STRLOGERR, SL_ERROR | SL_TRACE, "<- M_ERROR");
	return m_error(q, msg, EPROTO, EPROTO);
}
static inline fastcall int
n_ok_ack(struct np *np, queue_t *q, mblk_t *msg)
{
	N_ok_ack_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_OK_ACK;
		mp->b_wptr += sizeof(*p);
		return np_txprim(np, q, msg, mp, N_OK_ACK);
	}
	return (-ENOBUFS);
}
static inline fastcall int
n_unitdata_ind(struct np *np, queue_t *q, mblk_t *msg)
{
	N_unitdata_ind_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_UNITDATA_IND;
		mp->b_wptr += sizeof(*p);
		return np_txprim(np, q, msg, mp, N_UNITDATA_IND);
	}
	return (-ENOBUFS);
}
static inline fastcall int
n_uderror_ind(struct np *np, queue_t *q, mblk_t *msg)
{
	N_uderror_ind_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_UDERROR_IND;
		mp->b_wptr += sizeof(*p);
		return np_txprim(np, q, msg, mp, N_UDERROR_IND);
	}
	return (-ENOBUFS);
}
static inline fastcall int
n_datack_ind(struct np *np, queue_t *q, mblk_t *msg)
{
	N_datack_ind_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_DATACK_IND;
		mp->b_wptr += sizeof(*p);
		return np_txprim(np, q, msg, mp, N_DATACK_IND);
	}
	return (-ENOBUFS);
}
static inline fastcall int
n_reset_ind(struct np *np, queue_t *q, mblk_t *msg)
{
	N_reset_ind_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_RESET_IND;
		mp->b_wptr += sizeof(*p);
		return np_txprim(np, q, msg, mp, N_RESET_IND);
	}
	return (-ENOBUFS);
}
static inline fastcall int
n_reset_con(struct np *np, queue_t *q, mblk_t *msg)
{
	N_reset_con_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_RESET_CON;
		mp->b_wptr += sizeof(*p);
		return np_txprim(np, q, msg, mp, N_RESET_CON);
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
 * NP USER TO NP PROVIDER PRIMITIVES
 */

noinline fastcall __unlikely int
n_conn_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_conn_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = NNOTSUPPORT;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
n_conn_res(struct np *np, queue_t *q, mblk_t *mp)
{
	N_conn_res_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = NNOTSUPPORT;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
n_discon_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_discon_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = NNOTSUPPORT;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
static inline fastcall __hot_write int
n_write(struct np *np, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	/* FIXME: for now */
	return NNOTSUPPORT;
}
noinline fastcall __unlikely int
n_data_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_data_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = NNOTSUPPORT;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
n_exdata_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_exdata_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = NNOTSUPPORT;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
n_info_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_info_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = NNOTSUPPORT;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
n_bind_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_bind_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = NNOTSUPPORT;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
n_unbind_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_unbind_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = NNOTSUPPORT;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
n_unitdata_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_unitdata_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = NNOTSUPPORT;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
n_optmgmt_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_optmgmt_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = NNOTSUPPORT;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
n_datack_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_datack_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = NNOTSUPPORT;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
n_reset_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_reset_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = NNOTSUPPORT;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline fastcall __unlikely int
n_reset_res(struct np *np, queue_t *q, mblk_t *mp)
{
	N_reset_res_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = NNOTSUPPORT;
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
 * np_w_data: - process received M_DATA message from above
 * @np: CH private structure (locked)
 * @q: active queue (upper write queue)
 * @mp: the M_DATA message
 */
static fastcall int
np_w_data(struct np *np, queue_t *q, mblk_t *mp)
{
	int err;

	np_save_state(np);
	err = n_write(np, q, NULL, mp);
	if (likely(err == 0))
		return (err);
	np_restore_state(np);
	return n_reply_error(np, q, mp, N_DATA_REQ, err);
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
np_w_proto(struct np *np, queue_t *q, mblk_t *mp)
{
	np_ulong prim = -1U;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(prim))))
		goto tooshort;
	prim = *(typeof(prim) *) mp->b_rptr;
	np_rxprim(np, q, mp, prim);
	np_save_state(np);
	switch (__builtin_expect(prim, N_DATA_REQ)) {
	case N_CONN_REQ:
		err = n_conn_req(np, q, mp);
		break;
	case N_CONN_RES:
		err = n_conn_res(np, q, mp);
		break;
	case N_DISCON_REQ:
		err = n_discon_req(np, q, mp);
		break;
	case N_DATA_REQ:
		err = n_data_req(np, q, mp);
		break;
	case N_EXDATA_REQ:
		err = n_exdata_req(np, q, mp);
		break;
	case N_INFO_REQ:
		err = n_info_req(np, q, mp);
		break;
	case N_BIND_REQ:
		err = n_bind_req(np, q, mp);
		break;
	case N_UNBIND_REQ:
		err = n_unbind_req(np, q, mp);
		break;
	case N_UNITDATA_REQ:
		err = n_unitdata_req(np, q, mp);
		break;
	case N_OPTMGMT_REQ:
		err = n_optmgmt_req(np, q, mp);
		break;
	case N_DATACK_REQ:
		err = n_datack_req(np, q, mp);
		break;
	case N_RESET_REQ:
		err = n_reset_req(np, q, mp);
		break;
	case N_RESET_RES:
		err = n_reset_res(np, q, mp);
		break;
	default:
		err = NNOTSUPPORT;
		break;
	}
	if (likely(err == 0))
		return (err);
      out:
	np_restore_state(np);
	return n_reply_error(np, q, mp, prim, err);
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
np_w_ctl(struct np *np, queue_t *q, mblk_t *mp)
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
np_w_ioctl(struct np *np, queue_t *q, mblk_t *mp)
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
np_wsrv_msg(struct np *np, queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_DATA)) {
	case M_DATA:
	case M_HPDATA:
		return np_w_data(np, q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return np_w_proto(np, q, mp);
	case M_CTL:
	case M_PCCTL:
		return np_w_ctl(np, q, mp);
	case M_IOCTL:
	case M_IOCDATA:
		return np_w_ioctl(np, q, mp);
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
np_wput_msg(queue_t *q, mblk_t *mp)
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
				err = np_w_data(&priv->np, q, mp);
				break;
			case M_PCPROTO:
				err = np_w_proto(&priv->np, q, mp);
				break;
			case M_PCCTL:
				err = np_w_ctl(&priv->np, q, mp);
				break;
			case M_IOCDATA:
				err = np_w_ioctl(&priv->np, q, mp);
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

/*
 * QUEUE PUT AND SERVICE PROCEDURES
 */

static streamscall int
np_wput(queue_t *q, mblk_t *mp)
{
	if (unlikely((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY)))
		     || np_wput_msg(q, mp))) {
		if (unlikely(!putq(q, mp))) {
			mp->b_band = 0;
			putq(q, mp);
		}
	}
	return (0);
}
static streamscall int
np_wsrv(queue_t *q)
{
	struct priv *priv;
	mblk_t *mp;

	if (likely(!!(priv = (typeof(priv)) mi_trylock(q)))) {
		while (likely(!!(mp = getq(q)))) {
			if (unlikely(np_wsrv_msg(&priv->np, q, mp))) {
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

/*
 * STREAMS OPEN AND CLOSE ROUTINES
 */

/**
 * sc_qopen: - STREAMS open routine
 * @q: read queue of opened (pushed) queue pair
 * @devp: pointer to device number of driver
 * @oflags: open flags
 * @sflag: STREAMS flag
 * @crp: credentials of opening (pushing) process
 */
static streamscall int
sc_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
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
	write_lock(&sc_lock);
	if ((err = mi_open_comm(&sc_opens, sizeof(*priv), q, devp, oflags, sflag, crp))) {
		write_unlock(&sc_lock);
		freeb(rp);
		return (err);
	}
	priv = (struct priv *) (q->q_ptr);
	bzero(priv, sizeof(*priv));
	priv->rq = RD(q);
	priv->wq = WR(q);
	priv->np.dl = &priv->dl;
	priv->np.iq = WR(q);
	priv->np.oq = RD(q);
	priv->np.state.state = NS_UNBND;
	priv->np.oldstate.state = NS_UNBND;
	priv->np.info.PRIM_type = N_INFO_ACK;

	priv->dl.np = &priv->np;
	priv->dl.iq = RD(q);
	priv->dl.oq = WR(q);
	priv->dl.state.state = DL_UNATTACHED;
	priv->dl.oldstate.state = DL_UNATTACHED;
	priv->dl.info.dl_primitive = DL_INFO_ACK;

	write_unlock(&sc_lock);
	r = (typeof(r)) rp->b_wptr;
	r->dl_primitive = DL_INFO_REQ;
	rp->b_wptr += sizeof(*r);
	qprocson(q);
	putnext(WR(q), rp);
	return (0);
}

static streamscall int
sc_qclose(queue_t *q, int oflags, cred_t *crp)
{
	qprocsoff(q);
	write_lock(&sc_lock);
	mi_close_comm(&sc_opens, q);
	write_unlock(&sc_lock);
	return (0);
}

static modID_t modid = MOD_ID;

#ifdef LINUX
#ifdef module_param
module_param(modid, ushort, 0444);
#else				/* module_param */
MODULE_PARM(modid, "h");
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for SSCOP module. (0 for allocation.)");
#endif				/* LINUX */

static struct module_info sc_minfo = {
	.mi_idnum = MOD_ID,
	.mi_idname = MOD_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat sc_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat sc_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static struct qinit sc_rdinit = {
	.qi_putp = dl_rput,
	.qi_srvp = dl_rsrv,
	.qi_qopen = sc_qopen,
	.qi_qclose = sc_qclose,
	.qi_minfo = &sc_minfo,
	.qi_mstat = &sc_rstat,
};

static struct qinit sc_wrinit = {
	.qi_putp = np_wput,
	.qi_srvp = np_wsrv,
	.qi_minfo = &sc_minfo,
	.qi_mstat = &sc_wstat,
};

struct streamtab sscopinfo = {
	.st_rdinit = &sc_rdinit,
	.st_wrinit = &sc_wrinit,
};

static struct fmodsw sc_fmod = {
	.f_name = MOD_NAME,
	.f_str = &sscopinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

static int __init
sscopinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);
	if ((err = register_strmod(&sc_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module %d, err = %d\n", MOD_NAME,
			(int) modid, -err);
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

static int
sscopterminate(void)
{
	int err;

	if ((err = unregister_strmod(&sc_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister module %d, err = %d\n", MOD_NAME,
			(int) modid, -err);
		return (err);
	}
	modid = 0;
	return (0);
}

static void __exit
sscopexit(void)
{
	sscopterminate();
}

module_init(sscopinit);
module_exit(sscopexit);
