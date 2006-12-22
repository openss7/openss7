/*****************************************************************************

 @(#) $Id: mgi_ioctl.h,v 0.9.2.3 2006/12/22 07:17:38 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
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

 Last Modified $Date: 2006/12/22 07:17:38 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: mgi_ioctl.h,v $
 Revision 0.9.2.3  2006/12/22 07:17:38  brian
 - updated documentation for release (make check target)

 Revision 0.9.2.2  2006/12/20 23:07:39  brian
 - updates for release and current development

 Revision 0.9.2.1  2006/10/14 06:37:28  brian
 - added manpages, module, drivers, headers from strss7 package

 *****************************************************************************/

#ifndef __SYS_MGI_IOCTL_H__
#define __SYS_MGI_IOCTL_H__

#ident "@(#) $RCSfile: mgi_ioctl.h,v $ $Name:  $($Revision: 0.9.2.3 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

/* This file can be processed by doxygen(1). */

#include <linux/ioctl.h>

#define MG_IOC_MAGIC	'G'

#define MG_OBJ_TYPE_MG		1	/* media gateway control */
#define MG_OBJ_TYPE_SE		2	/* communications session */
#define MG_OBJ_TYPE_LG		3	/* connection leg */
#define MG_OBJ_TYPE_CH		4	/* channel */
#define MG_OBJ_TYPE_MX		5	/* multiplex */
#define MG_OBJ_TYPE_DF		6	/* default */

/*
 *  Media Gateway options
 *  -----------------------------------
 */
typedef struct mg_opt_conf_mg {
	ulong type;			/* media type */
	ulong flags;			/* options flags */
	ulong encoding;			/* encoding */
	ulong block_size;		/* data block size (bits) */
	ulong sample_size;		/* sample size (bits) */
	ulong samples;			/* samples per block */
	ulong rate;			/* clock rate (samples/second) */
	ulong tx_channels;		/* number of tx channels */
	ulong rx_channels;		/* number of rx channels */
} mg_opt_conf_mg_t;

/*
 *  Multiplex options
 *  -----------------------------------
 *  A multiplex is an upper or lower stream.
 *
 *  This specifies the media characteristics of a simple multiplex.  This is
 *  typically a read-only options structure.  To change media
 *  characteristics, change the characteristics of a channel a multiplex
 *  normally requires that the lower stream be unlinked an relinked or the
 *  pass-thru ioctls performed.
 */
typedef struct mg_opt_conf_mx {
	ulong type;			/* media type */
	ulong flags;			/* options flags */
	ulong encoding;			/* encoding */
	ulong block_size;		/* data block size (bits) */
	ulong sample_size;		/* sample size (bits) */
	ulong samples;			/* samples per block */
	ulong rate;			/* clock rate (samples/second) */
	ulong tx_channels;		/* number of tx channels */
	ulong rx_channels;		/* number of rx channels */
} mg_opt_conf_mx_t;

/*
 *  Channel options
 *  -----------------------------------
 *  A channel is a slot in a media stream.
 *
 *  This specifies the media characteristics of the channel.  The number of
 *  tx_channels and rx_channels must be 1 or 0.
 */
typedef struct mg_opt_conf_ch {
	ulong type;			/* media type */
	ulong flags;			/* options flags */
	ulong encoding;			/* encoding */
	ulong block_size;		/* data block size (bits) */
	ulong sample_size;		/* sample size (bits) */
	ulong samples;			/* samples per block */
	ulong rate;			/* clock rate (samples/second) */
	ulong tx_channels;		/* number of tx channels */
	ulong rx_channels;		/* number of rx channels */
} mg_opt_conf_ch_t;

