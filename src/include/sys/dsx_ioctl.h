/*****************************************************************************

 @(#) $Id$

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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ifndef __SYS_DSX_IOCTL_H__
#define __SYS_DSX_IOCTL_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2008-2009 Monavacon Limited."

/* This file can be processed by doxygen(1). */

#include <linux/ioctl.h>
#include <sys/smi_ioctl.h>

#define DSX_IOC_MAGIC	'D'

#define DSX_OBJ_TYPE_DFLT	0	/* driver defaults */
#define DSX_OBJ_TYPE_SPAN	1	/* span */
#define DSX_OBJ_TYPE_CHAN	2	/* channel */
#define DSX_OBJ_TYPE_FRAC	3	/* fractional */

/*
 * INFORMATION
 *
 * Provides read-only information that is not available for the manager to
 * configure.  Also, this information does not change while the object exists:
 * it is purely static for the lifetime of the object.
 */
struct dsx_info_dflt {
	/* no static driver information */
};
struct dsx_info_span {
	/* no static span information */
};
struct dsx_info_chan {
	/* no static channel information */
};
struct dsx_info_frac {
	/* no static fractional information */
};
union dsx_info_obj {
	struct dsx_info_dflt dflt;
	struct dsx_info_span span;
	struct dsx_info_chan chan;
	struct dsx_info_frac frac;
};
typedef struct dsx_info {
	uint type;			/* object type */
	uint id;			/* object identifier */
	uint cmd;			/* command */
	/* followed by object-specific structure */
	union dsx_info_obj info[0];
} dsx_info_t;

#define DSX_IOCGINFO	_IOWR(DSX_IOC_MAGIC,	 0, dsx_info_t)	/* get */

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
struct dsx_opt_conf_dflt {
};
struct dsx_opt_conf_span {
	uint dsx1SendCode;		/* ReadWrite */
#define DSX1SENDCODE_DSX1SENDNOCODE		 1
#define DSX1SENDCODE_DSX1SENDLINECODE		 2
#define DSX1SENDCODE_DSX1SENDPAYLOADCODE	 3
#define DSX1SENDCODE_DSX1SENDRESETCODE		 4
#define DSX1SENDCODE_DSX1SENDQRS		 5
#define DSX1SENDCODE_DSX1SEND511PATTERN		 6
#define DSX1SENDCODE_DSX1SEND3IN24PATTERN	 7
#define DSX1SENDCODE_DSX1SENDOTHERTESTPATTERN	 8
	uint dsx1LoopbackConfig;	/* ReadWrite */
#define DSX1LOOPBACKCONFIG_DSX1NOLOOP		 1
#define DSX1LOOPBACKCONFIG_DSX1PAYLOADLOOP	 2
#define DSX1LOOPBACKCONFIG_DSX1LINELOOP		 3
#define DSX1LOOPBACKCONFIG_DSX1OTHERLOOP	 4
#define DSX1LOOPBACKCONFIG_DSX1INWARDLOOP	 5
#define DSX1LOOPBACKCONFIG_DSX1DUALLOOP		 6
	uint dsx1TransmitClockSource;	/* ReadWrite */
#define DSX1TRANSMITCLOCKSOURCE_LOOPTIMING	 1
#define DSX1TRANSMITCLOCKSOURCE_LOCALTIMING	 2
#define DSX1TRANSMITCLOCKSOURCE_THROUGHTIMING	 3
#define DSX1TRANSMITCLOCKSOURCE_ADAPTIVE	 4
	uint dsx1Fdl;			/* ReadWrite */
#define DSX1FDL_OTHER				 0
#define DSX1FDL_DSX1ANSIT1403			 1
#define DSX1FDL_DSX1ATT54016			 2
#define DSX1FDL_DSX1FDLNONE			 3
	uint dsx1LineLength;		/* ReadWrite */
	uint dsx1LineStatusChangeTrapEnable;	/* ReadWrite */
#define DSX1LINESTATUSCHANGETRAPENABLE_ENABLED	 1
#define DSX1LINESTATUSCHANGETRAPENABLE_DISABLED	 2
	uint dsx1LineMode;		/* ReadWrite */
#define DSX1LINEMODE_CSU			 1
#define DSX1LINEMODE_DSU			 2
	uint dsx1LineBuildOut;		/* ReadWrite */
#define DSX1LINEBUILDOUT_NOTAPPLICABLE		 1
#define DSX1LINEBUILDOUT_NEG75DB		 2
#define DSX1LINEBUILDOUT_NEG15DB		 3
#define DSX1LINEBUILDOUT_NEG225DB		 4
#define DSX1LINEBUILDOUT_ZERODB			 5
	uint dsx1LineImpedance;		/* ReadWrite */
#define DSX1LINEIMPEDANCE_NOTAPPLICABLE		 1
#define DSX1LINEIMPEDANCE_UNBALANCED75OHMS	 2
#define DSX1LINEIMPEDANCE_BALANCED100OHMS	 3
#define DSX1LINEIMPEDANCE_BALANCED120OHMS	 4
};
struct dsx_opt_conf_chan {
	uint dsx0TransmitCodesEnable;	/* ReadWrite */
#define DSX0TRANSMITCODESENABLE_TRUE		 1
#define DSX0TRANSMITCODESENABLE_FALSE		 2
};
struct dsx_opt_conf_frac {
};
union dsx_opt_conf_obj {
	struct dsx_opt_conf_dflt dflt;
	struct dsx_opt_conf_span span;
	struct dsx_opt_conf_chan chan;
	struct dsx_opt_conf_frac frac;
};
typedef struct dsx_option {
	uint type;			/* object type */
	uint id;			/* object identifier */
	/* followed by object-specific structure */
	union dsx_opt_conf_obj option[0];
} dsx_option_t;

