/*****************************************************************************

 @(#) $RCSfile: sctp_sha1.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2006/10/27 23:10:56 $

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

 Last Modified $Date: 2006/10/27 23:10:56 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: sctp_sha1.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2006/10/27 23:10:56 $"

static char const ident[] =
    "$RCSfile: sctp_sha1.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2006/10/27 23:10:56 $";

#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/compiler.h>
#include <linux/types.h>

#include "sctp_sha1.h"

/* 
 *  =========================================================================
 *
 *  SHA-1
 *
 *  =========================================================================
 *  
 *  This is a GPL version fo the FIPS 180-1 Secure Hash Algorithm written by
 *  Brian Bidulock <bidulock@openss7.org>.
 *
 *  Adapted from code written November 2000 by David Ireland of DI Management
 *  Services Pty Limited <code@di-mgt.com.au>
 *  
 *  Adapted from code in the Python Cryptography Toolkit, version 1.0.0 by
 *  A.M.  Kuchling 1995.
 */

/* 
 *  The SHS f()-functions.  The f1 and f3 functions can be optimized to save one
 *  boolean operation each - thanks to Rich Schroeppel, rcs@cs.arizona.edu for
 *  discovering this
 */
#define f1(x,y,z)   (z^(x&(y^z)))	/* Rounds 0-19 */
#define f2(x,y,z)   (x^y^z)	/* Rounds 20-39 */
#define f3(x,y,z)   ((x&y)|(z&(x|y)))	/* Rounds 40-59 */
#define f4(x,y,z)   (x^y^z)	/* Rounds 60-79 */
/* 
 *  32-bit rotate left - kludged with shifts
 */
#define ROTL(n,X)   (((X)<<n)|((X)>>(32-n)))
/* 
 *  The initial expanding function.  The hash function is defined over an
 *  80-UINT2 expanded input array W, where the first 16 are copies of the input
 *  data, and the remaining 64 are defined by
 *
 *      W[i] = W[i - 16] ^ W[i - 14] ^ W[i - 8] ^ W[i - 3]
 *
 *  This implementation generates these values on the fly in a circular buffer -
 *  thanks to Colin Plumb, colin@nyx10.cs.du.edu for this optimization.
 *
 *  The updated SHS changes the expanding function by adding a rotate of 1 bit.
 *  Thanks to Jim Gillogly, jim@rand.org, and an anonymous contributor for this
 *  information
 */
#define x(W,i) \
	(W[i & 15] = \
	  ROTL(1, (W[i&15] ^ W[(i-14)&15] ^ W[(i-8)&15] ^ W[(i-3)&15])))
/* 
 *  The prototype SHS sub-round.  The fundamental sub-round is:
 *
 *      a' = e + ROTL(5, a) + f(b, c, d) + k + data;
 *      b' = a;
 *      c' = ROTL(30, b);
 *      d' = c;
 *      e' = d;
 *
 *  but this is implemented by unrolling the loop 5 times and renaming the
 *  variables (e, a, b, c, d) = (a', b', c', d', e') each iteration.  This
 *  code is then replicated 20 times for each of the 4 functions, using the next
 *  20 values from the W[] array each time
 */
#define subRound(a,b,c,d,e,f,k,data) \
    (e += ROTL(5,a)+f(b,c,d)+k+data, b = ROTL(30,b))
/* 
 *  Initialize the SHS values
 */
void
SHAInit(SHA_CTX * sha1)
{
	/* Set the h-vars to their initial values */
	sha1->dig[0] = 0x67452301L;
	sha1->dig[1] = 0xefcdab89L;
	sha1->dig[2] = 0x98badcfeL;
	sha1->dig[3] = 0x10325476L;
	sha1->dig[4] = 0xc3d2e1f0L;
	/* Initialise bit count */
	sha1->lo = sha1->hi = 0;
}

/* 
 *  Perform the SHS transformation.  Note that this code, like MD5, seems to
 *  break some optimizing compilers due to the complexity of the expressions and
 *  the size of the basic block.  It may be necessary to split it into sections,
 *  e.g. based on the four subrounds
 *
 *  Note that this corrupts the sha1->data area
 */
