/*****************************************************************************

 @(#) $Id: mtpi_ioctl.h,v 0.9.2.5 2007/02/17 02:49:11 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 2 of the License.

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

 Last Modified $Date: 2007/02/17 02:49:11 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: mtpi_ioctl.h,v $
 Revision 0.9.2.5  2007/02/17 02:49:11  brian
 - first clean recompile of MTP modules on LFS

 Revision 0.9.2.4  2007/02/13 14:05:28  brian
 - corrected ulong and long for 32-bit compat

 *****************************************************************************/

#ifndef __MTPI_IOCTL_H__
#define __MTPI_IOCTL_H__

#ident "@(#) $RCSfile: mtpi_ioctl.h,v $ $Name:  $($Revision: 0.9.2.5 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* This file can be processed by doxygen(1). */

#include <linux/ioctl.h>

#define MTP_IOC_MAGIC 'm'

#define MTP_OBJ_TYPE_DF		 0	/* default */
#define MTP_OBJ_TYPE_MT		 1	/* MTP user */
#define MTP_OBJ_TYPE_NA		 2	/* network appearance */
#define MTP_OBJ_TYPE_SP		 3	/* signalling point */
#define MTP_OBJ_TYPE_RS		 4	/* route set */
#define MTP_OBJ_TYPE_RL		 5	/* route list */
#define MTP_OBJ_TYPE_RT		 6	/* route */
#define MTP_OBJ_TYPE_LS		 7	/* combined link set */
#define MTP_OBJ_TYPE_LK		 8	/* link set */
#define MTP_OBJ_TYPE_SL		 9	/* signalling link */
#define MTP_OBJ_TYPE_CB		10	/* changeover buffer */
#define MTP_OBJ_TYPE_CR		11	/* controlled rerouting buffer */

#define RT_TYPE_MEMBER	0
#define RT_TYPE_CLUSTER	1

#define RS_TYPE_MEMBER	0
#define RS_TYPE_CLUSTER	1

#ifdef __KERNEL__
typedef mblk_t *mtp_timer_t;
#else				/* __KERNEL__ */
typedef unsigned long mtp_timer_t;
#endif				/* __KERNEL__ */

/*
 *  Signalling link options
 */
typedef struct mtp_opt_conf_sl {
	/* signalling link timers */
	mtp_ulong t1;			/* timer t1 value */
	mtp_ulong t2;			/* timer t2 value */
	mtp_ulong t3;			/* timer t3 value */
	mtp_ulong t4;			/* timer t4 value */
	mtp_ulong t5;			/* timer t5 value */
	mtp_ulong t12;			/* timer t12 value */
	mtp_ulong t13;			/* timer t13 value */
	mtp_ulong t14;			/* timer t14 value */
	mtp_ulong t17;			/* timer t17 value */
	mtp_ulong t19a;			/* timer t19a value */
	mtp_ulong t20a;			/* timer t20a value */
	mtp_ulong t21a;			/* timer t21a value */
	mtp_ulong t22;			/* timer t22 value */
	mtp_ulong t23;			/* timer t23 value */
	mtp_ulong t24;			/* timer t24 value */
	mtp_ulong t31a;			/* timer t31a value */
	mtp_ulong t32a;			/* timer t32a value */
	mtp_ulong t33a;			/* timer t33a value */
	mtp_ulong t34a;			/* timer t34a value */
	mtp_ulong t1t;			/* timer t1t value */
	mtp_ulong t2t;			/* timer t2t value */
	mtp_ulong t1s;			/* timer t1s value */
} mtp_opt_conf_sl_t;

/*
 *  Link set options
 */
typedef struct mtp_opt_conf_lk {
	/* signalling link timers */
	mtp_ulong t1;			/* timer t1 value */
	mtp_ulong t2;			/* timer t2 value */
	mtp_ulong t3;			/* timer t3 value */
	mtp_ulong t4;			/* timer t4 value */
	mtp_ulong t5;			/* timer t5 value */
	mtp_ulong t12;			/* timer t12 value */
	mtp_ulong t13;			/* timer t13 value */
	mtp_ulong t14;			/* timer t14 value */
	mtp_ulong t17;			/* timer t17 value */
	mtp_ulong t19a;			/* timer t19a value */
	mtp_ulong t20a;			/* timer t20a value */
	mtp_ulong t21a;			/* timer t21a value */
	mtp_ulong t22;			/* timer t22 value */
	mtp_ulong t23;			/* timer t23 value */
	mtp_ulong t24;			/* timer t24 value */
	mtp_ulong t31a;			/* timer t31a value */
	mtp_ulong t32a;			/* timer t32a value */
	mtp_ulong t33a;			/* timer t33a value */
	mtp_ulong t34a;			/* timer t34a value */
	mtp_ulong t1t;			/* timer t1t value */
	mtp_ulong t2t;			/* timer t2t value */
	mtp_ulong t1s;			/* timer t1s value */
	/* link timers */
	mtp_ulong t7;			/* timer t7 value */
	mtp_ulong t19;			/* timer t19 value */
	mtp_ulong t21;			/* timer t21 value */
	mtp_ulong t25a;			/* timer t25a value */
	mtp_ulong t28a;			/* timer t28a value */
	mtp_ulong t29a;			/* timer t29a value */
	mtp_ulong t30a;			/* timer t30a value */
} mtp_opt_conf_lk_t;

/*
 *  Combined link set options
 */
typedef struct mtp_opt_conf_ls {
	/* signalling link timers */
	mtp_ulong t1;			/* timer t1 value */
	mtp_ulong t2;			/* timer t2 value */
	mtp_ulong t3;			/* timer t3 value */
	mtp_ulong t4;			/* timer t4 value */
	mtp_ulong t5;			/* timer t5 value */
	mtp_ulong t12;			/* timer t12 value */
	mtp_ulong t13;			/* timer t13 value */
	mtp_ulong t14;			/* timer t14 value */
	mtp_ulong t17;			/* timer t17 value */
	mtp_ulong t19a;			/* timer t19a value */
	mtp_ulong t20a;			/* timer t20a value */
	mtp_ulong t21a;			/* timer t21a value */
	mtp_ulong t22;			/* timer t22 value */
	mtp_ulong t23;			/* timer t23 value */
	mtp_ulong t24;			/* timer t24 value */
	mtp_ulong t31a;			/* timer t31a value */
	mtp_ulong t32a;			/* timer t32a value */
	mtp_ulong t33a;			/* timer t33a value */
	mtp_ulong t34a;			/* timer t34a value */
	mtp_ulong t1t;			/* timer t1t value */
	mtp_ulong t2t;			/* timer t2t value */
	mtp_ulong t1s;			/* timer t1s value */
	/* link timers */
	mtp_ulong t7;			/* timer t7 value */
	mtp_ulong t19;			/* timer t19 value */
	mtp_ulong t21;			/* timer t21 value */
	mtp_ulong t25a;			/* timer t25a value */
	mtp_ulong t28a;			/* timer t28a value */
	mtp_ulong t29a;			/* timer t29a value */
	mtp_ulong t30a;			/* timer t30a value */
} mtp_opt_conf_ls_t;

/*
 *  Route options
 */
typedef struct mtp_opt_conf_rt {
	/* route timers */
	mtp_ulong t6;			/* timer t6 value */
	mtp_ulong t10;			/* timer t10 value */
} mtp_opt_conf_rt_t;

