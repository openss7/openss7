/*****************************************************************************

 @(#) $RCSfile: itot.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2006/10/12 10:24:51 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
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

 Last Modified $Date: 2006/10/12 10:24:51 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: itot.c,v $
 Revision 0.9.2.3  2006/10/12 10:24:51  brian
 - removed redundant debug flags, and got itot compiling

 Revision 0.9.2.2  2006/10/10 10:44:15  brian
 - updates for release, lots of additions and workup

 Revision 0.9.2.1  2006/07/11 12:32:05  brian
 - added ISO and other implementations to distribution

 Revision 0.9.2.1  2006/04/11 18:30:11  brian
 - added new ISO over TCP modules

 *****************************************************************************/

#ident "@(#) $RCSfile: itot.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2006/10/12 10:24:51 $"

static char const ident[] =
    "$RCSfile: itot.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2006/10/12 10:24:51 $";

/*
 *  ISO Transport over TCP (ITOT)
 */

#include <sys/os7/compat.h>

/*
 *  These are for TPI definitions.
 */
#if defined HAVE_TIHDR_H
#   include <tidhr.h>
#else
#   include <sys/tihdr.h>
#endif

#include <sys/npi.h>
#include <net/sock.h>
#include <net/ip.h>

#define ITOT_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define ITOT_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define ITOT_REVISION	"OpenSS7 $RCSfile: itot.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2006/10/12 10:24:51 $"
#define ITOT_DEVICE	"SVR 4.2 STREAMS ITOT Module for RFC 2126 (ITOT)"
#define ITOT_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define ITOT_LICENSE	"GPL"
#define ITOT_BANNER	ITOT_DESCRIP	"\n" \
			ITOT_COPYRIGHT	"\n" \
			ITOT_REVISION	"\n" \
			ITOT_DEVICE	"\n" \
			ITOT_CONTACT
#define ITOT_SPLASH	ITOT_DEVICE	" - " \
			ITOT_REVISION

