/*****************************************************************************

 @(#) $RCSfile: tcap.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2004/08/30 21:52:35 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2003  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 Ave, Cambridge, MA 02139, USA.

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
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2004/08/30 21:52:35 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: tcap.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2004/08/30 21:52:35 $"

static char const ident[] =
    "$RCSfile: tcap.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2004/08/30 21:52:35 $ Copyright (c) 1997-2003 OpenSS7 Corporation.";

/*
 *  This is a TCAP (Transaction Capabilities Application Part) multiplexing
 *  driver whcih can have SCCP (Signalling Connection Control Part) or any
 *  other NPI conforming (e.g., X.25 NSP) stream I_LINK'ed or I_PLINK'ed
 *  underneath it to form a complete Transaction Capabilities Application Part
 *  protocol layer for SS7.
 */

#include "compat.h"

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/mtpi.h>
#include <ss7/mtpi_ioctl.h>
#include <ss7/sccpi.h>
#include <ss7/sccpi_ioctl.h>
#include <ss7/tcap.h>
#include <ss7/tcap_ioctl.h>

#include <sys/npi.h>
#include <sys/npi_ss7.h>
#include <sys/npi_mtp.h>
#include <sys/npi_sccp.h>
//#include <sys/tpi.h>
//#include <sys/tpi_ss7.h>
//#include <sys/tpi_mtp.h>
//#include <sys/tpi_sccp.h>
//#include <sys/tpi_tr.h>
//#include <sys/tpi_tc.h>
#include <sys/tihdr.h>
#include <sys/xti.h>
#include <sys/xti_mtp.h>
#include <sys/xti_sccp.h>
#include <sys/xti_tcap.h>
//#include <sys/xti_tr.h>
//#include <sys/xti_tc.h>

#define TCAP_DESCRIP	"SS7 TRANSACTION CAPABILITIES APPLICATION PART (TCAP) STREAMS MULTIPLEXING DRIVER."
#define TCAP_EXTRA	"Part of the OpenSS7 Stack for Linux Fast-STREAMS"
#define TCAP_REVISION	"OpenSS7 $RCSfile: tcap.c,v $ $Name:  $ ($Revision: 0.9.2.4 $) $Date: 2004/08/30 21:52:35 $"
#define TCAP_COPYRIGHT	"Copyright (c) 1997-2004 OpenSS7 Corporation.  All Rights Reserved."
#define TCAP_DEVICE	"Supports OpenSS7 SCCP NPI Interface Pseudo-Device Drivers."
#define TCAP_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define TCAP_LICENSE	"GPL"
#define TCAP_BANNER	TCAP_DESCRIP	"\n" \
			TCAP_EXTRA	"\n" \
			TCAP_REVISION	"\n" \
			TCAP_COPYRIGHT	"\n" \
			TCAP_DEVICE	"\n" \
			TCAP_CONTACT
#define TCAP_SPLASH	TCAP_DESCRIP	"\n" \
			TCAP_REVISION

#ifdef LINUX
MODULE_AUTHOR(TCAP_CONTACT);
MODULE_DESCRIPTION(TCAP_DESCRIP);
MODULE_SUPPORTED_DEVICE(TCAP_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(TCAP_LICENSE);
#endif				/* MODULE_LICENSE */
#endif				/* LINUX */

#ifndef FIXME
#define FIXME -1UL
#endif

#ifdef LFS
#define TCAP_DRV_ID		CONFIG_STREAMS_TCAP_MODID
#define TCAP_DRV_NAME		CONFIG_STREAMS_TCAP_NAME
#define TCAP_CMAJORS		CONFIG_STREAMS_TCAP_NMAJORS
#define TCAP_CMAJOR_0		CONFIG_STREAMS_TCAP_MAJOR
#define TCAP_UNITS		CONFIG_STREAMS_TCAP_NMINORS
#endif

#ifndef TCAP_CMINOR_TRI
#define TCAP_CMINOR_TRI		1
#define TCAP_CMINOR_TCI		2
#define TCAP_CMINOR_TPI		3
#define TCAP_CMINOR_MGMT	4
#endif

#define TCAP_STYLE_TRI	TCAP_CMINOR_TRI
#define TCAP_STYLE_TCI	TCAP_CMINOR_TCI
#define TCAP_STYLE_TPI	TCAP_CMINOR_TPI
#define TCAP_STYLE_MGMT	TCAP_CMINOR_MGMT

/* 
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

#define DRV_ID		TCAP_DRV_ID
#define DRV_NAME	TCAP_DRV_NAME
#define CMAJORS		TCAP_CMAJORS
#define CMAJOR_0	TCAP_CMAJOR_0
#define UNITS		TCAP_UNITS
#ifdef MODULE
#define DRV_BANNER	TCAP_BANNER
#else				/* MODULE */
#define DRV_BANNER	TCAP_SPLASH
#endif				/* MODULE */

STATIC struct module_info tcap_minfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME,		/* Module ID name */
	.mi_minpsz = 1,			/* Min packet size accepted */
	.mi_maxpsz = 272 + 1,		/* Max packet size accepted */
	.mi_hiwat = 1 << 15,		/* Hi water mark */
	.mi_lowat = 1 << 10,		/* Lo water mark */
};

STATIC struct module_stat tcap_mstat = { 0, };

STATIC int tcap_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC int tcap_close(queue_t *, int, cred_t *);

STATIC struct qinit tcap_rinit = {
	.qi_putp = ss7_oput,		/* Read put (message from below) */
	.qi_srvp = ss7_osrv,		/* Read queue service */
	.qi_qopen = tcap_open,		/* Each open */
	.qi_qclose = tcap_close,	/* Last close */
	.qi_minfo = &tcap_minfo,	/* Information */
	.qi_mstat = &tcap_mstat,	/* Statistics */
};

STATIC struct qinit tcap_winit = {
	.qi_putp = ss7_iput,		/* Write put (message from above) */
	.qi_srvp = ss7_isrv,		/* Write queue service */
	.qi_minfo = &tcap_minfo,	/* Information */
	.qi_mstat = &tcap_mstat,	/* Statistics */
};

STATIC struct module_info sccp_minfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME,		/* Module ID name */
	.mi_minpsz = 1,			/* Min packet size accepted */
	.mi_maxpsz = 272 + 1,		/* Max packet size accepted */
	.mi_hiwat = 1 << 15,		/* Hi water mark */
	.mi_lowat = 1 << 10,		/* Lo water mark */
};

STATIC struct module_stat sccp_mstat = { 0, };

STATIC struct qinit sccp_rinit = {
	.qi_putp = ss7_iput,		/* Read put (message from below) */
	.qi_srvp = ss7_isrv,		/* Read queue service */
	.qi_minfo = &sccp_minfo,	/* Information */
	.qi_mstat = &sccp_mstat,	/* Statistics */
};

STATIC struct qinit sccp_winit = {
	.qi_putp = ss7_oput,		/* Write put (message from above) */
	.qi_srvp = ss7_osrv,		/* Write queue service */
	.qi_minfo = &sccp_minfo,	/* Information */
	.qi_mstat = &sccp_mstat,	/* Statistics */
};

MODULE_STATIC struct streamtab tcapinfo = {
	.st_rdinit = &tcap_rinit,	/* Upper read queue */
	.st_wrinit = &tcap_winit,	/* Upper write queue */
	.st_muxrinit = &sccp_rinit,	/* Lower read queue */
	.st_muxwinit = &sccp_winit,	/* Lower write queue */
};

/* 
 *  =========================================================================
 *
 *  Private Structures
 *
 *  =========================================================================
 */

#define TCAP_BUF_MAXLEN	    32
typedef struct tcap_buf {
	uchar buf[TCAP_BUF_MAXLEN];
	size_t len;
} tcap_buf_t;

typedef struct tcap_options {
	tcap_buf_t ctx;			/* application context */
	tcap_buf_t inf;			/* user information */
	t_uscalar_t pcl;		/* protocol class */
	t_uscalar_t ret;		/* return option */
	t_uscalar_t imp;		/* importance */
	t_uscalar_t seq;		/* sequence control */
	t_uscalar_t pri;		/* priority */
	t_uscalar_t sls;		/* signalling link selection */
	t_uscalar_t mp;			/* message priority */
	t_uscalar_t mtu;		/* maximum transfer unit */
} tcap_options_t;

struct tcap_options opt_defaults;

struct tcap;				/* TCAP user */
struct iv;				/* invoke */
struct dg;				/* dialog */
struct tr;				/* transaction */
struct sp;				/* SCCP-SAP */
struct sccp;				/* SCCP provider */

/*
 *  Trasnaction Capabilities Application Part
 *  -----------------------------------------------------------
 */
typedef struct tcap {
	STR_DECLARATION (struct tcap);	/* stream declaration */
	SLIST_HEAD (tr, tr);		/* transaction list */
	SLIST_HEAD (dg, dg);		/* dialogue list */
	ulong token;			/* token for this stream */
	ulong conind;			/* max connection inds */
	ulong outcnt;			/* out connection inds */
	ulong pclass;			/* protocol class */
	ulong reterr;			/* return on error */
	struct sccp_addr src;		/* bound src address */
	uint8_t saddr[SCCP_MAX_ADDR_LENGTH];
	struct sccp_addr dst;		/* bound dst address */
	uint8_t daddr[SCCP_MAX_ADDR_LENGTH];
	bufq_t conq;			/* connection indication queue */
	struct tcap_options options;	/* settable options */
	struct lmi_option option;	/* tcap protocol variant and options */
	struct tcap_notify_tc notify;	/* tcap notifications */
	struct tcap_timers_tc timers;	/* tcap timers */
	struct tcap_opt_conf_tc config;	/* tcap configuration */
	struct tcap_stats_tc statsp;	/* tcap statistics periods */
	struct tcap_stats_tc stamp;	/* tcap statistics timestamps */
	struct tcap_stats_tc stats;	/* tcap statistics */
} tcap_t;
#define TCAP_PRIV(__q) ((struct tcap *)(__q)->q_ptr)

STATIC struct tcap *tcap_alloc_priv(queue_t *, struct tcap **, dev_t *, cred_t *, ushort);
STATIC struct tcap *tcap_get(struct tcap *);
STATIC struct tcap *tcap_lookup(ulong);
STATIC ulong tcap_get_id(ulong);
STATIC void tcap_free_priv(struct tcap *);
STATIC void tcap_put(struct tcap *);

/*
 *  Invoke
 *  -----------------------------------------------------------
 */
typedef struct iv {
	HEAD_DECLARATION (struct iv);	/* head declaration */
	SLIST_LINKAGE (dg, iv, dg);	/* associated dialogue */
	ulong oper;			/* operation */
	ulong iid;			/* invoke id */
	ulong lid;			/* linked id */
	struct tcap_notify_iv notify;	/* invoke notifications */
	struct tcap_timers_iv timers;	/* invoke protocol timers */
	struct tcap_opt_conf_iv config;	/* invoke configuration */
	struct tcap_stats_iv statsp;	/* invoke statistics periods */
	struct tcap_stats_iv stamp;	/* invoke statsistics timestamps */
	struct tcap_stats_iv stats;	/* invoke statistics */
} iv_t;

STATIC struct iv *tcap_alloc_iv(ulong);
STATIC struct iv *iv_get(struct iv *);
STATIC struct iv *iv_lookup(struct dg *, ulong);
STATIC ulong iv_get_id(ulong);
STATIC void tcap_free_iv(struct iv *);
STATIC void iv_put(struct iv *);

/*
 *  Dialog
 *  -----------------------------------------------------------
 */
typedef struct dg {
	HEAD_DECLARATION (struct dg);	/* head declaration */
	SLIST_LINKAGE (tr, dg, tr);	/* associated transaction */
	SLIST_LINKAGE (iv, dg, iv);	/* list of invocations */
	ulong did;			/* dialogue id */
	struct tcap_notify_dg notify;	/* dialog notifications */
	struct tcap_timers_dg timers;	/* dialog protocol timers */
	struct tcap_opt_conf_dg config;	/* dialog configuration */
	struct tcap_stats_dg statsp;	/* dialog statistics periods */
	struct tcap_stats_dg stamp;	/* dialog statsistics timestamps */
	struct tcap_stats_dg stats;	/* dialog statistics */
} dg_t;

STATIC struct dg *tcap_alloc_dg(ulong);
STATIC struct dg *dg_get(struct dg *);
STATIC struct dg *dg_lookup(struct tr *, ulong);
STATIC ulong dg_get_id(ulong);
STATIC void tcap_free_dg(struct dg *);
STATIC void dg_put(struct dg *);

/*
 *  Transaction
 *  -----------------------------------------------------------
 */
typedef struct tr {
	HEAD_DECLARATION (struct tr);	/* head declaration */
	SLIST_LINKAGE (tcap, tr, tcap);	/* associated TCAP user */
	SLIST_LINKAGE (sp, tr, sp);	/* associated SCCP-SAP */
	SLIST_HEAD (iv, iv);		/* invokes for this transaction */
	ulong cid;			/* correlation id */
	ulong tid;			/* transaction id */
	ulong ocls;			/* operation class */
	struct tcap_notify_tr notify;	/* transaction notifications */
	struct tcap_timers_tr timers;	/* transaction protocol timers */
	struct tcap_opt_conf_tr config;	/* transaction configuration */
	struct tcap_stats_tr statsp;	/* transaction statistics periods */
	struct tcap_stats_tr stamp;	/* transaction statsistics timestamps */
	struct tcap_stats_tr stats;	/* transaction statistics */
} tr_t;

STATIC struct tr *tcap_alloc_tr(struct tcap *, ulong, ulong);
STATIC struct tr *tr_get(struct tr *);
STATIC struct tr *tr_lookup_cid(struct tcap *, ulong);
STATIC struct tr *tr_lookup_tid(struct tcap *, ulong);
STATIC ulong tr_get_id(ulong);
STATIC void tcap_free_tr(struct tr *);
STATIC void tr_put(struct tr *);

/*
 *  SCCP SAP
 *  -----------------------------------------------------------
 */
typedef struct sp {
	HEAD_DECLARATION (struct sp);	/* head declaration */
	struct sccp *sccp;		/* sccp for this sp */
	SLIST_HEAD (tr, tr);		/* transaction list for this sp */
	struct tcap_notify_sp notify;	/* SCCP-SAP notifications */
	struct tcap_timers_sp timers;	/* SCCP-SAP protocol timers */
	struct tcap_opt_conf_sp config;	/* SCCP-SAP configuration */
	struct tcap_stats_sp statsp;	/* SCCP-SAP statistics periods */
	struct tcap_stats_sp stamp;	/* SCCP-SAP statistics timestamps */
	struct tcap_stats_sp stats;	/* SCCP-SAP statistics */
} sp_t;

STATIC struct sp *tcap_alloc_sp(ulong);
STATIC struct sp *sp_get(struct sp *);
STATIC struct sp *sp_lookup(ulong);
STATIC ulong sp_get_id(ulong);
STATIC void tcap_free_sp(struct sp *);
STATIC void sp_put(struct sp *);

/*
 *  SCCP
 *  -----------------------------------------------------------
 */
typedef struct sccp {
	STR_DECLARATION (struct sccp);	/* stream declaration */
	struct sp *sp;			/* SCCP-SAP instance for this stream */
	size_t outcnt;			/* outstanding connection indications */
	ulong psdu;			/* pSDU size */
	ulong pidu;			/* pIDU size */
	ulong podu;			/* pODU size */
	struct sccp_addr add;		/* local address */
	uint8_t addr[SCCP_MAX_ADDR_LENGTH];
	struct lmi_option option;	/* protocol variant and options */
	struct tcap_notify_sc notify;	/* tcap notifications */
	struct tcap_timers_sc timers;	/* tcap protocol timers */
	struct tcap_opt_conf_sc config;	/* tcap configuration */
	struct tcap_stats_sc statsp;	/* tcap statistics periods */
	struct tcap_stats_sc stamp;	/* tcap statistics timestamp */
	struct tcap_stats_sc stats;	/* tcap statistics */
} sccp_t;
#define SCCP_PRIV(__q) ((struct sccp *)(__q)->q_ptr)

STATIC struct sccp *tcap_alloc_link(ulong);
STATIC struct sccp *sccp_get(struct sccp *);
STATIC struct sccp *sccp_lookup(ulong);
STATIC ulong sccp_get_id(ulong);
STATIC void sccp_free_link(struct sccp *);
STATIC void sccp_put(struct sccp *);

/*
 *  Default
 *  -----------------------------------------------------------
 */
typedef struct df {
	spinlock_t lock;
	SLIST_HEAD (tcap, tcap);	/* master list of TCAP users */
	SLIST_HEAD (tr, tr);		/* master list of transactions */
	SLIST_HEAD (sp, sp);		/* master list of SCCP-SAPs */
	SLIST_HEAD (sccp, sccp);	/* master list of SCCP providers */
	struct lmi_option proto;	/* default protocol variant and options */
	struct tcap_notify_df notify;	/* default notifications */
	struct tcap_timers_df timers;	/* default protocol timers */
	struct tcap_opt_conf_df config;	/* default configuration */
	struct tcap_stats_df statsp;	/* default statistics periods */
	struct tcap_stats_df stamp;	/* default statistics timestamps */
	struct tcap_stats_df stats;	/* default statistics */
} df_t;
STATIC struct df master;

/*
 *  =========================================================================
 *
 *  TCAP Message Structures
 *
 *  =========================================================================
 */

/* Message Types */
#define	TCAP_MT_UNI		1	/* Unidirectional */
#define	TCAP_MT_QWP		2	/* Query w/ permission */
#define	TCAP_MT_QWOP		3	/* Query w/o permission */
#define	TCAP_MT_CWP		4	/* Conversation w/ permission */
#define	TCAP_MT_CWOP		5	/* Conversation w/o permission */
#define	TCAP_MT_RESP		6	/* Response */
#define	TCAP_MT_ABORT		7	/* Abort */

/* Component Types */
#define	TCAP_CT_INVOKE_L	1	/* Invoke (Last) */
#define	TCAP_CT_INVOKE_NL	2	/* Invoke (Not Last) */
#define	TCAP_CT_RESULT_L	3	/* Return Result (Last) */
#define	TCAP_CT_RESULT_NL	4	/* Return Result (Not Last) */
#define	TCAP_CT_REJECT		5	/* Reject */
#define	TCAP_CT_ERROR		6	/* Error */

#define TCAP_TAG_UNIV_INT	2	/* UNIV Integer */
#define TCAP_TAG_UNIV_OSTR	4	/* UNIV Octet String */
#define TCAP_TAG_UNIV_OID	6	/* UNIV Object Id */
#define	TCAP_TAG_UNIV_PSEQ	16	/* UNIV Parameter Sequence */
#define TCAP_TAG_UNIV_UTC	17	/* UNIV Universal Time */
#define TCAP_TAG_UNIV_SEQ	48	/* UNIV Sequence */

#define	TCAP_TAG_TCAP_ACG	 1	/* TCAP ACG Indicators */
#define	TCAP_TAG_TCAP_STA	 2	/* TCAP Standard Announcement */
#define TCAP_TAG_TCAP_CUA	 3	/* TCAP Customized Announcment */
#define	TCAP_TAG_TCAP_TDIG	 4	/* TCAP Digits */
#define	TCAP_TAG_TCAP_SUEC	 5	/* TCAP Standard User Error Code */
#define	TCAP_TAG_TCAP_PDTA	 6	/* TCAP Problem Data */
#define	TCAP_TAG_TCAP_TCGPA	 7	/* TCAP SCCP Calling Party Address */
#define	TCAP_TAG_TCAP_TRSID	 8	/* TCAP Transaction ID */
#define	TCAP_TAG_TCAP_PCTY	 9	/* TCAP Package Type */
#define	TCAP_TAG_TCAP_SKEY	10	/* TCAP Service Key (Constructor) */
#define	TCAP_TAG_TCAP_BISTAT	11	/* TCAP Busy/Idle Status */
#define	TCAP_TAG_TCAP_CFSTAT	12	/* TCAP Call Forwarding Status */
#define	TCAP_TAG_TCAP_ORIGR	13	/* TCAP Origination Restrictions */
#define	TCAP_TAG_TCAP_TERMR	14	/* TCAP Terminating Restrictions */
#define	TCAP_TAG_TCAP_DNMAP	15	/* TCAP DN to Line Service TYpe Mapping */
#define	TCAP_TAG_TCAP_DURTN	16	/* TCAP Duration */
#define	TCAP_TAG_TCAP_RETD	17	/* TCAP Return Data (Constructor) */
#define	TCAP_TAG_TCAP_BCRQ	18	/* TCAP Bearer Capability Requested */
#define	TCAP_TAG_TCAP_BCSUP	19	/* TCAP Bearer Capability Supported */
#define	TCAP_TAG_TCAP_REFID	20	/* TCAP Reference Id */
#define	TCAP_TAG_TCAP_BGROUP	21	/* TCAP Business Group */
#define	TCAP_TAG_TCAP_SNI	22	/* TCAP Signalling Networks Identifier */
#define	TCAP_TAG_TCAP_MWIT	23	/* TCAP Message Waiting Indicator Type */

#define	TCAP_TAG_PRIV_UNI	 1	/* ANSI Unidirectional */
#define	TCAP_TAG_PRIV_QWP	 2	/* ANSI Query w/ permission */
#define	TCAP_TAG_PRIV_QWOP	 3	/* ANSI Query w/o permission */
#define	TCAP_TAG_PRIV_RESP	 4	/* ANSI Response */
#define	TCAP_TAG_PRIV_CWP	 5	/* ANSI Conversaion w/ permission */
#define	TCAP_TAG_PRIV_CWOP	 6	/* ANSI Conversaion w/o permission */
#define	TCAP_TAG_PRIV_TRSID	 7	/* ANSI Transaction Id */
#define	TCAP_TAG_PRIV_CSEQ	 8	/* ANSI Component Sequence */
#define	TCAP_TAG_PRIV_INKL	 9	/* ANSI Invoke (Last) */
#define	TCAP_TAG_PRIV_RRL	10	/* ANSI Return Result (Last) */
#define	TCAP_TAG_PRIV_RER	11	/* ANSI Return Error */
#define	TCAP_TAG_PRIV_REJ	12	/* ANSI Reject */
#define	TCAP_TAG_PRIV_INK	13	/* ANSI Invoke (Not Last) */
#define	TCAP_TAG_PRIV_RR	14	/* ANSI Result (Not Last) */
#define	TCAP_TAG_PRIV_CORID	15	/* ANSI Correlation Id(s) */
#define	TCAP_TAG_PRIV_NOPCO	16	/* ANSI National Operation Code */
#define	TCAP_TAG_PRIV_POPCO	17	/* ANSI Private Operation Code */
#define	TCAP_TAG_PRIV_PSET	18	/* ANSI Parameter Set */
#define TCAP_TAG_PRIV_NECODE	19	/* ANSI National Error Code */
#define TCAP_TAG_PRIV_PECODE	20	/* ANSI Private Error Code */
#define	TCAP_TAG_PRIV_PBCODE	21	/* ANSI Reject Problem Code */
#define	TCAP_TAG_PRIV_PSEQ	21	/* ANSI Parameter Sequence */
#define	TCAP_TAG_PRIV_ABORT	22	/* ANSI Abort */
#define	TCAP_TAG_PRIV_PCAUSE	23	/* ANSI P-Abort Cause */
#define	TCAP_TAG_PRIV_U_ABORT	24	/* ANSI User Abort Information */
#define	TCAP_TAG_PRIV_DLGP	25	/* ANSI Dialog Portion */
#define	TCAP_TAG_PRIV_VERSION	26	/* ANSI Protocol Version */
#define	TCAP_TAG_PRIV_CONTEXT	27	/* ANSI Integer Application Context */
#define	TCAP_TAG_PRIV_CTX_OID	28	/* ANSI OID Application Context */
#define	TCAP_TAG_PRIV_UINFO	29	/* ANSI User Information */

#define TCAP_TAG_APPL_UNI	1	/* ITUT Unidirectional */
#define TCAP_TAG_APPL_BEGIN	2	/* ITUT Begin Transaction */
#define TCAP_TAG_APPL_END	4	/* ITUT End Transaction */
#define TCAP_TAG_APPL_CONT	5	/* ITUT Continue Transaction */
#define TCAP_TAG_APPL_ABORT	7	/* ITUT Abort Transaction */
#define TCAP_TAG_APPL_ORIGID	8	/* ITUT Origination Transaction Id */
#define TCAP_TAG_APPL_DESTID	9	/* ITUT Destination Transaction Id */
#define TCAP_TAG_APPL_PCAUSE	10	/* ITUT P-Abort Cause */
#define TCAP_TAG_APPL_DLGP	11	/* ITUT Dialog Portion */
#define TCAP_TAG_APPL_CSEQ	12	/* ITUT Component Portion */

#define TCAP_TAG_APPL_AUDT_PDU	0	/* ITUT AUDT APDU */
#define TCAP_TAG_APPL_AARQ_PDU	0	/* ITUT AARQ APDU */
#define TCAP_TAG_APPL_AARE_PDU	1	/* ITUT AARE APDU */
#define TCAP_TAG_APPL_RLRQ_PDU	2	/* ITUT RLRQ APDU */
#define TCAP_TAG_APPL_RLRE_PDU	3	/* ITUT RLRE APDU */
#define TCAP_TAG_APPL_ABRT_PDU	4	/* ITUT ABRT APDU */

#define TCAP_TAG_CNTX_LID	0	/* Linked Id */
#define TCAP_TAG_CNTX_INK	1	/* Invoke */
#define TCAP_TAG_CNTX_RRL	2	/* Return Result (Last) */
#define TCAP_TAG_CNTX_RER	3	/* Return Error */
#define TCAP_TAG_CNTX_REJ	4	/* Reject */
#define TCAP_TAG_CNTX_RR	7	/* Return Result (Not Last) */

typedef struct sc_msg {
	struct sccp_addr orig;		/* Originating address */
	uchar oaddr[SCCP_MAX_ADDR_LENGTH];	/* Originating address signals */
	struct sccp_addr dest;		/* Destination address */
	uchar daddr[SCCP_MAX_ADDR_LENGTH];	/* Destination address signals */
	ulong pcl;			/* protocol class */
	ulong imp;			/* importance */
	ulong seq;			/* sequence control */
	ulong ret;			/* return option */
	ulong sls;			/* signalling link selection */
	ulong mp;			/* message priority */
} sc_msg_t;

typedef struct tr_msg {
	struct sc_msg sccp;		/* SCCP message */
	ulong type;			/* Message type */
	ulong parms;			/* Bit mask of parms included */
	ulong origid;			/* Originating Transaction Id */
	ulong destid;			/* Destination Transaction Id */
	ulong cause;			/* Abort Cause */
	ulong version;			/* TCAP version flags */
	uchar *dlg_beg;			/* beg of dialog portion */
	uchar *dlg_end;			/* end of dialog portion */
	uchar *cmp_beg;			/* beg of components or u-abort info */
	uchar *cmp_end;			/* end of components or u-abort info */
	uchar *abt_beg;			/* beg of u-abort info */
	uchar *abt_end;			/* end of u-abort info */
} tr_msg_t;

#define TCAP_PTF_ORIGID	(1<<0)	/* Originating Transaction id */
#define TCAP_PTF_DESTID	(1<<1)	/* Destination Transaction id */
#define TCAP_PTF_DLGP	(1<<2)	/* Dialog Portion */
#define TCAP_PTF_CSEQ	(1<<3)	/* Component Portion */

typedef struct tc_msg {
	ulong type;			/* Component type */
	ulong parms;			/* Bit mask of parms included */
	ulong iid;			/* Invoke Id */
	ulong lid;			/* Linked Id */
	ulong opcode;			/* Operation Code */
	ulong ecode;			/* Error Code */
	ulong pcode;			/* Problem Code */
	uchar *prm_beg;			/* beg of parameters */
	uchar *prm_end;			/* end of parameters */
} tc_msg_t;

#define TCAP_PTF_IID	(1<<0)	/* Invoke Id */
#define TCAP_PTF_LID	(1<<1)	/* Linked Id */
#define TCAP_PTF_NOPCO	(1<<2)	/* National Opcode */
#define TCAP_PTF_POPCO	(1<<3)	/* Private Opcode */
#define TCAP_PTF_LOPCO	(1<<4)	/* Local Opcode */
#define TCAP_PTF_PSEQ	(1<<5)	/* Parameter Sequence */
#define TCAP_PTF_NECODE	(1<<6)	/* National Error Code */
#define TCAP_PTF_PECODE	(1<<7)	/* Private Error Code */
#define TCAP_PTF_LECODE	(1<<8)	/* Local Error Code */
#define TCAP_PTF_PCODE	(1<<9)	/* Problem Code */

/*
 *  =========================================================================
 *
 *  TCAP DECODE Message Functions
 *
 *  =========================================================================
 */
/*
 *  General purpose decoding functions.
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
unpack_taglen(uchar **p, uchar **e, ulong *tag, ulong *cls)
{
	ulong len;
	if (*p >= *e)
		return (-EMSGSIZE);
	*cls = *(*p)++;
	if ((*cls & 0x1f) != 0x1f) {	/* tag is not extended */
		*tag = (*cls & 0x1f);
	} else {		/* tag is extended */
		uint8_t ptag;
		*tag = 0;
		if (*p >= *e)
			return (-EMSGSIZE);
		ptag = *(*p)++;
		*tag |= (ptag & 0x7f);
		if (ptag & 0x80) {
			if (*p >= *e)
				return (-EMSGSIZE);
			*tag <<= 7;
			ptag = *(*p)++;
			*tag |= (ptag & 0x7f);
			if (ptag & 0x80) {
				if (*p >= *e)
					return (-EMSGSIZE);
				*tag <<= 7;
				ptag = *(*p)++;
				*tag |= (ptag & 0x7f);
				if (ptag & 0x80) {
					if (*p >= *e)
						return (-EMSGSIZE);
					*tag <<= 7;
					ptag = *(*p)++;
					*tag |= (ptag & 0x7f);
					if (ptag & 0x80)
						return (-EMSGSIZE);
				}
			}
		}
	}
	if (*p >= *e)
		return (-EMSGSIZE);
	len = *(*p)++;
	if ((len & 0x80) != 0x00) {	/* extended length */
		ulong plen = len & 0x7f;
		if (plen > 4 || plen == 0)
			return (-EMSGSIZE);
		len = 0;
		while (plen--) {
			if (*p >= *e)
				return (-EMSGSIZE);
			len |= *(*p)++;
			len <<= 8;
		}
		if (*p + len > *e)
			return (-EMSGSIZE);
		*e = *p + len;
	}
	*cls &= ~0x1f;
	return (0);
}

/* unpack universal primitive class 0 form 0 */
STATIC INLINE int
unpack_univ_prim(uchar **p, uchar **e, ulong *tag)
{
	int err;
	ulong cls;
	if ((err = unpack_taglen(p, e, tag, &cls)))
		return (err);
	if ((cls & 0xe0) == 0x00)
		return (0);
	return (-EINVAL);
}

/* unpack universal constructor class 0 form 1 */
STATIC INLINE int
unpack_univ_cons(uchar **p, uchar **e, ulong *tag)
{
	int err;
	ulong cls;
	if ((err = unpack_taglen(p, e, tag, &cls)))
		return (err);
	if ((cls & 0xe0) == 0x20)
		return (0);
	return (-EINVAL);
}

/* unpack application-wide primitive class 1 form 0 */
STATIC INLINE int
unpack_appl_prim(uchar **p, uchar **e, ulong *tag)
{
	int err;
	ulong cls;
	if ((err = unpack_taglen(p, e, tag, &cls)))
		return (err);
	if ((cls & 0xe0) == 0x80)
		return (0);
	return (-EINVAL);
}

/* unpack application-wide constructor class 1 form 1 */
STATIC INLINE int
unpack_appl_cons(uchar **p, uchar **e, ulong *tag)
{
	int err;
	ulong cls;
	if ((err = unpack_taglen(p, e, tag, &cls)))
		return (err);
	if ((cls & 0xe0) == 0xa0)
		return (0);
	return (-EINVAL);
}

/* unpack context-specific primitive class 2 form 0 */
STATIC INLINE int
unpack_ctxt_prim(uchar **p, uchar **e, ulong *tag)
{
	int err;
	ulong cls;
	if ((err = unpack_taglen(p, e, tag, &cls)))
		return (err);
	if ((cls & 0xe0) == 0x40)
		return (0);
	return (-EINVAL);
}

/* unpack context-specific constructor class 2 form 1 */
STATIC INLINE int
unpack_ctxt_cons(uchar **p, uchar **e, ulong *tag)
{
	int err;
	ulong cls;
	if ((err = unpack_taglen(p, e, tag, &cls)))
		return (err);
	if ((cls & 0xe0) == 0x60)
		return (0);
	return (-EINVAL);
}

/* unpack private primitive class 3 form 0 */
STATIC INLINE int
unpack_priv_prim(uchar **p, uchar **e, ulong *tag)
{
	int err;
	ulong cls;
	if ((err = unpack_taglen(p, e, tag, &cls)))
		return (err);
	if ((cls & 0xe0) == 0xc0)
		return (0);
	return (-EINVAL);
}

/* unpack private constructor class 3 form 1 */
STATIC INLINE int
unpack_priv_cons(uchar **p, uchar **e, ulong *tag)
{
	int err;
	ulong cls;
	if ((err = unpack_taglen(p, e, tag, &cls)))
		return (err);
	if ((cls & 0xe0) == 0xe0)
		return (0);
	return (-EINVAL);
}
STATIC INLINE int
unpack_implicit_int(uchar **p, uchar *e, ulong *val)
{
	/* FIXME */
	return (-EFAULT);
}
STATIC INLINE int
unpack_explicit_int(uchar **p, uchar *e, ulong *val)
{
	/* FIXME */
	return (-EFAULT);
}

/*
 *  =========================================================================
 *
 *  TCAP DECODE Messages (Component (TC) Sub-Layer)
 *
 *  =========================================================================
 *
 *  ANSI PRIVATE-TCAP version of Components.
 *
 *  -------------------------------------------------------------------------
 *
 *  UNPACK ANSI Correlation Ids.
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
unpack_priv_corids(uchar **p, uchar *e, struct tc_msg *m, const ulong ctype)
{
	int err;
	ulong tag;
	if ((err = unpack_priv_prim(p, &e, &tag)))
		return (err);
	if (tag != TCAP_TAG_PRIV_CORID)
		return (-EINVAL);
	switch (e - *p) {
	case 0:		/* no ids */
		switch (ctype) {
		case TCAP_CT_INVOKE_L:
		case TCAP_CT_INVOKE_NL:
		case TCAP_CT_REJECT:
			return (0);
		}
		return (-EPROTO);
	case 1:		/* invoke id */
		switch (ctype) {
		default:
		case TCAP_CT_INVOKE_L:
		case TCAP_CT_INVOKE_NL:
		case TCAP_CT_RESULT_L:
		case TCAP_CT_RESULT_NL:
		case TCAP_CT_REJECT:
		case TCAP_CT_ERROR:
			if (*p >= e)
				return (-EMSGSIZE);
			m->iid = *(*p)++;
			m->parms |= TCAP_PTF_IID;
			break;
		}
		return (-EPROTO);
	case 2:		/* invoke id and correlation id */
		switch (ctype) {
		case TCAP_CT_INVOKE_L:
		case TCAP_CT_INVOKE_NL:
			if (*p >= e)
				return (-EMSGSIZE);
			m->iid = *(*p)++;
			m->parms |= TCAP_PTF_IID;
			if (*p >= e)
				return (-EMSGSIZE);
			m->lid = *(*p)++;
			m->parms |= TCAP_PTF_LID;
			return (0);
		}
		return (-EPROTO);
	}
	return (-EINVAL);
}

/*
 *  UNPACK ANSI Opcodes
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
unpack_priv_opcode(uchar **p, uchar *e, struct tc_msg *m)
{
	int err;
	ulong tag;
	if ((err = unpack_priv_prim(p, &e, &tag)))
		return (err);
	if (e - *p != 2)
		return (-EMSGSIZE);
	switch (tag) {
	case TCAP_TAG_PRIV_NOPCO:
		if ((err = unpack_implicit_int(p, e, &m->opcode)))
			return (err);
		m->parms |= TCAP_PTF_NOPCO;
		return (0);
	case TCAP_TAG_PRIV_POPCO:
		if ((err = unpack_implicit_int(p, e, &m->opcode)))
			return (err);
		m->parms |= TCAP_PTF_POPCO;
		return (0);
	}
	return (-EPROTO);
}

/*
 *  UNPACK ANSI Error Codes
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
unpack_priv_ecode(uchar **p, uchar *e, struct tc_msg *m)
{
	int err;
	ulong tag;
	if ((err = unpack_priv_prim(p, &e, &tag)))
		return (err);
	switch (tag) {
	case TCAP_TAG_PRIV_NECODE:
		if ((err = unpack_explicit_int(p, e, &m->ecode)))
			return (err);
		m->parms |= TCAP_PTF_NECODE;
		return (0);
	case TCAP_TAG_PRIV_PECODE:
		if ((err = unpack_explicit_int(p, e, &m->ecode)))
			return (err);
		m->parms |= TCAP_PTF_PECODE;
		return (0);
	}
	return (-EPROTO);
}

/*
 *  UNPACK ANSI Problem Codes
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
unpack_priv_pcode(uchar **p, uchar *e, struct tc_msg *m)
{
	int err;
	ulong tag;
	if ((err = unpack_priv_prim(p, &e, &tag)))
		return (err);
	if ((err = unpack_implicit_int(p, e, &m->pcode)))
		return (err);
	m->parms |= TCAP_PTF_PCODE;
	return (0);
}

/*
 *  UNPACK ANSI Component Parameter Sets
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
unpack_priv_pset(uchar **p, uchar *e, struct tc_msg *m)
{
	int err;
	ulong tag;
	if ((err = unpack_priv_cons(p, &e, &tag)))
		return (err);
	if (tag != TCAP_TAG_PRIV_PSET && tag != TCAP_TAG_PRIV_PSEQ)
		return (-EINVAL);
	m->prm_beg = *p;
	m->prm_end = e;
	*p = e;
	m->parms |= TCAP_PTF_PSEQ;
	return (0);
}

/*
 *  ANSI INVOKE (LAST)
 *  ------------------------------------------------------------------------
 */
