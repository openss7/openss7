/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2011  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

#include <stropts.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef _GNU_SOURCE
#include <getopt.h>
#endif

#include <time.h>
#include <signal.h>
#include <syslog.h>
#include <sys/utsname.h>

#include <pcap/pcap.h>

int output = 1;
int debug = 0;
int lasterr = 0;

char inpfile[256] = "/dev/stdin";
char outfile[256] = "/dev/stdout";
char errfile[256] = "/dev/stderr";

char errbuf[PCAP_ERRBUF_SIZE] = "";

struct message {
	int eof;
	int valid;
	struct pcap_pkthdr *hdr;
	const u_char *dat;
} msgs;

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

#define PCAPNG_SHB_TYPE	0x0a0d0d0a
#define PCAPNG_IDB_TYPE	0x00000001
#define PCAPNG_OPB_TYPE 0x00000002
#define PCAPNG_SPB_TYPE 0x00000003
#define PCAPNG_NRB_TYPE 0x00000004
#define PCAPNG_ISB_TYPE 0x00000005
#define PCAPNG_EPB_TYPE 0x00000006
#define PCAPNG_ITB_TYPE 0x00000007
#define PCAPNG_AEB_TYPE 0x00000008

struct var {
	uint8_t *var_value;
	int var_length;
};

struct opt {
	uint16_t opt_code;
	uint16_t opt_len;
	uint32_t opt_value[0];
};

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

struct shb_info {
	uint16_t shbi_major;		// always 0x0001
	uint16_t shbi_minor;		// always 0x0000
	uint64_t shbi_section_length;	// 0xffffffffffffffff when unknown
	char *shbi_comment;
	char *shbi_hardware;
	char *shbi_os;
	uint8_t *shbi_uuid;		// len 16
};

struct idb_info {
	uint16_t idbi_linktype;
	uint32_t idbi_snaplen;
	char *idbi_comment;
	char *idbi_name;
	char *idbi_desc;
	uint8_t *idbi_ipv4addr;		// len 8
	uint8_t *idbi_ipv6addr;		// len 17
	uint8_t *idbi_macaddr;		// len 6
	uint8_t *idbi_euiaddr;		// len 8
	uint64_t idbi_speed;		// len 8
	uint8_t idbi_tsresol;		// len 1
	uint32_t idbi_tzone;		// len 4
	struct var idbi_filter;
	char *idbi_os;
	uint8_t idbi_fcslen;		// len 1
	uint32_t idbi_tsoffset[2];
	struct var idbi_physaddr;
	uint8_t *idbi_uuid;		// len 16
	uint32_t idbi_ifindex;		// len 4
};

struct epb_info {
	uint32_t epbi_interfaceid;
	uint32_t epbi_ts[2];
	uint32_t epbi_captlen;
	uint32_t epbi_packlen;
	struct var epbi_data;
	char *epbi_comment;
	uint32_t epbi_flags;		// len 4
	struct var epbi_hash;
	uint64_t epbi_dropcount;
	uint64_t epbi_compcount;
};

struct spb_info {
	uint32_t spbi_packlen;
	struct var spbi_data;
};

struct opb_info {
	uint16_t opbi_interfaceid;
	uint16_t opbi_drops;
	uint32_t opbi_ts[2];
	uint32_t opbi_captlen;
	uint32_t opbi_packlen;
	struct var opbi_data;
	char *opbi_comment;
	uint32_t opbi_flags;		// len 4
	struct var opbi_hash;
};

struct isb_info {
	uint32_t isbi_interfaceid;
	uint32_t isbi_ts[2];
	char *isbi_comment;
	uint32_t isbi_starttime[2];
	uint32_t isbi_endtime[2];
	uint64_t isbi_ifrecv;		// len 8
	uint64_t isbi_ifdrop;		// len 8
	uint64_t isbi_filteraccept;	// len 8
	uint64_t isbi_osdrop;		// len 8
	uint64_t isbi_usrdeliv;		// len 8
	uint64_t isbi_ifcomp;		// len 8
};

