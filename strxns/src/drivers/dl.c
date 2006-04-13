/*****************************************************************************

 @(#) $RCSfile: dl.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2006/04/12 20:36:03 $

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

 Last Modified $Date: 2006/04/12 20:36:03 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: dl.c,v $
 Revision 0.9.2.3  2006/04/12 20:36:03  brian
 - added some experimental drivers

 Revision 0.9.2.2  2006/04/11 22:08:51  brian
 - working up DL driver

 Revision 0.9.2.1  2006/04/11 18:26:49  brian
 - added new DL driver

 *****************************************************************************/

#ident "@(#) $RCSfile: dl.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2006/04/12 20:36:03 $"

static char const ident[] =
    "$RCSfile: dl.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2006/04/12 20:36:03 $";

#include <sys/os7/compat.h>

#ifdef LINUX
#undef ASSERT

#include <linux/bitops.h>

#define d_tst_bit(nr,addr)	test_bit(nr,addr)
#define d_set_bit(nr,addr)	__set_bit(nr,addr)
#define d_clr_bit(nr,addr)	__clear_bit(nr,addr)

#include <linux/interrupt.h>

#include <sys/dlpi.h>

#define DL_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define DL_EXTRA	"Part of the OpenSS7 stack for Linux Fast-STREAMS"
#define DL_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define DL_REVISION	"OpenSS7 $RCSfile: dl.c,v $ $Name:  $ ($Revision: 0.9.2.3 $) $Date: 2006/04/12 20:36:03 $"
#define DL_DEVICE	"SVR 4.2 STREAMS DLPI OSI Data Link Provider"
#define DL_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define DL_LICENSE	"GPL"
#define DL_BANNER	DL_DESCRIP	"\n" \
			DL_EXTRA	"\n" \
			DL_COPYRIGHT	"\n" \
			DL_DEVICE	"\n" \
			DL_CONTACT
#define DL_SPLASH	DL_DESCRIP	"\n" \
			DL_REVISION

#ifdef LINUX
MODULE_AUTHOR(DL_CONTACT);
MODULE_DESCRIPTION(DL_DESCRIP);
MODULE_SUPPORTED_DEVICE(DL_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(DL_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-dl");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifdef LFS
#define DL_DRV_ID	CONFIG_STREAMS_DL_MODID
#define DL_DRV_NAME	CONFIG_STREAMS_DL_NAME
#define DL_CMAJORS	CONFIG_STREAMS_DL_NMAJOR
#define DL_CMAJOR_0	CONFIG_STREAMS_DL_MAJOR
#define DL_UNITS	CONFIG_STREAMS_DL_NMINORS
#endif				/* LFS */

#ifdef LINUX
#ifdef MODULE_ALIAS
#ifdef LFS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_DL_MODID));
MODULE_ALIAS("streams-driver-dl");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_DL_MAJOR));
MODULE_ALIAS("/dev/streams/dl");
MODULE_ALIAS("/dev/streams/dl/*");
MODULE_ALIAS("/dev/streams/clone/dl");
#endif				/* LFS */
MODULE_ALIAS("char-major-" __stringify(DL_CMAJOR_0));
MODULE_ALIAS("char-major-" __stringify(DL_CMAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(DL_CMAJOR_0) "-0");
MODULE_ALIAS("char-major-" __stringify(DL_CMAJOR_0) "-" __stringify(DL_CMINOR));
MODULE_ALIAS("/dev/dl");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

/*
 *  STREAMS Definitions
 *  ===================
 */

#define DRV_ID		DL_DRV_ID
#define CMAJORS		DL_CMAJORS
#define CMAJOR_0	DL_CMAJOR_0
#define UNITS		DL_UNITS
#ifdef MODULE
#define DRV_BANNER	DL_BANNER
#else				/* MODULE */
#define DRV_BANNER	DL_SPLASH
#endif				/* MODULE */

STATIC struct module_info dl_minfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME,		/* Module name */
	.mi_minpsz = 0,			/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,		/* Max packet size acceptd */
	.mi_hiwat = 1 << 15,		/* Hi water mark */
	.mi_hiwat = 1 << 10,		/* Lo water mark */
};

STATIC struct module_stat dl_mstat = {
};

/* Upper multiplex is a DL provider following the DLPI. */

STATIC streamscall int dl_qopen(queue_t *, dev_t *, int, int, cred_t *);
STATIC streamscall int dl_qclose(queue_t *, int, cred_t *);

STATIC struct qinit dl_rinit = {
	.qi_putp = &ss7_oput,		/* Read put procedure (message from below) */
	.qi_srvp = &ss7_osrv,		/* Read service procedure */
	.qi_qopen = &dl_qopen,		/* Each open */
	.qi_qclose = &dl_qclose,	/* Last close */
	.qi_minfo = &dl_minfo,		/* Module information */
	.qi_mstat = &dl_mstat,		/* Module statistics */
};

STATIC struct qinit dl_winit = {
	.qi_putp = &ss7_iput,		/* Read put procedure (message from below) */
	.qi_srvp = &ss7_isrv,		/* Read service procedure */
	.qi_minfo = &dl_minfo,		/* Module information */
	.qi_mstat = &dl_mstat,		/* Module statistics */
};

/* Lower multiplex is a CD user following the CDI. */

STATIC struct qinit cd_rinit = {
	.qi_putp = &ss7_iput,		/* Read put procedure (message from below) */
	.qi_srvp = &ss7_isrv,		/* Read service procedure */
	.qi_minfo = &cd_minfo,		/* Module information */
	.qi_mstat = &cd_mstat,		/* Module statistics */
};

STATIC struct qinit cd_winit = {
	.qi_putp = &ss7_oput,		/* Read put procedure (message from below) */
	.qi_srvp = &ss7_osrv,		/* Read service procedure */
	.qi_minfo = &cd_minfo,		/* Module information */
	.qi_mstat = &cd_mstat,		/* Module statistics */
};

STATIC struct streamtab dl_info = {
	.st_rdinit = &dl_rinit,		/* Upper read queue */
	.st_wrinit = &dl_winit,		/* Upper write queue */
	.st_muxrinit = &cd_rinit,	/* Lower read queue */
	.st_muxwinit = &cd_winit,	/* Lower write queue */
};

/*
 *  Service Primitives passed downwards -- lower multiplex or module
 *  ----------------------------------------------------------------
 */

