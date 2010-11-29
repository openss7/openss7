/*****************************************************************************

 @(#) $Id: tcap_ioctl.h,v 1.1.2.2 2010-11-28 14:21:47 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2010  Monavacon Limited <http://www.monavacon.com/>
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

 Last Modified $Date: 2010-11-28 14:21:47 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: tcap_ioctl.h,v $
 Revision 1.1.2.2  2010-11-28 14:21:47  brian
 - remove #ident, protect _XOPEN_SOURCE

 Revision 1.1.2.1  2009-06-21 11:25:34  brian
 - added files to new distro

 *****************************************************************************/

#ifndef __TCAP_IOCTL_H__
#define __TCAP_IOCTL_H__

/* This file can be processed by doxygen(1). */

#include <linux/ioctl.h>

#define TCAP_IOC_MAGIC 'T'

#define TCAP_OBJ_TYPE_DF	0	/* default */
#define TCAP_OBJ_TYPE_TC	1	/* component */
#define TCAP_OBJ_TYPE_IV	2	/* invoke */
#define TCAP_OBJ_TYPE_OP	3	/* operation */
#define TCAP_OBJ_TYPE_DG	4	/* dialog */
#define TCAP_OBJ_TYPE_AC	5	/* application context */
#define TCAP_OBJ_TYPE_TR	6	/* transaction */
#define TCAP_OBJ_TYPE_TE	7	/* TCAP Entity */
#define TCAP_OBJ_TYPE_SP	8	/* SCCP SAP */
#define TCAP_OBJ_TYPE_SC	9	/* SCCP */

/*
 * INFORMATION
 *
 * Provides read-only information that is not available for the manager to
 * configure.  Also, this information does not change while the object exists:
 * it is purely static for the lifetime of the object.
 */
typedef struct tcap_info_df {
} tcap_info_df_t;
typedef struct tcap_info_tc {
} tcap_info_tc_t;
typedef struct tcap_info_iv {
} tcap_info_iv_t;
typedef struct tcap_info_op {
} tcap_info_op_t;
typedef struct tcap_info_dg {
} tcap_info_dg_t;
typedef struct tcap_info_ac {
} tcap_info_ac_t;
typedef struct tcap_info_tr {
} tcap_info_tr_t;
typedef struct tcap_info_te {
} tcap_info_te_t;
typedef struct tcap_info_sp {
} tcap_info_sp_t;
typedef struct tcap_info_sc {
} tcap_info_sc_t;

typedef union tcap_info_obj {
	struct tcap_info_df df;
	struct tcap_info_tc tc;
	struct tcap_info_iv iv;
	struct tcap_info_op op;
	struct tcap_info_dg dg;
	struct tcap_info_ac ac;
	struct tcap_info_tr tr;
	struct tcap_info_te te;
	struct tcap_info_sp sp;
	struct tcap_info_sc sc;
} tcap_info_obj_t;

typedef struct tcap_info {
	uint type;			/* object type */
	uint id;			/* object identifier */
	uint cmd;			/* command */
	/* followed by object-specific structure */
	union tcap_info_obj info[0];
} tcap_info_t;

#define TCAP_IOCGINFO	_IOWR(TCAP_IOC_MAGIC,	 0, tcap_info_t)	/* get */

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
typedef struct tcap_opt_conf_df {
} tcap_opt_conf_df_t;
typedef struct tcap_opt_conf_tc {
} tcap_opt_conf_tc_t;
typedef struct tcap_opt_conf_iv {
} tcap_opt_conf_iv_t;
typedef struct tcap_opt_conf_op {
} tcap_opt_conf_op_t;
typedef struct tcap_opt_conf_dg {
} tcap_opt_conf_dg_t;
typedef struct tcap_opt_conf_ac {
} tcap_opt_conf_ac_t;
typedef struct tcap_opt_conf_tr {
} tcap_opt_conf_tr_t;
typedef struct tcap_opt_conf_te {
} tcap_opt_conf_te_t;
typedef struct tcap_opt_conf_sp {
} tcap_opt_conf_sp_t;
typedef struct tcap_opt_conf_sc {
} tcap_opt_conf_sc_t;

typedef union tcap_option_obj {
	struct tcap_opt_conf_df df;
	struct tcap_opt_conf_tc tc;
	struct tcap_opt_conf_iv iv;
	struct tcap_opt_conf_op op;
	struct tcap_opt_conf_dg dg;
	struct tcap_opt_conf_ac ac;
	struct tcap_opt_conf_tr tr;
	struct tcap_opt_conf_te te;
	struct tcap_opt_conf_sp sp;
	struct tcap_opt_conf_sc sc;
} tcap_option_obj_t;

typedef struct tcap_option {
	uint type;			/* object type */
	uint id;			/* object id */
	/* followed by specific proto structure */
	union tcap_option_obj options[0];
} tcap_option_t;

#define TCAP_IOCGOPTION	_IOWR(TCAP_IOC_MAGIC,	 1, tcap_option_t)	/* get */
#define TCAP_IOCSOPTION	_IOWR(TCAP_IOC_MAGIC,	 2, tcap_option_t)	/* set */

/*
 * CONFIGURATION
 *
 * Configures the object (creates, destroys, moves).  These actions require
 * reconfiguration of the object and a change in its relationship to other
 * objects.  Also, this structure contains information that must be specified
 * during creation of the object.  Each structure can optionally be followed by
 * an options structure to specify the options during creation.
 */
