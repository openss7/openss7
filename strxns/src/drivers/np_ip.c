/*****************************************************************************

 @(#) $RCSfile: np_ip.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2006/04/18 18:00:45 $

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

 Last Modified $Date: 2006/04/18 18:00:45 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: np_ip.c,v $
 Revision 0.9.2.2  2006/04/18 18:00:45  brian
 - working up DL and NP drivers

 Revision 0.9.2.1  2006/04/13 18:32:50  brian
 - working up DL and NP drivers.

 *****************************************************************************/

#ident "@(#) $RCSfile: np_ip.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2006/04/18 18:00:45 $"

static char const ident[] =
    "$RCSfile: np_ip.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2006/04/18 18:00:45 $";

/*
 *  This is a NPI connectionless driver for the IP subsystem.  The purpose of this driver is to
 *  directly access the layer beneath IP for the purposes of network interface taps and testing
 *  (primarily for testing SCTP and RTP).  It is effectively equivalent to a raw socket, except
 *  that it has the ability to intercept IP packets.
 */

#include <sys/os7/compat.h>

#ifdef LINUX
#undef ASSERT

#include <linux/bitops.h>

#define n_tst_bit(nr,addr)	test_bit(nr,addr)
#define n_set_bit(nr,addr)	__set_bit(nr,addr)
#define n_clr_bit(nr,addr)	__clear_bit(nr,addr)

#include <linux/interrupt.h>

#include <sys/npi.h>

#define NP_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define NP_EXTRA	"Part of the OpenSS7 stack for Linux Fast-STREAMS"
#define NP_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define NP_REVISION	"OpenSS7 $RCSfile: np_ip.c,v $ $Name:  $ ($Revision: 0.9.2.2 $) $Date: 2006/04/18 18:00:45 $"
#define NP_DEVICE	"SVR 4.2 STREAMS NPI NP_IP Data Link Provider"
#define NP_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define NP_LICENSE	"GPL"
#define NP_BANNER	NP_DESCRIP	"\n" \
			NP_EXTRA	"\n" \
			NP_COPYRIGHT	"\n" \
			NP_DEVICE	"\n" \
			NP_CONTACT
#define NP_SPLASH	NP_DESCRIP	"\n" \
			NP_REVISION

#ifdef LINUX
MODULE_AUTHOR(NP_CONTACT);
MODULE_DESCRIPTION(NP_DESCRIP);
MODULE_SUPPORTED_DEVICE(NP_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(NP_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-np_ip");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifdef LFS
#define NP_IP_DRV_ID	CONFIG_STREAMS_NP_IP_MODID
#define NP_IP_DRV_NAME	CONFIG_STREAMS_NP_IP_NAME
#define NP_IP_CMAJORS	CONFIG_STREAMS_NP_IP_NMAJOR
#define NP_IP_CMAJOR_0	CONFIG_STREAMS_NP_IP_MAJOR
#define NP_IP_UNITS	CONFIG_STREAMS_NP_IP_NMINORS
#endif				/* LFS */

#ifdef LINUX
#ifdef MODULE_ALIAS
#ifdef LFS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_NP_MODID));
MODULE_ALIAS("streams-driver-np_ip");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_NP_MAJOR));
MODULE_ALIAS("/dev/streams/np_ip");
MODULE_ALIAS("/dev/streams/np_ip/*");
MODULE_ALIAS("/dev/streams/clone/np_ip");
#endif				/* LFS */
MODULE_ALIAS("char-major-" __stringify(NP_IP_CMAJOR_0));
MODULE_ALIAS("char-major-" __stringify(NP_IP_CMAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(NP_IP_CMAJOR_0) "-0");
MODULE_ALIAS("char-major-" __stringify(NP_IP_CMAJOR_0) "-" __stringify(NP_IP_CMINOR));
MODULE_ALIAS("/dev/np_ip");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

/*
 *  STREAMS Definitions
 *  ===================
 */

#define DRV_ID		NP_IP_DRV_ID
#define CMAJORS		NP_IP_CMAJORS
#define CMAJOR_0	NP_IP_CMAJOR_0
#define UNITS		NP_IP_UNITS
#ifdef MODULE
#define DRV_BANNER	NP_IP_BANNER
#else				/* MODULE */
#define DRV_BANNER	NP_IP_SPLASH
#endif				/* MODULE */

STATIC struct module_info np_minfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME,		/* Module name */
	.mi_minpsz = 0,			/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,		/* Max packet size acceptd */
	.mi_hiwat = 1 << 15,		/* Hi water mark */
	.mi_hiwat = 1 << 10,		/* Lo water mark */
};