STATIC int
cd_info_req(queue_t *q, queue_t *oq)
{
	struct cd *cd = CD_PRIV(oq);
	cd_info_req_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (cd_info_req_t *) mp->b_rptr;
		p->cd_primitive = CD_INFO_REQ;
		mp->b_wptr += sizeof(*p);
		cdi_set_state(cd, cdi_get_state(cd));
		putnext(oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

STATIC int
cd_attach_req(queue_t *q, queue_t *oq, dl_ulong cd_ppa)
{
	cd_attach_req_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (cd_attach_req_t *) mp->b_rptr;
		p->cd_primitive = CD_ATTACH_REQ;
		p->cd_ppa = cd_ppa;
		mp->b_wptr += sizeof(*p);
		putnext(oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
STATIC int
cd_detach_req(queue_t *q, queue_t *oq)
{
	cd_detach_req_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap_db_type = M_PROTO;
		p = (cd_detach_req_t *) mp->b_rptr;
		p->cd_primitive = CD_DETACH_REQ;
		mp->b_wptr += sizeof(*p);
		putnext(oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
STATIC int
cd_enable_req(queue_t *q, queue_t *oq, cd_ulong cd_dial_type, struct netbuf *dial)
{
	cd_enable_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p) + dial->len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (cd_enable_req_t *) mp->b_wptr;
		p->cd_primitive = CD_ENABLE_REQ;
		p->cd_dial_type = cd_dial_type;
		p->cd_dial_length = dial->len;
		p->cd_dial_offset = dial->len ? sizeof(*p) : 0;
		mp->b_wptr += sizeof(*p);
		if (dial->len) {
			bcopy(dial->buf, mp->b_wptr, dial->len);
			mp->b_wptr += dial->len;
		}
		cdi_set_state(CD_PRIM(oq), CD_ENABLE_PENDING);
		putnext(oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
STATIC int
cd_disable_req(queue_t *q, queue_t *oq, cd_ulong cd_disposal)
{
	cd_disable_req_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (cd_disable_req_t *) mp->b_wptr;
		p->cd_primitive = CD_DISABLE_REQ;
		p->cd_disposal = cd_disposal;
		mp->b_wptr += sizeof(*p);
		cdi_set_state(CD_PRIM(oq), CD_DISABLE_PENDING);
		putnext(oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
STATIC int
cd_allow_input_req(queue_t *q, queue_t *oq)
{
	cd_allow_input_req_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (cd_allow_input_req_t *) mp->b_wptr;
		p->cd_primitive = CD_ALLOW_INPUT_REQ;
		mp->b_wptr += sizeof(*p);
		putnext(oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
STATIC int
cd_read_req(queue_t *q, queue_t *oq, cd_ulong cd_msec)
{
	cd_read_req_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;	/* could also be M_PCPROTO */
		p = (cd_read_req_t *) mp->b_wptr;
		p->cd_primitive = CD_READ_REQ;
		p->cd_msec = cd_msec;
		mp->b_wptr += sizeof(*p);
		putnext(oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
STATIC int
cd_unitdata_req(queue_t *q, queue_t *oq, cd_ushort cd_addr_type, cd_ushort cd_priority,
		struct netbuf *dest, mblk_t *data)
{
	cd_unitdata_req_t *p;
	mblk_t *mp;

	if (bcanputnext(q, cd_priority)) {
		if ((mp = ss7_allocb(q, sizeof(*p) + dest->len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (cd_unitdata_req_t *) mp->b_wptr;
			p->cd_primitive = CD_UNITDATA_REQ;
			p->cd_addr_type = cd_addr_type;
			p->cd_priority = cd_priority;
			p->cd_dest_addr_length = 0;
			p->cd_dest_addr_offset = 0;
			mp->b_wptr += sizeof(*p);
			if (cd_addr_type == CD_SPECIFIC) {
				p->cd_dest_addr_length = dest->len;
				p->cd_dest_addr_offset = dest->len ? sizeof(*p) : 0;
				if (dest->len) {
					bcopy(dest->buf, mp->b_wptr, dest->len);
					mp->b_wptr += sizeof(dest->len);
				}
			}
			mp->b_cont = data;
			putnext(oq, mp);
			return (QR_ABSORBED);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}
STATIC int
cd_write_read_req(queue_t *q, queue_t *oq, cd_ushort cd_addr_type, cd_ushort cd_priority,
		  struct netbuf *dest, mblk_t *data, cd_ulong cd_msec)
{
	cd_write_read_req_t *p;
	mblk_t *mp;

	if (bcanputnext(q, cd_priority)) {
		if ((mp = ss7_allocb(q, sizeof(*p) = dest->len, BRPI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (cd_write_read_req_t *) mp->b_wptr;
			p->cd_primitive = CD_WRITE_READ_REQ;
			p->cd_unitdata_req.cd_primitive = CD_UNITDATA_REQ;
			p->cd_unitdata_req.cd_addr_type = cd_addr_type;
			p->cd_unitdata_req.cd_priority = cd_priority;
			p->cd_unitdata_req.cd_addr_length = 0;
			p->cd_unitdata_req.cd_addr_offset = 0;
			p->cd_read_req.cd_primitive = CD_READ_REQ;
			p->cd_read_req.cd_msec = cd_msec;
			mp->b_wptr += sizeof(*p);
			if (cd_addr_type == CD_SPECIFIC) {
				p->cd_unitdata_req.cd_addr_length = dest->len;
				p->cd_unitdata_req.cd_addr_offset = dest->len ? sizeof(*p) : 0;
				if (dest->len) {
					bcopy(dest->buf, mp->b_wptr, dest->len);
					mp->b_wptr += dest->len;
				}
			}
			mp->b_cont = data;
			putnext(oq, mp);
			return (QR_ABSORBED);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}
STATIC int
cd_halt_input_req(queue_t *q, queue_t *oq, cd_ulong cd_disposal)
{
	cd_halt_input_req_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (cd_halt_input_req_t *) mp->b_wptr;
		p->cd_primitive = CD_HALT_INPUT_REQ;
		p->cd_disposal = cd_disposal;
		mp->b_wptr += sizeof(*p);
		putnext(oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}
STATIC int
cd_abort_output_req(queue_t *q, queue_t *oq)
{
	cd_abort_output_req_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (cd_abort_output_req_t *) mp->b_wptr;
		p->cd_primitive = CD_HALT_INPUT_REQ;
		p->cd_disposal = cd_disposal;
		mp->b_wptr += sizeof(*p);
		putnext(oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}
STATIC int
cd_mux_name_req(queue_t *q, queue_t *oq)
{
	cd_ulong *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (cd_ulong *) mp->b_wptr;
		*p = CD_MUX_NAME_REQ;
		mp->b_wptr += sizeof(*p);
		putnext(oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}
STATIC int
cd_modem_sig_req(queue_t *q, queue_t *oq, cd_ulong cd_sigs)
{
	cd_modem_sig_req_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (cd_modem_sig_req_t *) mp->b_wptr;
		p->cd_primitive = CD_MODEM_SIG_REQ;
		p->cd_sigs = cd_sigs;
		mp->b_wptr += sizeof(*p);
		putnext(oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}
STATIC int
cd_modem_sig_poll(queue_t *q)
{
	cd_modem_sig_poll_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (cd_modem_sig_poll_t *) mp->b_wptr;
		p->cd_primitive = CD_MODEM_SIG_POLL;
		mp->b_wptr += sizeof(*p);
		putnext(oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  Service Primitives passed upwards -- upper multiplex or module
 *  --------------------------------------------------------------
 */
STATIC mblk_t *
dl_info_ack(queue_t *q)
{
	struct dl *dl = DL_PRIV(q);
	dl_info_ack_t *p;
	mblk_t *mp;
	size_t len = dl->add.len + dl->qos.len + dl->qor.len + dl->brd.len;

	if ((mp = ss7_allocb(q, sizeof(*p) + len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (dl_info_ack_t *) mp->b_rptr;
		p->dl_primitive = DL_INFO_ACK;
		p->dl_max_sdu = dl->info.dl_max_sdu;
		p->dl_min_sdu = dl->info.dl_min_sdu;
		p->dl_addr_length = dl->add.len;
		p->dl_mac_type = dl->info.dl_mac_type;
		p->dl_reserved = 0;
		p->dl_current_state = dlpi_get_state(dl);
		p->dl_sap_length = dl->info.dl_sap_length;
		p->dl_serivce_mode = dl->info.dl_service_mode;
		p->dl_qos_length = dl->qos.len;
		p->dl_qos_offset = (dl->qos.len ? sizeof(*p) : 0);
		p->dl_qos_range_length = dl->qor.len;
		p->dl_qos_range_offset = (dl->qor.len ? sizeof(*p) + dl->qos.len : 0);
		p->dl_provider_style = dl->info.dl_provider_style;
		p->dl_addr_offset = (dl->add.len ? sizeof(*p) + dl->qos.len + dl->qor.len : 0);
		p->dl_version = dl->info.dl_version;
		p->dl_brdcst_addr_length = dl->brd.len;
		p->dl_brdcst_addr_offset =
		    (dl->brd.len ? sizeof(*p) + dl->qos.len + dl->qor.len + dl->add.len : 0);
		p->dl_growth = 0;
		mp->b_wptr += sizeof(*p);
		if (dl->qos.len) {
			bcopy(dl->qos.buf, mp->b_wptr, dl->qos.len);
			mp->b_wptr += dl->qos.len;
		}
		if (dl->qor.len) {
			bcopy(dl->qor.buf, mp->b_wptr, dl->qor.len);
			mp->b_wptr += dl->qor.len;
		}
		if (dl->add.len) {
			bcopy(dl->add.buf, mp->b_wptr, dl->add.len);
			mp->b_wptr += dl->add.len;
		}
		if (dl->brd.len) {
			bcopy(dl->brd.buf, mp->b_wptr, dl->brd.len);
			mp->b_wptr += dl->brd.len;
		}
	}
	return (mp);
}
STATIC mblk_t *
dl_bind_ack(queue_t *q, struct netbuf *add, dl_ulong dl_sap, dl_ulong dl_max_conind,
	    dl_ulong dl_xidtest_flg)
{
	dl_bind_ack_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p) + add->len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (dl_bind_ack_t *) mp->b_wptr;
		p->dl_primitive = DL_BIND_ACK;
		p->dl_sap = dl_sap;
		p->dl_addr_length = add->len;
		p->dl_addr_offset = (add->len ? sizeof(*p) : 0);
		p->dl_max_conind = dl_max_conind;
		p->dl_xidtest_flg = dl_xidtest_flg;
		mp->b_wptr += sizeof(*p);
		if (add->len) {
			bcopy(add->buf, mp->b_wptr, add->len);
			mp->b_wptr += add->len;
		}
	}
	return (mp);
}

/**
 * dl_ok_ack - send a DL_OK_ACK and commit a pending action
 * @q: DL write queue
 * @prim: corret primitive
 * @aq: accepting queue if DL_CONNECT_RES
 * @cp: connection indication pointer if DL_CONNECT_RES or DL_DISCONNECT_REQ
 */
STATIC int
dl_ok_ack(queue_t *q, dl_long prim, queue_t *aq, mblk_t *cp)
{
	struct dl *dl = DL_PRIV(q);
	dl_ok_ack_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (dl_ok_ack_t *) mp->b_wptr;
		p->dl_primitive = DL_OK_ACK;
		p->dl_correct_primitive = prim;
		mp->b_wptr += sizeof(*p);
		switch (dlpi_get_state(dl)) {
		case DL_ATTACH_PENDING:	/* Waiting ack of DL_ATTACH_REQ */
			dlpi_set_state(dl, DL_UNBOUND);
			break;
		case DL_DETACH_PENDING:	/* Waiting ack of DL_DETACH_REQ */
			dlpi_set_state(dl, DL_UNATTACHED);
			break;
		case DL_BIND_PENDING:	/* Waiting ack of DL_BIND_REQ */
			swerr();	/* should be DL_BIND_ACK */
			break;
		case DL_UNBIND_PENDING:	/* Waiting ack of DL_UNBIND_REQ */
			dlpi_set_state(dl, DL_UNBOUND);
			break;
		case DL_UDQOS_PENDING:	/* Waiting ack of DL_UDQOS_REQ */
			dlpi_set_state(dl, DL_IDLE);
			break;
		case DL_OUTCON_PENDING:	/* awaiting DL_CONN_CON */
		case DL_INCON_PENDING:	/* awaiting DL_CONN_RES */
			swerr();
			break;
		case DL_CONN_RES_PENDING:	/* Waiting ack of DL_CONNECT_RES */
			if (--dl->dl_outstanding_coninds > 0)
				dlpi_set_state(dl, DL_INCON_PENDING);
			else
				dlpi_set_state(dl, DL_IDLE);
			break;
		case DL_USER_RESET_PENDING:	/* awaiting DL_RESET_CON */
		case DL_PROV_RESET_PENDING:	/* awaiting DL_RESET_RES */
			swerr();
			break;
		case DL_RESET_RES_PENDING:	/* Waiting ack of DL_RESET_RES */
			dlpi_set_state(dl, DL_DATAXFER);
			break;
		case DL_DISCON8_PENDING:	/* Waiting ack of DL_DISC_REQ from
						   DL_OUTCON_PENDING */
			dlpi_set_state(dl, DL_IDLE);
			break;
		case DL_DISCON9_PENDING:	/* Waiting ack of DL_DISC_REQ from DL_INCON_PENDING 
						 */
			if (--dl->dl_outstanding_coninds > 0)
				dlpi_set_state(dl, DL_INCON_PENDING);
			else
				dlpi_set_state(dl, DL_IDLE);
			break;
		case DL_DISCON11_PENDING:	/* Waiting ack of DL_DISC_REQ from DL_ DATAXFER */
			dlpi_set_state(dl, DL_IDLE);
			break;
		case DL_DISCON12_PENDING:	/* Waiting ack of DL_DISC_REQ from
						   DL_USER_RESET_PENDING */
			dlpi_set_state(dl, DL_IDLE);
			break;
		case DL_DISCON13_PENDING:	/* Waiting ack of DL_DISC_REQ from
						   DL_PROV_RESET_PENDING */
			dlpi_set_state(dl, DL_IDLE);
			break;
		case DL_SUBS_BIND_PND:	/* Waiting ack of DL_SUBS_BIND_REQ */
			swerr();	/* should be DL_SUBS_BIND_ACK */
			break;
		case DL_SUBS_UNBIND_PND:	/* Waiting ack of DL_SUBS_UNBIND_REQ */
			dlpi_set_state(dl, DL_IDLE);
			break;

		case DL_UNATTACHED:	/* PPA not attached */
		case DL_UNBOUND:	/* PPA attached */
		case DL_IDLE:	/* dlsap bound, awaiting use */
		case DL_DATAXFER:	/* connection-oriented data transfer */
			/* If not in a pending state, then this is probably an acknowledged
			   primitive that does not have a pending state (can be issued from any
			   state), such as DL_PROMISCON_REQ.  Just stay in the same state and
			   acknowledge the primitive. */
			break;
		}
		qreply(q, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
	return (mp);
}

/**
 * dl_error_ack - generate a DL_ERROR_ACK upstream
 * @q: DL write queue
 * @prim: primitive in error
 * @error: zero (0) no error, positive (>0) dlpi error, negative(<0) linux error
 *
 * Generates a DL_ERROR_ACK message upstream with state recovery and error recovery.  The return
 * value from this function may be used as a return value from the procedure.
 */
STATIC int
dl_error_ack(queue_t *q, dl_long prim, dl_long error)
{
	struct dl *dl = DL_PRIV(q);
	dl_error_ack_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (dl_error_ack_t *) mp->b_wptr;
		p->dl_primitive = DL_ERROR_ACK;
		p->dl_error_primitive = prim;
		p->dl_errno = (error < 0 ? DL_SYSERR : error);
		p->dl_unix_errno = (error < 0 ? -error : 0);
		mp->b_wptr += sizeof(*p);
		switch (dlpi_get_state(dl)) {
		case DL_UNATTACHED:
			/* PPA not attached */
		case DL_ATTACH_PENDING:
			/* Waiting ack of DL_ATTACH_REQ */

			dlpi_set_state(dl, DL_UNATTACHED);
			break;

		case DL_DETACH_PENDING:
			/* Waiting ack of DL_DETACH_REQ */
		case DL_UNBOUND:
			/* PPA attached */
		case DL_BIND_PENDING:
			/* Waiting ack of DL_BIND_REQ */

			dlpi_set_state(dl, DL_UNBOUND);
			break;

		case DL_UNBIND_PENDING:
			/* Waiting ack of DL_UNBIND_REQ */
		case DL_SUBS_BIND_PND:
			/* Waiting ack of DL_SUBS_BIND_REQ */
		case DL_SUBS_UNBIND_PND:
			/* Waiting ack of DL_SUBS_UNBIND_REQ */
		case DL_IDLE:
			/* dlsap bound, awaiting use */
		case DL_UDQOS_PENDING:
			/* Waiting ack of DL_UDQOS_REQ */
		case DL_OUTCON_PENDING:
			/* awaiting DL_CONN_CON */

			dlpi_set_state(dl, DL_IDLE);
			break;

		case DL_INCON_PENDING:
			/* awaiting DL_CONN_RES */
		case DL_CONN_RES_PENDING:
			/* Waiting ack of DL_CONNECT_RES */
		case DL_DISCON9_PENDING:
			/* Waiting ack of DL_DISC_REQ from DL_INCON_PENDING */

			dlpi_set_state(dl, DL_INCON_PENDING);
			break;

		case DL_DISCON8_PENDING:
			/* Waiting ack of DL_DISC_REQ from DL_OUTCON_PENDING */

			dlpi_set_state(dl, DL_OUTCON_PENDING);
			break;

		case DL_DATAXFER:
			/* connection-oriented data transfer */
		case DL_USER_RESET_PENDING:
			/* awaiting DL_RESET_CON */
		case DL_DISCON11_PENDING:
			/* Waiting ack of DL_DISC_REQ from DL_DATAXFER */

			dlpi_set_state(dl, DL_DATAXFER);
			break;

		case DL_DISCON12_PENDING:
			/* Waiting ack of DL_DISC_REQ from DL_USER_RESET_PENDING */

			dlpi_set_state(dl, DL_USER_RESET_PENDING);
			break;

		case DL_PROV_RESET_PENDING:
			/* awaiting DL_RESET_RES */
		case DL_RESET_RES_PENDING:
			/* Waiting ack of DL_RESET_RES */
		case DL_DISCON13_PENDING:
			/* Waiting ack of DL_DISC_REQ from DL_PROV_RESET_PENDING */

			dlpi_set_state(dl, DL_PROV_RESET_PENDING);
			break;
		}
		qreply(q, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}
STATIC mblk_t *
dl_subs_bind_ack(queue_t *q)
{
	struct dl *dl = DL_PRIV(q);
	dl_subs_bind_ack_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p) + dl->sub.len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (dl_subs_bind_ack_t *) mp->b_wptr;
		p->dl_primitive = DL_SUBS_BIND_ACK;
		p->dl_subs_sap_offset = (dl->sub.len ? sizeof(*p) : 0);
		p->dl_subs_sap_length = dl->sub.len;
		mp->b_wptr += sizeof(*p);
		if (dl->sub.len) {
			bcopy(dl->sub.buf, mp->b_wptr, dl->sub.len);
			mp->b_wptr += dl->sub.len;
		}
	}
	return (mp);
}
STATIC INLINE fastcall __hot_get mblk_t *
dl_unitdata_ind(queue_t *q, struct netbuf *dst, struct netbuf *src, int group)
{
	dl_unitdata_ind_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p) + dst->len + src->len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (dl_unitdata_ind_t *) mp->b_wptr;
		p->dl_primitive = DL_UNITDATA_IND;
		p->dl_dest_addr_length = dst->len;
		p->dl_dest_addr_offset = dst->len ? sizeof(*p) : 0;
		p->dl_src_addr_length = src->len;
		p->dl_src_addr_offset = src->len ? sizeof(*p) + dst->len : 0;
		p->dl_group_address = group ? 1 : 0;
		mp->b_wptr += sizeof(*p);
		if (dst->len) {
			bcopy(dst->buf, mp->b_wptr, dst->len);
			mp->b_wptr += dst->len;
		}
		if (src->len) {
			bcopy(src->buf, mp->b_wptr, src->len);
			mp->b_wptr += src->len;
		}
		/* Note: caller must attach their own data */
	}
	return (mp);
}
STATIC INLINE fastcall __hot_get mblk_t *
dl_uderror_ind(queue_t *q, struct netbuf *dst, dl_long error)
{
	dl_uderror_ind_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p) + dst->len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (dl_uderror_ind_t *) mp->b_rptr;
		p->dl_primitive = DL_UDERROR_IND;
		p->dl_dest_addr_length = dst->len;
		p->dl_dest_addr_offset = dst->len ? sizeof(*p) : 0;
		p->dl_unix_errno = error < 0 ? -error : 0;
		p->dl_errno = error < 0 ? DL_SYSERR : error;
		mp->b_wptr += sizeof(*p);
		if (dst->len) {
			bcopy(dst->buf, mp->b_wptr, dst->len);
			mp->b_wptr += dst->len;
		}
		/* Note: caller must attach their own data */
	}
	return (mp);
}
STATIC mblk_t *
dl_connect_ind(queue_t *q, struct netbuf *cld, struct netbuf *clg, struct netbuf *qos,
	       dl_ulong token)
{
	dl_connect_ind_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p) + cld->len + clg->len + qos->len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (dl_connect_ind_t *) mp->b_wptr;
		p->dl_primitive = DL_CONNECT_IND;
		p->dl_correlation = token;
		p->dl_called_addr_length = cld->len;
		p->dl_called_addr_offset = cld->len ? sizeof(*p) : 0;
		p->dl_calling_addr_length = clg->len;
		p->dl_calling_addr_offset = clg->len ? sizeof(*p) + cld->len : 0;
		p->dl_qos_length = qos->len;
		p->dl_qos_offset = qos->len ? sizeof(*p) + cld->len + clg->len : 0;
		p->dl_growth = 0;
		mp->b_wptr += sizeof(*p);
		if (cld->len) {
			bcopy(cld->buf, mp->b_wptr, cld->len);
			mp->b_wptr += cld->len;
		}
		if (clg->len) {
			bcopy(clg->buf, mp->b_wptr, clg->len);
			mp->b_wptr += clg->len;
		}
		if (qos->len) {
			bcopy(qos->buf, mp->b_wptr, qos->len);
			mp->b_wptr += qos->len;
		}
		/* Note: caller must attach their own data */
	}
	return (mp);
}
STATIC mblk_t *
dl_connect_con(queue_t *q, struct netbuf *res, struct netbuf *qos)
{
	dl_connect_con_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p) + res->len + qos->len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (dl_connect_con_t *) mp->b_wptr;
		p->dl_primitive = DL_CONNECT_CON;
		p->dl_resp_addr_length = res->len;
		p->dl_resp_addr_offset = resl->len ? sizeof(*p) : 0;
		p->dl_qos_length = qos->len;
		p->dl_qos_offset = qos->len ? sizeof(*p) + res->len : 0;
		p->dl_growth = 0;
		mp->b_wptr += sizeof(*p);
		if (res->len) {
			bcopy(res->buf, mp->b_wptr, res->len);
			mp->b_wptr += res->len;
		}
		if (qos->len) {
			bcopy(qos->buf, mp->b_wptr, qos->len);
			mp->b_wptr += qos->len;
		}
		/* Note: caller must attach their own data */
	}
	return (mp);
}
STATIC mblk_t *
dl_token_ack(queue_t *q, dl_ulong token)
{
	dl_token_ack_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (dl_token_ack_t *) mp->b_wptr;
		p->dl_primitive = DL_TOKEN_ACK;
		p->dl_token = token;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}
STATIC mblk_t *
dl_disconnect_ind(queue_t *q, dl_ulong dl_originator, dl_ulong dl_reason, dl_ulong dl_correlation)
{
	dl_disconnect_ind_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (dl_disconect_ind_t *) mp->b_wptr;
		p->dl_primitive = DL_DISCONNECT_IND;
		p->dl_originator = dl_originator;
		p->dl_reason = dl_reason;
		p->dl_correlation = dl_correlation;
		mp->b_wptr += sizeof(*p);
		/* Note: caller must attach their own data */
	}
	return (mp);
}
STATIC INLINE fastcall __hot_get mblk_t *
dl_reset_ind(queue_t *q, dl_ulong dl_originator, dl_ulong dl_reason)
{
	dl_reset_ind_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (dl_reset_ind_t *) mp->b_wptr;
		p->dl_primitive = DL_RESET_IND;
		p->dl_originator = dl_originator;
		p->dl_reason = dl_reason;
		mp->b_wptr += sizeof(*p);
		/* Note: caller must attach their own data */
	}
	return (mp);
}
STATIC INLINE fastcall __hot_get mblk_t *
dl_reset_con(queue_t *q)
{
	dl_reset_con_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (dl_reset_con_t *) mp->b_wptr;
		p->dl_primitive = DL_RESET_CON;
		mp->b_wptr += sizeof(*p);
		/* Note: caller must attach their own data */
	}
	return (mp);
}
STATIC INLINE fastcall __hot_get mblk_t *
dl_data_ack_ind(queue_t *q, struct netbuf *dst, struct netbuf *src, dl_ulong dl_priority,
		dl_ulong dl_service_class)
{
	dl_data_ack_ind_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p) + dst->len + src->len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (dl_data_ack_ind_t *) mp->b_wptr;
		p->dl_primitive = DL_DATA_ACK_IND;
		p->dl_dest_addr_length = dst->len;
		p->dl_dest_addr_offset = dst->len ? sizeof(*p) : 0;
		p->dl_src_addr_length = src->len;
		p->dl_src_addr_offset = src->len ? sizeof(*p) + dst->len : 0;
		p->dl_priority = dl_priority;
		p->dl_service_class = dl_service_class;
		mp->b_wptr += sizeof(*p);
		if (dst->len) {
			bcopy(dst->buf, mp->b_wptr, dst->len);
			mp->b_wptr += dst->len;
		}
		if (src->len) {
			bcopy(src->buf, mp->b_wptr, src->len);
			mp->b_wptr += src->len;
		}
		/* Note: caller must attach their own data */
	}
	return (mp);
}
STATIC INLINE fastcall __hot_get mblk_t *
dl_data_ack_status_ind(queue_t *q, dl_ulong dl_correlation, dl_ulong dl_status)
{
	dl_data_ack_status_ind_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (dl_data_ack_status_ind_t *) mp->b_wptr;
		p->dl_primitive = DL_DATA_ACK_STATUS_IND;
		p->dl_correlation = dl_correlation;
		p->dl_status = dl_status;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}
STATIC INLINE fastcall __hot_get mblk_t *
dl_reply_ind(queue_t *q, struct netbuf *dst, struct netbuf *src, dl_ulong dl_priority,
	     dl_ulong dl_service_class)
{
	dl_reply_ind_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p) + dst->len + src->len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (dl_reply_ind_t *) mp->b_wptr;
		p->dl_primitive = DL_REPLY_IND;
		p->dl_dest_addr_length = dst->len;
		p->dl_dest_addr_offset = dst->len ? sizeof(*p) : 0;
		p->dl_src_addr_length = src->len;
		p->dl_src_addr_offset = src->len ? sizeof(*p) + dst->len : 0;
		p->dl_priority = dl_priority;
		p->dl_service_class = dl_service_class;
		mp->b_wptr += sizeof(*p);
		if (dst->len) {
			bcopy(dst->buf, mp->b_wptr, dst->len);
			mp->b_wptr += dst->len;
		}
		if (src->len) {
			bcopy(src->buf, mp->b_wptr, src->len);
			mp->b_wptr += src->len;
		}
		/* Note: caller must attach their own data */
	}
	return (mp);
}
STATIC INLINE fastcall __hot_get mblk_t *
dl_reply_status_ind(queue_t *q, dl_ulong dl_correlation, dl_ulong dl_status)
{
	dl_reply_status_ind_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (dl_reply_status_ind_t *) mp->b_wptr;
		p->dl_primitive = DL_REPLY_STATUS_IND;
		p->dl_correlation = dl_correlation;
		p->dl_status = dl_status;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}
STATIC INLINE fastcall __hot_get mblk_t *
dl_reply_update_status_ind(queue_t *q, dl_ulong dl_correlation, dl_ulong dl_status)
{
	dl_reply_update_status_ind_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (dl_reply_update_status_ind_t *) mp->b_wptr;
		p->dl_primitive = DL_REPLY_UPDATE_STATUS_IND;
		p->dl_correlation = dl_correlation;
		p->dl_status = dl_status;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}
STATIC mblk_t *
dl_xid_ind(queue_t *q, struct netbuf *dst, struct netbuf *src, dl_ulong dl_flag)
{
	dl_xid_ind_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p) + dst->len + src->len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (dl_xid_ind_t *) mp->b_wptr;
		p->dl_primitive = DL_XID_IND;
		p->dl_flag = dl_flag;
		p->dl_dest_addr_length = dst->len;
		p->dl_dest_addr_offset = dst->len ? sizeof(*p) : 0;
		p->dl_src_addr_length = src->len;
		p->dl_src_addr_offset = src->len ? sizeof(*p) + dst->len : 0;
		mp->b_wptr += sizeof(*p);
		if (dst->len) {
			bcopy(dst->buf, mp->b_wptr, dst->len);
			mp->b_wptr += dst->len;
		}
		if (src->len) {
			bcopy(src->buf, mp->b_wptr, src->len);
			mp->b_wptr += src->len;
		}
		/* Note: caller must attach their own data */
	}
	return (mp);
}
STATIC mblk_t *
dl_xid_con(queue_t *q, struct netbuf *dst, struct netbuf *src, dl_ulong dl_flag)
{
	dl_xid_con_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p) + dst->len + src->len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (dl_xid_con_t *) mp->b_wptr;
		p->dl_primitive = DL_XID_CON;
		p->dl_flag = dl_flag;
		p->dl_dest_addr_length = dst->len;
		p->dl_dest_addr_offset = dst->len ? sizeof(*p) : 0;
		p->dl_src_addr_length = src->len;
		p->dl_src_addr_offset = src->len ? sizeof(*p) + dst->len : 0;
		mp->b_wptr += sizeof(*p);
		if (dst->len) {
			bcopy(dst->buf, mp->b_wptr, dst->len);
			mp->b_wptr += dst->len;
		}
		if (src->len) {
			bcopy(src->buf, mp->b_wptr, src->len);
			mp->b_wptr += src->len;
		}
		/* Note: caller must attach their own data */
	}
	return (mp);
}
STATIC mblk_t *
dl_test_ind(queue_t *q, struct netbuf *dst, struct netbuf *src, dl_ulong dl_flag)
{
	dl_test_ind_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p) + dst->len + src->len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (dl_test_ind_t *) mp->b_wptr;
		p->dl_primitive = DL_TEST_IND;
		p->dl_flag = dl_flag;
		p->dl_dest_addr_length = dst->len;
		p->dl_dest_addr_offset = dst->len ? sizeof(*p) : 0;
		p->dl_src_addr_length = src->len;
		p->dl_src_addr_offset = src->len ? sizeof(*p) + dst->len : 0;
		mp->b_wptr += sizeof(*p);
		if (dst->len) {
			bcopy(dst->buf, mp->b_wptr, dst->len);
			mp->b_wptr += dst->len;
		}
		if (src->len) {
			bcopy(src->buf, mp->b_wptr, src->len);
			mp->b_wptr += src->len;
		}
		/* Note: caller must attach their own data */
	}
	return (mp);
}
STATIC mblk_t *
dl_test_con(queue_t *q, struct netbuf *dst, struct netbuf *src, dl_ulong dl_flag)
{
	dl_test_con_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p) + dst->len + src->len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (dl_test_con_t *) mp->b_wptr;
		p->dl_primitive = DL_TEST_CON;
		p->dl_flag = dl_flag;
		p->dl_dest_addr_length = dst->len;
		p->dl_dest_addr_offset = dst->len ? sizeof(*p) : 0;
		p->dl_src_addr_length = src->len;
		p->dl_src_addr_offset = src->len ? sizeof(*p) + dst->len : 0;
		mp->b_wptr += sizeof(*p);
		if (dst->len) {
			bcopy(dst->buf, mp->b_wptr, dst->len);
			mp->b_wptr += dst->len;
		}
		if (src->len) {
			bcopy(src->buf, mp->b_wptr, src->len);
			mp->b_wptr += src->len;
		}
		/* Note: caller must attach their own data */
	}
	return (mp);
}
STATIC mblk_t *
dl_phys_addr_ack(queue_t *q, struct netbuf *add)
{
	dl_phys_addr_ack_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p) + add->len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (dl_phys_addr_ack_t *) mp->b_wptr;
		p->dl_primitive = DL_PHYS_ADDR_ACK;
		p->dl_addr_length = add->len;
		p->dl_addr_offset = add->len ? sizeof(*p) : 0;
		mp->b_wptr += sizeof(*p);
		if (add->len) {
			bcopy(add->buf, mp->b_wptr, add->len);
			mp->b_wptr += add->len;
		}
		/* Note: caller must attach their own data */
	}
	return (mp);
}
STATIC mblk_t *
dl_get_statistics_ack(queue_t *q, struct netbuf *sta)
{
	dl_get_statistics_ack_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p) + sta->len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (dl_get_statistics_ack_t *) mp->b_wptr;
		p->dl_primitive = DL_PHYS_ADDR_ACK;
		p->dl_stat_length = sta->len;
		p->dl_stat_offset = sta->len ? sizeof(*p) : 0;
		mp->b_wptr += sizeof(*p);
		if (sta->len) {
			bcopy(sta->buf, mp->b_wptr, sta->len);
			mp->b_wptr += sta->len;
		}
		/* Note: caller must attach their own data */
	}
	return (mp);
}

