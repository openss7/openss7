/*****************************************************************************

 @(#) $Id: map_ioctl.h,v 1.1.2.1 2009-06-21 11:25:34 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
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

 Last Modified $Date: 2009-06-21 11:25:34 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: map_ioctl.h,v $
 Revision 1.1.2.1  2009-06-21 11:25:34  brian
 - added files to new distro

 *****************************************************************************/

#ifndef __MAP_IOCTL_H__
#define __MAP_IOCTL_H__

#ident "@(#) $RCSfile: map_ioctl.h,v $ $Name:  $($Revision: 1.1.2.1 $) Copyright (c) 2008-2009 Monavacon Limited."

/* This file can be processed by doxygen(1). */

#include <linux/ioctl.h>

/*
 *  This file is rather Linux specific for now.  Linux encodes direction and
 *  size in upper 16 bits of 32 bit ioctl command.  This should make
 *  transparent ioctl in STREAMS easier for Linux (i.e., all transparent
 *  ioctls could be converted to I_STR types because size is known).
 *
 *  If using SVR4.2, perhaps these macros should be redefined to something
 *  which does not accept size and transpartent ioctls would have to be
 *  provided for in the STREAMS code anyways.  Then it would work either way.
 *
 *  Following defines would do the job...
 *
 *  #define _IO(magic,nr)        ((magic<<8)+nr)
 *  #define _IOR(magic,nr,size)  ((magic<<8)+nr)
 *  #define _IOW(magic,nr,size)  ((magic<<8)+nr)
 *  #define _IOWR(magic,nr,size) ((magic<<8)+nr)
 */

#define MAP_IOC_MAGIC 'M'

#define MAP_OBJ_TYPE_DF		0	/* default */
#define MAP_OBJ_TYPE_CM		1	/* component */
#define MAP_OBJ_TYPE_IV		2	/* invoke */
#define MAP_OBJ_TYPE_OP		3	/* operation */
#define MAP_OBJ_TYPE_DG		4	/* dialog */
#define MAP_OBJ_TYPE_AC		5	/* application context */
#define MAP_OBJ_TYPE_TR		6	/* transaction */
#define MAP_OBJ_TYPE_ME		7	/* MAP Entity */
#define MAP_OBJ_TYPE_SP		8	/* SCCP SAP */
#define MAP_OBJ_TYPE_TC		9	/* TC Entity */

/*
 * INFORMATION
 *
 * Provides read-only information that is not available for the manager to
 * configure.  Also, this information does not change while the object exists:
 * it is purely static for the lifetime of the object.
 */
typedef struct map_info_df {
} map_info_df_t;
typedef struct map_info_cm {
} map_info_cm_t;
typedef struct map_info_iv {
} map_info_iv_t;
typedef struct map_info_op {
} map_info_op_t;
typedef struct map_info_dg {
} map_info_dg_t;
typedef struct map_info_ac {
} map_info_ac_t;
typedef struct map_info_tr {
} map_info_tr_t;
typedef struct map_info_me {
} map_info_me_t;
typedef struct map_info_sp {
} map_info_sp_t;
typedef struct map_info_tc {
} map_info_tc_t;

typedef union map_info_obj {
	struct map_info_df df;
	struct map_info_cm cm;
	struct map_info_iv iv;
	struct map_info_op op;
	struct map_info_dg dg;
	struct map_info_ac ac;
	struct map_info_tr tr;
	struct map_info_me me;
	struct map_info_sp sp;
	struct map_info_tc tc;
} map_info_obj_t;

typedef struct map_info {
	uint type;			/* object type */
	uint id;			/* object identifier */
	uint cmd;			/* command */
	/* followed by object-specific structure */
	union map_info_obj info[0];
} map_info_t;

