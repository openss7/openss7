/*                               -*- Mode: C -*- 
 * safe.c --- stream safe
 * Author          : Graham Wheeler
 * Created On      : Tue May 31 22:25:19 1994
 * Last Modified By: David Grothe
 * RCS Id          : $Id: safe.c,v 1.4 1996/01/20 17:02:56 dave Exp $
 * Purpose         : stream safe processing stuff
 * ----------------______________________________________________
 *
 *  Copyright (C) 1995  Graham Wheeler
 *  Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>
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
 *    dave@gcom.com
 */

#ident "@(#) LiS safe.c 2.9 9/30/03 20:39:19 "


/*  -------------------------------------------------------------------  */
/*				 Dependencies                            */

#include <sys/stream.h>
#include <sys/osif.h>


/*  -------------------------------------------------------------------  */
/*				  Glob. Vars                             */

/*  -------------------------------------------------------------------  */
/*			   Local functions & macros                      */

#define	LOG(fil, line, msg)	printk("%s: called from file %s #%d\n",  \
					msg, fil, line)


/*  -------------------------------------------------------------------  */
/*			Exported functions & macros                      */



void lis_safe_noenable(queue_t *q, char *f, int l)
{
    lis_flags_t     psw;
    if (!lis_check_q_magic(q,f,l)) return ;
    LIS_QISRLOCK(q, &psw) ;
    q->q_flag |= QNOENB;
    LIS_QISRUNLOCK(q, &psw) ;
}

void lis_safe_enableok(queue_t *q, char *f, int l)
{
    lis_flags_t     psw;
    if (!lis_check_q_magic(q,f,l)) return ;
    LIS_QISRLOCK(q, &psw) ;
    q->q_flag &= ~QNOENB;
    LIS_QISRUNLOCK(q, &psw) ;
}

int lis_safe_canenable(queue_t *q, char *f, int l)
{
    if (lis_check_q_magic(q,f,l))
	return !(q->q_flag & QNOENB);

    return 0;
}

queue_t *lis_safe_OTHERQ(queue_t *q, char *f, int l)
{
    queue_t	*oq = NULL ;

    if (lis_check_q_magic(q,f,l))
    {
	oq = q->q_other;
	if (lis_check_q_magic(oq,f,l))
	    return (oq) ;
    }

    return NULL;
}

queue_t *lis_safe_RD(queue_t *q, char *f, int l)
{
    queue_t	*oq = NULL ;

    if (lis_check_q_magic(q,f,l))
    {
	if ((q->q_flag&QREADR))
	    oq = q ;
	else
	    oq = q->q_other;

	if (lis_check_q_magic(oq,f,l))
	    return (oq) ;

    }
    return NULL;
}

queue_t *lis_safe_WR(queue_t *q, char *f, int l)
{
    queue_t	*oq = NULL ;

    if (lis_check_q_magic(q,f,l))
    {
	if ((q->q_flag&QREADR))
	    oq = q->q_other;
	else
	    oq = q ;

	if (lis_check_q_magic(oq,f,l))
	    return (oq) ;
    }

    return NULL;
}

int lis_safe_SAMESTR(queue_t *q, char *f, int l)
{
    if (   lis_check_q_magic(q,f,l)
	&& q->q_next != NULL
	&& lis_check_q_magic(q->q_next,f,l)
       )
	return ((q->q_flag&QREADR) == (q->q_next->q_flag&QREADR));

    return 0;
}

void lis_safe_putmsg(queue_t *q, mblk_t *mp, char *f, int l)
{
    if (   mp == NULL
	|| !lis_check_q_magic(q,f,l)
	|| q->q_qinfo == NULL
	|| q->q_qinfo->qi_putp == NULL
       )
    {
	LOG(f, l, "NULL q, mp, q_qinfo or qi_putp in putmsg");
	freemsg(mp) ;
	return ;
    }

    lis_lockqf(q, f,l) ;
    if (q->q_flag & QPROCSOFF)
    {
	if ( LIS_DEBUG_CLOSE )
	    printk("lis_safe_putmsg: message on closing stream: %s #%d\n",
		    f, l);
	freemsg(mp) ;
    }
    else
	(*(q->q_qinfo->qi_putp))(q, mp);

    lis_unlockqf(q, f,l) ;
}

void lis_safe_putnext(queue_t *q, mblk_t *mp, char *f, int l)
{
    queue_t    *qnxt = NULL ;

    if (   mp == NULL
	|| !lis_check_q_magic(q,f,l)
	|| !lis_check_q_magic(qnxt = q->q_next,f,l)
       )
    {
	LOG(f, l, "NULL q, mp or q_next in putnext");
	freemsg(mp) ;
	return ;
    }

    if (LIS_DEBUG_TRCE_MSG)
	lis_mark_mem(mp, lis_queue_name(qnxt), MEM_MSG) ;

    if (LIS_DEBUG_PUTNEXT)
    {
	printk("putnext: %s from \"%s\" to \"%s\" size %d\n",
			    lis_msg_type_name(mp),
			    lis_queue_name(q), lis_queue_name(qnxt),
			    lis_msgsize(mp)
	      ) ;

	if (LIS_DEBUG_ADDRS)
	    printk("        q=%lx, mp=%lx, mp->b_rptr=%lx, wptr=%lx\n",
		   (long) q, (long) mp, (long) mp->b_rptr, (long) mp->b_wptr);
    }

    lis_safe_putmsg(qnxt, mp, f, l) ;
}

void lis_safe_qreply(queue_t *q, mblk_t *mp, char *f, int l)
{
    if (mp == NULL)
    {
	LOG(f, l, "NULL msg in qreply");
	return ;
    }

    if ((q = LIS_OTHERQ(q)) == NULL)
    {
	LOG(f, l, "NULL OTHERQ(q) in qreply");
	freemsg(mp) ;
	return ;
    }

    lis_safe_putnext(q, mp, f, l);
}



/*----------------------------------------------------------------------
# Local Variables:      ***
# change-log-default-name: "~/src/prj/streams/src/NOTES" ***
# End: ***
  ----------------------------------------------------------------------*/
