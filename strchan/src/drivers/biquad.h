/*****************************************************************************

 @(#) $Id: biquad.h,v 0.9.2.4 2008-04-28 23:39:54 brian Exp $

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

 Last Modified $Date: 2008-04-28 23:39:54 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: biquad.h,v $
 Revision 0.9.2.4  2008-04-28 23:39:54  brian
 - updated headers for release

 Revision 0.9.2.3  2007/08/15 05:32:54  brian
 - GPLv3 updates

 Revision 0.9.2.2  2007/08/14 06:47:28  brian
 - GPLv3 header update

 *****************************************************************************/

#ifndef __LOCAL_BIQUAD_H__
#define __LOCAL_BIQUAD_H__

#ident "@(#) $RCSfile: biquad.h,v $ $Name:  $($Revision: 0.9.2.4 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

/*
 * SpanDSP - a series of DSP components for telephony
 *
 * biquad.h - General telephony bi-quad section routines (currently this just
 *            handles canonic/type 2 form)
 *
 * Written by Steve Underwood <steveu@coppice.org>
 *
 * Copyright (C) 2001 Steve Underwood
 *
 * All rights reserved.
 */

typedef struct {
	int32_t gain;
	int32_t a1;
	int32_t a2;
	int32_t b1;
	int32_t b2;

	int32_t z1;
	int32_t z2;
} biquad2_state_t;

static inline void
biquad2_init(biquad2_state_t * bq, int32_t gain, int32_t a1, int32_t a2, int32_t b1, int32_t b2)
{
	bq->gain = gain;
	bq->a1 = a1;
	bq->a2 = a2;
	bq->b1 = b1;
	bq->b2 = b2;

	bq->z1 = 0;
	bq->z2 = 0;
}

/*- End of function --------------------------------------------------------*/

static inline int16_t
biquad2(biquad2_state_t * bq, int16_t sample)
{
	int32_t y;
	int32_t z0;

	z0 = sample * bq->gain + bq->z1 * bq->a1 + bq->z2 * bq->a2;
	y = z0 + bq->z1 * bq->b1 + bq->z2 * bq->b2;

	bq->z2 = bq->z1;
	bq->z1 = z0 >> 15;
	y >>= 15;
	return y;
}

/*- End of function --------------------------------------------------------*/
/*- End of file ------------------------------------------------------------*/

#endif				/* __LOCAL_BIQUAD_H__ */
