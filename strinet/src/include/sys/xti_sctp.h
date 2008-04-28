/*****************************************************************************

 @(#) $Id: xti_sctp.h,v 0.9.2.8 2008-04-28 22:52:11 brian Exp $

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

 Last Modified $Date: 2008-04-28 22:52:11 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: xti_sctp.h,v $
 Revision 0.9.2.8  2008-04-28 22:52:11  brian
 - updated headers for release

 Revision 0.9.2.7  2007/08/14 04:27:23  brian
 - GPLv3 header update

 Revision 0.9.2.6  2006/10/19 12:48:15  brian
 - corrections to ETSI SACK frequency

 Revision 0.9.2.5  2006/09/25 12:07:34  brian
 - updated and doxygenified headers

 Revision 0.9.2.4  2005/07/18 12:47:51  brian
 - standard indentation

 Revision 0.9.2.3  2005/05/14 08:28:53  brian
 - copyright header correction

 Revision 0.9.2.2  2004/08/06 09:11:30  brian
 - Added override header files for TLI and XTI

 Revision 0.9.2.1  2004/05/16 04:12:33  brian
 - Updating strxnet release.

 Revision 0.9  2004/05/14 08:00:03  brian
 - Updated xns, tli, inet, xnet and documentation.

 Revision 0.9.2.1  2004/04/13 12:12:52  brian
 - Rearranged header files.

 *****************************************************************************/

#ifndef _SYS_XTI_SCTP_H
#define _SYS_XTI_SCTP_H

#ident "@(#) $RCSfile: xti_sctp.h,v $ $Name:  $($Revision: 0.9.2.8 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

/* This file can be processed with doxygen(1). */

/** @addtogroup xnet
  * @{ */

/** @file
  * XTI SCTP-Specific header file.
  *
  * These definitions are specific to the OpenSS7 STREAMS implementation of SCTP
  * and are not an OpenGroup specification.  */

/*
 * XTI SCTP-Specific Header File
 */

/** SCTP Options Level */
#define T_INET_SCTP	132	/* SCTP level (same as protocol number). */

/**
  * @name SCTP Transport Provider Options
  * @{ */
#define T_SCTP_NODELAY			 1
#define T_SCTP_CORK			 2
#define T_SCTP_PPI			 3
#define T_SCTP_SID			 4
#define T_SCTP_SSN			 5
#define T_SCTP_TSN			 6
#define T_SCTP_RECVOPT			 7
#define T_SCTP_COOKIE_LIFE		 8
#define T_SCTP_SACK_DELAY		 9
#define T_SCTP_PATH_MAX_RETRANS		10
#define T_SCTP_ASSOC_MAX_RETRANS	11
#define T_SCTP_MAX_INIT_RETRIES		12
#define T_SCTP_HEARTBEAT_ITVL		13
#define T_SCTP_RTO_INITIAL		14
#define T_SCTP_RTO_MIN			15
#define T_SCTP_RTO_MAX			16
#define T_SCTP_OSTREAMS			17
#define T_SCTP_ISTREAMS			18
#define T_SCTP_COOKIE_INC		19
#define T_SCTP_THROTTLE_ITVL		20
#define T_SCTP_MAC_TYPE			21
#define T_SCTP_CKSUM_TYPE		22
#define T_SCTP_ECN			23
#define T_SCTP_ALI			24
#define T_SCTP_ADD			25
#define T_SCTP_SET			26
#define T_SCTP_ADD_IP			27
#define T_SCTP_DEL_IP			28
#define T_SCTP_SET_IP			29
#define T_SCTP_PR			30
#define T_SCTP_LIFETIME			31
#define T_SCTP_DISPOSITION		32
#define T_SCTP_MAX_BURST		33
#define T_SCTP_HB			34
#define T_SCTP_RTO			35
/*
 *  Read-only options...
 */
#define T_SCTP_MAXSEG			36
#define T_SCTP_STATUS			37
#define T_SCTP_DEBUG			38
#define T_SCTP_SACK_FREQUENCY		39
/** @} */

/** @name T_SCTP_MAC_TYPE Values
  * @{ */
#define T_SCTP_HMAC_NONE	0
#define T_SCTP_HMAC_SHA1	1
#define T_SCTP_HMAC_MD5		2
/** @} */

/** @name T_SCTP_CKSUM_TYPE Values
  * @{ */
#define T_SCTP_CSUM_ADLER32	0
#define T_SCTP_CSUM_CRC32C	1
/** @} */

/** @name T_SCTP_DISPOSITION Values
  * @{ */
#define T_SCTP_DISPOSITION_NONE		0
#define T_SCTP_DISPOSITION_UNSENT	1
#define T_SCTP_DISPOSITION_SENT		2
#define T_SCTP_DISPOSITION_GAP_ACKED	3
#define T_SCTP_DISPOSITION_ACKED	4
/** @} */

/**
  * T_SCTP_HB structure.
  */
typedef struct t_sctp_hb {
	t_uscalar_t hb_dest;		/**< Destination address. */
	t_uscalar_t hb_onoff;		/**< Activation flag. */
	t_uscalar_t hb_itvl;		/**< Interval in milliseconds. */
} t_sctp_hb_t;

/**
  * T_SCTP_RTO structure.
  */
typedef struct t_sctp_rto {
	t_uscalar_t rto_dest;		/**< Destination address. */
	t_uscalar_t rto_initial;	/**< RTO.Initial (milliseconds). */
	t_uscalar_t rto_min;		/**< RTO.Min (milliseconds). */
	t_uscalar_t rto_max;		/**< RTO.Max (milliseconds). */
	t_uscalar_t max_retrans;	/**< Path.Max.Retrans (retries). */
} t_sctp_rto_t;

/**
  * T_SCTP_STATUS Structure.
  */
typedef struct t_sctp_dest_status {
	t_uscalar_t dest_addr;		/**< Dest address. */
	t_uscalar_t dest_cwnd;		/**< Dest congestion window. */
	t_uscalar_t dest_unack;		/**< Dest unacknowledged chunks. */
	t_uscalar_t dest_srtt;		/**< Dest smooth round trip time. */
	t_uscalar_t dest_rvar;		/**< Dest rtt variance. */
	t_uscalar_t dest_rto;		/**< Dest current rto. */
	t_uscalar_t dest_sst;		/**< Dest slow start threshold. */
} t_sctp_dest_status_t;

/**
  * T_SCTP_STATUS Element.
  */
typedef struct t_sctp_status {
	t_uscalar_t curr_rwnd;		/**< Current receive window. */
	t_uscalar_t curr_rbuf;		/**< Current receive buffer. */
	t_uscalar_t curr_nrep;		/**< Current dests reported. */
	t_sctp_dest_status_t curr_dest[0];	/**< Current primary dest. */
} t_sctp_status_t;

#ifndef SCTP_OPTION_DROPPING
/**
  * @name T_SCTP_DEBUG Values
  * @{ */
#define SCTP_OPTION_DROPPING	0x01	/**< Stream will drop packets. */
#define SCTP_OPTION_BREAK	0x02	/**< Stream will break dest #1. */
#define SCTP_OPTION_DBREAK	0x04	/**< Stream will break dest both ways. */
#define SCTP_OPTION_RANDOM	0x08	/**< Stream will drop packets at random. */
/** @} */
#endif

#endif				/* _SYS_XTI_SCTP_H */

/** @} */

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
