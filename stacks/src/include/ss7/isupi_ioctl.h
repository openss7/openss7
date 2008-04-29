/*****************************************************************************

 @(#) $Id: isupi_ioctl.h,v 0.9.2.8 2008-04-29 07:10:43 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation; version 3 of the License.

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

 Last Modified $Date: 2008-04-29 07:10:43 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: isupi_ioctl.h,v $
 Revision 0.9.2.8  2008-04-29 07:10:43  brian
 - updating headers for release

 Revision 0.9.2.7  2007/08/14 12:17:09  brian
 - GPLv3 header updates

 Revision 0.9.2.6  2006/12/20 23:12:51  brian
 - cosmetic changes

 Revision 0.9.2.5  2006/10/31 21:04:37  brian
 - changes for 32-bit compatibility and remove HZ dependency

 *****************************************************************************/

#ifndef __ISUPI_IOCTL_H__
#define __ISUPI_IOCTL_H__

#ident "@(#) $RCSfile: isupi_ioctl.h,v $ $Name:  $($Revision: 0.9.2.8 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

/* This file can be processed by doxygen(1). */

#define ISUP_IOC_MAGIC 'i'

#define ISUP_OBJ_TYPE_DF	0	/* default */
#define ISUP_OBJ_TYPE_CT	1	/* circuit */
#define ISUP_OBJ_TYPE_CG	2	/* circuit group */
#define ISUP_OBJ_TYPE_TG	3	/* trunk group */
#define ISUP_OBJ_TYPE_SR	4	/* signalling relation */
#define ISUP_OBJ_TYPE_SP	5	/* signalling point */
#define ISUP_OBJ_TYPE_MT	6	/* message transfer part */

/*
 *  Circuit configuration
 *  -----------------------------------
 */
typedef struct isup_conf_ct {
	cc_ulong cgid;			/* circuit group identifier */
	cc_ulong tgid;			/* trunk group identifier */
	cc_ulong cic;			/* circuit identification code */
} isup_conf_ct_t;

typedef struct isup_opt_conf_ct {
} isup_opt_conf_ct_t;

/*
 *  Circuit Group configuration
 *  -----------------------------------
 */
typedef struct isup_conf_cg {
	cc_ulong srid;			/* remote signalling point identifier */
	lmi_option_t proto;		/* protocol variant and options */
} isup_conf_cg_t;

typedef struct isup_opt_conf_cg {
} isup_opt_conf_cg_t;

/*
 *  Trunk Group configuration
 *  -----------------------------------
 */
typedef struct isup_conf_tg {
	cc_ulong srid;			/* remote signalling point identifier */
	lmi_option_t proto;		/* protocol variant and options */
} isup_conf_tg_t;

typedef struct isup_opt_conf_tg {
	cc_ulong flags;			/* configuration flags */
	cc_ulong type;			/* trunk group type */
	cc_ulong exchange_type;		/* exchange type */
	cc_ulong select_type;		/* circuit selection type */
	cc_ulong t1;			/* waiting for RLC retry */
	cc_ulong t2;			/* waiting for RES */
	cc_ulong t3;			/* waiting OVL timeout */
	cc_ulong t5;			/* waiting for RLC maintenance */
	cc_ulong t6;			/* waiting for RES */
	cc_ulong t7;			/* waiting for ACM/ANM/CON */
	cc_ulong t8;			/* waiting for COT */
	cc_ulong t9;			/* waiting for ANM/CON */
	cc_ulong t10;			/* waiting more information. Interworking */
	cc_ulong t11;			/* waiting for ACM, Interworking */
	cc_ulong t12;			/* waiting for BLA retry */
	cc_ulong t13;			/* waiting for BLA maintenance */
	cc_ulong t14;			/* waiting for UBA retry */
	cc_ulong t15;			/* waiting for UBA maintenance */
	cc_ulong t16;			/* waiting for RLC retry */
	cc_ulong t17;			/* waiting for RLC maintenance */
	cc_ulong t24;			/* waiting for continuity IAM */
	cc_ulong t25;			/* waiting for continuity CCR retry */
	cc_ulong t26;			/* waiting for continuity CCR maintenance */
	cc_ulong t27;			/* waiting for COT reset */
	cc_ulong t31;			/* call reference guard */
	cc_ulong t32;			/* waiting to send E2E message */
	cc_ulong t33;			/* waiting for INF */
	cc_ulong t34;			/* waiting for SEG */
	cc_ulong t35;			/* waiting more information. */
	cc_ulong t36;			/* waiting for COT/REL */
	cc_ulong t37;			/* waiting echo control device */
	cc_ulong t38;			/* waiting for RES */
	cc_ulong tacc_r;		/* */
	cc_ulong tccr;			/* */
	cc_ulong tccr_r;		/* */
	cc_ulong tcra;			/* */
	cc_ulong tcrm;			/* */
	cc_ulong tcvt;			/* */
	cc_ulong texm_d;		/* */
} isup_opt_conf_tg_t;

