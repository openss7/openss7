/*****************************************************************************

 @(#) $RCSfile: xx25.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-05-07 16:01:41 $

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

 Last Modified $Date: 2008-05-07 16:01:41 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: xx25.c,v $
 Revision 0.9.2.1  2008-05-07 16:01:41  brian
 - added NLI X.25-PLP CONS and XX25 implemetnation

 *****************************************************************************/

#ident "@(#) $RCSfile: xx25.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-05-07 16:01:41 $"

static char const ident[] = "$RCSfile: xx25.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-05-07 16:01:41 $";

/*
 * This is XX25 implemented as a pushable module that pushes over an NPI
 * stream that interfaces to a Network Layer Interface (NLI) multiplex.  The
 * module simply converts from TPI (for XTI-X25) to NPI (for X.25).  This may
 * be used for two thing: XTI access for X.25 and CONS.
 */

/*
 * X25MOD - XX25 Module: This is an X.25 module that is pushed over a X.25-PLP
 * stream supporting the Network Provider Interface.  It implements the TPI as
 * it is used to support the OpenGroup XX25 XTI interface.  As it is not
 * expected that the XX25 interface will experience much use, I thought it
 * unnecessary to clutter the X.25-PLP drivers with supporting both an NPI and
 * a TPI interface.  Thus this module.  This module should be used in an
 * autopush(8) specification to be autopushed on a minor device number on the
 * X.25-PLP driver.
 */

#include <sys/os7/compat.h>

#if defined HAVE_TIHDR_H
#   include <tihdr.h>
#else
#   include <sys/tihdr.h>
#endif

#include <sys/npi.h>

#define XX25_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define XX25_COPYRIGHT	"Copyright (c) 1997-2008  OpenSS7 Corporation.  All Rights Reserved."
#define XX25_REVISION	"OpenSS7 $RCSfile: xx25.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-05-07 16:01:41 $"
#define XX25_DEVICE	"SVR 4.2 STREAMS XX25 Modle for X.25-PLP (X25MOD)"
#define XX25_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define XX25_LICENSE	"GPL"
#define XX25_BANNER	XX25_DESCRIP	"\n" \
			XX25_COPYRIGHT	"\n" \
			XX25_REVISION	"\n" \
			XX25_DEVICE	"\n" \
			XX25_CONTACT
#define XX25_SPLASH	XX25_DEVICE	" - " \
			XX25_REVISION

#ifdef LINUX
MODULE_AUTHOR(XX25_CONTACT);
MODULE_DESCRIPTION(XX25_DESCRIP);
MODULE_SUPPORTED_DEVICE(XX25_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(XX25_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-xx25");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifndef XX25_MOD_NAME
#define XX25_MOD_NAME		"xx25"
#endif				/* XX25_MOD_NAME */
#ifndef XX25_MOD_ID
#define XX25_MOD_ID		0
#endif				/* XX25_MOD_ID */

/*
 * STREAMS Definitions
 */

#define MOD_ID		XX25_MOD_ID
#define MOD_NAME	XX25_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	XX25_BANNER
#else				/* MODULE */
#define MOD_BANNER	XX25_SPLASH
#endif				/* MODULE */

static struct module_info xx25_minfo = {
	.mi_idnum = MOD_ID,
	.mi_idname = MOD_NAME,
	.mi_minpsz = 0,
	.mi_maxpsz = INFPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat xx25_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat xx25_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

/*
 * --------------------------------------------------------------------------
 *
 * XX25 OPTIONS
 *
 * --------------------------------------------------------------------------
 */

#define X25_MAX_NUI_LEN		64
#define X25_MAX_MU_LEN		16
#define X25_MAX_RPOA_LEN	 4
#define X25_MAX_FAC_LEN		32
#define X25_MAX_TARIFFS		 4

struct tp_options {
	unsigned long flags[3]; /* at least 96 flags */
	struct {
		t_uscalar_t debug[4];	/* XTI_DEBUG */
		struct t_linger linger;	/* XTI_LINGER */
		t_uscalar_t rcvbuf;	/* XTI_RCVBUF */
		t_uscalar_t rcvlowat;	/* XTI_RCVLOWAT */
		t_uscalar_t sndbuf;	/* XTI_SNDBUF */
		t_uscalar_t sndlowat;	/* XTI_SNDLOWAT */
	} xti;
	struct {
		t_uscalar_t user_dack;	/* T_X25_USER_DACK */
		t_uscalar_t user_eack;	/* T_X25_USER_EACK */
		t_uscalar_t rst_opt;	/* T_X25_RST_OPT */
		t_uscalar_t version;	/* T_X25_VERSION */
		t_uscalar_t discon_reason;	/* T_X25_DISCON_REASON */
		struct t_x25facaddr discon_add;	/* T_X25_DISCON_ADD */
		t_uscalar_t d_opt;	/* T_X25_D_OPT */
		t_uscalar_t conn_dbit;	/* T_X25_CONN_DBIT */
		struct t_x25facval pktsize;	/* T_X25_PKTSIZE */
		struct t_x25facval windowsize;	/* T_X25_WINDOWSIZE */
		struct t_x25facval tcn;	/* T_X25_TCN */
		t_uscalar_t cug;	/* T_X25_CUG */
		t_uscalar_t cugout;	/* T_X25_CUGOUT */
		t_uscalar_t bcug;	/* T_X25_BCUG */
		t_uscalar_t fastselect;	/* T_X25_FASTSELECT */
		t_uscalar_t revchg;	/* T_X25_REVCHG */
		char nui[X25_MAX_NUI_LEN];	/* T_X25_NUI */
		t_uscalar_t chginfo_req;	/* T_X25_CHGINFO_REQ */
		char chginfo_mu[X25_MAX_MU_LEN];	/* T_X25_CHGINFO_MU */
		struct t_x25facval chginfo_sc[X25_MAX_TARIFFS];	/* T_X25_CHGINFO_SC */
		struct t_x25facinfocd chginfo_cd[X25_MAX_TARIFFS];	/* T_X25_CHGINFO_CD */
		t_uscalar_t rpoa[X25_MAX_RPOA_LEN];	/* T_X25_RPOA */
		struct t_x25facaddr calldef;	/* T_X25_CALLDEF */
		struct t_x25facaddr callred;	/* T_X25_CALLRED */
		t_uscalar_t calladdmod;	/* T_X25_CALLADDMOD */
		t_uscalar_t tdsai;	/* T_X25_TDSAI */
		struct t_x25addext calling_addext;	/* T_X25_CALLING_ADDEXT 
							 */
		struct t_x25addext called_addext;	/* T_X25_CALLED_ADDEXT */
		struct t_x25facval mtcn;	/* T_X25_MTCN */
		struct t_x25faceetdn eetdn;	/* T_X25_EETDN */
		struct t_x25facpr priority[3];	/* T_X25_PRIORITY */
		struct t_x25facpr protection[3];	/* T_X25_PROTECTION */
		t_uscalar_t edn;	/* T_X25_EDN */
		char loc_nonx25[X25_MAX_FAC_LEN];	/* T_X25_LOC_NONX25 */
		char rem_nonx25[X25_MAX_FAC_LEN];	/* T_X25_REM_NONX25 */
	} x25;
};

#define xti_default_debug		{ 0, }
#define xti_default_linger		(struct t_linger){T_YES, 120}
#define xti_default_rcvbuf		STRHIGH
#define xti_default_rcvlowat		1
#define xti_default_sndbuf		STRHIGH
#define xti_default_sndlowat		1

#define x25_default_user_dack		T_NO
#define x25_default_user_eack		T_NO
#define x25_default_rst_opt		T_NO
#define x25_default_version		T_X25_1996
#define x25_default_discon_reason	T_X25_SET_CAUSE_DIAG(0,0xf1)
#define x25_default_discon_add		(struct t_x25facaddr){T_UNSPEC, 0, {0, }}
#define x25_default_d_opt		T_YES
#define x25_default_conn_dbit		T_NO
#define x25_default_pktsize		(struct t_x25facval){T_UNSPEC, T_UNSPEC}
#define x25_default_windowsize		(struct t_x25facval){T_UNSPEC, T_UNSPEC}
#define x25_default_tcn			(struct t_x25facval){T_UNSPEC, T_UNSPEC}
#define x25_default_cug			T_UNSPEC
#define x25_default_cugout		T_UNSPEC
#define x25_default_bcug		T_UNSPEC
#define x25_default_fastselect		T_UNSPEC
#define x25_default_revchg		T_UNSPEC
#define x25_default_nui			{0, }
#define x25_default_chginfo_req		T_UNSPEC
#define x25_default_chginfo_mu		{0, }
#define x25_default_chginfo_sc		{(struct t_x25facval){T_UNSPEC, T_UNSPEC}, \
					 (struct t_x25facval){T_UNSPEC, T_UNSPEC}, \
					 (struct t_x25facval){T_UNSPEC, T_UNSPEC}, \
					 (struct t_x25facval){T_UNSPEC, T_UNSPEC}}
#define x25_default_chginfo_cd		{(struct t_x25facinfocd){T_UNSPEC, T_UNSPEC, T_UNSPEC, T_UNSPEC}, \
					 (struct t_x25facinfocd){T_UNSPEC, T_UNSPEC, T_UNSPEC, T_UNSPEC}, \
					 (struct t_x25facinfocd){T_UNSPEC, T_UNSPEC, T_UNSPEC, T_UNSPEC}, \
					 (struct t_x25facinfocd){T_UNSPEC, T_UNSPEC, T_UNSPEC, T_UNSPEC}}
#define x25_default_rpoa		{0, }
#define x25_default_calldef		(struct t_x25facaddr){T_UNSPEC, 0, {0, }}
#define x25_default_callred		(struct t_x25facaddr){T_UNSPEC, 0, {0, }}
#define x25_default_calladdmod		T_UNSPEC
#define x25_default_tdsai		T_UNSPEC
#define x25_default_calling_addext	(struct t_x25addext){T_UNSPEC, 0, {0, }}
#define x25_default_called_addext	(struct t_x25addext){T_UNSPEC, 0, {0, }}
#define x25_default_mtcn		(struct t_x25facval){T_UNSPEC, T_UNSPEC}
#define x25_default_eetdn		(struct t_x25faceetdn){T_UNSPEC, T_UNSPEC, T_UNSPEC}
#define x25_default_priority		{(struct t_x25facpr){T_UNSPEC, T_UNSPEC, T_UNSPEC}, \
					 (struct t_x25facpr){T_UNSPEC, T_UNSPEC, T_UNSPEC}, \
					 (struct t_x25facpr){T_UNSPEC, T_UNSPEC, T_UNSPEC}}
#define x25_default_protection		{(struct t_x25facpr){T_UNSPEC, T_UNSPEC, T_UNSPEC}, \
					 (struct t_x25facpr){T_UNSPEC, T_UNSPEC, T_UNSPEC}, \
					 (struct t_x25facpr){T_UNSPEC, T_UNSPEC, T_UNSPEC}}
#define x25_default_edn			T_UNSPEC
#define x25_default_loc_nonx25		{0, }
#define x25_default_rem_nonx25		{0, }

enum {
	_T_BIT_XTI_DEBUG = 0,
	_T_BIT_XTI_LINGER,
	_T_BIT_XTI_RCVBUF,
	_T_BIT_XTI_RCVLOWAT,
	_T_BIT_XTI_SNDBUF,
	_T_BIT_XTI_SNDLOWAT,
	_T_BIT_X25_USER_DACK,
	_T_BIT_X25_USER_EACK,
	_T_BIT_X25_RST_OPT,
	_T_BIT_X25_VERSION,
	_T_BIT_X25_DISCON_REASON,
	_T_BIT_X25_DISCON_ADD,
	_T_BIT_X25_D_OPT,
	_T_BIT_X25_CONN_DBIT,
	_T_BIT_X25_PKTSIZE,
	_T_BIT_X25_WINDOWSIZE,
	_T_BIT_X25_TCN,
	_T_BIT_X25_CUG,
	_T_BIT_X25_CUGOUT,
	_T_BIT_X25_BCUG,
	_T_BIT_X25_FASTSELECT,
	_T_BIT_X25_REVCHG,
	_T_BIT_X25_NUI,
	_T_BIT_X25_CHGINFO_REQ,
	_T_BIT_X25_CHGINFO_MU,
	_T_BIT_X25_CHGINFO_SC,
	_T_BIT_X25_CHGINFO_CD,
	_T_BIT_X25_RPOA,
	_T_BIT_X25_CALLDEF,
	_T_BIT_X25_CALLRED,
	_T_BIT_X25_CALLADDMOD,
	_T_BIT_X25_TDSAI,
	_T_BIT_X25_CALLING_ADDEXT,
	_T_BIT_X25_CALLED_ADDEXT,
	_T_BIT_X25_MTCN,
	_T_BIT_X25_EETDN,
	_T_BIT_X25_PRIORITY,
	_T_BIT_X25_PROTECTION,
	_T_BIT_X25_EDN,
	_T_BIT_X25_LOC_NONX25,
	_T_BIT_X25_REM_NONX25,
};

struct tp_options t_defaults = {
	{0,},
	{
	 xti_default_debug,
	 xti_default_linger,
	 xti_default_rcvbuf,
	 xti_default_rcvlowat,
	 xti_default_sndbuf,
	 xti_default_sndlowat,
	 },
	{
	 x25_default_user_dack,
	 x25_default_user_eack,
	 x25_default_rst_opt,
	 x25_default_version,
	 x25_default_discon_reason,
	 x25_default_discon_add,
	 x25_default_d_opt,
	 x25_default_conn_dbit,
	 x25_default_pktsize,
	 x25_default_windowsize,
	 x25_default_tcn,
	 x25_default_cug,
	 x25_default_cugout,
	 x25_default_bcug,
	 x25_default_fastselect,
	 x25_default_revchg,
	 x25_default_nui,
	 x25_default_chginfo_req,
	 x25_default_chginfo_mu,
	 x25_default_chginfo_sc,
	 x25_default_chginfo_cd,
	 x25_default_rpoa,
	 x25_default_calldef,
	 x25_default_callred,
	 x25_default_calladdmod,
	 x25_default_tdsai,
	 x25_default_calling_addext,
	 x25_default_called_addext,
	 x25_default_mtcn,
	 x25_default_eetdn,
	 x25_default_priority,
	 x25_default_protection,
	 x25_default_edn,
	 x25_default_loc_nonx25,
	 x25_default_rem_nonx25,
	 }
};

/*
 * --------------------------------------------------------------------------
 *
 * PRIVATE STRUCTURE
 *
 * --------------------------------------------------------------------------
 */

struct priv;
struct tp;
struct np;

struct tp {
	struct np *np;
	queue_t *oq;
	t_uscalar_t state;
	t_uscalar_t oldstate;
	struct {
		struct T_info_ack info;
	} proto;
	struct tp_options opts;
	unsigned char loc[20];
	unsigned char rem[20];
};
struct np {
	struct tp *tp;
	queue_t *oq;
	np_ulong state;
	np_ulong oldstate;
	struct {
		N_info_ack_t info;
		unsigned char add[20];
		N_qos_co_opt_sel_t qos;
		N_qos_co_opt_range_t qor;
	} proto;
	unsigned char loc[20];
	unsigned char rem[20];
};

struct priv {
	struct tp tpi;
	struct np npi;
};

#define PRIV(q) ((struct priv *)q->q_ptr)
#define TP_PRIV(q) (&PRIV(q)->tpi)
#define NP_PRIV(q) (&PRIV(q)->npi)

#define STRLOGIO    0		/* input-output controls */
#define STRLOGST    1		/* state transitions */
#define STRLOGTO    2		/* timeouts */
#define STRLOGRX    3		/* primitives received */
#define STRLOGTX    4		/* primitives issued */
#define STRLOGTE    5		/* timer events */
#define STRLOGDA    6		/* data */

static const char *
tp_primname(t_uscalar_t prim)
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
		return ("T_????");
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

static inline const char *
tp_errname(t_scalar_t error)
{
	if (error < 0)
		return ("TSYSERR");
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
		return ("TNOSTRUCTYPE");
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

static inline const char *
tp_strerror(t_scalar_t error)
{
	if (error < 0)
		return ("System error.");
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
		return ("Has to t_look() for event (not TPI).");
	case TBADDATA:
		return ("Bad amount of data.");
	case TBUFOVFLW:
		return ("Buffer was too small (not TPI).");
	case TFLOW:
		return ("Would block due to flow control (not TPI).");
	case TNODATA:
		return ("No data indication (not TPI).");
	case TNODIS:
		return ("No disconnect indication (not TPI).");
	case TNOUDERR:
		return ("No unitdata error indication (not TPI).");
	case TBADFLAG:
		return ("Bad flags.");
	case TNOREL:
		return ("No orderly release indication (not TPI).");
	case TNOTSUPPORT:
		return ("Not supported.");
	case TSTATECHNG:
		return ("State is currently changing (not TPI).");
	case TNOSTRUCTYPE:
		return ("Structure type not supported (not TPI).");
	case TBADNAME:
		return ("Bad transport provider name (not TPI).");
	case TBADQLEN:
		return ("Listener queue length limit is zero (not TPI).");
	case TADDRBUSY:
		return ("Address already in use (not TPI).");
	case TINDOUT:
		return ("Outstanding connect indications (not TPI).");
	case TPROVMISMATCH:
		return ("Not same transport provider (not TPI).");
	case TRESQLEN:
		return ("Connection acceptor has qlen > 0 (not TPI).");
	case TRESADDR:
		return ("Conn. acceptor bound to different address (not TPI).");
	case TQFULL:
		return ("Connection indicator queue is full (not TPI).");
	case TPROTO:
		return ("Protocol error (not TPI).");
	default:
		return ("Unknown error code.");
	}
}

static t_uscalar_t
tp_get_state(sturct tp * tp)
{
	return (tp->state);
}

static t_uscalar_t
tp_set_state(struct tp *tp, t_uscalar_t newstate)
{
	t_uscalar_t oldstate = tp->state;

	if (newstate != oldstate) {
		tp->state = newstate;
		tp->proto.info.CURRENT_state = newstate;
		mi_strlog(tp->oq, STRLOGST, SL_TRACE, "%s <- %s", tp_statename(newstate),
			  tp_statename(oldstate));
	}
	return (newstate);
}

static t_uscalar_t
tp_save_state(struct tp *tp)
{
	return ((tp->oldstate = tp_get_state(tp)));
}

static t_uscalar_t
tp_restore_state(struct tp *tp)
{
	return (tp_set_state(tp, tp->oldstate));
}

static inline const char *
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
		return ("N_????");
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

