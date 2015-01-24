/*****************************************************************************

 @(#) src/include/ss7/sccp_ioctl.h

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

#ifndef __SCCP_IOCTL_H__
#define __SCCP_IOCTL_H__

/* This file can be processed by doxygen(1). */

#include <linux/ioctl.h>

#define SCCP_IOC_MAGIC 's'

#define SCCP_OBJ_TYPE_DF	    0	/* Default */
#define SCCP_OBJ_TYPE_SC	    1	/* SCCP User */
#define SCCP_OBJ_TYPE_NA	    2	/* Network Appearance */
#define SCCP_OBJ_TYPE_CP	    3	/* Coupling */
#define SCCP_OBJ_TYPE_SS	    4	/* Local Subsystem (sccpLinkageTable) */
#define SCCP_OBJ_TYPE_RS	    5	/* Remote Subsystem (sccpLinkageTable) */
#define SCCP_OBJ_TYPE_SR	    6	/* Signalling Relation (sccpMtpTable) */
#define SCCP_OBJ_TYPE_CA	    7	/* Concerned Area (sccpConcernedAreaTable) */
#define SCCP_OBJ_TYPE_ES	    7	/* Entity Set (sccpEntitySetTable) */
#define SCCP_OBJ_TYPE_SP	    8	/* Signalling Point (mtpMsTable) */
#define SCCP_OBJ_TYPE_MT	    9	/* Message Transfer Part (sccpLinkageTable) */

/*
 * INFORMATION
 *
 * Provides read-only information that is not available for the manager to
 * configure.  Also, this information does not change while the object exists:
 * it is purely static for the lifetime of the object.
 */
typedef struct sccp_info_df {
} sccp_info_df_t;
typedef struct sccp_info_sc {
} sccp_info_sc_t;
typedef struct sccp_info_na {
} sccp_info_na_t;
typedef struct sccp_info_cp {
} sccp_info_cp_t;
typedef struct sccp_info_ss {
} sccp_info_ss_t;
typedef struct sccp_info_rs {
} sccp_info_rs_t;
typedef struct sccp_info_sr {
} sccp_info_sr_t;
typedef struct sccp_info_ca {
} sccp_info_ca_t;
typedef struct sccp_info_es {
} sccp_info_es_t;
typedef struct sccp_info_sp {
} sccp_info_sp_t;
typedef struct sccp_info_mt {
} sccp_info_mt_t;

typedef union sccp_info_obj {
	struct sccp_info_df df;
	struct sccp_info_sc sc;
	struct sccp_info_na na;
	struct sccp_info_cp cp;
	struct sccp_info_ss ss;
	struct sccp_info_rs rs;
	struct sccp_info_sr sr;
	struct sccp_info_ca ca;
	struct sccp_info_es es;
	struct sccp_info_sp sp;
	struct sccp_info_mt mt;
} sccp_info_obj_t;

typedef struct sccp_info {
	uint type;			/* object type */
	uint id;			/* object identifier */
	uint cmd;			/* command */
	/* followed by object-specific structure */
	union sccp_info_obj info[0];
} sccp_info_t;

