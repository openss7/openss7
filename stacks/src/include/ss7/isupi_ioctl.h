/*****************************************************************************

 @(#) $Id: isupi_ioctl.h,v 0.9.2.2 2004/08/30 06:19:38 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (C) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>

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

 Last Modified $Date: 2004/08/30 06:19:38 $ by $Author: brian $

 *****************************************************************************/

#ifndef __ISUPI_IOCTL_H__
#define __ISUPI_IOCTL_H__

#ident "@(#) $Name:  $($Revision: 0.9.2.2 $) Copyright (c) 1997-2002 OpenSS7 Corporation."

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
	ulong cgid;			/* circuit group identifier */
	ulong tgid;			/* trunk group identifier */
	ulong cic;			/* circuit identification code */
} isup_conf_ct_t;

typedef struct isup_opt_conf_ct {
} isup_opt_conf_ct_t;

/*
 *  Circuit Group configuration
 *  -----------------------------------
 */
typedef struct isup_conf_cg {
	ulong srid;			/* remote signalling point identifier */
	lmi_option_t proto;		/* protocol variant and options */
} isup_conf_cg_t;

typedef struct isup_opt_conf_cg {
} isup_opt_conf_cg_t;

/*
 *  Trunk Group configuration
 *  -----------------------------------
 */
typedef struct isup_conf_tg {
	ulong srid;			/* remote signalling point identifier */
	lmi_option_t proto;		/* protocol variant and options */
} isup_conf_tg_t;

typedef struct isup_opt_conf_tg {
	ulong flags;			/* configuration flags */
	ulong type;			/* trunk group type */
	ulong exchange_type;		/* exchange type */
	ulong select_type;		/* circuit selection type */
	ulong t1;			/* waiting for RLC retry */
	ulong t2;			/* waiting for RES */
	ulong t3;			/* waiting OVL timeout */
	ulong t5;			/* waiting for RLC maintenance */
	ulong t6;			/* waiting for RES */
	ulong t7;			/* waiting for ACM/ANM/CON */
	ulong t8;			/* waiting for COT */
	ulong t9;			/* waiting for ANM/CON */
	ulong t10;			/* waiting more information. Interworking */
	ulong t11;			/* waiting for ACM, Interworking */
	ulong t12;			/* waiting for BLA retry */
	ulong t13;			/* waiting for BLA maintenance */
	ulong t14;			/* waiting for UBA retry */
	ulong t15;			/* waiting for UBA maintenance */
	ulong t16;			/* waiting for RLC retry */
	ulong t17;			/* waiting for RLC maintenance */
	ulong t24;			/* waiting for continuity IAM */
	ulong t25;			/* waiting for continuity CCR retry */
	ulong t26;			/* waiting for continuity CCR maintenance */
	ulong t27;			/* waiting for COT reset */
	ulong t31;			/* call reference guard */
	ulong t32;			/* waiting to send E2E message */
	ulong t33;			/* waiting for INF */
	ulong t34;			/* waiting for SEG */
	ulong t35;			/* waiting more information. */
	ulong t36;			/* waiting for COT/REL */
	ulong t37;			/* waiting echo control device */
	ulong t38;			/* waiting for RES */
	ulong tacc_r;			/* */
	ulong tccr;			/* */
	ulong tccr_r;			/* */
	ulong tcra;			/* */
	ulong tcrm;			/* */
	ulong tcvt;			/* */
	ulong texm_d;			/* */
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
	ulong spid;			/* local signalling point identifier */
	struct mtp_addr add;		/* remote signalling point code */
	lmi_option_t proto;		/* protocol variant and options */
} isup_conf_sr_t;

typedef struct isup_opt_conf_sr {
	ulong t4;			/* waiting for UPA/other */
	ulong t18;			/* waiting CGBA retry */
	ulong t19;			/* waiting CGBA maintenance */
	ulong t20;			/* waiting CGUA retry */
	ulong t21;			/* waiting CGUA maintenance */
	ulong t22;			/* waiting GRA retry */
	ulong t23;			/* waiting GRA maintenance */
	ulong t28;			/* waiting CQR */
	ulong t29;			/* congestion attack timer */
	ulong t30;			/* congestion decay timer */
	ulong tcgb;			/* */
	ulong tgrs;			/* */
	ulong thga;			/* */
	ulong tscga;			/* */
	ulong tscga_d;			/* */
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
	ulong spid;			/* local signalling point identifier */
	ulong srid;			/* remote signalling point identifier */
	ulong muxid;			/* lower multipleding driver id */
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
	ulong type;			/* object type */
	ulong id;			/* object id */
	/*
	   followed by specific proto structure 
	 */
} isup_option_t;