#define DSX_IOCGOPTION	_IOWR(DSX_IOC_MAGIC,	 1, dsx_option_t)	/* get */
#define DSX_IOCSOPTION	_IOWR(DSX_IOC_MAGIC,	 2, dsx_option_t)	/* set */

/*
 * CONFIGURATION
 *
 * Configures the object (creates, destroys, moves).  These actions require
 * reconfiguration of the object and a change in its relationship to other
 * objects.  Also, this structure contains information that must be specified
 * during creation of the object.  Each structure can optionally be followed by
 * an options structure to specify the options during creation.
 */
struct dsx_conf_dflt {
};
struct dsx_conf_span {
	uint dsx1LineIndex;		/* ReadOnly */
	uint dsx1IfIndex;		/* ReadOnly */
	uint dsx1Ds1ChannelNumber;	/* ReadOnly */
	char dsx1CircuitIdentifier[32];	/* ReadWrite */
	uint dsx1LineType;		/* ReadWrite */
#define DSX1LINETYPE_OTHER			 1
#define DSX1LINETYPE_DSX1ESF			 2
#define DSX1LINETYPE_DSX1D4			 3
#define DSX1LINETYPE_DSX1E1			 4
#define DSX1LINETYPE_DSX1E1CRC			 5
#define DSX1LINETYPE_DSX1E1MF			 6
#define DSX1LINETYPE_DSX1E1CRCMF		 7
#define DSX1LINETYPE_DSX1UNFRAMED		 8
#define DSX1LINETYPE_DSX1E1UNFRAMED		 9
#define DSX1LINETYPE_DSX1DS2M12			10
#define DSX1LINETYPE_DSX1E2			11
#define DSX1LINETYPE_DSX1E1Q50			12
#define DSX1LINETYPE_DSX1E1Q50CRC		13
#define DSX1LINETYPE_DSX1J1ESF			14
#define DSX1LINETYPE_DSX1J1UNFRAMED		16
	uint dsx1LineCoding;		/* ReadWrite */
#define DSX1LINECODING_DSX1JBZS			 1
#define DSX1LINECODING_DSX1B8ZS			 2
#define DSX1LINECODING_DSX1HDB3			 3
#define DSX1LINECODING_DSX1ZBTSI		 4
#define DSX1LINECODING_DSX1AMI			 5
#define DSX1LINECODING_OTHER			 6
#define DSX1LINECODING_DSX1B6ZS			 7
	uint dsx1SignalMode;		/* ReadWrite */
#define DSX1SIGNALMODE_NONE			 1
#define DSX1SIGNALMODE_ROBBEDBIT		 2
#define DSX1SIGNALMODE_BITORIENTED		 3
#define DSX1SIGNALMODE_MESSAGEORIENTED		 4
#define DSX1SIGNALMODE_OTHER			 5
	uint dsx1Channelization;	/* ReadWrite */
#define DSX1CHANNELIZATION_DISABLED		 1
#define DSX1CHANNELIZATION_ENABLEDDS0		 2
#define DSX1CHANNELIZATION_ENABLEDDS1		 3
};
struct dsx_conf_chan {
	uint ifIndex;			/* ReadOnly */
	uint dsx1IfIndex;		/* */
	uint dsx0Ds0ChannelNumber;	/* ReadOnly */
	char dsx0CircuitIdentifier[32];	/* ReadWrite */
	uint dsx0RobbedBitSignalling;	/* ReadWrite */
#define DSX0ROBBEDBITSIGNALLING_TRUE		 1
#define DSX0ROBBEDBITSIGNALLING_FALSE		 2
	uint dsx0IdleCode;		/* ReadWrite */
	uint dsx0SeizedCode;		/* ReadWrite */
};
struct dsx_conf_frac {
	uint dsxFracIndex;		/* ReadOnly */
	uint dsxFracNumber;		/* ReadOnly */
	uint dsxFracIfIndex;		/* ReadWrite */
};
union dsx_conf_obj {
	struct dsx_conf_dflt dflt;
	struct dsx_conf_span span;
	struct dsx_conf_chan chan;
	struct dsx_conf_frac frac;
};

