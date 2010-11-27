/*****************************************************************************

 @(#) $RCSfile: sccp_npi.c,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:40:33 $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2010  Monavacon Limited <http://www.monavacon.com/>
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

 Last Modified $Date: 2009-06-21 11:40:33 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sccp_npi.c,v $
 Revision 1.1.2.1  2009-06-21 11:40:33  brian
 - added files to new distro

 *****************************************************************************/

static char const ident[] = "$RCSfile: sccp_npi.c,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:40:33 $";

/*
 * This is a pushable STREAMS module that converts between the SCCPI (Signalling Connection Control
 * Part Interface) and NPI (Network Provider Interface).  SCCPI is, in fact, an extension of the
 * NPI interface: it provides extension primitives in support of ITU-T Recommendation Q.711
 * primitives that are not provided in ITU-T Recommendation X.213 and thus not reflected in the NPI
 * Revision 2.0.0 interface.  The NPI Revision 2.0.0 interface was largely developed in accordance
 * with X.213 in support of OSI CONS (X.25) and CLNS (CLNP).  This conversion module enhances the
 * NPI Revision 2.0.0 interface in a way that it can transparently support SCCP to NPI applications
 * and libraries written to support CONS (X.25) and CLNS (CLNP).
 */

#define _MPS_SOURCE
#define _SVR4_SOURCE

#include <sys/os7/compat.h>
#include <sys/strsun.h>

#include <sys/npi.h>
#include <sys/sccp_npi.h>
#include <ss7/sccpi.h>


#if 0
#include <sys/tihdr.h>
#include <sys/xti.h>
#include <sys/xti_ss7.h>
#include <sys/xti_sccp.h>
#endif

#define n_tst_bit(nr,addr)  test_bit(nr,addr)
#define n_set_bit(nr,addr)  __set_bit(nr,addr)
#define n_clr_bit(nr,addr) __clear_bit(nr,addr)

#define SCCP_NPI_DESCRIP	"SCCPI to NPI CONVERSION MODULE FOR LINUX FAST-STREAMS"
#define SCCP_NPI_EXTRA		"Part of the OpenSS7 SS7 Stack for Linux Fast-STREAMS"
#define SCCP_NPI_COPYRIGHT	"Copyright (c) 2008-2010  Monavacon Limited.  All Rights Reserved."
#define SCCP_NPI_REVISION	"OpenSS7 $RCSfile: sccp_npi.c,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:40:33 $"
#define SCCP_NPI_DEVICE		"SVR 4.2MP SCCPI to NPI Conversion Module (NPI) for SCCP"
#define SCCP_NPI_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SCCP_NPI_LICENSE	"GPL"
#define SCCP_NPI_BANNER		SCCP_NPI_DESCRIP	"\n" \
				SCCP_NPI_EXTRA		"\n" \
				SCCP_NPI_COPYRIGHT	"\n" \
				SCCP_NPI_REVISION	"\n" \
				SCCP_NPI_DEVICE		"\n" \
				SCCP_NPI_CONTACT	"\n"
#define SCCP_NPI_SPLASH		SCCP_NPI_DESCRIP	" - " \
				SCCP_NPI_REVISION

#ifndef CONFIG_STREAMS_SCCP_NPI_NAME
#error CONFIG_STREAMS_SCCP_NPI_NAME must be defined.
#endif
#ifndef CONFIG_STREAMS_SCCP_NPI_MODID
#error CONFIG_STREAMS_SCCP_NPI_MODID must be defined.
#endif

#ifdef LINUX
#ifdef MODULE
MODULE_AUTHOR(SCCP_NPI_CONTACT);
MODULE_DESCRIPTION(SCCP_NPI_DESCRIP);
MODULE_SUPPORTED_DEVICE(SCCP_NPI_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(SCCP_NPI_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-sccp-npi");
MODULE_ALIAS("streams-module-sccp-npi");
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_SCCP_NPI_MODID));
#endif				/* MODULE_ALIAS */
#ifdef MODULE_VERSION
MODULE_VERSION(__stringify(PACKAGE_RPMEPOCH) ":" PACKAGE_VERSION "." PACKAGE_RELEASE
	       PACKAGE_PATCHLEVEL "-" PACKAGE_RPMRELEASE PACKAGE_RPMEXTRA2);
#endif				/* MODULE_VERSION */
#endif				/* MODULE */
#endif				/* LINUX */

#define SCCP_NPI_MOD_ID		CONFIG_STREAMS_SCCP_NPI_MODID
#define SCCP_NPI_MOD_NAME	CONFIG_STREAMS_SCCP_NPI_NAME

/*
 * STREAMS DEFINITIONS
 */

#define MOD_ID		SCCP_NPI_MOD_ID
#define MOD_NAME	SCCP_NPI_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	SCCP_NPI_BANNER
#else				/* MODULE */
#define MOD_BANNER	SCCP_NPI_SPLASH
#endif				/* MODULE */

#define STRLOGNO	0
#define STRLOGST	1
#define STRLOGTO	2
#define STRLOGRX	3
#define STRLOGTX	4
#define STRLOGTE	5
#define STRLOGDA	6

/*
 * OPTIONS HANDLING
 */

/*
 * PRIVATE STRUCTURE
 */

struct np;
struct sc;

/* Upper interface structure */
struct np {
	struct sc *sc;
	queue_t *oq;
	np_ulong CONIND_number;
	struct {
		np_ulong state;
		np_ulong datinds;
		np_ulong datacks;
		np_ulong edatack;
		np_ulong coninds;
		np_ulong infinds;
		np_ulong ACCEPTOR_id;
		mblk_t *pending;
		np_ulong resinds;
		np_ulong resacks;
	} state, oldstate;
	N_info_ack_t INFO_ack;
	np_ulong INFORM_reason;
	np_ulong RESET_reason;
	np_ulong DISCON_reason;
	np_ulong TOKEN_value;
	np_ulong BIND_flags;
	np_ulong SRC_lref;
	np_ulong QOS_length;
	N_qos_sccp_t qos;
	np_ulong QOS_range_length;
	N_qos_sccp_t qor;
	struct {
		np_ulong ADDR_length;
		struct sccp_addr add;
		unsigned char add_addr[SCCP_MAX_ADDR_LENGTH];
	} __attribute((__packed__));
	struct {
		np_ulong LOCADDR_length;
		struct sccp_addr loc;
		unsigned char loc_addr[SCCP_MAX_ADDR_LENGTH];
	} __attribute((__packed__));
	struct {
		np_ulong REMADDR_length;
		struct sccp_addr rem;
		unsigned char rem_addr[SCCP_MAX_ADDR_LENGTH];
	} __attribute((__packed__));
	struct {
		np_ulong RES_length;
		struct sccp_addr res;
		unsigned char res_addr[SCCP_MAX_ADDR_LENGTH];
	} __attribute((__packed__));
	np_ulong PROTOID_length;
	np_ulong pro[4];
};

/* Lower interface structure */
struct sc {
	struct np *np;
	queue_t *oq;
	np_ulong CONIND_number;
	struct {
		np_ulong state;
		np_ulong datinds;
		np_ulong datacks;
		np_ulong edatack;
		np_ulong coninds;
		np_ulong infinds;
		np_ulong ACCEPTOR_id;
		mblk_t *pending;
		np_ulong resinds;
		np_ulong resacks;
	} state, oldstate;
	N_info_ack_t INFO_ack;
	np_ulong SRC_lref;
	np_ulong TOKEN_value;
	np_ulong BIND_flags;
	np_ulong QOS_length;
	N_qos_sccp_t qos;
	np_ulong QOS_range_length;
	N_qos_sccp_t qor;
	struct {
		np_ulong ADDR_length;
		struct sccp_addr add;
		unsigned char add_addr[SCCP_MAX_ADDR_LENGTH];
	} __attribute((__packed__));
	struct {
		np_ulong LOCADDR_length;
		struct sccp_addr loc;
		unsigned char loc_addr[SCCP_MAX_ADDR_LENGTH];
	} __attribute((__packed__));
	struct {
		np_ulong REMADDR_length;
		struct sccp_addr rem;
		unsigned char rem_addr[SCCP_MAX_ADDR_LENGTH];
	} __attribute((__packed__));
	struct {
		np_ulong RES_length;
		struct sccp_addr res;
		unsigned char res_addr[SCCP_MAX_ADDR_LENGTH];
	} __attribute((__packed__));
	np_ulong PROTOID_length;
	np_ulong pro[4];
};

struct priv {
	struct np np;
	struct sc sc;
};

#define PRIV(q) ((struct priv *)q->q_ptr)
#define NP_PRIV(q) (&PRIV(q)->np)
#define SC_PRIV(q) (&PRIV(q)->sc)

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
#ifdef N_EXT_BIND_REQ
	case N_EXT_BIND_REQ:
		return ("N_EXT_BIND_REQ");
#endif
#ifdef N_EXT2_BIND_REQ
	case N_EXT2_BIND_REQ:
		return ("N_EXT2_BIND_REQ");
#endif
#ifdef N_DRAIN_REQ
	case N_DRAIN_REQ:
		return ("N_DRAIN_REQ");
#endif
#ifdef N_FLOW_REQ
	case N_FLOW_REQ:
		return ("N_FLOW_REQ");
#endif
	default:
		return ("(unknown)");
	}
}
static const char *
sc_primname(np_ulong prim)
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
	case N_RESET_REQ:
		return ("N_RESET_REQ");
	case N_RESET_IND:
		return ("N_RESET_IND");
	case N_RESET_RES:
		return ("N_RESET_RES");
	case N_RESET_CON:
		return ("N_RESET_CON");
	case N_INFORM_REQ:
		return ("N_INFORM_REQ");
	case N_INFORM_IND:
		return ("N_INFORM_IND");
	case N_NOTICE_IND:
		return ("N_NOTICE_IND");
	case N_STATE_REQ:
		return ("N_STATE_REQ");
	case N_STATE_IND:
		return ("N_STATE_IND");
	case N_PCSTATE_IND:
		return ("N_PCSTATE_IND");
	case N_TRAFFIC_IND:
		return ("N_TRAFFIC_IND");
	case N_COORD_REQ:
		return ("N_COORD_REQ");
	case N_COORD_IND:
		return ("N_COORD_IND");
	case N_COORD_RES:
		return ("N_COORD_RES");
	case N_COORD_CON:
		return ("N_COORD_CON");
	case N_REQUEST_REQ:
		return ("N_REQUEST_REQ");
	case N_REPLY_ACK:
		return ("N_REPLY_ACK");
	default:
		return ("(unknown)");
	}
}
static inline const char *
np_statename(np_ulong state)
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
static const char *
sc_statename(np_ulong state)
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
static const char *
np_errname(np_long error)
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
static const char *
sc_errname(np_long error)
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

static const char *
np_strerror(np_long error)
{
	if (error < 0)
		error = NSYSERR;
	switch (error) {
	case NBADADDR:
		return ("Incorrect address format/illegal address information.");
	case NBADOPT:
		return
		    ("Options in incorrect format or contain illegal information.");
	case NACCESS:
		return ("User did not have proper permissions.");
	case NNOADDR:
		return ("NS Provider could not allocate address.");
	case NOUTSTATE:
		return ("Primitive was issues in wrong sequence.");
	case NBADSEQ:
		return ("Sequence number in primitive was incorrect/illegal.");
	case NSYSERR:
		return ("UNIX system error occurred.");
	case NBADDATA:
		return ("User data spec. outside range supported by NS provider.");
	case NBADFLAG:
		return ("Flags specified in primitive were illegal/incorrect.");
	case NNOTSUPPORT:
		return ("Primitive type not supported by the NS provider.");
	case NBOUND:
		return ("Illegal second attempt to bind listener or default listener.");
	case NBADQOSPARAM:
		return ("QOS values specified are outside the range supported by the NS provider.");
	case NBADQOSTYPE:
		return ("QOS structure type specified is not supported by the NS provider.");
	case NBADTOKEN:
		return ("Token used is not associated with an open stream.");
	case NNOPROTOID:
		return ("Protocol id could not be allocated.");
	default:
		return ("(unknown)");
	}
}
static inline const char *
sc_strerror(np_long error)
{
	if (error < 0)
		error = NSYSERR;
	switch (error) {
	case NBADADDR:
		return ("Incorrect address format/illegal address information.");
	case NBADOPT:
		return ("Options in incorrect format or contain illegal information.");
	case NACCESS:
		return ("User did no have proper permissions.");
	case NNOADDR:
		return ("NS Provider could not allocate address.");
	case NOUTSTATE:
		return ("Primitive was issued in wrong sequence.");
	case NBADSEQ:
		return ("Sequence number in primitive was incorrect/illegal.");
	case NSYSERR:
		return ("UNIX system error occured.");
	case NBADDATA:
		return ("User data spec. outside rnage supported by NS provider.");
	case NBADFLAG:
		return ("Flags specified in primitive were illegal/incorrect.");
	case NNOTSUPPORT:
		return ("Primitive type not supported by the NS provider.");
	case NBOUND:
		return ("Illegal second attempt to bind listenter or default listener.");
	case NBADQOSPARAM:
		return ("QOS values outside range supported by NS provider.");
	case NBADQOSTYPE:
		return ("QOS structure type not supported by NS provider.");
	case NBADTOKEN:
		return ("Token used is not associated with an open stream.");
	case NNOPROTOID:
		return ("Protocol id could not be allocated.");
	default:
		return ("(unknown)");
	}
}

static np_ulong
np_get_state(struct np *np)
{
	return (np->state.state);
}

static np_ulong
np_set_state(struct np *np, np_ulong newstate)
{
	np_ulong oldstate = np->state.state;

	if (newstate != oldstate) {
		np->state.state = newstate;
		np->INFO_ack.CURRENT_state = newstate;
		mi_strlog(np->oq, STRLOGST, SL_TRACE, "%s <- %s", np_statename(newstate),
			  np_statename(oldstate));
	}
	return (newstate);
}

static np_ulong
np_save_state(struct np *np)
{
	np->oldstate.state = np->state.state;
	return ((np->oldstate.state = np_get_state(np)));
}

static np_ulong
np_restore_state(struct np *np)
{
	return (np_set_state(np, np->oldstate.state));
}

static np_ulong
sc_get_state(struct sc *sc)
{
	return (sc->state.state);
}

static np_ulong
sc_set_state(struct sc *sc, np_ulong newstate)
{
	np_ulong oldstate = sc->state.state;

	if (newstate != oldstate) {
		sc->state.state = newstate;
		sc->INFO_ack.CURRENT_state = newstate;
		mi_strlog(sc->oq, STRLOGST, SL_TRACE, "%s <- %s", sc_statename(newstate),
			  sc_statename(oldstate));
	}
	return (newstate);
}

static np_ulong
sc_save_state(struct sc *sc)
{
	return ((sc->oldstate.state = sc_get_state(sc)));
}

static np_ulong
sc_restore_state(struct sc *sc)
{
	return (sc_set_state(sc, sc->oldstate.state));
}

/**
 * stripmsg: - strip leading blocks of a message
 * @mp: message to strip blocks from
 * @dp: message block to stop at
 *
 * Returns true if @dp was found in the message and false otherwise.  When @dp is null this function
 * has the same effect as freemsg(9) and returns true.
 */
static inline fastcall int
stripmsg(mblk_t *mp, mblk_t *dp)
{
	mblk_t *b, *b_next = mp;

	while ((b = b_next) && b != dp) {
		b_next = b->b_next;
		freeb(b);
	}
	return (b == dp);
}

/**
 * np_txprim: - trace log sent primitive
 * @np: private structure (locked)
 * @msg: message to strip upon success
 * @mp: the TPI primitive being sent
 * @prim: primitive being transmitted
 */