#define ISUP_XCHG_TYPE_A	0UL
#define ISUP_XCHG_TYPE_B	1UL

#define ISUP_TGF_GLARE_PRIORITY				0x00000001UL
#define ISUP_TGF_INCOMING_INTERNATIONAL_EXCHANGE	0x00000002UL
#define ISUP_TGF_SUSPEND_NATIONALLY_PERFORMED		0x00000004UL
#define ISUP_TGF_CONTROLLING_EXCHANGE			0x00000008UL

#define ISUP_TG_TYPE_OG			1	/* outgoing */
#define ISUP_TG_TYPE_IC			2	/* incoming */
#define ISUP_TG_TYPE_2W			3	/* two way */

#define ISUP_SELECTION_TYPE_MRU		0	/* most recently used */
#define ISUP_SELECTION_TYPE_LRU		1	/* least recently used */
#define ISUP_SELECTION_TYPE_ASEQ	2	/* ascending sequential */
#define ISUP_SELECTION_TYPE_DSEQ	3	/* decending sequential */

/*
 *  Remote Signalling Point configuration
 *  -----------------------------------
 */
typedef struct isup_conf_sr {
	cc_ulong spid;			/* local signalling point identifier */
	struct mtp_addr add;		/* remote signalling point code */
	lmi_option_t proto;		/* protocol variant and options */
} isup_conf_sr_t;

typedef struct isup_opt_conf_sr {
	cc_ulong t4;			/* waiting for UPA/other */
	cc_ulong t18;			/* waiting CGBA retry */
	cc_ulong t19;			/* waiting CGBA maintenance */
	cc_ulong t20;			/* waiting CGUA retry */
	cc_ulong t21;			/* waiting CGUA maintenance */
	cc_ulong t22;			/* waiting GRA retry */
	cc_ulong t23;			/* waiting GRA maintenance */
	cc_ulong t28;			/* waiting CQR */
	cc_ulong t29;			/* congestion attack timer */
	cc_ulong t30;			/* congestion decay timer */
	cc_ulong tcgb;			/* */
	cc_ulong tgrs;			/* */
	cc_ulong thga;			/* */
	cc_ulong tscga;			/* */
	cc_ulong tscga_d;		/* */
} isup_opt_conf_sr_t;

/*
 *  Signalling Point configuration
 *  -----------------------------------
 */
typedef struct isup_conf_sp {
	struct mtp_addr add;		/* local signalling point code */
	lmi_option_t proto;		/* protocol variant and options */
} isup_conf_sp_t;

typedef struct isup_opt_conf_sp {
} isup_opt_conf_sp_t;

/*
 *  Message Transfer Part configuration
 *  -----------------------------------
 */
typedef struct isup_conf_mtp {
	cc_ulong spid;			/* local signalling point identifier */
	cc_ulong srid;			/* remote signalling point identifier */
	cc_ulong muxid;			/* lower multipleding driver id */
	lmi_option_t proto;		/* protocol variant and options */
} isup_conf_mtp_t;

typedef struct isup_opt_conf_mtp {
} isup_opt_conf_mtp_t;

/*
 *  Default configuration
 *  -----------------------------------
 */
typedef struct isup_conf_df {
	lmi_option_t proto;		/* protocol variant and options */
} isup_conf_df_t;

typedef struct isup_opt_conf_df {
} isup_opt_conf_df_t;

enum {
	SS7_POPT_UPT = 0x00000100,	/* send UPT messages */
	SS7_POPT_LPA = 0x00000200,	/* send LPA messages */
	SS7_POPT_UCIC = 0x00000400,	/* send UCIC messages */
};

/*
 *  OPTIONS
 */
typedef struct isup_option {
	cc_ulong type;			/* object type */
	cc_ulong id;			/* object id */
	/* followed by specific proto structure */
} isup_option_t;

#define ISUP_IOCGOPTIONS	_IOR(	ISUP_IOC_MAGIC,  0,  isup_option_t   )
#define ISUP_IOCSOPTIONS	_IOW(	ISUP_IOC_MAGIC,  1,  isup_option_t   )

/*
 *  CONFIGURATION
 */
typedef struct isup_config {
	cc_ulong type;			/* object type */
	cc_ulong id;			/* object id */
	cc_ulong cmd;			/* object command */
	/* followed by specific configuration structure */
} isup_config_t;

#define ISUP_GET	0
#define ISUP_ADD	1
#define ISUP_CHA	2
#define ISUP_DEL	3