STATIC int
tcap_priv_dec_inkl(uchar **p, uchar *e, struct tc_msg *m)
{
	int err;
	m->type = TCAP_CT_INVOKE_L;
	if ((err = unpack_priv_corids(p, e, m, TCAP_CT_INVOKE_L)))
		return (err);
	if ((err = unpack_priv_opcode(p, e, m)))
		return (err);
	if ((err = unpack_priv_pset(p, e, m)))
		return (err);
	if (*p != e)
		return (-EMSGSIZE);
	return (0);
}

/*
 *  ANSI INVOKE (NOT LAST)
 *  ------------------------------------------------------------------------
 */
STATIC int
tcap_priv_dec_ink(uchar **p, uchar *e, struct tc_msg *m)
{
	int err;
	m->type = TCAP_CT_INVOKE_NL;
	if ((err = unpack_priv_corids(p, e, m, TCAP_CT_INVOKE_NL)))
		return (err);
	if ((err = unpack_priv_opcode(p, e, m)))
		return (err);
	if ((err = unpack_priv_pset(p, e, m)))
		return (err);
	if (*p != e)
		return (-EMSGSIZE);
	return (0);
}

/*
 *  ANSI RETURN RESULT (LAST)
 *  ------------------------------------------------------------------------
 */
STATIC int
tcap_priv_dec_rrl(uchar **p, uchar *e, struct tc_msg *m)
{
	int err;
	m->type = TCAP_CT_RESULT_L;
	if ((err = unpack_priv_corids(p, e, m, TCAP_CT_RESULT_L)))
		return (err);
	if ((err = unpack_priv_pset(p, e, m)))
		return (err);
	if (*p != e)
		return (-EMSGSIZE);
	return (0);
}

/*
 *  ANSI RETURN RESULT (NOT LAST)
 *  ------------------------------------------------------------------------
 */
STATIC int
tcap_priv_dec_rr(uchar **p, uchar *e, struct tc_msg *m)
{
	int err;
	m->type = TCAP_CT_RESULT_NL;
	if ((err = unpack_priv_corids(p, e, m, TCAP_CT_RESULT_NL)))
		return (err);
	if ((err = unpack_priv_pset(p, e, m)))
		return (err);
	if (*p != e)
		return (-EMSGSIZE);
	return (0);
}

/*
 *  ANSI RETURN ERROR
 *  ------------------------------------------------------------------------
 */
STATIC int
tcap_priv_dec_rer(uchar **p, uchar *e, struct tc_msg *m)
{
	int err;
	m->type = TCAP_CT_ERROR;
	if ((err = unpack_priv_corids(p, e, m, TCAP_CT_ERROR)))
		return (err);
	if ((err = unpack_priv_ecode(p, e, m)))
		return (err);
	if ((err = unpack_priv_pset(p, e, m)))
		return (err);
	if (*p != e)
		return (-EMSGSIZE);
	return (0);
}

/*
 *  ANSI REJECT
 *  ------------------------------------------------------------------------
 */
STATIC int
tcap_priv_dec_rej(uchar **p, uchar *e, struct tc_msg *m)
{
	int err;
	m->type = TCAP_CT_REJECT;
	if ((err = unpack_priv_corids(p, e, m, TCAP_CT_REJECT)))
		return (err);
	if ((err = unpack_priv_pcode(p, e, m)))
		return (err);
	if ((err = unpack_priv_pset(p, e, m)))
		return (err);
	if (*p != e)
		return (-EMSGSIZE);
	return (0);
}

/*
 *  ANSI Component Decoder
 *  ------------------------------------------------------------------------
 */
STATIC int
tcap_priv_dec_comp(uchar **p, uchar *e, struct tc_msg *m)
{
	int err;
	ulong tag;
	if (!(err = unpack_appl_cons(p, &e, &tag)))
		switch (tag) {
		case TCAP_TAG_PRIV_INKL:	/* Invoke (Last) */
			if (!(err = tcap_priv_dec_inkl(p, e, m)))
				return (0);
			break;
		case TCAP_TAG_PRIV_INK:	/* Invoke (Not Last) */
			if (!(err = tcap_priv_dec_ink(p, e, m)))
				return (0);
			break;
		case TCAP_TAG_PRIV_RRL:	/* Return Result (Last) */
			if (!(err = tcap_priv_dec_rrl(p, e, m)))
				return (0);
			break;
		case TCAP_TAG_PRIV_RR:	/* Return Result (Not Last) */
			if (!(err = tcap_priv_dec_rr(p, e, m)))
				return (0);
			break;
		case TCAP_TAG_PRIV_RER:	/* Return Error */
			if (!(err = tcap_priv_dec_rer(p, e, m)))
				return (0);
			break;
		case TCAP_TAG_PRIV_REJ:	/* Reject */
			if (!(err = tcap_priv_dec_rej(p, e, m)))
				return (0);
			break;
		default:
			err = -EINVAL;
			break;
		}
	return (err);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  ITUT APPLICATION-TCAP version of Components.
 *
 *  -------------------------------------------------------------------------
 *
 *  UNPACK ITUT Correlation Ids.
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
unpack_univ_iid(uchar **p, uchar *e, struct tc_msg *m)
{
	(void) p;
	(void) e;
	(void) m;
	/* FIXME */
	return (-EFAULT);
}
STATIC INLINE int
unpack_univ_lid(uchar **p, uchar *e, struct tc_msg *m)
{
	(void) p;
	(void) e;
	(void) m;
	/* FIXME */
	return (-EFAULT);
}
STATIC INLINE int
unpack_univ_corids(uchar **p, uchar *e, struct tc_msg *m, const ulong ctype)
{
	int err;
	switch (ctype) {
	case TCAP_CT_INVOKE_L:
		if ((err = unpack_univ_iid(p, e, m)))
			return (err);
		m->parms |= TCAP_PTF_IID;
		if ((err = unpack_univ_lid(p, e, m)))	/* optional */
			return (err);
		m->parms |= TCAP_PTF_LID;
		return (0);
	case TCAP_CT_RESULT_L:
	case TCAP_CT_RESULT_NL:
	case TCAP_CT_ERROR:
		if ((err = unpack_univ_iid(p, e, m)))
			return (err);
		m->parms |= TCAP_PTF_IID;
		return (0);
	case TCAP_CT_REJECT:
		/* could be NULL */
		if ((err = unpack_univ_iid(p, e, m)))
			return (err);
		m->parms |= TCAP_PTF_IID;
		return (0);
	}
	return (-EFAULT);
}

/*
 *  UNPACK ITUT Opcodes
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
unpack_univ_opcode(uchar **p, uchar *e, struct tc_msg *m)
{
	int err;
	ulong tag;
	if ((err = unpack_univ_prim(p, &e, &tag)))
		return (err);
	if (tag == TCAP_TAG_UNIV_OID)	/* 6 */
		return (-EOPNOTSUPP);	/* we don't support OID opcodes */
	if (tag != TCAP_TAG_UNIV_INT)	/* 2 */
		return (-EPROTO);
	if ((err = unpack_implicit_int(p, e, &m->opcode)))
		return (err);
	m->parms |= TCAP_PTF_LOPCO;
	return (0);
}

/*
 *  UNPACK ITUT Parameter Sequences
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
unpack_univ_pseq(uchar **p, uchar *e, struct tc_msg *m, const ulong opt)
{
	int err;
	ulong tag;
	if (opt) {
		if (*p >= e)
			return (0);
		if (**p != (0x20 | TCAP_TAG_UNIV_SEQ))
			return (0);
	}
	if ((err = unpack_univ_cons(p, &e, &tag)))
		return (err);
	if (tag != TCAP_TAG_UNIV_SEQ)
		return (-EINVAL);
	m->prm_beg = *p;
	m->prm_end = e;
	*p = e;
	m->parms |= TCAP_PTF_PSEQ;
	return (0);
}

/*
 *  UNPACK ITUT Result Opcode and Parameter Sequences
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
unpack_univ_rseq(uchar **p, uchar *e, struct tc_msg *m)
{
	int err;
	ulong tag;
	if (*p >= e)
		return (0);
	if (**p != (0x20 | TCAP_TAG_UNIV_SEQ))
		return (0);
	if ((err = unpack_univ_cons(p, &e, &tag)))
		return (err);
	if (tag != TCAP_TAG_UNIV_SEQ)
		return (-EINVAL);
	if ((err = unpack_univ_opcode(p, e, m)))
		return (err);
	if ((err = unpack_univ_pseq(p, e, m, 0)))
		return (err);
	return (0);
}

/*
 *  UNPACK ITUT Error Codes
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
unpack_univ_ecode(uchar **p, uchar *e, struct tc_msg *m)
{
	int err;
	ulong tag;
	if ((err = unpack_univ_prim(p, &e, &tag)))
		return (err);
	switch (tag) {
	case TCAP_TAG_UNIV_INT:
		if ((err = unpack_implicit_int(p, e, &m->ecode)))
			return (err);
		m->parms |= TCAP_PTF_LECODE;
		return (0);
	case TCAP_TAG_UNIV_OID:
		return (-EOPNOTSUPP);	/* don't support global error codes */
	}
	return (-EPROTO);
}

/*
 *  UNPACK ITUT Problem Codes
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
unpack_ctxt_pcode(uchar **p, uchar *e, struct tc_msg *m)
{
	int err;
	ulong tag;
	ulong ptype;
	if ((err = unpack_ctxt_prim(p, &e, &tag)))
		return (err);
	ptype = tag;
	if ((err = unpack_implicit_int(p, e, &m->pcode)))
		return (err);
	m->pcode |= ptype << 16;
	m->parms |= TCAP_PTF_PCODE;
	return (0);
}

/*
 *  ITUT INVOKE (LAST)
 *  ------------------------------------------------------------------------
 */
STATIC int
tcap_ctxt_dec_inkl(uchar **p, uchar *e, struct tc_msg *m)
{
	int err;
	m->type = TCAP_CT_INVOKE_L;
	if ((err = unpack_univ_corids(p, e, m, TCAP_CT_INVOKE_L)))
		return (err);
	if ((err = unpack_univ_opcode(p, e, m)))
		return (err);
	if ((err = unpack_univ_pseq(p, e, m, 1)))
		return (err);
	if (*p != e)
		return (-EMSGSIZE);
	return (0);
}

/*
 *  ITUT RETURN RESULT (LAST)
 *  ------------------------------------------------------------------------
 */
STATIC int
tcap_ctxt_dec_rrl(uchar **p, uchar *e, struct tc_msg *m)
{
	int err;
	m->type = TCAP_CT_RESULT_L;
	if ((err = unpack_univ_corids(p, e, m, TCAP_CT_RESULT_L)))
		return (err);
	if ((err = unpack_univ_rseq(p, e, m)))
		return (err);
	if (*p != e)
		return (-EMSGSIZE);
	return (0);
}

/*
 *  ITUT RETURN RESULT (NOT LAST)
 *  ------------------------------------------------------------------------
 */
STATIC int
tcap_ctxt_dec_rr(uchar **p, uchar *e, struct tc_msg *m)
{
	int err;
	m->type = TCAP_CT_RESULT_NL;
	if ((err = unpack_univ_corids(p, e, m, TCAP_CT_RESULT_NL)))
		return (err);
	if ((err = unpack_univ_rseq(p, e, m)))
		return (err);
	if (*p != e)
		return (-EMSGSIZE);
	return (0);
}

/*
 *  ITUT RETURN ERROR
 *  ------------------------------------------------------------------------
 */
STATIC int
tcap_ctxt_dec_rer(uchar **p, uchar *e, struct tc_msg *m)
{
	int err;
	m->type = TCAP_CT_ERROR;
	if ((err = unpack_univ_corids(p, e, m, TCAP_CT_ERROR)))
		return (err);
	if ((err = unpack_univ_ecode(p, e, m)))
		return (err);
	if ((err = unpack_univ_pseq(p, e, m, 1)))
		return (err);
	if (*p != e)
		return (-EMSGSIZE);
	return (0);
}

/*
 *  ITUT REJECT
 *  ------------------------------------------------------------------------
 */
STATIC int
tcap_ctxt_dec_rej(uchar **p, uchar *e, struct tc_msg *m)
{
	int err;
	m->type = TCAP_CT_REJECT;
	if ((err = unpack_univ_corids(p, e, m, TCAP_CT_REJECT)))
		return (err);
	if ((err = unpack_ctxt_pcode(p, e, m)))
		return (err);
	if (*p != e)
		return (-EMSGSIZE);
	return (0);
}

/*
 *  ITUT Component Decoder
 *  ------------------------------------------------------------------------
 */
STATIC int
tcap_appl_dec_comp(uchar **p, uchar *e, struct tc_msg *m)
{
	int err;
	ulong tag;
	if (!(err = unpack_ctxt_cons(p, &e, &tag)))
		switch (tag) {
		case TCAP_TAG_CNTX_INK:	/* Invoke */
			if (!(err = tcap_ctxt_dec_inkl(p, e, m)))
				return (0);
			break;
		case TCAP_TAG_CNTX_RRL:	/* Return Result (Last) */
			if (!(err = tcap_ctxt_dec_rrl(p, e, m)))
				return (0);
			break;
		case TCAP_TAG_CNTX_RER:	/* Return Error */
			if (!(err = tcap_ctxt_dec_rer(p, e, m)))
				return (0);
			break;
		case TCAP_TAG_CNTX_REJ:	/* Reject */
			if (!(err = tcap_ctxt_dec_rej(p, e, m)))
				return (0);
			break;
		case TCAP_TAG_CNTX_RR:	/* Return Result (Not Last) */
			if (!(err = tcap_ctxt_dec_rr(p, e, m)))
				return (0);
			break;
		default:
			err = -EINVAL;
			break;
		}
	return (err);
}

/*
 *  =========================================================================
 *
 *  TCAP DECODE Messages (Transaction (TR) Sub-Layer)
 *
 *  =========================================================================
 */
/*
 *  PRIVATE-TCAP version of Packages.  (Used by the TR sub-layer.)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
unpack_priv_trsid(uchar **p, uchar *e, struct tr_msg *m, const ulong mtype)
{
	int err;
	ulong tag;
	if ((err = unpack_priv_prim(p, &e, &tag)))
		return (err);
	if (tag != TCAP_TAG_PRIV_TRSID)
		return (-EINVAL);
	switch (e - *p) {
	case 0:		/* no transaction ids */
		switch (mtype) {
		case TCAP_MT_UNI:
			return (0);
		}
		return (-EPROTO);
	case 4:		/* orig or dest transaction id */
		switch (mtype) {
		case TCAP_MT_QWP:
		case TCAP_MT_QWOP:
		case TCAP_MT_RESP:
		case TCAP_MT_ABORT:
			unpack_implicit_int(p, e, &m->origid);
			m->parms |= TCAP_PTF_ORIGID;
			return (0);
		}
		return (-EPROTO);
	case 8:		/* orig and dest transaction id */
		switch (mtype) {
		case TCAP_MT_CWP:
		case TCAP_MT_CWOP:
			unpack_implicit_int(p, e - 4, &m->origid);
			m->parms |= TCAP_PTF_ORIGID;
			unpack_implicit_int(p, e, &m->destid);
			m->parms |= TCAP_PTF_DESTID;
			return (0);
		}
		return (-EPROTO);
	}
	return (-EMSGSIZE);
}

/* ANSI dialog portion */
STATIC int
unpack_priv_dialog_portion(uchar **p, uchar *e, struct tr_msg *m)
{
	ulong err;
	ulong tag;
	if (*p >= e)
		return (0);
	if (**p != (0xc0 | TCAP_TAG_PRIV_DLGP))
		return (0);
	if ((err = unpack_priv_cons(p, &e, &tag)))
		return (err);
	if (tag != TCAP_TAG_PRIV_DLGP)
		return (-EINVAL);
	m->dlg_beg = *p;
	m->dlg_end = e;
	*p = e;
	m->parms |= TCAP_PTF_DLGP;
	return (0);
}

/* ANSI component portion */
STATIC int
unpack_priv_component_portion(uchar **p, uchar *e, struct tr_msg *m, ulong opt)
{
	ulong err;
	ulong tag;
	if (opt) {
		if (*p >= e)
			return (0);
		if (**p != (0xc0 | TCAP_TAG_PRIV_CSEQ))
			return (0);
	}
	if ((err = unpack_priv_cons(p, &e, &tag)))
		return (err);
	if (tag != TCAP_TAG_PRIV_CSEQ)
		return (-EINVAL);
	m->cmp_beg = *p;
	m->cmp_end = e;
	*p = e;
	m->parms |= TCAP_PTF_CSEQ;
	return (0);
}

/* ANSI abort cause information */
STATIC int
unpack_priv_cause_info(uchar **p, uchar *e, struct tr_msg *m)
{
	(void) p;
	(void) e;
	(void) m;
	/* FIXME */
	return (-EFAULT);
}
STATIC int
tcap_priv_dec_uni(uchar *p, uchar *e, struct tr_msg *m)
{
	int err;
	m->type = TCAP_MT_UNI;
	if ((err = unpack_priv_trsid(&p, e, m, TCAP_MT_UNI)))
		return (err);
	if ((err = unpack_priv_dialog_portion(&p, e, m)))
		return (err);
	if ((err = unpack_priv_component_portion(&p, e, m, 0)))
		return (err);
	return (0);
}
STATIC int
tcap_priv_dec_qwp(uchar *p, uchar *e, struct tr_msg *m)
{
	int err;
	m->type = TCAP_MT_QWP;
	if ((err = unpack_priv_trsid(&p, e, m, TCAP_MT_QWP)))
		return (err);
	if ((err = unpack_priv_dialog_portion(&p, e, m)))
		return (err);
	if ((err = unpack_priv_component_portion(&p, e, m, 1)))
		return (err);
	return (0);
}
STATIC int
tcap_priv_dec_qwop(uchar *p, uchar *e, struct tr_msg *m)
{
	int err;
	m->type = TCAP_MT_QWOP;
	if ((err = unpack_priv_trsid(&p, e, m, TCAP_MT_QWOP)))
		return (err);
	if ((err = unpack_priv_dialog_portion(&p, e, m)))
		return (err);
	if ((err = unpack_priv_component_portion(&p, e, m, 1)))
		return (err);
	return (0);
}
STATIC int
tcap_priv_dec_cwp(uchar *p, uchar *e, struct tr_msg *m)
{
	int err;
	m->type = TCAP_MT_CWP;
	if ((err = unpack_priv_trsid(&p, e, m, TCAP_MT_CWP)))
		return (err);
	if ((err = unpack_priv_dialog_portion(&p, e, m)))
		return (err);
	if ((err = unpack_priv_component_portion(&p, e, m, 1)))
		return (err);
	return (0);
}
STATIC int
tcap_priv_dec_cwop(uchar *p, uchar *e, struct tr_msg *m)
{
	int err;
	m->type = TCAP_MT_CWOP;
	if ((err = unpack_priv_trsid(&p, e, m, TCAP_MT_CWOP)))
		return (err);
	if ((err = unpack_priv_dialog_portion(&p, e, m)))
		return (err);
	if ((err = unpack_priv_component_portion(&p, e, m, 1)))
		return (err);
	return (0);
}
STATIC int
tcap_priv_dec_resp(uchar *p, uchar *e, struct tr_msg *m)
{
	int err;
	m->type = TCAP_MT_RESP;
	if ((err = unpack_priv_trsid(&p, e, m, TCAP_MT_RESP)))
		return (err);
	if ((err = unpack_priv_dialog_portion(&p, e, m)))
		return (err);
	if ((err = unpack_priv_component_portion(&p, e, m, 1)))
		return (err);
	return (0);
}
STATIC int
tcap_priv_dec_abt(uchar *p, uchar *e, struct tr_msg *m)
{
	int err;
	m->type = TCAP_MT_ABORT;
	if ((err = unpack_priv_trsid(&p, e, m, TCAP_MT_ABORT)))
		return (err);
	if ((err = unpack_priv_dialog_portion(&p, e, m)))
		return (err);
	if ((err = unpack_priv_cause_info(&p, e, m)))
		return (err);
	return (0);
}

/*
 *  APPLICATION-TCAP version of Packages.  (Used by the TR sub-layer.)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
unpack_appl_origid(uchar **p, uchar *e, struct tr_msg *m)
{
	ulong err;
	ulong tag;
	if ((err = unpack_appl_prim(p, &e, &tag)))
		return (err);
	if (tag != TCAP_TAG_APPL_ORIGID)
		return (-EPROTO);
	if (4 < e - *p || e - *p < 1)
		return (-EPROTO);
	unpack_implicit_int(p, e, &m->origid);
	m->parms |= TCAP_PTF_ORIGID;
	return (0);
}
STATIC INLINE int
unpack_appl_destid(uchar **p, uchar *e, struct tr_msg *m)
{
	ulong err;
	ulong tag;
	if ((err = unpack_appl_prim(p, &e, &tag)))
		return (err);
	if (tag != TCAP_TAG_APPL_DESTID)
		return (-EPROTO);
	if (4 < e - *p || e - *p < 1)
		return (-EPROTO);
	unpack_implicit_int(p, e, &m->destid);
	m->parms |= TCAP_PTF_DESTID;
	return (0);
}
STATIC INLINE int
unpack_appl_trsid(uchar **p, uchar *e, struct tr_msg *m, const ulong mtype)
{
	ulong err;
	switch (mtype) {
	case TCAP_MT_UNI:
		return (0);
	case TCAP_MT_QWP:
		if ((err = unpack_appl_origid(p, e, m)))
			return (err);
		return (0);
	case TCAP_MT_CWP:
		if ((err = unpack_appl_origid(p, e, m)))
			return (err);
		if ((err = unpack_appl_destid(p, e, m)))
			return (err);
		return (0);
	case TCAP_MT_RESP:
	case TCAP_MT_ABORT:
		if ((err = unpack_appl_destid(p, e, m)))
			return (err);
		return (0);
	}
	return (-EFAULT);
}

/* ITU-T dialog portion */
STATIC int
unpack_appl_dialog_portion(uchar **p, uchar *e, struct tr_msg *m)
{
	ulong err;
	ulong tag;
	if (*p >= e)
		return (0);
	if (**p != (0x60 | TCAP_TAG_APPL_DLGP))
		return (0);
	if ((err = unpack_appl_cons(p, &e, &tag)))
		return (err);
	if (tag != TCAP_TAG_APPL_DLGP)
		return (-EINVAL);
	m->dlg_beg = *p;
	m->dlg_end = e;
	*p = e;
	m->parms |= TCAP_PTF_DLGP;
	return (0);
}

/* ITU-T component portion */
STATIC int
unpack_appl_component_portion(uchar **p, uchar *e, struct tr_msg *m, ulong opt)
{
	ulong err;
	ulong tag;
	if (opt) {
		if (*p >= e)
			return (0);
		if (**p != (0x60 | TCAP_TAG_APPL_CSEQ))
			return (0);
	}
	if ((err = unpack_appl_cons(p, &e, &tag)))
		return (err);
	if (tag != TCAP_TAG_APPL_CSEQ)
		return (-EINVAL);
	m->cmp_beg = *p;
	m->cmp_end = e;
	*p = e;
	m->parms |= TCAP_PTF_CSEQ;
	return (0);
}

/* ITU-T abort cause information */
STATIC int
unpack_appl_cause_info(uchar **p, uchar *e, struct tr_msg *m)
{
	(void) p;
	(void) e;
	(void) m;
	/* FIXME */
	return (-EFAULT);
}
STATIC int
tcap_appl_dec_uni(uchar *p, uchar *e, struct tr_msg *m)
{
	/* Application Context, Component Sequence */
	int err;
	m->type = TCAP_MT_UNI;
	if ((err = unpack_appl_trsid(&p, e, m, TCAP_MT_UNI)))
		return (err);
	if ((err = unpack_appl_dialog_portion(&p, e, m)))
		return (err);
	if ((err = unpack_appl_component_portion(&p, e, m, 0)))
		return (err);
	return (0);
}
STATIC int
tcap_appl_dec_beg(uchar *p, uchar *e, struct tr_msg *m)
{
	/* Originating Transaction Id, Application Context, Component Sequence */
	int err;
	m->type = TCAP_MT_QWP;	/* Query with permission to release */
	if ((err = unpack_appl_trsid(&p, e, m, TCAP_MT_QWP)))
		return (err);
	if ((err = unpack_appl_dialog_portion(&p, e, m)))
		return (err);
	if ((err = unpack_appl_component_portion(&p, e, m, 1)))
		return (err);
	return (0);
}
STATIC int
tcap_appl_dec_cnt(uchar *p, uchar *e, struct tr_msg *m)
{
	int err;
	m->type = TCAP_MT_CWP;	/* Conversation with permission to release */
	if ((err = unpack_appl_trsid(&p, e, m, TCAP_MT_CWP)))
		return (err);
	if ((err = unpack_appl_dialog_portion(&p, e, m)))
		return (err);
	if ((err = unpack_appl_component_portion(&p, e, m, 1)))
		return (err);
	return (0);
}
STATIC int
tcap_appl_dec_end(uchar *p, uchar *e, struct tr_msg *m)
{
	/* Dest Transaction Id, Application Context, Component Sequence */
	int err;
	m->type = TCAP_MT_RESP;	/* Response */
	if ((err = unpack_appl_trsid(&p, e, m, TCAP_MT_RESP)))
		return (err);
	if ((err = unpack_appl_dialog_portion(&p, e, m)))
		return (err);
	if ((err = unpack_appl_component_portion(&p, e, m, 1)))
		return (err);
	return (0);
}
STATIC int
tcap_appl_dec_abt(uchar *p, uchar *e, struct tr_msg *m)
{
	/* Dest Transaction Id, P-Abort-Cause prim or U-Abort-Info const */
	int err;
	if ((err = unpack_appl_trsid(&p, e, m, TCAP_MT_ABORT)))
		return (err);
	if ((err = unpack_appl_dialog_portion(&p, e, m)))
		return (err);
	if ((err = unpack_appl_cause_info(&p, e, m)))
		return (err);
	return (0);
}

/*
 *  TCAP Package decoder.  (Only decodes TR Sub-Layer.)
 *  -------------------------------------------------------------------------
 */
STATIC int
tcap_dec_msg(uchar *p, uchar *e, struct tr_msg *m)
{
	int err;
	ulong tag;
	ulong cls;
	if ((err = unpack_taglen(&p, &e, &tag, &cls)))
		return (err);
	if ((cls & 0xe0) == 0xe0) {	/* private constructor */
		switch (tag) {
		case TCAP_TAG_PRIV_UNI:	/* Unidirectional */
			return tcap_priv_dec_uni(p, e, m);
		case TCAP_TAG_PRIV_QWP:	/* Begin or Query w/ permission */
			return tcap_priv_dec_qwp(p, e, m);
		case TCAP_TAG_PRIV_QWOP:	/* Query w/o permission */
			return tcap_priv_dec_qwop(p, e, m);
		case TCAP_TAG_PRIV_RESP:	/* End or Response */
			return tcap_priv_dec_resp(p, e, m);
		case TCAP_TAG_PRIV_CWP:	/* Continue or Conversation w/ permission */
			return tcap_priv_dec_cwp(p, e, m);
		case TCAP_TAG_PRIV_CWOP:	/* Conversation w/o permission */
			return tcap_priv_dec_cwop(p, e, m);
		case TCAP_TAG_PRIV_ABORT:	/* Abort */
			return tcap_priv_dec_abt(p, e, m);
		}
	}
	if ((cls & 0xe0) == 0xa0) {	/* application-wide constructor */
		switch (tag) {
		case TCAP_TAG_APPL_UNI:	/* Unidirectional */
			return tcap_appl_dec_uni(p, e, m);
		case TCAP_TAG_APPL_BEGIN:	/* Begin */
			return tcap_appl_dec_beg(p, e, m);
		case TCAP_TAG_APPL_END:	/* End */
			return tcap_appl_dec_end(p, e, m);
		case TCAP_TAG_APPL_CONT:	/* Continue */
			return tcap_appl_dec_cnt(p, e, m);
		case TCAP_TAG_APPL_ABORT:	/* Abort */
			return tcap_appl_dec_abt(p, e, m);
		}
	}
	return (-EINVAL);
}

/*
 *  =========================================================================
 *
 *  TCAP ENCODE Message Functions
 *
 *  =========================================================================
 */
/*
 *  General purpose encoding functions.
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
len_size(ulong len)
{
	if (!(len & 0xffffff80))
		return (1);
	if (len & 0xff000000)
		return (5);
	if (len & 0xffff0000)
		return (4);
	if (len & 0xffffff00)
		return (3);
	if (len & 0xffffff80)
		return (2);
	return (1);
}
STATIC INLINE int
tag_size(ulong tag)
{
	if (!(tag & 0xffffffc0))
		return (1);
	if (tag & 0xf0000000)
		return (6);
	if (tag & 0xffe00000)
		return (5);
	if (tag & 0xffffc000)
		return (4);
	if (tag & 0xffffff80)
		return (3);
	if (tag & 0xffffffc0)
		return (2);
	return (1);
}
STATIC INLINE void
pack_tag_class(uchar **p, ulong tag, ulong fcls)
{
	int n;
	if (tag < 32) {
		*(*p)++ = tag | fcls;
		return;
	}
	n = tag_size(tag) - 1;
	*(*p)++ = 0x1f | fcls;
	switch (n) {
	case 5:
		*(*p)++ = (tag >> 28) | 0x80;
	case 4:
		*(*p)++ = (tag >> 21) | 0x80;
	case 3:
		*(*p)++ = (tag >> 14) | 0x80;
	case 2:
		*(*p)++ = (tag >> 7) | 0x80;
	case 1:
		*(*p)++ = (tag >> 0) | 0x00;
	}
	return;
}
STATIC INLINE void
pack_tag_univ_prim(uchar **p, ulong tag)
{
	return pack_tag_class(p, tag, 0x00);
}
STATIC INLINE void
pack_tag_univ_cons(uchar **p, ulong tag)
{
	return pack_tag_class(p, tag, 0x20);
}
STATIC INLINE void
pack_tag_appl_prim(uchar **p, ulong tag)
{
	return pack_tag_class(p, tag, 0x40);
}
STATIC INLINE void
pack_tag_appl_cons(uchar **p, ulong tag)
{
	return pack_tag_class(p, tag, 0x60);
}
STATIC INLINE void
pack_tag_cntx_prim(uchar **p, ulong tag)
{
	return pack_tag_class(p, tag, 0x80);
}
STATIC INLINE void
pack_tag_cntx_cons(uchar **p, ulong tag)
{
	return pack_tag_class(p, tag, 0xa0);
}
STATIC INLINE void
pack_tag_priv_prim(uchar **p, ulong tag)
{
	return pack_tag_class(p, tag, 0xc0);
}
STATIC INLINE void
pack_tag_priv_cons(uchar **p, ulong tag)
{
	return pack_tag_class(p, tag, 0xe0);
}
STATIC INLINE void
pack_len(uchar **p, ulong len)
{
	ulong n;
	if (len < 128) {
		*(*p)++ = len;
		return;
	}
	n = len_size(len) - 1;
	*(*p)++ = 0x80 | n;
	switch (n) {
	case 4:
		*(*p)++ = len >> 24;
	case 3:
		*(*p)++ = len >> 16;
	case 2:
		*(*p)++ = len >> 8;
	case 1:
		*(*p)++ = len >> 0;
	}
	return;
}
STATIC INLINE void
pack_int(uchar **p, ulong val, size_t len)
{
	fixme(("Write this function.\n"));
	return;
}

/*
 *  =========================================================================
 *
 *  TCAP ENCODE Messages (Transaction (TR) Sub-Layer).
 *
 *  =========================================================================
 *
 *  ANSI TCAP Message (Transaction (TR) Sub-Layer) encoding.
 *
 *  -------------------------------------------------------------------------
 *
 *  ANSI UNI (Unidirectional)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE mblk_t *
tcap_priv_enc_uni(mblk_t *dp, mblk_t *cp)
{
	mblk_t *mp, *mc;
	size_t pkg_len, dlg_len, cmp_len, mlen, hlen, ilen, dlen, clen;
	/* 
	 *  TODO: reduce these to constants where possible
	 */
	dlg_len = dp ? msgdsize(dp) : 0;
	cmp_len = cp ? msgdsize(cp) : 0;
	ilen = tag_size(TCAP_TAG_PRIV_TRSID) + len_size(0);
	dlen = dlg_len ? (tag_size(TCAP_TAG_PRIV_DLGP) + len_size(dlg_len)) : 0;
	clen = cmp_len ? (tag_size(TCAP_TAG_PRIV_CSEQ) + len_size(cmp_len)) : 0;
	pkg_len = ilen + dlen + dlg_len + clen + cmp_len;
	hlen = tag_size(TCAP_TAG_PRIV_UNI) + len_size(pkg_len);
	mlen = hlen + ilen + dlen ? dlen : clen;
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		pack_tag_priv_cons(&mp->b_wptr, TCAP_TAG_PRIV_UNI);
		pack_len(&mp->b_wptr, pkg_len);
		pack_tag_priv_prim(&mp->b_wptr, TCAP_TAG_PRIV_TRSID);
		pack_len(&mp->b_wptr, 0);
		if (clen && !dlen) {	/* common case */
			pack_tag_priv_cons(&mp->b_wptr, TCAP_TAG_PRIV_CSEQ);
			pack_len(&mp->b_wptr, cmp_len);
			linkb(mp, cp);
			return (mp);
		}
		if (dlen) {	/* oddball case */
			pack_tag_priv_cons(&mp->b_wptr, TCAP_TAG_PRIV_DLGP);
			pack_len(&mp->b_wptr, dlg_len);
			linkb(mp, dp);
		}
		if (clen) {	/* even odder case */
			if ((mc = allocb(clen, BPRI_MED))) {
				mc->b_datap->db_type = M_DATA;
				pack_tag_priv_cons(&mc->b_wptr, TCAP_TAG_PRIV_CSEQ);
				pack_len(&mc->b_wptr, cmp_len);
				linkb(mp, mc);
				linkb(mp, cp);
				return (mp);
			}
			freeb(mp);
			return (NULL);
		}
	}
	return (mp);
}

/*
 *  ANSI QWP (Query with permission)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE mblk_t *
tcap_priv_enc_qwp(ulong origid, mblk_t *dp, mblk_t *cp)
{
	mblk_t *mp, *mc;
	size_t pkg_len, dlg_len, cmp_len, mlen, hlen, ilen, dlen, clen;
	/* 
	 *  TODO: reduce these to constants where possible
	 */
	dlg_len = dp ? msgdsize(dp) : 0;
	cmp_len = cp ? msgdsize(cp) : 0;
	ilen = tag_size(TCAP_TAG_PRIV_TRSID) + len_size(sizeof(origid));
	dlen = dlg_len ? (tag_size(TCAP_TAG_PRIV_DLGP) + len_size(dlg_len)) : 0;
	clen = cmp_len ? (tag_size(TCAP_TAG_PRIV_CSEQ) + len_size(cmp_len)) : 0;
	pkg_len = ilen + sizeof(origid) + dlen + dlg_len + clen + cmp_len;
	hlen = tag_size(TCAP_TAG_PRIV_QWP) + len_size(pkg_len);
	mlen = hlen + ilen + dlen ? dlen : clen;
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		pack_tag_priv_cons(&mp->b_wptr, TCAP_TAG_PRIV_QWP);
		pack_len(&mp->b_wptr, pkg_len);
		pack_tag_priv_prim(&mp->b_wptr, TCAP_TAG_PRIV_TRSID);
		pack_len(&mp->b_wptr, sizeof(origid));
		pack_int(&mp->b_wptr, origid, sizeof(origid));
		if (clen && !dlen) {	/* common case */
			pack_tag_priv_cons(&mp->b_wptr, TCAP_TAG_PRIV_CSEQ);
			pack_len(&mp->b_wptr, cmp_len);
			linkb(mp, cp);
			return (mp);
		}
		if (dlen) {	/* oddball case */
			pack_tag_priv_cons(&mp->b_wptr, TCAP_TAG_PRIV_DLGP);
			pack_len(&mp->b_wptr, dlg_len);
			linkb(mp, dp);
		}
		if (clen) {	/* even odder case */
			if ((mc = allocb(clen, BPRI_MED))) {
				mc->b_datap->db_type = M_DATA;
				pack_tag_priv_cons(&mc->b_wptr, TCAP_TAG_PRIV_CSEQ);
				pack_len(&mc->b_wptr, cmp_len);
				linkb(mp, mc);
				linkb(mp, cp);
				return (mp);
			}
			freeb(mp);
			return (NULL);
		}
	}
	return (mp);
}