#define MAP_IOCGINFO	_IOWR(MAP_IOC_MAGIC,	 0, map_info_t)	/* get */

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
typedef struct map_opt_conf_df {
} map_opt_conf_df_t;
typedef struct map_opt_conf_cm {
} map_opt_conf_cm_t;
typedef struct map_opt_conf_iv {
} map_opt_conf_iv_t;
typedef struct map_opt_conf_op {
} map_opt_conf_op_t;
typedef struct map_opt_conf_dg {
} map_opt_conf_dg_t;
typedef struct map_opt_conf_ac {
} map_opt_conf_ac_t;
typedef struct map_opt_conf_tr {
} map_opt_conf_tr_t;
typedef struct map_opt_conf_me {
} map_opt_conf_me_t;
typedef struct map_opt_conf_sp {
} map_opt_conf_sp_t;
typedef struct map_opt_conf_tc {
} map_opt_conf_tc_t;

typedef union map_option_obj {
	struct map_opt_conf_df df;
	struct map_opt_conf_cm cm;
	struct map_opt_conf_iv iv;
	struct map_opt_conf_op op;
	struct map_opt_conf_dg dg;
	struct map_opt_conf_ac ac;
	struct map_opt_conf_tr tr;
	struct map_opt_conf_me me;
	struct map_opt_conf_sp sp;
	struct map_opt_conf_tc tc;
} map_option_obj_t;

typedef struct map_option {
	uint type;			/* object type */
	uint id;			/* object id */
	/* followed by specific proto structure */
	union map_option_obj options[0];
} map_option_t;

#define MAP_IOCGOPTION	_IOWR(MAP_IOC_MAGIC,	 1, map_option_t)	/* get */
#define MAP_IOCSOPTION	_IOWR(MAP_IOC_MAGIC,	 2, map_option_t)	/* set */

/*
 * CONFIGURATION
 *
 * Configures the object (creates, destroys, moves).  These actions require
 * reconfiguration of the object and a change in its relationship to other
 * objects.  Also, this structure contains information that must be specified
 * during creation of the object.  Each structure can optionally be followed by
 * an options structure to specify the options during creation.
 */
typedef struct map_conf_df {
	lmi_option_t proto;		/* protocol variant and options */
} map_conf_df_t;
typedef struct map_conf_cm {
} map_conf_cm_t;
typedef struct map_conf_iv {
} map_conf_iv_t;
typedef struct map_conf_op {
} map_conf_op_t;
typedef struct map_conf_dg {
} map_conf_dg_t;
typedef struct map_conf_ac {
} map_conf_ac_t;
typedef struct map_conf_tr {
} map_conf_tr_t;
typedef struct map_conf_me {
} map_conf_me_t;
typedef struct map_conf_sp {
} map_conf_sp_t;
typedef struct map_conf_tc {
} map_conf_tc_t;

typedef union map_conf_obj {
	struct map_conf_df df;
	struct map_conf_cm cm;
	struct map_conf_iv iv;
	struct map_conf_op op;
	struct map_conf_dg dg;
	struct map_conf_ac ac;
	struct map_conf_tr tr;
	struct map_conf_me me;
	struct map_conf_sp sp;
	struct map_conf_tc tc;
} map_conf_obj_t;

#define MAP_GET		0	/* get options or configuration */
#define MAP_ADD		1	/* add configuration */
#define MAP_CHA		2	/* set options or change configuration */
#define MAP_DEL		3	/* delete configuraiton */
#define MAP_TST		4	/* test options or configuration */
#define MAP_COM		5	/* commit options or configuration */

typedef struct map_config {
	uint type;			/* object type */
	uint id;			/* object id */
	uint cmd;			/* object command */
	/* followed by specific configuration structure */
	union map_conf_obj config[0];
} map_config_t;

