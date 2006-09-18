/*****************************************************************************

 @(#) $Id: xti_xti.h,v 0.9.2.4 2006/09/18 01:43:57 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (C) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (C) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2006/09/18 01:43:57 $ by $Author: brian $

 *****************************************************************************/

#ifndef _SYS_XTI_XTI_H
#define _SYS_XTI_XTI_H

#ident "@(#) $RCSfile: xti_xti.h,v $ $Name:  $($Revision: 0.9.2.4 $) Copyright (c) 2001-2004 OpenSS7 Corporation."

#ifndef t_scalar_t
typedef int32_t t_scalar_t;

#define t_scalar_t t_scalar_t
#endif

/* OPTIONS ON XTI LEVEL */

/* 
 *  XTI Level
 */
#define XTI_GENERIC	0xffff

/* 
 *  XTI-level Options
 */
#define XTI_DEBUG	0x0001	/* enable debugging */
#define XTI_LINGER	0x0080	/* linger on close if data present */
#define XTI_SNDBUF	0x1001	/* send buffer size */
#define XTI_RCVBUF	0x1002	/* receive buffer size */
#define XTI_SNDLOWAT	0x1003	/* send low-water mark */
#define XTI_RCVLOWAT	0x1004	/* receive low-water mark */

/*
 * Additional extension XTI-level Options
 */
#define XTI_REUSEADDR	0x2001	/* reuse addresses */
#define XTI_DONTROUTE	0x2002	/* do not route */
#define XTI_BROADCAST	0x2003	/* permit broadcast */
#define XTI_KEEPALIVE	0x2004	/* keep connections alive */
#define XTI_PRIORITY	0x2005	/* set connection priority */

/* 
 * Structure used with linger option.
 */
struct t_linger {
	t_scalar_t l_onoff;		/* option on/off */
	t_scalar_t l_linger;		/* linger time */
};

#endif				/* _SYS_XTI_XTI_H */
