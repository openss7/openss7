/*****************************************************************************

 @(#) $RCSfile: sctp_hmac_sha1.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2005/01/01 11:17:54 $

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

 Last Modified $Date: 2005/01/01 11:17:54 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: sctp_hmac_sha1.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2005/01/01 11:17:54 $"

static char const ident[] = "$RCSfile: sctp_hmac_sha1.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2005/01/01 11:17:54 $";

#include <linux/config.h>
#include <linux/version.h>
#ifdef MODVERSIONS
#   include <linux/modversions.h>
#endif
#include <linux/module.h>
#include <linux/modversions.h>
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
#ifdef HAVE_NET_XFRM_H
#include <net/xfrm.h>
#endif
#include <net/icmp.h>
#ifdef HAVE_NET_DST_H
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

#include "net/sock.h"
#include "net/snmp.h"
#include "net/sctp.h"
#include "os7_namespace.h"

#include "sctp_sha1.h"
#include "sctp_hmac_sha1.h"

/*
 *  -------------------------------------------------------------------------
 *
 *  HMAC-SHA-1
 *
 *  -------------------------------------------------------------------------
 *
 *  Code adapted directly from RFC 2401.
 */
void
hmac_sha1(uint8_t * text, int tlen, uint8_t * key, int klen, uint8_t * digest)
{
	SHA_CTX context;
	uint8_t k_ipad[64];
	uint8_t k_opad[64];
	uint8_t tk[16];
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
	/* inner */
	SHAInit(&context);
	SHAUpdate(&context, k_ipad, 64);
	SHAUpdate(&context, text, tlen);
	SHAFinal(digest, &context);
	/* outer */
	SHAInit(&context);
	SHAUpdate(&context, k_opad, 64);
	SHAUpdate(&context, digest, 20);
	SHAFinal(digest, &context);
}
