/*****************************************************************************

 @(#) src/include/sys/snet/dl_control.h

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
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

 *****************************************************************************/

#ifndef __SYS_SNET_DL_CONTROL_H__
#define __SYS_SNET_DL_CONTROL_H__

#define DL_VERSION	2

#define DATAL_STAT	('E'<<8| 1)
#define DATAL_ZERO	('E'<<8| 2)
#define DATAL_REGISTER	('E'<<8| 3)
#define DATAL_GPARM	('E'<<8| 4)
#define DATAL_DEBUG	('E'<<8| 5)
#define DATAL_SET_ADDR	('E'<<8| 6)
#define DATAL_DFLT_ADDR	('E'<<8| 7)
#define DATAL_VERSION	('E'<<8| 8)

#define DATAL_RANGETAB	0x01

struct datal_stat {
	uint32_t dl_tx;
	uint32_t dl_rx;
	uint32_t dl_coll;
	uint32_t dl_lost;
	uint32_t dl_txerr;
	uint32_t dl_rxerr;
};

struct datal_register {
	uint8_t version;
	uint32_t mac_type;
	uint8_t addr_len;
	uint8_t align;
	uint16_t lwb;
	uint16_t upb;
};

struct datal_gparm {
	uint8_t version;
	uint32_t mac_type;
	uint8_t addr_len;
	uint8_t align;
	uint16_t frgsz;
	uint8_t addr[1];
};

struct datal_debug {
	int flags;
	int count;
	char space[1024];
};

#endif				/* __SYS_SNET_DL_CONTROL_H__ */