STATIC struct module_stat np_mstat = {
};

/* Upper multiplex is a N provider following the NPI. */

STATIC streamscall int np_qopen(queue_t *, dev_t *, int, int, cred_t *);
STATIC streamscall int np_qclose(queue_t *, int, cred_t *);

STATIC struct qinit np_rinit = {
	.qi_putp = &ss7_oput,		/* Read put procedure (message from below) */
	.qi_srvp = &ss7_osrv,		/* Read service procedure */
	.qi_qopen = &np_qopen,		/* Each open */
	.qi_qclose = &np_qclose,	/* Last close */
	.qi_minfo = &np_minfo,		/* Module information */
	.qi_mstat = &np_mstat,		/* Module statistics */
};

STATIC struct qinit np_winit = {
	.qi_putp = &ss7_iput,		/* Read put procedure (message from below) */
	.qi_srvp = &ss7_isrv,		/* Read service procedure */
	.qi_minfo = &np_minfo,		/* Module information */
	.qi_mstat = &np_mstat,		/* Module statistics */
};

STATIC struct streamtab np_info = {
	.st_rdinit = &np_rinit,		/* Upper read queue */
	.st_wrinit = &np_winit,		/* Upper write queue */
};

/*
 *  Addressing:
 *
 *  NSAPs (Protocol IDs) are IP protocol numbers.  NSAP addresses consist of a port number and a
 *  list of IP addreses.  If the port number is zero, any port number is used.  Initially it is only
 *  supporting IPv4.
 *
 *  There are two types of providers: connectionless and connection oriented.
 *
 *  - Connectionless providers will start delivering packets after the bind.
 *
 *  - When the NS provider is bound to multiple protocol ids, or bound or connected to multiple
 *    addresses, data will be delivered as N_DATA_IND primitives that contain the protocol id index,
 *    destination address index, and source addresses index in the DATA_xfer_flags as the highest
 *    order byte, next highest order byte and so on.  An index of 0 indicates the first bound
 *    protocol id, source address or destination address.  In this way, the high order 3 bytes of
 *    the DATA_xfer_flags are coded all zeros in the non-multiple case.
 *
 *    The NS user is also permitted to send N_DATA_REQ primitives that contain the protocol id,
 *    destination address, and source addresses, similarly encoded in the DATA_xfer_flags.  Invalid
 *    indexes will error the stream.
 *
 *  - Connection oriented provider bound as listening will start delivering packets as connection
 *    indications after the bind.  The user can either reject these with a disconnect request or can
 *    accept them with a connection response, with a slight difference from normal NPI: the
 *    responding address in the connection response is the list of peer addresses to which to
 *    establish a connection connect rather than the local responding address.
 *
 *    If the connection is accepted on the listening stream, no further connection indications will
 *    be delivered.  If accepted on another stream, further connection indications (belonging to
 *    annother association) will be delivered.
 *
 *  - Connection oriented providers bound as non-listening will deliver packets only after a
 *    successful connection establishment operation.  Connection establishment does not doe anything
 *    except to provide a port and list of addresses from which the provider is to deliver packets.
 *    This can be useful for RTP and for establishing endpoint communication with SCTP.
 *
 *  - Before bind, the provider will report both N_CLNS and N_CONS.  When bound with bind flags
 *    equal to N_CLNS in the second least significant BIND_flags byte, the provider will be
 *    connectionless.  When bound with bind flags equal to N_CONS in the second least significant
 *    BIND_flags byte, the provider will be connection-oriented.
 */