static void
SHSTransform(uint32_t *dig, uint32_t *dat)
{
	uint32_t A, B, C, D, E;		/* Local vars */
	uint32_t xd[16];		/* Expanded data */

	/* Set up first buffer and local data buffer */
	A = dig[0];
	B = dig[1];
	C = dig[2];
	D = dig[3];
	E = dig[4];
	memcpy(xd, dat, 64);
	/* Heavy mangling, in 4 sub-rounds of 20 interations each. */
	subRound(A, B, C, D, E, f1, 0x5a827999L, xd[0]);
	subRound(E, A, B, C, D, f1, 0x5a827999L, xd[1]);
	subRound(D, E, A, B, C, f1, 0x5a827999L, xd[2]);
	subRound(C, D, E, A, B, f1, 0x5a827999L, xd[3]);
	subRound(B, C, D, E, A, f1, 0x5a827999L, xd[4]);
	subRound(A, B, C, D, E, f1, 0x5a827999L, xd[5]);
	subRound(E, A, B, C, D, f1, 0x5a827999L, xd[6]);
	subRound(D, E, A, B, C, f1, 0x5a827999L, xd[7]);
	subRound(C, D, E, A, B, f1, 0x5a827999L, xd[8]);
	subRound(B, C, D, E, A, f1, 0x5a827999L, xd[9]);
	subRound(A, B, C, D, E, f1, 0x5a827999L, xd[10]);
	subRound(E, A, B, C, D, f1, 0x5a827999L, xd[11]);
	subRound(D, E, A, B, C, f1, 0x5a827999L, xd[12]);
	subRound(C, D, E, A, B, f1, 0x5a827999L, xd[13]);
	subRound(B, C, D, E, A, f1, 0x5a827999L, xd[14]);
	subRound(A, B, C, D, E, f1, 0x5a827999L, xd[15]);
	subRound(E, A, B, C, D, f1, 0x5a827999L, x(xd, 16));
	subRound(D, E, A, B, C, f1, 0x5a827999L, x(xd, 17));
	subRound(C, D, E, A, B, f1, 0x5a827999L, x(xd, 18));
	subRound(B, C, D, E, A, f1, 0x5a827999L, x(xd, 19));
	subRound(A, B, C, D, E, f2, 0x63d9eba1L, x(xd, 20));
	subRound(E, A, B, C, D, f2, 0x63d9eba1L, x(xd, 21));
	subRound(D, E, A, B, C, f2, 0x63d9eba1L, x(xd, 22));
	subRound(C, D, E, A, B, f2, 0x63d9eba1L, x(xd, 23));
	subRound(B, C, D, E, A, f2, 0x63d9eba1L, x(xd, 24));
	subRound(A, B, C, D, E, f2, 0x63d9eba1L, x(xd, 25));
	subRound(E, A, B, C, D, f2, 0x63d9eba1L, x(xd, 26));
	subRound(D, E, A, B, C, f2, 0x63d9eba1L, x(xd, 27));
	subRound(C, D, E, A, B, f2, 0x63d9eba1L, x(xd, 28));
	subRound(B, C, D, E, A, f2, 0x63d9eba1L, x(xd, 29));
	subRound(A, B, C, D, E, f2, 0x63d9eba1L, x(xd, 30));
	subRound(E, A, B, C, D, f2, 0x63d9eba1L, x(xd, 31));
	subRound(D, E, A, B, C, f2, 0x63d9eba1L, x(xd, 32));
	subRound(C, D, E, A, B, f2, 0x63d9eba1L, x(xd, 33));
	subRound(B, C, D, E, A, f2, 0x63d9eba1L, x(xd, 34));
	subRound(A, B, C, D, E, f2, 0x63d9eba1L, x(xd, 35));
	subRound(E, A, B, C, D, f2, 0x63d9eba1L, x(xd, 36));
	subRound(D, E, A, B, C, f2, 0x63d9eba1L, x(xd, 37));
	subRound(C, D, E, A, B, f2, 0x63d9eba1L, x(xd, 38));
	subRound(B, C, D, E, A, f2, 0x63d9eba1L, x(xd, 39));
	subRound(A, B, C, D, E, f3, 0x8f1bbcdcL, x(xd, 40));
	subRound(E, A, B, C, D, f3, 0x8f1bbcdcL, x(xd, 41));
	subRound(D, E, A, B, C, f3, 0x8f1bbcdcL, x(xd, 42));
	subRound(C, D, E, A, B, f3, 0x8f1bbcdcL, x(xd, 43));
	subRound(B, C, D, E, A, f3, 0x8f1bbcdcL, x(xd, 44));
	subRound(A, B, C, D, E, f3, 0x8f1bbcdcL, x(xd, 45));
	subRound(E, A, B, C, D, f3, 0x8f1bbcdcL, x(xd, 46));
	subRound(D, E, A, B, C, f3, 0x8f1bbcdcL, x(xd, 47));
	subRound(C, D, E, A, B, f3, 0x8f1bbcdcL, x(xd, 48));
	subRound(B, C, D, E, A, f3, 0x8f1bbcdcL, x(xd, 49));
	subRound(A, B, C, D, E, f3, 0x8f1bbcdcL, x(xd, 50));
	subRound(E, A, B, C, D, f3, 0x8f1bbcdcL, x(xd, 51));
	subRound(D, E, A, B, C, f3, 0x8f1bbcdcL, x(xd, 52));
	subRound(C, D, E, A, B, f3, 0x8f1bbcdcL, x(xd, 53));
	subRound(B, C, D, E, A, f3, 0x8f1bbcdcL, x(xd, 54));
	subRound(A, B, C, D, E, f3, 0x8f1bbcdcL, x(xd, 55));
	subRound(E, A, B, C, D, f3, 0x8f1bbcdcL, x(xd, 56));
	subRound(D, E, A, B, C, f3, 0x8f1bbcdcL, x(xd, 57));
	subRound(C, D, E, A, B, f3, 0x8f1bbcdcL, x(xd, 58));
	subRound(B, C, D, E, A, f3, 0x8f1bbcdcL, x(xd, 59));
	subRound(A, B, C, D, E, f4, 0xca62c1d6L, x(xd, 60));
	subRound(E, A, B, C, D, f4, 0xca62c1d6L, x(xd, 61));
	subRound(D, E, A, B, C, f4, 0xca62c1d6L, x(xd, 62));
	subRound(C, D, E, A, B, f4, 0xca62c1d6L, x(xd, 63));
	subRound(B, C, D, E, A, f4, 0xca62c1d6L, x(xd, 64));
	subRound(A, B, C, D, E, f4, 0xca62c1d6L, x(xd, 65));
	subRound(E, A, B, C, D, f4, 0xca62c1d6L, x(xd, 66));
	subRound(D, E, A, B, C, f4, 0xca62c1d6L, x(xd, 67));
	subRound(C, D, E, A, B, f4, 0xca62c1d6L, x(xd, 68));
	subRound(B, C, D, E, A, f4, 0xca62c1d6L, x(xd, 69));
	subRound(A, B, C, D, E, f4, 0xca62c1d6L, x(xd, 70));
	subRound(E, A, B, C, D, f4, 0xca62c1d6L, x(xd, 71));
	subRound(D, E, A, B, C, f4, 0xca62c1d6L, x(xd, 72));
	subRound(C, D, E, A, B, f4, 0xca62c1d6L, x(xd, 73));
	subRound(B, C, D, E, A, f4, 0xca62c1d6L, x(xd, 74));
	subRound(A, B, C, D, E, f4, 0xca62c1d6L, x(xd, 75));
	subRound(E, A, B, C, D, f4, 0xca62c1d6L, x(xd, 76));
	subRound(D, E, A, B, C, f4, 0xca62c1d6L, x(xd, 77));
	subRound(C, D, E, A, B, f4, 0xca62c1d6L, x(xd, 78));
	subRound(B, C, D, E, A, f4, 0xca62c1d6L, x(xd, 79));
	/* Build message digest */
	dig[0] += A;
	dig[1] += B;
	dig[2] += C;
	dig[3] += D;
	dig[4] += E;
}

