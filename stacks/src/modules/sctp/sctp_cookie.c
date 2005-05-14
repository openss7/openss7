/*****************************************************************************

 @(#) $RCSfile: sctp_cookie.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2005/05/14 08:31:05 $

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

 Last Modified $Date: 2005/05/14 08:31:05 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: sctp_cookie.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2005/05/14 08:31:05 $"

static char const ident[] = "$RCSfile: sctp_cookie.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2005/05/14 08:31:05 $";

/*
 *  This file contains all the algorithms for generating MAC for SCTP cookies
 *  and for verifying existing cookies.  The SHA-1 and MD5 code takes a lot of
 *  space (about 8k kernel space per algorithm) so I have added some
 *  configuration options to allow removing unused algorithms.
 */

#define __NO_VERSION__

#include "os7/compat.h"

#include <linux/random.h>	/* for secure_tcp_sequence_number */

#include "sctp.h"
#include "sctp_defs.h"
#include "sctp_cookie.h"

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
 *  The structure for storing SHS info
 */
typedef struct {
	u32 dig[5];			/* Message digest */
	u32 lo, hi;			/* 64-bit bit count */
	u32 dat[16];			/* SHS data buffer */
} SHA_CTX;

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
STATIC void
SHAInit(SHA_CTX * sha1)
{
	/*
	   Set the h-vars to their initial values 
	 */
	sha1->dig[0] = 0x67452301L;
	sha1->dig[1] = 0xefcdab89L;
	sha1->dig[2] = 0x98badcfeL;
	sha1->dig[3] = 0x10325476L;
	sha1->dig[4] = 0xc3d2e1f0L;

	/*
	   Initialise bit count 
	 */
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
STATIC void
SHSTransform(u32 * dig, u32 * dat)
{
	u32 A, B, C, D, E;		/* Local vars */
	u32 xd[16];			/* Expanded data */

	/*
	   Set up first buffer and local data buffer 
	 */
	A = dig[0];
	B = dig[1];
	C = dig[2];
	D = dig[3];
	E = dig[4];
	memcpy(xd, dat, 64);

	/*
	   Heavy mangling, in 4 sub-rounds of 20 interations each. 
	 */
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

	/*
	   Build message digest 
	 */
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
STATIC void
longReverse(u32 * buf, int cnt)
{
	u32 val;

	cnt /= sizeof(u32);
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
STATIC void
SHAUpdate(SHA_CTX * sha1, u8 * buf, int len)
{
	u32 tmp;
	int cnt;

	/*
	   Update bitcount 
	 */
	tmp = sha1->lo;
	if ((sha1->lo = tmp + ((u32) len << 3)) < tmp)
		sha1->hi++;	/* Carry from low to high */
	sha1->hi += len >> 29;

	/*
	   Get count of bytes already in data 
	 */
	cnt = (int) (tmp >> 3) & 0x3F;

	/*
	   Handle any leading odd-sized chunks 
	 */
	if (cnt) {
		u8 *p = (u8 *) sha1->dat + cnt;

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

	/*
	   Process data in 64 chunks 
	 */
	while (len >= 64) {
		memcpy(sha1->dat, buf, 64);
		longReverse(sha1->dat, 64);
		SHSTransform(sha1->dig, sha1->dat);
		buf += 64;
		len -= 64;
	}

	/*
	   Handle any remaining bytes of data. 
	 */
	memcpy(sha1->dat, buf, len);
}

/*
 *  Final wrapup - pad to 64-byte boundary with the bit pattern 1 0*
 *  (64-bit count of bits processed, MSB-first)
 */
STATIC void
SHAFinal(u8 * out, SHA_CTX * sha1)
{
	int len;
	unsigned int i, j;
	u8 *dat;

	/*
	   Compute number of bytes mod 64 
	 */
	len = (int) sha1->lo;
	len = (len >> 3) & 0x3F;

	/*
	 *  Set the first char of padding to 0x80.  This is safe since there is
	 *  always at least one byte free
	 */
	dat = (u8 *) sha1->dat + len;
	*dat++ = 0x80;

	/*
	   Bytes of padding needed to make 64 bytes 
	 */
	len = 64 - 1 - len;

	/*
	   Pad out to 56 mod 64 
	 */
	if (len < 8) {
		/*
		   Two lots of padding: Pad the first block to 64 bytes 
		 */
		memset(dat, 0, len);
		longReverse(sha1->dat, 64);
		SHSTransform(sha1->dig, sha1->dat);

		/*
		   Now fill the next block with 56 bytes 
		 */
		memset(sha1->dat, 0, 64 - 8);
	} else
		/*
		   Pad block to 56 bytes 
		 */
		memset(dat, 0, len - 8);

	/*
	   Append length in bits and transform 
	 */
	sha1->dat[14] = sha1->hi;
	sha1->dat[15] = sha1->lo;

	longReverse(sha1->dat, 64 - 8);
	SHSTransform(sha1->dig, sha1->dat);

	/*
	   Output SHA digest in byte array 
	 */
	for (i = 0, j = 0; j < 20; i++, j += 4) {
		out[j + 3] = (u8) (sha1->dig[i] & 0xff);
		out[j + 2] = (u8) ((sha1->dig[i] >> 8) & 0xff);
		out[j + 1] = (u8) ((sha1->dig[i] >> 16) & 0xff);
		out[j] = (u8) ((sha1->dig[i] >> 24) & 0xff);
	}

	/*
	   Zeroise sensitive stuff 
	 */
	memset(sha1, 0, sizeof(sha1));
}

/*
 *  -------------------------------------------------------------------------
 *
 *  HMAC-SHA-1
 *
 *  -------------------------------------------------------------------------
 *
 *  Code adapted directly from RFC 2401.
 */
STATIC void
hmac_sha1(u8 * text, int tlen, u8 * key, int klen, u8 * digest)
{
	SHA_CTX context;
	u8 k_ipad[64];
	u8 k_opad[64];
	u8 tk[16];

	int i;

	if (klen > 64) {
		SHA_CTX ctx;
		SHAInit(&ctx);
		SHAUpdate(&ctx, key, klen);
		SHAFinal(tk, &ctx);

		key = tk;
		klen = 16;
	}

	memset(k_ipad, 0, sizeof(k_ipad));
	memset(k_opad, 0, sizeof(k_opad));
	memcpy(k_ipad, key, klen);
	memcpy(k_opad, key, klen);

	for (i = 0; i < 64; i++) {
		k_ipad[i] ^= 0x36;
		k_opad[i] ^= 0x5c;
	}

	/*
	   inner 
	 */
	SHAInit(&context);
	SHAUpdate(&context, k_ipad, 64);
	SHAUpdate(&context, text, tlen);
	SHAFinal(digest, &context);

	/*
	   outer 
	 */
	SHAInit(&context);
	SHAUpdate(&context, k_opad, 64);
	SHAUpdate(&context, digest, 20);
	SHAFinal(digest, &context);
}

/*
 *  =========================================================================
 *
 *  MD5
 *
 *  =========================================================================
 */

typedef struct {
	u32 buf[4];
	u32 lo, hi;
	u32 dat[16];
} MD5_CTX;

/*
   The four core functions - F1 is optimized somewhat 
 */
/*
   #define F1(x, y, z) (x & y | ~x & z) 
 */
#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))

/*
   This is the central step in the MD5 algorithm. 
 */
#define MD5STEP(f,w,x,y,z,dat,s) \
	 (w += f(x,y,z) + dat, w = (w<<s | w>>(32-s)) + x)

/*
 *  The core of the MD5 algorithm, this alters an existing MD5 hash to reflect
 *  the addition of 16 longwords of new data.  MD5Update blocks the data and
 *  converts bytes into longwords for this routine.
 */
STATIC void
MD5Transform(u32 dig[4], u32 const dat[16])
{
	register u32 a, b, c, d;

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
void
byteSwap(u32 * buf, unsigned cnt)
{
	u8 *p = (u8 *) buf;

	do {
		*buf++ = (u32) ((unsigned) p[3] << 8 | p[2]) << 16 | ((unsigned) p[1] << 8 | p[0]);
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
STATIC void
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
STATIC void
MD5Update(MD5_CTX * md5, u8 const *buf, unsigned len)
{
	u32 t;

	/*
	   Update byte count 
	 */

	t = md5->lo;
	if ((md5->lo = t + len) < t)
		md5->hi++;	/* Carry from low to high */

	t = 64 - (t & 0x3f);	/* Space available in md5->dat (at least 1) */
	if (t > len) {
		memcpy((u8 *) md5->dat + 64 - t, buf, len);
		return;
	}
	/*
	   First chunk is an odd size 
	 */
	memcpy((u8 *) md5->dat + 64 - t, buf, t);
	byteSwap(md5->dat, 16);
	MD5Transform(md5->buf, md5->dat);
	buf += t;
	len -= t;

	/*
	   Process data in 64-byte chunks 
	 */
	while (len >= 64) {
		memcpy(md5->dat, buf, 64);
		byteSwap(md5->dat, 16);
		MD5Transform(md5->buf, md5->dat);
		buf += 64;
		len -= 64;
	}

	/*
	   Handle any remaining bytes of data. 
	 */
	memcpy(md5->dat, buf, len);
}

/*
 * Final wrapup - pad to 64-byte boundary with the bit pattern 
 * 1 0* (64-bit count of bits processed, MSB-first)
 */
STATIC void
MD5Final(u8 dig[16], MD5_CTX * md5)
{
	int count = md5->lo & 0x3f;	/* Number of bytes in md5->dat */
	u8 *p = (u8 *) md5->dat + count;

	/*
	   Set the first char of padding to 0x80.  There is always room. 
	 */
	*p++ = 0x80;

	/*
	   Bytes of padding needed to make 56 bytes (-8..55) 
	 */
	count = 56 - 1 - count;

	if (count < 0) {	/* Padding forces an extra block */
		memset(p, 0, count + 8);
		byteSwap(md5->dat, 16);
		MD5Transform(md5->buf, md5->dat);
		p = (u8 *) md5->dat;
		count = 56;
	}
	memset(p, 0, count);
	byteSwap(md5->dat, 14);

	/*
	   Append length in bits and transform 
	 */
	md5->dat[14] = md5->lo << 3;
	md5->dat[15] = md5->hi << 3 | md5->lo >> 29;
	MD5Transform(md5->buf, md5->dat);

	byteSwap(md5->buf, 4);
	memcpy(dig, md5->buf, 16);
	memset(md5, 0, sizeof(md5));	/* In case it's sensitive */
}

/*
 *  -------------------------------------------------------------------------
 *
 *  HMAC-MD5
 *
 *  -------------------------------------------------------------------------
 *
 *  Code adapted directly from RFC 2401.
 */
STATIC void
hmac_md5(u8 * text, int tlen, u8 * key, int klen, u8 * digest)
{
	MD5_CTX context;
	u8 k_ipad[65];
	u8 k_opad[65];
	u8 tk[16];

	int i;

	if (klen > 64) {
		MD5_CTX ctx;
		MD5Init(&ctx);
		MD5Update(&ctx, key, klen);
		MD5Final(tk, &ctx);

		key = tk;
		klen = 16;
	}

	memset(k_ipad, 0, sizeof(k_ipad));
	memset(k_opad, 0, sizeof(k_opad));
	memcpy(k_ipad, key, klen);
	memcpy(k_opad, key, klen);

	for (i = 0; i < 64; i++) {
		k_ipad[i] ^= 0x36;
		k_opad[i] ^= 0x5c;
	}

	/*
	   inner 
	 */
	MD5Init(&context);
	MD5Update(&context, k_ipad, 64);
	MD5Update(&context, text, tlen);
	MD5Final(digest, &context);

	/*
	   outer 
	 */
	MD5Init(&context);
	MD5Update(&context, k_opad, 64);
	MD5Update(&context, digest, 16);
	MD5Final(digest, &context);
}

/*
 *  =========================================================================
 *
 *  SCTP Key handling
 *
 *  =========================================================================
 *
 *  This algorithm uses 2^n keys (NUM_KEYS) which are recycled quickly and
 *  evenly.  Each key has a key tag which is placed in the SCTP cookie.  Each
 *  key is valid for the cookie lifespan plus a key life.  The key life should
 *  be set to whatever cookie life increment has been permitted.  When a cookie
 *  is checked for validity, its MAC is verified using the key with the key tag
 *  in the cookie.  If the key has already been recycled, the tagged key will
 *  not fit the lock anymore.  Note that the keys are cycled only as quickly as
 *  the requests for signatures come in.  This adds another degree of
 *  variability to the key selection.
 */

struct sctp_key {
	union {
		u32 seq[16];
		u8 key[64];
	} u;
	unsigned int last;
	unsigned long created;
};

#define NUM_KEYS 4
STATIC struct sctp_key sctp_keys[NUM_KEYS];
STATIC int sctp_current_key = 0;

extern u32 secure_tcp_sequence_number(u32, u32, u16, u16);

/*
 *  TODO:  This rekeying is too predicatable.  There are several things bad
 *  about it: (1) the key has a historic component, which is bad; (2) initial
 *  keys have zeros in alot of places which makes it no stronger than if only
 *  32 bit keys were used.
 */
STATIC void
sctp_rekey(k)
	int k;
{
	u32 *seq;
	int n = (sctp_keys[k].last + 1) & 0xf;

	sctp_keys[k].last = n;
	seq = &sctp_keys[k].u.seq[n];
	*seq = secure_tcp_sequence_number(*(seq + 1), *(seq + 2), *(seq + 3), *(seq + 4));
}

STATIC int
sctp_get_key(sp)
	sctp_t *sp;
{
	int k = sctp_current_key;
	unsigned long duration = ((sp->ck_life + NUM_KEYS - 1) / NUM_KEYS) + sp->ck_inc;
	unsigned long created = sctp_keys[k].created;

	if (!created) {
		sctp_keys[k].created = jiffies;
		return k;
	}
	if (created + duration < jiffies) {
		k = (k + 1) % NUM_KEYS;
		sctp_rekey(k);
		sctp_current_key = k;
	}
	return k;
}

STATIC void
sctp_hmac(sp, text, tlen, key, klen, hmac)
	sctp_t *sp;
	u8 *text;
	int tlen;
	u8 *key;
	int klen;
	u8 *hmac;
{
	memset(hmac, 0xff, HMAC_SIZE);
	switch (sp->hmac) {
	case SCTP_HMAC_SHA_1:
		hmac_sha1(text, tlen, key, klen, hmac);
		break;
	case SCTP_HMAC_MD5:
		hmac_md5(text, tlen, key, klen, hmac);
		break;
	default:
	case SCTP_HMAC_NONE:
		break;
	}
}

void
sctp_sign_cookie(sp, ck)
	sctp_t *sp;
	struct sctp_cookie *ck;
{
	u8 *text = (u8 *) ck;
	int tlen = raw_cookie_size(ck);
	int ktag = sctp_get_key(sp);
	u8 *key = sctp_keys[ktag].u.key;
	int klen = sizeof(sctp_keys[0].u.key);
	u8 *hmacp = ((u8 *) ck) + tlen;

#if 0
	int i;
	ptrace(("Signing cookie:\n"));
	printk("text = %u, tlen = %d\n", (uint) text, tlen);
	printk("ktag = %d, klen = %d\n", ktag, klen);
	printk("key = ");
	for (i = 0; i < klen; i++)
		printk("%02x", key[i]);
	printk("\n");
#endif
	ck->key_tag = ktag;
	sctp_hmac(sp, text, tlen, key, klen, hmacp);
#if 0
	printk("hmac = ");
	for (i = 0; i < HMAC_SIZE; i++)
		printk("%02x", hmacp[i]);
	printk("\n");
#endif
}

/*
   Note: caller must verify length of cookie 
 */
int
sctp_verify_cookie(sp, ck)
	sctp_t *sp;
	struct sctp_cookie *ck;
{
	u8 hmac[HMAC_SIZE];
	u8 *text = (u8 *) ck;
	int tlen = raw_cookie_size(ck);
	int ktag = (ck->key_tag) % NUM_KEYS;
	u8 *key = sctp_keys[ktag].u.key;
	int klen = sizeof(sctp_keys[0].u.key);
	u8 *hmacp = ((u8 *) ck) + tlen;

#if 0
	int i;
	ptrace(("Verifying cookie:\n"));
	printk("text = %u, tlen = %d\n", (uint) text, tlen);
	printk("ktag = %d, klen = %d\n", ktag, klen);
	printk("key = ");
	for (i = 0; i < klen; i++)
		printk("%02x", key[i]);
	printk("\n");
	printk("hmac = ");
	for (i = 0; i < HMAC_SIZE; i++)
		printk("%02x", hmacp[i]);
	printk("\n");
#endif
	sctp_hmac(sp, text, tlen, key, klen, hmac);
#if 0
	printk("hmac = ");
	for (i = 0; i < HMAC_SIZE; i++)
		printk("%02x", hmac[i]);
	printk("\n");
#endif
	return memcmp(hmacp, hmac, HMAC_SIZE);
}