#define DSX_GET		0	/* get options or configuration */
#define DSX_ADD		1	/* add configuration */
#define DSX_CHA		2	/* set options or change configuration */
#define DSX_DEL		3	/* delete configuraiton */
#define DSX_TST		4	/* test options or configuration */
#define DSX_COM		5	/* commit options or configuration */
typedef struct dsx_config {
	uint type;			/* object type */
	uint id;			/* object identifier */
	uint cmd;			/* command */
	/* followed by object-specific structure */
	union dsx_conf_obj config[0];
} dsx_config_t;

#define DSX_IOCLCONFIG	_IOWR(DSX_IOC_MAGIC,	 3, dsx_config_t)	/* list */
#define DSX_IOCGCONFIG	_IOWR(DSX_IOC_MAGIC,	 4, dsx_config_t)	/* get */
#define DSX_IOCSCONFIG	_IOWR(DSX_IOC_MAGIC,	 5, dsx_config_t)	/* set */
#define DSX_IOCTCONFIG	_IOWR(DSX_IOC_MAGIC,	 6, dsx_config_t)	/* test */
#define DSX_IOCCCONFIG	_IOWR(DSX_IOC_MAGIC,	 7, dsx_config_t)	/* commit */

/*
 * STATE
 *
 * Provides state machine state information.  The purpose of these items is
 * primarily for debugging: to view the internal state of the state machine.
 * Some of these state variables might also be reflected in status fields.
 */
struct dsx_statem_dflt {
};
struct dsx_statem_span {
};
struct dsx_statem_chan {
};
struct dsx_statem_frac {
};
union dsx_statem_obj {
	struct dsx_statem_dflt dflt;
	struct dsx_statem_span span;
	struct dsx_statem_chan chan;
	struct dsx_statem_frac frac;
};
typedef struct dsx_statem {
	uint type;			/* object type */
	uint id;			/* object identifier */
	uint cmd;			/* command */
	/* followed by object-specific structure */
	union dsx_statem_obj statem[0];
} dsx_statem_t;

#define DSX_IOCGSTATEM	_IOWR(DSX_IOC_MAGIC,	 8, dsx_statem_t)	/* get */
#define DSX_IOCCMRESET	_IOWR(DSX_IOC_MAGIC,	 9, dsx_statem_t)	/* master reset */

/*
 * STATUS
 *
 * Provides status on the object.  Many of these are X.721 status fields or
 * object-specific status information.  Only read-write status fields may be
 * altered with a set or clear command.
 */
