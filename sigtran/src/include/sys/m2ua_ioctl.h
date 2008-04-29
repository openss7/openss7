/*****************************************************************************

 @(#) $Id: m2ua_ioctl.h,v 0.9.2.5 2008-04-29 01:52:23 brian Exp $

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

 Last Modified $Date: 2008-04-29 01:52:23 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: m2ua_ioctl.h,v $
 Revision 0.9.2.5  2008-04-29 01:52:23  brian
 - updated headers for release

 Revision 0.9.2.4  2007/08/14 08:34:06  brian
 - GPLv3 header update

 *****************************************************************************/

#ifndef __M2UA_IOCTL_H__
#define __M2UA_IOCTL_H__

#ident "@(#) $RCSfile: m2ua_ioctl.h,v $ $Name:  $($Revision: 0.9.2.5 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

/* This file can be processed by doxygen(1). */

#define M2UA_IOC_MAGIC	'2'

#define M2UA_OBJ_TYPE_LM	 0	/* layer management */
#define M2UA_OBJ_TYPE_AS_U	 1	/* application server (user) */
#define M2UA_OBJ_TYPE_AS_P	 2	/* application server (provider) */
#define M2UA_OBJ_TYPE_SP	 3	/* signalling process */
#define M2UA_OBJ_TYPE_SG	 4	/* signalling gateway */
#define M2UA_OBJ_TYPE_SPP	 5	/* signalling process proxy */
#define M2UA_OBJ_TYPE_ASP	 6	/* signalling process proxy */
#define M2UA_OBJ_TYPE_SGP	 7	/* signalling process proxy */
#define M2UA_OBJ_TYPE_SL_U	 8	/* signalling link (user) */
#define M2UA_OBJ_TYPE_SL_P	 9	/* signalling link (provider) */
#define M2UA_OBJ_TYPE_XP_SCTP	10	/* transport provider */
#define M2UA_OBJ_TYPE_XP_TCP	11	/* transport provider */
#define M2UA_OBJ_TYPE_XP_SSCOP	12	/* transport provider */
#define M2UA_OBJ_TYPE_DF	13	/* default */

typedef struct m2ua_addr {
	ulong spid;			/* signalling point identifier */
	ushort sdti;			/* signalling data terminal identifier */
	ushort sdli;			/* signalling data link identifier */
} m2ua_addr_t;

/*
 *  Application Server options
 *  -----------------------------------
 */
typedef struct m2ua_opt_conf_as {
	ulong tack;
} m2ua_opt_conf_as_t;

/*
 *  Signalling Process options
 *  -----------------------------------
 */
typedef struct m2ua_opt_conf_sp {
	ulong tack;
	ulong tbeat;
	ulong tidle;
} m2ua_opt_conf_sp_t;

/*
 *  Signalling Process Proxy options
 *  -----------------------------------
 */
typedef struct m2ua_opt_conf_spp {
	ulong tack;
} m2ua_opt_conf_spp_t;

/*
 *  Signalling Link options
 *  -----------------------------------
 */
typedef struct m2ua_opt_conf_sl {
	ulong tack;
} m2ua_opt_conf_sl_t;

/*
 *  Transport Provider options
 *  -----------------------------------
 */
typedef struct m2ua_opt_conf_xp {
	ulong tack;
} m2ua_opt_conf_xp_t;

/*
 *  Default options
 *  -----------------------------------
 */
typedef struct m2ua_opt_conf_df {
} m2ua_opt_conf_df_t;

/*
 *  OPTIONS
 */
typedef struct m2ua_option {
	ulong type;			/* object type */
	ulong id;			/* object id */
	/* 
	   followed by object-specific option structure */
} m2ua_option_t;

#define M2UA_IOCGOPTIONS	_IOR(	M2UA_IOC_MAGIC,	 0,	m2ua_option_t	)
#define M2UA_IOCSOPTIONS	_IOW(	M2UA_IOC_MAGIC,	 1,	m2ua_option_t	)

/*
 *  Application Server configuration
 *  -----------------------------------
 */
typedef struct m2ua_conf_as {
	ulong spid;			/* signalling process identifier */
	ulong iid;			/* interface id */
	m2ua_addr_t add;		/* signalling link address */
} m2ua_conf_as_t;

/*
 *  Signalling Process configuration
 *  -----------------------------------
 */
typedef struct m2ua_conf_sp {
	ulong spid;			/* paired signalling process identifier */
	ulong cost;			/* cost */
	ulong tmode;			/* traffic mode */
} m2ua_conf_sp_t;