/*
 *  Route list options
 */
typedef struct mtp_opt_conf_rl {
	/* route timers */
	mtp_ulong t6;			/* timer t6 value */
	mtp_ulong t10;			/* timer t10 value */
} mtp_opt_conf_rl_t;

/*
 *  Route set options
 */
typedef struct mtp_opt_conf_rs {
	/* route timers */
	mtp_ulong t6;			/* timer t6 value */
	mtp_ulong t10;			/* timer t10 value */
	/* route set timers */
	mtp_ulong t8;			/* timer t8 value */
	mtp_ulong t11;			/* timer t11 value */
	mtp_ulong t15;			/* timer t15 value */
	mtp_ulong t16;			/* timer t16 value */
	mtp_ulong t18a;			/* timer t18a value */
} mtp_opt_conf_rs_t;

/*
 *  Signalling point options
 */
typedef struct mtp_opt_conf_sp {
	/* signalling link timers */
	mtp_ulong t1;			/* timer t1 value */
	mtp_ulong t2;			/* timer t2 value */
	mtp_ulong t3;			/* timer t3 value */
	mtp_ulong t4;			/* timer t4 value */
	mtp_ulong t5;			/* timer t5 value */
	mtp_ulong t12;			/* timer t12 value */
	mtp_ulong t13;			/* timer t13 value */
	mtp_ulong t14;			/* timer t14 value */
	mtp_ulong t17;			/* timer t17 value */
	mtp_ulong t19a;			/* timer t19a value */
	mtp_ulong t20a;			/* timer t20a value */
	mtp_ulong t21a;			/* timer t21a value */
	mtp_ulong t22;			/* timer t22 value */
	mtp_ulong t23;			/* timer t23 value */
	mtp_ulong t24;			/* timer t24 value */
	mtp_ulong t31a;			/* timer t31a value */
	mtp_ulong t32a;			/* timer t32a value */
	mtp_ulong t33a;			/* timer t33a value */
	mtp_ulong t34a;			/* timer t34a value */
	mtp_ulong t1t;			/* timer t1t value */
	mtp_ulong t2t;			/* timer t2t value */
	mtp_ulong t1s;			/* timer t1s value */
	/* link timers */
	mtp_ulong t7;			/* timer t7 value */
	mtp_ulong t19;			/* timer t19 value */
	mtp_ulong t21;			/* timer t21 value */
	mtp_ulong t25a;			/* timer t25a value */
	mtp_ulong t28a;			/* timer t28a value */
	mtp_ulong t29a;			/* timer t29a value */
	mtp_ulong t30a;			/* timer t30a value */
	/* route timers */
	mtp_ulong t6;			/* timer t6 value */
	mtp_ulong t10;			/* timer t10 value */
	/* route set timers */
	mtp_ulong t8;			/* timer t8 value */
	mtp_ulong t11;			/* timer t11 value */
	mtp_ulong t15;			/* timer t15 value */
	mtp_ulong t16;			/* timer t16 value */
	mtp_ulong t18a;			/* timer t18a value */
	/* signalling point timers */
	mtp_ulong t1r;			/* timer t1r value */
	mtp_ulong t18;			/* timer t18 value */
	mtp_ulong t20;			/* timer t20 value */
	mtp_ulong t22a;			/* timer t22a value */
	mtp_ulong t23a;			/* timer t23a value */
	mtp_ulong t24a;			/* timer t24a value */
	mtp_ulong t26a;			/* timer t26a value */
	mtp_ulong t27a;			/* timer t27a value */
} mtp_opt_conf_sp_t;

/*
 *  Network appearance options
 */
typedef struct mtp_opt_conf_na {
	/* signalling link timers */
	mtp_ulong t1;			/* timer t1 value */
	mtp_ulong t2;			/* timer t2 value */
	mtp_ulong t3;			/* timer t3 value */
	mtp_ulong t4;			/* timer t4 value */
	mtp_ulong t5;			/* timer t5 value */
	mtp_ulong t12;			/* timer t12 value */
	mtp_ulong t13;			/* timer t13 value */
	mtp_ulong t14;			/* timer t14 value */
	mtp_ulong t17;			/* timer t17 value */
	mtp_ulong t19a;			/* timer t19a value */
	mtp_ulong t20a;			/* timer t20a value */
	mtp_ulong t21a;			/* timer t21a value */
	mtp_ulong t22;			/* timer t22 value */
	mtp_ulong t23;			/* timer t23 value */
	mtp_ulong t24;			/* timer t24 value */
	mtp_ulong t31a;			/* timer t31a value */
	mtp_ulong t32a;			/* timer t32a value */
	mtp_ulong t33a;			/* timer t33a value */
	mtp_ulong t34a;			/* timer t34a value */
	mtp_ulong t1t;			/* timer t1t value */
	mtp_ulong t2t;			/* timer t2t value */
	mtp_ulong t1s;			/* timer t1s value */
	/* link timers */
	mtp_ulong t7;			/* timer t7 value */
	mtp_ulong t19;			/* timer t19 value */
	mtp_ulong t21;			/* timer t21 value */
	mtp_ulong t25a;			/* timer t25a value */
	mtp_ulong t28a;			/* timer t28a value */
	mtp_ulong t29a;			/* timer t29a value */
	mtp_ulong t30a;			/* timer t30a value */
	/* route timers */
	mtp_ulong t6;			/* timer t6 value */
	mtp_ulong t10;			/* timer t10 value */
	/* route set timers */
	mtp_ulong t8;			/* timer t8 value */
	mtp_ulong t11;			/* timer t11 value */
	mtp_ulong t15;			/* timer t15 value */
	mtp_ulong t16;			/* timer t16 value */
	mtp_ulong t18a;			/* timer t18a value */
	/* signalling point timers */
	mtp_ulong t1r;			/* timer t1r value */
	mtp_ulong t18;			/* timer t18 value */
	mtp_ulong t20;			/* timer t20 value */
	mtp_ulong t22a;			/* timer t22a value */
	mtp_ulong t23a;			/* timer t23a value */
	mtp_ulong t24a;			/* timer t24a value */
	mtp_ulong t26a;			/* timer t26a value */
	mtp_ulong t27a;			/* timer t27a value */
} mtp_opt_conf_na_t;

/*
 *  Default options
 */
typedef struct mtp_opt_conf_df {
} mtp_opt_conf_df_t;

typedef union mtp_option_obj {
	struct mtp_opt_conf_na na;	/* Network Appearance */
	struct mtp_opt_conf_sp sp;	/* Signalling Point */
	struct mtp_opt_conf_rs rs;	/* Route Set */
	struct mtp_opt_conf_rl rl;	/* Route List */
	struct mtp_opt_conf_rt rt;	/* Route */
	struct mtp_opt_conf_ls ls;	/* Combined Link Set */
	struct mtp_opt_conf_lk lk;	/* Link Set */
	struct mtp_opt_conf_sl sl;	/* Signalling Link */
	struct mtp_opt_conf_df df;	/* Default */
} mtp_option_obj_t;

/*
 *  OPTIONS
 */
typedef struct mtp_option {
	mtp_ulong type;			/* object type */
	mtp_ulong id;			/* object id */
	/* followed by object-specific protocol options structure */
	mtp_option_obj_t options[0];
} mtp_option_t;

