/*****************************************************************************

 @(#) $Id: sctp_output.h,v 0.9.2.5 2007/08/14 12:18:43 brian Exp $

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

 Last Modified $Date: 2007/08/14 12:18:43 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sctp_output.h,v $
 Revision 0.9.2.5  2007/08/14 12:18:43  brian
 - GPLv3 header updates

 Revision 0.9.2.4  2007/06/17 01:56:20  brian
 - updates for release, remove any later language

 *****************************************************************************/

#ifndef __SCTP_OUTPUT_H__
#define __SCTP_OUTPUT_H__

#ident "@(#) $RCSfile: sctp_output.h,v $ $Name:  $($Revision: 0.9.2.5 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

extern void sctp_xmit_ootb(uint32_t daddr, uint32_t saddr, mblk_t *mp);
extern void sctp_xmit_msg(uint32_t daddr, mblk_t *mp, sctp_t * sp);
extern void sctp_send_msg(sctp_t * sp, sctp_daddr_t * sd, mblk_t *mp);

#define SCTP_CONFIG_ERROR_GENERATOR

#define SCTP_CONFIG_ERROR_GENERATOR_LEVEL  8
#define SCTP_CONFIG_ERROR_GENERATOR_LIMIT 13
#define SCTP_CONFIG_BREAK_GENERATOR_LEVEL 50
#define SCTP_CONFIG_BREAK_GENERATOR_LIMIT 200

#endif				/* __SCTP_OUTPUT_H__ */
