/************************************************************************
*                     User Level Testing Routines 			*
*************************************************************************
*									*
* This file contains abstract routine definitions suitable for running	*
* STREAMS as a user-level process.					*
*									*
* It includes "port-mdep.c" first and then adds in code specific to	*
* the user level environment.						*
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

#ident "@(#) LiS user-mdep.c 2.6 12/15/02 18:00:04 "

#include "port-mdep.c"

#include <sys/LiS/head.h>		/* for strread, strwrite, etc */

int	lis_runq_sched ;		/* needed to resolve a reference */
lis_atomic_t	lis_runq_active_flags[64] ;

struct file_operations
lis_streams_fops = {
	NULL,			/* lseek   -- no lseek  */
	lis_strread,		/* read    		*/
	lis_strwrite,		/* write to stream      */
	NULL,			/* readdir -- no readdir*/
	NULL,			/* select  		*/
	lis_strioctl,		/* ioctl   		*/
	NULL,			/* mmap    -- no mmap   */
	lis_stropen,		/* open the stream      */
	lis_strclose,		/* close the stream     */
	NULL,			/* fsync   -- no fsync  */
	NULL,			/* fasync  -- no fasync */
	NULL,			/* check_media_change	*/
	NULL,			/* revalidate		*/
	lis_strputpmsg,		/* putpmsg & putmsg	*/
	lis_strgetpmsg,		/* getpmsg & getmsg	*/
	lis_strpoll		/* poll			*/
};

/*
 *  user_hitime() -
 *
 *  semantics are identical to lis_hitime() in linux-mdep.c
 */
unsigned long user_hitime(void)
{
    struct timeval tv; 

    gettimeofday(&tv,NULL);		/* user time function */
    return( (tv.tv_sec & 0x3f) * 1000000 + tv.tv_usec );

}

/*
 *  user_get_fifo()
 *
 *  this is cheating (to use user_open), but usrio wants to do everything
 *  user_open() does anyway...
 */
int user_get_fifo( struct file **f )
{
    int fd = user_open( "fifo", O_RDWR, 0 );

    if (fd < 0)  return -errno;

    *f = user_file_of_fd(fd);

    return 0;
}

/*
 *  user_get_pipe()
 */
int user_get_pipe( struct file **f0, struct file **f1 )
{
    stdata_t *hd0, *hd1;
    queue_t *wq0, *wq1;
    int error;

    /*
     *  get a pair of unique FIFOs for the pipe ends
     */
    if ((error = lis_get_fifo( f0 )) < 0)  goto no_fifos;
    if ((error = lis_get_fifo( f1 )) < 0)  goto no_fifo1;

    /*
     *  OK - make them peers, and get the head queues for twisting
     */
    hd0 = FILE_STR(*f0);  wq0 = hd0->sd_wq;
    hd1 = FILE_STR(*f1);  wq1 = hd1->sd_wq;
    hd0->sd_peer = hd1;
    hd1->sd_peer = hd0;

    /*
     *  twist the write queues to point to the peers' read queues
     */
    wq0->q_next = RD(wq1);
    wq1->q_next = RD(wq0);

    return 0;
    
/*
 *  handle failures...
 */
no_fifo1:
    user_close((*f0)->f_fdnr);
no_fifos:
    return error;
}

/*
 *  user_sendfd()
 */