#define	MTP_IOCGOPTION	_IOWR(	MTP_IOC_MAGIC,	 0,	mtp_option_t	)
#define	MTP_IOCSOPTION	_IOWR(	MTP_IOC_MAGIC,	 1,	mtp_option_t	)

/*
 *  Signalling link configuration
 */
typedef struct mtp_conf_sl {
	mtp_ulong muxid;		/* lower multiplexor id */
	mtp_ulong lkid;			/* link set id */
	mtp_ulong slc;			/* signalling link code in lk */
} mtp_conf_sl_t;

/*
 *  Link set configuration
 */
typedef struct mtp_conf_lk {
	mtp_ulong lsid;			/* combined link set id */
	mtp_ulong rsid;			/* routeset of adjacent signalling point */
	mtp_ulong ni;			/* network indicator for link set */
	mtp_ulong slot;			/* slot of SLS for this link set */
} mtp_conf_lk_t;

/*
 *  Combined link set configuration
 */
typedef struct mtp_conf_ls {
	mtp_ulong spid;			/* signalling point id */
	mtp_ulong sls_mask;		/* mask of bits selecting link set */
} mtp_conf_ls_t;

/*
 *  Route configuration
 */
typedef struct mtp_conf_rt {
	mtp_ulong rlid;			/* route list id */
	mtp_ulong lkid;			/* link id */
	mtp_ulong slot;			/* slot of SLS for this route */
} mtp_conf_rt_t;

/*
 *  Route list configuration
 */
typedef struct mtp_conf_rl {
	mtp_ulong rsid;			/* route set id */
	mtp_ulong lsid;			/* combined link set id */
	mtp_ulong cost;			/* cost in routeset */
} mtp_conf_rl_t;

/*
 *  Route set configuration
 */
typedef struct mtp_conf_rs {
	mtp_ulong spid;			/* signalling point id */
	mtp_ulong dest;			/* destination point code */
	mtp_ulong flags;		/* options flags */
} mtp_conf_rs_t;

/*
 *  Signalling point configuration
 */
typedef struct mtp_conf_sp {
	mtp_ulong naid;			/* network appearance id */
	mtp_ulong pc;			/* point code */
	mtp_ulong users;		/* mask of equipped users */
	mtp_ulong flags;		/* options flags */
} mtp_conf_sp_t;

/*
 *  Network appearance configuration
 */
typedef struct mtp_conf_na {
	lmi_option_t options;		/* protocol options */
	struct {
		mtp_ulong member;	/* PC member mask */
		mtp_ulong cluster;	/* PC cluster mask */
		mtp_ulong network;	/* PC network mask */
	} mask;
	mtp_ulong sls_bits;		/* bits in SLS */
} mtp_conf_na_t;

/*
   additional MTP protocol options 
 */
#define SS7_POPT_TFR	0x00010000	/* old broadcast method - no responsive */
#define SS7_POPT_TFRB	0x00020000	/* new broadcast method - no regulation */
#define SS7_POPT_TFRR	0x00040000	/* new responsive method - regulated */
#define SS7_POPT_MCSTA	0x00080000	/* multiple congestion states */

/*
 *  Default configuration
 */
typedef struct mtp_conf_df {
} mtp_conf_df_t;

typedef union mtp_conf_obj {
	struct mtp_conf_na na;		/* Network Appearance */
	struct mtp_conf_sp sp;		/* Signalling Point */
	struct mtp_conf_rs rs;		/* Route Set */
	struct mtp_conf_rl rl;		/* Route List */
	struct mtp_conf_rt rt;		/* Route */
	struct mtp_conf_ls ls;		/* Combined Link Set */
	struct mtp_conf_lk lk;		/* Link Set */
	struct mtp_conf_sl sl;		/* Signalling Link */
	struct mtp_conf_df df;		/* Default */
} mtp_conf_obj_t;

/*
 *  CONFIGURATION
 */
typedef struct mtp_config {
	mtp_ulong type;			/* object type */
	mtp_ulong id;			/* object id */
	mtp_ulong cmd;			/* configuration command */
	/* followed by object-specific configuration structure */
	mtp_conf_obj_t config[0];
} mtp_config_t;

#define MTP_GET		0	/* get configuration */
#define MTP_ADD		1	/* add configuration */
#define MTP_CHA		2	/* cha configuration */
#define MTP_DEL		3	/* del configuration */

#define	MTP_IOCGCONFIG	_IOWR(	MTP_IOC_MAGIC,	 2,	mtp_config_t	)
#define	MTP_IOCSCONFIG	_IOWR(	MTP_IOC_MAGIC,	 3,	mtp_config_t	)
#define	MTP_IOCTCONFIG	_IOWR(	MTP_IOC_MAGIC,	 4,	mtp_config_t	)
#define	MTP_IOCCCONFIG	_IOWR(	MTP_IOC_MAGIC,	 5,	mtp_config_t	)

/*
 *  Common state.
 */
/*
   primary states 
 */
#define MTP_ALLOWED		0x00UL	/* Entity Allowed */
#define MTP_DANGER		0x01UL	/* Entity Danger of congestion (primary or secondary) */
#define MTP_CONGESTED		0x02UL	/* Entity Congested (Link Set congestion, primary or
					   secondary ) */
#define MTP_RESTRICTED		0x03UL	/* Entity Restricted (Route Failure or received TFR) */
#define MTP_RESTART		0x04UL	/* Entity Restarting */
#define MTP_PROHIBITED		0x05UL	/* Entity Prohibited (Received TFP) */
#define MTP_INHIBITED		0x06UL	/* Entity Inhibited (Management inhibited) */
#define MTP_BLOCKED		0x07UL	/* Entity Blocked (Local Link Set failure) */
#define MTP_INACTIVE		0x08UL	/* Entity Inactive (Link out of service) */

/*
   pseudo-states 
 */
#define MTP_NODANGER		0x11UL	/* Entity Out of Danger (transient state) */
#define MTP_UNCONGESTED		0x12UL	/* Entity Uncongested (transient state) */
#define MTP_RESTARTED		0x14UL	/* Entity Restarted */
#define MTP_UNINHIBITED		0x16UL	/* Entity Uninhibited (transient state) */
#define MTP_UNBLOCKED		0x17UL	/* Entity Unblocked (transient state) */
#define MTP_ACTIVE		0x18UL	/* Entity Active (Link in service) */
#define MTP_RESTART_PHASE_1	0x24UL	/* Entity Restarting Phase 1 */
#define MTP_RESTART_PHASE_2	0x34UL	/* Entity Restarting Phase 2 */

/*
   state flags 
 */
#define MTPF_ALLOWED		(1<< MTP_ALLOWED)	/* Entity is allowed */
#define MTPF_DANGER		(1<< MTP_DANGER)	/* Entity is in danger of congestion */
#define MTPF_CONGESTED		(1<< MTP_CONGESTED)	/* Entity is congested */
#define MTPF_RESTRICTED		(1<< MTP_RESTRICTED)	/* Entity is restricted */
#define MTPF_RESTART		(1<< MTP_RESTART)	/* Entity is restarting */
#define MTPF_PROHIBITED		(1<< MTP_PROHIBITED)	/* Entity is prohibited */
#define MTPF_INHIBITED		(1<< MTP_INHIBITED)	/* Entity is inhibited */
#define MTPF_BLOCKED		(1<< MTP_BLOCKED)	/* Entity is blocked */
#define MTPF_INACTIVE		(1<< MTP_INACTIVE)	/* Entity is inactive */