#define ISUP_IOCGOPTIONS	_IOR(	ISUP_IOC_MAGIC,  0,  isup_option_t   )
#define ISUP_IOCSOPTIONS	_IOW(	ISUP_IOC_MAGIC,  1,  isup_option_t   )

/*
 *  CONFIGURATION
 */
typedef struct isup_config {
	ulong type;			/* object type */
	ulong id;			/* object id */
	ulong cmd;			/* object command */
	/*
	   followed by specific configuration structure 
	 */
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
	/*
	   Call Control Timers 
	 */
	ulong t1;
	ulong t2;
	ulong t3;
	ulong t5;
	ulong t6;
	ulong t7;
	ulong t8;
	ulong t9;
	ulong t10;
	ulong t11;
	ulong t24;
	ulong t31;
	ulong t32;
	ulong t33;
	ulong t34;
	ulong t35;
	ulong t38;
	ulong tacc_r;
	ulong tcra;
	ulong tcrm;
	ulong texm_d;
	/*
	   Circuit Timers 
	 */
	ulong t12;
	ulong t13;
	ulong t14;
	ulong t15;
	ulong t16;
	ulong t17;
	ulong t25;
	ulong t26;
	ulong t27;
	ulong t36;
	ulong t37;
	ulong tccr;
	ulong tccr_r;
	ulong tcvt;
} isup_timers_ct_t;

typedef struct isup_statem_ct {
	isup_timers_ct_t timers;
	ulong c_state;
	ulong i_state;
	ulong m_state;
	ulong tst_actv;
	ulong mgm_actv;
	ulong cpc_actv;
	ulong tst_bind;
	ulong mgm_bind;
	ulong mnt_bind;
	ulong icc_bind;
	ulong ogc_bind[0];
	/*
	   followed by bound streams for outgoing calls 
	 */
} isup_statem_ct_t;

typedef struct isup_timers_cg {
	/*
	   Circuit Group Timers 
	 */
	ulong t18;
	ulong t19;
	ulong t20;
	ulong t21;
	ulong t22;
	ulong t23;
	ulong t28;
	ulong tcgb;
	ulong tgrs;
	ulong thga;
	ulong tscga;
	ulong tscga_d;
} isup_timers_cg_t;

typedef struct isup_statem_cg {
	isup_timers_cg_t timers;
	ulong g_state;
	ulong gmg_actv;
	ulong tst_bind;
	ulong mgm_bind;
	ulong mnt_bind;
	ulong icc_bind;
	ulong ogc_bind[0];
	/*
	   followed by bound streams for outgoing calls 
	 */
} isup_statem_cg_t;

typedef struct isup_timers_tg {
} isup_timers_tg_t;

typedef struct isup_statem_tg {
	isup_timers_tg_t timers;
	ulong tst_bind;
	ulong mgm_bind;
	ulong mnt_bind;
	ulong icc_bind;
	ulong ogc_bind[0];
	/*
	   followed by bound streams for outgoing calls 
	 */
} isup_statem_tg_t;

typedef struct isup_timers_sr {
	ulong t4;			/* UPT timer */
	ulong t29;			/* signalling congestion attack timer */
	ulong t30;			/* signalling congestion decay timer */
} isup_timers_sr_t;

typedef struct isup_statem_sr {
	isup_timers_sr_t timers;
	ulong cong_level;
	ulong tst_bind;
	ulong mgm_bind;
	ulong mnt_bind;
	ulong icc_bind;
	ulong ogc_bind[0];
	/*
	   followed by bound streams for outgoing calls 
	 */
} isup_statem_sr_t;

typedef struct isup_timers_sp {
} isup_timers_sp_t;

