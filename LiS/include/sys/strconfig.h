/*****************************************************************************

 @(#) $RCSfile: strconfig.h,v $ $Name:  $($Revision: 1.1.1.3.4.7 $) $Date: 2008-04-29 08:33:15 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

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

 Last Modified $Date: 2008-04-29 08:33:15 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: strconfig.h,v $
 Revision 1.1.1.3.4.7  2008-04-29 08:33:15  brian
 - update headers for Affero release

 Revision 1.1.1.3.4.6  2007/08/14 10:47:08  brian
 - GPLv3 header update

 *****************************************************************************/

/*                               -*- Mode: C -*- 
 * strconfig.h --- configuration symbols for LiS
 * Author          : Francisco J. Ballesteros
 * Created On      : Tue May 31 21:40:37 1994
 * Last Modified By: David Grothe
 * RCS Id          : $Id: strconfig.h,v 1.1.1.3.4.7 2008-04-29 08:33:15 brian Exp $
 * Purpose         : just document symbols
 * ----------------______________________________________________
 *    Copyright (C) 1995  Graham Wheeler, Francisco J. Ballesteros
 *    Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>
 *
 *    You can reach us by email to any of
 *    gram@aztec.co.za, nemo@ordago.uc3m.es
 */

#ifndef _LIS_CONFIG_H
#define _LIS_CONFIG_H 1

#ident "@(#) $RCSfile: strconfig.h,v $ $Name:  $($Revision: 1.1.1.3.4.7 $) $Date: 2008-04-29 08:33:15 $"

/*  -------------------------------------------------------------------  */
/*				 Dependencies                            */

#ifdef LINUX
#include <linux/major.h>	/* for MAX_CHRDEV */
#else
#define	MAX_CHRDEV	256
#endif

/*  -------------------------------------------------------------------  */
/*				   Symbols                               */

/*
 * The dimension of LiS structures that exist for each CPU.
 */
#define	LIS_NR_CPUS		64

/*  -------------------------------------------------------------------  */
/* This is only for documentation purposes
 */

#define SHARE			/* you can RW */
#define EXPORT			/* you can R */
#define PRIVATE			/* you can't */

/*  -------------------------------------------------------------------  */
/*		       Global tunable vars. and symbols                  */

/*
 * Maximum number of mblk headers to keep on hand for fast allocation
 */
#define	MAX_MBLKS	10

/* From mod.h:
 * (we've been using a char for the module id, so if the # of modules
 * get's over 255 we should define  mid_t and use this.
 */
#ifdef USE_OLD_CONSTS
#define FMNAMESZ        16	/* Max module name size */
#else
#define FMNAMESZ        8	/* compatibility with Solaris and UnixWare */
#endif
#define	LIS_NAMESZ	64	/* LiS internal name length */

#ifdef KERNEL_2_5		/* 2.5/2.6 kernel */
#define MAX_STRDEV	1024	/* Max # of stream devices */
#define MAX_STRMOD	256	/* Max # of stream modules */
#else
#define MAX_STRDEV	MAX_CHRDEV	/* Max # of stream devices */
#define MAX_STRMOD	MAX_CHRDEV	/* Max # of stream modules */
#endif
#define MAX_STRAMOD	8	/* max # of autopushed mods per str */
#define MAX_APUSH       8	/* max # of autopushed mods */
#ifdef __KERNEL__
#ifdef __LIS_INTERNAL__
extern int lis_reuse_modsw;		/* we reuse modsw entries if this is true */
#endif
#endif				/* __KERNEL__ */

/* From queue.h:
 */
#define NBAND   256		/* Max # of bands. Keep below 256 */

/* From stream.c: 
 */
#define LIS_CLTIME   15000	/* # of milli-secs. to wait on close */
#define LIS_RTIME    1		/* # of secs. to wait to fwd held msg */
#define LIS_MAXPSZ   4096	/* max. packet size (must be non-zero) */
#define LIS_MINPSZ   0		/* min. packet size */

#define LIS_MEMLIMIT	(1*1024*1024)	/* STREAMS memory limit */

#ifdef __KERNEL__
#ifdef __LIS_INTERNAL__
extern unsigned long lis_strthresh;	/* configurable STREAMS memory limit */
#endif
#ifdef __LIS_INTERNAL__
extern int lis_nstrpush;		/* maximum # of pushed modules */
#endif
#ifdef __LIS_INTERNAL__
extern int lis_strhold;			/* if not zero str hold feature's activated */
#endif
#endif				/* __KERNEL__ */

/* From msg.h:
 */
#ifdef __KERNEL__
#ifdef __LIS_INTERNAL__
extern int lis_strmsgsz;		/* maximum stream message size */
#endif
#endif				/* __KERNEL__ */

#endif				/* !_LIS_CONFIG_H */

/*----------------------------------------------------------------------
# Local Variables:      ***
# change-log-default-name: "~/src/prj/streams/src/NOTES" ***
# End: ***
  ----------------------------------------------------------------------*/
