/*****************************************************************************

 @(#) $RCSfile: stats.h,v $ $Name:  $($Revision: 1.1.1.3.4.6 $) $Date: 2008-04-29 08:33:16 $

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

 Last Modified $Date: 2008-04-29 08:33:16 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: stats.h,v $
 Revision 1.1.1.3.4.6  2008-04-29 08:33:16  brian
 - update headers for Affero release

 Revision 1.1.1.3.4.5  2007/08/14 10:47:10  brian
 - GPLv3 header update

 *****************************************************************************/

/*                               -*- Mode: C -*- 
 * stats.h --- streams statistics
 * Author          : Francisco J. Ballesteros, Graham Wheeler
 * Created On      : Tue May 31 21:40:37 1994
 * Last Modified By: David Grothe
 * RCS Id          : $Id: stats.h,v 1.1.1.3.4.6 2008-04-29 08:33:16 brian Exp $
 * Purpose         : provide statistics for LiS
 * ----------------______________________________________________
 *
 *   Copyright (C) 1995  Francisco J. Ballesteros, Graham Wheeler
 *   Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>
 *
 *    You can reach us by email to any of
 *    nemo@ordago.uc3m.es, gram@aztec.co.za
 */

#ifndef _STATS_H
#define _STATS_H 1

#ident "@(#) $RCSfile: stats.h,v $ $Name:  $($Revision: 1.1.1.3.4.6 $) $Date: 2008-04-29 08:33:16 $"

/*  -------------------------------------------------------------------  */
/*				 Dependencies                            */

/*  -------------------------------------------------------------------  */
/*				   Symbols                               */

/* accounted items for stats and their names
 */
#define HEADERS		0
#define FREEHDRS	1
#define DATABS		2
#define	MEMALLOCS	3	/* # times allocator called */
#define	MEMFREES	4	/* # times free called */
#define	MEMALLOCD	5	/* amount of memory */
#define	DBLKMEM		6	/* amt of mem allocated dblks */
#define	MSGSQD		7	/* # messages queued */
#define	MSGQDSTRHD	8	/* # msgs qd at strm hd */
#define	QUEUES		9	/* # queues allocated */
#define	CANPUTS		10	/* # canput calls */
#define	QSCHEDS		11	/* # queues scheduled to run */
#define	BUFCALLS	12	/* # bufcalls */
#define	MEMLIM		13	/* lis_max_mem */
#define	MSGMEMLIM	14	/* lis_max_msg_mem */
#define	MODUSE		15	/* really # open files */
#define	OPENTIME	16	/* file open time */
#define	CLOSETIME	17	/* file close time */
#define	READTIME	18	/* file read time */
#define	WRITETIME	19	/* file write time */
#define	IOCTLTIME	20	/* file ioctl time */
#define	GETMSGTIME	21	/* file getmsg time */
#define	PUTMSGTIME	22	/* file putmsg time */
#define	POLLTIME	23	/* poll time */
#define	LOCKCNTS	24	/* locks and contention */
#define	WRITECNT	25	/* write/putmsg */
#define	READCNT		26	/* read/getmsg */

