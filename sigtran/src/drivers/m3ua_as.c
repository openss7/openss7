/*****************************************************************************

 @(#) $Id: m3ua_as.c,v 0.9.2.2 2007/01/23 10:00:46 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (C) 2001  OpenSS7 Corporation <http://www.openss7.com>

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

 Last Modified $Date: 2007/01/23 10:00:46 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: m3ua_as.c,v $
 Revision 0.9.2.2  2007/01/23 10:00:46  brian
 - added test program and m2ua-as updates

 Revision 0.9.2.1  2006/10/17 11:55:42  brian
 - copied files into new packages from strss7 package

 Revision 0.9.2.9  2005/07/13 12:01:32  brian
 - working up compat and check pass (finally lindented LiS)

 Revision 0.9.2.8  2005/07/05 22:45:32  brian
 - change for strcompat package

 Revision 0.9.2.7  2005/05/10 18:05:52  brian
 - do not set clone flag cause symbol no longer exported

 Revision 0.9.2.6  2005/04/09 09:42:20  brian
 - addition of module alias for ko modules

 Revision 0.9.2.5  2005/03/31 06:53:08  brian
 - changes for EL$ (CentOS 4.0) compatibility

 Revision 0.9.2.4  2005/03/08 19:30:03  brian
 - Changes for new build compile.

 Revision 0.9.2.3  2004/08/29 20:25:21  brian
 - Updates to driver registration for Linux Fast-STREAMS.

 Revision 0.9.2.2  2004/08/26 23:37:57  brian
 - Converted for use with Linux Fast-STREAMS.

 Revision 0.9.2.1  2004/08/21 10:14:44  brian
 - Force checkin on branch.

 Revision 0.9  2004/01/17 08:20:21  brian
 - Added files for 0.9 baseline autoconf release.

 Revision 0.8.2.3  2003/04/14 12:13:00  brian
 Updated module license defines.

 Revision 0.8.2.2  2003/04/03 19:50:31  brian
 Updates preparing for release.

 Revision 0.8.2.1  2002/10/18 03:27:42  brian
 Indentation changes only.

 Revision 0.8  2002/04/02 08:20:42  brian
 Started Linux 2.4 development branch.

 Revision 0.7  2001/06/04 03:48:36  brian
 Added files for M3UA.

 *****************************************************************************/

static char const ident[] = "$Name:  $($Revision: 0.9.2.2 $) $Date: 2007/01/23 10:00:46 $";

#include <sys/os7/compat.h>

#include <ss7/m3ua.h>
#include <ss7/m3ua_ioctl.h>

#define M3UA_DESCRIP	"M3UA/SCTP STREAMS MULTIPLEXOR."
#define M3UA_COPYRIGHT	"Copyright (c) 2001 OpenSS7 Corp. All Rights Reserved."
#define M3UA_DEVICES	"Supports OpenSS7 drivers."
#define M3UA_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define M3UA_LICENSE	"GPL"
#define M3UA_BANNER	M3UA_DESCRIP	"\n" \
			M3UA_COPYRIGHT	"\n" \
			M3UA_DEVICES	"\n" \
			M3UA_CONTACT	"\n"

#ifdef LINUX
#ifdef MODULE
MODULE_AUTHOR(M3UA_CONTACT);
MODULE_DESCRIPTION(M3UA_DESCRIP);
MODULE_SUPPORTED_DEVICE(M3UA_DEVICES);
#ifdef MODULE_LICENSE
MODULE_LICENSE(M3UA_LICENSE);
#endif
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-m3ua_as");
#endif
#endif				/* LINUX */

#ifdef M3UA_DEBUG
static int m3ua_debug = M3UA_DEBUG;
#else
static int m3ua_debug = 2;
#endif

#define DEBUG_LEVEL m3ua_debug

#ifdef LFS
#define M3UA_DRV_ID		CONFIG_STREAMS_M3UA_MODID
#define M3UA_DRV_NAME		CONFIG_STREAMS_M3UA_NAME
#define M3UA_CMAJORS		CONFIG_STREAMS_M3UA_NMAJORS
#define M3UA_CMAJOR_0		CONFIG_STREAMS_M3UA_MAJOR
#define M3UA_UNITS		CONFIG_STREAMS_M3UA_NMINORS
#endif				/* LFS */

/*
 *  M3UA Message Definitions
 *  ========================
 */

#ifndef M3UA_PPI
#define M3UA_PPI    3
#endif

#define UA_VERSION  1
#define UA_PAD4(__len) (((__len)+3)&~0x3)
#define UA_MHDR(__version, __spare, __class, __type) \
	(__constant_htonl(((__version)<<24)|((__spare)<<16)|((__class)<<8)|(__type)))

#define UA_MSG_VERS(__hdr) ((ntohl(__hdr)>>24)&0xff)
#define UA_MSG_CLAS(__hdr) ((ntohl(__hdr)>> 8)&0xff)
#define UA_MSG_TYPE(__hdr) ((ntohl(__hdr)>> 0)&0xff)

/*
 *  MESSAGE CLASSES:-
 */
#define UA_CLASS_MGMT	0x00	/* UA Management (MGMT) Message */
#define UA_CLASS_XFER	0x01	/* M3UA Data transfer message */
#define UA_CLASS_SNMM	0x02	/* Signalling Network Mgmt (SNM) Messages */
#define UA_CLASS_ASPS	0x03	/* ASP State Maintenance (ASPSM) Messages */
#define UA_CLASS_ASPT	0x04	/* ASP Traffic Maintenance (ASPTM) Messages */
#define UA_CLASS_Q921	0x05	/* Q.931 User Part Messages */
#define UA_CLASS_MAUP	0x06	/* M2UA Messages */
#define UA_CLASS_CNLS	0x07	/* SUA Connectionless Messages */
#define UA_CLASS_CONS	0x08	/* SUA Connection Oriented Messages */
#define UA_CLASS_RKMM	0x09	/* Routing Key Management Messages */
#define UA_CLASS_TDHM	0x0a	/* TUA Dialog Handling Mesages */
#define UA_CLASS_TCHM	0x0b	/* TUA Component Handling Messages */

/*
 *  MESSAGES DEFINED IN EACH CLASS:-
 */
#define UA_MGMT_ERR		UA_MHDR(UA_VERSION, 0, UA_CLASS_MGMT, 0x00)
#define UA_MGMT_NTFY		UA_MHDR(UA_VERSION, 0, UA_CLASS_MGMT, 0x01)
#define UA_MGMT_LAST		0x01

#define UA_XFER_DATA		UA_MHDR(UA_VERSION, 0, UA_CLASS_XFER, 0x01)

#define UA_SNMM_DUNA		UA_MHDR(UA_VERSION, 0, UA_CLASS_SNMM, 0x01)
#define UA_SNMM_DAVA		UA_MHDR(UA_VERSION, 0, UA_CLASS_SNMM, 0x02)
#define UA_SNMM_DAUD		UA_MHDR(UA_VERSION, 0, UA_CLASS_SNMM, 0x03)
#define UA_SNMM_SCON		UA_MHDR(UA_VERSION, 0, UA_CLASS_SNMM, 0x04)
#define UA_SNMM_DUPU		UA_MHDR(UA_VERSION, 0, UA_CLASS_SNMM, 0x05)
#define UA_SNMM_DRST		UA_MHDR(UA_VERSION, 0, UA_CLASS_SNMM, 0x06)
#define UA_SNMM_LAST		0x06

#define UA_ASPS_ASPUP_REQ	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPS, 0x01)
#define UA_ASPS_ASPDN_REQ	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPS, 0x02)
#define UA_ASPS_HBEAT_REQ	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPS, 0x03)
#define UA_ASPS_ASPUP_ACK	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPS, 0x04)
#define UA_ASPS_ASPDN_ACK	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPS, 0x05)
#define UA_ASPS_HBEAT_ACK	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPS, 0x06)
#define UA_ASPS_LAST		0x06

#define UA_ASPT_ASPAC_REQ	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPT, 0x01)
#define UA_ASPT_ASPIA_REQ	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPT, 0x02)
#define UA_ASPT_ASPAC_ACK	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPT, 0x03)
#define UA_ASPT_ASPIA_ACK	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPT, 0x04)
#define UA_ASPT_LAST		0x04

#define UA_RKMM_REG_REQ		UA_MHDR(UA_VERSION, 0, UA_CLASS_RKMM, 0x01)
#define UA_RKMM_REG_RSP		UA_MHDR(UA_VERSION, 0, UA_CLASS_RKMM, 0x02)
#define UA_RKMM_DEREG_REQ	UA_MHDR(UA_VERSION, 0, UA_CLASS_RKMM, 0x03)
#define UA_RKMM_DEREG_RSP	UA_MHDR(UA_VERSION, 0, UA_CLASS_RKMM, 0x04)
#define UA_RKMM_LAST		0x04

#define UA_MHDR_SIZE (sizeof(uint32_t)*2)
#define UA_PHDR_SIZE (sizeof(uint32_t))
#define UA_MAUP_SIZE (UA_MHDR_SIZE + UA_PHDR_SIZE + sizeof(uint32_t))

#define UA_TAG_MASK		(htonl(0xffff0000))
#define UA_PTAG(__phdr)		((htonl(__phdr)>>16)&0xffff)
#define UA_PLEN(__phdr)		(htonl(__phdr)&0xffff)
#define UA_SIZE(__phdr)		(htonl(__phdr)&0xffff)
#define UA_TAG(__phdr)		((htonl(__phdr)>>16)&0xffff)
#define UA_PHDR(__phdr, __length) \
	(htonl(((__phdr)<<16)|((__length)+sizeof(uint32_t))))
#define UA_CONST_PHDR(__phdr, __length) \
	(htonl(((__phdr)<<16)|((__length)+sizeof(uint32_t))))

/*
 *  COMMON PARAMETERS:-
 *
 *  Common parameters per draft-ietf-sigtran-m2ua-10.txt
 *  Common parameters per draft-ietf-sigtran-m3ua-08.txt
 *  Common parameters per draft-ietf-sigtran-sua-07.txt
 *  Common parameters per rfc3057.txt
 *  -------------------------------------------------------------------
 */
#define UA_PARM_RESERVED	UA_CONST_PHDR(0x0000,0)
#define UA_PARM_IID		UA_CONST_PHDR(0x0001,sizeof(uint32_t))
#define UA_PARM_IID_RANGE	UA_CONST_PHDR(0x0002,0)	/* m2ua-10 */
#define UA_PARM_DATA		UA_CONST_PHDR(0x0003,0)	/* sua-07 */
#define UA_PARM_IID_TEXT	UA_CONST_PHDR(0x0003,0)
#define UA_PARM_INFO		UA_CONST_PHDR(0x0004,0)
#define UA_PARM_APC		UA_CONST_PHDR(0x0005,sizeof(uint32_t))	/* sua-07 */
#define UA_PARM_DLCI		UA_CONST_PHDR(0x0005,sizeof(uint32_t))	/* rfc3057 */
#define UA_PARM_RC		UA_CONST_PHDR(0x0006,sizeof(uint32_t))
#define UA_PARM_DIAG		UA_CONST_PHDR(0x0007,0)
#define UA_PARM_IID_RANGE1	UA_CONST_PHDR(0x0008,0)	/* rfc3057 */
#define UA_PARM_HBDATA		UA_CONST_PHDR(0x0009,0)
#define UA_PARM_REASON		UA_CONST_PHDR(0x000a,sizeof(uint32_t))
#define UA_PARM_TMODE		UA_CONST_PHDR(0x000b,sizeof(uint32_t))
#define UA_PARM_ECODE		UA_CONST_PHDR(0x000c,sizeof(uint32_t))
#define UA_PARM_STATUS		UA_CONST_PHDR(0x000d,sizeof(uint32_t))
#define UA_PARM_ASPID		UA_CONST_PHDR(0x000e,sizeof(uint32_t))
#define UA_PARM_PROT_DATA	UA_CONST_PHDR(0x000e,sizeof(uint32_t))	/* rfc3057 */
#define UA_PARM_CONG_LEVEL	UA_CONST_PHDR(0x000f,sizeof(uint32_t))	/* sua-07 */
#define UA_PARM_REL_REASON	UA_CONST_PHDR(0x000f,sizeof(uint32_t))	/* rfc3057 */
#define UA_PARM_TEI_STATUS	UA_CONST_PHDR(0x0010,sizeof(uint32_t))	/* rfc3057 */

/*
 *  Somewhat common field values:
 */
#define   UA_ECODE_INVALID_VERSION		(0x01)
#define   UA_ECODE_INVALID_IID			(0x02)
#define   UA_ECODE_UNSUPPORTED_MESSAGE_CLASS	(0x03)
#define   UA_ECODE_UNSUPPORTED_MESSAGE_TYPE	(0x04)
#define   UA_ECODE_UNSUPPORTED_TRAFFIC_MODE	(0x05)
#define   UA_ECODE_UNEXPECTED_MESSAGE		(0x06)
#define   UA_ECODE_PROTOCOL_ERROR		(0x07)
#define   UA_ECODE_UNSUPPORTED_IID_TYPE		(0x08)
#define   UA_ECODE_INVALID_STREAM_IDENTIFIER	(0x09)
#define  IUA_ECODE_UNASSIGNED_TEI		(0x0a)
#define  IUA_ECODE_UNRECOGNIZED_SAPI		(0x0b)
#define  IUA_ECODE_INVALID_TEI_SAPI_COMBINATION	(0x0c)
#define   UA_ECODE_REFUSED_MANAGEMENT_BLOCKING	(0x0d)
#define   UA_ECODE_ASPID_REQUIRED		(0x0e)
#define   UA_ECODE_INVALID_ASPID		(0x0f)
#define M2UA_ECODE_ASP_ACTIVE_FOR_IIDS		(0x10)
#define   UA_ECODE_INVALID_PARAMETER_VALUE	(0x11)
#define   UA_ECODE_PARAMETER_FIELD_ERROR	(0x12)
#define   UA_ECODE_UNEXPECTED_PARAMETER		(0x13)
#define   UA_ECODE_DESTINATION_STATUS_UNKNOWN	(0x14)
#define   UA_ECODE_INVALID_NETWORK_APPEARANCE	(0x15)
#define   UA_ECODE_MISSING_PARAMETER		(0x16)
#define   UA_ECODE_ROUTING_KEY_CHANGE_REFUSED	(0x17)
#define   UA_ECODE_INVALID_ROUTING_CONTEXT	(0x19)
#define   UA_ECODE_NO_CONFIGURED_AS_FOR_ASP	(0x1a)
#define   UA_ECODE_SUBSYSTEM_STATUS_UNKNOWN	(0x1b)

#define UA_STATUS_AS_DOWN			(0x00010001)
#define UA_STATUS_AS_INACTIVE			(0x00010002)
#define UA_STATUS_AS_ACTIVE			(0x00010003)
#define UA_STATUS_AS_PENDING			(0x00010004)
#define UA_STATUS_AS_INSUFFICIENT_ASPS		(0x00020001)
#define UA_STATUS_ALTERNATE_ASP_ACTIVE		(0x00020002)
#define UA_STATUS_ASP_FAILURE			(0x00020003)
#define UA_STATUS_AS_MINIMUM_ASPS		(0x00020004)

#define UA_TMODE_OVERRIDE			(0x1)
#define UA_TMODE_LOADSHARE			(0x2)
#define UA_TMODE_BROADCAST			(0x3)
#define UA_TMODE_SB_OVERRIDE			(0x4)
#define UA_TMODE_SB_LOADSHARE			(0x5)
#define UA_TMODE_SB_BROADCAST			(0x6)

#define UA_RESULT_SUCCESS			(0x00)
#define UA_RESULT_FAILURE			(0x01)

/* M3UA Specific parameters. */

#define M3UA_PARM_NTWK_APP	UA_CONST_PHDR(0x0200,sizeof(uint32_t))
#define M3UA_PARM_PROT_DATA1	UA_CONST_PHDR(0x0201,0)
#define M3UA_PARM_PROT_DATA2	UA_CONST_PHDR(0x0202,0)
#define M3UA_PARM_AFFECT_DEST	UA_CONST_PHDR(0x0203,sizeof(uint32_t))
#define M3UA_PARM_USER_CAUSE	UA_CONST_PHDR(0x0204,sizeof(uint32_t))
#define M3UA_PARM_CONG_IND	UA_CONST_PHDR(0x0205,sizeof(uint32_t))
#define M3UA_PARM_CONCERN_DEST	UA_CONST_PHDR(0x0206,sizeof(uint32_t))
#define M3UA_PARM_ROUTING_KEY	UA_CONST_PHDR(0x0207,0)
#define M3UA_PARM_REG_RESULT	UA_CONST_PHDR(0x0208,sizeof(uint32_t))
#define M3UA_PARM_DEREG_RESULT	UA_CONST_PHDR(0x0209,sizeof(uint32_t)*4)
#define M3UA_PARM_LOC_KEY_ID	UA_CONST_PHDR(0x020a,sizeof(uint32_t))
#define M3UA_PARM_DPC		UA_CONST_PHDR(0x020b,0)
#define M3UA_PARM_SI		UA_CONST_PHDR(0x020c,0)
#define M3UA_PARM_SSN		UA_CONST_PHDR(0x020d,0)
#define M3UA_PARM_OPC		UA_CONST_PHDR(0x020e,0)
#define M3UA_PARM_CIC		UA_CONST_PHDR(0x020f,0)
#define M3UA_PARM_PROT_DATA3	UA_CONST_PHDR(0x0210,0)	/* proposed */
#define M3UA_PARM_REG_STATUS	UA_CONST_PHDR(0x0212,sizeof(uint32_t))
#define M3UA_PARM_DEREG_STATUS	UA_CONST_PHDR(0x0213,sizeof(uint32_t))

struct ua_parm {
	union {
		uchar *cp;		/* pointer to parameter field */
		uint32_t *wp;		/* pointer to parameter field */
	};
	size_t len;			/* length of parameter field */
	uint32_t val;			/* value of first 4 bytes (host order) */
};

/**
 * ua_dec_parm: - extract parameter from message or field.
 * @beg: beginning of field or message
 * @eng: end of field or message
 * @parm: structure to hold result
 * @tag: parameter tag value
 *
 * If the parameter does not exist in the field or message it returns false; otherwise true, and
 * sets the parameter values if parm is non-NULL.
 *
 * A couple of problems with this function: it does not check if UA_SIZE is zero or less than
 * UA_PHDR_SIZE, both of which are syntax errors (well, a parameter of length zero and tag value
 * zero can be considered excessive padding).  It does not check if UA_SIZE for a given parameter
 * exceeds the given length, which is another syntax error.  The function should return an integer
 * value, zero on success, and an informative negative error number on failure.
 *
 * OTOH, do we really care if there is a syntax error in the message, provided that the necessary
 * information can be obtained?  No, probably not.  We can be forgiving.
 */
static bool
ua_dec_parm(uchar *beg, uchar *end, struct ua_parm *parm, uint32_t tag)
{
	uint32_t *wp;
	bool rtn = false;
	int plen;

	for (wp = (uint32_t *) beg;
	     (uchar *) (wp + 1) <= end && (plen = UA_SIZE(*wp)) >= UA_PHDR_SIZE;
	     wp = (uint32_t *) ((uchar *) wp + UA_PAD4(plen))) {
		if (UA_TAG(wp[0]) == UA_TAG(tag)) {
			rtn = true;
			if (parm) {
				parm->wp = (wp + 1);
				parm->len = plen;
				parm->val = ntohl(wp[1]);
			}
			break;
		}
	}
	return (rtn);

}

/*
 *  MTP-Provider to MTP-User primitives.
 *  ====================================
 */

/**
 * mtp_ok_ack: - issue MTP_OK_ACK primitive upstream
 * @mtp: MTP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @prim: correct primitive
 */