/*
   additional state flags 
 */
#define MTPF_TRAFFIC		(1<< 9)	/* Entity has sent traffic */
#define MTPF_COO_RECV		(1<<10)	/* Entity has received a COO */
#define MTPF_ECO_RECV		(1<<11)	/* Entity has received a ECO */
#define MTPF_WACK_SLTM		(1<<12)	/* Entity waiting for response to 1st SLTM */
#define MTPF_WACK_SLTM2		(1<<13)	/* Entity waiting for response to 2nd SLTM */
#define MTPF_WACK_SSLTM		(1<<14)	/* Entity waiting for response to 1st SSLTM */
#define MTPF_WACK_SSLTM2	(1<<15)	/* Entity waiting for response to 2nd SSLTM */
#define MTPF_TFR_PENDING	(1<<16)	/* Entity has TFR pending */

/*
   non-state (options) flags 
 */
#define MTPF_CLUSTER		(1<<17)	/* Entity is cluster route */
#define MTPF_XFER_FUNC		(1<<18)	/* Entity has transfer function */
#define MTPF_SECURITY		(1<<19)	/* Entity has additional security procedures */
#define MTPF_ADJACENT		(1<<20)	/* Entity is adjacent */
#define MTPF_LOSC_PROC_A	(1<<21)	/* Entity uses link oscillation procedure A */
#define MTPF_LOSC_PROC_B	(1<<22)	/* Entity uses link oscillation procedure B */
#define MTPF_RESTART_PHASE_1	(1<<23)	/* Entity restarting phase 1 */
#define MTPF_RESTART_PHASE_2	(1<<24)	/* Entity restarting phase 2 */
#define MTPF_RESTART_LOCKOUT	(1<<25)	/* Entity restarting but locked out (T27a) */

/*
 *  Signalling link state
 */
typedef struct mtp_timers_sl {
	mtp_timer_t t1;			/* timer t1 */
	mtp_timer_t t2;			/* timer t2 */
	mtp_timer_t t3;			/* timer t3 */
	mtp_timer_t t4;			/* timer t4 */
	mtp_timer_t t5;			/* timer t5 */
	mtp_timer_t t12;		/* timer t12 */
	mtp_timer_t t13;		/* timer t13 */
	mtp_timer_t t14;		/* timer t14 */
	mtp_timer_t t17;		/* timer t17 */
	mtp_timer_t t19a;		/* timer t19a */
	mtp_timer_t t20a;		/* timer t20a */
	mtp_timer_t t21a;		/* timer t21a */
	mtp_timer_t t22;		/* timer t22 */
	mtp_timer_t t23;		/* timer t23 */
	mtp_timer_t t24;		/* timer t24 */
	mtp_timer_t t31a;		/* timer t31a */
	mtp_timer_t t32a;		/* timer t32a */
	mtp_timer_t t33a;		/* timer t33a */
	mtp_timer_t t34a;		/* timer t34a */
	mtp_timer_t t1t;		/* timer t1t */
	mtp_timer_t t2t;		/* timer t2t */
	mtp_timer_t t1s;		/* timer t1s */
} mtp_timers_sl_t;
typedef struct mtp_statem_sl {
	struct mtp_timers_sl timers;
} mtp_statem_sl_t;

#define SLS_OUT_OF_SERVICE	0	/* out of service */
#define SLS_PROC_OUTG		1	/* processor outage */
#define SLS_IN_SERVICE		2	/* in service */
#define SLS_WACK_COO		3	/* waiting COA/ECA in response to COO */
#define SLS_WACK_ECO		4	/* waiting COA/ECA in response to ECO */
#define SLS_WCON_RET		5	/* waiting for retrieval confrimation */
#define SLS_WIND_CLRB		6	/* waiting for clear buffers indication */
#define SLS_WIND_BSNT		7	/* waiting for BSNT indication */
#define SLS_WIND_INSI		8	/* waiting for in service indication */
#define SLS_WACK_SLTM		9	/* waiting SLTA in response to 1st SLTM */

#define SL_RESTORED	    (MTP_ALLOWED)	/* Sig link Activated/Restored/Resumed */
#define SL_DANGER	    (MTP_DANGER)	/* Sig link Danger of congestion (overloaded) */
#define SL_CONGESTED	    (MTP_CONGESTED)	/* Sig link Congested (link congestion) */
#define SL_UNUSABLE	    (MTP_RESTRICTED)	/* Sig link Unusable (Local Processor Outage) */
#define SL_RETRIEVAL	    (MTP_RESTART)	/* Sig link Retrieving */
#define SL_FAILED	    (MTP_PROHIBITED)	/* Sig link Failed */
#define SL_INHIBITED	    (MTP_INHIBITED)	/* Sig link Inhibited (Management inhibited) */
#define SL_BLOCKED	    (MTP_BLOCKED)	/* Sig link Blocked (Processor Outage) */
#define SL_INACTIVE	    (MTP_INACTIVE)	/* Sig link Inactive (Out of Service) */
#define SL_NODANGER	    (MTP_NODANGER)	/* Sig link Out of Danger (transient state) */
#define SL_UNCONGESTED	    (MTP_UNCONGESTED)	/* Sig link Uncongested (transient state) */
#define SL_UPDATED	    (MTP_RESTARTED)	/* Sig link Buffer Update Complete (transient
						   state) */
#define SL_UNINHIBITED	    (MTP_UNINHIBITED)	/* Sig link Uninhibited (transient state) */
#define SL_UNBLOCKED	    (MTP_UNBLOCKED)	/* Sig link Unblocked (transient state) */
#define SL_ACTIVE	    (MTP_ACTIVE)	/* Sig link Active (Link in service) */

#define SLF_TRAFFIC	    (MTPF_TRAFFIC)	/* Sig link has sent traffic */
#define SLF_COO_RECV	    (MTPF_COO_RECV)	/* Sig link has received a COO */
#define SLF_ECO_RECV	    (MTPF_ECO_RECV)	/* Sig link has received a ECO */
#define SLF_WACK_SLTM	    (MTPF_WACK_SLTM)	/* Sig link waiting for response to 1st SLTM */
#define SLF_WACK_SLTM2	    (MTPF_WACK_SLTM2)	/* Sig link waiting for response to 2nd SLTM */
#define SLF_WACK_SSLTM	    (MTPF_WACK_SSLTM)	/* Sig link waiting for response to 1st SSLTM */
#define SLF_WACK_SSLTM2	    (MTPF_WACK_SSLTM2)	/* Sig link waiting for response to 2nd SSLTM */

#define SLF_RESTORED	    (MTPF_ALLOWED)	/* Sig link Activated/Restored */
#define SLF_DANGER	    (MTPF_DANGER)	/* Sig link Danger of congestion (overloaded) */
#define SLF_CONGESTED	    (MTPF_CONGESTED)	/* Sig link Congested (link congestion) */
#define SLF_UNUSABLE	    (MTPF_RESTRICTED)	/* Sig link Unusable (Local Processor Outage) */
#define SLF_RETRIEVAL	    (MTPF_RESTART)	/* Sig link Retrieving */
#define SLF_FAILED	    (MTPF_PROHIBITED)	/* Sig link Failed */
#define SLF_INHIBITED	    (MTPF_INHIBITED)	/* Sig link Inhibited (Management inhibited) */
#define SLF_BLOCKED	    (MTPF_BLOCKED)	/* Sig link Blocked (Processor Outage) */
#define SLF_INACTIVE	    (MTPF_INACTIVE)	/* Sig link Inactive (Out of Service) */