static size_t
size_shb(struct shb_info *shbi)
{
	size_t size = 0;
	int len;

	size += 8;		// header
	size += 4;		// magic
	size += 2;		// major
	size += 2;		// minor
	size += 8;		// section length
	if (shbi->shbi_comment != NULL) {
		if ((len = strnlen(shbi->shbi_comment, 128))) {
			len = (len + 3) & ~0x3;
			size += 4 + len;
		}
	}
	if (shbi->shbi_hardware != NULL) {
		if ((len = strnlen(shbi->shbi_hardware, 128))) {
			len = (len + 3) & ~0x3;
			size += 4 + len;
		}
	}
	if (shbi->shbi_os != NULL) {
		if ((len = strnlen(shbi->shbi_os, 128))) {
			len = (len + 3) & ~0x3;
			size += 4 + len;
		}
	}
	if (shbi->shbi_uuid != NULL) {
		size += 4 + 16;
	}
	size += 4;		// end of options
	size += 4;		// trailer
	return size;
}

static size_t
size_idb(struct idb_info *idbi)
{
	size_t size = 0;
	int len;

	size += 8;		// header
	size += 2;		// linktype
	size += 2;		// reserved
	size += 4;		// snaplen
	if (idbi->idbi_comment != NULL) {
		if ((len = strnlen(idbi->idbi_comment, 128))) {
			len = (len + 3) & ~0x3;
			size += 4 + len;
		}
	}
	if (idbi->idbi_name != NULL) {
		if ((len = strnlen(idbi->idbi_name, 128))) {
			len = (len + 3) & ~0x3;
			size += 4 + len;
		}
	}
	if (idbi->idbi_desc != NULL) {
		if ((len = strnlen(idbi->idbi_desc, 128))) {
			len = (len + 3) & ~0x3;
			size += 4 + len;
		}
	}
	if (idbi->idbi_ipv4addr != NULL) {
		size += 4 + 8;
	}
	if (idbi->idbi_ipv6addr != NULL) {
		size += 4 + 20;
	}
	if (idbi->idbi_macaddr != NULL) {
		size += 4 + 8;
	}
	if (idbi->idbi_euiaddr != NULL) {
		size += 4 + 8;
	}
	if (idbi->idbi_speed != 0) {
		size += 4 + 8;
	}
	if (idbi->idbi_tsresol != 0) {
		size += 4 + 4;
	}
	if (idbi->idbi_tzone != 0) {
		size += 4 + 4;
	}
	if (idbi->idbi_filter.var_value != NULL) {
		if ((len = idbi->idbi_filter.var_length)) {
			len = (len + 3) & ~0x3;
			size += 4 + len;
		}
	}
	if (idbi->idbi_os != NULL) {
		if ((len = strnlen(idbi->idbi_os, 128))) {
			len = (len + 3) & ~0x3;
			size += 4 + len;
		}
	}
	if (idbi->idbi_fcslen != 0) {
		size += 4 + 4;
	}
	if (idbi->idbi_tsoffset[0] != 0 || idbi->idbi_tsoffset[1] != 0) {
		size += 4 + 8;
	}
	if (idbi->idbi_physaddr != NULL) {
		if ((len = strnlen(idbi->idbi_physaddr, 128))) {
			len = (len + 3) & ~0x3;
			size += 4 + len;
		}
	}
	if (idbi->idbi_uuid != NULL) {
		size += 4 + 16;
	}
	if (idbi->idbi_ifindex != 0) {
		size += 4 + 4;
	}
	size += 4;		// end of options
	size += 4;		// trailer
}

static size_t
size_epb(struct epb_info *epbi)
{
	size_t size = 0;
	int len;

	size += 8;		// header
	size += 4;		// interfaceid
	size += 8;		// ts
	size += 4;		// captlen
	size += 4;		// packlen
	if (epbi->epbi_data.var_value != NULL) {
		if ((len = epbi->epbi_data.var_length)) {
			len = (len + 3) & ~0x3;
			size += len;
		}
	}
	if (epbi->epbi_comment != NULL) {
		if ((len = strnlen(epbi->epbi_comment, 128))) {
			len = (len + 3) & ~0x3;
			size += 4 + len;
		}
	}
	if (epbi->epbi_flags != 0) {
		size += 4 + 4;
	}
	if (epbi->epbi_hash.var_value != NULL) {
		if ((len = epbi->epbi_hash.var_length)) {
			len = (len + 3) & ~0x3;
			size += 4 + len;
		}
	}
	if (epbi->epbi_dropcount != 0) {
		size += 4 + 8;
	}
	if (epbi->epbi_compcount != 0) {
		size += 4 + 8;
	}
	size += 4;		// end of options
	size += 4;		// trailer
	return size;
}

