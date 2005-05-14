/*****************************************************************************

 @(#) $RCSfile: sctp_t.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2005/05/14 08:31:06 $

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

 Last Modified $Date: 2005/05/14 08:31:06 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: sctp_t.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2005/05/14 08:31:06 $"

static char const ident[] = "$RCSfile: sctp_t.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2005/05/14 08:31:06 $";

#define __NO_VERSION__

#include "os7/compat.h"

#if 0
#include <sys/tpi.h>
#include <sys/tpi_sctp.h>
#else
#include <sys/tihdr.h>
#endif
#include <sys/xti_ip.h>
#include <sys/xti_sctp.h>

#ifndef sctp_addr_t
typedef struct sctp_addr {
	uint16_t port __attribute__ ((packed));
	uint32_t addr[0] __attribute__ ((packed));
} sctp_addr_t;
#define sctp_addr_t sctp_addr_t
#endif				/* sctp_addr_t */

#include "sctp.h"
#include "sctp_defs.h"
#include "sctp_hash.h"
#include "sctp_cache.h"
#include "sctp_msg.h"

#include "sctp_t.h"

#define SCTP_T_CMINORS 255

#undef min
#define min lis_min
#undef max
#define max lis_max

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 *  This driver defines two user interfaces: one NPI, the other TPI.
 */

STATIC struct module_info sctp_t_minfo = {
	mi_idnum:0,			/* Module ID number */
	mi_idname:"sctp",		/* Module name */
	mi_minpsz:0,			/* Min packet size accepted */
	mi_maxpsz:INFPSZ,		/* Max packet size accepted */
	mi_hiwat:1 << 15,		/* Hi water mark */
	mi_lowat:1 << 10,		/* Lo water mark */
};

STATIC int sctp_t_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC int sctp_t_close(queue_t *, int, cred_t *);

STATIC int sctp_t_rput(queue_t *, mblk_t *);
STATIC int sctp_t_rsrv(queue_t *);

STATIC struct qinit sctp_t_rinit = {
	qi_putp:sctp_t_rput,		/* Read put (msg from below) */
	qi_srvp:sctp_t_rsrv,		/* Read queue service */
	qi_qopen:sctp_t_open,		/* Each open */
	qi_qclose:sctp_t_close,		/* Last close */
	qi_minfo:&sctp_t_minfo,		/* Information */
};

STATIC int sctp_t_wput(queue_t *, mblk_t *);
STATIC int sctp_t_wsrv(queue_t *);

STATIC struct qinit sctp_t_winit = {
	qi_putp:sctp_t_wput,		/* Write put (msg from above) */
	qi_srvp:sctp_t_wsrv,		/* Write queue service */
	qi_minfo:&sctp_t_minfo,		/* Information */
};

STATIC struct streamtab sctp_t_info = {
	st_rdinit:&sctp_t_rinit,	/* Upper read queue */
	st_wrinit:&sctp_t_winit,	/* Upper write queue */
};

#define QR_DONE		0
#define QR_ABSORBED	1
#define QR_TRIMMED	2
#define QR_LOOP		3
#define QR_PASSALONG	4
#define QR_PASSFLOW	5
#define QR_DISABLE	6

/*
 *  =========================================================================
 *
 *  Option handling functions
 *
 *  =========================================================================
 */
typedef struct sctp_opts {
	uint flags;
	struct t_opthdr *bcast;		/* T_IP_BROADCAST */
	struct t_opthdr *norte;		/* T_IP_DONTROUTE */
	struct t_opthdr *opts;		/* T_IP_OPTIONS */
	struct t_opthdr *reuse;		/* T_IP_REUSEADDR */
	struct t_opthdr *tos;		/* T_IP_TOS */
	struct t_opthdr *ttl;		/* T_IP_TTL */
	struct t_opthdr *nd;		/* T_SCTP_NODELAY */
	struct t_opthdr *cork;		/* T_SCTP_CORK */
	struct t_opthdr *ppi;		/* T_SCTP_PPI */
	struct t_opthdr *sid;		/* T_SCTP_SID */
	struct t_opthdr *ssn;		/* T_SCTP_SSN */
	struct t_opthdr *tsn;		/* T_SCTP_TSN */
	struct t_opthdr *ropt;		/* T_SCTP_RECVOPT */
	struct t_opthdr *cklife;	/* T_SCTP_COOKIE_LIFE */
	struct t_opthdr *sack;		/* T_SCTP_SACK_DELAY */
	struct t_opthdr *path;		/* T_SCTP_PATH_MAX_RETRANS */
	struct t_opthdr *assoc;		/* T_SCTP_ASSOC_MAX_RETRANS */
	struct t_opthdr *init;		/* T_SCTP_MAX_INIT_RETRIES */
	struct t_opthdr *hbitvl;	/* T_SCTP_HEARTBEAT_ITVL */
	struct t_opthdr *rtoinit;	/* T_SCTP_RTO_INITIAL */
	struct t_opthdr *rtomin;	/* T_SCTP_RTO_MIN */
	struct t_opthdr *rtomax;	/* T_SCTP_RTO_MAX */
	struct t_opthdr *ostr;		/* T_SCTP_OSTREAMS */
	struct t_opthdr *istr;		/* T_SCTP_ISTREAMS */
	struct t_opthdr *ckinc;		/* T_SCTP_COOKIE_INC */
	struct t_opthdr *titvl;		/* T_SCTP_THROTTLE_ITVL */
	struct t_opthdr *hmac;		/* T_SCTP_MAC_TYPE */
	struct t_opthdr *hb;		/* T_SCTP_HB */
	struct t_opthdr *rto;		/* T_SCTP_RTO */
	struct t_opthdr *mseg;		/* T_SCTP_MAXSEG */
	struct t_opthdr *status;	/* T_SCTP_STATUS */
	struct t_opthdr *debug;		/* T_SCTP_DEBUG */
} sctp_opts_t;

/*
 *  Size and Build Default options
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Either builds the default options requested or builds all default options.
 */
STATIC size_t
sctp_default_opts_size(sctp_t * sp, sctp_opts_t * ops)
{
	size_t len = 0;
	const size_t hlen = sizeof(struct t_opthdr);
	const size_t olen = hlen + sizeof(t_scalar_t);
	if (!ops || ops->bcast) {
		len += olen;
	}
	if (!ops || ops->norte) {
		len += olen;
	}
	if (!ops || ops->opts) {
		len += olen;
	}
	if (!ops || ops->reuse) {
		len += olen;
	}
	if (!ops || ops->tos) {
		len += olen;
	}
	if (!ops || ops->ttl) {
		len += olen;
	}
	if (!ops || ops->nd) {
		len += olen;
	}
	if (!ops || ops->cork) {
		len += olen;
	}
	if (!ops || ops->ppi) {
		len += olen;
	}
	if (!ops || ops->sid) {
		len += olen;
	}
	if (!ops || ops->ropt) {
		len += olen;
	}
	if (!ops || ops->cklife) {
		len += olen;
	}
	if (!ops || ops->sack) {
		len += olen;
	}
	if (!ops || ops->path) {
		len += olen;
	}
	if (!ops || ops->assoc) {
		len += olen;
	}
	if (!ops || ops->init) {
		len += olen;
	}
	if (!ops || ops->hbitvl) {
		len += olen;
	}
	if (!ops || ops->rtoinit) {
		len += olen;
	}
	if (!ops || ops->rtomin) {
		len += olen;
	}
	if (!ops || ops->rtomax) {
		len += olen;
	}
	if (!ops || ops->ostr) {
		len += olen;
	}
	if (!ops || ops->istr) {
		len += olen;
	}
	if (!ops || ops->ckinc) {
		len += olen;
	}
	if (!ops || ops->titvl) {
		len += olen;
	}
	if (!ops || ops->hmac) {
		len += olen;
	}
	if (!ops || ops->mseg) {
		len += olen;
	}
	if (!ops || ops->debug) {
		len += olen;
	}
	return (len);
}
STATIC void
sctp_build_default_opts(sctp_t * sp, sctp_opts_t * ops, unsigned char **p)
{
	struct t_opthdr *oh;
	const size_t hlen = sizeof(*oh);
	const size_t olen = hlen + sizeof(t_scalar_t);

	if (!ops || ops->bcast) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_IP;
		oh->name = T_IP_BROADCAST;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = T_NO;
	}
	if (!ops || ops->norte) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_IP;
		oh->name = T_IP_DONTROUTE;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = T_NO;
	}
	if (!ops || ops->opts) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_IP;
		oh->name = T_IP_OPTIONS;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = 0;
	}
	if (!ops || ops->reuse) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_IP;
		oh->name = T_IP_REUSEADDR;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = T_NO;
	}
	if (!ops || ops->tos) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_IP;
		oh->name = T_IP_TOS;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sctp_default_ip_tos;
	}
	if (!ops || ops->ttl) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_IP;
		oh->name = T_IP_TTL;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sctp_default_ip_ttl;
	}
	if (!ops || ops->nd) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_NODELAY;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = T_YES;
	}
	if (!ops || ops->cork) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_CORK;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = T_NO;
	}
	if (!ops || ops->ppi) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_PPI;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sctp_default_ppi;
	}
	if (!ops || ops->sid) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_SID;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sctp_default_sid;
	}
	/*
	   note ssn and tsn are per-packet 
	 */

	if (!ops || ops->ropt) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_RECVOPT;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = T_NO;
	}
	if (!ops || ops->cklife) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_COOKIE_LIFE;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sctp_default_valid_cookie_life * 1000 / HZ;
	}
	if (!ops || ops->sack) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_SACK_DELAY;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sctp_default_max_sack_delay * 1000 / HZ;
	}
	if (!ops || ops->path) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_PATH_MAX_RETRANS;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sctp_default_path_max_retrans;
	}
	if (!ops || ops->assoc) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_ASSOC_MAX_RETRANS;
		oh->status = T_SUCCESS;
		*((t_uscalar_t *) * p)++ = sctp_default_assoc_max_retrans;
	}
	if (!ops || ops->init) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_MAX_INIT_RETRIES;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sctp_default_max_init_retries;
	}
	if (!ops || ops->hbitvl) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_HEARTBEAT_ITVL;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sctp_default_heartbeat_itvl * 1000 / HZ;
	}
	if (!ops || ops->rtoinit) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_RTO_INITIAL;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sctp_default_rto_initial * 1000 / HZ;
	}
	if (!ops || ops->rtomin) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_RTO_MIN;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sctp_default_rto_min * 1000 / HZ;
	}
	if (!ops || ops->rtomax) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_RTO_MAX;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sctp_default_rto_max * 1000 / HZ;
	}
	if (!ops || ops->ostr) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_OSTREAMS;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sctp_default_req_ostreams;
	}
	if (!ops || ops->istr) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_ISTREAMS;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sctp_default_max_istreams;
	}
	if (!ops || ops->ckinc) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_COOKIE_INC;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sctp_default_cookie_inc * 1000 / HZ;
	}
	if (!ops || ops->titvl) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_THROTTLE_ITVL;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sctp_default_throttle_itvl * 1000 / HZ;
	}
	if (!ops || ops->hmac) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_MAC_TYPE;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sctp_default_mac_type;
	}
	if (!ops || ops->mseg) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_MAXSEG;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = 576;
	}
	if (!ops || ops->debug) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_DEBUG;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = 0;
	}
}

/*
 *  Size and Build Current options
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Either builds the current options requested or builds all current options.
 */