typedef struct isup_statem_sp {
	isup_timers_sp_t timers;
	ulong tst_bind;
	ulong mgm_bind;
	ulong mnt_bind;
	ulong icc_bind;
	ulong ogc_bind[0];
	/*
	   followed by bound streams for outgoing calls 
	 */
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
	ulong tst_bind;
	ulong mgm_bind;
	ulong mnt_bind;
	ulong icc_bind;
	ulong ogc_bind[0];
	/*
	   followed by bound streams for outgoing calls 
	 */
} isup_statem_df_t;

typedef struct isup_statem {
	ulong type;			/* object type */
	ulong id;			/* object id */
	ulong flags;			/* object flags */
	ulong state;			/* object state */
	/*
	   followed by object-specific state structure 
	 */
} isup_statem_t;

#define ISUP_IOCGSTATEM		_IOWR(	ISUP_IOC_MAGIC,  6,  isup_statem_t )
#define ISUP_IOCCMRESET		_IOWR(	ISUP_IOC_MAGIC,  7,  isup_statem_t )

/*
 *  STATISTICS
 */
typedef struct isup_stats {
	ulong type;			/* object type */
	ulong id;			/* object id */
	ulong header;			/* object stats header */
} isup_stats_t;

typedef struct isup_stats_ct {
	/*
	   circuit stats 
	 */
	ulong ct_t17_expiry;		/* Q.752 12.1 */
	ulong ct_rel_abnormal;		/* Q.752 12.6 */
	ulong ct_unexpected_bla;	/* Q.752 12.14 */
	ulong ct_unexpected_uba;	/* Q.752 12.15 */
	ulong ct_t13_expiry;		/* Q.752 12.16 */
	ulong ct_t15_expiry;		/* Q.752 12.17 */
	ulong ct_bad_msg_format;	/* Q.752 12.20 */
	ulong ct_unexpected_msg;	/* Q.752 12.21 */
	ulong ct_rel_unrec_info;	/* Q.752 12.22 */
	ulong ct_rel_problem;		/* Q.752 12.23 */
	ulong ct_segm_discarded;
	ulong ct_missing_ack_cgba;;	/* Q.752 12.8 */
	ulong ct_missing_ack_cgua;;	/* Q.752 12.9 */
	ulong ct_abnormal_ack_cgba;;	/* Q.752 12.10 */
	ulong ct_abnormal_ack_cgua;;	/* Q.752 12.11 */
} isup_stats_ct_t;

typedef struct isup_stats_cg {
	/*
	   circuit stats 
	 */
	ulong ct_t17_expiry;		/* Q.752 12.1 */
	ulong ct_t23_expiry;		/* Q.752 12.2 */
	ulong ct_rel_abnormal;		/* Q.752 12.6 */
	ulong ct_unexpected_bla;	/* Q.752 12.14 */
	ulong ct_unexpected_uba;	/* Q.752 12.15 */
	ulong ct_t13_expiry;		/* Q.752 12.16 */
	ulong ct_t15_expiry;		/* Q.752 12.17 */
	ulong ct_bad_msg_format;	/* Q.752 12.20 */
	ulong ct_unexpected_msg;	/* Q.752 12.21 */
	ulong ct_rel_unrec_info;	/* Q.752 12.22 */
	ulong ct_rel_problem;		/* Q.752 12.23 */
	ulong ct_segm_discarded;
	ulong ct_missing_ack_cgba;;	/* Q.752 12.8 */
	ulong ct_missing_ack_cgua;;	/* Q.752 12.9 */
	ulong ct_abnormal_ack_cgba;;	/* Q.752 12.10 */
	ulong ct_abnormal_ack_cgua;;	/* Q.752 12.11 */
	/*
	   circuit group stats 
	 */
	ulong cg_unexpected_cgba;	/* Q.752 12.12 */
	ulong cg_unexpected_cgua;	/* Q.752 12.13 */
	ulong cg_t19_expiry;		/* Q.752 12.18 */
	ulong cg_t21_expiry;		/* Q.752 12.19 */
	ulong cg_t23_expiry;
} isup_stats_cg_t;