#define ISUP_IOCGCONFIG		_IOWR(	ISUP_IOC_MAGIC,  2,  isup_config_t )
#define ISUP_IOCSCONFIG		_IOWR(	ISUP_IOC_MAGIC,  3,  isup_config_t )
#define ISUP_IOCTCONFIG		_IOWR(	ISUP_IOC_MAGIC,  4,  isup_config_t )
#define ISUP_IOCCCONFIG		_IOWR(	ISUP_IOC_MAGIC,  5,  isup_config_t )

/*
 *  STATE
 */
typedef struct isup_timers_ct {
	/* Call Control Timers */
	cc_ulong t1;
	cc_ulong t2;
	cc_ulong t3;
	cc_ulong t5;
	cc_ulong t6;
	cc_ulong t7;
	cc_ulong t8;
	cc_ulong t9;
	cc_ulong t10;
	cc_ulong t11;
	cc_ulong t24;
	cc_ulong t31;
	cc_ulong t32;
	cc_ulong t33;
	cc_ulong t34;
	cc_ulong t35;
	cc_ulong t38;
	cc_ulong tacc_r;
	cc_ulong tcra;
	cc_ulong tcrm;
	cc_ulong texm_d;
	/* Circuit Timers */
	cc_ulong t12;
	cc_ulong t13;
	cc_ulong t14;
	cc_ulong t15;
	cc_ulong t16;
	cc_ulong t17;
	cc_ulong t25;
	cc_ulong t26;
	cc_ulong t27;
	cc_ulong t36;
	cc_ulong t37;
	cc_ulong tccr;
	cc_ulong tccr_r;
	cc_ulong tcvt;
} isup_timers_ct_t;

typedef struct isup_statem_ct {
	isup_timers_ct_t timers;
	cc_ulong c_state;
	cc_ulong i_state;
	cc_ulong m_state;
	cc_ulong tst_actv;
	cc_ulong mgm_actv;
	cc_ulong cpc_actv;
	cc_ulong tst_bind;
	cc_ulong mgm_bind;
	cc_ulong mnt_bind;
	cc_ulong icc_bind;
	cc_ulong ogc_bind[0];
	/* followed by bound streams for outgoing calls */
} isup_statem_ct_t;

typedef struct isup_timers_cg {
	/* Circuit Group Timers */
	cc_ulong t18;
	cc_ulong t19;
	cc_ulong t20;
	cc_ulong t21;
	cc_ulong t22;
	cc_ulong t23;
	cc_ulong t28;
	cc_ulong tcgb;
	cc_ulong tgrs;
	cc_ulong thga;
	cc_ulong tscga;
	cc_ulong tscga_d;
} isup_timers_cg_t;

typedef struct isup_statem_cg {
	isup_timers_cg_t timers;
	cc_ulong g_state;
	cc_ulong gmg_actv;
	cc_ulong tst_bind;
	cc_ulong mgm_bind;
	cc_ulong mnt_bind;
	cc_ulong icc_bind;
	cc_ulong ogc_bind[0];
	/* followed by bound streams for outgoing calls */
} isup_statem_cg_t;

typedef struct isup_timers_tg {
} isup_timers_tg_t;

typedef struct isup_statem_tg {
	isup_timers_tg_t timers;
	cc_ulong tst_bind;
	cc_ulong mgm_bind;
	cc_ulong mnt_bind;
	cc_ulong icc_bind;
	cc_ulong ogc_bind[0];
	/* followed by bound streams for outgoing calls */
} isup_statem_tg_t;

typedef struct isup_timers_sr {
	cc_ulong t4;			/* UPT timer */
	cc_ulong t29;			/* signalling congestion attack timer */
	cc_ulong t30;			/* signalling congestion decay timer */
} isup_timers_sr_t;

typedef struct isup_statem_sr {
	isup_timers_sr_t timers;
	cc_ulong cong_level;
	cc_ulong tst_bind;
	cc_ulong mgm_bind;
	cc_ulong mnt_bind;
	cc_ulong icc_bind;
	cc_ulong ogc_bind[0];
	/* followed by bound streams for outgoing calls */
} isup_statem_sr_t;

typedef struct isup_timers_sp {
} isup_timers_sp_t;

typedef struct isup_statem_sp {
	isup_timers_sp_t timers;
	cc_ulong tst_bind;
	cc_ulong mgm_bind;
	cc_ulong mnt_bind;
	cc_ulong icc_bind;
	cc_ulong ogc_bind[0];
	/* followed by bound streams for outgoing calls */
} isup_statem_sp_t;

typedef struct isup_timers_mtp {
} isup_timers_mtp_t;

typedef struct isup_statem_mtp {
	isup_timers_mtp_t timers;
} isup_statem_mtp_t;

typedef struct isup_timers_df {
} isup_timers_df_t;

typedef struct isup_statem_df {
	isup_timers_df_t timers;
	cc_ulong tst_bind;
	cc_ulong mgm_bind;
	cc_ulong mnt_bind;
	cc_ulong icc_bind;
	cc_ulong ogc_bind[0];
	/* followed by bound streams for outgoing calls */
} isup_statem_df_t;