#ifdef LINUX
MODULE_AUTHOR(ITOT_CONTACT);
MODULE_DESCRIPTION(ITOT_DESCRIP);
MODULE_SUPPORTED_DEVICE(ITOT_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(ITOT_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-itot");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifndef ITOT_MOD_NAME
#define ITOT_MOD_NAME		"itot"
#endif				/* ITOT_MOD_NAME */
#ifndef ITOT_MOD_ID
#define ITOT_MOD_ID		0
#endif				/* ITOT_MOD_ID */

/*
 *  STREAMS Definitions
 */

#define MOD_ID		ITOT_MOD_ID
#define MOD_NAME	ITOT_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	ITOT_BANNER
#else				/* MODULE */
#define MOD_BANNER	ITOT_SPLASH
#endif				/* MODULE */

static struct module_info itot_minfo = {
	.mi_idnum = MOD_ID,		/* Module ID number */
	.mi_idname = MOD_NAME,		/* Module name */
	.mi_minpsz = 0,			/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,		/* Max packet size accepted */
	.mi_hiwat = STRHIGH,		/* Hi water mark */
	.mi_lowat = STRLOW,		/* Lo water mark */
};

static struct module_stat itot_rstat __attribute__ ((aligned(SMP_CACHE_BYTES)));
static struct module_stat itot_wstat __attribute__ ((aligned(SMP_CACHE_BYTES)));

static streamscall int itot_open(queue_t *, dev_t *, int, int, cred_t *);
static streamscall int itot_close(queue_t *, int, cred_t *);

static streamscall int itot_rput(queue_t *, mblk_t *);
static streamscall int itot_wput(queue_t *, mblk_t *);

static struct qinit itot_rinit = {
	.qi_putp = itot_rput,		/* Read put (message from below) */
	.qi_qopen = &itot_open,		/* Each open */
	.qi_qclose = &itot_close,	/* Last close */
	.qi_minfo = &itot_minfo,	/* Module information */
	.qi_mstat = &itot_rstat,	/* Module statistics */
};

static struct qinit itot_winit = {
	.qi_putp = itot_wput,		/* Write put (message from above) */
	.qi_minfo = &itot_minfo,	/* Module information */
	.qi_mstat = &itot_wstat,	/* Module statistics */
};

struct streamtab itot_info = {
	.st_rdinit = &itot_rinit,
	.st_wrinit = &itot_winit,
};

/*
 *  PUTP and SRVP, put and service procedures.
 *
 *  ITOT does not provide service procedures.  This is primarily because the message handling across
 *  the module never amplifies message blocks (that is an allocation can never fail causing the
 *  incoming message to be placed onto the queue for later processing by the service procedure).
 */

/*
 *  Read-Side Processing
 */

/**
 * itot_r_proto - process M_(PC)PROTO message block on read side
 * @q: read queue
 * @mp: the message
 *
 * Note that much of this processing is stateless and pretty automatic.  When a request is generated
 * on the write side, we trasnform to an NPI message, pass it down and wait for the response.  Here
 * the response is most often simply translated into the corresponding NPI response.
 */
static streams_fastcall streams_inline __hot_get int
itot_r_proto(queue_t *q, mblk_t *mp)
{
	switch (*(t_scalar_t *) mp->b_rptr) {
	case T_CONN_IND:
	{
		/* Transform into N_CONN_IND. */
		struct prim { N_conn_ind_t prim; unsigned short family; char address[20]; } n = { { N_CONN_IND, }, };
		struct T_conn_ind *t = (typeof(t)) mp->b_rptr;
		struct sockaddr_in *sin = (struct sockaddr_in *)((unsigned char *)t + t->SRC_offset);
		int length = 0;

		(void) sin;
		n.prim.PRIM_type = N_CONN_IND;
		n.prim.DEST_length = 0;
		n.prim.DEST_offset = 0;
		n.prim.SRC_length = t->SRC_length ? 20 : 0;
		n.prim.SRC_offset = sizeof(n);
		n.prim.SEQ_number = t->SEQ_number;
		n.prim.CONN_flags = 0;
		n.prim.QOS_length = 0;
		n.prim.QOS_offset = 0;

		if (n.prim.SRC_length) {
			n.family = AF_X25;
			n.address[0] = 0;
			n.address[1] = 0;
			n.address[2] = 0;

			n.address[3] = 0;
			n.address[4] = 0;
			n.address[5] = 0;
			n.address[6] = 0;

			/* the remainder are zero */

			length = sizeof(n);
		} else
			length = sizeof(n.prim);

	}
	case T_CONN_CON:
	{
		/* Transform into N_CONN_CON. */
		N_conn_con_t n = { N_CONN_CON, };
		struct T_conn_con *t = (typeof(t)) mp->b_rptr;
		(void) n;
		(void) t;
	}
	case T_DISCON_IND:
	{
		/* Transform into N_DISCON_IND. */
		N_discon_ind_t n = { N_DISCON_IND, };
		struct T_discon_ind *t = (typeof(t)) mp->b_rptr;
		(void) n;
		(void) t;
	}
	case T_DATA_IND:
	{
		/* Transform into N_DATA_IND. */
		N_data_ind_t n = { N_DATA_IND, };
		(void) n;
	}
	case T_EXDATA_IND:
	{
		/* Transform into N_EXDATA_IND. */
		N_exdata_ind_t n = { N_EXDATA_IND, };
		(void) n;
	}
	case T_INFO_ACK:
	{
		/* Transform into N_INFO_ACK. */
		N_info_ack_t n = { N_INFO_ACK, };
		(void) n;
	}
	case T_BIND_ACK:
	{
		/* Transform into N_BIND_ACK. */
		N_bind_ack_t n = { N_BIND_ACK, };
		(void) n;
	}
	case T_ERROR_ACK:
	{
		/* Transform into N_ERROR_ACK. */
		N_error_ack_t n = { N_ERROR_ACK, };
		(void) n;
	}
	case T_OK_ACK:
	{
		/* Transform into N_OK_ACK. */
		N_ok_ack_t n = { N_OK_ACK, };
		(void) n;
	}
	case T_UNITDATA_IND:
	{
		/* Transform into N_UNITDATA_IND. */
		N_unitdata_ind_t n = { N_UNITDATA_IND, };
		(void) n;
	}
	case T_UDERROR_IND:
	{
		/* Transform into N_UDERROR_IND. */
		N_uderror_ind_t n = { N_UDERROR_IND, };
		(void) n;
	}
	case T_OPTMGMT_ACK:
	{
		/* Transform into N_OK_ACK. */
		N_ok_ack_t n = { N_OK_ACK, };
		(void) n;
	}
	case T_ORDREL_IND:
	{
		/* Transform into N_DISCON_IND. */
		N_discon_ind_t n = { N_DISCON_IND, };
		(void) n;
	}
	case T_OPTDATA_IND:
	{
		/* Transform into N_QOSDATA_IND, N_DATA_IND or N_EXDATA_IND. */
#ifdef N_QOSDATA_IND
		N_qosdata_ind_t n = { N_QOSDATA_IND, };
		(void) n;
#else
		int isnotexpedited = 0;
		if (isnotexpedited) {
			N_data_ind_t n = { N_DATA_IND, };
			(void) n;
		} else {
			N_exdata_ind_t n = { N_EXDATA_IND, };
			(void) n;
		}
#endif

	}
	case T_ADDR_ACK:
	{
		/* Transform into N_INFO_ACK (well, maybe just store the addresses for later
		   inclusion into an N_INFO_ACK. */
	}
	case T_CAPABILITY_ACK:
	{
		/* Transform into N_INFO_ACK. */
		N_info_ack_t n = { N_INFO_ACK, };
		(void) n;
	}

/*
 *  None of the following messages should appear at the read-side put procedure.  Only upward defined
 *  TPI messages should appear at the read-side put procedure.  Discard these.
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
		__swerr();
	default:
		freemsg(mp);
		return (0);
	}
}

static streamscall int
itot_r_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHALL);
		else
			flushq(q, FLUSHALL);
	}
	putnext(q, mp);
	return (0);
}

static streamscall int
itot_r_data(queue_t *q, mblk_t *mp)
{
	__swerr();
	return (EFAULT);
}

static streams_fastcall streams_noinline int
itot_r_msg(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_PROTO:
	case M_PCPROTO:
		return itot_r_proto(q, mp);
	case M_DATA:
		return itot_r_data(q, mp);
	case M_FLUSH:
		return itot_r_flush(q, mp);
	}
	putnext(q, mp);
	return (0);
}

static streamscall __hot_get int
itot_rput(queue_t *q, mblk_t *mp)
{
	if (mp->b_datap->db_type == M_PROTO)
		return itot_r_proto(q, mp);
	return itot_r_msg(q, mp);
}

/*
 *  Write-Side Processing
 */

static streams_fastcall streams_inline __hot_put int
itot_w_proto(queue_t *q, mblk_t *mp)
{
	switch (*(np_long *) mp->b_rptr) {
	case N_CONN_REQ:
	case N_CONN_RES:
	case N_DISCON_REQ:
	case N_DATA_REQ:
	case N_EXDATA_REQ:
	case N_INFO_REQ:
	case N_BIND_REQ:
	case N_UNBIND_REQ:
	case N_UNITDATA_REQ:
	case N_OPTMGMT_REQ:

	case N_DATACK_REQ:
	case N_RESET_REQ:
	case N_RESET_RES:

	case N_CONN_IND:
	case N_CONN_CON:
	case N_DISCON_IND:
	case N_DATA_IND:
	case N_EXDATA_IND:
	case N_INFO_ACK:
	case N_BIND_ACK:
	case N_ERROR_ACK:
	case N_OK_ACK:
	case N_UNITDATA_IND:
	case N_UDERROR_IND:

	case N_DATACK_IND:
	case N_RESET_IND:
	case N_RESET_CON:

	default:
		freemsg(mp);
		return (0);
	}
}

static streamscall int
itot_w_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHALL);
		else
			flushq(q, FLUSHALL);
	}
	putnext(q, mp);
	return (0);
}

static streamscall int
itot_w_data(queue_t *q, mblk_t *mp)
{
	__swerr();
	return (EFAULT);
}

static streams_fastcall streams_noinline int
itot_w_msg(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_PROTO:
	case M_PCPROTO:
		return itot_w_proto(q, mp);
	case M_DATA:
		return itot_w_data(q, mp);
	case M_FLUSH:
		return itot_w_flush(q, mp);
	}
	putnext(q, mp);
	return (0);
}

/**
 * itot_wput - write-side put procedure
 * @q: write queue
 * @mp: message to put
 *
 * Messages arriving at the write-side put procedure are messages from above that are NPI formatted
 * messages.  For all intents and purposes, the module above us should believe that we are any X.213
 * based ISO CONS provider.
 */
static streamscall __hot_put int
itot_wput(queue_t *q, mblk_t *mp)
{
	int type = mp->b_datap->db_type;

	/* Fast path. */
	if (type == M_PROTO)
		return itot_w_proto(q, mp);
	return itot_w_msg(q, mp);
}

static streamscall int
itot_open(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	return (ENXIO);
}

static streamscall int
itot_close(queue_t *q, int oflags, cred_t *crp)
{
	return (ENXIO);
}