#define SCCP_IOCGINFO	_IOWR(SCCP_IOC_MAGIC,	 0, sccp_info_t)	/* get */

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
typedef struct sccp_opt_conf_df {
	lmi_option_t proto;		/* protocol variant and options */
} sccp_opt_conf_df_t;
typedef struct sccp_opt_conf_sc {
	uint tcon;
	uint tias;
	uint tiar;
	uint trel;
	uint trel2;
	uint tint;
	uint tguard;
	uint tres;
	uint trea;
	uint tack;
} sccp_opt_conf_sc_t;
typedef struct sccp_opt_conf_na {
} sccp_opt_conf_na_t;
typedef struct sccp_opt_conf_cp {
} sccp_opt_conf_cp_t;
typedef struct sccp_opt_conf_ss {
	uint sccpNetworkEntitySystemTypes;
	uint sccpNetworkEntityVersion;
	uint sccpNetworkEntityLUDTandLUDTSSupported;
	uint sccpNetworkEntityCoordChangeTimer;
	uint sccpNetworkEntityIgnoreSSTTimer;
	uint sccpNetworkEntityMaxStatInfoTimer;
	uint tisst;
	uint twsog;
} sccp_opt_conf_ss_t;
typedef struct sccp_opt_conf_rs {
	sccpMtpUserPart;
	uint tsst;			/* T(sst) timer */
} sccp_opt_conf_rs_t;
typedef struct sccp_opt_conf_sr {
	uint sccpLinkageOperationalProtocols;
	uint sccpLinkageAttackTimerValue;
	uint sccpLinkageDecayTimerValue;
	uint sccpLinkageNrOfRestrictionLevels;
	uint sccpLinkageNrOfSubLevels;
	uint sccpLinkageCLS;
	uint sccpLinkageCongestionTimerValue;
	uint sccpLinkageP;
	uint8_t sccpLinkageImportanceLevelCR[2];
	uint8_t sccpLinkageImportanceLevelCC[2];
	uint8_t sccpLinkageImportanceLevelCREF[2];
	uint8_t sccpLinkageImportanceLevelDT1[2];
	uint8_t sccpLinkageImportanceLevelDT2[2];
	uint8_t sccpLinkageImportanceLevelAK[2];
	uint8_t sccpLinkageImportanceLevelIT[2];
	uint8_t sccpLinkageImportanceLevelED[2];
	uint8_t sccpLinkageImportanceLevelEA[2];
	uint8_t sccpLinkageImportanceLevelRSR[2];
	uint8_t sccpLinkageImportanceLevelRSC[2];
	uint8_t sccpLinkageImportanceLevelERR[2];
	uint8_t sccpLinkageImportanceLevelRLC[2];
	uint8_t sccpLinkageImportanceLevelRLSD[2];
	uint8_t sccpLinkageImportanceLevelUDT[2];
	uint8_t sccpLinkageImportanceLevelUDTS[2];
	uint8_t sccpLinkageImportanceLevelXUDT[2];
	uint8_t sccpLinkageImportanceLevelXUDTS[2];
	uint8_t sccpLinkageImportanceLevelLUDT[2];
	uint8_t sccpLinkageImportanceLevelLUDTS[2];
	uint sccpLinkageLowerLimitForSegmentation;
	uint sccpLinkageUpperLimitForSegmentation;
	uint tattack;			/* T(attack) timer */
	uint tdecay;			/* T(decay) timer */
	uint tstatinfo;			/* T(stat_info) timer */
	uint tsst;			/* T(sst) timer */
} sccp_opt_conf_sr_t;
typedef struct sccp_opt_conf_ca {
} sccp_opt_conf_ca_t;
typedef struct sccp_opt_conf_es {
	uint sccpEntitySetSharingMode;
	uint sccpEntitySetLoadSharingAlg;
	uint sccpEntitySetType;
} sccp_opt_conf_es_t;
typedef struct sccp_opt_conf_sp {
	uint tgtt;
} sccp_opt_conf_sp_t;
typedef struct sccp_opt_conf_mt {
} sccp_opt_conf_mt_t;

typedef union sccp_option_obj {
	struct sccp_opt_conf_df df;
	struct sccp_opt_conf_sc sc;
	struct sccp_opt_conf_na na;
	struct sccp_opt_conf_cp cp;
	struct sccp_opt_conf_ss ss;
	struct sccp_opt_conf_rs rs;
	struct sccp_opt_conf_sr sr;
	struct sccp_opt_conf_ca ca;
	struct sccp_opt_conf_es es;
	struct sccp_opt_conf_sp sp;
	struct sccp_opt_conf_mt mt;
} sccp_option_obj_t;

typedef struct sccp_option {
	uint type;			/* object type */
	uint id;			/* object id */
	/* followed by specific proto structure */
	union sccp_option_obj options[0];
} sccp_option_t;

#define SCCP_IOCGOPTION	_IOWR(SCCP_IOC_MAGIC,	 1, sccp_option_t)	/* get */
#define SCCP_IOCSOPTION	_IOWR(SCCP_IOC_MAGIC,	 2, sccp_option_t)	/* set */

