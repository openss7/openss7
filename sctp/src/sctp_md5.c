/*****************************************************************************

 @(#) $RCSfile: sctp_md5.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2006/10/27 22:44:37 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2006/10/27 22:44:37 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: sctp_md5.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2006/10/27 22:44:37 $"

static char const ident[] =
    "$RCSfile: sctp_md5.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2006/10/27 22:44:37 $";

#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/sysctl.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/random.h>
#include <linux/init.h>
#include <linux/socket.h>
#include <net/sock.h>
#include <linux/ipsec.h>
#include <linux/poll.h>
#include <linux/slab.h>
#include <linux/mm.h>

#ifdef CONFIG_SCTP_MODULE
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <net/protocol.h>
#endif

#include <net/inet_common.h>
#ifdef HAVE_KINC_NET_XFRM_H
#include <net/xfrm.h>
#endif
#include <net/icmp.h>
#ifdef HAVE_KINC_NET_DST_H
#include <net/dst.h>
#endif
#include <net/ip.h>
#ifdef	CONFIG_IP_MASQUERADE
#include <net/ip_masq.h>
#endif
#ifdef	CONFIG_SCTP_ECN
#include <net/inet_ecn.h>
#endif

#include <asm/uaccess.h>
#include <asm/segment.h>

#include <linux/inet.h>
#include <linux/stddef.h>
#include <linux/string.h>
#include <asm/types.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>

#include "sctp_debug.h"
#include "sctp_md5.h"

/*
 *  =========================================================================
 *
 *  MD5
 *
 *  =========================================================================
 */

/* The four core functions - F1 is optimized somewhat */
/* #define F1(x, y, z) (x & y | ~x & z) */
#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))
/* This is the central step in the MD5 algorithm. */
#define MD5STEP(f,w,x,y,z,dat,s) \
	 (w += f(x,y,z) + dat, w = (w<<s | w>>(32-s)) + x)
/*
 *  The core of the MD5 algorithm, this alters an existing MD5 hash to reflect
 *  the addition of 16 longwords of new data.  MD5Update blocks the data and
 *  converts bytes into longwords for this routine.
 */
