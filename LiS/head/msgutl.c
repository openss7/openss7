/*****************************************************************************

 @(#) $RCSfile: msgutl.c,v $ $Name:  $($Revision: 1.1.1.5.4.7 $) $Date: 2008-04-29 08:33:13 $

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

 Last Modified $Date: 2008-04-29 08:33:13 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: msgutl.c,v $
 Revision 1.1.1.5.4.7  2008-04-29 08:33:13  brian
 - update headers for Affero release

 Revision 1.1.1.5.4.6  2007/08/14 10:47:01  brian
 - GPLv3 header update

 *****************************************************************************/

#ident "@(#) $RCSfile: msgutl.c,v $ $Name:  $($Revision: 1.1.1.5.4.7 $) $Date: 2008-04-29 08:33:13 $"

static char const ident[] = "$RCSfile: msgutl.c,v $ $Name:  $($Revision: 1.1.1.5.4.7 $) $Date: 2008-04-29 08:33:13 $";

/*                               -*- Mode: C -*- 
 * msgutl.c --- streams message utilities.
 * Author          : Graham Wheeler, Francisco J. Ballesteros
 * Created On      : Tue May 31 22:25:19 1994
 * Last Modified By: David Grothe
 * RCS Id          : $Id: msgutl.c,v 1.1.1.5.4.7 2008-04-29 08:33:13 brian Exp $
 * Purpose         : here you have utilites to handle str messages.
 * ----------------______________________________________________
 *
 *   Copyright (C) 1995  Francisco J. Ballesteros, Graham Wheeler
 *   Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>
 *
 *    You can reach us by email to any of
 *    nemo@ordago.uc3m.es, gram@aztec.co.za
 */

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
/*				 Dependencies                            */

#include <sys/stream.h>
#include <sys/osif.h>

/*  -------------------------------------------------------------------  */
/*				  Glob. Vars                             */

extern lis_spin_lock_t lis_msg_lock;	/* msg.c */

/*  -------------------------------------------------------------------  */
/*			   Local functions & macros                      */

/* tmsgsize - count sizes of consecutive blocks of the same
 *	type as the last; i.e. like lis_xmsgsize but measured
 *	from the back, not the front. For internal use.
 */
static int
tmsgsize(mblk_t *mp, mblk_t **first)
{
	int rtn = 0;
	unsigned char type;

	if (mp == NULL)
		return 0;
	type = mp->b_datap->db_type;
	*first = mp;
	while (mp) {
		if (mp->b_datap->db_type == type) {
			int n = (mp->b_wptr - mp->b_rptr);

			if (n > 0)
				rtn += n;
		} else {	/* different type; reset counter and count these */

			type = mp->b_datap->db_type;
			*first = mp;
			rtn = (mp->b_wptr - mp->b_rptr);	/* start w/this size */
		}
		mp = mp->b_cont;
	}
	return rtn;
}				/* tmsgsize */

/*  -------------------------------------------------------------------  */
/*			Exported functions & macros                      */

/* lis_msgsize - count sizes of blocks of message
 */
int _RP
lis_msgsize(mblk_t *mp)
{
	int rtn = 0;

	while (mp) {
		rtn += mp->b_datap->db_lim - mp->b_datap->db_base;
		mp = mp->b_cont;
	}
	return rtn;
}				/* lis_msgsize */

/*  -------------------------------------------------------------------  */
/* lis_msgdsize - return number of data bytes in M_DATA blocks in message
 */
int _RP
lis_msgdsize(mblk_t *mp)
{
	int rtn = 0;

	while (mp) {
		if (mp->b_datap->db_type == M_DATA) {
			int n = mp->b_wptr - mp->b_rptr;

			if (n > 0)
				rtn += n;
		}
		mp = mp->b_cont;
	}
	return rtn;
}				/* lis_msgdsize */

/*  -------------------------------------------------------------------  */

/* xmsgsize - count sizes of consecutive blocks of the same
 *	type as the first
 */
int _RP
lis_xmsgsize(mblk_t *mp)
{
	int rtn = 0;
	unsigned char type;

	if (mp == NULL)
		return 0;
	type = mp->b_datap->db_type;
	while (mp && mp->b_datap->db_type == type) {
		int n = (mp->b_wptr - mp->b_rptr);

		if (n > 0)
			rtn += n;
		mp = mp->b_cont;
	}
	return rtn;
}				/* lis_xmsgsize */

/*  -------------------------------------------------------------------  */

/* adjmsg - trim abs(len) bytes from a message. If len<0, trim
 *	from tail; else trim from head. If len is greater than
 *      the message size or the trim crosses message blocks of
 *      differing types, adjmsg fails. Any blocks that are
 *	completely trimmed are not removed, but have their
 *	rptrs set to their wptrs.
 *	Returns 1 on success; 0 otherwise.
 */
