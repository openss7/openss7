/*****************************************************************************

 @(#) $RCSfile: dl.c,v $ $Name:  $($Revision: 0.9.2.13 $) $Date: 2008-04-25 11:39:31 $

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

 Last Modified $Date: 2008-04-25 11:39:31 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: dl.c,v $
 Revision 0.9.2.13  2008-04-25 11:39:31  brian
 - updates to AGPLv3

 Revision 0.9.2.12  2007/08/15 05:35:40  brian
 - GPLv3 updates

 Revision 0.9.2.11  2007/08/14 03:31:07  brian
 - GPLv3 header update

 Revision 0.9.2.10  2007/07/14 01:37:18  brian
 - make license explicit, add documentation

 Revision 0.9.2.9  2007/03/25 19:02:45  brian
 - changes to support 2.6.20-1.2307.fc5 kernel

 Revision 0.9.2.8  2007/03/25 00:53:47  brian
 - synchronization updates

 Revision 0.9.2.7  2006/07/11 12:32:07  brian
 - added ISO and other implementations to distribution

 Revision 0.9.2.6  2006/05/08 11:26:10  brian
 - post inc problem and working through test cases

 Revision 0.9.2.5  2006/04/18 18:00:44  brian
 - working up DL and NP drivers

 Revision 0.9.2.4  2006/04/13 18:32:49  brian
 - working up DL and NP drivers.

 Revision 0.9.2.3  2006/04/12 20:36:03  brian
 - added some experimental drivers

 Revision 0.9.2.2  2006/04/11 22:08:51  brian
 - working up DL driver

 Revision 0.9.2.1  2006/04/11 18:26:49  brian
 - added new DL driver

 *****************************************************************************/

#ident "@(#) $RCSfile: dl.c,v $ $Name:  $($Revision: 0.9.2.13 $) $Date: 2008-04-25 11:39:31 $"

static char const ident[] =
    "$RCSfile: dl.c,v $ $Name:  $($Revision: 0.9.2.13 $) $Date: 2008-04-25 11:39:31 $";

/*
 *  This multiplexing driver is a master device driver for Data Link Provider streams prsenting a
 *  Data Link Provider Interface (DLPI Revision 2.0.0) Service Interface at the upper boundary.  It
 *  collects a wide range of DLPI drivers into a single device heirarchy using the Linux device
 *  independent packet layer.
 *
 *  CD streams presenting the Communications Device Interface (CDI) Service Interface can be linked
 *  under the multiplexing driver at the lower multiplex.  Rather than directly providing
 *  connections to the link layer, most of these drivers will marshall packets to and from the Linux
 *  device independent packet layer.  As such, it is not necessary to link any CDI streams for the
 *  data link provider to be of use.  Data Link provider streams can access any Linux packet system
 *  network device.
 */

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
#define DL_REVISION	"OpenSS7 $RCSfile: dl.c,v $ $Name:  $ ($Revision: 0.9.2.13 $) $Date: 2008-04-25 11:39:31 $"
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
#define DRV_NAME	DL_DRV_NAME
#define CMAJORS		DL_CMAJORS
#define CMAJOR_0	DL_CMAJOR_0
#define UNITS		DL_UNITS
#ifdef MODULE
#define DRV_BANNER	DL_BANNER
#else				/* MODULE */
#define DRV_BANNER	DL_SPLASH
#endif				/* MODULE */

/* Upper multiplex is a DL provider following the DLPI. */

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

STATIC struct module_info cd_minfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME,		/* Module name */
	.mi_minpsz = 0,			/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,		/* Max packet size acceptd */
	.mi_hiwat = 1 << 15,		/* Hi water mark */
	.mi_hiwat = 1 << 10,		/* Lo water mark */
};

STATIC struct module_stat cd_mstat = {
};

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
 *  A portion of the minor device number is used to determine the mac type of the opened Stream and
 *  the remainder of the minor device number determines the interface id resulting in a Style 1
 *  provider.  Interface Id 0 can act as a control stream for linking additional CDI or DLPI
 *  drivers.  The low order 5 bits of the device number provide the mac type and the high order 3
 *  bits (old 2.4 kernels without device number extensions) or 15 bits (2.6 kernels and LiS) or 11
 *  bits (LfS) provide the unit number.  Unit number 0 is special and does not represent a physical
 *  device and is a Style 2 driver.  These devices need to have a ppa attached, where the ppa is the
 *  device index.  mac type 0 is special (and corresponds to DL_OTHER).  These devices are Style 2
 *  devices and need a mac type to be specified (as part of the ppa) when they are attached.
 *
 *  minor device number
 *  ----------------+-+-+-+-+-+-+-+-+
 *      interface id      |   mac   |
 *  ----------------+-+-+-+-+-+-+-+-+
 *
 *  physical point of attachment
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |    EtherType or MPOxx Code    :         Interface Id          |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
enum {
	DL_MINOR_OTHER = 0,		/* Other */
	DL_MINOR_CSMACD,		/* IEEE 802.3 (ISO 8802/3) CSMA/CD Network */
	DL_MINOR_TPB,			/* IEEE 802.4 (ISO 8802/4) Token Bus */
	DL_MINOR_TPR,			/* IEEE 802.5 (ISO 8802/5 Token Ring */
	DL_MINOR_METRO,			/* IEEE 802.6 (ISO 8802/6) DBDQ */
	DL_MINOR_ETHER,			/* Ethernet Bus */
	DL_MINOR_HDLC,			/* ISO HDLC bit-synchronous communications line */
	DL_MINOR_CHAR,			/* Character synchronous communications line (e.g. BISYNC) */
	DL_MINOR_CTCA,			/* IBM Channel-to-Channel Adapter */
	DL_MINOR_FDDI,			/* FDDI Fibre Distribute Data Interface */
	DL_MINOR_FC,			/* Fiber Channel */
	DL_MINOR_ATM,			/* ATM Asyncrhonous Transfer Mode */
	DL_MINOR_IPATM,			/* ATM Classical IP (CLIP) interace (RFC 1577) */
	DL_MINOR_X25,			/* X.25 LAPB (X.25) */
	DL_MINOR_ISDN,			/* ISDN LAPD (Q.921) */
	DL_MINOR_HIPPI,			/* HIPPI High Performance Parallel Interface */
	DL_MINOR_100VG,			/* 100 Base VG Ethernet */
	DL_MINOR_100VGTPR,		/* 100 Base VG Token Ring */
	DL_MINOR_ETH_CSMA,		/* IEEE 802.3 | ISO 8802/3 and Ethernet */
	DL_MINOR_100BT,			/* 100 Base T */
	DL_MINOR_FRAME,			/* Frame Relay LAPF (Q.922) */
	DL_MINOR_MPFRAME,		/* Multi-protocol over Frame Relay (MPOFR) */
	DL_MINOR_ASYNC,			/* Character asyncrhonous communications line */
	DL_MINOR_IPX25,			/* X.25 Classical IP (X.25 over IP MPOX RFC 1490) */
	DL_MINOR_LOOP,			/* Software loopback */
	DL_MINOR_FREE
};

typedef streamscall int (*dl_event_t) (queue_t *, struct dl *, union dl_event *);
typedef streamscall int (*cd_event_t) (queue_t *, struct cd *, union cd_event *);

STATIC kmem_cachep_t dl_priv_cachep;
STATIC kmem_cachep_t dl_link_cachep;

struct dl {
	STR_DECLARATION (struct dl);	/* Stream declaration */
	struct dl_info_ack dl_info;	/* information structure */
	dl_event_t dl_event;		/* event interface */
	bufq_t dl_conq;			/* connection indication queue */
	uint dl_coninds;		/* outstanding connection indications */
	bufq_t dl_datq;			/* data indications */
	uint dl_datinds;		/* outstanding data indications */
	dl_ulong dl_ppa;		/* attached PPA */
	dl_ulong dl_sap;		/* partial DLSAP */
	dl_ulong dl_level;		/* promiscuity level */
	dl_ulong dl_xidtest_flg;	/* auto XID/TEST flags */
	char dl_addr_buffer[64];	/* address buffer */
	uint dl_addr_length;		/* address length */
	char dl_qos_buffer[64];		/* quality of service buffer */
	uint dl_qos_length;		/* quality of service length */
	char dl_qos_range_buffer[64];	/* quality of service range buffer */
	uint dl_qos_range_length;	/* quality of service range length */
	char dl_brdcst_addr_buffer[64];	/* broadcast address buffer */
	char dl_brdcst_addr_length;	/* broadcast address length */
};

struct cd {
	STR_DECLARATION (struct cd);	/* Stream declaration */
	struct cd_info_ack cd_info;	/* information structure */
	cd_event_t cd_event;		/* event interface */
	bufq_t cd_conq;			/* connection indication queue */
	uint cd_coninds;		/* outstanding connection indications */
	bufq_t cd_datq;			/* data indication queue */
	uint cd_datinds;		/* outstanding data indications */
};

STATIC INLINE fastcall struct dl *
dl_get(struct dl *dl)
{
	if (dl)
		atomic_inc(&dl->refcnt);
	return (dl);
}
STATIC INLINE fastcall void
dl_put(struct dl *dl)
{
	if (dl && atomic_dec_and_test(&dl->refcnt))
		kmem_cache_free(dl_priv_cachep);
}
STATIC INLINE fastcall void
dl_release(struct dl **dlp)
{
	dl_put(xchg(dlp, NULL));
}
STATIC void
dl_alloc(void)
{
	struct dl *dl;

	if ((dl = kmem_cache_alloc(dl_priv_cachep, GFP_ATOMIC))) {
		bzero(dl, sizeof(*dl));
		atomic_set(&dl->refcnt, 1);
		spin_lock_init(&dl->lock);	/* "dl-lock" */
		dl->priv_put = &dl_put;
		dl->priv_get = &dl_get;
	}
	return (dl);
}

STATIC INLINE fastcall struct cd *
cd_get(struct cd *cd)
{
	if (cd)
		atomic_inc(&cd->refcnt);
	return (cd);
}
STATIC INLINE fastcall void
cd_put(struct cd *cd)
{
	if (cd && atomic_dec_and_test(&cd->refcnt))
		kmem_cache_free(dl_link_cachep);
}
STATIC INLINE fastcall void
cd_release(struct cd **cdp)
{
	cd_put(xchg(cdp, NULL));
}
STATIC void
cd_alloc(void)
{
	struct cd *cd;

	if ((cd = kmem_cache_alloc(dl_link_cachep, GFP_ATOMIC))) {
		bzero(cd, sizeof(*cd));
		atomic_set(&cd->refcnt, 1);
		spin_lock_init(&cd->lock);	/* "cd-lock" */
		cd->priv_put = &cd_put;
		cd->priv_get = &cd_get;
	}
	return (cd);
}

/*
 *  STATE CHANGES
 */
STATIC INLINE fastcall dl_ulong
dl_get_state(struct dl *dl)
{
	return (dl->dl_info.dl_current_state);
}
STATIC INLINE fastcall dl_ulong
dl_get_statef(struct dl *dl)
{
	return (1 << dl_get_state(dl));
}
STATIC INLINE fastcall void
dl_set_state(struct dl *dl, dl_ulong state)
{
	dl->dl_info.dl_current_state = state;
}

STATIC INLINE fastcall cd_ulong
cd_get_state(struct cd *cd)
{
	return (cd->cd_info.cd_current_state);
}
STATIC INLINE fastcall cd_ulong
cd_get_statef(struct cd *cd)
{
	return (1 << cd_get_state(cd));
}
STATIC INLINE fastcall void
cd_set_state(struct cd *cd, cd_ulong state)
{
	cd->cd_info.cd_current_state = state;
}

/*
 *  DL PROVIDER PRIMITIVES SENT UPSTREAM
 *  ====================================
 */

STATIC fastcall int
dl_reply_ack(queue_t *q, struct dl *dl, dl_ulong dl_primitive, dl_long error, mblk_t *mp)
{
	union DL_primitives *p = (typeof(p)) mp->b_rptr;

	if (likely(error == 0)) {
		mp->b_datap->db_type = M_PCPROTO;
		mp->b_band = 0;
		p->ok_ack.dl_primitive = DL_OK_ACK;
		p->ok_ack.dl_correct_primitive = dl_primitive;
		mp->b_wptr = mp->b_rptr + sizeof(p->ok_ack);
		mp->b_cont = NULL;
		qreply(q, mp);
		return (QR_ABSORBED);
	}
	/* Note: don't ever call this with a positive queue return code other than QR_DONE (which
	   is really zero), because those codes overlap with provider specific error codes.  The
	   ones to really watch out for are QR_ABSORBED and QR_STRIP. */
	switch (error) {
	case -ENOBUFS:
	case -ENOMEM:
	case -EAGAIN:
	case -EBUSY:
		/* These do not generate an error ack, but get returned directly */
		return (error);
	case -EPROTO:
	case -EMSGSIZE:
	case -EINVAL:
	case -EFAULT:
	case -EOPNOTSUPP:
		break;
	}
	mp->b_datap->db_type = M_PCPROTO;
	mp->b_band = 0;
	p->error_ack.dl_primitive = DL_ERROR_ACK;
	p->error_ack.dl_error_primitive = dl_primitive;
	p->error_ack.dl_errno = error > 0 ? error : DL_SYSERR;
	p->error_ack.dl_unix_errno = error < 0 ? -errno : 0;
	if (mp->b_cont)
		freemsg(XCHG(&mp->b_cont, NULL));
	dl_set_state(dl, dl->i_oldstate);
	qreply(q, mp);
	return (QR_ABSORBED);
}
STATIC fastcall int
dl_reply_err(queue_t *q, struct dl *dl, dl_ulong dl_primitive, dl_long error, mblk_t *mp)
{
	if (likely(error == 0))
		return (QR_DONE);
	switch (error) {
	case -ENOBUFS:
	case -ENOMEM:
	case -EAGAIN:
	case -EBUSY:
		/* These do not generate an error, but get returned directly */
		return (error);
	case -EPROTO:
	case -EMSGSIZE:
	case -EINVAL:
	case -EFAULT:
	case -EOPNOTSUPP:
		break;
	}
	mp->b_datap->db_type = M_ERROR;
	mp->b_band = 0;
	mp->b_wptr = mp->b_rptr;
	*mp->b_wptr++ = EPROTO;
	*mp->b_wptr++ = EPROTO;
	if (mp->b_cont)
		freemsg(XCHG(&mp->b_cont, NULL));
	qreply(q, mp);
	return (QR_ABSORBED);
}

