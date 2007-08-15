/*****************************************************************************

 @(#) $RCSfile: sscop.c,v $ $Name:  $($Revision: 0.9.2.17 $) $Date: 2007/08/15 05:20:28 $

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

 Last Modified $Date: 2007/08/15 05:20:28 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sscop.c,v $
 Revision 0.9.2.17  2007/08/15 05:20:28  brian
 - GPLv3 updates

 Revision 0.9.2.16  2007/08/14 12:18:50  brian
 - GPLv3 header updates

 Revision 0.9.2.15  2007/07/14 01:35:11  brian
 - make license explicit, add documentation

 Revision 0.9.2.14  2007/03/25 19:00:22  brian
 - changes to support 2.6.20-1.2307.fc5 kernel

 Revision 0.9.2.13  2007/03/25 05:59:45  brian
 - flush corrections

 Revision 0.9.2.12  2007/03/25 02:23:03  brian
 - add D_MP and D_MTPERQ flags

 Revision 0.9.2.11  2007/03/25 00:52:12  brian
 - synchronization updates

 Revision 0.9.2.10  2006/03/07 01:13:01  brian
 - updated headers

 *****************************************************************************/

#ident "@(#) $RCSfile: sscop.c,v $ $Name:  $($Revision: 0.9.2.17 $) $Date: 2007/08/15 05:20:28 $"

static char const ident[] =
    "$RCSfile: sscop.c,v $ $Name:  $($Revision: 0.9.2.17 $) $Date: 2007/08/15 05:20:28 $";

/*
 *  This driver provides the functionality of SSCOP-MCE over a connectionless
 *  network provider (NPI).  It is suitable for use with an IP network
 *  provider or a UDP network provider.
 */

#include <sys/os7/compat.h>

#include <sys/npi.h>

//#include "sscop.h"
//#include "sscop_defs.h"
//#include "sscop_hash.h"
//#include "sscop_cache.h"

//#include "sscop_n.h"
//#include "sscop_t.h"
//#include "sscop_input.h"

#define SSCOP_DESCRIP	"SSCOPMCE/IP STREAMS DRIVER."
#define SSCOP_REVISION	"OpenSS7 $RCSfile: sscop.c,v $ $Name:  $($Revision: 0.9.2.17 $) $Date: 2007/08/15 05:20:28 $"
#define SSCOP_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define SSCOP_DEVICE	"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define SSCOP_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SSCOP_LICENSE	"GPL"
#define SSCOP_BANNER	SSCOP_DESCRIP	"\n" \
			SSCOP_REVISION	"\n" \
			SSCOP_COPYRIGHT	"\n" \
			SSCOP_DEVICE	"\n" \
			SSCOP_CONTACT	"\n"
#define SSCOP_SPLASH	SSCOP_DEVICE	" - " \
			SSCOP_REVISION	"\n" \