#define MAP_IOCLCONFIG	_IOWR(MAP_IOC_MAGIC,	 3, map_config_t)	/* list */
#define MAP_IOCGCONFIG	_IOWR(MAP_IOC_MAGIC,	 4, map_config_t)	/* get */
#define MAP_IOCSCONFIG	_IOWR(MAP_IOC_MAGIC,	 5, map_config_t)	/* set */
#define MAP_IOCTCONFIG	_IOWR(MAP_IOC_MAGIC,	 6, map_config_t)	/* test */
#define MAP_IOCCCONFIG	_IOWR(MAP_IOC_MAGIC,	 7, map_config_t)	/* commit */

/*
 * STATE
 *
 * Provides state machine state information.  The purpose of these items is
 * primarily for debugging: to view the internal state of the state machine.
 * Some of these state variables might also be reflected in status fields.
 */
typedef struct map_timers_df {
} map_timers_df_t;
typedef struct map_timers_cm {
} map_timers_cm_t;
typedef struct map_timers_iv {
} map_timers_iv_t;
typedef struct map_timers_op {
} map_timers_op_t;
typedef struct map_timers_dg {
} map_timers_dg_t;
typedef struct map_timers_ac {
} map_timers_ac_t;
typedef struct map_timers_tr {
} map_timers_tr_t;
typedef struct map_timers_me {
} map_timers_me_t;
typedef struct map_timers_sp {
} map_timers_sp_t;
typedef struct map_timers_tc {
} map_timers_tc_t;

typedef struct map_statem_df {
	struct map_timers_df timers;
} map_statem_df_t;
typedef struct map_statem_cm {
	struct map_timers_cm timers;
} map_statem_cm_t;
typedef struct map_statem_iv {
	struct map_timers_iv timers;
} map_statem_iv_t;
typedef struct map_statem_op {
	struct map_timers_op timers;
} map_statem_op_t;
typedef struct map_statem_dg {
	struct map_timers_dg timers;
} map_statem_dg_t;
typedef struct map_statem_ac {
	struct map_timers_ac timers;
} map_statem_ac_t;
typedef struct map_statem_tr {
	struct map_timers_tr timers;
} map_statem_tr_t;
typedef struct map_statem_me {
	struct map_timers_me timers;
} map_statem_me_t;
typedef struct map_statem_sp {
	struct map_timers_sp timers;
} map_statem_sp_t;
typedef struct map_statem_tc {
	struct map_timers_tc timers;
} map_statem_tc_t;

typedef union map_statem_obj {
	struct map_statem_df df;
	struct map_statem_cm cm;
	struct map_statem_iv iv;
	struct map_statem_op op;
	struct map_statem_dg dg;
	struct map_statem_ac ac;
	struct map_statem_tr tr;
	struct map_statem_me me;
	struct map_statem_sp sp;
	struct map_statem_tc tc;
} map_statem_obj_t;

typedef struct map_statem {
	uint type;			/* object type */
	uint id;			/* object id */
	uint flags;			/* object flags */
	uint state;			/* object state */
	/* followed by object-specific state structure */
	union map_statem_obj statem[0];
} map_statem_t;

#define MAP_IOCGSTATEM	_IOWR(MAP_IOC_MAGIC,	 8, map_statem_t)	/* get */
#define MAP_IOCCMRESET	_IOWR(MAP_IOC_MAGIC,	 9, map_statem_t)	/* master reset */

/*
 * STATUS
 *
 * Provides status on the object.  Many of these are X.721 status fields or
 * object-specific status information.  Only read-write status fields may be
 * altered with a set or clear command.
 */
typedef struct map_status_df {
} map_status_df_t;
typedef struct map_status_cm {
} map_status_cm_t;
typedef struct map_status_iv {
} map_status_iv_t;
typedef struct map_status_op {
} map_status_op_t;
typedef struct map_status_dg {
} map_status_dg_t;
typedef struct map_status_ac {
} map_status_ac_t;
typedef struct map_status_tr {
} map_status_tr_t;
typedef struct map_status_me {
} map_status_me_t;
typedef struct map_status_sp {
} map_status_sp_t;
typedef struct map_status_tc {
} map_status_tc_t;

