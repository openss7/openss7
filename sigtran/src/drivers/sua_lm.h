/*****************************************************************************

 @(#) $Id: sua_lm.h,v 0.9.2.1 2006/10/17 11:55:43 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (C) 2001  OpenSS7 Corporation <http://www.openss7.com>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 Ave, Cambridge, MA 02139, USA.

 Last Modified $Date: 2006/10/17 11:55:43 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sua_lm.h,v $
 Revision 0.9.2.1  2006/10/17 11:55:43  brian
 - copied files into new packages from strss7 package

 Revision 0.9.2.1  2004/08/21 10:14:59  brian
 - Force checkin on branch.

 Revision 0.9  2004/01/17 08:23:12  brian
 - Added files for 0.9 baseline autoconf release.

 Revision 0.8.2.1  2002/10/18 02:33:28  brian
 Indentation changes.

 Revision 0.8  2002/04/02 08:21:09  brian
 Started Linux 2.4 development branch.

 Revision 0.7  2001/07/29 10:52:19  brian
 Split up SUA files.

 *****************************************************************************/

#ifndef __SUA_LM_H__
#define __SUA_LM_H__

/*
 *  =========================================================================
 *
 *  SUA --> LM Upstream Primitives
 *
 *  =========================================================================
 */

/*
 *  LM_LINK_IND
 *  -------------------------------------------------------------------------
 *  All of the local management function associated with lower multiplexor
 *  streams are performed by SUA Layer Management.  This relieves SUA from the
 *  knowledge of the implementation details of various lower layer providers.
 *  All local management primitives are passed to and from the SUA Layer
 *  Manager in LM_LINK_IND and LM_EVENT_REQ primitives.  This function
 *  performs the details:
 */
static int m_link_ind(queue_t * q, mblk_t * dp)
{
	mblk_t *mp;
	lm_link_ind_t *p;
	pp_t *pp = (pp_t *) q->q_ptr;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = dp->b_datap->db_type;
		p = (lm_link_ind_t *) mp->b_wptr;
		p->primitive = LM_LINK_IND;
		p->muxid = pp->muxid;
		p->type = pp->type;
		p->mode = pp->mode;
		p->class = pp->class;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = dp;
		pullmsg(mp, -1);
		ua_u_rput(ua_lmq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

#endif				/* __SUA_LM_H__ */
