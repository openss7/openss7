/*****************************************************************************

 @(#) $Id$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 2 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 Ave, Cambridge, MA 02139, USA.

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

 $Log: sua_ctrl.h,v $
 Revision 0.9.2.1  2006/10/17 11:55:42  brian
 - copied files into new packages from strss7 package

 Revision 0.9.2.1  2004/08/21 10:14:59  brian
 - Force checkin on branch.

 Revision 0.9  2004/01/17 08:23:12  brian
 - Added files for 0.9 baseline autoconf release.

 Revision 0.8.2.1  2002/10/18 02:33:28  brian
 Indentation changes.

 Revision 0.8  2002/04/02 08:21:09  brian
 Started Linux 2.4 development branch.

 Revision 0.7  2001/08/20 07:14:49  brian
 Added two files consistent with SCCP.

 *****************************************************************************/

#ifndef __SUA_CTRL_H__
#define __SUA_CTRL_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/*
 *  =========================================================================
 *
 *  Module --> Module (Control messages)
 *
 *  =========================================================================
 */
static inline mblk_t *m_error(int r_error, int w_error)
{
	mblk_t *mp;
	if ((mp = allocb(2, BPRI_HI))) {
		mp->b_datap->db_type = M_ERROR;
		*(mp->b_wptr++) = r_error;
		*(mp->b_wptr++) = w_error;
	}
	return (mp);
}
static inline mblk_t *m_hangup(void)
{
	mblk_t *mp;
	if ((mp = allocb(0, BPRI_HI))) {
		mp->b_datap->db_type = M_HANGUP;
	}
	return (mp);
}
static inline mblk_t *m_flush(int flags, int band)
{
	mblk_t *mp;
	if ((mp = allocb(2, BPRI_HI))) {
		mp->b_datap->db_type = M_FLUSH;
		*(mp->b_wptr++) = flags;
		*(mp->b_wptr++) = band;
	}
	return (mp);
}
static inline int m_error_reply(queue_t * q, mblk_t * pdu, int r_error, int w_error)
{
	mblk_t *mp;
	if ((mp = m_error(r_error, w_error))) {
		qreply(q, mp);
		return (-EPROTO);
	}
	return (-ENOBUFS);
}

#endif				/* __SUA_CTRL_H__ */