static inline fastcall int
mtp_ok_ack(struct mtp *mtp, queue_t *q, mblk_t *msg, mtp_long prim)
{
	struct MTP_ok_ack *p;
	mblk_t *mp;

	if ((mp = mtp_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_OK_ACK;
		p->mtp_correct_prim = prim;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(mtp->mid, mtp->sid, MTLOGTX, SL_TRACE, "<- MTP_OK_ACK");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mtp_error_ack: - issue MTP_ERROR_ACK primitive upstream
 * @mtp: MTP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @prim: error primitive
 * @error: negative UNIX, positive MTPI error
 */
static inline fastcall int
mtp_error_ack(struct mtp *mtp, queue_t *q, mblk_t *msg, mtp_long prim, mtp_long error)
{
	struct MTP_error_ack *p;
	mblk_t *mp;

	if ((mp = mtp_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_ERROR_ACK;
		p->mtp_error_primitive = prim;
		p->mtp_mtpi_error = error >= 0 ? error : MSYSERR;
		p->mtp_unix_error = error >= 0 ? 0 : -error;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(mtp->mid, mtp->sid, MTLOGTX, SL_TRACE, "<- MTP_ERROR_ACK");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mtp_bind_ack: - issue MTP_BIND_ACK primitive upstream
 * @mtp: MTP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @aptr: address pointer
 * @alen: address length
 * @flags: bind flags
 */
static inline fastcall int
mtp_bind_ack(struct mtp *mtp, queue_t *q, mblk_t *msg, caddr_t aptr, size_t alen, mtp_ulong flags)
{
	struct MTP_bind_ack *p;
	mblk_t *mp;

	if ((mp = mtp_allocb(q, sizeof(*p) + alen, BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_BIND_ACK;
		p->mtp_addr_length = alen;
		p->mtp_addr_offset = sizeof(*p);
		p->mtp_bind_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(aptr, mp->b_wptr, alen);
		mp->b_wptr += alen;
		freemsg(msg);
		strlog(mtp->mid, mtp->sid, MTLOGTX, SL_TRACE, "<- MTP_BIND_ACK");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mtp_addr_ack: - issue MTP_ADDR_ACK primitive upstream
 * @mtp: MTP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @lptr: local address pointer
 * @llen: local address length
 * @rptr: remote address pointer
 * @rlen: remote address length
 */
static inline fastcall int
mtp_addr_ack(struct mtp *mtp, queue_t *q, mblk_t *msg, caddr_t lptr, size_t llen, caddr_t rptr,
	     size_t rlen)
{
	struct MTP_addr_ack *p;
	mblk_t *mp;

	if ((mp = mtp_allocb(q, sizeof(*p) + llen + rlen, BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_ADDR_ACK;
		p->mtp_loc_length = llen;
		p->mtp_loc_offset = sizeof(*p);
		p->mtp_rem_length = rlen;
		p->mtp_rem_offset = sizeof(*p) + llen;
		mp->b_wptr += sizeof(*p);
		bcopy(lptr, mp->b_wptr, llen);
		mp->b_wptr += llen;
		bcopy(rptr, mp->b_wptr, rlen);
		mp->b_wptr += rlen;
		freemsg(msg);
		strlog(mtp->mid, mtp->sid, MTLOGTX, SL_TRACE, "<- MTP_ADDR_ACK");
		putnext(mtp->rq, mp);
		return (0);

	}
	return (-ENOBUFS);
}

/**
 * mtp_info_ack: - issue MTP_INFO_ACK primitive upstream
 * @mtp: MTP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @aptr: address pointer
 * @alen: address length
 */
static inline fastcall int
mtp_info_ack(struct mtp *mtp, queue_t *q, mblk_t *msg, caddr_t aptr, size_t alen)
{
	struct MTP_info_ack *p;
	mblk_t *mp;

	if ((mp = mtp_allocb(q, sizeof(*p) + alen, BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_INFO_ACK;
		p->mtp_msu_size = mtp->info.mtp_msu_size;
		p->mtp_addr_size = mtp->info.mtp_addr_size;
		p->mtp_addr_length = alen;
		p->mtp_addr_offset = sizeof(*p);
		p->mtp_current_state = mtp->info.mtp_current_state;
		p->mtp_serv_type = mtp->info.mtp_serv_type;
		p->mtp_version = mtp->info.mtp_version;
		mp->b_wptr += sizeof(*p);
		bcopy(aptr, mp->b_wptr, alen);
		mp->b_wptr += alen;
		freemsg(msg);
		strlog(mtp->mid, mtp->sid, MTLOGTX, SL_TRACE, "<- MTP_INFO_ACK");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mtp_optmgmt_ack: - issue MTP_OPTMGMT_ACK primitive upstream
 * @mtp: MTP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @optr: options pointer
 * @olen: options length
 * @flags: management flags
 */
static inline fastcall int
mtp_optmgmt_ack(struct mtp *mtp, queue_t *q, mblk_t *msg, caddr_t optr, size_t olen,
		mtp_ulong flags)
{
	struct MTP_optmgmt_ack *p;
	mblk_t *mp;

	if ((mp = mtp_allocb(q, sizeof(*p) + olen, BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_OPTMGMT_ACK;
		p->mtp_opt_length = olen;
		p->mtp_opt_offset = sizeof(*p);
		p->mtp_mgmt_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(optr, mp->b_wptr, olen);
		mp->b_wptr += olen;
		freemsg(msg);
		strlog(mtp->mid, mtp->sid, MTLOGTX, SL_TRACE, "<- MTP_OPTMGMT_ACK");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mtp_transfer_ind: - issue MTP_TRANSFER_IND primitive upstream
 * @mtp: MTP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @srce: source address
 * @pri: message priority
 * @sls: signalling link selection
 * @dp: user data
 */
static inline fastcall int
mtp_transfer_ind(struct mtp *mtp, queue_t *q, mblk_t *msg, struct mtp_addr *srce, mtp_ulong pri,
		 mtp_ulong sls, mblk_t *dp)
{
	struct MTP_transfer_ind *p;
	mblk_t *mp;

	if ((mp = mtp_allocb(q, sizeof(*p) + sizeof(*srce), BPRI_MED))) {
		if (canputnext(mtp->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mtp_primitive = MTP_TRANSFER_IND;
			p->mtp_srce_length = sizeof(*srce);
			p->mtp_srce_offset = sizeof(*p);
			p->mtp_mp = pri;
			p->mtp_sls = sls;
			mp->b_wptr += sizeof(*p);
			bcopy(srce, mp->b_wptr, sizeof(*srce));
			mp->b_wptr += sizeof(*srce);
			mp->b_cont = dp;
			freemsg(msg);
			strlog(mtp->mid, mtp->sid, MTLOGTX, SL_TRACE, "<- MTP_TRANSFER_IND");
			putnext(mtp->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * mtp_pause_ind: - issue MTP_PAUSE_IND primitive upstream
 * @mtp: MTP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @addr: address
 */
static inline fastcall int
mtp_pause_ind(struct mtp *mtp, queue_t *q, mblk_t *msg, struct mtp_addr *addr)
{
	struct MTP_pause_ind *p;
	mblk_t *mp;

	if ((mp = mtp_allocb(q, sizeof(*p) + sizeof(*addr), BPRI_MED))) {
		if (bcanputnext(mtp->rq, 3)) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 3;
			p = (typeof(p)) mp->b_wptr;
			p->mtp_primitive = MTP_PAUSE_IND;
			p->mtp_addr_length = sizeof(*addr);
			p->mtp_addr_offset = sizeof(*p);
			mp->b_wptr += sizeof(*p);
			bcopy(aptr, mp->b_wptr, sizeof(*addr));
			mp->b_wptr += sizeof(*addr);
			freemsg(msg);
			strlog(mtp->mid, mtp->sid, MTLOGTX, SL_TRACE, "<- MTP_PAUSE_IND");
			putnext(mtp->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * mtp_resume_ind: - issue MTP_RESUME_IND primitive upstream
 * @mtp: MTP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @addr: address
 */
static inline fastcall int
mtp_resume_ind(struct mtp *mtp, queue_t *q, mblk_t *msg, struct mtp_addr *addr)
{
	struct MTP_resume_ind *p;
	mblk_t *mp;

	if ((mp = mtp_allocb(q, sizeof(*p) + sizeof(*addr), BPRI_MED))) {
		if (bcanputnext(mtp->rq, 3)) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 3;
			p = (typeof(p)) mp->b_wptr;
			p->mtp_primitive = MTP_RESUME_IND;
			p->mtp_addr_length = sizeof(*addr);
			p->mtp_addr_offset = sizeof(*p);
			mp->b_wptr += sizeof(*p);
			bcopy(aptr, mp->b_wptr, sizeof(*addr));
			mp->b_wptr += sizeof(*addr);
			freemsg(msg);
			strlog(mtp->mid, mtp->sid, MTLOGTX, SL_TRACE, "<- MTP_RESUME_IND");
			putnext(mtp->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * mtp_status_ind: - issue MTP_STATUS_IND primitive upstream
 * @mtp: MTP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @addr: address
 * @type: status type
 * @status: the status
 */
static inline fastcall int
mtp_status_ind(struct mtp *mtp, queue_t *q, mblk_t *msg, struct mtp_addr *addr, mtp_ulong type,
	       mtp_ulong status)
{
	struct MTP_status_ind *p;
	mblk_t *mp;

	if ((mp = mtp_allocb(q, sizeof(*p) + sizeof(*addr), BPRI_MED))) {
		if (bcanputnext(mtp->rq, 3)) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 3;
			p = (typeof(p)) mp->b_wptr;
			p->mtp_primitive = MTP_STATUS_IND;
			p->mtp_addr_length = sizeof(*addr);
			p->mtp_addr_offset = sizeof(*p);
			p->mtp_type = type;
			p->mtp_status = status;
			mp->b_wptr += sizeof(*p);
			bcopy(aptr, mp->b_wptr, sizeof(*addr));
			mp->b_wptr += sizeof(*addr);
			freemsg(msg);
			strlog(mtp->mid, mtp->sid, MTLOGTX, SL_TRACE, "<- MTP_STATUS_IND");
			putnext(mtp->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * mtp_restart_begins_ind: - issue MTP_RESTART_BEGINS_IND primitive upstream
 * @q: active queue
 * @mtp: MTP private structure
 * @msg: message to free upon success
 */
static inline fastcall int
mtp_restart_begins_ind(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
	struct MTP_restart_begins_ind *p;
	mblk_t *mp;

	if ((mp = mtp_allocb(q, sizeof(*p) + alen, BPRI_MED))) {
		if (bcanputnext(mtp->rq, 3)) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 3;
			p = (typeof(p)) mp->b_wptr;
			p->mtp_primitive = MTP_RESTART_BEGINS_IND;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			strlog(mtp->mid, mtp->sid, MTLOGTX, SL_TRACE, "<- MTP_RESTART_BEGINS");
			putnext(mtp->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * mtp_restart_complete_ind: - issue MTP_RESTART_COMPLETE_IND primitive upstream
 * @mtp: MTP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
mtp_restart_complete_ind(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
	struct MTP_restart_complete_ind *p;
	mblk_t *mp;

	if ((mp = mtp_allocb(q, sizeof(*p) + alen, BPRI_MED))) {
		if (bcanputnext(mtp->rq, 3)) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 3;
			p = (typeof(p)) mp->b_wptr;
			p->mtp_primitive = MTP_RESTART_COMPLETE_IND;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			strlog(mtp->mid, mtp->sid, MTLOGTX, SL_TRACE, "<- MTP_RESTART_COMPLETE_IND");
			putnext(mtp->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  TS-User to TS-Provider primitives.
 *  ==================================
 */

/**
 * t_conn_req: - issue a T_CONN_REQ primitive downstream
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
t_conn_req(struct tp *tp, queue_t *q, mblk_t *msg, size_t dlen, caddr_t dptr, size_t olen,
	   caddr_t optr, mblk_t *dp)
{
	struct T_conn_req *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p) + dlen + olen, BPRI_MED)))) {
		if (likely(bcanputnext(tp->wq, 2))) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 2;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_CONN_REQ;
			p->DEST_length = dlen;
			p->DEST_offset = sizeof(*p);
			p->OPT_length = olen;
			p->OPT_offset = sizeof(*p) + dlen;
			mp->b_wptr += sizeof(*p);
			bcopy(dptr, mp->b_wptr, dlen);
			mp->b_wptr += dlen;
			bcopy(optr, mp->b_wptr, olen);
			mp->b_wptr += olen;
			mp->b_cont = dp;
			tp_set_i_state(tp, TS_WACK_CREQ);
			freemsg(msg);
			strlog(tp->mid, tp->sid, SLLOGTX, SL_TRACE, "T_CONN_REQ ->");
			putnext(tp->wq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * t_conn_res: - issue a T_CONN_RES primitive downstream
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @token: stream upon which to accept the connection
 * @olen: options length
 * @optr: options pointer
 * @sequence: sequence number of connect indication
 * @dp: user data
 */
static inline fastcall int
t_conn_res(struct tp *tp, queue_t *q, mblk_t *msg, t_scalar_t token, size_t olen, caddr_t optr,
	   t_scalar_t sequence, mblk_t *dp)
{
	struct T_conn_res *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p) + olen, BPRI_MED)))) {
		if (likely(bcanputnext(tp->wq, 2))) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 2;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_CONN_RES;
			p->ACCEPTOR_id = token;
			p->OPT_length = olen;
			p->OPT_offset = sizeof(*p);
			p->SEQ_number = sequence;
			mp->b_wptr += sizeof(*p);
			bcopy(optr, mp->b_wptr, olen);
			mp->b_wptr += olen;
			mp->b_cont = dp;
			tp_set_i_state(tp, TS_WACK_CRES);
			freemsg(msg);
			strlog(tp->mid, tp->sid, SLLOGTX, SL_TRACE, "T_CONN_RES ->");
			putnext(tp->wq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * t_discon_req: - issue a T_DISCON_REQ primitive downstream
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @sequence: sequence number of connect indication (or zero)
 * @dp: user data
 */
static inline fastcall int
t_discon_req(struct tp *tp, queue_t *q, mblk_t *msg, t_scalar_t sequence, mblk_t *dp)
{
	struct T_discon_req *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(bcanputnext(tp->wq, 2))) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 2;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_DISCON_REQ;
			p->SEQ_number = sequence;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			switch (tp_get_i_state(tp)) {
			case TS_WCON_CREQ:
				tp_set_i_state(tp, TS_WACK_DREQ6);
				break;
			case TS_WRES_CIND:
				tp_set_i_state(tp, TS_WACK_DREQ7);
				break;
			case TS_DATA_XFER:
				tp_set_i_state(tp, TS_WACK_DREQ9);
				break;
			case TS_WIND_ORDREL:
				tp_set_i_state(tp, TS_WACK_DREQ10);
				break;
			case TS_WREQ_ORDREL:
				tp_set_i_state(tp, TS_WACK_DREQ11);
				break;
			}
			freemsg(msg);
			strlog(tp->mid, tp->sid, SLLOGTX, SL_TRACE, "T_DISCON_REQ ->");
			putnext(tp->wq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * t_data_req: - issue a T_DATA_REQ primitive downstream
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @more: more flag
 * @dp: user data
 */
static inline fastcall int
t_data_req(struct tp *tp, queue_t *q, mblk_t *msg, t_scalar_t more, mblk_t *dp)
{
	struct T_data_req *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(bcanputnext(tp->wq, 0))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_DATA_REQ;
			p->MORE_flag = more;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			freemsg(msg);
			strlog(tp->mid, tp->sid, SLLOGTX, SL_TRACE, "T_DATA_REQ ->");
			putnext(tp->wq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * t_exdata_req: - issue a T_EXDATA_REQ primitive downstream
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @more: more flag
 * @dp: user data
 */
static inline fastcall int
t_exdata_req(struct tp *tp, queue_t *q, mblk_t *msg)
{
	struct T_exdata_req *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(bcanputnext(tp->wq, 1))) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_EXDATA_REQ;
			p->MORE_flag = more;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			freemsg(msg);
			strlog(tp->mid, tp->sid, SLLOGTX, SL_TRACE, "T_EXDATA_REQ ->");
			putnext(tp->wq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * t_info_req: - issue a T_INFO_REQ primitive downstream
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
t_info_req(struct tp *tp, queue_t *q, mblk_t *msg)
{
	struct T_info_req *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_INFO_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(tp->mid, tp->sid, SLLOGTX, SL_TRACE, "T_INFO_REQ ->");
		putnext(tp->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * t_bind_req: - issue a T_BIND_REQ primitive downstream
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @alen: address length
 * @aptr: address pointer
 */
static inline fastcall int
t_bind_req(struct tp *tp, queue_t *q, mblk_t *msg)
{
	struct T_bind_req *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p) + alen, BPRI_MED)))) {
		if (likely(canputnext(tp->wq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_BIND_REQ;
			p->ADDR_length = alen;
			p->ADDR_offset = sizeof(*p);
			mp->b_wptr += sizeof(*p);
			bcopy(aptr, mp->b_wptr, alen);
			mp->b_wptr += alen;
			if (tp_get_i_state(tp) == TS_IDLE)
				tp_set_i_state(tp, TS_WACK_BREQ);
			freemsg(msg);
			strlog(tp->mid, tp->sid, SLLOGTX, SL_TRACE, "T_BIND_REQ ->");
			putnext(tp->wq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * t_unbind_req: - issue a T_UNBIND_REQ primitive downstream
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
t_unbind_req(struct tp *tp, queue_t *q, mblk_t *msg)
{
	struct T_unbind_req *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(tp->wq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_UNBIND_REQ;
			mp->b_wptr += sizeof(*p);
			tp_set_i_state(tp, TS_WACK_UREQ);
			freemsg(msg);
			strlog(tp->mid, tp->sid, SLLOGTX, SL_TRACE, "T_UNBIND_REQ ->");
			putnext(tp->wq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * t_unitdata_req: - issue a T_UNITDATA_REQ primitive downstream
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @dlen: destination address length
 * @dptr: destination address pointer
 * @olen: options length
 * @optr: options pointer
 * @dp: user data
 */
static inline fastcall int
t_unitdata_req(struct tp *tp, queue_t *q, mblk_t *msg, size_t dlen, caddr_t dptr, size_t olen,
	       caddr_t optr, mblk_t *dp)
{
	struct T_unitdata_req *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p) + dlen + olen, BPRI_MED)))) {
		if (likely(bcanputnext(tp->wq, dp->b_band))) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = dp->b_band;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_UNITDATA_REQ;
			p->DEST_length = dlen;
			p->DEST_offset = sizeof(*p);
			p->OPT_length = olen;
			p->OPT_offset = sizeof(*p) + dlen;
			mp->b_wptr += sizeof(*p);
			bcopy(dptr, mp->b_wptr, dlen);
			mp->b_wptr += dlen;
			bcopy(optr, mp->b_wptr, olen);
			mp->b_wptr += olen;
			mp->b_cont = dp;
			freemsg(msg);
			strlog(tp->mid, tp->sid, SLLOGTX, SL_TRACE, "T_UNITDATA_REQ ->");
			putnext(tp->wq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * t_optmgmt_req: - issue a T_OPTMGMT_REQ primitive downstream
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @olen: options length
 * @optr: options pointer
 * @flags: management flags
 */
static inline fastcall int
t_optmgmt_req(struct tp *tp, queue_t *q, mblk_t *msg, size_t olen, caddr_t optr, t_scalar_t flags)
{
	struct T_optmgmt_req *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p) + olen, BPRI_MED)))) {
		if (likely(canputnext(tp->wq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_OPTMGMT_REQ;
			p->OPT_length = olen;
			p->OPT_offset = sizeof(*p);
			p->MGMT_flags = flags;
			mp->b_wptr += sizeof(*p);
			bcopy(optr, mp->b_wptr, olen);
			mp->b_wptr += olen;
			if (tp_get_i_state(tp) == TS_IDLE)
				tp_set_i_state(tp, TS_WACK_OPTREQ);
			freemsg(msg);
			strlog(tp->mid, tp->sid, SLLOGTX, SL_TRACE, "T_OPTMGMT_REQ ->");
			putnext(tp->wq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * t_ordrel_req: - issue a T_ORDREL_REQ primitive downstream
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @dp: user data
 */
static inline fastcall int
t_ordrel_req(struct tp *tp, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	struct T_ordrel_req *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(tp->wq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_ORDREL_REQ;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			switch (tp_get_i_state(tp)) {
			case TS_DATA_XFER:
				tp_set_i_state(tp, TS_WIND_ORDREL);
				break;
			case TS_WREQ_ORDREL:
				tp_set_i_state(tp, TS_IDLE);
				break;
			}
			freemsg(msg);
			strlog(tp->mid, tp->sid, SLLOGTX, SL_TRACE, "T_ORDREL_REQ ->");
			putnext(tp->wq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * t_optdata_req: - issue a T_OPTDATA_REQ primitive downstream
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @flag: data transfer flag
 * @sid: stream id
 * @dp: user data
 */
static inline fastcall int
t_optdata_req(struct tp *tp, queue_t *q, mblk_t *msg, t_scalar_t flag, t_uscalar_t sid, mblk_t *dp)
{
	struct T_optdata_req *p;
	mblk_t *mp;
	struct t_opthdr *oh;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p) + sizeof(*oh) + sizeof(sid), BPRI_MED)))) {
		if (likely(bcanputnext(tp->wq, dp->b_band))) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = dp->b_band;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_OPTDATA_REQ;
			p->DATA_flag = flag;
			p->OPT_length = sizeof(*oh) + sizeof(sid);
			p->OPT_offset = sizeof(*p);
			mp->b_wptr += sizeof(*p);
			oh = (typeof(oh)) mp->b_wptr;
			oh->level = T_INET_SCTP;
			oh->name = T_SCTP_SID;
			oh->len = sizeof(sid);
			mp->b_wptr += sizeof(*oh);
			*(t_uscalar_t *) mp->b_wptr = sid;
			mp->b_wptr += sizeof(sid);
			mp->b_cont = dp;
			freemsg(msg);
			strlog(tp->mid, tp->sid, SLLOGTX, SL_TRACE, "T_OPTDATA_REQ ->");
			putnext(tp->wq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * t_addr_req: - issue a T_ADDR_REQ primitive downstream
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
t_addr_req(struct tp *tp, queue_t *q, mblk_t *msg)
{
	struct T_addr_req *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(tp->wq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_ADDR_REQ;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			strlog(tp->mid, tp->sid, SLLOGTX, SL_TRACE, "T_ADDR_REQ ->");
			putnext(tp->wq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * t_capability_req: - issue a T_CAPABILITY_REQ primitive downstream
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @bits: flags
 */
static inline fastcall int
t_capability_req(struct tp *tp, queue_t *q, mblk_t *msg)
{
	struct T_capability_req *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_CAPABILITY_REQ;
		p->CAP_bits1 = bits;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(tp->mid, tp->sid, SLLOGTX, SL_TRACE, "T_CAPABILITY_REQ ->");
		putnext(tp->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  M3UA-AS to M3UA-G (Sent) M3UA Protocol Messages.
 *  ================================================
 */

/**
 * tp_send_mgmt_err: - send MGMT ERR message
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @ecode: error code
 * @dptr: diagnostics pointer
 * @dlen: diagnostics length
 */
static noinline fastcall __unlikely int
tp_send_mgmt_err(struct tp *tp, queue_t *q, mblk_t *msg, uint32_t ecode, caddr_t dptr, size_t dlen)
{
	int err;
	mblk_t *mp;
	size_t mlen =
	    UA_MHDR_SIZE + UA_SIZE(UA_PARM_ECODE) + dlen ? UA_SIZE(UA_PARM_DIAG) +
	    UA_PAD4(dlen) : 0;

	if (likely((mp = mtp_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_MGMT_ERR;
		p[1] = htonl(mlen);
		p[2] = UA_PARM_ECODE;
		p[3] = htonl(ecode);
		p += 4;
		if (dlen) {
			*p++ = UA_PHDR(UA_PARM_DIAG, dlen);
			bcopy(dptr, p, dlen);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(dlen);

		strlog(tp->mid, tp->sid, MTLOGTX, SL_TRACE, "MGMT ERR ->");
		/* send unordered and expedited on management stream 0 */
		if (unlikely((err = t_optdata_req(tp, q, msg, T_ODF_EX, 0, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * tp_send_asps_aspup_req: - send ASP Up
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @aspid: ASP identifier if not NULL
 * @iptr: Info pointer
 * @ilen: Info length
 */
static fastcall int
tp_send_asps_aspup_req(struct tp *tp, queue_t *q, mblk_t *msg, uint32_t *aspid, caddr_t iptr,
		       size_t ilen)
{
	int err;
	mblk_t *mp;
	size_t mlen =
	    UA_MHDR_SIZE + aspid ? UA_SIZE(UA_PARM_ASPID) : 0 + ilen ? UA_PHDR_SIZE +
	    UA_PAD4(ilen) : 0;

	if (likely((mp = mtp_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPS_ASPUP_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (aspid) {
			p[0] = UA_PARM_ASPID;
			p[1] = htonl(*aspid);
			p += 2;
		}
		if (ilen) {
			*p++ = UA_PHDR(UA_PARM_INFO, ilen);
			bcopy(iptr, p, ilen);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(ilen);
		tp_set_u_state(tp, ASP_WACK_ASPUP);
		mi_timer(tp->wack_aspup, 2000);

		strlog(tp->mid, tp->sid, MTLOGTX, SL_TRACE, "ASPS ASPUP Req ->");
		/* send unordered and expedited on stream 0 */
		if (unlikely((err = t_optdata_req(tp, q, msg, T_ODF_EX, 0, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * tp_send_asps_aspdn_req: - send ASP Down
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @aspid: ASP identifier if not NULL
 * @iptr: Info pointer
 * @ilen: Info length
 */
static fastcall int
tp_send_asps_aspdn_req(struct tp *tp, queue_t *q, mblk_t *msg, uint32_t *aspid, caddr_t iptr,
		       size_t ilen)
{
	int err;
	mblk_t *mp;
	size_t mlen =
	    UA_MHDR_SIZE + aspid ? UA_SIZE(UA_PARM_ASPID) : 0 + ilen ? UA_PHDR_SIZE +
	    UA_PAD4(ilen) : 0;

	if (likely((mp = mtp_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPS_ASPDN_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (aspid) {
			p[0] = UA_PARM_ASPID;
			p[1] = htonl(*aspid);
			p += 2;
		}
		if (ilen) {
			*p++ = UA_PHDR(UA_PARM_INFO, ilen);
			bcopy(iptr, p, ilen);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(ilen);
		tp_set_u_state(tp, ASP_WACK_ASPDN);
		mi_timer(tp->wack_aspdn, 2000);

		strlog(tp->mid, tp->sid, MTLOGTX, SL_TRACE, "ASPS ASPDN Req ->");
		/* send unordered and expedited on stream 0 */
		if (unlikely((err = t_optdata_req(tp, q, msg, T_ODF_EX, 0, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * tp_send_asps_hbeat_req: - send a BEAT message
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @hptr: heartbeat info pointer
 * @hlen: heartbeat info length
 *
 * This one is used for MGMT hearbeat not associated with any specific signalling link and,
 * therefore, sent on SCTP stream identifier 0 (management stream).
 */
fastcall int
tp_send_asps_hbeat_req(struct tp *tp, queue_t *q, mblk_t *msg, caddr_t hptr, size_t hlen)
{
	int err;
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE + (hlen ? UA_PHDR_SIZE + UA_PAD4(hlen) : 0);

	if (likely((mp = mtp_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPS_HBEAT_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (hlen) {
			*p++ = UA_PHDR(UA_PARM_HBDATA, hlen);
			bcopy(hptr, p, hlen);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(hlen);
		mi_timer(tp->wack_hbeat, 2000);
		/* send ordered on management stream 0 */

		strlog(tp->mid, tp->sid, MTLOGTX, SL_TRACE, "ASPS BEAT Req ->");
		if (unlikely((err = t_optdata_req(tp, q, msg, 0, 0, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * mtp_send_asps_hbeat_req: - send a BEAT message
 * @mtp: MTP private structure
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @hptr: heartbeat info pointer
 * @hlen: heartbeat info length
 *
 * This one is used for AS-SG heartbeat associated with a specific MTP-User and, therefore,
 * sent on the same SCTP stream identifier as is data for the MTP-User.
 */
fastcall int
mtp_send_asps_hbeat_req(struct mtp *mtp, struct tp *tp, queue_t *q, mblk_t *msg, caddr_t hptr, size_t hlen)
{
	int err;
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE +
		(hlen ? UA_PHDR_SIZE + UA_PAD4(hlen) : 0) +
		(mtp->rc ? UA_PHDR_SIZE + sizeof(uint32_t) : 0);

	if (likely((mp = mtp_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPS_HBEAT_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (mtp->rc) {
			p[0] = UA_PHDR(UA_PARM_RC, sizeof(uint32_t));
			p[1] = htonl(mtp->rc);
			p += 2;
		}
		if (hlen) {
			*p++ = UA_PHDR(UA_PARM_HBDATA, hlen);
			bcopy(hptr, p, hlen);
			p = (uint32_t *)((caddr_t) p + UA_PAD4(hlen));
		}
		mp->b_wptr = (unsigned char *) p;
		mi_timer(mtp->wack_hbeat, 2000);
		/* send ordered on specified stream */

		strlog(mtp->mid, mtp->sid, SLLOGTX, SL_TRACE, "ASPS BEAT Req ->");
		if (unlikely((err = t_optdata_req(tp, q, msg, 0, mtp->streamid, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * tp_send_asps_hbeat_ack: - send a BEAT Ack message
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @hptr: heartbeat data pointer
 * @hlen: heartbeat data length
 *
 * This one is used for ASP-SG heartbeat not associatedw with a specific signalling link and,
 * therefore, sent on SCTP stream identifier zero (0).
 */
static fastcall int
tp_send_asps_hbeat_ack(struct tp *tp, queue_t *q, mblk_t *msg, caddr_t hptr, size_t hlen)
{
	int err;
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE + (hlen ? UA_PHDR_SIZE + UA_PAD4(hlen) : 0);

	if (likely((mp = mtp_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPS_HBEAT_ACK;
		p[1] = htonl(mlen);
		p += 2;
		if (hlen) {
			*p++ = UA_PHDR(UA_PARM_HBDATA, hlen);
			bcopy(hptr, p, hlen);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(hlen);
		mi_timer(tp->wack_hbeat, 2000);
		/* send ordered on management stream 0 */

		strlog(tp->mid, tp->sid, MTLOGTX, SL_TRACE, "ASPS BEAT Req ->");
		if (unlikely((err = t_optdata_req(tp, q, msg, 0, 0, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * mtp_send_asps_hbeat_ack: - send a BEAT Ack message
 * @mtp: MTP private structure
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @hptr: heartbeat data pointer
 * @hlen: heartbeat data length
 *
 * This one is used for AS-SG hearbeat associated with a specific MTP-User and, therefore,
 * sent on the same SCTP stream identifier as is used for data (DATA) messages.
 */
static fastcall int
mtp_send_asps_hbeat_ack(struct mtp *mtp, struct tp *tp, queue_t *q, mblk_t *msg, caddr_t hptr, size_t hlen)
{
	int err;
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE + (hlen ? UA_PHDR_SIZE + UA_PAD4(hlen) : 0);

	if (likely((mp = mtp_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPS_HBEAT_ACK;
		p[1] = htonl(mlen);
		p += 2;
		if (hlen) {
			*p++ = UA_PHDR(UA_PARM_HBDATA, hlen);
			bcopy(hptr, p, hlen);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(hlen);
		mi_timer(mtp->wack_hbeat, 2000);
		/* send ordered on specified stream */

		strlog(mtp->mid, mtp->sid, MTLOGTX, SL_TRACE, "ASPS BEAT Req ->");
		if (unlikely((err = t_optdata_req(tp, q, msg, 0, mtp->streamid, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * mtp_send_aspt_aspac_req: - send an ASP Active Request
 * @mtp: MTP private structure
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @iptr: info pointer
 * @ilen: info length
 *
 * This is used to activate an AS (SL).  MTP-User are activated when they are enabled by the
 * MTP-User (MTP_BIND_REQ/MTP_CONN_REQ).  They are deactivated when they are disabled by the MTP-User
 * (MTP_UNBIND_REQ/MTP_DISCON_REQ).
 */
static int
mtp_send_aspt_aspac_req(struct mtp *mtp, struct tp *tp, queue_t *q, mblk_t *msg, caddr_t iptr, size_t ilen)
{
	int err;
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE +
	    (mtp->tmode ? UA_SIZE(UA_PARM_TMODE) : 0) +
	    (mtp->rc ? UA_PHDR_SIZE + sizeof(uint32_t) : 0) +
	    (ilen ? UA_PHDR_SIZE + UA_PAD4(ilen) : 0);

	if (likely((mp = mtp_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPT_ASPAC_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (mtp->tmode) {
			p[0] = UA_PARM_TMODE;
			p[1] = htonl(mtp->tmode);
			p += 2;
		}
		if (mtp->rc) {
			p[0] = UA_PHDR(UA_PARM_RC, sizeof(uint32_t));
			p[1] = htonl(mtp->rc);
			p += 2;
		}
		if (ilen) {
			*p++ = UA_PHDR(UA_PARM_INFO, ilen);
			bcopy(iptr, p, ilen);
			p = (uint32_t *)((caddr_t) p + UA_PAD4(ilen));
		}
		mp->b_wptr = (unsigned char *) p;
		mtp_set_u_state(mtp, ASP_WACK_ASPAC);
		mi_timer(mtp->wack_aspac, 2000);

		strlog(mtp->mid, mtp->sid, MTLOGTX, SL_TRACE, "ASPT ASPAC Req ->");
		/* always sent on same stream as data */
		if (unlikely((err = t_optdata_req(tp, q, msg, 0, mtp->streamid, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * tp_send_aspt_apsia_req: - send an ASP Inactive Request
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @rc: pointer to RC or NULL
 * @iptr: info pointer
 * @ilen: info length
 *
 * This version is disassociated with a given MTP-User (AS) and is used to request deactivation of
 * MTP-User (AS) for which messages have been received but for which there is no corresponding upper
 * stream.   This might happen when the upper stream closed non-gracefully and a message buffer
 * could not be allocated during close to deactivate the AS.  Also, it might happen if the SG is
 * foolish enough to send data (DATA) messages to an inactive (non-existent) AS.
 */
static int
tp_send_aspt_aspia_req(struct tp *tp, queue_t *q, mblk_t *msg, uint32_t *rc, caddr_t iptr, size_t ilen)
{
	int err;
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE +
	    (rc ? UA_PHDR_SIZE + sizeof(uint32_t) : 0) +
	    (tptr ? UA_PHDR_SIZE + UA_PAD4(tlen) : 0) + (iptr ? UA_PHDR_SIZE + UA_PAD4(ilen) : 0);

	if (likely((mp = mtp_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPT_ASPIA_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (rc) {
			p[0] = UA_PHDR(UA_PARM_RC, sizeof(uint32_t));
			p[1] = htonl(*rc);
			p += 2;
		}
		if (ilen) {
			*p++ = UA_PHDR(UA_PARM_INFO, ilen);
			bcopy(iptr, p, ilen);
			p = (uint32_t *) ((caddr_t) p + UA_PAD4(ilen));
		}
		mp->b_wptr = (unsigned char *) p;

		strlog(tp->mid, tp->sid, MTLOGTX, SL_TRACE, "ASPT ASPIA Req ->");
		/* always sent on stream 0 when no AS exists */
		if (unlikely((err = t_optdata_req(tp, q, msg, 0, 0, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * mtp_send_aspt_aspia_req: - send an ASP Inactive Request
 * @mtp: MTP private structure
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @iptr: info pointer
 * @ilen: info length
 *
 * This is used to deactivate an AS (MTP).  Signalling links are deactivated when they are disabled
 * by the MTP-User (MTP_UNBIND_REQ/MTP_DISCON_REQ).  They are activate when they are enabled by the
 * MTP-User (MTP_BIND_REQ/MTP_CONN_REQ).
 */
static int
mtp_send_aspt_aspia_req(struct mtp *mtp, struct tp *tp, queue_t *q, mblk_t *msg, caddr_t iptr, size_t ilen)
{
	int err;
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE +
	    (mtp->rc ? UA_PHDR_SIZE + sizeof(uint32_t) : 0) +
	    (ilen ? UA_PHDR_SIZE + UA_PAD4(ilen) : 0);

	if (likely((mp = mtp_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPT_ASPIA_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (mtp->rc) {
			p[0] = UA_PHDR(UA_PARM_RC, sizeof(uint32_t));
			p[1] = htonl(mtp->rc);
			p += 2;
		}
		if (ilen) {
			*p++ = UA_PHDR(UA_PARM_INFO, ilen);
			bcopy(iptr, p, ilen);
			p = (uint32_t *)((caddr_t) p + UA_PAD4(ilen));
		}
		mp->b_wptr = (unsigned char *) p;
		mtp_set_u_state(mtp, ASP_WACK_ASPIA);
		mi_timer(mtp->wack_aspia, 2000);

		strlog(mtp->mid, mtp->sid, MTLOGTX, SL_TRACE, "ASPT ASPIA Req ->");
		/* always sent on same stream as data */
		if (unlikely((err = t_optdata_req(tp, q, msg, 0, mtp->streamid, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * mtp_send_rkmm_reg_req: - send REG REQ
 * @mtp: MTP private structure
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @id: request identifier
 * @sdti: signalling data terminal identifier
 * @sdli: signalling data link identifer
 *
 * Used to register a PPA if the PPA to IID mapping is not statically assigned (using input-output
 * controls) and the SG is a Style II SG (that is, it supports registration requests).
 */
static int
mtp_send_rkmm_reg_req(struct mtp *mtp, struct tp *tp, queue_t *q, mblk_t *msg, uint32_t id, uint16_t sdti,
		     uint16_t sdli)
{
	int err;
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE + UA_SIZE(M2UA_PARM_LINK_KEY);

	if (likely((mp = mtp_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_RKMM_REG_REQ;
		p[1] = htonl(mlen);
		/* FIXME: make this for M3UA */
		p[2] = M2UA_PARM_LINK_KEY;
		p[3] = M2UA_PARM_LOC_KEY_ID;
		p[4] = htonl(id);
		p[5] = M2UA_PARM_SDTI;
		p[6] = htonl(sdti);
		p[7] = M2UA_PARM_SDLI;
		p[8] = htonl(sdli);
		p += 9;
		mp->b_wptr = (unsigned char *) p;

		strlog(mtp->mid, mtp->sid, MTLOGTX, SL_TRACE, "RKMM REG Req ->");
		if (unlikely((err = t_optdata_req(tp, q, msg, T_ODF_EX, 0, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * mtp_send_rkmm_dereg_req: - send DEREG REQ
 * @mtp: MTP private structure
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
mtp_send_rkmm_dereg_req(struct mtp *mtp, struct tp *tp, queue_t *q, mblk_t *msg)
{
	int err;
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE +
	    (mtp->rc ? UA_PHDR_SIZE + sizeof(uint32_t) : 0);

	if (likely((mp = mtp_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_RKMM_DEREG_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (mtp->rc) {
			p[0] = UA_PHDR(UA_PARM_RC, sizeof(uint32_t));
			p[1] = htonl(mtp->rc);
			p += 2;
		}
		mp->b_wptr = (unsigned char *) p;

		strlog(mtp->mid, mtp->sid, MTLOGTX, SL_TRACE, "RKMM DEREG Req ->");
		if (unlikely((err = t_optdata_req(tp, q, msg, T_ODF_EX, 0, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * mtp_send_xfer_data: - send XFER DATA
 * @mtp: MTP private structure
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @orig: originating address
 * @dest: destination address
 * @pri: message priority
 * @sls: signalling link selection
 * @dp: user data
 */
static int
mtp_send_xfer_data(struct mtp *mtp, struct tp *tp, queue_t *q, mblk_t *msg, struct mtp_addr *orig,
		   struct mtp_addr *dest, uint8_t pri, uint8_t sls, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	size_t dlen = msgdsize(dp) + 3 * sizeof(uint32_t);
	size_t mlen = UA_MHDR_SIZE + (mtp->rc ? UA_PHDR_SIZE + sizeof(uint32_t) : 0) + UA_PHDR_SIZE;

	if (likely((mp = mtp_allocb(q, mlen, BPRI_MED)))) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_XFER_DATA;
		p[1] = htonl(mlen + dlen);
		p += 2;
		if (mtp->rc) {
			p[0] = UA_PHDR(UA_PARM_RC, sizeof(uint32_t));
			p[1] = htonl(mtp->rc);
			p += 2;
		}
		p[0] = UA_PHDR(M3UA_PARM_PROT_DATA3, dlen);
		p[1] = htonl(orig->pc);
		p[2] = htonl(dest->pc);
		p[3] = htonl(((uint32_t) dest->si << 24) |
			     ((uint32_t) dest->ni << 16) |
			     ((uint32_t) pri << 8) | ((uint32_t) sls << 0)
		    );
		p += 4;
		mp->b_wptr = (unsigned char *) p;
		mp->b_cont = dp;

		strlog(mtp->mid, mtp->sid, MTLOGDA, SL_TRACE, "XFER DATA ->");
		if (unlikely((err = t_optdata_req(tp, q, msg, 0, mtp->streamid, 0, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * mtp_send_snmm_daud: - send SNMM DAUD
 * @mtp: MTP private structure
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @apc: affected point code
 * @iptr: info pointer
 * @ilen: info length
 */
static int
mtp_send_snmm_duna(struct mtp *mtp, struct tp *tp, queue_t *q, mblk_t *msg, uint32_t apc,
		   caddr_t iptr, size_t ilen)
{
	int err;
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE +
	    (mtp->rc ? UA_PHDR_SIZE + sizeof(uint32_t) : 0) +
	    UA_PDHR_SIZE + sizeof(uint32_t) + (ilen ? UA_PHDR_SIZE + UA_PAD4(ilen) : 0);

	if (likely((mp = mtp_allocb(q, mlen, BPRI_MED)))) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_SNMM_DAUD;
		p[1] = htonl(mlen);
		p += 2;
		if (mtp->rc) {
			p[0] = UA_PHDR(UA_PARM_RC, sizeof(uint32_t));
			p[1] = htonl(mtp->rc);
			p += 2;
		}
		p[0] = UA_PHDR(UA_PARM_APC, sizeof(uint32_t));
		p[1] = htonl(apc);
		p += 2;
		if (ilen) {
			*p++ = UA_PHDR(UA_PARM_INFO, ilen);
			bcopy(iptr, p, ilen);
			p = (uint32_t *) ((caddr_t) p + UA_PAD4(ilen));
		}
		mp->b_wptr = (unsigned char *) p;

		strlog(mtp->mid, mtp->sid, MTLOGTX, SL_TRACE, "SNMM DAUD ->");
		if (unlikely((err = t_optdata_req(tp, q, msg, 0, mtp->streamid, 0, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * mtp_send_snmm_scon: - send SNMM SCON
 * @mtp: MTP private structure
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @apc: affected point code
 * @cpc: concerned point code
 * @cong: congestion level or NULL
 * @iptr: info pointer
 * @ilen: info length
 */
static int
mtp_send_snmm_scon(struct mtp *mtp, struct tp *tp, queue_t *q, mblk_t *msg, uint32_t apc,
		   uint32_t cpc, uint32_t *cong, caddr_t iptr, size_t ilen)
{
	int err;
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE +
	    (mtp->rc ? UA_PDHR_SIZE + sizeof(uint32_t) : 0) +
	    UA_PHDR_SIZE + sizeof(uint32_t) +
	    UA_PHDR_SIZE + sizeof(uint32_t) +
	    (cong ? UA_PHDR_SIZE + sizeof(uint32_t) : 0) +
	    (ilen ? UA_PHDR_SIZE + UA_PAD4(ilen) : 0);

	if (likely(mp = mtp_allocb(q, mlen, BPRI_MED))) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_SNMM_SCON;
		p[1] = htonl(mlen);
		p += 2;
		if (mtp->rc) {
			p[0] = UA_PHDR(UA_PARM_RC, sizeof(uint32_t));
			p[1] = htonl(mtp->rc);
			p += 2;
		}
		p[0] = UA_PHDR(UA_PARM_APC, sizeof(uint32_t));
		p[1] = htonl(apc);
		p[2] = UA_PHDR(M3UA_PARM_CONCERN_DEST, sizeof(uint32_t));
		p[3] = htonl(cpc);
		p += 4;
		if (cong) {
			p[0] = UA_PHDR(M3UA_PARM_CONG_IND, sizeof(uint32_t));
			p[1] = htonl(*cong);
			p += 2;
		}
		if (ilen) {
			*p++ = UA_PHDR(UA_PARM_INFO, ilen);
			bcopy(iptr, p, ilen);
			p = (uint32_t *) ((caddr_t) p + UA_PAD4(ilen));
		}
		mp->b_wptr = (unsigned char *) p;

		strlog(mtp->mid, mtp->sid, MTLOGTX, SL_TRACE, "SNMM SCON ->");
		if (unlikely((err = t_optdata_req(tp, q, msg, 0, mtp->streamid, 0, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}


/*
 *  M3UA-SG to M3UA-AS Received Messages.
 *  =====================================
 */

/**
 * mtp_lookup: - lookup an AS from message
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static struct mtp *
mtp_lookup(struct tp *tp, queue_t *q, mblk_t *mp, int *errp)
{
	struct ua_parm rc = { {NULL,}, };
	struct mtp *mtp = NULL;

	if (ua_dec_parm(mp->b_rptr, mp->b_wptr, &rc, UA_PARM_RC)) {
		for (mtp = tp->mtp.list; mtp; mtp = mtp->tp.next)
			if (mtp->rc == rc.val)
				goto found;
	}
	/* if there is precisely one MTP-User, we do not need RC to identify the AS */
	if ((mtp = tp->mtp.list) && !mtp->tp.next)
		goto found;
	if (errp)
		*errp = -ESRCH;	/* could not find AS */
	return (NULL);
      found:
	if (errp)
		*errp = 0;
	return (mtp);
}

/**
 * mtp_recv_mgmt_err: - receive MGMT ERR message
 * @mtp: MTP private structure
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 *
 * Error messages pertaining to an AS or AS state must contain an RC.
 */
static int
mtp_recv_mgmt_err(struct mtp *mtp, struct tp *tp, queue_t *q, mblk_t *mp)
{
	/* FIXME */
	freemsg(mp);
	return (0);
}

/**
 * tp_recv_mgmt_err: - receive MGMT ERR message
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
tp_recv_mgmt_err(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct mtp *mtp;

	if ((mtp = mtp_lookup(tp, q, mp, NULL)))
		return mtp_recv_mgmt_err(mtp, tp, q, mp);

	/* FIXME: process as without RC */
	freemsg(mp);
	return (0);
}

/**
 * mtp_recv_mgmt_ntfy: - receive MGMT NTFY message
 * @mtp: MTP private structure
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 *
 * Notification messages pertaning to an AS or AS state change must contain an IID.
 */
static int
mtp_recv_mgmt_ntfy(struct mtp *mtp, struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct ua_parm status;
	int ecode;

	if (!ua_dec_parm(mp->b_rptr, mp->b_wptr, &status, UA_PARM_STATUS))
		goto missing_parm;
	switch (status.val) {
	case UA_STATUS_AS_DOWN:
	case UA_STATUS_AS_INACTIVE:
	case UA_STATUS_AS_ACTIVE:
	case UA_STATUS_AS_PENDING:
	case UA_STATUS_AS_INSUFFICIENT_ASPS:
	case UA_STATUS_ASP_FAILURE:
	case UA_STATUS_AS_MINIMUM_ASPS:
	default:
		/* All of these are just advisory.  For now we discard them.  Later we should
		   generate and MTP_EVENT_IND with an appropriate event. */
		goto discard;
	case UA_STATUS_ALTERNATE_ASP_ACTIVE:
		/* In the case of an Override mode AS, reception of an ASP Active message at an SGP
		   causes the (re)direction of all traffic for the AS to the ASP that sent the ASP
		   Active message.  Any previously active ASP in the AS is now considered to be in
		   the state ASP Inactive and SHOULD no longer receive traffic from the SGP within
		   the AS.  The SGP MUST send a Notify message ("Alternate ASP Active") to the
		   previously active ASP in the AS, and SHOULD stop traffic to/from that ASP.  The
		   ASP receiving this Notify MUST consider itself now in the ASP-INACTIVE state, it
		   it is not already aware of this via inter-ASP communication with the Overriding
		   ASP. */
		switch (mtp_get_u_state(mtp)) {
		case ASP_INACTIVE:
		case ASP_WACK_ASPUP:
		case ASP_WACK_ASPDN:
		case ASP_DOWN:
			goto discard;
		case ASP_WACK_ASPIA:
			mtp_set_u_state(mtp, ASP_INACTIVE);
			if (mtp_get_i_state(mtp) == MTPS_WACK_DREQ6)
				return mtp_ok_ack(mtp, q, mp, MTP_DISCON_REQ);
			goto outstate;
		case ASP_WACK_ASPAC:
			mtp_set_u_state(mtp, ASP_INACTIVE);
			if (mtp_get_i_state(mtp) == MTPS_WACK_CREQ)
				return mtp_ok_ack(mtp, q, mp, MTP_CONN_REQ);
			if (mtp_get_i_state(mtp) == MTPS_WACK_BREQ)
				return mtp_bind_ack(mtp, q, mp);
			goto outstate;
		case ASP_ACTIVE:
			mtp_set_u_state(mtp, ASP_INACTIVE);
			if (mtp_get_i_state(mtp) == MTPS_CONNECTED)
				return mtp_error_ind(mtp, q, mp);
			if (mtp_get_i_state(mtp) == MTPS_IDLE)
				return mtp_error_ind(mtp, q, mp);
			goto outstate;
		default:
		case ASP_UP:
			goto outstate;
		}
	      outstate:
		strlog(mtp->mid, mtp->sid, 0, SL_ERROR,
		       "Alternate ASP Active in invalid state %u:%u",
		       mtp_get_i_state(mtp), mtp_get_u_state(mtp));
		mtp_set_u_state(mtp, ASP_DOWN);
		return mtp_error_ind(mtp, q, mp);
	}
	/* FIXME */
      discard:
	freemsg(mp);
	return (0);
      missing_parm:
	ecode = UA_ECODE_MISSING_PARAMETER;
	goto error;
      error:
	return tp_send_mgmt_err(tp, q, mp, ecode, mp->b_rptr, mp->b_wptr - mp->b_rptr);
}

/**
 * tp_recv_mgmt_ntfy: - receive MGMT NTFY message
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
tp_recv_mgmt_ntfy(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct ua_parm status;
	struct mtp *mtp;
	int ecode;

	if ((mtp = mtp_lookup(tp, q, mp, NULL)))
		return mtp_recv_mgmt_ntfy(mtp, tp, q, mp);

	if (!ua_dec_parm(mp->b_rptr, mp->b_wptr, &status, UA_PARM_STATUS))
		goto missing_parm;
	switch (status.val) {
	case UA_STATUS_AS_DOWN:
	case UA_STATUS_AS_INACTIVE:
	case UA_STATUS_AS_ACTIVE:
	case UA_STATUS_AS_PENDING:
	case UA_STATUS_AS_INSUFFICIENT_ASPS:
	case UA_STATUS_AS_MINIMUM_ASPS:
	case UA_STATUS_ALTERNATE_ASP_ACTIVE:
		/* all of the above must have an IID unless there is precisely one AS */
		goto missing_parm;
	default:
	case UA_STATUS_ASP_FAILURE:
		/* TODO: eventually we should provide a signalling link notification event that the
		 * ASP with the associated ASP Id has failed.  For now, just discard. */
		goto discard;
	}
	/* FIXME: process as without RC */
      discard:
	freemsg(mp);
	return (0);
      missing_parm:
	ecode = UA_ECODE_MISSING_PARAMETER;
	goto error;
      error:
	return tp_send_mgmt_err(tp, q, mp, ecode, mp->b_rptr, mp->b_wptr - mp->b_rptr);
}

/**
 * tp_recv_asps_hbeat_req: - process BEAT Request
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 *
 * When processing a heartbeat, if there is an IID in the message or there is precisely one AS for
 * the ASP, respond to the heartbeat on the stream associated with the specified AS.  If there is no
 * IID in the message, respond to the heartbeat on stream 0 as a mangement heartbeat.
 *
 * This is not specifically according to the spec, but is consistent with the correlation id and
 * heartbeat procedures for UAs.  When an OpenSS7 AS or SG generates an AS-related heartbeat, it
 * includes the IID in the message.
 */
static int
tp_recv_asps_hbeat_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct ua_parm hbeat;
	struct mtp *mtp;

	if (!ua_dec_parm(mp->b_rptr, mp->b_wptr, &hbeat, UA_PARM_HBDATA))
		return (-EINVAL);

	if ((mtp = mtp_lookup(tp, q, mp, NULL)))
		return mtp_send_asps_hbeat_ack(mtp, tp, q, mp, hbeat.cp, hbeat.len);

	/* process as normal */
	return tp_send_asps_hbeat_ack(tp, q, mp, hbeat.cp, hbeat.len);
}

/**
 * tp_recv_asps_aspup_ack: - process ASP Up Ack message
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
tp_recv_asps_aspup_ack(struct tp *tp, queue_t *q, mblk_t *mp)
{
	int ecode;

	if (tp_get_i_state(tp) != TS_DATA_XFER)
		goto outstate;
	if (tp_get_u_state(tp) != ASP_WACK_ASPUP)
		goto outstate;
	tp_set_u_state(tp, ASP_UP);
	freemsg(mp);
	return (0);
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "ASPUP Ack in unexpected state %u:%u",
	       tp_get_i_state(tp), tp_get_u_state(tp));
	ecode = UA_ECODE_UNEXPECTED_MESSAGE;
	goto error;
      error:
	return tp_send_mgmt_err(tp, q, mp, ecode, mp->b_rptr, mp->b_wptr - mp->b_rptr);
}

/**
 * tp_recv_asps_aspdn_ack: - process ASP Down Ack message
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 *
 * ASPDN Ack can be solicited or unsolicited indicating NIF isolation.  When it was solicited, we
 * are in the ASP_WACK_ASPDN state and we complete the ASP_DOWN procedure. ASPDN Ack is only
 * solicited when we are shutting down an SCTP transport, therefore, the last step in the procedure
 * is an orderly disconnect.
 */
static int
tp_recv_asps_aspdn_ack(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct mtp *mtp;
	int err = 0;

	if (tp_get_i_state(tp) != TS_DATA_XFER)
		goto outstate;
	switch (tp_get_u_state(tp)) {
	case ASP_UP:
	case ASP_WACK_ASPUP:
	case ASP_WACK_ASPDN:
		tp_set_u_state(tp, ASP_DOWN);
		write_lock(&mtp_mux_lock);
		for (mtp = tp->mtp.list; mtp; mtp = mtp->tp.next) {
			if (tp->flags & M2UA_SG_DYNAMIC) {
				if ((err = lmi_error_ind(mtp, q, mp, LMI_DISC, LMI_UNATTACHED)))
					break;
				if ((*mtp->tp.prev = mtp->tp.next))
					mtp->tp.next->tp.prev = mtp->tp.prev;
				mtp->tp.tp = NULL;
				mtp->tp.next = NULL;
				mtp->tp.prev = &mtp->tp.next;
			} else {
				switch (mtp_get_i_state(mtp)) {
				case LMI_DETACH_PENDING:
				case LMI_ATTACH_PENDING:
				case LMI_UNATTACHED:
				default:
					strlog(mtp->mid, mtp->sid, 0, SL_ERROR,
					       "%s: unexpected in mtp state %u:%u",
					       __FUNCTION__, mtp_get_i_state(mtp), mtp_get_u_state(mtp));
					/* fall through */
				case LMI_ENABLED:
				case LMI_ENABLE_PENDING:
				case LMI_DISABLE_PENDING:
					if ((err =
					     lmi_error_ind(mtp, q, mp, LMI_DISC, LMI_DISABLED)))
						break;
					continue;
				case LMI_DISABLED:
					/* probably already disabled on a previous pass */
					continue;
				}
				break;
			}
		}
		write_unlock(&mtp_mux_lock);
		if (err)
			return (err);
		switch (tp_get_u_state(tp)) {
		case ASP_UP:
		case ASP_WACK_ASPUP:
			tp_set_u_state(tp, ASP_DOWN);
			/* try to bring ASP back up */
			return tp_send_asps_aspup_req(tp, q, mp, tp->aspid ? &tp->aspid : NULL, NULL, 0);
		case ASP_WACK_ASPDN:
			tp_set_u_state(tp, ASP_DOWN);
			/* continue disconnection */
			return t_ordrel_req(tp, q, mp, NULL);
		}
	case ASP_DOWN:
		goto discard;
	default:
		goto outstate;
	}
      discard:
	err = 0;
	goto error;
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "ASPDN Ack in unexpected state %u:%u",
	       tp_get_i_state(tp), tp_get_u_state(tp));
	err = -EINVAL;		/* unexpected message */
	goto error;
      error:
	freemsg(mp);
	return (err);
}

/**
 * mtp_recv_asps_hbeat_ack: - receive BEAT Ack message
 * @mtp: MTP private structure
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 *
 * Note that BEAT Ack messages in reply to a BEAT message that contains an RC should also contain
 * an RC.  This is not strictly to spec, but it is what the OpenSS7 stack does.
 */
static int
mtp_recv_asps_hbeat_ack(struct mtp *mtp, struct tp *tp, queue_t *q, mblk_t *mp)
{
	/* FIXME */
	freemsg(mp);
	return (0);
}

/**
 * tp_recv_asps_hbeat_ack: - receive BEAT Ack message
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
tp_recv_asps_hbeat_ack(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct mtp *mtp;

	if ((mtp = mtp_lookup(tp, q, mp, NULL)))
		return mtp_recv_asps_hbeat_ack(mtp, tp, q, mp);

	/* If the message does not include an IID, then perhaps the IID is contained in the
	   heartbeat data.  The OpenSS7 stack formats heatbeat data with TLVs in the same manner as
	   normal messages. */
	/* FIXME: process as without IID */
	freemsg(mp);
	return (0);
}

/**
 * mtp_recv_aspt_aspac_ack: - receive ASPAC Ack message
 * @mtp: MTP private structure
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
mtp_recv_aspt_aspac_ack(struct mtp *mtp, struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct ua_parm tmode = { {NULL,}, };
	int ecode;

	if (mtp_get_u_state(mtp) == ASP_ACTIVE)
		goto discard;
	if (mtp_get_u_state(mtp) != ASP_WACK_ASPAC)
		goto outstate;
	if (mtp_get_i_state(mtp) != LMI_ENABLE_PENDING)
		goto outstate;
	mtp_set_u_state(mtp, ASP_ACTIVE);
	if (ua_dec_parm(mp->b_rptr, mp->b_wptr, &tmode, UA_PARM_TMODE))
		mtp->tmode = tmode.val;
	return lmi_enable_con(mtp, q, mp);
      outstate:
	ecode = UA_ECODE_UNEXPECTED_MESSAGE;
	goto error;
      error:
	return tp_send_mgmt_err(tp, q, mp, ecode, mp->b_rptr, mp->b_wptr - mp->b_rptr);
      discard:
	freemsg(mp);
	return (0);
}

/**
 * tp_recv_apsac_ack: - receive ASPAC Ack message
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
tp_recv_aspt_aspac_ack(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct mtp *mtp;
	int ecode;

	if ((mtp = mtp_lookup(tp, q, mp, NULL)))
		return mtp_recv_aspt_aspac_ack(mtp, tp, q, mp);

	goto protocol_error;
	/* An ASPAC Ack with no IID is supposed to pertain to all AS configurated for the ASP.
	   However, we never send an ASPAC Req without an IID and so it is an error to send an
	   ASPAC Ack without an IID. */
      protocol_error:
	ecode = UA_ECODE_PROTOCOL_ERROR;
	goto error;
      error:
	return tp_send_mgmt_err(tp, q, mp, ecode, mp->b_rptr, mp->b_wptr - mp->b_rptr);
}

/**
 * mtp_recv_aspt_aspia_ack: - receive ASPIA Ack message
 * @mtp: MTP private structure
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 *
 * ASPIA Acks with an IID can be either solicited or unsolicited.  Unsolicited ASPIA Acks are
 * detected when they are received in the incorrect state.  Solicited ASPIA Acks are received in the
 * ASP_WACK_ASPIA state.   Nevertheless, we may have sent multiple ASPIA Requests if there was an
 * ack timeout.  Multiple ASPIA Acks may be returned.  Therefore, if we are already in an
 * ASP_INACTIVE state, the messge is ignored.
 */
static int
mtp_recv_aspt_aspia_ack(struct mtp *mtp, struct tp *tp, queue_t *q, mblk_t *mp)
{
	switch (mtp_get_u_state(mtp)) {
	case ASP_INACTIVE:
	case ASP_WACK_ASPUP:
	case ASP_WACK_ASPDN:
	case ASP_DOWN:
	case ASP_WACK_ASPAC:
		goto ignore;
	case ASP_ACTIVE:
		/* unsolicited */
		mtp_set_u_state(mtp, ASP_INACTIVE);
		if (mtp_get_i_state(mtp) == LMI_ENABLED)
			/* Note that another possibility here would be to issue a local processor
			   outage indication, but the SG should be smart enough to do that when it
			   is applicable. */
			return lmi_error_ind(mtp, q, mp, LMI_DISC, LMI_DISABLED);
		goto outstate;
	case ASP_WACK_ASPIA:
		/* solicited */
		mtp_set_u_state(mtp, ASP_INACTIVE);
		if (mtp_get_i_state(mtp) == LMI_DISABLE_PENDING)
			return lmi_disable_con(mtp, q, mp);
		goto outstate;
	default:
	case ASP_UP:
		goto outstate;
	}
      outstate:
	strlog(mtp->mid, mtp->sid, 0, SL_ERROR,
	       "APSIA Ack received in invalid state %u:%u", mtp_get_i_state(mtp),
	       mtp_get_u_state(mtp));
	mtp_set_u_state(mtp, ASP_DOWN);
	return lmi_error_ind(mtp, q, mp, LMI_FATALERR, LMI_UNUSABLE);
      ignore:
	freemsg(mp);
	return (0);
}

/**
 * tp_recv_aspt_apia_ack: - receive ASPIA Ack message
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 *
 * There are two types of ASPIA Acks: solicited and unsolicited.  An unsolicited ASPIA Ack indicates
 * some form of isolation between the M2UA and NIF at the SG.  Solicited ASPIA Acks have a
 * corresponding ASPIA Req.  Because we never send ASPIA Req without an IID, receiving a solicited
 * ASPIA Ack without an IID is an error.  ASPIA Acks without an IID can therefore always be
 * considered to be unsolicited.
 */
static int
tp_recv_aspt_aspia_ack(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct mtp *mtp;
	int err = 0;

	if ((mtp = mtp_lookup(tp, q, mp, &err)))
		return mtp_recv_aspt_aspia_ack(mtp, tp, q, mp);

	/* ASPIA Acks without IIDs are always unsolicited.  Also, ASPIA Acks without IIDS apply to
	   all AS served. */
	for (mtp = tp->mtp.list; mtp; mtp = mtp->tp.next) {
		switch (mtp_get_u_state(mtp)) {
		case ASP_DOWN:
		case ASP_WACK_ASPDN:
		case ASP_WACK_ASPUP:
		case ASP_UP:
		case ASP_INACTIVE:
			continue;
		case ASP_WACK_ASPIA:
		case ASP_WACK_ASPAC:
		case ASP_ACTIVE:
			mtp_set_u_state(mtp, ASP_INACTIVE);
			if ((err = lmi_error_ind(mtp, q, NULL, LMI_DISC, LMI_DISABLED)))
				break;
			continue;
		default:
			mtp_set_u_state(mtp, ASP_DOWN);
			if ((err = lmi_error_ind(mtp, q, NULL, LMI_FATALERR, LMI_UNUSABLE)))
				break;
			continue;
		}
	}
	if (err == 0)
		freemsg(mp);
	return (err);
}

/**
 * mtp_recv_xfer_data: - receive XFER DATA message
 * @mtp: MTP private structure
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static inline __hot_in int
mtp_recv_xfer_data(struct mtp *mtp, struct tp *tp, queue_t *q, mblk_t *mp)
{
	int err;
	struct ua_parm data;
	struct mtp_addr srce;
	int pri, sls;
	unsigned char *b_rptr = mp->b_rptr;

	if (mtp_get_i_state(mtp) == MTPS_UNBND)
		goto outstate;
	if (!ua_dec_parm(mp->b_rptr, mp->b_wptr, &data, M3UA_PARM_PROT_DATA3))
		goto missing;
	srce.pc = ntohl(data.wp[0]);
	srce.ni = (ntohl(data.wp[2]) >> 16) & 0xff;
	srce.si = (htohl(data.wp[2]) >> 24) & 0xff;
	pri = (htonl(data.wp[2]) >> 8) & 0xff;
	sls = (htonl(data.wp[2]) >> 0) & 0xff;
	mp->b_rptr = (unsigned char *) &data.wp[3];
	if ((err = mtp_transfer_ind(mtp, q, mp, &srce, pri, sls, mp)) == 0)
		return (0);
	mp->b_rptr = b_rptr;
	return (err);
      missing:
	strlog(mtp->mid, mtp->sid, 0, SL_ERROR, "XFER DATA missing PROT DATA3 parameter");
	err = -ENXIO;		/* missing mandatory parameter */
	goto error;
      outstate:
	strlog(mtp->mid, mtp->sid, 0, SL_ERROR, "XFER DATA in unexpected state %u:%u",
	       mtp_get_i_state(mtp), mtp_get_u_state(mtp));
	err = -EINVAL;		/* unexpected message */
	goto error;
      error:
	freemsg(mp);
	return (err);

}

/**
 * tp_recv_xfer_data: - receive XFER DATA message
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static inline __hot_in int
tp_recv_xfer_data(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct mtp *mtp;

	if ((mtp = mtp_lookup(tp, q, mp, NULL)))
		return mtp_recv_xfer_data(mtp, tp, q, mp);
	return (-ESRCH);
}

/**
 * mtp_recv_snmm_duna: - receive SNMM DUNA message
 * @mtp: MTP private structure
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
mtp_recv_snmm_duna(struct mtp *mtp, struct tp * tp, queue_t *q, mblk_t *mp)
{
	int err;
	struct ua_parm apc = { {NULL,}, };
	struct mtp_addr addr;

	if (mtp_get_i_state(mtp) == MTPS_UNBND)
		goto outstate;
	if (!ua_dec_parm(mp->b_rptr, mp->b_wptr, &apc, UA_PARM_APC))
		goto missing;
	addr.pc = apc.val;
	addr.si = 0;
	addr.ni = 0;
	/* FIXME: sanity check value */
	return mtp_pause_ind(mtp, q, mp, &addr);
      missing:
	strlog(mtp->mid, mtp->sid, 0, SL_ERROR, "SNMM DUNA missing APC parameter");
	err = -ENXIO;		/* missing mandatory parameter */
	goto error;
      outstate:
	strlog(mtp->mid, mtp->sid, 0, SL_ERROR, "SNMM DUNA in unexpected state %u:%u",
	       mtp_get_i_state(mtp), mtp_get_u_state(mtp));
	err = -EINVAL;		/* unexpected message */
	goto error;
      error:
	freemsg(mp);
	return (err);
}

/**
 * mtp_recv_snmm_dava: - receive SNMM DAVA message
 * @mtp: MTP private structure
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
mtp_recv_snmm_dava(struct mtp *mtp, stuct tp * tp, queue_t *q, mblk_t *mp)
{
	int err;
	struct ua_parm apc = { {NULL,}, };
	struct mtp_addr addr;

	if (mtp_get_i_state(mtp) == MTPS_UNBND)
		goto outstate;
	if (!ua_dec_parm(mp->b_rptr, mp->b_wptr, &apc, UA_PARM_APC))
		goto missing;
	addr.pc = apc.val;
	addr.si = 0;
	addr.ni = 0;
	/* FIXME: sanity check value */
	return mtp_resume_ind(mtp, q, mp, &addr);
      missing:
	strlog(mtp->mid, mtp->sid, 0, SL_ERROR, "SNMM DAVA missing APC parameter");
	err = -ENXIO;		/* missing mandatory parameter */
	goto error;
      outstate:
	strlog(mtp->mid, mtp->sid, 0, SL_ERROR, "SNMM DAVA in unexpected state %u:%u",
	       mtp_get_i_state(mtp), mtp_get_u_state(mtp));
	err = -EINVAL;		/* unexpected message */
	goto error;
      error:
	freemsg(mp);
	return (err);
}

/**
 * mtp_recv_snmm_scon: - receive SNMM SCON message
 * @mtp: MTP private structure
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
mtp_recv_snmm_scon(struct mtp *mtp, stuct tp * tp, queue_t *q, mblk_t *mp)
{
	int err;
	struct ua_parm apc, cong;
	struct mtp_addr addr;
	int status = MTP_STATUS_CONGESTION;

	if (mtp_get_i_state(mtp) == MTPS_UNBND)
		goto outstate;
	if (!ua_dec_parm(mp->b_rptr, mp->b_wptr, &apc, UA_PARM_APC))
		goto missing;
	addr.pc = apc.val;
	addr.si = 0;
	addr.ni = 0;
	/* FIXME: sanity check value */
	if (ua_dec_parm(mp->b_rptr, mp->b_wptr, &cong, M3UA_PARM_CONG_IND)) {
		switch (cong.val) {
		case 0:
			status = MTP_STATUS_CONGESTION_LEVEL0;
			break;
		case 1:
			status = MTP_STATUS_CONGESTION_LEVEL1;
			break;
		case 2:
			status = MTP_STATUS_CONGESTION_LEVEL2;
			break;
		case 3:
			status = MTP_STATUS_CONGESTION_LEVEL3;
			break;
		default:
			/* really an error */
			status = MTP_STATUS_CONGESTION;
			break;
		}
	}
	return mtp_status_ind(mtp, q, mp, &addr, MTP_STATUS_TYPE_CONG, status);
      missing:
	strlog(mtp->mid, mtp->sid, 0, SL_ERROR, "SNMM SCON missing APC parameter");
	err = -ENXIO;		/* missing mandatory parameter */
	goto error;
      outstate:
	strlog(mtp->mid, mtp->sid, 0, SL_ERROR, "SNMM SCON in unexpected state %u:%u",
	       mtp_get_i_state(mtp), mtp_get_u_state(mtp));
	err = -EINVAL;		/* unexpected message */
	goto error;
      error:
	freemsg(mp);
	return (err);
}

/**
 * mtp_recv_snmm_dupu: - receive SNMM DUPU message
 * @mtp: MTP private structure
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
mtp_recv_snmm_dupu(struct mtp *mtp, stuct tp * tp, queue_t *q, mblk_t *mp)
{
	int err;
	struct ua_parm apc, cause;
	struct mtp_addr addr;
	int status = MTP_STATUS_UPU_UNKNOWN;

	if (mtp_get_i_state(mtp) == MTPS_UNBND)
		goto outstate;
	if (!ua_dec_parm(mp->b_rptr, mp->b_wptr, &apc, UA_PARM_APC))
		goto missing_apc;
	if (!ua_dec_parm(mp->b_rptr, mp->b_wptr, &cause, M3UA_PARM_USER_CAUSE))
		goto missing_user_cause;
	addr.pc = apc.val;
	addr.si = (cause.val & 0xffff);
	addr.ni = 0;
	/* FIXME: sanity check value */
	switch (cause.val >> 16) {
		default:
		case 0:
			status = MTP_STATUS_UPU_UNKNOWN;
			break;
		case 1:
			status = MTP_STATUS_UPU_UNEQUIPPED;
			break;
		case 2:
			status = MTP_STATUS_UPU_INACCESSIBLE;
			break;
	}
	return mtp_status_ind(mtp, q, mp, &addr, MTP_STATUS_TYPE_UPU, status);
      missing_user_cause:
	strlog(mtp->mid, mtp->sid, 0, SL_ERROR, "SNMM DUPU missing USER/CAUSE parameter");
	err = -ENXIO;		/* missing mandatory parameter */
	goto error;
      missing_apc:
	strlog(mtp->mid, mtp->sid, 0, SL_ERROR, "SNMM DUPU missing APC parameter");
	err = -ENXIO;		/* missing mandatory parameter */
	goto error;
      outstate:
	strlog(mtp->mid, mtp->sid, 0, SL_ERROR, "SNMM DUPU in unexpected state %u:%u",
	       mtp_get_i_state(mtp), mtp_get_u_state(mtp));
	err = -EINVAL;		/* unexpected message */
	goto error;
      error:
	freemsg(mp);
	return (err);
}

/**
 * mtp_recv_snmm_drst: - receive SNMM DRST message
 * @mtp: MTP private structure
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
mtp_recv_snmm_drst(struct mtp *mtp, stuct tp * tp, queue_t *q, mblk_t *mp)
{
	int err;
	struct ua_parm apc = { {NULL,}, };
	struct mtp_addr addr;

	if (mtp_get_i_state(mtp) == MTPS_UNBND)
		goto outstate;
	if (!ua_dec_parm(mp->b_rptr, mp->b_wptr, &apc, UA_PARM_APC))
		goto missing;
	addr.pc = apc.val;
	addr.si = 0;
	addr.ni = 0;
	/* FIXME: sanity check value */
	return mtp_status_ind(mtp, q, mp, &addr, MTP_STATUS_TYPE_RSTR, 0);
      missing:
	strlog(mtp->mid, mtp->sid, 0, SL_ERROR, "SNMM DRST missing APC parameter");
	err = -ENXIO;		/* missing mandatory parameter */
	goto error;
      outstate:
	strlog(mtp->mid, mtp->sid, 0, SL_ERROR, "SNMM DRST in unexpected state %u:%u",
	       mtp_get_i_state(mtp), mtp_get_u_state(mtp));
	err = -EINVAL;		/* unexpected message */
	goto error;
      error:
	freemsg(mp);
	return (err);
}

/**
 * mtp_recv_rkmm_reg_rsp: - receive REG Rsp message
 * @mtp: MTP private structure
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
mtp_recv_rkmm_reg_rsp(struct mtp *mtp, struct tp *tp, queue_t *q, mblk_t *mp, uint32_t status,
		      uint32_t rc)
{
	int err, error = 0;

	mtp->rc = rc;

	if (mtp_get_u_state(mtp) != ASP_WACK_ASPUP)
		goto outstate;
	if (mtp_get_i_state(mtp) != LMI_ATTACH_PENDING)
		goto outstate;
	switch (status) {
	case 0:		/* Successfully Registered */
		error = 0;
		break;
	case 2:		/* Error - Invalid SDLI */
	case 3:		/* Error - Invalid SDTI */
	case 4:		/* Error - Invalid Link Key */
	case 6:		/* Error - Overlapping (Non-unique) Link Key */
	case 7:		/* Error - Link Key not Provisioned */
		error = LMI_BADPPA;
		break;
	default:
	case 1:		/* Error - Unknown */
		error = LMI_UNSPEC;
		break;
	case 5:		/* Error - Permission Denied */
		error = -EPERM;
		break;
	case 8:		/* Error - Insufficient Resources */
		error = -ENOMEM;
		break;
	}
	if ((err = lmi_error_ack(mtp, q, mp, LMI_ATTACH_REQ, error)) == 0)
		mtp->request_id = 0;
	return (err);
      outstate:
	strlog(mtp->mid, mtp->sid, 0, SL_ERROR, "REG RSP in unexpected state %u:%u",
	       mtp_get_i_state(mtp), mtp_get_u_state(mtp));
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/**
 * mtp_recv_rkmm_dereg_rsp: - receive DEREG Rsp message
 * @mtp: MTP private structure
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
mtp_recv_rkmm_dereg_rsp(struct mtp *mtp, struct tp *tp, queue_t *q, mblk_t *mp, uint32_t status,
			uint32_t rc)
{
	int err, error = 0;

	if (mtp_get_u_state(mtp) != ASP_WACK_ASPDN)
		goto outstate;
	if (mtp_get_i_state(mtp) != LMI_DETACH_PENDING)
		goto outstate;
	switch (status) {
	case 0:		/* Successfully De-registered */
		error = 0;
		break;
	default:		/* protocol error */
	case 1:		/* Error - Unknown */
		error = LMI_UNSPEC;
		break;
	case 2:		/* Error - Invalid Interface Identifier */
		error = LMI_BADPPA;
		break;
	case 3:		/* Error - Permission Denied */
		error = -EPERM;
		break;
	case 4:		/* Error - Not Registered */
		error = 0;
		break;
	}
	if ((err = lmi_ok_ack(mtp, q, mp, LMI_DETACH_REQ)) == 0)
		mtp->rc = 0;
	return (err);
      outstate:
	strlog(mtp->mid, mtp->sid, 0, SL_ERROR, "DEREG RSP in unexpected state %u:%u",
	       mtp_get_i_state(mtp), mtp_get_u_state(mtp));
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/**
 * tp_recv_err: - process receive error
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 * @err: error
 *
 * The purpose of this function is to collect common return errors together and provide treatment in
 * a single rather unlikely non-inline function.
 */
static noinline fastcall int
tp_recv_err(struct tp *tp, queue_t *q, mblk_t *mp, int err)
{
	switch (err) {
	case 0:
	case -EBUSY:
	case -ENOMEM:
	case -ENOBUFS:
	case -ENOSR:
	case -EAGAIN:
	case -EDEADLK:
		break;
	case -ESRCH:
	{
		struct ua_parm rc = { { NULL, }, };

		/* Return codes that result from the failure to locate an AS for a given MAUP
		   message (ESRCH) should also respond by attempting to disable the corresponding
		   application server.  In this way, we can simply close upper SL streams in
		   mtp_qclose() and the arrival of any message for the AS will result in the AS
		   being deactivated if the AS could not be deactivated during close (perhaps from
		   failure to allocate a buffer). */
		if (ua_dec_parm(mp->b_rptr, mp->b_wptr, &rc, UA_PARM_RC))
			return tp_send_aspt_aspia_req(tp, q, mp, &rc.val, NULL, 0);
		if (tp->mtp.list == NULL)
			return tp_send_aspt_aspia_req(tp, q, mp, NULL, NULL, 0);
		err = UA_ECODE_UNEXPECTED_MESSAGE;
		goto error;
	}
	case -EINVAL:
		err = UA_ECODE_UNEXPECTED_MESSAGE;
		goto error;
	case -EMSGSIZE:
		err = UA_ECODE_PARAMETER_FIELD_ERROR;
		goto error;
	case -EOPNOTSUPP:
		err = UA_ECODE_UNSUPPORTED_MESSAGE_TYPE;
		goto error;
	case -ENOPROTOOPT:
		err = UA_ECODE_UNSUPPORTED_MESSAGE_CLASS;
		goto error;
	case -EPROTO:
		err = UA_ECODE_PROTOCOL_ERROR;
		goto error;
	case -ENXIO:
		err = UA_ECODE_MISSING_PARAMETER;
		goto error;
	default:
		if (err < 0)
			err = UA_ECODE_PROTOCOL_ERROR;
	      error:
		return tp_send_mgmt_err(tp, q, mp, err, mp->b_rptr, mp->b_wptr - mp->b_rptr);
	}
	return (err);
}

/**
 * tp_recv_mgmt: - receive MGMT message
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
tp_recv_mgmt(struct tp *tp, queue_t *q, mblk_t *mp)
{
	register uint32_t *p = (typeof(p)) mp->b_rptr;
	int err;

	switch (UA_MSG_TYPE(p[0])) {
	case UA_MGMT_ERR:
		strlog(tp->mid, tp->sid, SLLOGRX, SL_TRACE, "ERR <-");
		err = tp_recv_mgmt_err(tp, q, mp);
		break;
	case UA_MGMT_NTFY:
		strlog(tp->mid, tp->sid, SLLOGRX, SL_TRACE, "NTFY <-");
		err = tp_recv_mgmt_ntfy(tp, q, mp);
		break;
	default:
		err = (-EOPNOTSUPP);
		break;
	}
	return (err);
}

/**
 * snp_recv_asps: - receive ASPS message
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
tp_recv_asps(struct tp *tp, queue_t *q, mblk_t *mp)
{
	register uint32_t *p = (typeof(p)) mp->b_rptr;
	int err;

	switch (UA_MSG_TYPE(p[0])) {
	case UA_ASPS_HBEAT_REQ:
		strlog(tp->mid, tp->sid, SLLOGRX, SL_TRACE, "BEAT <-");
		err = tp_recv_asps_hbeat_req(tp, q, mp);
		break;
	case UA_ASPS_ASPUP_ACK:
		strlog(tp->mid, tp->sid, SLLOGRX, SL_TRACE, "ASPUP Ack <-");
		err = tp_recv_asps_aspup_ack(tp, q, mp);
		break;
	case UA_ASPS_ASPDN_ACK:
		strlog(tp->mid, tp->sid, SLLOGRX, SL_TRACE, "ASPDN Ack <-");
		err = tp_recv_asps_aspdn_ack(tp, q, mp);
		break;
	case UA_ASPS_HBEAT_ACK:
		strlog(tp->mid, tp->sid, SLLOGRX, SL_TRACE, "BEAT Ack <-");
		err = tp_recv_asps_hbeat_ack(tp, q, mp);
		break;
	default:
		err = (-EOPNOTSUPP);
		break;
	}
	return (err);
}

/**
 * tp_recv_aspt - receive ASPT message
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 *
 * A note about ASPAC Ack and ASPIA Ack: the RFC says that if there is an IID in the request that
 * ther MUST be an IID in the Ack.  Because we always send an IID in the request, we require that
 * there always be an IID in the Ack.  We use the IID to determine for which signalling link the
 * acknowledgement applies.
 */
static int
tp_recv_aspt(struct tp *tp, queue_t *q, mblk_t *mp)
{
	register uint32_t *p = (typeof(p)) mp->b_rptr;
	int err;

	switch (UA_MSG_TYPE(p[0])) {
	case UA_ASPT_ASPAC_ACK:
		strlog(tp->mid, tp->sid, SLLOGRX, SL_TRACE, "ASPAC Ack <-");
		err = tp_recv_aspt_aspac_ack(tp, q, mp);
		break;
	case UA_ASPT_ASPIA_ACK:
		strlog(tp->mid, tp->sid, SLLOGRX, SL_TRACE, "ASPIA Ack <-");
		err = tp_recv_aspt_aspia_ack(tp, q, mp);
		break;
	default:
		err = (-EOPNOTSUPP);
		break;
	}
	return (err);
}

/**
 * tp_recv_xfer: - receive XFER message
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
tp_recv_xfer(struct tp *tp, queue_t *q, mblk_t *mp)
{
	register uint32_t *p = (typeof(p)) mp->b_rptr;
	struct mtp *mtp;
	int err;

	if (!(mtp = mtp_lookup(tp, q, mp, &err)) && err)
		return (err);

	switch (UA_MSG_TYPE(p[0])) {
	case UA_XFER_DATA:
		strlog(mtp->mid, mtp->sid, MTLOGDA, SL_TRACE, "XFER DATA <-");
		err = mtp_recv_xfer_data(mtp, tp, q, mp);
		break;
	default:
		err = -EOPNOTSUPP;
		break;
	}
	return (err);
}

/**
 * tp_recv_snmm: - receive SNMM message
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
tp_recv_snmm(struct tp *tp, queue_t *q, mblk_t *mp)
{
	register uint32_t *p = (typeof(p)) mp->b_rptr;
	struct mtp *mtp;
	int err;

	if (!(mtp = mtp_lookup(tp, q, mp, &err)) && err)
		return (err);

	switch (UA_MSG_TYPE(p[0])) {
	case UA_SNMM_DUNA:
		strlog(mtp->mid, mtp->sid, MTLOGRX, SL_TRACE, "SNMM DUNA <-");
		err = mtp_recv_snmm_duna(mtp, tp, q, mp);
		break;
	case UA_SNMM_DAVA:
		strlog(mtp->mid, mtp->sid, MTLOGRX, SL_TRACE, "SNMM DAVA <-");
		err = mtp_recv_snmm_dava(mtp, tp, q, mp);
		break;
	case UA_SNMM_DAUD:
		err = -EINVAL;	/* unexpected in this direction */
		break;
	case UA_SNMM_SCON:
		strlog(mtp->mid, mtp->sid, MTLOGRX, SL_TRACE, "SNMM SCON <-");
		err = mtp_recv_snmm_scon(mtp, tp, q, mp);
		break;
	case UA_SNMM_DUPU:
		strlog(mtp->mid, mtp->sid, MTLOGRX, SL_TRACE, "SNMM DUPU <-");
		err = mtp_recv_snmm_dupu(mtp, tp, q, mp);
		break;
	case UA_SNMM_DRST:
		strlog(mtp->mid, mtp->sid, MTLOGRX, SL_TRACE, "SNMM DRST <-");
		err = mtp_recv_snmm_drst(mtp, tp, q, mp);
		break;
	default:
		err = -EOPNOTSUPP;
		break;
	}
	return (err);
}

/**
 * tp_recv_rkmm - receive RKMM message from SG
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: RKMM message
 *
 * RKMM messages are only sent by upper MTP Streams.  They are sent as part of the LMI_ATTACH_REQ
 * process and used to translate a PPA to an IID (if required).  When an upper MTP Stream sends a
 * registration or deregistration request, it save the identifier of the request in the MTP private
 * structure.  We can find the MTP structure that corresponds to a response by finding the structure
 * with the same request identifier.
 */
static int
tp_recv_rkmm(struct tp *tp, queue_t *q, mblk_t *mp)
{
	register uint32_t *p = (typeof(p)) mp->b_rptr;
	struct ua_parm request_id;
	struct mtp *mtp;
	int err;

	/* FIXME: mesasge can contain multiple registration results (but probably won't) */
	if (ua_dec_parm(mp->b_rptr, mp->b_wptr, &request_id, M3UA_PARM_REG_RESULT)) {
		uint32_t status = 0;
		uint32_t iid = 0;

		/* FIXME also need M3UA_PARM_DEREG_RESULT, which does not have routing key id */
		for (mtp = tp->mtp.list; mtp; mtp = mtp->tp.next)
			if (mtp->request_id == request_id.val)
				break;
		if (mtp == NULL)
			/* FIXME: process error for each registration result instead of returning
			   it */
			return (-EPROTO);

		switch (UA_MSG_TYPE(p[0])) {
		case UA_RKMM_REG_RSP:
			/* FIXME: dig registration status and interface identifier out of the
			   registration result */
			strlog(mtp->mid, mtp->sid, MTLOGRX, SL_TRACE, "REG Rsp <-");
			err = mtp_recv_rkmm_reg_rsp(mtp, tp, q, mp, status, iid);
			break;
		case UA_RKMM_DEREG_RSP:
			/* FIXME: dig registration status out of the deregistration result */
			strlog(mtp->mid, mtp->sid, MTLOGRX, SL_TRACE, "DEREG Rsp <-");
			err = mtp_recv_rkmm_dereg_rsp(mtp, tp, q, mp, status, iid);
			break;
		default:
			err = (-EOPNOTSUPP);
			break;
		}
		/* FIXME: process error for each registration result instead of returning it */
		return (err);
	}
	return (0);
}

/**
 * tp_recv_msg_slow: - receive message (slow path)
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static noinline fastcall int
tp_recv_msg_slow(struct tp *tp, queue_t *q, mblk_t *mp)
{
	register uint32_t *p = (typeof(p)) mp->b_rptr;
	int err = -EMSGSIZE;

	if (mp->b_wptr < mp->b_rptr + 2 * sizeof(*p))
		goto error;
	if (mp->b_wptr < mp->b_rptr + ntohl(p[1]))
		goto error;
	switch (UA_MSG_CLAS(p[0])) {
	case UA_CLASS_MGMT:
		err = tp_recv_mgmt(tp, q, mp);
		break;
	case UA_CLASS_ASPS:
		err = tp_recv_asps(tp, q, mp);
		break;
	case UA_CLASS_ASPT:
		err = tp_recv_aspt(tp, q, mp);
		break;
	case UA_CLASS_XFER:
		err = tp_recv_xfer(tp, q, mp);
		break;
	case UA_CLASS_SNMM:
		err = tp_recv_snmm(tp, q, mp);
		break;
	case UA_CLASS_RKMM:
		err = tp_recv_rkmm(tp, q, mp);
		break;
	default:
		err = -ENOPROTOOPT;
		break;
	}
	if (err == 0)
		return (0);
      error:
	return tp_recv_err(tp, q, mp, err);
}

/**
 * tp_recv_msg: - receive message (fast path)
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message (just the M2UA part)
 */
static inline fastcall int
tp_recv_msg(struct tp *tp, queue_t *q, mblk_t *mp)
{
	uint32_t *p = (typeof(p)) mp->b_rptr;
	int err;

	/* fast path for data */
	if (mp->b_wptr >= mp->b_rptr + 2 * sizeof(*p))
		if (mp->b_wptr >= mp->b_rptr + ntohl(p[1]))
			if (UA_MSG_CLAS(p[0]) == UA_CLASS_XFER)
				if (UA_MSG_TYPE(p[0]) == UA_XFER_DATA) {
					if ((err = tp_recv_xfer_data(tp, q, mp)) == 0)
						return (0);
					return tp_recv_err(tp, q, mp, err);
				}
	return tp_recv_msg_slow(tp, q, mp);
}

/*
 *  MTP-User to MTP-Provider primitives.
 *  ====================================
 */

/**
 * mtp_bind_req: - process MTP_BIND_REQ primitive
 * @mtp: MTP private structure
 * @q: active queue (upper write queue)
 * @mp: the primitive
 */
static noinline fastcall int
mtp_bind_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
}

/**
 * mtp_unbind_req: - process MTP_UNBIND_REQ primitive
 * @mtp: MTP private structure
 * @q: active queue (upper write queue)
 * @mp: the primitive
 */
static noinline fastcall int
mtp_unbind_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
}

/**
 * mtp_conn_req: - process MTP_CONN_REQ primitive
 * @mtp: MTP private structure
 * @q: active queue (upper write queue)
 * @mp: the primitive
 */
static noinline fastcall int
mtp_conn_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
}

/**
 * mtp_discon_req: - process MTP_DISCON_REQ primitive
 * @mtp: MTP private structure
 * @q: active queue (upper write queue)
 * @mp: the primitive
 */
static noinline fastcall int
mtp_discon_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
}

/**
 * mtp_addr_req: - process MTP_ADDR_REQ primitive
 * @mtp: MTP private structure
 * @q: active queue (upper write queue)
 * @mp: the primitive
 */
static noinline fastcall int
mtp_addr_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
}

/**
 * mtp_info_req: - process MTP_INFO_REQ primitive
 * @mtp: MTP private structure
 * @q: active queue (upper write queue)
 * @mp: the primitive
 */
static noinline fastcall int
mtp_info_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
}

/**
 * mtp_optmgmt_req: - process MTP_OPTMGMT_REQ primitive
 * @mtp: MTP private structure
 * @q: active queue (upper write queue)
 * @mp: the primitive
 */
static noinline fastcall int
mtp_optmgmt_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
}

/**
 * mtp_transfer_req: - process MTP_TRANSFER_REQ primitive
 * @mtp: MTP private structure
 * @q: active queue (upper write queue)
 * @mp: the primitive
 */
static noinline fastcall int
mtp_transfer_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
}

/**
 * mtp_other_req: - process other MTP primitive
 * @mtp: MTP private structure
 * @q: active queue (upper write queue)
 * @mp: the primitive
 */
static noinline fastcall int
mtp_other_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
}

/*
 * TS-Provider to TS-User primitives.
 * ==================================
 */

/**
 * t_conn_ind: - process T_CONN_IND primitive
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the primitive
 *
 * We basically allow the linked SCTP T-Provider Stream to be in any state whatsoever for maximum
 * compatibility with possible use cases.  The only time that we would get an T_CONN_IND on a Stream
 * is if the Stream was bound as a listening Stream, in which case, we should complete the
 * connection by responding with the listening Stream as the accepting Stream and move the Stream to
 * the established state.  Also, we should save the connecting address(es) so that we can reconnect
 * the Stream at a later point.
 *
 * Because we move to the established state (TS_DATA_XFER) at the end of this procedure, we should
 * also determine whether there are any signalling links that are in a pending state, either
 * LMI_ATTACH_PENDING or LMI_ENABLE_PENDING and complete their procedures.  If there are any
 * signalling links in the pending state going down or are in a state better than LMI_DISABLED, they
 * need to be downgraded to at least disabled.  If the SG processes registration, all signalling
 * links must be considered in the unregistered state.
 */
static noinline fastcall int
t_conn_ind(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_conn_ind *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto fault;
	switch (tp_get_i_state(tp)) {
	case TS_WRES_CIND:
	case TS_IDLE:
	case (uint) - 1:
		tp_set_i_state(tp, TS_WRES_CIND);
		break;
	default:
		goto outstate;
	}
	return t_conn_res(tp, q, mp, tp->acceptorid, 0, NULL, p->SEQ_number, NULL);
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_CONN_IND out of state");
	goto error;
      fault:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_CONN_IND too short");
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/**
 * t_conn_con: - process T_CONN_CON primitive
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the primitive
 *
 * The connection of an association is being confirmed.  This is either the original connection
 * confirmation or is a connection confirmation resutling from a reconnect.  In either case, we want
 * to issue an T_ADDR_REQ to determine the local and remote addresses.
 */
static noinline fastcall int
t_conn_con(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_conn_con *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto fault;
	switch (tp_get_i_state(tp)) {
	case TS_WCON_CREQ:
	case (uint) - 1:
		break;
	default:
		goto outstate;
	}
	tp_set_i_state(tp, TS_DATA_XFER);
	if (tp->flags & M2UA_SG_CONFIGURED)
		return t_addr_req(tp, q, mp);
	if (!(tp->flags & M2UA_SG_LOCADDR))
		return t_addr_req(tp, q, mp);
	if (!(tp->flags & M2UA_SG_REMADDR)) {
		if (p->RES_length) {
			bcopy((caddr_t) p + p->RES_offset, &tp->rem, p->RES_length);
			tp->rem_len = p->RES_length;
			tp->flags |= M2UA_SG_REMADDR;
		} else {
			return t_addr_req(tp, q, mp);
		}
	}
	if (tp_get_u_state(tp) == ASP_DOWN) {
		tp_set_u_state(tp, ASP_WACK_ASPUP);
		return tp_send_asps_aspup_req(tp, q, mp, tp->aspid ? &tp->aspid : NULL, NULL, 0);
	}
	freemsg(mp);
	return (0);
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_CONN_CON out of state");
	goto error;
      fault:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_CONN_CON too short");
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/**
 * t_discon_ind: - process T_DISCON_IND primitive
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the primitive
 *
 * A disconnect indication corresponds to a disable indication for each of the enabled signalling
 * links being served by this association.  We issue an error indication for each enabled signalling
 * link and mark it as a fatal error.  Then we should attempt to reconnect the association.  What we
 * need to do as follows: when an association has been in the connected state previously, it should
 * store the remote address.  Then, when we wish to reconnect the association, we should use the
 * stored address in the connect request.
 */
static noinline fastcall int
t_discon_ind(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_discon_ind *p = (typeof(p)) mp->b_rptr;
	struct sl *sl;
	int err = 0;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto fault;
	switch (tp_get_i_state(tp)) {
	case TS_DATA_XFER:
	case TS_WIND_ORDREL:
	case TS_WREQ_ORDREL:
	case TS_WCON_CREQ:
		break;
	default:
		goto outstate;
	}
	write_lock(&sl_mux_lock);
	for (sl = tp->sl.list; sl; sl = sl->tp.next) {
		if (tp->flags & M2UA_SG_DYNAMIC) {
			/* When the SG requires dynamic registration, when the association is lost,
			   we also lose the registration, meaning that the SL-User must reattach.
			   This is probably not really what SL-user are expecting, but then
			   registration is complicated for M2UA anyway. */
			if ((err = lmi_error_ind(sl, q, mp, LMI_DISC, LMI_UNATTACHED)))
				break;
			if ((*sl->tp.prev = sl->tp.next))
				sl->tp.next->tp.prev = sl->tp.prev;
			sl->tp.tp = NULL;
			sl->tp.next = NULL;
			sl->tp.prev = &sl->tp.next;
		} else {
			/* When the SG requires static configuration, when the association is lost,
			   the configuration is not lost, meaning that the SL-User merely needs to
			   reenable.  This is probably what SL-users are expecting. */
			switch (sl_get_i_state(sl)) {
			case LMI_DETACH_PENDING:
			case LMI_ATTACH_PENDING:
			case LMI_UNATTACHED:
			default:
				strlog(sl->mid, sl->sid, 0, SL_TRACE | SL_ERROR,
				       "%s: unexpected sl i state %u", __FUNCTION__, sl_get_i_state(sl));
				/* fall through */
			case LMI_ENABLED:
			case LMI_ENABLE_PENDING:
			case LMI_DISABLE_PENDING:
				if ((err = lmi_error_ind(sl, q, mp, LMI_DISC, LMI_DISABLED)))
					break;
				continue;
			case LMI_DISABLED:
				/* probably already disabled on a previous pass */
				continue;
			}
			break;
		}
		sl_set_u_state(sl, ASP_DOWN);
	}
	write_unlock(&sl_mux_lock);
	if (err)
		return (err);
	tp_set_i_state(tp, TS_IDLE);
	/* XXX: we should attempt to reconnect the association. */
	freemsg(mp);
	return (0);
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_DISCON_IND out of state");
	goto error;
      fault:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_DISCON_IND too short");
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/**
 * t_data_ind: - process T_DATA_IND primitive
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the primitive
 *
 * When we receive data on a Stream, we take the M3UA part and pass it for processing.  Note that we
 * should check the PPI to ensure that it is either the M3UA PPI or PPI zero.
 */
static noinline fastcall int
t_data_ind(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_data_ind *p = (typeof(p)) mp->b_rptr;
	mblk_t *dp = mp->b_cont;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto fault;
	if (tp_get_i_state(tp) != TS_DATA_XFER)
		goto outstate;
	if ((err = tp_recv_msg(tp, q, dp)) == 0)
		freeb(mp);
	return (err);
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_DATA_IND out of state");
	goto error;
      fault:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_DATA_IND too short");
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/**
 * t_exdata_ind: - process T_EXDATA_IND primitive
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the primitive
 *
 * When we receive data on a Stream, we take the M3UA part and pass it for processing.  Note that we
 * should check the PPI to ensure that it is either the M3UA PPI or PPI zero.
 */
static noinline fastcall int
t_exdata_ind(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_exdata_ind *p = (typeof(p)) mp->b_rptr;
	mblk_t *dp = mp->b_cont;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto fault;
	if (tp_get_i_state(tp) != TS_DATA_XFER)
		goto outstate;
	if ((err = tp_recv_msg(tp, q, dp)) == 0)
		freeb(mp);
	return (err);
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_EXDATA_IND out of state");
	goto error;
      fault:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_EXDATA_IND too short");
	goto error;
      error:
	freemsg(mp);
	return (0);
}

static noinline fastcall int
t_info(struct tp *tp, queue_t *q, mblk_t *mp, struct T_info_ack *p)
{
	uint oldstate = tp_get_i_state(tp);

	tp->info.PRIM_type = T_INFO_ACK;
	tp->info.TSDU_size = p->TSDU_size;
	tp->info.ETSDU_size = p->ETSDU_size;
	tp->info.CDATA_size = p->CDATA_size;
	tp->info.DDATA_size = p->DDATA_size;
	tp->info.ADDR_size = p->ADDR_size;
	tp->info.OPT_size = p->OPT_size;
	tp->info.TIDU_size = p->TIDU_size;
	tp->info.SERV_type = p->SERV_type;
	tp->info.CURRENT_state = p->CURRENT_state;
	tp->info.PROVIDER_flag = p->PROVIDER_flag;
	if (oldstate != tp->info.CURRENT_state) {
		if (oldstate == (uint) - 1) {
			switch (tp->info.CURRENT_state) {
			case TS_UNBND:
				if (tp->flags & M3UA_SG_CONFIGURED)
					return t_bind_req(tp, q, mp, sizeof(tp->loc),
							  (caddr_t) &tp->loc);
				break;
			case TS_IDLE:
				if (tp->flags & M3UA_SG_CONFIGURED) {
					struct {
						struct t_opthdr oh;
						t_uscalar_t val;
					} opts;

					opts.oh.len = sizeof(opts);
					opts.oh.level = T_INET_SCTP;
					opts.oh.name = T_SCTP_OSTREAMS;
					opts.oh.status = T_NEGOTIATE;
					opts.val = tp->streams;

					return t_conn_req(tp, q, mp, sizeof(tp->rem),
							  (caddr_t) &tp->rem, sizeof(opts),
							  (caddr_t) &opts, NULL);
				}
				if (!(tp->flags & M3UA_SG_LOCADDR))
					return t_addr_req(tp, q, mp);
				break;
			case TS_DATA_XFER:
				if ((tp->flags & M3UA_SG_CONFIGURED)
				    && tp_get_u_state(tp) == ASP_DOWN)
					return tp_send_asps_aspup_req(tp, q, mp,
								      tp->aspid ? &tp->aspid : NULL,
								      NULL, 0);
				if ((tp->flags & (M3UA_SG_LOCADDR | M3UA_SG_REMADDR)) !=
				    (M3UA_SG_LOCADDR | M3UA_SG_REMADDR))
					return t_addr_req(tp, q, mp);
				break;
			case TS_WREQ_ORDREL:
				return t_ordrel_req(tp, q, mp, NULL);
			case TS_WACK_BREQ:
			case TS_WACK_UREQ:
			case TS_WACK_CREQ:
			case TS_WACK_CRES:
			case TS_WACK_DREQ6:
			case TS_WACK_DREQ7:
			case TS_WACK_DREQ9:
			case TS_WACK_DREQ10:
			case TS_WACK_DREQ11:
			case TS_WACK_OPTREQ:
			case TS_WIND_ORDREL:
				break;
			default:
				goto outstate;
			}
		} else
			goto outstate;
	}
      outstate:
	freemsg(mp);
	return (0);
}

/**
 * t_info_ack: - process T_INFO_ACK primitive
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the primitive
 *
 * Whenever and T_INFO_ACK is returned we process and save the information from the acknowledgement.
 * Also, the state is changed to synchronize with the information acknowledgement.
 *
 * Whenever an SCTP transport stream is linked under the multiplexing driver, we set the interface
 * state to -1 an issue an immediate T_INFO_ACK.  When we receive a T_INFO_ACK and the state is set
 * to -1, after processing the T_INFO_ACK we may need to perform additional actions.  One action is
 * issuing a T_ADDR_REQ to obtain addresses if the stream is in any state other than TS_UNBND.
 */
static noinline fastcall int
t_info_ack(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_info_ack *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto fault;
	return t_info(tp, q, mp, p);
#if 0
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_INFO_ACK unexpected state");
	goto error;
#endif
      fault:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_INFO_ACK too short");
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/**
 * t_bind_ack: - process T_BIND_ACK primitive
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the primitive
 *
 * Acknowledges a successful bind.  Note that the address bound is in the response; however, because
 * the trasnport provider may have assigned an address and we want to retain specification of a
 * wildcard bind, we only use the address if there was no adddress configured.
 */
static noinline fastcall int
t_bind_ack(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_bind_ack *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto fault;
	if (tp_get_i_state(tp) != TS_WACK_BREQ)
		goto outstate;
	tp_set_i_state(tp, TS_IDLE);
	if (tp->flags & (M2UA_SG_CONFIGURED | M2UA_SG_REMADDR)) {
		struct {
			struct t_opthdr oh;
			t_uscalar_t val;
		} opts;

		opts.oh.len = sizeof(opts);
		opts.oh.level = T_INET_SCTP;
		opts.oh.name = T_SCTP_OSTREAMS;
		opts.oh.status = T_NEGOTIATE;
		opts.val = tp->streams;

		return t_conn_req(tp, q, mp, (size_t) tp->rem_len, (caddr_t) &tp->rem, sizeof(opts),
				  (caddr_t) &opts, NULL);
	}
	if ((tp->flags & (M2UA_SG_LOCADDR)) != (M2UA_SG_LOCADDR))
		return t_addr_req(tp, q, mp);
	freemsg(mp);
	return (0);
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_BIND_ACK out of state");
	goto error;
      fault:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_BIND_ACK too short");
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/**
 * t_error_ack: - process T_ERROR_ACK primitive
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the primitive
 */
static noinline fastcall int
t_error_ack(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_error_ack *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto fault;
	switch (tp_get_i_state(tp)) {
	case TS_WACK_BREQ:
		tp_set_i_state(tp, TS_UNBND);
		break;
	case TS_WACK_UREQ:
		tp_set_i_state(tp, TS_IDLE);
		break;
	case TS_WACK_CREQ:
		tp_set_i_state(tp, TS_IDLE);
		break;
	case TS_WACK_CRES:
		tp_set_i_state(tp, TS_WRES_CIND);
		break;
	case TS_WACK_DREQ6:
		tp_set_i_state(tp, TS_WCON_CREQ);
		break;
	case TS_WACK_DREQ7:
		tp_set_i_state(tp, TS_WRES_CIND);
		break;
	case TS_WACK_DREQ9:
		tp_set_i_state(tp, TS_DATA_XFER);
		break;
	case TS_WACK_DREQ10:
		tp_set_i_state(tp, TS_WIND_ORDREL);
		break;
	case TS_WACK_DREQ11:
		tp_set_i_state(tp, TS_WREQ_ORDREL);
		break;
	case TS_WACK_OPTREQ:
		tp_set_i_state(tp, TS_IDLE);
		break;
	default:
		goto outstate;
	}
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_ERROR_ACK out of state");
	goto error;
      fault:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_ERROR_ACK too short");
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/**
 * t_ok_ack: - process T_OK_ACK primitive
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the primitive
 */
static noinline fastcall int
t_ok_ack(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_ok_ack *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto fault;
	switch (tp_get_i_state(tp)) {
	case TS_WACK_UREQ:
		tp_set_i_state(tp, TS_UNBND);
	case TS_UNBND:
		if (tp->flags & (M3UA_SG_CONFIGURED | M3UA_SG_LOCADDR))
			return t_bind_req(tp, q, mp, sizeof(tp->loc), (caddr_t) &tp->loc);
		break;
	case TS_WACK_CREQ:
		tp_set_i_state(tp, TS_WCON_CREQ);
	case TS_WCON_CREQ:
		break;
	case TS_WACK_CRES:
		tp_set_i_state(tp, TS_DATA_XFER);
	case TS_DATA_XFER:
		if (tp->flags & M3UA_SG_CONFIGURED) {
			if (tp_get_u_state(tp) == ASP_DOWN)
				return tp_send_asps_aspup_req(tp, q, mp,
							      tp->aspid ? &tp->aspid : NULL, NULL,
							      0);
		}
		if ((tp->flags & (M3UA_SG_REMADDR | M3UA_SG_LOCADDR)) !=
		    (M3UA_SG_REMADDR | M3UA_SG_LOCADDR)) {
			return t_addr_req(tp, q, mp);
		}
		if (tp_get_u_state(tp) == ASP_DOWN)
			return tp_send_asps_aspup_req(tp, q, mp,
						      tp->aspid ? &tp->aspid : NULL, NULL, 0);
		break;
	case TS_WACK_BREQ:
	case TS_WACK_OPTREQ:
	case TS_WACK_DREQ6:
	case TS_WACK_DREQ7:
	case TS_WACK_DREQ9:
	case TS_WACK_DREQ10:
	case TS_WACK_DREQ11:
		tp_set_i_state(tp, TS_IDLE);
	case TS_IDLE:
		if (tp->flags & M3UA_SG_CONFIGURED) {
			struct {
				struct t_opthdr oh;
				t_uscalar_t val;
			} opts;

			opts.oh.len = sizeof(opts);
			opts.oh.level = T_INET_SCTP;
			opts.oh.name = T_SCTP_OSTREAMS;
			opts.oh.status = T_NEGOTIATE;
			opts.val = tp->streams;

			return t_conn_req(tp, q, mp, sizeof(tp->rem), (caddr_t) &tp->rem,
					  sizeof(opts), (caddr_t) &opts, NULL);
		}
		if ((tp->flags & (M3UA_SG_REMADDR | M3UA_SG_LOCADDR)) !=
		    (M3UA_SG_REMADDR | M3UA_SG_LOCADDR)) {
			return t_addr_req(tp, q, mp);
		}
		if (tp->flags & M3UA_SG_REMADDR) {
			struct {
				struct t_opthdr oh;
				t_uscalar_t val;
			} opts;

			opts.oh.len = sizeof(opts);
			opts.oh.level = T_INET_SCTP;
			opts.oh.name = T_SCTP_OSTREAMS;
			opts.oh.status = T_NEGOTIATE;
			opts.val = tp->streams;

			return t_conn_req(tp, q, mp, sizeof(tp->rem), (caddr_t) &tp->rem,
					  sizeof(opts), (caddr_t) &opts, NULL);
		}
		break;
	default:
		goto outstate;
	}
	freemsg(mp);
	return (0);
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_OK_ACK out of state");
	goto error;
      fault:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_OK_ACK too short");
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/**
 * t_unitdata_ind: - process T_UNITDATA_IND primitive
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the primitive
 */
static noinline fastcall int
t_unitdata_ind(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_unitdata_ind *p = (typeof(p)) mp->b_rptr;
	mblk_t *dp = mp->b_cont;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto fault;
	switch (tp_get_i_state(tp)) {
	case TS_IDLE:
	case TS_DATA_XFER:
	case TS_WIND_ORDREL:
		break;
	default:
		goto outstate;
	}
	if ((err = tp_recv_msg(tp, q, dp)) == 0)
		freeb(mp);
	return (err);
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_UNITDATA_IND out of state");
	goto error;
      fault:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_UNITDATA_IND too short");
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/**
 * t_uderror_ind: - process T_UDERROR_IND primitive
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the primitive
 */
static noinline fastcall int
t_uderror_ind(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_uderror_ind *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto fault;
	switch (tp_get_i_state(tp)) {
	case TS_DATA_XFER:
	case TS_WREQ_ORDREL:
		break;
	default:
		goto outstate;
	}
	/* ignore for now */
	freemsg(mp);
	return (0);
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_UDERROR_IND out of state");
	goto error;
      fault:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_UDERROR_IND too short");
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/**
 * t_optmgmt_ack: - process T_OPTMGMT_ACK primitive
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the primitive
 */
static noinline fastcall int
t_optmgmt_ack(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_optmgmt_ack *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto fault;
	if (tp_get_i_state(tp) != TS_IDLE)
		goto outstate;
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_OPTMGMT_ACK out of state");
	goto error;
      fault:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_OPTMGMT_ACK too short");
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/**
 * t_ordrel_ind: - process T_ORDREL_IND primitive
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the primitive
 *
 * Orderly releases as just as bad as abortive disconnects.  We need to do the same thing as is done
 * for an abortive disconnect and reponse immediately with an orderly release request.
 */
static noinline fastcall int
t_ordrel_ind(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_ordrel_ind *p = (typeof(p)) mp->b_rptr;
	struct sl *sl;
	int err = 0;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto fault;
	switch (tp_get_i_state(tp)) {
	case TS_DATA_XFER:
		tp_set_i_state(tp, TS_WREQ_ORDREL);
		break;
	case TS_WIND_ORDREL:
		tp_set_i_state(tp, TS_IDLE);
		break;
	default:
		goto outstate;
	}
	write_lock(&sl_mux_lock);
	for (sl = tp->sl.list; sl; sl = sl->tp.next) {
		if (tp->flags & M3UA_SG_DYNAMIC) {
			/* When the SG requires dynamic registration, when the association is lost,
			   we also lose the registration, meaning that the SL-User must reattach.
			   This is probably not really what SL-user are expecting, but then
			   registration is complicated for M3UA anyway. */
			if ((err = lmi_error_ind(sl, q, mp, LMI_DISC, LMI_UNATTACHED)))
				break;
			if ((*sl->tp.prev = sl->tp.next))
				sl->tp.next->tp.prev = sl->tp.prev;
			sl->tp.tp = NULL;
			sl->tp.next = NULL;
			sl->tp.prev = &sl->tp.next;
		} else {
			/* When the SG requires static configuration, when the association is lost,
			   the configuration is not lost, meaning that the SL-User merely needs to
			   reenable.  This is probably what SL-users are expecting. */
			switch (sl_get_i_state(sl)) {
			case LMI_DETACH_PENDING:
			case LMI_ATTACH_PENDING:
			case LMI_UNATTACHED:
			default:
				strlog(sl->mid, sl->sid, 0, SL_TRACE | SL_ERROR,
				       "%s: unexpected sl i state %u", __FUNCTION__, sl_get_i_state(sl));
				/* fall through */
			case LMI_ENABLED:
			case LMI_ENABLE_PENDING:
			case LMI_DISABLE_PENDING:
				if ((err = lmi_error_ind(sl, q, mp, LMI_DISC, LMI_DISABLED)))
					break;
				continue;
			case LMI_DISABLED:
				/* probably already disabled on a previous pass */
				continue;
			}
			break;
		}
		sl_set_u_state(sl, ASP_DOWN);
	}
	write_unlock(&sl_mux_lock);
	if (err)
		return (err);
	switch (tp_get_i_state(tp)) {
	case TS_WREQ_ORDREL:
		tp_set_i_state(tp, TS_IDLE);
		tp_set_u_state(tp, ASP_DOWN);
		return t_ordrel_req(tp, q, mp, NULL);
	case TS_IDLE:
		tp_set_i_state(tp, TS_IDLE);
		tp_set_u_state(tp, ASP_DOWN);
		freemsg(mp);
		return (0);
	default:
		goto outstate;
	}
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_ORDREL_IND out of state");
	goto error;
      fault:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_ORDREL_IND too short");
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/**
 * t_optdata_ind: - process T_OPTDATA_IND primitive
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the primitive
 */
static noinline fastcall int
t_optdata_ind(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_optdata_ind *p = (typeof(p)) mp->b_rptr;
	mblk_t *dp = mp->b_cont;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto fault;
	switch (tp_get_i_state(tp)) {
	case TS_DATA_XFER:
	case TS_WIND_ORDREL:
		break;
	default:
		goto outstate;
	}
	if ((err = tp_recv_msg(tp, q, dp)) == 0)
		freeb(mp);
	return (err);
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_OPTDATA_IND out of state");
	goto error;
      fault:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_OPTDATA_IND too short");
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/**
 * t_addr_ack: - process T_ADDR_ACK primitive
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the primitive
 */
static noinline fastcall int
t_addr_ack(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_addr_ack *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto fault;
	if (p->LOCADDR_length) {
		tp->flags |= M3UA_SG_LOCADDR;
		bcopy((caddr_t) p + p->LOCADDR_offset, &tp->loc, p->LOCADDR_length);
		tp->loc_len = p->LOCADDR_length;
	}
	if (p->REMADDR_length) {
		tp->flags |= M3UA_SG_REMADDR;
		bcopy((caddr_t) p + p->REMADDR_offset, &tp->rem, p->REMADDR_length);
		tp->rem_len = p->REMADDR_length;
	}
	/* The T_ADDR_REQ is often initiated as part of another sequence that needs to complete.
	   Check the state and complete the sequence if possible. */
	switch (tp_get_i_state(tp)) {
	case TS_UNBND:
		/* bind if possible */
		if (tp->flags & (M3UA_SG_CONFIGURED | M3UA_SG_LOCADDR)) {
			tp_set_i_state(tp, TS_WACK_BREQ);
			return t_bind_req(tp, q, mp, (size_t) tp->loc_len, (caddr_t) &tp->loc);
		}
		break;
	case TS_WACK_BREQ:
	case TS_WACK_UREQ:
		break;
	case TS_IDLE:
		/* connect if possible */
		if (tp->flags & (M3UA_SG_CONFIGURED | M3UA_SG_REMADDR)) {
			struct {
				struct t_opthdr oh;
				t_uscalar_t val;
			} opts;

			opts.oh.len = sizeof(opts);
			opts.oh.level = T_INET_SCTP;
			opts.oh.name = T_SCTP_OSTREAMS;
			opts.oh.status = T_NEGOTIATE;
			opts.val = tp->streams;

			return t_conn_req(tp, q, mp, (size_t) tp->rem_len, (caddr_t) &tp->rem, 0,
					  NULL, NULL);
		}
		break;
	case TS_WACK_CREQ:
	case TS_WACK_CRES:
	case TS_WCON_CREQ:
		break;
	case TS_DATA_XFER:
		/* bring up ASP if necessary */
		switch (tp_get_u_state(tp)) {
		case ASP_DOWN:
			tp_set_u_state(tp, ASP_WACK_ASPUP);
			return tp_send_asps_aspup_req(tp, q, mp, tp->aspid ? &tp->aspid : NULL,
						      NULL, 0);
		case ASP_WACK_ASPDN:
		case ASP_WACK_ASPUP:
		case ASP_UP:
			break;
		default:
			goto fault;
		}
		break;
	case TS_WACK_OPTREQ:
	case TS_WACK_DREQ6:
	case TS_WACK_DREQ7:
	case TS_WACK_DREQ9:
	case TS_WACK_DREQ10:
	case TS_WACK_DREQ11:
		break;
	case TS_WIND_ORDREL:
		break;
	case TS_WREQ_ORDREL:
		/* request orderly release */
		return t_ordrel_req(tp, q, mp, NULL);
	}
	freemsg(mp);
	return (0);
#if 0
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_ADDR_ACK out of state");
	goto error;
#endif
      fault:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_ADDR_ACK too short");
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/**
 * t_capability_ack: - process T_CAPABILITY_ACK primitive
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the primitive
 */
static noinline fastcall int
t_capability_ack(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_capability_ack *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto fault;
	if (p->CAP_bits1 & TC1_ACCEPTOR_ID)
		tp->acceptorid = p->ACCEPTOR_id;
	if (p->CAP_bits1 & TC1_INFO)
		return t_info(tp, q, mp, &p->INFO_ack);
	freemsg(mp);
	return (0);
#if 0
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_CAPABILITY_ACK out of state");
	goto error;
#endif
      fault:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_CAPABILITY_ACK too short");
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/**
 * t_other_ind: - process other TPI primitive
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the primitive
 */
static noinline fastcall int
t_other_ind(struct tp *tp, queue_t *q, mblk_t *mp)
{
	t_scalar_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto fault;
	goto unexpected;
      unexpected:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "Unexpected primitive %d", *p);
	goto error;
      fault:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "Primitive too short");
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/*
 *  TS-User (APS) Timeouts.
 *  =======================
 */
static noinline fastcall int
tp_wack_aspup_timeout(struct tp *tp, queue_t *q, mblk_t *mp)
{
	int err = 0;

	if (tp_get_u_state(tp) == ASP_WACK_ASPUP)
		if ((err = tp_send_asps_aspup_req(tp, q, NULL, tp->aspid ? &tp->aspid : NULL, NULL, 0)) == 0)
			mi_timer(mp, 2000);
	return (err);
}
static noinline fastcall int
tp_wack_aspdn_timeout(struct tp *tp, queue_t *q, mblk_t *mp)
{
	int err = 0;

	if (tp_get_u_state(tp) == ASP_WACK_ASPDN)
		if ((err = tp_send_asps_aspdn_req(tp, q, NULL, tp->aspid ? &tp->aspid : NULL, NULL, 0)) == 0)
			mi_timer(mp, 2000);
	return (err);
}
static noinline fastcall int
tp_wack_aspia_timeout(struct tp *tp, queue_t *q, mblk_t *mp)
{
	int err = 0;

#if 0
	if (tp_get_u_state(tp) == ASP_WACK_ASPIA)
		if ((err = tp_send_aspt_aspia_req(tp, q, NULL, NULL, 0)) == 0)
			mi_timer(mp, 2000);
#endif
	return (err);
}
static noinline fastcall int
tp_wack_aspac_timeout(struct tp *tp, queue_t *q, mblk_t *mp)
{
	int err = 0;

#if 0
	if (tp_get_u_state(tp) == ASP_WACK_ASPAC)
		if ((err = tp_send_aspt_aspac_req(tp, q, NULL, NULL, 0)) == 0)
			mi_timer(mp, 2000);
#endif
	return (err);
}


/*
 *  STREAMS Message Handling (upper write queue)
 *  ============================================
 */

/**
 * mtp_w_data: - process M_DATA message
 * @q: active queue (upper write queue)
 * @mp: the M_DATA message
 */
static noinline fastcall int
mtp_w_data(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	int err;

	if (mtp->asp) {
		if (!asp_trylock(mtp->asp, q))
			return (-EDEADLK);

		if (mtp_get_i_state(mtp) != MTPS_CONNECTED)
			goto outstate;
		err = mtp_send_xfer_data(mtp, tp, q, &mtp->orig, &mtp->dest, 0, 0, mp);
		asp_unlock(mtp->asp);
		return (err);
	}
      outstate:
	freemsg(mp);
	return (0);
}

/**
 * mtp_w_proto: - process M_PROTO/M_PCPROTO message
 * @q: active queue (upper write queue)
 * @mp: the M_PROTO/M_PCPROTO message
 */
static noinline fastcall int
mtp_w_proto(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	int old_i_state, old_u_state, old_a_state;
	uint32_t prim;
	int rtn = 0;

	if (mp->b_wptr < mp->b_rptr + sizeof(prim)) {
		strlog(mtp->mid, mtp->sid, 0, SL_ERROR, "-> primitive too short");
		freemsg(mp);
		return (0);
	}
	prim = *(uint32_t *) mp->b_rptr;

	if (mtp->asp == NULL) {
		/* have not been attached yet */
		if (prim == MTP_BIND_REQ)
			return mtp_bind_req(mtp, q, mp);
		return mtp_error_ack(mtp, q, mp, prim, MOUTSTATE);
	}

	if (!asp_mtp_trylock(mtp, q))
		return (-EDEADLK);

	old_i_state = mtp_get_i_state(mtp);
	old_u_state = mtp_get_u_state(mtp);
	old_a_state = asp_get_u_state(mtp->asp);

	switch (prim) {
	case MTP_BIND_REQ:
		strlog(mtp->mid, mtp->sid, MTLOGRX, SL_TRACE, "-> MTP_BIND_REQ");
		rtn = mtp_bind_req(mtp, q, mp);
		break;
	case MTP_UNBIND_REQ:
		strlog(mtp->mid, mtp->sid, MTLOGRX, SL_TRACE, "-> MTP_UNBIND_REQ");
		rtn = mtp_unbind_req(mtp, q, mp);
		break;
	case MTP_CONN_REQ:
		strlog(mtp->mid, mtp->sid, MTLOGRX, SL_TRACE, "-> MTP_CONN_REQ");
		rtn = mtp_conn_req(mtp, q, mp);
		break;
	case MTP_DISCON_REQ:
		strlog(mtp->mid, mtp->sid, MTLOGRX, SL_TRACE, "-> MTP_DISCON_REQ");
		rtn = mtp_discon_req(mtp, q, mp);
		break;
	case MTP_ADDR_REQ:
		strlog(mtp->mid, mtp->sid, MTLOGRX, SL_TRACE, "-> MTP_ADDR_REQ");
		rtn = mtp_addr_req(mtp, q, mp);
		break;
	case MTP_INFO_REQ:
		strlog(mtp->mid, mtp->sid, MTLOGRX, SL_TRACE, "-> MTP_INFO_REQ");
		rtn = mtp_info_req(mtp, q, mp);
		break;
	case MTP_OPTMGMT_REQ:
		strlog(mtp->mid, mtp->sid, MTLOGRX, SL_TRACE, "-> MTP_OPTMGMT_REQ");
		rtn = mtp_optmgmt_req(mtp, q, mp);
		break;
	case MTP_TRANSFER_REQ:
		strlog(mtp->mid, mtp->sid, MTLOGRX, SL_TRACE, "-> MTP_TRANSFER_REQ");
		rtn = mtp_transfer_req(mtp, q, mp);
		break;
	default:
		strlog(mtp->mid, mtp->sid, MTLOGRX, SL_TRACE, "-> MTP_????_???");
		rtn = mtp_other_req(mtp, q, mp);
		break;
	}
	if (rtn < 0) {
		mtp_set_i_state(mtp, old_i_state);
		mtp_set_u_state(mtp, old_u_state);
		asp_set_u_state(mtp->asp, old_a_state);
	}
	asp_unlock(mtp->asp);
	return (rtn);
}

/**
 * mtp_w_ctl: - process M_CTL message
 * @q: active queue (upper write queue)
 * @mp: the M_CTL message
 *
 * Eventually we process M_CTL messages like M_IOCTL messages, but passed from module to module.
 * For now we just discard them.
 */
static noinline fastcall int
mtp_w_ctl(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);

	strlog(mtp->mid, mtp->sid, 0, SL_ERROR, "unexpected M_CTL message on upper write queue");
	freemsg(mp);
	return (0);
}

/**
 * mtp_w_ioctl: - process M_IOCTL message
 * @q: active queue (upper write queue)
 * @mp: the M_IOCTL message
 *
 * For I_(P)LINK and I_(P)UNLINK operations, the tp structure was already allocated when the upper
 * stream was opened on the multiplexing driver.  We simply attach or detach the linked queue pair
 * from the structure.  There are some other input-output controls for configuration that we need to
 * handle later, but we just return EINVAL for any other input-output control for now.
 */
static noinline fastcall int
mtp_w_ioctl(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;

	if (!mp->b_cont)
		goto einval;

	switch (_IOC_TYPE(ioc->ioc_cmd)) {
	case __SID:
		switch (_IOC_NR(ioc->ioc_cmd)) {
		case _IOC_NR(I_LINK):
			return mtp_i_link(mtp, q, mp);
		case _IOC_NR(I_UNLINK):
			return mtp_i_unlink(mtp, q, mp);
		case _IOC_NR(I_PLINK):
			return mtp_i_plink(mtp, q, mp);
		case _IOC_NR(I_PUNLINK):
			return mtp_i_punlink(mtp, q, mp);
		}
		break;
	default:
		break;
	}
      einval:
	mi_copy_done(q, mp, EINVAL);
	return (0);
}

/**
 * mtp_w_iocdata: - process M_IOCDATA message
 * @q: active queue (upper write queue)
 * @mp: the M_IOCDATA message
 */
static noinline fastcall int
mtp_w_iocdata(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	mblk_t *dp;

	switch (mi_copy_state(q, mp, &dp)) {
	case -1:
		return (0);
	default:
		mi_copy_done(q, mp, EFAULT);
		return (0);
	}
}

/**
 * mtp_w_sig: - process M_SIG/M_PCSIG message
 * @q: active queue (upper write queue)
 * @mp: the M_SIG/M_PCSIG message
 *
 * M_PCSIG messages are issued from mi timers.
 */
static noinline fastcall int
mtp_w_sig(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	int rtn = 0;

	if (!asp_mtp_trylock(mtp, q))
		return (-EDEADLK);
	if (likely(mi_timer_valid(mp))) {
		switch (*(int *) mp->b_rptr) {
		case wack_aspac:
			strlog(mtp->mid, mtp->sid, MTLOGTO, SL_TRACE, "-> WACK ASPAC TIMEOUT <-");
			rtn = mtp_wack_aspac_timeout(mtp, q, mp);
			break;
		case wack_aspia:
			strlog(mtp->mid, mtp->sid, MTLOGTO, SL_TRACE, "-> WACK ASPIA TIMEOUT <-");
			rtn = mtp_wack_aspia_timeout(mtp, q, mp);
			break;
		default:
			strlog(mtp->mid, mtp->sid, 0, SL_ERROR, "invalid timer %d",
			       *(int *) mp->b_rptr);
			rtn = 0;
			break;
		}
	}
	asp_mtp_unlock(mtp);
	return (rtn);
}

/**
 * mtp_w_flush: - process M_FLUSH message
 * @q: active queue (upper write queue)
 * @mp: the M_FLUSH message
 * 
 * What we should probably do here is send a BEAT message on the SCTP stream id that corresponds to
 * the MTP user and the return the flush message back upstream only after we get the BEAT ACK.
 * (Well, no, holding a priority message will accomplish nothing.
 *
 * Note that we do not queue messages to the upper read side queue of the upper queue pair.
 * Therefore we do not have to flush that queue.
 */
static noinline fastcall int
mtp_w_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		mp->b_rptr[0] &= ~FLUSHW;
	}
	if (mp->b_rptr[0] & FLUSHR) {
		qreply(q, 0);
		return (0);
	}
	freemsg(mp);
	return (0);
}

/**
 * mtp_w_other: - process other message
 * @q: active queue (upper write queue)
 * @mp: the other message
 */
static noinline fastcall int
mtp_w_other(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);

	strlog(mtp->mid, mtp->sid, 0, SL_ERROR | SL_TRACE,
			"unhandled STREAMS message on write queue");
	freemsg(mp);
	return (0);
}

static noinline fastcall int
mtp_w_msg_slow(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
		return mtp_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return mtp_w_proto(q, mp);
	case M_CTL:
		return mtp_w_ctl(q, mp);
	case M_IOCTL:
		return mtp_w_ioctl(q, mp);
	case M_IOCDATA:
		return mtp_w_iocdata(q, mp);
	case M_SIG:
	case M_PCSIG:
		return mtp_w_sig(q, mp);
	case M_FLUSH:
		return mtp_w_flush(q, mp);
	default:
		return mtp_w_other(q, mp);
	}
}

static inline fastcall int
mtp_w_msg(queue_t *q, mblk_t *mp)
{
	/* FIXME: write fast path */
	return mtp_w_msg_slow(q, mp);
}

/**
 * tp_r_data: - process M_DATA message
 * @q: active queue (lower read queue)
 * @mp: the M_DATA message
 */
static noinline fastcall int
tp_r_data(queue_t *q, mblk_t *mp)
{
}

/**
 * tp_r_proto: - process M_PROTO/M_PCPROT message
 * @q: active queue (lower read queue)
 * @mp: the M_PROTO/M_PCPROT message
 */
static noinline fastcall int
tp_r_proto(queue_t *q, mblk_t *mp)
{
	struct tp *tp = TP_PRIV(q);
	uint old_i_state, old_u_state;
	uint32_t prim;
	int rtn;

	if (mp->b_wptr < mp->b_rptr + sizeof(prim)) {
		strlog(tp->mid, tp->sid, 0, SL_TRACE | SL_ERROR, "primitive too short <-");
		freemsg(mp);
		return (0);
	}
	prim = *(uint32_t *)mp->b_rptr;

	if (!tp_trylock(tp, q))
		return (-EDEADLK);

	old_i_state = tp_get_i_state(tp);
	old_u_state = tp_get_u_state(tp);

	if (!pcmsg(DB_TYPE(mp)) && old_i_state == -1) {
		/* If we have not yet received a response to our T_INFO_REQ and this is not the
		   response, wait for the response (one _is_ coming).  Of course, if this is a
		   priority message it must be processed immediately anyway. */
		tp_unlock(tp);
		return (-EAGAIN);
	}
	switch (prim) {
	case T_CONN_IND:
		strlog(tp->mid, tp->sid, SLLOGRX, SL_TRACE, "T_CONN_IND <-");
		rtn = t_conn_ind(tp, q, mp);
		break;
	case T_CONN_CON:
		strlog(tp->mid, tp->sid, SLLOGRX, SL_TRACE, "T_CONN_CON <-");
		rtn = t_conn_con(tp, q, mp);
		break;
	case T_DISCON_IND:
		strlog(tp->mid, tp->sid, SLLOGRX, SL_TRACE, "T_DISCON_IND <-");
		rtn = t_discon_ind(tp, q, mp);
		break;
	case T_DATA_IND:
		strlog(tp->mid, tp->sid, SLLOGRX, SL_TRACE, "T_DATA_IND <-");
		rtn = t_data_ind(tp, q, mp);
		break;
	case T_EXDATA_IND:
		strlog(tp->mid, tp->sid, SLLOGRX, SL_TRACE, "T_EXDATA_IND <-");
		rtn = t_exdata_ind(tp, q, mp);
		break;
	case T_INFO_ACK:
		strlog(tp->mid, tp->sid, SLLOGRX, SL_TRACE, "T_INFO_ACK <-");
		rtn = t_info_ack(tp, q, mp);
		break;
	case T_BIND_ACK:
		strlog(tp->mid, tp->sid, SLLOGRX, SL_TRACE, "T_BIND_ACK <-");
		rtn = t_bind_ack(tp, q, mp);
		break;
	case T_ERROR_ACK:
		strlog(tp->mid, tp->sid, SLLOGRX, SL_TRACE, "T_ERROR_ACK <-");
		rtn = t_error_ack(tp, q, mp);
		break;
	case T_OK_ACK:
		strlog(tp->mid, tp->sid, SLLOGRX, SL_TRACE, "T_OK_ACK <-");
		rtn = t_ok_ack(tp, q, mp);
		break;
	case T_UNITDATA_IND:
		strlog(tp->mid, tp->sid, SLLOGRX, SL_TRACE, "T_UNITDATA_IND <-");
		rtn = t_unitdata_ind(tp, q, mp);
		break;
	case T_UDERROR_IND:
		strlog(tp->mid, tp->sid, SLLOGRX, SL_TRACE, "T_UDERROR_IND <-");
		rtn = t_uderror_ind(tp, q, mp);
		break;
	case T_OPTMGMT_ACK:
		strlog(tp->mid, tp->sid, SLLOGRX, SL_TRACE, "T_OPTMGMT_ACK <-");
		rtn = t_optmgmt_ack(tp, q, mp);
		break;
	case T_ORDREL_IND:
		strlog(tp->mid, tp->sid, SLLOGRX, SL_TRACE, "T_ORDREL_IND <-");
		rtn = t_ordrel_ind(tp, q, mp);
		break;
	case T_OPTDATA_IND:
		strlog(tp->mid, tp->sid, SLLOGRX, SL_TRACE, "T_OPTDATA_IND <-");
		rtn = t_optdata_ind(tp, q, mp);
		break;
	case T_ADDR_ACK:
		strlog(tp->mid, tp->sid, SLLOGRX, SL_TRACE, "T_ADDR_ACK <-");
		rtn = t_addr_ack(tp, q, mp);
		break;
	case T_CAPABILITY_ACK:
		strlog(tp->mid, tp->sid, SLLOGRX, SL_TRACE, "T_CAPABILITY_ACK <-");
		rtn = t_capability_ack(tp, q, mp);
		break;
	default:
		strlog(tp->mid, tp->sid, SLLOGRX, SL_TRACE, "N_????_??? <-");
		rtn = t_other_ind(tp, q, mp);
		break;
	}
	if (rtn < 0) {
		tp_set_i_state(tp, old_i_state);
		tp_set_u_state(tp, old_u_state);
	}
	tp_unlock(tp);
	return (rtn);
}

/**
 * tp_r_sig: - process M_SIG/M_PCSIG message
 * @q: active queue (lower read queue)
 * @mp: the M_SIG/M_PCSIG message
 */
static noinline fastcall int
tp_r_sig(queue_t *q, mblk_t *mp)
{
}

/**
 * tp_r_ctl: - process M_CTL message
 * @q: active queue (lower read queue)
 * @mp: the M_CTL message
 */
static noinline fastcall int
tp_r_ctl(queue_t *q, mblk_t *mp)
{
}

/**
 * tp_r_error: - process M_ERROR/M_HANGUP/M_UNHANGUP message
 * @q: active queue (lower read queue)
 * @mp: the M_ERROR/M_HANGUP/M_UNHANGUP message
 */
static noinline fastcall int
tp_r_error(queue_t *q, mblk_t *mp)
{
}

/**
 * tp_r_iocack: - process M_IOCACK/M_IOCNAK message
 * @q: active queue (lower read queue)
 * @mp: the M_IOCACK/M_IOCNAK message
 */
static noinline fastcall int
tp_r_iocack(queue_t *q, mblk_t *mp)
{
}

/**
 * tp_r_copy: - process M_COPYIN/M_COPYOUT message
 * @q: active queue (lower read queue)
 * @mp: the M_COPYIN/M_COPYOUT message
 */
static noinline fastcall int
tp_r_copy(queue_t *q, mblk_t *mp)
{
}

/**
 * tp_r_flush: - process M_FLUSH message
 * @q: active queue (lower read queue)
 * @mp: the M_FLUSH message
 *
 * Note that we never queue messages on the lower write side queue of the lower queue pair.
 * Therefore, it is not necessary to flush that queue.
 */
static noinline fastcall int
tp_r_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		mp->b_rptr[0] &= ~FLUSHR;
	}
	if (mp->b_rptr[0] & FLUSHW) {
		qreply(q, 0);
		return (0);
	}
	freemsg(mp);
	return (0);
}

/**
 * tp_r_other: - process other message
 * @q: active queue (lower read queue)
 * @mp: the other message
 */
static noinline fastcall int
tp_r_other(queue_t *q, mblk_t *mp)
{
}

static noinline fastcall int
tp_r_msg_slow(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
		return tp_r_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return tp_r_proto(q, mp);
	case M_SIG:
	case M_PCSIG:
		return tp_r_sig(q, mp);
	case M_CTL:
		return tp_r_ctl(q, mp);
	case M_ERROR:
	case M_HANGUP:
	case M_UNHANGUP:
		return tp_r_error(q, mp);
	case M_IOCACK:
	case M_IOCNAK:
		return tp_r_iocack(q, mp);
	case M_COPYIN:
	case M_COPYOUT:
		return tp_r_copy(q, mp);
	case M_FLUSH:
		return tp_r_flush(q, mp);
	default:
		return tp_r_other(q, mp);
	}
}

static inline fastcall int
tp_r_msg(queue_t *q, mblk_t *mp)
{
	/* FIXME: write fast path */
	return tp_r_msg_slow(q, mp);
}

/*
 *  PUT and SERVICE procedures
 */

/**
 * mtp_wput: - upper write put procedure
 * @q: queue to which to put message
 * @mp: message to put
 *
 * The upper write put procedure is responsible for accepting messages from MTP streams above the
 * multiplexing driver.  This is a simple canonical queueing put procedure.
 */
static streamscall __hot_write int
mtp_wput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || mtp_w_msg(q, mp))
		putq(q, mp);
	return (0);
}

/**
 * mtp_wsrv: - upper write service procedure
 * @q: queue to service
 *
 * The upper write service procedure is a canonical draining service procedure.
 */
static streamscall __hot_put int
mtp_wsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (mtp_w_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}

/**
 * mtp_rsrv: - upper read service procedure
 * @q: queue to service
 *
 * The upper read queue service procedure is simply for backenabling across the multiplexing driver.
 * This procedure is only invoked when the upstream queue becomes backenabled.  This means that flow
 * control is lifted and we backenable across the multiplexing driver each queue feeding this queue.
 */
static streamscall __hot_read int
mtp_rsrv(queue_t *q)
{
	struct mtp *mtp = MTP_PRIV(q);
	struct tp *tp;

	read_lock(&mtp_mux_lock);
	for (tp = mtp->tp.list; tp; tp = tp->mtp.next)
		qenable(tp->rq);
	read_unlock(&mtp_mux_lock);
}

/**
 * mtp_rput: - upper read put procedure
 * @q: queue to which to put message
 * @mp: message to put
 *
 * The upper read queue is internal to the multiplexing driver and messages are never queued to this
 * queue, but are always put to the next queue.  It is an error to call the put procedure for this
 * queue.
 */
static streamscall int
mtp_rput(queue_t *q, mblk_t *mp)
{
	/* error */
	putnext(q, mp);
	return (0);
}

/**
 * tp_wput: - lower write put procedure
 * @q: queue to which to put message
 * @mp: message to put
 *
 * The lower write queue is internal to the multiplexing driver and messages are never queued to this
 * queue, but are always put to the next queue.  It is an error to call the put procedure for this
 * queue.
 */
static streamscall int
tp_wput(queue_t *q, mblk_t *mp)
{
	/* error */
	putnext(q, mp);
	return (0);
}

/**
 * tp_wsrv: - lower write service procedure
 * @q: queue to service
 *
 * The lower write queue service procedure is simply for backenabling across the multiplexing driver.
 * This procedure is only invoked when the downstream queue becomes backenabled.  This means that flow
 * control is lifted and we backenable across the multiplexing driver each queue feeding this queue.
 */
static streamscall __hot_out int
tp_wsrv(queue_t *q)
{
	struct tp *tp = TP_PRIV(q);
	struct mtp *mtp;

	read_lock(&mtp_mux_lock);
	for (mtp = tp->mtp.list; mtp; mtp = mtp->tp.next)
		qenable(mtp->wq);
	read_unlock(&mtp_mux_lock);
}

/**
 * tp_rsrv: - lower read service procedure
 * @q: queue to service
 *
 * The lower read service procedure is a canonical draining service procedure.
 */
static streamscall __hot_read int
tp_rsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (tp_r_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}

/**
 * tp_rput: - lower read put procedure
 * @q: queue to which to put message
 * @mp: message to put
 *
 * The lower read put procedure is responsible for accepting messages from SCTP streams beneath the
 * multiplexing driver.  This is a simple canonical queueing put procedure.
 */
static streamscall __hot_in int
tp_rput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || tp_r_msg(q, mp))
		putq(q, mp);
	return (0);
}

/*
 *  OPEN and CLOSE
 */

#ifndef NUM_AS
#define NUM_AS 4096
#endif

/**
 * mtp_qopen - driver open routine
 * @q: newly created queue pair
 * @devp: pointer to device number associated with Stream
 * @oflags: flags to the open(2) call
 * @sflag: STREAMS flag
 * @crp: pointer to the credentials of the opening process
 *
 * When a Stream is opened on the driver it corresponds to an AS assocaited with a given ASP.  The
 * ASP is determined from the minor device number opened.  All minor devices corresponding to ASPs
 * are clone devices.  There are multiple SCTP lower Streams for each ASP (for M3UA).  If an TP
 * structure has not been allocated for the corresponding minor device number, we allocate one.
 * When an SCTP Stream is I_LINK'ed under the driver, it is associated with the TP structure.  An MTP
 * structure is allocated and associated with each each upper Stream.
 *
 * This driver can also be pushed as a module.  When pushed as a module it is pushed over an SCTP
 * Stream.  When pushed as a module, there exists precisely one AS.  (It takes a multiplexing driver
 * to provide more than on AS for a given SCTP association.)  This is a simplified case.  We will
 * handle this one later, but instead of allocating an MTP structure and an TP structure, they should
 * be allocated together.
 */
static streamscall int
mtp_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	mblk_t *t1, *t2;
	minor_t cminor;
	major_t cmajor;
	struct mtp *mtp;
	int err;

	if (q->q_ptr != NULL)
		return (0);	/* already open */
	if (sflag == MODOPEN || q->q_next)
		return (ENXIO);

	cmajor = getmajor(*devp);
	cminor = getminor(*devp);

	if (cminor > NUM_AS)
		return (ENXIO);

	*devp = makedevice(cmajor, NUM_AS + 1);
	sflag = CLONEOPEN;

	if (!(t1 = mi_timer_alloc(WR(q), sizeof(int))))
		return (ENOBUFS);
	if (!(t2 = mi_timer_alloc(WR(q), sizeof(int)))) {
		mi_timer_free(t1);
		return (ENOBUFS);
	}

	write_lock(&mtp_mux_lock);
	if ((err = mi_open_comm(&mtp_opens, sizeof(*mtp), q, devp, oflags, sflag, crp))) {
		write_unlock(&mtp_mux_lock);
		mi_timer_free(t1);
		mi_timer_free(t2);
		return (err);
	}
	/* initialize mtp structure */
	mtp = SL_PRIV(q);
	mtp_init_priv(mtp, q, devp, oflags, sflag, crp, t1, t2, cminor);
	write_unlock(&mtp_mux_lock);
	return (0);
}

/**
 * mtp_qclose - driver close routine
 * @q: queue pair
 * @oflags: flags to the open(2) call
 * @crp: pointer to the credentials of the closing process
 */
static streamscall int
mtp_qclose(queue_t *q, int oflags, cred_t *crp)
{
	struct mtp *mtp = MTP_PRIV(q);

	qprocsoff(q);
	write_lock(&mtp_mux_lock);
	mi_detach(q, (caddr_t) mtp);
	mtp_term_priv(mtp);
	mi_close_detached(&mtp_opens, (caddr_t) mtp);
	write_unlock(&mtp_mux_lock);
	return (0);
}


/*
 *  STREAMS Definitions.
 */

static struct module_stat mtp_mstat __attribute__ ((aligned(SMP_CACHE_BYTES)));

static struct module_info tp_minfo = {
	.mi_idnum = DRV_ID,
	.mi_idname = DRV_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat tp_mstat __attribute__ ((aligned(SMP_CACHE_BYTES)));

static struct qinit mtp_rinit = {
	.qi_putp = mtp_rput,
	.qi_srvp = mtp_rsrv,
	.qi_qopen = mtp_qopen,
	.qi_qclose = mtp_qclose,
	.qi_minfo = &mtp_minfo,
	.qi_mstat = &mtp_mstat,
};

static struct qinit mtp_winit = {
	.qi_putp = mtp_wput,
	.qi_srvp = mtp_wsrv,
	.qi_minfo = &mtp_minfo,
	.qi_mstat = &mtp_mstat,
};

static struct qinit tp_rinit = {
	.qi_putp = tp_rput,
	.qi_srvp = tp_rsrv,
	.qi_minfo = &tp_minfo,
	.qi_mstat = &tp_mstat,
};

static struct qinit tp_winit = {
	.qi_putp = tp_wput,
	.qi_srvp = tp_wsrv,
	.qi_minfo = &tp_minfo,
	.qi_mstat = &tp_mstat,
};

static struct streamtab m3ua_asinfo = {
	.st_rdinit = &mtp_rinit,
	.st_wrinit = &mtp_winit,
	.st_muxrinit = &tp_rinit,
	.st_muxwinit = &tp_winit,
};

STATIC struct cdevsw m3ua_cdev = {
	.d_name = DRV_NAME,
	.d_str = &m3ua_asinfo,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

unsigned short modid = DRV_ID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0);
#endif
MODULE_PARM_DESC(modid, "Module ID for the M3UA-AS driver. (0 for allocation.)");

unsigned short major = DRV_CMAJOR;

#ifndef module_param
MODULE_PARM(major, "h");
#else				/* module_param */
module_param(major, ushort, DRV_CMAJOR);
#endif				/* module_param */
MODULE_PARM_DESC(major, "Major device number for the M3UA-AS driver.  (0 for allocation.)");

static __init int
m3ua_asinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_SPLASH);	/* console splash */
	if ((err = register_strdev(&m3_cdev, major)) < 0) {
		cmn_err(CE_WARN, "%s: could not register major %d\n", MOD_NAME, (int) major);
		return (err);
	}
	if (major == 0)
		major = err;
	return (0);
}

static __exit void
m3ua_asexit(void)
{
	int err;

	if ((err = unregister_strdev(&m3_cdev, major)) < 0)
		cmn_err(CE_WARN, "%s: could not unregister driver, err = %d\n", MOD_NAME, err);
	return;
}

module_init(m3ua_asinit);
module_exit(m3ua_asexit);
