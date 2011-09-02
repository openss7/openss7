/*****************************************************************************

 @(#) $RCSfile: sccp_tpi_old.c,v $ $Name:  $($Revision: 1.1.2.3 $) $Date: 2011-09-02 08:46:54 $

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

 $Log: sccp_tpi_old.c,v $
 Revision 1.1.2.3  2011-09-02 08:46:54  brian
 - sync up lots of repo and build changes from git

 Revision 1.1.2.2  2010-11-28 14:22:05  brian
 - remove #ident, protect _XOPEN_SOURCE

 Revision 1.1.2.1  2009-06-21 11:40:34  brian
 - added files to new distro

 *****************************************************************************/

static char const ident[] = "$RCSfile: sccp_tpi_old.c,v $ $Name:  $($Revision: 1.1.2.3 $) $Date: 2011-09-02 08:46:54 $";

/*
 * This is a TPI module that can be pushed over an SCCPI (Signalling Connection Control Part
 * Interface) stream to effect a TPI interface to the SCCP suitable for use with XTI.  This module
 * is intended to be used by applications programs or by upper modules that expect a TPI
 * connection-oriented or connectionless service provider.
 */

#define _SVR4_SOURCE	1
#define _MPS_SOURCE	1
#define _SUN_SOURCE	1

#include <sys/os7/compat.h>
#include <sys/strsun.h>

#include <linux/socket.h>

#include <sys/npi.h>
#include <sys/npi_ss7.h>
#include <sys/npi_sccp.h>
#include <ss7/sccpi.h>

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>

#include <sys/tihdr.h>
#include <sys/xti.h>
#include <sys/xti_ss7.h>
#include <sys/xti_sccp.h>

#define t_tst_bit(nr,addr)  test_bit(nr,addr)
#define t_set_bit(nr,addr)  __set_bit(nr,addr)
#define t_clr_bit(nr,addr) __clear_bit(nr,addr)

#define SCCP_TPI_DESCRIP	"SS7 Signalling Connection Control Part (SCCP) TPI STREAMS MODULE."
#define SCCP_TPI_EXTRA		"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define SCCP_TPI_COPYRIGHT	"Copyright (c) 2008-2011  Monavacon Limited.  All Rights Reserved."
#define SCCP_TPI_REVISION	"OpenSS7 $RCSfile: sccp_tpi_old.c,v $ $Name:  $($Revision: 1.1.2.3 $) $Date: 2011-09-02 08:46:54 $"
#define SCCP_TPI_DEVICE		"SVR 4.2MP SCCPI to TPI Conversion Module (TPI) for SCCP"
#define SCCP_TPI_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SCCP_TPI_LICENSE	"GPL"
#define SCCP_TPI_BANNER		SCCP_TPI_DESCRIP	"\n" \
				SCCP_TPI_EXTRA		"\n" \
				SCCP_TPI_COPYRIGHT	"\n" \
				SCCP_TPI_REVISION	"\n" \
				SCCP_TPI_DEVICE		"\n" \
				SCCP_TPI_CONTACT	"\n"
#define SCCP_TPI_SPLASH		SCCP_TPI_DESCRIP	"\n" \
				SCCP_TPI_REVISION	"\n"

#ifndef CONFIG_STREAMS_SCCP_TPI_NAME
#error CONFIG_STREAMS_SCCP_TPI_NAME must be defined.
#endif
#ifndef CONFIG_STREAMS_SCCP_TPI_MODID
#error CONFIG_STREAMS_SCCP_TPI_MODID must be defined.
#endif

#ifdef LINUX
#ifdef MODULE
MODULE_AUTHOR(SCCP_TPI_CONTACT);
MODULE_DESCRIPTION(SCCP_TPI_DESCRIP);
MODULE_SUPPORTED_DEVICE(SCCP_TPI_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(SCCP_TPI_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-sccp-tpi");
MODULE_ALIAS("streams-module-sccp-tpi");
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_SCCP_TPI_MODID));
#endif				/* MODULE_ALIAS */
#ifdef MODULE_VERSION
MODULE_VERSION(PACKAGE_ENVR);
#endif				/* MODULE_VERSION */
#endif				/* MODULE */
#endif				/* LINUX */

#define SCCP_TPI_MOD_ID		CONFIG_STREAMS_SCCP_TPI_MODID
#define SCCP_TPI_MOD_NAME	CONFIG_STREAMS_SCCP_TPI_NAME

/*
 * STREAMS DEFINITIONS
 */

#define MOD_ID		SCCP_TPI_MOD_ID
#define MOD_NAME	SCCP_TPI_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	SCCP_TPI_BANNER
#else				/* MODULE */
#define MOD_BANNER	SCCP_TPI_SPLASH
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

struct tp;
struct sc;

/* Upper interface structure */
struct tp {
	struct sc *sc;
	queue_t *oq;
	t_uscalar_t CONIND_number;
	struct {
		t_uscalar_t state;
		t_uscalar_t datinds;
		t_uscalar_t datacks;
		t_uscalar_t edatack;
		t_uscalar_t coninds;
		t_uscalar_t infinds;
		t_uscalar_t ACCEPTOR_id;
		mblk_t *pending;
		t_uscalar_t resinds;
		t_uscalar_t resacks;
	} state, oldstate;
	struct T_capability_ack CAPABILITY_ack;
	t_uscalar_t INFORM_reason;
	t_uscalar_t RESET_reason;
	t_uscalar_t DISCON_reason;
	t_uscalar_t TOKEN_value;
	t_uscalar_t OPT_length;
	struct t_sccp_options opts;
	t_uscalar_t ADDR_length;
	struct sockaddr_sccp add;
	t_uscalar_t LOCADDR_length;
	struct sockaddr_sccp loc;
	t_uscalar_t REMADDR_length;
	struct sockaddr_sccp rem;
	t_uscalar_t RES_length;
	struct sockaddr_sccp res;
};

/* Lower interface structure */
struct sc {
	struct tp *tp;
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
	struct tp tp;
	struct sc sc;
};

#define PRIV(q) ((struct priv *)q->q_ptr)
#define NP_PRIV(q) (&PRIV(q)->tp)
#define SC_PRIV(q) (&PRIV(q)->sc)

