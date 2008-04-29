/*****************************************************************************

 @(#) $RCSfile: sctp_glue.c,v $ $Name:  $($Revision: 0.9.2.19 $) $Date: 2008-04-29 08:49:53 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2008-04-29 08:49:53 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sctp_glue.c,v $
 Revision 0.9.2.19  2008-04-29 08:49:53  brian
 - updated headers for Affero release

 Revision 0.9.2.18  2007/08/14 09:42:40  brian
 - GPLv3 header update

 *****************************************************************************/

#ident "@(#) sctp_glue.c,v LINUX-2-4-20-SCTP(1.1.6.3) 2004/02/09 17:36:19"

static char const ident[] = "sctp_glue.c,v LINUX-2-4-20-SCTP(1.1.6.3) 2004/02/09 17:36:19";

/*
 *  This file contains glue code for SCTP.  These are things which must be
 *  defined and included in the kernel whether SCTP is loaded as a module or
 *  linked directly with the kernel.
 */

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
#include "netinet/sctp.h"

#ifdef CONFIG_SCTP_HMAC_MD5
#define SCTP_HMAC_DEFAULT SCTP_HMAC_MD5
#else
#ifdef CONFIG_SCTP_HMAC_SHA1
#define SCTP_HMAC_DEFAULT SCTP_HMAC_SHA_1
#else
#define SCTP_HMAC_DEFAULT SCTP_HMAC_NONE
#endif
#endif

/*
 *  sysctls (controllable whether module loaded or not)
 */
int sysctl_sctp_max_istreams = 33;
int sysctl_sctp_req_ostreams = 1;
int sysctl_sctp_rto_initial = 3 * HZ;
int sysctl_sctp_rto_min = 1 * HZ;
int sysctl_sctp_rto_max = 60 * HZ;
int sysctl_sctp_heartbeat_itvl = 30 * HZ;
int sysctl_sctp_max_init_retries = 8;
int sysctl_sctp_valid_cookie_life = 60 * HZ;
int sysctl_sctp_max_sack_delay = 200 * HZ / 1000;
int sysctl_sctp_path_max_retrans = 5;
int sysctl_sctp_assoc_max_retrans = 10;
int sysctl_sctp_mac_type = SCTP_HMAC_DEFAULT;
int sysctl_sctp_csum_type = SCTP_CSUM_CRC32C;
int sysctl_sctp_cookie_inc = 1 * HZ;
int sysctl_sctp_throttle_itvl = 50 * HZ / 1000;
int sysctl_sctp_mem[3];
int sysctl_sctp_rmem[3] = { 4 * 1024, 87380, 87380 * 2 };
int sysctl_sctp_wmem[3] = { 4 * 1024, 16 * 1024, 128 * 1024 };

#ifdef CONFIG_INET_ECN
int sysctl_sctp_ecn = 1;
#else
int sysctl_sctp_ecn = 0;
#endif
int sysctl_sctp_adaptation_layer_info = 0;
int sysctl_sctp_partial_reliability = 0;
int sysctl_sctp_max_burst = 4;

int min_sctp_max_istreams = 1;
int min_sctp_req_ostreams = 1;
int min_sctp_max_burst = 1;

/*
 *  stats (reported whether module loaded or not)
 */
struct sctp_mib sctp_statistics[NR_CPUS * 2];

/*
 *  kmem caches
 *
 *  kmem cache pointers are never deleted between module loading and
 *  unloading, they are just shrunk.
 */
kmem_cachep_t sctp_bind_cachep = NULL;
kmem_cachep_t sctp_dest_cachep = NULL;
kmem_cachep_t sctp_srce_cachep = NULL;
kmem_cachep_t sctp_strm_cachep = NULL;

struct sock *sctp_protolist = NULL;
atomic_t sctp_orphan_count = ATOMIC_INIT(0);
atomic_t sctp_socket_count = ATOMIC_INIT(0);

/*
 *  prot hook for module
 */
#ifdef CONFIG_SCTP_MODULE
struct proto *sctp_prot_hook = NULL;
#endif