typedef struct tcap_conf_df {
	lmi_option_t proto;
} tcap_conf_df_t;
typedef struct tcap_conf_tc {
} tcap_conf_tc_t;
typedef struct tcap_conf_iv {
} tcap_conf_iv_t;
typedef struct tcap_conf_op {
} tcap_conf_op_t;
typedef struct tcap_conf_dg {
} tcap_conf_dg_t;
typedef struct tcap_conf_ac {
} tcap_conf_ac_t;
typedef struct tcap_conf_tr {
} tcap_conf_tr_t;
typedef struct tcap_conf_te {
} tcap_conf_te_t;
typedef struct tcap_conf_sp {
	struct sccp_addr add;
	uchar addr[SCCP_MAX_ADDR_LENGTH];
	lmi_option_t proto;
} tcap_conf_sp_t;
typedef struct tcap_conf_sc {
	int muxid;
	uint spid;
	lmi_option_t proto;
} tcap_conf_sc_t;

typedef union tcap_conf_obj {
	struct tcap_conf_df df;
	struct tcap_conf_tc tc;
	struct tcap_conf_iv iv;
	struct tcap_conf_op op;
	struct tcap_conf_dg dg;
	struct tcap_conf_ac ac;
	struct tcap_conf_tr tr;
	struct tcap_conf_te te;
	struct tcap_conf_sp sp;
	struct tcap_conf_sc sc;
} tcap_conf_obj_t;

#define TCAP_GET	0	/* get options or configuration */
#define TCAP_ADD	1	/* add configuration */
#define TCAP_CHA	2	/* set options or change configuration */
#define TCAP_DEL	3	/* delete configuraiton */
#define TCAP_TST	4	/* test options or configuration */
#define TCAP_COM	5	/* commit options or configuration */

typedef struct tcap_config {
	uint type;			/* object type */
	uint id;			/* object id */
	uint cmd;			/* object command */
	/* followed by specific configuration structure */
	union tcap_conf_obj config[0];
} tcap_config_t;

#define TCAP_IOCLCONFIG	_IOWR(TCAP_IOC_MAGIC,	 3, tcap_config_t)	/* list */
#define TCAP_IOCGCONFIG	_IOWR(TCAP_IOC_MAGIC,	 4, tcap_config_t)	/* get */
#define TCAP_IOCSCONFIG	_IOWR(TCAP_IOC_MAGIC,	 5, tcap_config_t)	/* set */
#define TCAP_IOCTCONFIG	_IOWR(TCAP_IOC_MAGIC,	 6, tcap_config_t)	/* test */
#define TCAP_IOCCCONFIG	_IOWR(TCAP_IOC_MAGIC,	 7, tcap_config_t)	/* commit */

/*
 * STATE
 *
 * Provides state machine state information.  The purpose of these items is
 * primarily for debugging: to view the internal state of the state machine.
 * Some of these state variables might also be reflected in status fields.
 */
typedef struct tcap_timers_df {
} tcap_timers_df_t;
typedef struct tcap_timers_tc {
} tcap_timers_tc_t;
typedef struct tcap_timers_iv {
} tcap_timers_iv_t;
typedef struct tcap_timers_op {
} tcap_timers_op_t;
typedef struct tcap_timers_dg {
} tcap_timers_dg_t;
typedef struct tcap_timers_ac {
} tcap_timers_ac_t;
typedef struct tcap_timers_tr {
} tcap_timers_tr_t;
typedef struct tcap_timers_te {
} tcap_timers_te_t;
typedef struct tcap_timers_sp {
} tcap_timers_sp_t;
typedef struct tcap_timers_sc {
} tcap_timers_sc_t;

typedef struct tcap_statem_df {
	struct tcap_timers_df timers;
} tcap_statem_df_t;
typedef struct tcap_statem_tc {
	struct tcap_timers_tc timers;
} tcap_statem_tc_t;
typedef struct tcap_statem_iv {
	struct tcap_timers_iv timers;
} tcap_statem_iv_t;
typedef struct tcap_statem_op {
	struct tcap_timers_op timers;
} tcap_statem_op_t;
typedef struct tcap_statem_dg {
	struct tcap_timers_dg timers;
} tcap_statem_dg_t;
typedef struct tcap_statem_ac {
	struct tcap_timers_ac timers;
} tcap_statem_ac_t;
typedef struct tcap_statem_tr {
	struct tcap_timers_tr timers;
} tcap_statem_tr_t;
typedef struct tcap_statem_te {
	struct tcap_timers_te timers;
} tcap_statem_te_t;
typedef struct tcap_statem_sp {
	struct tcap_timers_sp timers;
} tcap_statem_sp_t;
typedef struct tcap_statem_sc {
	struct tcap_timers_sc timers;
} tcap_statem_sc_t;

typedef union tcap_statem_obj {
	struct tcap_statem_df df;
	struct tcap_statem_tc tc;
	struct tcap_statem_iv iv;
	struct tcap_statem_op op;
	struct tcap_statem_dg dg;
	struct tcap_statem_ac ac;
	struct tcap_statem_tr tr;
	struct tcap_statem_te te;
	struct tcap_statem_sp sp;
	struct tcap_statem_sc sc;
} tcap_statem_obj_t;

typedef struct tcap_statem {
	uint type;			/* object type */
	uint id;			/* object id */
	uint flags;			/* object flags */
	uint state;			/* object state */
	/* followed by object-specific state structure */
	union tcap_statem_obj statem[0];
} tcap_statem_t;

#define TCAP_IOCGSTATEM	_IOWR(TCAP_IOC_MAGIC,	 8, tcap_statem_t)	/* get */
#define TCAP_IOCCMRESET	_IOWR(TCAP_IOC_MAGIC,	 9, tcap_statem_t)	/* master reset */

/*
 * STATUS
 *
 * Provides status on the object.  Many of these are X.721 status fields or
 * object-specific status information.  Only read-write status fields may be
 * altered with a set or clear command.
 */
