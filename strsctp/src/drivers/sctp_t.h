/*****************************************************************************

 @(#) $Id: sctp_t.h,v 0.9 2004/06/22 06:39:02 brian Exp $

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
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2004/06/22 06:39:02 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SCTP_T_H__
#define __SCTP_T_H__

#ident "@(#) $RCSfile: sctp_t.h,v $ $Name:  $($Revision: 0.9 $) $Date: 2004/06/22 06:39:02 $"

// 
// TLI interface state flags
// 
#define TSF_UNBND	( 1 << TS_UNBND		)
#define TSF_WACK_BREQ	( 1 << TS_WACK_BREQ	)
#define TSF_WACK_UREQ	( 1 << TS_WACK_UREQ	)
#define TSF_IDLE	( 1 << TS_IDLE		)
#define TSF_WACK_OPTREQ	( 1 << TS_WACK_OPTREQ	)
#define TSF_WACK_CREQ	( 1 << TS_WACK_CREQ	)
#define TSF_WCON_CREQ	( 1 << TS_WCON_CREQ	)
#define TSF_WRES_CIND	( 1 << TS_WRES_CIND	)
#define TSF_WACK_CRES	( 1 << TS_WACK_CRES	)
#define TSF_DATA_XFER	( 1 << TS_DATA_XFER	)
#define TSF_WIND_ORDREL	( 1 << TS_WIND_ORDREL	)
#define TSF_WREQ_ORDREL	( 1 << TS_WREQ_ORDREL	)
#define TSF_WACK_DREQ6	( 1 << TS_WACK_DREQ6	)
#define TSF_WACK_DREQ7	( 1 << TS_WACK_DREQ7	)
#define TSF_WACK_DREQ9	( 1 << TS_WACK_DREQ9	)
#define TSF_WACK_DREQ10	( 1 << TS_WACK_DREQ10	)
#define TSF_WACK_DREQ11	( 1 << TS_WACK_DREQ11	)
#define TSF_NOSTATES	( 1 << TS_NOSTATES	)

#define TSF_WACK_DREQ	(TSF_WACK_DREQ6 \
			|TSF_WACK_DREQ7 \
			|TSF_WACK_DREQ9 \
			|TSF_WACK_DREQ10 \
			|TSF_WACK_DREQ11)

#define TF_IP_BROADCAST			(1<< 0)
#define TF_IP_DONTROUTE			(1<< 1)
#define TF_IP_REUSEADDR			(1<< 2)
#define TF_IP_OPTIONS			(1<< 3)
#define TF_IP_TOS			(1<< 4)
#define TF_IP_TTL			(1<< 5)
#define TF_SCTP_NODELAY			(1<< 6)
#define TF_SCTP_CORK			(1<< 7)
#define TF_SCTP_PPI			(1<< 8)
#define TF_SCTP_SID			(1<< 9)
#define TF_SCTP_SSN			(1<<10)
#define TF_SCTP_TSN			(1<<11)
#define TF_SCTP_RECVOPT			(1<<12)
#define TF_SCTP_COOKIE_LIFE		(1<<13)
#define TF_SCTP_SACK_DELAY		(1<<14)
#define TF_SCTP_PATH_MAX_RETRANS	(1<<15)
#define TF_SCTP_ASSOC_MAX_RETRANS	(1<<16)
#define TF_SCTP_MAX_INIT_RETRIES	(1<<17)
#define TF_SCTP_HEARTBEAT_ITVL		(1<<18)
#define TF_SCTP_RTO_INITIAL		(1<<19)
#define TF_SCTP_RTO_MIN			(1<<20)
#define TF_SCTP_RTO_MAX			(1<<21)
#define TF_SCTP_OSTREAMS		(1<<22)
#define TF_SCTP_ISTREAMS		(1<<23)
#define TF_SCTP_COOKIE_INC		(1<<24)
#define TF_SCTP_THROTTLE_ITVL		(1<<25)
#define TF_SCTP_MAC_TYPE		(1<<26)
#define TF_SCTP_HB			(1<<27)
#define TF_SCTP_RTO			(1<<28)
#define TF_SCTP_MAXSEG			(1<<29)
#define TF_SCTP_STATUS			(1<<30)
#define TF_SCTP_DEBUG			(1<<31)
#define TF_SCTP_ALLOPS			0xffffffff

extern void sctp_t_init(void);
extern void sctp_t_term(void);

#endif				/* __SCTP_T_H__ */