typedef union map_status_obj {
	struct map_status_df df;
	struct map_status_cm cm;
	struct map_status_iv iv;
	struct map_status_op op;
	struct map_status_dg dg;
	struct map_status_ac ac;
	struct map_status_tr tr;
	struct map_status_me me;
	struct map_status_sp sp;
	struct map_status_tc tc;
} map_status_obj_t;

typedef struct map_status {
	uint type;			/* object type */
	uint id;			/* object identifier */
	/* followed by object-specific structure */
	union map_status_obj status[0];
} map_status_t;

#define MAP_IOCGSTATUS	_IOWR(MAP_IOC_MAGIC,	10, map_status_t)	/* get */
#define MAP_IOCSSTATUS	_IOWR(MAP_IOC_MAGIC,	11, map_status_t)	/* set */
#define MAP_IOCCSTATUS	_IOWR(MAP_IOC_MAGIC,	12, map_status_t)	/* clear */

/*
 * STATISTICS
 */
typedef struct map_stats_df {
} map_stats_df_t;
typedef struct map_stats_cm {
} map_stats_cm_t;
typedef struct map_stats_iv {
} map_stats_iv_t;
typedef struct map_stats_op {
} map_stats_op_t;
typedef struct map_stats_dg {
} map_stats_dg_t;
typedef struct map_stats_ac {
} map_stats_ac_t;
typedef struct map_stats_tr {
} map_stats_tr_t;
typedef struct map_stats_me {
} map_stats_me_t;
typedef struct map_stats_sp {
} map_stats_sp_t;
typedef struct map_stats_tc {
} map_stats_tc_t;

typedef union map_stats_obj {
	struct map_stats_df df;
	struct map_stats_cm cm;
	struct map_stats_iv iv;
	struct map_stats_op op;
	struct map_stats_dg dg;
	struct map_stats_ac ac;
	struct map_stats_tr tr;
	struct map_stats_me me;
	struct map_stats_sp sp;
	struct map_stats_tc tc;
} map_stats_obj_t;

typedef struct map_stats {
	uint type;			/* object type */
	uint id;			/* object id */
	uint header;			/* object stats header */
	/* followed by object-specific statistics structure */
	union map_stats_obj stats[0];
} map_stats_t;

#define MAP_IOCGSTATSP	_IOWR(MAP_IOC_MAGIC,	 8,  map_stats_t)	/* get period */
#define MAP_IOCSSTATSP	_IOWR(MAP_IOC_MAGIC,	 9,  map_stats_t)	/* set period */
#define MAP_IOCGSTATS	_IOWR(MAP_IOC_MAGIC,	10,  map_stats_t)	/* get stats */
#define MAP_IOCCSTATS	_IOWR(MAP_IOC_MAGIC,	11,  map_stats_t)	/* get and clear stats */

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
typedef struct map_notify_df {
	uint events;
} map_notify_df_t;
typedef struct map_notify_cm {
	uint events;
} map_notify_cm_t;
typedef struct map_notify_iv {
	uint events;
} map_notify_iv_t;
typedef struct map_notify_op {
	uint events;
} map_notify_op_t;
typedef struct map_notify_dg {
	uint events;
} map_notify_dg_t;
typedef struct map_notify_ac {
	uint events;
} map_notify_ac_t;
typedef struct map_notify_tr {
	uint events;
} map_notify_tr_t;
typedef struct map_notify_me {
	uint events;
} map_notify_me_t;
typedef struct map_notify_sp {
	uint events;
} map_notify_sp_t;
typedef struct map_notify_tc {
	uint events;
} map_notify_tc_t;

typedef union map_notify_obj {
	struct map_notify_df df;
	struct map_notify_cm cm;
	struct map_notify_iv iv;
	struct map_notify_op op;
	struct map_notify_dg dg;
	struct map_notify_ac ac;
	struct map_notify_tr tr;
	struct map_notify_me me;
	struct map_notify_sp sp;
	struct map_notify_tc tc;
} map_notify_obj_t;

