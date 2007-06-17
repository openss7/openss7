/*****************************************************************************

 @(#) $RCSfile: sctp_cookie.c,v $ $Name:  $($Revision: 0.9.2.9 $) $Date: 2007/06/17 01:56:19 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2007/06/17 01:56:19 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: sctp_cookie.c,v $ $Name:  $($Revision: 0.9.2.9 $) $Date: 2007/06/17 01:56:19 $"

static char const ident[] = "$RCSfile: sctp_cookie.c,v $ $Name:  $($Revision: 0.9.2.9 $) $Date: 2007/06/17 01:56:19 $";

/*
 *  This file contains all the algorithms for generating MAC for SCTP cookies
 *  and for verifying existing cookies.  The SHA-1 and MD5 code takes a lot of
 *  space (about 8k kernel space per algorithm) so I have added some
 *  configuration options to allow removing unused algorithms.
 */

#define __NO_VERSION__

#if defined LIS && !defined _LIS_SOURCE
#define _LIS_SOURCE
#endif

#if defined LFS && !defined _LFS_SOURCE
#define _LFS_SOURCE
#endif

#if !defined _LIS_SOURCE && !defined _LFS_SOURCE
#   error ****
#   error **** One of _LFS_SOURCE or _LIS_SOURCE must be defined
#   error **** to compile the sctp driver.
#   error ****
#endif

#ifdef LINUX
#   include <linux/autoconf.h>
#   include <linux/version.h>
#   ifndef HAVE_SYS_LIS_MODULE_H
#	include <linux/module.h>
#	include <linux/init.h>
#   else
#	include <sys/LiS/module.h>
#   endif
#endif

#if defined HAVE_OPENSS7_SCTP
#   if !defined CONFIG_SCTP && !defined CONFIG_SCTP_MODULE
#	undef HAVE_OPENSS7_SCTP
#   endif
#endif

#include <linux/random.h>	/* for secure_tcp_sequence_number */

#if defined HAVE_OPENSS7_SCTP
#   define sctp_bind_bucket __sctp_bind_bucket
#   define sctp_dup __sctp_dup
#   define sctp_strm __sctp_strm
#   define sctp_saddr __sctp_saddr
#   define sctp_daddr __sctp_daddr
#   define sctp_protolist __sctp_protolist
#endif

#if defined HAVE_LKSCTP_SCTP
#   if !defined CONFIG_IP_SCTP && !defined CONFIG_IP_SCTP_MODULE
#	undef HAVE_LKSCTP_SCTP
#   endif
#endif

#if defined HAVE_LKSCTP_SCTP
#   define sctp_bind_bucket __sctp_bind_bucket
#   define sctp_mib	    __sctp_mib
#   define sctphdr	    __sctphdr
#   define sctp_cookie	    __sctp_cookie
#   define sctp_chunk	    __sctp_chunk
#endif

#if !defined HAVE_OPENSS7_SCTP
#   undef sctp_addr
#   define sctp_addr stupid_sctp_addr_in_the_wrong_place
#endif

#include <linux/net.h>
#include <linux/in.h>
#include <linux/ip.h>

#include <net/sock.h>
#include <net/udp.h>
#include <net/tcp.h>

#if defined HAVE_OPENSS7_SCTP
#   undef STATIC
#   undef INLINE
#   include <net/sctp.h>
#endif

#ifndef HAVE_STRUCT_SOCKADDR_STORAGE
#define _SS_MAXSIZE     128
#define _SS_ALIGNSIZE   (__alignof__ (struct sockaddr *))
struct sockaddr_storage {
	sa_family_t ss_family;
	char __data[_SS_MAXSIZE - sizeof(sa_family_t)];
} __attribute__ ((aligned(_SS_ALIGNSIZE)));
#endif

#include <sys/kmem.h>
#include <sys/cmn_err.h>
#include <sys/stream.h>
#include <sys/dki.h>

#ifdef LFS_SOURCE
#include <sys/strconf.h>
#include <sys/strsubr.h>
#include <sys/strdebug.h>
#include <sys/debug.h>
#endif
#include <sys/ddi.h>

#ifndef LFS
#include <os7/debug.h>
#endif
#include <os7/bufq.h>

#include "sctp.h"
#include "sctp_defs.h"
#include "sctp_cookie.h"

#include "sctp_hmac_sha1.h"
#include "sctp_hmac_md5.h"

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
