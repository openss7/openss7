/*****************************************************************************

 @(#) $Id: xti_tcap.h,v 0.9.2.8 2008-04-29 07:10:47 brian Exp $

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

 Last Modified $Date: 2008-04-29 07:10:47 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: xti_tcap.h,v $
 Revision 0.9.2.8  2008-04-29 07:10:47  brian
 - updating headers for release

 Revision 0.9.2.7  2007/08/14 12:17:13  brian
 - GPLv3 header updates

 Revision 0.9.2.6  2007/08/03 13:35:03  brian
 - manual updates, put ss7 modules in public release

 Revision 0.9.2.5  2006/09/25 12:10:09  brian
 - updated and doxygenified headers

 *****************************************************************************/

#ifndef _SYS_XTI_TCAP_H
#define _SYS_XTI_TCAP_H

#ident "@(#) $RCSfile: xti_tcap.h,v $ $Name:  $($Revision: 0.9.2.8 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

/* This file can be processed with doxygen(1). */

/** @addtogroup xnet
  * @{ */

/** @file
  * XTI TCAP-specific header file. */

/** XTI TCAP Option level. */
#define T_SS7_TCAP		0x13	/**< TCAP level (same as SI value). */

/** @name TCAP Transport Provider Options
  * @{ */
#define T_TCAP_PVAR		1	/**< Protocol variant. */
#define T_TCAP_MPLEV		2	/**< Protocol options. */
#define T_TCAP_DEBUG		3	/**< Debug. */

#define T_TCAP_SEQ_CTRL		8	/**< Sequence control parameter. */
#define T_TCAP_PRIORITY		9	/**< Message priority. */

#define T_TCAP_PCLASS		10	/**< Protocol class. */
#define T_TCAP_IMPORTANCE	11	/**< Importance. */
#define T_TCAP_RET_ERROR	12	/**< Return on error. */

#define T_TCAP_APP_CTX		20	/**< Application context. */
#define T_TCAP_USER_INFO	21	/**< User information. */
#define T_TCAP_SEC_CTX		23	/**< Security context. */
#define T_TCAP_VERSION		24	/**< Version. */
/** @} */

/** @name TCAP Protocol Class
  * @{ */
#define T_TCAP_PCLASS_1		0x01	/**< Protocol class 1. */
#define T_TCAP_PCLASS_2		0x02	/**< Protocol class 2. */
#define T_TCAP_PCLASS_3		0x04	/**< Protocol class 3. */
#define T_TCAP_PCLASS_4		0x08	/**< Protocol class 4. */
#define T_TCAP_PCLASS_5		0x10	/**< Protocol class 5. */
#define T_TCAP_PCLASS_ALL	0x1f	/**< Protocol class any. */
/** @} */

/** @name Disconnect Reasons and Unitdata Errors
  * @{ */
#define T_TCAP_CLUST			(0x0100)    /**< Cluster modifier. */
#define T_TCAP_DEST_AVAILABLE		(0x1001)    /**< Destination available. */
#define T_TCAP_DEST_PROHIBITED		(0x1002)    /**< Destination prohibited. */
#define T_TCAP_DEST_CONGESTED		(0x1003)    /**< Destination congested. */
#define T_TCAP_CLUS_AVAILABLE		(T_TCAP_DEST_AVAILABLE |T_TCAP_CLUST)	/**< Cluster available. */
#define T_TCAP_CLUS_PROHIBITED		(T_TCAP_DEST_PROHIBITED|T_TCAP_CLUST)	/**< Cluster prohibited. */
#define T_TCAP_CLUS_CONGESTED		(T_TCAP_DEST_CONGESTED |T_TCAP_CLUST)	/**< Cluster congested. */
#define T_TCAP_RESTARTING		(0x1004)    /**< MTP Restart in progress. */
#define T_TCAP_USER_PART_UNKNOWN	(0x1005)    /**< MTP User Part unknown. */
#define T_TCAP_USER_PART_UNEQUIPPED	(0x1006)    /**< MTP User Part unequipped. */
#define T_TCAP_USER_PART_UNAVAILABLE	(0x1007)    /**< MTP User Part unavailable. */
/** @} */

/** @} */

#endif				/* _SYS_XTI_TCAP_H */

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
