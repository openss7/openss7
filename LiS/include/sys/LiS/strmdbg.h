/************************************************************************
*                      STREAMS Debugging Aids                           *
*************************************************************************
*									*
* These are routines that print things and bits of a debug mask.	*
*									*
* The debug mask can be turned on by the user via an ioctl.  Bits	*
* of this mask control the printing of various messages.		*
*									*
* Some of the streams primitives have been aliased such that they	*
* pass in the caller's file name and line number to the routine that	*
* really does the work.  This allows us to keep pretty good track of	*
* memory usage.  Stray memory is always a nasty problem in debugging	*
* streams drivers.							*
*									*
* Author:	David Grothe <dave@gcom.com>				*
*									*
* Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>		*
*									*
************************************************************************/
/*
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
 */

#ifndef	_STRMDBG_H
#define	_STRMDBG_H		1

#ident "@(#) LiS strmdbg.h 2.25 09/07/04 11:11:22 "

#ifndef _HEAD_H
#include <sys/LiS/head.h>
#endif


/*
 * These bits can be set to cause the streams module to print
 * interesting stuff while it is executing.
 */
extern unsigned long	lis_debug_mask ;
extern unsigned long	lis_debug_mask2 ;

/*
 * Bits in lis_debug_mask
 */
#define LIS_DEBUG_OPEN_BIT		0x00000001L
#define LIS_DEBUG_CLOSE_BIT		0x00000002L
#define LIS_DEBUG_READ_BIT		0x00000004L
#define LIS_DEBUG_WRITE_BIT		0x00000008L

#define LIS_DEBUG_IOCTL_BIT		0x00000010L
#define LIS_DEBUG_PUTNEXT_BIT		0x00000020L
#define LIS_DEBUG_STRRPUT_BIT		0x00000040L
#define LIS_DEBUG_SIG_BIT		0x00000080L

#define LIS_DEBUG_PUTMSG_BIT		0x00000100L
#define LIS_DEBUG_GETMSG_BIT		0x00000200L
#define LIS_DEBUG_POLL_BIT		0x00000400L
#define LIS_DEBUG_LINK_BIT		0x00000800L

#define LIS_DEBUG_MEAS_TIME_BIT		0x00001000L
#define	LIS_DEBUG_MEM_LEAK_BIT		0x00002000L
#define	LIS_DEBUG_FLUSH_BIT		0x00004000L
#define	LIS_DEBUG_FATTACH_BIT		0x00008000L

#define LIS_DEBUG_SAFE_BIT		0x00010000L
#define LIS_DEBUG_TRCE_MSG_BIT		0x00020000L
#define LIS_DEBUG_CLEAN_MSG_BIT		0x00040000L
#define LIS_DEBUG_SPL_TRACE_BIT		0x00080000L

#define LIS_DEBUG_MP_ALLOC		0x00100000L	/* allocb debug */ 
#define LIS_DEBUG_MP_FREEMSG		0x00200000L	/* freemsg debug */ 
#define	LIS_DEBUG_MALLOC_BIT		0x00400000L	/* lis_malloc/free */
#define LIS_DEBUG_MONITOR_MEM_BIT	0x00800000L	/* monitor memory */

#define LIS_DEBUG_DMP_QUEUE_BIT		0x01000000L
#define LIS_DEBUG_DMP_MBLK_BIT		0x02000000L
#define LIS_DEBUG_DMP_DBLK_BIT		0x04000000L
#define LIS_DEBUG_DMP_STRHD_BIT		0x08000000L

#define LIS_DEBUG_VOPEN_BIT		0x10000000L	/* Verbose open */
#define LIS_DEBUG_VCLOSE_BIT		0x20000000L	/* Verbose close */
#define LIS_DEBUG_ADDRS_BIT		0x80000000L	/* print addrs */

/*
 * Bits in lis_debug_mask2
 */
#define LIS_DEBUG_SNDFD_BIT		0x00000001L
#define LIS_DEBUG_CP_BIT		0x00000002L	/* print code path */
#define LIS_DEBUG_CACHE_BIT		0x00000004L	/* cache allocs */
#define LIS_DEBUG_LOCK_CONTENTION_BIT	0x00000008L	/* lock contention */
#define LIS_DEBUG_REFCNTS_BIT		0x00000010L
#define LIS_DEBUG_SEMTIME_BIT		0x00000020L	/* sem wkup time */

/*
 * Shorthand macros
 */

#if 1

#define LIS_DEBUG_OPEN		(lis_debug_mask & LIS_DEBUG_OPEN_BIT)
#define LIS_DEBUG_CLOSE		(lis_debug_mask & LIS_DEBUG_CLOSE_BIT)
#define LIS_DEBUG_READ		(lis_debug_mask & LIS_DEBUG_READ_BIT)
#define LIS_DEBUG_WRITE		(lis_debug_mask & LIS_DEBUG_WRITE_BIT)

