/*****************************************************************************

 @(#) src/include/ss7/mtp_ioctl.h

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
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

 *****************************************************************************/

#ifndef __MTP_IOCTL_H__
#define __MTP_IOCTL_H__

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

/*
 * INFORMATION
 *
 * Provides read-only information that is not available for the manager to
 * configure.  Also, this information does not change while the object exists:
 * it is purely static for the lifetime of the object.
 */
typedef struct mtp_info_df {
} mtp_info_df_t;
typedef struct mtp_info_mt {
} mtp_info_mt_t;
typedef struct mtp_info_na {
} mtp_info_na_t;
typedef struct mtp_info_sp {
} mtp_info_sp_t;
typedef struct mtp_info_rs {
} mtp_info_rs_t;
typedef struct mtp_info_rl {
} mtp_info_rl_t;
typedef struct mtp_info_rt {
} mtp_info_rt_t;
typedef struct mtp_info_ls {
} mtp_info_ls_t;
typedef struct mtp_info_lk {
} mtp_info_lk_t;
typedef struct mtp_info_sl {
} mtp_info_sl_t;
typedef struct mtp_info_cb {
} mtp_info_cb_t;
typedef struct mtp_info_cr {
} mtp_info_cr_t;

typedef union mtp_info_obj {
	struct mtp_info_df df;
	struct mtp_info_mt mt;
	struct mtp_info_na na;
	struct mtp_info_sp sp;
	struct mtp_info_rs rs;
	struct mtp_info_rl rl;
	struct mtp_info_rt rt;
	struct mtp_info_ls ls;
	struct mtp_info_lk lk;
	struct mtp_info_sl sl;
	struct mtp_info_cb cb;
	struct mtp_info_cr cr;
} mtp_info_obj_t;

typedef struct mtp_info {
	uint type;			/* object type */
	uint id;			/* object identifier */
	uint cmd;			/* command */
	/* followed by object-specific structure */
	union mtp_info_obj info[0];
} mtp_info_t;

#define MTP_IOCGINFO	_IOWR(MTP_IOC_MAGIC,	 0, mtp_info_t)	/* get */

/*
 * OPTIONS
 *
 * Provides options not necessary for configuration but that can be changed
 * while the object exists without reconfiguration of the object (that is, a
 * change to these items does not change the relationship between the object
 * changed and other objects).
 *
 * Note that one of these option structures can optionally be appended to a
 * configuration structure when an object is created or moved.
 */