int _RP
lis_adjmsg(mblk_t *mp, int length)
{
	if (mp == NULL)
		return 0;
	if (length >= 0) {	/* trim from front of msg */
		mblk_t *bp = mp;
		int mlen = lis_xmsgsize(mp);	/* bytes of blks of same type */

		if (length > mlen)	/* invalid length, so fail */
			return 0;
		for (;;) {
			int n = bp->b_wptr - bp->b_rptr;

			if (n > length) {	/* partial block */
				bp->b_rptr += length;	/* adjust it */
				break;	/* we're done */
			} else if (n >= 0) {	/* whole block */
				bp->b_rptr += n;
				length -= n;
				if (length == 0)
					break;
			}
			bp = bp->b_cont;
		}
	} else {
		mblk_t *mp2;
		int mlen = tmsgsize(mp, &mp2);	/* get message tail byte count */

		length += mlen;	/* length=bytes to skip */
		if (length < 0)	/* invalid length; fail. */
			return 0;
		else {		/* remove trailing data */

			while (mp2) {
				int n = mp2->b_wptr - mp2->b_rptr;

				if (length <= 0)	/* whole block */
					mp2->b_rptr = mp2->b_wptr;
				else if (n > 0) {
					if (n > length)	/* partial block? */
						mp2->b_wptr = mp2->b_rptr + length;
					length -= n;
				}
				mp2 = mp2->b_cont;
			}
		}
	}
	return 1;
}				/* lis_adjmsg */

/*  -------------------------------------------------------------------  */
/* lis_copyb - create and return a copy of a message block
 */
mblk_t *_RP
lis_copyb(mblk_t *mp)
{
	mblk_t *bp;
	int msgsize, msglen;

	if (mp == NULL || (msgsize = (mp->b_datap->db_lim - mp->b_datap->db_base)) < 0
	    || (msglen = (mp->b_wptr - mp->b_rptr)) < 0)
		return NULL;
	if ((bp = allocb(msgsize, BPRI_LO)) == NULL)
		return NULL;
	bp->b_rptr += (mp->b_rptr - mp->b_datap->db_base);
	bp->b_wptr = bp->b_rptr;
	MEMCPY(bp->b_wptr, mp->b_rptr, msglen);
	bp->b_wptr += msglen;
	bp->b_datap->db_type = mp->b_datap->db_type;
	bp->b_band = mp->b_band;
	bp->b_flag = mp->b_flag;
	return bp;
}				/* lis_copyb */

/*  -------------------------------------------------------------------  */

/* lis_copymsg - create and return a copy of a message
 */
mblk_t *_RP
lis_copymsg(mblk_t *mp)
{
	mblk_t *rtn, *bp;

	if (mp == NULL || (rtn = bp = lis_copyb(mp)) == NULL)
		return NULL;
	while (mp->b_cont) {
		mp = mp->b_cont;
		if ((bp->b_cont = lis_copyb(mp)) == NULL) {
			lis_freemsg(rtn);
			return NULL;
		}
		bp = bp->b_cont;
	}
	return rtn;
}				/* lis_copymsg */

/*  -------------------------------------------------------------------  */
/*
 * lis_dupb - duplicate a message block, updating the reference count.
 *	The data block and data buffer are reused.
 */

mblk_t *_RP
lis_dupb(mblk_t *mp)
{
	if (mp) {
		mblk_t *bp = allocb(0, BPRI_MED);
		lis_flags_t psw;

		if (bp) {
			*bp = *mp;
			lis_spin_lock_irqsave(&lis_msg_lock, &psw);
			mp->b_datap->db_ref++;
			lis_spin_unlock_irqrestore(&lis_msg_lock, &psw);
			bp->b_cont = NULL;
			return bp;
		}
	}
	return NULL;
}				/* lis_dupb */

/*  -------------------------------------------------------------------  */
/* lis_dupmsg - duplicate a message by duplicating the constituent
 *	data blocks.
 */
mblk_t *_RP
lis_dupmsg(mblk_t *mp)
{
	mblk_t *bp, *rtn = lis_dupb(mp);

	if (rtn == NULL)
		return NULL;
	bp = rtn;
	while (mp->b_cont) {
		mp = mp->b_cont;
		bp->b_cont = lis_dupb(mp);
		bp = bp->b_cont;
		if (bp == NULL) {
			lis_freemsg(rtn);
			return NULL;
		}
	}
	return rtn;
}				/* lis_dupmsg */