/*
 *  Connection Leg options
 *  -----------------------------------
 *  A connection leg is a collection of channels that act together towards
 *  a termination that are normally controlled as a group.  An example would
 *  be multiple 64kbps channels making up a multi-rate call.  Channels must be
 *  added to connection legs before they can be used.  Channels that are
 *  not engaged in a call can remain attached to their connection legs for
 *  their entire life-cycle, or can be rearranged by the media gateway.
 *
 *  Connection legs have their own characteristics.  Only connection legs
 *  of like characteristics to a communications session can be associated with
 *  a communications session.
 *
 *  Connection legs do not necessarily have the same media characteristics
 *  as the channels that make up the termination point.  The connection leg
 *  object performs media conversion between the channel media type and the
 *  conenction leg.
 *
 *  Channels which have multiple embedded Tx and Rx channels can be associated
 *  with more than one Connection Leg.
 *
 *  When an channel is added to a communications session, connection leg
 *  objects may be dynamically  created that will adapt between the media
 *  type of the communications session and the media type of the channel.
 *  In that case, commnection leg characteristics are read-only.
 */
typedef struct mg_opt_conf_lg {
	ulong type;			/* media type */
	ulong flags;			/* options flags */
	ulong encoding;			/* encoding */
	ulong block_size;		/* data block size (bits) */
	ulong sample_size;		/* sample size (bits) */
	ulong samples;			/* samples per block */
	ulong rate;			/* clock rate (samples/second) */
	ulong tx_channels;		/* number of tx channels */
	ulong rx_channels;		/* number of rx channels */
} mg_opt_conf_lg_t;

/*
 *  Communications Session options
 *  -----------------------------------
 *  A communications session is a collection of connection legs engaged in a
 *  communications session.  A communications session has its own media
 *  characteristics.  Only terminations points of like characteristics can be
 *  associated in a communications session.  Comunications sessions do not do
 *  media conversion between connection legs.  Connection legs are responsible
 *  for performing any necessary media conversion between the communications
 *  session and the channel.
 *
 *  Not all communications sessions necessarily support all media types.  For
 *  example, communications sessions which support conferencing might only
 *  support 16-bit linear PCM coded voice.  The reason being that conferencing
 *  calculations need not perform conversion.
 *
 *  The media characteristics refer the to media characteristics of a
 *  participant in the communications session as supported by the
 *  participating connnection legs.
 *
 *  Communications session are by dynamically created by adding channels or
 *  connection legs to the NULL communications session.  These configuration
 *  options are normall read-only.
 */
typedef struct mg_opt_conf_se {
	ulong type;			/* media type */
	ulong flags;			/* options flags */
	ulong encoding;			/* encoding */
	ulong block_size;		/* data block size (bits) */
	ulong sample_size;		/* sample size (bits) */
	ulong samples;			/* samples per block */
	ulong rate;			/* clock rate (samples/second) */
	ulong tx_channels;		/* number of tx channels */
	ulong rx_channels;		/* number of rx channels */
} mg_opt_conf_se_t;

/*
 *  Default options
 *  -----------------------------------
 *  When channels, connection legs and communications sessions are newly
 *  created and can support different media types, the default media type will
 *  be initially assigned.
 */
typedef struct mg_opt_conf_df {
	ulong type;			/* media type */
	ulong flags;			/* options flags */
	ulong encoding;			/* encoding */
	ulong block_size;		/* data block size (bits) */
	ulong sample_size;		/* sample size (bits) */
	ulong samples;			/* samples per block */
	ulong rate;			/* clock rate (samples/second) */
	ulong tx_channels;		/* number of tx channels */
	ulong rx_channels;		/* number of rx channels */
} mg_opt_conf_df_t;

/*
 *  OPTIONS
 */
typedef struct mg_option {
	ulong type;
	ulong id;
	/* followed by specific object options structure */
} mg_option_t;

#define MG_IOCGOPTIONS	_IOWR(	MG_IOC_MAGIC,	 0,	mg_option_t	)
#define MG_IOCSOPTIONS	_IOWR(	MG_IOC_MAGIC,	 1,	mg_option_t	)

/*
 *  Media Gateway configuration
 *  -----------------------------------
 */
typedef struct mg_conf_mg {
} mg_conf_mg_t;

/*
 *  Multiplex configuration
 *  -----------------------------------
 */
typedef struct mg_conf_mx {
	ulong muxid;			/* lower multiplexing driver id */
} mg_conf_mx_t;

/*
 *  Channel configuration
 *  -----------------------------------
 */
typedef struct mg_conf_ch {
	ulong tpid;			/* termination point id */
	ulong mxid;			/* multiplex id */
	ulong slot;			/* slot in multiplex */
	ulong encoding;			/* channel encoding */
} mg_conf_ch_t;