STATIC size_t
sctp_current_opts_size(sctp_t * sp, sctp_opts_t * ops)
{
	size_t len = 0;
	const size_t hlen = sizeof(struct t_opthdr);
	const size_t olen = hlen + sizeof(t_scalar_t);
	if (!ops || ops->bcast) {
		len += olen;
	}
	if (!ops || ops->norte) {
		len += olen;
	}
	if (!ops || ops->opts) {
		len += olen;
	}
	if (!ops || ops->reuse) {
		len += olen;
	}
	if (!ops || ops->tos) {
		len += olen;
	}
	if (!ops || ops->ttl) {
		len += olen;
	}
	if (!ops || ops->nd) {
		len += olen;
	}
	if (!ops || ops->cork) {
		len += olen;
	}
	if (!ops || ops->ppi) {
		len += olen;
	}
	if (!ops || ops->sid) {
		len += olen;
	}
	if (!ops || ops->ropt) {
		len += olen;
	}
	if (!ops || ops->cklife) {
		len += olen;
	}
	if (!ops || ops->sack) {
		len += olen;
	}
	if (!ops || ops->path) {
		len += olen;
	}
	if (!ops || ops->assoc) {
		len += olen;
	}
	if (!ops || ops->init) {
		len += olen;
	}
	if (!ops || ops->hbitvl) {
		len += olen;
	}
	if (!ops || ops->rtoinit) {
		len += olen;
	}
	if (!ops || ops->rtomin) {
		len += olen;
	}
	if (!ops || ops->rtomax) {
		len += olen;
	}
	if (!ops || ops->ostr) {
		len += olen;
	}
	if (!ops || ops->istr) {
		len += olen;
	}
	if (!ops || ops->ckinc) {
		len += olen;
	}
	if (!ops || ops->titvl) {
		len += olen;
	}
	if (!ops || ops->hmac) {
		len += olen;
	}
	if (!ops || ops->mseg) {
		len += olen;
	}
	if (!ops || ops->debug) {
		len += olen;
	}
	if (!ops || ops->hb) {
		size_t n = ops ? (ops->hb->len - hlen) / sizeof(t_sctp_hb_t) : sp->danum;
		if (!n)
			n = sp->danum;
		if (n)
			len += hlen + n * sizeof(t_sctp_hb_t);
	}
	if (!ops || ops->rto) {
		size_t n = ops ? (ops->rto->len - hlen) / sizeof(t_sctp_rto_t) : sp->danum;
		if (!n)
			n = sp->danum;
		if (n)
			len += hlen + n * sizeof(t_sctp_rto_t);
	}
	if (!ops || ops->status) {
		size_t n =
		    ops ? (ops->status->len - hlen -
			   sizeof(t_sctp_status_t)) / sizeof(t_sctp_dest_status_t) : sp->danum;
		if (!n)
			n = sp->danum;
		if (n)
			len += hlen + sizeof(t_sctp_status_t) + n * sizeof(t_sctp_dest_status_t);
	}
	return (len);
}
STATIC void
sctp_build_current_opts(sctp_t * sp, sctp_opts_t * ops, unsigned char **p)
{
	struct t_opthdr *oh;
	const size_t hlen = sizeof(*oh);
	const size_t olen = hlen + sizeof(t_scalar_t);

	if (!ops || ops->bcast) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_IP;
		oh->name = T_IP_BROADCAST;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sp->ip_broadcast ? T_YES : T_NO;
	}
	if (!ops || ops->norte) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_IP;
		oh->name = T_IP_DONTROUTE;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sp->ip_dontroute ? T_YES : T_NO;
	}
	if (!ops || ops->opts) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_IP;
		oh->name = T_IP_OPTIONS;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = 0;
	}
	if (!ops || ops->reuse) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_IP;
		oh->name = T_IP_REUSEADDR;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sp->i_flags & TF_IP_REUSEADDR ? T_YES : T_NO;
	}
	if (!ops || ops->tos) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_IP;
		oh->name = T_IP_TOS;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sp->ip_tos;
	}
	if (!ops || ops->ttl) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_IP;
		oh->name = T_IP_TTL;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sp->ip_ttl;
	}
	if (!ops || ops->nd) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_NODELAY;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sp->options & SCTP_OPTION_NAGLE ? T_NO : T_YES;
	}
	if (!ops || ops->cork) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_CORK;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sp->options & SCTP_OPTION_CORK ? T_YES : T_NO;
	}
	if (!ops || ops->ppi) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_PPI;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sp->ppi;
	}
	if (!ops || ops->sid) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_SID;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sp->sid;
	}
	/*
	   note ssn and tsn are per-packet 
	 */

	if (!ops || ops->ropt) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_RECVOPT;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sp->i_flags & TF_SCTP_RECVOPT ? T_YES : T_NO;
	}
	if (!ops || ops->cklife) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_COOKIE_LIFE;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = (sp->ck_life * 1000 + HZ - 1) / HZ;
	}
	if (!ops || ops->sack) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_SACK_DELAY;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = (sp->max_sack * 1000 + HZ - 1) / HZ;
	}
	if (!ops || ops->path) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_PATH_MAX_RETRANS;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sp->rtx_path;
	}
	if (!ops || ops->assoc) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_ASSOC_MAX_RETRANS;
		oh->status = T_SUCCESS;
		*((t_uscalar_t *) * p)++ = sp->max_retrans;
	}
	if (!ops || ops->init) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_MAX_INIT_RETRIES;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sp->max_inits;
	}
	if (!ops || ops->hbitvl) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_HEARTBEAT_ITVL;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = (sp->hb_itvl * 1000 + HZ - 1) / HZ;
	}
	if (!ops || ops->rtoinit) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_RTO_INITIAL;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = (sp->rto_ini * 1000 + HZ - 1) / HZ;
	}
	if (!ops || ops->rtomin) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_RTO_MIN;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = (sp->rto_min * 1000 + HZ - 1) / HZ;
	}
	if (!ops || ops->rtomax) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_RTO_MAX;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = (sp->rto_max * 1000 + HZ - 1) / HZ;
	}
	if (!ops || ops->ostr) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_OSTREAMS;
		oh->status = T_SUCCESS;
		if ((1 << sp->i_state) & (TSF_DATA_XFER | TSF_WIND_ORDREL | TSF_WREQ_ORDREL))
			*((t_scalar_t *) * p)++ = sp->n_ostr;
		else
			*((t_scalar_t *) * p)++ = sp->req_ostr;
	}
	if (!ops || ops->istr) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_ISTREAMS;
		oh->status = T_SUCCESS;
		if ((1 << sp->i_state) & (TSF_DATA_XFER | TSF_WIND_ORDREL | TSF_WREQ_ORDREL))
			*((t_scalar_t *) * p)++ = sp->n_istr;
		else
			*((t_scalar_t *) * p)++ = sp->max_istr;
	}
	if (!ops || ops->ckinc) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_COOKIE_INC;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = (sp->ck_inc * 1000 + HZ - 1) / HZ;
	}
	if (!ops || ops->titvl) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_THROTTLE_ITVL;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = (sp->throttle * 1000 + HZ - 1) / HZ;
	}
	if (!ops || ops->hmac) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_MAC_TYPE;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sp->hmac;
	}
	if (!ops || ops->mseg) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_MAXSEG;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sp->pmtu;
	}
	if (!ops || ops->debug) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_DEBUG;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sp->options;
	}
	if (!ops || ops->hb) {
		size_t n = ops ? (ops->hb->len - hlen) / sizeof(t_sctp_hb_t) : sp->danum;
		if (!n)
			n = sp->danum;
		if (n) {
			sctp_daddr_t *sd;

			oh = ((struct t_opthdr *) *p)++;
			oh->len = sizeof(*oh) + n * sizeof(t_sctp_hb_t);
			oh->level = T_INET_SCTP;
			oh->name = T_SCTP_HB;
			oh->status = T_SUCCESS;

			for (sd = sp->daddr; n && sd; n--, sd = sd->next) {
				t_sctp_hb_t *hb = ((t_sctp_hb_t *) * p)++;
				hb->hb_dest = sd->daddr;
				hb->hb_onoff = sd->hb_onoff;
				hb->hb_itvl = (sd->hb_itvl * 1000 + HZ - 1) / HZ;
			}
		}
	}
	if (!ops || ops->rto) {
		size_t n = ops ? (ops->rto->len - hlen) / sizeof(t_sctp_rto_t) : sp->danum;
		if (!n)
			n = sp->danum;
		if (n) {
			sctp_daddr_t *sd;

			oh = ((struct t_opthdr *) *p)++;
			oh->len = sizeof(*oh) + n * sizeof(t_sctp_rto_t);
			oh->level = T_INET_SCTP;
			oh->name = T_SCTP_RTO;
			oh->status = T_SUCCESS;

			for (sd = sp->daddr; n && sd; n--, sd = sd->next) {
				t_sctp_rto_t *rto = ((t_sctp_rto_t *) * p)++;
				rto->rto_dest = sd->daddr;
				rto->rto_initial = (sp->rto_ini * 1000 + HZ - 1) / HZ;
				rto->rto_min = (sd->rto_min * 1000 + HZ - 1) / HZ;
				rto->rto_max = (sd->rto_max * 1000 + HZ - 1) / HZ;
				rto->max_retrans = sd->max_retrans;
			}
		}
	}
	if (!ops || ops->status) {
		size_t n =
		    ops ? (ops->status->len - hlen -
			   sizeof(t_sctp_status_t)) / sizeof(t_sctp_dest_status_t) : sp->danum;
		if (!n)
			n = sp->danum;
		if (n) {
			sctp_daddr_t *sd;
			t_sctp_status_t *curr;

			oh = ((struct t_opthdr *) *p)++;
			oh->len =
			    sizeof(*oh) + sizeof(t_sctp_status_t) +
			    n * sizeof(t_sctp_dest_status_t);
			oh->level = T_INET_SCTP;
			oh->name = T_SCTP_STATUS;
			oh->status = T_SUCCESS;

			curr = ((t_sctp_status_t *) * p)++;
			curr->curr_rwnd = sp->p_rwnd - sp->in_flight;
			curr->curr_rbuf = sp->a_rwnd;
			curr->curr_nrep = n;

			for (sd = sp->daddr; n && sd; n--, sd = sd->next) {
				t_sctp_dest_status_t *dest = ((t_sctp_dest_status_t *) * p)++;
				dest->dest_addr = sd->daddr;
				dest->dest_cwnd = sd->cwnd;
				dest->dest_unack = sd->in_flight;
				dest->dest_srtt = (sd->srtt * 1000 + HZ - 1) / HZ;
				dest->dest_rvar = sd->rttvar;
				dest->dest_rto = (sd->rto * 1000 + HZ - 1) / HZ;
				dest->dest_sst = sd->ssthresh;
			}
		}
	}
}

/*
 *  Size and Build Checked or Negotiated options
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Builds the checked or negotiated options.
 */
STATIC size_t
sctp_set_opts_size(sctp_t * sp, sctp_opts_t * ops)
{
	size_t len = 0;
	const size_t hlen = sizeof(struct t_opthdr);
	const size_t olen = hlen + sizeof(t_scalar_t);
	if (ops->bcast) {
		len += olen;
	}
	if (ops->norte) {
		len += olen;
	}
	if (ops->opts) {
		len += olen;
	}
	if (ops->reuse) {
		len += olen;
	}
	if (ops->tos) {
		len += olen;
	}
	if (ops->ttl) {
		len += olen;
	}
	if (ops->nd) {
		len += olen;
	}
	if (ops->cork) {
		len += olen;
	}
	if (ops->ppi) {
		len += olen;
	}
	if (ops->sid) {
		len += olen;
	}
	if (ops->ssn) {
		len += olen;
	}
	if (ops->tsn) {
		len += olen;
	}
	if (ops->ropt) {
		len += olen;
	}
	if (ops->cklife) {
		len += olen;
	}
	if (ops->sack) {
		len += olen;
	}
	if (ops->path) {
		len += olen;
	}
	if (ops->assoc) {
		len += olen;
	}
	if (ops->init) {
		len += olen;
	}
	if (ops->hbitvl) {
		len += olen;
	}
	if (ops->rtoinit) {
		len += olen;
	}
	if (ops->rtomin) {
		len += olen;
	}
	if (ops->rtomax) {
		len += olen;
	}
	if (ops->ostr) {
		len += olen;
	}
	if (ops->istr) {
		len += olen;
	}
	if (ops->ckinc) {
		len += olen;
	}
	if (ops->titvl) {
		len += olen;
	}
	if (ops->hmac) {
		len += olen;
	}
	if (ops->mseg) {
		len += olen;
	}
	if (ops->debug) {
		len += olen;
	}
	if (ops->hb) {
		len += ops->hb->len;
	}
	if (ops->rto) {
		len += ops->rto->len;
	}
	if (ops->status) {
		len += ops->status->len;
	}
	return (len);
}
STATIC void
sctp_build_set_opts(sctp_t * sp, sctp_opts_t * ops, unsigned char **p)
{
	struct t_opthdr *oh;
	const size_t hlen = sizeof(*oh);
	const size_t olen = hlen + sizeof(t_scalar_t);

	if (ops->bcast) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_IP;
		oh->name = T_IP_BROADCAST;
		oh->status = ops->flags & TF_IP_BROADCAST ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->bcast + 1));
	}
	if (ops->norte) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_IP;
		oh->name = T_IP_DONTROUTE;
		oh->status = ops->flags & TF_IP_DONTROUTE ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->norte + 1));
	}
	if (ops->opts) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_IP;
		oh->name = T_IP_OPTIONS;
		oh->status = ops->flags & TF_IP_OPTIONS ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->opts + 1));
	}
	if (ops->reuse) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_IP;
		oh->name = T_IP_REUSEADDR;
		oh->status = ops->flags & TF_IP_REUSEADDR ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->reuse + 1));
	}
	if (ops->tos) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_IP;
		oh->name = T_IP_TOS;
		oh->status = ops->flags & TF_IP_TOS ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->tos + 1));
	}
	if (ops->ttl) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_IP;
		oh->name = T_IP_TTL;
		oh->status = ops->flags & TF_IP_TTL ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->ttl + 1));
	}
	if (ops->nd) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_NODELAY;
		oh->status = ops->flags & TF_SCTP_NODELAY ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->nd + 1));
	}
	if (ops->cork) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_CORK;
		oh->status = ops->flags & TF_SCTP_CORK ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->cork + 1));
	}
	if (ops->ppi) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_PPI;
		oh->status = ops->flags & TF_SCTP_PPI ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->ppi + 1));
	}
	if (ops->sid) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_SID;
		oh->status = ops->flags & TF_SCTP_SID ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->sid + 1));
	}
	if (ops->ssn) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_SSN;
		oh->status = ops->flags & TF_SCTP_SSN ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->ssn + 1));
	}
	if (ops->tsn) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_TSN;
		oh->status = ops->flags & TF_SCTP_TSN ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->tsn + 1));
	}
	if (ops->ropt) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_RECVOPT;
		oh->status = ops->flags & TF_SCTP_RECVOPT ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->ropt + 1));
	}
	if (ops->cklife) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_COOKIE_LIFE;
		oh->status = ops->flags & TF_SCTP_COOKIE_LIFE ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->cklife + 1));
	}
	if (ops->sack) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_SACK_DELAY;
		oh->status = ops->flags & TF_SCTP_SACK_DELAY ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->sack + 1));
	}
	if (ops->path) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_PATH_MAX_RETRANS;
		oh->status = ops->flags & TF_SCTP_PATH_MAX_RETRANS ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->path + 1));
	}
	if (ops->assoc) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_ASSOC_MAX_RETRANS;
		oh->status = ops->flags & TF_SCTP_ASSOC_MAX_RETRANS ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->assoc + 1));
	}
	if (ops->init) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_MAX_INIT_RETRIES;
		oh->status = ops->flags & TF_SCTP_MAX_INIT_RETRIES ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->init + 1));
	}
	if (ops->hbitvl) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_HEARTBEAT_ITVL;
		oh->status = ops->flags & TF_SCTP_HEARTBEAT_ITVL ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->hbitvl + 1));
	}
	if (ops->rtoinit) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_RTO_INITIAL;
		oh->status = ops->flags & TF_SCTP_RTO_INITIAL ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->rtoinit + 1));
	}
	if (ops->rtomin) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_RTO_MIN;
		oh->status = ops->flags & TF_SCTP_RTO_MIN ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->rtomin + 1));
	}
	if (ops->rtomax) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_RTO_MAX;
		oh->status = ops->flags & TF_SCTP_RTO_MAX ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->rtomax + 1));
	}
	if (ops->ostr) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_OSTREAMS;
		oh->status = ops->flags & TF_SCTP_OSTREAMS ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->ostr + 1));
	}
	if (ops->istr) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_ISTREAMS;
		oh->status = ops->flags & TF_SCTP_ISTREAMS ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->istr + 1));
	}
	if (ops->ckinc) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_COOKIE_INC;
		oh->status = ops->flags & TF_SCTP_COOKIE_INC ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->ckinc + 1));
	}
	if (ops->titvl) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_THROTTLE_ITVL;
		oh->status = ops->flags & TF_SCTP_THROTTLE_ITVL ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->titvl + 1));
	}
	if (ops->hmac) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_MAC_TYPE;
		oh->status = ops->flags & TF_SCTP_MAC_TYPE ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->hmac + 1));
	}
	if (ops->mseg) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_MAXSEG;
		oh->status = ops->flags & TF_SCTP_MAXSEG ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->mseg + 1));
	}
	if (ops->debug) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_DEBUG;
		oh->status = ops->flags & TF_SCTP_DEBUG ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->debug + 1));
	}
	if (ops->hb) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = ops->hb->len;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_HB;
		oh->status = ops->flags & TF_SCTP_HB ? T_SUCCESS : T_FAILURE;
		bcopy(ops->hb + 1, *p, ops->hb->len - sizeof(*oh));
		*p += ops->hb->len - sizeof(*oh);
	}
	if (ops->rto) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = ops->rto->len;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_RTO;
		oh->status = ops->flags & TF_SCTP_RTO ? T_SUCCESS : T_FAILURE;
		bcopy(ops->rto + 1, *p, ops->rto->len - sizeof(*oh));
		*p += ops->rto->len - sizeof(*oh);
	}
	if (ops->status) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = ops->status->len;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_STATUS;
		oh->status = ops->flags & TF_SCTP_STATUS ? T_SUCCESS : T_FAILURE;
		bcopy(ops->status + 1, *p, ops->status->len - sizeof(*oh));
		*p += ops->status->len - sizeof(*oh);
	}
}