typedef struct isup_statem {
	cc_ulong type;			/* object type */
	cc_ulong id;			/* object id */
	cc_ulong flags;			/* object flags */
	cc_ulong state;			/* object state */
	/* followed by object-specific state structure */
} isup_statem_t;

#define ISUP_IOCGSTATEM		_IOWR(	ISUP_IOC_MAGIC,  6,  isup_statem_t )
#define ISUP_IOCCMRESET		_IOWR(	ISUP_IOC_MAGIC,  7,  isup_statem_t )

/*
 *  STATISTICS
 */
typedef struct isup_stats {
	cc_ulong type;			/* object type */
	cc_ulong id;			/* object id */
	cc_ulong header;		/* object stats header */
} isup_stats_t;

typedef struct isup_stats_ct {
	/* circuit stats */
	cc_ulong ct_t17_expiry;		/* Q.752 12.1 */
	cc_ulong ct_rel_abnormal;	/* Q.752 12.6 */
	cc_ulong ct_unexpected_bla;	/* Q.752 12.14 */
	cc_ulong ct_unexpected_uba;	/* Q.752 12.15 */
	cc_ulong ct_t13_expiry;		/* Q.752 12.16 */
	cc_ulong ct_t15_expiry;		/* Q.752 12.17 */
	cc_ulong ct_bad_msg_format;	/* Q.752 12.20 */
	cc_ulong ct_unexpected_msg;	/* Q.752 12.21 */
	cc_ulong ct_rel_unrec_info;	/* Q.752 12.22 */
	cc_ulong ct_rel_problem;	/* Q.752 12.23 */
	cc_ulong ct_segm_discarded;
	cc_ulong ct_missing_ack_cgba;;	/* Q.752 12.8 */
	cc_ulong ct_missing_ack_cgua;;	/* Q.752 12.9 */
	cc_ulong ct_abnormal_ack_cgba;;	/* Q.752 12.10 */
	cc_ulong ct_abnormal_ack_cgua;;	/* Q.752 12.11 */
} isup_stats_ct_t;

typedef struct isup_stats_cg {
	/* circuit stats */
	cc_ulong ct_t17_expiry;		/* Q.752 12.1 */
	cc_ulong ct_t23_expiry;		/* Q.752 12.2 */
	cc_ulong ct_rel_abnormal;	/* Q.752 12.6 */
	cc_ulong ct_unexpected_bla;	/* Q.752 12.14 */
	cc_ulong ct_unexpected_uba;	/* Q.752 12.15 */
	cc_ulong ct_t13_expiry;		/* Q.752 12.16 */
	cc_ulong ct_t15_expiry;		/* Q.752 12.17 */
	cc_ulong ct_bad_msg_format;	/* Q.752 12.20 */
	cc_ulong ct_unexpected_msg;	/* Q.752 12.21 */
	cc_ulong ct_rel_unrec_info;	/* Q.752 12.22 */
	cc_ulong ct_rel_problem;	/* Q.752 12.23 */
	cc_ulong ct_segm_discarded;
	cc_ulong ct_missing_ack_cgba;;	/* Q.752 12.8 */
	cc_ulong ct_missing_ack_cgua;;	/* Q.752 12.9 */
	cc_ulong ct_abnormal_ack_cgba;;	/* Q.752 12.10 */
	cc_ulong ct_abnormal_ack_cgua;;	/* Q.752 12.11 */
	/* circuit group stats */
	cc_ulong cg_unexpected_cgba;	/* Q.752 12.12 */
	cc_ulong cg_unexpected_cgua;	/* Q.752 12.13 */
	cc_ulong cg_t19_expiry;		/* Q.752 12.18 */
	cc_ulong cg_t21_expiry;		/* Q.752 12.19 */
	cc_ulong cg_t23_expiry;
} isup_stats_cg_t;

typedef struct isup_stats_tg {
	/* circuit stats */
	cc_ulong ct_t17_expiry;		/* Q.752 12.1 */
	cc_ulong ct_t23_expiry;		/* Q.752 12.2 */
	cc_ulong ct_rel_abnormal;	/* Q.752 12.6 */
	cc_ulong ct_unexpected_bla;	/* Q.752 12.14 */
	cc_ulong ct_unexpected_uba;	/* Q.752 12.15 */
	cc_ulong ct_t13_expiry;		/* Q.752 12.16 */
	cc_ulong ct_t15_expiry;		/* Q.752 12.17 */
	cc_ulong ct_bad_msg_format;	/* Q.752 12.20 */
	cc_ulong ct_unexpected_msg;	/* Q.752 12.21 */
	cc_ulong ct_rel_unrec_info;	/* Q.752 12.22 */
	cc_ulong ct_rel_problem;	/* Q.752 12.23 */
	cc_ulong ct_segm_discarded;
	cc_ulong ct_missing_ack_cgba;;	/* Q.752 12.8 */
	cc_ulong ct_missing_ack_cgua;;	/* Q.752 12.9 */
	cc_ulong ct_abnormal_ack_cgba;;	/* Q.752 12.10 */
	cc_ulong ct_abnormal_ack_cgua;;	/* Q.752 12.11 */
	/* trunk group stats */
	cc_ulong tg_dual_siezures;
} isup_stats_tg_t;

