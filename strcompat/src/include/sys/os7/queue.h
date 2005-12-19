/*****************************************************************************

 @(#) $Id: queue.h,v 0.9.2.8 2005/12/19 03:26:01 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (C) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>

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

 Last Modified $Date: 2005/12/19 03:26:01 $ by $Author: brian $

 *****************************************************************************/

#ifndef __OS7_QUEUE_H__
#define __OS7_QUEUE_H__

#define QR_DONE		0
#define QR_ABSORBED	1
#define QR_TRIMMED	2
#define QR_LOOP		3
#define QR_PASSALONG	4
#define QR_PASSFLOW	5
#define QR_DISABLE	6
#define QR_STRIP	7
#define QR_RETRY	8

extern int ss7_w_flush(queue_t *q, mblk_t *mp);
extern int ss7_r_flush(queue_t *q, mblk_t *mp);
extern int ss7_putq(queue_t *q, mblk_t *mp, int (*proc) (queue_t *, mblk_t *),
		    void (*wakeup) (queue_t *));
extern int ss7_srvq(queue_t *q, int (*proc) (queue_t *, mblk_t *), void (*wakeup) (queue_t *));
extern int streamscall ss7_oput(queue_t *q, mblk_t *mp);
extern int streamscall ss7_osrv(queue_t *q);
extern int streamscall ss7_iput(queue_t *q, mblk_t *mp);
extern int streamscall ss7_isrv(queue_t *q);

#endif				/* __OS7_QUEUE_H__ */