/*
 *  Size and Build options.
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC size_t
sctp_opts_size(ulong flags, sctp_t * sp, sctp_opts_t * ops)
{
	switch (flags) {
	case T_CHECK:
	case T_NEGOTIATE:
		if (ops)
			return sctp_set_opts_size(sp, ops);
	case T_DEFAULT:
		return sctp_default_opts_size(sp, ops);
	case T_CURRENT:
		return sctp_current_opts_size(sp, ops);
	}
	return (0);
}
STATIC void
sctp_build_opts(ulong flags, sctp_t * sp, sctp_opts_t * ops, unsigned char **p)
{
	switch (flags) {
	case T_CHECK:
	case T_NEGOTIATE:
		if (ops)
			return sctp_build_set_opts(sp, ops, p);
	case T_DEFAULT:
		return sctp_build_default_opts(sp, ops, p);
	case T_CURRENT:
		return sctp_build_current_opts(sp, ops, p);
	}
	return;
}

/*
 *  Parse options
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
sctp_parse_opts(sctp_opts_t * ops, unsigned char *opt_ptr, size_t opt_len)
{
	struct t_opthdr *oh = (struct t_opthdr *) opt_ptr;
	unsigned char *opt_end = opt_ptr + opt_len;
	for (; opt_ptr + sizeof(*oh) <= opt_end && oh->len >= sizeof(*oh)
	     && opt_ptr + oh->len <= opt_end; opt_ptr += oh->len, oh = (struct t_opthdr *) opt_ptr) {
		switch (oh->level) {
		case T_INET_IP:
			switch (oh->name) {
			case T_IP_BROADCAST:
				ops->bcast = oh;
				continue;
			case T_IP_DONTROUTE:
				ops->norte = oh;
				continue;
			case T_IP_OPTIONS:
				ops->opts = oh;
				continue;
			case T_IP_REUSEADDR:
				ops->reuse = oh;
				continue;
			case T_IP_TOS:
				ops->tos = oh;
				continue;
			case T_IP_TTL:
				ops->ttl = oh;
				continue;
			}
			break;
		case T_INET_SCTP:
			switch (oh->name) {
			case T_SCTP_NODELAY:
				ops->nd = oh;
				continue;
			case T_SCTP_CORK:
				ops->cork = oh;
				continue;
			case T_SCTP_PPI:
				ops->ppi = oh;
				continue;
			case T_SCTP_SID:
				ops->sid = oh;
				continue;
			case T_SCTP_SSN:
				ops->ssn = oh;
				continue;
			case T_SCTP_TSN:
				ops->tsn = oh;
				continue;
			case T_SCTP_RECVOPT:
				ops->ropt = oh;
				continue;
			case T_SCTP_COOKIE_LIFE:
				ops->cklife = oh;
				continue;
			case T_SCTP_SACK_DELAY:
				ops->sack = oh;
				continue;
			case T_SCTP_PATH_MAX_RETRANS:
				ops->path = oh;
				continue;
			case T_SCTP_ASSOC_MAX_RETRANS:
				ops->assoc = oh;
				continue;
			case T_SCTP_MAX_INIT_RETRIES:
				ops->init = oh;
				continue;
			case T_SCTP_HEARTBEAT_ITVL:
				ops->hbitvl = oh;
				continue;
			case T_SCTP_RTO_INITIAL:
				ops->rtoinit = oh;
				continue;
			case T_SCTP_RTO_MIN:
				ops->rtomin = oh;
				continue;
			case T_SCTP_RTO_MAX:
				ops->rtomax = oh;
				continue;
			case T_SCTP_OSTREAMS:
				ops->ostr = oh;
				continue;
			case T_SCTP_ISTREAMS:
				ops->istr = oh;
				continue;
			case T_SCTP_COOKIE_INC:
				ops->ckinc = oh;
				continue;
			case T_SCTP_THROTTLE_ITVL:
				ops->titvl = oh;
				continue;
			case T_SCTP_MAC_TYPE:
				ops->hmac = oh;
				continue;
			case T_SCTP_MAXSEG:
				ops->mseg = oh;
				continue;
			case T_SCTP_DEBUG:
				ops->debug = oh;
				continue;
			case T_SCTP_HB:
				ops->hb = oh;
				continue;
			case T_SCTP_RTO:
				ops->rto = oh;
				continue;
			case T_SCTP_STATUS:
				ops->status = oh;
				continue;
			}
			break;
		}
		return (TBADOPT);
	}
	if (opt_ptr != opt_end)
		return (TBADOPT);
	return (0);
}

/*
 *  Negotiate options
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void
sctp_negotiate_opts(sctp_t * sp, sctp_opts_t * ops)
{
	const size_t hlen = sizeof(struct t_opthdr);
	const size_t olen = hlen + sizeof(t_scalar_t);

	if (!ops)
		return;

	if (ops->bcast) {
		if (ops->bcast->len >= olen) {
			t_scalar_t *val = (t_scalar_t *) (ops->bcast + 1);
			switch (*val) {
			case T_YES:
				sp->ip_broadcast = 1;
				break;
			case T_NO:
				sp->ip_broadcast = 0;
				break;
			}
			*val = sp->ip_broadcast;
			ops->flags |= TF_IP_BROADCAST;
		}
	}
	if (ops->norte) {
		if (ops->norte->len >= olen) {
			t_scalar_t *val = (t_scalar_t *) (ops->norte + 1);
			switch (*val) {
			case T_YES:
				sp->ip_dontroute = 1;
				break;
			case T_NO:
				sp->ip_dontroute = 0;
				break;
			}
			*val = sp->ip_dontroute;
			ops->flags |= TF_IP_DONTROUTE;
		}
	}
	if (ops->opts) {
		/*
		   not supported yet 
		 */
	}
	if (ops->reuse) {
		/*
		   not supported yet 
		 */
	}
	if (ops->tos) {
		if (ops->tos->len >= olen) {
			t_scalar_t *val = (t_scalar_t *) (ops->tos + 1);
			sp->ip_tos = *val & 0xff;
			*val = sp->ip_tos;
			ops->flags |= TF_IP_TOS;
		}
	}
	if (ops->ttl) {
		if (ops->ttl->len >= olen) {
			t_scalar_t *val = (t_scalar_t *) (ops->ttl + 1);
			sp->ip_ttl = *val & 0xff;
			*val = sp->ip_ttl;
			ops->flags |= TF_IP_TTL;
		}
	}
	if (ops->nd) {
		if (ops->nd->len >= olen) {
			t_scalar_t *val = (t_scalar_t *) (ops->nd + 1);
			switch (*val) {
			case T_YES:
				sp->options &= ~SCTP_OPTION_NAGLE;
				break;
			case T_NO:
				sp->options |= SCTP_OPTION_NAGLE;
				break;
				break;
			}
			*val = (sp->options & SCTP_OPTION_NAGLE) ? T_NO : T_YES;
			ops->flags |= TF_SCTP_NODELAY;
		}
	}
	if (ops->cork) {
		if (ops->cork->len >= olen) {
			t_scalar_t *val = (t_scalar_t *) (ops->cork + 1);
			switch (*val) {
			case T_YES:
				sp->options |= SCTP_OPTION_CORK;
				break;
			case T_NO:
				sp->options &= ~SCTP_OPTION_CORK;
				break;
			}
			*val = (sp->options & SCTP_OPTION_CORK) ? T_YES : T_NO;
			ops->flags |= TF_SCTP_CORK;
		}
	}
	if (ops->ppi) {
		if (ops->ppi->len >= olen) {
			t_scalar_t *val = (t_scalar_t *) (ops->ppi + 1);
			sp->ppi = *val;
			*val = sp->ppi;
			ops->flags |= TF_SCTP_PPI;
		}
	}
	if (ops->sid) {
		if (ops->sid->len >= olen) {
			t_scalar_t *val = (t_scalar_t *) (ops->sid + 1);
			sp->sid = *val;
			*val = sp->sid;
			ops->flags |= TF_SCTP_SID;
		}
	}
	if (ops->ssn) {
		/*
		   not writeable 
		 */
	}
	if (ops->tsn) {
		/*
		   not writeable 
		 */
	}
	if (ops->ropt) {
		if (ops->ropt->len >= olen) {
			t_scalar_t *val = (t_scalar_t *) (ops->ropt + 1);
			switch (*val) {
			case T_YES:
				sp->i_flags |= TF_SCTP_RECVOPT;
				break;
			case T_NO:
				sp->i_flags &= ~TF_SCTP_RECVOPT;
				break;
			}
			*val = (sp->i_flags & TF_SCTP_RECVOPT) ? T_YES : T_NO;
			ops->flags |= TF_SCTP_RECVOPT;
		}
	}
	if (ops->cklife) {
		if (ops->cklife->len >= olen) {
			t_scalar_t *val = (t_scalar_t *) (ops->cklife + 1);
			if (*val < 10)
				*val = 10;
			sp->ck_life = (*val * HZ + 999) / 1000;
			*val = (sp->ck_life * 1000 + HZ - 1) / HZ;
			ops->flags |= TF_SCTP_COOKIE_LIFE;
		}
	}
	if (ops->sack) {
		if (ops->sack->len >= olen) {
			t_scalar_t *val = (t_scalar_t *) (ops->sack + 1);
			sp->max_sack = (*val * HZ + 999) / 1000;
			*val = (sp->max_sack * 1000 + HZ - 1) / HZ;
			ops->flags |= TF_SCTP_SACK_DELAY;
		}
	}
	if (ops->path) {
		if (ops->path->len >= olen) {
			t_scalar_t *val = (t_scalar_t *) (ops->path + 1);
			sp->rtx_path = *val;
			*val = sp->rtx_path;
			ops->flags |= TF_SCTP_PATH_MAX_RETRANS;
		}
	}
	if (ops->assoc) {
		if (ops->assoc->len >= olen) {
			t_scalar_t *val = (t_scalar_t *) (ops->assoc + 1);
			sp->max_retrans = *val;
			*val = sp->max_retrans;
			ops->flags |= TF_SCTP_ASSOC_MAX_RETRANS;
		}
	}
	if (ops->init) {
		if (ops->init->len >= olen) {
			t_scalar_t *val = (t_scalar_t *) (ops->init + 1);
			sp->max_inits = *val;
			*val = sp->max_inits;
			ops->flags |= TF_SCTP_MAX_INIT_RETRIES;
		}
	}
	if (ops->hbitvl) {
		if (ops->hbitvl->len >= olen) {
			t_scalar_t *val = (t_scalar_t *) (ops->hbitvl + 1);
			sp->hb_itvl = (*val * HZ + 999) / 1000;
			*val = (sp->hb_itvl * 1000 + HZ - 1) / HZ;
			ops->flags |= TF_SCTP_HEARTBEAT_ITVL;
		}
	}
	if (ops->rtoinit) {
		if (ops->rtoinit->len >= olen) {
			t_scalar_t *val = (t_scalar_t *) (ops->rtoinit + 1);
			sp->rto_ini = (*val * HZ + 999) / 1000;
			*val = (sp->rto_ini * 1000 + HZ - 1) / HZ;
			ops->flags |= TF_SCTP_RTO_INITIAL;
		}
	}
	if (ops->rtomin) {
		if (ops->rtomin->len >= olen) {
			t_scalar_t *val = (t_scalar_t *) (ops->rtomin + 1);
			sp->rto_min = (*val * HZ + 999) / 1000;
			*val = (sp->rto_min * 1000 + HZ - 1) / HZ;
			ops->flags |= TF_SCTP_RTO_MIN;
		}
	}
	if (ops->rtomax) {
		if (ops->rtomax->len >= olen) {
			t_scalar_t *val = (t_scalar_t *) (ops->rtomax + 1);
			sp->rto_max = (*val * HZ + 999) / 1000;
			*val = (sp->rto_max * 1000 + HZ - 1) / HZ;
			ops->flags |= TF_SCTP_RTO_MAX;
		}
	}
	if (ops->ostr) {
		if (ops->ostr->len >= olen) {
			t_scalar_t *val = (t_scalar_t *) (ops->ostr + 1);
			sp->req_ostr = *val;
			*val = sp->req_ostr;
			ops->flags |= TF_SCTP_OSTREAMS;
		}
	}
	if (ops->istr) {
		if (ops->istr->len >= olen) {
			t_scalar_t *val = (t_scalar_t *) (ops->istr + 1);
			sp->max_istr = *val;
			*val = sp->max_istr;
			ops->flags |= TF_SCTP_ISTREAMS;
		}
	}
	if (ops->ckinc) {
		if (ops->ckinc->len >= olen) {
			t_scalar_t *val = (t_scalar_t *) (ops->ckinc + 1);
			sp->ck_inc = (*val * HZ + 999) / 1000;
			*val = (sp->ck_inc * 1000 + HZ - 1) / HZ;
			ops->flags |= TF_SCTP_COOKIE_INC;
		}
	}
	if (ops->titvl) {
		if (ops->titvl->len >= olen) {
			t_scalar_t *val = (t_scalar_t *) (ops->titvl + 1);
			sp->throttle = (*val * HZ + 999) / 1000;
			*val = (sp->throttle * 1000 + HZ - 1) / HZ;
			ops->flags |= TF_SCTP_THROTTLE_ITVL;
		}
	}
	if (ops->hmac) {
		if (ops->hmac->len >= olen) {
			t_scalar_t *val = (t_scalar_t *) (ops->hmac + 1);
			sp->hmac = *val;
			*val = sp->hmac;
			ops->flags |= TF_SCTP_MAC_TYPE;
		}
	}
	if (ops->mseg) {
		/*
		   not writeable 
		 */
	}
	if (ops->debug) {
		if (ops->debug->len >= olen) {
			t_scalar_t *val = (t_scalar_t *) (ops->debug + 1);
			sp->options = *val;
			*val = sp->options;
			ops->flags |= TF_SCTP_DEBUG;
		}
	}
	if (ops->hb) {
		/*
		   not support yet 
		 */
	}
	if (ops->rto) {
		/*
		   not support yet 
		 */
	}
	if (ops->status) {
		/*
		   not writeable 
		 */
	}
	return;
}