#define LIS_DEBUG_IOCTL		(lis_debug_mask & LIS_DEBUG_IOCTL_BIT)
#define LIS_DEBUG_PUTNEXT	(lis_debug_mask & LIS_DEBUG_PUTNEXT_BIT)
#define LIS_DEBUG_STRRPUT	(lis_debug_mask & LIS_DEBUG_STRRPUT_BIT)
#define LIS_DEBUG_SIG		(lis_debug_mask & LIS_DEBUG_SIG_BIT)

#define LIS_DEBUG_PUTMSG	(lis_debug_mask & LIS_DEBUG_PUTMSG_BIT)
#define LIS_DEBUG_GETMSG	(lis_debug_mask & LIS_DEBUG_GETMSG_BIT)
#define LIS_DEBUG_POLL		(lis_debug_mask & LIS_DEBUG_POLL_BIT)
#define LIS_DEBUG_LINK		(lis_debug_mask & LIS_DEBUG_LINK_BIT)

#define LIS_DEBUG_MEAS_TIME	(lis_debug_mask & LIS_DEBUG_MEAS_TIME_BIT)
#define LIS_DEBUG_MEM_LEAK	(lis_debug_mask & LIS_DEBUG_MEM_LEAK_BIT)
#define LIS_DEBUG_FLUSH		(lis_debug_mask & LIS_DEBUG_FLUSH_BIT)
#define LIS_DEBUG_FATTACH	(lis_debug_mask & LIS_DEBUG_FATTACH_BIT)

#define LIS_DEBUG_SAFE		(lis_debug_mask & LIS_DEBUG_SAFE_BIT)
#define LIS_DEBUG_TRCE_MSG	(lis_debug_mask & LIS_DEBUG_TRCE_MSG_BIT)
#define LIS_DEBUG_CLEAN_MSG	(lis_debug_mask & LIS_DEBUG_CLEAN_MSG_BIT)
#define LIS_DEBUG_SPL_TRACE	(lis_debug_mask & LIS_DEBUG_SPL_TRACE_BIT)

#define LIS_DEBUG_ALLOC 	(lis_debug_mask & LIS_DEBUG_MP_ALLOC)
#define LIS_DEBUG_FREEMSG 	(lis_debug_mask & LIS_DEBUG_MP_FREEMSG)
#define LIS_DEBUG_MALLOC	(lis_debug_mask & LIS_DEBUG_MALLOC_BIT)
#define LIS_DEBUG_MONITOR_MEM	(lis_debug_mask & LIS_DEBUG_MONITOR_MEM_BIT)

#define LIS_DEBUG_DMP_QUEUE	(lis_debug_mask & LIS_DEBUG_DMP_QUEUE_BIT)
#define LIS_DEBUG_DMP_MBLK	(lis_debug_mask & LIS_DEBUG_DMP_MBLK_BIT)
#define LIS_DEBUG_DMP_DBLK	(lis_debug_mask & LIS_DEBUG_DMP_DBLK_BIT)
#define LIS_DEBUG_DMP_STRHD	(lis_debug_mask & LIS_DEBUG_DMP_STRHD_BIT)

#define LIS_DEBUG_VOPEN		(lis_debug_mask & LIS_DEBUG_VOPEN_BIT)
#define LIS_DEBUG_VCLOSE	(lis_debug_mask & LIS_DEBUG_VCLOSE_BIT)
#define LIS_DEBUG_ADDRS		(lis_debug_mask & LIS_DEBUG_ADDRS_BIT)


#define LIS_DEBUG_SNDFD		(lis_debug_mask2 & LIS_DEBUG_SNDFD_BIT)
#define LIS_DEBUG_CP		(lis_debug_mask2 & LIS_DEBUG_CP_BIT)
#define LIS_DEBUG_CACHE		(lis_debug_mask2 & LIS_DEBUG_CACHE_BIT)
#define LIS_DEBUG_LOCK_CONTENTION \
			    (lis_debug_mask2 & LIS_DEBUG_LOCK_CONTENTION_BIT)
#define LIS_DEBUG_REFCNTS	(lis_debug_mask2 & LIS_DEBUG_REFCNTS_BIT)
#define LIS_DEBUG_SEMTIME	(lis_debug_mask2 & LIS_DEBUG_SEMTIME_BIT)

#else /* !CONFIG_DEV */

#define LIS_DEBUG_OPEN		0
#define LIS_DEBUG_CLOSE		0
#define LIS_DEBUG_READ		0
#define LIS_DEBUG_WRITE		0

#define LIS_DEBUG_IOCTL		0
#define LIS_DEBUG_PUTNEXT	0
#define LIS_DEBUG_STRRPUT	0
#define LIS_DEBUG_SIG		0

#define LIS_DEBUG_PUTMSG	0
#define LIS_DEBUG_GETMSG	0
#define LIS_DEBUG_POLL		0
#define LIS_DEBUG_LINK		0

#define LIS_DEBUG_MEAS_TIME	0
#define LIS_DEBUG_MEM_LEAK	0
#define LIS_DEBUG_FLUSH		0
#define LIS_DEBUG_FATTACH	0