/*
 *  Signalling Process Proxy configuration
 *  -----------------------------------
 */
typedef struct m2ua_conf_spp {
	ulong spid;			/* signalling process identifier */
	ulong aspid;			/* ASP Id */
	ulong cost;			/* cost */
} m2ua_conf_spp_t;

/*
 *  Signalling Link configuration
 *  -----------------------------------
 */
typedef struct m2ua_conf_sl {
	ulong asid;			/* application server id */
	ulong muxid;			/* lower multiplexing driver id */
	ulong iid;			/* interface id */
	m2ua_addr_t add;		/* signalling link address */
	lmi_option_t proto;		/* protocol variant and options */
} m2ua_conf_sl_t;

/*
 *  Transport Provider configuration
 *  -----------------------------------
 */
typedef struct m2ua_conf_xp {
	ulong sppid;			/* signalling process proxy identifier */
	ulong spid;			/* signalling process identifier */
	ulong muxid;			/* lower multiplexing driver id */
} m2ua_conf_xp_t;

/*
 *  Default configuration
 *  -----------------------------------
 */
typedef struct m2ua_conf_df {
	lmi_option_t proto;		/* protocol variant and options */
} m2ua_conf_df_t;

/*
 *  CONFIGURATION
 */
typedef struct m2ua_config {
	ulong type;			/* object type */
	ulong id;			/* object id */
	ulong cmd;			/* object command */
	/* 
	   followed by object-specific config structure */
} m2ua_config_t;

#define M2UA_GET	0
#define M2UA_ADD	1
#define M2UA_CHA	2
#define M2UA_DEL	3

#define M2UA_IOCGCONFIG		_IOWR(	M2UA_IOC_MAGIC,	 2,	m2ua_config_t	)
#define M2UA_IOCSCONFIG		_IOWR(	M2UA_IOC_MAGIC,	 3,	m2ua_config_t	)
#define M2UA_IOCTCONFIG		_IOWR(	M2UA_IOC_MAGIC,	 4,	m2ua_config_t	)
#define M2UA_IOCCCONFIG		_IOWR(	M2UA_IOC_MAGIC,	 5,	m2ua_config_t	)

/*
 *  Application Server state
 *  -----------------------------------
 */
typedef struct m2ua_timers_as {
	ulong tack;
} m2ua_timers_as_t;
typedef struct m2ua_statem_as {
	m2ua_timers_as_t timers;
	ulong as_numb;			/* number of id/state pairs for AS */
	ulong spp_numb;			/* number of id/state pairs for SPP */
	/* followed by id/state pairs for AS */
	/* followed by id/state pairs for SPP */
	/* terminated by zero */
} m2ua_statem_as_t;

/*
 *  Signalling Process state
 *  -----------------------------------
 */
typedef struct m2ua_timers_sp {
	ulong tack;
} m2ua_timers_sp_t;
typedef struct m2ua_statem_sp {
	m2ua_timers_sp_t timers;
	ulong sp_numb;			/* number of id/state pairs for SP */
	/* followed by id/state pairs for SP */
	/* terminated by zero */
} m2ua_statem_sp_t;

/*
 *  Signalling Process Proxy state
 *  -----------------------------------
 */
typedef struct m2ua_timers_spp {
	ulong tack;
	ulong tbeat;
	ulong tidle;
} m2ua_timers_spp_t;
typedef struct m2ua_statem_spp {
	m2ua_timers_spp_t timers;
	ulong as_numb;			/* number of id/state pairs for AS */
	/* followed by id/state pairs for AS */
	/* terminated by zero */
} m2ua_statem_spp_t;

/*
 *  Signalling Link state
 *  -----------------------------------
 */
typedef struct m2ua_timers_sl {
	ulong tack;
} m2ua_timers_sl_t;
typedef struct m2ua_statem_sl {
	m2ua_timers_sl_t timers;
} m2ua_statem_sl_t;

/*
 *  Transport Provider state
 *  -----------------------------------
 */
typedef struct m2ua_timers_xp {
	ulong tack;
} m2ua_timers_xp_t;
typedef struct m2ua_statem_xp {
	m2ua_timers_xp_t timers;
} m2ua_statem_xp_t;

/*
 *  Default state
 *  -----------------------------------
 */
typedef struct m2ua_timers_df {
} m2ua_timers_df_t;
typedef struct m2ua_statem_df {
	m2ua_timers_df_t timers;
} m2ua_statem_df_t;