STATIC void
MD5Transform(uint32_t dig[4], uint32_t const dat[16])
{
	register uint32_t a, b, c, d;

	a = dig[0];
	b = dig[1];
	c = dig[2];
	d = dig[3];
	MD5STEP(F1, a, b, c, d, dat[0] + 0xd76aa478, 7);
	MD5STEP(F1, d, a, b, c, dat[1] + 0xe8c7b756, 12);
	MD5STEP(F1, c, d, a, b, dat[2] + 0x242070db, 17);
	MD5STEP(F1, b, c, d, a, dat[3] + 0xc1bdceee, 22);
	MD5STEP(F1, a, b, c, d, dat[4] + 0xf57c0faf, 7);
	MD5STEP(F1, d, a, b, c, dat[5] + 0x4787c62a, 12);
	MD5STEP(F1, c, d, a, b, dat[6] + 0xa8304613, 17);
	MD5STEP(F1, b, c, d, a, dat[7] + 0xfd469501, 22);
	MD5STEP(F1, a, b, c, d, dat[8] + 0x698098d8, 7);
	MD5STEP(F1, d, a, b, c, dat[9] + 0x8b44f7af, 12);
	MD5STEP(F1, c, d, a, b, dat[10] + 0xffff5bb1, 17);
	MD5STEP(F1, b, c, d, a, dat[11] + 0x895cd7be, 22);
	MD5STEP(F1, a, b, c, d, dat[12] + 0x6b901122, 7);
	MD5STEP(F1, d, a, b, c, dat[13] + 0xfd987193, 12);
	MD5STEP(F1, c, d, a, b, dat[14] + 0xa679438e, 17);
	MD5STEP(F1, b, c, d, a, dat[15] + 0x49b40821, 22);
	MD5STEP(F2, a, b, c, d, dat[1] + 0xf61e2562, 5);
	MD5STEP(F2, d, a, b, c, dat[6] + 0xc040b340, 9);
	MD5STEP(F2, c, d, a, b, dat[11] + 0x265e5a51, 14);
	MD5STEP(F2, b, c, d, a, dat[0] + 0xe9b6c7aa, 20);
	MD5STEP(F2, a, b, c, d, dat[5] + 0xd62f105d, 5);
	MD5STEP(F2, d, a, b, c, dat[10] + 0x02441453, 9);
	MD5STEP(F2, c, d, a, b, dat[15] + 0xd8a1e681, 14);
	MD5STEP(F2, b, c, d, a, dat[4] + 0xe7d3fbc8, 20);
	MD5STEP(F2, a, b, c, d, dat[9] + 0x21e1cde6, 5);
	MD5STEP(F2, d, a, b, c, dat[14] + 0xc33707d6, 9);
	MD5STEP(F2, c, d, a, b, dat[3] + 0xf4d50d87, 14);
	MD5STEP(F2, b, c, d, a, dat[8] + 0x455a14ed, 20);
	MD5STEP(F2, a, b, c, d, dat[13] + 0xa9e3e905, 5);
	MD5STEP(F2, d, a, b, c, dat[2] + 0xfcefa3f8, 9);
	MD5STEP(F2, c, d, a, b, dat[7] + 0x676f02d9, 14);
	MD5STEP(F2, b, c, d, a, dat[12] + 0x8d2a4c8a, 20);
	MD5STEP(F3, a, b, c, d, dat[5] + 0xfffa3942, 4);
	MD5STEP(F3, d, a, b, c, dat[8] + 0x8771f681, 11);
	MD5STEP(F3, c, d, a, b, dat[11] + 0x6d9d6122, 16);
	MD5STEP(F3, b, c, d, a, dat[14] + 0xfde5380c, 23);
	MD5STEP(F3, a, b, c, d, dat[1] + 0xa4beea44, 4);
	MD5STEP(F3, d, a, b, c, dat[4] + 0x4bdecfa9, 11);
	MD5STEP(F3, c, d, a, b, dat[7] + 0xf6bb4b60, 16);
	MD5STEP(F3, b, c, d, a, dat[10] + 0xbebfbc70, 23);
	MD5STEP(F3, a, b, c, d, dat[13] + 0x289b7ec6, 4);
	MD5STEP(F3, d, a, b, c, dat[0] + 0xeaa127fa, 11);
	MD5STEP(F3, c, d, a, b, dat[3] + 0xd4ef3085, 16);
	MD5STEP(F3, b, c, d, a, dat[6] + 0x04881d05, 23);
	MD5STEP(F3, a, b, c, d, dat[9] + 0xd9d4d039, 4);
	MD5STEP(F3, d, a, b, c, dat[12] + 0xe6db99e5, 11);
	MD5STEP(F3, c, d, a, b, dat[15] + 0x1fa27cf8, 16);
	MD5STEP(F3, b, c, d, a, dat[2] + 0xc4ac5665, 23);
	MD5STEP(F4, a, b, c, d, dat[0] + 0xf4292244, 6);
	MD5STEP(F4, d, a, b, c, dat[7] + 0x432aff97, 10);
	MD5STEP(F4, c, d, a, b, dat[14] + 0xab9423a7, 15);
	MD5STEP(F4, b, c, d, a, dat[5] + 0xfc93a039, 21);
	MD5STEP(F4, a, b, c, d, dat[12] + 0x655b59c3, 6);
	MD5STEP(F4, d, a, b, c, dat[3] + 0x8f0ccc92, 10);
	MD5STEP(F4, c, d, a, b, dat[10] + 0xffeff47d, 15);
	MD5STEP(F4, b, c, d, a, dat[1] + 0x85845dd1, 21);
	MD5STEP(F4, a, b, c, d, dat[8] + 0x6fa87e4f, 6);
	MD5STEP(F4, d, a, b, c, dat[15] + 0xfe2ce6e0, 10);
	MD5STEP(F4, c, d, a, b, dat[6] + 0xa3014314, 15);
	MD5STEP(F4, b, c, d, a, dat[13] + 0x4e0811a1, 21);
	MD5STEP(F4, a, b, c, d, dat[4] + 0xf7537e82, 6);
	MD5STEP(F4, d, a, b, c, dat[11] + 0xbd3af235, 10);
	MD5STEP(F4, c, d, a, b, dat[2] + 0x2ad7d2bb, 15);
	MD5STEP(F4, b, c, d, a, dat[9] + 0xeb86d391, 21);
	dig[0] += a;
	dig[1] += b;
	dig[2] += c;
	dig[3] += d;
}