static inline fastcall int
np_txprim(struct np *np, mblk_t *msg, mblk_t *mp, const np_ulong prim)
{
	int priority, rtn;

	switch (prim) {
	case T_DATA_IND:
	case T_EXDATA_IND:
	case T_OPTDATA_IND:
	case T_UDERROR_IND:
	case T_UNITDATA_IND:
		priority = STRLOGDA;
		break;
	default:
		priority = STRLOGRX;
		break;
	}
	rtn = mi_strlog(np->oq, priority, SL_TRACE, "<- %s", np_primname(prim));
	stripmsg(msg, mp->b_cont);
	putnext(np->oq, mp);
	return (rtn);
}

/**
 * sc_txprim: - trace log sent primitive
 * @sc: private structure (locked)
 * @msg: message to strip upon success
 * @mp: the NPI primitive being sent
 * @prim: primitive being transmitted
 */
static int
sc_txprim(struct sc *sc, mblk_t *msg, mblk_t *mp, const np_ulong prim)
{
	np_long prim = *(np_long *) mp->b_rptr;
	int priority, rtn;

	switch (prim) {
	case N_DATA_REQ:
	case N_EXDATA_REQ:
	case N_DATACK_REQ:
	case N_UNITDATA_REQ:
		priority = STRLOGDA;
		break;
	default:
		priority = STRLOGRX;
		break;
	}
	rtn = mi_strlog(sc->oq, priority, SL_TRACE, "%s ->", sc_primname(prim));
	stripmsg(msg, mp->b_cont);
	putnext(sc->oq, mp);
	return (rtn);
}

/**
 * np_error: - trace log error condition
 * @np: private structure (locked)
 * @error: TPI error type
 * @func: function in which error was detected
 */
static int
np_error(struct np *np, t_scalar_t error, const char *func)
{
	return mi_strlog(np->oq, STRLOGNO, SL_TRACE, "ERROR: %s: %s: %s", func,
			 np_errname(error), np_strerror(error));
}

/**
 * sc_error: - trace log error condition
 * @sc: private structure (locked)
 * @error: NPI error type
 * @func: function in which error was detected
 */
static int
sc_error(struct sc *sc, np_long error, const char *func)
{
	return mi_strlog(sc->oq, STRLOGNO, SL_TRACE, "ERROR: %s: %s: %s", func,
			 sc_errname(error), sc_strerror(error));
}

/**
 * sc_n_prim: - translate NPI primitive to TPI primitive
 * @prim: the primitive type to translate
 */
static t_scalar_t
sc_n_prim(np_ulong prim)
{
	switch (prim) {
	case N_CONN_REQ:
		return (N_CONN_REQ);
	case N_CONN_RES:
		return (N_CONN_RES);
	case N_DISCON_REQ:
		return (N_DISCON_REQ);
	case N_DATA_REQ:
		return (N_DATA_REQ);
	case N_EXDATA_REQ:
		return (N_EXDATA_REQ);
	case N_INFO_REQ:
		return (N_INFO_REQ);
	case N_BIND_REQ:
		return (N_BIND_REQ);
	case N_UNBIND_REQ:
		return (N_UNBIND_REQ);
	case N_UNITDATA_REQ:
		return (N_UNITDATA_REQ);
	case N_OPTMGMT_REQ:
		return (N_OPTMGMT_REQ);
	case N_CONN_IND:
		return (N_CONN_IND);
	case N_CONN_CON:
		return (N_CONN_CON);
	case N_DISCON_IND:
		return (N_DISCON_IND);
	case N_DATA_IND:
		return (N_DATA_IND);
	case N_EXDATA_IND:
		return (N_EXDATA_IND);
	case N_INFO_ACK:
		return (N_INFO_ACK);
	case N_BIND_ACK:
		return (N_BIND_ACK);
	case N_ERROR_ACK:
		return (N_ERROR_ACK);
	case N_OK_ACK:
		return (N_OK_ACK);
	case N_UNITDATA_IND:
		return (N_UNITDATA_IND);
	case N_UDERROR_IND:
		return (N_UDERROR_IND);
	case N_DATACK_REQ:
		return (N_DATACK_REQ);
	case N_DATACK_IND:
		return (N_DATACK_IND);
	case N_RESET_REQ:
		return (N_RESET_REQ);
	case N_RESET_IND:
		return (N_RESET_IND);
	case N_RESET_RES:
		return (N_RESET_RES);
	case N_RESET_CON:
		return (N_RESET_CON);
	default:
		return (-1);
	}
}

noinline fastcall np_ulong
sc_n_serv_type(np_ulong type)
{
	np_ulong np_type = 0;

	if (type & N_CONS)
		np_type |= N_COTS;
	if (type & N_CLNS)
		np_type |= N_CLTS;
	return (np_type);
}

noinline fastcall np_ulong
sc_n_state(np_ulong state)
{
	switch (state) {
	case NS_UNBND:
		return (NS_UNBND);
	case NS_WACK_BREQ:
		return (NS_WACK_BREQ);
	case NS_WACK_UREQ:
		return (NS_WACK_UREQ);
	case NS_IDLE:
		return (NS_IDLE);
	case NS_WACK_OPTREQ:
		return (NS_WACK_OPTREQ);
	case NS_WACK_RRES:
		return (NS_WACK_RRES);
	case NS_WCON_CREQ:
		return (NS_WCON_CREQ);
	case NS_WRES_CIND:
		return (NS_WRES_CIND);
	case NS_WACK_CRES:
		return (NS_WACK_CREQ);
	case NS_DATA_XFER:
		return (NS_DATA_XFER);
	case NS_WCON_RREQ:
		return (NS_WCON_RREQ);
	case NS_WRES_RIND:
		return (NS_WRES_RIND);
	case NS_WACK_DREQ6:
		return (NS_WACK_DREQ6);
	case NS_WACK_DREQ7:
		return (NS_WACK_DREQ7);
	case NS_WACK_DREQ9:
		return (NS_WACK_DREQ9);
	case NS_WACK_DREQ10:
		return (NS_WACK_DREQ10);
	case NS_WACK_DREQ11:
		return (NS_WACK_DREQ11);
	default:
		return (-1);
	}
}

/**
 * sc_n_error: - translate NPI error to TPI error
 * @error: error type to translate
 */
static np_ulong
sc_n_error(np_ulong error, np_ulong errno)
{
	switch (error) {
	case NBADADDR:
		return (NBADADDR);
	case NBADOPT:
		return (NBADOPT);
	case NACCESS:
		return (NACCES);
	case NNOADDR:
		return (NNOADDR);
	case NOUTSTATE:
		return (NOUTSTATE);
	case NBADSEQ:
		return (NBADSEQ);
	case NSYSERR:
		return (-errno);
	case NBADDATA:
		return (NBADDATA);
	case NBADFLAG:
		return (NBADFLAG);
	case NNOTSUPPORT:
		return (NNOTSUPPORT);
	case NBOUND:
		return (NBOUND);
	case NBADQOSPARAM:
		return (NBADQOSPARAM);
	case NBADQOSTYPE:
		return (NBADQOSTYPE);
	case NBADTOKEN:
		return (NBADTOKEN);
	case NNOPROTOID:
		return (NNOPROTOID);
	default:
		return (error);
	}
}

/**
 * np_s_address: - convert a NPI address to an SCCP address
 * @n_add: pointer to NPI address
 * @s_add: pointer to SCCP address
 */
static void
np_s_address(struct sccp_addr *n_add, struct sccp_addr *s_add, np_ulong *lenp)
{
	if (n_add && s_add) {
		s_add->ni = n_add->ni;
		s_add->ri = n_add->ri;
		s_add->pc = n_add->pc;
		s_add->ssn = n_add->ssn;
		s_add->gtt = n_add->gtt;
		s_add->nai = n_add->nai;
		s_add->es = n_add->es;
		s_add->nplan = n_add->nplan;
		s_add->tt = n_add->tt;
		s_add->alen = n_add->alen;
		memcpy(s_add->addr, n_add->addr, s_add->alen);
		if (lenp)
			*lenp = sizeof(s_add) + s_add->alen;
	}
	if (lenp)
		*lenp = 0;
}

/**
 * sc_n_address: - convert an NPI SCCP address to a TPI SCCP address
 * @s_add: pointer to NPI address (or NULL)
 * @n_add: pointer to NPI address (not NULL when s_add not NULL)
 */
static void
sc_n_address(struct sccp_addr *s_add, struct sccp_addr *n_add, t_uscalar_t *lenp)
{
	if (s_add && n_add) {

		n_add->ni = s_add->ni;
		n_add->ri = s_add->ri;
		n_add->pc = s_add->pc;
		n_add->ssn = s_add->ssn;
		n_add->gtt = s_add->gtt;
		n_add->nai = s_add->nai;
		n_add->es = s_add->es;
		n_add->nplan = s_add->nplan;
		n_add->tt = s_add->tt;
		n_add->alen = s_add->alen;
		memcpy(n_add->addr, s_add->addr, n_add->alen);

		if (lenp)
			*lenp = sizeof(n_add) + n_add->alen;
	}
	if (lenp)
		*lenp = 0;
}

/**
 * sc_n_options: - convert an NPI SCCP QOS structure to NPI options
 * @qos: pointer to NPI SCCP QOS structure
 * @opt: pointer to NPI SCCP options structure
 */
static void
sc_n_options(N_qos_sccp_t * qos, struct N_qos_sccp_t *opt)
{
	if (qos == NULL)
		return;
	switch (qos->n_qos_type) {
	case N_QOS_SEL_DATA_SCCP:
		/* FIXME */
		break;
	case N_QOS_SEL_CONN_SCCP:
		/* FIXME */
		break;
	case N_QOS_SEL_INFO_SCCP:
		/* FIXME */
		break;
	case N_QOS_RANGE_INFO_SCCP:
		/* FIXME */
		break;
	case N_QOS_SEL_INFR_SCCP:
		/* FIXME */
		break;
	default:
		break;
	}
	return;
}

/*
 * NPI OPTION HANDLING
 */



static inline fastcall size_t
np_size_qos(struct np *np, N_qos_sccp_t *qos, const np_ulong prim)
{
	size_t size = 0;

	switch (prim) {
	case N_CONN_IND:
		size += sizeof(N_qos_sel_conn_sccp_t);
		break;
	case N_CONN_CON:
		size += sizeof(N_qos_sel_conn_sccp_t);
		break;
	case N_UNITDATA_IND:
		size += sizeof(N_qos_sel_data_sccp_t);
		break;
	default:
		break;
	}
	return (size);
}







/*
 * ---------------------------------------------------
 * ---------------------------------------------------
 * ---------------------------------------------------
 */

/*
 * NPI PROVIDER TO NPI USER ISSUED PRIMITIVES
 */

/**
 * n_conn_ind: - issue an N_CONN_IND primitive upstream
 * @np: network provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @loc: destination address
 * @rem: source address
 * @qos: quality of service parameters
 * @seq: connection indication sequence number
 * @flags: connect flags
 * @dp: user data
 */
noinline fastcall int
n_conn_ind(struct np *np, queue_t *q, mblk_t *msg, struct sccp_addr *loc, struct sccp_addr *rem,
	   N_qos_sel_conn_sccp_t *qos, np_ulong seq, np_ulong flags, mblk_t *dp)
{
	N_conn_ind_t *p;
	mblk_t *mp;
	const size_t olen = np_size_qos(np, (N_qos_sccp_t *) qos, N_CONN_IND);
	const size_t mlen = sizeof(*p) + sizeof(np->rem) + SCCP_MAX_ADDR_LENGTH + sizeof(np->loc) + olen;

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(np->oq, 0)))
		goto ebusy;
	sc_n_address(rem, &np->rem, &np->REMADDR_length);
	sc_n_address(loc, &np->loc, &np->LOCADDR_length);
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_CONN_IND;
	p->SRC_length = np->REMADDR_length;
	p->SRC_offset = np->REMADDR_length ? sizeof(*p) : 0;
	p->QOS_length = olen;
	p->QOS_offset = olen ? p->SRC_offset + p->SRC_length : 0;
	p->SEQ_number = seq;
	p->DEST_length = np->LOCADDR_length;
	p->DEST_offset = np->LOCADDR_length ? p->QOS_offset + p->QOS_length: 0;
	p->CONN_flags = flags;
	mp->b_wptr += sizeof(*p);
	bcopy(&np->rem, mp->b_wptr, p->SRC_length);
	mp->b_wptr += p->SRC_length;
	np_build_qos(np, (N_qos_sccp_t *) qos, mp->b_wptr, olen, N_CONN_IND);
	mp->b_wptr += olen;
	bcopy(&np->loc, mp->b_wptr, p->DEST_length);
	mp->b_wptr += p->DEST_length;
	mp->b_cont = dp;
	np->state.coninds++;
	np_set_state(np, NS_WRES_CIND);
	np_txprim(np, msg, mp, N_CONN_IND);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_conn_con: - issue an N_CONN_CON primitive upstream
 * @np: network provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @res: responding address
 * @qos: quality of service parameters
 * @flags: connection flags
 * @dp: user data
 */