#define SLF_LOSC_PROC_A	    (MTPF_LOSC_PROC_A)	/* Sig link uses link oscillation procedure A */
#define SLF_LOSC_PROC_B	    (MTPF_LOSC_PROC_B)	/* Sig link uses link oscillation procedure B */
#define SLF_RESTART_LOCKOUT (MTPF_RESTART_LOCKOUT)	/* Sig link restart lockout (T27a running) */

/*
 *  Link set state
 */
typedef struct mtp_timers_lk {
	mtp_timer_t t7;			/* timer t7 */
	mtp_timer_t t19;		/* timer t19 */
	mtp_timer_t t21;		/* timer t21 */
	mtp_timer_t t25a;		/* timer t25a */
	mtp_timer_t t28a;		/* timer t28a */
	mtp_timer_t t29a;		/* timer t29a */
	mtp_timer_t t30a;		/* timer t30a */
} mtp_timers_lk_t;
typedef struct mtp_statem_lk {
	struct mtp_timers_lk timers;
} mtp_statem_lk_t;

#define LK_ALLOWED	    (MTP_ALLOWED)	/* Link Set Allowed */
#define LK_DANGER	    (MTP_DANGER)	/* Link Set Danger of congestion (primary or
						   secondary) */
#define LK_CONGESTED	    (MTP_CONGESTED)	/* Link Set Congested (Link Set congestion, primary 
						   or secondary ) */
#define LK_RESTRICTED	    (MTP_RESTRICTED)	/* Link Set Restricted (Route Failure or received
						   TFR) */
#define LK_RESTART	    (MTP_RESTART)	/* Link Set Restarting */
#define LK_PROHIBITED	    (MTP_PROHIBITED)	/* Link Set Prohibited (Received TFP) */
#define LK_INHIBITED	    (MTP_INHIBITED)	/* Link Set Inhibited (Management inhibited) */
#define LK_BLOCKED	    (MTP_BLOCKED)	/* Link Set Blocked (Local Link Set failure) */
#define LK_INACTIVE	    (MTP_INACTIVE)	/* Link Set Inactive (Link out of service) */
#define LK_NODANGER	    (MTP_NODANGER)	/* Link Set Out of Danger (transient state) */
#define LK_UNCONGESTED	    (MTP_UNCONGESTED)	/* Link Set Uncongested (transient state) */
#define LK_RESTARTED	    (MTP_RESTARTED)	/* Link Set Restarted */
#define LK_UNINHIBITED	    (MTP_UNINHIBITED)	/* Link Set Uninhibited (transient state) */
#define LK_UNBLOCKED	    (MTP_UNBLOCKED)	/* Link Set Unblocked (transient state) */
#define LK_ACTIVE	    (MTP_ACTIVE)	/* Link Set Active (Link in service) */

#define LKF_ALLOWED	    (MTPF_ALLOWED)	/* Link Set is active */
#define LKF_DANGER	    (MTPF_DANGER)	/* Link Set is in danger of congestion */
#define LKF_CONGESTED	    (MTPF_CONGESTED)	/* Link Set is congested */
#define LKF_UNUSABLE	    (MTPF_RESTRICTED)	/* Link Set is unusable (local processor outage) */
#define LKF_RESTART	    (MTPF_RESTART)	/* Link Set is restarting */
#define LKF_FAILED	    (MTPF_PROHIBITED)	/* Link Set is failed */
#define LKF_INHIBITED	    (MTPF_INHIBITED)	/* Link Set is inhibited (management inhibited) */
#define LKF_BLOCKED	    (MTPF_BLOCKED)	/* Link Set is blocked (remote processor outage) */
#define LKF_INACTIVE	    (MTPF_INACTIVE)	/* Link Set is out of service */

/*
 *  Combined link set state
 */
typedef struct mtp_timers_ls {
} mtp_timers_ls_t;
typedef struct mtp_statem_ls {
	struct mtp_timers_ls timers;
} mtp_statem_ls_t;

#define LS_ALLOWED	    (MTP_ALLOWED)	/* Linkset Allowed */
#define LS_DANGER	    (MTP_DANGER)	/* Linkset Danger of congestion (primary or
						   secondary) */
#define LS_CONGESTED	    (MTP_CONGESTED)	/* Linkset Congested (Link Set congestion, primary
						   or secondary ) */
#define LS_RESTRICTED	    (MTP_RESTRICTED)	/* Linkset Restricted (Route Failure or received
						   TFR) */
#define LS_RESTART	    (MTP_RESTART)	/* Linkset Restarting */
#define LS_PROHIBITED	    (MTP_PROHIBITED)	/* Linkset Prohibited (Received TFP) */
#define LS_INHIBITED	    (MTP_INHIBITED)	/* Linkset Inhibited (Management inhibited) */
#define LS_BLOCKED	    (MTP_BLOCKED)	/* Linkset Blocked (Local Link Set failure) */
#define LS_INACTIVE	    (MTP_INACTIVE)	/* Linkset Inactive (Link out of service) */
#define LS_NODANGER	    (MTP_NODANGER)	/* Linkset Out of Danger (transient state) */
#define LS_UNCONGESTED	    (MTP_UNCONGESTED)	/* Linkset Uncongested (transient state) */
#define LS_RESTARTED	    (MTP_RESTARTED)	/* Linkset Restarted */
#define LS_UNINHIBITED	    (MTP_UNINHIBITED)	/* Linkset Uninhibited (transient state) */
#define LS_UNBLOCKED	    (MTP_UNBLOCKED)	/* Linkset Unblocked (transient state) */
#define LS_ACTIVE	    (MTP_ACTIVE)	/* Linkset Active (Link in service) */

/*
 *  Route state
 */
typedef struct mtp_timers_rt {
	mtp_timer_t t6;			/* timer t6 */
	mtp_timer_t t10;		/* timer t10 */
} mtp_timers_rt_t;
typedef struct mtp_statem_rt {
	struct mtp_timers_rt timers;
} mtp_statem_rt_t;

#define RT_ALLOWED	    (MTP_ALLOWED)	/* Route Allowed */
#define RT_DANGER	    (MTP_DANGER)	/* Route Danger of congestion (primary or
						   secondary) */
#define RT_CONGESTED	    (MTP_CONGESTED)	/* Route Congested (Link Set congestion, primary or 
						   secondary ) */
#define RT_RESTRICTED	    (MTP_RESTRICTED)	/* Route Restricted (Route Failure or received TFR) 
						 */
