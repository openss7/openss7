/*****************************************************************************

 @(#) $Id$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 3 of the License.

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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

/*                               -*- Mode: C -*- 
 * msg.h --- streams message handling
 * Author          : Graham Wheeler
 * Created On      : Tue May 31 22:25:19 1994
 * Last Modified By: David Grothe
 * RCS Id          : $Id: msg.h,v 1.1.1.4.4.4 2005/12/18 05:41:24 brian Exp $
 * Purpose         : provide streams message handling
 *                 : this is the main memory allocation related module.
 * ----------------______________________________________________
 *
 *   Copyright (C) 1995  Graham Wheeler
 *   Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>
 *
 *    You can reach me by email to
 *    gram@aztec.co.za
 */

#ifndef _MSG_H
#define _MSG_H 1

#ident "@(#) $RCSfile: msg.h,v $ $Name:  $($Revision: 1.1.1.4.4.4 $) $Date: 2005/12/18 05:41:24 $"

/*  -------------------------------------------------------------------  */
/*				 Dependencies                            */

#ifndef _SYS_TYPES_H
# if	defined(LINUX)
#   include <linux/types.h>
# else
#   include <sys/types.h>
# endif
#endif
#ifndef _SHARE_H
#include <sys/LiS/share.h>
#endif
#ifndef _LIS_CONFIG_H
#include <sys/strconfig.h>	/* config definitions */
#endif
#include <sys/LiS/genconf.h>

/*
 * The memory allocation mechanism is based on that in SVR4.2.
 * That is, all memory is allocated dynamically, with freed
 * message headers being held on a free list. When kernel memory
 * is low some of these can be reclaimed by calling strgiveback.
 *
 * Message headers are 128 bytes in size. The extra space
 * is used as the data buffer for smallish messages. This
 * scheme means that in most cases, a call to allocb just
 * requires unlinking a message header from the free list
 * and initialising it.
 *
 * This scheme does add some complexity, however, with
 * regard to dupb/dupmsg. In this case the duplicate can
 * have pointers to a data buffer within some other message
 * header. Thus, if a message header is freed, we have to
 * check if its internal data buffer is still in use by
 * someone else, in which case we defer freeing the header;
 * on the other hand, if we are freeing the last reference
 * to some other data buffer in a message header, we have two
 * headers to free. All of this logic is nicely hidden in freeb()
 * (with a little bit of it leaking into pullupmsg).
 *
 * NB: This does rely on the fact that if a message block
 * has a data buffer of FASTBUF or less in size and no special
 * free function (i.e. it wasn't an esballoc), then that data
 * buffer lives internally within some (not necessarily the same)
 * message header, and was *not* allocated elsewhere.
 *
 * To put it another way, if you don't completely understand
 * the memory management scheme, don't fiddle with any of
 * the following code, and don't ever directly modify data
 * block elements like db_base, db_lim and db_size.
 */

/*  -------------------------------------------------------------------  */
/*				   Symbols                               */

/* size of header and fastbuf data 
 */
#define HDRSZ	(sizeof(struct mbinfo)+sizeof(struct dbinfo))
#define FASTBUF	(128-HDRSZ)	/* space remaining for data */

/* Code for M_ERROR msg 
 */
#define NOERROR ((unsigned char)-1)	/* ?? */

/*  -------------------------------------------------------------------  */
/* Priority for block allocs (ignored)
 */
#define BPRI_HI		0
#define BPRI_MED	1
#define BPRI_LO		2

/* infinite packet size
 */
#ifndef INFPSZ
#define INFPSZ  (-1)
#endif

/*  -------------------------------------------------------------------  */
/*				    Types                                */

/* This is a message block. Messages are lists of blocks
 * It points to a data block which enable us to share data
 * in msgs.
 */

typedef struct msgb {
	struct msgb *b_next;		/* next msg on queue */
	struct msgb *b_prev;		/* prev msg on queue */
	struct msgb *b_cont;		/* next blk of msg */
	unsigned char *b_rptr;		/* 1st unread byte */
	unsigned char *b_wptr;		/* 1st unwriten byte */
	struct datab *b_datap;		/* pointer to data */
	unsigned char b_band;		/* message priority */
	unsigned char b_pad1;
	/* implementation specific */
	unsigned short b_flag;		/* see below */
	long b_pad2;
} msgb_t;