typedef struct tcap_status_df {
} tcap_status_df_t;
typedef struct tcap_status_tc {
} tcap_status_tc_t;
typedef struct tcap_status_iv {
} tcap_status_iv_t;
typedef struct tcap_status_op {
} tcap_status_op_t;
typedef struct tcap_status_dg {
} tcap_status_dg_t;
typedef struct tcap_status_ac {
} tcap_status_ac_t;
typedef struct tcap_status_tr {
} tcap_status_tr_t;
typedef struct tcap_status_te {
} tcap_status_te_t;
typedef struct tcap_status_sp {
} tcap_status_sp_t;
typedef struct tcap_status_sc {
} tcap_status_sc_t;

typedef union tcap_status_obj {
	struct tcap_status_df df;
	struct tcap_status_tc tc;
	struct tcap_status_iv iv;
	struct tcap_status_op op;
	struct tcap_status_dg dg;
	struct tcap_status_ac ac;
	struct tcap_status_tr tr;
	struct tcap_status_te te;
	struct tcap_status_sp sp;
	struct tcap_status_sc sc;
} tcap_status_obj_t;

typedef struct tcap_status {
	uint type;			/* object type */
	uint id;			/* object identifier */
	/* followed by object-specific structure */
	union tcap_status_obj status[0];
} tcap_status_t;

#define TCAP_IOCGSTATUS	_IOWR(TCAP_IOC_MAGIC,	10, tcap_status_t)	/* get */
#define TCAP_IOCSSTATUS	_IOWR(TCAP_IOC_MAGIC,	11, tcap_status_t)	/* set */
#define TCAP_IOCCSTATUS	_IOWR(TCAP_IOC_MAGIC,	12, tcap_status_t)	/* clear */

/*
 * STATISTICS
 */