STATIC INLINE fastcall __hot_get int
dl_unitdata_ind(queue_t *q, struct dl *dl, struct dle_unitdata_ind *dle)
{
	dl_unitdata_ind_t *p;
	mblk_t *mp;
	dl_ulong dl_dest_addr_length = dle->dl_dest_addr_length;
	dl_ulong dl_src_addr_length = dle->dl_src_addr_length;
	size_t size = sizeof(*p) + dl_dest_addr_length + dl_src_addr_length;

	if (unlikely((mp = ss7_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;
	if (unlikely(!canputnext(q)))
		goto ebusy;
	mp->b_datap->db_type = M_PROTO;
	p = (dl_unitdata_ind_t *) mp->b_wptr;
	p->dl_primitive = DL_UNITDATA_IND;
	p->dl_dest_addr_length = dl_dest_addr_length;
	p->dl_dest_addr_offset = dl_dest_addr_length ? sizeof(*p) : 0;
	p->dl_src_addr_length = dl_src_addr_length;
	p->dl_src_addr_offset = dl_src_addr_length ? sizeof(*p) + dl_dest_addr_length : 0;
	p->dl_group_address = dle->dl_group_address;
	mp->b_wptr += sizeof(*p);
	if (dl_dest_addr_length) {
		bcopy(dle->dl_dest_addr_buffer, mp->b_wptr, dl_dest_addr_length);
		mp->b_wptr += dl_dest_addr_length;
	}
	if (dl_src_addr_length) {
		bcopy(dle->dl_src_addr_buffer, mp->b_wptr, dl_src_addr_length);
		mp->b_wptr += dl_src_addr_length;
	}
	mp->b_cont = dle->dl_data_blocks;
	putnext(q, mp);
	return (0);
      ebusy:
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}
STATIC INLINE fastcall __hot_get int
dl_uderror_ind(queue_t *q, struct dl *dl, struct dle_uderror_ind *dle)
{
	dl_uderror_ind_t *p;
	mblk_t *mp;
	dl_ulong dl_dest_addr_length = dle->dl_dest_addr_length;
	size_t size = sizeof(*p) + dl_dest_addr_length;

	if (unlikely((mp = ss7_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;
	if (unlikely(!canputnext(q)))
		goto ebusy;
	mp->b_datap->db_type = M_PROTO;
	p = (dl_uderror_ind_t *) mp->b_rptr;
	p->dl_primitive = DL_UDERROR_IND;
	p->dl_dest_addr_length = dl_dest_addr_length;
	p->dl_dest_addr_offset = dl_dest_addr_length ? sizeof(*p) : 0;
	p->dl_unix_errno = dle->dl_unix_errno;
	p->dl_errno = dle->dl_errno;
	mp->b_wptr += sizeof(*p);
	if (dl_dest_addr_length) {
		bcopy(dle->dl_dest_addr_buffer, mp->b_wptr, dl_dest_addr_length);
		mp->b_wptr += dl_dest_addr_length;
	}
	mp->b_cont = dle->dl_data_blocks;
	putnext(q, mp);
	return (0);
      ebusy:
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}
STATIC int
dl_connect_ind(queue_t *q, struct dl *dl, struct dle_connect_ind *dle)
{
	dl_connect_ind_t *p;
	mblk_t *mp;
	dl_ulong dl_called_addr_length = dle->dl_called_addr_length;
	dl_ulong dl_calling_addr_length = dle->dl_calling_addr_length;
	dl_ulong dl_qos_length = dle->dl_qos_length;
	size_t size = sizeof(*p) + dl_called_addr_length + dl_calling_addr_length + dl_qos_length;

	if (unlikely((mp = ss7_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;
	if (unlikely(!canputnext(q)))
		goto ebusy;
	/* FIXME: queue the connection indication */
	if (unlikely(dl->dl_max_conind < dl->dl_coninds)) {
		/* can't indicate, but can queue it */
		return (0);
	}
	mp->b_datap->db_type = M_PROTO;
	p = (dl_connect_ind_t *) mp->b_wptr;
	p->dl_primitive = DL_CONNECT_IND;
	p->dl_correlation = token;
	p->dl_called_addr_length = dl_called_addr_length;
	p->dl_called_addr_offset = dl_called_addr_length ? sizeof(*p) : 0;
	p->dl_calling_addr_length = dl_calling_addr_length;
	p->dl_calling_addr_offset = dl_calling_addr_length ? sizeof(*p) + dl_called_addr_length : 0;
	p->dl_qos_length = dl_qos_length;
	p->dl_qos_offset =
	    dl_qos_length ? sizeof(*p) + dl_called_addr_length + dl_calling_addr_length : 0;
	p->dl_growth = 0;
	mp->b_wptr += sizeof(*p);
	if (dl_called_addr_length) {
		bcopy(dle->dl_called_addr_buffer, mp->b_wptr, dl_called_addr_length);
		mp->b_wptr += dl_called_addr_length;
	}
	if (dl_calling_addr_length) {
		bcopy(dle->dl_calling_addr_buffer, mp->b_wptr, dl_calling_addr_length);
		mp->b_wptr += dl_calling_addr_length;
	}
	if (dl_qos_length) {
		bcopy(dle->dl_qos_buffer, mp->b_wptr, dl_qos_length);
		mp->b_wptr += dl_qos_length;
	}
	dl_set_state(dl, DL_INCON_PENDING);
	putnext(q, mp);
	return (0);
      ebusy:
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}
STATIC int
dl_connect_con(queue_t *q, struct dl *dl, struct dle_connect_con *dle)
{
	dl_connect_con_t *p;
	mblk_t *mp;
	dl_ulong dl_resp_addr_length = dle->dl_resp_addr_length;
	dl_ulong dl_qos_length = dle->dl_qos_length;
	size_t size = sizeof(*p) + dl_resp_addr_length + dl_qos_length;

	if (unlikely((mp = ss7_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (dl_connect_con_t *) mp->b_wptr;
	p->dl_primitive = DL_CONNECT_CON;
	p->dl_resp_addr_length = dl_resp_addr_length;
	p->dl_resp_addr_offset = dl_resp_addr_length ? sizeof(*p) : 0;
	p->dl_qos_length = dl_qos_length;
	p->dl_qos_offset = dl_qos_length ? sizeof(*p) + dl_resp_addr_length : 0;
	p->dl_growth = 0;
	mp->b_wptr += sizeof(*p);
	if (dl_resp_addr_length) {
		bcopy(dle->dl_resp_addr_buffer, mp->b_wptr, dl_resp_addr_length);
		mp->b_wptr += dl_resp_addr_length;
	}
	if (dl_qos_length) {
		bcopy(dle->dl_qos_buffer, mp->b_wptr, dl_qos_length);
		mp->b_wptr += dl_qos_length;
	}
	dl_set_state(dl, DL_DATAXFER);
	putnext(q, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}
STATIC int
dl_disconnect_ind(queue_t *q, struct dl *dl, struct dle_disconnect_ind *dle)
{
	dl_disconnect_ind_t *p;
	mblk_t *mp;
	const size_t size = sizeof(*p);

	if (unlikely((mp = ss7_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (dl_disconect_ind_t *) mp->b_wptr;
	p->dl_primitive = DL_DISCONNECT_IND;
	p->dl_originator = dle->dl_originator;
	p->dl_reason = dle->dl_reason;
	p->dl_correlation = (dl_ulong) (long) dle->dl_correlation;
	mp->b_wptr += sizeof(*p);
	if (dle->dl_correlation != NULL) {
		bufq_dequeue(&dl->dl_conq, dle->dl_correlation);
		freemsg(dle->dl_correlation);
		if (bufq_length(&dl->dl_conq) < 1)
			dl_set_state(dl, DL_IDLE);
		else
			dl_set_state(dl, DL_INCON_PENDING);
	} else
		dl_set_state(dl, DL_IDLE);
	putnext(q, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}
STATIC INLINE fastcall __hot_get int
dl_reset_ind(queue_t *q, struct dl *dl, struct dle_reset_ind *dle)
{
	dl_reset_ind_t *p;
	mblk_t *mp;
	const size_t size = sizeof(*p);

	if (unlikely((mp = ss7_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (dl_reset_ind_t *) mp->b_wptr;
	p->dl_primitive = DL_RESET_IND;
	p->dl_originator = dle->dl_originator;
	p->dl_reason = dle->dl_reason;
	mp->b_wptr += sizeof(*p);
	dl_set_state(dl, DL_PROV_RESET_PENDING);
	putnext(q, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}
STATIC INLINE fastcall __hot_get int
dl_reset_con(queue_t *q)
{
	dl_reset_con_t *p;
	mblk_t *mp;
	const size_t size = sizeof(*p);

	if (unlikely((mp = ss7_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (dl_reset_con_t *) mp->b_wptr;
	p->dl_primitive = DL_RESET_CON;
	mp->b_wptr += sizeof(*p);
	dl_set_state(dl, DL_DATAXFER);
	putnext(q, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}
STATIC INLINE fastcall __hot_get int
dl_data_ack_ind(queue_t *q, struct dl *dl, struct dle_data_ack_ind *dle)
{
	dl_data_ack_ind_t *p;
	mblk_t *mp;
	dl_ulong dl_dest_addr_length = dle->dl_dest_addr_length;
	dl_ulong dl_src_addr_length = dle->dl_src_addr_length;
	size_t size = sizeof(*p) + dl_dest_addr_length + dl_src_addr_length;

	if (unlikely((mp = ss7_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (dl_data_ack_ind_t *) mp->b_wptr;
	p->dl_primitive = DL_DATA_ACK_IND;
	p->dl_dest_addr_length = dl_dest_addr_length;
	p->dl_dest_addr_offset = dl_dest_addr_length ? sizeof(*p) : 0;
	p->dl_src_addr_length = dl_src_addr_length;
	p->dl_src_addr_offset = dl_src_addr_length ? sizeof(*p) + dl_dest_addr_length : 0;
	p->dl_priority = dle->dl_priority;
	p->dl_service_class = dle->dl_service_class;
	mp->b_wptr += sizeof(*p);
	if (dl_dest_addr_length) {
		bcopy(dle->dl_dest_addr_buffer, mp->b_wptr, dl_dest_addr_length);
		mp->b_wptr += dl_dest_addr_length;
	}
	if (dl_src_addr_length) {
		bcopy(dle->dl_src_addr_buffer, mp->b_wptr, dl_src_addr_length);
		mp->b_wptr += dl_src_addr_length;
	}
	mp->b_cont = dle->dl_data_blocks;
	putnext(q, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}
STATIC INLINE fastcall __hot_get int
dl_data_ack_status_ind(queue_t *q, struct dl *dl, struct dle_data_ack_status_ind *dle)
{
	dl_data_ack_status_ind_t *p;
	mblk_t *mp;
	const size_t size = sizeof(*p);

	if (unlikely((mp = ss7_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (dl_data_ack_status_ind_t *) mp->b_wptr;
	p->dl_primitive = DL_DATA_ACK_STATUS_IND;
	p->dl_correlation = dle->dl_correlation;
	p->dl_status = dle->dl_status;
	mp->b_wptr += sizeof(*p);
	mp->b_cont = dle->dl_data_blocks;
	putnext(q, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}
STATIC INLINE fastcall __hot_get int
dl_reply_ind(queue_t *q, struct dl *dl, struct dle_reply_ind *dle)
{
	dl_reply_ind_t *p;
	mblk_t *mp;
	dl_ulong dl_dest_addr_length = dle->dl_dest_addr_length;
	dl_ulong dl_src_addr_length = dle->dl_src_addr_length;
	size_t size = sizeof(*p) + dl_dest_addr_length + dl_src_addr_length;

	if (unlikely((mp = ss7_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (dl_reply_ind_t *) mp->b_wptr;
	p->dl_primitive = DL_REPLY_IND;
	p->dl_dest_addr_length = dl_dest_addr_length;
	p->dl_dest_addr_offset = dl_dest_addr_length ? sizeof(*p) : 0;
	p->dl_src_addr_length = dl_src_addr_length;
	p->dl_src_addr_offset = dl_src_addr_length ? sizeof(*p) + dl_dest_addr_length : 0;
	p->dl_priority = dl_priority;
	p->dl_service_class = dl_service_class;
	mp->b_wptr += sizeof(*p);
	if (dl_dest_addr_length) {
		bcopy(dle->dl_dest_addr_buffer, mp->b_wptr, dl_dest_addr_length);
		mp->b_wptr += dl_dest_addr_length;
	}
	if (dl_src_addr_length) {
		bcopy(dle->dl_src_addr_buffer, mp->b_wptr, dl_src_addr_length);
		mp->b_wptr += dl_src_addr_length;
	}
	mp->b_cont = dle->dl_data_blocks;
	putnext(q, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}
STATIC INLINE fastcall __hot_get int
dl_reply_status_ind(queue_t *q, dl_ulong dl_correlation, dl_ulong dl_status)
{
	dl_reply_status_ind_t *p;
	mblk_t *mp;
	size_t size = sizeof(*p);

	if (unlikely((mp = ss7_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (dl_reply_status_ind_t *) mp->b_wptr;
	p->dl_primitive = DL_REPLY_STATUS_IND;
	p->dl_correlation = dle->dl_correlation;
	p->dl_status = dle->dl_status;
	mp->b_wptr += sizeof(*p);
	mp->b_cont = dle->dl_data_blocks;
	putnext(q, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}
STATIC INLINE fastcall __hot_get int
dl_reply_update_status_ind(queue_t *q, dl_ulong dl_correlation, dl_ulong dl_status)
{
	dl_reply_update_status_ind_t *p;
	mblk_t *mp;
	size_t size = sizeof(*p);

	if (unlikely((mp = ss7_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (dl_reply_update_status_ind_t *) mp->b_wptr;
	p->dl_primitive = DL_REPLY_UPDATE_STATUS_IND;
	p->dl_correlation = dle->dl_correlation;
	p->dl_status = dle->dl_status;
	mp->b_wptr += sizeof(*p);
	mp->b_cont = dle->dl_data_blocks;
	putnext(q, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}
STATIC int
dl_xid_ind(queue_t *q, struct dl *dl, struct dle_xid_ind *dle)
{
	dl_xid_ind_t *p;
	mblk_t *mp;
	dl_ulong dl_dest_addr_length = dle->dl_dest_addr_length;
	dl_ulong dl_src_addr_length = dle->dl_src_addr_length;
	size_t size = sizeof(*p) + dl_dest_addr_length + dl_src_addr_length;

	if (unlikely((mp = ss7_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (dl_xid_ind_t *) mp->b_wptr;
	p->dl_primitive = DL_XID_IND;
	p->dl_flag = dle->dl_flag;
	p->dl_dest_addr_length = dl_dest_addr_length;
	p->dl_dest_addr_offset = dl_dest_addr_length ? sizeof(*p) : 0;
	p->dl_src_addr_length = dl_src_addr_length;
	p->dl_src_addr_offset = dl_src_addr_length ? sizeof(*p) + dl_dest_addr_length : 0;
	mp->b_wptr += sizeof(*p);
	if (dl_dest_addr_length) {
		bcopy(dle->dl_dest_addr_buffer, mp->b_wptr, dl_dest_addr_length);
		mp->b_wptr += dl_dest_addr_length;
	}
	if (dl_src_addr_length) {
		bcopy(dle->dl_src_addr_buffer, mp->b_wptr, dl_src_addr_length);
		mp->b_wptr += dl_src_addr_length;
	}
	mp->b_cont = dle->dl_data_blocks;
	putnext(q, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}
STATIC int
dl_xid_con(queue_t *q, struct dl *dl, struct dle_xid_con *dle)
{
	dl_xid_con_t *p;
	mblk_t *mp;
	dl_ulong dl_dest_addr_length = dle->dl_dest_addr_length;
	dl_ulong dl_src_addr_length = dle->dl_src_addr_length;
	size_t size = sizeof(*p) + dl_dest_addr_length + dl_src_addr_length;

	if (unlikely((mp = ss7_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (dl_xid_con_t *) mp->b_wptr;
	p->dl_primitive = DL_XID_CON;
	p->dl_flag = dle->dl_flag;
	p->dl_dest_addr_length = dl_dest_addr_length;
	p->dl_dest_addr_offset = dl_dest_addr_length ? sizeof(*p) : 0;
	p->dl_src_addr_length = dl_src_addr_length;
	p->dl_src_addr_offset = dl_src_addr_length ? sizeof(*p) + dl_dest_addr_length : 0;
	mp->b_wptr += sizeof(*p);
	if (dl_dest_addr_length) {
		bcopy(dle->dl_dest_addr_buffer, mp->b_wptr, dl_dest_addr_length);
		mp->b_wptr += dl_dest_addr_length;
	}
	if (dl_src_addr_length) {
		bcopy(dle->dl_src_addr_buffer, mp->b_wptr, dl_src_addr_length);
		mp->b_wptr += dl_src_addr_length;
	}
	mp->b_cont = dle->dl_data_blocks;
	putnext(q, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}
STATIC int
dl_test_ind(queue_t *q, struct netbuf *dst, struct netbuf *src, dl_ulong dl_flag)
{
	dl_test_ind_t *p;
	mblk_t *mp;
	dl_ulong dl_dest_addr_length = dle->dl_dest_addr_length;
	dl_ulong dl_src_addr_length = dle->dl_src_addr_length;
	size_t size = sizeof(*p) + dl_dest_addr_length + dl_src_addr_length;

	if (unlikely((mp = ss7_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (dl_test_ind_t *) mp->b_wptr;
	p->dl_primitive = DL_TEST_IND;
	p->dl_flag = dle->dl_flag;
	p->dl_dest_addr_length = dl_dest_addr_length;
	p->dl_dest_addr_offset = dl_dest_addr_length ? sizeof(*p) : 0;
	p->dl_src_addr_length = dl_src_addr_length;
	p->dl_src_addr_offset = dl_src_addr_length ? sizeof(*p) + dl_dest_addr_length : 0;
	mp->b_wptr += sizeof(*p);
	if (dl_dest_addr_length) {
		bcopy(dle->dl_dest_addr_buffer, mp->b_wptr, dl_dest_addr_length);
		mp->b_wptr += dl_dest_addr_length;
	}
	if (dl_src_addr_length) {
		bcopy(dle->dl_src_addr_buffer, mp->b_wptr, dl_src_addr_length);
		mp->b_wptr += dl_src_addr_length;
	}
	mp->b_cont = dle->dl_data_blocks;
	putnext(q, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}
STATIC int
dl_test_con(queue_t *q, struct netbuf *dst, struct netbuf *src, dl_ulong dl_flag)
{
	dl_test_con_t *p;
	mblk_t *mp;
	dl_ulong dl_dest_addr_length = dle->dl_dest_addr_length;
	dl_ulong dl_src_addr_length = dle->dl_src_addr_length;
	size_t size = sizeof(*p) + dl_dest_addr_length + dl_src_addr_length;

	if (unlikely((mp = ss7_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (dl_test_con_t *) mp->b_wptr;
	p->dl_primitive = DL_TEST_CON;
	p->dl_flag = dle->dl_flag;
	p->dl_dest_addr_length = dl_dest_addr_length;
	p->dl_dest_addr_offset = dl_dest_addr_length ? sizeof(*p) : 0;
	p->dl_src_addr_length = dl_src_addr_length;
	p->dl_src_addr_offset = dl_src_addr_length ? sizeof(*p) + dl_dest_addr_length : 0;
	mp->b_wptr += sizeof(*p);
	if (dl_dest_addr_length) {
		bcopy(dle->dl_dest_addr_buffer, mp->b_wptr, dl_dest_addr_length);
		mp->b_wptr += dl_dest_addr_length;
	}
	if (dl_src_addr_length) {
		bcopy(dle->dl_src_addr_buffer, mp->b_wptr, dl_src_addr_length);
		mp->b_wptr += dl_src_addr_length;
	}
	mp->b_cont = dle->dl_data_blocks;
	putnext(q, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

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
 *  STATE MACHINES for Individual Provider Types
 *  ============================================
 */

/*
 * DL_OTHER
 * =========
 */

/* Information */

STATIC dl_info_ack_t dl_other_info = {
	.dl_primitive = DL_INFO_ACK,
	.dl_max_sdu = 0x0000ffff,
	.dl_min_sdu = 0,
	.dl_addr_length = QOS_UNKNOWN,
	.dl_mac_type = DL_OTHER,
	.dl_current_state = DL_UNATTACHED,
	.dl_sap_length = QOS_UNKNOWN,
	.dl_service_mode = (DL_CODLS | DL_CLDLS | DL_ACLDLS),
	.dl_provider_style = DL_STYLE2,
	.dl_version = DL_CURRENT_VERSION,
};

/* Actions */

STATIC int streamscall
dl_other_event(queue_t *q, struct dl *dl, union dl_event *ep)
{
	return (DL_NOTSUPPORTED);
}

/*
 * DL_CSMACD
 * =========
 */

/* Information */

STATIC dl_info_ack_t dl_csmacd_info = {
	.dl_primitive = DL_INFO_ACK,
	.dl_max_sdu = 0x0000ffff,
	.dl_min_sdu = 0,
	.dl_addr_length = QOS_UNKNOWN,
	.dl_mac_type = DL_CSMACD,
	.dl_current_state = DL_UNBOUND,
	.dl_sap_length = QOS_UNKNOWN,
	.dl_service_mode = (DL_CODLS | DL_CLDLS | DL_ACLDLS),
	.dl_provider_style = DL_STYLE1,
	.dl_version = DL_CURRENT_VERSION,
};

/* Actions */

STATIC int streamscall
dl_csmacd_event(queue_t *q, struct dl *dl, union dl_event *ep)
{
	return (DL_NOTSUPPORTED);
}

/*
 * DL_TPB
 * =========
 */

/* Information */

STATIC dl_info_ack_t dl_tpb_info = {
	.dl_primitive = DL_INFO_ACK,
	.dl_max_sdu = 0x0000ffff,
	.dl_min_sdu = 0,
	.dl_addr_length = QOS_UNKNOWN,
	.dl_mac_type = DL_TPB,
	.dl_current_state = DL_UNBOUND,
	.dl_sap_length = QOS_UNKNOWN,
	.dl_service_mode = (DL_CODLS | DL_CLDLS | DL_ACLDLS),
	.dl_provider_style = DL_STYLE1,
	.dl_version = DL_CURRENT_VERSION,
};

/* Actions */

STATIC int streamscall
dl_tpb_event(queue_t *q, struct dl *dl, union dl_event *ep)
{
	return (DL_NOTSUPPORTED);
}

/*
 * DL_TPR
 * =========
 */

/* Information */

STATIC dl_info_ack_t dl_tpr_info = {
	.dl_primitive = DL_INFO_ACK,
	.dl_max_sdu = 0x0000ffff,
	.dl_min_sdu = 0,
	.dl_addr_length = QOS_UNKNOWN,
	.dl_mac_type = DL_TPR,
	.dl_current_state = DL_UNBOUND,
	.dl_sap_length = QOS_UNKNOWN,
	.dl_service_mode = (DL_CODLS | DL_CLDLS | DL_ACLDLS),
	.dl_provider_style = DL_STYLE1,
	.dl_version = DL_CURRENT_VERSION,
};

/* Actions */

STATIC int streamscall
dl_tpr_event(queue_t *q, struct dl *dl, union dl_event *ep)
{
	return (DL_NOTSUPPORTED);
}

/*
 * DL_METRO
 * =========
 */

/* Information */

STATIC dl_info_ack_t dl_metro_info = {
	.dl_primitive = DL_INFO_ACK,
	.dl_max_sdu = 0x0000ffff,
	.dl_min_sdu = 0,
	.dl_addr_length = QOS_UNKNOWN,
	.dl_mac_type = DL_METRO,
	.dl_current_state = DL_UNBOUND,
	.dl_sap_length = QOS_UNKNOWN,
	.dl_service_mode = (DL_CODLS | DL_CLDLS | DL_ACLDLS),
	.dl_provider_style = DL_STYLE1,
	.dl_version = DL_CURRENT_VERSION,
};

/* Actions */

STATIC int streamscall
dl_metro_event(queue_t *q, struct dl *dl, union dl_event *ep)
{
	return (DL_NOTSUPPORTED);
}

/*
 * DL_ether
 * =========
 */

/* Information */

STATIC dl_info_ack_t dl_ether_info = {
	.dl_primitive = DL_INFO_ACK,
	.dl_max_sdu = 0x0000ffff,
	.dl_min_sdu = 0,
	.dl_addr_length = QOS_UNKNOWN,
	.dl_mac_type = DL_ETHER,
	.dl_current_state = DL_UNBOUND,
	.dl_sap_length = QOS_UNKNOWN,
	.dl_service_mode = (DL_CODLS | DL_CLDLS | DL_ACLDLS),
	.dl_provider_style = DL_STYLE1,
	.dl_version = DL_CURRENT_VERSION,
};

/* Actions */

STATIC int streamscall
dl_ether_event(queue_t *q, struct dl *dl, union dl_event *ep)
{
	return (DL_NOTSUPPORTED);
}

/*
 * DL_hdlc
 * =========
 */

/* Information */

STATIC dl_info_ack_t dl_hdlc_info = {
	.dl_primitive = DL_INFO_ACK,
	.dl_max_sdu = 0x0000ffff,
	.dl_min_sdu = 0,
	.dl_addr_length = QOS_UNKNOWN,
	.dl_mac_type = DL_HDLC,
	.dl_current_state = DL_UNBOUND,
	.dl_sap_length = QOS_UNKNOWN,
	.dl_service_mode = (DL_CODLS | DL_CLDLS | DL_ACLDLS),
	.dl_provider_style = DL_STYLE1,
	.dl_version = DL_CURRENT_VERSION,
};

/* Actions */

STATIC int streamscall
dl_hdlc_event(queue_t *q, struct dl *dl, union dl_event *ep)
{
	return (DL_NOTSUPPORTED);
}

/*
 * DL_char
 * =========
 */

/* Information */

STATIC dl_info_ack_t dl_char_info = {
	.dl_primitive = DL_INFO_ACK,
	.dl_max_sdu = 0x0000ffff,
	.dl_min_sdu = 0,
	.dl_addr_length = QOS_UNKNOWN,
	.dl_mac_type = DL_CHAR,
	.dl_current_state = DL_UNBOUND,
	.dl_sap_length = QOS_UNKNOWN,
	.dl_service_mode = (DL_CODLS | DL_CLDLS | DL_ACLDLS),
	.dl_provider_style = DL_STYLE1,
	.dl_version = DL_CURRENT_VERSION,
};

/* Actions */

STATIC int streamscall
dl_char_event(queue_t *q, struct dl *dl, union dl_event *ep)
{
	return (DL_NOTSUPPORTED);
}

/*
 * DL_ctca
 * =========
 */

/* Information */

STATIC dl_info_ack_t dl_ctca_info = {
	.dl_primitive = DL_INFO_ACK,
	.dl_max_sdu = 0x0000ffff,
	.dl_min_sdu = 0,
	.dl_addr_length = QOS_UNKNOWN,
	.dl_mac_type = DL_CTCA,
	.dl_current_state = DL_UNBOUND,
	.dl_sap_length = QOS_UNKNOWN,
	.dl_service_mode = (DL_CODLS | DL_CLDLS | DL_ACLDLS),
	.dl_provider_style = DL_STYLE1,
	.dl_version = DL_CURRENT_VERSION,
};

/* Actions */

STATIC int streamscall
dl_ctca_event(queue_t *q, struct dl *dl, union dl_event *ep)
{
	return (DL_NOTSUPPORTED);
}

/*
 * DL_FDDI
 * =========
 */

/* Information */

STATIC dl_info_ack_t dl_fddi_info = {
	.dl_primitive = DL_INFO_ACK,
	.dl_max_sdu = 0x0000ffff,
	.dl_min_sdu = 0,
	.dl_addr_length = QOS_UNKNOWN,
	.dl_mac_type = DL_FDDI,
	.dl_current_state = DL_UNBOUND,
	.dl_sap_length = QOS_UNKNOWN,
	.dl_service_mode = (DL_CODLS | DL_CLDLS | DL_ACLDLS),
	.dl_provider_style = DL_STYLE1,
	.dl_version = DL_CURRENT_VERSION,
};

/* Actions */

STATIC int streamscall
dl_fddi_event(queue_t *q, struct dl *dl, union dl_event *ep)
{
	return (DL_NOTSUPPORTED);
}

/*
 * DL_FC
 * =========
 */

/* Information */

STATIC dl_info_ack_t dl_fc_info = {
	.dl_primitive = DL_INFO_ACK,
	.dl_max_sdu = 0x0000ffff,
	.dl_min_sdu = 0,
	.dl_addr_length = QOS_UNKNOWN,
	.dl_mac_type = DL_FC,
	.dl_current_state = DL_UNBOUND,
	.dl_sap_length = QOS_UNKNOWN,
	.dl_service_mode = (DL_CODLS | DL_CLDLS | DL_ACLDLS),
	.dl_provider_style = DL_STYLE1,
	.dl_version = DL_CURRENT_VERSION,
};

/* Actions */

STATIC int streamscall
dl_fc_event(queue_t *q, struct dl *dl, union dl_event *ep)
{
	return (DL_NOTSUPPORTED);
}

/*
 * DL_ATM
 * =========
 */

/* Information */

STATIC dl_info_ack_t dl_atm_info = {
	.dl_primitive = DL_INFO_ACK,
	.dl_max_sdu = 0x0000ffff,
	.dl_min_sdu = 0,
	.dl_addr_length = QOS_UNKNOWN,
	.dl_mac_type = DL_ATM,
	.dl_current_state = DL_UNBOUND,
	.dl_sap_length = QOS_UNKNOWN,
	.dl_service_mode = (DL_CODLS | DL_CLDLS | DL_ACLDLS),
	.dl_provider_style = DL_STYLE1,
	.dl_version = DL_CURRENT_VERSION,
};

/* Actions */

STATIC int streamscall
dl_atm_event(queue_t *q, struct dl *dl, union dl_event *ep)
{
	return (DL_NOTSUPPORTED);
}

/*
 * DL_IPATM
 * =========
 */

/* Information */

STATIC dl_info_ack_t dl_ipatm_info = {
	.dl_primitive = DL_INFO_ACK,
	.dl_max_sdu = 0x0000ffff,
	.dl_min_sdu = 0,
	.dl_addr_length = QOS_UNKNOWN,
	.dl_mac_type = DL_IPATM,
	.dl_current_state = DL_UNBOUND,
	.dl_sap_length = QOS_UNKNOWN,
	.dl_service_mode = (DL_CODLS | DL_CLDLS | DL_ACLDLS),
	.dl_provider_style = DL_STYLE1,
	.dl_version = DL_CURRENT_VERSION,
};

/* Actions */

STATIC int streamscall
dl_ipatm_event(queue_t *q, struct dl *dl, union dl_event *ep)
{
	return (DL_NOTSUPPORTED);
}

/*
 * DL_X25
 * =========
 */

/* Information */

STATIC dl_info_ack_t dl_x25_info = {
	.dl_primitive = DL_INFO_ACK,
	.dl_max_sdu = 0x0000ffff,
	.dl_min_sdu = 0,
	.dl_addr_length = QOS_UNKNOWN,
	.dl_mac_type = DL_X25,
	.dl_current_state = DL_UNBOUND,
	.dl_sap_length = QOS_UNKNOWN,
	.dl_service_mode = (DL_CODLS | DL_CLDLS | DL_ACLDLS),
	.dl_provider_style = DL_STYLE1,
	.dl_version = DL_CURRENT_VERSION,
};

/* Actions */

STATIC int streamscall
dl_x25_event(queue_t *q, struct dl *dl, union dl_event *ep)
{
	return (DL_NOTSUPPORTED);
}

/*
 * DL_IDSN
 * =========
 */

/* Information */

STATIC dl_info_ack_t dl_isdn_info = {
	.dl_primitive = DL_INFO_ACK,
	.dl_max_sdu = 0x0000ffff,
	.dl_min_sdu = 0,
	.dl_addr_length = QOS_UNKNOWN,
	.dl_mac_type = DL_ISDN,
	.dl_current_state = DL_UNBOUND,
	.dl_sap_length = QOS_UNKNOWN,
	.dl_service_mode = (DL_CODLS | DL_CLDLS | DL_ACLDLS),
	.dl_provider_style = DL_STYLE1,
	.dl_version = DL_CURRENT_VERSION,
};

/* Actions */

STATIC int streamscall
dl_isdn_event(queue_t *q, struct dl *dl, union dl_event *ep)
{
	return (DL_NOTSUPPORTED);
}

/*
 * DL_HIPPI
 * =========
 */

/* Information */

STATIC dl_info_ack_t dl_hippi_info = {
	.dl_primitive = DL_INFO_ACK,
	.dl_max_sdu = 0x0000ffff,
	.dl_min_sdu = 0,
	.dl_addr_length = QOS_UNKNOWN,
	.dl_mac_type = DL_HIPPI,
	.dl_current_state = DL_UNBOUND,
	.dl_sap_length = QOS_UNKNOWN,
	.dl_service_mode = (DL_CODLS | DL_CLDLS | DL_ACLDLS),
	.dl_provider_style = DL_STYLE1,
	.dl_version = DL_CURRENT_VERSION,
};

/* Actions */

STATIC int streamscall
dl_hippi_event(queue_t *q, struct dl *dl, union dl_event *ep)
{
	return (DL_NOTSUPPORTED);
}

/*
 * DL_100VG
 * =========
 */

/* Information */

STATIC dl_info_ack_t dl_100vg_info = {
	.dl_primitive = DL_INFO_ACK,
	.dl_max_sdu = 0x0000ffff,
	.dl_min_sdu = 0,
	.dl_addr_length = QOS_UNKNOWN,
	.dl_mac_type = DL_100VG,
	.dl_current_state = DL_UNBOUND,
	.dl_sap_length = QOS_UNKNOWN,
	.dl_service_mode = (DL_CODLS | DL_CLDLS | DL_ACLDLS),
	.dl_provider_style = DL_STYLE1,
	.dl_version = DL_CURRENT_VERSION,
};

/* Actions */

STATIC int streamscall
dl_100vg_event(queue_t *q, struct dl *dl, union dl_event *ep)
{
	return (DL_NOTSUPPORTED);
}

/*
 * DL_100VGTPR
 * =========
 */

/* Information */

STATIC dl_info_ack_t dl_100vgtpr_info = {
	.dl_primitive = DL_INFO_ACK,
	.dl_max_sdu = 0x0000ffff,
	.dl_min_sdu = 0,
	.dl_addr_length = QOS_UNKNOWN,
	.dl_mac_type = DL_100VGTPR,
	.dl_current_state = DL_UNBOUND,
	.dl_sap_length = QOS_UNKNOWN,
	.dl_service_mode = (DL_CODLS | DL_CLDLS | DL_ACLDLS),
	.dl_provider_style = DL_STYLE1,
	.dl_version = DL_CURRENT_VERSION,
};

/* Actions */

STATIC int streamscall
dl_100vgtpr_event(queue_t *q, struct dl *dl, union dl_event *ep)
{
	return (DL_NOTSUPPORTED);
}

/*
 * DL_ETH_CSMA
 * =========
 */

/* Information */

STATIC dl_info_ack_t dl_eth_csma_info = {
	.dl_primitive = DL_INFO_ACK,
	.dl_max_sdu = 0x0000ffff,
	.dl_min_sdu = 0,
	.dl_addr_length = QOS_UNKNOWN,
	.dl_mac_type = DL_ETH_CSMA,
	.dl_current_state = DL_UNBOUND,
	.dl_sap_length = QOS_UNKNOWN,
	.dl_service_mode = (DL_CODLS | DL_CLDLS | DL_ACLDLS),
	.dl_provider_style = DL_STYLE1,
	.dl_version = DL_CURRENT_VERSION,
};

/* Actions */

STATIC int streamscall
dl_eth_csma_event(queue_t *q, struct dl *dl, union dl_event *ep)
{
	return (DL_NOTSUPPORTED);
}

/*
 * DL_100BT
 * =========
 */

/* Information */

STATIC dl_info_ack_t dl_100bt_info = {
	.dl_primitive = DL_INFO_ACK,
	.dl_max_sdu = 0x0000ffff,
	.dl_min_sdu = 0,
	.dl_addr_length = QOS_UNKNOWN,
	.dl_mac_type = DL_100BT,
	.dl_current_state = DL_UNBOUND,
	.dl_sap_length = QOS_UNKNOWN,
	.dl_service_mode = (DL_CODLS | DL_CLDLS | DL_ACLDLS),
	.dl_provider_style = DL_STYLE1,
	.dl_version = DL_CURRENT_VERSION,
};

/* Actions */

STATIC int streamscall
dl_100bt_event(queue_t *q, struct dl *dl, union dl_event *ep)
{
	return (DL_NOTSUPPORTED);
}

/*
 * DL_FRAME
 * =========
 */

/* Information */

STATIC dl_info_ack_t dl_frame_info = {
	.dl_primitive = DL_INFO_ACK,
	.dl_max_sdu = 0x0000ffff,
	.dl_min_sdu = 0,
	.dl_addr_length = QOS_UNKNOWN,
	.dl_mac_type = DL_FRAME,
	.dl_current_state = DL_UNBOUND,
	.dl_sap_length = QOS_UNKNOWN,
	.dl_service_mode = (DL_CODLS | DL_CLDLS | DL_ACLDLS),
	.dl_provider_style = DL_STYLE1,
	.dl_version = DL_CURRENT_VERSION,
};

/* Actions */

STATIC int streamscall
dl_frame_event(queue_t *q, struct dl *dl, union dl_event *ep)
{
	return (DL_NOTSUPPORTED);
}

/*
 * DL_MPFRAME
 * =========
 */

/* Information */

STATIC dl_info_ack_t dl_mpframe_info = {
	.dl_primitive = DL_INFO_ACK,
	.dl_max_sdu = 0x0000ffff,
	.dl_min_sdu = 0,
	.dl_addr_length = QOS_UNKNOWN,
	.dl_mac_type = DL_MPFRAME,
	.dl_current_state = DL_UNBOUND,
	.dl_sap_length = QOS_UNKNOWN,
	.dl_service_mode = (DL_CODLS | DL_CLDLS | DL_ACLDLS),
	.dl_provider_style = DL_STYLE1,
	.dl_version = DL_CURRENT_VERSION,
};

/* Actions */

STATIC int streamscall
dl_mpframe_event(queue_t *q, struct dl *dl, union dl_event *ep)
{
	return (DL_NOTSUPPORTED);
}

/*
 * DL_ASYNC
 * =========
 */

/* Information */

STATIC dl_info_ack_t dl_async_info = {
	.dl_primitive = DL_INFO_ACK,
	.dl_max_sdu = 0x0000ffff,
	.dl_min_sdu = 0,
	.dl_addr_length = QOS_UNKNOWN,
	.dl_mac_type = DL_ASYNC,
	.dl_current_state = DL_UNBOUND,
	.dl_sap_length = QOS_UNKNOWN,
	.dl_service_mode = (DL_CODLS | DL_CLDLS | DL_ACLDLS),
	.dl_provider_style = DL_STYLE1,
	.dl_version = DL_CURRENT_VERSION,
};

/* Actions */

STATIC int streamscall
dl_async_event(queue_t *q, struct dl *dl, union dl_event *ep)
{
	return (DL_NOTSUPPORTED);
}

/*
 * DL_IPX25
 * =========
 */

/* Information */

STATIC dl_info_ack_t dl_ipx25_info = {
	.dl_primitive = DL_INFO_ACK,
	.dl_max_sdu = 0x0000ffff,
	.dl_min_sdu = 0,
	.dl_addr_length = QOS_UNKNOWN,
	.dl_mac_type = DL_IPX25,
	.dl_current_state = DL_UNBOUND,
	.dl_sap_length = QOS_UNKNOWN,
	.dl_service_mode = (DL_CODLS | DL_CLDLS | DL_ACLDLS),
	.dl_provider_style = DL_STYLE1,
	.dl_version = DL_CURRENT_VERSION,
};

/* Actions */

STATIC int streamscall
dl_ipx25_event(queue_t *q, struct dl *dl, union dl_event *ep)
{
	return (DL_NOTSUPPORTED);
}

/*
 * DL_LOOP
 * =========
 */

/* Information */

STATIC dl_info_ack_t dl_loop_info = {
	.dl_primitive = DL_INFO_ACK,
	.dl_max_sdu = 0x0000ffff,
	.dl_min_sdu = 0,
	.dl_addr_length = QOS_UNKNOWN,
	.dl_mac_type = DL_LOOP,
	.dl_current_state = DL_UNBOUND,
	.dl_sap_length = QOS_UNKNOWN,
	.dl_service_mode = (DL_CODLS | DL_CLDLS | DL_ACLDLS),
	.dl_provider_style = DL_STYLE1,
	.dl_version = DL_CURRENT_VERSION,
};

/* Actions */

STATIC int streamscall
dl_loop_event(queue_t *q, struct dl *dl, union dl_event *ep)
{
	return (DL_NOTSUPPORTED);
}

/* Information */

STATIC dl_info_ack_t *dl_info[DL_MINOR_FREE] = {
	[DL_MINOR_OTHER] = &dl_other_info,
	[DL_MINOR_CSMACD] = &dl_csmacd_info,
	[DL_MINOR_TPB] = &dl_tpb_info,
	[DL_MINOR_TPR] = &dl_tpr_info,
	[DL_MINOR_METRO] = &dl_metro_info,
	[DL_MINOR_ETHER] = &dl_ether_info,
	[DL_MINOR_HDLC] = &dl_hdlc_info,
	[DL_MINOR_CHAR] = &dl_char_info,
	[DL_MINOR_CTCA] = &dl_ctca_info,
	[DL_MINOR_FDDI] = &dl_fddi_info,
	[DL_MINOR_FC] = &dl_fc_info,
	[DL_MINOR_ATM] = &dl_atm_info,
	[DL_MINOR_IPATM] = &dl_ipatm_info,
	[DL_MINOR_X25] = &dl_x25_info,
	[DL_MINOR_ISDN] = &dl_isdn_info,
	[DL_MINOR_HIPPI] = &dl_hippi_info,
	[DL_MINOR_100VG] = &dl_100vg_info,
	[DL_MINOR_100VGTPR] = &dl_100vgtpr_info,
	[DL_MINOR_ETH_CSMA] = &dl_eth_csma_info,
	[DL_MINOR_100BT] = &dl_100bt_info,
	[DL_MINOR_FRAME] = &dl_frame_info,
	[DL_MINOR_MPFRAME] = &dl_mpframe_info,
	[DL_MINOR_ASYNC] = &dl_async_info,
	[DL_MINOR_IPX25] = &dl_ipx25_info,
	[DL_MINOR_LOOP] = &dl_loop_info,
};

/* Actions */

STATIC dl_event_t dl_event[DL_MINOR_FREE] = {
	[DL_MINOR_OTHER] = &dl_other_event,
	[DL_MINOR_CSMACD] = &dl_csmacd_event,
	[DL_MINOR_TPB] = &dl_tpb_event,
	[DL_MINOR_TPR] = &dl_tpr_event,
	[DL_MINOR_METRO] = &dl_metro_event,
	[DL_MINOR_ETHER] = &dl_ether_event,
	[DL_MINOR_HDLC] = &dl_hdlc_event,
	[DL_MINOR_CHAR] = &dl_char_event,
	[DL_MINOR_CTCA] = &dl_ctca_event,
	[DL_MINOR_FDDI] = &dl_fddi_event,
	[DL_MINOR_FC] = &dl_fc_event,
	[DL_MINOR_ATM] = &dl_atm_event,
	[DL_MINOR_IPATM] = &dl_ipatm_event,
	[DL_MINOR_X25] = &dl_x25_event,
	[DL_MINOR_ISDN] = &dl_isdn_event,
	[DL_MINOR_HIPPI] = &dl_hippi_event,
	[DL_MINOR_100VG] = &dl_100vg_event,
	[DL_MINOR_100VGTPR] = &dl_100vgtpr_event,
	[DL_MINOR_ETH_CSMA] = &dl_eth_csma_event,
	[DL_MINOR_100BT] = &dl_100bt_event,
	[DL_MINOR_FRAME] = &dl_frame_event,
	[DL_MINOR_MPFRAME] = &dl_mpframe_event,
	[DL_MINOR_ASYNC] = &dl_async_event,
	[DL_MINOR_IPX25] = &dl_ipx25_event,
	[DL_MINOR_LOOP] = &dl_loop_event,
};

/*
 *  Service Primitives from above -- lower multiplex
 *  ------------------------------------------------
 */

STATIC int
cd_w_rse(queue_t *q, mblk_t *mp)
{
}

/*
 *  DL USER PRIMITIVES SENT DOWNSTREAM
 *  ==================================
 */

STATIC int
dl_info_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	union DL_primitives *p = (typeof(p)) mp->b_rptr;
	dl_long dl_errno;
	size_t size = sizeof(p->info_ack) + dl->dl_addr_length + dl->dl_qos_length
	    + dl->dl_qos_range_length + dl->dl_brdcst_addr_length;
	mblk_t *pp = mp;

	dl_errno = -EMSGSIZE;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(p->info_req)))
		goto error;
	dl_errno = -EFAULT;
	if (unlikely(p->dl_primitive != DL_INFO_REQ))
		goto error;
	dl_errno = -ENOBUFS;
	if (unlikely(mp->b_datap->db_lim < mp->b_datap->db_base + size))
		if (unlikely((mp = ss7_allocb(q, size, BPRI_MED)) == NULL))
			goto error;
	mp->b_rptr = mp->b_wptr = mp->b_datap->db_base;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_rptr;
	mp->b_wptr += sizeof(p->info_ack);
	p->info_ack = dl->dl_info;
	p->info_ack.dl_primitive = DL_INFO_ACK;
	if (dl->dl_addr_length) {
		bcopy(dl->dl_addr_buffer, mp->b_wptr, dl->dl_addr_length);
		p->info_ack.dl_addr_length = dl->dl_addr_length;
		p->info_ack.dl_addr_offset = mp->b_wptr - mp->b_rptr;
		mp->b_wptr += dl->dl_addr_length;
	}
	if (dl->dl_qos_length) {
		bcopy(dl->dl_qos_buffer, mp->b_wptr, dl->dl_qos_length);
		p->info_ack.dl_qos_length = dl->dl_qos_length;
		p->info_ack.dl_qos_offset = mp->b_wptr - mp->b_rptr;
		mp->b_wptr += dl->dl_qos_length;
	}
	if (dl->dl_qos_range_length) {
		bcopy(dl->dl_qos_range_buffer, mp->b_wptr, dl->dl_qos_range_length);
		p->info_ack.dl_qos_range_length = dl->dl_qos_range_length;
		p->info_ack.dl_qos_range_offset = mp->b_wptr - mp->b_rptr;
		mp->b_wptr += dl->dl_qos_range_length;
	}
	if (dl->dl_brdcst_addr_length) {
		bcopy(dl->dl_brdcst_addr_buffer, mp->b_wptr, dl->dl_brdcst_addr_length);
		p->info_ack.dl_brdcst_addr_length = dl->dl_brdcst_addr_length;
		p->info_ack.dl_brdcst_addr_offset = mp->b_wptr - mp->b_rptr;
		mp->b_wptr += dl->dl_brdcst_addr_length;
	}
	qreply(q, mp);
	return (mp == pp ? QR_ABSORBED : QR_DONE);
      error:
	return dl_reply_ack(q, dl, DL_INFO_REQ, dl_errno, pp);
}

STATIC int
dl_attach_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	union DL_primitives *p = (typeof(p)) mp->b_rptr;
	struct dle_attach_req dle = { DLE_ATTACH_REQ, };
	dl_long dl_errno;

	dl_errno = -EMSGSIZE;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(p->attach_req)))
		goto error;
	dl_errno = -EFAULT;
	if (unlikely(p->dl_primitive != DL_ATTACH_REQ))
		goto error;
	dl_errno = DL_OUTSTATE;
	if (unlikely(dl_get_state(dl) != DL_UNATTACHED))
		goto error;
	dl_errno = DL_UNSUPPORTED;
	if (unlikely(dl->dl_info.dl_provider_style != DL_STYLE2))
		goto error;
	dl_set_state(dl, DL_ATTACH_PENDING);
	/* provider specific attach */
	if (unlikely((dl_errno = dl->dl_event(q, dl, &dle)) != 0))
		dl_set_state(dl, DL_UNBOUND);
      error:
	return dl_reply_ack(q, dl, DL_ATTACH_REQ, dl_errno, mp);
}

STATIC int
dl_detach_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	union DL_primitives *p = (typeof(p)) mp->b_rptr;
	struct dle_detach_req *dle = { DLE_DETACH_REQ, };
	dl_long dl_errno;

	dl_errno = -EMSGSIZE;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(p->detach_req)))
		goto error;
	dl_errno = -EFAULT;
	if (unlikely(p->dl_primitive != DL_DETACH_REQ))
		goto error;
	dl_errno = DL_OUTSTATE;
	if (unlikely(dl_get_state(dl) != DL_UNBOUND))
		goto error;
	dl_errno = DL_UNSUPPORTED;
	if (unlikely(dl->dl_info.dl_provider_style != DL_STYLE2))
		goto error;
	dl_set_state(dl, DL_DETACH_PENDING);
	/* provider specific detach */
	if (unlikely((dl_errno = dl->dl_event(q, dl, &dle)) != 0))
		dl_set_state(dl, DL_UNATTACHED);
      error:
	return dl_reply_ack(q, dl, DL_DETACH_REQ, dl_errno, mp);
}

STATIC int
dl_bind_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	union DL_primitives *p = (typeof(p)) mp->b_rptr;
	struct dle_bind_req *dle = { DLE_BIND_REQ, };
	dl_ulong state;
	dl_long dl_errno;
	const size_t size = sizeof(p->bind_ack) + sizeof(dl->dl_addr_buffer);
	mblk_t *pp = mp;

	state = dl_get_state(dl);
	if (unlikely(state == DL_UNATTACHED)) {
		/* auto attach to wildcard PPA */
		dl->dl_ppa = 0;
		dl_set_state(DL_UNBOUND);
	}
	dl_errno = DL_OUTSTATE;
	if (unlikely(state != DL_UNBOUND))
		goto error;
	dl_errno = -EMSGSIZE;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(p->bind_req)))
		goto error;
	dl_errno = -EFAULT;
	if (unlikely(p->dl_primitive != DL_BIND_REQ))
		goto error;
	dl_errno = -EINVAL;
	if ((dle.dl_service_mode = p->bind_req.dl_service_mode) != DL_CODLS
	    && dle.dl_service_mode != DL_CLDLS && dle.dl_service_mode != DL_ACLDLS)
		goto error;
	dle.dl_sap = p->bind_req.dl_sap;	/* provider specific */
	dle.dl_max_conind = (dle.dl_service_mode == DL_CODLS) ? p->bind_req.dl_max_conind : 0;
	dle.dl_conn_mgmt = (dle.dl_service_mode == DL_CODLS) ? p->bind_req.dl_conn_mgmt : 0;
	dl_errno = -EINVAL;
	if (unlikely((dle.dl_xidtest_flg
		      = p->bind_req.dl_xidtest_flg) & ~(DL_AUTO_XID | DL_AUTO_TEST)))
		goto error;
	dl_errno = -ENOBUFS;
	if (unlikely(mp->b_datap->db_lim < mp->b_datap->db_base + size))
		if (unlikely((mp = ss7_allocb(q, size, BPRI_MED)) == NULL))
			goto error;
	dl_set_state(dl, DL_BIND_PENDING);
	/* provider specific bind */
	if (unlikely((dl_errno = dl->dl_event(q, dl, &dle)) != 0)) {
		freemsg(mp);
		goto error;
	}
	mp->b_rptr = mp->b_wptr = mp->b_datap->db_base;
	mp->b_datap->db_type = M_PCPROTO;
	mp->b_band = 0;
	p = (typeof(p)) mp->b_rptr;
	p->bind_ack.dl_primitive = DL_BIND_ACK;
	p->bind_ack.dl_sap = dle.dl_sap;
	p->bind_ack.dl_addr_length = dle.dl_addr_length;
	p->bind_ack.dl_addr_offset = dle.dl_addr_length ? sizeof(p->bind_ack) : 0;
	p->bind_ack.dl_max_conind = dle.dl_max_conind;
	p->bind_ack.dl_xidtest_flg = dle.dl_xidtest_flg;
	mp->b_wptr += sizeof(p->bind_ack);
	if (dle.dl_addr_length) {
		bcopy(dle.dl_addr_buffer, mp->b_wptr, dle.dl_addr_length);
		mp->b_wptr += dle.dl_addr_length;
	}
	dl_set_state(dl, DL_IDLE);
	qreply(q, mp);
	return (mp == pp ? QR_ABSORBED : QR_DONE);
      error:
	return dl_reply_ack(q, dl, DL_BIND_REQ, dl_errno, pp);
}

STATIC int
dl_unbind_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	union DL_primitives *p = (typeof(p)) mp->b_rptr;
	struct dle_unbind_req *dle = { DLE_UNBIND_REQ, };
	dl_long dl_errno;

	dl_errno = DL_OUTSTATE;
	if (unlikely(dl_get_state(dl) != DL_IDLE))
		goto error;
	dl_errno = -EMSGSIZE;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(p->unbind_req)))
		goto error;
	dl_errno = -EFAULT;
	if (unlikely(p->dl_primitive != DL_UNBIND_REQ))
		goto error;
	dl_set_state(dl, DL_UNBIND_PENDING);
	/* provider specific unbind */
	if (unlikely((dl_errno = dl->dl_event(q, dl, &dle)) != 0))
		dl_set_state(dl, DL_UNBOUND);
      error:
	return dl_reply_ack(q, dl, DL_UNBIND_REQ, dl_errno, mp);
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
	struct dl *dl = DL_PRIV(q);
	union DL_primitives *p = (typeof(p)) mp->b_rptr;
	struct dle_subs_bind_req *dle = { DLE_SUBS_BIND_REQ, };
	dl_long dl_errno;
	const size_t size = sizeof(p->subs_bind_ack) + sizeof(dl->dl_addr_buffer);
	mblk_t *pp = mp;

	dl_errno = -EMSGSIZE;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(p->subs_bind_req)))
		goto error;
	dl_errno = -EFAULT;
	if (unlikely(p->dl_primitive != DL_SUBS_BIND_REQ))
		goto error;
	dl_errno = DL_OUTSTATE;
	if (unlikely(dl_get_state(dl) != DL_IDLE))
		goto error;
	dl_errno = DL_NOADDR;
	if (unlikely((dle.dl_subs_sap_length = p->subs_bind_req.dl_subs_sap_length) == 0))
		goto error;
	dl_errno = DL_BADADDR;
	if (unlikely(mp->b_wptr < (dle.dl_subs_sap_buffer
				   = mp->b_rptr + p->subs_bind_req.dl_subs_sap_offset)))
		goto error;
	dl_errno = DL_UNSUPPORTED;
	if (unlikely((dle.dl_subs_bind_class = p->subs_bind_req.dl_subs_bind_class) != DL_PEER_BIND
		     && dle.dl_subs_bind_class != DL_HIERARCHICAL_BIND))
		goto error;
	dl_errno = -ENOBUFS;
	if (unlikely(mp->b_datap->db_lim < mp->b_datap->db_base + size))
		if (unlikely((mp = ss7_allocb(q, size, BPR_MED) == NULL)))
			goto error;
	dl_set_state(dl, DL_SUBS_BIND_PND);
	/* provider specific subsequent bind */
	if (unlikely((dl_errno = dl->dl_event(q, dl, &dle)) != 0)) {
		freemsg(mp);
		goto error;
	}
	mp->b_rptr = mp->b_wptr = mp->b_datap->db_base;
	mp->b_datap->db_type = M_PCPROTO;
	bp->b_band = 0;
	p = (typeof(p)) mp->b_rptr;
	p->subs_bind_ack.dl_primitive = DL_SUBS_BIND_ACK;
	p->subs_bind_ack.dl_subs_sap_offset = dle.dl_subs_sap_length ? sizeof(p->subs_bind_ack) : 0;
	p->subs_bind_ack.dl_subs_sap_length = dle.dl_subs_sap_length;
	mp->b_wptr += sizeof(p->subs_bind_ack);
	dl_set_state(dl, DL_IDLE);
	qreply(q, mp);
	return (QR_DONE);
      error:
	return dl_reply_ack(q, dl, DL_SUBS_BIND_REQ, dl_errno, pp);
}

STATIC int
dl_subs_unbind_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	union DL_primitives *p = (typeof(p)) mp->b_rptr;
	struct dle_subs_unbind_req dle = { DLE_SUBS_UNBIND_REQ, };
	dl_long dl_errno;

	dl_errno = -EMSGSIZE;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(p->subs_unbind_req)))
		goto error;
	dl_errno = -EFAULT;
	if (unlikely(p->dl_primitive != DL_SUBS_UNBIND_REQ))
		goto error;
	dl_errno = DL_OUTSTATE;
	if (unlikely(dl_get_state(dl) != DL_IDLE))
		goto error;
	dl_errno = DL_NOADDR;
	if (unlikely((dle.dl_subs_sap_length = p->subs_unbind_req.dl_subs_sap_length) == 0))
		goto error;
	dl_errno = DL_BADADDR;
	if (unlikely(mp->b_wptr < (dle.dl_subs_sap_buffer
				   = mp->b_rptr + p->subs_unbind_req.dl_subs_sap_offset)))
		goto error;
	dl_set_state(dl, DL_SUBS_UNBIND_PND);
	/* provider specific subsequent unbind */
	if (unlikely((dl_errno = dl->dl_event(q, dl, &dle)) != 0))
		goto error;
	dl_set_state(dl, DL_IDLE);
      error:
	return dl_reply_ack(q, dl, DL_SUBS_UNBIND_REQ, dl_errno, mp);
}

STATIC int
dl_enabmulti_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	union DL_primitives *p = (typeof(p)) mp->b_rptr;
	struct dle_enabmulti_req dle = { DLE_ENABMULTI_REQ, };
	dl_long dl_errno;

	dl_errno = -EMSGSIZE;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(p->enabmulti_req)))
		goto error;
	dl_errno = -EFAULT;
	if (unlikely(p->dl_primitive != DL_ENABMULTI_REQ))
		goto error;
	dl_errno = DL_OUTSTATE;
	if (unlikely(dl_get_state(dl) == DL_UNATTACHED))
		goto error;
	dl_errno = DL_NOADDR;
	if (unlikely((dle.dl_addr_length = p->enabmulti_req.dl_addr_length) == 0))
		goto error;
	dl_errno = DL_BADADDR;
	if (unlikely(mp->b_wptr < (dle.dl_addr_buffer
				   = mp->b_rptr + p->enabmulti_req.dl_addr_offset)))
		goto error;
	/* provider specific enable multicast */
	if (unlikely((dl_errno = dl->dl_event(q, dl, &dle)) != 0))
		goto error;
      error:
	return dl_reply_ack(q, dl, DL_ENABMULTI_REQ, dl_errno, mp);
}

STATIC int
dl_disabmulti_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	union DL_primitives *p = (typeof(p)) mp->b_rptr;
	struct dle_disabmulti_req dle = { DLE_DISABMULTI_REQ, };
	dl_long dl_errno;

	dl_errno = -EMSGSIZE;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(p->disabmulti_req)))
		goto error;
	dl_errno = -EFAULT;
	if (unlikely(p->dl_primitive != DL_DISABMULTI_REQ))
		goto error;
	dl_errno = DL_OUTSTATE;
	if (unlikely(dl_get_state(dl) == DL_UNATTACHED))
		goto error;
	dl_errno = DL_NOADDR;
	if (unlikely((dle.dl_addr_length = p->disabmulti_req.dl_addr_length) == 0))
		goto error;
	dl_errno = DL_BADADDR;
	if (unlikely(mp->b_wptr < (dle.dl_addr_buffer
				   = mp->b_rptr + p->disabmulti_req.dl_addr_offset)))
		goto error;
	/* provider specific disable multicast */
	if (unlikely((dl_errno = dl->dl_event(q, dl, &dle)) != 0))
		goto error;
      error:
	return dl_reply_ack(q, dl, DL_DISABMULTI_REQ, dl_errno, mp);
}

STATIC int
dl_promiscon_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	union DL_primitives *p = (typeof(p)) mp->b_rptr;
	struct dle_promiscon_req dle = { DLE_PROMISCON_REQ, };
	dl_long dl_errno;

	dl_errno = -EMSGSIZE;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(p->promsicon_req)))
		goto error;
	dl_errno = -EFAULT;
	if (unlikely(p->dl_primitive != DL_PROMISCON_REQ))
		goto error;
	dl_errno = DL_UNSUPPORTED;
	if (unlikely((dle.dl_level = p->promiscon_req.dl_level) != DL_PROMISC_PHYS
		     && dle.dl_level != DL_PROMISC_SAP && dle.dl_level != DL_PROMISC_MULTI))
		goto error;
	if (unlikely((dl_errno = dl->dl_event(q, dl, &dle)) != 0))
		goto error;
	dl->dl_level = dle.dl_level;
      error:
	return dl_reply_ack(q, dl, DL_PROMISCON_REQ, dl_errno, mp);
}

STATIC int
dl_promiscoff_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	union DL_primitives *p = (typeof(p)) mp->b_rptr;
	struct dle_promiscoff_req dle = { DLE_PROMISCOFF_REQ, };
	dl_long dl_errno;

	dl_errno = -EMSGSIZE;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(p->promsicoff_req)))
		goto error;
	dl_errno = -EFAULT;
	if (unlikely(p->dl_primitive != DL_PROMISCOFF_REQ))
		goto error;
	dl_errno = DL_UNSUPPORTED;
	if (unlikely((dle.dl_level = p->promiscoff_req.dl_level) != DL_PROMISC_PHYS
		     && dle.dl_level != DL_PROMISC_SAP && dle.dl_level != DL_PROMISC_MULTI))
		goto error;
	dl_errno = DL_NOTENAB;
	if (unlikely(dle.dl_level != dl->dl_level))
		goto error;
	if (unlikely((dl_errno = dl->dl_event(q, dl, &dle))))
		goto error;
	dl->dl_level = 0;
      error:
	return dl_reply_ack(q, dl, DL_PROMISCOFF_REQ, dl_errno, mp);
}

STATIC int
dl_unitdata_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	union DL_primitives *p = (typeof(p)) mp->b_rptr;
	struct dle_unitdata_req dle = { DLE_UNITDATA_REQ, };
	dl_long dl_errno;
	size_t size;

	dl_errno = -EMSGSIZE;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(p->unitdata_req)))
		goto error;
	dl_errno = -EFAULT;
	if (unlikely(p->dl_primitive != DL_UNITDATA_REQ))
		goto error;
	dl_errno = DL_OUTSTATE;
	if (unlikely(dl_get_state(dl) != DL_IDLE))
		goto error;
	dl_errno = DL_NOADDR;
	if (unlikely((dle.dl_addr_length = p->unitdata_req.dl_addr_length) == 0))
		goto error;
	dl_errno = DL_BADADDR;
	if (unlikely(mp->b_wptr <
		     (dle.dl_addr_buffer = mp->b_rptr = p->unitdata_req.dl_addr_offset)))
		goto error;
	dl_errno = DL_BADDATA;
	if (unlikely((dle.dl_data_blocks = mp->b_cont) == NULL))
		goto error;
	dl_errno = DL_BADDATA;
	if (unlikely((dle.dl_data_length = msgsize(mp->b_cont)) < dl->dl_info.dl_min_sdu
		     || dle.dl_data_length > dl->dl_info.dl_max_sdu))
		goto error;
	dle.dl_priority = p->unitdata_req.dl_priority;
	/* provider specific unit data transmission */
	if (unlikely((dl_errno = dl->dl_event(q, dl, &dle)) != 0))
		goto error;
	return (QR_TRIMMED);
      error:
	size = sizeof(p->uderror_ind) + dle.dl_addr_length;

	if (unlikely((mp = ss7_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;

	mp->b_datap->db_type = M_PCPROTO;
	mp->b_band = 0;
	p = (typeof(p)) mp->b_rptr;
	p->uderror_ind.dl_primitive = DL_UDERROR_IND;
	p->uderror_ind.dl_addr_length = 0;
	p->uderror_ind.dl_addr_offset = 0;
	p->uderror_ind.dl_unix_errno = dl_errno < 0 ? -dl_errno : 0;
	p->uderror_ind.dl_errno = dl_errno > 0 ? dl_errno : DL_SYSERR;
	mp->b_wptr += sizeof(p->uderror_ind);
	if (dle.dl_addr_length) {
		p->uderror_ind.dl_addr_length = dle.dl_addr_length;
		p->uderror_ind.dl_addr_offset = mp->b_wptr - mp->b_rptr;
		bcopy(dle.dl_addr_buffer, mp->b_wptr, dle.dl_addr_length);
		mp->b_wptr += dle.dl_addr_length;
	}
	mp->b_cont = dle.dl_data_blocks;
	qreply(q, mp);
	return (QR_TRIMMED);
      enobufs:
	return (-ENOBUFS);
}

STATIC INLINE fastcall __hot_put int
dl_udqos_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	union DL_primitives *p = (typeof(p)) mp->b_rptr;
	struct dle_udqos_req dle = { DLE_UDQOS_REQ, };
	dl_long dl_errno;

	dl_errno = -EMSGSIZE;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(p->udqos_req)))
		goto error;
	dl_errno = -EFAULT;
	if (unlikely(p->dl_primitive != DL_UDQOS_REQ))
		goto error;
	dl_errno = DL_OUTSTATE;
	if (unlikely(dl_get_state(dl) != DL_IDLE))
		goto error;
	dl_errno = DL_BADQOSTYPE;
	if (unlikely((dle.dl_qos_length = p->udqos_req.dl_qos_length) < sizeof(dl_ulong)))
		goto error;
	dl_errno = DL_BADQOSTYPE;
	if (unlikely(mp->b_wptr < (dle.dl_qos_buffer = mp->b_rptr + p->udqos_req.dl_qos_offset)))
		goto error;
	/* provider specific data qos */
	if (unlikely((dl_errno = dl->dl_event(q, dl, &dle)) != 0))
		goto error;
      error:
	return dl_reply_ack(q, dl, DL_UDQOS_REQ, dl_errno, mp);
}

STATIC int
dl_connect_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	union DL_primitives *p = (typeof(p)) mp->b_rptr;
	struct dle_connect_req dle = { DLE_CONNECT_REQ, };
	dl_long dl_errno;

	dl_errno = -EMSGSIZE;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(p->connect_req)))
		goto error;
	dl_errno = -EFAULT;
	if (unlikely(p->dl_primitive != DL_CONNECT_REQ))
		goto error;
	dl_errno = DL_NOTSUPPORTED;
	if (unlikely(dl->dl_service_mode != DL_CODLS))
		goto error;
	dl_errno = DL_OUTSTATE;
	if (unlikely(dl_get_state(dl) != DL_IDLE))
		goto error;
	dl_errno = DL_NOADDR;
	if (unlikely((dle.dl_dest_addr_length = p->connect_req.dl_dest_addr_length) == 0))
		goto error;
	dl_errno = DL_BADADDR;
	if (unlikely(mp->b_wptr <
		     (dle.dl_dest_addr_buffer = mp->b_rptr + p->connect_req.dl_dest_addr_offset)))
		goto error;
	dl_errno = DL_BADQOSTYPE;
	if (unlikely((dle.dl_qos_length = p->connect_req.dl_qos_length) < sizeof(dl_ulong)))
		goto error;
	dl_errno = DL_BADQOSTYPE;
	if (unlikely(mp->b_wptr < (dle.dl_qos_buffer = mp->b_rptr + p->connect_req.dl_qos_offset)))
		goto error;
	dl_set_state(dl, DL_OUTCON_PENDING);
	/* provider specific connection establishment */
	if (unlikely((dl_errno = dl->dl_event(q, dl, &dle)) != 0))
		goto error;
	return (QR_DONE);
      error:
	return dl_reply_ack(q, dl, DL_CONNECT_REQ, dl_errno, mp);
}

STATIC int
dl_connect_res(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	union DL_primitives *p = (typeof(p)) mp->b_rptr;
	struct dle_connect_res dle = { DLE_CONNECT_RES, };
	dl_long dl_errno;

	dl_errno = -EMSGSIZE;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(p->connect_res)))
		goto error;
	dl_errno = -EFAULT;
	if (unlikely(p->dl_primitive != DL_CONNECT_RES))
		goto error;
	dl_errno = DL_NOTSUPPORTED;
	if (unlikely(dl->dl_service_mode != DL_CODLS))
		goto error;
	dl_errno = DL_OUTSTATE;
	if (unlikely(dl_get_state(dl) != DL_INCON_PENDING))
		goto error;
	dle.dl_correlation = dl_find_sequence(p->connect_res.dl_correlation);
	dl_errno = DL_BADCORR;
	if (unlikely(dle.dl_correlation == NULL))
		goto error;
	if (p->connect_res.dl_resp_token) {
		dle.dl_resp_token = dl_find_token(p->connect_res.dl_resp_token);
		dl_errno = DL_BADTOKEN;
		if (unlikely(dle.dl_resp_token == NULL))
			goto error;
	}
	if (dle.dl_resp_token == NULL)
		dle.dl_resp_token = dl;
	dl_errno = DL_PENDING;
	if (unlikely(dle.dl_resp_token == dl && dl->dl_coninds > 1))
		goto error;
	dle.dl_qos_length = p->connect_res.dl_qos_length;
	dl_errno = DL_BADQOSTYPE;
	if (unlikely(dle.dl_qos_length < sizeof(dl_ulong)))
		goto error;
	dle.dl_qos_buffer = mp->b_rptr + p->connect_res.dl_qos_offset;
	dl_errno = DL_BADQOSTYPE;
	if (unlikely(mp->b_wptr < dle.dl_qos_buffer))
		goto error;
	dlpi_set_state(dl, DL_CONN_RES_PENDING);
	if (unlikely((dl_errno = dl->dl_event(q, dl, &dle)) != 0))
		goto error;
	bufq_dequeue(&dl->dl_conq, dle.dl_correlation);
	dl->dl_coninds--;
	freemsg(dle.dl_correlation);
	dl_set_state(dle.dl_resp_token, DL_DATAXFER);
	if (dl != dle.dl_resp_token) {
		if (bufq_length(&dl->dl_conq) > 0)
			dl_set_state(dl, DL_INCON_PENDING);
		else
			dl_set_state(dl, DL_IDLE);
	}
      error:
	return dl_reply_ack(q, dl, DL_CONNECT_RES, dl_errno, mp);
}

STATIC int
dl_token_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	union DL_primitives *p = (typeof(p)) mp->b_rptr;
	dl_long dl_errno;

	dl_errno = -EMSGSIZE;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(p->token_req)))
		goto error;
	dl_errno = -EFAULT;
	if (unlikely(p->dl_primitive != DL_TOKEN_REQ))
		goto error;
	mp->b_datap->db_type = M_PCPROTO;
	mp->b_band = 0;
	p->dl_primitive = DL_TOKEN_ACK;
	p->dl_token = (dl_ulong) (long) dl;
	mp->b_wptr = mp->b_rptr + sizeof(p->token_ack);
	qreply(q, mp);
	return (QR_ABSORBED);
      error:
	return dl_reply_ack(q, dl, DL_TOKEN_REQ, dl_errno, mp);
}

STATIC int
dl_disconnect_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	union DL_primitives *p = (typeof(p)) mp->b_rptr;
	struct dle_disconnect_req dle = { DLE_DISCONNECT_REQ, };
	dl_long dl_errno;
	dl_ulong state, nextstate;

	dl_errno = -EMSGSIZE;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(p->disconnect_req)))
		goto error;
	dl_errno = -EFAULT;
	if (unlikely(p->dl_primitive != DL_DISCONNECT_REQ))
		goto error;
	dl_errno = DL_NOTSUPPORTED;
	if (unlikely(dl->dl_service_mode != DL_CODLS))
		goto error;
	dl_errno = DL_OUTSTATE;
	switch ((state = dl_get_state(dl))) {
	case DL_DATAXFER:
		nextstate = DL_DISCON8_PENDING;
		break;
	case DL_INCON_PENDING:
		dle.dl_correlation = dl_find_sequence(p->disconnect_req.dl_correlation);
		dl_errno = DL_BADCORR;
		if (unlikely(dle.dl_correlation == NULL))
			goto error;
		nextstate = DL_DISCON9_PENDING;
		break;
	case DL_OUTCON_PENDING:
		nextstate = DL_DISCON11_PENDING;
		break;
	case DL_PROV_RESET_PENDING:
		nextstate = DL_DISCON12_PENDING;
		break;
	case DL_USER_RESET_PENDING:
		nextstate = DL_DISCON13_PENDING;
		break;
	default:
		goto error;
	}
	dl_errno = DL_UNSUPPORTED;
	switch ((dle.dl_reason = p->disconnect_req.dl_reason)) {
	case DL_DISC_NORMAL_CONDITION:
	case DL_DISC_ABNORMAL_CONDITION:
		if (state == DL_INCON_PENDING)
			goto error;
		break;
	case DL_CONREJ_PERMANENT_COND:
	case DL_CONREJ_TRANSIENT_COND:
		if (state != DL_INCON_PENDING)
			goto error;
		break;
	case DL_DISC_UNSPECIFIED:
		break;
	default:
		goto error;
	}
	dl_set_state(dl, nextstate);
	/* provider specific disconnect or connection rejection */
	if (unlikely((dl_errno = dl->dl_event(q, dl, &dle)) != 0))
		goto error;
	if (dle.dl_correlation != NULL) {
		bufq_dequeue(&dl->dl_conq, dle.dl_correlation);
		freemsg(dle.dl_correlation);
		dl->dl_coninds--;
		if (bufq_length(&dl->dl_conq) > 0)
			dl_set_state(dl, DL_INCON_PENDING);
		else
			dl_set_state(dl, DL_IDLE);
	} else
		dl_set_state(dl, DL_IDLE);
      error:
	return dl_reply_ack(q, dl, DL_DISCONNECT_REQ, dl_errno, mp);
}

STATIC INLINE fastcall __hot_put int
dl_reset_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	union DL_primitives *p = (typeof(p)) mp->b_rptr;
	struct dle_reset_req dle = { DLE_RESET_REQ, };
	dl_long dl_errno;

	dl_errno = -EMSGSIZE;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(p->reset_req)))
		goto error;
	dl_errno = -EFAULT;
	if (unlikely(p->dl_primitive != DL_RESET_REQ))
		goto error;
	dl_errno = DL_NOTSUPPORTED;
	if (unlikely(dl->dl_service_mode != DL_CODLS))
		goto error;
	dl_errno = DL_OUTSTATE;
	if (unlikely(dl_get_state(dl) != DL_DATAXFER))
		goto error;
	dlpi_set_state(dl, DL_USER_RESET_PENDING);
	if (unlikely((dl_errno = dl->dl_event(q, dl, &dle)) != 0))
		goto error;
	return (QR_DONE);
      error:
	return dl_reply_ack(q, dl, DL_RESET_REQ, dl_errno, mp);
}

STATIC INLINE fastcall __hot_put int
dl_reset_res(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	union DL_primitives *p = (typeof(p)) mp->b_rptr;
	struct dle_reset_res dle = { DLE_RESET_RES, };
	dl_long dl_errno;

	dl_errno = -EMSGSIZE;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(p->reset_res)))
		goto error;
	dl_errno = -EFAULT;
	if (unlikely(p->dl_primitive != DL_RESET_RES))
		goto error;
	dl_errno = DL_NOTSUPPORTED;
	if (unlikely(dl->dl_service_mode != DL_CODLS))
		goto error;
	dl_errno = DL_OUTSTATE;
	if (unlikely(dl_get_state(dl) != DL_PROV_RESET_PENDING))
		goto error;
	dlpi_set_state(dl, DL_RESET_RES_PENDING);
	if (unlikely((dl_errno = dl->dl_event(q, dl, &dle)) != 0))
		goto error;
	dl_set_state(dl, DL_DATAXFER);
      error:
	return dl_reply_ack(q, dl, DL_RESET_RES, dl_errno, mp);
}

STATIC INLINE fastcall __hot_put int
dl_data_ack_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	union DL_primitives *p = (typeof(p)) mp->b_rptr;
	struct dle_data_ack_req dle = { DLE_DATA_ACK_REQ, };
	dl_long dl_errno;

	dl_errno = -EMSGSIZE;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(p->data_ack_req)))
		goto error;
	dl_errno = -EFAULT;
	if (unlikely(p->dl_primitive != DL_DATA_ACK_REQ))
		goto error;
	dl_errno = DL_NOTSUPPORTED;
	if (unlikely(dl->dl_service_mode != DL_ACLDLS))
		goto error;
	dl_errno = DL_OUTSTATE;
	if (unlikely(dl_get_state(dl) != DL_IDLE))
		goto error;
	dle.dl_dest_addr_length = p->data_ack_req.dl_dest_addr_length;
	dl_errno = DL_NOADDR;
	if (unlikely(dle.dl_dest_addr_length == 0))
		goto error;
	dle.dl_dest_addr_buffer = mp->b_rptr + p->data_ack_req.dl_dest_addr_offset;
	dl_errno = DL_BADADDR;
	if (unlikely(mp->b_wptr < dle.dl_dest_addr_buffer + dle.dl_dest_addr_length))
		goto error;
	dle.dl_src_addr_length = p->data_ack_req.dl_src_addr_length;
	dl_errno = DL_NOADDR;
	if (unlikely(dle.dl_src_addr_length == 0))
		goto error;
	dle.dl_src_addr_buffer = mp->b_rptr + p->data_ack_req.dl_src_addr_offset;
	dl_errno = DL_BADADDR;
	if (unlikely(mp->b_wptr < dle.dl_src_addr_buffer + dle.dl_src_addr_length))
		goto error;
	dl_errno = DL_BADDATA;
	if (unlikely((dle.dl_data_blocks = mp->b_cont) == NULL))
		goto error;
	dle.dl_data_length = msgsize(dle.dl_data_blocks);
	dl_errno = DL_BADDATA;
	if (unlikely(dle.dl_data_length < dl->dl_info.dl_min_sdu
		     || dle.dle_data_length > dl->dl_info.dl_max_sdu))
		goto error;
	dle.dl_correlation = p->data_ack_req.dl_correlation;
	dle.dl_priority = p->data_ack_req.dl_priority;
	dle.dl_service_class = p->data_ack_req.dl_service_class;
	if (unlikely((dl_errno = dl->dl_event(q, dl, &dle)) != 0))
		goto error;
	return (QR_TRIMMED);
      error:
	return dl_reply_ack(q, dl, DL_DATA_ACK_REQ, dl_errno, mp);
}

STATIC INLINE fastcall __hot_put int
dl_reply_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	union DL_primitives *p = (typeof(p)) mp->b_rptr;
	struct dle_reply_req dle = { DLE_REPLY_REQ, };
	dl_long dl_errno;

	dl_errno = -EMSGSIZE;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(p->reply_req)))
		goto error;
	dl_errno = -EFAULT;
	if (unlikely(p->dl_primitive != DL_REPLY_REQ))
		goto error;
	dl_errno = DL_NOTSUPPORTED;
	if (unlikely(dl->dl_service_mode != DL_ACLDLS))
		goto error;
	dl_errno = DL_OUTSTATE;
	if (unlikely(dl_get_state(dl) != DL_IDLE))
		goto error;
	dle.dl_dest_addr_length = p->reply_req.dl_dest_addr_length;
	dl_errno = DL_NOADDR;
	if (unlikely(dle.dl_dest_addr_length == 0))
		goto error;
	dle.dl_dest_addr_buffer = mp->b_rptr + p->reply_req.dl_dest_addr_offset;
	dl_errno = DL_BADADDR;
	if (unlikely(mp->b_wptr < dle.dl_dest_addr_buffer + dle.dl_dest_addr_length))
		goto error;
	dle.dl_src_addr_length = p->reply_req.dl_src_addr_length;
	dl_errno = DL_NOADDR;
	if (unlikely(dle.dl_src_addr_length == 0))
		goto error;
	dle.dl_src_addr_buffer = mp->b_rptr + p->reply_req.dl_src_addr_offset;
	dl_errno = DL_BADADDR;
	if (unlikely(mp->b_wptr < dle.dl_src_addr_buffer + dle.dl_src_addr_length))
		goto error;
	dl_errno = DL_BADDATA;
	if (unlikely((dle.dl_data_blocks = mp->b_cont) == NULL))
		goto error;
	dle.dl_data_length = msgsize(dle.dl_data_blocks);
	dl_errno = DL_BADDATA;
	if (unlikely(dle.dl_data_length < dl->dl_info.dl_min_sdu
		     || dle.dle_data_length > dl->dl_info.dl_max_sdu))
		goto error;
	dle.dl_correlation = p->reply_req.dl_correlation;
	dle.dl_priority = p->reply_req.dl_priority;
	dle.dl_service_class = p->reply_req.dl_service_class;
	if (unlikely((dl_errno = dl->dl_event(q, dl, &dle)) != 0))
		goto error;
	return (QR_TRIMMED);
      error:
	return dl_reply_ack(q, dl, DL_REPLY_REQ, dl_errno, mp);
}

STATIC INLINE fastcall __hot_put int
dl_reply_update_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	union DL_primitives *p = (typeof(p)) mp->b_rptr;
	struct dle_reply_update_req dle = { DLE_REPLY_UPDATE_REQ, };
	dl_long dl_errno;

	dl_errno = -EMSGSIZE;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(p->reply_update_req)))
		goto error;
	dl_errno = -EFAULT;
	if (unlikely(p->dl_primitive != DL_REPLY_UPDATE_REQ))
		goto error;
	dl_errno = DL_NOTSUPPORTED;
	if (unlikely(dl->dl_service_mode != DL_ACLDLS))
		goto error;
	dl_errno = DL_OUTSTATE;
	if (unlikely(dl_get_state(dl) != DL_IDLE))
		goto error;
	dle.dl_src_addr_length = p->reply_update_req.dl_src_addr_length;
	dl_errno = DL_NOADDR;
	if (unlikely(dle.dl_src_addr_length == 0))
		goto error;
	dle.dl_src_addr_buffer = mp->b_rptr + p->reply_update_req.dl_src_addr_offset;
	dl_errno = DL_BADADDR;
	if (unlikely(mp->b_wptr < dle.dl_src_addr_buffer + dle.dl_src_addr_length))
		goto error;
	dl_errno = DL_BADDATA;
	if (unlikely((dle.dl_data_blocks = mp->b_cont) == NULL))
		goto error;
	dle.dl_data_length = msgsize(dle.dl_data_blocks);
	dl_errno = DL_BADDATA;
	if (unlikely(dle.dl_data_length < dl->dl_info.dl_min_sdu
		     || dle.dle_data_length > dl->dl_info.dl_max_sdu))
		goto error;
	dle.dl_correlation = p->reply_update_req.dl_correlation;
	if (unlikely((dl_errno = dl->dl_event(q, dl, &dle)) != 0))
		goto error;
	return (QR_TRIMMED);
      error:
	return dl_reply_ack(q, dl, DL_REPLY_UPDATE_REQ, dl_errno, mp);
}

STATIC int
dl_xid_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	union DL_primitives *p = (typeof(p)) mp->b_rptr;
	struct dle_xid_req dle = { DLE_XID_REQ, };
	dl_long dl_errno;
	dl_ulong state;

	dl_errno = -EMSGSIZE;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(p->xid_req)))
		goto error;
	dl_errno = -EFAULT;
	if (unlikely(p->dl_primitive != DL_XID_REQ))
		goto error;
	state = dl_get_state(dl);
	dl_errno = DL_OUTSTATE;
	if (unlikely(state != DL_IDLE && state != DL_DATAXFER))
		goto error;
	dle.dl_dest_addr_length = p->xid_req.dl_dest_addr_length;
	dl_errno = DL_NOADDR;
	if (dle.dl_dest_addr_length == 0) {
		if (unlikely(state != DL_DATAXFER))
			goto error;
	} else {
		dle.dl_dest_addr_buffer = mp->b_rptr + p->xid_req.dl_dest_addr_offset;
		dl_errno = DL_BADADDR;
		if (unlikely(mp->b_wptr < dle.dl_dest_addr_buffer + dle.dl_dest_addr_length))
			goto error;
	}
	dl_errno = DL_BADDATA;
	if (unlikely((dle.dl_data_blocks = mp->b_cont) == NULL))
		goto error;
	dle.dl_data_length = msgsize(dle.dl_data_blocks);
	dl_errno = DL_BADDATA;
	if (unlikely(dle.dl_data_length < dl->dl_info.dl_min_sdu
		     || dle.dle_data_length > dl->dl_info.dl_max_sdu))
		goto error;
	dle.dl_flag = p->xid_req.dl_flag;
	if (unlikely((dl_errno = dl->dl_event(q, dl, &dle)) != 0))
		goto error;
	return (QR_TRIMMED);
      error:
	return dl_reply_ack(q, dl, DL_XID_REQ, dl_errno, mp);
}