/*
 *  STATE
 */
typedef struct m2ua_statem {
	ulong type;			/* object type */
	ulong id;			/* object id */
	ulong flags;			/* object flags */
	ulong state;			/* object state */
	/* followed by object-specific state structure */
} m2ua_statem_t;

#define M2UA_IOCGSTATEM		_IOWR(	M2UA_IOC_MAGIC,	 6,	m2ua_statem_t	)
#define M2UA_IOCCMRESET		_IOWR(	M2UA_IOC_MAGIC,	 7,	m2ua_statem_t	)

/*
 *  Application Server stats
 *  -----------------------------------
 */
typedef struct m2ua_stats_as {
} m2ua_stats_as_t;

/*
 *  Signalling Process stats
 *  -----------------------------------
 */
typedef struct m2ua_stats_sp {
} m2ua_stats_sp_t;

/*
 *  Signalling Process Proxy stats
 *  -----------------------------------
 */
typedef struct m2ua_stats_spp {
} m2ua_stats_spp_t;

/*
 *  Signalling Link stats
 *  -----------------------------------
 */
typedef struct m2ua_stats_sl {
} m2ua_stats_sl_t;

/*
 *  Transport Provider stats
 *  -----------------------------------
 */
typedef struct m2ua_stats_xp {
} m2ua_stats_xp_t;

/*
 *  Default stats
 *  -----------------------------------
 */
typedef struct m2ua_stats_df {
} m2ua_stats_df_t;

/*
 *  STATISTICS
 */
typedef struct m2ua_stats {
	ulong type;			/* object type */
	ulong id;			/* object id */
	ulong header;			/* object stats header */
	/* followed by object-specific stats structure */
} m2ua_stats_t;

#define M2UA_IOCGSTATSP		_IOWR(	M2UA_IOC_MAGIC,	 8,	m2ua_stats_t	)
#define M2UA_IOCSSTATSP		_IOWR(	M2UA_IOC_MAGIC,	 9,	m2ua_stats_t	)
#define M2UA_IOCGSTATS		_IOWR(	M2UA_IOC_MAGIC,	10,	m2ua_stats_t	)
#define M2UA_IOCCSTATS		_IOWR(	M2UA_IOC_MAGIC,	11,	m2ua_stats_t	)

/*
 *  EVENTS
 */
typedef struct m2ua_notify {
	ulong type;			/* object type */
	ulong id;			/* object id */
	lmi_notify_t notify;		/* notifications */
} m2ua_notify_t;

#define M2UA_IOCGNOTIFY		_IOR(	M2UA_IOC_MAGIC,	12,	m2ua_notify_t	)
#define M2UA_IOCSNOTIFY		_IOR(	M2UA_IOC_MAGIC,	13,	m2ua_notify_t	)
#define M2UA_IOCCNOTIFY		_IOR(	M2UA_IOC_MAGIC,	14,	m2ua_notify_t	)

/*
 *  MANAGEMENT
 */
typedef struct m2ua_mgmt {
	ulong type;			/* object type */
	ulong id;			/* object id */
	ulong cmd;			/* object command */
} m2ua_mgmt_t;

#define M2UA_MGMT_UP		1
#define M2UA_MGMT_DOWN		2
#define M2UA_MGMT_ACTIVATE	3
#define M2UA_MGMT_DEACTIVATE	4
#define M2UA_MGMT_UP_BLOCK	5
#define M2UA_MGMT_UP_UNBLOCK	6
#define M2UA_MGMT_ACT_BLOCK	7
#define M2UA_MGMT_ACT_UNBLOCK	8

#define M2UA_IOCCMGMT		_IOWR(	M2UA_IOC_MAGIC,	15,	m2ua_mgmt_t	)

/*
 *  CONTROL LOWER
 */
typedef struct m2ua_pass {
	ulong muxid;			/* mux index of lower stream to pass message to */
	ulong type;			/* type of message block */
	ulong band;			/* band of message block */
	ulong ctl_length;		/* length of cntl part */
	ulong dat_length;		/* length of data part */
	/* followed by cntl and data part of message to pass to lower */
} m2ua_pass_t;

#define M2UA_IOCCPASS		_IOWR(	M2UA_IOC_MAGIC,	16,	m2ua_pass_t	)

#define M2UA_IOC_FIRST		 0
#define M2UA_IOC_LAST		16
#define M2UA_IOC_PRIVATE	32

#endif				/* __M2UA_IOCTL_H__ */