/*  -------------------------------------------------------------------  */
/* lis_linkb - concatenate mp1 and mp2.
 */

void _RP
lis_linkb(mblk_t *mp1, mblk_t *mp2)
{
	if (mp1 && mp2) {
		while (mp1->b_cont)
			mp1 = mp1->b_cont;
		mp1->b_cont = mp2;
	}
}				/* lis_linkb */

/*  -------------------------------------------------------------------  */
/* unlinkb - remove first message block from a message. Return the
 *	next message block pointer, or NULL if no further blocks.
 */
mblk_t *_RP
lis_unlinkb(mblk_t *mp)
{
	mblk_t *rtn;

	if (mp == NULL)
		return NULL;
	rtn = mp->b_cont;
	mp->b_cont = NULL;
	return rtn;
}				/* lis_unlinkb */

/*  -------------------------------------------------------------------  */
/* lis_pullupmsg - attempt to merge the first len data bytes of a
 *	message into a single block. If len is -1, all leading
 *	blocks of the same type are merged.
 *	The message header is reused, but a new data block and
 *	data buffer are allocated for the first block.
 *	Only blocks of the same type can be merged.
 *	Returns 1 on success; 0 otherwise.
 */
#define ALIGN_MOD	(sizeof(char *) - 1)
int _RP
lis_pullupmsg(mblk_t *mp, int length)
{
	mblk_t *tmpbp, *newbp;
	mblk_t *prev;
	int nbytes;
	int n;

	if (mp == NULL || length < -1)
		return 0;

	nbytes = lis_xmsgsize(mp);	/* # bytes in msgs of same type as 1st */
	if (length == -1)
		length = nbytes;	/* pull up all bytes */

	if (length == 0)	/* nothing to do */
		return 1;

	if (length < 0 || length > nbytes)	/* do we have that many bytes? */
		return 0;

	if (length <= (n = mp->b_wptr - mp->b_rptr)) {
		if ((((uintptr_t) (long) mp->b_rptr) & ALIGN_MOD) == 0)	/* aligned? */
			return 1;	/* already pulled up */
		length = n;	/* don't make 1st bfr any smaller */
	}

	/* try to allocate a new message for merged blocks. Ensure that the new message has an
	   external data block for its buffer.  Setting nbytes to FASTBUF+1 ensures this. */
	nbytes = length;
	if (nbytes <= FASTBUF)
		nbytes = FASTBUF + 1;

	if ((newbp = allocb(nbytes, BPRI_MED)) == NULL)
		return 0;

	newbp->b_datap->db_type = mp->b_datap->db_type;
	/* copy the data */
	for (tmpbp = mp, prev = NULL; length > 0 && tmpbp != NULL;) {
		if (tmpbp->b_datap->db_type != newbp->b_datap->db_type)
			break;	/* not the same type */

		n = tmpbp->b_wptr - tmpbp->b_rptr;
		if (n > 0) {	/* same type as result msg */
			nbytes = lis_min(n, length);
			MEMCPY(newbp->b_wptr, tmpbp->b_rptr, nbytes);
			tmpbp->b_rptr += nbytes;
			newbp->b_wptr += nbytes;
			length -= nbytes;
		}

		if (prev != NULL	/* not the 1st mblk */
		    && tmpbp->b_rptr == tmpbp->b_wptr	/* bfr is empty */
		    ) {
			prev->b_cont = tmpbp->b_cont;	/* link around buffer */
			tmpbp->b_cont = NULL;	/* sever link to nxt bfr */
			lis_freeb(tmpbp);	/* free it */
		} /* do not move prev */
		else		/* 1st mblk or non-empty bfr */
			prev = tmpbp;	/* reassign prev */

		tmpbp = prev->b_cont;	/* to next buffer in chain */
	}

	/* 
	 * We now have mp pointing to the mblk of the first buffer
	 * of the original message.  We have newbp pointing to the
	 * single buffer result message which contains the entire
	 * message in one block.
	 *
	 * The data blk struct in newbp "owns" the pulled up data.  We point
	 * mp's datap to that data blk.  We have to leave mp's data blk struct
	 * alone because a dup of the mp message might be referencing it for
	 * the original data buffer.
	 *
	 * We do set mp's rptr/wptr to point to newbp where the pulled up
	 * data reside.
	 *
	 * We also "free" the data block that mp originally pointed to.  If
	 * the message had been dup'd then this is just a ref count decr.
	 * Otherwise it may actually free the data buffer if it was external
	 * to the header.
	 *
	 * It may look a little odd, but we don't free the header for newbp
	 * at this point.  That will happen as a side-effect of freeing
	 * the "mp" message later.
	 */
	lis_freedb(mp, 0);	/* free mp's data block (not hdr) */
	mp->b_datap = newbp->b_datap;
	mp->b_rptr = newbp->b_rptr;	/* point into new data buffer */
	mp->b_wptr = newbp->b_wptr;	/* point into new data buffer */
	newbp->b_datap = NULL;	/* sever newbp from its data block */
	newbp->b_rptr = NULL;	/* extra care */
	newbp->b_wptr = NULL;	/* extra care */
	/* data buf stays allocated */
	return (1);		/* success */

}				/* lis_pullupmsg */