STATIC int
dl_xid_res(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	union DL_primitives *p = (typeof(p)) mp->b_rptr;
	struct dle_xid_res dle = { DLE_XID_RES, };
	dl_long dl_errno;
	dl_ulong state;

	dl_errno = -EMSGSIZE;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(p->xid_res)))
		goto error;
	dl_errno = -EFAULT;
	if (unlikely(p->dl_primitive != DL_XID_RES))
		goto error;
	state = dl_get_state(dl);
	dl_errno = DL_OUTSTATE;
	if (unlikely(state != DL_IDLE && state != DL_DATAXFER))
		goto error;
	dle.dl_dest_addr_length = p->xid_res.dl_dest_addr_length;
	dl_errno = DL_XIDAUTO;
	if (dl->dl_xidtest_flg & DL_AUTO_XID)
		goto error;
	dl_errno = DL_NOADDR;
	if (dle.dl_dest_addr_length == 0) {
		if (unlikely(state != DL_DATAXFER))
			goto error;
	} else {
		dle.dl_dest_addr_buffer = mp->b_rptr + p->xid_res.dl_dest_addr_offset;
		dl_errno = DL_BADADDR;
		if (unlikely(mp->b_wptr < dle.dl_dest_addr_buffer + dle.dl_dest_addr_length))
			goto error;
	}
	dl_errno = DL_BADDATA;
	if (unlikely((dle.dl_data_blocks = mp->b_cont) == NULL))
		goto error;
	dle.dl_data_length = msgsize(dle.dl_data_blocks);
	dl_errno = DL_BADDATA;
	if (unlikely(dle.dl_data_length < dl->dl_info.dl_min_sdu
		     || dle.dle_data_length > dl->dl_info.dl_max_sdu))
		goto error;
	dle.dl_flag = p->xid_res.dl_flag;
	if (unlikely((dl_errno = dl->dl_event(q, dl, &dle)) != 0))
		goto error;
	return (QR_TRIMMED);
      error:
	return dl_reply_ack(q, dl, DL_XID_RES, dl_errno, mp);
}