typedef struct tcap_stats_df {
} tcap_stats_df_t;
typedef struct tcap_stats_tc {
} tcap_stats_tc_t;
typedef struct tcap_stats_iv {
} tcap_stats_iv_t;
typedef struct tcap_stats_op {
} tcap_stats_op_t;
typedef struct tcap_stats_dg {
	uint dg_13_3;			/* Q.752/13.3 Total comps tx by the node (5,30) */
	uint dg_13_4;			/* Q.752/13.4 Total comps rx by the node (5,30) */
	uint dg_14_3_d;			/* Q.752/14.3 d) TC-u rej rx: iv resource limitation (1+D) */
	uint dg_14_6_d;			/* Q.752/14.6 d) TC-u rej tx: iv resource limitation (1+D) */
	uint dg_14_7;			/* Q.752/14.7 TC_L_CANCEL indications for Op Class 1 (5) */
	uint dg_14_11;			/* Q.752/14.11 Rejects rx (5) */
	uint dg_A_2_a;			/* Q.752/A.2 a) CP PE (rej rx): gp unrec comp */
	uint dg_A_2_b;			/* Q.752/A.2 b) CP PE (rej rx): gp mistyped comp */
	uint dg_A_2_c;			/* Q.752/A.2 c) CP PE (rej rx): gp badly structured comp */
	uint dg_A_2_d;			/* Q.752/A.2 d) CP PE (rej rx): iv unrec invoke id */
	uint dg_A_2_e;			/* Q.752/A.2 e) CP PE (rej rx): rr unrec comp */
	uint dg_A_2_f;			/* Q.752/A.2 f) CP PE (rej rx): rr RR unexp */
	uint dg_A_2_g;			/* Q.752/A.2 g) CP PE (rej rx): re unrec invoke id */
	uint dg_A_2_h;			/* Q.752/A.2 h) CP PE (rej rx): re RE unexp */
	uint dg_A_3_a;			/* Q.752/A.3 a) TC-u rej rx: iv duplicate invoke id */
	uint dg_A_3_b;			/* Q.752/A.3 b) TC-u rej rx: iv unrec op */
	uint dg_A_3_c;			/* Q.752/A.3 c) TC-u rej rx: iv mistyped parm */
	uint dg_A_3_d;			/* Q.752/A.3 d) TC-u rej rx: iv initiating release */
	uint dg_A_3_e;			/* Q.752/A.3 e) TC-u rej rx: iv linked response unexp */
	uint dg_A_3_f;			/* Q.752/A.3 f) TC-u rej rx: iv unexp linked op */
	uint dg_A_3_g;			/* Q.752/A.3 g) TC-u rej rx: re unrec error */
	uint dg_A_3_h;			/* Q.752/A.3 h) TC-u rej rx: re unexp error */
	uint dg_A_3_i;			/* Q.752/A.3 i) TC-u rej rx: rr mistyped parm */
	uint dg_A_3_j;			/* Q.752/A.3 j) TC-u rej rx: re mistyped parm */
	uint dg_A_5_a;			/* Q.752/A.5 a) CP PE (rej tx): gp unrec comp */
	uint dg_A_5_b;			/* Q.752/A.5 b) CP PE (rej tx): gp mistyped comp */
	uint dg_A_5_c;			/* Q.752/A.5 c) CP PE (rej tx): gp badly structured comp */
	uint dg_A_5_d;			/* Q.752/A.5 d) CP PE (rej tx): iv unrec invoke id */
	uint dg_A_5_e;			/* Q.752/A.5 e) CP PE (rej tx): rr unrec comp */
	uint dg_A_5_f;			/* Q.752/A.5 f) CP PE (rej tx): rr RR unexp */
	uint dg_A_5_g;			/* Q.752/A.5 g) CP PE (rej tx): re unrec invoke id */
	uint dg_A_5_h;			/* Q.752/A.5 h) CP PE (rej tx): re RE unexp */
	uint dg_A_6_a;			/* Q.752/A.6 a) TC-u rej tx: iv duplicate invoke id */
	uint dg_A_6_b;			/* Q.752/A.6 b) TC-u rej tx: iv unrec op */
	uint dg_A_6_c;			/* Q.752/A.6 c) TC-u rej tx: iv mistyped parm */
	uint dg_A_6_d;			/* Q.752/A.6 d) TC-u rej tx: iv initiating release */
	uint dg_A_6_e;			/* Q.752/A.6 e) TC-u rej tx: iv linked response unexp */
	uint dg_A_6_f;			/* Q.752/A.6 f) TC-u rej tx: iv unexp linked op */
	uint dg_A_6_g;			/* Q.752/A.6 g) TC-u rej tx: re unrec error */
	uint dg_A_6_h;			/* Q.752/A.6 h) TC-u rej tx: re unexp error */
	uint dg_A_6_i;			/* Q.752/A.6 i) TC-u rej tx: rr mistyped parm */
	uint dg_A_6_j;			/* Q.752/A.6 j) TC-u rej tx: re mistyped parm */
} tcap_stats_dg_t;
typedef struct tcap_stats_ac {
} tcap_stats_ac_t;
typedef struct tcap_stats_tr {
} tcap_stats_tr_t;
typedef struct tcap_stats_te {
	uint s_msgs_uni;		/* Q.752/13.1 Total TC msgs tx by the node (by MT) (5,30) */
	uint s_msgs_begin;		/* Q.752/13.1 Total TC msgs tx by the node (by MT) (5,30) */
	uint s_msgs_cont;		/* Q.752/13.1 Total TC msgs tx by the node (by MT) (5,30) */
	uint s_msgs_end;		/* Q.752/13.1 Total TC msgs tx by the node (by MT) (5,30) */
	uint s_msgs_abort;		/* Q.752/13.1 Total TC msgs tx by the node (by MT) (5,30) */
	uint r_msgs_uni;		/* Q.752/13.2 Total TC msgs rx by the node (by MT) (5,30) */
	uint r_msgs_begin;		/* Q.752/13.2 Total TC msgs rx by the node (by MT) (5,30) */
	uint r_msgs_cont;		/* Q.752/13.2 Total TC msgs rx by the node (by MT) (5,30) */
	uint r_msgs_end;		/* Q.752/13.2 Total TC msgs rx by the node (by MT) (5,30) */
	uint r_msgs_abort;		/* Q.752/13.2 Total TC msgs rx by the node (by MT) (5,30) */
	uint s_msgs_all;		/* Q.752/13.1 bis Total TC msgs tx by the node (5,30) */
	uint r_msgs_all;		/* Q.752/13.2 bis Total TC msgs rx by the node (5,30) */
	uint s_comps_all;		/* Q.752/13.3 Total comps tx by the node (5,30) */
	uint r_comps_all;		/* Q.752/13.4 Total comps rx by the node (5,30) */
	/* The following are in units of Transactions per Application Entity: */
	uint new_transactions;		/* Q.752/13.6 New trans in the interval (5) */
	uint mean_open_tids;		/* Q.752/13.7 Mean open trans ids in interval (5) */
	uint trans_cumm_mean_duration;	/* Q.752/13.9 Cumulative mean duration of trans */
	uint max_open_tids;		/* Q.752/13.10 Maximum open trans ids during interval (5) */
	/* (threshold pre-defined per AE). */
	uint open_tid_thresh_exceeded;	/* Q.752/13.11 Open trans ids threshold (occur) */
	uint r_abt_unrec_tid;		/* Q.752/14.1 d) TP PE (abt rx): unrec TID */
	uint r_abt_resource_lim;	/* Q.752/14.1 e) TP PE (abt rx): resource lim */
	uint r_rej_resource_lim;	/* Q.752/14.3 d) TC-U rej rx: iv resource lim */
	uint s_abt_unrec_tid;		/* Q.752/14.4 d) TP PE (abt tx): unrec TID */
	uint s_abt_resource_lim;	/* Q.752/14.4 e) TP PE (abt tx): resource lim */
	uint s_rej_resouce_lim;		/* Q.752/14.6 d) TC-U rej tx: iv resource lim */
	uint l_cancel_inds;		/* Q.752/14.7 TC_L_CANCEL indications for Class 1 op (5) */
	uint r_msgs_discard;		/* Q.752/14.8 Msgs discarded (all reasons) (5) */
	uint r_msgs_pabort;		/* Q.752/14.10 Provider abt rx (5) */
	uint r_comp_reject;		/* Q.752/14.11 Rej rx (5) */
	uint r_tp_errors;		/* Q.752/14.12 Errors detected in TP (5) */
	/* All of these are in units of events, first and delta: */
	uint r_unrec_mt;		/* Q.752/A.1 a) TP PE (abt rx): unrec MT */
	uint r_incorrect_tp;		/* Q.752/A.1 b) TP PE (abt rx): incorrect TP */
	uint r_badly_formatted_tp;	/* Q.752/A.1 c) TP PE (abt rx): badly formatted TP */
	uint r_gp_unrec_comp;		/* Q.752/A.2 a) CP PE (rej rx): gp unrec comp */
	uint r_gp_mistyped_comp;	/* Q.752/A.2 b) CP PE (rej rx): gp mistyped comp */
	uint r_gp_bad_struct_comp;	/* Q.752/A.2 c) CP PE (rej rx): gp bad struct comp */
	uint r_gp_unrec_invk_id;	/* Q.752/A.2 d) CP PE (rej rx): iv unrec invk id */
	uint r_rr_unrec_comp;		/* Q.752/A.2 e) CP PE (rej rx): rr unrec comp */
	uint r_rr_unexp;		/* Q.752/A.2 f) CP PE (rej rx): rr RR unexp */
	uint r_re_unrec_invk_id;	/* Q.752/A.2 g) CP PE (rej rx): re unrec invk id */
	uint r_re_re_unexp;		/* Q.752/A.2 h) CP PE (rej rx): re RE unexp */
	uint r_iv_dup_invk_id;		/* Q.752/A.3 a) TC-U rej rx: iv dup invk id */
	uint r_iv_unrec_op;		/* Q.752/A.3 b) TC-U rej rx: iv unrec op */
	uint r_iv_mistyped_parm;	/* Q.752/A.3 c) TC-U rej rx: iv mistyped parm */
	uint r_iv_initiating_rlse;	/* Q.752/A.3 d) TC-U rej rx: iv initiating release */
	uint r_iv_link_resp_unexp;	/* Q.752/A.3 e) TC-U rej rx: iv linked resp unexp */
	uint r_iv_unexp_linked_op;	/* Q.752/A.3 f) TC-U rej rx: iv unexp linked op */
	uint r_re_unrec_error;		/* Q.752/A.3 g) TC-U rej rx: re unrec error */
	uint r_re_unexp_error;		/* Q.752/A.3 h) TC-U rej rx: re unexp error */
	uint r_rr_mistyped_parm;	/* Q.752/A.3 i) TC-U rej rx: rr mistyped parm */
	uint r_re_mistyped_parm;	/* Q.752/A.3 j) TC-U rej rx: re mistyped parm */
	/* The following are in units of events/dest,User, first and delta: */
	uint s_unrec_mt;		/* Q.752/A.4 a) TP PE (abt tx): unrec MT */
	uint s_incorrect_tp;		/* Q.752/A.4 b) TP PE (abt tx): incorrect TP */
	uint s_badly_formatted_tp;	/* Q.752/A.4 c) TP PE (abt tx): bad formatted TP */
	/* The following are in units of events, first and delta: */
	uint s_gp_unrec_comp;		/* Q.752/A.5 a) CP PE (rej tx): gp unrec comp */
	uint s_gp_mistyped_comp;	/* Q.752/A.5 b) CP PE (rej tx): gp mistyped comp */
	uint s_gp_bad_struct_comp;	/* Q.752/A.5 c) CP PE (rej tx): gp bad struct comp */
	uint s_gp_unrec_invk_id;	/* Q.752/A.5 d) CP PE (rej tx): iv unrec invk id */
	uint s_rr_unrec_comp;		/* Q.752/A.5 e) CP PE (rej tx): rr unrec comp */
	uint s_rr_unexp;		/* Q.752/A.5 f) CP PE (rej tx): rr RR unexp */
	uint s_re_unrec_invk_id;	/* Q.752/A.5 g) CP PE (rej tx): re unrec invk id */
	uint s_re_re_unexp;		/* Q.752/A.5 h) CP PE (rej tx): re RE unexp */
	/* The following are in units of events/dest,User, first and delta: */
	uint s_iv_dup_invk_id;		/* Q.752/A.6 a) TC-U rej tx: iv dup invk id */
	uint s_iv_unrec_op;		/* Q.752/A.6 b) TC-U rej tx: iv unrec op */
	uint s_iv_mistyped_parm;	/* Q.752/A.6 c) TC-U rej tx: iv mistyped parm */
	uint s_iv_initiating_rlse;	/* Q.752/A.6 d) TC-U rej tx: iv initiating release */
	uint s_iv_link_resp_unexp;	/* Q.752/A.6 e) TC-U rej tx: iv linked resp unexp */
	uint s_iv_unexp_linked_op;	/* Q.752/A.6 f) TC-U rej tx: iv unexp linked op */
	uint s_re_unrec_error;		/* Q.752/A.6 g) TC-U rej tx: re unrec error */
	uint s_re_unexp_error;		/* Q.752/A.6 h) TC-U rej tx: re unexp error */
	uint s_rr_mistyped_parm;	/* Q.752/A.6 i) TC-U rej tx: rr mistyped parm */
	uint s_re_mistyped_parm;	/* Q.752/A.6 j) TC-U rej tx: re mistyped parm */
} tcap_stats_te_t;
typedef struct tcap_stats_sp {
} tcap_stats_sp_t;
typedef struct tcap_stats_sc {
} tcap_stats_sc_t;