typedef msgb_t mblk_t;

/*
 * Msg flags 
 */
#define	MSGMARK		0x01	/* last byte of message is "marked" */
#define	MSGNOLOOP	0x02	/* stream head won't loop around to write q */
#define	MSGDELIM	0x04	/* message is delimited */

/*
 * This structure is used in calls to esballoc
 */

typedef struct free_rtn {
	void _RP(*free_func) (char *);	/* the free() function */
	char *free_arg;			/* argument */
} frtn_t;

/* This is the data block. It stores data for a message block
 */
typedef struct datab {
	struct free_rtn *frtnp;		/* for SVR4 compatibility */
	unsigned char *db_base;
	unsigned char *db_lim;
	volatile
	unsigned char db_ref;
	unsigned char db_type;		/* QNORM or QPCTL */
	/* implementation specific */
	unsigned int db_size;
	frtn_t db_rtn;
} datab_t;

typedef datab_t dblk_t;

/* Structures for the message headers
 */

struct mbinfo {
	mblk_t m_mblock;
	void (*m_func) (void);
};

struct dbinfo {
	dblk_t d_dblock;
};

struct mdbblock {
	struct mbinfo msgblk;		/* message block header info */
	struct dbinfo datblk;		/* data block header info */
	char databuf[FASTBUF];		/* internal small data buffer */
};

/*  -------------------------------------------------------------------  */
/*				 Glob. Vars.                             */
#ifdef __KERNEL__

#ifdef __LIS_INTERNAL__
extern volatile struct mdbblock *lis_mdbfreelist;	/* msg block free list */
#endif

#endif				/* __KERNEL__ */
/*  -------------------------------------------------------------------  */
/*			Exported functions & macros                      */

#ifdef __KERNEL__

/* lis_strgiveback - return some free headers to system heap
 */
#ifdef __LIS_INTERNAL__
extern void lis_strgiveback(unsigned long arg);
#endif

#if defined(LINUX)
#ifdef __LIS_INTERNAL__
extern void lis_init_msg(void);
#endif
#endif

/*  lis_terminate_msg - do the final shutdown of the msg memory subsystem
 */
#if !(defined(LINUX) && defined(USE_KMEM_CACHE))
#ifdef __LIS_INTERNAL__
extern void lis_terminate_msg(void);
#endif
#endif

/* allocb: allocate an M_DATA message block of the specified
 *	size. The priority is for compatibility only.
 *
 * allocb_physreq: allocate M_DATA in a memory block with specific
 *      physical characteristics.
 */
struct msgb *_RP lis_allocb(int size, unsigned int priority, char *file_name, int line_nr);
struct msgb *_RP lis_allocb_physreq(int size, unsigned int priority, void *physreq_ptr,
				    char *file_name, int line_nr);

/* testb: see if an allocation can actually be done.
 *
 */
extern int _RP lis_testb(int size, unsigned int priority);

/*
 * esballoc: allocate a message block, using a user-provided data buffer
 *
 */
extern mblk_t *_RP lis_esballoc(unsigned char *base, int size, int priority, frtn_t *freeinfo,
				char *file_name, int line_nr);

/* freeb - Free data buffer and place message block on free list. Don't
 *      follow the continuation pointer.
 *
 */
extern void _RP lis_freeb(mblk_t *bp);

#ifdef __LIS_INTERNAL__
extern void lis_freedb(mblk_t *bp, int free_hdr);
#endif

/* freemsg - free a whole message
 *
 */
void _RP lis_freemsg(mblk_t *mp);

#endif				/* __KERNEL__ */

/*  -------------------------------------------------------------------  */
#endif				/* !_MSG_H */

/*----------------------------------------------------------------------
# Local Variables:      ***
# change-log-default-name: "~/src/prj/streams/src/NOTES" ***
# End: ***
  ----------------------------------------------------------------------*/