STATIC int
dl_test_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	union DL_primitives *p = (typeof(p)) mp->b_rptr;
	struct dle_test_req dle = { DLE_TEST_REQ, };
	dl_long dl_errno;
	dl_ulong state;

	dl_errno = -EMSGSIZE;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(p->test_req)))
		goto error;
	dl_errno = -EFAULT;
	if (unlikely(p->dl_primitive != DL_TEST_REQ))
		goto error;
	state = dl_get_state(dl);
	dl_errno = DL_OUTSTATE;
	if (unlikely(state != DL_IDLE && state != DL_DATAXFER))
		goto error;
	dle.dl_dest_addr_length = p->test_req.dl_dest_addr_length;
	dl_errno = DL_NOADDR;
	if (dle.dl_dest_addr_length == 0) {
		if (unlikely(state != DL_DATAXFER))
			goto error;
	} else {
		dle.dl_dest_addr_buffer = mp->b_rptr + p->test_req.dl_dest_addr_offset;
		dl_errno = DL_BADADDR;
		if (unlikely(mp->b_wptr < dle.dl_dest_addr_buffer + dle.dl_dest_addr_length))
			goto error;
	}
	dl_errno = DL_BADDATA;
	if (unlikely((dle.dl_data_blocks = mp->b_cont) == NULL))
		goto error;
	dle.dl_data_length = msgsize(dle.dl_data_blocks);
	dl_errno = DL_BADDATA;
	if (unlikely(dle.dl_data_length < dl->dl_info.dl_min_sdu
		     || dle.dle_data_length > dl->dl_info.dl_max_sdu))
		goto error;
	dle.dl_flag = p->test_req.dl_flag;
	if (unlikely((dl_errno = dl->dl_event(q, dl, &dle)) != 0))
		goto error;
	return (QR_TRIMMED);
      error:
	return dl_reply_ack(q, dl, DL_TEST_REQ, dl_errno, mp);
}