typedef struct isup_stats_tg {
	/*
	   circuit stats 
	 */
	ulong ct_t17_expiry;		/* Q.752 12.1 */
	ulong ct_t23_expiry;		/* Q.752 12.2 */
	ulong ct_rel_abnormal;		/* Q.752 12.6 */
	ulong ct_unexpected_bla;	/* Q.752 12.14 */
	ulong ct_unexpected_uba;	/* Q.752 12.15 */
	ulong ct_t13_expiry;		/* Q.752 12.16 */
	ulong ct_t15_expiry;		/* Q.752 12.17 */
	ulong ct_bad_msg_format;	/* Q.752 12.20 */
	ulong ct_unexpected_msg;	/* Q.752 12.21 */
	ulong ct_rel_unrec_info;	/* Q.752 12.22 */
	ulong ct_rel_problem;		/* Q.752 12.23 */
	ulong ct_segm_discarded;
	ulong ct_missing_ack_cgba;;	/* Q.752 12.8 */
	ulong ct_missing_ack_cgua;;	/* Q.752 12.9 */
	ulong ct_abnormal_ack_cgba;;	/* Q.752 12.10 */
	ulong ct_abnormal_ack_cgua;;	/* Q.752 12.11 */
	/*
	   trunk group stats 
	 */
	ulong tg_dual_siezures;
} isup_stats_tg_t;

typedef struct isup_stats_sr {
	/*
	   circuit stats 
	 */
	ulong ct_t17_expiry;		/* Q.752 12.1 */
	ulong ct_t23_expiry;		/* Q.752 12.2 */
	ulong ct_rel_abnormal;		/* Q.752 12.6 */
	ulong ct_unexpected_bla;	/* Q.752 12.14 */
	ulong ct_unexpected_uba;	/* Q.752 12.15 */
	ulong ct_t13_expiry;		/* Q.752 12.16 */
	ulong ct_t15_expiry;		/* Q.752 12.17 */
	ulong ct_bad_msg_format;	/* Q.752 12.20 */
	ulong ct_unexpected_msg;	/* Q.752 12.21 */
	ulong ct_rel_unrec_info;	/* Q.752 12.22 */
	ulong ct_rel_problem;		/* Q.752 12.23 */
	ulong ct_segm_discarded;
	ulong ct_missing_ack_cgba;;	/* Q.752 12.8 */
	ulong ct_missing_ack_cgua;;	/* Q.752 12.9 */
	ulong ct_abnormal_ack_cgba;;	/* Q.752 12.10 */
	ulong ct_abnormal_ack_cgua;;	/* Q.752 12.11 */
	/*
	   circuit group stats 
	 */
	ulong cg_unexpected_cgba;	/* Q.752 12.12 */
	ulong cg_unexpected_cgua;	/* Q.752 12.13 */
	ulong cg_t19_expiry;		/* Q.752 12.18 */
	ulong cg_t21_expiry;		/* Q.752 12.19 */
	ulong cg_t23_expiry;
	/*
	   trunk group stats 
	 */
	ulong tg_dual_siezures;
	/*
	   user part stats 
	 */
	ulong sr_up_unavailable;	/* Q.752 10.4, 10.10 */
	ulong sr_up_congested;		/* Q.752 10.7, 10.13 */
	ulong sr_unequipped_cic;
	/*
	   message stats 
	 */
	ulong msgs_sent;		/* Q.752 11.1 */
	ulong msgs_recv;		/* Q.752 11.2 */
	ulong msgs_sent_by_type[256];	/* Q.752 11.1 */
	ulong msgs_recv_by_type[256];	/* Q.752 11.2 */
} isup_stats_sr_t;