/*
 *  Service Primitives from above -- lower multiplex
 *  ------------------------------------------------
 */

STATIC int
cd_w_rse(queue_t *q, mblk_t *mp)
{
}

/*
 *  Service Primitives from above -- upper multiplex
 *  ------------------------------------------------
 */

STATIC int
dl_info_req(queue_t *q, mblk_t *mp)
{
	mblk_t *bp;

	if ((bp = dl_info_ack(q))) {
		qreply(q, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}
STATIC int
dl_attach_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_attach_req_t *p;
	dl_long dl_errno;
	mblk_t *bp;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (dlpi_get_state(dl) != DL_UNATTACHED)
		goto outstate;
	if (dl->info.dl_provider_style != DL_STYLE2)
		goto outstate;
	p = (dl_attach_req_t *) mp->b_rptr;
	/* FIXME: more stuff to attach ppa */
	dlpi_set_state(dl, DL_ATTACH_PENDING);
	if ((bp = dl_ok_ack(q, DL_ATTACH_REQ))) {
		dl->dl_ppa = p->dl_ppa;
		dlpi_set_state(dl, DL_UNBOUND);
		qreply(q, bp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
      outstate:
	dl_errno = DL_OUTSTATE;
	goto error;
      einval:
	dl_errno = -EPROTO;
	goto error;
      error:
	return dl_error_ack(q, DL_ATTACH_REQ, dl_errno);
}
STATIC int
dl_detach_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_long dl_errno;
	mblk_t *bp;

	if (dlpi_get_state(dl) != DL_UNBOUND)
		goto outstate;
	dlpi_set_state(dl, DL_DETACH_PENDING);
	/* FIXME: more stuff to detach ppa */
	dl->dl_ppa = 0;
	if ((bp = dl_ok_ack(q, DL_DETACH_REQ))) {
		qreply(q, bp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
      outstate:
	dl_errno = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_ack(q, DL_DETACH_REQ, dl_errno);
}
STATIC int
dl_bind_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_bind_req_t *p;
	dl_long dl_errno;
	mblk_t *bp;
	struct netbuf add;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	p = (dl_bind_req_t *) mp->b_rptr;
	if (dlpi_get_state(dl) == DL_UNATTACHED)
		/* well, perhaps we could wildcard attach */
		goto oustate;
	if (dlpi_get_state(dl) != DL_UNBOUND)
		goto outstate;
	if (p->dl_addr_length == 0)
		goto noaddr;
	if (mp->b_wptr < mp->b_rptr + p->dl_addr_length + p->dl_addr_offset)
		goto badaddr;
	if (p->dl_addr_length > dl->add.maxlen)
		goto baddaddr;
	add.len = p->dl_addr_length;
	add.buf = mp->b_rptr + p->dl_addr_offset;
	/* FIXME do some more binding */

	dlpi_set_state(dl, DL_BIND_PENDING);
	if ((bp = dl_bind_ack(q, &add, p->dl_sap, p->dl_max_conind, p->dl_xidtest_flg))) {
		/* commit the bind */
		bcopy(add->buf, dl->add.buf, add->len);
		dl->add.len = add->len;
		dl->dl_sap = p->dl_sap;
		dl->dl_max_conind = p->dl_max_conind;
		dl->dl_xidtest_flg = p->dl_xidtest_flg;
		dl->info.dl_service_mode = p->dl_service_mode;
		dlpi_set_state(dl, DL_BOUND);
		qreply(q, bp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
      badaddr:
	dl_errno = DL_BADADDR;
	goto error;

      noaddr:
	dl_errno = DL_NOADDR;
	goto error;
      outstate:
	dl_errno = DL_OUTSTATE;
	goto error;
      einval:
	dl_errno = -EPROTO;
	goto error;
      error:
	return dl_error_ack(q, DL_BIND_REQ, dl_errno);
}
STATIC int
dl_unbind_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_long dl_errno;
	mblk_t *bp;

	if (dlpi_get_state(dl) != DL_BOUND)
		goto outstate;
	/* FIXME: more stuff to unbind */
	dlpi_set_state(dl, DL_UNBIND_PENDING);
	if ((bp = dl_ok_ack(q, DL_UNBIND_REQ))) {
		dl->add.len = 0;
		dl->dl_sap = 0;
		dlpi_set_state(dl, DL_UNBOUND);
		qreply(q, bp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
      outstate:
	dl_errno = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_ack(q, DL_UNBIND_REQ, dl_errno);
}

/**
 * dl_subs_bind_req
 *
 * Notes: DL_PEER binding will result in multiple DLSAPs (e.g. IP and ARP); DL_HEIRARCHAL will
 * result in a combined DLSAP (e.g, X.121 IDC and DSP Routing Domain Area.)
 */
STATIC int
dl_subs_bind_req(queue_t *q, mblk_t *mp)
{
	dlpi_set_state(dl, DL_SUBS_BIND_PND);
      toomany:
	dl_errno = DL_TOOMANY;
	goto error;
      access:
	dl_errno = DL_ACCESS;
	goto error;
      badaddr:
	dl_errno = DL_BADADDR;
	goto error;
      outstate:
	dl_errno = DL_OUTSTATE;
	goto error;
      unsupported:
	dl_errno = DL_UNSUPPORTED;
	goto error;
      eproto:
	dl_errno = -EPROTO;
	goto error;
      error:
	return dl_error_ack(q, DL_SUBS_BIND_REQ, dl_errno);
}
STATIC int
dl_subs_unbind_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_subs_unbind_req_t *p;
	dl_long dl_errno;
	mblk_t *bp;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (dlpi_get_state(dl) != DL_BOUND)
		goto outstate;
	p = (dl_subs_unbind_req_t *) mp->b_rptr;
	if (p->dl_subs_sap_length == 0)
		goto noaddr;
	if (p->dl_subs_sap_length != dl->sub.len)
		goto badaddr;
	if (strncmp(mp->b_wptr + p->dl_subs_sap_offset, dl->sub.buf, dl->sub.len))
		goto badaddr;
	dlpi_set_state(dl, DL_SUBS_UNBIND_PND);
	/* FIXME: more stuff to unbind */
	if ((bp = dl_ok_ack(q, DL_UNBIND_REQ))) {
		dl->sub.len = 0;
		dlpi_set_state(dl, DL_BOUND);
		qreply(q, bp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
      badaddr:
	dl_errno = DL_BADADDR;
	goto error;
      noaddr:
	dl_errno = DL_NOADDR;
	goto error;
      outstate:
	dl_errno = DL_OUTSTATE;
	goto error;
      einval:
	dl_errno = -EPROTO;
	goto error;
      error:
	return dl_error_ack(q, DL_SUBS_UNBIND_REQ, dl_errno);
}
STATIC int
dl_enabmulti_req(queue_t *q, mblk_t *mp)
{
      error:
	return dl_error_ack(q, DL_ENABMULTI_REQ, dl_errno);
}
STATIC int
dl_disabmulti_req(queue_t *q, mblk_t *mp)
{
      error:
	return dl_error_ack(q, DL_DISABMULTI_REQ, dl_errno);
}
STATIC int
dl_promiscon_req(queue_t *q, mblk_t *mp)
{
      error:
	return dl_error_ack(q, DL_PROMISCON_REQ, dl_errno);
}
STATIC int
dl_promiscoff_req(queue_t *q, mblk_t *mp)
{
      error:
	return dl_error_ack(q, DL_PROMISCOFF_REQ, dl_errno);
}
STATIC int
dl_unitdata_req(queue_t *q, mblk_t *mp)
{
      error:
	return dl_error_ack(q, DL_UNITDATA_REQ, dl_errno);
}
STATIC INLINE fastcall __hot_put int
dl_udqos_req(queue_t *q, mblk_t *mp)
{
      error:
	return dl_error_ack(q, DL_UDQOS_REQ, dl_errno);
}
STATIC int
dl_connect_req(queue_t *q, mblk_t *mp)
{
	dlpi_set_state(dl, DL_OUTCON_PENDING);
      error:
	return dl_error_ack(q, DL_CONNECT_REQ, dl_errno);
}
STATIC int
dl_connect_res(queue_t *q, mblk_t *mp)
{
	dlpi_set_state(dl, DL_CONN_RES_PENDING);
      error:
	return dl_error_ack(q, DL_CONNECT_RES, dl_errno);
}
STATIC int
dl_token_req(queue_t *q, mblk_t *mp)
{
      error:
	return dl_error_ack(q, DL_TOKEN_REQ, dl_errno);
}
STATIC int
dl_disconnect_req(queue_t *q, mblk_t *mp)
{
	dlpi_set_state(dl, DL_DISCON8_PENDING);
	dlpi_set_state(dl, DL_DISCON9_PENDING);
	dlpi_set_state(dl, DL_DISCON11_PENDING);
	dlpi_set_state(dl, DL_DISCON12_PENDING);
	dlpi_set_state(dl, DL_DISCON13_PENDING);
      error:
	return dl_error_ack(q, DL_DISCONNECT_REQ, dl_errno);
}
STATIC INLINE fastcall __hot_put int
dl_reset_req(queue_t *q, mblk_t *mp)
{
	dlpi_set_state(dl, DL_USER_RESET_PENDING);
      error:
	return dl_error_ack(q, DL_RESET_REQ, dl_errno);
}
STATIC INLINE fastcall __hot_put int
dl_reset_res(queue_t *q, mblk_t *mp)
{
	dlpi_set_state(dl, DL_RESET_RES_PENDING);
      error:
	return dl_error_ack(q, DL_RESET_RES, dl_errno);
}
STATIC INLINE fastcall __hot_put int
dl_data_ack_req(queue_t *q, mblk_t *mp)
{
      error:
	return dl_error_ack(q, DL_DATA_ACK_REQ, dl_errno);
}
STATIC INLINE fastcall __hot_put int
dl_reply_req(queue_t *q, mblk_t *mp)
{
      error:
	return dl_error_ack(q, DL_REPLY_REQ, dl_errno);
}
STATIC INLINE fastcall __hot_put int
dl_reply_update_req(queue_t *q, mblk_t *mp)
{
      error:
	return dl_error_ack(q, DL_REPLY_UPDATE_REQ, dl_errno);
}
STATIC int
dl_xid_req(queue_t *q, mblk_t *mp)
{
      error:
	return dl_error_ack(q, DL_XID_REQ, dl_errno);
}
STATIC int
dl_xid_res(queue_t *q, mblk_t *mp)
{
      error:
	return dl_error_ack(q, DL_XID_RES, dl_errno);
}
STATIC int
dl_test_req(queue_t *q, mblk_t *mp)
{
      error:
	return dl_error_ack(q, DL_TEST_REQ, dl_errno);
}
STATIC int
dl_test_res(queue_t *q, mblk_t *mp)
{
      error:
	return dl_error_ack(q, DL_TEST_RES, dl_errno);
}
STATIC int
dl_phys_addr_req(queue_t *q, mblk_t *mp)
{
      error:
	return dl_error_ack(q, DL_PHYS_ADDR_REQ, dl_errno);
}
STATIC int
dl_set_phys_addr_req(queue_t *q, mblk_t *mp)
{
      error:
	return dl_error_ack(q, DL_SET_PHYS_ADDR_REQ, dl_errno);
}
STATIC int
dl_get_statistics_req(queue_t *q, mblk_t *mp)
{
      error:
	return dl_error_ack(q, DL_GET_STATISTICS_REQ, dl_errno);
}

STATIC INLINE fastcall __hot_put int
dl_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn = -EPROTO;
	dl_long prim = 0;
	struct dl *dl = DL_PRIV(q);
	dl_long oldstate = dlpi_get_state(dl);

	if (mp->b_wptr >= mp->b_rptr + sizeof(prim)) {
		switch ((prim = *(dl_long *) mp->b_rptr)) {
		case DL_INFO_REQ:	/* Information Req */
			rtn = dl_info_req(q, mp);
			break;
		case DL_ATTACH_REQ:	/* Attach a PPA */
			rtn = dl_attach_req(q, mp);
			break;
		case DL_DETACH_REQ:	/* Detach a PPA */
			rtn = dl_detach_req(q, mp);
			break;
		case DL_BIND_REQ:	/* Bind dlsap address */
			rtn = dl_bind_req(q, mp);
			break;
		case DL_UNBIND_REQ:	/* Unbind dlsap address */
			rtn = dl_unbind_req(q, mp);
			break;
		case DL_SUBS_BIND_REQ:	/* Bind Subsequent DLSAP address */
			rtn = dl_subs_bind_req(q, mp);
			break;
		case DL_SUBS_UNBIND_REQ:	/* Subsequent unbind */
			rtn = dl_subs_unbind_req(q, mp);
			break;
		case DL_ENABMULTI_REQ:	/* Enable multicast addresses */
			rtn = dl_enabmulti_req(q, mp);
			break;
		case DL_DISABMULTI_REQ:	/* Disable multicast addresses */
			rtn = dl_disabmulti_req(q, mp);
			break;
		case DL_PROMISCON_REQ:	/* Turn on promiscuous mode */
			rtn = dl_promiscon_req(q, mp);
			break;
		case DL_PROMISCOFF_REQ:	/* Turn off promiscuous mode */
			rtn = dl_promiscoff_req(q, mp);
			break;
		case DL_UNITDATA_REQ:	/* datagram send request */
			rtn = dl_unitdata_req(q, mp);
			break;
		case DL_UDQOS_REQ:	/* set QOS for subsequent datagrams */
			rtn = dl_udqos_req(q, mp);
			break;
		case DL_CONNECT_REQ:	/* Connect request */
			rtn = dl_connect_req(q, mp);
			break;
		case DL_CONNECT_RES:	/* Accept previous connect indication */
			rtn = dl_connect_res(q, mp);
			break;
		case DL_TOKEN_REQ:	/* Passoff token request */
			rtn = dl_token_req(q, mp);
			break;
		case DL_DISCONNECT_REQ:	/* Disconnect request */
			rtn = dl_disconnect_req(q, mp);
			break;
		case DL_RESET_REQ:	/* Reset service request */
			rtn = dl_reset_req(q, mp);
			break;
		case DL_RESET_RES:	/* Complete reset processing */
			rtn = dl_reset_res(q, mp);
			break;
		case DL_DATA_ACK_REQ:	/* data unit transmission request */
			rtn = dl_data_ack_req(q, mp);
			break;
		case DL_REPLY_REQ:	/* Request a DLSDU from the remote */
			rtn = dl_reply_req(q, mp);
			break;
		case DL_REPLY_UPDATE_REQ:	/* Hold a DLSDU for transmission */
			rtn = dl_reply_update_req(q, mp);
			break;
		case DL_XID_REQ:	/* Request to send an XID PDU */
			rtn = dl_xid_req(q, mp);
			break;
		case DL_XID_RES:	/* request to send a response XID PDU */
			rtn = dl_xid_res(q, mp);
			break;
		case DL_TEST_REQ:	/* TEST command request */
			rtn = dl_test_req(q, mp);
			break;
		case DL_TEST_RES:	/* TEST response */
			rtn = dl_test_res(q, mp);
			break;
		case DL_PHYS_ADDR_REQ:	/* Request to get physical addr */
			rtn = dl_phys_addr_req(q, mp);
			break;
		case DL_SET_PHYS_ADDR_REQ:	/* set physical addr */
			rtn = dl_set_phys_addr_req(q, mp);
			break;
		case DL_GET_STATISTICS_REQ:	/* Request to get statistics */
			rtn = dl_get_statistics_req(q, mp);
			break;
		case DL_INFO_ACK:	/* Information Ack */
		case DL_BIND_ACK:	/* Dlsap address bound */
		case DL_OK_ACK:	/* Success acknowledgment */
		case DL_ERROR_ACK:	/* Error acknowledgment */
		case DL_SUBS_BIND_ACK:	/* Subsequent DLSAP address bound */
		case DL_UNITDATA_IND:	/* datagram receive indication */
		case DL_UDERROR_IND:	/* datagram error indication */
		case DL_CONNECT_IND:	/* Incoming connect indication */
		case DL_CONNECT_CON:	/* Connection established */
		case DL_TOKEN_ACK:	/* Passoff token ack */
		case DL_DISCONNECT_IND:	/* Disconnect indication */
		case DL_RESET_IND:	/* Incoming reset indication */
		case DL_RESET_CON:	/* Reset processing complete */
		case DL_DATA_ACK_IND:	/* Arrival of a command PDU */
		case DL_DATA_ACK_STATUS_IND:	/* Status indication of DATA_ACK_REQ */
		case DL_REPLY_IND:	/* Arrival of a command PDU */
		case DL_REPLY_STATUS_IND:	/* Status indication of REPLY_REQ */
		case DL_REPLY_UPDATE_STATUS_IND:	/* Status of REPLY_UPDATE req */
		case DL_XID_IND:	/* Arrival of an XID PDU */
		case DL_XID_CON:	/* Arrival of a response XID PDU */
		case DL_TEST_IND:	/* TEST response indication */
		case DL_TEST_CON:	/* TEST Confirmation */
		case DL_PHYS_ADDR_ACK:	/* Return physical addr */
		case DL_GET_STATISTICS_ACK:	/* Return statistics */
			/* wrong direction */
			rtn = -EPROTO;
			break;
		default:
			/* unrecognized primitive */
			rtn = -EOPNOTSUPP;
			break;
		}
	}
	if (rtn < 0) {
		seldom();
		dlpi_set_state(dl, oldstate);
		/* The put and service procedure do not recognize all errors. Sometimes we return
		   an error to here just to restore the previous state. */
		switch (rtn) {
		case -EBUSY:	/* flow controlled */
		case -EAGAIN:	/* try again */
		case -ENOMEM:	/* could not allocate memory */
		case -ENOBUFS:	/* could not allocate an mblk */
		case -EOPNOTSUPP:	/* primitive not supported */
			return dl_error_ack(q, prim, rtn);
		case -EPROTO:
			return dl_error_reply(q, rtn);
		default:
			/* ignore all other errors */
			rtn = 0;
			break;
		}
	}
	return (rtn);
}
STATIC int
dl_w_data(queue_t *q, mblk_t *mp)
{
}
STATIC int
dl_w_ctl(queue_t *q, mblk_t *mp)
{
}
STATIC int
dl_w_ioctl(queue_t *q, mblk_t *mp)
{
}

/*
 *  Service Primitives from below -- upper multiplex
 *  ------------------------------------------------
 *  Messages arriving at the upper multiplex read queue are either passed directly by a CD stream
 *  upon which DL is pushed as a module, or are passed across the multiplex from a lower CD stream.
 *  These are CDI primitives and other lower stream related messages.
 */

STATIC int
dl_r_rse(queue_t *q, mblk_t *mp)
{
}

/*
 *  Service primitives from below -- lower multiplex
 *  ------------------------------------------------
 */

STATIC int
cd_info_ack(queue_t *q, mblk_t *mp)
{
	struct cd *cd = CD_PRIV(q);
	cd_info_ack_t *p;
	cd_ulong mux = 0;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
	p = (cd_info_ack_t *) mp->b_rptr;
}
STATIC int
cd_ok_ack(queue_t *q, mblk_t *mp)
{
}
STATIC int
cd_error_ack(queue_t *q, mblk_t *mp)
{
}
STATIC int
cd_enable_con(queue_t *q, mblk_t *mp)
{
}
STATIC int
cd_disable_con(queue_t *q, mblk_t *mp)
{
}
STATIC int
cd_error_ind(queue_t *q, mblk_t *mp)
{
}

/**
 * cd_unitdata_ack - process a CD_UNITDATA_ACK message
 * @q: lower multiplex or module read queue
 * @mp: the CD_UNITDATA_ACK message
 *
 * This provider originated primitive acknowledges that that previou CD_UNITDATA_REQ primitive as
 * acknowledged as sent.  (Note: sent -- not received.)  CD_UNITDATA_ACK primitives are indicated or
 * not depending on the output style as indicated in the cd_output_style field of the CD_INFO_ACK
 * primitives as follows: CD_UNACKEDOUTPUT -- no C_UNITDATA_ACK primitives will be issued;
 * CD_ACKEDOUTPUT -- CD_UNITDATA_ACK primitives will be issued for every outstanding
 * CD_UNITDATA_REQ; CD_PACEDOUTPUT -- CD_UNITDATA_ACK primitives will only be issued as a timing
 * clude for output.
 */
STATIC INLINE fastcall __hot_read int
cd_unitdata_ack(queue_t *q, mblk_t *mp)
{
	struct cd *cd = CD_PRIV(q);
	cd_unitdata_ack_t *p;
	int err = -EPROTO;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;

	p = (cd_unitdata_ack_t *) mp->b_rptr;

	if (!((1 << cdi_get_state(cd)) & ((1 << CD_OUTPUT_ACTIVE) | (1 << CD_INPUT_ALLOWED))))
		goto outstate;

	switch (cd->info.cd_output_style) {
	case CD_UNACKEDOUTPUT:
		goto eproto;
	case CD_ACKEDOUTPUT:
		/* ackwnowledge to higher level */
		/* If a DL_DATA_ACK_REQ is outstanding from the upper layer (i.e., dl_service_mode
		   in DL_INFO_ACK is set to DL_ACLDS, acknowledged connectionless data service),
		   send a DL_DATA_ACK_STATUS_IND with dl_status of DL_CMD_OK and dl_correlation set 
		   to that of the outstanding DL_DATA_ACK_REQ. */
		break;
	case CD_PACEDOUTPUT:
		/* enable the output section */
		qenable(OTHERQ(q));
		break;
	default:
		goto efault;
	}
	assure((1 << p->cd_state)
	       & ((1 << CD_OUTPUT_ACTIVE) | (1 << CD_READ_ACTIVE) | (1 << CD_INPUT_ALLOWED)));
	cdi_set_state(cd, p->cd_state);
	return (QR_DONE);
      outstate:
	/* FIXME: error the stream */
	/* discard for now */
	return (-EPROTO);
      efault:
	/* FIXME: error the stream */
	/* discard for now */
	return (-EFAULT);
}

/**
 * cd_unitdata_ind - process a CD_UNITDATA_IND message
 * @q: lower multiplex or module read queue
 * @mp: the CD_UNITDATA_IND message
 *
 * This is were most of the real work is done.  This data is the same for both connection-oriented
 * (or should I say call- or circuit-oriented) and connectionless interfaces (e.g. CDMA/MAC).
 *
 * Data can be delivered as just M_DATA blocks too.  That is probably all that is necessary for a
 * permanent HDLC data link connection (e.g. point-to-point Citynet 56kpbs service).
 */
STATIC INLINE fastcall __hot_read int
cd_unitdata_ind(queue_t *q, mblk_t *mp)
{
	struct cd *cd = CD_PRIV(q);
	cd_unitdata_ind_t *p;
	int err = -EPROTO;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;

	p = (cd_unitdata_ind_t *) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + p->cd_src_addr_offset + p->cd_src_addr_length)
		goto efault;

	if (!((1 << cdi_get_state(cd)) & ((1 << CD_READ_ACTIVE) | (1 << CD_INPUT_ALLOWED))))
		goto outstate;

	switch (cd->cd_addr_type) {
	case CD_SPECIFIC:
		/* Indicates that an address is contained in the primitive.  When cd_addr_type is
		   set to CD_SPECIFIC, a destination address is indicated in the
		   cd_dest_addr_length and cd_dest_addr_offset fields. */
		if (p->cd_dest_addr_length == 0)
			goto efault;
		if (mp->b_wptr < mp->b_rptr + p->cd_dest_addr_offset + p->cd_dest_addr_length)
			goto efault;
		break;
	case CD_BROADCAST:
		/* Inidcates that the data was sent to the implicit broadcast address and no
		   specific address follows.  When cd_addr_type is set to CD_BROADCAST, the fields
		   cd_dest_addr_length and cd_dest_addr_offset are coded zero (9) and should be
		   ignored by the CD user. */
		break;
	case CD_IMPLICIT:
		/* Indicates that an implicit addres was used, or that the addressis embedded in
		   the data.  When cd_addr_type is set to CD_IMPLICIT, the fields
		   cd_dest_addr_length cd_dest_addr_offset are coded zero (0) and should be ignored 
		   by the CD user. */
		break;
	case default:
		goto efault;
	}

	/* Set the new state of the input section.  Most of the stuff that we do is full-duplex
	   with a permanently enabled input section so this should normally by CD_INPUT_ALLOWED. */
	assure(p->cd_state == CD_INPUT_ALLOWED);
	cdi_set_state(cd, p->cd_state);

}

STATIC int
cd_bad_frame_ind(queue_t *q, mblk_t *mp)
{
}
STATIC int
cd_modem_sig_ind(queue_t *q, mblk_t *mp)
{
}

STATIC INLINE fastcall __hot_read int
cd_r_proto(queue_t *q, mblk_t *mp)
{
	int rtn = -EPROTO;
	cd_long prim = 0;
	struct cd *cd = CD_PRIV(q);
	cd_long oldstate = cdi_get_state(cd);

	if (mp->b_wptr >= mp->b_rptr + sizeof(prim)) {
		switch ((prim = *(cd_long *) mp->b_rptr)) {
		case CD_INFO_ACK:	/* Information acknowledgement */
			rtn = cd_info_ack(q, mp);
			break;
		case CD_OK_ACK:	/* Success acknowledgement */
			rtn = cd_ok_ack(q, mp);
			break;
		case CD_ERROR_ACK:	/* Error acknowledgement */
			rtn = cd_error_ack(q, mp);
			break;
		case CD_ENABLE_CON:	/* Enable confirmation */
			rtn = cd_enable_con(q, mp);
			break;
		case CD_DISABLE_CON:	/* Disable confirmation */
			rtn = cd_disable_con(q, mp);
			break;
		case CD_ERROR_IND:	/* Error indication */
			rtn = cd_error_ind(q, mp);
			break;
		case CD_UNITDATA_ACK:	/* Data send acknowledgement */
			rtn = cd_unitdata_ack(q, mp);
			break;
		case CD_UNITDATA_IND:	/* Data receive indication */
			rtn = cd_unitdata_ind(q, mp);
			break;
		case CD_BAD_FRAME_IND:	/* frame w/error (Gcom extension) */
			rtn = cd_bad_frame_ind(q, mp);
			break;
		case CD_MODEM_SIG_IND:	/* Report modem signal state (Gcom) */
			rtn = cd_modem_sig_ind(q, mp);
			break;
		case CD_INFO_REQ:	/* Information request */
		case CD_ATTACH_REQ:	/* Attach a PPA */
		case CD_DETACH_REQ:	/* Detach a PPA */
		case CD_ENABLE_REQ:	/* Prepare a device */
		case CD_DISABLE_REQ:	/* Disable a device */
		case CD_ALLOW_INPUT_REQ:	/* Allow input */
		case CD_READ_REQ:	/* Wait-for-input request */
		case CD_UNITDATA_REQ:	/* Data send request */
		case CD_WRITE_READ_REQ:	/* Write/read request */
		case CD_HALT_INPUT_REQ:	/* Halt input */
		case CD_ABORT_OUTPUT_REQ:	/* Abort output */
		case CD_MUX_NAME_REQ:	/* get mux name (Gcom) */
		case CD_MODEM_SIG_REQ:	/* Assert modem signals (Gcom) */
		case CD_MODEM_SIG_POLL:	/* requests a CD_MODEM_SIG_IND (Gcom) */
			/* wrong direction */
			rtn = -EPROTO;
			break;
		default:
			/* unrecognized primitive */
			rtn = -EOPNOTSUPP;
			break;
		}
	}
	if (rtn < 0) {
		seldom();
		cdi_set_state(cd, oldstate);
		/* The put and service procedure do not recognize all errors. Sometimes we return
		   an error to here just to restore the previous state. */
		switch (rtn) {
		case -EBUSY:	/* flow controlled */
		case -EAGAIN:	/* try again */
		case -ENOMEM:	/* could not allocate memory */
		case -ENOBUFS:	/* could not allocate an mblk */
			return (rtn);
		case -EOPNOTSUPP:	/* primitive not supported */
			/* discard it */
			rtn = QR_DONE;
			break;
		case -EPROTO:
			/* FIXME: mark stream bad */
			rtn = QR_DONE;
			break;
		default:
			/* ignore all other errors */
			rtn = 0;
			break;
		}
	}
	return (rtn);
}

STATIC INLINE fastcall __hot_read int
cd_r_data(queue_t *q, mblk_t *mp)
{
}
STATIC int
cd_r_ctl(queue_t *q, mblk_t *mp)
{
}
STATIC int
cd_r_ioctl(queue_t *q, mblk_t *mp)
{
}
STATIC int
cd_r_hangup(queue_t *q, mblk_t *mp)
{
}
STATIC int
cd_r_error(queue_t *q, mblk_t *mp)
{
}

/*
 *  Put and Service procedures
 *  ==========================
 */
/**
 * dl_w_prim - process messages on the upper multiplex write queue
 * @q: upper multiplex write queue
 * @mp: message to process
 */
STATIC int
dl_w_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_PROTO:
	case M_PCPROTO:
		return dl_w_proto(q, mp);
	case M_DATA:
		return dl_w_data(q, mp);
	case M_CTL:
	case M_PCCTL:
		/* intermodule IOCTLs */
		return dl_w_ctl(q, mp);
	case M_IOCTL:
	case M_IOCDATA:
		return dl_w_ioctl(q, mp);
	case M_FLUSH:
		return ss7_w_flush(q, mp);
	}
	swerr();
	return (QR_PASSFLOW);
}

/**
 * dl_r_prim - process messages on the upper multiplex read queue
 * @q: upper multiplex read queue
 * @mp: message to process
 *
 * Message only arrive at the upper multiplex when passed from the other queue in the pair or when
 * passed across the multiplexing driver from the lower multiplex.  Normally, message that are
 * destined upstream are passed to the queue above the upper multiplex.  There is only one situation
 * in which messages are placed on the upper read queue: lock contention on the upper multiplex
 * private structure (from the lower).  Therefore, all messages can do a simple passflow except any
 * special messages that we want to interpret internally across the multiplex.  These are always
 * M_RSE or M_PCRSE messages.
 *
 * Note that if we are pushed as a module, cd_r_prim() is used for the read queue instead of this
 * function.  This function is only used for the read queue of an upper multiplex.
 *
 */
STATIC int
dl_r_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_RSE:
	case M_PCRSE:
		/* intra multiplexer messages */
		return dl_r_rse(q, mp);
	case M_FLUSH:
		return ss7_r_flush(q, mp);
	}
	return (QR_PASSFLOW);
}

/**
 * cd_w_prim - process messages for the lower multiplex write queue
 * @q: lower multiplex write queue
 * @mp: message to process
 *
 * Messages only arrive at the lower multiplex when passed from the other queue in the pair or when
 * passed across the multiplexing driver from the upper multiplex.  Normally, messages that are
 * destined downstream are passed the queue beneath the lower multiplex.  There is only one
 * situation in which messages are placed on the lower write queue: lock contention on the lower
 * multiplex private structure (from the upper).  Therefore, all messages can do a simple passflow
 * except any special messages that we want to interpret internally across the multiplex.  These are
 * always M_RSE or M_PCRSE messages.
 *
 * Note that if we are pushed as a module, dl_w_prim() is used for the write queue instead of this
 * function.  This function is only used for the write queue of a lower multiplex.
 *
 */
STATIC int
cd_w_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_RSE:
	case M_PCRSE:
		/* intra multiplexer messages */
		return cd_w_rse(q, mp);
	case M_FLUSH:
		return ss7_w_flush(q, mp);
	}
	return (QR_PASSFLOW);
}