noinline fastcall int
n_conn_con(struct np *np, queue_t *q, mblk_t *msg, struct sccp_addr *res, N_qos_sel_conn_sccp_t *qos, np_ulong flags,
	   mblk_t *dp)
{
	N_conn_con_t *p;
	mblk_t *mp;
	const size_t olen = np_size_qos(np, (N_qos_sccp_t *) qos, N_CONN_CON);
	const size_t mlen = sizeof(*p) + sizeof(np->res) + SCCP_MAX_ADDR_LENGTH + olen;

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	sc_n_address(res, &np->res, &np->RES_length);
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_CONN_CON;
	p->RES_length = np->RES_length;
	p->RES_offset = np->RES_length ? sizeof(*p) : 0;
	p->QOS_length = olen;
	p->QOS_offset = olen ? p->RES_offset + p->RES_length : 0;
	p->CONN_flags = flags;
	mp->b_wptr += sizeof(*p);
	bcopy(&np->res, mp->b_wptr, p->RES_length);
	mp->b_wptr += p->RES_length;
	np_build_qos(np, (N_qos_sccp_t *) qos, mp->b_wptr, olen);
	mp->b_wptr += olen;
	mp->b_cont = dp;
	np_set_state(np, NS_DATA_XFER);
	np_txprim(tp, msg, mp, N_CONN_CON);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_discon_ind: - issue an N_DISCON_IND primitive upstream
 * @np: network provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @res: responding address
 * @orig: disconnect originator
 * @reason: disconnect reason
 * @seq: connection indication sequence number (or zero)
 * @dp: user data
 */
noinline fastcall int
n_discon_ind(struct np *np, queue_t *q, mblk_t *msg, struct sccp_addr *res, np_ulong orig, np_ulong reason, np_ulong seq,
	     mblk_t *dp)
{
	N_discon_ind_t *p;
	mblk_t *mp;
	static const const size_t mlen = sizeof(*p) + sizeof(np->res);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	sc_n_address(res, &np->res, &np->RES_length);
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_DISCON_IND;
	p->DISCON_orig = orig;
	p->DISCON_reason = reason;
	p->RES_length = np->RES_length;
	p->RES_offset = np->RES_length ? sizeof(*p) : 0;
	p->SEQ_number = seq;
	mp->b_wptr += sizeof(*p);
	bcopy(&np->res, mp->b_wptr, p->RES_length);
	mp->b_wptr += p->RES_length;
	mp->b_cont = dp;
	if (seq)
		np->state.coninds--;
	np_set_state(np, np->state.coninds ? NS_WRES_CIND : NS_IDLE);
	np_txprim(np, msg, mp, N_DISCON_IND);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_data_ind: - issue an N_DATA_IND primitive upstream
 * @np: network provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @flags: data transfer flags
 * @dp: user data
 */
noinline fastcall int
n_data_ind(struct np *np, queue_t *q, mblk_t *msg, np_ulong flags, mblk_t *dp)
{
	N_data_ind_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(np->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_DATA_IND;
	p->DATA_xfer_flags = flags;
	mp->b_wptr += sizeof(*p);
	mp->b_cont = dp;
	if (flags & N_RC_FLAG)
		np->state.datinds++;
	np_txprim(np, msg, mp, N_DATA_IND);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_exdata_ind: - issue an N_EXDATA_IND primitive upstream
 * @np: network provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @dp: user data
 */
noinline fastcall int
n_exdata_ind(struct np *np, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	N_exdata_ind_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(np->oq, 1)))
		goto ebusy;
	mp->b_band = 1;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_EXDATA_IND;
	mp->b_wptr += sizeof(*p);
	mp->b_cont = dp;
	np_txprim(np, msg, mp, N_EXDATA_IND);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_info_ack: - issue an N_INFO_ACK primitive upstream
 * @np: network provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
noinline fastcall int
n_info_ack(struct np *np, queue_t *q, mblk_t *msg)
{
	N_info_ack_t *p;
	mblk_t *mp;
	static const size_t mlen =
	    sizeof(*p) + sizeof(np->loc) + sizeof(np->qos) + sizeof(np->qor) + sizeof(np->pro);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	*p = np->INFO_ack;
	mp->b_wptr += sizeof(*p);
	p->ADDR_offset = MBLKL(mp);
	bcopy(&np->add, mp->b_wptr, p->ADDR_length);
	mp->b_wptr += p->ADDR_length;
	p->QOS_offset = MBLKL(mp);
	bcopy(&np->qos, mp->b_wptr, p->QOS_length);
	mp->b_wptr += p->QOS_length;
	p->QOS_range_offset = MBLKL(mp);
	bcopy(&np->qor, mp->b_wptr, p->QOS_range_length);
	mp->b_wptr += p->QOS_range_length;
	p->PROTOID_offset = MBLKL(mp);
	bcopy(&np->pro, mp->b_wptr, p->PROTOID_length);
	mp->b_wptr += p->PROTOID_length;
	np_txprim(np, msg, mp, N_INFO_ACK);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_bind_ack: - issue an N_BIND_ACK primitive upstream
 * @np: network provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @add: bound address (or NULL)
 * @coninds: maximum number of outstanding connection indications
 */
noinline fastcall int
n_bind_ack(struct np *np, queue_t *q, mblk_t *msg, struct sccp_addr *add, np_ulong coninds)
{
	N_bind_ack_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p) + sizeof(np->add) + SCCP_MAX_ADDR_LENGTH + sizeof(np->pro);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	sc_n_address(add, &np->add, &np->ADDR_length);
	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_BIND_ACK;
	p->ADDR_length = sizeof(np->add);
	p->ADDR_offset = sizeof(*p);
	p->CONIND_number = np->CONIND_number;
	p->PROTOID_length = np->INFO_ack.PROTOID_length;
	p->PROTOID_offset = p->ADDR_offset + p->ADDR_length;
	mp->b_wptr += sizeof(*p);
	bcopy(&np->add, mp->b_wptr, p->ADDR_length);
	mp->b_wptr += p->ADDR_length;
	bcopy(&np->pro, mp->b_wptr, p->PROTOID_length);
	mp->b_wptr += p->PROTOID_length;
	np_set_state(np, NS_IDLE);
	np_txprim(np, msg, mp, N_BIND_ACK);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_error_ack: - issue an N_ERROR_ACK primitive upstream
 * @np: network provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @prim: primitive in error
 * @error: negative UNIX error, positive NPI error
 */
noinline fastcall int
n_error_ack(struct np *np, queue_t *q, mblk_t *msg, np_ulong prim, np_ulong error)
{
	N_error_ack_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);
	np_ulong state;

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_ERROR_ACK;
	p->ERROR_prim = prim;
	p->NPI_error = error < 0 ? NSYSERR : error;
	p->UNIX_error = error < 0 ? -error : 0;
	mp->b_wptr += sizeof(*p);
	mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_ERROR_ACK");
	np_restore_state(np);
	switch ((state = np_get_state(np))) {
	case NS_WACK_BREQ:
		state = NS_UNBND;
		break;
	case NS_WACK_UREQ:
		state = NS_IDLE;
		break;
	case NS_WACK_OPTREQ:
		state = NS_IDLE;
		break;
	case NS_WCON_CREQ:
		state = NS_IDLE;
		break;
	case NS_WACK_CRES:
		state = NS_WRES_CIND;
		break;
	case NS_WCON_RREQ:
		state = NS_DATA_XFER;
		break;
	case NS_WACK_RRES:
		state = NS_WRES_RIND;
		break;
	case NS_WACK_DREQ6:
		state = NS_WCON_CREQ;
		break;
	case NS_WACK_DREQ7:
		state = NS_WRES_CIND;
		break;
	case NS_WACK_DREQ9:
		state = NS_DATA_XFER;
		break;
	case NS_WACK_DREQ10:
		state = NS_WCON_RREQ;
		break;
	case NS_WACK_DREQ11:
		state = NS_WRES_RIND;
		break;
	}
	np_set_state(np, state);
	np_txprim(np, msg, mp, N_ERROR_ACK);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

noinline fastcall int
m_error(struct np *np, queue_t *q, mblk_t *msg, unsigned char rerror, unsigned char werror)
{
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(rerror) + sizeof(werror), BPRI_HI))))
		goto enobufs;
	DB_TYPE(mp) = M_ERROR;
	mp->b_wptr[0] = rerror;
	mp->b_wptr[1] = werror;
	mp->b_wptr += 2;
	sc_set_state(np->sc, -1);
	np_set_state(np, -1);
	freemsg(msg);
	putnext(np->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_error_reply: - process error conditions out of line
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @prim: primitive (or -1)
 * @error: negative UNIX error or positive TLI error (or zero (0))
 * @func: where error was detected
 */
noinline fastcall int
n_error_reply(struct np *np, queue_t *q, mblk_t *msg, t_scalar_t prim, t_scalar_t error, const char *func)
{
	switch (error) {
	case -EAGAIN:
	case -ENOBUFS:
	case -ENOMEM:
	case -EDEADLK:
	case -EBUSY:
	case 0:
		return (error);
	}
	switch (prim) {
	case N_DATA_REQ:
	case N_EXDATA_REQ:
	case N_UNITDATA_REQ:
	case N_DATACK_REQ:
		/* If the primitive in error is one for which no T_ERROR_ACK is possible, then the
		   TPI user is given an M_ERROR message. */
		mi_strlog(q, STRLOGDA, SL_TRACE, "ERROR: %s: %s: %s", func, np_errname(error),
			  np_strerror(error));
		return m_error(np, q, msg, EPROTO, EPROTO);
	}
	switch (np_get_state(np)) {
	case NS_WACK_BREQ:
	case NS_WACK_UREQ:
	case NS_WACK_OPTREQ:
	case NS_WACK_RRES:
	case NS_WACK_CRES:
	case NS_WACK_DREQ6:
	case NS_WACK_DREQ7:
	case NS_WACK_DREQ9:
	case NS_WACK_DREQ10:
	case NS_WACK_DREQ11:
		/* If the interface is in the waiting for acknowledgement state then the TPI user
		   has violated the interface protocol and deserves an M_ERROR message. */
		mi_strlog(q, STRLOGNO, SL_TRACE | SL_ERROR, "ERROR: %s: %s: %s", func,
			  np_errname(error), np_strerror(error));
		return m_error(np, q, msg, EPROTO, EPROTO);
	}
	mi_strlog(q, STRLOGNO, SL_TRACE, "ERROR: %s: %s: %s", func, np_errname(error),
		  np_strerror(error));
	return t_error_ack(np, q, msg, prim, error);
}

/**
 * n_ok_ack: - issue an N_OK_ACK primitive upstream
 * @np: network provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @prim: correct prim
 */
noinline fastcall int
n_ok_ack(struct np *np, queue_t *q, mblk_t *msg, np_ulong prim)
{
	N_ok_ack_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);
	np_ulong state;

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_OK_ACK;
	p->CORRECT_prim = prim;
	mp->b_wptr += sizeof(*p);
	switch (np_get_state(np)) {
	case NS_WACK_BREQ:
	case NS_WACK_OPTREQ:
	case NS_WACK_DREQ6:
	case NS_WACK_DREQ7:
	case NS_WACK_DREQ9:
	case NS_WACK_DREQ10:
	case NS_WACK_DREQ11:
		state = NS_IDLE;
		break;
	case NS_WACK_UREQ:
		state = NS_UNBND;
		break;
	case NS_WACK_RRES:
		state = NS_DATA_XFER;
		break;
	case NS_WACK_CRES:
		np->state.coninds--;
		/* FIXME: when ACCEPTOR_id is non-zero, need to find the other Stream and set its state
		   to NS_DATA_XFER. */
		if (np->state.ACCEPTOR_id == 0)
			state = NS_DATA_XFER;
		else if (np->state.coninds > 0)
			state = NS_WRES_CIND;
		else
			state = NS_IDLE;
		break;
	default:
		state = np_get_state(np);
		break;
	}
	np_set_state(np, state);
	np_txprim(np, msg, mp, N_OK_ACK);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_unitdata_ind: - issue an N_UNITDATA_IND primitive upstream
 * @np: network provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @rem: source address
 * @loc: destination address
 * @qos: quality of service parameters
 * @error: error type
 * @dp: user data
 */
noinline fastcall int
n_unitdata_ind(struct np *np, queue_t *q, mblk_t *msg, struct sccp_addr *rem,
	       struct sccp_addr *loc, N_qos_sel_data_sccp_t *qos, np_ulong error,
	       mblk_t *dp)
{
	N_unitdata_ind_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p) + sizeof(*loc) + sizeof(*rem);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(np->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_UNITDATA_IND;
	p->SRC_length = loc ? sizeof(*loc) : 0;
	p->SRC_offset = loc ? sizeof(*p) : 0;
	p->DEST_length = rem ? sizeof(*rem) : 0;
	p->DEST_offset = rem ? p->SRC_offset + p->SRC_length : 0;
	p->ERROR_type = error;
	mp->b_wptr += sizeof(*p);
	bcopy(loc, mp->b_wptr, p->SRC_length);
	mp->b_wptr += p->SRC_length;
	bcopy(rem, mp->b_wptr, p->DEST_length);
	mp->b_wptr += p->DEST_length;
	mp->b_cont = dp;
	np_txprim(np, msg, mp, N_UNITDATA_IND);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_uderror_ind: - issue an N_UDERROR_IND primitive upstream
 * @np: network provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @rem: destination address (or NULL)
 * @qos: quality of service parameters
 * @error: error type
 * @dp: user data
 */
noinline fastcall int
n_uderror_ind(struct np *np, queue_t *q, mblk_t *msg, struct sccp_addr *rem,
	      N_qos_sel_data_sccp_t *qos, np_ulong error, mblk_t *dp)
{
	N_uderror_ind_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p) + sizeof(*rem);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(np->oq, 1)))
		goto ebusy;
	mp->b_band = 1;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_UDERROR_IND;
	p->DEST_length = rem ? sizeof(*rem) : 0;
	p->DEST_offset = rem ? sizeof(*p) : 0;
	p->RESERVED_field = 0;
	p->ERROR_type = error;
	mp->b_wptr += sizeof(*p);
	bcopy(rem, mp->b_wptr, p->DEST_length);
	mp->b_wptr += p->DEST_length;
	mp->b_cont = dp;
	np_txprim(np, msg, mp, N_UDERROR_IND);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_datack_ind: - issue an N_DATACK_IND primitive upstream
 * @np: network provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
noinline fastcall int
n_datack_ind(struct np *np, queue_t *q, mblk_t *msg)
{
	N_datack_ind_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(np->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_DATACK_IND;
	mp->b_wptr += sizeof(*p);
	np->state.datacks--;
	np_txprim(np, msg, mp, N_DATACK_IND);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_inform_ind: - issue an N_RESET_IND primitive upstream
 * @np: network provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @reason: inform reason
 */
noinline fastcall int
n_inform_ind(struct np *np, queue_t *q, mblk_t *msg, np_ulong orig, np_ulong reason)
{
	N_reset_ind_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(np->oq, 2)))
		goto ebusy;
	mp->b_band = 2;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_RESET_IND;
	p->RESET_orig = orig;
	p->RESET_reason = reason;
	mp->b_wptr += sizeof(*p);
	np_set_state(np, NS_WRES_RIND);
	np->state.resinds++;
	np_txprim(np, msg, mp, N_RESET_IND);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_reset_ind: - issue an N_RESET_IND primitive upstream
 * @np: network provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @orig: reset originator
 * @reason: reset reason
 */
noinline fastcall int
n_reset_ind(struct np *np, queue_t *q, mblk_t *msg, np_ulong orig, np_ulong reason)
{
	N_reset_ind_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_RESET_IND;
	p->RESET_orig = orig;
	p->RESET_reason = reason;
	mp->b_wptr += sizeof(*p);
	np_set_state(np, NS_WRES_RIND);
	np->state.datinds = 0;
	np->state.datacks = 0;
	np->state.resinds = 1;
	np_txprim(np, msg, mp, N_RESET_IND);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_reset_con: - issue an N_RESET_CON primitive upstream
 * @np: network provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
noinline fastcall int
n_reset_con(struct np *np, queue_t *q, mblk_t *msg)
{
	N_reset_con_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_RESET_CON;
	mp->b_wptr += sizeof(*p);
	np_set_state(np, NS_IDLE);
	np->state.resacks = 0;
	np->state.resinds = 0;
	np->state.datinds = 0;
	np->state.datacks = 0;
	np_txprim(np, msg, mp, N_RESET_CON);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/*
 * SCCPI USER TO SCCPI PROVIDER ISSUED PRIMITIVES
 */

/**
 * n_conn_req: - issue an N_CONN_REQ primitive downstream
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @msg: message to free upon error (or NULL)
 * @rem: destination address (not NULL)
 * @qos: quality of service parameters
 * @flags: connect flags
 * @dp: user data
 */
noinline fastcall int
n_conn_req(struct sc *sc, queue_t *q, mblk_t *msg, struct sccp_addr *rem,
	   N_qos_sel_conn_sccp_t *qos, np_ulong flags, mblk_t *dp)
{
	N_conn_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p) + sizeof(sc->rem) + SCCP_MAX_ADDR_LENGTH + sizeof(sc->qos);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(sc->oq, 0)))
		goto ebusy;
	np_s_address(rem, &sc->rem, &sc->REMADDR_length);
	np_s_options(qos, &sc->qos, &sc->QOS_length, &flags);
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_CONN_REQ;
	p->DEST_length = sc->REMADDR_length;
	p->DEST_offset = sc->REMADDR_length ? sizeof(*p) : 0;
	p->CONN_flags = flags;
	p->QOS_length = sc->QOS_length;
	p->QOS_offset = sc->QOS_length ? p->DEST_offset + p->DEST_length : 0;
	mp->b_wptr += sizeof(*p);
	bcopy(&sc->rem, mp->b_wptr, p->DEST_length);
	mp->b_wptr += p->DEST_length;
	bcopy(&sc->qos, mp->b_wptr, p->QOS_length);
	mp->b_wptr += p->QOS_length;
	mp->b_cont = dp;
	sc_set_state(sc, NS_WCON_CREQ);
	sc_txprim(sc, msg, mp, N_CONN_REQ);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_conn_res: - issue an N_CONN_RES primitive downstream
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @msg: message to free upon error (or NULL)
 * @res: responding address (or NULL)
 * @qos: quality of service parameters (or NULL)
 * @token: token of accepting stream or zero
 * @flags: connect flags
 * @seq: connection indication responded to
 */
noinline fastcall int
n_conn_res(struct sc *sc, queue_t *q, mblk_t *msg, struct sccp_addr *res,
	   N_qos_sel_conn_sccp_t *qos, np_ulong token, np_ulong flags, np_ulong seq, mblk_t *dp)
{
	N_conn_res_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p) + sizeof(sc->res) + sizeof(sc->qos);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(sc->oq, 0)))
		goto ebusy;
	np_s_address(res, &sc->res, &sc->RES_length);
	np_s_options(qos, &sc->qos, &sc->QOS_length, &flags);
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_CONN_RES;
	p->TOKEN_value = token;
	p->RES_length = sc->RES_length;
	p->RES_offset = sc->RES_length ? sizeof(*p) : 0;
	p->SEQ_number = seq;
	p->CONN_flags = flags;
	p->QOS_length = sc->QOS_length;
	p->QOS_offset = sc->QOS_length ? p->RES_offset + p->RES_length : 0;
	mp->b_wptr += sizeof(*p);
	bcopy(&sc->res, mp->b_wptr, p->RES_length);
	mp->b_wptr += p->RES_length;
	bcopy(&sc->qos, mp->b_wptr, p->QOS_length);
	mp->b_wptr += p->QOS_length;
	mp->b_cont = dp;
	sc_set_state(sc, NS_WACK_CRES);
	sc->state.coninds--;
	sc_txprim(sc, msg, mp, N_CONN_RES);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_discon_req: - issue an N_DISCON_REQ primitive downstream
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @msg: message to free upon error (or NULL)
 * @res: responding address (or NULL)
 * @reason: disconnect reason
 * @seq: connection indication sequence number (or zero)
 * @dp: user data
 */