static size_t
size_spb(struct spb_info *spbi)
{
	size_t size = 0;
	int len;

	size += 8;		// header
	size += 4;		// packlen
	if (spbi->spbi_data.var_value != NULL) {
		if ((len = spbi->spbi_data.var_length)) {
			len = (len + 3) & ~0x3;
			size += len;
		}
	}
	size += 4;		// trailer
	return size;
}

static size_t
size_opb(struct opb_info *opbi)
{
	size_t size = 0;
	int len;

	size += 8;		// header
	size += 2;		// interfaceid
	size += 2;		// drops
	size += 8;		// ts
	size += 4;		// captlen
	size += 4;		// packlen
	if (opbi->opbi_data.var_value != NULL) {
		if ((len = opbi->opbi_data.var_length)) {
			len = (len + 3) & ~0x3;
			size += len;
		}
	}
	if (opbi->opbi_comment != NULL) {
		if ((len = strnlen(opbi->opbi_comment, 128))) {
			len = (len + 3) & ~0x3;
			size += 4 + len;
		}
	}
	if (opbi->opbi_flags != 0) {
		size += 4 + 4;
	}
	if (opbi->opbi_hash.var_value != NULL) {
		if ((len = opbi->opbi_hash.var_length)) {
			len = (len + 3) & ~0x3;
			size += 4 + len;
		}
	}
	size += 4;		// end of options
	size += 4;		// trailer
	return size;
}

static size_t
size_isb(struct isb_info *isbi)
{
	size_t size = 0;
	int len;

	size += 8;		// header
	size += 4;		// interfaceid
	size += 8;		// ts
	if (isbi->isbi_comment != NULL) {
		if ((len = strnlen(isbi->isbi_comment, 128))) {
			len = (len + 3) & ~0x3;
			size += 4 + len;
		}
	}
	if (isbi->isbi_starttime[0] != 0 || isbi->isbi_starttime[1] != 0) {
		size += 4 + 8;
	}
	if (isbi->isbi_endtime[0] != 0 || isbi->isbi_endtime[1] != 0) {
		size += 4 + 8;
	}
	if (isbi->isbi_ifrecv != 0) {
		size += 4 + 8;
	}
	if (isbi->isbi_ifdrop != 0) {
		size += 4 + 8;
	}
	if (isbi->isbi_filteraccept != 0) {
		size += 4 + 8;
	}
	if (isbi->isbi_osdrop != 0) {
		size += 4 + 8;
	}
	if (isbi->isbi_usrdeliv != 0) {
		size += 4 + 8;
	}
	if (isbi->isbi_ifcomp != 0) {
		size += 4 + 8;
	}
	size += 4;		// end of options
	size += 4;		// trailer
	return size;
}