/*
 * CONFIGURATION
 *
 * Configures the object (creates, destroys, moves).  These actions require
 * reconfiguration of the object and a change in its relationship to other
 * objects.  Also, this structure contains information that must be specified
 * during creation of the object.  Each structure can optionally be followed by
 * an options structure to specify the options during creation.
 */
typedef struct sccp_conf_df {
} sccp_conf_df_t;
typedef struct sccp_conf_sc {
} sccp_conf_sc_t;
typedef struct sccp_conf_na {
	lmi_option_t proto;		/* protocol variant and options */
} sccp_conf_na_t;
typedef struct sccp_conf_cp {
} sccp_conf_cp_t;
typedef struct sccp_conf_ss {
	uint mtpMsId;			/* local signalling point identifier */
	uint sccpNetworkEntityLocalSapNames;
	uint sccpAccessPointSap2Address;
	uint sccpAccessPointConcernedAreaPointer;	/* Create */
	uint spid;			/* local signalling point identifier */
	uint ssn;			/* subsystem number */
} sccp_conf_ss_t;
typedef struct sccp_conf_rs {
	uint sccpLinkageId; /* remote signalling point linkage */
	uint sccpAccessPointSap2Address;
	uint srid;			/* remote signalling point identifier */
	uint ssn;			/* subsystem number */
} sccp_conf_rs_t;
typedef struct sccp_conf_sr {
	uint spid;			/* local signalling point identifier */
	struct mtp_addr add;		/* remote signalling point code */
	lmi_option_t proto;		/* protocol variant and options */
} sccp_conf_sr_t;
typedef struct sccp_conf_ca {
	uint mtpMsId;			/* local signalling point identifier */
	uint sccpNetworkEntityId;	/* local subsystem identifier */
	uint sccpRemoteSCCPId;		/* signalling relation identifier */
	uint sccpRemoteSCCPMTPAccessPoint;	/* MTP identifier */
} sccp_conf_ca_t;
typedef struct sccp_conf_es {
	uint sccpEntitySetSsn;
} sccp_conf_es_t;
typedef struct sccp_conf_sp {
	uint naid;			/* network appearance id */
	struct mtp_addr add;		/* local signalling point code */
	lmi_option_t proto;		/* protocol variant and options */
} sccp_conf_sp_t;
typedef struct sccp_conf_mt {
	int muxid;			/* lower multipleding driver id */
	uint spid;			/* local signalling point identifier */
	uint srid;			/* remote signalling point identifier */
	lmi_option_t proto;		/* protocol variant and options */
} sccp_conf_mt_t;

typedef union sccp_conf_obj {
	struct sccp_conf_df df;
	struct sccp_conf_sc sc;
	struct sccp_conf_na na;
	struct sccp_conf_cp cp;
	struct sccp_conf_ss ss;
	struct sccp_conf_rs rs;
	struct sccp_conf_sr sr;
	struct sccp_conf_ca ca;
	struct sccp_conf_es es;
	struct sccp_conf_sp sp;
	struct sccp_conf_mt mt;
} sccp_conf_obj_t;

#define SCCP_GET	0	/* get options or configuration */
#define SCCP_ADD	1	/* add configuration */
#define SCCP_CHA	2	/* set options or change configuration */
#define SCCP_DEL	3	/* delete configuraiton */
#define SCCP_TST	4	/* test options or configuration */
#define SCCP_COM	5	/* commit options or configuration */

typedef struct sccp_config {
	uint type;			/* object type */
	uint id;			/* object id */
	uint cmd;			/* object command */
	/* followed by specific configuration structure */
	union sccp_conf_obj config[0];
} sccp_config_t;

#define SCCP_IOCLCONFIG	_IOWR(SCCP_IOC_MAGIC,	 3, sccp_config_t)	/* list */
#define SCCP_IOCGCONFIG	_IOWR(SCCP_IOC_MAGIC,	 4, sccp_config_t)	/* get */
#define SCCP_IOCSCONFIG	_IOWR(SCCP_IOC_MAGIC,	 5, sccp_config_t)	/* set */
#define SCCP_IOCTCONFIG	_IOWR(SCCP_IOC_MAGIC,	 6, sccp_config_t)	/* test */
#define SCCP_IOCCCONFIG	_IOWR(SCCP_IOC_MAGIC,	 7, sccp_config_t)	/* commit */