noinline fastcall int
n_discon_req(struct sc *sc, queue_t *q, mblk_t *msg, struct sccp_addr *res, np_ulong reason,
	     np_ulong seq, mblk_t *dp)
{
	N_discon_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p) + sizeof(sc->res) + SCCP_MAX_ADDR_LENGTH;

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(sc->oq, 0)))
		goto ebusy;
	np_s_address(res, &sc->res, &sc->RES_length);
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_DISCON_REQ;
	p->DISCON_reason = reason;
	p->RES_length = sc->RES_length;
	p->RES_offset = sc->RES_length ? sizeof(*p) : 0;
	p->SEQ_number = seq;
	mp->b_wptr += sizeof(*p);
	bcopy(&sc->res, mp->b_wptr, p->RES_length);
	mp->b_wptr += p->RES_length;
	mp->b_cont = dp;
	switch (__builtin_expect(sc_get_state(sc), NS_DATA_XFER)) {
	case NS_DATA_XFER:
		sc_set_state(sc, NS_WACK_DREQ9);
		break;
	case NS_WCON_RREQ:
		sc_set_state(sc, NS_WACK_DREQ10);
		break;
	case NS_WRES_RIND:
		sc_set_state(sc, NS_WACK_DREQ11);
		break;
	case NS_WCON_CREQ:
		sc_set_state(sc, NS_WACK_DREQ6);
		break;
	case NS_WRES_CIND:
		sc_set_state(sc, NS_WACK_DREQ7);
		break;
	}
	sc_txprim(sc, msg, mp, N_DISCON_REQ);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_data_req: - issue an N_DATA_REQ primitive downstream
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @msg: message to free upon error (or NULL)
 * @flags: data transfer flags
 * @dp: user data
 */
noinline fastcall int
n_data_req(struct sc *sc, queue_t *q, mblk_t *msg, np_ulong flags, mblk_t *dp)
{
	N_data_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(sc->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_DATA_REQ;
	p->DATA_xfer_flags = flags;
	mp->b_wptr += sizeof(*p);
	mp->b_cont = dp;
	sc_txprim(sc, msg, mp, N_DATA_REQ);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_exdata_req: - issue an N_EXDATA_REQ primitive downstream
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @msg: message to free upon error (or NULL)
 * @dp: user data
 */
noinline fastcall int
n_exdata_req(struct sc *sc, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	N_exdata_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(sc->oq, 1)))
		goto ebusy;
	mp->b_band = 1;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_EXDATA_REQ;
	mp->b_wptr += sizeof(*p);
	mp->b_cont = dp;
	sc_txprim(sc, msg, mp, N_EXDATA_REQ);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_info_req: - issue an N_INFO_REQ primitive downstream
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @msg: message to free upon error (or NULL)
 */
noinline fastcall int
n_info_req(struct sc *sc, queue_t *q, mblk_t *msg)
{
	N_info_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_INFO_REQ;
	mp->b_wptr += sizeof(*p);
	sc_txprim(sc, msg, mp, N_INFO_REQ);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_bind_req: - issue an N_BIND_REQ primitive downstream
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @msg: message to free upon error (or NULL)
 * @add: address to bind
 * @coninds: maximum number of connection indications
 * @flags: bind flags
 */
noinline fastcall int
n_bind_req(struct sc *sc, queue_t *q, mblk_t *msg, struct sccp_addr *add, np_ulong coninds, np_ulong flags)
{
	N_bind_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p) + sizeof(sc->add) + SCCP_MAX_ADDR_LENGTH + sizeof(sc->pro);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(sc->oq, 0)))
		goto ebusy;
	np_s_address(add, &sc->add, &sc->ADDR_length);
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_BIND_REQ;
	p->ADDR_length = sc->ADDR_length;
	p->ADDR_offset = sc->ADDR_length ? sizeof(*p) : 0;
	p->CONIND_number = coninds;
	p->BIND_flags = flags;
	p->PROTOID_length = sc->PROTOID_length;
	p->PROTOID_offset = sc->PROTOID_length ? p->ADDR_offset + p->ADDR_length : 0;
	mp->b_wptr += sizeof(*p);
	bcopy(&sc->add, mp->b_wptr, p->ADDR_length);
	mp->b_wptr += p->ADDR_length;
	bcopy(&sc->pro, mp->b_wptr, p->PROTOID_length);
	mp->b_wptr += p->PROTOID_length;
	sc_set_state(sc, NS_WACK_BREQ);
	sc_txprim(sc, msg, mp, N_BIND_REQ);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_unbind_req: - issue an N_UNBIND_REQ primitive downstream
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @msg: message to free upon error (or NULL)
 */
noinline fastcall int
n_unbind_req(struct sc *sc, queue_t *q, mblk_t *msg)
{
	N_unbind_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(sc->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_UNBIND_REQ;
	mp->b_wptr += sizeof(*p);
	sc_set_state(sc, NS_WACK_UREQ);
	sc_txprim(sc, msg, mp, N_UNBIND_REQ);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_unitdata_req: - issue an N_UNITDATA_REQ primitive downstream
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @msg: message to free upon error (or NULL)
 * @dst: destination of message
 * @qos: Quality of Service parameters
 * @dp: user data
 */
noinline fastcall int
n_unitdata_req(struct sc *sc, queue_t *q, mblk_t *msg, struct sccp_addr *dst,
	       N_qos_sel_data_sccp_t *qos, mblk_t *dp)
{
	N_unitdata_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p) + sizeof(*dst) + sizeof(*qos);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(sc->oq, 0)))
		goto ebusy;
	np_s_address(dst, &sc->rem, &sc->REMADDR_length);
	np_s_options(qos, &sc->qos, &sc->QOS_length);
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_UNITDATA_REQ;
	p->DEST_length = sc->REMADDR_length;
	p->DEST_offset = sc->REMADDR_length ? sizeof(*p) : 0;
	p->RESERVED_field[0] = sc->QOS_length;
	p->RESERVED_field[1] = sc->QOS_length ? p->DEST_offset + p->DEST_length : 0;
	mp->b_wptr += sizeof(*p);
	bcopy(&sc->rem, mp->b_wptr, p->DEST_length);
	mp->b_wptr += p->DEST_length;
	bcopy(&sc->qos, mp->b_wptr, p->RESERVED_field[0]);
	mp->b_wptr += p->RESERVED_field[0];
	mp->b_cont = dp;
	sc_txprim(sc, msg, mp, N_UNITDATA_REQ);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_optmgmt_req: - issue an N_OPTMGMT_REQ primitive downstream
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @msg: message to free upon error (or NULL)
 * @qptr: QoS parameter pointer
 * @qlen: Qos parameter length
 * @flags: management flags
 */