static inline const char *
np_errname(np_long error)
{
	if (error < 0)
		return ("NSYSERR");
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

static inline const char *
np_strerror(np_long error)
{
	if (error < 0)
		return ("System error.");
	switch (error) {
	case NBADADDR:
		return ("Incorrect address format/illegal address information.");
	case NBADOPT:
		return ("Options in incorrect format or contain illegal information.");
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
		return ("Unknown error code.");
	}
}

static np_ulong
np_get_state(sturct np * np)
{
	return (np->state);
}

static np_ulong
np_set_state(struct np *np, np_ulong newstate)
{
	np_ulong oldstate = np->state;

	if (newstate != oldstate) {
		np->state = newstate;
		np->proto.info.CURRENT_state = newstate;
		mi_strlog(np->oq, STRLOGST, SL_TRACE, "%s <- %s", np_statename(newstate),
			  np_statename(oldstate));
	}
	return (newstate);
}

static np_ulong
np_save_state(struct np *np)
{
	return ((np->oldstate = np_get_state(np)));
}

static np_ulong
np_restore_state(struct np *np)
{
	return (np_set_state(np, np->oldstate));
}

/*
 * --------------------------------------------------------------------------
 *
 * TPI PROVIDER TO TPI USER PRIMITIVES ISSUED UPSTREAM
 *
 * --------------------------------------------------------------------------
 * For all of these functions @tp is a pointer to the TPI private structure,
 * @q is a pointer to the active queue, @msg is a pointer to a message block
 * to be freed upon success (if not NULL).  Additional parameters are
 * primitive specific.
 */

/**
 * m_error: - issue M_ERROR message
 * @tp: transport private structure
 * @q: active queue
 * @msg: message to be freed upont success
 * @rerr: read error
 * @werr: write error
 * @func: calling function
 */
static fastcall noinline __unlikely int
m_error(struct tp *tp, queue_t *q, mblk_t *msg, int rerr, int werr, const char *func)
{
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, 2, BPRI_MED)))) {
		DB_TYPE(mp) = M_ERROR;
		*mp->b_wptr++ = rerr < 0 ? -rerr : rerr;
		*mp->b_wptr++ = werr < 0 ? -werr : werr;
		freemsg(msg);
		np_set_state(tp->np, NS_NOSTATES);
		tp_set_state(tp, TS_NOSTATES);
		if (func != NULL)
			mi_strlog(tp->oq, 0, SL_TRACE, "M_ERROR: %s", func);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- M_ERROR");
		putnext(tp->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * tp_conn_ind: - issue T_CONN_IND primitive
 * @tp: transport private structure
 * @q: active queue
 * @msg: message to free upon success
 * @src: buffer describing source address
 * @opt: buffer describing options
 * @seq: sequence number
 * @dp: user data
 */
static fastcall noinline __unlikely int
tp_conn_ind(struct tp *tp, queue_t *q, mblk_t *msg, struct strbuf *src, struct strbuf *opt,
	    t_scalar_t seq, mblk_t *dp)
{
	struct T_conn_ind *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + src->len + opt->len, BPRI_MED)))) {
		if (likely(canpuntext(tp->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_CONN_IND;
			p->SRC_length = src->len;
			p->SRC_offset = sizeof(*p);
			p->OPT_length = opt->len;
			p->OPT_offset = sizeof(*p) + src->len;
			p->SEQ_number = seq;
			mp->b_wptr += sizeof(*p);
			bcopy(src->buf, mp->b_wptr, src->len);
			mp->b_wptr += src->len;
			bcopy(opt->buf, mp->b_wptr, opt->len);
			mp->b_wptr += opt->len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			tp_set_state(tp, TS_WRES_CIND);
			tp->coninds++;
			mi_strlog(tp->oq, STRLOGTX, SL_TRACE, "<- T_CONN_IND");
			putnext(tp->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * tp_conn_con: - issue T_CONN_CON primitive
 * @tp: transport private structure
 * @q: active queue
 * @msg: message to free upon success
 * @res: buffer describing responding address
 * @opt: buffer describing options
 * @dp: user data
 */
static fastcall noinline __unlikely int
tp_conn_con(struct tp *tp, queue_t *q, mblk_t *msg, struct strbuf *res, struct strbuf *opt,
	    mblk_t *dp)
{
	struct T_conn_con *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + res->len + opt->len, BPRI_MED)))) {
		if (likely(canpuntext(tp->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_CONN_CON;
			p->RES_length = res->len;
			p->RES_offset = sizeof(*p);
			p->OPT_length = opt->len;
			p->OPT_offset = sizeof(*p) + res->len;
			mp->b_wptr += sizeof(*p);
			bcopy(res->buf, mp->b_wptr, res->len);
			mp->b_wptr += res->len;
			bcopy(opt->buf, mp->b_wptr, opt->len);
			mp->b_wptr += opt->len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			tp_set_state(tp, TS_DATA_XFER);
			mi_strlog(tp->oq, STRLOGTX, SL_TRACE, "<- T_CONN_CON");
			putnext(tp->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * tp_discon_ind: - issue T_DISCON_IND primitive
 * @tp: transport private structure
 * @q: active queue
 * @msg: message to free upon success
 * @reason: disconnect reason
 * @seq: sequence number
 * @dp: user data
 */
static fastcall noinline __unlikely int
tp_discon_ind(struct tp *tp, queue_t *q, mblk_t *msg, t_uscalar_t reason, t_uscalar_t seq,
	      mblk_t *dp)
{
	struct T_discon_ind *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canpuntext(tp->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_DISCON_IND;
			p->DISCON_reason = reason;
			p->SEQ_number = seq;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			if (seq != 0)
				tp->coninds--;
			tp_set_state(tp, (tp->coninds > 0) ? TS_WRES_CIND : TS_IDLE);
			mi_strlog(tp->oq, STRLOGTX, SL_TRACE, "<- T_DISCON_IND");
			putnext(tp->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * tp_data_ind: - issue T_DATA_IND primitive
 * @tp: transport private structure
 * @q: active queue
 * @msg: message to free upon success
 * @more: more flag
 * @dp: user data
 *
 * The behaviour of the t_rcv(3) function remains unchanged.  The default
 * behaviour is to acknowledge, in an automatic way, data sent with the
 * Delivery Confirmation bit and expedited data.  The optional "explicity
 * acknowledgement" is selcted with with the T_X25_USER_DACK option, for the
 * acknowledgement of data sent with the D bit, or with the T_X25_USER_EACK
 * option, for the acknowledgement of expedited data.
 *
 * If expedited data arrives after part of a TSDU has been received, receipt
 * of the remainder of the TSDU is suspended until the ETSDU has been
 * processed.  Only after the full ETSDU has been received (the T_MORE flag
 * not set), the remainder of the TSDU is made available to the user.
 *
 * Also, flags additional to the T_EXPEDITED and T_MORE flags can be set in
 * the argument flags:
 *
 * - On return from the call, if T_X25_D is set in flags, this indicates that
 *   the data returned was sent with the D bit, and the T_X25_USER_DACK option
 *   is set.  This data has to be acknowledged explicity by the receiver.
 *
 * - On return from the call, if T_X25_DACK or T_X25_EACK is set in flags, the
 *   previously sent data or expedited data has been acknowledged.
 *
 *   Note: If either T_X25_DACK or T_X25_EACK is set in flags, then no other
 *         flags are set and no user data is returned to the user.
 *
 * - On return from the call, if T_X25_Q is set in flags, the data returned
 *   are qualified.
 *
 * - On return from the call, if T_X25_RST is set in flags, this indicates
 *   that a reset indication occured.
 *
 *   When T_X25_RST is returned, the argument buf contains the cause and
 *   diagnostic of the reset.  Each one is coded in one octet.  The cause is
 *   encoded in the first octet and the diagnostic in the second octet.  If
 *   the user's buffer is less than two bytes long then the diagnostic value
 *   is disregarded, and if the length is zero, the cause is also discarded.
 */
static fastcall inline __hot_read int
tp_data_ind(struct tp *tp, queue_t *q, mblk_t *msg, t_uscalar_t more, mblk_t *dp)
{
	struct T_data_ind *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canpuntext(tp->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_DATA_IND;
			p->MORE_flag = more;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(tp->oq, STRLOGDA, SL_TRACE, "<- T_DATA_IND");
			putnext(tp->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * tp_exdata_ind: - issue T_EXDATA_IND primitive
 * @tp: transport private structure
 * @q: active queue
 * @msg: message to free upon success
 * @more: more flag
 * @dp: user data
 */
static fastcall inline __hot_read int
tp_exdata_ind(struct tp *tp, queue_t *q, mblk_t *msg, t_uscalar_t more, mblk_t *dp)
{
	struct T_exdata_ind *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canpuntext(tp->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_EXDATA_IND;
			p->MORE_flag = more;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(tp->oq, STRLOGDA, SL_TRACE, "<- T_EXDATA_IND");
			putnext(tp->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * tp_info_ack: - issue T_INFO_ACK primitive
 * @tp: transport private structure
 * @q: active queue
 * @msg: message to free upon success
 *
 * The information returned by t_getinfo(3) reflects the characteristics of
 * the X.25 connection, or if no connection is established, the maximum
 * characteristics an X.25 connection could take on using the underlying X.25
 * provider.
 *
 * The parameters of the t_getinfo(3) function for the different versions of
 * the X.25 protocol (X.25-1980, X.25-1984, X.25-1988, X.25-1993, X.25-1996,
 * and so on) are presented in the table below:
 *
 *  - TSDU_size:     T_INFINITE or a number greater than zero.
 *  - ETSDU_size:    T_INVALID or 1 (1980) or 32 (others) when negotiated
 *  - CDATA_size:    16 basic format, 128 extended (if fast-select negotiated)
 *  - DDATA_size:    0 basic format, 128 extended (if fast-select negotiated)
 *  - SERV_type:     T_COTS
 *  - PROVIDER_flag: T_SENDZERO
 */
static fastcall noinline __unlikely int
tp_info_ack(struct tp *tp, queue_t *q, mblk_t *msg)
{
	struct T_info_ack *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_INFO_ACK;
		p->TSDU_size = tp->proto.info.TSDU_size;
		p->ETSDU_size = tp->proto.info.ETSDU_size;
		p->CDATA_size = tp->proto.info.CDATA_size;
		p->DDATA_size = tp->proto.info.DDATA_size;
		p->ADDR_size = tp->proto.info.ADDR_size;
		p->OPT_size = tp->proto.info.OPT_size;
		p->TIDU_size = tp->proto.info.TIDU_size;
		p->SERV_type = tp->proto.info.SERV_type;
		p->CURRENT_state = tp->proto.info.CURRENT_state;
		p->PROVIDER_flag = tp->proto.info.PROVIDER_flag;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(tp->oq, STRLOGTX, SL_TRACE, "<- T_INFO_ACK");
		putnext(tp->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * tp_bind_ack: - issue T_BIND_ACK primitive
 * @tp: transport private structure
 * @q: active queue
 * @msg: message to free upon success
 * @add: buffer describing address
 * @conind: number of connection indications
 */
static fastcall noinline __unlikely int
tp_bind_ack(struct tp *tp, queue_t *q, mblk_t *msg, struct strbuf *add, t_uscalar_t conind)
{
	struct T_bind_ack *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + add->len, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_BIND_ACK;
		p->ADDR_length = add->len;
		p->ADDR_offset = sizeof(*p);
		p->CONIND_number = conind;
		mp->b_wptr += sizeof(*p);
		bcopy(add->buf, mp->b_wptr, add->len);
		mp->b_wptr += add->len;
		freemsg(msg);
		tp_set_state(tp, TS_IDLE);
		mi_strlog(tp->oq, STRLOGTX, SL_TRACE, "<- T_BIND_ACK");
		putnext(tp->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * tp_error_ack: - issue T_ERROR_ACK primitive
 * @tp: transport private structure
 * @q: active queue
 * @msg: message to free upon success
 * @prim: primitive in error
 * @error: negative UNIX positive TLI error
 * @func: calling function
 */
static fastcall noinline __unlikely int
tp_error_ack(struct tp *tp, queue_t *q, mblk_t *msg, t_uscalar_t prim, t_scalar_t error,
	     const char *func)
{
	struct T_error_ack *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_ERROR_ACK;
		p->ERROR_prim = prim;
		p->TLI_error = error < 0 ? TSYSERR : error;
		p->UNIX_error = error < 0 ? -error : 0;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		if (func != NULL)
			mi_strlog(tp->oq, 0, SL_TRACE, "%s: %s: %s", func, tp_primname(prim),
				  tp_strerror(error));
		switch (tp_get_state(tp)) {
		case TS_WACK_BREQ:
			tp_set_state(tp, TS_UNBND);
			break;
		case TS_WACK_CREQ:
			tp_set_state(tp, TS_IDLE);
			break;
		case TS_WACK_CRES:
			tp_set_state(tp, TS_WRES_CIND);
			break;
		case TS_WACK_DREQ6:
			tp_set_state(tp, TS_WCON_CREQ);
			break;
		case TS_WACK_DREQ7:
			tp_set_state(tp, TS_WRES_CIND);
			break;
		case TS_WACK_DREQ9:
			tp_set_state(tp, TS_DATA_XFER);
			break;
		case TS_WACK_DREQ10:
			tp_set_state(tp, TS_WIND_ORDREL);
			break;
		case TS_WACK_DREQ11:
			tp_set_state(tp, TS_WREQ_ORDREL);
			break;
		case TS_WACK_UREQ:
			tp_set_state(tp, TS_IDLE);
			break;
		case TS_WACK_OPTREQ:
			tp_set_state(tp, TS_IDLE);
			break;
		default:
			/* Not typically an error: T_OPTMGMT_REQ does not
			   change the state to TS_WACK_OPTREQ when the original 
			   state was not TS_IDLE. */
			break;
		}
		mi_strlog(tp->oq, STRLOGTX, SL_TRACE, "<- T_ERROR_ACK");
		putnext(tp->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * tp_ok_ack: - issue T_OK_ACK primitive
 * @tp: transport private structure
 * @q: active queue
 * @msg: message to free upon success
 * @prim: correct primitive
 */
static fastcall noinline __unlikely int
tp_ok_ack(struct tp *tp, queue_t *q, mblk_t *msg, t_uscalar_t prim)
{
	struct T_ok_ack *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_OK_ACK;
		p->CORRECT_prim = prim;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		switch (tp_get_state(tp)) {
		case TS_WACK_CREQ:
			tp_set_state(tp, TS_WCON_CREQ);
			break;
		case TS_WACK_CRES:
			tp_set_state(tp, TS_DATA_XFER);
			break;
		case TS_WACK_DREQ6:
			tp_set_state(tp, TS_IDLE);
			break;
		case TS_WACK_DREQ7:
			tp->coninds--;
			tp_set_state(tp, (tp->coninds > 0) ? TS_WRES_CIND : TS_IDLE);
			break;
		case TS_WACK_DREQ9:
			tp_set_state(tp, (tp->coninds > 0) ? TS_WRES_CIND : TS_IDLE);
			break;
		case TS_WACK_DREQ10:
			tp_set_state(tp, (tp->coninds > 0) ? TS_WRES_CIND : TS_IDLE);
			break;
		case TS_WACK_DREQ11:
			tp_set_state(tp, (tp->coninds > 0) ? TS_WRES_CIND : TS_IDLE);
			break;
		case TS_WACK_UREQ:
			tp_set_state(tp, TS_UNBND);
			break;
		default:
			/* Don't change state.  This is not necessarily an
			   error as T_OPTMGMT_REQ when issued in a state other
			   than TS_IDLE does not change the state to
			   TS_WACK_OPTREQ. */
			break;
		}
		mi_strlog(tp->oq, STRLOGTX, SL_TRACE, "<- T_OK_ACK");
		putnext(tp->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * tp_unitdata_ind: - issue T_UNITDATA_IND primitive
 * @tp: transport private structure
 * @q: active queue
 * @msg: message to free upon success
 * @src: buffer describing source address
 * @opt: buffer describing options
 * @dp: user data
 */
static fastcall inline __hot_read int
tp_unitdata_ind(struct tp *tp, queue_t *q, mblk_t *msg, struct strbuf *src, struct strbuf *opt,
		mblk_t *dp)
{
	struct T_unitdata_ind *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + src->len + opt->len, BPRI_MED)))) {
		if (likely(canpuntext(tp->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_UNITDATA_IND;
			p->SRC_length = src->len;
			p->SRC_offset = sizeof(*p);
			p->OPT_length = opt->len;
			p->OPT_offset = sizeof(*p) + src->len;
			mp->b_wptr += sizeof(*p);
			bcopy(src->buf, mp->b_wptr, src->len);
			mp->b_wptr += src->len;
			bcopy(opt->buf, mp->b_wptr, opt->len);
			mp->b_wptr += opt->len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(tp->oq, STRLOGDA, SL_TRACE, "<- T_UNITDATA_IND");
			putnext(tp->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * tp_uderror_ind: - issue T_UDERROR_IND primitive
 * @tp: transport private structure
 * @q: active queue
 * @msg: message to free upon success
 * @dst: buffer describing destination address
 * @opt: buffer describing options
 * @type: error type
 * @dp: user data
 */
static fastcall noinline __unlikely int
tp_uderror_ind(struct tp *tp, queue_t *q, mblk_t *msg, struct strbuf *dst, struct strbuf *opt,
	       t_uscalar_t type, mblk_t *dp)
{
	struct T_uderror_ind *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + dst->len + opt->len, BPRI_MED)))) {
		if (likely(canpuntext(tp->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_UDERROR_IND;
			p->DEST_length = dst->len;
			p->DEST_offset = sizeof(*p);
			p->OPT_length = opt->len;
			p->OPT_offset = sizeof(*p) + dst->len;
			p->ERROR_type = type;
			mp->b_wptr += sizeof(*p);
			bcopy(dst->buf, mp->b_wptr, dst->len);
			mp->b_wptr += dst->len;
			bcopy(opt->buf, mp->b_wptr, opt->len);
			mp->b_wptr += opt->len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(tp->oq, STRLOGDA, SL_TRACE, "<- T_UDERROR_IND");
			putnext(tp->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * tp_optmgmt_ack: - issue T_OPTMGMT_ACK primitive
 * @tp: transport private structure
 * @q: active queue
 * @msg: message to free upon success
 * @opt: buffer describing options
 * @flags: management flags
 */
static fastcall noinline __unlikely int
tp_optmgmt_ack(struct tp *tp, queue_t *q, mblk_t *msg, struct strbuf *opt, t_uscalar_t flags)
{
	struct T_optmgmt_ack *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + opt->len, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_OPTMGMT_ACK;
		p->OPT_length = opt->len;
		p->OPT_offset = sizeof(*p);
		p->MGMT_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(opt->buf, mp->b_wptr, opt->len);
		mp->b_wptr += opt->len;
		freemsg(msg);
		if (tp_get_state(tp) == TS_WACK_OPTREQ)
			tp_set_state(tp, TS_IDLE);
		mi_strlog(tp->oq, STRLOGTX, SL_TRACE, "<- T_OPTMGMT_ACK");
		putnext(tp->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * tp_ordrel_ind: - issue T_ORDREL_IND primitive
 * @tp: transport private structure
 * @q: active queue
 * @msg: message to free upon success
 * @dp: user data
 */
static fastcall noinline __unlikely int
tp_ordrel_ind(struct tp *tp, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	struct T_ordrel_ind *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canpuntext(tp->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_ORDREL_IND;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			switch (tp_get_state(tp)) {
			case TS_WIND_ORDREL:
				tp_set_state(tp, TS_IDLE);
				break;
			case TS_DATA_XFER:
				tp_set_state(tp, TS_WIND_ORDREL);
				break;
			default:
				mi_strlog(tp->oq, 0, SL_ERROR, "%s: state machine error",
					  __FUNCTION__);
				freemsg(mp);
				return (0);
			}
			mi_strlog(tp->oq, STRLOGTX, SL_TRACE, "<- T_ORDREL_IND");
			putnext(tp->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * tp_optdata_ind: - issue T_OPTDATA_IND primitive
 * @tp: transport private structure
 * @q: active queue
 * @msg: message to free upon success
 * @flags: data transfer flags
 * @opt: buffer describing options
 * @dp: user data
 */
static fastcall inline __hot_read int
tp_optdata_ind(struct tp *tp, queue_t *q, mblk_t *msg, t_uscalar_t flags, struct strbuf *opt,
	       mblk_t *dp)
{
	struct T_optdata_ind *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + opt->len, BPRI_MED)))) {
		unsigned char band = (flags & T_ODF_EX) ? 1 : 0;

		if (likely(bcanpuntext(tp->oq, band))) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = band;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_OPTDATA_IND;
			p->DATA_flag = flags;
			p->OPT_length = opt->len;
			p->OPT_offset = sizeof(*p);
			mp->b_wptr += sizeof(*p);
			bcopy(opt->buf, mp->b_wptr, opt->len);
			mp->b_wptr += opt->len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(tp->oq, STRLOGDA, SL_TRACE, "<- T_OPTDATA_IND");
			putnext(tp->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * tp_addr_ack: - issue T_ADDR_ACK primitive
 * @tp: transport private structure
 * @q: active queue
 * @msg: message to free upon success
 * @loc: buffer describing local address
 * @rem: buffer describing remote address
 */
static fastcall noinline __unlikely int
tp_addr_ack(struct tp *tp, queue_t *q, mblk_t *msg, struct strbuf *loc, struct strbuf *rem)
{
	struct T_addr_ack *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + loc->len + rem->len, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_ADDR_ACK;
		p->LOCADDR_length = loc->len;
		p->LOCADDR_offset = sizeof(*p);
		p->REMADDR_length = rem->len;
		p->REMADDR_offset = sizeof(*p) + loc->len;
		mp->b_wptr += sizeof(*p);
		bcopy(loc->buf, mp->b_wptr, loc->len);
		mp->b_wptr += loc->len;
		bcopy(rem->buf, mp->b_wptr, rem->len);
		mp->b_wptr += rem->len;
		freemsg(msg);
		mi_strlog(tp->oq, STRLOGTX, SL_TRACE, "<- T_ADDR_ACK");
		putnext(tp->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * tp_capability_ack: - issue T_CAPABILITY_ACK primitive
 * @tp: transport private structure
 * @q: active queue
 * @msg: message to free upon success
 * @db_type: M_PROTO or M_PCPROTO
 */
static fastcall noinline __unlikely int
tp_capability_ack(struct tp *tp, queue_t *q, mblk_t *msg, uchar db_type)
{
	struct T_capability_ack *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(pcmsg(db_type) || canpuntext(tp->oq))) {
			DB_TYPE(mp) = db_type;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_CAPABILITY_ACK;
			p->CAP_bits1 = TC1_INFO | TC1_ACCEPTOR_ID;
			p->INFO_ack.PRIM_type = T_INFO_ACK;
			p->INFO_ack.TSDU_size = tp->proto.info.TSDU_size;
			p->INFO_ack.ETSDU_size = tp->proto.info.ETSDU_size;
			p->INFO_ack.CDATA_size = tp->proto.info.CDATA_size;
			p->INFO_ack.DDATA_size = tp->proto.info.DDATA_size;
			p->INFO_ack.ADDR_size = tp->proto.info.ADDR_size;
			p->INFO_ack.OPT_size = tp->proto.info.OPT_size;
			p->INFO_ack.TIDU_size = tp->proto.info.TIDU_size;
			p->INFO_ack.SERV_type = tp->proto.info.SERV_type;
			p->INFO_ack.CURRENT_state = tp->proto.info.CURRENT_state;
			p->INFO_ack.PROVIDER_flag = tp->proto.info.PROVIDER_flag;
			p->ACCEPTOR_id = (t_uscalar_t) (ulong) (tp->oq);
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(tp->oq, STRLOGTX, SL_TRACE, "<- T_CAPABILITY_ACK");
			putnext(tp->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 * Primitives issued downstream (NPI NS-User Primitives)
 * --------------------------------------------------------------------------
 * For all of these functions @xp is a pointer to the PLP private structure,
 * @q is a pointer to the active queue, @msg is a pointer to a message to be
 * freed upon success (if not NULL).  Additional parameters are primitive
 * specific.
 */
/**
 * np_conn_req: - issue N_CONN_REQ primitive
 * @np: network private structure
 * @q: active queue
 * @msg: message to free upon success
 * @dst: buffer describing destination address
 * @flags: connect flags
 * @qos: buffer describing QOS parameters
 * @dp: user data
 */
static fastcall noinline __unlikely int
np_conn_req(struct np *np, queue_t *q, mblk_t *msg, struct strbuf *dst, np_ulong flags,
	    struct strbuf *qos, mblk_t *dp)
{
	N_conn_req_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p) + dst->len + qos->len, BPRI_MED))) {
		if (canputnext(np->oq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_CONN_REQ;
			p->DEST_length = dst->len;
			p->DEST_offset = sizeof(*p);
			p->CONN_flags = flags;
			p->QOS_length = qos->len;
			p->QOS_offset = sizeof(*p) + dst->len;
			mp->b_wptr += sizeof(*p);
			bcopy(dst->buf, mp->b_wptr, dst->len);
			mp->b_wptr += dst->len;
			bcopy(qos->buf, mp->b_wptr, qos->len);
			mp->b_wptr += qos->len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				mp->b_cont = NULL;
			freemsg(msg);
			np_set_state(np, NS_WCON_CREQ);
			mi_strlog(np->oq, STRLOGTX, SL_TRACE, "-> N_CONN_REQ");
			putnext(np->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * np_conn_res: - issue N_CONN_RES primitive
 * @np: network private structure
 * @q: active queue
 * @msg: message to free upon success
 * @tok: token value
 * @res: buffer describing responding address
 * @seq: sequence number
 * @flags: connection flags
 * @qos: buffer describing QOS parameters
 * @dp: user data
 */
static fastcall noinline __unlikely int
np_conn_res(struct np *np, queue_t *q, mblk_t *msg, np_ulong tok, struct strbuf *res, np_ulong seq,
	    np_ulong flags, struct strbuf *qos, mblk_t *dp)
{
	N_conn_res_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p) + res->len + qos->len, BPRI_MED))) {
		if (canputnext(np->oq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_CONN_RES;
			p->TOKEN_value = tok;
			p->RES_length = res->len;
			p->RES_offset = sizeof(*p);
			p->SEQ_number = seq;
			p->CONN_flags = flags;
			p->QOS_length = qos->len;
			p->QOS_offset = sizeof(*p) + res->len;
			mp->b_wptr += sizeof(*p);
			bcopy(res->buf, mp->b_wptr, res->len);
			mp->b_wptr += res->len;
			bcopy(qos->buf, mp->b_wptr, qos->len);
			mp->b_wptr += qos->len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				mp->b_cont = NULL;
			freemsg(msg);
			np_set_state(np, NS_WACK_CRES);
			mi_strlog(np->oq, STRLOGTX, SL_TRACE, "-> N_CONN_RES");
			putnext(np->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * np_discon_req: - issue N_DISCON_REQ primitive
 * @np: network private structure
 * @q: active queue
 * @msg: message to free upon success
 * @reason: disconnect reason
 * @res: buffer describing responding address
 * @seq: sequence number
 * @dp: user data
 */
static fastcall noinline __unlikely int
np_discon_req(struct np *np, queue_t *q, mblk_t *msg, np_ulong reason, struct strbuf *res,
	      np_ulong seq, mblk_t *dp)
{
	N_discon_req_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p) + res->len, BPRI_MED))) {
		if (canputnext(np->oq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_DISCON_REQ;
			p->DISCON_reason = reason;
			p->RES_length = res->len;
			p->RES_offset = sizeof(*p);
			p->SEQ_number = seq;
			mp->b_wptr += sizeof(*p);
			bcopy(res->buf, mp->b_wptr, res->len);
			mp->b_wptr += res->len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				mp->b_cont = NULL;
			freemsg(msg);
			np_set_state(np, np_sequence_free(np, seq) ? NS_WRES_CIND : NS_IDLE);
			mi_strlog(np->oq, STRLOGTX, SL_TRACE, "-> N_DISCON_REQ");
			putnext(np->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * np_data_req: - issue N_DATA_REQ primitive
 * @np: network private structure
 * @q: active queue
 * @msg: message to free upon success
 * @flags: data transfer flags
 * @dp: user data
 */
static fastcall inline __hot_write int
np_data_req(struct np *np, queue_t *q, mblk_t *msg, np_ulong flags, mblk_t *dp)
{
	N_data_req_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(np->oq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_DATA_REQ;
			p->DATA_xfer_flags = flags;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				mp->b_cont = NULL;
			freemsg(msg);
			if (flags & N_MORE_DATA_FLAG)
				flags &= ~N_RC_FLAG;
			else if ((flags & N_RC_FLAG) || np->options & REC_CONF_OPT)
				np->datacks++;
			mi_strlog(np->oq, STRLOGDA, SL_TRACE, "-> N_DATA_REQ");
			putnext(np->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * np_exdata_req: - issue N_EXDATA_REQ primitive
 * @np: network private structure
 * @q: active queue
 * @msg: message to free upon success
 * @dp: user data
 */
static fastcall inline __hot_write int
np_exdata_req(struct np *np, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	N_exdata_req_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		if (bcanputnext(np->oq, 1)) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_EXDATA_REQ;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				mp->b_cont = NULL;
			freemsg(msg);
#if 0
			if (np->options & REC_CONF_OPT)
				np->edatacks++;
#endif
			mi_strlog(np->oq, STRLOGDA, SL_TRACE, "-> N_EXDATA_REQ");
			putnext(np->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * np_info_req: - issue N_INFO_REQ primitive
 * @np: network private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall noinline __unlikely int
np_info_req(struct np *np, queue_t *q, mblk_t *msg)
{
	N_info_req_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_INFO_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(np->oq, STRLOGTX, SL_TRACE, "-> N_INFO_REQ");
		putnext(np->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * np_bind_req: - issue N_BIND_REQ primitive
 * @np: network private structure
 * @q: active queue
 * @msg: message to free upon success
 * @add: buffer describing address
 * @conind: number of outstanding connection indications
 * @flags: bind flags
 * @pro: buffer describing protocol ids
 */
static fastcall noinline __unlikely int
np_bind_req(struct np *np, queue_t *q, mblk_t *msg, struct strbuf *add, np_ulong conind,
	    np_ulong flags, struct strbuf *pro)
{
	N_bind_req_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p) + add->len + pro->len, BPRI_MED))) {
		if (canputnext(np->oq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_BIND_REQ;
			p->ADDR_length = add->len;
			p->ADDR_offset = sizeof(*p);
			p->CONIND_number = conind;
			p->BIND_flags = flags;
			p->PROTOID_length = pro->len;
			p->PROTOID_offset = sizeof(*p) + add->len;
			mp->b_wptr += sizeof(*p);
			bcopy(add->buf, mp->b_wptr, add->len);
			mp->b_wptr += add->len;
			bcopy(pro->buf, mp->b_wptr, pro->len);
			mp->b_wptr += pro->len;
			freemsg(msg);
			np_set_state(np, NS_WACK_BREQ);
			mi_strlog(np->oq, STRLOGTX, SL_TRACE, "-> N_BIND_REQ");
			putnext(np->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * np_unbind_req: - issue N_UNBIND_REQ primitive
 * @np: network private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall noinline __unlikely int
np_unbind_req(struct np *np, queue_t *q, mblk_t *msg)
{
	N_unbind_req_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(np->oq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_UNBIND_REQ;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			np_set_state(np, NS_WACK_UREQ);
			mi_strlog(np->oq, STRLOGTX, SL_TRACE, "-> N_UNBIND_REQ");
			putnext(np->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * np_unitdata_req: - issue N_UNITDATA_REQ primitive
 * @np: network private structure
 * @q: active queue
 * @msg: message to free upon success
 * @dst: buffer describing destination address
 * @dp: user data
 */
static fastcall inline __hot_write int
np_unitdata_req(struct np *np, queue_t *q, mblk_t *msg, struct strbuf *dst, mblk_t *dp)
{
	N_unitdata_req_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p) + dst->len, BPRI_MED))) {
		if (canputnext(np->oq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_UNITDATA_REQ;
			p->DEST_length = dst->len;
			p->DEST_offset = sizeof(*p);
			p->RESERVED_field[0] = 0;
			p->RESERVED_field[1] = 0;
			mp->b_wptr += sizeof(*p);
			bcopy(dst->buf, mp->b_wptr, dst->len);
			mp->b_wptr += dst->len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				mp->b_cont = NULL;
			freemsg(msg);
			mi_strlog(np->oq, STRLOGDA, SL_TRACE, "-> N_UNITDATA_REQ");
			putnext(np->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * np_optmgmt_req: - issue N_OPTMGMT_REQ primitive
 * @np: network private structure
 * @q: active queue
 * @msg: message to free upon success
 * @qos: buffer describing QOS parameters
 * @flags: management flags
 */
static fastcall noinline __unlikely int
np_optmgmt_req(struct np *np, queue_t *q, mblk_t *msg, struct strbuf *qos, np_ulong flags)
{
	N_optmgmt_req_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p) + qos->len, BPRI_MED))) {
		if (canputnext(np->oq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_OPTMGMT_REQ;
			p->QOS_length = qos->len;
			p->QOS_offset = sizeof(*p);
			p->OPTMGMT_flags = flags;
			mp->b_wptr += sizeof(*p);
			bcopy(qos->buf, mp->b_wptr, qos->len);
			mp->b_wptr += qos->len;
			freemsg(msg);
			if (np_get_state(np) == NS_IDLE)
				np_set_state(np, NS_WACK_OPTREQ);
			mi_strlog(np->oq, STRLOGTX, SL_TRACE, "-> N_OPTMGMT_REQ");
			putnext(np->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * np_datack_req: - issue N_DATACK_REQ primitive
 * @np: network private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall inline __hot_write int
np_datack_req(struct np *np, queue_t *q, mblk_t *msg)
{
	N_datack_req_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		if (bcanputnext(np->oq, 1)) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_DATACK_REQ;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			np->datinds--;
			mi_strlog(np->oq, STRLOGDA, SL_TRACE, "-> N_DATACK_REQ");
			putnext(np->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * np_reset_req: - issue N_RESET_REQ primitive
 * @np: network private structure
 * @q: active queue
 * @msg: message to free upon success
 * @reason: reason for reset
 */
static fastcall noinline __unlikely int
np_reset_req(struct np *np, queue_t *q, mblk_t *msg, np_ulong reason)
{
	N_reset_req_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(np->oq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_RESET_REQ;
			p->RESET_reason = reason;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			np_set_state(np, NS_WCON_RREQ);
			mi_strlog(np->oq, STRLOGTX, SL_TRACE, "-> N_RESET_REQ");
			putnext(np->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * np_reset_res: - issue N_RESET_RES primitive
 * @np: network private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall noinline __unlikely int
np_reset_res(struct np *np, queue_t *q, mblk_t *msg)
{
	N_reset_res_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(np->oq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_RESET_RES;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			np_set_state(np, NS_WACK_RRES);
			mi_strlog(np->oq, STRLOGTX, SL_TRACE, "-> N_RESET_RES");
			putnext(np->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 * --------------------------------------------------------------------------
 *
 * TPI USER TO TPI PROVIDER PRIMITIVES RECEIVED FROM UPSTREAM
 *
 * --------------------------------------------------------------------------
 * For all these functions @tp is a pointer to the locked TPI private
 * structure, @q is a pointer to the active queue, @mp is a pointer to the
 * message being processed.
 * --------------------------------------------------------------------------
 */

/**
 * tp_conn_req: - process received T_CONN_REQ primitive
 * @tp: transport private structure
 * @q: active queue (upper write queue)
 * @mp: the T_CONN_REQ primitive
 *
 * The DEST_offset and DEST_length contains a representation of the addressing
 * information neccessary to reach the destination, it may contain (but is not
 * limited to) one or more of the following:
 *
 *  - SNPA identifier,
 *  - destination X.25 address, or
 *  - destination NSAP.
 *
 * When the connection has been established, the RES_offset and RES_length of
 * the T_CONN_CON primitive represnts the address on which the call ahs been
 * accepted.
 *
 * For a PVC, the DEST_offset and DEST_length represents the PVC to be used.
 * If it is already in use, the error [TADDRBUSY] is returned.  On successful
 * return:
 *
 *  - In synchronous mode, the PVC will be in state T_DATAXFER.
 *
 *  - In asynchronous mode, the PVC will be in state T_OUTCON and a T_CONNECT
 *    event will be outstanding.
 *
 * When the connection has been established, the RES_offest and RES_length of
 * the T_CONN_CON represents the actual PVC allocated.
 *
 * For PVCs it is necessary to issue a N_CONN_REQ and await an N_CONN_CON
 * before issuing either the T_OK_ACK and T_CONN_CON or T_ERROR_ACK.  For
 * others it would be wiser to issue the T_OK_ACK before receiving the
 * N_ERROR_ACK or N_CONN_CON.
 */
static fastcall noinline __unlikely int
tp_conn_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_conn_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->DEST_offset, p->DEST_length))
		goto badaddr;
	if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
		goto badopt;
	if (p->CONN_flags & ~0)
		goto badflag;
	if (tp_get_state(tp) != TS_IDLE)
		goto outstate;
	if (tp_get_state(tp) == TS_IDLE)
		goto discard;
	/* FIXME */
      tooshort:
	err = -EFAULT;
	goto error;
      badaddr:
	err = TBADADDR;
	goto error;
      badopt:
	err = TBADOPT;
	goto error;
      badflag:
	err = TBADFLAG;
	goto error;
      outstate:
	err = TOUTSTATE;
	goto error;
      error:
	return (tp_error_ack(tp, q, mp, T_CONN_REQ, err, __FUNCTION__));
      discard:
	freemsg(mp);
	return (0);
}

/**
 * tp_conn_res: - process received T_CONN_RES primitive
 * @tp: transport private structure
 * @q: active queue (upper write queue)
 * @mp: the T_CONN_RES primitive
 */
static fastcall noinline __unlikely int
tp_conn_res(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_conn_res *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->DEST_offset, p->DEST_length))
		goto badaddr;
	if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
		goto badopt;
	if (p->CONN_flags & ~0)
		goto badflag;
	if (tp_get_state(tp) != TS_IDLE)
		goto outstate;
	if (tp_get_state(tp) == TS_IDLE)
		goto discard;
	/* FIXME */
      tooshort:
	err = -EFAULT;
	goto error;
      badaddr:
	err = TBADADDR;
	goto error;
      badopt:
	err = TBADOPT;
	goto error;
      badflag:
	err = TBADFLAG;
	goto error;
      outstate:
	err = TOUTSTATE;
	goto error;
      error:
	return (tp_error_ack(tp, q, mp, T_CONN_RES, err, __FUNCTION__));
      discard:
	freemsg(mp);
	return (0);
}

/**
 * tp_discon_req: - process received T_DISCON_REQ primitive
 * @tp: transport private structure
 * @q: active queue (upper write queue)
 * @mp: the T_DISCON_REQ primitive
 */
static fastcall noinline __unlikely int
tp_discon_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_discon_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->DEST_offset, p->DEST_length))
		goto badaddr;
	if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
		goto badopt;
	if (p->CONN_flags & ~0)
		goto badflag;
	if (tp_get_state(tp) != TS_IDLE)
		goto outstate;
	if (tp_get_state(tp) == TS_IDLE)
		goto discard;
	/* FIXME */
      tooshort:
	err = -EFAULT;
	goto error;
      badaddr:
	err = TBADADDR;
	goto error;
      badopt:
	err = TBADOPT;
	goto error;
      badflag:
	err = TBADFLAG;
	goto error;
      outstate:
	err = TOUTSTATE;
	goto error;
      error:
	return (tp_error_ack(tp, q, mp, T_DISCON_REQ, err, __FUNCTION__));
      discard:
	freemsg(mp);
	return (0);
}

/**
 * tp_data_req: - process received T_DATA_REQ primitive
 * @tp: transport private structure
 * @q: active queue (upper write queue)
 * @mp: the T_DATA_REQ primitive
 *
 * - T_X25_D: If set in flags, the data is sent with the D bit set.  This data
 *   has to be acknowledged by the peer.
 *
 *   As with normal t_snd(3) requests, the user may issue multiple t_snd(3)
 *   requests with the D-bit set that will be queue by the provider.  A
 *   separate acknowledgement is generated for each one.  The normal flow
 *   control mechanism applies; if a t_snd(3) cannot be accepted, the [TFLOW]
 *   code is returned.
 *
 *   Note: As a D-bit send requires end-to-end acknowledgement, it can
 *         considerably delay the transmission of further packets.
 *
 * - T_X25_Q: If set in flags, the data is sent as normal qualified data.
 *
 * - T_X25_RST: If set in flags, this indicates to the underlying provider
 *   that a request or a confirmation of reset is required.
 *
 *   The t_snd(3) function returns immediately.  If further t_snd(3)
 */
static fastcall noinline __unlikely int
tp_data_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_data_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->DEST_offset, p->DEST_length))
		goto badaddr;
	if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
		goto badopt;
	if (p->CONN_flags & ~0)
		goto badflag;
	if (tp_get_state(tp) != TS_IDLE)
		goto outstate;
	if (tp_get_state(tp) == TS_IDLE)
		goto discard;
	/* FIXME */
      tooshort:
	err = -EFAULT;
	goto error;
      badaddr:
	err = TBADADDR;
	goto error;
      badopt:
	err = TBADOPT;
	goto error;
      badflag:
	err = TBADFLAG;
	goto error;
      outstate:
	err = TOUTSTATE;
	goto error;
      error:
	return (tp_error_ack(tp, q, mp, T_DATA_REQ, err, __FUNCTION__));
      discard:
	freemsg(mp);
	return (0);
}

/**
 * tp_exdata_req: - process received T_EXDATA_REQ primitive
 * @tp: transport private structure
 * @q: active queue (upper write queue)
 * @mp: the T_EXDATA_REQ primitive
 */
static fastcall noinline __unlikely int
tp_exdata_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_exdata_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->DEST_offset, p->DEST_length))
		goto badaddr;
	if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
		goto badopt;
	if (p->CONN_flags & ~0)
		goto badflag;
	if (tp_get_state(tp) != TS_IDLE)
		goto outstate;
	if (tp_get_state(tp) == TS_IDLE)
		goto discard;
	/* FIXME */
      tooshort:
	err = -EFAULT;
	goto error;
      badaddr:
	err = TBADADDR;
	goto error;
      badopt:
	err = TBADOPT;
	goto error;
      badflag:
	err = TBADFLAG;
	goto error;
      outstate:
	err = TOUTSTATE;
	goto error;
      error:
	return (tp_error_ack(tp, q, mp, T_EXDATA_REQ, err, __FUNCTION__));
      discard:
	freemsg(mp);
	return (0);
}

/**
 * tp_info_req: - process received T_INFO_REQ primitive
 * @tp: transport private structure
 * @q: active queue (upper write queue)
 * @mp: the T_INFO_REQ primitive
 */
static fastcall noinline __unlikely int
tp_info_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	return (tp_info_ack(tp, q, mp));
}

/**
 * tp_bind_req: - process received T_BIND_REQ primitive
 * @tp: transport private structure
 * @q: active queue (upper write queue)
 * @mp: the T_BIND_REQ primitive
 *
 * The address field of the t_bind(3) structure contains the matching
 * requirements for routing incoming calls to the endpoint.  This may include
 * (but is not limited to) representation of one or more of the following:
 *
 *  - a local SNPA identifier,
 *  - a local X.25 address,
 *  - a local X.25 subaddress,
 *  - a local NSAP,
 *  - a call user data matching requirement, or,
 *  - a PVC number.
 *
 *  Where an incoming call can be routed to multiple endpoints on the basis of
 *  their matching requirements, the actual endpoint selected will be
 *  implementation dependent.
 *
 *  If the application likes to initiate a connection, it can either bind
 *  itself to a NULL address, or use any of the matching requirements defined
 *  above.  If a NULL address is used, the requirements have been defined,
 *  connections may be restricted to SVCs, a certain PVC, or a certain X.25
 *  line, depending on the matching criteria.
 *
 *  It is not possible to receive connection indications on a NULL address.
 *
 *  Note: An implementation may choose not to provide support for a wildcard
 *        mechanism for address information, for example to route incoming
 *        calls whose call user data starts with a particular pattern.
 */
static fastcall noinline __unlikely int
tp_bind_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_bind_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->DEST_offset, p->DEST_length))
		goto badaddr;
	if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
		goto badopt;
	if (p->CONN_flags & ~0)
		goto badflag;
	if (tp_get_state(tp) != TS_IDLE)
		goto outstate;
	if (tp_get_state(tp) == TS_IDLE)
		goto discard;
	/* FIXME */
      tooshort:
	err = -EFAULT;
	goto error;
      badaddr:
	err = TBADADDR;
	goto error;
      badopt:
	err = TBADOPT;
	goto error;
      badflag:
	err = TBADFLAG;
	goto error;
      outstate:
	err = TOUTSTATE;
	goto error;
      error:
	return (tp_error_ack(tp, q, mp, T_BIND_REQ, err, __FUNCTION__));
      discard:
	freemsg(mp);
	return (0);
}

/**
 * tp_unbind_req: - process received T_UNBIND_REQ primitive
 * @tp: transport private structure
 * @q: active queue (upper write queue)
 * @mp: the T_UNBIND_REQ primitive
 */
static fastcall noinline __unlikely int
tp_unbind_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_unbind_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->DEST_offset, p->DEST_length))
		goto badaddr;
	if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
		goto badopt;
	if (p->CONN_flags & ~0)
		goto badflag;
	if (tp_get_state(tp) != TS_IDLE)
		goto outstate;
	if (tp_get_state(tp) == TS_IDLE)
		goto discard;
	/* FIXME */
      tooshort:
	err = -EFAULT;
	goto error;
      badaddr:
	err = TBADADDR;
	goto error;
      badopt:
	err = TBADOPT;
	goto error;
      badflag:
	err = TBADFLAG;
	goto error;
      outstate:
	err = TOUTSTATE;
	goto error;
      error:
	return (tp_error_ack(tp, q, mp, T_UNBIND_REQ, err, __FUNCTION__));
      discard:
	freemsg(mp);
	return (0);
}

/**
 * tp_unitdata_req: - process received T_UNITDATA_REQ primitive
 * @tp: transport private structure
 * @q: active queue (upper write queue)
 * @mp: the T_UNITDATA_REQ primitive
 */
static fastcall noinline __unlikely int
tp_unitdata_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_unitdata_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->DEST_offset, p->DEST_length))
		goto badaddr;
	if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
		goto badopt;
	if (p->CONN_flags & ~0)
		goto badflag;
	if (tp_get_state(tp) != TS_IDLE)
		goto outstate;
	if (tp_get_state(tp) == TS_IDLE)
		goto discard;
	/* FIXME */
      tooshort:
	err = -EFAULT;
	goto error;
      badaddr:
	err = TBADADDR;
	goto error;
      badopt:
	err = TBADOPT;
	goto error;
      badflag:
	err = TBADFLAG;
	goto error;
      outstate:
	err = TOUTSTATE;
	goto error;
      error:
	return (tp_error_ack(tp, q, mp, T_UNITDATA_REQ, err, __FUNCTION__));
      discard:
	freemsg(mp);
	return (0);
}

/**
 * tp_optmgmt_req: - process received T_OPTMGMT_REQ primitive
 * @tp: transport private structure
 * @q: active queue (upper write queue)
 * @mp: the T_OPTMGMT_REQ primitive
 */
static fastcall noinline __unlikely int
tp_optmgmt_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_optmgmt_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->DEST_offset, p->DEST_length))
		goto badaddr;
	if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
		goto badopt;
	if (p->CONN_flags & ~0)
		goto badflag;
	if (tp_get_state(tp) != TS_IDLE)
		goto outstate;
	if (tp_get_state(tp) == TS_IDLE)
		goto discard;
	/* FIXME */
      tooshort:
	err = -EFAULT;
	goto error;
      badaddr:
	err = TBADADDR;
	goto error;
      badopt:
	err = TBADOPT;
	goto error;
      badflag:
	err = TBADFLAG;
	goto error;
      outstate:
	err = TOUTSTATE;
	goto error;
      error:
	return (tp_error_ack(tp, q, mp, T_OPTMGMT_REQ, err, __FUNCTION__));
      discard:
	freemsg(mp);
	return (0);
}

/**
 * tp_ordrel_req: - process received T_ORDREL_REQ primitive
 * @tp: transport private structure
 * @q: active queue (upper write queue)
 * @mp: the T_ORDREL_REQ primitive
 */
static fastcall noinline __unlikely int
tp_ordrel_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_ordrel_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->DEST_offset, p->DEST_length))
		goto badaddr;
	if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
		goto badopt;
	if (p->CONN_flags & ~0)
		goto badflag;
	if (tp_get_state(tp) != TS_IDLE)
		goto outstate;
	if (tp_get_state(tp) == TS_IDLE)
		goto discard;
	/* FIXME */
      tooshort:
	err = -EFAULT;
	goto error;
      badaddr:
	err = TBADADDR;
	goto error;
      badopt:
	err = TBADOPT;
	goto error;
      badflag:
	err = TBADFLAG;
	goto error;
      outstate:
	err = TOUTSTATE;
	goto error;
      error:
	return (tp_error_ack(tp, q, mp, T_ORDREL_REQ, err, __FUNCTION__));
      discard:
	freemsg(mp);
	return (0);
}

/**
 * tp_optdata_req: - process received T_OPTDATA_REQ primitive
 * @tp: transport private structure
 * @q: active queue (upper write queue)
 * @mp: the T_OPTDATA_REQ primitive
 */
static fastcall noinline __unlikely int
tp_optdata_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_optdata_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->DEST_offset, p->DEST_length))
		goto badaddr;
	if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
		goto badopt;
	if (p->CONN_flags & ~0)
		goto badflag;
	if (tp_get_state(tp) != TS_IDLE)
		goto outstate;
	if (tp_get_state(tp) == TS_IDLE)
		goto discard;
	/* FIXME */
      tooshort:
	err = -EFAULT;
	goto error;
      badaddr:
	err = TBADADDR;
	goto error;
      badopt:
	err = TBADOPT;
	goto error;
      badflag:
	err = TBADFLAG;
	goto error;
      outstate:
	err = TOUTSTATE;
	goto error;
      error:
	return (tp_error_ack(tp, q, mp, T_OPTDATA_REQ, err, __FUNCTION__));
      discard:
	freemsg(mp);
	return (0);
}

/**
 * tp_addr_req: - process received T_ADDR_REQ primitive
 * @tp: transport private structure
 * @q: active queue (upper write queue)
 * @mp: the T_ADDR_REQ primitive
 */
static fastcall noinline __unlikely int
tp_addr_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_addr_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->DEST_offset, p->DEST_length))
		goto badaddr;
	if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
		goto badopt;
	if (p->CONN_flags & ~0)
		goto badflag;
	if (tp_get_state(tp) != TS_IDLE)
		goto outstate;
	if (tp_get_state(tp) == TS_IDLE)
		goto discard;
	/* FIXME */
      tooshort:
	err = -EFAULT;
	goto error;
      badaddr:
	err = TBADADDR;
	goto error;
      badopt:
	err = TBADOPT;
	goto error;
      badflag:
	err = TBADFLAG;
	goto error;
      outstate:
	err = TOUTSTATE;
	goto error;
      error:
	return (tp_error_ack(tp, q, mp, T_ADDR_REQ, err, __FUNCTION__));
      discard:
	freemsg(mp);
	return (0);
}

/**
 * tp_capability_req: - process received T_CAPABILITY_REQ primitive
 * @tp: transport private structure
 * @q: active queue (upper write queue)
 * @mp: the T_CAPABILITY_REQ primitive
 */
static fastcall noinline __unlikely int
tp_capability_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_capability_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->DEST_offset, p->DEST_length))
		goto badaddr;
	if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
		goto badopt;
	if (p->CONN_flags & ~0)
		goto badflag;
	if (tp_get_state(tp) != TS_IDLE)
		goto outstate;
	if (tp_get_state(tp) == TS_IDLE)
		goto discard;
	/* FIXME */
      tooshort:
	err = -EFAULT;
	goto error;
      badaddr:
	err = TBADADDR;
	goto error;
      badopt:
	err = TBADOPT;
	goto error;
      badflag:
	err = TBADFLAG;
	goto error;
      outstate:
	err = TOUTSTATE;
	goto error;
      error:
	return (tp_error_ack(tp, q, mp, T_CAPABILITY_REQ, err, __FUNCTION__));
      discard:
	freemsg(mp);
	return (0);
}

/**
 * tp_other_req: - process received other primitive
 * @tp: transport private structure
 * @q: active queue (upper write queue)
 * @mp: the other primitive
 */
static fastcall noinline __unlikely int
tp_other_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	t_uscalar_t *p = (typeof(p)) mp->b_rptr;

	return (tp_error_ack(tp, q, mp, *p, TNOTSUPPORT, __FUNCTION__));
}

/*
 * --------------------------------------------------------------------------
 *
 * NPI PROVIDER TO NPI USER PRIMITIVES RECEIVED FROM DOWNSTREAM
 *
 * --------------------------------------------------------------------------
 * For all these functions @np is a pointer to the locked NPI private
 * structure, @q is a pointer to the active queue (lower read queue), @mp is a
 * pointer to the message being processed.
 * --------------------------------------------------------------------------
 */

/**
 * np_conn_ind: - process received N_CONN_IND primitive
 * @np: network private structure
 * @q: active queue (lower read queue)
 * @mp: the N_CONN_IND primitive
 */
static fastcall noinline __unlikely int
np_conn_ind(struct np *np, queue_t *q, mblk_t *mp)
{
	N_conn_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->DEST_offset, p->DEST_length))
		goto baddr;
	if (!MBLKIN(mp, p->QOS_offset, p->QOS_length))
		goto badopt;
	if (p->CONN_flags & ~(REC_CONF_OPT | EX_DATA_OPT))
		goto badflags;
	if (np_get_state(np) == NS_IDLE)
		goto discard;
	if (np_get_state(np) != NS_IDLE)
		goto outstate;
      tooshort:
	err = -EFAULT;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      badopt:
	err = NBADOPT;
	goto error;
      badflag:
	err = NBADFLAG;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return (m_error(np->tp, q, mp, EFAULT, EFAULT, __FUNCTION__));
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_conn_con: - process received N_CONN_CON primitive
 * @np: network private structure
 * @q: active queue (lower read queue)
 * @mp: the N_CONN_CON primitive
 */
static fastcall noinline __unlikely int
np_conn_con(struct np *np, queue_t *q, mblk_t *mp)
{
	N_conn_con_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->DEST_offset, p->DEST_length))
		goto baddr;
	if (!MBLKIN(mp, p->QOS_offset, p->QOS_length))
		goto badopt;
	if (p->CONN_flags & ~(REC_CONF_OPT | EX_DATA_OPT))
		goto badflags;
	if (np_get_state(np) == NS_IDLE)
		goto discard;
	if (np_get_state(np) != NS_IDLE)
		goto outstate;
      tooshort:
	err = -EFAULT;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      badopt:
	err = NBADOPT;
	goto error;
      badflag:
	err = NBADFLAG;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return (m_error(np->tp, q, mp, EFAULT, EFAULT, __FUNCTION__));
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_discon_ind: - process received N_DISCON_IND primitive
 * @np: network private structure
 * @q: active queue (lower read queue)
 * @mp: the N_DISCON_IND primitive
 */
static fastcall noinline __unlikely int
np_discon_ind(struct np *np, queue_t *q, mblk_t *mp)
{
	N_discon_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->DEST_offset, p->DEST_length))
		goto baddr;
	if (!MBLKIN(mp, p->QOS_offset, p->QOS_length))
		goto badopt;
	if (p->CONN_flags & ~(REC_CONF_OPT | EX_DATA_OPT))
		goto badflags;
	if (np_get_state(np) == NS_IDLE)
		goto discard;
	if (np_get_state(np) != NS_IDLE)
		goto outstate;
      tooshort:
	err = -EFAULT;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      badopt:
	err = NBADOPT;
	goto error;
      badflag:
	err = NBADFLAG;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return (m_error(np->tp, q, mp, EFAULT, EFAULT, __FUNCTION__));
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_data_ind: - process received N_DATA_IND primitive
 * @np: network private structure
 * @q: active queue (lower read queue)
 * @mp: the N_DATA_IND primitive
 */
static fastcall noinline __unlikely int
np_data_ind(struct np *np, queue_t *q, mblk_t *mp)
{
	N_data_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->DEST_offset, p->DEST_length))
		goto baddr;
	if (!MBLKIN(mp, p->QOS_offset, p->QOS_length))
		goto badopt;
	if (p->CONN_flags & ~(REC_CONF_OPT | EX_DATA_OPT))
		goto badflags;
	if (np_get_state(np) == NS_IDLE)
		goto discard;
	if (np_get_state(np) != NS_IDLE)
		goto outstate;
      tooshort:
	err = -EFAULT;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      badopt:
	err = NBADOPT;
	goto error;
      badflag:
	err = NBADFLAG;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return (m_error(np->tp, q, mp, EFAULT, EFAULT, __FUNCTION__));
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_exdata_ind: - process received N_EXDATA_IND primitive
 * @np: network private structure
 * @q: active queue (lower read queue)
 * @mp: the N_EXDATA_IND primitive
 */
static fastcall noinline __unlikely int
np_exdata_ind(struct np *np, queue_t *q, mblk_t *mp)
{
	N_exdata_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->DEST_offset, p->DEST_length))
		goto baddr;
	if (!MBLKIN(mp, p->QOS_offset, p->QOS_length))
		goto badopt;
	if (p->CONN_flags & ~(REC_CONF_OPT | EX_DATA_OPT))
		goto badflags;
	if (np_get_state(np) == NS_IDLE)
		goto discard;
	if (np_get_state(np) != NS_IDLE)
		goto outstate;
      tooshort:
	err = -EFAULT;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      badopt:
	err = NBADOPT;
	goto error;
      badflag:
	err = NBADFLAG;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return (m_error(np->tp, q, mp, EFAULT, EFAULT, __FUNCTION__));
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_info_ack: - process received N_INFO_ACK primitive
 * @np: network private structure
 * @q: active queue (lower read queue)
 * @mp: the N_INFO_ACK primitive
 */
static fastcall noinline __unlikely int
np_info_ack(struct np *np, queue_t *q, mblk_t *mp)
{
	N_info_ack_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->DEST_offset, p->DEST_length))
		goto baddr;
	if (!MBLKIN(mp, p->QOS_offset, p->QOS_length))
		goto badopt;
	if (p->CONN_flags & ~(REC_CONF_OPT | EX_DATA_OPT))
		goto badflags;
	if (np_get_state(np) == NS_IDLE)
		goto discard;
	if (np_get_state(np) != NS_IDLE)
		goto outstate;
      tooshort:
	err = -EFAULT;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      badopt:
	err = NBADOPT;
	goto error;
      badflag:
	err = NBADFLAG;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return (m_error(np->tp, q, mp, EFAULT, EFAULT, __FUNCTION__));
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_bind_ack: - process received N_BIND_ACK primitive
 * @np: network private structure
 * @q: active queue (lower read queue)
 * @mp: the N_BIND_ACK primitive
 */
static fastcall noinline __unlikely int
np_bind_ack(struct np *np, queue_t *q, mblk_t *mp)
{
	N_bind_ack_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->DEST_offset, p->DEST_length))
		goto baddr;
	if (!MBLKIN(mp, p->QOS_offset, p->QOS_length))
		goto badopt;
	if (p->CONN_flags & ~(REC_CONF_OPT | EX_DATA_OPT))
		goto badflags;
	if (np_get_state(np) == NS_IDLE)
		goto discard;
	if (np_get_state(np) != NS_IDLE)
		goto outstate;
      tooshort:
	err = -EFAULT;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      badopt:
	err = NBADOPT;
	goto error;
      badflag:
	err = NBADFLAG;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return (m_error(np->tp, q, mp, EFAULT, EFAULT, __FUNCTION__));
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_error_ack: - process received N_ERROR_ACK primitive
 * @np: network private structure
 * @q: active queue (lower read queue)
 * @mp: the N_ERROR_ACK primitive
 */
static fastcall noinline __unlikely int
np_error_ack(struct np *np, queue_t *q, mblk_t *mp)
{
	N_error_ack_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->DEST_offset, p->DEST_length))
		goto baddr;
	if (!MBLKIN(mp, p->QOS_offset, p->QOS_length))
		goto badopt;
	if (p->CONN_flags & ~(REC_CONF_OPT | EX_DATA_OPT))
		goto badflags;
	if (np_get_state(np) == NS_IDLE)
		goto discard;
	if (np_get_state(np) != NS_IDLE)
		goto outstate;
      tooshort:
	err = -EFAULT;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      badopt:
	err = NBADOPT;
	goto error;
      badflag:
	err = NBADFLAG;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return (m_error(np->tp, q, mp, EFAULT, EFAULT, __FUNCTION__));
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_ok_ack: - process received N_OK_ACK primitive
 * @np: network private structure
 * @q: active queue (lower read queue)
 * @mp: the N_OK_ACK primitive
 */
static fastcall noinline __unlikely int
np_ok_ack(struct np *np, queue_t *q, mblk_t *mp)
{
	N_ok_ack_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->DEST_offset, p->DEST_length))
		goto baddr;
	if (!MBLKIN(mp, p->QOS_offset, p->QOS_length))
		goto badopt;
	if (p->CONN_flags & ~(REC_CONF_OPT | EX_DATA_OPT))
		goto badflags;
	if (np_get_state(np) == NS_IDLE)
		goto discard;
	if (np_get_state(np) != NS_IDLE)
		goto outstate;
      tooshort:
	err = -EFAULT;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      badopt:
	err = NBADOPT;
	goto error;
      badflag:
	err = NBADFLAG;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return (m_error(np->tp, q, mp, EFAULT, EFAULT, __FUNCTION__));
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_unitdata_ind: - process received N_UNITDATA_IND primitive
 * @np: network private structure
 * @q: active queue (lower read queue)
 * @mp: the N_UNITDATA_IND primitive
 */
static fastcall noinline __unlikely int
np_unitdata_ind(struct np *np, queue_t *q, mblk_t *mp)
{
	N_unitdata_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->DEST_offset, p->DEST_length))
		goto baddr;
	if (!MBLKIN(mp, p->QOS_offset, p->QOS_length))
		goto badopt;
	if (p->CONN_flags & ~(REC_CONF_OPT | EX_DATA_OPT))
		goto badflags;
	if (np_get_state(np) == NS_IDLE)
		goto discard;
	if (np_get_state(np) != NS_IDLE)
		goto outstate;
      tooshort:
	err = -EFAULT;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      badopt:
	err = NBADOPT;
	goto error;
      badflag:
	err = NBADFLAG;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return (m_error(np->tp, q, mp, EFAULT, EFAULT, __FUNCTION__));
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_uderror_ind: - process received N_UDERROR_IND primitive
 * @np: network private structure
 * @q: active queue (lower read queue)
 * @mp: the N_UDERROR_IND primitive
 */
static fastcall noinline __unlikely int
np_uderror_ind(struct np *np, queue_t *q, mblk_t *mp)
{
	N_uderror_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->DEST_offset, p->DEST_length))
		goto baddr;
	if (!MBLKIN(mp, p->QOS_offset, p->QOS_length))
		goto badopt;
	if (p->CONN_flags & ~(REC_CONF_OPT | EX_DATA_OPT))
		goto badflags;
	if (np_get_state(np) == NS_IDLE)
		goto discard;
	if (np_get_state(np) != NS_IDLE)
		goto outstate;
      tooshort:
	err = -EFAULT;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      badopt:
	err = NBADOPT;
	goto error;
      badflag:
	err = NBADFLAG;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return (m_error(np->tp, q, mp, EFAULT, EFAULT, __FUNCTION__));
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_datack_ind: - process received N_DATACK_IND primitive
 * @np: network private structure
 * @q: active queue (lower read queue)
 * @mp: the N_DATACK_IND primitive
 */
static fastcall noinline __unlikely int
np_datack_ind(struct np *np, queue_t *q, mblk_t *mp)
{
	N_datack_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->DEST_offset, p->DEST_length))
		goto baddr;
	if (!MBLKIN(mp, p->QOS_offset, p->QOS_length))
		goto badopt;
	if (p->CONN_flags & ~(REC_CONF_OPT | EX_DATA_OPT))
		goto badflags;
	if (np_get_state(np) == NS_IDLE)
		goto discard;
	if (np_get_state(np) != NS_IDLE)
		goto outstate;
      tooshort:
	err = -EFAULT;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      badopt:
	err = NBADOPT;
	goto error;
      badflag:
	err = NBADFLAG;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return (m_error(np->tp, q, mp, EFAULT, EFAULT, __FUNCTION__));
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_reset_ind: - process received N_RESET_IND primitive
 * @np: network private structure
 * @q: active queue (lower read queue)
 * @mp: the N_RESET_IND primitive
 *
 * XX25 specifies that reset indications are translated into data indications
 * that have the special T_X25_RST flags bit set, an zero, one or two octet
 * data containing the cause and diagnostic codes.  This is only performed;
 * however, if the T_X25_RST_OPT option is set, (tp->opts.x25.rst_opt ==
 * T_YES).  Otherwise, a disconnect indication is returned.
 *
 * XX25 passes the X.25 cause and diagnostic up in the M_DATA block; however,
 * the only documented way of doing that for NPI is the AIX/X.25 that adds two
 * more fields to the N_RESET_IND structure.
 */
static fastcall noinline __unlikely int
np_reset_ind(struct np *np, queue_t *q, mblk_t *mp)
{
	N_reset_ind_t *p = (typeof(p)) mp->b_rptr;
	struct tp *tp = np->tp;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (np_get_state(np) == NS_IDLE)
		goto discard;
	if (np_get_state(np) != NS_DATA_XFER)
		goto outstate;
	if (tp->opts.x25.rst_opt == T_YES) {
		/* skip cause and diag for now */
		np_set_state(np, NS_WRES_RIND);
		return tp_data_ind(tp, q, mp, T_X25_RST, NULL);
	} else {
		struct strbuf res = { 0, };

		if ((tp_get_state(tp) == TS_DATA_XFER)) {
			t_uscalar_t DISCON_reason;

			/* won't get here if we pass through twice */
			DISCON_reason = (p->RESET_orig << 16) | p->RESET_reason;
			if ((err = tp_discon_ind(tp, q, NULL, DISCON_reason, 0, NULL)))
				goto error;
		}
		return np_discon_req(np, q, mp, N_DISC_T, &res, 0, NULL);
	}
      tooshort:
	err = -EFAULT;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return (m_error(np->tp, q, mp, EFAULT, EFAULT, __FUNCTION__));
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_reset_con: - process received N_RESET_CON primitive
 * @np: network private structure
 * @q: active queue (lower read queue)
 * @mp: the N_RESET_CON primitive
 *
 * Same thing for reset confirmations as reset indications.
 */
static fastcall noinline __unlikely int
np_reset_con(struct np *np, queue_t *q, mblk_t *mp)
{
	N_reset_con_t *p = (typeof(p)) mp->b_rptr;
	struct tp *tp = np->tp;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (np_get_state(np) == NS_IDLE)
		goto discard;
	if (np_get_state(np) != NS_WCON_RREQ)
		goto outstate;
	if (tp->opts.x25.rst_opt == T_YES) {
		/* skip cause and diag for now */
		return tp_data_ind(tp, q, mp, T_X25_RST, NULL);
	} else {
		struct strbuf res = { 0, };

		if ((tp_get_state(tp) == TS_DATA_XFER)) {
			t_uscalar_t DISCON_reason = 0xf1;	/* FIXME */

			/* won't get here if we pass through twice */
			if ((err = tp_discon_ind(tp, q, NULL, DISCON_reason, 0, NULL)))
				goto error;
		}
		return np_discon_req(np, q, mp, N_DISC_T, &res, 0, NULL);
	}
      tooshort:
	err = -EFAULT;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return (m_error(np->tp, q, mp, EFAULT, EFAULT, __FUNCTION__));
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_other_ind: - process received other primitive
 * @np: network private structure
 * @q: active queue (lower read queue)
 * @mp: the other primitive
 */
static fastcall noinline __unlikely int
np_other_ind(struct np *np, queue_t *q, mblk_t *mp)
{
	return (m_error(np->tp, q, mp, EFAULT, EFAULT, __FUNCTION__));
}

/*
 * --------------------------------------------------------------------------
 * --------------------------------------------------------------------------
 */
static int
tp_conn_req(struct tp *tp, queue_t *q, mblk_t *bp)
{
	struct T_conn_req *p = (typeof(p)) bp->b_rptr;
	struct T_ok_ack *o;
	struct t_opthdr *t;
	N_conn_req_t *r;
	mblk_t *mp = NULL, *rp = NULL;
	int err;

	if (!MBLKIN(bp, 0, sizeof(*p)))
		goto badproto;
	if (!MBLKIN(bp, p->DEST_offset, p->DEST_length))
		goto badaddr;
	if (!MBLKIN(bp, p->OPT_offset, p->OPT_length))
		goto badopt;
	if (!(mp = mi_allocb(q, sizeof(*r) + p->DEST_length, BPRI_MED)))
		goto enobufs;
	if (!(rp = mi_allocb(q, sizeof(*o), BPRI_MED)))
		goto enobufs;
	if (!pcmsg(DB_TYPE(bp)) && !bcanputnext(q, bp->b_band))
		goto ebusy;
	DB_TYPE(mp) = DB_TYPE(bp);
	mp->b_band = bp->b_band;
	r = (typeof(r)) mp->b_rptr;
	mp->b_wptr += sizeof(*r);
	r->PRIM_type = N_CONN_REQ;
	r->DEST_length = p->DEST_length;
	r->DEST_offset = sizeof(*r);
	r->CONN_flags = DEFAULT_RC_SEL;
	r->QOS_length = sizeof(*qos);
	r->QOS_offset = PAD4(p->DEST_offset + p->DEST_length);
	bcopy(bp->b_rptr + p->DEST_offset, mp->b_wptr, p->DEST_length);
	mp->b_wptr = mp->b_rptr + r->QOS_offset;
	qos = (typeof(qos)) mp->b_wptr;
	mp->b_wptr += sizeof(*qos);
	bzero(qos, sizeof(*qos));
	qos->n_qos_type = N_QOS_CO_X25_RANGE;
	for (t = _T_OPT_FIRSTHDR_OFS(bp->b_rptr, p->OPT_length, p->OPT_offset);
	     t; t = _T_OPT_NEXTHDR_OFS(bp->b_rptr, p->OPT_length, t, p->OPT_offset)) {
		if (t->len < sizeof(*t))
			goto badopt;
		switch (t->level) {
		case XTI_GENERIC:
			break;
		case T_X25_NP:
			switch (t->name) {
			case T_X25_USER_DACK:
				if (t->len < sizeof(*t) + sizeof(t_scalar_t))
					goto badopt;
				switch (*(t_scalar_t *) T_OPT_DATA(t)) {
				case T_YES:
					r->CONN_flags &= ~DEFAULT_RC_SEL;
					break;
				case T_NO:
					r->CONN_flags |= DEFAULT_RC_SEL;
					break;
				default:
					goto badopt;
				}
				break;
			case T_X25_USER_EACK:
				if (t->len < sizeof(*t) + sizeof(t_scalar_t))
					goto badopt;
				switch (*(t_scalar_t *) T_OPT_DATA(t)) {
				case T_YES:
					r->CONN_flags &= ~DEFAULT_RC_SEL;
					break;
				case T_NO:
					r->CONN_flags |= DEFAULT_RC_SEL;
					break;
				default:
					goto badopt;
				}
				break;
			case T_X25_D_OPT:
				if (t->len < sizeof(*t) + sizeof(t_scalar_t))
					goto badopt;
				switch (*(t_scalar_t *) T_OPT_DATA(t)) {
				case T_YES:
					r->CONN_flags |= REC_CONF_OPT;
					break;
				case T_NO:
					r->CONN_flags &= ~REC_CONF_OPT;
					break;
				default:
					goto badopt;
				}
				break;
			case T_X25_EDN:
				if (t->len < sizeof(*t) + sizeof(t_scalar_t))
					goto badopt;
				switch (*(t_scalar_t *) T_OPT_DATA(t)) {
				case T_YES:
					r->CONN_flags |= EX_DATA_OPT;
					break;
				case T_NO:
					r->CONN_flags &= ~EX_DATA_OPT;
					break;
				default:
					goto badopt;
				}
				break;
			}
			break;
		}
	}
	mp->b_cont = XCHG(&bp->b_cont, NULL);
	freeb(bp);
	/* TPI needs an T_OK_ACK in response to T_CONN_REQ. */
	DB_TYPE(rp) = M_PCPROTO;
	o = (typeof(o)) rp->b_rptr;
	rp->b_wptr += sizeof(*o);
	o->PRIM_type = T_OK_ACK;
	o->CORRECT_prim = T_CONN_REQ;
	xp->chkstate = np_get_state(xp);
	tp_set_state(xp, TS_WACK_CREQ);
	qreply(q, rp);
	np_set_state(xp, NS_WCON_CREQ);
	tp_set_state(xp, TS_WCON_CREQ);
	putnext(q, mp);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	freemsg(mp);		/* freemsg can handle NULLs */
	freemsg(rp);		/* freemsg can handle NULLs */
	return (err);
}
static int
tp_conn_res(struct tp *tp, queue_t *q, mblk_t *bp)
{
	struct T_conn_res *p = (typeof(p)) bp->b_rptr;
	N_conn_res_t *r;
	mblk_t *mp = NULL;
	int err;

	if (!MBLKIN(bp, 0, sizeof(*p)))
		goto badprim;
	if (!MBLKIN(bp, p->OPT_offset, p->OPT_length))
		goto badopt;
	if (!(mp = mi_allocb(q, sizeof(*r) + xp->LOCADDR_length + sizeof(*qos), BPRI_MED)))
		goto enobufs;
	if (!pcmsg(DB_TYPE(bp)) && !bcanputnext(q, bp->b_band))
		goto ebusy;
	DB_TYPE(mp) = DB_TYPE(bp);
	mp->b_band = bp->b_band;
	r = (typeof(r)) mp->b_rptr;
	mp->b_wptr += sizeof(*r);
	r->PRIM_type = N_CONN_RES;
	r->TOKEN_value = p->ACCEPTOR_id;
	r->RES_length = xp->LOCADDR_length;
	r->RES_offset = xp->LOCADDR_length ? sizeof(*r) : 0;
	r->SEQ_number = p->SEQ_number;
	r->CONN_flags = DEFAULT_RC_SEL;
	r->QOS_length = sizeof(*qos);
	r->QOS_offset = PAD4(p->LOCADDR_length + sizeof(*r));
	bcopy(xp->LOCADDR_address, mp->b_rptr, xp->LOCADDR_length);
	mp->b_wptr = mp->b_rptr + r->QOS_offset;
	qos = (typeof(qos)) mp->b_wptr;
	mp->b_wptr += sizeof(qos);
	bzero(qos, sizeof(*qos));
	qos->n_qos_type = N_QOS_CO_X25_SEL;
	for (t = _T_OPT_FIRSTHDR_OFS(bp->b_rptr, p->OPT_length, p->OPT_offset);
	     t; t = _T_OPT_NEXTHDR_FS(bp->b_rptr, p->OPT_length, p->OPT_offset)) {
		if (t->len < sizeof(*t))
			goto badopt;
		switch (t->level) {
		case XTI_GENERIC:
			break;
		case T_X25_NP:
			switch (t->name) {
			case T_X25_USER_DACK:
				if (t->len < sizeof(*t) + sizeof(t_scalar_t))
					goto badopt;
				switch (*(t_scalar_t *) T_OPT_DATA(t)) {
				case T_YES:
					r->CONN_flags &= ~DEFAULT_RC_SEL;
					break;
				case T_NO:
					r->CONN_flags |= DEFAULT_RC_SEL;
					break;
				default:
					goto badopt;
				}
				break;
			case T_X25_USER_EACK:
				if (t->len < sizeof(*t) + sizeof(t_scalar_t))
					goto badopt;
				switch (*(t_scalar_t *) T_OPT_DATA(t)) {
				case T_YES:
					r->CONN_flags &= ~DEFAULT_RC_SEL;
					break;
				case T_NO:
					r->CONN_flags |= DEFAULT_RC_SEL;
					break;
				default:
					goto badopt;
				}
				break;
			case T_X25_D_OPT:
				if (t->len < sizeof(*t) + sizeof(t_scalar_t))
					goto badopt;
				switch (*(t_scalar_t *) T_OPT_DATA(t)) {
				case T_YES:
					r->CONN_flags |= REC_CONF_OPT;
					break;
				case T_NO:
					r->CONN_flags &= ~REC_CONF_OPT;
					break;
				default:
					goto badopt;
				}
				break;
			case T_X25_EDN:
				if (t->len < sizeof(*t) + sizeof(t_scalar_t))
					goto badopt;
				switch (*(t_scalar_t *) T_OPT_DATA(t)) {
				case T_YES:
					r->CONN_flags |= EX_DATA_OPT;
					break;
				case T_NO:
					r->CONN_flags &= ~EX_DATA_OPT;
					break;
				default:
					goto badopt;
				}
				break;
			}
			break;
		}
	}
	mp->b_cont = XCHG(&bp->b_cont, NULL);
	freeb(bp);
	xp->chkstate = np_get_state(xp);
	np_set_state(xp, NS_WACK_CRES);
	tp_set_state(xp, TS_WACK_CRES);
	putnext(q, mp);
	return (0);
      badprim:
	err = -EPROTO;
	goto terror;
      badopt:
	err = TBADOPT;
	goto terror;
      terror:
	return tp_error_ack(xp, q, bp, T_CONN_RES, err);
      ebusy:
	err = -EBUSY;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	freemsg(mp);
	return (err);
}
static int
tp_discon_req(struct tp *tp, queue_t *q, mblk_t *bp)
{
	struct T_discon_req *p = (typeof(p)) bp->b_rptr;
	N_discon_req_t *r;
	mblk_t *mp = NULL;
	int err;

	if (!MBLKIN(bp, 0, sizeof(*p)))
		goto badprim;
	if (!(mp = mi_allocb(q, sizeof(*r) + xp->LOCADDR_length, BPRI_MED)))
		goto enobufs;
	if (!pcmsg(DB_TYPE(bp)) && !bcanputnext(q, bp->b_band))
		goto ebusy;
	DB_TYPE(mp) = DB_TYPE(bp);
	mp->b_band = bp->b_band;
	r = (typeof(r)) mp->b_rptr;
	mp->b_wptr += sizeof(*r);
	r->PRIM_type = N_DISCON_REQ;
	r->DISCON_reason = xp->DISCON_reason;
	r->RES_length = xp->LOCADDR_length;
	r->RES_offset = xp->LOCADDR_length ? sizeof(*r) : 0;
	r->SEQ_number = p->SEQ_number;
	mp->b_cont = XCHG(&bp->b_cont, NULL);
	bcopy(xp->LOCADDR_address, mp->b_wptr, xp->LOCADDR_length);
	mp->b_wptr += xp->LOCADDR_length;
	freeb(bp);
	xp->chkstate = np_get_state(xp);
	switch (np_get_state(xp)) {
	case NS_WCON_CREQ:
		np_set_state(xp, NS_WACK_DREQ6);
	case NS_WRES_CIND:
		np_set_state(xp, NS_WACK_DREQ7);
	default:
	case NS_DATA_XFER:
		np_set_state(xp, NS_WACK_DREQ9);
	case NS_WCON_RREQ:
		np_set_state(xp, NS_WACK_DREQ10);
	case NS_WRES_RIND:
		np_set_state(xp, NS_WACK_DREQ11);
	}
	putnext(q, mp);
	return (0);
      enobufs:
	err = -ENOBUFS;
	goto error;
      ebusy:
	err = -EBUSY;
	goto error;
      error:
	freemsg(mp);
	return (err);
}
static int
tp_data_req(struct tp *tp, queue_t *q, mblk_t *bp)
{
	struct T_data_req *p = (typeof(p)) bp->b_rptr;
	mblk_t *mp;

	if (!MBLKIN(bp, 0, sizeof(*p)))
		goto badprim;
	if (p->MORE_flag & T_X25_RST) {
		N_reset_req_t *r;

		if (p->MORE_flag & ~T_X25_RST)
			goto badflag;
		if (!(mp = mi_allocb(q, sizeof(*r), BPRI_MED)))
			goto enobufs;
		if (!pcmsg(DB_TYPE(bp)) && !bcanputnext(q, bp->b_band))
			goto ebusy;
		r = (typeof(r)) mp->b_rptr;
		mp->b_wptr += sizeof(*r);
		r->PRIM_type = N_RESET_REQ;
		r->RESET_reason = 0;
		xp->chkstate = np_get_state(xp);
		np_set_state(xp, NS_WCON_RREQ);
	} else if (p->MORE_flag & (T_X25_DACK | T_X25_EACK)) {
		N_datack_req_t *r;

		if (p->MORE_flag & ~(T_X25_DACK | T_X25_EACK))
			goto badflag;
		if (!(mp = mi_allocb(q, sizeof(*r), BPRI_MED)))
			goto enobufs;
		if (!pcmsg(DB_TYPE(bp)) && !bcanputnext(q, bp->b_band))
			goto ebusy;
		r = (typeof(r)) mp->b_rptr;
		mp->b_wptr += sizeof(*r);
		r->PRIM_type = N_DATACK_REQ;
	} else {
		N_data_req_t *r;

		if (p->MORE_flag & ~(T_MORE | T_X25_Q | T_X25_D))
			goto badflag;
		if (!(mp = mi_allocb(q, sizeof(*r), BPRI_MED)))
			goto enobufs;
		if (!pcmsg(DB_TYPE(bp)) && !bcanputnext(q, bp->b_band))
			goto ebusy;
		r = (typeof(r)) mp->b_rptr;
		mp->b_wptr += sizeof(*r);
		r->PRIM_type = N_DATA_REQ;
		r->DATA_xfer_flags = 0;
		if (p->MORE_flag & T_MORE)
			r->DATA_xfer_flags |= N_MORE_DATA_FLAG;
		if (p->MORE_flag & T_X25_D)
			r->DATA_xfer_flags |= N_RC_FLAG;
	}
	mp->b_cont = XCHG(&bp->b_cont, NULL);
	freeb(bp);
	putnext(q, mp);
	return (0);
      badprim:
	err = EPROTO;
	goto merror;
      badflag:
	err = EPROTO;
	goto merror;
      merror:
	return m_error(xp, q, bp, err, err);
      enobufs:
	err = -ENOBUFS;
	goto error;
      ebusy:
	err = -EBUSY;
	goto error;
      error:
	freemsg(mp);
	return (err);
}
static int
tp_exdata_req(struct tp *tp, queue_t *q, mblk_t *bp)
{
	struct T_exdata_req *p = (typeof(p)) bp->b_rptr;
	N_exdata_req_t *r;
	mblk_t *mp = NULL;
	int err;

	if (!MBLKIN(bp, 0, sizeof(*p)))
		goto badprim;
	if (p->MORE_flag != 0)
		goto badflag;
	if (!(mp = mi_allocb(q, sizeof(*r), BPRI_MED)))
		goto enobufs;
	if (!pcmsg(DB_TYPE(bp)) && !bcanputnext(q, bp->b_band))
		goto ebusy;
	DB_TYPE(mp) = DB_TYPE(bp);
	mp->b_band = bp->b_band;
	r = (typeof(r)) mp->b_rptr;
	mp->b_wptr += sizeof(*r);
	r->PRIM_type = N_EXDATA_REQ;
	mp->b_cont = XCHG(&bp->b_cont, NULL);
	freeb(bp);
	putnext(q, mp);
	return (0);
      badprim:
	err = EPROTO;
	goto merror;
      badflag:
	err = EPROTO;
	goto merror;
      merror:
	return m_error(xp, q, bp, err, err);
      enobufs:
	err = -ENOBUFS;
	goto error;
      ebusy:
	err = -EBUSY;
	goto error;
      error:
	freemsg(mp);
	return (err);
}
static int
tp_info_req(struct tp *tp, queue_t *q, mblk_t *bp)
{
	struct T_info_req *p = (typeof(p)) bp->b_rptr;
	N_info_req_t *r;
	mblk_t *mp = NULL;
	int err;

	if (!MBLKIN(bp, 0, sizeof(*p)))
		goto badprim;
	if (!(mp = mi_allocb(q, sizeof(*r), BPRI_MED)))
		goto enobufs;
	if (!pcmsg(DB_TYPE(bp)) && !bcanputnext(q, bp->b_band))
		goto ebusy;
	DB_TYPE(mp) = DB_TYPE(bp);
	mp->b_band = bp->b_band;
	r = (typeof(r)) mp->b_rptr;
	mp->b_wptr += sizeof(*r);
	r->PRIM_type = N_INFO_REQ;
	freemsg(bp);
	xp->wack_ireq++;
	putnext(q, mp);
	return (0);
      badprim:
	err = EPROTO;
	goto terror;
      terror:
	return tp_error_ack(xp, q, bp, T_INFO_REQ, err);
      enobufs:
	err = -ENOBUFS;
	goto error;
      ebusy:
	err = -EBUSY;
	goto error;
      error:
	freemsg(mp);
	return (err);
}
static int
tp_bind_req(struct tp *tp, queue_t *q, mblk_t *bp)
{
	struct T_bind_req *p = (typeof(p)) bp->b_rptr;
	N_bind_req_t *r;
	mblk_t *mp = NULL;
	int err;

	if (!MBLKIN(bp, 0, sizeof(*p)))
		goto badprim;
	if (!MBLKIN(bp, p->ADDR_offset, p->ADDR_length))
		goto badaddr;
	if (!(mp = mi_allocb(q, sizeof(*r) + p->ADDR_length, BPRI_MED)))
		goto enobufs;
	if (!pcmsg(DB_TYPE(bp)) && !bcanputnext(q, bp->b_band))
		goto ebusy;
	DB_TYPE(mp) = DB_TYPE(bp);
	mp->b_band = bp->b_band;
	r = (typeof(r)) mp->b_rptr;
	mp->b_wptr += sizeof(*r);
	r->PRIM_type = N_BIND_REQ;
	r->ADDR_length = p->ADDR_length;
	r->ADDR_offset = p->ADDR_length ? sizeof(*r) : 0;
	r->CONIND_number = p->CONIND_number;
	r->BIND_flags = 0;
	r->PROTOID_length = 0;
	r->PROTOID_offset = 0;
	bcopy(bp->b_rptr + p->ADDR_offset, mp->b_wptr, p->ADDR_length);
	mp->b_wptr += p->ADDR_length;
	freemsg(bp);
	xp->chkstate = np_get_state(xp);
	np_set_state(xp, NS_WACK_BREQ);
	tp_set_state(xp, TS_WACK_BREQ);
	putnext(q, mp);
	return (0);
}
static int
tp_unbind_req(struct tp *tp, queue_t *q, mblk_t *bp)
{
	struct T_unbind_req *p = (typeof(p)) bp->b_rptr;
	N_unbind_req_t *r;
	mblk_t *mp = NULL;
	int err;

	if (!MBLKIN(bp, 0, sizeof(*p)))
		goto badprim;
	if (!(mp = mi_allocb(q, sizeof(*r), BPRI_MED)))
		goto enobufs;
	if (!pcmsg(DB_TYPE(bp)) && !bcanputnext(q, bp->b_band))
		goto ebusy;
	DB_TYPE(mp) = DB_TYPE(bp);
	mp->b_band = bp->b_band;
	r = (typeof(r)) mp->b_rptr;
	mp->b_wptr += sizeof(*r);
	r->PRIM_type = N_UNBIND_REQ;
	freemsg(bp);
	xp->chkstate = np_get_state(xp);
	np_set_state(xp, NS_WACK_UREQ);
	tp_set_state(xp, TS_WACK_UREQ);
	putnext(q, mp);
	return (0);
}
static int
tp_unitdata_req(struct tp *tp, queue_t *q, mblk_t *bp)
{
	return tp_error_ack(xp, q, bp, T_UNITDATA_REQ, TNOTSUPPORT);
}
static int
tp_optmgmt_req(struct tp *tp, queue_t *q, mblk_t *bp)
{
	struct T_optmgmt_req *p = (typeof(p)) bp->b_rptr;
	N_optmgmt_req_t *r;
	mblk_t *mp;
}
static int
tp_ordrel_req(struct tp *tp, queue_t *q, mblk_t *bp)
{
	struct T_ordrel_req *p = (typeof(p)) bp->b_rptr;
	N_discon_req_t *r;
	mblk_t *mp;

	if (!MBLKIN(bp, 0, sizeof(*p)))
		goto badprim;
	goto notsupport;
	if (!(mp = mi_allocb(q, sizeof(*r) + xp->LOCADDR_length, BPRI_MED)))
		goto enobufs;
	if (!pcmsg(DB_TYPE(bp)) && !bcanputnext(q, bp->b_band))
		goto ebusy;
	DB_TYPE(mp) = DB_TYPE(bp);
	mp->b_band = bp->b_band;
	r = (typeof(r)) mp->b_rptr;
	mp->b_wptr += sizeof(*r);
	r->PRIM_type = N_DISCON_REQ;
	r->DISCON_reason = 0;	/* FIXME */
	r->RES_length = xp->LOCADDR_length;
	r->RES_offset = xp->LOCADDR_length ? sizeof(*r) : 0;
	r->SEQ_number = 0;
	bcopy(xp->LOCADDR_address, mp->b_wptr, xp->LOCADDR_length);
	mp->b_wptr += xp->LOCADDR_length;
	mp->b_cont = XCHG(&bp->b_cont, NULL);
	freeb(bp);
	xp->chkstate = np_get_state(xp);
	switch (np_get_state(xp)) {
	case NS_WCON_CREQ:
		np_set_state(xp, NS_WACK_DREQ6);
	case NS_WRES_CIND:
		np_set_state(xp, NS_WACK_DREQ7);
	default:
	case NS_DATA_XFER:
		np_set_state(xp, NS_WACK_DREQ9);
	case NS_WCON_RREQ:
		np_set_state(xp, NS_WACK_DREQ10);
	case NS_WRES_RIND:
		np_set_state(xp, NS_WACK_DREQ11);
	}
	putnext(q, mp);
	return (0);
      badprim:
	err = -EPROTO;
	goto terror;
      notsupport:
	err = TNOTSUPPORT;
	goto terror;
      terror:
	return tp_error_ack(xp, q, bp, T_ORDREL_REQ, err);
      enobufs:
	err = -ENOBUFS;
	goto error;
      ebusy:
	err = -EBUSY;
	goto error;
      error:
	freemsg(mp);
	return (0);
}
static int
tp_optdata_req(struct tp *tp, queue_t *q, mblk_t *bp)
{
	struct T_optdata_req *p = (typeof(p)) bp->b_rptr;
	N_optdata_req_t *r;
	mblk_t *mp;
}
static int
tp_addr_req(struct tp *tp, queue_t *q, mblk_t *bp)
{
	struct T_addr_req *p = (typeof(p)) bp->b_rptr;
	struct T_addr_ack *r;
	mblk_t *mp = NULL;
	int err;

	if (!MBLKIN(bp, 0, sizeof(*p)))
		goto badprim;
	if (!(mp = mi_allocb(q, sizeof(*r) + xp->LOCADDR_length + xp->REMADDR_length, BPRI_MED)))
		goto enobufs;
	DB_TYPE(mp) = M_PCPROTO;
	r = (typeof(r)) mp->b_rptr;
	mp->b_wptr += sizeof(*r);
	r->PRIM_type = T_ADDR_ACK;
	r->LOCADDR_length = xp->LOCADDR_length;
	r->LOCADDR_offset = sizeof(*r);
	r->REMADDR_length = xp->REMADDR_length;
	r->REMADDR_offset = sizeof(*r) + xp->LOCADDR_length;
	bcopy(xp->LOCADDR_address, mp->b_wptr, xp->LOCADDR_length);
	mp->b_wptr += xp->LOCADDR_length;
	bcopy(xp->REMADDR_address, mp->b_wptr, xp->REMADDR_length);
	mp->b - wptr += xp->REMADDR_length;
	freemsg(bp);
	qreply(q, mp);
	return (0);
      badprim:
	err = -EPROTO;
	goto terror;
      terror:
	return tp_error_ack(xp, q, bp, T_ADDR_REQ, err);
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	freemsg(mp);
	return (err);
}
static int
tp_capability_req(struct tp *tp, queue_t *q, mblk_t *bp)
{
	struct T_capability_req *p = (typeof(p)) bp->b_rptr;
	N_info_req_t *r;
	mblk_t *mp = NULL;
	int err;

	if (!MBLKIN(bp, 0, sizeof(*p)))
		goto badprim;
	if (!(mp = mi_allocb(q, sizeof(*r), BPRI_MED)))
		goto enobufs;
	if (!pcmsg(DB_TYPE(bp)) && !bcanputnext(q, bp->b_band))
		goto ebusy;
	DB_TYPE(mp) = DB_TYPE(bp);
	mp->b_band = bp->b_band;
	r = (typeof(r)) mp->b_rptr;
	mp->b_wptr += sizeof(*r);
	r->PRIM_type = N_INFO_REQ;
	freemsg(bp);
	xp->wack_creq++;
	putnext(q, mp);
	return (0);
      badprim:
	err = EPROTO;
	goto terror;
      terror:
	return tp_error_ack(xp, q, bp, T_INFO_REQ, err);
      enobufs:
	err = -ENOBUFS;
	goto error;
      ebusy:
	err = -EBUSY;
	goto error;
      error:
	freemsg(mp);
	return (err);
}
static int
tp_other_req(struct tp *tp, queue_t *q, mblk_t *bp)
{
	return tp_error_ack(xp, q, bp, *(t_uscalar_t *) bp->b_rptr, TNOTSUPPORT);
}

/*
 * Primitives receive from downstream (NPI NS-Proivder Primitives)
 * --------------------------------------------------------------------------
 * For all these functions @xp is a pointer to the locked PLP private
 * structure, @q is a pointer to the active queue, @bp is a pointer to the
 * message being processed.
 *
 * These pretty much trust the driver state machine in that it assumes that
 * primitives are only issued by the NS-provider in the correct state.  We
 * also trust that the NS-Provider issues correctly formatted primitives.
 * Primitives are simply translated to the corresponding TPI primitives and
 * passed upstream.
 */
static int
np_conn_con(struct np *np, queue_t *q, mblk_t *bp)
{
	N_conn_con_t *p = (typeof(p)) bp->b_rptr;
	struct t_opthdr *t;
	struct T_conn_con *r;
	size_t olen;
	mblk_t *mp;

	/* Translate RES_length and RES_offset, responding address */
	/* Translate QOS_length, QOS_offset and CONN_flags into OPT_length and
	   OPT_offset */
	olen = xx25_size_options((caddr_t) p + p->QOS_offset, p->QOS_length, p->CONN_flags);
	if (!(mp = mi_allocb(q, olen + sizeof(*r) + p->RES_length, BPRI_MED)))
		goto enobufs;
	if (!bcanputnext(q, bp->b_band))
		goto ebusy;
	DB_TYPE(mp) = DB_TYPE(bp);
	mp->b_band = bp->b_band;
	r = (typeof(r)) mp->b_rptr;
	mp->b_wptr = mp->b_rptr + sizeof(*r);
	r->PRIM_type = T_CONN_CON;
	r->RES_length = p->RES_length;
	r->RES_offset = sizeof(*r);
	r->OPT_length = olen;
	r->OPT_offset = sizeof(*r) + p->RES_length;
	bcopy((caddr_t) p + p->RES_offset, mp->b_wptr, p->RES_length);
	mp->b_wptr += p->RES_length;
	t = (typeof(t)) mp->b_wptr;
	t->len = sizeof(*t) + sizeof(t_scalar_t);
	t->level = T_X25_NP;
	t->name = T_X25_D_OPT;
	t->state = T_SUCCESS;
	*(t_scalar_t *) t->value = (p->CONN_flags & REC_CONF_OPT) ? T_YES : T_NO;
	mp->b_wptr += t->len;
	t = (typeof(t)) mp->b_wptr;
	t->len = sizeof(*t) + sizeof(t_scalar_t);
	t->level = T_X25_NP;
	t->name = T_X25_EDN;
	t->state = T_SUCCESS;
	*(t_scalar_t *) t->value = (p->CONN_flags & EX_DATA_OPT) ? T_YES : T_NO;

	xx25_build_options(mp, (caddr_t) p + p->QOS_offset, p->QOS_length, p->CONN_flags);
	mp->b_cont = XCHG(&bp->b_cont, NULL);
	freeb(bp);
	np_set_state(xp, NS_DATA_XFER);
	tp_set_state(xp, TS_DATA_XFER);
	putnext(q, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}
static int
np_discon_ind(struct np *np, queue_t *q, mblk_t *mp)
{
	N_discon_ind_t *p = (typeof(p)) mp->b_rptr;

	return tp_discon_ind(xp, q, mp, p->RES_offset, p->RES_length, p->DISCON_orig,
			     p->DISCON_reason, p->SEQ_number, mp->b_cont);
}
static int
np_data_ind(struct np *np, queue_t *q, mblk_t *mp)
{
	N_data_ind_t *p = (typeof(p)) mp->b_rptr;

	return tp_data_ind(xp, q, mp, p->DATA_xfer_flags, mp->b_cont);
}
static int
np_exdata_ind(struct np *np, queue_t *q, mblk_t *mp)
{
	N_exdata_ind_t *p = (typeof(p)) mp->b_rptr;

	return tp_exdata_ind(xp, q, mp, mp->b_cont);
}

/**
 * np_info_ack: - process N_INFO_ACK primitive
 * @xp: X.25 private structure
 * @q: active queue (lower read queue)
 * @bp: the N_INFO_ACK message
 *
 * This primitive must do double duty: one each in response to T_INFO_REQ or T_CAPABILITY_REQ.  Each
 * of these translates into a N_INFO_REQ on the downward path.  We mark in the upward path whether
 * we are interested in a T_INFO_ACK or T_CAPABILITY_ACK so that the appropriate responses can be
 * given.
 */
static int
np_info_ack(struct np *np, queue_t *q, mblk_t *bp)
{
	N_info_ack_t *p = (typeof(p)) bp->b_rptr;

	if (xp->wack_ireq) {
		struct T_info_ack *r;

		if (!(mp = mi_allocb(q, sizeof(*r), BPRI_MED)))
			goto enobufs;
		if (!pcmsg(DB_TYPE(bp)) && !bcanputnext(q, bp->b_band))
			goto ebusy;
		DB_TYPE(mp) = DB_TYPE(bp);
		mp->b_band = bp->b_band;
		r = (typeof(r)) mp->b_rptr;
		mp->b_wptr += sizeof(*r);
		r->PRIM_type = T_INFO_ACK;
		r->TSDU_size = p->NSDU_size;
		r->ETSDU_size = p->ENSDU_size;
		r->CDATA_size = p->CDATA_size;
		r->DDATA_size = p->DDATA_size;
		r->ADDR_size = p->ADDR_size;
		r->OPT_size = FIXME;
		r->TIDU_size = p->NIDU_size;
		r->SERV_type = T_COTS;
		r->CURRENT_state = xx25_map_state(p->CURRENT_state);
		r->PROVIDER_flag = SNDZERO | XPG4_1;
		freemsg(bp);
		putnext(q, mp);
		xp->wack_ireq--;
	} else if (xp->wack_creq) {
		struct T_capability_ack *r;

		if (!(mp = mi_allocb(q, sizeof(*r), BPRI_MED)))
			goto enobufs;
		if (!pcmsg(DB_TYPE(bp)) && !bcanputnext(q, bp->b_band))
			goto ebusy;
		DB_TYPE(mp) = DB_TYPE(bp);
		mp->b_band = bp->b_band;
		r = (typeof(r)) mp->b_rptr;
		mp->b_wptr += sizeof(*r);
		r->PRIM_type = T_CAPABILITY_ACK;
		/* always reply with both regardless of request */
		r->CAP_bits1 = TC1_INFO | TC1_ACCEPTOR_ID;
		r->INFO_ack.PRIM_type = T_INFO_ACK;
		r->INFO_ack.TSDU_size = p->NSDU_size;
		r->INFO_ack.ETSDU_size = p->ENSDU_size;
		r->INFO_ack.CDATA_size = p->CDATA_size;
		r->INFO_ack.DDATA_size = p->DDATA_size;
		r->INFO_ack.ADDR_size = p->ADDR_size;
		r->INFO_ack.OPT_size = FIXME;
		r->INFO_ack.TIDU_size = p->NIDU_size;
		r->INFO_ack.SERV_type = T_COTS;
		r->INFO_ack.CURRENT_state = xx25_map_state(p->CURRENT_state);
		r->INFO_ack.PROVIDER_flag = SNDZERO | XPG4_1;
		/* We just happen to know this. */
		r->ACCEPTOR_id = (t_uscalar_t) (long) OTHERQ(q)->q_next;
		freemsg(bp);
		putnext(q, mp);
		xp->wack_creq--;
	} else {
		freemsg(bp);
	}
	return (0);
}
static int
np_bind_ack(struct np *np, queue_t *q, mblk_t *bp)
{
	N_bind_ack_t *p = (typeof(p)) bp->b_rptr;
	struct T_bind_ack *r;
	mblk_t *mp;

	if (!(mp = mi_allocb(q, sizeof(*r) + p->ADDR_length, BPRI_MED)))
		goto enobufs;
	if (!pcmsg(DB_TYPE(bp)) && !bcanputnext(q, bp->b_band))
		goto ebusy;
	DB_TYPE(mp) = DB_TYPE(bp);
	mp->b_band = bp->b_band;
	r = (typeof(r)) mp->b_rptr;
	mp->b_wptr += sizeof(*r);
	r->PRIM_type = T_BIND_ACK;
	r->ADDR_length = p->ADDR_length;
	r->ADDR_offset = sizeof(*r);
	r->CONIND_number = p->CONIND_number;
	bcopy(bp->b_rptr + p->ADDR_offset, mp->b_wptr, p->ADDR_length);
	mp->b_wptr += p->ADDR_length;
	np_set_state(NS_IDLE);
	tp_set_state(TS_IDLE);
	freemsg(bp);
	putnext(q, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}
static int
np_error_ack(struct np *np, queue_t *q, mblk_t *bp)
{
	N_error_ack_t *p = (typeof(p)) bp->b_rptr;
	struct T_error_ack *r;
	mblk_t *mp;

	if (!(mp = mi_allocb(q, sizeof(*r), BPRI_MED)))
		goto enobufs;
	if (!pcmsg(DB_TYPE(bp)) && !bcanputnext(q, bp->b_band))
		goto ebusy;
	DB_TYPE(mp) = DB_TYPE(bp);
	mp->b_band = bp->b_band;
	r = (typeof(r)) mp->b_rptr;
	mp->b_wptr += sizeof(*r);
	r->PRIM_type = T_ERROR_ACK;
	switch (p->ERROR_prim) {
	case N_BIND_REQ:
		r->ERROR_prim = T_BIND_REQ;
		break;
	case N_UNBIND_REQ:
		r->ERROR_prim = T_UNBIND_REQ;
		break;
	case N_CONN_REQ:
		r->ERROR_prim = T_CONN_REQ;
		break;
	case N_CONN_RES:
		r->ERROR_prim = T_CONN_RES;
		break;
	case N_DATA_REQ:
		r->ERROR_prim = T_DATA_REQ;
		break;
	case N_EXDATA_REQ:
		r->ERROR_prim = T_EXDATA_REQ;
		break;
	case N_DISCON_REQ:
		r->ERROR_prim = T_DISCON_REQ;
		break;
	case N_RESET_REQ:
		r->ERROR_prim = T_DATA_REQ;
		break;
	case N_RESET_RES:
		r->ERROR_prim = T_DATA_REQ;
		break;
	case N_INFO_REQ:
		if (xp->wack_ireq) {
			r->ERROR_prim = T_INFO_REQ;
			xp->wack_ireq--;
		} else if (xp->wack_creq) {
			r->ERROR_prim = T_CAPABILITY_REQ;
			xp->wack_creq--;
		} else {
			r->ERROR_prim = T_INFO_REQ;
		}
		break;
	case N_UNITDATA_REQ:
		r->ERROR_prim = T_UNITDATA_REQ;
		break;
	case N_OPTMGMT_REQ:
		r->ERROR_prim = T_OPTMGMT_REQ;
		break;
	case N_DATACK_REQ:
		r->ERROR_prim = T_OPTDATA_REQ;
		break;
	default:
		r->ERROR_prim = p->ERROR_prim;
		break;
	}
	r->UNIX_error = p->UNIX_error;
	switch (p->NPI_error) {
#if 0
	case NBADADDR:		/* 1 */
		r->TLI_error = TBADADDR;	/* 1 */
		break;
	case NBADOPT:		/* 2 */
		r->TLI_error = TBADOPT;	/* 2 */
		break;
	case NACCESS:		/* 3 */
		r->TLI_error = TACCES;	/* 3 */
		break;
	case NNOADDR:		/* 5 */
		r->TLI_error = TNOADDR;	/* 5 */
		break;
	case NOUTSTATE:	/* 6 */
		r->TLI_error = TOUTSTATE;	/* 6 */
		break;
	case NBADSEQ:		/* 7 */
		r->TLI_error = TBADSEQ;	/* 7 */
		break;
	case NSYSERR:		/* 8 */
		r->TLI_error = TSYSERR;	/* 8 */
		break;
	case NBADDATA:		/* 10 */
		r->TLI_error = TBADDATA;	/* 10 */
		break;
	case NBADFLAG:		/* 16 */
		r->TLI_error = TBADFLAG;	/* 16 */
		break;
	case NNOTSUPPORT:	/* 18 */
		r->TLI_error = TNOTSUPPORT;	/* 18 */
		break;
#endif
	case NBOUND:		/* 19 */
		r->TLI_error = TADDRBUSY;	/* 23 */
		break;
	case NBADQOSPARAM:	/* 20 */
		r->TLI_error = TBADOPT;	/* 2 */
		break;
	case NBADQOSTYPE:	/* 21 */
		r->TLI_error = TBADOPT;	/* 2 */
		break;
	case NBADTOKEN:	/* 22 */
		r->TLI_error = TPROVMISMATCH;	/* 25 */
		break;
	case NNOPROTOID:	/* 23 */
		r->TLI_error = TBADADDR;	/* 1 */
		break;
	default:
		r->TLI_error = p->NPI_error;
		break;
	}
	np_set_state(xp, xp->chkstate);
	tp_set_state(xp, xp->chkstate);
	freemsg(bp);
	putnext(q, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);

}
static int
np_ok_ack(struct np *np, queue_t *q, mblk_t *bp)
{
	N_info_req_t *p = (typeof(p)) bp->b_rptr;
	struct T_info_req *r;
	mblk_t *mp;

	if (!(mp = mi_allocb(q, sizeof(*r), BPRI_MED)))
		goto enobufs;
	if (!pcmsg(DB_TYPE(bp)) && !bcanputnext(q, bp->b_band))
		goto ebusy;
	DB_TYPE(mp) = DB_TYPE(bp);
	mp->b_band = bp->b_band;
	r = (typeof(r)) mp->b_rptr;
	mp->b_rptr += sizeof(*r);
	r->PRIM_type = T_OK_ACK;
	switch (p->CORRECT_prim) {
	case N_UNBIND_REQ:
		r->CORRECT_prim = T_UNBIND_REQ;
		np_set_state(xp, NS_UNBND);
		tp_set_state(xp, TS_UNBND);
		break;
	case N_CONN_RES:
		r->CORRECT_prim = T_CONN_RES;
		np_set_state(xp, NS_DATA_XFER);
		tp_set_state(xp, TS_DATA_XFER);
		break;
	case N_DISCON_REQ:
		r->CORRECT_prim = T_DISCON_REQ;
		np_set_state(xp, NS_IDLE);
		tp_set_state(xp, TS_IDLE);
		break;
	case N_RESET_RES:
		r->CORRECT_prim = T_DATA_REQ;
		np_set_state(xp, NS_DATA_XFER);
		tp_set_state(xp, TS_DATA_XFER);
		/* Note we do not propagate acks for reset responses. */
		freemsg(bp);
		freeb(mp);
		return (0);
	case N_OPTMGMT_REQ:
		r->CORRECT_prim = T_OPTMGMT_REQ;
		if (np_get_state(xp, NS_WACK_OPTREQ))
			np_set_state(xp, NS_IDLE);
		if (tp_get_state(xp, TS_WACK_OPTREQ))
			tp_set_state(xp, NS_IDLE);
		break;
	default:
		r->CORRECT_prim = p->CORRECT_prim;
		break;
	}
	freemsg(bp);
	putnext(q, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}
static int
np_unitdata_ind(struct np *np, queue_t *q, mblk_t *bp)
{
	N_unitdata_ind_t *p = (typeof(p)) bp->b_rptr;
	struct T_unitdata_ind *r;
	mblk_t *mp;

	if (!(mp = mi_allocb(q, sizeof(*r) + p->SRC_length, BPRI_MED)))
		goto enobufs;
	if (!pcmsg(DB_TYPE(bp)) && !bcanputnext(q, bp->b_band))
		goto ebusy;
	DB_TYPE(mp) = DB_TYPE(bp);
	mp->b_band = bp->b_band;
	r = (typeof(r)) mp->b_rptr;
	mp->b_wptr += sizeof(*r);
	r->PRIM_type = T_UNITDATA_IND;
	r->SRC_length = p->SRC_length;
	r->SRC_offset = sizeof(*r);
	r->OPT_length = 0;
	r->OPT_offset = 0;
	bcopy(bp->b_rptr + p->SRC_offset, mp->b_wptr, p->SRC_length);
	mp->b_wptr += p->SRC_length;
	mp->b_cont = XCHG(&bp->b_cont, NULL);
	freeb(bp);
	putnext(q, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}
static int
np_uderror_ind(struct np *np, queue_t *q, mblk_t *bp)
{
	N_uderror_ind_t *p = (typeof(p)) bp->b_rptr;
	struct T_uderror_ind *r;
	mblk_t *mp;

	if (!(mp = mi_allocb(q, sizeof(*r) + p->DEST_length, BPRI_MED)))
		goto enobufs;
	if (!pcmsg(DB_TYPE(bp)) && !bcanputnext(q, bp->b_band))
		goto ebusy;
	DB_TYPE(mp) = DB_TYPE(bp);
	mp->b_band = bp->b_band;
	r = (typeof(r)) mp->b_rptr;
	mp->b_wptr += sizeof(*r);
	r->PRIM_type = T_UDERROR_IND;
	r->DEST_length = p->DEST_length;
	r->DEST_offset = sizeof(*r);
	r->OPT_length = 0;
	r->OPT_offset = 0;
	r->ERROR_type = p->ERROR_type;
	bcopy(bp->b_rptr + p->DEST_offset, mp->b_wptr, p->DEST_length);
	mp->b_wptr += p->DEST_length;
	mp->b_cont = XCHG(&bp->b_cont, NULL);
	freeb(bp);
	putnext(q, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * np_datack_ind: - process N_DATACK_IND primitive
 * @xp: X.25 private structure
 * @q: active queue (lower read queue)
 * @bp: the N_DATACK_IND primitive
 *
 * XX25 specifies that data acknolwedgements are translated into zero length data indications that
 * have the special T_X25_DACK or T_X25_EACK flags bits set.
 */
static int
np_datack_ind(struct np *np, queue_t *q, mblk_t *bp)
{
	struct T_data_ind *r;
	mblk_t *mp;

	if (!(mp = mi_allocb(q, sizeof(*r), BPRI_MED)))
		goto enobufs;
	if (!pcmsg(DB_TYPE(bp)) && !bcanputnext(q, bp->b_band))
		goto ebusy;
	DB_TYPE(mp) = DB_TYPE(bp);
	mp->b_band = bp->b_band;
	r = (typeof(r)) mp->b_rptr;
	mp->b_wptr += sizeof(*r);
	r->PRIM_type = T_DATA_IND;
	r->MORE_flag = T_X25_DACK;
	freemsg(bp);
	putnext(q, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

static int
np_other_ind(struct np *np, queue_t *q, mblk_t *bp)
{
	freemsg(bp);
	return (0);
}

/*
 *  M_IOCTL, M_IOCDATA Handling
 *  -------------------------------------------------------------------------
 *  Note that the XX25 module does not implement any input-output controls of its own and they are
 *  merely passed to the driver.
 */
static int
xx25_w_ioctl(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/*
 *  M_COPYIN, M_COPYOUT Handling
 *  -------------------------------------------------------------------------
 *  Note that the XX25 module does not implement any input-output controls of its own and they are
 *  merely passed to the driver.
 */
static int
xx25_r_copy(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/*
 *  M_CTL, M_PCCTL Handling
 *  -------------------------------------------------------------------------
 *  The module uses M_CTL and M_PCCTL like input-output controls to control the driver.
 */
static int
xx25_r_cmd(struct np *np, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;

	if (!MBLKIN(mp, 0, sizeof(*ioc)))
		return (-EINVAL);

	switch (_IOC_NR(ioc->ioc_cmd)) {
	case XX25_IOCGOPTION:
		return xx25_get_option(xp, q, mp);
	case XX25_IOCSOPTION:
		return xx25_set_option(xp, q, mp);
	case XX25_IOCGCONFIG:
		return xx25_get_config(xp, q, mp);
	case XX25_IOCSCONFIG:
		return xx25_set_config(xp, q, mp);
	case XX25_IOCTCONFIG:
		return xx25_test_config(xp, q, mp);
	case XX25_IOCCCONFIG:
		return xx25_confirm_config(xp, q, mp);
	case XX25_IOCLCONFIG:
		return xx25_list_config(xp, q, mp);
	case XX25_IOCGSTATEM:
		return xx25_get_statem(xp, q, mp);
	case XX25_IOCCMRESET:
		return xx25_master_reset(xp, q, mp);
	case XX25_IOCGSTATSP:
		return xx25_get_statsp(xp, q, mp);
	case XX25_IOCSSTATSP:
		return xx25_set_statsp(xp, q, mp);
	case XX25_IOCGSTATS:
		return xx25_get_stats(xp, q, mp);
	case XX25_IOCCSTATS:
		return xx25_set_stats(xp, q, mp);
	case XX25_IOCGNOTIFY:
		return xx25_get_notify(xp, q, mp);
	case XX25_IOCSNOTIFY:
		return xx25_set_notify(xp, q, mp);
	case XX25_IOCCNOTIFY:
		return xx25_clear_notify(xp, q, mp);
	case XX25_IOCCMGMT:
		return xx25_mgmt(xp, q, mp);
	case XX25_IOCCPASS:
		return xx25_pass(xp, q, mp);
	default:
		freemsg(mp);
		return (0);
	}
}

static int
__xx25_r_ctl(struct np *np, queue_t *q, mblk_t *mp)
{
	int err;

	err = xx25_r_cmd(xp, q, mp);
	return (err < 0 ? err : 0);
}

static int
xx25_r_ctl(queue_t *q, mblk_t *mp)
{
	struct np *np;
	int err = -EAGAIN;

	if ((xp = xp_acquire(q))) {
		err = __xx25_r_ctl(xp, q, mp);
		xp_release(xp);
	}
	return (err);
}

/*
 *  M_PROTO, M_PCPROTO Handling
 *  -------------------------------------------------------------------------
 */
static int
xx25_w_prim(struct np *np, queue_t *q, mblk_t *mp)
{
	t_uscalar_t prim;

	if (!MBLKIN(mp, 0, sizeof(prim)))
		return (-EINVAL);
	switch ((prim = *(t_uscalar_t *) mp->b_rptr)) {
	case T_DATA_REQ:
	case T_EXDATA_REQ:
	case T_OPTDATA_REQ:
		mi_strlog(q, STRLOGDA, SL_TRACE, "%s [%d] ->", tp_primname(prim),
			  (int) msgdsize(mp->b_cont));
		break;
	default:
		mi_strlog(q, STRLOGRX, SL_TRACE, "%s ->", tp_primname(prim));
		break;
	}
	switch (prim) {
	case T_CONN_REQ:
		return tp_conn_req(xp, q, mp);
	case T_CONN_RES:
		return tp_conn_res(xp, q, mp);
	case T_DISCON_REQ:
		return tp_discon_req(xp, q, mp);
	case T_DATA_REQ:
		return tp_data_req(xp, q, mp);
	case T_EXDATA_REQ:
		return tp_exdata_req(xp, q, mp);
	case T_INFO_REQ:
		return tp_info_req(xp, q, mp);
	case T_BIND_REQ:
		return tp_bind_req(xp, q, mp);
	case T_UNBIND_REQ:
		return tp_unbind_req(xp, q, mp);
	case T_UNITDATA_REQ:
		return tp_unitdata_req(xp, q, mp);
	case T_OPTMGMT_REQ:
		return tp_optmgmt_req(xp, q, mp);
	case T_ORDREL_REQ:
		return tp_ordrel_req(xp, q, mp);
	case T_OPTDATA_REQ:
		return tp_optdata_req(xp, q, mp);
	case T_ADDR_REQ:
		return tp_addr_req(xp, q, mp);
	case T_CAPABILITY_REQ:
		return tp_capability_req(xp, q, mp);
	default:
		return tp_other_req(xp, q, mp);
	}
}
static int
__xx25_w_proto(struct np *np, queue_t *q, mblk_t *mp)
{
	uint oldstate = xp_get_state(xp);
	int err = 0;

	if ((err = xx25_w_prim(xp, q, mp)))
		xp_set_state(xp, oldstate);
	return (err < 0 ? err : 0);
}
static int
xx25_w_proto(queue_t *q, mblk_t *mp)
{
	struct np *np;
	int err = -EAGAIN;

	if ((xp = xp_acquire(q))) {
		err = __xx25_w_proto(xp, q mp);
		xp_release(xp);
	}
	return (err);
}
static int
xx25_r_prim(struct np *np, queue_t *q, mblk_t *mp)
{
	np_ulong prim;

	if (!MBLKIN(mp, 0, sizeof(prim)))
		return (-EINVAL);
	switch ((prim = *(np_ulong *) mp->b_rptr)) {
	case N_DATA_REQ:
	case N_EXDATA_REQ:
		mi_strlog(q, STRLOGDA, SL_TRACE, "%s [%d] <-", np_primname(prim),
			  (int) msgdsize(mp->b_cont));
		break;
	default:
		mi_strlog(q, STRLOGRX, SL_TRACE, "%s <-", np_primname(prim));
		break;
	}
	switch (prim) {
	case N_CONN_IND:
		return np_conn_ind(np, q, mp);
	case N_CONN_CON:
		return np_conn_con(np, q, mp);
	case N_DISCON_IND:
		return np_discon_ind(np, q, mp);
	case N_DATA_IND:
		return np_data_ind(np, q, mp);
	case N_EXDATA_IND:
		return np_exdata_ind(np, q, mp);
	case N_INFO_ACK:
		return np_info_ack(np, q, mp);
	case N_BIND_ACK:
		return np_bind_ack(np, q, mp);
	case N_ERROR_ACK:
		return np_error_ack(np, q, mp);
	case N_OK_ACK:
		return np_ok_ack(np, q, mp);
	case N_UNITDATA_IND:
		return np_unitdata_ind(np, q, mp);
	case N_UDERROR_IND:
		return np_uderror_ind(np, q, mp);
	case N_DATACK_IND:
		return np_datack_ind(np, q, mp);
	case N_RESET_IND:
		return np_reset_ind(np, q, mp);
	case N_RESET_CON:
		return np_reset_con(np, q, mp);
	default:
		return np_other_ind(np, q, mp);
	}
}
static int
__xx25_r_proto(struct np *np, queue_t *q, mblk_t *mp)
{
	uint oldstate = xp_get_state(xp);
	int err = 0;

	if ((err = xx25_r_prim(xp, q, mp)))
		xp_set_state(xp, oldstate);
	return (err < 0 ? err : 0);
}
static int
xx25_r_proto(queue_t *q, mblk_t *mp)
{
	struct np *np;
	int err = -EAGAIN;

	if ((xp = xp_acquire(q))) {
		err = __xx25_r_proto(xp, q, mp);
		xp_release(xp);
	}
	return (err);
}

/*
 * M_DATA, M_HPDATA Handling
 * --------------------------------------------------------------------------
 */
static int
__xx25_w_data(struct np *np, queue_t *q, mblk_t *mp)
{
	return tp_data(xp, q, mp);
}
static int
xx25_w_data(queue_t *q, mblk_t *mp)
{
	struct np *np;
	int err = -EAGAIN;

	if ((xp = xp_acquire(q))) {
		err = __xx25_w_data(xp, q, mp);
		xp_release(xp);
	}
	return (err);
}

static int
__xx25_r_data(struct np *np, queue_t *q, mblk_t *mp)
{
	return np_data(xp, q, mp);
}
static int
xx25_r_data(queue_t *q, mblk_t *mp)
{
	struct np *np;
	int err = -EAGAIN;

	if ((xp = xp_acquire(q))) {
		err = __xx25_w_data(xp, q, mp);
		xp_release(xp);
	}
	return (err);
}

/*
 * M_SIG, M_PCSIG handling
 * --------------------------------------------------------------------------
 */
static int
xx25_r_sig(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/*
 * M_RSE, M_PCRSE handling
 * --------------------------------------------------------------------------
 */
static int
xx25_r_rse(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/*
 * M_ERROR handling
 * --------------------------------------------------------------------------
 */
static int
xx25_r_error(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		/* FIXME: must record fact that there is an error */
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/*
 * M_HANGUP handling
 * --------------------------------------------------------------------------
 */
static int
xx25_r_hangup(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		/* FIXME: must record fact that there is a hangup */
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/*
 * M_FLUSH handling
 * --------------------------------------------------------------------------
 */
static int
xx25_w_flush(queue_t *q, mblk_t *mp)
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
static int
xx25_r_flush(queue_t *q, mblk_t *mp)
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

/*
 *  Other STREAMS Message Handling
 *  -------------------------------------------------------------------------
 */
static int
xx25_w_other(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

static int
xx25_r_other(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/*
 *  STREAMS Message Handling
 *  -------------------------------------------------------------------------
 */

static int
xx25_w_msg(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		return xx25_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return xx25_w_proto(q, mp);
	case M_IOCTL:
	case M_IOCDATA:
		return xx25_w_ioctl(q, mp);
	case M_FLUSH:
		return xx25_w_flush(q, mp);
	default:
		return xx25_w_other(q, mp);
	}
}
static int
__xx25_w_msg(struct np *np, queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		return __xx25_w_data(xp, q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return __xx25_w_proto(xp, q, mp);
	case M_IOCTL:
	case M_IOCDATA:
		return xx25_w_ioctl(q, mp);
	case M_FLUSH:
		return xx25_w_flush(q, mp);
	default:
		return xx25_w_other(q, mp);
	}
}
static int
__xx25_r_msg(struct np *np, queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		return xx25_r_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return xx25_r_proto(q, mp);
	case M_COPYIN:
	case M_COPYOUT:
		return xx25_r_copy(q, mp);
	case M_SIG:
	case M_PCSIG:
		return xx25_r_sig(q, mp);
	case M_RSE:
	case M_PCRSE:
		return xx25_r_rse(q, mp);
	case M_CTL:
	case M_PCCTL:
		return xx25_r_ctl(q, mp);
	case M_ERROR:
		return xx25_r_error(q, mp);
	case M_HANGUP:
		return xx25_r_hangup(q, mp);
	case M_FLUSH:
		return xx25_r_flush(q, mp);
	default:
		return xx25_r_other(q, mp);
	}
}
static int
xx25_r_msg(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		return __xx25_r_data(xp, q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return __xx25_r_proto(xp, q, mp);
	case M_COPYIN:
	case M_COPYOUT:
		return xx25_r_copy(q, mp);
	case M_SIG:
	case M_PCSIG:
		return xx25_r_sig(q, mp);
	case M_RSE:
	case M_PCRSE:
		return xx25_r_rse(q, mp);
	case M_CTL:
	case M_PCCTL:
		return __xx25_r_ctl(xp, q, mp);
	case M_ERROR:
		return xx25_r_error(q, mp);
	case M_HANGUP:
		return xx25_r_hangup(q, mp);
	case M_FLUSH:
		return xx25_r_flush(q, mp);
	default:
		return xx25_r_other(q, mp);
	}
}

/*
 *  PUT AND SERVICE PROCEDURES
 *  -------------------------------------------------------------------------
 */
static streamscall int
xx25_wput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flags & QSVCBUSY))) || xx25_w_msg(q, mp))
		putq(q, mp);
	return (0);
}
static streamscall int
xx25_wsrv(queue_t *q)
{
	struct np *np;

	if ((xp = xp_acquire(q))) {
		mblk_t *mp;

		while ((mp = getq(q))) {
			if (__xx25_w_msg(xp, q, mp)) {
				putbq(q, mp);
				break;
			}
		}
		xp_release(q);
	}
	return (0);
}
static streamscall int
xx25_rsrv(queue_t *q)
{
	struct np *np;

	if ((xp = xp_acquire(q))) {
		mblk_t *mp;

		while ((mp = getq(q))) {
			if (__xx25_r_msg(xp, q, mp)) {
				putbq(q, mp);
				break;
			}
		}
	}
	return (0);
}
static streamscall int
xx25_rput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flags & QSVCBUSY))) || xx25_r_msg(q, mp))
		putq(q, mp);
	return (0);
}

/*
 *  OPEN AND CLOSE ROUTINES
 *  -------------------------------------------------------------------------
 */
static int
xx25_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct priv *p;
	struct tp *tp;
	struct np *np;
	mblk_t *mp;
	int err;

	if (q->q_ptr != NULL)
		return (0);	/* already open */
	if (!(mp = allocb(sizeof(N_info_req_t), BPRI_WAITOK)))
		return (ENOBUFS);
	if ((err = mi_open_comm(&xx25_opens, sizeof(*p), q, devp, oflags, sflag, crp))) {
		freeb(mp);
		return (err);
	}
	p = PRIV(q);
	tp = &p->r_priv;
	np = &p->w_priv;

	/* intitialize private structure */
	tp->priv = p;
	tp->np = np;
	tp->oq = q;
	tp->state = TS_UNBND;
	tp->oldstate = TS_UNBND;

	/* Defaults are maximum available for X.25-1996: Note that if no
	   connection is established, these values reflect the maximum
	   characteristics an X.25 connection could take on using the
	   underlying X.25 provider. */
	tp->proto.caps.INFO_ack.PRIM_type = T_INFO_ACK;
	tp->proto.caps.INFO_ack.TSDU_size = T_INFINITE;
	tp->proto.caps.INFO_ack.ETSDU_size = 32;
	tp->proto.caps.INFO_ack.CDATA_size = 128;
	tp->proto.caps.INFO_ack.DDATA_size = 128;
	tp->proto.caps.INFO_ack.ADDR_size = 40;
	tp->proto.caps.INFO_ack.OPT_size = T_INFINITE;
	tp->proto.caps.INFO_ack.TIDU_size = STRMAXPSZ;
	tp->proto.caps.INFO_ack.SERV_type = T_COTS;
	tp->proto.caps.INFO_ack.CURRENT_state = TS_UNBND;
	tp->proto.caps.INFO_ack.PROVIDER_flag = XPG4_1 | T_SNDZERO;

	tp->add[0] = 35;	/* AFI 35 */
	tp->add[1] = 0x00;	/* ICP 1 */
	tp->add[2] = 0x01;
	tp->add[3] = 0x00;	/* IPv4 address */
	tp->add[4] = 0x00;
	tp->add[5] = 0x00;
	tp->add[6] = 0x00;
	tp->add[7] = 0x00;	/* IPv4 port number */
	tp->add[8] = 0x00;

	tp->loc[0] = 35;	/* AFI 35 */
	tp->loc[1] = 0x00;	/* ICP 1 */
	tp->loc[2] = 0x01;
	tp->loc[3] = 0x00;	/* IPv4 address */
	tp->loc[4] = 0x00;
	tp->loc[5] = 0x00;
	tp->loc[6] = 0x00;
	tp->loc[7] = 0x00;	/* IPv4 port number */
	tp->loc[8] = 0x00;

	tp->rem[0] = 35;	/* AFI 35 */
	tp->rem[1] = 0x00;	/* ICP 1 */
	tp->rem[2] = 0x01;
	tp->rem[3] = 0x00;	/* IPv4 address */
	tp->rem[4] = 0x00;
	tp->rem[5] = 0x00;
	tp->rem[6] = 0x00;
	tp->rem[7] = 0x00;	/* IPv4 port number */
	tp->rem[8] = 0x00;

	np->priv = p;
	np->tp = tp;
	np->oq = WR(q);
	np->state = NS_UNBND;
	np->oldstate = NS_UNBND;

	np->proto.info.PRIM_type = N_INFO_ACK;
	np->proto.info.NSDU_size = 128;
	np->proto.info.CDATA_size = N_UNDEFINED;
	np->proto.info.CDATA_size = N_UNDEFINED;
	np->proto.info.ADDR_size = 40;
	np->proto.info.ADDR_length = sizeof(np->proto.add);
	np->proto.info.ADDR_offset = sizeof(np->proto.info);
	np->proto.info.QOS_length = sizeof(np->proto.qos);
	np->proto.info.QOS_offset = np->proto.info.ADDR_offset + sizeof(np->proto.add);
	np->proto.info.QOS_range_length = sizeof(np->proto.qor);
	np->proto.info.QOS_range_offset = np->proto.info.QOS_offset + sizeof(np->proto.qos);
	np->proto.info.OPTIONS_flags = 0;
	np->proto.info.NIDU_size = 128;
	np->proto.info.SERV_type = N_CONS;
	np->proto.info.CURRENT_state = NS_UNBND;
	np->proto.info.PROVIDER_type = N_SUBNET;
	np->proto.info.NODU_size = 128;
	np->proto.info.PROTOID_length = sizeof(np->proto.pro);
	np->proto.info.PROTOID_offset = np->proto.info.QOS_range_offset + sizeof(np->proto.qor);
	np->proto.info.NPI_version = N_CURRENT_VERSION;

	np->proto.add[0] = 35;	/* AFI 35 */
	np->proto.add[1] = 0x00;	/* ICP 1 */
	np->proto.add[2] = 0x01;
	np->proto.add[3] = 0x00;	/* IPv4 address */
	np->proto.add[4] = 0x00;
	np->proto.add[5] = 0x00;
	np->proto.add[6] = 0x00;
	np->proto.add[7] = 0x00;	/* IPv4 port number */
	np->proto.add[8] = 0x00;

	np->proto.qos.n_qos_type = N_QOS_CO_OPT_SEL;
	np->proto.qos.src_throughput.thru_targ_value = QOS_UNKNOWN;
	np->proto.qos.src_throughput.thru_min_value = QOS_UNKNOWN;
	np->proto.qos.dest_throughput.thru_targ_value = QOS_UNKNOWN;
	np->proto.qos.dest_throughput.thru_min_value = QOS_UNKNOWN;
	np->proto.qos.transit_delay_t.td_targ_value = QOS_UNKNOWN;
	np->proto.qos.transit_delay_t.td_max_value = QOS_UNKNOWN;
	np->proto.qos.nc_estab_delay = QOS_UNKNOWN;
	np->proto.qos.nc_estab_fail_prob = QOS_UNKNOWN;
	np->proto.qos.residual_error_rate = QOS_UNKNOWN;
	np->proto.qos.xfer_fail_prob = QOS_UNKNOWN;
	np->proto.qos.nc_resilience = QOS_UNKNOWN;
	np->proto.qos.nc_rel_delay = QOS_UNKNOWN;
	np->proto.qos.nc_rel_fail_prob = QOS_UNKNOWN;
	np->proto.qos.protection_sel = QOS_UNKNOWN;
	np->proto.qos.priority_sel = QOS_UNKNOWN;
	np->proto.qos.max_accept_cost = QOS_UNKNOWN;

	np->proto.qor.n_qos_type = N_QOS_CO_OPT_RANGE;
	np->proto.qor.src_throughput.thru_targ_value = QOS_UNKNOWN;
	np->proto.qor.src_throughput.thru_min_value = QOS_UNKNOWN;
	np->proto.qor.dest_throughput.thru_targ_value = QOS_UNKNOWN;
	np->proto.qor.dest_throughput.thru_min_value = QOS_UNKNOWN;
	np->proto.qor.transit_delay_t.td_targ_value = QOS_UNKNOWN;
	np->proto.qor.transit_delay_t.td_max_value = QOS_UNKNOWN;
	np->proto.qor.nc_estab_delay = QOS_UNKNOWN;
	np->proto.qor.nc_estab_fail_prob = QOS_UNKNOWN;
	np->proto.qor.residual_error_rate = QOS_UNKNOWN;
	np->proto.qor.xfer_fail_prob = QOS_UNKNOWN;
	np->proto.qor.nc_resilience = QOS_UNKNOWN;
	np->proto.qor.nc_rel_delay = QOS_UNKNOWN;
	np->proto.qor.nc_rel_fail_prob = QOS_UNKNOWN;
	np->proto.qor.protection_range.protect_targ_value = QOS_UNKNOWN;
	np->proto.qor.protection_range.protect_min_value = QOS_UNKNOWN;
	np->proto.qor.priority_range.priority_targ_value = QOS_UNKNOWN;
	np->proto.qor.priority_range.priority_min_value = QOS_UNKNOWN;
	np->proto.qor.max_accept_cost = QOS_UNKNOWN;

	/* issue an immediate information request */
	DB_TYPE(mp) = M_PCPROTO;
	((N_info_req_t *) mp->b_wptr)->PRIM_type = N_INFO_REQ;
	mp->b_wptr += sizeof(N_info_req_t);

	qprocson(q);
	putnext(q, mp);
	return (0);
}

static int
xx25_qclose(queue_t *q, int oflags, cred_t *crp)
{
	struct priv *p = PRIV(q);

	if (p == NULL)
		return (0);
	qprocsoff(q);
	/* FIXME: free timers and other things */
	mi_close_comm(&xx25_opens, q);
	return (0);
}

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS INITIALIZATION
 *
 * --------------------------------------------------------------------------
 */

static struct qinit xx25_rinit = {
	.qi_putp = xx25_rput,
	.qi_srvp = xx25_rsrv,
	.qi_qopen = xx25_qopen,
	.qi_qclose = xx25_qclose,
	.qi_minfo = &xx25_minfo,
	.qi_mstat = &xx25_rstat,
};

static struct qinit xx25_rinit = {
	.qi_putp = xx25_wput,
	.qi_srvp = xx25_wsrv,
	.qi_minfo = &xx25_minfo,
	.qi_mstat = &xx25_wstat,
};

struct streamtab xx25_info = {
	.st_rdinit = &xx25_rinit,
	.st_wrinit = &xx25_winit,
};

static modID_t modid = MOD_ID;

/*
 * --------------------------------------------------------------------------
 *
 * LINUX KERNEL MODULE INITIALIZATION
 *
 * --------------------------------------------------------------------------
 */

#ifdef LINUX

#ifndef module_param
MODULE_PARM(modid, "h");
#else				/* module_param */
module_param(modid, ushort, 0444);
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for XX25. (0 for allocation.)");

/*
 *  LINUX FAST-STREAMS REGISTRATION
 */

#ifdef LIS
#define fmodsw _fmodsw
#endif				/* LIS */

static __init int
xx25_modinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);
	if ((err = register_strmod(&xx25_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module %d", MOD_NAME, (int) modid);
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

static __exit void
xx25_modexit(void)
{
	int err;

	if ((err = unregister_strmod(&xx25_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister module, err = %d", MOD_NAME, err);
		return;
	}
	return;
}

module_init(xx25_modinit);
module_exit(xx25_modexit);

#endif				/* LINUX */
