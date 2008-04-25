/*****************************************************************************

 @(#) $RCSfile: cdi_conv.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2008-04-25 11:39:33 $

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

 Last Modified $Date: 2008-04-25 11:39:33 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: cdi_conv.c,v $
 Revision 0.9.2.6  2008-04-25 11:39:33  brian
 - updates to AGPLv3

 Revision 0.9.2.5  2007/08/15 05:35:46  brian
 - GPLv3 updates

 Revision 0.9.2.4  2007/08/14 03:31:19  brian
 - GPLv3 header update

 Revision 0.9.2.3  2007/07/14 01:37:24  brian
 - make license explicit, add documentation

 Revision 0.9.2.2  2007/03/25 19:02:50  brian
 - changes to support 2.6.20-1.2307.fc5 kernel

 Revision 0.9.2.1  2007/02/10 22:33:13  brian
 - added new working files

 *****************************************************************************/

#ident "@(#) $RCSfile: cdi_conv.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2008-04-25 11:39:33 $"

static char const ident[] =
    "$RCSfile: cdi_conv.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2008-04-25 11:39:33 $";

/*
 *  CDI-CONV is a simple endian conversion module for use with the RMUX driver.  It converts service
 *  primitives from host to network byte order and visa versa.  It is pushed over CDI Stream to
 *  perform the conversion.  Because the module neither allocates nor frees messages, no service
 *  procedure is required and the conversion is performed completely in the put procedure.
 */

#include <sys/stream.h>
#include <sys/cdi.h>

#define CDI_CONV_DESCRIP	"CDI ENDIAN CONVERSION (CDI-CONV) FOR LINUX FAST-STREAMS"
#define CDI_CONV_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define CDI_CONV_REVISION	"OpenSS7 $RCSfile: cdi_conv.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2008-04-25 11:39:33 $"
#define CDI_CONV_DEVICE		"SVR 4.2 CDI Endian Conversion (CDI-CONV) for STREAMS"
#define CDI_CONV_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define CDI_CONV_LICENSE	"GPL"
#define CDI_CONV_BANNER		CDI_CONV_DESCRIP	"\n" \
				CDI_CONV_COPYRIGHT	"\n" \
				CDI_CONV_REVISION	"\n" \
				CDI_CONV_DEVICE		"\n" \
				CDI_CONV_CONTACT	"\n"
#define CDI_CONV_SPLASH		CDI_CONV_DEVICE		" - " \
				CDI_CONV_REVISION	"\n"

#ifdef MODULE
MODULE_AUTHOR(CDI_CONV_CONTACT);
MODULE_DESCRIPTION(CDI_CONV_DESCRIP);
MODULE_SUPPORTED_DEVICE(CDI_CONV_DEVICE);
MODULE_LICENSE(CDI_CONV_LICENSE);
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-cdi-conv");
#endif				/* MODULE_ALIAS */
#endif				/* MODULE */

#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_CDI_CONV_MODID));
MODULE_ALIAS("streams-module-cdi-conv");
#endif				/* MODULE_ALIAS */

/*
 * The point of this module is only to convert M_PROTO messages to and from network byte order.  All
 * other messages are passed along.  It is the responsibility of the upper layer module to convert
 * other message types.
 */