typedef struct isup_stats_sp {
	/*
	   circuit stats 
	 */
	ulong ct_t17_expiry;		/* Q.752 12.1 */
	ulong ct_t23_expiry;		/* Q.752 12.2 */
	ulong ct_rel_abnormal;		/* Q.752 12.6 */
	ulong ct_unexpected_bla;	/* Q.752 12.14 */
	ulong ct_unexpected_uba;	/* Q.752 12.15 */
	ulong ct_t13_expiry;		/* Q.752 12.16 */
	ulong ct_t15_expiry;		/* Q.752 12.17 */
	ulong ct_bad_msg_format;	/* Q.752 12.20 */
	ulong ct_unexpected_msg;	/* Q.752 12.21 */
	ulong ct_rel_unrec_info;	/* Q.752 12.22 */
	ulong ct_rel_problem;		/* Q.752 12.23 */
	ulong ct_segm_discarded;
	ulong ct_missing_ack_cgba;;	/* Q.752 12.8 */
	ulong ct_missing_ack_cgua;;	/* Q.752 12.9 */
	ulong ct_abnormal_ack_cgba;;	/* Q.752 12.10 */
	ulong ct_abnormal_ack_cgua;;	/* Q.752 12.11 */
	/*
	   circuit group stats 
	 */
	ulong cg_unexpected_cgba;	/* Q.752 12.12 */
	ulong cg_unexpected_cgua;	/* Q.752 12.13 */
	ulong cg_t19_expiry;		/* Q.752 12.18 */
	ulong cg_t21_expiry;		/* Q.752 12.19 */
	ulong cg_t23_expiry;
	/*
	   trunk group stats 
	 */
	ulong tg_dual_siezures;
	/*
	   user part stats 
	 */
	ulong sr_up_unavailable;	/* Q.752 10.4, 10.10 */
	ulong sr_up_congested;		/* Q.752 10.7, 10.13 */
	ulong sr_unequipped_cic;
	/*
	   message stats 
	 */
	ulong msgs_sent;		/* Q.752 11.1 */
	ulong msgs_recv;		/* Q.752 11.2 */
	ulong msgs_sent_by_type[256];	/* Q.752 11.1 */
	ulong msgs_recv_by_type[256];	/* Q.752 11.2 */
} isup_stats_sp_t;

typedef struct isup_stats_mtp {
} isup_stats_mtp_t;

typedef struct isup_stats_df {
	/*
	   circuit stats 
	 */
	ulong ct_t17_expiry;		/* Q.752 12.1 */
	ulong ct_t23_expiry;		/* Q.752 12.2 */
	ulong ct_rel_abnormal;		/* Q.752 12.6 */
	ulong ct_unexpected_bla;	/* Q.752 12.14 */
	ulong ct_unexpected_uba;	/* Q.752 12.15 */
	ulong ct_t13_expiry;		/* Q.752 12.16 */
	ulong ct_t15_expiry;		/* Q.752 12.17 */
	ulong ct_bad_msg_format;	/* Q.752 12.20 */
	ulong ct_unexpected_msg;	/* Q.752 12.21 */
	ulong ct_rel_unrec_info;	/* Q.752 12.22 */
	ulong ct_rel_problem;		/* Q.752 12.23 */
	ulong ct_segm_discarded;
	ulong ct_missing_ack_cgba;;	/* Q.752 12.8 */
	ulong ct_missing_ack_cgua;;	/* Q.752 12.9 */
	ulong ct_abnormal_ack_cgba;;	/* Q.752 12.10 */
	ulong ct_abnormal_ack_cgua;;	/* Q.752 12.11 */
	/*
	   circuit group stats 
	 */
	ulong cg_unexpected_cgba;	/* Q.752 12.12 */
	ulong cg_unexpected_cgua;	/* Q.752 12.13 */
	ulong cg_t19_expiry;		/* Q.752 12.18 */
	ulong cg_t21_expiry;		/* Q.752 12.19 */
	ulong cg_t23_expiry;
	/*
	   trunk group stats 
	 */
	ulong tg_dual_siezures;
	/*
	   message stats 
	 */
	ulong msgs_sent;		/* Q.752 11.1 */
	ulong msgs_recv;		/* Q.752 11.2 */
	ulong msgs_sent_by_type[256];	/* Q.752 11.1 */
	ulong msgs_recv_by_type[256];	/* Q.752 11.2 */
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
	ulong type;			/* object type */
	ulong id;			/* object id */
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
	ulong type;			/* object type */
	ulong id;			/* object id */
	ulong cmd;			/* command */
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
	ulong muxid;			/* mux index of lower MTP to pass message to */
	ulong type;			/* type of message block */
	ulong band;			/* band of message block */
	ulong ctl_length;		/* length of cntl part */
	ulong dat_length;		/* length of data part */
	/*
	   followed by cntl and data part of message to pass to MTP 
	 */
} isup_pass_t;

#define ISUP_IOCCPASS		_IOWR(	ISUP_IOC_MAGIC, 16,  isup_pass_t )

#define ISUP_IOC_FIRST		 0
#define ISUP_IOC_LAST		16
#define ISUP_IOC_PRIVATE	32

#endif				/* __ISUPI_IOCTL_H__ */