#define RT_RESTART	    (MTP_RESTART)	/* Route Restarting */
#define RT_PROHIBITED	    (MTP_PROHIBITED)	/* Route Prohibited (Received TFP) */
#define RT_INHIBITED	    (MTP_INHIBITED)	/* Route Inhibited (Management inhibited) */
#define RT_BLOCKED	    (MTP_BLOCKED)	/* Route Blocked (Local Link Set failure) */
#define RT_INACTIVE	    (MTP_INACTIVE)	/* Route Inactive (Link out of service) */
#define RT_NODANGER	    (MTP_NODANGER)	/* Route Out of Danger (transient state) */
#define RT_UNCONGESTED      (MTP_UNCONGESTED)	/* Route Uncongested (transient state) */
#define RT_RESTARTED	    (MTP_RESTARTED)	/* Route Restarted */
#define RT_UNINHIBITED      (MTP_UNINHIBITED)	/* Route Uninhibited (transient state) */
#define RT_UNBLOCKED	    (MTP_UNBLOCKED)	/* Route Unblocked (transient state) */
#define RT_ACTIVE	    (MTP_ACTIVE)	/* Route Active (Link in service) */
// #define RT_RESTART_PHASE_1 (MTP_RESTART_PHASE_1) /* Route Restarting Phase 1 */
// #define RT_RESTART_PHASE_2 (MTP_RESTART_PHASE_2) /* Route Restarting Phase 2 */

#define RTF_ALLOWED	    (MTPF_ALLOWED)	/* Route is allowed */
#define RTF_DANGER	    (MTPF_DANGER)	/* Route is in danger of congestion */
#define RTF_CONGESTED	    (MTPF_CONGESTED)	/* Route is congested */
#define RTF_RESTRICTED	    (MTPF_RESTRICTED)	/* Route is restricted */
#define RTF_RESTART	    (MTPF_RESTART)	/* Route is restarting */
#define RTF_PROHIBITED	    (MTPF_PROHIBITED)	/* Route is prohibited */
#define RTF_INHIBITED	    (MTPF_INHIBITED)	/* Route is inhibited */
#define RTF_BLOCKED	    (MTPF_BLOCKED)	/* Route is blocked */
#define RTF_INACTIVE	    (MTPF_INACTIVE)	/* Route is inactive */

/*
 *  Route list state
 */
typedef struct mtp_timers_rl {
} mtp_timers_rl_t;
typedef struct mtp_statem_rl {
	struct mtp_timers_rl timers;
} mtp_statem_rl_t;

// #define RL_ALLOWED (MTP_ALLOWED) /* Routelist Allowed */
// #define RL_DANGER (MTP_DANGER) /* Routelist Danger of congestion (primary or secondary) */
// #define RL_CONGESTED (MTP_CONGESTED) /* Routelist Congested (Link Set cong, primary or
// secondary) */
#define RL_RESTRICTED	    (MTP_RESTRICTED)	/* Routelist Restricted (Route Failure or received
						   TFR) */
#define RL_RESTART	    (MTP_RESTART)	/* Routelist Restarting */
// #define RL_PROHIBITED (MTP_PROHIBITED) /* Routelist Prohibited (Received TFP) */
// #define RL_INHIBITED (MTP_INHIBITED) /* Routelist Inhibited (Management inhibited) */
// #define RL_BLOCKED (MTP_BLOCKED) /* Routelist Blocked (Local Link Set failure) */
// #define RL_INACTIVE (MTP_INACTIVE) /* Routelist Inactive (Link out of service) */
// #define RL_NODANGER (MTP_NODANGER) /* Routelist Out of Danger (transient state) */
// #define RL_UNCONGESTED (MTP_UNCONGESTED) /* Routelist Uncongested (transient state) */
// #define RL_RESTARTED (MTP_RESTARTED) /* Routelist Restarted */
// #define RL_UNINHIBITED (MTP_UNINHIBITED) /* Routelist Uninhibited (transient state) */
// #define RL_UNBLOCKED (MTP_UNBLOCKED) /* Routelist Unblocked (transient state) */
// #define RL_ACTIVE (MTP_ACTIVE) /* Routelist Active (Link in service) */
// #define RL_RESTART_PHASE_1 (MTP_RESTART_PHASE_1) /* Routelist Restarting Phase 1 */
// #define RL_RESTART_PHASE_2 (MTP_RESTART_PHASE_2) /* Routelist Restarting Phase 2 */

/*
 *  Route set state
 */
typedef struct mtp_timers_rs {
	mtp_timer_t t8;			/* timer t8 */
	mtp_timer_t t11;		/* timer t11 */
	mtp_timer_t t15;		/* timer t15 */
	mtp_timer_t t16;		/* timer t16 */
	mtp_timer_t t18a;		/* timer t18a */
} mtp_timers_rs_t;
typedef struct mtp_statem_rs {
	struct mtp_timers_rs timers;
} mtp_statem_rs_t;

#define RS_ALLOWED	    (MTP_ALLOWED)	/* Routeset Allowed */
#define RS_DANGER	    (MTP_DANGER)	/* Routeset Danger of congestion (primary or
						   secondary) */
#define RS_CONGESTED	    (MTP_CONGESTED)	/* Routeset Congested (Link Set cong, primary or
						   secondary) */
#define RS_RESTRICTED	    (MTP_RESTRICTED)	/* Routeset Restricted (Route Failure or received
						   TFR) */
#define RS_RESTART	    (MTP_RESTART)	/* Routeset Restarting */
#define RS_PROHIBITED	    (MTP_PROHIBITED)	/* Routeset Prohibited (Received TFP) */
#define RS_INHIBITED	    (MTP_INHIBITED)	/* Routeset Inhibited (Management inhibited) */
#define RS_BLOCKED	    (MTP_BLOCKED)	/* Routeset Blocked (Local Link Set failure) */
#define RS_INACTIVE	    (MTP_INACTIVE)	/* Routeset Inactive (Link out of service) */
#define RS_NODANGER	    (MTP_NODANGER)	/* Routeset Out of Danger (transient state) */
#define RS_UNCONGESTED	    (MTP_UNCONGESTED)	/* Routeset Uncongested (transient state) */
#define RS_RESTARTED	    (MTP_RESTARTED)	/* Routeset Restarted */
#define RS_UNINHIBITED	    (MTP_UNINHIBITED)	/* Routeset Uninhibited (transient state) */
#define RS_UNBLOCKED	    (MTP_UNBLOCKED)	/* Routeset Unblocked (transient state) */
#define RS_ACTIVE	    (MTP_ACTIVE)	/* Routeset Active (Link in service) */
#define RS_RESTART_PHASE_1  (MTP_RESTART_PHASE_1)	/* Routeset Restarting Phase 1 */
#define RS_RESTART_PHASE_2  (MTP_RESTART_PHASE_2)	/* Routeset Restarting Phase 2 */

#define RSF_ALLOWED	    (MTPF_ALLOWED)	/* Routeset is allowed */
#define RSF_DANGER	    (MTPF_DANGER)	/* Routeset is in danger of congestion */
#define RSF_CONGESTED	    (MTPF_CONGESTED)	/* Routeset is congested */
#define RSF_RESTRICTED	    (MTPF_RESTRICTED)	/* Routeset is restricted */
#define RSF_RESTART	    (MTPF_RESTART)	/* Routeset is restarting */
#define RSF_PROHIBITED	    (MTPF_PROHIBITED)	/* Routeset is prohibited */
#define RSF_INHIBITED	    (MTPF_INHIBITED)	/* Routeset is inhibited */
#define RSF_BLOCKED	    (MTPF_BLOCKED)	/* Routeset is blocked */
#define RSF_INACTIVE	    (MTPF_INACTIVE)	/* Routeset is inactive */