static inline const char *
tp_primname(const t_scalar_t prim)
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
		return ("(unknown)");
	}
}
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
tp_statename(t_uscalar_t state)
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
static const char *
np_statename(np_long state)
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
tp_errname(t_scalar_t error)
{
	switch (error) {
	case TBADADDR:
		return ("TBADADDR");
	case TBADOPT:
		return ("TBADOPT");
	case TACCES:
		return ("TACCES");
	case TBADF:
		return ("TBADF");
	case TNOADDR:
		return ("TNOADDR");
	case TOUTSTATE:
		return ("TOUTSTATE");
	case TBADSEQ:
		return ("TBADSEQ");
	case TSYSERR:
		return ("TSYSERR");
	case TLOOK:
		return ("TLOOK");
	case TBADDATA:
		return ("TBADDATA");
	case TBUFOVFLW:
		return ("TBUFOVFLW");
	case TFLOW:
		return ("TFLOW");
	case TNODATA:
		return ("TNODATA");
	case TNODIS:
		return ("TNODIS");
	case TNOUDERR:
		return ("TNOUDERR");
	case TBADFLAG:
		return ("TBADFLAG");
	case TNOREL:
		return ("TNOREL");
	case TNOTSUPPORT:
		return ("TNOTSUPPORT");
	case TSTATECHNG:
		return ("TSTATECHNG");
	case TNOSTRUCTYPE:
		return ("TNOSTRUCTURE");
	case TBADNAME:
		return ("TBADNAME");
	case TBADQLEN:
		return ("TBADQLEN");
	case TADDRBUSY:
		return ("TADDRBUSY");
	case TINDOUT:
		return ("TINDOUT");
	case TPROVMISMATCH:
		return ("TPROVMISMATCH");
	case TRESQLEN:
		return ("TRESQLEN");
	case TRESADDR:
		return ("TRESADDR");
	case TQFULL:
		return ("TQFULL");
	case TPROTO:
		return ("TPROTO");
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
tp_strerror(t_scalar_t error)
{
	if (error < 0)
		error = TSYSERR;
	switch (error) {
	case TBADADDR:
		return ("Bad address format.");
	case TBADOPT:
		return ("Bad options format.");
	case TACCES:
		return ("Bad permissions.");
	case TBADF:
		return ("Bad file descriptor.");
	case TNOADDR:
		return ("Unable to allocate an address.");
	case TOUTSTATE:
		return ("Would place interface out of state.");
	case TBADSEQ:
		return ("Bad call sequence number.");
	case TSYSERR:
		return ("System error.");
	case TLOOK:
		return ("Has to t_look() for event.");
	case TBADDATA:
		return ("Bad amount of data.");
	case TBUFOVFLW:
		return ("Buffer was too small.");
	case TFLOW:
		return ("Would block due to flow control.");
	case TNODATA:
		return ("No data indication.");
	case TNODIS:
		return ("No disconnect indication.");
	case TNOUDERR:
		return ("No unitdata error indication.");
	case TBADFLAG:
		return ("Bad flags.");
	case TNOREL:
		return ("No orderly release indication.");
	case TNOTSUPPORT:
		return ("Not supported.");
	case TSTATECHNG:
		return ("State is currently changing.");
	case TNOSTRUCTYPE:
		return ("Structure type not supported.");
	case TBADNAME:
		return ("Bad transport provider name.");
	case TBADQLEN:
		return ("Listener queue length limit is zero.");
	case TADDRBUSY:
		return ("Address already in use.");
	case TINDOUT:
		return ("Outstanding connect indications.");
	case TPROVMISMATCH:
		return ("Not same transport provider.");
	case TRESQLEN:
		return ("Connection acceptor has qlen > 0.");
	case TRESADDR:
		return ("Conn. acceptor bound to different address.");
	case TQFULL:
		return ("Connection indicator queue is full.");
	case TPROTO:
		return ("Protocol error.");
	default:
		return ("(unknown)");
	}
}
static const char *
np_strerror(np_long error)
{
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
		return
		    ("Illegal second attempt to bind listener or default listener.");
	case NBADQOSPARAM:
		return
		    ("QOS values specified are outside the range supported by the NS provider.");
	case NBADQOSTYPE:
		return
		    ("QOS structure type specified is not supported by the NS provider.");
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

static t_uscalar_t
tp_get_state(struct tp *tp)
{
	return (tp->state.state);
}

static t_uscalar_t
tp_set_state(struct tp *tp, t_uscalar_t newstate)
{
	t_uscalar_t oldstate = tp->state.state;

	if (newstate != oldstate) {
		tp->state.state = newstate;
		tp->CAPABILITY_ack.INFO_ack.CURRENT_state = newstate;
		mi_strlog(tp->oq, STRLOGST, SL_TRACE, "%s <- %s", tp_statename(newstate),
			  tp_statename(oldstate));
	}
	return (newstate);
}

static t_uscalar_t
tp_save_state(struct tp *tp)
{
	tp->oldstate = tp->state;
	return ((tp->oldstate.state = tp_get_state(tp)));
}

static t_uscalar_t
tp_restore_state(struct tp *tp)
{
	return (tp_set_state(tp, tp->oldstate.state));
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
 * tp_txprim: - trace log sent primitive
 * @tp: private structure (locked)
 * @msg: message to strip upon success
 * @mp: the TPI primitive being sent
 */
static int
tp_txprim(struct tp *tp, mblk_t *msg, mblk_t *mp)
{
	t_scalar_t prim = *(t_scalar_t *) mp->b_rptr;
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
	rtn = mi_strlog(tp->oq, priority, SL_TRACE, "<- %s", tp_primname(prim));
	stripmsg(msg, mp->b_cont);
	putnext(tp->oq, mp);
	return (rtn);
}

/**
 * np_txprim: - trace log sent primitive
 * @sc: private structure (locked)
 * @msg: message to strip upon success
 * @mp: the NPI primitive being sent
 */
static int
np_txprim(struct sc *sc, mblk_t *msg, mblk_t *mp)
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
	rtn = mi_strlog(sc->oq, priority, SL_TRACE, "%s ->", np_primname(prim));
	stripmsg(msg, mp->b_cont);
	putnext(sc->oq, mp);
	return (rtn);
}

/**
 * tp_error: - trace log error condition
 * @tp: private structure (locked)
 * @error: TPI error type
 * @func: function in which error was detected
 */
static int
tp_error(struct tp *tp, t_scalar_t error, const char *func)
{
	return mi_strlog(tp->oq, STRLOGNO, SL_TRACE, "ERROR: %s: %s: %s", func,
			 tp_errname(error), tp_strerror(error));
}

/**
 * np_error: - trace log error condition
 * @tp: private structure (locked)
 * @error: NPI error type
 * @func: function in which error was detected
 */
static int
np_error(struct sc *sc, np_long error, const char *func)
{
	return mi_strlog(sc->oq, STRLOGNO, SL_TRACE, "ERROR: %s: %s: %s", func,
			 np_errname(error), np_strerror(error));
}

/**
 * np_t_prim: - translate NPI primitive to TPI primitive
 * @prim: the primitive type to translate
 */
static t_scalar_t
np_t_prim(np_ulong prim)
{
	switch (prim) {
	case N_CONN_REQ:
		return (T_CONN_REQ);
	case N_CONN_RES:
		return (T_CONN_RES);
	case N_DISCON_REQ:
		return (T_DISCON_REQ);
	case N_DATA_REQ:
		return (T_DATA_REQ);
	case N_EXDATA_REQ:
		return (T_EXDATA_REQ);
	case N_INFO_REQ:
		return (T_INFO_REQ);
	case N_BIND_REQ:
		return (T_BIND_REQ);
	case N_UNBIND_REQ:
		return (T_UNBIND_REQ);
	case N_UNITDATA_REQ:
		return (T_UNITDATA_REQ);
	case N_OPTMGMT_REQ:
		return (T_OPTMGMT_REQ);
	case N_CONN_IND:
		return (T_CONN_IND);
	case N_CONN_CON:
		return (T_CONN_CON);
	case N_DISCON_IND:
		return (T_DISCON_IND);
	case N_DATA_IND:
		return (T_DATA_IND);
	case N_EXDATA_IND:
		return (T_EXDATA_IND);
	case N_INFO_ACK:
		return (T_INFO_ACK);
	case N_BIND_ACK:
		return (T_BIND_ACK);
	case N_ERROR_ACK:
		return (T_ERROR_ACK);
	case N_OK_ACK:
		return (T_OK_ACK);
	case N_UNITDATA_IND:
		return (T_UNITDATA_IND);
	case N_UDERROR_IND:
		return (T_UDERROR_IND);
	case N_DATACK_REQ:
		return (T_OPTMGMT_REQ);
	case N_DATACK_IND:
		return (T_OPTDATA_IND);
	case N_RESET_REQ:
		return (T_OPTMGMT_REQ);
	case N_RESET_IND:
		return (T_OPTDATA_IND);
	case N_RESET_RES:
		return (T_OPTMGMT_REQ);
	case N_RESET_CON:
		return (T_OPTDATA_IND);
	default:
		return (-1);
	}
}

noinline fastcall t_uscalar_t
np_t_serv_type(np_ulong type)
{
	t_uscalar_t tp_type = 0;

	if (type & N_CONS)
		tp_type |= T_COTS;
	if (type & N_CLNS)
		tp_type |= T_CLTS;
	return (tp_type);
}

noinline fastcall t_scalar_t
np_t_state(np_ulong state)
{
	switch (state) {
	case NS_UNBND:
		return (TS_UNBND);
	case NS_WACK_BREQ:
		return (TS_WACK_BREQ);
	case NS_WACK_UREQ:
		return (TS_WACK_UREQ);
	case NS_IDLE:
		return (TS_IDLE);
	case NS_WACK_OPTREQ:
		return (TS_WACK_OPTREQ);
	case NS_WACK_RRES:
		return (TS_DATA_XFER);
	case NS_WCON_CREQ:
		return (TS_WCON_CREQ);
	case NS_WRES_CIND:
		return (TS_WRES_CIND);
	case NS_WACK_CRES:
		return (TS_WACK_CREQ);
	case NS_DATA_XFER:
		return (TS_DATA_XFER);
	case NS_WCON_RREQ:
		return (TS_DATA_XFER);
	case NS_WRES_RIND:
		return (TS_DATA_XFER);
	case NS_WACK_DREQ6:
		return (TS_WACK_DREQ6);
	case NS_WACK_DREQ7:
		return (TS_WACK_DREQ7);
	case NS_WACK_DREQ9:
		return (TS_WACK_DREQ9);
	case NS_WACK_DREQ10:
		return (TS_WACK_DREQ9);
	case NS_WACK_DREQ11:
		return (TS_WACK_DREQ9);
	default:
		return (-1);
	}
}


/**
 * np_t_error: - translate NPI error to TPI error
 * @error: error type to translate
 */
static t_scalar_t
np_t_error(np_ulong error, np_ulong errno)
{
	switch (error) {
	case NBADADDR:
		return (TBADADDR);
	case NBADOPT:
		return (TBADOPT);
	case NACCESS:
		return (TACCES);
	case NNOADDR:
		return (TNOADDR);
	case NOUTSTATE:
		return (TOUTSTATE);
	case NBADSEQ:
		return (TBADSEQ);
	case NSYSERR:
		return (-errno);
	case NBADDATA:
		return (TBADDATA);
	case NBADFLAG:
		return (TBADFLAG);
	case NNOTSUPPORT:
		return (TNOTSUPPORT);
	case NBOUND:
		return (TADDRBUSY);
	case NBADQOSPARAM:
		return (TBADOPT);
	case NBADQOSTYPE:
		return (TBADOPT);
	case NBADTOKEN:
		return (TBADF);
	case NNOPROTOID:
		return (TNOADDR);
	default:
		return (error);
	}
}

/**
 * np_t_address: - convert an NPI SCCP address to a TPI SCCP address
 * @n_add: pointer to NPI address (or NULL)
 * @t_add: pointer to TPI address (not NULL when n_add not NULL)
 */
static void
np_t_address(struct sccp_addr *n_add, struct sockaddr_sccp *t_add)
{
	if (n_add == NULL || n_len < sizeof(*n_add))
		return;
	t_add->sccp_family = AF_SCCP;
	t_add->sccp_ni = n_add->ni;
	t_add->sccp_ri = n_add->ri;
	t_add->sccp_pc = n_add->pc;
	t_add->sccp_ssn = n_add->ssn;
	t_add->sccp_gtt = n_add->gtt;
	t_add->sccp_nai = n_add->nai;
	t_add->sccp_es = n_add->es;
	t_add->sccp_nplan = n_add->nplan;
	t_add->sccp_tt = n_add->tt;
	t_add->sccp_alen = n_add->alen;
	memcpy(t_add->sccp_addr, n_add->addr, t_add->sccp_alen);
	memset(t_add->sccp_addr + t_add->sccp_alen, 0, SCCP_MAX_ADDR_LENGTH - t_add->sccp_alen);
}

/**
 * np_t_options: - convert an NPI SCCP QOS structure to TPI options
 * @qos: pointer to NPI SCCP QOS structure
 * @opts: pointer to TPI SCCP options structure
 */
static void
np_t_options(N_qos_sccp_t * qos, struct t_sccp_options *opts)
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
 * TPI PROVIDER TO TPI USER ISSUED PRIMITIVES
 */

/**
 * t_conn_ind: - issue a T_CONN_IND primitive upstream
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @loc: destination address
 * @rem: source address
 * @qos: quality of service parameters
 * @seq: connection indication sequence number
 * @dp: user data
 */
noinline fastcall int
t_conn_ind(struct tp *tp, queue_t *q, mblk_t *msg, struct sccp_addr *loc, struct sccp_addr *rem,
	   N_qos_sel_conn_sccp_t *qos, t_uscalar_t seq, mblk_t *dp)
{
	struct T_conn_ind *p;
	mblk_t *mp;
	size_t olen = t_size_opts((N_qos_sccp_t *) qos);
	const size_t mlen = sizeof(*p) + tp->REMADDR_length + olen;

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(tp->oq, 0)))
		goto ebusy;
	if (rem) {
		np_t_address(rem, &tp->rem);
		tp->REMADDR_length = sizeof(tp->rem);
	}
	if (loc) {
		np_t_address(loc, &tp->loc);
		tp->LOCADDR_length = sizeof(tp->loc);
	}

	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_CONN_IND;
	p->SRC_length = tp->REMADDR_length;
	p->SRC_offset = tp->REMADDR_length ? sizeof(*p) : 0;
	p->OPT_length = olen;
	p->OPT_offset = olen ? p->SRC_offset + p->SRC_length : 0;
	p->SEQ_number = seq;
	mp->b_wptr += sizeof(*p);
	bcopy(rem, mp->b_wptr, p->SRC_length);
	mp->b_wptr += p->SRC_length;
	olen = t_build_opts((N_qos_sccp_t *) qos, mp->b_wptr, olen);
	mp->b_wptr += olen;
	mp->b_cont = dp;
	tp->state.coninds++;
	tp_set_state(tp, TS_WRES_CIND);
	tp_txprim(tp, msg, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * t_conn_con: - issue a T_CONN_CON primitive upstream
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @res: responding address
 * @qos: quality of service parameters
 * @dp: user data
 */
noinline fastcall int
t_conn_con(struct tp *tp, queue_t *q, mblk_t *msg, struct sccp_addr *res, N_qos_sel_conn_sccp_t *qos, mblk_t *dp)
{
	struct T_conn_con *p;
	mblk_t *mp;
	size_t olen = t_size_opts((N_qos_sccp_t *) qos);
	const size_t rlen = res ? sizeof(*res) + res->alen: 0;
	unsigned char *rptr = res ? (typeof(rptr)) &tp->res : NULL;
	const size_t mlen = sizeof(*p) + rlen + olen;

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (res) {
		np_t_address(res, &tp->res);
		tp->RES_length = sizeof(tp->res);
	}
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_CONN_CON;
	p->RES_length = rlen;
	p->RES_offset = rlen ? sizeof(*p) : 0;
	p->OPT_length = olen;
	p->OPT_offset = olen ? p->RES_offset + p->RES_length : 0;
	mp->b_wptr += sizeof(*p);
	bcopy(rptr, mp->b_wptr, p->RES_length);
	mp->b_wptr += p->RES_length;
	olen = t_build_opts((N_qos_sccp_t *) qos, mp->b_wptr, olen);
	mp->b_wptr += olen;
	mp->b_cont = dp;
	tp_set_state(tp, TS_DATA_XFER);
	tp_txprim(tp, msg, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * t_discon_ind: - issue a T_DISCON_IND primitive upstream
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @res: responding address
 * @orig: disconnect originator
 * @reason: disconnect reason
 * @seq: connection indication sequence number (or zero)
 * @dp: user data
 */
noinline fastcall int
t_discon_ind(struct tp *tp, queue_t *q, mblk_t *msg, struct sccp_addr *res, t_scalar_t reason,
	     t_scalar_t seq, mblk_t *dp)
{
	struct T_discon_ind *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (res) {
		np_t_address(res, &tp->res);
		tp->RES_length = sizeof(tp->res);
	}
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_DISCON_IND;
	p->DISCON_reason = reason;
	p->SEQ_number = seq;
	mp->b_wptr += sizeof(*p);
	mp->b_cont = dp;
	if (seq)
		tp->state.coninds--;
	tp_set_state(tp, tp->state.coninds ? TS_WRES_CIND : TS_IDLE);
	tp_txprim(tp, msg, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * t_data_ind: - issue a T_DATA_IND primitive upstream
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @flag: more flag
 * @dp: user data
 */
noinline fastcall int
t_data_ind(struct tp *tp, queue_t *q, mblk_t *msg, t_scalar_t flag, mblk_t *dp)
{
	struct T_data_ind *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(tp->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_DATA_IND;
	p->MORE_flag = flag ? 1 : 0;
	mp->b_wptr += sizeof(*p);
	mp->b_cont = dp;
	tp_txprim(tp, msg, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * t_exdata_ind: - issue a T_EXDATA_IND primitive upstream
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @flag: more flag
 * @dp: user data
 */
noinline fastcall int
t_exdata_ind(struct tp *tp, queue_t *q, mblk_t *msg, t_scalar_t flag, mblk_t *dp)
{
	struct T_exdata_ind *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(tp->oq, 1)))
		goto ebusy;
	mp->b_band = 1;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_EXDATA_IND;
	p->MORE_flag = flag ? 1 : 0;
	mp->b_wptr += sizeof(*p);
	mp->b_cont = dp;
	tp_txprim(tp, msg, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * t_info_ack: - issue a T_INFO_ACK primitive upstream
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (original T_INFO_REQ)
 */
noinline fastcall int
t_info_ack(struct tp *tp, queue_t *q, mblk_t *msg)
{
	struct T_info_ack *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	*p = tp->CAPABILITY_ack.INFO_ack;
	p->PRIM_type = T_INFO_ACK;
	mp->b_wptr += sizeof(*p);
	tp_txprim(tp, msg, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * t_bind_ack: - issue a T_BIND_ACK primitive upstream
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @add: bound address (or NULL)
 * @coninds: maximum number of outstanding connection indications
 */
noinline fastcall int
t_bind_ack(struct tp *tp, queue_t *q, mblk_t *msg, struct sccp_addr *add, t_uscalar_t coninds)
{
	struct T_bind_ack *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p) + sizeof(tp->add);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (add) {
		np_t_address(add, &tp->add);
		tp->ADDR_length = sizeof(tp->add);
	}
	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_BIND_ACK;
	p->ADDR_length = tp->ADDR_length;
	p->ADDR_offset = tp->ADDR_length ? sizeof(*p) : 0;
	p->CONIND_number = coninds;
	mp->b_wptr += sizeof(*p);
	bcopy(&tp->add, mp->b_wptr, p->ADDR_length);
	mp->b_wptr += p->ADDR_length;
	tp_set_state(tp, TS_IDLE);
	tp_txprim(tp, msg, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * t_error_ack: - issue a T_ERROR_ACK primitive upstream
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @prim: primitive in error
 * @error: negative UNIX error, positive TLI error
 */
noinline fastcall int
t_error_ack(struct tp *tp, queue_t *q, mblk_t *msg, t_uscalar_t prim, t_scalar_t error)
{
	struct T_error_ack *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);
	t_scalar_t state;

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_ERROR_ACK;
	p->ERROR_prim = prim;
	p->TLI_error = error < 0 ? TSYSERR : error;
	p->UNIX_error = error < 0 ? -error : 0;
	mp->b_wptr += sizeof(*p);
	tp_restore_state(tp);
	switch (tp_get_state(tp)) {
	case TS_WACK_BREQ:
		state = TS_UNBND;
		break;
	case TS_WACK_UREQ:
		state = TS_IDLE;
		break;
	case TS_WACK_OPTREQ:
		state = TS_IDLE;
		break;
	case TS_WACK_CREQ:
		state = TS_IDLE;
		break;
	case TS_WACK_CRES:
		if (tp->state.coninds == 0) {
			state = TS_IDLE;
			break;
		}
		state = TS_WRES_CIND;
		break;
	case TS_WACK_DREQ6:
		state = TS_WCON_CREQ;
		break;
	case TS_WACK_DREQ7:
		if (tp->state.coninds == 0) {
			state = TS_IDLE;
			break;
		}
		state = TS_WRES_CIND;
		break;
	case TS_WACK_DREQ9:
		state = TS_DATA_XFER;
		break;
	case TS_WACK_DREQ10:
		state = TS_WIND_ORDREL;
		break;
	case TS_WACK_DREQ11:
		state = TS_WREQ_ORDREL;
		break;
	default:
		state = tp_get_state(tp);
		break;
	}
	tp_set_state(tp, state);
	tp_txprim(tp, msg, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

noinline fastcall int
m_error(struct tp *tp, queue_t *q, mblk_t *msg, unsigned char rerror, unsigned char werror)
{
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(rerror) + sizeof(werror), BPRI_HI))))
		goto enobufs;
	DB_TYPE(mp) = M_ERROR;
	mp->b_wptr[0] = rerror;
	mp->b_wptr[1] = werror;
	mp->b_wptr += 2;
	sc_set_state(tp->sc, -1);
	tp_set_state(tp, -1);
	freemsg(msg);
	putnext(tp->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * t_error_reply: - process error conditions out of line
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @prim: primitive (or -1)
 * @error: negative UNIX error or positive TLI error (or zero (0))
 * @func: where error was detected
 */
noinline fastcall int
t_error_reply(struct tp *tp, queue_t *q, mblk_t *msg, t_scalar_t prim, t_scalar_t error, const char *func)
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
	case T_DATA_REQ:
	case T_EXDATA_REQ:
	case T_UNITDATA_REQ:
	case T_ORDREL_REQ:
	case T_OPTDATA_REQ:
		/* If the primitive in error is one for which no T_ERROR_ACK is possible, then the
		   TPI user is given an M_ERROR message. */
		mi_strlog(q, STRLOGDA, SL_TRACE, "ERROR: %s: %s: %s", func, tp_errname(error),
			  tp_strerror(error));
		return m_error(tp, q, msg, EPROTO, EPROTO);
	}
	switch (tp_get_state(tp)) {
	case TS_WACK_BREQ:
	case TS_WACK_UREQ:
	case TS_WACK_OPTREQ:
	case TS_WACK_CREQ:
	case TS_WACK_CRES:
	case TS_WACK_DREQ6:
	case TS_WACK_DREQ7:
	case TS_WACK_DREQ9:
	case TS_WACK_DREQ10:
	case TS_WACK_DREQ11:
		/* If the interface is in the waiting for acknowledgement state then the TPI user
		   has violated the interface protocol and deserves an M_ERROR message. */
		mi_strlog(q, STRLOGNO, SL_TRACE | SL_ERROR, "ERROR: %s: %s: %s", func,
			  tp_errname(error), tp_strerror(error));
		return m_error(tp, q, msg, EPROTO, EPROTO);
	}
	mi_strlog(q, STRLOGNO, SL_TRACE, "ERROR: %s: %s: %s", func, tp_errname(error),
		  tp_strerror(error));
	return t_error_ack(tp, q, msg, prim, error);
}

/**
 * t_ok_ack: - issue a T_OK_ACK primitive upstream
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @prim: correct prim
 */
noinline fastcall int
t_ok_ack(struct tp *tp, queue_t *q, mblk_t *msg, struct tp *accept, t_uscalar_t prim)
{
	struct T_ok_ack *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);
	t_uscalar_t state;

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_OK_ACK;
	p->CORRECT_prim = prim;
	mp->b_wptr += sizeof(*p);
	switch (tp_get_state(tp)) {
	case TS_WACK_BREQ:
	case TS_WACK_OPTREQ:
	case TS_WACK_DREQ6:
	case TS_WACK_DREQ9:
	case TS_WACK_DREQ10:
	case TS_WACK_DREQ11:
		state = TS_IDLE;
		break;
	case TS_WACK_CREQ:
		state = TS_WCON_CREQ;
		break;
	case TS_WACK_UREQ:
		state = TS_UNBND;
		break;
	case TS_WACK_CRES:
		tp_set_state(accept, TS_DATA_XFER);
		if (tp != accept) {
			if (--tp->state.coninds)
				state = TS_WRES_CIND;
			else
				state = TS_IDLE;
		}
		break;
	case TS_WACK_DREQ7:
		state = --tp->state.coninds ? TS_WRES_CIND : TS_IDLE;
		break;
	default:
		state = tp_get_state(tp);
		break;
	}
	tp_set_state(tp, state);
	tp_txprim(tp, msg, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * t_unitdata_ind: - issue a T_UNITDATA_IND primitive upstream
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @rem: source address
 * @loc: destination address
 * @qos: quality of service parameters
 * @dp: user data
 */
noinline fastcall int
t_unitdata_ind(struct tp *tp, queue_t *q, mblk_t *msg, struct sccp_addr *rem, struct sccp_addr *loc,
	       N_qos_sel_data_sccp_t *qos, mblk_t *dp)
{
	struct T_unitdata_ind *p;
	mblk_t *mp;
	size_t olen = t_size_opts((N_qos_sccp_t *) qos);
	const size_t mlen = sizeof(*p) + sizeof(&tp->loc) + olen;

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(tp->oq, 0)))
		goto ebusy;
	if (rem) {
		np_t_address(rem, &tp->rem);
		tp->REMADDR_length = sizeof(&tp->rem);
	}
	if (loc) {
		np_t_address(loc, &tp->loc);
		tp->LOCADDR_length = sizeof(&tp->loc);
	}
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_UNITDATA_IND;
	p->SRC_length = tp->LOCADDR_length;
	p->SRC_offset = tp->LOCADDR_length ? sizeof(*p) : 0;
	p->OPT_length = olen;
	p->OPT_offset = olen ? p->SRC_offset + p->SRC_length : 0;
	mp->b_wptr += sizeof(*p);
	bcopy(loc, mp->b_wptr, p->SRC_length);
	mp->b_wptr += p->SRC_length;
	olen = t_build_opts((N_qos_sccp_t *) qos, mp->b_wptr, olen);
	mp->b_wptr += olen;
	mp->b_cont = dp;
	tp_txprim(tp, msg, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * t_uderror_ind: - issue a T_UDERROR_IND primitive upstream
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @rem: destination address (or NULL)
 * @qos: quality of service parameters
 * @error: error type
 * @dp: user data
 */
noinline fastcall int
t_uderror_ind(struct tp *tp, queue_t *q, mblk_t *msg, struct sccp_addr *rem,
	      N_qos_sel_data_sccp_t *qos, t_scalar_t error, mblk_t *dp)
{
	struct T_uderror_ind *p;
	mblk_t *mp;
	size_t olen = t_size_opts((N_qos_sccp_t *) qos);
	const size_t mlen = sizeof(*p) + sizeof(tp->rem) + olen;

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(tp->oq, 1)))
		goto ebusy;
	if (rem) {
		np_t_address(rem, &tp->rem);
		tp->REMADDR_length = sizeof(tp->rem);
	}
	mp->b_band = 1;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_UDERROR_IND;
	p->DEST_length = tp->REMADDR_length;
	p->DEST_offset = tp->REMADDR_length ? sizeof(*p) : 0;
	p->OPT_length = olen;
	p->OPT_offset = olen ? p->DEST_offset + p->DEST_length : 0;
	p->ERROR_type = error;
	mp->b_wptr += sizeof(*p);
	bcopy(rem, mp->b_wptr, p->DEST_length);
	mp->b_wptr += p->DEST_length;
	olen = t_build_opts((N_qos_sccp_t *) qos, mp->b_wptr, olen);
	mp->b_wptr += olen;
	mp->b_cont = dp;
	tp_txprim(tp, msg, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * t_datack_ind: - issue a T_OPTDATA_IND primitive upstream
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
noinline fastcall int
t_datack_ind(struct tp *tp, queue_t *q, mblk_t *msg)
{
	struct T_optdata_ind *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(tp->oq, 2)))
		goto ebusy;
	mp->b_band = 2;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_OPTDATA_IND;
	p->DATA_flag = T_SCCP_ACK;
	mp->b_wptr += sizeof(*p);
	tp->state.datacks--;
	tp_txprim(tp, msg, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * t_optmgmt_ack: - issue a T_OPTMGMT_ACK primitive upstream
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @qos: quality of service parameters
 * @flags: management flags
 */
noinline fastcall int
t_optmgmt_ack(struct tp *tp, queue_t *q, mblk_t *msg, N_qos_sccp_t * qos, t_scalar_t flags)
{
	struct T_optmgmt_ack *p;
	mblk_t *mp;
	size_t olen = t_size_opts(qos);
	const size_t mlen = sizeof(*p) + olen;

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_OPTMGMT_ACK;
	p->OPT_length = olen;
	p->OPT_offset = olen ? sizeof(*p) : 0;
	p->MGMT_flags = flags;
	mp->b_wptr += sizeof(*p);
	olen = t_build_opts(qos, mp->b_wptr, olen);
	mp->b_wptr += olen;
	if (tp_get_state(tp) == TS_WACK_OPTREQ)
		tp_set_state(tp, TS_IDLE);
	tp_txprim(tp, msg, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * t_ordrel_ind: - issue a T_ORDREL_IND primitive upstream
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @dp: user data
 */
noinline fastcall int
t_ordrel_ind(struct tp *tp, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	struct T_ordrel_ind *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(tp->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_ORDREL_IND;
	mp->b_wptr += sizeof(*p);
	mp->b_cont = dp;
	switch (__builtin_expect(tp_get_state(tp), TS_DATA_XFER)) {
	case TS_DATA_XFER:
		tp_set_state(tp, TS_WREQ_ORDREL);
		break;
	case TS_WIND_ORDREL:
		tp_set_state(tp, TS_IDLE);
		break;
	case TS_WACK_DREQ9:
		tp_set_state(tp, TS_WACK_DREQ11);
		break;
	case TS_WACK_DREQ11:
		tp_set_state(tp, TS_IDLE);
		break;
	}
	tp_txprim(tp, msg, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * t_optdata_ind: - issue a T_OPTDATA_IND primitive upstream
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @qos: quality of service parameters
 * @flag: data flags
 * @dp: user data
 */
noinline fastcall int
t_optdata_ind(struct tp *tp, queue_t *q, mblk_t *msg, N_qos_sccp_t * qos, t_scalar_t flag,
	      mblk_t *dp)
{
	struct T_optdata_ind *p;
	mblk_t *mp;
	size_t olen = t_size_opts(qos);
	const size_t mlen = sizeof(*p) + olen;
	unsigned char band = (flag & T_ODF_EX) ? 1 : 0;

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(tp->oq, band)))
		goto ebusy;
	mp->b_band = band;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_OPTDATA_IND;
	p->DATA_flag = flag;
	p->OPT_length = olen;
	p->OPT_offset = olen ? sizeof(*p) : 0;
	mp->b_wptr += sizeof(*p);
	olen = t_build_opts(qos, mp->b_wptr, olen);
	mp->b_wptr += olen;
	mp->b_cont = dp;
	tp_txprim(tp, msg, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * t_addr_ack: - issue a T_ADDR_ACK primitive upstream
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (orginal T_ADDR_REQ)
 */
noinline fastcall int
t_addr_ack(struct tp *tp, queue_t *q, mblk_t *msg)
{
	struct T_addr_ack *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p) + sizeof(&tp->add) + sizeof(&tp->res);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_ADDR_ACK;
	p->LOCADDR_length = tp->LOCADDR_length;
	p->LOCADDR_offset = tp->LOCADDR_length ? sizeof(*p) : 0;
	p->REMADDR_length = tp->REMADDR_length;
	p->REMADDR_offset = tp->REMADDR_length ? p->LOCADDR_offset + p->LOCADDR_length : 0;
	mp->b_wptr += sizeof(*p);
	bcopy(&tp->add, mp->b_wptr, p->LOCADDR_length);
	mp->b_wptr += p->LOCADDR_length;
	bcopy(&tp->res, mp->b_wptr, p->REMADDR_length);
	mp->b_wptr += p->REMADDR_length;
	tp_txprim(tp, msg, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * t_capability_ack: - issue a T_CAPABILITY_ACK primitive upstream
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (original T_CAPABILITY_REQ)
 * @type: message type of request
 * @flags: capability bits
 */
noinline fastcall int
t_capability_ack(struct tp *tp, queue_t *q, mblk_t *msg)
{
	struct T_capability_ack *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!pcmsg(DB_TYPE(msg)) && !bcanputnext(tp->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = DB_TYPE(msg);
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_CAPABILITY_ACK;
	p->CAP_bits1 = ((typeof(p)) msg->b_rptr)->CAP_bits1;
	p->INFO_ack = tp->CAPABILITY_ack.INFO_ack;
	p->INFO_ack.PRIM_type = T_INFO_ACK;
	p->ACCEPTOR_id = tp->TOKEN_value;
	mp->b_wptr += sizeof(*p);
	tp_txprim(tp, msg, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * t_inform_ind: - issue a T_OPTDATA_IND primitive upstream
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @reason: inform reason
 */
noinline fastcall int
t_inform_ind(struct tp *tp, queue_t *q, mblk_t *msg, t_uscalar_t orig, t_uscalar_t reason)
{
	struct T_optdata_ind *p;
	mblk_t *mp;
	struct t_opthdr *oh;
	size_t olen = 3 * (sizeof(*oh) + sizeof(t_scalar_t));
	const size_t mlen = sizeof(*p) + olen;

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(tp->oq, 2)))
		goto ebusy;
	mp->b_band = 2;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_OPTDATA_IND;
	p->DATA_flag = T_SCCP_INF;
	p->OPT_length = olen;
	p->OPT_offset = sizeof(*p);
	oh = (typeof(oh)) mp->b_wptr;
	oh->level = T_SS7_SCCP;
	oh->name = T_SCCP_INFORM_REASON;
	oh->len = sizeof(*oh) + sizeof(t_scalar_t);
	*(t_scalar_t *)(oh + 1) = reason;
	mp->b_wptr += oh->len;
	oh = (typeof(oh)) mp->b_wptr;
	oh->level = T_SS7_SCCP;
	oh->name = T_SCCP_PCLASS;
	oh->len = sizeof(*oh) + sizeof(t_scalar_t);
	*(t_scalar_t *)(oh + 1) = tp->opts.sccp.pclass;
	mp->b_wptr += oh->len;
	oh = (typeof(oh)) mp->b_wptr;
	oh->level = T_SS7_SCCP;
	oh->name = T_SCCP_CREDIT;
	oh->len = sizeof(*oh)+ sizeof(t_scalar_t);
	*(t_scalar_t *)(oh + 1) = tp->opts.sccp.credit;
	mp->b_wptr += oh->len;
	tp_set_state(tp, tp_get_state(tp));
	tp_txprim(tp, msg, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * t_reset_ind: - issue a T_OPTDATA_IND primitive upstream
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @orig: reset originator
 * @reason: reset reason
 */
noinline fastcall int
t_reset_ind(struct tp *tp, queue_t *q, mblk_t *msg, np_ulong orig, np_ulong reason)
{
	struct T_optdata_ind *p;
	mblk_t *mp;
	size_t olen = 2 * (sizeof(struct t_opthdr) + sizeof(t_scalar_t));
	const size_t mlen = sizeof(*p) + olen;
	struct t_opthdr *oh;

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_OPTDATA_IND;
	p->DATA_flag = T_SCCP_RST;
	p->OPT_length = olen;
	p->OPT_offset = olen ? sizeof(*p) : 0;
	mp->b_wptr += sizeof(*p);
	oh = (typeof(oh)) mp->b_wptr;
	oh->level = T_SS7_SCCP;
	oh->name = T_SCCP_RESET_ORIG;
	oh->len = sizeof(*oh) + sizeof(t_scalar_t);
	*(t_scalar_t *)(oh + 1) = orig;
	mp->b_wptr += sizeof(*oh) + sizeof(t_scalar_t);
	oh = (typeof(oh)) mp->b_wptr;
	oh->level = T_SS7_SCCP;
	oh->name = T_SCCP_RESET_REASON;
	oh->len = sizeof(*oh) + sizeof(t_scalar_t);
	*(t_scalar_t *)(oh + 1) = reason;
	mp->b_wptr += sizeof(*oh) + sizeof(t_scalar_t);
	tp_set_state(tp, NS_DATA_XFER);
	tp->state.resinds = 1;
	tp->state.datinds = 0;
	tp->state.datacks = 0;
	tp_txprim(tp, msg, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * t_reset_con: - issue a T_OPTDATA_IND primitive upstream
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
noinline fastcall int
t_reset_con(struct tp *tp, queue_t *q, mblk_t *msg)
{
	struct T_optdata_ind *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = T_OPTDATA_IND;
	p->DATA_flag = T_SCCP_RST;
	p->OPT_length = 0;
	p->OPT_offset = 0;
	mp->b_wptr += sizeof(*p);
	tp_set_state(tp, NS_DATA_XFER);
	tp->state.resacks = 0;
	tp->state.resinds = 0;
	tp->state.datinds = 0;
	tp->state.datacks = 0;
	tp_txprim(tp, msg, mp);
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
n_conn_req(struct sc *sc, queue_t *q, mblk_t *msg, np_ulong flags, const struct strbuf *dst,
	   const struct strbuf *qos, const mblk_t *dp)
{
	N_conn_req_t *p;
	mblk_t *mp;
	const size_t dlen = dst->len;
	const size_t qlen = qos->len;
	unsigned char *dptr = dst->buf;
	unsigned char *qptr = qos->buf;
	const size_t mlen = sizeof(*p) + dlen + qlen;

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(sc->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_CONN_REQ;
	p->DEST_length = dlen;
	p->DEST_offset = dlen ? sizeof(*p) : 0;
	p->CONN_flags = flags;
	p->QOS_length = qlen;
	p->QOS_offset = qlen ? p->DEST_offset + p->DEST_length : 0;
	mp->b_wptr += sizeof(*p);
	bcopy(dptr, mp->b_wptr, p->DEST_length);
	mp->b_wptr += p->DEST_length;
	bcopy(qptr, mp->b_wptr, p->QOS_length);
	mp->b_wptr += p->QOS_length;
	mp->b_cont = dp;
	sc_set_state(sc, NS_WCON_CREQ);
	np_txprim(sc, msg, mp);
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
	if (res) {
		sc->res = *res;
		sc->RES_length = sizeof(*res) + res->alen;
	} else
		sc->RES_length = 0;
	if (qos) {
		sc->qos = *qos;
		sc->QOS_length = sizeof(*qos);
	} else
		sc->QOS_length = 0;
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
	np_txprim(sc, msg, mp);
	sc->state.coninds--;
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
n_discon_req(struct sc *sc, queue_t *q, mblk_t *msg, struct sockaddr_sccp *res, np_ulong reason,
	     np_ulong seq, mblk_t *dp)
{
	N_discon_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p) + sizeof(sc->res) + SCCP_MAX_ADDR_LENGTH;

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(sc->oq, 0)))
		goto ebusy;
	if (res) {
		tp_n_address(res, &sc->res);
		sc->RES_length = sizeof(sc->res) + sc->res.alen;
	} else
		sc->RES_length = 0;
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
	np_txprim(sc, msg, mp);
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
	np_txprim(sc, msg, mp);
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
	np_txprim(sc, msg, mp);
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
	np_txprim(sc, msg, mp);
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
n_bind_req(struct sc *sc, queue_t *q, mblk_t *msg, struct sockaddr_sccp *add, np_ulong coninds, np_ulong flags,
		caddr_t pptr, size_t plen)
{
	N_bind_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p) + sizeof(sc->add) + sizeof(sc->pro);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(sc->oq, 0)))
		goto ebusy;
	if (add) {
		tp_n_address(add, &sc->add);
		sc->ADDR_length = sizeof(sc->add) + sc->add.alen;
	} else
		sc->ADDR_length = 0;
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
	np_txprim(sc, msg, mp);
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
	np_txprim(sc, msg, mp);
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
	if (dst) {
		bcopy(dst, &sc->rem, sizeof(*dst));
		sc->REMADDR_length = sizeof(*dst);
	} else
		sc->REMADDR_length = 0;
	if (qos) {
		bcopy(qos, &sc->qos, sizeof(*qos));
		sc->QOS_length = sizeof(*qos);
	} else
		sc->QOS_length = 0;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_UNITDATA_REQ;
	p->DEST_length = sc->REMADDR_length;
	p->DEST_offset = sc->REMADDR_length ? sizeof(*p) : 0;
	p->RESERVED_field[0] = sc->QOS_length;
	p->RESERVED_field[1] = sc->QOS_length ? p->DEST_offset + p->DEST_length : 0;
	mp->b_wptr += sizeof(*p);
	bcopy(dst, mp->b_wptr, p->DEST_length);
	mp->b_wptr += p->DEST_length;
	bcopy(qos, mp->b_wptr, p->RESERVED_field[0]);
	mp->b_wptr += p->RESERVED_field[0];
	mp->b_cont = dp;
	np_txprim(sc, msg, mp);
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
	np_txprim(sc, msg, mp);
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
	np_txprim(sc, msg, mp);
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
	np_txprim(sc, msg, mp);
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
	np_txprim(sc, msg, mp);
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
n_request_req(struct sc *sc, queue_t *q, mblk_t *msg, struct sockaddr_sccp *loc, np_ulong sref)
{
	N_request_req_t *p;
	mblk_t *mp;
	static const size_t mlen = sizeof(*p) + sizeof(*loc) + sizeof(sc->qos);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;
	if (unlikely(!bcanputnext(sc->oq, 0)))
		goto ebusy;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_REQUEST_REQ;
	p->SRC_length = loc ? sizeof(*loc) : 0;
	p->SRC_offset = loc ? sizeof(*p) : 0;
	p->SRC_lref = sref;
	p->QOS_length = sizeof(sc->qos);
	p->QOS_offset = p->SRC_offset + p->SRC_length;
	mp->b_wptr += sizeof(*p);
	bcopy(loc, mp->b_wptr, p->SRC_length);
	mp->b_wptr += p->SRC_length;
	bcopy(&sc->qos, mp->b_wptr, p->QOS_length);
	mp->b_wptr += p->QOS_length;
	sc_set_state(sc, NS_WCON_CREQ);
	np_txprim(sc, msg, mp);
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
	np_txprim(sc, msg, mp);
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
	np_txprim(sc, msg, mp);
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
	np_txprim(sc, msg, mp);
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
	np_txprim(sc, msg, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/*
 * TPI User to TPI Provider Primitives
 */

/**
 * tp_conn_req: - process T_CONN_REQ primitive
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @mp: the T_CONN_REQ primitive
 *
 * There is a mismatch between TPI and NPI here.  TPI returns T_ERROR_ACK or T_OK_ACK and then
 * T_CONN_CON or T_DISCON_IND wherease NPI only returns N_ERROR_ACK or N_CONN_CON or N_DISCON_IND
 * (that is no N_OK_ACK).  There are two possibilities for this mismatch: postpone returning
 * T_OK_ACK until N_CONN_CON is received, or return T_OK_ACK immediately and issue T_DISCON_IND
 * if an N_ERROR_ACK arrives.  Another approach is a hybrid of the two: issue the N_CONN_REQ with
 * the private structure unlocked and then reaquire the lock and see whether an N_ERROR_ACK has
 * already been processed: if so, return, otherwise issue the T_OK_ACK and await N_CONN_CON or
 * N_DISCON_IND.  In the event that the N_ERROR_ACK was delayed, if an N_ERROR_ACK is received in
 * the TS_WCON_CREQ state, change it to a T_DISCON_IND.
 */
noinline fastcall int
tp_conn_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_conn_req *p = (typeof(p)) mp->b_rptr;
	struct {
		struct sccp_addr add;
		unsigned char addr[SCCP_MAX_ADDR_LENGTH];
	} rem __attribute__((__packed__));
	N_qos_sel_conn_sccp_t qos = { N_QOS_SEL_CONN_SCCP, };
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	if (unlikely(tp_get_state(tp) != TS_IDLE))
		goto outstate;
	if (unlikely(!MBLKIN(mp, p->DEST_offset, p->DEST_length)))
		goto badaddr;
	if (p->DEST_length == 0)
		goto noaddr;
	if (p->DEST_length < sizeof(tp->rem))
		goto badaddr;
	bcopy(mp->b_wptr + p->DEST_offset, &tp->rem, sizeof(tp->rem));
	if (unlikely(!MBLKIN(mp, p->OPT_offset, p->OPT_length)))
		goto badopt;
	if ((err = tp_n_address(&tp->rem, &rem)))
		goto error;
	if ((err = t_read_opts(tp, (N_qos_sccp_t *) &qos,
			       mp->b_rptr + p->OPT_offset, p->OPT_length)))
		goto error;
	if (mp->b_cont && msgsize(mp->b_cont) > tp->CAPABILITY_ack.INFO_ack.CDATA_size)
		goto baddata;
	tp_set_state(tp, TS_WACK_CREQ);
	if ((err = n_conn_req(tp->sc, q, NULL, 0, &dst, &qos, mp->b_cont)))
		goto error;
#if 0
	if (likely(tp_get_state(tp) == TS_WACK_CREQ)) {
		if ((err = t_ok_ack(tp, q, mp, T_CONN_REQ)))
			return (err);
	} else
#endif
		freemsg(mp);
	return (0);
      baddata:
	err = TBADDATA;
	goto error;
      badopt:
	err = TBADOPT;
	goto error;
      noaddr:
	err = TNOADDR;
	goto error;
      badaddr:
	err = TBADADDR;
	goto error;
      outstate:
	err = TOUTSTATE;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * tp_conn_res: - process T_CONN_RES primitive
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @mp: the T_CONN_RES primitive
 */
noinline fastcall int
tp_conn_res(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_conn_res *p = (typeof(p)) mp->b_rptr;
	struct strbuf res = { -1, 0, NULL };
	struct strbuf qos = { -1, 0, NULL };
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	if (unlikely(tp_get_state(tp) != TS_WRES_CIND))
		goto outstate;
	if (unlikely(!MBLKIN(mp, p->OPT_offset, p->OPT_length)))
		goto badopt;
	/* FIXME: format options into qos parameters */
	if (mp->b_cont && msgsize(mp->b_cont) > tp->CAPABILITY_ack.INFO_ack.CDATA_size)
		goto baddata;
	tp_set_state(tp, TS_WACK_CRES);
	return n_conn_res(tp->sc, q, mp, p->ACCEPTOR_id, 0, &res, &qos, p->SEQ_number, mp->b_cont);
      baddata:
	err = TBADDATA;
	goto error;
      badopt:
	err = TBADOPT;
	goto error;
      outstate:
	err = TOUTSTATE;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * tp_discon_req: - process T_DISCON_REQ primitive
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @mp: the T_DISCON_REQ primitive
 *
 * Note that T_DISCON_REQ has no direct way of specifying the responding address.  The responding
 * address might be set using a TPI option, say, T_SCCP_DISCON_ADD, before issuing the T_DISCON_REQ
 * primitive.  Also, T_DISCON_REQ has no direct way of specifying the disconnect reason.  The
 * disconnect reason might be set using a TPI option, say, T_SCCP_DISCON_REASON, before issuing the
 * T_DISCON_REQ primitive.
 */
noinline fastcall int
tp_discon_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_discon_req *p = (typeof(p)) mp->b_rptr;
	struct strbuf res = { -1, 0, NULL };
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	switch (__builting_expect(tp_get_state(tp), TS_DATA_XFER)) {
	case TS_DATA_XFER:
		tp_set_state(tp, TS_WACK_DREQ9);
		break;
	case TS_WIND_ORDREL:
		tp_set_state(tp, TS_WACK_DREQ10);
		break;
	case TS_WREQ_ORDREL:
		tp_set_state(tp, TS_WACK_DREQ11);
		break;
	case TS_WCON_CREQ:
		tp_set_state(tp, TS_WACK_DREQ6);
		break;
	case TS_WRES_CIND:
		tp_set_state(tp, TS_WACK_DREQ7);
		break;
	default:
		goto outstate;
	}
	if (mp->b_cont && msgsize(mp->b_cont) > tp->CAPABILITY_ack.INFO_ack.DDATA_size)
		goto baddata;
	/* FIXME: set reason, fake out responding address */
	return n_discon_req(tp->sc, q, mp, 0, &res, p->SEQ_number, mp->b_cont);
      baddata:
	err = TBADDATA;
	goto error;
      outstate:
	err = TOUTSTATE;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * tp_data_req: - process T_DATA_REQ primitive
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @mp: the T_DATA_REQ primitive
 */
noinline fastcall int
tp_data_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_data_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	switch (__builtin_expect(tp_get_state(tp), TS_DATA_XFER)) {
	case TS_DATA_XFER:
	case TS_WREQ_ORDREL:
		break;
	case TS_IDLE:
		goto discard;
	default:
		goto outstate;
	}
	if (tp->state.edatack)
		/* The TS user must wait until the expedited data request is acknowledged before
		   any other normal data or expedited data primitives are issued. */
		goto outstate;
	if (p->MORE_flag & ~(0x01))
		goto badflag;
	if (mp->b_cont && msgsize(mp->b_cont) > tp->CAPABILITY_ack.INFO_ack.TIDU_size)
		goto baddata;
	if (mp->b_cont && msgsize(mp->b_cont) > tp->CAPABILITY_ack.INFO_ack.TSDU_size)
		goto baddata;
	return n_data_req(tp->sc, q, mp, p->MORE_flag ? N_MORE_DATA_FLAG : 0, mp->b_cont);
      baddata:
	err = TBADDATA;
	goto error;
      badflag:
	err = TBADFLAG;
	goto error;
      outstate:
	err = TOUTSTATE;
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
 * tp_exdata_req: - process T_EXDATA_REQ primitive
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @mp: the T_EXDATA_REQ primitive
 */
noinline fastcall int
tp_exdata_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_exdata_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	switch (__builtin_expect(tp_get_state(tp), TS_DATA_XFER)) {
	case TS_DATA_XFER:
	case TS_WREQ_ORDREL:
		break;
	case TS_IDLE:
		goto discard;
	default:
		goto outstate;
	}
	if (tp->state.edatack)
		/* The TS user must wait until the expedited data request is acknowledged before
		   any other normal data or expedited data primitives are issued. */
		goto outstate;
	if (p->MORE_flag & ~(0x01))
		goto badflag;
	if (p->MORE_flag & 0x01)
		/* Note that the NPI cannot handle segmentation of expedited data and it is not
		   permitted to use the "more" flag with expedited data. */
		goto notsupport;
	if (mp->b_cont && msgsize(mp->b_cont) > tp->CAPABILITY_ack.INFO_ack.TIDU_size)
		goto baddata;
	if (mp->b_cont && msgsize(mp->b_cont) > tp->CAPABILITY_ack.INFO_ack.ETSDU_size)
		goto baddata;
	if (unlikely(p->MORE_flag))
		goto badflag;
	tp->state.edatack = 1;
	return n_exdata_req(tp->sc, q, mp, mp->b_cont);
      baddata:
	err = TBADDATA;
	goto error;
      notsupport:
	err = TNOTSUPPORT;
	goto error;
      badflag:
	err = TBADFLAG;
	goto error;
      outstate:
	err = TOUTSTATE;
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
 * tp_info_req: - process T_INFO_REQ primitive
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @mp: the T_INFO_REQ primitive
 */
noinline fastcall int
tp_info_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	tp->state.pending = mp;
	return n_info_req(tp->sc, q, NULL);
}

/**
 * tp_bind_req: - process T_BIND_REQ primitive
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @mp: the T_BIND_REQ primitive
 */
noinline fastcall int
tp_bind_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_bind_req *p = (typeof(p)) mp->b_rptr;
	struct sockaddr_sccp *add;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	if (unlikely(tp_get_state(tp) != TS_UNBND))
		goto outstate;
	if (unlikely(!MBLKIN(mp, p->ADDR_offset, p->ADDR_length)))
		goto badaddr;
	if (p->ADDR_length) {
		if (p->ADDR_length < sizeof(tp->add))
			goto badaddr;
		add = (typeof(add))(mp->b_rptr + p->ADDR_offset);
		bcopy(add, &tp->add, sizeof(tp->add));
		add = &tp->add;
	} else
		add = NULL;
	tp_set_state(tp, TS_WACK_BREQ);
	/* FIXME: handle flags and protocol ids */
	return n_bind_req(tp, q, mp, (caddr_t) (mp->b_rptr + p->ADDR_offset),
			  p->ADDR_length, p->CONIND_number, 0, NULL, 0);
      badaddr:
	err = TBADADDR;
	goto error;
      outstate:
	err = TOUTSTATE;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * tp_unbind_req: - process T_UNBIND_REQ primitive
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @mp: the T_UNBIND_REQ primitive
 */
noinline fastcall int
tp_unbind_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_unbind_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	if (unlikely(tp_get_state(tp) != TS_IDLE))
		goto outstate;
	tp_set_state(tp, TS_WACK_UREQ);
	return n_unbind_req(tp->sc, q, mp);
      outstate:
	err = TOUTSTATE;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * tp_unitdata_req: - process T_UNITDATA_REQ primitive
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @mp: the T_UNITDATA_REQ primitive
 */
noinline fastcall int
tp_unitdata_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_unitdata_req *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr rem, *rptr;
	N_qos_sel_data_sccp_t qos, *qptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	switch (__builtin_expect(tp_get_state(tp), TS_IDLE)) {
	case NS_IDLE:
		if (p->DEST_length == 0)
			goto noaddr;
		break;
	case NS_DATA_XFER:
		/* Why not? If connection oriented wants to use this primitive, let it.  But ignore
		   the destination address. */
		break;
	default:
		goto outstate;
	}
	if (likely(p->DEST_length != 0)) {
		if (unlikely(!MBLKIN(mp, p->DEST_offset, p->DEST_length)))
			goto badaddr;
		if (unlikely(p->DEST_offset < sizeof(*p)))
			goto badaddr;
		if (unlikely(p->DEST_length < sizeof(struct sockaddr_sccp)))
			goto badaddr;
		bcopy(mp->b_rptr + p->DEST_offset, &tp->rem, sizeof(tp->rem));
		rptr = &rem;
	} else
		rptr = NULL;
	if (unlikely(p->OPT_length != 0)) {
		if (unlikely(!MBLKIN(mp, p->OPT_offset, p->OPT_length)))
			goto badopt;
		if (unlikely(p->OPT_offset < sizeof(*p)))
			goto badopt;
		if (unlikely(p->OPT_length < sizeof(struct t_opthdr) + sizeof(t_scalar_t)))
			goto badopt;
		qptr = &qos;
	} else
		qptr = NULL;
	if (unlikely(!MBLKIN(mp, p->OPT_offset, p->OPT_length)))
		goto badopt;
	t_read_opts(tp, (N_qos_sccp_t *)qptr, mp->b_rptr + p->OPT_offset, p->OPT_length);
	if (mp->b_cont && msgsize(mp->b_cont) > tp->CAPABILITY_ack.INFO_ack.TIDU_size)
		goto baddata;
	if (mp->b_cont && msgsize(mp->b_cont) > tp->CAPABILITY_ack.INFO_ack.TSDU_size)
		goto baddata;
	return n_unitdata_req(tp->sc, q, mp, (caddr_t) (mp->b_rptr + p->DEST_offset),
			      p->DEST_length, mp->b_cont);
      baddata:
	err = TBADDATA;
	goto error;
      badopt:
	err = TBADOPT;
	goto error;
      badaddr:
	err = TBADADDR;
	goto error;
      outstate:
	err = TOUTSTATE;
	goto error;
      noaddr:
	err = TNOADDR;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * tp_optmgmt_req: - process T_OPTMGMT_REQ primitive
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @mp: the T_OPTMGMT_REQ primitive
 */
noinline fastcall int
tp_optmgmt_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_optmgmt_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	if (unlikely(!MBLKIN(mp, p->OPT_offset, p->OPT_length)))
		goto badopt;
	if (tp_get_state(tp) == NS_IDLE)
		tp_set_state(tp, TS_WACK_OPTREQ);
	goto notsupport;
	/* FIXME: If we are reading current or default options, we need to issue an N_INFO_REQ and
	   mark the response as for T_OPTMGMT_ACK.  If we are negiotiating options, then we need to 
	   issue an N_OPTMGMT_REQ and mark the response as for T_OPTMGMT_ACK. */
      notsupport:
	err = TNOTSUPPORT;
	goto error;
      badopt:
	err = TBADOPT;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * tp_ordrel_req: - process T_ORDREL_REQ primitive
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @mp: the T_ORDREL_REQ primitive
 */
noinline fastcall int
tp_ordrel_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_ordrel_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	switch (__builtin_expect(tp_get_state(tp), TS_DATA_XFER)) {
	case TS_IDLE:
		/* Note: If the interface is in the TS_IDLE state when the provider receives the
		   T_ORDREL_REQ primitive, then the TS provider should discard the request without
		   generating a fatal error. */
		goto discard;
	case TS_DATA_XFER:
		tp_set_state(tp, TS_WIND_ORDREL);
		break;
	case TS_WREQ_ORDREL:
		tp_set_state(tp, TS_IDLE);
		break;
	default:
		goto outstate;
	}
	goto notsupport;
      notsupport:
	err = TNOTSUPPORT;
	goto error;
      outstate:
	err = TOUTSTATE;
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
 * tp_inform_req: - process T_OPTDATA_REQ primitive
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @mp: the T_OPTDATA_REQ primitive
 *
 * The N-INFORM request primitive is implemented using the T_OPTDATA_REQ primitive.
 */
noinline fastcall int
tp_inform_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_optdata_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	switch (__builtin_expect(tp_get_state(tp), TS_DATA_XFER)) {
	case TS_IDLE:
		goto discard;
	case TS_DATA_XFER:
		break;
	default:
		goto outstate;
	}
	if (p->DATA_flag & ~(T_SCCP_INF))
		goto badflag;
	return n_inform_req(tp->sc, q, mp, tp->INFORM_reason);
      badflag:
	err = TBADFLAG;
	goto error;
      outstate:
	err = TOUTSTATE;
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
 * tp_reset_req: = process T_OPTDATA_REQ primitive
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @mp: the T_OPTDATA_REQ primitive
 *
 * The N-RESET request primitive is implemented using the T_OPTDATA_REQ primitive.
 */
noinline fastcall int
tp_reset_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_optdata_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	switch (__builtin_expect(tp_get_state(tp), TS_DATA_XFER)) {
	case TS_IDLE:
		/* Note: If the interface is in the NS_IDLE state when the provider receives the
		   N_RESET_REQ primitive, then the NS provider should discard the message without
		   generating an error. */
		goto discard;
	case TS_DATA_XFER:
		break;
	default:
		goto outstate;
	}
	if (tp->state.resacks)
		/* If there are outstanding reset requests, we are out of state. */
		goto outstate;
	t_read_opts(tp, NULL, mp->b_rptr + p->OPT_offset, p->OPT_length);
	tp->state.resacks = 1;
	tp_set_state(tp, NS_DATA_XFER);
	return n_reset_req(tp->sc, q, mp, tp->RESET_reason);
      outstate:
	err = TOUTSTATE;
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
 * tp_reset_res: = process T_OPTDATA_REQ primitive
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @mp: the T_OPTDATA_REQ primitive
 *
 * The N-RESET response primitive is implemented using the T_OPTDATA_REQ primitive.
 */
noinline fastcall int
tp_reset_res(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_optdata_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	switch (__builtin_expect(tp_get_state(tp), TS_DATA_XFER)) {
	case TS_IDLE:
		/* Note: If the interface is in the NS_IDLE state when the provider receives the
		   N_RESET_RES primitive, then the NS provider should discard the message without
		   generating an error. */
		goto discard;
	case TS_DATA_XFER:
		break;
	default:
		goto outstate;
	}
	if (tp->state.resinds <= 0)
		goto outstate;
	tp->state.resinds = 0;
	tp_set_state(tp, TS_DATA_XFER);
	return n_reset_res(tp->sc, q, mp);
      outstate:
	err = TOUTSTATE;
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














/**
 * tp_optdata_req: - process T_OPTDATA_REQ primitive
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @mp: the T_OPTDATA_REQ primitive
 */
noinline fastcall int
tp_optdata_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_optdata_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	if (p->DATA_flag & T_SCCP_INF)
		return tp_inform_req(tp, q, mp);
	if (p->DATA_flag & T_SCCP_RST) {
		if (tp->state.resinds)
			return tp_reset_res(tp, q, mp);
		else
			return tp_reset_req(tp, q, mp);
	}
	switch (__builtin_expect(tp_get_state(tp), TS_DATA_XFER)) {
	case TS_IDLE:
		/* Note: If the interface is in the TS_IDLE state when the provider receives the
		   T_OPTDATA_REQ primitive, then the TS provider should discard the message without
		   generating an error. */
		goto discard;
	case TS_DATA_XFER:
	case TS_WREQ_ORDREL:
		break;
	default:
		goto outstate;
	}
	if (tp->state.edatack)
		/* The TS user must wait until the expedited data request is acknowledged before
		   any other normal data or expedited data primitives are issued. */
		goto outstate;
	if (unlikely(!MBLKIN(mp, p->OPT_offset, p->OPT_length)))
		goto badopt;
	if (p->DATA_flag & ~(T_ODF_MORE | T_ODF_EX | T_SCCP_RST | T_SCCP_INF))
		goto badflag;
	if (p->DATA_flag & T_SCCP_RST) {
		if (p->DATA_flag & (T_ODF_MORE | T_ODF_EX | T_SCCP_INF))
			goto notsupport;
		if (tp->state.resacks)
			goto outstate;
		tp->state.resacks = 1;
		return n_reset_req(tp->sc, q, mp, tp->RESET_reason);
	}
	if (p->DATA_flag & T_SCCP_INF) {
		if (p->DATA_flag & (T_ODF_MORE | T_ODF_EX | T_SCCP_RST))
			goto notsupport;
		return n_inform_req(tp->sc, q, mp, tp->INFORM_reason);
	}
	/* XXX: options must be set separately by issuing M_PROTO type N_OPTMGMT_REQ 
	   primitives first. */
	if (mp->b_cont && msgsize(mp->b_cont) > tp->CAPABILITY_ack.INFO_ack.TIDU_size)
		goto baddata;
	if (p->DATA_flag & T_ODF_EX) {
		if (p->DATA_flag & T_ODF_MORE)
			/* Note that the NPI cannot handle segmentation of expedited data and it is 
			   not permitted to use the "more" flag with expedited data. */
			goto notsupport;
		if (mp->b_cont && msgsize(mp->b_cont) > tp->CAPABILITY_ack.INFO_ack.ETSDU_size)
			goto baddata;
		return n_exdata_req(tp->sc, q, mp, mp->b_cont);
	}
	if (mp->b_cont && msgsize(mp->b_cont) > tp->CAPABILITY_ack.INFO_ack.TSDU_size)
		goto baddata;
	return n_data_req(tp->sc, q, mp,
			  (p->DATA_flag & T_ODF_MORE) ? N_MORE_DATA_FLAG : 0,
			  mp->b_cont);
      notsupport:
	err = TNOTSUPPORT;
	goto error;
      baddata:
	err = TBADDATA;
	goto error;
      badflag:
	err = TBADFLAG;
	goto error;
      badopt:
	err = TBADOPT;
	goto error;
      outstate:
	err = TOUTSTATE;
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
 * tp_addr_req: - process T_ADDR_REQ primitive
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @mp: the T_ADDR_REQ primitive
 */
noinline fastcall int
tp_addr_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	tp->state.pending = mp;
	return n_info_req(tp->sc, q, NULL);
}

/**
 * tp_capability_req: - process T_CAPABILITY_REQ primitive
 * @tp: transport provider private structure (locked)
 * @q: active queue
 * @mp: the T_CAPABILITY_REQ primitive
 */
noinline fastcall int
tp_capability_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_capability_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	tp->state.pending = mp;
	return n_info_req(tp->sc, q, NULL);
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * tp_other_req: - process unknown primitive
 * @tp: transport provider private structure
 * @q: active queue
 * @mp: the unknown primitive
 */
noinline fastcall int
tp_other_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	t_scalar_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	err = -EPROTO;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

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
	return m_error(sc->tp, q, msg, EPROTO, EPROTO);
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
	/* FIXME: convert connect flags and QOS parameters into options */
	return t_conn_ind(sc->tp, q, mp, loc, rem, qos, p->SEQ_number, mp->b_cont);
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
	struct sockaddr_sccp *res;
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
	/* FIXME: convert QOS parameters and connect flags into options */
	/* FIXME: QOS parameters are converted but not connect flags! */
	return t_conn_con(sc->tp, q, mp, res, qos, mp->b_cont);
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
	struct sockaddr_sccp *res;
	int err;

	sc_set_state(sc, NS_IDLE);
	/* A wrinkle of the impeadance mismatch between NPI and TPI is that if we
	   have not yet issued the T_OK_ACK in response to a T_CONN_REQ and now
	   receive an N_DISCON_IND, we need to issue the T_OK_ACK before issuing the 
	   T_DISCON_IND to meet the TPI user's expectations. */
	if (tp_get_state(sc->tp) == TS_WACK_CREQ)
		if ((err = t_ok_ack(sc->tp, q, NULL, T_CONN_REQ)))
			goto error;
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
	return t_discon_ind(sc->tp, q, mp, res, p->DISCON_reason, p->SEQ_number, mp->b_cont);
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
	return t_data_ind(sc->tp, q, mp, p->DATA_xfer_flags & N_MORE_DATA_FLAG, mp->b_cont);
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
	return t_exdata_ind(sc->tp, q, mp, 0, mp->b_cont);
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
	struct tp *tp = sc->tp;
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

	tp->CAPABILITY_ack.PRIM_type = T_CAPABILITY_ACK;
	tp->CAPABILITY_ack.CAP_bits1 |= TC1_INFO;
	tp->CAPABILITY_ack.INFO_ack.PRIM_type = T_INFO_ACK;
	tp->CAPABILITY_ack.INFO_ack.TSDU_size = p->NSDU_size;
	tp->CAPABILITY_ack.INFO_ack.ETSDU_size = p->ENSDU_size;
	tp->CAPABILITY_ack.INFO_ack.CDATA_size = p->CDATA_size;
	tp->CAPABILITY_ack.INFO_ack.DDATA_size = p->DDATA_size;
	tp->CAPABILITY_ack.INFO_ack.ADDR_size = p->ADDR_size;
	tp->CAPABILITY_ack.INFO_ack.OPT_size = -1;
	tp->CAPABILITY_ack.INFO_ack.TIDU_size = p->NIDU_size;
	tp->CAPABILITY_ack.INFO_ack.SERV_type = np_t_serv_type(p->SERV_type);
	tp->CAPABILITY_ack.INFO_ack.CURRENT_state = np_t_state(p->CURRENT_state);
	tp->CAPABILITY_ack.INFO_ack.PROVIDER_flag = 0;

	if (p->ADDR_length) {
		struct sccp_addr *add;

		add = (typeof(add)) (mp->b_rptr + p->ADDR_offset);
		bcopy(add, &sc->add, p->ADDR_length);
		sc->ADDR_length = p->ADDR_length;
		add = &sc->add;
		np_t_address(add, &tp->add);
		tp->ADDR_length = sizeof(tp->add);
	}
	if (p->QOS_length) {
		N_qos_sel_info_sccp_t *qos;

		qos = (typeof(qos)) (mp->b_rptr + p->QOS_offset);
		bcopy(qos, &sc->qos, p->QOS_length);
		sc->QOS_length = p->QOS_length;
		qos = &sc->qos;
		np_t_options((N_qos_sccp_t *) qos, &tp->opts);
	}
	if (p->QOS_range_length) {
		N_qos_range_info_sccp_t *qor;

		qor = (typeof(qor)) (mp->b_rptr + p->QOS_range_offset);
		bcopy(qor, &sc->qor, p->QOS_range_length);
		sc->QOS_range_length = p->QOS_length;
		qor = &sc->qor;
		np_t_options((N_qos_sccp_t *) qor, &tp->opts);
	}
	if (p->PROTOID_length) {
		np_ulong *pro;

		pro = (typeof(pro)) (mp->b_rptr + p->PROTOID_offset);
		bcopy(pro, &sc->pro, p->PROTOID_length);
		sc->PROTOID_length = p->PROTOID_length;
		pro = &sc->pro;
	}

	if ((msg = XCHG(&sc->tp->state.pending, NULL))) {
		switch (*(t_scalar_t *)mp->b_rptr) {
		case T_ADDR_REQ:
			return t_addr_ack(sc->tp, q, msg);
		case T_INFO_REQ:
			return t_info_ack(sc->tp, q, msg);
		case T_CAPABILITY_REQ:
			return t_capability_ack(sc->tp, q, msg);
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
	return t_bind_ack(sc->tp, q, mp, add, p->CONIND_number);
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
	return t_error_ack(sc->tp, q, mp, np_t_prim(prim), np_t_error(error));
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
	return t_ok_ack(sc->tp, q, mp, np_t_prim(prim));
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
	int err;

	switch (sc_get_state(sc), NS_IDLE) {
	case NS_IDLE:
	case NS_WACK_UREQ:
		break;
	default:
		goto discard;
	}
	/* XXX: what about destination address and options? */
	return t_unitdata_ind(sc, q, mp, (caddr_t) (mp->b_rptr + p->SRC_offset),
			      p->SRC_length, NULL, 0, mp->b_cont);
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
	int err;

	switch (__builtin_expect(sc_get_state(sc), NS_IDLE)) {
	case NS_IDLE:
	case NS_WACK_UREQ:
		break;
	default:
		goto outstate;
	}
	/* FIXME: convert error types */
	return t_uderror_ind(sc, s, mp, (caddr_t) (mp->b_rptr + p->DEST_offset),
			     p->DEST_length, NULL, 0, p->ERROR_type, mp->b_cont);
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
	return t_datack_ind(sc->tp, q, mp);
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
	return t_reset_ind(sc->tp, q, mp, p->RESET_orig, p->RESET_reason);
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
	return t_reset_con(sc->tp, q, mp);
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
	return t_uderror_ind(sc->tp, q, mp, rem, NULL, p->RETURN_cause, mp->b_cont);
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
	return t_inform_ind(sc->tp, q, mp, N_PROVIDER, p->REASON);
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
 * @tp: transport provider private structure (locked)
 * @q: active queue (write queue)
 * @mp: M_DATA or M_HPDATA message
 */
static inline fastcall int
sccp_w_data(struct tp *tp, queue_t *q, mblk_t *mp)
{
	int err;

	switch (__builtin_expect(tp_get_state(tp), TS_DATA_XFER)) {
	case TS_DATA_XFER:
		break;
	case TS_IDLE:
		goto discard;
	default:
		goto outstate;
	}
	if (unlikely(tp->state.edatack))
		/* The NS user must wait until the expedited data request is acknowledged before
		   any other normal data or expedited data primitives are issued. */
		goto outstate;
	if (unlikely(msgsize(mp) > tp->CAPABILITY_ack.INFO_ack.TIDU_size))
		goto baddata;
	if (likely(mp->b_band)) {
		if (unlikely(msgsize(mp) > tp->CAPABILITY_ack.INFO_ack.ETSDU_size))
			goto baddata;
		return n_exdata_req(tp->sc, q, NULL, mp);
	}
	if (unlikely(msgsize(mp) > tp->CAPABILITY_ack.INFO_ack.TSDU_size))
		goto baddata;
	return n_data_req(tp->sc, q, NULL, (mp->b_flag & MSGDELIM) ? 0 : N_MORE_DATA_FLAG, mp);
      baddata:
	err = TBADDATA;
	goto error;
      outstate:
	err = TOUTSTATE;
	goto error;
      discard:
	err = 0;
	goto error;
      error:
	return t_error_reply(tp, q, mp, T_DATA_REQ, err, __FUNCTION__);
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
		return t_exdata_ind(sc->tp, q, mp, 0, mp);
	}
	if (unlikely(msgsize(mp) > sc->INFO_ack.NSDU_size))
		goto baddata;
	return t_data_ind(sc->tp, q, NULL, (mp->b_flag & MSGDELIM) ? 0 : N_MORE_DATA_FLAG, mp);
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
		err = sccp_w_data(&priv->tp, q, mp);
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
 * @tp: transport provider private structure (locked)
 * @q: active queue (write queue)
 * @mp: M_PROTO or M_PCPROTO message
 */
static fastcall int
sccp_w_proto(struct tp *tp, queue_t *q, mblk_t *mp)
{
	t_scalar_t prim;
	int err, level;

	tp_save_state(tp);
	if (unlikely(!MBLKIN(mp, 0, sizeof(prim))))
		goto efault;
	switch ((prim = *(typeof(prim) *) mp->b_rptr)) {
	case T_DATA_REQ:
	case T_EXDATA_REQ:
	case T_OPTDATA_REQ:
	case T_UNITDATA_REQ:
		level = STRLOGDA;
		break;
	default:
		level = STRLOGRX;
		break;
	}
	mi_strlog(q, level, SL_TRACE, "-> %s", tp_primname(prim));
	switch (prim) {
	case T_CONN_REQ:
		err = tp_conn_req(tp, q, mp);
		break;
	case T_CONN_RES:
		err = tp_conn_res(tp, q, mp);
		break;
	case T_DISCON_REQ:
		err = tp_discon_req(tp, q, mp);
		break;
	case T_DATA_REQ:
		err = tp_data_req(tp, q, mp);
		break;
	case T_EXDATA_REQ:
		err = tp_exdata_req(tp, q, mp);
		break;
	case T_INFO_REQ:
		err = tp_info_req(tp, q, mp);
		break;
	case T_BIND_REQ:
		err = tp_bind_req(tp, q, mp);
		break;
	case T_UNBIND_REQ:
		err = tp_unbind_req(tp, q, mp);
		break;
	case T_UNITDATA_REQ:
		err = tp_unitdata_req(tp, q, mp);
		break;
	case T_OPTMGMT_REQ:
		err = tp_optmgmt_req(tp, q, mp);
		break;
	case T_ORDREL_REQ:
		err = tp_ordrel_req(tp, q, mp);
		break;
	case T_OPTDATA_REQ:
		err = tp_optdata_req(tp, q, mp);
		break;
	case T_ADDR_REQ:
		err = tp_addr_req(tp, q, mp);
		break;
	case T_CAPABILITY_REQ:
		err = tp_capability_req(tp, q, mp);
		break;
	default:
		err = tp_other_req(tp, q, mp);
		break;
	}
      done:
	if (unlikely(err)) {
		tp_restore_state(tp);
		return t_error_reply(tp, q, mp, prim, err, tp_primname(prim));
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
		err = sccp_w_proto(&priv->tp, q, mp);
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
 * @tp: transport provider private structure (locked)
 * @q: active queue (write queue)
 * @mp: STREAMS message
 */
static fastcall int
sccp_w_msg(struct tp *tp, queue_t *q, mblk_t *mp)
{
	int err;

	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		err = sccp_w_data(tp, q, mp);
		break;
	case M_PROTO:
	case M_PCPROTO:
		err = sccp_w_proto(tp, q, mp);
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
	if (!pcmsg(DB_TYPE(mp)) && m_w_msg(q, mp)) {
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
				if (unlikely(sccp_w_msg(&priv->tp, q, mp))) {
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
	if (!pcmsg(DB_TYPE(mp)) && m_r_msg(q, mp)) {
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
	priv = (struct priv *) q->q_ptr;
	bzero(priv, sizeof(*priv));
	priv->cred = *credp;
	priv->tp.oq = RD(q);





	priv->tp.CAPABILITY_ack.INFO_ack.PRIM_type = 0;
	priv->tp.CAPABILITY_ack.INFO_ack.TSDU_size = -1;
	priv->tp.CAPABILITY_ack.INFO_ack.ETSDU_size = -2;
	priv->tp.CAPABILITY_ack.INFO_ack.CDATA_size = -2;
	priv->tp.CAPABILITY_ack.INFO_ack.DDATA_size = -2;
	priv->tp.CAPABILITY_ack.INFO_ack.ADDR_size = sizeof(struct sockaddr_sccp);
	priv->tp.CAPABILITY_ack.INFO_ack.OPT_size = -1;
	priv->tp.CAPABILITY_ack.INFO_ack.TIDU_size = -1;
	priv->tp.CAPABILITY_ack.INFO_ack.SERV_type = T_CLTS | T_COTS;
	priv->tp.CAPABILITY_ack.INFO_ack.CURRENT_state = TS_UNBND;
	priv->tp.CAPABILITY_ack.INFO_ack.PROVIDER_flag = 0;
	/* ... */
	priv->sc.oq = WR(q);





	priv->sc.INFO_ack.PRIM_type = 0;
	priv->sc.INFO_ack.NSDU_size = -1;
	priv->sc.INFO_ack.ENSDU_size = -2;
	priv->sc.INFO_ack.CDATA_size = -2;
	priv->sc.INFO_ack.DDATA_size = -2;
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
	.mi_minpsz = 1,
	.mi_maxpsz = INFPSZ,
	.mi_hiwat = 1,
	.mi_lowat = 0,
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

static struct streamtab sccp_tpiinfo = {
	.st_rdinit = &sccp_rdinit,
	.st_wrinit = &sccp_wrinit,
};

static struct fmodsw sccp_fmod = {
	.f_name = MOD_NAME,
	.f_str = &sccp_tpiinfo,
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
MODULE_PARM_DESC(modid, "Module ID for SCCP-TPI.  (0 for allocation.)");
#endif				/* LINUX */

/**
 * sccp_tpiinit: - initialize kernel module
 */
MODULE_STATIC int __init
sccp_tpiinit(void)
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
 * sccp_tpiterminate: - deinitialize kernel module
 */
MODULE_STATIC void __exit
sccp_tpiterminate(void)
{
	int err;

	if (unlikely((err = unregister_strmod(&sccp_fmod)) < 0))
		cmn_err(CE_WARN, "%s: could not unregister module: err = %d\n", MOD_NAME, -err);
	return;
}

#ifdef LINUX
module_init(sccp_tpiinit);
module_exit(sccp_tpiterminate);
#endif				/* LINUX */