/*
 *  ANSI QWOP (Query without permission)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE mblk_t *
tcap_priv_enc_qwop(ulong origid, mblk_t *dp, mblk_t *cp)
{
	mblk_t *mp, *mc;
	size_t pkg_len, dlg_len, cmp_len, mlen, hlen, ilen, dlen, clen;
	/* 
	 *  TODO: reduce these to constants where possible
	 */
	dlg_len = dp ? msgdsize(dp) : 0;
	cmp_len = cp ? msgdsize(cp) : 0;
	ilen = tag_size(TCAP_TAG_PRIV_TRSID) + len_size(sizeof(origid));
	dlen = dlg_len ? (tag_size(TCAP_TAG_PRIV_DLGP) + len_size(dlg_len)) : 0;
	clen = cmp_len ? (tag_size(TCAP_TAG_PRIV_CSEQ) + len_size(cmp_len)) : 0;
	pkg_len = ilen + sizeof(origid) + dlen + dlg_len + clen + cmp_len;
	hlen = tag_size(TCAP_TAG_PRIV_QWOP) + len_size(pkg_len);
	mlen = hlen + ilen + dlen ? dlen : clen;
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		pack_tag_priv_cons(&mp->b_wptr, TCAP_TAG_PRIV_QWOP);
		pack_len(&mp->b_wptr, pkg_len);
		pack_tag_priv_prim(&mp->b_wptr, TCAP_TAG_PRIV_TRSID);
		pack_len(&mp->b_wptr, sizeof(origid));
		pack_int(&mp->b_wptr, origid, sizeof(origid));
		if (clen && !dlen) {	/* common case */
			pack_tag_priv_cons(&mp->b_wptr, TCAP_TAG_PRIV_CSEQ);
			pack_len(&mp->b_wptr, cmp_len);
			linkb(mp, cp);
			return (mp);
		}
		if (dlen) {	/* oddball case */
			pack_tag_priv_cons(&mp->b_wptr, TCAP_TAG_PRIV_DLGP);
			pack_len(&mp->b_wptr, dlg_len);
			linkb(mp, dp);
		}
		if (clen) {	/* even odder case */
			if ((mc = allocb(clen, BPRI_MED))) {
				mc->b_datap->db_type = M_DATA;
				pack_tag_priv_cons(&mc->b_wptr, TCAP_TAG_PRIV_CSEQ);
				pack_len(&mc->b_wptr, cmp_len);
				linkb(mp, mc);
				linkb(mp, cp);
				return (mp);
			}
			freeb(mp);
			return (NULL);
		}
	}
	return (mp);
}

/*
 *  ANSI RESP (Response)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE mblk_t *
tcap_priv_enc_resp(ulong destid, mblk_t *dp, mblk_t *cp)
{
	mblk_t *mp, *mc;
	size_t pkg_len, dlg_len, cmp_len, mlen, hlen, ilen, dlen, clen;
	/* 
	 *  TODO: reduce these to constants where possible
	 */
	dlg_len = dp ? msgdsize(dp) : 0;
	cmp_len = cp ? msgdsize(cp) : 0;
	ilen = tag_size(TCAP_TAG_PRIV_TRSID) + len_size(sizeof(destid));
	dlen = dlg_len ? (tag_size(TCAP_TAG_PRIV_DLGP) + len_size(dlg_len)) : 0;
	clen = cmp_len ? (tag_size(TCAP_TAG_PRIV_CSEQ) + len_size(cmp_len)) : 0;
	pkg_len = ilen + sizeof(destid) + dlen + dlg_len + clen + cmp_len;
	hlen = tag_size(TCAP_TAG_PRIV_RESP) + len_size(pkg_len);
	mlen = hlen + ilen + dlen ? dlen : clen;
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		pack_tag_priv_cons(&mp->b_wptr, TCAP_TAG_PRIV_RESP);
		pack_len(&mp->b_wptr, pkg_len);
		pack_tag_priv_prim(&mp->b_wptr, TCAP_TAG_PRIV_TRSID);
		pack_len(&mp->b_wptr, sizeof(destid));
		pack_int(&mp->b_wptr, destid, sizeof(destid));
		if (clen && !dlen) {	/* common case */
			pack_tag_priv_cons(&mp->b_wptr, TCAP_TAG_PRIV_CSEQ);
			pack_len(&mp->b_wptr, cmp_len);
			linkb(mp, cp);
			return (mp);
		}
		if (dlen) {	/* oddball case */
			pack_tag_priv_cons(&mp->b_wptr, TCAP_TAG_PRIV_DLGP);
			pack_len(&mp->b_wptr, dlg_len);
			linkb(mp, dp);
		}
		if (clen) {	/* even odder case */
			if ((mc = allocb(clen, BPRI_MED))) {
				mc->b_datap->db_type = M_DATA;
				pack_tag_priv_cons(&mc->b_wptr, TCAP_TAG_PRIV_CSEQ);
				pack_len(&mc->b_wptr, cmp_len);
				linkb(mp, mc);
				linkb(mp, cp);
				return (mp);
			}
			freeb(mp);
			return (NULL);
		}
	}
	return (mp);
}

/*
 *  ANSI CWP (Conversation with permission)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE mblk_t *
tcap_priv_enc_cwp(ulong origid, ulong destid, mblk_t *dp, mblk_t *cp)
{
	mblk_t *mp, *mc;
	size_t pkg_len, dlg_len, cmp_len, mlen, hlen, ilen, dlen, clen;
	/* 
	 *  TODO: reduce these to constants where possible
	 */
	dlg_len = dp ? msgdsize(dp) : 0;
	cmp_len = cp ? msgdsize(cp) : 0;
	ilen = tag_size(TCAP_TAG_PRIV_TRSID) + len_size(sizeof(origid) + sizeof(destid));
	dlen = dlg_len ? (tag_size(TCAP_TAG_PRIV_DLGP) + len_size(dlg_len)) : 0;
	clen = cmp_len ? (tag_size(TCAP_TAG_PRIV_CSEQ) + len_size(cmp_len)) : 0;
	pkg_len = ilen + sizeof(origid) + sizeof(destid) + dlen + dlg_len + clen + cmp_len;
	hlen = tag_size(TCAP_TAG_PRIV_CWP) + len_size(pkg_len);
	mlen = hlen + ilen + dlen ? dlen : clen;
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		pack_tag_priv_cons(&mp->b_wptr, TCAP_TAG_PRIV_CWP);
		pack_len(&mp->b_wptr, pkg_len);
		pack_tag_priv_prim(&mp->b_wptr, TCAP_TAG_PRIV_TRSID);
		pack_len(&mp->b_wptr, sizeof(origid) + sizeof(destid));
		pack_int(&mp->b_wptr, origid, sizeof(origid));
		pack_int(&mp->b_wptr, destid, sizeof(destid));
		if (clen && !dlen) {	/* common case */
			pack_tag_priv_cons(&mp->b_wptr, TCAP_TAG_PRIV_CSEQ);
			pack_len(&mp->b_wptr, cmp_len);
			linkb(mp, cp);
			return (mp);
		}
		if (dlen) {	/* oddball case */
			pack_tag_priv_cons(&mp->b_wptr, TCAP_TAG_PRIV_DLGP);
			pack_len(&mp->b_wptr, dlg_len);
			linkb(mp, dp);
		}
		if (clen) {	/* even odder case */
			if ((mc = allocb(clen, BPRI_MED))) {
				mc->b_datap->db_type = M_DATA;
				pack_tag_priv_cons(&mc->b_wptr, TCAP_TAG_PRIV_CSEQ);
				pack_len(&mc->b_wptr, cmp_len);
				linkb(mp, mc);
				linkb(mp, cp);
				return (mp);
			}
			freeb(mp);
			return (NULL);
		}
	}
	return (mp);
}

/*
 *  ANSI CWOP (Conversation without permission)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE mblk_t *
tcap_priv_enc_cwop(ulong origid, ulong destid, mblk_t *dp, mblk_t *cp)
{
	mblk_t *mp, *mc;
	size_t pkg_len, dlg_len, cmp_len, mlen, hlen, ilen, dlen, clen;
	/* 
	 *  TODO: reduce these to constants where possible
	 */
	dlg_len = dp ? msgdsize(dp) : 0;
	cmp_len = cp ? msgdsize(cp) : 0;
	ilen = tag_size(TCAP_TAG_PRIV_TRSID) + len_size(sizeof(origid) + sizeof(destid));
	dlen = dlg_len ? (tag_size(TCAP_TAG_PRIV_DLGP) + len_size(dlg_len)) : 0;
	clen = cmp_len ? (tag_size(TCAP_TAG_PRIV_CSEQ) + len_size(cmp_len)) : 0;
	pkg_len = ilen + sizeof(origid) + sizeof(destid) + dlen + dlg_len + clen + cmp_len;
	hlen = tag_size(TCAP_TAG_PRIV_CWOP) + len_size(pkg_len);
	mlen = hlen + ilen + dlen ? dlen : clen;
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		pack_tag_priv_cons(&mp->b_wptr, TCAP_TAG_PRIV_CWOP);
		pack_len(&mp->b_wptr, pkg_len);
		pack_tag_priv_prim(&mp->b_wptr, TCAP_TAG_PRIV_TRSID);
		pack_len(&mp->b_wptr, sizeof(origid) + sizeof(destid));
		pack_int(&mp->b_wptr, origid, sizeof(origid));
		pack_int(&mp->b_wptr, destid, sizeof(destid));
		if (clen && !dlen) {	/* common case */
			pack_tag_priv_cons(&mp->b_wptr, TCAP_TAG_PRIV_CSEQ);
			pack_len(&mp->b_wptr, cmp_len);
			linkb(mp, cp);
			return (mp);
		}
		if (dlen) {	/* oddball case */
			pack_tag_priv_cons(&mp->b_wptr, TCAP_TAG_PRIV_DLGP);
			pack_len(&mp->b_wptr, dlg_len);
			linkb(mp, dp);
		}
		if (clen) {	/* even odder case */
			if ((mc = allocb(clen, BPRI_MED))) {
				mc->b_datap->db_type = M_DATA;
				pack_tag_priv_cons(&mc->b_wptr, TCAP_TAG_PRIV_CSEQ);
				pack_len(&mc->b_wptr, cmp_len);
				linkb(mp, mc);
				linkb(mp, cp);
				return (mp);
			}
			freeb(mp);
			return (NULL);
		}
	}
	return (mp);
}

/*
 *  ANSI ABORT (Abort)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE mblk_t *
tcap_priv_enc_abt(ulong destid, mblk_t *dp, mblk_t *cp)
{
	mblk_t *mp, *mc;
	size_t pkg_len, dlg_len, cmp_len, mlen, hlen, ilen, dlen, clen;
	/* 
	 *  TODO: reduce these to constants where possible
	 */
	dlg_len = dp ? msgdsize(dp) : 0;
	cmp_len = cp ? msgdsize(cp) : 0;
	ilen = tag_size(TCAP_TAG_PRIV_TRSID) + len_size(sizeof(destid));
	dlen = dlg_len ? (tag_size(TCAP_TAG_PRIV_DLGP) + len_size(dlg_len)) : 0;
	clen = cmp_len ? (tag_size(TCAP_TAG_PRIV_CSEQ) + len_size(cmp_len)) : 0;
	pkg_len = ilen + sizeof(destid) + dlen + dlg_len + clen + cmp_len;
	hlen = tag_size(TCAP_TAG_PRIV_ABORT) + len_size(pkg_len);
	mlen = hlen + ilen + dlen ? dlen : clen;
	if ((mp = allocb(mlen, BPRI_MED))) {
		/* 
		 *  FIXME for cause information rather than components
		 */
		mp->b_datap->db_type = M_DATA;
		pack_tag_priv_cons(&mp->b_wptr, TCAP_TAG_PRIV_ABORT);
		pack_len(&mp->b_wptr, pkg_len);
		pack_tag_priv_prim(&mp->b_wptr, TCAP_TAG_PRIV_TRSID);
		pack_len(&mp->b_wptr, sizeof(destid));
		pack_int(&mp->b_wptr, destid, sizeof(destid));
		if (clen && !dlen) {	/* common case */
			pack_tag_priv_cons(&mp->b_wptr, TCAP_TAG_PRIV_CSEQ);
			pack_len(&mp->b_wptr, cmp_len);
			linkb(mp, cp);
			return (mp);
		}
		if (dlen) {	/* oddball case */
			pack_tag_priv_cons(&mp->b_wptr, TCAP_TAG_PRIV_DLGP);
			pack_len(&mp->b_wptr, dlg_len);
			linkb(mp, dp);
		}
		if (clen) {	/* even odder case */
			if ((mc = allocb(clen, BPRI_MED))) {
				mc->b_datap->db_type = M_DATA;
				pack_tag_priv_cons(&mc->b_wptr, TCAP_TAG_PRIV_CSEQ);
				pack_len(&mc->b_wptr, cmp_len);
				linkb(mp, mc);
				linkb(mp, cp);
				return (mp);
			}
			freeb(mp);
			return (NULL);
		}
	}
	return (mp);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  ITUT TCAP Message (Transaction (TR) Sub-Layer) encoding.
 *
 *  -------------------------------------------------------------------------
 *
 *  ITUT UNI (Unidirectional)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE mblk_t *
tcap_appl_enc_uni(mblk_t *dp, mblk_t *cp)
{
	mblk_t *mp, *mc;
	size_t pkg_len, dlg_len, cmp_len, mlen, hlen, ilen, dlen, clen;
	/* 
	 *  TODO: reduce these to constants where possible
	 */
	dlg_len = dp ? msgdsize(dp) : 0;
	cmp_len = cp ? msgdsize(cp) : 0;
	ilen = 0;
	dlen = dlg_len ? (tag_size(TCAP_TAG_APPL_DLGP) + len_size(dlg_len)) : 0;
	clen = cmp_len ? (tag_size(TCAP_TAG_APPL_CSEQ) + len_size(cmp_len)) : 0;
	pkg_len = ilen + dlen + dlg_len + clen + cmp_len;
	hlen = tag_size(TCAP_TAG_APPL_UNI) + len_size(pkg_len);
	mlen = hlen + ilen + dlen ? dlen : clen;
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		pack_tag_appl_cons(&mp->b_wptr, TCAP_TAG_APPL_UNI);
		pack_len(&mp->b_wptr, pkg_len);
		if (clen && !dlen) {	/* common case, components no dialog */
			pack_tag_appl_cons(&mp->b_wptr, TCAP_TAG_APPL_CSEQ);
			pack_len(&mp->b_wptr, cmp_len);
			linkb(mp, cp);
			return (mp);
		}
		if (dlen) {	/* oddball case, dialog */
			pack_tag_appl_cons(&mp->b_wptr, TCAP_TAG_APPL_DLGP);
			pack_len(&mp->b_wptr, dlg_len);
			linkb(mp, dp);
		}
		if (clen) {	/* oddbal case, dialog and components */
			if ((mc = allocb(clen, BPRI_MED))) {
				mc->b_datap->db_type = M_DATA;
				pack_tag_appl_cons(&mc->b_wptr, TCAP_TAG_APPL_CSEQ);
				pack_len(&mc->b_wptr, cmp_len);
				linkb(mp, mc);
				linkb(mp, cp);
				return (mp);
			}
			freeb(mp);
			return (NULL);
		}
	}
	return (mp);
}

/*
 *  ITUT BEG (Begin)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE mblk_t *
tcap_appl_enc_beg(ulong origid, mblk_t *dp, mblk_t *cp)
{
	mblk_t *mp, *mc;
	size_t pkg_len, dlg_len, cmp_len, mlen, hlen, ilen, dlen, clen;
	/* 
	 *  TODO: reduce these to constants where possible
	 */
	dlg_len = dp ? msgdsize(dp) : 0;
	cmp_len = cp ? msgdsize(cp) : 0;
	ilen = tag_size(TCAP_TAG_APPL_ORIGID) + len_size(sizeof(origid));
	dlen = dlg_len ? (tag_size(TCAP_TAG_APPL_DLGP) + len_size(dlg_len)) : 0;
	clen = cmp_len ? (tag_size(TCAP_TAG_APPL_CSEQ) + len_size(cmp_len)) : 0;
	pkg_len = ilen + sizeof(origid) + dlen + dlg_len + clen + cmp_len;
	hlen = tag_size(TCAP_TAG_APPL_BEGIN) + len_size(pkg_len);
	mlen = hlen + ilen + dlen ? dlen : clen;
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		pack_tag_appl_cons(&mp->b_wptr, TCAP_TAG_APPL_BEGIN);
		pack_len(&mp->b_wptr, pkg_len);
		pack_tag_appl_prim(&mp->b_wptr, TCAP_TAG_APPL_ORIGID);
		pack_len(&mp->b_wptr, sizeof(origid));
		pack_int(&mp->b_wptr, origid, sizeof(origid));
		if (clen && !dlen) {	/* common case, components no dialog */
			pack_tag_appl_cons(&mp->b_wptr, TCAP_TAG_APPL_CSEQ);
			pack_len(&mp->b_wptr, cmp_len);
			linkb(mp, cp);
			return (mp);
		}
		if (dlen) {	/* oddball case, dialog */
			pack_tag_appl_cons(&mp->b_wptr, TCAP_TAG_APPL_DLGP);
			pack_len(&mp->b_wptr, dlg_len);
			linkb(mp, dp);
		}
		if (clen) {	/* oddbal case, dialog and components */
			if ((mc = allocb(clen, BPRI_MED))) {
				mc->b_datap->db_type = M_DATA;
				pack_tag_appl_cons(&mc->b_wptr, TCAP_TAG_APPL_CSEQ);
				pack_len(&mc->b_wptr, cmp_len);
				linkb(mp, mc);
				linkb(mp, cp);
				return (mp);
			}
			freeb(mp);
			return (NULL);
		}
	}
	return (mp);
}

/*
 *  ITUT END (End)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE mblk_t *
tcap_appl_enc_end(ulong destid, mblk_t *dp, mblk_t *cp)
{
	mblk_t *mp, *mc;
	size_t pkg_len, dlg_len, cmp_len, mlen, hlen, ilen, dlen, clen;
	/* 
	 *  TODO: reduce these to constants where possible
	 */
	dlg_len = dp ? msgdsize(dp) : 0;
	cmp_len = cp ? msgdsize(cp) : 0;
	ilen = tag_size(TCAP_TAG_APPL_DESTID) + len_size(sizeof(destid));
	dlen = dlg_len ? (tag_size(TCAP_TAG_APPL_DLGP) + len_size(dlg_len)) : 0;
	clen = cmp_len ? (tag_size(TCAP_TAG_APPL_CSEQ) + len_size(cmp_len)) : 0;
	pkg_len = ilen + sizeof(destid) + dlen + dlg_len + clen + cmp_len;
	hlen = tag_size(TCAP_TAG_APPL_END) + len_size(pkg_len);
	mlen = hlen + ilen + dlen ? dlen : clen;
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		pack_tag_appl_cons(&mp->b_wptr, TCAP_TAG_APPL_END);
		pack_len(&mp->b_wptr, pkg_len);
		pack_tag_appl_prim(&mp->b_wptr, TCAP_TAG_APPL_DESTID);
		pack_len(&mp->b_wptr, sizeof(destid));
		pack_int(&mp->b_wptr, destid, sizeof(destid));
		if (clen && !dlen) {	/* common case, components no dialog */
			pack_tag_appl_cons(&mp->b_wptr, TCAP_TAG_APPL_CSEQ);
			pack_len(&mp->b_wptr, cmp_len);
			linkb(mp, cp);
			return (mp);
		}
		if (dlen) {	/* oddball case, dialog */
			pack_tag_appl_cons(&mp->b_wptr, TCAP_TAG_APPL_DLGP);
			pack_len(&mp->b_wptr, dlg_len);
			linkb(mp, dp);
		}
		if (clen) {	/* oddbal case, dialog and components */
			if ((mc = allocb(clen, BPRI_MED))) {
				mc->b_datap->db_type = M_DATA;
				pack_tag_appl_cons(&mc->b_wptr, TCAP_TAG_APPL_CSEQ);
				pack_len(&mc->b_wptr, cmp_len);
				linkb(mp, mc);
				linkb(mp, cp);
				return (mp);
			}
			freeb(mp);
			return (NULL);
		}
	}
	return (mp);
}

/*
 *  ITUT CONT (Continue)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE mblk_t *
tcap_appl_enc_cont(ulong origid, ulong destid, mblk_t *dp, mblk_t *cp)
{
	mblk_t *mp, *mc;
	size_t pkg_len, dlg_len, cmp_len, mlen, hlen, ilen, dlen, clen;
	/* 
	 *  TODO: reduce these to constants where possible
	 */
	dlg_len = dp ? msgdsize(dp) : 0;
	cmp_len = cp ? msgdsize(cp) : 0;
	ilen =
	    tag_size(TCAP_TAG_APPL_ORIGID) + len_size(sizeof(origid)) +
	    tag_size(TCAP_TAG_APPL_DESTID) + len_size(sizeof(destid));
	dlen = dlg_len ? (tag_size(TCAP_TAG_APPL_DLGP) + len_size(dlg_len)) : 0;
	clen = cmp_len ? (tag_size(TCAP_TAG_APPL_CSEQ) + len_size(cmp_len)) : 0;
	pkg_len = ilen + sizeof(destid) + dlen + dlg_len + clen + cmp_len;
	hlen = tag_size(TCAP_TAG_APPL_CONT) + len_size(pkg_len);
	mlen = hlen + ilen + dlen ? dlen : clen;
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		pack_tag_appl_cons(&mp->b_wptr, TCAP_TAG_APPL_CONT);
		pack_len(&mp->b_wptr, pkg_len);
		pack_tag_appl_prim(&mp->b_wptr, TCAP_TAG_APPL_ORIGID);
		pack_len(&mp->b_wptr, sizeof(origid));
		pack_int(&mp->b_wptr, origid, sizeof(origid));
		pack_tag_appl_prim(&mp->b_wptr, TCAP_TAG_APPL_DESTID);
		pack_len(&mp->b_wptr, sizeof(destid));
		pack_int(&mp->b_wptr, destid, sizeof(destid));
		if (clen && !dlen) {	/* common case, components no dialog */
			pack_tag_appl_cons(&mp->b_wptr, TCAP_TAG_APPL_CSEQ);
			pack_len(&mp->b_wptr, cmp_len);
			linkb(mp, cp);
			return (mp);
		}
		if (dlen) {	/* oddball case, dialog */
			pack_tag_appl_cons(&mp->b_wptr, TCAP_TAG_APPL_DLGP);
			pack_len(&mp->b_wptr, dlg_len);
			linkb(mp, dp);
		}
		if (clen) {	/* oddbal case, dialog and components */
			if ((mc = allocb(clen, BPRI_MED))) {
				mc->b_datap->db_type = M_DATA;
				pack_tag_appl_cons(&mc->b_wptr, TCAP_TAG_APPL_CSEQ);
				pack_len(&mc->b_wptr, cmp_len);
				linkb(mp, mc);
				linkb(mp, cp);
				return (mp);
			}
			freeb(mp);
			return (NULL);
		}
	}
	return (mp);
}

/*
 *  ITUT ABORT (Abort)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE mblk_t *
tcap_appl_enc_abt(ulong destid, mblk_t *dp, mblk_t *cp)
{
	mblk_t *mp, *mc;
	size_t pkg_len, dlg_len, cmp_len, mlen, hlen, ilen, dlen, clen;
	/* 
	 *  TODO: reduce these to constants where possible
	 */
	dlg_len = dp ? msgdsize(dp) : 0;
	cmp_len = cp ? msgdsize(cp) : 0;
	ilen = tag_size(TCAP_TAG_APPL_DESTID) + len_size(sizeof(destid));
	dlen = dlg_len ? (tag_size(TCAP_TAG_APPL_DLGP) + len_size(dlg_len)) : 0;
	clen = cmp_len ? (tag_size(TCAP_TAG_APPL_CSEQ) + len_size(cmp_len)) : 0;
	pkg_len = ilen + sizeof(destid) + dlen + dlg_len + clen + cmp_len;
	hlen = tag_size(TCAP_TAG_APPL_ABORT) + len_size(pkg_len);
	mlen = hlen + ilen + dlen ? dlen : clen;
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		pack_tag_appl_cons(&mp->b_wptr, TCAP_TAG_APPL_ABORT);
		pack_len(&mp->b_wptr, pkg_len);
		pack_tag_appl_prim(&mp->b_wptr, TCAP_TAG_APPL_DESTID);
		pack_len(&mp->b_wptr, sizeof(destid));
		pack_int(&mp->b_wptr, destid, sizeof(destid));
		/* 
		 *  FIXME for cause information rather than components
		 */
		if (clen && !dlen) {	/* common case, components no dialog */
			pack_tag_appl_cons(&mp->b_wptr, TCAP_TAG_APPL_CSEQ);
			pack_len(&mp->b_wptr, cmp_len);
			linkb(mp, cp);
			return (mp);
		}
		if (dlen) {	/* oddball case, dialog */
			pack_tag_appl_cons(&mp->b_wptr, TCAP_TAG_APPL_DLGP);
			pack_len(&mp->b_wptr, dlg_len);
			linkb(mp, dp);
		}
		if (clen) {	/* oddbal case, dialog and components */
			if ((mc = allocb(clen, BPRI_MED))) {
				mc->b_datap->db_type = M_DATA;
				pack_tag_appl_cons(&mc->b_wptr, TCAP_TAG_APPL_CSEQ);
				pack_len(&mc->b_wptr, cmp_len);
				linkb(mp, mc);
				linkb(mp, cp);
				return (mp);
			}
			freeb(mp);
			return (NULL);
		}
	}
	return (mp);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  ANSI TCAP Component (Component (TC) Sub-Layer) encoding.
 *
 *  -------------------------------------------------------------------------
 *
 *  ANSI INKL (Invoke Last)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE mblk_t *
tcap_priv_enc_inkl(ulong *iid, ulong *lid, ulong opclass, ulong opcode, mblk_t *pseq)
{
	mblk_t *mp = NULL;
	return (mp);
}

/*
 *  ANSI INK  (Invoke Not Last)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE mblk_t *
tcap_priv_enc_ink(ulong *iid, ulong *lid, ulong opclass, ulong opcode, mblk_t *pseq)
{
	mblk_t *mp = NULL;
	return (mp);
}

/*
 *  ANSI RRL  (Return Result Last)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE mblk_t *
tcap_priv_enc_rrl(ulong *iid, mblk_t *pseq)
{
	mblk_t *mp = NULL;
	return (mp);
}

/*
 *  ANSI RR   (Return Result Not Last)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE mblk_t *
tcap_priv_enc_rr(ulong *iid, mblk_t *pseq)
{
	mblk_t *mp = NULL;
	return (mp);
}

/*
 *  ANSI RER  (Return Error)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE mblk_t *
tcap_priv_enc_rer(ulong *iid, ulong ecode, mblk_t *pseq)
{
	mblk_t *mp = NULL;
	return (mp);
}

/*
 *  ANSI REJ  (Reject)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE mblk_t *
tcap_priv_enc_rej(ulong *iid, ulong pcode, mblk_t *pseq)
{
	mblk_t *mp = NULL;
	return (mp);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  ITUT TCAP Component (Component (TC) Sub-Layer) encoding.
 *
 *  -------------------------------------------------------------------------
 *
 *  ITUT INKL (Invoke Last)
 *  -------------------------------------------------------------------------
 */
/*
 *  ITUT RRL  (Return Result Last)
 *  -------------------------------------------------------------------------
 */
/*
 *  ITUT RR   (Return Result Not Last)
 *  -------------------------------------------------------------------------
 */
/*
 *  ITUT RER  (Return Error)
 *  -------------------------------------------------------------------------
 */
/*
 *  ITUT REJ  (Reject)
 *  -------------------------------------------------------------------------
 */

/*
 *  =========================================================================
 *
 *  OPTION Handling
 *
 *  =========================================================================
 */

typedef struct tcap_var {
	uchar *ptr;
	size_t len;
} tcap_var_t;

typedef struct tcap_opts {
	ulong flags;			/* success flags */
	tcap_var_t ctx;			/* application context */
	tcap_var_t inf;			/* user information */
	t_uscalar_t *pcl;		/* protocol class */
	t_uscalar_t *ret;		/* return option */
	t_uscalar_t *imp;		/* importance */
	t_uscalar_t *seq;		/* sequence control */
	t_uscalar_t *pri;		/* priority */
	t_uscalar_t *sls;		/* signalling link selection */
	t_uscalar_t *mp;		/* message priority */
} tcap_opts_t;

#define TF_OPT_APP	(1<<0)	/* application context */
#define TF_OPT_INF	(1<<1)	/* user information */
#define TF_OPT_PCL	(1<<2)	/* protocol class */
#define TF_OPT_RET	(1<<3)	/* return option */
#define TF_OPT_IMP	(1<<4)	/* importance */
#define TF_OPT_SEQ	(1<<5)	/* sequence control */
#define TF_OPT_PRI	(1<<6)	/* priority */
#define TF_OPT_SLS	(1<<7)	/* signalling link selection */
#define TF_OPT_MP	(1<<8)	/* message priority */

#define _T_ALIGN_SIZEOF(s) \
	((sizeof((s)) + _T_ALIGN_SIZE - 1) & ~(_T_ALIGN_SIZE - 1))
STATIC size_t
tcap_opts_size(struct tcap *tcap, struct tcap_opts *ops)
{
	size_t len = 0;
	if (ops) {
		const size_t hlen = sizeof(struct t_opthdr);	/* 32 bytes */
		if (ops->ctx.ptr)
			len += hlen + ops->ctx.len;
		if (ops->inf.ptr)
			len += hlen + ops->inf.len;
		if (ops->pcl)
			len += hlen + _T_ALIGN_SIZEOF(*(ops->pcl));
		if (ops->ret)
			len += hlen + _T_ALIGN_SIZEOF(*(ops->ret));
		if (ops->imp)
			len += hlen + _T_ALIGN_SIZEOF(*(ops->imp));
		if (ops->seq)
			len += hlen + _T_ALIGN_SIZEOF(*(ops->seq));
		if (ops->pri)
			len += hlen + _T_ALIGN_SIZEOF(*(ops->pri));
		if (ops->sls)
			len += hlen + _T_ALIGN_SIZEOF(*(ops->sls));
		if (ops->mp)
			len += hlen + _T_ALIGN_SIZEOF(*(ops->mp));
	}
	return (len);
}
STATIC void
tcap_build_opts(struct tcap *tcap, struct tcap_opts *ops, uchar *p)
{
	if (ops) {
		struct t_opthdr *oh;
		const size_t hlen = sizeof(struct t_opthdr);
		if (ops->ctx.ptr) {
			oh = ((typeof(oh)) p)++;
			oh->len = hlen + ops->ctx.len;
			oh->level = T_SS7_TCAP;
			oh->name = T_TCAP_APP_CTX;
			oh->status = (ops->flags & TF_OPT_APP) ? T_SUCCESS : T_FAILURE;
			bcopy(ops->ctx.ptr, p, ops->ctx.len);
			p += _T_ALIGN_SIZEOF(ops->ctx.len);
		}
		if (ops->inf.ptr) {
			oh = ((typeof(oh)) p)++;
			oh->len = hlen + ops->inf.len;
			oh->level = T_SS7_TCAP;
			oh->name = T_TCAP_USER_INFO;
			oh->status = (ops->flags & TF_OPT_INF) ? T_SUCCESS : T_FAILURE;
			bcopy(ops->inf.ptr, p, ops->inf.len);
			p += _T_ALIGN_SIZEOF(ops->inf.len);
		}
		if (ops->pcl) {
			oh = ((typeof(oh)) p)++;
			oh->len = hlen + sizeof(*(ops->pcl));
			oh->level = T_SS7_SCCP;
			oh->name = T_SCCP_PCLASS;
			oh->status = (ops->flags & TF_OPT_PCL) ? T_SUCCESS : T_FAILURE;
			*((typeof(ops->pcl)) p) = *(ops->pcl);
			p += _T_ALIGN_SIZEOF(*ops->pcl);
		}
		if (ops->ret) {
			oh = ((typeof(oh)) p)++;
			oh->len = hlen + sizeof(*(ops->ret));
			oh->level = T_SS7_SCCP;
			oh->name = T_SCCP_RET_ERROR;
			oh->status = (ops->flags & TF_OPT_RET) ? T_SUCCESS : T_FAILURE;
			*((typeof(ops->ret)) p) = *(ops->ret);
			p += _T_ALIGN_SIZEOF(*ops->ret);
		}
		if (ops->imp) {
			oh = ((typeof(oh)) p)++;
			oh->len = hlen + sizeof(*(ops->imp));
			oh->level = T_SS7_SCCP;
			oh->name = T_SCCP_IMPORTANCE;
			oh->status = (ops->flags & TF_OPT_IMP) ? T_SUCCESS : T_FAILURE;
			*((typeof(ops->imp)) p) = *(ops->imp);
			p += _T_ALIGN_SIZEOF(*ops->imp);
		}
		if (ops->seq) {
			oh = ((typeof(oh)) p)++;
			oh->len = hlen + sizeof(*(ops->seq));
			oh->level = T_SS7_SCCP;
			oh->name = T_SCCP_SEQ_CTRL;
			oh->status = (ops->flags & TF_OPT_SEQ) ? T_SUCCESS : T_FAILURE;
			*((typeof(ops->seq)) p) = *(ops->seq);
			p += _T_ALIGN_SIZEOF(*ops->seq);
		}
		if (ops->pri) {
			oh = ((typeof(oh)) p)++;
			oh->len = hlen + sizeof(*(ops->pri));
			oh->level = T_SS7_SCCP;
			oh->name = T_SCCP_PRIORITY;
			oh->status = (ops->flags & TF_OPT_PRI) ? T_SUCCESS : T_FAILURE;
			*((typeof(ops->pri)) p) = *(ops->pri);
			p += _T_ALIGN_SIZEOF(*ops->pri);
		}
		if (ops->sls) {
			oh = ((typeof(oh)) p)++;
			oh->len = hlen + sizeof(*(ops->sls));
			oh->level = T_SS7_MTP;
			oh->name = T_MTP_SLS;
			oh->status = (ops->flags & TF_OPT_SLS) ? T_SUCCESS : T_FAILURE;
			*((typeof(ops->sls)) p) = *(ops->sls);
			p += _T_ALIGN_SIZEOF(*ops->sls);
		}
		if (ops->mp) {
			oh = ((typeof(oh)) p)++;
			oh->len = hlen + sizeof(*(ops->mp));
			oh->level = T_SS7_MTP;
			oh->name = T_MTP_MP;
			oh->status = (ops->flags & TF_OPT_MP) ? T_SUCCESS : T_FAILURE;
			*((typeof(ops->mp)) p) = *(ops->mp);
			p += _T_ALIGN_SIZEOF(*ops->mp);
		}
	}
}
STATIC int
tcap_parse_opts(struct tcap *tcap, struct tcap_opts *ops, uchar *op, size_t len)
{
	struct t_opthdr *oh;
	for (oh = _T_OPT_FIRSTHDR_OFS(op, len, 0); oh; oh = _T_OPT_NEXTHDR_OFS(op, len, oh, 0)) {
		switch (oh->level) {
		case T_SS7_TCAP:
			switch (oh->name) {
			case T_TCAP_APP_CTX:
				ops->ctx.ptr = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->ctx.len = oh->len - sizeof(*oh);
				ops->flags |= TF_OPT_APP;
				continue;
			case T_TCAP_USER_INFO:
				ops->inf.ptr = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->inf.len = oh->len - sizeof(*oh);
				ops->flags |= TF_OPT_INF;
				continue;
			}
			break;
		case T_SS7_SCCP:
			switch (oh->name) {
			case T_SCCP_PCLASS:
				ops->pcl = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->flags |= TF_OPT_PCL;
				continue;
			case T_SCCP_RET_ERROR:
				ops->ret = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->flags |= TF_OPT_RET;
				continue;
			case T_SCCP_IMPORTANCE:
				ops->imp = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->flags |= TF_OPT_IMP;
				continue;
			case T_SCCP_SEQ_CTRL:
				ops->seq = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->flags |= TF_OPT_SEQ;
				continue;
			case T_SCCP_PRIORITY:
				ops->pri = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->flags |= TF_OPT_PRI;
				continue;
			}
			break;
		case T_SS7_MTP:
			switch (oh->name) {
			case T_MTP_SLS:
				ops->sls = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->flags |= TF_OPT_SLS;
				continue;
			case T_MTP_MP:
				ops->mp = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->flags |= TF_OPT_MP;
				continue;
			}
			break;
			break;
		}
	}
	if (oh)
		return (TBADOPT);
	return (0);
}

/*
 *  =========================================================================
 *
 *  OPTIONS handling
 *
 *  =========================================================================
 */