/*
 *  Connection leg configuration
 *  -----------------------------------
 *  Connection leg object are not created statically.
 */
typedef struct mg_conf_lg {
	ulong seid;			/* session id */
} mg_conf_lg_t;

/*
 *  Communications Session configuration
 *  -----------------------------------
 *  Communications session objects are not created statically.
 */
typedef struct mg_conf_se {
} mg_conf_se_t;

/*
 *  Default configuration
 *  -----------------------------------
 *  The default object is not created statically.
 */
typedef struct mg_conf_df {
} mg_conf_df_t;

/*
 *  CONFIGURATION
 */
typedef struct mg_config {
	ulong type;			/* object type */
	ulong id;			/* object id */
	ulong cmd;			/* configuration command */
	/* followed by specific configuration structure */
} mg_config_t;

#define MG_GET		0	/* get configuration */
#define MG_ADD		1	/* add configuration */
#define MG_CHA		2	/* cha configuration */
#define MG_DEL		3	/* del configuration */

#define MG_IOCGCONFIG	_IOWR(	MG_IOC_MAGIC,	2,	mg_config_t	)
#define MG_IOCSCONFIG	_IOWR(	MG_IOC_MAGIC,	3,	mg_config_t	)
#define MG_IOCTCONFIG	_IOWR(	MG_IOC_MAGIC,	4,	mg_config_t	)
#define MG_IOCCCONFIG	_IOWR(	MG_IOC_MAGIC,	5,	mg_config_t	)

/*
 *  Multiplex state
 *  -----------------------------------
 */
typedef struct mg_timers_mx {
} mg_timers_mx_t;
typedef struct mg_statem_mx {
	struct mg_timers_mx timers;
	/* followed by the channel associations */
	struct {
		ulong slot;		/* slot number */
		ulong chid;		/* channel id */
	} slot[0];
} mg_statem_mx_t;

/*
 *  Channel state
 *  -----------------------------------
 */
typedef struct mg_timers_ch {
} mg_timers_ch_t;
typedef struct mg_statem_ch {
	struct mg_timers_ch timers;
	ulong mxid;
} mg_statem_ch_t;

/*
 *  Termination Point state
 *  -----------------------------------
 */
typedef struct mg_timers_lg {
} mg_timers_lg_t;
typedef struct mg_statem_lg {
	struct mg_timers_lg timers;
	ulong mxid;
} mg_statem_lg_t;

/*
 *  Communications Session state
 *  -----------------------------------
 */
typedef struct mg_timers_se {
} mg_timers_se_t;
typedef struct mg_statem_se {
	struct mg_timers_se timers;
	/* followed by the connection leg participation */
	struct {
		ulong role;		/* participant role */
		ulong flags;		/* topology flags */
		ulong lgid;		/* connection leg id */
	} leg[0];
} mg_statem_se_t;

/*
 *  Media Gateway state
 *  -----------------------------------
 */
typedef struct mg_timers_mg {
} mg_timers_mg_t;
typedef struct mg_statem_mg {
	struct mg_timers_mg timers;
} mg_statem_mg_t;

/*
 *  Default state
 *  -----------------------------------
 */
typedef struct mg_timers_df {
} mg_timers_df_t;
typedef struct mg_statem_df {
	struct mg_timers_df timers;
	/* followed by a list of connection sessions */
	struct {
		ulong seid;		/* session identifier */
	} sessions[0];
} mg_statem_df_t;

/*
 *  STATE
 */
typedef struct mg_statem {
	ulong type;			/* object type */
	ulong id;			/* object id */
	ulong flags;			/* object flags */
	ulong state;			/* object state */
	/* followed by object-specific state structure */
} mg_statem_t;

#define	MG_IOCGSTATEM	_IOR(	MG_IOC_MAGIC,	6,	mg_statem_t	)
#define	MG_IOCCMRESET	_IOR(	MG_IOC_MAGIC,	7,	mg_statem_t	)

/*
 *  Media Gateway statistics
 *  -----------------------------------
 */
typedef struct mg_stats_mg {
} mg_stats_mg_t;

