/*****************************************************************************

 @(#) $Id: x32_control.h,v 1.1.2.3 2011-02-07 04:54:42 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2011  Monavacon Limited <http://www.monavacon.com/>
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

 Last Modified $Date: 2011-02-07 04:54:42 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: x32_control.h,v $
 Revision 1.1.2.3  2011-02-07 04:54:42  brian
 - code updates for new distro support

 Revision 1.1.2.2  2010-11-28 14:21:46  brian
 - remove #ident, protect _XOPEN_SOURCE

 Revision 1.1.2.1  2009-06-21 11:25:33  brian
 - added files to new distro

 *****************************************************************************/

#ifndef __SYS_SNET_X32_CONTROL_H__
#define __SYS_SNET_X32_CONTROL_H__

#define MAX_IDENTITY_LEN	32
#define MAX_SIGNATURE_LEN	32

#define PKT			 1
#define LNK			 2

#define NO_X32_CONF		 0
#define X32_NOT_REG		 1
#define X32_REG_FAILED		 2
#define X32_ID_SENT		 3
#define X32_CMD_CONF_PENDING	 4
#define X32_RSP_CONF_PENDING	 5
#define X32_REG_SUCCESS		 6

#define X32_MAP_SIZE   (MAXIOCBSZ - 8)
#define MAX_X32_ENTS   (X32_MAP_SIZE / sizeof(struct x32conff))

struct x32_facilities {
	uint8_t identity_len;		/* X.32 identity len */
	uint8_t signature_len;		/* X.32 sig. len */
	uint8_t x32_state;		/* X.32 state */
	uint8_t diagnostic;		/* X.32 diagnostic */
	uint8_t identity[MAX_IDENTITY_LEN];	/* X.32 identity */
	uint8_t signature[MAX_SIGNATURE_LEN];	/* X.32 signature */
};

struct x32conff {
	uint32_t sn_id;			/* Subnetwork */
	struct x32_facilities x32_facs;
};

struct x32mapf {
	struct x32conff entries[MAX_X32_ENTS];	/* Data buffer */
	uint32_t first_ent;		/* Where to start search */
	uint32_t num_ent;		/* Number entries returned */
};

struct reg_facilities {
	uint8_t has_x32;		/* Has X.32 facs */
	struct x32_facilities x32_facs;
};

struct xid_user_data {
	uint8_t has_x32;		/* Has X.32 facs */
	struct x32_facilities x32_facs;
};

#endif				/* __SYS_SNET_X32_CONTROL_H__ */