/**
 * cd_r_prim - process messages on the lower multiplex read queue or the module read queue
 * @q: upper multiplex or module read queue
 * @mp: message to process
 *
 * In general the procedures can detect whether this is a lower multiplex read queue or a module
 * read queue by checking q->q_next.  If it is NULL, this is a lower multiplex read queue; if
 * non-NULL, a module.  Typically, the procedures can check this when considering which upper
 * multiplex stream to which to deliver the message.  When a module, the result is trivial
 * (q->q_next); when a multiplexing driver, some algorithm that considers all upper multiplex
 * streams.
 */
STATIC int
cd_r_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_PROTO:
	case M_PCPROTO:
		return cd_r_proto(q, mp);
	case M_DATA:
		return cd_r_data(q, mp);
	case M_CTL:
	case M_PCCTL:
		/* intermodule IOCTLs */
		return cd_r_ctl(q, mp);
	case M_IOCACK:
	case M_IOCNAK:
	case M_COPYIN:
	case M_COPYOUT:
		return cd_r_ioctl(q, mp);
	case M_HANGUP:
		return cd_r_hangup(q, mp);
	case M_ERROR:
		return cd_r_error(q, mp);
	case M_FLUSH:
		return ss7_r_flush(q, mp);
	}
	swerr();
	return (QR_PASSFLOW);
}

