/*                               -*- Mode: C -*- 
 * msg.h --- streams message handling
 * Author          : Graham Wheeler
 * Created On      : Tue May 31 22:25:19 1994
 * Last Modified By: David Grothe
 * RCS Id          : $Id: msg.h,v 1.3 1996/01/27 00:40:27 dave Exp $
 * Purpose         : provide streams message handling
 *                 : this is the main memory allocation related module.
 * ----------------______________________________________________
 *
 *   Copyright (C) 1995  Graham Wheeler
 *   Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>
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
 *    You can reach me by email to
 *    gram@aztec.co.za
 */


#ifndef _MSG_H
#define _MSG_H 1

#ident "@(#) LiS msg.h 2.3 3/16/01 19:52:55 "

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
#define FASTBUF	(128-HDRSZ)		  /* space remaining for data	*/


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

typedef
struct  msgb {
  SHARE    
        struct  msgb    *b_next; /* next msg on queue */
        struct  msgb    *b_prev; /* prev msg on queue */
        struct  msgb    *b_cont; /* next blk of msg */
        unsigned char   *b_rptr; /* 1st unread byte */
        unsigned char   *b_wptr; /* 1st unwriten byte */
  EXPORT
        struct datab    *b_datap; /* pointer to data */
        unsigned char	 b_band;  /* message priority */
        unsigned char	 b_pad1;
  PRIVATE
	unsigned short	 b_flag;  /* see below */
        long         	 b_pad2;
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

typedef
struct free_rtn {
  SHARE
        void (*free_func)(char *);      /* the free() function */
        char *free_arg;                 /* argument */
} frtn_t;

/* This is the data block. It stores data for a message block
 */
typedef
struct datab {
  SHARE
        struct  free_rtn* frtnp;	/* for SVR4 compatibility */
  EXPORT
        unsigned char   *db_base;
        unsigned char   *db_lim;
        unsigned char   db_ref;
	unsigned char	db_type; /* QNORM or QPCTL */
  PRIVATE
        unsigned int    db_size;
	frtn_t		db_rtn ;
} datab_t;

typedef datab_t dblk_t;


/* Structures for the message headers
 */

struct mbinfo{
    mblk_t	m_mblock;
    void	(*m_func)(void);
};

struct dbinfo{
    dblk_t	d_dblock;
};

struct mdbblock{
    struct mbinfo	msgblk;		  /* message block header info	*/
    struct dbinfo	datblk;		  /* data block header info	*/
    char   		databuf[FASTBUF]; /* internal small data buffer */
};

/*  -------------------------------------------------------------------  */
/*				 Glob. Vars.                             */
#ifdef __KERNEL__

extern volatile struct mdbblock  *lis_mdbfreelist; /* msg block free list */

#endif /* __KERNEL__ */
/*  -------------------------------------------------------------------  */
/*			Exported functions & macros                      */

#ifdef __KERNEL__

/* lis_strgiveback - return some free headers to system heap
 */
extern void
lis_strgiveback(unsigned long arg);

/*  lis_terminate_msg - do the final shutdown of the msg memory subsystem
 */
extern void
lis_terminate_msg(void);

/* allocb: allocate an M_DATA message block of the specified
 *	size. The priority is for compatibility only.
 *
 * allocb_physreq: allocate M_DATA in a memory block with specific
 *      physical characteristics.
 */
struct msgb *
lis_allocb(int size, unsigned int priority, char *file_name, int line_nr);
struct msgb *
lis_allocb_physreq(int size, unsigned int priority, void *physreq_ptr,
		   char *file_name, int line_nr);

/* testb: see if an allocation can actually be done.
 *
 */
extern int
lis_testb(int size, unsigned int priority);

/*
 * esballoc: allocate a message block, using a user-provided data buffer
 *
 */
extern mblk_t *
lis_esballoc(unsigned char *base, int size, int priority,
	     frtn_t *freeinfo, char *file_name, int line_nr);

/* freeb - Free data buffer and place message block on free list. Don't
 *      follow the continuation pointer.
 *
 */
extern void lis_freeb(mblk_t *bp);
extern void lis_freedb(mblk_t *bp, int free_hdr);

/* freemsg - free a whole message
 *
 */
void
lis_freemsg(mblk_t *mp);

#endif				/* __KERNEL__ */

/*  -------------------------------------------------------------------  */
#endif /*!_MSG_H*/

/*----------------------------------------------------------------------
# Local Variables:      ***
# change-log-default-name: "~/src/prj/streams/src/NOTES" ***
# End: ***
  ----------------------------------------------------------------------*/