/*
 *  Multiplex statistics
 *  -----------------------------------
 */
typedef struct mg_stats_mx {
} mg_stats_mx_t;

/*
 *  Channel statistics
 *  -----------------------------------
 */
typedef struct mg_stats_ch {
} mg_stats_ch_t;

/*
 *  Connection Leg statistics
 *  -----------------------------------
 */
typedef struct mg_stats_lg {
} mg_stats_lg_t;

/*
 *  Communications Session statistics
 *  -----------------------------------
 */
typedef struct mg_stats_se {
} mg_stats_se_t;

/*
 *  Default statistics
 *  -----------------------------------
 */
typedef struct mg_stats_df {
} mg_stats_df_t;

/*
 *  STATISTICS
 */
typedef struct mg_stats {
	ulong type;			/* object type */
	ulong id;			/* object id */
	/* followed by object-specific statistics type */
} mg_stats_t;

#define	MG_IOCGSTATSP	_IOR(	MG_IOC_MAGIC,	 8,	mg_stats_t	)
#define	MG_IOCSSTATSP	_IOWR(	MG_IOC_MAGIC,	 9,	mg_stats_t	)
#define	MG_IOCGSTATS	_IOR(	MG_IOC_MAGIC,	10,	mg_stats_t	)
#define	MG_IOCCSTATS	_IOW(	MG_IOC_MAGIC,	11,	mg_stats_t	)

/*
 *  Media Gateway notifications
 *  -----------------------------------
 */
typedef struct mg_notify_mg {
	ulong events;
} mg_notify_mg_t;

/*
 *  Multiplex notifications
 *  -----------------------------------
 */
typedef struct mg_notify_mx {
	ulong events;
} mg_notify_mx_t;

/*
 *  Channel notifications
 *  -----------------------------------
 */
typedef struct mg_notify_ch {
	ulong events;
} mg_notify_ch_t;

/*
 *  Connection Leg notifications
 *  -----------------------------------
 */
typedef struct mg_notify_lg {
	ulong events;
} mg_notify_lg_t;

/*
 *  Communications Session notifications
 *  -----------------------------------
 */
typedef struct mg_notify_se {
	ulong events;
} mg_notify_se_t;

/*
 *  Default notifications
 *  -----------------------------------
 */
typedef struct mg_notify_df {
	ulong events;
} mg_notify_df_t;

/*
 *  EVENTS
 */
typedef struct mg_notify {
	ulong type;			/* object type */
	ulong id;			/* object id */
	/* followed by object-specific notification type */
} mg_notify_t;

#define	MG_IOCGNOTIFY	_IOR(	MG_IOC_MAGIC,	12,	mg_notify_t	)
#define	MG_IOCSNOTIFY	_IOW(	MG_IOC_MAGIC,	13,	mg_notify_t	)
#define	MG_IOCCNOTIFY	_IOW(	MG_IOC_MAGIC,	14,	mg_notify_t	)

/*
 *  MG MANAGEMENT
 */
typedef struct mg_mgmt {
	ulong type;			/* object type */
	ulong id;			/* object id */
	ulong cmd;			/* mgmt command */
} mg_mgmt_t;

#define MG_MGMT_BLOCK	    1
#define MG_MGMT_UNBLOCK	    2
#define MG_MGMT_RESET	    3
#define MG_MGMT_QUERY	    4

#define MG_IOCCMGMT	_IOWR(	MG_IOC_MAGIC,	15,	mg_mgmt_t	)

/*
 *  PASS LOWER
 */
typedef struct mg_pass {
	ulong muxid;			/* mux index of lower CH structure to pass message to */
	ulong type;			/* type of message block */
	ulong band;			/* band of message block */
	ulong ctl_length;		/* length of cntl part */
	ulong dat_length;		/* length of data part */
	/* followed by cntl and data part of message to pass to to channel */
} mg_pass_t;

#define MG_IOCCPASS	_IOWR(	MG_IOC_MAGIC,	16,	mg_pass_t	)

#define MG_IOC_FIRST	 0
#define MG_IOC_LAST	16
#define MG_IOC_PRIVATE	32

#endif				/* __SYS_MGI_IOCTL_H__ */