/*
 *  Private structure allocation, deallocate, cache
 *  ===============================================
 */
STATIC int
dl_term_caches(void)
{
	if (dl_priv_cachep != NULL) {
		if (kmem_cache_destroy(dl_priv_cachep)) {
			strlog(DRV_ID, 0, LOG_ERR, SL_ERROR | SL_CONSOLE,
			       "could not destroy dl_priv_cachep");
			return (-EBUSY);
		}
		strlog(DRV_ID, 0, LOG_DEBUG, SL_TRACE, "destroyed dl_priv_cachep");
	}
	return (0);
}
STATIC int
dl_init_caches(void)
{
	if (dl_priv_cachep == NULL) {
		dl_priv_cachep = kmem_cache_create("dl_priv_cachep", sizeof(struct dl), 0,
						   SLAB_HWCACHE_ALIGN, NULL, NULL);
		if (dl_priv_cachep == NULL) {
			strlog(DRV_ID, 0, LOG_WARNING, SL_WARN | SL_CONSOLE,
			       "cannot allocate dl_priv_cachep");
			dl_term_caches();
			return (-ENOMEM);
		}
		strlog(DRV_ID, 0, LOG_DEBUG, SL_TRACE,
		       "initialized driver private structure cache");
	}
	return (0);
}

STATIC struct dl *
dl_alloc_priv(queue_t *q, struct dl **dlp, int sflag, dev_t *devp, cred_t *crp)
{
	struct dl *dl;

	if ((dl = dl_alloc())) {
		/* dl generic members */
		dl->u.dev.cmajor = getmajor(*devp);
		dl->u.dev.cminor = getminor(*devp);
		dl->cred = *crp;
		(dl->oq = q)->q_ptr = dl_get(dl);
		(dl->iq = WR(q))->q_ptr = dl_get(dl);
		if (sflag != MODOPEN) {
			dl->i_prim = &dl_w_prim;
			dl->o_prim = &dl_r_prim;
			dl->i_wakeup = NULL;
			dl->o_wakeup = &dl_r_wakeup;
		} else {
			dl->i_prim = &dl_w_prim;
			dl->o_prim = &cd_r_prim;
			dl->i_wakeup = NULL;
			dl->o_wakeup = NULL;
		}
		spin_lock_init(&dl->qlock);
		dl->i_state = DL_UNATTACHED;
		dl->i_style = DL_STYLE2;
		dl->i_version = DL_CURRENT_VERSION;
		dl->i_oldstate = DL_UNATTACHED;
		/* link into master list */
		if ((dl->next = *dlp))
			dl->next->prev = &dl->next;
		dl->prev = dlp;
		*dlp = dl_get(dl);
	} else
		strlog(DRV_ID, getminor(*devp), LOG_WARNING, SL_WARN | SL_CONSOLE,
		       "could not allocate driver private structure");
	return (dl);
}
STATIC void
dl_free_priv(queue_t *q)
{
	struct dl *dl = DL_PRIV(q);

	/* FIXME: Here we need to handle the disposition of the state of the Stream.  If the state
	   is not unattached, we need to take it there. */

	qprocsoff(q);
	strlog(DRV_ID, dl->u.dev.cminor, LOG_DEBUG, SL_TRACE,
	       "unlinking private structure: reference count = %d", atomic_read(&dl->refcnt));
	ss7_unbufcall((str_t *) dl);
	strlog(DRV_ID, dl->u.dev.cminor, LOG_DEBUG, SL_TRACE,
	       "removed bufcalls: reference count = %d", atomic_read(&dl->refcnt));
	/* remove from master list */
	write_lock_bh(&master.lock);
	if ((*dl->prev = dl->next))
		dl->next->prev = dl->prev;
	dl->next = NULL;
	dl->prev = &dl->next;
	write_unlock_bh(&master.lock);
	strlog(DRV_ID, dl->u.dev.cminor, LOG_DEBUG, SL_TRACE,
	       "unlinked: reference count = %d", atomic_read(&dl->refcnt));
	dl_release(&dl->oq->q_ptr);
	dl->oq = NULL;
	dl_release(&dl->iq->q_ptr);
	dl->iq = NULL;
	assure(atomic_read(&tp->refcnt) == 1);
	dl_release(&dl);
	return;
}