/*
 *  NS PROVIDER PRIMITIVES SENT UPSTREAM
 *  ====================================
 */

STATIC INLINE int
n_conn_ind(queue_t *q, uchar *dest, size_t dlen, uchar *src, size_t slen, np_ulong SEQ_number,
	   np_ulong CONN_flags, uchar *qos, size_t qlen, mblk_t *dp)
{
	N_conn_ind_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p) + dlen + slen + qlen, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_CONN_IND;
		p->DEST_length = dlen;
		p->DEST_offset = dlen ? sizeof(*p) : 0;
		p->SRC_length = slen;
		p->SRC_offset = slen ? sizeof(*p) + dlen : 0;
		p->SEQ_number = SEQ_number;
		p->CONN_flags = CONN_flags;
		p->QOS_length = qlen;
		p->QOS_offset = qlen ? sizeof(*p) + qlen : 0;
		mp->b_wptr += sizeof(*p);
		if (dlen) {
			bcopy(dest, mp->b_wptr, dlen);
			mp->b_wptr += dlen;
		}
		if (slen) {
			bcopy(src, mp->b_wptr, slen);
			mp->b_wptr += slen;
		}
		if (qlen) {
			bcopy(qos, mp->b_wptr, qlen);
			mp->b_wptr += qlen;
		}
		mp->b_cont = dp;
		putnext(RD(q), mp);
		return (QR_ABSORBED);
	}
	return (-ENOBUFS);
}
STATIC INLINE int
n_conn_con(queue_t *q, uchar *res, size_t rlen, np_ulong CONN_flags, uchar *qos, size_t qlen,
	   mblk_t *dp)
{
	N_conn_con_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p) + rlen + qlen, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_CONN_CON;
		p->RES_length = rlen;
		p->RES_offset = rlen ? sizeof(*p) : 0;
		p->CONN_flags = CONN_flags;
		p->QOS_length = qlen;
		p->QOS_offset = qlen ? sizeof(*p) + rlen : 0;
		mp->b_wptr += sizeof(*p);
		if (rlen) {
			bcopy(res, mp->b_wptr, rlen);
			mp->b_wptr += rlen;
		}
		mp->b_cont = dp;
		putnext(RD(q), mp);
		return (QR_ABSORBED);
	}
	return (-ENOBUFS);
}
STATIC INLINE int
n_discon_ind(queue_t *q, np_ulong DISCON_orig, np_ulong DISCON_reason, uchar *res, size_t rlen,
	     np_ulong SEQ_number, mblk_t *dp)
{
	N_discon_ind_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p) + rlen, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_DISCON_IND;
		p->DISCON_orig = DISCON_orig;
		p->DISCON_reason = DISCON_reason;
		p->RES_length = rlen;
		p->RES_offset = rlen ? sizeof(*p) : 0;
		p->SEQ_number = SEQ_number;
		mp->b_wptr += sizeof(*p);
		if (rlen) {
			bcopy(res, mp->b_wptr, rlen);
			mp->b_wptr += rlen;
		}
		mp->b_cont = dp;
		putnext(RD(q), mp);
		return (QR_ABSORBED);
	}
	return (-ENOBUFS);
}
STATIC INLINE int
n_data_ind(queue_t *q, int pindex, int dindex, int sindex, np_ulong DATA_xfer_flags, mblk_t *dp)
{
	N_data_ind_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_DATA_IND;
		p->DATA_xfer_flags =
		    (pindex << 24) | (dindex << 16) | (sindex << 8) | DATA_xfer_flags;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = dp;
		putnext(RD(q), mp);
		return (QR_ABSORBED);
	}
	return (-ENOBUFS);
}
STATIC INLINE int
n_exdata_ind(queue_t *q, mblk_t *dp)
{
	N_exdata_ind_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_EXDATA_IND;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = dp;
		putnext(RD(q), mp);
		return (QR_ABSORBED);
	}
	return (-ENOBUFS);
}
STATIC INLINE int
n_info_ack(queue_t *q)
{
	struct np *np = NP_PRIV(q);
	N_info_ack_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		*p = np->info;
		p->PRIM_type = N_INFO_ACK;
		mp->b_wptr += sizeof(*p);
		putnext(RD(q), mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}
STATIC INLINE int
n_bind_ack(queue_t *q, uchar *add, size_t alen, np_ulong CONIND_number, np_ulong TOKEN_value,
	   uchar *pid, size_t plen)
{
	N_bind_ack_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p) + alen + plen, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_BIND_ACK;
		p->ADDR_length = alen;
		p->ADDR_offset = alen ? sizeof(*p) : 0;
		p->CONIND_number = CONIND_number;
		p->TOKEN_value = TOKEN_value;
		p->PROTOID_length = plen;
		p->PROTOID_offset = plen ? sizeof(*p) + alen : 0;
		mp->b_wptr += sizeof(*p);
		if (alen) {
			bcopy(add, mp->b_wptr, alen);
			mp->b_wptr += alen;
		}
		if (plen) {
			bcopy(pid, mp->b_wptr, plen);
			mp->b_wptr += plen;
		}
		putnext(RD(q), mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}
STATIC INLINE int
n_error_ack(queue_t *q, np_ulong ERROR_prim, np_long error)
{
	N_error_ack_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_ERROR_ACK;
		p->ERROR_prim = ERROR_prim;
		p->NPI_error = error < 0 ? NSYSERR : 0;
		p->UNIX_error = error < 0 ? -error : 0;
		mp->b_wptr += sizeof(*p);
		putnext(RD(q), mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}
STATIC INLINE int
n_ok_ack(queue_t *q, np_ulong CORRECT_prim)
{
	N_ok_ack_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_OK_ACK;
		p->CORRECT_prim = CORRECT_prim;
		mp->b_wptr += sizeof(*p);
		putnext(RD(q), mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}
STATIC INLINE int
n_unitdata_ind(queue_t *q, uchar *src, size_t slen, uchar *dest, size_t dlen, np_ulong ERROR_type,
	       mblk_t *dp)
{
	N_unitdata_ind_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p) + slen + dlen, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_UNITDATA_IND;
		p->SRC_length = slen;
		p->SRC_offset = slen ? sizeof(*p) : 0;
		p->DEST_length = dlen;
		p->DEST_offset = dlen ? sizeof(*p) + slen : 0;
		p->ERROR_type = ERROR_type;
		mp->b_wptr += sizeof(*p);
		if (slen) {
			bcopy(src, mp->b_wptr, slen);
			mp->b_wptr += slen;
		}
		if (dlen) {
			bcopy(dest, mp->b_wptr, dlen);
			mp->b_wptr += dlen;
		}
		mp->b_cont = dp;
		putnext(RD(q), mp);
		return (QR_ABSORBED);
	}
	return (-ENOBUFS);
}
STATIC INLINE int
n_uderror_ind(queue_t *q, uchar *dest, size_t dlen, np_ulong RESERVED_field, np_ulong ERROR_type,
	      mblk_t *dp)
{
	N_uderror_ind_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p) + dlen, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_UDERROR_IND;
		p->DEST_length = dlen;
		p->DEST_offset = dlen ? sizeof(*p) : 0;
		p->RESERVED_field = RESERVED_field;
		p->ERROR_type = ERROR_type;
		mp->b_wptr += sizeof(*p);
		if (dlen) {
			bcopy(dest, mp->b_wptr, dlen);
			mp->b_wptr += dlen;
		}
		mp->b_cont = dp;
		putnext(RD(q), mp);
		return (QR_ABSORBED);
	}
	return (-ENOBUFS);
}
STATIC INLINE int
n_datack_ind(queue_t *q)
{
	N_datack_ind_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_DATACK_IND;
		mp->b_wptr += sizeof(*p);
		putnext(RD(q), mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}
STATIC INLINE int
n_reset_ind(queue_t *q, np_ulong RESET_orig, np_ulong RESET_reason)
{
	N_reset_ind_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_RESET_IND;
		p->RESET_orig = RESET_orig;
		p->RESET_reason = RESET_reason;
		mp->b_wptr += sizeof(*p);
		putnext(RD(q), mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}
STATIC INLINE int
n_reset_con(queue_t *q)
{
	N_reset_con_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_RESET_CON;
		mp->b_wptr += sizeof(*p);
		putnext(RD(q), mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  NS USER PRIMITIVES SENT DOWNSTREAM
 *  ==================================
 */

STATIC INLINE int
n_conn_req(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	N_conn_req_t *p;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + p->DEST_offset = p->DEST_length)
		goto badaddr;
	if (mp->b_wptr < mp->b_rptr + p->QOS_offset + p->QOS_length)
		goto badopt;
	if (p->DEST_offset == 0)
		goto noaddr;
	if (mp->b_cont != NULL && msgsize(mp->b_cont) > np->info.CDATA_size)
		goto baddata;
      baddata:
	err = NBADDATA;
	goto error;
      noaddr:
	err = NNOADDR;
	goto error;
      badopt:
	err = NBADOPT;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return n_error_ack(q, *(np_long *) mp->b_rptr, err);
}


/**
 * n_conn_res - accept a specified NS connection indication on the requested stream
 * @q: write queue
 * @mp: N_CONN_RES message
 *
 * There is a slight variation on normal NPI Revision 2.0.0 semantics here: the Responding Address
 * is, here, the remote address(es) to which the connection is to be formed, rather than the
 * local address(es) with which the connection is established.  This is only necessary, however, for
 * multihomed operation, where the NS user is the only one aware of the other remote addresses from
 * the message.
 */
STATIC INLINE int
n_conn_res(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	N_conn_res_t *p;
	int err;
	struct np *ap;
	mblk_t *cp;
	N_qos_sel_conn_ip_t qos = { N_QOS_SEL_CONN_IP, 64, 0, 576, };

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + p->RES_offset + p->RES_length)
		goto badaddr;
	if (mp->b_wptr < mp->b_rptr + p->QOS_offset + p->QOS_length)
		goto badopt;
	if (mp->b_cont != NULL && msgsize(mp->b_cont) > np->info.CDATA_size)
		goto baddata;
	if (p->TOKEN_value != 0 && (ap = np_find_token(p->TOKEN_value)) == NULL)
		goto badtoken;
	if ((cp = cp_find_sequence(p->SEQ_number)) == NULL)
		goto badseq;
	if (p->RES_length != 0) {
	}
	if (p->QOS_length != 0) {
		if (p->QOS_length != sizeof(N_qos_sel_conn_ip_t))
			goto badqostype;
		bcopy(mp->b_rptr + p->QOS_offset, &qos, p->QOS_length);
		if (qos.n_qos_type != N_QOS_SEL_CONN_IP)
			goto badqostype;
		if (qos.ttl > 255)
			goto badqosparam;
	}
      badqosparam:
	err = NBADQOSPARAM;
	goto error;
      badqostype:
	err = NBADQOSTYPE;
	goto error;
      badseq:
	err = NBADSEQ;
	goto error;
      badtoken:
	err = NBADTOKEN;
      baddata:
	err = NBADDATA;
	goto error;
      badopt:
	err = NBADOPT;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return n_error_ack(q, *(np_long *) mp->b_rptr, err);
}
STATIC INLINE int
n_discon_req(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	N_discon_req_t *p;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + p->RES_offset + p->RES_length)
		goto badaddr;
      badaddr:
	err = NBADADDR;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return n_error_ack(q, *(np_long *) mp->b_rptr, err);
}
STATIC INLINE int
n_data_req(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	N_data_req_t *p;
	size_t size;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	p = (typeof(p)) mp->b_rptr;
	if (!mp->b_cont)
		goto baddata;
	size = msgsize(mp->b_cont);
	if (size == 0 || size > np->info.NIDU_size)
		goto baddata;
      baddata:
	err = NBADDATA;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return n_error_ind(q, *(np_long *) mp->b_rptr, err);
}
STATIC INLINE int
n_exdata_req(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	N_exdata_req_t *p;
	size_t size;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	p = (typeof(p)) mp->b_rptr;
	if (!mp->b_cont)
		goto baddata;
	size = msgsize(mp->b_cont);
	if (size == 0 || size > np->info.NIDU_size)
		goto baddata;
      baddata:
	err = NBADDATA;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return n_error_ack(q, *(np_long *) mp->b_rptr, err);
}
STATIC INLINE int
n_info_req(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	N_info_req_t *p;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	p = (typeof(p)) mp->b_rptr;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return n_error_ack(q, *(np_long *) mp->b_rptr, err);
}

/**
 * n_bind_req - bind to a NS address
 * @q: write queue
 * @mp: the N_BIND_REQ message
 *
 * Because the NS provider can be either N_CONS or N_CLNS, as a feature we recognize N_CONS or
 * N_CLNS set in the second least significant byte of the BIND_flags member.  When not specified,
 * the service class defaults to a default based on which character device was opened.
 */
STATIC INLINE int
n_bind_req(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	N_bind_req_t *p;
	int np_class;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + p->ADDR_offset + p->ADDR_length)
		goto badaddr;
	if (mp->b_wptr < mp->b_rptr + p->PROTOID_offset + p->PROTOID_length)
		goto badproto;
	if (p->ADDR_length == 0)
		goto noaddr;
	if (p->PROTOID_length == 0)
		goto noprotoid;
	switch ((np_class = ((p->BIND_flags >> 8) & 0xff))) {
	case N_CONS:
	case N_CLNS:
		break;
	case 0:		/* use default setting */
		np_class = np->np_class;
		break;
	default:
		goto badflag;
	}
      badflag:
	err = NBADFLAG;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      badproto:
	err = NBADADDR;
	goto error;
      noaddr:
	err = NNOADDR;
	goto error;
      noprotoid:
	err = NNOPROTOID;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return n_error_ack(q, *(np_long *) mp->b_rptr, err);
}
STATIC INLINE int
n_unbind_req(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	N_unbind_req_t *p;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	p = (typeof(p)) mp->b_rptr;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return n_error_ack(q, *(np_long *) mp->b_rptr, err);
}