STATIC int
dl_test_res(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	union DL_primitives *p = (typeof(p)) mp->b_rptr;
	struct dle_test_res dle = { DLE_TEST_RES, };
	dl_long dl_errno;
	dl_ulong state;

	dl_errno = -EMSGSIZE;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(p->test_res)))
		goto error;
	dl_errno = -EFAULT;
	if (unlikely(p->dl_primitive != DL_TEST_RES))
		goto error;
	state = dl_get_state(dl);
	dl_errno = DL_OUTSTATE;
	if (unlikely(state != DL_IDLE && state != DL_DATAXFER))
		goto error;
	dle.dl_dest_addr_length = p->test_res.dl_dest_addr_length;
	dl_errno = DL_TESTAUTO;
	if (dl->dl_xidtest_flg & DL_AUTO_TEST)
		goto error;
	dl_errno = DL_NOADDR;
	if (dle.dl_dest_addr_length == 0) {
		if (unlikely(state != DL_DATAXFER))
			goto error;
	} else {
		dle.dl_dest_addr_buffer = mp->b_rptr + p->test_res.dl_dest_addr_offset;
		dl_errno = DL_BADADDR;
		if (unlikely(mp->b_wptr < dle.dl_dest_addr_buffer + dle.dl_dest_addr_length))
			goto error;
	}
	dl_errno = DL_BADDATA;
	if (unlikely((dle.dl_data_blocks = mp->b_cont) == NULL))
		goto error;
	dle.dl_data_length = msgsize(dle.dl_data_blocks);
	dl_errno = DL_BADDATA;
	if (unlikely(dle.dl_data_length < dl->dl_info.dl_min_sdu
		     || dle.dle_data_length > dl->dl_info.dl_max_sdu))
		goto error;
	dle.dl_flag = p->test_res.dl_flag;
	if (unlikely((dl_errno = dl->dl_event(q, dl, &dle)) != 0))
		goto error;
	return (QR_TRIMMED);
      error:
	return dl_reply_ack(q, dl, DL_TEST_RES, dl_errno, mp);
}

