/*****************************************************************************

 @(#) $Id: xti_ss7.h,v 0.9.2.5 2007/08/14 12:17:13 brian Exp $

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

 Last Modified $Date: 2007/08/14 12:17:13 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: xti_ss7.h,v $
 Revision 0.9.2.5  2007/08/14 12:17:13  brian
 - GPLv3 header updates

 Revision 0.9.2.4  2006/09/25 12:10:09  brian
 - updated and doxygenified headers

 *****************************************************************************/

#ifndef _SYS_XTI_SS7_H
#define _SYS_XTI_SS7_H

#ident "@(#) $RCSfile: xti_ss7.h,v $ $Name:  $($Revision: 0.9.2.5 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

/* This file can be processed with doxygen(1). */

/** @addtogroup xnet
  * @{ */

/** @file
  * XTI SS7-Specific header file. */

/** XTI SS7 Option level. */
#define T_SS7_SS7		0	/**< Common to all SS7 Levels. */

/** @name XTI SS7 Options
  * @{ */
#define T_SS7_PVAR		1	/**< Protocol variant. */
#define T_SS7_MPLEV		2	/**< Multiple priorities. */
#define T_SS7_DEBUG		3	/**< Debug. */
#define T_SS7_PCR		4	/**< Preventative cyclic retransmission. */
#define T_SS7_HSL		5	/**< High-speed links. */
#define T_SS7_XSN		6	/**< Extended sequence number. */
#define T_SS7_CLUSTER		7	/**< Cluster routing. */
#define T_SS7_SEQ_CTRL		8	/**< Sequence control parameter. */
#define T_SS7_PRIORITY		9	/**< Message priority. */
#define T_SS7_PCLASS		10	/**< Protocol class. */
#define T_SS7_IMPORTANCE	11	/**< Importance. */
#define T_SS7_RET_ERROR		12	/**< Return on error. */
/** @} */

/** @name Protocol Variants
  * Note: these values must be aligned with <ss7/lmi_ioctl.h>
  * @{ */
#define T_SS7_PVAR_ITUT88	0x0100	/**< ITU-T 1988 */
#define T_SS7_PVAR_ITUT93	0x0101	/**< ITU-T 1993 */
#define T_SS7_PVAR_ITUT96	0x0102	/**< ITU-T 1996 */
#define T_SS7_PVAR_ITUT00	0x0103	/**< ITU-T 2000 */
#define T_SS7_PVAR_ETSI88	0x0200	/**< ETSI 1988 */
#define T_SS7_PVAR_ETSI93	0x0201	/**< ETSI 1993 */
#define T_SS7_PVAR_ETSI96	0x0202	/**< ETSI 1996 */
#define T_SS7_PVAR_ETSI00	0x0203	/**< ETSI 2000 */
#define T_SS7_PVAR_ANSI88	0x0300	/**< ANSI 1988 */
#define T_SS7_PVAR_ANSI92	0x0301	/**< ANSI 1992 */
#define T_SS7_PVAR_ANSI96	0x0302	/**< ANSI 1996 */
#define T_SS7_PVAR_ANSI00	0x0303	/**< ANSI 2000 */
#define T_SS7_PVAR_JTTC94	0x0400	/**< JTTC 1994 */
/** @} */

/** @name Protocol Options
  * @{ */
#define T_SS7_POPT_MPLEV	0x0001	/**< Multiple priority levels. */
#define T_SS7_POPT_PCR		0x0002	/**< Preventative cyclic retransmission. */
#define T_SS7_POPT_HSL		0x0004	/**< High speed links. */
#define T_SS7_POPT_XSN		0x0008	/**< Extended sequence numbers. */
#define T_SS7_POPT_CLUSTER	0x0010	/**< Cluster routing. */
#define T_SS7_POPT_DEBUG	0x0020	/**< Debug. */
/** @} */

/** @} */

#endif				/* _SYS_XTI_SS7_H */

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
