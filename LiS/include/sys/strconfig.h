/*                               -*- Mode: C -*- 
 * strconfig.h --- configuration symbols for LiS
 * Author          : Francisco J. Ballesteros
 * Created On      : Tue May 31 21:40:37 1994
 * Last Modified By: David Grothe
 * RCS Id          : $Id: strconfig.h,v 1.3 1996/01/07 20:48:32 dave Exp $
 * Purpose         : just document symbols
 * ----------------______________________________________________
 *    Copyright (C) 1995  Graham Wheeler, Francisco J. Ballesteros
 *    Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Cambridge,
 * MA 02139, USA.
 * 
 *
 *    You can reach us by email to any of
 *    gram@aztec.co.za, nemo@ordago.uc3m.es
 */


#ifndef _LIS_CONFIG_H
#define _LIS_CONFIG_H 1

#ident "@(#) LiS strconfig.h 2.3 2/8/01 21:24:53 "

/*  -------------------------------------------------------------------  */
/*				 Dependencies                            */

#ifdef LINUX
#include <linux/major.h>		/* for MAX_CHRDEV */
#else
#define	MAX_CHRDEV	256
#endif


/*  -------------------------------------------------------------------  */
/*				   Symbols                               */



/* this is to use safe version of streams api, to get nice warnings and so
 * on...
 */
#ifdef LIS_CONFIG_SAFE
#define SAFE 1
#endif

/* This is to use a test version
 */
#if defined(LIS_TESTING) || defined(LIS_DEBUG)
#define TEST 1
#endif

/*
 * The dimension of LiS structures that exist for each CPU.
 */
#define	LIS_NR_CPUS		64

/*  -------------------------------------------------------------------  */
/* This is only for documentation purposes
 */

#define SHARE          /* you can RW */
#define EXPORT         /* you can R */
#define PRIVATE        /* you can't */

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

#define MAX_STRDEV	MAX_CHRDEV /*Max # of stream devices */
#define MAX_STRMOD	MAX_CHRDEV /* Max # of stream modules */
#define MAX_STRAMOD	8	/* max # of autopushed mods per str */
#define MAX_APUSH       8	/* max # of autopushed mods */
#ifdef __KERNEL__
extern int lis_reuse_modsw;	/* we reuse modsw entries if this is true */
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

#define LIS_MEMLIMIT	(1*1024*1024) /*  STREAMS memory limit */

#ifdef __KERNEL__
extern unsigned long lis_strthresh;	/* configurable STREAMS memory limit */
extern int lis_nstrpush;	/* maximum # of pushed modules */
extern int lis_strhold;		/* if not zero str hold feature's activated*/
#endif				/* __KERNEL__ */

/* From msg.h:
 */
#ifdef __KERNEL__
extern int lis_strmsgsz;		/* maximum stream message size */
#endif				/* __KERNEL__ */


#endif /*!_LIS_CONFIG_H*/

/*----------------------------------------------------------------------
# Local Variables:      ***
# change-log-default-name: "~/src/prj/streams/src/NOTES" ***
# End: ***
  ----------------------------------------------------------------------*/
