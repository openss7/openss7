/*****************************************************************************

 @(#) $Id: lmi_ioctl.h,v 0.9.2.4 2007/06/17 01:56:01 brian Exp $

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

 Last Modified $Date: 2007/06/17 01:56:01 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: lmi_ioctl.h,v $
 Revision 0.9.2.4  2007/06/17 01:56:01  brian
 - updates for release, remove any later language

 *****************************************************************************/

#ifndef __LMI_IOCTL_H__
#define __LMI_IOCTL_H__

#ident "@(#) $RCSfile: lmi_ioctl.h,v $ $Name:  $($Revision: 0.9.2.4 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* This file can be processed by doxygen(1). */

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

#define LMI_IOC_MAGIC 'L'

/*
 *  PROTOCOL VARIANT AND OPTIONS 
 */

#define LMI_IOCGOPTIONS	_IOR(  LMI_IOC_MAGIC, 0, lmi_option_t )
#define LMI_IOCSOPTIONS	_IOW(  LMI_IOC_MAGIC, 1, lmi_option_t )

typedef struct lmi_option {
	lmi_ulong pvar;
	lmi_ulong popt;
} lmi_option_t;

#define SS7_PVAR_ITUT	0x0100
#define SS7_PVAR_ETSI	0x0200
#define SS7_PVAR_ANSI	0x0300
#define SS7_PVAR_JTTC	0x0400
#define SS7_PVAR_CHIN	0x0500
#define SS7_PVAR_SING	0x0600
#define SS7_PVAR_SPAN	0x0700
#define SS7_PVAR_MASK	0xff00

#define SS7_PVAR_88	0x0000
#define SS7_PVAR_92	0x0001
#define SS7_PVAR_96	0x0002
#define SS7_PVAR_00	0x0003
#define SS7_PVAR_YR	0x00ff

enum {
	SS7_PVAR_ITUT_88 = SS7_PVAR_ITUT | SS7_PVAR_88,	/* ITU-T protocol variant */
	SS7_PVAR_ITUT_93 = SS7_PVAR_ITUT | SS7_PVAR_92,	/* ITU-T protocol variant */
	SS7_PVAR_ITUT_96 = SS7_PVAR_ITUT | SS7_PVAR_96,	/* ITU-T protocol variant */
	SS7_PVAR_ITUT_00 = SS7_PVAR_ITUT | SS7_PVAR_00,	/* ITU-T protocol variant */
	SS7_PVAR_ETSI_88 = SS7_PVAR_ETSI | SS7_PVAR_88,	/* ETSI protocol variant */
	SS7_PVAR_ETSI_93 = SS7_PVAR_ETSI | SS7_PVAR_92,	/* ETSI protocol variant */
	SS7_PVAR_ETSI_96 = SS7_PVAR_ETSI | SS7_PVAR_96,	/* ETSI protocol variant */
	SS7_PVAR_ETSI_00 = SS7_PVAR_ETSI | SS7_PVAR_00,	/* ETSI protocol variant */
	SS7_PVAR_ANSI_88 = SS7_PVAR_ANSI | SS7_PVAR_88,	/* ANSI protocol variant */
	SS7_PVAR_ANSI_92 = SS7_PVAR_ANSI | SS7_PVAR_92,	/* ANSI protocol variant */
	SS7_PVAR_ANSI_96 = SS7_PVAR_ANSI | SS7_PVAR_96,	/* ANSI protocol variant */
	SS7_PVAR_ANSI_00 = SS7_PVAR_ANSI | SS7_PVAR_00,	/* ANSI protocol variant */
	SS7_PVAR_JTTC_94 = SS7_PVAR_JTTC | SS7_PVAR_92,	/* JTTC protocol variant */
	SS7_PVAR_CHIN_00 = SS7_PVAR_CHIN | SS7_PVAR_00,	/* CHIN protocol variant */
};

enum {
	SS7_POPT_MPLEV = 0x00000001,	/* multiple priority/congestion levels */
	SS7_POPT_PCR = 0x00000002,	/* preventative cyclic retranlmission */
	SS7_POPT_HSL = 0x00000004,	/* high speed links */
	SS7_POPT_XSN = 0x00000008,	/* extended sequence numbers */
	SS7_POPT_NOPR = 0x00000010,	/* no proving */
	SS7_POPT_ALL = 0x0000001f	/* all options */
};

/*
 *  CONFIGURATION
 */
typedef struct lmi_config {
	lmi_ulong version;
	lmi_ulong style;
} lmi_config_t;

#define LMI_IOCGCONFIG	_IOWR( LMI_IOC_MAGIC, 2, lmi_config_t )
#define LMI_IOCSCONFIG	_IOWR( LMI_IOC_MAGIC, 3, lmi_config_t )
#define LMI_IOCTCONFIG	_IOWR( LMI_IOC_MAGIC, 4, lmi_config_t )
#define LMI_IOCCCONFIG	_IOWR( LMI_IOC_MAGIC, 5, lmi_config_t )

/*
 *  STATE
 */
typedef struct lmi_statem {
	lmi_ulong state;
} lmi_statem_t;

#define LMI_IOCGSTATEM	_IOR(  LMI_IOC_MAGIC, 6, lmi_statem_t )
#define LMI_IOCCMRESET	_IOR(  LMI_IOC_MAGIC, 7, lmi_statem_t )

/*
 *  STATISTICS
 */

typedef struct lmi_sta {
	lmi_ulong object_id;		/* object id for stats */
	lmi_ulong colperiod;		/* collection period */
	lmi_ulong timestamp;		/* timestamp end of period */
} lmi_sta_t;

#define LMI_IOCGSTATSP	_IOR(  LMI_IOC_MAGIC,  8, lmi_sta_t )
#define LMI_IOCSSTATSP	_IOWR( LMI_IOC_MAGIC,  9, lmi_sta_t )

typedef struct lmi_stats {
} lmi_stats_t;

#define LMI_IOCGSTATS	_IOR(  LMI_IOC_MAGIC, 10, lmi_stats_t )
#define LMI_IOCCSTATS	_IOW(  LMI_IOC_MAGIC, 11, lmi_stats_t )

/*
 *  NOTIFICATIONS
 */

typedef struct lmi_notify {
	lmi_ulong events;
} lmi_notify_t;

#define LMI_IOCGNOTIFY	_IOR(  LMI_IOC_MAGIC, 12, lmi_notify_t )
#define LMI_IOCSNOTIFY	_IOW(  LMI_IOC_MAGIC, 13, lmi_notify_t )
#define LMI_IOCCNOTIFY	_IOW(  LMI_IOC_MAGIC, 14, lmi_notify_t )

/*
 *  COMMON IOCTL NUMBERS
 */

#define LMI_IOC_FIRST		 0
#define LMI_IOC_LAST		14
#define LMI_IOC_PRIVATE		32

#endif				/* __LMI_IOCTL_H__ */
