/*****************************************************************************

 @(#) $RCSfile: sctp_cookie.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2004/08/21 11:04:33 $

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
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2004/08/21 11:04:33 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: sctp_cookie.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2004/08/21 11:04:33 $"

static char const ident[] = "$RCSfile: sctp_cookie.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2004/08/21 11:04:33 $";

/*
 *  This file contains all the algorithms for generating MAC for SCTP cookies
 *  and for verifying existing cookies.  The SHA-1 and MD5 code takes a lot of
 *  space (about 8k kernel space per algorithm) so I have added some
 *  configuration options to allow removing unused algorithms.
 */

#define __NO_VERSION__

#include <linux/config.h>
#include <linux/version.h>
#ifdef MODVERSIONS
#include <linux/modversions.h>
#endif
#include <linux/module.h>

#include <sys/stream.h>
#include <sys/cmn_err.h>
#include <sys/dki.h>

#include <linux/random.h>	/* for secure_tcp_sequence_number */

#include "debug.h"
#include "bufq.h"

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