#define RSF_TFR_PENDING	    (MTPF_TFR_PENDING)	/* Routeset has TFR pending */
#define RSF_CLUSTER	    (MTPF_CLUSTER)	/* Routeset is cluster route */
#define RSF_XFER_FUNC	    (MTPF_XFER_FUNC)	/* Routeset has transfer function */
#define RSF_ADJACENT	    (MTPF_ADJACENT)	/* Routeset is adjacent */

/*
 *  Signalling point state
 */
typedef struct mtp_timers_sp {
	mtp_timer_t t1r;		/* timer t1r */
	mtp_timer_t t18;		/* timer t18 */
	mtp_timer_t t20;		/* timer t20 */
	mtp_timer_t t22a;		/* timer t22a */
	mtp_timer_t t23a;		/* timer t23a */
	mtp_timer_t t24a;		/* timer t24a */
	mtp_timer_t t26a;		/* timer t26a */
	mtp_timer_t t27a;		/* timer t27a */
} mtp_timers_sp_t;
typedef struct mtp_statem_sp {
	struct mtp_timers_sp timers;
} mtp_statem_sp_t;

#define SP_ALLOWED	    (MTP_ALLOWED)	/* Sig Point Allowed */
#define SP_DANGER	    (MTP_DANGER)	/* Sig Point Danger of congestion (primary or
						   secondary) */
#define SP_CONGESTED	    (MTP_CONGESTED)	/* Sig Point Congested (Link Set cong, primary or
						   secondary ) */
#define SP_RESTRICTED	    (MTP_RESTRICTED)	/* Sig Point Restricted (Route Failure or received
						   TFR) */
#define SP_RESTART	    (MTP_RESTART)	/* Sig Point Restarting */
#define SP_PROHIBITED	    (MTP_PROHIBITED)	/* Sig Point Prohibited (Received TFP) */
#define SP_INHIBITED	    (MTP_INHIBITED)	/* Sig Point Inhibited (Management inhibited) */
#define SP_BLOCKED	    (MTP_BLOCKED)	/* Sig Point Blocked (Local Link Set failure) */
#define SP_INACTIVE	    (MTP_INACTIVE)	/* Sig Point Inactive (Link out of service) */
#define SP_NODANGER	    (MTP_NODANGER)	/* Sig Point Out of Danger (transient state) */
#define SP_UNCONGESTED	    (MTP_UNCONGESTED)	/* Sig Point Uncongested (transient state) */
#define SP_RESTARTED	    (MTP_RESTARTED)	/* Sig Point Restarted */
#define SP_UNINHIBITED	    (MTP_UNINHIBITED)	/* Sig Point Uninhibited (transient state) */
#define SP_UNBLOCKED	    (MTP_UNBLOCKED)	/* Sig Point Unblocked (transient state) */
#define SP_ACTIVE	    (MTP_ACTIVE)	/* Sig Point Active (Link in service) */
#define SP_RESTART_PHASE_1  (MTP_RESTART_PHASE_1)	/* Sig Point Restarting Phase 1 */
#define SP_RESTART_PHASE_2  (MTP_RESTART_PHASE_2)	/* Sig Point Restarting Phase 2 */

#define SPF_RESTART	    (MTPF_RESTART)	/* Sig Point restarting */
#define SPF_CLUSTER	    (MTPF_CLUSTER)	/* Sig Point is cluster route */
#define SPF_XFER_FUNC	    (MTPF_XFER_FUNC)	/* Sig Point has transfer function */
#define SPF_SECURITY	    (MTPF_SECURITY)	/* Sig Point has additional security procedures */
#define SPF_LOSC_PROC_A	    (MTPF_LOSC_PROC_A)	/* Sig Point uses link oscillation procedure A */
#define SPF_LOSC_PROC_B	    (MTPF_LOSC_PROC_B)	/* Sig Point uses link oscillation procedure B */
#define SPF_RESTART_PHASE_1 (MTPF_RESTART_PHASE_1)	/* Sig Point restarting */
#define SPF_RESTART_PHASE_2 (MTPF_RESTART_PHASE_2)	/* Sig Point restarting */
#define SPF_RESTART_LOCKOUT (MTPF_RESTART_LOCKOUT)	/* Sig Point restarting (T27a running) */

/*
 *  Network appearance state
 */
typedef struct mtp_timers_na {
} mtp_timers_na_t;
typedef struct mtp_statem_na {
	struct mtp_timers_na timers;
} mtp_statem_na_t;

/*
 *  Default state
 */
typedef struct mtp_timers_df {
} mtp_timers_df_t;
typedef struct mtp_statem_df {
	struct mtp_timers_df timers;
} mtp_statem_df_t;

typeodef union mtp_statem_obj {
	struct mtp_statem_na na;	/* Network Appearance */
	struct mtp_statem_sp sp;	/* Signalling Point */
	struct mtp_statem_rs rs;	/* Route Set */
	struct mtp_statem_rl rl;	/* Route List */
	struct mtp_statem_rt rt;	/* Route */
	struct mtp_statem_ls ls;	/* Combined Link Set */
	struct mtp_statem_lk lk;	/* Link Set */
	struct mtp_statem_sl sl;	/* Signalling Link */
	struct mtp_statem_df df;	/* Default */
} mtp_statem_obj_t;

/*
 *  STATE
 */

typedef struct mtp_statem {
	mtp_ulong type;			/* object type */
	mtp_ulong id;			/* object id */
	mtp_ulong flags;		/* object flags */
	mtp_ulong state;		/* object state */
	/* followed by object-specific state structure */
	mtp_statem_obj_t statem[0];
} mtp_statem_t;

#define	MTP_IOCGSTATEM	_IOWR(	MTP_IOC_MAGIC,	 6,	mtp_statem_t	)
#define	MTP_IOCCMRESET	_IOWR(	MTP_IOC_MAGIC,	 7,	mtp_statem_t	)

/*
 *  Signalling link statistics
 */
typedef struct mtp_stats_sl {
} mtp_stats_sl_t;

/*
 *  Link statistics
 */
typedef struct mtp_stats_lk {
} mtp_stats_lk_t;

/*
 *  Link set statistics
 */
typedef struct mtp_stats_ls {
} mtp_stats_ls_t;

/*
 *  Route statistics
 */
typedef struct mtp_stats_rt {
} mtp_stats_rt_t;

/*
 *  Route list statistics
 */
typedef struct mtp_stats_rl {
} mtp_stats_rl_t;

/*
 *  Route set statistics
 */
typedef struct mtp_stats_rs {
} mtp_stats_rs_t;

/*
 *  Signalling point statistics
 */
typedef struct mtp_stats_sp {
} mtp_stats_sp_t;

/*
 *  Network appearance statistics
 */
typedef struct mtp_stats_na {
} mtp_stats_na_t;

/*
 *  Default statistics
 */
typedef struct mtp_stats_df {
} mtp_stats_df_t;

typedef union mtp_stats_obj {
	struct mtp_stats_na na;		/* Network Appearance */
	struct mtp_stats_sp sp;		/* Signalling Point */
	struct mtp_stats_rs rs;		/* Route Set */
	struct mtp_stats_rl rl;		/* Route List */
	struct mtp_stats_rt rt;		/* Route */
	struct mtp_stats_ls ls;		/* Combined Link Set */
	struct mtp_stats_lk lk;		/* Link Set */
	struct mtp_stats_sl sl;		/* Signalling Link */
	struct mtp_stats_df df;		/* Default */
} mtp_stats_obj_t;

/*
 *  STATISTICS
 */