typedef struct isup_stats_sr {
	/* circuit stats */
	cc_ulong ct_t17_expiry;		/* Q.752 12.1 */
	cc_ulong ct_t23_expiry;		/* Q.752 12.2 */
	cc_ulong ct_rel_abnormal;	/* Q.752 12.6 */
	cc_ulong ct_unexpected_bla;	/* Q.752 12.14 */
	cc_ulong ct_unexpected_uba;	/* Q.752 12.15 */
	cc_ulong ct_t13_expiry;		/* Q.752 12.16 */
	cc_ulong ct_t15_expiry;		/* Q.752 12.17 */
	cc_ulong ct_bad_msg_format;	/* Q.752 12.20 */
	cc_ulong ct_unexpected_msg;	/* Q.752 12.21 */
	cc_ulong ct_rel_unrec_info;	/* Q.752 12.22 */
	cc_ulong ct_rel_problem;	/* Q.752 12.23 */
	cc_ulong ct_segm_discarded;
	cc_ulong ct_missing_ack_cgba;;	/* Q.752 12.8 */
	cc_ulong ct_missing_ack_cgua;;	/* Q.752 12.9 */
	cc_ulong ct_abnormal_ack_cgba;;	/* Q.752 12.10 */
	cc_ulong ct_abnormal_ack_cgua;;	/* Q.752 12.11 */
	/* circuit group stats */
	cc_ulong cg_unexpected_cgba;	/* Q.752 12.12 */
	cc_ulong cg_unexpected_cgua;	/* Q.752 12.13 */
	cc_ulong cg_t19_expiry;		/* Q.752 12.18 */
	cc_ulong cg_t21_expiry;		/* Q.752 12.19 */
	cc_ulong cg_t23_expiry;
	/* trunk group stats */
	cc_ulong tg_dual_siezures;
	/* user part stats */
	cc_ulong sr_up_unavailable;	/* Q.752 10.4, 10.10 */
	cc_ulong sr_up_congested;	/* Q.752 10.7, 10.13 */
	cc_ulong sr_unequipped_cic;
	/* message stats */
	cc_ulong msgs_sent;		/* Q.752 11.1 */
	cc_ulong msgs_recv;		/* Q.752 11.2 */
	cc_ulong msgs_sent_by_type[256];	/* Q.752 11.1 */
	cc_ulong msgs_recv_by_type[256];	/* Q.752 11.2 */
} isup_stats_sr_t;

typedef struct isup_stats_sp {
	/* circuit stats */
	cc_ulong ct_t17_expiry;		/* Q.752 12.1 */
	cc_ulong ct_t23_expiry;		/* Q.752 12.2 */
	cc_ulong ct_rel_abnormal;	/* Q.752 12.6 */
	cc_ulong ct_unexpected_bla;	/* Q.752 12.14 */
	cc_ulong ct_unexpected_uba;	/* Q.752 12.15 */
	cc_ulong ct_t13_expiry;		/* Q.752 12.16 */
	cc_ulong ct_t15_expiry;		/* Q.752 12.17 */
	cc_ulong ct_bad_msg_format;	/* Q.752 12.20 */
	cc_ulong ct_unexpected_msg;	/* Q.752 12.21 */
	cc_ulong ct_rel_unrec_info;	/* Q.752 12.22 */
	cc_ulong ct_rel_problem;	/* Q.752 12.23 */
	cc_ulong ct_segm_discarded;
	cc_ulong ct_missing_ack_cgba;;	/* Q.752 12.8 */
	cc_ulong ct_missing_ack_cgua;;	/* Q.752 12.9 */
	cc_ulong ct_abnormal_ack_cgba;;	/* Q.752 12.10 */
	cc_ulong ct_abnormal_ack_cgua;;	/* Q.752 12.11 */
	/* circuit group stats */
	cc_ulong cg_unexpected_cgba;	/* Q.752 12.12 */
	cc_ulong cg_unexpected_cgua;	/* Q.752 12.13 */
	cc_ulong cg_t19_expiry;		/* Q.752 12.18 */
	cc_ulong cg_t21_expiry;		/* Q.752 12.19 */
	cc_ulong cg_t23_expiry;
	/* trunk group stats */
	cc_ulong tg_dual_siezures;
	/* user part stats */
	cc_ulong sr_up_unavailable;	/* Q.752 10.4, 10.10 */
	cc_ulong sr_up_congested;	/* Q.752 10.7, 10.13 */
	cc_ulong sr_unequipped_cic;
	/* message stats */
	cc_ulong msgs_sent;		/* Q.752 11.1 */
	cc_ulong msgs_recv;		/* Q.752 11.2 */
	cc_ulong msgs_sent_by_type[256];	/* Q.752 11.1 */
	cc_ulong msgs_recv_by_type[256];	/* Q.752 11.2 */
} isup_stats_sp_t;