noinline fastcall int
n_optmgmt_req(struct sc *sc, queue_t *q, mblk_t *msg, caddr_t qptr, size_t qlen, np_ulong flags)
{
	N_optmgmt_req_t *p;
	mblk_t *mp;
	const size_t mlen = sizeof(*p) + qlen;

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(sc->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_OPTMGMT_REQ;
	p->QOS_length = qlen;
	p->QOS_offset = qlen ? sizeof(*p) : 0;
	p->OPTMGMT_flags = flags;
	mp->b_wptr += sizeof(*p);
	bcopy(qptr, mp->b_wptr, p->QOS_length);
	mp->b_wptr += p->QOS_length;
	if (sc_get_state(sc) == NS_IDLE)
		sc_set_state(sc, NS_WACK_OPTREQ);
	sc_txprim(sc, msg, mp, N_OPTMGMT_REQ);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_datack_req: - issue an N_DATACK_REQ primitive downstream
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @msg: message to free upon error (or NULL)
 */
noinline fastcall int
n_datack_req(struct sc *sc, queue_t *q, mblk_t *msg)
{
	N_datack_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(sc->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_DATACK_REQ;
	mp->b_wptr += sizeof(*p);
	sc->state.datacks--;
	sc_txprim(sc, msg, mp, N_DATACK_REQ);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_reset_req: - issue an N_RESET_REQ primitive downstream
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @msg: message to free upon error (or NULL)
 * @reason: reset reason
 */
noinline fastcall int
n_reset_req(struct sc *sc, queue_t *q, mblk_t *msg, np_ulong reason)
{
	N_reset_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(sc->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_RESET_REQ;
	p->RESET_reason = reason;
	mp->b_wptr += sizeof(*p);
	sc_set_state(sc, NS_WCON_RREQ);
	sc_txprim(sc, msg, mp, N_RESET_REQ);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_reset_res: - issue an N_RESET_RES primitive downstream
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @msg: message to free upon error (or NULL)
 */
noinline fastcall int
n_reset_res(struct sc *sc, queue_t *q, mblk_t *msg)
{
	N_reset_res_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(sc->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_RESET_RES;
	mp->b_wptr += sizeof(*p);
	sc_set_state(sc, NS_WACK_RRES);
	sc_txprim(sc, msg, mp, N_RESET_RES);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_request_req: - issue an N_REQUEST_REQ primitive downstream
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @msg: message to free upon error (or NULL)
 * @loc: source address (or NULL)
 * @sref: source local reference
 */
noinline fastcall int
n_request_req(struct sc *sc, queue_t *q, mblk_t *msg, struct sccp_addr *loc,
	      N_qos_sel_conn_sccp_t *qos, np_ulong sref)
{
	N_request_req_t *p;
	mblk_t *mp;
	static const size_t mlen =
	    sizeof(*p) + sizeof(sc->loc) + SCCP_MAX_ADDR_LENGTH + sizeof(sc->qos);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(sc->oq, 0)))
		goto ebusy;
	np_s_address(loc, &sc->loc, &sc->LOCADDR_length);
	np_s_options(qos, &sc->qos, &sc->QOS_length, NULL);
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_REQUEST_REQ;
	p->SRC_length = sc->LOCADDR_length;
	p->SRC_offset = sc->LOCADDR_length ? sizeof(*p) : 0;
	p->SRC_lref = sref;
	p->QOS_length = sc->QOS_length;
	p->QOS_offset = sc->QOS_length ? p->SRC_offset + p->SRC_length : 0;
	mp->b_wptr += sizeof(*p);
	bcopy(&sc->loc, mp->b_wptr, p->SRC_length);
	mp->b_wptr += p->SRC_length;
	bcopy(&sc->qos, mp->b_wptr, p->QOS_length);
	mp->b_wptr += p->QOS_length;
	sc_set_state(sc, NS_WCON_CREQ);
	sc_txprim(sc, msg, mp, N_REQUEST_REQ);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_inform_req: - issue an N_INFORM_REQ primitive downstream
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @msg: message to free upon error (or NULL)
 * @resaon: inform reason
 */
noinline fastcall int
n_inform_req(struct sc *sc, queue_t *q, mblk_t *msg, np_ulong reason)
{
	N_inform_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p) + sizeof(sc->qos);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(sc->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_INFORM_REQ;
	p->QOS_length = sizeof(sc->qos);
	p->QOS_offset = sizeof(*p);
	p->REASON = reason;
	mp->b_wptr += sizeof(*p);
	bcopy(&sc->qos, mp->b_wptr, p->QOS_length);
	mp->b_wptr += p->QOS_length;
	sc_txprim(sc, msg, mp, N_INFORM_REQ);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_state_req: - issue an N_STATE_REQ primitive downstream
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @msg: message to free upon error (or NULL)
 */
noinline fastcall int
n_state_req(struct sc *sc, queue_t *q, mblk_t *msg, np_ulong status)
{
	N_state_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p) + sizeof(sc->add);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(sc->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_STATE_REQ;
	p->ADDR_length = sizeof(sc->add);
	p->ADDR_offset = sizeof(*p);
	p->STATUS = status;
	mp->b_wptr += sizeof(*p);
	bcopy(&sc->add, mp->b_wptr, p->ADDR_length);
	mp->b_wptr += p->ADDR_length;
	sc_txprim(sc, msg, mp, N_STATE_REQ);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_coord_req: - issue an N_COORD_REQ primitive downstream
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @msg: message to free upon error (or NULL)
 */
noinline fastcall int
n_coord_req(struct sc *sc, queue_t *q, mblk_t *msg)
{
	N_coord_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p) + sizeof(sc->add);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(sc->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_COORD_REQ;
	p->ADDR_length = sizeof(sc->add);
	p->ADDR_offset = sizeof(*p);
	mp->b_wptr += sizeof(*p);
	bcopy(&sc->add, mp->b_wptr, p->ADDR_length);
	mp->b_wptr += p->ADDR_length;
	sc_txprim(sc, msg, mp, N_COORD_REQ);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_coord_res: - issue an N_COORD_RES primitive downstream
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @msg: message to free upon error (or NULL)
 */
noinline fastcall int
n_coord_res(struct sc *sc, queue_t *q, mblk_t *msg)
{
	N_coord_res_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p) + sizeof(sc->add);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(sc->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_COORD_RES;
	p->ADDR_length = sizeof(sc->add);
	p->ADDR_offset = sizeof(*p);
	mp->b_wptr += sizeof(*p);
	bcopy(&sc->add, mp->b_wptr, p->ADDR_length);
	mp->b_wptr += p->ADDR_length;
	sc_txprim(sc, msg, mp, N_COORD_RES);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * n_parse_qos: - parse options
 * @np: private structure (locked)
 * @optr: options pointer
 * @olen: options length
 * @flags: options flags
 * @prim: primitive for which to parse options
 */
static int
np_parse_qos(struct np *np, unsigned char *optr, size_t olen, np_ulong flags, np_ulong prim)
{
	N_qos_sccp_t qos;
	int err;

	if (unlikely(olen > sizeof(qos)))
		goto badopt;
	bcopy(optr, &qos, olen);
	switch (prim) {
	case N_CONN_REQ:
		if (qos->n_qos_type != N_QOS_SEL_CONN_SCCP)
			goto badqostype;
		if (qos->sel_conn.protocol_class != QOS_UNKNOWN)
			np->qos.protocol_class = qos->sel_conn.protocol_class;
		if (qos->sel_conn.option_flags != QOS_UNKNOWN)
			np->qos.option_flags = qos->sel_conn.option_flags;
		if (qos->sel_conn.sequence_selection != QOS_UNKNOWN)
			np->qos.sequence_selection = qos->sel_conn.sequence_selection;
		if (qos->sel_conn.message_priority != QOS_UNKNOWN)
			np->qos.message_priority = qos->sel_conn.message_priority;
		if (qos->sel_conn.importance != QOS_UNKNOWN)
			np->qos.importance = qos->sel_conn.importance;
		if (qos->sel_conn.credit != QOS_UNKNOWN)
			np->qos.credit = qos->sel_conn.credit;
		break;
	case N_CONN_RES:
		if (qos->n_qos_type != N_QOS_SEL_CONN_SCCP)
			goto badqostype;
		if (qos->sel_conn.protocol_class != QOS_UNKNOWN)
			np->qos.protocol_class = qos->sel_conn.protocol_class;
		if (qos->sel_conn.option_flags != QOS_UNKNOWN)
			np->qos.option_flags = qos->sel_conn.option_flags;
		if (qos->sel_conn.sequence_selection != QOS_UNKNOWN)
			np->qos.sequence_selection = qos->sel_conn.sequence_selection;
		if (qos->sel_conn.message_priority != QOS_UNKNOWN)
			np->qos.message_priority = qos->sel_conn.message_priority;
		if (qos->sel_conn.importance != QOS_UNKNOWN)
			np->qos.importance = qos->sel_conn.importance;
		if (qos->sel_conn.credit != QOS_UNKNOWN)
			np->qos.credit = qos->sel_conn.credit;
		break;
	case N_UNITDATA_REQ:
		if (qos->n_qos_type != N_QOS_SEL_DATA_SCCP)
			goto badqostype;
		if (qos->sel_data.protocol_class != QOS_UNKNOWN)
			np->qos.protocol_class = qos->sel_data.protocol_class;
		if (qos->sel_data.option_flags != QOS_UNKNOWN)
			np->qos.option_flags = qos->sel_data.option_flags;
		if (qos->sel_data.sequence_selection != QOS_UNKNOWN)
			np->qos.sequence_selection = qos->sel_data.sequence_selection;
		if (qos->sel_data.message_priority != QOS_UNKNOWN)
			np->qos.message_priority = qos->sel_data.message_priority;
		if (qos->sel_data.importance != QOS_UNKNOWN)
			np->qos.importance = qos->sel_data.importance;
		break;
	default:
		break;
	}
	return (0);
      badqostype:
	err = NBADQOSTYPE;
	goto error;
      badopt:
	err = NBADOPT;
	goto error;
      error:
	return (err);
}

/*
 * NPI User to NPI Provider Primitives
 */

/**
 * np_conn_req: - process N_CONN_REQ primitive
 * @np: network provider private structure (locked)
 * @q: active queue
 * @mp: the N_CONN_REQ primitive
 */
noinline fastcall int
np_conn_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_conn_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	if (unlikely(np_get_state(np) != NS_IDLE))
		goto outstate;
	if (unlikely(!MBLKIN(mp, p->DEST_offset, p->DEST_length)))
		goto badaddr;
	if (unlikely(p->DEST_length == 0))
		goto noaddr;
	if (unlikely(p->DEST_length < sizeof(np->rem)))
		goto badaddr;
	bcopy(mp->b_rptr + p->DEST_offset, &np->rem, sizeof(np->rem));
	if (unlikely(!MBLKIN(mp, p->QOS_offset, p->QOS_length)))
		goto badopt;
	if (p->QOS_length == 0)
		goto noqos;
	if (p->QOS_length < sizeof(np->qos))
		goto badopt;
	if ((err = n_parse_qos(np, mp->b_rptr + p->QOS_offset, p->QOS_length, p->CONN_flags, N_CONN_REQ)))
		goto error;
      noqos:
	if (mp->b_cont && unlikely(msgsize(mp->b_cont) > np->INFO_ack.CDATA_size))
		goto baddata;
	np_set_state(np, NS_WCON_CREQ);
	return n_conn_req(np->sc, q, mp, &np->rem, &np->qos, p->CONN_flags, mp->b_cont);
      baddata:
	err = NBADDATA;
	goto error;
      badopt:
	err = NBADOPT;
	goto error;
      noaddr:
	err = NNOADDR;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * np_conn_res: - process N_CONN_RES primitive
 * @np: network provider private structure (locked)
 * @q: active queue
 * @mp: the N_CONN_RES primitive
 */
noinline fastcall int
np_conn_res(struct np *np, queue_t *q, mblk_t *mp)
{
	N_conn_res_t *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *res;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	if (unlikely(np_get_state(np) != NS_WRES_CIND))
		goto outstate;

	if (unlikely(!MBLKIN(mp, p->RES_offset, p->RES_length)))
		goto badaddr;
	if (p->RES_length) {
		if (p->RES_length < sizeof(*res))
			goto badaddr;
		res = (typeof(res)) (mp->b_rptr + p->RES_offset);
		bcopy(res, &np->res, sizeof(*res));
		np->RES_length = sizeof(*res);
		res = &np->res;
	} else
		res = NULL;	/* Responding address is implied by the bound address. */

	if (unlikely(!MBLKIN(mp, p->QOS_offset, p->QOS_length)))
		goto badqos;
	if (p->QOS_length == 0)
		goto noqos;
	if (unlikely(p->QOS_length < sizeof(np->qos)))
		goto badopt;
	if (unlikely((err = n_parse_qos(np, mp->b_rptr + p->QOS_offset, p->QOS_length, p->CONN_flags, N_CONN_RES))))
		goto error;
      noqos:
	if (mp->b_cont && unlikely(msgsize(mp->b_cont) > np->INFO_ack.CDATA_size))
		goto baddata;
	np->state.ACCEPTOR_id = p->TOKEN_value;
	np_set_state(np, NS_WACK_CRES);
	return n_conn_res(np->sc, q, mp, res, &np->qos, p->TOKEN_value, p->CONN_flags, p->SEQ_number,
			  mp->b_cont);
      baddata:
	err = NBADDATA;
	goto error;
      badopt:
	err = NBADOPT;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * np_discon_req: - process N_DISCON_REQ primitive
 * @np: network provider private structure (locked)
 * @q: active queue
 * @mp: the N_DISCON_REQ primitive
 */
noinline fastcall int
np_discon_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_discon_req_t *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *res;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	switch (__builtin_expect(np_get_state(np), NS_DATA_XFER)) {
	case NS_WCON_CREQ:
		np_set_state(np, NS_WACK_DREQ6);
		break;
	case NS_WRES_CIND:
		np_set_state(np, NS_WACK_DREQ7);
		break;
	case NS_DATA_XFER:
		np_set_state(np, NS_WACK_DREQ9);
		break;
	case NS_WCON_RREQ:
		np_set_state(np, NS_WACK_DREQ10);
		break;
	case NS_WRES_RIND:
		np_set_state(np, NS_WACK_DREQ11);
		break;
	default:
		goto outstate;
	}
	if (unlikely(!MBLKIN(mp, p->RES_offset, p->RES_length)))
		goto badaddr;
	if (p->RES_length) {
		if (p->RES_length < sizeof(np->res))
			goto badaddr;
		res = (typeof(res)) (mp->b_rptr + p->RES_offset);
		bcopy(res, &np->res, sizeof(np->res));
	} else {
		res = NULL;
	}
	if (mp->b_cont && unlikely(msgsize(mp->b_cont) > np->INFO_ack.DDATA_size))
		goto baddata;
	return n_discon_req(np->sc, q, mp, res, p->DISCON_reason, p->SEQ_number, mp->b_cont);
      baddata:
	err = NBADDATA;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * np_data_req: - process N_DATA_REQ primitive
 * @np: network provider private structure (locked)
 * @q: active queue
 * @mp: the N_DATA_REQ primitive
 */
noinline fastcall int
np_data_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_data_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	switch (__builtin_expect(np_get_state(np), NS_DATA_XFER)) {
	case NS_DATA_XFER:
		break;
	case NS_IDLE:
		goto discard;
	default:
		goto outstate;
	}
	if (np->state.edatack)
		/* The NS user must wait until the expedited data request is acknowledged before
		   any other normal data or expedited data primitives are issued. */
		goto outstate;
	if (p->DATA_xfer_flags & ~(N_RC_FLAG | N_MORE_DATA_FLAG))
		goto badflag;
	if (mp->b_cont && unlikely(msgsize(mp->b_cont) > np->INFO_ack.NIDU_size))
		goto baddata;
	if (mp->b_cont && unlikely(msgsize(mp->b_cont) > np->INFO_ack.NSDU_size))
		goto baddata;
	if (p->DATA_xfer_flags & N_RC_FLAG)
		np->state.datacks++;
	return n_data_req(np->sc, q, mp, p->DATA_xfer_flags, mp->b_cont);
      baddata:
	err = NBADDATA;
	goto error;
      badflag:
	err = NBADFLAG;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      discard:
	freemsg(mp);
	err = 0;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * np_exdata_req: - process N_EXDATA_REQ primitive
 * @np: network provider private structure (locked)
 * @q: active queue
 * @mp: the N_EXDATA_REQ primitive
 */
noinline fastcall int
np_exdata_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_exdata_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	switch (__builtin_expect(np_get_state(np), NS_DATA_XFER)) {
	case NS_DATA_XFER:
		break;
	case NS_IDLE:
		goto discard;
	default:
		goto outstate;
	}
	if (np->state.edatack)
		/* The NS user must wait until the expedited data request is acknowledged before
		   any other normal data or expedited data primitives are issued. */
		goto outstate;






	if (mp->b_cont && unlikely(msgsize(mp->b_cont) > np->INFO_ack.ENSDU_size))
		goto baddata;


	np->state.edatack = 1;
	return n_exdata_req(np->sc, q, mp, mp->b_cont);
      baddata:
	err = NBADDATA;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      discard:
	freemsg(mp);
	err = 0;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * np_info_req: - process N_INFO_REQ primitive
 * @np: network provider private structure (locked)
 * @q: active queue
 * @mp: the N_INFO_REQ primitive
 */
noinline fastcall int
np_info_req(struct np *np, queue_t *q, mblk_t *mp)
{
	return n_info_req(np->sc, q, mp);
}

/**
 * np_bind_req: - process N_BIND_REQ primitive
 * @np: network provider private structure (locked)
 * @q: active queue
 * @mp: the N_BIND_REQ primitive
 */
noinline fastcall int
np_bind_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_bind_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	if (unlikely(np_get_state(np) != NS_UNBND))
		goto outstate;
	if (unlikely(!MBLKIN(mp, p->ADDR_offset, p->ADDR_length)))
		goto badaddr;
	if (p->ADDR_length) {
		if (p->ADDR_length < sizeof(np->add))
			goto badaddr;
		bcopy(mp->b_rptr + p->ADDR_offset, &np->add, sizeof(np->add));
	}
	if (unlikely(!MBLKIN(mp, p->PROTOID_offset, p->PROTOID_length)))
		goto badaddr;
	if (p->PROTOID_length == 0)
		goto noproto;
	if (p->PROTOID_length > sizeof(np->pro))
		goto badaddr;
	bcopy(mp->b_rptr + p->PROTOID_offset, &np->pro, p->PROTOID_length);
	if (p->BIND_flags & ~(DEFAULT_LISTENER | TOKEN_REQUEST | DEFAULT_DEST))
		goto badflag;
	np_set_state(np, NS_WACK_BREQ);
	return n_bind_req(np->sc, q, mp, p->CONIND_number, p->BIND_flags);
      badflag:
	err = NBADFLAG;
	goto error;
      noproto:
	err = NNOPROTOID;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * np_unbind_req: - process N_UNBIND_REQ primitive
 * @np: network provider private structure (locked)
 * @q: active queue
 * @mp: the N_UNBIND_REQ primitive
 */
noinline fastcall int
np_unbind_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_unbind_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	if (unlikely(np_get_state(np) != NS_IDLE))
		goto outstate;
	np_set_state(np, NS_WACK_UREQ);
	return n_unbind_req(np->sc, q, mp);
      outstate:
	err = NOUTSTATE;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * np_unitdata_req: - process N_UNITDATA_REQ primitive
 * @np: network provider private structure (locked)
 * @q: active queue
 * @mp: the N_UNITDATA_REQ primitive
 */
noinline fastcall int
np_unitdata_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_unitdata_req_t *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *rptr = NULL;
	N_qos_sel_data_sccp_t *qptr = NULL;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	switch (__builtin_expect(np_get_state(np), NS_IDLE)) {
	case NS_IDLE:
		if (p->DEST_length == 0)
			goto noaddr;
		rptr = &np->rem;
		break;
	case NS_DATA_XFER:
		/* Why not? If connection oriented wants to use this primitive, let it.  But ignore
		   the destination address. */
		break;
	default:
		goto outstate;
	}
	if (unlikely(!MBLKIN(mp, p->DEST_offset, p->DEST_length)))
		goto badaddr;
	if (rptr) {
		if (p->DEST_length < sizeof(*rptr))
			goto badaddr;
		bcopy(mp->b_rptr + p->DEST_offset, rptr, sizeof(*rptr));
	}
	if (unlikely(!MBLKIN(mp, p->RESERVED_field[1], p->RESERVED_field[0])))
		goto badqos;
	if (p->RESERVED_field[0]) {
		qptr = (N_qos_sel_data_sccp_t *)&np->qos;
		if (p->RESERVED_field[0] < sizeof(*qptr))
			goto badqos;
		bcopy(mp->b_rptr + p->RESERVED_field[1], qptr, sizeof(*qptr));
	}
	if (mp->b_cont && unlikely(msgsize(mp->b_cont) > np->INFO_ack.NIDU_size))
		goto baddata;
	if (mp->b_cont && unlikely(msgsize(mp->b_cont) > np->INFO_ack.NSDU_size))
		goto baddata;
	return n_unitdata_req(np->sc, q, mp, rptr, qptr, mp->b_cont);
      baddata:
	err = NBADDATA;
	goto error;
      badqos:
	err = NBADOPT;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      noaddr:
	err = NNOADDR;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * np_optmgmt_req: - process N_OPTMGMG_REQ primitive
 * @np: network provider private structure (locked)
 * @q: active queue
 * @mp: the N_OPTMGMG_REQ primitive
 */
noinline fastcall int
np_optmgmt_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_optmgmt_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	if (unlikely(!MBLKIN(mp, p->QOS_offset, p->QOS_length)))
		goto badqos;
	if (unlikely(p->QOS_length == 0))
		goto badqostype;
	if (unlikely(p->OPTMGMT_flags & ~(DEFAULT_RC_SEL)))
		goto badflag;
	if (np_get_state(np) == NS_IDLE)
		np_set_state(np, NS_WACK_OPTREQ);
	return n_optmgmt_req(np->sc, q, mp, mp->b_rptr + p->QOS_offset, p->QOS_length,
			     p->OPTMGMT_flags);
      badflag:
	err = NBADFLAG;
	goto error;
      badqostype:
	err = NBADQOSTYPE;
	goto error;
      badqos:
	err = NBADOPT;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * np_datack_req: - process N_DATACK_REQ primitive
 * @np: network provider private structure (locked)
 * @q: active queue
 * @mp: the N_DATACK_REQ primitive
 */
noinline fastcall int
np_datack_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_datack_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	switch (__builtin_expect(np_get_state(np), NS_DATA_XFER)) {
	case NS_IDLE:
		/* Note: If the interface is in the NS_IDLE state when the provider receives the
		   N_DATACK_REQ primitive, then the NS provider should discard the request without
		   generating a fatal error. */
		goto discard;
	case NS_DATA_XFER:
		break;
	default:
		goto outstate;
	}
	if (np->state.datinds <= 0)
		/* Note: If the NS provider had no knowledge of a previous N_DATA_IND with the
		   receipt confirmation flag set, then the NS provider should just ignore the
		   request without generating a fatal error. */
		goto discard;
	np->state.datinds--;
	return n_datack_req(np->sc, q, mp);
      outstate:
	err = NOUTSTATE;
	goto error;
      discard:
	freemsg(mp);
	err = 0;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * np_inform_req: - process N_RESET_REQ primitive
 * @np: network provider private structure (locked)
 * @q: active queue
 * @mp: the N_RESET_REQ primitive
 *
 * The N-INFORM request primitive is implemented using the N_RESET_REQ primitive.
 */
noinline fastcall int
np_inform_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_reset_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	switch (__builtin_expect(np_get_state(np), NS_DATA_XFER)) {
	case NS_DATA_XFER:
		break;
	case NS_IDLE:
		goto discard;
	default:
		goto outstate;
	}
	return n_inform_req(np->sc, q, mp, p->RESET_reason);
      outstate:
	err = NOUTSTATE;
	goto error;
      discard:
	freemsg(mp);
	err = 0;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * np_reset_req: - process N_RESET_REQ primitive
 * @np: network provider private structure (locked)
 * @q: active queue
 * @mp: the N_RESET_REQ primitive
 *
 * The N-RESET request primitive is implemented using the N_RESET_REQ primitive.
 */
noinline fastcall int
np_reset_req(struct np *np, queue_t *q, mblk_t *mp)
{
	N_reset_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	switch (__builtin_expect(np_get_state(np), NS_DATA_XFER)) {
	case NS_DATA_XFER:
		break;
	case NS_IDLE:
		/* Note: If the interface is in the NS_IDLE state when the provider receives the
		   N_RESET_REQ primitive, then the NS provider should discard the message without
		   generating an error. */
		goto discard;
	default:
		goto outstate;
	}
	if (np->state.resacks)
		/* If there are outstanding reset requests, we are out of state. */
		goto outstate;
	np->state.resacks = 1;
	np_set_state(np, NS_WCON_RREQ);
	switch (p->RESET_reason) {
	case 0:
#if 0
	case XXX: /* FIXME */
	case YYY:
	case ZZZ:
#endif
		/* NPI-SCCP uses a combination of the N_OPTMGMT_REQ and the N_RESET_REQ to emulate
		   the N_INFORM_REQ primitive.  When the RESET_reason is one of the three reset
		   reasons used by the N_INFORM_REQ, then the primitive is transformed to an
		   N_INFORM_REQ, otherwise it is passed as an N_RESET_REQ. */
		np->state.infinds = 1;
		return n_inform_req(np->sc, q, mp, p->RESET_reason);
	}
	np->state.infinds = 0;
	return n_reset_req(np->sc, q, mp, p->RESET_reason);
      outstate:
	err = NOUTSTATE;
	goto error;
      discard:
	freemsg(mp);
	err = 0;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * np_reset_res: - process N_RESET_RES primitive
 * @np: network provider private structure (locked)
 * @q: active queue
 * @mp: the N_RESET_RES primitive
 *
 * The N-RESET response primitive is implemented using the N_RESET_RES primitive.
 */
noinline fastcall int
np_reset_res(struct np *np, queue_t *q, mblk_t *mp)
{
	N_reset_res_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	switch (__builtin_expect(np_get_state(np), NS_WRES_RIND)) {
	case NS_WRES_RIND:
		break;
	case NS_IDLE:
		/* Note: If the interface is in the NS_IDLE state when the provider receives the
		   N_RESET_RES primitive, then the NS provider should discard the message without
		   generating an error. */
		goto discard;
	default:
		goto outstate;
	}
	if (np->state.resinds <= 0)
		goto outstate;
	if (np->state.infinds) {
		np->state.infinds = 0;
		np_set_state(np, NS_WACK_RRES);
		return n_ok_ack(np, q, mp, N_RESET_RES);
	}
	np->state.resinds = 0;
	np_set_state(np, NS_WACK_RRES);
	return n_reset_res(np->sc, q, mp);
      outstate:
	err = NOUTSTATE;
	goto error;
      discard:
	freemsg(mp);
	err = 0;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/*
 * --------------------------------------------------------------------------
 * --------------------------------------------------------------------------
 * --------------------------------------------------------------------------
 * --------------------------------------------------------------------------
 */

















/*
 * SCCPI provider to SCCPI user primitives.
 */

/**
 * sc_error_reply: - process errors out of line
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @prim: primitive in error
 * @error: negative UNIX error or positive NPI error (or zero)
 * @location: location error was detected
 */
noinline fastcall __unlikely int
sc_error_reply(struct sc *sc, queue_t *q, mblk_t *msg, np_ulong prim, np_long error,
	       const char *location)
{
	int level, flags;

	switch (error) {
	case -EAGAIN:
	case -ENOBUFS:
	case -ENOMEM:
	case -EDEADLK:
	case -EBUSY:
	case 0:
		return (err);
	}
	switch (prim) {
	case N_DATA_IND:
	case N_EXDATA_IND:
	case N_DATACK_IND:
	case N_UDERROR_IND:
	case N_UNITDATA_IND:
		level = STRLOGDA;
		flags = SL_TRACE;
		break;
	default:
		level = STRLOGNO;
		flags = SL_TRACE | SL_ERROR;
		break;
	}
	mi_strlog(q, level, flags, "ERROR: %s: %s: %s", location, np_errname(error),
		  np_strerror(error));
	return m_error(sc->np, q, msg, EPROTO, EPROTO);
}

/**
 * sc_conn_ind: - process N_CONN_IND primitive
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @mp: the N_CONN_IND primitive
 */
noinline fastcall int
sc_conn_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_conn_ind_t *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *loc, *rem;
	N_qos_sel_conn_sccp_t *qos;
	int err;

	switch (__builtin_expect(sc_get_state(sc), NS_IDLE)) {
	case NS_IDLE:
	case NS_WRES_CIND:
		break;
	default:
		goto outstate;
	}
	if (p->DEST_length) {
		loc = (typeof(loc)) (mp->b_rptr + p->DEST_offset);
		bcopy(loc, &sc->loc, p->DEST_length);
		sc->LOCADDR_length = p->DEST_length;
		loc = &sc->loc;
	} else
		loc = NULL;
	if (p->SRC_length) {
		rem = (typeof(rem)) (mp->b_rptr + p->SRC_offset);
		bcopy(rem, &sc->rem, p->SRC_length);
		sc->REMADDR_length = p->SRC_length;
		rem = &sc->rem;
	} else
		rem = NULL;
	if (p->QOS_length) {
		qos = (typeof(qos)) (mp->b_rptr + p->QOS_offset);
		bcopy(qos, &sc->qos, p->QOS_length);
		sc->QOS_length = p->QOS_length;
		qos = &sc->qos;
	} else
		qos = NULL;
	sc->state.coninds++;
	sc_set_state(sc, NS_WRES_CIND);

	return n_conn_ind(sc->np, q, mp, loc, rem, qos, p->SEQ_number, p->CONN_flags, mp->b_cont);
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return (err);
}

/**
 * sc_conn_con: - process N_CONN_CON primitive
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @mp: the N_CONN_CON primitive
 */
noinline fastcall int
sc_conn_con(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_conn_con_t *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *res;
	N_qos_sel_conn_sccp_t *qos;
	int err;

	if (unlikely(sc_get_state(sc) != NS_WCON_CREQ))
		goto outstate;
	if (p->RES_length) {
		res = (typeof(res)) (mp->b_rptr + p->RES_offset);
		bcopy(res, &sc->res, p->RES_length);
		res = &sc->res;
	} else
		res = NULL;
	if (p->QOS_length) {
		qos = (typeof(qos)) (mp->b_rptr + p->QOS_offset);
		bcopy(qos, &sc->qos, p->QOS_length);
		qos = &sc->qos;
	} else
		qos = NULL;
	sc_set_state(sc, NS_DATA_XFER);


	return n_conn_con(sc->np, q, mp, res, qos, p->CONN_flags, mp->b_cont);
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return (err);
}

/**
 * sc_discon_ind: - process N_DISCON_IND primitive
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @mp: the N_DISCON_IND primitive
 */
noinline fastcall int
sc_discon_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_discon_ind_t *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *res;
	int err;

	sc_set_state(sc, NS_IDLE);
	if (p->RES_length) {
		res = (typeof(res)) (mp->b_rptr + p->RES_offset);
		bcopy(res, &sc->res, p->RES_length);
		res = &sc->res;
	} else
		res = NULL;
	if (p->SEQ_number)
		sc->state.coninds--;
	sc_set_state(sc, sc->state.coninds ? NS_WRES_CIND : NS_IDLE);
	/* FIXME: convert disconnet origin and reason */
	return n_discon_ind(sc->np, q, mp, res, p->DISCON_orig, p->DISCON_reason, p->SEQ_number,
			    mp->b_cont);
      error:
	return (err);
}

/**
 * sc_data_ind: - process N_DATA_IND primitive
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @mp: the N_DATA_IND primitive
 */
noinline fastcall int
sc_data_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_data_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	switch (__builtin_expect(sc_get_state(sc), NS_DATA_XFER)) {
	case NS_IDLE:
		goto discard;
	case NS_DATA_XFER:
	case NS_WRES_RIND:
	case NS_WCON_RREQ:
		break;
	default:
		goto outstate;
	}
	return n_data_ind(sc->np, q, mp, p->DATA_xfer_flags, mp->b_cont);
      outstate:
	err = NOUTSTATE;
	goto error;
      discard:
	freemsg(mp);
	err = 0;
	goto error;
      error:
	return (err);
}

/**
 * sc_exdata_ind: - process N_EXDATA_IND primitive
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @mp: the N_EXDATA_IND primitive
 */
noinline fastcall int
sc_exdata_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_exdata_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	switch (__builtin_expect(sc_get_state(sc), NS_DATA_XFER)) {
	case NS_DATA_XFER:
	case NS_WRES_RIND:
	case NS_WCON_RREQ:
		break;
	case NS_IDLE:
		goto discard;
	default:
		goto outstate;
	}
	return n_exdata_ind(sc->np, q, mp, 0, mp->b_cont);
      outstate:
	err = NOUTSTATE;
	goto error;
      discard:
	freemsg(mp);
	err = 0;
	goto error;
      error:
	return (err);
}

/**
 * sc_info_ack: - process N_INFO_ACK primitive
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @mp: the N_INFO_ACK primitive
 */
noinline fastcall int
sc_info_ack(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_info_ack_t *p = (typeof(p)) mp->b_rptr;
	struct np *np = sc->np;
	mblk_t *msg;

	sc->INFO_ack.PRIM_type = p->PRIM_type;
	sc->INFO_ack.NSDU_size = p->NSDU_size;
	sc->INFO_ack.ENSDU_size = p->ENSDU_size;
	sc->INFO_ack.CDATA_size = p->CDATA_size;
	sc->INFO_ack.DDATA_size = p->DDATA_size;
	sc->INFO_ack.ADDR_size = p->ADDR_size;
	sc->INFO_ack.ADDR_length = p->ADDR_length;
	sc->INFO_ack.ADDR_offset = p->ADDR_offset;
	sc->INFO_ack.QOS_length = p->QOS_length;
	sc->INFO_ack.QOS_offset = p->QOS_offset;
	sc->INFO_ack.QOS_range_length = p->QOS_range_length;
	sc->INFO_ack.QOS_range_offset = p->QOS_range_offset;
	sc->INFO_ack.OPTIONS_flags = p->OPTIONS_flags;
	sc->INFO_ack.NIDU_size = p->NIDU_size;
	sc->INFO_ack.SERV_type = p->SERV_type;
	sc->INFO_ack.CURRENT_state = p->CURRENT_state;
	sc->INFO_ack.PROVIDER_type = p->PROVIDER_type;
	sc->INFO_ack.NODU_size = p->NODU_size;
	sc->INFO_ack.PROTOID_length = p->PROTOID_length;
	sc->INFO_ack.PROTOID_offset = p->PROTOID_offset;
	sc->INFO_ack.NPI_version = p->NPI_version;


	if (p->ADDR_length) {
		struct sccp_addr *add;

		add = (typeof(add)) (mp->b_rptr + p->ADDR_offset);
		bcopy(add, &sc->add, p->ADDR_length);
		sc->ADDR_length = p->ADDR_length;
		add = &sc->add;
		sc_n_address(add, &np->add, &np->ADDR_length);
	}
	if (p->QOS_length) {
		N_qos_sel_info_sccp_t *qos;

		qos = (typeof(qos)) (mp->b_rptr + p->QOS_offset);
		bcopy(qos, &sc->qos, p->QOS_length);
		sc->QOS_length = p->QOS_length;
		qos = &sc->qos;
		bcopy(qos, &np->qos, p->QOS_length);
		np->QOS_length = p->QOS_length;
	}
	if (p->QOS_range_length) {
		N_qos_range_info_sccp_t *qor;

		qor = (typeof(qor)) (mp->b_rptr + p->QOS_range_offset);
		bcopy(qor, &sc->qor, p->QOS_range_length);
		sc->QOS_range_length = p->QOS_length;
		qor = &sc->qor;
		bcopy(qor, &np->qor, p->QOS_range_length);
		np->QOS_range_length = p->QOS_range_length;
	}
	if (p->PROTOID_length) {
		np_ulong *pro;

		pro = (typeof(pro)) (mp->b_rptr + p->PROTOID_offset);
		bcopy(pro, &sc->pro, p->PROTOID_length);
		sc->PROTOID_length = p->PROTOID_length;
		pro = &sc->pro;
	}

	if ((msg = XCHG(&sc->np->state.pending, NULL))) {
		switch (*(np_ulong *)mp->b_rptr) {
		case N_INFO_REQ:
			return n_info_ack(sc->np, q, msg);
		}
	}
	freemsg(mp);
	return (0);
}

/**
 * sc_bind_ack: - process N_BIND_ACK primitive
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @mp: the N_BIND_ACK primitive
 */
noinline fastcall int
sc_bind_ack(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_bind_ack_t *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *add;
	unsigned char *pro;
	int err;

	if (unlikely(sc_get_state(sc) != NS_WACK_BREQ))
		goto outstate;
	if (p->ADDR_length) {
		add = (typeof(add)) (mp->b_rptr + p->ADDR_offset);
		bcopy(add, &sc->add, p->ADDR_length);
		sc->ADDR_length = p->ADDR_length;
		add = &sc->add;
	} else
		add = NULL;
	sc->INFO_ack.ADDR_length = p->ADDR_length;
	if (p->PROTOID_length) {
		pro = (typeof(pro)) (mp->b_rptr + p->PROTOID_offset);
		bcopy(pro, &sc->pro, p->PROTOID_length);
		sc->PROTOID_length = p->PROTOID_length;
		pro = &sc->pro;
	} else
		pro = NULL;
	sc->INFO_ack.PROTOID_length = p->PROTOID_length;
	sc->CONIND_number = p->CONIND_number;
	sc->TOKEN_value = p->TOKEN_value;
	sc_set_state(sc, NS_IDLE);
	return n_bind_ack(sc->np, q, mp);
      oustate:
	err = NOUTSTATE;
	goto error;
      error:
	return (err);
}

/**
 * sc_error_ack: - process N_ERROR_ACK primitive
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @mp: the N_ERROR_ACK primitive
 */
noinline fastcall int
sc_error_ack(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_error_ack_t *p = (typeof(p)) mp->b_rptr;
	np_long error, prim;

	prim = p->ERROR_prim;
	if ((error = p->NPI_error) == NSYSERR)
		error = -p->UNIX_error;
	switch (sc_get_state(sc)) {
	case NS_WACK_BREQ:
		sc_set_state(sc, NS_UNBND);
		break;
	case NS_WACK_UREQ:
		sc_set_state(sc, NS_IDLE);
		break;
	case NS_WACK_OPTREQ:
		sc_set_state(sc, NS_IDLE);
		break;
	case NS_WACK_RRES:
		sc_set_state(sc, NS_WRES_RIND);
		break;
	case NS_WCON_CREQ:
		sc_set_state(sc, NS_IDLE);
		if (tp_get_state(sc->tp) != TS_WCON_CREQ)
			break;
		/* In this state we have already issued a T_OK_ACK back in response
		   to a received T_CONN_REQ and the upper module is not expecting a
		   T_ERROR_ACK. The event must be transformed into a T_DISCON_IND
		   and issued upstream. */
		return t_discon_ind(sc->tp, q, mp, NULL, np_t_error(error), 0, NULL);
	case NS_WACK_CRES:
		sc_set_state(sc, NS_WRES_CIND);
		break;
	case NS_WCON_RREQ:
		sc_set_state(sc, NS_DATA_XFER);
		break;
	case NS_WACK_DREQ6:
		sc_set_state(sc, NS_WCON_CREQ);
		break;
	case NS_WACK_DREQ7:
		sc_set_state(sc, NS_WRES_CIND);
		break;
	case NS_WACK_DREQ9:
		sc_set_state(sc, NS_DATA_XFER);
		break;
	case NS_WACK_DREQ10:
		sc_set_state(sc, NS_WCON_RREQ);
		break;
	case NS_WACK_DREQ11:
		sc_set_state(sc, NS_WRES_RIND);
		break;
	}
	return n_error_ack(sc->np, q, mp, np_t_prim(prim), np_t_error(error));
}

/**
 * sc_ok_ack: - process N_OK_ACK primitive
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @mp: the N_OK_ACK primitive
 */
noinline fastcall int
sc_ok_ack(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_ok_ack_t *p = (typeof(p)) mp->b_rptr;
	np_ulong prim;

	prim = p->CORRECT_prim;
	switch (__builtin_expect(sc_get_state(sc), NS_WACK_RRES)) {
	case NS_WACK_BREQ:
	case NS_WACK_OPTREQ:
	case NS_WACK_DREQ6:
	case NS_WACK_DREQ7:
	case NS_WACK_DREQ9:
	case NS_WACK_DREQ10:
	case NS_WACK_DREQ11:
		sc_set_state(sc, NS_IDLE);
		break;
	case NS_WACK_UREQ:
		sc_set_state(sc, NS_UNBND);
		break;
	case NS_WACK_RRES:
	case NS_WACK_CRES:
		sc_set_state(sc, NS_DATA_XFER);
		break;
	}
	return n_ok_ack(sc->np, q, mp, np_t_prim(prim));
}

/**
 * sc_unitdata_ind: - process N_UNITDATA_IND primitive
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @mp: the N_UNITDATA_IND primitive
 */
noinline fastcall int
sc_unitdata_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_unitdata_ind_t *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *rem;
	struct sccp_addr *loc;
	int err;

	switch (sc_get_state(sc), NS_IDLE) {
	case NS_IDLE:
	case NS_WACK_UREQ:
		break;
	default:
		goto discard;
	}
	loc = p->SRC_length ? (typeof(loc)) (mp->b_rptr + p->SRC_offset) : NULL;
	rem = p->DEST_length ? (typeof(rem)) (mp->b_rptr + p->DEST_offset) : NULL;
	return n_unitdata_ind(sc->np, q, mp, rem, loc, NULL, p->ERROR_type, mp->b_cont);
      discard:
	freemsg(mp);
	err = 0;
	goto error;
      error:
	return (err);
}

/**
 * sc_uderror_ind: - process N_UDERROR_IND primitive
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @mp: the N_UDERROR_IND primitive
 */
noinline fastcall int
sc_uderror_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_uderror_ind_t *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *rem;
	int err;

	switch (__builtin_expect(sc_get_state(sc), NS_IDLE)) {
	case NS_IDLE:
	case NS_WACK_UREQ:
		break;
	default:
		goto outstate;
	}
	rem = p->DEST_length ? (typeof(rem)) (mp->b_rptr + p->DEST_offset) : NULL;
	return n_uderror_ind(sc->np, q, mp, rem, NULL, p->ERROR_type, mp->b_cont);
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return (err);
}

/**
 * sc_datack_ind: - process N_DATACK_IND primitive
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @mp: the N_DATACK_IND primitive
 */
noinline fastcall int
sc_datack_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_datack_ind_t *p = (typeof(p)) mp->b_rptr;

	if (sc->state.edatack)
		sc->state.edatack = 0;
	else if (sc->state.datacks >= 0)
		sc->state.datacks--;
	return n_datack_ind(sc->np, q, mp);
}

/**
 * sc_reset_ind: - process N_RESET_IND primitive
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @mp: the N_RESET_IND primitive
 */
noinline fastcall int
sc_reset_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_reset_ind_t *p = (typeof(p)) mp->b_rptr;

	sc_set_state(sc, NS_WRES_RIND);
	return n_reset_ind(sc->np, q, mp, p->RESET_orig, p->RESET_reason);
}

/**
 * sc_reset_con: - process N_RESET_CON primitive
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @mp: the N_RESET_CON primitive
 */
noinline fastcall int
sc_reset_con(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_reset_con_t *p = (typeof(p)) mp->b_rptr;

	sc_set_state(sc, NS_DATA_XFER);
	return n_reset_con(sc->np, q, mp);
}

/**
 * sc_notice_ind: - process N_NOTICE_IND primitive
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @mp: the N_NOTICE_IND primitive
 */
noinline fastcall int
sc_notice_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_notice_ind_t *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *rem;

	rem = (typeof(rem)) (mp->b_rptr + p->DEST_offset);
	/* In mapping the N_NOTICE_IND primitive to the N_UDERROR_IND primitive, the source address 
	   and quality of service parameters associated with the message are lost. */
	return n_uderror_ind(sc->np, q, mp, rem, NULL, p->RETURN_cause, mp->b_cont);
}

/**
 * sc_inform_ind: - process N_INFORM_IND primitive
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @mp: the N_INFORM_IND primitive
 */
noinline fastcall int
sc_inform_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_inform_ind_t *p = (typeof(p)) mp->b_rptr;

	bcopy(mp->b_rptr + p->QOS_offset, &sc->qos, p->QOS_length);
	sc->state.infinds = 1;
	sc_set_state(sc, NS_WRES_RIND);
	return n_inform_ind(sc->np, q, mp, N_PROVIDER, p->REASON);
}

/**
 * sc_coord_ind: - process N_COORD_IND primitive
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @mp: the N_COORD_IND primitive
 */
noinline fastcall int
sc_coord_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_coord_ind_t *p = (typeof(p)) mp->b_rptr;

	freemsg(mp);
	return (0);
}

/**
 * sc_coord_con: - process N_COORD_CON primitive
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @mp: the N_COORD_CON primitive
 */
noinline fastcall int
sc_coord_con(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_coord_con_t *p = (typeof(p)) mp->b_rptr;

	freemsg(mp);
	return (0);
}

/**
 * sc_state_ind: - process N_STATE_IND primitive
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @mp: the N_STATE_IND primitive
 */
noinline fastcall int
sc_state_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_state_ind_t *p = (typeof(p)) mp->b_rptr;

	freemsg(mp);
	return (0);
}

/**
 * sc_pcstate_ind: - process N_PCSTATE_IND primitive
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @mp: the N_PCSTATE_IND primitive
 */
noinline fastcall int
sc_pcstate_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_pcstate_ind_t *p = (typeof(p)) mp->b_rptr;

	freemsg(mp);
	return (0);
}

/**
 * sc_traffic_ind: - process N_TRAFFIC_IND primitive
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @mp: the N_TRAFFIC_IND primitive
 */
noinline fastcall int
sc_traffic_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_traffic_ind_t *p = (typeof(p)) mp->b_rptr;

	freemsg(mp);
	return (0);
}