struct dsx_status_dflt {
};
struct dsx_status_span {
	uint dsx1LineStatus;		/* ReadOnly */
#define DSX1LINESTATUS_DSX1NOALARM		 0
#define DSX1LINESTATUS_DSX1RCVFARENDLOF		 1
#define DSX1LINESTATUS_DSX1XMTFARENDLOF		 2
#define DSX1LINESTATUS_DSX1RCVAIS		 3
#define DSX1LINESTATUS_DSX1XMTAIS		 4
#define DSX1LINESTATUS_DSX1LOSSOFFRAME		 5
#define DSX1LINESTATUS_DSX1LOSSOFSIGNAL		 6
#define DSX1LINESTATUS_DSX1LOOPBACKSTATE	 7
#define DSX1LINESTATUS_DSX1T16AIS		 8
#define DSX1LINESTATUS_DSX1RCVFARENDLOMF	 9
#define DSX1LINESTATUS_DSX1XMTFARENDLOMF	10
#define DSX1LINESTATUS_DSX1RCVTESTCODE		11
#define DSX1LINESTATUS_DSX1OTHERFAILURE		12
#define DSX1LINESTATUS_DSX1UNAVAILSIGSTATE	13
#define DSX1LINESTATUS_DSX1NETEQUIPOOS		14
#define DSX1LINESTATUS_DSX1RCVPAYLOADAIS	15
#define DSX1LINESTATUS_DSX1DS2PERFTHRESHOLD	16
	uint dsx1LineStatusLastChange;	/* ReadOnly */
	uint dsx1LoopbackStatus;	/* ReadOnly */
#define DSX1LOOPBACKSTATUS_DSX1NOLOOPBACK		 0
#define DSX1LOOPBACKSTATUS_DSX1NEARENDPAYLOADLOOPBACK	 1
#define DSX1LOOPBACKSTATUS_DSX1NEARENDLINELOOPBACK	 2
#define DSX1LOOPBACKSTATUS_DSX1NEARENDOTHERLOOPBACK	 3
#define DSX1LOOPBACKSTATUS_DSX1NEARENDINWARDLOOPBACK	 4
#define DSX1LOOPBACKSTATUS_DSX1FARENDPAYLOADLOOPBACK	 5
#define DSX1LOOPBACKSTATUS_DSX1FARENDLINELOOPBACK	 6
	uint dsx0Ds0BundleMappedIfIndex;	/* ReadOnly */
};
struct dsx_status_chan {
	uint dsx0ReceivedCode;		/* ReadOnly */
};
struct dsx_status_frac {
};
union dsx_status_obj {
	struct dsx_status_dflt dflt;
	struct dsx_status_span span;
	struct dsx_status_chan chan;
	struct dsx_status_frac frac;
};
typedef struct dsx_status {
	uint type;			/* object type */
	uint id;			/* object identifier */
	/* followed by object-specific structure */
	union dsx_status_obj status[0];
} dsx_status_t;

#define DSX_IOCGSTATUS	_IOWR(DSX_IOC_MAGIC,	10, dsx_status_t)	/* get */
#define DSX_IOCSSTATUS	_IOWR(DSX_IOC_MAGIC,	11, dsx_status_t)	/* set */
#define DSX_IOCCSTATUS	_IOWR(DSX_IOC_MAGIC,	12, dsx_status_t)	/* clear */

/*
 * STATISTICS
 *
 * Interval zero (0) contains current information.  Intervals (1..N) contain
 * historical data for the last N periods.  Interval (0xffffffff) contains
 * totals.
 */