#ifdef __BIG_ENDIAN
STATIC void
byteSwap(uint32_t *buf, unsigned cnt)
{
	uint8_t *p = (uint8_t *) buf;

	do {
		*buf++ = (uint32_t) ((unsigned) p[3] << 8 | p[2]) << 16 | ((unsigned)
									   p[1] << 8 | p[0]);
		p += 4;
	} while (--cnt);
}
#else
#define byteSwap(__buf,__cnt)
#endif
/*
 * Start MD5 accumulation.  Set bit count to 0 and buffer to mysterious
 * initialization constants.
 */
void
MD5Init(MD5_CTX * md5)
{
	md5->buf[0] = 0x67452301;
	md5->buf[1] = 0xefcdab89;
	md5->buf[2] = 0x98badcfe;
	md5->buf[3] = 0x10325476;
	md5->lo = 0;
	md5->hi = 0;
}

/*
 * Update context to reflect the concatenation of another buffer full
 * of bytes.
 */
void
MD5Update(MD5_CTX * md5, uint8_t const *buf, unsigned len)
{
	uint32_t t;

	/* Update byte count */
	t = md5->lo;
	if ((md5->lo = t + len) < t)
		md5->hi++;	/* Carry from low to high */
	t = 64 - (t & 0x3f);	/* Space available in md5->dat (at least 1) */
	if (t > len) {
		memcpy((uint8_t *) md5->dat + 64 - t, buf, len);
		return;
	}
	/* First chunk is an odd size */
	memcpy((uint8_t *) md5->dat + 64 - t, buf, t);
	byteSwap(md5->dat, 16);
	MD5Transform(md5->buf, md5->dat);
	buf += t;
	len -= t;
	/* Process data in 64-byte chunks */
	while (len >= 64) {
		memcpy(md5->dat, buf, 64);
		byteSwap(md5->dat, 16);
		MD5Transform(md5->buf, md5->dat);
		buf += 64;
		len -= 64;
	}
	/* Handle any remaining bytes of data. */
	memcpy(md5->dat, buf, len);
}

/*
 * Final wrapup - pad to 64-byte boundary with the bit pattern 
 * 1 0* (64-bit count of bits processed, MSB-first)
 */
void
MD5Final(uint8_t dig[16], MD5_CTX * md5)
{
	int count = md5->lo & 0x3f;	/* Number of bytes in md5->dat */
	uint8_t *p = (uint8_t *) md5->dat + count;

	/* Set the first char of padding to 0x80.  There is always room. */
	*p++ = 0x80;
	/* Bytes of padding needed to make 56 bytes (-8..55) */
	count = 56 - 1 - count;
	if (count < 0) {	/* Padding forces an extra block */
		memset(p, 0, count + 8);
		byteSwap(md5->dat, 16);
		MD5Transform(md5->buf, md5->dat);
		p = (uint8_t *) md5->dat;
		count = 56;
	}
	memset(p, 0, count);
	byteSwap(md5->dat, 14);
	/* Append length in bits and transform */
	md5->dat[14] = md5->lo << 3;
	md5->dat[15] = md5->hi << 3 | md5->lo >> 29;
	MD5Transform(md5->buf, md5->dat);
	byteSwap(md5->buf, 4);
	memcpy(dig, md5->buf, 16);
	memset(md5, 0, sizeof(*md5));	/* In case it's sensitive */
}