/**
 * sc_reply_ack: - process N_REPLY_ACK primitive
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @mp: the N_REPLY_ACK primitive
 */
noinline fastcall int
sc_reply_ack(struct sc *sc, queue_t *q, mblk_t *mp)
{
	N_reply_ack_t *p = (typeof(p)) mp->b_rptr;

	bcopy(mp->b_rptr + p->QOS_offset, &sc->qos, p->QOS_length);
	sc->SRC_lref = p->SRC_lref;
	freemsg(mp);
	return (0);
}

/**
 * sc_other_ind: - process unrecognized primitive
 * @sc: SCCP private structure (locked)
 * @q: active queue
 * @mp: the unrecognized primitive
 */
noinline fastcall int
sc_other_ind(struct sc *sc, queue_t *q, mblk_t *mp)
{
	return (NNOTSUPPORT);
}

/*
 * MESSGAGE HANDLING
 */

/**
 * m_w_other: - process other message
 * @q: active queue (write queue)
 * @mp: STREAMS message
 */
static int
m_w_other(queue_t *q, mblk_t *mp)
{
	if (!pcmsg(DB_TYPE(mp)) && !bcanputnext(q, mp->b_band))
		return (-EBUSY);
	putnext(q, mp);
	return (0);
}

/**
 * m_r_other: - process other message
 * @q: active queue (read queue)
 * @mp: STREAMS message
 */
