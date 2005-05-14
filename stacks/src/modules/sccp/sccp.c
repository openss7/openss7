/*****************************************************************************

 @(#) $RCSfile: sccp.c,v $ $Name:  $($Revision: 0.9.2.12 $) $Date: 2005/05/14 08:31:03 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
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
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2005/05/14 08:31:03 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: sccp.c,v $ $Name:  $($Revision: 0.9.2.12 $) $Date: 2005/05/14 08:31:03 $"

static char const ident[] =
    "$RCSfile: sccp.c,v $ $Name:  $($Revision: 0.9.2.12 $) $Date: 2005/05/14 08:31:03 $";

/*
 *  This is an SCCP (Signalling Connection Control Part) multiplexing driver
 *  which can have MTP (Message Transfer Part) streams I_LINK'ed or I_PLINK'ed
 *  underneath it to form a complete Signalling Connection Control Part
 *  protocol layer for SS7.
 */
#include "os7/compat.h"

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/mtpi.h>
#include <ss7/mtpi_ioctl.h>

#include <ss7/sccpi.h>
#include <ss7/sccpi_ioctl.h>
#include <sys/npi.h>
#include <sys/npi_ss7.h>
#include <sys/npi_mtp.h>
#include <sys/npi_sccp.h>
#include <sys/tihdr.h>
// #include <sys/tpi.h>
// #include <sys/tpi_ss7.h>
// #include <sys/tpi_mtp.h>
// #include <sys/tpi_sccp.h>
#include <sys/xti.h>
#include <sys/xti_mtp.h>
#include <sys/xti_sccp.h>

#define SCCP_DESCRIP	"SS7 SIGNALLING CONNECTION CONTROL PART (SCCP) STREAMS MULTIPLEXING DRIVER."
#define SCCP_REVISION	"LfS $RCSfile: sccp.c,v $ $Name:  $($Revision: 0.9.2.12 $) $Date: 2005/05/14 08:31:03 $"
#define SCCP_COPYRIGHT	"Copyright (c) 1997-2003 OpenSS7 Corporation.  All Rights Reserved."
#define SCCP_DEVICE	"Part of the OpenSS7 Stack for LiS STREAMS."
#define SCCP_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SCCP_LICENSE	"GPL"
#define SCCP_BANNER	SCCP_DESCRIP	"\n" \
			SCCP_REVISION	"\n" \
			SCCP_COPYRIGHT	"\n" \
			SCCP_DEVICE	"\n" \
			SCCP_CONTACT

#ifdef LINUX
MODULE_AUTHOR(SCCP_CONTACT);
MODULE_DESCRIPTION(SCCP_DESCRIP);
MODULE_SUPPORTED_DEVICE(SCCP_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(SCCP_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-sccp");
#endif
#endif				/* LINUX */

#ifdef LFS
#define SCCP_DRV_ID	CONFIG_STREAMS_SCCP_MODID
#define SCCP_DRV_NAME	CONFIG_STREAMS_SCCP_NAME
#define SCCP_CMAJORS	CONFIG_STREAMS_SCCP_NMAJORS
#define SCCP_CMAJOR_0	CONFIG_STREAMS_SCCP_MAJOR
#define SCCP_UNITS	CONFIG_STREAMS_SCCP_NMINORS
#endif

#define SCCP_CMINOR_SCCPI   0
#define SCCP_CMINOR_TPI	    1
#define SCCP_CMINOR_NPI	    2
#define SCCP_CMINOR_GTT	    3
#define SCCP_CMINOR_MGMT    4
#define SCCP_CMINOR_FREE    5

#define SCCP_STYLE_SCCPI    SCCP_CMINOR_SCCPI
#define SCCP_STYLE_TPI	    SCCP_CMINOR_TPI
#define SCCP_STYLE_NPI	    SCCP_CMINOR_NPI
#define SCCP_STYLE_GTT	    SCCP_CMINOR_GTT
#define SCCP_STYLE_MGMT	    SCCP_CMINOR_MGMT

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

#define DRV_ID		SCCP_DRV_ID
#define DRV_NAME	SCCP_DRV_NAME
#define CMAJORS		SCCP_CMAJORS
#define CMAJOR_0	SCCP_CMAJOR_0
#define UNITS		SCCP_UNITS
#ifdef MODULE
#define DRV_BANNER	SCCP_BANNER
#else				/* MODULE */
#define DRV_BANNER	SCCP_SPLASH
#endif				/* MODULE */

STATIC struct module_info sccp_winfo = {
	mi_idnum:DRV_ID,		/* Module ID number */
	mi_idname:DRV_NAME,		/* Module ID name */
	mi_minpsz:1,			/* Min packet size accepted */
	mi_maxpsz:272 + 1,		/* Max packet size accepted */
	mi_hiwat:1 << 15,		/* Hi water mark */
	mi_lowat:1 << 10,		/* Lo water mark */
};
STATIC struct module_info sccp_rinfo = {
	mi_idnum:DRV_ID,		/* Module ID number */
	mi_idname:DRV_NAME,		/* Module ID name */
	mi_minpsz:1,			/* Min packet size accepted */
	mi_maxpsz:272 + 1,		/* Max packet size accepted */
	mi_hiwat:1 << 15,		/* Hi water mark */
	mi_lowat:1 << 10,		/* Lo water mark */
};
STATIC struct module_info mtp_winfo = {
	mi_idnum:DRV_ID,		/* Module ID number */
	mi_idname:DRV_NAME,		/* Module ID name */
	mi_minpsz:1,			/* Min packet size accepted */
	mi_maxpsz:272 + 1,		/* Max packet size accepted */
	mi_hiwat:1 << 15,		/* Hi water mark */
	mi_lowat:1 << 10,		/* Lo water mark */
};
STATIC struct module_info mtp_rinfo = {
	mi_idnum:DRV_ID,		/* Module ID number */
	mi_idname:DRV_NAME,		/* Module ID name */
	mi_minpsz:1,			/* Min packet size accepted */
	mi_maxpsz:272 + 1,		/* Max packet size accepted */
	mi_hiwat:1 << 15,		/* Hi water mark */
	mi_lowat:1 << 10,		/* Lo water mark */
};

STATIC int sccp_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC int sccp_close(queue_t *, int, cred_t *);

STATIC struct qinit sccp_rinit = {
	qi_putp:ss7_oput,		/* Read put (message from below) */
	qi_srvp:ss7_osrv,		/* Read queue service */
	qi_qopen:sccp_open,		/* Each open */
	qi_qclose:sccp_close,		/* Last close */
	qi_minfo:&sccp_rinfo,		/* Information */
};
STATIC struct qinit sccp_winit = {
	qi_putp:ss7_iput,		/* Write put (message from above) */
	qi_srvp:ss7_isrv,		/* Write queue service */
	qi_minfo:&sccp_winfo,		/* Information */
};
STATIC struct qinit mtp_rinit = {
	qi_putp:ss7_iput,		/* Read put (message from below) */
	qi_srvp:ss7_isrv,		/* Read queue service */
	qi_minfo:&mtp_rinfo,		/* Information */
};
STATIC struct qinit mtp_winit = {
	qi_putp:ss7_oput,		/* Write put (message from above) */
	qi_srvp:ss7_osrv,		/* Write queue service */
	qi_minfo:&mtp_winfo,		/* Information */
};

STATIC struct streamtab sccpinfo = {
	st_rdinit:&sccp_rinit,		/* Upper read queue */
	st_wrinit:&sccp_winit,		/* Upper write queue */
	st_muxrinit:&mtp_rinit,		/* Lower read queue */
	st_muxwinit:&mtp_winit,		/* Lower write queue */
};

/*
 *  =========================================================================
 *
 *  Private Structures
 *
 *  =========================================================================
 */

#ifndef PAD4
#define PAD4(_a) (((_a)+sizeof(ulong)-1) & ~(sizeof(ulong)-1))
#endif

typedef struct sccp_options {
	t_uscalar_t pcl;
	t_uscalar_t ret;
	t_uscalar_t imp;
	t_uscalar_t seq;
	t_uscalar_t pri;
	t_uscalar_t sls;
	t_uscalar_t mp;
	t_uscalar_t debug;
} sccp_options_t;

struct sccp_options opt_defaults;

/*
 *  Signalling Connection Control Part
 *  -----------------------------------
 */
typedef struct sc {
	STR_DECLARATION (struct sc);	/* stream declaration */
	SLIST_LINKAGE (ss, sc, ss);	/* bound/listen subsystem linkage */
	SLIST_LINKAGE (sp, sc, sp);	/* connected local signalling point */
	SLIST_LINKAGE (sr, sc, sr);	/* connected remote signalling point */
	ulong token;			/* token for this stream */
	ulong conind;			/* max connection inds */
	ulong outcnt;			/* out connection inds */
	ulong pcl;			/* protocol class */
	ulong ret;			/* return on error */
	ulong slr;			/* src local reference */
	ulong dlr;			/* dst local reference */
	struct sccp_addr src;		/* bound src address */
	uint8_t saddr[SCCP_MAX_ADDR_LENGTH];
	struct sccp_addr dst;		/* bound dst address */
	uint8_t daddr[SCCP_MAX_ADDR_LENGTH];
	ulong sls;			/* SLS value (seq. control) */
	ulong mp;			/* MP value (message prior.) */
	ulong imp;			/* SCCP importance */
	ulong credit;			/* window */
	ulong p_cred;			/* peer window */
	ulong ps;			/* send sequence number */
	ulong pr;			/* recv sequence number */
	ulong datinds;			/* unacked data inds */
	ulong mtu;			/* maximum transfer unit */
	bufq_t conq;			/* connection indication queue */
	bufq_t sndq;			/* send buffer */
	bufq_t urgq;			/* expedited data send buffer */
	bufq_t ackq;			/* recv awaiting ack buffer */
	bufq_t rcvq;			/* recv buffer */
	struct sccp_options options;	/* settable options */
	struct lmi_option proto;	/* sc protocol variant and options */
	struct sccp_notify_sc notify;	/* sc notifications */
	struct sccp_timers_sc timers;	/* sc protocol timers */
	struct sccp_opt_conf_sc config;	/* sc configuration */
	struct sccp_stats_sc statsp;	/* sc statistics periods */
	struct sccp_stats_sc stamp;	/* sc statistics timestamps */
	struct sccp_stats_sc stats;	/* sc statistics */
} sc_t;
#define SCCP_PRIV(__q) ((struct sc *)(__q)->q_ptr)

STATIC struct sc *sccp_alloc_priv(queue_t *, struct sc **, dev_t *, cred_t *, minor_t);
STATIC struct sc *sccp_get(struct sc *);
STATIC struct sc *sccp_lookup(ulong);
STATIC ulong sccp_get_id(ulong);
STATIC void sccp_free_priv(struct sc *);
STATIC void sccp_put(struct sc *);

#define SCCPF_DEFAULT_RC_SEL	0x00000001
#define SCCPF_LOC_ED		0x00000002
#define SCCPF_REM_ED		0x00000004
#define SCCPF_REC_CONF_OPT	0x00000008

#define SCCPF_EX_DATA_OPT	0x00000010
#define SCCPF_RETURN_ERROR	0x00000020
#define SCCPF_ACK_OUT		0x00000040

#define SCCPF_PROHIBITED	0x00000100
#define SCCPF_CONGESTED		0x00000200
#define SCCPF_SCCP_UPU		0x00000400
#define SCCPF_SUBSYSTEM_TEST	0x00000800

#define SCCPF_UNEQUIPPED	0x00001000
#define SCCPF_INACCESSIBLE	0x00002000
#define SCCPF_STRICT_ROUTING	0x00004000
#define SCCPF_STRICT_CONFIG	0x00008000

#define SCCPF_HOPC_VIOLATION	0x00010000
#define SCCPF_XLAT_FAILURE	0x00020000
#define SCCPF_ASSOC_REQUIRED	0x00040000
#define SCCPF_CHANGE_REQUIRED	0x00080000

/*
 *  NA - Network Appearance
 *  -----------------------------------
 *  The NA structure defines a network appearance.  A network appearance is a combination of protocol variant,
 *  protocol options and point code format.  The network appearance identifies a point code numbering space and
 *  defines the service indicators within that numbering space.  Signalling points belong to a single network
 *  appearance.
 */
typedef struct na {
	HEAD_DECLARATION (struct na);	/* head declaration */
	SLIST_HEAD (sp, sp);		/* signalling points in this na */
	struct lmi_option proto;	/* protocol variant and options */
	N_info_ack_t prot;		/* protocol profiles for SCCP for this na */
	struct sccp_timers_na timers;	/* network appearance protocol timers */
	struct sccp_opt_conf_na config;	/* network appearance configuration */
	struct sccp_stats_na statsp;	/* network appearance statistics periods */
	struct sccp_stats_na stats;	/* network appearance statistics */
	struct sccp_notify_na notify;	/* network appearance notifications */
} na_t;

STATIC struct na *sccp_alloc_na(ulong, struct lmi_option *);
STATIC void sccp_free_na(struct na *);
STATIC struct na *na_lookup(ulong);
STATIC ulong na_get_id(ulong);
STATIC struct na *na_get(struct na *);
STATIC void na_put(struct na *);

/*
 *  These structures define a single concerned signalling point.  These structures are listed against an equipped
 *  subsystem.  They can be added and removed with IOCTLs on either the management stream or any stream bound to the
 *  associated subsystem.
 *
 *  When MTP-PAUSE, RESUME or STATUS primitives are received by SCCP with respect to a remote singalling point, we
 *  scan the list of equipped subsystems and concerned signalling points associated with the MTP-provider on which
 *  these primitves were recieved.  If there is an equipped and bound subsystem with the local signalling point as a
 *  concerned signalling point, we deliver the N-PCSTATE primitive to any protocol class 0 or 1 SCCP-Users as an
 *  N_UDERROR_IND/T_UDERROR_IND.  Otherwise, we do not inform the SCCP-User using N-PCSTATE for remote point codes
 *  which are not concerned, nor for protocol classes 2 and 3.  (For protocol classes 2 and 3 we simply generate an
 *  error when an attempt is made to connect or send to the remote point code.) FIXME: we should send
 *  N_DISCON_IND/T_DISCON_IND with appropriate reason.
 *
 *  If we receive a subsystem allowed, prohibited or subsystem congestion indication for a concerned susbsystem, if
 *  there is an equipped subsystem that has the affected susbsystem as a concerned subsystem, we deliver the N-STATE
 *  primitive to any protocol class 0 or 1 SCCP-Users as a N_UDERROR_IND/T_UDERROR_IND.  Otherwise, we do not inform
 *  the SCCP-User using N-STATE for remote subsystems which are not concenred, nor for protocol classes 2 and 3.
 *  (For protocol classes 2 and 3 we simply generate an error when an attempt is made to connect to the remote
 *  subsystem.)  For protocol variants (e.g, SS7_PVAR_JTTC) that do not support SCCP management, remote concerned
 *  subsystems are always considered available.
 *
 *  If an SCCP-User attempts to send to or connect to a non-concerned remote signalling point or subsystem, we add
 *  the signalling point or subsystem to the lists automagically.
 */

/*
 *  Connection Point
 *  -----------------------------------
 *  A connection point represents the association of two connection sections.
 */
typedef struct cp {
	HEAD_DECLARATION (struct cp);	/* head declaration */
	SLIST_LINKAGE (sp, cp, sp);	/* local signalling point linkage */
	ulong pcl;			/* protocol class */
	struct {
		struct sr *sr;		/* remote signalling point */
		ulong slr;		/* source local reference */
		ulong dlr;		/* destination local reference */
		ulong sls;		/* SLS value (seq. control) */
		ulong mp;		/* MP value (message priority) */
		ulong imp;		/* SCCP importance */
		ulong credit;		/* credit */
		ulong p_cred;		/* peer window */
		ulong ps;		/* send sequence number */
		ulong pr;		/* recv sequence number */
		ulong datinds;		/* unacked data inds */
		ulong mtu;		/* maximum transfer unit */
		bufq_t sndq;		/* send buffer */
		bufq_t urgq;		/* expedited data send buffer */
		bufq_t ackq;		/* recv awaiting ack buffer */
		bufq_t rcvq;		/* recv buffer */
	} csec[2];			/* two connection sections */
	struct sccp_notify_cp notify;	/* cp notifications */
	struct sccp_timers_cp timers;	/* cp timers */
	struct sccp_opt_conf_cp config;	/* cp configuration */
	struct sccp_stats_cp statsp;	/* cp statistics periods */
	struct sccp_stats_cp stamp;	/* cp statistics timestamps */
	struct sccp_stats_cp stats;	/* cp statistics */
} cp_t;

#if 0
STATIC struct cp *sccp_alloc_cp(ulong, struct sp *, ulong, ulong);
STATIC struct cp *cp_get(struct cp *);
#endif
STATIC struct cp *cp_lookup(ulong);
#if 0
STATIC struct cp *sccp_lookup_cp(struct sp *, ulong, const int);
STATIC ulong cp_get_id(ulong);
#endif
STATIC void sccp_free_cp(struct cp *);
STATIC void cp_put(struct cp *);

/*
 *  Subsystem (local)
 *  -----------------------------------
 */
typedef struct ss {
	HEAD_DECLARATION (struct ss);	/* head declaration */
	SLIST_LINKAGE (sp, ss, sp);	/* concerned signalling point */
	SLIST_HEAD (sc, cl);		/* bound CL SCCP users */
	SLIST_HEAD (sc, co);		/* bound CO SCCP users */
	SLIST_HEAD (sc, cr);		/* listening SCCP users */
	SLIST_HEAD (rs, rs);		/* affected subsystem list */
	ulong ssn;			/* subsystem number */
	ulong smi;			/* subsystem multiplicity indicator */
	ulong level;			/* subsystem congestion level */
	ulong count;			/* count of congestion messages */
	struct lmi_option proto;	/* ss protocol variant and options */
	struct sccp_notify_ss notify;	/* ss notifications */
	struct sccp_timers_ss timers;	/* ss timers */
	struct sccp_opt_conf_ss config;	/* ss configuration */
	struct sccp_stats_ss statsp;	/* ss statistics periods */
	struct sccp_stats_ss stamp;	/* ss statistics timestamps */
	struct sccp_stats_ss stats;	/* ss statistics */
} ss_t;
#define SS_STATUS_AVAILABLE	0	/* subsystem is available */
#define SS_STATUS_UNAVAILABLE	1	/* subsystem is unavailable */
#define SS_STATUS_CONGESTED	2	/* subsystem is congested */
#define SS_STATUS_BLOCKED	3	/* subsystem is management blocked */
#define SS_STATUS_UNEQUIPPED	4	/* subsystem is unequipped */

STATIC struct ss *sccp_alloc_ss(ulong, struct sp *, ulong);
STATIC struct ss *ss_get(struct ss *);
STATIC struct ss *ss_lookup(ulong);
STATIC struct ss *sccp_lookup_ss(struct sp *, ulong, const int);
STATIC ulong ss_get_id(ulong);
STATIC void sccp_free_ss(struct ss *);
STATIC void ss_put(struct ss *);

/*
 *  Subsystem (remote)
 *  -----------------------------------
 */
typedef struct rs {
	HEAD_DECLARATION (struct rs);	/* head declaration */
	SLIST_LINKAGE (sr, rs, sr);	/* affected signalling point */
	SLIST_LINKAGE (ss, rs, ss);	/* concerned subsystem linkage */
	ulong ssn;			/* subsystem number */
	ulong smi;			/* subsystem multiplicity indicator */
	ulong level;			/* subsystem congestion level */
	ulong count;			/* count of congestion messages */
	struct lmi_option proto;	/* rs protocol variant and options */
	struct sccp_notify_rs notify;	/* rs notifications */
	struct sccp_timers_rs timers;	/* rs timers */
	struct sccp_opt_conf_rs config;	/* rs configuration */
	struct sccp_stats_rs statsp;	/* rs statistics periods */
	struct sccp_stats_rs stamp;	/* rs statistics timestamps */
	struct sccp_stats_rs stats;	/* rs statistics */
} rs_t;
#define RS_STATUS_AVAILABLE	0	/* subsystem is available */
#define RS_STATUS_UNAVAILABLE	1	/* subsystem is unavailable */
#define RS_STATUS_CONGESTED	2	/* subsystem is congested */
#define RS_STATUS_BLOCKED	3	/* subsystem is management blocked */
#define RS_STATUS_UNEQUIPPED	4	/* subsystem is unequipped */

STATIC struct rs *sccp_alloc_rs(ulong, struct sr *, ulong);
STATIC struct rs *rs_get(struct rs *);
STATIC struct rs *rs_lookup(ulong);
STATIC struct rs *sccp_lookup_rs(struct sr *, ulong ssn, const int);
STATIC ulong rs_get_id(ulong);
STATIC void sccp_free_rs(struct rs *);
STATIC void rs_put(struct rs *);

/*
 *  Signalling Relation
 *  -----------------------------------
 */
typedef struct sr {
	HEAD_DECLARATION (struct sr);
	SLIST_LINKAGE (sp, sr, sp);	/* list linkage */
	SLIST_HEAD (rs, rs);		/* affected subsystems */
	SLIST_HEAD (sc, sc);		/* SCCP users connected to this remote signalling point */
	struct mtp_addr add;		/* SS7 address associated with remote signalling point */
	struct mt *mt;			/* MTP if linked at SR */
	ulong level;			/* affected signalling point level */
	ulong count;			/* count for congestion dropping */
	struct lmi_option proto;	/* sr protocol variant and options */
	struct sccp_notify_sr notify;	/* sr notifications */
	struct sccp_timers_sr timers;	/* sr timers */
	struct sccp_opt_conf_sr config;	/* sr configuration */
	struct sccp_stats_sr statsp;	/* sr statistics periods */
	struct sccp_stats_sr stamp;	/* sr statistics timestamps */
	struct sccp_stats_sr stats;	/* sr statistics */
} sr_t;
#define SR_STATUS_AVAILABLE	0	/* sr is available */
#define SR_STATUS_UNAVAILABLE	1	/* sr is unavailable */
#define SR_STATUS_CONGESTED	2	/* sr is congested */
#define SR_STATUS_BLOCKED	3	/* sr is management blocked */

STATIC struct sr *sccp_alloc_sr(ulong, struct sp *, ulong);
STATIC struct sr *sr_get(struct sr *);
STATIC struct sr *sr_lookup(ulong);
STATIC struct sr *sccp_lookup_sr(struct sp *, ulong, const int);
STATIC ulong sr_get_id(ulong);
STATIC void sccp_free_sr(struct sr *);
STATIC void sr_put(struct sr *);

/*
 *  Signalling Point
 *  -----------------------------------
 */
#define SCCP_CL_HASH_SIZE   32	/* connection-less bind hash slots */
#define SCCP_CR_HASH_SIZE   16	/* connection-oriented listening hash slots */
#define SCCP_CO_HASH_SIZE   64	/* connection-oriented established hash slots */
#define SCCP_CP_HASH_SIZE   64	/* connection-oriented coupling slots */
typedef struct sp {
	HEAD_DECLARATION (struct sp);	/* head declaration */
	SLIST_HEAD (sr, sr);		/* signalling relation list */
	SLIST_HEAD (ss, ss);		/* concerned subsystem list */
	SLIST_HEAD (sc, sc);		/* bound, connected or listening users */
	SLIST_HEAD (sc, gt);		/* global title streams */
	SLIST_HEAD (cp, cp);		/* list of couplings */
	struct mtp_addr add;		/* MTP address associated with local signalling point */
	struct mt *mt;			/* MTP if linked at SP */
	SLIST_LINKAGE (na, sp, na);	/* network appearance linkage */
	ulong sccp_next_cl_sls;		/* rotate SLS values */
	ulong sccp_next_co_sls;		/* rotate SLS values */
	ulong sccp_next_slr;		/* rotate SLR values */
	ulong level;			/* congestion level */
	ulong count;			/* count for congestion dropping */
	struct sc *sccp_cl_hash[SCCP_CL_HASH_SIZE];	/* hash of connectionless binds */
	struct sc *sccp_cr_hash[SCCP_CR_HASH_SIZE];	/* hash of connection request listeners */
	struct sc *sccp_co_hash[SCCP_CO_HASH_SIZE];	/* hash of connection-oriented connections */
	struct cp *sccp_cp_hash[SCCP_CP_HASH_SIZE];	/* hash of connection-oriented couplings */
	struct lmi_option proto;	/* sp protocol variant and options */
	struct sccp_notify_sp notify;	/* sp nitifications */
	struct sccp_timers_sp timers;	/* sp protocol timers */
	struct sccp_opt_conf_sp config;	/* sp configuration */
	struct sccp_stats_sp statsp;	/* sp statistics periods */
	struct sccp_stats_sp stamp;	/* sp statistics timestamps */
	struct sccp_stats_sp stats;	/* sp statistics */
} sp_t;

STATIC struct sp *sccp_alloc_sp(ulong, struct na *, mtp_addr_t *);
STATIC struct sp *sp_get(struct sp *);
STATIC struct sp *sp_lookup(ulong);
STATIC struct sp *sccp_lookup_sp(ulong ni, ulong pc, const int);
STATIC ulong sp_get_id(ulong);
STATIC void sccp_free_sp(struct sp *);
STATIC void sp_put(struct sp *);

/*
 *  Message Transfer Part
 *  -----------------------------------
 */
typedef struct mt {
	STR_DECLARATION (struct mt);	/* stream declaration */
	struct sp *sp;			/* signalling point */
	struct sr *sr;			/* signalling relation */
	// SLIST_LINKAGE (sp, mt, sp); /* signalling point list linkage */
	// SLIST_LINKAGE (sr, mt, sr); /* signalling relation list linkage */
	ulong psdu;			/* pSDU size */
	ulong pidu;			/* pIDU size */
	ulong podu;			/* pODU size */
	struct mtp_addr loc;		/* local signalling point */
	struct mtp_addr rem;		/* remote signalling point */
	struct lmi_option proto;	/* mt protocol variant and options */
	struct sccp_notify_mt notify;	/* notifications */
	struct sccp_timers_mt timers;	/* mt protocol timers */
	struct sccp_opt_conf_mt config;	/* mt configuration */
	struct sccp_stats_mt statsp;	/* mt statistics periods */
	struct sccp_stats_mt stamp;	/* mt statistics timestamps */
	struct sccp_stats_mt stats;	/* mpt statistics */
} mt_t;
#define MTP_PRIV(__q) ((struct mt *)(__q)->q_ptr)

STATIC struct mt *sccp_alloc_link(queue_t *, struct mt **, ulong, cred_t *);
STATIC struct mt *mtp_get(struct mt *);
STATIC struct mt *mtp_lookup(ulong);
STATIC ulong mtp_get_id(ulong);
STATIC void sccp_free_link(struct mt *);
STATIC void mtp_put(struct mt *);

/*
 *  Default structure
 *  -----------------------------------
 */
typedef struct df {
	spinlock_t lock;
	SLIST_HEAD (sc, sc);		/* master list of sc */
	SLIST_HEAD (na, na);		/* master list of network appearances */
	SLIST_HEAD (cp, cp);		/* master list of couplings */
	SLIST_HEAD (ss, ss);		/* master list of local subsystems */
	SLIST_HEAD (rs, rs);		/* master list of remote subsystems */
	SLIST_HEAD (sr, sr);		/* master list of remote signalling points */
	SLIST_HEAD (sp, sp);		/* master list of local signalling points */
	SLIST_HEAD (mt, mt);		/* master list of message transfer parts */
	struct lmi_option proto;	/* default protocol options */
	struct sccp_notify_df notify;	/* default notifications */
	struct sccp_opt_conf_df config;	/* default configuration */
	struct sccp_stats_df stats;	/* default statistics */
	struct sccp_stats_df stamp;	/* default statistics timestamps */
	struct sccp_stats_df statsp;	/* default statistics periods */
} df_t;
STATIC struct df master;

/*
 *  =========================================================================
 *
 *  OPTION Handling
 *
 *  =========================================================================
 */
typedef struct sccp_opts {
	ulong flags;			/* success flags */
	t_uscalar_t *pcl;
	t_uscalar_t *ret;
	t_uscalar_t *imp;
	t_uscalar_t *seq;
	t_uscalar_t *pri;
	t_uscalar_t *sls;
	t_uscalar_t *mp;
	t_uscalar_t *debug;
} sccp_opts_t;

#define TF_OPT_PCL	(1<<0)	/* protocol class */
#define TF_OPT_RET	(1<<1)	/* return on error option */
#define TF_OPT_IMP	(1<<2)	/* importance */
#define TF_OPT_SEQ	(1<<3)	/* sequence control */
#define TF_OPT_PRI	(1<<4)	/* priority */
#define TF_OPT_SLS	(1<<5)	/* signalling link selection */
#define TF_OPT_MP	(1<<6)	/* message priority */
#define TF_OPT_DBG	(1<<7)	/* debug */

#ifndef _T_ALIGN_SIZE
#define _T_ALIGN_SIZE sizeof(t_uscalar_t)
#endif
#ifndef _T_ALIGN_SIZEOF
#define _T_ALIGN_SIZEOF(s) \
	((sizeof((s)) + _T_ALIGN_SIZE - 1) & ~(_T_ALIGN_SIZE - 1))
#endif

STATIC size_t
sccp_opts_size(struct sc *sc, struct sccp_opts *ops)
{
	size_t len = 0;
	if (ops) {
		const size_t hlen = sizeof(struct t_opthdr);	/* 32 bytes */
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
		if (ops->debug)
			len += hlen + _T_ALIGN_SIZEOF(*(ops->debug));
	}
	return (len);
}
STATIC void
sccp_build_opts(struct sc *sc, struct sccp_opts *ops, uchar *p)
{
	if (ops) {
		struct t_opthdr *oh;
		const size_t hlen = sizeof(struct t_opthdr);
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
		if (ops->debug) {
			oh = ((typeof(oh)) p)++;
			oh->len = hlen + sizeof(*(ops->debug));
			oh->level = T_SS7_MTP;
			oh->name = T_MTP_DEBUG;
			oh->status = (ops->flags & TF_OPT_DBG) ? T_SUCCESS : T_FAILURE;
			*((typeof(ops->debug)) p) = *(ops->debug);
			p += _T_ALIGN_SIZEOF(*ops->debug);
		}
	}
}
STATIC int
sccp_parse_opts(struct sc *sc, struct sccp_opts *ops, uchar *op, size_t len)
{
	struct t_opthdr *oh;
	for (oh = _T_OPT_FIRSTHDR_OFS(op, len, 0); oh; oh = _T_OPT_NEXTHDR_OFS(op, len, oh, 0)) {
		switch (oh->level) {
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
			case T_MTP_DEBUG:
				ops->debug = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->flags |= TF_OPT_DBG;
				continue;
			}
			break;
		}
	}
	if (oh)
		return (TBADOPT);
	return (0);
}

#if 0
STATIC int
sccp_parse_qos(struct sc *sc, struct sccp_opts *ops, uchar *op, size_t len)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}
#endif

/*
 *  =========================================================================
 *
 *  OPTIONS handling
 *
 *  =========================================================================
 */
STATIC int
sccp_opt_check(struct sc *sc, struct sccp_opts *ops)
{
	if (ops->flags) {
		ops->flags = 0;
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
		if (ops->debug)
			ops->flags |= TF_OPT_DBG;
	}
	return (0);
}
STATIC int
sccp_opt_default(struct sc *sc, struct sccp_opts *ops)
{
	if (ops) {
		int flags = ops->flags;
		ops->flags = 0;
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
		if (!flags || ops->debug) {
			ops->debug = &opt_defaults.debug;
			ops->flags |= TF_OPT_DBG;
		}
		return (0);
	}
	swerr();
	return (-EFAULT);
}
STATIC int
sccp_opt_current(struct sc *sc, struct sccp_opts *ops)
{
	int flags = ops->flags;
	ops->flags = 0;
	if (!flags || ops->pcl) {
		ops->pcl = &sc->options.pcl;
		ops->flags |= TF_OPT_PCL;
	}
	if (!flags || ops->ret) {
		ops->ret = &sc->options.ret;
		ops->flags |= TF_OPT_RET;
	}
	if (!flags || ops->imp) {
		ops->imp = &sc->options.imp;
		ops->flags |= TF_OPT_IMP;
	}
	if (!flags || ops->seq) {
		ops->seq = &sc->options.seq;
		ops->flags |= TF_OPT_SEQ;
	}
	if (!flags || ops->pri) {
		ops->pri = &sc->options.pri;
		ops->flags |= TF_OPT_PRI;
	}
	if (!flags || ops->sls) {
		ops->sls = &sc->options.sls;
		ops->flags |= TF_OPT_SLS;
	}
	if (!flags || ops->mp) {
		ops->mp = &sc->options.mp;
		ops->flags |= TF_OPT_MP;
	}
	if (!flags || ops->debug) {
		ops->debug = &sc->options.debug;
		ops->flags |= TF_OPT_DBG;
	}
	return (0);
}
STATIC int
sccp_opt_negotiate(struct sc *sc, struct sccp_opts *ops)
{
	if (ops->flags) {
		ops->flags = 0;
		if (ops->pcl) {
			sc->options.pcl = *ops->pcl;
			ops->pcl = &sc->options.pcl;
			ops->flags |= TF_OPT_PCL;
		}
		if (ops->ret) {
			sc->options.ret = *ops->ret;
			ops->ret = &sc->options.ret;
			ops->flags |= TF_OPT_RET;
		}
		if (ops->imp) {
			sc->options.imp = *ops->imp;
			ops->imp = &sc->options.imp;
			ops->flags |= TF_OPT_IMP;
		}
		if (ops->seq) {
			sc->options.seq = *ops->seq;
			ops->seq = &sc->options.seq;
			ops->flags |= TF_OPT_SEQ;
		}
		if (ops->pri) {
			sc->options.pri = *ops->pri;
			ops->pri = &sc->options.pri;
			ops->flags |= TF_OPT_PRI;
		}
		if (ops->sls) {
			sc->options.sls = *ops->sls;
			ops->sls = &sc->options.sls;
			ops->flags |= TF_OPT_SLS;
		}
		if (ops->mp) {
			sc->options.mp = *ops->mp;
			ops->mp = &sc->options.mp;
			ops->flags |= TF_OPT_MP;
		}
		if (ops->debug) {
			sc->options.debug = *ops->debug;
			ops->debug = &sc->options.debug;
			ops->flags |= TF_OPT_DBG;
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
#define SS_UNBND	0	/* NS user not bound to network address */
#define SS_WACK_BREQ	1	/* Awaiting acknowledgement of N_BIND_REQ */
#define SS_WACK_UREQ	2	/* Pending acknowledgement for N_UNBIND_REQ */
#define SS_IDLE		3	/* Idle, no connection */
#define SS_WACK_OPTREQ	4	/* Pending acknowledgement of N_OPTMGMT_REQ */
#define SS_WACK_RRES	5	/* Pending acknowledgement of N_RESET_RES */
#define SS_WCON_CREQ	6	/* Pending confirmation of N_CONN_REQ */
#define SS_WRES_CIND	7	/* Pending response of N_CONN_REQ */
#define SS_WACK_CRES	8	/* Pending acknowledgement of N_CONN_RES */
#define SS_DATA_XFER	9	/* Connection-mode data transfer */
#define SS_WCON_RREQ	10	/* Pending confirmation of N_RESET_REQ */
#define SS_WRES_RIND	11	/* Pending response of N_RESET_IND */
#define SS_WACK_DREQ6	12	/* Waiting ack of N_DISCON_REQ */
#define SS_WACK_DREQ7	13	/* Waiting ack of N_DISCON_REQ */
#define SS_WACK_DREQ9	14	/* Waiting ack of N_DISCON_REQ */
#define SS_WACK_DREQ10	15	/* Waiting ack of N_DISCON_REQ */
#define SS_WACK_DREQ11	16	/* Waiting ack of N_DISCON_REQ */
#define SS_NOSTATES	18	/* No states */
#define SS_WRES_DIND	19	/* for now */
#define SS_WCON_DREQ	20	/* for now */
#define SS_BOTH_RESET	21	/* for now */
#define SS_WCON_CREQ2	22	/* for now */

#define SSN_ALLOWED	0
#define SSN_CONGESTED	1
#define SSN_PROHIBITED	2

#define SSF_UNBND	(1<<SS_UNBND		)
#define SSF_WACK_BREQ	(1<<SS_WACK_BREQ	)
#define SSF_WACK_UREQ	(1<<SS_WACK_UREQ	)
#define SSF_IDLE	(1<<SS_IDLE		)
#define SSF_WACK_OPTREQ	(1<<SS_WACK_OPTREQ	)
#define SSF_WACK_RRES	(1<<SS_WACK_RRES	)
#define SSF_WCON_CREQ	(1<<SS_WCON_CREQ	)
#define SSF_WRES_CIND	(1<<SS_WRES_CIND	)
#define SSF_WACK_CRES	(1<<SS_WACK_CRES	)
#define SSF_DATA_XFER	(1<<SS_DATA_XFER	)
#define SSF_WCON_RREQ	(1<<SS_WCON_RREQ	)
#define SSF_WRES_RIND	(1<<SS_WRES_RIND	)
#define SSF_WACK_DREQ6	(1<<SS_WACK_DREQ6	)
#define SSF_WACK_DREQ7	(1<<SS_WACK_DREQ7	)
#define SSF_WACK_DREQ9	(1<<SS_WACK_DREQ9	)
#define SSF_WACK_DREQ10	(1<<SS_WACK_DREQ10	)
#define SSF_WACK_DREQ11	(1<<SS_WACK_DREQ11	)
#define SSF_NOSTATES	(1<<SS_NOSTATES	)
#define SSF_WRES_DIND	(1<<SS_WRES_DIND	)
#define SSF_WCON_DREQ	(1<<SS_WCON_DREQ	)
#define SSF_BOTH_RESET	(1<<SS_BOTH_RESET	)
#define SSF_WCON_CREQ2	(1<<SS_WCON_CREQ2	)

#ifdef _DEBUG
STATIC INLINE const char *
sccp_n_state(ulong state)
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
STATIC INLINE const char *
sccp_t_state(ulong state)
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
sccp_state(ulong state)
{
	switch (state) {
	case SS_UNBND:
		return ("SS_UNBND");
	case SS_WACK_BREQ:
		return ("SS_WACK_BREQ");
	case SS_WACK_UREQ:
		return ("SS_WACK_UREQ");
	case SS_IDLE:
		return ("SS_IDLE");
	case SS_WACK_OPTREQ:
		return ("SS_WACK_OPTREQ");
	case SS_WACK_RRES:
		return ("SS_WACK_RRES");
	case SS_WCON_CREQ:
		return ("SS_WCON_CREQ");
	case SS_WRES_CIND:
		return ("SS_WRES_CIND");
	case SS_WACK_CRES:
		return ("SS_WACK_CRES");
	case SS_DATA_XFER:
		return ("SS_DATA_XFER");
	case SS_WCON_RREQ:
		return ("SS_WCON_RREQ");
	case SS_WRES_RIND:
		return ("SS_WRES_RIND");
	case SS_WACK_DREQ6:
		return ("SS_WACK_DREQ6");
	case SS_WACK_DREQ7:
		return ("SS_WACK_DREQ7");
	case SS_WACK_DREQ9:
		return ("SS_WACK_DREQ9");
	case SS_WACK_DREQ10:
		return ("SS_WACK_DREQ10");
	case SS_WACK_DREQ11:
		return ("SS_WACK_DREQ11");
	case SS_NOSTATES:
		return ("SS_NOSTATES");
	case SS_WRES_DIND:
		return ("SS_WRES_DIND");
	case SS_WCON_DREQ:
		return ("SS_WCON_DREQ");
	case SS_BOTH_RESET:
		return ("SS_BOTH_RESET");
	case SS_WCON_CREQ2:
		return ("SS_WCON_CREQ2");
	default:
		return ("(unknown)");
	}
}
#endif

STATIC INLINE void
sccp_set_state(struct sc *sc, ulong newstate)
{
	ulong oldstate = sc->state;
	if (newstate != oldstate) {
		printd(("%s: %p: %s <- %s\n", DRV_NAME, sc, sccp_state(newstate),
			sccp_state(oldstate)));
		sc->state = newstate;
	}
}
STATIC INLINE ulong
sccp_get_state(struct sc *sc)
{
	return sc->state;
}
STATIC INLINE void
sccp_set_n_state(struct sc *sc, ulong newstate)
{
	ulong oldstate = sc->i_state;
	if (newstate != oldstate) {
		printd(("%s: %p: %s <- %s\n", DRV_NAME, sc, sccp_n_state(newstate),
			sccp_n_state(oldstate)));
		sc->i_state = newstate;
	}
}
STATIC INLINE ulong
sccp_get_n_state(struct sc *sc)
{
	return sc->i_state;
}
STATIC INLINE void
sccp_set_t_state(struct sc *sc, ulong newstate)
{
	ulong oldstate = sc->i_state;
	if (newstate != oldstate) {
		printd(("%s: %p: %s <- %s\n", DRV_NAME, sc, sccp_t_state(newstate),
			sccp_t_state(oldstate)));
		sc->i_state = newstate;
	}
}
STATIC INLINE ulong
sccp_get_t_state(struct sc *sc)
{
	return sc->i_state;
}

#if 0
STATIC INLINE ulong
mtp_get_state(struct mt *mt)
{
	return mt->state;
}
STATIC INLINE void
mtp_set_state(struct mt *mt, ulong newstate)
{
	ulong oldstate = mt->state;
	if (newstate != oldstate) {
		printd(("%s: %p: %s <- %s\n", DRV_NAME, mt, mtp_state(newstate),
			mtp_state(oldstate)));
		mt->state = newstate;
	}
}

STATIC INLINE ulong
mtp_get_i_state(struct mt *mt)
{
	return mt->i_state;
}
STATIC INLINE void
mtp_set_i_state(struct mt *mt, ulong newstate)
{
	ulong oldstate = mt->i_state;
	if (newstate != oldstate) {
		printd(("%s: %p: %s <- %s\n", DRV_NAME, mt, mtp_i_state(newstate),
			mtp_i_state(oldstate)));
		mt->i_state = newstate;
	}
}
#endif

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
 *  SCCP Message Structures
 *  -------------------------------------------------------------------------
 */
#define SCCP_MT_NONE	0x00	/* (reserved) */
#define SCCP_MT_CR	0x01	/* Connection Request */
#define SCCP_MT_CC	0x02	/* Connection Confirm */
#define SCCP_MT_CREF	0x03	/* Connection Refused */
#define SCCP_MT_RLSD	0x04	/* Released */
#define SCCP_MT_RLC	0x05	/* Release Complete */
#define SCCP_MT_DT1	0x06	/* Data Form 1 */
#define SCCP_MT_DT2	0x07	/* Data Form 2 */
#define SCCP_MT_AK	0x08	/* Data Acknowledgement */
#define SCCP_MT_UDT	0x09	/* Unitdata */
#define SCCP_MT_UDTS	0x0a	/* Unitdata Service */
#define SCCP_MT_ED	0x0b	/* Expedited Data */
#define SCCP_MT_EA	0x0c	/* Expedited Data Acknowledgement */
#define SCCP_MT_RSR	0x0d	/* Reset Request */
#define SCCP_MT_RSC	0x0e	/* Reset Confirm */
#define SCCP_MT_ERR	0x0f	/* Protocol Data Unit Error */
#define SCCP_MT_IT	0x10	/* Inactivity Test */
#define SCCP_MT_XUDT	0x11	/* Extended Unitdata */
#define SCCP_MT_XUDTS	0x12	/* Extended Unitdata Service */
#define SCCP_MT_LUDT	0x13	/* Long Unitdata */
#define SCCP_MT_LUDTS	0x14	/* Long Unitdata Service */
#define SCCP_NOMESSAGES	0x15	/* number of message types */

#define SCMG_MT_SSA	0x01	/* Subsystem allowed */
#define SCMG_MT_SSP	0x02	/* Subsystem prohibited */
#define SCMG_MT_SST	0x03	/* Subsystem status test */
#define SCMG_MT_SOR	0x04	/* Subsystem out of service request */
#define SCMG_MT_SOG	0x05	/* Subsystem out of service grant */
#define SCMG_MT_SSC	0x06	/* Subsystem congestion */

#define SCMG_MT_SBR	0xfd	/* Subsystem backup routing */
#define SCMG_MT_SNR	0xfe	/* Subsystem normal routing */
#define SCMG_MT_SRT	0xff	/* Subsystem routing status test */

#define SCCP_MP_CR	0x0	/* 0 or 1 */
#define SCCP_MP_CC	0x1
#define SCCP_MP_CREF	0x1
#define SCCP_MP_RLSD	0x2
#define SCCP_MP_RLC	0x2
#define SCCP_MP_DT1	0x0	/* 0 or 1 */
#define SCCP_MP_DT2	0x0	/* 0 or 1 */
#define SCCP_MP_AK	0x0	/* 0 or 1 */
#define SCCP_MP_UDT	0x0	/* 0, 1 or 2 */
#define SCCP_MP_UDTS	0x0	/* 0, 1 or 2 */
#define SCCP_MP_ED	0x1
#define SCCP_MP_EA	0x1
#define SCCP_MP_RSR	0x1
#define SCCP_MP_RSC	0x1
#define SCCP_MP_ERR	0x1
#define SCCP_MP_IT	0x1
#define SCCP_MP_XUDT	0x0	/* 0, 1 or 2 */
#define SCCP_MP_XUDTS	0x0	/* 0, 1 or 2 */
#define SCCP_MP_LUDT	0x0	/* 0, 1 or 2 */
#define SCCP_MP_LUDTS	0x0	/* 0, 1 or 2 */

/*
 *  Importance values from Table 2/Q.714-1996
 */
/*
   default importance values 
 */
#define SCCP_DI_CR	0x2	/* inc at relay */
#define SCCP_DI_CC	0x3	/* inc at relay */
#define SCCP_DI_CREF	0x2	/* inc at relay */
#define SCCP_DI_DT1	0x4	/* def at relay */
#define SCCP_DI_DT2	0x4	/* def at relay */
#define SCCP_DI_AK	0x6	/* def at relay */
#define SCCP_DI_IT	0x6	/* def at relay */
#define SCCP_DI_ED	0x7	/* def at relay */
#define SCCP_DI_EA	0x7	/* def at relay */
#define SCCP_DI_RSR	0x6	/* def at relay */
#define SCCP_DI_RSC	0x6	/* def at relay */
#define SCCP_DI_ERR	0x7	/* def at relay */
#define SCCP_DI_RLC	0x4	/* def at relay */
#define SCCP_DI_RLSD	0x6	/* inc at relay */
#define SCCP_DI_UDT	0x4	/* def at relay */
#define SCCP_DI_UDTS	0x3	/* def at relay */
#define SCCP_DI_XUDT	0x4	/* inc at relay */
#define SCCP_DI_XUDTS	0x3	/* inc at relay */
#define SCCP_DI_LUDT	0x4	/* inc at relay */
#define SCCP_DI_LUDTS	0x3	/* inc at relay */

/*
   maximum importance values 
 */
#define SCCP_MI_CR	0x4
#define SCCP_MI_CC	0x4
#define SCCP_MI_CREF	0x4
#define SCCP_MI_DT1	0x6
#define SCCP_MI_DT2	0x6
#define SCCP_MI_AK	SCCP_DI_AK
#define SCCP_MI_IT	SCCP_DI_IT
#define SCCP_MI_ED	SCCP_DI_ED
#define SCCP_MI_EA	SCCP_DI_EA
#define SCCP_MI_RSR	SCCP_DI_RSR
#define SCCP_MI_RSC	SCCP_DI_RSC
#define SCCP_MI_ERR	SCCP_DI_ERR
#define SCCP_MI_RLC	SCCP_DI_RLC
#define SCCP_MI_RLSD	0x6
#define SCCP_MI_UDT	0x6
#define SCCP_MI_UDTS	0x0	/* same as returned UDT */
#define SCCP_MI_XUDT	0x6
#define SCCP_MI_XUDTS	0x0	/* same as returned XUDT */
#define SCCP_MI_LUDT	0x6
#define SCCP_MI_LUDTS	0x0	/* same as returned LUDT */

#define SCMG_MP_SSA	0x3
#define SCMG_MP_SSP	0x3
#define SCMG_MP_SST	0x2
#define SCMG_MP_SOR	0x1
#define SCMG_MP_SOG	0x1
#define SCMG_MP_SSC	0x0	/* not defined for ANSI */

#define SCMG_MP_SBR	0x0
#define SCMG_MP_SNR	0x0
#define SCMG_MP_SRT	0x0

/*
 *  Notes on message priority:
 *
 *  (1) The UDTS, XUDTS and LUDTS messages assume the message priority of the
 *      message which they contain.
 *
 *  (2) The UDT, XUDT and LUDT messages can assume the message priority of 0
 *      or 1: the value 2 is reserved for use by OMAP.
 *
 *  (3) The priority of DT1, DT2 and AK should match that of the connection
 *      request.
 */

#define SCCP_MTF_NONE	(1<<SCCP_MT_NONE	)
#define SCCP_MTF_CR	(1<<SCCP_MT_CR		)
#define SCCP_MTF_CC	(1<<SCCP_MT_CC		)
#define SCCP_MTF_CREF	(1<<SCCP_MT_CREF	)
#define SCCP_MTF_RLSD	(1<<SCCP_MT_RLSD	)
#define SCCP_MTF_RLC	(1<<SCCP_MT_RLC		)
#define SCCP_MTF_DT1	(1<<SCCP_MT_DT1		)
#define SCCP_MTF_DT2	(1<<SCCP_MT_DT2		)
#define SCCP_MTF_AK	(1<<SCCP_MT_AK		)
#define SCCP_MTF_UDT	(1<<SCCP_MT_UDT		)
#define SCCP_MTF_UDTS	(1<<SCCP_MT_UDTS	)
#define SCCP_MTF_ED	(1<<SCCP_MT_ED		)
#define SCCP_MTF_EA	(1<<SCCP_MT_EA		)
#define SCCP_MTF_RSR	(1<<SCCP_MT_RSR		)
#define SCCP_MTF_RSC	(1<<SCCP_MT_RSC		)
#define SCCP_MTF_ERR	(1<<SCCP_MT_ERR		)
#define SCCP_MTF_IT	(1<<SCCP_MT_IT		)
#define SCCP_MTF_XUDT	(1<<SCCP_MT_XUDT	)
#define SCCP_MTF_XUDTS	(1<<SCCP_MT_XUDTS	)
#define SCCP_MTF_LUDT	(1<<SCCP_MT_LUDT	)
#define SCCP_MTF_LUDTS	(1<<SCCP_MT_LUDTS	)

/*
 *  Connection-Oriented messages
 */
#define SCCP_MTM_CO	(	SCCP_MTF_CR	\
			|	SCCP_MTF_CC	\
			|	SCCP_MTF_CREF	\
			|	SCCP_MTF_RLSD	\
			|	SCCP_MTF_DT1	\
			|	SCCP_MTF_DT2	\
			|	SCCP_MTF_AK	\
			|	SCCP_MTF_ED	\
			|	SCCP_MTF_EA	\
			|	SCCP_MTF_RSR	\
			|	SCCP_MTF_RSC	\
			|	SCCP_MTF_ERR	\
			|	SCCP_MTF_IT	\
			)
/*
 *  Conection-Less Messages
 */
#define SCCP_MTM_CL	(	SCCP_MTF_UDT	\
			|	SCCP_MTF_UDTS	\
			|	SCCP_MTF_XUDT	\
			|	SCCP_MTF_XUDTS	\
			|	SCCP_MTF_LUDT	\
			|	SCCP_MTF_LUDTS	\
			)
/*
 *  Messages that might require GTT
 */
#define SCCP_MTM_GT	( SCCP_MTM_CL | SCCP_MTF_CR )
/*
 *  Protocol Class 0 messages
 */
#define SCCP_MTM_PCLS0	(	SCCP_MTM_CL	)
/*
 *  Protocol Class 1 messages
 */
#define SCCP_MTM_PCLS1	(	SCCP_MTM_CL	)
/*
 *  Protocol Class 2 messages
 */
#define SCCP_MTM_PCLS2	(	SCCP_MTF_CR	\
			|	SCCP_MTF_CC	\
			|	SCCP_MTF_CREF	\
			|	SCCP_MTF_RLSD	\
			|	SCCP_MTF_DT1	\
			|	SCCP_MTF_ERR	\
			|	SCCP_MTF_IT	\
			)
/*
 *  Protocol Class 3 messages
 */
#define SCCP_MTM_PCLS3	(	SCCP_MTF_CR	\
			|	SCCP_MTF_CC	\
			|	SCCP_MTF_CREF	\
			|	SCCP_MTF_RLSD	\
			|	SCCP_MTF_DT2	\
			|	SCCP_MTF_AK	\
			|	SCCP_MTF_ED	\
			|	SCCP_MTF_EA	\
			|	SCCP_MTF_RSR	\
			|	SCCP_MTF_RSC	\
			|	SCCP_MTF_ERR	\
			|	SCCP_MTF_IT	\
			)

#define SCCP_PT_EOP	 0	/* End of Optional Parameters */
#define SCCP_PT_DLR	 1	/* Destination Local Reference */
#define SCCP_PT_SLR	 2	/* Source Local reference */
#define SCCP_PT_CDPA	 3	/* Called Party Address */
#define SCCP_PT_CGPA	 4	/* Calling Party Address */
#define SCCP_PT_PCLS	 5	/* Protocol Class */
#define SCCP_PT_SEG	 6	/* Segmenting/Reassembly */
#define SCCP_PT_RSN	 7	/* Receive Sequence Number */
#define SCCP_PT_SEQ	 8	/* Sequencing/Segmenting */
#define SCCP_PT_CRED	 9	/* Credit */
#define SCCP_PT_RELC	10	/* Release Cause */
#define SCCP_PT_RETC	11	/* Return Cause */
#define SCCP_PT_RESC	12	/* Reset Cause */
#define SCCP_PT_ERRC	13	/* Error Cause */
#define SCCP_PT_REFC	14	/* Refusal Cause */
#define SCCP_PT_DATA	15	/* Data */
#define SCCP_PT_SGMT	16	/* Segmentation */
#define SCCP_PT_HOPC	17	/* Hop Counter */
#define SCCP_PT_IMP	18	/* Importance */
#define SCCP_PT_LDATA	19	/* Long Data */
#define SCCP_PT_ISNI	-6	/* Intermediate Sig. Ntwk. Id */
#define SCCP_PT_INS	-7	/* Intermediate Network Selection */
#define SCCP_PT_MTI	-8	/* Message Type Interworking */

#define SCCP_PTF_EOP	(1<<SCCP_PT_EOP  )
#define SCCP_PTF_DLR	(1<<SCCP_PT_DLR  )
#define SCCP_PTF_SLR	(1<<SCCP_PT_SLR  )
#define SCCP_PTF_CDPA	(1<<SCCP_PT_CDPA )
#define SCCP_PTF_CGPA	(1<<SCCP_PT_CGPA )
#define SCCP_PTF_PCLS	(1<<SCCP_PT_PCLS )
#define SCCP_PTF_SEG	(1<<SCCP_PT_SEG  )
#define SCCP_PTF_RSN	(1<<SCCP_PT_RSN  )
#define SCCP_PTF_SEQ	(1<<SCCP_PT_SEQ  )
#define SCCP_PTF_CRED	(1<<SCCP_PT_CRED )
#define SCCP_PTF_RELC	(1<<SCCP_PT_RELC )
#define SCCP_PTF_RETC	(1<<SCCP_PT_RETC )
#define SCCP_PTF_RESC	(1<<SCCP_PT_RESC )
#define SCCP_PTF_ERRC	(1<<SCCP_PT_ERRC )
#define SCCP_PTF_REFC	(1<<SCCP_PT_REFC )
#define SCCP_PTF_DATA	(1<<SCCP_PT_DATA )
#define SCCP_PTF_SGMT	(1<<SCCP_PT_SGMT )
#define SCCP_PTF_HOPC	(1<<SCCP_PT_HOPC )
#define SCCP_PTF_IMP	(1<<SCCP_PT_IMP  )
#define SCCP_PTF_LDATA	(1<<SCCP_PT_LDATA)
#define SCCP_PTF_MTI	(0x80000000>>-SCCP_PT_MTI )
#define SCCP_PTF_INS	(0x80000000>>-SCCP_PT_INS )
#define SCCP_PTF_ISNI	(0x80000000>>-SCCP_PT_ISNI)

#define SCCP_MTI_OMT_UNQUAL	0x00
#define SCCP_MTI_OMT_UDT_S	0x01
#define SCCP_MTI_OMT_XUDT_S	0x02
#define SCCP_MTI_OMT_LUDT_S	0x03
#define SCCP_MTI_DROP		0x40

union nids {
	struct {
		uint8_t network __attribute__ ((packed));
		uint8_t cluster __attribute__ ((packed));
	} nid __attribute__ ((packed));
	uint16_t nsi __attribute__ ((packed));
};

typedef struct sccp_var {
	ulong len;
	uchar *ptr;
	// ulong val;
	mblk_t *buf;
} sccp_var_t;

typedef struct sccp_isni {
	ulong mi;			/* mark for identification indicator */
	ulong iri;			/* ISNI routing indicator */
	ulong ti;			/* type indicator */
	ulong count;			/* counter */
	ulong ns;			/* network specific (type 2 only) */
	size_t nids;			/* number of network ids */
	union nids u[7];		/* up to 7 network ids */
} sccp_isni_t;

typedef struct sccp_ins {
	ulong itype;			/* information type indicator */
	ulong rtype;			/* type of routing */
	ulong count;			/* conter */
	size_t nids;			/* number of network ids */
	union nids u[2];		/* up to 2 network ids */
} sccp_ins_t;

typedef struct sccp_sgmt {
	ulong first;			/* first bit */
	ulong pcl;			/* stored protocol class */
	ulong rem;			/* remaining segments */
	ulong slr;			/* source local reference */
} sccp_sgmt_t;

typedef struct sccp_msg {
	ulong flags;			/* translation result flags */
	mblk_t *bp;			/* identd (also unique GTT xaction id */
	queue_t *eq;			/* queue to write errors to */
	queue_t *xq;			/* queue to write results to */
	struct sc *sc;			/* SCCP-User to which this message belongs */
	struct mt *mt;			/* MTP-User to which this message belongs */
	ulong timestamp;		/* jiffie clock timestamp */
	ulong pvar;			/* protocol variant */
	struct {
		ulong opc;		/* origination point code */
		ulong dpc;		/* destination point code */
		ulong sls;		/* signalling link selection */
		ulong mp;		/* message priority */
	} rl;				/* routing label */
	ulong type;			/* message type */
	uint32_t parms;			/* parameter mask */
	ulong pcl;
	ulong ret;
	ulong slr;
	ulong dlr;
	ulong seg;
	ulong rsn;
	ulong ps;
	ulong more;
	ulong pr;
	ulong mti;
	sccp_ins_t ins;
	sccp_isni_t isni;
	struct sccp_addr cdpa;
	struct sccp_addr cgpa;
	ulong cred;
	ulong cause;
	struct sccp_sgmt sgmt;
	ulong hopc;
	ulong imp;
	struct sccp_var data;
	ulong fi;
	ulong assn;
	ulong apc;
	ulong smi;
	ulong cong;
} sccp_msg_t;

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
#if 0
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
#endif

/*
 *  M_ERROR
 *  -----------------------------------
 */
#if 0
STATIC int
m_error(queue_t *q, struct sc *sc, int error)
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
		sccp_set_state(sc, NS_NOSTATES);
		printd(("%s: %p: <- M_HANGUP\n", DRV_NAME, sc));
		ss7_oput(sc->oq, mp);
		return (-error);
	} else {
		mp->b_datap->db_type = M_ERROR;
		*(mp->b_wptr)++ = error < 0 ? -error : error;
		*(mp->b_rptr)++ = error < 0 ? -error : error;
		sccp_set_state(sc, NS_NOSTATES);
		printd(("%s: %p: <- M_ERROR\n", DRV_NAME, sc));
		ss7_oput(sc->oq, mp);
		return (QR_DONE);
	}
      enobufs:
	rare();
	return (-ENOBUFS);
}
#endif

/*
 *  NPI Primitives
 *  -------------------------------------------------------------------------
 */
/*
 *  N_CONN_IND
 *  -----------------------------------
 */
STATIC INLINE int
n_conn_ind(queue_t *q, struct sc *sc, mblk_t *cp)
{
	int err;
	mblk_t *mp;
	N_conn_ind_t *p;
	N_qos_sel_conn_sccp_t qos;
	size_t dst_len = sizeof(sc->dst) + sc->dst.alen;
	size_t src_len = sizeof(sc->src) + sc->src.alen;
	size_t qos_len = sizeof(qos);
	if ((1 << sccp_get_n_state(sc)) & ~(NSF_IDLE | NSF_WRES_CIND))
		goto efault;
	if (!canputnext(sc->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, sizeof(*p) + dst_len + src_len + qos_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = N_CONN_IND;
	p->DEST_length = dst_len;
	p->DEST_offset = dst_len ? sizeof(*p) : 0;
	p->SRC_length = src_len;
	p->SRC_offset = src_len ? sizeof(*p) + dst_len : 0;
	p->SEQ_number = (ulong) cp;
	p->CONN_flags = REC_CONF_OPT | EX_DATA_OPT;
	p->QOS_length = qos_len;
	p->QOS_offset = qos_len ? sizeof(*p) + dst_len + src_len : 0;
	if (dst_len) {
		fixme(("Need to take this from the cp not sc structure\n"));
		bcopy(&sc->dst, mp->b_wptr, dst_len);
		mp->b_wptr += dst_len;
	}
	if (src_len) {
		fixme(("Need to take this from the cp not sc structure\n"));
		bcopy(&sc->src, mp->b_wptr, dst_len);
		mp->b_wptr += src_len;
	}
	if (qos_len) {
		qos.n_qos_type = N_QOS_SEL_CONN_SCCP;
		qos.protocol_class = sc->pcl;
		bcopy(&qos, mp->b_wptr, qos_len);
		mp->b_wptr += qos_len;
	}
	bufq_queue(&sc->conq, cp);
	sccp_set_n_state(sc, NS_WRES_CIND);
	putnext(sc->oq, mp);
	return (QR_ABSORBED);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sc));
	goto error;
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: Flow controlled\n", DRV_NAME, sc));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: SWERR: unexpected indication for state %ld\n", DRV_NAME, sc,
		sccp_get_n_state(sc)));
	goto error;
      error:
	return (err);
}

/*
 *  N_CONN_CON
 *  -----------------------------------
 */
STATIC INLINE int
n_conn_con(queue_t *q, struct sc *sc, ulong pcl, ulong flags, struct sccp_addr *res, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	N_conn_con_t *p;
	N_qos_sel_conn_sccp_t *qos;
	size_t res_len = res ? sizeof(*res) + res->alen : 0;
	size_t qos_len = sizeof(*qos);
	if (sccp_get_n_state(sc) != NS_WCON_CREQ)
		goto efault;
	if (!bcanputnext(sc->oq, 1))
		goto ebusy;
	if (!(mp = ss7_allocb(q, sizeof(*p) + PAD4(res_len) + qos_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	mp->b_band = 1;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = N_CONN_CON;
	p->RES_length = res_len;
	p->RES_offset = res_len ? sizeof(*p) : 0;
	p->CONN_flags = flags;
	p->QOS_length = qos_len;
	p->QOS_offset = qos_len ? sizeof(*p) + PAD4(res_len) : 0;
	if (res_len) {
		bcopy(res, mp->b_wptr, res_len);
		mp->b_wptr += res_len;
	}
	if (qos_len) {
		qos = ((typeof(qos)) mp->b_wptr)++;
		qos->n_qos_type = N_QOS_SEL_CONN_SCCP;
		qos->protocol_class = pcl;
	}
	mp->b_cont = dp;
	sccp_set_n_state(sc, NS_DATA_XFER);
	putnext(sc->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sc));
	goto error;
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: Flow controlled\n", DRV_NAME, sc));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: SWERR: unexpected indication for state %ld\n", DRV_NAME, sc,
		sccp_get_n_state(sc)));
	goto error;
      error:
	return (err);
}

/*
 *  N_DISCON_IND
 *  -----------------------------------
 */
STATIC INLINE int
n_discon_ind(queue_t *q, struct sc *sc, ulong orig, long reason, struct sccp_addr *res, mblk_t *seq,
	     mblk_t *dp)
{
	int err;
	mblk_t *mp;
	N_discon_ind_t *p;
	size_t res_len = res ? sizeof(*res) + res->alen : 0;
	if ((1 << sccp_get_n_state(sc)) &
	    ~(NSF_WCON_CREQ | NSF_WRES_CIND | NSF_DATA_XFER | NSF_WCON_RREQ | NSF_WRES_RIND))
		goto efault;
	if (!canputnext(sc->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, sizeof(*p) + PAD4(res_len), BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = N_DISCON_IND;
	p->DISCON_orig = orig;
	p->DISCON_reason = reason;
	p->RES_length = res_len;
	p->RES_offset = res_len ? sizeof(*p) : 0;
	p->SEQ_number = (ulong) seq;
	if (res_len) {
		bcopy(res, mp->b_wptr, res_len);
		mp->b_wptr += res_len;
	}
	if (seq) {
		bufq_unlink(&sc->conq, seq);
		freemsg(seq);
	}
	mp->b_cont = dp;
	if (!bufq_length(&sc->conq))
		sccp_set_n_state(sc, NS_IDLE);
	else
		sccp_set_n_state(sc, NS_WRES_CIND);
	putnext(sc->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sc));
	goto error;
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: Flow controlled\n", DRV_NAME, sc));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: SWERR: unexpected indication for state %ld\n", DRV_NAME, sc,
		sccp_get_n_state(sc)));
	goto error;
      error:
	return (err);
}

/*
 *  N_DATA_IND
 *  -----------------------------------
 */
STATIC INLINE int
n_data_ind(queue_t *q, struct sc *sc, ulong more, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	N_data_ind_t *p;
	N_qos_sel_data_sccp_t *qos;
	size_t qos_len = sizeof(*qos);
	if (sccp_get_n_state(sc) != NS_DATA_XFER)
		goto efault;
	if (!canputnext(sc->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, sizeof(*p) + qos_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = N_DATA_IND;
	p->DATA_xfer_flags = (more ? N_MORE_DATA_FLAG : 0);
	qos = ((typeof(qos)) mp->b_wptr)++;
	qos->n_qos_type = N_QOS_SEL_DATA_SCCP;
	qos->protocol_class = sc->pcl;	/* FIXME */
	qos->option_flags = sc->flags;	/* FIXME */
	qos->importance = sc->imp;	/* FIXME */
	qos->sequence_selection = sc->sls;	/* FIXME */
	qos->message_priority = sc->mp;	/* FIXME */
	mp->b_cont = dp;
	putnext(sc->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sc));
	goto error;
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: Flow controlled\n", DRV_NAME, sc));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: SWERR: unexpected indication for state %ld\n", DRV_NAME, sc,
		sccp_get_n_state(sc)));
	goto error;
      error:
	return (err);
}

/*
 *  N_EXDATA_IND
 *  -----------------------------------
 */
STATIC INLINE int
n_exdata_ind(queue_t *q, struct sc *sc, ulong more, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	N_exdata_ind_t *p;
	N_qos_sel_data_sccp_t *qos;
	size_t qos_len = sizeof(*qos);
	if (sccp_get_n_state(sc) != NS_DATA_XFER)
		goto efault;
	if (!bcanputnext(sc->oq, 1))
		goto ebusy;
	if (!(mp = ss7_allocb(q, sizeof(*p) + qos_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	mp->b_band = 1;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = N_EXDATA_IND;
	qos = ((typeof(qos)) mp->b_wptr)++;
	qos->n_qos_type = N_QOS_SEL_DATA_SCCP;
	qos->protocol_class = sc->pcl;	/* FIXME */
	qos->option_flags = sc->flags;	/* FIXME */
	qos->importance = sc->imp;	/* FIXME */
	qos->sequence_selection = sc->sls;	/* FIXME */
	qos->message_priority = sc->mp;	/* FIXME */
	mp->b_cont = dp;
	putnext(sc->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sc));
	goto error;
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: Flow controlled\n", DRV_NAME, sc));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: SWERR: unexpected indication for state %ld\n", DRV_NAME, sc,
		sccp_get_n_state(sc)));
	goto error;
      error:
	return (err);
}

/*
 *  N_INFO_ACK
 *  -----------------------------------
 */
STATIC INLINE int
n_info_ack(queue_t *q, struct sc *sc)
{
	int err;
	mblk_t *mp;
	N_info_ack_t *p;
	N_qos_sel_info_sccp_t *qos;
	N_qos_range_info_sccp_t *qor;
	struct sccp_addr *add, *ss = &sc->src;
	uint32_t *pro;
	size_t add_len = ss ? sizeof(*ss) + ss->alen : 0;
	size_t qos_len = sizeof(*qos);
	size_t qor_len = sizeof(*qor);
	size_t pro_len = sizeof(*pro);
	if (!(mp = ss7_allocb(q, sizeof(*p) + add_len + qos_len + qor_len + pro_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = N_INFO_ACK;
	p->NSDU_size = -1;	/* no limit on NSDU size */
	p->ENSDU_size = -1;	/* no limit on ENSDU size */
	p->CDATA_size = -1;	/* no limit on CDATA size */
	p->DDATA_size = -1;	/* no limit on DDATA size */
	p->ADDR_size = sizeof(struct sccp_addr) + SCCP_MAX_ADDR_LENGTH;
	p->ADDR_length = add_len;
	p->ADDR_offset = add_len ? sizeof(*p) : 0;
	p->QOS_length = qos_len;
	p->QOS_offset = qos_len ? sizeof(*p) + add_len : 0;
	p->QOS_range_length = qor_len;
	p->QOS_range_offset = qor_len ? sizeof(*p) + add_len + qos_len : 0;
	p->OPTIONS_flags =
	    REC_CONF_OPT | EX_DATA_OPT | ((sc->flags & SCCPF_DEFAULT_RC_SEL) ? DEFAULT_RC_SEL : 0);
	p->NIDU_size = -1;
	p->SERV_type = N_CONS | N_CLNS;
	p->CURRENT_state = sccp_get_n_state(sc);
	p->PROVIDER_type = N_SUBNET;
	p->NODU_size = 254;
	p->PROTOID_length = pro_len;
	p->PROTOID_offset = pro_len ? sizeof(*p) + add_len + qos_len + qor_len : 0;
	p->NPI_version = N_CURRENT_VERSION;
	if (add_len) {
		add = (typeof(add)) mp->b_wptr;
		bcopy(ss, add, sizeof(*ss) + ss->alen);
		mp->b_wptr += add_len;
	}
	if (qos_len) {
		qos = ((typeof(qos)) mp->b_wptr)++;
		qos->n_qos_type = N_QOS_SEL_INFO_SCCP;
		qos->protocol_class = sc->pcl;
		qos->option_flags = sc->flags;
	}
	if (qor_len) {
		qor = ((typeof(qor)) mp->b_wptr)++;
		qor->n_qos_type = N_QOS_RANGE_INFO_SCCP;
		qor->protocol_classes = sc->pcl;
		qor->sequence_selection = sc->sls;
	}
	if (pro_len) {
		pro = ((typeof(pro)) mp->b_wptr)++;
		*pro = 3;	/* SI value for SCCP */
	}
	putnext(sc->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sc));
	goto error;
	goto error;
      error:
	return (err);
}

/*
 *  N_BIND_ACK
 *  -----------------------------------
 */
STATIC INLINE int
n_bind_ack(queue_t *q, struct sc *sc)
{
	int err;
	mblk_t *mp;
	N_bind_ack_t *p;
	struct sccp_addr *add, *ss = &sc->src;
	uint32_t *pro;
	size_t add_len = ss ? sizeof(*ss) + ss->alen : 0;
	size_t pro_len = sizeof(*pro);
	size_t msg_len = sizeof(*p) + add_len + pro_len;
	if (sccp_get_n_state(sc) != NS_UNBND)
		goto efault;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = N_BIND_ACK;
	p->ADDR_length = add_len;
	p->ADDR_offset = add_len ? sizeof(*p) : 0;
	p->CONIND_number = sc->conind;
	p->TOKEN_value = (ulong) sc->oq;
	p->PROTOID_length = pro_len;
	p->PROTOID_offset = pro_len ? sizeof(*p) + add_len : 0;
	if (add_len) {
		add = (typeof(add)) mp->b_wptr;
		bcopy(ss, add, sizeof(*ss) + ss->alen);
		mp->b_wptr += add_len;
	}
	if (pro_len) {
		pro = ((typeof(pro)) mp->b_wptr)++;
		*pro = 3;	/* SI value for SCCP */
	}
	sccp_set_n_state(sc, NS_IDLE);
	putnext(sc->oq, mp);
	return (0);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sc));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: SWERR: unexpected indication for state %ld\n", DRV_NAME, sc,
		sccp_get_n_state(sc)));
	goto error;
      error:
	return (err);
}

/*
 *  N_ERROR_ACK
 *  -----------------------------------
 */
STATIC INLINE int
n_error_ack(queue_t *q, struct sc *sc, const long prim, int err)
{
	mblk_t *mp;
	N_error_ack_t *p;
	switch (err) {
	case -EBUSY:
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
	case 0:
		return (err);
	}
	if (!(mp = ss7_allocb(q, sizeof(*p), BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = N_ERROR_ACK;
	p->ERROR_prim = prim;
	p->NPI_error = err < 0 ? NSYSERR : err;
	p->UNIX_error = err < 0 ? -err : 0;
	switch (sccp_get_n_state(sc)) {
	case NS_WACK_OPTREQ:
	case NS_WACK_UREQ:
	case NS_WCON_CREQ:
		sccp_set_n_state(sc, NS_IDLE);
		break;
	case NS_WCON_RREQ:
		sccp_set_n_state(sc, NS_DATA_XFER);
		break;
	case NS_WACK_BREQ:
		sccp_set_n_state(sc, NS_UNBND);
		break;
	case NS_WACK_CRES:
		sccp_set_n_state(sc, NS_WRES_CIND);
		break;
	case NS_WACK_DREQ6:
		sccp_set_n_state(sc, NS_WCON_CREQ);
		break;
	case NS_WACK_DREQ7:
		sccp_set_n_state(sc, NS_WRES_CIND);
		break;
	case NS_WACK_DREQ9:
		sccp_set_n_state(sc, NS_DATA_XFER);
		break;
	case NS_WACK_DREQ10:
		sccp_set_n_state(sc, NS_WCON_RREQ);
		break;
	case NS_WACK_DREQ11:
		sccp_set_n_state(sc, NS_WRES_RIND);
		break;
	default:
		/* 
		   Note: if we are not in a WACK state we simply do not change state.  This occurs
		   normally when we send NOUTSTATE or NNOTSUPPORT or are responding to an
		   N_OPTMGMT_REQ in other than the NS_IDLE state. */
		break;
	}
	putnext(sc->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sc));
	goto error;
      error:
	return (err);
}

/*
 *  N_OK_ACK
 *  -----------------------------------
 */
STATIC INLINE int
n_ok_ack(queue_t *q, struct sc *sc, long prim, ulong seq, ulong tok)
{
	int err;
	mblk_t *mp;
	N_ok_ack_t *p;
	if (!(mp = ss7_allocb(q, sizeof(*p), BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = N_OK_ACK;
	p->CORRECT_prim = prim;
	switch (sccp_get_n_state(sc)) {
	case NS_WACK_OPTREQ:
		sccp_set_n_state(sc, NS_IDLE);
		break;
	case NS_WACK_RRES:
		sccp_set_n_state(sc, NS_DATA_XFER);
		break;
	case NS_WACK_UREQ:
		sccp_set_n_state(sc, NS_UNBND);
		break;
	case NS_WACK_CRES:
	{
		queue_t *aq = (queue_t *) tok;
		struct sc *ap = SCCP_PRIV(aq);
		if (ap) {
			ap->i_state = NS_DATA_XFER;
			// sccp_cleanup_read(sc->oq); /* deliver to user what is possible */
			// sccp_transmit_wakeup(sc->oq); /* reply to peer what is necessary */
		}
		if (seq) {
			bufq_unlink(&sc->conq, (mblk_t *) seq);
			freemsg((mblk_t *) seq);
		}
		if (aq != sc->oq) {
			if (bufq_length(&sc->conq))
				sccp_set_n_state(sc, NS_WRES_CIND);
			else
				sccp_set_n_state(sc, NS_IDLE);
		}
		break;
	}
	case NS_WACK_DREQ7:
		if (seq)
			bufq_unlink(&sc->conq, (mblk_t *) seq);
	case NS_WACK_DREQ6:
	case NS_WACK_DREQ9:
	case NS_WACK_DREQ10:
	case NS_WACK_DREQ11:
		if (bufq_length(&sc->conq))
			sccp_set_n_state(sc, NS_WRES_CIND);
		else
			sccp_set_n_state(sc, NS_IDLE);
		break;
	default:
		/* 
		   Note: if we are not in a WACK state we simply do not change state.  This occurs
		   normally when we are responding to a N_OPTMGMT_REQ in other than the NS_IDLE
		   state. */
		break;
	}
	putnext(sc->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sc));
	goto error;
      error:
	return (err);
}

/*
 *  N_UNITDATA_IND
 *  -----------------------------------
 */
STATIC INLINE int
n_unitdata_ind(queue_t *q, struct sc *sc, struct sccp_addr *src, struct sccp_addr *dst, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	N_unitdata_ind_t *p;
	size_t src_len = src ? sizeof(*src) + src->alen : 0;
	size_t dst_len = dst ? sizeof(*dst) + dst->alen : 0;
	if (sccp_get_n_state(sc) != NS_IDLE)
		goto efault;
	if (!canputnext(sc->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, sizeof(*p) + PAD4(src_len) + PAD4(dst_len), BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = N_UNITDATA_IND;
	p->SRC_length = src_len;
	p->SRC_offset = src_len ? sizeof(*p) : 0;
	p->DEST_length = dst_len;;
	p->DEST_offset = dst_len ? sizeof(*p) + PAD4(src_len) : 0;
	p->ERROR_type = 0;
	if (src_len) {
		bcopy(src, mp->b_wptr, src_len);
		mp->b_wptr += PAD4(src_len);
	}
	if (dst_len) {
		bcopy(dst, mp->b_wptr, dst_len);
		mp->b_wptr += PAD4(dst_len);
	}
	mp->b_cont = dp;
	putnext(sc->oq, mp);
	return (QR_ABSORBED);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sc));
	goto error;
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: Flow controlled\n", DRV_NAME, sc));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: SWERR: unexpected indication for state %ld\n", DRV_NAME, sc,
		sccp_get_n_state(sc)));
	goto error;
      error:
	return (err);
}

/*
 *  N_UDERROR_IND
 *  -----------------------------------
 *
 *  NOTE:- We might want to do a special N-NOTICE primitive here because the N_UDERROR_IND primitive does not have
 *  that much information about the returned message.  Or, if we attach the M_DATA block, the use might be able to
 *  gleen more information from the message itself.  Another approach is to stuff some extra information at the end
 *  of the primitive.  Here we attache the returned message in the M_DATA portion.
 */
STATIC INLINE int
n_uderror_ind(queue_t *q, struct sc *sc, struct sccp_addr *dst, mblk_t *dp, ulong etype)
{
	int err;
	mblk_t *mp;
	N_uderror_ind_t *p;
	size_t dst_len = dst ? sizeof(*dst) + dst->alen : 0;
	if (sccp_get_n_state(sc) != NS_IDLE)
		goto efault;
	if (!canputnext(sc->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, sizeof(*p) + PAD4(dst_len), BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = N_UNITDATA_IND;
	p->DEST_length = dst_len;;
	p->DEST_offset = dst_len ? sizeof(*p) : 0;
	p->RESERVED_field = 0;
	p->ERROR_type = etype;
	if (dst_len) {
		bcopy(dst, mp->b_wptr, dst_len);
		mp->b_wptr += PAD4(dst_len);
	}
	mp->b_cont = dp;
	putnext(sc->oq, mp);
	return (QR_ABSORBED);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sc));
	goto error;
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: Flow controlled\n", DRV_NAME, sc));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: SWERR: unexpected indication for state %ld\n", DRV_NAME, sc,
		sccp_get_n_state(sc)));
	goto error;
      error:
	return (err);
}

/*
 *  N_DATACK_IND
 *  -----------------------------------
 */
STATIC INLINE int
n_datack_ind(queue_t *q, struct sc *sc)
{
	int err;
	mblk_t *mp;
	N_datack_ind_t *p;
	N_qos_sel_data_sccp_t *qos;
	size_t qos_len = sizeof(*qos);
	if (sccp_get_n_state(sc) != NS_DATA_XFER)
		goto efault;
	if (!canputnext(sc->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, sizeof(*p) + qos_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = N_DATACK_IND;
	qos = ((typeof(qos)) mp->b_wptr)++;
	qos->n_qos_type = N_QOS_SEL_DATA_SCCP;
	qos->protocol_class = sc->pcl;	/* FIXME */
	qos->option_flags = sc->flags;	/* FIXME */
	qos->importance = sc->imp;	/* FIXME */
	qos->sequence_selection = sc->sls;	/* FIXME */
	qos->message_priority = sc->mp;	/* FIXME */
	putnext(sc->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sc));
	goto error;
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: Flow controlled\n", DRV_NAME, sc));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: SWERR: unexpected indication for state %ld\n", DRV_NAME, sc,
		sccp_get_n_state(sc)));
	goto error;
      error:
	return (err);
}

/*
 *  N_RESET_IND
 *  -----------------------------------
 */
STATIC INLINE int
n_reset_ind(queue_t *q, struct sc *sc, ulong orig, ulong reason, mblk_t *cp)
{
	int err;
	mblk_t *mp;
	N_reset_ind_t *p;
	if (sccp_get_n_state(sc) != NS_DATA_XFER)
		goto efault;
	if (!canputnext(sc->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, sizeof(*p), BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = N_RESET_IND;
	p->RESET_orig = orig;
	p->RESET_reason = reason;
	bufq_queue(&sc->conq, cp);
	sccp_set_n_state(sc, NS_WRES_RIND);
	putnext(sc->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sc));
	goto error;
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: Flow controlled\n", DRV_NAME, sc));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: SWERR: unexpected indication for state %ld\n", DRV_NAME, sc,
		sccp_get_n_state(sc)));
	goto error;
      error:
	return (err);
}

/*
 *  N_RESET_CON
 *  -----------------------------------
 */
STATIC INLINE int
n_reset_con(queue_t *q, struct sc *sc)
{
	int err;
	mblk_t *mp;
	N_reset_con_t *p;
	if (sccp_get_n_state(sc) != NS_WCON_RREQ)
		goto efault;
	if (!canputnext(sc->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, sizeof(*p), BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = N_RESET_CON;
	sccp_set_n_state(sc, NS_DATA_XFER);
	putnext(sc->oq, mp);
	return (QR_DONE);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sc));
	goto error;
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: Flow controlled\n", DRV_NAME, sc));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: SWERR: unexpected indication for state %ld\n", DRV_NAME, sc,
		sccp_get_n_state(sc)));
	goto error;
      error:
	return (err);
}

#if 0
/*
 *  N_RECOVER_IND       29 - NC Recovery indication
 *  ---------------------------------------------------------
 */
STATIC INLINE int
n_recover_ind(queue_t *q, struct sc *sc)
{
	int err;
	mblk_t *mp;
	N_recover_ind_t *p;
	if (!canputnext(sc->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, sizeof(*p), BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = N_RECOVER_IND;
	sccp_set_n_state(sc, NS_DATA_XFER);
	putnext(sc->oq, mp);
	return (0);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sc));
	goto error;
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: Flow controlled\n", DRV_NAME, sc));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: SWERR: unexpected indication for state %ld\n", DRV_NAME, sc,
		sccp_get_n_state(sc)));
	goto error;
      error:
	return (err);
}

/*
 *  N_RETRIEVE_IND      32 - NC Retrieval indication
 *  ---------------------------------------------------------
 */
STATIC int
n_retrieve_ind(queue_t *q, struct sc *sc, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	N_retrieve_ind_t *p;
	if ((1 << sccp_get_n_state(sc)) & ~(NSF_IDLE))
		goto efault;
	if (!canputnext(sc->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, sizeof(*p), BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = N_RETREIVE_IND;
	mp->b_cont = dp;
	sccp_set_n_state(sc, NS_IDLE);
	putnext(sc->oq, mp);
	return (0);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sc));
	goto error;
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: Flow controlled\n", DRV_NAME, sc));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: SWERR: unexpected indication for state %ld\n", DRV_NAME, sc,
		sccp_get_n_state(sc)));
	goto error;
      error:
	return (err);
}

/*
 *  N_RETRIEVE_CON      33 - NC Retrieval complete confirmation
 *  -----------------------------------------------------------
 */
STATIC int
n_retrieve_con(queue_t *q, struct sc *sc)
{
	int err;
	mblk_t *mp;
	N_retrieve_con_t *p;
	if ((1 << sccp_get_n_state(sc)) & ~(NSF_IDLE))
		goto efault;
	if (!canputnext(sc->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, sizeof(*p), BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = N_RETRIEVE_CON;
	sccp_set_n_state(sc, NS_IDLE);
	putnext(sc->oq, mp);
	return (0);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sc));
	goto error;
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: Flow controlled\n", DRV_NAME, sc));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: SWERR: unexpected indication for state %ld\n", DRV_NAME, sc,
		sccp_get_n_state(sc)));
	goto error;
      error:
	return (err);
}
#endif

#if 0
/*
 *  N_ORDREL_IND
 *  -----------------------------------------------------------------
 *  There is really no orderly release for NPI, so we just echo an orderly
 *  releae request back.
 */
STATIC int sccp_ordrel_req(queue_t *q, struct sc *sc);
STATIC INLINE int
n_ordrel_ind(queue_t *q, struct sc *sc)
{
	return sccp_ordrel_req(q, sc);
}
#endif

/*
 *  N_NOTICE_IND
 *  -----------------------------------------------------------------
 */
STATIC INLINE int
n_notice_ind(queue_t *q, struct sc *sc, ulong cause, struct sccp_addr *dst, struct sccp_addr *src,
	     ulong pri, ulong seq, ulong pcl, ulong imp, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	N_notice_ind_t *p;
	N_qos_sel_data_sccp_t *qos;
	size_t dst_len = dst ? sizeof(*dst) + dst->alen : 0;
	size_t src_len = src ? sizeof(*src) + src->alen : 0;
	size_t qos_len = sizeof(*qos);
	size_t msg_len = sizeof(*p) + PAD4(dst_len) + PAD4(src_len) + qos_len;
	if (!(canputnext(sc->oq)))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = N_NOTICE_IND;
	p->DEST_length = dst_len;
	p->DEST_offset = dst_len ? sizeof(*p) : 0;
	p->SRC_length = src_len;
	p->SRC_offset = src_len ? sizeof(*p) + PAD4(dst_len) : 0;
	p->QOS_length = qos_len;
	p->QOS_offset = sizeof(*p) + PAD4(dst_len) + PAD4(src_len);
	p->RETURN_cause = cause;
	if (dst_len) {
		bcopy(dst, mp->b_wptr, dst_len);
		mp->b_wptr += PAD4(dst_len);
	}
	if (src_len) {
		bcopy(src, mp->b_wptr, src_len);
		mp->b_wptr += PAD4(src_len);
	}
	qos = (typeof(qos)) mp->b_wptr;
	mp->b_wptr += sizeof(*qos);
	qos->n_qos_type = N_QOS_SEL_DATA_SCCP;
	qos->protocol_class = pcl;
	qos->option_flags = 1;
	qos->importance = imp;
	qos->sequence_selection = seq;
	qos->message_priority = pri;
	printd(("%s: %p: <- N_NOTICE_IND\n", DRV_NAME, sc));
	putnext(sc->oq, mp);
	return (QR_DONE);
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: Flow controlled\n", DRV_NAME, sc));
	goto error;
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sc));
	goto error;
      error:
	return (err);
}

/*
 *  N_INFORM_IND
 *  -----------------------------------------------------------------
 */
STATIC INLINE int
n_inform_ind(queue_t *q, struct sc *sc, N_qos_sel_infr_sccp_t * qos, ulong reason)
{
	int err;
	mblk_t *mp;
	N_inform_ind_t *p;
	size_t qos_len = qos ? sizeof(*qos) : 0;
	size_t msg_len = sizeof(*p) + qos_len;
	if (!(canputnext(sc->oq)))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = N_INFORM_IND;
	p->QOS_length = qos_len;
	p->QOS_offset = qos_len ? sizeof(*p) : 0;
	p->REASON = reason;
	if (qos_len) {
		bcopy(qos, mp->b_wptr, qos_len);
		mp->b_wptr += qos_len;
	}
	printd(("%s: %p: <- N_INFORM_IND\n", DRV_NAME, sc));
	putnext(sc->oq, mp);
	return (QR_DONE);
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: Flow controlled\n", DRV_NAME, sc));
	goto error;
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sc));
	goto error;
      error:
	return (err);
}

/*
 *  N_COORD_IND
 *  -----------------------------------------------------------------
 */
STATIC INLINE int
n_coord_ind(queue_t *q, struct sc *sc, struct sccp_addr *add)
{
	int err;
	mblk_t *mp;
	N_coord_ind_t *p;
	size_t add_len = add ? sizeof(*add) + add->alen : 0;
	size_t msg_len = sizeof(*p) + add_len;
	if (!canputnext(sc->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = N_COORD_IND;
	p->ADDR_length = add_len;
	p->ADDR_offset = add_len ? sizeof(*p) : 0;
	if (add_len) {
		bcopy(add, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
	}
	printd(("%s: %p: <- N_COORD_IND\n", DRV_NAME, sc));
	putnext(sc->oq, mp);
	return (QR_DONE);
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: Flow controlled\n", DRV_NAME, sc));
	goto error;
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sc));
	goto error;
      error:
	return (err);
}

/*
 *  N_COORD_CON
 *  -----------------------------------------------------------------
 */
STATIC INLINE int
n_coord_con(queue_t *q, struct sc *sc, struct sccp_addr *add, ulong smi)
{
	int err;
	mblk_t *mp;
	N_coord_con_t *p;
	size_t add_len = add ? sizeof(*add) + add->alen : 0;
	size_t msg_len = sizeof(*p) + add_len;
	if (!canputnext(sc->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = N_COORD_CON;
	p->ADDR_length = add_len;
	p->ADDR_offset = add_len ? sizeof(*p) : 0;
	p->SMI = smi;
	if (add_len) {
		bcopy(add, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
	}
	printd(("%s: %p: <- N_COORD_CON\n", DRV_NAME, sc));
	putnext(sc->oq, mp);
	return (QR_DONE);
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: Flow controlled\n", DRV_NAME, sc));
	goto error;
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sc));
	goto error;
      error:
	return (err);
}

/*
 *  N_STATE_IND
 *  -----------------------------------------------------------------
 */
STATIC INLINE int
n_state_ind(queue_t *q, struct sc *sc, struct sccp_addr *add, ulong status, ulong smi)
{
	int err;
	mblk_t *mp;
	N_state_ind_t *p;
	size_t add_len = add ? sizeof(*add) + add->alen : 0;
	size_t msg_len = sizeof(*p) + add_len;
	if (!canputnext(sc->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = N_STATE_IND;
	p->ADDR_length = add_len;
	p->ADDR_offset = add_len ? sizeof(*p) : 0;
	p->STATUS = status;
	p->SMI = smi;
	if (add_len) {
		bcopy(add, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
	}
	printd(("%s: %p: <- N_STATE_IND\n", DRV_NAME, sc));
	putnext(sc->oq, mp);
	return (QR_DONE);
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: Flow controlled\n", DRV_NAME, sc));
	goto error;
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sc));
	goto error;
      error:
	return (err);
}

/*
 *  N_PCSTATE_IND
 *  -----------------------------------------------------------------
 *  Indicate the state of a remote SCCP User or a remote Signalling Point (MTP).
 */
STATIC INLINE int
n_pcstate_ind(queue_t *q, struct sc *sc, struct mtp_addr *add, ulong status)
{
	int err;
	mblk_t *mp;
	N_pcstate_ind_t *p;
	size_t add_len = add ? sizeof(*add) : 0;
	size_t msg_len = sizeof(*p) + add_len;
	if (!canputnext(sc->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = N_PCSTATE_IND;
	p->ADDR_length = add_len;
	p->ADDR_offset = add_len ? sizeof(*p) : 0;
	p->STATUS = status;
	if (add_len) {
		bcopy(add, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
	}
	printd(("%s: %p: <- N_PCSTATE_IND\n", DRV_NAME, sc));
	putnext(sc->oq, mp);
	return (QR_DONE);
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: Flow controlled\n", DRV_NAME, sc));
	goto error;
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sc));
	goto error;
      error:
	return (err);
}

/*
 *  N_TRAFFIC_IND
 *  -----------------------------------------------------------------
 *  ANSI T1.112.1-2000 2.3.2.2.2 Affected User.  The parameter "affected user" identifies an affected user that is
 *  failed, withdrawn (has requested withdrawal), congested or allowed.  The affected user parameter contains the
 *  same type of information as the called address and calling address.  In the case of N-TRAFFIC primitive (see
 *  2.3.2.3.3), the parameter identifies the user from which a preferred subsystem is receiving backup traffic.
 */
STATIC INLINE int
n_traffic_ind(queue_t *q, struct sc *sc, struct sccp_addr *add, ulong tmix)
{
	int err;
	mblk_t *mp;
	N_traffic_ind_t *p;
	size_t add_len = add ? sizeof(*add) : 0;
	size_t msg_len = sizeof(*p) + add_len;
	if (!canputnext(sc->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = N_TRAFFIC_IND;
	p->ADDR_length = add_len;
	p->ADDR_offset = add_len ? sizeof(*p) : 0;
	p->TRAFFIC_mix = tmix;
	if (add_len) {
		bcopy(add, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
	}
	printd(("%s: %p: <- N_TRAFFIC_IND\n", DRV_NAME, sc));
	putnext(sc->oq, mp);
	return (QR_DONE);
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: Flow controlled\n", DRV_NAME, sc));
	goto error;
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sc));
	goto error;
      error:
	return (err);
}

/*
 *  TPI Primitives
 *  -------------------------------------------------------------------------
 */
/*
 *  T_CONN_IND          11 - connection indication
 *  -----------------------------------------------------------------
 *  We get the information from the CR message in the connection indication.  We queue the CR message (complete with
 *  decode parameter block) itself as the connection indication.  The sequence number is really just a pointer to
 *  the first mblk_t in the received CR message.
 */
STATIC INLINE int
t_conn_ind(queue_t *q, struct sc *sc, mblk_t *cp)
{
	mblk_t *mp;
	struct sccp_msg *m = (typeof(m)) cp->b_rptr;
	struct T_conn_ind *p;
	struct t_opthdr *oh;
	struct sccp_addr *src = &m->cgpa;
	size_t src_len = sizeof(*src) + src->alen;
	size_t opt_len = sizeof(*oh) + sizeof(t_scalar_t);
	size_t msg_len = sizeof(*p) + PAD4(src_len) + opt_len;
	if ((1 << sccp_get_t_state(sc)) & ~(TSF_IDLE | TSF_WRES_CIND))
		goto efault;
	if (bufq_length(&sc->conq) >= sc->conind)
		goto erestart;
	if (!canputnext(sc->oq))
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
	*((t_uscalar_t *) mp->b_wptr)++ = m->pcl;
	bufq_queue(&sc->conq, cp);
	sccp_set_t_state(sc, TS_WRES_CIND);
	putnext(sc->oq, mp);
	return (0);
      enobufs:
	rare();
	return (-ENOBUFS);
      ebusy:
	rare();
	return (-EBUSY);
      erestart:
	ptrace(("%s: %p: PROTO: too many connection indications\n", DRV_NAME, sc));
	return (-ERESTART);
      efault:
	pswerr(("%s: %p: SWERR: unexpected indication for state %ld\n", DRV_NAME, sc,
		sccp_get_t_state(sc)));
	return (-EFAULT);
}

/*
 *  T_CONN_CON          12 - connection confirmation
 *  -----------------------------------------------------------------
 *  The only options with end-to-end significance that are negotiated is the protocol class.
 */
STATIC INLINE int
t_conn_con(queue_t *q, struct sc *sc, ulong pcl, ulong flgas, struct sccp_addr *res, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	struct T_conn_con *p;
	struct t_opthdr *oh;
	size_t res_len = res ? sizeof(*res) + res->alen : 0;
	size_t opt_len = sizeof(*oh) + sizeof(t_scalar_t);
	size_t msg_len = sizeof(*p) + PAD4(res_len) + opt_len;
	if (sccp_get_t_state(sc) != TS_WCON_CREQ)
		goto efault;
	if (!canputnext(sc->oq))
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
	*((t_uscalar_t *) mp->b_wptr)++ = pcl;
	sccp_set_t_state(sc, TS_DATA_XFER);
	putnext(sc->oq, mp);
	return (0);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: no buffers\n", DRV_NAME, sc));
	goto error;
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: flow controlled\n", DRV_NAME, sc));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: SWERR: unexpected indication for state %ld\n", DRV_NAME, sc,
		sccp_get_t_state(sc)));
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
t_discon_ind(queue_t *q, struct sc *sc, long reason, mblk_t *seq, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	struct T_discon_ind *p;
	size_t msg_len = sizeof(*p);
	if ((1 << sc->
	     state) & ~(TSF_WCON_CREQ | TSF_WRES_CIND | TSF_DATA_XFER | TSF_WIND_ORDREL |
			TSF_WREQ_ORDREL))
		goto efault;
	if (!canputnext(sc->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = T_DISCON_IND;
	p->DISCON_reason = reason;
	p->SEQ_number = (ulong) seq;
	if (seq) {
		bufq_unlink(&sc->conq, seq);
		freemsg(seq);
	}
	if (!bufq_length(&sc->conq))
		sccp_set_t_state(sc, TS_IDLE);
	else
		sccp_set_t_state(sc, TS_WRES_CIND);
	mp->b_cont = dp;
	putnext(sc->oq, mp);
	return (0);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: no buffers\n", DRV_NAME, sc));
	goto error;
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: flow controlled\n", DRV_NAME, sc));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: SWERR: unexpected indication for state %ld\n", DRV_NAME, sc,
		sccp_get_t_state(sc)));
	goto error;
      error:
	return (err);
}

/*
 *  T_DATA_IND          14 - data indication
 *  -----------------------------------------------------------------
 */
STATIC INLINE int
t_data_ind(queue_t *q, struct sc *sc, ulong more, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	struct T_data_ind *p;
	size_t msg_len = sizeof(*p);
	if ((1 << sccp_get_t_state(sc)) & ~(TSF_DATA_XFER | TSF_WIND_ORDREL))
		goto efault;
	if (!canputnext(sc->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = T_DATA_IND;
	p->MORE_flag = more;
	mp->b_cont = dp;
	putnext(sc->oq, mp);
	return (0);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: no buffers\n", DRV_NAME, sc));
	goto error;
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: flow controlled\n", DRV_NAME, sc));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: SWERR: unexpected indication for state %ld\n", DRV_NAME, sc,
		sccp_get_t_state(sc)));
	goto error;
      error:
	return (err);
}

/*
 *  T_EXDATA_IND        15 - expedited data indication
 *  -----------------------------------------------------------------
 */
STATIC INLINE int
t_exdata_ind(queue_t *q, struct sc *sc, ulong more, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	struct T_exdata_ind *p;
	size_t msg_len = sizeof(*p);
	if ((1 << sccp_get_t_state(sc)) & ~(TSF_DATA_XFER | TSF_WIND_ORDREL))
		goto efault;
	if (!canputnext(sc->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	mp->b_band = 1;		/* expedite */
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = T_EXDATA_IND;
	p->MORE_flag = more;
	mp->b_cont = dp;
	putnext(sc->oq, mp);
	return (0);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: no buffers\n", DRV_NAME, sc));
	goto error;
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: flow controlled\n", DRV_NAME, sc));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: SWERR: unexpected indication for state %ld\n", DRV_NAME, sc,
		sccp_get_t_state(sc)));
	goto error;
      error:
	return (err);
}

/*
 *  T_INFO_ACK          16 - information acknowledgement
 *  -----------------------------------------------------------------
 */
STATIC INLINE int
t_info_ack(queue_t *q, struct sc *sc)
{
	int err;
	mblk_t *mp;
	struct T_info_ack *p;
	size_t msg_len = sizeof(*p);
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	{
		ulong serv = sc->pcl < 2 ? T_CLTS : T_COTS_ORD;
		ulong etsdu = sc->pcl < 2 ? T_INVALID : sc->mtu;
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		fixme(("Still some things to double-check here\n"));
		p->PRIM_type = T_INFO_ACK;
		p->TSDU_size = T_INFINITE;	/* no limit on TSDU size */
		p->ETSDU_size = etsdu;	/* no concept of ETSDU size */
		p->CDATA_size = sc->mtu;	/* no concept of CDATA size */
		p->DDATA_size = sc->mtu;	/* no concept of DDATA size */
		p->ADDR_size = sizeof(struct sccp_addr);	/* no limit on ADDR size */
		p->OPT_size = T_INFINITE;	/* no limit on OPTIONS size */
		p->TIDU_size = T_INFINITE;	/* no limit on TIDU size */
		p->SERV_type = serv;	/* COTS or CLTS */
		p->CURRENT_state = sccp_get_t_state(sc);
		p->PROVIDER_flag = XPG4_1 & ~T_SNDZERO;
		putnext(sc->oq, mp);
		return (0);
	}
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sc));
	goto error;
      error:
	return (err);
}

/*
 *  T_BIND_ACK          17 - bind acknowledgement
 *  -----------------------------------------------------------------
 */
STATIC int
t_bind_ack(queue_t *q, struct sc *sc, struct sccp_addr *add)
{
	int err;
	mblk_t *mp;
	struct T_bind_ack *p;
	size_t add_len = add ? sizeof(*add) : 0;
	size_t msg_len = sizeof(*p) + add_len;
	if (sccp_get_t_state(sc) != TS_WACK_BREQ)
		goto efault;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = T_BIND_ACK;
	p->ADDR_length = add_len;
	p->ADDR_offset = add_len ? sizeof(*p) : 0;
	p->CONIND_number = sc->conind;
	if (add_len) {
		bcopy(add, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
	}
	sccp_set_t_state(sc, TS_IDLE);
	putnext(sc->oq, mp);
	return (0);
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: SWERR: unexpected indication for state %ld\n", DRV_NAME, sc,
		sccp_get_t_state(sc)));
	goto error;
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sc));
	goto error;
      error:
	return (err);
}

/*
 *  T_ERROR_ACK         18 - error acknowledgement
 *  -----------------------------------------------------------------
 */
STATIC int
t_error_ack(queue_t *q, struct sc *sc, const ulong prim, long error)
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
	switch (sccp_get_t_state(sc)) {
#ifdef TS_WACK_OPTREQ
	case TS_WACK_OPTREQ:
#endif
	case TS_WACK_UREQ:
	case TS_WACK_CREQ:
		sccp_set_t_state(sc, TS_IDLE);
		break;
	case TS_WACK_BREQ:
		sccp_set_t_state(sc, TS_UNBND);
		break;
	case TS_WACK_CRES:
		sccp_set_t_state(sc, TS_WRES_CIND);
		break;
	case TS_WACK_DREQ6:
		sccp_set_t_state(sc, TS_WCON_CREQ);
		break;
	case TS_WACK_DREQ7:
		sccp_set_t_state(sc, TS_WRES_CIND);
		break;
	case TS_WACK_DREQ9:
		sccp_set_t_state(sc, TS_DATA_XFER);
		break;
	case TS_WACK_DREQ10:
		sccp_set_t_state(sc, TS_WIND_ORDREL);
		break;
	case TS_WACK_DREQ11:
		sccp_set_t_state(sc, TS_WREQ_ORDREL);
		break;
		/* 
		   Note: if we are not in a WACK state we simply do not change state.  This occurs
		   normally when we send TOUTSTATE or TNOTSUPPORT or are responding to a
		   T_OPTMGMT_REQ in other then TS_IDLE state. */
	}
	putnext(sc->oq, mp);
	return (0);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sc));
	goto error;
      error:
	return (err);
}

/*
 *  T_OK_ACK            19 - success acknowledgement
 *  -----------------------------------------------------------------
 */
STATIC int
t_ok_ack(queue_t *q, struct sc *sc, ulong prim, ulong seq, ulong tok)
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
	switch (sccp_get_t_state(sc)) {
	case TS_WACK_CREQ:
		sccp_set_t_state(sc, TS_WCON_CREQ);
		break;
	case TS_WACK_UREQ:
		sccp_set_t_state(sc, TS_UNBND);
		break;
	case TS_WACK_CRES:
	{
		queue_t *aq = (queue_t *) tok;
		struct sc *ap = SCCP_PRIV(aq);
		if (ap) {
			ap->i_state = TS_DATA_XFER;
			// sccp_cleanup_read(q);
			// sccp_transmit_wakeup(q);
		}
		if (seq) {
			bufq_unlink(&sc->conq, (mblk_t *) seq);
			freemsg((mblk_t *) seq);
		}
		if (aq != sc->oq) {
			if (bufq_length(&sc->conq))
				sccp_set_t_state(sc, TS_WRES_CIND);
			else
				sccp_set_t_state(sc, TS_IDLE);
		}
		break;
	}
	case TS_WACK_DREQ7:
		if (seq)
			bufq_unlink(&sc->conq, (mblk_t *) seq);
	case TS_WACK_DREQ6:
	case TS_WACK_DREQ9:
	case TS_WACK_DREQ10:
	case TS_WACK_DREQ11:
		if (bufq_length(&sc->conq))
			sccp_set_t_state(sc, TS_WRES_CIND);
		else
			sccp_set_t_state(sc, TS_IDLE);
		break;
		/* 
		   Note: if we are not in a WACK state we simply do not change state.  This occurs
		   normally when we are responding to a T_OPTMGMT_REQ in other than the TS_IDLE
		   state. */
	}
	putnext(sc->oq, mp);
	return (0);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sc));
	goto error;
      error:
	return (err);
}

/*
 *  T_OPTMGMT_ACK       22 - options management acknowledgement
 *  -----------------------------------------------------------------
 */
STATIC int
t_optmgmt_ack(queue_t *q, struct sc *sc, ulong flags, struct sccp_opts *ops)
{
	int err;
	mblk_t *mp;
	struct T_optmgmt_ack *p;
	size_t opt_len = sccp_opts_size(sc, ops);
	size_t msg_len = sizeof(*p) + opt_len;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = T_OPTMGMT_ACK;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) : 0;
	p->MGMT_flags = flags;
	sccp_build_opts(sc, ops, mp->b_wptr);
	mp->b_wptr += opt_len;
#ifdef TS_WACK_OPTREQ
	if (sccp_get_t_state(sc) == TS_WACK_OPTREQ)
		sccp_set_t_state(sc, TS_IDLE);
#endif
	putnext(sc->oq, mp);
	return (0);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sc));
	goto error;
      error:
	return (err);
}

#if 0
/*
 *  T_ORDREL_IND        23 - orderly release indication
 *  -----------------------------------------------------------------
 */
STATIC INLINE int
t_ordrel_ind(queue_t *q, struct sc *sc)
{
	int err;
	mblk_t *mp;
	struct T_ordrel_ind *p;
	size_t msg_len = sizeof(*p);
	if ((1 << sccp_get_t_state(sc)) & ~(TSF_DATA_XFER | TSF_WIND_ORDREL))
		goto efault;
	if (!canputnext(sc->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, msg_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = ((typeof(p)) mp->b_wptr)++;
	p->PRIM_type = T_ORDREL_IND;
	switch (sccp_get_t_state(sc)) {
	case TS_DATA_XFER:
		sccp_set_t_state(sc, TS_WREQ_ORDREL);
		break;
	case TS_WIND_ORDREL:
		sccp_set_t_state(sc, TS_IDLE);
		break;
	}
	putnext(sc->oq, mp);
	return (0);
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: SWERR: unexpected indication for state %ld\n", DRV_NAME, sc,
		sccp_get_t_state(sc)));
	goto error;
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sc));
	goto error;
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: flow controlled\n", DRV_NAME, sc));
	goto error;
      error:
	return (err);
}
#endif

/*
 *  T_ADDR_ACK          27 - address acknowledgement
 *  -----------------------------------------------------------------
 */
STATIC int
t_addr_ack(queue_t *q, struct sc *sc, struct sccp_addr *loc, struct sccp_addr *rem)
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
	putnext(sc->oq, mp);
	return (0);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sc));
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
t_capability_ack(queue_t *q, struct sc *sc, ulong caps)
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
		p->ACCEPTOR_id = (caps & TC1_ACCEPTOR_ID) ? (ulong) sc->oq : 0;
		if (caps & TC1_INFO) {
			p->INFO_ack.PRIM_type = T_INFO_ACK;
			p->INFO_ack.TSDU_size = sc->tsdu;
			p->INFO_ack.ETSDU_size = sc->etsdu;
			p->INFO_ack.CDATA_size = sc->cdata;
			p->INFO_ack.DDATA_size = sc->ddata;
			p->INFO_ack.ADDR_size = sc->addlen;
			p->INFO_ack.OPT_size = sc->optlen;
			p->INFO_ack.TIDU_size = sc->tidu;
			p->INFO_ack.SERV_type = sc->stype;
			p->INFO_ack.CURRENT_state = sccp_get_t_state(sc);
			p->INFO_ack.PROVIDER_flag = sc->ptype;
		} else
			bzero(&p->INFO_ack, sizeof(p->INFO_ack));
		putnext(sc->oq, mp);
		return (0);
	} else {
		err = -ENOBUFS;
		ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sc));
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
t_unitdata_ind(queue_t *q, struct sc *sc, struct sccp_addr *src, ulong *seq, ulong *prior,
	       ulong *pcl, ulong *imp, ulong *rerr, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	struct t_opthdr *oh;
	const size_t olen = sizeof(*oh) + sizeof(t_scalar_t);
	struct T_unitdata_ind *p;
	size_t src_len = src ? sizeof(*src) : 0;
	size_t opt_len =
	    (seq ? olen : 0) + (prior ? olen : 0) + (pcl ? olen : 0) + (imp ? olen : 0) +
	    (rerr ? olen : 0);
	size_t msg_len = sizeof(*p) + src_len + opt_len;
	if (!canputnext(sc->oq))
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
		if (seq) {
			oh = ((typeof(oh)) mp->b_wptr)++;
			oh->len = olen;
			oh->level = T_SS7_SCCP;
			oh->name = T_SCCP_SEQ_CTRL;
			oh->status = T_SUCCESS;
			*((t_uscalar_t *) mp->b_wptr)++ = *seq;
		}
		if (prior) {
			oh = ((typeof(oh)) mp->b_wptr)++;
			oh->len = olen;
			oh->level = T_SS7_SCCP;
			oh->name = T_SCCP_PRIORITY;
			oh->status = T_SUCCESS;
			*((t_uscalar_t *) mp->b_wptr)++ = *prior;
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
		if (rerr) {
			oh = ((typeof(oh)) mp->b_wptr)++;
			oh->len = olen;
			oh->level = T_SS7_SCCP;
			oh->name = T_SCCP_RET_ERROR;
			oh->status = T_SUCCESS;
			*((t_uscalar_t *) mp->b_wptr)++ = *rerr;
		}
	}
	putnext(sc->oq, mp);
	return (0);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sc));
	goto error;
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: Flow controlled\n", DRV_NAME, sc));
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
t_uderror_ind(queue_t *q, struct sc *sc, ulong etype, struct sccp_addr *dst, ulong *seq, ulong *pri,
	      ulong *pcl, ulong *imp, ulong *ret, mblk_t *dp)
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
	if (!canputnext(sc->oq))
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
	putnext(sc->oq, mp);
	return (0);
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sc));
	goto error;
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: Flow controlled\n", DRV_NAME, sc));
	goto error;
      error:
	return (err);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Primitives sent downstream
 *
 *  -------------------------------------------------------------------------
 */
#if 0
/*
 *  MTP_BIND_REQ
 *  -----------------------------------
 */
STATIC INLINE int
mtp_bind_req(queue_t *q, struct mt *mt, ulong flags, struct mtp_addr *add)
{
	mblk_t *mp;
	struct MTP_bind_req *p;
	size_t add_len = add ? sizeof(*add) : 0;
	if ((mp = ss7_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->mtp_primitive = MTP_BIND_REQ;
		p->mtp_addr_length = add_len;
		p->mtp_addr_offset = add_len ? sizeof(*p) : 0;
		p->mtp_bind_flags = flags;
		if (add_len) {
			bcopy(add, mp->b_wptr, add_len);
			mp->b_wptr += add_len;
		}
		printd(("%s: %p: MTP_BIND_REQ ->\n", DRV_NAME, mt));
		ss7_oput(mt->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}
#endif

#if 0
/*
 *  MTP_UNBIND_REQ
 *  -----------------------------------
 */
STATIC INLINE int
mtp_unbind_req(queue_t *q, struct mt *mt)
{
	mblk_t *mp;
	struct MTP_unbind_req *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->mtp_primitive = MTP_UNBIND_REQ;
		printd(("%s: %p: MTP_UNBIND_REQ ->\n", DRV_NAME, mt));
		ss7_oput(mt->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}
#endif

#if 0
/*
 *  MTP_CONN_REQ
 *  -----------------------------------
 */
STATIC INLINE int
mtp_conn_req(queue_t *q, struct mt *mt, ulong flags, struct mtp_addr *add)
{
	mblk_t *mp;
	struct MTP_conn_req *p;
	size_t add_len = add ? sizeof(*add) : 0;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->mtp_primitive = MTP_CONN_REQ;
		p->mtp_addr_length = add_len;
		p->mtp_addr_offset = add_len ? sizeof(*p) : 0;
		p->mtp_conn_flags = flags;
		if (add_len) {
			bcopy(add, mp->b_wptr, add_len);
			mp->b_wptr += add_len;
		}
		printd(("%s: %p: MTP_CONN_REQ ->\n", DRV_NAME, mt));
		ss7_oput(mt->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}
#endif

#if 0
/*
 *  MTP_DISCON_REQ
 *  -----------------------------------
 */
STATIC INLINE int
mtp_discon_req(queue_t *q, struct mt *mt)
{
	if (canput(mt->oq)) {
		mblk_t *mp;
		struct MTP_discon_req *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->mtp_primitive = MTP_DISCON_REQ;
			printd(("%s: %p: MTP_DISCON_REQ ->\n", DRV_NAME, mt));
			ss7_oput(mt->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}
#endif

#if 0
/*
 *  MTP_ADDR_REQ
 *  -----------------------------------
 */
STATIC INLINE int
mtp_addr_req(queue_t *q, struct mt *mt)
{
	mblk_t *mp;
	struct MTP_addr_req *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->mtp_primitive = MTP_ADDR_REQ;
		printd(("%s: %p: MTP_ADDR_REQ ->\n", DRV_NAME, mt));
		ss7_oput(mt->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}
#endif

#if 0
/*
 *  MTP_INFO_REQ
 *  -----------------------------------
 */
STATIC INLINE int
mtp_info_req(queue_t *q, struct mt *mt)
{
	mblk_t *mp;
	struct MTP_info_req *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->mtp_primitive = MTP_INFO_REQ;
		printd(("%s: %p: MTP_INFO_REQ ->\n", DRV_NAME, mt));
		ss7_oput(mt->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}
#endif

#if 0
/*
 *  MTP_OPTMGMT_REQ
 *  -----------------------------------
 */
STATIC INLINE int
mtp_optmgmt_req(queue_t *q, struct mt *mt, ulong flags, uchar *opt_ptr, size_t opt_len)
{
	mblk_t *mp;
	struct MTP_optmgmt_req *p;
	if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->mtp_primitive = MTP_OPTMGMT_REQ;
		p->mtp_opt_length = opt_len;
		p->mtp_opt_offset = opt_len ? sizeof(*p) : 0;
		p->mtp_mgmt_flags = flags;
		if (opt_len) {
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
		}
		printd(("%s: %p: MTP_OPTMGMT_REQ ->\n", DRV_NAME, mt));
		ss7_oput(mt->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}
#endif

/*
 *  MTP_TRANSFER_REQ
 *  -----------------------------------
 */
STATIC INLINE int
mtp_transfer_req(queue_t *q, struct mt *mt, struct mtp_addr *add, ulong prior, ulong sls,
		 mblk_t *dp)
{
	if (canput(mt->oq)) {
		mblk_t *mp;
		struct MTP_transfer_req *p;
		size_t add_len = add ? sizeof(*add) : 0;
		if ((mp = ss7_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->mtp_primitive = MTP_TRANSFER_REQ;
			p->mtp_dest_length = add_len;
			p->mtp_dest_offset = add_len ? sizeof(*p) : 0;
			p->mtp_mp = prior;
			p->mtp_sls = sls;
			if (add_len) {
				bcopy(add, mp->b_wptr, add_len);
				mp->b_wptr += add_len;
			}
			mp->b_cont = dp;
			printd(("%s: %p: MTP_TRANSFER_REQ ->\n", DRV_NAME, mt));
			ss7_oput(mt->oq, mp);
			return (QR_ABSORBED);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Encode/Decode message functions
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  Parameter sizing, packing and unpacking functions:
 *  -------------------------------------------------------------------------
 */
STATIC inline int
size_one(void)
{
	return (1);
}
STATIC inline int
pack_one(ulong val, uchar **p)
{
	*(*p)++ = val;
	return (1);
}

/*
 *  SCCP_PT_EOP   0x00 - End of Optional Parameters
 *  -------------------------------------------------------------------------
 */
STATIC inline int
size_eop(void)
{
	return (0);
}
STATIC inline int
pack_eop(struct sccp_msg *m, uchar **p)
{
	(void) p;
	return (0);
}
STATIC inline int
unpack_eop(uchar *p, uchar *e, struct sccp_msg *m)
{
	if (p + size_eop() <= e) {
		int len = size_eop();
		m->parms |= SCCP_PTF_EOP;
		return (len);
	}
	return (-EMSGSIZE);
}

/*
 *  SCCP_PT_DLR   0x01 - Destination Local Reference
 *  -------------------------------------------------------------------------
 */
STATIC inline int
size_lrn(void)
{
	return (3);
}
STATIC inline int
pack_lrn(uchar **p, ulong lrn)
{
	*(*p)++ = (lrn >> 16) & 0xff;
	*(*p)++ = (lrn >> 8) & 0xff;
	*(*p)++ = (lrn >> 0) & 0xff;
	return (3);
}
STATIC inline int
size_dlr(void)
{
	return size_lrn();
}
STATIC inline int
pack_dlr(struct sccp_msg *m, uchar **p)
{
	return pack_lrn(p, m->dlr);
}
STATIC inline int
unpack_dlr(uchar *p, uchar *e, struct sccp_msg *m)
{
	if (!(m->parms & SCCP_PTF_DLR)) {
		int len = size_dlr();
		if (p + len <= e) {
			m->dlr = 0;
			m->dlr |= ((ulong) (*p++) << 0);
			m->dlr |= ((ulong) (*p++) << 8);
			m->dlr |= ((ulong) (*p++) << 16);
			m->parms |= SCCP_PTF_DLR;
			return (len);
		}
		return (-EMSGSIZE);
	}
	return (-EPROTO);
}

/*
 *  SCCP_PT_SLR   0x02 - Source Local reference
 *  -------------------------------------------------------------------------
 */
STATIC inline int
size_slr(void)
{
	return size_lrn();
}
STATIC inline int
pack_slr(struct sccp_msg *m, uchar **p)
{
	return pack_lrn(p, m->slr);
}
STATIC inline int
unpack_slr(uchar *p, uchar *e, struct sccp_msg *m)
{
	if (!(m->parms & SCCP_PTF_SLR)) {
		int len = size_slr();
		if (p + len <= e) {
			m->slr = 0;
			m->slr |= (((ulong) *p++) << 0);
			m->slr |= (((ulong) *p++) << 8);
			m->slr |= (((ulong) *p++) << 16);
			m->parms |= SCCP_PTF_SLR;
			return (len);
		}
		return (-EMSGSIZE);
	}
	return (-EPROTO);
}

/*
 *  SCCP_PT_CDPA  0x03 - Called Party Address
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_cpa(ulong pvar, struct sccp_addr *cpa)
{
	size_t len = 0;
	len += 1;
	switch (pvar & SS7_PVAR_MASK) {
	case SS7_PVAR_JTTC:
		fixme(("Not sure is this is correct for JTTC\n"));
		goto skip_ni;
	case SS7_PVAR_ANSI:
		if (cpa->ni) {
		      skip_ni:
			if (cpa->pc != -1UL)
				len += 3;
			// if ( cpa->ssn ) /* always code SSN */
			len += 1;
			if (cpa->gtt) {
				switch (cpa->gtt) {
				case 1:
					len += 2;
					break;
				case 2:
					len += 1;
					break;
				}
				len += cpa->alen;
			}
			break;
		}
	default:
		if (cpa->pc != -1UL)
			len += 2;
		// if ( cpa->ssn ) /* always code SSN */
		len += 1;
		if (cpa->gtt) {
			switch (cpa->gtt) {
			case 1:
				len += 1;
				break;
			case 2:
				len += 1;
				break;
			case 3:
				len += 2;
				break;
			case 4:
				len += 3;
				break;
			}
			len += cpa->alen;
		}
		break;
	}
	return (len);
}
STATIC INLINE int
pack_cpa(ulong pvar, uchar **p, struct sccp_addr *a)
{
	ulong len = 0;
	ulong pci = (a->pc == -1UL) ? 0 : 1;
	ulong ssni = (a->ssn == 0) ? 1 : 1;	/* always include ssn */
	*(*p) = 0;
	*(*p) |= (pci & 0x01) << 0;
	*(*p) |= (ssni & 0x01) << 1;
	*(*p) |= (a->gtt & 0x0f) << 2;
	*(*p) |= (a->ri & 0x01) << 6;
	*(*p) |= (a->ni & 0x01) << 7;
	(*p)++;
	len++;
	switch (pvar & SS7_PVAR_MASK) {
	case SS7_PVAR_JTTC:
		fixme(("Not sure is this is correct for JTTC\n"));
		goto skip_ni;
	case SS7_PVAR_ANSI:
		if (a->ni) {
		      skip_ni:
			if (pci) {
				*(*p)++ = (a->pc >> 0) & 0xff;
				len++;
				*(*p)++ = (a->pc >> 8) & 0xff;
				len++;
				*(*p)++ = (a->pc >> 16) & 0xff;
				len++;
			}
			if (ssni) {
				*(*p)++ = a->ssn;
				len++;
			}
			if (a->gtt) {
				switch (a->gtt) {
				case 1:	/* 0001 */
					*(*p)++ = a->tt;
					len++;
					*(*p) = 0;
					*(*p) |= (a->es & 0xf) << 0;
					*(*p) |= (a->nplan & 0xf) << 4;
					(*p)++;
					len++;
					break;
				case 2:	/* 0010 */
					*(*p)++ = a->tt;
					len++;
					break;
				}
				bcopy(a->addr, (*p), a->alen);
				(*p) += a->alen;
				len += a->alen;
			}
		}
	default:
		if (pci) {
			*(*p)++ = (a->pc >> 0) & 0xff;
			len++;
			*(*p)++ = (a->pc >> 8) & 0x3f;
			len++;
		}
		if (ssni) {
			*(*p)++ = a->ssn;
			len++;
		}
		if (a->gtt) {
			switch (a->gtt) {
			case 1:	/* 0001 */
				*(*p)++ = a->nai;
				len++;
				break;
			case 2:	/* 0010 */
				*(*p)++ = a->tt;
				len++;
				break;
			case 3:	/* 0011 */
				*(*p)++ = a->tt;
				len++;
				*(*p) = 0;
				*(*p) |= (a->es & 0xf) << 0;
				*(*p) |= (a->nplan & 0xf) << 4;
				(*p)++;
				len++;
				break;
			case 4:	/* 0100 */
				*(*p)++ = a->tt;
				len++;
				*(*p) = 0;
				*(*p) |= (a->es & 0xf) << 0;
				*(*p) |= (a->nplan & 0xf) << 4;
				(*p)++;
				len++;
				*(*p)++ = a->nai;
				len++;
				break;
			}
			bcopy(a->addr, (*p), a->alen);
			(*p) += a->alen;
			len += a->alen;
		}
	}
	return (len);
}
STATIC INLINE int
unpack_cpa(ulong pvar, uchar *p, uchar *e, struct sccp_addr *a)
{
	ulong oe = 0, pci, ssni, len = 0;
	if (p + 1 > e)
		return (-EMSGSIZE);
	pci = (*p >> 0) & 0x01;
	ssni = (*p >> 1) & 0x01;
	a->gtt = (*p >> 2) & 0x0f;
	a->ri = (*p >> 6) & 0x01;
	a->ni = (*p >> 7) & 0x01;
	p++;
	len++;
	switch (pvar & SS7_PVAR_MASK) {
	case SS7_PVAR_JTTC:
		fixme(("Not sure is this is correct for JTTC\n"));
		goto skip_ni;
	case SS7_PVAR_ANSI:
		if (a->ni) {
		      skip_ni:
			if (pci) {
				if (p + 3 > e)
					return (-EMSGSIZE);
				a->pc = 0;
				a->pc |= (*p++ << 0) & 0xff;
				len++;
				a->pc |= (*p++ << 8) & 0xff;
				len++;
				a->pc |= (*p++ << 16) & 0xff;
				len++;
			} else
				a->pc = -1UL;
			if (ssni) {
				if (p + 1 > e)
					return (-EMSGSIZE);
				a->ssn = *p++;
				len++;
			} else
				a->ssn = 0;
			if (a->gtt) {
				switch (a->gtt) {
				case 1:	/* 0001 */
					if (p + 2 > e)
						return (-EMSGSIZE);
					a->tt = (*p++);
					len++;
					a->es = (*p >> 0) & 0x0f;
					a->nplan = (*p >> 4) & 0x0f;
					p++;
					len++;
					break;
				case 2:	/* 0010 */
					if (p + 1 > e)
						return (-EMSGSIZE);
					a->tt = *p++;
					len++;
					break;
				default:
					return (-EPROTO);
				}
				/* 
				   followed by address bytes */
				if ((a->alen = e - p) > SCCP_MAX_ADDR_LENGTH)
					return (-EMSGSIZE);
				bcopy(p, a->addr, a->alen);
				p += a->alen;
				len += a->alen;
			}
			break;
		}
	default:
		if (pci) {
			if (p + 2 > e)
				return (-EMSGSIZE);
			a->pc = 0;
			a->pc |= (*p++ << 0) & 0xff;
			len++;
			a->pc |= (*p++ << 8) & 0x3f;
			len++;
		} else
			a->pc = -1UL;
		if (ssni) {
			if (p + 1 > e)
				return (-EMSGSIZE);
			a->ssn = *p++;
			len++;
		} else
			a->ssn = 0;
		if (a->gtt) {
			switch (a->gtt) {
			case 1:	/* 0001 */
				if (p + 1 > e)
					return (-EMSGSIZE);
				oe = (*p >> 7) & 0x01;
				a->nai = (*p++) & 0x7f;
				len++;
				break;
			case 2:	/* 0010 */
				if (p + 1 > e)
					return (-EMSGSIZE);
				a->tt = *p++;
				len++;
				break;
			case 3:	/* 0011 */
				if (p + 2 > e)
					return (-EMSGSIZE);
				a->tt = (*p++);
				len++;
				a->es = (*p >> 0) & 0x0f;
				a->nplan = (*p >> 4) & 0x0f;
				p++;
				len++;
				break;
			case 4:	/* 0100 */
				if (p + 3 > e)
					return (-EMSGSIZE);
				a->tt = (*p++);
				len++;
				a->es = (*p >> 0) & 0x0f;
				a->nplan = (*p >> 4) & 0x0f;
				p++;
				len++;
				a->nai = (*p++) & 0x7f;
				len++;
				break;
			default:
				return (-EPROTO);
			}
			/* 
			   followed by address bytes */
			if ((a->alen = e - p) > SCCP_MAX_ADDR_LENGTH)
				return (-EMSGSIZE);
			bcopy(p, a->addr, a->alen);
			p += a->alen;
			len += a->alen;
		}
		break;
	}
	return (len);
}
STATIC inline int
size_cdpa(struct sccp_msg *m)
{
	return size_cpa(m->pvar, &m->cdpa);
}
STATIC inline int
pack_cdpa(struct sccp_msg *m, uchar **p)
{
	return pack_cpa(m->pvar, p, &m->cdpa);
}
STATIC inline int
unpack_cdpa(uchar *p, uchar *e, struct sccp_msg *m)
{
	if (!(m->parms & SCCP_PTF_CDPA)) {
		int rtn;
		if ((rtn = unpack_cpa(m->pvar, p, e, &m->cdpa)) < 0)
			return (rtn);
		m->parms |= SCCP_PTF_CDPA;
		return (rtn);
	}
	return (-EPROTO);
}

/*
 *  SCCP_PT_CGPA  0x04 - Calling Party Address
 *  -------------------------------------------------------------------------
 */
STATIC inline int
size_cgpa(struct sccp_msg *m)
{
	return size_cpa(m->pvar, &m->cgpa);
}
STATIC inline int
pack_cgpa(struct sccp_msg *m, uchar **p)
{
	return pack_cpa(m->pvar, p, &m->cgpa);
}
STATIC inline int
unpack_cgpa(uchar *p, uchar *e, struct sccp_msg *m)
{
	if (!(m->parms & SCCP_PTF_CGPA)) {
		int rtn;
		if ((rtn = unpack_cpa(m->pvar, p, e, &m->cgpa)) < 0)
			return (rtn);
		m->parms |= SCCP_PTF_CGPA;
		return (rtn);
	}
	return (-EPROTO);
}

/*
 *  SCCP_PT_PCLS  0x05 - Protocol Class
 *  -------------------------------------------------------------------------
 */
STATIC inline int
size_pcls(void)
{
	return (1);
}
STATIC inline int
pack_pcls(struct sccp_msg *m, uchar **p)
{
	return pack_one(m->pcl | ((m->pcl < 2) ? m->ret : 0), p);
}
STATIC inline int
unpack_pcls(uchar *p, uchar *e, struct sccp_msg *m)
{
	if (!(m->parms & SCCP_PTF_PCLS)) {
		int len = size_pcls();
		if (p + len <= e) {
			m->pcl = *p & 0x0f;
			m->ret = *p & 0x80;
			m->parms |= SCCP_PTF_PCLS;
			return (len);
		}
		return (-EMSGSIZE);
	}
	return (-EPROTO);
}

/*
 *  SCCP_PT_SEG   0x06 - Segmenting/Reassembly
 *  -------------------------------------------------------------------------
 */
STATIC inline int
size_seg(void)
{
	return (1);
}
STATIC inline int
pack_seg(struct sccp_msg *m, uchar **p)
{
	return pack_one(m->more ? 1 : 0, p);
}
STATIC inline int
unpack_seg(uchar *p, uchar *e, struct sccp_msg *m)
{
	if (!(m->parms & SCCP_PTF_SEG)) {
		int len = size_seg();
		if (p + len >= e) {
			m->seg = *p & 0x1;
			m->parms |= SCCP_PTF_SEG;
			return (len);
		}
		return (-EMSGSIZE);
	}
	return (-EPROTO);
}

/*
 *  SCCP_PT_RSN   0x07 - Receive Sequence Number
 *  -------------------------------------------------------------------------
 */
STATIC inline int
size_rsn(void)
{
	return (1);
}
STATIC inline int
pack_rsn(struct sccp_msg *m, uchar **p)
{
	return pack_one((m->rsn << 1) & 0xfe, p);
}
STATIC inline int
unpack_rsn(uchar *p, uchar *e, struct sccp_msg *m)
{
	if (!(m->parms & SCCP_PTF_RSN)) {
		int len = size_rsn();
		if (p + len <= e) {
			m->pr = (*p & 0xfe) >> 1;
			m->parms |= SCCP_PTF_RSN;
			return (len);
		}
		return (-EMSGSIZE);
	}
	return (-EPROTO);
}

/*
 *  SCCP_PT_SEQ   0x08 - Sequencing/Segmenting
 *  -------------------------------------------------------------------------
 */
STATIC inline int
size_seq(void)
{
	return (2);
}
STATIC inline int
pack_seq(struct sccp_msg *m, uchar **p)
{
	*(*p)++ = ((m->ps << 1) & 0xfe);
	*(*p)++ = ((m->pr << 1) & 0xfe) | (m->more ? 1 : 0);
	return (2);
}
STATIC inline int
unpack_seq(uchar *p, uchar *e, struct sccp_msg *m)
{
	if (!(m->parms & SCCP_PTF_SEQ)) {
		int len = size_seq();
		if (p + len <= e) {
			m->ps = (*p++ & 0xfe) >> 1;
			m->more = *p & 0x1;
			m->pr = (*p++ & 0xfe) >> 1;
			m->parms |= SCCP_PTF_SEQ;
			return (len);
		}
		return (-EMSGSIZE);
	}
	return (-EPROTO);
}

/*
 *  SCCP_PT_CRED  0x09 - Credit
 *  -------------------------------------------------------------------------
 */
STATIC inline int
size_cred(void)
{
	return (1);
}
STATIC inline int
pack_cred(struct sccp_msg *m, uchar **p)
{
	return pack_one(m->cred, p);
}
STATIC inline int
unpack_cred(uchar *p, uchar *e, struct sccp_msg *m)
{
	if (!(m->parms & SCCP_PTF_CRED)) {
		int len = size_cred();
		if (p + len <= e) {
			m->cred = *p;
			m->parms |= SCCP_PTF_CRED;
			return (len);
		}
		return (-EMSGSIZE);
	}
	return (-EPROTO);
}

/*
 *  SCCP_PT_RELC  0x0a - Release Cause
 *  -------------------------------------------------------------------------
 */
STATIC inline int
size_relc(void)
{
	return size_one();
}
STATIC inline int
pack_relc(struct sccp_msg *m, uchar **p)
{
	return pack_one(m->cause, p);
}
STATIC inline int
unpack_relc(uchar *p, uchar *e, struct sccp_msg *m)
{
	if (!
	    (m->
	     parms & (SCCP_PTF_RELC | SCCP_PTF_RETC | SCCP_PTF_RESC | SCCP_PTF_ERRC |
		      SCCP_PTF_REFC))) {
		int len = size_relc();
		if (p + len <= e) {
			m->cause = *p;
			m->parms |= SCCP_PTF_RELC;
			return (len);
		}
		return (-EMSGSIZE);
	}
	return (-EPROTO);
}

/*
 *  SCCP_PT_RETC  0x0b - Return Cause
 *  -------------------------------------------------------------------------
 */
STATIC inline int
size_retc(void)
{
	return size_one();
}
STATIC inline int
pack_retc(struct sccp_msg *m, uchar **p)
{
	return pack_one(m->cause, p);
}
STATIC inline int
unpack_retc(uchar *p, uchar *e, struct sccp_msg *m)
{
	if (!
	    (m->
	     parms & (SCCP_PTF_RELC | SCCP_PTF_RETC | SCCP_PTF_RESC | SCCP_PTF_ERRC |
		      SCCP_PTF_REFC))) {
		int len = size_retc();
		if (p + len <= e) {
			m->cause = *p;
			m->parms |= SCCP_PTF_RETC;
			return (len);
		}
		return (-EMSGSIZE);
	}
	return (-EPROTO);
}

/*
 *  SCCP_PT_RESC  0x0c - Reset Cause
 *  -------------------------------------------------------------------------
 */
STATIC inline int
size_resc(void)
{
	return size_one();
}
STATIC inline int
pack_resc(struct sccp_msg *m, uchar **p)
{
	return pack_one(m->cause, p);
}
STATIC inline int
unpack_resc(uchar *p, uchar *e, struct sccp_msg *m)
{
	if (!
	    (m->
	     parms & (SCCP_PTF_RELC | SCCP_PTF_RETC | SCCP_PTF_RESC | SCCP_PTF_ERRC |
		      SCCP_PTF_REFC))) {
		int len = size_resc();
		if (p + len <= e) {
			m->cause = *p;
			m->parms |= SCCP_PTF_RESC;
			return (len);
		}
		return (-EMSGSIZE);
	}
	return (-EPROTO);
}

/*
 *  SCCP_PT_ERRC  0x0d - Error Cause
 *  -------------------------------------------------------------------------
 */
STATIC inline int
size_errc(void)
{
	return size_one();
}
STATIC inline int
pack_errc(struct sccp_msg *m, uchar **p)
{
	return pack_one(m->cause, p);
}
STATIC inline int
unpack_errc(uchar *p, uchar *e, struct sccp_msg *m)
{
	if (!
	    (m->
	     parms & (SCCP_PTF_RELC | SCCP_PTF_RETC | SCCP_PTF_RESC | SCCP_PTF_ERRC |
		      SCCP_PTF_REFC))) {
		int len = size_errc();
		if (p + len >= e) {
			m->cause = *p;
			m->parms |= SCCP_PTF_ERRC;
			return (len);
		}
		return (-EMSGSIZE);
	}
	return (-EPROTO);
}

/*
 *  SCCP_PT_REFC  0x0e - Refusal Cause
 *  -------------------------------------------------------------------------
 */
STATIC inline int
size_refc(void)
{
	return size_one();
}
STATIC inline int
pack_refc(struct sccp_msg *m, uchar **p)
{
	return pack_one(m->cause, p);
}
STATIC inline int
unpack_refc(uchar *p, uchar *e, struct sccp_msg *m)
{
	if (!
	    (m->
	     parms & (SCCP_PTF_RELC | SCCP_PTF_RETC | SCCP_PTF_RESC | SCCP_PTF_ERRC |
		      SCCP_PTF_REFC))) {
		int len = size_refc();
		if (p + len <= e) {
			m->cause = *p;
			m->parms |= SCCP_PTF_REFC;
			return (len);
		}
		return (-EMSGSIZE);
	}
	return (-EPROTO);
}

/*
 *  SCCP_PT_DATA  0x0f - Data
 *  -------------------------------------------------------------------------
 */
STATIC inline int
size_data(struct sccp_msg *m)
{
	return (m->data.len);
}
STATIC inline void
link_data(mblk_t *mp, mblk_t *data)
{
	/* 
	   IMPLEMENTATION NOTE:- Because sizeable mandatory variable part data exists, MVP DATA and 
	   MVP LDATA are linked at the end of the SCCP message rather than copying it into the SCCP 
	   M_DATA message block. */
	linkb(mp, data);
}
STATIC inline int
pack_data(struct sccp_msg *m, uchar **p)
{
	/* 
	   IMPLEMENTATION NOTE:- For optional data (in connection and disconnect) the farthest
	   towards the end of the message that DATA can be placed is just before the EOP byte.
	   Because optional data is not normally large, we byte-copy the data. */
	int len = 0;
	mblk_t *dp = m->bp->b_cont;
	while ((dp = dp->b_cont)) {
		size_t slen = (dp->b_wptr > dp->b_rptr) ? dp->b_wptr - dp->b_rptr : 0;
		bcopy(dp->b_rptr, *p, slen);
		*p += slen;
		len += slen;
	}
	return (len);
}
STATIC inline int
unpack_data(uchar *p, uchar *e, struct sccp_msg *m)
{
	if (!(m->parms & (SCCP_PTF_DATA | SCCP_PTF_LDATA))) {
		int len = e - p;
		if (len >= 3) {
			m->data.ptr = p;
			m->data.len = len;
			m->parms |= SCCP_PTF_DATA;
			return (len);
		}
		return (-EMSGSIZE);
	}
	return (-EPROTO);
}

/*
 *  SCCP_PT_SGMT  0x10 - Segmentation
 *  -------------------------------------------------------------------------
 */
STATIC inline int
size_sgmt(void)
{
	return (4);
}
STATIC inline int
pack_sgmt(struct sccp_msg *m, uchar **p)
{
	*(*p)++ = (m->sgmt.first << 7) | ((m->sgmt.pcl & 0x1) << 6) | (m->sgmt.rem & 0xf);
	*(*p)++ = (m->sgmt.slr >> 16) & 0xff;
	*(*p)++ = (m->sgmt.slr >> 8) & 0xff;
	*(*p)++ = (m->sgmt.slr >> 0) & 0xff;
	return (4);
}
STATIC inline int
unpack_sgmt(uchar *p, uchar *e, struct sccp_msg *m)
{
	if (!(m->parms & SCCP_PTF_SGMT)) {
		int len = size_sgmt();
		if (p + len <= e) {
			m->sgmt.first = (*p & 0x80) >> 7;
			m->sgmt.pcl = (*p & 0x40) >> 6;
			m->sgmt.rem = (*p & 0x0f) >> 0;
			p++;
			m->sgmt.slr = 0;
			m->sgmt.slr |= (*p++) << 0;
			m->sgmt.slr |= (*p++) << 8;
			m->sgmt.slr |= (*p++) << 16;
			m->parms |= SCCP_PTF_SGMT;
			return (len);
		}
		return (-EMSGSIZE);
	}
	return (-EPROTO);
}

/*
 *  SCCP_PT_HOPC  0x11 - Hop Counter
 *  -------------------------------------------------------------------------
 */
STATIC inline int
size_hopc(void)
{
	return size_one();
}
STATIC inline int
pack_hopc(struct sccp_msg *m, uchar **p)
{
	return pack_one(m->hopc, p);
}
STATIC inline int
unpack_hopc(uchar *p, uchar *e, struct sccp_msg *m)
{
	if (!(m->parms & SCCP_PTF_HOPC)) {
		int len = size_hopc();
		if (p + len <= e) {
			m->hopc = *p;
			m->parms |= SCCP_PTF_HOPC;
			return (len);
		}
		return (-EMSGSIZE);
	}
	return (-EPROTO);
}

/*
 *  SCCP_PT_IMP   0x12 - Importance
 *  -------------------------------------------------------------------------
 */
STATIC inline int
size_imp(void)
{
	return size_one();
}
STATIC inline int
pack_imp(struct sccp_msg *m, uchar **p)
{
	return pack_one((m->imp & 0x7), p);
}
STATIC inline int
unpack_imp(uchar *p, uchar *e, struct sccp_msg *m)
{
	if (!(m->parms & SCCP_PTF_IMP)) {
		int len = size_imp();
		if (p + len <= e) {
			m->imp = (*p & 0x7);
			m->parms |= SCCP_PTF_IMP;
			return (len);
		}
		return (-EMSGSIZE);
	}
	return (-EPROTO);
}

/*
 *  SCCP_PT_LDATA 0x13 - Long Data
 *  -------------------------------------------------------------------------
 */
STATIC inline int
size_ldata(struct sccp_msg *m)
{
	return size_data(m);
}
STATIC inline void
link_ldata(mblk_t *mp, mblk_t *data)
{
	return link_data(mp, data);
}
STATIC inline int
pack_ldata(struct sccp_msg *m, uchar **p)
{
	return pack_data(m, p);
}
STATIC inline int
unpack_ldata(uchar *p, uchar *e, struct sccp_msg *m)
{
	if (!(m->parms & (SCCP_PTF_DATA | SCCP_PTF_LDATA))) {
		int len = e - p;
		if (len >= 3) {
			m->data.ptr = p;
			m->data.len = len;
			m->parms |= SCCP_PTF_LDATA;
			return (len);
		}
		return (-EMSGSIZE);
	}
	return (-EPROTO);
}

/*
 *  SCCP_PT_ISNI  0xfa - Intermediate Signalling Network Identification
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_isni(struct sccp_msg *m)
{
	size_t num, len = 0;
	len++;
	if (m->isni.ti)
		len++;
	if ((num = m->isni.nids) > 7)
		num = 7;
	len += num << 1;
	return (len);
}
STATIC INLINE int
pack_isni(struct sccp_msg *m, uchar **p)
{
	int i, len = 0;
	*(*p) = 0;
	*(*p) |= (m->isni.mi & 0x01) << 0;
	*(*p) |= (m->isni.iri & 0x03) << 1;
	*(*p) |= (m->isni.ti & 0x01) << 4;
	*(*p) |= (m->isni.count & 0x07) << 5;
	(*p)++;
	len++;
	if (m->isni.ti & 0x1) {
		*(*p)++ = m->isni.ns & 0x3;
		len++;
	}
	for (i = 0; i < 7 && i < m->isni.nids; i++) {
		*(*p)++ = m->isni.u[i].nid.network;
		len++;
		*(*p)++ = m->isni.u[i].nid.cluster;
		len++;
	}
	return (len);
}
STATIC INLINE int
unpack_isni(uchar *p, uchar *e, struct sccp_msg *m)
{
	if (!(m->parms & SCCP_PTF_ISNI)) {
		ulong i, len = 0;
		if (p + 2 > e)
			return (-EMSGSIZE);
		if (p + 1 > e)
			return (-EMSGSIZE);
		else
			len++;
		m->isni.mi = (*p >> 0) & 0x01;
		m->isni.iri = (*p >> 1) & 0x03;
		m->isni.ti = (*p >> 4) & 0x01;
		m->isni.count = (*p >> 5) & 0x07;
		p++;
		if (m->isni.ti) {
			if (p + 1 > e)
				return (-EMSGSIZE);
			else
				len++;
			m->isni.ns = (*p++) & 0x3;
		}
		if (p + 2 > e)
			return (-EMSGSIZE);
		m->isni.nids = (e - p) >> 1;
		if (m->isni.nids > 7)
			m->isni.nids = 7;
		for (i = 0; i < m->isni.nids; i++) {
			m->isni.u[i].nid.network = *p++;
			len++;
			m->isni.u[i].nid.cluster = *p++;
			len++;
		}
		m->parms |= SCCP_PTF_ISNI;
		return (len);
	}
	return (-EPROTO);
}

/*
 *  SCCP_PT_INS   0xf9 - Intermediate Network Selection
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_ins(struct sccp_msg *m)
{
	size_t num, len = 0;
	len++;
	if ((num = m->ins.nids) > 2)
		num = 2;
	len += num << 1;
	return (len);
}
STATIC INLINE int
pack_ins(struct sccp_msg *m, uchar **p)
{
	ulong i, len = 0;
	*(*p) = 0;
	*(*p) |= (m->ins.itype & 0x03) << 0;
	*(*p) |= (m->ins.rtype & 0x03) << 2;
	*(*p) |= (m->ins.count & 0x03) << 6;
	(*p)++;
	len++;
	for (i = 0; i < 2 && i < m->ins.nids; i++) {
		*(*p)++ = m->ins.u[i].nid.network;
		len++;
		*(*p)++ = m->ins.u[i].nid.cluster;
		len++;
	}
	return (len);
}
STATIC INLINE int
unpack_ins(uchar *p, uchar *e, struct sccp_msg *m)
{
	if (!(m->parms & SCCP_PTF_INS)) {
		ulong i, len = 0;
		if (p + 2 > e)
			return (-EMSGSIZE);
		if (p + 1 > e)
			return (-EMSGSIZE);
		else
			len++;
		m->ins.itype = (*p >> 0) & 0x03;
		m->ins.rtype = (*p >> 2) & 0x03;
		m->ins.count = (*p >> 6) & 0x03;
		p++;
		if (p + 1 > e)
			return (-EMSGSIZE);
		else
			len++;
		m->ins.nids = (e - p) >> 1;
		if (m->ins.nids > 2)
			m->ins.nids = 2;
		for (i = 0; i < m->ins.nids; i++) {
			m->ins.u[i].nid.network = *p++;
			len++;
			m->ins.u[i].nid.cluster = *p++;
			len++;
		}
		m->parms |= SCCP_PTF_INS;
		return (len);
	}
	return (-EPROTO);
}

/*
 *  SCCP_PT_MTI   0xf8 - Message Type Interworking
 *  -------------------------------------------------------------------------
 */
STATIC inline int
size_mti(void)
{
	return (1);
}
STATIC inline int
pack_mti(struct sccp_msg *m, uchar **p)
{
	return pack_one(m->mti, p);
}
STATIC inline int
unpack_mti(uchar *p, uchar *e, struct sccp_msg *m)
{
	if (!(m->parms & SCCP_PTF_MTI)) {
		int len = size_mti();
		if (p + len <= e) {
			m->mti = *p;
			m->parms |= SCCP_PTF_MTI;
			return (len);
		}
		return (-EMSGSIZE);
	}
	return (-EPROTO);
}

/*
 *  SCMG FI
 *  -------------------------------------------------------------------------
 */
STATIC inline int
size_scmgfi(void)
{
	return size_one();
}
STATIC inline int
pack_scmgfi(struct sccp_msg *m, uchar **p)
{
	return pack_one(m->fi, p);
}
STATIC inline int
unpack_scmgfi(uchar *p, uchar *e, struct sccp_msg *m)
{
	if (p + 1 <= e) {
		m->fi = *p;
		return (1);
	}
	return (-EMSGSIZE);
}

/*
 *  SCMG ASSN
 *  -------------------------------------------------------------------------
 */
STATIC inline int
size_assn(void)
{
	return size_one();
}
STATIC inline int
pack_assn(struct sccp_msg *m, uchar **p)
{
	return pack_one(m->assn, p);
}
STATIC inline int
unpack_assn(uchar *p, uchar *e, struct sccp_msg *m)
{
	if (p + 1 <= e) {
		m->assn = *p;
		return (1);
	}
	return (-EMSGSIZE);
}

/*
 *  SCMG APC
 *  -------------------------------------------------------------------------
 */
STATIC inline int
size_apc(void)
{
	return (3);
}
STATIC inline int
pack_apc(struct sccp_msg *m, uchar **p)
{
	*(*p)++ = (m->apc >> 0);
	*(*p)++ = (m->apc >> 8);
	*(*p)++ = (m->apc >> 16);
	return (3);
}
STATIC inline int
unpack_apc(uchar *p, uchar *e, struct sccp_msg *m)
{
	if (p + 1 <= e) {
		m->apc = 0;
		m->apc |= (((ulong) *p++) << 0);
		m->apc |= (((ulong) *p++) << 8);
		m->apc |= (((ulong) *p++) << 16);
		return (3);
	}
	return (-EMSGSIZE);
}

/*
 *  SCMG SMI
 *  -------------------------------------------------------------------------
 */
STATIC inline int
size_smi(void)
{
	return size_one();
}
STATIC inline int
pack_smi(struct sccp_msg *m, uchar **p)
{
	return pack_one(m->smi, p);
}
STATIC inline int
unpack_smi(uchar *p, uchar *e, struct sccp_msg *m)
{
	if (p + 1 <= e) {
		m->smi = *p;
		return (1);
	}
	return (-EMSGSIZE);
}

/*
 *  SCMG CONG
 *  -------------------------------------------------------------------------
 */
STATIC inline int
size_cong(void)
{
	return size_one();
}
STATIC inline int
pack_cong(struct sccp_msg *m, uchar **p)
{
	return pack_one(m->cong, p);
}
STATIC inline int
unpack_cong(uchar *p, uchar *e, struct sccp_msg *m)
{
	if (p + 1 <= e) {
		m->cong = *p;
		return (1);
	}
	return (-EMSGSIZE);
}

/*
 *  Optional Parameters
 *  -------------------------------------------------------------------------
 */
STATIC inline int
size_opt(struct sccp_msg *m, size_t len)
{
	return (len);
}
STATIC inline int
pack_opt(uchar **p, uchar *ptr, size_t len)
{
	bcopy(ptr, *p, len);
	*p += len;
	return (len);
}
STATIC inline int
unpack_opt(uchar *p, uchar *e, struct sccp_var *v, size_t len)
{
	if (p + len - 1 < e) {
		v->len = len - 1;
		v->ptr = p;
		return (len);
	}
	trace();
	return (-EMSGSIZE);
}

#if 0
STATIC inline int
sccp_check_opt(uchar *p, size_t len)
{
	uchar ptype = 0;
	uchar *pp = p, *ep, *e = p + len;
	if (!len)
		return (0);
	for (pp = p, ep = pp + pp[1] + 2; pp < e && (ptype = *pp); pp = ep, ep = pp + pp[1] + 2) ;
	return ((((pp != (e - 1) || ptype)) && pp != e) ? -EINVAL : 0);
}
#endif
STATIC int
sccp_dec_opt(uchar *p, uchar *e, struct sccp_msg *m)
{
	int err = 0;
	int8_t ptype;
	uchar *pp = p, *ep = e;
	if (e - p < 3) {
		trace();
		return (-EMSGSIZE);
	}
	for (pp = p, ep = pp + pp[1] + 2; pp < e && (ptype = *pp); pp = ep, ep = pp + pp[1] + 2) {
		switch (ptype) {
		case SCCP_PT_MTI:
			err = unpack_mti(pp + 2, ep, m);
			break;
		case SCCP_PT_INS:
			err = unpack_ins(pp + 2, ep, m);
			break;
		case SCCP_PT_ISNI:
			err = unpack_isni(pp + 2, ep, m);
			break;
		case SCCP_PT_EOP:
			err = unpack_eop(pp + 2, ep, m);
			break;
		case SCCP_PT_DLR:
			err = unpack_dlr(pp + 2, ep, m);
			break;
		case SCCP_PT_SLR:
			err = unpack_slr(pp + 2, ep, m);
			break;
		case SCCP_PT_CDPA:
			err = unpack_cdpa(pp + 2, ep, m);
			break;
		case SCCP_PT_CGPA:
			err = unpack_cgpa(pp + 2, ep, m);
			break;
		case SCCP_PT_PCLS:
			err = unpack_pcls(pp + 2, ep, m);
			break;
		case SCCP_PT_SEG:
			err = unpack_seg(pp + 2, ep, m);
			break;
		case SCCP_PT_RSN:
			err = unpack_rsn(pp + 2, ep, m);
			break;
		case SCCP_PT_SEQ:
			err = unpack_seq(pp + 2, ep, m);
			break;
		case SCCP_PT_CRED:
			err = unpack_cred(pp + 2, ep, m);
			break;
		case SCCP_PT_RELC:
			err = unpack_relc(pp + 2, ep, m);
			break;
		case SCCP_PT_RETC:
			err = unpack_retc(pp + 2, ep, m);
			break;
		case SCCP_PT_RESC:
			err = unpack_resc(pp + 2, ep, m);
			break;
		case SCCP_PT_ERRC:
			err = unpack_errc(pp + 2, ep, m);
			break;
		case SCCP_PT_REFC:
			err = unpack_refc(pp + 2, ep, m);
			break;
		case SCCP_PT_DATA:
			err = unpack_data(pp + 2, ep, m);
			break;
		case SCCP_PT_SGMT:
			err = unpack_sgmt(pp + 2, ep, m);
			break;
		case SCCP_PT_HOPC:
			err = unpack_hopc(pp + 2, ep, m);
			break;
		case SCCP_PT_IMP:
			err = unpack_imp(pp + 2, ep, m);
			break;
#if 0
		case SCCP_PT_LDATA:	/* ldata cannot be optional */
			err = unpack_ldata(pp + 2, ep, m);
			break;
#endif
		default:
			err = 0;	/* ignore unregocnized optional parameters */
		}
		if (err < 0)
			return (err);
	}
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Message Packing Functions
 *
 *  -------------------------------------------------------------------------
 *  These functions are used to take an unpacked message from the queue, pack
 *  it into a message for transmission and then place it back on the queue.
 */

/*
 *  SCCP_MT_CR    0x01 - Connection Request
 *  -------------------------------------------------------------------------
 *  ITUT: F(SLR, PCLS), V(CDPA), O(CRED, CGPA, DATA, HOPC, IMP, EOP)
 *  ASNI: F(SLR, PCLS), V(CDPA), O(CRED, CGPA, DATA, HOPC, EOP)
 */
STATIC mblk_t *
sccp_pack_cr(queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	struct sccp_msg *m = (typeof(m)) msg->b_rptr;
	size_t mlen = 1 +		/* MT */
	    size_slr() +		/* SLR */
	    size_pcls() +		/* PCLS */
	    2 + size_cdpa(m) +		/* CDPA */
	    1 +				/* oparms */
	    ((m->pcl == 3 && (m->parms & SCCP_PTF_CRED)) ? 2 + size_cred() : 0) +	/* CRED */
	    ((m->parms & SCCP_PTF_CGPA) ? 2 + size_cgpa(m) : 0) +	/* CGPA */
	    ((m->parms & SCCP_PTF_DATA) ? 2 + size_data(m) : 0) +	/* DATA */
	    ((m->parms & SCCP_PTF_HOPC) ? 2 + size_hopc() : 0) +	/* HOPC */
	    ((m->parms & SCCP_PTF_IMP) ? 2 + size_imp() : 0) +	/* IMP */
	    1;
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p;
		mp->b_datap->db_type = M_DATA;
		*(mp->b_wptr)++ = SCCP_MT_CR;
		pack_slr(m, &mp->b_wptr);	/* SLR F */
		pack_pcls(m, &mp->b_wptr);	/* PCLS F */
		p = mp->b_wptr;
		mp->b_wptr += 2;
		*p = mp->b_wptr - p;
		p++;
		*(mp->b_wptr)++ = size_cdpa(m);	/* CDPA V */
		pack_cdpa(m, &mp->b_wptr);
		*p = mp->b_wptr - p;
		p++;
		if (m->pcl == 3 && (m->parms & SCCP_PTF_CRED)) {
			*(mp->b_wptr)++ = SCCP_PT_CRED;	/* CRED O */
			*(mp->b_wptr)++ = size_cred();
			pack_cred(m, &mp->b_wptr);
		}
		if ((m->parms & SCCP_PTF_CGPA)) {
			*(mp->b_wptr)++ = SCCP_PT_CGPA;	/* CGPA O */
			*(mp->b_wptr)++ = size_cgpa(m);
			pack_cgpa(m, &mp->b_wptr);
		}
		if ((m->parms & SCCP_PTF_DATA)) {
			*(mp->b_wptr)++ = SCCP_PT_DATA;	/* DATA O */
			*(mp->b_wptr)++ = size_data(m);
			pack_data(m, &mp->b_wptr);
		}
		if ((m->parms & SCCP_PTF_HOPC)) {
			*(mp->b_wptr)++ = SCCP_PT_HOPC;	/* HOPC O */
			*(mp->b_wptr)++ = size_hopc();
			pack_hopc(m, &mp->b_wptr);
		}
		if ((m->parms & SCCP_PTF_IMP)) {
			*(mp->b_wptr)++ = SCCP_PT_IMP;	/* IMP O */
			*(mp->b_wptr)++ = size_imp();
			pack_imp(m, &mp->b_wptr);
		}
		*(mp->b_wptr)++ = SCCP_PT_EOP;	/* EOP O */
	}
	return (mp);
}

/*
 *  SCCP_MT_CC    0x02 - Connection Confirm
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR, SLR, PCLS), O(CRED, CDPA, DATA, IMP, EOP)
 *  ANSI: F(DLR, SLR, PCLS), O(CRED, CDPA, DATA, EOP)
 */
STATIC mblk_t *
sccp_pack_cc(queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	struct sccp_msg *m = (typeof(m)) msg->b_rptr;
	size_t mlen = 1 +		/* MT */
	    size_dlr() +		/* DLR */
	    size_slr() +		/* SLR */
	    size_pcls() +		/* PCLS */
	    1 +				/* oparms */
	    ((m->pcl == 3 && (m->parms & SCCP_PTF_CRED)) ? 2 + size_cred() : 0) +	/* PCLS */
	    ((m->parms & SCCP_PTF_CDPA) ? 2 + size_cdpa(m) : 0) +	/* CDPA */
	    ((m->parms & SCCP_PTF_DATA) ? 2 + size_data(m) : 0) +	/* DATA */
	    ((m->parms & SCCP_PTF_IMP) ? 2 + size_imp() : 0) +	/* IMP */
	    1;				/* EOP */
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p;
		mp->b_datap->db_type = M_DATA;
		*(mp->b_wptr)++ = SCCP_MT_CC;
		pack_dlr(m, &mp->b_wptr);	/* DLR F */
		pack_slr(m, &mp->b_wptr);	/* SLR F */
		pack_pcls(m, &mp->b_wptr);	/* PCLS F */
		p = mp->b_wptr;
		mp->b_wptr += 1;
		*p = mp->b_wptr - p;
		p++;
		if (m->pcl == 3 && (m->parms & SCCP_PTF_CRED)) {
			*(mp->b_wptr)++ = SCCP_PT_CRED;	/* CRED O */
			*(mp->b_wptr)++ = size_cred();
			pack_cred(m, &mp->b_wptr);
		}
		if ((m->parms & SCCP_PTF_CDPA)) {
			*(mp->b_wptr)++ = SCCP_PT_CDPA;	/* CDPA O */
			*(mp->b_wptr)++ = size_cdpa(m);
			pack_cdpa(m, &mp->b_wptr);
		}
		if ((m->parms & SCCP_PTF_DATA)) {
			*(mp->b_wptr)++ = SCCP_PT_DATA;	/* DATA O */
			*(mp->b_wptr)++ = size_data(m);
			pack_data(m, &mp->b_wptr);
		}
		if ((m->parms & SCCP_PTF_IMP)) {
			*(mp->b_wptr)++ = SCCP_PT_IMP;	/* IMP O */
			*(mp->b_wptr)++ = size_imp();
			pack_imp(m, &mp->b_wptr);
		}
		*(mp->b_wptr)++ = SCCP_PT_EOP;	/* EOP O */
		m->rl.mp = SCCP_MP_CC;
	}
	return (mp);
}

/*
 *  SCCP_MT_CREF  0x03 - Connection Refused
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR, REFC), O(CDPA, DATA, IMP, EOP)
 *  ANSI: F(DLR, REFC), O(CDPA, DATA, EOP)
 */
STATIC mblk_t *
sccp_pack_cref(queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	struct sccp_msg *m = (typeof(m)) msg->b_rptr;
	size_t mlen = 1 +		/* MT */
	    size_dlr() +		/* DLR */
	    size_refc() +		/* REFC */
	    1 +				/* oparms */
	    ((m->parms & SCCP_PTF_CDPA) ? 2 + size_cdpa(m) : 0) +	/* CDPA */
	    ((m->parms & SCCP_PTF_DATA) ? 2 + size_data(m) : 0) +	/* DATA */
	    ((m->parms & SCCP_PTF_IMP) ? 2 + size_imp() : 0) +	/* IMP */
	    1;				/* EOP */
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p;
		mp->b_datap->db_type = M_DATA;
		*(mp->b_wptr)++ = SCCP_MT_CREF;
		pack_dlr(m, &mp->b_wptr);	/* DLR F */
		pack_refc(m, &mp->b_wptr);	/* REFC F */
		p = mp->b_wptr;
		mp->b_wptr += 1;
		*p = mp->b_wptr - p;
		p++;
		if ((m->parms & SCCP_PTF_CDPA)) {
			*(mp->b_wptr)++ = SCCP_PT_CDPA;	/* CDPA O */
			*(mp->b_wptr)++ = size_cdpa(m);
			pack_cdpa(m, &mp->b_wptr);
		}
		if ((m->parms & SCCP_PTF_DATA)) {
			*(mp->b_wptr)++ = SCCP_PT_DATA;	/* DATA O */
			*(mp->b_wptr)++ = size_data(m);
			pack_data(m, &mp->b_wptr);
		}
		if ((m->parms & SCCP_PTF_IMP)) {
			*(mp->b_wptr)++ = SCCP_PT_IMP;	/* IMP O */
			*(mp->b_wptr)++ = size_imp();
			pack_imp(m, &mp->b_wptr);
		}
		*(mp->b_wptr)++ = SCCP_PT_EOP;	/* EOP O */
		m->rl.mp = SCCP_MP_CREF;
	}
	return (mp);
}

/*
 *  SCCP_MT_RLSD  0x04 - Released
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR, SLR, RELC), O(DATA, IMP, EOP)
 *  ANSI: F(DLR, SLR, RELC), O(DATA, EOP)
 */
STATIC mblk_t *
sccp_pack_rlsd(queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	struct sccp_msg *m = (typeof(m)) msg->b_rptr;
	size_t mlen = 1 +		/* MT */
	    size_dlr() +		/* DLR */
	    size_slr() +		/* SLR */
	    size_relc() +		/* RELC */
	    1 +				/* oparms */
	    ((m->parms & SCCP_PTF_DATA) ? 2 + size_data(m) : 0) +	/* DATA */
	    ((m->parms & SCCP_PTF_IMP) ? 2 + size_imp() : 0) +	/* IMP */
	    1;				/* EOP */
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p;
		mp->b_datap->db_type = M_DATA;
		*(mp->b_wptr)++ = SCCP_MT_RLSD;
		pack_dlr(m, &mp->b_wptr);	/* DLR F */
		pack_slr(m, &mp->b_wptr);	/* SLR F */
		pack_relc(m, &mp->b_wptr);	/* RELC F */
		p = mp->b_wptr;
		mp->b_wptr += 1;
		*p = mp->b_wptr - p;
		p++;
		*(mp->b_wptr)++ = SCCP_PT_DATA;	/* DATA O */
		*(mp->b_wptr)++ = size_data(m);
		pack_data(m, &mp->b_wptr);
		*(mp->b_wptr)++ = SCCP_PT_IMP;	/* IMP O */
		*(mp->b_wptr)++ = size_imp();
		pack_imp(m, &mp->b_wptr);
		*(mp->b_wptr)++ = SCCP_PT_EOP;	/* EOP O */
		m->rl.mp = SCCP_MP_RLSD;
	}
	return (mp);
}

/*
 *  SCCP_MT_RLC   0x05 - Release Complete
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR, SLR)
 *  ANSI: F(DLR, SLR)
 */
STATIC mblk_t *
sccp_pack_rlc(queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	struct sccp_msg *m = (typeof(m)) msg->b_rptr;
	size_t mlen = 1 +		/* MT */
	    size_dlr() +		/* DLR */
	    size_slr();			/* SLR */
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		*(mp->b_wptr)++ = SCCP_MT_RLC;
		pack_dlr(m, &mp->b_wptr);	/* DLR F */
		pack_slr(m, &mp->b_wptr);	/* SLR F */
		m->rl.mp = SCCP_MP_RLC;
	}
	return (mp);
}

/*
 *  SCCP_MT_DT1   0x06 - Data Form 1
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR, SEG), V(DATA)
 *  ANSI: F(DLR, SEG), V(DATA)
 */
STATIC mblk_t *
sccp_pack_dt1(queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	struct sccp_msg *m = (typeof(m)) msg->b_rptr;
	size_t mlen = 1 +		/* MT */
	    size_dlr() +		/* DLR */
	    size_seg() +		/* SEG */
	    2 + size_data(m) +		/* DATA */
	    1;				/* EOP */
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p;
		mp->b_datap->db_type = M_DATA;
		*(mp->b_wptr)++ = SCCP_MT_DT1;
		pack_dlr(m, &mp->b_wptr);	/* DLR F */
		pack_seg(m, &mp->b_wptr);	/* SEG F */
		p = mp->b_wptr;
		mp->b_wptr += 1;
		*p = mp->b_wptr - p;
		p++;
		*(mp->b_wptr)++ = size_data(m);	/* DATA V */
		link_data(mp, msg->b_cont);
	}
	return (mp);
}

/*
 *  SCCP_MT_DT2   0x07 - Data Form 2
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR, SEQ), V(DATA)
 *  ANSI: F(DLR, SEQ), V(DATA)
 */
STATIC mblk_t *
sccp_pack_dt2(queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	struct sccp_msg *m = (typeof(m)) msg->b_rptr;
	size_t mlen = 1 +		/* MT */
	    size_dlr() +		/* DLR */
	    size_seq() +		/* SEQ */
	    2 + size_data(m) +		/* DATA */
	    1;				/* EOP */
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p;
		mp->b_datap->db_type = M_DATA;
		*(mp->b_wptr)++ = SCCP_MT_DT2;
		pack_dlr(m, &mp->b_wptr);	/* DLR F */
		pack_seq(m, &mp->b_wptr);	/* SEQ F */
		p = mp->b_wptr;
		mp->b_wptr += 1;
		*p = mp->b_wptr - p;
		p++;
		*(mp->b_wptr)++ = size_data(m);	/* DATA V */
		link_data(mp, msg->b_cont);
	}
	return (mp);
}

/*
 *  SCCP_MT_AK    0x08 - Data Acknowledgement
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR, RSN, CRED)
 *  ANSI: F(DLR, RSN, CRED)
 */
STATIC mblk_t *
sccp_pack_ak(queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	struct sccp_msg *m = (typeof(m)) msg->b_rptr;
	size_t mlen = 1 +		/* MT */
	    size_dlr() +		/* DLR */
	    size_rsn() +		/* RSN */
	    size_cred() +		/* CRED */
	    1;				/* EOP */
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		*(mp->b_wptr)++ = SCCP_MT_AK;
		pack_dlr(m, &mp->b_wptr);	/* DLR F */
		pack_rsn(m, &mp->b_wptr);	/* RSN F */
		pack_cred(m, &mp->b_wptr);	/* CRED F */
	}
	return (mp);
}

/*
 *  SCCP_MT_UDT   0x09 - Unitdata
 *  -------------------------------------------------------------------------
 *  ITUT: F(PCLS), V(CDPA, CGPA, DATA[2-254])
 *  ANSI: F(PCLS), V(CDPA, CGPA, DATA[2-252])
 */
STATIC mblk_t *
sccp_pack_udt(queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	struct sccp_msg *m = (typeof(m)) msg->b_rptr;
	size_t mlen = 1 +		/* MT */
	    size_pcls() +		/* PCLS */
	    2 + size_cdpa(m) +		/* CDPA */
	    2 + size_cgpa(m) +		/* CGPA */
	    2 + size_data(m) +		/* DATA */
	    1;				/* EOP */
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p;
		mp->b_datap->db_type = M_DATA;
		*(mp->b_wptr)++ = SCCP_MT_UDT;
		pack_pcls(m, &mp->b_wptr);	/* PCLS F */
		p = mp->b_wptr;
		mp->b_wptr += 3;
		*p = mp->b_wptr - p;
		p++;
		*(mp->b_wptr)++ = size_cdpa(m);	/* CDPA V */
		pack_cdpa(m, &mp->b_wptr);
		*p = mp->b_wptr - p;
		p++;
		*(mp->b_wptr)++ = size_cgpa(m);	/* CGPA V */
		pack_cgpa(m, &mp->b_wptr);
		*p = mp->b_wptr - p;
		p++;
		*(mp->b_wptr)++ = size_data(m);	/* DATA V */
		link_data(mp, msg->b_cont);
	}
	return (mp);
}

/*
 *  SCCP_MT_UDTS  0x0a - Unitdata Service
 *  -------------------------------------------------------------------------
 *  ITUT: F(RETC), V(CDPA, CGPA, DATA[2-253])
 *  ANSI: F(RETC), V(CDPA, CGPA, DATA[2-251])
 */
STATIC mblk_t *
sccp_pack_udts(queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	struct sccp_msg *m = (typeof(m)) msg->b_rptr;
	size_t mlen = 1 +		/* MT */
	    size_retc() +		/* RETC */
	    2 + size_cdpa(m) +		/* CDPA */
	    2 + size_cgpa(m) +		/* CGPA */
	    2 + size_data(m) +		/* DATA */
	    1;				/* EOP */
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p;
		mp->b_datap->db_type = M_DATA;
		*(mp->b_wptr)++ = SCCP_MT_UDTS;
		pack_retc(m, &mp->b_wptr);	/* RETC F */
		p = mp->b_wptr;
		mp->b_wptr += 3;
		*p = mp->b_wptr - p;
		p++;
		*(mp->b_wptr)++ = size_cdpa(m);	/* CDPA V */
		pack_cdpa(m, &mp->b_wptr);
		*p = mp->b_wptr - p;
		p++;
		*(mp->b_wptr)++ = size_cgpa(m);	/* CGPA V */
		pack_cgpa(m, &mp->b_wptr);
		*p = mp->b_wptr - p;
		p++;
		*(mp->b_wptr)++ = size_data(m);	/* DATA V */
		link_data(mp, msg->b_cont);
	}
	return (mp);
}

/*
 *  SCCP_MT_ED    0x0b - Expedited Data
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR), V(DATA)
 *  ANSI: F(DLR), V(DATA)
 */
STATIC mblk_t *
sccp_pack_ed(queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	struct sccp_msg *m = (typeof(m)) msg->b_rptr;
	size_t mlen = 1 +		/* MT */
	    size_dlr() +		/* DLR */
	    2 + size_data(m) +		/* DATA */
	    1;				/* EOP */
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p;
		mp->b_datap->db_type = M_DATA;
		*(mp->b_wptr)++ = SCCP_MT_ED;
		pack_dlr(m, &mp->b_wptr);	/* DLR F */
		p = mp->b_wptr;
		mp->b_wptr += 1;
		*p = mp->b_wptr - p;
		p++;
		*(mp->b_wptr)++ = size_data(m);	/* DATA V */
		link_data(mp, msg->b_cont);
		m->rl.mp = SCCP_MP_ED;
	}
	return (mp);
}

/*
 *  SCCP_MT_EA    0x0c - Expedited Data Acknowledgement
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR)
 *  ANSI: F(DLR)
 */
STATIC mblk_t *
sccp_pack_ea(queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	struct sccp_msg *m = (typeof(m)) msg->b_rptr;
	size_t mlen = 1 +		/* MT */
	    size_dlr() +		/* DLR */
	    1;				/* EOP */
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		*(mp->b_wptr)++ = SCCP_MT_EA;
		pack_dlr(m, &mp->b_wptr);	/* DLR F */
		m->rl.mp = SCCP_MP_EA;
	}
	return (mp);
}

/*
 *  SCCP_MT_RSR   0x0d - Reset Request
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR, SLR, RESC)
 *  ANSI: F(DLR, SLR, RESC)
 */
STATIC mblk_t *
sccp_pack_rsr(queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	struct sccp_msg *m = (typeof(m)) msg->b_rptr;
	size_t mlen = 1 +		/* MT */
	    size_dlr() +		/* DLR */
	    size_slr() +		/* SLR */
	    size_resc() +		/* RESC */
	    1;				/* EOP */
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		*(mp->b_wptr)++ = SCCP_MT_RSR;
		pack_dlr(m, &mp->b_wptr);	/* DLR F */
		pack_slr(m, &mp->b_wptr);	/* SLR F */
		pack_resc(m, &mp->b_wptr);	/* RESC F */
		m->rl.mp = SCCP_MP_RSR;
	}
	return (mp);
}

/*
 *  SCCP_MT_RSC   0x0e - Reset Confirm
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR, SLR)
 *  ANSI: F(DLR, SLR)
 */
STATIC mblk_t *
sccp_pack_rsc(queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	struct sccp_msg *m = (typeof(m)) msg->b_rptr;
	size_t mlen = 1 +		/* MT */
	    size_dlr() +		/* DLR */
	    size_slr() +		/* SLR */
	    1;				/* EOP */
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		*(mp->b_wptr)++ = SCCP_MT_RSC;
		pack_dlr(m, &mp->b_wptr);	/* DLR F */
		pack_slr(m, &mp->b_wptr);	/* SLR F */
		m->rl.mp = SCCP_MP_RSC;
	}
	return (mp);
}

/*
 *  SCCP_MT_ERR   0x0f - Protocol Data Unit Error
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR, ERRC)
 *  ANSI: F(DLR, ERRC)
 */
STATIC mblk_t *
sccp_pack_err(queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	struct sccp_msg *m = (typeof(m)) msg->b_rptr;
	size_t mlen = 1 +		/* MT */
	    size_dlr() +		/* DLR */
	    size_errc() +		/* ERRC */
	    1;				/* EOP */
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		*(mp->b_wptr)++ = SCCP_MT_ERR;
		pack_dlr(m, &mp->b_wptr);	/* DLR F */
		pack_errc(m, &mp->b_wptr);	/* ERRC F */
		m->rl.mp = SCCP_MP_ERR;
	}
	return (mp);
}

/*
 *  SCCP_MT_IT    0x10 - Inactivity Test
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR, SLR, PCLS, SEQ, CRED)
 *  ANSI: F(DLR, SLR, PCLS, SEQ, CRED)
 */
STATIC mblk_t *
sccp_pack_it(queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	struct sccp_msg *m = (typeof(m)) msg->b_rptr;
	size_t mlen = 1 +		/* MT */
	    size_dlr() +		/* DLR */
	    size_slr() +		/* SLR */
	    size_pcls() +		/* PCLS */
	    size_seq() +		/* SEQ */
	    size_cred() +		/* CRED */
	    1;				/* EOP */
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		*(mp->b_wptr)++ = SCCP_MT_IT;
		pack_dlr(m, &mp->b_wptr);	/* DLR F */
		pack_slr(m, &mp->b_wptr);	/* SLR F */
		pack_pcls(m, &mp->b_wptr);	/* PCLS F */
		pack_seq(m, &mp->b_wptr);	/* SEQ F */
		pack_cred(m, &mp->b_wptr);	/* CRED F */
		m->rl.mp = SCCP_MP_IT;
	}
	return (mp);
}

/*
 *  SCCP_MT_XUDT  0x11 - Extended Unitdata
 *  -------------------------------------------------------------------------
 *  ITUT: F(PCLS, HOPC), V(CDPA, CGPA, DATA[2-253]), O(SGMT, IMP, EOP)
 *  ANSI: F(PCLS, HOPC), V(CDPA, CGPA, DATA[2-251]), O(SGMT, ISNI, INS, MTI, EOP)
 */
STATIC mblk_t *
sccp_pack_xudt(queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	struct sccp_msg *m = (typeof(m)) msg->b_rptr;
	size_t mlen = 1 +		/* MT */
	    size_pcls() +		/* PCLS */
	    size_hopc() +		/* HOPC */
	    2 + size_cdpa(m) +		/* CDPA */
	    2 + size_cgpa(m) +		/* CGPA */
	    2 + size_data(m) +		/* DATA */
	    1 +				/* optr */
	    ((m->parms & SCCP_PTF_SGMT) ? 2 + size_sgmt() : 0) +	/* SGMT */
	    ((m->parms & SCCP_PTF_IMP) ? 2 + size_imp() : 0) +	/* IMP */
	    ((m->parms & SCCP_PTF_ISNI) ? 2 + size_isni(m) : 0) +	/* ISNI */
	    ((m->parms & SCCP_PTF_INS) ? 2 + size_ins(m) : 0) +	/* INS */
	    ((m->parms & SCCP_PTF_MTI) ? 2 + size_mti() : 0) +	/* MTI */
	    1;				/* EOP */
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p, *pd;
		mp->b_datap->db_type = M_DATA;
		*(mp->b_wptr)++ = SCCP_MT_XUDT;
		pack_pcls(m, &mp->b_wptr);	/* PCLS F */
		pack_hopc(m, &mp->b_wptr);	/* HOPC F */
		p = mp->b_wptr;
		mp->b_wptr += 4;
		*p = mp->b_wptr - p;	/* CDPA V */
		p++;
		*(mp->b_wptr)++ = size_cdpa(m);
		pack_cdpa(m, &mp->b_wptr);
		*p = mp->b_wptr - p;	/* CGPA V */
		p++;
		*(mp->b_wptr)++ = size_cgpa(m);
		pack_cgpa(m, &mp->b_wptr);
		pd = p++;	/* DATA V */
		*p = mp->b_wptr - p;	/* Opt Params */
		p++;
		if ((m->parms & SCCP_PTF_SGMT)) {
			*(mp->b_wptr)++ = SCCP_PT_SGMT;	/* SGMT O */
			*(mp->b_wptr)++ = size_sgmt();
			pack_sgmt(m, &mp->b_wptr);
		}
		if ((m->parms & SCCP_PTF_IMP)) {
			*(mp->b_wptr)++ = SCCP_PT_IMP;	/* IMP O */
			*(mp->b_wptr)++ = size_imp();
			pack_imp(m, &mp->b_wptr);
		}
		if ((m->parms & SCCP_PTF_ISNI)) {
			*(mp->b_wptr)++ = SCCP_PT_ISNI;	/* ISNI O */
			*(mp->b_wptr)++ = size_isni(m);
			pack_isni(m, &mp->b_wptr);
		}
		if ((m->parms & SCCP_PTF_INS)) {
			*(mp->b_wptr)++ = SCCP_PT_INS;	/* INS O */
			*(mp->b_wptr)++ = size_ins(m);
			pack_ins(m, &mp->b_wptr);
		}
		if ((m->parms & SCCP_PTF_MTI)) {
			*(mp->b_wptr)++ = SCCP_PT_MTI;	/* MTI O */
			*(mp->b_wptr)++ = size_mti();
			pack_mti(m, &mp->b_wptr);
		}
		*(mp->b_wptr)++ = SCCP_PT_EOP;	/* EOP O */
		*pd = mp->b_wptr - pd;	/* DATA V */
		*(mp->b_wptr)++ = size_data(m);
		link_data(mp, msg->b_cont);
	}
	return (mp);
}

/*
 *  SCCP_MT_XUDTS 0x12 - Extended Unitdata Service
 *  -------------------------------------------------------------------------
 *  ITUT: F(RETC, HOPC), V(CDPA, CGPA, DATA[2-253]), O(SGMT, IMP, EOP)
 *  ANSI: F(RETC, HOPC), V(CDPA, CGPA, DATA[2-251]), O(SGMT, INS, MTI, ISNI, EOP)
 */
STATIC mblk_t *
sccp_pack_xudts(queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	struct sccp_msg *m = (typeof(m)) msg->b_rptr;
	size_t mlen = 1 +		/* MT */
	    size_retc() +		/* RETC */
	    size_hopc() +		/* HOPC */
	    2 + size_cdpa(m) +		/* CDPA */
	    2 + size_cgpa(m) +		/* CGPA */
	    2 + size_data(m) +		/* DATA */
	    1 +				/* oparms */
	    ((m->parms & SCCP_PTF_SGMT) ? 2 + size_sgmt() : 0) +	/* SGMT */
	    ((m->parms & SCCP_PTF_IMP) ? 2 + size_imp() : 0) +	/* IMP */
	    ((m->parms & SCCP_PTF_INS) ? 2 + size_ins(m) : 0) +	/* INS */
	    ((m->parms & SCCP_PTF_MTI) ? 2 + size_mti() : 0) +	/* MTI */
	    ((m->parms & SCCP_PTF_ISNI) ? 2 + size_isni(m) : 0) +	/* ISNI */
	    1;				/* EOP */
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p, *pd;
		mp->b_datap->db_type = M_DATA;
		*(mp->b_wptr)++ = SCCP_MT_XUDTS;
		pack_retc(m, &mp->b_wptr);	/* RETC F */
		pack_hopc(m, &mp->b_wptr);	/* HOPC F */
		p = mp->b_wptr;
		mp->b_wptr += 4;
		*p = mp->b_wptr - p;	/* CDPA V */
		p++;
		*(mp->b_wptr)++ = size_cdpa(m);
		pack_cdpa(m, &mp->b_wptr);
		*p = mp->b_wptr - p;	/* CGPA V */
		p++;
		*(mp->b_wptr)++ = size_cgpa(m);
		pack_cgpa(m, &mp->b_wptr);
		pd = p++;	/* DATA V */
		*p = mp->b_wptr - p;	/* Opt Parms */
		p++;
		if (m->parms & SCCP_PTF_SGMT) {
			*(mp->b_wptr)++ = SCCP_PT_SGMT;	/* SGMT O */
			*(mp->b_wptr)++ = size_sgmt();
			pack_sgmt(m, &mp->b_wptr);
		}
		if (m->parms & SCCP_PTF_IMP) {
			*(mp->b_wptr)++ = SCCP_PT_IMP;	/* IMP O */
			*(mp->b_wptr)++ = size_imp();
			pack_imp(m, &mp->b_wptr);
		}
		if (m->parms & SCCP_PTF_INS) {
			*(mp->b_wptr)++ = SCCP_PT_INS;	/* INS O */
			*(mp->b_wptr)++ = size_ins(m);
			pack_ins(m, &mp->b_wptr);
		}
		if (m->parms & SCCP_PTF_MTI) {
			*(mp->b_wptr)++ = SCCP_PT_MTI;	/* MTI O */
			*(mp->b_wptr)++ = size_mti();
			pack_mti(m, &mp->b_wptr);
		}
		if (m->parms & SCCP_PTF_ISNI) {
			*(mp->b_wptr)++ = SCCP_PT_ISNI;	/* ISNI O */
			*(mp->b_wptr)++ = size_isni(m);
			pack_isni(m, &mp->b_wptr);
		}
		*(mp->b_wptr)++ = SCCP_PT_EOP;	/* EOP O */
		*pd = mp->b_wptr - pd;	/* DATA V */
		*(mp->b_wptr)++ = size_data(m);
		link_data(mp, msg->b_cont);
	}
	return (mp);
}

/*
 *  SCCP_MT_LUDT  0x13 - Long Unitdata
 *  -------------------------------------------------------------------------
 *  ITUT: F(PCLS, HOPC), V(CDPA, CGPA, LDATA[3-3906]), O(SGMT, IMP, EOP)
 *  ANSI: F(PCLS, HOPC), V(CDPA, CGPA, LDATA[3-3904]), O(SGMT, ISNI, INS, EOP)
 */
STATIC mblk_t *
sccp_pack_ludt(queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	struct sccp_msg *m = (typeof(m)) msg->b_rptr;
	size_t mlen = 1 +		/* MT */
	    size_pcls() +		/* PCLS */
	    size_hopc() +		/* HOPC */
	    2 + size_cdpa(m) +		/* CDPA */
	    2 + size_cgpa(m) +		/* CGPA */
	    2 + size_data(m) +		/* DATA */
	    1 +				/* oparms */
	    ((m->parms & SCCP_PTF_SGMT) ? 2 + size_sgmt() : 0) +	/* SGMT */
	    ((m->parms & SCCP_PTF_IMP) ? 2 + size_imp() : 0) +	/* IMP */
	    ((m->parms & SCCP_PTF_ISNI) ? 2 + size_isni(m) : 0) +	/* ISNI */
	    ((m->parms & SCCP_PTF_INS) ? 2 + size_ins(m) : 0) +	/* INS */
	    1;				/* EOP */
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p, *pd;
		mp->b_datap->db_type = M_DATA;
		*(mp->b_wptr)++ = SCCP_MT_LUDT;
		pack_pcls(m, &mp->b_wptr);	/* PCLS F */
		pack_hopc(m, &mp->b_wptr);	/* HOPC F */
		p = mp->b_wptr;
		mp->b_wptr += 4;
		*p = mp->b_wptr - p;
		p++;
		*(mp->b_wptr)++ = size_cdpa(m);	/* CDPA V */
		pack_cdpa(m, &mp->b_wptr);
		*p = mp->b_wptr - p;
		p++;
		*(mp->b_wptr)++ = size_cgpa(m);	/* CGPA V */
		pack_cgpa(m, &mp->b_wptr);
		pd = p++;	/* LDATA V */
		*p = mp->b_wptr - p;	/* Opt Parms */
		p++;
		if (m->parms & SCCP_PTF_SGMT) {
			*(mp->b_wptr)++ = SCCP_PT_SGMT;	/* SGMT O */
			*(mp->b_wptr)++ = size_sgmt();
			pack_sgmt(m, &mp->b_wptr);
		}
		if (m->parms & SCCP_PTF_IMP) {
			*(mp->b_wptr)++ = SCCP_PT_IMP;	/* IMP O */
			*(mp->b_wptr)++ = size_imp();
			pack_imp(m, &mp->b_wptr);
		}
		if (m->parms & SCCP_PTF_ISNI) {
			*(mp->b_wptr)++ = SCCP_PT_ISNI;	/* ISNI O */
			*(mp->b_wptr)++ = size_isni(m);
			pack_isni(m, &mp->b_wptr);
		}
		if (m->parms & SCCP_PTF_INS) {
			*(mp->b_wptr)++ = SCCP_PT_INS;	/* INS O */
			*(mp->b_wptr)++ = size_ins(m);
			pack_ins(m, &mp->b_wptr);
		}
		*(mp->b_wptr)++ = SCCP_PT_EOP;	/* EOP O */
		*pd = mp->b_wptr - pd;	/* LDATA V */
		*(mp->b_wptr)++ = size_ldata(m);
		link_data(mp, msg->b_cont);
	}
	return (mp);
}

/*
 *  SCCP_MT_LUDTS 0x14 - Long Unitdata Service
 *  -------------------------------------------------------------------------
 *  ITUT: F(RETC, HOPC), V(CDPA, CGPA, LDATA[3-3906]), O(SGMT, IMP, EOP)
 *  ANSI: F(RETC, HOPC), V(CDPA, CGPA, LDATA[3-3904]), O(SGMT, ISNI, INS, EOP)
 */
STATIC mblk_t *
sccp_pack_ludts(queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	struct sccp_msg *m = (typeof(m)) msg->b_rptr;
	size_t mlen = 1 +		/* MT */
	    size_retc() +		/* RETC */
	    size_hopc() +		/* HOPC */
	    2 + size_cdpa(m) +		/* CDPA */
	    2 + size_cgpa(m) +		/* CGPA */
	    2 + size_data(m) +		/* DATA */
	    1 +				/* oparms */
	    ((m->parms & SCCP_PTF_SGMT) ? 2 + size_sgmt() : 0) +	/* SGMT */
	    ((m->parms & SCCP_PTF_IMP) ? 2 + size_imp() : 0) +	/* IMP */
	    ((m->parms & SCCP_PTF_ISNI) ? 2 + size_isni(m) : 0) +	/* ISNI */
	    ((m->parms & SCCP_PTF_INS) ? 2 + size_ins(m) : 0) +	/* INS */
	    1;				/* EOP */
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p, *pd;
		mp->b_datap->db_type = M_DATA;
		*(mp->b_wptr)++ = SCCP_MT_LUDTS;
		pack_retc(m, &mp->b_wptr);	/* RETC F */
		pack_hopc(m, &mp->b_wptr);	/* HOPC F */
		p = mp->b_wptr;
		mp->b_wptr += 4;
		*p = mp->b_wptr - p;
		p++;
		*(mp->b_wptr)++ = size_cdpa(m);	/* CDPA V */
		pack_cdpa(m, &mp->b_wptr);
		*p = mp->b_wptr - p;
		p++;
		*(mp->b_wptr)++ = size_cgpa(m);	/* CGPA V */
		pack_cgpa(m, &mp->b_wptr);
		*p = mp->b_wptr - p;
		p++;
		pd = p++;	/* LDATA V */
		*p = mp->b_wptr - p;	/* Opt Parms */
		p++;
		if (m->parms & SCCP_PTF_SGMT) {
			*(mp->b_wptr)++ = SCCP_PT_SGMT;	/* SGMT O */
			*(mp->b_wptr)++ = size_sgmt();
			pack_sgmt(m, &mp->b_wptr);
		}
		if (m->parms & SCCP_PTF_IMP) {
			*(mp->b_wptr)++ = SCCP_PT_IMP;	/* IMP O */
			*(mp->b_wptr)++ = size_imp();
			pack_imp(m, &mp->b_wptr);
		}
		if (m->parms & SCCP_PTF_ISNI) {
			*(mp->b_wptr)++ = SCCP_PT_ISNI;	/* ISNI O */
			*(mp->b_wptr)++ = size_isni(m);
			pack_isni(m, &mp->b_wptr);
		}
		if (m->parms & SCCP_PTF_INS) {
			*(mp->b_wptr)++ = SCCP_PT_INS;	/* INS O */
			*(mp->b_wptr)++ = size_ins(m);
			pack_ins(m, &mp->b_wptr);
		}
		*(mp->b_wptr)++ = SCCP_PT_EOP;	/* EOP O */
		/* 
		   always link data at end of message */
		*pd = mp->b_wptr - pd;	/* LDATA V */
		*(mp->b_wptr)++ = size_ldata(m);
		link_data(mp, msg->b_cont);
	}
	return (mp);
}

STATIC mblk_t *
sccp_pack_scmg(queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	struct sccp_msg *m = (typeof(m)) msg->b_rptr;
	size_t dlen =
	    size_scmgfi() + size_assn() + size_apc() + size_smi() +
	    ((m->fi == SCMG_MT_SSC) ? size_cong() : 0);
	size_t mlen = 1 +		/* MT */
	    size_pcls() +		/* PCLS */
	    2 + size_cdpa(m) +		/* CDPA */
	    2 + size_cgpa(m) +		/* CGPA */
	    2 + dlen +			/* DATA */
	    1;				/* EOP */
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p;
		mp->b_datap->db_type = M_DATA;
		*(mp->b_wptr)++ = SCCP_MT_UDT;
		pack_pcls(m, &mp->b_wptr);	/* PCLS F */
		p = mp->b_wptr;
		mp->b_wptr += 3;
		*p = mp->b_wptr - p;
		p++;
		*(mp->b_wptr)++ = size_cdpa(m);	/* CDPA V */
		pack_cdpa(m, &mp->b_wptr);
		*p = mp->b_wptr - p;
		p++;
		*(mp->b_wptr)++ = size_cgpa(m);	/* CGPA V */
		pack_cgpa(m, &mp->b_wptr);
		*p = mp->b_wptr - p;
		p++;
		*(mp->b_wptr)++ = size_data(m);	/* DATA V */
		pack_scmgfi(m, &mp->b_wptr);
		pack_assn(m, &mp->b_wptr);
		pack_apc(m, &mp->b_wptr);
		pack_smi(m, &mp->b_wptr);
		if (m->fi == SCMG_MT_SSC) {
			pack_cong(m, &mp->b_wptr);
		}
	}
	return (mp);
}

STATIC int
sccp_send_msg(queue_t *q, mblk_t *msg)
{
	int err;
	mblk_t *mp, *dp;
	struct mt *mt = MTP_PRIV(q);
	struct sccp_msg *m = (typeof(m)) msg->b_rptr;
	struct mtp_addr *a;
	struct MTP_transfer_req *p;
	if (!canput(mt->oq))
		goto ebusy;
	if (!(mp = ss7_allocb(q, sizeof(*p) + sizeof(*a), BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->mtp_primitive = MTP_TRANSFER_REQ;
	p->mtp_dest_length = sizeof(*a);
	p->mtp_dest_offset = sizeof(*p);
	p->mtp_mp = m->rl.mp;
	p->mtp_sls = m->rl.sls;
	a = (typeof(a)) mp->b_wptr;
	mp->b_wptr += sizeof(*a);
	*a = mt->loc;
	a->pc = m->rl.dpc;
	switch (m->type) {
	case SCCP_MT_CR:	/* Connection Request */
		dp = sccp_pack_cr(q, msg);
		break;
	case SCCP_MT_CC:	/* Connection Confirm */
		dp = sccp_pack_cc(q, msg);
		break;
	case SCCP_MT_CREF:	/* Connection Refused */
		dp = sccp_pack_cref(q, msg);
		break;
	case SCCP_MT_RLSD:	/* Released */
		dp = sccp_pack_rlsd(q, msg);
		break;
	case SCCP_MT_RLC:	/* Release Complete */
		dp = sccp_pack_rlc(q, msg);
		break;
	case SCCP_MT_DT1:	/* Data Form 1 */
		dp = sccp_pack_dt1(q, msg);
		break;
	case SCCP_MT_DT2:	/* Data Form 2 */
		dp = sccp_pack_dt2(q, msg);
		break;
	case SCCP_MT_AK:	/* Data Acknowledgement */
		dp = sccp_pack_ak(q, msg);
		break;
	case SCCP_MT_UDT:	/* Unitdata */
		switch (m->fi) {
		case 0:
			dp = sccp_pack_udt(q, msg);
			break;
		case SCMG_MT_SSA:	/* Subsystem allowed */
		case SCMG_MT_SSP:	/* Subsystem prohibited */
		case SCMG_MT_SST:	/* Subsystem status test */
		case SCMG_MT_SOR:	/* Subsystem out of service request */
		case SCMG_MT_SOG:	/* Subsystem out of service grant */
		case SCMG_MT_SSC:	/* Subsystem congestion */
		case SCMG_MT_SBR:	/* Subsystem backup routing */
		case SCMG_MT_SNR:	/* Subsystem normal routing */
		case SCMG_MT_SRT:	/* Subsystem routing status test */
			dp = sccp_pack_scmg(q, msg);
			break;
		default:
			err = -EFAULT;
			swerr();
			goto error_free;
		}
		break;
	case SCCP_MT_UDTS:	/* Unitdata Service */
		dp = sccp_pack_udts(q, msg);
		break;
	case SCCP_MT_ED:	/* Expedited Data */
		dp = sccp_pack_ed(q, msg);
		break;
	case SCCP_MT_EA:	/* Expedited Data Acknowledgement */
		dp = sccp_pack_ea(q, msg);
		break;
	case SCCP_MT_RSR:	/* Reset Request */
		dp = sccp_pack_rsr(q, msg);
		break;
	case SCCP_MT_RSC:	/* Reset Confirm */
		dp = sccp_pack_rsc(q, msg);
		break;
	case SCCP_MT_ERR:	/* Protocol Data Unit Error */
		dp = sccp_pack_err(q, msg);
		break;
	case SCCP_MT_IT:	/* Inactivity Test */
		dp = sccp_pack_it(q, msg);
		break;
	case SCCP_MT_XUDT:	/* Extended Unitdata */
		dp = sccp_pack_xudt(q, msg);
		break;
	case SCCP_MT_XUDTS:	/* Extended Unitdata Service */
		dp = sccp_pack_xudts(q, msg);
		break;
	case SCCP_MT_LUDT:	/* Long Unitdata */
		dp = sccp_pack_ludt(q, msg);
		break;
	case SCCP_MT_LUDTS:	/* Long Unitdata Service */
		dp = sccp_pack_ludts(q, msg);
		break;
	default:
		err = -EFAULT;
		swerr();
		goto error_free;
	}
	if (dp) {
		mp->b_cont = dp;
		printd(("%s: %p: MTP_TRANSFER_REQ ->\n", DRV_NAME, mt));
		ss7_oput(mt->oq, mp);
		return (QR_DONE);
	}
	err = -ENOBUFS;
	rare();
      error_free:
	freemsg(mp);
      error:
	return (err);
      enobufs:
	err = -ENOBUFS;
	rare();
	goto error;
      ebusy:
	err = -EBUSY;
	rare();
	goto error;
}

/*
 *  Encode, decode, send message functions
 *  -------------------------------------------------------------------------
 */
#if 0
STATIC int
sccp_send(queue_t *q, struct sr *sr, ulong pri, ulong sls, mblk_t *dp)
{
	int err;
	struct mt *mt;
	if (!sr || (!(mt = sr->mt) && !(mt = sr->sp.sp->mt)))
		goto efault;
	if ((err = mtp_transfer_req(q, mt, &sr->add, pri, sls, dp)) < 0)
		goto error;
	return (QR_DONE);
      efault:
	err = -EFAULT;
	swerr();
	goto error;
      error:
	freemsg(dp);
	return (err);
}
#endif

STATIC struct sccp_msg *
sccp_enc_msg(queue_t *q, struct sp *sp, ulong dpc, ulong pri, ulong sls)
{
	mblk_t *mp;
	struct sccp_msg *m = NULL;
	if ((mp = ss7_allocb(q, sizeof(*m), BPRI_MED))) {
		mp->b_datap->db_type = M_RSE;
		mp->b_band = 3;
		m = (typeof(m)) mp->b_wptr;
		mp->b_wptr += sizeof(*m);
		bzero(m, sizeof(*m));
		m->bp = mp;
		m->eq = q;
		m->xq = q;
		m->sc = NULL;
		m->mt = NULL;
		m->timestamp = jiffies;
		m->rl.opc = sp->add.pc;
		m->rl.dpc = dpc;
		m->rl.sls = sls;
		m->rl.mp = pri;
	}
	return (m);
}

STATIC int sccp_orte_msg(queue_t *q, mblk_t *mp);

/*
 *  SCCP_MT_CR    0x01 - Connection Request
 *  -------------------------------------------------------------------------
 *  ITUT: F(SLR, PCLS), V(CDPA), O(CRED, CGPA, DATA, HOPC, IMP, EOP)
 *  ASNI: F(SLR, PCLS), V(CDPA), O(CRED, CGPA, DATA, HOPC, EOP)
 */
STATIC INLINE int
sccp_send_cr(queue_t *q, struct sp *sp, ulong dpc, ulong pri, ulong sls, ulong slr, ulong pcl,
	     struct sccp_addr *cdpa, ulong *cred, struct sccp_addr *cgpa, mblk_t *data, ulong *hopc,
	     ulong *imp)
{
	struct sccp_msg *m;
	if ((m = sccp_enc_msg(q, sp, dpc, pri, sls))) {
		m->type = SCCP_MT_CR;
		m->slr = slr;	/* SLR F */
		m->pcl = pcl;	/* PCL F */
		bcopy(cdpa, &m->cdpa, sizeof(*cdpa) + cdpa->alen);	/* CDPA V */
		if (cred) {	/* CRED O */
			m->cred = *cred;
			m->parms |= SCCP_PTF_CRED;
		}
		if (cgpa) {	/* CGPA O */
			bcopy(cgpa, &m->cgpa, sizeof(*cgpa) + cgpa->alen);
			m->parms |= SCCP_PTF_CGPA;
		}
		if (data) {	/* DATA O */
			m->data.buf = data;
			m->data.ptr = data->b_rptr;
			m->data.len = msgdsize(data);
			m->parms |= SCCP_PTF_DATA;
		}
		if (hopc) {	/* HOPC O */
			m->hopc = *hopc;
			m->parms |= SCCP_PTF_HOPC;
		}
		if (imp && !(sp->proto.popt & SS7_POPT_MPLEV)) {	/* IMP O */
			m->imp = *imp;
			if (m->imp > SCCP_MI_CR)
				m->imp = SCCP_MI_CR;
			m->parms |= SCCP_PTF_IMP;
		} else
			m->imp = SCCP_DI_CR;
		return sccp_orte_msg(q, m->bp);
	}
	return (-ENOBUFS);
}

/*
 *  SCCP_MT_CC    0x02 - Connection Confirm
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR, SLR, PCLS), O(CRED, CDPA, DATA, IMP, EOP)
 *  ANSI: F(DLR, SLR, PCLS), O(CRED, CDPA, DATA, EOP)
 */
STATIC INLINE int
sccp_send_cc(queue_t *q, struct sp *sp, ulong dpc, ulong sls, ulong dlr, ulong slr, ulong pcl,
	     ulong *cred, struct sccp_addr *cdpa, mblk_t *data, ulong *imp)
{
	struct sccp_msg *m;
	if ((m = sccp_enc_msg(q, sp, dpc, SCCP_MP_CC, sls))) {
		m->type = SCCP_MT_CC;
		m->dlr = dlr;	/* DLR F */
		m->slr = slr;	/* SLR F */
		m->pcl = pcl;	/* PCLS F */
		if (pcl == 3 && cred) {	/* CRED O */
			m->cred = *cred;
			m->parms |= SCCP_PTF_CRED;
		}
		if (cdpa) {	/* CDPA O */
			bcopy(cdpa, &m->cdpa, sizeof(*cdpa) + cdpa->alen);
			m->parms |= SCCP_PTF_CDPA;
		}
		if (data) {	/* DATA O */
			m->data.buf = data;
			m->data.ptr = data->b_rptr;
			m->data.len = msgdsize(data);
			m->parms |= SCCP_PTF_DATA;
		}
		if (imp && !(sp->proto.popt & SS7_POPT_MPLEV)) {	/* IMP O */
			m->imp = *imp;
			if (m->imp > SCCP_MI_CC)
				m->imp = SCCP_MI_CC;
			m->parms |= SCCP_PTF_IMP;
		} else
			m->imp = SCCP_DI_CC;
		return sccp_orte_msg(q, m->bp);
	}
	return (-ENOBUFS);
}

/*
 *  SCCP_MT_CREF  0x03 - Connection Refused
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR, REFC), O(CDPA, DATA, IMP, EOP)
 *  ANSI: F(DLR, REFC), O(CDPA, DATA, EOP)
 */
STATIC INLINE int
sccp_send_cref(queue_t *q, struct sp *sp, ulong dpc, ulong sls, ulong dlr, ulong refc,
	       struct sccp_addr *cdpa, mblk_t *data, ulong *imp)
{
	struct sccp_msg *m;
	if ((m = sccp_enc_msg(q, sp, dpc, SCCP_MP_CREF, sls))) {
		m->type = SCCP_MT_CREF;
		m->dlr = dlr;	/* DLR F */
		m->cause = refc;	/* REFC F */
		if (cdpa) {	/* CDPA O */
			bcopy(cdpa, &m->cdpa, sizeof(*cdpa) + cdpa->alen);
			m->parms |= SCCP_PTF_CDPA;
		}
		if (data) {	/* DATA O */
			m->data.buf = data;
			m->data.ptr = data->b_rptr;
			m->data.len = msgdsize(data);
			m->parms |= SCCP_PTF_DATA;
		}
		if (imp && !(sp->proto.popt & SS7_POPT_MPLEV)) {	/* IMP O */
			m->imp = *imp;
			if (m->imp > SCCP_MI_CREF)
				m->imp = SCCP_MI_CREF;
			m->parms |= SCCP_PTF_IMP;
		} else
			m->imp = SCCP_DI_CREF;
		return sccp_orte_msg(q, m->bp);
	}
	return (-ENOBUFS);
}

/*
 *  SCCP_MT_RLSD  0x04 - Released
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR, SLR, RELC), O(DATA, IMP, EOP)
 *  ANSI: F(DLR, SLR, RELC), O(DATA, EOP)
 */
STATIC INLINE int
sccp_send_rlsd(queue_t *q, struct sp *sp, ulong dpc, ulong sls, ulong dlr, ulong slr, ulong relc,
	       mblk_t *data, ulong *imp)
{
	struct sccp_msg *m;
	if ((m = sccp_enc_msg(q, sp, dpc, SCCP_MP_RLSD, sls))) {
		m->type = SCCP_MT_RLSD;
		m->dlr = dlr;	/* DLR F */
		m->slr = slr;	/* SLR F */
		m->cause = relc;	/* RELC F */
		if (data) {	/* DATA O */
			m->data.buf = data;
			m->data.ptr = data->b_rptr;
			m->data.len = msgdsize(data);
			m->parms |= SCCP_PTF_DATA;
		}
		if (imp && !(sp->proto.popt & SS7_POPT_MPLEV)) {	/* IMP O */
			m->imp = *imp;
			if (m->imp > SCCP_MI_RLSD)
				m->imp = SCCP_MI_RLSD;
			m->parms |= SCCP_PTF_IMP;
		} else
			m->imp = SCCP_DI_RLSD;
		return sccp_orte_msg(q, m->bp);
	}
	return (-ENOBUFS);
}

/*
 *  SCCP_MT_RLC   0x05 - Release Complete
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR, SLR)
 *  ANSI: F(DLR, SLR)
 */
STATIC INLINE int
sccp_send_rlc(queue_t *q, struct sp *sp, ulong dpc, ulong sls, ulong dlr, ulong slr)
{
	struct sccp_msg *m;
	if ((m = sccp_enc_msg(q, sp, dpc, SCCP_MP_RLC, sls))) {
		m->type = SCCP_MT_RLC;
		m->dlr = dlr;	/* DLR F */
		m->slr = slr;	/* SLR F */
		m->imp = SCCP_DI_RLC;
		return sccp_orte_msg(q, m->bp);
	}
	return (-ENOBUFS);
}

/*
 *  SCCP_MT_DT1   0x06 - Data Form 1
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR, SEG), V(DATA)
 *  ANSI: F(DLR, SEG), V(DATA)
 */
STATIC INLINE int
sccp_send_dt1(queue_t *q, struct sp *sp, ulong dpc, ulong pri, ulong sls, ulong dlr, ulong more,
	      mblk_t *data)
{
	struct sccp_msg *m;
	if ((m = sccp_enc_msg(q, sp, dpc, pri, sls))) {
		m->type = SCCP_MT_DT1;
		m->dlr = dlr;	/* DLR F */
		m->more = more;	/* SEG F */
		m->data.buf = data;	/* DATA V */
		m->data.ptr = data->b_rptr;
		m->data.len = msgdsize(data);
		m->imp = SCCP_DI_DT1;
		return sccp_orte_msg(q, m->bp);
	}
	return (-ENOBUFS);
}

/*
 *  SCCP_MT_DT2   0x07 - Data Form 2
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR, SEQ), V(DATA)
 *  ANSI: F(DLR, SEQ), V(DATA)
 */
STATIC INLINE int
sccp_send_dt2(queue_t *q, struct sp *sp, ulong dpc, ulong pri, ulong sls, ulong dlr, ulong ps,
	      ulong pr, ulong more, mblk_t *data)
{
	struct sccp_msg *m;
	if ((m = sccp_enc_msg(q, sp, dpc, pri, sls))) {
		m->type = SCCP_MT_DT2;
		m->dlr = dlr;	/* DLR F */
		m->more = more;	/* SEG F */
		m->data.buf = data;	/* DATA V */
		m->data.ptr = data->b_rptr;
		m->data.len = msgdsize(data);
		m->imp = SCCP_DI_DT2;
		return sccp_orte_msg(q, m->bp);
	}
	return (-ENOBUFS);
}

/*
 *  SCCP_MT_AK    0x08 - Data Acknowledgement
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR, RSN, CRED)
 *  ANSI: F(DLR, RSN, CRED)
 */
STATIC INLINE int
sccp_send_ak(queue_t *q, struct sp *sp, ulong dpc, ulong pri, ulong sls, ulong dlr, ulong rsn,
	     ulong cred)
{
	struct sccp_msg *m;
	if ((m = sccp_enc_msg(q, sp, dpc, pri, sls))) {
		m->type = SCCP_MT_AK;
		m->dlr = dlr;	/* DLR F */
		m->rsn = rsn;	/* RSN F */
		m->cred = cred;	/* CRED F */
		m->imp = SCCP_DI_AK;
		return sccp_orte_msg(q, m->bp);
	}
	return (-ENOBUFS);
}

/*
 *  SCCP_MT_UDT   0x09 - Unitdata
 *  -------------------------------------------------------------------------
 *  ITUT: F(PCLS), V(CDPA, CGPA, DATA[2-254])
 *  ANSI: F(PCLS), V(CDPA, CGPA, DATA[2-252])
 */
STATIC INLINE int
sccp_send_udt(queue_t *q, struct sp *sp, ulong dpc, ulong pri, ulong sls, ulong pcl, ulong ret,
	      struct sccp_addr *cdpa, struct sccp_addr *cgpa, mblk_t *data)
{
	struct sccp_msg *m;
	if ((m = sccp_enc_msg(q, sp, dpc, pri, sls))) {
		m->type = SCCP_MT_UDT;
		m->pcl = pcl;	/* PCLS F */
		m->ret = ret;
		bcopy(cdpa, &m->cdpa, sizeof(*cdpa) + cdpa->alen);	/* CDPA V */
		bcopy(cgpa, &m->cgpa, sizeof(*cgpa) + cgpa->alen);	/* CGPA V */
		m->data.buf = data;	/* DATA V */
		m->data.ptr = data->b_rptr;
		m->data.len = msgdsize(data);
		m->imp = SCCP_DI_UDT;
		return sccp_orte_msg(q, m->bp);
	}
	return (-ENOBUFS);
}

/*
 *  SCCP_MT_UDTS  0x0a - Unitdata Service
 *  -------------------------------------------------------------------------
 *  ITUT: F(RETC), V(CDPA, CGPA, DATA[2-253])
 *  ANSI: F(RETC), V(CDPA, CGPA, DATA[2-251])
 */
STATIC INLINE int
sccp_send_udts(queue_t *q, struct sp *sp, ulong dpc, ulong pri, ulong sls, ulong retc,
	       struct sccp_addr *cdpa, struct sccp_addr *cgpa, mblk_t *data)
{
	struct sccp_msg *m;
	if ((m = sccp_enc_msg(q, sp, dpc, pri, sls))) {
		m->type = SCCP_MT_UDTS;
		m->cause = retc;	/* RETC F */
		bcopy(cdpa, &m->cdpa, sizeof(*cdpa) + cdpa->alen);	/* CDPA V */
		bcopy(cgpa, &m->cgpa, sizeof(*cgpa) + cgpa->alen);	/* CGPA V */
		m->data.buf = data;	/* DATA V */
		m->data.ptr = data->b_rptr;
		m->data.len = msgdsize(data);
		m->imp = SCCP_DI_UDTS;
		return sccp_orte_msg(q, m->bp);
	}
	return (-ENOBUFS);
}

/*
 *  SCCP_MT_ED    0x0b - Expedited Data
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR), V(DATA)
 *  ANSI: F(DLR), V(DATA)
 */
STATIC INLINE int
sccp_send_ed(queue_t *q, struct sp *sp, ulong dpc, ulong sls, ulong dlr, mblk_t *data)
{
	struct sccp_msg *m;
	if ((m = sccp_enc_msg(q, sp, dpc, SCCP_MP_ED, sls))) {
		m->type = SCCP_MT_ED;
		m->dlr = dlr;	/* DLR F */
		m->data.buf = data;	/* DATA V */
		m->data.ptr = data->b_rptr;
		m->data.len = msgdsize(data);
		m->imp = SCCP_DI_ED;
		return sccp_orte_msg(q, m->bp);
	}
	return (-ENOBUFS);
}

/*
 *  SCCP_MT_EA    0x0c - Expedited Data Acknowledgement
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR)
 *  ANSI: F(DLR)
 */
STATIC INLINE int
sccp_send_ea(queue_t *q, struct sp *sp, ulong dpc, ulong sls, ulong dlr)
{
	struct sccp_msg *m;
	if ((m = sccp_enc_msg(q, sp, dpc, SCCP_MP_EA, sls))) {
		m->type = SCCP_MT_ED;
		m->dlr = dlr;	/* DLR F */
		m->imp = SCCP_DI_EA;
		return sccp_orte_msg(q, m->bp);
	}
	return (-ENOBUFS);
}

/*
 *  SCCP_MT_RSR   0x0d - Reset Request
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR, SLR, RESC)
 *  ANSI: F(DLR, SLR, RESC)
 */
STATIC INLINE int
sccp_send_rsr(queue_t *q, struct sp *sp, ulong dpc, ulong sls, ulong dlr, ulong slr, ulong resc)
{
	struct sccp_msg *m;
	if ((m = sccp_enc_msg(q, sp, dpc, SCCP_MP_RSR, sls))) {
		m->type = SCCP_MT_RSR;
		m->dlr = dlr;	/* DLR F */
		m->slr = slr;	/* SLR F */
		m->cause = resc;	/* RESC F */
		m->imp = SCCP_DI_RSR;
		return sccp_orte_msg(q, m->bp);
	}
	return (-ENOBUFS);
}

/*
 *  SCCP_MT_RSC   0x0e - Reset Confirm
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR, SLR)
 *  ANSI: F(DLR, SLR)
 */
STATIC INLINE int
sccp_send_rsc(queue_t *q, struct sp *sp, ulong dpc, ulong sls, ulong dlr, ulong slr)
{
	struct sccp_msg *m;
	if ((m = sccp_enc_msg(q, sp, dpc, SCCP_MP_RSC, sls))) {
		m->type = SCCP_MT_RSC;
		m->dlr = dlr;	/* DLR F */
		m->slr = slr;	/* SLR F */
		m->imp = SCCP_DI_RSC;
		return sccp_orte_msg(q, m->bp);
	}
	return (-ENOBUFS);
}

/*
 *  SCCP_MT_ERR   0x0f - Protocol Data Unit Error
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR, ERRC)
 *  ANSI: F(DLR, ERRC)
 */
STATIC INLINE int
sccp_send_err(queue_t *q, struct sp *sp, ulong dpc, ulong sls, ulong dlr, ulong errc)
{
	struct sccp_msg *m;
	if ((m = sccp_enc_msg(q, sp, dpc, SCCP_MP_ERR, sls))) {
		m->type = SCCP_MT_ERR;
		m->dlr = dlr;	/* DLR F */
		m->cause = errc;	/* ERRC F */
		m->imp = SCCP_DI_ERR;
		return sccp_orte_msg(q, m->bp);
	}
	return (-ENOBUFS);
}

/*
 *  SCCP_MT_IT    0x10 - Inactivity Test
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR, SLR, PCLS, SEQ, CRED)
 *  ANSI: F(DLR, SLR, PCLS, SEQ, CRED)
 */
STATIC INLINE int
sccp_send_it(queue_t *q, struct sp *sp, ulong dpc, ulong sls, ulong dlr, ulong slr, ulong pcl,
	     ulong ps, ulong pr, ulong more, ulong cred)
{
	struct sccp_msg *m;
	if ((m = sccp_enc_msg(q, sp, dpc, SCCP_MP_IT, sls))) {
		m->type = SCCP_MT_IT;
		m->dlr = dlr;	/* DLR F */
		m->slr = slr;	/* SLR F */
		m->pcl = pcl;	/* PCLS F */
		m->ps = ps;	/* SEQ F */
		m->pr = pr;
		m->more = more;
		m->cred = cred;	/* CRED F */
		m->imp = SCCP_DI_IT;
		return sccp_orte_msg(q, m->bp);
	}
	return (-ENOBUFS);
}

/*
 *  SCCP_MT_XUDT  0x11 - Extended Unitdata
 *  -------------------------------------------------------------------------
 *  ITUT: F(PCLS, HOPC), V(CDPA, CGPA, DATA[2-253]), O(SGMT, IMP, EOP)
 *  ANSI: F(PCLS, HOPC), V(CDPA, CGPA, DATA[2-251]), O(SGMT, ISNI, INS, MTI, EOP)
 */
STATIC INLINE int
sccp_send_xudt(queue_t *q, struct sp *sp, ulong dpc, ulong pri, ulong sls, ulong pcl, ulong ret,
	       ulong hopc, struct sccp_addr *cdpa, struct sccp_addr *cgpa, mblk_t *data,
	       sccp_sgmt_t * sgmt, ulong *imp, sccp_isni_t * isni, sccp_ins_t * ins, ulong *mti)
{
	struct sccp_msg *m;
	if ((m = sccp_enc_msg(q, sp, dpc, pri, sls))) {
		m->type = SCCP_MT_XUDT;
		m->pcl = pcl;	/* PCLS F */
		m->ret = ret;
		m->hopc = hopc;	/* HOPC F */
		bcopy(cdpa, &m->cdpa, sizeof(*cdpa) + cdpa->alen);	/* CDPA V */
		bcopy(cgpa, &m->cgpa, sizeof(*cgpa) + cgpa->alen);	/* CGPA V */
		m->data.buf = data;	/* DATA V */
		m->data.ptr = data->b_rptr;
		m->data.len = msgdsize(data);
		if (sgmt) {	/* SGMT O */
			m->sgmt = *sgmt;
			m->parms |= SCCP_PTF_SGMT;
		}
		if (imp && !(sp->proto.popt & SS7_POPT_MPLEV)) {	/* IMP O */
			m->imp = *imp;
			if (m->imp > SCCP_MI_XUDT)
				m->imp = SCCP_MI_XUDT;
			m->parms |= SCCP_PTF_IMP;
		} else
			m->imp = SCCP_DI_XUDT;
		if ((m->pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) {
			if (isni) {	/* ISNI O */
				m->isni = *isni;
				m->parms |= SCCP_PTF_ISNI;
			}
			if (ins) {	/* INS O */
				m->ins = *ins;
				m->parms |= SCCP_PTF_INS;
			}
			if (mti) {	/* MTI O */
				m->mti = *mti;
				m->parms |= SCCP_PTF_MTI;
			}
		}
		return sccp_orte_msg(q, m->bp);
	}
	return (-ENOBUFS);
}

/*
 *  SCCP_MT_XUDTS 0x12 - Extended Unitdata Service
 *  -------------------------------------------------------------------------
 *  ITUT: F(RETC, HOPC), V(CDPA, CGPA, DATA[2-253]), O(SGMT, IMP, EOP)
 *  ANSI: F(RETC, HOPC), V(CDPA, CGPA, DATA[2-251]), O(SGMT, INS, MTI, ISNI, EOP)
 */
STATIC INLINE int
sccp_send_xudts(queue_t *q, struct sp *sp, ulong dpc, ulong pri, ulong sls, ulong retc, ulong hopc,
		struct sccp_addr *cdpa, struct sccp_addr *cgpa, mblk_t *data, sccp_sgmt_t * sgmt,
		ulong *imp, sccp_ins_t * ins, ulong *mti, sccp_isni_t * isni)
{
	struct sccp_msg *m;
	if ((m = sccp_enc_msg(q, sp, dpc, pri, sls))) {
		m->type = SCCP_MT_XUDTS;
		m->cause = retc;	/* RETC F */
		bcopy(cdpa, &m->cdpa, sizeof(*cdpa) + cdpa->alen);	/* CDPA V */
		bcopy(cgpa, &m->cgpa, sizeof(*cgpa) + cgpa->alen);	/* CGPA V */
		m->data.buf = data;	/* DATA V */
		m->data.ptr = data->b_rptr;
		m->data.len = msgdsize(data);
		if (sgmt) {	/* SGMT O */
			m->sgmt = *sgmt;
			m->parms |= SCCP_PTF_SGMT;
		}
		if (imp && !(sp->proto.popt & SS7_POPT_MPLEV)) {	/* IMP O */
			m->imp = *imp;
			if (m->imp > SCCP_MI_XUDTS)
				m->imp = SCCP_MI_XUDTS;
			m->parms |= SCCP_PTF_IMP;
		} else
			m->imp = SCCP_DI_XUDTS;
		if ((m->pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) {
			if (isni) {	/* ISNI O */
				m->isni = *isni;
				m->parms |= SCCP_PTF_ISNI;
			}
			if (ins) {	/* INS O */
				m->ins = *ins;
				m->parms |= SCCP_PTF_INS;
			}
			if (mti) {	/* MTI O */
				m->mti = *mti;
				m->parms |= SCCP_PTF_MTI;
			}
		}
		return sccp_orte_msg(q, m->bp);
	}
	return (-ENOBUFS);
}

/*
 *  SCCP_MT_LUDT  0x13 - Long Unitdata
 *  -------------------------------------------------------------------------
 *  ITUT: F(PCLS, HOPC), V(CDPA, CGPA, LDATA[3-3906]), O(SGMT, IMP, EOP)
 *  ANSI: F(PCLS, HOPC), V(CDPA, CGPA, LDATA[3-3904]), O(SGMT, ISNI, INS, EOP)
 */
STATIC INLINE int
sccp_send_ludt(queue_t *q, struct sp *sp, ulong dpc, ulong pri, ulong sls, ulong pcl, ulong ret,
	       ulong hopc, struct sccp_addr *cdpa, struct sccp_addr *cgpa, mblk_t *data,
	       sccp_sgmt_t * sgmt, ulong *imp, sccp_isni_t * isni, sccp_ins_t * ins)
{
	struct sccp_msg *m;
	if ((m = sccp_enc_msg(q, sp, dpc, pri, sls))) {
		m->type = SCCP_MT_LUDT;
		m->pcl = pcl;	/* PCLS F */
		m->ret = ret;
		m->hopc = hopc;	/* HOPC F */
		bcopy(cdpa, &m->cdpa, sizeof(*cdpa) + cdpa->alen);	/* CDPA V */
		bcopy(cgpa, &m->cgpa, sizeof(*cgpa) + cgpa->alen);	/* CGPA V */
		m->data.buf = data;	/* DATA V */
		m->data.ptr = data->b_rptr;
		m->data.len = msgdsize(data);
		if (sgmt) {	/* SGMT O */
			m->sgmt = *sgmt;
			m->parms |= SCCP_PTF_SGMT;
		}
		if (imp && !(sp->proto.popt & SS7_POPT_MPLEV)) {	/* IMP O */
			m->imp = *imp;
			if (m->imp > SCCP_MI_LUDT)
				m->imp = SCCP_MI_LUDT;
			m->parms |= SCCP_PTF_IMP;
		} else
			m->imp = SCCP_DI_LUDT;
		if ((m->pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) {
			if (isni) {	/* ISNI O */
				m->isni = *isni;
				m->parms |= SCCP_PTF_ISNI;
			}
			if (ins) {	/* INS O */
				m->ins = *ins;
				m->parms |= SCCP_PTF_INS;
			}
		}
		return sccp_orte_msg(q, m->bp);
	}
	return (-ENOBUFS);
}

/*
 *  SCCP_MT_LUDTS 0x14 - Long Unitdata Service
 *  -------------------------------------------------------------------------
 *  ITUT: F(RETC, HOPC), V(CDPA, CGPA, LDATA[3-3906]), O(SGMT, IMP, EOP)
 *  ANSI: F(RETC, HOPC), V(CDPA, CGPA, LDATA[3-3904]), O(SGMT, ISNI, INS, EOP)
 */
STATIC INLINE int
sccp_send_ludts(queue_t *q, struct sp *sp, ulong dpc, ulong pri, ulong sls, ulong retc, ulong hopc,
		struct sccp_addr *cdpa, struct sccp_addr *cgpa, mblk_t *data, sccp_sgmt_t * sgmt,
		ulong *imp, sccp_isni_t * isni, sccp_ins_t * ins)
{
	struct sccp_msg *m;
	if ((m = sccp_enc_msg(q, sp, dpc, pri, sls))) {
		m->type = SCCP_MT_LUDTS;
		m->cause = retc;	/* RETC F */
		bcopy(cdpa, &m->cdpa, sizeof(*cdpa) + cdpa->alen);	/* CDPA V */
		bcopy(cgpa, &m->cgpa, sizeof(*cgpa) + cgpa->alen);	/* CGPA V */
		m->data.buf = data;	/* DATA V */
		m->data.ptr = data->b_rptr;
		m->data.len = msgdsize(data);
		if (sgmt) {	/* SGMT O */
			m->sgmt = *sgmt;
			m->parms |= SCCP_PTF_SGMT;
		}
		if (imp && !(sp->proto.popt & SS7_POPT_MPLEV)) {	/* IMP O */
			m->imp = *imp;
			if (m->imp > SCCP_MI_LUDTS)
				m->imp = SCCP_MI_LUDTS;
			m->parms |= SCCP_PTF_IMP;
		} else
			m->imp = SCCP_DI_LUDTS;
		if ((m->pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) {
			if (isni) {	/* ISNI O */
				m->isni = *isni;
				m->parms |= SCCP_PTF_ISNI;
			}
			if (ins) {	/* INS O */
				m->ins = *ins;
				m->parms |= SCCP_PTF_INS;
			}
		}
		return sccp_orte_msg(q, m->bp);
	}
	return (-ENOBUFS);
}

/*
 *  SCMG Message
 *  -------------------------------------------------------------------------
 */
STATIC int
sccp_send_scmg(queue_t *q, struct sp *sp, ulong dpc, ulong pri, ulong sls, struct sccp_addr *cdpa,
	       struct sccp_addr *cgpa, ulong scmgfi, ulong assn, ulong apc, ulong smi, ulong cong)
{
	struct sccp_msg *m;
	if ((m = sccp_enc_msg(q, sp, dpc, pri, sls))) {
		m->type = SCCP_MT_UDT;
		m->pcl = 0;	/* PCLS F */
		m->ret = 0;
		bcopy(cdpa, &m->cdpa, sizeof(*cdpa) + cdpa->alen);	/* CDPA V */
		bcopy(cgpa, &m->cgpa, sizeof(*cgpa) + cgpa->alen);	/* CGPA V */
		m->fi = scmgfi;
		m->assn = assn;
		m->apc = apc;
		m->smi = smi;
		m->cong = cong;
		return sccp_orte_msg(q, m->bp);
	}
	return (-ENOBUFS);
}

/*
 *  SCMG_MT_SSA     0x01 - Subsystem allowed
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
sccp_send_ssa(queue_t *q, struct sp *sp, ulong dpc, struct sccp_addr *cdpa, struct sccp_addr *cgpa,
	      ulong assn, ulong apc, ulong smi)
{
	return sccp_send_scmg(q, sp, dpc, SCMG_MP_SSA, sp->sccp_next_cl_sls++, cdpa, cgpa,
			      SCMG_MT_SSA, assn, apc, smi, 0);
}
STATIC INLINE int
sccp_bcast_ssa(queue_t *q, struct sp *sp, ulong dpc, ulong assn, ulong apc, ulong smi)
{
	int err;
	struct sr *sr;
	struct sccp_addr cgpa;
	bzero(&cgpa, sizeof(cgpa));
	cgpa.ni = sp->add.ni;
	cgpa.ri = SCCP_RI_DPC_SSN;
	cgpa.pc = sp->add.pc;
	cgpa.ssn = 1;
	for (sr = sp->sr.list; sr; sr = sr->sp.next) {
		struct sccp_addr cdpa;
		bzero(&cdpa, sizeof(cdpa));
		cdpa.ni = sr->add.ni;
		cdpa.ri = SCCP_RI_DPC_SSN;
		cdpa.pc = sr->add.pc;
		cdpa.ssn = 1;
		if ((err = sccp_send_ssa(q, sp, dpc, &cdpa, &cgpa, assn, apc, smi)) < 0)
			goto error;
	}
	return (QR_DONE);
      error:
	return (err);
}

/*
 *  SCMG_MT_SSP     0x02 - Subsystem prohibited
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
sccp_send_ssp(queue_t *q, struct sp *sp, ulong dpc, struct sccp_addr *cdpa, struct sccp_addr *cgpa,
	      ulong assn, ulong apc, ulong smi)
{
	return sccp_send_scmg(q, sp, dpc, SCMG_MP_SSP, sp->sccp_next_cl_sls++, cdpa, cgpa,
			      SCMG_MT_SSP, assn, apc, smi, 0);
}

/*
 *  SCMG_MT_SST     0x03 - Subsystem status test
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
sccp_send_sst(queue_t *q, struct sp *sp, ulong dpc, struct sccp_addr *cdpa, struct sccp_addr *cgpa,
	      ulong assn, ulong apc, ulong smi)
{
	return sccp_send_scmg(q, sp, dpc, SCMG_MP_SST, sp->sccp_next_cl_sls++, cdpa, cgpa,
			      SCMG_MT_SST, assn, apc, smi, 0);
}

/*
 *  SCMG_MT_SOR     0x04 - Subsystem out of service request
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
sccp_send_sor(queue_t *q, struct sp *sp, ulong dpc, struct sccp_addr *cdpa, struct sccp_addr *cgpa,
	      ulong assn, ulong apc, ulong smi)
{
	return sccp_send_scmg(q, sp, dpc, SCMG_MP_SOR, sp->sccp_next_cl_sls++, cdpa, cgpa,
			      SCMG_MT_SOR, assn, apc, smi, 0);
}

/*
 *  SCMG_MT_SOG     0x05 - Subsystem out of service grant
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
sccp_send_sog(queue_t *q, struct sp *sp, ulong dpc, struct sccp_addr *cdpa, struct sccp_addr *cgpa,
	      ulong assn, ulong apc, ulong smi)
{
	return sccp_send_scmg(q, sp, dpc, SCMG_MP_SOG, sp->sccp_next_cl_sls++, cdpa, cgpa,
			      SCMG_MT_SOG, assn, apc, smi, 0);
}

/*
 *  SCMG_MT_SSC     0x06 - Subsystem congestion
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
sccp_send_ssc(queue_t *q, struct sp *sp, ulong dpc, struct sccp_addr *cdpa, struct sccp_addr *cgpa,
	      ulong assn, ulong apc, ulong smi, ulong cong)
{
	return sccp_send_scmg(q, sp, dpc, SCMG_MP_SSC, sp->sccp_next_cl_sls++, cdpa, cgpa,
			      SCMG_MT_SSC, assn, apc, smi, cong);
}

/*
 *  SCMG_MT_SBR     0xfd - Subsystem backup routing
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
sccp_send_sbr(queue_t *q, struct sp *sp, ulong dpc, struct sccp_addr *cdpa, struct sccp_addr *cgpa,
	      ulong assn, ulong apc, ulong smi)
{
	return sccp_send_scmg(q, sp, dpc, SCMG_MP_SBR, sp->sccp_next_cl_sls++, cdpa, cgpa,
			      SCMG_MT_SBR, assn, apc, smi, 0);
}

/*
 *  SCMG_MT_SNR     0xfe - Subsystem normal routing
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
sccp_send_snr(queue_t *q, struct sp *sp, ulong dpc, struct sccp_addr *cdpa, struct sccp_addr *cgpa,
	      ulong assn, ulong apc, ulong smi)
{
	return sccp_send_scmg(q, sp, dpc, SCMG_MP_SNR, sp->sccp_next_cl_sls++, cdpa, cgpa,
			      SCMG_MT_SNR, assn, apc, smi, 0);
}

/*
 *  SCMG_MT_SRT     0xff - Subsystem routing status test
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
sccp_send_srt(queue_t *q, struct sp *sp, ulong dpc, struct sccp_addr *cdpa, struct sccp_addr *cgpa,
	      ulong assn, ulong apc, ulong smi)
{
	return sccp_send_scmg(q, sp, dpc, SCMG_MP_SRT, sp->sccp_next_cl_sls++, cdpa, cgpa,
			      SCMG_MT_SRT, assn, apc, smi, 0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Configuration Defaults
 *
 *  -------------------------------------------------------------------------
 */

STATIC sccp_opt_conf_na_t itut_na_config_default = {
};
STATIC sccp_opt_conf_na_t etsi_na_config_default = {
};
STATIC sccp_opt_conf_na_t ansi_na_config_default = {
};
STATIC sccp_opt_conf_na_t jttc_na_config_default = {
};

#if 0
STATIC sccp_opt_conf_sp_t itut_sp_config_default = {
};
STATIC sccp_opt_conf_sp_t etsi_sp_config_default = {
};
STATIC sccp_opt_conf_sp_t ansi_sp_config_default = {
};
STATIC sccp_opt_conf_sp_t jttc_sp_config_default = {
};

STATIC sccp_opt_conf_sr_t itut_sr_config_default = {
};
STATIC sccp_opt_conf_sr_t etsi_sr_config_default = {
};
STATIC sccp_opt_conf_sr_t ansi_sr_config_default = {
};
STATIC sccp_opt_conf_sr_t jttc_sr_config_default = {
};
#endif

/*
 *  -------------------------------------------------------------------------
 *
 *  Timers
 *
 *  -------------------------------------------------------------------------
 */

enum { tall, tcon, tias, tiar, trel, trel2, tint, tguard, tres, trea, tack, tgtt,
	tattack, tdecay, tstatinfo, tisst, twsog, tsst
};

/*
 *  SCCP timers
 *  -------------------------------------------------------------------------
 */
SS7_DECLARE_TIMER(DRV_NAME, sc, tcon, config);
SS7_DECLARE_TIMER(DRV_NAME, sc, tias, config);
SS7_DECLARE_TIMER(DRV_NAME, sc, tiar, config);
SS7_DECLARE_TIMER(DRV_NAME, sc, trel, config);
SS7_DECLARE_TIMER(DRV_NAME, sc, trel2, config);
SS7_DECLARE_TIMER(DRV_NAME, sc, tint, config);
SS7_DECLARE_TIMER(DRV_NAME, sc, tguard, config);
SS7_DECLARE_TIMER(DRV_NAME, sc, tres, config);
SS7_DECLARE_TIMER(DRV_NAME, sc, trea, config);
SS7_DECLARE_TIMER(DRV_NAME, sc, tack, config);

STATIC INLINE void
__sccp_timer_stop(struct sc *sc, const ulong t)
{
	int single = 1;
	switch (t) {
	case tall:
		single = 0;
		/* 
		   fall through */
	case tcon:
		sc_stop_timer_tcon(sc);
		if (single)
			break;
		/* 
		   fall through */
	case tias:
		sc_stop_timer_tias(sc);
		if (single)
			break;
		/* 
		   fall through */
	case tiar:
		sc_stop_timer_tiar(sc);
		if (single)
			break;
		/* 
		   fall through */
	case trel:
		sc_stop_timer_trel(sc);
		if (single)
			break;
		/* 
		   fall through */
	case trel2:
		sc_stop_timer_trel2(sc);
		if (single)
			break;
		/* 
		   fall through */
	case tint:
		sc_stop_timer_tint(sc);
		if (single)
			break;
		/* 
		   fall through */
	case tguard:
		sc_stop_timer_tguard(sc);
		if (single)
			break;
		/* 
		   fall through */
	case tres:
		sc_stop_timer_tres(sc);
		if (single)
			break;
		/* 
		   fall through */
	case trea:
		sc_stop_timer_trea(sc);
		if (single)
			break;
		/* 
		   fall through */
	case tack:
		sc_stop_timer_tack(sc);
		if (single)
			break;
		/* 
		   fall through */
		break;
	default:
		swerr();
		break;
	}
}
STATIC INLINE void
sccp_timer_stop(struct sc *sc, const ulong t)
{
	psw_t flags;
	spin_lock_irqsave(&sc->lock, flags);
	{
		__sccp_timer_stop(sc, t);
	}
	spin_unlock_irqrestore(&sc->lock, flags);
}
STATIC INLINE void
sccp_timer_start(struct sc *sc, const ulong t)
{
	psw_t flags;
	spin_lock_irqsave(&sc->lock, flags);
	{
		__sccp_timer_stop(sc, t);
		switch (t) {
		case tcon:
			sc_start_timer_tcon(sc);
			break;
		case tias:
			sc_start_timer_tias(sc);
			break;
		case tiar:
			sc_start_timer_tiar(sc);
			break;
		case trel:
			sc_start_timer_trel(sc);
			break;
		case trel2:
			sc_start_timer_trel2(sc);
			break;
		case tint:
			sc_start_timer_tint(sc);
			break;
		case tguard:
			sc_start_timer_tguard(sc);
			break;
		case tres:
			sc_start_timer_tres(sc);
			break;
		case trea:
			sc_start_timer_trea(sc);
			break;
		case tack:
			sc_start_timer_tack(sc);
			break;
		default:
			swerr();
			break;
		}
	}
	spin_unlock_irqrestore(&sc->lock, flags);
}

/*
 *  SS timers
 *  -------------------------------------------------------------------------
 */
SS7_DECLARE_TIMER(DRV_NAME, ss, tisst, config);
SS7_DECLARE_TIMER(DRV_NAME, ss, twsog, config);

STATIC INLINE void
__ss_timer_stop(struct ss *ss, const ulong t)
{
	int single = 1;
	switch (t) {
	case tall:
		single = 0;
		/* 
		   fall through */
	case tisst:
		ss_stop_timer_tisst(ss);
		if (single)
			break;
		/* 
		   fall through */
	case twsog:
		ss_stop_timer_twsog(ss);
		if (single)
			break;
		/* 
		   fall through */
		break;
	default:
		swerr();
		break;
	}
}
STATIC INLINE void
ss_timer_stop(struct ss *ss, const ulong t)
{
	psw_t flags;
	spin_lock_irqsave(&ss->lock, flags);
	{
		__ss_timer_stop(ss, t);
	}
	spin_unlock_irqrestore(&ss->lock, flags);
}
STATIC INLINE void
ss_timer_start(struct ss *ss, const ulong t)
{
	psw_t flags;
	spin_lock_irqsave(&ss->lock, flags);
	{
		__ss_timer_stop(ss, t);
		switch (t) {
		case tisst:
			ss_start_timer_tisst(ss);
			break;
		case twsog:
			ss_start_timer_twsog(ss);
			break;
		default:
			swerr();
			break;
		}
	}
	spin_unlock_irqrestore(&ss->lock, flags);
}

/*
 *  RS timers
 *  -------------------------------------------------------------------------
 */
SS7_DECLARE_TIMER(DRV_NAME, rs, tsst, config);

STATIC INLINE void
__rs_timer_stop(struct rs *rs, const ulong t)
{
	int single = 1;
	switch (t) {
	case tall:
		single = 0;
		/* 
		   fall through */
	case tsst:
		rs_stop_timer_tsst(rs);
		if (single)
			break;
		/* 
		   fall through */
		break;
	default:
		swerr();
		break;
	}
}
STATIC INLINE void
rs_timer_stop(struct rs *rs, const ulong t)
{
	psw_t flags;
	spin_lock_irqsave(&rs->lock, flags);
	{
		__rs_timer_stop(rs, t);
	}
	spin_unlock_irqrestore(&rs->lock, flags);
}
STATIC INLINE void
rs_timer_start(struct rs *rs, const ulong t)
{
	psw_t flags;
	spin_lock_irqsave(&rs->lock, flags);
	{
		__rs_timer_stop(rs, t);
		switch (t) {
		case tsst:
			rs_start_timer_tsst(rs);
			break;
		default:
			swerr();
			break;
		}
	}
	spin_unlock_irqrestore(&rs->lock, flags);
}

/*
 *  SP timers
 *  -------------------------------------------------------------------------
 */
SS7_DECLARE_TIMER(DRV_NAME, sp, tgtt, config);

STATIC INLINE void
__sp_timer_stop(struct sp *sp, const ulong t)
{
	int single = 1;
	switch (t) {
	case tall:
		single = 0;
		/* 
		   fall through */
	case tgtt:
		sp_stop_timer_tgtt(sp);
		if (single)
			break;
		/* 
		   fall through */
		break;
	default:
		swerr();
		break;
	}
}
STATIC INLINE void
sp_timer_stop(struct sp *sp, const ulong t)
{
	psw_t flags;
	spin_lock_irqsave(&sp->lock, flags);
	{
		__sp_timer_stop(sp, t);
	}
	spin_unlock_irqrestore(&sp->lock, flags);
}
STATIC INLINE void
sp_timer_start(struct sp *sp, const ulong t)
{
	psw_t flags;
	spin_lock_irqsave(&sp->lock, flags);
	{
		__sp_timer_stop(sp, t);
		switch (t) {
		case tgtt:
			sp_start_timer_tgtt(sp);
			break;
		default:
			swerr();
			break;
		}
	}
	spin_unlock_irqrestore(&sp->lock, flags);
}

/*
 *  SR timers
 *  -------------------------------------------------------------------------
 */
SS7_DECLARE_TIMER(DRV_NAME, sr, tattack, config);
SS7_DECLARE_TIMER(DRV_NAME, sr, tdecay, config);
SS7_DECLARE_TIMER(DRV_NAME, sr, tstatinfo, config);
SS7_DECLARE_TIMER(DRV_NAME, sr, tsst, config);

STATIC INLINE void
__sr_timer_stop(struct sr *sr, const ulong t)
{
	int single = 1;
	switch (t) {
	case tall:
		single = 0;
		/* 
		   fall through */
	case tattack:
		sr_stop_timer_tattack(sr);
		if (single)
			break;
		/* 
		   fall through */
	case tdecay:
		sr_stop_timer_tdecay(sr);
		if (single)
			break;
		/* 
		   fall through */
	case tstatinfo:
		sr_stop_timer_tstatinfo(sr);
		if (single)
			break;
		/* 
		   fall through */
	case tsst:
		sr_stop_timer_tsst(sr);
		if (single)
			break;
		/* 
		   fall through */
		break;
	default:
		swerr();
		break;
	}
}
STATIC INLINE void
sr_timer_stop(struct sr *sr, const ulong t)
{
	psw_t flags;
	spin_lock_irqsave(&sr->lock, flags);
	{
		__sr_timer_stop(sr, t);
	}
	spin_unlock_irqrestore(&sr->lock, flags);
}
STATIC INLINE void
sr_timer_start(struct sr *sr, const ulong t)
{
	psw_t flags;
	spin_lock_irqsave(&sr->lock, flags);
	{
		__sr_timer_stop(sr, t);
		switch (t) {
		case tattack:
			sr_start_timer_tattack(sr);
			break;
		case tdecay:
			sr_start_timer_tdecay(sr);
			break;
		case tstatinfo:
			sr_start_timer_tstatinfo(sr);
			break;
		case tsst:
			sr_start_timer_tsst(sr);
			break;
		default:
			swerr();
			break;
		}
	}
	spin_unlock_irqrestore(&sr->lock, flags);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SCCP State Machines
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  -------------------------------------------------------------------------
 *
 *  SCCP Interface Functions
 *
 *  -------------------------------------------------------------------------
 *  These interface functions adapt between user interface events requested by the state machine and the particular
 *  style of interface that is supported by the particular SCCP user stream.  Not all events are supported by all
 *  interface styles.  The most complete interface style is the SCCPI which is the NPI Version 2 interface with the
 *  extended N-primitives described in Q.711 and T1.112.1.
 */

STATIC int
sccp_conn_ind(queue_t *q, struct sc *sc, mblk_t *cp)
{
	switch (sc->i_style) {
	case SCCP_STYLE_SCCPI:
	case SCCP_STYLE_NPI:
		return n_conn_ind(q, sc, cp);
	case SCCP_STYLE_TPI:
		return t_conn_ind(q, sc, cp);
	case SCCP_STYLE_GTT:
	case SCCP_STYLE_MGMT:
	default:
		swerr();
		return (-EFAULT);
	}
}

STATIC int
sccp_conn_con(queue_t *q, struct sc *sc, ulong pcl, ulong flags, struct sccp_addr *res, mblk_t *dp)
{
	switch (sc->i_style) {
	case SCCP_STYLE_SCCPI:
	case SCCP_STYLE_NPI:
		return n_conn_con(q, sc, pcl, flags, res, dp);
	case SCCP_STYLE_TPI:
		return t_conn_con(q, sc, pcl, flags, res, dp);
	case SCCP_STYLE_GTT:
	case SCCP_STYLE_MGMT:
	default:
		swerr();
		return (-EFAULT);
	}
}

STATIC int
sccp_data_ind(queue_t *q, struct sc *sc, ulong exp, ulong more, mblk_t *dp)
{
	switch (sc->i_style) {
	case SCCP_STYLE_SCCPI:
	case SCCP_STYLE_NPI:
		if (exp)
			return n_exdata_ind(q, sc, more, dp);
		else
			return n_data_ind(q, sc, more, dp);
	case SCCP_STYLE_TPI:
		if (exp)
			return t_exdata_ind(q, sc, more, dp);
		else
			return t_data_ind(q, sc, more, dp);
	case SCCP_STYLE_GTT:
	case SCCP_STYLE_MGMT:
	default:
		swerr();
		return (-EFAULT);
	}
}

STATIC int
sccp_datack_ind(queue_t *q, struct sc *sc)
{
	switch (sc->i_style) {
	case SCCP_STYLE_SCCPI:
	case SCCP_STYLE_NPI:
		return n_datack_ind(q, sc);
	case SCCP_STYLE_TPI:
		/* 
		   could possibly use t_opdata_ind here */
		return (-EOPNOTSUPP);
	case SCCP_STYLE_GTT:
	case SCCP_STYLE_MGMT:
	default:
		swerr();
		return (-EFAULT);
	}
}

STATIC int
sccp_reset_ind(queue_t *q, struct sc *sc, ulong orig, ulong reason, mblk_t *cp)
{
	switch (sc->i_style) {
	case SCCP_STYLE_SCCPI:
	case SCCP_STYLE_NPI:
		return n_reset_ind(q, sc, orig, reason, cp);
	case SCCP_STYLE_TPI:
		/* 
		   could possibly use t_opdata_ind here */
		return (-EOPNOTSUPP);
	case SCCP_STYLE_GTT:
	case SCCP_STYLE_MGMT:
	default:
		swerr();
		return (-EFAULT);
	}
}

STATIC int
sccp_reset_con(queue_t *q, struct sc *sc)
{
	switch (sc->i_style) {
	case SCCP_STYLE_SCCPI:
	case SCCP_STYLE_NPI:
		/* 
		   could possibly use t_opdata_ind here */
		return n_reset_con(q, sc);
	case SCCP_STYLE_TPI:
		return (-EOPNOTSUPP);
	case SCCP_STYLE_GTT:
	case SCCP_STYLE_MGMT:
	default:
		swerr();
		return (-EFAULT);
	}
}

STATIC int
sccp_discon_ind(queue_t *q, struct sc *sc, ulong orig, long reason, struct sccp_addr *res,
		mblk_t *cp, mblk_t *dp)
{
	switch (sc->i_style) {
	case SCCP_STYLE_SCCPI:
	case SCCP_STYLE_NPI:
		return n_discon_ind(q, sc, orig, reason, res, cp, dp);
	case SCCP_STYLE_TPI:
		return t_discon_ind(q, sc, reason, cp, dp);
	case SCCP_STYLE_GTT:
	case SCCP_STYLE_MGMT:
	default:
		swerr();
		return (-EFAULT);
	}
}

#if 0
STATIC int
sccp_ordrel_ind(queue_t *q, struct sc *sc)
{
	switch (sc->i_style) {
	case SCCP_STYLE_SCCPI:
	case SCCP_STYLE_NPI:
		return n_ordrel_ind(q, sc);
	case SCCP_STYLE_TPI:
		return t_ordrel_ind(q, sc);
	case SCCP_STYLE_GTT:
	case SCCP_STYLE_MGMT:
	default:
		swerr();
		return (-EFAULT);
	}
}
#endif

STATIC int
sccp_unitdata_ind(queue_t *q, struct sc *sc, struct sccp_addr *dst, struct sccp_addr *src,
		  ulong pcl, ulong opt, ulong imp, ulong seq, ulong pri, mblk_t *dp)
{
	switch (sc->i_style) {
	case SCCP_STYLE_SCCPI:
	case SCCP_STYLE_NPI:
		return n_unitdata_ind(q, sc, src, dst, dp);
	case SCCP_STYLE_TPI:
		return t_unitdata_ind(q, sc, src, &seq, &pri, &pcl, &imp, &opt, dp);
	case SCCP_STYLE_GTT:
	case SCCP_STYLE_MGMT:
	default:
		swerr();
		return (-EFAULT);
	}
}

STATIC int
sccp_notice_ind(queue_t *q, struct sc *sc, ulong cause, struct sccp_addr *dst,
		struct sccp_addr *src, ulong pcl, ulong ret, ulong imp, ulong seq, ulong pri,
		mblk_t *dp)
{
	switch (sc->i_style) {
	case SCCP_STYLE_SCCPI:
		return n_notice_ind(q, sc, cause, dst, src, pri, seq, pcl, imp, dp);
	case SCCP_STYLE_NPI:
		return n_uderror_ind(q, sc, dst, dp, cause);
	case SCCP_STYLE_TPI:
		return t_uderror_ind(q, sc, cause, dst, &seq, &pri, &pcl, &imp, &ret, dp);
	case SCCP_STYLE_GTT:
	case SCCP_STYLE_MGMT:
	default:
		swerr();
		return (-EFAULT);
	}
}

#if 0
STATIC int
sccp_inform_ind(queue_t *q, struct sc *sc, N_qos_sel_infr_sccp_t * qos, ulong reason)
{
	switch (sc->i_style) {
	case SCCP_STYLE_SCCPI:
		return n_inform_ind(q, sc, qos, reason);
	case SCCP_STYLE_NPI:
		/* 
		   XXX: could possibly use n_reset_ind */
	case SCCP_STYLE_TPI:
		/* 
		   XXX: could possible use t_optdata_ind with no data */
	case SCCP_STYLE_GTT:
	case SCCP_STYLE_MGMT:
	default:
		swerr();
		return (-EFAULT);
	}
}

STATIC int
sccp_coord_ind(queue_t *q, struct sc *sc, struct sccp_addr *add)
{
	switch (sc->i_style) {
	case SCCP_STYLE_SCCPI:
	case SCCP_STYLE_MGMT:
	case SCCP_STYLE_GTT:
		return n_coord_ind(q, sc, add);
	case SCCP_STYLE_NPI:
	case SCCP_STYLE_TPI:
		swerr();
		return (-EOPNOTSUPP);
	default:
		swerr();
		return (-EFAULT);
	}
}

STATIC int
sccp_coord_con(queue_t *q, struct sc *sc, struct sccp_addr *add, ulong smi)
{
	switch (sc->i_style) {
	case SCCP_STYLE_SCCPI:
	case SCCP_STYLE_GTT:
	case SCCP_STYLE_MGMT:
		return n_coord_con(q, sc, add, smi);
	case SCCP_STYLE_NPI:
	case SCCP_STYLE_TPI:
		swerr();
		return (-EOPNOTSUPP);
	default:
		swerr();
		return (-EFAULT);
	}
}
#endif

STATIC int
sccp_state_ind(queue_t *q, struct sc *sc, struct sccp_addr *add, ulong status, ulong smi)
{
	switch (sc->i_style) {
	case SCCP_STYLE_SCCPI:
	case SCCP_STYLE_GTT:
	case SCCP_STYLE_MGMT:
		return n_state_ind(q, sc, add, status, smi);
	case SCCP_STYLE_NPI:
		/* 
		   could possibly use n_uderror_ind or n_reset_ind */
	case SCCP_STYLE_TPI:
		/* 
		   could possibly use t_uderror_ind or t_optdata_ind */
		swerr();
		return (-EOPNOTSUPP);
	default:
		swerr();
		return (-EFAULT);
	}
}

/*
   local broadcast of N-STATE indication 
 */
STATIC int
sccp_state_lbr(queue_t *q, struct sccp_addr *add, ulong status, ulong smi)
{
	int err;
	struct na *na;
	struct sp *sp;
	struct ss *ss;
	struct sc *sc;
	if (!(na = na_lookup(add->ni)))
		goto done;
	for (sp = na->sp.list; sp; sp = sp->na.next) {
		for (sc = sp->gt.list; sc; sc = sc->sp.next) {
			if ((err = sccp_state_ind(q, sc, add, status, smi)) < 0)
				goto error;
		}
		for (ss = sp->ss.list; ss; ss = ss->sp.next) {
			for (sc = ss->cl.list; sc; sc = sc->ss.next) {
				if ((err = sccp_state_ind(q, sc, add, status, smi)) < 0)
					goto error;
			}
			for (sc = ss->cr.list; sc; sc = sc->ss.next) {
				if ((err = sccp_state_ind(q, sc, add, status, smi)) < 0)
					goto error;
			}
			for (sc = ss->co.list; sc; sc = sc->ss.next) {
				if ((err = sccp_state_ind(q, sc, add, status, smi)) < 0)
					goto error;
			}
		}
	}
      done:
	return (QR_DONE);
      error:
	return (err);
}

STATIC int
sccp_pcstate_ind(queue_t *q, struct sc *sc, struct mtp_addr *add, ulong status)
{
	switch (sc->i_style) {
	case SCCP_STYLE_SCCPI:
	case SCCP_STYLE_GTT:
	case SCCP_STYLE_MGMT:
		return n_pcstate_ind(q, sc, add, status);
	case SCCP_STYLE_NPI:
		/* 
		   could possibly use n_uderror_ind or n_reset_ind */
	case SCCP_STYLE_TPI:
		/* 
		   could possibly use t_uderror_ind or t_optdata_ind */
		swerr();
		return (-EOPNOTSUPP);
	default:
		swerr();
		return (-EFAULT);
	}
}

/*
   local broadcast of N-PCSTATE indication 
 */
STATIC int
sccp_pcstate_lbr(queue_t *q, struct mtp_addr *add, ulong status)
{
	int err;
	struct na *na;
	struct sp *sp;
	struct ss *ss;
	struct sc *sc;
	if (!(na = na_lookup(add->ni)))
		goto done;
	for (sp = na->sp.list; sp; sp = sp->na.next) {
		for (sc = sp->gt.list; sc; sc = sc->sp.next) {
			if ((err = sccp_pcstate_ind(q, sc, add, status)) < 0)
				goto error;
		}
		for (ss = sp->ss.list; ss; ss = ss->sp.next) {
			for (sc = ss->cl.list; sc; sc = sc->ss.next) {
				if ((err = sccp_pcstate_ind(q, sc, add, status)) < 0)
					goto error;
			}
			for (sc = ss->cr.list; sc; sc = sc->ss.next) {
				if ((err = sccp_pcstate_ind(q, sc, add, status)) < 0)
					goto error;
			}
			for (sc = ss->co.list; sc; sc = sc->ss.next) {
				if ((err = sccp_pcstate_ind(q, sc, add, status)) < 0)
					goto error;
			}
		}
	}
      done:
	return (QR_DONE);
      error:
	return (err);
}

#if 0
STATIC int
sccp_traffic_ind(queue_t *q, struct sc *sc, struct sccp_addr *add, ulong tmix)
{
	switch (sc->i_style) {
	case SCCP_STYLE_SCCPI:
	case SCCP_STYLE_GTT:
	case SCCP_STYLE_MGMT:
		return n_traffic_ind(q, sc, add, tmix);
	case SCCP_STYLE_NPI:
		/* 
		   could possibly use n_uderror_ind or n_reset_ind */
	case SCCP_STYLE_TPI:
		/* 
		   could possibly use t_uderror_ind or t_optdata_ind */
		swerr();
		return (-EOPNOTSUPP);
	default:
		swerr();
		return (-EFAULT);
	}
}
#endif

STATIC INLINE void
sccp_release(struct sc *sc)
{
	psw_t flags;
	sccp_timer_stop(sc, tcon);
	sccp_timer_stop(sc, tias);
	sccp_timer_stop(sc, tiar);
	sccp_timer_stop(sc, trel);
	sccp_timer_stop(sc, tint);
	sccp_timer_stop(sc, trel2);
	spin_lock_irqsave(&master.lock, flags);
	{
		if (sc->sp.sp) {
			fixme(("Freeze local reference\n"));
			// sc->slr = 0; /* Oooops need this for released message */ 
			if ((*(sc->sp.prev) = sc->sp.next))
				sc->sp.next->sp.prev = sc->sp.prev;
			sc->sp.next = NULL;
			sc->sp.prev = &sc->sp.next;
			sp_put(xchg(&sc->sp.sp, NULL));
			sccp_put(sc);
		}
	}
	spin_unlock_irqrestore(&master.lock, flags);
	return;
}

STATIC INLINE int
sccp_release_error(queue_t *q, struct sc *sc)
{
	int err;
	/* 
	   Figure C.2/Q.714 Sheet 2 of 7: Figure 2A/T1.112.4-2000 Sheet 2 of 4: stop connection
	   timer, release resources and local reference, freeze local reference, send N_DISCON_IND
	   to user, and move to the idle state. */
	sccp_release(sc);
	if ((err =
	     sccp_discon_ind(q, sc, N_PROVIDER, SCCP_RELC_REMOTE_PROCEDURE_ERROR, NULL, NULL,
			     NULL)))
		return (err);
	sccp_set_state(sc, SS_IDLE);
	return (QR_DONE);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SCCP Receive Message Functions
 *
 *  -------------------------------------------------------------------------
 *  These messages are received at the upper queue to which they belong: they have already been routed to the
 *  appropriate upper queue by the lower queue discrimination functions and have been decoded and checked for the
 *  existence of mandatory parameters.
 */
/*
 *  SCCP_MT_CR    0x01 - Connection Request
 *  -------------------------------------------------------------------------
 *  ITUT: F(SLR, PCLS), V(CDPA), O(CRED, CGPA, DATA, HOPC, IMP, EOP)
 *  ASNI: F(SLR, PCLS), V(CDPA), O(CRED, CGPA, DATA, HOPC, EOP)
 */
STATIC int
sccp_recv_cr(queue_t *q, struct sc *sc, mblk_t *msg, struct sccp_msg *m)
{
	/* 
	   CRED is only used with protocol class 3, DATA is optional, HOPC is only used at a relay
	   node, CGPA is optional, the OPC of the message is used if the CGPA is absent, IMP is
	   used for congestion control */
	int err;
	switch (sccp_get_state(sc)) {
	case SS_IDLE:
	case SS_WRES_CIND:
		/* 
		   Notes: we defer all of this stuff until we get an N_CONN_RES from the listening
		   stream.  Then we associate all of this stuff with the connection section.  This
		   might not be completely correct.  SCCP only sends one CR and then assigns a new
		   reference to the next connection.  We could actually use the local reference as
		   the SEQUENCE_number in the N_CONN_IND. */
		/* 
		   Figure 2B/T1.112.4-2000 1of2: if resources are not available, send a CREF;
		   otherwise, assign a local reference and SLS to incoming connection section and
		   determine protocol class and credit, associate originating node of CR with the
		   incoming connection section, send an N_CONN_IND to the user and move to the
		   connection pending state. */
		/* 
		   Figure C.3/Q.714 Sheet 1 of 6: if resources are not available, send a CREF;
		   otherwise, associate the remote reference to the connection, determine protocol
		   class and credit, send user an N_CONN_IND, move to the connection pending state. 
		 */
		/* 
		   FIXME: should send pcl and src address in options */
		if ((err = sccp_conn_ind(q, sc, msg)))
			return (err);
		sccp_set_state(sc, SS_WRES_CIND);
		return (QR_ABSORBED);
	case SS_WCON_CREQ2:
		/* 
		   Note that ITUT96 does not use SLR in the CR to send a RLSD message as does ANSI. 
		 */
		switch (sc->proto.pvar & SS7_PVAR_MASK) {
		case SS7_PVAR_ITUT:
		case SS7_PVAR_ETSI:
			/* 
			   Figure C.2/Q.714 Sheet 3 of 7: stop connection timer, release resources
			   and local reference, freeze local reference, move to the idle state. */
			return sccp_release_error(q, sc);
		default:
		case SS7_PVAR_JTTC:
		case SS7_PVAR_ANSI:
			/* 
			   Figure 2A/T1.112.4-2000 Sheet 3 of 4: same action as for CC in this
			   state: stop connection timer, associate destination reference to
			   connection section, release resources, stop inactivity timers, send
			   RLSD, start released (ANSI: and interval timers), move to the disconnect 
			   pending state. */
			sccp_release(sc);
			sc->dlr = m->slr;
			if ((err =
			     sccp_send_rlsd(q, sc->sp.sp, sc->sp.sp->add.pc, sc->sls, sc->dlr,
					    sc->slr, SCCP_RELC_END_USER_ORIGINATED, NULL, NULL)))
				return (err);
			sccp_timer_start(sc, trel);
			sccp_timer_start(sc, tint);	/* ANSI */
			sccp_set_state(sc, SS_WCON_DREQ);
			return (QR_DONE);
		}
	default:
		swerr();
		return (-EFAULT);
	}
}

/*
 *  SCCP_MT_CC    0x02 - Connection Confirm
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR, SLR, PCLS), O(CRED, CDPA, DATA, IMP, EOP)
 *  ANSI: F(DLR, SLR, PCLS), O(CRED, CDPA, DATA, EOP)
 */
STATIC int
sccp_recv_cc(queue_t *q, struct sc *sc, mblk_t *msg, struct sccp_msg *m)
{
	/* 
	   CRED is only used with protocol class 3, DATA is optional, CDPA is optional, the OPC of
	   the message is used in subseqent messages if CDPA is absent, IMP is used for congestion
	   control.  DLR is used to look up the stream in the NS_WCON_REQ state.  SLR is saved.
	   PCLS is passed to user.  This should result in an N_CONN_CON primitive to the SCCP-User. 
	 */
	int err;
	// mblk_t *mp;
	struct sccp_addr *res = m->parms & SCCP_PTF_CDPA ? &m->cdpa : NULL;
	ulong flags = m->pcl == 3 ? REC_CONF_OPT | EX_DATA_OPT : 0;
	switch (sccp_get_state(sc)) {
	case SS_WCON_CREQ:
		/* 
		   Figure C.2/Q.714 Sheet 2 of 7: Figure 2A/T1.112.4-2000 Sheet 2 of 4: stop
		   connection timer, start inactivity timers, assign protocol class and credit,
		   associate remote reference to connection section, (ANSI: associate originating
		   node of CC with connection section), send N_CONN_CON to user, and enter the data 
		   transfer state. */
		sccp_timer_stop(sc, tcon);
		if ((err = sccp_conn_con(q, sc, m->pcl, flags, res, msg->b_cont->b_cont)))
			return (err);
		sccp_timer_start(sc, tias);
		sccp_timer_start(sc, tiar);
		if ((sc->pcl = m->pcl) == 3)
			sc->p_cred = m->cred;
		sc->dlr = m->slr;
		sc->sp.sp->add.pc = m->rl.opc;	/* ANSI */
		sccp_set_state(sc, SS_DATA_XFER);
		return (QR_DONE);
	case SS_WCON_CREQ2:
		/* 
		   Figure C.2/Q.714 Sheet 3 of 7: Figure 2A/T1.112.4-2000 Sheet 3 of 4: Stop
		   connection timer, associate destination reference to connection section, release 
		   resources, stop inactivity timers, send RLSD, start released (ANSI: and interval 
		   timers), move to the disconnect pending state. */
		sccp_release(sc);
		sc->dlr = m->slr;
		switch (sc->proto.pvar & SS7_PVAR_MASK) {
		case SS7_PVAR_ANSI:
			if ((err =
			     sccp_send_rlsd(q, sc->sp.sp, sc->sp.sp->add.pc, sc->sls, sc->dlr,
					    sc->slr, SCCP_RELC_END_USER_ORIGINATED, NULL, NULL)))
				return (err);
			break;
		default:
		case SS7_PVAR_ITUT:
			if ((err =
			     sccp_send_rlsd(q, sc->sp.sp, sc->sp.sp->add.pc, sc->sls, sc->dlr,
					    sc->slr, SCCP_RELC_END_USER_ORIGINATED, NULL,
					    &sc->imp)))
				return (err);
			break;
		}
		sccp_timer_start(sc, trel);
		sccp_timer_start(sc, tint);	/* ANSI */
		sccp_set_state(sc, SS_WCON_DREQ);
		return (QR_DONE);
	case SS_WRES_RIND:
		/* 
		   Figure C.6/Q.714 Sheet 4 of 4: discard received message and generate internal
		   reset request, move to the active state. */
		fixme(("Internal reset request\n"));
		sccp_set_state(sc, SS_DATA_XFER);
		return (QR_DONE);
	case SS_WRES_DIND:
		goto error1;
	      error1:
		/* 
		   send RLSD */
		sccp_set_state(sc, SS_WCON_DREQ);
		return (QR_DONE);
	case SS_IDLE:
		/* 
		   Figure 2A/T1.112.4-2000 1of4: Figure 2B/T1.112.4-2000 1of2: send ERR. */
		if ((err =
		     sccp_send_err(q, sc->sp.sp, sc->sp.sp->add.pc, sc->sls, sc->dlr,
				   SCCP_ERRC_UNQUALIFIED)))
			return (err);
		swerr();	/* Note: this shouldn't happen */
		return (QR_DONE);
	case SS_DATA_XFER:
		switch (sc->proto.pvar & SS7_PVAR_MASK) {
		case SS7_PVAR_ANSI:
			/* 
			   Figure 2A/T1.112.4-2000: Figure 2B/T1.112.4-2000 2of2: send an
			   N_DISCON_IND to the user, release resources for the connection, stop
			   inactivity timers, send RLSD, start released and interval timers, move
			   to the disconnect pending state. */
			sccp_release(sc);
			if ((err =
			     sccp_send_rlsd(q, sc->sp.sp, sc->sp.sp->add.pc, sc->sls, sc->dlr,
					    sc->slr, SCCP_RELC_REMOTE_PROCEDURE_ERROR, NULL, NULL)))
				return (err);
			{
				int cause = 0;
				fixme(("Determine cuase\n"));
				if ((err =
				     sccp_discon_ind(q, sc, N_PROVIDER, cause, NULL, NULL, NULL)))
					return (err);
			}
			sccp_timer_start(sc, trel);
			sccp_timer_start(sc, tint);
			sccp_set_state(sc, SS_WCON_DREQ);
			return (QR_DONE);
		default:
			/* 
			   Figure C.3/Q.714 Sheet 3 of 6: discard */
			return (QR_DONE);
		}
	case SS_BOTH_RESET:	/* Figure C.6/Q.714 Sheet 3 of 4 */
	case SS_WCON_RREQ:	/* Figure C.6/Q.714 Sheet 2 of 4 */
	case SS_WCON_DREQ:	/* Figure C.2/Q.714 Sheet 6 of 7 */
		return (QR_DONE);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  SCCP_MT_CREF  0x03 - Connection Refused
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR, REFC), O(CDPA, DATA, IMP, EOP)
 *  ANSI: F(DLR, REFC), O(CDPA, DATA, EOP)
 */
STATIC int
sccp_recv_cref(queue_t *q, struct sc *sc, mblk_t *msg, struct sccp_msg *m)
{
	/* 
	   CDPA is completely optional and only identifies the ultimate refuser, the OPC of the
	   message is used if the CDPA is absent, IMP is used for congestion control.  DLR is used
	   to look up the stream in the NS_WCON_CREQ state.  This should result in a N_DISCON_IND
	   primitive to the SCCP-User. */
	int err;
	ulong cause = 0x5000 | m->cause;
	struct sccp_addr *res = m->parms & SCCP_PTF_CDPA ? &m->cdpa : NULL;
	switch (sccp_get_state(sc)) {
	case SS_WCON_CREQ:
		/* 
		   Figure C.2/Q.714 Sheet 2 of 7: Figure 2A/T1.112.4-2000 Sheet 2 of 4: stop the
		   connection timer, release resources and local reference, freeze local reference, 
		   and send N_DISCON_IND to the user.  Move to the idle state. */
		sccp_release(sc);
		if ((err = sccp_discon_ind(q, sc, N_USER, cause, res, NULL, msg->b_cont)))
			return (err);
		sccp_set_state(sc, SS_IDLE);
		return (QR_DONE);
	case SS_WCON_CREQ2:
		/* 
		   Figure C.2/Q.714 Sheet 3 of 7: Figure 2A/T1.112.4-2000 Sheet 3 of 4: Stop
		   connection timer, release resources and references, move to the idle state. */
		sccp_release(sc);
		sccp_set_state(sc, SS_IDLE);
		return (QR_DONE);
	case SS_WRES_RIND:
		/* 
		   Figure C.6/Q.714 Sheet 4 of 4: discard received message and generate internal
		   reset request, move to the active state. */
		fixme(("Internal reset request\n"));
		sccp_set_state(sc, SS_DATA_XFER);
		return (QR_DONE);
	case SS_WRES_DIND:
		goto error1;
	      error1:
		/* 
		   send RLSD */
		sccp_set_state(sc, SS_WCON_DREQ);
		return (QR_DONE);
	case SS_DATA_XFER:
		switch (sc->proto.pvar & SS7_PVAR_MASK) {
		case SS7_PVAR_ANSI:
			/* 
			   Figure 2A/T1.112.4-2000: Figure 2B/T1.112.4-2000: send an N_DISCON_IND
			   to the user, release resources for the connection, stop inactivity
			   timers, send RLSD, start released and interval timers, move to the
			   disconnect pending state. */
			sccp_release(sc);
			if ((err =
			     sccp_send_rlsd(q, sc->sp.sp, sc->sp.sp->add.pc, sc->sls, sc->dlr,
					    sc->slr, SCCP_RELC_END_USER_ORIGINATED, NULL, NULL)))
				return (err);
			{
				int cause = 0;
				fixme(("Determine cuase\n"));
				if ((err =
				     sccp_discon_ind(q, sc, N_PROVIDER, cause, NULL, NULL, NULL)))
					return (err);
			}
			sccp_timer_start(sc, trel);
			sccp_timer_start(sc, tint);
			sccp_set_state(sc, SS_WCON_DREQ);
			return (QR_DONE);
		default:
			/* 
			   Figure C.3/Q.714 Sheet 3 of 6: discard */
			return (QR_DONE);
		}
	case SS_BOTH_RESET:	/* Figure C.6/Q.714 Sheet 3 of 4 */
	case SS_WCON_RREQ:	/* Figure C.6/Q.714 Sheet 2 of 4 */
	case SS_WCON_DREQ:	/* Figure C.2/Q.714 Sheet 6 of 7 */
	case SS_IDLE:		/* Figure 2A/T1.112.4-2000 1of4: Figure 2B 1of2 */
		return (QR_DONE);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  SCCP_MT_RLSD  0x04 - Released
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR, SLR, RELC), O(DATA, IMP, EOP)
 *  ANSI: F(DLR, SLR, RELC), O(DATA, EOP)
 */
STATIC int
sccp_recv_rlsd(queue_t *q, struct sc *sc, mblk_t *msg, struct sccp_msg *m)
{
	/* 
	   DATA is optional, IMP is used for congestion control.  DLR is used to look up the stream 
	   in the NS_DATA_XFER state.  This should result in a N_DISCON_IND primitive to the
	   SCCP-User and a SCCP_MT_RLC message issued back to the peer. */
	int err;
	mblk_t *cp = NULL;
	struct sccp_addr *res = m->parms & SCCP_PTF_CDPA ? &m->cdpa : NULL;
	fixme(("Have to look up any connection indications and set cp\n"));
	switch (sccp_get_state(sc)) {
	case SS_IDLE:
		/* 
		   Figure 2A/T1.112.4-2000 1of4: Figure 2B/T1.112.4-2000 1of2: return a release
		   complete. */
		if ((err =
		     sccp_send_rlc(q, sc->sp.sp, sc->sp.sp->add.pc, sc->sls, sc->dlr, sc->slr)))
			return (err);
		/* 
		   Note: this case cannot happen.  We handle this case in the bottom end routing,
		   so this should not happen. */
		swerr();
		return (QR_DONE);
	case SS_WRES_CIND:
		/* 
		   Note: the ANSI procedures permit the side initiating the connection to release
		   the connection before receiving the Connection Confirm, which is weird as it
		   waits for the connection confirm before releasing under user control. */
		switch (sc->proto.pvar & SS7_PVAR_MASK) {
		case SS7_PVAR_ANSI:
			/* 
			   Figure 2B/T1.112.4-2000 1of2: send N_DISCON_IND to user, release
			   resources and reference for the connection section, stop inactivity
			   timers, send RLC, move to the idle state. */
			fixme(("Look up cc cp in connection list\n"));
			sccp_release(sc);
			if ((err =
			     sccp_send_rlc(q, sc->sp.sp, sc->sp.sp->add.pc, sc->sls, sc->dlr,
					   sc->slr)))
				return (err);
			if ((err =
			     sccp_discon_ind(q, sc, N_USER, 0x4000 | m->cause, res, cp,
					     msg->b_cont)))
				return (err);
			sccp_set_state(sc, SS_IDLE);
			return (QR_DONE);
		default:
		case SS7_PVAR_ITUT:
			/* 
			   Figure C.2/Q.714 2of6: discard received message. */
			return (QR_DONE);
		}
	case SS_DATA_XFER:
		/* 
		   Figure C.2/Q.714 Sheet 5 of 7: Figure C.3/Q.714 Sheet 4 of 6: Figure
		   2A/T1.112.4-2000 Sheet 4 of 4: Figure 2B/T1.112.4-2000 2of2: send N_DISCON_IND
		   to user, release resources and local reference, freeze local reference, stop
		   inactivity timers, send RLC, and move to the IDLE state. */
		sccp_release(sc);
		if ((err =
		     sccp_send_rlc(q, sc->sp.sp, sc->sp.sp->add.pc, sc->sls, sc->dlr, sc->slr)))
			return (err);
		if ((err = sccp_discon_ind(q, sc, N_USER, 0x1000 | m->cause, res, cp, msg->b_cont)))
			return (err);
		sccp_set_state(sc, SS_IDLE);
		return (QR_DONE);
	case SS_WCON_DREQ:
		/* 
		   Figure C.2/Q.714 Sheet 6 of 7: release resources and local reference, freeze
		   local reference, stop release and interval timers, move to the idle state.
		   Figure 2A/T1.112.4-2000: Figure 2B/T1.112.4-2000: Release references for the
		   connection section, stop release and interval timers, move to the idle state. */
		sccp_release(sc);
		sccp_set_state(sc, SS_IDLE);
		return (QR_DONE);
	case SS_WRES_RIND:
		/* 
		   Figure C.2/Q.714 Sheet 4 of 4: FIXME this is confusing. */
		fixme(("Figure this out\n"));
		return (-EFAULT);
	case SS_WCON_CREQ:
		/* 
		   Figure C.2/Q.714 Sheet 2 of 7: Figure 2A/T1.112.4-2000 Sheet 2 of 4: reply with
		   RLC, stop the connection timer, release resources and local reference, freeze
		   local reference, and send N_DISCON_IND to the user. */
		sccp_release(sc);
		if ((err =
		     sccp_send_rlc(q, sc->sp.sp, sc->sp.sp->add.pc, sc->sls, sc->dlr, sc->slr)))
			return (err);
		if ((err = sccp_discon_ind(q, sc, N_USER, 0x1000 | m->cause, res, cp, msg->b_cont)))
			return (err);
		sccp_set_state(sc, SS_IDLE);
		return (QR_DONE);
	case SS_WCON_CREQ2:
		/* 
		   Figure C.2/Q.714 Sheet 3 of 7: In this case we have already received an
		   N_DISCON_REQ from the user.  Send an RLC, stop the connection timer, release
		   resources and local reference, freeze local reference, and move to the idle
		   state. */
		sccp_release(sc);
		if ((err =
		     sccp_send_rlc(q, sc->sp.sp, sc->sp.sp->add.pc, sc->sls, sc->dlr, sc->slr)))
			return (err);
		sccp_set_state(sc, SS_IDLE);
		return (QR_DONE);
	case SS_BOTH_RESET:
		/* 
		   Figure C.6/Q.714 Sheet 3 of 4: same as below. */
	case SS_WCON_RREQ:
		/* 
		   Figure 2E/T1.112.4-2000 2of2: stop reset timer and go to Figure 2A/T1.112.4-2000 
		   4of4: send N_DISCON_IND to user, release resources and reference for the
		   connection section, stop inactivity timers, send RLC and move to the idle state. 
		   Same as below for ITU. */
		/* 
		   Figure C.6/Q.714 Sheet 2 of 4: Figure C.2/Q.714 Sheet 5 of 7: Send an
		   N_DISCON_IND to the user, release resource and local reference, freeze local
		   reference, stop inactivity timers, send a RLC, and move to the Idle state. */
		sccp_release(sc);
		if ((err =
		     sccp_send_rlc(q, sc->sp.sp, sc->sp.sp->add.pc, sc->sls, sc->dlr, sc->slr)))
			return (err);
		if ((err = sccp_discon_ind(q, sc, N_USER, 0x1000 | m->cause, res, cp, msg->b_cont)))
			return (err);
		sccp_set_state(sc, SS_IDLE);
		return (QR_DONE);
	case SS_WRES_DIND:
		return (QR_DONE);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  SCCP_MT_RLC   0x05 - Release Complete
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR, SLR)
 *  ANSI: F(DLR, SLR)
 */
STATIC int
sccp_recv_rlc(queue_t *q, struct sc *sc, mblk_t *msg, struct sccp_msg *m)
{
	/* 
	   No optional parameters.  Optional parameters will be ignored.  DLR is used to look up
	   the stream in the NS_WACK_DREQ or NS_IDLE state.  This should result in a N_OK_ACK
	   primitive to the SCCP-User if not already sent. */
	int err;
	// struct sccp_addr *res = m->parms & SCCP_PTF_CDPA ? &m->cdpa : NULL;
	switch (sccp_get_state(sc)) {
	case SS_WCON_DREQ:
		/* 
		   Figure C.2/Q.714 Sheet 6 of 7: release resources and local reference for
		   connection section, freeze local reference, stop release and interval timers,
		   move to idle state. Figure 2A/T1.112.4-2000: Figure 2B/T1.112.4-2000 2of2:
		   Release references for the connection section, stop release and interval timers, 
		   move to the idle state. */
		sccp_release(sc);
		sccp_set_state(sc, SS_IDLE);
		return (QR_DONE);
	case SS_WRES_RIND:
		/* 
		   Figure C.6/Q.714 Sheet 4 of 4: discard received message and generate internal
		   reset request, move to the active state. */
		fixme(("Internal reset request\n"));
		sccp_set_state(sc, SS_DATA_XFER);
		return (QR_DONE);
	case SS_WRES_DIND:
		goto error1;
	      error1:
		/* 
		   send RLSD */
		sccp_set_state(sc, SS_WCON_DREQ);
		return (QR_DONE);
	case SS_WCON_CREQ:
		return sccp_release_error(q, sc);
	case SS_WCON_CREQ2:
		/* 
		   Note that ITUT96 does not use the SLR in the RLC to send a RLSD message as does
		   ANSI. */
		switch (sc->proto.pvar & SS7_PVAR_MASK) {
		case SS7_PVAR_ITUT:
		case SS7_PVAR_ETSI:
			/* 
			   Figure C.2/Q.714 Sheet 3 of 7: stop connection timer, release resources
			   and local reference, freeze local reference, move to the idle state. */
			return sccp_release_error(q, sc);
		default:
		case SS7_PVAR_JTTC:
		case SS7_PVAR_ANSI:
			/* 
			   Figure 2A/T1.112.4-2000 Sheet 3 of 4: same action as for CC in this
			   state: stop connection timer, associate destination reference to
			   connection section, release resources, stop inactivity timers, send
			   RLSD, start released (ANSI: and interval timers), move to the disconnect 
			   pending state. */
			sccp_release(sc);
			sc->dlr = m->slr;
			if ((err =
			     sccp_send_rlsd(q, sc->sp.sp, sc->sp.sp->add.pc, sc->sls, sc->dlr,
					    sc->slr, SCCP_RELC_END_USER_ORIGINATED, NULL, NULL)))
				return (err);
			sccp_timer_start(sc, trel);
			sccp_timer_start(sc, tint);	/* ANSI */
			sccp_set_state(sc, SS_WCON_DREQ);
			return (QR_DONE);
		}
	case SS_DATA_XFER:
		switch (sc->proto.pvar & SS7_PVAR_MASK) {
		case SS7_PVAR_ANSI:
			/* 
			   Figure 2A/T1.112.4-2000: Figure 2B/T1.112.4-2000 2of2: send an
			   N_DISCON_IND to the user, release resources for the connection, stop
			   inactivity timers, send RLSD, start released and interval timers, move
			   to the disconnect pending state. */
			sccp_release(sc);
			if ((err =
			     sccp_send_rlsd(q, sc->sp.sp, sc->sp.sp->add.pc, sc->sls, sc->dlr,
					    sc->slr, SCCP_RELC_REMOTE_PROCEDURE_ERROR, NULL, NULL)))
				return (err);
			{
				int cause = 0;
				fixme(("Determine cuase\n"));
				if ((err =
				     sccp_discon_ind(q, sc, N_PROVIDER, cause, NULL, NULL, NULL)))
					return (err);
			}
			sccp_timer_start(sc, trel);
			sccp_timer_start(sc, tint);
			sccp_set_state(sc, SS_WCON_DREQ);
			return (QR_DONE);
		default:
			/* 
			   Figure C.3/Q.714 Sheet 3 of 6: discard */
			return (QR_DONE);
		}
	case SS_BOTH_RESET:	/* Figure C.6/Q.714 Sheet 3 of 4 */
	case SS_WCON_RREQ:	/* Figure C.6/Q.714 Sheet 2 of 4 */
	case SS_IDLE:		/* Figure 2A/T1.112.4-2000 1of4: Figure 2B 1of2 */
		return (QR_DONE);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  SCCP_MT_DT1   0x06 - Data Form 1
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR, SEG), V(DATA)
 *  ANSI: F(DLR, SEG), V(DATA)
 */
STATIC int
sccp_recv_dt1(queue_t *q, struct sc *sc, mblk_t *msg, struct sccp_msg *m)
{
	/* 
	   No optional parameters.  Optional parameters will be ignored. This message is only valid 
	   for protocol class 2.  DLR is used to look up the stream in the NS_DATA_XFER state.  SEG 
	   is passed as the more bit to the user.  This should result in an N_DATA_IND primitive to 
	   the SCCP-User. */
	int err;
	fixme(("Handle sequence numbers and credit\n"));
	switch (sccp_get_state(sc)) {
	case SS_DATA_XFER:
		/* 
		   Figure 2C/T1.112.4-2000 1of2: reset receive inactivity timer.  For protocol
		   class 3 if P(R) is in the rage from the last P(R) received up to including the
		   send sequence number of the next message to be transmitted, if P(R) is lower
		   than the window edge, update the lower window edge and perform the transmission
		   wakeup function.  Otherwise, if P(S) is the next in sequence, mark ack
		   outstanding and deliver to the user.  If P(S) is not next in sequence do an
		   internal reset. For protocol class 2 deliver data to the user. */
		/* 
		   Figure C.4/Q.714 Sheet 2 of 4: When we receive a DT1 we restart the receive
		   inactivity timer. If the M-bit is set, we can either reassemble before
		   delivering to the user, or we can deliver immediately using the more flag in the 
		   N_DATA_IND.  In either case, once we have a deliverable message we generate an
		   N_DATA_IND to the user. */
		sccp_timer_start(sc, tiar);
		if ((err = sccp_data_ind(q, sc, 0, m->seg, msg->b_cont)))
			return (err);
		return (QR_DONE);
	case SS_WRES_RIND:
		switch (sc->proto.pvar & SS7_PVAR_MASK) {
		case SS7_PVAR_ANSI:
			/* 
			   Figure 2E/T1.112.4-2000 2of2: queue received message. */
			bufq_queue(&sc->rcvq, msg);
			return (QR_DONE);
		default:
		case SS7_PVAR_ITUT:
			/* 
			   Figure C.6/Q.714 Sheet 4 of 4: discard received message and generate
			   internal reset request, move to the active state. */
			fixme(("Internal reset request\n"));
			sccp_set_state(sc, SS_DATA_XFER);
			return (QR_DONE);
		}
	case SS_WRES_DIND:
		goto error1;
	      error1:
		/* 
		   send RLSD */
		sccp_set_state(sc, SS_WCON_DREQ);
		return (QR_DONE);
	case SS_WCON_CREQ:
		return sccp_release_error(q, sc);
	case SS_WCON_CREQ2:
		switch (sc->proto.pvar & SS7_PVAR_MASK) {
		case SS7_PVAR_ITUT:
		case SS7_PVAR_ETSI:
			return sccp_release_error(q, sc);
		default:
		case SS7_PVAR_JTTC:
		case SS7_PVAR_ANSI:
			/* 
			   Figure 2A/T1.112.4-2000 Sheet 3 of 4: discard, continue to wait for
			   connection confirm. */
			return (QR_DONE);
		}
	case SS_BOTH_RESET:	/* Figure C.6/Q.714 Sheet 3 of 4 */
		/* 
		   Figure 2E/T1.112.4-2000 Sheet 1 of 2 */
	case SS_WCON_RREQ:	/* Figure C.6/Q.714 Sheet 2 of 4 */
		/* 
		   Figure 2E/T1.112.4-2000 2of2 */
	case SS_WCON_DREQ:	/* Figure C.2/Q.714 Sheet 6 of 7 */
		return (QR_DONE);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  SCCP_MT_DT2   0x07 - Data Form 2
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR, SEQ), V(DATA)
 *  ANSI: F(DLR, SEQ), V(DATA)
 */
STATIC int
sccp_recv_dt2(queue_t *q, struct sc *sc, mblk_t *msg, struct sccp_msg *m)
{
	/* 
	   No optional parameters.  Optional parameters will be ignored. This message is only valid 
	   for protocol class 3.  DLR is used to look up the stream in the NS_DATA_XFER state.  SEQ 
	   is used for sequencing and the more bit is passed to the user.  This should result in a
	   N_DATA_IND primitive to the SCCP-User. */
	int err;
	fixme(("Handle sequence numbers and credit\n"));
	switch (sccp_get_state(sc)) {
	case SS_DATA_XFER:
		/* 
		   Figure 2C/T1.112.4-2000 1of2: reset receive inactivity timer.  For protocol
		   class 3 if P(R) is in the rage from the last P(R) received up to including the
		   send sequence number of the next message to be transmitted, if P(R) is lower
		   than the window edge, update the lower window edge and perform the transmission
		   wakeup function.  Otherwise, if P(S) is the next in sequence, mark ack
		   outstanding and deliver to the user.  If P(S) is not next in sequence do an
		   internal reset. For protocol class 2 deliver data to the user. */
		/* 
		   Figure C.4/Q.714 Sheet 2 of 4: If the received P(S) is not the next in sequence, 
		   or P(R) is outside the range from the last received P(R) to the last P(S) sent
		   plus 1, the an internal reset (see figure) is peformed.  Otherwise, perform a
		   Transmit Wakeup: ------------------------------------- If the received P(R) is
		   at the lower edge of the sending window, the sending window is set to the value
		   of P(R).  While we have a message in the transmit queue, (we are flow controlled
		   on the sending side) and the P(S) assigned to the message in queue is inside the
		   send window, we set P(R) to the last received P(S) plus 1 and send another of the
		   queued data messages and restart the send inactivity timer.  In this way we clear
		   any backlog of messages once we receive acknowledgement that opens the window.
		   Otherwise, if the received P(S) is equal to the upper edge of the window, or if we 
		   need to send a data ack (acknowledged service) then we construct a data ack.
		   message with credit = W and send it, restarting the send inactivity timer.  After
		   this operation, we deliver the data to the user in an N_DATA_IND.  IMPLEMENTATION
		   NOTE:- We allow the user to explicitly acknowledge receipt of data messages in
		   protocol class 3.  To do this, we queue the message to an ack queue rather than
		   performing the P(S) P(R) processing on it yet.  We postpone this processing until
		   a data ack request is received from the user. */
		if ((err = sccp_data_ind(q, sc, 0, m->seg, msg->b_cont)))
			return (err);
		return (QR_DONE);
	case SS_WRES_RIND:
		switch (sc->proto.pvar & SS7_PVAR_MASK) {
		case SS7_PVAR_ANSI:
			/* 
			   Figure 2E/T1.112.4-2000 2of2: queue received message. */
			bufq_queue(&sc->rcvq, msg);
			return (QR_DONE);
		default:
		case SS7_PVAR_ITUT:
			/* 
			   Figure C.6/Q.714 Sheet 4 of 4: discard received message and generate
			   internal reset request, move to the active state. */
			fixme(("Internal reset request\n"));
			sccp_set_state(sc, SS_DATA_XFER);
			return (QR_DONE);
		}
	case SS_WRES_DIND:
		goto error1;
	      error1:
		/* 
		   send RLSD */
		sccp_set_state(sc, SS_WCON_DREQ);
		return (QR_DONE);
	case SS_WCON_CREQ:
		return sccp_release_error(q, sc);
	case SS_WCON_CREQ2:
		switch (sc->proto.pvar & SS7_PVAR_MASK) {
		case SS7_PVAR_ITUT:
		case SS7_PVAR_ETSI:
			return sccp_release_error(q, sc);
		default:
		case SS7_PVAR_JTTC:
		case SS7_PVAR_ANSI:
			/* 
			   Figure 2A/T1.112.4-2000 Sheet 3 of 4: discard, continue to wait for
			   connection confirm. */
			return (QR_DONE);
		}
	case SS_BOTH_RESET:	/* Figure C.6/Q.714 Sheet 3 of 4 */
		/* 
		   Figure 2E/T1.112.4-2000 Sheet 1 of 2 */
	case SS_WCON_RREQ:	/* Figure C.6/Q.714 Sheet 2 of 4 */
		/* 
		   Figure 2E/T1.112.4-2000 2of2 */
	case SS_WCON_DREQ:	/* Figure C.2/Q.714 Sheet 6 of 7 */
		return (QR_DONE);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  SCCP_MT_AK    0x08 - Data Acknowledgement
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR, RSN, CRED)
 *  ANSI: F(DLR, RSN, CRED)
 */
STATIC int
sccp_recv_ak(queue_t *q, struct sc *sc, mblk_t *msg, struct sccp_msg *m)
{
	/* 
	   No optional parameters.  Optional parameters will be ignored. This message is only valid 
	   for protocol class 3.  DLR is used to look up the stream in the NS_DATA_XFER state.  RSN 
	   is used to acknowledge a specific data segment and CRED is used to update the flow
	   control window.  If RC_OPT_SEL has been set for the stream, this should result in a
	   N_DATACK_IND to the SCCP-User. */
	int err;
	fixme(("Handle sequence numbers\n"));
	switch (sccp_get_state(sc)) {
	case SS_DATA_XFER:
		/* 
		   Figure 2C/T1.112.4-2000 1of2: reset receive inactivity timer, update sequence
		   number for next message to be sent and update credit.  While P(R)>lower window
		   edge and message in transmit queue: update window edge, construct message, send
		   data, mark no ack outstanding, reset send inactivity timer.  If P(S) is next in
		   sequence mark ack oustanding and deliver data to user, otherwise do an internal
		   reset. */
		/* 
		   Figure C.4/Q.714 Sheet 3 of 4: Restart the receive inactivity timer.  If the
		   received P(R) is not in the correct range, we perform an internal reset.
		   Otherwise, we set the sending cred to the value received in the data ack.
		   Perform a Transmit Wakeup: -------------------------- If the received P(R) is
		   not at the lower edge of the window, we set the lower edge of the sending window 
		   to the valur of the received P(R).  And, if there is a message in the transmit
		   queue and the P(S) of the message is inside the sending window, we set the P(R)
		   of the message to the last received P(S)+1 and send the message, restarting the
		   receive inactivity timer. IMPLEMENTATION NOTES:- Q.714 does not specify an
		   N-DATACK Indication; however, the ISO NSD as provided by NPI does support this
		   (TPI doesn't directly).  Therefore, we deliver a data acknowledgement indication
		   to the user if the interface supports this. */
		sccp_timer_start(sc, tiar);
		if ((err = sccp_datack_ind(q, sc)))
			return (err);
		return (QR_DONE);
	case SS_WRES_RIND:
		switch (sc->proto.pvar & SS7_PVAR_MASK) {
		case SS7_PVAR_ANSI:
			/* 
			   Figure 2E/T1.112.4-2000 2of2: queue received message. */
			bufq_queue(&sc->rcvq, msg);
			return (QR_DONE);
		default:
		case SS7_PVAR_ITUT:
			/* 
			   Figure C.6/Q.714 Sheet 4 of 4: discard received message and generate
			   internal reset request, move to the active state. */
			fixme(("Internal reset request\n"));
			sccp_set_state(sc, SS_DATA_XFER);
			return (QR_DONE);
		}
	case SS_WRES_DIND:
		goto error1;
	      error1:
		/* 
		   send RLSD */
		sccp_set_state(sc, SS_WCON_DREQ);
		return (QR_DONE);
	case SS_WCON_CREQ:
		return sccp_release_error(q, sc);
	case SS_WCON_CREQ2:
		switch (sc->proto.pvar & SS7_PVAR_MASK) {
		case SS7_PVAR_ITUT:
		case SS7_PVAR_ETSI:
			return sccp_release_error(q, sc);
		default:
		case SS7_PVAR_JTTC:
		case SS7_PVAR_ANSI:
			/* 
			   Figure 2A/T1.112.4-2000 Sheet 3 of 4: discard, continue to wait for
			   connection confirm. */
			return (QR_DONE);
		}
	case SS_BOTH_RESET:	/* Figure C.6/Q.714 Sheet 3 of 4 */
		/* 
		   Figure 2E/T1.112.4-2000 Sheet 1 of 2 */
	case SS_WCON_RREQ:	/* Figure C.6/Q.714 Sheet 2 of 4 */
		/* 
		   Figure 2E/T1.112.4-2000 2of2 */
	case SS_WCON_DREQ:	/* Figure C.2/Q.714 Sheet 6 of 7 */
		return (QR_DONE);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  SCCP_MT_ED    0x0b - Expedited Data
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR), V(DATA)
 *  ANSI: F(DLR), V(DATA)
 */
STATIC int
sccp_recv_ed(queue_t *q, struct sc *sc, mblk_t *msg, struct sccp_msg *m)
{
	/* 
	   No optional parameters.  Optional parameters will be ignored. This message is only valid 
	   in protocol class 3.  DLR is used to look up the stream in the NS_DATA_XFER state.  This 
	   should result in an N_EXDATA_IND to the SCCP-User. */
	int err;
	fixme(("Handle sequence numbers\n"));
	switch (sccp_get_state(sc)) {
	case SS_DATA_XFER:
		/* 
		   Figure 2D/T1.112.4-2000: this is shown a little differently (no internal reset
		   procedure), but we wait for ack from user if necessary.  Figure C.5/Q.714 Sheet
		   1 of 2: Restart the receive inactivity timer.  Deliever an N_DATA_IND for
		   expedited data to the user.  Wait for an acknowledgement from the user. */
		if (sc->flags & SCCPF_REM_ED) {
			/* 
			   Figure C.5/Q.714 Sheet 1 of 2: Perform an internal reset and move to the 
			   active state. */
			fixme(("Perform internal reset\n"));
			sc->flags &= ~SCCPF_REM_ED;
			return (-EPROTO);
		}
		if ((err = sccp_data_ind(q, sc, 1, 0, msg->b_cont)))
			return (err);
		sc->flags |= SCCPF_REM_ED;
		return (QR_DONE);
	case SS_WRES_RIND:
		switch (sc->proto.pvar & SS7_PVAR_MASK) {
		case SS7_PVAR_ANSI:
			/* 
			   Figure 2E/T1.112.4-2000 2of2: queue received message. */
			bufq_queue(&sc->rcvq, msg);
			return (QR_DONE);
		default:
		case SS7_PVAR_ITUT:
			/* 
			   Figure C.6/Q.714 Sheet 4 of 4: discard received message and generate
			   internal reset request, move to the active state. */
			fixme(("Internal reset request\n"));
			sccp_set_state(sc, SS_DATA_XFER);
			return (QR_DONE);
		}
	case SS_WRES_DIND:
		goto error1;
	      error1:
		/* 
		   send RLSD */
		sccp_set_state(sc, SS_WCON_DREQ);
		return (QR_DONE);
	case SS_WCON_CREQ:
		return sccp_release_error(q, sc);
	case SS_WCON_CREQ2:
		switch (sc->proto.pvar & SS7_PVAR_MASK) {
		case SS7_PVAR_ITUT:
		case SS7_PVAR_ETSI:
			return sccp_release_error(q, sc);
		default:
		case SS7_PVAR_JTTC:
		case SS7_PVAR_ANSI:
			/* 
			   Figure 2A/T1.112.4-2000 Sheet 3 of 4: discard, continue to wait for
			   connection confirm. */
			return (QR_DONE);
		}
	case SS_BOTH_RESET:	/* Figure C.6/Q.714 Sheet 3 of 4 */
		/* 
		   Figure 2E/T1.112.4-2000 Sheet 1 of 2 */
	case SS_WCON_RREQ:	/* Figure C.6/Q.714 Sheet 2 of 4 */
		/* 
		   Figure 2E/T1.112.4-2000 2of2 */
	case SS_WCON_DREQ:	/* Figure C.2/Q.714 Sheet 6 of 7 */
		return (QR_DONE);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  SCCP_MT_EA    0x0c - Expedited Data Acknowledgement
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR)
 *  ANSI: F(DLR)
 */
STATIC int
sccp_recv_ea(queue_t *q, struct sc *sc, mblk_t *msg, struct sccp_msg *m)
{
	/* 
	   No optional parameters.  Optional parameters will be ignored. This message is only valid 
	   in protocol class 3.  DLR is used to look up the stream in the NS_DATA_XFER state.  No
	   indication is given to the user.  It will be used internally in protocol class 3 to
	   unlock the normal data flow. */
	int err;
	fixme(("Handle sequence numbers\n"));
	switch (sccp_get_state(sc)) {
	case SS_DATA_XFER:
		/* 
		   Figure 2D/T1.112.4-2000: same.  Figure C.5/Q.714 Sheet 1 of 2: Restart the
		   receive inactivity timer.  If there is expedited data in queue, send another
		   expedited data now. IMPLEMENTATION NOTE:- We deliver expedited data
		   acknolwedgements to the User as well. */
		if (!(sc->flags & SCCPF_LOC_ED))
			return (QR_DONE);
		if ((err = sccp_datack_ind(q, sc)))
			return (err);
		sc->flags &= ~SCCPF_LOC_ED;
		return (QR_DONE);
	case SS_WRES_RIND:
		switch (sc->proto.pvar & SS7_PVAR_MASK) {
		case SS7_PVAR_ANSI:
			/* 
			   Figure 2E/T1.112.4-2000 2of2: queue received message. */
			bufq_queue(&sc->rcvq, msg);
			return (QR_DONE);
		default:
		case SS7_PVAR_ITUT:
			/* 
			   Figure C.6/Q.714 Sheet 4 of 4: discard received message and generate
			   internal reset request, move to the active state. */
			fixme(("Internal reset request\n"));
			sccp_set_state(sc, SS_DATA_XFER);
			return (QR_DONE);
		}
	case SS_WRES_DIND:
		goto error1;
	      error1:
		/* 
		   send RLSD */
		sccp_set_state(sc, SS_WCON_DREQ);
		return (QR_DONE);
	case SS_WCON_CREQ:
		return sccp_release_error(q, sc);
	case SS_WCON_CREQ2:
		switch (sc->proto.pvar & SS7_PVAR_MASK) {
		case SS7_PVAR_ITUT:
		case SS7_PVAR_ETSI:
			return sccp_release_error(q, sc);
		default:
		case SS7_PVAR_JTTC:
		case SS7_PVAR_ANSI:
			/* 
			   Figure 2A/T1.112.4-2000 Sheet 3 of 4: discard, continue to wait for
			   connection confirm. */
			return (QR_DONE);
		}
	case SS_BOTH_RESET:	/* Figure C.6/Q.714 Sheet 3 of 4 */
		/* 
		   Figure 2E/T1.112.4-2000 Sheet 1 of 2 */
	case SS_WCON_RREQ:	/* Figure C.6/Q.714 Sheet 2 of 4 */
		/* 
		   Figure 2E/T1.112.4-2000 2of2 */
	case SS_WCON_DREQ:	/* Figure C.2/Q.714 Sheet 6 of 7 */
		return (QR_DONE);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  SCCP_MT_RSR   0x0d - Reset Request
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR, SLR, RESC)
 *  ANSI: F(DLR, SLR, RESC)
 */
STATIC int
sccp_recv_rsr(queue_t *q, struct sc *sc, mblk_t *msg, struct sccp_msg *m)
{
	/* 
	   No optional parameters.  Optional parameters will be ignored. This message is only valid 
	   in protocol class 3.  DLR is used to look up the stream in the NS_DATA_XFER state.  This 
	   normally results in a N_RESET_IND primtiive to the SCCP-User.  The stream is reset. */
	int err;
	switch (sccp_get_state(sc)) {
	case SS_DATA_XFER:
		/* 
		   ANSI is peculiar in that it does not wait for a reset response from the user
		   before sending an RSC. */
		switch (sc->proto.pvar & SS7_PVAR_MASK) {
		case SS7_PVAR_ANSI:
			/* 
			   Figure 2E/T1.112.4-2000: send an RSC, send an N_RESET_IND to the user,
			   reset variables and discard all queued and unacknowledged messages, move 
			   to the reset incoming state. */
			if ((err =
			     sccp_send_rsc(q, sc->sp.sp, sc->sp.sp->add.pc, sc->sls, sc->dlr,
					   sc->slr)))
				return (err);
			fixme(("Reset variables and discard queued unacked messages\n"));
			if ((err = sccp_reset_ind(q, sc, N_USER, 0x3000 | m->cause, msg->b_cont)))
				return (err);
			return (QR_DONE);
			sccp_set_state(sc, SS_WRES_RIND);
		default:
		case SS7_PVAR_ITUT:
			/* 
			   Figure C.6/Q.714 Sheet 1 of 4: Restart the receive inactivity timer.
			   Deliver an N_RESET_IND to the user, reset variables and discard all
			   queued and unacked messages. Mover the reset incoming state. */
			sccp_timer_start(sc, tiar);
			fixme(("Reset variables and discard queued unacked messages\n"));
			if ((err = sccp_reset_ind(q, sc, N_USER, 0x3000 | m->cause, msg->b_cont)))
				return (err);
			sccp_set_state(sc, SS_WRES_RIND);
			return (QR_DONE);
		}
	case SS_BOTH_RESET:
		switch (sc->proto.pvar & SS7_PVAR_MASK) {
		case SS7_PVAR_ANSI:
			/* 
			   Figure 2E/T1.112.4-2000: stop reset timer and move to the reset incoming 
			   state. */
			sccp_timer_stop(sc, tres);
			sccp_set_state(sc, SS_WRES_RIND);
			return (QR_DONE);
		default:
		case SS7_PVAR_ITUT:
			/* 
			   Figure C.6/Q.714 Sheet 3 of 4: send a N_RESET_CON to the user, stop the
			   reset timer, restart the receive inactivity timer, move to the reset
			   incoming state. */
			sccp_timer_stop(sc, tres);
			sccp_timer_start(sc, tiar);
			if ((err = sccp_reset_con(q, sc)))
				return (err);
			sccp_set_state(sc, SS_WRES_RIND);
			return (QR_DONE);
		}
	case SS_WCON_RREQ:
		switch (sc->proto.pvar & SS7_PVAR_MASK) {
		case SS7_PVAR_ANSI:
			/* 
			   Figure 2E/T1.112.4-2000: stop reset timer, transfer all queued
			   information, move to the active state. */
			sccp_timer_stop(sc, tres);
			fixme(("Transfer all queued information\n"));
			sccp_set_state(sc, SS_DATA_XFER);
			return (QR_DONE);
		default:
		case SS7_PVAR_ITUT:
			/* 
			   Figure C.6/Q.714 Sheet 2 of 4: send N_RESET_CON, stop reset timer,
			   restart receive inactivity timer, move to the active state. */
			sccp_timer_stop(sc, tres);
			sccp_timer_start(sc, tiar);
			if ((err = sccp_reset_con(q, sc)))
				return (err);
			sccp_set_state(sc, SS_DATA_XFER);
			return (QR_DONE);
		}
	case SS_WRES_DIND:
		goto error1;
	      error1:
		/* 
		   send RLSD */
		sccp_set_state(sc, SS_WCON_DREQ);
		return (QR_DONE);
	case SS_WCON_CREQ:
		return sccp_release_error(q, sc);
	case SS_WCON_CREQ2:
		/* 
		   Note that ITUT96 does not use the SLR in the RLC to send a RLSD message as does
		   ANSI. */
		switch (sc->proto.pvar & SS7_PVAR_MASK) {
		case SS7_PVAR_ITUT:
		case SS7_PVAR_ETSI:
			/* 
			   Figure C.2/Q.714 Sheet 3 of 7: stop connection timer, release resources
			   and local reference, freeze local reference, move to the idle state. */
			return sccp_release_error(q, sc);
		default:
		case SS7_PVAR_JTTC:
		case SS7_PVAR_ANSI:
			/* 
			   Figure 2A/T1.112.4-2000 Sheet 3 of 4: same action as for CC in this
			   state: stop connection timer, associate destination reference to
			   connection section, release resources, stop inactivity timers, send
			   RLSD, start released (ANSI: and interval timers), move to the disconnect 
			   pending state. */
			sccp_release(sc);
			sc->dlr = m->slr;
			if ((err =
			     sccp_send_rlsd(q, sc->sp.sp, sc->sp.sp->add.pc, sc->sls, sc->dlr,
					    sc->slr, SCCP_RELC_END_USER_ORIGINATED, NULL, NULL)))
				return (err);
			sccp_timer_start(sc, trel);
			sccp_timer_start(sc, tint);	/* ANSI */
			sccp_set_state(sc, SS_WCON_DREQ);
			return (QR_DONE);
		}
	case SS_WRES_RIND:
		switch (sc->proto.pvar & SS7_PVAR_MASK) {
		case SS7_PVAR_ANSI:
			/* 
			   Figure 2E/T1.112.4-2000 2of2: send RSC, discard all queue and
			   unacknolwedged messages, */
			if ((err =
			     sccp_send_rsc(q, sc->sp.sp, sc->sp.sp->add.pc, sc->sls, sc->dlr,
					   sc->slr)))
				return (err);
			fixme(("Discard all queued and unacked messages\n"));
			return (QR_DONE);
		default:
		case SS7_PVAR_ITUT:
			/* 
			   Figure C.6/Q.714 Sheet 4 of 4: discard received message and generate
			   internal reset request, move to the active state. */
			fixme(("Internal reset request\n"));
			sccp_set_state(sc, SS_DATA_XFER);
			return (QR_DONE);
		}
	case SS_WCON_DREQ:	/* Figure C.2/Q.714 Sheet 6 of 7 */
		return (QR_DONE);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  SCCP_MT_RSC   0x0e - Reset Confirm
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR, SLR)
 *  ANSI: F(DLR, SLR)
 */
STATIC int
sccp_recv_rsc(queue_t *q, struct sc *sc, mblk_t *msg, struct sccp_msg *m)
{
	/* 
	   No optional parameters.  Optional parameters will be ignored. This message is only valid 
	   in protocol class 3.  DLR is used to look up the stream in the NS_DATA_XFER state.  This 
	   normally results in a N_RESET_CON primitive to the SCCP-User.  The stream is confirmed
	   reset. */
	int err;
	switch (sccp_get_state(sc)) {
	case SS_BOTH_RESET:
		switch (sc->proto.pvar & SS7_PVAR_MASK) {
		case SS7_PVAR_ANSI:
			/* 
			   Figure 2E/T1.112.4-2000: stop reset timer and move to the reset incoming 
			   state. */
			sccp_timer_stop(sc, tres);
			sccp_set_state(sc, SS_WRES_RIND);
			return (QR_DONE);
		default:
		case SS7_PVAR_ITUT:
			/* 
			   Figure C.6/Q.714 Sheet 3 of 4: Send a N_RESET_CON to the user, stop the
			   reset timer, restart the inactivity timer and move to the reset incoming 
			   state. */
			sccp_timer_stop(sc, tres);
			sccp_timer_start(sc, tiar);
			if ((err = sccp_reset_con(q, sc)))
				return (err);
			sccp_set_state(sc, SS_WRES_CIND);
			return (QR_DONE);
		}
	case SS_WCON_RREQ:
		/* 
		   Figure C.6/Q.714 Sheet 2 of 4: Send an N_RESET_CON to the user, stop the reset
		   timer, restart the receive inactivity timer, resume data transfer, move to the
		   active state. */
		sccp_timer_stop(sc, tres);
		sccp_timer_start(sc, tiar);
		if ((err = sccp_reset_con(q, sc)))
			return (err);
		sccp_set_state(sc, SS_DATA_XFER);
		return (QR_DONE);
	case SS_DATA_XFER:
		/* 
		   Figure C.6/Q.714 Sheet 1 of 4: Discard received message. */
		return (QR_DONE);
	case SS_WRES_RIND:
		/* 
		   Figure C.6/Q.714 Sheet 4 of 4: discard received message and generate internal
		   reset request, move to the active state. */
		fixme(("Internal reset request\n"));
		sccp_set_state(sc, SS_DATA_XFER);
		return (QR_DONE);
	case SS_WRES_DIND:
		goto error1;
	      error1:
		/* 
		   send RLSD */
		sccp_set_state(sc, SS_WCON_DREQ);
		return (QR_DONE);
	case SS_WCON_CREQ:
		return sccp_release_error(q, sc);
	case SS_WCON_CREQ2:
		/* 
		   Note that ITUT96 does not use the SLR in the RLC to send a RLSD message as does
		   ANSI. */
		switch (sc->proto.pvar & SS7_PVAR_MASK) {
		case SS7_PVAR_ITUT:
		case SS7_PVAR_ETSI:
			/* 
			   Figure C.2/Q.714 Sheet 3 of 7: stop connection timer, release resources
			   and local reference, freeze local reference, move to the idle state. */
			return sccp_release_error(q, sc);
		default:
		case SS7_PVAR_JTTC:
		case SS7_PVAR_ANSI:
			/* 
			   Figure 2A/T1.112.4-2000 Sheet 3 of 4: same action as for CC in this
			   state: stop connection timer, associate destination reference to
			   connection section, release resources, stop inactivity timers, send
			   RLSD, start released (ANSI: and interval timers), move to the disconnect 
			   pending state. */
			sccp_release(sc);
			sc->dlr = m->slr;
			if ((err =
			     sccp_send_rlsd(q, sc->sp.sp, sc->sp.sp->add.pc, sc->sls, sc->dlr,
					    sc->slr, SCCP_RELC_END_USER_ORIGINATED, NULL, NULL)))
				return (err);
			sccp_timer_start(sc, trel);
			sccp_timer_start(sc, tint);	/* ANSI */
			sccp_set_state(sc, SS_WCON_DREQ);
			return (QR_DONE);
		}
	case SS_WCON_DREQ:	/* Figure C.2/Q.714 Sheet 6 of 7 */
		return (QR_DONE);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  SCCP_MT_ERR   0x0f - Protocol Data Unit Error
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR, ERRC)
 *  ANSI: F(DLR, ERRC)
 */
STATIC int
sccp_recv_err(queue_t *q, struct sc *sc, mblk_t *msg, struct sccp_msg *m)
{
	/* 
	   No optional parameters.  Optional parameters will be ignored. This message is only valid 
	   in protocol class 3.  DLR is used to look up the stream.  This does not result in a
	   primitive to the SCCP-User directly, any one of a number of primitives may be sent
	   depending on the disposition of the error. Management is informed of the error
	   condition. */
	int err;
	switch (sccp_get_state(sc)) {
		/* 
		   Figure C.2/Q.714 Sheet 5 of 7: Figure C.3/Q.714 Sheet 4 of 6: if the error is
		   not service class mismatch: release resourcesa and local reference, freeze local 
		   reference, send N_DISCON_IND to user, stop inactivity timers, move to the idle
		   state. */
	case SS_DATA_XFER:
		/* 
		   Notes: this is somewhat different between variants here. ANSI always sends RLSD
		   and goes through the released procedure.  ITUT only sends RLSD in the case of
		   service class mismatch, otherwise it simply aborts. */
		switch (sc->proto.pvar & SS7_PVAR_MASK) {
		case SS7_PVAR_ANSI:
			/* 
			   Figure 2A/T1.112.4-2000: Figure 2B/T1.112.4-2000 2of2: send an
			   N_DISCON_IND to the user, release resources for the connection, stop
			   inactivity timers, send RLSD, start released and interval timers, move
			   to the disconnect pending state. */
			sccp_release(sc);
			if ((err =
			     sccp_send_rlsd(q, sc->sp.sp, sc->sp.sp->add.pc, sc->sls, sc->dlr,
					    sc->slr, SCCP_RELC_REMOTE_PROCEDURE_ERROR, NULL, NULL)))
				return (err);
			{
				int cause = 0;
				fixme(("Determine cuase\n"));
				if ((err =
				     sccp_discon_ind(q, sc, N_PROVIDER, cause, NULL, NULL, NULL)))
					return (err);
			}
			sccp_timer_start(sc, trel);
			sccp_timer_start(sc, tint);
			sccp_set_state(sc, SS_WCON_DREQ);
			return (QR_DONE);
		default:
		case SS7_PVAR_ITUT:
			if (m->cause == (SCCP_ERRC_SERVICE_CLASS_MISMATCH & 0xff)) {
				/* 
				   Figure C.2/Q.714 Sheet 4 of 7: send N_DISCON_IND to user, stop
				   inactivity timers, send RLSD, start released timer, move to
				   disconnect pending state. */
				sccp_timer_stop(sc, tias);
				sccp_timer_stop(sc, tiar);
				if ((err =
				     sccp_send_rlsd(q, sc->sp.sp, sc->sp.sp->add.pc, sc->sls,
						    sc->dlr, sc->slr,
						    SCCP_RELC_REMOTE_PROCEDURE_ERROR, NULL,
						    &sc->imp)))
					return (err);
				if ((err =
				     sccp_discon_ind(q, sc, N_PROVIDER, m->cause | 0x4000, NULL,
						     NULL, NULL)))
					return (err);
				sccp_timer_start(sc, trel);
				sccp_set_state(sc, SS_WCON_DREQ);
				return (QR_DONE);
			} else {
				/* 
				   Figure C.2/Q.714 Sheet 5 of 7: release resources and local
				   reference, deliver N_DISCON_IND to user, stop inactivity timers, 
				   move to idle state. */
				sccp_release(sc);
				if ((err =
				     sccp_discon_ind(q, sc, N_PROVIDER, m->cause | 0x4000, NULL,
						     NULL, NULL)))
					return (err);
				sccp_set_state(sc, SS_IDLE);
				return (QR_DONE);
			}
		}
	case SS_WRES_RIND:
		/* 
		   Figure C.6/Q.714 Sheet 4 of 4: Figure C.3/Q.714 Sheet 4 of 6: release resources
		   and local reference, freeze local reference, send N_DISCON_IND to the user, stop 
		   inactivity timers, move to the idle state. */
		return sccp_release_error(q, sc);
	case SS_WCON_CREQ:
		return sccp_release_error(q, sc);
	case SS_WCON_CREQ2:
		switch (sc->proto.pvar & SS7_PVAR_MASK) {
		case SS7_PVAR_ITUT:
		case SS7_PVAR_ETSI:
			return sccp_release_error(q, sc);
		default:
		case SS7_PVAR_JTTC:
		case SS7_PVAR_ANSI:
			/* 
			   Figure 2A/T1.112.4-2000 Sheet 3 of 4: discard, continue to wait for
			   connection confirm. */
			return (QR_DONE);
		}
	case SS_WCON_DREQ:
		/* 
		   Figure C.2/Q.714 Sheet 6 of 7: release resources and local reference, freeze
		   local reference, stop release and interval timers, move to the idle state.
		   Figure 2A/T1.112.4-2000: Figure 2B/T1.112.4-2000: Release references for the
		   connection section, stop release and interval timers, move to the idle state. */
		sccp_release(sc);
		sccp_set_state(sc, SS_IDLE);
		return (QR_DONE);
	case SS_WRES_DIND:
		sccp_release(sc);
		sccp_set_state(sc, SS_IDLE);
		return (QR_DONE);
	case SS_BOTH_RESET:
		/* 
		   Figure C.6/Q.714 Sheet 3 of 4: same as below. */
	case SS_WCON_RREQ:
		/* 
		   Figure C.6/Q.714 Sheet 2 of 4: Figure C.2/Q.714 Sheet 5 of 7: Release resources
		   and local reference, freeze local reference, send an N_DISCON_IND to the user,
		   stop inactivity timers and move to the idle state. */
		return sccp_release_error(q, sc);
		return (QR_DONE);
	default:
		swerr();
		return (-EFAULT);
	}
}

/*
 *  SCCP_MT_IT    0x10 - Inactivity Test
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR, SLR, PCLS, SEQ, CRED)
 *  ANSI: F(DLR, SLR, PCLS, SEQ, CRED)
 */
STATIC int
sccp_recv_it(queue_t *q, struct sc *sc, mblk_t *msg, struct sccp_msg *m)
{
	/* 
	   No optional parameters.  Optional parameters will be ignored.  DLR is used to look up
	   the stream. This does not result in a primitive to the SCCP-User.  This is a heartbeat
	   between SCCP peers.  It is responded to by the state machine. */
	int err;
	switch (sccp_get_state(sc)) {
	case SS_DATA_XFER:
		/* 
		   Figure 2D/T1.112.4-2000: just reset the receive inactivity timer.  No audit or
		   internal reset specified for ANSI. Figure C.4/Q.714 Sheet 4 of 4: If we are
		   protocol class 3 we audit the received P(S), P(R), M and W.  If these are ok, or 
		   we are protocol class 2, we reset the receive inactivity timer.  If these are
		   not ok, we perform an internal reset. */
		fixme(("Write this function\n"));
		return (-EFAULT);
	case SS_BOTH_RESET:	/* Figure C.6/Q.714 Sheet 3 of 4 */
		return (QR_DONE);
	case SS_WCON_CREQ2:
		/* 
		   Note that ITUT96 does not use the SLR in the RLC to send a RLSD message as does
		   ANSI. */
		switch (sc->proto.pvar & SS7_PVAR_MASK) {
		case SS7_PVAR_ITUT:
		case SS7_PVAR_ETSI:
			/* 
			   Figure C.2/Q.714 Sheet 3 of 7: stop connection timer, release resources
			   and local reference, freeze local reference, move to the idle state. */
			return sccp_release_error(q, sc);
		default:
		case SS7_PVAR_JTTC:
		case SS7_PVAR_ANSI:
			/* 
			   Figure 2A/T1.112.4-2000 Sheet 3 of 4: same action as for CC in this
			   state: stop connection timer, associate destination reference to
			   connection section, release resources, stop inactivity timers, send
			   RLSD, start released (ANSI: and interval timers), move to the disconnect 
			   pending state. */
			sccp_release(sc);
			sc->dlr = m->slr;
			if ((err =
			     sccp_send_rlsd(q, sc->sp.sp, sc->sp.sp->add.pc, sc->sls, sc->dlr,
					    sc->slr, SCCP_RELC_REMOTE_PROCEDURE_ERROR, NULL, NULL)))
				return (err);
			sccp_timer_start(sc, trel);
			sccp_timer_start(sc, tint);	/* ANSI */
			sccp_set_state(sc, SS_WCON_DREQ);
			return (QR_DONE);
		}
	default:
		swerr();
		return (-EFAULT);
	}
}

/*
 *  SCCP_MT_UDT   0x09 - Unitdata
 *  -------------------------------------------------------------------------
 *  ITUT: F(PCLS), V(CDPA, CGPA, DATA[2-254])
 *  ANSI: F(PCLS), V(CDPA, CGPA, DATA[2-252])
 */
STATIC int
sccp_recv_udt(queue_t *q, struct sc *sc, mblk_t *msg, struct sccp_msg *m)
{
	/* 
	   No optional parameters.  Optional parameters will be ignored. This message is only valid 
	   in protocol classes 0 and 1.  CDPA and the DPC in the message is used to look up a
	   connection-less stream in the NS_IDLE state.  This should result in a N_UNITDATA_IND to
	   the SCCP-User. */
	int err;
	fixme(("Perform segmentation\n"));
	switch (sccp_get_state(sc)) {
	case SS_IDLE:
		if ((err =
		     sccp_unitdata_ind(q, sc, &m->cgpa, &m->cdpa, m->pcl, m->ret, m->imp, m->rl.sls,
				       m->rl.mp, msg->b_cont)))
			return (err);
		return (QR_DONE);
	case SS_WCON_CREQ:
		return sccp_release_error(q, sc);
	case SS_WCON_CREQ2:
		switch (sc->proto.pvar & SS7_PVAR_MASK) {
		case SS7_PVAR_ITUT:
		case SS7_PVAR_ETSI:
			return sccp_release_error(q, sc);
		default:
		case SS7_PVAR_JTTC:
		case SS7_PVAR_ANSI:
			/* 
			   Figure 2A/T1.112.4-2000 Sheet 3 of 4: discard, continue to wait for
			   connection confirm. */
			return (QR_DONE);
		}
	case SS_WRES_RIND:
		/* 
		   Figure C.6/Q.714 Sheet 4 of 4: discard received message and generate internal
		   reset request, move to the active state. */
		fixme(("Internal reset request\n"));
		sccp_set_state(sc, SS_DATA_XFER);
		return (QR_DONE);
	case SS_DATA_XFER:
	case SS_BOTH_RESET:	/* Figure C.6/Q.714 Sheet 3 of 4 */
	case SS_WCON_RREQ:	/* Figure C.6/Q.714 Sheet 2 of 4 */
	case SS_WRES_DIND:
	case SS_WCON_DREQ:	/* Figure C.2/Q.714 Sheet 6 of 7 */
		return (QR_DONE);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  SCCP_MT_UDTS  0x0a - Unitdata Service
 *  -------------------------------------------------------------------------
 *  ITUT: F(RETC), V(CDPA, CGPA, DATA[2-253])
 *  ANSI: F(RETC), V(CDPA, CGPA, DATA[2-251])
 */
STATIC int
sccp_recv_udts(queue_t *q, struct sc *sc, mblk_t *msg, struct sccp_msg *m)
{
	/* 
	   No optional parameters.  Optional parameters will be ignored. This message is only valid 
	   in protocol classes 0 and 1.  CGPA and DPC in the message are used to look up a
	   connection-less stream in the NS_IDLE state.  This should result in a N_UDERROR_IND to
	   the SCCP-User. */
	int err;
	switch (sccp_get_state(sc)) {
	case SS_IDLE:
		if ((err =
		     sccp_notice_ind(q, sc, 0x2000 | m->cause, &m->cgpa, &m->cdpa, m->pcl, m->ret,
				     m->imp, m->rl.sls, m->rl.mp, msg->b_cont)))
			return (err);
		return (QR_DONE);
	case SS_WRES_RIND:
		/* 
		   Figure C.6/Q.714 Sheet 4 of 4: discard received message and generate internal
		   reset request, move to the active state. */
		fixme(("Internal reset request\n"));
		sccp_set_state(sc, SS_DATA_XFER);
		return (QR_DONE);
	case SS_WCON_CREQ:
		return sccp_release_error(q, sc);
	case SS_WCON_CREQ2:
		switch (sc->proto.pvar & SS7_PVAR_MASK) {
		case SS7_PVAR_ITUT:
		case SS7_PVAR_ETSI:
			return sccp_release_error(q, sc);
		default:
		case SS7_PVAR_JTTC:
		case SS7_PVAR_ANSI:
			/* 
			   Figure 2A/T1.112.4-2000 Sheet 3 of 4: discard, continue to wait for
			   connection confirm. */
			return (QR_DONE);
		}
	case SS_DATA_XFER:
	case SS_BOTH_RESET:	/* Figure C.6/Q.714 Sheet 3 of 4 */
	case SS_WCON_RREQ:	/* Figure C.6/Q.714 Sheet 2 of 4 */
	case SS_WRES_DIND:
	case SS_WCON_DREQ:	/* Figure C.2/Q.714 Sheet 6 of 7 */
		return (QR_DONE);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  SCCP_MT_XUDT  0x11 - Extended Unitdata
 *  -------------------------------------------------------------------------
 *  ITUT: F(PCLS, HOPC), V(CDPA, CGPA, DATA[2-253]), O(SGMT, IMP, EOP)
 *  ANSI: F(PCLS, HOPC), V(CDPA, CGPA, DATA[2-251]), O(SGMT, ISNI, INS, MTI, EOP)
 */
STATIC int
sccp_recv_xudt(queue_t *q, struct sc *sc, mblk_t *msg, struct sccp_msg *m)
{
	/* 
	   PCLS is passed to the user.  HOPC is only used by a relay node. CDPA and CGPA and DATA
	   are passed to the user.  SGMT is only used when segmentation of the XUDT message has
	   occured.  Segmented XUDT messages will be reassembled before passing to the user.  ISNI, 
	   INS and MTI are for special network interworking and are only used at relay points.  IMP 
	   is used for congestion control.  This message is only valid in protocol classes 0 and 1. 
	   If SGMT is present, the message must be protocol class 1 (with the original protocol
	   class from the SGMT parameter passed to the user).  CDPA was already used to look up
	   this CLNS stream in the NS_IDLE state.  After reassembly addresses and data are sent to
	   the user. This normally results in a N_UNITDATA_IND primitive to the SCCP-User. */
	int err;
	fixme(("Perform segmentation\n"));
	switch (sccp_get_state(sc)) {
	case SS_IDLE:
		if ((err =
		     sccp_unitdata_ind(q, sc, &m->cgpa, &m->cdpa, m->pcl, m->ret, m->imp, m->rl.sls,
				       m->rl.mp, msg->b_cont)))
			return (err);
		return (QR_DONE);
	case SS_WCON_CREQ:
		return sccp_release_error(q, sc);
	case SS_WCON_CREQ2:
		switch (sc->proto.pvar & SS7_PVAR_MASK) {
		case SS7_PVAR_ITUT:
		case SS7_PVAR_ETSI:
			return sccp_release_error(q, sc);
		default:
		case SS7_PVAR_JTTC:
		case SS7_PVAR_ANSI:
			/* 
			   Figure 2A/T1.112.4-2000 Sheet 3 of 4: discard, continue to wait for
			   connection confirm. */
			return (QR_DONE);
		}
	case SS_WRES_RIND:
		/* 
		   Figure C.6/Q.714 Sheet 4 of 4: discard received message and generate internal
		   reset request, move to the active state. */
		fixme(("Internal reset request\n"));
		sccp_set_state(sc, SS_DATA_XFER);
		return (QR_DONE);
	case SS_DATA_XFER:
	case SS_BOTH_RESET:	/* Figure C.6/Q.714 Sheet 3 of 4 */
	case SS_WCON_RREQ:	/* Figure C.6/Q.714 Sheet 2 of 4 */
	case SS_WRES_DIND:
	case SS_WCON_DREQ:	/* Figure C.2/Q.714 Sheet 6 of 7 */
		return (QR_DONE);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  SCCP_MT_XUDTS 0x12 - Extended Unitdata Service
 *  -------------------------------------------------------------------------
 *  ITUT: F(RETC, HOPC), V(CDPA, CGPA, DATA[2-253]), O(SGMT, IMP, EOP)
 *  ANSI: F(RETC, HOPC), V(CDPA, CGPA, DATA[2-251]), O(SGMT, INS, MTI, ISNI, EOP)
 */
STATIC int
sccp_recv_xudts(queue_t *q, struct sc *sc, mblk_t *msg, struct sccp_msg *m)
{
	/* 
	   Same as XUDT but this is passed as a N_UDERROR_IND. */
	int err;
	switch (sccp_get_state(sc)) {
	case SS_IDLE:
		if ((err =
		     sccp_notice_ind(q, sc, 0x2000 | m->cause, &m->cgpa, &m->cdpa, m->pcl, m->ret,
				     m->imp, m->rl.sls, m->rl.mp, msg->b_cont)))
			return (err);
		return (QR_DONE);
	case SS_WCON_CREQ:
		return sccp_release_error(q, sc);
	case SS_WCON_CREQ2:
		switch (sc->proto.pvar & SS7_PVAR_MASK) {
		case SS7_PVAR_ITUT:
		case SS7_PVAR_ETSI:
			return sccp_release_error(q, sc);
		default:
		case SS7_PVAR_JTTC:
		case SS7_PVAR_ANSI:
			/* 
			   Figure 2A/T1.112.4-2000 Sheet 3 of 4: discard, continue to wait for
			   connection confirm. */
			return (QR_DONE);
		}
	case SS_WRES_RIND:
		/* 
		   Figure C.6/Q.714 Sheet 4 of 4: discard received message and generate internal
		   reset request, move to the active state. */
		fixme(("Internal reset request\n"));
		sccp_set_state(sc, SS_DATA_XFER);
		return (QR_DONE);
	case SS_DATA_XFER:
	case SS_BOTH_RESET:	/* Figure C.6/Q.714 Sheet 3 of 4 */
	case SS_WCON_RREQ:	/* Figure C.6/Q.714 Sheet 2 of 4 */
	case SS_WRES_DIND:
	case SS_WCON_DREQ:	/* Figure C.2/Q.714 Sheet 6 of 7 */
		return (QR_DONE);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  SCCP_MT_LUDT  0x13 - Long Unitdata
 *  -------------------------------------------------------------------------
 *  ITUT: F(PCLS, HOPC), V(CDPA, CGPA, LDATA[3-3906]), O(SGMT, IMP, EOP)
 *  ANSI: F(PCLS, HOPC), V(CDPA, CGPA, LDATA[3-3904]), O(SGMT, ISNI, INS, EOP)
 */
STATIC int
sccp_recv_ludt(queue_t *q, struct sc *sc, mblk_t *msg, struct sccp_msg *m)
{
	/* 
	   Same as XUDT but longer data and no MTI. */
	int err;
	fixme(("Perform segmentation\n"));
	switch (sccp_get_state(sc)) {
	case SS_IDLE:
		if ((err =
		     sccp_unitdata_ind(q, sc, &m->cgpa, &m->cdpa, m->pcl, m->ret, m->imp, m->rl.sls,
				       m->rl.mp, msg->b_cont)))
			return (err);
		return (QR_DONE);
	case SS_WCON_CREQ:
		swerr();
		return sccp_release_error(q, sc);
	case SS_WCON_CREQ2:
		swerr();
		switch (sc->proto.pvar & SS7_PVAR_MASK) {
		case SS7_PVAR_ITUT:
		case SS7_PVAR_ETSI:
			return sccp_release_error(q, sc);
		default:
		case SS7_PVAR_JTTC:
		case SS7_PVAR_ANSI:
			/* 
			   Figure 2A/T1.112.4-2000 Sheet 3 of 4: discard, continue to wait for
			   connection confirm. */
			return (QR_DONE);
		}
	case SS_WRES_RIND:
		swerr();
		/* 
		   Figure C.6/Q.714 Sheet 4 of 4: discard received message and generate internal
		   reset request, move to the active state. */
		fixme(("Internal reset request\n"));
		sccp_set_state(sc, SS_DATA_XFER);
		return (QR_DONE);
	case SS_DATA_XFER:
	case SS_BOTH_RESET:	/* Figure C.6/Q.714 Sheet 3 of 4 */
	case SS_WCON_RREQ:	/* Figure C.6/Q.714 Sheet 2 of 4 */
	case SS_WRES_DIND:
	case SS_WCON_DREQ:	/* Figure C.2/Q.714 Sheet 6 of 7 */
		swerr();
		return (QR_DONE);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  SCCP_MT_LUDTS 0x14 - Long Unitdata Service
 *  -------------------------------------------------------------------------
 *  ITUT: F(RETC, HOPC), V(CDPA, CGPA, LDATA[3-3906]), O(SGMT, IMP, EOP)
 *  ANSI: F(RETC, HOPC), V(CDPA, CGPA, LDATA[3-3904]), O(SGMT, ISNI, INS, EOP)
 */
STATIC int
sccp_recv_ludts(queue_t *q, struct sc *sc, mblk_t *msg, struct sccp_msg *m)
{
	/* 
	   Same as XUDTS but longer data and no MTI. */
	int err;
	switch (sccp_get_state(sc)) {
	case SS_IDLE:
		if ((err =
		     sccp_notice_ind(q, sc, 0x2000 | m->cause, &m->cgpa, &m->cdpa, m->pcl, m->ret,
				     m->imp, m->rl.sls, m->rl.mp, msg->b_cont)))
			return (err);
		return (QR_DONE);
	case SS_WCON_CREQ:
		return sccp_release_error(q, sc);
	case SS_WCON_CREQ2:
		switch (sc->proto.pvar & SS7_PVAR_MASK) {
		case SS7_PVAR_ITUT:
		case SS7_PVAR_ETSI:
			return sccp_release_error(q, sc);
		default:
		case SS7_PVAR_JTTC:
		case SS7_PVAR_ANSI:
			/* 
			   Figure 2A/T1.112.4-2000 Sheet 3 of 4: discard, continue to wait for
			   connection confirm. */
			return (QR_DONE);
		}
	case SS_WRES_RIND:
		/* 
		   Figure C.6/Q.714 Sheet 4 of 4: discard received message and generate internal
		   reset request, move to the active state. */
		fixme(("Internal reset request\n"));
		sccp_set_state(sc, SS_DATA_XFER);
		return (QR_DONE);
	case SS_DATA_XFER:
	case SS_BOTH_RESET:	/* Figure C.6/Q.714 Sheet 3 of 4 */
	case SS_WCON_RREQ:	/* Figure C.6/Q.714 Sheet 2 of 4 */
	case SS_WRES_DIND:
	case SS_WCON_DREQ:	/* Figure C.2/Q.714 Sheet 6 of 7 */
		return (QR_DONE);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  SCMG_MT_SSA
 *  -------------------------------------------------------------------------
 */
STATIC int
scmg_recv_ssa(queue_t *q, struct sp *sp, struct sr *sr, struct rs *rs, mblk_t *mp,
	      struct sccp_msg *m)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  SCMG_MT_SSP
 *  -------------------------------------------------------------------------
 */
STATIC int
scmg_recv_ssp(queue_t *q, struct sp *sp, struct sr *sr, struct rs *rs, mblk_t *mp,
	      struct sccp_msg *m)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  SCMG_MT_SST
 *  -------------------------------------------------------------------------
 */
STATIC int
scmg_recv_sst(queue_t *q, struct sp *sp, struct sr *sr, struct rs *rs, mblk_t *mp,
	      struct sccp_msg *m)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  SCMG_MT_SOR
 *  -------------------------------------------------------------------------
 */
STATIC int
scmg_recv_sor(queue_t *q, struct sp *sp, struct sr *sr, struct rs *rs, mblk_t *mp,
	      struct sccp_msg *m)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  SCMG_MT_SOG
 *  -------------------------------------------------------------------------
 */
STATIC int
scmg_recv_sog(queue_t *q, struct sp *sp, struct sr *sr, struct rs *rs, mblk_t *mp,
	      struct sccp_msg *m)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  SCMG_MT_SSC
 *  -------------------------------------------------------------------------
 */
STATIC int
scmg_recv_ssc(queue_t *q, struct sp *sp, struct sr *sr, struct rs *rs, mblk_t *mp,
	      struct sccp_msg *m)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  SCMG_MT_SBR
 *  -------------------------------------------------------------------------
 */
STATIC int
scmg_recv_sbr(queue_t *q, struct sp *sp, struct sr *sr, struct rs *rs, mblk_t *mp,
	      struct sccp_msg *m)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  SCMG_MT_SNR
 *  -------------------------------------------------------------------------
 */
STATIC int
scmg_recv_snr(queue_t *q, struct sp *sp, struct sr *sr, struct rs *rs, mblk_t *mp,
	      struct sccp_msg *m)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  SCMG_MT_SRT
 *  -------------------------------------------------------------------------
 */
STATIC int
scmg_recv_srt(queue_t *q, struct sp *sp, struct sr *sr, struct rs *rs, mblk_t *mp,
	      struct sccp_msg *m)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SCCP Decode Message Functions
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  SCCP_MT_CR    0x01 - Connection Request
 *  -------------------------------------------------------------------------
 *  ITUT: F(SLR, PCLS), V(CDPA), O(CRED, CGPA, DATA, HOPC, IMP, EOP)
 *  ASNI: F(SLR, PCLS), V(CDPA), O(CRED, CGPA, DATA, HOPC, EOP)
 */
STATIC INLINE int
sccp_dec_cr(uchar *p, uchar *e, struct sccp_msg *m)
{
	int rtn;
	uchar *pp, *ep;
	m->type = SCCP_MT_CR;
	m->parms = 0;
	if ((rtn = unpack_slr(p, e, m)) < 0)
		return (rtn);	/* SLR F */
	p += rtn;
	if ((rtn = unpack_pcls(p, e, m)) < 0)
		return (rtn);	/* PCLS F */
	p += rtn;
	if (m->pcl != 2 && m->pcl != 3)
		return (-EPROTO);
	if ((pp = p + *p++) > e)
		return (-EMSGSIZE);
	if ((ep = pp + *pp++) > e)
		return (-EMSGSIZE);
	if ((rtn = unpack_cdpa(p, e, m)) < 0)
		return (rtn);	/* CDPA V */
	if (*p)
		sccp_dec_opt(p + *p, e, m);
	if (!(m->parms & SCCP_PTF_IMP))
		m->imp = SCCP_DI_CR;
	return (0);
}

/*
 *  SCCP_MT_CC    0x02 - Connection Confirm
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR, SLR, PCLS), O(CRED, CDPA, DATA, IMP, EOP)
 *  ANSI: F(DLR, SLR, PCLS), O(CRED, CDPA, DATA, EOP)
 */
STATIC INLINE int
sccp_dec_cc(uchar *p, uchar *e, struct sccp_msg *m)
{
	int rtn;
	m->type = SCCP_MT_CC;
	m->parms = 0;
	if ((rtn = unpack_dlr(p, e, m)) < 0)
		return (rtn);	/* DLR F */
	p += rtn;
	if ((rtn = unpack_slr(p, e, m)) < 0)
		return (rtn);	/* SLR F */
	p += rtn;
	if ((rtn = unpack_pcls(p, e, m)) < 0)
		return (rtn);	/* PCLS F */
	p += rtn;
	if (m->pcl != 2 && m->pcl != 3)
		return (-EPROTO);
	if (*p)
		sccp_dec_opt(p + *p, e, m);
	if (!(m->parms & SCCP_PTF_IMP))
		m->imp = SCCP_DI_CC;
	return (0);
}

/*
 *  SCCP_MT_CREF  0x03 - Connection Refused
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR, REFC), O(CDPA, DATA, IMP, EOP)
 *  ANSI: F(DLR, REFC), O(CDPA, DATA, EOP)
 */
STATIC INLINE int
sccp_dec_cref(uchar *p, uchar *e, struct sccp_msg *m)
{
	int rtn;
	m->type = SCCP_MT_CREF;
	m->parms = 0;
	if ((rtn = unpack_dlr(p, e, m)) < 0)
		return (rtn);	/* DLR F */
	p += rtn;
	if ((rtn = unpack_refc(p, e, m)) < 0)
		return (rtn);	/* REFC F */
	p += rtn;
	if (*p)
		sccp_dec_opt(p + *p, e, m);
	if (!(m->parms & SCCP_PTF_IMP))
		m->imp = SCCP_DI_CREF;
	return (0);
}

/*
 *  SCCP_MT_RLSD  0x04 - Released
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR, SLR, RELC), O(DATA, IMP, EOP)
 *  ANSI: F(DLR, SLR, RELC), O(DATA, EOP)
 */
STATIC INLINE int
sccp_dec_rlsd(uchar *p, uchar *e, struct sccp_msg *m)
{
	int rtn;
	m->type = SCCP_MT_RLSD;
	m->parms = 0;
	if ((rtn = unpack_dlr(p, e, m)) < 0)
		return (rtn);	/* DLR F */
	p += rtn;
	if ((rtn = unpack_slr(p, e, m)) < 0)
		return (rtn);	/* SLR F */
	p += rtn;
	if ((rtn = unpack_relc(p, e, m)) < 0)
		return (rtn);	/* RELC F */
	p += rtn;
	if (*p)
		sccp_dec_opt(p + *p, e, m);
	if (!(m->parms & SCCP_PTF_IMP))
		m->imp = SCCP_DI_RLSD;
	return (0);
}

/*
 *  SCCP_MT_RLC   0x05 - Release Complete
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR, SLR)
 *  ANSI: F(DLR, SLR)
 */
STATIC INLINE int
sccp_dec_rlc(uchar *p, uchar *e, struct sccp_msg *m)
{
	int rtn;
	m->type = SCCP_MT_RLC;
	m->parms = 0;
	if ((rtn = unpack_dlr(p, e, m)) < 0)
		return (rtn);	/* DLR F */
	p += rtn;
	if ((rtn = unpack_slr(p, e, m)) < 0)
		return (rtn);	/* SLR F */
	p += rtn;
	m->imp = SCCP_DI_RLC;
	return (0);
}

/*
 *  SCCP_MT_DT1   0x06 - Data Form 1
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR, SEG), V(DATA)
 *  ANSI: F(DLR, SEG), V(DATA)
 */
STATIC INLINE int
sccp_dec_dt1(uchar *p, uchar *e, struct sccp_msg *m)
{
	int rtn;
	uchar *pp, *ep;
	m->type = SCCP_MT_DT1;
	m->parms = 0;
	if ((rtn = unpack_dlr(p, e, m)) < 0)
		return (rtn);	/* DLR F */
	p += rtn;
	if ((rtn = unpack_seg(p, e, m)) < 0)
		return (rtn);	/* SEG F */
	p += rtn;
	if ((pp = p + *p++) > e)
		return (-EMSGSIZE);
	if ((ep = pp + *pp++) > e)
		return (-EMSGSIZE);
	if ((rtn = unpack_data(pp, ep, m)) < 0)
		return (rtn);	/* DATA V */
	m->imp = SCCP_DI_DT1;
	return (0);
}

/*
 *  SCCP_MT_DT2   0x07 - Data Form 2
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR, SEQ), V(DATA)
 *  ANSI: F(DLR, SEQ), V(DATA)
 */
STATIC INLINE int
sccp_dec_dt2(uchar *p, uchar *e, struct sccp_msg *m)
{
	int rtn;
	uchar *pp, *ep;
	m->type = SCCP_MT_DT2;
	m->parms = 0;
	if ((rtn = unpack_dlr(p, e, m)) < 0)
		return (rtn);	/* DLR F */
	p += rtn;
	if ((rtn = unpack_seq(p, e, m)) < 0)
		return (rtn);	/* SEQ F */
	p += rtn;
	if ((pp = p + *p++) > e)
		return (-EMSGSIZE);
	if ((ep = pp + *pp++) > e)
		return (-EMSGSIZE);
	if ((rtn = unpack_data(pp, ep, m)) < 0)
		return (rtn);	/* DATA V */
	m->imp = SCCP_DI_DT2;
	return (0);
}

/*
 *  SCCP_MT_AK    0x08 - Data Acknowledgement
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR, RSN, CRED)
 *  ANSI: F(DLR, RSN, CRED)
 */
STATIC INLINE int
sccp_dec_ak(uchar *p, uchar *e, struct sccp_msg *m)
{
	int rtn;
	m->type = SCCP_MT_AK;
	m->parms = 0;
	if ((rtn = unpack_dlr(p, e, m)) < 0)
		return (rtn);	/* DLR F */
	p += rtn;
	if ((rtn = unpack_rsn(p, e, m)) < 0)
		return (rtn);	/* RSN F */
	p += rtn;
	if ((rtn = unpack_cred(p, e, m)) < 0)
		return (rtn);	/* CRED F */
	p += rtn;
	m->imp = SCCP_DI_AK;
	return (0);
}

/*
 *  SCCP_MT_ED    0x0b - Expedited Data
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR), V(DATA)
 *  ANSI: F(DLR), V(DATA)
 */
STATIC INLINE int
sccp_dec_ed(uchar *p, uchar *e, struct sccp_msg *m)
{
	int rtn;
	uchar *pp, *ep;
	m->type = SCCP_MT_ED;
	m->parms = 0;
	if ((rtn = unpack_dlr(p, e, m)) < 0)
		return (rtn);	/* DLR F */
	p += rtn;
	if ((pp = p + *p++) > e)
		return (-EMSGSIZE);
	if ((ep = pp + *pp++) > e)
		return (-EMSGSIZE);
	if ((rtn = unpack_data(pp, ep, m)) < 0)
		return (rtn);	/* DATA V */
	m->imp = SCCP_DI_ED;
	return (0);
}

/*
 *  SCCP_MT_EA    0x0c - Expedited Data Acknowledgement
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR)
 *  ANSI: F(DLR)
 */
STATIC INLINE int
sccp_dec_ea(uchar *p, uchar *e, struct sccp_msg *m)
{
	int rtn;
	m->type = SCCP_MT_EA;
	m->parms = 0;
	if ((rtn = unpack_dlr(p, e, m)) < 0)
		return (rtn);	/* DLR F */
	p += rtn;
	m->imp = SCCP_DI_EA;
	return (0);
}

/*
 *  SCCP_MT_RSR   0x0d - Reset Request
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR, SLR, RESC)
 *  ANSI: F(DLR, SLR, RESC)
 */
STATIC INLINE int
sccp_dec_rsr(uchar *p, uchar *e, struct sccp_msg *m)
{
	int rtn;
	m->type = SCCP_MT_RSR;
	m->parms = 0;
	if ((rtn = unpack_dlr(p, e, m)) < 0)
		return (rtn);	/* DLR F */
	p += rtn;
	if ((rtn = unpack_slr(p, e, m)) < 0)
		return (rtn);	/* SLR F */
	p += rtn;
	if ((rtn = unpack_resc(p, e, m)) < 0)
		return (rtn);	/* RESC F */
	p += rtn;
	m->imp = SCCP_DI_RSR;
	return (0);
}

/*
 *  SCCP_MT_RSC   0x0e - Reset Confirm
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR, SLR)
 *  ANSI: F(DLR, SLR)
 */
STATIC INLINE int
sccp_dec_rsc(uchar *p, uchar *e, struct sccp_msg *m)
{
	int rtn;
	m->type = SCCP_MT_RSC;
	m->parms = 0;
	if ((rtn = unpack_dlr(p, e, m)) < 0)
		return (rtn);	/* DLR F */
	p += rtn;
	if ((rtn = unpack_slr(p, e, m)) < 0)
		return (rtn);	/* SLR F */
	p += rtn;
	m->imp = SCCP_DI_RSC;
	return (0);
}

/*
 *  SCCP_MT_ERR   0x0f - Protocol Data Unit Error
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR, ERRC)
 *  ANSI: F(DLR, ERRC)
 */
STATIC INLINE int
sccp_dec_err(uchar *p, uchar *e, struct sccp_msg *m)
{
	int rtn;
	m->type = SCCP_MT_ERR;
	m->parms = 0;
	if ((rtn = unpack_dlr(p, e, m)) < 0)
		return (rtn);	/* DLR F */
	p += rtn;
	if ((rtn = unpack_errc(p, e, m)) < 0)
		return (rtn);	/* ERRC F */
	p += rtn;
	m->imp = SCCP_DI_ERR;
	return (0);
}

/*
 *  SCCP_MT_IT    0x10 - Inactivity Test
 *  -------------------------------------------------------------------------
 *  ITUT: F(DLR, SLR, PCLS, SEQ, CRED)
 *  ANSI: F(DLR, SLR, PCLS, SEQ, CRED)
 */
STATIC INLINE int
sccp_dec_it(uchar *p, uchar *e, struct sccp_msg *m)
{
	int rtn;
	m->type = SCCP_MT_IT;
	m->parms = 0;
	if ((rtn = unpack_dlr(p, e, m)) < 0)
		return (rtn);	/* DLR F */
	p += rtn;
	if ((rtn = unpack_slr(p, e, m)) < 0)
		return (rtn);	/* SLR F */
	p += rtn;
	if ((rtn = unpack_pcls(p, e, m)) < 0)
		return (rtn);	/* PCLS F */
	p += rtn;
	if (m->pcl != 2 && m->pcl != 3)
		return (-EPROTO);
	if ((rtn = unpack_seq(p, e, m)) < 0)
		return (rtn);	/* SEQ F */
	p += rtn;
	if ((rtn = unpack_cred(p, e, m)) < 0)
		return (rtn);	/* CRED F */
	p += rtn;
	m->imp = SCCP_DI_IT;
	return (0);
}

/*
 *  SCCP_MT_UDT   0x09 - Unitdata
 *  -------------------------------------------------------------------------
 *  ITUT: F(PCLS), V(CDPA, CGPA, DATA[2-254])
 *  ANSI: F(PCLS), V(CDPA, CGPA, DATA[2-252])
 */
STATIC INLINE int
sccp_dec_udt(uchar *p, uchar *e, struct sccp_msg *m)
{
	int rtn;
	uchar *pp, *ep;
	m->type = SCCP_MT_UDT;
	m->parms = 0;
	if ((rtn = unpack_pcls(p, e, m)) < 0)
		return (rtn);	/* PCLS F */
	p += rtn;
	if (m->pcl != 0 && m->pcl != 1)
		return (-EPROTO);
	if ((pp = p + *p++) > e)
		return (-EMSGSIZE);
	if ((ep = pp + *pp++) > e)
		return (-EMSGSIZE);
	if ((rtn = unpack_cdpa(p, e, m)) < 0)
		return (rtn);	/* CDPA V */
	if ((pp = p + *p++) > e)
		return (-EMSGSIZE);
	if ((ep = pp + *pp++) > e)
		return (-EMSGSIZE);
	if ((rtn = unpack_cgpa(p, e, m)) < 0)
		return (rtn);	/* CGPA V */
	if ((pp = p + *p++) > e)
		return (-EMSGSIZE);
	if ((ep = pp + *pp++) > e)
		return (-EMSGSIZE);
	if ((rtn = unpack_data(pp, ep, m)) < 0)
		return (rtn);	/* DATA V */
	m->imp = SCCP_DI_UDT;
	return (0);
}

/*
 *  SCCP_MT_UDTS  0x0a - Unitdata Service
 *  -------------------------------------------------------------------------
 *  ITUT: F(RETC), V(CDPA, CGPA, DATA[2-253])
 *  ANSI: F(RETC), V(CDPA, CGPA, DATA[2-251])
 */
STATIC INLINE int
sccp_dec_udts(uchar *p, uchar *e, struct sccp_msg *m)
{
	int rtn;
	uchar *pp, *ep;
	m->type = SCCP_MT_UDTS;
	m->parms = 0;
	if ((rtn = unpack_retc(p, e, m)) < 0)
		return (rtn);	/* RETC F */
	p += rtn;
	if ((pp = p + *p++) > e)
		return (-EMSGSIZE);
	if ((ep = pp + *pp++) > e)
		return (-EMSGSIZE);
	if ((rtn = unpack_cdpa(p, e, m)) < 0)
		return (rtn);	/* CDPA V */
	if ((pp = p + *p++) > e)
		return (-EMSGSIZE);
	if ((ep = pp + *pp++) > e)
		return (-EMSGSIZE);
	if ((rtn = unpack_cgpa(p, e, m)) < 0)
		return (rtn);	/* CGPA V */
	if ((pp = p + *p++) > e)
		return (-EMSGSIZE);
	if ((ep = pp + *pp++) > e)
		return (-EMSGSIZE);
	if ((rtn = unpack_data(pp, ep, m)) < 0)
		return (rtn);	/* DATA V */
	m->imp = SCCP_DI_UDTS;
	return (0);
}

/*
 *  SCCP_MT_XUDT  0x11 - Extended Unitdata
 *  -------------------------------------------------------------------------
 *  ITUT: F(PCLS, HOPC), V(CDPA, CGPA, DATA[2-253]), O(SGMT, IMP, EOP)
 *  ANSI: F(PCLS, HOPC), V(CDPA, CGPA, DATA[2-251]), O(SGMT, ISNI, INS, MTI, EOP)
 */
STATIC INLINE int
sccp_dec_xudt(uchar *p, uchar *e, struct sccp_msg *m)
{
	int rtn;
	uchar *pp, *ep;
	m->type = SCCP_MT_XUDT;
	m->parms = 0;
	if ((rtn = unpack_pcls(p, e, m)) < 0)
		return (rtn);	/* PCLS F */
	p += rtn;
	if (m->pcl != 0 && m->pcl != 1)
		return (-EPROTO);
	if ((rtn = unpack_hopc(p, e, m)) < 0)
		return (rtn);	/* HOPC F */
	p += rtn;
	if ((pp = p + *p++) > e)
		return (-EMSGSIZE);
	if ((ep = pp + *pp++) > e)
		return (-EMSGSIZE);
	if ((rtn = unpack_cdpa(p, e, m)) < 0)
		return (rtn);	/* CDPA V */
	if ((pp = p + *p++) > e)
		return (-EMSGSIZE);
	if ((ep = pp + *pp++) > e)
		return (-EMSGSIZE);
	if ((rtn = unpack_cgpa(p, e, m)) < 0)
		return (rtn);	/* CGPA V */
	if ((pp = p + *p++) > e)
		return (-EMSGSIZE);
	if ((ep = pp + *pp++) > e)
		return (-EMSGSIZE);
	if ((rtn = unpack_data(pp, ep, m)) < 0)
		return (rtn);	/* DATA V */
	if (*p)
		sccp_dec_opt(p + *p, e, m);
	if (!(m->parms & SCCP_PTF_IMP))
		m->imp = SCCP_DI_XUDT;
	return (0);
}

/*
 *  SCCP_MT_XUDTS 0x12 - Extended Unitdata Service
 *  -------------------------------------------------------------------------
 *  ITUT: F(RETC, HOPC), V(CDPA, CGPA, DATA[2-253]), O(SGMT, IMP, EOP)
 *  ANSI: F(RETC, HOPC), V(CDPA, CGPA, DATA[2-251]), O(SGMT, INS, MTI, ISNI, EOP)
 */
STATIC INLINE int
sccp_dec_xudts(uchar *p, uchar *e, struct sccp_msg *m)
{
	int rtn;
	uchar *pp, *ep;
	m->type = SCCP_MT_XUDTS;
	m->parms = 0;
	if ((rtn = unpack_retc(p, e, m)) < 0)
		return (rtn);	/* RETC F */
	p += rtn;
	if ((rtn = unpack_hopc(p, e, m)) < 0)
		return (rtn);	/* HOPC F */
	p += rtn;
	if ((pp = p + *p++) > e)
		return (-EMSGSIZE);
	if ((ep = pp + *pp++) > e)
		return (-EMSGSIZE);
	if ((rtn = unpack_cdpa(p, e, m)) < 0)
		return (rtn);	/* CDPA V */
	if ((pp = p + *p++) > e)
		return (-EMSGSIZE);
	if ((ep = pp + *pp++) > e)
		return (-EMSGSIZE);
	if ((rtn = unpack_cgpa(p, e, m)) < 0)
		return (rtn);	/* CGPA V */
	if ((pp = p + *p++) > e)
		return (-EMSGSIZE);
	if ((ep = pp + *pp++) > e)
		return (-EMSGSIZE);
	if ((rtn = unpack_data(pp, ep, m)) < 0)
		return (rtn);	/* DATA V */
	if (*p)
		sccp_dec_opt(p + *p, e, m);
	if (!(m->parms & SCCP_PTF_IMP))
		m->imp = SCCP_DI_XUDTS;
	return (0);
}

/*
 *  SCCP_MT_LUDT  0x13 - Long Unitdata
 *  -------------------------------------------------------------------------
 *  ITUT: F(PCLS, HOPC), V(CDPA, CGPA, LDATA[3-3906]), O(SGMT, IMP, EOP)
 *  ANSI: F(PCLS, HOPC), V(CDPA, CGPA, LDATA[3-3904]), O(SGMT, ISNI, INS, EOP)
 */
STATIC INLINE int
sccp_dec_ludt(uchar *p, uchar *e, struct sccp_msg *m)
{
	int rtn;
	uchar *pp, *ep;
	m->type = SCCP_MT_LUDT;
	m->parms = 0;
	if ((rtn = unpack_pcls(p, e, m)) < 0)
		return (rtn);	/* PCLS F */
	p += rtn;
	if (m->pcl != 0 && m->pcl != 1)
		return (-EPROTO);
	if ((rtn = unpack_hopc(p, e, m)) < 0)
		return (rtn);	/* HOPC F */
	p += rtn;
	if ((pp = p + *p++) > e)
		return (-EMSGSIZE);
	if ((ep = pp + *pp++) > e)
		return (-EMSGSIZE);
	if ((rtn = unpack_cdpa(p, e, m)) < 0)
		return (rtn);	/* CDPA V */
	if ((pp = p + *p++) > e)
		return (-EMSGSIZE);
	if ((ep = pp + *pp++) > e)
		return (-EMSGSIZE);
	if ((rtn = unpack_cgpa(p, e, m)) < 0)
		return (rtn);	/* CGPA V */
	if ((pp = p + *p++) + 1 > e)
		return (-EMSGSIZE);
	if ((ep = pp + ((*pp++) | ((*p++) << 8))) > e)
		return (-EMSGSIZE);
	if ((rtn = unpack_ldata(pp, ep, m)) < 0)
		return (rtn);	/* LDATA V */
	if (*p)
		sccp_dec_opt(p + *p, e, m);
	if (!(m->parms & SCCP_PTF_IMP))
		m->imp = SCCP_DI_LUDT;
	return (0);
}

/*
 *  SCCP_MT_LUDTS 0x14 - Long Unitdata Service
 *  -------------------------------------------------------------------------
 *  ITUT: F(RETC, HOPC), V(CDPA, CGPA, LDATA[3-3906]), O(SGMT, IMP, EOP)
 *  ANSI: F(RETC, HOPC), V(CDPA, CGPA, LDATA[3-3904]), O(SGMT, ISNI, INS, EOP)
 */
STATIC INLINE int
sccp_dec_ludts(uchar *p, uchar *e, struct sccp_msg *m)
{
	int rtn;
	uchar *pp, *ep;
	m->type = SCCP_MT_LUDTS;
	m->parms = 0;
	if ((rtn = unpack_retc(p, e, m)) < 0)
		return (rtn);	/* RETC F */
	p += rtn;
	if ((rtn = unpack_hopc(p, e, m)) < 0)
		return (rtn);	/* HOPC F */
	p += rtn;
	if ((pp = p + *p++) > e)
		return (-EMSGSIZE);
	if ((ep = pp + *pp++) > e)
		return (-EMSGSIZE);
	if ((rtn = unpack_cdpa(p, e, m)) < 0)
		return (rtn);	/* CDPA V */
	if ((pp = p + *p++) > e)
		return (-EMSGSIZE);
	if ((ep = pp + *pp++) > e)
		return (-EMSGSIZE);
	if ((rtn = unpack_cgpa(p, e, m)) < 0)
		return (rtn);	/* CGPA V */
	if ((pp = p + *p++) + 1 > e)
		return (-EMSGSIZE);
	if ((ep = pp + ((*pp++) | ((*p++) << 8))) > e)
		return (-EMSGSIZE);
	if ((rtn = unpack_ldata(pp, ep, m)) < 0)
		return (rtn);	/* LDATA V */
	if (*p)
		sccp_dec_opt(p + *p, e, m);
	if (!(m->parms & SCCP_PTF_IMP))
		m->imp = SCCP_DI_LUDTS;
	return (0);
}

/*
 *  SCMG Message
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
sccp_dec_scmg(uchar *p, uchar *e, struct sccp_msg *m)
{
	int rtn;
	if ((rtn = unpack_scmgfi(p, e, m)) < 0)
		return (rtn);	/* SCMG FI */
	p += rtn;
	if ((rtn = unpack_assn(p, e, m)) < 0)
		return (rtn);	/* ASSN */
	p += rtn;
	if ((rtn = unpack_apc(p, e, m)) < 0)
		return (rtn);	/* APC */
	p += rtn;
	if ((rtn = unpack_smi(p, e, m)) < 0)
		return (rtn);	/* SMI */
	p += rtn;
	if (p != e)
		return (-EMSGSIZE);
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SCCP Routing Functions
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sccp_recv_msg(queue_t *q, mblk_t *mp)
{
	int err;
	struct sc *sc = SCCP_PRIV(q);
	struct sccp_msg *m = (typeof(m)) mp->b_rptr;
	switch (m->type) {
	case SCCP_MT_CR:
		err = sccp_recv_cr(q, sc, mp, m);
		break;
	case SCCP_MT_CC:
		err = sccp_recv_cc(q, sc, mp, m);
		break;
	case SCCP_MT_CREF:
		err = sccp_recv_cref(q, sc, mp, m);
		break;
	case SCCP_MT_RLSD:
		err = sccp_recv_rlsd(q, sc, mp, m);
		break;
	case SCCP_MT_RLC:
		err = sccp_recv_rlc(q, sc, mp, m);
		break;
	case SCCP_MT_DT1:
		err = sccp_recv_dt1(q, sc, mp, m);
		break;
	case SCCP_MT_DT2:
		err = sccp_recv_dt2(q, sc, mp, m);
		break;
	case SCCP_MT_AK:
		err = sccp_recv_ak(q, sc, mp, m);
		break;
	case SCCP_MT_UDT:
		err = sccp_recv_udt(q, sc, mp, m);
		break;
	case SCCP_MT_UDTS:
		err = sccp_recv_udts(q, sc, mp, m);
		break;
	case SCCP_MT_ED:
		err = sccp_recv_ed(q, sc, mp, m);
		break;
	case SCCP_MT_EA:
		err = sccp_recv_ea(q, sc, mp, m);
		break;
	case SCCP_MT_RSR:
		err = sccp_recv_rsr(q, sc, mp, m);
		break;
	case SCCP_MT_RSC:
		err = sccp_recv_rsc(q, sc, mp, m);
		break;
	case SCCP_MT_ERR:
		err = sccp_recv_err(q, sc, mp, m);
		break;
	case SCCP_MT_IT:
		err = sccp_recv_it(q, sc, mp, m);
		break;
	case SCCP_MT_XUDT:
		err = sccp_recv_xudt(q, sc, mp, m);
		break;
	case SCCP_MT_XUDTS:
		err = sccp_recv_xudts(q, sc, mp, m);
		break;
	case SCCP_MT_LUDT:
		err = sccp_recv_ludt(q, sc, mp, m);
		break;
	case SCCP_MT_LUDTS:
		err = sccp_recv_ludts(q, sc, mp, m);
		break;
	default:
		/* 
		   any message with an unknown message type is discarded */
		err = -ENOPROTOOPT;
		break;
	}
	return (err);
}

STATIC int
sccp_recv_scmg(queue_t *q, struct sp *sp, mblk_t *mp, struct sccp_msg *m)
{
	int err;
	struct sr *sr = NULL;
	struct rs *rs = NULL;
	uchar *p, *e;
	if (!(m->parms & (SCCP_PTF_DATA | SCCP_PTF_LDATA)))
		goto eproto;
	p = m->data.ptr;
	e = p + m->data.len;
	if ((err = sccp_dec_scmg(p, e, m)) < 0)
		goto error;
	if (m->apc) {
		if (!(sr = sccp_lookup_sr(sp, m->apc, 1)))
			goto discard;
		if (m->assn)
			if (!(rs = sccp_lookup_rs(sr, m->assn, 1)))
				goto discard;
	}
	switch (m->fi) {
	case SCMG_MT_SSA:
		err = scmg_recv_ssa(q, sp, sr, rs, mp, m);
		break;
	case SCMG_MT_SSP:
		err = scmg_recv_ssp(q, sp, sr, rs, mp, m);
		break;
	case SCMG_MT_SST:
		err = scmg_recv_sst(q, sp, sr, rs, mp, m);
		break;
	case SCMG_MT_SOR:
		err = scmg_recv_sor(q, sp, sr, rs, mp, m);
		break;
	case SCMG_MT_SOG:
		err = scmg_recv_sog(q, sp, sr, rs, mp, m);
		break;
	case SCMG_MT_SSC:
		err = scmg_recv_ssc(q, sp, sr, rs, mp, m);
		break;
	case SCMG_MT_SBR:
		err = scmg_recv_sbr(q, sp, sr, rs, mp, m);
		break;
	case SCMG_MT_SNR:
		err = scmg_recv_snr(q, sp, sr, rs, mp, m);
		break;
	case SCMG_MT_SRT:
		err = scmg_recv_srt(q, sp, sr, rs, mp, m);
		break;
	}
	/* 
	   any message with an unknown message type is discarded */
	err = -ENOPROTOOPT;
	goto error;
      eproto:
	err = -EPROTO;
	rare();
	goto error;
      discard:
	err = QR_DONE;
	rare();
	goto error;
      error:
	return (err);
}

/*
 *  SCCP GTT ROUTING
 *  -------------------------------------------------------------------------
 */
#if 0
STATIC int
sccp_grte_cr(queue_t *q, struct sp *src, mblk_t *mp, struct sccp_msg *m)
{
	int err, refc;
	struct sp *dst;
	ulong smi, cong;
	/* 
	   translate and handle hop counter */
	if (m->flags & SCCPF_HOPC_VIOLATION)
		goto sccp_hopc_violation;
	if (m->flags & SCCPF_XLAT_FAILURE)
		goto no_address_type_translation;
	if ((dst = sccp_lookup_sp(src->add.ni, m->cdpa.pc, 0))) {
		struct ss *ss;
		struct sc *sc;
		switch (m->cdpa.ri) {
		case SCCP_RI_DPC_SSN:
			if (dst->flags & SCCPF_PROHIBITED)
				goto destination_inaccessible;
			if (dst->flags & (SCCPF_UNEQUIPPED | SCCPF_INACCESSIBLE))
				goto sccp_failure;
			if (dst->flags & SCCPF_CONGESTED) {
				if (src->proto.popt & SS7_POPT_MPLEV) {
					if (m->rl.mp < dst->level)
						goto access_congestion;
				} else if (m->imp <= dst->level)
					if (m->imp < dst->level
					    || (dst->count++ & 0x3) < dst->level)
						goto access_congestion;
			}
			smi = 0;
			cong = 0;
			if (!(ss = sccp_lookup_ss(dst, m->cdpa.ssn, 0)))
				goto unequipped_user;
			smi = ss->smi;
			cong = ss->level;
			if ((ss->flags & SCCPF_PROHIBITED) || !(sc = ss->cr.list))
				goto subsystem_failure;
			if ((ss->flags & SCCPF_CONGESTED) && (ss->count++ & 0x7))
				goto subsystem_congestion;
			if (!canput(sc->oq))
				goto subsystem_congestion;
			ss7_oput(sc->oq, mp);
			return (QR_DONE);
		case SCCP_RI_GT:
			if (src == dst)
				/* 
				   inform maintenance of GTT loop */
				goto unqualified;
			if (!(sc = dst->gt.list))
				goto access_failure;
			if (!canput(sc->oq))
				goto access_congestion;
			ss7_oput(sc->oq, mp);
			return (QR_DONE);
		}
	} else {
		struct sr *sr;
		struct rs *rs;
		struct mt *mt;
		if (!(sr = sccp_lookup_sr(src, m->cdpa.pc, 1)))
			goto destination_address_unknown;
		if (sr->flags & SCCPF_PROHIBITED)
			goto destination_inaccessible;
		if (sr->flags & (SCCPF_UNEQUIPPED | SCCPF_INACCESSIBLE))
			goto sccp_failure;
		if (sr->flags & SCCPF_CONGESTED) {
			if (src->proto.popt & SS7_POPT_MPLEV) {
				if (m->rl.mp < sr->level)
					goto access_congestion;
			} else if (m->imp <= sr->level)
				if (m->imp < sr->level || (sr->count++ & 0x3) < sr->level)
					goto access_congestion;
		}
		if (m->cdpa.ri == SCCP_RI_DPC_SSN) {
			smi = 0;
			cong = 0;
			if (!(rs = sccp_lookup_rs(sr, m->cdpa.ssn, 1)))
				goto unequipped_user;
			smi = rs->smi;
			cong = rs->level;
			if ((rs->flags & SCCPF_PROHIBITED))
				goto subsystem_failure;
			if ((rs->flags & SCCPF_CONGESTED) && (rs->count++ & 0x7))
				goto subsystem_congestion;
		}
		if (!(mt = sr->mt))
			goto unqualified;
		if (canput(mt->oq))
			goto unqualified;
		if (m->flags & SCCPF_ASSOC_REQUIRED) {
			/* 
			   create coupling */
			/* 
			   process cr on coupling */
			fixme(("Write this function\n"));
			return (-EFAULT);
		} else {
			ss7_oput(mt->oq, mp);
			return (QR_ABSORBED);
		}
	}
      sccp_failure:
	refc = SCCP_REFC_SCCP_FAILURE;
	ptrace(("%s: ERROR: SCCP failure\n", DRV_NAME));
	goto do_refc;
      sccp_hopc_violation:
	refc = SCCP_REFC_SCCP_HOP_COUNTER_VIOLATION;
	ptrace(("%s: ERROR: hop counter violation\n", DRV_NAME));
	goto do_refc;
      no_address_type_translation:
	refc = SCCP_REFC_NO_ADDRESS_TYPE_TRANSLATION;
	ptrace(("%s: ERROR: no address type translation\n", DRV_NAME));
	goto do_refc;
      unqualified:
	/* 
	   inform maintenance of GTT loop */
	refc = SCCP_REFC_UNQUALIFIED;
	ptrace(("%s: ERROR: GT translation loop\n", DRV_NAME));
	goto do_refc;
      access_failure:
	refc = SCCP_REFC_ACCESS_FAILURE;
	ptrace(("%s: ERROR: access failure\n", DRV_NAME));
	goto do_refc;
      access_congestion:
	refc = SCCP_REFC_ACCESS_CONGESTION;
	ptrace(("%s: ERROR: access congestion\n", DRV_NAME));
	goto do_refc;
      unequipped_user:
	refc = SCCP_REFC_UNEQUIPPED_USER;
	ptrace(("%s: ERROR: subsystem unequipped\n", DRV_NAME));
	goto do_ssp;
      subsystem_failure:
	refc = SCCP_REFC_SUBSYSTEM_FAILURE;
	ptrace(("%s: ERROR: subsystem failure\n", DRV_NAME));
	goto do_ssp;
      subsystem_congestion:
	refc = SCCP_REFC_SUBSYSTEM_CONGESTION;
	ptrace(("%s: ERROR: flow controlled\n", DRV_NAME));
	if (!(src->proto.popt & SS7_POPT_MPLEV))
		goto do_ssc;
	goto do_refc;
      destination_address_unknown:
	refc = SCCP_REFC_DESTINATION_ADDRESS_UNKNOWN;
	ptrace(("%s: ERROR: destination unknown\n", DRV_NAME));
	goto do_refc;
      destination_inaccessible:
	refc = SCCP_REFC_DESTINATION_INACCESSIBLE;
	ptrace(("%s: ERROR: \n", DRV_NAME));
	goto do_refc;
      do_ssc:
	if ((err =
	     sccp_send_ssc(q, src, m->rl.opc, &m->cdpa,
			   (m->parms & SCCP_PTF_CGPA) ? &m->cgpa : NULL, m->cdpa.ssn, m->cdpa.pc,
			   smi, cong)) < 0)
		return (err);
	goto do_refc;
      do_ssp:
	if ((err =
	     sccp_send_ssp(q, src, m->rl.opc, &m->cdpa,
			   (m->parms & SCCP_PTF_CGPA) ? &m->cgpa : NULL, m->cdpa.ssn, m->cdpa.pc,
			   smi)) < 0)
		return (err);
	goto do_refc;
      do_refc:
	/* 
	   invoke CREF procedures with REFC */
	if ((err =
	     sccp_send_cref(q, src, m->rl.opc, m->rl.sls, m->slr, refc,
			    (m->parms & SCCP_PTF_CGPA ? &m->cgpa : NULL), mp->b_cont,
			    (m->parms & SCCP_PTF_IMP ? &m->imp : NULL))) < 0)
		return (err);
	return (QR_DONE);
}
#endif

#if 0
STATIC int
sccp_grte_cl(queue_t *q, struct sp *src, mblk_t *mp, struct sccp_msg *m)
{
	int err, retc;
	struct sp *dst;
	ulong smi, cong;
	/* 
	   translate and handle hop counter */
	if (m->flags & SCCPF_HOPC_VIOLATION)
		goto sccp_hopc_violation;
	if (m->flags & SCCPF_XLAT_FAILURE)
		goto no_address_type_translation;
	if ((dst = sccp_lookup_sp(src->add.ni, m->cdpa.pc, 0))) {
		struct ss *ss;
		struct sc *sc;
		switch (m->cdpa.ri) {
		case SCCP_RI_DPC_SSN:
			if (dst->flags & SCCPF_PROHIBITED)
				goto mtp_failure;
			if (dst->flags & (SCCPF_UNEQUIPPED | SCCPF_INACCESSIBLE))
				goto sccp_failure;
			if (dst->flags & SCCPF_CONGESTED) {
				if (src->proto.popt & SS7_POPT_MPLEV) {
					if (m->rl.mp < dst->level)
						goto network_congestion;
				} else if (m->imp <= dst->level)
					if (m->imp < dst->level
					    || (dst->count++ & 0x3) < dst->level)
						goto network_congestion;
			}
			smi = 0;
			cong = 0;
			if (!(ss = sccp_lookup_ss(dst, m->cdpa.ssn, 0)))
				goto unequipped_user;
			smi = ss->smi;
			cong = ss->level;
			if ((ss->flags & SCCPF_PROHIBITED) || !(sc = ss->cl.list))
				goto subsystem_failure;
			if (!canput(sc->oq))
				goto subsystem_congestion;
			ss7_oput(sc->oq, mp);
			return (QR_DONE);
		case SCCP_RI_GT:
			if (src == dst)
				/* 
				   inform maintenance of GTT loop */
				goto unqualified;
			if (!(sc = dst->gt.list))
				goto no_address_translation;
			if (!canput(sc->oq))
				goto unqualified;
			ss7_oput(sc->oq, mp);
			return (QR_DONE);
		}
	} else {
		struct sr *sr;
		struct rs *rs;
		struct mt *mt;
		if (!(sr = sccp_lookup_sr(src, m->cdpa.pc, 1)))
			goto mtp_failure;
		if (sr->flags & SCCPF_PROHIBITED)
			goto mtp_failure;
		if (sr->flags & (SCCPF_UNEQUIPPED | SCCPF_INACCESSIBLE))
			goto sccp_failure;
		if (sr->flags & SCCPF_CONGESTED) {
			if (src->proto.popt & SS7_POPT_MPLEV) {
				if (m->rl.mp < sr->level)
					goto network_congestion;
			} else if (m->imp <= sr->level)
				if (m->imp < sr->level || (sr->count++ & 0x3) < sr->level)
					goto network_congestion;
		}
		smi = 0;
		cong = 0;
		if (m->cdpa.ri == SCCP_RI_DPC_SSN) {
			if (!(rs = sccp_lookup_rs(sr, m->cdpa.ssn, 1)))
				goto unequipped_user;
			smi = rs->smi;
			cong = rs->level;
			if (rs->flags & SCCPF_PROHIBITED)
				goto subsystem_failure;
			if (rs->flags & SCCPF_CONGESTED && (rs->count++ & 0x7))
				goto subsystem_congestion;
		}
		if (!(mt = sr->mt))
			goto local_processing_error;
		if (!canput(mt->oq))
			goto network_congestion;
		if (m->flags & SCCPF_CHANGE_REQUIRED) {
			/* 
			   change message */
			goto message_change_failure;
			goto segmentation_not_supported;
			goto segmentation_failure;
		}
		ss7_oput(mt->oq, mp);
		return (QR_DONE);
	}
      sccp_hopc_violation:
	retc = SCCP_RETC_SCCP_HOP_COUNTER_VIOLATION;
	ptrace(("%s: ERROR: SCCP hop counter violation\n", DRV_NAME));
	goto do_retc;
      no_address_type_translation:
	retc = SCCP_RETC_NO_ADDRESS_TYPE_TRANSLATION;
	ptrace(("%s: ERROR: no address type translation\n", DRV_NAME));
	goto do_retc;
      unqualified:
	retc = SCCP_RETC_UNQUALIFIED;
	ptrace(("%s: ERROR: unqualified\n", DRV_NAME));
	goto do_retc;
      no_address_translation:
	retc = SCCP_RETC_NO_ADDRESS_TRANSLATION;
	ptrace(("%s: ERROR: no address translation\n", DRV_NAME));
	goto do_retc;
      unequipped_user:
	retc = SCCP_RETC_UNEQUIPPED_USER;
	ptrace(("%s: ERROR: unequipped user\n", DRV_NAME));
	goto do_ssp;
      subsystem_congestion:
	retc = SCCP_RETC_SUBSYSTEM_CONGESTION;
	ptrace(("%s: ERROR: subsystem congestion\n", DRV_NAME));
	if (!(src->proto.popt & SS7_POPT_MPLEV))
		goto do_ssc;
	goto do_retc;
      mtp_failure:
	retc = SCCP_RETC_MTP_FAILURE;
	ptrace(("%s: ERROR: MTP failure\n", DRV_NAME));
	goto do_retc;
      sccp_failure:
	retc = SCCP_RETC_SCCP_FAILURE;
	ptrace(("%s: ERROR: SCCP failure\n", DRV_NAME));
	goto do_retc;
      subsystem_failure:
	retc = SCCP_RETC_SUBSYSTEM_FAILURE;
	ptrace(("%s: ERROR: subsystem failure\n", DRV_NAME));
	goto do_ssp;
      local_processing_error:
	retc = SCCP_RETC_LOCAL_PROCESSING_ERROR;
	ptrace(("%s: ERROR: local processing error\n", DRV_NAME));
	goto do_retc;
      network_congestion:
	retc = SCCP_RETC_NETWORK_CONGESTION;
	ptrace(("%s: ERROR: network congestion\n", DRV_NAME));
	goto do_retc;
      message_change_failure:
	retc = SCCP_RETC_MESSAGE_CHANGE_FAILURE;
	ptrace(("%s: ERROR: message change failure\n", DRV_NAME));
	goto do_retc;
      segmentation_not_supported:
	retc = SCCP_RETC_SEGMENTATION_NOT_SUPPORTED;
	ptrace(("%s: ERROR: segmentation not supported\n", DRV_NAME));
	goto do_retc;
      segmentation_failure:
	retc = SCCP_RETC_SEGMENTATION_FAILURE;
	ptrace(("%s: ERROR: segmentation failure\n", DRV_NAME));
	goto do_retc;
#if 0
      no_resassembly_at_destination:
	retc = SCCP_RETC_NO_REASSEMBLY_AT_DESTINATION;
	ptrace(("%s: ERROR: no reassembly at destination\n", DRV_NAME));
	goto do_retc;
#endif
      do_ssc:
	if ((err =
	     sccp_send_ssc(q, src, m->rl.opc, &m->cdpa,
			   (m->parms & SCCP_PTF_CGPA) ? &m->cgpa : NULL, m->cdpa.ssn, m->cdpa.pc,
			   smi, cong)) < 0)
		return (err);
	goto do_retc;
      do_ssp:
	if ((err =
	     sccp_send_ssp(q, src, m->rl.opc, &m->cdpa,
			   (m->parms & SCCP_PTF_CGPA) ? &m->cgpa : NULL, m->cdpa.ssn, m->cdpa.pc,
			   smi)) < 0)
		return (err);
	goto do_retc;
      do_retc:
	if ((1 << m->type) & (SCCP_MTF_UDTS | SCCP_MTF_XUDTS | SCCP_MTF_LUDTS)) {
		ptrace(("%s: INFO: Discarding service message\n", DRV_NAME));
		return (QR_DONE);
	}
	if (!m->ret) {
		ptrace(("%s: INFO: Discarding message with no return on error\n", DRV_NAME));
		return (QR_DONE);
	}
	switch (m->type) {
	case SCCP_MT_UDT:
		if ((err =
		     sccp_send_udts(q, src, m->rl.opc, m->rl.mp, m->rl.sls, retc,
				    ((m->parms & SCCP_PTF_CGPA) ? &m->cgpa : NULL),
				    ((m->parms & SCCP_PTF_CDPA) ? &m->cdpa : NULL),
				    mp->b_cont)) < 0)
			return (err);
		return (QR_ABSORBED);
	case SCCP_MT_XUDT:
		if ((err =
		     sccp_send_xudts(q, src, m->rl.opc, m->rl.mp, m->rl.sls, retc, m->hopc,
				     ((m->parms & SCCP_PTF_CGPA) ? &m->cgpa : NULL),
				     ((m->parms & SCCP_PTF_CDPA) ? &m->cdpa : NULL), mp->b_cont,
				     ((m->parms & SCCP_PTF_SGMT) ? &m->sgmt : NULL),
				     ((m->parms & SCCP_PTF_IMP) ? &m->imp : NULL),
				     ((m->parms & SCCP_PTF_INS) ? &m->ins : NULL),
				     ((m->parms & SCCP_PTF_MTI) ? &m->mti : NULL),
				     ((m->parms & SCCP_PTF_ISNI) ? &m->isni : NULL))) < 0)
			return (err);
		return (QR_ABSORBED);
	case SCCP_MT_LUDT:
		if ((err =
		     sccp_send_ludts(q, src, m->rl.opc, m->rl.mp, m->rl.sls, retc, m->hopc,
				     ((m->parms & SCCP_PTF_CGPA) ? &m->cgpa : NULL),
				     ((m->parms & SCCP_PTF_CDPA) ? &m->cdpa : NULL), mp->b_cont,
				     ((m->parms & SCCP_PTF_SGMT) ? &m->sgmt : NULL),
				     ((m->parms & SCCP_PTF_IMP) ? &m->imp : NULL),
				     ((m->parms & SCCP_PTF_ISNI) ? &m->isni : NULL),
				     ((m->parms & SCCP_PTF_INS) ? &m->ins : NULL))) < 0)
			return (err);
		return (QR_ABSORBED);
	}
	pswerr(("%s: SWERR: RoE procedure no UDTS\n", DRV_NAME));
	return (-EFAULT);
}
#endif

/*
 *  SCCP GTT ROUTING
 *  -------------------------------------------------------------------------
 *  Process the result of a global title translation.
 */
#if 0 
STATIC int
sccp_grte_msg(queue_t *q, mblk_t *mp)
{
	struct sc *sc = SCCP_PRIV(q);
	struct sccp_msg *m = (typeof(m)) mp->b_rptr;
	struct sp *sp = sc->sp.sp;
	if (sp) {
		if ((1 << m->type) & (SCCP_MTM_CL))
			return sccp_grte_cl(q, sp, mp, m);
		if ((1 << m->type) & (SCCP_MTF_CR))
			return sccp_grte_cr(q, sp, mp, m);
		pswerr(("%s: ERROR: routing failure\n", DRV_NAME));
		return (-EPROTO);
	}
	swerr();
	return (QR_DISABLE);	/* FIXME: should be hangup */
}
#endif

/*
 *  SCCP O/G ROUTING
 *  -------------------------------------------------------------------------
 */
STATIC int
sccp_orte_cr(queue_t *q, struct sc *sc, mblk_t *mp, struct sccp_msg *m)
{
	int err, refc;
	struct sp *src = sc->sp.sp;
	ulong dpc, smi = 0, cong = 0;
	if ((dpc = m->cdpa.pc) != -1UL || (dpc = m->rl.dpc) != -1UL) {
		struct sp *dst;
		if ((dst = sccp_lookup_sp(src->add.ni, dpc, 0))) {
			struct ss *ss;
			struct sc *sc;
			switch (m->cdpa.ri) {
			case SCCP_RI_DPC_SSN:
				if (dst->flags & SCCPF_PROHIBITED)
					goto destination_inaccessible;
				if (dst->flags & (SCCPF_UNEQUIPPED | SCCPF_INACCESSIBLE))
					goto sccp_failure;
				if (dst->flags & SCCPF_CONGESTED) {
					if (src->proto.popt & SS7_POPT_MPLEV) {
						if (m->rl.mp < dst->level)
							goto access_congestion;
					} else if (m->imp <= dst->level)
						if (m->imp < dst->level
						    || (dst->count++ & 0x3) < dst->level)
							goto access_congestion;
				}
				if (!(ss = sccp_lookup_ss(dst, m->cdpa.ssn, 0)))
					goto unequipped_user;
				smi = ss->smi;
				cong = ss->level;
				if ((ss->flags & SCCPF_PROHIBITED) || !(sc = ss->cr.list))
					goto subsystem_failure;
				if (!canput(sc->oq))
					goto subsystem_congestion;
				ss7_oput(sc->oq, mp);
				return (QR_DONE);
			case SCCP_RI_GT:
				goto do_gtt;
			}
		} else {
			struct sr *sr;
			struct mt *mt;
			if (!(sr = sccp_lookup_sr(src, dpc, 1)))
				goto destination_address_unknown;
			if (sr->flags & SCCPF_PROHIBITED)
				goto destination_inaccessible;
			if (sr->flags & (SCCPF_UNEQUIPPED | SCCPF_INACCESSIBLE))
				goto sccp_failure;
			if (sr->flags & SCCPF_CONGESTED) {
				if (src->proto.popt & SS7_POPT_MPLEV) {
					if (m->rl.mp < sr->level)
						goto access_congestion;
				} else if (m->imp <= sr->level)
					if (m->imp < sr->level || (sr->count++ & 0x3) < sr->level)
						goto access_congestion;
			}
			if (m->cdpa.ri == SCCP_RI_DPC_SSN) {
				struct rs *rs;
				if (!(rs = sccp_lookup_rs(sr, m->cdpa.ssn, 1)))
					goto unequipped_user;
				smi = rs->smi;
				cong = rs->level;
				if (rs->flags & SCCPF_PROHIBITED)
					goto subsystem_failure;
			}
			if (!(mt = sr->mt))
				goto local_processing_error;
			if (!canput(mt->oq))
				goto access_congestion;
			ss7_oput(mt->oq, mp);
			return (QR_DONE);
		}
	} else {
		struct sc *sc;
		if (m->cdpa.ri == SCCP_RI_DPC_SSN)
			goto access_failure;
	      do_gtt:
		if (!(sc = src->gt.list))
			goto access_failure;
		if (!canput(sc->oq))
			goto access_congestion;
		ss7_oput(sc->oq, mp);
		return (QR_DONE);
	}
      unequipped_user:
	refc = SCCP_REFC_UNEQUIPPED_USER;
	ptrace(("%s: ERROR: \n", DRV_NAME));
	goto do_ssp;
      subsystem_failure:
	refc = SCCP_REFC_SUBSYSTEM_FAILURE;
	ptrace(("%s: ERROR: \n", DRV_NAME));
	goto do_ssp;
      subsystem_congestion:
	refc = SCCP_REFC_SUBSYSTEM_CONGESTION;
	ptrace(("%s: ERROR: \n", DRV_NAME));
	if (!(src->proto.popt & SS7_POPT_MPLEV))
		goto do_ssc;
	goto do_refc;
      destination_address_unknown:
	refc = SCCP_REFC_DESTINATION_ADDRESS_UNKNOWN;
	ptrace(("%s: ERROR: \n", DRV_NAME));
	goto do_refc;
      destination_inaccessible:
	refc = SCCP_REFC_DESTINATION_INACCESSIBLE;
	ptrace(("%s: ERROR: \n", DRV_NAME));
	goto do_refc;
      sccp_failure:
	refc = SCCP_REFC_SCCP_FAILURE;
	ptrace(("%s: ERROR: \n", DRV_NAME));
	goto do_refc;
      local_processing_error:
	refc = SCCP_REFC_UNQUALIFIED;
	ptrace(("%s: ERROR: \n", DRV_NAME));
	goto do_refc;
      access_failure:
	refc = SCCP_REFC_ACCESS_FAILURE;
	ptrace(("%s: ERROR: \n", DRV_NAME));
	goto do_refc;
      access_congestion:
	refc = SCCP_REFC_ACCESS_CONGESTION;
	ptrace(("%s: ERROR: \n", DRV_NAME));
	goto do_refc;
      do_ssc:
	if ((err =
	     sccp_send_ssc(q, src, m->rl.opc, &m->cdpa,
			   (m->parms & SCCP_PTF_CGPA) ? &m->cgpa : NULL, m->cdpa.ssn, dpc, smi,
			   cong)) < 0)
		return (err);
	goto do_refc;
      do_ssp:
	if ((err =
	     sccp_send_ssp(q, src, m->rl.opc, &m->cdpa,
			   (m->parms & SCCP_PTF_CGPA) ? &m->cgpa : NULL, m->cdpa.ssn, dpc,
			   smi)) < 0)
		return (err);
	goto do_refc;
      do_refc:
	/* 
	   invoke CREF procedures with REFC */
	if ((err =
	     sccp_send_cref(q, src, m->rl.opc, m->rl.sls, m->slr, refc,
			    (m->parms & SCCP_PTF_CGPA ? &m->cgpa : NULL), mp->b_cont,
			    (m->parms & SCCP_PTF_IMP ? &m->imp : NULL))) < 0)
		return (err);
	return (QR_DONE);
}

STATIC int
sccp_orte_co(queue_t *q, struct sc *sc, mblk_t *mp, struct sccp_msg *m)
{
	int err, relc = 0;
	struct sr *sr;
	struct mt *mt;
	if (!(sr = sccp_lookup_sr(sc->sp.sp, sc->sp.sp->add.pc, 1)))
		goto mtp_failure;
	if (sr->flags & SCCPF_PROHIBITED)
		goto mtp_failure;
	if (sr->flags & (SCCPF_UNEQUIPPED | SCCPF_INACCESSIBLE))
		goto sccp_failure;
	if (sr->flags & SCCPF_CONGESTED) {
		if (sr->proto.popt & SS7_POPT_MPLEV) {
			if (m->rl.mp < sr->level)
				goto network_congestion;
		} else if (m->imp <= sr->level)
			if (m->imp < sr->level || (sr->count++ & 0x3) < sr->level)
				goto network_congestion;
	}
	if (!(mt = sr->mt))
		goto mtp_failure;
	if (!canput(mt->oq))
		goto network_congestion;
	ss7_oput(mt->oq, mp);
	return (QR_DONE);
#if 0
      access_failure:
	relc = SCCP_RELC_ACCESS_FAILURE;
	ptrace(("%s: ERROR: access failure\n", DRV_NAME));
	goto do_relc;
      access_congestion:
	relc = SCCP_RELC_ACCESS_CONGESTION;
	ptrace(("%s: ERROR: access congestion\n", DRV_NAME));
	goto do_relc;
      subsystem_failure:
	relc = SCCP_RELC_SUBSYSTEM_FAILURE;
	ptrace(("%s: ERROR: subsystem failure\n", DRV_NAME));
	goto do_relc;
      subsystem_congestion:
	relc = SCCP_RELC_SUBSYSTEM_CONGESTION;
	ptrace(("%s: ERROR: subsystem congestion\n", DRV_NAME));
	goto do_relc;
#endif
      mtp_failure:
	relc = SCCP_RELC_MTP_FAILURE;
	ptrace(("%s: ERROR: MTP failure\n", DRV_NAME));
	goto do_relc;
      network_congestion:
	relc = SCCP_RELC_NETWORK_CONGESTION;
	ptrace(("%s: ERROR: network congestion\n", DRV_NAME));
	goto do_relc;
#if 0
      unqualified:
	relc = SCCP_RELC_UNQUALIFIED;
	ptrace(("%s: ERROR: unqualified\n", DRV_NAME));
	goto do_relc;
#endif
      sccp_failure:
	relc = SCCP_RELC_SCCP_FAILURE;
	ptrace(("%s: ERROR: SCCP failure\n", DRV_NAME));
	goto do_relc;
      do_relc:
	if ((err =
	     sccp_send_rlsd(q, sc->sp.sp, m->rl.opc, sc->sls, sc->dlr, sc->slr, relc, mp->b_cont,
			    (m->parms & SCCP_PTF_IMP) ? &m->imp : NULL)) < 0)
		return (err);
	return (QR_DONE);
}

STATIC int
sccp_orte_cl(queue_t *q, struct sc *sc, mblk_t *mp, struct sccp_msg *m)
{
	int err, retc;
	struct sp *src = sc->sp.sp;
	ulong dpc, smi = 0, cong = 0;
	if ((dpc = m->cdpa.pc) != -1UL || (dpc = m->rl.dpc) != -1UL) {
		struct sp *dst;
		if ((dst = sccp_lookup_sp(src->add.ni, dpc, 0))) {
			struct ss *ss;
			struct sc *sc;
			switch (m->cdpa.ri) {
			case SCCP_RI_DPC_SSN:
				if (dst->flags & SCCPF_PROHIBITED)
					goto mtp_failure;
				if (dst->flags & (SCCPF_UNEQUIPPED | SCCPF_INACCESSIBLE))
					goto sccp_failure;
				if (dst->flags & SCCPF_CONGESTED) {
					if (src->proto.popt & SS7_POPT_MPLEV) {
						if (m->rl.mp < dst->level)
							goto network_congestion;
					} else if (m->imp <= dst->level)
						if (m->imp < dst->level
						    || (dst->count++ & 0x3) < dst->level)
							goto network_congestion;
				}
				if (m->cdpa.ssn == 1)
					goto do_scmg;
				if (!(ss = sccp_lookup_ss(dst, m->cdpa.ssn, 0)))
					goto unequipped_user;
				smi = ss->smi;
				cong = ss->level;
				if ((ss->flags & SCCPF_PROHIBITED) || !(sc = ss->cl.list))
					goto subsystem_failure;
				if (!canput(sc->oq))
					goto subsystem_congestion;
				ss7_oput(sc->oq, mp);
				return (QR_DONE);
			case SCCP_RI_GT:
				goto do_gtt;
			}
		} else {
			struct sr *sr;
			struct rs *rs;
			struct mt *mt;
			if (!(sr = sccp_lookup_sr(src, dpc, 1)))
				goto mtp_failure;
			if (sr->flags & SCCPF_PROHIBITED)
				goto mtp_failure;
			if (sr->flags & (SCCPF_UNEQUIPPED | SCCPF_INACCESSIBLE))
				goto sccp_failure;
			if (sr->flags & SCCPF_CONGESTED) {
				if (src->proto.popt & SS7_POPT_MPLEV) {
					if (m->rl.mp < sr->level)
						goto network_congestion;
				} else if (m->imp <= sr->level)
					if (m->imp < sr->level || (sr->count++ & 0x3) < sr->level)
						goto network_congestion;
			}
			if (m->cdpa.ri == SCCP_RI_DPC_SSN) {
				if (!(rs = sccp_lookup_rs(sr, m->cdpa.ssn, 1)))
					goto unequipped_user;	/* XXX */
				smi = rs->smi;
				cong = rs->level;
				if (rs->flags & SCCPF_PROHIBITED)
					goto subsystem_failure;
			}
			if (!(mt = sr->mt))
				goto mtp_failure;
			if (!canput(mt->oq))
				goto network_congestion;
			if (m->flags & SCCPF_CHANGE_REQUIRED) {
				/* 
				   change message */
			}
			ss7_oput(mt->oq, mp);
			return (QR_DONE);
		}
	} else {
		struct sc *sc;
		if (m->cdpa.ri == SCCP_RI_DPC_SSN)
			goto mtp_failure;
	      do_gtt:
		if (!(sc = src->gt.list))
			goto no_address_translation;
		if (!canput(sc->oq))
			goto unqualified;
		ss7_oput(sc->oq, mp);
		return (QR_DONE);
	}
      do_scmg:
	return sccp_recv_scmg(q, src, mp, m);
      unequipped_user:
	retc = SCCP_RETC_UNEQUIPPED_USER;
	printd(("%s: ERROR: unequipped user\n", DRV_NAME));
	goto do_ssp;
      subsystem_failure:
	retc = SCCP_RETC_SUBSYSTEM_FAILURE;
	printd(("%s: ERROR: subsystem failure\n", DRV_NAME));
	goto do_ssp;
      subsystem_congestion:
	retc = SCCP_RETC_SUBSYSTEM_CONGESTION;
	printd(("%s: ERROR: subsystem congestion\n", DRV_NAME));
	if (!(src->proto.popt & SS7_POPT_MPLEV))
		goto do_ssc;
	goto do_retc;
      mtp_failure:
	retc = SCCP_RETC_MTP_FAILURE;
	printd(("%s: ERROR: MTP failure\n", DRV_NAME));
	goto do_retc;
      sccp_failure:
	retc = SCCP_RETC_SCCP_FAILURE;
	printd(("%s: ERROR: SCCP failure\n", DRV_NAME));
	goto do_retc;
      network_congestion:
	retc = SCCP_RETC_NETWORK_CONGESTION;
	printd(("%s: ERROR: network congestion\n", DRV_NAME));
	goto do_retc;
      no_address_translation:
	retc = SCCP_RETC_NO_ADDRESS_TRANSLATION;
	printd(("%s: ERROR: no address translation\n", DRV_NAME));
	goto do_retc;
      unqualified:
	retc = SCCP_RETC_UNQUALIFIED;
	printd(("%s: ERROR: unqualified\n", DRV_NAME));
	goto do_retc;
      do_ssc:
	if ((err =
	     sccp_send_ssc(q, src, m->rl.opc, &m->cdpa,
			   (m->parms & SCCP_PTF_CGPA) ? &m->cgpa : NULL, m->cdpa.ssn, dpc, smi,
			   cong)) < 0)
		return (err);
	goto do_retc;
      do_ssp:
	if ((err =
	     sccp_send_ssp(q, src, m->rl.opc, &m->cdpa,
			   (m->parms & SCCP_PTF_CGPA) ? &m->cgpa : NULL, m->cdpa.ssn, dpc,
			   smi)) < 0)
		return (err);
	goto do_retc;
      do_retc:
	if ((1 << m->type) & (SCCP_MTF_UDTS | SCCP_MTF_XUDTS | SCCP_MTF_LUDTS)) {
		ptrace(("%s: INFO: Discarding service message\n", DRV_NAME));
		return (QR_DONE);
	}
	if (!m->ret) {
		ptrace(("%s: INFO: Discarding message with no return on error\n", DRV_NAME));
		return (QR_DONE);
	}
	switch (m->type) {
	case SCCP_MT_UDT:
		if ((err =
		     sccp_send_udts(q, src, m->rl.opc, m->rl.mp, m->rl.sls, retc,
				    ((m->parms & SCCP_PTF_CGPA) ? &m->cgpa : NULL),
				    ((m->parms & SCCP_PTF_CDPA) ? &m->cdpa : NULL),
				    mp->b_cont)) < 0)
			return (err);
		return (QR_ABSORBED);
	case SCCP_MT_XUDT:
		if ((err =
		     sccp_send_xudts(q, src, m->rl.opc, m->rl.mp, m->rl.sls, retc, m->hopc,
				     ((m->parms & SCCP_PTF_CGPA) ? &m->cgpa : NULL),
				     ((m->parms & SCCP_PTF_CDPA) ? &m->cdpa : NULL), mp->b_cont,
				     ((m->parms & SCCP_PTF_SGMT) ? &m->sgmt : NULL),
				     ((m->parms & SCCP_PTF_IMP) ? &m->imp : NULL),
				     ((m->parms & SCCP_PTF_INS) ? &m->ins : NULL),
				     ((m->parms & SCCP_PTF_MTI) ? &m->mti : NULL),
				     ((m->parms & SCCP_PTF_ISNI) ? &m->isni : NULL))) < 0)
			return (err);
		return (QR_ABSORBED);
	case SCCP_MT_LUDT:
		if ((err =
		     sccp_send_ludts(q, src, m->rl.opc, m->rl.mp, m->rl.sls, retc, m->hopc,
				     ((m->parms & SCCP_PTF_CGPA) ? &m->cgpa : NULL),
				     ((m->parms & SCCP_PTF_CDPA) ? &m->cdpa : NULL), mp->b_cont,
				     ((m->parms & SCCP_PTF_SGMT) ? &m->sgmt : NULL),
				     ((m->parms & SCCP_PTF_IMP) ? &m->imp : NULL),
				     ((m->parms & SCCP_PTF_ISNI) ? &m->isni : NULL),
				     ((m->parms & SCCP_PTF_INS) ? &m->ins : NULL))) < 0)
			return (err);
		return (QR_ABSORBED);
	}
	pswerr(("%s: SWERR: RoE procedure no UDTS\n", DRV_NAME));
	return (-EFAULT);
}

/*
 *  SCCP O/G ROUTING
 *  -------------------------------------------------------------------------
 *  This follows the routing rules for ANSI/ITU SCCP.  We process CL first for speed and then CO and lastly CR.
 *  This is in order of probability.
 */
STATIC int
sccp_orte_msg(queue_t *q, mblk_t *mp)
{
	struct sc *sc = SCCP_PRIV(q);
	struct sccp_msg *m = (typeof(m)) mp->b_rptr;
	struct sp *sp = sc->sp.sp;
	if (sp) {
		if ((1 << m->type) & (SCCP_MTM_CL))
			return sccp_orte_cl(q, sc, mp, m);
		if ((1 << m->type) & (SCCP_MTF_CR))
			return sccp_orte_cr(q, sc, mp, m);
		if ((1 << m->type) & (SCCP_MTM_CO))
			return sccp_orte_co(q, sc, mp, m);
		ptrace(("%s: ERROR: routing failure\n", DRV_NAME));
		return (-EPROTO);
	}
	swerr();
	return (QR_DISABLE);	/* FIXME: should be hangup */
}

/*
 *  SCCP ROUTING Connection Request (CR) Messages only
 *  -------------------------------------------------------------------------
 */
STATIC INLINE struct sc **
sccp_cr_lookup(struct sp *sp, ulong ssn)
{
	swerr();
	return (NULL);
}
STATIC int
sccp_irte_cr(queue_t *q, struct sp *sp, mblk_t *mp, struct sccp_msg *m)
{
	int err, refc;
	struct ss *ss;
	struct sc *sc;
	ulong smi, cong;
	switch (m->cdpa.ri) {
	case SCCP_RI_DPC_SSN:
		smi = 0;
		cong = 0;
		if (!(ss = sccp_lookup_ss(sp, m->cdpa.ssn, 0)))
			goto unequipped_user;
		smi = ss->smi;
		cong = ss->level;
		if (!(ss->flags & SCCPF_PROHIBITED))
			goto subsystem_failure;
		if (!(sc = ss->cr.list))
			goto subsystem_failure;
		if (!canput(sc->oq))
			goto subsystem_congestion;
		ss7_oput(sc->oq, mp);
		return (QR_DONE);
	case SCCP_RI_GT:
		if (!(sc = sp->gt.list))
			goto access_failure;
		if (!canput(sc->oq))
			goto access_congestion;
		ss7_oput(sc->oq, mp);
		return (QR_DONE);
	}
	swerr();
	return (-EFAULT);
      unequipped_user:
	refc = SCCP_REFC_UNEQUIPPED_USER;
	ptrace(("%s: ERROR: subsystem unequipped\n", DRV_NAME));
	goto do_ssp;
      subsystem_failure:
	refc = SCCP_REFC_SUBSYSTEM_FAILURE;
	ptrace(("%s: ERROR: subsystem failure\n", DRV_NAME));
	goto do_ssp;
      subsystem_congestion:
	refc = SCCP_REFC_SUBSYSTEM_CONGESTION;
	ptrace(("%s: ERROR: flow controlled\n", DRV_NAME));
	if (!(sp->proto.popt & SS7_POPT_MPLEV))
		goto do_ssc;
	goto do_refc;
      access_failure:
	refc = SCCP_REFC_ACCESS_FAILURE;
	ptrace(("%s: ERROR: GT Translation failure\n", DRV_NAME));
	goto do_refc;
      access_congestion:
	refc = SCCP_REFC_ACCESS_CONGESTION;
	ptrace(("%s: ERROR: GT Translation congested\n", DRV_NAME));
	goto do_refc;
      do_ssc:
	if ((err =
	     sccp_send_ssc(q, sp, m->rl.opc, &m->cdpa, (m->parms & SCCP_PTF_CGPA) ? &m->cgpa : NULL,
			   m->cdpa.ssn, m->cdpa.pc, smi, cong)) < 0)
		return (err);
	goto do_refc;
      do_ssp:
	if ((err =
	     sccp_send_ssp(q, sp, m->rl.opc, &m->cdpa, (m->parms & SCCP_PTF_CGPA) ? &m->cgpa : NULL,
			   m->cdpa.ssn, m->cdpa.pc, smi)) < 0)
		return (err);
	goto do_refc;
      do_refc:
	/* 
	   invoke CREF procedures with REFC */
	if ((err =
	     sccp_send_cref(q, sp, m->rl.opc, m->rl.sls, m->slr, refc,
			    (m->parms & SCCP_PTF_CGPA ? &m->cgpa : NULL), mp->b_cont,
			    (m->parms & SCCP_PTF_IMP ? &m->imp : NULL))) < 0)
		return (err);
	return (QR_DONE);
}

/*
 *  SCCP ROUTING Connection-Less Messages only
 *  -------------------------------------------------------------------------
 */
STATIC INLINE struct sc **
sccp_cl_lookup(struct sp *sp, ulong ssn)
{
	swerr();
	return (NULL);
}
STATIC int
sccp_irte_cl(queue_t *q, struct sp *sp, mblk_t *mp, struct sccp_msg *m)
{
	int err, retc;
	struct ss *ss;
	struct sc *sc;
	ulong smi, cong;
	switch (m->cdpa.ri) {
	case SCCP_RI_DPC_SSN:
		if (sp->flags & SCCPF_CONGESTED) {
			if (sp->proto.popt & SS7_POPT_MPLEV) {
				if (m->rl.mp < sp->level)
					goto network_congestion;
			} else if (m->imp <= sp->level)
				if (m->imp < sp->level || (sp->count++ & 0x3) < sp->level)
					goto network_congestion;
		}
		if (m->cdpa.ssn == 1)
			goto do_scmg;
		smi = 0;
		cong = 0;
		if (!(ss = sccp_lookup_ss(sp, m->cdpa.ssn, 0)))
			goto unequipped_user;
		smi = ss->smi;
		cong = ss->level;
		if ((ss->flags & SCCPF_PROHIBITED) || !(sc = ss->cl.list))
			goto subsystem_failure;
		if (!canput(sc->oq))
			goto subsystem_congestion;
		ss7_oput(sc->oq, mp);
		return (QR_DONE);
	case SCCP_RI_GT:
		if (!(sc = sp->gt.list))
			goto no_address_translation;
		if (!canput(sc->oq))
			goto subsystem_congestion2;
		ss7_oput(sc->oq, mp);
		return (QR_DONE);
	}
	swerr();
	return (-EFAULT);
      do_scmg:
	return sccp_recv_scmg(q, sp, mp, m);
      unequipped_user:
	retc = SCCP_RETC_UNEQUIPPED_USER;
	ptrace(("%s: ERROR: subsystem unequipped\n", DRV_NAME));
	goto do_ssp;
      subsystem_congestion:
	retc = SCCP_RETC_SUBSYSTEM_CONGESTION;
	ptrace(("%s: ERROR: subsystem congestion\n", DRV_NAME));
	if (!(sp->proto.popt & SS7_POPT_MPLEV))
		goto do_ssc;
	goto do_retc;
      subsystem_failure:
	retc = SCCP_RETC_SUBSYSTEM_FAILURE;
	ptrace(("%s: ERROR: subsystem failure\n", DRV_NAME));
	goto do_ssp;
      network_congestion:
	retc = SCCP_RETC_NETWORK_CONGESTION;
	ptrace(("%s: ERROR: network congestion\n", DRV_NAME));
	goto do_retc;
      subsystem_congestion2:
	retc = SCCP_RETC_SUBSYSTEM_CONGESTION;
	ptrace(("%s: ERROR: GT Translation congested\n", DRV_NAME));
	goto do_retc;
      no_address_translation:
	retc = SCCP_RETC_NO_ADDRESS_TRANSLATION;
	ptrace(("%s: ERROR: GT Translation unavailable\n", DRV_NAME));
	goto do_retc;
      do_ssc:
	if ((err =
	     sccp_send_ssc(q, sp, m->rl.opc, &m->cdpa, (m->parms & SCCP_PTF_CGPA) ? &m->cgpa : NULL,
			   m->cdpa.ssn, m->cdpa.pc, smi, cong)) < 0)
		return (err);
	goto do_retc;
      do_ssp:
	if ((err =
	     sccp_send_ssp(q, sp, m->rl.opc, &m->cdpa, (m->parms & SCCP_PTF_CGPA) ? &m->cgpa : NULL,
			   m->cdpa.ssn, m->cdpa.pc, smi)) < 0)
		return (err);
	goto do_retc;
      do_retc:
	if ((1 << m->type) & (SCCP_MTF_UDTS | SCCP_MTF_XUDTS | SCCP_MTF_LUDTS)) {
		ptrace(("%s: INFO: Discarding service message\n", DRV_NAME));
		return (QR_DONE);
	}
	if (!m->ret) {
		ptrace(("%s: INFO: Discarding message with no return on error\n", DRV_NAME));
		return (QR_DONE);
	}
	switch (m->type) {
	case SCCP_MT_UDT:
		if ((err =
		     sccp_send_udts(q, sp, m->rl.opc, m->rl.mp, m->rl.sls, retc,
				    ((m->parms & SCCP_PTF_CGPA) ? &m->cgpa : NULL),
				    ((m->parms & SCCP_PTF_CDPA) ? &m->cdpa : NULL),
				    mp->b_cont)) < 0)
			return (err);
		return (QR_ABSORBED);
	case SCCP_MT_XUDT:
		if ((err =
		     sccp_send_xudts(q, sp, m->rl.opc, m->rl.mp, m->rl.sls, retc, m->hopc,
				     ((m->parms & SCCP_PTF_CGPA) ? &m->cgpa : NULL),
				     ((m->parms & SCCP_PTF_CDPA) ? &m->cdpa : NULL), mp->b_cont,
				     ((m->parms & SCCP_PTF_SGMT) ? &m->sgmt : NULL),
				     ((m->parms & SCCP_PTF_IMP) ? &m->imp : NULL),
				     ((m->parms & SCCP_PTF_INS) ? &m->ins : NULL),
				     ((m->parms & SCCP_PTF_MTI) ? &m->mti : NULL),
				     ((m->parms & SCCP_PTF_ISNI) ? &m->isni : NULL))) < 0)
			return (err);
		return (QR_ABSORBED);
	case SCCP_MT_LUDT:
		if ((err =
		     sccp_send_ludts(q, sp, m->rl.opc, m->rl.mp, m->rl.sls, retc, m->hopc,
				     ((m->parms & SCCP_PTF_CGPA) ? &m->cgpa : NULL),
				     ((m->parms & SCCP_PTF_CDPA) ? &m->cdpa : NULL), mp->b_cont,
				     ((m->parms & SCCP_PTF_SGMT) ? &m->sgmt : NULL),
				     ((m->parms & SCCP_PTF_IMP) ? &m->imp : NULL),
				     ((m->parms & SCCP_PTF_ISNI) ? &m->isni : NULL),
				     ((m->parms & SCCP_PTF_INS) ? &m->ins : NULL))) < 0)
			return (err);
		return (QR_ABSORBED);
	}
	pswerr(("%s: SWERR: RoE procedure no UDTS\n", DRV_NAME));
	return (-EFAULT);
}

/*
 *  SCCP ROUTING Connection Oriented Messages (other than CR) only
 *  -------------------------------------------------------------------------
 */
STATIC INLINE struct sc **
sccp_co_lookup(struct sp *sp, ulong slr)
{
	swerr();
	return (NULL);
}
STATIC int
sccp_irte_co(queue_t *q, struct sp *sp, mblk_t *mp, struct sccp_msg *m)
{
	/* 
	   pass message to SCCP user */
	return (QR_DONE);
}

#if 0
STATIC int
sccp_irte_co(queue_t *q, struct sp *sp, mblk_t *mp, struct sccp_msg *m)
{
	int err;
	ulong relc;
	struct sr *sr;
	struct sc *sc, **scp;
	/* 
	   these are always route on DLR */
	if (!(scp = sccp_co_lookup(sp, m->dlr)) || !(sc = *scp))
		goto unassigned_dlr;
	/* 
	   we have a connected segment */
	if (((sc->pcl != 2) || (1 << m->type) & ~(SCCP_MTM_PCLS2))
	    && ((sc->pcl != 3) || (1 << m->type) & ~(SCCP_MTM_PCLS3)))
		goto unqualified_discard;
	if ((m->parms & SCCP_PTF_SLR) && m->slr != sc->slr)
		goto slr_mismatch;
	if (m->rl.opc != sc->opc.pc)
		goto pc_mismatch;
	/* 
	   Figure C.4/Q.714 Sheet 4 of 4: Construct an AK message with credit = 0 and send it,
	   restarting the send inactivity timer.  We need to mark that the connection is congested
	   as well so that when congestion ceases (i.e, when the service procedure runs and we are
	   ready to backenable, we will construct an AK message with credit = W and sent it,
	   restarting the inactivity timer). */
	/* 
	   Figure 2C/T1.112.4-2000 2of2: same. */
	if (!canput(sc->oq))
		goto ebusy;
	/* 
	   pass on for user */
	ss7_oput(sc->oq, mp);
	return (QR_ABSORBED);
      unassigned_dlr:
	/* 
	   Figure C.2/Q.714 Sheet 1 of 7 */
	/* 
	   Table B-1/T1.112.4-2000 case (a) */
	if ((1 << m->type) & (SCCP_MTF_CC | SCCP_MTF_RSR | SCCP_MTF_RSC))
		goto unassigned_dlrn;
	if ((1 << m->type) & ~(SCCP_MTF_RLSD))
		goto unqualified_discard2;
	goto unqualified_rlc;
      pc_mismatch:
	/* 
	   Table B-1/T1.112.4-2000 case (b) */
	if ((1 << m->type) & (SCCP_MTF_RSR | SCCP_MTF_RSC | SCCP_MTF_RLSD))
		goto point_code_mismatch_err;
	goto point_code_mismatch_discard;
      slr_mismatch:
	/* 
	   Figure C.2/Q.714 Sheet 1 of 7 */
	/* 
	   Table B-1/T1.112.4-2000 case (c) */
	if ((1 << m->type) & (SCCP_MTF_RSR | SCCP_MTF_RSC | SCCP_MTF_RLSD))
		goto inconsistent_slrn_err;
	if ((1 << m->type) & (SCCP_MTF_IT))
		goto inconsistent_connection_data;
	goto inconsistent_slrn_discard;
      point_code_mismatch_err:
	relc = SCCP_ERRC_POINT_CODE_MISMATCH;
	ptrace(("%s: ERROR: OPC mismatch (send ERR)\n", DRV_NAME));
	/* 
	   send ERR */
	goto do_err;
      point_code_mismatch_discard:
	relc = SCCP_ERRC_POINT_CODE_MISMATCH;
	ptrace(("%s: ERROR: OPC mismatch (discard)\n", DRV_NAME));
	/* 
	   discard */
	goto discard;
      inconsistent_slrn_err:
	relc = SCCP_ERRC_LRN_MISMATCH_INCONSISTENT_SOURCE_LRN;
	ptrace(("%s: ERROR: SLR mismatch (send ERR)\n", DRV_NAME));
	goto do_err;
      inconsistent_slrn_discard:
	relc = SCCP_ERRC_LRN_MISMATCH_INCONSISTENT_SOURCE_LRN;
	ptrace(("%s: ERROR: SLR mismatch (discard)\n", DRV_NAME));
	/* 
	   discard */
	goto discard;
      inconsistent_connection_data:
	relc = SCCP_RELC_INCONSISTENT_CONNECTION_DATA;
	ptrace(("%s: ERROR: SLR mismatch (send RLSD)\n", DRV_NAME));
	/* 
	   Send RLSD(2) */
	goto do_rlsd;
      unassigned_dlrn:
	relc = SCCP_ERRC_LRN_MISMATCH_UNASSIGNED_DEST_LRN;
	ptrace(("%s: ERROR: DLR unassigned (send ERR)\n", DRV_NAME));
	goto do_err;
      unqualified_rlc:
	relc = SCCP_RELC_UNQUALIFIED;
	ptrace(("%s: ERROR: DLR unassigned (send RLC)\n", DRV_NAME));
	/* 
	   Send RLC */
	goto do_rlc;
      unqualified_discard:
	relc = SCCP_ERRC_UNQUALIFIED;
	ptrace(("%s: ERROR: inconsistent CO msg type (discard)\n", DRV_NAME));
	/* 
	   discard */
	goto discard;
      unqualified_discard2:
	relc = SCCP_RELC_UNQUALIFIED;
	ptrace(("%s: ERROR: DLR unassigned (discard)\n", DRV_NAME));
	goto discard;
      ebusy:
	relc = -ENOSR;
	ptrace(("%s: ERROR: flow controlled (dropping)\n", DRV_NAME));
	goto discard;
      discard:
	return (QR_DONE);
      do_err:
	if ((err = sccp_send_err(q, sc->sp.sp, sc->sp.sp->add.pc, m->rl.sls, m->slr, relc)))
		goto error;
	goto discard;
      do_rlsd:
	if ((err =
	     sccp_send_rlsd(q, sc->sp.sp, sc->sp.sp->add.pc, m->rl.sls, m->slr, m->dlr, relc, NULL,
			    (m->parms & SCCP_PTF_IMP) ? &m->imp : NULL)))
		goto error;
	goto discard;
      do_rlc:
	if ((err = sccp_send_rlc(q, sc->sp.sp, sc->sp.sp->add.pc, m->rl.sls, m->slr, m->dlr)))
		goto error;
	goto discard;
      enomem:
	err = -ENOMEM;
	rare();
	goto error;
      error:
	return (err);
}
#endif

/*
 *  SCCP I/C ROUTING
 *  -------------------------------------------------------------------------
 *  This follows the routing rules for ANSI/ITU SCCP.  We process CL first for speed and then CO and lastly CR.
 *  This is in order of probability.
 */
STATIC int
sccp_irte_msg(queue_t *q, mblk_t *mp)
{
	struct mt *mt = MTP_PRIV(q);
	struct sccp_msg *m = (typeof(m)) mp->b_rptr;
	struct sp *sp = mt->sp ? mt->sp : (mt->sr ? mt->sr->sp.sp : NULL);
	if (sp) {
		if ((1 << m->type) & (SCCP_MTM_CL))
			return sccp_irte_cl(q, sp, mp, m);
		if ((1 << m->type) & (SCCP_MTF_CR))
			return sccp_irte_cr(q, sp, mp, m);
		if ((1 << m->type) & (SCCP_MTM_CO))
			return sccp_irte_co(q, sp, mp, m);
		ptrace(("%s: ERROR: routing failure\n", DRV_NAME));
		return (-EPROTO);
	}
	swerr();
	return (QR_DISABLE);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SCCP Interface Functions
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  SCCP BIND REQ
 *  -------------------------------------------------------------------------
 *  When we bind, we bind to a local Network Id, Point Code and Subsystem Number.  To accomplish this, we associate
 *  a small hash table on SSN with each of the lower MTP streams.  When we bind we look for the lower MTP stream
 *  which has the appropriate Network Id, Point Code (and Service Indicator(=3)).  If there is no upper stream bound
 *  for the subsystem number (or default listener) identified, the stream is bound and the position in the hash is
 *  filled and the upper stream linked into the lower stream's list.
 */
STATIC int
sccp_bind_req(queue_t *q, struct sc *sc, struct sccp_addr *src, ulong cons)
{
	int err;
	struct sp *sp;
	struct ss *ss;
	struct sc **scp;
	/* 
	   look for bound MTP provider */
	if (!(sp = sccp_lookup_sp(src->ni, src->pc, 1)))
		goto eaddrnotavail;
	if (!(ss = sccp_lookup_ss(sp, src->ssn, 1)))
		goto eaddrnotavail;
	switch (sc->pcl) {
	case 0:		/* protocol class 0 */
	case 1:		/* protocol class 1 */
		/* 
		   look for bind slot on provider */
		if (!(scp = sccp_cl_lookup(sp, src->ssn)) || (*scp))
			goto eaddrinuse;
		/* 
		   we have a CL slot, use it */
		if ((sc->sp.next = *scp))
			sc->sp.next->sp.prev = &sc->sp.next;
		sc->sp.prev = scp;
		*scp = sccp_get(sc);
		sc->sp.sp = sp_get(sp);
		/* 
		   bind to subsystem */
		if ((sc->ss.next = ss->cl.list))
			sc->ss.next->ss.prev = &sc->ss.next;
		sc->ss.prev = &ss->cl.list;
		ss->cl.list = sccp_get(sc);
		sc->ss.ss = ss_get(ss);
		break;
	case 2:		/* protocol class 2 */
	case 3:		/* protocol class 3 */
		if (cons) {	/* listening */
			if (!(scp = sccp_cr_lookup(sp, src->ssn)) || (*scp))
				goto eaddrinuse;
			/* 
			   we have a CR slot, use it */
			if ((sc->sp.next = *scp))
				sc->sp.next->sp.prev = &sc->sp.next;
			sc->sp.prev = scp;
			*scp = sccp_get(sc);
			sc->sp.sp = sp_get(sp);
			/* 
			   bind to subsystem */
			if ((sc->ss.next = ss->cr.list))
				sc->ss.next->ss.prev = &sc->ss.next;
			sc->ss.prev = &ss->cr.list;
			ss->cr.list = sccp_get(sc);
			sc->ss.ss = ss_get(ss);
		} else {	/* not listening */
			/* 
			   We don't bind non-listening streams because until they are assigned an
			   SLR, they are not in the hashes.  SLRs are only assigned when we enter
			   the SS_WCON_CREQ state from SS_IDLE or the SS_DATA_XFER state from
			   SS_WRES_CIND. */
			/* 
			   bind to subsystem */
			if ((sc->ss.next = ss->co.list))
				sc->ss.next->ss.prev = &sc->ss.next;
			sc->ss.prev = &ss->co.list;
			ss->co.list = sccp_get(sc);
			sc->ss.ss = ss_get(ss);
		}
		break;
	}
	return (0);
      eaddrinuse:
	err = -EADDRINUSE;
	ptrace(("%s: %p: ERROR: address already bound\n", DRV_NAME, sc));
	goto error;
      eaddrnotavail:
	err = -EADDRNOTAVAIL;
	ptrace(("%s: %p: ERROR: no such NI/PC pair available\n", DRV_NAME, sc));
	goto error;
      error:
	return (err);
}

/*
 *  SCCP UNBIND REQ
 *  -------------------------------------------------------------------------
 *  Here we just take the SCCP stream out of the bind, listen or established hashes for the MTP providers to which
 *  it was bound.
 */
STATIC int
sccp_unbind_req(queue_t *q, struct sc *sc)
{
	/* 
	   remove from bind/listen hash */
	if (sc->ss.ss) {
		if ((*sc->ss.prev = sc->ss.next))
			sc->ss.next->ss.prev = &sc->ss.next;
		sc->ss.next = NULL;
		sc->ss.prev = &sc->ss.next;
		ss_put(xchg(&sc->ss.ss, NULL));
		sccp_put(sc);
	}
	/* 
	   remove from established hash */
	if (sc->sp.sp) {
		if ((*sc->sp.prev = sc->sp.next))
			sc->sp.next->sp.prev = &sc->sp.next;
		sc->sp.next = NULL;
		sc->sp.prev = &sc->sp.next;
		sp_put(xchg(&sc->sp.sp, NULL));
		sccp_put(sc);
	}
	return (0);
}

/*
 *  SCCP CONN REQ
 *  -------------------------------------------------------------------------
 *  Launching a connection request is like a subsequent bind to a DLRN.  We select a new LRN for the connection and
 *  place ourselves in the LRN hashes for the MTP provider and send a Connection Request.
 */
#define SCCP_SLR_MAX_TRIES	10
STATIC int
sccp_conn_req(queue_t *q, struct sc *sc, struct sccp_addr *dst, ulong *sls, ulong *pri, ulong *pcl,
	      ulong *imp, mblk_t *dp)
{
	int err = 0;
	int tries;
	unsigned short slr = 0;		/* just to shut up compiler */
	struct sc **scp = NULL;		/* just to shut up compiler */
	struct sp *sp;
	if (sccp_get_state(sc) != SS_IDLE)
		goto efault;
	if (!(sp = sc->ss.ss->sp.sp))
		goto efault;
	if (sc->sp.sp)
		goto eisconn;
	/* 
	   Figure C.2/Q.714: Figure 2A/T1.112.4-2000: If not sufficient resources, return an
	   N_DISCON_IND indication or simply return an N_ERROR_ACK. */
	if (!sc->slr) {
		struct sr *sr;
		/* 
		   normal N_CONN_REQ */
		if (!(sr = sccp_lookup_sr(sp, sc->sp.sp->add.pc, 1)))
			goto enomem;
		/* 
		   Figure C.2/Q.714: Figure 2A/T1.112.4-2000 1of4: Assign a local reference and an
		   SLS to the connection section. Determine proposed protocol class and credit. */
		for (tries = SCCP_SLR_MAX_TRIES; tries; tries--) {
			/* 
			   IMPLEMENTAION NOTE: This is where we would do labelling and offset for
			   SUA.  There would need to be a fixed SLR portion mask and value.  We
			   would then only select SLRs which are within the variable portion.  For
			   now, this uses the entire SLR numbering space. */
			slr = (sp->sccp_next_slr++) & 0xffff;
			if ((scp = sccp_co_lookup(sp, slr)) && !(*scp))
				break;
		}
		if (*scp)
			goto eagain2;
		/* 
		   we have CO slot, use it */
		if ((sc->sp.next = *scp))
			sc->sp.next->sp.prev = &sc->sp.next;
		sc->sp.prev = scp;
		*scp = sccp_get(sc);
		sc->sp.sp = sp_get(sp);
		/* 
		   connect to remote signalling point */
		if ((sc->sr.next = sr->sc.list))
			sc->sr.next->sr.prev = &sc->sr.next;
		sc->sr.prev = &sr->sc.list;
		sr->sc.list = sccp_get(sc);
		sc->sr.sr = sr_get(sr);
		fixme(("Figure out HOPC\n"));
		fixme(("Set selected sls\n"));
		if (sls) {
			sc->sls = *sls;
		} else {
			sls = &sc->sls;
		}
		if (pri) {
			sc->mp = *pri;
			if (sc->mp > 1) {
				ptrace(("%s: %p: ERROR: user selected message priority > 1\n",
					DRV_NAME, sc));
				sc->mp = 1;
			}
		}
		if (pcl) {
			sc->pcl = *pcl;
			if (sc->pcl < 2) {
				ptrace(("%s: %p: ERROR: user selected protocol class < 2\n",
					DRV_NAME, sc));
				sc->pcl = 2;
			}
			if (sc->pcl > 3) {
				ptrace(("%s: %p: ERROR: user selected protocol class > 3\n",
					DRV_NAME, sc));
				sc->pcl = 3;
			}
		}
		if (imp) {
			sc->imp = *imp;
		}
		fixme(("Set selected slr\n"));
		if ((err =
		     sccp_send_cr(q, sp, sp->add.pc, sc->mp, sc->sls, sc->slr, sc->pcl, &sc->dst,
				  &sc->credit, &sc->src, dp, NULL, imp)))
			return (err);
	} else {
		/* 
		   Request Type 1 or Request Type 2 */
		/* 
		   Figure 2A/T1.112.4-2000 1of4: Request Type 1 has SLR, DLR, DPC, PCLASS and
		   credit already assigned (for permanent or semi-permanent connection). */
		/* 
		   Figure 2A/T1.112.4-2000 1of2: Request Type 2 has SLR, DLR, DPC, PCLASS and
		   credit already assigned (for permanent or semi-permanent connection). */
		slr = sc->slr;
		scp = sccp_co_lookup(sp, slr);
		if (scp && (*scp))
			goto eisconn;
	}
	sccp_timer_start(sc, tcon);
	sccp_set_state(sc, SS_WCON_CREQ);
	/* 
	   we have a blind slot, use it */
	sc->slr = slr;
	if ((sc->sp.next = *scp))
		sc->sp.next->sp.prev = &sc->sp.next;
	sc->sp.prev = scp;
	*scp = sccp_get(sc);
	sc->sp.sp = sp_get(sp);
	return (0);
      enomem:
	err = -ENOMEM;
	ptrace(("%s: %p: ERROR: insufficient memory\n", DRV_NAME, sc));
	goto error;
      eagain2:
	err = -EAGAIN;
	ptrace(("%s: %p: ERROR: couldn't assign SLR\n", DRV_NAME, sc));
	goto error;
      eisconn:
	err = -EISCONN;
	ptrace(("%s: %p: ERROR: user already connected\n", DRV_NAME, sc));
	goto error;
      efault:
	err = -EFAULT;
	ptrace(("%s: %p: SWERR: connection request in wrong state\n", DRV_NAME, sc));
	goto error;
      error:
	return (err);
}

/*
 *  SCCP CONN RES
 *  -------------------------------------------------------------------------
 */
STATIC int
sccp_conn_res(queue_t *q, struct sc *sc, mblk_t *cp, struct sc *ap, mblk_t *dp)
{
	int err;
	int tries;
	unsigned short slr;
	struct sc **scp = NULL;		/* just to shut up compiler */
	struct sccp_msg *m = (struct sccp_msg *) cp->b_rptr;
	struct sp *sp;
	struct sr *sr;
	if ((1 << sccp_get_state(sc)) & ~(SSF_IDLE | SSF_WRES_CIND))
		goto efault;
	if (!(sp = sc->ss.ss->sp.sp))
		goto efault;
	if (ap->sp.sp)
		goto eisconn;
	/* 
	   Figure 2B/T1.112.4-2000 1of2: update protocol class and credit, assign local
	   referenceand SLS for incoming connection section, send CC, start activity timers and
	   move tot he active state.

	   Figure C.3/Q.714 Sheet 2 of 6: assign local reference, sls, protocol class and credit
	   for the incoming connection section, send a CC, start inactivity timers, move to the
	   active state. */
	fixme(("We should really get the user's protocol class and\n"
	       "credit preferences from the N_CONN_RES options\n"));
	for (tries = 0; tries < SCCP_SLR_MAX_TRIES; tries++) {
		slr = (sp->sccp_next_slr++) & 0xffff;
		if ((scp = sccp_co_lookup(sp, slr)) && !(*scp))
			break;
	}
	if (*scp)
		goto eagain2;
	if (ap->sp.sp)
		goto eisconn2;
	if (!(sr = sccp_lookup_sr(sp, sc->sp.sp->add.pc, 1)))
		goto enomem;
	/* 
	   we have CO slot, use it */
	if ((ap->sp.next = *scp))
		ap->sp.next->sp.prev = &ap->sp.next;
	ap->sp.prev = scp;
	*scp = sccp_get(ap);
	ap->sp.sp = sp_get(sp);
	/* 
	   connect to remote signalling point */
	if ((ap->sr.next = sr->sc.list))
		ap->sr.next->sr.prev = &ap->sr.next;
	ap->sr.prev = &sr->sc.list;
	sr->sc.list = sccp_get(ap);
	ap->sr.sr = sr_get(sr);
	if ((ap->pcl = m->pcl) == 3)
		ap->p_cred = m->cred;
	ap->dlr = m->slr;
	fixme(("copy more stuff and rebind ap if necessary"));
	switch (ap->proto.pvar & SS7_PVAR_MASK) {
	case SS7_PVAR_ANSI:
		if ((err =
		     sccp_send_cc(q, ap->sp.sp, ap->sp.sp->add.pc, ap->sls, ap->dlr, ap->slr,
				  ap->pcl, &ap->credit, &ap->dst, dp, NULL)))
			return (err);
		break;
	default:
		if ((err =
		     sccp_send_cc(q, ap->sp.sp, ap->sp.sp->add.pc, ap->sls, ap->dlr, ap->slr,
				  ap->pcl, &ap->credit, &ap->dst, dp, &ap->imp)))
			return (err);
		break;
	}
	fixme(("Assign subsystem\n"));
	sccp_timer_start(ap, tias);
	sccp_timer_start(ap, tiar);
	ap->state = SS_DATA_XFER;
	return (QR_DONE);
      enomem:
	err = -ENOMEM;
	ptrace(("%s: %p: ERROR: insufficient memory\n", DRV_NAME, sc));
	goto error;
      eisconn2:
	err = -EISCONN;
	ptrace(("%s: %p: ERROR: user already connected\n", DRV_NAME, sc));
	goto error;
      eagain2:
	ptrace(("%s: %p: ERROR: slr assignment timed out\n", DRV_NAME, sc));
	return (-EAGAIN);
	goto error;
      eisconn:
	err = -EISCONN;
	ptrace(("%s: %p: ERROR: accepting stream already connected\n", DRV_NAME, sc));
	goto error;
      efault:
	err = -EFAULT;
	ptrace(("%s: %p: SWERR: connection response in wrong state\n", DRV_NAME, sc));
	goto error;
      error:
	return (err);
}

STATIC int
sccp_data_req(queue_t *q, struct sc *sc, ulong exp, ulong more, ulong rctp, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	struct sccp_msg *m;
	switch (sccp_get_state(sc)) {
	case SS_DATA_XFER:
		if (!(mp = ss7_allocb(q, sizeof(*m), BPRI_MED)))
			goto enobufs;
		mp->b_datap->db_type = M_DATA;
		mp->b_band = exp ? 1 : 0;
		m = ((typeof(m)) mp->b_wptr)++;
		bzero(m, sizeof(*m));
		m->type = (sc->pcl == 3) ? SCCP_MT_DT1 : SCCP_MT_DT2;
		fixme(("Fill out message structure\n"));
		m->rl.opc = sc->sp.sp->add.pc;
		m->rl.sls = sc->sls;
		m->rl.mp = sc->mp;
		m->parms = SCCP_PTF_DLR | SCCP_PTF_DATA;
		m->dlr = sc->dlr;
		m->data.ptr = dp->b_rptr;
		m->data.len = dp->b_wptr - dp->b_rptr;
		fixme(("Fill out message structure\n"));
		mp->b_cont = dp;
		/* 
		   Figure 2C/T1.112.4-2002 1of2: segment message, reset receive inactivity timer,
		   For class 3 peform sequence number function: if P(S) outside window, queue
		   segment, otherwise construct message and send it out.  For class 2 construct
		   message and send it out.  Do this for each segment. Figure C.4/Q.714 Sheet 1 of
		   4: We need to segment the NSDU if required and assign a value to bit M and place 
		   the data in the send buffer already segmented. For protocol class 3, for each
		   segment in the M-bit sequence, we need to assign the next value of value of P(S) 
		   to the data.  If there are already messages in the send buffer, or P(S) is
		   outside the sending window, we need to place the data at the end of the transmit 
		   queue.  Otherwise we can send the message immediately. For protocol class 2 and
		   when we send a protocol class 3 message immediately, we send a DT1(class 2) or
		   DT2(class 3) and restart the send inactivity timer. This operation is performed
		   for each message in the M-bit sequence. */
		bufq_queue(&sc->sndq, mp);
		return (QR_TRIMMED);
	case SS_BOTH_RESET:
		/* 
		   Figure 2E/T1.112.4-2000: dicard */
		/* 
		   Figure C.6/Q.714 Shee 2 of 4: discard */
		return (QR_DONE);
	case SS_WCON_RREQ:
		/* 
		   Figure 2E/T1.112.4-2002 2of2: queue received information. */
		/* 
		   Figure C.6/Q.714 Sheet 2 of 4: Received information (whatever that means: see
		   figure). */
		fixme(("Write this procedure\n"));
		return (-EFAULT);
	case SS_WRES_RIND:
		switch (sc->proto.pvar & SS7_PVAR_MASK) {
		case SS7_PVAR_ANSI:
			/* 
			   Figure 2E/T1.112.4-2000 2of2: discard received information. */
			return (QR_DONE);
		default:
			swerr();
			return (-EFAULT);
		}
	default:
		swerr();
		return (-EFAULT);
	}
      enobufs:
	err = -ENOBUFS;
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sc));
	goto error;
      error:
	return (err);
}

STATIC int
sccp_exdata_req(queue_t *q, struct sc *sc, ulong more, ulong rctp, mblk_t *dp)
{
	int err;
	switch (sccp_get_state(sc)) {
	case SS_DATA_XFER:
		/* 
		   Figure 2D/T1.112.4-2000: same. */
		/* 
		   Figure C.5/Q.714 Sheet 1 of 2: If there is an unacknowledged ED, queue the
		   message, otherwise send the ED and restart send inactivity timer. */
		if (sc->flags & SCCPF_LOC_ED) {
			bufq_queue(&sc->urgq, dp);
			return (QR_TRIMMED);
		}
		if ((err = sccp_send_ed(q, sc->sp.sp, sc->sp.sp->add.pc, sc->sls, sc->dlr, dp)))
			return (err);
		sccp_timer_start(sc, tias);
		return (QR_TRIMMED);
	case SS_BOTH_RESET:
		/* 
		   Figure 2E/T1.112.4-2000: dicard */
		/* 
		   Figure C.6/Q.714 Shee 2 of 4: discard */
		return (QR_DONE);
	case SS_WCON_RREQ:
		/* 
		   Figure 2E/T1.112.4-2002 2of2: queue received information. */
		/* 
		   Figure C.6/Q.714 Sheet 2 of 4: Received information (whatever that means: see
		   figure). */
		fixme(("Write this procedure\n"));
		return (-EFAULT);
	case SS_WRES_RIND:
		switch (sc->proto.pvar & SS7_PVAR_MASK) {
		case SS7_PVAR_ANSI:
			/* 
			   Figure 2E/T1.112.4-2000 2of2: discard received information. */
			return (QR_DONE);
		default:
			err = -EFAULT;
			swerr();
			return (err);
		}
	default:
		err = -EFAULT;
		swerr();
		return (err);
	}
}

STATIC int
sccp_datack_req(queue_t *q, struct sc *sc)
{
	int err;
	switch (sccp_get_state(sc)) {
	case SS_DATA_XFER:
		if (sc->flags & SCCPF_REM_ED) {
			/* 
			   we are acking expedited data */
			/* 
			   Figure 2D/T1.112.4-2000: same. */
			/* 
			   Figure C.5/Q.714 Sheet 1 of 2: Send and EA and restart the send
			   inactivity timer. */
			if ((err = sccp_send_ea(q, sc->sp.sp, sc->sp.sp->add.pc, sc->sls, sc->dlr)))
				return (err);
			sccp_timer_start(sc, tias);
			sc->flags &= ~SCCPF_REM_ED;
			return (QR_DONE);
		} else {
			/* 
			   we are acking normal data */
			if (bufq_size(&sc->ackq)) {	/* we must have data to ack */
				/* 
				   Perform the processing deferred from when we received the data
				   message and queued it on the ack queue

				   Figure C.4/Q.714 Sheet 2 of 4: If the received P(S) is not the
				   next in sequence, or P(R) is outside the range from the last
				   received P(R) to the last P(S) sent plus 1, the an internal
				   reset (see figure) is peformed.

				   Otherwise, perform a Transmit Wakeup: If the received P(R) is at 
				   the lower edge of the sending window, the sending window is set
				   to the value of P(R). While we have a message in the transmit
				   queue, (we are flow controlled on the sending side) and the P(S) 
				   assigned to the message in queue is inside the send window, we
				   set P(R) to the last received P(S) plus 1 and send another of
				   the queued data messages and restart the send inactivity timer.
				   In this way we clear any backlog of messages once we receive
				   acknowledgement that opens the window.

				   Otherwise, if the received P(S) is equal to the upper edge of
				   the window, or if we need to send a data ack (acknowledged
				   service) then we construct a data ack. message with credit = W
				   and send it, restarting the send inactivity timer.

				   After this operation, we deliver the data to the user in an
				   N_DATA_IND.

				   IMPLEMENTATION NOTE:- We allow the user to explicitly
				   acknowledge receipt of data messages in protocol class 3.  To do 
				   this, we queue the message to an ack queue rather than
				   performing the P(S) P(R) processing on it yet. We postpone this
				   processing until a data ack request is received from the user. */
				fixme(("Write this procedure\n"));
				return (-EFAULT);
			}
		}
	default:
		err = -EFAULT;
		swerr();
		return (err);
	}
}

STATIC int
sccp_reset_req(queue_t *q, struct sc *sc)
{
	switch (sccp_get_state(sc)) {
	case SS_DATA_XFER:
		/* 
		   Note: ANSI returns an N_RESET_CON immediately here.  That is strange. */
		switch (sc->proto.pvar & SS7_PVAR_MASK) {
		case SS7_PVAR_ANSI:
			/* 
			   Figure 2E/T1.112.4-2000 1of2: return N_RESET_CON, send RSR, start reset
			   timer, reset variables and discard all queued messages and
			   unacknowledged messages, move to the reset outgoing state. */
			fixme(("SWERR: write this function\n"));
			sccp_set_state(sc, SS_WCON_RREQ);
			return (-EFAULT);
		default:
		case SS7_PVAR_ITUT:
			/* 
			   Figure C.6/Q.714 Sheet 1 of 4: Send a reset request message to the perr, 
			   start the reset timer, restart the send inactivity timer, reset
			   variables and discard all queued and unacknowledged messages. Move to
			   the reset ongoing state. */
			fixme(("SWERR: write this function\n"));
			sccp_set_state(sc, SS_WCON_RREQ);
			return (-EFAULT);
		}
	case SS_BOTH_RESET:
		/* 
		   Figure 2E/T1.112.4-2000: same but no "received information" function. */
		/* 
		   Figure C.6/Q.714 Sheet 3 of 4: Received information (whatever that means), and
		   move to the reset outgoing state. */
		fixme(("Do received information function\n"));
		/* 
		   FIXME: We don't want to lose track of the fact that the reset was a both way
		   reset and not to send an N_RESET_CON to the user when the reset completes (for
		   ITU).  This state transition alone doesn't cut it. */
		sccp_set_state(sc, SS_WCON_RREQ);
		return (QR_DONE);
	case SS_WRES_RIND:
		switch (sc->proto.pvar & SS7_PVAR_MASK) {
		case SS7_PVAR_ANSI:
			/* 
			   Figure 2E/T1.112.4-2000 2of2: transfer all queued information, move to
			   the active state. */
			fixme(("Transfer all queued information\n"));
			sccp_set_state(sc, SS_DATA_XFER);
			return (QR_DONE);
		default:
		case SS7_PVAR_ITUT:
			/* 
			   Figure C.6/Q.714 Sheet 4 of 4: received information (whatever that
			   means). */
			fixme(("Received information\n"));
			return (QR_DONE);
		}
	case SS_WCON_RREQ:
		switch (sc->proto.pvar & SS7_PVAR_MASK) {
		case SS7_PVAR_ANSI:
			/* 
			   Figure 2E/T1.112.4-2002 2of2: send N_RESET_CON to the user, discard all
			   queue and unacknowledged messages. */
			fixme(("Write this function\n"));
			return (-EFAULT);
		default:
			swerr();
			return (-EFAULT);
		}
	default:
		swerr();
		return (-EFAULT);
	}
}

STATIC int
sccp_reset_res(queue_t *q, struct sc *sc)
{
	switch (sccp_get_state(sc)) {
	case SS_BOTH_RESET:
		/* 
		   Figure 2E/T1.112.4-2000: same but no "received information" function. */
		/* 
		   Figure C.6/Q.714 Sheet 3 of 4: Received information (whatever that means), and
		   move to the reset outgoing state. */
		fixme(("Do received information function\n"));
		/* 
		   FIXME: We don't want to lose track of the fact that the reset was a both way
		   reset and not to send an N_RESET_CON to the user when the reset completes (for
		   ITU).  This state transition alone doesn't cut it. */
		sccp_set_state(sc, SS_WCON_RREQ);
		return (QR_DONE);
	case SS_WRES_RIND:
		switch (sc->proto.pvar & SS7_PVAR_MASK) {
		case SS7_PVAR_ANSI:
			/* 
			   Figure 2E/T1.112.4-2000 2of2: transfer all queued information, move to
			   the active state. */
			fixme(("Transfer all queued information\n"));
			sccp_set_state(sc, SS_DATA_XFER);
			return (QR_DONE);
		default:
		case SS7_PVAR_ITUT:
			/* 
			   Figure C.6/Q.714 Sheet 4 of 4: received information (whatever that
			   means). */
			fixme(("Received information\n"));
			return (QR_DONE);
		}
	default:
		swerr();
		return (-EFAULT);
	}
}

STATIC int
sccp_discon_req(queue_t *q, struct sc *sc, mblk_t *cp, mblk_t *dp)
{
	int err;
	if ((1 << sc->
	     state) & ~(SSF_IDLE | SSF_DATA_XFER | SSF_WRES_RIND | SSF_WCON_RREQ | SSF_WCON_CREQ |
			SSF_WRES_CIND))
		goto efault;
	switch (sccp_get_state(sc)) {
	case SS_IDLE:
	case SS_WRES_CIND:
	{
		/* 
		   Figure 2B/T1.112.4-2000 1of2: release allocated resources, send CREF, move to
		   the idle state. */
		/* 
		   Figure C.3/Q.714 Sheet 2 of 6: release resources, send CREF, move to the idle
		   state. */
		struct sccp_msg *m = (struct sccp_msg *) cp->b_rptr;
		fixme(("Select sr, but don't set sc->sr.sr\n"));
		sccp_send_cref(q, sc->sp.sp, sc->sp.sp->add.pc, m->rl.sls, m->slr,
			       SCCP_REFC_END_USER_ORIGINATED, &sc->dst, dp,
			       (m->parms & SCCP_PTF_IMP ? &m->imp : NULL));
		sccp_set_state(sc, SS_IDLE);
		return (QR_DONE);
	}
	case SS_WCON_CREQ:
		/* 
		   Figure C.2/Q.714 Sheet 2 of 7: Figure 2A/T1.112.4-2000 Sheet 2 of 4: We need to
		   remember the fact that we got a disconnect request while an outgoing connection
		   was pending and wait for a connection confirmation anyway. When we receive the
		   connection confirmation we will disconnect. */
		sccp_set_state(sc, SS_WCON_CREQ2);
		return (QR_DONE);
	case SS_DATA_XFER:
	{
		/* 
		   Figure C.2/Q.714 Sheet 4 of 7: Figure C.3/Q.714 Sheet 3 of 6: Stop inactivity
		   timers, send RLSD, start release timer, move to disconnect pending state. */
		/* 
		   Figure 2A/T1.112.4-2000 Sheet 4 of 4: release resources for the connection
		   section, stop inactivity timers, send RLSD, start released and interval timers. */
		/* 
		   Notes: Although ANSI says to release resources for the connection section, it
		   does not talk about releasing the local reference (which is needed to receive
		   the RLC). */
		sccp_timer_stop(sc, tias);
		sccp_timer_stop(sc, tiar);
		switch (sc->proto.pvar & SS7_PVAR_MASK) {
		case SS7_PVAR_ANSI:
			if ((err =
			     sccp_send_rlsd(q, sc->sp.sp, sc->sp.sp->add.pc, sc->sls, sc->dlr,
					    sc->slr, SCCP_RELC_END_USER_ORIGINATED, dp, NULL)))
				return (err);
			sccp_timer_start(sc, trel);
			sccp_timer_start(sc, tint);
			break;
		default:
			if ((err =
			     sccp_send_rlsd(q, sc->sp.sp, sc->sp.sp->add.pc, sc->sls, sc->dlr,
					    sc->slr, SCCP_RELC_END_USER_ORIGINATED, dp, &sc->imp)))
				return (err);
			sccp_timer_start(sc, trel);
			break;
		}
		sccp_set_state(sc, SS_WCON_DREQ);
		return (QR_DONE);
	}
	}
      efault:
	fixme(("SWERR: write this function\n"));
	return (-EFAULT);
}

STATIC int
sccp_ordrel_req(queue_t *q, struct sc *sc)
{
	fixme(("SWERR: write this function\n"));
	return (-EFAULT);
}

STATIC int
sccp_unitdata_req(queue_t *q, struct sc *sc, struct sccp_addr *dst, ulong pcl, ulong opt, ulong imp,
		  ulong seq, ulong pri, mblk_t *dp)
{
	/* 
	   Figure 3/T1.112.4-2000 1of2: assign an sls based on the sequence parameter in the
	   N-UNITDATA Request, construct an [X/L]UDT message, deciding which message to use based
	   on local information, decide whether segmentation is necessary and if necessary segment
	   the message, send the resulting messages to routing control for routing */
	/* 
	   IMPLEMENTATION NOTE:- Actually we don't care at this point whether the message needs to
	   be segmented or not: we will decide that after routing and perform segmentation during
	   routing if necessary.  Also, we build just a UDT here.  If global title is performed and 
	   a hop counter is required, the message will be converted into an XUDT.  If segmentation
	   is required the message will be converted into an XUDT/LUDT when the need for
	   segementation is determined after routing. */
	fixme(("SWERR: write this function\n"));
	return (-EFAULT);
}

STATIC int
sccp_inform_req(queue_t *q, struct sc *sc, N_qos_sel_infr_sccp_t * qos, ulong reason)
{
	fixme(("SWERR: write this function\n"));
	return (-EFAULT);
}

STATIC int
sccp_coord_req(queue_t *q, struct sc *sc, struct sccp_addr *add)
{
	fixme(("SWERR: write this function\n"));
	return (-EFAULT);
}

STATIC int
sccp_coord_res(queue_t *q, struct sc *sc, struct sccp_addr *add)
{
	fixme(("SWERR: write this function\n"));
	return (-EFAULT);
}

#define SSNF_IGNORE_SST_IN_PROGRESS	0x0001
#define SSNF_WAIT_FOR_GRANT		0x0002
STATIC int
sccp_state_req(queue_t *q, struct sc *sc, struct sccp_addr *add, ulong status)
{
	int err;
	struct na *na;
	struct sp *sp;
	struct ss *ss;
	ulong smi = 0;			/* FIXME */
	if (!add->ni || !(na = na_lookup(add->ni)))
		goto enoaddr;
	if (!add->pc || !(sp = sccp_lookup_sp(add->ni, add->pc, 1)))
		goto enoaddr;
	if (!add->ssn || !(ss = sccp_lookup_ss(sp, add->ssn, 1)))
		goto enoaddr;
	switch (status) {
	case N_SCCP_STATUS_USER_IN_SERVICE:
		if (ss->state != SSN_ALLOWED) {
			struct sr *sr;
			/* 
			   broadcast SSA */
			for (sr = sp->sr.list; sr; sr = sr->sp.next) {
				/* 
				   send SSA */
				struct sccp_addr cdpa, cgpa;
				cdpa.ni = add->ni;
				cdpa.ri = SCCP_RI_DPC_SSN;
				cdpa.pc = sr->add.pc;
				cdpa.ssn = 1;
				cdpa.gtt = 0;
				cdpa.alen = 0;
				cgpa.ni = sp->add.ni;
				cgpa.ri = SCCP_RI_DPC_SSN;
				cgpa.pc = sp->add.pc;
				cgpa.ssn = 1;
				cgpa.gtt = 0;
				cgpa.alen = 0;
				if ((err =
				     sccp_send_ssa(q, sc->sp.sp, sc->sp.sp->add.pc, &cdpa, &cgpa,
						   add->ssn, sp->add.pc, smi)) < 0)
					goto error;
			}
			/* 
			   local broadcast UIS (including GTT function) */
			if ((err = sccp_state_lbr(q, add, status, smi)) < 0)
				goto error;
			ss->state = SSN_ALLOWED;
		}
		break;
	case N_SCCP_STATUS_USER_OUT_OF_SERVICE:
		/* 
		   See ANSI Figure 8/T1.112.4-2000 sheet 1of1 */
		if (ss->state != SSN_PROHIBITED) {
			struct sr *sr;
			if (ss->flags & SSNF_IGNORE_SST_IN_PROGRESS) {
				ss_timer_stop(ss, tisst);
				ss->flags &= ~SSNF_IGNORE_SST_IN_PROGRESS;
			} else if (ss->flags & SSNF_WAIT_FOR_GRANT) {
				ss_timer_stop(ss, twsog);
				ss->flags &= ~SSNF_WAIT_FOR_GRANT;
			}
			/* 
			   broadcast SSP */
			for (sr = sp->sr.list; sr; sr = sr->sp.next) {
				/* 
				   send SSP */
				struct sccp_addr cdpa, cgpa;
				cdpa.ni = add->ni;
				cdpa.ri = SCCP_RI_DPC_SSN;
				cdpa.pc = sr->add.pc;
				cdpa.ssn = 1;
				cdpa.gtt = 0;
				cdpa.alen = 0;
				cgpa.ni = sp->add.ni;
				cgpa.ri = SCCP_RI_DPC_SSN;
				cgpa.pc = sp->add.pc;
				cgpa.ssn = 1;
				cgpa.gtt = 0;
				cgpa.alen = 0;
				if ((err =
				     sccp_send_ssp(q, sc->sp.sp, sc->sp.sp->add.pc, &cdpa, &cgpa,
						   add->ssn, sp->add.pc, smi)) < 0)
					goto error;
			}
			/* 
			   local broadcast UOS (including GTT function) */
			if ((err = sccp_state_lbr(q, add, status, smi)) < 0)
				goto error;
			ss->state = SSN_PROHIBITED;
		}
		break;
	default:
		swerr();
		goto efault;
	}
	return (QR_DONE);
      enoaddr:
	err = -EADDRNOTAVAIL;
	rare();
	goto error;
      efault:
	err = -EFAULT;
	swerr();
	goto error;
      error:
	return (err);
}

/*
 *  =========================================================================
 *
 *  EVENTS
 *
 *  =========================================================================
 */

/*
 *  -------------------------------------------------------------------------
 *
 *  Primitives from below
 *
 *  -------------------------------------------------------------------------
 */

#define SCCPF_LOC_S_BLOCKED	0x01
/*
 *  M_ERROR, M_HANGUP
 *  -----------------------------------
 */
#if 0
STATIC int
mtp_hangup(struct mt *mt)
{
	if (mt->sr) {
		struct sr *sr = mt->sr;
		if (!(sr->flags & SCCPF_LOC_S_BLOCKED)) {
			struct ss *ss;
			for (ss = sr->sp.sp->ss.list; ss; ss = ss->sp.next) {
				ss->flags |= SCCPF_LOC_S_BLOCKED;
			}
			sr->flags |= SCCPF_LOC_S_BLOCKED;
		}
	} else if (mt->sp) {
		struct sr *sr;
		struct sp *sp = mt->sp;
		for (sr = sp->sr.list; sr; sr = sr->sp.next) {
			if (!(sr->flags & SCCPF_LOC_S_BLOCKED)) {
				struct ss *ss;
				for (ss = sp->ss.list; ss; ss = ss->sp.next) {
					ss->flags |= SCCPF_LOC_S_BLOCKED;
				}
				sr->flags |= SCCPF_LOC_S_BLOCKED;
			}
		}
	} else {
		/* 
		   mt lower stream is not attached to a signalling point */
		rare();
	}
	fixme(("Notify management of failed lower stream\n"));
	return (QR_DISABLE);
}
#endif

/*
 *  M_DATA
 *  -----------------------------------
 */
STATIC int
mtp_read(queue_t *q, mblk_t *pdu, struct sr *sr, ulong pri, ulong sls)
{
	int err;
	mblk_t *mp = NULL;
	struct sccp_msg *m;
	struct sp *sp = sr->sp.sp;
	uchar *p, *e;
	if (!pdu)
		goto efault;
	if (pdu->b_cont && !ss7_pullupmsg(q, pdu, -1))
		goto enobufs;
	p = pdu->b_rptr;
	e = pdu->b_wptr;
	if (p + 1 > e)
		goto emsgsize;
	if (!(mp = ss7_allocb(q, sizeof(*m), BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_RSE;
	m = (typeof(m)) mp->b_wptr;
	mp->b_wptr += sizeof(*m);
	bzero(m, sizeof(*m));
	mp->b_cont = pdu;
	m->bp = mp;
	m->eq = NULL;
	m->xq = q;
	m->sc = NULL;
	m->mt = NULL;
	m->timestamp = jiffies;
	m->pvar = sp->proto.pvar;
	m->rl.opc = sr->add.pc;
	m->rl.dpc = sp->add.pc;
	m->rl.mp = pri;
	m->rl.sls = sls;
	m->type = *p++;
	if (p >= e)
		goto emsgsize;
	switch (m->type) {
	case SCCP_MT_CR:
		err = sccp_dec_cr(p, e, m);
		break;
	case SCCP_MT_CC:
		err = sccp_dec_cc(p, e, m);
		break;
	case SCCP_MT_CREF:
		err = sccp_dec_cref(p, e, m);
		break;
	case SCCP_MT_RLSD:
		err = sccp_dec_rlsd(p, e, m);
		break;
	case SCCP_MT_RLC:
		err = sccp_dec_rlc(p, e, m);
		break;
	case SCCP_MT_DT1:
		err = sccp_dec_dt1(p, e, m);
		break;
	case SCCP_MT_DT2:
		err = sccp_dec_dt2(p, e, m);
		break;
	case SCCP_MT_AK:
		err = sccp_dec_ak(p, e, m);
		break;
	case SCCP_MT_UDT:
		err = sccp_dec_udt(p, e, m);
		break;
	case SCCP_MT_UDTS:
		err = sccp_dec_udts(p, e, m);
		break;
	case SCCP_MT_ED:
		err = sccp_dec_ed(p, e, m);
		break;
	case SCCP_MT_EA:
		err = sccp_dec_ea(p, e, m);
		break;
	case SCCP_MT_RSR:
		err = sccp_dec_rsr(p, e, m);
		break;
	case SCCP_MT_RSC:
		err = sccp_dec_rsc(p, e, m);
		break;
	case SCCP_MT_ERR:
		err = sccp_dec_err(p, e, m);
		break;
	case SCCP_MT_IT:
		err = sccp_dec_it(p, e, m);
		break;
	case SCCP_MT_XUDT:
		err = sccp_dec_xudt(p, e, m);
		break;
	case SCCP_MT_XUDTS:
		err = sccp_dec_xudts(p, e, m);
		break;
	case SCCP_MT_LUDT:
		err = sccp_dec_ludt(p, e, m);
		break;
	case SCCP_MT_LUDTS:
		err = sccp_dec_ludts(p, e, m);
		break;
	default:
		/* 
		   any message with an unknown message type is discarded */
		err = -ENOPROTOOPT;
		break;
	}
	/* 
	   handle return value */
	if (err < 0 || (err = sccp_irte_msg(q, mp)) < 0) {
		switch (err) {
		case -EOPNOTSUPP:	/* unexpected message */
			break;
		case -EPROTONOSUPPORT:	/* parameter type error */
			break;
		case -ENOPROTOOPT:	/* message type error */
			/* 
			   ANSI T1.112.4-2000 Table B-1/T1.112.4: any message with an unknown
			   message type is discarded */
			break;
		}
		goto error;
	}
	return (err);
      emsgsize:
	err = -EMSGSIZE;
	ptrace(("ERROR: message size decoding error\n"));
	goto error;
      enobufs:
	err = -ENOBUFS;
	ptrace(("ERROR: no buffers\n"));
	goto error;
      efault:
	err = -EFAULT;
	swerr();
	goto error;
      error:
	if (mp)
		freeb(mp);
	return (err);
}

/*
 *  MTP_OK_ACK
 *  -----------------------------------
 *  We shouldn't get these.  All requests which require an MTP_OK_ACK must be performed before the stream is linked.
 */
STATIC int
mtp_ok_ack(queue_t *q, struct mt *mt, mblk_t *mp)
{
	struct MTP_ok_ack *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	goto efault;
      efault:
	swerr();
	return -EFAULT;
      emsgsize:
	swerr();
	return -EMSGSIZE;
}

/*
 *  MTP_ERROR_ACK
 *  -----------------------------------
 *  We shouldn't get these; however, we attempt a MTP_ADDR_REQ when we link to get information about the MTP we have
 *  linked.  This might be an error response to our request.
 */
STATIC int
mtp_error_ack(queue_t *q, struct mt *mt, mblk_t *mp)
{
	struct MTP_error_ack *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	goto efault;
      efault:
	swerr();
	return -EFAULT;
      emsgsize:
	swerr();
	return -EMSGSIZE;
}

/*
 *  MTP_BIND_ACK
 *  -----------------------------------
 */
STATIC int
mtp_bind_ack(queue_t *q, struct mt *mt, mblk_t *mp)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  MTP_ADDR_ACK
 *  -----------------------------------
 *  This is a response to our request for the addresses bound and connected to the MTP stream after linking.  What
 *  we want to do is find the associated Signalling Point structure for the local signalling point or Signalling
 *  Relation structure for the local and remote signalling point.  MTP streams can be bound in two fashions: with
 *  local signalling point code and wildcarded remote signalling point code, or with both local and remote
 *  signalling points specified.
 */
STATIC int
mtp_addr_ack(queue_t *q, struct mt *mt, mblk_t *mp)
{
	struct sp *sp;
	struct sr *sr = NULL;
	struct MTP_addr_ack *p = (typeof(p)) mp->b_rptr;
	uchar *loc_ptr, *rem_ptr;
	size_t loc_len, rem_len;
	struct mtp_addr *loc, *rem;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	loc_ptr = mp->b_rptr + p->mtp_loc_offset;
	if ((loc_len = p->mtp_loc_length) < sizeof(*loc))
		goto emsgsize;
	if (mp->b_wptr < mp->b_rptr + loc_len)
		goto emsgsize;
	rem_ptr = mp->b_rptr + p->mtp_rem_offset;
	if ((rem_len = p->mtp_loc_length) < sizeof(*rem))
		goto emsgsize;
	if (mp->b_wptr < mp->b_rptr + rem_len)
		goto emsgsize;
	if (loc_len && loc_len < sizeof(*loc))
		goto efault;
	if (rem_len && rem_len < sizeof(*rem))
		goto efault;
	loc = (typeof(loc)) loc_ptr;
	rem = (typeof(rem)) rem_ptr;
	if (!loc_len)
		goto efault;
	for (sp = master.sp.list;
	     sp && (sp->add.ni != loc->ni || sp->add.pc != loc->pc || sp->add.si != loc->si);
	     sp = sp->next) ;
	if (!sp)
		goto notfound;
	if (!rem_len)
		goto noremote;
	for (sr = master.sr.list;
	     sr && (sr->add.ni != rem->ni || sr->add.pc != rem->pc || sr->add.si != rem->si);
	     sr = sr->next) ;
	if (!sr)
		goto notfound;
	unless(sr->mt || sp->mt, goto ebusy);
	sr->mt = mtp_get(mt);
	unless(mt->sr, goto efault);	/* FIXME */
	mt->sr = sr_get(sr);	/* FIXME */
	fixme(("Set proper state for SR and SP\n"));
	return (QR_DONE);
      noremote:
	unless(sp->mt, goto ebusy);
	sp->mt = mtp_get(mt);
	unless(mt->sp, goto efault);	/* FIXME */
	mt->sp = sp_get(sp);	/* FIXME */
	fixme(("Set proper state for SR and SP\n"));
	return (QR_DONE);
      notfound:
	noenable(mt->iq);
	noenable(mt->oq);
	swerr();
	__ptrace(("%s: %p: ERROR: MTP provider stream already linked\n", DRV_NAME, mt));
	fixme(("Need to inform layer management to unlink stream\n"));
	return -EINVAL;
#if defined(_SAFE)||defined(_DEBUG)
      ebusy:
	noenable(mt->iq);
	noenable(mt->oq);
	swerr();
	__ptrace(("%s: %p: ERROR: MTP provider stream already linked\n", DRV_NAME, mt));
	fixme(("Need to inform layer management to unlink stream\n"));
	return -EBUSY;
#endif
      efault:
	noenable(mt->iq);
	noenable(mt->oq);
	swerr();
	__ptrace(("%s: %p: ERROR: MTP provider stream already linked\n", DRV_NAME, mt));
	fixme(("Need to inform layer management to unlink stream\n"));
	return -EFAULT;
      emsgsize:
	noenable(mt->iq);
	noenable(mt->oq);
	swerr();
	__ptrace(("%s: %p: ERROR: Bad message size\n", DRV_NAME, mt));
	fixme(("Need to inform layer management to unlink stream\n"));
	return -EMSGSIZE;
}

/*
 *  MTP_INFO_ACK
 *  -----------------------------------
 *  We can use this instead of MTP_ADDR_ACK which has less information in it.
 */
STATIC int
mtp_info_ack(queue_t *q, struct mt *mt, mblk_t *mp)
{
	struct sp *sp;
	struct sr *sr = NULL;
	struct MTP_info_ack *p = (typeof(p)) mp->b_rptr;
	uchar *add_ptr, *loc_ptr = NULL, *rem_ptr = NULL;
	size_t add_len, loc_len = 0, rem_len = 0;
	struct mtp_addr *loc, *rem;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	add_ptr = mp->b_rptr + p->mtp_addr_offset;
	add_len = p->mtp_addr_length;
	if (mp->b_wptr < add_ptr + add_len)
		goto emsgsize;
	if (add_len) {
		if (add_len >= sizeof(*loc)) {
			loc_ptr = add_ptr;
			loc_len = sizeof(*loc);
			add_ptr += sizeof(*loc);
			add_len -= sizeof(*loc);
		} else if (add_len)
			goto efault;
	}
	if (add_len) {
		if (add_len >= sizeof(*rem)) {
			rem_ptr = add_ptr;
			rem_len = sizeof(*rem);
			add_ptr += sizeof(*rem);
			add_len -= sizeof(*rem);
		} else if (add_len)
			goto efault;
	}
	loc = (typeof(loc)) loc_ptr;
	rem = (typeof(rem)) rem_ptr;
	if (!loc)
		goto efault;
	for (sp = master.sp.list;
	     sp && (sp->add.ni != loc->ni || sp->add.pc != loc->pc || sp->add.si != loc->si);
	     sp = sp->next) ;
	if (!sp)
		goto notfound;
	if (!rem)
		goto noremote;
	for (sr = master.sr.list;
	     sr && (sr->add.ni != rem->ni || sr->add.pc != rem->pc || sr->add.si != rem->si);
	     sr = sr->next) ;
	if (!sr)
		goto notfound;
	unless(sr->mt || sp->mt, goto ebusy);
	unless(mt->sr, goto efault);	/* FIXME */
	sr->mt = mtp_get(mt);
	mt->sr = sr_get(sr);	/* FIXME */
	fixme(("Set proper state for SR and SP\n"));
	return (QR_DONE);
      noremote:
	unless(sp->mt, goto ebusy);
	unless(mt->sp, goto efault);	/* FIXME */
	sp->mt = mtp_get(mt);
	mt->sp = sp_get(sp);	/* FIXME */
	fixme(("Set proper state for SR and SP\n"));
	return (QR_DONE);
      notfound:
	noenable(mt->iq);
	noenable(mt->oq);
	swerr();
	__ptrace(("%s: %p: ERROR: MTP provider stream already linked\n", DRV_NAME, mt));
	fixme(("Need to inform layer management to unlink stream\n"));
	return -EINVAL;
#if defined(_SAFE)||defined(_DEBUG)
      ebusy:
	noenable(mt->iq);
	noenable(mt->oq);
	swerr();
	__ptrace(("%s: %p: ERROR: MTP provider stream already linked\n", DRV_NAME, mt));
	fixme(("Need to inform layer management to unlink stream\n"));
	return -EBUSY;
#endif
      efault:
	noenable(mt->iq);
	noenable(mt->oq);
	swerr();
	__ptrace(("%s: %p: ERROR: MTP provider stream already linked\n", DRV_NAME, mt));
	fixme(("Need to inform layer management to unlink stream\n"));
	return -EFAULT;
      emsgsize:
	noenable(mt->iq);
	noenable(mt->oq);
	swerr();
	__ptrace(("%s: %p: ERROR: MTP provider stream already linked\n", DRV_NAME, mt));
	fixme(("Need to inform layer management to unlink stream\n"));
	return -EMSGSIZE;
}

/*
 *  MTP_OPTMGMT_ACK
 *  -----------------------------------
 *  We shouldn't get these.  MTP streams must have options set before they are linked under the SCCP multiplexing
 *  driver.
 */
STATIC int
mtp_optmgmt_ack(queue_t *q, struct mt *mt, mblk_t *mp)
{
	struct MTP_optmgmt_ack *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	goto efault;
      efault:
	swerr();
	return (-EFAULT);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/*
 *  MTP_TRANSFER_IND
 *  -----------------------------------
 *  This is data.  It should normally come to use, however, as M_DATA blocks rather than MTP_TRANSFER_IND when SCCP
 *  is bound connection oriented (local and remote point codes fixed).  This is normal for GSM-A.
 */
STATIC int
mtp_transfer_ind(queue_t *q, struct mt *mt, mblk_t *mp)
{
	int err;
	struct sr *sr;
	struct mtp_addr add;
	struct MTP_transfer_ind *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (mp->b_wptr < mp->b_rptr + p->mtp_srce_offset + p->mtp_srce_length)
		goto emsgsize;
	if (p->mtp_srce_length < sizeof(add))
		goto badaddr;
	if (!mp->b_cont)
		goto baddata;
	bcopy(mp->b_rptr + p->mtp_srce_offset, &add, sizeof(add));
	if (!mt->sp)
		sr = mt->sr;
	else
		for (sr = mt->sp->sr.list; sr && sr->id != add.pc; sr = sr->sp.next) ;
	if (!sr)
		goto eproto;
	if ((err = mtp_read(q, mp->b_cont, sr, p->mtp_mp, p->mtp_sls)) == QR_ABSORBED)
		return (QR_TRIMMED);
	return (err);
      baddata:
	err = -EFAULT;
	pswerr(("%s: %p: SWERR: no data\n", DRV_NAME, mt));
	goto error;
      badaddr:
	err = -EFAULT;
	pswerr(("%s: %p: SWERR: bad address\n", DRV_NAME, mt));
	goto error;
      emsgsize:
	err = -EFAULT;
	pswerr(("%s: %p: SWERR: bad message size\n", DRV_NAME, mt));
	goto error;
      eproto:
	err = -EPROTO;
	fixme(("Handle message from SP not known to SCCP\n"));
	goto error;
      error:
	return (err);
}

/*
 *  MTP_PAUSE_IND
 *  -----------------------------------
 *  ANSI T1.112.4 5.2.2 Signalling Point Prohibited.  When SCCP management receives an MTP-PAUSE indication relating
 *  to a destination that becomes inaccessible, SCCP management: (1) markes as "prohibited" the status of that
 *  signalling point; (2) intitiates a local broadcast (5.3.6) of "signalling point inaccessible" information for
 *  that signalling point; (3) [optionally] discontinues any SCCP status test for that signalling point; (4)
 *  [optionally] marks as "prohibited" the status of that signalling point's SCCP; (5) [optionally] initiates a
 *  local broadcast (5.3.6) of "SCCP inaccessible" information for that signalling point's SCCP; (6) if the
 *  indicated signalling point is a preferred node, marks the translation as "translate to next preferred node" if
 *  that signalling point has a backup; or, if the indicated signalling point is a loadshare node, marks the
 *  translation as "translate to the remaining available loadshare nodes."  Otherwise, marks the affected node
 *  inaccessible.  (7) marks as "prohibited" the status of each subsystem at that signalling point; (8) discontinues
 *  any subsystem status tests (Section 5.3.4) it may be conducting to any subsystems at that signalling point; (9)
 *  if the signalling point is a preferred node, marks the translation as "translate to next preferred subsystem"
 *  for each subsystem at that signalling point for which a backup subsystem exists; or, if the indicated signalling
 *  point is a loadshare node, marks the translation as 'translate to the remaining available loadshare subsystems'
 *  for each subsystem at that signalling point for which a loadshare subsystem exists.  Otherwise, marks the
 *  affected subsystems as inaccessible.  (10) initiates a local broadcast (Section 5.3.6) of "User-out-of-service"
 *  information for each subsystem at that signalling point; (11) if the MTP-PAUSE pertains to the preferred node
 *  and traffic will be diverted to the next preferred node, sends a Subsystem-Backup-Routing message regarding each
 *  replicated subsystem at that signalling point to SCCP management at the location of the corresponding next
 *  preferred subsystem.  (This action is taken only if the node receiving the MTP-PAUSE is a "translator node" that
 *  is adjacent to the node at which the next preferred subsystem is located.  For example, a signalling transfer
 *  point which learns that an adjacent databse has failed sends a Subsystem-Backup-Routing message to SCCP
 *  management for each next preferred subsystem at the next preferred node.  If the singalling transfer point is
 *  not adjacent to the allowed next preferred subsystem, it does not send Subsystem-Backup-Routing messages); (12)
 *  makrs all local equipped duplicated subsystems back routed from the failed signalling point, if the failed
 *  signalling point is an adjacent translator node; (13) initiates the traffic-mix information procedure
 *  (5.4.2.1.1) to the local allowed users if the failed signalling point is an adjacent translator node; (14) stops
 *  all subsystem routing status tests for the failed signalling point (5.4.4).
 *
 *  ANSI Figure 5/T1.112.4-2000.
 */
STATIC int
mtp_pause_ind(queue_t *q, struct mt *mt, mblk_t *mp)
{
	int err;
	struct sr *sr;
	struct sp *sp;
	struct ss *ss;
	struct sc *sc;
	struct mtp_addr *a;
	struct MTP_pause_ind *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (mp->b_wptr < mp->b_rptr + p->mtp_addr_offset + p->mtp_addr_length)
		goto emsgsize;
	a = (typeof(a)) (mp->b_rptr + p->mtp_addr_offset);
	if (!(sp = mt->sp))
		sr = mt->sr;
	else
		for (sr = sp->sr.list; sr && sr->add.pc != a->pc; sr = sr->sp.next) ;
	if (sr) {
		/* 
		   ANSI Figure 5/T1.112.4-2000 1of3: mark sp prohibited; local broadcast of "SP
		   inactive", mark sc prohibited, local broadcast of SCCP Inactive, update PC
		   translation; for each subsystem stop SST; mark subsystem prohibited; update
		   subsystem translation information; local broadcast of "UOS" */
		if (!sp)
			sp = sr->sp.sp;
		if (!(sr->flags & SCCPF_PROHIBITED)) {
			ulong smi = 0;	/* FIXME */
			/* 
			   local broadcast SP inactive (N-PCSTATE) */
			if ((err =
			     sccp_pcstate_lbr(q, a, N_SCCP_STATUS_SIGNALLING_POINT_INACCESSIBLE)))
				return (err);
			if (!(sr->flags & SCCPF_PROHIBITED)) {
				struct rs *rs;
				struct sccp_addr add;
				add.ni = a->ni;
				add.ri = SCCP_RI_DPC_SSN;
				add.pc = a->pc;
				add.gtt = 0;
				add.alen = 0;
				/* 
				   local broadcast of SCCP Inactive */
				for (sc = sp->gt.list; sc; sc = sc->sp.next) {
				}
				for (ss = sp->ss.list; ss; ss = ss->sp.next) {
					for (sc = ss->cl.list; sc; sc = sc->ss.next) {
					}
					for (sc = ss->cr.list; sc; sc = sc->ss.next) {
					}
					for (sc = ss->co.list; sc; sc = sc->ss.next) {
					}
				}
				for (rs = sr->rs.list; rs; rs = rs->sr.next) {
					add.ssn = rs->ssn;
					if (!(rs->flags & SCCPF_PROHIBITED)) {
						/* 
						   local broadcast of "UOS" N-STATE */
						if ((err =
						     sccp_state_lbr(q, &add,
								    N_SCCP_STATUS_USER_OUT_OF_SERVICE,
								    smi)))
							return (err);
						rs->flags |= SCCPF_PROHIBITED;
					} else if (rs->flags & SCCPF_SUBSYSTEM_TEST) {
						rs_timer_stop(rs, tsst);
						rs->flags &= ~SCCPF_SUBSYSTEM_TEST;
					}
				}
				sr->flags |= SCCPF_PROHIBITED;
			} else if (sr->flags & SCCPF_SUBSYSTEM_TEST) {
				sr_timer_stop(sr, tsst);
				sr->flags &= ~SCCPF_SUBSYSTEM_TEST;
			}
			sr->flags |= SCCPF_PROHIBITED;
		}
		if (!(sr->flags & SCCPF_LOC_S_BLOCKED)) {
			struct ss *ss;
			for (ss = sr->sp.sp->ss.list; ss; ss = ss->sp.next) {
				fixme(("Need to send N-PCSTATUS to ss\n"));
				ss->flags |= SCCPF_LOC_S_BLOCKED;
			}
			sr->flags |= SCCPF_LOC_S_BLOCKED;
		}
	}
	return (QR_DONE);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/*
 *  MTP_RESUME_IND
 *  -----------------------------------
 *  ANSI T1.112.4 5.2.3 Signalling Point Allowed.  When SCCP management receives an MTP-RESUME indicaiton relating
 *  to a destination that becomes accessible, SCCP management: (1) marks as "allowed" the status of that signalling
 *  point; (2) resets the congesiton level of the signalling point; (3) initiates a local broadcast (5.3.6) of
 *  "signalling point accessible" information for that signalling point; (4) [optionally] discontines any SCCP
 *  status test for that signalling point; (5) [optionally] marks as "allowed" the status of the signalling point's
 *  SCCP; (6) [optionally] initiates a local broadcast (5.3.6) of "SCCP accessible" information for the signalling
 *  point's SCCP; (7) marks the translation as "translate to previously inaccessible higher priority
 *  node/subsystem," if the signalling point is a replicated higher priority node, or, marks the translation as
 *  'translate to remaining available loadshare nodes', if the signalling point is a loadshare node. (8) performs
 *  either action (a) or (b) for each subsystem; (a) except for the selected replicated subsystem (8b); i. marks as
 *  "allowed" the status of each remote subsystem; ii. initiates a local broadcast (5.3.6) of "User-in-service"
 *  information for each subssytem at the signalling point; iii. marks the translation as "translate to previously
 *  inaccessible higher priority subsystem" for each replicated subsystem at the signalling point if the signalling
 *  point contains replicated higher prioirty subsystems, or, marks the translation as 'translate to all available
 *  loadshare subsystems' for each loadshare subsystem at that signalling point. or (b) As a network provider
 *  option, for selected replicated subsystems (operating in either dominant or the loadshare mode), the subsystem
 *  status test procedure is intiated. (9) inities, if the recovered signalling point is an adjacnet translator
 *  node; (a) the subsystem routing status test procedure (5.4.4) for all local equipped duplicated subsystems; (b)
 *  the traffic-mix information procedure (5.4.2.1.1) to the local allowed subsystems.
 */
STATIC int
mtp_resume_ind(queue_t *q, struct mt *mt, mblk_t *mp)
{
	struct sr *sr;
	struct mtp_addr *a;
	struct MTP_resume_ind *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (mp->b_wptr < mp->b_rptr + p->mtp_addr_offset + p->mtp_addr_length)
		goto emsgsize;
	a = (typeof(a)) (mp->b_rptr + p->mtp_addr_offset);
	if (!mt->sp)
		sr = mt->sr;
	else
		for (sr = mt->sp->sr.list; sr && sr->add.pc != a->pc; sr = sr->sp.next) ;
	if (sr) {
		if (sr->flags & SCCPF_LOC_S_BLOCKED) {
			struct ss *ss;
			for (ss = sr->sp.sp->ss.list; ss; ss = ss->sp.next) {
				fixme(("Need to send N-PCSTATUS to ss\n"));
				ss->flags |= SCCPF_LOC_S_BLOCKED;
			}
			sr->flags &= ~SCCPF_LOC_S_BLOCKED;
		}
	}
	return (QR_DONE);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/*
 *  MTP_STATUS_IND
 *  -----------------------------------
 *  ANSI T1.111.4/2000 5.2.4 Signalling Point Congestion.  When SCCP management receives an MTP-STATUS indicaiton
 *  relating to singalling network congestion to a signalling point, SCCP mangement: (1) updates that signalling
 *  point status to reflect the congesiton; (2) intiates a local broadcast (5.3.6) of "signalling point congested"
 *  information for that signalling point.
 *
 *  The congestion status of a signalling point may be used during translation to determine whether to route a
 *  received Class 0 message to the preferred node/subsystem or to the next preferred node/subsystem if the option
 *  is selected to alternate route traffic because of signalling point congestion for the dominant mode (defined ona
 *  per subsystem number basis for replicated subsystems operating in the dominant mode).  If the loadshare mode is
 *  in use, the congestion status of a signalling point may also be used during translation to determine if any of
 *  the loadshare node/subsystem should NOT receive a Class 0 message due to congesiton.
 *
 *  ANSI T1.112.4/2000 5.5.2 SCCP Prohibited Control.  When SCCP management receives an MTP-STATUS primitive
 *  indicating that the status of a remote SCCP is "inaccessible", "unequipped" or "unknown", SCCP management: (1)
 *  if the statis indicated for the remote SCCP is "inaccessible" or "unknown", initiates the SCCP Status Test
 *  (Section 5.5.4); (2) performs action (4) through (14) described in section 5.2.2, Signalling Point Prohibited
 *  Control.
 *
 *  5.5.3 SCCP Allowed Control.  When, for a remote SCCP marked "prohibited." (1) a Subsystem-Allowed message with
 *  SSN=1 is received; or; (2) Timer T(stat_info) expires but an MTP-STATUS prmitive indicating remote SCCP
 *  unavailability has not been received since the timer was started or restarted, SCCP management: (1) performs
 *  actions (4) through (9) described in Section 5.2.3 terminates [sic], if Subsystem-Allowed message received; (2)
 *  performs actions (5) through (9) described in Section 5.2.3, if timer T(stat_info) expires.
 *
 *  5.5.4 SCCP Status Test.  An SCCP status test is intiated when MTP-STATUS indication for an inacessible or
 *  unknown SCCP is received (Section 5.5.2).
 *
 *  An SCCP status test associated with an inaccessible or unknown SCCP is commenced by sending a
 *  Subsystem-Status-Test message (with the subsystem number set to one to indicate an SCCP status test) to the
 *  remote SCCP and starting a timer T(stat_info).  The timer is reset if another SST initiation request is received
 *  for the inacessible or unknown SCCP.  This cycle continues until the test is treminated by another SCCP
 *  management function at that node.  Termination of the test causes the timer to be cancelled.
 *
 *  The expiration of the timer T(stat_info) for an inaccessible or unknown SCCP is handled by the SCCP Allowed
 *  Control procedure (Section 5.5.3).
 *
 *  When SCCP management receives a Subsystem-Status-Test message (SSN=1), Subsystem-Alloed (SSN=1) message is sent
 *  to SCCP management at the node conduction the test.
 *
 *  Q.714/1996 5.2.4 Signalling point congested.   When SCCP management receives an MTP-STATUS indication primitive
 *  relating to signalling network congestion to a signalling point, SCCP management: 1) Determines the severity of
 *  the congestion in the remote signalling point and updates that signalling point status to reflect the congesiton
 *  as follows: MTP provides a single level of congesiton indication (international method).  The severity is
 *  reflected by a local internal status variable referred to as "restriction level" RL(M).  Each of the N+1
 *  restriction levels except the highest level is further divided into M restriction sublevels, RSL(M) where: N =
 *  8, M = 4.  The method to compute these levels uses an attack timer Ta and a decay timer Td.  a) When timer Ta is
 *  not running then: Timer Ta is started and Td is (re)started.  If RL(M) is equal to N, then no further action is
 *  taken.  RSL(M) is incremented.  If RLS(M) reaches M, then RSL(M) is set to zero and RL(M) is incremented.  b)
 *  When timer Ta is running, the MTP-STATUS indication primitive is ignored.  2) Initiates the procedure of 5.2.8.
 *
 *  When congestion abates, the traffic is gradually resumed.  SCCP management: 1) Decreases the restriction level
 *  (RL(M)) in a time-controlled manner as follows: When timer Td expires, then RSL(M) is decremented and: a) if
 *  RSL(M) reachs -1 and RL(M) is not zero, then RSL(M) is reset to M-1 and RL(M) is decreased by one; b) if either
 *  RSL(M) or RL(M) is not zero, then timer Td is restarted again.  2) Initiates the procedure or 5.2.8.
 *
 *  When an indication of the end of MTP-RESTART is received, the associated RL(M) and RSL(M) are set to zero.
 *
 *  The values of M, N, Ta and Td parameters are administratable and provisional.
 *
 *  Q.714/1996 5.2.8  Inter- and Intra- SCMG congesiton reports procedure.  This SCMG procedure uses the values of
 *  the following internal status variables: 1) RL(M) restriction level due to receipt of MTP-STATUS indication of
 *  congestion for each affected SP (5.2.4).  2) RSL(M) restriction sublevel per RL(M) due to receipt of the
 *  MTP-STATUS indication of congestion for each affected SP (5.2.4).  3) CL(S) SCCP congestion level due to receipt
 *  of the congestion level parameter of SSC message for each affected SP and SSN=1 (5.2.7).
 *
 *  The above values are used as inputs to compute the values of the following variables:  a) RL, SCRC traffic
 *  restriction level for each affected SP.  b) RSL, restriction sublevel per RL for each affected SP.  c) RIL,
 *  restricted importance level parameter reported to SCCP users for each affected SP.
 *
 *  If there is any change in RL or RSL, SCRC is informed of the new values of RL and RSL.
 *
 *  If there is any change in restricted importance level, the local broadcast procedure (5.3.6.6) is initiated to
 *  report the new value of restricted importance level.
 *
 *  NOTE:  The computation is left for further study.
 */
STATIC int
mtp_status_ind(queue_t *q, struct mt *mt, mblk_t *mp)
{
	struct sr *sr;
	struct mtp_addr *a;
	struct MTP_status_ind *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (mp->b_wptr < mp->b_rptr + p->mtp_addr_offset + p->mtp_addr_length)
		goto emsgsize;
	a = (typeof(a)) (mp->b_rptr + p->mtp_addr_offset);
	if (!mt->sp)
		sr = mt->sr;
	else
		for (sr = mt->sp->sr.list; sr && sr->add.pc != a->pc; sr = sr->sp.next) ;
	if (!sr)
		goto ignore;	/* not for us */
	switch (p->mtp_type) {
	case MTP_STATUS_TYPE_CONG:
		if (sr->proto.popt & SS7_POPT_MPLEV) {
			int level;
			/* 
			   multiple congestion levels ala ANSI */
			switch (p->mtp_status) {
			case MTP_STATUS_CONGESTION_LEVEL0:
				level = 0;
				break;
			case MTP_STATUS_CONGESTION_LEVEL1:
				level = 1;
				break;
			case MTP_STATUS_CONGESTION_LEVEL2:
				level = 2;
				break;
			case MTP_STATUS_CONGESTION_LEVEL3:
				level = 3;
				break;
			default:
				goto efault;
			}
			if (sr->level < level) {
				if (!sr->level) {
					/* 
					   congestion begins */
#if 0
					if ((err =
					     sccp_sr_maint_ind(q, sc->sr.sr,
							       SCCP_MAINT_USER_PART_CONGESTED)))
						return (err);
#endif
				}
				sr->level = level;
				sr_timer_start(sr, tdecay);
			}
		} else {
			/* 
			   single congestion level ala ITU */
			if (!sr->timers.tattack) {
				if (!sr->level) {
					/* 
					   congestion begins */
#if 0
					if ((err =
					     sccp_sr_maint_ind(q, sc->sr.sr,
							       SCCP_MAINT_USER_PART_CONGESTED)))
						return (err);
#endif
				}
				sr->level++;
				sr_timer_start(sr, tattack);
				sr_timer_start(sr, tdecay);
			}
		}
		return (QR_DONE);
	case MTP_STATUS_TYPE_UPU:
		switch (p->mtp_status) {
		case MTP_STATUS_UPU_UNEQUIPPED:
			/* 
			   inform management */
#if 0
			if ((err =
			     sccp_sr_maint_ind(q, sc->sr.sr, SCCP_MAINT_USER_PART_UNEQUIPPED)))
				return (err);
#endif
			return (QR_DONE);
		case MTP_STATUS_UPU_UNKNOWN:
		case MTP_STATUS_UPU_INACCESSIBLE:
#if 0
			if ((err =
			     sccp_sr_maint_ind(q, sc->sr.sr, SCCP_MAINT_USER_PART_UNAVAILABLE)))
				return (err);
#endif
#define SCCPF_UPT_PENDING 2	/* FIXME */
#define SS7_POPT_UPT (1<<5)	/* FIXME */
			/* 
			   start UPU procedures */
			if (!(sr->flags & SCCPF_UPT_PENDING)) {
				if ((sr->proto.popt & SS7_POPT_UPT) && sr->sp.sp
				    && sr->sp.sp->ss.list) {
					fixme(("Send SST message\n"));
					sr_timer_start(sr, tstatinfo);
				} else {
					/* 
					   send some other message */
				}
				sr->flags |= SCCPF_UPT_PENDING;
			}
			return (QR_DONE);
		default:
			goto efault;
		}
	default:
		goto efault;
	}
	goto efault;
      ignore:
	return (QR_DONE);
      efault:
	return (-EFAULT);
      emsgsize:
	return (-EMSGSIZE);
}

/*
 *  MTP_RESTART_BEGINS_IND
 *  -----------------------------------
 *  Q.714/1996 5.2.6 Local MTP network unavailability.  Any action taken is implementation dependent.
 */
STATIC int
mtp_restart_begins_ind(queue_t *q, struct mt *mt, mblk_t *mp)
{
	struct sr *sr;
	struct MTP_restart_begins_ind *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (!mt->sp && (!mt->sr || !mt->sr->sp.sp))
		goto efault;	/* shouldn't happen */
	for (sr = mt->sp->sr.list; sr; sr = sr->sp.next) {
		if (!(sr->flags & SCCPF_LOC_S_BLOCKED)) {
			struct ss *ss;
			for (ss = sr->sp.sp->ss.list; ss; ss = ss->sp.next) {
				fixme(("Send N-STATUS to users\n"));
				ss->flags |= SCCPF_LOC_S_BLOCKED;
			}
			sr->flags |= SCCPF_LOC_S_BLOCKED;
		}
	}
	return (QR_DONE);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
      efault:
	swerr();
	return (-EFAULT);
}

/*
 *  MTP_RESTART_COMPLETE_IND
 *  -----------------------------------
 *  ANSI T1.112.4/2000 5.2.5 SCCP Reaction to Local MTP Restart.  When SCCP management receives an indication of the
 *  end of MTP Restart, SCCP management: (1) resets the conestion level of the signalling points concerned by the
 *  restarintg MTP; (2) instructs the translation function to update the translation tables, taking into account the
 *  accessibility informaiton given by the MTP indicating the end of MTP Restart; (3) marks as "allowed" the status
 *  of the subsystems for each accessible point; (4) intiiates a local broadcast (5.3.6) of "signalling point
 *  accessible."
 *
 *  ANSI T1.112.4/2000 5.6 SCCP Restart.  On a signalling point restart, an indication is given to the SCCP by the
 *  MTP about the signallin points which are accessible after the restart actions.  For each accessible, concerned
 *  signalling point, all subsystems are marked allowed.  If SCCP Flow Control is used with the siganlling point,
 *  the status of its SCCP is also marked "allowed."  The response method is used to determine the status of
 *  subsystems and the SCCP in those signalling points.
 *
 *  At the restarted signalling point, the status of its own susbsystems are not broadcast to concerned signalling
 *  points.  In this case, the response method is used to inform other nodes attempting to access prohibited
 *  subsystems at the restarted signalling point.
 *
 *  The actions taken in cause of local MTP restart are described in section 5.2.5.
 *
 *  Q.714/1996 5.2.5 Local MTP network availability.  When SCCP management receives an indication reporting the end
 *  of MTP Restart, then it: 1) resets the congestion level of the associated signalling points; 2) instructs the
 *  translation function to update the translation tables, taking into account the accessibility given by the MTP
 *  indicating the end of MTP Restart; 3) marks as allowed the status of the SCCP and all subsystems for each
 *  accessible point; 4) initiates a local broadcast (5.3.6.4) of "signalling point accessible" information for the
 *  signalling point becoming accessible; 5) initiates a local broadcast of "remote SCCP accessible" for the remote
 *  SCCPs becoming accessible, and 6) initiates a local broadcast of "User-in-service" information for a subsystem
 *  associated with the end of the MTP-RESTART.
 */
STATIC int
mtp_restart_complete_ind(queue_t *q, struct mt *mt, mblk_t *mp)
{
	struct sr *sr;
	struct MTP_restart_complete_ind *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (!mt->sp && (!mt->sr || !mt->sr->sp.sp))
		goto efault;	/* shouldn't happen */
	for (sr = mt->sp->sr.list; sr; sr = sr->sp.next) {
		if (sr->flags & SCCPF_LOC_S_BLOCKED) {
			struct ss *ss;
			for (ss = sr->sp.sp->ss.list; ss; ss = ss->sp.next) {
				fixme(("Send N-STATUS to users\n"));
				ss->flags &= ~SCCPF_LOC_S_BLOCKED;
			}
			sr->flags &= ~SCCPF_LOC_S_BLOCKED;
		}
	}
	return (QR_DONE);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
      efault:
	swerr();
	return (-EFAULT);
}

/*
 *  UNRECOGNIZED PRIMITIVE
 *  -----------------------------------
 */
STATIC int
mtp_unrecognized_prim(queue_t *q, struct mt *mt, mblk_t *mp)
{
	swerr();
	fixme(("Notify management of unrecognized primitive\n"));
	return (-EFAULT);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Primitives from above
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  NPI Primitives
 *  -------------------------------------------------------------------------
 */
/*
 *  N_CONN_REQ:
 *  -----------------------------------
 */
STATIC int
n_conn_req(queue_t *q, struct sc *sc, mblk_t *mp)
{
	int err;
	N_conn_req_t *p = ((typeof(p)) mp->b_rptr);
	N_qos_sel_conn_sccp_t *qos = (typeof(qos)) (mp->b_rptr + p->QOS_offset);
	struct sccp_addr *a;
	ulong *sls = NULL;
	ulong *pri = NULL;
	ulong *pcl = NULL;
	ulong *imp = NULL;
	if (sccp_get_n_state(sc) != NS_IDLE)
		goto noutstate;
	sccp_set_n_state(sc, NS_WCON_CREQ);
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (p->QOS_length) {
		if (mp->b_wptr < mp->b_rptr + p->QOS_offset + p->QOS_length)
			goto nbadopt3;
		if (qos->n_qos_type != N_QOS_SEL_CONN_SCCP)
			goto nbadqostype;
		if (qos->protocol_class > 3)
			goto nbadopt2;
		if (qos->protocol_class != 2 && qos->protocol_class != 3)
			goto nnotsupport;
		if (p->QOS_length != sizeof(*qos))
			goto nbadopt;
	}
	a = (struct sccp_addr *) (mp->b_rptr + p->DEST_offset);
	if (!p->DEST_length)
		goto nnoaddr;
	if ((mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
	    || (p->DEST_length < sizeof(struct sccp_addr) + a->alen) || !a->ssn)
		goto nbadaddr;
	if (sc->cred.cr_uid != 0 && !a->ssn)
		goto naccess;
	fixme(("select a source local reference number\n"));
	if (p->QOS_length) {
		sls = &qos->sequence_selection;
		pri = &qos->message_priority;
		pcl = &qos->protocol_class;
		imp = &qos->importance;
	}
	sc->flags &= ~(SCCPF_REC_CONF_OPT | SCCPF_EX_DATA_OPT);
	if (p->CONN_flags & REC_CONF_OPT)
		sc->flags |= SCCPF_REC_CONF_OPT;
	if (p->CONN_flags & EX_DATA_OPT)
		sc->flags |= SCCPF_EX_DATA_OPT;
	if ((err = sccp_conn_req(q, sc, a, sls, pri, pcl, imp, mp->b_cont)))
		goto error;
	return (QR_TRIMMED);
      naccess:
	err = NACCESS;
	ptrace(("%s: %p: ERROR: no permission for requested address\n", DRV_NAME, sc));
	goto error;
      nbadaddr:
	err = NBADADDR;
	ptrace(("%s: %p: ERROR: address is unusable\n", DRV_NAME, sc));
	goto error;
      nnoaddr:
	err = NNOADDR;
	ptrace(("%s: %p: ERROR: no destination address\n", DRV_NAME, sc));
	goto error;
      nbadopt:
	err = NBADOPT;
	ptrace(("%s: %p: ERROR: options are unusable\n", DRV_NAME, sc));
	goto error;
      nnotsupport:
	err = NNOTSUPPORT;
	ptrace(("%s: %p: ERROR: protocol class not supported\n", DRV_NAME, sc));
	goto error;
      nbadopt2:
	err = NBADOPT;
	ptrace(("%s: %p: ERROR: invalid protocol class\n", DRV_NAME, sc));
	goto error;
      nbadqostype:
	err = NBADQOSTYPE;
	ptrace(("%s: %p: ERROR: QOS structure type not supported\n", DRV_NAME, sc));
	goto error;
      nbadopt3:
	err = NBADOPT;
	ptrace(("%s: %p: ERROR: options are unusable\n", DRV_NAME, sc));
	goto error;
      einval:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: invalid message format\n", DRV_NAME, sc));
	goto error;
      noutstate:
	err = NOUTSTATE;
	ptrace(("%s: %p: ERROR: would place interface out of state\n", DRV_NAME, sc));
	goto error;
      error:
	return n_error_ack(q, sc, N_CONN_REQ, err);
}

/*
 *  N_CONN_RES:
 *  -----------------------------------
 *  IMPLEMENTATION NOTE:- Sequence numbers are actually the address of the mblk which contains the CR message and
 *  contains the contents of this message as a connection indication.  To find if a particular sequence number is
 *  valid, we walk the connection indication queue looking for an mblk with the same address as the sequence number.
 *  Sequence numbers are only valid on the stream for which the connection indication is queued. 
 */
STATIC INLINE mblk_t *
n_seq_check(struct sc *sc, ulong seq)
{
	mblk_t *mp;
	bufq_lock(&sc->conq);
	for (mp = bufq_head(&sc->conq); mp && mp != (mblk_t *) seq; mp = mp->b_next) ;
	bufq_unlock(&sc->conq);
	usual(mp);
	return (mp);
}

/*
 *  IMPLEMENTATION NOTE:- Tokens are actually the address of the read queue.  To find if a particular token is
 *  valid, we walk the open list looking for an open device with a read queue with the same address as the token.
 *  Tokens are only valid for SCCP streams of the same provider type.  (That is, a TPI connection indication cannot
 *  be accepted on an NPI stream. 
 */
STATIC INLINE struct sc *
n_tok_check(struct sc *sc, ulong tok)
{
	struct sc *ap;
	queue_t *aq = (queue_t *) tok;
	for (ap = master.sc.list; ap && ap->oq != aq; ap = ap->next) ;
	usual(ap);
	return ((struct sc *) ap);
}
STATIC int
n_conn_res(queue_t *q, struct sc *sc, mblk_t *mp)
{
	int err;
	mblk_t *cp;
	struct sc *ap;
	N_conn_res_t *p = (typeof(p)) mp->b_rptr;
	N_qos_sel_conn_sccp_t *qos = (typeof(qos)) (mp->b_rptr + p->QOS_offset);
	if (sccp_get_n_state(sc) != NS_WRES_CIND)
		goto noutstate;
	sccp_set_n_state(sc, NS_WACK_CRES);
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (p->QOS_length) {
		if (mp->b_wptr < mp->b_rptr + p->QOS_offset + p->QOS_length)
			goto nbadopt2;
		if (qos->n_qos_type != N_QOS_SEL_CONN_SCCP)
			goto nbadqostype;
		if (p->QOS_length != sizeof(*qos))
			goto nbadopt;
	}
	if (!(cp = n_seq_check(sc, p->SEQ_number)))
		goto nbadseq;
	if (!(ap = n_tok_check(sc, p->TOKEN_value))
	    || (ap != sc && ((1 << ap->i_state) & ~(NSF_UNBND | NSF_IDLE))))
		goto nbadtoken2;
	if (ap->i_state == NS_IDLE && ap->conind)
		goto nbadtoken;
	/* 
	   protect at least r00t streams from users */
	if (sc->cred.cr_uid != 0 && ap->cred.cr_uid == 0)
		goto naccess;
	{
		ulong ap_oldstate = ap->i_state;
		ulong ap_oldflags = ap->flags;
		ap->i_state = NS_DATA_XFER;
		ap->flags &= ~(SCCPF_REC_CONF_OPT | SCCPF_EX_DATA_OPT);
		if (p->CONN_flags & REC_CONF_OPT)
			ap->flags |= SCCPF_REC_CONF_OPT;
		if (p->CONN_flags & EX_DATA_OPT)
			ap->flags |= SCCPF_EX_DATA_OPT;
		if ((err = sccp_conn_res(q, sc, cp, ap, mp->b_cont)))
			goto call_error;
		if ((err = n_ok_ack(q, sc, N_CONN_RES, p->SEQ_number, p->TOKEN_value)))
			goto call_error;
		return (QR_TRIMMED);
	      call_error:
		ap->i_state = ap_oldstate;
		ap->flags = ap_oldflags;
		goto error;
	}
      naccess:
	err = NACCESS;
	ptrace(("%s: %p: ERROR: no access to accepting queue\n", DRV_NAME, sc));
	goto error;
      nbadtoken:
	err = NBADTOKEN;
	ptrace(("%s: %p: ERROR: accepting queue is listening\n", DRV_NAME, sc));
	goto error;
      nbadtoken2:
	err = NBADTOKEN;
	ptrace(("%s: %p: ERROR: accepting queue id is invalid\n", DRV_NAME, sc));
	goto error;
      nbadseq:
	err = NBADSEQ;
	ptrace(("%s: %p: ERROR: connection ind reference is invalid\n", DRV_NAME, sc));
	goto error;
      nbadopt:
	err = NBADOPT;
	ptrace(("%s: %p: ERROR: quality of service options are bad\n", DRV_NAME, sc));
	goto error;
      nbadqostype:
	err = NBADQOSTYPE;
	ptrace(("%s: %p: ERROR: quality of service options are bad\n", DRV_NAME, sc));
	goto error;
      nbadopt2:
	err = NBADOPT;
	ptrace(("%s: %p: ERROR: quality of service options are bad\n", DRV_NAME, sc));
	goto error;
      einval:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, sc));
	goto error;
      noutstate:
	err = NOUTSTATE;
	ptrace(("%s: %p: ERROR: would place interface out of state\n", DRV_NAME, sc));
	goto error;
      error:
	return n_error_ack(q, sc, N_CONN_RES, err);
}

/*
 *  N_DISCON_REQ:
 *  -----------------------------------
 */
STATIC int
n_discon_req(queue_t *q, struct sc *sc, mblk_t *mp)
{
	int err;
	mblk_t *cp = NULL;
	N_discon_req_t *p = (typeof(p)) mp->b_rptr;
	if ((1 << sccp_get_n_state(sc)) &
	    ~(NSF_WCON_CREQ | NSF_WRES_CIND | NSF_DATA_XFER | NSF_WCON_RREQ | NSF_WRES_RIND))
		goto noutstate;
	switch (sccp_get_n_state(sc)) {
	case NS_WCON_CREQ:
		sccp_set_n_state(sc, NS_WACK_DREQ6);
		break;
	case NS_WRES_CIND:
		sccp_set_n_state(sc, NS_WACK_DREQ7);
		break;
	case NS_DATA_XFER:
		sccp_set_n_state(sc, NS_WACK_DREQ9);
		break;
	case NS_WCON_RREQ:
		sccp_set_n_state(sc, NS_WACK_DREQ10);
		break;
	case NS_WRES_RIND:
		sccp_set_n_state(sc, NS_WACK_DREQ11);
		break;
	}
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (p->RES_length)
		goto nbadaddr;
	if (sccp_get_n_state(sc) == NS_WACK_DREQ7 && !(cp = n_seq_check(sc, p->SEQ_number)))
		goto nbadseq;
	/* 
	   XXX: What to do with the disconnect reason? Nothing? */
	if ((err = sccp_discon_req(q, sc, cp, mp->b_cont)))
		goto error;
	if ((err = n_ok_ack(q, sc, p->PRIM_type, p->SEQ_number, 0)))
		goto error;
	return (QR_TRIMMED);
      nbadseq:
	err = NBADSEQ;
	ptrace(("%s: %p: ERROR: connection ind reference is invalid\n", DRV_NAME, sc));
	goto error;
      nbadaddr:
	err = NBADADDR;
	ptrace(("%s: %p: ERROR: responding addres invalid\n", DRV_NAME, sc));
	goto error;
      einval:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, sc));
	goto error;
      noutstate:
	err = NOUTSTATE;
	ptrace(("%s: %p: ERROR: would place interface out of state\n", DRV_NAME, sc));
	goto error;
      error:
	return n_error_ack(q, sc, N_DISCON_REQ, err);
}

/*
 *  N_DATA_REQ:
 *  -----------------------------------
 */
STATIC INLINE int
n_error_reply(queue_t *q, struct sc *sc, int err)
{
	mblk_t *mp;
	switch (err) {
	case -EBUSY:
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
	case QR_DONE:
	case QR_ABSORBED:
	case QR_TRIMMED:
		ptrace(("%s: %p: ERROR: No error condition\n", DRV_NAME, sc));
		return (err);
	}
	if ((mp = ss7_allocb(q, 2, BPRI_HI))) {
		mp->b_datap->db_type = M_ERROR;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		putnext(sc->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
STATIC INLINE int
n_write(queue_t *q, struct sc *sc, mblk_t *mp)
{
	int err;
	if (sccp_get_n_state(sc) == NS_IDLE)
		goto discard;
	if ((1 << sccp_get_n_state(sc)) & ~(NSF_DATA_XFER | NSF_WRES_RIND))
		goto eproto;
	if ((err = sccp_data_req(q, sc, 0, 0, 0, mp)))
		goto error;
	return (QR_ABSORBED);	/* absorbed mp */
      eproto:
	err = -EPROTO;
	ptrace(("%s: %p: ERROR: would place interface out of state\n", DRV_NAME, sc));
	goto error;
      discard:
	err = 0;
	ptrace(("%s: %p: ERROR: ignoring in idle state\n", DRV_NAME, sc));
	goto error;
      error:
	return n_error_reply(q, sc, err);
}
STATIC int
n_data_req(queue_t *q, struct sc *sc, mblk_t *mp)
{
	int err;
	N_qos_sel_data_sccp_t *qos = NULL;
	N_data_req_t *p = (typeof(p)) mp->b_rptr;
	if (sccp_get_n_state(sc) == NS_IDLE)
		goto discard;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (mp->b_wptr >= mp->b_rptr + sizeof(*p) + sizeof(*qos)) {
		qos = (typeof(qos)) (p + 1);
		if (qos->n_qos_type != N_QOS_SEL_DATA_SCCP)
			qos = NULL;
	}
	if ((1 << sccp_get_n_state(sc)) & ~(NSF_DATA_XFER | NSF_WRES_RIND))
		goto eproto;
	{
		ulong exp = 0;		/* FIXME */
		ulong more = 0;		/* FIXME */
		ulong rcpt = 0;		/* FIXME */
		if ((err = sccp_data_req(q, sc, exp, more, rcpt, mp->b_cont)))
			goto error;
		return (QR_ABSORBED);	/* absorbed mp->b_cont */
	}
      eproto:
	err = -EPROTO;
	ptrace(("%s: %p: ERROR: would place interface out of state\n", DRV_NAME, sc));
	goto error;
      einval:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, sc));
	goto error;
      discard:
	err = QR_DONE;
	ptrace(("%s: %p: ERROR: ignore in idle state\n", DRV_NAME, sc));
	goto error;
      error:
	return n_error_reply(q, sc, err);
}

/*
 *  N_EXDATA_REQ:
 *  -----------------------------------
 */
STATIC int
n_exdata_req(queue_t *q, struct sc *sc, mblk_t *mp)
{
	int err;
	N_qos_sel_data_sccp_t *qos = NULL;
	N_exdata_req_t *p = (typeof(p)) mp->b_rptr;
	if (sccp_get_n_state(sc) == NS_IDLE)
		goto discard;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (mp->b_wptr >= mp->b_rptr + sizeof(*p) + sizeof(*qos)) {
		qos = (typeof(qos)) (p + 1);
		if (qos->n_qos_type != N_QOS_SEL_DATA_SCCP)
			qos = NULL;
	}
	if ((1 << sccp_get_n_state(sc)) & ~(NSF_DATA_XFER | NSF_WRES_RIND))
		goto eproto;
	{
		ulong exp = 0;		/* FIXME */
		ulong more = 0;		/* FIXME */
		ulong rcpt = 0;		/* FIXME */
		(void) sccp_exdata_req;
		if ((err = sccp_data_req(q, sc, exp, more, rcpt, mp->b_cont)))
			goto error;
		return (QR_ABSORBED);	/* absorbed mp->b_cont */
	}
      eproto:
	err = -EPROTO;
	ptrace(("%s: %p: ERROR: would place interface out of state\n", DRV_NAME, sc));
	goto error;
      einval:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, sc));
	goto error;
      discard:
	err = QR_DONE;
	ptrace(("%s: %p: ERROR: ignore in idle state\n", DRV_NAME, sc));
	goto error;
      error:
	return n_error_reply(q, sc, err);
}

/*
 *  N_INFO_REQ:
 *  -----------------------------------
 */
STATIC int
n_info_req(queue_t *q, struct sc *sc, mblk_t *mp)
{
	(void) mp;
	return n_info_ack(q, sc);
}

/*
 *  N_BIND_REQ:
 *  -----------------------------------
 */
STATIC int
n_bind_req(queue_t *q, struct sc *sc, mblk_t *mp)
{
	int err;
	N_bind_req_t *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *a;
	if (sccp_get_n_state(sc) != NS_UNBND)
		goto noutstate;
	sccp_set_n_state(sc, NS_WACK_BREQ);
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	a = (struct sccp_addr *) (mp->b_rptr + p->ADDR_offset);
	if ((mp->b_wptr < mp->b_rptr + p->ADDR_offset + p->ADDR_length)
	    || (p->ADDR_length < sizeof(struct sccp_addr) + a->alen) || !a->ssn)
		goto nbadaddr;
	fixme(("Deal with wildcards\n"));
	fixme(("Deal with default binds\n"));
	fixme(("Deal with slr assignment\n"));
	if (0)
		goto nnoaddr;
	if (sc->cred.cr_uid != 0 && !a->ssn)
		goto naccess;
	if ((err = sccp_bind_req(q, sc, a, p->CONIND_number)))
		goto error;
	return n_bind_ack(q, sc);
      naccess:
	err = NACCESS;
	ptrace(("%s: %p: ERROR: no permission for requested address\n", DRV_NAME, sc));
	goto error;
      nnoaddr:
	err = NNOADDR;
	ptrace(("%s: %p: ERROR: could not allocate address\n", DRV_NAME, sc));
	goto error;
      nbadaddr:
	err = NBADADDR;
	ptrace(("%s: %p: ERROR: address is invalid\n", DRV_NAME, sc));
	goto error;
      einval:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, sc));
	goto error;
      noutstate:
	err = NOUTSTATE;
	ptrace(("%s: %p: ERROR: would place interface out of state\n", DRV_NAME, sc));
	goto error;
      error:
	return n_error_ack(q, sc, N_BIND_REQ, err);
}

/*
 *  N_UNBIND_REQ:
 *  -----------------------------------
 */
STATIC int
n_unbind_req(queue_t *q, struct sc *sc, mblk_t *mp)
{
	int err;
	N_unbind_req_t *p = (typeof(p)) mp->b_rptr;
	(void) p;
	if (sccp_get_n_state(sc) != NS_IDLE)
		goto noutstate;
	sccp_set_n_state(sc, NS_WACK_UREQ);
	if ((err = sccp_unbind_req(q, sc)))
		goto error;
	return n_ok_ack(q, sc, p->PRIM_type, 0, 0);
      noutstate:
	err = NOUTSTATE;
	ptrace(("%s: %p: ERROR: would place interface out of state\n", DRV_NAME, sc));
	goto error;
      error:
	return n_error_ack(q, sc, N_UNBIND_REQ, err);
}

/*
 *  N_UNITDATA_REQ:
 *  -----------------------------------
 */
STATIC INLINE int
n_uderror_reply(queue_t *q, struct sc *sc, mblk_t *mp, int etype)
{
	N_uderror_ind_t *p = (typeof(p)) mp->b_rptr;
	switch (etype) {
	case -EBUSY:
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
	case QR_DONE:
	case QR_ABSORBED:
	case QR_TRIMMED:
		ptrace(("%s: %p: ERROR: No error condition\n", DRV_NAME, sc));
		return (etype);
	}
	if (etype > 0) {
		mp->b_wptr = mp->b_rptr + sizeof(*p);
		p->PRIM_type = N_UDERROR_IND;
		p->ERROR_type = etype;
	} else {
		mp->b_datap->db_type = M_ERROR;
		mp->b_wptr = mp->b_rptr;
		*(mp->b_wptr)++ = -etype;
		*(mp->b_wptr)++ = -etype;
	}
	putnext(sc->oq, mp);
	return (QR_ABSORBED);
}
STATIC int
n_unitdata_req(queue_t *q, struct sc *sc, mblk_t *mp)
{
	int err;
	size_t dlen = mp->b_cont ? msgdsize(mp->b_cont) : 0;
	const N_unitdata_req_t *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *dst;
	N_qos_sel_data_sccp_t *qos;
	if (dlen == 0 || dlen > sc->mtu)
		goto eproto5;
	if (sccp_get_n_state(sc) != NS_IDLE)
		goto eproto4;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto eproto3;
	dst = (typeof(dst)) (mp->b_rptr + p->DEST_offset);
	qos = NULL;
	if ((mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
	    || (!p->DEST_offset) || (p->DEST_offset < sizeof(*p))
	    || (p->DEST_length < sizeof(*dst)) || (p->DEST_length < sizeof(*dst) + dst->alen))
		goto eproto2;
	if (sc->cred.cr_uid != 0 && !dst->ssn)
		goto eproto;
	if (mp->b_wptr >=
	    mp->b_rptr + p->DEST_offset + p->DEST_length + PAD4(dst->alen) + sizeof(*qos)) {
		qos = (typeof(qos)) (mp->b_rptr + p->DEST_offset + p->DEST_length);
		if (qos->n_qos_type != N_QOS_SEL_DATA_SCCP)
			qos = NULL;
	}
	{
		ulong pcl = qos ? qos->protocol_class : sc->pcl;
		ulong ret = qos ? qos->option_flags : sc->flags;
		ulong imp = qos ? qos->importance : sc->imp;
		ulong seq = qos ? qos->sequence_selection : sc->sls;
		ulong pri = qos ? qos->message_priority : sc->mp;
		if ((err = sccp_unitdata_req(q, sc, dst, pcl, ret, imp, seq, pri, mp->b_cont)))
			goto error;
		return (QR_TRIMMED);
	}
      eproto:
	err = -EPROTO;
	ptrace(("%s: %p: ERROR: no permission to send to address\n", DRV_NAME, sc));
	goto error;
      eproto2:
	err = -EPROTO;
	ptrace(("%s: %p: ERROR: bad destnation address\n", DRV_NAME, sc));
	goto error;
      eproto3:
	err = -EPROTO;
	ptrace(("%s: %p: ERROR: invalid primitive\n", DRV_NAME, sc));
	goto error;
      eproto4:
	err = -EPROTO;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, sc));
	goto error;
      eproto5:
	err = -EPROTO;
	ptrace(("%s: %p: ERROR: bad data size\n", DRV_NAME, sc));
	goto error;
      error:
	return n_uderror_reply(q, sc, mp, err);
}

/*
 *  N_OPTMGMT_REQ:
 *  -----------------------------------
 */
STATIC int
n_optmgmt_req(queue_t *q, struct sc *sc, mblk_t *mp)
{
	int err;
	N_optmgmt_req_t *p = (typeof(p)) mp->b_rptr;
	N_qos_sel_info_sccp_t *qos = (typeof(qos)) (mp->b_rptr + p->QOS_offset);
	if (sccp_get_n_state(sc) == NS_IDLE)
		sccp_set_n_state(sc, NS_WACK_OPTREQ);
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (p->QOS_length) {
		if (mp->b_wptr < mp->b_rptr + p->QOS_offset + p->QOS_length)
			goto nbadopt3;
		if (qos->n_qos_type != N_QOS_SEL_INFO_SCCP)
			goto nbadqostype;
		if (p->QOS_length != sizeof(*qos))
			goto nbadopt2;
		if (qos->protocol_class > 3)
			goto nbadopt;
		if (qos->protocol_class != -1UL)
			sc->pcl = qos->protocol_class;
		if (qos->option_flags != -1UL)
			sc->flags = qos->option_flags;
	}
	if (p->OPTMGMT_flags & DEFAULT_RC_SEL)
		sc->flags |= SCCPF_DEFAULT_RC_SEL;
	else
		sc->flags &= ~SCCPF_DEFAULT_RC_SEL;
	return n_ok_ack(q, sc, p->PRIM_type, 0, 0);
      nbadopt:
	err = NBADOPT;
	ptrace(("%s: %p: ERROR: bad protocol class\n", DRV_NAME, sc));
	goto error;
      nbadopt2:
	err = NBADOPT;
	ptrace(("%s: %p: ERROR: invalid qos options\n", DRV_NAME, sc));
	goto error;
      nbadqostype:
	err = NBADQOSTYPE;
	ptrace(("%s: %p: ERROR: bad qos structure type\n", DRV_NAME, sc));
	goto error;
      nbadopt3:
	err = NBADOPT;
	ptrace(("%s: %p: ERROR: bad qos structure format\n", DRV_NAME, sc));
	goto error;
      einval:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, sc));
	goto error;
      error:
	return n_error_ack(q, sc, N_OPTMGMT_REQ, err);
}

/*
 *  N_DATACK_REQ:
 *  -----------------------------------
 */
STATIC int
n_datack_req(queue_t *q, struct sc *sc, mblk_t *mp)
{
	N_datack_req_t *p = (typeof(p)) mp->b_rptr;
	(void) sccp_datack_req;
	fixme(("SWERR: Write this function\n"));
	return n_error_ack(q, sc, N_DATACK_REQ, NNOTSUPPORT);
}

/*
 *  N_RESET_REQ:
 *  -----------------------------------
 */
STATIC int
n_reset_req(queue_t *q, struct sc *sc, mblk_t *mp)
{
	int err;
	N_reset_req_t *p = (typeof(p)) mp->b_rptr;
	if (sccp_get_n_state(sc) == NS_IDLE)
		goto discard;
	if (sccp_get_n_state(sc) != NS_DATA_XFER)
		goto noutstate;
	sccp_set_n_state(sc, NS_WCON_RREQ);
	if ((err = sccp_reset_req(q, sc)))
		goto error;
	return (0);
      noutstate:
	err = NOUTSTATE;
	ptrace(("%s: %p: ERROR: would place interface out of state\n", DRV_NAME, sc));
	goto error;
      discard:
	err = 0;
	ptrace(("%s: %p: ERROR: ignore in idle state\n", DRV_NAME, sc));
	goto error;
      error:
	return n_error_ack(q, sc, N_RESET_REQ, err);
}

/*
 *  N_RESET_RES:
 *  -----------------------------------
 */
STATIC int
n_reset_res(queue_t *q, struct sc *sc, mblk_t *mp)
{
	int err;
	N_reset_res_t *p = (typeof(p)) mp->b_rptr;
	if (sccp_get_n_state(sc) == NS_IDLE)
		goto discard;
	if (sccp_get_n_state(sc) != NS_WRES_RIND)
		goto noutstate;
	sccp_set_n_state(sc, NS_WACK_RRES);
	if ((err = sccp_reset_res(q, sc)))
		goto error;
	return n_ok_ack(q, sc, p->PRIM_type, 0, 0);
      noutstate:
	err = NOUTSTATE;
	ptrace(("%s: %p: ERROR: would place interface out of state\n", DRV_NAME, sc));
	goto error;
      discard:
	err = 0;
	ptrace(("%s: %p: ERROR: ignore in idle state\n", DRV_NAME, sc));
	goto error;
      error:
	return n_error_ack(q, sc, N_RESET_RES, err);
}

/*
 *  N_INFORM_REQ
 *  -----------------------------------
 */
STATIC int
n_inform_req(queue_t *q, struct sc *sc, mblk_t *mp)
{
	int err;
	N_inform_req_t *p = (typeof(p)) mp->b_rptr;
	N_qos_sel_infr_sccp_t *qos = NULL;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (p->QOS_length) {
		if (mp->b_wptr < mp->b_rptr + p->QOS_offset + p->QOS_length)
			goto emsgsize;
		if (p->QOS_length != sizeof(*qos))
			goto badqostype;
		qos = (typeof(qos)) (mp->b_rptr + p->QOS_offset);
		if (qos->n_qos_type != N_QOS_SEL_INFR_SCCP)
			goto badqostype;
	}
	if ((err = sccp_inform_req(q, sc, qos, p->REASON)))
		goto error;
	return n_ok_ack(q, sc, N_INFORM_REQ, 0, 0);
      badqostype:
	err = NBADQOSTYPE;
	ptrace(("%s: %p: ERROR: QOS structure type not supported\n", DRV_NAME, sc));
	goto error;
      emsgsize:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, sc));
	goto error;
      error:
	return n_error_ack(q, sc, N_INFORM_REQ, err);
}

/*
 *  N_COORD_REQ
 *  -----------------------------------
 */
STATIC int
n_coord_req(queue_t *q, struct sc *sc, mblk_t *mp)
{
	int err;
	N_coord_req_t *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *add;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (!p->ADDR_length)
		goto noaddr;
	if (mp->b_wptr < mp->b_rptr + p->ADDR_offset + p->ADDR_length)
		goto emsgsize;
	if (p->ADDR_length < sizeof(*add))
		goto badaddr;
	add = (typeof(add)) (mp->b_rptr + p->ADDR_offset);
	if (p->ADDR_length != sizeof(*add) + add->alen)
		goto badaddr;
	if ((err = sccp_coord_req(q, sc, add)))
		goto error;
	return n_ok_ack(q, sc, N_COORD_REQ, 0, 0);
      badaddr:
	err = NBADADDR;
	ptrace(("%s: %p: ERROR: bad address format\n", DRV_NAME, sc));
	goto error;
      noaddr:
	err = NNOADDR;
	ptrace(("%s: %p: ERROR: could not assign address\n", DRV_NAME, sc));
	goto error;
      emsgsize:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, sc));
	goto error;
      error:
	return n_error_ack(q, sc, N_COORD_REQ, err);
}

/*
 *  N_COORD_RES
 *  -----------------------------------
 */
STATIC int
n_coord_res(queue_t *q, struct sc *sc, mblk_t *mp)
{
	int err;
	N_coord_res_t *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *add;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (!p->ADDR_length)
		goto noaddr;
	if (mp->b_wptr < mp->b_rptr + p->ADDR_offset + p->ADDR_length)
		goto emsgsize;
	if (p->ADDR_length < sizeof(*add))
		goto badaddr;
	add = (typeof(add)) (mp->b_rptr + p->ADDR_offset);
	if (p->ADDR_length != sizeof(*add) + add->alen)
		goto badaddr;
	if ((err = sccp_coord_res(q, sc, add)))
		goto error;
	return n_ok_ack(q, sc, N_COORD_RES, 0, 0);
      badaddr:
	err = NBADADDR;
	ptrace(("%s: %p: ERROR: bad address format\n", DRV_NAME, sc));
	goto error;
      noaddr:
	err = NNOADDR;
	ptrace(("%s: %p: ERROR: could not assign address\n", DRV_NAME, sc));
	goto error;
      emsgsize:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, sc));
	goto error;
      error:
	return n_error_ack(q, sc, N_COORD_RES, err);
}

/*
 *  N_STATE_REQ
 *  -----------------------------------
 */
STATIC int
n_state_req(queue_t *q, struct sc *sc, mblk_t *mp)
{
	int err;
	N_state_req_t *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *add;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (!p->ADDR_length)
		goto noaddr;
	if (mp->b_wptr < mp->b_rptr + p->ADDR_offset + p->ADDR_length)
		goto emsgsize;
	if (p->ADDR_length < sizeof(*add))
		goto badaddr;
	add = (typeof(add)) (mp->b_rptr + p->ADDR_offset);
	if (p->ADDR_length != sizeof(*add) + add->alen)
		goto badaddr;
	if ((err = sccp_state_req(q, sc, add, p->STATUS)))
		goto error;
	return n_ok_ack(q, sc, N_STATE_REQ, 0, 0);
      badaddr:
	err = NBADADDR;
	ptrace(("%s: %p: ERROR: bad address format\n", DRV_NAME, sc));
	goto error;
      noaddr:
	err = NNOADDR;
	ptrace(("%s: %p: ERROR: could not assign address\n", DRV_NAME, sc));
	goto error;
      emsgsize:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: bad primitive format\n", DRV_NAME, sc));
	goto error;
      error:
	return n_error_ack(q, sc, N_STATE_REQ, err);
}

/*
 *  TPI Primitives
 *  -------------------------------------------------------------------------
 */
/*
 *  T_CONN_REQ
 *  -----------------------------------
 */
STATIC int
t_conn_req(queue_t *q, struct sc *sc, mblk_t *mp)
{
	int err;
	const struct T_conn_req *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *a;
	uchar *op, *oe;
	struct t_opthdr *oh;
	ulong *sls = NULL;
	ulong *pri = NULL;
	ulong *pcl = NULL;
	ulong *imp = NULL;
	if (sccp_get_t_state(sc) != TS_IDLE)
		goto outstate;
	sccp_set_t_state(sc, TS_WACK_CREQ);
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (p->OPT_length) {
		if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
			goto einval;
		/* 
		   check options ranges and format */
		for (op = mp->b_rptr + p->OPT_offset, oe = op + p->OPT_length, oh = (typeof(oh)) op;
		     op + sizeof(*oh) <= oe && oh->len >= sizeof(*oh)
		     && op + oh->len <= oe; op += oh->len, oh = (typeof(oh)) op) {
			t_scalar_t val = *((t_scalar_t *) (oh + 1));
			switch (oh->level) {
			case T_SS7_SCCP:
				switch (oh->name) {
				case T_SCCP_PCLASS:
					if (val < 0 || val > 3)
						goto badopt3;
					if (val != 2 && val != 3)
						goto notsupport;
					continue;
				}
			}
		}
		if (op != oe)
			goto badopt;
	}
	a = (struct sccp_addr *) (mp->b_rptr + p->DEST_offset);
	if (!p->DEST_length)
		goto noaddr;
	if ((mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
	    || (p->DEST_length < sizeof(struct sccp_addr) + a->alen) || !a->ssn)
		goto badaddr;
	if (sc->cred.cr_uid != 0 && !a->ssn)
		goto access;
	fixme(("select a source local reference number\n"));
	if (p->OPT_length) {
		for (op = mp->b_rptr + p->OPT_offset, oe = op + p->OPT_length, oh = (typeof(oh)) op;
		     op + sizeof(*oh) <= oe && oh->len >= sizeof(*oh)
		     && op + oh->len <= oe; op += oh->len, oh = (typeof(oh)) op) {
			switch (oh->level) {
			case T_SS7_SCCP:
				switch (oh->name) {
				case T_SCCP_SEQ_CTRL:
					sls = (ulong *) (oh + 1);
					continue;
				case T_SCCP_PRIORITY:
					pri = (ulong *) (oh + 1);
					continue;
				case T_SCCP_PCLASS:
					pcl = (ulong *) (oh + 1);
					continue;
				case T_SCCP_IMPORTANCE:
					imp = (ulong *) (oh + 1);
					continue;
				default:
				case T_SCCP_PVAR:
				case T_SCCP_MPLEV:
				case T_SCCP_DEBUG:
				case T_SCCP_CLUSTER:
				case T_SCCP_RET_ERROR:
					/* 
					   ignore */
					continue;
				}
			}
		}
	}
	if ((err = sccp_conn_req(q, sc, a, sls, pri, pcl, imp, mp->b_cont)) < 0)
		goto error;
	if ((err = t_ok_ack(q, sc, p->PRIM_type, 0, 0)) < 0)
		goto error;
	return (QR_TRIMMED);
      access:
	err = TACCES;
	ptrace(("%s: %p: ERROR: no permission for requested address\n", DRV_NAME, sc));
	goto error;
      badaddr:
	err = TBADADDR;
	ptrace(("%s: %p: ERROR: address is unusable\n", DRV_NAME, sc));
	goto error;
      noaddr:
	err = TNOADDR;
	ptrace(("%s: %p: ERROR: no destination address\n", DRV_NAME, sc));
	goto error;
      badopt:
	err = TBADOPT;
	ptrace(("%s: %p: ERROR: options are unusable\n", DRV_NAME, sc));
	goto error;
      notsupport:
	err = TNOTSUPPORT;
	ptrace(("%s: %p: ERROR: primitive not supported for protocol class\n", DRV_NAME, sc));
	goto error;
      badopt3:
	err = TBADOPT;
	ptrace(("%s: %p: ERROR: invalid protocol class\n", DRV_NAME, sc));
	goto error;
      einval:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: invalid message format\n", DRV_NAME, sc));
	goto error;
      outstate:
	err = TOUTSTATE;
	ptrace(("%s: %p: ERROR: would place interface out of state\n", DRV_NAME, sc));
	goto error;
      error:
	return t_error_ack(q, sc, T_CONN_REQ, err);
}

/*
 *  T_CONN_RES
 *  -----------------------------------
 */
STATIC INLINE mblk_t *
t_seq_check(struct sc *sc, ulong seq)
{
	/* 
	   IMPLEMENTATION NOTE:- Sequence numbers are actually the address of the mblk which
	   contains the CR message and contains the contents of this message as a connection
	   indication.  To find if a particular sequence number is valid, we walk the connection
	   indication queue looking for an mblk with the same address as the sequence number.
	   Sequence numbers are only valid on the stream for which the connection indication is
	   queued. */
	mblk_t *mp;
	bufq_lock(&sc->conq);
	for (mp = bufq_head(&sc->conq); mp && mp != (mblk_t *) seq; mp = mp->b_next) ;
	bufq_unlock(&sc->conq);
	usual(mp);
	return (mp);
}
STATIC INLINE struct sc *
t_tok_check(struct sc *sc, ulong tok)
{
	/* 
	   IMPLEMENTATION NOTE:- Tokens are actually the address of the read queue.  To find if a
	   particular token is valid, we walk the open list looking for an open device with a read
	   queue with the same address as the token.  Tokens are only valid for SCCP streams of the 
	   same provider type.  (That is, a TPI connection indication cannot be accepted on an NPI
	   stream. */
	struct sc *ap;
	queue_t *aq = (queue_t *) tok;
	for (ap = master.sc.list; ap && ap->oq != aq; ap = ap->next) ;
	usual(ap);
	return ((struct sc *) ap);
}
STATIC int
t_conn_res(queue_t *q, struct sc *sc, mblk_t *mp)
{
	int err;
	mblk_t *cp;
	struct sc *ap;
	const struct T_conn_res *p = (typeof(p)) mp->b_rptr;
	uchar *op, *oe;
	struct t_opthdr *oh;
	ulong ap_oldstate;
	if (sccp_get_t_state(sc) != TS_WRES_CIND)
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (p->OPT_length) {
		if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
			goto einval;
		/* 
		   check options ranges and format */
		for (op = mp->b_rptr + p->OPT_offset, oe = op + p->OPT_length, oh = (typeof(oh)) op;
		     op + sizeof(*oh) <= oe && oh->len >= sizeof(*oh)
		     && op + oh->len <= oe; op += oh->len, oh = (typeof(oh)) op) {
			t_scalar_t val = *((t_scalar_t *) (oh + 1));
			switch (oh->level) {
			case T_SS7_SCCP:
				switch (oh->name) {
				case T_SCCP_PCLASS:
					if (val < 0 || val > 3)
						goto badopt3;
					if (val != 2 && val != 3)
						goto notsupport;
					continue;
				}
			}
		}
		if (op != oe)
			goto badopt;
	}
	if (!(cp = t_seq_check(sc, p->SEQ_number)))
		goto badseq;
	if (!(ap = t_tok_check(sc, p->ACCEPTOR_id))
	    || (ap != sc && !((1 << sccp_get_t_state(sc)) & (TSF_UNBND | TSF_IDLE))))
		goto badf;
	if ((ap_oldstate = sccp_get_t_state(ap)) == TS_IDLE && ap->conind)
		goto resqlen;
	/* 
	   protect at least r00t streams from users */
	if (sc->cred.cr_uid != 0 && ap->cred.cr_uid == 0)
		goto access;
	if (p->OPT_length) {
		for (op = mp->b_rptr + p->OPT_offset, oe = op + p->OPT_length, oh = (typeof(oh)) op;
		     op + sizeof(*oh) <= oe && oh->len >= sizeof(*oh)
		     && op + oh->len <= oe; op += oh->len, oh = (typeof(oh)) op) {
			t_scalar_t val = *((t_scalar_t *) (oh + 1));
			switch (oh->level) {
			case T_SS7_SCCP:
				switch (oh->name) {
				case T_SCCP_SEQ_CTRL:
					ap->sls = val;
					continue;
				case T_SCCP_PRIORITY:
					ap->mp = val;
					continue;
				case T_SCCP_PCLASS:
					ap->pcl = val;
					continue;
				case T_SCCP_IMPORTANCE:
					ap->imp = val;
					continue;
				default:
				case T_SCCP_PVAR:
				case T_SCCP_MPLEV:
				case T_SCCP_DEBUG:
				case T_SCCP_CLUSTER:
				case T_SCCP_RET_ERROR:
					/* 
					   ignore */
					continue;
				}
			}
		}
	}
	sccp_set_t_state(ap, TS_DATA_XFER);
	if ((err = sccp_conn_res(q, sc, cp, ap, mp->b_cont)) < 0)
		goto call_error;
	if ((err = t_ok_ack(q, sc, p->PRIM_type, (ulong) cp, (ulong) ap)) < 0)
		goto call_error;
	return (QR_TRIMMED);
      call_error:
	sccp_set_t_state(ap, ap_oldstate);
	goto error;
      access:
      resqlen:
	err = TRESQLEN;
	ptrace(("%s: %p: ERROR: accepting queue is listening\n", DRV_NAME, sc));
	goto error;
      badf:
	err = TBADF;
	ptrace(("%s: %p: ERROR: accepting queue id is invalid\n", DRV_NAME, sc));
	goto error;
      badseq:
	err = TBADSEQ;
	ptrace(("%s: %p: ERROR: connection ind reference is invalid\n", DRV_NAME, sc));
	goto error;
      badopt:
	err = TBADOPT;
	ptrace(("%s: %p: ERROR: options are bad\n", DRV_NAME, sc));
	goto error;
      notsupport:
	err = TNOTSUPPORT;
	ptrace(("%s: %p: ERROR: primitive not supported for protocol class\n", DRV_NAME, sc));
	goto error;
      badopt3:
	err = TBADOPT;
	ptrace(("%s: %p: ERROR: invalid protocol class\n", DRV_NAME, sc));
	goto error;
      einval:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, sc));
	goto error;
      outstate:
	err = TOUTSTATE;
	ptrace(("%s: %p: ERROR: would place interface out of state\n", DRV_NAME, sc));
	goto error;
      error:
	return t_error_ack(q, sc, T_CONN_RES, err);
}

/*
 *  T_DISCON_REQ
 *  -----------------------------------
 */
STATIC int
t_discon_req(queue_t *q, struct sc *sc, mblk_t *mp)
{
	int err;
	mblk_t *cp = NULL;
	struct T_discon_req *p = (typeof(p)) mp->b_rptr;
	if (((1 << sccp_get_t_state(sc)) &
	     ~(TSF_WCON_CREQ | TSF_WRES_CIND | TSF_DATA_XFER | TSF_WIND_ORDREL | TSF_WREQ_ORDREL)))
		goto outstate;
	switch (sccp_get_t_state(sc)) {
	case TS_WCON_CREQ:
		sccp_set_t_state(sc, TS_WACK_DREQ6);
		break;
	case TS_WRES_CIND:
		sccp_set_t_state(sc, TS_WACK_DREQ7);
		break;
	case TS_DATA_XFER:
		sccp_set_t_state(sc, TS_WACK_DREQ9);
		break;
	case TS_WIND_ORDREL:
		sccp_set_t_state(sc, TS_WACK_DREQ10);
		break;
	case TS_WREQ_ORDREL:
		sccp_set_t_state(sc, TS_WACK_DREQ11);
		break;
	default:
		goto outstate;
	}
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (sccp_get_t_state(sc) == TS_WACK_DREQ7 && !(cp = t_seq_check(sc, p->SEQ_number)))
		goto badseq;
	if ((err = sccp_discon_req(q, sc, cp, mp->b_cont)))
		goto error;
	if ((err = t_ok_ack(q, sc, p->PRIM_type, p->SEQ_number, 0)))
		goto error;
	return (QR_TRIMMED);
      badseq:
	err = TBADSEQ;
	ptrace(("%s: %p: ERROR: connection ind reference is invalid\n", DRV_NAME, sc));
	goto error;
      einval:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, sc));
	goto error;
      outstate:
	err = TOUTSTATE;
	ptrace(("%s: %p: ERROR: would place interface out of state\n", DRV_NAME, sc));
	goto error;
      error:
	return t_error_ack(q, sc, T_DISCON_REQ, err);
}

/*
 *  T_DATA_REQ
 *  -----------------------------------
 */
STATIC int
t_error_reply(queue_t *q, struct sc *sc, int err)
{
	mblk_t *mp;
	switch (err) {
	case -EBUSY:
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
	case QR_DONE:
	case QR_ABSORBED:
	case QR_TRIMMED:
		ptrace(("%s: %p: ERROR: No error condition\n", DRV_NAME, sc));
		return (err);
	}
	if ((mp = ss7_allocb(q, 2, BPRI_HI))) {
		mp->b_datap->db_type = M_ERROR;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		putnext(sc->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
STATIC int
t_write(queue_t *q, struct sc *sc, mblk_t *mp)
{
	int err;
	if (sccp_get_t_state(sc) == TS_IDLE)
		goto discard;
	if ((1 << sccp_get_t_state(sc)) & ~(TSF_DATA_XFER | TSF_WREQ_ORDREL))
		goto eproto;
	if ((err = sccp_data_req(q, sc, 0, 0, 0, mp)))
		goto error;
	return (QR_ABSORBED);
      eproto:
	err = -EPROTO;
	ptrace(("%s: %p: ERROR: would place interface out of state\n", DRV_NAME, sc));
	goto error;
      discard:
	err = 0;
	ptrace(("%s: %p: ERROR: ignoring in idle state\n", DRV_NAME, sc));
	goto error;
      error:
	return t_error_reply(q, sc, err);
}
STATIC int
t_data_req(queue_t *q, struct sc *sc, mblk_t *mp)
{
	int err;
	struct T_data_req *p = (typeof(p)) mp->b_rptr;
	if (sccp_get_t_state(sc) == TS_IDLE)
		goto discard;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if ((1 << sccp_get_t_state(sc)) & ~(TSF_DATA_XFER | TSF_WREQ_ORDREL))
		goto eproto;
	{
		ulong exp = 0;		/* FIXME */
		ulong more = 0;		/* FIXME */
		ulong rcpt = 0;		/* FIXME */
		if ((err = sccp_data_req(q, sc, exp, more, rcpt, mp->b_cont)))
			goto error;
		return (QR_ABSORBED);
	}
      eproto:
	err = -EPROTO;
	ptrace(("%s: %p: ERROR: would place interface out of state\n", DRV_NAME, sc));
	goto error;
      einval:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, sc));
	goto error;
      discard:
	err = QR_DONE;
	ptrace(("%s: %p: ERROR: ignore in idle state\n", DRV_NAME, sc));
	goto error;
      error:
	return t_error_reply(q, sc, err);
}

/*
 *  T_EXDATA_REQ
 *  -----------------------------------
 */
STATIC int
t_exdata_req(queue_t *q, struct sc *sc, mblk_t *mp)
{
	int err;
	const struct T_exdata_req *p = (typeof(p)) mp->b_rptr;
	if (sccp_get_t_state(sc) == TS_IDLE)
		goto discard;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if ((1 << sccp_get_t_state(sc)) & ~(TSF_DATA_XFER | TSF_WREQ_ORDREL))
		goto eproto;
	{
		ulong exp = 0;		/* FIXME */
		ulong more = 0;		/* FIXME */
		ulong rcpt = 0;		/* FIXME */
		if ((err = sccp_data_req(q, sc, exp, more, rcpt, mp->b_cont)))
			goto error;
		return (QR_ABSORBED);	/* absorbed mp->b_cont */
	}
      eproto:
	err = -EPROTO;
	ptrace(("%s: %p: ERROR: would place interface out of state\n", DRV_NAME, sc));
	goto error;
      einval:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, sc));
	goto error;
      discard:
	err = QR_DONE;
	ptrace(("%s: %p: ERROR: ignore in idle state\n", DRV_NAME, sc));
	goto error;
      error:
	return t_error_reply(q, sc, err);
}

/*
 *  T_INFO_REQ           5 - Information Request
 *  -----------------------------------------------------------------
 */
STATIC int
t_info_req(queue_t *q, struct sc *sc, mblk_t *mp)
{
	(void) mp;
	return t_info_ack(q, sc);
}

/*
 *  T_BIND_REQ           6 - Bind a TS user to a transport address
 *  -----------------------------------------------------------------
 */
STATIC int
t_bind_req(queue_t *q, struct sc *sc, mblk_t *mp)
{
	int err;
	const struct T_bind_req *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *add;
	const size_t add_len = sizeof(*add);
	if (sccp_get_t_state(sc) != TS_UNBND)
		goto toutstate;
	sccp_set_t_state(sc, TS_WACK_BREQ);
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	add = (typeof(add)) (mp->b_rptr + p->ADDR_offset);
	if ((mp->b_wptr < mp->b_rptr + p->ADDR_offset + p->ADDR_length)
	    || (p->ADDR_length != add_len))
		goto tbadaddr;
	/* 
	   we don't allow wildcards just yet */
	if (!add->ssn)
		goto tnoaddr;
	if (sc->cred.cr_uid != 0 && !add->ssn)
		goto tacces;
	if ((err = sccp_bind_req(q, sc, add, p->CONIND_number)))
		goto error;
	return t_bind_ack(q, sc, &sc->src);
      tacces:
	err = TACCES;
	ptrace(("%s: %p: ERROR: no permission for address\n", DRV_NAME, sc));
	goto error;
      tnoaddr:
	err = TNOADDR;
	ptrace(("%s: %p: ERROR: couldn't allocate address\n", DRV_NAME, sc));
	goto error;
      tbadaddr:
	err = TBADADDR;
	ptrace(("%s: %p: ERROR: address is invalid\n", DRV_NAME, sc));
	goto error;
      einval:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, sc));
	goto error;
      toutstate:
	err = TOUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, sc));
	goto error;
      error:
	return t_error_ack(q, sc, T_BIND_REQ, err);
}

/*
 *  T_UNBIND_REQ         7 - Unbind TS user from transport address
 *  -----------------------------------------------------------------
 */
STATIC int
t_unbind_req(queue_t *q, struct sc *sc, mblk_t *mp)
{
	int err;
	const struct T_unbind_req *p = (typeof(p)) mp->b_rptr;
	(void) p;
	if (sccp_get_t_state(sc) != TS_IDLE)
		goto toutstate;
	sccp_set_t_state(sc, TS_WACK_UREQ);
	if ((err = sccp_unbind_req(q, sc)))
		goto error;
	return t_ok_ack(q, sc, T_UNBIND_REQ, 0, 0);
      toutstate:
	err = TOUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, sc));
	goto error;
      error:
	return t_error_ack(q, sc, T_UNBIND_REQ, err);
}

/*
 *  T_UNITDATA_REQ      ?? - Unitdata request
 *  -----------------------------------------------------------------
 */
STATIC int
t_uderror_reply(queue_t *q, struct sc *sc, mblk_t *mp, int etype)
{
	struct T_uderror_ind *p = (typeof(p)) mp->b_rptr;
	switch (etype) {
	case -EBUSY:
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
	case QR_DONE:
	case QR_ABSORBED:
	case QR_TRIMMED:
		ptrace(("%s: %p: ERROR: No error condition\n", DRV_NAME, sc));
		return (etype);
	}
	if (etype > 0) {
		static const int offset =
		    sizeof(struct T_uderror_ind) - sizeof(struct T_unitdata_req);
		if (offset > 0) {
			/* 
			   shift addresses, if necessary */
			bcopy(mp->b_rptr + sizeof(*p), mp->b_rptr + sizeof(*p) + offset,
			      mp->b_wptr - mp->b_rptr - sizeof(*p));
			if (p->DEST_offset)
				p->DEST_offset += offset;
			if (p->OPT_offset)
				p->OPT_offset += offset;
			mp->b_wptr += offset;
		}
		p->PRIM_type = T_UDERROR_IND;
		p->ERROR_type = etype;
	} else {
		mp->b_datap->db_type = M_ERROR;
		mp->b_wptr = mp->b_rptr;
		*(mp->b_wptr)++ = -etype;
		*(mp->b_wptr)++ = -etype;
	}
	putnext(sc->oq, mp);
	return (QR_ABSORBED);
}
STATIC INLINE int
t_unitdata_req(queue_t *q, struct sc *sc, mblk_t *mp)
{
	int err;
	size_t dlen = mp->b_cont ? msgdsize(mp->b_cont) : 0;
	const struct T_unitdata_req *p = (typeof(p)) mp->b_rptr;
	struct sccp_addr *dst;
	const size_t dst_len = sizeof(*dst);
	ulong seq, pri, pcl, imp, ret;
	if (dlen == 0 || dlen > sc->mtu)
		goto tbaddata;
	if (sccp_get_t_state(sc) != TS_IDLE)
		goto toutstate;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	dst = (typeof(dst)) (mp->b_rptr + p->DEST_offset);
	if ((mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
	    || (p->DEST_length != dst_len))
		goto tbadaddr;
	if (sc->cred.cr_uid != 0 && dst->ssn != sc->src.ssn)
		goto eperm;
	seq = sc->sls;
	pri = sc->mp;
	pcl = sc->pcl;
	imp = sc->imp;
	ret = (sc->flags & SCCPF_RETURN_ERROR) ? 1 : 0;
	if (p->OPT_length) {
		uchar *op = mp->b_rptr + p->OPT_offset;
		uchar *oe = op + p->OPT_length;
		struct t_opthdr *oh = (typeof(oh)) op;
		for (; op + sizeof(*oh) <= oe && oh->len >= sizeof(*oh)
		     && op + oh->len <= oe; op += oh->len, oh = (typeof(oh)) op) {
			if (oh->level == T_SS7_SCCP)
				switch (oh->name) {
				case T_SCCP_SEQ_CTRL:
					seq = (*((t_uscalar_t *) (oh + 1)));
					continue;
				case T_SCCP_PRIORITY:
					pri = (*((t_uscalar_t *) (oh + 1)));
					continue;
				case T_SCCP_PCLASS:
					pcl = (*((t_uscalar_t *) (oh + 1)));
					continue;
				case T_SCCP_IMPORTANCE:
					imp = (*((t_uscalar_t *) (oh + 1)));
					continue;
				case T_SCCP_RET_ERROR:
					ret = (*((t_uscalar_t *) (oh + 1)));
					continue;
				}
		}
	}
	if ((err = sccp_unitdata_req(q, sc, dst, pcl, ret, imp, seq, pri, mp->b_cont)))
		goto error;
	return (QR_TRIMMED);
      eperm:
	err = -EPROTO;
	ptrace(("%s: %p: ERROR: no permission to send to address\n", DRV_NAME, sc));
	goto error;
      tbadaddr:
	err = -EPROTO;
	ptrace(("%s: %p: ERROR: bad destination address\n", DRV_NAME, sc));
	goto error;
      einval:
	err = -EPROTO;
	ptrace(("%s: %p: ERROR: invalid primitive\n", DRV_NAME, sc));
	goto error;
      toutstate:
	err = -EPROTO;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, sc));
	goto error;
      tbaddata:
	err = -EPROTO;
	ptrace(("%s: %p: ERROR: bad data size\n", DRV_NAME, sc));
	goto error;
      error:
	return t_uderror_reply(q, sc, mp, err);
}

/*
 *  T_OPTMGMT_REQ        9 - Options management request
 *  -----------------------------------------------------------------
 *  The T_OPTMGMT_REQ is responsible for establishing options while the stream is in the T_IDLE state.  When the
 *  stream is bound to a local address using the T_BIND_REQ, the settings of options with end-to-end significance
 *  will have an affect on how the driver response to an INIT with INIT-ACK for SCCP.  For example, the bound list
 *  of addresses is the list of addresses that will be sent in the INIT-ACK.  The number of inbound streams and
 *  outbound streams are the numbers that will be used in the INIT-ACK.
 */
/*
 *  Errors:
 *
 *  TACCES:     the user did not have proper permissions for the user of the requested options.
 *
 *  TBADFLAG:   the flags as specified were incorrect or invalid.
 *
 *  TBADOPT:    the options as specified were in an incorrect format, or they contained invalid information.
 *
 *  TOUTSTATE:  the primitive would place the transport interface out of state.
 *
 *  TNOTSUPPORT: this prmitive is not supported.
 *
 *  TSYSERR:    a system error has occured and the UNIX system error is indicated in the primitive.
 */
STATIC int
t_optmgmt_req(queue_t *q, struct sc *sc, mblk_t *mp)
{
	int err;
	const struct T_optmgmt_req *p = (typeof(p)) mp->b_rptr;
#ifdef TS_WACK_OPTREQ
	if (sccp_get_t_state(sc) == TS_IDLE)
		sccp_set_t_state(sc, TS_WACK_OPTREQ);
#endif
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
		goto tbadopt1;
	{
		ulong flags = p->MGMT_flags;
		size_t opt_len = p->OPT_length;
		uchar *opt_ptr = mp->b_rptr + p->OPT_offset;
		struct sccp_opts ops = { 0L, NULL, };
		struct sccp_opts *opsp = NULL;
		if (!opt_len)
			goto tbadopt2;
		if ((err = sccp_parse_opts(sc, &ops, opt_ptr, opt_len)))
			goto tbadopt3;
		opsp = &ops;
		switch (flags) {
		case T_CHECK:
			sccp_opt_check(sc, opsp);
			return t_optmgmt_ack(q, sc, flags, opsp);
		case T_NEGOTIATE:
			sccp_opt_negotiate(sc, opsp);
			return t_optmgmt_ack(q, sc, flags, opsp);
		case T_DEFAULT:
			sccp_opt_default(sc, opsp);
			return t_optmgmt_ack(q, sc, flags, opsp);
		case T_CURRENT:
			sccp_opt_current(sc, opsp);
			return t_optmgmt_ack(q, sc, flags, opsp);
		default:
			goto tbadflag;
		}
	}
      tbadflag:
	err = TBADFLAG;
	ptrace(("%s: %p: ERROR: bad options flags\n", DRV_NAME, sc));
	goto error;
      tbadopt3:
	err = err;
	ptrace(("%s: %p: ERROR: option parsing error\n", DRV_NAME, sc));
	goto error;
      tbadopt2:
	err = TBADOPT;
	ptrace(("%s: %p: ERROR: invalid options\n", DRV_NAME, sc));
	goto error;
      tbadopt1:
	err = TBADOPT;
	ptrace(("%s: %p: ERROR: invalid options\n", DRV_NAME, sc));
	goto error;
      einval:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, sc));
	goto error;
      error:
	return t_error_ack(q, sc, T_OPTMGMT_REQ, err);
}

/*
 *  T_ORDREL_REQ
 *  -----------------------------------
 */
STATIC int
t_ordrel_req(queue_t *q, struct sc *sc, mblk_t *mp)
{
	int err;
	if ((1 << sccp_get_t_state(sc)) & ~(TSF_DATA_XFER | TSF_WREQ_ORDREL))
		goto outstate;
	switch (sccp_get_t_state(sc)) {
	case TS_DATA_XFER:
		sccp_set_t_state(sc, TS_WIND_ORDREL);
		break;
	case TS_WREQ_ORDREL:
		sccp_set_t_state(sc, TS_IDLE);
		break;
	}
	if ((err = sccp_ordrel_req(q, sc)))
		goto error;
	return (QR_DONE);
      outstate:
	err = TOUTSTATE;
	ptrace(("%s: %p: ERROR: would place interface out of state\n", DRV_NAME, sc));
	goto error;
      error:
	return t_error_ack(q, sc, T_ORDREL_REQ, err);
}

/*
 *  T_OPTDATA_REQ
 *  -----------------------------------
 */
STATIC int
t_optdata_req(queue_t *q, struct sc *sc, mblk_t *mp)
{
	int err;
	const struct T_optdata_req *p = (typeof(p)) mp->b_rptr;
	uchar *op, *oe;
	struct t_opthdr *oh;
	ulong exp, more, rcpt;
	if (sccp_get_t_state(sc) == TS_IDLE)
		goto discard;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (p->OPT_length) {
		if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
			goto einval;
		/* 
		   check options ranges and format */
		for (op = mp->b_rptr + p->OPT_offset, oe = op + p->OPT_length, oh = (typeof(oh)) op;
		     op + sizeof(*oh) <= oe && oh->len >= sizeof(*oh)
		     && op + oh->len <= oe; op += oh->len, oh = (typeof(oh)) op) ;
		if (op != oe)
			goto badopt;
	}
	if ((1 << sccp_get_t_state(sc)) & ~(TSF_DATA_XFER | TSF_WREQ_ORDREL))
		goto outstate;
	if (p->OPT_length) {
		for (op = mp->b_rptr + p->OPT_offset, oe = op + p->OPT_length, oh = (typeof(oh)) op;
		     op + sizeof(*oh) <= oe && oh->len >= sizeof(*oh)
		     && op + oh->len <= oe; op += oh->len, oh = (typeof(oh)) op) {
			t_scalar_t val = *((t_scalar_t *) (oh + 1));
			switch (oh->level) {
			case T_SS7_SCCP:
				switch (oh->name) {
				case T_SCCP_PRIORITY:
					sc->mp = val;
					continue;
				case T_SCCP_IMPORTANCE:
					sc->imp = val;
					continue;
				default:
				case T_SCCP_PVAR:
				case T_SCCP_MPLEV:
				case T_SCCP_DEBUG:
				case T_SCCP_PCLASS:
				case T_SCCP_CLUSTER:
				case T_SCCP_SEQ_CTRL:
				case T_SCCP_RET_ERROR:
					/* 
					   ignore */
					continue;
				}
			}
		}
	}
	fixme(("Set these options\n"));
	exp = 0;
	more = 0;
	rcpt = 0;
	if ((err = sccp_data_req(q, sc, exp, more, rcpt, mp->b_cont)))
		goto error;
	return (QR_ABSORBED);
      outstate:
	err = -EPROTO;
	ptrace(("%s: %p: ERROR: would place interface out of state\n", DRV_NAME, sc));
	goto error;
      badopt:
	err = -EPROTO;
	ptrace(("%s: %p: ERROR: invalid options format\n", DRV_NAME, sc));
	goto error;
      einval:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, sc));
	goto error;
      discard:
	err = QR_DONE;
	ptrace(("%s: %p: ERROR: ignore in idle state\n", DRV_NAME, sc));
	goto error;
      error:
	return t_error_reply(q, sc, err);
}

#ifdef T_ADDR_REQ
/*
 *  T_ADDR_REQ          25 - address request
 *  -----------------------------------------------------------------
 */
STATIC int
t_addr_req(queue_t *q, struct sc *sc, mblk_t *mp)
{
	(void) mp;
	switch (sccp_get_t_state(sc)) {
	case TS_UNBND:
	case TS_WACK_BREQ:
		return t_addr_ack(q, sc, NULL, NULL);
	case TS_WACK_UREQ:
	case TS_IDLE:
		return t_addr_ack(q, sc, &sc->src, NULL);
	}
	return t_error_ack(q, sc, T_ADDR_REQ, TOUTSTATE);
}
#endif

#ifdef T_CAPABILITY_REQ
/*
 *  T_CAPABILITY_REQ
 *  -----------------------------------
 */
STATIC int
t_capability_req(queue_t *q, struct sc *sc, mblk_t *mp)
{
	fixme(("Implement this function\n"));
	return t_error_ack(q, sc, T_CAPABILITY_REQ, TNOTSUPPORT);
}
#endif

/*
 *  -------------------------------------------------------------------------
 *
 *  Timer Events
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  TIMEOUT Connection Establishment
 *  -----------------------------------
 *  Delay to send an IT message on a connection section when there are no messages to send. (5 to 10 minutes)
 */
STATIC int
sc_tcon_timeout(struct sc *sc)
{
	int err;
	switch (sccp_get_state(sc)) {
	case SS_WCON_CREQ:
		/* 
		   Figure C.2/Q.714 Sheet 2 of 7: Figure 2A/T1.112.4-2000 Sheet 2 of 4: release
		   resources and local reference. Freeze local reference.  Send an N_DISCON_IND to
		   the user. Move to the idle state. */
		sccp_release(sc);
		if ((err =
		     sccp_discon_ind(NULL, sc, N_PROVIDER, SCCP_REFC_EXPIRATION_OF_NC_ESTAB_TIMER,
				     NULL, NULL, NULL)))
			return (err);
		sccp_set_state(sc, SS_IDLE);
		return (QR_DONE);
	case SS_WCON_CREQ2:
		/* 
		   Figure C.2/Q.714 Sheet 3 of 7: Figure 2A/T1.112.4-2000 Sheet 3 of 4: In this
		   case, we have received a disconnect from the user before receiving the CC from
		   the peer.  We have not received a CC from the peer within the timeout so we will 
		   now disconnect.  Release resource and local reference.  Freeze the local
		   reference.  Move to the idle state. */
		sccp_release(sc);
		sccp_set_state(sc, SS_IDLE);
		return (QR_DONE);
	default:
		swerr();
		return (-EFAULT);
	}
}

/*
 *  TIMEOUT Inactivity Send
 *  -----------------------------------
 *  Waiting to receive a message on a connection section. (11 to 21 minutes)
 */
STATIC int
sc_tias_timeout(struct sc *sc)
{
	int err;
	ulong more = 0;
	switch (sccp_get_state(sc)) {
	case SS_DATA_XFER:
		/* 
		   Figure C.4/Q.714 Sheet 4 of 4: Send an inactivity test and restart the send
		   inactivity timer. Figure 2C/T1.112.4-2002 2of2: same. */
		if ((err =
		     sccp_send_it(NULL, sc->sp.sp, sc->sp.sp->add.pc, sc->sls, sc->dlr, sc->slr,
				  sc->pcl, sc->ps, sc->pr, more, sc->credit)))
			return (err);
		sccp_timer_start(sc, tias);
		return (QR_DONE);
	default:
		err = -EFAULT;
		swerr();
		goto error;
	}
      error:
	return (err);
}

/*
 *  TIMEOUT Inactivity Receive
 *  -----------------------------------
 *  Waiting for release complete message. (10 to 20 seconds)
 */
STATIC int
sc_tiar_timeout(struct sc *sc)
{
	int err;
	switch (sccp_get_state(sc)) {
	case SS_DATA_XFER:
		switch (sc->proto.pvar & SS7_PVAR_MASK) {
		case SS7_PVAR_ANSI:
		{
			/* 
			   Figure 2A/T1.112.4-2000 4of4: Figure 2B/T1.112.4-2000 2of2: send
			   N_DISCON_IND to user, release resource for connection section, stop
			   inactivity timers, send RLSD, start released and interval timers. */
			sccp_timer_stop(sc, tiar);
			sccp_timer_stop(sc, tias);
			if ((err =
			     sccp_send_rlsd(NULL, sc->sp.sp, sc->sp.sp->add.pc, sc->sls, sc->dlr,
					    sc->slr,
					    SCCP_RELC_EXPIRATION_OF_RECEIVE_INACTIVITY_TIMER, NULL,
					    NULL)))
				return (err);
			{
				int cause = 0;
				fixme(("Figure out cause\n"));
				if ((err =
				     sccp_discon_ind(NULL, sc, N_PROVIDER, cause, NULL, NULL,
						     NULL)))
					return (err);
			}
			sccp_timer_start(sc, trel);
			sccp_timer_start(sc, tint);
			sccp_set_state(sc, SS_WCON_DREQ);
			return (QR_DONE);
		}
		default:
		case SS7_PVAR_ITUT:
		{
			/* 
			   Figure C.2/Q.714 Sheet 5 of 7: Figure C.3/Q.714 Sheet 4 of 6: send
			   N_DISCON_IND to user, stop inactivity timers, start release timer, move
			   to disconnect pending state. */
			sccp_release(sc);
			if ((err =
			     sccp_discon_ind(NULL, sc, N_PROVIDER,
					     SCCP_RELC_EXPIRATION_OF_RECEIVE_INACTIVITY_TIMER, NULL,
					     NULL, NULL)))
				return (err);
			if ((err =
			     sccp_send_rlsd(NULL, sc->sp.sp, sc->sp.sp->add.pc, sc->sls, sc->dlr,
					    sc->slr,
					    SCCP_RELC_EXPIRATION_OF_RECEIVE_INACTIVITY_TIMER, NULL,
					    &sc->imp)))
				return (err);
			sccp_timer_start(sc, trel);
			sccp_set_state(sc, SS_WCON_DREQ);
			return (QR_DONE);
		}
		}
	}
	err = -EFAULT;
	swerr();
	return (err);
}

/*
 *  TIMEOUT Released
 *  -----------------------------------
 *  Waiting for release complete message; or to repeat sending released message after the initial T(rel) expiry. (10
 *  to 20 seconds)
 */
STATIC int
sc_trel_timeout(struct sc *sc)
{
	int err;
	switch (sccp_get_state(sc)) {
	case SS_WCON_DREQ:
		switch (sc->proto.pvar & SS7_PVAR_MASK) {
		case SS7_PVAR_ANSI:
			/* 
			   Figure 2A/T1.112.4-2000 Sheet 4 of 4: Figure 2B/T1.112.4-2000 Sheet 2 of 
			   2: send RLSD, restart released timer. */
			if ((err =
			     sccp_send_rlsd(NULL, sc->sp.sp, sc->sp.sp->add.pc, sc->sls, sc->dlr,
					    sc->slr, SCCP_RELC_UNQUALIFIED, NULL, NULL)))
				return (err);
			sccp_timer_start(sc, trel);
			return (QR_DONE);
		default:
		case SS7_PVAR_ITUT:
			/* 
			   Figure C.2/Q.714 Sheet 6 of 7: Send RLSD again, start the interval
			   timer, start the repeat release timer. */
			if ((err =
			     sccp_send_rlsd(NULL, sc->sp.sp, sc->sp.sp->add.pc, sc->sls, sc->dlr,
					    sc->slr, SCCP_RELC_UNQUALIFIED, NULL, &sc->imp)))
				return (err);
			sccp_timer_start(sc, tint);
			sccp_timer_start(sc, trel2);
			return (QR_DONE);
		}
	default:
		swerr();
		return (-EFAULT);
	}
}

/*
 *  TIMEOUT Repeat Released
 *  -----------------------------------
 *  Waiting for release complete message; or to repeat sending released message after the initial T(rel) expiry. (10
 *  to 20 seconds)
 */
STATIC int
sc_trel2_timeout(struct sc *sc)
{
	int err;
	switch (sccp_get_state(sc)) {
	case SS_WCON_DREQ:
		switch (sc->proto.pvar & SS7_PVAR_MASK) {
		case SS7_PVAR_ANSI:
			/* 
			   Figure C.2/Q.714 Sheet 7 of 7: send another RLSD, restart the repeat
			   release timer. */
			if ((err =
			     sccp_send_rlsd(NULL, sc->sp.sp, sc->sp.sp->add.pc, sc->sls, sc->dlr,
					    sc->slr, SCCP_RELC_UNQUALIFIED, NULL, NULL)))
				return (err);
			sccp_timer_start(sc, trel2);
			return (QR_DONE);
		default:
		case SS7_PVAR_ITUT:
			/* 
			   Figure C.2/Q.714 Sheet 7 of 7: send another RLSD, restart the repeat
			   release timer. */
			if ((err =
			     sccp_send_rlsd(NULL, sc->sp.sp, sc->sp.sp->add.pc, sc->sls, sc->dlr,
					    sc->slr, SCCP_RELC_UNQUALIFIED, NULL, &sc->imp)))
				return (err);
			sccp_timer_start(sc, trel2);
			return (QR_DONE);
		}
	default:
		swerr();
		return (-EFAULT);
	}
}

/*
 *  TIMEOUT Interrupt Released
 *  -----------------------------------
 *  Waiting for release complete message; or to release connection resources, fress the LRN and alert a maintenance
 *  function after the initial T(rel) expiry. (extending to 1 minute)
 */
STATIC int
sc_tint_timeout(struct sc *sc)
{
	switch (sccp_get_state(sc)) {
	case SS_WCON_DREQ:
		/* 
		   Figure C.2/Q.714 Sheet 6 of 7: inform maintenance, release resources and local
		   reference number, freeze local reference, stop release and interval timers, move 
		   to the idle state. */
		fixme(("Inform maintenance\n"));
		/* 
		   Figure 2A/T1.112.4-2000 Sheet 4 of 4: Figure 2B/T1.112.4-2000 Sheet 2 of 2: ASNI 
		   says to inform maintenance and then place the connection section into the
		   Maintenance Blocking state. */
		fixme(("Handle ANSI\n"));
		sccp_release(sc);
		sccp_set_state(sc, SS_IDLE);
		return (QR_DONE);
	default:
		swerr();
		return (-EFAULT);
	}
}

/*
 *  TIMEOUT Guard
 *  -----------------------------------
 *  Waiting to resume normal procedure for temporary connection sections during the restart procedure (see
 *  Q.714/3.8). (23 to 25 minutes)
 */
STATIC int
sc_tguard_timeout(struct sc *sc)
{
	switch (sccp_get_state(sc)) {
	default:
		swerr();
		return (-EFAULT);
	}
}

/*
 *  TIMEOUT Reset
 *  -----------------------------------
 *  Waiting to release temporary connection section or alert maintenance function after reset request message is
 *  sent.  (10 to 20 seconds).
 */
STATIC int
sc_tres_timeout(struct sc *sc)
{
	int err;
	switch (sccp_get_state(sc)) {
	case SS_BOTH_RESET:
		/* 
		   Figure C.6/Q.174 Sheet 3 of 4: same as Figure C.6/Q.714 Sheet 2 of 4: below. */
	case SS_WCON_RREQ:
		if (1) {	/* temporary connection */
			switch (sc->proto.pvar & SS7_PVAR_MASK) {
			case SS7_PVAR_ANSI:
				/* 
				   Figure 2E/T1.112.4-2000: if this is a temporary connection,
				   release resources for the connection section, stop inactivity
				   timers, send a RLSD, start release and interval timers, move to
				   the disconnect pending state. */
				sccp_timer_stop(sc, tiar);
				sccp_timer_stop(sc, tias);
				if ((err =
				     sccp_send_rlsd(NULL, sc->sp.sp, sc->sp.sp->add.pc, sc->sls,
						    sc->dlr, sc->slr,
						    SCCP_RELC_EXPIRATION_OF_RESET_TIMER, NULL,
						    NULL)))
					return (err);
				sccp_timer_start(sc, trel);
				sccp_timer_start(sc, tint);
				sccp_set_state(sc, SS_WCON_DREQ);
				return (QR_DONE);
			default:
			case SS7_PVAR_ITUT:
				/* 
				   Figure C.6/Q.714 Sheet 2 of 4: If this is a temporary connection 
				   section then (Figure C.2/Q.714 Sheet 5 of 7) release resources
				   an local reference, freeze local reference, send an N_DISCON_IND 
				   to the user, stop inactivity timers and move the idle state. */
				sccp_release(sc);
				if ((err =
				     sccp_discon_ind(NULL, sc, N_PROVIDER,
						     SCCP_RELC_EXPIRATION_OF_RESET_TIMER, NULL,
						     NULL, NULL)))
					return (err);
				sccp_set_state(sc, SS_IDLE);
				return (QR_DONE);
			}
		} else {
			/* 
			   Figure 2E/T1.112.4-2000: same.  Figure C.6/Q.714 Sheet 2 of 4: If this
			   is a permanent connection, inform maintenance and place the connection
			   section in management blocking state. */
			fixme(("Handle permanent connection\n"));
			sccp_set_state(sc, SS_IDLE);
			return (-EFAULT);
		}
	default:
		swerr();
		return (-EFAULT);
	}
}

/*
 *  TIMEOUT Reassembly
 *  -----------------------------------
 *  Waiting to receive all the segments of the remaining segments single segmented message after receiving the first
 *  segment.  (10 to 20 seconds).
 */
STATIC int
sc_trea_timeout(struct sc *sc)
{
	switch (sccp_get_state(sc)) {
	default:
		swerr();
		return (-EFAULT);
	}
}

/*
 *  TIMEOUT Ack
 *  -----------------------------------
 *  Waiting to send a data acknowledgement. (ANSI only.)
 */
STATIC int
sc_tack_timeout(struct sc *sc)
{
	int err;
	switch (sccp_get_state(sc)) {
	case SS_DATA_XFER:
		/* 
		   Figure 2C/T1.112.4-2000 2of2: if an ack is outstanding, send an AK, clear ack
		   outstanding, reset the send inactivity timer and reset the ack timer. */
		if (sc->flags & SCCPF_ACK_OUT) {
			if ((err =
			     sccp_send_ak(NULL, sc->sp.sp, sc->sp.sp->add.pc, sc->mp, sc->sls,
					  sc->dlr, sc->pr, sc->credit)))
				return (err);
			sc->flags &= ~SCCPF_ACK_OUT;
			sccp_timer_start(sc, tias);
			sccp_timer_start(sc, tack);
		}
		return (QR_DONE);
	default:
		swerr();
		return (-EFAULT);
	}
}

/*
 *  TIMEOUT Gtt
 *  -----------------------------------
 *  Waiting to receive a GTT result.
 */
STATIC int
sp_tgtt_timeout(struct sp *sp)
{
	fixme(("Write this function\n"));
	swerr();
	return (-EFAULT);
}

/*
 *  TIMEOUT Attack
 *  -----------------------------------
 */
STATIC int
sr_tattack_timeout(struct sr *sr)
{
	fixme(("Write this function\n"));
	swerr();
	return (-EFAULT);
}

/*
 *  TIMEOUT Decay
 *  -----------------------------------
 */
STATIC int
sr_tdecay_timeout(struct sr *sr)
{
	fixme(("Write this function\n"));
	swerr();
	return (-EFAULT);
}

/*
 *  TIMEOUT Status Info
 *  -----------------------------------
 */
STATIC int
sr_tstatinfo_timeout(struct sr *sr)
{
	fixme(("Write this function\n"));
	swerr();
	return (-EFAULT);
}

/*
 *  TIMEOUT Subssytem Test
 *  -----------------------------------
 */
STATIC int
sr_tsst_timeout(struct sr *sr)
{
	fixme(("Write this function\n"));
	swerr();
	return (-EFAULT);
}

/*
 *  TIMEOUT Inhibit Subsystem Test
 *  -----------------------------------
 */
STATIC int
ss_tisst_timeout(struct ss *ss)
{
	fixme(("Write this function\n"));
	swerr();
	return (-EFAULT);
}

/*
 *  TIMEOUT Waiting for SOG
 *  -----------------------------------
 */
STATIC int
ss_twsog_timeout(struct ss *ss)
{
	fixme(("Write this function\n"));
	swerr();
	return (-EFAULT);
}

/*
 *  TIMEOUT Inhibit Subsystem Test
 *  -----------------------------------
 */
STATIC int
rs_tsst_timeout(struct rs *rs)
{
	fixme(("Write this function\n"));
	swerr();
	return (-EFAULT);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  IO Controls
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  GET - Get Object Configuration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
sccp_get_sc(struct sccp_config *arg, struct sc *sc, int size)
{
	struct sccp_conf_sc *cnf = (typeof(cnf)) (arg + 1);
	fixme(("Write this function\n"));
	return (-EFAULT);
}
STATIC int
sccp_get_na(struct sccp_config *arg, struct na *na, int size)
{
	struct sp *sp;
	struct sccp_conf_sp *chd;
	struct sccp_conf_na *cnf = (typeof(cnf)) (arg + 1);
	if (!na || (size -= sizeof(*cnf)) < sizeof(*arg))
		return (-EINVAL);
	cnf->proto = na->proto;
	/* 
	   write out list of signalling point configurations following na configuration */
	arg = (typeof(arg)) (cnf + 1);
	chd = (typeof(chd)) (arg + 1);
	for (sp = na->sp.list; sp && size >= sizeof(*arg) + sizeof(*chd) + sizeof(*arg);
	     sp = sp->na.next, size -= sizeof(*arg) + sizeof(*chd), arg =
	     (typeof(arg)) (chd + 1), chd = (typeof(chd)) (arg + 1)) {
		arg->type = SCCP_OBJ_TYPE_SP;
		arg->id = sp->id;
		chd->naid = sp->na.na ? sp->na.na->id : 0;
		chd->proto = sp->proto;
		chd->add = sp->add;
	}
	/* 
	   terminate list with zero object type */
	arg->type = 0;
	arg->id = 0;
	return (QR_DONE);
}
STATIC int
sccp_get_cp(struct sccp_config *arg, struct cp *cp, int size)
{
	struct sccp_conf_cp *cnf = (typeof(cnf)) (arg + 1);
	fixme(("Write this function\n"));
	return (-EFAULT);
}
STATIC int
sccp_get_ss(struct sccp_config *arg, struct ss *ss, int size)
{
	struct sc *sc;
	struct sccp_conf_sc *chd;
	struct sccp_conf_ss *cnf = (typeof(cnf)) (arg + 1);
	if (!ss || (size -= sizeof(*cnf)) < sizeof(*arg))
		return (-EINVAL);
	cnf->spid = ss->sp.sp ? ss->sp.sp->id : 0;
	cnf->ssn = ss->ssn;
	/* 
	   write out list of SCCP users following subsystem configuration */
	arg = (typeof(arg)) (cnf + 1);
	chd = (typeof(chd)) (arg + 1);
	/* 
	   bound CL users */
	for (sc = ss->cl.list; sc && size >= sizeof(*arg) + sizeof(*chd) + sizeof(*arg);
	     sc = sc->ss.next, size -= sizeof(*arg) + sizeof(*chd), arg =
	     (typeof(arg)) (chd + 1), chd = (typeof(chd)) (arg + 1)) {
		arg->type = SCCP_OBJ_TYPE_SC;
		arg->id = ss->id;
		todo(("Write out child info\n"));
	}
	/* 
	   bound CO users */
	for (sc = ss->co.list; sc && size >= sizeof(*arg) + sizeof(*chd) + sizeof(*arg);
	     sc = sc->ss.next, size -= sizeof(*arg) + sizeof(*chd), arg =
	     (typeof(arg)) (chd + 1), chd = (typeof(chd)) (arg + 1)) {
		arg->type = SCCP_OBJ_TYPE_SC;
		arg->id = ss->id;
		todo(("Write out child info\n"));
	}
	/* 
	   listening users */
	for (sc = ss->cr.list; sc && size >= sizeof(*arg) + sizeof(*chd) + sizeof(*arg);
	     sc = sc->ss.next, size -= sizeof(*arg) + sizeof(*chd), arg =
	     (typeof(arg)) (chd + 1), chd = (typeof(chd)) (arg + 1)) {
		arg->type = SCCP_OBJ_TYPE_SC;
		arg->id = ss->id;
		todo(("Write out child info\n"));
	}
	/* 
	   terminate list with zero object type */
	arg->type = 0;
	arg->id = 0;
	return (QR_DONE);
}
STATIC int
sccp_get_rs(struct sccp_config *arg, struct rs *rs, int size)
{
	struct sccp_conf_rs *cnf = (typeof(cnf)) (arg + 1);
	if (!rs || (size -= sizeof(*cnf)) < sizeof(*arg))
		return (-EINVAL);
	cnf->srid = rs->sr.sr ? rs->sr.sr->id : 0;
	cnf->ssn = rs->ssn;
	arg = (typeof(arg)) (cnf + 1);
	/* 
	   terminate list with zero object type */
	arg->type = 0;
	arg->id = 0;
	return (QR_DONE);
}
STATIC int
sccp_get_sr(struct sccp_config *arg, struct sr *sr, int size)
{
	struct rs *rs;
	struct sccp_conf_rs *chd;
	struct sccp_conf_sr *cnf = (typeof(cnf)) (arg + 1);
	if (!sr || (size -= sizeof(*cnf)) < sizeof(*arg))
		return (-EINVAL);
	cnf->spid = sr->sp.sp ? sr->sp.sp->id : 0;
	cnf->proto = sr->proto;
	cnf->add = sr->add;
	/* 
	   write out list of remote subsystems */
	arg = (typeof(arg)) (cnf + 1);
	chd = (typeof(chd)) (arg + 1);
	for (rs = sr->rs.list; rs && size >= sizeof(*arg) + sizeof(*chd) + sizeof(*arg);
	     rs = rs->sr.next, size -= sizeof(*arg) + sizeof(*chd), arg =
	     (typeof(arg)) (chd + 1), chd = (typeof(chd)) (arg + 1)) {
		arg->type = SCCP_OBJ_TYPE_RS;
		arg->id = rs->id;
		chd->srid = rs->sr.sr ? rs->sr.sr->id : 0;
		chd->ssn = rs->ssn;
	}
	/* 
	   terminate list with zero object type */
	arg->type = 0;
	arg->id = 0;
	return (QR_DONE);
}
STATIC int
sccp_get_sp(struct sccp_config *arg, struct sp *sp, int size)
{
	struct sr *sr;
	struct ss *ss;
	struct sccp_conf_sr *chr;
	struct sccp_conf_ss *chs;
	struct sccp_conf_sp *cnf = (typeof(cnf)) (arg + 1);
	if (!sp || (size -= sizeof(*cnf)) < sizeof(*arg))
		return (-EINVAL);
	cnf->naid = sp->na.na ? sp->na.na->id : 0;
	cnf->proto = sp->proto;
	cnf->add = sp->add;
	/* 
	   write out list of signalling relations */
	arg = (typeof(arg)) (cnf + 1);
	chr = (typeof(chr)) (arg + 1);
	for (sr = sp->sr.list; sr && size >= sizeof(*arg) + sizeof(*chr) + sizeof(*arg);
	     sr = sr->sp.next, size -= sizeof(*arg) + sizeof(*chr), arg =
	     (typeof(arg)) (chr + 1), chr = (typeof(chr)) (arg + 1)) {
		arg->type = SCCP_OBJ_TYPE_SR;
		arg->id = sr->id;
		chr->spid = sr->sp.sp ? sr->sp.sp->id : 0;
		chr->proto = sr->proto;
		chr->add = sr->add;
	}
	/* 
	   write out list of local subsystems */
	chs = (typeof(chs)) chr;
	for (ss = sp->ss.list; ss && size >= sizeof(*arg) + sizeof(*chs) + sizeof(*arg);
	     ss = ss->sp.next, size -= sizeof(*arg) + sizeof(*chs), arg =
	     (typeof(arg)) (chs + 1), chs = (typeof(chs)) (arg + 1)) {
		arg->type = SCCP_OBJ_TYPE_SS;
		arg->id = ss->id;
		chs->spid = ss->sp.sp ? ss->sp.sp->id : 0;
		chs->ssn = ss->ssn;
	}
	/* 
	   terminate list with zero object type */
	arg->type = 0;
	arg->id = 0;
	return (QR_DONE);
}
STATIC int
sccp_get_mt(struct sccp_config *arg, struct mt *mt, int size)
{
	struct sr *sr;
	struct sp *sp;
	struct sccp_conf_sr *chr;
	struct sccp_conf_sp *chp;
	struct sccp_conf_mt *cnf = (typeof(cnf)) (arg + 1);
	if (!mt || (size -= sizeof(*cnf)) < sizeof(*arg))
		return (-EINVAL);
	cnf->proto = mt->proto;
	/* 
	   write out remote signalling point or local signalling point configuration following
	   message transfer part configuration */
	arg = (typeof(arg)) (cnf + 1);
	chr = (typeof(chr)) (arg + 1);
	/* 
	   remote signalling point */
	for (sr = mt->sr; sr && size >= sizeof(*arg) + sizeof(*chr) + sizeof(*arg);
	     sr = NULL, size -= sizeof(*arg) + sizeof(*chr), arg = (typeof(arg)) (chr + 1), chr =
	     (typeof(chr)) (arg + 1)) {
		arg->type = SCCP_OBJ_TYPE_SR;
		arg->id = sr->id;
		chr->spid = sr->sp.sp ? sr->sp.sp->id : 0;
		chr->proto = sr->proto;
		chr->add = sr->add;
	}
	chp = (typeof(chp)) chr;
	/* 
	   local signalling point */
	for (sp = mt->sp; sp && size >= sizeof(*arg) + sizeof(*chp) + sizeof(*arg);
	     sp = NULL, size -= sizeof(*arg) + sizeof(*chp), arg = (typeof(arg)) (chp + 1), chp =
	     (typeof(chp)) (arg + 1)) {
		arg->type = SCCP_OBJ_TYPE_SP;
		arg->id = sp->id;
		chp->naid = sp->na.na ? sp->na.na->id : 0;
		chp->proto = sp->proto;
		chp->add = sp->add;
	}
	/* 
	   terminate list with zero object type */
	arg->type = 0;
	arg->id = 0;
	return (QR_DONE);
}
STATIC int
sccp_get_df(struct sccp_config *arg, struct df *df, int size)
{
	struct na *na;
	struct sccp_conf_na *cha;
	struct sccp_conf_df *cnf = (typeof(cnf)) (arg + 1);
	if (!df || (size -= sizeof(*cnf)) < sizeof(*arg))
		return (-EINVAL);
	cnf->proto = df->proto;
	/* 
	   write out list of network appearances following default configuration */
	arg = (typeof(arg)) (cnf + 1);
	cha = (typeof(cha)) (arg + 1);
	for (na = df->na.list; na && size >= sizeof(*arg) + sizeof(*cha) + sizeof(*arg);
	     na = na->next, size -= sizeof(*arg) + sizeof(*cha), arg =
	     (typeof(arg)) (cha + 1), cha = (typeof(cha)) (arg + 1)) {
		arg->type = SCCP_OBJ_TYPE_NA;
		arg->id = na->id;
		cha->proto = na->proto;
	}
	/* 
	   terminate list with zero object type */
	arg->type = 0;
	arg->id = 0;
	return (QR_DONE);
}
STATIC int
sccp_get_conf(struct sccp_config *arg, int size)
{
	switch (arg->type) {
	case SCCP_OBJ_TYPE_SC:
		return sccp_get_sc(arg, sccp_lookup(arg->id), size);
	case SCCP_OBJ_TYPE_NA:
		return sccp_get_na(arg, na_lookup(arg->id), size);
	case SCCP_OBJ_TYPE_CP:
		return sccp_get_cp(arg, cp_lookup(arg->id), size);
	case SCCP_OBJ_TYPE_SS:
		return sccp_get_ss(arg, ss_lookup(arg->id), size);
	case SCCP_OBJ_TYPE_RS:
		return sccp_get_rs(arg, rs_lookup(arg->id), size);
	case SCCP_OBJ_TYPE_SR:
		return sccp_get_sr(arg, sr_lookup(arg->id), size);
	case SCCP_OBJ_TYPE_SP:
		return sccp_get_sp(arg, sp_lookup(arg->id), size);
	case SCCP_OBJ_TYPE_MT:
		return sccp_get_mt(arg, mtp_lookup(arg->id), size);
	case SCCP_OBJ_TYPE_DF:
		return sccp_get_df(arg, &master, size);
	default:
		rare();
		return -EINVAL;
	}
}

/*
 *  ADD - Add Object Configuration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
sccp_add_sc(struct sccp_config *arg, struct sc *sc, int size, int force, int test)
{
	struct sccp_conf_sc *cnf = (typeof(cnf)) (arg + 1);
	if (sc || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	return (-EINVAL);
}
STATIC int
sccp_add_na(struct sccp_config *arg, struct na *na, int size, int force, int test)
{
	struct sccp_conf_na *cnf = (typeof(cnf)) (arg + 1);
	if (na || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (!force) {
	}
	if (!test) {
		if (!(na = sccp_alloc_na(na_get_id(arg->id), &cnf->proto)))
			return (-ENOMEM);
		arg->id = na->id;
	}
	return (QR_DONE);
}
STATIC int
sccp_add_cp(struct sccp_config *arg, struct cp *cp, int size, int force, int test)
{
	struct sccp_conf_cp *cnf = (typeof(cnf)) (arg + 1);
	if (cp || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	return (-EINVAL);
}
STATIC int
sccp_add_ss(struct sccp_config *arg, struct ss *ss, int size, int force, int test)
{
	struct sp *sp = NULL;
	struct sccp_conf_ss *cnf = (typeof(cnf)) (arg + 1);
	if (ss || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (cnf->spid)
		sp = sp_lookup(cnf->spid);
	if (!sp)
		return (-EINVAL);
	/* 
	   local subsystem number must be unique for local signalling point */
	for (ss = sp->ss.list; ss; ss = ss->sp.next)
		if (ss->ssn == cnf->ssn)
			return (-EBUSY);
	if (!force) {
	}
	if (!test) {
		if (!(ss = sccp_alloc_ss(ss_get_id(arg->id), sp, cnf->ssn)))
			return (-ENOMEM);
		arg->id = ss->id;
	}
	return (QR_DONE);
}
STATIC int
sccp_add_rs(struct sccp_config *arg, struct rs *rs, int size, int force, int test)
{
	struct sr *sr = NULL;
	struct sccp_conf_rs *cnf = (typeof(cnf)) (arg + 1);
	if (sr || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (cnf->srid)
		sr = sr_lookup(cnf->srid);
	if (!sr)
		return (-EINVAL);
	/* 
	   remote subsystem number must be unique for remote signalling point */
	for (rs = sr->rs.list; rs; rs = rs->sr.next)
		if (rs->ssn == cnf->ssn)
			return (-EBUSY);
	if (!force) {
	}
	if (!test) {
		if (!(rs = sccp_alloc_rs(rs_get_id(arg->id), sr, cnf->ssn)))
			return (-ENOMEM);
		arg->id = rs->id;
	}
	return (QR_DONE);
}
STATIC int
sccp_add_sr(struct sccp_config *arg, struct sr *sr, int size, int force, int test)
{
	struct sp *sp = NULL;
	struct sccp_conf_sr *cnf = (typeof(cnf)) (arg + 1);
	if (sr || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (cnf->spid)
		sp = sp_lookup(cnf->spid);
	if (!sp)
		return (-EINVAL);
	/* 
	   network appearance of remote must match network appearance of local */
	if (cnf->add.ni)
		if (sp->add.ni != cnf->add.ni)
			return (-EINVAL);
	/* 
	   service indicator of remote must match service indicator of local */
	if (cnf->add.si)
		if (sp->add.si != cnf->add.si)
			return (-EINVAL);
	/* 
	   remote address must not be local address */
	if (sp->add.pc == cnf->add.pc)
		return (-EINVAL);
	/* 
	   remote address must be unique for local signalling point */
	for (sr = sp->sr.list; sr; sr = sr->sp.next)
		if (sr->add.pc == cnf->add.pc)
			return (-EBUSY);
	if (!force) {
	}
	if (!test) {
		if (!(sr = sccp_alloc_sr(sr_get_id(arg->id), sp, cnf->add.pc)))
			return (-ENOMEM);
		sr->proto = cnf->proto;
		arg->id = sr->id;
	}
	return (QR_DONE);
}
STATIC int
sccp_add_sp(struct sccp_config *arg, struct sp *sp, int size, int force, int test)
{
	struct na *na = NULL;
	struct sccp_conf_sp *cnf = (typeof(cnf)) (arg + 1);
	if (sp || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (cnf->naid)
		na = na_lookup(cnf->naid);
	if (!na)
		return (-EINVAL);
	if (cnf->add.ni) {
		if (na->id != cnf->add.ni)
			return (-EINVAL);
	} else
		cnf->add.ni = na->id;
	/* 
	   local address must be unique for network appearance */
	for (sp = na->sp.list; sp; sp = sp->na.next)
		if (sp->add.ni == cnf->add.ni && sp->add.pc == cnf->add.pc
		    && sp->add.si == cnf->add.si)
			return (-EBUSY);
	if (!force) {
	}
	if (!test) {
		if (!(sp = sccp_alloc_sp(sp_get_id(arg->id), na, &cnf->add)))
			return (-ENOMEM);
		sp->proto = cnf->proto;
		arg->id = sp->id;
	}
	return (QR_DONE);
}
STATIC int
sccp_add_mt(struct sccp_config *arg, struct mt *mt, int size, int force, int test)
{
	struct sp *sp = NULL;
	struct sr *sr = NULL;
	struct sccp_conf_mt *cnf = (typeof(cnf)) (arg + 1);
	if (mt || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (cnf->spid)
		sp = sp_lookup(cnf->spid);
	if (cnf->srid)
		sr = sr_lookup(cnf->srid);
	if (!sp || (!sr && cnf->srid))
		return (-EINVAL);
	if (sp->mt || (sr && sr->mt))
		return (-EINVAL);
	for (mt = master.mt.list; mt; mt = mt->next)
		if (mt->u.mux.index == cnf->muxid) {
			if (mt->id)
				return (-EINVAL);
			break;
		}
	if (!mt)
		return (-EINVAL);
	if (!force) {
	}
	if (!test) {
		mt->id = mtp_get_id(arg->id);
		if (sr) {
			mt->sr = sr_get(sr);
			sr->mt = mtp_get(mt);
		} else {
			mt->sp = sp_get(sp);
			sp->mt = mtp_get(mt);
		}
		mt->proto = cnf->proto;
		arg->id = mt->id;
	}
	return (QR_DONE);
}
STATIC int
sccp_add_df(struct sccp_config *arg, struct df *df, int size, int force, int test)
{
	struct sccp_conf_df *cnf = (typeof(cnf)) (arg + 1);
	if (df || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	return (-EINVAL);
}
STATIC int
sccp_add_conf(struct sccp_config *arg, int size, const int force, const int test)
{
	switch (arg->type) {
	case SCCP_OBJ_TYPE_SC:
		return sccp_add_sc(arg, sccp_lookup(arg->id), size, force, test);
	case SCCP_OBJ_TYPE_NA:
		return sccp_add_na(arg, na_lookup(arg->id), size, force, test);
	case SCCP_OBJ_TYPE_CP:
		return sccp_add_cp(arg, cp_lookup(arg->id), size, force, test);
	case SCCP_OBJ_TYPE_SS:
		return sccp_add_ss(arg, ss_lookup(arg->id), size, force, test);
	case SCCP_OBJ_TYPE_RS:
		return sccp_add_rs(arg, rs_lookup(arg->id), size, force, test);
	case SCCP_OBJ_TYPE_SR:
		return sccp_add_sr(arg, sr_lookup(arg->id), size, force, test);
	case SCCP_OBJ_TYPE_SP:
		return sccp_add_sp(arg, sp_lookup(arg->id), size, force, test);
	case SCCP_OBJ_TYPE_MT:
		return sccp_add_mt(arg, mtp_lookup(arg->id), size, force, test);
	case SCCP_OBJ_TYPE_DF:
		return sccp_add_df(arg, &master, size, force, test);
	default:
		rare();
		return -EINVAL;
	}
}

/*
 *  CHA - Change Object Configuration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
sccp_cha_sc(struct sccp_config *arg, struct sc *sc, int size, int force, int test)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}
STATIC int
sccp_cha_na(struct sccp_config *arg, struct na *na, int size, int force, int test)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}
STATIC int
sccp_cha_cp(struct sccp_config *arg, struct cp *cp, int size, int force, int test)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}
STATIC int
sccp_cha_ss(struct sccp_config *arg, struct ss *ss, int size, int force, int test)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}
STATIC int
sccp_cha_rs(struct sccp_config *arg, struct rs *rs, int size, int force, int test)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}
STATIC int
sccp_cha_sr(struct sccp_config *arg, struct sr *sr, int size, int force, int test)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}
STATIC int
sccp_cha_sp(struct sccp_config *arg, struct sp *sp, int size, int force, int test)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}
STATIC int
sccp_cha_mt(struct sccp_config *arg, struct mt *mt, int size, int force, int test)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}
STATIC int
sccp_cha_df(struct sccp_config *arg, struct df *df, int size, int force, int test)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}
STATIC int
sccp_cha_conf(struct sccp_config *arg, int size, const int force, const int test)
{
	switch (arg->type) {
	case SCCP_OBJ_TYPE_SC:
		return sccp_cha_sc(arg, sccp_lookup(arg->id), size, force, test);
	case SCCP_OBJ_TYPE_NA:
		return sccp_cha_na(arg, na_lookup(arg->id), size, force, test);
	case SCCP_OBJ_TYPE_CP:
		return sccp_cha_cp(arg, cp_lookup(arg->id), size, force, test);
	case SCCP_OBJ_TYPE_SS:
		return sccp_cha_ss(arg, ss_lookup(arg->id), size, force, test);
	case SCCP_OBJ_TYPE_RS:
		return sccp_cha_rs(arg, rs_lookup(arg->id), size, force, test);
	case SCCP_OBJ_TYPE_SR:
		return sccp_cha_sr(arg, sr_lookup(arg->id), size, force, test);
	case SCCP_OBJ_TYPE_SP:
		return sccp_cha_sp(arg, sp_lookup(arg->id), size, force, test);
	case SCCP_OBJ_TYPE_MT:
		return sccp_cha_mt(arg, mtp_lookup(arg->id), size, force, test);
	case SCCP_OBJ_TYPE_DF:
		return sccp_cha_df(arg, &master, size, force, test);
	default:
		rare();
		return -EINVAL;
	}
}

/*
 *  DEL - Delete Object Configuration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
sccp_del_sc(struct sccp_config *arg, struct sc *sc, int size, int force, int test)
{
	if (!sc)
		return (-EINVAL);
	if (!force) {
		/* 
		   bound to subsystem */
		if (sc->ss.ss)
			return (-EBUSY);
	}
	if (!test) {
		// sccp_free_priv(sc);
	}
	return (QR_DONE);
}
STATIC int
sccp_del_na(struct sccp_config *arg, struct na *na, int size, int force, int test)
{
	if (!na)
		return (-EINVAL);
	if (!force) {
		/* 
		   attached signalling points */
		if (na->sp.list)
			return (-EBUSY);
	}
	if (!test) {
		sccp_free_na(na);
	}
	return (QR_DONE);
}
STATIC int
sccp_del_cp(struct sccp_config *arg, struct cp *cp, int size, int force, int test)
{
	if (!cp)
		return (-EINVAL);
	if (!force) {
	}
	if (!test) {
		sccp_free_cp(cp);
	}
	return (QR_DONE);
}
STATIC int
sccp_del_ss(struct sccp_config *arg, struct ss *ss, int size, int force, int test)
{
	if (!ss)
		return (-EINVAL);
	if (!force) {
		/* 
		   bound CL SCCP users */
		if (ss->cl.list)
			return (-EBUSY);
		/* 
		   bound CO SCCP users */
		if (ss->co.list)
			return (-EBUSY);
		/* 
		   listening SCCP users */
		if (ss->cr.list)
			return (-EBUSY);
	}
	if (!test) {
		sccp_free_ss(ss);
	}
	return (QR_DONE);
}
STATIC int
sccp_del_rs(struct sccp_config *arg, struct rs *rs, int size, int force, int test)
{
	if (!rs)
		return (-EINVAL);
	if (!force) {
	}
	if (!test) {
		sccp_free_rs(rs);
	}
	return (QR_DONE);
}
STATIC int
sccp_del_sr(struct sccp_config *arg, struct sr *sr, int size, int force, int test)
{
	if (!sr)
		return (-EINVAL);
	if (!force) {
		/* 
		   attached to a live MTP interface */
		if (sr->mt)
			return (-EBUSY);
		/* 
		   we have remote subsystems */
		if (sr->rs.list)
			return (-EBUSY);
		/* 
		   we have connected SCCP users */
		if (sr->sc.list)
			return (-EBUSY);
	}
	if (!test) {
		sccp_free_sr(sr);
	}
	return (QR_DONE);
}
STATIC int
sccp_del_sp(struct sccp_config *arg, struct sp *sp, int size, int force, int test)
{
	if (!sp)
		return (-EINVAL);
	if (!force) {
		/* 
		   bound to SCCP user */
		if (sp->sc.list)
			return (-EBUSY);
		/* 
		   attached to a live MTP interface */
		if (sp->mt)
			return (-EBUSY);
		/* 
		   we have remote signalling points associated */
		if (sp->sr.list)
			return (-EBUSY);
		/* 
		   we have local subsystems */
		if (sp->ss.list)
			return (-EBUSY);
		/* 
		   we have couplings */
		if (sp->cp.list)
			return (-EBUSY);
	}
	if (!test) {
		sccp_free_sp(sp);
	}
	return (QR_DONE);
}
STATIC int
sccp_del_mt(struct sccp_config *arg, struct mt *mt, int size, int force, int test)
{
	if (!mt)
		return (-EINVAL);
	if (!force) {
		/* 
		   bound to internal datastructures */
		if (mt->sp || mt->sr)
			return (-EBUSY);
	}
	if (!test) {
		// sccp_free_mtp(mt);
	}
	return (QR_DONE);
}
STATIC int
sccp_del_df(struct sccp_config *arg, struct df *df, int size, int force, int test)
{
	struct sccp_conf_df *cnf = (typeof(cnf)) (arg + 1);
	if (!df || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (!force) {
	}
	if (!test) {
		// sccp_free_df(df);
	}
	return (QR_DONE);
}
STATIC int
sccp_del_conf(struct sccp_config *arg, int size, const int force, const int test)
{
	switch (arg->type) {
	case SCCP_OBJ_TYPE_SC:
		return sccp_del_sc(arg, sccp_lookup(arg->id), size, force, test);
	case SCCP_OBJ_TYPE_NA:
		return sccp_del_na(arg, na_lookup(arg->id), size, force, test);
	case SCCP_OBJ_TYPE_CP:
		return sccp_del_cp(arg, cp_lookup(arg->id), size, force, test);
	case SCCP_OBJ_TYPE_SS:
		return sccp_del_ss(arg, ss_lookup(arg->id), size, force, test);
	case SCCP_OBJ_TYPE_RS:
		return sccp_del_rs(arg, rs_lookup(arg->id), size, force, test);
	case SCCP_OBJ_TYPE_SR:
		return sccp_del_sr(arg, sr_lookup(arg->id), size, force, test);
	case SCCP_OBJ_TYPE_SP:
		return sccp_del_sp(arg, sp_lookup(arg->id), size, force, test);
	case SCCP_OBJ_TYPE_MT:
		return sccp_del_mt(arg, mtp_lookup(arg->id), size, force, test);
	case SCCP_OBJ_TYPE_DF:
		return sccp_del_df(arg, &master, size, force, test);
	default:
		rare();
		return -EINVAL;
	}
}

/*
 *  ALL Management
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
sccp_mgmt_all_sc(queue_t *q, struct sc *sc)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
sccp_mgmt_all_na(queue_t *q, struct na *na)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
sccp_mgmt_all_cp(queue_t *q, struct cp *cp)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
sccp_mgmt_all_ss(queue_t *q, struct ss *ss)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
sccp_mgmt_all_rs(queue_t *q, struct rs *rs)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
sccp_mgmt_all_sr(queue_t *q, struct sr *sr)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
sccp_mgmt_all_sp(queue_t *q, struct sp *sp)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
sccp_mgmt_all_mt(queue_t *q, struct mt *mt)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
sccp_mgmt_all_df(queue_t *q, struct df *df)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}

/*
 *  PRO Management
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
sccp_mgmt_pro_sc(queue_t *q, struct sc *sc)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
sccp_mgmt_pro_na(queue_t *q, struct na *na)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
sccp_mgmt_pro_cp(queue_t *q, struct cp *cp)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
sccp_mgmt_pro_ss(queue_t *q, struct ss *ss)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
sccp_mgmt_pro_rs(queue_t *q, struct rs *rs)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
sccp_mgmt_pro_sr(queue_t *q, struct sr *sr)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
sccp_mgmt_pro_sp(queue_t *q, struct sp *sp)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
sccp_mgmt_pro_mt(queue_t *q, struct mt *mt)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
sccp_mgmt_pro_df(queue_t *q, struct df *df)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}

/*
 *  BLO Management
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
sccp_mgmt_blo_sc(queue_t *q, struct sc *sc)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
sccp_mgmt_blo_na(queue_t *q, struct na *na)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
sccp_mgmt_blo_cp(queue_t *q, struct cp *cp)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
sccp_mgmt_blo_ss(queue_t *q, struct ss *ss)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
sccp_mgmt_blo_rs(queue_t *q, struct rs *rs)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
sccp_mgmt_blo_sr(queue_t *q, struct sr *sr)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
sccp_mgmt_blo_sp(queue_t *q, struct sp *sp)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
sccp_mgmt_blo_mt(queue_t *q, struct mt *mt)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
sccp_mgmt_blo_df(queue_t *q, struct df *df)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}

/*
 *  UBL Management
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
sccp_mgmt_ubl_sc(queue_t *q, struct sc *sc)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
sccp_mgmt_ubl_na(queue_t *q, struct na *na)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
sccp_mgmt_ubl_cp(queue_t *q, struct cp *cp)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
sccp_mgmt_ubl_ss(queue_t *q, struct ss *ss)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
sccp_mgmt_ubl_rs(queue_t *q, struct rs *rs)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
sccp_mgmt_ubl_sr(queue_t *q, struct sr *sr)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
sccp_mgmt_ubl_sp(queue_t *q, struct sp *sp)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
sccp_mgmt_ubl_mt(queue_t *q, struct mt *mt)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
sccp_mgmt_ubl_df(queue_t *q, struct df *df)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}

/*
 *  RES Management
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
sccp_mgmt_res_sc(queue_t *q, struct sc *sc)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
sccp_mgmt_res_na(queue_t *q, struct na *na)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
sccp_mgmt_res_cp(queue_t *q, struct cp *cp)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
sccp_mgmt_res_ss(queue_t *q, struct ss *ss)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
sccp_mgmt_res_rs(queue_t *q, struct rs *rs)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
sccp_mgmt_res_sr(queue_t *q, struct sr *sr)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
sccp_mgmt_res_sp(queue_t *q, struct sp *sp)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
sccp_mgmt_res_mt(queue_t *q, struct mt *mt)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
sccp_mgmt_res_df(queue_t *q, struct df *df)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}

/*
 *  SCCP_IOCGOPTION
 *  -----------------------------------
 */
STATIC int
sccp_iocgoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int size = msgdsize(mp);
		sccp_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) >= 0) {
			switch (arg->type) {
			case SCCP_OBJ_TYPE_SC:
			{
				struct sc *sc = sccp_lookup(arg->id);
				sccp_opt_conf_sc_t *opt = (typeof(opt)) (arg + 1);
				if (!sc || (size -= sizeof(*opt)) < 0)
					goto einval;
				*opt = sc->config;
				break;
			}
			case SCCP_OBJ_TYPE_NA:
			{
				struct na *na = na_lookup(arg->id);
				sccp_opt_conf_na_t *opt = (typeof(opt)) (arg + 1);
				if (!na || (size -= sizeof(*opt)) < 0)
					goto einval;
				*opt = na->config;
				break;
			}
			case SCCP_OBJ_TYPE_CP:
			{
				struct cp *cp = cp_lookup(arg->id);
				sccp_opt_conf_cp_t *opt = (typeof(opt)) (arg + 1);
				if (!cp || (size -= sizeof(*opt)) < 0)
					goto einval;
				*opt = cp->config;
				break;
			}
			case SCCP_OBJ_TYPE_SS:
			{
				struct ss *ss = ss_lookup(arg->id);
				sccp_opt_conf_ss_t *opt = (typeof(opt)) (arg + 1);
				if (!ss || (size -= sizeof(*opt)) < 0)
					goto einval;
				*opt = ss->config;
				break;
			}
			case SCCP_OBJ_TYPE_RS:
			{
				struct rs *rs = rs_lookup(arg->id);
				sccp_opt_conf_rs_t *opt = (typeof(opt)) (arg + 1);
				if (!rs || (size -= sizeof(*opt)) < 0)
					goto einval;
				*opt = rs->config;
				break;
			}
			case SCCP_OBJ_TYPE_SR:
			{
				struct sr *sr = sr_lookup(arg->id);
				sccp_opt_conf_sr_t *opt = (typeof(opt)) (arg + 1);
				if (!sr || (size -= sizeof(*opt)) < 0)
					goto einval;
				*opt = sr->config;
				break;
			}
			case SCCP_OBJ_TYPE_SP:
			{
				struct sp *sp = sp_lookup(arg->id);
				sccp_opt_conf_sp_t *opt = (typeof(opt)) (arg + 1);
				if (!sp || (size -= sizeof(*opt)) < 0)
					goto einval;
				*opt = sp->config;
				break;
			}
			case SCCP_OBJ_TYPE_MT:
			{
				struct mt *mt = mtp_lookup(arg->id);
				sccp_opt_conf_mt_t *opt = (typeof(opt)) (arg + 1);
				if (!mt || (size -= sizeof(*opt)) < 0)
					goto einval;
				*opt = mt->config;
				break;
			}
			case SCCP_OBJ_TYPE_DF:
			{
				struct df *df = &master;
				sccp_opt_conf_df_t *opt = (typeof(opt)) (arg + 1);
				if (!df || (size -= sizeof(*opt)) < 0)
					goto einval;
				*opt = df->config;
				break;
			}
			default:
				rare();
			      einval:
				return (-EINVAL);
			}
			return (QR_DONE);
		}
	}
	rare();
	return (-EINVAL);
}

/*
 *  SCCP_IOCSOPTION
 *  -----------------------------------
 */
STATIC int
sccp_iocsoption(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int size = msgdsize(mp);
		sccp_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) >= 0) {
			switch (arg->type) {
			case SCCP_OBJ_TYPE_SC:
			{
				struct sc *sc = sccp_lookup(arg->id);
				sccp_opt_conf_sc_t *opt = (typeof(opt)) (arg + 1);
				if (!sc || (size -= sizeof(*opt)) < 0)
					goto einval;
				sc->config = *opt;
				break;
			}
			case SCCP_OBJ_TYPE_NA:
			{
				struct na *na = na_lookup(arg->id);
				sccp_opt_conf_na_t *opt = (typeof(opt)) (arg + 1);
				if (!na || (size -= sizeof(*opt)) < 0)
					goto einval;
				na->config = *opt;
				break;
			}
			case SCCP_OBJ_TYPE_CP:
			{
				struct cp *cp = cp_lookup(arg->id);
				sccp_opt_conf_cp_t *opt = (typeof(opt)) (arg + 1);
				if (!cp || (size -= sizeof(*opt)) < 0)
					goto einval;
				cp->config = *opt;
				break;
			}
			case SCCP_OBJ_TYPE_SS:
			{
				struct ss *ss = ss_lookup(arg->id);
				sccp_opt_conf_ss_t *opt = (typeof(opt)) (arg + 1);
				if (!ss || (size -= sizeof(*opt)) < 0)
					goto einval;
				ss->config = *opt;
				break;
			}
			case SCCP_OBJ_TYPE_RS:
			{
				struct rs *rs = rs_lookup(arg->id);
				sccp_opt_conf_rs_t *opt = (typeof(opt)) (arg + 1);
				if (!rs || (size -= sizeof(*opt)) < 0)
					goto einval;
				rs->config = *opt;
				break;
			}
			case SCCP_OBJ_TYPE_SR:
			{
				struct sr *sr = sr_lookup(arg->id);
				sccp_opt_conf_sr_t *opt = (typeof(opt)) (arg + 1);
				if (!sr || (size -= sizeof(*opt)) < 0)
					goto einval;
				sr->config = *opt;
				break;
			}
			case SCCP_OBJ_TYPE_SP:
			{
				struct sp *sp = sp_lookup(arg->id);
				sccp_opt_conf_sp_t *opt = (typeof(opt)) (arg + 1);
				if (!sp || (size -= sizeof(*opt)) < 0)
					goto einval;
				sp->config = *opt;
				break;
			}
			case SCCP_OBJ_TYPE_MT:
			{
				struct mt *mt = mtp_lookup(arg->id);
				sccp_opt_conf_mt_t *opt = (typeof(opt)) (arg + 1);
				if (!mt || (size -= sizeof(*opt)) < 0)
					goto einval;
				mt->config = *opt;
				break;
			}
			case SCCP_OBJ_TYPE_DF:
			{
				struct df *df = &master;
				sccp_opt_conf_df_t *opt = (typeof(opt)) (arg + 1);
				if (!df || (size -= sizeof(*opt)) < 0)
					goto einval;
				df->config = *opt;
				break;
			}
			default:
				rare();
			      einval:
				return (-EINVAL);
			}
			return (QR_DONE);
		}
	}
	rare();
	return (-EINVAL);
}

/*
 *  SCCP_IOCGCONFIG
 *  -----------------------------------
 */
STATIC int
sccp_iocgconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int size = msgdsize(mp);
		struct sccp_config *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) >= 0)
			return sccp_get_conf(arg, size);
	}
	rare();
	return (-EINVAL);
}

/*
 *  SCCP_IOCSCONFIG
 *  -----------------------------------
 */
STATIC int
sccp_iocsconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int size = msgdsize(mp);
		struct sccp_config *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) >= 0)
			switch (arg->cmd) {
			case SCCP_ADD:
				return sccp_add_conf(arg, size, 0, 0);
			case SCCP_CHA:
				return sccp_cha_conf(arg, size, 0, 0);
			case SCCP_DEL:
				return sccp_del_conf(arg, size, 0, 0);
			default:
				rare();
				return (-EINVAL);
			}
	}
	rare();
	return (-EINVAL);
}

/*
 *  SCCP_IOCTCONFIG
 *  -----------------------------------
 */
STATIC int
sccp_ioctconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int size = msgdsize(mp);
		struct sccp_config *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) >= 0)
			switch (arg->cmd) {
			case SCCP_ADD:
				return sccp_add_conf(arg, size, 0, 1);
			case SCCP_CHA:
				return sccp_cha_conf(arg, size, 0, 1);
			case SCCP_DEL:
				return sccp_del_conf(arg, size, 0, 1);
			default:
				rare();
				return (-EINVAL);
			}
	}
	rare();
	return (-EINVAL);
}

/*
 *  SCCP_IOCCCONFIG
 *  -----------------------------------
 */
STATIC int
sccp_ioccconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int size = msgdsize(mp);
		struct sccp_config *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) >= 0)
			switch (arg->cmd) {
			case SCCP_ADD:
				return sccp_add_conf(arg, size, 1, 0);
			case SCCP_CHA:
				return sccp_cha_conf(arg, size, 1, 0);
			case SCCP_DEL:
				return sccp_del_conf(arg, size, 1, 0);
			default:
				rare();
				return (-EINVAL);
			}
	}
	rare();
	return (-EINVAL);
}

/*
 *  SCCP_IOCGSTATEM
 *  -----------------------------------
 */
STATIC int
sccp_iocgstatem(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		psw_t flags;
		int ret = QR_DONE;
		int size = msgdsize(mp);
		struct sccp_statem *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) < 0)
			return -EMSGSIZE;
		spin_lock_irqsave(&master.lock, flags);
		switch (arg->type) {
		case SCCP_OBJ_TYPE_SC:
		{
			struct sc *sc = sccp_lookup(arg->id);
			sccp_statem_sc_t *sta = (typeof(sta)) (arg + 1);
			if (!sc || (size -= sizeof(*sta)) < 0)
				goto einval;
			arg->flags = sc->flags;
			arg->state = sc->state;
			sta->timers = sc->timers;
			break;
		}
		case SCCP_OBJ_TYPE_NA:
		{
			struct na *na = na_lookup(arg->id);
			sccp_statem_na_t *sta = (typeof(sta)) (arg + 1);
			if (!na || (size -= sizeof(*sta)) < 0)
				goto einval;
			arg->flags = na->flags;
			arg->state = na->state;
			sta->timers = na->timers;
			break;
		}
		case SCCP_OBJ_TYPE_CP:
		{
			struct cp *cp = cp_lookup(arg->id);
			sccp_statem_cp_t *sta = (typeof(sta)) (arg + 1);
			if (!cp || (size -= sizeof(*sta)) < 0)
				goto einval;
			arg->flags = cp->flags;
			arg->state = cp->state;
			sta->timers = cp->timers;
			break;
		}
		case SCCP_OBJ_TYPE_SS:
		{
			struct ss *ss = ss_lookup(arg->id);
			sccp_statem_ss_t *sta = (typeof(sta)) (arg + 1);
			if (!ss || (size -= sizeof(*sta)) < 0)
				goto einval;
			arg->flags = ss->flags;
			arg->state = ss->state;
			sta->timers = ss->timers;
			break;
		}
		case SCCP_OBJ_TYPE_RS:
		{
			struct rs *rs = rs_lookup(arg->id);
			sccp_statem_rs_t *sta = (typeof(sta)) (arg + 1);
			if (!rs || (size -= sizeof(*sta)) < 0)
				goto einval;
			arg->flags = rs->flags;
			arg->state = rs->state;
			sta->timers = rs->timers;
			break;
		}
		case SCCP_OBJ_TYPE_SR:
		{
			struct sr *sr = sr_lookup(arg->id);
			sccp_statem_sr_t *sta = (typeof(sta)) (arg + 1);
			if (!sr || (size -= sizeof(*sta)) < 0)
				goto einval;
			arg->flags = sr->flags;
			arg->state = sr->state;
			sta->timers = sr->timers;
			break;
		}
		case SCCP_OBJ_TYPE_SP:
		{
			struct sp *sp = sp_lookup(arg->id);
			sccp_statem_sp_t *sta = (typeof(sta)) (arg + 1);
			if (!sp || (size -= sizeof(*sta)) < 0)
				goto einval;
			arg->flags = sp->flags;
			arg->state = sp->state;
			sta->timers = sp->timers;
			break;
		}
		case SCCP_OBJ_TYPE_MT:
		{
			struct mt *mt = mtp_lookup(arg->id);
			sccp_statem_mt_t *sta = (typeof(sta)) (arg + 1);
			if (!mt || (size -= sizeof(*sta)) < 0)
				goto einval;
			arg->flags = mt->flags;
			arg->state = mt->state;
			sta->timers = mt->timers;
			break;
		}
		case SCCP_OBJ_TYPE_DF:
		{
			struct df *df = &master;
			sccp_statem_df_t *sta = (typeof(sta)) (arg + 1);
			if (!df || (size -= sizeof(*sta)) < 0)
				goto einval;
			// arg->flags = df->flags;
			// arg->state = df->state;
			// sta->timers = df->timers;
			break;
		}
		default:
			rare();
		      einval:
			ret = -EINVAL;
			break;
		}
		spin_unlock_irqrestore(&master.lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}

/*
 *  SCCP_IOCCMRESET
 *  -----------------------------------
 */
STATIC int
sccp_ioccmreset(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int size = msgdsize(mp);
		struct sccp_statem *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) < 0)
			return -EMSGSIZE;
		(void) arg;
		return -EOPNOTSUPP;
	}
	rare();
	return -EINVAL;
}

/*
 *  SCCP_IOCGSTATSP
 *  -----------------------------------
 */
STATIC int
sccp_iocgstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		psw_t flags;
		int ret = QR_DONE;
		int size = msgdsize(mp);
		struct sccp_stats *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) < 0)
			return -EMSGSIZE;
		spin_lock_irqsave(&master.lock, flags);
		switch (arg->type) {
		case SCCP_OBJ_TYPE_SC:
		{
			struct sc *sc = sccp_lookup(arg->id);
			sccp_stats_sc_t *sta = (typeof(sta)) (arg + 1);
			if (!sc || (size -= sizeof(*sta)) < 0)
				goto einval;
			*sta = sc->statsp;
			break;
		}
		case SCCP_OBJ_TYPE_NA:
		{
			struct na *na = na_lookup(arg->id);
			sccp_stats_na_t *sta = (typeof(sta)) (arg + 1);
			if (!na || (size -= sizeof(*sta)) < 0)
				goto einval;
			*sta = na->statsp;
			break;
		}
		case SCCP_OBJ_TYPE_CP:
		{
			struct cp *cp = cp_lookup(arg->id);
			sccp_stats_cp_t *sta = (typeof(sta)) (arg + 1);
			if (!cp || (size -= sizeof(*sta)) < 0)
				goto einval;
			*sta = cp->statsp;
			break;
		}
		case SCCP_OBJ_TYPE_SS:
		{
			struct ss *ss = ss_lookup(arg->id);
			sccp_stats_ss_t *sta = (typeof(sta)) (arg + 1);
			if (!ss || (size -= sizeof(*sta)) < 0)
				goto einval;
			*sta = ss->statsp;
			break;
		}
		case SCCP_OBJ_TYPE_RS:
		{
			struct rs *rs = rs_lookup(arg->id);
			sccp_stats_rs_t *sta = (typeof(sta)) (arg + 1);
			if (!rs || (size -= sizeof(*sta)) < 0)
				goto einval;
			*sta = rs->statsp;
			break;
		}
		case SCCP_OBJ_TYPE_SR:
		{
			struct sr *sr = sr_lookup(arg->id);
			sccp_stats_sr_t *sta = (typeof(sta)) (arg + 1);
			if (!sr || (size -= sizeof(*sta)) < 0)
				goto einval;
			*sta = sr->statsp;
			break;
		}
		case SCCP_OBJ_TYPE_SP:
		{
			struct sp *sp = sp_lookup(arg->id);
			sccp_stats_sp_t *sta = (typeof(sta)) (arg + 1);
			if (!sp || (size -= sizeof(*sta)) < 0)
				goto einval;
			*sta = sp->statsp;
			break;
		}
		case SCCP_OBJ_TYPE_MT:
		{
			struct mt *mt = mtp_lookup(arg->id);
			sccp_stats_mt_t *sta = (typeof(sta)) (arg + 1);
			if (!mt || (size -= sizeof(*sta)) < 0)
				goto einval;
			*sta = mt->statsp;
			break;
		}
		case SCCP_OBJ_TYPE_DF:
		{
			struct df *df = &master;
			sccp_stats_df_t *sta = (typeof(sta)) (arg + 1);
			if (!df || (size -= sizeof(*sta)) < 0)
				goto einval;
			*sta = df->statsp;
			break;
		}
		default:
			rare();
		      einval:
			ret = -EINVAL;
			break;
		}
		spin_unlock_irqrestore(&master.lock, flags);
		return (ret);
	}
	rare();
	return -EINVAL;
}

/*
 *  SCCP_IOCSSTATSP
 *  -----------------------------------
 */
STATIC int
sccp_iocsstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		psw_t flags;
		int ret = QR_DONE;
		int size = msgdsize(mp);
		struct sccp_stats *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) < 0)
			return -EMSGSIZE;
		spin_lock_irqsave(&master.lock, flags);
		switch (arg->type) {
		case SCCP_OBJ_TYPE_SC:
		{
			struct sc *sc = sccp_lookup(arg->id);
			sccp_stats_sc_t *sta = (typeof(sta)) (arg + 1);
			if (!sc || (size -= sizeof(*sta)) < 0)
				goto einval;
			sc->statsp = *sta;
			break;
		}
		case SCCP_OBJ_TYPE_NA:
		{
			struct na *na = na_lookup(arg->id);
			sccp_stats_na_t *sta = (typeof(sta)) (arg + 1);
			if (!na || (size -= sizeof(*sta)) < 0)
				goto einval;
			na->statsp = *sta;
			break;
		}
		case SCCP_OBJ_TYPE_CP:
		{
			struct cp *cp = cp_lookup(arg->id);
			sccp_stats_cp_t *sta = (typeof(sta)) (arg + 1);
			if (!cp || (size -= sizeof(*sta)) < 0)
				goto einval;
			cp->statsp = *sta;
			break;
		}
		case SCCP_OBJ_TYPE_SS:
		{
			struct ss *ss = ss_lookup(arg->id);
			sccp_stats_ss_t *sta = (typeof(sta)) (arg + 1);
			if (!ss || (size -= sizeof(*sta)) < 0)
				goto einval;
			ss->statsp = *sta;
			break;
		}
		case SCCP_OBJ_TYPE_RS:
		{
			struct rs *rs = rs_lookup(arg->id);
			sccp_stats_rs_t *sta = (typeof(sta)) (arg + 1);
			if (!rs || (size -= sizeof(*sta)) < 0)
				goto einval;
			rs->statsp = *sta;
			break;
		}
		case SCCP_OBJ_TYPE_SR:
		{
			struct sr *sr = sr_lookup(arg->id);
			sccp_stats_sr_t *sta = (typeof(sta)) (arg + 1);
			if (!sr || (size -= sizeof(*sta)) < 0)
				goto einval;
			sr->statsp = *sta;
			break;
		}
		case SCCP_OBJ_TYPE_SP:
		{
			struct sp *sp = sp_lookup(arg->id);
			sccp_stats_sp_t *sta = (typeof(sta)) (arg + 1);
			if (!sp || (size -= sizeof(*sta)) < 0)
				goto einval;
			sp->statsp = *sta;
			break;
		}
		case SCCP_OBJ_TYPE_MT:
		{
			struct mt *mt = mtp_lookup(arg->id);
			sccp_stats_mt_t *sta = (typeof(sta)) (arg + 1);
			if (!mt || (size -= sizeof(*sta)) < 0)
				goto einval;
			mt->statsp = *sta;
			break;
		}
		case SCCP_OBJ_TYPE_DF:
		{
			struct df *df = &master;
			sccp_stats_df_t *sta = (typeof(sta)) (arg + 1);
			if (!df || (size -= sizeof(*sta)) < 0)
				goto einval;
			df->statsp = *sta;
			break;
		}
		default:
			rare();
		      einval:
			ret = -EINVAL;
			break;
		}
		spin_unlock_irqrestore(&master.lock, flags);
		return (ret);
	}
	rare();
	return -EINVAL;
}

/*
 *  SCCP_IOCGSTATS
 *  -----------------------------------
 */
STATIC int
sccp_iocgstats(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		psw_t flags;
		int ret = QR_DONE;
		int size = msgdsize(mp);
		struct sccp_stats *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) < 0)
			return -EMSGSIZE;
		spin_lock_irqsave(&master.lock, flags);
		switch (arg->type) {
		case SCCP_OBJ_TYPE_SC:
		{
			struct sc *sc = sccp_lookup(arg->id);
			sccp_stats_sc_t *sta = (typeof(sta)) (arg + 1);
			if (!sc || (size -= sizeof(*sta)) < 0)
				goto einval;
			*sta = sc->stats;
			arg->header = jiffies;
			break;
		}
		case SCCP_OBJ_TYPE_NA:
		{
			struct na *na = na_lookup(arg->id);
			sccp_stats_na_t *sta = (typeof(sta)) (arg + 1);
			if (!na || (size -= sizeof(*sta)) < 0)
				goto einval;
			*sta = na->stats;
			arg->header = jiffies;
			break;
		}
		case SCCP_OBJ_TYPE_CP:
		{
			struct cp *cp = cp_lookup(arg->id);
			sccp_stats_cp_t *sta = (typeof(sta)) (arg + 1);
			if (!cp || (size -= sizeof(*sta)) < 0)
				goto einval;
			*sta = cp->stats;
			arg->header = jiffies;
			break;
		}
		case SCCP_OBJ_TYPE_SS:
		{
			struct ss *ss = ss_lookup(arg->id);
			sccp_stats_ss_t *sta = (typeof(sta)) (arg + 1);
			if (!ss || (size -= sizeof(*sta)) < 0)
				goto einval;
			*sta = ss->stats;
			arg->header = jiffies;
			break;
		}
		case SCCP_OBJ_TYPE_RS:
		{
			struct rs *rs = rs_lookup(arg->id);
			sccp_stats_rs_t *sta = (typeof(sta)) (arg + 1);
			if (!rs || (size -= sizeof(*sta)) < 0)
				goto einval;
			*sta = rs->stats;
			arg->header = jiffies;
			break;
		}
		case SCCP_OBJ_TYPE_SR:
		{
			struct sr *sr = sr_lookup(arg->id);
			sccp_stats_sr_t *sta = (typeof(sta)) (arg + 1);
			if (!sr || (size -= sizeof(*sta)) < 0)
				goto einval;
			*sta = sr->stats;
			arg->header = jiffies;
			break;
		}
		case SCCP_OBJ_TYPE_SP:
		{
			struct sp *sp = sp_lookup(arg->id);
			sccp_stats_sp_t *sta = (typeof(sta)) (arg + 1);
			if (!sp || (size -= sizeof(*sta)) < 0)
				goto einval;
			*sta = sp->stats;
			arg->header = jiffies;
			break;
		}
		case SCCP_OBJ_TYPE_MT:
		{
			struct mt *mt = mtp_lookup(arg->id);
			sccp_stats_mt_t *sta = (typeof(sta)) (arg + 1);
			if (!mt || (size -= sizeof(*sta)) < 0)
				goto einval;
			*sta = mt->stats;
			arg->header = jiffies;
			break;
		}
		case SCCP_OBJ_TYPE_DF:
		{
			struct df *df = &master;
			sccp_stats_df_t *sta = (typeof(sta)) (arg + 1);
			if (!df || (size -= sizeof(*sta)) < 0)
				goto einval;
			*sta = df->stats;
			arg->header = jiffies;
			break;
		}
		default:
			rare();
		      einval:
			ret = -EINVAL;
			break;
		}
		spin_unlock_irqrestore(&master.lock, flags);
		return (ret);
	}
	rare();
	return -EINVAL;
}

/*
 *  SCCP_IOCSSTATS
 *  -----------------------------------
 */
STATIC int
sccp_iocsstats(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		psw_t flags;
		int ret = QR_DONE;
		int size = msgdsize(mp);
		struct sccp_stats *arg = (typeof(arg)) mp->b_cont->b_rptr;
		uchar *d, *s = (typeof(s)) (arg + 1);
		if ((size -= sizeof(*arg)) < 0)
			return -EMSGSIZE;
		spin_lock_irqsave(&master.lock, flags);
		arg->header = jiffies;
		switch (arg->type) {
		case SCCP_OBJ_TYPE_SC:
		{
			struct sc *sc = sccp_lookup(arg->id);
			sccp_stats_sc_t *sta = (typeof(sta)) (arg + 1);
			if (!sc || (size -= sizeof(*sta)) < 0)
				goto einval;
			d = (typeof(d)) & sc->stats;
			for (size = sizeof(*sta); size--; *d &= *s) ;
			break;
		}
		case SCCP_OBJ_TYPE_NA:
		{
			struct na *na = na_lookup(arg->id);
			sccp_stats_na_t *sta = (typeof(sta)) (arg + 1);
			if (!na || (size -= sizeof(*sta)) < 0)
				goto einval;
			d = (typeof(d)) & na->stats;
			for (size = sizeof(*sta); size--; *d &= *s) ;
			break;
		}
		case SCCP_OBJ_TYPE_CP:
		{
			struct cp *cp = cp_lookup(arg->id);
			sccp_stats_cp_t *sta = (typeof(sta)) (arg + 1);
			if (!cp || (size -= sizeof(*sta)) < 0)
				goto einval;
			d = (typeof(d)) & cp->stats;
			for (size = sizeof(*sta); size--; *d &= *s) ;
			break;
		}
		case SCCP_OBJ_TYPE_SS:
		{
			struct ss *ss = ss_lookup(arg->id);
			sccp_stats_ss_t *sta = (typeof(sta)) (arg + 1);
			if (!ss || (size -= sizeof(*sta)) < 0)
				goto einval;
			d = (typeof(d)) & ss->stats;
			for (size = sizeof(*sta); size--; *d &= *s) ;
			break;
		}
		case SCCP_OBJ_TYPE_RS:
		{
			struct rs *rs = rs_lookup(arg->id);
			sccp_stats_rs_t *sta = (typeof(sta)) (arg + 1);
			if (!rs || (size -= sizeof(*sta)) < 0)
				goto einval;
			d = (typeof(d)) & rs->stats;
			for (size = sizeof(*sta); size--; *d &= *s) ;
			break;
		}
		case SCCP_OBJ_TYPE_SR:
		{
			struct sr *sr = sr_lookup(arg->id);
			sccp_stats_sr_t *sta = (typeof(sta)) (arg + 1);
			if (!sr || (size -= sizeof(*sta)) < 0)
				goto einval;
			d = (typeof(d)) & sr->stats;
			for (size = sizeof(*sta); size--; *d &= *s) ;
			break;
		}
		case SCCP_OBJ_TYPE_SP:
		{
			struct sp *sp = sp_lookup(arg->id);
			sccp_stats_sp_t *sta = (typeof(sta)) (arg + 1);
			if (!sp || (size -= sizeof(*sta)) < 0)
				goto einval;
			d = (typeof(d)) & sp->stats;
			for (size = sizeof(*sta); size--; *d &= *s) ;
			break;
		}
		case SCCP_OBJ_TYPE_MT:
		{
			struct mt *mt = mtp_lookup(arg->id);
			sccp_stats_mt_t *sta = (typeof(sta)) (arg + 1);
			if (!mt || (size -= sizeof(*sta)) < 0)
				goto einval;
			d = (typeof(d)) & mt->stats;
			for (size = sizeof(*sta); size--; *d &= *s) ;
			break;
		}
		case SCCP_OBJ_TYPE_DF:
		{
			struct df *df = &master;
			sccp_stats_df_t *sta = (typeof(sta)) (arg + 1);
			if (!df || (size -= sizeof(*sta)) < 0)
				goto einval;
			d = (typeof(d)) & df->stats;
			for (size = sizeof(*sta); size--; *d &= *s) ;
			break;
		}
		default:
			rare();
		      einval:
			ret = -EINVAL;
			break;
		}
		spin_unlock_irqrestore(&master.lock, flags);
		return (ret);
	}
	rare();
	return -EINVAL;
}

/*
 *  SCCP_IOCGNOTIFY
 *  -----------------------------------
 */
STATIC int
sccp_iocgnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int size = msgdsize(mp);
		struct sccp_notify *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) >= 0) {
			switch (arg->type) {
			case SCCP_OBJ_TYPE_SC:
			{
				struct sc *sc = sccp_lookup(arg->id);
				sccp_notify_sc_t *not = (typeof(not)) (arg + 1);
				if (!sc || (size -= sizeof(*not)) < 0)
					goto einval;
				not->events = sc->notify.events;
				break;
			}
			case SCCP_OBJ_TYPE_NA:
			{
				struct na *na = na_lookup(arg->id);
				sccp_notify_na_t *not = (typeof(not)) (arg + 1);
				if (!na || (size -= sizeof(*not)) < 0)
					goto einval;
				not->events = na->notify.events;
				break;
			}
			case SCCP_OBJ_TYPE_CP:
			{
				struct cp *cp = cp_lookup(arg->id);
				sccp_notify_cp_t *not = (typeof(not)) (arg + 1);
				if (!cp || (size -= sizeof(*not)) < 0)
					goto einval;
				not->events = cp->notify.events;
				break;
			}
			case SCCP_OBJ_TYPE_SS:
			{
				struct ss *ss = ss_lookup(arg->id);
				sccp_notify_ss_t *not = (typeof(not)) (arg + 1);
				if (!ss || (size -= sizeof(*not)) < 0)
					goto einval;
				not->events = ss->notify.events;
				break;
			}
			case SCCP_OBJ_TYPE_RS:
			{
				struct rs *rs = rs_lookup(arg->id);
				sccp_notify_rs_t *not = (typeof(not)) (arg + 1);
				if (!rs || (size -= sizeof(*not)) < 0)
					goto einval;
				not->events = rs->notify.events;
				break;
			}
			case SCCP_OBJ_TYPE_SR:
			{
				struct sr *sr = sr_lookup(arg->id);
				sccp_notify_sr_t *not = (typeof(not)) (arg + 1);
				if (!sr || (size -= sizeof(*not)) < 0)
					goto einval;
				not->events = sr->notify.events;
				break;
			}
			case SCCP_OBJ_TYPE_SP:
			{
				struct sp *sp = sp_lookup(arg->id);
				sccp_notify_sp_t *not = (typeof(not)) (arg + 1);
				if (!sp || (size -= sizeof(*not)) < 0)
					goto einval;
				not->events = sp->notify.events;
				break;
			}
			case SCCP_OBJ_TYPE_MT:
			{
				struct mt *mt = mtp_lookup(arg->id);
				sccp_notify_mt_t *not = (typeof(not)) (arg + 1);
				if (!mt || (size -= sizeof(*not)) < 0)
					goto einval;
				not->events = mt->notify.events;
				break;
			}
			case SCCP_OBJ_TYPE_DF:
			{
				struct df *df = &master;
				sccp_notify_df_t *not = (typeof(not)) (arg + 1);
				if (!df || (size -= sizeof(*not)) < 0)
					goto einval;
				not->events = df->notify.events;
				break;
			}
			default:
				rare();
				goto einval;
			}
			return (QR_DONE);
		}
	}
	rare();
      einval:
	return -EINVAL;
}

/*
 *  SCCP_IOCSNOTIFY
 *  -----------------------------------
 */
STATIC int
sccp_iocsnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int size = msgdsize(mp);
		struct sccp_notify *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) >= 0) {
			switch (arg->type) {
			case SCCP_OBJ_TYPE_SC:
			{
				struct sc *sc = sccp_lookup(arg->id);
				sccp_notify_sc_t *not = (typeof(not)) (arg + 1);
				if (!sc || (size -= sizeof(*not)) < 0)
					goto einval;
				sc->notify.events |= not->events;
				break;
			}
			case SCCP_OBJ_TYPE_NA:
			{
				struct na *na = na_lookup(arg->id);
				sccp_notify_na_t *not = (typeof(not)) (arg + 1);
				if (!na || (size -= sizeof(*not)) < 0)
					goto einval;
				na->notify.events |= not->events;
				break;
			}
			case SCCP_OBJ_TYPE_CP:
			{
				struct cp *cp = cp_lookup(arg->id);
				sccp_notify_cp_t *not = (typeof(not)) (arg + 1);
				if (!cp || (size -= sizeof(*not)) < 0)
					goto einval;
				cp->notify.events |= not->events;
				break;
			}
			case SCCP_OBJ_TYPE_SS:
			{
				struct ss *ss = ss_lookup(arg->id);
				sccp_notify_ss_t *not = (typeof(not)) (arg + 1);
				if (!ss || (size -= sizeof(*not)) < 0)
					goto einval;
				ss->notify.events |= not->events;
				break;
			}
			case SCCP_OBJ_TYPE_RS:
			{
				struct rs *rs = rs_lookup(arg->id);
				sccp_notify_rs_t *not = (typeof(not)) (arg + 1);
				if (!rs || (size -= sizeof(*not)) < 0)
					goto einval;
				rs->notify.events |= not->events;
				break;
			}
			case SCCP_OBJ_TYPE_SR:
			{
				struct sr *sr = sr_lookup(arg->id);
				sccp_notify_sr_t *not = (typeof(not)) (arg + 1);
				if (!sr || (size -= sizeof(*not)) < 0)
					goto einval;
				sr->notify.events |= not->events;
				break;
			}
			case SCCP_OBJ_TYPE_SP:
			{
				struct sp *sp = sp_lookup(arg->id);
				sccp_notify_sp_t *not = (typeof(not)) (arg + 1);
				if (!sp || (size -= sizeof(*not)) < 0)
					goto einval;
				sp->notify.events |= not->events;
				break;
			}
			case SCCP_OBJ_TYPE_MT:
			{
				struct mt *mt = mtp_lookup(arg->id);
				sccp_notify_mt_t *not = (typeof(not)) (arg + 1);
				if (!mt || (size -= sizeof(*not)) < 0)
					goto einval;
				mt->notify.events |= not->events;
				break;
			}
			case SCCP_OBJ_TYPE_DF:
			{
				struct df *df = &master;
				sccp_notify_df_t *not = (typeof(not)) (arg + 1);
				if (!df || (size -= sizeof(*not)) < 0)
					goto einval;
				df->notify.events |= not->events;
				break;
			}
			default:
				rare();
				goto einval;
			}
		}
	}
	rare();
      einval:
	return -EINVAL;
}

/*
 *  SCCP_IOCCNOTIFY
 *  -----------------------------------
 */
STATIC int
sccp_ioccnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int size = msgdsize(mp);
		struct sccp_notify *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) >= 0) {
			switch (arg->type) {
			case SCCP_OBJ_TYPE_SC:
			{
				struct sc *sc = sccp_lookup(arg->id);
				sccp_notify_sc_t *not = (typeof(not)) (arg + 1);
				if (!sc || (size -= sizeof(*not)) < 0)
					goto einval;
				sc->notify.events &= ~not->events;
				break;
			}
			case SCCP_OBJ_TYPE_NA:
			{
				struct na *na = na_lookup(arg->id);
				sccp_notify_na_t *not = (typeof(not)) (arg + 1);
				if (!na || (size -= sizeof(*not)) < 0)
					goto einval;
				na->notify.events &= ~not->events;
				break;
			}
			case SCCP_OBJ_TYPE_CP:
			{
				struct cp *cp = cp_lookup(arg->id);
				sccp_notify_cp_t *not = (typeof(not)) (arg + 1);
				if (!cp || (size -= sizeof(*not)) < 0)
					goto einval;
				cp->notify.events &= ~not->events;
				break;
			}
			case SCCP_OBJ_TYPE_SS:
			{
				struct ss *ss = ss_lookup(arg->id);
				sccp_notify_ss_t *not = (typeof(not)) (arg + 1);
				if (!ss || (size -= sizeof(*not)) < 0)
					goto einval;
				ss->notify.events &= ~not->events;
				break;
			}
			case SCCP_OBJ_TYPE_RS:
			{
				struct rs *rs = rs_lookup(arg->id);
				sccp_notify_rs_t *not = (typeof(not)) (arg + 1);
				if (!rs || (size -= sizeof(*not)) < 0)
					goto einval;
				rs->notify.events &= ~not->events;
				break;
			}
			case SCCP_OBJ_TYPE_SR:
			{
				struct sr *sr = sr_lookup(arg->id);
				sccp_notify_sr_t *not = (typeof(not)) (arg + 1);
				if (!sr || (size -= sizeof(*not)) < 0)
					goto einval;
				sr->notify.events &= ~not->events;
				break;
			}
			case SCCP_OBJ_TYPE_SP:
			{
				struct sp *sp = sp_lookup(arg->id);
				sccp_notify_sp_t *not = (typeof(not)) (arg + 1);
				if (!sp || (size -= sizeof(*not)) < 0)
					goto einval;
				sp->notify.events &= ~not->events;
				break;
			}
			case SCCP_OBJ_TYPE_MT:
			{
				struct mt *mt = mtp_lookup(arg->id);
				sccp_notify_mt_t *not = (typeof(not)) (arg + 1);
				if (!mt || (size -= sizeof(*not)) < 0)
					goto einval;
				mt->notify.events &= ~not->events;
				break;
			}
			case SCCP_OBJ_TYPE_DF:
			{
				struct df *df = &master;
				sccp_notify_df_t *not = (typeof(not)) (arg + 1);
				if (!df || (size -= sizeof(*not)) < 0)
					goto einval;
				df->notify.events &= ~not->events;
				break;
			}
			default:
				rare();
				goto einval;
			}
			return (QR_DONE);
		}
	}
	rare();
      einval:
	return -EINVAL;
}

/*
 *  SCCP_IOCCMGMT
 *  -----------------------------------
 */
STATIC int
sccp_ioccmgmt(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sccp_mgmt_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		switch (arg->cmd) {
		case SCCP_MGMT_ALLOW:
			switch (arg->type) {
			case SCCP_OBJ_TYPE_SC:
				return sccp_mgmt_all_sc(q, sccp_lookup(arg->id));
			case SCCP_OBJ_TYPE_NA:
				return sccp_mgmt_all_na(q, na_lookup(arg->id));
			case SCCP_OBJ_TYPE_CP:
				return sccp_mgmt_all_cp(q, cp_lookup(arg->id));
			case SCCP_OBJ_TYPE_SS:
				return sccp_mgmt_all_ss(q, ss_lookup(arg->id));
			case SCCP_OBJ_TYPE_RS:
				return sccp_mgmt_all_rs(q, rs_lookup(arg->id));
			case SCCP_OBJ_TYPE_SR:
				return sccp_mgmt_all_sr(q, sr_lookup(arg->id));
			case SCCP_OBJ_TYPE_SP:
				return sccp_mgmt_all_sp(q, sp_lookup(arg->id));
			case SCCP_OBJ_TYPE_MT:
				return sccp_mgmt_all_mt(q, mtp_lookup(arg->id));
			case SCCP_OBJ_TYPE_DF:
				return sccp_mgmt_all_df(q, &master);
			}
			break;
		case SCCP_MGMT_PROHIBIT:
			switch (arg->type) {
			case SCCP_OBJ_TYPE_SC:
				return sccp_mgmt_pro_sc(q, sccp_lookup(arg->id));
			case SCCP_OBJ_TYPE_NA:
				return sccp_mgmt_pro_na(q, na_lookup(arg->id));
			case SCCP_OBJ_TYPE_CP:
				return sccp_mgmt_pro_cp(q, cp_lookup(arg->id));
			case SCCP_OBJ_TYPE_SS:
				return sccp_mgmt_pro_ss(q, ss_lookup(arg->id));
			case SCCP_OBJ_TYPE_RS:
				return sccp_mgmt_pro_rs(q, rs_lookup(arg->id));
			case SCCP_OBJ_TYPE_SR:
				return sccp_mgmt_pro_sr(q, sr_lookup(arg->id));
			case SCCP_OBJ_TYPE_SP:
				return sccp_mgmt_pro_sp(q, sp_lookup(arg->id));
			case SCCP_OBJ_TYPE_MT:
				return sccp_mgmt_pro_mt(q, mtp_lookup(arg->id));
			case SCCP_OBJ_TYPE_DF:
				return sccp_mgmt_pro_df(q, &master);
			}
			break;
		case SCCP_MGMT_BLOCK:
			switch (arg->type) {
			case SCCP_OBJ_TYPE_SC:
				return sccp_mgmt_blo_sc(q, sccp_lookup(arg->id));
			case SCCP_OBJ_TYPE_NA:
				return sccp_mgmt_blo_na(q, na_lookup(arg->id));
			case SCCP_OBJ_TYPE_CP:
				return sccp_mgmt_blo_cp(q, cp_lookup(arg->id));
			case SCCP_OBJ_TYPE_SS:
				return sccp_mgmt_blo_ss(q, ss_lookup(arg->id));
			case SCCP_OBJ_TYPE_RS:
				return sccp_mgmt_blo_rs(q, rs_lookup(arg->id));
			case SCCP_OBJ_TYPE_SR:
				return sccp_mgmt_blo_sr(q, sr_lookup(arg->id));
			case SCCP_OBJ_TYPE_SP:
				return sccp_mgmt_blo_sp(q, sp_lookup(arg->id));
			case SCCP_OBJ_TYPE_MT:
				return sccp_mgmt_blo_mt(q, mtp_lookup(arg->id));
			case SCCP_OBJ_TYPE_DF:
				return sccp_mgmt_blo_df(q, &master);
			}
			break;
		case SCCP_MGMT_UNBLOCK:
			switch (arg->type) {
			case SCCP_OBJ_TYPE_SC:
				return sccp_mgmt_ubl_sc(q, sccp_lookup(arg->id));
			case SCCP_OBJ_TYPE_NA:
				return sccp_mgmt_ubl_na(q, na_lookup(arg->id));
			case SCCP_OBJ_TYPE_CP:
				return sccp_mgmt_ubl_cp(q, cp_lookup(arg->id));
			case SCCP_OBJ_TYPE_SS:
				return sccp_mgmt_ubl_ss(q, ss_lookup(arg->id));
			case SCCP_OBJ_TYPE_RS:
				return sccp_mgmt_ubl_rs(q, rs_lookup(arg->id));
			case SCCP_OBJ_TYPE_SR:
				return sccp_mgmt_ubl_sr(q, sr_lookup(arg->id));
			case SCCP_OBJ_TYPE_SP:
				return sccp_mgmt_ubl_sp(q, sp_lookup(arg->id));
			case SCCP_OBJ_TYPE_MT:
				return sccp_mgmt_ubl_mt(q, mtp_lookup(arg->id));
			case SCCP_OBJ_TYPE_DF:
				return sccp_mgmt_ubl_df(q, &master);
			}
			break;
		case SCCP_MGMT_RESTART:
			switch (arg->type) {
			case SCCP_OBJ_TYPE_SC:
				return sccp_mgmt_res_sc(q, sccp_lookup(arg->id));
			case SCCP_OBJ_TYPE_NA:
				return sccp_mgmt_res_na(q, na_lookup(arg->id));
			case SCCP_OBJ_TYPE_CP:
				return sccp_mgmt_res_cp(q, cp_lookup(arg->id));
			case SCCP_OBJ_TYPE_SS:
				return sccp_mgmt_res_ss(q, ss_lookup(arg->id));
			case SCCP_OBJ_TYPE_RS:
				return sccp_mgmt_res_rs(q, rs_lookup(arg->id));
			case SCCP_OBJ_TYPE_SR:
				return sccp_mgmt_res_sr(q, sr_lookup(arg->id));
			case SCCP_OBJ_TYPE_SP:
				return sccp_mgmt_res_sp(q, sp_lookup(arg->id));
			case SCCP_OBJ_TYPE_MT:
				return sccp_mgmt_res_mt(q, mtp_lookup(arg->id));
			case SCCP_OBJ_TYPE_DF:
				return sccp_mgmt_res_df(q, &master);
			}
			break;
		}
	}
	rare();
	return (-EINVAL);
}

/*
 *  SCCP_IOCCPASS
 *  -----------------------------------
 */
STATIC int
sccp_ioccpass(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		sccp_pass_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		mblk_t *bp, *dp;
		struct mt *mt;
		for (mt = master.mt.list; mt && mt->u.mux.index != arg->muxid; mt = mt->next) ;
		if (!mt || !mt->oq)
			return -EINVAL;
		if (arg->type < QPCTL && !canput(mt->oq))
			return -EBUSY;
		if (!(bp = ss7_dupb(q, mp)))
			return -ENOBUFS;
		if (!(dp = ss7_dupb(q, mp))) {
			freeb(bp);
			return -ENOBUFS;
		}
		bp->b_datap->db_type = arg->type;
		bp->b_band = arg->band;
		bp->b_cont = dp;
		bp->b_rptr += sizeof(*arg);
		bp->b_wptr = bp->b_rptr + arg->ctl_length;
		dp->b_datap->db_type = M_DATA;
		dp->b_rptr += sizeof(*arg) + arg->ctl_length;
		dp->b_wptr = dp->b_rptr + arg->dat_length;
		ss7_oput(mt->oq, bp);
		return (QR_DONE);
	}
	rare();
	return -EINVAL;
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
sccp_w_ioctl(queue_t *q, mblk_t *mp)
{
	struct sc *sc = SCCP_PRIV(q);
	struct iocblk *iocp = (struct iocblk *) mp->b_rptr;
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	int cmd = iocp->ioc_cmd, count = iocp->ioc_count;
	int type = _IOC_TYPE(cmd), nr = _IOC_NR(cmd), size = _IOC_SIZE(cmd);
	int ret = 0;
	(void) sc;
	switch (type) {
	case __SID:
	{
		struct mt *mt;
		struct linkblk *lb;
		if (!(lb = arg)) {
			swerr();
			ret = (-EINVAL);
			break;
		}
		switch (nr) {
		case _IOC_NR(I_PLINK):
			if (iocp->ioc_cr->cr_uid != 0) {
				ptrace(("%s: %p: ERROR: Non-root attempt to I_PLINK\n", DRV_NAME,
					sc));
				ret = -EPERM;
				break;
			}
		case _IOC_NR(I_LINK):
			if ((mt =
			     sccp_alloc_link(lb->l_qbot, &master.mt.list, lb->l_index,
					     iocp->ioc_cr)))
				break;
			ret = -ENOMEM;
			break;
		case _IOC_NR(I_PUNLINK):
			if (iocp->ioc_cr->cr_uid != 0) {
				ptrace(("%s: %p: ERROR: Non-root attempt to I_PUNLINK\n", DRV_NAME,
					sc));
				ret = -EPERM;
				break;
			}
		case _IOC_NR(I_UNLINK):
			for (mt = master.mt.list; mt; mt = mt->next)
				if (mt->u.mux.index == lb->l_index)
					break;
			if (!mt) {
				ret = -EINVAL;
				ptrace(("%s: %p: ERROR: could not find I_UNLINK muxid\n", DRV_NAME,
					sc));
				break;
			}
			sccp_free_link(mt);
			break;
		default:
		case _IOC_NR(I_STR):
			ptrace(("%s: %p: ERROR: Unsupported STREAMS ioctl %d\n", DRV_NAME, sc, nr));
			ret = (-EOPNOTSUPP);
			break;
		}
		break;
	}
	case SCCP_IOC_MAGIC:
	{
		if (count < size) {
			ret = -EINVAL;
			break;
		}
		switch (nr) {
		case _IOC_NR(SCCP_IOCGOPTION):
			printd(("%s: %p: -> SCCP_IOCGOPTION\n", DRV_NAME, sc));
			ret = sccp_iocgoptions(q, mp);
			break;
		case _IOC_NR(SCCP_IOCSOPTION):
			printd(("%s: %p: -> SCCP_IOCSOPTION\n", DRV_NAME, sc));
			ret = sccp_iocsoption(q, mp);
			break;
		case _IOC_NR(SCCP_IOCGCONFIG):
			printd(("%s: %p: -> SCCP_IOCGCONFIG\n", DRV_NAME, sc));
			ret = sccp_iocgconfig(q, mp);
			break;
		case _IOC_NR(SCCP_IOCSCONFIG):
			printd(("%s: %p: -> SCCP_IOCSCONFIG\n", DRV_NAME, sc));
			ret = sccp_iocsconfig(q, mp);
			break;
		case _IOC_NR(SCCP_IOCTCONFIG):
			printd(("%s: %p: -> SCCP_IOCTCONFIG\n", DRV_NAME, sc));
			ret = sccp_ioctconfig(q, mp);
			break;
		case _IOC_NR(SCCP_IOCCCONFIG):
			printd(("%s: %p: -> SCCP_IOCCCONFIG\n", DRV_NAME, sc));
			ret = sccp_ioccconfig(q, mp);
			break;
		case _IOC_NR(SCCP_IOCGSTATEM):
			printd(("%s: %p: -> SCCP_IOCGSTATEM\n", DRV_NAME, sc));
			ret = sccp_iocgstatem(q, mp);
			break;
		case _IOC_NR(SCCP_IOCCMRESET):
			printd(("%s: %p: -> SCCP_IOCCMRESET\n", DRV_NAME, sc));
			ret = sccp_ioccmreset(q, mp);
			break;
		case _IOC_NR(SCCP_IOCGSTATSP):
			printd(("%s: %p: -> SCCP_IOCGSTATSP\n", DRV_NAME, sc));
			ret = sccp_iocgstatsp(q, mp);
			break;
		case _IOC_NR(SCCP_IOCSSTATSP):
			printd(("%s: %p: -> SCCP_IOCSSTATSP\n", DRV_NAME, sc));
			ret = sccp_iocsstatsp(q, mp);
			break;
		case _IOC_NR(SCCP_IOCGSTATS):
			printd(("%s: %p: -> SCCP_IOCGSTATS\n", DRV_NAME, sc));
			ret = sccp_iocgstats(q, mp);
			break;
		case _IOC_NR(SCCP_IOCSSTATS):
			printd(("%s: %p: -> SCCP_IOCSSTATS\n", DRV_NAME, sc));
			ret = sccp_iocsstats(q, mp);
			break;
		case _IOC_NR(SCCP_IOCGNOTIFY):
			printd(("%s: %p: -> SCCP_IOCGNOTIFY\n", DRV_NAME, sc));
			ret = sccp_iocgnotify(q, mp);
			break;
		case _IOC_NR(SCCP_IOCSNOTIFY):
			printd(("%s: %p: -> SCCP_IOCSNOTIFY\n", DRV_NAME, sc));
			ret = sccp_iocsnotify(q, mp);
			break;
		case _IOC_NR(SCCP_IOCCNOTIFY):
			printd(("%s: %p: -> SCCP_IOCCNOTIFY\n", DRV_NAME, sc));
			ret = sccp_ioccnotify(q, mp);
			break;
		case _IOC_NR(SCCP_IOCCMGMT):
			printd(("%s: %p: -> SCCP_IOCCMGMT\n", DRV_NAME, sc));
			ret = sccp_ioccmgmt(q, mp);
			break;
		case _IOC_NR(SCCP_IOCCPASS):
			printd(("%s: %p: -> SCCP_IOCCPASS\n", DRV_NAME, sc));
			ret = sccp_ioccpass(q, mp);
			break;
		default:
			ptrace(("%s: %p: ERROR: Unsupported SCCP ioctl %d\n", DRV_NAME, sc, nr));
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	}
		/* 
		   TODO: Need to add standard TPI/NPI ioctls */
	default:
		ret = (-EOPNOTSUPP);
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
 *  M_RSE, M_PCRSE Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sccp_w_rse(queue_t *q, mblk_t *mp)
{
	return sccp_orte_msg(q, mp);
}
STATIC int
sccp_r_rse(queue_t *q, mblk_t *mp)
{
	return sccp_recv_msg(q, mp);
}
STATIC int
mtp_w_rse(queue_t *q, mblk_t *mp)
{
	return sccp_send_msg(q, mp);
}
STATIC int
mtp_r_rse(queue_t *q, mblk_t *mp)
{
	return sccp_irte_msg(q, mp);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_PROTO, M_PCPROTO Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
mtp_r_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	ulong prim;
	struct mt *mt = MTP_PRIV(q);
	// ulong oldstate = mtp_get_state(mt);
	/* 
	   fast path */
	if ((prim = *(ulong *) mp->b_rptr) == MTP_TRANSFER_IND) {
		printd(("%s: %p: MTP_TRANSFER_IND [%d] <-\n", DRV_NAME, mt, msgdsize(mp->b_cont)));
		rtn = mtp_transfer_ind(q, mt, mp);
		// if (rtn < 0)
		// mtp_set_state(mt, oldstate);
		return (rtn);
	}
	switch ((prim = *(ulong *) mp->b_rptr)) {
	case MTP_OK_ACK:
		printd(("%s: %p: MTP_OK_ACK <-\n", DRV_NAME, mt));
		rtn = mtp_ok_ack(q, mt, mp);
		break;
	case MTP_ERROR_ACK:
		printd(("%s: %p: MTP_ERROR_ACK <-\n", DRV_NAME, mt));
		rtn = mtp_error_ack(q, mt, mp);
		break;
	case MTP_BIND_ACK:
		printd(("%s: %p: MTP_BIND_ACK <-\n", DRV_NAME, mt));
		rtn = mtp_bind_ack(q, mt, mp);
		break;
	case MTP_ADDR_ACK:
		printd(("%s: %p: MTP_ADDR_ACK <-\n", DRV_NAME, mt));
		rtn = mtp_addr_ack(q, mt, mp);
		break;
	case MTP_INFO_ACK:
		printd(("%s: %p: MTP_INFO_ACK <-\n", DRV_NAME, mt));
		rtn = mtp_info_ack(q, mt, mp);
		break;
	case MTP_OPTMGMT_ACK:
		printd(("%s: %p: MTP_OPGMGMT_ACK <-\n", DRV_NAME, mt));
		rtn = mtp_optmgmt_ack(q, mt, mp);
		break;
	case MTP_TRANSFER_IND:
		printd(("%s: %p: MTP_TRANSFER_IND [%d] <-\n", DRV_NAME, mt, msgdsize(mp->b_cont)));
		rtn = mtp_transfer_ind(q, mt, mp);
		break;
	case MTP_PAUSE_IND:
		printd(("%s: %p: MTP_PAUSE_IND <-\n", DRV_NAME, mt));
		rtn = mtp_pause_ind(q, mt, mp);
		break;
	case MTP_RESUME_IND:
		printd(("%s: %p: MTP_RESUME_IND <-\n", DRV_NAME, mt));
		rtn = mtp_resume_ind(q, mt, mp);
		break;
	case MTP_STATUS_IND:
		printd(("%s: %p: MTP_STATUS_IND <-\n", DRV_NAME, mt));
		rtn = mtp_status_ind(q, mt, mp);
		break;
	case MTP_RESTART_BEGINS_IND:
		printd(("%s: %p: MTP_RESTART_BEGINS_IND <-\n", DRV_NAME, mt));
		rtn = mtp_restart_begins_ind(q, mt, mp);
		break;
	case MTP_RESTART_COMPLETE_IND:
		printd(("%s: %p: MTP_RESTART_COMPLETE_IND <-\n", DRV_NAME, mt));
		rtn = mtp_restart_complete_ind(q, mt, mp);
		break;
	default:
		printd(("%s: %p: ??? %lu <-\n", DRV_NAME, mt, prim));
		rtn = mtp_unrecognized_prim(q, mt, mp);
		break;
	}
	// if (rtn < 0)
	// mtp_set_state(mt, oldstate);
	return (rtn);
}

STATIC int
sccp_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	ulong prim;
	struct sc *sc = SCCP_PRIV(q);
	ulong oldstate = sccp_get_state(sc);
	switch ((prim = *(ulong *) mp->b_rptr)) {
	case N_CONN_REQ:
		printd(("%s: %p: -> N_CONN_REQ\n", DRV_NAME, sc));
		rtn = n_conn_req(q, sc, mp);
		break;
	case N_CONN_RES:
		printd(("%s: %p: -> N_CONN_RES\n", DRV_NAME, sc));
		rtn = n_conn_res(q, sc, mp);
		break;
	case N_DISCON_REQ:
		printd(("%s: %p: -> N_DISCON_REQ\n", DRV_NAME, sc));
		rtn = n_discon_req(q, sc, mp);
		break;
	case N_DATA_REQ:
		printd(("%s: %p: -> N_DATA_REQ\n", DRV_NAME, sc));
		rtn = n_data_req(q, sc, mp);
		break;
	case N_EXDATA_REQ:
		printd(("%s: %p: -> N_EXDATA_REQ\n", DRV_NAME, sc));
		rtn = n_exdata_req(q, sc, mp);
		break;
	case N_INFO_REQ:
		printd(("%s: %p: -> N_INFO_REQ\n", DRV_NAME, sc));
		rtn = n_info_req(q, sc, mp);
		break;
	case N_BIND_REQ:
		printd(("%s: %p: -> N_BIND_REQ\n", DRV_NAME, sc));
		rtn = n_bind_req(q, sc, mp);
		break;
	case N_UNBIND_REQ:
		printd(("%s: %p: -> N_UNBIND_REQ\n", DRV_NAME, sc));
		rtn = n_unbind_req(q, sc, mp);
		break;
	case N_UNITDATA_REQ:
		printd(("%s: %p: -> N_UNITDATA_REQ\n", DRV_NAME, sc));
		rtn = n_unitdata_req(q, sc, mp);
		break;
	case N_OPTMGMT_REQ:
		printd(("%s: %p: -> N_OPTMGMT_REQ\n", DRV_NAME, sc));
		rtn = n_optmgmt_req(q, sc, mp);
		break;
	case N_DATACK_REQ:
		printd(("%s: %p: -> N_DATACK_REQ\n", DRV_NAME, sc));
		rtn = n_datack_req(q, sc, mp);
		break;
	case N_RESET_REQ:
		printd(("%s: %p: -> N_RESET_REQ\n", DRV_NAME, sc));
		rtn = n_reset_req(q, sc, mp);
		break;
	case N_RESET_RES:
		printd(("%s: %p: -> N_RESET_RES\n", DRV_NAME, sc));
		rtn = n_reset_res(q, sc, mp);
		break;
	case N_INFORM_REQ:
		printd(("%s: %p: -> N_INFORM_REQ\n", DRV_NAME, sc));
		rtn = n_inform_req(q, sc, mp);
		break;
	case N_COORD_REQ:
		printd(("%s: %p: -> N_COORD_REQ\n", DRV_NAME, sc));
		rtn = n_coord_req(q, sc, mp);
		break;
	case N_COORD_RES:
		printd(("%s: %p: -> N_COORD_RES\n", DRV_NAME, sc));
		rtn = n_coord_res(q, sc, mp);
		break;
	case N_STATE_REQ:
		printd(("%s: %p: -> N_STATE_REQ\n", DRV_NAME, sc));
		rtn = n_state_req(q, sc, mp);
		break;
	default:
		rtn = n_error_ack(q, sc, prim, NNOTSUPPORT);
		break;
	}
	if (rtn < 0)
		sccp_set_state(sc, oldstate);
	return (rtn);
}

STATIC int
tpi_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	ulong prim;
	struct sc *sc = SCCP_PRIV(q);
	ulong oldstate = sccp_get_state(sc);
	switch ((prim = *(ulong *) mp->b_rptr)) {
	case T_CONN_REQ:
		printd(("%s: %p: -> T_CONN_REQ\n", DRV_NAME, sc));
		rtn = t_conn_req(q, sc, mp);
		break;
	case T_CONN_RES:
		printd(("%s: %p: -> T_CONN_RES\n", DRV_NAME, sc));
		rtn = t_conn_res(q, sc, mp);
		break;
	case T_DISCON_REQ:
		printd(("%s: %p: -> T_DISCON_REQ\n", DRV_NAME, sc));
		rtn = t_discon_req(q, sc, mp);
		break;
	case T_DATA_REQ:
		printd(("%s: %p: -> T_DATA_REQ\n", DRV_NAME, sc));
		rtn = t_data_req(q, sc, mp);
		break;
	case T_EXDATA_REQ:
		printd(("%s: %p: -> T_EXDATA_REQ\n", DRV_NAME, sc));
		rtn = t_exdata_req(q, sc, mp);
		break;
	case T_INFO_REQ:
		printd(("%s: %p: -> T_INFO_REQ\n", DRV_NAME, sc));
		rtn = t_info_req(q, sc, mp);
		break;
	case T_BIND_REQ:
		printd(("%s: %p: -> T_BIND_REQ\n", DRV_NAME, sc));
		rtn = t_bind_req(q, sc, mp);
		break;
	case T_UNBIND_REQ:
		printd(("%s: %p: -> T_UNBIND_REQ\n", DRV_NAME, sc));
		rtn = t_unbind_req(q, sc, mp);
		break;
	case T_UNITDATA_REQ:
		printd(("%s: %p: -> T_UNITDATA_REQ\n", DRV_NAME, sc));
		rtn = t_unitdata_req(q, sc, mp);
		break;
	case T_OPTMGMT_REQ:
		printd(("%s: %p: -> T_OPTMGMT_REQ\n", DRV_NAME, sc));
		rtn = t_optmgmt_req(q, sc, mp);
		break;
	case T_ORDREL_REQ:
		printd(("%s: %p: -> T_ORDREL_REQ\n", DRV_NAME, sc));
		rtn = t_ordrel_req(q, sc, mp);
		break;
	case T_OPTDATA_REQ:
		printd(("%s: %p: -> T_OPTDATA_REQ\n", DRV_NAME, sc));
		rtn = t_optdata_req(q, sc, mp);
		break;
#ifdef T_ADDR_REQ
	case T_ADDR_REQ:
		printd(("%s: %p: -> T_ADDR_REQ\n", DRV_NAME, sc));
		rtn = t_addr_req(q, sc, mp);
		break;
#endif
#ifdef T_CAPABILITY_REQ
	case T_CAPABILITY_REQ:
		printd(("%s: %p: -> T_CAPABILITY_REQ\n", DRV_NAME, sc));
		rtn = t_capability_req(q, sc, mp);
		break;
#endif
	default:
		rtn = t_error_ack(q, sc, prim, TNOTSUPPORT);
		break;
	}
	if (rtn < 0)
		sccp_set_state(sc, oldstate);
	return (rtn);
}

STATIC int
npi_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	ulong prim;
	struct sc *sc = SCCP_PRIV(q);
	ulong oldstate = sccp_get_state(sc);
	switch ((prim = *(ulong *) mp->b_rptr)) {
	case N_CONN_REQ:
		printd(("%s: %p: -> N_CONN_REQ\n", DRV_NAME, sc));
		rtn = n_conn_req(q, sc, mp);
		break;
	case N_CONN_RES:
		printd(("%s: %p: -> N_CONN_RES\n", DRV_NAME, sc));
		rtn = n_conn_res(q, sc, mp);
		break;
	case N_DISCON_REQ:
		printd(("%s: %p: -> N_DISCON_REQ\n", DRV_NAME, sc));
		rtn = n_discon_req(q, sc, mp);
		break;
	case N_DATA_REQ:
		printd(("%s: %p: -> N_DATA_REQ\n", DRV_NAME, sc));
		rtn = n_data_req(q, sc, mp);
		break;
	case N_EXDATA_REQ:
		printd(("%s: %p: -> N_EXDATA_REQ\n", DRV_NAME, sc));
		rtn = n_exdata_req(q, sc, mp);
		break;
	case N_INFO_REQ:
		printd(("%s: %p: -> N_INFO_REQ\n", DRV_NAME, sc));
		rtn = n_info_req(q, sc, mp);
		break;
	case N_BIND_REQ:
		printd(("%s: %p: -> N_BIND_REQ\n", DRV_NAME, sc));
		rtn = n_bind_req(q, sc, mp);
		break;
	case N_UNBIND_REQ:
		printd(("%s: %p: -> N_UNBIND_REQ\n", DRV_NAME, sc));
		rtn = n_unbind_req(q, sc, mp);
		break;
	case N_UNITDATA_REQ:
		printd(("%s: %p: -> N_UNITDATA_REQ\n", DRV_NAME, sc));
		rtn = n_unitdata_req(q, sc, mp);
		break;
	case N_OPTMGMT_REQ:
		printd(("%s: %p: -> N_OPTMGMT_REQ\n", DRV_NAME, sc));
		rtn = n_optmgmt_req(q, sc, mp);
		break;
	case N_DATACK_REQ:
		printd(("%s: %p: -> N_DATACK_REQ\n", DRV_NAME, sc));
		rtn = n_datack_req(q, sc, mp);
		break;
	case N_RESET_REQ:
		printd(("%s: %p: -> N_RESET_REQ\n", DRV_NAME, sc));
		rtn = n_reset_req(q, sc, mp);
		break;
	case N_RESET_RES:
		printd(("%s: %p: -> N_RESET_RES\n", DRV_NAME, sc));
		rtn = n_reset_res(q, sc, mp);
		break;
	default:
		rtn = n_error_ack(q, sc, prim, NNOTSUPPORT);
		break;
	}
	if (rtn < 0)
		sccp_set_state(sc, oldstate);
	return (rtn);
}

STATIC int
gtt_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	ulong prim;
	struct sc *sc = SCCP_PRIV(q);
	ulong oldstate = sccp_get_state(sc);
	switch ((prim = *(ulong *) mp->b_rptr)) {
	case N_BIND_REQ:
		printd(("%s: %p: -> N_BIND_REQ\n", DRV_NAME, sc));
		rtn = n_bind_req(q, sc, mp);
		break;
	case N_UNBIND_REQ:
		printd(("%s: %p: -> N_UNBIND_REQ\n", DRV_NAME, sc));
		rtn = n_unbind_req(q, sc, mp);
		break;
	case N_INFORM_REQ:
		printd(("%s: %p: -> N_INFORM_REQ\n", DRV_NAME, sc));
		rtn = n_inform_req(q, sc, mp);
		break;
	case N_STATE_REQ:
		printd(("%s: %p: -> N_STATE_REQ\n", DRV_NAME, sc));
		rtn = n_state_req(q, sc, mp);
		break;
#ifdef N_TRANSLATE_RES
	case N_TRANSLATE_RES:
		printd(("%s: %p: -> N_TRANSLATE_RES\n", DRV_NAME, sc));
		rtn = n_translate_res(q, sc, mp);
		break;
#endif
	default:
		rtn = -EOPNOTSUPP;
		break;
	}
	if (rtn < 0)
		sccp_set_state(sc, oldstate);
	return (rtn);
}

STATIC int
mgm_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	ulong prim;
	struct sc *sc = SCCP_PRIV(q);
	ulong oldstate = sccp_get_state(sc);
	switch ((prim = *(ulong *) mp->b_rptr)) {
	case N_INFORM_REQ:
		printd(("%s: %p: -> N_INFORM_REQ\n", DRV_NAME, sc));
		rtn = n_inform_req(q, sc, mp);
		break;
	case N_COORD_REQ:
		printd(("%s: %p: -> N_COORD_REQ\n", DRV_NAME, sc));
		rtn = n_coord_req(q, sc, mp);
		break;
	case N_COORD_RES:
		printd(("%s: %p: -> N_COORD_RES\n", DRV_NAME, sc));
		rtn = n_coord_res(q, sc, mp);
		break;
	case N_STATE_REQ:
		printd(("%s: %p: -> N_STATE_REQ\n", DRV_NAME, sc));
		rtn = n_state_req(q, sc, mp);
		break;
	default:
		rtn = -EOPNOTSUPP;
		break;
	}
	if (rtn < 0)
		sccp_set_state(sc, oldstate);
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
sccp_w_data(queue_t *q, mblk_t *mp)
{
	struct sc *sc = SCCP_PRIV(q);
	return n_write(q, sc, mp);
}
STATIC int
tpi_w_data(queue_t *q, mblk_t *mp)
{
	struct sc *sc = SCCP_PRIV(q);
	return t_write(q, sc, mp);
}
STATIC int
npi_w_data(queue_t *q, mblk_t *mp)
{
	struct sc *sc = SCCP_PRIV(q);
	return n_write(q, sc, mp);
}
STATIC int
gtt_w_data(queue_t *q, mblk_t *mp)
{
	swerr();
	return (-EFAULT);
}
STATIC int
mgm_w_data(queue_t *q, mblk_t *mp)
{
	swerr();
	return (-EFAULT);
}
STATIC int
mtp_r_data(queue_t *q, mblk_t *mp)
{
	struct mt *mt = MTP_PRIV(q);
	struct sr *sr;
	if ((sr = mt->sr))
		return mtp_read(q, mp, sr, 0, 0);
	swerr();
	return (-EFAULT);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_ERROR Handling
 *
 *  -------------------------------------------------------------------------
 *  A hangup from below indicates that a message transfer part has failed badly.  Move the MTP to the out-of-service
 *  state, notify management, and perform normal MTP failure actions.
 */
STATIC int
mtp_r_error(queue_t *q, mblk_t *mp)
{
	struct mt *mt = MTP_PRIV(q);
	(void) mt;
	// mtp_set_i_state(mt, LMI_UNUSABLE);
	fixme(("Complete this function\n"));
	return (-EFAULT);
}
STATIC int
sccp_r_error(queue_t *q, mblk_t *mp)
{
	int err;
	struct sc *sc = SCCP_PRIV(q);
	switch (sccp_get_state(sc)) {
	case SS_DATA_XFER:
	{
		/* 
		   Figure C.2/Q.714 Sheet 4 of 7: Figure 2A/T1.112.4-2000 Sheet 4 of 4: Figure
		   2B/T1.112.4-2000 2of2: This is an internal disconnect.  Provide an N_DISCON_IND
		   to the user, release resources for the connection section, stop inactivity
		   timers, send a RLSD (if possible), start the release and interval timers, move
		   to the disconnect pending state. */
		sccp_release(sc);
		switch (sc->proto.pvar & SS7_PVAR_MASK) {
		case SS7_PVAR_ANSI:
			if ((err =
			     sccp_send_rlsd(q, sc->sp.sp, sc->sp.sp->add.pc, sc->sls, sc->dlr,
					    sc->slr, SCCP_RELC_MTP_FAILURE, NULL, NULL)))
				return (err);
			break;
		default:
		case SS7_PVAR_ITUT:
			if ((err =
			     sccp_send_rlsd(q, sc->sp.sp, sc->sp.sp->add.pc, sc->sls, sc->dlr,
					    sc->slr, SCCP_RELC_MTP_FAILURE, NULL, &sc->imp)))
				return (err);
			break;
		}
		{
			int cause = 0;
			fixme(("Determine appropriate error\n"));
			if ((err = sccp_discon_ind(q, sc, N_PROVIDER, cause, NULL, NULL, NULL)))
				return (err);
		}
		sccp_timer_start(sc, trel);
		switch (sc->proto.pvar & SS7_PVAR_MASK) {
		case SS7_PVAR_ANSI:
			sccp_timer_start(sc, tint);
		}
		sccp_set_state(sc, SS_WCON_DREQ);
		return (QR_DONE);
	}
	case SS_WCON_DREQ:
		/* 
		   Figure 2B/T1.112.4-2000 2of2: release reference for the connection section, stop 
		   release and interval timer, move to the idle state. */
		sccp_release(sc);
		sccp_set_state(sc, SS_IDLE);
		return (QR_DONE);
	default:
		return (QR_PASSALONG);
	}
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_HANGUP Handling
 *
 *  -------------------------------------------------------------------------
 *  A hangup from below indicates that a message transfer part has failed badly.  Move the MTP to the out-of-service
 *  state, notify management, and perform normal MTP failure actions.
 */
STATIC int
mtp_r_hangup(queue_t *q, mblk_t *mp)
{
	struct mt *mt = MTP_PRIV(q);
	(void) mt;
	// mtp_set_i_state(mt, LMI_UNUSABLE);
	fixme(("Complete this function\n"));
	return (-EFAULT);
}
STATIC int
sccp_r_hangup(queue_t *q, mblk_t *mp)
{
	int err;
	struct sc *sc = SCCP_PRIV(q);
	switch (sccp_get_state(sc)) {
	case SS_DATA_XFER:
	{
		/* 
		   Figure C.2/Q.714 Sheet 4 of 7: Figure 2A/T1.112.4-2000 Sheet 4 of 4: Figure
		   2B/T1.112.4-2000 2of2: This is an internal disconnect.  Provide an N_DISCON_IND
		   to the user, release resources for the connection section, stop inactivity
		   timers, send a RLSD (if possible), start the release and interval timers, move
		   to the disconnect pending state. */
		sccp_release(sc);
		switch (sc->proto.pvar & SS7_PVAR_MASK) {
		case SS7_PVAR_ANSI:
			if ((err =
			     sccp_send_rlsd(q, sc->sp.sp, sc->sp.sp->add.pc, sc->sls, sc->dlr,
					    sc->slr, SCCP_RELC_MTP_FAILURE, NULL, NULL)))
				return (err);
			break;
		default:
		case SS7_PVAR_ITUT:
			if ((err =
			     sccp_send_rlsd(q, sc->sp.sp, sc->sp.sp->add.pc, sc->sls, sc->dlr,
					    sc->slr, SCCP_RELC_MTP_FAILURE, NULL, &sc->imp)))
				return (err);
			break;
		}
		{
			int cause = 0;
			fixme(("Determine appropriate error\n"));
			if ((err = sccp_discon_ind(q, sc, N_PROVIDER, cause, NULL, NULL, NULL)))
				return (err);
		}
		sccp_timer_start(sc, trel);
		switch (sc->proto.pvar & SS7_PVAR_MASK) {
		case SS7_PVAR_ANSI:
			sccp_timer_start(sc, tint);
		}
		sccp_set_state(sc, SS_WCON_DREQ);
		return (QR_DONE);
	}
	case SS_WCON_DREQ:
		/* 
		   Figure 2B/T1.112.4-2000 2of2: release reference for the connection section, stop 
		   release and interval timer, move to the idle state. */
		sccp_release(sc);
		sccp_set_state(sc, SS_IDLE);
		return (QR_DONE);
	default:
		return (QR_PASSALONG);
	}
}

/*
 *  =========================================================================
 *
 *  PUT and SRV
 *
 *  =========================================================================
 */
STATIC int
sccp_r_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_RSE:
	case M_PCRSE:
		return sccp_r_rse(q, mp);
	case M_HANGUP:
		return sccp_r_hangup(q, mp);
	case M_ERROR:
		return sccp_r_error(q, mp);
	case M_FLUSH:
		return ss7_r_flush(q, mp);
	}
	return (QR_PASSFLOW);
}
STATIC int
sccp_w_prim(queue_t *q, mblk_t *mp)
{
	/* 
	   Fast Path */
	if (mp->b_datap->db_type == M_DATA)
		return sccp_w_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return sccp_w_data(q, mp);
	case M_RSE:
	case M_PCRSE:
		return sccp_w_rse(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return sccp_w_proto(q, mp);
	case M_FLUSH:
		return ss7_w_flush(q, mp);
	case M_IOCTL:
		return sccp_w_ioctl(q, mp);
	}
	swerr();
	return (-EOPNOTSUPP);
}
STATIC int
tpi_w_prim(queue_t *q, mblk_t *mp)
{
	/* 
	   Fast Path */
	if (mp->b_datap->db_type == M_DATA)
		return tpi_w_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return tpi_w_data(q, mp);
	case M_RSE:
	case M_PCRSE:
		return sccp_w_rse(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return tpi_w_proto(q, mp);
	case M_FLUSH:
		return ss7_w_flush(q, mp);
	case M_IOCTL:
		return sccp_w_ioctl(q, mp);
	}
	swerr();
	return (-EOPNOTSUPP);
}
STATIC int
npi_w_prim(queue_t *q, mblk_t *mp)
{
	/* 
	   Fast Path */
	if (mp->b_datap->db_type == M_DATA)
		return npi_w_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return npi_w_data(q, mp);
	case M_RSE:
	case M_PCRSE:
		return sccp_w_rse(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return npi_w_proto(q, mp);
	case M_FLUSH:
		return ss7_w_flush(q, mp);
	case M_IOCTL:
		return sccp_w_ioctl(q, mp);
	}
	swerr();
	return (-EOPNOTSUPP);
}
STATIC int
gtt_w_prim(queue_t *q, mblk_t *mp)
{
	/* 
	   Fast Path */
	if (mp->b_datap->db_type == M_DATA)
		return gtt_w_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return gtt_w_data(q, mp);
	case M_RSE:
	case M_PCRSE:
		return sccp_w_rse(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return gtt_w_proto(q, mp);
	case M_FLUSH:
		return ss7_w_flush(q, mp);
	case M_IOCTL:
		return sccp_w_ioctl(q, mp);
	}
	swerr();
	return (-EOPNOTSUPP);
}
STATIC int
mgm_w_prim(queue_t *q, mblk_t *mp)
{
	/* 
	   Fast Path */
	if (mp->b_datap->db_type == M_DATA)
		return mgm_w_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return mgm_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return mgm_w_proto(q, mp);
	case M_FLUSH:
		return ss7_w_flush(q, mp);
	case M_IOCTL:
		return sccp_w_ioctl(q, mp);
	}
	swerr();
	return (-EOPNOTSUPP);
}
STATIC int
mtp_r_prim(queue_t *q, mblk_t *mp)
{
	/* 
	   Fast Path */
	if (mp->b_datap->db_type == M_DATA)
		return mtp_r_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return mtp_r_data(q, mp);
	case M_RSE:
	case M_PCRSE:
		return mtp_r_rse(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return mtp_r_proto(q, mp);
	case M_FLUSH:
		return ss7_r_flush(q, mp);
	case M_ERROR:
		return mtp_r_error(q, mp);
	case M_HANGUP:
		return mtp_r_hangup(q, mp);
	}
	return (QR_PASSFLOW);
}
STATIC int
mtp_w_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_RSE:
	case M_PCRSE:
		return mtp_w_rse(q, mp);
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
 *
 *  OPEN
 *  -------------------------------------------------------------------------
 */
STATIC int sccp_majors[SCCP_CMAJORS] = {
	SCCP_CMAJOR_0,
};
STATIC int
sccp_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	psw_t flags;
	int mindex = 0;
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);
	minor_t bminor = cminor;
	struct sc *sc, **scp = &master.sc.list;
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
	if (cmajor != SCCP_CMAJOR_0 || cminor >= SCCP_CMINOR_FREE) {
		MOD_DEC_USE_COUNT;
		return (ENXIO);
	}
	/* allocate a new device */
	cminor = SCCP_CMINOR_FREE;
	spin_lock_irqsave(&master.lock, flags);
	for (; *scp; scp = &(*scp)->next) {
		major_t dmajor = (*scp)->u.dev.cmajor;
		if (cmajor != dmajor)
			break;
		if (cmajor == dmajor) {
			minor_t dminor = (*scp)->u.dev.cminor;
			if (cminor < dminor)
				break;
			if (cminor > dminor)
				continue;
			if (cminor == dminor) {
				if (++cminor >= NMINORS) {
					if (++mindex >= SCCP_CMAJORS
					    || !(cmajor = sccp_majors[mindex]))
						break;
					cminor = 0;
				}
				continue;
			}
		}
	}
	if (mindex >= SCCP_CMAJORS || !cmajor) {
		ptrace(("%s: ERROR: no device numbers available\n", DRV_NAME));
		spin_unlock_irqrestore(&master.lock, flags);
		MOD_DEC_USE_COUNT;
		return (ENXIO);
	}
	printd(("%s: opened character device %d:%d\n", DRV_NAME, cmajor, cminor));
	*devp = makedevice(cmajor, cminor);
	if (!(sc = sccp_alloc_priv(q, scp, devp, crp, bminor))) {
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
sccp_close(queue_t *q, int flag, cred_t *crp)
{
	struct sc *sc = SCCP_PRIV(q);
	psw_t flags;
	(void) flag;
	(void) crp;
	(void) sc;
	printd(("%s: closing character device %d:%d\n", DRV_NAME, sc->u.dev.cmajor,
		sc->u.dev.cminor));
	spin_lock_irqsave(&master.lock, flags);
	sccp_free_priv(sc);
	spin_unlock_irqrestore(&master.lock, flags);
	MOD_DEC_USE_COUNT;
	return (0);
}

/*
 *  =========================================================================
 *
 *  Private Structure allocation, deallocation and cache
 *
 *  =========================================================================
 */
STATIC kmem_cache_t *sccp_sc_cachep = NULL;	/* SCCP User cache */
STATIC kmem_cache_t *sccp_na_cachep = NULL;	/* Network Appearance cache */
STATIC kmem_cache_t *sccp_cp_cachep = NULL;	/* Coupling */
STATIC kmem_cache_t *sccp_sp_cachep = NULL;	/* Signalling Point cache */
STATIC kmem_cache_t *sccp_sr_cachep = NULL;	/* Route Set cache */
STATIC kmem_cache_t *sccp_ss_cachep = NULL;	/* Controlled Rerouting Buffer cache */
STATIC kmem_cache_t *sccp_rs_cachep = NULL;	/* Route List cache */
STATIC kmem_cache_t *sccp_mt_cachep = NULL;	/* Route cache */
STATIC int
sccp_init_caches(void)
{
	if (!sccp_sc_cachep
	    && !(sccp_sc_cachep =
		 kmem_cache_create("sccp_sc_cachep", sizeof(struct sc), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate sccp_sc_cachep", DRV_NAME);
		goto failed_mt;
	} else
		printd(("%s: initialized sc structure cache\n", DRV_NAME));
	if (!sccp_na_cachep
	    && !(sccp_na_cachep =
		 kmem_cache_create("sccp_na_cachep", sizeof(struct na), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate sccp_na_cachep", DRV_NAME);
		goto failed_na;
	} else
		printd(("%s: initialized na structure cache\n", DRV_NAME));
	if (!sccp_cp_cachep
	    && !(sccp_cp_cachep =
		 kmem_cache_create("sccp_cp_cachep", sizeof(struct cp), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate sccp_cp_cachep", DRV_NAME);
		goto failed_cp;
	} else
		printd(("%s: initialized cp structure cache\n", DRV_NAME));
	if (!sccp_sp_cachep
	    && !(sccp_sp_cachep =
		 kmem_cache_create("sccp_sp_cachep", sizeof(struct sp), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate sccp_sp_cachep", DRV_NAME);
		goto failed_sp;
	} else
		printd(("%s: initialized sp structure cache\n", DRV_NAME));
	if (!sccp_sr_cachep
	    && !(sccp_sr_cachep =
		 kmem_cache_create("sccp_sr_cachep", sizeof(struct sr), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate sccp_sr_cachep", DRV_NAME);
		goto failed_rs;
	} else
		printd(("%s: initialized rs structure cache\n", DRV_NAME));
	if (!sccp_ss_cachep
	    && !(sccp_ss_cachep =
		 kmem_cache_create("sccp_ss_cachep", sizeof(struct ss), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate sccp_ss_cachep", DRV_NAME);
		goto failed_cr;
	} else
		printd(("%s: initialized cr structure cache\n", DRV_NAME));
	if (!sccp_rs_cachep
	    && !(sccp_rs_cachep =
		 kmem_cache_create("sccp_rs_cachep", sizeof(struct rs), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate sccp_rs_cachep", DRV_NAME);
		goto failed_rl;
	} else
		printd(("%s: initialized rl structure cache\n", DRV_NAME));
	if (!sccp_mt_cachep
	    && !(sccp_mt_cachep =
		 kmem_cache_create("sccp_mt_cachep", sizeof(struct mt), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate sccp_mt_cachep", DRV_NAME);
		goto failed_rt;
	} else
		printd(("%s: initialized rt structure cache\n", DRV_NAME));
	return (0);
	// failed_sl:
	// kmem_cache_destroy(sccp_mt_cachep);
      failed_rt:
	kmem_cache_destroy(sccp_rs_cachep);
      failed_rl:
	kmem_cache_destroy(sccp_ss_cachep);
      failed_cr:
	kmem_cache_destroy(sccp_sr_cachep);
      failed_rs:
	kmem_cache_destroy(sccp_sp_cachep);
      failed_sp:
	kmem_cache_destroy(sccp_cp_cachep);
      failed_cp:
	kmem_cache_destroy(sccp_na_cachep);
      failed_na:
	kmem_cache_destroy(sccp_sc_cachep);
      failed_mt:
	return (-ENOMEM);
}
STATIC int
sccp_term_caches(void)
{
	int err = 0;
	if (sccp_sc_cachep) {
		if (kmem_cache_destroy(sccp_sc_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy sccp_sc_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed sccp_sc_cachep\n", DRV_NAME));
	}
	if (sccp_na_cachep) {
		if (kmem_cache_destroy(sccp_na_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy sccp_na_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed sccp_na_cachep\n", DRV_NAME));
	}
	if (sccp_sp_cachep) {
		if (kmem_cache_destroy(sccp_sp_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy sccp_sp_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed sccp_sp_cachep\n", DRV_NAME));
	}
	if (sccp_cp_cachep) {
		if (kmem_cache_destroy(sccp_cp_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy sccp_cp_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed sccp_cp_cachep\n", DRV_NAME));
	}
	if (sccp_sr_cachep) {
		if (kmem_cache_destroy(sccp_sr_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy sccp_sr_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed sccp_sr_cachep\n", DRV_NAME));
	}
	if (sccp_ss_cachep) {
		if (kmem_cache_destroy(sccp_ss_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy sccp_ss_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed sccp_ss_cachep\n", DRV_NAME));
	}
	if (sccp_rs_cachep) {
		if (kmem_cache_destroy(sccp_rs_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy sccp_rs_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed sccp_rs_cachep\n", DRV_NAME));
	}
	if (sccp_mt_cachep) {
		if (kmem_cache_destroy(sccp_mt_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy sccp_mt_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed sccp_mt_cachep\n", DRV_NAME));
	}
	return (err);
}

/*
 *  SCCP allocation and deallocation
 *  -------------------------------------------------------------------------
 */
STATIC struct sc *
sccp_alloc_priv(queue_t *q, struct sc **scp, dev_t *devp, cred_t *crp, minor_t bminor)
{
	struct sc *sc;
	if ((sc = kmem_cache_alloc(sccp_sc_cachep, SLAB_ATOMIC))) {
		printd(("%s: %p: allocated sc private structure\n", DRV_NAME, sc));
		bzero(sc, sizeof(*sc));
		sc->priv_put = &sccp_put;
		sc->u.dev.cmajor = getmajor(*devp);
		sc->u.dev.cminor = getminor(*devp);
		sc->cred = *crp;
		(sc->oq = RD(q))->q_ptr = sccp_get(sc);
		(sc->iq = WR(q))->q_ptr = sccp_get(sc);
		spin_lock_init(&sc->qlock);	/* "sc-queue-lock" */
		sc->o_prim = &sccp_r_prim;
		/* 
		   style of interface depends on bminor */
		switch (bminor) {
		case SCCP_STYLE_SCCPI:
			sc->i_prim = &sccp_w_prim;
			sc->i_state = NS_UNBND;
			sc->i_style = bminor;
			sc->i_version = N_CURRENT_VERSION;
			break;
		case SCCP_STYLE_TPI:
			sc->i_prim = &tpi_w_prim;
			sc->i_state = TS_UNBND;
			sc->i_style = bminor;
			sc->i_version = T_CURRENT_VERSION;
			break;
		case SCCP_STYLE_NPI:
			sc->i_prim = &npi_w_prim;
			sc->i_state = NS_UNBND;
			sc->i_style = bminor;
			sc->i_version = N_CURRENT_VERSION;
			break;
		case SCCP_STYLE_GTT:
			sc->i_prim = &gtt_w_prim;
			sc->i_state = NS_UNBND;
			sc->i_style = bminor;
			sc->i_version = N_CURRENT_VERSION;
			break;
		case SCCP_STYLE_MGMT:
			sc->i_prim = &mgm_w_prim;
			sc->i_state = NS_UNBND;
			sc->i_style = bminor;
			sc->i_version = N_CURRENT_VERSION;
			break;
		}
		spin_lock_init(&sc->lock);	/* "sc-queue-lock" */
		if ((sc->next = *scp))
			sc->next->prev = &sc->next;
		sc->prev = scp;
		*scp = sccp_get(sc);
		/* 
		   set defaults */
		printd(("%s: %p: linked sc private structure\n", DRV_NAME, sc));
	} else
		ptrace(("%s: ERROR: Could not allocate sc private structure\n", DRV_NAME));
	return (sc);
}
STATIC void
sccp_free_priv(struct sc *sc)
{
	psw_t flags;
	ensure(sc, return);
	spin_lock_irqsave(&sc->lock, flags);
	{
		ss7_unbufcall((str_t *) sc);
		if (sc->ss.next || sc->ss.prev != &sc->ss.next) {
			/* 
			   disconnect from local subsystem */
			if ((*sc->ss.prev = sc->ss.next))
				sc->ss.next->ss.prev = sc->ss.prev;
			sc->ss.next = NULL;
			sc->ss.prev = &sc->ss.next;
			ss_put(xchg(&sc->ss.ss, NULL));
			sccp_put(sc);
		}
		if (sc->sr.next || sc->sr.prev != &sc->sr.next) {
			/* 
			   disconnect from remote signalling point */
			if ((*sc->sr.prev = sc->sr.next))
				sc->sr.next->sr.prev = sc->sr.prev;
			sc->sr.next = NULL;
			sc->sr.prev = &sc->sr.next;
			sr_put(xchg(&sc->sr.sr, NULL));
			sccp_put(sc);
		}
		if (sc->next || sc->prev != &sc->next) {
			/* 
			   remove from master list */
			if ((*sc->prev = sc->next))
				sc->next->prev = sc->prev;
			sc->next = NULL;
			sc->prev = &sc->next;
			sccp_put(sc);
		}
		sc->oq->q_ptr = NULL;
		flushq(sc->oq, FLUSHALL);
		sc->oq = NULL;
		sccp_put(sc);
		sc->iq->q_ptr = NULL;
		flushq(sc->iq, FLUSHALL);
		sc->iq = NULL;
	}
	spin_unlock_irqrestore(&sc->lock, flags);
	sccp_put(sc);		/* final put */
}
STATIC struct sc *
sccp_lookup(ulong id)
{
	struct sc *sc = NULL;
	if (id)
		for (sc = master.sc.list; sc && sc->id != id; sc = sc->next) ;
	return (sc);
}
STATIC INLINE ulong
sccp_get_id(ulong id)
{
	STATIC ulong sequence = 0;
	if (!id)
		for (id = ++sequence; sccp_lookup(id); id = ++sequence) ;
	return (id);
}
STATIC struct sc *
sccp_get(struct sc *sc)
{
	atomic_inc(&sc->refcnt);
	return (sc);
}
STATIC void
sccp_put(struct sc *sc)
{
	if (atomic_dec_and_test(&sc->refcnt)) {
		kmem_cache_free(sccp_sc_cachep, sc);
		printd(("%s: %s: %p: deallocated sc private structure", DRV_NAME, __FUNCTION__,
			sc));
	}
}

/*
 *  NA allocation and deallocation
 *  -------------------------------------------------------------------------
 */
STATIC struct na *
sccp_alloc_na(ulong id, struct lmi_option *proto)
{
	struct na *na;
	if ((na = kmem_cache_alloc(sccp_na_cachep, SLAB_ATOMIC))) {
		bzero(na, sizeof(*na));
		na->priv_put = &na_put;
		spin_lock_init(&na->lock);	/* "na-lock" */
		na_get(na);	/* first get */
		/* 
		   add to master list */
		if ((na->next = master.na.list))
			na->next->prev = &na->next;
		na->prev = &master.na.list;
		master.na.list = na_get(na);
		master.na.numb++;
		/* 
		   fill out structure */
		na->id = na_get_id(id);
		na->type = MTP_OBJ_TYPE_NA;
		na->proto = *proto;
		/* 
		   populate defaults based on protoocl variant */
		switch ((na->proto.pvar & SS7_PVAR_MASK)) {
		default:
		case SS7_PVAR_ITUT:
			na->config = itut_na_config_default;
			break;
		case SS7_PVAR_ETSI:
			na->config = etsi_na_config_default;
			break;
		case SS7_PVAR_ANSI:
			na->config = ansi_na_config_default;
			break;
		case SS7_PVAR_JTTC:
			na->config = jttc_na_config_default;
			break;
		}
	}
	return (na);
}
STATIC void
sccp_free_na(struct na *na)
{
	psw_t flags;
	ensure(na, return);
	spin_lock_irqsave(&na->lock, flags);
	{
		struct sp *sp;
		/* 
		   remove all attached signalling points */
		while ((sp = na->sp.list))
			sccp_free_sp(sp);
		if (na->next || na->prev != &na->next) {
			/* 
			   remove from master list */
			if ((*na->prev = na->next))
				na->next->prev = na->prev;
			na->next = NULL;
			na->prev = &na->next;
			master.na.numb--;
			na_put(na);
		}
	}
	spin_unlock_irqrestore(&na->lock, flags);
	na_put(na);		/* final put */
	return;
}
STATIC struct na *
na_lookup(ulong id)
{
	struct na *na = NULL;
	if (id)
		for (na = master.na.list; na && na->id != id; na = na->next) ;
	return (na);
}
STATIC ulong
na_get_id(ulong id)
{
	static ulong sequence = 0;
	if (!id)
		id = ++sequence;
	return (id);
}
STATIC struct na *
na_get(struct na *na)
{
	atomic_inc(&na->refcnt);
	return (na);
}
STATIC void
na_put(struct na *na)
{
	if (atomic_dec_and_test(&na->refcnt)) {
		kmem_cache_free(sccp_na_cachep, na);
		printd(("%s: %p: freed na structure\n", DRV_NAME, na));
	}
}

/*
 *  CP allocation and deallocation
 *  -------------------------------------------------------------------------
 */
#if 0
STATIC struct cp *
sccp_alloc_cp(ulong id, struct sp *sp, ulong slr0, ulong slr1)
{
	struct cp *cp;
	if ((cp = kmem_cache_alloc(sccp_cp_cachep, SLAB_ATOMIC))) {
		bzero(cp, sizeof(*cp));
		cp->priv_put = &cp_put;
		spin_lock_init(&cp->lock);	/* "cp-lock" */
		cp_get(cp);	/* first get */
		/* 
		   add to master list */
		if ((cp->next = master.cp.list))
			cp->next->prev = &cp->next;
		cp->prev = &master.cp.list;
		master.cp.list = cp_get(cp);
		master.cp.numb++;
		/* 
		   link to signalling point */
		if ((cp->sp.next = sp->cp.list))
			cp->sp.next->sp.prev = &cp->sp.next;
		cp->sp.prev = &sp->cp.list;
		cp->sp.sp = sp_get(sp);
		sp->cp.list = cp_get(cp);
		sp->cp.numb++;
		/* 
		   fill out structure */
		cp->csec[0].slr = slr0;
		cp->csec[1].slr = slr1;
		fixme(("fill out structure\n"));
		/* 
		   populate defaults */
		fixme(("populate defaults\n"));
	}
	return (cp);
}
#endif
STATIC void
sccp_free_cp(struct cp *cp)
{
	psw_t flags;
	ensure(cp, return);
	spin_lock_irqsave(&cp->lock, flags);
	{
		if (cp->sp.next || cp->sp.prev != &cp->sp.next) {
			/* 
			   remove from sp list */
			if ((*cp->sp.prev = cp->sp.next))
				cp->sp.next->sp.prev = cp->sp.prev;
			cp->sp.next = NULL;
			cp->sp.prev = &cp->sp.next;
			cp->sp.sp->cp.numb--;
			sp_put(xchg(&cp->sp.sp, NULL));
			cp_put(cp);
		}
		if (cp->next || cp->prev != &cp->next) {
			/* 
			   remove from master list */
			if ((*cp->prev = cp->next))
				cp->next->prev = cp->prev;
			cp->next = NULL;
			cp->prev = &cp->next;
			master.cp.numb--;
			cp_put(cp);
		}
	}
	spin_unlock_irqrestore(&cp->lock, flags);
	cp_put(cp);		/* final put */
	return;
}
#if 0
STATIC struct cp *
cp_get(struct cp *cp)
{
	atomic_inc(&cp->refcnt);
	return (cp);
}
#endif
STATIC void
cp_put(struct cp *cp)
{
	if (atomic_dec_and_test(&cp->refcnt)) {
		kmem_cache_free(sccp_cp_cachep, cp);
		printd(("%s: %p: freed cp structure\n", DRV_NAME, cp));
	}
}
STATIC struct cp *
cp_lookup(ulong id)
{
	struct cp *cp = NULL;
	if (id)
		for (cp = master.cp.list; cp && cp->id != id; cp = cp->next) ;
	return (cp);
}
#if 0
STATIC ulong
cp_get_id(ulong id)
{
	static ulong sequence = 0;
	if (!id)
		id = ++sequence;
	return (id);
}
STATIC struct cp *
sccp_lookup_cp(struct sp *sp, ulong slr, const int create)
{
	struct cp *cp;
	(void) create;
	for (cp = sp->cp.list; cp && cp->csec[0].slr != slr && cp->csec[1].slr != slr;
	     cp = cp->sp.next) ;
	return (cp);
}
#endif

/*
 *  SS allocation and deallocation
 *  -------------------------------------------------------------------------
 */
STATIC struct ss *
sccp_alloc_ss(ulong id, struct sp *sp, ulong ssn)
{
	struct ss *ss;
	if ((ss = kmem_cache_alloc(sccp_ss_cachep, SLAB_ATOMIC))) {
		bzero(ss, sizeof(*ss));
		ss->priv_put = &ss_put;
		spin_lock_init(&ss->lock);	/* "ss-lock" */
		ss_get(ss);	/* first get */
		/* 
		   add to master list */
		if ((ss->next = master.ss.list))
			ss->next->prev = &ss->next;
		ss->prev = &master.ss.list;
		master.ss.list = ss_get(ss);
		master.ss.numb++;
		/* 
		   link to signalling point */
		if ((ss->sp.next = sp->ss.list))
			ss->sp.next->sp.prev = &ss->sp.next;
		ss->sp.prev = &sp->ss.list;
		ss->sp.sp = sp_get(sp);
		sp->ss.list = ss_get(ss);
		sp->ss.numb++;
		/* 
		   fill out structure */
		ss->ssn = ssn;
		fixme(("fill out structure\n"));
		/* 
		   populate defaults */
		fixme(("populate defaults\n"));
	}
	return (ss);
}
STATIC void
sccp_free_ss(struct ss *ss)
{
	psw_t flags;
	ensure(ss, return);
	spin_lock_irqsave(&ss->lock, flags);
	{
		struct rs *rs;
		while ((rs = ss->rs.list))
			sccp_free_rs(rs);
		if (ss->sp.next || ss->sp.prev != &ss->sp.next) {
			/* 
			   remove from sp list */
			if ((*ss->sp.prev = ss->sp.next))
				ss->sp.next->sp.prev = ss->sp.prev;
			ss->sp.next = NULL;
			ss->sp.prev = &ss->sp.next;
			ss->sp.sp->ss.numb--;
			sp_put(xchg(&ss->sp.sp, NULL));
			ss_put(ss);
		}
		if (ss->next || ss->prev != &ss->next) {
			/* 
			   remove from master list */
			if ((*ss->prev = ss->next))
				ss->next->prev = ss->prev;
			ss->next = NULL;
			ss->prev = &ss->next;
			master.ss.numb--;
			ss_put(ss);
		}
	}
	spin_unlock_irqrestore(&ss->lock, flags);
	ss_put(ss);		/* final put */
	return;
}
STATIC struct ss *
ss_get(struct ss *ss)
{
	atomic_inc(&ss->refcnt);
	return (ss);
}
STATIC void
ss_put(struct ss *ss)
{
	if (atomic_dec_and_test(&ss->refcnt)) {
		kmem_cache_free(sccp_ss_cachep, ss);
		printd(("%s: %p: freed ss structure\n", DRV_NAME, ss));
	}
}
STATIC struct ss *
ss_lookup(ulong id)
{
	struct ss *ss = NULL;
	if (id)
		for (ss = master.ss.list; ss && ss->id != id; ss = ss->next) ;
	return (ss);
}
STATIC ulong
ss_get_id(ulong id)
{
	static ulong sequence = 0;
	if (!id)
		id = ++sequence;
	return (id);
}
STATIC struct ss *
sccp_lookup_ss(struct sp *sp, ulong ssn, const int create)
{
	struct ss *ss;
	for (ss = sp->ss.list; ss && ss->ssn != ssn; ss = ss->sp.next) ;
	if (!ss && create && !(sp->flags & SCCPF_STRICT_CONFIG))
		ss = sccp_alloc_ss(ss_get_id(0), sp, ssn);
	return (ss);
}

/*
 *  RS allocation and deallocation
 *  -------------------------------------------------------------------------
 */
STATIC struct rs *
sccp_alloc_rs(ulong id, struct sr *sr, ulong ssn)
{
	struct rs *rs;
	if ((rs = kmem_cache_alloc(sccp_rs_cachep, SLAB_ATOMIC))) {
		bzero(rs, sizeof(*rs));
		rs->priv_put = &rs_put;
		spin_lock_init(&rs->lock);	/* "rs-lock" */
		rs_get(rs);	/* first get */
		/* 
		   add to master list */
		if ((rs->next = master.rs.list))
			rs->next->prev = &rs->next;
		rs->prev = &master.rs.list;
		master.rs.list = rs_get(rs);
		master.rs.numb++;
		/* 
		   link to sr */
		if ((rs->sr.next = sr->rs.list))
			rs->sr.next->sr.prev = &rs->sr.next;
		rs->sr.prev = &sr->rs.list;
		rs->sr.sr = sr_get(sr);
		sr->rs.list = rs_get(rs);
		sr->rs.numb++;
#if 0
		/* 
		   link to ss */
		if ((rs->ss.next = ss->rs.list))
			rs->ss.next->ss.prev = &rs->ss.next;
		rs->ss.prev = &ss->rs.list;
		rs->ss.ss = ss_get(ss);
		ss->rs.list = rs_get(rs);
		ss->rs.numb++;
#endif
		/* 
		   fill out structure */
		fixme(("fill out structure\n"));
		rs->ssn = ssn;
		/* 
		   populate defaults */
		fixme(("populate defaults\n"));
	}
	return (rs);
}
STATIC void
sccp_free_rs(struct rs *rs)
{
	psw_t flags;
	ensure(rs, return);
	spin_lock_irqsave(&rs->lock, flags);
	{
		if (rs->sr.next || rs->sr.prev != &rs->sr.next) {
			/* 
			   remove from sr list */
			if ((*rs->sr.prev = rs->sr.next))
				rs->sr.next->sr.prev = rs->sr.prev;
			rs->sr.next = NULL;
			rs->sr.prev = &rs->sr.next;
			rs->sr.sr->rs.numb--;
			sr_put(xchg(&rs->sr.sr, NULL));
			rs_put(rs);
		}
		if (rs->ss.next || rs->ss.prev != &rs->ss.next) {
			/* 
			   remove from ss list */
			if ((*rs->ss.prev = rs->ss.next))
				rs->ss.next->ss.prev = rs->ss.prev;
			rs->ss.next = NULL;
			rs->ss.prev = &rs->ss.next;
			rs->ss.ss->rs.numb--;
			ss_put(xchg(&rs->ss.ss, NULL));
			rs_put(rs);
		}
		if (rs->next || rs->prev != &rs->next) {
			/* 
			   remove from master list */
			if ((*rs->prev = rs->next))
				rs->next->prev = rs->prev;
			rs->next = NULL;
			rs->prev = &rs->next;
			master.rs.numb--;
			rs_put(rs);
		}
	}
	spin_unlock_irqrestore(&rs->lock, flags);
	rs_put(rs);		/* final put */
	return;
}
STATIC struct rs *
rs_get(struct rs *rs)
{
	atomic_inc(&rs->refcnt);
	return (rs);
}
STATIC void
rs_put(struct rs *rs)
{
	if (atomic_dec_and_test(&rs->refcnt)) {
		kmem_cache_free(sccp_rs_cachep, rs);
		printd(("%s: %p: freed rs structure\n", DRV_NAME, rs));
	}
}
STATIC struct rs *
rs_lookup(ulong id)
{
	struct rs *rs = NULL;
	if (id)
		for (rs = master.rs.list; rs && rs->id != id; rs = rs->next) ;
	return (rs);
}
STATIC ulong
rs_get_id(ulong id)
{
	static ulong sequence = 0;
	if (!id)
		id = ++sequence;
	return (id);
}
STATIC struct rs *
sccp_lookup_rs(struct sr *sr, ulong ssn, const int create)
{
	struct rs *rs;
	for (rs = sr->rs.list; rs && rs->ssn != ssn; rs = rs->sr.next) ;
	if (!rs && create && !(sr->flags & SCCPF_STRICT_ROUTING))
		rs = sccp_alloc_rs(rs_get_id(0), sr, ssn);
	return (rs);
}

/*
 *  SR allocation and deallocation
 *  -------------------------------------------------------------------------
 */
STATIC struct sr *
sccp_alloc_sr(ulong id, struct sp *sp, ulong pc)
{
	struct sr *sr, **srp;
	printd(("%s: %s: create sr->id = %ld\n", DRV_NAME, __FUNCTION__, id));
	if ((sr = kmem_cache_alloc(sccp_sr_cachep, SLAB_ATOMIC))) {
		bzero(sr, sizeof(*sr));
		sr_get(sr);	/* first get */
		spin_lock_init(&sr->lock);	/* "sr-lock" */
		sr->id = id;
		/* 
		   add to master list */
		for (srp = &master.sr.list; *srp && (*srp)->id < id; srp = &(*srp)->next) ;
		if ((sr->next = *srp))
			sr->next->prev = &sr->next;
		sr->prev = srp;
		*srp = sr_get(sr);
		master.sr.numb++;
		/* 
		   add to signalling point list of signalling relations */
		sr->sp.sp = sp_get(sp);
		if ((sr->sp.next = sp->sr.list))
			sr->sp.next->sp.prev = &sr->sp.next;
		sr->sp.prev = &sp->sr.list;
		sp->sr.list = sr_get(sr);
		sp->sr.numb++;
		sr->add = sp->add;
		sr->add.pc = pc;
	} else
		printd(("%s: %s: ERROR: failed to allocate sr structure %lu\n", DRV_NAME,
			__FUNCTION__, id));
	return (sr);
}
STATIC void
sccp_free_sr(struct sr *sr)
{
	psw_t flags;
	ensure(sr, return);
	printd(("%s: %s: %p free sr->id = %ld\n", DRV_NAME, __FUNCTION__, sr, sr->id));
	spin_lock_irqsave(&sr->lock, flags);
	{
		struct rs *rs;
		struct sc *sc;
		/* 
		   stop all timers */
		__sr_timer_stop(sr, tall);
		/* 
		   remove all remote subsystems */
		while ((rs = sr->rs.list))
			sccp_free_rs(rs);
		/* 
		   release all connected SCCP users */
		while ((sc = sr->sc.list))
			sccp_release(sc);
		/* 
		   unlink from message transfer part */
		if (sr->mt) {
			sr_put(xchg(&sr->mt->sr, NULL));
			mtp_put(xchg(&sr->mt, NULL));
		}
		/* 
		   remove from signalling point list */
		if (sr->sp.sp) {
			if ((*sr->sp.prev = sr->sp.next))
				sr->sp.next->sp.prev = sr->sp.prev;
			sr->sp.next = NULL;
			sr->sp.prev = &sr->sp.next;
			sp_put(xchg(&sr->sp.sp, NULL));
			ensure(atomic_read(&sr->refcnt) > 1, sr_get(sr));
			sr_put(sr);
		}
		/* 
		   remove from master list */
		if ((*sr->prev = sr->next))
			sr->next->prev = sr->prev;
		sr->next = NULL;
		sr->prev = &sr->next;
		ensure(atomic_read(&sr->refcnt) > 1, sr_get(sr));
		sr_put(sr);
		assure(master.sr.numb > 0);
		master.sr.numb--;
		/* 
		   done, check final count */
		if (atomic_read(&sr->refcnt) != 1) {
			__printd(("%s: %s: %p: ERROR: sr lingering reference count = %d\n",
				  DRV_NAME, __FUNCTION__, sr, atomic_read(&sr->refcnt)));
			atomic_set(&sr->refcnt, 1);
		}
	}
	spin_unlock_irqrestore(&sr->lock, flags);
	sr_put(sr);		/* final put */
}
STATIC struct sr *
sr_get(struct sr *sr)
{
	assure(sr);
	if (sr)
		atomic_inc(&sr->refcnt);
	return (sr);
}
STATIC void
sr_put(struct sr *sr)
{
	assure(sr);
	if (sr) {
		assure(atomic_read(&sr->refcnt) > 1);
		if (atomic_dec_and_test(&sr->refcnt)) {
			kmem_cache_free(sccp_sr_cachep, sr);
			printd(("%s: %s: %p: deallocated sr structure\n", DRV_NAME, __FUNCTION__,
				sr));
		}
	}
}
STATIC struct sr *
sr_lookup(ulong id)
{
	struct sr *sr;
	for (sr = master.sr.list; sr && sr->id != id; sr = sr->next) ;
	return (sr);
}
STATIC ulong
sr_get_id(ulong id)
{
	struct sr *sr;
	if (!id) {
		id = 1;
		for (sr = master.sr.list; sr; sr = sr->next)
			if (sr->id == id)
				id++;
			else if (sr->id > id)
				break;
	}
	return (id);
}
STATIC struct sr *
sccp_lookup_sr(struct sp *sp, ulong pc, const int create)
{
	struct sr *sr;
	for (sr = sp->sr.list; sr && sr->add.pc != pc; sr = sr->sp.next) ;
	if (!sr && create && !(sp->flags & SCCPF_STRICT_ROUTING))
		sr = sccp_alloc_sr(sr_get_id(0), sp, pc);
	return (sr);
}

/*
 *  SP allocation and deallocation
 *  -------------------------------------------------------------------------
 */
STATIC struct sp *
sccp_alloc_sp(ulong id, struct na *na, mtp_addr_t * add)
{
	struct sp *sp, **spp;
	printd(("%s: %s: create sp->id = %ld\n", DRV_NAME, __FUNCTION__, id));
	if ((sp = kmem_cache_alloc(sccp_sp_cachep, SLAB_ATOMIC))) {
		bzero(sp, sizeof(*sp));
		sp_get(sp);	/* first get */
		spin_lock_init(&sp->lock);	/* "sp-lock" */
		sp->id = id;
		for (spp = &master.sp.list; *spp && (*spp)->id < id; spp = &(*spp)->next) ;
		if ((sp->next = *spp))
			sp->next->prev = &sp->next;
		sp->prev = spp;
		*spp = sp_get(sp);
		master.sp.numb++;
		/* 
		   add to network appearance list of signalling points */
		sp->na.na = na_get(na);
		if ((sp->na.next = na->sp.list))
			sp->na.next->na.prev = &sp->na.next;
		sp->na.prev = &na->sp.list;
		na->sp.list = sp_get(sp);
		na->sp.numb++;
		sp->add = *add;
	} else
		printd(("%s: %s: ERROR: failed to allocate sp structure %lu\n", DRV_NAME,
			__FUNCTION__, id));
	return (sp);
}
STATIC void
sccp_free_sp(struct sp *sp)
{
	psw_t flags;
	ensure(sp, return);
	printd(("%s: %s: %p free sp->id = %ld\n", DRV_NAME, __FUNCTION__, sp, sp->id));
	spin_lock_irqsave(&sp->lock, flags);
	{
		__sp_timer_stop(sp, tall);
		/* 
		   freeing all signalling relations */
		while (sp->sr.list)
			sccp_free_sr(sp->sr.list);
		/* 
		   unlink from message transfer part */
		if (sp->mt) {
			sp_put(xchg(&sp->mt->sp, NULL));
			mtp_put(xchg(&sp->mt, NULL));
		}
		/* 
		   remote from network appearance list */
		if (sp->na.na) {
			if ((*sp->na.prev = sp->na.next))
				sp->na.next->na.prev = sp->na.prev;
			sp->na.next = NULL;
			sp->na.prev = &sp->na.next;
			na_put(xchg(&sp->na.na, NULL));
			ensure(atomic_read(&sp->refcnt) < 1, sp_get(sp));
			sp_put(sp);
		}
		/* 
		   remove from master list */
		if ((*sp->prev = sp->next))
			sp->next->prev = sp->prev;
		sp->next = NULL;
		sp->prev = &sp->next;
		ensure(atomic_read(&sp->refcnt) > 1, sp_get(sp));
		sp_put(sp);
		assure(master.sp.numb > 0);
		master.sp.numb--;
		/* 
		   done, check final count */
		if (atomic_read(&sp->refcnt) != 1) {
			__printd(("%s: %s: %p: ERROR: sp lingering reference count = %d\n",
				  DRV_NAME, __FUNCTION__, sp, atomic_read(&sp->refcnt)));
			atomic_set(&sp->refcnt, 1);
		}
	}
	spin_unlock_irqrestore(&sp->lock, flags);
	sp_put(sp);		/* final put */
}
STATIC struct sp *
sp_get(struct sp *sp)
{
	assure(sp);
	if (sp)
		atomic_inc(&sp->refcnt);
	return (sp);
}
STATIC void
sp_put(struct sp *sp)
{
	assure(sp);
	if (sp) {
		assure(atomic_read(&sp->refcnt) > 1);
		if (atomic_dec_and_test(&sp->refcnt)) {
			kmem_cache_free(sccp_sp_cachep, sp);
			printd(("%s: %s: %p: deallocated sp structure\n", DRV_NAME, __FUNCTION__,
				sp));
		}
	}
}
STATIC struct sp *
sp_lookup(ulong id)
{
	struct sp *sp;
	for (sp = master.sp.list; sp && sp->id != id; sp = sp->next) ;
	return (sp);
}
STATIC ulong
sp_get_id(ulong id)
{
	struct sp *sp;
	if (!id) {
		id = 1;
		for (sp = master.sp.list; sp; sp = sp->next)
			if (sp->id == id)
				id++;
			else if (sp->id > id)
				break;
	}
	return (id);
}
STATIC struct sp *
sccp_lookup_sp(ulong ni, ulong pc, const int create)
{
	struct na *na;
	struct sp *sp = NULL;
	for (na = master.na.list; na && na->id != ni; na = na->next) ;
	if (na) {
		for (sp = na->sp.list; sp && sp->add.pc != pc; sp = sp->next) ;
		if (!sp && create && !(na->flags & SCCPF_STRICT_CONFIG)) {
			struct mtp_addr add;
			add.family = AF_MTP;
			add.ni = ni;
			add.si = 3;
			add.pc = pc;
			sp = sccp_alloc_sp(sp_get_id(0), na, &add);
		}
	}
	return (sp);
}

/*
 *  MT allocation and deallocation
 *  -------------------------------------------------------------------------
 */
STATIC struct mt *
sccp_alloc_link(queue_t *q, struct mt **mpp, ulong index, cred_t *crp)
{
	struct mt *mt;
	printd(("%s: %s: create mt index = %lu\n", DRV_NAME, __FUNCTION__, index));
	if ((mt = kmem_cache_alloc(sccp_mt_cachep, SLAB_ATOMIC))) {
		bzero(mt, sizeof(*mt));
		mtp_get(mt);	/* first get */
		mt->u.mux.index = index;
		mt->cred = *crp;
		spin_lock_init(&mt->qlock);	/* "mt-queue-lock" */
		(mt->iq = RD(q))->q_ptr = mtp_get(mt);
		(mt->oq = WR(q))->q_ptr = mtp_get(mt);
		mt->o_prim = mtp_w_prim;
		mt->i_prim = mtp_r_prim;
		mt->o_wakeup = NULL;
		mt->i_wakeup = NULL;
		mt->i_state = LMI_UNUSABLE;
		mt->i_style = LMI_STYLE1;
		mt->i_version = 1;
		spin_lock_init(&mt->lock);	/* "mt-lock" */
		/* 
		   place in master list */
		if ((mt->next = *mpp))
			mt->next->prev = &mt->next;
		mt->prev = mpp;
		*mpp = mtp_get(mt);
		master.mt.numb++;
	} else
		printd(("%s: %s: ERROR: failed to allocate mt structure %lu\n", DRV_NAME,
			__FUNCTION__, index));
	return (mt);
}
STATIC void
sccp_free_link(struct mt *mt)
{
	psw_t flags;
	ensure(mt, return);
	printd(("%s: %s: %p free mt index = %lu\n", DRV_NAME, __FUNCTION__, mt, mt->u.mux.index));
	spin_lock_irqsave(&mt->lock, flags);
	{
		/* 
		   flushing buffers */
		ss7_unbufcall((str_t *) mt);
		flushq(mt->iq, FLUSHALL);
		flushq(mt->oq, FLUSHALL);
		/* 
		   unlink from signalling relation */
		if (mt->sr) {
			struct sr *sr = mt->sr;
			(void) sr;
#if 0
			if (!(sr->flags & CCTF_LOC_S_BLOCKED)) {
				/* 
				   software block all circuits */
				struct ct *ct;
				for (ct = sr->ct.list; ct; ct = ct->sr.next) {
					ct_set(ct, CCTF_LOC_S_BLOCKED);
				}
				sr->flags |= CCTF_LOC_S_BLOCKED;
			}
#endif
			mtp_put(xchg(&mt->sr->mt, NULL));
			sr_put(xchg(&mt->sr, NULL));
		}
		/* 
		   unlink from signalling point */
		if (mt->sp) {
			struct sr *sr;
			for (sr = mt->sp->sr.list; sr; sr = sr->sp.next) {
#if 0
				if (!(sr->flags & CCTF_LOC_S_BLOCKED)) {
					/* 
					   software block all circuits */
					struct ct *ct;
					for (ct = sr->ct.list; ct; ct = ct->sr.next) {
						ct_set(ct, CCTF_LOC_S_BLOCKED);
					}
					sr->flags |= CCTF_LOC_S_BLOCKED;
				}
#endif
			}
			mtp_put(xchg(&mt->sp->mt, NULL));
			sp_put(xchg(&mt->sp, NULL));
		}
		/* 
		   remote from master list */
		if ((*mt->prev = mt->next))
			mt->next->prev = mt->prev;
		mt->next = NULL;
		mt->prev = &mt->next;
		ensure(atomic_read(&mt->refcnt) > 1, mtp_get(mt));
		mtp_put(mt);
		assure(master.mt.numb > 0);
		master.mt.numb--;
		/* 
		   remove from queues */
		ensure(atomic_read(&mt->refcnt) > 1, mtp_get(mt));
		mtp_put(xchg(&mt->iq->q_ptr, NULL));
		ensure(atomic_read(&mt->refcnt) > 1, mtp_get(mt));
		mtp_put(xchg(&mt->oq->q_ptr, NULL));
		/* 
		   done, check final count */
		if (atomic_read(&mt->refcnt) != 1) {
			__printd(("%s: %s: %p: ERROR: mt lingering reference count = %d\n",
				  DRV_NAME, __FUNCTION__, mt, atomic_read(&mt->refcnt)));
			atomic_set(&mt->refcnt, 1);
		}
	}
	spin_unlock_irqrestore(&mt->lock, flags);
	mtp_put(mt);		/* final put */
}
STATIC struct mt *
mtp_get(struct mt *mt)
{
	assure(mt);
	if (mt)
		atomic_inc(&mt->refcnt);
	return (mt);
}
STATIC void
mtp_put(struct mt *mt)
{
	assure(mt);
	if (mt) {
		assure(atomic_read(&mt->refcnt) > 1);
		if (atomic_dec_and_test(&mt->refcnt)) {
			kmem_cache_free(sccp_mt_cachep, mt);
			printd(("%s: %s: %p: deallocated mt structure\n", DRV_NAME, __FUNCTION__,
				mt));
		}
	}
}
STATIC struct mt *
mtp_lookup(ulong id)
{
	struct mt *mt;
	for (mt = master.mt.list; mt && mt->id != id; mt = mt->next) ;
	return (mt);
}
STATIC ulong
mtp_get_id(ulong id)
{
	static ulong identifier = 0;
	if (!id) {
		id = ++identifier;
	}
	return (id);
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
#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0);
#endif
MODULE_PARM_DESC(modid, "Module ID for the SCCP driver. (0 for allocation.)");

major_t major = CMAJOR_0;
#ifndef module_param
MODULE_PARM(major, "h");
#else
module_param(major, uint, 0);
#endif
MODULE_PARM_DESC(major, "Device number for the SCCP driver. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LFS

STATIC struct cdevsw sccp_cdev = {
	.d_name = DRV_NAME,
	.d_str = &sccpinfo,
	.d_flag = 0,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

STATIC int
sccp_register_strdev(major_t major)
{
	int err;
	if ((err = register_strdev(&sccp_cdev, major)) < 0)
		return (err);
	return (0);
}

STATIC int
sccp_unregister_strdev(major_t major)
{
	int err;
	if ((err = unregister_strdev(&sccp_cdev, major)) < 0)
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
sccp_register_strdev(major_t major)
{
	int err;
	if ((err = lis_register_strdev(major, &sccpinfo, UNITS, DRV_NAME)) < 0)
		return (err);
	return (0);
}

STATIC int
sccp_unregister_strdev(major_t major)
{
	int err;
	if ((err = lis_unregister_strdev(major)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

MODULE_STATIC void __exit
sccpterminate(void)
{
	int err, mindex;
	for (mindex = CMAJORS - 1; mindex >= 0; mindex--) {
		if (sccp_majors[mindex]) {
			if ((err = sccp_unregister_strdev(sccp_majors[mindex])))
				cmn_err(CE_PANIC, "%s: cannot unregister major %d", DRV_NAME,
					sccp_majors[mindex]);
			if (mindex)
				sccp_majors[mindex] = 0;
		}
	}
	if ((err = sccp_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", DRV_NAME);
	return;
}

MODULE_STATIC int __init
sccpinit(void)
{
	int err, mindex = 0;
	cmn_err(CE_NOTE, DRV_BANNER);	/* console splash */
	if ((err = sccp_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", DRV_NAME, err);
		sccpterminate();
		return (err);
	}
	for (mindex = 0; mindex < CMAJORS; mindex++) {
		if ((err = sccp_register_strdev(sccp_majors[mindex])) < 0) {
			if (mindex) {
				cmn_err(CE_WARN, "%s: could not register major %d", DRV_NAME,
					sccp_majors[mindex]);
				continue;
			} else {
				cmn_err(CE_WARN, "%s: could not register driver, err = %d",
					DRV_NAME, err);
				sccpterminate();
				return (err);
			}
		}
		if (sccp_majors[mindex] == 0)
			sccp_majors[mindex] = err;
#if 0
		LIS_DEVFLAGS(sccp_majors[mindex]) |= LIS_MODFLG_CLONE;
#endif
		if (major == 0)
			major = sccp_majors[0];
	}
	return (0);
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(sccpinit);
module_exit(sccpterminate);

#endif				/* LINUX */