/*
 * STATE
 *
 * Provides state machine state information.  The purpose of these items is
 * primarily for debugging: to view the internal state of the state machine.
 * Some of these state variables might also be reflected in status fields.
 */
typedef struct sccp_timers_df {
} sccp_timers_df_t;
typedef struct sccp_timers_sc {
	uint tcon;
	uint tias;
	uint tiar;
	uint trel;
	uint trel2;
	uint tint;
	uint tguard;
	uint tres;
	uint trea;
	uint tack;
} sccp_timers_sc_t;
typedef struct sccp_timers_na {
} sccp_timers_na_t;
typedef struct sccp_timers_cp {
} sccp_timers_cp_t;
typedef struct sccp_timers_ss {
	uint sccpNetworkEntityCoordChangeTimer;
	uint sccpNetworkEntityIgnoreSSTTimer;
	uint sccpNetworkEntityMaxStatInfoTimer;
	uint tisst;
	uint twsog;
} sccp_timers_ss_t;
typedef struct sccp_timers_rs {
	uint tsst;			/* T(sst) timer */
} sccp_timers_rs_t;
typedef struct sccp_timers_sr {
	uint sccpLinkageAttachTimerValue;
	uint sccpLinkageDecayTimerValue;
	uint sccpLinkageCongestionTimerValue;
	uint tattack;			/* T(attack) timer */
	uint tdecay;			/* T(decay) timer */
	uint tstatinfo;			/* T(stat_info) timer */
	uint tsst;			/* T(sst) timer */
} sccp_timers_sr_t;
typedef struct sccp_timers_ca {
} sccp_timers_ca_t;
typedef struct sccp_timers_es {
} sccp_timers_es_t;
typedef struct sccp_timers_sp {
	uint sccpSclcInitialValueReassTimer;
	uint tgtt;
} sccp_timers_sp_t;
typedef struct sccp_timers_mt {
} sccp_timers_mt_t;

typedef struct sccp_statem_df {
	struct sccp_timers_df timers;
} sccp_statem_df_t;
typedef struct sccp_statem_sc {
	struct sccp_timers_sc timers;
} sccp_statem_sc_t;
typedef struct sccp_statem_na {
	struct sccp_timers_na timers;
} sccp_statem_na_t;
typedef struct sccp_statem_cp {
	struct sccp_timers_cp timers;
} sccp_statem_cp_t;
typedef struct sccp_statem_ss {
	struct sccp_timers_ss timers;
} sccp_statem_ss_t;
typedef struct sccp_statem_rs {
	struct sccp_timers_rs timers;
} sccp_statem_rs_t;
typedef struct sccp_statem_sr {
	struct sccp_timers_sr timers;
} sccp_statem_sr_t;
typedef struct sccp_statem_ca {
	struct sccp_timers_ca timers;
} sccp_statem_ca_t;
typedef struct sccp_statem_es {
	struct sccp_timers_es timers;
} sccp_statem_es_t;
typedef struct sccp_statem_sp {
	struct sccp_timers_sp timers;
} sccp_statem_sp_t;
typedef struct sccp_statem_mt {
	struct sccp_timers_mt timers;
} sccp_statem_mt_t;

typedef union sccp_statem_obj {
	struct sccp_statem_df df;
	struct sccp_statem_sc sc;
	struct sccp_statem_na na;
	struct sccp_statem_cp cp;
	struct sccp_statem_ss ss;
	struct sccp_statem_rs rs;
	struct sccp_statem_sr sr;
	struct sccp_statem_ca ca;
	struct sccp_statem_es es;
	struct sccp_statem_sp sp;
	struct sccp_statem_mt mt;
} sccp_statem_obj_t;

typedef struct sccp_statem {
	uint type;			/* object type */
	uint id;			/* object id */
	uint flags;			/* object flags */
	uint state;			/* object state */
	/* followed by object-specific state structure */
	union sccp_statem_obj statem[0];
} sccp_statem_t;