STATIC int
tcap_opt_check(struct tcap *tcap, struct tcap_opts *ops)
{
	if (ops && ops->flags) {
		ops->flags = 0;
		if (ops->ctx.ptr && ops->ctx.len <= TCAP_BUF_MAXLEN)
			ops->flags |= TF_OPT_APP;
		if (ops->inf.ptr && ops->inf.len <= TCAP_BUF_MAXLEN)
			ops->flags |= TF_OPT_INF;
		if (ops->pcl)
			ops->flags |= TF_OPT_PCL;
		if (ops->ret)
			ops->flags |= TF_OPT_RET;
		if (ops->imp)
			ops->flags |= TF_OPT_IMP;
		if (ops->seq)
			ops->flags |= TF_OPT_SEQ;
		if (ops->pri)
			ops->flags |= TF_OPT_PRI;
		if (ops->sls)
			ops->flags |= TF_OPT_SLS;
		if (ops->mp)
			ops->flags |= TF_OPT_MP;
	}
	return (0);
}
STATIC int
tcap_opt_default(struct tcap *tcap, struct tcap_opts *ops)
{
	if (ops) {
		int flags = ops->flags;
		ops->flags = 0;
		if (!flags || ops->ctx.ptr) {
			ops->ctx.ptr = opt_defaults.ctx.buf;
			ops->ctx.len = opt_defaults.ctx.len;
			ops->flags |= TF_OPT_APP;
		}
		if (!flags || ops->inf.ptr) {
			ops->inf.ptr = opt_defaults.inf.buf;
			ops->inf.len = opt_defaults.inf.len;
			ops->flags |= TF_OPT_INF;
		}
		if (!flags || ops->pcl) {
			ops->pcl = &opt_defaults.pcl;
			ops->flags |= TF_OPT_PCL;
		}
		if (!flags || ops->ret) {
			ops->ret = &opt_defaults.ret;
			ops->flags |= TF_OPT_RET;
		}
		if (!flags || ops->imp) {
			ops->imp = &opt_defaults.imp;
			ops->flags |= TF_OPT_IMP;
		}
		if (!flags || ops->seq) {
			ops->seq = &opt_defaults.seq;
			ops->flags |= TF_OPT_SEQ;
		}
		if (!flags || ops->pri) {
			ops->pri = &opt_defaults.pri;
			ops->flags |= TF_OPT_PRI;
		}
		if (!flags || ops->sls) {
			ops->sls = &opt_defaults.sls;
			ops->flags |= TF_OPT_SLS;
		}
		if (!flags || ops->mp) {
			ops->mp = &opt_defaults.mp;
			ops->flags |= TF_OPT_MP;
		}
		return (0);
	}
	swerr();
	return (-EFAULT);
}
STATIC int
tcap_opt_current(struct tcap *tcap, struct tcap_opts *ops)
{
	int flags = ops->flags;
	ops->flags = 0;
	if (!flags || ops->ctx.ptr) {
		ops->ctx.ptr = tcap->options.ctx.buf;
		ops->ctx.len = tcap->options.ctx.len;
		ops->flags |= TF_OPT_APP;
	}
	if (!flags || ops->inf.ptr) {
		ops->inf.ptr = tcap->options.inf.buf;
		ops->inf.len = tcap->options.inf.len;
		ops->flags |= TF_OPT_INF;
	}
	if (!flags || ops->pcl) {
		ops->pcl = &tcap->options.pcl;
		ops->flags |= TF_OPT_PCL;
	}
	if (!flags || ops->ret) {
		ops->ret = &tcap->options.ret;
		ops->flags |= TF_OPT_RET;
	}
	if (!flags || ops->imp) {
		ops->imp = &tcap->options.imp;
		ops->flags |= TF_OPT_IMP;
	}
	if (!flags || ops->seq) {
		ops->seq = &tcap->options.seq;
		ops->flags |= TF_OPT_SEQ;
	}
	if (!flags || ops->pri) {
		ops->pri = &tcap->options.pri;
		ops->flags |= TF_OPT_PRI;
	}
	if (!flags || ops->sls) {
		ops->sls = &tcap->options.sls;
		ops->flags |= TF_OPT_SLS;
	}
	if (!flags || ops->mp) {
		ops->mp = &tcap->options.mp;
		ops->flags |= TF_OPT_MP;
	}
	return (0);
}
STATIC int
tcap_opt_negotiate(struct tcap *tcap, struct tcap_opts *ops)
{
	if (ops->flags) {
		ops->flags = 0;
		if (ops->ctx.ptr && ops->ctx.len <= TCAP_BUF_MAXLEN) {
			bcopy(ops->ctx.ptr, tcap->options.ctx.buf, ops->ctx.len);
			tcap->options.ctx.len = ops->ctx.len;
			ops->ctx.ptr = tcap->options.ctx.buf;
			ops->flags |= TF_OPT_APP;
		}
		if (ops->inf.ptr && ops->inf.len <= TCAP_BUF_MAXLEN) {
			bcopy(ops->inf.ptr, tcap->options.inf.buf, ops->inf.len);
			tcap->options.inf.len = ops->inf.len;
			ops->inf.ptr = tcap->options.inf.buf;
			ops->flags |= TF_OPT_INF;
		}
		if (ops->pcl) {
			tcap->options.pcl = *ops->pcl;
			ops->pcl = &tcap->options.pcl;
			ops->flags |= TF_OPT_PCL;
		}
		if (ops->ret) {
			tcap->options.ret = *ops->ret;
			ops->ret = &tcap->options.ret;
			ops->flags |= TF_OPT_RET;
		}
		if (ops->imp) {
			tcap->options.imp = *ops->imp;
			ops->imp = &tcap->options.imp;
			ops->flags |= TF_OPT_IMP;
		}
		if (ops->seq) {
			tcap->options.seq = *ops->seq;
			ops->seq = &tcap->options.seq;
			ops->flags |= TF_OPT_SEQ;
		}
		if (ops->pri) {
			tcap->options.pri = *ops->pri;
			ops->pri = &tcap->options.pri;
			ops->flags |= TF_OPT_PRI;
		}
		if (ops->sls) {
			tcap->options.sls = *ops->sls;
			ops->sls = &tcap->options.sls;
			ops->flags |= TF_OPT_SLS;
		}
		if (ops->mp) {
			tcap->options.mp = *ops->mp;
			ops->mp = &tcap->options.mp;
			ops->flags |= TF_OPT_MP;
		}
	}
	return (0);
}

/*
 *  =========================================================================
 *
 *  STATE Changes
 *
 *  =========================================================================
 */

#ifdef _DEBUG
STATIC INLINE const char *
tcap_r_state(ulong state)
{
	switch (state) {
	case TRS_UNBND:
		return ("TRS_UNBND");
	case TRS_WACK_BREQ:
		return ("TRS_WACK_BREQ");
	case TRS_WACK_UREQ:
		return ("TRS_WACK_UREQ");
	case TRS_IDLE:
		return ("TRS_IDLE");
	case TRS_WACK_OPTREQ:
		return ("TRS_WACK_OPTREQ");
	case TRS_WACK_RRES:
		return ("TRS_WACK_RRES");
	case TRS_WCON_CREQ:
		return ("TRS_WCON_CREQ");
	case TRS_WRES_CIND:
		return ("TRS_WRES_CIND");
	case TRS_WACK_CRES:
		return ("TRS_WACK_CRES");
	case TRS_DATA_XFER:
		return ("TRS_DATA_XFER");
	case TRS_WCON_RREQ:
		return ("TRS_WCON_RREQ");
	case TRS_WRES_RIND:
		return ("TRS_WRES_RIND");
	case TRS_WACK_DREQ6:
		return ("TRS_WACK_DREQ6");
	case TRS_WACK_DREQ7:
		return ("TRS_WACK_DREQ7");
	case TRS_WACK_DREQ9:
		return ("TRS_WACK_DREQ9");
	case TRS_WACK_DREQ10:
		return ("TRS_WACK_DREQ10");
	case TRS_WACK_DREQ11:
		return ("TRS_WACK_DREQ11");
	default:
		return ("(unknown)");
	}
}
STATIC INLINE const char *
tcap_c_state(ulong state)
{
	switch (state) {
	case TCS_UNBND:
		return ("TCS_UNBND");
	case TCS_WACK_BREQ:
		return ("TCS_WACK_BREQ");
	case TCS_WACK_UREQ:
		return ("TCS_WACK_UREQ");
	case TCS_IDLE:
		return ("TCS_IDLE");
	case TCS_WACK_OPTREQ:
		return ("TCS_WACK_OPTREQ");
	case TCS_WACK_RRES:
		return ("TCS_WACK_RRES");
	case TCS_WCON_CREQ:
		return ("TCS_WCON_CREQ");
	case TCS_WRES_CIND:
		return ("TCS_WRES_CIND");
	case TCS_WACK_CRES:
		return ("TCS_WACK_CRES");
	case TCS_DATA_XFER:
		return ("TCS_DATA_XFER");
	case TCS_WCON_RREQ:
		return ("TCS_WCON_RREQ");
	case TCS_WRES_RIND:
		return ("TCS_WRES_RIND");
	case TCS_WACK_DREQ6:
		return ("TCS_WACK_DREQ6");
	case TCS_WACK_DREQ7:
		return ("TCS_WACK_DREQ7");
	case TCS_WACK_DREQ9:
		return ("TCS_WACK_DREQ9");
	case TCS_WACK_DREQ10:
		return ("TCS_WACK_DREQ10");
	case TCS_WACK_DREQ11:
		return ("TCS_WACK_DREQ11");
	default:
		return ("(unknown)");
	}
}
STATIC INLINE const char *
tcap_t_state(ulong state)
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
	default:
		return ("(unknown)");
	}
}
STATIC INLINE const char *
tcap_m_state(ulong state)
{
	switch (state) {
	default:
		return ("(unknown)");
	}
}
STATIC INLINE const char *
sccp_i_state(ulong state)
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
	default:
		return ("(unknown)");
	}
}
#endif

STATIC INLINE void
tcap_set_state(struct tcap *tcap, ulong newstate)
{
	ulong oldstate = tcap->state;
	(void) oldstate;
	printd(("%s: %p: %s <- %s\n", DRV_NAME, tcap, tcap_state(newstate), tcap_state(oldstate)));
	tcap->state = newstate;
}
STATIC INLINE ulong
tcap_get_state(struct tcap *tcap)
{
	return tcap->state;
}
STATIC INLINE void
tcap_set_r_state(struct tcap *tcap, ulong newstate)
{
	ulong oldstate = tcap->i_state;
	(void) oldstate;
	printd(("%s: %p: %s <- %s\n", DRV_NAME, tcap, tcap_r_state(newstate),
		tcap_r_state(oldstate)));
	tcap->i_state = newstate;
}
STATIC INLINE ulong
tcap_get_r_state(struct tcap *tcap)
{
	return tcap->i_state;
}
STATIC INLINE void
tcap_set_c_state(struct tcap *tcap, ulong newstate)
{
	ulong oldstate = tcap->i_state;
	(void) oldstate;
	printd(("%s: %p: %s <- %s\n", DRV_NAME, tcap, tcap_c_state(newstate),
		tcap_c_state(oldstate)));
	tcap->i_state = newstate;
}
STATIC INLINE ulong
tcap_get_c_state(struct tcap *tcap)
{
	return tcap->i_state;
}
STATIC INLINE void
tcap_set_t_state(struct tcap *tcap, ulong newstate)
{
	ulong oldstate = tcap->i_state;
	(void) oldstate;
	printd(("%s: %p: %s <- %s\n", DRV_NAME, tcap, tcap_t_state(newstate),
		tcap_t_state(oldstate)));
	tcap->i_state = newstate;
}
STATIC INLINE ulong
tcap_get_t_state(struct tcap *tcap)
{
	return tcap->i_state;
}
STATIC INLINE void
tcap_set_m_state(struct tcap *tcap, ulong newstate)
{
	ulong oldstate = tcap->i_state;
	(void) oldstate;
	printd(("%s: %p: %s <- %s\n", DRV_NAME, tcap, tcap_m_state(newstate),
		tcap_m_state(oldstate)));
	tcap->i_state = newstate;
}
STATIC INLINE ulong
tcap_get_m_state(struct tcap *tcap)
{
	return tcap->i_state;
}

STATIC INLINE void
sccp_set_i_state(struct sccp *sccp, ulong newstate)
{
	ulong oldstate = sccp->i_state;
	(void) oldstate;
	printd(("%s: %p: %s <- %s\n", DRV_NAME, sccp, sccp_i_state(newstate),
		sccp_i_state(oldstate)));
	sccp->i_state = newstate;
}
STATIC INLINE ulong
sccp_get_i_state(struct sccp *sccp)
{
	return sccp->i_state;
}

/*
 *  TLI interface state flags
 */
#define TSF_UNBND	( 1 << TS_UNBND		)
#define TSF_WACK_BREQ	( 1 << TS_WACK_BREQ	)
#define TSF_WACK_UREQ	( 1 << TS_WACK_UREQ	)
#define TSF_IDLE	( 1 << TS_IDLE		)
#define TSF_WACK_OPTREQ	( 1 << TS_WACK_OPTREQ	)
#define TSF_WACK_CREQ	( 1 << TS_WACK_CREQ	)
#define TSF_WCON_CREQ	( 1 << TS_WCON_CREQ	)
#define TSF_WRES_CIND	( 1 << TS_WRES_CIND	)
#define TSF_WACK_CRES	( 1 << TS_WACK_CRES	)
#define TSF_DATA_XFER	( 1 << TS_DATA_XFER	)
#define TSF_WIND_ORDREL	( 1 << TS_WIND_ORDREL	)
#define TSF_WREQ_ORDREL	( 1 << TS_WREQ_ORDREL	)
#define TSF_WACK_DREQ6	( 1 << TS_WACK_DREQ6	)
#define TSF_WACK_DREQ7	( 1 << TS_WACK_DREQ7	)
#define TSF_WACK_DREQ9	( 1 << TS_WACK_DREQ9	)
#define TSF_WACK_DREQ10	( 1 << TS_WACK_DREQ10	)
#define TSF_WACK_DREQ11	( 1 << TS_WACK_DREQ11	)
#define TSF_NOSTATES	( 1 << TS_NOSTATES	)

#define TSF_WACK_DREQ	(TSF_WACK_DREQ6 \
			|TSF_WACK_DREQ7 \
			|TSF_WACK_DREQ9 \
			|TSF_WACK_DREQ10 \
			|TSF_WACK_DREQ11)

/* 
 *  =========================================================================
 *
 *  PRIMITIVES
 *
 *  =========================================================================
 */
/* 
 *  -------------------------------------------------------------------------
 *
 *  Primitives sent upstream
 *
 *  -------------------------------------------------------------------------
 */

/* 
 *  M_FLUSH
 *  -----------------------------------
 */