#ifdef LINUX
MODULE_AUTHOR(SSCOP_CONTACT);
MODULE_DESCRIPTION(SSCOP_DESCRIP);
MODULE_SUPPORTED_DEVICE(SSCOP_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(SSCOP_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-sscop");
#endif
#endif				/* LINUX */

#ifdef SCCOP_DEBUG
static int sscop_debug = SSCOP_DEBUG;
#else
static int sscop_debug = 2;
#endif

#ifdef LFS
#define SSCOP_DRV_ID		CONFIG_STREAMS_SSCOP_MODID
#define SSCOP_DRV_NAME		CONFIG_STREAMS_SSCOP_NAME
#define SSCOP_CMAJORS		CONFIG_STREAMS_SSCOP_NMAJORS
#define SSCOP_CMAJOR_0		CONFIG_STREAMS_SSCOP_MAJOR
#define SSCOP_NMINOR		CONFIG_STREAMS_SSCOP_NMINORS
#endif

#ifndef SSCOP_DRV_NAME
#define SSCOP_DRV_NAME		"sscop-mce"
#endif				/* SSCOP_DRV_NAME */

#ifndef SSCOP_CMAJOR_0
#define SSCOP_CMAJOR_0		240
#endif				/* SSCOP_CMAJOR_0 */

#ifndef SSCOP_NMINOR
#define SSCOP_NMINOR		255
#endif				/* SSCOP_NMINOR */

/*
 *  =========================================================================
 *
 *  STREAMS Defintions
 *
 *  =========================================================================
 */

#define DRV_ID		SSCOP_DRV_ID
#define DRV_NAME	SSCOP_DRV_NAME
#ifdef MODULE
#define DRV_BANNER	SSCOP_BANNER
#else				/* MODULE */
#define DRV_BANNER	SSCOP_SPLASH
#endif				/* MODULE */

STATIC struct module_info sscop_minfo = {
	DRV_ID,				/* Module ID number */
	DRV_NAME,			/* Module name */
	1,				/* Min packet size accepted */
	512,				/* Max packet size accepted */
	8 * 512,			/* Hi water mark */
	1 * 512				/* Lo water mark */
};

STATIC int sscop_rput(queue_t *, mblk_t *);
STATIC int sscop_rsrv(queue_t *);

STATIC int sscop_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC int sscop_close(queue_t *, int, cred_t *);

STATIC struct qinit sscop_rinit = {
	sscop_rput,			/* Read put (msg from below) */
	sscop_rsrv,			/* Read queue service */
	sscop_open,			/* Each open */
	sscop_close,			/* Last close */
	NULL,				/* Admin (not used) */
	&sscop_minfo,			/* Information */
	NULL				/* Statistics */
};

STATIC int sscop_wput(queue_t *, mblk_t *);
STATIC int sscop_wsrv(queue_t *);

STATIC struct qinit sscop_winit = {
	sscop_wput,			/* Write put (msg from above) */
	sscop_wsrv,			/* Write queue service */
	NULL,				/* Each open */
	NULL,				/* Last close */
	NULL,				/* Admin (not used) */
	&sscop_minfo,			/* Information */
	NULL				/* Statistics */
};

MODULE_STATIC struct streamtab sscopinfo = {
	&sscop_rinit,			/* Upper read queue */
	&sscop_winit,			/* Upper write queue */
	NULL,				/* Lower read queue */
	NULL				/* Lower write queue */
};

/*
 *  =========================================================================
 *
 *  SSCOP Private Structures
 *
 *  =========================================================================
 */

#define AA_ESTABLISH_REQ		0
#define AA_ESTABLISH_RES		1
#define AA_RELEASE_REQ			2
#define AA_DATA_REQ			3
#define AA_INFO_REQ			5
#define AA_BIND_REQ			6
#define AA_UNBIND_REQ			7
#define AA_UNITDATA_REQ			8
#define AA_OPTMGMT_REQ			9

#define AA_ESTABLISH_IND		11
#define AA_ESTABLISH_CON		12
#define AA_RELEASE_IND			13
#define AA_DATA_IND			14
#define AA_INFO_ACK			16
#define AA_BIND_ACK			17
#define AA_ERROR_ACK			18
#define AA_OK_ACK			19
#define AA_UNITDATA_IND			20

#define AA_DATACK_REQ			23
#define AA_DATACK_IND			24
#define AA_RESYNC_REQ			25
#define AA_RESYNC_IND			26
#define AA_RESYNC_RES			27
#define AA_RESYNC_CON			28

#define AA_RECOVER_IND			21
#define AA_RECOVER_RES			22
#define AA_RETRIEVE_REQ			29
#define AA_RETRIEVE_IND			30
#define AA_RETRIEVE_COMPLETE_IND	31

//#define AA_UDERROR_IND    mapped to MAA_ERROR_IND
//#define AA_EXDATA_REQ
//#define AA_EXDATA_IND
//
#define MAA_ERROR_IND
#define MAA_UNITDATA_REQ
#define MAA_UNITDATA_IND
//#define MAA_SET_TIMER_REQ
//#define MAA_ADD_LINK_REQ
//#define MAA_REMOVE_LINK_REQ
//#define MAA_REMOVE_LINK_IND

#define A_UNINIT		0
#define A_UNBND			1
#define	A_IDLE			2
#define A_OUTCON		3
#define A_INCON			4
#define A_DATAXFER		5
#define A_OUTREL		6
#define A_INREL			7	(not used)
#define A_FAKE			8	(not used)
#define A_HACK			12
#define A_OUTRES		16
#define A_INRES			17
#define A_OUTREC		18
#define A_INREC			19
#define A_RECOVERY		20

#define AS_UNBND		0
#define AS_WACK_BREQ		1
#define AS_WACK_UREQ		2
#define AS_IDLE			3
#define AS_WACK_OPTREQ		4
#define AS_WACK_RRES		5
#define AS_WCON_CREQ		6
#define AS_WRES_CIND		7
#define AS_WACK_CRES		8
#define AS_DATA_XFER		9
#define AS_WCON_RREQ		10
#define AS_WRES_RIND		11
#define AS_WACK_DREQ6		12
#define AS_WACK_DREQ7		13
#define AS_WACK_DREQ9		14
#define AS_WACK_DREQ10		15
#define AS_WACK_DREQ11		16
#define AS_WIND_ERROR		17
#define AS_WRES_VIND		18

#define AE_PDU_BGAK		    (1<<0)
#define AE_PDU_BGN		    (1<<1)
#define AE_PDU_BGREJ		    (1<<2)
#define AE_PDU_BREJ		    (1<<3)
#define AE_PDU_END		    (1<<4)
#define AE_PDU_ENDAK		    (1<<5)
#define AE_PDU_ER		    (1<<6)
#define AE_PDU_ERAK		    (1<<7)
#define AE_PDU_MD		    (1<<8)
#define AE_PDU_POLL		    (1<<9)
#define AE_PDU_RS		    (1<<10)
#define AE_PDU_RSAK		    (1<<11)
#define AE_PDU_SD		    (1<<12)
#define AE_PDU_STAT		    (1<<13)
#define AE_PDU_UD		    (1<<14)
#define AE_PDU_USTAT		    (1<<15)

#define AE_INFO_REQ		    (1<<0)
#define AE_INFO_ACK		    (1<<1)
#define AE_BIND_REQ		    (1<<2)
#define AE_BIND_ACK		    (1<<3)
#define AE_UNBIND_REQ		    (1<<4)
#define AE_OPTMGMT_REQ		    (1<<5)
#define AE_ERROR_ACK		    (1<<6)
#define AE_OK_ACK		    (1<<7)
#define AE_ESTABLISH_REQ	    (1<<8)
#define AE_ESTABLISH_IND	    (1<<9)
#define AE_ESTABLISH_RES	    (1<<10)
#define AE_ESTABLISH_CON	    (1<<11)
#define AE_RELEASE_REQ		    (1<<12)
#define AE_RELEASE_IND		    (1<<13)
#define AE_DATA_REQ		    (1<<14)
#define AE_DATA_IND		    (1<<15)
#define AE_DATACK_REQ		    (1<<16)
#define AE_DATACK_IND		    (1<<17)
#define AE_RESYNC_REQ		    (1<<18)
#define AE_RESYNC_IND		    (1<<19)
#define AE_RESYNC_RES		    (1<<20)
#define AE_RESYNC_CON		    (1<<21)
#define AE_RECOVER_IND		    (1<<22)
#define AE_RECOVER_RES		    (1<<23)
#define AE_UNITDATA_REQ		    (1<<24)
#define AE_UNITDATA_IND		    (1<<25)
#define AE_RETRIEVE_REQ		    (1<<26)
#define AE_RETRIEVE_IND		    (1<<27)
#define AE_RETRIEVE_COMPLETE_IND    (1<<28)

#define AE_TIMEOUT_CC		    (1<<29)
#define AE_TIMEOUT_POLL		    (1<<30)

#define SSCOP_BGN	0x01
#define SSCOP_BGAK	0x02
#define SSCOP_END	0x03
#define SSCOP_ENDAK	0x04
#define SSCOP_RS	0x05
#define SSCOP_RSAK	0x06
#define SSCOP_BGREJ	0x07
#define SSCOP_SD	0x08
#define SSCOP_ER	0x09
#define SSCOP_POLL	0x0a
#define SSCOP_STAT	0x0b
#define SSCOP_USTAT	0x0c
#define SSCOP_UD	0x0d
#define SSCOP_MD	0x0e
#define SSCOP_ERAK	0x0f

/*
 *  AA_INFO_REQ
 */
struct AA_info_req {
	ulong PRIM_type;
};

/*
 *  AA_INFO_ACK
 */
struct AA_info_ack {
	ulong PRIM_type;
	ulong NSDU_size;		/* maximum NSDU size */
	ulong ENSDU_size;		/* maximum ENSDU size */
	ulong CDATA_size;		/* connect data size */
	ulong DDATA_size;		/* discon data size */
	ulong ADDR_size;		/* address size */
	ulong ADDR_length;		/* address length */
	ulong ADDR_offset;		/* address offset */
	ulong QOS_length;		/* QOS values length */
	ulong QOS_offset;		/* QOS values offset */
	ulong QOS_range_length;		/* length of QOS values' range */
	ulong QOS_range_offset;		/* offset of QOS values' range */
	ulong OPTIONS_flags;		/* bit masking for options supported */
	ulong NIDU_size;		/* network i/f data unit size */
	long SERV_type;			/* service type */
	ulong CURRENT_state;		/* current state */
	ulong PROVIDER_type;		/* type of NS provider */
	ulong NODU_size;		/* optimal NSDU size */
	ulong PROTOID_length;		/* length of bound protocol ids */
	ulong PROTOID_offset;		/* offset of bound protocol ids */
	ulong NPI_version;		/* version # of npi that is supported */
};

/*
 *  AA_BIND_REQ
 */
struct AA_bind_req {
	ulong PRIM_type;		/* always AA_BIND_REQ */
	ulong ADDR_length;		/* length of address */
	ulong ADDR_offset;		/* offset of address */
	ulong CONIND_number;		/* requested # of connect-indications to be queued */
	ulong BIND_flags;		/* bind flags */
	ulong PROTOID_length;		/* length of bound protocol ids */
	ulong PROTOID_offset;		/* offset of bound protocol ids */
};

/*
 *  AA_BIND_ACK
 */
struct AA_bind_ack {
	ulong PRIM_type;		/* always N_BIND_ACK */
	ulong ADDR_length;		/* address length */
	ulong ADDR_offset;		/* offset of address */
	ulong CONIND_number;		/* connection indications */
	ulong TOKEN_value;		/* value of "token" assigned to stream */
	ulong PROTOID_length;		/* length of bound protocol ids */
	ulong PROTOID_offset;		/* offset of bound protocol ids */
};

/*
 *  AA_UNBIND_REQ
 */
struct AA_unbind_req {
	ulong PRIM_type;
};

/*
 *  AA_OPTMGMT_REQ
 */
struct AA_optmgmt_req {
	ulong PRIM_type;
	ulong QOS_length;
	ulong QOS_offset;
	ulong OPTMGMT_flags;
};

/*
 *  AA_ERROR_ACK
 */
struct AA_error_ack {
	ulong PRIM_type;		/* always N_ERROR_ACK */
	ulong ERROR_prim;		/* primitive in error */
	ulong NPI_error;		/* NPI error code */
	ulong UNIX_error;		/* UNIX error code */
};

/*
 *  AA_OK_ACK
 */
struct AA_ok_ack {
	np_ulong PRIM_type;		/* always N_OK_ACK */
	np_ulong CORRECT_prim;		/* primitive being acknowledged */
};

/*
 *  AA_ESTABLISH_REQ (M_PROTO, 0 or more M_DATA)
 */
struct AA_establish_req {
	ulong PRIM_type;
	ulong DEST_length;
	ulong DEST_offset;
	ulong CONN_flags;		/* BR option */
	ulong QOS_length;
	ulong QOS_offset;
};

/*
 *  AA_ESTABLISH_IND (M_PROTO, 0 or more M_DATA)
 */
struct AA_establish_ind {
	ulong PRIM_type;
	ulong DEST_length;
	ulong DEST_offset;
	ulong SRC_length;
	ulong SRC_offset;
	ulong SEQ_number;
	ulong CONN_flags;
	ulong QOS_length;
	ulong QOS_offset;
};

/*
 *  AA_ESTABLISH_RES (M_PROTO, 0 or more M_DATA)
 */
struct AA_establish_res {
	ulong PRIM_type;
	ulong TOKEN_value;
	ulong RES_length;
	ulong RES_offset;
	ulong SEQ_number;
	ulong CONN_flags;		/* BR option */
	ulong QOS_length;
	ulong QOS_offset;
};

/*
 *  AA_ESTABLISH_CON (M_PROTO, 0 or more M_DATA)
 */
struct AA_establish_con {
	ulong PRIM_type;
	ulong RES_length;
	ulong RES_offset;
	ulong CONN_flags;
	ulong QOS_length;
	ulong QOS_offset;
};

/*
 *  AA_RELEASE_REQ (M_PROTO, 0 or more M_DATA)
 */
struct AA_release_req {
	ulong PRIM_type;
	ulong DISCON_reason;
	ulong RES_length;
	ulong RES_offset;
	ulong SEQ_number;
};

/*
 *  AA_RELEASE_IND (M_PROTO, 0 or more M_DATA)
 */
struct AA_release_ind {
	ulong PRIM_type;
	ulong DISCON_orig;		/* Source */
	ulong RES_length;
	ulong RES_offset;
	ulong SEQ_number;
};

/*
 *  AA_DATA_REQ (M_PROTO, 1 or more M_DATA )
 */
struct AA_data_req {
	ulong PRIM_type;
	ulong DATA_xfer_flags;		/* OOS */
};

/*
 *  AA_DATA_IND (M_PROTO, 1 or more M_DATA)
 */
struct AA_data_ind {
	ulong PRIM_type;
	ulong DATA_xfer_flags;		/* OOS */
	ulong PDU_sequence;		/* SN *//* in addition to NPI */
};

/*
 *  AA_DATACK_REQ (M_PROTO)
 */
struct AA_datack_req {
	ulong PRIM_type;
};

/*
 *  AA_DATACK_IND (M_PROTO) (not used)
 */
struct AA_datack_ind {
	ulong PRIM_type;
	ulong PDU_sequence;		/* SN *//* in addition to NPI */
};

/*
 *  AA_RESYNC_REQ (M_PROTO, 0 or more M_DATA)
 */
struct AA_resync_req {
	ulong PRIM_type;
	ulong RESET_reason;		/* not used */
};

/*
 *  AA_RESYNC_IND (M_PROTO, 0 or more M_DATA)
 */
struct AA_resync_ind {
	ulong PRIM_type;
	ulong RESET_orig;
	ulong RESET_reason;
};

/*
 *  AA_RESYNC_RES (M_PROTO)
 */
struct AA_resync_res {
	ulong PRIM_type;
};

/*
 *  AA_RESYNC_CON (M_PROTO)
 */
struct AA_resync_con {
	ulong PRIM_type;
};

/*
 *  AA_RECOVER_IND (M_PROTO)	(not in NPI)
 */
struct AA_recover_ind {
	ulong PRIM_type;
};

/*
 *  AA_RECOVER_RES (M_PROTO)	(not in NPI)
 */
struct AA_recover_res {
	ulong PRIM_type;
};

/*
 *  AA_UNITDATA_REQ (M_PROTO, 0 or more M_DATA)
 */
struct AA_unitdata_req {
	ulong PRIM_type;
	ulong DEST_length;
	ulong DEST_offset;
	ulong RESERVED_field[2];
};

/*
 *  AA_UNITDATA_IND (M_PROTO, 0 or more M_DATA)
 */
struct AA_unitdata_ind {
	ulong PRIM_type;
	ulong DEST_length;
	ulong DEST_offset;
	ulong SRC_length;
	ulong SRC_offset;
	ulong RESERVED_field;
};

/*
 *  AA_RETRIEVE_REQ (M_PROTO)	(not in NPI)
 */
struct AA_retieve_req {
	ulong PRIM_type;
	ulong RETRIEVE_sequence;
};

/*
 *  AA_RETRIEVE_IND (M_PROTO, 0 or more M_DATA)	(not in NPI)
 */
struct AA_retrieve_ind {
	ulong PRIM_type;
};

/*
 *  AA_RETRIEVE_COMPLETE_IND (M_PROTO)	(not in NPI)
 */
struct AA_retrieve_complete_ind {
	ulong PRIM_type;
};

/*
 *  MAA_ERROR_IND   (M_PROTO)	(not in NPI)
 */
struct MAA_error_ind {
	ulong PRIM_type;
	ulong DEST_length;
	ulong DEST_offset;
	ulong RESERVED_field;
	ulong ERROR_type;
	ulong ERROR_count;		/* not in NPI */
};

/*
 *  MAA_UNITDATA_REQ (M_PROTO, 0 or more M_DATA)
 */
#define MAA_unitdata_req    AA_unitdata_req
/*
 *  MAA_UNITDATA_IND (M_PROTO, 0 or more M_DATA)
 */
#define MAA_unitdata_ind    AA_unitdata_ind

/*
 *  =========================================================================
 *
 *  SSCOP Message Structures
 *
 *  =========================================================================
 */

/*
 *  =========================================================================
 *
 *  SSCOP AA-Provider --> AA-User Primitives
 *
 *  =========================================================================
 */
/*
 *  AA_INFO_ACK
 *  -------------------------------------------------------------------------
 */
STATIC __inline__ mblk_t *
aa_info_ack(queue_t *q)
{
	mblk_t *mp;
	struct AA_info_ack *p;
	sscop_t *sp = (sscop_t *) q->q_ptr;

	if ((mp = allocb(sizeof(*p) + add_len + qos_len + rng_len + pro_len, BPRI_MED))) {
		mp->b - datap->db_type = M_PCPROTO;
		p = (struct AA_info_ack *) mp->b_wptr;
		p->PRIM_type = AA_INFO_ACK;
		p->NSDU_size = -1;
		p->ENSDU_size = 0;
		p->CDATA_size = -1;
		p->DDATA_size = -1;
		p->ADDR_size = sizeof(uint16_t) + sizeof(uint32_t);
		p->ADDR_length = add_len;
		p->ADDR_offset = add_len ? sizeof(*p) : 0;
		p->QOS_length = qos_len;
		p->QOS_offset = qos_len ? sizeof(*p) + add_len : 0;
		p->QOS_range_length = rng_len;
		p->QOS_range_offset = rng_len ? sizeof(*p) + add_len + qos_len : 0;
		p->OPTIONS_flags = 0;
		p->NIDU_size = -1;
		p->SERV_type = FIXME;
		p->CURRENT_state = sp->state;
		p->PROVIDER_type = FIXME;
		p->NODU_size = FIXME;
		p->PROTOID_length = pro_len;
		p->PROTOID_offset = pro_len ? sizeof(*p) + add_len + qos_len + rng_len : 0;
		p->NPI_version = FIXME;
		mp->b_wptr += sizeof(*p);
		bcopy(add_ptr, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
		bcopy(qos_ptr, mp->b_wptr, qos_len);
		mp->b_wptr += qos_len;
		bcopy(rng_ptr, mp->b_wptr, rng_len);
		mp->b_wptr += rng_len;
		bcopy(pro_ptr, mp->b_wptr, pro_len);
		mp->b_wptr += pro_len;
	}
	return (mp);
}

/*
 *  AA_BIND_ACK
 *  -------------------------------------------------------------------------
 */
STATIC __inline__ mblk_t *
aa_bind_ack()
{
	mblk_t *mp;
	struct AA_bind_req *p;

	if ((mp = allocb(sizeof(*p) + add_len + pro_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (struct AA_bind_req *) mp->b_wptr;
		p->PRIM_type = AA_BIND_ACK;
		p->ADDR_length = add_len;
		p->ADDR_offset = add_len ? sizeof(*p) : 0;
		p->CONIND_number = sp->cons;
		p->BIND_flags = FIXME;
		p->PROTOID_length = pro_len;
		p->PROTOID_offset = pro_len ? sizeof(*p) + add_len : 0;
		mp->b_wptr += sizeof(*p);
		bcopy(add_ptr, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
		bcopy(pro_ptr, mp->b_wptr, pro_len);
		mp->b_wptr += pro_len;
	}
	return (mp);
}

/*
 *  AA_ERROR_ACK
 *  -------------------------------------------------------------------------
 */
STATIC __inline__ mblk_t *
aa_error_ack(ulong prim, long err)
{
	mblk_t *mp;
	struct AA_error_ack *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (struct AA_error_ack *) mp->b_wptr;
		p->PRIM_type = AA_ERROR_ACK;
		p->ERROR_prim = prim;
		p->NPI_error = err > 0 ? err : AASYSERR;
		p->UNIX_error = err < 0 ? -err : 0;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  AA_OK_ACK
 *  -------------------------------------------------------------------------
 */
STATIC __inline__ mblk_t *
aa_ok_ack(ulong prim)
{
	mblk_t *mp;
	struct AA_ok_ack *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (struct AA_ok_ack *) mp->b_wptr;
		p->PRIM_type = AA_OK_ACK;
		p->CORRECT_prim = prim;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  AA_ESTABLISH_IND
 *  -------------------------------------------------------------------------
 */
STATIC __inline__ mblk_t *
aa_establish_ind(dst_ptr, dst_len, src_ptr, src_len, seq, flags, qos_ptr, qos_len, db)
	caddr_t dst_ptr;
	size_t dst_len;
	caddr_t src_ptr;
	size_t src_len;
	ulong seq;
	ulong flags;
	caddr_t qos_ptr;
	size_t qos_len;
	mblk_t *db;
{
	mblk_t *mp;
	struct AA_establish_ind *p;

	if ((mp = allocb(sizeof(*p) + dst_len + src_len + qos + len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (struct AA_establish_ind *) mp->b_wptr;
		p->PRIM_type = AA_ESTABLISH_IND;
		p->DEST_length = dst_len;
		p->DEST_offset = dst_len ? sizeof(*p) : 0;
		p->SRC_length = src_len;
		p->SRC_offset = src_len ? sizeof(*p) + dst_len : 0;
		p->SEQ_number = seq;
		p->CONN_flags = flags;
		p->QOS_length = qos_len;
		p->QOS_offset = qos_len ? sizeof(*p) + dst_len + src_len : 0;
		mp->b_wptr += sizeof(*p);
		bcopy(dst_ptr, mp->b_wptr, dst_len);
		mp->b_wptr += dst_len;
		bcopy(src_ptr, mp->b_wptr, src_len);
		mp->b_wptr += src_len;
		bcopy(qos_ptr, mp->b_wptr, qos_len);
		mp->b_wptr += qos_len;
		mp->b_cont = db;
	}
	return (mp);
}

/*
 *  AA_ESTABLISH_CON
 *  -------------------------------------------------------------------------
 */
STATIC __inline__ mblk_t *
aa_establish_con(res_ptr, res_len, flags, qos_ptr, qos_len, db)
	caddr_t res_ptr;
	size_t res_len;
	uint flags;
	caddr_t qos_ptr;
	size_t qos_len;
	mblk_t *db;
{
	mblk_t *mp;
	struct AA_establish_con *p;

	if ((mp = allocb(sizeof(*p) + res_len + qos_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (struct AA_establish_con *) mp->b_wptr;
		p->PRIM_type = AA_ESTABLISH_CON;
		p->RES_length = res_len;
		p->RES_offset = res_len ? sizeof(*p) : 0;
		p->CONN_flags = flags;
		p->QOS_length = res_len;
		p->QOS_offset = res_len ? sizeof(*p) : 0;
		mp->b_wptr += sizeof(*p);
		bcopy(res_ptr, mp->b_wptr, res_len);
		mp->b_wptr += res_len;
		bcopy(qos_ptr, mp->b_wptr, qos_len);
		mp->b_wptr += qos_len;
		mp->b_cont = db;
	}
	return (mp);
}

/*
 *  AA_RELEASE_IND
 *  -------------------------------------------------------------------------
 */
STATIC __inline__ mblk_t *
aa_release_ind(orig, res_ptr, res_len, seq, db)
	uint orig;
	caddr_t res_ptr;
	size_t res_len;
	uint seq;
	mblk_t *db;
{
	mblk_t *mp;
	struct AA_release_ind *p;

	if ((mp = allocb(sizeof(*p) + res_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (struct AA_release_ind *) mp->b_wptr;
		p->PRIM_type = AA_RELEASE_IND;
		p->DISCON_orig = orig;
		p->RES_length = res_len;
		p->RES_offset = res_len ? sizeof(*p) : 0;
		p->SEQ_number = seq;
		mp->b_wptr += sizeof(*p);
		bcopy(res_ptr, mp->b_wptr, res_len);
		mp->b_wptr += res_len;
		mp->b_cont = db;
	}
	return (mp);
}

/*
 *  AA_DATA_IND
 *  -------------------------------------------------------------------------
 */
STATIC __inline__ mblk_t *
aa_data_ind(flags, seqno, db)
	ulong flags;
	ulong seqno;
	mblk_t *db;
{
	mblk_t *mp;
	struct AA_data_ind *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (struct AA_data_ind *) mp->b_wptr;
		p->PRIM_type = AA_DATA_IND;
		p->DATA_xfer_flags = flags;
		p->PDU_sequence = seqno;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  AA_DATACK_IND
 *  -------------------------------------------------------------------------
 */
STATIC __inline__ mblk_t *
aa_datack_ind(seqno, db)
	ulong seqno;
	mblk_t *db;
{
	mblk_t *mp;
	struct AA_datack_ind *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (struct AA_datack_ind *) mp->b_wptr;
		p->PRIM_type = AA_DATACK_IND;
		p->PDU_sequence = seqno;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  AA_RESYNC_IND
 *  -------------------------------------------------------------------------
 */
STATIC __inline__ mblk_t *
aa_resync_ind(orig, reason, db)
	ulong orig;
	ulong reason;
	mblk_t *db;
{
	mblk_t *mp;
	struct AA_resync_ind *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (struct AA_resync_ind *) mp->b_wptr;
		p->PRIM_type = AA_RESYNC_IND;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = db;
	}
      return (mp):
}

/*
 *  AA_RESYNC_IND
 *  -------------------------------------------------------------------------
 */
STATIC __inline__ mblk_t *
aa_resync_ind(orig, reason, db)
	ulong orig;
	ulong reason;
	mblk_t *db;
{
	mblk_t *mp;
	struct AA_resync_ind *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (struct AA_resync_ind *) mp->b_wptr;
		p->PRIM_type = AA_RESYNC_IND;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = db;
	}
      return (mp):
}

/*
 *  AA_RECOVER_IND
 *  -------------------------------------------------------------------------
 */
STATIC __inline__ mblk_t *
aa_recover_ind(void)
{
	mblk_t *mp;
	struct AA_recover_ind *p;

	if ((mp = allocb(sizoef(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (struct AA_recover_ind *) mp->b_wptr;
		p->PRIM_type = AA_RECOVER_IND;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  AA_UNITDATA_IND
 *  -------------------------------------------------------------------------
 */
STATIC __inline__ mblk_t *
aa_unitdata_ind(dst_ptr, dst_len, src_ptr, src_len, db)
	caddr_t dst_ptr;
	size_t dst_len;
	caddr_t src_ptr;
	size_t src_len;
	mblk_t *db;
{
	mblk_t *mp;
	struct AA_unitdata_ind *p;

	if ((mp = allocb(sizeof(*p) + dst_len + src_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (struct AA_unitdata_ind *) mp->b_wptr;
		p->PRIM_type = AA_UNITDATA_IND;
		p->DEST_length = dst_len;
		p->DEST_offset = dst_len ? sizeof(*p) : 0;
		p->SRC_length = src_len;
		p->SRC_offset = src_len ? sizeof(*p) + dst_len : 0;
		mp->b_wptr += sizeof(*p);
		bcopy(dst_ptr, mp->b_wptr, dst_len);
		mp->b_wptr += dst_len;
		bcopy(src_ptr, mp->b_wptr, src_len);
		mp->b_wptr += src_len;
		mp->b_cont = db;
	}
	return (mp);
}

/*
 *  AA_RETRIEVE_IND
 *  -------------------------------------------------------------------------
 */
STATIC __inline__ mblk_t *
aa_retrieve_ind(db)
	mblk_t *db;
{
	mblk_t *mp;
	struct AA_retrieve_ind *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (struct AA_retrieve_ind *) mp->b_wptr;
		p->PRIM_type = AA_RETRIEVE_IND;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = db;
	}
	return (mp);
}

/*
 *  AA_RETRIEVE_COMPLETE
 *  -------------------------------------------------------------------------
 */
STATIC __inline__ mblk_t *
aa_retrieve_complete_ind(void)
{
	mblk_t *mp;
	struct AA_retrieve_complete_ind *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (struct AA_retrieve_complete_ind *) mp->b_wptr;
		p->PRIM_type = AA_RETRIEVE_COMPLETE_IND;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  MAA_ERROR_IND
 *  -------------------------------------------------------------------------
 */
STATIC __inline__ mblk_t *
maa_error_ind(dst_ptr, dst_len, code, count)
	caddr_t dst_ptr;
	size_t dst_len;
	uint code;
	uint count;
{
	mblk_t *mp;
	struct MAA_error_ind *p;

	if ((mp = allocb(sizeof(*p) + dst_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (struct MAA_error_ind *) mp->b_wptr;
		p->PRIM_type = MAA_ERROR_IND;
		p->DEST_length = dst_len;
		p->DEST_offset = dst_len ? sizeof(*p) : 0;
		p->RESERVED_field = 0;
		p->ERROR_type = code;
		p->ERROR_count = count;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  MAA_UNITDATA_IND
 *  -------------------------------------------------------------------------
 */
STATIC __inline__ mblk_t *
maa_unitdata_ind(dst_ptr, dst_len, src_ptr, src_len, db)
	caddr_t dst_ptr;
	size_t dst_len;
	caddr_t src_ptr;
	size_t src_len;
	mblk_t *db;
{
	mblk_t *mp;
	struct MAA_unitdata_ind *p;

	if ((mp = allocb(sizeof(*p) + dst_len + src_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (struct MAA_unitdata_ind *) mp->b_wptr;
		p->PRIM_type = MAA_UNITDATA_IND;
		p->DEST_length = dst_len;
		p->DEST_offset = dst_len ? sizeof(*p) : 0;
		p->SRC_length = src_len;
		p->SRC_offset = src_len ? sizeof(*p) + dst_len : 0;
		mp->b_wptr += sizeof(*p);
		bcopy(dst_ptr, mp->b_wptr, dst_len);
		mp->b_wptr += dst_len;
		bcopy(src_ptr, mp->b_wptr, src_len);
		mp->b_wptr += src_len;
		mp->b_cont = db;
	}
	return (mp);
}

/*
 *  =========================================================================
 *
 *  SSCOP AA-Provider --> N-User (UDP or IP) Primitive
 *
 *  =========================================================================
 */
STATIC __inline__ mblk_t *
n_info_req(void)
{
	mblk_t *mp;
	N_info_req_t *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (N_info_req_t *) mp->b_wptr;
		p->PRIM_type = N_INFO_REQ;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
};
STATIC __inline__ mblk_t *
n_bind_req(add_ptr, add_len, cons, flags, pro_ptr, pro_len)
	caddr_t add_ptr;
	size_t add_len;
	uint cons;
	uint flags;
	caddr_t pro_ptr;
	size_t pro_len;
{
	mblk_t *mp;
	N_bind_req_t *p;

	if ((mp = allocb(sizeof(*p) + add_len + pro_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (N_bind_req_t *) mp->b_wptr;
		p->PRIM_type = N_BIND_REQ;
		p->ADDR_length = add_len;
		p->ADDR_offset = add_len ? sizeof(*p) : 0;
		p->CONIND_number = cons;
		p->BIND_flags = flags;
		p->PROTOID_length = pro_len;
		p->PROTOID_offset = pro_len ? sizeof(*p) + add_len : 0;
		mp->b_wptr += sizeof(*p);
		bcopy(add_ptr, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
		bcopy(pro_ptr, mp->b_wptr, pro_len);
		mp->b_wptr += pro_len;
	}
	return (mp);
}
STATIC __inline__ mblk_t *
n_unbind_req(void)
{
	mblk_t *mp;
	N_unbind_req_t *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (N_unbind_req_t *) mp->b_wptr;
		p->PRIM_type = N_UNBIND_REQ;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}
STATIC __inline__ mblk_t *
n_optmgmt_req(qos_ptr, qos_len, flags)
	caddr_t qos_ptr;
	size_t qos_len;
	uint flags;
{
	mblk_t *mp;
	N_optmgmt_req_t *p;

	if ((mp = allocb(sizeof(*p) + qos_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (N_optmgmt_req_t *) mp->b_wptr;
		p->PRIM_type = N_OPTMGMT_REQ;
		p->QOS_length = qos_len;
		p->QOS_offset = qos_len ? sizeof(*p) : 0;
		p->OPTMGMT_flags = flags;
		bcopy(qos_ptr, mp->b_wptr, qos_len);
		mp->b_wptr += qos_len;
	}
	return (mp);
}
STATIC __inline__ mblk_t *
n_conn_req(dst_ptr, dst_len, flags, qos_ptr, qos_len)
	caddr_t dst_ptr;
	size_t dst_len;
	uint flags;
	caddr_t qos_ptr;
	size_t qos_len;
{
	mblk_t *mp;
	N_conn_req_t *p;

	if ((mp = allocb(sizeof(*p) + dst_len + qos_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (N_conn_req_t *) mp->b_wptr;
		p->PRIM_type = N_CONN_REQ;
		p->DEST_length = dst_len;
		p->DEST_offset = dst_len ? sizeof(*p) : 0;
		p->CONN_flags = flags;
		p->QOS_length = qos_len;
		p->QOS_offset = qos_len ? sizeof(*p) + dst_len : 0;
		mp->b_wptr += sizeof(*p);
		bcopy(dst_ptr, mp->b_wptr, dst_len);
		mp->b_wptr += dst_len;
		bcopy(qos_ptr, mp->b_wptr, qos_len);
		mp->b_wptr += qos_len;
	}
	return (mp);
}
STATIC __inline__ mblk_t *
n_data_req(uint flags, mblk_t *db)
{
	mblk_t *mp;
	N_data_req_t *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (N_data_req_t *) mp->b_wptr;
		p->PRIM_type = N_DATA_REQ;
		p->DATA_xfer_flags = flags;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = db;
	}
	return (mp);
}
STATIC __inline__ mblk_t *
n_discon_req(reason, res_ptr, res_len, seq)
	uint reason;
	caddr_t res_ptr;
	size_t res_len;
	uint seq;
{
	mblk_t *mp;

	if ((mp = allocb(sizeof(*p) + res_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (N_discon_req_t *) mp->b_wptr;
		p->PRIM_type = N_DISCON_REQ;
		p->DISCON_reason = reason;
		p->RES_length = res_len;
		p->RES_offset = res_len ? sizeof(*p) : 0;
		p->SEQ_number = seq;
		mp->b_wptr += sizeof(*p);
		bcopy(res_ptr, mp->b_wptr, res_len);
		mp->b_wptr += res_len;
	}
	return (mp);
}
STATIC __inline__ mblk_t *
n_unitdata_req(dst_ptr, dst_len)
	caddr_t dst_ptr;
	size_t dst_len;
{
	mblk_t *mp;

	if ((mp = allocb(sizeof(*p) + dst_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (N_unitdata_req_t *) mp->b_wptr;
		p->PRIM_type = N_UNITDATA_REQ;
		p->DEST_length = dst_len;
		p->DEST_offset = dst_len ? sizeof(*p) : 0;
		p->RESERVED_field[0] = 0;
		p->RESERVED_field[1] = 0;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  ========================================================================
 *
 *  SSCOP --> SSCOP Peer Send Messages
 *
 *  ========================================================================
 */
/*
 *  SEND
 *  -------------------------------------------------------------------------
 */
STATIC __inline__ int
s_send_msg(queue_t *q, mblk_t *mp)
{
	mblk_t *pp;

	if ((pp = n_data_req(0))) {
		pp->b_cont = mp;
		putnext(q, pp);
		return (0);
	}
	freemsg(mp);
	return (-ENOBUFS);
}

/*
 *  SEND BGN.request N(W), N(SQ), N(S), SSCOP-UU
 *  -------------------------------------------------------------------------
 */
STATIC __inline__ int
s_send_bgn(queue_t *q, uint n_sq, uint n_mr, mblk_t *db)
{
	mblk_t *mp;
	size_t plen = 4 - ((db ? msgdsize(db) : 0) & 0x3);
	const uint32_t ptype = ((plen << 6) | SSCOP_BGN) << 24;

	if ((mp = allocb(12, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		mp->b_rptr += plen;
		*((uint32_t *) mp->b_wptr)++ = 0;
		*((uint32_t *) mp->b_wptr)++ = htonl(n_sq & 0xff);
		*((uint32_t *) mp->b_wptr)++ = htonl(ptype | (n_mr & 0xffffff));
		linkb(db, mp);
		return s_send_msg(WR(q), db);
	}
	return (-ENOBUFS);
}

/*
 *  SEND BGAK.request N(W), N(SQ), N(S), SSCOP-UU
 *  -------------------------------------------------------------------------
 */
STATIC __inline__ int
s_send_bgak(queue_t *q, uint n_mr, mblk_t *db)
{
	mblk_t *mp;
	size_t plen = 4 - ((db ? msgdsize(db) : 0) & 0x3);
	const uint32_t ptype = ((plen << 6) | SSCOP_BGAK) << 24;

	if ((mp = allocb(12, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		mp->b_rptr += plen;
		*((uint32_t *) mp->b_wptr)++ = 0;
		*((uint32_t *) mp->b_wptr)++ = 0;
		*((uint32_t *) mp->b_wptr)++ = htonl(ptype | (n_mr & 0xffffff));
		linkb(db, mp);
		return s_send_msg(WR(q), db);
	}
	return (-ENOBUFS);
}

/*
 *  SEND BGREJ.request SSCOP-UU
 *  -------------------------------------------------------------------------
 */
STATIC __inline__ int
s_send_bgrej(queue_t *q, mblk_t *db)
{
	mblk_t *mp;
	size_t plen = 4 - ((db ? msgdsize(db) : 0) & 0x3);
	const uint32_t ptype = ((plen << 6) | SSCOP_BGREJ) << 24;

	if ((mp = allocb(12, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		mp->b_rptr += plen;
		*((uint32_t *) mp->b_wptr)++ = 0;
		*((uint32_t *) mp->b_wptr)++ = 0;
		*((uint32_t *) mp->b_wptr)++ = htonl(ptype);
		linkb(db, mp);
		return s_send_msg(WR(q), db);
	}
	return (-ENOBUFS);
}

/*
 *  SEND END.request [src] SSCOP-UU
 *  -------------------------------------------------------------------------
 */
STATIC __inline__ int
s_send_end(queue_t *q, uint s, mblk_t *db)
{
	mblk_t *mp;
	size_t plen = 4 - ((db ? msgdsize(db) : 0) & 0x3);
	uint32_t ptype = (((plen << 2) | ((s & 1)) << 4) | SSCOP_END) << 24;

	if ((mp = allocb(12, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		mp->b_rptr += plen;
		*((uint32_t *) mp->b_wptr)++ = 0;
		*((uint32_t *) mp->b_wptr)++ = 0;
		*((uint32_t *) mp->b_wptr)++ = htonl(ptype);
		linkb(db, mp);
		return s_send_msg(WR(q), db);
	}
	return (-ENOBUFS);
}

/*
 *  SEND ENDAK.request ()
 *  -------------------------------------------------------------------------
 */
STATIC __inline__ int
s_send_endak(queue_t *q)
{
	mblk_t *mp;
	const uint32_t ptype = SSCOP_ENDAK << 24;

	if ((mp = allocb(8, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		*((uint32_t *) mp->b_wptr)++ = 0;
		*((uint32_t *) mp->b_wptr)++ = htonl(ptype);
		return s_send_msg(WR(q), mp);
	}
	return (-ENOBUFS);
}

/*
 *  SEND RS.request N(W), N(SQ), N(S), SSCOP-UU
 *  -------------------------------------------------------------------------
 */
STATIC __inline__ int
s_send_rs(queue_t *q, uint n_w, uint n_sq, uint n_s, mblk_t *db)
{
	mblk_t *mp;
	size_t plen = 4 - ((db ? msgdsize(db) : 0) & 0x3);
	const uint32_t ptype = ((plen << 6) | SSCOP_RS) << 24;

	if ((mp = allocb(12, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		mp->b_rptr += plen;
		*((uint32_t *) mp->b_wptr)++ = 0;
		*((uint32_t *) mp->b_wptr)++ = htonl(n_sq & 0xff);
		*((uint32_t *) mp->b_wptr)++ = htonl(ptype | (n_mr & 0xffffff));
		linkb(db, mp);
		return s_send_msg(WR(q), db);
	}
	return (-ENOBUFS);
}

/*
 *  SEND RSAK.request N(W), N(SQ), N(S)
 *  -------------------------------------------------------------------------
 */
STATIC __inline__ int
s_send_rsak(queue_t *q, uint n_mr)
{
	mblk_t *mp;
	const uint32_t ptype = (SSCOP_RSAK) << 24;

	if ((mp = allocb(8, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		*((uint32_t *) mp->b_wptr)++ = 0;
		*((uint32_t *) mp->b_wptr)++ = htonl(ptype | (n_mr & 0xffffff));
		return s_send_msg(WR(q), mp);
	}
	return (-ENOBUFS);
}

/*
 *  SEND ER.request N(W), N(SQ), N(S)
 *  -------------------------------------------------------------------------
 */
STATIC __inline__ int
s_send_er(queue_t *q, uint n_sq, uint n_mr)
{
	mblk_t *mp;
	const uint32_t ptype = (SSCOP_ER) << 24;

	if ((mp = allocb(8, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		*((uint32_t *) mp->b_wptr)++ = htonl(n_sq & 0xff);
		*((uint32_t *) mp->b_wptr)++ = htonl(ptype | (n_mr & 0xffffff));
		return s_send_msg(WR(q), mp);
	}
	return (-ENOBUFS);
}

/*
 *  SEND ERAK.request N(W), N(SQ), N(S)
 *  -------------------------------------------------------------------------
 */
STATIC __inline__ int
s_send_erak(queue_t *q, uint n_mr)
{
	mblk_t *mp;
	const uint32_t ptype = (SSCOP_ERAK) << 24;

	if ((mp = allocb(8, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		*((uint32_t *) mp->b_wptr)++ = 0;
		*((uint32_t *) mp->b_wptr)++ = htonl((n_mr & 0xffffff) | ptype);
		return s_send_msg(WR(q), mp);
	}
	return (-ENOBUFS);
}

/*
 *  SEND SD.request N(S), OOS, MU
 *  -------------------------------------------------------------------------
 */
STATIC __inline__ int
s_send_sd(queue_t *q, uint n_s, mblk_t *db)
{
	mblk_t *mp;
	size_t plen = 4 - ((db ? msgdsize(db) : 0) & 0x3);
	const uint32_t ptype = ((plen << 6) | SSCOP_SD) << 24;

	if ((mp = allocb(8, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		mp->b_rptr += plen;
		*((uint32_t *) mp->b_wptr)++ = 0;
		*((uint32_t *) mp->b_wptr)++ = htonl((n_s & 0xffffff) | ptype);
		linkb(db, mp);
		return s_send_msg(WR(q), db);
	}
	return (-ENOBUFS);
}

/*
 *  SEND POLL.request N(S), N(PS), N(SQ)
 *  -------------------------------------------------------------------------
 */
STATIC __inline__ int
s_send_poll(queue_t *q, uint n_ps, uint n_s)
{
	mblk_t *mp;
	const uint32_t ptype = (SSCOP_POLL) << 24;

	if ((mp = allocb(8, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		*((uint32_t *) mp->b_wptr)++ = htonl(n_ps & 0xffffff);
		*((uint32_t *) mp->b_wptr)++ = htonl((n_s & 0xffffff) | ptype);
		return s_send_msg(WR(q), mp);
	}
	return (-ENOBUFS);
}

/*
 *  SEND STAT.indicaiton N(R), N(MR), N(PS), N(SQ), N(SS), [list]
 *  -------------------------------------------------------------------------
 */
STATIC __inline__ int
s_send_stat(queue_t *q, uint n_r, uint n_mr, uint n_ps)
{
	mblk_t *mp;
	const uint32_t ptype = (SSCOP_STAT) << 24;

	if ((mp = allocb(12 + 4 * lnum, BPRI_MED))) {
		int i;

		mp->b_datap->db_type = M_DATA;
		for (i = 0; i < lnum; i++)
			*((uint32_t *) mp->b_wptr)++ = htonl(sp->le[i] & 0xffffff);
		*((uint32_t *) mp->b_wptr)++ = htonl(n_ps & 0xffffff);
		*((uint32_t *) mp->b_wptr)++ = htonl(n_mr & 0xffffff);
		*((uint32_t *) mp->b_wptr)++ = htonl((n_r & 0xffffff) | ptype);
		return s_send_msg(WR(q), mp);
	}
	return (-ENOBUFS);
}

/*
 *  SEND USTAT.request N(R), N(MR), N(PS), N(SQ), N(SS), [list]
 *  -------------------------------------------------------------------------
 */
STATIC __inline__ int
s_send_ustat(queue_t *q, uint n_r, uint n_mr)
{
	mblk_t *mp;
	const uint32_t ptype = (SSCOP_USTAT) << 24;

	if ((mp = allocb(16, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		*((uint32_t *) mp->b_wptr)++ = htonl(sp->le[0] & 0xffffff);
		*((uint32_t *) mp->b_wptr)++ = htonl(sp->le[1] & 0xffffff);
		*((uint32_t *) mp->b_wptr)++ = htonl(n_mr & 0xffffff);
		*((uint32_t *) mp->b_wptr)++ = htonl((n_r & 0xffffff) | ptype);
		return s_send_msg(WR(q), mp);
	}
	return (-ENOBUFS);
}

/*
 *  SEND UD.request MU
 *  -------------------------------------------------------------------------
 */
STATIC __inline__ int
s_send_ud(queue_t *q, mblk_t *mp)
{
	mblk_t *mp;
	size_t plen = 4 - ((db ? msgdsize(db) : 0) & 0x3);
	const uint32_t ptype = ((plen << 6) | SSCOP_UD) << 24;

	if ((mp = allocb(8, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		mp->b_rptr += plen;
		*((uint32_t *) mp->b_wptr)++ = 0;
		*((uint32_t *) mp->b_wptr)++ = htonl(ptype);
		return s_send_msg(WR(q), mp);
	}
	return (-ENOBUFS);
}

/*
 *  SEND MD.request MU
 *  -------------------------------------------------------------------------
 */
STATIC __inline__ int
s_send_md(queue_t *q, mblk_t *mp)
{
	mblk_t *mp;
	size_t plen = 4 - ((db ? msgdsize(db) : 0) & 0x3);
	const uint32_t ptype = ((plen << 6) | SSCOP_MD) << 24;

	if ((mp = allocb(8, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		mp->b_rptr += plen;
		*((uint32_t *) mp->b_wptr)++ = 0;
		*((uint32_t *) mp->b_wptr)++ = htonl(ptype);
		return s_send_msg(WR(q), mp);
	}
	return (-ENOBUFS);
}

/*
 *  ========================================================================
 *
 *  SSCOP State Machines
 *
 *  ========================================================================
 */

STATIC int
aa_idle_msg(queue_t *q, mblk_t *mp)
{
	mblk_t *bp;
	sscop_t *sp = (sscop_t *) q->q_ptr;
	const uint prim = (ntohl(*(((uint32_t *) mp->b_wptr) - 1) >> 24)) & 0xf;
	uint cause;

	switch (prim) {
	case SSCOP_BGN:
	case SSCOP_ENDAK:
		goto aa_idle_msg_ignore;
	case SSCOP_END:
	case SSCOP_ER:
		cause = A_CAUSE_L;
		break;
	case SSCOP_POLL:
		cause = A_CAUSE_G;
		break;
	case SSCOP_SD:
		cause = A_CAUSE_A;
		break;
	case SSCOP_BGAK:
		cause = A_CAUSE_C;
		break;
	case SSCOP_ERAK:
		cause = A_CAUSE_M;
		break;
	case SSCOP_STAT:
		cause = A_CAUSE_M;
		break;
	case SCCOP_USTAT:
		cause = A_CAUSE_I;
		break;
	case SSCOP_RS:
		cause = A_CAUSE_J;
		break;
	case SSCOP_RSAK:
		cause = A_CAUSE_K;
		break;
	case SSCOP_BGREJ:
		cause = A_CAUSE_D;
		break;
	default:
		cause = A_CAUSE_X;
		break;
	}
	if ((err = s_send_end(q, 1, NULL)))
		return (err);
	freemsg(mp);
	if (mgmt_queue)
		putnext(mgmt_queue, maa_error_ind(cause, 0));
	return (0);
      aa_idle_msg_ignore:
	freemsg(mp);
	return (0);
}

STATIC int
aa_idle_sig(queue_t *q, mblk_t *mp)
{
	mblk_t *bp;
	sscop_t *sp = (sscop_t *) q->q_ptr;
	const uint sig = ((union AA_signals *) mp->b_rptr)->signal;

	switch (sig) {
	case AA_ESTABLISH_REQ:
		/* clear transmitter */
		sp->clear_buffers = p->CONN_flags;
		sp->VT.CC = 1;
		sp->VT.SQ++;
		sp->VR.MR = FIXME;	/* initialize */
		sp->cdata = mp->b_cont;
		s_send_bgn(q, sp->VT.SQ, sp->VR.MR, dupmsg(mp->b_cont));
		mod_timeout(&sp->timer_cc, sscop_timeout_cc, sp->timer_cc_val);
		sp->a_state = AS_WCON_CREQ;
		break;
	case AA_RETRIEVE_REQ:
		/* data retrieval */
		break;
	case AA_DATA_REQ:
		/* queue SD */
		break;
	}
      aa_idle_sig_ignore:
	freemsg(mp);
	return (0);
}

STATIC int
aa_outcon_msg(queue_t *q, mblk_t *mp)
{
	mblk_t *bp;
	sscop_t *sp = (sscop_t *) q->q_ptr;
	const uint prim = (ntohl(*(((uint32_t *) mp->b_wptr) - 1) >> 24)) & 0xf;

	switch (prim) {
	case SSCOP_BGN:
		/* 
		 *  FIXME:
		 */
		return (0);
	case SSCOP_BGAK:
		/* 
		 * FIXME
		 */
		linkb(bp, mp->b_cont);
		putnext(q, bp);
		freeb(mp);
		sp->a_state = AS_DATA_XFER;
		return (0);
	case SSCOP_BGREJ:
		untimeoyt(xchg(&sp->timer_cc, 0));
		if (!(bp = aa_release_ind(q, 0, mp)))
			return -ENOBUFS;
		linkb(bp, mp->b_cont);
		putnext(q, bp);
		freeb(mp);
		sp->a_state = AS_IDLE;
		return (0);
	case SSCOP_ENDAK:
		break;
	case SSCOP_SD:
		break;
	case SSCOP_ERAK:
		break;
	case SSCOP_END:
		break;
	case SSCOP_STAT:
		break;
	case SSCOP_USTAT:
		break;
	case SSCOP_POLL:
		break;
	case SSCOP_SD:
		break;
	case SSCOP_ER:
		break;
	case SSCOP_RSAK:
		break;
	case SSCOP_RS:
		break;
	}
      aa_outcon_msg_ignore:
	freemsg(mp);
	return (0);
}

STATIC int
aa_outcon_sig(queue_t *q, mblk_t *mp)
{
	sscop_t *sp = (sscop_t *) q->q_ptr;
	const uint sig = ((union AA_signals *) mp->b_rptr)->signal;

	switch (sig) {
	case AA_RELEASE_REQ:
		if ((err = s_send_end(q)))
			return (err);
		sp->VT.CC = 1;
		mod_timeout(&sp->timer_cc, aa_cc_timeout, sp->timer_cc_val);
		sp->a_state = AS_WIND_DREQ;
		freemsg(mp);
		return (0);
	default:
		freemsg(mp);
		return (0);
	}
}

STATIC int
aa_incon_msg(queue_t *q, mblk_t *mp)
{
	mblk_t *bp, *ap;
	sscop_t *sp = (sscop_t *) q->q_ptr;
	const uint prim = (ntohl(*(((uint32_t *) mp->b_wptr) - 1) >> 24)) & 0xf;

	switch (prim) {
	case SSCOP_BGAK:
		freemsg(mp);
		putq(mgmt_queue, maa_error_ind(A_CAUSE_C, 0));
	return case SSCOP_BGREJ:
	{
		sscop_t *cp;

		if (!(bp = maa_error_ind(A_CAUSE_D, 0)))
			return (-ENOBUFS);
		/* 
		 *  FIXME:  Find incoming connection indication to which the
		 *  BGREJ applies.
		 */
		if (!(ap = aa_release_ind(A_SOURCE_SSCOP, NULL, 0, cp->seq, NULL))) {
			freeb(bp);
			return (-ENOBUFS);
		}
		/* 
		 *  FIXME:  Remove the incoming connection indication from the
		 *  listener's list.
		 */
		if (!sp->outcnt)
			sp->a_state = AS_IDLE;
		putq(mgmt_queue, bp);
		putnext(q, ap);
		freemsg(mp);
		return (0);
	}
	case SSCOP_END:
	{
		sscop_t *cp;

		/* 
		 *  FIXME:  Find incoming connection indication to which the
		 *  BGREJ applies.
		 */
		if (!(ap = aa_release_ind(AA_SOURCE_USER, NULL, 0, cp->seq, NULL)))
			return (-ENOBUFS);
		if ((err = s_send_endak(q))) {
			freemsg(ap);
			return (err);
		}
		/* 
		 *  FIXME:  Remove the incoming connection indication from the
		 *  listener's list.
		 */
		if (!sp->outcnt)
			sp->a_state = AS_IDLE;
		putnext(q, ap);
		freemsg(mp);
		return (0);
	}
	case SSCOP_SD:
		cause = A_CAUSE_A;
		break;
	case SSCOP_USTAT:
		cause = A_CAUSE_I;
		break;
	case SSCOP_STAT:
		cause = A_CAUSE_H;
		break;
	case SSCOP_POLL:
		cause = A_CAUSE_G;
		break;
	case SSCOP_ERAK:
		cause = A_CAUSE_M;
		break;
	case SSCOP_RSAK:
		cause = A_CAUSE_K;
		break;
	case SSCOP_RS:
		cause = A_CAUSE_J;
		break;
	}
	if (mgmt_queue && !(bp = maa_error_ind(cause, 0)))
		return (-ENOBUFS);
	/* 
	 *  FIXME:  Find incoming connection indication to which the
	 *  BGREJ applies.
	 */
	if (!(ap = aa_release_ind(A_SOURCE_SSCOP, NULL, 0, sp->seq, NULL))) {
		if (bp)
			freemsg(bp);
		return (-ENOBUFS);
	}
	if ((err = s_send_end(q, 1, NULL))) {
		if (bp)
			freemsg(bp);
		if (ap)
			freemsg(ap);
		return (err);
	}
	/* 
	 *  FIXME:  Remove the incoming connection indication from the
	 *  listener's list.
	 */
	if (!sp->outcnt)
		sp->a_state = AS_IDLE;
	if (mgmt_queue)
		putnext(mgmt_queue, bp);
	freemsg(mp);
	return (0);

      aa_incon_msg_report:
	freemsg(mp);
	if (mgmt_queue)
		putnext(mgmt_queue, maa_error_ind(cause, 0));
	return (0);
}

STATIC int
aa_incon_sig(queue_t *q, mblk_t *mp)
{
	sscop_t *sp = (sscop_t *) q->q_ptr;
	const uint sig = ((union AA_signals *) mp->b_rptr)->signal;

	switch (sig) {
	case AA_ESTABLISH_RES:
		/* clear transmitter */
		sp->clear_buffers = FIXME;	/* get br from flags */
		/* initialize VR.MR */
		if ((err = s_send_bgak(q, sp->VT.MR, mp->b_cont)))
			return (err);
		/* initialize state variables */
		/* set data transfer times */
		sp->a_state = AS_DATA_XFER;
		break;
	case AA_RELEASE_REQ:
	{
		sscop_t *cp;

		/* 
		 *  FIXME:  Find incoming connection indication to thwich the
		 *  AA_RELEASE_REQ applies (use seq number).
		 */
		if ((err = s_send_bgrej(cp->q)))
			return (err);
		/* 
		 *  FIXME:  Remove the incoming connection indication from the
		 *  listener's list.
		 */
		if (!sp->outcnt)
			sp->a_state = AS_IDLE;
		putnext(q, ap);
		break;
	}
	case AA_RETRIEVE_REQ:
		/* data retrieval */
		break;
	case AA_DATA_REQ:
		/* queue SD */
		break;
	}
	freemsg(mp);
      return (0):
}

STATIC int
aa_outdis_msg(queue_t *q, mblk_t *mp)
{
	mblk_t *bp, *ap;
	sscop_t *sp = (sscop_t *) q->q_ptr;
	const uint prim = (ntohl(*(((uint32_t *) mp->b_wptr) - 1) >> 24)) & 0xf;

	switch (prim) {
	case SSCOP_BGN:
		/* 
		 *  FIXME:
		 */
		return (0);
	case SSCOP_END:
		if ((err = s_send_endak(q)))
			return (err);
		/* fall through */
	case SSCOP_ENDAK:
	case SSCOP_BGREJ:
		untimeout(xchg(&sp->timer_cc, 0));
		if (!(ap = aa_release_confirm()))
			return (-ENOBUFS);
		putnext(q, ap);
		freemsg(mp);
		sp->a_state = AS_IDLE;
		return (0);
	default:
	case SSCOP_SD:
	case SSCOP_BGAK:
	case SSCOP_POLL:
	case SSCOP_STAT:
	case SSCOP_USTAT:
	case SSCOP_ERAK:
	case SSCOP_RS:
	case SSCOP_RSAK:
	case SSCOP_ER:
		break;
	}
	freemsg(mp);
	return (0);
}

STATIC int
aa_outdis_sig(queue_t *q, mblk_t *mp)
{
	mblk_t *bp, *ap;
	sscop_t *sp = (sscop_t *) q->q_ptr;
	const uint sig = ((union AA_signals *) mp->b_rptr)->signal;

	switch (sig) {
	case AA_ESTABLISH_REQ:
		if ((err = s_send_bgn()))
			return (err);
		/* clear transmitter */
		sp->clear_buffers = FIXME;	/* BR from message */
		sp->VT.CC = 1;
		sp->VT.SQ++;
		sp->VR.MR = FIXME;	/* initialize VR.MR */
		mod_timeout(&sp->timer_cc, aa_timeout_cc, sp->timer_cc_val);
		sp->a_state = AS_WCON_CREQ;

	case AA_RETRIEVE_REQ:
		/* data retrieval */
		freemsg(mp);
		return (0);
	}
}

/*
 *  =========================================================================
 *
 *  N-Provider (UDP or IP) --> AA-Provider
 *
 *  =========================================================================
 */
STATIC int
n_info_ack(q, mp)
	const queue_t *q;
	const mblk_t *mp;
{
	sscop_t *sp = (sscop_t *) q->q_ptr;
	N_info_ack_t *p = (N_info_ack_t *) mp->b_rptr;

	sp->nsdu_size = p->NSDU_size;
	sp->ensdu_size = p->ENSDU_size;
	sp->cdata_size = p->CDATA_size;
	sp->ddata_size = p->DDATA_size;
	sp->nidu_size = p->NIDU_size;
	sp->serv_type = p->SERV_type;
	sp->n_state = p->CURRENT_state;
	sp->prov_type = p->PROVIDER_type;
	sp->nodu_size = p->NODU_size;
	sp->npi_version = p->NPI_version;
	freemsg(mp);
	return (0);
}
STATIC int
n_bind_ack(q, mp)
	const queue_t *q;
	const mblk_t *mp;
{
	sscop_t *sp = (sscop_t *) q->q_ptr;
	N_bind_ack_t *p = (N_bind_ack_t *) mp->b_rptr;
	caddr_t add_ptr;
	size_t add_len;
	caddr_t pro_ptr;
	size_t pro_len;

	add_len = p->ADDR_length;
	add_ptr = p->ADDR_offset + mp->b_rptr;
	pro_len = p->PROTOID_length;
	pro_ptr = p->PROTOID_offset + mp->b_rptr;
	sp->bport = *((uint16_t *) add_ptr)++;
	sp->baddr = *((uint32_t *) add_ptr)++;
	sp->proto = *((uint16_t *) pro_ptr)++;
	freemsg(mp);
	return (0);
}
STATIC int
n_error_ack(q, mp)
	const queue_t *q;
	const mblk_t *mp;
{
	sscop_t *sp = (sscop_t *) q->q_ptr;
	N_error_ack_t *p = (N_error_ack_t *) mp->b_rptr;

	switch (sp->n_state) {
		/* 
		 *  FIXME
		 *  abort state transition
		 */
	}
	freemsg(mp);
	return (0);
}
STATIC int
n_ok_ack(q, mp)
	const queue_t *q;
	const mblk_t *mp;
{
	sscop_t *sp = (sscop_t *) q->q_ptr;
	N_ok_ack_t *p = (N_ok_ack_t *) mp->b_rptr;

	switch (sp->n_state) {
		/* 
		 *  FIXME
		 *  complete state transition
		 */
	}
	freemsg(mp);
	return (0);
}
STATIC int
n_conn_con(q, mp)
	const queue_t *q;
	const mblk_t *mp;
{
	sscop_t *sp = (sscop_t *) q->q_ptr;
	N_conn_con_t *p = (N_conn_con_t *) mp->b_rptr;

	if (sp->n_state == NS_WCON_CREQ) {
		sp->n_state = NS_DATA_XFER;
		freemsg(mp);
		return (0);
	}
	return -EPROTO;
}
STATIC int
n_discon_ind(q, mp)
	const queue_t *q;
	const mblk_t *mp;
{
	sscop_t *sp = (sscop_t *) q->q_ptr;
	N_discon_ind_t *p = (N_discon_ind_t *) mp->b_rptr;

	if (sp->n_state == NS_DATA_XFER) {
		sp->n_state = NS_IDLE;
		/* 
		 *  FIXME
		 *  indicate disconnection to AA-user
		 */
		if (sp->buf_ret) {
			sp->a_state = AS_WRES_VIND;
			/* 
			 *  FIXME
			 */
		}
	}
	return -EPROTO;
}
STATIC int
n_unitdata_ind(q, mp)
	const queue_t *q;
	const mblk_t *mp;
{
	sscop_t *sp = (sscop_t *) q->q_ptr;
	N_unitdata_ind_t *p = (N_unitdata_ind_t *) mp->b_rptr;

	if (sp->n_state == NS_IDLE) {
	}
	return -EPROTO;
}
STATIC int
n_uderror_ind(q, mp)
	const queue_t *q;
	const mblk_t *mp;
{
	sscop_t *sp = (sscop_t *) q->q_ptr;
	N_uderror_ind_t *p = (N_uderror_ind_t *) q->q_ptr;

	if (sp->n_state == NS_IDLE) {
	}
	return -EPROTO;
}

STATIC int (*n_prim[]) (queue_t *, mblk_t *) = {
	NULL,			/* N_CONN_REQ 0 */
	    NULL,		/* N_CONN_RES 1 */
	    NULL,		/* N_DISCON_REQ 2 */
	    NULL,		/* N_DATA_REQ 3 */
	    NULL,		/* N_ExDATA_REQ 4 */
	    NULL,		/* N_INFO_REQ 5 */
	    NULL,		/* N_BIND_REQ 6 */
	    NULL,		/* N_UNBIND_REQ 7 */
	    NULL,		/* N_UNITDATA_REQ 8 */
	    NULL,		/* N_OPTMGMT_REQ 9 */
	    NULL,		/* not used 10 */
	    NULL,		/* N_CONN_IND 11 */
	    NULL,		/* N_CONN_CON 12 */
	    NULL,		/* N_DISCON_IND 13 */
	    NULL,		/* N_DATA_IND 14 */
	    NULL,		/* N_EXDATA_IND 15 */
	    &n_info_ack,	/* N_INFO_ACK 16 */
	    &n_bind_ack,	/* N_BIND_ACK 17 */
	    &n_error_ack,	/* N_ERROR_ACK 18 */
	    &n_ok_ack,		/* N_OK_ACK 19 */
	    &n_unitdata_ind,	/* N_UNITDATA_IND 20 */
	    &n_uderror_ind,	/* N_UDERROR_IND 21 */
	    NULL,		/* not used 22 */
	    NULL,		/* N_DATACK_REQ 23 */
	    NULL,		/* N_DATACK_IND 24 */
	    NULL,		/* N_RESET_REQ 25 */
	    NULL,		/* N_RESET_IND 26 */
	    NULL,		/* N_RESET_RES 27 */
	    NULL		/* N_RESET_CON 28 */
};

/*
 *  =========================================================================
 *
 *  AA-User --> AA-Provider Primitives (M_CTL, M_PROTO, M_PCPROTO)
 *
 *  =========================================================================
 */
STATIC int
aa_info_req(queue_t *q, mblk_t *mp)
{
	mblk_t *ap;

	(void) mp;
	if (!(ap = aa_info_ack(q)))
		return (-EAGAIN);
	qreply(q, ap);
	return (0);
}
STATIC int
aa_bind_req(queue_t *q, mblk_t *mp)
{
	mblk_t *ap;
	sscop_t *sp = (sscop_t *) q->q_ptr;
	AA_bind_req_t *p = (AA_bind_req_t *) mp->b_rptr;
	caddr_t add_ptr;
	size_t add_len;

	/* 
	 *  FIXME: more...
	 *  FIXME: more...
	 *  FIXME: more...
	 */
}
STATIC int
aa_unbind_req(queue_t *q, mblk_t *mp)
{
}
STATIC int
aa_unitdata_req(queue_t *q, mblk_t *mp)
{
}
STATIC int
aa_optmgmt_req(queue_t *q, mblk_t *mp)
{
}
STATIC int
aa_establish_req(queue_t *q, mblk_t *mp)
{
}
STATIC int
aa_establish_res(queue_t *q, mblk_t *mp)
{
}
STATIC int
aa_release_req(queue_t *q, mblk_t *mp)
{
}
STATIC int
aa_data_req(queue_t *q, mblk_t *mp)
{
}
STATIC int
aa_exdata_req(queue_t *q, mblk_t *mp)
{
}
STATIC int
aa_datack_req(queue_t *q, mblk_t *mp)
{
}
STATIC int
aa_resync_req(queue_t *q, mblk_t *mp)
{
}
STATIC int
aa_resync_res(queue_t *q, mblk_t *mp)
{
}
STATIC int
aa_recover_res(queue_t *q, mblk_t *mp)
{
}
STATIC int
aa_retrieve_req(queue_t *q, mblk_t *mp)
{
}
STATIC int
maa_set_timer_req(queue_t *q, mblk_t *mp)
{
}
STATIC int
maa_add_link_req(queue_t *q, mblk_t *mp)
{
}
STATIC int
maa_remove_link_req(queue_t *q, mblk_t *mp)
{
}

STATIC int (*n_prim[]) (queue_t *, mblk_t *) = {
	&aa_establish_req,	/* AA_ESTABLISH_REQ 0 */
	    &aa_establish_res,	/* AA_ESTABLISH_RES 1 */
	    &aa_release_req,	/* AA_RELEASE_REQ 2 */
	    &aa_data_req,	/* AA_DATA_REQ 3 */
	    &aa_exdata_req,	/* AA_ExDATA_REQ 4 */
	    &aa_info_req,	/* AA_INFO_REQ 5 */
	    &aa_bind_req,	/* AA_BIND_REQ 6 */
	    &aa_unbind_req,	/* AA_UNBIND_REQ 7 */
	    &aa_unitdata_req,	/* AA_UNITDATA_REQ 8 */
	    &aa_optmgmt_req,	/* AA_OPTMGMT_REQ 9 */
	    NULL,		/* AA_RELEASE_CON 10 */
	    NULL,		/* AA_ESTABLISH_IND 11 */
	    NULL,		/* AA_ESTABLISH_CON 12 */
	    NULL,		/* AA_RELEASE_IND 13 */
	    NULL,		/* AA_DATA_IND 14 */
	    NULL,		/* AA_EXDATA_IND 15 */
	    NULL,		/* AA_INFO_ACK 16 */
	    NULL,		/* AA_BIND_ACK 17 */
	    NULL,		/* AA_ERROR_ACK 18 */
	    NULL,		/* AA_OK_ACK 19 */
	    NULL,		/* AA_UNITDATA_IND 20 */
	    NULL,		/* MAA_ERROR_IND 21 */
	    NULL,		/* not used 22 */
	    &aa_datack_req,	/* AA_DATACK_REQ 23 */
	    NULL,		/* AA_DATACK_IND 24 */
	    &aa_resync_req,	/* AA_RESET_REQ 25 */
	    NULL,		/* AA_RESET_IND 26 */
	    &aa_resync_res,	/* AA_RESET_RES 27 */
	    NULL,		/* AA_RESET_CON 28 */
	    NULL,		/* AA_RECOVER_IND 29 */
	    &aa_recover_res,	/* AA_RECOVER_RES 30 */
	    &aa_retrieve_req,	/* AA_RETRIEVE_REQ 31 */
	    NULL,		/* AA_RETRIEVE_IND 32 */
	    NULL,		/* AA_RETRIEVE_COMPLETE_IND 33 */
	    &maa_set_timer_req,	/* MAA_SET_TIMER_REQ 34 */
	    &maa_add_link_req,	/* MAA_ADD_LINK_REQ 35 */
	    &maa_remove_link_req,	/* MAA_REMOVE_LINK_REQ 36 */
	    NULL		/* MAA_REMOVE_LINK_IND 37 */
};

/*
 *  =========================================================================
 *
 *  SCTP IOCTLs
 *
 *  =========================================================================
 */
STATIC int (*aa_ioctl[]) (queue_t *, uint, void *) = {
};

/*
 *  =========================================================================
 *
 *  SCTP STREAMS Message Handling
 *
 *  =========================================================================
 *
 *  M_DATA Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC inline int
sscop_w_data(queue_t *q, mblk_t *mp)
{
	return sscop_write_data(q, mp);
}
STATIC inline int
sscop_r_data(queue_t *q, mblk_t *mp)
{
	return sscop_recv_data(q, mp);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_PROTO, M_PCPROTO Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC inline int
sscop_w_proto(queue_t *q, mblk_t *mp)
{
	int sig = ((union AA_signals *) (mp->b_rptr))->type;
	if (0 <= sig && sig < sizeof(aa_sigs) / sizeof(void *))
		if (aa_sigs[sig])
			return (*aa_sigs[sig]) (q, mp);
	return (-EOPNOTSUPP);
}
STATIC inline int
sscop_r_proto(queue_t *q, mblk_t *mp)
{
	int prim = ((union N_primitives *) (mp->b_rptr))->type;
	if (0 <= prim && prim <= sizeof(n_prim) / sizeof(void *))
		if (n_prim[prim])
			return (*n_prim[prim]) (q, mp);
	return (-EOPNOTSUPP);
}
STATIC inline int
sscop_w_pcproto(queue_t *q, mblk_t *mp)
{
	return sscop_w_proto(q, mp);
}
STATIC inline int
sscop_r_pcproto(queue_t *q, mblk_t *mp)
{
	return sscop_r_proto(q, mp);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_CTL Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC inline int
sscop_w_ctl(queue_t *q, mblk_t *mp)
{
	(void) q;
	(void) mp;
	return (-EOPNOTSUPP);
}
STATIC inline int
sscop_r_ctl(queue_t *q, mblk_t *mp)
{
	(void) q;
	(void) mp;
	return (-EOPNOTSUPP);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_IOCTL Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC inline int
sscop_w_ioctl(queue_t *q, mblk_t *mp)
{
	int ret;
	struct iocblk *iocp = (struct iocblk *) mp->b_rptr;
	int cmd = iocp->ioc_cmd;
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	int type = _IOC_TYPE(cmd);
	int nr = _IOC_NR(cmd);
	int size = _IOC_SIZE(cmd);

	switch (type) {
	case __SID:
		switch (cmd) {
		case I_LINK:
		case I_PLINK:
		case I_UNLINK:
		case I_PUNLINK:
		case I_FDINSERT:
			ret = -EINVAL;
		}
		break;

	case SSCOP_IOC_MAGIC:
		if (iocp->ioc_count >= size) {
			if (0 <= nr && nr < sizeof(aa_ioctl) / sizeof(void *)) {
				if (aa_ioctl[nr])
					ret = (*aa_ioctl[nr]) (q, cmd, arg);
				else
					ret = -EOPNOTSUPP;
			}
		} else
			return = -EINVAL;
		break;

	default:
		if (q->q_next) {
			putnext(q, mp);
			return (0);
		}
		ret = -EOPNOTSUPP;
	}
	mp->b_datap->db_type = ret ? M_IOCNAK : M_IOCACK;
	iocp->ioc_error = -ret;
	iocp->ioc_rval = ret ? -1 : 0;
	qreply(q, mp);
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_FLUSH Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC inline void
sscop_w_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		if (q->q_next) {
			putnext(q, mp);
			return;
		}
		mp->b_rptr[0] &= ~FLUSHW;
	}
	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(RD(q), FLUSHDATA);
		qreply(q, mp);
		return;
	}
	if (q->q_next) {
		putnext(q, mp);
		return;
	}
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_ERROR Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC inline void
sscop_r_error(queue_t *q, mblk_t *mp)
{
	sscop_t *sp = (sscop_t *) q->q_ptr;

	sp->a_state = AA_UNUSABLE;
	if (q->q_next) {
		putnext(q, mp);
		return;
	}
}

/*
 *  =========================================================================
 *
 *  STREAMS QUEUE PUT AND QUEUE SERVICE routines
 *
 *  =========================================================================
 *
 *  READ QUEUE PUT and SRV routinges
 *
 *  -------------------------------------------------------------------------
 *
 *  SSCOP RPUT - Message from below.
 *  -------------------------------------------------------------------------
 *  If the message is a priority message we attempt to process it immediately.
 *  If the message is a non-priority message, but there are no messages on the
 *  queue yet, we attempt to process it immediately.  If the message is not
 *  supported, we pass it down-queue if possible.  If the message cannot be
 *  processed immediately we place it on the queue.
 */
STATIC int
sscop_rput(q, mp)
	queue_t *q;
	mblk_t *mp;
{
	int err = -EOPNOTSUPP;

	if (mp->b_datap->db_type < QPCTL && q->q_count) {
		putq(q, mp);
		return (0);
	}
	switch (mp->b_datap->db_type) {
	case M_DATA:
		if ((err = sscop_r_data(q, mp)))
			break;
		return (0);
	case M_PROTO:
		if ((err = sscop_r_proto(q, mp)))
			break;
		return (0);
	case M_PCPROTO:
		if ((err = sscop_r_pcproto(q, mp)))
			break;
		return (0);
	case M_CTL:
		if ((err = sscop_r_ctl(q, mp)))
			break;
		return (0);
	case M_ERROR:
		sscop_r_error(q, mp);
		return (0);
	}
	switch (err) {
	case -EAGAIN:
		putq(q, mp);
		return (0);
	case -EOPNOTSUPP:
		if (q->q_next) {
			putnext(q, mp);
			return (0);
		}
	}
	freemsg(mp);
	return (err);
}

/*
 *  SSCOP RSRV - Queued message from below.
 *  -------------------------------------------------------------------------
 *  If the message is a priority message we attempt to process it immediately
 *  and without flow control.  If the message is a non-priority message and
 *  the next queue is flow controlled, we put the message back on the queue
 *  and wait.  If we cannot process a prority message immediately we cannot
 *  place it back on the queue and discard it.  We requeue the non-priority
 *  messages which cannot be processed immediately.  Unrecognized messages are
 *  passwed down-queue.
 */
STATIC int
sscop_rsrv(q)
	queue_t *q;
{
	mblk_t *mp;
	int err = -EOPNOTSUPP;

	while ((mp = getq(q))) {
		if (mp->b_datap->db_type < QPCTL && !canputnext(q)) {
			putbq(q, mp);
			return (0);
		}
		switch (mp->b_datap->db_type) {
		case M_DATA:
			if ((err = sscop_r_data(q, mp)))
				break;
			continue;
		case M_PROTO:
			if ((err = sscop_r_proto(q, mp)))
				break;
			continue;
		case M_PCPROTO:
			if ((err = sscop_r_pcproto(q, mp)))
				break;
			continue;
		case M_CTL:
			if ((err = sscop_r_ctl(q, mp)))
				break;
			continue;
		}
		switch (err) {
		case -EAGAIN:
			if (mp->b_datap->db_type < QPCTL) {
				putbq(q, mp);
				return (0);
			}
		case -EOPNOTSUPP:
			if (q->q_next) {
				putnext(q, mp);
				continue;
			}
		}
		freemsg(mp);
	}
	return (0);
}

/*
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *  
 *  =========================================================================
 */

/*
 *  Private structure allocation and deallocation.  We use Linux hardware
 *  aligned cache here for speedy access to information contained in the
 *  private data structure.
 */
kmem_cachep_t sscop_cachep = NULL;

STATIC sscop_t *
sscop_alloc_priv(queue_t *q)
{
	sscop_t *sp;

	if ((sscop = kmem_cache_alloc(sscop_cachep))) {
		bzero(sscop, sizeof(*sp));
		RD(q)->q_ptr = WR(q)->q_ptr = sp;
		sp->rq = RD(q);
		sp->wq = WR(q);
	}
	return (sp);
}

STATIC void
sscop_init_priv(void)
{
	if (!(sscop_cachep = kmem_find_general_cachep(sizeof(sscop_t)))) {
		/* allocate a new cachep */
	}
	return;
}

/*
 *  -------------------------------------------------------------------------
 *
 *  OPEN - Each open
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sscop_open(queue_t *q, drv_t * devp, int flag, int sflag, cred_t *crp)
{
	(void) crp;
	if (q->q_ptr != NULL)
		return (0);	/* open already */
	if (sflag == MODOPEN || WR(q)->q_next != NULL) {
		/* 
		 *  TODO: check to make sure that the module we are being
		 *  pushed over is compatible (i.e., it is of the right kind
		 *  of transport module.
		 */
		if (!(sscop_alloc_priv(q)))
			return ENOMEM;
		return (0);
	}
	return EIO;
}

/*
 *  -------------------------------------------------------------------------
 *
 *  CLOSE - Last close
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sscop_close(queue_t *q, int flag, cred_t *crp)
{
	(void) flag;
	(void) crp;
	sscop_free_priv(q);
	return (0);
}

/*
 *  =========================================================================
 *
 *  Registration and initialization
 *
 *  =========================================================================
 */
#ifdef LINUX
/*
 *  Linux Registration
 *  -------------------------------------------------------------------------
 */

unsigned short modid = MOD_ID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0444);
#endif
MODULE_PARM_DESC(modid, "Module ID for the SSCOP module. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LFS

STATIC struct fmodsw aa_fmod = {
	.f_name = MOD_NAME,
	.f_str = &sscopinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

STATIC int
aa_register_strmod(void)
{
	int err;

	if ((err = register_strmod(&aa_fmod)) < 0)
		return (err);
	return (0);
}

STATIC int
aa_unregister_strmod(void)
{
	int err;

	if ((err = unregister_strmod(&aa_fmod)) < 0)
		return (err);
	return (0);
}

#endif				/* LFS */

/*
 *  Linux STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LIS

STATIC int
aa_register_strmod(void)
{
	int err;

	if ((err = lis_register_strmod(&sscopinfo, MOD_NAME)) == LIS_NULL_MID)
		return (-EIO);
	if ((err = lis_register_module_qlock_option(err, LIS_QLOCK_NONE)) < 0) {
		lis_unregister_strmod(&sscopinfo);
		return (err);
	}
	return (0);
}

STATIC int
aa_unregister_strmod(void)
{
	int err;

	if ((err = lis_unregister_strmod(&sscopinfo)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

MODULE_STATIC int __init
sscopinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);	/* banner message */
	if ((err = aa_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", MOD_NAME, err);
		return (err);
	}
	if ((err = aa_register_strmod())) {
		cmn_err(CE_WARN, "%s: could not register module, err = %d", MOD_NAME, err);
		aa_term_caches();
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

MODULE_STATIC void __exit
sscopterminate(void)
{
	int err;

	if ((err = aa_unregister_strmod()))
		cmn_err(CE_WARN, "%s: could not unregister module", MOD_NAME);
	if ((err = aa_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", MOD_NAME);
	return;
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(sscopinit);
module_exit(sscopterminate);

#endif				/* LINUX */
