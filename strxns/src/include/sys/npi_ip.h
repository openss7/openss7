/*****************************************************************************

 @(#) $Id: npi_ip.h,v 0.9.2.1 2006/03/18 09:39:04 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 2 of the License.

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

 Last Modified $Date: 2006/03/18 09:39:04 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: npi_ip.h,v $
 Revision 0.9.2.1  2006/03/18 09:39:04  brian
 - added ip driver headers

 *****************************************************************************/

#ifndef SYS_NPI_IP_H
#define SYS_NPI_IP_H

#ident "@(#) $RCSfile: npi_ip.h,v $ $Name:  $($Revision: 0.9.2.1 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

#include <sys/npi.h>

#define IP_FLAG_DEFAULT_RC_SEL	(1<<0)
#define IP_BINDPORT_LOCK	(1<<1)

#define N_QOS_SEL_INFO_IP	0x0201
#define N_QOS_RANGE_INFO_IP	0x0202

typedef struct N_qos_sel_info_ip {
	np_ulong n_qos_type;		/* always N_QOS_SEL_INFO_IP */
	np_ulong ttl;
	np_ulong tos;
} N_qos_sel_info_ip_t;

typedef struct N_qos_range_info_ip {
	np_ulong n_qos_type;		/* always N_QOS_RANGE_INFO_IP */
} N_qos_range_info_ip_t;

#endif				/* SYS_NPI_IP_H */