#define SCCP_IOCGSTATEM	_IOWR(SCCP_IOC_MAGIC,	 8, sccp_statem_t)	/* get */
#define SCCP_IOCCMRESET	_IOWR(SCCP_IOC_MAGIC,	 9, sccp_statem_t)	/* master reset */

/*
 * STATUS
 *
 * Provides status on the object.  Many of these are X.721 status fields or
 * object-specific status information.  Only read-write status fields may be
 * altered with a set or clear command.
 */
typedef struct sccp_status_df {
} sccp_status_df_t;
typedef struct sccp_status_sc {
	sccpAccessPointAlarmStatus;
	sccpAccessPointAvailabilityStatus;
} sccp_status_sc_t;
typedef struct sccp_status_na {
} sccp_status_na_t;
typedef struct sccp_status_cp {
} sccp_status_cp_t;
typedef struct sccp_status_ss {
	uint sccpNetworkEntityAlarmStatus;
	uint sccpNetworkEntityOperationalState;
} sccp_status_ss_t;
typedef struct sccp_status_rs {
	uint sccpMtpUserPartStatus;
	uint sccpMtpUsageState;
	uint sccpMtpAdministrativeState;
	uint sccpMtpOperationalState;
	uint sccpMtpCongestedState;
	uint sccpMtpCongestionlevel;
} sccp_status_rs_t;
typedef struct sccp_status_sr {
	uint sccpLinkageRLM;
	uint sccpLinkageRSLM;
} sccp_status_sr_t;
typedef struct sccp_status_ca {
} sccp_status_ca_t;
typedef struct sccp_status_es {
} sccp_status_es_t;
typedef struct sccp_status_sp {
	uint sccpSclcAlarmStatus;
	uint sccpSclcOperationalState;
	uint sccpSclcAdministrativeState;
	uint sccpScocOperationalState;
	uint sccpScocAdministrativeState;
	uint sccpScrcAlarmStatus;
} sccp_status_sp_t;
typedef struct sccp_status_mt {
} sccp_status_mt_t;

typedef union sccp_status_obj {
	struct sccp_status_df df;
	struct sccp_status_sc sc;
	struct sccp_status_na na;
	struct sccp_status_cp cp;
	struct sccp_status_ss ss;
	struct sccp_status_rs rs;
	struct sccp_status_sr sr;
	struct sccp_status_ca ca;
	struct sccp_status_es es;
	struct sccp_status_sp sp;
	struct sccp_status_mt mt;
} sccp_status_obj_t;

typedef struct sccp_status {
	uint type;			/* object type */
	uint id;			/* object identifier */
	/* followed by object-specific structure */
	union sccp_status_obj status[0];
} sccp_status_t;

#define SCCP_IOCGSTATUS	_IOWR(SCCP_IOC_MAGIC,	10, sccp_status_t)	/* get */
#define SCCP_IOCSSTATUS	_IOWR(SCCP_IOC_MAGIC,	11, sccp_status_t)	/* set */
#define SCCP_IOCCSTATUS	_IOWR(SCCP_IOC_MAGIC,	12, sccp_status_t)	/* clear */

/*
 * STATISTICS
 */
typedef struct sccp_stats_df {
} sccp_stats_df_t;
typedef struct sccp_stats_sc {
} sccp_stats_sc_t;
typedef struct sccp_stats_na {
} sccp_stats_na_t;
typedef struct sccp_stats_cp {
} sccp_stats_cp_t;
typedef struct sccp_stats_ss {
} sccp_stats_ss_t;
typedef struct sccp_stats_rs {
} sccp_stats_rs_t;
typedef struct sccp_stats_sr {
} sccp_stats_sr_t;
typedef struct sccp_stats_ca {
} sccp_stats_ca_t;
typedef struct sccp_stats_es {
} sccp_stats_es_t;
typedef struct sccp_stats_sp {
} sccp_stats_sp_t;
typedef struct sccp_stats_mt {
} sccp_stats_mt_t;

