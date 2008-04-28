/*****************************************************************************

 @(#) $Id: lock.h,v 0.9.2.8 2008-04-28 16:47:11 brian Exp $

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

 Last Modified $Date: 2008-04-28 16:47:11 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: lock.h,v $
 Revision 0.9.2.8  2008-04-28 16:47:11  brian
 - updates for release

 Revision 0.9.2.7  2007/08/12 15:51:10  brian
 - header and extern updates, GPLv3, 3 new lock functions

 Revision 0.9.2.6  2006/11/03 10:39:21  brian
 - updated headers, correction to mi_timer_expiry type

 *****************************************************************************/

#ifndef __OS7_LOCK_H__
#define __OS7_LOCK_H__

#ident "@(#) $RCSfile: lock.h,v $ $Name:  $($Revision: 0.9.2.8 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

extern int streamscall ss7_trylockq(queue_t *q);
extern void streamscall ss7_unlockq(queue_t *q);

#endif				/* __OS7_LOCK_H__ */