/* 
 *  When run on a little-endian CPU we need to perform byte reversal on an array
 *  of long words.
 */
#ifdef __LITTLE_ENDIAN
static inline void
longReverse(uint32_t *buf, int cnt)
{
	uint32_t val;
	cnt /= sizeof(uint32_t);
	while (cnt--) {
		val = *buf;
		val = ((val & 0xff00ff00L) >> 8) | ((val & 0x00ff00ffL) << 8);
		*buf++ = (val << 16) | (val >> 16);
	}
}
#else
#define longReverse(__buf, __cnt) do { } while(0)
#endif
/* 
 *  Update SHS for a block of data
 */
void
SHAUpdate(SHA_CTX * sha1, uint8_t *buf, int len)
{
	uint32_t tmp;
	int cnt;

	/* Update bitcount */
	tmp = sha1->lo;
	if ((sha1->lo = tmp + ((uint32_t) len << 3)) < tmp)
		sha1->hi++;	/* Carry from low to high */
	sha1->hi += len >> 29;
	/* Get count of bytes already in data */
	cnt = (int) (tmp >> 3) & 0x3F;
	/* Handle any leading odd-sized chunks */
	if (cnt) {
		uint8_t *p = (uint8_t *) sha1->dat + cnt;

		cnt = 64 - cnt;
		if (len < cnt) {
			memcpy(p, buf, len);
			return;
		}
		memcpy(p, buf, cnt);
		longReverse(sha1->dat, 64);
		SHSTransform(sha1->dig, sha1->dat);
		buf += cnt;
		len -= cnt;
	}
	/* Process data in 64 chunks */
	while (len >= 64) {
		memcpy(sha1->dat, buf, 64);
		longReverse(sha1->dat, 64);
		SHSTransform(sha1->dig, sha1->dat);
		buf += 64;
		len -= 64;
	}
	/* Handle any remaining bytes of data. */
	memcpy(sha1->dat, buf, len);
}

