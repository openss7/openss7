/*****************************************************************************

 @(#) $Id: sccpi.h,v 0.9.2.2 2005/05/14 08:30:45 brian Exp $

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

 Last Modified $Date: 2005/05/14 08:30:45 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SS7_SCCPI_H__
#define __SS7_SCCPI_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2004  OpenSS7 Corporation"

/* This file can be processed by doxygen(1). */

/*
 *  Primitives
 */
#define	S_CONN_REQ		0
#define	S_CONN_RES		1
#define	S_DISCON_REQ		2
#define	S_DATA_REQ		3
#define	S_EXDATA_REQ		4
#define	S_INFO_REQ		5
#define	S_BIND_REQ		6
#define	S_UNBIND_REQ		7
#define	S_UNITDATA_REQ		8
#define	S_OPTMGMT_REQ		9

#define	S_CONN_IND		11
#define	S_CONN_CON		12
#define	S_DISCON_IND		13
#define	S_DATA_IND		14
#define	S_EXDATA_IND		15
#define	S_INFO_ACK		16
#define	S_BIND_ACK		17
#define	S_ERROR_ACK		18
#define	S_OK_ACK		19
#define	S_UNITDATA_IND		20
#define	S_UDERROR_IND		21

#define	S_DATACK_REQ		23
#define	S_DATACK_IND		24
#define	S_RESET_REQ		25
#define	S_RESET_RES		26

/*
 *  These are actually management messages...
 */
#define	S_INFORM_REQ		40
#define	S_COORD_REQ		41
#define	S_COORD_CON		42
#define	S_STATE_IND		43
#define	S_PCSTATE_IND		44
#define	S_TRAFFIC_IND		45

#endif				/* __SS7_SCCPI_H__ */
