/*****************************************************************************

 @(#) $RCSfile: sctp_hmac_sha1.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2004/08/23 11:44:04 $

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

 Last Modified $Date: 2004/08/23 11:44:04 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: sctp_hmac_sha1.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2004/08/23 11:44:04 $"

static char const ident[] = "$RCSfile: sctp_hmac_sha1.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2004/08/23 11:44:04 $";

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
#   include <linux/config.h>
#   include <linux/version.h>
#   ifndef HAVE_SYS_LIS_MODULE_H
#	ifdef MODVERSIONS
#	    include <linux/modversions.h>
#	endif
#	include <linux/module.h>
#	include <linux/modversions.h>
#	ifndef __GENKSYMS__
#	    if defined HAVE_SYS_LIS_MODVERSIONS_H
#		include <sys/LiS/modversions.h>
#	    elif defined HAVE_SYS_STREAMS_MODVERSIONS_H
#		include <sys/streams/modversions.h>
#	    endif
#	endif
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

#if defined HAVE_OPENSS7_SCTP
#   define sctp_bind_bucket __sctp_bind_bucket
#   define sctp_dup __sctp_dup
#   define sctp_strm __sctp_strm
#   define sctp_saddr __sctp_saddr
#   define sctp_daddr __sctp_daddr
#   define sctp_protolist __sctp_protolist
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
        sa_family_t     ss_family;
        char            __data[_SS_MAXSIZE - sizeof(sa_family_t)];
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
#include "debug.h"
#endif
#include "bufq.h"

#include "sctp_sha1.h"
#include "sctp_hmac_sha1.h"

/* 
 *  -------------------------------------------------------------------------
 *
 *  HMAC-SHA-1
 *
 *  -------------------------------------------------------------------------
 *
 *  Code adapted directly from RFC 2104.
 */
void
hmac_sha1(uint8_t *text, int tlen, uint8_t *key, int klen, uint8_t *digest)
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