STATIC int
dl_phys_addr_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	union DL_primitives *p = (typeof(p)) mp->b_rptr;
	struct dle_phys_addr_req del = { DLE_PHYS_ADDR_REQ, };
	dl_long dl_errno;
	mblk_t *pp = mp;
	size_t size;

	dl_errno = -EMSGSIZE;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(p->phys_addr_req)))
		goto error;
	dl_errno = -EFAULT;
	if (unlikely(p->dl_primitive != DL_PHYS_ADDR_REQ))
		goto error;
	dl_errno = DL_OUTSTATE;
	if (unlikely(dl_get_state(dl) == DL_UNATTACHED))
		goto error;
	dl_errno = DL_UNSUPPORTED;
	if (unlikely(dl->dl_ppa == 0))
		goto error;
	dle.dl_addr_type = p->phys_addr_req.dl_addr_type;
	dl_errno = DL_UNSUPPORTED;
	if (unlikely(dle.dl_addr_type != DL_FACT_PHYS_ADDR
		     && dle.dl_addr_type != DL_CURR_PHYS_ADDR))
		goto error;
	if (unlikely((dl_errno = dl->dl_event(q, dl, &dle)) != 0))
		goto error;
	size = sizeof(p->phys_addr_ack) + dle.dl_addr_length;
	dl_errno = -ENOBUFS;
	if (unlikely(mp->b_datap->db_lim < mp->b_datap->db_base + size))
		if (unlikely((mp = ss7_allocb(q, size, BPRI_MED)) == NULL))
			goto error;
	mp->b_rptr = mp->b_wptr = mp->b_datap->db_base;
	mp->b_datap->db_type = M_PCPROTO;
	mp->b_band = 0;
	p = (typeof(p)) mp->b_rptr;
	p->phys_addr_ack.dl_primitive = DL_PHYS_ADDR_ACK;
	p->phys_addr_ack.dl_addr_length = 0;
	p->phys_addr_ack.dl_addr_offset = 0;
	mp->b_wptr += sizeof(p->phys_addr_ack);
	if (dle.dl_addr_length) {
		p->phys_addr_ack.dl_addr_length = dle.dl_addr_length;
		p->phys_addr_ack.dl_addr_offset = mp->b_wptr - mp->b_rptr;
		bcopy(dle.dl_addr_buffer, mp->b_wptr, dle.dl_addr_length);
		mp->b_wptr += dle.dl_addr_length;
	}
	qreply(q, mp);
	return (pp == mp ? QR_ABSORBED : QR_DONE);
      error:
	return dl_reply_ack(q, dl, DL_PHYS_ADDR_REQ, dl_errno, mp);
}