int user_sendfd( stdata_t *sendhd, unsigned int fd, struct file *fp )
{
    stdata_t *recvhd;
    mblk_t *mp;
    strrecvfd_t *send;
    int error;

    error = -EPIPE;
    if (!sendhd ||
	!(sendhd->magic == STDATA_MAGIC) ||
	!(recvhd = sendhd->sd_peer) ||
	F_ISSET(sendhd->sd_flag, STRHUP))
	goto not_fifo;

    error = -ENOSR;
    if (!(mp = allocb( sizeof(strrecvfd_t), BPRI_HI )))  goto no_msg;

    if (!fp) {
	/*
	 *  get the file pointer corresponding to fd in the current (i.e.,
	 *  the sender's) process.
	 */
	error = -EBADF;
	if (!(fp = user_file_of_fd(fd)))  goto bad_file;
	/*
	 *  there's one case where we don't want the fp's count bumped;
	 *  if the fp is for the receiving stream itself, it can't be
	 *  closed if the count is high and no receiver shows up.  In
	 *  that case, the fp doesn't need bumping anyway.
	 */
	if (FILE_INODE(fp) != recvhd->sd_inode) {
	    (fp->f_count)++;  /* bump count */
	    (FILE_INODE(fp)->i_count)++;
	}
    }
    /*
     *  OK - set up an M_PASSFP message containing a strrecvfd and
     *  put it in the peer's stream head read queue.
     */
    mp->b_datap->db_type = M_PASSFP;
    send = (strrecvfd_t *) mp->b_rptr;
    send->f.fp = fp;
    send->uid  = EUID(fp);
    send->gid  = EGID(fp);
    send->r.fp = (FILE_INODE(fp) == recvhd->sd_inode ? fp : NULL);
    send->r.hd = recvhd;
    mp->b_wptr = mp->b_rptr + sizeof(strrecvfd_t);
    (recvhd->sd_rfdcnt)++;

    if (LIS_DEBUG_IOCTL) {
	printk( "sendfd( ..., %d, 0x%x ) from %s%s to %s%s",
		 fd, fp,
		 lis_strm_name(sendhd), lis_maj_min_name(sendhd),
		 lis_strm_name(recvhd), lis_maj_min_name(recvhd) );
	if (send->r.fp)
	    printk( " as 0x%x\n", fp );
	else
	    printk( "\n" );
    }

    /*
     *  the following wakes up a receiver if needed
     */
    lis_strrput( recvhd->sd_rq, mp );

    return 0;

bad_file:
no_msg:
    freemsg(mp);
not_fifo:
    return error;
}

/*
 *  user_recvfd()
 */
int user_recvfd( stdata_t *recvhd, strrecvfd_t *recv, struct file *fp )
{
    mblk_t *mp;
    strrecvfd_t *sent;
    int error;

    lis_bzero( recv, sizeof(strrecvfd_t) );

    error = -EPIPE;
    if (!recvhd || !(recvhd->magic == STDATA_MAGIC))
	goto not_stream;

    /*
     *  we expect the caller to have sync'ed with the sender;
     *  we just fail if no message is waiting
     */
    error = -EAGAIN;
    if (!(mp = getq(recvhd->sd_rq)))
	goto no_msg;

    /*
     *  check the message and its contents for validity.
     */
    if (mp->b_datap->db_type != M_PASSFP)
	goto not_passfp;
    (recvhd->sd_rfdcnt)--;
    if (mp->b_wptr - mp->b_rptr < sizeof(strrecvfd_t))
	goto not_passfp;

    /*
     *  it's a passed FP - hook up the file that was passed to the new FD
     *
     *  FIXME: check permissions first?  I don't think so; I think the
     *  uid/gid are actually provided so that the caller can do checks
     */
    sent = (strrecvfd_t *) mp->b_rptr;
    recv->uid = sent->uid;
    recv->gid = sent->gid;

    recv->f.fd = (sent->f.fp)->f_fdnr;

#if 0
    if (sent->r.fp)  ((sent->f.fp)->f_count)++;
#endif

    freemsg(mp);  /* we can release the sent message now */

    return 0;     /* OK */

/*
 *  handle failures
 */
not_passfp:
    putbq( recvhd->sd_rq, mp );   /* put the message back */
no_msg:
not_stream:
    return error;
}

/*
 *  user_free_passfp()
 */
void user_free_passfp( mblk_t *mp )
{
    strrecvfd_t *sent;
    stdata_t *recvhd;

    /*
     *  check the message and its contents for validity.
     */
    if (mp->b_datap->db_type != M_PASSFP)  goto not_passfp;
    if (mp->b_wptr - mp->b_rptr < sizeof(strrecvfd_t))  goto not_passfp;

    sent = (strrecvfd_t *) mp->b_rptr;
    recvhd = sent->r.hd;
    (recvhd->sd_rfdcnt)--;

    if (!(sent->r.fp)) {
	((sent->f.fp)->f_count)--;
	(FILE_INODE(sent->f.fp)->i_count)--;
    }

not_passfp:
    lis_freemsg(mp);
}