typedef struct mtp_stats {
	mtp_ulong type;			/* object type */
	mtp_ulong id;			/* object id */
	mtp_ulong header;		/* object stats header */
	/* followed by object-specific statistics structure */
	mtp_stats_obj_t stats[0];
} mtp_stats_t;

#define	MTP_IOCGSTATSP	_IOWR(	MTP_IOC_MAGIC,	 8,	mtp_stats_t	)
#define	MTP_IOCSSTATSP	_IOWR(	MTP_IOC_MAGIC,	 9,	mtp_stats_t	)
#define	MTP_IOCGSTATS	_IOWR(	MTP_IOC_MAGIC,	10,	mtp_stats_t	)
#define	MTP_IOCSSTATS	_IOWR(	MTP_IOC_MAGIC,	11,	mtp_stats_t	)

/*
 *  Signalling link notifications
 */
typedef struct mtp_notify_sl {
	mtp_ulong events;
} mtp_notify_sl_t;

#define MTP_EVT_SL_RPO_START			(1<< 0)	/* Table 2/Q.752 2.10 */
#define MTP_EVT_SL_RPO_STOP			(1<< 1)	/* Table 2/Q.752 2.10 */
#define MTP_EVT_SL_LOC_INHIB_START		(1<< 2)	/* Table 2/Q.752 2.10 */
#define MTP_EVT_SL_LOC_INHIB_STOP		(1<< 3)	/* Table 2/Q.752 2.10 */
#define MTP_EVT_SL_REM_INHIB_START		(1<< 4)	/* Table 2/Q.752 2.10 */
#define MTP_EVT_SL_REM_INHIB_STOP		(1<< 5)	/* Table 2/Q.752 2.10 */
#define MTP_EVT_SL_CONG_START			(1<< 6)	/* Table 2/Q.752 2.10 */
#define MTP_EVT_SL_CONG_STOP			(1<< 7)	/* Table 2/Q.752 2.10 */
#define MTP_EVT_SL_CONG_LOSS			(1<< 8)	/* Table 2/Q.752 2.10 */

/*
 *  Link set notifications
 */
typedef struct mtp_notify_lk {
	mtp_ulong events;
} mtp_notify_lk_t;

#define MTP_EVT_LK_FAILURE_START		(1<< 9)
#define MTP_EVT_LK_FAILURE_STOP			(1<<10)
#define MTP_EVT_LK_SEND_TFP			(1<<11)
#define MTP_EVT_LK_SEND_TFA			(1<<12)

/*
 *  Combined link set notifications
 */
typedef struct mtp_notify_ls {
	mtp_ulong events;
} mtp_notify_ls_t;

/*
 *  Route notifications
 */
typedef struct mtp_notify_rt {
	mtp_ulong events;
} mtp_notify_rt_t;

/*
 *  Route list notifications
 */
typedef struct mtp_notify_rl {
	mtp_ulong events;
} mtp_notify_rl_t;

/*
 *  Route set notifications
 */
typedef struct mtp_notify_rs {
	mtp_ulong events;
} mtp_notify_rs_t;

#define MTP_EVT_RS_UNAVAIL_START		(1<<13)
#define MTP_EVT_RS_UNAVAIL_STOP			(1<<14)
#define MTP_EVT_RS_REROUTE_ADJ			(1<<15)
#define MTP_EVT_RS_ADJ_SP_INACCESS_START	(1<<16)
#define MTP_EVT_RS_ADJ_SP_INACCESS_STOP		(1<<17)
#define MTP_EVT_RS_TFC_RECV			(1<<18)

/*
 *  Signalling point notifications
 */
typedef struct mtp_notify_sp {
	mtp_ulong events;
} mtp_notify_sp_t;

#define MTP_EVT_SP_MSU_DISCARDED		(1<<19)
#define MTP_EVT_SP_UPU_SENT			(1<<20)
#define MTP_EVT_SP_UPU_RECV			(1<<21)

/*
 *  Network appearance notifications
 */
typedef struct mtp_notify_na {
	mtp_ulong events;
} mtp_notify_na_t;

/*
 *  Default notifications
 */
typedef struct mtp_notify_df {
	mtp_ulong events;
} mtp_notify_df_t;

typedef union mtp_notify_obj {
	struct mtp_notify_na na;	/* Network Appearance */
	struct mtp_notify_sp sp;	/* Signalling Point */
	struct mtp_notify_rs rs;	/* Route Set */
	struct mtp_notify_rl rl;	/* Route List */
	struct mtp_notify_rt rt;	/* Route */
	struct mtp_notify_ls ls;	/* Combined Link Set */
	struct mtp_notify_lk lk;	/* Link Set */
	struct mtp_notify_sl sl;	/* Signalling Link */
	struct mtp_notify_df df;	/* Default */
} mtp_notify_obj_t;

/*
 *  EVENTS
 */
typedef struct mtp_notify {
	mtp_ulong type;			/* object type */
	mtp_ulong id;			/* object id */
	/* followed by object-specific notification structure */
	mtp_notify_obj_t events[0];
} mtp_notify_t;

#define	MTP_IOCGNOTIFY	_IOWR(	MTP_IOC_MAGIC,	12,	mtp_notify_t	)
#define	MTP_IOCSNOTIFY	_IOWR(	MTP_IOC_MAGIC,	13,	mtp_notify_t	)
#define	MTP_IOCCNOTIFY	_IOWR(	MTP_IOC_MAGIC,	14,	mtp_notify_t	)

/*
 *  MANAGEMENT
 */
typedef struct mtp_mgmt {
	mtp_ulong type;			/* object type */
	mtp_ulong id;			/* object id */
	mtp_ulong cmd;			/* mgmt command */
} mtp_mgmt_t;

#define MTP_MGMT_ALLOW			 0
#define MTP_MGMT_RESTRICT		 1
#define MTP_MGMT_PROHIBIT		 2
#define MTP_MGMT_ACTIVATE		 3
#define MTP_MGMT_DEACTIVATE		 4
#define MTP_MGMT_BLOCK			 5
#define MTP_MGMT_UNBLOCK		 6
#define MTP_MGMT_INHIBIT		 7
#define MTP_MGMT_UNINHIBIT		 8
#define MTP_MGMT_CONGEST		 9
#define MTP_MGMT_UNCONGEST		10
#define MTP_MGMT_DANGER			11
#define MTP_MGMT_NODANGER		12
#define MTP_MGMT_RESTART		13
#define MTP_MGMT_RESTARTED		14

#define	MTP_IOCCMGMT	_IOW(	MTP_IOC_MAGIC,	15,	mtp_mgmt_t	)

/*
 *  PASS LOWER
 */
typedef struct mtp_pass {
	mtp_ulong muxid;		/* mux index of lower SL structure to pass message to */
	mtp_ulong type;			/* type of message block */
	mtp_ulong band;			/* band of mesage block */
	mtp_ulong ctl_length;		/* length of cntl part */
	mtp_ulong dat_length;		/* length of data part */
	/* followed by cntl and data part of message to pass to signalling link */
} mtp_pass_t;

#define MTP_IOCCPASS	_IOW(	MTP_IOC_MAGIC,	16,	mtp_pass_t	)

#define MTP_IOC_FIRST    0
#define MTP_IOC_LAST    16
#define MTP_IOC_PRIVATE 32

#endif				/* __MTPI_IOCTL_H__ */