/*
 *  STREAMS Open and Close
 *  ======================
 */
STATIC streamscall int
dl_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	int mindex = 0;
	major_t cmajor = getmajor(*devp);
	major_t cminor = getminor(*devp);
	struct dl *dl, **dlp;

	if (q->q_ptr != NULL)
		return (0);	/* already open */
	if (sflag == MODOPEN || WR(q)->q_next) {
		strlog(DRV_ID, cminor, LOG_WARNING, SL_WARN | SL_CONSOLE, "cannot push as module");
		return (ENXIO);
	}
#ifdef LIS
	if (cmajor != CMAJOR_0) {
		strlog(DRV_ID, cminor, LOG_WARNING, SL_WARN | SL_CONSOLE,
		       "major device number mismatch %d != %d", cmajor, CMAJOR_0);
		return (ENXIO);
	}
#endif				/* LIS */
#ifdef LFS
	/* Linux Fast-STREAMS always passes internal major dvice numbers (module ids) */
	if (cmajor != DRV_ID) {
		strlog(DRV_ID, cminor, LOG_WARNING, SL_WARN | SL_CONSOLE,
		       "major device number mismatch %d != %d", cmajor, DRV_ID);
		return (ENXIO);
	}
#endif				/* LFS */
	if (sflag == CLONEOPEN || cminor < 1) {
		strlog(DRV_ID, cminor, LOG_DEBUG, SL_TRACE, "clone open in effect");
		sflag = CLONEOPEN;
		cminor = 1;
	}
	write_lock_bh(&master.lock);
	for (dlp = &master.drv.list; *dlp; dlp = &(*dlp)->next) {
		if (cmajor != (*dlp)->u.dev.cmajor)
			break;
		if (cmajor == (*dlp)->u.dev.cmajor) {
			if (cminor < (*dlp)->u.dev.cminor) {
				if (++cminor >= NMINORS) {
					if (++mindex >= CMAJORS || !(cmajor = dl_majors[mindex]))
						break;
					cminor = 0;	/* start next major */
				}
				continue;
			}
		}
	}
	if (mindex >= CMAJORS || !cmajor) {
		strlog(DRV_ID, 0, LOG_WARNING, SL_WARN | SL_CONSOLE, "no device numbers available");
		write_unlock_bh(&master.lock);
		return (ENXIO);
	}
	cmn_err(CE_NOTE, "%s: opened character device %d:%d", DRV_NAME, cmajor, cminor);
	*devp = makedevice(cmajor, cminor);
	if (!(dl = dl_alloc_priv(q, dlp, sflag, devp, crp))) {
		write_unlock_bh(&master.lock);
		return (ENOMEM);
	}
	write_unlock_bh(&master.lock);
	qprocson(q);
	return (0);
}