/* 
 *  Final wrapup - pad to 64-byte boundary with the bit pattern 1 0*
 *  (64-bit count of bits processed, MSB-first)
 */
void
SHAFinal(uint8_t *out, SHA_CTX * sha1)
{
	int len;
	unsigned int i, j;
	uint8_t *dat;

	/* Compute number of bytes mod 64 */
	len = (int) sha1->lo;
	len = (len >> 3) & 0x3F;
	/* Set the first char of padding to 0x80.  This is safe since there is always at least one
	   byte free */
	dat = (uint8_t *) sha1->dat + len;
	*dat++ = 0x80;
	/* Bytes of padding needed to make 64 bytes */
	len = 64 - 1 - len;
	/* Pad out to 56 mod 64 */
	if (len < 8) {
		/* Two lots of padding: Pad the first block to 64 bytes */
		memset(dat, 0, len);
		longReverse(sha1->dat, 64);
		SHSTransform(sha1->dig, sha1->dat);
		/* Now fill the next block with 56 bytes */
		memset(sha1->dat, 0, 64 - 8);
	} else
		/* Pad block to 56 bytes */
		memset(dat, 0, len - 8);
	/* Append length in bits and transform */
	sha1->dat[14] = sha1->hi;
	sha1->dat[15] = sha1->lo;
	longReverse(sha1->dat, 64 - 8);
	SHSTransform(sha1->dig, sha1->dat);
	/* Output SHA digest in byte array */
	for (i = 0, j = 0; j < 20; i++, j += 4) {
		out[j + 3] = (uint8_t) (sha1->dig[i] & 0xff);
		out[j + 2] = (uint8_t) ((sha1->dig[i] >> 8) & 0xff);
		out[j + 1] = (uint8_t) ((sha1->dig[i] >> 16) & 0xff);
		out[j] = (uint8_t) ((sha1->dig[i] >> 24) & 0xff);
	}
	/* Zeroise sensitive stuff */
	memset(sha1, 0, sizeof(*sha1));
}