struct dsx_stats_dflt {
};
struct dsx_stats_span {
	union {
		struct {
			uint dsx1ValidIntervals;	/* ReadOnly */
			uint dsx1InvalidIntervals;	/* ReadOnly */
			uint dsx1ESs;	/* ReadOnly */
			uint dsx1SESs;	/* ReadOnly */
			uint dsx1SEFSs;	/* ReadOnly */
			uint dsx1UASs;	/* ReadOnly */
			uint dsx1CSSs;	/* ReadOnly */
			uint dsx1PCVs;	/* ReadOnly */
			uint dsx1LESs;	/* ReadOnly */
			uint dsx1BESs;	/* ReadOnly */
			uint dsx1DMs;	/* ReadOnly */
			uint dsx1LCVs;	/* ReadOnly *//* near end only */
			uint dsx1ValidData;	/* ReadOnly */
#define DSX1INTERVALVALIDDATA_TRUE		 1
#define DSX1INTERVALVALIDDATA_FALSE		 2
		} NearEnd;
#define	DSX_SUBTYPE_NEAREND	1
		struct {
			uint dsx1ValidIntervals;	/* ReadOnly */
			uint dsx1InvalidIntervals;	/* ReadOnly */
			uint dsx1ESs;	/* ReadOnly */
			uint dsx1SESs;	/* ReadOnly */
			uint dsx1SEFSs;	/* ReadOnly */
			uint dsx1UASs;	/* ReadOnly */
			uint dsx1CSSs;	/* ReadOnly */
			uint dsx1PCVs;	/* ReadOnly */
			uint dsx1LESs;	/* ReadOnly */
			uint dsx1BESs;	/* ReadOnly */
			uint dsx1DMs;	/* ReadOnly */
			uint dsx1ValidData;	/* ReadOnly */
#define DSX1FARENDINTERVALVALIDDATA_TRUE	 1
#define DSX1FARENDINTERVALVALIDDATA_FALSE	 2
		} FarEnd;
#define	DSX_SUBTYPE_FAREND	2
	};
};
struct dsx_stats_chan {
};
struct dsx_stats_frac {
};
union dsx_stats_obj {
	struct dsx_stats_dflt dflt;
	struct dsx_stats_span span;
	struct dsx_stats_chan chan;
	struct dsx_stats_frac frac;
};
typedef struct dsx_stats {
	uint type;			/* object type */
	uint id;			/* object identifier */
	uint subtype;			/* statistics subtype */
	uint interval;			/* time interval */
	uint time;			/* time period */
	uint stamp;			/* time stamp */
	/* followed by object-specific structure */
	union dsx_stats_obj stats[0];
} dsx_stats_t;

#define DSX_IOCGSTATSP	_IOWR(DSX_IOC_MAGIC,	13, dsx_stats_t)	/* get period */
#define DSX_IOCSSTATSP	_IOWR(DSX_IOC_MAGIC,	14, dsx_stats_t)	/* set period */
#define DSX_IOCGSTATS	_IOWR(DSX_IOC_MAGIC,	15, dsx_stats_t)	/* get */
#define DSX_IOCCSTATS	_IOWR(DSX_IOC_MAGIC,	16, dsx_stats_t)	/* get and clear */

/*
 * EVENTS
 *
 * Provides a mechanism for requesting event notifications.  The flags field in
 * the header is used to requesting X.721 events (e.g. object creation
 * notification).  Events can be requested by object class, and a mechanism
 * exists for requesting events by specific object; however, the driver is not
 * required to support per-object notification and will return EOPNOTSUPP.  To
 * request notification for an object class, specific zero (0) for the
 * object identifier.
 *
 * Event requests are per requesting Stream.  Non-management streams should only
 * be notified of events for objects associated with the Stream and not X.721
 * events unless also specified as a specific event (e.g. communications alarm
 * events).  Management Streams can be notified of any object.  Notifications
 * are provided using the LMI_EVENT_IND (event indication) for X.721 events, and
 * the MX_EVENT_IND (for other events).
 */
struct dsx_notify_dflt {
	uint events;
};
struct dsx_notify_span {
	uint events;
};
struct dsx_notify_chan {
	uint events;
};
struct dsx_notify_frac {
	uint events;
};
union dsx_notify_obj {
	struct dsx_notify_dflt dflt;
	struct dsx_notify_span span;
	struct dsx_notify_chan chan;
	struct dsx_notify_frac frac;
};

#define DSX_SYNCTRANSITION		(1<< 0)
#define DSX_LOOPUP			(1<< 1)
#define DSX_LOOPDOWN			(1<< 2)
#define DSX_YELLOWALARM			(1<< 3)
#define DSX_YELLOWALARM_CLEARED		(1<< 4)
#define DSX_REDALARM			(1<< 5)
#define DSX_REDALARM_CLEARED		(1<< 6)
#define DSX_BLUEALARM			(1<< 7)
#define DSX_BLUEALARM_CLEARED		(1<< 8)
#define DSX_TESTABORTED			(1<< 9)
#define DSX_TESTCOMPLETE		(1<<10)
typedef struct dsx_notify {
	uint type;			/* object type */
	uint id;			/* object identifier */
	uint flags;			/* X.721 flags */
	/* followed by object-specific structure */
	union dsx_notify_obj events[0];
} dsx_notify_t;