typedef struct map_notify {
	uint type;			/* object type */
	uint id;			/* object id */
	/* followed by object-specific notification structure */
	union map_notify_obj notify[0];
} map_notify_t;

#define MAP_IOCGNOTIFY	_IOWR(MAP_IOC_MAGIC, 17,  map_notify_t)	/* get */
#define MAP_IOCSNOTIFY	_IOWR(MAP_IOC_MAGIC, 18,  map_notify_t)	/* set */
#define MAP_IOCCNOTIFY	_IOWR(MAP_IOC_MAGIC, 19,  map_notify_t)	/* clear */

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
typedef struct map_attr_df {
	struct map_conf_df config;
	struct map_opt_conf_df option;
	struct map_info_df inform;
	struct map_statem_df statem;
	struct map_status_df status;
	struct map_stats_df stats;
	struct map_notify_df events;
} map_attr_df_t;
typedef struct map_attr_cm {
	struct map_conf_cm config;
	struct map_opt_conf_cm option;
	struct map_info_cm inform;
	struct map_statem_cm statem;
	struct map_status_cm status;
	struct map_stats_cm stats;
	struct map_notify_cm events;
} map_attr_cm_t;
typedef struct map_attr_iv {
	struct map_conf_iv config;
	struct map_opt_conf_iv option;
	struct map_info_iv inform;
	struct map_statem_iv statem;
	struct map_status_iv status;
	struct map_stats_iv stats;
	struct map_notify_iv events;
} map_attr_iv_t;
typedef struct map_attr_op {
	struct map_conf_op config;
	struct map_opt_conf_op option;
	struct map_info_op inform;
	struct map_statem_op statem;
	struct map_status_op status;
	struct map_stats_op stats;
	struct map_notify_op events;
} map_attr_op_t;
typedef struct map_attr_dg {
	struct map_conf_dg config;
	struct map_opt_conf_dg option;
	struct map_info_dg inform;
	struct map_statem_dg statem;
	struct map_status_dg status;
	struct map_stats_dg stats;
	struct map_notify_dg events;
} map_attr_dg_t;
typedef struct map_attr_ac {
	struct map_conf_ac config;
	struct map_opt_conf_ac option;
	struct map_info_ac inform;
	struct map_statem_ac statem;
	struct map_status_ac status;
	struct map_stats_ac stats;
	struct map_notify_ac events;
} map_attr_ac_t;
typedef struct map_attr_tr {
	struct map_conf_tr config;
	struct map_opt_conf_tr option;
	struct map_info_tr inform;
	struct map_statem_tr statem;
	struct map_status_tr status;
	struct map_stats_tr stats;
	struct map_notify_tr events;
} map_attr_tr_t;
typedef struct map_attr_me {
	struct map_conf_me config;
	struct map_opt_conf_me option;
	struct map_info_me inform;
	struct map_statem_me statem;
	struct map_status_me status;
	struct map_stats_me stats;
	struct map_notify_me events;
} map_attr_me_t;
typedef struct map_attr_sp {
	struct map_conf_sp config;
	struct map_opt_conf_sp option;
	struct map_info_sp inform;
	struct map_statem_sp statem;
	struct map_status_sp status;
	struct map_stats_sp stats;
	struct map_notify_sp events;
} map_attr_sp_t;
typedef struct map_attr_tc {
	struct map_conf_tc config;
	struct map_opt_conf_tc option;
	struct map_info_tc inform;
	struct map_statem_tc statem;
	struct map_status_tc status;
	struct map_stats_tc stats;
	struct map_notify_tc events;
} map_attr_tc_t;

