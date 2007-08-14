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
 * msgutl.c --- streams message utilities.
 * Author          : Graham Wheeler
 * Created On      : Tue May 31 22:25:19 1994
 * Last Modified By: David Grothe
 * RCS Id          : $Id: msgutl.h,v 1.1.1.2.4.3 2005/12/18 05:41:24 brian Exp $
 * Purpose         : here you have utilites to handle str messages.
 *                 : 
 * ----------------______________________________________________
 *
 *   Copyright (C) 1995  Graham Wheeler
 *   Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>
 *
 *    You can reach me by email to
 *    gram@aztec.co.za
 */

#ifndef _MSGUTL_H
#define _MSGUTL_H 1

#ident "@(#) $RCSfile: msgutl.h,v $ $Name:  $($Revision: 1.1.1.2.4.3 $) $Date: 2005/12/18 05:41:24 $"

/*  -------------------------------------------------------------------  */
/*				 Dependencies                            */

#ifndef _LIS_CONFIG_H
#include <sys/strconfig.h>	/* config definitions */
#endif
#ifndef _SHARE_H
#include <sys/LiS/share.h>	/* streams shared defs */
#endif
#ifndef _MSG_H
#include <sys/LiS/msg.h>	/* streams messages */
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

/*  -------------------------------------------------------------------  */
/*				    Types                                */

/*  -------------------------------------------------------------------  */
/*				 Glob. Vars.                             */

/*  -------------------------------------------------------------------  */
/*			Exported functions & macros                      */

#ifdef __KERNEL__

/* return bytes in first msg blk
 */
#define lis_mblksize(mp)	((mp)->b_wptr - (mp)->b_rptr)

/* msgsize - count sizes of blocks of message
 *
 */
extern int _RP lis_msgsize(mblk_t *mp);

/* msgdsize - return number of data bytes in M_DATA blocks in message
 *
 */
extern int _RP lis_msgdsize(mblk_t *mp);

/* xmsgsize - count sizes of consecutive blocks of the same
 *	type as the first
 *
 */
extern int _RP lis_xmsgsize(mblk_t *mp);

/* adjmsg - trim abs(len) bytes from a message. If len<0, trim
 *	from tail; else trim from head. If len is greater than
 *      the message size or the trim crosses message blocks of
 *      differing types, adjmsg fails. Any blocks that are
 *	completely trimmed are not removed, but have their
 *	rptrs set to their wptrs.
 *	Returns 1 on success; 0 otherwise.
 *
 */
extern int _RP lis_adjmsg(mblk_t *mp, int length);

/* copyb - create and return a copy of a message block
 *
 */
extern mblk_t *_RP lis_copyb(mblk_t *mp);

/* lis_copymsg - create and return a copy of a message
 *
 */
extern mblk_t *_RP lis_copymsg(mblk_t *mp);

#endif				/* __KERNEL__ */

/*  -------------------------------------------------------------------  */

#ifdef __KERNEL__

/*
 * lis_dupb - duplicate a message block, updating the reference count.
 *	The data block and data buffer are reused.
 *
 */
extern mblk_t *_RP lis_dupb(mblk_t *mp);

/* lis_dupmsg - duplicate a message by duplicating the constituent
 *	data blocks.
 *
 */
extern mblk_t *_RP lis_dupmsg(mblk_t *mp);

/*
 * lis_linkb - concatenate mp1 and mp2.
 *
 */
extern void _RP lis_linkb(mblk_t *mp1, mblk_t *mp2);

/* unlinkb - remove first message block from a message. Return the
 *	next message block pointer, or NULL if no further blocks.
 *
 */
extern mblk_t *_RP lis_unlinkb(mblk_t *mp);

/* lis_pullupmsg - attempt to merge the first len data bytes of a
 *	message into a single block. If len is -1, all leading
 *	blocks of the same type are merged.
 *	The message header is reused, but a new data block and
 *	data buffer are allocated for the first block.
 *	Only blocks of the same type can be merged.
 *	Returns 1 on success; 0 otherwise.
 *
 */
extern int _RP lis_pullupmsg(mblk_t *mp, int length);

/*
 * lis_msgpullup
 *
 * Similar to pullupmsg except that it returns a pointer to a _copy_
 * of the message with 'length' bytes pulled up into the first buffer.
 * The original message is unaltered.
 *
 * 'length' of -1 means pull up all bytes.
 *
 * Return of NULL means there were not 'length' bytes in the orignial
 * message or that something went wrong.
 */
extern mblk_t *_RP lis_msgpullup(mblk_t *mp, int length);

#endif				/* __KERNEL__ */

/*  -------------------------------------------------------------------  */
#ifdef __KERNEL__
/* lis_rmvb - remove message block bp from message mp. Returns a
 *	pointer to the modified message, or NULL if bp was the
 *	only block, or -1 if bp wasn't in the message
 *
 */
extern mblk_t *_RP lis_rmvb(mblk_t *mp, mblk_t *bp);
#endif				/* __KERNEL__ */

/*  -------------------------------------------------------------------  */

#ifdef __KERNEL__

/* Check is msg is a data msg (should be given mp->b_datap->db_type)
 */

#define lis_datamsg(type)   \
        ((type)==M_DATA         || (type)==M_PROTO      || \
         (type)==M_PCPROTO      || (type)==M_DELAY         )

/* Okay, let's do it the right way
 */
#define lis_isdatamsg(mp)   (lis_datamsg((mp)->b_datap->db_type))
#define lis_isdatablk(dp)   (lis_datamsg((dp)->dbtype))

#endif				/* __KERNEL__ */

/*  -------------------------------------------------------------------  */
#ifdef __KERNEL__
/* Some shorthands
 */
#define lis_btype(mp)	((mp)->b_datap->db_type)
#define lis_bband(mp)	((mp)->b_band)
#define queclass(bp)	(lis_bband(bp))	/* return band for bp */
#endif				/* __KERNEL__ */

/*  -------------------------------------------------------------------  */
#endif				/* !_MSGUTL_H */

/*----------------------------------------------------------------------
# Local Variables:      ***
# change-log-default-name: "~/src/prj/streams/src/NOTES" ***
# End: ***
  ----------------------------------------------------------------------*/