static int
m_r_other(queue_t *q, mblk_t *mp)
{
	if (!pcmsg(DB_TYPE(mp)) && !bcanputnext(q, mp->b_band))
		return (-EBUSY);
	putnext(q, mp);
	return (0);
}

/*
 * M_DATA HANDLING
 */

/**
 * sccp_w_data: - process M_DATA or M_HPDATA message
 * @np: network provider private structure (locked)
 * @q: active queue (write queue)
 * @mp: M_DATA or M_HPDATA message
 */
static inline fastcall int
sccp_w_data(struct np *np, queue_t *q, mblk_t *mp)
{
	int err;

	switch (__builtin_expect(np_get_state(np), NS_DATA_XFER)) {
	case NS_DATA_XFER:
		break;
	case NS_IDLE:
		goto discard;
	default:
		goto outstate;
	}
	if (unlikely(np->state.edatack))
		/* The NS user must wait until the expedited data request is acknowledged before
		   any other normal data or expedited data primitives are issued. */
		goto outstate;
	if (unlikely(msgsize(mp) > np->INFO_ack.NIDU_size))
		goto baddata;
	if (likely(mp->b_band)) {
		if (unlikely(msgsize(mp) > tp->CAPABILITY_ack.INFO_ack.ETSDU_size))
			goto baddata;
		return n_exdata_req(np->sc, q, NULL, mp);
	}
	if (unlikely(msgsize(mp) > tp->CAPABILITY_ack.INFO_ack.TSDU_size))
		goto baddata;
	return n_data_req(np->sc, q, NULL, (mp->b_flag & MSGDELIM) ? 0 : N_MORE_DATA_FLAG, mp);
      baddata:
	err = NBADDATA;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      discard:
	err = 0;
	goto error;
      error:
	return n_error_reply(np, q, mp, N_DATA_REQ, err, __FUNCTION__);
}

/**
 * sccp_r_data: - process M_DATA or M_HPDATA message
 * @sc: SCCPI private structure (locked)
 * @q: active queue (read queue)
 * @mp: M_DATA or M_HPDATA message
 */
static inline fastcall int
sccp_r_data(struct sc *sc, queue_t *q, mblk_t *mp)
{
	int err;

	switch (__builtin_expect(sc_get_state(sc), NS_DATA_XFER)) {
	case NS_DATA_XFER:
		break;
	case NS_IDLE:
		goto discard;
	default:
		goto outstate;
	}
	if (unlikely(msgsize(mp) > sc->INFO_ack.NIDU_size))
		goto baddata;
	if (likely(mp->b_band)) {
		if (unlikely(msgsize(mp) > sc->INFO_ack.ENSDU_size))
			goto baddata;
		return n_exdata_ind(sc->np, q, NULL, mp);
	}
	if (unlikely(msgsize(mp) > sc->INFO_ack.NSDU_size))
		goto baddata;
	return n_data_ind(sc->np, q, NULL, (mp->b_flag & MSGDELIM) ? 0 : N_MORE_DATA_FLAG, mp);
      baddata:
	err = NBADDATA;
	goto error;
      discard:
	freemsg(mp);
	err = 0;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return n_error_reply(sc, q, mp, N_DATA_IND, err, __FUNCTION__);
}

/**
 * m_w_data: - process M_DATA or M_HPDATA message
 * @q: active queue (write queue)
 * @mp: M_DATA or M_HPDATA message
 */
static inline fastcall int
m_w_data(queue_t *q, mblk_t *mp)
{
	struct priv *priv;
	int err;

	if (likely(!!(priv = (struct priv *) mi_trylock(q)))) {
		err = sccp_w_data(&priv->np, q, mp);
		mi_unlock((caddr_t) priv);
	} else
		err = -EDEADLK;
	return (err);
}

/**
 * m_r_data: - process M_DATA or M_HPDATA message
 * @q: active queue (read queue)
 * @mp: M_DATA or M_HPDATA message
 */
static inline fastcall int
m_r_data(queue_t *q, mblk_t *mp)
{
	struct priv *priv;
	int err;

	if (likely(!!(priv = (struct priv *) mi_trylock(q)))) {
		err = sccp_r_data(&priv->sc, q, mp);
		mi_unlock((caddr_t) priv);
	} else
		err = -EDEADLK;
	return (err);
}

/*
 * M_(PC)PROTO HANDLING
 */

/**
 * sccp_w_proto: - process M_PROTO or M_PCPROTO message
 * @np: network provider private structure (locked)
 * @q: active queue (write queue)
 * @mp: M_PROTO or M_PCPROTO message
 */
static fastcall int
sccp_w_proto(struct np *np, queue_t *q, mblk_t *mp)
{
	np_ulong prim;
	int err, level;

	np_save_state(np);
	if (unlikely(!MBLKIN(mp, 0, sizeof(prim))))
		goto efault;
	switch ((prim = *(typeof(prim) *) mp->b_rptr)) {
	case N_DATA_REQ:
	case N_EXDATA_REQ:
	case N_DATACK_REQ:
	case N_UNITDATA_REQ:
		level = STRLOGDA;
		break;
	default:
		level = STRLOGRX;
		break;
	}
	mi_strlog(q, level, SL_TRACE, "-> %s", np_primname(prim));
	switch (prim) {
	case N_CONN_REQ:
		err = np_conn_req(np, q, mp);
		break;
	case N_CONN_RES:
		err = np_conn_res(np, q, mp);
		break;
	case N_DISCON_REQ:
		err = np_discon_req(np, q, mp);
		break;
	case N_DATA_REQ:
		err = np_data_req(np, q, mp);
		break;
	case N_EXDATA_REQ:
		err = np_exdata_req(np, q, mp);
		break;
	case N_INFO_REQ:
		err = np_info_req(np, q, mp);
		break;
	case N_BIND_REQ:
		err = np_bind_req(np, q, mp);
		break;
	case N_UNBIND_REQ:
		err = np_unbind_req(np, q, mp);
		break;
	case N_UNITDATA_REQ:
		err = np_unitdata_req(np, q, mp);
		break;
	case N_OPTMGMT_REQ:
		err = np_optmgmt_req(np, q, mp);
		break;
	case N_DATACK_REQ:
		err = np_datack_req(np, q, mp);
		break;
	case N_RESET_REQ:
		err = np_reset_req(np, q, mp);
		break;
	case N_RESET_RES:
		err = np_reset_res(np, q, mp);
		break;
	default:
		err = np_other_req(np, q, mp);
		break;
	}
      done:
	if (unlikely(err)) {
		np_restore_state(np);
		return n_error_reply(np, q, mp, prim, err, np_primname(prim));
	}
	return (err);
      efault:
	prim = -1;
	err = -EFAULT;
	goto done;
}

/**
 * sccp_r_proto: - process M_PROTO or M_PCPROTO message
 * @sc: SCCPI private structure (locked)
 * @q: active queue (read queue)
 * @mp: M_PROTO or M_PCPROTO message
 */