typedef struct isup_stats_mtp {
} isup_stats_mtp_t;

typedef struct isup_stats_df {
	/* circuit stats */
	cc_ulong ct_t17_expiry;		/* Q.752 12.1 */
	cc_ulong ct_t23_expiry;		/* Q.752 12.2 */
	cc_ulong ct_rel_abnormal;	/* Q.752 12.6 */
	cc_ulong ct_unexpected_bla;	/* Q.752 12.14 */
	cc_ulong ct_unexpected_uba;	/* Q.752 12.15 */
	cc_ulong ct_t13_expiry;		/* Q.752 12.16 */
	cc_ulong ct_t15_expiry;		/* Q.752 12.17 */
	cc_ulong ct_bad_msg_format;	/* Q.752 12.20 */
	cc_ulong ct_unexpected_msg;	/* Q.752 12.21 */
	cc_ulong ct_rel_unrec_info;	/* Q.752 12.22 */
	cc_ulong ct_rel_problem;	/* Q.752 12.23 */
	cc_ulong ct_segm_discarded;
	cc_ulong ct_missing_ack_cgba;;	/* Q.752 12.8 */
	cc_ulong ct_missing_ack_cgua;;	/* Q.752 12.9 */
	cc_ulong ct_abnormal_ack_cgba;;	/* Q.752 12.10 */
	cc_ulong ct_abnormal_ack_cgua;;	/* Q.752 12.11 */
	/* circuit group stats */
	cc_ulong cg_unexpected_cgba;	/* Q.752 12.12 */
	cc_ulong cg_unexpected_cgua;	/* Q.752 12.13 */
	cc_ulong cg_t19_expiry;		/* Q.752 12.18 */
	cc_ulong cg_t21_expiry;		/* Q.752 12.19 */
	cc_ulong cg_t23_expiry;
	/* trunk group stats */
	cc_ulong tg_dual_siezures;
	/* message stats */
	cc_ulong msgs_sent;		/* Q.752 11.1 */
	cc_ulong msgs_recv;		/* Q.752 11.2 */
	cc_ulong msgs_sent_by_type[256];	/* Q.752 11.1 */
	cc_ulong msgs_recv_by_type[256];	/* Q.752 11.2 */
} isup_stats_df_t;

#define ISUP_IOCGSTATSP		_IOWR(	ISUP_IOC_MAGIC,  8,  isup_stats_t )
#define ISUP_IOCSSTATSP		_IOWR(	ISUP_IOC_MAGIC,  9,  isup_stats_t )
#define ISUP_IOCGSTATS		_IOWR(	ISUP_IOC_MAGIC, 10,  isup_stats_t )
#define ISUP_IOCCSTATS		_IOWR(	ISUP_IOC_MAGIC, 11,  isup_stats_t )

/*
 *  EVENTS
 */