typedef union sccp_stats_obj {
	struct sccp_stats_df df;
	struct sccp_stats_sc sc;
	struct sccp_stats_na na;
	struct sccp_stats_cp cp;
	struct sccp_stats_ss ss;
	struct sccp_stats_rs rs;
	struct sccp_stats_sr sr;
	struct sccp_stats_ca ca;
	struct sccp_stats_es es;
	struct sccp_stats_sp sp;
	struct sccp_stats_mt mt;
} sccp_stats_obj_t;

typedef struct sccp_stats {
	uint type;			/* object type */
	uint id;			/* object id */
	uint header;			/* object stats header */
	/* followed by object-specific statistics structure */
	union sccp_stats_obj stats[0];
} sccp_stats_t;

#define SCCP_IOCGSTATSP	_IOWR(SCCP_IOC_MAGIC,	 8,  sccp_stats_t)	/* get period */
#define SCCP_IOCSSTATSP	_IOWR(SCCP_IOC_MAGIC,	 9,  sccp_stats_t)	/* set period */
#define SCCP_IOCGSTATS	_IOWR(SCCP_IOC_MAGIC,	10,  sccp_stats_t)	/* get stats */
#define SCCP_IOCCSTATS	_IOWR(SCCP_IOC_MAGIC,	11,  sccp_stats_t)	/* get and clear stats */

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
typedef struct sccp_notify_df {
	uint events;
} sccp_notify_df_t;
typedef struct sccp_notify_sc {
	uint events;
} sccp_notify_sc_t;
typedef struct sccp_notify_na {
	uint events;
} sccp_notify_na_t;
typedef struct sccp_notify_cp {
	uint events;
} sccp_notify_cp_t;
typedef struct sccp_notify_ss {
	uint events;
} sccp_notify_ss_t;
typedef struct sccp_notify_rs {
	uint events;
} sccp_notify_rs_t;
typedef struct sccp_notify_sr {
	uint events;
} sccp_notify_sr_t;
typedef struct sccp_notify_ca {
	uint events;
} sccp_notify_ca_t;
typedef struct sccp_notify_es {
	uint events;
} sccp_notify_es_t;
typedef struct sccp_notify_sp {
	uint events;
} sccp_notify_sp_t;
typedef struct sccp_notify_mt {
	uint events;
} sccp_notify_mt_t;

typedef union sccp_notify_obj {
	struct sccp_notify_df df;
	struct sccp_notify_sc sc;
	struct sccp_notify_na na;
	struct sccp_notify_cp cp;
	struct sccp_notify_ss ss;
	struct sccp_notify_rs rs;
	struct sccp_notify_sr sr;
	struct sccp_notify_ca ca;
	struct sccp_notify_es es;
	struct sccp_notify_sp sp;
	struct sccp_notify_mt mt;
} sccp_notify_obj_t;

typedef struct sccp_notify {
	uint type;			/* object type */
	uint id;			/* object id */
	/* followed by object-specific notification structure */
	union sccp_notify_obj notify[0];
} sccp_notify_t;

