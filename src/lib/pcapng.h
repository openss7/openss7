/*****************************************************************************

 @(#) src/lib/pcapng.h

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This library is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License as published by the Free
 Software Foundation; version 3 of the License.

 This library is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Lesser Public License for more
 details.

 You should have received a copy of the GNU Lesser General Public License
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

#ifndef __LOCAL_PCAPNG_H__
#define __LOCAL_PCAPNG_H__

#include <pcapng.h>

/* These definitions are private to the library implementation. */

#define PCAPNG_SHB_TYPE	0x0a0d0d0a
#define PCAPNG_IDB_TYPE	0x00000001
#define PCAPNG_OPB_TYPE 0x00000002
#define PCAPNG_SPB_TYPE 0x00000003
#define PCAPNG_NRB_TYPE 0x00000004
#define PCAPNG_ISB_TYPE 0x00000005
#define PCAPNG_EPB_TYPE 0x00000006
#define PCAPNG_ITB_TYPE 0x00000007
#define PCAPNG_AEB_TYPE 0x00000008

#define PCAPNG_MAGIC	0x1a2b3c4d

#define PCAPNG_MAXSTR	256

#define OPT_ENDOFOPT	 0	// len 0
#define OPT_COMMENT	 1	// variable

#define SHB_HARDWARE	 2	// variable
#define SHB_OS		 3	// variable
#define SHB_USERAPPL	 4	// variable
#define SHB_UUID	0x5572	// len 16

#define IDB_NAME	 2	// variable
#define IDB_DESCRIPTION	 3	// variable
#define IDB_IPV4ADDR	 4	// len 8 (address/netmask)
#define IDB_IPV6ADDR	 5	// len 17 (address/prefixlen)
#define IDB_MACADDR	 6	// len 6
#define IDB_EUIADDR	 7	// len 8
#define IDB_SPEED	 8	// len 8
#define IDB_TSRESOL	 9	// len 1
#define IDB_TZONE	10	// len 4
#define IDB_FILTER	11	// variable
#define IDB_OS		12	// variable
#define IDB_FCSLEN	13	// len 1
#define IDB_TSOFFSET	14	// len 8 (high/low)
#define IDB_PHYSADDR	0x5572	// variable
#define IDB_UUID	0x5573	// len 16
#define IDB_IFINDEX	0x5574	// len 4

#define EPB_FLAGS	 2	// len 4
#define EPB_HASH	 3	// variable
#define EPB_DROPCOUNT	 4	// len 8
#define EPB_COMPCOUNT	0x5572	// len 8

#define OPB_FLAGS	 2	// len 4
#define OPB_HASH	 3	// variable

#define ISB_STARTTIME	 2	// len 8 (high/low)
#define ISB_ENDTIME	 3	// len 8 (high/low)
#define ISB_IFRECV	 4	// len 8
#define ISB_IFDROP	 5	// len 8
#define ISB_FILTERACCEPT 6	// len 8
#define ISB_OSDROP	 7	// len 8
#define ISB_USRDELIV	 8	// len 8
#define ISB_IFCOMP	0x5572	// len 8

struct shb {
	uint32_t shb_type;		// always 0x0A0D0D0A
	uint32_t shb_length;
	uint32_t shb_magic;
	uint16_t shb_major;
	uint16_t shb_minor;
	uint64_t shb_section_length;	// set to 0xffffffffffffffff if unknown
	struct opt shb_options[0];
	// note trailed by another uint32_t shb_length for reverse navigation
};

struct idb {
	uint32_t idb_type;		// always 0x00000001
	uint32_t idb_length;
	uint16_t idb_linktype;
	uint16_t idb_reserved;
	uint32_t idb_snaplen;
	struct opt idb_options[0];
	// note trailed by another uint32_t idb_length for reverse navigation
};

struct epb {
	uint32_t epb_type;		// always 0x00000006
	uint32_t epb_length;
	uint32_t epb_interfaceid;
	uint32_t epb_ts[2];
	uint32_t epb_captlen;
	uint32_t epb_packlen;
	uint32_t epb_data[0];		// packet data epb_captlen padded to 32-bits
	struct opt epb_options[0];
	// note trailed by another uint32_t epb_length for reverse navigation
};

struct spb {
	uint32_t spb_type;		// always 0x00000003
	uint32_t spb_length;
	uint32_t spb_packlen;
	uint32_t spb_data[0];		// packet data padded to 32-bits (use block length to
	// calculate size)
	// note trailed by another uint32_t spb_length for reverse navigation
};

struct opb {
	uint32_t opb_type;		// always 0x00000002
	uint32_t opb_length;
	uint16_t opb_interfaceid;
	uint16_t opb_drops;
	uint32_t opb_ts[2];
	uint32_t opb_captlen;
	uint32_t opb_packlen;
	uint32_t opb_data[0];		// packet data opb_captlen padded to 32-bits
	struct opt opb_options[0];
	// note trailed by another uint32_t opb_length for reverse navigation
};

struct isb {
	uint32_t isb_type;		// always 0x00000005
	uint32_t isb_length;
	uint32_t isb_interfaceid;
	uint32_t isb_ts[2];
	struct opt isb_options[0];
	// note trailed by another uint32_t isb_length for reverse navigation
};


#endif				/* __LOCAL_PCAPNG_H__ */

