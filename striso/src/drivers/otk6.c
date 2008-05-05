/*****************************************************************************

 @(#) $RCSfile: otk6.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2008-05-05 15:34:58 $

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

 Last Modified $Date: 2008-05-05 15:34:58 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: otk6.c,v $
 Revision 0.9.2.3  2008-05-05 15:34:58  brian
 - be strict with MORE_data and DATA_flag

 Revision 0.9.2.2  2008-04-29 00:02:00  brian
 - updated headers for release

 Revision 0.9.2.1  2008-04-25 08:33:38  brian
 - added man pages and skeletons

 *****************************************************************************/

#ident "@(#) $RCSfile: otk6.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2008-05-05 15:34:58 $"

static char const ident[] = "$RCSfile: otk6.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2008-05-05 15:34:58 $";

/*
 * This file provides a multiplexing driver for RFC 1006 (RFC 2126) OSI Transport over TCP.  The
 * upper multiplex interface is Transport Provider Interface (TPI) Revision 2.0.0 with OSI options
 * for use with XTI-OSI and the lower multiplex interface is also Transport Provider Interface (TPI)
 * Revision 2.0.0 with TCP or UDP options for use with XTI-INET.  A daemon opens the control stream
 * and I_PLINK's a number of TCP and UDP Streams beneath the multiplexing driver for use by upper
 * Streams.  One lower TCP Stream is bound to port 102 as a listener and another is bound to port
 * 399 as a listener.  All other TCP Streams are used for accepting connections or for outbound
 * connections.  One lower UDP Stream is bound as a listener to port 102 and another is bound to
 * port 399.  No other UDP Streams are necessary.
 *
 * Procedures for Class 0:
 *
 * Procedures applicable at all times.  The transport entities shall use the following procedures:
 * a) TPDU transfer (see 6.2); b) association of TPDUs with trasnport connections (see 6.9); c)
 * treatment of protocol errors (see 6.22); d) error release (see 6.8).
 *
 * Connection establishment.  The transport entities shall use the following procedures: a)
 * assignment to network connection (see 6.1.1); then b) connection establishment (see 6.5) and, if
 * appropriate, connection refusal (see 6.6), subject to the following constraints: 1) the CR and
 * CC-TPDUs shall contain no parameter fields in the variable part of the header other than those
 * for Transport-Selector, maximum TPDU size, and preferred maximum TPDU size; 2) the CR and
 * CC-TPDUs shall not contain a data field.
 *
 * Data transfer.  The transport entities shall use the segmenting and reassembling procedures (see
 * 6.3).
 *
 * Release.  The transport entities shall use the implicit variant of the normal release procedure
 * (see 6.7.1.4).  NOTE -- The lifetime of the transport connection is directly correlated with the
 * lifetime of the network connection.
 */

#include <sys/os7/compat.h>

#ifdef LINUX
#undef ASSERT

#define t_tst_bit(nr,addr)	test_bit(nr,addr)
#define t_set_bit(nr,addr)	__set_bit(nr,addr)
#define t_clr_bit(nr,addr)	__clear_bit(nr,addr)

#include <linux/interrupt.h>

#endif				/* LINUX */

#if defined HAVE_TIHDR_H
#   include <tihdr.h>
#else
#   include <sys/tihdr.h>
#endif

#include <sys/xti.h>
#include <sys/xti_inet.h>
#include <sys/xti_osi.h>

#define OTK6_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define OTK6_EXTRA	"Part of the OpenSS7 Stack for Linux Fast-STREAMS"
#define OTK6_COPYRIGHT	"Copyright (c) 1997-2008 OpenSS7 Corporation.  All Rights Reserved."
#define OTK6_REVISION	"OpenSS7 $RCSfile: otk6.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2008-05-05 15:34:58 $"
#define OTK6_DEVICE	"SVR 4.2 STREAMS RFC1006 TPI OSI Transport Provider Driver"
#define OTK6_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define OTK6_LICENSE	"GPL"
#define OTK6_BANNER	OTK6_DESCRIP	"\n" \
			OTK6_EXTRA	"\n" \
			OTK6_COPYRIGHT	"\n" \
			OTK6_DEVICE	"\n" \
			OTK6_CONTACT
#define OTK6_SPLASH	OTK6_DESCRIP	" - " \
			OTK6_REVISION

#ifdef LINUX
MODULE_AUTHOR(OTK6_CONTACT);
MODULE_DESCRIPTION(OTK6_DESCRIP);
MODULE_SUPPORTED_DEVICE(OTK6_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(OTK6_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-otk6");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifdef LINUX
#ifdef MODULE_ALIAS
#ifdef LFS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_OTK6_MODID));
MODULE_ALIAS("streams-driver-otk6");
MODULE_ALIAS("streams-module-otk6");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_OTK6_MAJOR));
MODULE_ALIAS("/dev/streams/otk6");
MODULE_ALIAS("/dev/streams/otk6/*");
MODULE_ALIAS("/dev/streams/clone/otk6");
#endif				/* LFS */
MODULE_ALIAS("char-major-" __stringify(OTK6_CMAJOR_0));
MODULE_ALIAS("char-major-" __stringify(OTK6_CMAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(OTK6_CMAJOR_0) "-0");
MODULE_ALIAS("char-major-" __stringify(OTK6_CMAJOR_0) "-" __stringify(OTK6_CMINOR));
MODULE_ALIAS("/dev/otk6");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */
/*
 * STREAMS Definitions
 * ===================
 */
#define DRV_ID		OTK6_DRV_ID
#define DRV_NAME	OTK6_DRV_NAME
#define MOD_ID		OTK6_MOD_ID
#define CMAJORS		OTK6_CMAJORS
#define CMAJOR_0	OTK6_CMAJOR_0
#define UNITS		OTK6_UNITS
#ifdef MODULE
#define DRV_BANNER	OTK6_BANNER
#else				/* MODULE */
#define DRV_BANNER	OTK6_SPLASH
#endif				/* MODULE */

static struct module_info tp_minfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME,		/* Module name */
	.mi_minpsz = 0,			/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,		/* Max packet size accepted */
	.mi_hiwat = 1 << 15,		/* Hi water mark */
	.mi_lowat = 1 << 10,		/* Lo water mark */
};

static struct module_stat tp_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat tp_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static struct module_info np_minfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME,		/* Module name */
	.mi_minpsz = 0,			/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,		/* Max packet size accepted */
	.mi_hiwat = 1 << 15,		/* Hi water mark */
	.mi_lowat = 1 << 10,		/* Lo water mark */
};

static struct module_stat np_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat np_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

/*
 * XTI-OSI Options
 * ===============
 */

struct tp_xti {
	t_uscalar_t xti_debug[4];
	struct t_linger xti_linger;
	t_uscalar_t xti_rcvbuf;
	t_uscalar_t xti_rcvlowat;
	t_uscalar_t xti_sndbuf;
	t_uscalar_t xti_sndlowat;
};
struct tp_tco {
	struct thrpt throughput;
	struct transdel transdel;
	struct rate reserrorrate;
	struct rate transffailprob;
	struct rate estfailprob;
	struct rate relfailprob;
	struct rate estdelay;
	struct rate reldelay;
	struct rate connresil;
	t_uscalar_t protection;
	t_uscalar_t priority;
	t_uscalar_t expd;

	t_uscalar_t ltpdu;
	t_uscalar_t acktime;
	t_uscalar_t reastime;
	t_uscalar_t prefclass;
	t_uscalar_t altclass1;
	t_uscalar_t altclass2;
	t_uscalar_t altclass3;
	t_uscalar_t altclass4;
	t_uscalar_t extform;
	t_uscalar_t flowctrl;
	t_uscalar_t checksum;
	t_uscalar_t netexp;
	t_uscalar_t netrecptcf;
};
struct tp_tcl {
	t_uscalar_t transdel;
	struct rate reserrorrate;
	t_uscalar_t protection;
	t_uscalar_t priority;
	t_uscalar_t checksum;
};
struct tp_ip {
	unsigned char options[40];
	unsigned char tos;
	unsigned char ttl;
	unsigned int reuseaddr;
	unsigned int dontroute;
	unsigned int broadcast;
	uint32_t addr;
};
struct tp_udp {
	t_uscalar_t checksum;
};
struct tp_tcp {
	t_uscalar_t nodelay;
	t_uscalar_t maxseg;
	struct t_kpalive keepalive;
	t_uscalar_t cork;
	t_uscalar_t keepidle;
	t_uscalar_t keepitvl;
	t_uscalar_t keepcnt;
	t_uscalar_t syncnt;
	t_uscalar_t linger2;
	t_uscalar_t defer_accept;
	t_uscalar_t window_clamp;
	struct t_tcp_info info;
	t_uscalar_t quickack;
};

struct tp_options {
	unsigned long flags[3];		/* at least 96 flags */
	struct tp_xti xti;
	struct tp_tco tco;
	struct tp_tcl tcl;
	struct tp_ip ip;
	struct tp_udp udp;
	struct tp_tcp tcp;
};

/* Options flags */
enum {
	_T_BIT_XTI_DEBUG = 0,
	_T_BIT_XTI_LINGER,
	_T_BIT_XTI_RCVBUF,
	_T_BIT_XTI_RCVLOWAT,
	_T_BIT_XTI_SNDBUF,
	_T_BIT_XTI_SNDLOWAT,

	_T_BIT_TCO_THROUGHPUT,
	_T_BIT_TCO_TRANSDEL,
	_T_BIT_TCO_RESERRORRATE,
	_T_BIT_TCO_TRANSFFAILPROB,
	_T_BIT_TCO_ESTFAILPROB,
	_T_BIT_TCO_RELFAILPROB,
	_T_BIT_TCO_ESTDELAY,
	_T_BIT_TCO_RELDELAY,
	_T_BIT_TCO_CONNRESIL,
	_T_BIT_TCO_PROTECTION,
	_T_BIT_TCO_PRIORITY,
	_T_BIT_TCO_EXPD,

	_T_BIT_TCO_LTPDU,
	_T_BIT_TCO_ACKTIME,
	_T_BIT_TCO_REASTIME,
	_T_BIT_TCO_EXTFORM,
	_T_BIT_TCO_FLOWCTRL,
	_T_BIT_TCO_CHECKSUM,
	_T_BIT_TCO_NETEXP,
	_T_BIT_TCO_NETRECPTCF,
	_T_BIT_TCO_PREFCLASS,
	_T_BIT_TCO_ALTCLASS1,
	_T_BIT_TCO_ALTCLASS2,
	_T_BIT_TCO_ALTCLASS3,
	_T_BIT_TCO_ALTCLASS4,

	_T_BIT_TCL_TRANSDEL,
	_T_BIT_TCL_RESERRORRATE,
	_T_BIT_TCL_PROTECTION,
	_T_BIT_TCL_PRIORITY,
	_T_BIT_TCL_CHECKSUM,

	_T_BIT_IP_OPTIONS,
	_T_BIT_IP_TOS,
	_T_BIT_IP_TTL,
	_T_BIT_IP_REUSEADDR,
	_T_BIT_IP_DONTROUTE,
	_T_BIT_IP_BROADCAST,
	_T_BIT_IP_ADDR,
	_T_BIT_IP_RETOPTS,

	_T_BIT_UDP_CHECKSUM,

	_T_BIT_TCP_NODELAY,
	_T_BIT_TCP_MAXSEG,
	_T_BIT_TCP_KEEPALIVE,
	_T_BIT_TCP_CORK,
	_T_BIT_TCP_KEEPIDLE,
	_T_BIT_TCP_KEEPINTVL,
	_T_BIT_TCP_KEEPCNT,
	_T_BIT_TCP_SYNCNT,
	_T_BIT_TCP_LINGER2,
	_T_BIT_TCP_DEFER_ACCEPT,
	_T_BIT_TCP_WINDOW_CLAMP,
	_T_BIT_TCP_INFO,
	_T_BIT_TCP_QUICKACK,

	_T_BIT_LAST,
};

#define _T_FLAG_XTI_DEBUG		(1<<_T_BIT_XTI_DEBUG)
#define _T_FLAG_XTI_LINGER		(1<<_T_BIT_XTI_LINGER)
#define _T_FLAG_XTI_RCVBUF		(1<<_T_BIT_XTI_RCVBUF)
#define _T_FLAG_XTI_RCVLOWAT		(1<<_T_BIT_XTI_RCVLOWAT)
#define _T_FLAG_XTI_SNDBUF		(1<<_T_BIT_XTI_SNDBUF)
#define _T_FLAG_XTI_SNDLOWAT		(1<<_T_BIT_XTI_SNDLOWAT)

#define _T_MASK_XTI			(_T_FLAG_XTI_DEBUG \
					|_T_FLAG_XTI_LINGER \
					|_T_FLAG_XTI_RCVBUF \
					|_T_FLAG_XTI_RCVLOWAT \
					|_T_FLAG_XTI_SNDBUF \
					|_T_FLAG_XTI_SNDLOWAT)

#define _T_FLAG_TCO_THROUGHPUT		(1<<_T_BIT_TCO_THROUGHPUT)
#define _T_FLAG_TCO_TRANSDEL		(1<<_T_BIT_TCO_TRANSDEL)
#define _T_FLAG_TCO_RESERRORRATE	(1<<_T_BIT_TCO_RESERRORRATE)
#define _T_FLAG_TCO_TRANSFFAILPROB	(1<<_T_BIT_TCO_TRANSFFAILPROB)
#define _T_FLAG_TCO_ESTFAILPROB		(1<<_T_BIT_TCO_ESTFAILPROB)
#define _T_FLAG_TCO_RELFAILPROB		(1<<_T_BIT_TCO_RELFAILPROB)
#define _T_FLAG_TCO_ESTDELAY		(1<<_T_BIT_TCO_ESTDELAY)
#define _T_FLAG_TCO_RELDELAY		(1<<_T_BIT_TCO_RELDELAY)
#define _T_FLAG_TCO_CONNRESIL		(1<<_T_BIT_TCO_CONNRESIL)
#define _T_FLAG_TCO_PROTECTION		(1<<_T_BIT_TCO_PROTECTION)
#define _T_FLAG_TCO_PRIORITY		(1<<_T_BIT_TCO_PRIORITY)
#define _T_FLAG_TCO_EXPD		(1<<_T_BIT_TCO_EXPD)

#define _T_MASK_TCO_QOS			(_T_FLAG_TCO_THROUGHPUT \
					|_T_FLAG_TCO_TRANSDEL \
					|_T_FLAG_TCO_RESERRORRATE \
					|_T_FLAG_TCO_TRANSFFAILPROB \
					|_T_FLAG_TCO_ESTFAILPROB \
					|_T_FLAG_TCO_RELFAILPROB \
					|_T_FLAG_TCO_ESTDELAY \
					|_T_FLAG_TCO_RELDELAY \
					|_T_FLAG_TCO_CONNRESIL \
					|_T_FLAG_TCO_PROTECTION \
					|_T_FLAG_TCO_PRIORITY \
					|_T_FLAG_TCO_EXPD)

#define _T_FLAG_TCO_LTPDU		(1<<_T_BIT_TCO_LTPDU)
#define _T_FLAG_TCO_ACKTIME		(1<<_T_BIT_TCO_ACKTIME)
#define _T_FLAG_TCO_REASTIME		(1<<_T_BIT_TCO_REASTIME)
#define _T_FLAG_TCO_EXTFORM		(1<<_T_BIT_TCO_EXTFORM)
#define _T_FLAG_TCO_FLOWCTRL		(1<<_T_BIT_TCO_FLOWCTRL)
#define _T_FLAG_TCO_CHECKSUM		(1<<_T_BIT_TCO_CHECKSUM)
#define _T_FLAG_TCO_NETEXP		(1<<_T_BIT_TCO_NETEXP)
#define _T_FLAG_TCO_NETRECPTCF		(1<<_T_BIT_TCO_NETRECPTCF)
#define _T_FLAG_TCO_PREFCLASS		(1<<_T_BIT_TCO_PREFCLASS)
#define _T_FLAG_TCO_ALTCLASS1		(1<<_T_BIT_TCO_ALTCLASS1)
#define _T_FLAG_TCO_ALTCLASS2		(1<<_T_BIT_TCO_ALTCLASS2)
#define _T_FLAG_TCO_ALTCLASS3		(1<<_T_BIT_TCO_ALTCLASS3)
#define _T_FLAG_TCO_ALTCLASS4		(1<<_T_BIT_TCO_ALTCLASS4)

#define _T_MASK_TCO_MGMT		(_T_FLAG_TCO_LTPDU \
					|_T_FLAG_TCO_ACKTIME \
					|_T_FLAG_TCO_REASTIME \
					|_T_FLAG_TCO_EXTFORM \
					|_T_FLAG_TCO_FLOWCTRL \
					|_T_FLAG_TCO_CHECKSUM \
					|_T_FLAG_TCO_NETEXP \
					|_T_FLAG_TCO_NETRECPTCF \
					|_T_FLAG_TCO_PREFCLASS \
					|_T_FLAG_TCO_ALTCLASS1 \
					|_T_FLAG_TCO_ALTCLASS2 \
					|_T_FLAG_TCO_ALTCLASS3 \
					|_T_FLAG_TCO_ALTCLASS4)

#define _T_MASK_TCO			(_T_MASK_TCO_QOS|_T_MASK_TCO_MGMT)

#define _T_FLAG_TCL_TRANSDEL		(1<<_T_BIT_TCL_TRANSDEL)
#define _T_FLAG_TCL_RESERRORRATE	(1<<_T_BIT_TCL_RESERRORRATE)
#define _T_FLAG_TCL_PROTECTION		(1<<_T_BIT_TCL_PROTECTION)
#define _T_FLAG_TCL_PRIORITY		(1<<_T_BIT_TCL_PRIORITY)
#define _T_FLAG_TCL_CHECKSUM		(1<<_T_BIT_TCL_CHECKSUM)

#define _T_MASK_TCL			(_T_FLAG_TCL_TRANSDEL \
					|_T_FLAG_TCL_RESERRORRATE \
					|_T_FLAG_TCL_PROTECTION \
					|_T_FLAG_TCL_PRIORITY \
					|_T_FLAG_TCL_CHECKSUM)

#define _T_FLAG_IP_OPTIONS		(1<<_T_BIT_IP_OPTIONS)
#define _T_FLAG_IP_TOS			(1<<_T_BIT_IP_TOS)
#define _T_FLAG_IP_TTL			(1<<_T_BIT_IP_TTL)
#define _T_FLAG_IP_REUSEADDR		(1<<_T_BIT_IP_REUSEADDR)
#define _T_FLAG_IP_DONTROUTE		(1<<_T_BIT_IP_DONTROUTE)
#define _T_FLAG_IP_BROADCAST		(1<<_T_BIT_IP_BROADCAST)
#define _T_FLAG_IP_ADDR			(1<<_T_BIT_IP_ADDR)
#define _T_FLAG_IP_RETOPTS		(1<<_T_BIT_IP_RETOPTS)

#define _T_MASK_IP			(_T_FLAG_IP_OPTIONS \
					|_T_FLAG_IP_TOS \
					|_T_FLAG_IP_TTL \
					|_T_FLAG_IP_REUSEADDR \
					|_T_FLAG_IP_DONTROUTE \
					|_T_FLAG_IP_BROADCAST \
					|_T_FLAG_IP_ADDR \
					|_T_FLAG_IP_RETOPTS)

#define _T_FLAG_UDP_CHECKSUM		(1<<_T_BIT_UDP_CHECKSUM)

#define _T_MASK_UDP			(_T_FLAG_UDP_CHECKSUM)

#define _T_FLAG_TCP_NODELAY		(1<<_T_BIT_TCP_NODELAY)
#define _T_FLAG_TCP_MAXSEG		(1<<_T_BIT_TCP_MAXSEG)
#define _T_FLAG_TCP_KEEPALIVE		(1<<_T_BIT_TCP_KEEPALIVE)
#define _T_FLAG_TCP_CORK		(1<<_T_BIT_TCP_CORK)
#define _T_FLAG_TCP_KEEPIDLE		(1<<_T_BIT_TCP_KEEPIDLE)
#define _T_FLAG_TCP_KEEPINTVL		(1<<_T_BIT_TCP_KEEPINTVL)
#define _T_FLAG_TCP_KEEPCNT		(1<<_T_BIT_TCP_KEEPCNT)
#define _T_FLAG_TCP_SYNCNT		(1<<_T_BIT_TCP_SYNCNT)
#define _T_FLAG_TCP_LINGER2		(1<<_T_BIT_TCP_LINGER2)
#define _T_FLAG_TCP_DEFER_ACCEPT	(1<<_T_BIT_TCP_DEFER_ACCEPT)
#define _T_FLAG_TCP_WINDOW_CLAMP	(1<<_T_BIT_TCP_WINDOW_CLAMP)
#define _T_FLAG_TCP_INFO		(1<<_T_BIT_TCP_INFO)
#define _T_FLAG_TCP_QUICKACK		(1<<_T_BIT_TCP_QUICKACK)

#define _T_MASK_TCP			(_T_FLAG_TCP_NODELAY \
					|_T_FLAG_TCP_MAXSEG \
					|_T_FLAG_TCP_KEEPALIVE \
					|_T_FLAG_TCP_CORK \
					|_T_FLAG_TCP_KEEPIDLE \
					|_T_FLAG_TCP_KEEPINTVL \
					|_T_FLAG_TCP_KEEPCNT \
					|_T_FLAG_TCP_SYNCNT \
					|_T_FLAG_TCP_LINGER2 \
					|_T_FLAG_TCP_DEFER_ACCEPT \
					|_T_FLAG_TCP_WINDOW_CLAMP \
					|_T_FLAG_TCP_INFO \
					|_T_FLAG_TCP_QUICKACK)

STATIC const t_uscalar_t tp_space[_T_BIT_LAST] = {
	T_SPACE(4 * sizeof(t_uscalar_t)),
	_T_SPACE_SIZEOF(struct t_linger),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(struct thrpt),
	_T_SPACE_SIZEOF(struct transdel),
	_T_SPACE_SIZEOF(struct rate),
	_T_SPACE_SIZEOF(struct rate),
	_T_SPACE_SIZEOF(struct rate),
	_T_SPACE_SIZEOF(struct rate),
	_T_SPACE_SIZEOF(struct rate),
	_T_SPACE_SIZEOF(struct rate),
	_T_SPACE_SIZEOF(struct rate),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(struct rate),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(unsigned char),
	_T_SPACE_SIZEOF(unsigned char),
	_T_SPACE_SIZEOF(unsigned char),
	_T_SPACE_SIZEOF(unsigned int),
	_T_SPACE_SIZEOF(unsigned int),
	_T_SPACE_SIZEOF(unsigned int),
	_T_SPACE_SIZEOF(uint32_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(struct t_kpalive),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(t_uscalar_t),
	_T_SPACE_SIZEOF(struct t_tcp_info),
	_T_SPACE_SIZEOF(t_uscalar_t),
};

/*
 * Private data structures.
 * ========================
 */

struct tp;
struct np;

struct tp_tsapid {
	uint len;			/* length of contents of buffer */
	unsigned char buf[64];		/* the buffer */
};

struct tp {
	struct np *np;
	struct tp *next;
	queue_t *oq;
	t_scalar_t state;
	t_scalar_t oldstate;
	uchar n_prov;			/* N_CONS or N_CLNS */
	uchar class;			/* 0, 1, 2, 3 or 4 */
	uchar options;			/* class options */
	uchar addopts;			/* additional options */
	struct T_info_ack info;		/* protocol information */
	struct tp_tsapid src;		/* srce TSAP identifier */
	struct tp_tsapid dst;		/* dest TSAP identifier */
	struct {
		struct tp_options req;	/* request options */
		struct tp_options res;	/* response options */
	} opts;
};

#define TP_PRIV(__q) ((struct tp *)((__q)->q_ptr))

struct np {
	struct tp *tp;
	queue_t *oq;
	t_scalar_t state;
	t_scalar_t oldstate;
	struct T_info_ack info;
	struct sockaddr_in src;
	struct sockaddr_in dst;
};

static const char *
tpi_state_name(t_scalar_t state)
{
	switch (state) {
	case TS_UNBND:
		return ("TS_UNBND");
	case TS_WACK_BREQ:
		return ("TS_WACK_BREQ");
	case TS_WACK_UREQ:
		return ("TS_WACK_UREQ");
	case TS_IDLE:
		return ("TS_IDLE");
	case TS_WACK_OPTREQ:
		return ("TS_WACK_OPTREQ");
	case TS_WACK_CREQ:
		return ("TS_WACK_CREQ");
	case TS_WCON_CREQ:
		return ("TS_WCON_CREQ");
	case TS_WRES_CIND:
		return ("TS_WRES_CIND");
	case TS_WACK_CRES:
		return ("TS_WACK_CRES");
	case TS_DATA_XFER:
		return ("TS_DATA_XFER");
	case TS_WIND_ORDREL:
		return ("TS_WIND_ORDREL");
	case TS_WREQ_ORDREL:
		return ("TS_WREQ_ORDREL");
	case TS_WACK_DREQ6:
		return ("TS_WACK_DREQ6");
	case TS_WACK_DREQ7:
		return ("TS_WACK_DREQ7");
	case TS_WACK_DREQ9:
		return ("TS_WACK_DREQ9");
	case TS_WACK_DREQ10:
		return ("TS_WACK_DREQ10");
	case TS_WACK_DREQ11:
		return ("TS_WACK_DREQ11");
	case TS_NOSTATES:
		return ("TS_NOSTATES");
	default:
		return ("(unknown)");
	}
}

static inline fastcall t_scalar_t
tp_get_state(struct tp *tp)
{
	return (tp->state);
}

static inline fastcall t_scalar_t
tp_set_state(struct tp *tp, t_scalar_t newstate)
{
	t_scalar_t oldstate = tp_get_state(tp);

	if (newstate != oldstate) {
		mi_strlog(tp->oq, STRLOGST, SL_TRACE, "%s <- %s", tpi_statename(newstate),
			  tpi_statename(oldstate));
		tp->oldstate = oldstate;
		tp->info.CURRENT_state = newstate;
		tp->state = newstate;
	}
	return (newstate);
}

static inline fastcall t_scalar_t
np_chk_state(struct np *np, t_scalar_t mask)
{
	return (((1 << np_get_state(np)) & (mask)) != 0);
}

static inline fastcall t_scalar_t
np_not_state(struct np *np, t_scalar_t mask)
{
	return (((1 << np_get_state(np)) & (mask)) == 0);
}

static inline fastcall t_scalar_t
np_get_state(struct np *np)
{
	return (np->state);
}

static inline fastcall t_scalar_t
np_set_state(struct np *np, t_scalar_t newstate)
{
	t_scalar_t oldstate = np_get_state(np);

	if (newstate != oldstate) {
		mi_strlog(np->oq, STRLOGST, SL_TRACE, "%s <- %s", tpi_statename(newstate),
			  tpi_statename(oldstate));
		np->oldstate = oldstate;
		np->info.CURRENT_state = newstate;
		np->state = newstate;
	}
	return (newstate);
}

static inline fastcall t_scalar_t
np_chk_state(struct np *np, t_scalar_t mask)
{
	return (((1 << np_get_state(np)) & (mask)) != 0);
}

static inline fastcall t_scalar_t
np_not_state(struct np *np, t_scalar_t mask)
{
	return (((1 << np_get_state(np)) & (mask)) == 0);
}

/*
 * Option Processing
 * =================
 */
#define T_SPACE(len) \
	(sizeof(struct t_opthdr) + T_ALIGN(len))
#define T_LENGTH(len) \
	(sizeof(struct t_opthdr) + len)
#define _T_SPACE_SIZEOF(s) \
	T_SPACE(sizeof(s))
#define _T_LENGTH_SIZEOF(s) \
	T_LENGTH(sizeof(s))

static int
tp_size_opts(ulong *flags)
{
	int bit = 0, size = 0;

	for (bit = 0; bit < _T_BIT_LAST; bit++)
		if (t_tst_bit(bit, flags))
			size += tp_space[bit];
	return (size);
}

/**
 * tp_size_conn_opts - size conenction indiciation or confirmation options
 * @tp: transport endpoint
 */
static int
tp_size_conn_opts(struct tp *tp)
{
	int bit = 0, size = 0;

	for (bit = 0; bit < _T_BIT_LAST; bit++)
		if (t_tst_bit(bit, &tp->opts.req.flags) || t_tst_bit(bit, &tp->opts.res.flags))
			size += tp_space[bit];
	return (size);
}

/**
 * t_build_conn_ind_opts: - Build connection indication options
 * @tp: transport connection private structure
 * @rem: a pointer to an structure containing options from the CR-TPDU of the remote end.
 * @optr: a pointer into the buffer to contain the XTI options.
 * @olen: the length of the buffer area to contain the XTI options.
 *
 * The return value is the size of the options buffer, or a negative error number on software fault.
 *
 * These are only options with end-to-end significance.  The options are extracted directly from the
 * CR-TPDU.  Note that the options structure is held in a message buffer that is kept with the
 * CR-TPDU and represents the outstanding connection indication.  Note that additional options are
 * those associated with the TCP connection indication.
 *
 * Quality of Service is a list of parameters.  For each parameter, the values in the various TS
 * primitives are related so that: a) in the T-CONNECT request primitive, any defined value is
 * allowed; b) in the T-CONNECT indication primitive, the QOS parameter value is equal to or poorer
 * than the value in the T-CONNECT request primitive, except for the TC protection, that must have
 * the same value as specified in the T-CONNECT request primitive; c) in the T-CONNECT response
 * primitive, the QOS parameter value indicated is equal to or poorer than the value in the
 * T-CONNECT indication primitive; d) in the T-CONNECT confirm primtive, the QOS parameter value
 * indicated is equal to the value in the T-CONNECT response primitive.
 */
static int
tp_build_conn_ind_opts(struct tp *tp, struct tp_options *rem, unsigned char *optr, size_t olen)
{
	struct t_opthdr *oh;

	if (optr == NULL || olen == 0)
		return (0);
	oh = _T_OPT_FIRSTHDR_OFS(optr, olen, 0);
	if (t_tst_bit(_T_BIT_TCO_THROUGHPUT, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.throughput);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_THROUGHPUT;
		oh->status = T_SUCCESS;
		*((struct thrpt *) T_OPT_DATA(oh)) = rem->tco.throughput;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_TCO_TRANSDEL, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.transdel);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_TRANSDEL;
		oh->status = T_SUCCESS;
		*((struct transdel *) T_OPT_DATA(oh)) = rem->tco.transdel;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_TCO_RESERRORRATE, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.reserrorrate);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_RESERRORRATE;
		oh->status = T_SUCCESS;
		*((struct rate *) T_OPT_DATA(oh)) = rem->tco.reserrorrate;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_TCO_TRANSFFAILPROB, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.transffailprob);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_TRANSFFAILPROB;
		oh->status = T_SUCCESS;
		*((struct rate *) T_OPT_DATA(oh)) = rem->tco.transffailprob;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_TCO_ESTFAILPROB, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.estfailprob);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_ESTFAILPROB;
		oh->status = T_SUCCESS;
		*((struct rate *) T_OPT_DATA(oh)) = rem->tco.estfailprob;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_TCO_RELFAILPROB, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.relfailprob);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_RELFAILPROB;
		oh->status = T_SUCCESS;
		*((struct rate *) T_OPT_DATA(oh)) = rem->tco.relfailprob;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_TCO_ESTDELAY, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.estdelay);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_ESTDELAY;
		oh->status = T_SUCCESS;
		*((struct rate *) T_OPT_DATA(oh)) = rem->tco.estdelay;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_TCO_RELDELAY, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.reldelay);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_RELDELAY;
		oh->status = T_SUCCESS;
		*((struct rate *) T_OPT_DATA(oh)) = rem->tco.reldelay;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_TCO_CONNRESIL, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.connresil);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_CONNRESIL;
		oh->status = T_SUCCESS;
		*((struct rate *) T_OPT_DATA(oh)) = rem->tco.connresil;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_TCO_PROTECTION, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.protection);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_PROTECTION;
		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) = rem->tco.protection;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_TCO_PRIORITY, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.priority);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_PRIORITY;
		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) = rem->tco.priority;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_TCO_EXPD, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.expd);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_EXPD;
		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) = rem->tco.expd;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_TCO_LTPDU, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.ltpdu);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_LTPDU;
		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) = rem->tco.ltpdu;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_TCO_ACKTIME, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.acktime);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_ACKTIME;
		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) = rem->tco.acktime;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_TCO_REASTIME, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.reastime);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_REASTIME;
		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) = rem->tco.reastime;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_TCO_EXTFORM, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.extform);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_EXTFORM;
		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) = rem->tco.extform;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_TCO_FLOWCTRL, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.flowctrl);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_FLOWCTRL;
		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) = rem->tco.flowctrl;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_TCO_CHECKSUM, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.checksum);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_CHECKSUM;
		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) = rem->tco.checksum;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_TCO_NETEXP, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.netexp);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_NETEXP;
		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) = rem->tco.netexp;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_TCO_NETRECPTCF, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.netrecptcf);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_NETRECPTCF;
		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) = rem->tco.netrecptcf;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_TCO_PREFCLASS, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.prefclass);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_PREFCLASS;
		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) = rem->tco.prefclass;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_TCO_ALTCLASS1, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.altclass1);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_ALTCLASS1;
		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) = rem->tco.altclass1;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_TCO_ALTCLASS2, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.altclass2);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_ALTCLASS2;
		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) = rem->tco.altclass2;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_TCO_ALTCLASS3, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.altclass3);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_ALTCLASS3;
		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) = rem->tco.altclass3;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_TCO_ALTCLASS4, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.altclass4);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_ALTCLASS4;
		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) = rem->tco.altclass4;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	/* The only IP/UDP/TCP options with end-to-end significance. */
#if 0
	if (t_tst_bit(_T_BIT_IP_OPTIONS, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->ip.options);
		oh->level = T_INET_IP;
		oh->name = T_IP_OPTIONS;
		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) = rem->ip.options;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
#endif
	if (t_tst_bit(_T_BIT_IP_TOS, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->ip.tos);
		oh->level = T_INET_IP;
		oh->name = T_IP_TOS;
		oh->status = T_SUCCESS;
		*((unsigned char *) T_OPT_DATA(oh)) = rem->ip.tos;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	/* expect oh to be NULL (filled buffer) */
	if (oh != NULL)
		mi_strlog(tp->oq, 0, SL_ERROR, "%s: software error", __FUNCTION__);
	return (olen);
      efault:
	if (oh != NULL)
		mi_strlog(tp->oq, 0, SL_ERROR, "%s: software fault", __FUNCTION__);
	return (-EFAULT);
}

/**
 * tp_build_conn_con_opts: - build connection confirmation options
 * @tp: transport connection private structure
 * @loc: local options (requested on T_CONN_REQ)
 * @rem: remote options (contained in CC-TPDU)
 * @optr: pointer to buffer to contain options
 * @olen: length of buffer to contain options
 *
 * For each of the options that were requested by the T_CONN_REQ and each of the end-to-end options
 * returned in the CC-TPDU, build an option in the buffer with the negotiated value unless the
 * negotiated value is in error (e.g. absolute requirement not met, upgrading options, etc.).  This
 * and the XTI-OSI description make it appear that two structures are required to be held for
 * initiators: one holding t_optmgmt() options, another containing t_connect() options.  When the
 * CC-TPDU options and the CR-TPDU options can be negotiated into the t_optmgmt() options.
 */
static int
tp_build_conn_con_opts(struct tp *tp, struct tp_options *loc, struct tp_options *rem, uchar *optr,
		       uint olen)
{
	struct t_opthdr *oh;

	if (optr == NULL || olen == 0)
		return (0);
	oh = _T_OPT_FIRSTHDR_OFS(optr, olen, 0);
	if (t_tst_bit(_T_BIT_TCO_THROUGHPUT, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.throughput);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_THROUGHPUT;
		oh->status = T_SUCCESS;
		*((struct thrpt *) T_OPT_DATA(oh)) = rem->tco.throughput;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_TCO_TRANSDEL, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.transdel);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_TRANSDEL;
		oh->status = T_SUCCESS;
		*((struct transdel *) T_OPT_DATA(oh)) = rem->tco.transdel;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_TCO_RESERRORRATE, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.reserrorrate);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_RESERRORRATE;
		oh->status = T_SUCCESS;
		*((struct rate *) T_OPT_DATA(oh)) = rem->tco.reserrorrate;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_TCO_TRANSFFAILPROB, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.transffailprob);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_TRANSFFAILPROB;
		oh->status = T_SUCCESS;
		*((struct rate *) T_OPT_DATA(oh)) = rem->tco.transffailprob;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_TCO_ESTFAILPROB, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.estfailprob);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_ESTFAILPROB;
		oh->status = T_SUCCESS;
		*((struct rate *) T_OPT_DATA(oh)) = rem->tco.estfailprob;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_TCO_RELFAILPROB, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.relfailprob);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_RELFAILPROB;
		oh->status = T_SUCCESS;
		*((struct rate *) T_OPT_DATA(oh)) = rem->tco.relfailprob;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_TCO_ESTDELAY, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.estdelay);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_ESTDELAY;
		oh->status = T_SUCCESS;
		*((struct rate *) T_OPT_DATA(oh)) = rem->tco.estdelay;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_TCO_RELDELAY, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.reldelay);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_RELDELAY;
		oh->status = T_SUCCESS;
		*((struct rate *) T_OPT_DATA(oh)) = rem->tco.reldelay;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_TCO_CONNRESIL, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.connresil);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_CONNRESIL;
		oh->status = T_SUCCESS;
		*((struct rate *) T_OPT_DATA(oh)) = rem->tco.connresil;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_TCO_PROTECTION, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.protection);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_PROTECTION;
		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) = rem->tco.protection;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_TCO_PRIORITY, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.priority);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_PRIORITY;
		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) = rem->tco.priority;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_TCO_EXPD, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.expd);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_EXPD;
		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) = rem->tco.expd;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_TCO_LTPDU, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.ltpdu);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_LTPDU;
		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) = rem->tco.ltpdu;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_TCO_ACKTIME, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.acktime);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_ACKTIME;
		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) = rem->tco.acktime;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_TCO_REASTIME, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.reastime);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_REASTIME;
		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) = rem->tco.reastime;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_TCO_EXTFORM, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.extform);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_EXTFORM;
		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) = rem->tco.extform;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_TCO_FLOWCTRL, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.flowctrl);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_FLOWCTRL;
		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) = rem->tco.flowctrl;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_TCO_CHECKSUM, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.checksum);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_CHECKSUM;
		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) = rem->tco.checksum;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_TCO_NETEXP, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.netexp);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_NETEXP;
		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) = rem->tco.netexp;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_TCO_NETRECPTCF, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.netrecptcf);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_NETRECPTCF;
		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) = rem->tco.netrecptcf;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_TCO_PREFCLASS, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.prefclass);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_PREFCLASS;
		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) = rem->tco.prefclass;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_TCO_ALTCLASS1, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.altclass1);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_ALTCLASS1;
		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) = rem->tco.altclass1;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_TCO_ALTCLASS2, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.altclass2);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_ALTCLASS2;
		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) = rem->tco.altclass2;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_TCO_ALTCLASS3, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.altclass3);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_ALTCLASS3;
		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) = rem->tco.altclass3;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_TCO_ALTCLASS4, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->tco.altclass4);
		oh->level = T_ISO_TP;
		oh->name = T_TCO_ALTCLASS4;
		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) = rem->tco.altclass4;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	/* The only IP/UDP/TCP options with end-to-end significance. */
#if 0
	if (t_tst_bit(_T_BIT_IP_OPTIONS, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->ip.options);
		oh->level = T_INET_IP;
		oh->name = T_IP_OPTIONS;
		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) = rem->ip.options;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
#endif
	if (t_tst_bit(_T_BIT_IP_TOS, rem->flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->ip.tos);
		oh->level = T_INET_IP;
		oh->name = T_IP_TOS;
		oh->status = T_SUCCESS;
		*((unsigned char *) T_OPT_DATA(oh)) = rem->ip.tos;
		oh = _T_OPT_NEXTHDR_OFS(optr, olen, oh, 0);
	}
	/* expect oh to be NULL (filled buffer) */
	if (oh != NULL)
		mi_strlog(tp->oq, 0, SL_ERROR, "%s: software error", __FUNCTION__);
	return (olen);
      efault:
	if (oh != NULL)
		mi_strlog(tp->oq, 0, SL_ERROR, "%s: software fault", __FUNCTION__);
	return (-EFAULT);
}

/**
 * t_negotiate_conn_res_opts: - parse connection response options
 * @tp: transport endpoint private structure
 * @loc: local options structure (from transport endpoint)
 * @rem: remote options structure (from corresponding T_CONN_IND)
 * @iptr: input options (from T_CONN_RES primitives) pointer
 * @ilen: input options (from T_CONN_RES primitives) length
 *
 * Parses options provided in a T_CONN_RES primitive and applies them, along with options assigned
 * to the transport endpoint using T_OPTMGMT_REQ, to the options previously received in the
 * T_CONN_IND, to modify the transport endpoint options structure for generation of an appropriate
 * CC-TPDU, and to reflect the options associated with the transport connection during its
 * establishment.
 *
 * For each of the options of end-to-end significance, when they were specified in the T_CONN_IND
 * (rem) structure, they must have a value of the same or lesser quality when they appear in the.
 */
static int
tp_negotiate_conn_res_opts(struct tp *tp, struct tp_options *loc, struct tp_options *rem, uchar *iptr, uint ilen) {
}

/**
 * tp_parse_opts: - parts options into an options structure
 * @tp: transport endpoing
 * @opts: defaulted options structure
 * @iptr: option list pointer
 * @ilen: option list length
 *
 * Parses the options list and parses any options in the list into the provided options structure.
 */
static int
tp_parse_opts(struct tp *tp, struct tp_options *op, uchar *iptr, uint ilen)
{
	const struct t_opthdr *ih;
	ulong *flags = &op->flags;
	int optlen;

	for (ih = _T_OPT_FIRSTHDR_OFS(iptr, ilen, 0); ih; ih = _T_OPT_NEXTHDR_OFS(iptr, ilen, ih, 0)) {
		if (ih->len < sizeof(*ih))
			goto einval;
		if ((uchar *)ih + ih->len > iptr + len)
			goto einval;
		optlen = ih->len - sizeof(*ih);
		switch (ih->level) {
		case XTI_GENERIC:
		{
			struct tp_xti *xti = &op->xti;

			switch (ih->name) {
			case XTI_DEBUG:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (((ih->len - sizeof(*ih)) % sizeof(t_uscalar_t)) != 0)
					goto einval;
				t_set_bit(_T_BIT_XTI_DEBUG, flags);

				if (ih->len >= sizeof(*ih) + 4 * sizeof(t_uscalar_t))
					xti->xti_debug[3] = valp[3];
				else
					xti->xti_debug[3] = 0;
				if (ih->len >= sizeof(*ih) + 3 * sizeof(t_uscalar_t))
					xti->xti_debug[2] = valp[2];
				else
					xti->xti_debug[2] = 0;
				if (ih->len >= sizeof(*ih) + 2 * sizeof(t_uscalar_t))
					xti->xti_debug[1] = valp[1];
				else
					xti->xti_debug[1] = 0;
				if (ih->len >= sizeof(*ih) + 1 * sizeof(t_uscalar_t))
					xti->xti_debug[0] = valp[0];
				else
					xti->xti_debug[0] = 0;
				continue;
			}
			}
		}
		case T_ISO_TP:
		{
			struct tp_tcl *tcl = &op->tcl;
			struct tp_tco *tco = &op->tco;
		}
		case T_INET_IP:
		{
			struct tp_ip *ip = &op->ip;
		}
		case T_INET_UDP:
		{
			struct tp_udp *udp = &op->udp;
		}
		case T_INET_TCP:
		{
			struct tp_tcp *tcp = &op->tcp;
		}
		}
	}
}

/**
 * t_build_conn_con_opts - Build connection confirmation options
 * @tp: transport endpoint
 * @rem: remote options (always provided)
 * @loc: local options (only if this is a confirmation)
 * @op: output buffer pointer
 * @olen: output buffer len
 *
 * These are options with end-to-end significance plus any options without end-to-end significance
 * that were requested for negotiation in the connection request.  For a connection indication, this
 * is only options with end-to-end significance.  For this to work with connection indications, all
 * options request flags must be set to zero.  The return value is the resulting size of the options
 * buffer, or a negative error number on software fault.
 *
 * The t_connect() or t_rcvconnect() functions return the values of all options with end-to-end
 * significance that were received with the connection response and the negotiated values of those
 * options without end-to-end significance that had been specified on input.  However, options
 * specified on input with t_connect() call that are not supported or refer to an unknown option
 * level are discarded and not returned on output.
 *
 * The status field of each option returned with t_connect() or t_rcvconnect() indicates if the
 * proposed value (T_SUCCESS) or a degraded value (T_PARTSUCCESS) has been negotiated.  The status
 * field of received ancillary information (for example, T_IP options) that is not subject to
 * negotiation is always set to T_SUCCESS.
 *
 * If this is a connection indication, @rem is the options passed by the remote end.
 */
static int
t_build_conn_opts(struct tp *tp, struct tp_options *rem, struct tp_options *rsp, unsigned char *op,
		  size_t olen, int indication)
{
	struct t_opthdr *oh;
	unsigned long flags[4], toggles[4];

	if (op == NULL || olen = 0)
		return (0);
	if (!tp)
		goto eproto;
	{
		int i;

		for (i = 0; i < 4; i++) {
			flags[i] = rem->flags[i] | rsp->flags[i];
			toggles[i] = rem->flags[i] ^ rsp->flags[i];
		}
	}
	oh = _T_OPT_FIRSTHDR_OFS(op, olen, 0);
	if (t_tst_bit(_T_BIT_XTI_DEBUG, flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->xti.xti_debug);
		oh->level = XTI_GENERIC;
		oh->name = XTI_DEBUG;
		oh->status = T_SUCCESS;
		/* No end-to-end significance */
		/* FIXME: range check parameter */
		bcopy(rem->xti.xti_debug, T_OPT_DATA(oh), sizeof(rem->xti.xti_debug));
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_XTI_LINGER, flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->xti.xti_linger);
		oh->level = XTI_GENERIC;
		oh->name = XTI_LINGER;
		oh->status = T_SUCCESS;
		/* No end-to-end significance */
		/* FIXME: range check parameter */
		*((struct t_linger *) T_OPT_DATA(oh)) = rem->xti.xti_linger;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_XTI_RCVBUF, flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->xti.xti_rcvbuf);
		oh->level = XTI_GENERIC;
		oh->name = XTI_RCVBUF;
		oh->status = T_SUCCESS;
		/* No end-to-end significance */
		/* FIXME: range check parameter */
		*((t_uscalar_t *) T_OPT_DATA(oh)) = rem->xti.xti_rcvbuf;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_XTI_RCVLOWAT, flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->xti.xti_rcvlowat);
		oh->level = XTI_GENERIC;
		oh->name = XTI_RCVLOWAT;
		oh->status = T_SUCCESS;
		/* No end-to-end significance */
		/* FIXME: range check parameter */
		*((t_uscalar_t *) T_OPT_DATA(oh)) = rem->xti.xti_rcvlowat;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_XTI_SNDBUF, flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->xti.xti_sndbuf);
		oh->level = XTI_GENERIC;
		oh->name = XTI_SNDBUF;
		oh->status = T_SUCCESS;
		/* No end-to-end significance */
		/* FIXME: range check parameter */
		*((t_uscalar_t *) T_OPT_DATA(oh)) = rem->xti.xti_sndbuf;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_XTI_SNDLOWAT, flags)) {
		if (oh == NULL)
			goto efault;
		oh->len = _T_LENGTH_SIZEOF(rem->xti.xti_sndlowat);
		oh->level = XTI_GENERIC;
		oh->name = XTI_SNDLOWAT;
		oh->status = T_SUCCESS;
		/* No end-to-end significance */
		/* FIXME: range check parameter */
		*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.xti.xti_sndlowat;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	switch (tp->service) {
	case T_COTS:
		if (t_tst_bit(_T_BIT_TCO_THROUGHPUT, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.throughput);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_THROUGHPUT;
			oh->status = T_SUCCESS;
			/* End-to-end significance. */
			/* Check requested but not responded, responded without request */
			if (t_bit_tst(_T_BIT_TCO_THROUGHPUT, toggled))
				oh->status = T_FAILURE;
			/* Check if we got downgraded. */
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.throughput)) T_OPT_DATA(oh)) = rem->tco.throughput;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_TRANSDEL, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.transdel);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_TRANSDEL;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCO_TRANSDEL, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.transdel)) T_OPT_DATA(oh)) = rem->tco.transdel;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_RESERRORRATE, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.reserrorrate);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_RESERRORRATE;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCO_RESERRORRATE, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.reserrorrate)) T_OPT_DATA(oh)) = rem->tco.reserrorrate;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_TRANSFFAILPROB, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.transffailprob);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_TRANSFFAILPROB;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCO_TRANSFFAILPROB, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.tco_trasffailprob)) T_OPT_DATA(oh))
			    = rem->tco.tco_trasffailprob;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_ESTFAILPROB, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.estfailprob);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_ESTFAILPROB;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCO_ESTFAILPROB, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.estfailprob)) T_OPT_DATA(oh)) = rem->tco.estfailprob;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_RELFAILPROB, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.relfailprob);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_RELFAILPROB;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCO_RELFAILPROB, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.relfailprob)) T_OPT_DATA(oh)) = rem->tco.relfailprob;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_ESTDELAY, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.estdelay);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_ESTDELAY;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCO_ESTDELAY, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.estdelay)) T_OPT_DATA(oh)) = rem->tco.estdelay;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_RELDELAY, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.reldelay);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_RELDELAY;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCO_RELDELAY, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.reldelay)) T_OPT_DATA(oh)) = rem->tco.reldelay;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_CONNRESIL, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.connresil);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_CONNRESIL;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCO_CONNRESIL, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.connresil)) T_OPT_DATA(oh)) = rem->tco.connresil;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_PROTECTION, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.protection);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_PROTECTION;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCO_PROTECTION, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.protection)) T_OPT_DATA(oh)) = rem->tco.protection;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_PRIORITY, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.priority);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_PRIORITY;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCO_PRIORITY, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.priority)) T_OPT_DATA(oh)) = rem->tco.priority;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_EXPD, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.expd);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_EXPD;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCO_EXPD, toggled))
				oh->status = T_FAILURE;
			if (rem->tco.expd != T_UNSPEC) {
			}
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.expd)) T_OPT_DATA(oh)) = rem->tco.expd;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_LTPDU, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.ltpdu);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_LTPDU;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCO_LTPDU, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.ltpdu)) T_OPT_DATA(oh)) = rem->tco.ltpdu;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_ACKTIME, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.acktime);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_ACKTIME;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCO_ACKTIME, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.acktime)) T_OPT_DATA(oh)) = rem->tco.acktime;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_REASTIME, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.reastime);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_REASTIME;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCO_REASTIME, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.reastime)) T_OPT_DATA(oh)) = rem->tco.reastime;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_EXTFORM, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.extform);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_EXTFORM;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCO_EXTFORM, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.extform)) T_OPT_DATA(oh)) = rem->tco.extform;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_FLOWCTRL, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.flowctrl);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_FLOWCTRL;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCO_FLOWCTRL, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.flowctrl)) T_OPT_DATA(oh)) = rem->tco.flowctrl;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_CHECKSUM, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.checksum);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_CHECKSUM;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCO_CHECKSUM, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.checksum)) T_OPT_DATA(oh)) = rem->tco.checksum;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_NETEXP, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.netexp);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_NETEXP;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCO_NETEXP, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.netexp)) T_OPT_DATA(oh)) = rem->tco.netexp;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_NETRECPTCF, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.netrecptcf);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_NETRECPTCF;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCO_NETRECPTCF, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.netrecptcf)) T_OPT_DATA(oh)) = rem->tco.netrecptcf;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_PREFCLASS, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.prefclass);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_PREFCLASS;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCO_PREFCLASS, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.prefclass)) T_OPT_DATA(oh)) = rem->tco.prefclass;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_ALTCLASS1, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.altclass1);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_ALTCLASS1;
			oh->status = T_SUCCESS;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.altclass1)) T_OPT_DATA(oh)) = rem->tco.altclass1;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_ALTCLASS2, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.altclass2);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_ALTCLASS2;
			oh->status = T_SUCCESS;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.altclass2)) T_OPT_DATA(oh)) = rem->tco.altclass2;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_ALTCLASS3, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.altclass3);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_ALTCLASS3;
			oh->status = T_SUCCESS;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.altclass3)) T_OPT_DATA(oh)) = rem->tco.altclass3;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCO_ALTCLASS4, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tco.altclass4);
			oh->level = T_ISO_TP;
			oh->name = T_TCO_ALTCLASS4;
			oh->status = T_SUCCESS;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tco.altclass4)) T_OPT_DATA(oh)) = rem->tco.altclass4;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		break;
	case T_CLTS:
		if (t_tst_bit(_T_BIT_TCL_TRANSDEL, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tcl.transdel);
			oh->level = T_ISO_TP;
			oh->name = T_TCL_TRANSDEL;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCL_TRANSDEL, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tcl.transdel)) T_OPT_DATA(oh)) = rem->tcl.transdel;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCL_RESERRORRATE, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tcl.reserrorrate);
			oh->level = T_ISO_TP;
			oh->name = T_TCL_RESERRORRATE;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCL_RESERRORRATE, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tcl.reserrorrate)) T_OPT_DATA(oh)) = rem->tcl.reserrorrate;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCL_PROTECTION, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tcl.protection);
			oh->level = T_ISO_TP;
			oh->name = T_TCL_PROTECTION;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCL_PROTECTION, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tcl.protection)) T_OPT_DATA(oh)) = rem->tcl.protection;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCL_PRIORITY, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tcl.priority);
			oh->level = T_ISO_TP;
			oh->name = T_TCL_PRIORITY;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCL_PRIORITY, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tcl.priority)) T_OPT_DATA(oh)) = rem->tcl.priority;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		if (t_tst_bit(_T_BIT_TCL_CHECKSUM, flags)) {
			if (oh == NULL)
				goto efault;
			oh->len = _T_LENGTH_SIZEOF(rem->tcl.checksum);
			oh->level = T_ISO_TP;
			oh->name = T_TCL_CHECKSUM;
			oh->status = T_SUCCESS;
			if (t_bit_tst(_T_BIT_TCL_CHECKSUM, toggled))
				oh->status = T_FAILURE;
			/* FIXME: check validity of requested option */
			*((typeof(&rem->tcl.checksum)) T_OPT_DATA(oh)) = rem->tcl.checksum;
			oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
		}
		break;
	}
	/* expect oh to be NULL (filled buffer) */
	if (oh != NULL)
		mi_strlog(tp->oq, 0, SL_ERROR, "%s: SWERR: failure in option processing",
			  __FUNCTION__);
	return (olen);
	// return ((unsigned char *) oh - op); /* return actual length */
      eproto:
	mi_strlog(tp->oq, 0, SL_ERROR, "%s: SWERR: EPROTO failure in options processing",
		  __FUNCTION__);
	return (-EPROTO);
      efault:
	mi_strlog(tp->oq, 0, SL_ERROR, "%s: SWERR: EFAULT failure in options processing",
		  __FUNCTION__);
	return (-EFAULT);
}

/**
 * t_parse_conn_opts: - parse connection request or response options
 * @tp: transport endpoint
 * @op: options structure
 * @iptr: input options buffer pointer
 * @ilen: input options buffer length
 *
 * Only legal options can be negotiated; illegal options cause failure.  An option is illegal if the
 * following applies: 1) the length specified in the t_opthdr.len exceeds the remaining size of the
 * option buffer (counted from the beginning of the option); 2) the option valid is illegal: the
 * legal values are defined for each option.  If an illegal option is passed to XTI, the following
 * will happen: ... If an illegal option is passed to t_accept() or t_connect() then either the
 * function fails with t_errno set to [TBADOPT] or the connection establishment fails at a later
 * stage, depending on when the implementation detects the illegal option. ...
 *
 * If the transport user passes multiple options in one call and one of them is illegal, the call
 * failes as described above.  It is, however, possible that some or even all of the submitted legal
 * options were successfully negotiated.  The transport user can check the current status by a call
 * to t_optmgmt() with the T_CURRENT flag set.
 *
 * Specifying an option level unknown to the transport provider does not fail in calls to
 * t_connect(), t_accept() or t_sndudata(); the optoin is discarded in these cases.  The function
 * t_optmgmt() failes with [TBADOPT].
 *
 * Specifying an option name that is unknown to or not supported by the protocol selected by the
 * option level does not cause failure.  The option is discarded in calls to t_connect(), t_accept()
 * or t_sndudata().  The function t_optmgmt() returns T_NOTSUPPORT in the status field of the
 * option.
 *
 * If a transport user requests negotiation of a read-only option, or a non-privileged user requests
 * illegal access to a privileged option, the following outcomes are possible: ... 2) if negotiation
 * of a read-only option is required, t_accept() or t_connect() either fail with [TACCES], or the
 * connection establishment aborts and a T_DISCONNECT event occurs.  If the connection aborts, a
 * synchronous call to t_connect() fails with [TLOOK].  It depends on timing and implementation
 * conditions whether a t_accept() call still succeeds or fails with [TLOOK].  If a privileged
 * optoin is illegally requested, the option is quietly ignored.  (A non-privileged user shall not
 * be able to select an option which is privileged or unsupported.)
 *
 * If multiple options are submitted to t_connect(), t_accept() or t_sndudata() and a read-only
 * option is rejected, the connection or the datagram transmission failes as described.  Options
 * that could be successfully negotiated before the erroneous option was processed retain their
 * negotiated values.  There is no roll-back mechanism.
 */
static int
t_parse_conn_opts(struct tp *tp, struct tp_options *op, const char *iptr, size_t ilen)
{
	const struct t_opthdr *ih;
	ulong *flags = &op->flags;

	if (iptr == NULL || ilen = 0)
		return (0);
	if (!tp)
		goto eproto;

	/* For each option regocnized, we test the requested valud for legallity and validitiy, and
	   then set the requested value in the stream's option buffer and mark the option requested
	   in the options flags. */
	for (ih = _T_OPT_FIRSTHDR_OFS(iptr, ilen, 0); ih;
	     ih = _T_OPT_NEXTHDR_OFS(iptr, ilen, ih, 0)) {
		if (ih->len < sizeof(*ih))
			goto einval;
		switch (ih->level) {
		case XTI_GENERIC:
		{
			struct tp_xti *xti = &op->xti;

			switch (ih->name) {
			case XTI_DEBUG:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (((ih->len - sizeof(*ih)) % sizeof(t_uscalar_t)) != 0)
					goto einval;
				if (ih->len >= sizeof(*ih) + 4 * sizeof(t_uscalar_t))
					xti->xti_debug[3] = valp[3];
				else
					xti->xti_debug[3] = 0;
				if (ih->len >= sizeof(*ih) + 3 * sizeof(t_uscalar_t))
					xti->xti_debug[2] = valp[2];
				else
					xti->xti_debug[2] = 0;
				if (ih->len >= sizeof(*ih) + 2 * sizeof(t_uscalar_t))
					xti->xti_debug[1] = valp[1];
				else
					xti->xti_debug[1] = 0;
				if (ih->len >= sizeof(*ih) + 1 * sizeof(t_uscalar_t))
					xti->xti_debug[0] = valp[0];
				else
					xti->xti_debug[0] = 0;
				t_set_bit(_T_BIT_XTI_DEBUG, flags);
				continue;
			}
			case XTI_LINGER:
			{
				struct t_linger *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len != sizeof(*ih) + sizeof(*valp))
					goto einval;
				if (valp->l_onoff != T_NO && valp->l_onoff != T_YES)
					goto einval;
				if (valp->l_linger != T_INFINITE && valp->l_linger != T_UNSPEC
				    && valp->l_linger < 0)
					goto einval;
				if (valp->l_onoff == T_NO)
					valp->l_linger = T_UNSPEC;
				else {
					if (valp->l_linger == T_UNSPEC)
						valp->l_linger = t_defaults.xti.xti_linger.l_linger;
					if (valp->l_linger == T_INFINITE)
						valp->l_linger = MAX_SCHEDULE_TIMEOUT / HZ;
					if (valp->l_linger >= MAX_SCHEDULE_TIMEOUT / HZ)
						valp->l_linger = MAX_SCHEDULE_TIMEOUT / HZ;
				}
				xti->xti_linger = *valp;
				t_set_bit(_T_BIT_XTI_LINGER, flags);
				continue;
			}
			case XTI_RCVBUF:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len != sizeof(*ih) + sizeof(*valp))
					goto einval;
				if (*valp > sysctl_rmem_max)
					*valp = sysctl_rmem_max;
				if (*valp < SOCK_MIN_RCVBUF / 2)
					*valp = SOCK_MIN_RCVBUF / 2;
				xti->xti_rcvbuf = *valp;
				t_set_bit(_T_BIT_XTI_RCVBUF, flags);
				continue;
			}
			case XTI_RCVLOWAT:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len != sizeof(*ih) + sizeof(*valp))
					goto einval;
				if (*valp < 1)
					*valp = 1;
				if (*valp > INT_MAX)
					*valp = INT_MAX;
				xti->xti_rcvlowat = *valp;
				t_set_bit(_T_BIT_XTI_RCVLOWAT, flags);
				continue;
			}
			case XTI_SNDBUF:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len != sizeof(*ih) + sizeof(*valp))
					goto einval;
				if (*valp > sysctl_wmem_max)
					*valp = sysctl_wmem_max;
				if (*valp < SOCK_MIN_SNDBUF / 2)
					*valp = SOCK_MIN_SNDBUF / 2;
				xti->xti_sndbuf = *valp;
				t_set_bit(_T_BIT_XTI_SNDBUF, flags);
				continue;
			}
			case XTI_SNDLOWAT:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len != sizeof(*ih) + sizeof(*valp))
					goto einval;
				if (*valp < 1)
					*valp = 1;
				if (*valp > 1)
					*valp = 1;
				xti->xti_sndlowat = *valp;
				t_set_bit(_T_BIT_XTI_SNDLOWAT, flags);
				continue;
			}
			}
			continue;
		}
		case T_ISO_TP:
			if (tp->info.SERV_type == T_CLTS) {
				struct tp_tcl *tcl = &op->tcl;

				switch (ih->name) {
				case T_TCL_TRANSDEL:
				{
					struct rate *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len != sizeof(*ih) + sizeof(*valp))
						goto einval;
					/* zero is not a valid value for delay */
					if (valp->targetvalue == 0)
						goto einval;
					if (valp->minacceptvalue == 0)
						goto einval;
					tcl->transdel = *valp;
					t_set_bit(_T_BIT_TCL_TRANSDEL, flags);
					continue;
				}
				case T_TCL_RESERRORRATE:
				{
					struct rate *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len != sizeof(*ih) + sizeof(*valp))
						goto einval;
					/* zero is not a valid value for residual error rate target
					   value (but it is for minimum acceptable value) */
					if (valp->targetvalue == 0)
						goto einval;
					tcl->reserrorrate = *valp;
					t_set_bit(_T_BIT_TCL_RESERRORRATE, flags);
					continue;
				}
				case T_TCL_PROTECTION:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len != sizeof(*ih) + sizeof(*valp))
						goto einval;
					switch (*valp) {
					case T_ABSREQ | T_NOPROTECT:
					case T_ABSREQ | T_ACTIVEPROTECT:
					case T_ABSREQ | T_PASSIVEPROTECT:
					case T_ABSREQ | T_ACTIVEPROTECT | T_PASSIVEPROTECT:
					case T_NOPROTECT:
					case T_ACTIVEPROTECT:
					case T_PASSIVEPROTECT:
					case T_ACTIVEPROTECT | T_PASSIVEPROTECT:
					case T_UNSPEC:
						break;
					default:
						goto einval;
					}
					tcl->protection = *valp;
					t_set_bit(_T_BIT_TCL_PROTECTION, flags);
					continue;
				}
				case T_TCL_PRIORITY:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len != sizeof(*ih) + sizeof(*valp))
						goto einval;
					switch (*valp) {
					case T_PRIDFLT:
					case T_PRILOW:
					case T_PRIMID:
					case T_PRIHIGH:
					case T_PRITOP:
					case T_UNSPEC:
						break;
					default:
						goto einval;
					}
					tcl->priority = *valp;
					t_set_bit(_T_BIT_TCL_PRIORITY, flags);
					continue;
				}
				case T_TCL_CHECKSUM:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len != sizeof(*ih) + sizeof(*valp))
						goto einval;
					switch (*valp) {
					case T_NO:
					case T_YES:
					case T_UNSPEC:
						break;
					default:
						goto einval;
					}
					tcl->checksum = *valp;
					t_set_bit(_T_BIT_TCL_CHECKSUM, flags);
					continue;
				}
				}
			} else {
				struct tp_tco *tco = &op->tco;

				switch (ih->name) {
				case T_TCO_THROUGHPUT:
				{
					struct thrpt *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len != sizeof(*ih) + sizeof(*valp))
						goto einval;
					/* target values must be non-zero */
					if (valp->maxthrpt.targetvalue == 0)
						goto einval;
					if (valp->avgthrpt.targetvalue == 0)
						goto einval;
					tco->throughput = *valp;
					t_set_bit(_T_BIT_TCO_THROUGHPUT, flags);
					continue;
				}
				case T_TCO_TRANSDEL:
				{
					struct transdel *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len != sizeof(*ih) + sizeof(*valp))
						goto einval;
					if (valp->maxdel.minacceptvalue == 0)
						goto einval;
					if (valp->avgdel.minacceptvalue == 0)
						goto einval;
					tco->transdel = *valp;
					t_set_bit(_T_BIT_TCO_TRANSDEL, flags);
					continue;
				}
				case T_TCO_RESERRORRATE:
				{
					struct rate *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len != sizeof(*ih) + sizeof(*valp))
						goto einval;
					if (valp->targetvalue == 0)
						goto einval;
					tco->reserrorrate = *valp;
					t_set_bit(_T_BIT_TCO_RESERRORRATE, flags);
					continue;
				}
				case T_TCO_TRANSFFAILPROB:
				{
					struct rate *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len != sizeof(*ih) + sizeof(*valp))
						goto einval;
					if (valp->targetvalue == 0)
						goto einval;
					tco->transffailprob = *valp;
					t_set_bit(_T_BIT_TCO_TRANSFFAILPROB, flags);
					continue;
				}
				case T_TCO_ESTFAILPROB:
				{
					struct rate *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len != sizeof(*ih) + sizeof(*valp))
						goto einval;
					if (valp->targetvalue == 0)
						goto einval;
					tco->estfailprob = *valp;
					t_set_bit(_T_BIT_TCO_ESTFAILPROB, flags);
					continue;
				}
				case T_TCO_RELFAILPROB:
				{
					struct rate *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len != sizeof(*ih) + sizeof(*valp))
						goto einval;
					if (valp->targetvalue == 0)
						goto einval;
					tco->relfailprob = *valp;
					t_set_bit(_T_BIT_TCO_RELFAILPROB, flags);
					continue;
				}
				case T_TCO_ESTDELAY:
				{
					struct rate *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len != sizeof(*ih) + sizeof(*valp))
						goto einval;
					if (valp->minacceptvalue == 0)
						goto einval;
					tco->estdelay = *valp;
					t_set_bit(_T_BIT_TCO_ESTDELAY, flags);
					continue;
				}
				case T_TCO_RELDELAY:
				{
					struct rate *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len != sizeof(*ih) + sizeof(*valp))
						goto einval;
					if (valp->minacceptvalue == 0)
						goto einval;
					tco->reldelay = *valp;
					t_set_bit(_T_BIT_TCO_RELDELAY, flags);
					continue;
				}
				case T_TCO_CONNRESIL:
				{
					struct rate *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len != sizeof(*ih) + sizeof(*valp))
						goto einval;
					if (valp->targetvalue == 0)
						goto einval;
					tco->connresil = *valp;
					t_set_bit(_T_BIT_TCO_CONNRESIL, flags);
					continue;
				}
				case T_TCO_PROTECTION:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len != sizeof(*ih) + sizeof(*valp))
						goto einval;
					switch (*valp) {
					case T_ABSREQ|T_NOPROTECT:
					case T_ABSREQ|T_ACTIVEPROTECT:
					case T_ABSREQ|T_PASSIVEPROTECT:
					case T_ABSREQ|T_ACTIVEPROTECT|T_PASSIVEPROTECT:
					case T_NOPROTECT:
					case T_ACTIVEPROTECT:
					case T_PASSIVEPROTECT:
					case T_ACTIVEPROTECT|T_PASSIVEPROTECT:
					case T_UNSPEC:
						break;
					default:
						goto einval;
					}
					tco->protection = *valp;
					t_set_bit(_T_BIT_TCO_PROTECTION, flags);
					continue;
				}
				case T_TCO_PRIORITY:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len != sizeof(*ih) + sizeof(*valp))
						goto einval;
					switch (*valp) {
					case T_PRIDFLT:
					case T_PRILOW:
					case T_PRIMID:
					case T_PRIHIGH:
					case T_PRITOP:
					case T_UNSPEC:
						break;
					default:
						goto einval;
					}
					tco->priority = *valp;
					t_set_bit(_T_BIT_TCO_PRIORITY, flags);
					continue;
				}
				case T_TCO_EXPD:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len != sizeof(*ih) + sizeof(*valp))
						goto einval;
					switch (*valp) {
					case T_NO:
					case T_YES:
					case T_UNSPEC:
						break;
					default:
						goto einval;
					}
					tco->expd = *valp;
					t_set_bit(_T_BIT_TCO_EXPD, flags);
					continue;
				}
				case T_TCO_LTPDU:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len != sizeof(*ih) + sizeof(*valp))
						goto einval;
					switch (*valp) {
					case (1<<_T_TPDU_SIZE_8192):
					case (1<<_T_TPDU_SIZE_4096):
					case (1<<_T_TPDU_SIZE_2048):
					case (1<<_T_TPDU_SIZE_1024):
					case (1<<_T_TPDU_SIZE_512):
					case (1<<_T_TPDU_SIZE_256):
					case (1<<_T_TPDU_SIZE_128):
						break;
					default:
						goto einval;
					}
					tco->ltpdu = *valp;
					t_set_bit(_T_BIT_TCO_LTPDU, flags);
					continue;
				}
				case T_TCO_ACKTIME:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len != sizeof(*ih) + sizeof(*valp))
						goto einval;
					if (*valp == 0)
						goto einval;
					tco->acktime = *valp;
					t_set_bit(_T_BIT_TCO_ACKTIME, flags);
					continue;
				}
				case T_TCO_REASTIME:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len != sizeof(*ih) + sizeof(*valp))
						goto einval;
					if (*valp == 0)
						goto einval;
					tco->reastime = *valp;
					t_set_bit(_T_BIT_TCO_REASTIME, flags);
					continue;
				}
				case T_TCO_EXTFORM:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len != sizeof(*ih) + sizeof(*valp))
						goto einval;
					switch (*valp) {
					case T_NO:
					case T_YES:
					case T_UNSPEC:
						break;
					default:
						goto einval;
					}
					tco->extform = *valp;
					t_set_bit(_T_BIT_TCO_EXTFORM, flags);
					continue;
				}
				case T_TCO_FLOWCTRL:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len != sizeof(*ih) + sizeof(*valp))
						goto einval;
					switch (*valp) {
					case T_NO:
					case T_YES:
					case T_UNSPEC:
						break;
					default:
						goto einval;
					}
					tco->flowctrl = *valp;
					t_set_bit(_T_BIT_TCO_FLOWCTRL, flags);
					continue;
				}
				case T_TCO_CHECKSUM:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len != sizeof(*ih) + sizeof(*valp))
						goto einval;
					switch (*valp) {
					case T_NO:
					case T_YES:
					case T_UNSPEC:
						break;
					default:
						goto einval;
					}
					tco->checksum = *valp;
					t_set_bit(_T_BIT_TCO_CHECKSUM, flags);
					continue;
				}
				case T_TCO_NETEXP:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len != sizeof(*ih) + sizeof(*valp))
						goto einval;
					switch (*valp) {
					case T_NO:
					case T_YES:
					case T_UNSPEC:
						break;
					default:
						goto einval;
					}
					tco->netexp = *valp;
					t_set_bit(_T_BIT_TCO_NETEXP, flags);
					continue;
				}
				case T_TCO_NETRECPTCF:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len != sizeof(*ih) + sizeof(*valp))
						goto einval;
					switch (*valp) {
					case T_NO:
					case T_YES:
					case T_UNSPEC:
						break;
					default:
						goto einval;
					}
					tco->netrecptcf = *valp;
					t_set_bit(_T_BIT_TCO_NETRECPTCF, flags);
					continue;
				}
				case T_TCO_PREFCLASS:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len != sizeof(*ih) + sizeof(*valp))
						goto einval;
					switch (*valp) {
					case T_CLASS0:
					case T_CLASS1:
					case T_CLASS2:
					case T_CLASS3:
					case T_CLASS4:
					case T_UNSPEC:
						break;
					default:
						goto einval;
					}
					tco->prefclass = *valp;
					t_set_bit(_T_BIT_TCO_PREFCLASS, flags);
					continue;
				}
				case T_TCO_ALTCLASS1:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len != sizeof(*ih) + sizeof(*valp))
						goto einval;
					switch (*valp) {
					case T_CLASS0:
					case T_CLASS1:
					case T_CLASS2:
					case T_CLASS3:
					case T_CLASS4:
					case T_UNSPEC:
						break;
					default:
						goto einval;
					}
					tco->altclass1 = *valp;
					t_set_bit(_T_BIT_TCO_ALTCLASS1, flags);
					continue;
				}
				case T_TCO_ALTCLASS2:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len != sizeof(*ih) + sizeof(*valp))
						goto einval;
					switch (*valp) {
					case T_CLASS0:
					case T_CLASS1:
					case T_CLASS2:
					case T_CLASS3:
					case T_CLASS4:
					case T_UNSPEC:
						break;
					default:
						goto einval;
					}
					tco->altclass2 = *valp;
					t_set_bit(_T_BIT_TCO_ALTCLASS2, flags);
					continue;
				}
				case T_TCO_ALTCLASS3:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len != sizeof(*ih) + sizeof(*valp))
						goto einval;
					switch (*valp) {
					case T_CLASS0:
					case T_CLASS1:
					case T_CLASS2:
					case T_CLASS3:
					case T_CLASS4:
					case T_UNSPEC:
						break;
					default:
						goto einval;
					}
					tco->altclass3 = *valp;
					t_set_bit(_T_BIT_TCO_ALTCLASS3, flags);
					continue;
				}
				case T_TCO_ALTCLASS4:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

					if (ih->len != sizeof(*ih) + sizeof(*valp))
						goto einval;
					switch (*valp) {
					case T_CLASS0:
					case T_CLASS1:
					case T_CLASS2:
					case T_CLASS3:
					case T_CLASS4:
					case T_UNSPEC:
						break;
					default:
						goto einval;
					}
					tco->altclass4 = *valp;
					t_set_bit(_T_BIT_TCO_ALTCLASS4, flags);
					continue;
				}
				}
			}
			continue;
		case T_INET_IP:
		{
			struct tp_ip *ip = &op->ip;

			switch (ih->name) {
			case T_IP_OPTIONS:
			{
				continue;
			}
			case T_IP_TOS:
			{
				unsigned char *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len != sizeof(*valp) + sizeof(*ih))
					goto einval;
				ip->tos = *valp;
				t_set_bit(_T_BIT_IP_TOS, flags);
				continue;
			}
			case T_IP_TTL:
			{
				unsigned char *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len != sizeof(*valp) + sizeof(*ih))
					goto einval;
				ip->ttl = *valp;
				t_set_bit(_T_BIT_IP_TTL, flags);
				continue;
			}
			case T_IP_REUSEADDR:
			{
				unsigned int *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len != sizeof(*valp) + sizeof(*ih))
					goto einval;
				ip->reuseaddr = *valp;
				t_set_bit(_T_BIT_IP_REUSEADDR, flags);
				continue;
			}
			case T_IP_DONTROUTE:
			{
				unsigned int *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len != sizeof(*valp) + sizeof(*ih))
					goto einval;
				ip->dontroute = *valp;
				t_set_bit(_T_BIT_IP_DONTROUTE, flags);
				continue;
			}
			case T_IP_BROADCAST:
			{
				unsigned int *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len != sizeof(*valp) + sizeof(*ih))
					goto einval;
				ip->broadcast = *valp;
				t_set_bit(_T_BIT_IP_BROADCAST, flags);
				continue;
			}
			case T_IP_ADDR:
			{
				uint32_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len != sizeof(*valp) + sizeof(*ih))
					goto einval;
				ip->addr = *valp;
				t_set_bit(_T_BIT_IP_ADDR, flags);
				continue;
			}
			case T_IP_RETOPTS:
			{
				unsigned int *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len != sizeof(*valp) + sizeof(*ih))
					goto einval;
				ip->retopts = *valp;
				t_set_bit(_T_BIT_IP_RETOPTS, flags);
				continue;
			}
			}
			continue;
		}
		case T_INET_UDP:
		{
			struct tp_udp *udp = &op->udp;

			switch (ih->name) {
			case T_UDP_CHECKSUM:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len != sizeof(*valp) + sizeof(*ih))
					goto einval;
				switch (*valp) {
				case T_NO:
				case T_YES:
				case T_UNSPEC:
					break;
				default:
					goto einval;
				}
				udp->checksum = *valp;
				t_set_bit(_T_BIT_UDP_CHECKSUM, flags);
				continue;
			}
			}
			continue;
		}
		case T_INET_TCP:
		{
			struct tp_tcp *tcp = &op->tcp;

			switch (ih->name) {
			case T_TCP_NODELAY:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len != sizeof(*valp) + sizeof(*ih))
					goto einval;
				tcp->nodelay = *valp;
				t_set_bit(_T_BIT_TCP_NODELAY, flags);
				continue;
			}
			case T_TCP_MAXSEG:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len != sizeof(*valp) + sizeof(*ih))
					goto einval;
				if (*valp < 8)
					*valp = 8;
				if (*valp > MAX_TCP_WINDOW)
					*valp = MAX_TCP_WINDOW;
				tcp->maxseg = *valp;
				t_set_bit(_T_BIT_TCP_MAXSEG, flags);
				continue;
			}
			case T_TCP_KEEPALIVE:
			{
				struct t_kpalive *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len != sizeof(*valp) + sizeof(*ih))
					goto einval;
				if (valp->kp_onoff != T_YES && valp->kp_onoff != T_NO)
					goto einval;
				if (valp->kp_timeout != T_INFINITE && valp->kp_timeout != T_UNSPEC
				    && valp->kp_timeout < 0)
					goto einval;
				if (valp->kp_onoff == T_NO)
					valp->kp_timeout = T_UNSPEC;
				else {
					if (valp->kp_timeout == T_UNSPEC)
						valp->kp_timeout =
						    t_defaults.tcp.keepalive.kp_timeout;
					if (valp->kp_timeout < 1)
						valp->kp_timeout = 1;
					if (valp->kp_timeout > MAX_SCHEDULE_TIMEOUT / 60 / HZ)
						valp->kp_timeout = MAX_SCHEDULE_TIMEOUT / 60 / HZ;
				}
				tcp->keepalive = *valp;
				t_set_bit(_T_BIT_TCP_KEEPALIVE, flags);
				continue;
			}
			case T_TCP_CORK:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len != sizeof(*valp) + sizeof(*ih))
					goto einval;
				if (*valp != T_YES && *valp != T_NO)
					goto einval;
				tcp->cork = *valp;
				t_set_bit(_T_BIT_TCP_CORK, flags);
				continue;
			}
			case T_TCP_KEEPIDLE:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len != sizeof(*valp) + sizeof(*ih))
					goto einval;
				if (*valp < 1)
					*valp = 1;
				if (*valp > MAX_TCP_KEEPIDLE)
					*valp = MAX_TCP_KEEPIDLE;
				tcp->keepidle = *valp;
				t_set_bit(_T_BIT_TCP_KEEPIDLE, flags);
				continue;
			}
			case T_TCP_KEEPINTVL:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len != sizeof(*valp) + sizeof(*ih))
					goto einval;
				if (*valp < 1)
					*valp = 1;
				if (*valp > MAX_TCP_KEEPINTVL)
					*valp = MAX_TCP_KEEPINTVL;
				tcp->keepitvl = *valp;
				t_set_bit(_T_BIT_TCP_KEEPINTVL, flags);
				continue;
			}
			case T_TCP_KEEPCNT:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len != sizeof(*valp) + sizeof(*ih))
					goto einval;
				if (*valp < 1)
					*valp = 1;
				if (*valp > MAX_TCP_KEEPCNT)
					*valp = MAX_TCP_KEEPCNT;
				tcp->keepcnt = *valp;
				t_set_bit(_T_BIT_TCP_KEEPCNT, flags);
				continue;
			}
			case T_TCP_SYNCNT:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len != sizeof(*valp) + sizeof(*ih))
					goto einval;
				if (*valp < 1)
					*valp = 1;
				if (*valp > MAX_TCP_SYNCNT)
					*valp = MAX_TCP_SYNCNT;
				tcp->syncnt = *valp;
				t_set_bit(_T_BIT_TCP_SYNCNT, flags);
				continue;
			}
			case T_TCP_LINGER2:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len != sizeof(*valp) + sizeof(*ih))
					goto einval;
				tcp->linger2 = *valp;
				t_set_bit(_T_BIT_TCP_LINGER2, flags);
				continue;
			}
			case T_TCP_DEFER_ACCEPT:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len != sizeof(*valp) + sizeof(*ih))
					goto einval;
				tcp->defer_accept = *valp;
				t_set_bit(_T_BIT_TCP_DEFER_ACCEPT, flags);
				continue;
			}
			case T_TCP_WINDOW_CLAMP:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(ih);

				if (ih->len != sizeof(*valp) + sizeof(*ih))
					goto einval;
				if (*valp < SOCK_MIN_RCVBUF / 2)
					*valp = SOCK_MIN_RCVBUF / 2;
				tcp->window_clamp = *valp;
				t_set_bit(_T_BIT_TCP_WINDOW_CLAMP, flags);
				continue;
			}
			case T_TCP_INFO:
			{
				/* read-only */
				continue;
			}
			case T_TCP_QUICKACK:
			{
				/* not valid in this state */
				continue;
			}
			}
			continue;
		}
		}
	}
	return (0);
      einval:
	return (-EINVAL);
      eacces:
	return (-EACCES);
      eproto:
	return (-EPROTO);
      efault:
	return (-EFAULT);

}

static int
tp_size_default_options(struct tp *tp, const uchar *ip, size_t ilen)
{
	int olen = 0, optlen;
	const struct t_opthdr *ih;
	struct t_opthdr all;

	if (ip == NULL || ilen == 0) {
		/* for zero-length options, fake an option header for all names with all levels */
		all.level = T_ALLLEVELS;
		all.name = T_ALLOPT;
		all.len = sizeof(all);
		all.status = 0;
		ip = (const uchar *) &all;
		ilen = sizeof(all);
	}
	for (ih = _T_OPT_FIRSTHDR_OFS(ip, ilen, 0); ih; ih = _T_OPT_NEXTHDR_OFS(ip, ilen, ih, 0)) {
		if (ih->len < sizeof(*ih))
			goto einval;
		if ((uchar *) ih + ih->len > ip + len)
			goto einval;
		optlen = ih->len - sizeof(*ih);
		switch (ih->level) {
		default:
			goto einval;
		case T_ALLLEVELS:
			if (ih->name != T_ALLOPT)
				goto einval;
		case XTI_GENERIC:
			switch (ih->name) {
			default:
				olen += T_SPACE(0);
				continue;
			case T_ALLOPT:
			case XTI_DEBUG:
				olen += _T_SPACE_SIZEOF(t_defaults.xti.debug);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_LINGER:
				olen += _T_SPACE_SIZEOF(t_defaults.xti.linger);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_RCVBUF:
				olen += _T_SPACE_SIZEOF(t_defaults.xti.rcvbuf);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_RCVLOWAT:
				olen += _T_SPACE_SIZEOF(t_defaults.xti.rcvlowat);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_SNDBUF:
				olen += _T_SPACE_SIZEOF(t_defaults.xti.sndbuf);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_SNDLOWAT:
				olen += _T_SPACE_SIZEOF(t_defaults.xti.sndlowat);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		case T_INET_IP:
			switch (ih->name) {
			default:
				olen += T_SPACE(0);
				continue;
			case T_ALLOPT:
			case T_IP_OPTIONS:
				/* not supported yet */
				olen += T_SPACE(0);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_TOS:
				olen += _T_SPACE_SIZEOF(t_defaults.ip.tos);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_TTL:
				olen += _T_SPACE_SIZEOF(t_defaults.ip.ttl);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_REUSEADDR:
				olen += _T_SPACE_SIZEOF(t_defaults.ip.reuseaddr);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_DONTROUTE:
				olen += _T_SPACE_SIZEOF(t_defaults.ip.dontroute);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_BROADCAST:
				olen += _T_SPACE_SIZEOF(t_defaults.ip.broadcast);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_ADDR:
				olen += _T_SPACE_SIZEOF(t_defaults.ip.addr);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		case T_INET_UDP:
			switch (ih->name) {
			default:
				olen += T_SPACE(0);
				continue;
			case T_ALLOPT:
			case T_UDP_CHECKSUM:
				olen += _T_SPACE_SIZEOF(t_defaults.udp.checksum);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		case T_INET_TCP:
			switch (ih->name) {
			default:
				olen += T_SPACE(0);
				continue;
			case T_ALLOPT:
			case T_TCP_NODELAY:
				olen += _T_SPACE_SIZEOF(t_defaults.tcp.nodelay);
				if (ih->name != T_ALLOPT)
					continue;
			case T_TCP_MAXSEG:
				olen += _T_SPACE_SIZEOF(t_defaults.tcp.maxseg);
				if (ih->name != T_ALLOPT)
					continue;
			case T_TCP_KEEPALIVE:
				olen += _T_SPACE_SIZEOF(t_defaults.tcp.keepalive);
				if (ih->name != T_ALLOPT)
					continue;
			case T_TCP_CORK:
				olen += _T_SPACE_SIZEOF(t_defaults.tcp.cork);
				if (ih->name != T_ALLOPT)
					continue;
			case T_TCP_KEEPIDLE:
				olen += _T_SPACE_SIZEOF(t_defaults.tcp.keepidle);
				if (ih->name != T_ALLOPT)
					continue;
			case T_TCP_KEEPINTVL:
				olen += _T_SPACE_SIZEOF(t_defaults.tcp.keepitvl);
				if (ih->name != T_ALLOPT)
					continue;
			case T_TCP_KEEPCNT:
				olen += _T_SPACE_SIZEOF(t_defaults.tcp.keepcnt);
				if (ih->name != T_ALLOPT)
					continue;
			case T_TCP_SYNCNT:
				olen += _T_SPACE_SIZEOF(t_defaults.tcp.syncnt);
				if (ih->name != T_ALLOPT)
					continue;
			case T_TCP_LINGER2:
				olen += _T_SPACE_SIZEOF(t_defaults.tcp.linger2);
				if (ih->name != T_ALLOPT)
					continue;
			case T_TCP_DEFER_ACCEPT:
				olen += _T_SPACE_SIZEOF(t_defaults.tcp.defer_accept);
				if (ih->name != T_ALLOPT)
					continue;
			case T_TCP_WINDOW_CLAMP:
				olen += _T_SPACE_SIZEOF(t_defaults.tcp.window_clamp);
				if (ih->name != T_ALLOPT)
					continue;
			case T_TCP_INFO:
				/* read only, can't get default */
				olen += T_SPACE(0);
				if (ih->name != T_ALLOPT)
					continue;
			case T_TCP_QUICKACK:
				olen += _T_SPACE_SIZEOF(t_defaults.tcp.quickack);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		case T_ISO_TP:
			if (tp->info.SERV_type != T_CLTS) {
				switch (ih->name) {
				default:
					olen += T_SPACE(0);
					continue;
				case T_ALLOPT:
				case T_TCO_THROUGHPUT:
					olen += _T_SPACE_SIZEOF(t_defaults.tco.throughput);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_TRANSDEL:
					olen += _T_SPACE_SIZEOF(t_defaults.tco.transdel);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_RESERRORRATE:
					olen += _T_SPACE_SIZEOF(t_defaults.tco.reserrorrate);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_TRANSFFAILPROB:
					olen += _T_SPACE_SIZEOF(t_defaults.tco.transffailprob);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_ESTFAILPROB:
					olen += _T_SPACE_SIZEOF(t_defaults.tco.estfailprob);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_RELFAILPROB:
					olen += _T_SPACE_SIZEOF(t_defaults.tco.relfailprob);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_ESTDELAY:
					olen += _T_SPACE_SIZEOF(t_defaults.tco.estdelay);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_RELDELAY:
					olen += _T_SPACE_SIZEOF(t_defaults.tco.reldelay);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_CONNRESIL:
					olen += _T_SPACE_SIZEOF(t_defaults.tco.connresil);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_PROTECTION:
					olen += _T_SPACE_SIZEOF(t_defaults.tco.protection);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_PRIORITY:
					olen += _T_SPACE_SIZEOF(t_defaults.tco.priority);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_EXPD:
					olen += _T_SPACE_SIZEOF(t_defaults.tco.expd);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_LTPDU:
					olen += _T_SPACE_SIZEOF(t_defaults.tco.ltpdu);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_ACKTIME:
					olen += _T_SPACE_SIZEOF(t_defaults.tco.acktime);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_REASTIME:
					olen += _T_SPACE_SIZEOF(t_defaults.tco.reastime);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_EXTFORM:
					olen += _T_SPACE_SIZEOF(t_defaults.tco.extform);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_FLOWCTRL:
					olen += _T_SPACE_SIZEOF(t_defaults.tco.flowctrl);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_CHECKSUM:
					olen += _T_SPACE_SIZEOF(t_defaults.tco.checksum);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_NETEXP:
					olen += _T_SPACE_SIZEOF(t_defaults.tco.netexp);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_NETRECPTCF:
					olen += _T_SPACE_SIZEOF(t_defaults.tco.netrecptcf);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_PREFCLASS:
					olen += _T_SPACE_SIZEOF(t_defaults.tco.prefclass);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_ALTCLASS1:
					olen += _T_SPACE_SIZEOF(t_defaults.tco.altclass1);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_ALTCLASS2:
					olen += _T_SPACE_SIZEOF(t_defaults.tco.altclass2);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_ALTCLASS3:
					olen += _T_SPACE_SIZEOF(t_defaults.tco.altclass3);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_ALTCLASS4:
					olen += _T_SPACE_SIZEOF(t_defaults.tco.altclass4);
					if (ih->name != T_ALLOPT)
						continue;
				}
				if (ih->level != T_ALLLEVELS)
					continue;
			} else {
				if (ih->level != T_ALLLEVELS)
					goto einval;
			}
			if (tp->info.SERV_type == T_CLTS) {
				switch (ih->name) {
				default:
					olen += T_SPACE(0);
					continue;
				case T_ALLOPT:
				case T_TCL_TRANSDEL:
					olen += _T_SPACE_SIZEOF(t_defaults.tcl.transdel);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCL_RESERRORRATE:
					olen += _T_SPACE_SIZEOF(t_defaults.tcl.reserrorrate);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCL_PROTECTION:
					olen += _T_SPACE_SIZEOF(t_defaults.tcl.protection);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCL_PRIORITY:
					olen += _T_SPACE_SIZEOF(t_defaults.tcl.priority);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCL_CHECKSUM:
					olen += _T_SPACE_SIZEOF(t_defaults.tcl.checksum);
					if (ih->name != T_ALLOPT)
						continue;
				}
				if (ih->level != T_ALLLEVELS)
					continue;
			} else {
				if (ih->level != T_ALLLEVELS)
					goto einval;
			}
		}
	}
	mi_strlog(tp->oq, 0, SL_TRACE, "%s: calculated option size = %u", __FUNCTION__, olen);
	return (olen);
      einval:
	mi_strlog(tp->oq, 0, SL_TRACE, "%s: invalid input options", __FUNCTION__);
	return (-EINVAL);
}

static int
tp_size_current_options(struct tp *tp, const uchar *ip, size_t ilen)
{
	int olen = 0, optlen;
	const struct t_opthdr *ih;
	struct t_opthdr all;
	struct tp_options *op = &tp->opts.res;

	if (ip == NULL || ilen == 0) {
		/* for zero-length options, fake an option header for all names with all levels */
		all.level = T_ALLLEVELS;
		all.name = T_ALLOPT;
		all.len = sizeof(all);
		all.status = 0;
		ip = (const uchar *) &all;
		ilen = sizeof(all);
	}
	for (ih = _T_OPT_FIRSTHDR_OFS(ip, ilen, 0); ih; ih = _T_OPT_NEXTHDR_OFS(ip, ilen, ih, 0)) {
		if (ih->len < sizeof(*ih))
			goto einval;
		if ((uchar *) ih + ih->len > ip + len)
			goto einval;
		optlen = ih->len - sizeof(*ih);
		switch (ih->level) {
		default:
			goto einval;
		case T_ALLLEVELS:
			if (ih->name != T_ALLOPT)
				goto einval;
		case XTI_GENERIC:
		{
			struct tp_xti *xti = &op->xti;

			switch (ih->name) {
			default:
				olen += T_SPACE(0);
				continue;
			case T_ALLOPT:
			case XTI_DEBUG:
				olen += _T_SPACE_SIZEOF(xti->debug);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_LINGER:
				olen += _T_SPACE_SIZEOF(xti->linger);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_RCVBUF:
				olen += _T_SPACE_SIZEOF(xti->rcvbuf);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_RCVLOWAT:
				olen += _T_SPACE_SIZEOF(xti->rcvlowat);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_SNDBUF:
				olen += _T_SPACE_SIZEOF(xti->sndbuf);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_SNDLOWAT:
				olen += _T_SPACE_SIZEOF(xti->sndlowat);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		}
		case T_INET_IP:
		{
			struct tp_ip *ip = &op->ip;

			switch (ih->name) {
			default:
				olen += T_SPACE(0);
				continue;
			case T_ALLOPT:
			case T_IP_OPTIONS:
				/* not supported yet */
				olen += T_SPACE(0);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_TOS:
				olen += _T_SPACE_SIZEOF(ip->tos);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_TTL:
				olen += _T_SPACE_SIZEOF(ip->ttl);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_REUSEADDR:
				olen += _T_SPACE_SIZEOF(ip->reuseaddr);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_DONTROUTE:
				olen += _T_SPACE_SIZEOF(ip->dontroute);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_BROADCAST:
				olen += _T_SPACE_SIZEOF(ip->broadcast);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_ADDR:
				olen += _T_SPACE_SIZEOF(ip->addr);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		}
		case T_INET_UDP:
		{
			struct tp_udp *udp = &op->udp;

			switch (ih->name) {
			default:
				olen += T_SPACE(0);
				continue;
			case T_ALLOPT:
			case T_UDP_CHECKSUM:
				olen += _T_SPACE_SIZEOF(udp->checksum);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		}
		case T_INET_TCP:
		{
			struct tp_tcp *tcp = &op->tcp;

			switch (ih->name) {
			default:
				olen += T_SPACE(0);
				continue;
			case T_ALLOPT:
			case T_TCP_NODELAY:
				olen += _T_SPACE_SIZEOF(tcp->nodelay);
				if (ih->name != T_ALLOPT)
					continue;
			case T_TCP_MAXSEG:
				olen += _T_SPACE_SIZEOF(tcp->maxseg);
				if (ih->name != T_ALLOPT)
					continue;
			case T_TCP_KEEPALIVE:
				olen += _T_SPACE_SIZEOF(tcp->keepalive);
				if (ih->name != T_ALLOPT)
					continue;
			case T_TCP_CORK:
				olen += _T_SPACE_SIZEOF(tcp->cork);
				if (ih->name != T_ALLOPT)
					continue;
			case T_TCP_KEEPIDLE:
				olen += _T_SPACE_SIZEOF(tcp->keepidle);
				if (ih->name != T_ALLOPT)
					continue;
			case T_TCP_KEEPINTVL:
				olen += _T_SPACE_SIZEOF(tcp->keepitvl);
				if (ih->name != T_ALLOPT)
					continue;
			case T_TCP_KEEPCNT:
				olen += _T_SPACE_SIZEOF(tcp->keepcnt);
				if (ih->name != T_ALLOPT)
					continue;
			case T_TCP_SYNCNT:
				olen += _T_SPACE_SIZEOF(tcp->syncnt);
				if (ih->name != T_ALLOPT)
					continue;
			case T_TCP_LINGER2:
				olen += _T_SPACE_SIZEOF(tcp->linger2);
				if (ih->name != T_ALLOPT)
					continue;
			case T_TCP_DEFER_ACCEPT:
				olen += _T_SPACE_SIZEOF(tcp->defer_accept);
				if (ih->name != T_ALLOPT)
					continue;
			case T_TCP_WINDOW_CLAMP:
				olen += _T_SPACE_SIZEOF(tcp->window_clamp);
				if (ih->name != T_ALLOPT)
					continue;
			case T_TCP_INFO:
				olen += _T_SPACE_SIZEOF(tcp->info);
				if (ih->name != T_ALLOPT)
					continue;
			case T_TCP_QUICKACK:
				olen += _T_SPACE_SIZEOF(tcp->quickack);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		}
		case T_ISO_TP:
			if (tp->info.SERV_type == T_CLTS) {
				struct tp_tcl *tcl = &op->tcl;

				switch (ih->name) {
				default:
					olen += T_SPACE(0);
					continue;
				case T_ALLOPT:
				case T_TCL_TRANSDEL:
					olen += _T_SPACE_SIZEOF(tcl->transdel);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCL_RESERRORRATE:
					olen += _T_SPACE_SIZEOF(tcl->reserrorrate);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCL_PROTECTION:
					olen += _T_SPACE_SIZEOF(tcl->protection);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCL_PRIORITY:
					olen += _T_SPACE_SIZEOF(tcl->priority);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCL_CHECKSUM:
					olen += _T_SPACE_SIZEOF(tcl->checksum);
					if (ih->name != T_ALLOPT)
						continue;
				}
			} else {
				struct tp_tco *tcp = &op->tco;

				switch (ih->name) {
				default:
					olen += T_SPACE(0);
					continue;
				case T_ALLOPT:
				case T_TCO_THROUGHPUT:
					olen += _T_SPACE_SIZEOF(tco->throughput);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_TRANSDEL:
					olen += _T_SPACE_SIZEOF(tco->transdel);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_RESERRORRATE:
					olen += _T_SPACE_SIZEOF(tco->reserrorrate);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_TRANSFFAILPROB:
					olen += _T_SPACE_SIZEOF(tco->transffailprob);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_ESTFAILPROB:
					olen += _T_SPACE_SIZEOF(tco->estfailprob);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_RELFAILPROB:
					olen += _T_SPACE_SIZEOF(tco->relfailprob);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_ESTDELAY:
					olen += _T_SPACE_SIZEOF(tco->estdelay);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_RELDELAY:
					olen += _T_SPACE_SIZEOF(tco->reldelay);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_CONNRESIL:
					olen += _T_SPACE_SIZEOF(tco->connresil);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_PROTECTION:
					olen += _T_SPACE_SIZEOF(tco->protection);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_PRIORITY:
					olen += _T_SPACE_SIZEOF(tco->priority);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_EXPD:
					olen += _T_SPACE_SIZEOF(tco->expd);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_LTPDU:
					olen += _T_SPACE_SIZEOF(tco->ltpdu);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_ACKTIME:
					olen += _T_SPACE_SIZEOF(tco->acktime);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_REASTIME:
					olen += _T_SPACE_SIZEOF(tco->reastime);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_EXTFORM:
					olen += _T_SPACE_SIZEOF(tco->extform);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_FLOWCTRL:
					olen += _T_SPACE_SIZEOF(tco->flowctrl);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_CHECKSUM:
					olen += _T_SPACE_SIZEOF(tco->checksum);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_NETEXP:
					olen += _T_SPACE_SIZEOF(tco->netexp);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_NETRECPTCF:
					olen += _T_SPACE_SIZEOF(tco->netrecptcf);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_PREFCLASS:
					olen += _T_SPACE_SIZEOF(tco->prefclass);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_ALTCLASS1:
					olen += _T_SPACE_SIZEOF(tco->altclass1);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_ALTCLASS2:
					olen += _T_SPACE_SIZEOF(tco->altclass2);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_ALTCLASS3:
					olen += _T_SPACE_SIZEOF(tco->altclass3);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_ALTCLASS4:
					olen += _T_SPACE_SIZEOF(tco->altclass4);
					if (ih->name != T_ALLOPT)
						continue;
				}
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		}
	}
	mi_strlog(tp->oq, 0, SL_TRACE, "%s: calculated option size = %u", __FUNCTION__, olen);
	return (olen);
      einval:
	mi_strlog(tp->oq, 0, SL_TRACE, "%s: invalid input options", __FUNCTION__);
	return (-EINVAL);
}

static int
tp_size_check_options(struct tp *tp, const uchar *ip, size_t ilen)
{
	int olen = 0, optlen;
	const struct t_opthdr *ih;
	struct t_opthdr all;
	struct tp_options *op = &tp->opts.res;

	if (ip == NULL || ilen == 0) {
		/* for zero-length options, fake an option header for all names with all levels */
		all.level = T_ALLLEVELS;
		all.name = T_ALLOPT;
		all.len = sizeof(all);
		all.status = 0;
		ip = (const uchar *) &all;
		ilen = sizeof(all);
	}
	for (ih = _T_OPT_FIRSTHDR_OFS(ip, ilen, 0); ih; ih = _T_OPT_NEXTHDR_OFS(ip, ilen, ih, 0)) {
		if (ih->len < sizeof(*ih))
			goto einval;
		if ((uchar *) ih + ih->len > ip + len)
			goto einval;
		optlen = ih->len - sizeof(*ih);
		switch (ih->level) {
		default:
			goto einval;
		case T_ALLLEVELS:
			if (ih->name != T_ALLOPT)
				goto einval;
		case XTI_GENERIC:
		{
			struct tp_xti *xti = &op->xti;

			switch (ih->name) {
			default:
				olen += T_SPACE(optlen);
				continue;
			case T_ALLOPT:
			case XTI_DEBUG:
				/* can be any non-zero array of t_uscalar_t */
				if (optlen
				    && ((optlen % sizeof(t_uscalar_t)) != 0
					|| optlen > 4 * sizeof(t_uscalar_t)))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_LINGER:
				if (optlen && optlen != sizeof(xti->linger))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_RCVBUF:
				if (optlen && optlen != sizeof(xti->rcvbuf))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_RCVLOWAT:
				if (optlen && optlen != sizeof(xti->rcvlowat))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_SNDBUF:
				if (optlen && optlen != sizeof(xti->sndbuf))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_SNDLOWAT:
				if (optlen && optlen != sizeof(xti->sndlowat))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		}
		case T_INET_IP:
		{
			struct tp_ip *ip = &op->ip;

			switch (ih->name) {
			default:
				olen += T_SPACE(optlen);
				continue;
			case T_ALLOPT:
			case T_IP_OPTIONS:
				if (optlen && optlen != sizeof(ip->options))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_TOS:
				if (optlen && optlen != sizeof(ip->tos))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_TTL:
				if (optlen && optlen != sizeof(ip->ttl))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_REUSEADDR:
				if (optlen && optlen != sizeof(ip->reuseaddr))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_DONTROUTE:
				if (optlen && optlen != sizeof(ip->dontroute))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_BROADCAST:
				if (optlen && optlen != sizeof(ip->broadcast))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_ADDR:
				if (optlen && optlen != sizeof(ip->addr))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		}
		case T_INET_UDP:
		{
			struct tp_udp *udp = &op->udp;

			switch (ih->name) {
			default:
				olen += T_SPACE(optlen);
				continue;
			case T_ALLOPT:
			case T_UDP_CHECKSUM:
				if (optlen && optlen != sizeof(udp->checksum))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		}
		case T_INET_TCP:
		{
			struct tp_tcp *tcp = &op->tcp;

			switch (ih->name) {
			default:
				olen += T_SPACE(optlen);
				continue;
			case T_ALLOPT:
			case T_TCP_NODELAY:
				if (optlen && optlen != sizeof(tcp->nodelay))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_TCP_MAXSEG:
				if (optlen && optlen != sizeof(tcp->maxseg))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_TCP_KEEPALIVE:
				if (optlen && optlen != sizeof(tcp->keepalive))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_TCP_CORK:
				if (optlen && optlen != sizeof(tcp->cork))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_TCP_KEEPIDLE:
				if (optlen && optlen != sizeof(tcp->keepidle))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_TCP_KEEPINTVL:
				if (optlen && optlen != sizeof(tcp->keepitvl))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_TCP_KEEPCNT:
				if (optlen && optlen != sizeof(tcp->keepcnt))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_TCP_SYNCNT:
				if (optlen && optlen != sizeof(tcp->syncnt))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_TCP_LINGER2:
				if (optlen && optlen != sizeof(tcp->linger2))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_TCP_DEFER_ACCEPT:
				if (optlen && optlen != sizeof(tcp->defer_accept))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_TCP_WINDOW_CLAMP:
				if (optlen && optlen != sizeof(tcp->window_clamp))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_TCP_INFO:
				if (optlen && optlen != sizeof(tcp->info))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_TCP_QUICKACK:
				if (optlen && optlen != sizeof(tcp->quickack))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		}
		case T_ISO_TP:
			if (tp->info.SERV_type == T_CLTS) {
				struct tp_tcl *tcl = &op->tcl;

				switch (ih->name) {
				default:
					olen += T_SPACE(optlen);
					continue;
				case T_ALLOPT:
				case T_TCL_TRANSDEL:
					if (optlen && optlen != sizeof(tcl->transdel))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCL_RESERRORRATE:
					if (optlen && optlen != sizeof(tcl->reserrorrate))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCL_PROTECTION:
					if (optlen && optlen != sizeof(tcl->protection))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCL_PRIORITY:
					if (optlen && optlen != sizeof(tcl->priority))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCL_CHECKSUM:
					if (optlen && optlen != sizeof(tcl->checksum))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				}
			} else {
				struct tp_tco *tco = &op->tco;

				switch (ih->name) {
				default:
					olen += T_SPACE(optlen);
					continue;
				case T_ALLOPT:
				case T_TCO_THROUGHPUT:
					if (optlen && optlen != sizeof(tco->throughput))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_TRANSDEL:
					if (optlen && optlen != sizeof(tco->transdel))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_RESERRORRATE:
					if (optlen && optlen != sizeof(tco->reserrorrate))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_TRANSFFAILPROB:
					if (optlen && optlen != sizeof(tco->transffailprob))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_ESTFAILPROB:
					if (optlen && optlen != sizeof(tco->estfailprob))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_RELFAILPROB:
					if (optlen && optlen != sizeof(tco->relfailprob))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_ESTDELAY:
					if (optlen && optlen != sizeof(tco->estdelay))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_RELDELAY:
					if (optlen && optlen != sizeof(tco->reldelay))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_CONNRESIL:
					if (optlen && optlen != sizeof(tco->connresil))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_PROTECTION:
					if (optlen && optlen != sizeof(tco->protection))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_PRIORITY:
					if (optlen && optlen != sizeof(tco->priority))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_EXPD:
					if (optlen && optlen != sizeof(tco->expd))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_LTPDU:
					if (optlen && optlen != sizeof(tco->ltpdu))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_ACKTIME:
					if (optlen && optlen != sizeof(tco->acktime))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_REASTIME:
					if (optlen && optlen != sizeof(tco->reastime))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_EXTFORM:
					if (optlen && optlen != sizeof(tco->extform))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_FLOWCTRL:
					if (optlen && optlen != sizeof(tco->flowctrl))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_CHECKSUM:
					if (optlen && optlen != sizeof(tco->checksum))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_NETEXP:
					if (optlen && optlen != sizeof(tco->netexp))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_NETRECPTCF:
					if (optlen && optlen != sizeof(tco->netrecptcf))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_PREFCLASS:
					if (optlen && optlen != sizeof(tco->prefclass))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_ALTCLASS1:
					if (optlen && optlen != sizeof(tco->altclass1))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_ALTCLASS2:
					if (optlen && optlen != sizeof(tco->altclass2))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_ALTCLASS3:
					if (optlen && optlen != sizeof(tco->altclass3))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_ALTCLASS4:
					if (optlen && optlen != sizeof(tco->altclass4))
						goto einval;
					olen += T_SPACE(optlen);
					if (ih->name != T_ALLOPT)
						continue;
				}
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		}
	}
	mi_strlog(tp->oq, 0, SL_TRACE, "%s: calculated option size = %u", __FUNCTION__, olen);
	return (olen);
      einval:
	mi_strlog(tp->oq, 0, SL_TRACE, "%s: invalid input options", __FUNCTION__);
	return (-EINVAL);
}

static int
tp_size_negotiate_options(struct tp *tp, const uchar *ip, size_t ilen)
{
	int olen = 0, optlen;
	const struct t_opthdr *ih;
	struct t_opthdr all;
	struct tp_options *op = &tp->opts.res;

	if (ip == NULL || ilen == 0) {
		/* for zero-length options, fake an option header for all names with all levels */
		all.level = T_ALLLEVELS;
		all.name = T_ALLOPT;
		all.len = sizeof(all);
		all.status = 0;
		ip = (const uchar *) &all;
		ilen = sizeof(all);
	}
	for (ih = _T_OPT_FIRSTHDR_OFS(ip, ilen, 0); ih; ih = _T_OPT_NEXTHDR_OFS(ip, ilen, ih, 0)) {
		if (ih->len < sizeof(*ih))
			goto einval;
		if ((uchar *) ih + ih->len > ip + len)
			goto einval;
		optlen = ih->len - sizeof(*ih);
		switch (ih->level) {
		default:
			goto einval;
		case T_ALLLEVELS:
			if (ih->name != T_ALLOPT)
				goto einval;
		case XTI_GENERIC:
		{
			struct tp_xti *xti = &op->xti;

			switch (ih->name) {
			default:
				olen += T_SPACE(optlen);
				continue;
			case T_ALLOPT:
			case XTI_DEBUG:
				/* can be any non-zero array of t_uscalar_t */
				if (ih->name != T_ALLOPT
				    && ((optlen % sizeof(t_uscalar_t)) != 0
					|| optlen > 4 * sizeof(t_uscalar_t)))
					goto einval;
				olen += _T_SPACE_SIZEOF(xti->debug);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_LINGER:
				if (ih->name != T_ALLOPT && optlen != sizeof(xti->linger))
					goto einval;
				olen += _T_SPACE_SIZEOF(xti->linger);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_RCVBUF:
				if (ih->name != T_ALLOPT && optlen != sizeof(xti->rcvbuf))
					goto einval;
				olen += _T_SPACE_SIZEOF(xti->rcvbuf);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_RCVLOWAT:
				if (ih->name != T_ALLOPT && optlen != sizeof(xti->rcvlowat))
					goto einval;
				olen += _T_SPACE_SIZEOF(xti->rcvlowat);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_SNDBUF:
				if (ih->name != T_ALLOPT && optlen != sizeof(xti->sndbuf))
					goto einval;
				olen += _T_SPACE_SIZEOF(xti->sndbuf);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_SNDLOWAT:
				if (ih->name != T_ALLOPT && optlen != sizeof(xti->sndlowat))
					goto einval;
				olen += _T_SPACE_SIZEOF(xti->sndlowat);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		}
		case T_INET_IP:
		{
			struct tp_ip *ip = &op->ip;

			switch (ih->name) {
			default:
				olen += T_SPACE(optlen);
				continue;
			case T_ALLOPT:
			case T_IP_OPTIONS:
				/* If the status is T_SUCCESS, T_FAILURE, T_NOTSUPPORT or
				   T_READONLY, the returned option value is the same as the one
				   requested on input. */
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_TOS:
				if (ih->name != T_ALLOPT && optlen != sizeof(ip->tos))
					goto einval;
				olen += _T_SPACE_SIZEOF(ip->tos);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_TTL:
				if (ih->name != T_ALLOPT && optlen != sizeof(ip->ttl))
					goto einval;
				olen += _T_SPACE_SIZEOF(ip->ttl);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_REUSEADDR:
				if (ih->name != T_ALLOPT && optlen != sizeof(ip->reuseaddr))
					goto einval;
				olen += _T_SPACE_SIZEOF(ip->reuseaddr);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_DONTROUTE:
				if (ih->name != T_ALLOPT && optlen != sizeof(ip->dontroute))
					goto einval;
				olen += _T_SPACE_SIZEOF(ip->dontroute);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_BROADCAST:
				if (ih->name != T_ALLOPT && optlen != sizeof(ip->broadcast))
					goto einval;
				olen += _T_SPACE_SIZEOF(ip->broadcast);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_ADDR:
				if (ih->name != T_ALLOPT && optlen != sizeof(ip->addr))
					goto einval;
				olen += _T_SPACE_SIZEOF(ip->addr);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		}
		case T_INET_UDP:
		{
			struct tp_udp *udp = &op->udp;

			switch (ih->name) {
			default:
				olen += T_SPACE(optlen);
				continue;
			case T_ALLOPT:
			case T_UDP_CHECKSUM:
				if (ih->name != T_ALLOPT && optlen != sizeof(udp->checksum))
					goto einval;
				olen += _T_SPACE_SIZEOF(udp->checksum);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		}
		case T_INET_TCP:
		{
			struct tp_tcp *tcp = &op->tcp;

			switch (ih->name) {
			default:
				olen += T_SPACE(optlen);
				continue;
			case T_ALLOPT:
			case T_TCP_NODELAY:
				if (ih->name != T_ALLOPT && optlen != sizeof(tcp->nodelay))
					goto einval;
				olen += _T_SPACE_SIZEOF(tcp->nodelay);
				if (ih->name != T_ALLOPT)
					continue;
			case T_TCP_MAXSEG:
				if (ih->name != T_ALLOPT && optlen != sizeof(tcp->maxseg))
					goto einval;
				olen += _T_SPACE_SIZEOF(tcp->maxseg);
				if (ih->name != T_ALLOPT)
					continue;
			case T_TCP_KEEPALIVE:
				if (ih->name != T_ALLOPT && optlen != sizeof(tcp->keepalive))
					goto einval;
				olen += _T_SPACE_SIZEOF(tcp->keepalive);
				if (ih->name != T_ALLOPT)
					continue;
			case T_TCP_CORK:
				if (ih->name != T_ALLOPT && optlen != sizeof(tcp->cork))
					goto einval;
				olen += _T_SPACE_SIZEOF(tcp->cork);
				if (ih->name != T_ALLOPT)
					continue;
			case T_TCP_KEEPIDLE:
				if (ih->name != T_ALLOPT && optlen != sizeof(tcp->keepidle))
					goto einval;
				olen += _T_SPACE_SIZEOF(tcp->keepidle);
				if (ih->name != T_ALLOPT)
					continue;
			case T_TCP_KEEPINTVL:
				if (ih->name != T_ALLOPT && optlen != sizeof(tcp->keepitvl))
					goto einval;
				olen += _T_SPACE_SIZEOF(tcp->keepitvl);
				if (ih->name != T_ALLOPT)
					continue;
			case T_TCP_KEEPCNT:
				if (ih->name != T_ALLOPT && optlen != sizeof(tcp->keepcnt))
					goto einval;
				olen += _T_SPACE_SIZEOF(tcp->keepcnt);
				if (ih->name != T_ALLOPT)
					continue;
			case T_TCP_SYNCNT:
				if (ih->name != T_ALLOPT && optlen != sizeof(tcp->syncnt))
					goto einval;
				olen += _T_SPACE_SIZEOF(tcp->syncnt);
				if (ih->name != T_ALLOPT)
					continue;
			case T_TCP_LINGER2:
				if (ih->name != T_ALLOPT && optlen != sizeof(tcp->linger2))
					goto einval;
				olen += _T_SPACE_SIZEOF(tcp->linger2);
				if (ih->name != T_ALLOPT)
					continue;
			case T_TCP_DEFER_ACCEPT:
				if (ih->name != T_ALLOPT && optlen != sizeof(tcp->defer_accept))
					goto einval;
				olen += _T_SPACE_SIZEOF(tcp->accept);
				if (ih->name != T_ALLOPT)
					continue;
			case T_TCP_WINDOW_CLAMP:
				if (ih->name != T_ALLOPT && optlen != sizeof(tcp->window_clamp))
					goto einval;
				olen += _T_SPACE_SIZEOF(tcp->window_clamp);
				if (ih->name != T_ALLOPT)
					continue;
			case T_TCP_INFO:
				/* If the status is T_SUCCESS, T_FAILURE, T_NOTSUPPORT or
				   T_READONLY, the returned option value is the same as the one
				   requested on input. */
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_TCP_QUICKACK:
				if (ih->name != T_ALLOPT && optlen != sizeof(tcp->quickack))
					goto einval;
				olen += _T_SPACE_SIZEOF(tcp->quickack);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		}
		case T_ISO_TP:
			if (tp->info.SERV_type == T_CLTS) {
				struct tp_tcl *tcl = &op->tcl;

				switch (ih->name) {
				default:
					olen += T_SPACE(optlen);
					continue;
				case T_ALLOPT:
				case T_TCL_TRANSDEL:
					if (ih->name != T_ALLOPT && optlen != sizeof(tcl->transdel))
						goto einval;
					olen += _T_SPACE_SIZEOF(tcl->transdel);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCL_RESERRORRATE:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(tcl->reserrorrate))
						goto einval;
					olen += _T_SPACE_SIZEOF(tcl->reserrorrate);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCL_PROTECTION:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(tcl->protection))
						goto einval;
					olen += _T_SPACE_SIZEOF(tcl->protection);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCL_PRIORITY:
					if (ih->name != T_ALLOPT && optlen != sizeof(tcl->priority))
						goto einval;
					olen += _T_SPACE_SIZEOF(tcl->priority);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCL_CHECKSUM:
					if (ih->name != T_ALLOPT && optlen != sizeof(tcl->checksum))
						goto einval;
					olen += _T_SPACE_SIZEOF(tcl->checksum);
					if (ih->name != T_ALLOPT)
						continue;
				}
			} else {
				struct tp_tco *tco = &op->tco;

				switch (ih->name) {
				default:
					olen += T_SPACE(optlen);
					continue;
				case T_ALLOPT:
				case T_TCO_THROUGHPUT:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(tco->throughput))
						goto einval;
					olen += _T_SPACE_SIZEOF(tco->throughput);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_TRANSDEL:
					if (ih->name != T_ALLOPT && optlen != sizeof(tco->transdel))
						goto einval;
					olen += _T_SPACE_SIZEOF(tco->transdel);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_RESERRORRATE:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(tco->reserrorrate))
						goto einval;
					olen += _T_SPACE_SIZEOF(tco->reserrorrate);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_TRANSFFAILPROB:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(tco->transffailprob))
						goto einval;
					olen += _T_SPACE_SIZEOF(tco->transffailprob);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_ESTFAILPROB:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(tco->estfailprob))
						goto einval;
					olen += _T_SPACE_SIZEOF(tco->estfailprob);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_RELFAILPROB:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(tco->relfailprob))
						goto einval;
					olen += _T_SPACE_SIZEOF(tco->relfailprob);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_ESTDELAY:
					if (ih->name != T_ALLOPT && optlen != sizeof(tco->estdelay))
						goto einval;
					olen += _T_SPACE_SIZEOF(tco->estdelay);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_RELDELAY:
					if (ih->name != T_ALLOPT && optlen != sizeof(tco->reldelay))
						goto einval;
					olen += _T_SPACE_SIZEOF(tco->reldelay);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_CONNRESIL:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(tco->connresil))
						goto einval;
					olen += _T_SPACE_SIZEOF(tco->connresil);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_PROTECTION:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(tco->protection))
						goto einval;
					olen += _T_SPACE_SIZEOF(tco->protection);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_PRIORITY:
					if (ih->name != T_ALLOPT && optlen != sizeof(tco->priority))
						goto einval;
					olen += _T_SPACE_SIZEOF(tco->priority);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_EXPD:
					if (ih->name != T_ALLOPT && optlen != sizeof(tco->expd))
						goto einval;
					olen += _T_SPACE_SIZEOF(tco->expd);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_LTPDU:
					if (ih->name != T_ALLOPT && optlen != sizeof(tco->ltpdu))
						goto einval;
					olen += _T_SPACE_SIZEOF(tco->ltpdu);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_ACKTIME:
					if (ih->name != T_ALLOPT && optlen != sizeof(tco->acktime))
						goto einval;
					olen += _T_SPACE_SIZEOF(tco->acktime);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_REASTIME:
					if (ih->name != T_ALLOPT && optlen != sizeof(tco->reastime))
						goto einval;
					olen += _T_SPACE_SIZEOF(tco->reastime);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_EXTFORM:
					if (ih->name != T_ALLOPT && optlen != sizeof(tco->extform))
						goto einval;
					olen += _T_SPACE_SIZEOF(tco->extform);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_FLOWCTRL:
					if (ih->name != T_ALLOPT && optlen != sizeof(tco->flowctrl))
						goto einval;
					olen += _T_SPACE_SIZEOF(tco->flowctrl);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_CHECKSUM:
					if (ih->name != T_ALLOPT && optlen != sizeof(tco->checksum))
						goto einval;
					olen += _T_SPACE_SIZEOF(tco->checksum);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_NETEXP:
					if (ih->name != T_ALLOPT && optlen != sizeof(tco->netexp))
						goto einval;
					olen += _T_SPACE_SIZEOF(tco->netexp);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_NETRECPTCF:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(tco->netrecptcf))
						goto einval;
					olen += _T_SPACE_SIZEOF(tco->netrecptcf);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_PREFCLASS:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(tco->prefclass))
						goto einval;
					olen += _T_SPACE_SIZEOF(tco->prefclass);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_ALTCLASS1:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(tco->altclass1))
						goto einval;
					olen += _T_SPACE_SIZEOF(tco->altclass1);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_ALTCLASS2:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(tco->altclass2))
						goto einval;
					olen += _T_SPACE_SIZEOF(tco->altclass2);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_ALTCLASS3:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(tco->altclass3))
						goto einval;
					olen += _T_SPACE_SIZEOF(tco->altclass3);
					if (ih->name != T_ALLOPT)
						continue;
				case T_TCO_ALTCLASS4:
					if (ih->name != T_ALLOPT
					    && optlen != sizeof(tco->altclass4))
						goto einval;
					olen += _T_SPACE_SIZEOF(tco->altclass4);
					if (ih->name != T_ALLOPT)
						continue;
				}
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		}
	}
	mi_strlog(tp->oq, 0, SL_TRACE, "%s: calculated option size = %u", __FUNCTION__, olen);
	return (olen);
      einval:
	mi_strlog(tp->oq, 0, SL_TRACE, "%s: invalid input options", __FUNCTION__);
	return (-EINVAL);
}

static int
tp_size_options(struct tp *tp, unsigned char *ip, size_t ilen, t_scalar_t flag)
{
	switch (flag) {
	case T_DEFAULT:
		return tp_size_default_options(tp, ip, ilen);
	case T_CURRENT:
		return tp_size_current_options(tp, ip, ilen);
	case T_CHECK:
		return tp_size_check_options(tp, ip, ilen);
	case T_NEGOTIATE:
		return tp_size_negotiate_options(tp, ip, ilen);
	}
	return (-EINVAL);
}

static uint
t_overall_result(uint *overall, uint result)
{
	switch (result) {
	case T_NOTSUPPORT:
		if (!(*overall & (T_NOTSUPPORT)))
			*overall = T_NOTSUPPORT;
		break;
	case T_READONLY:
		if (!(*overall & (T_NOTSUPPORT | T_READONLY)))
			*overall = T_READONLY;
		break;
	case T_FAILURE:
		if (!(*overall & (T_NOTSUPPORT | T_READONLY | T_FAILURE)))
			*overall = T_FAILURE;
		break;
	case T_PARTSUCCESS:
		if (!(*overall & (T_NOTSUPPORT | T_READONLY | T_FAILURE | T_PARTSUCCESS)))
			*overall = T_PARTSUCCESS;
		break;
	case T_SUCCESS:
		if (!
		    (*overall &
		     (T_NOTSUPPORT | T_READONLY | T_FAILURE | T_PARTSUCCESS | T_SUCCESS)))
			*overall = T_SUCCESS;
		break;
	}
	return (result);
}

static t_scalar_t
tp_build_default_options(struct tp *tp, uchar *ip, uint ilen, uchar *op, uint olen, t_scalar_t flag)
{
	t_scalar_t overall = T_SUCCESS;
	const struct t_opthdr *ih;
	struct t_opthdr *oh, all;
	int optlen;

	if (ilen == 0) {
		/* For zero-length options fake an option for all names within all levels. */
		all.level = T_ALLLEVELS;
		all.name = T_ALLOPT;
		all.len = sizeof(all);
		all.status = 0;
		ip = (unsigned char *) &all;
		ilen = sizeof(all);
	}
	for (ih = _T_OPT_FIRSTHDR_OFS(ip, ilen, 0), oh = _T_OPT_FIRSTHDR_OFS(op, *olen, 0);
	     ih && oh;
	     ih = _T_OPT_NEXTHDR_OFS(ip, ilen, ih, 0), oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)) {
		/* don't need to do this, it was done when we sized options */
		if (ih->len < sizeof(*ih))
			goto einval;
		if ((unsigned char *) ih + ih->len > ip + ilen)
			goto einval;
		optlen = ih->len - sizeof(*ih);
		switch (ih->level) {
		default:
			goto einval;
		case T_ALLLEVELS:
			if (ih->name != T_ALLOPT)
				goto einval;
		case XTI_GENERIC:
			switch (ih->name) {
			default:
				oh->len = sizeof(*oh);
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				continue;
			case T_ALLOPT:
			case XTI_DEBUG:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.xti.debug);
				oh->level = XTI_GENERIC;
				oh->name = XTI_DEBUG;
				oh->status = T_SUCCESS;
				bcopy(t_defaults.xti.debug, T_OPT_DATA(oh),
				      sizeof(t_defaults.xti.debug));
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_LINGER:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.xti.linger);
				oh->level = XTI_GENERIC;
				oh->name = XTI_LINGER;
				oh->status = T_SUCCESS;
				*((struct t_linger *) T_OPT_DATA(oh)) = t_defaults.xti.linger;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_RCVBUF:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.xti.rcvbuf);
				oh->level = XTI_GENERIC;
				oh->name = XTI_RCVBUF;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = sysctl_rmem_default;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_RCVLOWAT:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.xti.rcvlowat);
				oh->level = XTI_GENERIC;
				oh->name = XTI_RCVLOWAT;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.xti.rcvlowat;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_SNDBUF:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.xti.sndbuf);
				oh->level = XTI_GENERIC;
				oh->name = XTI_SNDBUF;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = sysctl_wmem_default;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_SNDLOWAT:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.xti.sndlowat);
				oh->level = XTI_GENERIC;
				oh->name = XTI_SNDLOWAT;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.xti.sndlowat;
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
			if (ss->p.prot.family == PF_INET)
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
		case T_INET_IP:
			switch (ih->name) {
			default:
				oh->len = sizeof(*oh);
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				continue;
			case T_ALLOPT:
			case T_IP_OPTIONS:
			{
				oh->len = sizeof(*oh);
				oh->level = T_INET_IP;
				oh->name = T_IP_OPTIONS;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				/* not supported yet */
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_IP_TOS:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.ip.tos);
				oh->level = T_INET_IP;
				oh->name = T_IP_TOS;
				oh->status = T_SUCCESS;
				*((unsigned char *) T_OPT_DATA(oh)) = t_defaults.ip.tos;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_IP_TTL:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.ip.ttl);
				oh->level = T_INET_IP;
				oh->name = T_IP_TTL;
				oh->status = T_SUCCESS;
				*((unsigned char *) T_OPT_DATA(oh)) = t_defaults.ip.ttl;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_IP_REUSEADDR:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.ip.reuseaddr);
				oh->level = T_INET_IP;
				oh->name = T_IP_REUSEADDR;
				oh->status = T_SUCCESS;
				*((unsigned int *) T_OPT_DATA(oh)) = t_defaults.ip.reuseaddr;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_IP_DONTROUTE:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.ip.dontroute);
				oh->level = T_INET_IP;
				oh->name = T_IP_DONTROUTE;
				oh->status = T_SUCCESS;
				*((unsigned int *) T_OPT_DATA(oh)) = t_defaults.ip.dontroute;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_IP_BROADCAST:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.ip.broadcast);
				oh->level = T_INET_IP;
				oh->name = T_IP_BROADCAST;
				oh->status = T_SUCCESS;
				*((unsigned int *) T_OPT_DATA(oh)) = t_defaults.ip.broadcast;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_IP_ADDR:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.ip.addr);
				oh->level = T_INET_IP;
				oh->name = T_IP_ADDR;
				oh->status = T_SUCCESS;
				*((uint32_t *) T_OPT_DATA(oh)) = t_defaults.ip.addr;
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
			if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
				goto efault;
		case T_INET_UDP:
			switch (ih->name) {
			default:
				oh->len = sizeof(*oh);
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				continue;
			case T_ALLOPT:
			case T_UDP_CHECKSUM:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.udp.checksum);
				oh->level = T_INET_UDP;
				oh->name = T_UDP_CHECKSUM;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.udp.checksum;
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		case T_INET_TCP:
			switch (ih->name) {
			default:
				oh->len = sizeof(*oh);
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				continue;
			case T_ALLOPT:
			case T_TCP_NODELAY:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.tcp.nodelay);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_NODELAY;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.tcp.nodelay;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_TCP_MAXSEG:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.tcp.maxseg);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_MAXSEG;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.tcp.maxseg;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_TCP_KEEPALIVE:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.tcp.keepalive);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_KEEPALIVE;
				oh->status = T_SUCCESS;
				*((struct t_kpalive *) T_OPT_DATA(oh)) = t_defaults.tcp.keepalive;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_TCP_CORK:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.tcp.cork);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_CORK;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.tcp.cork;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_TCP_KEEPIDLE:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.tcp.keepidle);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_KEEPIDLE;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.tcp.keepidle;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_TCP_KEEPINTVL:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.tcp.keepitvl);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_KEEPINTVL;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.tcp.keepitvl;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_TCP_KEEPCNT:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.tcp.keepcnt);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_KEEPCNT;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.tcp.keepcnt;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_TCP_SYNCNT:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.tcp.syncnt);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_SYNCNT;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.tcp.syncnt;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_TCP_LINGER2:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.tcp.linger2);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_LINGER2;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.tcp.linger2;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_TCP_DEFER_ACCEPT:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.tcp.defer_accept);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_DEFER_ACCEPT;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.tcp.defer_accept;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_TCP_WINDOW_CLAMP:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.tcp.window_clamp);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_WINDOW_CLAMP;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.tcp.window_clamp;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_TCP_INFO:
				/* read only, can't get default */
				oh->len = sizeof(*oh);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_INFO;
				oh->status = t_overall_result(&overall, T_READONLY);
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_TCP_QUICKACK:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.tcp.quickack);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_QUICKACK;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.tcp.quickack;
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		case T_ISO_TP:
			if (tp->info.SERV_type != T_CLTS) {
				switch (ih->name) {
				default:
					oh->len = sizeof(*oh);
					oh->level = ih->level;
					oh->name = ih->name;
					oh->status = t_overall_result(&overall, T_NOTSUPPORT);
					continue;
				case T_ALLOPT:
				case T_TCO_THROUGHPUT:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.throughput);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_THROUGHPUT;
					oh->status = T_SUCCESS;
					*((struct thrpt *) T_OPT_DATA(oh)) =
					    t_defaults.tco.throughput;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCO_TRANSDEL:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.transdel);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_TRANSDEL;
					oh->status = T_SUCCESS;
					*((struct transdel *) T_OPT_DATA(oh)) =
					    t_defaults.tco.transdel;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCO_RESERRORRATE:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.reserrorrate);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_RESERRORRATE;
					oh->status = T_SUCCESS;
					*((struct rate *) T_OPT_DATA(oh)) =
					    t_defaults.tco.reserrorrate;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCO_TRANSFFAILPROB:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.transffailprob);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_TRANSFFAILPROB;
					oh->status = T_SUCCESS;
					*((struct rate *) T_OPT_DATA(oh)) =
					    t_defaults.tco.transffailprob;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCO_ESTFAILPROB:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.estfailprob);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_ESTFAILPROB;
					oh->status = T_SUCCESS;
					*((struct rate *) T_OPT_DATA(oh)) =
					    t_defaults.tco.estfailprob;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCO_RELFAILPROB:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.relfailprob);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_RELFAILPROB;
					oh->status = T_SUCCESS;
					*((struct rate *) T_OPT_DATA(oh)) =
					    t_defaults.tco.relfailprob;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCO_ESTDELAY:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.estdelay);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_ESTDELAY;
					oh->status = T_SUCCESS;
					*((struct rate *) T_OPT_DATA(oh)) = t_defaults.tco.estdelay;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCO_RELDELAY:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.reldelay);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_RELDELAY;
					oh->status = T_SUCCESS;
					*((struct rate *) T_OPT_DATA(oh)) = t_defaults.tco.reldelay;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCO_CONNRESIL:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.connresil);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_CONNRESIL;
					oh->status = T_SUCCESS;
					*((struct rate *) T_OPT_DATA(oh)) =
					    t_defaults.tco.connresil;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCO_PROTECTION:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.protection);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_PROTECTION;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) =
					    t_defaults.tco.protection;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCO_PRIORITY:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.priority);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_PRIORITY;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.tco.priority;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCO_EXPD:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.expd);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_EXPD;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.tco.expd;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCO_LTPDU:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.ltpdu);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_LTPDU;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.tco.ltpdu;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCO_ACKTIME:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.acktime);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_ACKTIME;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.tco.acktime;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCO_REASTIME:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.reastime);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_REASTIME;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.tco.reastime;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCO_EXTFORM:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.extform);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_EXTFORM;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.tco.extform;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCO_FLOWCTRL:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.flowctrl);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_FLOWCTRL;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.tco.flowctrl;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCO_CHECKSUM:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.checksum);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_CHECKSUM;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.tco.checksum;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCO_NETEXP:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.netexp);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_NETEXP;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.tco.netexp;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCO_NETRECPTCF:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.netrecptcf);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_NETRECPTCF;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) =
					    t_defaults.tco.netrecptcf;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCO_PREFCLASS:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.prefclass);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_PREFCLASS;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) =
					    t_defaults.tco.prefclass;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCO_ALTCLASS1:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.altclass1);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_ALTCLASS1;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) =
					    t_defaults.tco.altclass1;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCO_ALTCLASS2:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.altclass2);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_ALTCLASS2;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) =
					    t_defaults.tco.altclass2;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCO_ALTCLASS3:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.altclass3);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_ALTCLASS3;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) =
					    t_defaults.tco.altclass3;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCO_ALTCLASS4:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.tco.altclass4);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_ALTCLASS4;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) =
					    t_defaults.tco.altclass4;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
			} else {
				switch (ih->name) {
				default:
					oh->len = sizeof(*oh);
					oh->level = ih->level;
					oh->name = ih->name;
					oh->status = t_overall_result(&overall, T_NOTSUPPORT);
					continue;
				case T_ALLOPT:
				case T_TCL_TRANSDEL:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.tcl.transdel);
					oh->level = T_ISO_TP;
					oh->name = T_TCL_TRANSDEL;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.tcl.transdel;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCL_RESERRORRATE:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.tcl.reserrorrate);
					oh->level = T_ISO_TP;
					oh->name = T_TCL_RESERRORRATE;
					oh->status = T_SUCCESS;
					*((struct rate *) T_OPT_DATA(oh)) =
					    t_defaults.tcl.reserrorrate;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCL_PROTECTION:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.tcl.protection);
					oh->level = T_ISO_TP;
					oh->name = T_TCL_PROTECTION;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) =
					    t_defaults.tcl.protection;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCL_PRIORITY:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.tcl.priority);
					oh->level = T_ISO_TP;
					oh->name = T_TCL_PRIORITY;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.tcl.priority;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCL_CHECKSUM:
					oh->len = _T_LENGTH_SIZEOF(t_defaults.tcl.checksum);
					oh->level = T_ISO_TP;
					oh->name = T_TCL_CHECKSUM;
					oh->status = T_SUCCESS;
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.tcl.checksum;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		}
	}
	if (ih && !oh)
		goto efault;
	if (oh)
		*olen = (unsigned char *) oh - op;
	return (overall);
      einval:
	return (-EINVAL);
      efault:
	mi_strlog(tp->oq, 0, SL_ERROR, "%s: SWERR", __FUNCTION__);
	return (-EFAULT);
}

static t_scalar_t
tp_build_current_options(struct tp *tp, uchar *ip, uint ilen, uchar *op, uint olen, t_scalar_t flag)
{
	t_scalar_t overall = T_SUCCESS;
	const struct t_opthdr *oh, all;
	struct tp_options *op = &tp->opts.req;
	int optlen;

	if (ilen == 0) {
		/* For zero-length options fake an option for all names within all levels. */
		all.level = T_ALLLEVELS;
		all.name = T_ALLOPT;
		all.len = sizeof(all);
		all.status = 0;
		ip = (unsigned char *) &all;
		ilen = sizeof(all);
	}
	for (ih = _T_OPT_FIRSTHDR_OFS(ip, ilen, 0), oh = _T_OPT_FIRSTHDR_OFS(op, *olen, 0);
	     ih && oh;
	     ih = _T_OPT_NEXTHDR_OFS(ip, ilen, ih, 0), oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)) {
		/* don't need to do this, it was done when we sized options */
		if (ih->len < sizeof(*ih))
			goto einval;
		if ((unsigned char *) ih + ih->len > ip + ilen)
			goto einval;
		optlen = ih->len - sizeof(*ih);
		switch (ih->level) {
		default:
			goto einval;
		case T_ALLLEVELS:
			if (ih->name != T_ALLOPT)
				goto einval;
		case XTI_GENERIC:
		{
			struct tp_xti *xti = &op->xti;

			switch (ih->name) {
			default:
				oh->len = sizeof(*oh);
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				continue;
			case T_ALLOPT:
			case XTI_DEBUG:
				oh->len = _T_LENGTH_SIZEOF(xti->debug);
				oh->level = XTI_GENERIC;
				oh->name = XTI_DEBUG;
				oh->status = T_SUCCESS;
				bcopy(xti->debug, T_OPT_DATA(oh), 4 * sizeof(t_uscalar_t));
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_LINGER:
				oh->len = _T_LENGTH_SIZEOF(xti->linger);
				oh->level = XTI_GENERIC;
				oh->name = XTI_LINGER;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((struct t_linger *) T_OPT_DATA(oh)) = xti->linger;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_RCVBUF:
				oh->len = _T_LENGTH_SIZEOF(xti->rcvbuf);
				oh->level = XTI_GENERIC;
				oh->name = XTI_RCVBUF;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = xti->rcvbuf;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_RCVLOWAT:
				oh->len = _T_LENGTH_SIZEOF(xti->rcvlowat);
				oh->level = XTI_GENERIC;
				oh->name = XTI_RCVLOWAT;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = xti->rcvlowat;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_SNDBUF:
				oh->len = _T_LENGTH_SIZEOF(xti->sndbuf);
				oh->level = XTI_GENERIC;
				oh->name = XTI_SNDBUF;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = xti->sndbuf;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_SNDLOWAT:
				oh->len = _T_LENGTH_SIZEOF(xti->sndlowat);
				oh->level = XTI_GENERIC;
				oh->name = XTI_SNDLOWAT;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = xti->sndlowat;
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
			if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
				goto efault;
		}
		case T_INET_IP:
		{
			struct tp_ip *ip = &op->ip;

			switch (ih->name) {
			default:
				oh->len = sizeof(*oh);
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				continue;
			case T_ALLOPT:
			case T_IP_OPTIONS:
			{
				oh->len = sizeof(*oh);
				oh->level = T_INET_IP;
				oh->name = T_IP_OPTIONS;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				/* not supported yet */
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_IP_TOS:
				oh->len = _T_LENGTH_SIZEOF(ip->tos);
				oh->level = T_INET_IP;
				oh->name = T_IP_TOS;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((unsigned char *) T_OPT_DATA(oh)) = ip->tos;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_IP_TTL:
				oh->len = _T_LENGTH_SIZEOF(ip->ttl);
				oh->level = T_INET_IP;
				oh->name = T_IP_TTL;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((unsigned char *) T_OPT_DATA(oh)) = ip->ttl;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_IP_REUSEADDR:
				oh->len = _T_LENGTH_SIZEOF(ip->reuseaddr);
				oh->level = T_INET_IP;
				oh->name = T_IP_REUSEADDR;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((unsigned int *) T_OPT_DATA(oh)) = ip->reuseaddr;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_IP_DONTROUTE:
				oh->len = _T_LENGTH_SIZEOF(ip->dontroute);
				oh->level = T_INET_IP;
				oh->name = T_IP_DONTROUTE;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((unsigned int *) T_OPT_DATA(oh)) = ip->dontroute;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_IP_BROADCAST:
				oh->len = _T_LENGTH_SIZEOF(ip->broadcast);
				oh->level = T_INET_IP;
				oh->name = T_IP_BROADCAST;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((unsigned int *) T_OPT_DATA(oh)) = ip->broadcast;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_IP_ADDR:
				oh->len = _T_LENGTH_SIZEOF(ip->addr);
				oh->level = T_INET_IP;
				oh->name = T_IP_ADDR;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((uint32_t *) T_OPT_DATA(oh)) = ip->addr;
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
			if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
				goto efault;
		}
		case T_INET_UDP:
		{
			struct tp_udp *udp = &op->udp;

			switch (ih->name) {
			default:
				oh->len = sizeof(*oh);
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				continue;
			case T_ALLOPT:
			case T_UDP_CHECKSUM:
				oh->len = _T_LENGTH_SIZEOF(udp->checksum);
				oh->level = T_INET_UDP;
				oh->name = T_UDP_CHECKSUM;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = udp->checksum;
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		}
		case T_INET_TCP:
		{
			struct tp_tcp *tcp = &op->tcp;

			switch (ih->name) {
			default:
				oh->len = sizeof(*oh);
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				continue;
			case T_ALLOPT:
			case T_TCP_NODELAY:
				oh->len = _T_LENGTH_SIZEOF(tcp->nodelay);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_NODELAY;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = tcp->nodelay;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_TCP_MAXSEG:
				oh->len = _T_LENGTH_SIZEOF(tcp->maxseg);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_MAXSEG;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = tcp->maxseg;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_TCP_KEEPALIVE:
				oh->len = _T_LENGTH_SIZEOF(tcp->keepalive);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_KEEPALIVE;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((struct t_kpalive *) T_OPT_DATA(oh)) = tcp->keepalive;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_TCP_CORK:
				oh->len = _T_LENGTH_SIZEOF(tcp->cork);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_CORK;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = tcp->cork;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_TCP_KEEPIDLE:
				oh->len = _T_LENGTH_SIZEOF(tcp->keepidle);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_KEEPIDLE;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = tcp->keepidle;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_TCP_KEEPINTVL:
				oh->len = _T_LENGTH_SIZEOF(tcp->keepitvl);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_KEEPINTVL;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = tcp->keepitvl;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_TCP_KEEPCNT:
				oh->len = _T_LENGTH_SIZEOF(tcp->keepcnt);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_KEEPCNT;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = tcp->keepcnt;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_TCP_SYNCNT:
				oh->len = _T_LENGTH_SIZEOF(tcp->syncnt);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_SYNCNT;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = tcp->syncnt;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_TCP_LINGER2:
				oh->len = _T_LENGTH_SIZEOF(tcp->linger2);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_LINGER2;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = tcp->linger2;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_TCP_DEFER_ACCEPT:
				oh->len = _T_LENGTH_SIZEOF(tcp->defer_accept);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_DEFER_ACCEPT;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = tcp->defer_accept;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_TCP_WINDOW_CLAMP:
				oh->len = _T_LENGTH_SIZEOF(tcp->window_clamp);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_WINDOW_CLAMP;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = tcp->window_clamp;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_TCP_INFO:
				oh->len = _T_LENGTH_SIZEOF(tcp->info);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_INFO;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((struct t_tcp_info *) T_OPT_DATA(oh)) = tcp->info;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_TCP_QUICKACK:
				oh->len = _T_LENGTH_SIZEOF(tcp->quickack);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_QUICKACK;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = tcp->quickack;
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		}
		case T_ISO_TP:
			if (tp->info.SERV_type == T_CLTS) {
				struct tp_tcl *tcl = &op->tcl;

				switch (ih->name) {
				default:
					oh->len = sizeof(*oh);
					oh->level = ih->level;
					oh->name = ih->name;
					oh->status = t_overall_result(&overall, T_NOTSUPPORT);
					continue;
				case T_ALLOPT:
				case T_TCL_TRANSDEL:
					oh->len = _T_LENGTH_SIZEOF(tcl->transdel);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_THROUGHPUT;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((struct rate *) T_OPT_DATA(oh)) = tcl->transdel;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(tcl->transdel)))
						goto efault;
				case T_TCL_RESERRORRATE:
					oh->len = _T_LENGTH_SIZEOF(tcl->reserrorrate);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_THROUGHPUT;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_scalar_t *) T_OPT_DATA(oh)) = tcl->reserrorrate;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(tcl->reserrorrate)))
						goto efault;
				case T_TCL_PROTECTION:
					oh->len = _T_LENGTH_SIZEOF(tcl->protection);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_THROUGHPUT;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_scalar_t *) T_OPT_DATA(oh)) = tcl->protection;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(tcl->protection)))
						goto efault;
				case T_TCL_PRIORITY:
					oh->len = _T_LENGTH_SIZEOF(tcl->priority);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_THROUGHPUT;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_scalar_t *) T_OPT_DATA(oh)) = tcl->priority;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(tcl->priority)))
						goto efault;
				case T_TCL_CHECKSUM:
					oh->len = _T_LENGTH_SIZEOF(tcl->checksum);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_THROUGHPUT;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_scalar_t *) T_OPT_DATA(oh)) = tcl->checksum;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(tcl->checksum)))
						goto efault;
				}
			} else {
				struct tp_tco *tco = &op->tco;

				switch (ih->name) {
				default:
					oh->len = sizeof(*oh);
					oh->level = ih->level;
					oh->name = ih->name;
					oh->status = t_overall_result(&overall, T_NOTSUPPORT);
					continue;
				case T_ALLOPT:
				case T_TCO_THROUGHPUT:
					oh->len = _T_LENGTH_SIZEOF(tco->throughput);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_THROUGHPUT;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((struct thrpt *) T_OPT_DATA(oh)) = tco->throughput;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(tco->throughput)))
						goto efault;
				case T_TCO_TRANSDEL:
					oh->len = _T_LENGTH_SIZEOF(tco->transdel);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_THROUGHPUT;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((struct rate *) T_OPT_DATA(oh)) = tco->transdel;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(tco->transdel)))
						goto efault;
				case T_TCO_RESERRORRATE:
					oh->len = _T_LENGTH_SIZEOF(tco->reserrorrate);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_THROUGHPUT;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((struct rate *) T_OPT_DATA(oh)) = tco->reserrorrate;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(tco->reserrorrate)))
						goto efault;
				case T_TCO_TRANSFFAILPROB:
					oh->len = _T_LENGTH_SIZEOF(tco->transffailprob);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_THROUGHPUT;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((struct rate *) T_OPT_DATA(oh)) = tco->transffailprob;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(tco->transffailprob)))
						goto efault;
				case T_TCO_ESTFAILPROB:
					oh->len = _T_LENGTH_SIZEOF(tco->estfailprob);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_THROUGHPUT;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((struct rate *) T_OPT_DATA(oh)) = tco->estfailprob;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(tco->estfailprob)))
						goto efault;
				case T_TCO_RELFAILPROB:
					oh->len = _T_LENGTH_SIZEOF(tco->relfailprob);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_THROUGHPUT;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((struct rate *) T_OPT_DATA(oh)) = tco->relfailprob;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(tco->relfailprob)))
						goto efault;
				case T_TCO_ESTDELAY:
					oh->len = _T_LENGTH_SIZEOF(tco->estdelay);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_THROUGHPUT;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((struct rate *) T_OPT_DATA(oh)) = tco->estdelay;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(tco->estdelay)))
						goto efault;
				case T_TCO_RELDELAY:
					oh->len = _T_LENGTH_SIZEOF(tco->reldelay);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_THROUGHPUT;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((struct rate *) T_OPT_DATA(oh)) = tco->reldelay;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(tco->reldelay)))
						goto efault;
				case T_TCO_CONNRESIL:
					oh->len = _T_LENGTH_SIZEOF(tco->connresil);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_THROUGHPUT;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((struct rate *) T_OPT_DATA(oh)) = tco->connresil;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(tco->connresil)))
						goto efault;
				case T_TCO_PROTECTION:
					oh->len = _T_LENGTH_SIZEOF(tco->protection);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_THROUGHPUT;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_scalar_t *) T_OPT_DATA(oh)) = tco->protection;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(tco->protection)))
						goto efault;
				case T_TCO_PRIORITY:
					oh->len = _T_LENGTH_SIZEOF(tco->priority);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_THROUGHPUT;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_scalar_t *) T_OPT_DATA(oh)) = tco->priority;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(tco->priority)))
						goto efault;
				case T_TCO_EXPD:
					oh->len = _T_LENGTH_SIZEOF(tco->expd);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_THROUGHPUT;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_scalar_t *) T_OPT_DATA(oh)) = tco->expd;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(tco->expd)))
						goto efault;
				case T_TCO_LTPDU:
					oh->len = _T_LENGTH_SIZEOF(tco->ltpdu);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_THROUGHPUT;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_scalar_t *) T_OPT_DATA(oh)) = tco->ltpdu;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(tco->ltpdu)))
						goto efault;
				case T_TCO_ACKTIME:
					oh->len = _T_LENGTH_SIZEOF(tco->acktime);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_THROUGHPUT;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_scalar_t *) T_OPT_DATA(oh)) = tco->acktime;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(tco->acktime)))
						goto efault;
				case T_TCO_REASTIME:
					oh->len = _T_LENGTH_SIZEOF(tco->reastime);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_THROUGHPUT;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_scalar_t *) T_OPT_DATA(oh)) = tco->reastime;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(tco->reastime)))
						goto efault;
				case T_TCO_EXTFORM:
					oh->len = _T_LENGTH_SIZEOF(tco->extform);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_THROUGHPUT;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_scalar_t *) T_OPT_DATA(oh)) = tco->extform;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(tco->extform)))
						goto efault;
				case T_TCO_FLOWCTRL:
					oh->len = _T_LENGTH_SIZEOF(tco->flowctrl);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_THROUGHPUT;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_scalar_t *) T_OPT_DATA(oh)) = tco->flowctrl;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(tco->flowctrl)))
						goto efault;
				case T_TCO_CHECKSUM:
					oh->len = _T_LENGTH_SIZEOF(tco->checksum);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_THROUGHPUT;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_scalar_t *) T_OPT_DATA(oh)) = tco->checksum;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(tco->checksum)))
						goto efault;
				case T_TCO_NETEXP:
					oh->len = _T_LENGTH_SIZEOF(tco->netexp);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_THROUGHPUT;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_scalar_t *) T_OPT_DATA(oh)) = tco->netexp;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(tco->netexp)))
						goto efault;
				case T_TCO_NETRECPTCF:
					oh->len = _T_LENGTH_SIZEOF(tco->netrecptcf);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_THROUGHPUT;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_scalar_t *) T_OPT_DATA(oh)) = tco->netrecptcf;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(tco->netrecptcf)))
						goto efault;
				case T_TCO_PREFCLASS:
					oh->len = _T_LENGTH_SIZEOF(tco->prefclass);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_THROUGHPUT;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_scalar_t *) T_OPT_DATA(oh)) = tco->prefclass;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(tco->prefclass)))
						goto efault;
				case T_TCO_ALTCLASS1:
					oh->len = _T_LENGTH_SIZEOF(tco->altclass1);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_THROUGHPUT;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_scalar_t *) T_OPT_DATA(oh)) = tco->altclass1;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(tco->altclass1)))
						goto efault;
				case T_TCO_ALTCLASS2:
					oh->len = _T_LENGTH_SIZEOF(tco->altclass2);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_THROUGHPUT;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_scalar_t *) T_OPT_DATA(oh)) = tco->altclass2;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(tco->altclass2)))
						goto efault;
				case T_TCO_ALTCLASS3:
					oh->len = _T_LENGTH_SIZEOF(tco->altclass3);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_THROUGHPUT;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_scalar_t *) T_OPT_DATA(oh)) = tco->altclass3;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(tco->altclass3)))
						goto efault;
				case T_TCO_ALTCLASS4:
					oh->len = _T_LENGTH_SIZEOF(tco->altclass4);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_THROUGHPUT;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_scalar_t *) T_OPT_DATA(oh)) = tco->altclass4;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(tco->altclass4)))
						goto efault;
				}
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		}
	}
	if (ih && !oh)
		goto efault;
	if (oh)
		*olen = (unsigned char *) oh - op;
	return (overall);
      einval:
	return (-EINVAL);
      efault:
	mi_strlog(tp->oq, 0, SL_ERROR, "%s: software error", __FUNCTION);
	return (-EFAULT);
}

static t_scalar_t
tp_build_check_options(struct tp *tp, uchar *ip, uint ilen, uchar *op, uint olen, t_scalar_t flag)
{
	t_scalar_t overall = T_SUCCESS;
	const struct t_opthdr *ih;
	struct t_opthdr *oh, all;
	int optlen;

	if (ilen == 0) {
		/* For zero-length options fake an option for all names within all levels. */
		all.level = T_ALLLEVELS;
		all.name = T_ALLOPT;
		all.len = sizeof(all);
		all.status = 0;
		ip = (unsigned char *) &all;
		ilen = sizeof(all);
	}
	for (ih = _T_OPT_FIRSTHDR_OFS(ip, ilen, 0), oh = _T_OPT_FIRSTHDR_OFS(op, *olen, 0);
	     ih && oh;
	     ih = _T_OPT_NEXTHDR_OFS(ip, ilen, ih, 0), oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)) {
		/* don't need to do this, it was done when we sized options */
		if (ih->len < sizeof(*ih))
			goto einval;
		if ((unsigned char *) ih + ih->len > ip + ilen)
			goto einval;
		optlen = ih->len - sizeof(*ih);
		switch (ih->level) {
		default:
			goto einval;
		case T_ALLLEVELS:
			if (ih->name != T_ALLOPT)
				goto einval;
		case XTI_GENERIC:
			switch (ih->name) {
			default:
				oh->len = ih->len;
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				if (optlen)
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				continue;
			case T_ALLOPT:
			case XTI_DEBUG:
				oh->len = ih->len;
				oh->level = XTI_GENERIC;
				oh->name = XTI_DEBUG;
				oh->status = T_SUCCESS;
				if (optlen) {
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (!capable(CAP_NET_ADMIN)) {
						oh->status =
						    t_overall_result(&overall, T_NOTSUPPORT);
					} else if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(oh), valp, optlen);
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_LINGER:
				oh->len = ih->len;
				oh->level = XTI_GENERIC;
				oh->name = XTI_LINGER;
				oh->status = T_SUCCESS;
				if (optlen) {
					struct t_linger *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if ((valp->l_onoff != T_NO && valp->l_onoff != T_YES)
					    || (valp->l_linger == T_UNSPEC
						&& valp->l_onoff != T_NO))
						goto einval;
					if (valp->l_linger == T_UNSPEC) {
						valp->l_linger = t_defaults.xti.linger.l_linger;
					}
					if (valp->l_linger == T_INFINITE) {
						valp->l_linger = MAX_SCHEDULE_TIMEOUT / HZ;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					if (valp->l_linger < 0)
						goto einval;
					if (valp->l_linger > MAX_SCHEDULE_TIMEOUT / HZ) {
						valp->l_linger = MAX_SCHEDULE_TIMEOUT / HZ;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_RCVBUF:
				oh->len = ih->len;
				oh->level = XTI_GENERIC;
				oh->name = XTI_RCVBUF;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp > sysctl_rmem_max) {
						*valp = sysctl_rmem_max;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					} else if (*valp < SOCK_MIN_RCVBUF / 2) {
						*valp = SOCK_MIN_RCVBUF / 2;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_RCVLOWAT:
				oh->len = ih->len;
				oh->level = XTI_GENERIC;
				oh->name = XTI_RCVLOWAT;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp > INT_MAX) {
						*valp = INT_MAX;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					} else if (*valp <= 0) {
						*valp = 1;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_SNDBUF:
				oh->len = ih->len;
				oh->level = XTI_GENERIC;
				oh->name = XTI_SNDBUF;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp > sysctl_rmem_max) {
						*valp = sysctl_rmem_max;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					} else if (*valp < SOCK_MIN_SNDBUF / 2) {
						*valp = SOCK_MIN_SNDBUF / 2;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					break;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_SNDLOWAT:
				oh->len = ih->len;
				oh->level = XTI_GENERIC;
				oh->name = XTI_SNDLOWAT;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp > 1) {
						*valp = 1;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					} else if (*valp <= 0) {
						*valp = 1;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					break;
				}
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
			if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
				goto efault;
		case T_INET_IP:
			switch (ih->name) {
			default:
				oh->len = ih->len;
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				if (optlen)
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				continue;
			case T_ALLOPT:
			case T_IP_OPTIONS:
				/* not supported yet */
				oh->len = ih->len;
				oh->level = T_INET_IP;
				oh->name = T_IP_OPTIONS;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				if (optlen)
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_IP_TOS:
				oh->len = ih->len;
				oh->level = T_INET_IP;
				oh->name = T_IP_TOS;
				oh->status = T_SUCCESS;
				if (optlen) {
					unsigned char *valp = (typeof(valp)) T_OPT_DATA(oh);
					unsigned char prec, type;

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					prec = (*valp >> 5) & 0x7;
					type = *valp & (T_LDELAY | T_HITHRPT | T_HIREL | T_LOCOST);
					if (*valp != SET_TOS(prec, type))
						goto einval;
					if (prec >= T_CRITIC_ECP && !capable(CAP_NET_ADMIN))
						oh->status =
						    t_overall_result(&overall, T_NOTSUPPORT);
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_IP_TTL:
				oh->len = ih->len;
				oh->level = T_INET_IP;
				oh->name = T_IP_TTL;
				oh->status = T_SUCCESS;
				if (optlen) {
					unsigned char *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp == 0) {
						*valp = sysctl_ip_default_ttl;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					if (*valp < 1) {
						*valp = 1;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
#if 0
					if (*valp > 255) {
						*valp = 255;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
#endif
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_IP_REUSEADDR:
				oh->len = ih->len;
				oh->level = T_INET_IP;
				oh->name = T_IP_REUSEADDR;
				oh->status = T_SUCCESS;
				if (optlen) {
					unsigned int *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp != T_YES && *valp != T_NO)
						goto einval;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_IP_DONTROUTE:
				oh->len = ih->len;
				oh->level = T_INET_IP;
				oh->name = T_IP_DONTROUTE;
				oh->status = T_SUCCESS;
				if (optlen) {
					unsigned int *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp != T_YES && *valp != T_NO)
						goto einval;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_IP_BROADCAST:
				oh->len = ih->len;
				oh->level = T_INET_IP;
				oh->name = T_IP_BROADCAST;
				oh->status = T_SUCCESS;
				if (optlen) {
					unsigned int *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp != T_YES && *valp != T_NO)
						goto einval;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_IP_ADDR:
				oh->len = ih->len;
				oh->level = T_INET_IP;
				oh->name = T_IP_ADDR;
				oh->status = T_SUCCESS;
				if (optlen)
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
			if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
				goto efault;
		case T_INET_UDP:
			switch (ih->name) {
			default:
				oh->len = ih->len;
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				if (optlen)
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				continue;
			case T_ALLOPT:
			case T_UDP_CHECKSUM:
				oh->len = ih->len;
				oh->level = T_INET_UDP;
				oh->name = T_UDP_CHECKSUM;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp != T_YES && *valp != T_NO)
						goto einval;
				}
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		case T_INET_TCP:
			switch (ih->name) {
			default:
				oh->len = ih->len;
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				if (optlen)
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				continue;
			case T_ALLOPT:
			case T_TCP_NODELAY:
				oh->len = ih->len;
				oh->level = T_INET_TCP;
				oh->name = T_TCP_NODELAY;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp != T_YES && *valp != T_NO)
						goto einval;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_TCP_MAXSEG:
				oh->len = ih->len;
				oh->level = T_INET_TCP;
				oh->name = T_TCP_MAXSEG;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp < 8) {
						*valp = 8;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					if (*valp > MAX_TCP_WINDOW) {
						*valp = MAX_TCP_WINDOW;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_TCP_KEEPALIVE:
				oh->len = ih->len;
				oh->level = T_INET_TCP;
				oh->name = T_TCP_KEEPALIVE;
				oh->status = T_SUCCESS;
				if (optlen) {
					struct t_kpalive *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (valp->kp_onoff != T_YES && valp->kp_onoff != T_NO)
						goto einval;
					if (valp->kp_timeout == T_UNSPEC)
						valp->kp_timeout =
						    t_defaults.tcp.keepalive.kp_timeout;
					if (valp->kp_timeout < 0)
						goto einval;
					if (valp->kp_timeout > MAX_SCHEDULE_TIMEOUT / 60 / HZ) {
						valp->kp_timeout = MAX_SCHEDULE_TIMEOUT / 60 / HZ;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					if (valp->kp_onoff == T_YES && valp->kp_timeout < 1) {
						valp->kp_timeout = 1;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_TCP_CORK:
				oh->len = ih->len;
				oh->level = T_INET_TCP;
				oh->name = T_TCP_CORK;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp != T_YES && *valp != T_NO)
						goto einval;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_TCP_KEEPIDLE:
				oh->len = ih->len;
				oh->level = T_INET_TCP;
				oh->name = T_TCP_KEEPIDLE;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp < 1) {
						*valp = 1;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					if (*valp > MAX_TCP_KEEPIDLE) {
						*valp = MAX_TCP_KEEPIDLE;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_TCP_KEEPINTVL:
				oh->len = ih->len;
				oh->level = T_INET_TCP;
				oh->name = T_TCP_KEEPINTVL;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp < 1) {
						*valp = 1;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					if (*valp > MAX_TCP_KEEPINTVL) {
						*valp = MAX_TCP_KEEPINTVL;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_TCP_KEEPCNT:
				oh->len = ih->len;
				oh->level = T_INET_TCP;
				oh->name = T_TCP_KEEPCNT;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp < 1) {
						*valp = 1;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					if (*valp > MAX_TCP_KEEPCNT) {
						*valp = MAX_TCP_KEEPCNT;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_TCP_SYNCNT:
				oh->len = ih->len;
				oh->level = T_INET_TCP;
				oh->name = T_TCP_SYNCNT;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp < 1) {
						*valp = 1;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					if (*valp > MAX_TCP_SYNCNT) {
						*valp = MAX_TCP_SYNCNT;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_TCP_LINGER2:
				oh->len = ih->len;
				oh->level = T_INET_TCP;
				oh->name = T_TCP_LINGER2;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp != T_INFINITE) {
						if (*valp > sysctl_tcp_fin_timeout / HZ)
							*valp = 0;
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_TCP_DEFER_ACCEPT:
				oh->len = ih->len;
				oh->level = T_INET_TCP;
				oh->name = T_TCP_DEFER_ACCEPT;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp == T_INFINITE
					    || *valp > ((TCP_TIMEOUT_INIT / HZ) << 31)) {
						*valp = (TCP_TIMEOUT_INIT / HZ) << 31;
					} else if (*valp != 0) {
						int count;

						for (count = 0;
						     count < 32
						     && *valp > ((TCP_TIMEOUT_INIT / HZ) << count);
						     count++) ;
						count++;
						if (*valp != ((TCP_TIMEOUT_INIT / HZ) << count)) {
							*valp = (TCP_TIMEOUT_INIT / HZ) << count;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_TCP_WINDOW_CLAMP:
				oh->len = ih->len;
				oh->level = T_INET_TCP;
				oh->name = T_TCP_WINDOW_CLAMP;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp < SOCK_MIN_RCVBUF / 2) {
						*valp = SOCK_MIN_RCVBUF / 2;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_TCP_INFO:
				/* read-only */
				oh->len = ih->len;
				oh->level = T_INET_TCP;
				oh->name = T_TCP_INFO;
				oh->status = t_overall_result(&overall, T_READONLY);
				if (optlen)
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_TCP_QUICKACK:
				oh->len = ih->len;
				oh->level = T_INET_TCP;
				oh->name = T_TCP_QUICKACK;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp != T_YES && *valp != T_NO)
						goto einval;
				}
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		case T_ISO_TP:
			if (tp->info.SERV_type == T_CLTS) {
				switch (ih->name) {
				default:
					oh->len = ih->len;
					oh->level = ih->level;
					oh->name = ih->name;
					oh->status = t_overall_result(&overall, T_NOTSUPPORT);
					if (optlen)
						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					continue;
				case T_ALLOPT:
				case T_TCL_TRANSDEL:
					oh->len = ih->len;
					oh->level = T_ISO_TP;
					oh->name = T_TCL_TRANSDEL;
					oh->state = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						if (*valp == 0)
							goto einval;
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCL_RESERRORRATE:
					oh->len = ih->len;
					oh->level = T_ISO_TP;
					oh->name = T_TCL_RESERRORRATE;
					oh->state = T_SUCCESS;
					if (optlen) {
						struct rate *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						if (valp->targetvalue != T_UNSPEC
						    && valp->targetvalue < 0)
							goto einval;
						if (valp->minacceptvalue != T_UNSPEC
						    && valp->minacceptvalue < 0)
							goto einval;
						if (valp->targetvalue < valp->minacceptvalue)
							goto einval;
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCL_PROTECTION:
					oh->len = ih->len;
					oh->level = T_ISO_TP;
					oh->name = T_TCL_PROTECTION;
					oh->state = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						switch (*valp) {
						case T_UNSPEC:
						case T_ABSREQ | T_NOPROTECT:
						case T_ABSREQ | T_ACTIVEPROTECT:
						case T_ABSREQ | T_PASSIVEPROTECT:
						case T_ABSREQ | T_ACTIVEPROTECT | T_PASSIVEPROTECT:
						case T_NOPROTECT:
						case T_ACTIVEPROTECT:
						case T_PASSIVEPROTECT:
						case T_ACTIVEPROTECT | T_PASSIVEPROTECT:
							break;
						default:
							goto einval;
						}
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCL_PRIORITY:
					oh->len = ih->len;
					oh->level = T_ISO_TP;
					oh->name = T_TCL_PRIORITY;
					oh->state = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						switch (*valp) {
						case T_UNSPEC:
						case T_PRIDFLT:
						case T_PRILOW:
						case T_PRIMID:
						case T_PRIHIGH:
						case T_PRITOP:
							break;
						default:
							goto einval;
						}
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCL_CHECKSUM:
					oh->len = ih->len;
					oh->level = T_ISO_TP;
					oh->name = T_TCL_CHECKSUM;
					oh->state = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						switch (*valp) {
						case T_YES:
						case T_NO:
							break;
						default:
							goto einval;
						}
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
			} else {
				switch (ih->name) {
				default:
					oh->len = ih->len;
					oh->level = ih->level;
					oh->name = ih->name;
					oh->status = t_overall_result(&overall, T_NOTSUPPORT);
					if (optlen)
						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					continue;
				case T_ALLOPT:
				case T_TCO_THROUGHPUT:
					oh->len = ih->len;
					oh->level = T_ISO_TP;
					oh->name = T_TCO_THROUGHPUT;
					oh->state = T_SUCCESS;
					if (optlen) {
						struct thrpt *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCO_TRANSDEL:
					oh->len = ih->len;
					oh->level = T_ISO_TP;
					oh->name = T_TCO_TRANSDEL;
					oh->state = T_SUCCESS;
					if (optlen) {
						struct transdel *valp =
						    (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCO_RESERRORRATE:
					oh->len = ih->len;
					oh->level = T_ISO_TP;
					oh->name = T_TCO_RESERRORRATE;
					oh->state = T_SUCCESS;
					if (optlen) {
						struct rate *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCO_TRANSFFAILPROB:
					oh->len = ih->len;
					oh->level = T_ISO_TP;
					oh->name = T_TCO_TRANSFFAILPROB;
					oh->state = T_SUCCESS;
					if (optlen) {
						struct rate *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCO_ESTFAILPROB:
					oh->len = ih->len;
					oh->level = T_ISO_TP;
					oh->name = T_TCO_ESTFAILPROB;
					oh->state = T_SUCCESS;
					if (optlen) {
						struct rate *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCO_RELFAILPROB:
					oh->len = ih->len;
					oh->level = T_ISO_TP;
					oh->name = T_TCO_RELFAILPROB;
					oh->state = T_SUCCESS;
					if (optlen) {
						struct rate *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCO_ESTDELAY:
					oh->len = ih->len;
					oh->level = T_ISO_TP;
					oh->name = T_TCO_ESTDELAY;
					oh->state = T_SUCCESS;
					if (optlen) {
						struct rate *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCO_RELDELAY:
					oh->len = ih->len;
					oh->level = T_ISO_TP;
					oh->name = T_TCO_RELDELAY;
					oh->state = T_SUCCESS;
					if (optlen) {
						struct rate *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCO_CONNRESIL:
					oh->len = ih->len;
					oh->level = T_ISO_TP;
					oh->name = T_TCO_CONNRESIL;
					oh->state = T_SUCCESS;
					if (optlen) {
						struct rate *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCO_PROTECTION:
					oh->len = ih->len;
					oh->level = T_ISO_TP;
					oh->name = T_TCO_PROTECTION;
					oh->state = T_SUCCESS;
					if (optlen) {
						t_scalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCO_PRIORITY:
					oh->len = ih->len;
					oh->level = T_ISO_TP;
					oh->name = T_TCO_PRIORITY;
					oh->state = T_SUCCESS;
					if (optlen) {
						t_scalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCO_EXPD:
					oh->len = ih->len;
					oh->level = T_ISO_TP;
					oh->name = T_TCO_EXPD;
					oh->state = T_SUCCESS;
					if (optlen) {
						t_scalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCO_LTPDU:
					oh->len = ih->len;
					oh->level = T_ISO_TP;
					oh->name = T_TCO_LTPDU;
					oh->state = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						/* maximum length in octets */
						/* any value is valid */
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCO_ACKTIME:
					oh->len = ih->len;
					oh->level = T_ISO_TP;
					oh->name = T_TCO_ACKTIME;
					oh->state = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						/* time in milliseconds */
						switch (*valp) {
						case T_UNSPEC:
						default:
							break;
						case 0:
							goto einval;
						}
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCO_REASTIME:
					oh->len = ih->len;
					oh->level = T_ISO_TP;
					oh->name = T_TCO_REASTIME;
					oh->state = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						/* time in seconds */
						switch (*valp) {
						case T_UNSPEC:
						default:
							break;
						case 0:
							goto einval;
						}
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCO_EXTFORM:
					oh->len = ih->len;
					oh->level = T_ISO_TP;
					oh->name = T_TCO_EXTFORM;
					oh->state = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						/* extended format */
						switch (*valp) {
						case T_UNSPEC:
						case T_NO:
						case T_YES:
							break;
						default:
							goto einval;
						}
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCO_FLOWCTRL:
					oh->len = ih->len;
					oh->level = T_ISO_TP;
					oh->name = T_TCO_FLOWCTRL;
					oh->state = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						/* flow control */
						switch (*valp) {
						case T_UNSPEC:
						case T_NO:
						case T_YES:
							break;
						default:
							goto einval;
						}
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCO_CHECKSUM:
					oh->len = ih->len;
					oh->level = T_ISO_TP;
					oh->name = T_TCO_CHECKSUM;
					oh->state = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						/* checksum */
						switch (*valp) {
						case T_UNSPEC:
						case T_NO:
						case T_YES:
							break;
						default:
							goto einval;
						}
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCO_NETEXP:
					oh->len = ih->len;
					oh->level = T_ISO_TP;
					oh->name = T_TCO_NETEXP;
					oh->state = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						/* network expedited data */
						switch (*valp) {
						case T_UNSPEC:
						case T_NO:
						case T_YES:
							break;
						default:
							goto einval;
						}
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCO_NETRECPTCF:
					oh->len = ih->len;
					oh->level = T_ISO_TP;
					oh->name = T_TCO_NETRECPTCF;
					oh->state = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						/* network receipt confirmation */
						switch (*valp) {
						case T_UNSPEC:
						case T_NO:
						case T_YES:
							break;
						default:
							goto einval;
						}
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCO_PREFCLASS:
					oh->len = ih->len;
					oh->level = T_ISO_TP;
					oh->name = T_TCO_PREFCLASS;
					oh->state = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						/* preferred class */
						switch (*valp) {
						case T_CLASS0:
						case T_CLASS1:
						case T_CLASS2:
						case T_CLASS3:
						case T_CLASS4:
						case T_UNSPEC:
							break;
						default:
							goto einval;
						}
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCO_ALTCLASS1:
					oh->len = ih->len;
					oh->level = T_ISO_TP;
					oh->name = T_TCO_ALTCLASS1;
					oh->state = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						/* alternate class */
						switch (*valp) {
						case T_CLASS0:
						case T_CLASS1:
						case T_CLASS2:
						case T_CLASS3:
						case T_CLASS4:
						case T_UNSPEC:
							break;
						default:
							goto einval;
						}
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCO_ALTCLASS2:
					oh->len = ih->len;
					oh->level = T_ISO_TP;
					oh->name = T_TCO_ALTCLASS2;
					oh->state = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						/* alternate class */
						switch (*valp) {
						case T_CLASS0:
						case T_CLASS1:
						case T_CLASS2:
						case T_CLASS3:
						case T_CLASS4:
						case T_UNSPEC:
							break;
						default:
							goto einval;
						}
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCO_ALTCLASS3:
					oh->len = ih->len;
					oh->level = T_ISO_TP;
					oh->name = T_TCO_ALTCLASS3;
					oh->state = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						/* alternate class */
						switch (*valp) {
						case T_CLASS0:
						case T_CLASS1:
						case T_CLASS2:
						case T_CLASS3:
						case T_CLASS4:
						case T_UNSPEC:
							break;
						default:
							goto einval;
						}
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCO_ALTCLASS4:
					oh->len = ih->len;
					oh->level = T_ISO_TP;
					oh->name = T_TCO_ALTCLASS4;
					oh->state = T_SUCCESS;
					if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
						if (optlen != sizeof(*valp))
							goto einval;
						/* alternate class */
						switch (*valp) {
						case T_CLASS0:
						case T_CLASS1:
						case T_CLASS2:
						case T_CLASS3:
						case T_CLASS4:
						case T_UNSPEC:
							break;
						default:
							goto einval;
						}
					}
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				}
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		}
	}
	if (ih && !oh)
		goto efault;
	if (oh)
		*olen = (unsigned char *) oh - op;
	return (overall);
      einval:
	return (-EINVAL);
      efault:
	mi_strlog(tp->oq, 0, SL_ERROR, "%s: software error", __FUNCTION__);
	return (-EFAULT);
}

static t_scalar_t
tp_build_negotiate_options(struct tp *tp, uchar *ip, uint ilen, uchar *op, uint olen,
			   t_scalar_t flag)
{
	t_scalar_t overall = T_SUCCESS;
	const struct t_opthdr *ih;
	struct t_opthdr *oh, all;
	struct t_options *op = &tp->opts.req;
	int optlen;

	if (ilen == 0) {
		/* For zero-length options fake an option for all names within all levels. */
		all.level = T_ALLLEVELS;
		all.name = T_ALLOPT;
		all.len = sizeof(all);
		all.status = 0;
		ip = (unsigned char *) &all;
		ilen = sizeof(all);
	}
	for (ih = _T_OPT_FIRSTHDR_OFS(ip, ilen, 0), oh = _T_OPT_FIRSTHDR_OFS(op, *olen, 0);
	     ih && oh;
	     ih = _T_OPT_NEXTHDR_OFS(ip, ilen, ih, 0), oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)) {
		/* don't need to do this, it was done when we sized options */
		if (ih->len < sizeof(*ih))
			goto einval;
		if ((unsigned char *) ih + ih->len > ip + ilen)
			goto einval;
		optlen = ih->len - sizeof(*ih);
		switch (ih->level) {
		default:
			goto einval;
		case T_ALLLEVELS:
			if (ih->name != T_ALLOPT)
				goto einval;
		case XTI_GENERIC:
		{
			struct tp_xti *xti = &op->xti;

			switch (ih->name) {
			default:
				oh->len = ih->len;
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				continue;
			case T_ALLOPT:
			case XTI_DEBUG:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = XTI_GENERIC;
				oh->name = XTI_DEBUG;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = t_defaults.xti.debug[0];
				} else {
					bcopy(T_OPT_DATA(ih), valp, optlen);
				}
				if (!capable(CAP_NET_ADMIN)) {	/* XXX */
					oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				} else {
					bzero(xti->debug, sizeof(xti->debug));
					if (oh->len > sizeof(*oh))
						bcopy(valp, xti->debug, oh->len - sizeof(*oh));
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case XTI_LINGER:
			{
				struct t_linger *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = XTI_GENERIC;
				oh->name = XTI_LINGER;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = t_defaults.xti.linger;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
					if ((valp->l_onoff != T_NO && valp->l_onoff != T_YES)
					    || (valp->l_linger == T_UNSPEC
						&& valp->l_onoff != T_NO))
						goto einval;
					if (valp->l_linger == T_UNSPEC) {
						valp->l_linger = t_defaults.xti.linger.l_linger;
					}
					if (valp->l_linger == T_INFINITE) {
						valp->l_linger = MAX_SCHEDULE_TIMEOUT / HZ;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					if (valp->l_linger < 0)
						goto einval;
					if (valp->l_linger > MAX_SCHEDULE_TIMEOUT / HZ) {
						valp->l_linger = MAX_SCHEDULE_TIMEOUT / HZ;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				xti->linger = *valp;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case XTI_RCVBUF:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = XTI_GENERIC;
				oh->name = XTI_RCVBUF;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = sysctl_rmem_default;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
					if (*valp > sysctl_rmem_max) {
						*valp = sysctl_rmem_max;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					} else if (*valp < SOCK_MIN_RCVBUF / 2) {
						*valp = SOCK_MIN_RCVBUF / 2;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				xti->rcvbuf = *valp;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case XTI_RCVLOWAT:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = XTI_GENERIC;
				oh->name = XTI_RCVLOWAT;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = t_defaults.xti.rcvlowat;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
					if (*valp > INT_MAX) {
						*valp = INT_MAX;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					} else if (*valp <= 0) {
						*valp = 1;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				xti->rcvlowat = *valp;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case XTI_SNDBUF:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = XTI_GENERIC;
				oh->name = XTI_SNDBUF;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = sysctl_wmem_default;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
					if (*valp > sysctl_rmem_max) {
						*valp = sysctl_rmem_max;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					} else if (*valp < SOCK_MIN_SNDBUF / 2) {
						*valp = SOCK_MIN_SNDBUF / 2;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				xti->sndbuf = *valp;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case XTI_SNDLOWAT:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = XTI_GENERIC;
				oh->name = XTI_SNDLOWAT;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = t_defaults.xti.sndlowat;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
					if (*valp > 1) {
						*valp = 1;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					} else if (*valp <= 0) {
						*valp = 1;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				xti->sndlowat = *valp;
				if (ih->name != T_ALLOPT)
					continue;
			}
			}
			if (ih->level != T_ALLLEVELS)
				continue;
			if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
				goto efault;
		}
		case T_INET_IP:
		{
			struct tp_ip *ip = &op->ip;

			switch (ih->name) {
			default:
				oh->len = ih->len;
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				continue;
			case T_ALLOPT:
			case T_IP_OPTIONS:
			{
				/* not supported yet */
				oh->len = ih->len;
				oh->level = T_INET_IP;
				oh->name = T_IP_OPTIONS;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_IP_TOS:
			{
				unsigned char *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_IP;
				oh->name = T_IP_TOS;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = t_defaults.ip.tos;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
					{
						unsigned char prec = (*valp >> 5) & 0x7;
						unsigned char type =
						    *valp & (T_LDELAY | T_HITHRPT | T_HIREL |
							     T_LOCOST);

						if (*valp != SET_TOS(prec, type))
							goto einval;
						if (prec >= T_CRITIC_ECP && !capable(CAP_NET_ADMIN)) {
							oh->status =
							    t_overall_result(&overall,
									     T_NOTSUPPORT);
							break;
						}
					}
				}
				ip->tos = *valp;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_IP_TTL:
			{
				unsigned char *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_IP;
				oh->name = T_IP_TTL;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = t_defaults.ip.ttl;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
					if (*valp == 0) {
						*valp = sysctl_ip_default_ttl;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					if (*valp < 1) {
						*valp = 1;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
#if 0
					if (*valp > 255) {
						*valp = 255;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
#endif
				}
				ip->ttl = *valp;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_IP_REUSEADDR:
			{
				unsigned int *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_IP;
				oh->name = T_IP_REUSEADDR;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = t_defaults.ip.reuseaddr;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
					if (*valp != T_YES && *valp != T_NO)
						goto einval;
				}
				ip->reuseaddr = *valp;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_IP_DONTROUTE:
			{
				unsigned int *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_IP;
				oh->name = T_IP_DONTROUTE;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = t_defaults.ip.dontroute;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
					if (*valp != T_YES && *valp != T_NO)
						goto einval;
				}
				ip->dontroute = *valp;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_IP_BROADCAST:
			{
				unsigned int *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_IP;
				oh->name = T_IP_BROADCAST;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = t_defaults.ip.broadcast;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
					if (*valp != T_YES && *valp != T_NO)
						goto einval;
				}
				ip->broadcast = *valp;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_IP_ADDR:
			{
				uint32_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_IP;
				oh->name = T_IP_ADDR;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = t_defaults.ip.addr;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
				}
				ip->addr = *valp;
				if (ih->name != T_ALLOPT)
					continue;
			}
			}
			if (ih->level != T_ALLLEVELS)
				continue;
			if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
				goto efault;
		}
		case T_INET_UDP:
		{
			struct tp_udp *udp = &op->udp;

			switch (ih->name) {
			default:
				oh->len = ih->len;
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				continue;
			case T_ALLOPT:
			case T_UDP_CHECKSUM:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_UDP;
				oh->name = T_UDP_CHECKSUM;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = t_defaults.udp.checksum;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
					if (*valp != T_YES && *valp != T_NO)
						goto einval;
				}
				udp->checksum = *valp;
				if (ih->name != T_ALLOPT)
					continue;
			}
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		}
		case T_INET_TCP:
		{
			struct tp_tcp *tcp = &op->tcp;

			switch (ih->name) {
			default:
				oh->len = ih->len;
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				continue;
			case T_ALLOPT:
			case T_TCP_NODELAY:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_NODELAY;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = t_defaults.tcp.nodelay;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
					if (*valp != T_YES && *valp != T_NO)
						goto einval;
				}
				tcp->nodelay = *valp;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_TCP_MAXSEG:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_MAXSEG;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = t_defaults.tcp.maxseg;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
					if (*valp < 8) {
						*valp = 8;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					if (*valp > MAX_TCP_WINDOW) {
						*valp = MAX_TCP_WINDOW;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				tcp->maxseg = *valp;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_TCP_KEEPALIVE:
			{
				struct t_kpalive *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_KEEPALIVE;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = t_defaults.tcp.keepalive;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
					if (valp->kp_onoff != T_YES && valp->kp_onoff != T_NO)
						goto einval;
					if (valp->kp_timeout == T_UNSPEC) {
						valp->kp_timeout =
						    t_defaults.tcp.keepalive.kp_timeout;
					}
					if (valp->kp_timeout < 0)
						goto einval;
					if (valp->kp_timeout > MAX_SCHEDULE_TIMEOUT / 60 / HZ) {
						valp->kp_timeout = MAX_SCHEDULE_TIMEOUT / 60 / HZ;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					if (valp->kp_onoff == T_YES && valp->kp_timeout < 1) {
						valp->kp_timeout = 1;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				tcp->keepalive = *valp;
				if (valp->kp_onoff)
					tp->keepalive_time = valp->kp_timeout * 60 * HZ;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_TCP_CORK:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_CORK;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = t_defaults.tcp.cork;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
					/* negotiate value */
				}
				tcp->cork = *valp;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_TCP_KEEPIDLE:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_KEEPIDLE;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = t_defaults.tcp.keepidle;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
					/* negotiate value */
				}
				tcp->keepidle = *valp;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_TCP_KEEPINTVL:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_KEEPINTVL;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = t_defaults.tcp.keepitvl;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
					/* negotiate value */
				}
				tcp->keepitvl = *valp;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_TCP_KEEPCNT:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_KEEPCNT;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = t_defaults.tcp.keepcnt;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
					/* negotiate value */
				}
				tcp->keepcnt = *valp;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_TCP_SYNCNT:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_SYNCNT;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = t_defaults.tcp.syncnt;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
					/* negotiate value */
				}
				tcp->syncnt = *valp;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_TCP_LINGER2:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_LINGER2;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = t_defaults.tcp.linger2;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
					/* negotiate value */
				}
				tcp->linger2 = *valp;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_TCP_DEFER_ACCEPT:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_DEFER_ACCEPT;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = t_defaults.tcp.defer_accept;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
					/* negotiate value */
				}
				tcp->defer_accept = *valp;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_TCP_WINDOW_CLAMP:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_WINDOW_CLAMP;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = t_defaults.tcp.window_clamp;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
					/* negotiate value */
				}
				tcp->window_clamp = *valp;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_TCP_INFO:
			{
				struct t_tcp_info *valp = (typeof(valp)) T_OPT_DATA(oh);

				/* read-only */
				oh->len = ih->len;
				oh->level = T_INET_TCP;
				oh->name = T_TCP_INFO;
				oh->status = t_overall_result(&overall, T_READONLY);
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_TCP_QUICKACK:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_QUICKACK;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = t_defaults.tcp.quickack;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
					/* negotiate value */
				}
				tcp->quickack = *valp;
				if (ih->name != T_ALLOPT)
					continue;
			}
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		}
		case T_ISO_TP:
			if (tp->info.SERV_type == T_CLTS) {
				struct tp_tcl *tcl = &op->tcl;

				switch (ih->name) {
				default:
					oh->len = ih->len;
					oh->level = ih->level;
					oh->name = ih->name;
					oh->status = t_overall_result(&overall, T_NOTSUPPORT);
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					continue;
				case T_ALLOPT:
				case T_TCL_TRANSDEL:
				{
					struct rate *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_ISO_TP;
					oh->name = T_TCL_TRANSDEL;
					oh->state = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.tcl.transdel;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					tcl->transdel = *valp;
					if (ih->name != T_ALLOPT)
						continue;
				}
				case T_TCL_RESERRORRATE:
				{
					struct rate *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_ISO_TP;
					oh->name = T_TCL_RESERRORRATE;
					oh->state = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.tcl.reserrorrate;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					tcl->reserrorrate = *valp;
					if (ih->name != T_ALLOPT)
						continue;
				}
				case T_TCL_PROTECTION:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_ISO_TP;
					oh->name = T_TCL_PROTECTION;
					oh->state = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.tcl.protection;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					tcl->protection = *valp;
					if (ih->name != T_ALLOPT)
						continue;
				}
				case T_TCL_PRIORITY:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_ISO_TP;
					oh->name = T_TCL_PRIORITY;
					oh->state = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.tcl.priority;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					tcl->priority = *valp;
					if (ih->name != T_ALLOPT)
						continue;
				}
				case T_TCL_CHECKSUM:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_ISO_TP;
					oh->name = T_TCL_CHECKSUM;
					oh->state = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.tcl.checksum;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					tcl->checksum = *valp;
					if (ih->name != T_ALLOPT)
						continue;
				}
				}
			} else {
				struct tp_tco *tco = &op->tco;

				switch (ih->name) {
				default:
					oh->len = ih->len;
					oh->level = ih->level;
					oh->name = ih->name;
					oh->status = t_overall_result(&overall, T_NOTSUPPORT);
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					continue;
				case T_ALLOPT:
				case T_TCO_THROUGHPUT:
				{
					struct thrpt *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_THROUGHPUT;
					oh->state = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.tco.throughput;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					tco->throughput = *valp;
					if (ih->name != T_ALLOPT)
						continue;
				}
				case T_TCO_TRANSDEL:
				{
					struct transdel *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_TRANSDEL;
					oh->state = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.tco.transdel;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					tco->transdel = *valp;
					if (ih->name != T_ALLOPT)
						continue;
				}
				case T_TCO_RESERRORRATE:
				{
					struct rate *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_RESERRORRATE;
					oh->state = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.tco.reserrorrate;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					tco->reserrorrate = *valp;
					if (ih->name != T_ALLOPT)
						continue;
				}
				case T_TCO_TRANSFFAILPROB:
				{
					struct rate *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_TRANSFFAILPROB;
					oh->state = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.tco.transffailprob;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					tco->transffailprob = *valp;
					if (ih->name != T_ALLOPT)
						continue;
				}
				case T_TCO_ESTFAILPROB:
				{
					struct rate *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_ESTFAILPROB;
					oh->state = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.tco.estfailprob;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					tco->estfailprob = *valp;
					if (ih->name != T_ALLOPT)
						continue;
				}
				case T_TCO_RELFAILPROB:
				{
					struct rate *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_RELFAILPROB;
					oh->state = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.tco.relfailprob;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					tco->relfailprob = *valp;
					if (ih->name != T_ALLOPT)
						continue;
				}
				case T_TCO_ESTDELAY:
				{
					struct rate *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_ESTDELAY;
					oh->state = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.tco.estdelay;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					tco->estdelay = *valp;
					if (ih->name != T_ALLOPT)
						continue;
				}
				case T_TCO_RELDELAY:
				{
					struct rate *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_RELDELAY;
					oh->state = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.tco.reldelay;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					tco->reldelay = *valp;
					if (ih->name != T_ALLOPT)
						continue;
				}
				case T_TCO_CONNRESIL:
				{
					struct rate *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_CONNRESIL;
					oh->state = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.tco.connresil;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					tco->connresil = *valp;
					if (ih->name != T_ALLOPT)
						continue;
				}
				case T_TCO_PROTECTION:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_PROTECTION;
					oh->state = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.tco.protection;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					tco->protection = *valp;
					if (ih->name != T_ALLOPT)
						continue;
				}
				case T_TCO_PRIORITY:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_PRIORITY;
					oh->state = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.tco.priority;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					tco->priority = *valp;
					if (ih->name != T_ALLOPT)
						continue;
				}
				case T_TCO_EXPD:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_EXPD;
					oh->state = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.tco.expd;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					tco->expd = *valp;
					if (ih->name != T_ALLOPT)
						continue;
				}
				case T_TCO_LTPDU:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_LTPDU;
					oh->state = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.tco.ltpdu;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					tco->ltpdu = *valp;
					if (ih->name != T_ALLOPT)
						continue;
				}
				case T_TCO_ACKTIME:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_ACKTIME;
					oh->state = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.tco.acktime;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					tco->acktime = *valp;
					if (ih->name != T_ALLOPT)
						continue;
				}
				case T_TCO_REASTIME:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_REASTIME;
					oh->state = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.tco.reastime;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					tco->reastime = *valp;
					if (ih->name != T_ALLOPT)
						continue;
				}
				case T_TCO_EXTFORM:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_EXTFORM;
					oh->state = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.tco.extform;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					tco->extform = *valp;
					if (ih->name != T_ALLOPT)
						continue;
				}
				case T_TCO_FLOWCTRL:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_FLOWCTRL;
					oh->state = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.tco.flowctrl;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					tco->flowctrl = *valp;
					if (ih->name != T_ALLOPT)
						continue;
				}
				case T_TCO_CHECKSUM:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_CHECKSUM;
					oh->state = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.tco.checksum;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					tco->checksum = *valp;
					if (ih->name != T_ALLOPT)
						continue;
				}
				case T_TCO_NETEXP:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_NETEXP;
					oh->state = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.tco.netexp;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					tco->netexp = *valp;
					if (ih->name != T_ALLOPT)
						continue;
				}
				case T_TCO_NETRECPTCF:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_NETRECPTCF;
					oh->state = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.tco.netrecptcf;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					tco->netrecptcf = *valp;
					if (ih->name != T_ALLOPT)
						continue;
				}
				case T_TCO_PREFCLASS:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_PREFCLASS;
					oh->state = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.tco.prefclass;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					tco->prefclass = *valp;
					if (ih->name != T_ALLOPT)
						continue;
				}
				case T_TCO_ALTCLASS1:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_ALTCLASS1;
					oh->state = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.tco.altclass1;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					tco->altclass1 = *valp;
					if (ih->name != T_ALLOPT)
						continue;
				}
				case T_TCO_ALTCLASS2:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_ALTCLASS2;
					oh->state = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.tco.altclass2;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					tco->altclass2 = *valp;
					if (ih->name != T_ALLOPT)
						continue;
				}
				case T_TCO_ALTCLASS3:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_ALTCLASS3;
					oh->state = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.tco.altclass3;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					tco->altclass3 = *valp;
					if (ih->name != T_ALLOPT)
						continue;
				}
				case T_TCO_ALTCLASS4:
				{
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					oh->len = _T_LENGTH_SIZEOF(*valp);
					oh->level = T_ISO_TP;
					oh->name = T_TCO_ALTCLASS4;
					oh->state = T_SUCCESS;
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.tco.altclass4;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					tco->altclass4 = *valp;
					if (ih->name != T_ALLOPT)
						continue;
				}
				}
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		}
	}
	if (ih && !oh)
		goto efault;
	if (oh)
		*olen = (unsigned char *) oh - op;
	return (overall);
      einval:
	return (-EINVAL);
      efault:
	mi_strlog(tp->oq, 0, SL_ERROR, "%s: software error", __FUNCTION__);
	return (-EFAULT);
}

static t_scalar_t
tp_build_options(struct tp *tp, unsigned char *ip, size_t ilen, unsigned char *op, size_t *olen,
		 t_scalar_t flag)
{
	switch (flag) {
	case T_DEFAULT:
		return tp_build_default_options(tp, ip, ilen, op, olen);
	case T_CURRENT:
		return tp_build_current_options(tp, ip, ilen, op, olen);
	case T_CHECK:
		return tp_build_check_options(tp, ip, ilen, op, olen);
	case T_NEGOTIATE:
		return tp_build_negotiate_options(tp, ip, ilen, op, olen);
	}
	return (-EINVAL);
}

/*
 * TP-User Primitives send downstream (to TCP or UDP Provider)
 */

/**
 * np_conn_req: - issue T_CONN_REQ downstream
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
np_conn_req(struct np *np, queue_t *q, mblk_t *msg, caddr_t dst_ptr, size_t dst_len,
	    caddr_t opt_ptr, size_t opt_len, mblk_t *dp)
{
	struct T_conn_req *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p) + dst_len + opt_len, BPRI_MED)) != NULL) {
		if (bcanputnext(np->oq, 0)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_CONN_REQ;
			p->DEST_length = dst_len;
			p->DEST_offset = sizeof(*p);
			p->OPT_length = opt_len;
			p->OPT_offset = sizeof(*p) + dst_len;
			mp->b_wptr += sizeof(*p);
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "T_CONN_REQ ->");
			np_set_state(np, TS_WACK_CREQ);
			putnext(np->oq, mp);
			return (0);
		} else {
			freeb(mp);
			return (-EBUSY);
		}
	}
	return (-ENOBUFS);
}

/**
 * np_conn_res: - issue T_CONN_RES downstream
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
np_conn_res(struct np *np, queue_t *q, mblk_t *msg, t_uscalar_t tok, t_scalar_t seq,
	    caddr_t opt_ptr, size_t opt_len, mblk_t *dp)
{
	struct T_conn_res *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p) + opt_len, BPRI_MED)) != NULL) {
		if (bcanputnext(np->oq, 0)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_CONN_RES;
			p->ACCEPTOR_id = tok;
			p->OPT_length = opt_len;
			p->OPT_offset = sizeof(*p);
			p->SEQ_number = seq;
			mp->b_wptr += sizeof(*p);
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "T_CONN_RES ->");
			np_set_state(np, TS_WACK_CRES);
			putnext(np->oq, mp);
			return (0);
		} else {
			freeb(mp);
			return (-EBUSY);
		}
	}
	return (-ENOBUFS);
}

/**
 * np_discon_req: - issue T_DISCON_REQ downstream
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
np_discon_req(struct np *np, queue_t *q, mblk_t *msg, t_scalar_t seq, mblk_t *dp)
{
	struct T_discon_req *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED)) != NULL) {
		if (bcanputnext(np->oq, 0)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_DISCON_REQ;
			p->SEQ_number = seq;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "T_DISCON_REQ ->");
			switch (np_get_state(np)) {
			case TS_WCON_CREQ:
				np_set_state(np, TS_WACK_DREQ6);
				break;
			case TS_WRES_CIND:
				np_set_state(np, TS_WACK_DREQ7);
				break;
			case TS_DATA_XFER:
				np_set_state(np, TS_WACK_DREQ9);
				break;
			case TS_WIND_ORDREL:
				np_set_state(np, TS_WACK_DREQ10);
				break;
			case TS_WREQ_ORDREL:
				np_set_state(np, TS_WACK_DREQ11);
				break;
			default:
				mi_strlog(q, 0, SL_ERROR, "%s: SWERR wrong state", __FUNCTION__);
				break;
			}
			putnext(np->oq, mp);
			return (0);
		} else {
			freeb(mp);
			return (-EBUSY);
		}
	}
	return (-ENOBUFS);
}

/**
 * np_data_req: - issue T_DATA_REQ downstream
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall inline __hot_out int
np_data_req(struct np *np, queue_t *q, mblk_t *msg, t_scalar_t more, mblk_t *dp)
{
	struct T_data_req *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED)) != NULL) {
		if (bcanputnext(np->oq, 0)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_DATA_REQ;
			p->MORE_flag = more;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "T_DATA_REQ ->");
			putnext(np->oq, mp);
			return (0);
		} else {
			freeb(mp);
			return (-EBUSY);
		}
	}
	return (-ENOBUFS);
}

/**
 * np_exdata_req: - issue T_EXDATA_REQ downstream
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall inline __hot_out int
np_exdata_req(struct np *np, queue_t *q, mblk_t *msg, t_scalar_t more, mblk_t *dp)
{
	struct T_exdata_req *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED)) != NULL) {
		if (bcanputnext(np->oq, 1)) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_EXDATA_REQ;
			p->MORE_flag = more;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "T_EXDATA_REQ ->");
			putnext(np->oq, mp);
			return (0);
		} else {
			freeb(mp);
			return (-EBUSY);
		}
	}
	return (-ENOBUFS);
}

/**
 * np_info_req: - issue T_INFO_REQ downstream
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
np_info_req(struct np *np, queue_t *q, mblk_t *msg)
{
	struct T_info_req *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED)) != NULL) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_INFO_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "T_INFO_REQ ->");
		putnext(np->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * np_bind_req: - issue T_BIND_REQ downstream
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
np_bind_req(struct np *np, queue_t *q, mblk_t *msg, caddr_t add_ptr, size_t add_len,
	    t_uscalar_t conind)
{
	struct T_bind_req *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p) + add_len, BPRI_MED)) != NULL) {
		if (bcanputnext(np->oq, 0)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_BIND_REQ;
			p->ADDR_length = add_len;
			p->ADDR_offset = sizeof(*p);
			p->CONIND_number = conind;
			mp->b_wptr += sizeof(*p);
			bcopy(add_ptr, mp->b_wptr, add_len);
			mp->b_wptr += add_len;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "T_BIND_REQ ->");
			np_set_state(np, TS_WACK_BREQ);
			putnext(np->oq, mp);
			return (0);
		} else {
			freeb(mp);
			return (-EBUSY);
		}
	}
	return (-ENOBUFS);
}

/**
 * np_unbind_req: - issue T_UNBIND_REQ downstream
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
np_unbind_req(struct np *np, queue_t *q, mblk_t *msg)
{
	struct T_unbind_req *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED)) != NULL) {
		if (bcanputnext(np->oq, 0)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_UNBIND_REQ;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "T_UNBIND_REQ ->");
			np_set_state(np, TS_WACK_UREQ);
			putnext(np->oq, mp);
			return (0);
		} else {
			freeb(mp);
			return (-EBUSY);
		}
	}
	return (-ENOBUFS);
}

/**
 * np_unitdata_req: - issue T_UNITDATA_REQ downstream
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall inline __hot_out int
np_unitdata_req(struct np *np, queue_t *q, mblk_t *msg, caddr_t dst_ptr, size_t dst_len,
		caddr_t opt_ptr, size_t opt_len, mblk_t *dp)
{
	struct T_unitdata_req *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p) + dst_len + opt_len, BPRI_MED)) != NULL) {
		if (bcanputnext(np->oq, 0)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_UNITDATA_REQ;
			p->DEST_length = dst_len;
			p->DEST_offset = sizeof(*p);
			p->OPT_length = opt_len;
			p->OPT_offset = sizeof(*p) + dst_len;
			mp->b_wptr += sizeof(*p);
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "T_UNITDATA_REQ ->");
			putnext(np->oq, mp);
			return (0);
		} else {
			freeb(mp);
			return (-EBUSY);
		}
	}
	return (-ENOBUFS);
}

/**
 * np_optmgmt_req: - issue T_OPTMGMT_REQ downstream
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
np_optmgmt_req(struct np *np, queue_t *q, mblk_t *msg, caddr_t opt_ptr, size_t opt_len,
	       t_scalar_t flags)
{
	struct T_optmgmt_req *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p) + opt_len, BPRI_MED)) != NULL) {
		if (bcanputnext(np->oq, 0)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_OPTMGMT_REQ;
			p->OPT_length = opt_len;
			p->OPT_offset = sizeof(*p);
			p->MGMT_flags = flags;
			mp->b_wptr += sizeof(*p);
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "T_OPTMGMT_REQ ->");
			if (np_get_state(np) == TS_IDLE)
				np_set_state(np, TS_WACK_OPTREQ);
			putnext(np->oq, mp);
			return (0);
		} else {
			freeb(mp);
			return (-EBUSY);
		}
	}
	return (-ENOBUFS);
}

/**
 * np_ordrel_req: - issue T_ORDREL_REQ downstream
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
np_ordrel_req(struct np *np, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	struct T_ordrel_req *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED)) != NULL) {
		if (bcanputnext(np->oq, 0)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_ORDREL_REQ;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "T_ORDREL_REQ ->");
			switch (np_get_state(np)) {
			case TS_DATA_XFER:
				np_set_state(np, TS_WIND_ORDREL);
				break;
			case TS_WIND_ORDREL:
				np_set_state(np, TS_IDLE);
				break;
			default:
				mi_strlog(q, 0, SL_ERROR, "%s: SWERR wrong state", __FUNCTION__);
				break;
			}
			putnext(np->oq, mp);
			return (0);
		} else {
			freeb(mp);
			return (-EBUSY);
		}
	}
	return (-ENOBUFS);
}

/**
 * np_optdata_req: - issue T_OPTDATA_REQ downstream
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall inline __hot_out int
np_optdata_req(struct np *np, queue_t *q, mblk_t *msg, t_scalar_t flags, caddr_t opt_ptr,
	       size_t opt_len, mblk_t *dp)
{
	struct T_optdata_req *p;
	mblk_t *mp;
	unsigned char band;

	if ((mp = mi_allocb(q, sizeof(*p) + opt_len, BPRI_MED)) != NULL) {
		band = (flags & T_ODF_EX) ? 1 : 0;
		if (bcanputnext(np->oq, band)) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = band;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_OPTDATA_REQ;
			p->DATA_flag = flags;
			p->OPT_length = opt_len;
			p->OPT_offset = sizeof(*p);
			mp->b_wptr += sizeof(*p);
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "T_OPTDATA_REQ ->");
			putnext(np->oq, mp);
			return (0);
		} else {
			freeb(mp);
			return (-EBUSY);
		}
	}
	return (-ENOBUFS);
}

/**
 * np_addr_req: - issue T_ADDR_REQ downstream
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
np_addr_req(struct np *np, queue_t *q, mblk_t *msg)
{
	struct T_addr_req *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED)) != NULL) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_ADDR_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "T_ADDR_REQ ->");
		putnext(np->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * np_capability_req: - issue T_CAPABILITY_REQ downstream
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
np_capability_req(struct np *np, queue_t *q, mblk_t *msg)
{
	struct T_capablity_req *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED)) != NULL) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_CAPABILITY_REQ;
		p->CAP_bits1 = TC1_INFO | TC1_ACCEPTOR_ID;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "T_CAPABILITY_REQ ->");
		putnext(np->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 * TP-Provider Primitives sent upstream (to ISO TP User)
 */

/**
 * tp_m_error: issue an M_ERROR or M_HANGUP upstream
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: message to free upon success
 * @error: the error
 * @func: calling function
 */
static int
tp_m_error(struct tp *tp, queue_t *q, mblk_t *msg, int error, char *func)
{
	mblk_t *mp;
	int hangup = 0;
	t_scalar_t tperr = 0;

	if (error > 0) {
		tperr = error;
		error = -EPROTO;
	}
	if (error < 0) {
		tperr = TSYSERR;
		error = -error;
	}
	switch (error) {
	case EBUSY:
	case ENOBUFS:
	case EAGAIN:
	case ENOMEM:
	case EDEADLK:
		return (-error);
	case EPIPE:
	case ENETDOWN:
	case EHOSTUNREACH:
	case ECONNRESET:
	case ENXIO:
		hangup = 1;
	}
	if ((mp = mi_allocb(q, 2, BPRI_HI))) {
		if (hangup) {
			DB_TYPE(mp) = M_HANGUP;
		} else {
			DB_TYPE(mp) = M_ERROR;
			*mp->b_wptr++ = error;
			*mp->b_wptr++ = error;
		}
		freemsg(msg);
		mi_strlog(q, STRLOGRX, SL_TRACE, "%s: <- M_ERROR", func);
		mi_strlog(q, 0, SL_ERROR, "%s: %s", func, tpi_strerr(tperr));
		putnext(tp->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * tp_conn_ind: - issued T_CONN_IND upstream
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: mssages to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
tp_conn_ind(struct tp *tp, queue_t *q, mblk_t *msg, caddr_t src_ptr, size_t src_len,
	    caddr_t opt_ptr, size_t opt_len, t_scalar_t seq, mblk_t *dp)
{
	struct T_conn_ind *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p) + src_len + opt_len, BPRI_MED)) != NULL) {
		if (bcanputnext(tp->oq, 0)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_CONN_IND;
			p->SRC_length = src_len;
			p->SRC_offset = sizeof(*p);
			p->OPT_length = opt_len;
			p->OPT_offset = sizeof(*p) + src_len;
			p->SEQ_number = seq;
			mp->b_wptr += sizeof(*p);
			bcopy(src_ptr, mp->b_wptr, src_len);
			mp->b_wptr += src_len;
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_CONN_IND");
			tp_set_state(tp, TS_WRES_CIND);
			putnext(tp->oq, mp);
			return (0);
		} else {
			freeb(mp);
			return (-EBUSY);
		}
	}
	return (-ENOBUFS);
}

/**
 * tp_conn_con: - issued T_CONN_CON upstream
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: mssages to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
tp_conn_con(struct tp *tp, queue_t *q, mblk_t *msg, caddr_t res_ptr, size_t res_len,
	    caddr_t opt_ptr, size_t opt_len, mblk_t *dp)
{
	struct T_conn_con *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p) + res_len + opt_len, BPRI_MED)) != NULL) {
		if (bcanputnext(tp->oq, 0)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_CONN_CON;
			p->RES_length = res_len;
			p->RES_offset = sizeof(*p);
			p->OPT_length = opt_len;
			p->OPT_offset = sizeof(*p) + res_len;
			mp->b_wptr += sizeof(*p);
			bcopy(res_ptr, mp->b_wptr, res_len);
			mp->b_wptr += res_len;
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_CONN_CON");
			tp_set_state(tp, TS_DATA_XFER);
			putnext(tp->oq, mp);
			return (0);
		} else {
			freeb(mp);
			return (-EBUSY);
		}
	}
	return (-ENOBUFS);
}

/**
 * tp_discon_ind: - issued T_DISCON_IND upstream
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: mssages to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
tp_discon_ind(struct tp *tp, queue_t *q, mblk_t *msg, t_scalar_t reason, t_scalar_t seq, mblk_t *dp)
{
	struct T_discon_ind *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED)) != NULL) {
		if (bcanputnext(tp->oq, 0)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_DISCON_IND;
			p->DISCON_reason = reason;
			p->SEQ_number = seq;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_DISCON_IND");
			tp_set_state(tp, TS_IDLE);
			putnext(tp->oq, mp);
			return (0);
		} else {
			freeb(mp);
			return (-EBUSY);
		}
	}
	return (-ENOBUFS);
}

/**
 * tp_data_ind: - issued T_DATA_IND upstream
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: mssages to free upon success (or NULL)
 */
static fastcall inline __hot_get int
tp_data_ind(struct tp *tp, queue_t *q, mblk_t *msg, t_scalar_t more, mblk_t *dp)
{
	struct T_data_ind *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED)) != NULL) {
		if (bcanputnext(tp->oq, 0)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_DATA_IND;
			p->MORE_flag = more;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_DATA_IND");
			putnext(tp->oq, mp);
			return (0);
		} else {
			freeb(mp);
			return (-EBUSY);
		}
	}
	return (-ENOBUFS);
}

/**
 * tp_exdata_ind: - issued T_EXDATA_IND upstream
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: mssages to free upon success (or NULL)
 */
static fastcall inline __hot_get int
tp_exdata_ind(struct tp *tp, queue_t *q, mblk_t *msg, t_scalar_t more, mblk_t *dp)
{
	struct T_exdata_ind *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED)) != NULL) {
		if (bcanputnext(tp->oq, 1)) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_EXDATA_IND;
			p->MORE_flag = more;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_EXDATA_IND");
			putnext(tp->oq, mp);
			return (0);
		} else {
			freeb(mp);
			return (-EBUSY);
		}
	}
	return (-ENOBUFS);
}

/**
 * tp_info_ack: - issued T_INFO_ACK upstream
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: mssages to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
tp_info_ack(struct tp *tp, queue_t *q, mblk_t *msg)
{
	struct T_info_ack *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED)) != NULL) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_INFO_ACK;
		p->TSDU_size = tp->info.TSDU_size;
		p->ETSDU_size = tp->info.ETSDU_size;
		p->CDATA_size = tp->info.CDATA_size;
		p->DDATA_size = tp->info.DDATA_size;
		p->ADDR_size = tp->info.ADDR_size;
		p->OPT_size = tp->info.OPT_size;
		p->TIDU_size = tp->info.TIDU_size;
		p->SERV_type = tp->info.SERV_type;
		p->CURRENT_state = tp->info.CURRENT_state;
		p->PROVIDER_flag = tp->info.PROVIDER_flag;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_INFO_ACK");
		putnext(tp->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * tp_bind_ack: - issued T_BIND_ACK upstream
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: mssages to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
tp_bind_ack(struct tp *tp, queue_t *q, mblk_t *msg, caddr_t add_ptr, size_t add_len,
	    t_uscalar_t conind)
{
	struct T_bind_ack *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p) + add_len, BPRI_MED)) != NULL) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_BIND_ACK;
		p->ADDR_length = add_len;
		p->ADDR_offset = sizeof(*p);
		p->CONIND_number = conind;
		mp->b_wptr += sizeof(*p);
		bcopy(add_ptr, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_BIND_ACK");
		tp_set_state(tp, TS_IDLE);
		putnext(tp->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * tp_error_ack: - issued T_ERROR_ACK upstream
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: mssages to free upon success (or NULL)
 * @prim: primitive in error
 * @error: negative system error, positive TPI error
 * @func: calling function
 */
static fastcall noinline __unlikely int
tp_error_ack(struct tp *tp, queue_t *q, mblk_t *msg, t_scalar_t prim, t_scalar_t error, char *func)
{
	struct T_error_ack *p;
	t_scalar_t tperr = 0;
	mblk_t *mp;

	if (error > 0) {
		tperr = error;
		error = -EPROTO;
	}
	if (error < 0) {
		tperr = TSYSERR;
		error = -error;
	}
	switch (error) {
	case EBUSY:
	case ENOBUFS:
	case EAGAIN:
	case ENOMEM:
	case EDEADLK:
		return (-error);
	case EPIPE:
	case ENETDOWN:
	case EHOSTUNREACH:
	case ECONNRESET:
	case ENXIO:
		return tp_m_error(tp, q, msg, ENXIO, func);
	}
	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED)) != NULL) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_ERROR_ACK;
		p->ERROR_prim = prim;
		p->TLI_error = tperr;
		p->UNIX_error = error;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_ERROR_ACK");
		mi_strlog(q, 0, SL_ERROR, "%s: %s", func, tpi_strerr(tperr));
		switch (tp_get_state(tp)) {
		case TS_WACK_BREQ:
			tp_set_state(tp, TS_UNBND);
			break;
		case TS_WACK_CREQ:
			tp_set_state(tp, TS_IDLE);
			break;
		case TS_WACK_CRES:
			tp_set_state(tp, TS_WRES_CIND);
			break;
		case TS_WACK_DREQ6:
			tp_set_state(tp, TS_WCON_CREQ);
			break;
		case TS_WACK_DREQ7:
			tp_set_state(tp, TS_WRES_CIND);
			break;
		case TS_WACK_DREQ9:
			tp_set_state(tp, TS_DATA_XFER);
			break;
		case TS_WACK_DREQ10:
			tp_set_state(tp, TS_WIND_ORDREL);
			break;
		case TS_WACK_DREQ11:
			tp_set_state(tp, TS_WREQ_ORDREL);
			break;
		case TS_WACK_UREQ:
			tp_set_state(tp, TS_IDLE);
			break;
		case TS_WACK_OPTREQ:
			tp_set_state(tp, TS_IDLE);
			break;
		default:
			/* not an error */
			break;
		}
		putnext(tp->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * tp_ok_ack: - issued T_OK_ACK upstream
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: mssages to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
tp_ok_ack(struct tp *tp, queue_t *q, mblk_t *msg, t_scalar_t prim, bool other)
{
	struct T_ok_ack *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED)) != NULL) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_OK_ACK;
		p->CORRECT_prim = prim;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_OK_ACK");
		switch (tp_get_state(tp)) {
		case TS_WACK_BREQ:
			break;
		case TS_WACK_CREQ:
			tp_set_state(tp, TS_WCON_CREQ);
			break;
		case TS_WACK_CRES:
			tp->coninds--;
			if (other) {
				if (tp->coninds)
					tp_set_state(tp, TS_WRES_CIND);
				else
					tp_set_state(tp, TS_IDLE);
			} else {
				tp_set_state(tp, TS_DATA_XFER);
			}
			break;
		case TS_WACK_DREQ6:
		case TS_WACK_DREQ7:
		case TS_WACK_DREQ9:
		case TS_WACK_DREQ10:
		case TS_WACK_DREQ11:
			if (tp->coninds)
				tp_set_state(tp, TS_WRES_CIND);
			else
				tp_set_state(tp, TS_IDLE);
			break;
		case TS_WACK_UREQ:
			tp_set_state(tp, TS_UNBND);
			break;
		case TS_WACK_OPTREQ:
			break;
		default:
			/* not an error */
			break;
		}
		putnext(tp->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * tp_unitdata_ind: - issued T_UNITDATA_IND upstream
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: mssages to free upon success (or NULL)
 */
static fastcall inline __hot_get int
tp_unitdata_ind(struct tp *tp, queue_t *q, mblk_t *msg, caddr_t src_ptr, size_t src_len,
		caddr_t opt_ptr, size_t opt_len, mblk_t *dp)
{
	struct T_unitdata_ind *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p) + src_len + opt_len, BPRI_MED)) != NULL) {
		if (bcanputnext(tp->oq, 0)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_UNITDATA_IND;
			p->SRC_length = src_len;
			p->SRC_offset = sizeof(*p);
			p->OPT_length = opt_len;
			p->OPT_offset = sizeof(*p) + src_len;
			mp->b_wptr += sizeof(*p);
			bcopy(src_ptr, mp->b_wptr, src_len);
			mp->b_wptr += src_len;
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_UNITDATA_IND");
			putnext(tp->oq, mp);
			return (0);
		} else {
			freeb(mp);
			return (-EBUSY);
		}
	}
	return (-ENOBUFS);
}

/**
 * tp_uderror_ind: - issued T_UDERROR_IND upstream
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: mssages to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
tp_uderror_ind(struct tp *tp, queue_t *q, mblk_t *msg, caddr_t dst_ptr, size_t dst_len,
	       caddr_t opt_ptr, size_t opt_len, t_scalar_t etype, mblk_t *dp)
{
	struct T_uderror_ind *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p) + dst_len + opt_len, BPRI_MED)) != NULL) {
		if (bcanputnext(tp->oq, 1)) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_UDERROR_IND;
			p->DEST_length = dst_len;
			p->DEST_offset = sizeof(*p);
			p->OPT_length = opt_len;
			p->OPT_offset = sizeof(*p) + dst_len;
			p->ERROR_type = etype;
			mp->b_wptr += sizeof(*p);
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_UDERROR_IND");
			putnext(tp->oq, mp);
			return (0);
		} else {
			freeb(mp);
			return (-EBUSY);
		}
	}
	return (-ENOBUFS);
}

/**
 * tp_optmgmt_ack: - issued T_OPTMGMT_ACK upstream
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: mssages to free upon success (or NULL)
 * @req_ptr: options in the request pointer
 * @req_len: options in the request length
 * @flags: management flags
 */
static fastcall noinline __unlikely int
tp_optmgmt_ack(struct tp *tp, queue_t *q, mblk_t *msg, uchar *req_ptr, int req_len,
	       t_scalar_t flags)
{
	struct T_optmgmt_ack *p;
	mblk_t *mp;
	int opt_len;

	if ((opt_len = tp_size_options(tp, req_ptr, req_len, flags)) < 0)
		return (opt_len);

	if ((mp = mi_allocb(q, sizeof(*p) + opt_len, BPRI_MED)) != NULL) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		if ((flags =
		     tp_build_options(tp, req_ptr, req_len, mp->b_wptr, &opt_len, flags)) < 0) {
			freeb(mp);
			return (flags);
		}
		p->PRIM_type = T_OPTMGMT_ACK;
		p->OPT_length = opt_len;
		p->OPT_offset = sizeof(*p);
		p->MGMT_flags = flags;
		mp->b_wptr += opt_len;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_OPTMGMT_ACK");
		if (tp_get_state(tp) == TS_WACK_OPTREQ)
			tp_set_state(tp, TS_IDLE);
		putnext(tp->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * tp_ordrel_ind: - issued T_ORDREL_IND upstream
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: mssages to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
tp_ordrel_ind(struct tp *tp, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	struct T_ordrel_ind *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED)) != NULL) {
		if (bcanputnext(tp->oq, 0)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_ORDREL_IND;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_ORDREL_IND");
			switch (tp_get_state(tp)) {
			case TS_DATA_XFER:
				tp_set_state(tp, TS_WREQ_ORDREL);
				break;
			case TS_WIND_ORDREL:
				tp_set_state(tp, TS_IDLE);
				break;
			}
			putnext(tp->oq, mp);
			return (0);
		} else {
			freeb(mp);
			return (-EBUSY);
		}
	}
	return (-ENOBUFS);
}

/**
 * tp_optdata_ind: - issued T_OPTDATA_IND upstream
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: mssages to free upon success (or NULL)
 */
static fastcall inline __hot_get int
tp_optdata_ind(struct tp *tp, queue_t *q, mblk_t *msg, t_scalar_t flags, caddr_t opt_ptr,
	       size_t opt_len, mblk_t *dp)
{
	struct T_optdata_ind *p;
	mblk_t *mp;
	unsigned char band;

	if ((mp = mi_allocb(q, sizeof(*p) + opt_len, BPRI_MED)) != NULL) {
		band = (flags & T_ODF_EX) ? 1 : 0;
		if (bcanputnext(tp->oq, band)) {
			mp->b_band = band;
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_OPTDATA_IND;
			p->DATA_flag = flags;
			p->OPT_length = opt_len;
			p->OPT_offset = sizeof(*p);
			mp->b_wptr += sizeof(*p);
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_OPTDATA_IND");
			putnext(tp->oq, mp);
			return (0);
		} else {
			freeb(mp);
			return (-EBUSY);
		}
	}
	return (-ENOBUFS);
}

/**
 * tp_addr_ack: - issued T_ADDR_ACK upstream
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: mssages to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
tp_addr_ack(struct tp *tp, queue_t *q, mblk_t *msg, caddr_t loc_ptr, size_t loc_len,
	    caddr_t rem_ptr, size_t rem_len)
{
	struct T_addr_ack *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p) + loc_len + rem_len, BPRI_MED)) != NULL) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_ADDR_ACK;
		p->LOCADDR_length = loc_len;
		p->LOCADDR_offset = sizeof(*p);
		p->REMADDR_length = rem_len;
		p->REMADDR_offset = sizeof(*p) + src_len;
		mp->b_wptr += sizeof(*p);
		bcopy(loc_ptr, mp->b_wptr, loc_len);
		mp->b_wptr += loc_len;
		bcopy(rem_ptr, mp->b_wptr, rem_len);
		mp->b_wptr += rem_len;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_ADDR_ACK");
		putnext(tp->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * tp_capability_ack: - issued T_CAPABILITY_ACK upstream
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: mssages to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
tp_capability_ack(struct tp *tp, queue_t *q, mblk_t *msg)
{
	struct T_capability_ack *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED)) != NULL) {
		if (pcmsg(msg) || bcanputnext(tp->oq, 0)) {
			DB_TYPE(mp) = DB_TYPE(msg);
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_CAPABILITY_ACK;
			p->CAP_bits1 = TC1_INFO | TC1_ACCEPTOR_ID;
			p->INFO_ack.TSDU_size = tp->info.TSDU_size;
			p->INFO_ack.ETSDU_size = tp->info.ETSDU_size;
			p->INFO_ack.CDATA_size = tp->info.CDATA_size;
			p->INFO_ack.DDATA_size = tp->info.DDATA_size;
			p->INFO_ack.ADDR_size = tp->info.ADDR_size;
			p->INFO_ack.OPT_size = tp->info.OPT_size;
			p->INFO_ack.TIDU_size = tp->info.TIDU_size;
			p->INFO_ack.SERV_type = tp->info.SERV_type;
			p->INFO_ack.CURRENT_state = tp->info.CURRENT_state;
			p->INFO_ack.PROVIDER_flag = tp->info.PROVIDER_flag;
			p->ACCEPTOR_id = (t_uscalar_t) (long) (caddr_t) (tp->oq);
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_CAPABILITY_ACK");
			putnext(tp->oq, mp);
			return (0);
		} else {
			freeb(mp);
			return (-EBUSY);
		}
	}
	return (-ENOBUFS);
}

/*
 * ==========================================================================
 */

static uint32_t
t_unpack_n(uchar **p, uint *len)
{
	register uint32_t value = 0;

	while ((*len)-- > 0) {
		value <<= 8;
		value |= **p;
		(*p)++;
	}
	return (value);
}

static uint32_t
t_unpack_2(uchar **p)
{
	uint len = 2;

	return t_unpack_n(p, &len);
}
static uint32_t
t_unpack_3(uchar **p)
{
	uint len = 3;

	return t_unpack_n(p, &len);
}
static uint32_t
t_unpack_4(uchar **p)
{
	uint len = 4;

	return t_unpack_n(p, &len);
}

static size_t
tp_size_vpart(struct tp *tp, ulong *flagsp)
{
	register size_t size = 0;
	register uint bit;
	size_t addopts = 0;
	size_t altclass = 2;

	/* Note: this can be an estimate as long as the estimate is larger than the possible size. */
	for (bit = _T_BIT_TCO_THROUGHPUT; bit <= _T_BIT_TCO_ALTCLASS4; bit++) {
		switch (bit) {
		case _T_BIT_TCO_THROUGHPUT:
			size += 2 + 24;
			break;
		case _T_BIT_TCO_TRANSDEL:
			size += 2 + 8;
			break;
		case _T_BIT_TCO_RESERRORRATE:
			size += 2 + 3;
			break;
		case _T_BIT_TCO_TRANSFFAILPROB:
		case _T_BIT_TCO_ESTFAILPROB:
		case _T_BIT_TCO_RELFAILPROB:
		case _T_BIT_TCO_ESTDELAY:
		case _T_BIT_TCO_RELDELAY:
		case _T_BIT_TCO_CONNRESIL:
		case _T_BIT_TCO_PROTECTION:
			size += 2 + 1;
			break;
		case _T_BIT_TCO_PRIORITY:
			size += 2 + 2;
			break;
		case _T_BIT_TCO_EXPD:
			addopts = 2 + 1;
			break;
		case _T_BIT_TCO_LTPDU:
			size += 2 + 2;
			size += 2 + 4;
			break;
		case _T_BIT_TCO_ACKTIME:
			size += 2 + 2;
			break;
		case _T_BIT_TCO_REASTIME:
			size += 2 + 2;
			break;
		case _T_BIT_TCO_EXTFORM:
			size += 0;	/* part of class octet */
			break;
		case _T_BIT_TCO_FLOWCTRL:
			size += 0;	/* part of class octet */
			break;
		case _T_BIT_TCO_CHECKSUM:
			addopts = 2 + 1;
			break;
		case _T_BIT_TCO_NETEXP:
			addopts = 2 + 1;
			break;
		case _T_BIT_TCO_NETRECPTCF:
			addopts = 2 + 1;
			break;
		case _T_BIT_TCO_PREFCLASS:
			size += 0;	/* part of class octet */
			break;
		case _T_BIT_TCO_ALTCLASS1:
			altclass += 1;
			break;
		case _T_BIT_TCO_ALTCLASS2:
			altclass += 1;
			break;
		case _T_BIT_TCO_ALTCLASS3:
			altclass += 1;
			break;
		case _T_BIT_TCO_ALTCLASS4:
			altclass += 1;
			break;
		}
	}
}

static int
tp_build_vpart(struct tp *tp, ulong *flagsp)
{
}

/*
 * --------------------------------------------------------------------------
 *
 * Sending and Receiving TP Primitives
 *
 * --------------------------------------------------------------------------
 */

/* Class options */
#define _T_F_CO_RESERVED4	(1<<3)	/* reserved */
#define _T_F_CO_RESERVED3	(1<<2)	/* reserved */
#define _T_F_CO_EXTFORM		(1<<1)	/* extended format */
#define _T_F_CO_FLOWCTRL	(1<<0)	/* explicit flow control */

/* Additional options - XTI doesn't know about these first three */
#define _T_F_AO_NBLKEXPDATA	(1<<6)	/* non-blocking expedited data */
#define _T_F_AO_REQUESTACK	(1<<5)	/* request acknowledgement */
#define _T_F_AO_SELECTACK	(1<<4)	/* selective acknowledgement */
#define _T_F_AO_NETEXP		(1<<3)	/* network expedited data */
#define _T_F_AO_NETRECPTCF	(1<<2)	/* receipt confirmation */
#define _T_F_AO_CHECKSUM	(1<<1)	/* checksum */
#define _T_F_AO_EXPD		(1<<0)	/* transport expedited data */

/* these are really size = (1<<value) */
#define _T_TPDU_SIZE_8192	0x0d
#define _T_TPDU_SIZE_4096	0x0c
#define _T_TPDU_SIZE_2048	0x0b
#define _T_TPDU_SIZE_1024	0x0a
#define _T_TPDU_SIZE_512	0x09
#define _T_TPDU_SIZE_256	0x08
#define _T_TPDU_SIZE_128	0x07

/* disconnect reasons, these should be in TPI header file */
#define _TP_REASON_UNSPECIFIED		0x00
#define _TP_REASON_TSAP_CONGESTION	0x01
#define _TP_REASON_TSAP_NO_SESSION	0x02	/* persistent */
#define _TP_REASON_ADDRESS_UNKNOWN	0x03	/* persistent */
#define _TP_REASON_NORMAL_DISCONNECT	0x80
#define _TP_REASON_REMOTE_CONGESTION	0x81
#define _TP_REASON_NEGOTIATION_FAILED	0x82	/* persistent */
#define _TP_REASON_DUPLICATE_SREF	0x83
#define _TP_REASON_MISMATCH_REFS	0x84
#define _TP_REASON_PROTOCOL_ERROR	0x85
#define _TP_REASON_NOT_USED1		0x86
#define _TP_REASON_REFERENCE_OVERFLOW	0x87
#define _TP_REASON_REFUSED		0x88
#define _TP_REASON_NOT_USED2		0x89
#define _TP_REASON_INVALID_LENGTH	0x8a

/* error causes, these should be in TPI header file */
#define _TP_ERROR_UNSPECIFIED		0x00
#define _TP_ERROR_INVALID_PARM_TYPE	0x01
#define _TP_ERROR_INVALID_TPDU_TYPE	0x02
#define _TP_ERROR_INVALID_PARM_VALUE	0x03

/* message types */
#define	_TP_MT_ED	0x10
#define	_TP_MT_EA	0x20
#define _TP_MT_UD	0x40
#define	_TP_MT_RJ	0x50
#define	_TP_MT_AK	0x60
#define	_TP_MT_ER	0x70
#define	_TP_MT_DR	0x80
#define	_TP_MT_DC	0xc0
#define	_TP_MT_CC	0xd0
#define	_TP_MT_CR	0xe0
#define	_TP_MT_DT	0xf0

#define _TP_PT_INVALID_TPDU	0xc1	/* invalid TPDU */

#define _TP_PT_TPDU_SIZE	0xc0	/* TPDU size */
#define _TP_PT_CGTRANSSEL	0xc1	/* calling transport selector */
#define _TP_PT_CDTRANSSEL	0xc2	/* called transport selector */
#define _TP_PT_CHECKSUM		0xc3	/* checksum */
#define _TP_PT_VERSION		0xc4	/* version number */
#define _TP_PT_PROTECTION	0xc5	/* protection */
#define _TP_PT_ADDOPTIONS	0xc6	/* additional options */
#define _TP_PT_ALTCLASS		0xc7	/* alternative protocol classes */

#define _TP_PT_ACKTIME		0x85	/* acknowledgement time */
#define _TP_PT_RESERRORRATE	0x86	/* residual error rate */
#define _TP_PT_PRIORITY		0x87	/* priority */
#define _TP_PT_TRANSDEL		0x88	/* transit delay */
#define _TP_PT_THROUGHPUT	0x89	/* throughput */
#define _TP_PT_SUBSEQUENCE	0x8a	/* subsequence number */
#define _TP_PT_REASTIME		0x8b	/* reassignment time */
#define _TP_PT_FLOWCTLCF	0x8c	/* flow control confirmation */
#define _TP_PT_SELECTACK	0x8f	/* selective acknowledgement */

#define _TP_PT_ED_TPDU_NR	0x90	/* ED-TPDU-NR */

#define _TP_PT_DIAGNOSTIC	0xe0	/* diagnostic */

#define _TP_PT_PREF_TPDU_SIZE	0xf0	/* preferred TPDU size */
#define _TP_PT_INACTTIME	0xf2	/* inactivity time */

static fastcall inline void
tp_pack_2(uchar **p, uint32_t value)
{
	*(*p)++ = (value >> 8);
	*(*p)++ = (value >> 0);
}
static fastcall inline void
tp_pack_3(uchar **p, uint32_t value)
{
	*(*p)++ = (value >> 16);
	*(*p)++ = (value >> 8);
	*(*p)++ = (value >> 0);
}
static fastcall inline void
tp_pack_4(uchar **p, uint32_t value)
{
	*(*p)++ = (value >> 24);
	*(*p)++ = (value >> 16);
	*(*p)++ = (value >> 8);
	*(*p)++ = (value >> 0);
}

static fastcall inline void
tp_pack_tl1(uchar **p, uchar tag, uint32_t value)
{
	*(*p)++ = tag;
	*(*p)++ = 1;
	*(*p)++ = (value >> 0);
}
static fastcall inline void
tp_pack_tl2(uchar **p, uchar tag, uint32_t value)
{
	*(*p)++ = tag;
	*(*p)++ = 2;
	*(*p)++ = (value >> 8);
	*(*p)++ = (value >> 0);
}
static fastcall inline void
tp_pack_tl3(uchar **p, uchar tag, uint32_t value)
{
	*(*p)++ = tag;
	*(*p)++ = 3;
	*(*p)++ = (value >> 16);
	*(*p)++ = (value >> 8);
	*(*p)++ = (value >> 0);
}
static fastcall inline void
tp_pack_tl4(uchar **p, uchar tag, uint32_t value)
{
	*(*p)++ = tag;
	*(*p)++ = 4;
	*(*p)++ = (value >> 24);
	*(*p)++ = (value >> 16);
	*(*p)++ = (value >> 8);
	*(*p)++ = (value >> 0);
}
static fastcall inline void
tp_pack_tvar(uchar **p, uchar tag, uint32_t value)
{
	*(*p)++ = tag;
	if (value >= 1 << 24) {
		*(*p)++ = 4;
		*(*p)++ = (value >> 24);
		*(*p)++ = (value >> 16);
		*(*p)++ = (value >> 8);
		*(*p)++ = (value >> 0);
	} else if (value >= 1 << 16) {
		*(*p)++ = 3;
		*(*p)++ = (value >> 16);
		*(*p)++ = (value >> 8);
		*(*p)++ = (value >> 0);
	} else if (value >= 1 << 8) {
		*(*p)++ = 2;
		*(*p)++ = (value >> 8);
		*(*p)++ = (value >> 0);
	} else {
		*(*p)++ = 1;
		*(*p)++ = (value >> 0);
	}
}
static fastcall inline void
tp_pack_tbuf(uchar **p, uchar tag, struct netbuf *buf)
{
	if (buf && buf->len > 0 && buf->buf != NULL) {
		*(*p)++ = tag;
		*(*p)++ = buf->len;
		bcopy(buf->buf, *p, buf->len);
		*p += buf->len;
	}
}
static fastcall inline void
tp_pack_tsap(uchar **p, uchar tag, struct tp_tsapid *tsap)
{
	if (tsap->len > 0) {
		*(*p)++ = tag;
		*(*p)++ = tsap->len;
		bcopy(tsap->buf, p, tsap->len);
		*p += tsap->len;
	}
}

/**
 * tp_add_checksum - complete the checksum
 * @mp: message containing TPDU header in first mblk and user data in next
 *
 * Add a checksum parameter to the variable part and calculate a checksum for
 * the resulting TPDU.  This calculation follows the procedures in X.224 Annex B
 * and X.234 Annex C.
 *
 * Note that the entire header of the TPDU (from LI through variable part) must
 * be contained in @mp.  mp->b_cont must point to a buffer chain that contains
 * the user data, or NULL when there is no user data.
 */
static void
tp_add_checksum(mblk_t *mp)
{
	int c0 = 0, c1 = 0, L = 0;
	mblk_t *bp;
	uchar *c;
	int n;

	mp->b_rptr[0] += 4;	/* add 4 to length indicator */
	*mp->b_wptr++ = _TP_PT_CHECKSUM;
	*mp->b_wptr++ = 2;
	c = mp->b_wptr;
	n = mp->b_wptr - mp->b_rptr;
	/* zero checksum to begin with */
	*mp->b_wptr++ = 0;
	*mp->b_wptr++ = 0;
	{
		register uchar *p;
		register uchar *e;

		for (bp = mp; bp; bp = bp->b_cont) {
			p = bp->b_rptr;
			e = bp->b_wptr;
			L += e - p;
			while (p < e) {
				c0 += *p++;
				c1 += c0;
			}
		}
	}
	c[0] = -c1 + (L - n) * c0;
	c[1] = c1 - (L - n + 1) * c0;
	return;
}

static void
tp_adjust_header(struct tp *tp, mblk_t *mp, mblk_t *dp)
{
	mp->b_cont = dp;
	mp->b_rptr[0] = msgdsize(mp) - 1;
	if (tp->addopts & _T_F_AO_CHECKSUM)
		tp_add_checksum(mp);
	return;
}

/*
 * Procedures for packing up TPDUs.  These procedures pack a TPDU of a
 * particular type.  They only pack the header (fixed part and variable part),
 * and not the user data, but they do calculate the length indicator.
 */

/**
 * tp_pack_cr: - pack up a CR-TPDU (minus data and checksum)
 * @tp: transport provider private structure
 * @q: active queue
 * @dp: user data (or NULL)
 */
static mblk_t *
tp_pack_cr(struct tp *tp, queue_t *q, mblk_t *dp)
{
	mblk_t *mp;

	/* 128 is large enough to contain the entire CR-TPDU header */
	if ((mp = mi_allocb(q, 128, BPRI_MED))) {
		register uchar *p = mp->b_wptr;
		struct tp_options *op = &tp->opts.req;
		struct tp_tco *tco = &op->tco;
		struct reqvalue *req;
		uint32_t size;
		int order;

		*p++ = 0;	/* fix up later */
		*p++ = _TP_MT_CR | tp->credit;
		tp_pack_2(&p, tp->dref);
		tp_pack_2(&p, tp->sref);
		*p++ = (tco->prefclass << 4) | (tp->options & (_T_F_CO_EXTFORM | _T_F_CO_FLOWCTRL));
		/* calling transport selector */
		tp_pack_tsap(&p, _TP_PT_CGTRANSSEL, &tp->src);
		/* called transport selector */
		tp_pack_tsap(&p, _TP_PT_CDTRANSSEL, &tp->dst);
		if (tco->ltpdu != T_UNSPEC) {
			size = tco->ltpdu;
			for (order = _T_TPDU_SIZE_8192; order >= _T_TPDU_SIZE_128; order--)
				if ((1 << order) <= size)
					break;
			if (order != _T_TPDU_SIZE_128) {
				/* TPDU size */
				tp_pack_tl1(&p, _TP_PT_TPDU_SIZE, order);
			}
			if (!(size >>= 7))
				size = 1;
			/* preferred TPDU size */
			tp_pack_tvar(&p, _TP_PT_PREF_TPDU_SIZE, size);
		}
		/* For RFC 1006 operation, only protocol class 0 is used and this entire code block 
		   can be skipped. */
		if (tco->prefclass != T_CLASS0) {
			/* version number */
			tp_pack_tl1(&p, _TP_PT_VERSION, 1);
			if (t_tst_bit(_T_BIT_TCO_PROTECTION, op->flags)
			    && tco->protection != T_UNSPEC) {
				/* protection */
				tp_pack_tl1(&p, _TP_PT_PROTECTION, tco->protection & ~T_ABSREQ);
			}
			/* additional options */
			tp_pack_tl1(&p < _TP_PT_ADDOPTIONS, tp->addopts);
			if (tp->network != N_CLNS) {
				int i, altnum = 0;

				for (i = 0; i < 4; i++)
					if ((&tco->altclass1)[i] != T_UNSPEC)
						altnum++;
				if (altnum) {
					/* alternative protocol classes */
					*p++ = _TP_PT_ALTCLASS;
					*p++ = altnum;
					for (i = 0; i < 4; i++) {
						if ((&tco->altclass1)[i] != T_UNSPEC)
							*p++ = (&tco->altclass1)[i];
					}
				}
			}
			if (tco->prefclass == T_CLASS4) {
				/* acknowledgement time */
				tp_pack_tl2(&p, _TP_PT_ACKTIME, tco->acktime);
			}
			if (tp->flags & _TP_MAX_THROUGHPUT) {
				/* throughput */
				*p++ = _TP_PT_THROUGHPUT;
				*p++ = (tp->flags & _TP_AVG_THROUGHPUT) ? 24 : 12;
				req = &tco->throughput.maxthrpt;
				tp_pack_3(&p, req->called.targetvalue);
				tp_pack_3(&p, req->called.minacceptvalue);
				tp_pack_3(&p, req->calling.targetvalue);
				tp_pack_3(&p, req->calling.minacceptvalue);
				if (tp->flags & _TP_AVG_THROUGHPUT) {
					req = &tco->throughput.avgthrpt;
					tp_pack_3(&p, req->called.targetvalue);
					tp_pack_3(&p, req->called.minacceptvalue);
					tp_pack_3(&p, req->calling.targetvalue);
					tp_pack_3(&p, req->calling.minacceptvalue);
				}
			}
			if (tp->flags & _TP_RESIDERRRATE) {
				/* residual error rate */
				*p++ = _TP_PT_RESERRORRATE;
				*p++ = 3;
				req = &tco->reserrorrate;
				*p++ = req->targetvalue;
				*p++ = req->minacceptvalue;
				*p++ = tco->ltpdu;	/* FIXME */
			}
			/* priority */
			tp_pack_tl2(&p, _TP_PT_PRIORITY, tco->priority);
			/* transit delay */
			*p++ = _TP_PT_TRANSDEL;
			*p++ = 8;
			req = &tco->transdel.maxdel;
			tp_pack_2(&p, req->called.targetvalue);
			tp_pack_2(&p, req->called.minacceptvalue);
			tp_pack_2(&p, req->calling.targetvalue);
			tp_pack_2(&p, req->calling.minacceptvalue);
			if (tco->prefclass >= T_CLASS3) {
				/* reassignment time */
				tp_pack_tl2(&p, _TP_PT_REASTIME, tco->reastime);
			}
			if (tco->prefclass == T_CLASS4) {
				/* inactivity timer */
				tp_pack_tl4(&p, _TP_PT_INACTTIME, tp->intactivty);
			}
		}
		mp->b_wptr = p;
		tp_adjust_header(tp, mp, dp);
	}
	return (mp);
}

/**
 * tp_pack_cc: - pack up a CC-TPDU (minus data and checksum)
 * @tp: transport provider private structure
 * @q: active queue
 * @dp: user data (or NULL)
 */
static mblk_t *
tp_pack_cc(struct tp *tp, queue_t *q, mblk_t *dp)
{
	mblk_t *mp;

	/* 128 is large enough to contain the entire CC-TPDU header */
	if ((mp = mi_allocb(q, 128, BPRI_MED))) {
		register uchar *p = mp->b_wptr;
		struct tp_options *op = &tp->opts.res;
		struct tp_tco *tco = &op->tco;

		*p++ = 0;	/* fix up later */
		*p++ = _TP_MT_CC | tp->credit;
		tp_pack_2(&p, tp->dref);
		tp_pack_2(&p, tp->sref);
		*p++ = (tco->prefclass << 4) | (tp->options & (_T_F_CO_EXTFORM | _T_F_CO_FLOWCTRL));
		/* calling transport selector */
		tp_pack_tsap(&p, _TP_PT_CGTRANSSEL, &tp->src);
		/* called transport selector */
		tp_pack_tsap(&p, _TP_PT_CDTRANSSEL, &tp->dst);
		if (tco->ltpdu != T_UNSPEC) {
			int order, uint32_t size;

			size = tco->ltpdu;
			for (order = _T_TPDU_SIZE_8192; order >= _T_TPDU_SIZE_128; order--)
				if ((1 << order) <= size)
					break;
			if (order != _T_TPDU_SIZE_128) {
				/* TPDU size */
				tp_pack_tl1(&p, _TP_PT_TPDU_SIZE, order);
			}
			if (!(size >>= 7))
				size = 1;
			/* preferred TPDU size */
			tp_pack_tvar(&p, _TP_PT_PREF_TPDU_SIZE, size);
		}
		/* For RFC 1006 operation, only protocol class 0 is used and this entire code block 
		   can be skipped. */
		if (tco->prefclass != T_CLASS0) {
#if 0
			/* version number */
			tp_pack_tl1(&p, _TP_PT_VERSION, 1);
#endif
			/* additional options */
			tp_pack_tl1(&p < _TP_PT_ADDOPTIONS, tp->addopts);
#if 0
			if (tp->network != N_CLNS) {
				int i, altnum = 0;

				for (i = 0; i < 4; i++)
					if ((&tco->altclass1)[i] != T_UNSPEC)
						altnum++;
				if (altnum) {
					/* alternative protocol classes */
					*p++ = _TP_PT_ALTCLASS;
					*p++ = altnum;
					for (i = 0; i < 4; i++) {
						if ((&tco->altclass1)[i] != T_UNSPEC)
							*p++ = (&tco->altclass1)[i];
					}
				}
			}
#endif
			if (tco->prefclass == T_CLASS4) {
				/* acknowledgement time */
				tp_pack_tl2(&p, _TP_PT_ACKTIME, tco->acktime);
			}
			if (tp->flags & _TP_MAX_THROUGHPUT) {
				/* throughput */
				*p++ = _TP_PT_THROUGHPUT;
				*p++ = (tp->flags & _TP_AVG_THROUGHPUT) ? 24 : 12;
				req = &tco->throughput.maxthrpt;
				tp_pack_3(&p, req->called.targetvalue);
				tp_pack_3(&p, req->called.minacceptvalue);
				tp_pack_3(&p, req->calling.targetvalue);
				tp_pack_3(&p, req->calling.minacceptvalue);
				if (tp->flags & _TP_AVG_THROUGHPUT) {
					req = &tco->throughput.avgthrpt;
					tp_pack_3(&p, req->called.targetvalue);
					tp_pack_3(&p, req->called.minacceptvalue);
					tp_pack_3(&p, req->calling.targetvalue);
					tp_pack_3(&p, req->calling.minacceptvalue);
				}
			}
			if (tp->flags & _TP_RESIDERRRATE) {
				/* residual error rate */
				*p++ = _TP_PT_RESERRORRATE;
				*p++ = 3;
				req = &tco->reserrorrate;
				*p++ = req->targetvalue;
				*p++ = req->minacceptvalue;
				*p++ = tco->ltpdu;	/* FIXME */
			}
			/* priority */
			tp_pack_tl2(&p, _TP_PT_PRIORITY, tco->priority);
			/* transit delay */
			*p++ = _TP_PT_TRANSDEL;
			*p++ = 8;
			req = &tco->transdel.maxdel;
			tp_pack_2(&p, req->called.targetvalue);
			tp_pack_2(&p, req->called.minacceptvalue);
			tp_pack_2(&p, req->calling.targetvalue);
			tp_pack_2(&p, req->calling.minacceptvalue);
#if 0
			if (tco->prefclass >= T_CLASS3) {
				/* reassignment time */
				tp_pack_tl2(&p, _TP_PT_REASTIME, tco->reastime);
			}
#endif
			if (tco->prefclass == T_CLASS4) {
				/* inactivity timer */
				tp_pack_tl4(&p, _TP_PT_INACTTIME, tp->intactivty);
			}
		}

		mp->b_wptr = p;
		tp_adjust_header(tp, mp, dp);
	}
	return (mp);
}

/**
 * tp_pack_dr: - pack up a DR-TPDU (minus data and checksum)
 * @tp: transport provider private structure
 * @q: active queue
 * @reason: reason for disconnect
 * @diag: diagnostic to include (or NULL, or zeroed nebuf)
 * @dp: user data (or NULL)
 */
static mblk_t *
tp_pack_dr(struct tp *tp, queue_t *q, t_uscalar_t reason, struct netbuf *diag, mblk_t *dp)
{
	mblk_t *mp;

	if ((mp = mi_allocb(q, 13 + diag ? diag->len : 0, BPRI_MED))) {
		register uchar *p = mp->b_wptr;

		*p++ = 0;	/* fix up LI later */
		*p++ = _TP_MT_DR;	/* disconnect request */
		tp_pack_2(&p, tp->dref);
		tp_pack_2(&p, tp->sref);
		*p++ = reason;
		/* diagnostic */
		tp_pack_tbuf(&p, _TP_PT_DIAGNOSTIC, diag);
		mp->b_wptr = p;
		tp_adjust_header(tp, mp, dp);
	}
	return (mp);
}

/**
 * tp_pack_dc: - pack up a DC-TPDU (minus data and checksum)
 * @tp: transport provider private structure
 * @q: active queue
 * @dp: user data (or NULL)
 */
static mblk_t *
tp_pack_dc(struct tp *tp, queue_t *q, mblk_t *dp)
{
	mblk_t *mp;

	if ((mp = mi_allocb(q, 10, BPRI_MED))) {
		register uchar *p = mp->b_wptr;

		*p++ = 0;	/* fix up LI later */
		*p++ = _TP_MT_DC;	/* disconnect confirm */
		tp_pack_2(&p, tp->dref);
		tp_pack_2(&p, tp->sref);
		mp->b_wptr = p;
		tp_adjust_header(tp, mp, dp);
	}
	return (mp);
}

/**
 * tp_pack_dt: - pack up a DT-TPDU (minus data and checksum)
 * @tp: transport provider private structure
 * @q: active queue
 * @roa: receipt acknowledgement option
 * @eot: end of TPDU
 * @nr: sequence number
 * @ednr: expedited data sequence number
 * @dp: user data (or NULL)
 */
static fastcall inline __hot_put mblk_t *
tp_pack_dt(struct tp *tp, queue_t *q, uchar roa, char eot, uint32_t nr, int ednr, mblk_t *dp)
{
	mblk_t *mp;

	if ((mp = mi_allocb(q, 18, BPRI_MED))) {
		register uchar *p = mp->b_wptr;
		struct tp_options *op = &tp->opts.res;
		struct tp_tco *tco = &op->tco;

		*p++ = 0;
		*p++ = _TP_MT_DT | (roa ? 1 : 0);	/* data */
		switch (tco->prefclass) {
		case T_CLASS0:
		case T_CLASS1:
			*p++ = eot ? (nr | 0x80) : (nr & 0x7f);
			break;
		case T_CLASS2:
		case T_CLASS3:
		case T_CLASS4:
			tp_pack_2(&p, tp->dref);
			if (!(tp->options & _T_F_CO_EXTFORM)) {
				*p++ = eot ? (nr | 0x80) : (nr & 0x7f);
			} else {
				tp_pack_4(&p, eot ? (nr | 0x80000000) : (nr & 0x7fffffff));
			}
			if ((tp->addopts & _T_F_AO_NBLKEXPDATA) && ednr != -1) {
				/* ED-TPDU-NR */
				if (!(tp->options & _T_F_CO_EXTFORM)) {
					tp_pack_tl2(&p, _TP_PT_ED_TPDU_NR, ednr);
				} else {
					tp_pack_tl4(&p, _TP_PT_ED_TPDU_NR, ednr);
				}
			}
			break;
		}
		mp->b_wptr = p;
		tp_adjust_header(tp, mp, dp);
	}
	return (mp);
}

/**
 * tp_pack_ed: - pack up a ED-TPDU (minus data and checksum)
 * @tp: transport provider private structure
 * @q: active queue
 * @nr: sequence number
 * @dp: user data (or NULL)
 */
static mblk_t *
tp_pack_ed(struct tp *tp, queue_t *q, uint32_t nr, mblk_t *dp)
{
	mblk_t *mp;

	if ((mp = mi_allocb(q, 12, BPRI_MED))) {
		register uchar *p = mp->b_wptr;
		struct tp_options *op = &tp->opts.res;
		struct tp_tco *tco = &op->tco;

		*p++ = 0;	/* fix up LI later */
		*p++ = _TP_MT_ED;	/* expedited data */
		/* RFC 1006 does not include DST-REF in class 0 */
		if (tco->prefclass != T_CLASS0)
			tp_pack_2(&p, tp->dref);
		if (!(tp->options & _T_F_CO_EXTFORM)) {
			*p++ = nr | 0x80;
		} else {
			tp_pack_4(&p, nr | 0x80000000);
		}
		mp->b_wptr = p;
		tp_adjust_header(tp, mp, dp);
	}
	return (mp);
}

/**
 * tp_pack_ak: - pack up a AK-TPDU (minus data and checksum)
 * @tp: transport provider private structure
 * @q: active queue
 * @nr: sequence numebr
 * @dp: user data (or NULL)
 */
static mblk_t *
tp_pack_ak(struct tp *tp, queue_t *q, uint32_t nr, mblk_t *dp)
{
	mblk_t *mp;

	if ((mp = mi_allocb(q, 32 + (tp->sackblks << 3), BPRI_MED))) {
		register uchar *p = mp->b_rptr;
		struct tp_options *op = &tp->opts.res;
		struct tp_tco *tco = &op->tco;
		int sackblks;

		*p++ = 0;	/* fix up LI later */
		if (!(tp->options & _T_F_CO_EXTFORM)) {
			*p++ = _TP_MT_AK | (tp->credit & 0x0f);
		} else {
			*p++ = _TP_MT_AK;
		}
		tp_pack_2(&p, tp->dref);
		if (!(tp->options & _T_F_CO_EXTFORM)) {
			*p++ = nr & 0x7f;
		} else {
			tp_pack_4(&p, nr & 0x7fffffff);
			tp_pack_2(&p, tp->credit);
		}
		if (tco->prefclass = T_CLASS4) {
			if (tp->ssn != 0) {
				/* subsequence number */
				tp_pack_tl2(&p, _TP_PT_SUBSEQUENCE, tp->ssn);
			}
			if (tp->options & _T_F_CO_FLOWCTRL) {
				/* flow control confirmation */
				*p++ = _TP_PT_FLOWCTLCF;
				*p++ = 10;
				if (!(tp->options & _T_F_CO_EXTFORM)) {
					tp_pack_4(&p, tp->rnr & 0x7f);
				} else {
					tp_pack_4(&p, tp->rnr & 0x7fffffff);
				}
				tp_pack_2(&p, rssn);
				if (!(tp->options & _T_F_CO_EXTFORM)) {
					tp_pack_4(&p, tp->credit & 0x7f);
				} else {
					tp_pack_4(&p, tp->credit & 0x7fffffff);
				}
			}
			if (tp->addopts & _T_F_AO_SELECTACK) {
				/* selective acknowledgement */
				*p++ = _TP_PT_SELECTACK;
				if (!(tp->options & _T_F_CO_EXTFORM)) {
					*p++ = (sackblks = tp->sackblks) << 1;
					while (sackblks--) {
						*p++ = blk->beg & 0x7f;
						*p++ = blk->end & 0x7f;
					}
				} else {
					*p++ = (sackblks = tp->sackblks) << 3;
					while (sackblks--) {
						tp_pack_4(&p, blk->beg & 0x7fffffff);
						tp_pack_4(&p, blk->end & 0x7fffffff);
					}
				}
			}
		}
		mp->b_wptr = p;
		tp_adjust_header(tp, mp, dp);
	}
	return (mp);
}

/**
 * tp_pack_ea: - pack up a EA-TPDU (minus data and checksum)
 * @tp: transport provider private structure
 * @q: active queue
 * @dp: user data (or NULL)
 */
static mblk_t *
tp_pack_ea(struct tp *tp, queue_t *q, mblk_t *dp)
{
	mblk_t *mp;

	if ((mp = mi_allocb(q, 12, BPRI_MED))) {
		register uchar *p = mp->b_rptr;

		*p++ = 0;
		*p++ = _TP_MT_EA;
		tp_pack_2(&p, tp->dref);
		if (!(tp->options & _T_F_CO_EXTFORM)) {
			*p++ = nr & 0x7f;
		} else {
			tp_pack_4(&p, nr & 0x7fffffff);
		}
		mp->b_wptr = p;
		tp_adjust_header(tp, mp, dp);
	}
	return (mp);
}

/**
 * tp_pack_rj: - pack up a RJ-TPDU (minus data and checksum)
 * @tp: transport provider private structure
 * @q: active queue
 * @nr: sequence number
 * @dp: user data (or NULL)
 */
static mblk_t *
tp_pack_rj(struct tp *tp, queue_t *q, uint32_t nr, mblk_t *dp)
{
	mblk_t *mp;

	if ((mp = mi_allocb(q, 12, BPRI_MED))) {
		register uchar *p = mp->b_rptr;

		*p++ = 0;
		if (!(tp->options & _T_F_CO_EXTFORM)) {
			*p++ = _TP_MT_RJ | (tp->credit & 0x0f);
		} else {
			*p++ = _TP_MT_RJ;
		}
		tp_pack_2(&p, dref);
		if (!(tp->options & _T_F_CO_EXTFORM)) {
			*p++ = nr & 0x7f;
		} else {
			tp_pack_4(&p, nr & 0x7fffffff);
		}
		mp->b_wptr = p;
		tp_adjust_header(tp, mp, dp);
	}
	return (mp);
}

/**
 * tp_pack_er: - pack up a ER-TPDU (minus data and checksum)
 * @tp: transport provider private structure
 * @q: active queue
 * @tpdu: netbuf describing invalid TPDU
 * @dp: user data (or NULL)
 */
static mblk_t *
tp_pack_er(struct tp *tp, queue_t *q, struct netbuf *tpdu, mblk_t *dp)
{
	mblk_t *mp;

	if ((mp = mi_allocb(q, 11 + tpdu->len, BPRI_MED))) {
		register uchar *p = mp->b_wptr;

		*p++ = 0;
		*p++ = _TP_MT_ER;
		tp_pack_2(tp->dref);
		*p++ = cause;
		/* invalid TPDU */
		tp_pack_tbuf(&p, _TP_PT_INVALID_TPDU, tpdu);
		mp->b_wptr = p;
		tp_adjust_header(tp, mp, dp);
	}
	return (mp);
}

/**
 * tp_pack_ud: - pack up a UD-TPDU (minus data and checksum)
 * @tp: transport provider private structure
 * @q: active queue
 * @dp: user data (or NULL)
 */
static fastcall inline __hot_put mblk_t *
tp_pack_ud(struct tp *tp, queue_t *q, mblk_t *dp)
{
	mblk_t *mp;

	if ((mp = mi_allocb(q, 6 + tp->dst.len + tp->src.len, BPRI_MED))) {
		register uchar *p = mp->b_wptr;

		*p++ = 0;
		*p++ = _TP_MT_UD;
		/* called transport selector */
		tp_pack_tsap(&p, _TP_PT_CDTRANSSEL, &tp->dst);
		/* calling transport selector */
		tp_pack_tsap(&p, _TP_PT_CGTRANSSEL, &tp->src);
		mp->b_wptr = p;
		tp_adjust_header(tp, mp, dp);
	}
	return (mp);
}

/**
 * tp_send_cr: - send a CR-TPDU
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: message to free upon success
 * @dp: user data (or NULL)
 *
 * The CR-TPDU shall not exceed 128 octets.
 *
 * +-----+-----+-----+-----+-----+-----+-----+--------+--------+
 * | LI  |CR|CT|  DST-REF  |  SRC-REF  |CLASS|  VARP  |   UD   |
 * +-----+-----+-----+-----+-----+-----+-----+--------+--------+
 */
static int
tp_send_cr(struct tp *tp, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	int err = -ENOBUFS;
	mblk_t *bp;

	if ((bp = tp_pack_cr(tp, q, dp))) {
		if ((err = np_data_req(tp->np, q, msg, 0, bp)))
			freeb(bp);
	}
	return (err);
}

/**
 * tp_send_cc: - send a CC-TPDU
 * @tp: transport provider private structure
 * @flagsp: options flags
 * @dp: user data (or NULL)
 */
static int
tp_send_cc(struct tp *tp, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	int err = -ENOBUFS;
	mblk_t *bp;

	if ((bp = tp_pack_cc(tp, q, dp))) {
		if ((err = np_data_req(tp->np, q, msg, 0, bp)))
			freeb(bp);
	}
	return (err);
}

/**
 * tp_send_dr: - send a DR-TPDU
 * @tp: transport provider private structure
 * @dp: user data (or NULL)
 */
static int
tp_send_dr(struct tp *tp, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	int err = -ENOBUFS;
	mblk_t *bp;

	if ((bp = tp_pack_dr(tp, q, dp))) {
		if ((err = np_data_req(tp->np, q, msg, 0, bp)))
			freeb(bp);
	}
	return (err);
}

/**
 * tp_send_dc: - send a DC-TPDU
 * @tp: transport provider private structure
 */
static int
tp_send_dc(struct tp *tp, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	int err = -ENOBUFS;
	mblk_t *bp;

	if ((bp = tp_pack_dc(tp, q, dp))) {
		if ((err = np_data_req(tp->np, q, msg, 0, bp)))
			freeb(bp);
	}
	return (err);
}

/**
 * tp_send_dt: - send a DT-TPDU
 * @tp: transport provider private structure
 * @dp: user data
 */
static int
tp_send_dt(struct tp *tp, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	int err = -ENOBUFS;
	mblk_t *bp;

	if ((bp = tp_pack_dt(tp, q, dp))) {
		if ((err = np_data_req(tp->np, q, msg, 0, bp)))
			freeb(bp);
	}
	return (err);
}

/**
 * tp_send_ed: - send a ED-TPDU
 * @tp: transport provider private structure
 * @q: active queue (typically upper write queue)
 * @msg: message block to free upon success
 * @dp: user data
 */
static int
tp_send_ed(struct tp *tp, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	int err = -ENOBUFS;
	mblk_t *bp;

	if ((bp = tp_pack_ed(tp, q, tp->nr, dp))) {
		if (tp->addopts & _T_F_AO_NETEXP)
			err = np_exdata_req(tp->np, q, NULL, 0, bp);
		else
			err = np_data_req(tp->np, q, NULL, 0, bp);
		if (err == 0) {
			freeb(msg);
			tp->nr++;
		} else
			freeb(bp);
	}
	return (err);
}

/**
 * tp_send_ak: - send a AK-TPDU
 * @tp: transport provider private structure
 */
static int
tp_send_ak(struct tp *tp, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	int err = -ENOBUFS;
	mblk_t *bp;

	if ((bp = tp_pack_ak(tp, q, dp))) {
		if ((err = np_data_req(tp->np, q, msg, 0, bp)))
			freeb(bp);
	}
	return (err);
}

/**
 * tp_send_ea: - send a EA-TPDU
 * @tp: transport provider private structure
 */
static int
tp_send_ea(struct tp *tp, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	int err = -ENOBUFS;
	mblk_t *bp;

	if ((bp = tp_pack_ea(tp, q, dp))) {
		if ((err = np_data_req(tp->np, q, msg, 0, bp)))
			freeb(bp);
	}
	return (err);
}

/**
 * tp_send_rj: - send a RJ-TPDU
 * @tp: transport provider private structure
 */
static int
tp_send_rj(struct tp *tp, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	int err = -ENOBUFS;
	mblk_t *bp;

	if ((bp = tp_pack_rj(tp, q, dp))) {
		if ((err = np_data_req(tp->np, q, msg, 0, bp)))
			freeb(bp);
	}
	return (err);
}

/**
 * tp_send_er: - send a ER-TPDU
 * @tp: transport provider private structure
 */
static int
tp_send_er(struct tp *tp, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	int err = -ENOBUFS;
	mblk_t *bp;

	if ((bp = tp_pack_er(tp, q, dp))) {
		if ((err = np_data_req(tp->np, q, msg, 0, bp)))
			freeb(bp);
	}
	return (err);
}

/**
 * tp_send_ud: - send a UD-TPDU
 * @tp: transport provider private structure
 */
static int
tp_send_ud(struct tp *tp, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	int err = -ENOBUFS;
	mblk_t *bp;

	if ((bp = tp_pack_ud(tp, q, dp))) {
		if ((err = np_data_req(tp->np, q, msg, 0, bp)))
			freeb(bp);
	}
	return (err);
}

/**
 * np_recv_cr: - receive a Connection Request (CR) Message
 * @np: network provider private structure
 * @q: receiving queue (lower read queue)
 * @b: beginning of header (including li)
 * @e: end of header
 * @dp: message block containing message
 *
 * NOTE: The passed in data block must be pulled up by the caller.
 *
 * This is a connection request.  We must parse enough of the connection
 * request at the network level (lower multiplex) to determine to which
 * transport connection (new or existing) that the connection request
 * belongs.
 *
 * Information that will be used to identify the new or existing connection
 * are: the source IP address and port number, the destination IP address and
 * port number (which are basically identify and are emboidied by the network
 * connection upon which the connection request was received for TCP and
 * connectiln-like UDP, but are required to identify the virtual connection
 * for connectionless UDP), the called and calling transport selector and the
 * source reference.  Assuming that we just work with the connection-oriented
 * model, it is sufficient to use the source reference to identify an existing
 * transport connection, and the called NSAP and transport selector to
 * identify responder for a new connection.
 *
 * ISO/IEC 8073:1997(E) Appendix A, Table A.6 State table for classes 0 and 2:
 *
 * This TPDU is handled in states:
 *  TS_DATA_XFER (OPEN):-
 *	- P9: Class 4 CR
 *	    - stay in OPEN state
 *  TS_WIND_ORDREL (CLOSING):-
 *	- P9: Class 4 CR
 *	    - stay in CLOSING state
 *  TS_WRES_CIND (WFTRESP):-
 *	- P9: Class 4 CR
 *	    - stay in WFTRESP state
 *  TS_IDLE (CLOSED):-
 *	- P1; Unacceptable CR-TPDU
 *	    - send DR-TPDU
 *	    - an ER-TPDU should be sent in some cases (see 6.6)
 *	    - stay in CLOSED state
 *	- not P1: Acceptable CR-TPDU
 *	    - issue T_CONN_IND
 *	    - move to state TS_WRES_CIND (WFTRESP)
 */
static int
np_recv_cr(struct np *np, queue_t *q, uchar *b, uchar *e, mblk_t *dp)
{
	register uchar *p = b;
	ushort dref, sref, credit;
	uchar class, options, addopts = 0x01, altclasses = 0;

	if (e < p + 7)
		/* must include the fixed part */
		goto unspecified;
	p++;
	credit = (ushort) (*p++) & 0x0f;
	opt->rem.credit = credit;
	if (unlikely((dref = ((ushort) (*p++) << 8) | (ushort) (*p++)) != 0))
		goto invalid_parm_value;
	if (unlikely((sref = ((ushort) (*p++) << 8) | (ushort) (*p++)) == 0))
		goto invalid_parm_value;
	opt->rem.reference = sref;
	class = options = *p++;
	if ((class >>= 4) < T_CLASS4 && np->n_type != N_CONS)
		goto invalid_parm_value;
	if (class > T_CLASS4)
		goto invalid_parm_value;
	t_set_bit(_T_BIT_TCO_PREFCLASS, opt->flags);
	opt->tco.prefclass = class;
	altclasses = (1 << class);
	if ((options &= 0x0f) & (0x0c))
		goto invalid_parm_value;
	if (class >= T_CLASS1) {
		t_set_bit(_T_BIT_TCO_EXTFORM, opt->flags);
		opt->tco.extform = (options & _T_F_CO_EXTFORM) ? T_YES : T_NO;
	}
	if (class >= T_CLASS2) {
		t_set_bit(_T_BIT_TCO_FLOWCTRL, opt->flags);
		opt->tco.flowctrl = (options & _T_F_CO_FLOWCTRL) ? T_YES : T_NO;
	}
	while (p < e) {
		uchar ptype;
		uint plen;

		if (e < p + 3)
			/* parameters must be at least 3 bytes in length */
			goto invalid_parm_value;
		ptype = *p++;
		if ((plen = *p++) < 1)
			goto invalid_parm_value;
		if (e < p + plen)
			goto unspecified;
		switch (ptype) {
		case _TP_PT_TPDU_SIZE:	/* TPDU size */
			if (plen != 2)
				goto invalid_parm_value;
			if (*p < 7)
				goto invalid_parm_value;
			opt->tco.ltpdu = (1 << (*p++ - 7));
			t_set_bit(_T_BIT_TCO_LTPDU, opt->flags);
			continue;
		case _TP_PT_CGTRANSSEL:	/* calling transport selector */
			opt->rem.cgtranssel.len = plen;
			bcopy(p, opt->rem.cgtranssel.val, plen);
			p += plen;
			continue;
		case _TP_PT_CDTRANSSEL:	/* called transport selector */
			opt->rem.cdtranssel.len = plen;
			bcopy(p, opt->rem.cdtranssel.val, plen);
			p += plen;
			continue;
		case _TP_PT_CHECKSUM:	/* checksum */
			if (plen != 2)
				goto invalid_parm_value;
			checksum = t_unpack_2(&p);
			continue;
		case _TP_PT_VERSION:	/* version number */
			if (plen != 1)
				goto invalid_parm_value;
			opt->rem.version = *p++;
			continue;
		case _TP_PT_PROTECTION:	/* protection */
			if (plen != 1)
				goto invalid_parm_value;
			switch (*p) {
			case T_NOPROTECT:
			case T_PASSIVEPROTECT:
			case T_ACTIVEPROTECT:
			case (T_PASSIVEPROTECT | T_ACTIVEPROTECT):
				break;
			default:
				goto invalid_parm_value;
			}
			opt->tco.protection = *p++;
			t_set_bit(_T_BIT_TCO_PROTECTION, opt->flags);
			continue;
		case _TP_PT_ADDOPTIONS:	/* additional options */
			if (plen != 1)
				goto invalid_parm_value;
			if ((addopts = *p++) & 0x80)
				goto invalid_parm_value;
			if (class >= T_CLASS1) {
				/* use or non-use of network expedited data */
				opt->tco.netexp = (addopts & _T_F_AO_NETEXP) ? T_YES : T_NO;
				t_set_bit(_T_BIT_TCO_NETEXP, opt->flags);
				/* use or non-use of network receipt verification */
				opt->tco.netrecptcf = (addopts & _T_F_AO_NETRECPTCF) ? T_YES : T_NO;
				t_set_bit(_T_BIT_TCO_NETRECPTCF, opt->flags);
			}
			if (class >= T_CLASS4) {
				/* use or non-user of checksum */
				opt->tco.checksum = (addopts & _T_F_AO_CHECKSUM) ? T_YES : T_NO;
				t_set_bit(_T_BIT_TCO_CHECKSUM, opt->flags);
			}
			/* use or non-use of expedited data */
			opt->tco.expd = (addopts & _T_F_AO_EXPD) ? T_YES : T_NO;
			t_set_bit(_T_BIT_TCO_EXPD, opt->flags);
			continue;
		case _TP_PT_ALTCLASS:	/* alternative protocol classes */
			if (plen > 4)
				goto invalid_parm_value;
			while (plen--) {
				uchar altclass;

				t_set_bit(_T_BIT_TCO_ALTCLASS1 + plen);
				if ((altclass = (&opt->tco.altclass1)[plen] = *p++) > T_CLASS4)
					goto invalid_parm_value;
				if (altclass < T_CLASS4 && np->n_tryp != N_CONS)
					goto invalid parm value;
				altclasses |= (1 << altclass);
			}
		case _TP_PT_ACKTIME:	/* acknowledgement time */
			if (plen != 2)
				goto invalid_parm_value;
			opt->tco.acktime = t_unpack_n(&p, &plen);
			t_set_bit(_T_BIT_TCO_ACKTIME, opt->flags);
			continue;
		case _TP_PT_RESERRORRATE:	/* residual error rate */
			if (plen != 3)
				got invalid_parm_value;

			opt->tco.reserrorrate.targetvalue = *p++;
			opt->tco.reserrorrate.minacceptvalue = *p++;
			opt->rem.base_tpdu = *p++;
			t_set_bit(_T_BIT_TCO_RESERRORRATE, opt->flags);
			continue;
		case _TP_PT_PRIORITY:	/* priority */
			if (plen != 2)
				goto invalid_parm_value;
			opt->tco.priority = t_unpack_n(&p, &plen);
			t_set_bit(_T_BIT_TCO_PRIORITY, opt->flags);
			continue;
		case _TP_PT_TRANSDEL:	/* transit delay */
			if (plen != 8)
				goto invalid_parm_value;
			opt->tco.transdel.maxdel.called.targetvalue = t_unpack_2(&p);
			opt->tco.transdel.maxdel.called.minacceptvalue = t_unpack_2(&p);
			opt->tco.transdel.maxdel.calling.targetvalue = t_unpack_2(&p);
			opt->tco.transdel.maxdel.calling.minacceptvalue = t_unpack_2(&p);
			t_set_bit(_T_BIT_TCO_TRANSDEL, opt->flags);
			continue;
		case _TP_PT_THROUGHPUT:	/* throughput */
			if (plen != 12 && plen != 24)
				goto invalid_parm_value;
			opt->tco.throughput.maxthrpt.called.targetvalue = t_unpack_3(&p);
			opt->tco.throughput.maxthrpt.called.minacceptvalue = t_unpack_3(&p);
			opt->tco.throughput.maxthrpt.calling.targetvalue = t_unpack_3(&p);
			opt->tco.throughput.maxthrpt.calling.minacceptvalue = t_unpack_3(&p);
			if (plen == 24) {
				opt->tco.throughput.avgthrpt.called.targetvalue = t_unpack_3(&p);
				opt->tco.throughput.avgthrpt.called.minacceptvalue = t_unpack_3(&p);
				opt->tco.throughput.avgthrpt.calling.targetvalue = t_unpack_3(&p);
				opt->tco.throughput.avgthrpt.calling.minacceptvalue =
				    t_unpack_3(&p);
			} else {
				opt->tco.throughput.avgthrpt.called.targetvalue = T_UNSPEC;
				opt->tco.throughput.avgthrpt.called.minacceptvalue = T_UNSPEC;
				opt->tco.throughput.avgthrpt.calling.targetvalue = T_UNSPEC;
				opt->tco.throughput.avgthrpt.calling.minacceptvalue = T_UNSPEC;
			}
			t_set_bit(_T_BIT_TCO_THROUGHPUT, opt->flags);
			continue;
		case _TP_PT_SUBSEQUENCE:	/* subsequence number */
			goto invalid_parm_type;
		case _TP_PT_REASTIME:	/* reassignment time */
			if (plen != 2)
				goto invalid_parm_value;
			if ((1 << class) & ~((1 << T_CLASS0) | (1 << T_CLASS2)))
				goto invalid_parm_value;
			opt->tco.reastime = t_unpack_n(&p, &plen);
			t_set_bit(_T_BIT_TCO_REASTIME, opt->flags);
			continue;
		case _TP_PT_FLOWCTLCF:	/* flow control confirmation */
			goto invalid_parm_type;
		case _TP_PT_SELECTACK:	/* selective acknowledgement */
			goto invalid_parm_type;
		case _TP_PT_ED_TPDU_NR:	/* ED-TPDU-NR */
			goto invalid_parm_type;
		case _TP_PT_DIAGNOSTIC:	/* diagnostic */
			goto invalid_parm_type;
		case _TP_PT_PREF_TPDU_SIZE:	/* preferred TPDU size */
			if (plen > 4)
				goto invalid_parm_value;
			opt->tco.ltpdu = t_unpack_n(&p, &plen);
			t_set_bit(_T_BIT_TCO_LTPDU, opt->flags);
			continue;
		case _TP_PT_INACTTIME:	/* inactivity time */
			if (plen != 4)
				goto invalid_parm_value;
			if (class != T_CLASS4)
				goto invalid_parm_value;
			opt->rem.inactivity = t_unpack_n(&p, &plen);
			/* wht does not XTI know about this option? */
			continue;
		default:
			goto invalid_parm_type;
		}
		p += plen;
		continue;
	}
	return (0);
      unspecified:
	cause = _TP_ERROR_UNSPECIFIED;
	goto error;
      invalid_parm_type:
	cause = _TP_ERROR_INVALID_PARM_TYPE;
	goto error;
      error:
	if (!(ep = tp_pack_er(np, qback(q), cause, b, p - b)))
		return (-ENOBUFS);
}

/**
 * np_recv_cc: - receive a Connection Confirmation (CC) Message
 * @np: network provider private structure
 * @q: receiving queue (lower read queue)
 * @b: beginning of header (including li)
 * @e: end of header
 * @dp: message block containing message
 *
 * ISO/IEC 8073:1997(E) Appendix A, Table A.6 State table for classes 0 and 2:
 *
 * This TPDU is handled in states:
 * TS_WCON_CREQ (WFCC):-
 *  - P8: Acceptable CC
 *	- issue T_CONN_CON
 *	- move to the TS_DATA_XFER (OPEN) state
 *  - P6 and P5: Unacceptable CC and Class 0 (Class selected in CC)
 *	- issue T_DISCON_IND
 *	- issue N_DISCON_REQ
 *	- move to the TS_IDLE/NS_IDLE (CLOSED) state
 *  - P6 and P7: Unacceptable CC and Class 2
 *	- issue T_DISCON_IND
 *	- send DR-TPDU
 *	- move to the TS_WIND_ORDREL (CLOSING) state
 * TS_WCON_CREQ (WBCL):-
 *  - P5: A CR with class 2 has been sent and a CC class 0 is received.
 *	- issue N_DISCON_REQ
 *	- move to the TS_IDL/NS_IDLE (CLOSED) state
 *  - P7: Class 2
 *	- send DR-TPDU
 *	- move to the TS_WIND_ORDREL (CLOSING) state
 * TS_WIND_ORDREL (CLOSING):-
 *	- stay in the TS_WIND_ORDREL (CLOSING) state
 * TS_IDLE (CLOSED): -
 *	- send DR-TPDU
 *	- stay in the TS_IDLE (CLOSED) state
 */
static int
np_recv_cc(struct np *np, queue_t *q, uchar *b, uchar *e, mblk_t *dp)
{
}

/**
 * np_recv_dr: - receive a Disconnect Request (DR) Message
 * @np: network provider private structure
 * @q: receiving queue (lower read queue)
 * @b: beginning of header (including li)
 * @e: end of header
 * @dp: message block containing message
 *
 * ISO/IEC 8073:1997(E) Appendix A, Table A.6 State table for classes 0 and 2:
 *
 * This message is handled in states:
 * WFCC:-
 *	- issue T_DISCON_IND
 *	- move to the CLOSED state
 *	- if the network connection is not being used by another transport
 *	  connection assigned to it, it may be disconnected.
 * WBCL:-
 *	- move to the CLOSED state
 *	- if the network connection is not being used by another transport
 *	  connection assigned to it, it may be disconnected.
 * OPEN:-
 *  - P5: If received, it should be processed as a protocol error (see 6.22).
 *  - P7: Class 2
 *	- send DC-TPDU
 *	- issue T_DISCON_IND
 *	- move to state TS_IDLE (CLOSED)
 * CLOSING:-
 *	- move to the CLOSED state
 *	- if the network connection is not being used by another transport
 *	  connection assigned to it, it may be disconnected.
 * WFTRESP:-
 *  - P10:
 *	- send DC-TPDU
 *	- the DC-TPDU contains a src-ref filed set to zero and a dst-ref field
 *	  set to the SRC-REF of the DR-TPDU
 *	- this only happens when the preferred class of the CR-TPDU received is
 *	  class 4
 *	- send T_DISCON_IND
 *	- move to state TS_IDLE (CLOSED)
 * CLOSED:-
 *	- send DC-TPDU, unless the DC is not available (i.e. class 0 only
 *	  implemented) or SRC-REF is zero.
 *	- stay in CLOSED state
 */
static int
np_recv_dr(struct np *np, queue_t *q, uchar *b, uchar *e, mblk_t *dp)
{
}

/**
 * np_recv_dc: - receive a Disconnect Confirmation (DC) Message
 * @np: network provider private structure
 * @q: receiving queue (lower read queue)
 * @b: beginning of header (including li)
 * @e: end of header
 * @dp: message block containing message
 *
 * ISO/IEC 8073:1997(E) Appendix A, Table A.6 State table for classes 0 and 2:
 *
 * This message is handled in Class 2 in states:
 *
 * TS_WIND_ORDREL (CLOSING):-
 *  - P7: Class 2
 *	- move to state TS_IDLE (CLOSED)
 *	- if the network connection is not being used by another transport
 *	  connection assigned to it, it may be disconnected.
 *	- if received in Class 0, it should be processed as a protocol error
 *	  (see 6.22)
 * TS_IDLE (CLOSED):-
 *	- stay in the CLOSED state
 */
static int
np_recv_dc(struct np *np, queue_t *q, uchar *b, uchar *e, mblk_t *dp)
{
}

/**
 * np_recv_dt: - receive a Data (DT) Message
 * @np: network provider private structure
 * @q: receiving queue (lower read queue)
 * @b: beginning of header (including li)
 * @e: end of header
 * @dp: message block containing message
 *
 * ISO/IEC 8073:1997(E) Appendix A, Table A.6 State table for classes 0 and 2:
 *
 * This message is only handled in states:
 *
 * TS_DATA_XFER (OPEN):-
 *	- stay in the OPEN state
 *	- see data transfer procedure of the class
 * TS_WIND_ORDREL (CLOSING):-
 *	- stay in the CLOSING state
 * TS_IDLE (CLOSED):-
 *	- stay in the CLOSED state
 */
static int
np_recv_dt(struct np *np, queue_t *q, uchar *b, uchar *e, mblk_t *dp)
{
}

/**
 * np_recv_ed: - receive a Expedited Data (ED) Message
 * @np: network provider private structure
 * @q: receiving queue (lower read queue)
 * @b: beginning of header (including li)
 * @e: end of header
 * @dp: message block containing message
 *
 * ISO/IEC 8073:1997(E) Appendix A, Table A.6 State table for classes 0 and 2:
 *
 * This message is only handled in states:
 *
 * TS_DATA_XFER (OPEN):-
 *	- stay in the OPEN state
 *	- see expedited data transfer procedure of the class
 *	- if received in Class 0, it should be processed as a protocol error
 *	  (see 6.22)
 * TS_WIND_ORDREL (CLOSING):-
 *	- stay in the CLOSING state
 *	- if received in Class 0, it should be processed as a protocol error
 *	  (see 6.22)
 * TS_IDLE (CLOSED):-
 *	- stat in the CLOSED state
 */
static int
np_recv_ed(struct np *np, queue_t *q, uchar *b, uchar *e, mblk_t *dp)
{
}

/**
 * np_recv_ak: - receive a Acknowledgement (AK) Message
 * @np: network provider private structure
 * @q: receiving queue (lower read queue)
 * @b: beginning of header (including li)
 * @e: end of header
 * @dp: message block containing message
 *
 * ISO/IEC 8073:1997(E) Appendix A, Table A.6 State table for classes 0 and 2:
 *
 * This message is only handled in states:
 *
 * TS_DATA_XFER (OPEN):-
 *	- stay in the OPEN state
 *	- see data transfer procedure of the class
 *	- if received in Class 0, it should be processed as a protocol error
 *	  (see 6.22)
 * TS_WIND_ORDREL (CLOSING):-
 *	- stay in the CLOSING state
 *	- if received in Class 0, it should be processed as a protocol error
 *	  (see 6.22)
 * TS_IDLE (CLOSED):-
 *	- stay in the CLOSED state
 */
static int
np_recv_ak(struct np *np, queue_t *q, uchar *b, uchar *e, mblk_t *dp)
{
}

/**
 * np_recv_ea: - receive a Expedited Data Acknowledgement (EA) Message
 * @np: network provider private structure
 * @q: receiving queue (lower read queue)
 * @b: beginning of header (including li)
 * @e: end of header
 * @dp: message block containing message
 *
 * ISO/IEC 8073:1997(E) Appendix A, Table A.6 State table for classes 0 and 2:
 *
 * This message is only handled in states:
 *
 * TS_DATA_XFER (OPEN):-
 *	- state in the OPEN state
 *	- see expedited data transfer procedure of the class
 *	- if received in Class 0, it should be processed as a protocol error
 *	  (see 6.22)
 * TS_WIND_ORDREL (CLOSING):-
 *	- stay in the CLOSING state
 *	- if received in Class 0, it should be processed as a protocol error
 *	  (see 6.22)
 * TS_IDLE (CLOSED):-
 *	- stat in the CLOSED state
 */
static int
np_recv_ea(struct np *np, queue_t *q, uchar *b, uchar *e, mblk_t *dp)
{
}

/**
 * np_recv_rj: - receive a Reject (RJ) Message
 * @np: network provider private structure
 * @q: receiving queue (lower read queue)
 * @b: beginning of header (including li)
 * @e: end of header
 * @dp: message block containing message
 *
 * ISO/IEC 8073:1997(E) Appendix A, Table A.6 State table for classes 0 and 2:
 *
 * Not valid in Class 0 and 2: if received, it should be procssed as a protocol
 * error (see 6.22).
 *
 */
static int
np_recv_rj(struct np *np, queue_t *q, uchar *b, uchar *e, mblk_t *dp)
{
}

/**
 * np_recv_er: - receive a Error (ER) Message
 * @np: network provider private structure
 * @q: receiving queue (lower read queue)
 * @b: beginning of header (including li)
 * @e: end of header
 * @dp: message block containing message
 *
 * ISO/IEC 8073:1997(E) Appendix A, Table A.6 State table for classes 0 and 2:
 *
 * This message is only handled in states:
 *
 * TS_WCON_CREQ (WFCC):
 *	- issue T_DISCON_IND
 *	- move to tstate TS_IDLE (CLOSED)
 *	- if the network connection is not being used by another transport
 *	  connection assigned to it, it may be disconnected.
 * TS_WCON_CREQ (WBCL):
 *	- move to tstate TS_IDLE (CLOSED)
 *	- if the network connection is not being used by another transport
 *	  connection assigned to it, it may be disconnected.
 * TS_DATA_XFER (OPEN):
 *	- if received, it should be procssed as a protocol error (see 6.22).
 * TS_WIND_ORDREL (CLOSING):
 *	- if received, it should be procssed as a protocol error (see 6.22).
 * TS_IDLE (CLOSED):
 *	- stay in the TS_IDLE (CLOSE) state
 */
static int
np_recv_er(struct np *np, queue_t *q, uchar *b, uchar *e, mblk_t *dp)
{
}

/**
 * np_recv_ud: - receive a Unit Data (UD) Message
 * @np: network provider private structure
 * @q: receiving queue (lower read queue)
 * @b: beginning of header (including li)
 * @e: end of header
 * @dp: message block containing message
 *
 * Not valid in Class 0 and 2: if received, it should be procssed as a protocol
 * error (see 6.22).
 */
static int
np_recv_ud(struct np *np, queue_t *q, uchar *b, uchar *e, mblk_t *dp)
{
}

/**
 * np_rcv_msg: - receive NSDU
 * @q: active queue (lower read queue)
 * @mp: the message
 *
 * Association of TPDUs with transport connections.  Association of TPDUs with
 * transport connections when operating over CONS (TCP).  Purpose.  This
 * procedures is used in all classes to interpret a received NSDU as TPDU(s)
 * and, if possible, to associate each such TPDU with a transport connection.
 * Network service primitives.  The procedure uses the following network service
 * primitives: N-DATA indication, N-EXPEDITED-DATA indication, N-RESET request,
 * N-DISCONNECT reqeust.  TPDUs and parameters used.  The procedure uses the
 * following TPDUs and parameters: a) any TPDU except CR-TPDU, DT-TPDU in classes 0
 * and 1 and AK-TPDU in class 1: DST-REF; b) CR, CC, DR and DC-TPDUs: SRC-REF;
 * c) DT-TPDU in classes 0 or 1 and AK-TPDU in class 1.
 *
 * Procedures.  Identification of TPDUs.  If the received NSDU or expedited NSDU
 * cannot be decoded (i.e., does not contain one or more correct TPDUs) or is
 * corrupted (i.e. contains a TPDU with a wrong checksum), then the transport
 * entity shall: a) if the network connection on which the error is detected has
 * a class 0 or 1 transprot connection assigned to it, treate as a protocol
 * error (see 6.22) for that transport connection; b) otherwise: 1) if the NSDU
 * can be decoded but contains corrupted TPDUs, discard the TPDUs (class 4 only)
 * and optionally apply 6.9.1.4.1 b) 2); 2) if the NSDU cannot be decoded, issue
 * an N-RESET or N-DISCONNECT request for the network connection and for all the
 * transport connections assigned to this network connection (if any), apply the
 * procedures defined for handling of network signalling reset or disconnect.
 *
 * If the NSDU can be decoded and is not corrupted, the transport entity shall:
 * a) if the network connection on which the NSDU was received has a class 0
 * transport connection assigned to it, consider the NSDU as forming one TPDU
 * and associate the TPDU with the transport connection (see 6.9.1.4.2); b)
 * otherwise, invoke the separation procedures and for each of the individual
 * TPDUs in the order in which they appear in the NSDU apply the procedure
 * defined in 6.9.1.4.2.
 *
 * Association of individual TPDU.
 *
 * If the received TPDU is a CR-TPDU, and if the SRC-REF parameter and the
 * remote NSAP indicate an existing transport connection at that receiving
 * entity, then the CR-TPDU is associated with that transport connection,
 * otherwise it is processed as requesting the creation of a new transport
 * connection.  However, if the Network connection already has assigned a class
 * 0 or class 1 Transport connection, then, if the second CR that is the
 * retransmitted TPDU of class 4 has arrived, discard it and keep the first
 * Transport connection, otherwise (i.e. arrived CR-TPDU is not a retransmitted
 * TPDU of class 4) disconnect the Network connection.
 *
 * If the received TPDU is a DT-TPDU and the network connection has no TC
 * assigned to it, and the DT-TPDU is a class 0 or class 1 TPDU (as recognized
 * by the absense of a DST-REF field), then the TPDU should be ignored.  If the
 * received TPDU is a DT-TPDU and the network connection has a class 0 or 1
 * transport connection assigned to it, or an AK-TPDU where a class 1 transport
 * connectin is assigned, then the TPDU is associated with the transport
 * connection.
 *
 * Otherwise, the DST-REF parameter of the TPDU is used to identify the
 * transport connection.  The following cases are distinguished:
 *
 * a) If the DST-REF is not allocated to a transport connection, then no
 * association with a transport connection is made and there are three cases: 1)
 * If the TPDU is a CC-TPDU the transport entity shall respond on the same
 * network connection with a DR-TPDU.  The SRC-REF of the DR-TPDU may be either
 * 0 or the DST-REF from the received CC-TPDU.  2) If the TPDU is a DR-TPDU, the
 * transport entity shall respond on the same network connection with a DC-TPDU,
 * except in the case that the DR is carrying a SRC-REF set to zero, then no
 * DC-TPDU shall be sent, or in the case where the transport entity only
 * supports class 0, then the network conection shall be disconnected.  3) If
 * the TPDU is neither a CC nor a DR it shall be discarded.
 *
 * b) If the DST-REF is allocated to a transport connection, but the TPDU is
 * received on a network connection to which this connection has not been
 * assigned, then there are four cases: 1) If the transport connection is of
 * class 4 and if the TPDU is received on a network ocnnection with the same
 * pair of NSAPs as that of the CR-TPDU, then the TPDU is associated with this
 * transport connection and considered as performing assignment.  2) If the
 * transport connection is not assigned to any network connection (waiting for
 * reassignment after failure) and if the TPDU is received on a network
 * connection with the same pair of NSAPs as that of the CR-TPDU, then the
 * association with that transport connection with the same pair of NSAPs as
 * that of the CR-TPDU, then the association with that transport connection is
 * made, except in the acse of DC, DR and CC-TPDUs which are respeectively
 * described in 6.9.1.4.2 c), d) and e).  3) In classes 1 and 3, it is also
 * possible to receive a TPDU performing reassignment prior to the notification
 * of the disconnect of the current network connection (ie. the transport
 * connection is assigned to a network conenction, but a TPDU containing the
 * appropriate DST-REF is received on another network connection).  In this case
 * it is recommended that the transport entity: - issue an N-DISCONNECT request
 * on the network connection to which the transport connection is currently
 * assigned; - apply to all transport connections assigned to this network
 * connection the procedure for processing a received N-DISCONNECT indication;
 * and - then process the TPDU performing reassignment.  4) Otherwise, the TPDU
 * is considered as having a DST-REF not allocated to a transport connection
 * (case a).
 *
 * c) If the TPDU is a DC-TPDU, then it is associated with the transport
 * connection to which the DST-REF is allocated, unless the SRC-REF is not the
 * expected one, in which case, the DC-TPDU is discarded.
 *
 * d) If the TPDU is a DR-TPDU, then there are four cases: 1) if the SRC-REF is
 * not as expected, then a DC-TPDU with DST-REF equal to the SRC-REF of the
 * received DR-TPDU is sent back and no association is made, except that in the
 * case where the transport entity only supports class 0 and cannot transmit a
 * DC-TPDU, it disconects the network connection instead of transmitting a
 * DC-TPDU; 2) if a CR-TPDU is unacknowledged, then the DR-TPDU is associate
 * with the transport connection, regardless of the value of its SRC-REF
 * parameter.  3) if the transport entity implements class 4 and if the DST-REF
 * is zero and there is an unacknowledged CC-TPDU or T-CONNECT response is
 * awaited, then the DR-TPDU shall be associated with the trasnsport connection
 * holding th SRC-REF as the remote reference; 4) otherwise, the DR-TPDU is
 * associated with the transport connection identified by the DST-REF parameter.
 *
 * e) If the TPDU 0s a CC-TPDU whose DST-REF parameter identifies an open
 * connection (one for whcih a CC-TPDU has been previously received), and the
 * SRC-REF in the CC-TPDU does not match the remtoe reference, then a DR-TPDU is
 * sent back wtih DST-REF equal to the SRC-REF of the received CC-TPDU and no
 * association is made.
 *
 * f) If none of the above cases apply, then the TPDU is associated with the
 * transport connection identified by the DST-REF parameter.
 * 
 *
 */
static int
np_recv_msg(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	mblk_t *dp = mp->b_cont;
	register uchar *p = dp->b_rptr;
	register uchar *e = p + p[0] + 1;
	int hlen, dlen, rtn;
	uchar mtype;

	p = mp->b_rptr;
	hlen = p[0] + 1;
	e = p + hlen;
	if ((dlen = mp->b_rptr - e) < 0)
		goto short_header;
	switch ((mtype = p[1]) & 0xf0) {
	case _TP_MT_CR:	/* CR */
		rtn = np_recv_cr(np, q, p, e, mp);
		break;
	case _TP_MT_CC:	/* CC */
		rtn = np_recv_cc(np, q, p, e, mp);
		break;
	case _TP_MT_DR:	/* DR */
		rtn = np_recv_dr(np, q, p, e, mp);
		break;
	case _TP_MT_DC:	/* DC */
		rtn = np_recv_dc(np, q, p, e, mp);
		break;
	case _TP_MT_DT:	/* DT */
		rtn = np_recv_dt(np, q, p, e, mp);
		break;
	case _TP_MT_ED:	/* ED */
		rtn = np_recv_ed(np, q, p, e, mp);
		break;
	case _TP_MT_AK:	/* AK */
		rtn = np_recv_ak(np, q, p, e, mp);
		break;
	case _TP_MT_EA:	/* EA */
		rtn = np_recv_ea(np, q, p, e, mp);
		break;
	case _TP_MT_RJ:	/* RJ */
		rtn = np_recv_rj(np, q, p, e, mp);
		break;
	case _TP_MT_ER:	/* ER */
		rtn = np_recv_er(np, q, p, e, mp);
		break;
	case _TP_MT_UD:	/* UD */
		rtn = np_recv_ud(np, q, p, e, mp);
		break;
	default:
		goto bad_message_type;
	}
	return (rtn);
}

/*
 * ==========================================================================
 */

/*
 * TPI Provider -> TPI user primtiives (lower TCP TP)
 */

/**
 * np_conn_ind: - process received T_CONN_IND message
 * @np: network provider private structure
 * @q: active queue (read queue)
 * @mp: the T_CONN_IND message
 */
static fastcall noinline __unlikely int
np_conn_ind(struct np *np, queue_t *q, mblk_t *mp)
{
	struct T_conn_ind *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->SRC_offset, p->SRC_length))
		goto badaddr;
	if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
		goto badopt;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_conn_con: - process received T_CONN_CON message
 * @np: network provider private structure
 * @q: active queue (read queue)
 * @mp: the T_CONN_CON message
 *
 * ISO/IEC 8073:1997(E) Appendix A, Table A.6 State table for classes 0 and 2:
 *
 * This primitive is handled in state NS_WCON_CREQ (WFNC).
 *
 * The response is to send a CR-TPDU and then move to the TS_WCON_CREQ (WFCC)
 * state.
 */
static fastcall noinline __unlikely int
np_conn_con(struct np *np, queue_t *q, mblk_t *mp)
{
	struct T_conn_con *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->SRC_offset, p->SRC_length))
		goto badaddr;
	if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
		goto badopt;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_discon_ind: - process received T_DISCON_IND message
 * @np: network provider private structure
 * @q: active queue (read queue)
 * @mp: the T_DISCON_IND message
 *
 * ISO/IEC 8073:1997(E) Appendix A, Table A.6 State table for classes 0 and 2:
 *
 * NS_WCON_CREQ (WFNC):-
 * TS_WCON_CREQ (WFCC):-
 * TS_DATA_XFER (OPEN):-
 * TS_WRES_CIND (WFTRESP):-
 *	- issue T_DISCON_IND
 *	- move to state TS_IDLE (CLOSED)
 * TS_WCON_CREQ (WBCL):-
 * TS_WIND_ORDREL (CLOSING):-
 *	- move to state TS_IDLE (CLOSED)
 */
static fastcall noinline __unlikely int
np_discon_ind(struct np *np, queue_t *q, mblk_t *mp)
{
	struct T_discon_ind *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->SRC_offset, p->SRC_length))
		goto badaddr;
	if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
		goto badopt;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_data_ind: - process received T_DATA_IND message
 * @np: network provider private structure
 * @q: active queue (read queue)
 * @mp: the T_DATA_IND message
 */
static fastcall inline __hot_in int
np_data_ind(struct np *np, queue_t *q, mblk_t *mp)
{
	struct T_data_ind *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->SRC_offset, p->SRC_length))
		goto badaddr;
	if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
		goto badopt;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_exdata_ind: - process received T_EXDATA_IND message
 * @np: network provider private structure
 * @q: active queue (read queue)
 * @mp: the T_EXDATA_IND message
 */
static fastcall inline __hot_in int
np_exdata_ind(struct np *np, queue_t *q, mblk_t *mp)
{
	struct T_exdata_ind *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->SRC_offset, p->SRC_length))
		goto badaddr;
	if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
		goto badopt;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_info_ack: - process received T_INFO_ACK message
 * @np: network provider private structure
 * @q: active queue (read queue)
 * @mp: the T_INFO_ACK message
 */
static fastcall noinline __unlikely int
np_info_ack(struct np *np, queue_t *q, mblk_t *mp)
{
	struct T_info_ack *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->SRC_offset, p->SRC_length))
		goto badaddr;
	if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
		goto badopt;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_bind_ack: - process received T_BIND_ACK message
 * @np: network provider private structure
 * @q: active queue (read queue)
 * @mp: the T_BIND_ACK message
 */
static fastcall noinline __unlikely int
np_bind_ack(struct np *np, queue_t *q, mblk_t *mp)
{
	struct T_bind_ack *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->SRC_offset, p->SRC_length))
		goto badaddr;
	if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
		goto badopt;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_error_ack: - process received T_ERROR_ACK message
 * @np: network provider private structure
 * @q: active queue (read queue)
 * @mp: the T_ERROR_ACK message
 */
static fastcall noinline __unlikely int
np_error_ack(struct np *np, queue_t *q, mblk_t *mp)
{
	struct T_error_ack *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->SRC_offset, p->SRC_length))
		goto badaddr;
	if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
		goto badopt;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_ok_ack: - process received T_OK_ACK message
 * @np: network provider private structure
 * @q: active queue (read queue)
 * @mp: the T_OK_ACK message
 */
static fastcall noinline __unlikely int
np_ok_ack(struct np *np, queue_t *q, mblk_t *mp)
{
	struct T_ok_ack *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->SRC_offset, p->SRC_length))
		goto badaddr;
	if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
		goto badopt;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_unitdata_ind: - process received T_UNITDATA_IND message
 * @np: network provider private structure
 * @q: active queue (read queue)
 * @mp: the T_UNITDATA_IND message
 */
static fastcall inline __hot_in int
np_unitdata_ind(struct np *np, queue_t *q, mblk_t *mp)
{
	struct T_unitdata_ind *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->SRC_offset, p->SRC_length))
		goto badaddr;
	if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
		goto badopt;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_uderror_ind: - process received T_UDERROR_IND message
 * @np: network provider private structure
 * @q: active queue (read queue)
 * @mp: the T_UDERROR_IND message
 */
static fastcall noinline __unlikely int
np_uderror_ind(struct np *np, queue_t *q, mblk_t *mp)
{
	struct T_uderror_ind *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->SRC_offset, p->SRC_length))
		goto badaddr;
	if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
		goto badopt;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_optmgmt_ack: - process received T_OPTMGMT_ACK message
 * @np: network provider private structure
 * @q: active queue (read queue)
 * @mp: the T_OPTMGMT_ACK message
 */
static fastcall noinline __unlikely int
np_optmgmt_ack(struct np *np, queue_t *q, mblk_t *mp)
{
	struct T_optmgmt_ack *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->SRC_offset, p->SRC_length))
		goto badaddr;
	if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
		goto badopt;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_ordrel_ind: - process received T_ORDREL_IND message
 * @np: network provider private structure
 * @q: active queue (read queue)
 * @mp: the T_ORDREL_IND message
 */
static fastcall noinline __unlikely int
np_ordrel_ind(struct np *np, queue_t *q, mblk_t *mp)
{
	struct T_ordrel_ind *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->SRC_offset, p->SRC_length))
		goto badaddr;
	if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
		goto badopt;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_optdata_ind: - process received T_OPTDATA_IND message
 * @np: network provider private structure
 * @q: active queue (read queue)
 * @mp: the T_OPTDATA_IND message
 */
static fastcall inline __hot_in int
np_optdata_ind(struct np *np, queue_t *q, mblk_t *mp)
{
	struct T_optdata_ind *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->SRC_offset, p->SRC_length))
		goto badaddr;
	if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
		goto badopt;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_addr_ack: - process received T_ADDR_ACK message
 * @np: network provider private structure
 * @q: active queue (read queue)
 * @mp: the T_ADDR_ACK message
 */
static fastcall noinline __unlikely int
np_addr_ack(struct np *np, queue_t *q, mblk_t *mp)
{
	struct T_addr_ack *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->SRC_offset, p->SRC_length))
		goto badaddr;
	if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
		goto badopt;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * np_capability_ack: - process received T_CAPABILITY_ACK message
 * @np: network provider private structure
 * @q: active queue (read queue)
 * @mp: the T_CAPABILITY_ACK message
 */
static fastcall noinline __unlikely int
np_capability_ack(struct np *np, queue_t *q, mblk_t *mp)
{
	struct T_capability_ack *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->SRC_offset, p->SRC_length))
		goto badaddr;
	if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
		goto badopt;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/*
 * TPI User -> TPI Provider primitives (upper OSI TP)
 */

/**
 * tp_conn_req: - process received T_CONN_REQ message
 * @tp: transport provider private structure
 * @q: active queue (read queue)
 * @mp: the T_CONN_REQ message
 *
 * ISO/IEC 8073:1997(E) Appendix A, Table A.6 State table for classes 0 and 2:
 *
 * This primitives is only valid in state NS_IDLE (CLOSED).
 *
 * - P0: When the T-CONNECT request is unacceptable,
 *       - issue T_DISCON_IND (TDISind)
 *       - move to NS_IDLE (CLOSED) state.
 * - P2: When no network connection is available,
 *       - issue T_OK_ACK
 *       - move to TS_WCON_CREQ state (WFNC)
 *       - issue N_CONN_REQ (NCONreq)
 *       - np moves to NS_WCON_CREQ state
 * - P3: When network connection available and open,
 *       - issue T_OK_ACK
 *       - issue Connection Request (CR) message on NC
 *       - move to TS_WCON_CREQ state (WFCC)
 * - P4: When network connection available and open in progress,
 *	 - issue T_OK_ACK
 *	 - move to TS_WCON_CREQ state (WFNC)
 */
static fastcall noinline __unlikely int
tp_conn_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_conn_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->SRC_offset, p->SRC_length))
		goto badaddr;
	if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
		goto badopt;
	if (tp->np == NULL) {
		/* Iniator mode only.  Assign a network connection to this transport connection and 
		   bind the network connection to the local NSAP. */
	}
	if (np_get_state(tp->np) == TS_IDLE) {
		/* Bound network connection but not conencted yet.  Issue a N_CONN_REQ (T_CONN_REQ
		   in this case) and wait for the network connection to be formed. */
	}
	if (np_get_state(tp->np) == TS_WCON_CREQ) {
		/* Waiting for connection to be formed, just wait. */
	}
	if (np_get_state(tp->np) == TS_DATA_XFER) {
		/* Available, send T_OK_ACK, send CR and wait for CC. */
		tp_set_state(tp, TS_WCON_CREQ);
	}
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * tp_conn_res: - process received T_CONN_RES message
 * @tp: transport provider private structure
 * @q: active queue (read queue)
 * @mp: the T_CONN_RES message
 *
 * ISO/IEC 8073:1997(E) Appendix A, Table A.6 State table for classes 0 and 2:
 *
 * This primitive is only valid in state TS_WRES_CIND (WFTRESP), in which case
 * it always results in the TS_DATA_XFER (OPEN) state.
 */
static fastcall noinline __unlikely int
tp_conn_res(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_conn_res *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->SRC_offset, p->SRC_length))
		goto badaddr;
	if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
		goto badopt;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * tp_discon_req: - process received T_DISCON_REQ message
 * @tp: transport provider private structure
 * @q: active queue (read queue)
 * @mp: the T_DISCON_REQ message
 *
 * ISO/IEC 8073:1997(E) Appendix A, Table A.6 State table for classes 0 and 2:
 *
 * This primitive is valid in the following states:
 *
 * NS_WCON_CREQ (WFNC):-
 *	- move to the TS_IDLE (CLOSED) state
 *	- if the network connection is not being used by another transport
 *	  connection assigned to it, it may be disconnected.
 * TS_WCON_CREQ (WFCC):-
 *	- not P7: Class 0
 *	    - send N_DISCON_REQ
 *	    - move to state TS_IDLE (CLOSED)
 *	- P7: Class 2
 *	    - move to state TS_WCON_CREQ (WBCL) but wait for the CC-TPDU to
 *	      arrive before sending the DR-TPDU.
 * TS_DATA_XFER (OPEN):-
 *	- P5: Class 0 (Selected in CC)
 *	    - send N_DISCON_REQ
 *	    - moved to state TS_IDLE (CLOSED)
 *	- P7: Class 2
 *	    - send DR-TPDU
 *	    - move to TS_IDLE (CLOSING) state (more like TS_WIND_ORDREL)
 * TS_WRES_CIND (WFTRESP):-
 *	- send DR-TPDU
 *	- move to TS_IDLE (CLOSED) state
 */
static fastcall noinline __unlikely int
tp_discon_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_discon_req *p = (typeof(p)) mp->b_rptr;
	mblk_t *cp = NULL;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (tp->info.SERV_type == T_CLTS)
		goto notsupport;
	if (tp_get_state(tp) == TS_IDLE)
		goto discard;
	if (tp_not_state(tp, TSM_CONNECTED))
		goto oustate;
	if (mp->b_cont) {
		long mlen, mmax;

		mlen = msgdsize(mp);
		if (((mmax = tp->info.DDATA_size) > 0 && mlen > mmax) || mmax == T_INVALID
		    || ((mmax = tp->info.TIDU_size) > 0 && mlen > mmax) || mmax == T_INVALID)
			goto baddata;
		if (mlen == 0 && !(tp->info.PROVIDER_flag & T_SNDZERO))
			goto baddata;
	}
	switch (tp_get_state(tp)) {
	case TS_WRES_CIND:
		if (!(cp = tp_seq_check(tp, p->SEQ_number)))
			goto badseq;
		break;
	}
	switch (tp_get_state(tp)) {
	case TS_WCON_CREQ:
		/* Several possibilities here: 1) the network connection has not connected yet, or
		   has connected but we have not sent the CC yet, in which case, if we are Class 0
		   or Class 1, disconnect the NC and move to the TS_IDLE state, otherwise, if we
		   are Class 2 or Class 3, simply move to the TS_IDLE state; 2) the network
		   connection is OK and we have sent the CC-TPDU, in which case if we are Class 0
		   or Class 1, disconnect the NC and move to the TS_IDLE state, otherwise, if we
		   are Class 2 or Class 3, move to the TS_IDLE state but wait for the CC-TPDU
		   before sending a DR-TPDU. */
		switch (tp->opts.res.tco.prefclass) {
		case T_CLASS0:
		case T_CLASS1:
			/* send N_DISCON_REQ move to state TS_IDLE */
			if ((err = np_discon_req(tp->np, q, mp, 0, mp->b_cont)))
				goto error;
			break;
		case T_CLASS2:
		case T_CLASS3:
		case T_CLASS4:
			/* await CC-TPDU before sending DR-TPDU */
			break;
		}
		break;
	case TS_WRES_CIND:
	case TS_DATA_XFER:
	case TS_WIND_ORDREL:
	case TS_WREQ_ORDREL:
		switch (tp->opts.res.tco.prefclass) {
		case T_CLASS0:
		case T_CLASS1:
			/* send N_DISCON_REQ move to state TS_IDLE */
			if ((err = np_discon_req(tp->np, q, mp, 0, mp->b_cont)))
				goto error;
			break;
		case T_CLASS2:
		case T_CLASS3:
		case T_CLASS4:
			/* send DR-TPDU move to state TS_IDLE */
			if ((err = tp_send_dr(tp, q, mp)))
				goto error;
			break;
		}
		break;
	}
	switch (tp_get_state(tp)) {
	case TS_WCON_CREQ:
		tp_set_state(tp, TS_WACK_DREQ6);
		break;
	case TS_WRES_CIND:
		tp_set_state(tp, TS_WACK_DREQ7);
		break;
	case TS_DATA_XFER:
		tp_set_state(tp, TS_WACK_DREQ9);
		break;
	case TS_WIND_ORDREL:
		tp_set_state(tp, TS_WACK_DREQ10);
		break;
	case TS_WREQ_ORDREL:
		tp_set_state(tp, TS_WACK_DREQ11);
		break;
	}
	return tp_ok_ack(tp, q, mp, T_DISCON_REQ, 0);
      badseq:
	err = TBADSEQ;
	goto error;
      baddata:
	err = TBADDATA;
	goto error;
      oustate:
	err = TOUTSTATE;
	goto error;
      notsupport:
	err = TNOTSUPPORT;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return tp_error_ack(tp, q, mp, T_DISCON_REQ, err, __FUNCTION__);
      discard:
	freemsg(mp);
	return (0);
}

/**
 * tp_data_req: - process received T_DATA_REQ message
 * @tp: transport provider private structure
 * @q: active queue (read queue)
 * @mp: the T_DATA_REQ message
 *
 * ISO/IEC 8073:1997(E) Appendix A, Table A.6 State table for classes 0 and 2:
 *
 * This primitive is only valid in the TS_DATA_XFER (OPEN) state.
 *	- stay in the OPEN state
 *	- see data transfer procedure of the class
 */
static fastcall inline __hot_put int
tp_data_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_data_req *p = (typeof(p)) mp->b_rptr;
	long mlen, mmax;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (tp->info.SERV_type == T_CLTS)
		goto notsupport;
	if (tp_get_state(tp) == TS_IDLE)
		goto discard;
	if (tp_not_state(tp, TSM_OUTDATA))
		goto outstate;
	mlen = msgdsize(mp);
	if (((mmax = tp->info.TSDU_size) > 0 && mlen > mmax) || mmax == T_INVALID
	    || ((mmax = tp->info.TIDU_size) > 0 && mlen > mmax) || mmax == T_INVALID)
		goto baddata;
	if (mlen == 0 && !(tp->info.PROVIDER_flag & T_SNDZERO))
		goto baddata;
	return tp_send_dt(tp, q, mp, p->MORE_flag & T_MORE, mp->b_cont);
      baddata:
	err = TBADDATA;
	goto error;
      outstate:
	err = TOUTSTATE;
	goto error;
      notsupport:
	err = TNOTSUPPORT;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * tp_exdata_req: - process received T_EXDATA_REQ message
 * @tp: transport provider private structure
 * @q: active queue (read queue)
 * @mp: the T_EXDATA_REQ message
 *
 * ISO/IEC 8073:1997(E) Appendix A, Table A.6 State table for classes 0 and 2:
 *
 * This primitive is only valid in the TS_DATA_XFER (OPEN) state, and is only
 * valid for class 2  operation.
 *	- stay in the OPEN state
 *	- see expedited data transfer procedure of the class
 */
static fastcall inline __hot_put int
tp_exdata_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_exdata_req *p = (typeof(p)) mp->b_rptr;
	long mlen, mmax;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (tp->info.SERV_type == T_CLTS)
		goto notsupport;
	if (!(tp->addopts & _T_F_AO_EXPD))
		goto notsupport;
	if (p->MORE_flag & T_MORE)
		goto badflag;
	if (tp_get_state(tp) == TS_IDLE)
		goto discard;
	if (tp_not_state(tp, TSM_OUTDATA))
		goto outstate;
	mlen = msgdsize(mp);
	if (((mmax = tp->info.ETSDU_size) > 0 && mlen > mmax) || mmax == T_INVALID
	    || ((mmax = tp->info.TIDU_size) > 0 && mlen > mmax) || mmax == T_INVALID)
		goto baddata;
	if (mlen == 0 && !(tp->info.PROVIDER_flag & T_SNDZERO))
		goto baddata;
	return tp_send_ed(tp, q, mp, mp->b_cont);
      badflag:
	err = TBADFLAG;
	goto error;
      baddata:
	err = TBADDATA;
	goto error;
      outstate:
	err = TOUTSTATE;
	goto error;
      notsupport:
	err = TNOTSUPPORT;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return tp_m_error(tp, q, mp, err, __FUNCTION__);
      discard:
	freemsg(mp);
	return (0);
}

/**
 * tp_info_req: - process received T_INFO_REQ message
 * @tp: transport provider private structure
 * @q: active queue (read queue)
 * @mp: the T_INFO_REQ message
 */
static fastcall noinline __unlikely int
tp_info_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_info_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->SRC_offset, p->SRC_length))
		goto badaddr;
	if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
		goto badopt;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * tp_bind_req: - process received T_BIND_REQ message
 * @tp: transport provider private structure
 * @q: active queue (read queue)
 * @mp: the T_BIND_REQ message
 */
static fastcall noinline __unlikely int
tp_bind_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_bind_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->SRC_offset, p->SRC_length))
		goto badaddr;
	if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
		goto badopt;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * tp_unbind_req: - process received T_UNBIND_REQ message
 * @tp: transport provider private structure
 * @q: active queue (read queue)
 * @mp: the T_UNBIND_REQ message
 */
static fastcall noinline __unlikely int
tp_unbind_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_unbind_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->SRC_offset, p->SRC_length))
		goto badaddr;
	if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
		goto badopt;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * tp_unitdata_req: - process received T_UNITDATA_REQ message
 * @tp: transport provider private structure
 * @q: active queue (read queue)
 * @mp: the T_UNITDATA_REQ message
 */
static fastcall inline __hot_put int
tp_unitdata_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_unitdata_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->SRC_offset, p->SRC_length))
		goto badaddr;
	if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
		goto badopt;
	goto discard;
      badopt:
	goto discard;
      badaddr:
	goto discard;
      tooshort:
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * tp_optmgmt_req: - process received T_OPTMGMT_REQ message
 * @tp: transport provider private structure
 * @q: active queue (read queue)
 * @mp: the T_OPTMGMT_REQ message
 */
static fastcall noinline __unlikely int
tp_optmgmt_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_optmgmt_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
		goto badopt;
	if (tp_get_state(tp) == TS_IDLE)
		tp_set_state(tp, TS_WACK_OPTREQ);
	switch (p->MGMT_flags) {
	case T_DEFAULT:
		err = tp_size_default_options(tp, mp->b_rptr + p->OPT_offset, p->OPT_length);
	case T_CURRENT:
		err = tp_size_current_options(tp, mp->b_rptr + p->OPT_offset, p->OPT_length);
	case T_CHECK:
		err = tp_size_check_options(tp, mp->b_rptr + p->OPT_offset, p->OPT_length);
	case T_NEGOTIATE:
		err = tp_size_negotiate_options(tp, mp->b_rptr + p->OPT_offset, p->OPT_length);
	default:
		goto badflag;
	}
	if (err < 0) {
		switch (-err) {
		case EINVAL:
			goto badopt;
		case EACCES:
			goto acces;
		default:
			goto provspec;
		}
	}
	err = tp_optmgmt_ack(tp, q, mp, mp->b_rptr + p->OPT_offset, p->OPT_length, p->MGMT_flags);
	if (err < 0) {
		switch (-err) {
		case EINVAL:
			goto badopt;
		case EACCES:
			goto acces;
		case ENOBUFS:
		case ENOMEM:
		case EBUSY:
		case EAGAIN:
		case EDEADLK:
			break;
		default:
			goto provspec;
		}
	}
	return (err);
      provspec:
	err = err;
	goto error;
      acces:
	err = TACCES;
	goto error;
      badflag:
	err = TBADFLAG;
	goto error;
      badopt:
	err = TBADOPT;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return tp_error_ack(tp, q, mp, T_OPTMGMT_REQ, err, __FUNCTION__);
}

/**
 * tp_ordrel_req: - process received T_ORDREL_REQ message
 * @tp: transport provider private structure
 * @q: active queue (read queue)
 * @mp: the T_ORDREL_REQ message
 */
static fastcall noinline __unlikely int
tp_ordrel_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_ordrel_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (tp->info.SERV_type != T_COTS_ORD)
		goto notsupport;
	if (tp_not_state(tp, TSM_OUTDATA))
		goto outstate;
	if (mp->b_cont) {
		long mlen, mmax;

		mlen = msgdsize(mp);
		if (((mmax = tp->info.DDATA_size) > 0 && mlen > mmax) || mmax == T_INVALID
		    || ((mmax = tp->info.TIDU_size) > 0 && mlen > mmax) || mmax == T_INVALID)
			goto baddata;
		if (mlen == 0 && !(tp->info.PROVIDER_flag & T_SNDZERO))
			goto baddata;
	}
	switch (tp_get_state(tp)) {
	case TS_DATA_XFER:
		tp_set_state(tp, TS_WIND_ORDREL);
		/* Only class 2, 3, 4 allows this */
		if ((err = tp_send_dr(tp, q, mp, mp->b_cont)))
			goto error;
		break;
	case TS_WREQ_ORDREL:
		tp_set_state(tp, TS_IDLE);
		/* Only class 2, 3, 4 allows this */
		if ((err = tp_send_dc(tp, q, mp, mp->b_cont)))
			goto error;
		break;
	default:
		goto outstate;
	}
	return (0);
      baddata:
	err = TOUTSTATE;
	goto error;
      outstate:
	err = TOUTSTATE;
	goto error;
      notsupport:
	err = TNOTSUPPORT;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return tp_m_error(tp, q, mp, err, __FUNCTION__);
}

/**
 * tp_optdata_req: - process received T_OPTDATA_REQ message
 * @tp: transport provider private structure
 * @q: active queue (read queue)
 * @mp: the T_OPTDATA_REQ message
 */
static fastcall inline __hot_put int
tp_optdata_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_optdata_req *p = (typeof(p)) mp->b_rptr;
	long mlen, mmax;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
		goto badopt;
	if (tp->info.SERV_type == T_CLTS)
		goto notsupport;
	if (p->DATA_flag & T_ODF_EX) {
		if (!(tp->addopts & _T_F_AO_EXPD))
			goto notsupport;
		if (p->DATA_flag & T_ODF_MORE)
			goto badflag;
	}
	if (tp_get_state(tp) == TS_IDLE)
		goto discard;
	if (tp_not_state(tp, TSM_OUTDATA))
		goto outstate;
	mlen = msgdsize(mp);
	if (p->DATA_flag & T_ODF_EX) {
		if (((mmax = tp->info.ETSDU_size) > 0 && mlen > mmax) || mmax == T_INVALID)
			goto baddata;
	} else {
		if (((mmax = tp->info.TSDU_size) > 0 && mlen > mmax) || mmax == T_INVALID)
			goto baddata;
	}
	if (((mmax = tp->info.TIDU_size) > 0 && mlen > mmax) || mmax == T_INVALID)
		goto baddata;
	if (mlen == 0 && !(tp->info.PROVIDER_flag & T_SNDZERO))
		goto baddata;
	/* FIXME: process options */
	if (p->DATA_flag & T_ODF_EX) {
		if ((err = tp_send_ed(tp, q, mp, p->DATA_flag & T_ODF_MORE, mp->b_cont)))
			goto error;
	} else {
		if ((err = tp_send_dt(tp, q, mp, p->DATA_flag & T_ODF_MORE, mp->b_cont)))
			goto error;
	}
	return (err);
      baddata:
	err = TBADDATA;
	goto error;
      badflag:
	err = TBADFLAG;
	goto error;
      outstate:
	err = TOUTSTATE;
	goto error;
      discard:
	freemsg(mp);
	return (0);
      notsupport:
	err = TNOTSUPPORT;
	goto error;
      badopt:
	err = TBADOPT;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return tp_m_error(tp, q, mp, err, __FUNCTION__);
}

/**
 * tp_addr_req: - process received T_ADDR_REQ message
 * @tp: transport provider private structure
 * @q: active queue (read queue)
 * @mp: the T_ADDR_REQ message
 */
static fastcall noinline __unlikely int
tp_addr_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_addr_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	return tp_addr_ack(tp, q, mp, tp->src.buf, tp->src.len, tp->dst.buf, tp->dst.len);
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return tp_error_ack(tp, q, mp, T_ADDR_REQ, err, __FUNCTION__);
}

/**
 * tp_capability_req: - process received T_CAPABILITY_REQ message
 * @tp: transport provider private structure
 * @q: active queue (read queue)
 * @mp: the T_CAPABILITY_REQ message
 */
static fastcall noinline __unlikely int
tp_capability_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_capability_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	return tp_capability_ack(tp, q, mp);
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return tp_error_ack(tp, q, mp, T_CAPABILITY_ACK, err, __FUNCTION__);
}

/*
 * STREAMS Message Handling
 * ========================
 */

/**
 * np_m_data: - process M_DATA message
 * @q: active queue (read queue)
 * @mp: the M_DATA message
 */
static fastcall inline __hot_read int
np_m_data(queue_t *q, mblk_t *mp)
{
}

/**
 * np_m_proto: - process M_PROTO or M_PCPROTO message
 * @q: active queue (read queue)
 * @mp: the M_PROTO or M_PCPROTO message
 */
static fastcall inline __hot_read int
np_m_proto(queue_t *q, mblk_t *mp)
{
	t_scalar_t prim;
	struct np *np;
	caddr_t priv;
	int rtn;

	if (!MBLKIN(mp, 0, sizeof(prim)))
		goto tooshort;
	prim = *(t_scalar_t *) mp->b_rptr;
	if ((priv = mi_trylock(q)) == -NULL)
		goto edeadlk;
	np = NP_PRIV(q);
	np_save_state(np);
	switch (prim) {
	case T_CONN_IND:
		rtn = np_conn_ind(np, q, mp);
		break;
	case T_CONN_CON:
		rtn = np_conn_con(np, q, mp);
		break;
	case T_DISCON_IND:
		rtn = np_discon_ind(np, q, mp);
		break;
	case T_DATA_IND:
		rtn = np_data_ind(np, q, mp);
		break;
	case T_EXDATA_IND:
		rtn = np_exdata_ind(np, q, mp);
		break;
	case T_INFO_ACK:
		rtn = np_info_ack(np, q, mp);
		break;
	case T_BIND_ACK:
		rtn = np_bind_ack(np, q, mp);
		break;
	case T_ERROR_ACK:
		rtn = np_error_ack(np, q, mp);
		break;
	case T_OK_ACK:
		rtn = np_ok_ack(np, q, mp);
		break;
	case T_UNITDATA_IND:
		rtn = np_unitdata_ind(np, q, mp);
		break;
	case T_UDERROR_IND:
		rtn = np_uderror_ind(np, q, mp);
		break;
	case T_OPTMGMT_ACK:
		rtn = np_optmgmt_ack(np, q, mp);
		break;
	case T_ORDREL_IND:
		rtn = np_ordrel_ind(np, q, mp);
		break;
	case T_OPTDATA_IND:
		rtn = np_optdata_ind(np, q, mp);
		break;
	case T_ADDR_ACK:
		rtn = np_addr_ack(np, q, mp);
		break;
	case T_CAPABILITY_ACK:
		rtn = np_capability_ack(np, q, mp);
		break;
/*
 * None of the following messages should appear at the read-side queue.  Only
 * upward defined TPI messages should appear at the read-side queue.  Discard
 * these.
 */
	case T_CONN_REQ:
	case T_CONN_RES:
	case T_DISCON_REQ:
	case T_DATA_REQ:
	case T_EXDATA_REQ:
	case T_INFO_REQ:
	case T_BIND_REQ:
	case T_UNBIND_REQ:
	case T_UNITDATA_REQ:
	case T_OPTMGMT_REQ:
	case T_ORDREL_REQ:
	case T_OPTDATA_REQ:
	case T_ADDR_REQ:
	case T_CAPABILITY_REQ:
		goto wrongway;
	default:
		rtn = np_other_ind(np, q, mp);
		break;
	}
	if (rtn) {
		np_restore_state(np);
	}
	mi_unlock(priv);
	return (rtn);
      wrongway:
	mi_unlock(priv);
	freemsg(mp);
	mi_strlog(q, 0, SL_ERROR | SL_TRACE, "%s: discarding message in wrong direction",
		  __FUNCTION__);
	return (0);
      edeadlk:
	return (-EDEADLK);
      tooshort:
	freemsg(mp);
	mi_strlog(q, 0, SL_ERROR | SL_TRACE, "%s: discarding primitive too short", __FUNCTION__);
	return (0);
}

/**
 * np_m_sig: - process M_SIG or M_PCSIG message
 * @q: active queue (read queue)
 * @mp: the M_SIG or M_PCSIG message
 */
static fastcall noinline __unlikely int
np_m_sig(queue_t *q, mblk_t *mp)
{
	struct np *np;
	caddr_t *priv;
	int rtn;

	if (!MBLKIN(mp, 0, sizeof(int)))
		goto passalong;
	if ((priv = mi_trylock(q)) == NULL)
		return (mi_timer_requeue(mp) ? -EDEADLK : 0);
	if (!mi_timer_valid(mp))
		return (0);
	np = NP_PRIV(q);
	np_save_state(np);
	switch (*(int *) mp->b_rptr) {
#if 0-
	case 1:
		mi_strlog(q, STRLOGTO, SL_TRACE, "-> T1 TIMEOUT <-");
		rtn = np_t1_timeout(np, q, mp);
		break;
#endif
	default:
		mi_strlog(q, 0, SL_ERROR | SL_TRACE, "%s: discarding undefined timeout %d",
			  __FUNCTION__, *(int *) mp->b_rptr);
		rtn = 0;
		break;
	}
	if (rtn) {
		np_restore_state(np);
		rtn = mi_timer_requeue(mp) ? rtn : 0;
	}
	mi_unlock(priv);
	return (rtn);
      passalong:
	/* These are either timer messages (in which case they have a length greater than or equal
	   to 4) or true signal messages (in which case they have a length of 1). Therefore, if the 
	   length of the message is less than sizeof(int) we pass them along. */
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/**
 * np_m_copy: - process M_COPYIN or M_COPYOUT message
 * @q: active queue (read queue)
 * @mp: the M_COPYIN or M_COPYOUT message
 */
static fastcall noinline __unlikely int
np_m_copy(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/**
 * np_m_iocack: - process M_IOCACK or M_IOCNAK message
 * @q: active queue (read queue)
 * @mp: the M_IOCACK or M_IOCNAK message
 */
static fastcall noinline __unlikely int
np_m_iocack(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/**
 * np_m_flush: - process M_FLUSH message
 * @q: active queue (read queue)
 * @mp: the M_FLUSH message
 */
static fastcall noinline __unlikely int
np_m_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
	}
	putnext(q, mp);
	return (0);
}

/**
 * np_m_error: - process M_ERROR or M_HANGUP message
 * @q: active queue (read queue)
 * @mp: the M_ERROR or M_HANGUP message
 */
static fastcall noinline __unlikely int
np_m_error(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/**
 * np_m_unrec: - process unrecognized STREAMS message
 * @q: active queue (read queue)
 * @mp: the unrecognized STREAMS message
 */
static fastcall noinline __unlikely int
np_m_unrec(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/**
 * np_msg: - process STREAMS message
 * @q: active queue (write queue)
 * @mp: the STREAMS message
 */
static fastcall inline int
np_msg(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
		return np_m_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return np_m_proto(q, mp);
	case M_SIG:
	case M_PCSIG:
		return np_m_sig(q, mp);
	case M_COPYIN:
	case M_COPYOUT:
		return np_m_copy(q, mp);
	case M_IOCACK:
	case M_IOCNAK:
		return np_m_iocack(q, mp);
	case M_FLUSH:
		return np_m_flush(q, mp);
	case M_ERROR:
	case M_HANGUP:
		return np_m_error(q, mp);
	default:
		return np_m_unrec(q, mp);
	}
}

/**
 * tp_m_data: - process M_DATA message
 * @q: active queue (read queue)
 * @mp: the M_DATA message
 */
static fastcall inline __hot_write
tp_m_data(queue_t *q, mblk_t *mp)
{
}

/**
 * tp_m_proto: - process M_PROTO or M_PCPROTO message
 * @q: active queue (read queue)
 * @mp: the M_PROTO or M_PCPROTO message
 */
static fastcall inline __hot_write
tp_m_proto(queue_t *q, mblk_t *mp)
{
	t_scalar_t prim;
	struct tp *tp;
	caddr_t priv;
	int rtn;

	if (!MBLKIN(mp, 0, sizeof(prim)))
		goto tooshort;
	prim = *(t_scalar_t *) mp->b_rptr;
	if ((priv = mi_trylock(q)) == NULL)
		goto edeadlk;
	tp = TP_PRIV(q);
	tp_save_state(tp);
	np_save_state(tp->np);
	switch (prim) {
	case T_CONN_REQ:
		rtn = tp_conn_req(tp, q, mp);
		break;
	case T_CONN_RES:
		rtn = tp_conn_res(tp, q, mp);
		break;
	case T_DISCON_REQ:
		rtn = tp_discon_req(tp, q, mp);
		break;
	case T_DATA_REQ:
		rtn = tp_data_req(tp, q, mp);
		break;
	case T_EXDATA_REQ:
		rtn = tp_exdata_req(tp, q, mp);
		break;
	case T_INFO_REQ:
		rtn = tp_info_req(tp, q, mp);
		break;
	case T_BIND_REQ:
		rtn = tp_bind_req(tp, q, mp);
		break;
	case T_UNBIND_REQ:
		rtn = tp_unbind_req(tp, q, mp);
		break;
	case T_UNITDATA_REQ:
		rtn = tp_unitdata_req(tp, q, mp);
		break;
	case T_OPTMGMT_REQ:
		rtn = tp_optmgmt_req(tp, q, mp);
		break;
	case T_ORDREL_REQ:
		rtn = tp_ordrel_req(tp, q, mp);
		break;
	case T_OPTDATA_REQ:
		rtn = tp_optdata_req(tp, q, mp);
		break;
	case T_ADDR_REQ:
		rtn = tp_addr_req(tp, q, mp);
		break;
	case T_CAPABILITY_REQ:
		rtn = tp_capability_req(tp, q, mp);
		break;
/*
 * None of the following messages should appear at the write-side queue.  Only
 * downwardcaseTPI messages should appear at the write-side queue.  Discard
 * these.
 */
	case T_CONN_IND:
	case T_CONN_CON:
	case T_DISCON_IND:
	case T_DATA_IND:
	case T_EXDATA_IND:
	case T_INFO_ACK:
	case T_BIND_ACK:
	case T_ERROR_ACK:
	case T_OK_ACK:
	case T_UNITDATA_IND:
	case T_UDERROR_IND:
	case T_OPTMGMT_ACK:
	case T_ORDREL_IND:
	case T_OPTDATA_IND:
	case T_ADDR_ACK:
	case T_CAPABILITY_ACK:
		goto wrongway;
	default:
		rtn = tp_other_req(tp, q, mp);
		break;
	}
	if (rtn) {
		np_restore_state(tp->np);
		tp_restore_state(tp);
	}
	mi_unlock(priv);
	return (rtn);
      wrongway:
	mi_unlock(priv);
	freemsg(mp);
	mi_strlog(q, 0, SL_ERROR | SL_TRACE, "%s: discarding message in wrong direction",
		  __FUNCTION__);
	return (0);
      edeadlk:
	return (-EDEADLK);
      tooshort:
	mi_strlog(q, 0, SL_ERROR | SL_TRACE, "%s: discarding primitive too short", __FUNCTION__);
	freemsg(mp);
	return (0);
}

/**
 * tp_m_sig: - process M_SIG or M_PCSIG message
 * @q: active queue (read queue)
 * @mp: the M_SIG or M_PCSIG message
 */
static fastcall noinline __unlikely
tp_m_sig(queue_t *q, mblk_t *mp)
{
	struct tp *tp;
	caddr_t priv;
	int rtn;

	if (!MBLKIN(mp, 0, sizeof(int)))
		goto passalong;
	if ((priv = mi_trylock(q)) == NULL)
		goto edeadlk;
	if (!mi_timer_valid(mp))
		goto done;
	tp = TP_PRIV(q);
	tp_save_state(tp);
	switch (*(int *) mp->b_rptr) {
	case 1:
		mi_strlog(q, STRLOGTO, SL_TRACE, "-> T1 TIMEOUT <-");
		rtn = tp_t1_timeout(tp, q, mp);
		break;
	default:
		mi_strlog(q, 0, SL_ERROR, "%s: discarding undefined timeout %d", __FUNCTION__,
			  *(int *) mp->b_rptr);
		rtn = 0;
		break;
	}
	if (rtn) {
		tp_restore_state(tp);
		rtn = mi_timer_requeue(mp) ? rtn : 0;
	}
	mi_unlock(priv);
	return (rtn);
      edeadlk:
	if (mi_timer_requeue(mp))
		return (-EDEADLK);
      done:
	return (0);
      passalong:
	/* These are either timer messages (in which case they have a length greater than or equal
	   to 4) or true signal messages (in which case they have a length of 1). Therefore, if the 
	   length of the message is less than sizeof(int) we pass them along. */
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/**
 * tp_m_ioctl: - process M_IOCTL message
 * @q: active queue (read queue)
 * @mp: the M_IOCTL message
 */
static fastcall noinline __unlikely
tp_m_ioctl(queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct tp *tp;
	caddr_t priv;
	int err;

	if (_IOC_TYPE(ioc->ioc_cmd) != OTK6_IOC_MAGIC)
		goto notforus;
	if (!mp->b_cont)
		goto efault;
	if ((priv = mi_trylock(q)) == NULL)
		goto edeadlk;
	tp = TP_PRIV(q);
	err = tp_ioctl(tp, q, mp);
	mi_unlock(priv);
	return (err);
      edeadlk:
	return (-EDEADLK);
      efault:
	mi_copy_done(q, mp, EFAULT);
	return (0);
      notforus:
	putnext(q, mp);
	return (0);
}

/**
 * tp_m_iocdata: - process M_IOCDATA message
 * @q: active queue (read queue)
 * @mp: the M_IOCDATA message
 */
static fastcall noinline __unlikely
tp_m_iocdata(queue_t *q, mblk_t *mp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	struct tp *tp;
	caddr_t priv;
	mblk_t *dp;
	int err;

	if (_IOC_TYPE(cp->cp_cmd) != OTK6_IOC_MAGIC)
		goto notforus;
	if (!mp->b_cont)
		goto efault;
	if ((priv = mi_trylock(q)) == NULL)
		goto edeadlk;
	tp = TP_PRIV(q);
	switch (mi_copy_state(q, mp, &dp)) {
	case MI_COPY_CASE(MI_COPY_IN, 1):
		err = tp_copyin1(tp, q, mp, dp);
		break;
	case MI_COPY_CASE(MI_COPY_IN, 2):
		err = tp_copyin2(tp, q, mp, dp);
		break;
	case MI_COPY_CASE(MI_COPY_OUT, 1):
		err = tp_copyout(tp, q, mp, dp);
		break;
	default:
		mi_copy_done(q, mp, EPROTO);
		err = 0;
		break;
	}
	mi_unlock(priv);
	return (err);
      edeadlk:
	return (-EDEADLK);
      efault:
	mi_copy_done(q, mp, EFAULT);
	return (0);
      notforus:
	/* Let driver deal with these. */
	putnext(q, mp);
	return (0);
}

/**
 * tp_m_flush: - process M_FLUSH message
 * @q: active queue (read queue)
 * @mp: the M_FLUSH message
 */
static fastcall noinline __unlikely
tp_m_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
	}
	putnext(q, mp);
	return (0);
}

/**
 * tp_m_unrec: - process unrecognized STREAMS message
 * @q: active queue (read queue)
 * @mp: the unrecognized STREAMS message
 */
static fastcall noinline __unlikely
tp_m_unrec(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/**
 * tp_msg: - process STREAMS message
 * @q: active queue (write queue)
 * @mp: the STREAMS message
 */
static streams_fastcall streams_inline int
tp_msg(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
		return tp_m_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return tp_m_proto(q, mp);
	case M_SIG:
	case M_PCSIG:
		return tp_m_sig(q, mp);
	case M_IOCTL:
		return tp_m_ioctl(q, mp);
	case M_IOCDATA:
		return tp_m_iocdata(q, mp);
	case M_FLUSH:
		return tp_m_flush(q, mp);
	default:
		return tp_m_unrec(q, mp);
	}
}

/*
 * Queue put and service procedures
 * ================================
 */

static streamscall __hot_write int
np_put(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY)))
	    || np_msg(q, mp))
		putq(q, mp);
	return (0);
}
static streamscall __hot_out int
np_srv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (np_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}
static streamscall __hot_read int
tp_srv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (tp_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}
static streamscall __hot_in int
tp_put(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY)))
	    || tp_msg(q, mp))
		putq(q, mp);
	return (0);
}

/*
 * Queue open and close routines
 * =============================
 */

static caddr_t tp_opens = NULL;
static caddr_t np_links = NULL;

static streamscall int
tp_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *credp)
{
	struct tp *tp;

	if (q->q_ptr)
		return (0);	/* already open */
	if ((err = mi_open_comm(&tp_opens, sizeof(*p), q, devp, oflags, sflag, crp)))
		return (err);
	tp = TP_PRIV(q);
	tp->np = NULL;
	tp->oq = RD(q);
	tp->state = TS_UNBND;
	tp->oldstate = TS_UNBND;
	tp->n_prov = N_CONS;
	tp->class = 0;
	tp->optoins = 0;
	tp->addopts = 0;
	tp->info.PRIM_type = T_INFO_ACK;
	tp->info.TSDU_size = 4096;
	tp->info.ETSDU_size = 16;
	tp->info.CDATA_size = 4096;
	tp->info.DDATA_size = 4096;
	tp->info.ADDR_size = 128;
	tp->info.OPT_size = 4096;
	tp->info.TIDU_size = 4096;
	tp->info.SERV_type = T_COTS;
	tp->info.CURRENT_state = TS_UNBND;
	tp->info.PROVIDER_flag = XPG4_1;
	bzero(tp->src, sizeof(tp->src));
	bzero(tp->dst, sizeof(tp->dst));
	tp->opts.req = t_defaults;
	tp->opts.res = t_defaults;

	qprocson(q);
	putnext(q, mp);
	return (0);
}

static streamscall int
tp0_qclose(queue_t *q, int oflags, cred_t *crp)
{
	struct tp *tp = TP_PRIV(q);

	(void) tp;
	qprocsoff(q);
	/* FIXME free timers and other things */
	mi_close_comm(&tp_opens, q);
	return (0);
}

/*
 * STREAMS Initialization
 * ======================
 */

static struct qinit tp_rinit = {
	.qi_putp = &tp_rput,
	.qi_srvp = &tp_rsrv,
	.qi_qopen = &tp_qopen,
	.qi_qclose = &tp_qclose,
	.qi_minfo = &tp_minfo,
	.qi_mstat = &tp_rstat,
};

static struct qinit tp_winit = {
	.qi_putp = &tp_wput,
	.qi_srvp = &tp_wsrv,
	.qi_minfo = &tp_minfo,
	.qi_mstat = &tp_wstat,
};

static struct qinit np_rinit = {
	.qi_putp = &np_rput,
	.qi_srvp = &np_rsrv,
	.qi_minfo = &np_minfo,
	.qi_mstat = &np_rstat,
};

static struct qinit np_winit = {
	.qi_putp = &np_wput,
	.qi_srvp = &np_wsrv,
	.qi_minfo = &np_minfo,
	.qi_mstat = &np_wstat,
};

STATIC struct streamtab otk6_info = {
	.st_rdinit = &tp_rinit,
	.st_wrinit = &tp_winit,
	.st_muxrinit = &np_rinit,
	.st_muxwinit = &np_winit,
};

static modID_t modid = MOD_ID;
static major_t major = CMAJOR;

/*
 * Linux kernel module initialization
 * ==================================
 */

#ifdef LINUX

#ifndef module_param
MODULE_PARM(modid, "h");
#else				/* module_param */
module_param(modid, ushort, 0444);
module_param(major, ushort, 0444);
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for TP0. (0 for allocation.)");
MODULE_PARM_DESC(major, "Major device number for TP0. (0 for allocation.)");

/*
 * Linux Fast-STREAMS Registration
 */

#ifdef LFS

static struct cdevsw tp_cdev = {
	.d_str = &otk6_info,
	.d_flag = D_MP | D_CLONE,
	.d_fop = NULL,
	.d_mode = S_IFCHR | S_IRUGO | S_IWUGO,
	.d_kmod = THIS_MODULE,
};

static int
otk6_register_strdev(major_t major)
{
	int err;

	if ((err = register_strdev(&tp_cdev, major)) < 0)
		return (err);
	return (0);
}

static int
otk6_unregister_strdev(major_t major)
{
	int err;

	if ((err = unregister_strdev(&tp_cdev, major)) < 0)
		return (err);
	return (0);
}

#endif				/* LFS */

#ifdef LIS

static int
otk6_register_strdev(major_t major)
{
	int err;

	if ((err = lis_register_strdev(major, &tp_info, UNITS, DRV_NAME)) < 0)
		return (err);
	if (major == 0)
		major = err;
	if ((err = lis_register_driver_qlock_option(major, LIS_QLOCK_NONE)) < 0) {
		lis_unregister_strdev(major);
		return (err);
	}
	return (0);
}

static int
otk6_unregister_strdev(major_t major)
{
	int err;

	if ((err = lis_unregister_strdev(major)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

static __init int
otk6_modinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);
	if ((err = otk6_register_strdev(major)) < 0) {
		cmn_err(CE_WARN, "%s: could not register driver major %d", DRV_NAME, (int) major);
		return (err);
	}
	return (0);
}

static __exit void
otk6_modexit(void)
{
	int err;

	if ((err = otk6_unregister_strmod(major)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister driver, err = %d", DRV_NAME, err);
		return;
	}
	return;
}

module_init(tp0_modinit);
module_exit(tp0_modexit);

#endif				/* LINUX */