/**
 * n_unitdata_req - send unit data
 * @q: write queue
 * @mp: N_UNITDATA_REQ message
 *
 * A slight difference to normal NPI as a feature: the first RESERVED_field is used to indicate the
 * index of the protocol id from/for which to send.  The second RESERVED_field is used to indicate
 * the index of the source address from which to send.  When bound to a single protocol id or source
 * address, the corresponding index is zero (0).
 */
STATIC INLINE int
n_unitdata_req(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	N_unitdata_req_t *p;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	p = (typeof(p)) mp->b_rptr;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return n_error_ack(q, *(np_long *) mp->b_rptr, err);
}

/**
 * n_optmgmt_req - options management request
 * @q: write queue
 * @mp: the N_OPTMGMT_REQ message
 *
 * It might be an idea to define some provider-specific flags.  Some things could be performing or
 * not performing SAR, cooked and uncooked headers, and the like.  Another would be promiscuous mode
 * (processing packets with packet type == PACKET_OTHERHOST).
 */
STATIC INLINE int
n_optmgmt_req(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	N_optmgmt_req_t *p;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	p = (typeof(p)) mp->b_rptr;
	if (mp->b_wtpr < mp->b_rptr + p->QOS_offset + p->QOS_length)
		goto badqos;
      badqos:
	err = NBADOPT;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return n_error_ack(q, *(np_long *) mp->b_rptr, err);
}
STATIC INLINE int
n_datack_req(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	N_datack_req_t *p;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	p = (typeof(p)) mp->b_rptr;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return n_error_ack(q, *(np_long *) mp->b_rptr, err);
}
STATIC INLINE int
n_reset_req(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	N_reset_req_t *p;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	p = (typeof(p)) mp->b_rptr;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return n_error_ack(q, *(np_long *) mp->b_rptr, err);
}
STATIC INLINE int
n_reset_res(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	N_reset_res_t *p;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	p = (typeof(p)) mp->b_rptr;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return n_error_ack(q, *(np_long *) mp->b_rptr, err);
}

