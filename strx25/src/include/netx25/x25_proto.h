/*****************************************************************************

 @(#) $Id: x25_proto.h,v 0.9.2.1 2008-05-03 10:46:38 brian Exp $

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

 Last Modified $Date: 2008-05-03 10:46:38 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: x25_proto.h,v $
 Revision 0.9.2.1  2008-05-03 10:46:38  brian
 - added package files

 *****************************************************************************/

#ifndef __NETX25_X25_PROTO_H__
#define __NETX25_X25_PROTO_H__

#ident "@(#) $RCSfile: x25_proto.h,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

/* To identify the originator in N_RI and N_DI messages */
#define NS_USER		1
#define NS_PROVIDER	2

/* Reason when the originator is NS Provider */
#define NS_GENERIC		0xe0
#define NS_DTRANSIENT		0xe1
#define NS_DPERMENEN		0xe2
#define NS_TUNSPECIFIED		0xe3
#define NS_PUNSPECIFIED		0xe4
#define NS_QOSNATRANSIENT	0xe5
#define NS_QOSNAPERMENENT	0xe6
#define NS_NSAPTUNREACHABLE	0xe7
#define NS_NSAPPUNREACHABLE	0xe8
#define NS_NSAPPUNKNOWN		0xeb

/* Reason when the originator is NS User */
#define NU_GENERIC		0xf0
#define NU_DNORMAL		0xf1
#define NU_DABNORMAL		0xf2
#define NU_DINCOMPUSERDATA	0xf3
#define NU_TRANSIENT		0xf4
#define NU_PERMANENT		0xf5
#define NU_QOSNATRANSIENT	0xf6
#define NU_QOSNAPERMENENT	0xf7
#define NU_INCOMPUSERDATA	0xf8
#define NU_BADPROTID		0xf9

/* To specify the reason when the originator is NS Provider in N_RI messages */
#define NS_RUNSPECIFIED		0xe9
#define NS_RCONGESTION		0xea

/* To specify the reason when the originator is NS User in N_RI messages */
#define NU_RESYNC		0xfa


#endif				/* __NETX25_X25_PROTO_H__ */