#define LIS_DEBUG_SAFE		0
#define LIS_DEBUG_TRCE_MSG	0
#define LIS_DEBUG_CLEAN_MSG	0
#define LIS_DEBUG_SPL_TRACE	0

#define LIS_DEBUG_ALLOC 	0
#define LIS_DEBUG_FREEMSG 	0
#define LIS_DEBUG_MALLOC	0
#define LIS_DEBUG_MONITOR_MEM	0

#define LIS_DEBUG_DMP_QUEUE	0
#define LIS_DEBUG_DMP_MBLK	0
#define LIS_DEBUG_DMP_DBLK	0
#define LIS_DEBUG_DMP_STRHD	0

#define LIS_DEBUG_VOPEN		0
#define LIS_DEBUG_VCLOSE	0
#define LIS_DEBUG_ADDRS		0


#define LIS_DEBUG_SNDFD		0
#define LIS_DEBUG_CP		0
#define LIS_DEBUG_CACHE		0
#define LIS_DEBUG_LOCK_CONTENTION \
			    (lis_debug_mask2 & LIS_DEBUG_LOCK_CONTENTION_BIT)
#define LIS_DEBUG_SEMTIME	(lis_debug_mask2 & LIS_DEBUG_SEMTIME_BIT)

#define LIS_DEBUG_REFCNTS	0

#endif /* !CONFIG_DEV */

/*
 * Some routines to assist in debug printing
 */
#ifdef __KERNEL__
void		 lis_print_block(void *ptr) _RP;
void		 lis_print_mem(void) _RP;
void		 lis_print_queue(queue_t * q) _RP;
void		 lis_print_queues(void) _RP;
void		 lis_print_stream(stdata_t *head) _RP;
const char	*lis_strm_name(stdata_t *head) _RP;
const char      *lis_strm_name_from_queue(queue_t *q) _RP;
const char	*lis_queue_name(queue_t *q) _RP;
const char	*lis_maj_min_name(stdata_t *head) _RP;
const char	*lis_msg_type_name(mblk_t *mp) _RP;
void		 lis_print_data(mblk_t *mp, int opt, int cont) _RP;
void		 lis_print_msg(mblk_t *mp, const char *prefix, int opt) _RP;
char		*lis_poll_events(short events) _RP;
#endif				/* __KERNEL__ */

/*  -------------------------------------------------------------------  */
/*                             print_msg options			 */

#define	PRINT_DATA_SHORT	0		/* short form		 */
#define	PRINT_DATA_ENTIRE	1		/* print entire dblk	 */
#define	PRINT_DATA_RDWR		2		/* print just rptr->wptr */

/*  -------------------------------------------------------------------  */
/*                             Memory Allocation			 */
/*
 * Note that the lis memory allocator is located in strmdbg.c so that
 * it can maintain the secret memory links for all streams-allocated
 * memory.
 */
#ifdef __KERNEL__
void	*lis_malloc(int nbytes, int class, int use_cache, 
			char *file_name, int line_nr) _RP;
void	*lis_zmalloc(int nbytes, int class, char *file_name, int line_nr) _RP;
void	 lis_free(void *ptr, char *file_name, int line_nr) _RP;
void	 lis_mark_mem_fcn(void *ptr, const char *file_name, int line_nr) _RP;
#if defined(CONFIG_DEV)
#define	lis_mark_mem	lis_mark_mem_fcn
#else
#define	lis_mark_mem(a, b, c)
#endif
int      lis_check_guard(void *ptr, char *msg) ;
int      lis_check_mem(void) ;
void     lis_terminate_mem(void) ;

#if defined(LINUX)
static inline const char *lis_basename( const char *filename )
{
    char        *p ;

    p = strrchr(filename, '/') ;
    if (p == NULL)
	return(filename) ;
    return(p+1) ;
}
#else
const char *lis_basename(const char *filename);
#endif

#define __LIS_FILE__  lis_basename(__FILE__)

#else

#define __LIS_FILE__  __FILE__

#endif				/* __KERNEL__ */

/*  -------------------------------------------------------------------  */
/*				Memory Codes				 */

#define	MEM_QUEUE	-1		/* it's a queue */
#define	MEM_MSG		-2		/* it's a message (mblk) */
#define	MEM_STRMHD	-3		/* it's a stream head (stdata) */
#define	MEM_TIMER	-4		/* it's a timer structure */


/*
 * Guard word value
 */
#define	MEM_GUARD	0x1234abcd	/* unique pattern at end of mem area */


/*  -------------------------------------------------------------------  */
/*				Streams Statistics			 */

/*  -------------------------------------------------------------------  */
/*                             Print Buffer				 */

#define	LIS_PRINT_BUFFER_SIZE		50000	/* large-ish buffer */
extern void	lis_bprintf(char *fmt, ...) ;	/* print into buffer */
extern void	lis_flush_print_buffer(void) ;	/* print out the buffer */

#endif