static void
write_shb(FILE *f, struct shb_info *shbi)
{
	size_t size;
	uint8_t *buf;
	struct shb *shb;
	struct opt *opt;
	int len;

	size = size_shb(shbi);
	buf = (uint8_t *)malloc(size);
	memset(buf,0,size);
	shb = (struct shb *)buf;
	shb->shb_type = 0x0A0D0D0A;
	shb->shb_length = size;
	shb->shb_magic = 0x1A2B3C4D;
	shb->shb_major = 1;
	shb->shb_minor = 0;
	shb->shb_section_length = 0xffffffffffffffff;
	opt = shb->shb_options;
	if (shbi->shbi_comment != NULL) {
		if ((len = strnlen(shbi->shbi_comment,128))) {
			opt->opt_code = OPT_COMMENT;
			opt->opt_len = len;
			memcpy(opt->opt_value,shbi->shbi_comment,len);
			len = (4 + (len + 3)) >> 2;
			opt += len;
		}
	}
	if (shbi->shbi_hardware != NULL) {
		if ((len = strnlen(shbi->shbi_hardware,128))) {
			opt->opt_code = SHB_HARDWARE;
			opt->opt_len = len;
			memcpy(opt->opt_value,shbi->shbi_hardware,len);
			len = (4 + (len + 3)) >> 2;
			opt += len;
		}
	}
	if (shbi->shbi_os != NULL) {
		if ((len = strnlen(shbi->shbi_os,128))) {
			opt->opt_code = SHB_OS;
			opt->opt_len = len;
			memcpy(opt->opt_value,shbi->shbi_os,len);
			len = (4 + (len + 3)) >> 2;
			opt += len;
		}
	}
	if (shbi->shbi_uuid != NULL) {
		opt->opt_code = SHB_UUID;
		opt->opt_len = 16;
		memcpy(opt->opt_value,shbi->shbi_uuid,16);
		len = (4 + (16 + 3)) >> 2;
		opt += len;
	}
	opt->opt_code = 0;
	opt->opt_len = 0;
	opt += 1;
	*(uint32_t *)opt = size;
	if (fwrite(buf,size,1,f) < 0) {
		perror(__FUNCTION__);
		exit(1);
	}
}

static void
write_idb(FILE *f, struct idb_info *idbi)
{
	size_t size;
	uint8_t *buf;
	struct shb *shb;
	struct opt *opt;
	int len;

	size = size_idb(idbi);
	buf = (uint8_t *)malloc(size);
	memset(buf,0,size);

	if (fwrite(buf,size,1,f) < 0) {
		perror(__FUNCTION__);
		exit(1);
	}
}

static void
write_epb(FILE *f, struct epb_info *epbi)
{
	size_t size;
	uint8_t *buf;
	struct shb *shb;
	struct opt *opt;
	int len;

	size = size_epb(epbi);
	buf = (uint8_t *)malloc(size);
	memset(buf,0,size);

	if (fwrite(buf,size,1,f) < 0) {
		perror(__FUNCTION__);
		exit(1);
	}
}

static void
write_spb(FILE *f, struct spb_info *spbi)
{
	size_t size;
	uint8_t *buf;
	struct shb *shb;
	struct opt *opt;
	int len;

	size = size_spb(spbi);
	buf = (uint8_t *)malloc(size);
	memset(buf,0,size);

	if (fwrite(buf,size,1,f) < 0) {
		perror(__FUNCTION__);
		exit(1);
	}
}

static void
write_opb(FILE *f, struct opb_info *opbi)
{
	size_t size;
	uint8_t *buf;
	struct shb *shb;
	struct opt *opt;
	int len;

	size = size_opb(opbi);
	buf = (uint8_t *)malloc(size);
	memset(buf,0,size);

	if (fwrite(buf,size,1,f) < 0) {
		perror(__FUNCTION__);
		exit(1);
	}
}

static void
write_isb(FILE *f, struct isb_info *isbi)
{
	size_t size;
	uint8_t *buf;
	struct shb *shb;
	struct opt *opt;
	int len;

	size = size_isb(isbi);
	buf = (uint8_t *)malloc(size);
	memset(buf,0,size);

	if (fwrite(buf,size,1,f) < 0) {
		perror(__FUNCTION__);
		exit(1);
	}
}