/*
 *  Check options
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void
sctp_check_opts(sctp_t * sp, sctp_opts_t * ops)
{
	if (!ops)
		return;
	ops->flags = TF_SCTP_ALLOPS;
	/*
	 *  FIXME: actually check some options.
	 */
	fixme(("Actually check some options.\n"));
	return;
}

/*
 *  =========================================================================
 *
 *  SCTP T-Provider --> T-User Primitives (Indication, Confirmation and Ack)
 *
 *  =========================================================================
 *
 *  T_CONN_IND      11 - connection indication
 *  -----------------------------------------------------------------
 *  We get the connection indication information from the cookie received in the COOKIE ECHO
 *  which invokes the indication.  (We queue the COOKIE ECHO chunks themselves as
 *  indications.)
 */
STATIC int
t_conn_ind(sctp_t * sp, mblk_t *cp)
{
	mblk_t *mp;
	struct T_conn_ind *p;
	struct t_opthdr *oh;
	struct sctp_cookie_echo *m = (struct sctp_cookie_echo *) cp->b_rptr;
	struct sctp_cookie *ck = (struct sctp_cookie *) m->cookie;
	size_t danum = ck->danum + 1;
	uint32_t *daptr = (uint32_t *) (((caddr_t) (ck + 1) + ck->opt_len));
	size_t src_len = danum ? sizeof(uint16_t) + danum * sizeof(uint32_t) : 0;
	size_t str_len = sizeof(struct t_opthdr) + sizeof(t_scalar_t);
	size_t opt_len = 2 * str_len;
	ensure(((1 << sp->i_state) & (TSF_IDLE | TSF_WRES_CIND)), return (-EFAULT));
	if (bufq_length(&sp->conq) < sp->conind) {
		if (canputnext(sp->rq)) {
			if ((mp = allocb(sizeof(*p) + src_len + opt_len, BPRI_MED))) {
				mp->b_datap->db_type = M_PROTO;
				p = ((struct T_conn_ind *) mp->b_wptr)++;
				p->PRIM_type = T_CONN_IND;
				p->SRC_length = src_len;
				p->SRC_offset = src_len ? sizeof(*p) : 0;
				p->OPT_length = opt_len;
				p->OPT_offset = opt_len ? sizeof(*p) + src_len : 0;
				p->SEQ_number = (ulong) cp;

				/*
				   place address information from cookie 
				 */
				if (danum)
					*((uint16_t *) mp->b_wptr)++ = ck->dport;
				if (danum--)
					*((uint32_t *) mp->b_wptr)++ = ck->daddr;
				while (danum--)
					*((uint32_t *) mp->b_wptr)++ = *daptr++;

				/*
				   indicate options 
				 */
				oh = ((struct t_opthdr *) mp->b_wptr)++;
				oh->len = str_len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_ISTREAMS;
				oh->status = T_SUCCESS;
				*((t_scalar_t *) mp->b_wptr)++ = ck->n_istr;

				/*
				   indicate options 
				 */
				oh = ((struct t_opthdr *) mp->b_wptr)++;
				oh->len = str_len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_OSTREAMS;
				oh->status = T_SUCCESS;
				*((t_scalar_t *) mp->b_wptr)++ = ck->n_ostr;

				bufq_queue(&sp->conq, cp);

				sp->i_state = TS_WRES_CIND;
				putnext(sp->rq, mp);
				return (0);
			}
			seldom();
			return (-ENOBUFS);
		}
		seldom();
		return (-EBUSY);
	}
	seldom();
	return (-ERESTART);
}

/*
 *  T_CONN_CON      12 - connection confirmation
 *  -----------------------------------------------------------------
 *  The only options with end-to-end significance that are negotiated are the number of
 *  inbound and outbound streams.
 */