STATIC int
m_flush(queue_t *q, queue_t *pq, int band, int flags, int what)
{
	mblk_t *mp;
	if (!(mp = ss7_allocb(q, 2, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_FLUSH;
	*(mp->b_wptr)++ = flags | (band ? FLUSHBAND : 0);
	*(mp->b_wptr)++ = band;
	printd(("%s: %p: <- M_FLUSH\n", DRV_NAME, pq));
	putq(pq, mp);
	return (QR_DONE);
      enobufs:
	rare();
	return (-ENOBUFS);
}

/* 
 *  M_ERROR
 *  -----------------------------------
 */
STATIC int
m_error(queue_t *q, struct tcap *tcap, int error)
{
	mblk_t *mp;
	int hangup = 0;
	if (error < 0)
		error = -error;
	switch (error) {
	case EBUSY:
	case ENOBUFS:
	case ENOMEM:
	case EAGAIN:
		return (-error);
	case EPIPE:
	case ENETDOWN:
	case EHOSTUNREACH:
		hangup = 1;
	}
	if (!(mp = ss7_allocb(q, 2, BPRI_MED)))
		goto enobufs;
	if (hangup) {
		mp->b_datap->db_type = M_HANGUP;
		tcap_set_state(tcap, NS_NOSTATES);
		printd(("%s: %p: <- M_HANGUP\n", DRV_NAME, tcap));
		ss7_oput(tcap->oq, mp);
		return (-error);
	} else {
		mp->b_datap->db_type = M_ERROR;
		*(mp->b_wptr)++ = error < 0 ? -error : error;
		*(mp->b_rptr)++ = error < 0 ? -error : error;
		tcap_set_state(tcap, NS_NOSTATES);
		printd(("%s: %p: <- M_ERROR\n", DRV_NAME, tcap));
		ss7_oput(tcap->oq, mp);
		return (QR_DONE);
	}
      enobufs:
	rare();
	return (-ENOBUFS);
}

/* 
 *  TCI Primitives
 *  -------------------------------------------------------------------------
 */
/*
 *  TC_INFO_ACK
 *  -----------------------------------------------------------------
 */
STATIC INLINE int
tc_info_ack(queue_t *q, struct tcap *tcap)
{
	int err;
	mblk_t *mp;
	struct TC_info_ack *p;
	size_t msg_len = sizeof(*p);
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = TC_INFO_ACK;
	p->TSDU_size = FIXME;
	p->ETSDU_size = FIXME;
	p->CDATA_size = FIXME;
	p->DDATA_size = FIXME;
	p->ADDR_size = sizeof(struct sccp_addr) + SCCP_MAX_ADDR_LENGTH;
	p->OPT_size = FIXME;
	p->TIDU_size = FIXME;
	p->SERV_type = FIXME;
	p->CURRENT_state = tcap_get_c_state(tcap);
	p->PROVIDER_flag = FIXME;
	p->TRPI_version = FIXME;
	printd(("%s: %p: <- TC_INFO_ACK\n", DRV_NAME, tcap));
	ss7_oput(tcap->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TC_BIND_ACK
 *  -----------------------------------------------------------------
 */
STATIC INLINE int
tc_bind_ack(queue_t *q, struct tcap *tcap, struct sccp_addr *add, ulong xact, ulong tok)
{
	int err;
	mblk_t *mp;
	struct TC_bind_ack *p;
	size_t add_len = add ? sizeof(*add) + add->alen : 0;
	size_t msg_len = sizeof(*p) + add_len;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = TC_BIND_ACK;
	p->ADDR_length = add_len;
	p->ADDR_offset = add_len ? sizeof(*p) : 0;
	p->XACT_number = xact;
	p->TOKEN_value = tok;
	if (add_len) {
		bcopy(add, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
	}
	printd(("%s: %p: <- TC_BIND_ACK\n", DRV_NAME, tcap));
	ss7_oput(tcap->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TC_SUBS_BIND_ACK
 *  -----------------------------------------------------------------
 */
STATIC INLINE int
tc_subs_bind_ack(queue_t *q, struct tcap *tcap)
{
	int err;
	mblk_t *mp;
	struct TC_subs_bind_ack *p;
	size_t msg_len = sizeof(*p);
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = TC_SUBS_BIND_ACK;
	printd(("%s: %p: <- TC_SUBS_BIND_ACK\n", DRV_NAME, tcap));
	ss7_oput(tcap->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TC_OK_ACK
 *  -----------------------------------------------------------------
 */
STATIC INLINE int
tc_ok_ack(queue_t *q, struct tcap *tcap, ulong prim)
{
	int err;
	mblk_t *mp;
	struct TC_ok_ack *p;
	size_t msg_len = sizeof(*p);
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = TC_OK_ACK;
	p->CORRECT_prim = prim;
	printd(("%s: %p: <- TC_OK_ACK\n", DRV_NAME, tcap));
	ss7_oput(tcap->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TC_ERROR_ACK
 *  -----------------------------------------------------------------
 */
STATIC INLINE int
tc_error_ack(queue_t *q, struct tcap *tcap, ulong prim, long etype, ulong did, ulong iid)
{
	int err;
	mblk_t *mp;
	struct TC_error_ack *p;
	size_t msg_len = sizeof(*p);
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = TC_ERROR_ACK;
	p->ERROR_prim = prim;
	p->TRPI_error = etype < 0 ? TCSYSERR : etype;
	p->UNIX_error = etype < 0 ? -etype : 0;
	p->DIALOG_id = did;
	p->INVOKE_id = iid;
	printd(("%s: %p: <- TC_ERROR_ACK\n", DRV_NAME, tcap));
	ss7_oput(tcap->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TC_OPTMGMT_ACK
 *  -----------------------------------------------------------------
 */
STATIC INLINE int
tc_optmgmt_ack(queue_t *q, struct tcap *tcap, struct tcap_opts *opt, ulong flags)
{
	int err;
	mblk_t *mp;
	struct TC_optmgmt_ack *p;
	size_t opt_len = tcap_opts_size(tcap, opt);
	size_t msg_len = sizeof(*p) + opt_len;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = TC_OPTMGMT_ACK;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) : 0;
	p->MGMT_flags = flags;
	if (opt_len) {
		tcap_build_opts(tcap, opt, mp->b_wptr);
		mp->b_wptr += opt_len;
	}
	printd(("%s: %p: <- TC_OPTMGMT_ACK\n", DRV_NAME, tcap));
	ss7_oput(tcap->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TC_UNI_IND
 *  -----------------------------------------------------------------
 */
STATIC INLINE int
tc_uni_ind(queue_t *q, struct tcap *tcap, struct sccp_addr *src, struct sccp_addr *dst,
	   struct tcap_opts *opt, ulong did, ulong flags, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	struct TC_uni_ind *p;
	size_t src_len = src ? sizeof(*src) + src->alen : 0;
	size_t dst_len = dst ? sizeof(*dst) + dst->alen : 0;
	size_t opt_len = tcap_opts_size(tcap, opt);
	size_t msg_len = sizeof(*p) + src_len + dst_len + opt_len;
	if (!canput(tcap->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = TC_UNI_IND;
	p->SRC_length = src_len;
	p->SRC_offset = src_len ? sizeof(*p) : 0;
	p->DEST_length = dst_len;
	p->DEST_offset = dst_len ? sizeof(*p) + src_len : 0;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) + src_len + dst_len : 0;
	p->DIALOG_id = did;
	p->COMP_flags = flags;
	if (src_len) {
		bcopy(src, mp->b_wptr, src_len);
		mp->b_wptr += src_len;
	}
	if (dst_len) {
		bcopy(dst, mp->b_wptr, dst_len);
		mp->b_wptr += dst_len;
	}
	if (opt_len) {
		tcap_build_opts(tcap, opt, mp->b_wptr);
		mp->b_wptr += opt_len;
	}
	mp->b_cont = dp;
	printd(("%s: %p: <- TC_UNI_IND\n", DRV_NAME, tcap));
	ss7_oput(tcap->oq, mp);
	return (QR_ABSORBED);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TC_BEGIN_IND
 *  -----------------------------------------------------------------
 */
STATIC INLINE int
tc_begin_ind(queue_t *q, struct tcap *tcap, struct sccp_addr *src, struct sccp_addr *dst,
	     struct tcap_opts *opt, ulong did, ulong flags, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	struct TC_begin_ind *p;
	size_t src_len = src ? sizeof(*src) + src->alen : 0;
	size_t dst_len = dst ? sizeof(*dst) + dst->alen : 0;
	size_t opt_len = tcap_opts_size(tcap, opt);
	size_t msg_len = sizeof(*p) + src_len + dst_len + opt_len;
	if (!canput(tcap->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = TC_BEGIN_IND;
	p->SRC_length = src_len;
	p->SRC_offset = src_len ? sizeof(*p) : 0;
	p->DEST_length = dst_len;
	p->DEST_offset = dst_len ? sizeof(*p) + src_len : 0;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) + src_len + dst_len : 0;
	p->DIALOG_id = did;
	p->COMP_flags = flags;
	if (src_len) {
		bcopy(src, mp->b_wptr, src_len);
		mp->b_wptr += src_len;
	}
	if (dst_len) {
		bcopy(dst, mp->b_wptr, dst_len);
		mp->b_wptr += dst_len;
	}
	if (opt_len) {
		tcap_build_opts(tcap, opt, mp->b_wptr);
		mp->b_wptr += opt_len;
	}
	mp->b_cont = dp;
	printd(("%s: %p: <- TC_BEGIN_IND\n", DRV_NAME, tcap));
	ss7_oput(tcap->oq, mp);
	return (QR_ABSORBED);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TC_BEGIN_CON
 *  -----------------------------------------------------------------
 */
STATIC INLINE int
tc_begin_con(queue_t *q, struct tcap *tcap, struct tcap_opts *opt, ulong did, ulong flags,
	     mblk_t *dp)
{
	int err;
	mblk_t *mp;
	struct TC_begin_con *p;
	size_t opt_len = tcap_opts_size(tcap, opt);
	size_t msg_len = sizeof(*p) + opt_len;
	if (!canput(tcap->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = TC_BEGIN_CON;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) : 0;
	p->DIALOG_id = did;
	p->COMP_flags = flags;
	if (opt_len) {
		tcap_build_opts(tcap, opt, mp->b_wptr);
		mp->b_wptr += opt_len;
	}
	mp->b_cont = dp;
	printd(("%s: %p: <- TC_BEGIN_CON\n", DRV_NAME, tcap));
	ss7_oput(tcap->oq, mp);
	return (QR_ABSORBED);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TC_CONT_IND
 *  -----------------------------------------------------------------
 */
STATIC INLINE int
tc_cont_ind(queue_t *q, struct tcap *tcap, struct tcap_opts *opt, ulong did, ulong flags,
	    mblk_t *dp)
{
	int err;
	mblk_t *mp;
	struct TC_cont_ind *p;
	size_t opt_len = tcap_opts_size(tcap, opt);
	size_t msg_len = sizeof(*p) + opt_len;
	if (!canput(tcap->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = TC_CONT_IND;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) : 0;
	p->DIALOG_id = did;
	p->COMP_flags = flags;
	if (opt_len) {
		tcap_build_opts(tcap, opt, mp->b_wptr);
		mp->b_wptr += opt_len;
	}
	mp->b_cont = dp;
	printd(("%s: %p: <- TC_CONT_IND\n", DRV_NAME, tcap));
	ss7_oput(tcap->oq, mp);
	return (QR_ABSORBED);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TC_END_IND
 *  -----------------------------------------------------------------
 */
STATIC INLINE int
tc_end_ind(queue_t *q, struct tcap *tcap, struct tcap_opts *opt, ulong did, ulong flags, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	struct TC_end_ind *p;
	size_t opt_len = tcap_opts_size(tcap, opt);
	size_t msg_len = sizeof(*p) + opt_len;
	if (!canput(tcap->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = TC_END_IND;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) : 0;
	p->DIALOG_id = did;
	p->COMP_flags = flags;
	if (opt_len) {
		tcap_build_opts(tcap, opt, mp->b_wptr);
		mp->b_wptr += opt_len;
	}
	mp->b_cont = dp;
	printd(("%s: %p: <- TC_END_IND\n", DRV_NAME, tcap));
	ss7_oput(tcap->oq, mp);
	return (QR_ABSORBED);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TC_ABORT_IND
 *  -----------------------------------------------------------------
 */
STATIC INLINE int
tc_abort_ind(queue_t *q, struct tcap *tcap, struct tcap_opts *opt, ulong did, ulong reason,
	     ulong orig)
{
	int err;
	mblk_t *mp;
	struct TC_abort_ind *p;
	size_t opt_len = tcap_opts_size(tcap, opt);
	size_t msg_len = sizeof(*p) + opt_len;
	if (!canput(tcap->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = TC_ABORT_IND;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) : 0;
	p->DIALOG_id = did;
	p->ABORT_reason = reason;
	p->ORIGINATOR = orig;
	if (opt_len) {
		tcap_build_opts(tcap, opt, mp->b_wptr);
		mp->b_wptr += opt_len;
	}
	printd(("%s: %p: <- TC_ABORT_IND\n", DRV_NAME, tcap));
	ss7_oput(tcap->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TC_NOTICE_IND
 *  -----------------------------------------------------------------
 */
STATIC INLINE int
tc_notice_ind(queue_t *q, struct tcap *tcap, ulong did, ulong cause)
{
	int err;
	mblk_t *mp;
	struct TC_notice_ind *p;
	size_t msg_len = sizeof(*p);
	if (!canput(tcap->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = TC_NOTICE_IND;
	p->DIALOG_id = did;
	p->REPORT_cause = cause;
	printd(("%s: %p: <- TC_NOTICE_IND\n", DRV_NAME, tcap));
	ss7_oput(tcap->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TC_INVOKE_IND
 *  -----------------------------------------------------------------
 */
STATIC INLINE int
tc_invoke_ind(queue_t *q, struct tcap *tcap, ulong did, ulong ocls, ulong iid, ulong lid,
	      ulong oper, ulong more)
{
	int err;
	mblk_t *mp;
	struct TC_invoke_ind *p;
	size_t msg_len = sizeof(*p);
	if (!canput(tcap->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = TC_INVOKE_IND;
	p->DIALOG_id = did;
	p->OP_class = ocls;
	p->INVOKE_id = iid;
	p->LINKED_id = lid;
	p->OPERATION = oper;
	p->MORE_flag = more;
	printd(("%s: %p: <- TC_INVOKE_IND\n", DRV_NAME, tcap));
	ss7_oput(tcap->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TC_RESULT_IND
 *  -----------------------------------------------------------------
 */
STATIC INLINE int
tc_result_ind(queue_t *q, struct tcap *tcap, ulong did, ulong iid, ulong oper, ulong more)
{
	int err;
	mblk_t *mp;
	struct TC_result_ind *p;
	size_t msg_len = sizeof(*p);
	if (!canput(tcap->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = TC_RESULT_IND;
	p->DIALOG_id = did;
	p->INVOKE_id = iid;
	p->OPERATION = oper;
	p->MORE_flag = more;
	printd(("%s: %p: <- TC_RESULT_IND\n", DRV_NAME, tcap));
	ss7_oput(tcap->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TC_ERROR_IND
 *  -----------------------------------------------------------------
 */
STATIC INLINE int
tc_error_ind(queue_t *q, struct tcap *tcap, ulong did, ulong iid, ulong ecode)
{
	int err;
	mblk_t *mp;
	struct TC_error_ind *p;
	size_t msg_len = sizeof(*p);
	if (!canput(tcap->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = TC_ERROR_IND;
	p->DIALOG_id = did;
	p->INVOKE_id = iid;
	p->ERROR_code = ecode;
	printd(("%s: %p: <- TC_ERROR_IND\n", DRV_NAME, tcap));
	ss7_oput(tcap->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TC_CANCEL_IND
 *  -----------------------------------------------------------------
 */
STATIC INLINE int
tc_cancel_ind(queue_t *q, struct tcap *tcap, ulong did, ulong iid)
{
	int err;
	mblk_t *mp;
	struct TC_cancel_ind *p;
	size_t msg_len = sizeof(*p);
	if (!canput(tcap->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = TC_CANCEL_IND;
	p->DIALOG_id = did;
	p->INVOKE_id = iid;
	printd(("%s: %p: <- TC_CANCEL_IND\n", DRV_NAME, tcap));
	ss7_oput(tcap->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TC_REJECT_IND
 *  -----------------------------------------------------------------
 */
STATIC INLINE int
tc_reject_ind(queue_t *q, struct tcap *tcap, ulong did, ulong iid, ulong ecode)
{
	int err;
	mblk_t *mp;
	struct TC_reject_ind *p;
	size_t msg_len = sizeof(*p);
	if (!canput(tcap->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = TC_REJECT_IND;
	p->DIALOG_id = did;
	p->INVOKE_id = iid;
	p->PROBLEM_code = ecode;
	printd(("%s: %p: <- TC_REJECT_IND\n", DRV_NAME, tcap));
	ss7_oput(tcap->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/* 
 *  TRI Primitives
 *  -------------------------------------------------------------------------
 */
/*
 *  TR_INFO_ACK
 *  -----------------------------------------------------------------
 */
STATIC int
tr_info_ack(queue_t *q, struct tcap *tcap)
{
	int err;
	mblk_t *mp;
	struct TR_info_ack *p;
	size_t msg_len = sizeof(*p);
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = TR_INFO_ACK;
	p->TSDU_size = FIXME;
	p->ETSDU_size = FIXME;
	p->CDATA_size = FIXME;
	p->DDATA_size = FIXME;
	p->ADDR_size = sizeof(struct sccp_addr) + SCCP_MAX_ADDR_LENGTH;
	p->OPT_size = FIXME;
	p->TIDU_size = FIXME;
	p->SERV_type = FIXME;
	p->CURRENT_state = tcap_get_c_state(tcap);
	p->PROVIDER_flag = FIXME;
	p->TRPI_version = FIXME;
	printd(("%s: %p: <- TR_INFO_ACK\n", DRV_NAME, tcap));
	ss7_oput(tcap->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TR_BIND_ACK
 *  -----------------------------------------------------------------
 */
STATIC int
tr_bind_ack(queue_t *q, struct tcap *tcap, struct sccp_addr *add, ulong xact, ulong tok)
{
	int err;
	mblk_t *mp;
	struct TR_bind_ack *p;
	size_t add_len = add ? sizeof(*add) + add->alen : 0;
	size_t msg_len = sizeof(*p) + add_len;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = TR_BIND_ACK;
	p->ADDR_length = add_len;
	p->ADDR_offset = add_len ? sizeof(*p) : 0;
	p->XACT_number = xact;
	p->TOKEN_value = tok;
//      p->BIND_flags = flags;
	if (add_len) {
		bcopy(add, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
	}
	printd(("%s: %p: <- TR_BIND_ACK\n", DRV_NAME, tcap));
	ss7_oput(tcap->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TR_OK_ACK
 *  -----------------------------------------------------------------
 */
STATIC int
tr_ok_ack(queue_t *q, struct tcap *tcap, ulong prim)
{
	int err;
	mblk_t *mp;
	struct TR_ok_ack *p;
	size_t msg_len = sizeof(*p);
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = TR_OK_ACK;
	p->CORRECT_prim = prim;
	printd(("%s: %p: <- TR_OK_ACK\n", DRV_NAME, tcap));
	ss7_oput(tcap->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TR_ERROR_ACK
 *  -----------------------------------------------------------------
 */
STATIC int
tr_error_ack(queue_t *q, struct tcap *tcap, ulong prim, long etype, ulong tid)
{
	int err;
	mblk_t *mp;
	struct TR_error_ack *p;
	size_t msg_len = sizeof(*p);
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = TR_ERROR_ACK;
	p->ERROR_prim = prim;
	p->TRPI_error = etype < 0 ? TRSYSERR : etype;
	p->UNIX_error = etype < 0 ? -etype : 0;
	p->TRANS_id = tid;
	printd(("%s: %p: <- TR_ERROR_ACK\n", DRV_NAME, tcap));
	ss7_oput(tcap->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TR_OPTMGMT_ACK
 *  -----------------------------------------------------------------
 */
STATIC int
tr_optmgmt_ack(queue_t *q, struct tcap *tcap, struct tcap_opts *opt, ulong flags)
{
	int err;
	mblk_t *mp;
	struct TR_optmgmt_ack *p;
	size_t opt_len = tcap_opts_size(tcap, opt);
	size_t msg_len = sizeof(*p) + opt_len;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = TR_OPTMGMT_ACK;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) : 0;
	p->MGMT_flags = flags;
	if (opt_len) {
		tcap_build_opts(tcap, opt, mp->b_wptr);
		mp->b_wptr += opt_len;
	}
	printd(("%s: %p: <- TR_OPTMGMT_ACK\n", DRV_NAME, tcap));
	ss7_oput(tcap->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TR_UNI_IND
 *  -----------------------------------------------------------------
 */
STATIC int
tr_uni_ind(queue_t *q, struct tcap *tcap, struct sccp_addr *org, struct sccp_addr *dst,
	   struct tcap_opts *opt, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	struct TR_uni_ind *p;
	size_t dst_len = dst ? sizeof(*dst) + dst->alen : 0;
	size_t org_len = org ? sizeof(*org) + org->alen : 0;
	size_t opt_len = tcap_opts_size(tcap, opt);
	size_t msg_len = sizeof(*p) + dst_len + org_len + opt_len;
	if (!canput(tcap->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = TR_UNI_IND;
	p->DEST_length = dst_len;
	p->DEST_offset = dst_len ? sizeof(*p) : 0;
	p->ORIG_length = org_len;
	p->ORIG_offset = org_len ? sizeof(*p) + dst_len : 0;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) + dst_len + org_len : 0;
	if (dst_len) {
		bcopy(dst, mp->b_wptr, dst_len);
		mp->b_wptr += dst_len;
	}
	if (org_len) {
		bcopy(org, mp->b_wptr, org_len);
		mp->b_wptr += org_len;
	}
	if (opt_len) {
		tcap_build_opts(tcap, opt, mp->b_wptr);
		mp->b_wptr += opt_len;
	}
	mp->b_cont = dp;
	printd(("%s: %p: <- TR_UNI_IND\n", DRV_NAME, tcap));
	ss7_oput(tcap->oq, mp);
	return (QR_ABSORBED);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TR_BEGIN_IND
 *  -----------------------------------------------------------------
 */
STATIC int
tr_begin_ind(queue_t *q, struct tcap *tcap, struct tr *tr, struct sccp_addr *org,
	     struct sccp_addr *dst, struct tcap_opts *opt, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	struct TR_begin_ind *p;
	size_t dst_len = dst ? sizeof(*dst) + dst->alen : 0;
	size_t org_len = org ? sizeof(*org) + org->alen : 0;
	size_t opt_len = tcap_opts_size(tcap, opt);
	size_t msg_len = sizeof(*p) + dst_len + org_len + opt_len;
	if (!canput(tcap->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = TR_BEGIN_IND;
	p->TRANS_id = tr->tid;
	p->DEST_length = dst_len;
	p->DEST_offset = dst_len ? sizeof(*p) : 0;
	p->ORIG_length = org_len;
	p->ORIG_offset = org_len ? sizeof(*p) + dst_len : 0;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) + dst_len + org_len : 0;
	if (dst_len) {
		bcopy(dst, mp->b_wptr, dst_len);
		mp->b_wptr += dst_len;
	}
	if (org_len) {
		bcopy(org, mp->b_wptr, org_len);
		mp->b_wptr += org_len;
	}
	if (opt_len) {
		tcap_build_opts(tcap, opt, mp->b_wptr);
		mp->b_wptr += opt_len;
	}
	mp->b_cont = dp;
	printd(("%s: %p: <- TR_BEGIN_IND\n", DRV_NAME, tcap));
	ss7_oput(tcap->oq, mp);
	return (QR_ABSORBED);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TR_BEGIN_CON
 *  -----------------------------------------------------------------
 */
STATIC int
tr_begin_con(queue_t *q, struct tcap *tcap, struct tr *tr, struct sccp_addr *org,
	     struct tcap_opts *opt, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	struct TR_begin_con *p;
	size_t org_len = org ? sizeof(*org) + org->alen : 0;
	size_t opt_len = tcap_opts_size(tcap, opt);
	size_t msg_len = sizeof(*p) + org_len + opt_len;
	if (!canput(tcap->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = TR_BEGIN_CON;
	p->CORR_id = tr->cid;
	p->TRANS_id = tr->tid;
	p->ORIG_length = org_len;
	p->ORIG_offset = org_len ? sizeof(*p) : 0;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) + org_len : 0;
	if (org_len) {
		bcopy(org, mp->b_wptr, org_len);
		mp->b_wptr += org_len;
	}
	if (opt_len) {
		tcap_build_opts(tcap, opt, mp->b_wptr);
		mp->b_wptr += opt_len;
	}
	mp->b_cont = dp;
	printd(("%s: %p: <- TR_BEGIN_CON\n", DRV_NAME, tcap));
	ss7_oput(tcap->oq, mp);
	return (QR_ABSORBED);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TR_CONT_IND
 *  -----------------------------------------------------------------
 */
STATIC int
tr_cont_ind(queue_t *q, struct tcap *tcap, struct tr *tr, struct tcap_opts *opt, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	struct TR_cont_ind *p;
	size_t opt_len = tcap_opts_size(tcap, opt);
	size_t msg_len = sizeof(*p) + opt_len;
	if (!canput(tcap->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = TR_CONT_IND;
	p->TRANS_id = tr->tid;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) : 0;
	if (opt_len) {
		tcap_build_opts(tcap, opt, mp->b_wptr);
		mp->b_wptr += opt_len;
	}
	mp->b_cont = dp;
	printd(("%s: %p: <- TR_CONT_IND\n", DRV_NAME, tcap));
	ss7_oput(tcap->oq, mp);
	return (QR_ABSORBED);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TR_END_IND
 *  -----------------------------------------------------------------
 */
STATIC int
tr_end_ind(queue_t *q, struct tcap *tcap, struct tr *tr, struct tcap_opts *opt, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	struct TR_end_ind *p;
	size_t opt_len = tcap_opts_size(tcap, opt);
	size_t msg_len = sizeof(*p) + opt_len;
	if (!canput(tcap->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = TR_END_IND;
	p->CORR_id = tr->cid;
	p->TRANS_id = tr->tid;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) : 0;
	if (opt_len) {
		tcap_build_opts(tcap, opt, mp->b_wptr);
		mp->b_wptr += opt_len;
	}
	mp->b_cont = dp;
	printd(("%s: %p: <- TR_END_IND\n", DRV_NAME, tcap));
	ss7_oput(tcap->oq, mp);
	return (QR_ABSORBED);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TR_ABORT_IND
 *  -----------------------------------------------------------------
 */
STATIC int
tr_abort_ind(queue_t *q, struct tcap *tcap, struct tr *tr, ulong cause, ulong orig,
	     struct tcap_opts *opt)
{
	int err;
	mblk_t *mp;
	struct TR_abort_ind *p;
	size_t opt_len = tcap_opts_size(tcap, opt);
	size_t msg_len = sizeof(*p) + opt_len;
	if (!canput(tcap->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = TR_ABORT_IND;
	p->ABORT_cause = cause;
	p->ORIGINATOR = orig;
	p->CORR_id = tr->cid;
	p->TRANS_id = tr->tid;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) : 0;
	if (opt_len) {
		tcap_build_opts(tcap, opt, mp->b_wptr);
		mp->b_wptr += opt_len;
	}
	printd(("%s: %p: <- TR_ABORT_IND\n", DRV_NAME, tcap));
	ss7_oput(tcap->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  TR_NOTICE_IND
 *  -----------------------------------------------------------------
 */
STATIC int
tr_notice_ind(queue_t *q, struct tcap *tcap, ulong cause, struct tr *tr)
{
	int err;
	mblk_t *mp;
	struct TR_notice_ind *p;
	size_t msg_len = sizeof(*p);
	if (!canput(tcap->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = TR_NOTICE_IND;
	p->REPORT_cause = cause;
	p->CORR_id = tr->cid;
	p->TRANS_id = tr->tid;
	printd(("%s: %p: <- TR_NOTICE_IND\n", DRV_NAME, tcap));
	ss7_oput(tcap->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/* 
 *  TPI Primitives
 *  -------------------------------------------------------------------------
 */
#ifndef PAD4
#define PAD4(__x) (((__x)+3)&~(3))
#endif
/* 
 *  T_CONN_IND          11 - connection indication
 *  -----------------------------------------------------------------
 *  We get the information from the CR message in the connection indication.  We queue the CR message (complete with
 *  decode parameter block) itself as the connection indication.  The sequence number is really just a pointer to
 *  the first mblk_t in the received CR message.
 */
STATIC INLINE int
t_conn_ind(queue_t *q, struct tcap *tcap, mblk_t *cp)
{
	mblk_t *mp;
	struct tr_msg *m = (typeof(m)) cp->b_rptr;
	struct T_conn_ind *p;
	struct t_opthdr *oh;
	struct sccp_addr *src = &m->sccp.orig;
	size_t src_len = sizeof(*src) + src->alen;
	size_t opt_len = sizeof(*oh) + sizeof(t_scalar_t);
	size_t msg_len = sizeof(*p) + PAD4(src_len) + opt_len;
	if ((1 << tcap_get_t_state(tcap)) & ~(TSF_IDLE | TSF_WRES_CIND))
		goto efault;
	if (bufq_length(&tcap->conq) >= tcap->conind)
		goto erestart;
	if (!canputnext(tcap->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = T_CONN_IND;
	p->SRC_length = src_len;
	p->SRC_offset = src_len ? sizeof(*p) : 0;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) + PAD4(src_len) : 0;
	p->SEQ_number = (ulong) cp;
	if (src_len) {
		bcopy(src, mp->b_wptr, src_len);
		mp->b_wptr += PAD4(src_len);
	}
	oh = ((typeof(oh)) mp->b_wptr)++;
	oh->len = opt_len;
	oh->level = T_SS7_SCCP;
	oh->name = T_SCCP_PCLASS;
	oh->status = T_SUCCESS;
	*((t_uscalar_t *) mp->b_wptr)++ = m->sccp.pcl;
	bufq_queue(&tcap->conq, cp);
	tcap_set_t_state(tcap, TS_WRES_CIND);
	putnext(tcap->oq, mp);
	return (0);
      enobufs:
	rare();
	return (-ENOBUFS);
      ebusy:
	rare();
	return (-EBUSY);
      erestart:
	ptrace(("%s: %p: PROTO: too many connection indications\n", DRV_NAME, tcap));
	return (-ERESTART);
      efault:
	pswerr(("%s: %p: SWERR: unexpected indication for state %ld\n", DRV_NAME, tcap,
		tcap_get_t_state(tcap)));
	return (-EFAULT);
}

/* 
 *  T_CONN_CON          12 - connection confirmation
 *  -----------------------------------------------------------------
 *  The only options with end-to-end significance that are negotiated is the protocol class.
 */
STATIC INLINE int
t_conn_con(queue_t *q, struct tcap *tcap, struct sccp_addr *res, ulong pcls, ulong flags,
	   mblk_t *dp)
{
	int err;
	mblk_t *mp;
	struct T_conn_con *p;
	struct t_opthdr *oh;
	size_t res_len = res ? sizeof(*res) + res->alen : 0;
	size_t opt_len = sizeof(*oh) + sizeof(t_scalar_t);
	size_t msg_len = sizeof(*p) + PAD4(res_len) + opt_len;
	if (tcap_get_t_state(tcap) != TS_WCON_CREQ)
		goto efault;
	if (!canputnext(tcap->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	mp->b_band = 1;		/* expedite */
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = T_CONN_CON;
	p->RES_length = res_len;
	p->RES_offset = res_len ? sizeof(*p) : 0;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) + PAD4(res_len) : 0;
	if (res_len) {
		bcopy(res, mp->b_wptr, res_len);
		mp->b_wptr += PAD4(res_len);
	}
	oh = ((typeof(oh)) mp->b_wptr)++;
	oh->len = opt_len;
	oh->level = T_SS7_SCCP;
	oh->name = T_SCCP_PCLASS;
	oh->status = T_SUCCESS;
	*((t_uscalar_t *) mp->b_wptr)++ = pcls;
	tcap_set_t_state(tcap, TS_DATA_XFER);
	putnext(tcap->oq, mp);
	return (0);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: no buffers\n", DRV_NAME, tcap));
	goto error;
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: flow controlled\n", DRV_NAME, tcap));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: SWERR: unexpected indication for state %ld\n", DRV_NAME, tcap,
		tcap_get_t_state(tcap)));
	goto error;
      error:
	return (err);
}

/* 
 *  T_DISCON_IND        13 - disconnect indication
 *  -----------------------------------------------------------------
 *  We use the address of the mblk_t that contains the CR message as a SEQ_number for connection indications that
 *  are rejected with a disconnect indication as well.  We can use this to directly address the mblk in the
 *  connection indication bufq.
 */
STATIC INLINE int
t_discon_ind(queue_t *q, struct tcap *tcap, long reason, mblk_t *seq, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	struct T_discon_ind *p;
	size_t msg_len = sizeof(*p);
	if ((1 << tcap->
	     state) & ~(TSF_WCON_CREQ | TSF_WRES_CIND | TSF_DATA_XFER | TSF_WIND_ORDREL |
			TSF_WREQ_ORDREL))
		goto efault;
	if (!canputnext(tcap->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = T_DISCON_IND;
	p->DISCON_reason = reason;
	p->SEQ_number = (ulong) seq;
	if (seq) {
		bufq_unlink(&tcap->conq, seq);
		freemsg(seq);
	}
	if (!bufq_length(&tcap->conq))
		tcap_set_t_state(tcap, TS_IDLE);
	else
		tcap_set_t_state(tcap, TS_WRES_CIND);
	mp->b_cont = dp;
	putnext(tcap->oq, mp);
	return (0);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: no buffers\n", DRV_NAME, tcap));
	goto error;
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: flow controlled\n", DRV_NAME, tcap));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: SWERR: unexpected indication for state %ld\n", DRV_NAME, tcap,
		tcap_get_t_state(tcap)));
	goto error;
      error:
	return (err);
}

/* 
 *  T_DATA_IND          14 - data indication
 *  -----------------------------------------------------------------
 */
STATIC INLINE int
t_data_ind(queue_t *q, struct tcap *tcap, ulong more, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	struct T_data_ind *p;
	size_t msg_len = sizeof(*p);
	if ((1 << tcap_get_t_state(tcap)) & ~(TSF_DATA_XFER | TSF_WIND_ORDREL))
		goto efault;
	if (!canputnext(tcap->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = T_DATA_IND;
	p->MORE_flag = more;
	mp->b_cont = dp;
	putnext(tcap->oq, mp);
	return (0);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: no buffers\n", DRV_NAME, tcap));
	goto error;
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: flow controlled\n", DRV_NAME, tcap));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: SWERR: unexpected indication for state %ld\n", DRV_NAME, tcap,
		tcap_get_t_state(tcap)));
	goto error;
      error:
	return (err);
}

/* 
 *  T_EXDATA_IND        15 - expedited data indication
 *  -----------------------------------------------------------------
 */
STATIC INLINE int
t_exdata_ind(queue_t *q, struct tcap *tcap, ulong more, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	struct T_exdata_ind *p;
	size_t msg_len = sizeof(*p);
	if ((1 << tcap_get_t_state(tcap)) & ~(TSF_DATA_XFER | TSF_WIND_ORDREL))
		goto efault;
	if (!canputnext(tcap->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	mp->b_band = 1;		/* expedite */
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = T_EXDATA_IND;
	p->MORE_flag = more;
	mp->b_cont = dp;
	putnext(tcap->oq, mp);
	return (0);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: no buffers\n", DRV_NAME, tcap));
	goto error;
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: flow controlled\n", DRV_NAME, tcap));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: SWERR: unexpected indication for state %ld\n", DRV_NAME, tcap,
		tcap_get_t_state(tcap)));
	goto error;
      error:
	return (err);
}

/* 
 *  T_INFO_ACK          16 - information acknowledgement
 *  -----------------------------------------------------------------
 */
STATIC INLINE int
t_info_ack(queue_t *q, struct tcap *tcap)
{
	int err;
	mblk_t *mp;
	struct T_info_ack *p;
	size_t msg_len = sizeof(*p);
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	{
		ulong serv = tcap->pclass < 2 ? T_CLTS : T_COTS_ORD;
		ulong etsdu = tcap->pclass < 2 ? T_INVALID : tcap->options.mtu;
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		fixme(("Still some things to double-check here\n"));
		p->PRIM_type = T_INFO_ACK;
		p->TSDU_size = T_INFINITE;	/* no limit on TSDU size */
		p->ETSDU_size = etsdu;	/* no concept of ETSDU size */
		p->CDATA_size = tcap->options.mtu;	/* no concept of CDATA size */
		p->DDATA_size = tcap->options.mtu;	/* no concept of DDATA size */
		p->ADDR_size = sizeof(struct sccp_addr);	/* no limit on ADDR size */
		p->OPT_size = T_INFINITE;	/* no limit on OPTIONS size */
		p->TIDU_size = T_INFINITE;	/* no limit on TIDU size */
		p->SERV_type = serv;	/* COTS or CLTS */
		p->CURRENT_state = tcap_get_t_state(tcap);
		p->PROVIDER_flag = XPG4_1 & ~T_SNDZERO;
		putnext(tcap->oq, mp);
		return (0);
	}
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, tcap));
	goto error;
      error:
	return (err);
}

/* 
 *  T_BIND_ACK          17 - bind acknowledgement
 *  -----------------------------------------------------------------
 */
STATIC int
t_bind_ack(queue_t *q, struct tcap *tcap, struct sccp_addr *add)
{
	int err;
	mblk_t *mp;
	struct T_bind_ack *p;
	size_t add_len = add ? sizeof(*add) : 0;
	size_t msg_len = sizeof(*p) + add_len;
	if (tcap_get_t_state(tcap) != TS_WACK_BREQ)
		goto efault;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = T_BIND_ACK;
	p->ADDR_length = add_len;
	p->ADDR_offset = add_len ? sizeof(*p) : 0;
	p->CONIND_number = tcap->conind;
	if (add_len) {
		bcopy(add, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
	}
	tcap_set_t_state(tcap, TS_IDLE);
	putnext(tcap->oq, mp);
	return (0);
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: SWERR: unexpected indication for state %ld\n", DRV_NAME, tcap,
		tcap_get_t_state(tcap)));
	goto error;
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, tcap));
	goto error;
      error:
	return (err);
}

/* 
 *  T_ERROR_ACK         18 - error acknowledgement
 *  -----------------------------------------------------------------
 */
STATIC int
t_error_ack(queue_t *q, struct tcap *tcap, const ulong prim, long error)
{
	int err = error;
	mblk_t *mp;
	struct T_error_ack *p;
	size_t msg_len = sizeof(*p);
	switch (error) {
	case -EBUSY:
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
		seldom();
		goto error;
	case 0:
		never();
		goto error;
	}
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = T_ERROR_ACK;
	p->ERROR_prim = prim;
	p->TLI_error = error < 0 ? TSYSERR : error;
	p->UNIX_error = error < 0 ? -error : 0;
	switch (tcap_get_t_state(tcap)) {
#ifdef TS_WACK_OPTREQ
	case TS_WACK_OPTREQ:
#endif
	case TS_WACK_UREQ:
	case TS_WACK_CREQ:
		tcap_set_t_state(tcap, TS_IDLE);
		break;
	case TS_WACK_BREQ:
		tcap_set_t_state(tcap, TS_UNBND);
		break;
	case TS_WACK_CRES:
		tcap_set_t_state(tcap, TS_WRES_CIND);
		break;
	case TS_WACK_DREQ6:
		tcap_set_t_state(tcap, TS_WCON_CREQ);
		break;
	case TS_WACK_DREQ7:
		tcap_set_t_state(tcap, TS_WRES_CIND);
		break;
	case TS_WACK_DREQ9:
		tcap_set_t_state(tcap, TS_DATA_XFER);
		break;
	case TS_WACK_DREQ10:
		tcap_set_t_state(tcap, TS_WIND_ORDREL);
		break;
	case TS_WACK_DREQ11:
		tcap_set_t_state(tcap, TS_WREQ_ORDREL);
		break;
		/* Note: if we are not in a WACK state we simply do not change state.  This occurs
		   normally when we send TOUTSTATE or TNOTSUPPORT or are responding to a
		   T_OPTMGMT_REQ in other then TS_IDLE state. */
	}
	putnext(tcap->oq, mp);
	return (0);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, tcap));
	goto error;
      error:
	return (err);
}

/* 
 *  T_OK_ACK            19 - success acknowledgement
 *  -----------------------------------------------------------------
 */
STATIC int
t_ok_ack(queue_t *q, struct tcap *tcap, ulong prim, ulong seq, ulong tok)
{
	int err;
	mblk_t *mp;
	struct T_ok_ack *p;
	size_t msg_len = sizeof(*p);
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = T_OK_ACK;
	p->CORRECT_prim = prim;
	switch (tcap_get_t_state(tcap)) {
	case TS_WACK_CREQ:
		tcap_set_t_state(tcap, TS_WCON_CREQ);
		break;
	case TS_WACK_UREQ:
		tcap_set_t_state(tcap, TS_UNBND);
		break;
	case TS_WACK_CRES:
	{
		queue_t *aq = (queue_t *) tok;
		struct tcap *ap = TCAP_PRIV(aq);
		if (ap) {
			ap->i_state = TS_DATA_XFER;
//                      tcap_cleanup_read(q);
//                      tcap_transmit_wakeup(q);
		}
		if (seq) {
			bufq_unlink(&tcap->conq, (mblk_t *) seq);
			freemsg((mblk_t *) seq);
		}
		if (aq != tcap->oq) {
			if (bufq_length(&tcap->conq))
				tcap_set_t_state(tcap, TS_WRES_CIND);
			else
				tcap_set_t_state(tcap, TS_IDLE);
		}
		break;
	}
	case TS_WACK_DREQ7:
		if (seq)
			bufq_unlink(&tcap->conq, (mblk_t *) seq);
	case TS_WACK_DREQ6:
	case TS_WACK_DREQ9:
	case TS_WACK_DREQ10:
	case TS_WACK_DREQ11:
		if (bufq_length(&tcap->conq))
			tcap_set_t_state(tcap, TS_WRES_CIND);
		else
			tcap_set_t_state(tcap, TS_IDLE);
		break;
		/* Note: if we are not in a WACK state we simply do not change state.  This occurs
		   normally when we are responding to a T_OPTMGMT_REQ in other than the TS_IDLE
		   state. */
	}
	putnext(tcap->oq, mp);
	return (0);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, tcap));
	goto error;
      error:
	return (err);
}

/* 
 *  T_OPTMGMT_ACK       22 - options management acknowledgement
 *  -----------------------------------------------------------------
 */
STATIC int
t_optmgmt_ack(queue_t *q, struct tcap *tcap, ulong flags, struct tcap_opts *ops)
{
	int err;
	mblk_t *mp;
	struct T_optmgmt_ack *p;
	size_t opt_len = tcap_opts_size(tcap, ops);
	size_t msg_len = sizeof(*p) + opt_len;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = T_OPTMGMT_ACK;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) : 0;
	p->MGMT_flags = flags;
	tcap_build_opts(tcap, ops, mp->b_wptr);
	mp->b_wptr += opt_len;
#ifdef TS_WACK_OPTREQ
	if (tcap_get_t_state(tcap) == TS_WACK_OPTREQ)
		tcap_set_t_state(tcap, TS_IDLE);
#endif
	putnext(tcap->oq, mp);
	return (0);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, tcap));
	goto error;
      error:
	return (err);
}

/* 
 *  T_ORDREL_IND        23 - orderly release indication
 *  -----------------------------------------------------------------
 */
STATIC INLINE int
t_ordrel_ind(queue_t *q, struct tcap *tcap)
{
	int err;
	mblk_t *mp;
	struct T_ordrel_ind *p;
	size_t msg_len = sizeof(*p);
	if ((1 << tcap_get_t_state(tcap)) & ~(TSF_DATA_XFER | TSF_WIND_ORDREL))
		goto efault;
	if (!canputnext(tcap->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = T_ORDREL_IND;
	switch (tcap_get_t_state(tcap)) {
	case TS_DATA_XFER:
		tcap_set_t_state(tcap, TS_WREQ_ORDREL);
		break;
	case TS_WIND_ORDREL:
		tcap_set_t_state(tcap, TS_IDLE);
		break;
	}
	putnext(tcap->oq, mp);
	return (0);
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: SWERR: unexpected indication for state %ld\n", DRV_NAME, tcap,
		tcap_get_t_state(tcap)));
	goto error;
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, tcap));
	goto error;
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: flow controlled\n", DRV_NAME, tcap));
	goto error;
      error:
	return (err);
}

/* 
 *  T_ADDR_ACK          27 - address acknowledgement
 *  -----------------------------------------------------------------
 */
STATIC int
t_addr_ack(queue_t *q, struct tcap *tcap, struct sccp_addr *loc, struct sccp_addr *rem)
{
	int err;
	mblk_t *mp;
	struct T_addr_ack *p;
	size_t loc_len = loc ? sizeof(*loc) : 0;
	size_t rem_len = rem ? sizeof(*rem) : 0;
	size_t msg_len = sizeof(*p) + loc_len + rem_len;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = ((struct T_addr_ack *) mp->b_wptr)++;
	p->PRIM_type = T_ADDR_ACK;
	p->LOCADDR_length = loc_len;
	p->LOCADDR_offset = loc_len ? sizeof(*p) : 0;
	p->REMADDR_length = rem_len;
	p->REMADDR_offset = rem_len ? sizeof(*p) + loc_len : 0;
	if (loc_len) {
		bcopy(loc, mp->b_wptr, loc_len);
		mp->b_wptr += loc_len;
	}
	if (rem_len) {
		bcopy(rem, mp->b_wptr, rem_len);
		mp->b_wptr += rem_len;
	}
	putnext(tcap->oq, mp);
	return (0);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, tcap));
	goto error;
      error:
	return (err);
}

#if 0
/* 
 *  T_CAPABILITY_ACK    ?? - protocol capability ack
 *  -----------------------------------------------------------------
 */
STATIC int
t_capability_ack(queue_t *q, struct tcap *tcap, ulong caps)
{
	int err;
	mblk_t *mp;
	struct T_capability_ack *p;
	size_t msg_len = sizeof(*p);
	ulong caps = (acceptor ? TC1_ACCEPTOR_ID : 0) | (info ? TC1_INFO : 0);
	if ((mp = ss7_allocb(q, msg_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((struct T_capability_ack *) mp->b_wptr)++;
		p->PRIM_type = T_CAPABILITY_ACK;
		p->CAP_bits1 = caps;
		p->ACCEPTOR_id = (caps & TC1_ACCEPTOR_ID) ? (ulong) tcap->oq : 0;
		if (caps & TC1_INFO) {
			p->INFO_ack.PRIM_type = T_INFO_ACK;
			p->INFO_ack.TSDU_size = tcap->tsdu;
			p->INFO_ack.ETSDU_size = tcap->etsdu;
			p->INFO_ack.CDATA_size = tcap->cdata;
			p->INFO_ack.DDATA_size = tcap->ddata;
			p->INFO_ack.ADDR_size = tcap->addlen;
			p->INFO_ack.OPT_size = tcap->optlen;
			p->INFO_ack.TIDU_size = tcap->tidu;
			p->INFO_ack.SERV_type = tcap->stype;
			p->INFO_ack.CURRENT_state = tcap_get_t_state(tcap);
			p->INFO_ack.PROVIDER_flag = tcap->ptype;
		} else
			bzero(&p->INFO_ack, sizeof(p->INFO_ack));
		putnext(tcap->oq, mp);
		return (0);
	} else {
		err = -ENOBUFS;
		ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, tcap));
	}
	return (err);
}
#endif

/* 
 *  T_UNITDATA_IND      20 - Unitdata indication
 *  -----------------------------------------------------------------
 *  Note: If we cannot deliver the destination address in the option header or somewhere, it will not be possible to
 *  bind to multiple alias addresses, but will only permit us to bind to a single alias address.  This might or
 *  might not be a problem to the user.
 */
STATIC INLINE int
t_unitdata_ind(queue_t *q, struct tcap *tcap, struct sccp_addr *src, struct tcap_opts *opt,
	       mblk_t *dp)
{
	int err;
	mblk_t *mp;
	struct t_opthdr *oh;
	const size_t olen = sizeof(*oh) + sizeof(t_scalar_t);
	struct T_unitdata_ind *p;
	size_t src_len = src ? sizeof(*src) : 0;
	size_t opt_len =
	    (opt->seq ? olen : 0) + (opt->pri ? olen : 0) + (opt->pcl ? olen : 0) +
	    (opt->imp ? olen : 0) + (opt->ret ? olen : 0);
	size_t msg_len = sizeof(*p) + src_len + opt_len;
	if (!canputnext(tcap->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = T_UNITDATA_IND;
	p->SRC_length = src_len;
	p->SRC_offset = src_len ? sizeof(*p) : 0;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) + src_len : 0;
	if (src_len) {
		bcopy(src, mp->b_wptr, src_len);
		mp->b_wptr += src_len;
	}
	if (opt_len) {
		if (opt->seq) {
			oh = ((typeof(oh)) mp->b_wptr)++;
			oh->len = olen;
			oh->level = T_SS7_SCCP;
			oh->name = T_SCCP_SEQ_CTRL;
			oh->status = T_SUCCESS;
			*((t_uscalar_t *) mp->b_wptr)++ = *opt->seq;
		}
		if (opt->pri) {
			oh = ((typeof(oh)) mp->b_wptr)++;
			oh->len = olen;
			oh->level = T_SS7_SCCP;
			oh->name = T_SCCP_PRIORITY;
			oh->status = T_SUCCESS;
			*((t_uscalar_t *) mp->b_wptr)++ = *opt->pri;
		}
		if (opt->pcl) {
			oh = ((typeof(oh)) mp->b_wptr)++;
			oh->len = olen;
			oh->level = T_SS7_SCCP;
			oh->name = T_SCCP_PCLASS;
			oh->status = T_SUCCESS;
			*((t_uscalar_t *) mp->b_wptr)++ = *opt->pcl;
		}
		if (opt->imp) {
			oh = ((typeof(oh)) mp->b_wptr)++;
			oh->len = olen;
			oh->level = T_SS7_SCCP;
			oh->name = T_SCCP_IMPORTANCE;
			oh->status = T_SUCCESS;
			*((t_uscalar_t *) mp->b_wptr)++ = *opt->imp;
		}
		if (opt->ret) {
			oh = ((typeof(oh)) mp->b_wptr)++;
			oh->len = olen;
			oh->level = T_SS7_SCCP;
			oh->name = T_SCCP_RET_ERROR;
			oh->status = T_SUCCESS;
			*((t_uscalar_t *) mp->b_wptr)++ = *opt->ret;
		}
	}
	putnext(tcap->oq, mp);
	return (0);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, tcap));
	goto error;
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: Flow controlled\n", DRV_NAME, tcap));
	goto error;
      error:
	return (err);
}

/* 
 *  T_UDERROR_IND       21 - Unitdata error indication
 *  -----------------------------------------------------------------
 *  This primitive indicates to the transport user that a datagram with the specified destination address and
 *  options produced an error.
 */
STATIC INLINE int
t_uderror_ind(queue_t *q, struct tcap *tcap, ulong etype, struct sccp_addr *dst, ulong *seq,
	      ulong *pri, ulong *pcl, ulong *imp, ulong *ret, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	struct t_opthdr *oh;
	const size_t olen = sizeof(*oh) + sizeof(t_scalar_t);
	struct T_uderror_ind *p;
	size_t dst_len = dst ? sizeof(*dst) : 0;
	size_t opt_len =
	    (seq ? olen : 0) + (pri ? olen : 0) + (pcl ? olen : 0) + (imp ? olen : 0) +
	    (ret ? olen : 0);
	size_t msg_len = sizeof(*p) + dst_len;
	if (!canputnext(tcap->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	mp->b_band = 2;		/* XXX move ahead of data indications */
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = T_UDERROR_IND;
	p->DEST_length = dst_len;
	p->DEST_offset = dst_len ? sizeof(*p) : 0;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) + dst_len : 0;
	p->ERROR_type = etype;
	if (dst_len) {
		bcopy(dst, mp->b_wptr, dst_len);
		mp->b_wptr += dst_len;
	}
	if (opt_len) {
		if (seq) {
			oh = ((typeof(oh)) mp->b_wptr)++;
			oh->len = olen;
			oh->level = T_SS7_SCCP;
			oh->name = T_SCCP_SEQ_CTRL;
			oh->status = T_SUCCESS;
			*((t_uscalar_t *) mp->b_wptr)++ = *seq;
		}
		if (pri) {
			oh = ((typeof(oh)) mp->b_wptr)++;
			oh->len = olen;
			oh->level = T_SS7_SCCP;
			oh->name = T_SCCP_PRIORITY;
			oh->status = T_SUCCESS;
			*((t_uscalar_t *) mp->b_wptr)++ = *pri;
		}
		if (pcl) {
			oh = ((typeof(oh)) mp->b_wptr)++;
			oh->len = olen;
			oh->level = T_SS7_SCCP;
			oh->name = T_SCCP_PCLASS;
			oh->status = T_SUCCESS;
			*((t_uscalar_t *) mp->b_wptr)++ = *pcl;
		}
		if (imp) {
			oh = ((typeof(oh)) mp->b_wptr)++;
			oh->len = olen;
			oh->level = T_SS7_SCCP;
			oh->name = T_SCCP_IMPORTANCE;
			oh->status = T_SUCCESS;
			*((t_uscalar_t *) mp->b_wptr)++ = *imp;
		}
		if (ret) {
			oh = ((typeof(oh)) mp->b_wptr)++;
			oh->len = olen;
			oh->level = T_SS7_SCCP;
			oh->name = T_SCCP_RET_ERROR;
			oh->status = T_SUCCESS;
			*((t_uscalar_t *) mp->b_wptr)++ = *ret;
		}
	}
	putnext(tcap->oq, mp);
	return (0);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, tcap));
	goto error;
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: Flow controlled\n", DRV_NAME, tcap));
	goto error;
      error:
	return (err);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  Primitives sent downstream (to SCCP)
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  N_CONN_REQ
 *  -----------------------------------
 */
STATIC INLINE int
n_conn_req(queue_t *q, struct sccp *sccp, struct sccp_addr *dst, ulong flags,
	   N_qos_sel_conn_sccp_t * qos, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	N_conn_req_t *p;
	size_t dst_len = dst ? sizeof(*dst) + dst->alen : 0;
	size_t qos_len = qos ? sizeof(*qos) : 0;
	size_t msg_len = sizeof(*p) + dst_len + qos_len;
	if ((1 << sccp_get_i_state(sccp)) & ~(NSF_IDLE))
		goto efault;
	if (!canput(sccp->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = N_CONN_REQ;
	p->DEST_length = dst_len;
	p->DEST_offset = dst_len ? sizeof(*p) : 0;
	p->CONN_flags = flags;
	p->QOS_length = qos_len;
	p->QOS_offset = qos_len ? sizeof(*p) + dst_len : 0;
	if (dst_len) {
		bcopy(dst, mp->b_wptr, dst_len);
		mp->b_wptr += dst_len;
	}
	if (qos_len) {
		bcopy(qos, mp->b_wptr, qos_len);
		mp->b_wptr += qos_len;
	}
	mp->b_cont = dp;
	sccp_set_i_state(sccp, NS_WCON_CREQ);
	printd(("%s: %p: N_CONN_REQ ->\n", DRV_NAME, sccp));
	ss7_oput(sccp->oq, mp);
	return (QR_ABSORBED);
      efault:
	err = -EFAULT;
	swerr();
	goto error;
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  N_CONN_RES
 *  -----------------------------------
 */
STATIC INLINE int
n_conn_res(queue_t *q, struct sccp *sccp, ulong tok, struct sccp_addr *res, ulong seq,
	   ulong flags, N_qos_sel_conn_sccp_t * qos, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	N_conn_res_t *p;
	size_t res_len = res ? sizeof(*res) + res->alen : 0;
	size_t qos_len = qos ? sizeof(*qos) : 0;
	size_t msg_len = sizeof(*p) + res_len + qos_len;
	if ((1 << sccp_get_i_state(sccp)) & ~(NSF_WRES_CIND))
		goto efault;
	if (!canput(sccp->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = N_CONN_RES;
	p->TOKEN_value = tok;
	p->RES_length = res_len;
	p->RES_offset = res_len ? sizeof(*p) : 0;
	p->SEQ_number = seq;
	p->CONN_flags = flags;
	p->QOS_length = qos_len;
	p->QOS_offset = qos_len ? sizeof(*p) + res_len : 0;
	if (res_len) {
		bcopy(res, mp->b_wptr, res_len);
		mp->b_wptr += res_len;
	}
	if (qos_len) {
		bcopy(qos, mp->b_wptr, qos_len);
		mp->b_wptr += qos_len;
	}
	mp->b_cont = dp;
	if (--sccp->outcnt)
		sccp_set_i_state(sccp, NS_IDLE);
	else
		sccp_set_i_state(sccp, NS_WRES_CIND);
	printd(("%s: %p: N_CONN_RES ->\n", DRV_NAME, sccp));
	ss7_oput(sccp->oq, mp);
	return (QR_ABSORBED);
      efault:
	err = -EFAULT;
	swerr();
	goto error;
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  N_DISCON_REQ
 *  -----------------------------------
 */
STATIC INLINE int
n_discon_req(queue_t *q, struct sccp *sccp, ulong reason, struct sccp_addr *res, ulong seq,
	     mblk_t *dp)
{
	int err;
	mblk_t *mp;
	N_discon_req_t *p;
	size_t res_len = res ? sizeof(*res) + res->alen : 0;
	size_t msg_len = sizeof(*p) + res_len;
	if ((1 << sccp_get_i_state(sccp)) &
	    ~(NSF_WCON_CREQ | NSF_WRES_CIND | NSF_DATA_XFER | NSF_WCON_CREQ | NSF_WRES_RIND))
		goto efault;
	if (!canput(sccp->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = N_DISCON_REQ;
	p->DISCON_reason = reason;
	p->RES_length = res_len;
	p->RES_offset = res_len ? sizeof(*p) : 0;
	p->SEQ_number = seq;
	if (res_len) {
		bcopy(res, mp->b_wptr, res_len);
		mp->b_wptr += res_len;
	}
	mp->b_cont = dp;
	switch (sccp_get_i_state(sccp)) {
	case NS_WCON_CREQ:
		sccp_set_i_state(sccp, NS_WACK_DREQ6);
		break;
	case NS_WRES_CIND:
		sccp_set_i_state(sccp, NS_WACK_DREQ7);
		break;
	case NS_DATA_XFER:
		sccp_set_i_state(sccp, NS_WACK_DREQ9);
		break;
	case NS_WCON_RREQ:
		sccp_set_i_state(sccp, NS_WACK_DREQ10);
		break;
	case NS_WRES_RIND:
		sccp_set_i_state(sccp, NS_WACK_DREQ11);
		break;
	default:
		swerr();
		break;
	}
	printd(("%s: %p: N_DISCON_REQ ->\n", DRV_NAME, sccp));
	ss7_oput(sccp->oq, mp);
	return (QR_ABSORBED);
      efault:
	err = -EFAULT;
	swerr();
	goto error;
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  N_DATA_REQ
 *  -----------------------------------
 */
STATIC INLINE int
n_data_req(queue_t *q, struct sccp *sccp, ulong flags, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	N_data_req_t *p;
	size_t msg_len = sizeof(*p);
	if ((1 << sccp_get_i_state(sccp)) & ~(NSF_DATA_XFER))
		goto efault;
	if (!canput(sccp->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = N_DATA_REQ;
	p->DATA_xfer_flags = flags;
	mp->b_cont = dp;
	printd(("%s: %p: N_DATA_REQ ->\n", DRV_NAME, sccp));
	ss7_oput(sccp->oq, mp);
	return (QR_ABSORBED);
      efault:
	if (sccp_get_i_state(sccp) == NS_IDLE)
		goto discard;
	err = -EFAULT;
	swerr();
	goto error;
      discard:
	err = QR_DONE;
	rare();
	goto error;
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  N_EXDATA_REQ
 *  -----------------------------------
 */
STATIC INLINE int
n_exdata_req(queue_t *q, struct sccp *sccp, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	N_exdata_req_t *p;
	size_t msg_len = sizeof(*p);
	if ((1 << sccp_get_i_state(sccp)) & ~(NSF_DATA_XFER))
		goto efault;
	if (!canput(sccp->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	mp->b_band = 1;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = N_EXDATA_REQ;
	mp->b_cont = dp;
	printd(("%s: %p: N_EXDATA_REQ ->\n", DRV_NAME, sccp));
	ss7_oput(sccp->oq, mp);
	return (QR_ABSORBED);
      efault:
	if (sccp_get_i_state(sccp) == NS_IDLE)
		goto discard;
	err = -EFAULT;
	swerr();
	goto error;
      discard:
	err = QR_DONE;
	rare();
	goto error;
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  N_INFO_REQ
 *  -----------------------------------
 */
STATIC INLINE int
n_info_req(queue_t *q, struct sccp *sccp)
{
	int err;
	mblk_t *mp;
	N_info_req_t *p;
	size_t msg_len = sizeof(*p);
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = N_INFO_REQ;
	printd(("%s: %p: N_INFO_REQ ->\n", DRV_NAME, sccp));
	ss7_oput(sccp->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  N_BIND_REQ
 *  -----------------------------------
 */
STATIC INLINE int
n_bind_req(queue_t *q, struct sccp *sccp, struct sccp_addr *add, ulong cons, ulong flags,
	   uchar *pro, size_t pro_len)
{
	int err;
	mblk_t *mp;
	N_bind_req_t *p;
	size_t add_len = add ? sizeof(*add) + add->alen : 0;
	size_t msg_len = sizeof(*p) + add_len + pro_len;
	if ((1 << sccp_get_i_state(sccp)) & ~(NSF_UNBND))
		goto efault;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = N_BIND_REQ;
	p->ADDR_length = add_len;
	p->ADDR_offset = add_len ? sizeof(*p) : 0;
	p->CONIND_number = cons;
	p->BIND_flags = flags;
	p->PROTOID_length = pro_len;
	p->PROTOID_offset = pro_len ? sizeof(*p) + add_len : 0;
	if (add_len) {
		bcopy(add, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
	}
	if (pro_len) {
		bcopy(pro, mp->b_wptr, pro_len);
		mp->b_wptr += pro_len;
	}
	sccp_set_i_state(sccp, NS_WACK_BREQ);
	printd(("%s: %p: N_BIND_REQ ->\n", DRV_NAME, sccp));
	ss7_oput(sccp->oq, mp);
	return (QR_DONE);
      efault:
	err = -EFAULT;
	swerr();
	goto error;
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  N_UNBIND_REQ
 *  -----------------------------------
 */
STATIC INLINE int
n_unbind_req(queue_t *q, struct sccp *sccp)
{
	int err;
	mblk_t *mp;
	N_unbind_req_t *p;
	size_t msg_len = sizeof(*p);
	if ((1 << sccp_get_i_state(sccp)) & ~(NSF_IDLE))
		goto efault;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = N_UNBIND_REQ;
	sccp_set_i_state(sccp, NS_WACK_UREQ);
	printd(("%s: %p: N_UNBIND_REQ ->\n", DRV_NAME, sccp));
	ss7_oput(sccp->oq, mp);
	return (QR_DONE);
      efault:
	err = -EFAULT;
	swerr();
	goto error;
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  N_UNITDATA_REQ
 *  -----------------------------------
 */
STATIC INLINE int
n_unitdata_req(queue_t *q, struct sccp *sccp, struct sccp_addr *dst, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	N_unitdata_req_t *p;
	size_t dst_len = dst ? sizeof(*dst) + dst->alen : 0;
	size_t msg_len = sizeof(*p) + dst_len;
	if ((1 << sccp_get_i_state(sccp)) & ~(NSF_IDLE))
		goto efault;
	if (!canput(sccp->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = N_UNITDATA_REQ;
	p->DEST_length = dst_len;
	p->DEST_offset = dst_len ? sizeof(*p) : 0;
	p->RESERVED_field[0] = 0;
	p->RESERVED_field[1] = 0;
	if (dst_len) {
		bcopy(dst, mp->b_wptr, dst_len);
		mp->b_wptr += dst_len;
	}
	mp->b_cont = dp;
	printd(("%s: %p: N_UNITDATA_REQ ->\n", DRV_NAME, sccp));
	ss7_oput(sccp->oq, mp);
	return (QR_ABSORBED);
      efault:
	err = -EFAULT;
	swerr();
	goto error;
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  N_OPTMGMT_REQ
 *  -----------------------------------
 */
STATIC INLINE int
n_optmgmt_req(queue_t *q, struct sccp *sccp, N_qos_sel_info_sccp_t * qos, ulong flags)
{
	int err;
	mblk_t *mp;
	N_optmgmt_req_t *p;
	size_t qos_len = qos ? sizeof(*qos) : 0;
	size_t msg_len = sizeof(*p) + qos_len;
	if (!canput(sccp->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = N_OPTMGMT_REQ;
	p->QOS_length = qos_len;
	p->QOS_offset = qos_len ? sizeof(*p) : 0;
	p->OPTMGMT_flags = flags;
	if (sccp_get_i_state(sccp) == NS_IDLE)
		sccp_set_i_state(sccp, NS_WACK_OPTREQ);
	printd(("%s: %p: N_OPTMGMT_REQ ->\n", DRV_NAME, sccp));
	ss7_oput(sccp->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  N_DATACK_REQ
 *  -----------------------------------
 */
STATIC INLINE int
n_datack_req(queue_t *q, struct sccp *sccp)
{
	int err;
	mblk_t *mp;
	N_datack_req_t *p;
	size_t msg_len = sizeof(*p);
	if ((1 << sccp_get_i_state(sccp)) & ~(NSF_DATA_XFER))
		goto efault;
	if (!canput(sccp->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = N_DATACK_REQ;
	printd(("%s: %p: N_DATACK_REQ ->\n", DRV_NAME, sccp));
	ss7_oput(sccp->oq, mp);
	return (QR_DONE);
      efault:
	if (sccp_get_i_state(sccp) == NS_IDLE)
		goto discard;
	err = -EFAULT;
	swerr();
	goto error;
      discard:
	err = QR_DONE;
	rare();
	goto error;
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  N_RESET_REQ
 *  -----------------------------------
 */
STATIC INLINE int
n_reset_req(queue_t *q, struct sccp *sccp, ulong reason)
{
	int err;
	mblk_t *mp;
	N_reset_req_t *p;
	size_t msg_len = sizeof(*p);
	if ((1 << sccp_get_i_state(sccp)) & ~(NSF_DATA_XFER))
		goto efault;
	if (!canput(sccp->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = N_RESET_REQ;
	p->RESET_reason = reason;
	sccp_set_i_state(sccp, NS_WCON_RREQ);
	printd(("%s: %p: N_RESET_REQ ->\n", DRV_NAME, sccp));
	ss7_oput(sccp->oq, mp);
	return (QR_DONE);
      efault:
	err = -EFAULT;
	swerr();
	goto error;
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  N_RESET_RES
 *  -----------------------------------
 */
STATIC INLINE int
n_reset_res(queue_t *q, struct sccp *sccp)
{
	int err;
	mblk_t *mp;
	N_reset_res_t *p;
	size_t msg_len = sizeof(*p);
	if ((1 << sccp_get_i_state(sccp)) & ~(NSF_DATA_XFER))
		goto efault;
	if (!canput(sccp->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = N_RESET_RES;
	sccp_set_i_state(sccp, NS_DATA_XFER);
	printd(("%s: %p: N_RESET_RES ->\n", DRV_NAME, sccp));
	ss7_oput(sccp->oq, mp);
	return (QR_DONE);
      efault:
	err = -EFAULT;
	swerr();
	goto error;
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  N_INFORM_REQ
 *  -----------------------------------
 */
STATIC INLINE int
n_inform_req(queue_t *q, struct sccp *sccp)
{
	int err;
	mblk_t *mp;
	N_inform_req_t *p;
	size_t msg_len = sizeof(*p);
	if (!canput(sccp->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = N_INFORM_REQ;
	printd(("%s: %p: N_INFORM_REQ ->\n", DRV_NAME, sccp));
	ss7_oput(sccp->oq, mp);
	return (QR_ABSORBED);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  N_COORD_REQ
 *  -----------------------------------
 */
STATIC INLINE int
n_coord_req(queue_t *q, struct sccp *sccp, struct sccp_addr *add)
{
	int err;
	mblk_t *mp;
	N_coord_req_t *p;
	size_t add_len = add ? sizeof(*add) + add->alen : 0;
	size_t msg_len = sizeof(*p) + add_len;
	if (!canput(sccp->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = N_COORD_REQ;
	p->ADDR_length = add_len;
	p->ADDR_offset = add_len ? sizeof(*p) : 0;
	if (add_len) {
		bcopy(add, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
	}
	printd(("%s: %p: N_COORD_REQ ->\n", DRV_NAME, sccp));
	ss7_oput(sccp->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  N_COORD_RES
 *  -----------------------------------
 */
STATIC INLINE int
n_coord_res(queue_t *q, struct sccp *sccp, struct sccp_addr *add)
{
	int err;
	mblk_t *mp;
	N_coord_res_t *p;
	size_t add_len = add ? sizeof(*add) + add->alen : 0;
	size_t msg_len = sizeof(*p) + add_len;
	if (!canput(sccp->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = N_COORD_RES;
	p->ADDR_length = add_len;
	p->ADDR_offset = add_len ? sizeof(*p) : 0;
	if (add_len) {
		bcopy(add, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
	}
	printd(("%s: %p: N_COORD_RES ->\n", DRV_NAME, sccp));
	ss7_oput(sccp->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  N_STATE_REQ
 *  -----------------------------------
 */
STATIC INLINE int
n_state_req(queue_t *q, struct sccp *sccp, struct sccp_addr *add, ulong status)
{
	int err;
	mblk_t *mp;
	N_state_req_t *p;
	size_t add_len = add ? sizeof(*add) + add->alen : 0;
	size_t msg_len = sizeof(*p) + add_len;
	if (!canput(sccp->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr++;
	p->PRIM_type = N_STATE_REQ;
	p->ADDR_length = add_len;
	p->ADDR_offset = add_len ? sizeof(*p) : 0;
	p->STATUS = status;
	if (add_len) {
		bcopy(add, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
	}
	printd(("%s: %p: N_STATE_REQ ->\n", DRV_NAME, sccp));
	ss7_oput(sccp->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  =========================================================================
 *
 *  TCAP SEND Message Functions
 *
 *  =========================================================================
 */

/*
 *  TCAP_MT_UNI
 *  -----------------------------------
 */
STATIC int
tcap_send_uni(queue_t *q, struct sccp *sccp, struct tr_msg *m)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  TCAP_MT_QWP
 *  -----------------------------------
 */
STATIC int
tcap_send_qwp(queue_t *q, struct sccp *sccp, struct tr_msg *m)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  TCAP_MT_QWOP
 *  -----------------------------------
 */
STATIC int
tcap_send_qwop(queue_t *q, struct sccp *sccp, struct tr_msg *m)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  TCAP_MT_CWP
 *  -----------------------------------
 */
STATIC int
tcap_send_cwp(queue_t *q, struct sccp *sccp, struct tr_msg *m)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  TCAP_MT_CWOP
 *  -----------------------------------
 */
STATIC int
tcap_send_cwop(queue_t *q, struct sccp *sccp, struct tr_msg *m)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  TCAP_MT_RESP
 *  -----------------------------------
 */
STATIC int
tcap_send_resp(queue_t *q, struct sccp *sccp, struct tr_msg *m)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  TCAP_MT_ABORT
 *  -----------------------------------
 */
STATIC int
tcap_send_abort(queue_t *q, struct sccp *sccp, struct tr_msg *m)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

STATIC int
tcap_send_msg(queue_t *q, struct sccp *sccp, struct tr_msg *m)
{
	int ret;
	switch (m->type) {
	case TCAP_MT_UNI:
		ret = tcap_send_uni(q, sccp, m);
		break;
	case TCAP_MT_QWP:
		ret = tcap_send_qwp(q, sccp, m);
		break;
	case TCAP_MT_QWOP:
		ret = tcap_send_qwop(q, sccp, m);
		break;
	case TCAP_MT_CWP:
		ret = tcap_send_cwp(q, sccp, m);
		break;
	case TCAP_MT_CWOP:
		ret = tcap_send_cwop(q, sccp, m);
		break;
	case TCAP_MT_RESP:
		ret = tcap_send_resp(q, sccp, m);
		break;
	case TCAP_MT_ABORT:
		ret = tcap_send_abort(q, sccp, m);
		break;
	default:
		ret = -EFAULT;
		break;
	}
	return (ret);
}

/*
 *  =========================================================================
 *
 *  TCAP RECV Message Functions
 *
 *  =========================================================================
 */
/*
 *  TCAP_MT_UNI
 *  -----------------------------------
 */
STATIC int
tcap_recv_uni(queue_t *q, struct tcap *tcap, struct tcap_opts *opt, struct tr_msg *m, mblk_t *dp)
{
	int err;
	ulong did = 0;
	ulong flags = (m->parms & TCAP_PTF_CSEQ) ? 1 : 0;
	todo(("Fill out other options\n"));
	switch (tcap->i_style) {
	case TCAP_STYLE_TRI:
		if ((err = tr_uni_ind(q, tcap, &m->sccp.orig, &m->sccp.dest, opt, dp)) < 0)
			goto error;
		break;
	case TCAP_STYLE_TCI:
		if ((err =
		     tc_uni_ind(q, tcap, &m->sccp.orig, &m->sccp.dest, opt, did, flags, dp)) < 0)
			goto error;
		/* Provide a TC_UNI_IND and follow with any TC_XXX_IND component indications. */
		fixme(("Send components.\n"));
		break;
	case TCAP_STYLE_TPI:
		if ((err = t_unitdata_ind(q, tcap, &m->sccp.orig, opt, dp)) < 0)
			goto error;
		break;
	default:
		err = -EFAULT;
		swerr();
		goto error;
	}
	return (QR_TRIMMED);
      error:
	return (err);
}

/*
 *  TCAP_MT_QWP
 *  -----------------------------------
 */
STATIC int
tcap_recv_qwp(queue_t *q, struct tcap *tcap, struct tcap_opts *opt, struct tr_msg *m, mblk_t *dp)
{
	int err;
	switch (tcap->i_style) {
	case TCAP_STYLE_TRI:
		if ((err = tr_begin_ind(q, tcap, tr, &m->sccp.orig, &m->sccp.dest, opt, dp)) < 0)
			goto error;
		break;
	case TCAP_STYLE_TCI:
		if ((err =
		     tc_begin_ind(q, tcap, &m->sccp.orig, &m->sccp.dest, opt, did, flags, dp)) < 0)
			goto error;
		/* Provide a TC_BEG_IND and follow with any TC_XXX_IND component indications. */
		fixme(("Send components\n"));
		break;
	case TCAP_STYLE_TPI:
		if ((err = t_conn_ind(q, tcap, m->bp)) < 0)
			goto error;
		break;
	default:
		err = -EFAULT;
		swerr();
		goto error;
	}
	return (QR_TRIMMED);
      error:
	return (err);
}

/*
 *  TCAP_MT_QWOP
 *  -----------------------------------
 */
STATIC int
tcap_recv_qwop(queue_t *q, struct tcap *tcap, struct tcap_opts *opt, struct tr_msg *m, mblk_t *dp)
{
	int err;
	switch (tcap->i_style) {
	case TCAP_STYLE_TRI:
		if ((err = tr_begin_ind(q, tcap, tr, &m->sccp.orig, &m->sccp.dest, opt, dp)) < 0)
			goto error;
		break;
	case TCAP_STYLE_TCI:
		if ((err =
		     tc_begin_ind(q, tcap, &m->sccp.orig, &m->sccp.dest, opt, did, flags, dp)) < 0)
			goto error;
		/* Provide a TC_BEG_IND and follow with any TC_XXX_IND component indications. */
		fixme(("Send components\n"));
		break;
	case TCAP_STYLE_TPI:
		if ((err = t_conn_ind(q, tcap, m->bp)) < 0)
			goto error;
		break;
	default:
		err = -EFAULT;
		swerr();
		goto error;
	}
	return (QR_TRIMMED);
      error:
	return (err);
}

/*
 *  TCAP_MT_CWP
 *  -----------------------------------
 */
STATIC int
tcap_recv_cwp(queue_t *q, struct tcap *tcap, struct tcap_opts *opt, struct tr_msg *m, mblk_t *dp)
{
	int err;
	switch (tcap->i_style) {
	case TCAP_STYLE_TRI:
		if ((err = tr_begin_con(q, tcap, tr, &m->sccp.orig, opt, dp)))
			goto error;
		break;
	case TCAP_STYLE_TCI:
		if ((err = tc_begin_con(q, tcap, opt, did, flags, dp)))
			goto error;
		/* Provide a TC_BEG_CON or TC_CONT_IND and follow with any TC_XXX_IND component
		   indications. */
		fixme(("Send components.\n"));
		break;
	case TCAP_STYLE_TPI:
		if ((err = t_conn_con(q, tcap, &m->sccp.orig, opt, flags, dp)))
			goto error;
		break;
	}
	return (QR_TRIMMED);
      error:
	return (err);
}

/*
 *  TCAP_MT_CWOP
 *  -----------------------------------
 */
STATIC int
tcap_recv_cwop(queue_t *q, struct tcap *tcap, struct tcap_opts *opt, struct tr_msg *m, mblk_t *dp)
{
	int err;
	switch (tcap->i_style) {
	case TCAP_STYLE_TRI:
		if ((err = tr_begin_con(q, tcap, tr, &m->sccp.orig, opt, dp)))
			goto error;
		break;
	case TCAP_STYLE_TCI:
		if ((err = tc_begin_con(q, tcap, opt, did, flags, dp)))
			goto error;
		/* Provide a TC_BEG_CON or TC_CONT_IND and follow with any TC_XXX_IND component
		   indications. */
		fixme(("Send components.\n"));
		break;
	case TCAP_STYLE_TPI:
		if ((err = t_conn_con(q, tcap, &m->sccp.orig, opt, flags, dp)))
			goto error;
		break;
	}
	return (QR_TRIMMED);
      error:
	return (err);
}

/*
 *  TCAP_MT_RESP
 *  -----------------------------------
 */
STATIC int
tcap_recv_resp(queue_t *q, struct tcap *tcap, struct tcap_opts *opt, struct tr_msg *m, mblk_t *dp)
{
	int err;
	switch (tcap->i_style) {
	case TCAP_STYLE_TRI:
		if ((err = tr_end_ind(q, tcap, tr, opt, dp)))
			goto error;
		break;
	case TCAP_STYLE_TCI:
		if ((err = tc_end_ind(q, tcap, opt, did, flags, dp)))
			goto error;
		/* Provide a TC_END_IND and follow with any TC_XXX_IND component indications. */
		fixme(("Send components.\n"));
		break;
	case TCAP_STYLE_TPI:
		if ((err = t_ordrel_ind(q, tcap)))
			goto error;
		break;
	}
	return (QR_TRIMMED);
      error:
	return (err);
}

/*
 *  TCAP_MT_ABORT
 *  -----------------------------------
 */
STATIC int
tcap_recv_abort(queue_t *q, struct tcap *tcap, struct tcap_opts *opt, struct tr_msg *m, mblk_t *dp)
{
	int err;
	switch (tcap->i_style) {
	case TCAP_STYLE_TRI:
		if ((err = tr_abort_ind(q, tcap, tr, cause, orig, opt)))
			goto error;
		break;
	case TCAP_STYLE_TCI:
		if ((err = tc_abort_ind(q, tcap, opt, did, reason, orig)))
			goto error;
		fixme(("Send components.\n"));
		/* Provide a TC_ABORT_IND. */
		break;
	case TCAP_STYLE_TPI:
		if ((err = t_discon_ind(q, tcap, reason, seq, dp)))
			goto error;
		break;
	}
	return (QR_TRIMMED);
      error:
	return (err);
}

STATIC int
tcap_recv_msg(queue_t *q, struct tcap *tcap, struct tcap_opts *opt, struct tr_msg *m, mblk_t *dp)
{
	int ret;
	switch (m->type) {
	case TCAP_MT_UNI:
		ret = tcap_recv_uni(q, tcap, opt, m, dp);
		break;
	case TCAP_MT_QWP:
		ret = tcap_recv_qwp(q, tcap, opt, m, dp);
		break;
	case TCAP_MT_QWOP:
		ret = tcap_recv_qwop(q, tcap, opt, m, dp);
		break;
	case TCAP_MT_CWP:
		ret = tcap_recv_cwp(q, tcap, opt, m, dp);
		break;
	case TCAP_MT_CWOP:
		ret = tcap_recv_cwop(q, tcap, opt, m, dp);
		break;
	case TCAP_MT_RESP:
		ret = tcap_recv_resp(q, tcap, opt, m, dp);
		break;
	case TCAP_MT_ABORT:
		ret = tcap_recv_abort(q, tcap, opt, m, dp);
		break;
	default:
		ret = -EOPNOTSUPP;
		break;
	}
	return (ret);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Received Primitives
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  -------------------------------------------------------------------------
 *
 *  Primitives received from above (TCAP User).
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  TCI Primitives
 *  -----------------------------------------------------------
 */
/*
 *  TC_INFO_REQ
 *  -----------------------------------------------------------
 */
STATIC int
tc_info_req(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	int err;
	struct TC_info_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badprim:
	err = TCBADPRIM;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tcap));
	goto error;
      error:
	return tc_error_ack(q, tcap, p->PRIM_type, err, 0, 0);
}

/*
 *  TC_BIND_REQ
 *  -----------------------------------------------------------
 */
STATIC int
tc_bind_req(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	int err;
	struct TC_bind_req *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *add = NULL;
	ulong xact = 0, flags = 0;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (p->ADDR_length) {
		if (mp->b_wptr < mp->b_rptr + p->ADDR_offset + p->ADDR_length)
			goto badprim;
		if (p->ADDR_length < sizeof(*add))
			goto badaddr;
		add = (typeof(add)) (mp->b_rptr + p->ADDR_offset);
		if (p->ADDR_length != sizeof(*add) + add->alen)
			goto badaddr;
	}
	xact = p->XACT_number;
	flags = p->BIND_flags;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badaddr:
	err = TCBADADDR;
	ptrace(("%s: %p: ERROR: bad address format\n", DRV_NAME, tcap));
	goto error;
      badprim:
	err = TCBADPRIM;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tcap));
	goto error;
      error:
	return tc_error_ack(q, tcap, p->PRIM_type, err, 0, 0);
}

/*
 *  TC_UNBIND_REQ
 *  -----------------------------------------------------------
 */
STATIC int
tc_unbind_req(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	int err;
	struct TC_unbind_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (tcap->dg.list)
		goto outstate;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      outstate:
	err = TCOUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, tcap));
	goto error;
      badprim:
	err = TCBADPRIM;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tcap));
	goto error;
      error:
	return tc_error_ack(q, tcap, p->PRIM_type, err, 0, 0);
}

/*
 *  TC_SUBS_BIND_REQ
 *  -----------------------------------------------------------
 */
STATIC int
tc_subs_bind_req(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	int err;
	struct TC_subs_bind_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badprim:
	err = TCBADPRIM;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tcap));
	goto error;
      error:
	return tc_error_ack(q, tcap, p->PRIM_type, err, 0, 0);
}

/*
 *  TC_SUBS_UNBIND_REQ
 *  -----------------------------------------------------------
 */
STATIC int
tc_subs_unbind_req(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	int err;
	struct TC_subs_unbind_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badprim:
	err = TCBADPRIM;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tcap));
	goto error;
      error:
	return tc_error_ack(q, tcap, p->PRIM_type, err, 0, 0);
}

/*
 *  TC_OPTMGMT_REQ
 *  -----------------------------------------------------------
 */
STATIC int
tc_optmgmt_req(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	int err;
	struct TC_optmgmt_req *p = (typeof(p)) mp->b_rptr;
	struct tcap_opts opts = { 0UL, NULL, };
	ulong flags = 0;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (p->OPT_length) {
		if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
			goto badprim;
		if (tcap_parse_opts(tcap, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
	}
	flags = p->MGMT_flags;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badopt:
	err = TCBADOPT;
	ptrace(("%s: %p: ERROR: bad options format\n", DRV_NAME, tcap));
	goto error;
      badprim:
	err = TCBADPRIM;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tcap));
	goto error;
      error:
	return tc_error_ack(q, tcap, p->PRIM_type, err, 0, 0);
}

/*
 *  TC_UNI_REQ
 *  -----------------------------------------------------------
 */
STATIC int
tc_uni_req(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	int err;
	struct TC_uni_req *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *src = NULL;
	struct sccp_addr *dst = NULL;
	struct tcap_opts opts = { 0UL, };
	struct dg *dg;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (p->SRC_length) {
		if (mp->b_wptr < mp->b_rptr + p->SRC_offset + p->SRC_length)
			goto badprim;
		if (p->SRC_length < sizeof(*src))
			goto badaddr;
		src = (typeof(src)) (mp->b_rptr + p->SRC_offset);
		if (p->SRC_length != sizeof(*src) + src->alen)
			goto badaddr;
	}
	if (p->DEST_length) {
		if (mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
			goto badprim;
		if (p->DEST_length < sizeof(*dst))
			goto badaddr;
		dst = (typeof(dst)) (mp->b_rptr + p->DEST_offset);
		if (p->DEST_length != sizeof(*dst) + dst->alen)
			goto badaddr;
	}
	if (p->OPT_length) {
		if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
			goto badprim;
		if (tcap_parse_opts(tcap, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
	}
	if (!(dg = dg_lookup(tcap, p->DIALOG_id)))
		goto baddid;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badopt:
	err = TCBADOPT;
	ptrace(("%s: %p: ERROR: bad options format\n", DRV_NAME, tcap));
	goto error;
      badaddr:
	err = TCBADADDR;
	ptrace(("%s: %p: ERROR: bad address format\n", DRV_NAME, tcap));
	goto error;
      badprim:
	err = TCBADPRIM;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tcap));
	goto error;
      error:
	return tc_error_ack(q, tcap, p->PRIM_type, err, p->DIALOG_id, 0);
}

/*
 *  TC_BEGIN_REQ
 *  -----------------------------------------------------------
 */
STATIC int
tc_begin_req(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	int err;
	struct TC_begin_req *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *src = NULL;
	struct sccp_addr *dst = NULL;
	struct tcap_opts opts = { 0UL, };
	ulong flags;
	struct dg *dg;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (p->SRC_length) {
		if (mp->b_wptr < mp->b_rptr + p->SRC_offset + p->SRC_length)
			goto badprim;
		if (p->SRC_length < sizeof(*src))
			goto badaddr;
		src = (typeof(src)) (mp->b_rptr + p->SRC_offset);
		if (p->SRC_length != sizeof(*src) + src->alen)
			goto badaddr;
	}
	if (p->DEST_length) {
		if (mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
			goto badprim;
		if (p->DEST_length < sizeof(*dst))
			goto badaddr;
		dst = (typeof(dst)) (mp->b_rptr + p->DEST_offset);
		if (p->DEST_length != sizeof(*dst) + dst->alen)
			goto badaddr;
	}
	if (p->OPT_length) {
		if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
			goto badprim;
		if (tcap_parse_opts(tcap, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
	}
	if (!(dg = dg_lookup(tcap, p->DIALOG_id)))
		goto baddid;
	flags = p->COMP_flags;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badopt:
	err = TCBADOPT;
	ptrace(("%s: %p: ERROR: bad options format\n", DRV_NAME, tcap));
	goto error;
      badaddr:
	err = TCBADADDR;
	ptrace(("%s: %p: ERROR: bad address format\n", DRV_NAME, tcap));
	goto error;
      badprim:
	err = TCBADPRIM;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tcap));
	goto error;
      error:
	return tc_error_ack(q, tcap, p->PRIM_type, err, p->DIALOG_id, 0);
}

/*
 *  TC_BEGIN_RES
 *  -----------------------------------------------------------
 */
STATIC int
tc_begin_res(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	int err;
	struct TC_begin_res *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *src = NULL;
	struct tcap_opts opts = { 0UL, };
	ulong flags;
	struct dg *dg;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (p->SRC_length) {
		if (mp->b_wptr < mp->b_rptr + p->SRC_offset + p->SRC_length)
			goto badprim;
		if (p->SRC_length < sizeof(*src))
			goto badaddr;
		src = (typeof(src)) (mp->b_rptr + p->SRC_offset);
		if (p->SRC_length != sizeof(*src) + src->alen)
			goto badaddr;
	}
	if (p->OPT_length) {
		if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
			goto badprim;
		if (tcap_parse_opts(tcap, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
	}
	if (!(dg = dg_lookup(tcap, p->DIALOG_id)))
		goto baddid;
	flags = p->COMP_flags;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badopt:
	err = TCBADOPT;
	ptrace(("%s: %p: ERROR: bad options format\n", DRV_NAME, tcap));
	goto error;
      badaddr:
	err = TCBADADDR;
	ptrace(("%s: %p: ERROR: bad address format\n", DRV_NAME, tcap));
	goto error;
      badprim:
	err = TCBADPRIM;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tcap));
	goto error;
      error:
	return tc_error_ack(q, tcap, p->PRIM_type, err, p->DIALOG_id, 0);
}

/*
 *  TC_CONT_REQ
 *  -----------------------------------------------------------
 */
STATIC int
tc_cont_req(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	int err;
	struct TC_cont_req *p = (typeof(p)) mp->b_rptr;
	struct tcap_opts opts = { 0UL, };
	ulong flags;
	struct dg *dg;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (p->OPT_length) {
		if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
			goto badprim;
		if (tcap_parse_opts(tcap, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
	}
	if (!(dg = dg_lookup(tcap, p->DIALOG_id)))
		goto baddid;
	flags = p->COMP_flags;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badopt:
	err = TCBADOPT;
	ptrace(("%s: %p: ERROR: bad options format\n", DRV_NAME, tcap));
	goto error;
      badprim:
	err = TCBADPRIM;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tcap));
	goto error;
      error:
	return tc_error_ack(q, tcap, p->PRIM_type, err, p->DIALOG_id, 0);
}

/*
 *  TC_END_REQ
 *  -----------------------------------------------------------
 */
STATIC int
tc_end_req(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	int err;
	struct TC_end_req *p = (typeof(p)) mp->b_rptr;
	struct tcap_opts opts = { 0UL, };
	ulong scenario;
	struct dg *dg;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (p->OPT_length) {
		if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
			goto badprim;
		if (tcap_parse_opts(tcap, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
	}
	if (!(dg = dg_lookup(tcap, p->DIALOG_id)))
		goto baddid;
	scenario = p->TERM_scenario;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badopt:
	err = TCBADOPT;
	ptrace(("%s: %p: ERROR: bad options format\n", DRV_NAME, tcap));
	goto error;
      badprim:
	err = TCBADPRIM;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tcap));
	goto error;
      error:
	return tc_error_ack(q, tcap, p->PRIM_type, err, p->DIALOG_id, 0);
}

/*
 *  TC_ABORT_REQ
 *  -----------------------------------------------------------
 */
STATIC int
tc_abort_req(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	int err;
	struct TC_abort_req *p = (typeof(p)) mp->b_rptr;
	struct tcap_opts opts = { 0UL, };
	ulong reason;
	struct dg *dg;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (p->OPT_length) {
		if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
			goto badprim;
		if (tcap_parse_opts(tcap, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
	}
	if (!(dg = dg_lookup(tcap, p->DIALOG_id)))
		goto baddid;
	reason = p->ABORT_reason;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badopt:
	err = TCBADOPT;
	ptrace(("%s: %p: ERROR: bad options format\n", DRV_NAME, tcap));
	goto error;
      badprim:
	err = TCBADPRIM;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tcap));
	goto error;
      error:
	return tc_error_ack(q, tcap, p->PRIM_type, err, p->DIALOG_id, 0);
}

/*
 *  TC_INVOKE_REQ
 *  -----------------------------------------------------------
 */
STATIC int
tc_invoke_req(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	int err;
	struct TC_invoke_req *p = (typeof(p)) mp->b_rptr;
	ulong pcls, oper, more, to;
	struct dg *dg;
	struct iv *iv, *li;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (!(dg = dg_lookup(tcap, p->DIALOG_id)))
		goto baddid;
	if (p->LINKED_id && !(li = iv_lookup(dg, p->LINKED_id)))
		goto badlid;
	if (!(iv = iv_lookup(dg, p->INVOKE_id)))
		goto badiid;
	pcls = p->PROTOCOL_class;
	oper = p->OPERATION;
	more = p->MORE_flag;
	to = p->TIMEOUT;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badprim:
	err = TCBADPRIM;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tcap));
	goto error;
      error:
	return tc_error_ack(q, tcap, p->PRIM_type, err, p->DIALOG_id, p->INVOKE_id);
}

/*
 *  TC_RESULT_REQ
 *  -----------------------------------------------------------
 */
STATIC int
tc_result_req(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	int err;
	struct TC_result_req *p = (typeof(p)) mp->b_rptr;
	ulong oper, more;
	struct dg *dg;
	struct iv *iv;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (!(dg = dg_lookup(tcap, p->DIALOG_id)))
		goto baddid;
	if (!(iv = iv_lookup(dg, p->INVOKE_id)))
		goto badiid;
	oper = p->OPERATION;
	more = p->MORE_flag;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badprim:
	err = TCBADPRIM;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tcap));
	goto error;
      error:
	return tc_error_ack(q, tcap, p->PRIM_type, err, p->DIALOG_id, p->INVOKE_id);
}

/*
 *  TC_ERROR_REQ
 *  -----------------------------------------------------------
 */
STATIC int
tc_error_req(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	int err;
	struct TC_error_req *p = (typeof(p)) mp->b_rptr;
	ulong ecode, more;
	struct dg *dg;
	struct iv *iv;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (!(dg = dg_lookup(tcap, p->DIALOG_id)))
		goto baddid;
	if (!(iv = iv_lookup(dg, p->INVOKE_id)))
		goto badiid;
	ecode = p->ERROR_code;
	more = p->MORE_flag;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badprim:
	err = TCBADPRIM;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tcap));
	goto error;
      error:
	return tc_error_ack(q, tcap, p->PRIM_type, err, p->DIALOG_id, p->INVOKE_id);
}

/*
 *  TC_CANCEL_REQ
 *  -----------------------------------------------------------
 */
STATIC int
tc_cancel_req(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	int err;
	struct TC_cancel_req *p = (typeof(p)) mp->b_rptr;
	struct dg *dg;
	struct iv *iv;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (!(dg = dg_lookup(tcap, p->DIALOG_id)))
		goto baddid;
	if (!(iv = iv_lookup(dg, p->INVOKE_id)))
		goto badiid;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badprim:
	err = TCBADPRIM;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tcap));
	goto error;
      error:
	return tc_error_ack(q, tcap, p->PRIM_type, err, p->DIALOG_id, p->INVOKE_id);
}

/*
 *  TC_REJECT_REQ
 *  -----------------------------------------------------------
 */
STATIC int
tc_reject_req(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	int err;
	struct TC_reject_req *p = (typeof(p)) mp->b_rptr;
	ulong pcode;
	struct dg *dg;
	struct iv *iv;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (!(dg = dg_lookup(tcap, p->DIALOG_id)))
		goto baddid;
	if (!(iv = iv_lookup(dg, p->INVOKE_id)))
		goto badiid;
	pcode = p->PROBLEM_code;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badprim:
	err = TCBADPRIM;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tcap));
	goto error;
      error:
	return tc_error_ack(q, tcap, p->PRIM_type, err, p->DIALOG_id, p->INVOKE_id);
}

/*
 *  TRI Primitives
 *  -----------------------------------------------------------
 */
/*
 *  TR_INFO_REQ
 *  -----------------------------------------------------------
 */
STATIC int
tr_info_req(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	int err;
	struct TR_info_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badprim:
	err = TRBADPRIM;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tcap));
	goto error;
      error:
	return tr_error_ack(q, tcap, p->PRIM_type, err, 0);
}

/*
 *  TR_BIND_REQ
 *  -----------------------------------------------------------
 */
STATIC int
tr_bind_req(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	int err;
	struct TR_bind_req *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *add = NULL;
	ulong xact, flags;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (p->ADDR_length) {
		if (mp->b_wptr < mp->b_rptr + p->ADDR_offset + p->ADDR_length)
			goto badprim;
		if (p->ADDR_length < sizeof(*add))
			goto badaddr;
		add = (typeof(add)) (mp->b_rptr + p->ADDR_offset);
		if (p->ADDR_length != sizeof(*add) + add->alen)
			goto badaddr;
	}
	xact = p->XACT_number;
	flags = p->BIND_flags;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badaddr:
	err = TRBADADDR;
	ptrace(("%s: %p: ERROR: bad address format\n", DRV_NAME, tcap));
	goto error;
      badprim:
	err = TRBADPRIM;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tcap));
	goto error;
      error:
	return tr_error_ack(q, tcap, p->PRIM_type, err, 0);
}

/*
 *  TR_UNBIND_REQ
 *  -----------------------------------------------------------
 */
STATIC int
tr_unbind_req(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	int err;
	struct TR_unbind_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badprim:
	err = TRBADPRIM;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tcap));
	goto error;
      error:
	return tr_error_ack(q, tcap, p->PRIM_type, err, 0);
}

/*
 *  TR_OPTMGMT_REQ
 *  -----------------------------------------------------------
 */
STATIC int
tr_optmgmt_req(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	int err;
	struct TR_optmgmt_req *p = (typeof(p)) mp->b_rptr;
	struct tcap_opts opts = { 0, };
	ulong flags;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (p->OPT_length) {
		if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
			goto badprim;
		if (tcap_parse_opts(tcap, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
	}
	flags = p->MGMT_flags;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badopt:
	err = TRBADOPT;
	ptrace(("%s: %p: ERROR: bad options format\n", DRV_NAME, tcap));
	goto error;
      badprim:
	err = TRBADPRIM;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tcap));
	goto error;
      error:
	return tr_error_ack(q, tcap, p->PRIM_type, err, 0);
}

/*
 *  TR_UNI_REQ
 *  -----------------------------------------------------------
 */
STATIC int
tr_uni_req(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	int err;
	struct TR_uni_req *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *dst = NULL;
	struct sccp_addr *org = NULL;
	struct tcap_opts opts = { 0, };
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (p->DEST_length) {
		if (mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
			goto badprim;
		if (p->DEST_length < sizeof(*dst))
			goto badaddr;
		dst = (typeof(dst)) (mp->b_rptr + p->DEST_offset);
		if (p->DEST_length != sizeof(*dst) + dst->alen)
			goto badaddr;
	}
	if (p->ORIG_length) {
		if (mp->b_wptr < mp->b_rptr + p->ORIG_offset + p->ORIG_length)
			goto badprim;
		if (p->ORIG_length < sizeof(*org))
			goto badaddr;
		org = (typeof(org)) (mp->b_rptr + p->ORIG_offset);
		if (p->ORIG_length != sizeof(*org) + org->alen)
			goto badaddr;
	}
	if (p->OPT_length) {
		if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
			goto badprim;
		if (tcap_parse_opts(tcap, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
	}
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badopt:
	err = TRBADOPT;
	ptrace(("%s: %p: ERROR: bad options format\n", DRV_NAME, tcap));
	goto error;
      badaddr:
	err = TRBADADDR;
	ptrace(("%s: %p: ERROR: bad address format\n", DRV_NAME, tcap));
	goto error;
      badprim:
	err = TRBADPRIM;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tcap));
	goto error;
      error:
	return tr_error_ack(q, tcap, p->PRIM_type, err, 0);
}

/*
 *  TR_BEGIN_REQ
 *  -----------------------------------------------------------
 */
STATIC int
tr_begin_req(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	int err;
	struct TR_begin_req *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *dst = NULL;
	struct sccp_addr *org = NULL;
	struct tcap_opts opts = { 0, };
	struct tr *tr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (p->DEST_length) {
		if (mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
			goto badprim;
		if (p->DEST_length < sizeof(*dst))
			goto badaddr;
		dst = (typeof(dst)) (mp->b_rptr + p->DEST_offset);
		if (p->DEST_length != sizeof(*dst) + dst->alen)
			goto badaddr;
	}
	if (p->ORIG_length) {
		if (mp->b_wptr < mp->b_rptr + p->ORIG_offset + p->ORIG_length)
			goto badprim;
		if (p->ORIG_length < sizeof(*org))
			goto badaddr;
		org = (typeof(org)) (mp->b_rptr + p->ORIG_offset);
		if (p->ORIG_length != sizeof(*org) + org->alen)
			goto badaddr;
	}
	if (p->OPT_length) {
		if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
			goto badprim;
		if (tcap_parse_opts(tcap, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
	}
	if (!p->CORR_id || tr_lookup_cid(tcap, p->CORR_id))
		goto badcid;
	if (!(tr = tcap_alloc_tr(tcap, p->CORR_id, 0)))
		goto enomem;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badopt:
	err = TRBADOPT;
	ptrace(("%s: %p: ERROR: bad options format\n", DRV_NAME, tcap));
	goto error;
      badaddr:
	err = TRBADADDR;
	ptrace(("%s: %p: ERROR: bad address format\n", DRV_NAME, tcap));
	goto error;
      badprim:
	err = TRBADPRIM;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tcap));
	goto error;
      error:
	return tr_error_ack(q, tcap, p->PRIM_type, err, p->CORR_id);
}

/*
 *  TR_BEGIN_RES
 *  -----------------------------------------------------------
 */
STATIC int
tr_begin_res(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	int err;
	struct TR_begin_res *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *org = NULL;
	struct tcap_opts opts = { 0, };
	struct tr *tr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (p->ORIG_length) {
		if (mp->b_wptr < mp->b_rptr + p->ORIG_offset + p->ORIG_length)
			goto badprim;
		if (p->ORIG_length < sizeof(*org))
			goto badaddr;
		org = (typeof(org)) (mp->b_rptr + p->ORIG_offset);
		if (p->ORIG_length != sizeof(*org) + org->alen)
			goto badaddr;
	}
	if (p->OPT_length) {
		if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
			goto badprim;
		if (tcap_parse_opts(tcap, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
	}
	if (!(tr = tr_lookup_tid(tcap, p->TRANS_id)))
		goto badtid;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badtid:
	err = TRBADTID;
	ptrace(("%s: %p: ERROR: bad transaction id\n", DRV_NAME, tcap));
	goto error;
      badopt:
	err = TRBADOPT;
	ptrace(("%s: %p: ERROR: bad options format\n", DRV_NAME, tcap));
	goto error;
      badaddr:
	err = TRBADADDR;
	ptrace(("%s: %p: ERROR: bad address format\n", DRV_NAME, tcap));
	goto error;
      badprim:
	err = TRBADPRIM;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tcap));
	goto error;
      error:
	return tr_error_ack(q, tcap, p->PRIM_type, err, p->TRANS_id);
}

/*
 *  TR_CONT_REQ
 *  -----------------------------------------------------------
 */
STATIC int
tr_cont_req(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	int err;
	struct TR_cont_req *p = (typeof(p)) mp->b_rptr;
	struct tcap_opts opts = { 0, };
	struct tr *tr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (p->OPT_length) {
		if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
			goto badprim;
		if (tcap_parse_opts(tcap, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
	}
	if (!(tr = tr_lookup_tid(tcap, p->TRANS_id)))
		goto badtid;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badtid:
	err = TRBADTID;
	ptrace(("%s: %p: ERROR: bad transaction id\n", DRV_NAME, tcap));
	goto error;
      badopt:
	err = TRBADOPT;
	ptrace(("%s: %p: ERROR: bad options format\n", DRV_NAME, tcap));
	goto error;
      badprim:
	err = TRBADPRIM;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tcap));
	goto error;
      error:
	return tr_error_ack(q, tcap, p->PRIM_type, err, p->TRANS_id);
}

/*
 *  TR_END_REQ
 *  -----------------------------------------------------------
 */
STATIC int
tr_end_req(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	int err;
	struct TR_end_req *p = (typeof(p)) mp->b_rptr;
	struct tcap_opts opts = { 0, };
	ulong term;
	struct tr *tr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (p->OPT_length) {
		if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
			goto badprim;
		if (tcap_parse_opts(tcap, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
	}
	if (!(tr = tr_lookup_tid(tcap, p->TRANS_id)))
		goto badtid;
	term = p->TERM_scenario;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badtid:
	err = TRBADTID;
	ptrace(("%s: %p: ERROR: bad transaction id\n", DRV_NAME, tcap));
	goto error;
      badopt:
	err = TRBADOPT;
	ptrace(("%s: %p: ERROR: bad options format\n", DRV_NAME, tcap));
	goto error;
      badprim:
	err = TRBADPRIM;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tcap));
	goto error;
      error:
	return tr_error_ack(q, tcap, p->PRIM_type, err, p->TRANS_id);
}

/*
 *  TR_ABORT_REQ
 *  -----------------------------------------------------------
 */
STATIC int
tr_abort_req(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	int err;
	struct TR_abort_req *p = (typeof(p)) mp->b_rptr;
	struct tcap_opts opts = { 0, };
	ulong cause;
	struct tr *tr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (p->OPT_length) {
		if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
			goto badprim;
		if (tcap_parse_opts(tcap, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
	}
	if (!(tr = tr_lookup_tid(tcap, p->TRANS_id)))
		goto badtid;
	cause = p->ABORT_cause;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badtid:
	err = TRBADTID;
	ptrace(("%s: %p: ERROR: bad transaction id\n", DRV_NAME, tcap));
	goto error;
      badopt:
	err = TRBADOPT;
	ptrace(("%s: %p: ERROR: bad options format\n", DRV_NAME, tcap));
	goto error;
      badprim:
	err = TRBADPRIM;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tcap));
	goto error;
      error:
	return tr_error_ack(q, tcap, p->PRIM_type, err, p->TRANS_id);
}

/*
 *  TPI Primitives
 *  -----------------------------------------------------------
 */
/*
 *  T_CONN_REQ
 *  -----------------------------------------------------------
 */
STATIC int
t_conn_req(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	int err;
	struct T_conn_req *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *dst = NULL;
	struct tcap_opts opts = { 0UL, NULL, };
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (!p->DEST_length)
		goto noaddr;
	if (mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
		goto badprim;
	if (p->DEST_length < sizeof(*dst))
		goto badprim;
	dst = (typeof(dst)) (mp->b_rptr + p->DEST_offset);
	if (p->DEST_length != sizeof(*dst) + dst->alen)
		goto badaddr;
	if (p->OPT_length) {
		if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
			goto badprim;
		if (!tcap_parse_opts(tcap, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
	}
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badopt:
	err = TBADOPT;
	ptrace(("%s: %p: ERROR: bad options format\n", DRV_NAME, tcap));
	goto error;
      badaddr:
	err = TBADADDR;
	ptrace(("%s: %p: ERROR: bad address format\n", DRV_NAME, tcap));
	goto error;
      noaddr:
	err = TNOADDR;
	ptrace(("%s: %p: ERROR: could not assign address\n", DRV_NAME, tcap));
	goto error;
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tcap));
	goto error;
      error:
	return t_error_ack(q, tcap, p->PRIM_type, err);
}

/*
 *  T_CONN_RES
 *  -----------------------------------------------------------
 */
STATIC int
t_conn_res(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	int err;
	struct T_conn_res *p = (typeof(p)) mp->b_rptr;
	struct tcap_opts opts = { 0UL, NULL, };
	ulong tok, seq;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (p->OPT_length) {
		if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
			goto badprim;
		if (!tcap_parse_opts(tcap, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
	}
	tok = p->ACCEPTOR_id;
	seq = p->SEQ_number;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badopt:
	err = TBADOPT;
	ptrace(("%s: %p: ERROR: bad options format\n", DRV_NAME, tcap));
	goto error;
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tcap));
	goto error;
      error:
	return t_error_ack(q, tcap, p->PRIM_type, err);
}

/*
 *  T_DISCON_REQ
 *  -----------------------------------------------------------
 */
STATIC int
t_discon_req(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	int err;
	struct T_discon_req *p = (typeof(p)) mp->b_rptr;
	ulong seq;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	seq = p->SEQ_number;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tcap));
	goto error;
      error:
	return t_error_ack(q, tcap, p->PRIM_type, err);
}

/*
 *  T_DATA_REQ
 *  -----------------------------------------------------------
 */
STATIC int
t_data_req(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	int err;
	struct T_data_req *p = (typeof(p)) mp->b_rptr;
	ulong more;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	more = p->MORE_flag;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tcap));
	goto error;
      error:
	return t_error_ack(q, tcap, p->PRIM_type, err);
}

/*
 *  T_EXDATA_REQ
 *  -----------------------------------------------------------
 */
STATIC int
t_exdata_req(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	int err;
	struct T_exdata_req *p = (typeof(p)) mp->b_rptr;
	ulong more;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	more = p->MORE_flag;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tcap));
	goto error;
      error:
	return t_error_ack(q, tcap, p->PRIM_type, err);
}

/*
 *  T_INFO_REQ
 *  -----------------------------------------------------------
 */
STATIC int
t_info_req(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	int err;
	struct T_info_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tcap));
	goto error;
      error:
	return t_error_ack(q, tcap, p->PRIM_type, err);
}

/*
 *  T_BIND_REQ
 *  -----------------------------------------------------------
 */
STATIC int
t_bind_req(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	int err;
	struct T_bind_req *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *add = NULL;
	ulong cons;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (!p->ADDR_length)
		goto noaddr;
	if (mp->b_wptr < mp->b_rptr + p->ADDR_offset + p->ADDR_length)
		goto badprim;
	if (p->ADDR_length < sizeof(*add))
		goto badaddr;
	add = (typeof(add)) (mp->b_rptr + p->ADDR_offset);
	if (p->ADDR_length != sizeof(*add) + add->alen)
		goto badaddr;
	cons = p->CONIND_number;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badaddr:
	err = TBADADDR;
	ptrace(("%s: %p: ERROR: bad address format\n", DRV_NAME, tcap));
	goto error;
      noaddr:
	err = TNOADDR;
	ptrace(("%s: %p: ERROR: could not assign address\n", DRV_NAME, tcap));
	goto error;
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tcap));
	goto error;
      error:
	return t_error_ack(q, tcap, p->PRIM_type, err);
}

/*
 *  T_UNBIND_REQ
 *  -----------------------------------------------------------
 */
STATIC int
t_unbind_req(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	int err;
	struct T_unbind_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tcap));
	goto error;
      error:
	return t_error_ack(q, tcap, p->PRIM_type, err);
}

/*
 *  T_UNITDATA_REQ
 *  -----------------------------------------------------------
 */
STATIC int
t_unitdata_req(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	int err;
	struct T_unitdata_req *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *dst;
	struct tcap_opts opts = { 0UL, NULL, };
	if (tcap->ocls != 0)
		goto notsupport;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (!p->DEST_length)
		goto noaddr;
	if (mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
		goto badprim;
	if (p->DEST_length < sizeof(*dst))
		goto badaddr;
	dst = (typeof(dst)) (mp->b_rptr + p->DEST_offset);
	if (p->DEST_length != sizeof(*dst) + dst->alen)
		goto badaddr;
	if (p->OPT_length) {
		if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
			goto badprim;
		if (!tcap_parse_opts(tcap, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
	}
	/* 
	 *  Launch a UNIDIRECTIONAL to the destination address with the
	 *  options as specified.
	 */
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badopt:
	err = TBADOPT;
	ptrace(("%s: %p: ERROR: bad options format\n", DRV_NAME, tcap));
	goto error;
      badaddr:
	err = TBADADDR;
	ptrace(("%s: %p: ERROR: bad address format\n", DRV_NAME, tcap));
	goto error;
      noaddr:
	err = TNOADDR;
	ptrace(("%s: %p: ERROR: could not assign address\n", DRV_NAME, tcap));
	goto error;
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tcap));
	goto error;
      notsupport:
	err = TNOTSUPPORT;
	ptrace(("%s: %p: ERROR: primitive not supported for operations class\n", DRV_NAME, tcap));
	goto error;
      error:
	return t_error_ack(q, tcap, p->PRIM_type, err);
}

/*
 *  T_OPTMGMT_REQ
 *  -----------------------------------------------------------
 */
STATIC int
t_optmgmt_req(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	int err;
	struct T_optmgmt_req *p = (typeof(p)) mp->b_rptr;
	struct tcap_opts opts = { 0UL, NULL, };
	ulong flags;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (mp->OPT_length) {
		if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
			goto badprim;
		if (!tcap_parse_opts(tcap, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
	}
	switch ((flags = p->MGMT_flags)) {
	case T_CURRENT:
	case T_NEGOTIATE:
	case T_DEFAULT:
	case T_CHECK:
		break;
	default:
		goto badflag;
	}
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badflag:
	err = TBADFLAG;
	ptrace(("%s: %p: ERROR: bad flags\n", DRV_NAME, tcap));
	goto error;
      badopt:
	err = TBADOPT;
	ptrace(("%s: %p: ERROR: bad options format\n", DRV_NAME, tcap));
	goto error;
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tcap));
	goto error;
      error:
	return t_error_ack(q, tcap, p->PRIM_type, err);
}

/*
 *  T_ORDREL_REQ
 *  -----------------------------------------------------------
 */
STATIC int
t_ordrel_req(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	int err;
	struct T_ordrel_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tcap));
	goto error;
      error:
	return t_error_ack(q, tcap, p->PRIM_type, err);
}

#ifdef T_OPTDATA_REQ
/*
 *  T_OPTDATA_REQ
 *  -----------------------------------------------------------
 */
STATIC int
t_optdata_req(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	int err;
	struct T_optdata_req *p = (typeof(p)) mp->b_rptr;
	struct tcap_opts opts = { 0UL, NULL, };
	ulong flags;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (mp->OPT_length) {
		if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
			goto badprim;
		if (!tcap_parse_opts(tcap, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
	}
	flags = p->DATA_flag;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badopt:
	err = TBADOPT;
	ptrace(("%s: %p: ERROR: bad options format\n", DRV_NAME, tcap));
	goto error;
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tcap));
	goto error;
      error:
	return t_error_ack(q, tcap, p->PRIM_type, err);
}
#endif

#ifdef T_ADDR_REQ
/*
 *  T_ADDR_REQ
 *  -----------------------------------------------------------
 */
STATIC int
t_addr_req(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	int err;
	struct T_addr_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badprim:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, tcap));
	goto error;
      error:
	return t_error_ack(q, tcap, p->PRIM_type, err);
}
#endif

#ifdef T_CAPABILITY_REQ
/*
 *  T_CAPABILITY_REQ
 *  -----------------------------------------------------------
 */
STATIC int
t_capability_req(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	int err;
	struct T_capability_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	fixme(("Complete this function\n"));
	return (-EFAULT);
}
#endif

/*
 *  -------------------------------------------------------------------------
 *
 *  Primitives received from below (SCCP Provider).
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  NPI (SCCP) Primitives
 *  -----------------------------------------------------------
 */
/*
 *  N_CONN_IND
 *  -----------------------------------------------------------
 */
STATIC int
n_conn_ind(queue_t *q, struct sccp *sccp, mblk_t *mp)
{
	N_conn_ind_t *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *dst;
	N_qos_sel_conn_sccp_t qos = { N_QOS_SEL_CONN_SCCP, sccp->pcls };
	ulong flags;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (!p->DEST_length)
		goto noaddr;
	if (mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
		goto badprim;
	if (p->DEST_length < sizeof(*dst))
		goto badaddr;
	dst = (typeof(dst)) (mp->b_rptr + p->DEST_offset);
	if (p->DEST_length != sizeof(*dst) + dst->alen)
		goto badaddr;
	flags = p->CONN_flags;
	if (p->QOS_length) {
		if (mp->b_wptr < mp->b_rptr + p->QOS_offset + p->QOS_length)
			goto badprim;
		if (p->QOS_length < sizeof(qos))
			goto badqostype;
		bcopy(mp->b_rptr + p->QOS_offset, &qos, sizeof(qos));
		if (qos.n_qos_type != N_QOS_SEL_CONN_SCCP)
			goto badqostype;
		if (qos.protocol_class != 2 && qos.protocol_class != 3)
			goto badqosparam;
	}
	goto notsupport;
      notsupport:
	pswerr(("%s: %p: SWERR: unsupported primitive from below\n", DRV_NAME, sccp));
	goto error;
      badqosparam:
	pswerr(("%s: %p: SWERR: bad QOS parameter value\n", DRV_NAME, sccp));
	goto error;
      badqostype:
	pswerr(("%s: %p: SWERR: bad QOS structure type\n", DRV_NAME, sccp));
	goto error;
      badaddr:
	pswerr(("%s: %p: SWERR: bad address\n", DRV_NAME, sccp));
	goto error;
      noaddr:
	pswerr(("%s: %p: SWERR: could not assign address\n", DRV_NAME, sccp));
	goto error;
      badprim:
	pswerr(("%s: %p: SWERR: invalid primitive format\n", DRV_NAME, sccp));
	goto error;
      error:
	return sccp_hangup(q, sccp, -EPROTO);
}

/*
 *  N_CONN_CON
 *  -----------------------------------------------------------
 */
STATIC int
n_conn_con(queue_t *q, struct sccp *sccp, mblk_t *mp)
{
	N_conn_con_t *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *res = &sccp->rem;
	N_qos_sel_conn_sccp_t qos = { N_QOS_SEL_CONN_SCCP, sccp->pcls };
	ulong flags;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (p->RES_length) {
		if (mp->b_wptr < mp->b_rptr + p->RES_offset + p->RES_length)
			goto badprim;
		if (p->RES_length < sizeof(*res))
			goto badaddr;
		res = (typeof(res)) (mp->b_rptr + p->RES_offset);
		if (p->RES_length < sizeof(*res) + res->alen)
			goto badaddr;
	}
	flags = p->CONN_flags;
	if (p->QOS_length) {
		if (mp->b_wptr < mp->b_rptr + p->QOS_offset + p->QOS_length)
			goto badprim;
		if (p->QOS_length < sizeof(qos))
			goto badqostype;
		bcopy(mp->b_rptr + p->QOS_offset, &qos, sizeof(qos));
		if (qos.n_qos_type != N_QOS_SEL_CONN_SCCP)
			goto badqostype;
		if (qos.protocol_class != 2 && qos.protocol_class != 3)
			goto badqosparam;
	}
	goto notsupport;
      notsupport:
	pswerr(("%s: %p: SWERR: unsupported primitive from below\n", DRV_NAME, sccp));
	goto error;
      badqosparam:
	pswerr(("%s: %p: SWERR: bad QOS parameter value\n", DRV_NAME, sccp));
	goto error;
      badqostype:
	pswerr(("%s: %p: SWERR: bad QOS structure type\n", DRV_NAME, sccp));
	goto error;
      badaddr:
	pswerr(("%s: %p: SWERR: bad address\n", DRV_NAME, sccp));
	goto error;
      badprim:
	pswerr(("%s: %p: SWERR: invalid primitive format\n", DRV_NAME, sccp));
	goto error;
      error:
	return sccp_hangup(q, sccp, -EPROTO);
}

/*
 *  N_DISCON_IND
 *  -----------------------------------------------------------
 */
STATIC int
n_discon_ind(queue_t *q, struct sccp *sccp, mblk_t *mp)
{
	N_discon_ind_t *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *res = &sccp->rem;
	ulong orig, reason, seq;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (p->RES_length) {
		if (mp->b_wptr < mp->b_rptr + p->RES_offset + p->RES_length)
			goto badprim;
		if (p->RES_length < sizeof(*res))
			goto badaddr;
		res = (typeof(res)) (mp->b_rptr + p->RES_offset);
		if (p->RES_length < sizeof(*res) + res->alen)
			goto badaddr;
	}
	orig = p->DISCON_orig;
	reason = p->DISCON_reason;
	seq = p->SEQ_number;
	goto notsupport;
      notsupport:
	pswerr(("%s: %p: SWERR: unsupported primitive from below\n", DRV_NAME, sccp));
	goto error;
      badaddr:
	pswerr(("%s: %p: SWERR: bad address\n", DRV_NAME, sccp));
	goto error;
      badprim:
	pswerr(("%s: %p: SWERR: invalid primitive format\n", DRV_NAME, sccp));
	goto error;
      error:
	return sccp_hangup(q, sccp, -EPROTO);
}

/*
 *  N_DATA_IND
 *  -----------------------------------------------------------
 */
STATIC int
n_data_ind(queue_t *q, struct sccp *sccp, mblk_t *mp)
{
	N_data_ind_t *p = (typeof(p)) mp->b_rptr;
	ulong flags;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	flags = p->DATA_xfer_flags;
	goto notsupport;
      notsupport:
	pswerr(("%s: %p: SWERR: unsupported primitive from below\n", DRV_NAME, sccp));
	goto error;
      badprim:
	pswerr(("%s: %p: SWERR: invalid primitive format\n", DRV_NAME, sccp));
	goto error;
      error:
	return sccp_hangup(q, sccp, -EPROTO);
}

/*
 *  N_EXDATA_IND
 *  -----------------------------------------------------------
 */
STATIC int
n_exdata_ind(queue_t *q, struct sccp *sccp, mblk_t *mp)
{
	N_exdata_ind_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	goto notsupport;
      notsupport:
	pswerr(("%s: %p: SWERR: unsupported primitive from below\n", DRV_NAME, sccp));
	goto error;
      badprim:
	pswerr(("%s: %p: SWERR: invalid primitive format\n", DRV_NAME, sccp));
	goto error;
      error:
	return sccp_hangup(q, sccp, -EPROTO);
}

/*
 *  N_INFO_ACK
 *  -----------------------------------------------------------
 */
STATIC int
n_info_ack(queue_t *q, struct sccp *sccp, mblk_t *mp)
{
	N_info_ack_t *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *add = NULL;
	N_qos_sel_info_sccp_t qos;
	N_qos_range_info_sccp_t qor;
	uchar *pro = NULL;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (p->ADDR_length) {
		if (mp->b_wptr < mp->b_rptr + p->ADDR_offset + p->ADDR_length)
			goto badprim;
		if (p->ADDR_length < sizeof(*add))
			goto badaddr;
		add = (typeof(add)) (mp->b_rptr + p->ADDR_offset);
		if (p->ADDR_length < sizeof(*add) + add->alen)
			goto badaddr;
	}
	if (p->QOS_length) {
		if (mp->b_wptr < mp->b_rptr + p->QOS_offset + p->QOS_length)
			goto badprim;
		if (p->QOS_length < sizeof(qos))
			goto badqostype;
		bcopy(mp->b_rptr + p->QOS_offset, &qos, sizeof(qos));
		if (qos.n_qos_type != N_QOS_SEL_INFO_SCCP)
			goto badqostype;
		if (qos.protocol_class < 0 || qos.protocol_class > 3)
			goto badqosparam;
	}
	if (p->QOS_range_length) {
		if (mp->b_wptr < mp->b_rptr + p->QOS_range_offset + p->QOS_range_length)
			goto badprim;
		if (p->QOS_range_length < sizeof(qor))
			goto badqostype;
		bcopy(mp->b_rptr + p->QOS_range_offset, &qor, sizeof(qor));
		if (qor.n_qos_type != N_QOS_RANGE_INFO_SCCP)
			goto badqostype;
	}
	if (p->PROTOID_length) {
		if (mp->b_wptr < mp->b_rptr + p->PROTOID_offset + p->PROTOID_length)
			goto badprim;
		pro = (typeof(pro)) (mp->b_rptr + p->PROTOID_offset);
	}
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badqosparam:
	pswerr(("%s: %p: SWERR: bad QOS parameter value\n", DRV_NAME, sccp));
	goto error;
      badqostype:
	pswerr(("%s: %p: SWERR: bad QOS structure type\n", DRV_NAME, sccp));
	goto error;
      badaddr:
	pswerr(("%s: %p: SWERR: bad address\n", DRV_NAME, sccp));
	goto error;
      badprim:
	pswerr(("%s: %p: SWERR: invalid primitive format\n", DRV_NAME, sccp));
	goto error;
      error:
	return sccp_hangup(q, sccp, -EPROTO);
}

/*
 *  N_BIND_ACK
 *  -----------------------------------------------------------
 */
STATIC int
n_bind_ack(queue_t *q, struct sccp *sccp, mblk_t *mp)
{
	N_bind_ack_t *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *add;
	uchar *pro = NULL;
	ulong conind, tok;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (!p->ADDR_length)
		goto noaddr;
	if (mp->b_wptr < mp->b_rptr + p->ADDR_offset + p->ADDR_length)
		goto badprim;
	if (p->ADDR_length < sizeof(*add))
		goto badaddr;
	add = (typeof(add)) (mp->b_rptr + p->ADDR_offset);
	if (p->ADDR_length < sizeof(*add) + add->alen)
		goto badaddr;
	conind = p->CONIND_number;
	tok = p->TOKEN_value;
	if (p->PROTOID_length) {
		if (mp->b_wptr < mp->b_rptr + p->PROTOID_offset + p->PROTOID_length)
			goto badprim;
		pro = (typeof(pro)) (mp->b_rptr + p->PROTOID_offset);
	}
	goto notsupport;
      notsupport:
	pswerr(("%s: %p: SWERR: unsupported primitive from below\n", DRV_NAME, sccp));
	goto error;
      badaddr:
	pswerr(("%s: %p: SWERR: bad address\n", DRV_NAME, sccp));
	goto error;
      noaddr:
	pswerr(("%s: %p: SWERR: could not assign address\n", DRV_NAME, sccp));
	goto error;
      badprim:
	pswerr(("%s: %p: SWERR: invalid primitive format\n", DRV_NAME, sccp));
	goto error;
      error:
	return sccp_hangup(q, sccp, -EPROTO);
}

/*
 *  N_ERROR_ACK
 *  -----------------------------------------------------------
 */
STATIC int
n_error_ack(queue_t *q, struct sccp *sccp, mblk_t *mp)
{
	N_error_ack_t *p = (typeof(p)) mp->b_rptr;
	ulong prim, etype;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	prim = p->ERROR_prim;
	etype = (p->NPI_error == NSYSERR) ? -p->UNIX_error : p->NPI_error;
	goto notsupport;
      notsupport:
	pswerr(("%s: %p: SWERR: unsupported primitive from below\n", DRV_NAME, sccp));
	goto error;
      badprim:
	pswerr(("%s: %p: SWERR: invalid primitive format\n", DRV_NAME, sccp));
	goto error;
      error:
	return sccp_hangup(q, sccp, -EPROTO);
}

/*
 *  N_OK_ACK
 *  -----------------------------------------------------------
 */
STATIC int
n_ok_ack(queue_t *q, struct sccp *sccp, mblk_t *mp)
{
	N_ok_ack_t *p = (typeof(p)) mp->b_rptr;
	ulong prim;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	prim = p->CORRECT_prim;
	goto notsupport;
      notsupport:
	pswerr(("%s: %p: SWERR: unsupported primitive from below\n", DRV_NAME, sccp));
	goto error;
      badprim:
	pswerr(("%s: %p: SWERR: invalid primitive format\n", DRV_NAME, sccp));
	goto error;
      error:
	return sccp_hangup(q, sccp, -EPROTO);
}

/*
 *  N_UNITDATA_IND
 *  -----------------------------------------------------------
 */
STATIC int
n_unitdata_ind(queue_t *q, struct sccp *sccp, mblk_t *mp)
{
	N_unitdata_ind_t *p = (typeof(p)) mp->b_rptr;
	struct tr_msg msg;
	struct tcap_opts opts;
	struct sccp_addr *src;
	struct sccp_addr *dst = &sccp->loc;
	mblk_t *dp;
	size_t dlen;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (!p->SRC_length)
		goto noaddr;
	if (mp->b_wptr < mp->b_rptr + p->SRC_offset + p->SRC_length)
		goto badprim;
	if (p->SRC_length < sizeof(*src))
		goto badaddr;
	src = (typeof(src)) (mp->b_rptr + p->SRC_offset);
	if (p->SRC_length != sizeof(*src) + src->alen)
		goto badaddr;
	bcopy(src, &msg.sccp.orig, sizeof(*src) + src->alen);
	if (p->DEST_length) {
		if (mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
			goto badprim;
		if (p->DEST_length < sizeof(*dst))
			goto badaddr;
		dst = (typeof(dst)) (mp->b_rptr + p->DEST_offset);
		if (p->DEST_length != sizeof(*dst) + dst->alen)
			goto badaddr;
		if (dst->ni != sccp->loc.ni || dst->pc != sccp->loc.pc || dst->ssn != sccp->loc.ssn)
			goto badaddr;
	}
	bcopy(dst, &msg.sccp.dest, sizeof(*dst) + dst->alen);
	if (p->QOS_length) {
		N_qos_sel_data_sccp_t qos = { N_QOS_SEL_DATA_SCCP, };
		if (mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
			goto badprim;
		if (p->QOS_length < sizeof(qos))
			goto badqostype;
		bcopy(mp->b_rptr + QOS_offset, &qos, sizeof(qos));
		if (qos.n_qos_type != N_QOS_SEL_DATA_SCCP)
			goto badqostype;
		msg.sccp.pcl = qos.protocol_class;
		msg.sccp.ret = qos.option_flags;
		msg.sccp.imp = qos.importance;
		msg.sccp.seq = qos.sequence_control;
		msg.sccp.sls = qos.sequence_control;
		msg.sccp.mp = qos.message_priority;
	} else {
		msg.sccp.pcl = sccp->options.pcl;
		msg.sccp.ret = sccp->options.ret;
		msg.sccp.imp = sccp->options.imp;
		msg.sccp.seq = sccp->options.seq;
		msg.sccp.sls = sccp->options.sls;
		msg.sccp.mp = sccp->options.mp;
	}
	opts.pcl = &msg.sccp.pcl;
	opts.ret = &msg.sccp.ret;
	opts.imp = &msg.sccp.imp;
	opts.seq = &msg.sccp.seq;
	opts.sls = &msg.sccp.sls;
	opts.mp = &msg.sccp.mp;
	opts.flags = (TF_OPT_PCL | TF_OPT_RET | TF_OPT_IMP | TF_OPT_SEQ | TF_OPT_SLS | TF_OPT_MP);
	if (!(dp = mp->b_cont) || !(dlen = msgdsize(dp)) || dlen > sccp->prot.NSDU_size)
		goto baddata;
	if ((err = ss7_pullupmsg(dp, -1)) < 0)
		goto error;
	if ((err = tcap_dec_msg(dp->b_rtpr, dp->b_wptr, &msg)) < 0)
		goto error;
	if ((err = tcap_recv_msg(q, tcap, &opts, &msg, dp)) < 0)
		goto error;
	return (err);
      baddata:
	pswerr(("%s: %p: SWERR: bad data\n", DRV_NAME, sccp));
	goto error;
      badaddr:
	pswerr(("%s: %p: SWERR: bad address\n", DRV_NAME, sccp));
	goto error;
      noaddr:
	pswerr(("%s: %p: SWERR: could not assign address\n", DRV_NAME, sccp));
	goto error;
      badprim:
	pswerr(("%s: %p: SWERR: invalid primitive format\n", DRV_NAME, sccp));
	goto error;
      error:
	return sccp_hangup(q, sccp, -EPROTO);
}

/*
 *  N_UDERROR_IND
 *  -----------------------------------------------------------
 */
STATIC int
n_uderror_ind(queue_t *q, struct sccp *sccp, mblk_t *mp)
{
	N_uderror_ind_t *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *dst;
	ulong etype;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (!p->DEST_length)
		goto noaddr;
	if (mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
		goto badprim;
	if (p->DEST_length < sizeof(*dst))
		goto badaddr;
	dst = (typeof(dst)) (mp->b_rptr + p->DEST_offset);
	if (p->DEST_length < sizeof(*dst) + dst->alen)
		goto badaddr;
	etype = p->ERROR_type;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badaddr:
	pswerr(("%s: %p: SWERR: bad address\n", DRV_NAME, sccp));
	goto error;
      noaddr:
	pswerr(("%s: %p: SWERR: could not assign address\n", DRV_NAME, sccp));
	goto error;
      badprim:
	pswerr(("%s: %p: SWERR: invalid primitive format\n", DRV_NAME, sccp));
	goto error;
      error:
	return sccp_hangup(q, sccp, -EPROTO);
}

/*
 *  N_DATACK_IND
 *  -----------------------------------------------------------
 */
STATIC int
n_datack_ind(queue_t *q, struct sccp *sccp, mblk_t *mp)
{
	N_datack_ind_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	goto notsupport;
      notsupport:
	pswerr(("%s: %p: SWERR: unsupported primitive from below\n", DRV_NAME, sccp));
	goto error;
      badprim:
	pswerr(("%s: %p: SWERR: invalid primitive format\n", DRV_NAME, sccp));
	goto error;
      error:
	return sccp_hangup(q, sccp, -EPROTO);
}

/*
 *  N_RESET_IND
 *  -----------------------------------------------------------
 */
STATIC int
n_reset_ind(queue_t *q, struct sccp *sccp, mblk_t *mp)
{
	N_reset_ind_t *p = (typeof(p)) mp->b_rptr;
	ulong orig, reason;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	orig = p->RESET_orig;
	reason = p->RESET_reason;
	goto notsupport;
      notsupport:
	pswerr(("%s: %p: SWERR: unsupported primitive from below\n", DRV_NAME, sccp));
	goto error;
      badprim:
	pswerr(("%s: %p: SWERR: invalid primitive format\n", DRV_NAME, sccp));
	goto error;
      error:
	return sccp_hangup(q, sccp, -EPROTO);
}

/*
 *  N_RESET_CON
 *  -----------------------------------------------------------
 */
STATIC int
n_reset_con(queue_t *q, struct sccp *sccp, mblk_t *mp)
{
	N_reset_con_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	goto notsupport;
      notsupport:
	pswerr(("%s: %p: SWERR: unsupported primitive from below\n", DRV_NAME, sccp));
	goto error;
      badprim:
	pswerr(("%s: %p: SWERR: invalid primitive format\n", DRV_NAME, sccp));
	goto error;
      error:
	return sccp_hangup(q, sccp, -EPROTO);
}

/*
 *  N_COORD_IND
 *  -----------------------------------------------------------
 */
STATIC int
n_coord_ind(queue_t *q, struct sccp *sccp, mblk_t *mp)
{
	N_coord_ind_t *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *add;
	ulong smi;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (!p->ADDR_length)
		goto noaddr;
	if (mp->b_wptr < mp->b_rptr + p->ADDR_offset + p->ADDR_length)
		goto badprim;
	if (p->ADDR_length < sizeof(*add))
		goto badaddr;
	add = (typeof(add)) (mp->b_rptr + p->ADDR_offset);
	if (p->ADDR_length != sizeof(*add) + add->alen)
		goto badaddr;
	smi = p->SMI;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badaddr:
	pswerr(("%s: %p: SWERR: bad address\n", DRV_NAME, sccp));
	goto error;
      noaddr:
	pswerr(("%s: %p: SWERR: could not assign address\n", DRV_NAME, sccp));
	goto error;
      badprim:
	pswerr(("%s: %p: SWERR: invalid primitive format\n", DRV_NAME, sccp));
	goto error;
      error:
	return sccp_hangup(q, sccp, -EPROTO);
}

/*
 *  N_COORD_CON
 *  -----------------------------------------------------------
 */
STATIC int
n_coord_con(queue_t *q, struct sccp *sccp, mblk_t *mp)
{
	N_coord_con_t *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *add;
	ulong smi;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (!p->ADDR_length)
		goto noaddr;
	if (mp->b_wptr < mp->b_rptr + p->ADDR_offset + p->ADDR_length)
		goto badprim;
	if (p->ADDR_length < sizeof(*add))
		goto badaddr;
	add = (typeof(add)) (mp->b_rptr + p->ADDR_offset);
	if (p->ADDR_length != sizeof(*add) + add->alen)
		goto badaddr;
	smi = p->SMI;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badaddr:
	pswerr(("%s: %p: SWERR: bad address\n", DRV_NAME, sccp));
	goto error;
      noaddr:
	pswerr(("%s: %p: SWERR: could not assign address\n", DRV_NAME, sccp));
	goto error;
      badprim:
	pswerr(("%s: %p: SWERR: invalid primitive format\n", DRV_NAME, sccp));
	goto error;
      error:
	return sccp_hangup(q, sccp, -EPROTO);
}

/*
 *  N_STATE_IND
 *  -----------------------------------------------------------
 */
STATIC int
n_state_ind(queue_t *q, struct sccp *sccp, mblk_t *mp)
{
	N_state_ind_t *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *add;
	ulong status, smi;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (!p->ADDR_length)
		goto noaddr;
	if (mp->b_wptr < mp->b_rptr + p->ADDR_offset + p->ADDR_length)
		goto badprim;
	if (p->ADDR_length < sizeof(*add))
		goto badaddr;
	add = (typeof(add)) (mp->b_rptr + p->ADDR_offset);
	if (p->ADDR_length != sizeof(*add) + add->alen)
		goto badaddr;
	status = p->STATUS;
	smi = p->SMI;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badaddr:
	pswerr(("%s: %p: SWERR: bad address\n", DRV_NAME, sccp));
	goto error;
      noaddr:
	pswerr(("%s: %p: SWERR: could not assign address\n", DRV_NAME, sccp));
	goto error;
      badprim:
	pswerr(("%s: %p: SWERR: invalid primitive format\n", DRV_NAME, sccp));
	goto error;
      error:
	return sccp_hangup(q, sccp, -EPROTO);
}

/*
 *  N_PCSTATE_IND
 *  -----------------------------------------------------------
 */
STATIC int
n_pcstate_ind(queue_t *q, struct sccp *sccp, mblk_t *mp)
{
	N_pcstate_ind_t *p = (typeof(p)) mp->b_rptr;
	struct mtp_addr *add;
	ulong status;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (!p->ADDR_length)
		goto noaddr;
	if (mp->b_wptr < mp->b_rptr + p->ADDR_offset + p->ADDR_length)
		goto badprim;
	if (p->ADDR_length < sizeof(*add))
		goto badaddr;
	add = (typeof(add)) (mp->b_rptr + p->ADDR_offset);
	if (p->ADDR_length != sizeof(*add) + add->alen)
		goto badaddr;
	status = p->STATUS;
	fixme(("Complete this function\n"));
	return (-EFAULT);
      badaddr:
	pswerr(("%s: %p: SWERR: bad address\n", DRV_NAME, sccp));
	goto error;
      noaddr:
	pswerr(("%s: %p: SWERR: could not assign address\n", DRV_NAME, sccp));
	goto error;
      badprim:
	pswerr(("%s: %p: SWERR: invalid primitive format\n", DRV_NAME, sccp));
	goto error;
      error:
	return sccp_hangup(q, sccp, -EPROTO);
}

/*
 *  =========================================================================
 *
 *  I/O Controls
 *
 *  =========================================================================
 */

/*
 *  TCAP_IOCGOPTIONS
 *  -----------------------------------------------------------
 */
STATIC int
tcap_iocgoptions(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	swerr();
	return (-EFAULT);
}

/*
 *  TCAP_IOCSOPTIONS
 *  -----------------------------------------------------------
 */
STATIC int
tcap_iocsoptions(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	swerr();
	return (-EFAULT);
}

/*
 *  TCAP_IOCGCONFIG
 *  -----------------------------------------------------------
 */
STATIC int
tcap_iocgconfig(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	swerr();
	return (-EFAULT);
}

/*
 *  TCAP_IOCSCONFIG
 *  -----------------------------------------------------------
 */
STATIC int
tcap_iocsconfig(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	swerr();
	return (-EFAULT);
}

/*
 *  TCAP_IOCTCONFIG
 *  -----------------------------------------------------------
 */
STATIC int
tcap_ioctconfig(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	swerr();
	return (-EFAULT);
}

/*
 *  TCAP_IOCCCONFIG
 *  -----------------------------------------------------------
 */
STATIC int
tcap_ioccconfig(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	swerr();
	return (-EFAULT);
}

/*
 *  TCAP_IOCGSTATEM
 *  -----------------------------------------------------------
 */
STATIC int
tcap_iocgstatem(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	swerr();
	return (-EFAULT);
}

/*
 *  TCAP_IOCCMRESET
 *  -----------------------------------------------------------
 */
STATIC int
tcap_ioccmreset(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	swerr();
	return (-EFAULT);
}

/*
 *  TCAP_IOCGSTATSP
 *  -----------------------------------------------------------
 */
STATIC int
tcap_iocgstatsp(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	swerr();
	return (-EFAULT);
}

/*
 *  TCAP_IOCSSTATSP
 *  -----------------------------------------------------------
 */
STATIC int
tcap_iocsstatsp(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	swerr();
	return (-EFAULT);
}

/*
 *  TCAP_IOCGSTATS
 *  -----------------------------------------------------------
 */
STATIC int
tcap_iocgstats(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	swerr();
	return (-EFAULT);
}

/*
 *  TCAP_IOCCSTATS
 *  -----------------------------------------------------------
 */
STATIC int
tcap_ioccstats(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	swerr();
	return (-EFAULT);
}

/*
 *  TCAP_IOCGNOTIFY
 *  -----------------------------------------------------------
 */
STATIC int
tcap_iocgnotify(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	swerr();
	return (-EFAULT);
}

/*
 *  TCAP_IOCSNOTIFY
 *  -----------------------------------------------------------
 */
STATIC int
tcap_iocsnotify(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	swerr();
	return (-EFAULT);
}

/*
 *  TCAP_IOCCNOTIFY
 *  -----------------------------------------------------------
 */
STATIC int
tcap_ioccnotify(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	swerr();
	return (-EFAULT);
}

/*
 *  TCAP_IOCCMGMT
 *  -----------------------------------------------------------
 */
STATIC int
tcap_ioccmgmt(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	swerr();
	return (-EFAULT);
}

/*
 *  TCAP_IOCCPASS
 *  -----------------------------------------------------------
 */
STATIC int
tcap_ioccpass(queue_t *q, struct tcap *tcap, mblk_t *mp)
{
	swerr();
	return (-EFAULT);
}

/*
 *  =========================================================================
 *
 *  STREAMS Message Handling
 *
 *  =========================================================================
 */
/*
 *  -------------------------------------------------------------------------
 *
 *  M_IOCTL Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
tcap_w_ioctl(queue_t *q, mblk_t *mp)
{
	struct tcap *tcap = TCAP_PRIV(q);
	struct iocblk *iocp = (typeof(iocp)) mp->b_rptr;
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	int cmd = iocp->ioc_cmd, count = iocp->ioc_count;
	int type = _IOC_TYPE(cmd), nr = _IOC_NR(cmd), size = _IOC_SIZE(cmd);
	int ret = 0;
	switch (type) {
	case _IOC_TYPE(__SID):
	{
		psw_t flags;
		struct sccp *sccp, **sccpp;
		struct linkblk *lb;
		if (!(lb = arg)) {
			swerr();
			ret = -EINVAL;
			break;
		}
		switch (nr) {
		case _IOC_NR(I_PLINK):
			ptrace(("%s: %p: I_PLINK\n", DRV_NAME, tcap));
			if (iocp->ioc_cr->cr_uid != 0) {
				ptrace(("%s: %p: ERROR: Non-root attempt to I_PLINK\n", DRV_NAME,
					tcap));
				ret = -EPERM;
				break;
			}
		case _IOC_NR(I_LINK):
			ptrace(("%s: %p: I_LINK\n", DRV_NAME, tcap));
			MOD_INC_USE_COUNT;	/* keep module from unloading */
			spin_lock_irqsave(&master.lock, flags);
			{
				/* place in list in ascending index order */
				for (sccpp = &master.sccp.list;
				     *sccpp && (*sccpp)->u.mux.index < lb->l_index;
				     sccpp = &(*sccpp)->next) ;
				if ((sccp =
				     tcap_alloc_sccp(lb->l_qbot, sccpp, lb->l_index,
						     iocp->ioc_cr))) {
					spin_unlock_irqrestore(&master.lock, flags);
					break;
				}
				MOD_DEC_USE_COUNT;
				ret = -ENOMEM;
			}
			spin_unlock_irqrestore(&master.lock, flags);
			break;
		case _IOC_NR(I_PUNLINK):
			ptrace(("%s: %p: I_PUNLINK\n", DRV_NAME, tcap));
			if (iocp->ioc_cr->cr_uid != 0) {
				ptrace(("%s: %p: ERROR: Non-root attempt to I_PUNLINK\n", DRV_NAME,
					tcap));
				ret = -EPERM;
				break;
			}
		case _IOC_NR(I_UNLINK):
			ptrace(("%s: %p: I_UNLINK\n", DRV_NAME, tcap));
			spin_lock_irqsave(&master.lock, flags);
			{
				for (sccp = master.sccp.list; sccp; sccp = sccp->next)
					if (sccp->u.mux.index == lb->l_index)
						break;
				if (!sccp) {
					ret = -EINVAL;
					ptrace(("%s: %p: ERROR: Couldn't find I_UNLINK muxid\n",
						DRV_NAME, tcap));
					spin_unlock_irqrestore(&master.lock, flags);
					break;
				}
				tcap_free_sccp(sccp->iq);
				MOD_DEC_USE_COUNT;
			}
			spin_unlock_irqrestore(&master.lock, flags);
			break;
		default:
		case _IOC_NR(I_STR):
			ptrace(("%s: %p: ERROR: Unsupported STREAMS ioctl %c, %d\n", DRV_NAME, tcap,
				(char) type, nr));
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	}
	case TCAP_IOC_MAGIC:
	{
		if (count < size) {
			ret = -EINVAL;
			break;
		}
		switch (nr) {
		case _IOC_NR(TCAP_IOCGOPTIONS):
			printd(("%s; %p: -> TCAP_IOCGOPTIONS\n", DRV_NAME, tcap));
			ret = tcap_iocgoptions(q, tcap, mp);
			break;
		case _IOC_NR(TCAP_IOCSOPTIONS):
			printd(("%s; %p: -> TCAP_IOCSOPTIONS\n", DRV_NAME, tcap));
			ret = tcap_iocsoptions(q, tcap, mp);
			break;
		case _IOC_NR(TCAP_IOCGCONFIG):
			printd(("%s; %p: -> TCAP_IOCGCONFIG\n", DRV_NAME, tcap));
			ret = tcap_iocgconfig(q, tcap, mp);
			break;
		case _IOC_NR(TCAP_IOCSCONFIG):
			printd(("%s; %p: -> TCAP_IOCSCONFIG\n", DRV_NAME, tcap));
			ret = tcap_iocsconfig(q, tcap, mp);
			break;
		case _IOC_NR(TCAP_IOCTCONFIG):
			printd(("%s; %p: -> TCAP_IOCTCONFIG\n", DRV_NAME, tcap));
			ret = tcap_ioctconfig(q, tcap, mp);
			break;
		case _IOC_NR(TCAP_IOCCCONFIG):
			printd(("%s; %p: -> TCAP_IOCCCONFIG\n", DRV_NAME, tcap));
			ret = tcap_ioccconfig(q, tcap, mp);
			break;
		case _IOC_NR(TCAP_IOCGSTATEM):
			printd(("%s; %p: -> TCAP_IOCGSTATEM\n", DRV_NAME, tcap));
			ret = tcap_iocgstatem(q, tcap, mp);
			break;
		case _IOC_NR(TCAP_IOCCMRESET):
			printd(("%s; %p: -> TCAP_IOCCMRESET\n", DRV_NAME, tcap));
			ret = tcap_ioccmreset(q, tcap, mp);
			break;
		case _IOC_NR(TCAP_IOCGSTATSP):
			printd(("%s; %p: -> TCAP_IOCGSTATSP\n", DRV_NAME, tcap));
			ret = tcap_iocgstatsp(q, tcap, mp);
			break;
		case _IOC_NR(TCAP_IOCSSTATSP):
			printd(("%s; %p: -> TCAP_IOCSSTATSP\n", DRV_NAME, tcap));
			ret = tcap_iocsstatsp(q, tcap, mp);
			break;
		case _IOC_NR(TCAP_IOCGSTATS):
			printd(("%s; %p: -> TCAP_IOCGSTATS\n", DRV_NAME, tcap));
			ret = tcap_iocgstats(q, tcap, mp);
			break;
		case _IOC_NR(TCAP_IOCCSTATS):
			printd(("%s; %p: -> TCAP_IOCCSTATS\n", DRV_NAME, tcap));
			ret = tcap_ioccstats(q, tcap, mp);
			break;
		case _IOC_NR(TCAP_IOCGNOTIFY):
			printd(("%s; %p: -> TCAP_IOCGNOTIFY\n", DRV_NAME, tcap));
			ret = tcap_iocgnotify(q, tcap, mp);
			break;
		case _IOC_NR(TCAP_IOCSNOTIFY):
			printd(("%s; %p: -> TCAP_IOCSNOTIFY\n", DRV_NAME, tcap));
			ret = tcap_iocsnotify(q, tcap, mp);
			break;
		case _IOC_NR(TCAP_IOCCNOTIFY):
			printd(("%s; %p: -> TCAP_IOCCNOTIFY\n", DRV_NAME, tcap));
			ret = tcap_ioccnotify(q, tcap, mp);
			break;
		case _IOC_NR(TCAP_IOCCMGMT):
			printd(("%s; %p: -> TCAP_IOCCMGMT\n", DRV_NAME, tcap));
			ret = tcap_ioccmgmt(q, tcap, mp);
			break;
		case _IOC_NR(TCAP_IOCCPASS):
			printd(("%s; %p: -> TCAP_IOCCPASS\n", DRV_NAME, tcap));
			ret = tcap_ioccpass(q, tcap, mp);
			break;
		default:
			ptrace(("%s: %p: ERROR: Unsupported TCAP ioctl %c, %d\n", DRV_NAME, tcap,
				(char) type, nr));
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	}
	default:
		ptrace(("%s: %p: ERROR: Unsupported ioctl %c, %d\n", DRV_NAME, tcap, (char) type,
			nr));
		ret = -EOPNOTSUPP;
		break;
	}
	if (ret > 0) {
		return (ret);
	} else if (ret == 0) {
		mp->b_datap->db_type = M_IOCACK;
		iocp->ioc_error = 0;
		iocp->ioc_rval = 0;
	} else {
		mp->b_datap->db_type = M_IOCNAK;
		iocp->ioc_error = -ret;
		iocp->ioc_rval = -1;
	}
	qreply(q, mp);
	return (QR_ABSORBED);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_PROTO, M_PCPROTO Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sccp_r_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	struct sccp *sccp = SCCP_PRIV(q);
	ulong oldstate = sccp_get_i_state(sccp);
	switch (*(ulong *) mp->b_rptr) {
	case N_CONN_IND:
		printd(("%s: %p: N_CONN_IND <-\n", DRV_NAME, sccp));
		rtn = n_conn_ind(q, sccp, mp);
		break;
	case N_CONN_CON:
		printd(("%s: %p: N_CONN_CON <-\n", DRV_NAME, sccp));
		rtn = n_conn_con(q, sccp, mp);
		break;
	case N_DISCON_IND:
		printd(("%s: %p: N_DISCON_IND <-\n", DRV_NAME, sccp));
		rtn = n_discon_ind(q, sccp, mp);
		break;
	case N_DATA_IND:
		printd(("%s: %p: N_DATA_IND <-\n", DRV_NAME, sccp));
		rtn = n_data_ind(q, sccp, mp);
		break;
	case N_EXDATA_IND:
		printd(("%s: %p: N_EXDATA_IND <-\n", DRV_NAME, sccp));
		rtn = n_exdata_ind(q, sccp, mp);
		break;
	case N_INFO_ACK:
		printd(("%s: %p: N_INFO_ACK <-\n", DRV_NAME, sccp));
		rtn = n_info_ack(q, sccp, mp);
		break;
	case N_BIND_ACK:
		printd(("%s: %p: N_BIND_ACK <-\n", DRV_NAME, sccp));
		rtn = n_bind_ack(q, sccp, mp);
		break;
	case N_ERROR_ACK:
		printd(("%s: %p: N_ERROR_ACK <-\n", DRV_NAME, sccp));
		rtn = n_error_ack(q, sccp, mp);
		break;
	case N_OK_ACK:
		printd(("%s: %p: N_OK_ACK <-\n", DRV_NAME, sccp));
		rtn = n_ok_ack(q, sccp, mp);
		break;
	case N_UNITDATA_IND:
		printd(("%s: %p: N_UNITDATA_IND <-\n", DRV_NAME, sccp));
		rtn = n_unitdata_ind(q, sccp, mp);
		break;
	case N_UDERROR_IND:
		printd(("%s: %p: N_UDERROR_IND <-\n", DRV_NAME, sccp));
		rtn = n_uderror_ind(q, sccp, mp);
		break;
	case N_DATACK_IND:
		printd(("%s: %p: N_DATACK_IND <-\n", DRV_NAME, sccp));
		rtn = n_datack_ind(q, sccp, mp);
		break;
	case N_RESET_IND:
		printd(("%s: %p: N_RESET_IND <-\n", DRV_NAME, sccp));
		rtn = n_reset_ind(q, sccp, mp);
		break;
	case N_RESET_CON:
		printd(("%s: %p: N_RESET_CON <-\n", DRV_NAME, sccp));
		rtn = n_reset_con(q, sccp, mp);
		break;
	case N_COORD_IND:
		printd(("%s: %p: N_COORD_IND <-\n", DRV_NAME, sccp));
		rtn = n_coord_ind(q, sccp, mp);
		break;
	case N_COORD_CON:
		printd(("%s: %p: N_COORD_CON <-\n", DRV_NAME, sccp));
		rtn = n_coord_con(q, sccp, mp);
		break;
	case N_STATE_IND:
		printd(("%s: %p: N_STATE_IND <-\n", DRV_NAME, sccp));
		rtn = n_state_ind(q, sccp, mp);
		break;
	case N_PCSTATE_IND:
		printd(("%s: %p: N_PCSTATE_IND <-\n", DRV_NAME, sccp));
		rtn = n_pcstate_ind(q, sccp, mp);
		break;
	default:
		printd(("%s: %p: ???? <-\n", DRV_NAME, sccp));
		rtn = -EFAULT;
		break;
	}
	if (rtn < 0)
		sccp_set_i_state(sccp, oldstate);
	return (rtn);
}
STATIC int
tcap_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	struct tcap *tcap = TCAP_PRIV(q);
	switch (tcap->i_style) {
	case TCAP_STYLE_TCI:
	{
		ulong oldstate = tcap_get_c_state(tcap);
		switch (*(ulong *) mp->b_rptr) {
		case TC_INFO_REQ:
			printd(("%s: %p: -> TC_INFO_REQ\n", DRV_NAME, tcap));
			rtn = tc_info_req(q, tcap, mp);
			break;
		case TC_BIND_REQ:
			printd(("%s: %p: -> TC_BIND_REQ\n", DRV_NAME, tcap));
			rtn = tc_bind_req(q, tcap, mp);
			break;
		case TC_UNBIND_REQ:
			printd(("%s: %p: -> TC_UNBIND_REQ\n", DRV_NAME, tcap));
			rtn = tc_unbind_req(q, tcap, mp);
			break;
		case TC_SUBS_BIND_REQ:
			printd(("%s: %p: -> TC_SUBS_BIND_REQ\n", DRV_NAME, tcap));
			rtn = tc_subs_bind_req(q, tcap, mp);
			break;
		case TC_SUBS_UNBIND_REQ:
			printd(("%s: %p: -> TC_SUBS_UNBIND_REQ\n", DRV_NAME, tcap));
			rtn = tc_subs_unbind_req(q, tcap, mp);
			break;
		case TC_OPTMGMT_REQ:
			printd(("%s: %p: -> TC_OPTMGMT_REQ\n", DRV_NAME, tcap));
			rtn = tc_optmgmt_req(q, tcap, mp);
			break;
		case TC_UNI_REQ:
			printd(("%s: %p: -> TC_UNI_REQ\n", DRV_NAME, tcap));
			rtn = tc_uni_req(q, tcap, mp);
			break;
		case TC_BEGIN_REQ:
			printd(("%s: %p: -> TC_BEGIN_REQ\n", DRV_NAME, tcap));
			rtn = tc_begin_req(q, tcap, mp);
			break;
		case TC_BEGIN_RES:
			printd(("%s: %p: -> TC_BEGIN_RES\n", DRV_NAME, tcap));
			rtn = tc_begin_res(q, tcap, mp);
			break;
		case TC_CONT_REQ:
			printd(("%s: %p: -> TC_CONT_REQ\n", DRV_NAME, tcap));
			rtn = tc_cont_req(q, tcap, mp);
			break;
		case TC_END_REQ:
			printd(("%s: %p: -> TC_END_REQ\n", DRV_NAME, tcap));
			rtn = tc_end_req(q, tcap, mp);
			break;
		case TC_ABORT_REQ:
			printd(("%s: %p: -> TC_ABORT_REQ\n", DRV_NAME, tcap));
			rtn = tc_abort_req(q, tcap, mp);
			break;
		case TC_INVOKE_REQ:
			printd(("%s: %p: -> TC_INVOKE_REQ\n", DRV_NAME, tcap));
			rtn = tc_invoke_req(q, tcap, mp);
			break;
		case TC_RESULT_REQ:
			printd(("%s: %p: -> TC_RESULT_REQ\n", DRV_NAME, tcap));
			rtn = tc_result_req(q, tcap, mp);
			break;
		case TC_ERROR_REQ:
			printd(("%s: %p: -> TC_ERROR_REQ\n", DRV_NAME, tcap));
			rtn = tc_error_req(q, tcap, mp);
			break;
		case TC_CANCEL_REQ:
			printd(("%s: %p: -> TC_CANCEL_REQ\n", DRV_NAME, tcap));
			rtn = tc_cancel_req(q, tcap, mp);
			break;
		case TC_REJECT_REQ:
			printd(("%s: %p: -> TC_REJECT_REQ\n", DRV_NAME, tcap));
			rtn = tc_reject_req(q, tcap, mp);
			break;
		default:
			printd(("%s: %p: -> ????\n", DRV_NAME, tcap));
			rtn = -EPROTO;
			break;
		}
		if (rtn < 0)
			tcap_set_c_state(tcap, oldstate);
		break;
	}
	case TCAP_STYLE_TRI:
	{
		ulong oldstate = tcap_get_r_state(tcap);
		switch (*(ulong *) mp->b_rptr) {
		case TR_INFO_REQ:
			printd(("%s: %p: -> TR_INFO_REQ\n", DRV_NAME, tcap));
			rtn = tr_info_req(q, tcap, mp);
			break;
		case TR_BIND_REQ:
			printd(("%s: %p: -> TR_BIND_REQ\n", DRV_NAME, tcap));
			rtn = tr_bind_req(q, tcap, mp);
			break;
		case TR_UNBIND_REQ:
			printd(("%s: %p: -> TR_UNBIND_REQ\n", DRV_NAME, tcap));
			rtn = tr_unbind_req(q, tcap, mp);
			break;
		case TR_OPTMGMT_REQ:
			printd(("%s: %p: -> TR_OPTMGMT_REQ\n", DRV_NAME, tcap));
			rtn = tr_optmgmt_req(q, tcap, mp);
			break;
		case TR_UNI_REQ:
			printd(("%s: %p: -> TR_UNI_REQ\n", DRV_NAME, tcap));
			rtn = tr_uni_req(q, tcap, mp);
			break;
		case TR_BEGIN_REQ:
			printd(("%s: %p: -> TR_BEGIN_REQ\n", DRV_NAME, tcap));
			rtn = tr_begin_req(q, tcap, mp);
			break;
		case TR_BEGIN_RES:
			printd(("%s: %p: -> TR_BEGIN_RES\n", DRV_NAME, tcap));
			rtn = tr_begin_res(q, tcap, mp);
			break;
		case TR_CONT_REQ:
			printd(("%s: %p: -> TR_CONT_REQ\n", DRV_NAME, tcap));
			rtn = tr_cont_req(q, tcap, mp);
			break;
		case TR_END_REQ:
			printd(("%s: %p: -> TR_END_REQ\n", DRV_NAME, tcap));
			rtn = tr_end_req(q, tcap, mp);
			break;
		case TR_ABORT_REQ:
			printd(("%s: %p: -> TR_ABORT_REQ\n", DRV_NAME, tcap));
			rtn = tr_abort_req(q, tcap, mp);
			break;
		default:
			printd(("%s: %p: -> ????\n", DRV_NAME, tcap));
			rtn = -EPROTO;
			break;
		}
		if (rtn < 0)
			tcap_set_r_state(tcap, oldstate);
		break;
	}
	case TCAP_STYLE_TPI:
	{
		ulong oldstate = tcap_get_t_state(tcap);
		switch (*(ulong *) mp->b_rptr) {
		case T_CONN_REQ:
			printd(("%s: %p: -> T_CONN_REQ\n", DRV_NAME, tcap));
			rtn = t_conn_req(q, tcap, mp);
			break;
		case T_CONN_RES:
			printd(("%s: %p: -> T_CONN_RES\n", DRV_NAME, tcap));
			rtn = t_conn_res(q, tcap, mp);
			break;
		case T_DISCON_REQ:
			printd(("%s: %p: -> T_DISCON_REQ\n", DRV_NAME, tcap));
			rtn = t_discon_req(q, tcap, mp);
			break;
		case T_DATA_REQ:
			printd(("%s: %p: -> T_DATA_REQ\n", DRV_NAME, tcap));
			rtn = t_data_req(q, tcap, mp);
			break;
		case T_EXDATA_REQ:
			printd(("%s: %p: -> T_EXDATA_REQ\n", DRV_NAME, tcap));
			rtn = t_exdata_req(q, tcap, mp);
			break;
		case T_INFO_REQ:
			printd(("%s: %p: -> T_INFO_REQ\n", DRV_NAME, tcap));
			rtn = t_info_req(q, tcap, mp);
			break;
		case T_BIND_REQ:
			printd(("%s: %p: -> T_BIND_REQ\n", DRV_NAME, tcap));
			rtn = t_bind_req(q, tcap, mp);
			break;
		case T_UNBIND_REQ:
			printd(("%s: %p: -> T_UNBIND_REQ\n", DRV_NAME, tcap));
			rtn = t_unbind_req(q, tcap, mp);
			break;
		case T_UNITDATA_REQ:
			printd(("%s: %p: -> T_UNITDATA_REQ\n", DRV_NAME, tcap));
			rtn = t_unitdata_req(q, tcap, mp);
			break;
		case T_OPTMGMT_REQ:
			printd(("%s: %p: -> T_OPTMGMT_REQ\n", DRV_NAME, tcap));
			rtn = t_optmgmt_req(q, tcap, mp);
			break;
		case T_ORDREL_REQ:
			printd(("%s: %p: -> T_ORDREL_REQ\n", DRV_NAME, tcap));
			rtn = t_ordrel_req(q, tcap, mp);
			break;
		case T_OPTDATA_REQ:
			printd(("%s: %p: -> T_OPTDATA_REQ\n", DRV_NAME, tcap));
			rtn = t_optdata_req(q, tcap, mp);
			break;
		case T_ADDR_REQ:
			printd(("%s: %p: -> T_ADDR_REQ\n", DRV_NAME, tcap));
			rtn = t_addr_req(q, tcap, mp);
			break;
		case T_CAPABILITY_REQ:
			printd(("%s: %p: -> T_CAPABILITY_REQ\n", DRV_NAME, tcap));
			rtn = t_capability_req(q, tcap, mp);
			break;
		default:
			printd(("%s: %p: -> ???\n", DRV_NAME, tcap));
			rtn = -EPROTO;
			break;
		}
		if (rtn < 0)
			tcap_set_t_state(tcap, oldstate);
		break;
	}
	case TCAP_STYLE_MGMT:
	{
		ulong oldstate = tcap_get_m_state(tcap);
		switch (*(ulong *) mp->b_rptr) {
		}
		if (rtn < 0)
			tcap_set_m_state(tcap, oldstate);
		break;
	}
	default:
		swerr();
		rtn = -EPROTO;
		break;
	}
	return (rtn);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_DATA Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sccp_r_data(queue_t *q, mblk_t *mp)
{
	struct sccp *sccp = SCCP_PRIV(q);
	return n_data(q, sccp, mp);
}
STATIC int
tcap_w_data(queue_t *q, mblk_t *mp)
{
	struct tcap *tcap = TCAP_PRIV(q);
	switch (tcap->i_style) {
	case TCAP_STYLE_TRI:
		return tr_data(q, tcap, mp);
	case TCAP_STYLE_TCI:
		return tc_data(q, tcap, mp);
	case TCAP_STYLE_TPI:
		return t_data(q, tcap, mp);
	case TCAP_STYLE_MGMT:
		return mgm_data(q, tcap, mp);
	default:
		swerr();
		return (-EFAULT);
	}
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_ERROR, M_HANGUP Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
sccp_r_error(queue_t *q, mblk_t *mp)
{
	struct sccp *sccp = SCCP_PRIV(q);
	return sccp_hangup(q, sccp);
}
STATIC INLINE int
sccp_r_hangup(queue_t *q, mblk_t *mp)
{
	struct sccp *sccp = SCCP_PRIV(q);
	return sccp_hangup(q, sccp);
}

/*
 *  =========================================================================
 *
 *  PUT and SRV
 *
 *  =========================================================================
 */
STATIC INLINE int
tcap_r_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_FLUSH:
		return ss7_r_flush(q, mp);
	}
	return (QR_PASSFLOW);
}
STATIC INLINE int
tcap_w_prim(queue_t *q, mblk_t *mp)
{
	/* Fast Path */
	if (mp->b_datap->db_type == M_DATA)
		return tcap_w_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return tcap_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return tcap_w_proto(q, mp);
	case M_FLUSH:
		return ss7_w_flush(q, mp);
	case M_IOCTL:
		return tcap_w_ioctl(q, mp);
	}
	seldom();
	return (QR_PASSFLOW);
}
STATIC INLINE int
sccp_r_prim(queue_t *q, mblk_t *mp)
{
	/* Fast Path */
	if (mp->b_datap->db_type == M_DATA)
		return sccp_r_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return sccp_r_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return sccp_r_proto(q, mp);
	case M_FLUSH:
		return ss7_r_flush(q, mp);
	case M_ERROR:
		return sccp_r_error(q, mp);
	case M_HANGUP:
		return sccp_r_hangup(q, mp);
	}
	seldom();
	return (QR_PASSFLOW);
}
STATIC INLINE int
sccp_w_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_FLUSH:
		return ss7_w_flush(q, mp);
	}
	return (QR_PASSFLOW);
}

/*
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 */
STATIC int tcap_majors[CMAJORS] = { CMAJOR_0, };

/*
 *  OPEN
 *  -------------------------------------------------------------------------
 */
STATIC int
tcap_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	psw_t flags;
	int mindex = 0;
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);
	minor_t bminor = cminor;
	struct tcap *tcap, **tcapp = &master.tcap.list;
	MOD_INC_USE_COUNT;	/* keep module from unloading */
	if (q->q_ptr != NULL) {
		MOD_DEC_USE_COUNT;
		return (0);	/* already open */
	}
	if (sflag == MODOPEN || WR(q)->q_next) {
		ptrace(("%s: ERROR: cannot push as module\n", DRV_NAME));
		MOD_DEC_USE_COUNT;
		return (EIO);
	}
	if (cmajor != CMAJOR_0 || cminor >= TCAP_CMINOR_FREE) {
		MOD_DEC_USE_COUNT;
		return (ENXIO);
	}
	/* allocate a new device */
	cminor = TCAP_CMINOR_FREE;
	spin_lock_irqsave(&master.lock, flags);
	for (; *tcapp; tcapp = &(*tcapp)->next) {
		major_t dmajor = (*tcapp)->u.dev.cmajor;
		if (cmajor != dmajor)
			break;
		if (cmajor == dmajor) {
			minor_t dminor = (*tcapp)->u.dev.cminor;
			if (cminor < dminor)
				break;
			if (cminor > dminor)
				continue;
			if (cminor == dminor) {
				if (++cminor >= NMINORS) {
					if (++mindex >= CMAJORS || !(cmajor = tcap_majors[mindex]))
						break;
					cminor = 0;
				}
				continue;
			}
		}
	}
	if (mindex >= CMAJORS || !cmajor) {
		ptrace(("%s: ERROR: no device numbers available\n", DRV_NAME));
		spin_unlock_irqrestore(&master.lock, flags);
		MOD_DEC_USE_COUNT;
		return (ENXIO);
	}
	printd(("%s: opened character device %d:%d\n", DRV_NAME, cmajor, cminor));
	*devp = makedevice(cmajor, cminor);
	if (!(tcap = tcap_alloc_priv(q, tcapp, devp, crp, bminor))) {
		ptrace(("%s: ERROR: no memory\n", DRV_NAME));
		spin_unlock_irqrestore(&master.lock, flags);
		MOD_DEC_USE_COUNT;
		return (ENOMEM);
	}
	spin_unlock_irqrestore(&master.lock, flags);
	return (0);
}

/*
 *  CLOSE
 *  -------------------------------------------------------------------------
 */
STATIC int
tcap_close(queue_t *q, int flag, cred_t *crp)
{
	struct tcap *tcap = TCAP_PRIV(q);
	psw_t flags;
	(void) flag;
	(void) crp;
	(void) tcap;
	printd(("%s: closing character device %d:%d\n", DRV_NAME, tcap->u.dev.cmajor,
		tcap->u.dev.cminor));
	spin_lock_irqsave(&master.lock, flags);
	tcap_free_priv(tcap);
	spin_unlock_irqrestore(&master.lock, flags);
	MOD_DEC_USE_COUNT;
	return (0);
}

/*
 *  =========================================================================
 *
 *  Allocation and deallocation
 *
 *  =========================================================================
 */

/*
 *  TCAP Structure
 *  -----------------------------------
 */
STATIC struct tcap *
tcap_alloc_priv(queue_t *, struct tcap **, dev_t *, cred_t *, ushort)
{
}
STATIC struct tcap *
tcap_get(struct tcap *)
{
}
STATIC struct tcap *
tcap_lookup(ulong)
{
}
STATIC ulong
tcap_get_id(ulong)
{
}
STATIC void
tcap_free_priv(struct tcap *)
{
}
STATIC void
tcap_put(struct tcap *)
{
}

/*
 *  Invoke Structure
 *  -----------------------------------
 */
STATIC struct iv *
tcap_alloc_iv(ulong)
{
}
STATIC struct iv *
iv_get(struct iv *)
{
}
STATIC struct iv *
iv_lookup(struct dg *, ulong)
{
}
STATIC ulong
iv_get_id(ulong)
{
}
STATIC void
tcap_free_iv(struct iv *)
{
}
STATIC void
iv_put(struct iv *)
{
}

/*
 *  Dialogue Structure
 *  -----------------------------------
 */
STATIC struct dg *
tcap_alloc_dg(ulong)
{
}
STATIC struct dg *
dg_get(struct dg *)
{
}
STATIC struct dg *
dg_lookup(struct tr *, ulong)
{
}
STATIC ulong
dg_get_id(ulong)
{
}
STATIC void
tcap_free_dg(struct dg *)
{
}
STATIC void
dg_put(struct dg *)
{
}

/*
 *  Transaction Structure
 *  -----------------------------------
 */
STATIC struct tr *
tcap_alloc_tr(struct tcap *, ulong, ulong)
{
}
STATIC struct tr *
tr_get(struct tr *)
{
}
STATIC struct tr *
tr_lookup_cid(struct tcap *, ulong)
{
}
STATIC struct tr *
tr_lookup_tid(struct tcap *, ulong)
{
}
STATIC ulong
tr_get_id(ulong)
{
}
STATIC void
tcap_free_tr(struct tr *)
{
}
STATIC void
tr_put(struct tr *)
{
}

/*
 *  SCCP SAP Structure
 *  -----------------------------------
 */
STATIC struct sp *
tcap_alloc_sp(ulong)
{
}
STATIC struct sp *
sp_get(struct sp *)
{
}
STATIC struct sp *
sp_lookup(ulong)
{
}
STATIC ulong
sp_get_id(ulong)
{
}
STATIC void
tcap_free_sp(struct sp *)
{
}
STATIC void
sp_put(struct sp *)
{
}

/*
 *  SCCP Structure
 *  -----------------------------------
 */
STATIC struct sccp *
tcap_alloc_link(ulong)
{
}
STATIC struct sccp *
sccp_get(struct sccp *)
{
}
STATIC struct sccp *
sccp_lookup(ulong)
{
}
STATIC ulong
sccp_get_id(ulong)
{
}
STATIC void
sccp_free_link(struct sccp *)
{
}
STATIC void
sccp_put(struct sccp *)
{
}

/*
 *  =========================================================================
 *
 *  Registration and initialization
 *
 *  =========================================================================
 */
#ifdef LINUX
/*
 *  Linux Registration
 *  -------------------------------------------------------------------------
 */

unsigned short modid = DRV_ID;
MODULE_PARM(modid, "h");
MODULE_PARM_DESC(modid, "Module ID for the TCAP driver. (0 for allocation.)");

unsigned short major = CMAJOR_0;
MODULE_PARM(major, "h");
MODULE_PARM_DESC(major, "Device number for the TCAP driver. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LFS

STATIC struct cdevsw tcap_cdev = {
	.d_name = DRV_NAME,
	.d_str = &tcapinfo,
	.d_flag = 0,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

STATIC int
tcap_register_strdev(major_t major)
{
	int err;
	if ((err = register_strdev(&tcap_cdev, major)) < 0)
		return (err);
	return (0);
}

STATIC int
tcap_unregister_strdev(major_t major)
{
	int err;
	if ((err = unregister_strdev(&tcap_cdev, major)) < 0)
		return (err);
	return (0);
}

#endif				/* LFS */

/*
 *  Linux STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LIS

STATIC int
tcap_register_strdev(major_t major)
{
	int err;
	if ((err = lis_register_strdev(major, &tcapinfo, UNITS, DRV_NAME)) < 0)
		return (err);
	return (0);
}

STATIC int
tcap_unregister_strdev(major_t major)
{
	int err;
	if ((err = lis_unregister_strdev(major)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

MODULE_STATIC void __exit
tcapterminate(void)
{
	int err, mindex;
	for (mindex = CMAJORS - 1; mindex >= 0; mindex--) {
		if (tcap_majors[mindex]) {
			if ((err = tcap_unregister_strdev(tcap_majors[mindex])))
				cmn_err(CE_PANIC, "%s: cannot unregister major %d", DRV_NAME,
					tcap_majors[mindex]);
			if (mindex)
				tcap_majors[mindex] = 0;
		}
	}
	if ((err = tcap_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", DRV_NAME);
	return;
}

MODULE_STATIC int __init
tcapinit(void)
{
	int err, mindex = 0;
	cmn_err(CE_NOTE, DRV_BANNER);	/* console splash */
	if ((err = tcap_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", DRV_NAME, err);
		tcapterminate();
		return (err);
	}
	for (mindex = 0; mindex < CMAJORS; mindex++) {
		if ((err = tcap_register_strdev(tcap_majors[mindex])) < 0) {
			if (mindex) {
				cmn_err(CE_WARN, "%s: could not register major %d", DRV_NAME,
					tcap_majors[mindex]);
				continue;
			} else {
				cmn_err(CE_WARN, "%s: could not register driver, err = %d",
					DRV_NAME, err);
				tcapterminate();
				return (err);
			}
		}
		if (tcap_majors[mindex] == 0)
			tcap_majors[mindex] = err;
#ifdef LIS
		LIS_DEVFLAGS(tcap_majors[mindex]) |= LIS_MODFLG_CLONE;
#endif
		if (major == 0)
			major = tcap_majors[0];
	}
	return (0);
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(tcapinit);
module_exit(tcapterminate);

#endif				/* LINUX */