static void
pcap2ng(void)
{
	pcap_t *p;
	pcap_dumper_t *pd;
	FILE *f;
	pcap_t *pi;

	// open input file
	if (debug)
		fprintf(stderr, "opening input file %s\n", inpfile);
	if ((f = fopen(inpfile, "r")) == NULL) {
		perror(__FUNCTION__);
		exit(1);
	}
	if (debug)
		fprintf(stderr, "opening pcap file %s\n", inpfile);
	if ((pi = pcap_fopen_offline(f, errbuf)) == NULL) {
		fprintf(stderr, "error: %s: %s\n", __FUNCTION__, errbuf);
		exit(1);
	}
	msgs.eof = 0;
	msgs.valid = 0;
	msgs.hdr = NULL;
	msgs.dat = NULL;
	if (debug)
		fprintf(stderr, "opening output file %s\n", outfile);
	if (strncmp(outfile, "/dev/stdout", sizeof(outfile) - 1) != 0) {
		// redirect standard output
		if (freopen(outfile, "w", stdout) == NULL) {
			perror(__FUNCTION__);
			exit(1);
		}
	}
	if (debug)
		fprintf(stderr, "opening error file %s\n", errfile);
	if (strncmp(errfile, "/dev/stderr", sizeof(errfile) - 1) != 0) {
		// redirect standard error
		if (freopen(errfile, "w", stderr) == NULL) {
			perror(__FUNCTION__);
			exit(1);
		}
	}
	// FIXME: open pcapng file
	for (;;) {
		int rtn;
		u_char *psu;

		if (debug > 3)
			fprintf(stderr, "reading messages from input file\n");
		rtn = pcap_next_ext(pi, &msgs.hdr, &msgs.dat);
		msgs.eof = (rtn == -2);
		if (msgs.eof) {
			if (debug)
				fprintf(stderr, "hit end of input file\n");
			break;
		}
		msgs.valid = (rtn == 1);
		if (!msgs.valid) {
			if (debug > 3)
				fprintf(stderr, "error reading message from input file\n");
			break;
		}
		psu = (u_char *) msgs.dat;
		{
			int card, span, slot, dir;

			// our link numbers are 2 bits for card number, 2 bits for span on card,
			// and 5 bits
			// for slot in span Note that a slot number of zero means entire span
			// (Annex A).
			card = ((psu[MTP2_LINK_NUMBER_OFFSET] & 0x01) << 1) |
			    ((psu[MTP2_LINK_NUMBER_OFFSET + 1] & 0x80) >> 7);
			span = ((psu[MTP2_LINK_NUMBER_OFFSET + 1] & 0x60) >> 5);
			slot = ((psu[MTP2_LINK_NUMBER_OFFSET + 1] & 0x1f) >> 0);

			dir = span & 0x1;
			span &= ~0x1;

			psu[MTP2_SENT_OFFSET] = dir;
			psu[MTP2_LINK_NUMBER_OFFSET] = (card >> 1) & 0x1;
			psu[MTP2_LINK_NUMBER_OFFSET + 1] =
			    ((card & 0x1) << 7) | ((span & 0x3) << 5) | (slot & 0x1f);
		}
		// write it out
	}
}

static void
copying(int argc, char *argv[])
{
	if (!output && !debug)
		return;
	(void) fprintf(stdout, "\
--------------------------------------------------------------------------------\n\
%1$s\n\
--------------------------------------------------------------------------------\n\
Copyright (c) 2008-2012  Monavacon Limited <http://www.monavacon.com/>\n\
Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>\n\
Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>\n\
\n\
All Rights Reserved.\n\
--------------------------------------------------------------------------------\n\
This program is free software: you can  redistribute it  and/or modify  it under\n\
the terms of the  GNU Affero  General  Public  License  as published by the Free\n\
Software Foundation, version 3 of the license.\n\
\n\
This program is distributed in the hope that it will  be useful, but WITHOUT ANY\n\
WARRANTY; without even  the implied warranty of MERCHANTABILITY or FITNESS FOR A\n\
PARTICULAR PURPOSE.  See the GNU Affero General Public License for more details.\n\
\n\
You should have received a copy of the  GNU Affero General Public License  along\n\
with this program.   If not, see <http://www.gnu.org/licenses/>, or write to the\n\
Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.\n\
--------------------------------------------------------------------------------\n\
U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on behalf\n\
of the U.S. Government (\"Government\"), the following provisions apply to you. If\n\
the Software is supplied by the Department of Defense (\"DoD\"), it is classified\n\
as \"Commercial  Computer  Software\"  under  paragraph  252.227-7014  of the  DoD\n\
Supplement  to the  Federal Acquisition Regulations  (\"DFARS\") (or any successor\n\
regulations) and the  Government  is acquiring  only the  license rights granted\n\
herein (the license rights customarily provided to non-Government users). If the\n\
Software is supplied to any unit or agency of the Government  other than DoD, it\n\
is  classified as  \"Restricted Computer Software\" and the Government's rights in\n\
the Software  are defined  in  paragraph 52.227-19  of the  Federal  Acquisition\n\
Regulations (\"FAR\")  (or any successor regulations) or, in the cases of NASA, in\n\
paragraph  18.52.227-86 of  the  NASA  Supplement  to the FAR (or any  successor\n\
regulations).\n\
--------------------------------------------------------------------------------\n\
Commercial  licensing  and  support of this  software is  available from OpenSS7\n\
Corporation at a fee.  See http://www.openss7.com/\n\
--------------------------------------------------------------------------------\n\
", ident);
}