#define ISUP_EVENT_ISUP_UNAVAIL_FAILURE	0x00000001UL	/* Q.752 10.1, 10.8 on occrence */
#define ISUP_EVENT_ISUP_UNAVAIL_BLOCKED	0x00000002UL	/* Q.752 10.2 on occrence */
#define ISUP_EVENT_ISUP_AVAIL		0x00000004UL	/* Q.752 10.3, 10.9 on occrence */
#define ISUP_EVENT_ISUP_CONG		0x00000008UL	/* Q.752 10.5, 10.11 on occrence */
#define ISUP_EVENT_ISUP_CONG_CEASED	0x00000010UL	/* Q.752 10.6, 10.12 on occrence */
#define ISUP_EVENT_T17_TIMEOUT		0x00000020UL	/* Q.752 12.1 1st and delta */
#define ISUP_EVENT_T23_TIMEOUT		0x00000040UL	/* Q.752 12.2 1st and delta */
#define ISUP_EVENT_T5_TIMEOUT		0x00000080UL	/* Q.752 12.5 on occrence */
#define ISUP_EVENT_ABNORMAL_RELEASE	0x00000100UL	/* Q.752 12.6 1st and delta */
#define ISUP_EVENT_HARDWARE_BLOCKING	0x00000200UL	/* Q.752 12.7 on occrence */
#define ISUP_EVENT_MISSING_ACK_IN_CGBA	0x00000400UL	/* Q.752 12.8 1st and delta */
#define ISUP_EVENT_MISSING_ACK_IN_CGUA	0x00000800UL	/* Q.752 12.9 1st and delta */
#define ISUP_EVENT_ABNORMAL_ACK_IN_CGBA	0x00001000UL	/* Q.752 12.10 1st and delta */
#define ISUP_EVENT_ABNORMAL_ACK_IN_CGUA	0x00002000UL	/* Q.752 12.11 1st and delta */
#define ISUP_EVENT_UNEXPECTED_CGBA	0x00004000UL	/* Q.752 12.12 1st and delta */
#define ISUP_EVENT_UNEXPECTED_CGUA	0x00008000UL	/* Q.752 12.13 1st and delta */
#define ISUP_EVENT_UNEXPECTED_BLA	0x00010000UL	/* Q.752 12.14 1st and delta */
#define ISUP_EVENT_UNEXPECTED_UBA	0x00020000UL	/* Q.752 12.15 1st and delta */
#define ISUP_EVENT_T13_TIMEOUT		0x00040000UL	/* Q.752 12.16 1st and delta */
#define ISUP_EVENT_T15_TIMEOUT		0x00080000UL	/* Q.752 12.17 1st and delta */
#define ISUP_EVENT_T19_TIMEOUT		0x00100000UL	/* Q.752 12.18 1st and delta */
#define ISUP_EVENT_T21_TIMEOUT		0x00200000UL	/* Q.752 12.19 1st and delta */
#define ISUP_EVENT_MESSAGE_FORMAT_ERROR	0x00400000UL	/* Q.752 12.20 1st and delta */
#define ISUP_EVENT_UNEXPECTED_MESSAGE	0x00800000UL	/* Q.752 12.21 1st and delta */
#define ISUP_EVENT_RELEASE_UNREC_INFO	0x01000000UL	/* Q.752 12.22 1st and delta */
#define ISUP_EVENT_RELEAWE_FAILURE	0x02000000UL	/* Q.752 12.23 1st and delta */

#define ISUP_EVENT_CT_MASK ( \
		ISUP_EVENT_T17_TIMEOUT | \
		ISUP_EVENT_T23_TIMEOUT | \
		ISUP_EVENT_T5_TIMEOUT | \
		ISUP_EVENT_ABNORMAL_RELEASE | \
		ISUP_EVENT_HARDWARE_BLOCKING | \
		ISUP_EVENT_UNEXPECTED_BLA | \
		ISUP_EVENT_UNEXPECTED_UBA | \
		ISUP_EVENT_T13_TIMEOUT | \
		ISUP_EVENT_T15_TIMEOUT | \
		ISUP_EVENT_T19_TIMEOUT | \
		ISUP_EVENT_T21_TIMEOUT | \
		ISUP_EVENT_MESSAGE_FORMAT_ERROR | \
		ISUP_EVENT_UNEXPECTED_MESSAGE | \
		ISUP_EVENT_RELEASE_UNREC_INFO | \
		ISUP_EVENT_RELEAWE_FAILURE )

#define ISUP_EVENT_CG_MASK ( \
		ISUP_EVENT_CT_MASK | \
		ISUP_EVENT_MISSING_ACK_IN_CGBA | \
		ISUP_EVENT_MISSING_ACK_IN_CGUA | \
		ISUP_EVENT_ABNORMAL_ACK_IN_CGBA | \
		ISUP_EVENT_ABNORMAL_ACK_IN_CGUA | \
		ISUP_EVENT_UNEXPECTED_CGBA | \
		ISUP_EVENT_UNEXPECTED_CGUA )

#define ISUP_EVENT_TG_MASK ( \
		ISUP_EVENT_CT_MASK \
		ISUP_EVENT_CG_MASK )

#define ISUP_EVENT_SR_MASK ( ( \
		ISUP_EVENT_ISUP_UNAVAIL_FAILURE | \
		ISUP_EVENT_ISUP_UNAVAIL_BLOCKED | \
		ISUP_EVENT_ISUP_AVAIL | \
		ISUP_EVENT_ISUP_CONG | \
		ISUP_EVENT_ISUP_CONG_CEASED ) | \
		ISUP_EVENT_TG_MASK )

#define ISUP_EVENT_SP_MASK ( \
		ISUP_EVENT_SR_MASK )

#define ISUP_EVENT_DF_MASK ( \
		ISUP_EVENT_SP_MASK )

typedef struct isup_notify {
	cc_ulong type;			/* object type */
	cc_ulong id;			/* object id */
	lmi_notify_t notify;		/* notifications */
} isup_notify_t;

#define ISUP_IOCGNOTIFY		_IOR(	ISUP_IOC_MAGIC, 12,  isup_notify_t )
#define ISUP_IOCSNOTIFY		_IOW(	ISUP_IOC_MAGIC, 13,  isup_notify_t )
#define ISUP_IOCCNOTIFY		_IOW(	ISUP_IOC_MAGIC, 14,  isup_notify_t )

