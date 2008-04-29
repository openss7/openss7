/*****************************************************************************

 @(#) $Id: tali.h,v 0.9.2.6 2008-04-29 07:11:15 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

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

 Last Modified $Date: 2008-04-29 07:11:15 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: tali.h,v $
 Revision 0.9.2.6  2008-04-29 07:11:15  brian
 - updating headers for release

 Revision 0.9.2.5  2007/08/14 12:18:54  brian
 - GPLv3 header updates

 Revision 0.9.2.4  2007/06/17 01:56:33  brian
 - updates for release, remove any later language

 *****************************************************************************/

#ifndef __TALI_H__
#define __TALI_H__

#ident "@(#) $RCSfile: tali.h,v $ $Name:  $($Revision: 0.9.2.6 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

extern int tali_reuseb(tali_t *, size_t, int, mblk_t *);

extern int tali_putq(queue_t *, mblk_t *, int (*)(queue_t *, mblk_t *));
extern int tali_srvq(queue_t *, int (*)(queue_t *, mblk_t *));
extern int tali_w_flush(queue_t *, mblk_t *);
extern int tali_r_flush(queue_t *, mblk_t *);

typedef struct ops {
	int (*r_prim) (queue_t *, mblk_t *);	/* read primitives */
	int (*w_prim) (queue_t *, mblk_t *);	/* write primitives */
} ops_t;

typedef struct drv {
	uint cmajor;
	uint nmajor;
	uint nminor;
	ops_t u_ops_lm;
	ops_t u_ops_ss7;
	ops_t l_ops_ss7;
	ops_t l_ops_ip;
	ops_t l_ops_sg;
} drv_t;

extern drv_t tali_lm_driver;
extern drv_t tali_sccp_driver;
extern drv_t tali_isup_driver;
extern drv_t tali_mtp_driver;

extern kmem_cachep_t tali_pp_cachep;
extern kmem_cachep_t tali_xp_cachep;
extern kmem_cachep_t tali_gp_cachep;
extern kmem_cachep_t tali_as_cachep;
extern kmem_cachep_t tali_ap_cachep;
extern kmem_cachep_t tali_sp_cachep;
extern kmem_cachep_t tali_np_cachep;

#endif				/* __TALI_H__ */