static int
cdi_m_proto(queue_t *q, mblk_t *mp, uint32_t prim)
{
	union CDI_primitives *p = (typeof(p)) mp->b_rptr;

	switch (prim) {
	case CD_INFO_REQ:
		break;
	case CD_INFO_ACK:
		p->info_ack.cd_state = htonl(p->info_ack.cd_state);
		p->info_ack.cd_max_sdu = htonl(p->info_ack.cd_max_sdu);
		p->info_ack.cd_min_sdu = htonl(p->info_ack.cd_min_sdu);
		p->info_ack.cd_class = htonl(p->info_ack.cd_class);
		p->info_ack.cd_duplex = htonl(p->info_ack.cd_duplex);
		p->info_ack.cd_output_style = htonl(p->info_ack.cd_output_style);
		p->info_ack.cd_features = htonl(p->info_ack.cd_features);
		p->info_ack.cd_addr_length = htonl(p->info_ack.cd_addr_length);
		p->info_ack.cd_ppa_style = htonl(p->info_ack.cd_ppa_style);
		break;
	case CD_ATTACH_REQ:
		p->attach_req.cd_ppa = htonl(p->attach_req.cd_ppa);
		break;
	case CD_DETACH_REQ:
		break;
	case CD_OK_ACK:
		p->ok_ack.cd_state = htonl(p->ok_ack.cd_state);
		p->ok_ack.cd_correct_primitive = htonl(p->ok_ack.cd_correct_primitive);
		break;
	case CD_ERROR_ACK:
		p->error_ack.cd_state = htonl(p->error_ack.cd_state);
		p->error_ack.cd_error_primitive = htonl(p->error_ack.cd_error_primitive);
		p->error_ack.cd_errno = htonl(p->error_ack.cd_errno);
		p->error_ack.cd_explanation = htonl(p->error_ack.cd_explanation);
		break;
	case CD_ENABLE_REQ:
		p->enable_req.cd_dial_type = htonl(p->enable_req.cd_dial_type);
		p->enable_req.cd_dial_length = htonl(p->enable_req.cd_dial_length);
		p->enable_req.cd_dial_offset = htonl(p->enable_req.cd_dial_offset);
		break;
	case CD_ENABLE_CON:
		p->enable_con.cd_state = htonl(p->enable_con.cd_state);
		break;
	case CD_DISABLE_REQ:
		p->disable_req.cd_disposal = htonl(p->disable_req.cd_disposal);
		break;
	case CD_DISABLE_CON:
		p->disable_con.cd_state = htonl(p->disable_con.cd_state);
		break;
	case CD_ERROR_IND:
		p->error_ind.cd_state = htonl(p->error_ind.cd_state);
		p->error_ind.cd_errno = htonl(p->error_ind.cd_errno);
		p->error_ind.cd_explanation = htonl(p->error_ind.cd_explanation);
		break;
	case CD_BAD_FRAME_IND:
		p->bad_frame_ind.cd_state = htonl(p->bad_frame_ind.cd_state);
		p->bad_frame_ind.cd_error = htonl(p->bad_frame_ind.cd_error);
		break;
	case CD_READ_REQ:
		p->read_req.cd_msec = htonl(p->read_req.cd_msec);
		break;
	case CD_ALLOW_INPUT_REQ:
		break;
	case CD_HALT_INPUT_REQ:
		p->halt_input_req.cd_disposal = htonl(p->halt_input_req.cd_disposal);
		break;
	case CD_UNITDATA_IND:
		p->unitdata_ind.cd_state = htonl(p->unitdata_ind.cd_state);
		p->unitdata_ind.cd_src_addr_length = htonl(p->unitdata_ind.cd_src_addr_length);
		p->unitdata_ind.cd_src_addr_offset = htonl(p->unitdata_ind.cd_src_addr_offset);
		p->unitdata_ind.cd_addr_type = htonl(p->unitdata_ind.cd_addr_type);
		p->unitdata_ind.cd_priority = htonl(p->unitdata_ind.cd_priority);
		p->unitdata_ind.cd_dest_addr_length = htonl(p->unitdata_ind.cd_dest_addr_length);
		p->unitdata_ind.cd_dest_addr_offset = htonl(p->unitdata_ind.cd_dest_addr_offset);
		break;
	case CD_UNITDATA_REQ:
		p->unitdata_req.cd_addr_type = htonl(p->unitdata_req.cd_addr_type);
		p->unitdata_req.cd_priority = htonl(p->unitdata_req.cd_priority);
		p->unitdata_req.cd_dest_addr_length = htonl(p->unitdata_req.cd_dest_addr_length);
		p->unitdata_req.cd_dest_addr_offset = htonl(p->unitdata_req.cd_dest_addr_offset);
		break;
	case CD_UNITDATA_ACK:
		p->unitdata_ack.cd_state = htonl(p->unitdata_ack.cd_state);
		break;
	case CD_ABORT_OUTPUT_REQ:
		break;
	case CD_WRITE_READ_REQ:
		p->write_read_req.cd_unitdata_req.cd_addr_type =
		    htonl(p->write_read_req.cd_unitdata_req.cd_addr_type);
		p->write_read_req.cd_unitdata_req.cd_priority =
		    htonl(p->write_read_req.cd_unitdata_req.cd_priority);
		p->write_read_req.cd_unitdata_req.cd_dest_addr_length =
		    htonl(p->write_read_req.cd_unitdata_req.cd_dest_addr_length);
		p->write_read_req.cd_unitdata_req.cd_dest_addr_offset =
		    htonl(p->write_read_req.cd_unitdata_req.cd_dest_addr_offset);
		p->write_read_req.cd_read_req.cd_msec =
		    htonl(p->write_read_req.cd_read_req.cd_msec);
		break;
	case CD_MODEM_SIG_IND:
		p->modem_sig_ind.cd_sigs = htonl(p->modem_sig_ind.cd_sigs);
		break;
	case CD_MODEM_SIG_POLL:
		break;
	case CD_MUX_NAME_REQ:
		break;
	default:
		break;
	}
	putnext(q, mp);
	break;
}

