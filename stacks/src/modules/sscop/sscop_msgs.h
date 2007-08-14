/*****************************************************************************

 @(#) $Id: sscop_msgs.h,v 0.9.2.4 2007/08/14 12:18:51 brian Exp $

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

 Last Modified $Date: 2007/08/14 12:18:51 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sscop_msgs.h,v $
 Revision 0.9.2.4  2007/08/14 12:18:51  brian
 - GPLv3 header updates

 Revision 0.9.2.3  2007/06/17 01:56:28  brian
 - updates for release, remove any later language

 *****************************************************************************/

#ifndef __SSCOP_MSGS_H__
#define __SSCOP_MSGS_H__

#ident "@(#) $RCSfile: sscop_msgs.h,v $ $Name:  $($Revision: 0.9.2.4 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

#define	SSCOP_BGN	0x01	/* Request initialization */
#define SSCOP_BGAK	0x02	/* Request acknowledgement */
#define SSCOP_END	0x03	/* Disconnect command */
#define SSCOP_ENDAK	0x04	/* Disconnect acknowledgement */
#define SSCOP_RS	0x05	/* Resynchronization command */
#define SSCOP_RSAK	0x06	/* Resynchronization acknowledgement */
#define SSCOP_BGREJ	0x07	/* Connection reject */
#define SSCOP_SD	0x08	/* Sequence connection-mode data */
#define SSCOP_ER	0x09	/* Recovery command */
#define SSCOP_POLL	0x0a	/* Transmitter State Info Recv Sta Req */
#define SSCOP_STAT	0x0b	/* Solicited receiver state info */
#define SSCOP_USTAT	0x0c	/* Unsolicited receiver state info */
#define SSCOP_UD	0x0d	/* Unnumbered user data */
#define SSCOP_MD	0x0e	/* Unnumbered management data */
#define SSCOP_ERAK	0x0f	/* Recovery acknowledgement */

/*
 *  BGN PDU (Begin)
 *  -------------------------------------------------------------------------
 *  The BGN PDU is used to establish an SSCOP connection between two peer
 *  entities.  The BGN PDU requiest the clearing of the peer's transmitter and
 *  receiver buffers, and the initialization of the peer's transmitter and
 *  receiver state variables.
 */

#endif				/* __SSCOP_MSGS_H__ */