STATIC streamscall int
dl_qclose(queue_t *q, int oflags, cred_t *crp)
{
	struct dl *dl = DL_PRIV(q);

	strlog(DRV_ID, dl->u.dev.cminor, LOG_DEBUG, SL_TRACE, "closing character device");
#ifdef LIS
	/* protect against LiS bugs */
	if (q->q_ptr == NULL) {
		strlog(DRV_ID, dl->u.dev.cminor, LOG_WARNING, SL_WARN | SL_CONSOLE,
		       "LiS double-close bug detected");
		goto quit;
	}
#if 0
	/* only for modules pushed on pipe ends */
	if (q->q_next == NULL) {
		strlog(DRV_ID, dl->u.dev.cminor, LOG_WARNING, SL_WARN | SL_CONSOLE,
		       "LiS pipe bug: called with NULL q->q_next pointer");
		goto skip_pop;
	}
#endif
#endif				/* LIS */
	goto skip_pop;
      skip_pop:
	qprocsoff(q);
	dl_free_priv(q);
	goto quit;
      quit:
	return (0);
}

/*
 *  Registration and Deregistration
 *  ===============================
 */

#ifdef LINUX
unsigned short modid = DRV_ID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else				/* module_param */
module_param(modid, ushort, 0);
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID number for DL driver (0-for allocation).");