typedef struct mtp_opt_conf_df {
} mtp_opt_conf_df_t;
typedef struct mtp_opt_conf_mt {
} mtp_opt_conf_mt_t;
typedef struct mtp_opt_conf_na {
	/* signalling link timers */
	uint t1;			/* timer t1 value */
	uint t2;			/* timer t2 value */
	uint t3;			/* timer t3 value */
	uint t4;			/* timer t4 value */
	uint t5;			/* timer t5 value */
	uint t12;			/* timer t12 value */
	uint t13;			/* timer t13 value */
	uint t14;			/* timer t14 value */
	uint t17;			/* timer t17 value */
	uint t19a;			/* timer t19a value */
	uint t20a;			/* timer t20a value */
	uint t21a;			/* timer t21a value */
	uint t22;			/* timer t22 value */
	uint t23;			/* timer t23 value */
	uint t24;			/* timer t24 value */
	uint t31a;			/* timer t31a value */
	uint t32a;			/* timer t32a value */
	uint t33a;			/* timer t33a value */
	uint t34a;			/* timer t34a value */
	uint t1t;			/* timer t1t value */
	uint t2t;			/* timer t2t value */
	uint t1s;			/* timer t1s value */
	/* link timers */
	uint t7;			/* timer t7 value */
	uint t19;			/* timer t19 value */
	uint t21;			/* timer t21 value */
	uint t25a;			/* timer t25a value */
	uint t28a;			/* timer t28a value */
	uint t29a;			/* timer t29a value */
	uint t30a;			/* timer t30a value */
	/* route timers */
	uint t6;			/* timer t6 value */
	uint t10;			/* timer t10 value */
	/* route set timers */
	uint t8;			/* timer t8 value */
	uint t11;			/* timer t11 value */
	uint t15;			/* timer t15 value */
	uint t16;			/* timer t16 value */
	uint t18a;			/* timer t18a value */
	/* signalling point timers */
	uint t1r;			/* timer t1r value */
	uint t18;			/* timer t18 value */
	uint t20;			/* timer t20 value */
	uint t22a;			/* timer t22a value */
	uint t23a;			/* timer t23a value */
	uint t24a;			/* timer t24a value */
	uint t26a;			/* timer t26a value */
	uint t27a;			/* timer t27a value */
} mtp_opt_conf_na_t;
typedef struct mtp_opt_conf_sp {
	/* signalling link timers */
	uint t1;			/* timer t1 value */
	uint t2;			/* timer t2 value */
	uint t3;			/* timer t3 value */
	uint t4;			/* timer t4 value */
	uint t5;			/* timer t5 value */
	uint t12;			/* timer t12 value */
	uint t13;			/* timer t13 value */
	uint t14;			/* timer t14 value */
	uint t17;			/* timer t17 value */
	uint t19a;			/* timer t19a value */
	uint t20a;			/* timer t20a value */
	uint t21a;			/* timer t21a value */
	uint t22;			/* timer t22 value */
	uint t23;			/* timer t23 value */
	uint t24;			/* timer t24 value */
	uint t31a;			/* timer t31a value */
	uint t32a;			/* timer t32a value */
	uint t33a;			/* timer t33a value */
	uint t34a;			/* timer t34a value */
	uint t1t;			/* timer t1t value */
	uint t2t;			/* timer t2t value */
	uint t1s;			/* timer t1s value */
	/* link timers */
	uint t7;			/* timer t7 value */
	uint t19;			/* timer t19 value */
	uint t21;			/* timer t21 value */
	uint t25a;			/* timer t25a value */
	uint t28a;			/* timer t28a value */
	uint t29a;			/* timer t29a value */
	uint t30a;			/* timer t30a value */
	/* route timers */
	uint t6;			/* timer t6 value */
	uint t10;			/* timer t10 value */
	/* route set timers */
	uint t8;			/* timer t8 value */
	uint t11;			/* timer t11 value */
	uint t15;			/* timer t15 value */
	uint t16;			/* timer t16 value */
	uint t18a;			/* timer t18a value */
	/* signalling point timers */
	uint t1r;			/* timer t1r value */
	uint t18;			/* timer t18 value */
	uint t20;			/* timer t20 value */
	uint t22a;			/* timer t22a value */
	uint t23a;			/* timer t23a value */
	uint t24a;			/* timer t24a value */
	uint t26a;			/* timer t26a value */
	uint t27a;			/* timer t27a value */
} mtp_opt_conf_sp_t;
typedef struct mtp_opt_conf_rs {
	/* route timers */
	uint t6;			/* timer t6 value */
	uint t10;			/* timer t10 value */
	/* route set timers */
	uint t8;			/* timer t8 value */
	uint t11;			/* timer t11 value */
	uint t15;			/* timer t15 value */
	uint t16;			/* timer t16 value */
	uint t18a;			/* timer t18a value */
} mtp_opt_conf_rs_t;
typedef struct mtp_opt_conf_rl {
	/* route timers */
	uint t6;			/* timer t6 value */
	uint t10;			/* timer t10 value */
} mtp_opt_conf_rl_t;
typedef struct mtp_opt_conf_rt {
	/* route timers */
	uint t6;			/* timer t6 value */
	uint t10;			/* timer t10 value */
} mtp_opt_conf_rt_t;
typedef struct mtp_opt_conf_ls {
	/* signalling link timers */
	uint t1;			/* timer t1 value */
	uint t2;			/* timer t2 value */
	uint t3;			/* timer t3 value */
	uint t4;			/* timer t4 value */
	uint t5;			/* timer t5 value */
	uint t12;			/* timer t12 value */
	uint t13;			/* timer t13 value */
	uint t14;			/* timer t14 value */
	uint t17;			/* timer t17 value */
	uint t19a;			/* timer t19a value */
	uint t20a;			/* timer t20a value */
	uinuint t21a;			/* timer t21a value */
	uint t22;			/* timer t22 value */
	uint t23;			/* timer t23 value */
	uint t24;			/* timer t24 value */
	uint t31a;			/* timer t31a value */
	uint t32a;			/* timer t32a value */
	uint t33a;			/* timer t33a value */
	uint t34a;			/* timer t34a value */
	uint t1t;			/* timer t1t value */
	uint t2t;			/* timer t2t value */
	uint t1s;			/* timer t1s value */
	/* link timers */
	uint t7;			/* timer t7 value */
	uint t19;			/* timer t19 value */
	uint t21;			/* timer t21 value */
	uint t25a;			/* timer t25a value */
	uint t28a;			/* timer t28a value */
	uint t29a;			/* timer t29a value */
	uint t30a;			/* timer t30a value */
} mtp_opt_conf_ls_t;
typedef struct mtp_opt_conf_lk {
	/* signalling link timers */
	uint t1;			/* timer t1 value */
	uint t2;			/* timer t2 value */
	uint t3;			/* timer t3 value */
	uint t4;			/* timer t4 value */
	uint t5;			/* timer t5 value */
	uint t12;			/* timer t12 value */
	uint t13;			/* timer t13 value */
	uint t14;			/* timer t14 value */
	uint t17;			/* timer t17 value */
	uint t19a;			/* timer t19a value */
	uint t20a;			/* timer t20a value */
	uint t21a;			/* timer t21a value */
	uint t22;			/* timer t22 value */
	uint t23;			/* timer t23 value */
	uint t24;			/* timer t24 value */
	uint t31a;			/* timer t31a value */
	uint t32a;			/* timer t32a value */
	uint t33a;			/* timer t33a value */
	uint t34a;			/* timer t34a value */
	uint t1t;			/* timer t1t value */
	uint t2t;			/* timer t2t value */
	uint t1s;			/* timer t1s value */
	/* link timers */
	uint t7;			/* timer t7 value */
	uint t19;			/* timer t19 value */
	uint t21;			/* timer t21 value */
	uint t25a;			/* timer t25a value */
	uint t28a;			/* timer t28a value */
	uint t29a;			/* timer t29a value */
	uint t30a;			/* timer t30a value */
} mtp_opt_conf_lk_t;
typedef struct mtp_opt_conf_sl {
	/* signalling link timers */
	uint t1;			/* timer t1 value */
	uint t2;			/* timer t2 value */
	uint t3;			/* timer t3 value */
	uint t4;			/* timer t4 value */
	uint t5;			/* timer t5 value */
	uint t12;			/* timer t12 value */
	uint t13;			/* timer t13 value */
	uint t14;			/* timer t14 value */
	uint t17;			/* timer t17 value */
	uint t19a;			/* timer t19a value */
	uint t20a;			/* timer t20a value */
	uint t21a;			/* timer t21a value */
	uint t22;			/* timer t22 value */
	uint t23;			/* timer t23 value */
	uint t24;			/* timer t24 value */
	uint t31a;			/* timer t31a value */
	uint t32a;			/* timer t32a value */
	uint t33a;			/* timer t33a value */
	uint t34a;			/* timer t34a value */
	uint t1t;			/* timer t1t value */
	uint t2t;			/* timer t2t value */
	uint t1s;			/* timer t1s value */
} mtp_opt_conf_sl_t;
typedef struct mtp_opt_conf_cb {
} mtp_opt_conf_cb_t;
typedef struct mtp_opt_conf_cr {
} mtp_opt_conf_cr_t;

typedef union mtp_option_obj {
	struct mtp_opt_conf_df df;
	struct mtp_opt_conf_mt mt;
	struct mtp_opt_conf_na na;
	struct mtp_opt_conf_sp sp;
	struct mtp_opt_conf_rs rs;
	struct mtp_opt_conf_rl rl;
	struct mtp_opt_conf_rt rt;
	struct mtp_opt_conf_ls ls;
	struct mtp_opt_conf_lk lk;
	struct mtp_opt_conf_sl sl;
	struct mtp_opt_conf_cb cb;
	struct mtp_opt_conf_cr cr;
} mtp_option_obj_t;

