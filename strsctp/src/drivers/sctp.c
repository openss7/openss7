/*****************************************************************************

 @(#) $RCSfile: sctp.c,v $ $Name:  $($Revision: 0.9.2.14 $) $Date: 2007/08/15 05:34:33 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

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

 Last Modified $Date: 2007/08/15 05:34:33 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sctp.c,v $
 Revision 0.9.2.14  2007/08/15 05:34:33  brian
 - GPLv3 updates

 Revision 0.9.2.13  2007/08/14 06:22:22  brian
 - GPLv3 header update

 Revision 0.9.2.12  2007/07/14 01:36:42  brian
 - make license explicit, add documentation

 Revision 0.9.2.11  2006/10/27 23:10:56  brian
 - changes for 2.6.18 kernel

 Revision 0.9.2.10  2006/03/07 00:49:40  brian
 - updated headers

 *****************************************************************************/

#ident "@(#) $RCSfile: sctp.c,v $ $Name:  $($Revision: 0.9.2.14 $) $Date: 2007/08/15 05:34:33 $"

static char const ident[] =
    "$RCSfile: sctp.c,v $ $Name:  $($Revision: 0.9.2.14 $) $Date: 2007/08/15 05:34:33 $";

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
#include "sctp_hash.h"
#include "sctp_cache.h"

#include "sctp_n.h"
#include "sctp_t.h"
#include "sctp_input.h"

#define SCTP_DESCRIP	"SCTP/IP STREAMS (NPI/TPI) DRIVER." "\n" \
			"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define SCTP_REVISION	"LfS $RCSfile: sctp.c,v $ $Name:  $($Revision: 0.9.2.14 $) $Date: 2007/08/15 05:34:33 $"
#define SCTP_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corp. All Rights Reserved."
#define SCTP_DEVICE	"Supports LiS STREAMS and Linux NET4."
#define SCTP_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SCTP_LICENSE	"GPL"
#define SCTP_BANNER	SCTP_DESCRIP	"\n" \
			SCTP_REVISION	"\n" \
			SCTP_COPYRIGHT	"\n" \
			SCTP_DEVICE	"\n" \
			SCTP_CONTACT	"\n"

MODULE_AUTHOR(SCTP_CONTACT);
MODULE_DESCRIPTION(SCTP_DESCRIP);
MODULE_SUPPORTED_DEVICE(SCTP_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(SCTP_LICENSE);
#endif
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-sctp");
#endif

STATIC void
sctp_init(void)
{
	sctp_init_locks();
	sctp_init_caches();
	sctp_init_proto();
}
STATIC void
sctp_term(void)
{
	sctp_term_locks();
	sctp_term_caches();
	sctp_term_proto();
}

/*
 *  =========================================================================
 *
 *  Kernel Module Initialization
 *
 *  =========================================================================
 */
int
init_module(void)
{
	cmn_err(CE_NOTE, SCTP_BANNER);	/* console splash */
	sctp_init();
	sctp_n_init();
	sctp_t_init();
	return (0);
}

void
cleanup_module(void)
{
	sctp_term();
	sctp_n_term();
	sctp_t_term();
	return;
}