static streamscall int
cdi_rput(queue_t *q, mblk_t *mp)
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
				return cdi_m_proto(q, mp, prim);
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
cdi_wput(queue_t *q, mblk_t *mp)
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
				return cdi_m_proto(q, mp, prim);
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
cdi_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	if (q->q_ptr != NULL)
		return (0);
	if (sflag != MODOPEN || !q->q_next)
		return (EIO);
	q->q_ptr = WR(q)->q_ptr = (queue_t *) 1;	/* just mark it open */
	qprocson(q);
	return (0);
}

modID_t modid = CONFIG_STREAMS_CDI_CONV_MODID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else				/* module_param */
module_param(modid, ushort, 0444);
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for CDI-CONV module.");

static struct module_info cdi_conv_minfo = {
	.mi_idnum = CONFIG_STREAMS_CDI_CONV_MODID,
	.mi_idname = CONFIG_STREAMS_CDI_CONV_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat cdi_conv_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat cdi_conv_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static streamscall int
cdi_qclose(queue_t *q, dev_t *devp, int oflag)
{
	qprocsoff(q);
	q->q_ptr = WR(q)->q_ptr = (queue_t *) 0;	/* just mark it closed */
	return (0);
}

static struct qinit cdi_rinit = {
	.qi_putp = cdi_rput,
	.qi_srvp = NULL,
	.qi_qopen = cdi_qopen,
	.qi_qclose = cdi_qclose,
	.qi_minfo = &cdi_conv_minfo,
	.qi_mstat = &cdi_conv_rstat,
};

static struct qinit cdi_winit = {
	.qi_putp = cdi_wput,
	.qi_srvp = NULL,
	.qi_minfo = &cdi_conv_minfo,
	.qi_mstat = &cdi_conv_wstat,
};

struct streamtab cdi_convinfo = {
	.st_rdinit = &cdi_rinit,
	.st_wrinit = &cdi_winit,
};

static struct fmodsw cdi_fmod = {
	.f_name = CONFIG_STREAMS_CDI_CONV_NAME,
	.f_str = &cdi_convinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

int __init
cdi_convinit(void)
{
	int err;

#ifdef CONFIG_STREAMS_CDI_CONV_MODULE
	cmn_err(CE_NOTE, CDI_CONV_BANNER);
#else				/* CONFIG_STREAMS_CDI_CONV_MODULE */
	cmn_err(CE_NOTE, CDI_CONV_SPLASH);
#endif				/* CONFIG_STREAMS_CDI_CONV_MODULE */
	cdi_conv_minfo.mi_idnum = modid;
	if ((err = register_strmod(&cdi_fmod)) < 0)
		return (err);
	if (modid == 0 && err > 0)
		modid = err;
	return (0);
}

void __exit
cdi_convexit(void)
{
	unregister_strmod(&cdi_fmod);
}

#ifdef CONFIG_STREAMS_CDI_CONV_MODULE
module_init(cdi_convinit);
module_init(cdi_convexit);
#endif				/* CONFIG_STREAMS_CDI_CONV_MODULE */