typedef union tcap_stats_obj {
	struct tcap_stats_df df;
	struct tcap_stats_tc tc;
	struct tcap_stats_iv iv;
	struct tcap_stats_op op;
	struct tcap_stats_dg dg;
	struct tcap_stats_ac ac;
	struct tcap_stats_tr tr;
	struct tcap_stats_te te;
	struct tcap_stats_sp sp;
	struct tcap_stats_sc sc;
} tcap_stats_obj_t;

typedef struct tcap_stats {
	uint type;			/* object type */
	uint id;			/* object id */
	uint header;			/* object stats header */
	/* followed by object-specific statistics structure */
	union tcap_stats_obj stats[0];
} tcap_stats_t;

#define TCAP_IOCGSTATSP	_IOWR(TCAP_IOC_MAGIC,	13,  tcap_stats_t) /* get period */
#define TCAP_IOCSSTATSP	_IOWR(TCAP_IOC_MAGIC,	14,  tcap_stats_t) /* set period */
#define TCAP_IOCGSTATS	_IOWR(TCAP_IOC_MAGIC,	15,  tcap_stats_t) /* get stats */
#define TCAP_IOCCSTATS	_IOWR(TCAP_IOC_MAGIC,	16,  tcap_stats_t) /* get and clear stats */

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
typedef struct tcap_notify_df {
	uint events;
} tcap_notify_df_t;
typedef struct tcap_notify_tc {
	uint events;
} tcap_notify_tc_t;
typedef struct tcap_notify_iv {
	uint events;
} tcap_notify_iv_t;
typedef struct tcap_notify_op {
	uint events;
} tcap_notify_op_t;
typedef struct tcap_notify_dg {
	uint events;
} tcap_notify_dg_t;
typedef struct tcap_notify_ac {
	uint events;
} tcap_notify_ac_t;
typedef struct tcap_notify_tr {
	uint events;
} tcap_notify_tr_t;
typedef struct tcap_notify_te {
	uint events;
} tcap_notify_te_t;
typedef struct tcap_notify_sp {
	uint events;
} tcap_notify_sp_t;
typedef struct tcap_notify_sc {
	uint events;
} tcap_notify_sc_t;