#define RT_TYPE_MEMBER	0
#define RT_TYPE_CLUSTER	1

#define RS_TYPE_MEMBER	0
#define RS_TYPE_CLUSTER	1

typedef struct mtp_option {
	uint type;			/* object type */
	uint id;			/* object id */
	/* followed by specific proto structure */
	union mtp_option_obj options[0];
} mtp_option_t;

#define MTP_IOCGOPTION	_IOWR(MTP_IOC_MAGIC,	 1, mtp_option_t)	/* get */
#define MTP_IOCSOPTION	_IOWR(MTP_IOC_MAGIC,	 2, mtp_option_t)	/* set */

/*
 * CONFIGURATION
 *
 * Configures the object (creates, destroys, moves).  These actions require
 * reconfiguration of the object and a change in its relationship to other
 * objects.  Also, this structure contains information that must be specified
 * during creation of the object.  Each structure can optionally be followed by
 * an options structure to specify the options during creation.
 */
typedef struct mtp_conf_df {
} mtp_conf_df_t;
typedef struct mtp_conf_mt {
} mtp_conf_mt_t;
typedef struct mtp_conf_na_mask {
	uint member;			/* PC member mask */
	uint cluster;			/* PC cluster mask */
	uint network;			/* PC network mask */
} mtp_conf_na_mask_t;
typedef struct mtp_conf_na {
	struct lmi_option options;	/* protocol options */
	struct mtp_conf_na_mask mask;	/* point code mask */
	uint sls_bits;			/* bits in SLS */
} mtp_conf_na_t;
typedef struct mtp_conf_sp {
	uint naid;			/* network appearance id */
	uint pc;			/* point code */
	uint users;			/* mask of equipped users */
	uint flags;			/* options flags */
} mtp_conf_sp_t;
typedef struct mtp_conf_rs {
	uint spid;			/* signalling point id */
	uint dest;			/* destination point code */
	uint flags;			/* options flags */
} mtp_conf_rs_t;
typedef struct mtp_conf_rl {
	uint rsid;			/* route set id */
	uint lsid;			/* combined link set id */
	uint cost;			/* cost in routeset */
} mtp_conf_rl_t;
typedef struct mtp_conf_rt {
	uint rlid;			/* route list id */
	uint lkid;			/* link id */
	uint slot;			/* slot of SLS for this route */
} mtp_conf_rt_t;
typedef struct mtp_conf_ls {
	uint spid;			/* signalling point id */
	uint sls_mask;			/* mask of bits selecting link set */
} mtp_conf_ls_t;
typedef struct mtp_conf_lk {
	uint lsid;			/* combined link set id */
	uint rsid;			/* routeset of adjacent signalling point */
	uint ni;			/* network indicator for link set */
	uint slot;			/* slot of SLS for this link set */
} mtp_conf_lk_t;
typedef struct mtp_conf_sl {
	int muxid;			/* lower multiplexor id */
	uint lkid;			/* link set id */
	uint slc;			/* signalling link code in lk */
} mtp_conf_sl_t;
typedef struct mtp_conf_cb {
} mtp_conf_cb_t;
typedef struct mtp_conf_cr {
} mtp_conf_cr_t;

typedef union mtp_conf_obj {
	struct mtp_conf_df df;
	struct mtp_conf_mt mt;
	struct mtp_conf_na na;
	struct mtp_conf_sp sp;
	struct mtp_conf_rs rs;
	struct mtp_conf_rl rl;
	struct mtp_conf_rt rt;
	struct mtp_conf_ls ls;
	struct mtp_conf_lk lk;
	struct mtp_conf_sl sl;
	struct mtp_conf_cb cb;
	struct mtp_conf_cr cr;
} mtp_conf_obj_t;

/*
   additional MTP protocol options 
 */
#define SS7_POPT_TFR	0x00010000	/* old broadcast method - no responsive */
#define SS7_POPT_TFRB	0x00020000	/* new broadcast method - no regulation */
#define SS7_POPT_TFRR	0x00040000	/* new responsive method - regulated */
#define SS7_POPT_MCSTA	0x00080000	/* multiple congestion states */

#define MTP_GET		0	/* get options or configuration */
#define MTP_ADD		1	/* add configuration */
#define MTP_CHA		2	/* set options or change configuration */
#define MTP_DEL		3	/* delete configuraiton */
#define MTP_TST		4	/* test options or configuration */
#define MTP_COM		5	/* commit options or configuration */

typedef struct mtp_config {
	uint type;			/* object type */
	uint id;			/* object id */
	uint cmd;			/* object command */
	/* followed by specific configuration structure */
	union mtp_conf_obj config[0];
} mtp_config_t;

#define MTP_IOCLCONFIG	_IOWR(MTP_IOC_MAGIC,	 3, mtp_config_t)	/* list */
#define MTP_IOCGCONFIG	_IOWR(MTP_IOC_MAGIC,	 4, mtp_config_t)	/* get */
#define MTP_IOCSCONFIG	_IOWR(MTP_IOC_MAGIC,	 5, mtp_config_t)	/* set */
#define MTP_IOCTCONFIG	_IOWR(MTP_IOC_MAGIC,	 6, mtp_config_t)	/* test */
#define MTP_IOCCCONFIG	_IOWR(MTP_IOC_MAGIC,	 7, mtp_config_t)	/* commit */

/*
 * STATE
 *
 * Provides state machine state information.  The purpose of these items is
 * primarily for debugging: to view the internal state of the state machine.
 * Some of these state variables might also be reflected in status fields.
 */