STATIC int
dl_set_phys_addr_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	union DL_primitives *p = (typeof(p)) mp->b_rptr;
	struct dle_set_phys_addr_req del = { DLE_SET_PHYS_ADDR_REQ, };
	dl_long dl_errno;

	dl_errno = -EMSGSIZE;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(p->set_phys_addr_req)))
		goto error;
	dl_errno = -EFAULT;
	if (unlikely(p->dl_primitive != DL_SET_PHYS_ADDR_REQ))
		goto error;
	dl_errno = DL_OUTSTATE;
	if (unlikely(dl_get_state(dl) == DL_UNATTACHED))
		goto error;
	dl_errno = DL_UNSUPPORTED;
	if (unlikely(dl->dl_ppa == 0))
		goto error;
	if (unlikely((dl_errno = dl->dl_event(q, dl, &dle)) != 0))
		goto error;
      error:
	return dl_reply_ack(q, dl, DL_SET_PHYS_ADDR_REQ, dl_errno, mp);
}

STATIC int
dl_get_statistics_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	union DL_primitives *p = (typeof(p)) mp->b_rptr;
	struct dle_get_statistics_req del = { DLE_GET_STATISTICS_REQ, };
	dl_long dl_errno;
	mblk_t *pp = mp;
	size_t size;

	dl_errno = -EMSGSIZE;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(p->get_statistics_req)))
		goto error;
	dl_errno = -EFAULT;
	if (unlikely(p->dl_primitive != DL_GET_STATISTICS_REQ))
		goto error;
	dl_errno = DL_OUTSTATE;
	if (unlikely(dl_get_state(dl) == DL_UNATTACHED))
		goto error;
	dl_errno = DL_UNSUPPORTED;
	if (unlikely(dl->dl_ppa == 0))
		goto error;
	if (unlikely((dl_errno = dl->dl_event(q, dl, &dle)) != 0))
		goto error;
	size = sizeof(p->get_statistics_ack) + dle.dl_stat_length;
	dl_errno = -ENOBUFS;
	if (unlikely(mp->b_datap->db_lim < mp->b_datap->db_base + size))
		if (unlikely((mp = ss7_allocb(q, size, BPRI_MED)) == NULL))
			goto error;
	mp->b_rptr = mp->b_wptr = mp->b_datap->db_base;
	mp->b_datap->db_type = M_PCPROTO;
	mp->b_band = 0;
	p = (typeof(p)) mp->b_rptr;
	p->get_statistics_ack.dl_primitive = DL_GET_STATISTICS_ACK;
	p->get_statistics_ack.dl_stat_length = 0;
	p->get_statistics_ack.dl_stat_offset = 0;
	mp->b_wptr += sizeof(p->get_statistics_ack);
	if (dle.dl_stat_length) {
		p->get_statistics_ack.dl_stat_length = dle.dl_stat_length;
		p->get_statistics_ack.dl_stat_offset = mp->b_wptr - mp->b_rptr;
		bcopy(dle.dl_stat_buffer, mp->b_wptr, dle.dl_stat_length);
		mp->b_wptr += dle.dl_stat_length;
	}
	qreply(q, mp);
	return (pp == mp ? QR_ABSORBED : QR_DONE);
      error:
	return dl_reply_ack(q, dl, DL_GET_STATISTICS_REQ, dl_errno, mp);
}