/*
   The following are the Q.752 "occurence" or "first & interval" events associated with TC
   montitoring and development fault detection.  The default is not to report any events at all (but
   to merely use the strlog(9) facility to report the "occurence" or "first" part of the "first &
   interval").  When delivered, the Table 13 and Table 14 events are reported durning normal
   operating conditions, whereas the Annex A events are normally reported only during development
   fault detection.
 */

#define TC_EVT_OPEN_TID_THRESH_EXCEEDED	(1<< 0)	/* Q.752/13.11 Open trans ids threshold (occur) */
#define TC_EVT_RECV_ABT_UNREC_TID	(1<< 1)	/* Q.752/14.1 d) TP PE (abt rx): unrec TID */
#define TC_EVT_SEND_ABT_UNREC_TID	(1<< 1)	/* Q.752/14.4 d) TP PE (abt tx): unrec TID */
#define TC_EVT_RECV_ABT_RESOURCE_LIM	(1<< 2)	/* Q.752/14.1 e) TP PE (abt rx): resource lim */
#define TC_EVT_SEND_ABT_RESOURCE_LIM	(1<< 2)	/* Q.752/14.4 e) TP PE (abt tx): resource lim */
#define TC_EVT_RECV_REJ_RESOURCE_LIM	(1<< 3)	/* Q.752/14.3 d) TC-U rej rx: iv resource lim */
#define TC_EVT_RECV_REJ_RESOURCE_LIM	(1<< 3)	/* Q.752/14.6 d) TC-U rej tx: iv resource lim */

/*
   The following Annex A "first & interval" events are associated with TC development fault
   detection.  The default is to not report any of these events during normal operation, but to only
   report these events when explicitly wishing to perform development fault detection.
 */

#define TC_EVT_RECV_UNREC_MT		(1<< 4)	/* Q.752/A.1 a) TP PE (abt rx): unrec MT */
#define TC_EVT_SEND_UNREC_MT		(1<< 4)	/* Q.752/A.4 a) TP PE (abt tx): unrec MT */
#define TC_EVT_RECV_INCORRECT_TP	(1<< 5)	/* Q.752/A.1 b) TP PE (abt rx): incorrect TP */
#define TC_EVT_SEND_INCORRECT_TP	(1<< 5)	/* Q.752/A.4 b) TP PE (abt tx): incorrect TP */
#define TC_EVT_RECV_BADLY_FORMATTED_TP	(1<< 6)	/* Q.752/A.1 c) TP PE (abt rx): badly formatted TP */
#define TC_EVT_SEND_BADLY_FORMATTED_TP	(1<< 6)	/* Q.752/A.4 c) TP PE (abt tx): badly formatted TP */
#define TC_EVT_RECV_GP_UNREC_COMP	(1<< 7)	/* Q.752/A.2 a) CP PE (rej rx): gp unrec comp */
#define TC_EVT_SEND_GP_UNREC_COMP	(1<< 7)	/* Q.752/A.5 a) CP PE (rej tx): gp unrec comp */
#define TC_EVT_RECV_GP_MISTYPED_COMP	(1<< 8)	/* Q.752/A.2 b) CP PE (rej rx): gp mistyped comp */
#define TC_EVT_SEND_GP_MISTYPED_COMP	(1<< 8)	/* Q.752/A.5 b) CP PE (rej tx): gp mistyped comp */
#define TC_EVT_RECV_GP_BAD_STRUCT_COMP	(1<< 9)	/* Q.752/A.2 c) CP PE (rej rx): gp bad struct comp */
#define TC_EVT_SEND_GP_BAD_STRUCT_COMP	(1<< 9)	/* Q.752/A.5 c) CP PE (rej tx): gp bad struct comp */
#define TC_EVT_RECV_IV_UNREC_INVK_ID	(1<<10)	/* Q.752/A.2 d) CP PE (rej rx): iv unrec invk id */
#define TC_EVT_SEND_IV_UNREC_INVK_ID	(1<<10)	/* Q.752/A.5 d) CP PE (rej tx): iv unrec invk id */
#define TC_EVT_RECV_RR_UNREC_COMP	(1<<11)	/* Q.752/A.2 e) CP PE (rej rx): rr unrec comp */
#define TC_EVT_SEND_RR_UNREC_COMP	(1<<11)	/* Q.752/A.5 e) CP PE (rej tx): rr unrec comp */
#define TC_EVT_RECV_RR_UNEXP		(1<<12)	/* Q.752/A.2 f) CP PE (rej rx): rr RR unexp */
#define TC_EVT_SEND_RR_UNEXP		(1<<12)	/* Q.752/A.5 f) CP PE (rej tx): rr RR unexp */
#define TC_EVT_RECV_RE_UNREC_INVK_ID	(1<<13)	/* Q.752/A.2 g) CP PE (rej rx): re unrec invk id */
#define TC_EVT_SEND_RE_UNREC_INVK_ID	(1<<13)	/* Q.752/A.5 g) CP PE (rej tx): re unrec invk id */
#define TC_EVT_RECV_RE_RE_UNEXP		(1<<14)	/* Q.752/A.2 h) CP PE (rej rx): re RE unexp */
#define TC_EVT_RECV_RE_RE_UNEXP		(1<<14)	/* Q.752/A.5 h) CP PE (rej tx): re RE unexp */
#define TC_EVT_RECV_IV_DUP_INVK_ID	(1<<15)	/* Q.752/A.3 a) TC-U rej rx: iv dup invk id */
#define TC_EVT_SEND_IV_DUP_INVK_ID	(1<<15)	/* Q.752/A.6 a) TC-U rej tx: iv dup invk id */
#define TC_EVT_RECV_IV_UNREC_OP		(1<<16)	/* Q.752/A.3 b) TC-U rej rx: iv unrec op */
#define TC_EVT_SEND_IV_UNREC_OP		(1<<16)	/* Q.752/A.6 b) TC-U rej tx: iv unrec op */
#define TC_EVT_RECV_IV_MISTYPED_PARM	(1<<17)	/* Q.752/A.3 c) TC-U rej rx: iv mistyped parm */
#define TC_EVT_SEND_IV_MISTYPED_PARM	(1<<17)	/* Q.752/A.6 c) TC-U rej tx: iv mistyped parm */
#define TC_EVT_RECV_IV_INITIATING_RLSE	(1<<18)	/* Q.752/A.3 d) TC-U rej rx: iv initiating rlse */
#define TC_EVT_SEND_IV_INITIATING_RLSE	(1<<18)	/* Q.752/A.6 d) TC-U rej tx: iv initiating rlse */
#define TC_EVT_RECV_IV_LINK_RESP_UNEXP	(1<<19)	/* Q.752/A.3 e) TC-U rej rx: iv link resp unexp */
#define TC_EVT_SEND_IV_LINK_RESP_UNEXP	(1<<19)	/* Q.752/A.6 e) TC-U rej tx: iv link resp unexp */
#define TC_EVT_RECV_IV_UNEXP_LINKED_OP	(1<<20)	/* Q.752/A.3 f) TC-U rej rx: iv unexp linked op */
#define TC_EVT_SEND_IV_UNEXP_LINKED_OP	(1<<20)	/* Q.752/A.6 f) TC-U rej tx: iv unexp linked op */
#define TC_EVT_RECV_RE_UNREC_ERROR	(1<<21)	/* Q.752/A.3 g) TC-U rej rx: re unrec error */
#define TC_EVT_SEND_RE_UNREC_ERROR	(1<<21)	/* Q.752/A.6 g) TC-U rej tx: re unrec error */
#define TC_EVT_RECV_RE_UNEXP_ERROR	(1<<22)	/* Q.752/A.3 h) TC-U rej rx: re unexp error */
#define TC_EVT_SEND_RE_UNEXP_ERROR	(1<<22)	/* Q.752/A.6 h) TC-U rej tx: re unexp error */
#define TC_EVT_RECV_RR_MISTYPED_PARM	(1<<23)	/* Q.752/A.3 i) TC-U rej rx: rr mistyped parm */
#define TC_EVT_SEND_RR_MISTYPED_PARM	(1<<23)	/* Q.752/A.6 i) TC-U rej tx: rr mistyped parm */
#define TC_EVT_RECV_RE_MISTYPED_PARM	(1<<24)	/* Q.752/A.3 j) TC-U rej rx: re mistyped parm */
#define TC_EVT_SEND_RE_MISTYPED_PARM	(1<<24)	/* Q.752/A.6 j) TC-U rej tx: re mistyped parm */