typedef struct mtp_timers_df {
} mtp_timers_df_t;
typedef struct mtp_timers_mt {
} mtp_timers_mt_t;
/* primary states */
#define MTP_ALLOWED		0x00UL	/* Entity Allowed */
#define MTP_DANGER		0x01UL	/* Entity Danger of congestion (primary or secondary) */
#define MTP_CONGESTED		0x02UL	/* Entity Congested (Link Set congestion, primary or secondary ) */
#define MTP_RESTRICTED		0x03UL	/* Entity Restricted (Route Failure or received TFR) */
#define MTP_RESTART		0x04UL	/* Entity Restarting */
#define MTP_PROHIBITED		0x05UL	/* Entity Prohibited (Received TFP) */
#define MTP_INHIBITED		0x06UL	/* Entity Inhibited (Management inhibited) */
#define MTP_BLOCKED		0x07UL	/* Entity Blocked (Local Link Set failure) */
#define MTP_INACTIVE		0x08UL	/* Entity Inactive (Link out of service) */
#define MTP_LOC_INHIBIT		0x09UL	/* Entity locally inhibited */
#define MTP_REM_INHIBIT		0x0aUL	/* Entity remotely inhibited */

/* pseudo-states */
#define MTP_NODANGER		0x11UL	/* Entity Out of Danger (transient state) */
#define MTP_UNCONGESTED		0x12UL	/* Entity Uncongested (transient state) */
#define MTP_RESTARTED		0x14UL	/* Entity Restarted */
#define MTP_UNINHIBITED		0x16UL	/* Entity Uninhibited (transient state) */
#define MTP_UNBLOCKED		0x17UL	/* Entity Unblocked (transient state) */
#define MTP_ACTIVE		0x18UL	/* Entity Active (Link in service) */
#define MTP_RESTART_PHASE_1	0x24UL	/* Entity Restarting Phase 1 */
#define MTP_RESTART_PHASE_2	0x34UL	/* Entity Restarting Phase 2 */

/* state flags */
#define MTPF_ALLOWED		(1<< MTP_ALLOWED)	/* Entity is allowed */
#define MTPF_DANGER		(1<< MTP_DANGER)	/* Entity is in danger of congestion */
#define MTPF_CONGESTED		(1<< MTP_CONGESTED)	/* Entity is congested */
#define MTPF_RESTRICTED		(1<< MTP_RESTRICTED)	/* Entity is restricted */
#define MTPF_RESTART		(1<< MTP_RESTART)	/* Entity is restarting */
#define MTPF_PROHIBITED		(1<< MTP_PROHIBITED)	/* Entity is prohibited */
#define MTPF_INHIBITED		(1<< MTP_INHIBITED)	/* Entity is inhibited */
#define MTPF_BLOCKED		(1<< MTP_BLOCKED)	/* Entity is blocked */
#define MTPF_INACTIVE		(1<< MTP_INACTIVE)	/* Entity is inactive */
#define MTPF_LOC_INHIBIT	(1<< MTP_LOC_INHIBIT)	/* Entity is locally inhibited */
#define MTPF_REM_INHIBIT	(1<< MTP_REM_INHIBIT)	/* Entity is remotely inhibited */

/* additional state flags */
#define MTPF_TRAFFIC		(1<< 9)	/* Entity has sent traffic */
#define MTPF_COO_RECV		(1<<10)	/* Entity has received a COO */
#define MTPF_ECO_RECV		(1<<11)	/* Entity has received a ECO */
#define MTPF_WACK_SLTM		(1<<12)	/* Entity waiting for response to 1st SLTM */
#define MTPF_WACK_SLTM2		(1<<13)	/* Entity waiting for response to 2nd SLTM */
#define MTPF_WACK_SSLTM		(1<<14)	/* Entity waiting for response to 1st SSLTM */
#define MTPF_WACK_SSLTM2	(1<<15)	/* Entity waiting for response to 2nd SSLTM */
#define MTPF_TFR_PENDING	(1<<16)	/* Entity has TFR pending */

/* non-state (options) flags */
#define MTPF_CLUSTER		(1<<17)	/* Entity is cluster route */
#define MTPF_XFER_FUNC		(1<<18)	/* Entity has transfer function */
#define MTPF_SECURITY		(1<<19)	/* Entity has additional security procedures */
#define MTPF_ADJACENT		(1<<20)	/* Entity is adjacent */
#define MTPF_LOSC_PROC_A	(1<<21)	/* Entity uses link oscillation procedure A */
#define MTPF_LOSC_PROC_B	(1<<22)	/* Entity uses link oscillation procedure B */
#define MTPF_RESTART_PHASE_1	(1<<23)	/* Entity restarting phase 1 */
#define MTPF_RESTART_PHASE_2	(1<<24)	/* Entity restarting phase 2 */
#define MTPF_RESTART_LOCKOUT	(1<<25)	/* Entity restarting but locked out (T27a) */

typedef struct mtp_timers_na {
} mtp_timers_na_t;
typedef struct mtp_timers_sp {
	uint t1r;			/* timer t1r */
	uint t18;			/* timer t18 */
	uint t20;			/* timer t20 */
	uint t22a;			/* timer t22a */
	uint t23a;			/* timer t23a */
	uint t24a;			/* timer t24a */
	uint t26a;			/* timer t26a */
	uint t27a;			/* timer t27a */
} mtp_timers_sp_t;
#define SP_ALLOWED	    (MTP_ALLOWED)	/* Sig Point Allowed */
#define SP_DANGER	    (MTP_DANGER)	/* Sig Point Danger of congestion (primary or secondary) */
#define SP_CONGESTED	    (MTP_CONGESTED)	/* Sig Point Congested (Link Set cong, primary or secondary ) */
#define SP_RESTRICTED	    (MTP_RESTRICTED)	/* Sig Point Restricted (Route Failure or received TFR) */
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

typedef struct mtp_timers_rs {
	uint t8;			/* timer t8 */
	uint t11;			/* timer t11 */
	uint t15;			/* timer t15 */
	uint t16;			/* timer t16 */
	uint t18a;			/* timer t18a */
} mtp_timers_rs_t;
#define RS_ALLOWED	    (MTP_ALLOWED)	/* Routeset Allowed */
#define RS_DANGER	    (MTP_DANGER)	/* Routeset Danger of congestion (primary or secondary) */
#define RS_CONGESTED	    (MTP_CONGESTED)	/* Routeset Congested (Link Set cong, primary or secondary) */
#define RS_RESTRICTED	    (MTP_RESTRICTED)	/* Routeset Restricted (Route Failure or received TFR) */
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
#define RSF_SECURITY	    (MTPF_SECURITY)	/* Routeset has additional security procedures */