major_t major = CMAJOR_0;

#ifndef module_param
MODULE_PARM(major, "h");
#else				/* module_param */
module_param(major, uint, 0);
#endif				/* module_param */
MODULE_PARM_DESC(major, "Major device number for DL driver (0 for allocation).");

#endif				/* LINUX */

#ifdef LFS
STATIC struct cdevsw dl_cdev = {
	.d_name = DRV_NAME,
	.d_str = &dl_info,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

int __init
dl_init(void)
{
	int err;

	cmn_err(CE_NOTE, DRV_BANNER);
	if ((err = dl_init_caches()))
		return (err);
	if ((err = register_strdev(&dl_cdev, major)) < 0) {
		strlog(DRV_ID, 0, LOG_WARNING, SL_WARN | SL_CONSOLE,
		       "could not register major %d", major);
		dl_term_caches();
		return (err);
	}
	if (err > 0)
		major = err;
	return (0);
}

void __exit
dl_exit(void)
{
	unregister_strdev(&dl_cdev, major);
	dl_term_caches();
}

#ifdef MODULE
module_init(dl_init);
module_exit(dl_exit);
#endif				/* MODULE */

#endif				/* LFS */

#ifdef LIS
STATIC int dl_initalized = 0;
STATIC void
dl_init(void)
{
	int err;

	if (dl_initialized != 0)
		return;
	cmn_err(CE_NOTE, DRV_BANNER);	/* console splash */
	if ((err = dl_init_caches())) {
		dl_initialized = err;
		return;
	}
	if ((err = lis_register_strdev(major, &dl_info, UNITS, DRV_NAME)) < 0) {
		strlog(DRV_ID, 0, LOG_WARNING, SL_WARN | SL_CONSOLE,
		       "cannot register major %d", major);
		dl_initialized = err;
		dl_term_caches();
		return;
	}
	dl_initialized = 1;
	if (major = 0 && err > 0) {
		major = err;
		dl_initialized = 2;
	}
	return;
}
STATIC void
dl_terminate(void)
{
	int err;

	if (dl_initialized <= 0)
		return;
	if (major) {
		if ((err = lis_unregister_strdev(major)) < 0)
			strlog(DRV_ID, 0, LOG_CRIT, SL_FATAL | SL_CONSOLE,
			       "cannot unregister major %d", major);
		major = 0;
	}
	dl_term_caches();
	dl_initialized = 0;
	return;
}

#ifdef MODULE
int
init_module(void)
{
	dl_init();
	if (dl_initialized < 0)
		return dl_initialized;
	return (0);
}

void
cleanup_module(void)
{
	return dl_terminate();
}
#endif				/* MODULE */

#endif				/* LIS */