#define	STRMAXSTAT	27	/* largest slot */
#define HEADERSSTR      "In-Use Message Blocks"
#define FREEHDRSSTR     "Free Message Blocks"
#define DATABSSTR       "Data Blocks"
#define	MEMALLOCSSTR	"Memory Allocator Calls"
#define	MEMFREESSSTR	"Memory Free Calls"
#define	MEMALLOCDSTR	"Bytes Allocated"
#define	DBLKMEMSTR	"Bytes Allocated to D-Blks"
#define	MSGSQDSTR	"Messages Queued"
#define	MSGQDSTRHDSTR	"Messages Queued at Strm Hd"
#define	QUEUESSTR	"Queues Allocated"
#define	CANPUTSSTR	"Canput Calls"
#define	QSCHEDSSTR	"Scheduled Queues"
#define	BUFCALLSSTR	"Bufcalls"
#define	MEMLIMSTR	"Memory Limit"
#define	MSGMEMLIMSTR	"D-Blk Memory Limit"
#define	MODUSESTR	"STREAMS Files Open"
#define	OPENTIMESTR	"Open Processing Time"
#define	CLOSETIMESTR	"Close Processing Time"
#define	READTIMESTR	"Read Processing Time"
#define	WRITETIMESTR	"Write Processing Time"
#define	IOCTLTIMESTR	"Ioctl Processing Time"
#define	GETMSGTIMESTR	"Getmsg Processing Time"
#define	PUTMSGTIMESTR	"Putmsg Processing Time"
#define	POLLTIMESTR	"Poll Processing Time"
#define LOCKCNTSSTR	"Spin Lock Counts"
#define	WRITESTR	"Write/Putmsg"
#define READSTR		"Read/Getmsg"

/* per item statistics & their names
 */
#define CURRENT		0
#define TOTAL		1
#define MAXIMUM		2
#define FAILURES	3
#define CURRENTSTR	"Current"
#define TOTALSTR	"Total"
#define MAXIMUMSTR	"Maximum"
#define FAILURESSTR	"Failures"

/*  -------------------------------------------------------------------  */
/*				    Types                                */

/*  -------------------------------------------------------------------  */
/*				 Glob. Vars.                             */

#ifdef __KERNEL__
#ifdef __LIS_INTERNAL__
extern lis_atomic_t lis_strstats[STRMAXSTAT][4];	/* the stats */
#endif
#endif				/* __KERNEL__ */

/* If in user mode, include a stats dump routine 
 */
#ifdef MEMPRINT

typedef struct {
	int stats_inx;			/* index into lis_strstats */
	char *name;			/* ASCII name */
} itemname_t;

#ifdef __LIS_INTERNAL__
extern itemname_t lis_itemnames[];
extern char *lis_countnames[];
#endif
#endif

/*  -------------------------------------------------------------------  */
/*			Exported functions & macros                      */
#ifdef __KERNEL__
#ifdef __LIS_INTERNAL__
/* increment count for one item
 */
extern void LisUpCounter(int item, int n);

/*
 * set item to a value
 */
extern void LisSetCounter(int item, int val);

extern void lis_stat_neg(void);
#endif

#define	LisUpCount(item)	LisUpCounter(item,1)

/* increment fail count for item
 */
#define LisUpFailCount(item)	K_ATOMIC_INC(&lis_strstats[item][FAILURES])

/* decrement count for item
 */
#define	LisChkNeg(item)	     do	{					     \
				  if (K_ATOMIC_READ(			     \
					  &lis_strstats[item][CURRENT]) < 0) \
				    lis_stat_neg() ;			     \
				} while (0)
#define LisDownCount(item)   do	{					     \
				  if (K_ATOMIC_READ(			     \
					  &lis_strstats[item][CURRENT]) != 0)\
				      K_ATOMIC_DEC(			     \
					  &lis_strstats[item][CURRENT]) ;    \
				  else					     \
				    lis_stat_neg() ;			     \
				} while (0)
#define LisDownCounter(item,n)	do {					     \
				  int	_n = (n) ;			     \
				  if (K_ATOMIC_READ(			     \
				       &lis_strstats[item][CURRENT]) >= _n)  \
				      K_ATOMIC_SUB(			     \
				       &lis_strstats[item][CURRENT], _n) ;   \
				  else					     \
				    lis_stat_neg() ;			     \
				} while (0)

#endif				/* __KERNEL__ */

/*  -------------------------------------------------------------------  */
#endif				/* !_STATS_H */

/*----------------------------------------------------------------------
# Local Variables:      ***
# change-log-default-name: "~/src/prj/streams/src/NOTES" ***
# End: ***
  ----------------------------------------------------------------------*/