STATIC INLINE fastcall __hot_put int
dl_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn = -EPROTO;
	dl_long prim = 0;
	struct dl *dl = DL_PRIV(q);
	dl_long oldstate = dl->i_oldstate = dlpi_get_state(dl);

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
			return (rtn);
		case -EOPNOTSUPP:	/* primitive not supported */
			return dl_reply_ack(q, dl, prim, rtn, mp);
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
STATIC INLINE fastcall __hot_in int
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
STATIC INLINE fastcall __hot_in int
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

STATIC INLINE fastcall __hot_in int
cd_r_proto(queue_t *q, mblk_t *mp)
{
	int rtn = -EPROTO;
	cd_long prim = 0;
	struct cd *cd = CD_PRIV(q);
	cd_long oldstate = cd->i_oldstate = cdi_get_state(cd);

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

STATIC INLINE fastcall __hot_in int
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
STATIC streamscall __hot_put int
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
STATIC streamscall __hot_get int
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
STATIC streamscall __hot_out int
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
STATIC streamscall __hot_in int
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
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(dl_priv_cachep)) {
			strlog(DRV_ID, 0, LOG_ERR, SL_ERROR | SL_CONSOLE,
			       "could not destroy dl_priv_cachep");
			return (-EBUSY);
		}
#else
		kmem_cache_destroy(dl_priv_cachep);
#endif
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
dl_alloc_priv(queue_t *q, struct dl **dlp, minor_t bminor, int sflag, dev_t *devp, cred_t *crp)
{
	struct dl *dl;

	if ((dl = dl_alloc())) {
		major_t cmajor = getmajor(*devp);
		minor_t cminor = getminor(*devp);
		int index = (cminor & 0x1f);
		int unit = (cminor >> 5);
		bool style2 = (cmajor != CMAJOR_0 || index == 0 && unit == 0);

		/* dl generic members */
		dl->u.dev.cmajor = cmajor;
		dl->u.dev.cminor = cminor;
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
		dl->i_version = DL_CURRENT_VERSION;
		/* information to report */
		dl->info = *dl_info[index];
		/* actions to take */
		dl->actions = *dl_actions[index];
		/* style and state */
		if (style2) {
			dl->i_state = dl->i_oldstate = dl->info.dl_state = DL_UNATTACHED;
			dl->i_style = dl->info.dl_provider_style = DL_STYLE2;
		} else {
			dl->i_state = dl->i_oldstate = dl->info.dl_state = DL_UNBOUND;
			dl->i_style = dl->info.dl_provider_style = DL_STYLE1;
		}
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
	minor_t cminor = getminor(*devp), bminor = cminor;
	struct dl *dl, **dlp;

	if (q->q_ptr != NULL)
		return (0);	/* already open */
	if (sflag == MODOPEN || WR(q)->q_next) {
		strlog(DRV_ID, cminor, LOG_WARNING, SL_WARN | SL_CONSOLE, "cannot push as module");
		return (ENXIO);
	}
#ifdef LIS
	if (cmajor != CMAJOR_0 || (cminor & 0x1f >= DL_MINOR_FREE)) {
		strlog(DRV_ID, cminor, LOG_WARNING, SL_WARN | SL_CONSOLE,
		       "attempt to open device %d:%d directly", cmajor, cminor);
		return (ENXIO);
	}
#endif				/* LIS */
#ifdef LFS
	/* Linux Fast-STREAMS always passes internal major dvice numbers (module ids) */
	if (cmajor != DRV_ID || (cminor & 0x1f >= DL_MINOR_FREE)) {
		strlog(DRV_ID, cminor, LOG_WARNING, SL_WARN | SL_CONSOLE,
		       "attempt to open device %d:%d directly", cmajor, cminor);
		return (ENXIO);
	}
#endif				/* LFS */
	if (sflag == CLONEOPEN || (cminor & 0x1f) == 0 || (cminor & ~0x1f) == 0) {
		strlog(DRV_ID, cminor, LOG_DEBUG, SL_TRACE, "clone open in effect");
		sflag = CLONEOPEN;
		/* allocate dynamic device numbers from the second major */
		mindex = 1;
		cmajor = dl_majors[mindex];
		cminor = 0;
	}
	dlp = &master.drv.list;
	write_lock_bh(&master.lock);
	/* find position of majors in list */
	if (mindex)
		/* scan to second major block */
		for (; *dlp && (*dlp)->u.dev.cmajor != dl_majors[0]; dlp = &(*dlp)->next) ;
	for (; *dlp; dlp = &(*dlp)->next) {
		if (cmajor != (*dlp)->u.dev.cmajor)
			break;
		if (cminor < (*dlp)->u.dev.cminor)
			break;
		if (cminor == (*dlp)->u.dev.cminor) {
			if (sflag != CLONEOPEN) {
				write_unlock_bh(&master.lock);
				/* should not happen with DRVOPEN: STREAMS should know that the
				   device is already open */
				strlog(DRV_ID, 0, LOG_WARNING, SL_WARN | SL_CONSOLE,
				       "opened device %d:%d in use!", cmajor, cminor);
				return (EAGAIN);
			}
			if (getmajor(makedevice(cmajor, ++cminor)) != cmajor) {
				if (++mindex >= CMAJORS)
					break;
				if (!(cmajor = dl_majors[mindex]))
					break;
				cminor = 0;	/* start next major */
			}
		}
	}
	if (mindex >= CMAJORS || !cmajor) {
		write_unlock_bh(&master.lock);
		strlog(DRV_ID, 0, LOG_WARNING, SL_WARN | SL_CONSOLE, "no device numbers available");
		return (ENXIO);
	}
	cmn_err(CE_NOTE, "%s: opened character device %d:%d", DRV_NAME, cmajor, cminor);
	*devp = makedevice(cmajor, cminor);
	if (!(dl = dl_alloc_priv(q, dlp, bminor, sflag, devp, crp))) {
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
module_param(modid, ushort, 0444);
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID number for DL driver (0-for allocation).");

major_t major = CMAJOR_0;

#ifndef module_param
MODULE_PARM(major, "h");
#else				/* module_param */
module_param(major, uint, 0444);
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
	if ((err = lis_register_driver_qlock_option(major, LIS_QLOCK_NONE)) < 0) {
		lis_unregister_strdev(major);
		strlog(DRV_ID, 0, LOG_WARNING, SL_WARN | SL_CONSOLE,
		       "cannot register major %d", major);
		dl_initialized = err;
		return;
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