static void
version(int argc, char *argv[])
{
	if (!output && !debug)
		return;
	(void) fprintf(stdout, "\
%1$s (OpenSS7 %2$s) %3$s (%4$s)\n\
Written by Brian Bidulock.\n\
\n\
Copyright (c) 2008, 2009, 2010, 2011, 2012  Monavacon Limited.\n\
Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008  OpenSS7 Corporation.\n\
Copyright (c) 1997, 1998, 1999, 2000, 2001  Brian F. G. Bidulock.\n\
This is free software; see the source for copying conditions.  There is NO\n\
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\
\n\
Distributed by OpenSS7 under GNU Affero General Public License Version 3,\n\
with conditions, incorporated herein by reference.\n\
\n\
See `%1$s --copying' for copying permissions.\n\
", NAME, PACKAGE, VERSION, "$Revision: 1.1.2.4 $ $Date: 2011-05-31 09:46:19 $");
}

static void
usage(int argc, char *argv[])
{
	if (!output && !debug)
		return;
	(void) fprintf(stderr, "\
Usage:\n\
    %1$s [options] [{-o|--outfile} outfile] [{-e|--errfile} errfile] [infile]\n\
    %1$s {-h|--help}\n\
    %1$s {-V|--version}\n\
    %1$s {-C|--copying}\n\
", argv[0]);
}

static void
help(int argc, char *argv[])
{
	if (!output && !debug)
		return;
	(void) fprintf(stdout, "\
Usage:\n\
    %1$s [options] [{-o|--outfile} outfile] [{-e|--errfile} errfile] [infile]\n\
    %1$s {-h|--help}\n\
    %1$s {-V|--version}\n\
    %1$s {-C|--copying}\n\
Arguments:\n\
    infile                              (default: %2$s)\n\
        input file to read pcap formatted data\n\
Options:\n\
  Direction Options:\n\
    -2, --settotwo                      (default: %7$d)\n\
        set direction in MTP2 pseudo-header to 2\n\
    -3, --settothr                      (default: %7$d)\n\
        set direction in MTP2 pseudo-header to 3\n\
    -4, --settofor                      (default: %7$d)\n\
        set direction in MTP2 pseudo-header to 4\n\
    -d, --settodir                      (default: %8$d)\n\
        set direction in MTP2 pseudo-header to span\n\
        and merge spans\n\
  File Options:\n\
    -o, --outfile outfile               (default: %3$s)\n\
        output file to write pcap formatted data\n\
    -e, --errfile errfile               (default: %4$s)\n\
        error log file to which to write errors\n\
  General Options:\n\
    -q, --quiet                         (default: off)\n\
        suppress normal output\n\
    -D, --debug [LEVEL]                 (default: %5$d)\n\
        increment or set debug LEVEL\n\
    -v, --verbose [LEVEL]               (default: %6$d)\n\
        increment or set verbosity LEVEL\n\
    -h, --help\n\
        prints this usage information and exits\n\
    -V, --version\n\
        print version and exit\n\
    -C, --copying\n\
        print copying permission and exit\n\
", argv[0], inpfile, outfile, errfile, debug, output, settotwo, settodir);
}