#define SCCP_IOCGNOTIFY	_IOWR(SCCP_IOC_MAGIC, 17,  sccp_notify_t)	/* get */
#define SCCP_IOCSNOTIFY	_IOWR(SCCP_IOC_MAGIC, 18,  sccp_notify_t)	/* set */
#define SCCP_IOCCNOTIFY	_IOWR(SCCP_IOC_MAGIC, 19,  sccp_notify_t)	/* clear */

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
typedef struct sccp_attr_df {
	struct sccp_conf_df config;
	struct sccp_opt_conf_df option;
	struct sccp_info_df inform;
	struct sccp_statem_df statem;
	struct sccp_status_df status;
	struct sccp_stats_df stats;
	struct sccp_notify_df events;
} sccp_attr_df_t;
typedef struct sccp_attr_sc {
	struct sccp_conf_sc config;
	struct sccp_opt_conf_sc option;
	struct sccp_info_sc inform;
	struct sccp_statem_sc statem;
	struct sccp_status_sc status;
	struct sccp_stats_sc stats;
	struct sccp_notify_sc events;
} sccp_attr_sc_t;
typedef struct sccp_attr_na {
	struct sccp_conf_na config;
	struct sccp_opt_conf_na option;
	struct sccp_info_na inform;
	struct sccp_statem_na statem;
	struct sccp_status_na status;
	struct sccp_stats_na stats;
	struct sccp_notify_na events;
} sccp_attr_na_t;
typedef struct sccp_attr_cp {
	struct sccp_conf_cp config;
	struct sccp_opt_conf_cp option;
	struct sccp_info_cp inform;
	struct sccp_statem_cp statem;
	struct sccp_status_cp status;
	struct sccp_stats_cp stats;
	struct sccp_notify_cp events;
} sccp_attr_cp_t;
typedef struct sccp_attr_ss {
	struct sccp_conf_ss config;
	struct sccp_opt_conf_ss option;
	struct sccp_info_ss inform;
	struct sccp_statem_ss statem;
	struct sccp_status_ss status;
	struct sccp_stats_ss stats;
	struct sccp_notify_ss events;
} sccp_attr_ss_t;
typedef struct sccp_attr_rs {
	struct sccp_conf_rs config;
	struct sccp_opt_conf_rs option;
	struct sccp_info_rs inform;
	struct sccp_statem_rs statem;
	struct sccp_status_rs status;
	struct sccp_stats_rs stats;
	struct sccp_notify_rs events;
} sccp_attr_rs_t;
typedef struct sccp_attr_sr {
	struct sccp_conf_sr config;
	struct sccp_opt_conf_sr option;
	struct sccp_info_sr inform;
	struct sccp_statem_sr statem;
	struct sccp_status_sr status;
	struct sccp_stats_sr stats;
	struct sccp_notify_sr events;
} sccp_attr_sr_t;
typedef struct sccp_attr_ca {
	struct sccp_conf_ca config;
	struct sccp_opt_conf_ca option;
	struct sccp_info_ca inform;
	struct sccp_statem_ca statem;
	struct sccp_status_ca status;
	struct sccp_stats_ca stats;
	struct sccp_notify_ca events;
} sccp_attr_ca_t;
typedef struct sccp_attr_es {
	struct sccp_conf_es config;
	struct sccp_opt_conf_es option;
	struct sccp_info_es inform;
	struct sccp_statem_es statem;
	struct sccp_status_es status;
	struct sccp_stats_es stats;
	struct sccp_notify_es events;
} sccp_attr_es_t;
typedef struct sccp_attr_sp {
	struct sccp_conf_sp config;
	struct sccp_opt_conf_sp option;
	struct sccp_info_sp inform;
	struct sccp_statem_sp statem;
	struct sccp_status_sp status;
	struct sccp_stats_sp stats;
	struct sccp_notify_sp events;
} sccp_attr_sp_t;
typedef struct sccp_attr_mt {
	struct sccp_conf_mt config;
	struct sccp_opt_conf_mt option;
	struct sccp_info_mt inform;
	struct sccp_statem_mt statem;
	struct sccp_status_mt status;
	struct sccp_stats_mt stats;
	struct sccp_notify_mt events;
} sccp_attr_mt_t;

typedef union sccp_attr_obj {
	struct sccp_attr_df df;
	struct sccp_attr_sc sc;
	struct sccp_attr_na na;
	struct sccp_attr_cp cp;
	struct sccp_attr_ss ss;
	struct sccp_attr_rs rs;
	struct sccp_attr_sr sr;
	struct sccp_attr_ca ca;
	struct sccp_attr_es es;
	struct sccp_attr_sp sp;
	struct sccp_attr_mt mt;
} sccp_attr_obj_t;

typedef struct sccp_attr {
	uint type;			/* object type */
	uint id;			/* object identifier */
	uint cmd;			/* command */
	/* followed by object-specific structure */
	union sccp_attr_obj attrs[0];
} sccp_attr_t;

#define	SCCP_IOCGATTR	_IOWR(SCCP_IOC_MAGIC,	20, sccp_attr_t)	/* get attributes */

/*
 *  MANAGEMENT
 */
