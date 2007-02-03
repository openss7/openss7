/*****************************************************************************

 @(#) $Id: rmux.h,v 1.1.2.1 2007/02/03 03:05:22 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2007/02/03 03:05:22 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: rmux.h,v $
 Revision 1.1.2.1  2007/02/03 03:05:22  brian
 - added new files

 *****************************************************************************/

#ifndef __SYS_RMUX_H__
#define __SYS_RMUX_H__

#ident "@(#) $RCSfile: rmux.h,v $ $Name:  $($Revision: 1.1.2.1 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

/*
 *  R_OPEN_IND	- indicate a remote open request
 */
struct R_open_ind {
	int PRIM_type;			/* always R_OPEN_IND */
	int REMOTE_id;			/* remote system identifier */
	uint STREAM_id;			/* remote Stream identifier */
	uint PATH_length;		/* length of logical path */
	uint PATH_offset;		/* offset of logical path */
	uint OPEN_flags;		/* oflags argument */
	uint CRED_length;		/* length of credentials */
	uint CRED_offset;		/* offset of credentials */
};

/*
 *  R_OPEN_RES	- response to a remote open request
 */
struct R_open_res {
	int PRIM_type;			/* always R_OPEN_RES */
	int REMOTE_id;			/* remote system identifier */
	uint STREAM_id;			/* remote Stream identiifer */
	int MUX_id;			/* lower mux identifier for linked Stream */
	uint STREAMS_flags;		/* sflag used */
	uint ERROR_number;		/* error if non-zero */
};

#endif				/* __SYS_RMUX_H__ */
