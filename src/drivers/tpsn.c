/*****************************************************************************

 @(#) File: src/drivers/tpsn.c

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation; version 3 of the License.

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

 *****************************************************************************/

static char const ident[] = "src/drivers/tpsn.c (" PACKAGE_ENVR ") " PACKAGE_DATE;

/*
 * This file provides an integrated driver for ISO/OSI X.224/X.234  Connection-mode and
 * connectionless-mode transport protocol running over a ISO/IEC 8802 LAN with subnetwork-only
 * addressing and a null network layer as a non-multiplexing pseudo-device driver.  The driver
 * provides a Transport Provider Interface (TPI) interface and can be used directly with the XTI/TLI
 * library.  No configuration is necessary to use the driver and associated devices: the Linux
 * network device independent subsystem is used internally instead of linking network drivers and
 * datalink drivers.
 *
 * The TPSN driver can support null network layer (inactive subset) subnetwork interconnection for
 * CLNP. ISO-IP and ISO-UDP in accordance with RFC 1070 operation is also supported.
 */

//#define _DEBUG 1
//#undef _DEBUG

#define _SVR4_SOURCE	1
#define _MPS_SOURCE	1

#include <sys/os7/compat.h>

#include <linux/bitops.h>

#define t_tst_bit(nr,addr)	test_bit(nr,addr)
#define t_set_bit(nr,addr)	__set_bit(nr,addr)
#define t_clr_bit(nr,addr)	__clear_bit(nr,addr)

#include <linux/interrupt.h>

#ifdef HAVE_KINC_LINUX_BRLOCK_H
#include <linux/brlock.h>
#endif

#include <linux/udp.h>

#include <linux/net.h>
#include <linux/in.h>
#include <linux/un.h>
#include <linux/ip.h>

#undef ASSERT

#include <net/sock.h>
#include <net/ip.h>
#include <net/icmp.h>
#include <net/route.h>
#include <net/inet_ecn.h>
#include <net/snmp.h>

#include <net/udp.h>
#include <net/llc.h>

#ifdef HAVE_KINC_NET_DST_H
#include <net/dst.h>
#endif

#include <net/protocol.h>

#include <linux/skbuff.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>

#include "net_hooks.h"

#if 0
/* Turn on some tracing and debugging. */
#undef ensure
#undef assure

#define ensure __ensure
#define assure __assure
#endif

#if defined HAVE_TIHDR_H
#   include <tihdr.h>
#else
#   include <sys/tihdr.h>
#endif

#include <sys/xti.h>
#include <sys/xti_osi.h>

#define T_ALLLEVELS -1

#define LINUX_2_4 1

#define	TP_DESCRIP	"OSI Transport Provider for Subnetworks (TPSN) STREAMS Driver"
#define TP_EXTRA	"Part of the OpenSS7 OSI Stack for Linux Fast-STREAMS"
#define TP_COPYRIGHT	"Copyright (c) 2008-2015  Monavacon Ltd.  All Rights Reserved."
#define TP_REVISION	"OpenSS7 src/drivers/tpsn.c (" PACKAGE_ENVR ") " PACKAGE_DATE
#define TP_DEVICE	"SVR 4.2 MP STREAMS TPI OSI Transport for Subnetworks Driver"
#define TP_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define TP_LICENSE	"GPL"
#define TP_BANNER	TP_DESCRIP	"\n" \
			TP_EXTRA	"\n" \
			TP_COPYRIGHT	"\n" \
			TP_DEVICE	"\n" \
			TP_CONTACT
#define TP_SPLASH	TP_DESCRIP	" - " \
			TP_REVISION

#ifdef LINUX
MODULE_AUTHOR(TP_CONTACT);
MODULE_DESCRIPTION(TP_DESCRIP);
MODULE_SUPPORTED_DEVICE(TP_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(TP_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-tpsn");
#endif
#ifdef MODULE_VERSION
MODULE_VERSION(PACKAGE_ENVR);
#endif
#endif

#define TP_DRV_ID	CONFIG_STREAMS_TPSN_MODID
#define TP_MOD_ID	CONFIG_STREAMS_TPSN_MODID
#define TP_DRV_NAME	CONFIG_STREAMS_TPSN_NAME
#define TP_MOD_NAME	CONFIG_STREAMS_TPSN_NAME
#define TP_CMAJORS	CONFIG_STREAMS_TPSN_NMAJOR
#define TP_CMAJOR_0	CONFIG_STREAMS_TPSN_MAJOR
#define TP_UNITS	CONFIG_STREAMS_TPSN_NMINORS

#ifdef LINUX
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_TPSN_MODID));
MODULE_ALIAS("streams-driver-tpsn");
MODULE_ALIAS("streams-module-tpsn");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_TPSN_MAJOR));
MODULE_ALIAS("/dev/streams/tpsn");
MODULE_ALIAS("/dev/streams/tpsn/*");
MODULE_ALIAS("/dev/streams/clone/tpsn");
MODULE_ALIAS("char-major-" __stringify(CONFIG_STREAMS_CLONE_MAJOR) "-" __stringify(TP_CMAJOR_0));
MODULE_ALIAS("/dev/tpsn");
//MODULE_ALIAS("devname:tpsn");
MODULE_ALIAS("/dev/clts");
MODULE_ALIAS("/dev/cots");
#endif				/* MOUDLE_ALIAS */
#endif				/* LINUX */