typedef struct mtp_timers_rl {
} mtp_timers_rl_t;
// #define RL_ALLOWED (MTP_ALLOWED) /* Routelist Allowed */
// #define RL_DANGER (MTP_DANGER) /* Routelist Danger of congestion (primary or secondary) */
// #define RL_CONGESTED (MTP_CONGESTED) /* Routelist Congested (Link Set cong, primary or
// secondary) */
#define RL_RESTRICTED	    (MTP_RESTRICTED)	/* Routelist Restricted (Route Failure or received TFR) */
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

typedef struct mtp_timers_rt {
	uint t6;			/* timer t6 */
	uint t10;			/* timer t10 */
} mtp_timers_rt_t;
#define RT_ALLOWED	    (MTP_ALLOWED)	/* Route Allowed */
#define RT_DANGER	    (MTP_DANGER)	/* Route Danger of congestion (primary or secondary) */
#define RT_CONGESTED	    (MTP_CONGESTED)	/* Route Congested (Link Set congestion, primary or secondary ) */
#define RT_RESTRICTED	    (MTP_RESTRICTED)	/* Route Restricted (Route Failure or received TFR) */
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

typedef struct mtp_timers_ls {
} mtp_timers_ls_t;
#define LS_ALLOWED	    (MTP_ALLOWED)	/* Linkset Allowed */
#define LS_DANGER	    (MTP_DANGER)	/* Linkset Danger of congestion (primary or secondary) */
#define LS_CONGESTED	    (MTP_CONGESTED)	/* Linkset Congested (Link Set congestion, primary or secondary ) */
#define LS_RESTRICTED	    (MTP_RESTRICTED)	/* Linkset Restricted (Route Failure or received TFR) */
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

typedef struct mtp_timers_lk {
	uint t7;			/* timer t7 */
	uint t19;			/* timer t19 */
	uint t21;			/* timer t21 */
	uint t25a;			/* timer t25a */
	uint t28a;			/* timer t28a */
	uint t29a;			/* timer t29a */
	uint t30a;			/* timer t30a */
} mtp_timers_lk_t;
#define LK_ALLOWED	    (MTP_ALLOWED)	/* Link Set Allowed */
#define LK_DANGER	    (MTP_DANGER)	/* Link Set Danger of congestion (primary or secondary) */
#define LK_CONGESTED	    (MTP_CONGESTED)	/* Link Set Congested (Link Set congestion, primary or secondary ) */
#define LK_RESTRICTED	    (MTP_RESTRICTED)	/* Link Set Restricted (Route Failure or received TFR) */
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

typedef struct mtp_timers_sl {
	uint t1;			/* timer t1 */
	uint t2;			/* timer t2 */
	uint t3;			/* timer t3 */
	uint t4;			/* timer t4 */
	uint t5;			/* timer t5 */
	uint t12;			/* timer t12 */
	uint t13;			/* timer t13 */
	uint t14;			/* timer t14 */
	uint t17;			/* timer t17 */
	uint t19a;			/* timer t19a */
	uint t20a;			/* timer t20a */
	uint t21a;			/* timer t21a */
	uint t22;			/* timer t22 */
	uint t23;			/* timer t23 */
	uint t24;			/* timer t24 */
	uint t31a;			/* timer t31a */
	uint t32a;			/* timer t32a */
	uint t33a;			/* timer t33a */
	uint t34a;			/* timer t34a */
	uint t1t;			/* timer t1t */
	uint t2t;			/* timer t2t */
	uint t1s;			/* timer t1s */
} mtp_timers_sl_t;
#define SLS_OOS			0	/* out of service */
#define SLS_PROC_OUTG		1	/* processor outage */
#define SLS_IS			2	/* in service */
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
#define SL_UPDATED	    (MTP_RESTARTED)	/* Sig link Buffer Update Complete (transient state) */
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
#define SLF_LOC_INHIBIT	    (MTPF_LOC_INHIBIT)	/* Sig link Locally Inhibited */
#define SLF_REM_INHIBIT	    (MTPF_REM_INHIBIT)	/* Sig link Remotely Inhibited */

#define SLF_LOSC_PROC_A	    (MTPF_LOSC_PROC_A)	/* Sig link uses link oscillation procedure A */
#define SLF_LOSC_PROC_B	    (MTPF_LOSC_PROC_B)	/* Sig link uses link oscillation procedure B */
#define SLF_RESTART_LOCKOUT (MTPF_RESTART_LOCKOUT)	/* Sig link restart lockout (T27a running) */

typedef struct mtp_timers_cb {
} mtp_timers_cb_t;
typedef struct mtp_timers_cr {
} mtp_timers_cr_t;

typedef struct mtp_statm_df {
	struct mtp_timers_df timers;
} mtp_statm_df_t;
typedef struct mtp_statm_mt {
	struct mtp_timers_mt timers;
} mtp_statm_mt_t;
typedef struct mtp_statm_na {
	struct mtp_timers_na timers;
} mtp_statm_na_t;
typedef struct mtp_statm_sp {
	struct mtp_timers_sp timers;
} mtp_statm_sp_t;
typedef struct mtp_statm_rs {
	struct mtp_timers_rs timers;
} mtp_statm_rs_t;
typedef struct mtp_statm_rl {
	struct mtp_timers_rl timers;
} mtp_statm_rl_t;
typedef struct mtp_statm_rt {
	struct mtp_timers_rt timers;
} mtp_statm_rt_t;
typedef struct mtp_statm_ls {
	struct mtp_timers_ls timers;
} mtp_statm_ls_t;
typedef struct mtp_statm_lk {
	struct mtp_timers_lk timers;
} mtp_statm_lk_t;
typedef struct mtp_statm_sl {
	struct mtp_timers_sl timers;
} mtp_statm_sl_t;
typedef struct mtp_statm_cb {
	struct mtp_timers_cb timers;
} mtp_statm_cb_t;
typedef struct mtp_statm_cr {
	struct mtp_timers_cr timers;
} mtp_statm_cr_t;

