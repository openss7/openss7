/*****************************************************************************

 @(#) $RCSfile: tpi_conv.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2007/08/14 04:00:55 $

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

 Last Modified $Date: 2007/08/14 04:00:55 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: tpi_conv.c,v $
 Revision 0.9.2.4  2007/08/14 04:00:55  brian
 - GPLv3 header update

 Revision 0.9.2.3  2007/07/14 01:37:10  brian
 - make license explicit, add documentation

 Revision 0.9.2.2  2007/03/25 19:02:39  brian
 - changes to support 2.6.20-1.2307.fc5 kernel

 Revision 0.9.2.1  2007/02/10 22:33:11  brian
 - added new working files

 *****************************************************************************/

#ident "@(#) $RCSfile: tpi_conv.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2007/08/14 04:00:55 $"

static char const ident[] =
    "$RCSfile: tpi_conv.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2007/08/14 04:00:55 $";

/*
 *  TPI-CONV is a simple endian conversion module for use with the RMUX driver.  It converts service
 *  primitives fron host to network byte order and visa versa.  It is pushed over a TPI Stream to
 *  perform the conversion.  Because the module neither allocates nor frees messages, no service
 *  procedure is required and the conversion is performed completely in the put procedure.
 */

#include <sys/stream.h>
#include <sys/ddi.h>
#include <sys/cmn_err.h>
#include <sys/dki.h>
#include <sys/tpi.h>

#define TPI_CONV_DESCRIP	"TPI ENDIAN CONVERSION (TPI-CONV) FOR LINUX FAST-STREAMS"
#define TPI_CONV_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define TPI_CONV_REVISION	"OpenSS7 $RCSfile: tpi_conv.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2007/08/14 04:00:55 $"
#define TPI_CONV_DEVICE		"SVR 4.2 TPI Endian Conversion (TPI-CONV) for STREAMS"
#define TPI_CONV_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define TPI_CONV_LICENSE	"GPL v2"
#define TPI_CONV_BANNER		TPI_CONV_DESCRIP	"\n" \
				TPI_CONV_COPYRIGHT	"\n" \
				TPI_CONV_REVISION	"\n" \
				TPI_CONV_DEVICE		"\n" \
				TPI_CONV_CONTACT	"\n"
#define TPI_CONV_SPLASH		TPI_CONV_DEVICE		" - " \
				TPI_CONV_REVISION	"\n"

#ifdef MODULE
MODULE_AUTHOR(TPI_CONV_CONTACT);
MODULE_DESCRIPTION(TPI_CONV_DESCRIP);
MODULE_SUPPORTED_DEVICE(TPI_CONV_DEVICE);
MODULE_LICENSE(TPI_CONV_LICENSE);
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-tpi-conv");
#endif				/* MODULE_ALIAS */
#endif				/* MODULE */

#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_TPI_CONV_MODID));
MODULE_ALIAS("streams-module-tpi-conv");
#endif				/* MODULE_ALIAS */

/*
 * The point of this module is only to convert M_PROTO messages to and from network byte order.  All
 * other messages are passed along.  It is the responsibility of the upper layer module to convert
 * other message types.
 */

static int
tpi_m_proto(queue_t *q, mblk_t *mp, uint32_t prim)
{
	union TPI_primitives *p = (typeof(p)) mp->b_rptr;

	/* Note that addresses must be specified in network byte order for this to work. */
	switch (prim) {
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
	default:
		break;
	}
	putnext(q, mp);
	break;
}

static streamscall int
tpi_rput(queue_t *q, mblk_t *mp)
{
	if (datamsg(DB_TYPE(mp))) {
		switch (DB_TYPE(mp)) {
		case M_DATA:
			break;
		case M_PROTO:
		case M_PCPROTO:
			if (mp->b_wptr >= mp->b_rptr + sizeof(uint32_t)) {
				uint32_t prim;

				prim = *(uint32_t *) mp->b_rptr;
				*(uint32_t *) mp->b_rptr = ntohl(prim);
				return tpi_m_proto(q, mp, prim);
			}
			break;
		default:
		case M_DELAY:
			break;
		}
	}
	putnext(q, mp);
	return (0);
}

static streamscall int
tpi_wput(queue_t *q, mblk_t *mp)
{
	if (datamsg(DB_TYPE(mp))) {
		switch (DB_TYPE(mp)) {
		case M_DATA:
			break;
		case M_PROTO:
		case M_PCPROTO:
			if (mp->b_wptr >= mp->b_rptr + sizeof(uint32_t)) {
				uint32_t prim;

				prim = htonl(*(uint32_t *) mp->b_rptr);
				*(uint32_t *) mp->b_rptr = prim;
				return tpi_m_proto(q, mp, prim);
			}
			break;
		default:
		case M_DELAY:
			break;
		}
	}
	putnext(q, mp);
	return (0);
}

static streamscall int
tpi_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	if (q->q_ptr != NULL)
		return (0);
	if (sflag != MODOPEN || !q->q_next)
		return (EIO);
	q->q_ptr = WR(q)->q_ptr = (queue_t *) 1;	/* just mark it open */
	qprocson(q);
	return (0);
}

modID_t modid = CONFIG_STREAMS_TPI_CONV_MODID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else				/* module_param */
module_param(modid, ushort, 0444);
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for TPI-CONV module.");

static struct module_info tpi_conv_minfo = {
	.mi_idnum = CONFIG_STREAMS_TPI_CONV_MODID,
	.mi_idname = CONFIG_STREAMS_TPI_CONV_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat tpi_conv_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat tpi_conv_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static streamscall int
tpi_qclose(queue_t *q, dev_t *devp, int oflag)
{
	qprocsoff(q);
	q->q_ptr = WR(q)->q_ptr = (queue_t *) 0;	/* just mark it closed */
	return (0);
}

static struct qinit tpi_rinit = {
	.qi_putp = tpi_rput,
	.qi_srvp = NULL,
	.qi_qopen = tpi_qopen,
	.qi_qclose = tpi_qclose,
	.qi_minfo = &tpi_conv_minfo,
	.qi_mstat = &tpi_conv_rstat,
};

static struct qinit tpi_winit = {
	.qi_putp = tpi_wput,
	.qi_srvp = NULL,
	.qi_minfo = &tpi_conv_minfo,
	.qi_mstat = &tpi_conv_wstat,
};

struct streamtab tpi_convinfo = {
	.st_rdinit = &tpi_rinit,
	.st_wrinit = &tpi_winit,
};

static struct fmodsw tpi_fmod = {
	.f_name = CONFIG_STREAMS_TPI_CONV_NAME,
	.f_str = &tpi_convinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

int __init
tpi_convinit(void)
{
	int err;

#ifdef CONFIG_STREAMS_TPI_CONV_MODULE
	cmn_err(CE_NOTE, TPI_CONV_BANNER);
#else				/* CONFIG_STREAMS_TPI_CONV_MODULE */
	cmn_err(CE_NOTE, TPI_CONV_SPLASH);
#endif				/* CONFIG_STREAMS_TPI_CONV_MODULE */
	tpi_conv_minfo.mi_idnum = modid;
	if ((err = register_strmod(&tpi_fmod)) < 0)
		return (err);
	if (modid == 0 && err > 0)
		modid = err;
	return (0);
}

void __exit
tpi_convexit(void)
{
	unregister_strmod(&tpi_fmod);
}

#ifdef CONFIG_STREAMS_TPI_CONV_MODULE
module_init(tpi_convinit);
module_init(tpi_convexit);
#endif				/* CONFIG_STREAMS_TPI_CONV_MODULE */