static fastcall int
sccp_r_proto(struct sc *sc, queue_t *q, mblk_t *mp)
{
	np_ulong prim;
	int err, level;

	sc_save_state(sc);
	if (unlikely(!MBLKIN(mp, 0, sizeof(prim))))
		goto efault;
	switch ((prim = *(typeof(prim) *) mp->b_rptr)) {
	case N_DATA_IND:
	case N_EXDATA_IND:
	case N_UNITDATA_IND:
	case N_UDERROR_IND:
	case N_DATACK_IND:
	case N_NOTICE_IND:
		level = STRLOGDA;
		break;
	default:
		level = STRLOGRX;
		break;
	}
	mi_strlog(q, level, SL_TRACE, "%s <-", sc_primname(prim));
	switch (prim) {
	case N_CONN_IND:
		err = sc_conn_ind(sc, q, mp);
		break;
	case N_CONN_CON:
		err = sc_conn_con(sc, q, mp);
		break;
	case N_DISCON_IND:
		err = sc_discon_ind(sc, q, mp);
		break;
	case N_DATA_IND:
		err = sc_data_ind(sc, q, mp);
		break;
	case N_EXDATA_IND:
		err = sc_exdata_ind(sc, q, mp);
		break;
	case N_INFO_ACK:
		err = sc_info_ack(sc, q, mp);
		break;
	case N_BIND_ACK:
		err = sc_bind_ack(sc, q, mp);
		break;
	case N_ERROR_ACK:
		err = sc_error_ack(sc, q, mp);
		break;
	case N_OK_ACK:
		err = sc_ok_ack(sc, q, mp);
		break;
	case N_UNITDATA_IND:
		err = sc_unitdata_ind(sc, q, mp);
		break;
	case N_UDERROR_IND:
		err = sc_uderror_ind(sc, q, mp);
		break;
	case N_DATACK_IND:
		err = sc_datack_ind(sc, q, mp);
		break;
	case N_RESET_IND:
		err = sc_reset_ind(sc, q, mp);
		break;
	case N_RESET_CON:
		err = sc_reset_con(sc, q, mp);
		break;
	case N_NOTICE_IND:
		err = sc_notice_ind(sc, q, mp);
		break;
	case N_INFORM_IND:
		err = sc_inform_ind(sc, q, mp);
		break;
	case N_COORD_IND:
		err = sc_coord_ind(sc, q, mp);
		break;
	case N_COORD_CON:
		err = sc_coord_con(sc, q, mp);
		break;
	case N_STATE_IND:
		err = sc_state_ind(sc, q, mp);
		break;
	case N_PCSTATE_IND:
		err = sc_pcstate_ind(sc, q, mp);
		break;
	case N_TRAFFIC_IND:
		err = sc_traffic_ind(sc, q, mp);
		break;
	case N_REPLY_ACK:
		err = sc_reply_ack(sc, q, mp);
		break;
	default:
		err = sc_other_ind(sc, q, mp);
		break;
	}
      done:
	if (unlikely(err)) {
		sc_restore_state(sc);
		return sc_error_reply(sc, q, mp, prim, err, sc_primname(prim));
	}
	return (err);
      efault:
	prim = -1;
	err = -EFAULT;
	goto done;
}

/**
 * m_w_proto: - process M_PROTO or M_PCPROTO message
 * @q: active queue (write queue)
 * @mp: M_PROTO or M_PCPROTO message
 */
static int
m_w_proto(queue_t *q, mblk_t *mp)
{
	struct priv *priv;
	int err;

	if (likely(!!(priv = (struct priv *) mi_trylock(q)))) {
		err = sccp_w_proto(&priv->np, q, mp);
		mi_unlock((caddr_t) priv);
	} else
		err = -EDEADLK;
	return (err);
}

/**
 * m_r_proto: - process M_PROTO or M_PCPROTO message
 * @q: active queue (read queue)
 * @mp: M_PROTO or M_PCPROTO message
 */
static int
m_r_proto(queue_t *q, mblk_t *mp)
{
	struct priv *priv;
	int err;

	if (likely(!!(priv = (struct priv *) mi_trylock(q)))) {
		err = sccp_r_proto(&priv->sc, q, mp);
		mi_unlock((caddr_t) priv);
	} else
		err = -EDEADLK;
	return (err);
}

/*
 * M_FLUSH HANDLING
 */

/**
 * m_w_flush: - process M_FLUSH message
 * @q: active queue (write queue)
 * @mp: M_FLUSH message
 */
noinline fastcall int
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

/**
 * m_r_flush: - process M_FLUSH message
 * @q: active queue (read queue)
 * @mp: M_FLUSH message
 */
noinline fastcall int
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

/**
 * m_r_error: - process M_ERROR message
 * @q: active queue (read queue)
 * @mp: M_ERROR message
 */
noinline fastcall int
m_r_error(queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

/**
 * m_r_hangup: - process M_HANGUP message
 * @q: active queue (read queue)
 * @mp: M_HANGUP message
 */
noinline fastcall int
m_r_hangup(queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

/*
 * STREAMS MESSAGE HANDLING
 */

/**
 * sccp_w_msg: - process STREAMS message
 * @np: network provider private structure (locked)
 * @q: active queue (write queue)
 * @mp: STREAMS message
 */
static fastcall int
sccp_w_msg(struct np *np, queue_t *q, mblk_t *mp)
{
	int err;

	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		err = sccp_w_data(np, q, mp);
		break;
	case M_PROTO:
	case M_PCPROTO:
		err = sccp_w_proto(np, q, mp);
		break;
	case M_FLUSH:
		err = m_w_flush(q, mp);
		break;
	default:
		err = m_w_other(q, mp);
		break;
	}
	return (err);
}

/**
 * sccp_r_msg: - process STREAMS message
 * @sc: SCCPI private structure (locked)
 * @q: active queue (read queue)
 * @mp: STREAMS message
 */
static fastcall int
sccp_r_msg(struct sc *sc, queue_t *q, mblk_t *mp)
{
	int err;

	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		err = sccp_r_data(sc, q, mp);
		break;
	case M_PROTO:
	case M_PCPROTO:
		err = sccp_r_proto(sc, q, mp);
		break;
	case M_FLUSH:
		err = m_r_flush(q, mp);
		break;
	case M_ERROR:
		err = m_r_error(q, mp);
		break;
	case M_HANGUP:
		err = m_r_hangup(q, mp);
		break;
	default:
		err = m_r_other(q, mp);
		break;
	}
	return (err);
}

/**
 * m_w_msg: - process STREAMS message
 * @q: active queue (write queue)
 * @mp: STREAMS message
 */
static fastcall int
m_w_msg(queue_t *q, mblk_t *mp)
{
	int err;

	switch (__builtin_expect(DB_TYPE(mp), M_DATA)) {
	case M_DATA:
	case M_HPDATA:
		err = m_w_data(q, mp);
		break;
	case M_PROTO:
	case M_PCPROTO:
		err = m_w_proto(q, mp);
		break;
	case M_FLUSH:
		err = m_w_flush(q, mp);
		break;
	default:
		err = m_w_other(q, mp);
		break;
	}
	return (err);
}

/**
 * m_r_msg: - process STREAMS message
 * @q: active queue (read queue)
 * @mp: STREAMS message
 */
static fastcall int
m_r_msg(queue_t *q, mblk_t *mp)
{
	int err;

	switch (__builtin_expect(DB_TYPE(mp), M_DATA)) {
	case M_DATA:
	case M_HPDATA:
		err = m_r_data(q, mp);
		break;
	case M_PROTO:
	case M_PCPROTO:
		err = m_r_proto(q, mp);
		break;
	case M_FLUSH:
		err = m_r_flush(q, mp);
		break;
	case M_ERROR:
		err = m_r_error(q, mp);
		break;
	case M_HANGUP:
		err = m_r_hangup(q, mp);
		break;
	default:
		err = m_r_other(q, mp);
		break;
	}
	return (err);
}

/*
 * QUEUE PUT AND SERVICE PROCEDURE
 */

/**
 * sccp_wput: - write put procedure
 * @q: active queue (write queue)
 * @mp: message to put
 */
static streamscall __hot_put int
sccp_wput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || m_w_msg(q, mp)) {
		if (unlikely(!putq(q, mp))) {
			mp->b_band = 0;
			putq(q, mp);
		}
	}
	return (0);
}

/**
 * sccp_wsrv: - write service procedure
 * @q: active queue (write queue)
 */
static streamscall __hot_out int
sccp_wsrv(queue_t *q)
{
	struct priv *priv;
	mblk_t *mp;

	if (likely(!!(priv = (struct priv *) mi_trylock(q)))) {
		if (likely(!!(mp = getq(q)))) {
			do {
				if (unlikely(sccp_w_msg(&priv->np, q, mp))) {
					if (unlikely(!putbq(q, mp))) {
						mp->b_band = 0;
						putbq(q, mp);
					}
					break;
				}
			} while (unlikely(!!(mp = getq(q))));
		}
		mi_unlock((caddr_t) priv);
	}
	return (0);
}

/**
 * sccp_rsrv: - read service procedure
 * @q: active queue (read queue)
 */
static streamscall __hot_get int
sccp_rsrv(queue_t *q)
{
	struct priv *priv;
	mblk_t *mp;

	if (likely(!!(priv = (struct priv *) mi_trylock(q)))) {
		if (likely(!!(mp = getq(q)))) {
			do {
				if (unlikely(sccp_r_msg(&priv->sc, q, mp))) {
					if (unlikely(!putbq(q, mp))) {
						mp->b_band = 0;
						putbq(q, mp);
					}
					break;
				}
			} while (unlikely(!!(mp = getq(q))));
		}
		mi_unlock((caddr_t) priv);
	}
	return (0);
}

/**
 * sccp_rput: - read put procedure
 * @q: active queue (read queue)
 * @mp: message to put
 */
static streamscall __hot_in int
sccp_rput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || m_r_msg(q, mp)) {
		if (unlikely(!putq(q, mp))) {
			mp->b_band = 0;
			putq(q, mp);
		}
	}
	return (0);
}

/*
 * QUEUE OPEN AND CLOSE ROUTINE
 */

static caddr_t sccp_opens = NULL;

/**
 * sccp_qopen: - STREAMS open routine
 * @q: opening queue pair read queue
 * @devp: device number pointer
 * @oflags: open flags
 * @sflag: STREAMS flag
 * @credp: credentials of opening process
 */
static streamscall int
sccp_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *credp)
{
	struct priv *priv;
	int err;

	if (q->q_ptr != NULL)
		return (0);
	if (unlikely((err = mi_open_comm(&sccp_opens, sizeof(*priv), q, devp,
					 oflags, sflag, credp))))
		return (err);
	priv = PRIV(q);
	bzero(priv, sizeof(*priv));
	priv->cred = *credp;
	priv->np.oq = RD(q);
	priv->np.state.state = NS_UNBND;
	priv->np.state.datinds = 0;
	priv->np.state.datacks = 0;
	priv->np.state.coninds = 0;
	priv->np.state.infinds = 0;
	priv->np.INFO_ack.PRIM_type = N_INFO_ACK;
	priv->np.INFO_ack.NSDU_size = QOS_UNKNOWN;
	priv->np.INFO_ack.ENSDU_size = QOS_UNKNOWN;
	priv->np.INFO_ack.CDATA_size = QOS_UNKNOWN;
	priv->np.INFO_ack.DDATA_size = QOS_UNKNOWN;
	priv->np.INFO_ack.ADDR_size = sizeof(struct sccp_addr) + SCCP_MAX_ADDR_LENGTH;
	priv->np.INFO_ack.ADDR_length = 0;
	priv->np.INFO_ack.ADDR_offset = 0;
	priv->np.INFO_ack.QOS_length = 0;
	priv->np.INFO_ack.QOS_offset = 0;
	priv->np.INFO_ack.QOS_range_length = 0;
	priv->np.INFO_ack.QOS_range_offset = 0;
	priv->np.INFO_ack.OPTIONS_flags = 0;
	priv->np.INFO_ack.NIDU_size = QOS_UNKNOWN;
	priv->np.INFO_ack.SERV_type = N_CLNS | N_CONS;
	priv->np.INFO_ack.CURRENT_state = NS_UNBND;
	priv->np.INFO_ack.PROVIDER_type = N_SNICFP;
	priv->np.INFO_ack.NODU_size = 256;
	priv->np.INFO_ack.PROTOID_length = 0;
	priv->np.INFO_ack.PROTOID_offset = 0;
	priv->np.INFO_ack.NPI_version = N_VERSION_2;
	/* ... */
	priv->sc.oq = WR(q);
	priv->sc.state.state = NS_UNBND;
	priv->sc.state.datinds = 0;
	priv->sc.state.datacks = 0;
	priv->sc.state.coninds = 0;
	priv->sc.state.infinds = 0;
	priv->sc.INFO_ack.PRIM_type = N_INFO_ACK;
	priv->sc.INFO_ack.NSDU_size = QOS_UNKNOWN;
	priv->sc.INFO_ack.ENSDU_size = QOS_UNKNOWN;
	priv->sc.INFO_ack.CDATA_size = QOS_UNKNOWN;
	priv->sc.INFO_ack.DDATA_size = QOS_UNKNOWN;
	priv->sc.INFO_ack.ADDR_size = sizeof(struct sccp_addr) + SCCP_MAX_ADDR_LENGTH;
	priv->sc.INFO_ack.ADDR_length = 0;
	priv->sc.INFO_ack.ADDR_offset = 0;
	priv->sc.INFO_ack.QOS_length = 0;
	priv->sc.INFO_ack.QOS_offset = 0;
	priv->sc.INFO_ack.QOS_range_length = 0;
	priv->sc.INFO_ack.QOS_range_offset = 0;
	priv->sc.INFO_ack.OPTIONS_flags = 0;
	priv->sc.INFO_ack.NIDU_size = QOS_UNKNOWN;
	priv->sc.INFO_ack.SERV_type = N_CLNS | N_CONS;
	priv->sc.INFO_ack.CURRENT_state = NS_UNBND;
	priv->sc.INFO_ack.PROVIDER_type = N_SNICFP;
	priv->sc.INFO_ack.NODU_size = 256;
	priv->sc.INFO_ack.PROTOID_length = 0;
	priv->sc.INFO_ack.PROTOID_offset = 0;
	priv->sc.INFO_ack.NPI_version = N_VERSION_2;
	/* ... */
	return (0);
}

/**
 * sccp_qclose: - STREAMS close procedure
 * @q: closing queue pair read queue
 * @oflags: open flags
 * @credp: credentials of closing process
 */
static streamscall int
sccp_qclose(queue_t *q, int oflags, cred_t *credp)
{
	qprocsoff(q);
	flushq(RD(q), FLUSHALL);
	flushq(WR(q), FLUSHALL);
	mi_close_comm(&sccp_opens, q);
	return (0);
}

/*
 * STREAMS DEFINITIONS
 */

static struct module_info sccp_minfo = {
	.mi_idnum = MOD_ID,
	.mi_idname = MOD_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat sccp_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat sccp_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static struct qinit sccp_rdinit = {
	.qi_putp = sccp_rput,
	.qi_srvp = sccp_rsrv,
	.qi_qopen = sccp_qopen,
	.qi_qclose = sccp_qclose,
	.qi_minfo = &sccp_minfo,
	.qi_mstat = &sccp_rstat,
};

static struct qinit sccp_wrinit = {
	.qi_putp = sccp_wput,
	.qi_srvp = sccp_wsrv,
	.qi_minfo = &sccp_minfo,
	.qi_mstat = &sccp_wstat,
};

static struct streamtab sccp_npiinfo = {
	.st_rdinit = &sccp_rdinit,
	.st_wrinit = &sccp_wrinit,
};

static struct fmodsw sccp_fmod = {
	.f_name = MOD_NAME,
	.f_str = &sccp_npiinfo,
	.f_flag = D_MP,
	.f_modid = MOD_ID,
	.f_kmod = THIS_MODULE,
};

static modID_t modid = MOD_ID;

#ifdef LINUX
#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0444);
#endif
MODULE_PARM_DESC(modid, "Module ID for SCCP-NPI.  (0 for allocation.)");
#endif				/* LINUX */

/**
 * sccp_npiinit: - initialize kernel module
 */
MODULE_STATIC int __init
sccp_npiinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);
	if (unlikely((err = register_strmod(&sccp_fmod)) < 0)) {
		cmn_err(CE_WARN, "%s: could not register module: err = %d\n", MOD_NAME, -err);
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

/**
 * sccp_npiterminate: - deinitialize kernel module
 */
MODULE_STATIC void __exit
sccp_npiterminate(void)
{
	int err;

	if (unlikely((err = unregister_strmod(&sccp_fmod)) < 0))
		cmn_err(CE_WARN, "%s: could not unregister module: err = %d\n", MOD_NAME, -err);
	return;
}

#ifdef LINUX
module_init(sccp_npiinit);
module_exit(sccp_npiterminate);
#endif				/* LINUX */