typedef union mtp_statm_obj {
	struct mtp_statm_df df;
	struct mtp_statm_mt mt;
	struct mtp_statm_na na;
	struct mtp_statm_sp sp;
	struct mtp_statm_rs rs;
	struct mtp_statm_rl rl;
	struct mtp_statm_rt rt;
	struct mtp_statm_ls ls;
	struct mtp_statm_lk lk;
	struct mtp_statm_sl sl;
	struct mtp_statm_cb cb;
	struct mtp_statm_cr cr;
} mtp_statm_obj_t;

typedef struct mtp_statem {
	uint type;			/* object type */
	uint id;			/* object id */
	uint flags;			/* object flags */
	uint state;			/* object state */
	/* followed by object-specific state structure */
	union mtp_statem_obj statem[0];
} mtp_statem_t;

#define MTP_IOCGSTATEM	_IOWR(MTP_IOC_MAGIC,	 8, mtp_statem_t)	/* get */
#define MTP_IOCCMRESET	_IOWR(MTP_IOC_MAGIC,	 9, mtp_statem_t)	/* master reset */

/*
 * STATUS
 *
 * Provides status on the object.  Many of these are X.721 status fields or
 * object-specific status information.  Only read-write status fields may be
 * altered with a set or clear command.
 */
typedef struct mtp_status_df {
} mtp_status_df_t;
typedef struct mtp_status_mt {
} mtp_status_mt_t;
typedef struct mtp_status_na {
} mtp_status_na_t;
typedef struct mtp_status_sp {
} mtp_status_sp_t;
typedef struct mtp_status_rs {
} mtp_status_rs_t;
typedef struct mtp_status_rl {
} mtp_status_rl_t;
typedef struct mtp_status_rt {
} mtp_status_rt_t;
typedef struct mtp_status_ls {
} mtp_status_ls_t;
typedef struct mtp_status_lk {
} mtp_status_lk_t;
typedef struct mtp_status_sl {
} mtp_status_sl_t;
typedef struct mtp_status_cb {
} mtp_status_cb_t;
typedef struct mtp_status_cr {
} mtp_status_cr_t;

typedef union mtp_status_obj {
	struct mtp_status_df df;
	struct mtp_status_mt mt;
	struct mtp_status_na na;
	struct mtp_status_sp sp;
	struct mtp_status_rs rs;
	struct mtp_status_rl rl;
	struct mtp_status_rt rt;
	struct mtp_status_ls ls;
	struct mtp_status_lk lk;
	struct mtp_status_sl sl;
	struct mtp_status_cb cb;
	struct mtp_status_cr cr;
} mtp_status_obj_t;

typedef struct mtp_status {
	uint type;			/* object type */
	uint id;			/* object identifier */
	/* followed by object-specific structure */
	union mtp_status_obj status[0];
} mtp_status_t;

#define MTP_IOCGSTATUS	_IOWR(MTP_IOC_MAGIC,	10, mtp_status_t)	/* get */
#define MTP_IOCSSTATUS	_IOWR(MTP_IOC_MAGIC,	11, mtp_status_t)	/* set */
#define MTP_IOCCSTATUS	_IOWR(MTP_IOC_MAGIC,	12, mtp_status_t)	/* clear */

/*
 * STATISTICS
 */
typedef struct mtp_stats_df {
} mtp_stats_df_t;
typedef struct mtp_stats_mt {
} mtp_stats_mt_t;
typedef struct mtp_stats_na {
} mtp_stats_na_t;
typedef struct mtp_stats_sp {
} mtp_stats_sp_t;
typedef struct mtp_stats_rs {
} mtp_stats_rs_t;
typedef struct mtp_stats_rl {
} mtp_stats_rl_t;
typedef struct mtp_stats_rt {
} mtp_stats_rt_t;
typedef struct mtp_stats_ls {
} mtp_stats_ls_t;
typedef struct mtp_stats_lk {
} mtp_stats_lk_t;
typedef struct mtp_stats_sl {
} mtp_stats_sl_t;
typedef struct mtp_stats_cb {
} mtp_stats_cb_t;
typedef struct mtp_stats_cr {
} mtp_stats_cr_t;

typedef union mtp_stats_obj {
	struct mtp_stats_df df;
	struct mtp_stats_mt mt;
	struct mtp_stats_na na;
	struct mtp_stats_sp sp;
	struct mtp_stats_rs rs;
	struct mtp_stats_rl rl;
	struct mtp_stats_rt rt;
	struct mtp_stats_ls ls;
	struct mtp_stats_lk lk;
	struct mtp_stats_sl sl;
	struct mtp_stats_cb cb;
	struct mtp_stats_cr cr;
} mtp_stats_obj_t;

typedef struct mtp_stats {
	uint type;			/* object type */
	uint id;			/* object id */
	uint header;			/* object stats header */
	/* followed by object-specific statistics structure */
	union mtp_stats_obj stats[0];
} mtp_stats_t;

#define MTP_IOCGSTATSP	_IOWR(MTP_IOC_MAGIC,	 8,  mtp_stats_t)	/* get period */
#define MTP_IOCSSTATSP	_IOWR(MTP_IOC_MAGIC,	 9,  mtp_stats_t)	/* set period */
#define MTP_IOCGSTATS	_IOWR(MTP_IOC_MAGIC,	10,  mtp_stats_t)	/* get stats */
#define MTP_IOCCSTATS	_IOWR(MTP_IOC_MAGIC,	11,  mtp_stats_t)	/* get and clear stats */

/*
 * EVENTS
 *
 * Provides a mechanism for requesting event notifications.  The flags field in
 * the header is used to requesting X.721 events (e.g. object creation
 * notification).  Events can be requested by object class, and a mechanism
 * exists for requesting events by specific object; however, the driver is not
 * required to support per-object notification and will return EOPNOTSUPP.  To
 * request notification for an object class, specify zero (0) for the
 * object identifier.
 *
 * Event requests are per requesting Stream.  Non-management streams should only
 * be notified of events for objects associated with the Stream and not X.721
 * events unless also specified as a specific event (e.g. communications alarm
 * events).  Management Streams can be notified of any object.  Notifications
 * are provided using the LMI_EVENT_IND (event indication) for X.721 events, and
 * the LMI_EVENT_IND (for other events).
 */