int
main(int argc, char **argv)
{
	int c, val;

	while (1) {
#if defined _GNU_SOURCE
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{"settotwo",	no_argument,		NULL,	'2'},
			{"settothr",	no_argument,		NULL,	'3'},
			{"settofor",	no_argument,		NULL,	'4'},
			{"settodir",	no_argument,		NULL,	'd'},
			{"outfile",	required_argument,	NULL,	'o'},
			{"errfile",	required_argument,	NULL,	'e'},
			{"quiet",	no_argument,		NULL,	'q'},
			{"debug",	optional_argument,	NULL,	'D'},
			{"verbose",	optional_argument,	NULL,	'v'},
			{"help",	no_argument,		NULL,	'h'},
			{"version",	no_argument,		NULL,	'V'},
			{"copying",	no_argument,		NULL,	'C'},
			{"?",		no_argument,		NULL,	'H'},
			{NULL,		0,			NULL,	 0 }

		};
		/* *INDENT-ON* */

		c = getopt_long(argc, argv, "234do:e:qD::v::hVC?W:", long_options, &option_index);
#else				/* defined _GNU_SOURCE */
		c = getopt(argc, argv, "234do:e:qDvhVC?");
#endif				/* defined _GNU_SOURCE */
		if (c == -1) {
			break;
		}
		switch (c) {
		case 0:
			usage(argc, argv);
			exit(2);
		case '2':	/* -2, --settotwo */
			settotwo = 2;
			settodir = 0;
		case '3':	/* -3, --settothr */
			settotwo = 3;
			settodir = 0;
		case '4':	/* -4, --settofor */
			settotwo = 4;
			settodir = 0;
			break;
		case 'd':	/* -d, --settodir */
			settodir = 1;
			settotwo = 0;
			break;
		case 'o':	/* -o, --outfile outfile */
			if (debug)
				fprintf(stderr, "%s: setting outfile to %s\n", argv[0], optarg);
			strncpy(outfile, optarg, sizeof(outfile) - 1);
			break;
		case 'e':	/* -e, --errfile errfile */
			if (debug)
				fprintf(stderr, "%s: setting errfile to %s\n", argv[0], optarg);
			strncpy(errfile, optarg, sizeof(errfile) - 1);
			break;
		case 'q':	/* -q, --quiet */
			if (debug)
				fprintf(stderr, "%s: suppressing normal output\n", argv[0]);
			output = 0;
			debug = 0;
			break;
		case 'D':	/* -D, --debug [LEVEL] */
			if (debug)
				fprintf(stderr, "%s: increasing debug verbosity\n", argv[0]);
			if (optarg == NULL) {
				debug += 1;
				break;
			}
			if ((val = strtol(optarg, NULL, 0)) < 0)
				goto bad_option;
			debug = val;
			break;
		case 'v':	/* -v, --verbose */
			if (debug)
				fprintf(stderr, "%s: increasing output verbosity\n", argv[0]);
			if (optarg == NULL) {
				output += 1;
				break;
			}
			if ((val = strtol(optarg, NULL, 0)) < 0)
				goto bad_option;
			output = val;
			break;
		case 'h':	/* -h, --help */
		case 'H':	/* -H, --? */
			if (debug)
				fprintf(stderr, "%s: printing help message\n", argv[0]);
			help(argc, argv);
			exit(0);
		case 'V':	/* -V, --version */
			if (debug)
				fprintf(stderr, "%s: printing version message\n", argv[0]);
			version(argc, argv);
			exit(0);
		case 'C':	/* -C, --copying */
			if (debug)
				fprintf(stderr, "%s: printing copying message\n", argv[0]);
			copying(argc, argv);
			exit(0);
		case '?':
		default:
		      bad_option:
			optind--;
			goto bad_nonopt;
		      bad_nonopt:
			if (output || debug) {
				if (optind < argc) {
					fprintf(stderr, "%s: illegal syntax -- ", argv[0]);
					while (optind < argc)
						fprintf(stderr, "%s ", argv[optind++]);
					fprintf(stderr, "\n");
				} else {
					fprintf(stderr, "%s: missing option or argument", argv[0]);
					fprintf(stderr, "\n");
				}
				fflush(stderr);
				goto bad_usage;
			      bad_usage:
				usage(argc, argv);
			}
			exit(2);
		}
	}
	if (optind < argc) {
		infiles = 0;
		for (; optind < argc && infiles < 1; optind++, infiles++) {
			if (debug)
				fprintf(stderr, "%s: assigning input file to %s\n", argv[0],
					argv[optind]);
			strncpy(inpfile, argv[optind], sizeof(inpfile) - 1);
		}
		if (debug) {
			int i;

			for (i = 0; i < infiles; i++)
				fprintf(stderr, "%s: input file was assigned %s\n", argv[0],
					inpfile);
		}
	}
	ss7capsetdir();
	exit(4);
}