typedef struct sccp_action_df {
} sccp_action_df_t;
typedef struct sccp_action_sc {
} sccp_action_sc_t;
typedef struct sccp_action_na {
} sccp_action_na_t;
typedef struct sccp_action_cp {
} sccp_action_cp_t;
typedef struct sccp_action_ss {
} sccp_action_ss_t;
typedef struct sccp_action_rs {
} sccp_action_rs_t;
typedef struct sccp_action_sr {
} sccp_action_sr_t;
typedef struct sccp_action_ca {
} sccp_action_ca_t;
typedef struct sccp_action_es {
} sccp_action_es_t;
typedef struct sccp_action_sp {
} sccp_action_sp_t;
typedef struct sccp_action_mt {
} sccp_action_mt_t;

typedef union sccp_action_obj {
	struct sccp_action_df df;
	struct sccp_action_sc sc;
	struct sccp_action_na na;
	struct sccp_action_cp cp;
	struct sccp_action_ss ss;
	struct sccp_action_rs rs;
	struct sccp_action_sr sr;
	struct sccp_action_ca ca;
	struct sccp_action_es es;
	struct sccp_action_sp sp;
	struct sccp_action_mt mt;
} sccp_action_obj_t;

typedef struct sccp_mgmt {
	uint type;			/* object type */
	uint id;			/* object id */
	uint cmd;			/* command */
	/* followed by object-specific structure */
	union sccp_action_obj action[0];
} sccp_mgmt_t;

#define SCCP_MGMT_ALLOW			 0
#define SCCP_MGMT_RESTRICT		 1
#define SCCP_MGMT_PROHIBIT		 2
#define SCCP_MGMT_ACTIVATE		 3
#define SCCP_MGMT_DEACTIVATE		 4
#define SCCP_MGMT_BLOCK			 5
#define SCCP_MGMT_UNBLOCK		 6
#define SCCP_MGMT_INHIBIT		 7
#define SCCP_MGMT_UNINHIBIT		 8
#define SCCP_MGMT_CONGEST		 9
#define SCCP_MGMT_UNCONGEST		10
#define SCCP_MGMT_DANGER		11
#define SCCP_MGMT_NODANGER		12
#define SCCP_MGMT_RESTART		13
#define SCCP_MGMT_RESTARTED		14

#define SCCP_IOCCMGMT	_IOWR(SCCP_IOC_MAGIC,	21, sccp_mgmt_t)	/* command */

/*
 * PASS LOWER
 *
 * This structure is deprecated and as largely used for testing as a mechanism
 * to bypass an input-output control to a stream linked under a multiplexing
 * driver supporting this interface.
 */
typedef struct sccp_pass {
	uint muxid;			/* mux index of lower stream to pass message to */
	uint8_t type;			/* type of message block */
	uint8_t band;			/* band of message block */
	uint ctl_length;		/* length of cntl part */
	uint dat_length;		/* length of data part */
	/* followed by cntl and data part of message to pass */
} sccp_pass_t;

#define SCCP_IOCCPASS	_IOW(	SCCP_IOC_MAGIC,	22,	sccp_pass_t	)

union sccp_ioctls {
	struct {
		struct sccp_info info;
		union sccp_info_obj obj;
	} info;
	struct {
		struct sccp_option option;
		union sccp_option_obj obj;
	} opt_conf;
	struct {
		struct sccp_config config;
		union sccp_conf_obj obj;
	} conf;
	struct {
		struct sccp_statem statem;
		union sccp_statem_obj obj;
	} statem;
	struct {
		struct sccp_status status;
		union sccp_status_obj obj;
	} status;
	struct {
		struct sccp_stats stats;
		union sccp_stats_obj obj;
	} stats;
	struct {
		struct sccp_notify notify;
		union sccp_notify_obj obj;
	} notify;
	struct {
		struct sccp_attr attr;
		union sccp_attr_obj obj;
	} attr;
	struct {
		struct sccp_mgmt mgmt;
		union sccp_action_obj obj;
	} mgmt;
	struct sccp_pass pass;
};

#define SCCP_IOC_FIRST    0
#define SCCP_IOC_LAST    22
#define SCCP_IOC_PRIVATE 32

#endif				/* __SCCP_IOCTL_H__ */