typedef union map_attr_obj {
	struct map_attr_df df;
	struct map_attr_cm cm;
	struct map_attr_iv iv;
	struct map_attr_op op;
	struct map_attr_dg dg;
	struct map_attr_ac ac;
	struct map_attr_tr tr;
	struct map_attr_me me;
	struct map_attr_sp sp;
	struct map_attr_tc tc;
} map_attr_obj_t;

typedef struct map_attr {
	uint type;			/* object type */
	uint id;			/* object identifier */
	uint cmd;			/* command */
	/* followed by object-specific structure */
	union map_attr_obj attrs[0];
} map_attr_t;

#define	MAP_IOCGATTR	_IOWR(MAP_IOC_MAGIC,	20, map_attr_t)	/* get attributes */

/*
 *  MANAGEMENT
 */
typedef struct map_action_df {
} map_action_df_t;
typedef struct map_action_cm {
} map_action_cm_t;
typedef struct map_action_iv {
} map_action_iv_t;
typedef struct map_action_op {
} map_action_op_t;
typedef struct map_action_dg {
} map_action_dg_t;
typedef struct map_action_ac {
} map_action_ac_t;
typedef struct map_action_tr {
} map_action_tr_t;
typedef struct map_action_me {
} map_action_me_t;
typedef struct map_action_sp {
} map_action_sp_t;
typedef struct map_action_tc {
} map_action_tc_t;

typedef union map_action_obj {
	struct map_action_df df;
	struct map_action_cm cm;
	struct map_action_iv iv;
	struct map_action_op op;
	struct map_action_dg dg;
	struct map_action_ac ac;
	struct map_action_tr tr;
	struct map_action_me me;
	struct map_action_sp sp;
	struct map_action_tc tc;
} map_action_obj_t;

typedef struct map_mgmt {
	uint type;			/* object type */
	uint id;			/* object id */
	uint cmd;			/* command */
	/* followed by object-specific structure */
	union map_action_obj action[0];
} map_mgmt_t;

#define MAP_MGMT_BLOCK			1
#define MAP_MGMT_UNBLOCK		2
#define MAP_MGMT_RESET			3
#define MAP_MGMT_QUERY			4

#define MAP_IOCCMGMT	_IOWR(MAP_IOC_MAGIC,	21, map_mgmt_t)	/* command */

/*
 * PASS LOWER
 *
 * This structure is deprecated and as largely used for testing as a mechanism
 * to bypass an input-output control to a stream linked under a multiplexing
 * driver supporting this interface.
 */
typedef struct map_pass {
	uint muxid;			/* mux index of lower stream to pass message to */
	uint8_t type;			/* type of message block */
	uint8_t band;			/* band of message block */
	uint ctl_length;		/* length of cntl part */
	uint dat_length;		/* length of data part */
	/* followed by cntl and data part of message to pass */
} map_pass_t;

#define MAP_IOCCPASS	_IOWR(MAP_IOC_MAGIC,	22, map_pass_t)	/* pass */

union map_ioctls {
	struct {
		struct map_info info;
		union map_info_obj obj;
	} info;
	struct {
		struct map_option option;
		union map_option_obj obj;
	} opt_conf;
	struct {
		struct map_config config;
		union map_conf_obj obj;
	} conf;
	struct {
		struct map_statem statem;
		union map_statem_obj obj;
	} statem;
	struct {
		struct map_status status;
		union map_status_obj obj;
	} status;
	struct {
		struct map_stats stats;
		union map_stats_obj obj;
	} stats;
	struct {
		struct map_notify notify;
		union map_notify_obj obj;
	} notify;
	struct {
		struct map_attr attr;
		union map_attr_obj obj;
	} attr;
	struct {
		struct map_mgmt mgmt;
		union map_action_obj obj;
	} mgmt;
	struct map_pass pass;
};

#define MAP_IOC_FIRST		 0
#define MAP_IOC_LAST		22
#define MAP_IOC_PRIVATE		32

#endif				/* __MAP_IOCTL_H__ */