/*  -------------------------------------------------------------------  */
/*
 * lis_msgpullup
 *
 * Copy 'len' bytes into a new message if possible.  Return a pointer
 * to the new message.
 *
 * If there are more than 'len' bytes in the passed-in msg then copy
 * the rest of the passed-in message onto the end of the new message.
 *
 * Return is a ptr to the new message or NULL if len bytes cannot be
 * found in the passed-in msg.
 *
 * len of -1 means concatenate all bytes of the passed-in msg.
 */
mblk_t *_RP
lis_msgpullup(mblk_t *mp, int len)
{
	unsigned char msg_type;
	int nbytes;
	int cpbytes;
	mblk_t *new_mp;
	mblk_t *bp;
	mblk_t *blast;

	if (mp == NULL)
		return (NULL);

	msg_type = mp->b_datap->db_type;

	if (len < 0) {		/* pull up all bytes *//* count bytes in orginal msg */
		for (len = 0, bp = mp; bp != NULL && bp->b_datap->db_type == msg_type;
		     bp = bp->b_cont) {
			if (bp->b_rptr < bp->b_wptr)	/* safety first */
				len += bp->b_wptr - bp->b_rptr;
		}
	}

	new_mp = allocb(len, BPRI_MED);
	if (new_mp == NULL)	/* couldn't allocate */
		return (NULL);

	new_mp->b_datap->db_type = msg_type;
	for (nbytes = 0; len > 0 && mp != NULL && mp->b_datap->db_type == msg_type;
	     mp = mp->b_cont, len -= nbytes) {
		if ((nbytes = mp->b_wptr - mp->b_rptr) > 0) {
			if ((cpbytes = nbytes) > len)
				cpbytes = len;	/* max out bytes to copy */

			memcpy(new_mp->b_wptr, mp->b_rptr, cpbytes);
			new_mp->b_wptr += cpbytes;
			if (cpbytes == len && cpbytes < nbytes) {	/* mp has leftover bytes */
				bp = allocb(nbytes - cpbytes, BPRI_MED);
				if (bp == NULL) {
					freemsg(new_mp);
					return (NULL);
				}

				bp->b_datap->db_type = msg_type;
				memcpy(bp->b_wptr, mp->b_rptr + cpbytes, nbytes - cpbytes);
				bp->b_wptr += nbytes - cpbytes;
				new_mp->b_cont = bp;
				/* allow the loop to terminate pointing at next buffer */
			}
		} else
			nbytes = 0;
	}

	if (len > 0) {		/* msg smaller than len bytes */
		freemsg(new_mp);
		return (NULL);	/* can't pull up len bytes */
	}

	if (mp != NULL) {	/* more buffers left */
		for (blast = new_mp; blast->b_cont != NULL;)
			blast = blast->b_cont;	/* find last buffer */

		bp = copymsg(mp);	/* copy the rest of the message */
		if (bp == NULL) {
			freemsg(new_mp);
			return (NULL);
		}

		blast->b_cont = bp;	/* hook onto end of new message */
	}

	return (new_mp);	/* return new message */

}				/* lis_msgpullup */

/*  -------------------------------------------------------------------  */
/* lis_rmvb - remove message block bp from message mp. Returns a
 *	pointer to the modified message, or NULL if bp was the
 *	only block, or -1 if bp wasn't in the message
 */
mblk_t *_RP
lis_rmvb(mblk_t *mp, mblk_t *bp)
{
	mblk_t *rtn = mp;

	if (mp == NULL || bp == NULL)
		return NULL;
	if (mp == bp)
		rtn = mp->b_cont;
	else
		for (;;) {
			if (mp == NULL)
				return (mblk_t *) -1;
			else if (mp->b_cont == bp) {
				mp->b_cont = bp->b_cont;
				break;
			} else
				mp = mp->b_cont;
		}
	bp->b_cont = NULL;
	return rtn;
}				/* lis_rmvb */

/*----------------------------------------------------------------------
# Local Variables:      ***
# change-log-default-name: "~/src/prj/streams/src/NOTES" ***
# End: ***
  ----------------------------------------------------------------------*/