/*
   10.1 (Event:On occur) Start of local ISDN-UP unavailable - failure 
   10.2 (Event:On occur) Start of local ISDN-UP unavailable - maint made busy 
   10.3 (Event:On occur) ISDN-UP available 
   10.4 (s:30min) Total duration ISDN-UP unavailable 
   10.5 (Event:On occur) Start of local ISDN-UP congestion 
   10.6 (Event:On occur) Stop of local ISDN-UP congestion 
   10.7 (s:30min) Duration of local ISDN-UP congestion 
   10.8 (Event/dest:On occur) Start of remote ISDN-UP unavailable 
   10.9 (Event/dest:On occur) Stop of remote ISDN-UP unavailable 
   10.10 (s/dest:30min) Duration of remote ISDN-UP unavailable 
   10.11 (Event/dest:On occur) Start of remote ISDN-UP congestion 
   10.12 (Event/dest:On occur) Stop of remote ISDN-UP congestion 
   10.13 (s/dest:30min) Duration of remote ISDN-UP congestion 
   11.1 (Msgs/type:5/30min) Total ISDN-UP messages sent (by type) 
   11.2 (Msgs/type:5/30min) Total ISDN-UP messages received (by type) 
   12.1 (Event/CIC/dest:1+d) No acknowledgement for circuit reset with T17 
   12.2 (Event/CIC/dest:1+d) No GRA reeived for GRS within T23 
   12.3 - 
   12.4 - 
   12.5 (Event/CIC/dest:On occur) RLC not received within T5 
   12.6 (Event/CIC/dest:1+d) Release initiated due to abnormal conditions 
   12.7 (Event/CIC/dest:On occur) Circuit BLO (excessive errors detected by CRC) 
   12.8 (Event/CIC/dest:1+d) Missing blocking acknowledgement in CGBA for previous CGB 
   12.9 (Event/CIC/dest:1+d) Missing unblocking acknowledgement in CGUA for previous CGU 
   12.10 (Event/CIC/dest:1+d) Abnormal blocking acknowledgement in CGBA for prevous CGB 
   12.11 (Event/CIC/dest:1+d) Abnormal unblocking acknowledgement in CGUA for previous CGU 
   12.12 (Event/CIC/dest:1+d) Unexpected CGBA with abnormal blocking acknowledgement 
   12.13 (Event/CIC/dest:1+d) Unexpected CGUA with abnormal unblocking acknowledgement 
   12.14 (Event/CIC/dest:1+d) Unexpected BLA with abnormal blocking acknowledgement 
   12.15 (Event/CIC/dest:1+d) Unexpected UBA with abnormal unblocking acknowledgement 
   12.16 (Event/CIC/dest:1+d) No BLA received for BLO within T13 
   12.17 (Event/CIC/dest:1+d) No UBA received for UBL within T15 
   12.18 (Event/CIC/dest:1+d) No CGBA received for CGB within T19 
   12.19 (Event/CIC/dest:1+d) No CGUA received for CGU within T21 
   12.20 (Event/CIC/dest:1+d) Message format error 
   12.21 (Event/CIC/dest:1+d) Unexpected message received 
   12.22 (Event/CIC/dest:1+d) Release due to unrecognized info. 
   12.23 (Event/CIC:1+d) Inability to release a circuit 
 */

/*
 *  ISUP MANAGEMENT
 */
typedef struct isup_mgmt {
	cc_ulong type;			/* object type */
	cc_ulong id;			/* object id */
	cc_ulong cmd;			/* command */
} isup_mgmt_t;

#define ISUP_MGMT_BLOCK			1
#define ISUP_MGMT_UNBLOCK		2
#define ISUP_MGMT_RESET			3
#define ISUP_MGMT_QUERY			4

#define ISUP_IOCCMGMT		_IOWR(	ISUP_IOC_MAGIC, 15,  isup_mgmt_t )

/*
 *  CONTROL LOWER
 */
typedef struct isup_pass {
	cc_ulong muxid;			/* mux index of lower MTP to pass message to */
	cc_ulong type;			/* type of message block */
	cc_ulong band;			/* band of message block */
	cc_ulong ctl_length;		/* length of cntl part */
	cc_ulong dat_length;		/* length of data part */
	/* followed by cntl and data part of message to pass to MTP */
} isup_pass_t;

#define ISUP_IOCCPASS		_IOWR(	ISUP_IOC_MAGIC, 16,  isup_pass_t )

#define ISUP_IOC_FIRST		 0
#define ISUP_IOC_LAST		16
#define ISUP_IOC_PRIVATE	32

#endif				/* __ISUPI_IOCTL_H__ */