#define DSX_IOCGNOTIFY	_IOWR(DSX_IOC_MAGIC,	17, dsx_notify_t)	/* get */
#define DSX_IOCSNOTIFY	_IOWR(DSX_IOC_MAGIC,	18, dsx_notify_t)	/* set */
#define DSX_IOCCNOTIFY	_IOWR(DSX_IOC_MAGIC,	19, dsx_notify_t)	/* clear */

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
struct dsx_attr_dflt {
	struct dsx_conf_dflt config;
	struct dsx_opt_conf_dflt option;
	struct dsx_info_dflt inform;
	struct dsx_statem_dflt statem;
	struct dsx_status_dflt status;
	struct dsx_stats_dflt stats;
	struct dsx_notify_dflt events;
};
struct dsx_attr_span {
	struct dsx_conf_span config;
	struct dsx_opt_conf_span option;
	struct dsx_info_span inform;
	struct dsx_statem_span statem;
	struct dsx_status_span status;
	struct dsx_stats_span stats;
	struct dsx_notify_span events;
};
struct dsx_attr_chan {
	struct dsx_conf_chan config;
	struct dsx_opt_conf_chan option;
	struct dsx_info_chan inform;
	struct dsx_statem_chan statem;
	struct dsx_status_chan status;
	struct dsx_stats_chan stats;
	struct dsx_notify_chan events;
};
struct dsx_attr_frac {
	struct dsx_conf_frac config;
	struct dsx_opt_conf_frac option;
	struct dsx_info_frac inform;
	struct dsx_statem_frac statem;
	struct dsx_status_frac status;
	struct dsx_stats_frac stats;
	struct dsx_notify_frac events;
};
union dsx_attr_obj {
	struct dsx_attr_dflt dflt;
	struct dsx_attr_span span;
	struct dsx_attr_chan chan;
	struct dsx_attr_frac frac;
};
typedef struct dsx_attr {
	uint type;			/* object type */
	uint id;			/* object identifier */
	uint cmd;			/* command */
	/* followed by object-specific structure */
	union dsx_attr_obj attrs[0];
} dsx_attr_t;

#define	DSX_IOCGATTR	_IOWR(DSX_IOC_MAGIC,	20, dsx_attr_t)	/* get attributes */

/*
 * MANAGEMENT
 *
 * Provides specific and defined management actions that can be affected on
 * objects.
 */
struct dsx_action_dflt {
};
struct dsx_action_span {
};
struct dsx_action_chan {
};
struct dsx_action_frac {
};
union dsx_action_obj {
	struct dsx_action_dflt dflt;
	struct dsx_action_span span;
	struct dsx_action_chan chan;
	struct dsx_action_frac frac;
};

#define DSX_TST_STOP		 1
#define DSX_TST_FBIT		 2
#define DSX_TST_SPAN		 3
#define DSX_TST_CHANNELS		 4
typedef struct dsx_mgmt {
	uint type;			/* object type */
	uint id;			/* object identifier */
	uint cmd;			/* command */
	/* followed by object-specific structure */
	union dsx_action_obj action[0];
} dsx_mgmt_t;

#define DSX_IOCCMGMT	_IOWR(DSX_IOC_MAGIC,	21, dsx_mgmt_t)	/* command */

/*
 * PASS LOWER
 *
 * This structure is deprecated and as largely used for testing as a mechanism
 * to bypass an input-output control to a stream linked under a multiplexing
 * driver supporting this interface.
 */
typedef struct dsx_pass {
	int muxid;			/* mux index of lower stream */
	uint8_t type;			/* type of message block */
	uint8_t band;			/* band of message block */
	uint ctl_length;		/* length of ctrl part */
	uint dat_length;		/* length of data part */
	/* followed by ctrl and data part of message to pass */
} dsx_pass_t;

#define DSX_IOCCPASS	_IOWR(DSX_IOC_MAGIC,	22, dsx_pass_t)	/* pass */

#define DSX_IOC_FIRST	0
#define DSX_IOC_LAST	22
#define DSX_IOC_PRIVATE	32

#endif				/* __SYS_DSX_IOCTL_H__ */