typedef union tcap_notify_obj {
	struct tcap_notify_df df;
	struct tcap_notify_tc tc;
	struct tcap_notify_iv iv;
	struct tcap_notify_op op;
	struct tcap_notify_dg dg;
	struct tcap_notify_ac ac;
	struct tcap_notify_tr tr;
	struct tcap_notify_te te;
	struct tcap_notify_sp sp;
	struct tcap_notify_sc sc;
} tcap_notify_obj_t;

typedef struct tcap_notify {
	uint type;			/* object type */
	uint id;			/* object id */
	/* followed by object-specific notification structure */
	union tcap_notify_obj notify[0];
} tcap_notify_t;

#define TCAP_IOCGNOTIFY	_IOWR(TCAP_IOC_MAGIC,	17,  tcap_notify_t)	/* get */
#define TCAP_IOCSNOTIFY	_IOWR(TCAP_IOC_MAGIC,	18,  tcap_notify_t)	/* set */
#define TCAP_IOCCNOTIFY	_IOWR(TCAP_IOC_MAGIC,	19,  tcap_notify_t)	/* clear */

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
typedef struct tcap_attr_df {
	struct tcap_conf_df config;
	struct tcap_opt_conf_df option;
	struct tcap_info_df inform;
	struct tcap_statem_df statem;
	struct tcap_status_df status;
	struct tcap_stats_df stats;
	struct tcap_notify_df events;
} tcap_attr_df_t;
typedef struct tcap_attr_tc {
	struct tcap_conf_tc config;
	struct tcap_opt_conf_tc option;
	struct tcap_info_tc inform;
	struct tcap_statem_tc statem;
	struct tcap_status_tc status;
	struct tcap_stats_tc stats;
	struct tcap_notify_tc events;
} tcap_attr_tc_t;
typedef struct tcap_attr_iv {
	struct tcap_conf_iv config;
	struct tcap_opt_conf_iv option;
	struct tcap_info_iv inform;
	struct tcap_statem_iv statem;
	struct tcap_status_iv status;
	struct tcap_stats_iv stats;
	struct tcap_notify_iv events;
} tcap_attr_iv_t;
typedef struct tcap_attr_op {
	struct tcap_conf_op config;
	struct tcap_opt_conf_op option;
	struct tcap_info_op inform;
	struct tcap_statem_op statem;
	struct tcap_status_op status;
	struct tcap_stats_op stats;
	struct tcap_notify_op events;
} tcap_attr_op_t;
typedef struct tcap_attr_dg {
	struct tcap_conf_dg config;
	struct tcap_opt_conf_dg option;
	struct tcap_info_dg inform;
	struct tcap_statem_dg statem;
	struct tcap_status_dg status;
	struct tcap_stats_dg stats;
	struct tcap_notify_dg events;
} tcap_attr_dg_t;
typedef struct tcap_attr_ac {
	struct tcap_conf_ac config;
	struct tcap_opt_conf_ac option;
	struct tcap_info_ac inform;
	struct tcap_statem_ac statem;
	struct tcap_status_ac status;
	struct tcap_stats_ac stats;
	struct tcap_notify_ac events;
} tcap_attr_ac_t;
typedef struct tcap_attr_tr {
	struct tcap_conf_tr config;
	struct tcap_opt_conf_tr option;
	struct tcap_info_tr inform;
	struct tcap_statem_tr statem;
	struct tcap_status_tr status;
	struct tcap_stats_tr stats;
	struct tcap_notify_tr events;
} tcap_attr_tr_t;
typedef struct tcap_attr_te {
	struct tcap_conf_te config;
	struct tcap_opt_conf_te option;
	struct tcap_info_te inform;
	struct tcap_statem_te statem;
	struct tcap_status_te status;
	struct tcap_stats_te stats;
	struct tcap_notify_te events;
} tcap_attr_te_t;
typedef struct tcap_attr_sp {
	struct tcap_conf_sp config;
	struct tcap_opt_conf_sp option;
	struct tcap_info_sp inform;
	struct tcap_statem_sp statem;
	struct tcap_status_sp status;
	struct tcap_stats_sp stats;
	struct tcap_notify_sp events;
} tcap_attr_sp_t;
typedef struct tcap_attr_sc {
} tcap_attr_sc_t;
	struct tcap_conf_sc config;
	struct tcap_opt_conf_sc option;
	struct tcap_info_sc inform;
	struct tcap_statem_sc statem;
	struct tcap_status_sc status;
	struct tcap_stats_sc stats;
	struct tcap_notify_sc events;