/**
 * np_w_proto - process M_PROTO/M_PCPROTO messages on the write queue
 * @q: write queue
 * @mp: the M_PROTO/M_PCPROTO message
 */
STATIC INLINE fastcall __hot_put int
np_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn = -EPROTO;
	np_long prim = 0;
	struct np *np = DL_PRIV(q);
	np_long oldstate = npi_get_state(np);

	if (mp->b_wptr >= mp->b_rptr + sizeof(prim)) {
		switch ((prim = *(np_long *) mp->b_rptr)) {
		case N_CONN_REQ:	/* NC request */
			rtn = n_conn_req(q, mp);
			break;
		case N_CONN_RES:	/* Accept previous connection indication */
			rtn = n_conn_res(q, mp);
			break;
		case N_DISCON_REQ:	/* NC disconnection request */
			rtn = n_discon_req(q, mp);
			break;
		case N_DATA_REQ:	/* Connection-Mode data transfer request */
			rtn = n_data_req(q, mp);
			break;
		case N_EXDATA_REQ:	/* Expedited data request */
			rtn = n_exdata_req(q, mp);
			break;
		case N_INFO_REQ:	/* Information Request */
			rtn = n_info_req(q, mp);
			break;
		case N_BIND_REQ:	/* Bind a NS user to network address */
			rtn = n_bind_req(q, mp);
			break;
		case N_UNBIND_REQ:	/* Unbind NS user from network address */
			rtn = n_unbind_req(q, mp);
			break;
		case N_UNITDATA_REQ:	/* Connection-less data send request */
			rtn = n_unitdata_req(q, mp);
			break;
		case N_OPTMGMT_REQ:	/* Options Management request */
			rtn = n_optmgmt_req(q, mp);
			break;
		case N_DATACK_REQ:	/* Data acknowledgement request */
			rtn = n_datack_req(q, mp);
			break;
		case N_RESET_REQ:	/* NC reset request */
			rtn = n_reset_req(q, mp);
			break;
		case N_RESET_RES:	/* Reset processing accepted */
			rtn = n_reset_res(q, mp);
			break;

		case N_CONN_IND:	/* Incoming connection indication */
		case N_CONN_CON:	/* Connection established */
		case N_DISCON_IND:	/* NC disconnected */
		case N_DATA_IND:	/* Incoming connection-mode data indication */
		case N_EXDATA_IND:	/* Incoming expedited data indication */
		case N_INFO_ACK:	/* Information Acknowledgement */
		case N_BIND_ACK:	/* NS User bound to network address */
		case N_ERROR_ACK:	/* Error Acknowledgement */
		case N_OK_ACK:	/* Success Acknowledgement */
		case N_UNITDATA_IND:	/* Connection-less data receive indication */
		case N_UDERROR_IND:	/* UNITDATA Error Indication */
		case N_DATACK_IND:	/* Data acknowledgement indication */
		case N_RESET_IND:	/* Incoming NC reset request indication */
		case N_RESET_CON:	/* Reset processing complete */
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
		npi_set_state(np, oldstate);
		/* The put and service procedure do not recognize all errors. Sometimes we return
		   an error to here just to restore the previous state. */
		switch (rtn) {
		case -EBUSY:	/* flow controlled */
		case -EAGAIN:	/* try again */
		case -ENOMEM:	/* could not allocate memory */
		case -ENOBUFS:	/* could not allocate an mblk */
		case -EOPNOTSUPP:	/* primitive not supported */
			return np_error_ack(q, prim, rtn);
		case -EPROTO:
			return np_error_reply(q, rtn);
		default:
			/* ignore all other errors */
			rtn = 0;
			break;
		}
	}
	return (rtn);
}
STATIC int
np_w_data(queue_t *q, mblk_t *mp)
{
}
STATIC int
np_w_ctl(queue_t *q, mblk_t *mp)
{
}
STATIC int
np_w_ioctl(queue_t *q, mblk_t *mp)
{
}