STATIC int
t_conn_con(sctp_t * sp)
{
	mblk_t *mp;
	struct T_conn_con *p;
	struct t_opthdr *oh;
	struct sctp_daddr *sd = sp->daddr;
	size_t res_len = sp->danum ? sizeof(uint16_t) + sp->danum * sizeof(sd->daddr) : 0;
	size_t str_len = sizeof(*oh) + sizeof(t_scalar_t);
	size_t opt_len = 2 * str_len;
	ensure((sp->i_state == TS_WCON_CREQ), return (-EFAULT));
	if (canputnext(sp->rq)) {
		if ((mp = allocb(sizeof(*p) + res_len + opt_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = 1;	/* expedite */
			p = ((struct T_conn_con *) mp->b_wptr)++;
			p->PRIM_type = T_CONN_CON;
			p->RES_length = res_len;
			p->RES_offset = res_len ? sizeof(*p) : 0;
			p->OPT_length = opt_len;
			p->OPT_offset = opt_len ? sizeof(*p) + res_len : 0;

			/*
			   place destination (responding) address 
			 */
			if (sd)
				*((uint16_t *) mp->b_wptr)++ = sp->dport;
			for (; sd; sd = sd->next)
				*((uint32_t *) mp->b_wptr)++ = sd->daddr;

			/*
			   indicate options 
			 */
			oh = ((struct t_opthdr *) mp->b_wptr)++;
			oh->len = str_len;
			oh->level = T_INET_SCTP;
			oh->name = T_SCTP_ISTREAMS;
			oh->status = T_SUCCESS;
			*((t_scalar_t *) mp->b_wptr)++ = sp->n_istr;

			/*
			   indicate options 
			 */
			oh = ((struct t_opthdr *) mp->b_wptr)++;
			oh->len = str_len;
			oh->level = T_INET_SCTP;
			oh->name = T_SCTP_OSTREAMS;
			oh->status = T_SUCCESS;
			*((t_scalar_t *) mp->b_wptr)++ = sp->n_ostr;

			sp->i_state = TS_DATA_XFER;
			putnext(sp->rq, mp);
			return (0);
		}
		seldom();
		return (-ENOBUFS);
	}
	seldom();
	return (-EBUSY);
}

/*
 *  T_DISCON_IND    13 - disconnect indication
 *  -----------------------------------------------------------------
 *  We use the address of the mblk that contains the COOKIE-ECHO chunk as a SEQ_number for
 *  connect indications that are rejected with a disconnect indication as well.  We can use
 *  this to directly address the mblk in the connection indication bufq.
 *
 *  If the caller provides disconnect data, the caller needs to set the current ord, ppi,
 *  sid, and ssn fields so that the user can examine them with T_OPTMGMT_REQ T_CURRENT if it
 *  has need to know them.
 */
STATIC int
t_discon_ind(sctp_t * sp, long reason, mblk_t *seq)
{
	mblk_t *mp;
	struct T_discon_ind *p;
	ensure(((1 << sp->
		 i_state) & (TSF_WCON_CREQ | TSF_WRES_CIND | TSF_DATA_XFER | TSF_WIND_ORDREL |
			     TSF_WREQ_ORDREL)), return (-EFAULT));
	if (canputnext(sp->rq)) {
		if ((mp = allocb(sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((struct T_discon_ind *) mp->b_wptr)++;
			p->PRIM_type = T_DISCON_IND;
			p->DISCON_reason = reason;
			p->SEQ_number = (ulong) seq;
			if (seq) {
				bufq_unlink(&sp->conq, seq);
				freemsg(seq);
			}
			if (!bufq_length(&sp->conq))
				sp->i_state = TS_IDLE;
			else
				sp->i_state = TS_WRES_CIND;
			putnext(sp->rq, mp);
			return (0);
		}
		seldom();
		return (-ENOBUFS);
	}
	seldom();
	return (-EBUSY);
}

/*
 *  T_DATA_IND      14 - data indication
 *  -----------------------------------------------------------------
 *  This indication is only useful for delivering data indications for the default stream.
 *  The caller should check that ppi and sid match the default before using this indication.
 *  Otherwise the caller should use the T_OPTDATA_IND.
 */
STATIC int
t_data_ind(sctp_t * sp, ulong more, mblk_t *dp)
{
	mblk_t *mp;
	struct T_data_ind *p;
	ensure(((1 << sp->i_state) & (TSF_DATA_XFER | TSF_WIND_ORDREL)), return (-EFAULT));
	if (canputnext(sp->rq)) {
		if ((mp = allocb(sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((struct T_data_ind *) mp->b_wptr)++;
			p->PRIM_type = T_DATA_IND;
			p->MORE_flag = more;
			mp->b_cont = dp;
			putnext(sp->rq, mp);
			return (0);
		}
		seldom();
		return (-ENOBUFS);
	}
	seldom();
	return (-EBUSY);
}

/*
 *  T_EXDATA_IND    15 - expedited data indication
 *  -----------------------------------------------------------------
 *  This indication is only useful for delivering data indications for the default stream.
 *  The caller should check that ppi and ssn match the default before using this indication.
 *  Otherwise the caller should use the T_OPTDATA_IND.
 */
STATIC int
t_exdata_ind(sctp_t * sp, ulong more, mblk_t *dp)
{
	mblk_t *mp;
	struct T_exdata_ind *p;
	ensure(((1 << sp->i_state) & (TSF_DATA_XFER | TSF_WIND_ORDREL)), return (-EFAULT));
	if (canputnext(sp->rq)) {
		if ((mp = allocb(sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = 1;	/* expedite */
			p = ((struct T_exdata_ind *) mp->b_wptr)++;
			p->PRIM_type = T_EXDATA_IND;
			p->MORE_flag = more;
			mp->b_cont = dp;
			putnext(sp->rq, mp);
			return (0);
		}
		seldom();
		return (-ENOBUFS);
	}
	seldom();
	return (-EBUSY);
}

/*
 *  T_INFO_ACK      16 - information acknowledgement
 *  -----------------------------------------------------------------
 *  Although there is no limit on CDATA and DDATA size, if these are too large then we will
 *  IP fragment the message.
 */
STATIC int
t_info_ack(sctp_t * sp)
{
	mblk_t *mp;
	struct T_info_ack *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((struct T_info_ack *) mp->b_wptr)++;
		p->PRIM_type = T_INFO_ACK;
		p->TSDU_size = -1;	/* no limit on TSDU size */
		p->ETSDU_size = -1;	/* no limit on ETSDU size */
		p->CDATA_size = -1;	/* no limit on CDATA size */
		p->DDATA_size = -1;	/* no limit on DDATA size */
		p->ADDR_size = -1;	/* no limit on ADDR size */
		p->OPT_size = -1;	/* no limit on OPTIONS size */
		p->TIDU_size = -1;	/* no limit on TIDU size */
		p->SERV_type = T_COTS_ORD;	/* COTS with orderly release */
		p->CURRENT_state = sp->i_state;
		p->PROVIDER_flag = XPG4_1 & ~T_SNDZERO;
		putnext(sp->rq, mp);
		return (0);
	}
	seldom();
	return (-ENOBUFS);
}

/*
 *  T_BIND_ACK      17 - bind acknowledgement
 *  -----------------------------------------------------------------
 */
STATIC int
t_bind_ack(sctp_t * sp)
{
	mblk_t *mp;
	struct T_bind_ack *p;
	struct sctp_saddr *ss = sp->saddr;
	size_t add_len = sp->sanum ? sizeof(sp->sport) + sp->sanum * sizeof(ss->saddr) : 0;
	ensure((sp->i_state == TS_WACK_BREQ), return (-EFAULT));
	if ((mp = allocb(sizeof(*p) + add_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((struct T_bind_ack *) mp->b_wptr)++;
		p->PRIM_type = T_BIND_ACK;
		p->ADDR_length = add_len;
		p->ADDR_offset = add_len ? sizeof(*p) : 0;
		p->CONIND_number = sp->conind;
		if (ss)
			*((typeof(sp->sport) *) mp->b_wptr)++ = sp->sport;
		for (; ss; ss = ss->next)
			*((typeof(ss->saddr) *) mp->b_wptr)++ = ss->saddr;
		sp->i_state = TS_IDLE;
		putnext(sp->rq, mp);
		return (0);
	}
	seldom();
	return (-ENOBUFS);
}

/*
 *  T_ERROR_ACK     18 - error acknowledgement
 *  -----------------------------------------------------------------
 */
STATIC int
t_error_ack(sctp_t * sp, ulong prim, long err)
{
	mblk_t *mp;
	struct T_error_ack *p;
	switch (err) {
	case -EBUSY:
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
		seldom();
		return (err);
	case 0:
		never();
		return (err);
	}
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((struct T_error_ack *) mp->b_wptr)++;
		p->PRIM_type = T_ERROR_ACK;
		p->ERROR_prim = prim;
		p->TLI_error = err < 0 ? TSYSERR : err;
		p->UNIX_error = err < 0 ? -err : 0;
		switch (sp->i_state) {
#ifdef TS_WACK_OPTREQ
		case TS_WACK_OPTREQ:
#endif
		case TS_WACK_UREQ:
		case TS_WACK_CREQ:
			sp->i_state = TS_IDLE;
			break;
		case TS_WACK_BREQ:
			sp->i_state = TS_UNBND;
			break;
		case TS_WACK_CRES:
			sp->i_state = TS_WRES_CIND;
			break;
		case TS_WACK_DREQ6:
			sp->i_state = TS_WCON_CREQ;
			break;
		case TS_WACK_DREQ7:
			sp->i_state = TS_WRES_CIND;
			break;
		case TS_WACK_DREQ9:
			sp->i_state = TS_DATA_XFER;
			break;
		case TS_WACK_DREQ10:
			sp->i_state = TS_WIND_ORDREL;
			break;
		case TS_WACK_DREQ11:
			sp->i_state = TS_WREQ_ORDREL;
			break;
			/*
			 *  Note: if we are not in a WACK state we simply do
			 *  not change state.  This occurs normally when we
			 *  send TOUTSTATE or TNOTSUPPORT or are responding to
			 *  a T_OPTMGMT_REQ in other then TS_IDLE state.
			 */
		}
		putnext(sp->rq, mp);
		return (0);
	}
	seldom();
	return (-ENOBUFS);
}

/*
 *  T_OK_ACK        19 - success acknowledgement
 *  -----------------------------------------------------------------
 */
STATIC int
t_ok_ack(sctp_t * sp, ulong prim, ulong seq, ulong tok)
{
	mblk_t *mp;
	struct T_ok_ack *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((struct T_ok_ack *) mp->b_wptr)++;
		p->PRIM_type = T_OK_ACK;
		p->CORRECT_prim = prim;
		switch (sp->i_state) {
		case TS_WACK_CREQ:
			sp->i_state = TS_WCON_CREQ;
			break;
		case TS_WACK_UREQ:
			sp->i_state = TS_UNBND;
			break;
		case TS_WACK_CRES:
		{
			queue_t *aq = (queue_t *) tok;
			sctp_t *ap = (sctp_t *) aq->q_ptr;
			if (ap) {
				ap->i_state = TS_DATA_XFER;
				sctp_cleanup_read(sp);	/* deliver to user what is possible */
				sctp_transmit_wakeup(ap);	/* reply to peer what is necessary */
			}
			if (seq) {
				bufq_unlink(&sp->conq, (mblk_t *) seq);
				freemsg((mblk_t *) seq);
			}
			if (aq != sp->rq) {
				if (bufq_length(&sp->conq))
					sp->i_state = TS_WRES_CIND;
				else
					sp->i_state = TS_IDLE;
			}
			break;
		}
		case TS_WACK_DREQ7:
			if (seq) {
				bufq_unlink(&sp->conq, (mblk_t *) seq);
				freemsg((mblk_t *) seq);
			}
		case TS_WACK_DREQ6:
		case TS_WACK_DREQ9:
		case TS_WACK_DREQ10:
		case TS_WACK_DREQ11:
			if (bufq_length(&sp->conq))
				sp->i_state = TS_WRES_CIND;
			else
				sp->i_state = TS_IDLE;
			break;
			/*
			 *  Note: if we are not in a WACK state we simply do
			 *  not change state.  This occurs normally when we
			 *  are responding to a T_OPTMGMT_REQ in other than
			 *  the TS_IDLE state.
			 */
		}
		putnext(sp->rq, mp);
		return (0);
	}
	seldom();
	return (-ENOBUFS);
}

/*
 *  T_OPTMGMT_ACK   22 - options management acknowledgement
 *  -----------------------------------------------------------------
 */
STATIC int
t_optmgmt_ack(sctp_t * sp, ulong flags, sctp_opts_t * ops)
{
	mblk_t *mp;
	size_t opt_len = sctp_opts_size(flags, sp, ops);
	struct T_optmgmt_ack *p;
	if ((mp = allocb(sizeof(*p) + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((struct T_optmgmt_ack *) mp->b_wptr)++;
		p->PRIM_type = T_OPTMGMT_ACK;
		p->OPT_length = opt_len;
		p->OPT_offset = opt_len ? sizeof(*p) : 0;
		p->MGMT_flags = flags;
		sctp_build_opts(flags, sp, ops, &mp->b_wptr);
#ifdef TS_WACK_OPTREQ
		if (sp->i_state == TS_WACK_OPTREQ)
			sp->i_state = TS_IDLE;
#endif
		putnext(sp->rq, mp);
		return (0);
	}
	seldom();
	return (-ENOBUFS);
}

/*
 *  T_ORDREL_IND    23 - orderly release indication
 *  -----------------------------------------------------------------
 */
STATIC int
t_ordrel_ind(sctp_t * sp)
{
	mblk_t *mp;
	struct T_ordrel_ind *p;
	ensure(((1 << sp->i_state) & (TSF_DATA_XFER | TSF_WIND_ORDREL)), return (-EFAULT));
	if (canputnext(sp->rq)) {
		if ((mp = allocb(sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((struct T_ordrel_ind *) mp->b_wptr)++;
			p->PRIM_type = T_ORDREL_IND;
			switch (sp->i_state) {
			case TS_DATA_XFER:
				sp->i_state = TS_WREQ_ORDREL;
				break;
			case TS_WIND_ORDREL:
				sp->i_state = TS_IDLE;
				break;
			}
			putnext(sp->rq, mp);
			return (0);
		}
		seldom();
		return (-ENOBUFS);
	}
	seldom();
	return (-EBUSY);
}

/*
 *  T_OPTDATA_IND   26 - data with options indication
 *  -----------------------------------------------------------------
 */
STATIC int
t_optdata_ind(sctp_t * sp, uint32_t ppi, uint16_t sid, uint16_t ssn, uint32_t tsn, uint ord,
	      uint more, mblk_t *dp)
{
	mblk_t *mp;
	struct t_opthdr *oh;
	struct T_optdata_ind *p;
	size_t str_len = sizeof(*oh) + sizeof(t_scalar_t);
	size_t opt_len = 0;
	ensure(((1 << sp->i_state) & (TSF_DATA_XFER | TSF_WREQ_ORDREL)), return (-EFAULT));
	if (canputnext(sp->rq)) {
		if (sp->i_flags & TF_SCTP_RECVOPT)
			opt_len = 4 * str_len;
		if ((mp = allocb(sizeof(*p) + opt_len, BPRI_MED))) {

			mp->b_datap->db_type = M_PROTO;
			mp->b_band = ord ? 0 : 1;	/* expedite */
			p = ((struct T_optdata_ind *) mp->b_wptr)++;
			p->PRIM_type = T_OPTDATA_IND;
			p->DATA_flag = (more ? T_ODF_MORE : 0) | (ord ? 0 : T_ODF_EX);
			p->OPT_length = opt_len;
			p->OPT_offset = opt_len ? sizeof(*p) : 0;

			/*
			   indicate options 
			 */
			if (sp->i_flags & TF_SCTP_RECVOPT) {
				oh = ((struct t_opthdr *) mp->b_wptr)++;
				oh->len = str_len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_PPI;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) mp->b_wptr)++ = ppi;

				oh = ((struct t_opthdr *) mp->b_wptr)++;
				oh->len = str_len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_SID;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) mp->b_wptr)++ = sid;

				oh = ((struct t_opthdr *) mp->b_wptr)++;
				oh->len = str_len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_SSN;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) mp->b_wptr)++ = ssn;

				oh = ((struct t_opthdr *) mp->b_wptr)++;
				oh->len = str_len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_TSN;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) mp->b_wptr)++ = tsn;
			}
			mp->b_cont = dp;
			putnext(sp->rq, mp);
			return (0);
		}
		seldom();
		return (-ENOBUFS);
	}
	seldom();
	return (-EBUSY);
}

/*
 *  T_ADDR_ACK      27 - address acknowledgement
 *  -----------------------------------------------------------------
 */
STATIC int
t_addr_ack(sctp_t * sp)
{
	mblk_t *mp;
	struct T_addr_ack *p;
	struct sctp_saddr *ss = sp->saddr;
	struct sctp_daddr *sd = sp->daddr;
	size_t loc_len = sp->sanum ? sizeof(sp->sport) + sp->sanum * sizeof(ss->saddr) : 0;
	size_t rem_len = sp->danum ? sizeof(sp->dport) + sp->danum * sizeof(sd->daddr) : 0;
	if ((mp = allocb(sizeof(*p) + loc_len + rem_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((struct T_addr_ack *) mp->b_wptr)++;
		p->PRIM_type = T_ADDR_ACK;
		p->LOCADDR_length = loc_len;
		p->LOCADDR_offset = loc_len ? sizeof(*p) : 0;
		p->REMADDR_length = rem_len;
		p->REMADDR_offset = rem_len ? sizeof(*p) + loc_len : 0;
		if (ss)
			*((typeof(sp->sport) *) mp->b_wptr)++ = sp->sport;
		for (; ss; ss = ss->next)
			*((typeof(ss->saddr) *) mp->b_wptr)++ = ss->saddr;
		if (sd)
			*((typeof(sp->dport) *) mp->b_wptr)++ = sp->dport;
		for (; sd; sd = sd->next)
			*((typeof(sd->daddr) *) mp->b_wptr)++ = sd->daddr;
		putnext(sp->rq, mp);
		return (0);
	}
	seldom();
	return (-ENOBUFS);
}

#if 0
/*
 *  T_CAPABILITY_ACK ?? - protocol capability ack
 *  -----------------------------------------------------------------
 */
STATIC int
t_capability_ack(sctp_t * sp, ulong caps)
{
	mblk_t *mp;
	struct T_capability_ack *p;
	uint caps = (acceptor ? TC1_ACCEPTOR_ID : 0) | (info ? TC1_INFO : 0);
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((struct T_capability_ack *) mp->b_wptr)++;
		p->PRIM_type = T_CAPABILITY_ACK;
		p->CAP_bits1 = caps;
		p->ACCEPTOR_id = (caps & TC1_ACCEPTOR_ID) ? (ulong) sp->rq : 0;
		if (caps & TC1_INFO) {
			p->INFO_ack.PRIM_type = T_INFO_ACK;
			p->INFO_ack.TSDU_size = sp->tsdu;
			p->INFO_ack.ETSDU_size = sp->etsdu;
			p->INFO_ack.CDATA_size = sp->cdata;
			p->INFO_ack.DDATA_size = sp->ddata;
			p->INFO_ack.ADDR_size = sp->addlen;
			p->INFO_ack.OPT_size = sp->optlen;
			p->INFO_ack.TIDU_size = sp->tidu;
			p->INFO_ack.SERV_type = sp->stype;
			p->INFO_ack.CURRENT_state = sp->i_state;
			p->INFO_ack.PROVIDER_flag = sp->ptype;
		} else
			bzero(&p->INFO_ack, sizeof(p->INFO_ack));
		putnext(sp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
#endif

/*
 *  NOTES:- TPI cannot do data acknowledgements, resets or retrieval.  Data
 *  acknowledgements and retrieval are different forms of the same service.
 *  For data acknowledgement, use the NPI interface.  For reset support (SCTP
 *  Restart indication different from SCTP CDI), use the NPI interface.
 */
STATIC int
sctp_t_conn_ind(sctp_t * sp, mblk_t *cp)
{
	// ptrace(("sp = %x, CONN_IND: seq = %x\n", (uint)sp, (uint)cp));
	return t_conn_ind(sp, cp);
}
STATIC int
sctp_t_conn_con(sctp_t * sp)
{
	// ptrace(("sp = %x, CONN_CONF\n", (uint)sp));
	return t_conn_con(sp);
}
STATIC int
sctp_t_data_ind(sctp_t * sp, uint32_t ppi, uint16_t sid, uint16_t ssn, uint32_t tsn, uint ord,
		uint more, mblk_t *dp)
{
	// ptrace(("sp = %x, DATA_IND: ppi=%u,sid=%u,ssn=%u,tsn=%u,ord=%u,more=%u\n", (uint)sp,
	// ppi,sid,ssn,tsn,ord,more));
	if (sp->i_flags & TF_SCTP_RECVOPT)
		return t_optdata_ind(sp, ppi, sid, ssn, tsn, ord, more, dp);
	if (ord)
		return t_data_ind(sp, more, dp);
	else
		return t_exdata_ind(sp, more, dp);
}
STATIC int
sctp_t_discon_ind(sctp_t * sp, ulong orig, long reason, mblk_t *cp)
{
	// ptrace(("sp = %x, DISCON_IND\n", (uint)sp));
	(void) orig;
	return t_discon_ind(sp, reason, cp);
}
STATIC int
sctp_t_ordrel_ind(sctp_t * sp)
{
	// ptrace(("sp = %x, ORDREL_IND\n", (uint)sp));
	return t_ordrel_ind(sp);
}
STATIC struct sctp_ifops t_ops = {
	sctp_t_conn_ind,
	sctp_t_conn_con,
	sctp_t_data_ind,
	NULL,
	NULL,
	NULL,
	sctp_t_discon_ind,
	sctp_t_ordrel_ind,
	NULL,
	NULL
};

/*
 *  =========================================================================
 *
 *  SCTP T-User --> T-Provider Primitives (Request and Response)
 *
 *  =========================================================================
 *  These represent primitive requests and responses from the Transport Provider Interface
 *  (TPI) transport user.  Each of these requests or responses invoked a protocol action
 *  depending on the current state of the provider.
 */

/*
 *  T_CONN_REQ           0 - TC requeset
 *  -------------------------------------------------------------------------
 *  We have received a connection request from the user.  We use the characteristics of the
 *  primitive and the options on the stream to formulate and INIT message and send it to the
 *  peer.  We acknowledge the success or failure to starting this process to the user.  Once
 *  the process is started, a successful connection will conclude with COOKIE_ACK message
 *  that will generate a T_CONN_CON.  If the init process times out or there are other errors
 *  associated with establishing the SCTP association, they will be returned via the
 *  T_DISCON_IND primitive.
 *
 *  The time-sequence diagrams look like this:
 *
 *                       |                            |                      
 *   T_CONN_REQ -------->|---+ TS_WACK_CREQ           |                      
 *                       |   |                        |                      
 *   T_ERROR_ACK <-------|<--+ TS_IDLE                |                      
 *                       |                            |                      
 *                       |           INIT             |                      
 *   T_CONN_REQ -------->|---+----------------------->|---+                  
 *                       |   | TS_WACK_CREQ           |   |                  
 *   T_OK_ACK <----------|<--+ TS_WCON_CREQ           |   |  Listener Stream 
 *                       |                            |   |  w/ Options      
 *                       |           ABORT            |   |                  
 *   T_DISCON_IND <------|<----TS_IDLE----------------|<--+                  
 *                       |    Timeout, other problem  |                      
 *                       |                            |                      
 *                       |           INIT             |                      
 *   T_CONN_REQ --+----->|---+----------------------->|---+                  
 *                |      |   | TS_WACK_CREQ           |   |                  
 *   T_OK_ACK <----------|<--+ TS_WCON_CREQ           |   |  Listener Stream 
 *                |      |                            |   |  w/ Options      
 *                |      |         INIT ACK           |   |                  
 *                |  +---|<---------------------------|<--+                  
 *                |  |   |       COOKIE ECHO          |                      
 *           DATA +--+-->|--------------------------->|----------> T_CONN_IND
 *                       |                            |                      
 *                       |       COOKIE ACK           |                      
 *   T_CONN_CON <--------|<---------------------------|<---------- T_CONN_RES
 *                       |     TS_DATA_XFER           |                      
 *                       |                            |                      
 *                       |                            |                      
 */

STATIC int
t_conn_req(sctp_t * sp, mblk_t *mp)
{
	int err = -EFAULT;
	struct sctp_addr *a;
	size_t anum;
	const struct T_conn_req *p = (struct T_conn_req *) mp->b_rptr;
	if (sp->i_state != TS_IDLE)
		goto outstate;
	sp->i_state = TS_WACK_CREQ;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
		goto badopt;
	a = (struct sctp_addr *) (mp->b_rptr + p->DEST_offset);
	anum = (p->DEST_length - sizeof(a->port)) / sizeof(a->addr[0]);
	if (mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
		goto badaddr;
	if ((!anum || p->DEST_length != sizeof(a->port) + anum * sizeof(a->addr[0])) || !a->port)
		goto badaddr;
	if (sp->cred.cr_uid != 0 && a->port < 1024)
		goto access;
	{
		struct t_opthdr *rto = NULL;
		struct t_opthdr *hb = NULL;
		/*
		   address per-association options 
		 */
		if (p->OPT_length) {
			unsigned char *op = mp->b_rptr + p->OPT_offset;
			unsigned char *oe = op + p->OPT_length;
			struct t_opthdr *oh = (struct t_opthdr *) op;
			err = 0;
			for (;
			     op + sizeof(*oh) <= oe && oh->len >= sizeof(*oh) && op + oh->len <= oe;
			     op += oh->len, oh = (struct t_opthdr *) op) {
				t_scalar_t val = *((t_scalar_t *) (oh + 1));
				if (oh->level == T_INET_SCTP)
					switch (oh->name) {
					case T_SCTP_ISTREAMS:
						sp->max_istr = val ? val : 1;
						continue;
					case T_SCTP_OSTREAMS:
						sp->req_ostr = val ? val : 1;
						continue;
					case T_SCTP_SID:
						val = min(val, 0);
						val = max(val, sp->n_ostr);
						sp->sid = val;
						continue;
					case T_SCTP_PPI:
						sp->ppi = val;
						continue;
					case T_SCTP_SACK_DELAY:
						val = min(val, 10);
						sp->max_sack = val;
						continue;
					case T_SCTP_PATH_MAX_RETRANS:
						val = min(val, 0);
						sp->rtx_path = val;
						continue;
					case T_SCTP_ASSOC_MAX_RETRANS:
						val = min(val, 0);
						sp->max_retrans = val;
						continue;
					case T_SCTP_HEARTBEAT_ITVL:
						val = min(val, 10);
						sp->hb_itvl = val;
						continue;
					case T_SCTP_RTO_INITIAL:
						val = min(val, 10);
						sp->rto_ini = val;
						continue;
					case T_SCTP_RTO_MIN:
						val = min(val, 10);
						val = min(val, sp->rto_max);
						sp->rto_min = val;
						continue;
					case T_SCTP_RTO_MAX:
						val = min(val, 10);
						val = min(val, sp->rto_min);
						sp->rto_max = val;
						continue;
					case T_SCTP_HB:
						hb = oh;
						continue;
					case T_SCTP_RTO:
						rto = oh;
						continue;
					}
				err = TBADOPT;
				goto error;
			}
			if (op != oe)
				err = TBADOPT;
			if (err)
				goto error;
		}
#if 0
		/*
		   allocate addresses now 
		 */
		if ((err = sctp_alloc_daddrs(sp, a->port, a->addr, anum)))
			goto error;
		/*
		   address per-destination options 
		 */
		if (rto) {
			struct sctp_daddr
			*sd;
			t_sctp_rto_t *op = (t_sctp_rto_t *) rto->value;
			t_sctp_rto_t *oe = (t_sctp_rto_t *) (((caddr_t) rto) + rto->len);
			for (; op + 1 <= oe; op++) {
				if ((sd = sctp_find_daddr(sp, op->rto_dest))) {
					sd->rto = op->rto_initial;
					sd->rto_min = op->rto_min;
					sd->rto_max = op->rto_max;
					sd->max_retrans = op->max_retrans;
					continue;
				}
				err = TBADOPT;
				goto error;
			}
			if (op != oe)
				err = TBADOPT;
			if (err)
				goto error;
		}
		if (hb) {
			struct sctp_daddr
			*sd;
			t_sctp_hb_t *op = (t_sctp_hb_t *) hb->value;
			t_sctp_hb_t *oe = (t_sctp_hb_t *) (((caddr_t) hb) + hb->len);
			for (; op + 1 <= oe; op++) {
				if ((sd = sctp_find_daddr(sp, op->hb_dest))) {
					sd->hb_onoff = op->hb_onoff;
					sd->hb_itvl = op->hb_itvl;
					continue;
				}
				err = TBADOPT;
				goto error;
			}
			if (op != oe)
				err = TBADOPT;
			if (err)
				goto error;
		}
#endif
	}
	if ((err = sctp_conn_req(sp, a->port, a->addr, anum, mp->b_cont)))
		goto error;
	mp->b_cont = NULL;	/* absorbed mp->b_cont */
	return t_ok_ack(sp, T_CONN_REQ, 0, 0);
      access:
	seldom();
	err = TACCES;
	goto error;		/* no permission for requested address */
      badaddr:
	seldom();
	err = TBADADDR;
	goto error;		/* address is unusable */
      badopt:
	seldom();
	err = TBADOPT;
	goto error;		/* options are unusable */
      einval:
	seldom();
	err = -EINVAL;
	goto error;		/* invalid message format */
      outstate:
	seldom();
	err = TOUTSTATE;
	goto error;		/* would place interface out of state */
      error:
	seldom();
	return t_error_ack(sp, T_CONN_REQ, err);
}

/*
 *  T_CONN_RES           1 - Accept previous connection indication
 *  -----------------------------------------------------------------
 */
/*
 *  IMPLEMENTATION NOTE:- Sequence numbers are actually the address of the mblk which
 *  contains the COOKIE-ECHO chunk and contains the cookie as a connection indication.  To
 *  find if a particular sequence number is valid, we walk the connection indication queue
 *  looking for a mblk with the same address as the sequence number.  Sequence numbers are
 *  only valid on the stream for which the connection indication is queued.
 */
STATIC mblk_t *
t_seq_check(sctp_t * sp, ulong seq)
{
	mblk_t *mp = bufq_head(&sp->conq);
	for (; mp && mp != (mblk_t *) seq; mp = mp->b_next) ;
	usual(mp);
	return (mp);
}

sctp_t *sctp_t_list;
STATIC sctp_t *
t_tok_check(ulong acceptor)
{
	sctp_t *ap;
	queue_t *aq = (queue_t *) acceptor;
	for (ap = sctp_t_list; ap && ap->rq != aq; ap = ap->next) ;
	usual(ap);
	return (ap);
}
STATIC int
t_conn_res(sctp_t * sp, mblk_t *mp)
{
	int err = 0;
	mblk_t *cp;
	sctp_t *ap;
	const size_t mlen = mp->b_wptr - mp->b_rptr;
	const struct T_conn_res *p = (struct T_conn_res *) mp->b_rptr;
	if (sp->i_state != TS_WRES_CIND)
		goto outstate;
	sp->i_state = TS_WACK_CRES;
	if (mlen < sizeof(*p))
		goto einval;
	if (mlen < p->OPT_offset + p->OPT_length)
		goto badopt;
	if (!(cp = t_seq_check(sp, p->SEQ_number)))
		goto badseq;
	if (!(ap = t_tok_check(p->ACCEPTOR_id))
	    || (ap != sp && !((1 << ap->i_state) & (TSF_UNBND | TSF_IDLE))))
		goto badf;
	if (ap->i_state == TS_IDLE && ap->conind)
		goto resqlen;
	/*
	   protect at least r00t streams from users 
	 */
	if (sp->cred.cr_uid != 0 && ap->cred.cr_uid != sp->cred.cr_uid)
		goto access;
	{
		uint ap_oldstate = ap->i_state;
		struct t_opthdr *rto = NULL;
		struct t_opthdr *hb = NULL;
		/*
		   address per-association options 
		 */
		if (p->OPT_length) {
			unsigned char *op = mp->b_rptr + p->OPT_offset;
			unsigned char *oe = op + p->OPT_length;
			struct t_opthdr *oh = (struct t_opthdr *) op;
			for (;
			     op + sizeof(*oh) <= oe && oh->len >= sizeof(*oh) && op + oh->len <= oe;
			     op += oh->len, oh = (struct t_opthdr *) op) {
				t_scalar_t val = *((t_scalar_t *) (oh + 1));
				if (oh->level == T_INET_SCTP)
					switch (oh->name) {
					case T_SCTP_SID:
						val = min(val, 0);
						val = max(val, sp->n_ostr);
						sp->sid = val;
						continue;
					case T_SCTP_PPI:
						sp->ppi = val;
						continue;
					case T_SCTP_SACK_DELAY:
						val = min(val, 10);
						sp->max_sack = val;
						continue;
					case T_SCTP_PATH_MAX_RETRANS:
						val = min(val, 0);
						sp->rtx_path = val;
						continue;
					case T_SCTP_ASSOC_MAX_RETRANS:
						val = min(val, 0);
						sp->max_retrans = val;
						continue;
					case T_SCTP_HEARTBEAT_ITVL:
						val = min(val, 10);
						sp->hb_itvl = val;
						continue;
					case T_SCTP_RTO_INITIAL:
						val = min(val, 10);
						sp->rto_ini = val;
						continue;
					case T_SCTP_RTO_MIN:
						val = min(val, 10);
						val = min(val, sp->rto_max);
						sp->rto_min = val;
						continue;
					case T_SCTP_RTO_MAX:
						val = min(val, 10);
						val = min(val, sp->rto_min);
						sp->rto_max = val;
						continue;
					case T_SCTP_HB:
						hb = oh;
						continue;
					case T_SCTP_RTO:
						rto = oh;
						continue;
					}
				goto badopt;
			}
			if (op != oe)
				goto badopt;
			if (err)
				goto error;
		}
#if 0
		/*
		   address per-destination options 
		 */
		if (rto) {
			struct sctp_daddr
			*sd;
			t_sctp_rto_t *op = (t_sctp_rto_t *) rto->value;
			t_sctp_rto_t *oe = (t_sctp_rto_t *) (((caddr_t) rto) + rto->len);
			for (; op + 1 <= oe; op++) {
				if ((sd = sctp_find_daddr(sp, op->rto_dest))) {
					sd->rto = op->rto_initial;
					sd->rto_min = op->rto_min;
					sd->rto_max = op->rto_max;
					sd->max_retrans = op->max_retrans;
					continue;
				}
				goto badopt;
			}
			if (op != oe)
				goto badopt;
			if (err)
				goto error;
		}
		if (hb) {
			struct sctp_daddr
			*sd;
			t_sctp_hb_t *op = (t_sctp_hb_t *) hb->value;
			t_sctp_hb_t *oe = (t_sctp_hb_t *) (((caddr_t) hb) + hb->len);
			for (; op + 1 <= oe; op++) {
				if ((sd = sctp_find_daddr(sp, op->hb_dest))) {
					sd->hb_onoff = op->hb_onoff;
					sd->hb_itvl = op->hb_itvl;
					continue;
				}
				goto badopt;
			}
			if (op != oe)
				goto badopt;
			if (err)
				goto error;
		}
#endif
		ap->i_state = TS_DATA_XFER;
		if ((err = sctp_conn_res(sp, cp, ap, mp->b_cont))) {
			ap->i_state = ap_oldstate;
			goto error;
		}
		mp->b_cont = NULL;	/* absorbed mp->b_cont */
		return t_ok_ack(sp, T_CONN_RES, (ulong) cp, (ulong) ap);
	}
      access:
	seldom();
	err = TACCES;
	goto error;		/* no access to accepting queue */
      resqlen:
	seldom();
	err = TRESQLEN;
	goto error;		/* accepting queue is listening */
      badf:
	seldom();
	err = TBADF;
	goto error;		/* accepting queue id is invalid */
      badseq:
	seldom();
	err = TBADSEQ;
	goto error;		/* connection ind referenced is invalid */
      badopt:
	seldom();
	err = TBADOPT;
	goto error;		/* options are bad */
      einval:
	seldom();
	err = -EINVAL;
	goto error;		/* invalid primitive format */
      outstate:
	seldom();
	err = TOUTSTATE;
	goto error;		/* would place interface out of state */
      error:
	seldom();
	return t_error_ack(sp, T_CONN_RES, err);
}

/*
 *  T_DISCON_REQ        2 - TC disconnection request
 *  -----------------------------------------------------------------
 *
 *  The time-sequence diagrams for abortive disconnect looks like this:
 *
 *
 *  Refusing a connection indication:
 *
 *                       |                            |                      
 *                       |        COOKIE ECHO         |                      
 *  T_CONN_IND <---------|<---------------------------|                      
 *                       |     TS_WRES_CIND           |                      
 *  T_DISCON_REQ ------->|---+                        |                      
 *                       |   | TS_WACK_DREQ7          |                      
 *  T_ERROR_ACK <--------|<--+                        |                      
 *                       |     TS_WRES_CIND           |                      
 *                       |                            |                      
 *                       |                            |                      
 *                       |        COOKIE ECHO         |                      
 *  T_CONN_IND <---------|<---------------------------|                      
 *    (DATA)             |     TS_WRES_CIND           |                      
 *                       |                            |                      
 *                       |       DATA + ABORT         |                      
 *  T_DISCON_REQ ------->|---+----------------------->|--------> T_DISCON_IND
 *    (DATA)             |   | TS_WACK_DREQ7          |            (DATA)    
 *                       |   |                        |                      
 *  T_OK_ACK <-----------|<--+ TS_IDLE                |                      
 *                       |                            |                      
 *                       |                            |                      
 *
 *  Disconnecting an established connection:
 *
 *                       |                            |                      
 *  T_DISCON_REQ ------->|---+                        |                      
 *    (DATA)             |   |                        |                      
 *  T_ERROR_ACK <--------|<--+                        |                      
 *                       |                            |                      
 *                       |                            |                      
 *                       |                            |                      
 *                       |        DATA + ABORT        |                      
 *  T_DISCON_REQ ------->|---+----------------------->|--------> T_DISCON_IND
 *    (DATA)             |   |                        |            (DATA)    
 *  T_OK_ACK <-----------|<--+                        |                      
 *                       |                            |                      
 *                       |                            |                      
 *                       |                            |                      
 *                       |        DATA + ABORT        |                      
 *  T_DISCON_REQ ------->|---+-------------------+--->|--------> T_DATA_IND  
 *    (DATA)             |   |                   |    |                      
 *  T_OK_ACK <-----------|<--+                   +--->|--------> T_DISCON_IND
 *                       |                            |                      
 *
 *  Any data that is associated wtih the T_DISCON_REQ will be bundled as DATA chunks before
 *  the ABORT chunk in the mesage.  These DATA chunks will be sent out of order and
 *  unreliably on the default stream id and with the default payload protocol identifier in
 *  the hope that it makes it through to the other ULP before the ABORT chunk is processed.
 *
 */
STATIC int
t_discon_req(sctp_t * sp, mblk_t *mp)
{
	int err;
	mblk_t *cp = NULL;
	size_t mlen = mp->b_wptr - mp->b_rptr;
	struct T_discon_req *p = (struct T_discon_req *) mp->b_rptr;
	if (!
	    ((1 << sp->
	      i_state) & (TSF_WCON_CREQ | TSF_WRES_CIND | TSF_DATA_XFER | TSF_WIND_ORDREL |
			  TSF_WREQ_ORDREL)))
		goto outstate;
	switch (sp->i_state) {
	case TS_WCON_CREQ:
		sp->i_state = TS_WACK_DREQ6;
		break;
	case TS_WRES_CIND:
		sp->i_state = TS_WACK_DREQ7;
		break;
	case TS_DATA_XFER:
		sp->i_state = TS_WACK_DREQ9;
		break;
	case TS_WIND_ORDREL:
		sp->i_state = TS_WACK_DREQ10;
		break;
	case TS_WREQ_ORDREL:
		sp->i_state = TS_WACK_DREQ11;
		break;
	default:
		goto outstate;
	}
	if (mlen < sizeof(*p))
		goto einval;
	if (sp->i_state == TS_WACK_DREQ7 && !(cp = t_seq_check(sp, p->SEQ_number)))
		goto badseq;
	if ((err = sctp_discon_req(sp, cp)))
		goto error;
	return t_ok_ack(sp, T_DISCON_REQ, p->SEQ_number, 0);
      badseq:
	seldom();
	err = TBADSEQ;
	goto error;		/* connection ind reference is invalid */
      einval:
	seldom();
	err = -EINVAL;
	goto error;		/* invalid primitive format */
      outstate:
	seldom();
	err = TOUTSTATE;
	goto error;		/* would place interface out of state */
      error:
	seldom();
	return t_error_ack(sp, T_DISCON_REQ, err);
}

/*
 *  T_DATA_REQ          3 - Connection-Mode data transfer request
 *  -----------------------------------------------------------------
 */
STATIC int
t_error_reply(sctp_t * sp, int err)
{
	mblk_t *mp;
	switch (err) {
	case -EBUSY:
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
		seldom();
		return (err);
	case 0:
	case 1:
	case 2:
		never();
		return (err);
	}
	if ((mp = allocb(2, BPRI_HI))) {
		mp->b_datap->db_type = M_ERROR;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		putnext(sp->rq, mp);
		return (0);
	}
	seldom();
	return (-ENOBUFS);
}
STATIC int
t_write(sctp_t * sp, mblk_t *mp)
{
	int err;
	if (sp->i_state == TS_IDLE)
		goto discard;
	if (!((1 << sp->i_state) & (TSF_DATA_XFER | TSF_WREQ_ORDREL)))
		goto oustate;
	{
		ulong ppi = sp->ppi;
		ulong sid = sp->sid;
		ulong ord = 1;
		ulong more = 0;
		ulong rcpt = 0;
		if ((err = sctp_data_req(sp, ppi, sid, ord, more, rcpt, mp)))
			goto error;
		return (1);	/* absorbed */
	}
      oustate:
	seldom();
	err = -EPROTO;
	goto error;		/* would place interface out of state */
      discard:
	seldom();
	return (0);		/* ignore in idle state */
      error:
	seldom();
	return t_error_reply(sp, err);
}
STATIC int
t_data_req(sctp_t * sp, mblk_t *mp)
{
	int err;
	size_t mlen = mp->b_wptr - mp->b_rptr;
	const struct T_data_req *p = (struct T_data_req *) mp->b_rptr;
	if (sp->i_state == TS_IDLE)
		goto discard;
	if (mlen < sizeof(*p))
		goto einval;
	if (!((1 << sp->i_state) & (TSF_DATA_XFER | TSF_WREQ_ORDREL)))
		goto oustate;
	{
		ulong ppi = sp->ppi;
		ulong sid = sp->sid;
		ulong ord = 1;
		ulong more = p->MORE_flag;
		ulong rcpt = 0;
		if ((err = sctp_data_req(sp, ppi, sid, ord, more, rcpt, mp->b_cont)))
			goto error;
		mp->b_cont = NULL;	/* absorbed mp->b_cont */
		return (0);
	}
      oustate:
	seldom();
	err = -EPROTO;
	goto error;		/* would place interface out of state */
      einval:
	seldom();
	err = -EINVAL;
	goto error;		/* invlaid primitive format */
      discard:
	seldom();
	return (0);		/* ignore in idle state */
      error:
	seldom();
	return t_error_reply(sp, err);
}

/*
 *  T_EXDATA_REQ         4 - Expedited data request
 *  -----------------------------------------------------------------
 */
STATIC int
t_exdata_req(sctp_t * sp, mblk_t *mp)
{
	int err;
	size_t mlen = mp->b_wptr - mp->b_rptr;
	const struct T_exdata_req *p = (struct T_exdata_req *) mp->b_rptr;
	if (sp->i_state == TS_IDLE)
		goto discard;
	if (mlen < sizeof(*p))
		goto einval;
	if (!((1 << sp->i_state) & (TSF_DATA_XFER | TSF_WREQ_ORDREL)))
		goto oustate;
	{
		ulong ppi = sp->ppi;
		ulong sid = sp->sid;
		ulong ord = 0;
		ulong more = p->MORE_flag;
		ulong rcpt = 0;
		if ((err = sctp_data_req(sp, ppi, sid, ord, more, rcpt, mp->b_cont)))
			goto error;
		mp->b_cont = NULL;	/* absorbed mp->b_cont */
		return (0);
	}
      oustate:
	seldom();
	err = -EPROTO;
	goto error;		/* would place interface out of state */
      einval:
	seldom();
	err = -EINVAL;
	goto error;		/* invlaid primitive format */
      discard:
	seldom();
	return (0);
      error:
	seldom();
	return t_error_reply(sp, err);
}

/*
 *  T_INFO_REQ           5 - Information Request
 *  -----------------------------------------------------------------
 */
STATIC int
t_info_req(sctp_t * sp, mblk_t *mp)
{
	(void) mp;
	return t_info_ack(sp);
}

/*
 *  T_BIND_REQ           6 - Bind a TS user to a transport address
 *  -----------------------------------------------------------------
 */
STATIC int
t_bind_req(sctp_t * sp, mblk_t *mp)
{
	int err;
	const size_t mlen = mp->b_wptr - mp->b_rptr;
	const struct T_bind_req *p = (struct T_bind_req *) mp->b_rptr;
	if (sp->i_state != TS_UNBND)
		goto outstate;
	sp->i_state = TS_WACK_BREQ;
	if (mlen < sizeof(*p))
		goto einval;
	{
		struct sctp_addr *a = (struct sctp_addr *) (mp->b_rptr + p->ADDR_offset);
		size_t anum = (p->ADDR_length - sizeof(a->port)) / sizeof(a->addr[0]);
		if ((mlen < p->ADDR_offset + p->ADDR_length) ||
		    (p->ADDR_length != sizeof(*a) + anum * sizeof(a->addr[0])))
			goto badaddr;
		/*
		   we don't allow wildcards just yet 
		 */
		if (!anum || (!a->port && !(a->port = sctp_get_port())))
			goto noaddr;
		if (sp->cred.cr_uid != 0 && a->port < 1024)
			goto acces;
		if ((err = sctp_bind_req(sp, a->port, a->addr, anum, p->CONIND_number)))
			goto error;
		return t_bind_ack(sp);
	}
      acces:
	seldom();
	err = TACCES;
	goto error;		/* no permission for requested address */
      noaddr:
	seldom();
	err = TNOADDR;
	goto error;		/* cound not allocatea address */
      badaddr:
	seldom();
	err = TBADADDR;
	goto error;		/* address is invalid */
      einval:
	seldom();
	err = -EINVAL;
	goto error;		/* invalid primitive format */
      outstate:
	seldom();
	err = TOUTSTATE;
	goto error;		/* would place interface out of state */
      error:
	seldom();
	return t_error_ack(sp, T_BIND_REQ, err);
}

/*
 *  T_UNBIND_REQ         7 - Unbind TS user from transport address
 *  -----------------------------------------------------------------
 */
STATIC int
t_unbind_req(sctp_t * sp, mblk_t *mp)
{
	int err;
	const struct T_unbind_req *p = (struct T_unbind_req *) mp->b_rptr;
	(void) p;
	if (sp->i_state != TS_IDLE)
		goto outstate;
	sp->i_state = TS_WACK_UREQ;
	if ((err = sctp_unbind_req(sp)))
		goto error;
	return t_ok_ack(sp, T_UNBIND_REQ, 0, 0);
      outstate:
	seldom();
	err = TOUTSTATE;
	goto error;		/* would place interface out of state */
      error:
	seldom();
	return t_error_ack(sp, T_UNBIND_REQ, err);
}

/*
 *  T_OPTMGMT_REQ        9 - Options management request
 *  -----------------------------------------------------------------
 *  The T_OPTMGMT_REQ is responsible for establishing options while the stream
 *  is in the T_IDLE state.  When the stream is bound to a local address using
 *  the T_BIND_REQ, the settings of options with end-to-end significance will
 *  have an affect on how the driver response to an INIT with INIT-ACK for SCTP.
 *  For example, the bound list of addresses is the list of addresses that will
 *  be sent in the INIT-ACK.  The number of inbound streams and outbound streams
 *  are the numbers that will be used in the INIT-ACK.
 */

/*
 *  Errors:
 *
 *  TACCES:     the user did not have proper permissions for the user of the requested
 *              options.
 *
 *  TBADFLAG:   the flags as specified were incorrect or invalid.
 *
 *  TBADOPT:    the options as specified were in an incorrect format, or they contained
 *              invalid information.
 *
 *  TOUTSTATE:  the primitive would place the transport interface out of state.
 *
 *  TNOTSUPPORT: this prmitive is not supported.
 *
 *  TSYSERR:    a system error has occured and the UNIX system error is indicated in the
 *              primitive.
 */

STATIC int
t_optmgmt_req(sctp_t * sp, mblk_t *mp)
{
	int err = 0;
	const size_t mlen = mp->b_wptr - mp->b_rptr;
	const struct T_optmgmt_req *p = (struct T_optmgmt_req *) mp->b_rptr;
#ifdef TS_WACK_OPTREQ
	if (sp->i_state == TS_IDLE)
		sp->i_state = TS_WACK_OPTREQ;
#endif
	if (mlen < sizeof(*p))
		goto einval;
	if (mlen < p->OPT_offset + p->OPT_length)
		goto badopt;
	{
		ulong flags = p->MGMT_flags;
		size_t opt_len = p->OPT_length;
		unsigned char *opt_ptr = mp->b_rptr + p->OPT_offset;
		struct sctp_opts ops = { 0L, NULL, };
		struct sctp_opts *opsp = NULL;
		if (opt_len) {
			if ((err = sctp_parse_opts(&ops, opt_ptr, opt_len)))
				goto error;
			opsp = &ops;
		}
		switch (flags) {
		case T_CHECK:
			sctp_check_opts(sp, opsp);
			return t_optmgmt_ack(sp, flags, opsp);
		case T_NEGOTIATE:
			sctp_negotiate_opts(sp, opsp);
			return t_optmgmt_ack(sp, flags, opsp);
		case T_DEFAULT:
			/*
			   return defaults for the specified options 
			 */
		case T_CURRENT:
			return t_optmgmt_ack(sp, flags, opsp);
		default:
			goto badflag;
		}
	}
      badflag:
	seldom();
	err = TBADFLAG;
	goto error;		/* bad options flags */
      badopt:
	seldom();
	err = TBADOPT;
	goto error;		/* options were invalid */
      einval:
	seldom();
	err = -EINVAL;
	goto error;		/* invalid primitive format */
      error:
	seldom();
	return t_error_ack(sp, T_OPTMGMT_REQ, err);
}

/*
 *  T_ORDREL_REQ        10 - TS user is finished sending
 *  -----------------------------------------------------------------
 *
 *  The time-sequence diagrams look like this:
 *
 *                       |                            |                      
 *                  TS_DATA_XFER                 TS_DATA_XFER                
 *                       |            DATA            |                      
 *  T_DATA_REQ --------->|--------------------------->|----------> T_DATA_IND
 *                       |            DATA            |                      
 *  T_DATA_REQ --------->|--------------------------->|----------> T_DATA_IND
 *                       |            DATA            |                      
 *  T_DATA_REQ --------->|--------------------------->|----------> T_DATA_IND
 *                       |            DATA            |                      
 *  T_DATA_REQ --------->|--------------------------->|----------> T_DATA_IND
 *                       |                            |                      
 *  T_ORDREL_REQ ------->|<---------- SACK -----------|                      
 *                 TS_WIND_ORDREL                     |                      
 *                       |<---------- SACK -----------|                      
 *                       |<---------- SACK -----------|                      
 *                       |<---------- SACK -----------|                      
 *                       |         SHUTDOWN           |                      
 *                       |--------------------------->|--------> T_ORDREL_IND
 *                       |                      TS_WREQ_ORDREL               
 *                       |           DATA             |                      
 *  T_DATA_IND <---------|<---+-----------------------|<---------- T_DATA_REQ
 *                       |    |    SHUTDOWN           |                      
 *                       |    +---------------------->|                      
 *                       |           DATA             |                      
 *  T_DATA_IND <---------|<---+-----------------------|<---------- T_DATA_REQ
 *                       |    |    SHUTDOWN           |                      
 *                       |    +---------------------->|                      
 *                       |                            |                      
 *                       |       SHUTDOWN ACK         |                      
 *  T_ORDREL_IND <-------|<---+-----------------------|<-------- T_ORDREL_REQ
 *                    TS_IDLE |                    TS_IDLE                   
 *                       |    |SHUTDOWN COMPLETE      |                      
 *                       |    +---------------------->|                      
 *                       |                            |                      
 *  =====================|============================|======================
 *                       |                            |                      
 *  T_ORDREL_REQ ------->|                            |<-------- T_ORDREL_REQ
 *                 TS_WIND_ORDREL               TS_WIND_ORDREL               
 *                       |<---------- SACK -----------|                      
 *                       |----------- SACK ---------->|                      
 *                       |<---------- SACK -----------|                      
 *                       |----------- SACK ---------->|                      
 *                       |                            |                      
 *                       |<-------- SHUTDOWN ---------|                      
 *                       |--------- SHUTDOWN -------->|                      
 *  T_ORDREL_IND <-------|<--+--- SHUTDOWN ACK -------|                      
 *                       |---|--- SHUTDOWN ACK ---+-->|--------> T_ORDREL_IND
 *                       |   +- SHUTDOWN COMPLETE-|-->|                      
 *                       |<-----SHUTDOWN COMPLETE-+   |                      
 *                       |                            |                      
 *                       |                            |                      
 *                       |                            |                      
 *                       |                            |                      
 */
STATIC int
t_ordrel_req(sctp_t * sp, mblk_t *mp)
{
	int err;
	if (!((1 << sp->i_state) & (TSF_DATA_XFER | TSF_WREQ_ORDREL)))
		goto outstate;
	switch (sp->i_state) {
	case TS_DATA_XFER:
		sp->i_state = TS_WIND_ORDREL;
		break;
	case TS_WREQ_ORDREL:
		sp->i_state = TS_IDLE;
		break;
	}
	if ((err = sctp_ordrel_req(sp)))
		goto error;
	return (0);
      outstate:
	seldom();
	err = -EPROTO;
	goto error;		/* would place interface out of state */
      error:
	seldom();
	return t_error_reply(sp, err);
}

/*
 *  T_OPTDATA_REQ       24- data with options request
 *  -----------------------------------------------------------------
 *  Basically the purpose of this for SCTP is to be able to set the SCTP Unordered Bit
 *  (U-bit), Payload Protocol Identifier (PPI) and Stream Id (sid) associated with the data
 *  message.  If not specified, each option will reduce to the current default settings.
 */
STATIC int
t_optdata_req(sctp_t * sp, mblk_t *mp)
{
	int err;
	const size_t mlen = mp->b_wptr - mp->b_rptr;
	const struct T_optdata_req *p = (struct T_optdata_req *) mp->b_rptr;
	if (sp->i_state == TS_IDLE)
		goto discard;
	if (mlen < sizeof(*p))
		goto einval;
	if ((p->OPT_length && (mlen < p->OPT_offset + p->OPT_length)))
		goto einval;
	if (!((1 << sp->i_state) & (TSF_DATA_XFER | TSF_WREQ_ORDREL)))
		goto outstate;
	{
		ulong ppi = sp->ppi;
		ulong sid = sp->sid;
		ulong ord = !(p->DATA_flag & T_ODF_EX);
		ulong more = (p->DATA_flag & T_ODF_MORE);
		ulong rcpt = 0;
		if (p->OPT_length) {
			unsigned char *op = mp->b_rptr + p->OPT_offset;
			unsigned char *oe = op + p->OPT_length;
			struct t_opthdr *oh = (struct t_opthdr *) op;
			for (; op + sizeof(*oh) <= oe && oh->len >= sizeof(*oh)
			     && op + oh->len <= oe; op += oh->len, oh = (struct t_opthdr *) op) {
				if (oh->level == T_INET_SCTP)
					switch (oh->name) {
					case T_SCTP_SID:
						sid = (*((t_scalar_t *) (oh + 1))) & 0xffff;
						continue;
					case T_SCTP_PPI:
						ppi = (*((t_scalar_t *) (oh + 1))) & 0xffffffff;
						continue;
					}
			}
		}
		if ((err = sctp_data_req(sp, ppi, sid, ord, more, rcpt, mp->b_cont)))
			goto error;
		mp->b_cont = NULL;	/* absorbed mp->b_cont */
		return (0);
	}
      outstate:
	seldom();
	err = -EPROTO;
	goto error;		/* would place interface out of state */
      einval:
	seldom();
	err = -EINVAL;
	goto error;		/* invalid primitive format */
      discard:
	seldom();
	return (0);		/* ignore the idle state */
      error:
	seldom();
	return t_error_reply(sp, err);
}

/*
 *  T_ADDR_REQ          25 - address request
 *  -----------------------------------------------------------------
 */
STATIC int
t_addr_req(sctp_t * sp, mblk_t *mp)
{
	(void) mp;
	return t_addr_ack(sp);
}

/*
 *  =========================================================================
 *
 *  STREAMS Message Handling
 *
 *  =========================================================================
 *
 *  M_PROTO, M_PCPROTO Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sctp_t_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	ulong prim;
	sctp_t *sp = (sctp_t *) q->q_ptr;
	ulong oldstate = sp->i_state;
	switch ((prim = *((ulong *) mp->b_rptr))) {
	case T_CONN_REQ:
		rtn = t_conn_req(sp, mp);
		break;
	case T_CONN_RES:
		rtn = t_conn_res(sp, mp);
		break;
	case T_DISCON_REQ:
		rtn = t_discon_req(sp, mp);
		break;
	case T_DATA_REQ:
		rtn = t_data_req(sp, mp);
		break;
	case T_EXDATA_REQ:
		rtn = t_exdata_req(sp, mp);
		break;
	case T_INFO_REQ:
		rtn = t_info_req(sp, mp);
		break;
	case T_BIND_REQ:
		rtn = t_bind_req(sp, mp);
		break;
	case T_UNBIND_REQ:
		rtn = t_unbind_req(sp, mp);
		break;
	case T_OPTMGMT_REQ:
		rtn = t_optmgmt_req(sp, mp);
		break;
	case T_ORDREL_REQ:
		rtn = t_ordrel_req(sp, mp);
		break;
	case T_OPTDATA_REQ:
		rtn = t_optdata_req(sp, mp);
		break;
	case T_ADDR_REQ:
		rtn = t_addr_req(sp, mp);
		break;
	default:
		rtn = -EOPNOTSUPP;
		break;
	}
	if (rtn < 0) {
		rare();
		sp->i_state = oldstate;
	}
	return (rtn);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_DATA Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sctp_t_w_data(queue_t *q, mblk_t *mp)
{
	sctp_t *sp = (sctp_t *) q->q_ptr;
	return t_write(sp, mp);
}
STATIC int
sctp_t_r_data(queue_t *q, mblk_t *mp)
{
	sctp_t *sp = (sctp_t *) q->q_ptr;
	return sctp_recv_msg(sp, mp);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_ERROR Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sctp_t_r_error(queue_t *q, mblk_t *mp)
{
	sctp_t *sp = (sctp_t *) q->q_ptr;
	rare();
	return sctp_recv_err(sp, mp);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_FLUSH Handling
 *
 *  -------------------------------------------------------------------------
 *  This is complete flush handling in both directions.  Standard stuff.
 */
STATIC int
sctp_t_m_flush(queue_t *q, mblk_t *mp, const uint8_t mflag, const uint8_t oflag)
{
	if (*mp->b_rptr & mflag) {
		if (*mp->b_rptr & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHALL);
		else
			flushq(q, FLUSHALL);
		if (q->q_next) {
			putnext(q, mp);
			return (QR_ABSORBED);
		}
		*mp->b_rptr &= ~mflag;
	}
	if (*mp->b_rptr & oflag) {
		queue_t *oq = q->q_other;
		if (*mp->b_rptr & FLUSHBAND)
			flushband(oq, mp->b_rptr[1], FLUSHALL);
		else
			flushq(oq, FLUSHALL);
		if (oq->q_next) {
			putnext(oq, mp);
			return (QR_ABSORBED);
		}
		*mp->b_rptr &= ~oflag;
	}
	return (0);
}
STATIC int
sctp_t_w_flush(queue_t *q, mblk_t *mp)
{
	return sctp_t_m_flush(q, mp, FLUSHW, FLUSHR);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Other messages (e.g. M_IOCACK)
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sctp_t_r_other(queue_t *q, mblk_t *mp)
{
	sctp_t *sp = SCTP_PRIV(q);
	rare();
	cmn_err(CE_WARN, "Unsupported block type %d on RD(q) %d\n", mp->b_datap->db_type,
		sp->cminor);
	putnext(q, mp);
	return (QR_ABSORBED);
}
STATIC int
sctp_t_w_other(queue_t *q, mblk_t *mp)
{
	sctp_t *sp = SCTP_PRIV(q);
	rare();
	cmn_err(CE_WARN, "Unsupported block type %d on WR(q) %d\n", mp->b_datap->db_type,
		sp->cminor);
	return (-EOPNOTSUPP);
}

/*
 *  =========================================================================
 *
 *  STREAMS PUTQ and SRVQ routines
 *
 *  =========================================================================
 */
/*
 *  TPI Write Message
 */
STATIC INLINE int
sctp_t_w_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return sctp_t_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return sctp_t_w_proto(q, mp);
	case M_FLUSH:
		return sctp_t_w_flush(q, mp);
	default:
		return sctp_t_w_other(q, mp);
	}
}

/*
 *  IP Read Message
 */
STATIC INLINE int
sctp_t_r_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return sctp_t_r_data(q, mp);
	case M_ERROR:
		return sctp_t_r_error(q, mp);
	default:
		return sctp_t_r_other(q, mp);
	}
}

/*
 *  PUTQ Put Routine
 *  -----------------------------------
 */
STATIC INLINE int
sctp_t_putq(queue_t *q, mblk_t *mp, int (*proc) (queue_t *, mblk_t *))
{
	ensure(q, return (-EFAULT));
	ensure(mp, return (-EFAULT));
	if (mp->b_datap->db_type >= QPCTL && !q->q_count && !sctp_trylock(q)) {
		int rtn;
		switch ((rtn = proc(q, mp))) {
		case QR_DONE:
			freemsg(mp);
		case QR_ABSORBED:
			break;
		case QR_TRIMMED:
			freeb(mp);
			break;
		case QR_LOOP:
			if (!q->q_next) {
				qreply(q, mp);
				break;
			}
		case QR_PASSALONG:
			if (q->q_next) {
				putnext(q, mp);
				break;
			}
			rtn = -EOPNOTSUPP;
		default:
			ptrace(("ERROR: (q dropping) %d\n", rtn));
			freemsg(mp);
			break;
		case QR_DISABLE:
			putq(q, mp);
			rtn = 0;
			break;
		case QR_PASSFLOW:
			if (mp->b_datap->db_type >= QPCTL || canputnext(q)) {
				putnext(q, mp);
				break;
			}
		case -ENOBUFS:
		case -EBUSY:
		case -ENOMEM:
		case -EAGAIN:
			putq(q, mp);
			break;
		}
		sctp_unlock(q);
		return (rtn);
	} else {
		seldom();
		putq(q, mp);
		return (0);
	}
}

/*
 *  SRVQ Put Routine
 *  -----------------------------------
 */
STATIC INLINE int
sctp_t_srvq(queue_t *q, int (*proc) (queue_t *, mblk_t *))
{
	ensure(q, return (-EFAULT));
	if (!sctp_waitlock(q)) {
		int rtn = 0;
		mblk_t *mp;
		while ((mp = getq(q))) {
			switch ((rtn = proc(q, mp))) {
			case QR_DONE:
				freemsg(mp);
			case QR_ABSORBED:
				continue;
			case QR_TRIMMED:
				freeb(mp);
				continue;
			case QR_LOOP:
				if (!q->q_next) {
					qreply(q, mp);
					continue;
				}
			case QR_PASSALONG:
				if (q->q_next) {
					putnext(q, mp);
					continue;
				}
				rtn = -EOPNOTSUPP;
			default:
				ptrace(("ERROR: (q dropping) %d\n", rtn));
				freemsg(mp);
				continue;
			case QR_DISABLE:
				ptrace(("ERROR: (q disabling) %d\n", rtn));
				noenable(q);
				putbq(q, mp);
				rtn = 0;
				break;
			case QR_PASSFLOW:
				if (mp->b_datap->db_type >= QPCTL || canputnext(q)) {
					putnext(q, mp);
					continue;
				}
			case -ENOBUFS:	/* proc must schedule bufcall */
			case -EBUSY:	/* proc must fail canput */
			case -ENOMEM:	/* proc must schedule re-enable */
			case -EAGAIN:	/* proc must schedule re-enable */
				if (mp->b_datap->db_type < QPCTL) {
					ptrace(("ERROR: (q stalled) %d\n", rtn));
					putbq(q, mp);
					break;
				}
				if (mp->b_datap->db_type == M_PCPROTO) {
					mp->b_datap->db_type = M_PROTO;
					mp->b_band = 255;
					putq(q, mp);
					break;
				}
				ptrace(("ERROR: (q dropping) %d\n", rtn));
				freemsg(mp);
				continue;
			}
			break;
		}
		sctp_unlock(q);
		return (rtn);
	}
	return (-EAGAIN);
}

STATIC int
sctp_t_rput(queue_t *q, mblk_t *mp)
{
	return (int) sctp_t_putq(q, mp, &sctp_t_r_prim);
}

STATIC int
sctp_t_rsrv(queue_t *q)
{
	return (int) sctp_t_srvq(q, &sctp_t_r_prim);
}

STATIC int
sctp_t_wput(queue_t *q, mblk_t *mp)
{
	return (int) sctp_t_putq(q, mp, &sctp_t_w_prim);
}

STATIC int
sctp_t_wsrv(queue_t *q)
{
	return (int) sctp_t_srvq(q, &sctp_t_w_prim);
}

/*
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 */
sctp_t *sctp_t_list = NULL;

STATIC int
sctp_t_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	int cmajor = getmajor(*devp);
	int cminor = getminor(*devp);
	sctp_t *sp, **spp = &sctp_t_list;
	(void) crp;
	if (q->q_ptr != NULL)
		return (0);	/* already open */
	if (sflag == MODOPEN || WR(q)->q_next) {
		rare();
		return (EIO);	/* can't open as module */
	}
	if (!cminor)
		sflag = CLONEOPEN;
	if (sflag == CLONEOPEN)
		cminor = 1;
	for (; *spp && (*spp)->cmajor < cmajor; spp = &(*spp)->next) ;
	for (; *spp && cminor <= SCTP_T_CMINORS; spp = &(*spp)->next) {
		minor_t dminor = (*spp)->cminor;
		if (cminor < dminor)
			break;
		if (cminor == dminor) {
			if (sflag != CLONEOPEN) {
				rare();
				return (ENXIO);	/* requested device in use */
			}
			cminor++;
		}
	}
	if (cminor > SCTP_T_CMINORS) {
		rare();
		return (ENXIO);
	}
	*devp = makedevice(cmajor, cminor);
	if (!(sp = sctp_alloc_priv(q, spp, cmajor, cminor, &t_ops))) {
		rare();
		return (ENOMEM);
	}
	return (0);
}
STATIC int
sctp_t_close(queue_t *q, int flag, cred_t *crp)
{
	(void) flag;
	(void) crp;
	sctp_free_priv(q);
	return (0);
}

/*
 *  =========================================================================
 *
 *  LiS Module Initialization
 *
 *  =========================================================================
 */
void
sctp_t_init(void)
{
	int cmajor;
	if ((cmajor =
	     lis_register_strdev(SCTP_T_CMAJOR_0, &sctp_t_info, SCTP_T_CMINORS,
				 sctp_t_minfo.mi_idname)) < 0) {
		sctp_t_minfo.mi_idnum = 0;
		rare();
		cmn_err(CE_NOTE, "sctp: couldn't register driver\n");
		return;
	}
	sctp_t_minfo.mi_idnum = cmajor;
}

void
sctp_t_term(void)
{
	if (sctp_t_minfo.mi_idnum) {
		if ((sctp_t_minfo.mi_idnum = lis_unregister_strdev(sctp_t_minfo.mi_idnum))) {
			sctp_t_minfo.mi_idnum = 0;
			rare();
			cmn_err(CE_WARN, "sdt: couldn't unregister driver!\n");
		}
	}
};