typedef union tcap_attr_obj {
	struct tcap_attr_df df;
	struct tcap_attr_tc tc;
	struct tcap_attr_iv iv;
	struct tcap_attr_op op;
	struct tcap_attr_dg dg;
	struct tcap_attr_ac ac;
	struct tcap_attr_tr tr;
	struct tcap_attr_te te;
	struct tcap_attr_sp sp;
	struct tcap_attr_sc sc;
} tcap_attr_obj_t;

typedef struct tcap_attr {
	uint type;			/* object type */
	uint id;			/* object identifier */
	uint cmd;			/* command */
	/* followed by object-specific structure */
	union tcap_attr_obj attrs[0];
} tcap_attr_t;

#define	TCAP_IOCGATTR	_IOWR(TCAP_IOC_MAGIC,	20, tcap_attr_t)	/* get attributes */

/*
 *  MANAGEMENT
 */
typedef struct tcap_action_df {
} tcap_action_df_t;
typedef struct tcap_action_tc {
} tcap_action_tc_t;
typedef struct tcap_action_iv {
} tcap_action_iv_t;
typedef struct tcap_action_op {
} tcap_action_op_t;
typedef struct tcap_action_dg {
} tcap_action_dg_t;
typedef struct tcap_action_ac {
} tcap_action_ac_t;
typedef struct tcap_action_tr {
} tcap_action_tr_t;
typedef struct tcap_action_te {
} tcap_action_te_t;
typedef struct tcap_action_sp {
} tcap_action_sp_t;
typedef struct tcap_action_sc {
} tcap_action_sc_t;

typedef union tcap_action_obj {
	struct tcap_action_df df;
	struct tcap_action_tc tc;
	struct tcap_action_iv iv;
	struct tcap_action_op op;
	struct tcap_action_dg dg;
	struct tcap_action_ac ac;
	struct tcap_action_tr tr;
	struct tcap_action_te te;
	struct tcap_action_sp sp;
	struct tcap_action_sc sc;
} tcap_action_obj_t;

typedef struct tcap_mgmt {
	uint type;			/* object type */
	uint id;			/* object id */
	uint cmd;			/* command */
	/* followed by object-specific structure */
	union tcap_action_obj action[0];
} tcap_mgmt_t;

#define TCAP_MGMT_BLOCK			1
#define TCAP_MGMT_UNBLOCK		2
#define TCAP_MGMT_RESET			3
#define TCAP_MGMT_QUERY			4

#define TCAP_IOCCMGMT	_IOWR(TCAP_IOC_MAGIC,	21,  tcap_mgmt_t )

/*
 * PASS LOWER
 *
 * This structure is deprecated and as largely used for testing as a mechanism
 * to bypass an input-output control to a stream linked under a multiplexing
 * driver supporting this interface.
 */
typedef struct tcap_pass {
	uint muxid;			/* mux index of lower stream to pass message to */
	uint8_t type;			/* type of message block */
	uint8_t band;			/* band of message block */
	uint ctl_length;		/* length of cntl part */
	uint dat_length;		/* length of data part */
	/* followed by cntl and data part of message to pass */
} tcap_pass_t;

#define TCAP_IOCCPASS	_IOWR(TCAP_IOC_MAGIC,	22, tcap_pass_t)	/* pass */

union tcap_ioctls {
	struct {
		struct tcap_info info;
		union tcap_info_obj obj;
	};
	struct {
		struct tcap_option option;
		union tcap_option_obj obj;
	} opt_conf;
	struct {
		struct tcap_config config;
		union tcap_conf_obj obj;
	} conf;
	struct {
		struct tcap_statem statem;
		union tcap_statem_obj obj;
	} statem;
	struct {
		struct tcap_status status;
		union tcap_status_obj obj;
	} status;
	struct {
		struct tcap_stats stats;
		union tcap_stats_obj obj;
	} stats;
	struct {
		struct tcap_notify notify;
		union tcap_notify_obj obj;
	} notify;
	struct {
		struct tcap_attr attr;
		union tcap_attr_obj obj;
	} attr;
	struct {
		struct tcap_mgmt mgmt;
		union tcap_action_obj obj;
	} mgmt;
	struct tcap_pass pass;
};

#define TCAP_IOC_FIRST		 0
#define TCAP_IOC_LAST		22
#define TCAP_IOC_PRIVATE	32

#endif				/* __TCAP_IOCTL_H__ */