typedef struct mtp_notify_df {
	uint events;
} mtp_notify_df_t;
typedef struct mtp_notify_mt {
	uint events;
} mtp_notify_mt_t;
typedef struct mtp_notify_na {
	uint events;
} mtp_notify_na_t;
typedef struct mtp_notify_sp {
	uint events;
} mtp_notify_sp_t;
#define MTP_EVT_SP_MSU_DISCARDED		(1<<19)
#define MTP_EVT_SP_UPU_SENT			(1<<20)
#define MTP_EVT_SP_UPU_RECV			(1<<21)
typedef struct mtp_notify_rs {
	uint events;
} mtp_notify_rs_t;
#define MTP_EVT_RS_UNAVAIL_START		(1<<13)
#define MTP_EVT_RS_UNAVAIL_STOP			(1<<14)
#define MTP_EVT_RS_REROUTE_ADJ			(1<<15)
#define MTP_EVT_RS_ADJ_SP_INACCESS_START	(1<<16)
#define MTP_EVT_RS_ADJ_SP_INACCESS_STOP		(1<<17)
#define MTP_EVT_RS_TFC_RECV			(1<<18)
typedef struct mtp_notify_rl {
	uint events;
} mtp_notify_rl_t;
typedef struct mtp_notify_rt {
	uint events;
} mtp_notify_rt_t;
typedef struct mtp_notify_ls {
	uint events;
} mtp_notify_ls_t;
typedef struct mtp_notify_lk {
	uint events;
} mtp_notify_lk_t;
#define MTP_EVT_LK_FAILURE_START		(1<< 9)
#define MTP_EVT_LK_FAILURE_STOP			(1<<10)
#define MTP_EVT_LK_SEND_TFP			(1<<11)
#define MTP_EVT_LK_SEND_TFA			(1<<12)
typedef struct mtp_notify_sl {
	uint events;
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
typedef struct mtp_notify_cb {
	uint events;
} mtp_notify_cb_t;
typedef struct mtp_notify_cr {
	uint events;
} mtp_notify_cr_t;

typedef union mtp_notify_obj {
	struct mtp_notify_df df;
	struct mtp_notify_mt mt;
	struct mtp_notify_na na;
	struct mtp_notify_sp sp;
	struct mtp_notify_rs rs;
	struct mtp_notify_rl rl;
	struct mtp_notify_rt rt;
	struct mtp_notify_ls ls;
	struct mtp_notify_lk lk;
	struct mtp_notify_sl sl;
	struct mtp_notify_cb cb;
	struct mtp_notify_cr cr;
} mtp_notify_obj_t;

typedef struct mtp_notify {
	uint type;			/* object type */
	uint id;			/* object id */
	/* followed by object-specific notification structure */
	union mtp_notify_obj notify[0];
} mtp_notify_t

#define MTP_IOCGNOTIFY	_IOWR(MTP_IOC_MAGIC, 17,  mtp_notify_t)	/* get */
#define MTP_IOCSNOTIFY	_IOWR(MTP_IOC_MAGIC, 18,  mtp_notify_t)	/* set */
#define MTP_IOCCNOTIFY	_IOWR(MTP_IOC_MAGIC, 19,  mtp_notify_t)	/* clear */

/*
 * ATTRIBUTES
 *
 * Provides a structure that contains all of the other information structures
 * pertaining to an object.  This is the total view of the object.  This 
 * structure can only be read and cannot be used to reconfigure or change
 * options associated with an object.  This structure is primarily for the
 * convenience of management agents so that it is possible to atomically read
 * all of the attributes associated with an object.
 */
    typedef struct mtp_attr_df {
	struct mtp_conf_df config;
	struct mtp_opt_conf_df option;
	struct mtp_info_df inform;
	struct mtp_statem_df statem;
	struct mtp_status_df status;
	struct mtp_stats_df stats;
	struct mtp_notify_df events;
} mtp_attr_df_t;
typedef struct mtp_attr_mt {
	struct mtp_conf_mt config;
	struct mtp_opt_conf_mt option;
	struct mtp_info_mt inform;
	struct mtp_statem_mt statem;
	struct mtp_status_mt status;
	struct mtp_stats_mt stats;
	struct mtp_notify_mt events;
} mtp_attr_mt_t;
typedef struct mtp_attr_na {
	struct mtp_conf_na config;
	struct mtp_opt_conf_na option;
	struct mtp_info_na inform;
	struct mtp_statem_na statem;
	struct mtp_status_na status;
	struct mtp_stats_na stats;
	struct mtp_notify_na events;
} mtp_attr_na_t;
typedef struct mtp_attr_sp {
	struct mtp_conf_sp config;
	struct mtp_opt_conf_sp option;
	struct mtp_info_sp inform;
	struct mtp_statem_sp statem;
	struct mtp_status_sp status;
	struct mtp_stats_sp stats;
	struct mtp_notify_sp events;
} mtp_attr_sp_t;
typedef struct mtp_attr_rs {
	struct mtp_conf_rs config;
	struct mtp_opt_conf_rs option;
	struct mtp_info_rs inform;
	struct mtp_statem_rs statem;
	struct mtp_status_rs status;
	struct mtp_stats_rs stats;
	struct mtp_notify_rs events;
} mtp_attr_rs_t;
typedef struct mtp_attr_rl {
	struct mtp_conf_rl config;
	struct mtp_opt_conf_rl option;
	struct mtp_info_rl inform;
	struct mtp_statem_rl statem;
	struct mtp_status_rl status;
	struct mtp_stats_rl stats;
	struct mtp_notify_rl events;
} mtp_attr_rl_t;
typedef struct mtp_attr_rt {
	struct mtp_conf_rt config;
	struct mtp_opt_conf_rt option;
	struct mtp_info_rt inform;
	struct mtp_statem_rt statem;
	struct mtp_status_rt status;
	struct mtp_stats_rt stats;
	struct mtp_notify_rt events;
} mtp_attr_rt_t;
typedef struct mtp_attr_ls {
	struct mtp_conf_ls config;
	struct mtp_opt_conf_ls option;
	struct mtp_info_ls inform;
	struct mtp_statem_ls statem;
	struct mtp_status_ls status;
	struct mtp_stats_ls stats;
	struct mtp_notify_ls events;
} mtp_attr_ls_t;
typedef struct mtp_attr_lk {
	struct mtp_conf_lk config;
	struct mtp_opt_conf_lk option;
	struct mtp_info_lk inform;
	struct mtp_statem_lk statem;
	struct mtp_status_lk status;
	struct mtp_stats_lk stats;
	struct mtp_notify_lk events;
} mtp_attr_lk_t;
typedef struct mtp_attr_sl {
	struct mtp_conf_sl config;
	struct mtp_opt_conf_sl option;
	struct mtp_info_sl inform;
	struct mtp_statem_sl statem;
	struct mtp_status_sl status;
	struct mtp_stats_sl stats;
	struct mtp_notify_sl events;
} mtp_attr_sl_t;
typedef struct mtp_attr_cb {
	struct mtp_conf_cb config;
	struct mtp_opt_conf_cb option;
	struct mtp_info_cb inform;
	struct mtp_statem_cb statem;
	struct mtp_status_cb status;
	struct mtp_stats_cb stats;
	struct mtp_notify_cb events;
} mtp_attr_cb_t;
typedef struct mtp_attr_cr {
	struct mtp_conf_cr config;
	struct mtp_opt_conf_cr option;
	struct mtp_info_cr inform;
	struct mtp_statem_cr statem;
	struct mtp_status_cr status;
	struct mtp_stats_cr stats;
	struct mtp_notify_cr events;
} mtp_attr_cr_t;

typedef union mtp_attr_obj {
	struct mtp_attr_df df;
	struct mtp_attr_mt mt;
	struct mtp_attr_na na;
	struct mtp_attr_sp sp;
	struct mtp_attr_rs rs;
	struct mtp_attr_rl rl;
	struct mtp_attr_rt rt;
	struct mtp_attr_ls ls;
	struct mtp_attr_lk lk;
	struct mtp_attr_sl sl;
	struct mtp_attr_cb cb;
	struct mtp_attr_cr cr;
} mtp_attr_obj_t;

typedef struct mtp_attr {
	uint type;			/* object type */
	uint id;			/* object identifier */
	uint cmd;			/* command */
	/* followed by object-specific structure */
	union mtp_attr_obj attrs[0];
} mtp_attr_t;

#define	MTP_IOCGATTR	_IOWR(MTP_IOC_MAGIC,	20, mtp_attr_t)	/* get attributes */

/*
 *  MANAGEMENT
 */
typedef struct mtp_action_df {
} mtp_action_df_t;
typedef struct mtp_action_mt {
} mtp_action_mt_t;
typedef struct mtp_action_na {
} mtp_action_na_t;
typedef struct mtp_action_sp {
} mtp_action_sp_t;
typedef struct mtp_action_rs {
} mtp_action_rs_t;
typedef struct mtp_action_rl {
} mtp_action_rl_t;
typedef struct mtp_action_rt {
} mtp_action_rt_t;
typedef struct mtp_action_ls {
} mtp_action_ls_t;
typedef struct mtp_action_lk {
} mtp_action_lk_t;
typedef struct mtp_action_sl {
} mtp_action_sl_t;
typedef struct mtp_action_cb {
} mtp_action_cb_t;
typedef struct mtp_action_cr {
} mtp_action_cr_t;

typedef union mtp_action_obj {
	struct mtp_action_df df;
	struct mtp_action_mt mt;
	struct mtp_action_na na;
	struct mtp_action_sp sp;
	struct mtp_action_rs rs;
	struct mtp_action_rl rl;
	struct mtp_action_rt rt;
	struct mtp_action_ls ls;
	struct mtp_action_lk lk;
	struct mtp_action_sl sl;
	struct mtp_action_cb cb;
	struct mtp_action_cr cr;
} mtp_action_obj_t;

typedef struct mtp_mgmt {
	uint type;			/* object type */
	uint id;			/* object id */
	uint cmd;			/* command */
	/* followed by object-specific structure */
	union mtp_action_obj action[0];
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

#define MTP_IOCCMGMT	_IOWR(MTP_IOC_MAGIC,	21, mtp_mgmt_t)	/* command */

/*
 * PASS LOWER
 *
 * This structure is deprecated and as largely used for testing as a mechanism
 * to bypass an input-output control to a stream linked under a multiplexing
 * driver supporting this interface.
 */
typedef struct mtp_pass {
	uint muxid;			/* mux index of lower stream to pass message to */
	uint8_t type;			/* type of message block */
	uint8_t band;			/* band of message block */
	uint ctl_length;		/* length of cntl part */
	uint dat_length;		/* length of data part */
	/* followed by cntl and data part of message to pass */
} mtp_pass_t;

#define MTP_IOCCPASS	_IOW(	MTP_IOC_MAGIC,	22,	mtp_pass_t	)

union mtp_ioctls {
	struct {
		struct mtp_info info;
		union mtp_info_obj obj;
	} info;
	struct {
		struct mtp_option option;
		union mtp_option_obj obj;
	} opt_conf;
	struct {
		struct mtp_config config;
		union mtp_conf_obj obj;
	} conf;
	struct {
		struct mtp_statem statem;
		union mtp_statem_obj obj;
	} statem;
	struct {
		struct mtp_status status;
		union mtp_status_obj obj;
	} status;
	struct {
		struct mtp_stats stats;
		union mtp_stats_obj obj;
	} stats;
	struct {
		struct mtp_notify notify;
		union mtp_notify_obj obj;
	} notify;
	struct {
		struct mtp_attr attr;
		union mtp_attr_obj obj;
	} attr;
	struct {
		struct mtp_mgmt mgmt;
		union mtp_action_obj obj;
	} mgmt;
	struct mtp_pass pass;
};

#define MTP_IOC_FIRST    0
#define MTP_IOC_LAST    22
#define MTP_IOC_PRIVATE 32

#endif				/* __MTP_IOCTL_H__ */
