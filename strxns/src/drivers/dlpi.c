/*****************************************************************************

 @(#) $RCSfile: dlpi.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-06-18 16:43:16 $

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

 Last Modified $Date: 2008-06-18 16:43:16 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: dlpi.c,v $
 Revision 0.9.2.1  2008-06-18 16:43:16  brian
 - added new files for NLI and DLPI

 *****************************************************************************/

#ident "@(#) $RCSfile: dlpi.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-06-18 16:43:16 $"

static char const ident[] = "$RCSfile: dlpi.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-06-18 16:43:16 $";

/*
 * Welcome to the final interation of the Linux DL driver.  This driver opens
 * any Linux networking device in a raw, connectionless or connection-oriented
 * fashion.  Of primary interest are all DLPI supported things like Ethernet,
 * LLC1, LLC2, HDLC(RAW), ASYNC(RAW), HDLC, LAPB, LAPD, LAPF, LAPM, SDLC.  This
 * is accomplished in two ways: for Linux native network devices, we hook to
 * the generic Linux packet layer for frames and then superimpose the raw,
 * connectionless or connection-oriented protocol on top.  For STREAMS devices,
 * we link them under this multiplexing driver and assign them as a Linux
 * networking device.  This makes them available to the Linux kernel and to
 * this driver.  If the attached PPA is a Linux native network device, we
 * attach internally to that device.  If the attached PPA is a STREAMS device,
 * we link directly to the bottom multiplex.  In this fashion, Linux has access
 * to any device we link under the multiplexing driver (and properly
 * configure), and STREAMS has access to all devices.
 *
 * The primary purpose of this driver for now is LLC2 and LAPB.  But the driver
 * also supports Ethernet and 802.2 SNAP in the fashion of the older LDL driver
 * that has become so difficult to maintain.
 */

#define _MPS_SOURCE

#include <sys/os7/compat.h>	/* STREAMS compatibility header */
#include <sys/strsuh.h>		/* some SUN helper macros */

/* We want _all_ the DLPI definitions */
#define _SUN_SOURCE
#define _HPUX_SOURCE
#define _AIX_SOURCE

#include <sys/dlpi.h>		/* DLPI Revision 2.0.0. interface */
#include <sys/dlpi_ext.h>	/* HP-UX extensions */
#include <sys/dlpi_sun.h>	/* Solaris and Solstice extensions */
#include <sys/dlpi_ioctl.h>	/* a wide range of compatibility IOCTLs */

#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <net/protocol.h>

#define DLPI_DESCRIP	"UNIX SVR 4.2 DLPI DRIVER FOR LINUX FAST-STREAMS"
#define DLPI_EXTRA	"Part of the OpenSS7 Protocol Stacks for Linux Fast-STREAMS"
#define DLPI_COPYRIGHT	"Copyright (c) 1997-2008  OpenSS7 Corporation.  All Rights Reserved."
#define DLPI_REVISION	"OpenSS7 $RCSfile: dlpi.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-06-18 16:43:16 $"
#define DLPI_DEVICE	"SVR 4.2MP DLPI Driver (DLPI)"
#define DLPI_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define DLPI_LICENSE	"GPL"
#define DLPI_BANNER	DLPI_DESCRIP	"\n" \
			DLPI_EXTRA	"\n" \
			DLPI_COPYRIGHT	"\n" \
			DLPI_REVISION	"\n" \
			DLPI_DEVICE	"\n" \
			DLPI_CONTACT	"\n"
#define DLPI_SPLASH	DLPI_DESCRIP	" - " \
			DLPI_REVISION

#ifndef CONFIG_STREAMS_DLPI_NAME
#error "CONFIG_STREAMS_DLPI_NAME must be defined."
#endif				/* CONFIG_STREAMS_DLPI_NAME */
#ifndef CONFIG_STREAMS_DLPI_MODID
#error "CONFIG_STREAMS_DLPI_MODID must be defined."
#endif				/* CONFIG_STREAMS_DLPI_MODID */

#ifdef LINUX
#ifdef MODULE
MODULE_AUTHOR(DLPI_CONTACT);
MODULE_DESCRIPTION(DLPI_DESCRIP);
MODULE_SUPPORTED_DEVICE(DLPI_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(DLPI_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
#ifdef LFS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_DLPI_MODID));
MODULE_ALIAS("streams-driver-dlpi");
MODULE_ALIAS("streams-module-dlpi");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_DLPI_MAJOR));
MODULE_ALIAS("/dev/streams/dlpi");
MODULE_ALIAS("/dev/streams/dlpi/*");
MODULE_ALIAS("/dev/streams/dlpi/eth");
MODULE_ALIAS("/dev/streams/dlpi/llc");
MODULE_ALIAS("/dev/streams/dlpi/hdlc");
MODULE_ALIAS("/dev/streams/dlpi/lapb");
#endif				/* LFS */
MODULE_ALIAS("char-major-" __stringify(DLPI_MAJOR_0));
MODULE_ALIAS("char-major-" __stringify(DLPI_MAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(DLPI_MAJOR_0) "-0");
MODULE_ALIAS("char-major-" __stringify(DLPI_MAJOR_0) "-1");
MODULE_ALIAS("char-major-" __stringify(DLPI_MAJOR_0) "-2");
MODULE_ALIAS("char-major-" __stringify(DLPI_MAJOR_0) "-3");
MODULE_ALIAS("char-major-" __stringify(DLPI_MAJOR_0) "-4");
MODULE_ALIAS("char-major-" __stringify(DLPI_MAJOR_0) "-" __stringify(DLPI_CMINOR));
MODULE_ALIAS("/dev/eth");
MODULE_ALIAS("/dev/llc");
MODULE_ALIAS("/dev/hdlc");
MODULE_ALIAS("/dev/lapb");
#endif				/* MODULE_ALIAS */
#endif				/* MODULE */
#endif				/* LINUX */

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS DEFINITIONS
 *
 * --------------------------------------------------------------------------
 */

#define DRV_ID		DLPI_DRV_ID
#define DRV_NAME	DLPI_DRV_NAME
#define CMAJORS		DLPI_CMAJORS
#define CMAJOR_0	DLPI_CMAJOR_0
#define UNITS		DLPI_UNITS
#ifdef MODULE
#define DRV_BANNER	DLPI_BANNER
#else				/* MODULE */
#define DRV_BANNER	DLPI_SPLASH
#endif				/* MODULE */

static struct module_info dl_minfo = {
	.mi_idnum = DRV_ID,
	.mi_idname = DRV_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat dl_urstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat dl_uwstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat dl_lrstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat dl_lwstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

/*
 * --------------------------------------------------------------------------
 *
 * PRIVATE STRUCTURE
 *
 * --------------------------------------------------------------------------
 */

struct list_head dl_ppa_list;		/* list of all PPAs */

/* PPA structure */
struct ppa {
	dl_ulong ppa;
};

struct list_head dl_sap_list;		/* list of all SAPs */

/* SAP structure */
struct sap {
	struct packet_type pt;		/* make this first */
	struct list_head sap_list;	/* all SAPs */
	struct list_head stn_list;	/* all SAPs for this STN */
	struct stn *stn;		/* STN for this SAP */
	struct list_head dl_list;	/* all SAPs for this DL */
	struct dl *dl;			/* DL for this SAP */
	/* SAP information */
	dl_ulong lsap;			/* LSAP (ssap or ethertype) */
	uint8_t len;
	uint8_t sap[MAX_SAP_LEN];
};

struct list_head dl_stn_list;		/* list of all stations */

/* STN structure */
struct stn {
};

struct dlops {
	fastcall int (*dl_sap_check) (struct dl *, dl_ulong);
	fastcall int (*dl_bind) (struct dl *, struct sap *);
	fastcall int (*dl_subs_bind) (struct dl *, queue_t *, caddr_t, size_t, dl_ulong);
	fastcall int (*dl_snddata) (struct dl *, queue_t *, mblk_t *, dl_ulong, mblk_t *);
	fastcall int (*dl_subs_unbind) (struct dl *, queue_t *, caddr_t, size_t);
	fastcall int (*dl_unbind) (struct dl *);
	fastcall int (*dl_setbusy) (struct dl *, queue_t *, dl_ulong);
	fastcall int (*dl_detach) (struct dl *);
	fastcall int (*dl_multicast) (struct dl *, dl_ulong, caddr_t, size_t);
	/**< dl_multicast: enable or disable multicast address */
	fastcall int (*dl_promisc) (struct dl *, dl_ulong, dl_ulong);
	/**< dl_promisc: set promiscious on or off at level depending on the primitive */
	fastcall int (*dl_unitdata) (struct dl *, queue_t *, mblk_t *, caddr_t, size_t, mblk_t *);
	fastcall int (*dl_udqos) (struct dl *, queue_t *, caddr_t, size_t);
	fastcall int (*dl_accept) (struct dl *, queue_t *, dl_ulong, dl_ulong, caddr_t, size_t);
	fastcall int (*dl_xidtest) (struct dl *, queue_t *, mblk_t *, dl_ulong, dl_ulong, caddr_t,
				    size_t, mblk_t *);
};

#define dl_op_sap_check(dl,sap)			(*dl->ops->dl_sap_check)(dl, sap)
#define dl_op_bind(dl)				(*dl->ops->dl_bind)(dl)
#define dl_op_subs_bind(dl,q,sap,len,cls)	(*dl->ops->dl_subs_bind)(dl,q,sap,len,cls)
#define dl_op_snddata(dl,q,msg,prim,dat)	(*dl->ops->dl_snddata)(dl,q,msg,prim,dat)
#define dl_op_subs_unbind(dl,sap,len)		(*dl->ops->dl_subs_unbind)(dl,sap,len)
#define dl_op_unbind(dl)			(*dl->ops->dl_unbind)(dl)
#define dl_op_setbusy(dl,q,prim)		(*dl->ops->dl_setbusy)(dl,q,prim)
#define dl_op_detach(dl)			(*dl->ops->dl_detach)(dl)
#define dl_op_multicast(dl,prim,add,len)	(*dl->ops->dl_multicast)(dl,prim,add,len)
#define dl_op_promisc(dl,prim,level)		(*dl->ops->dl_promisc)(dl,prim,level)
#define dl_op_unitdata(dl)			(*dl->ops->dl_unitdata)(dl)
#define dl_op_udqos(dl)				(*dl->ops->dl_udqos)(dl)
#define dl_op_accept(dl)			(*dl->ops->dl_accept)(dl)
#define dl_op_xidtest(dl)			(*dl->ops->dl_xidtest)(dl)

struct dl {
	struct list_head sap_list;	/* all SAPs for this DL */
	struct sap *sap;		/* current SAP */
	struct dl *dl;			/* attached lower datalink (if any) */
	queue_t *oq;			/* output queue (upper RD, lower WR) */
	dl_ulong state;			/* current DLPI state */
	dl_ulong oldstate;		/* previous DLPI state */
	dl_ulong conmax;		/* maximum connection indications */
	dl_ulong conind;		/* connection indications */
	dl_ulong resind;		/* reset indications */
	dl_ulong xidtest;		/* autoxid and autotest flags */
	bufq_t conq;			/* connection indications mblks */
	bufq_t resq;			/* reset indication mblks */
	struct {
		dl_hp_info_ack_t hp_info;	/* DLPI CO provider information */
		dl_info_ack_t info;	/* DLPI provider information */
		struct {
			dl_qos_co_sel1_t qos;	/* CO QOS select */
			dl_qos_co_range1_t qor;	/* CO QOS range */
		} co;
		struct {
			dl_qos_cl_sel1_t qos;	/* CL QOS select */
			dl_qos_cl_range1_t qor;	/* CL QOS range */
		} cl;
		uint8_t add[20];	/* locally bound address */
		uint8_t brd[20];	/* broadcast address */
	} proto;
	dl_ulong ppa;			/* attached PPA */
	struct strbuf sta;		/* statistics */
	struct dlops *ops;		/* device specific operations */
};

#define DL_PRIV(q) ((struct dl *)q->q_ptr)

#define STRLOGIO    0		/* ioctls */
#define STRLOGST    1		/* state transisions */
#define STRLOGTO    2		/* timeouts */
#define STRLOGRX    3		/* primitives received */
#define STRLOGTX    4		/* primtiives issued */
#define STRLOGTE    5		/* timer events */
#define STRLOGDA    6		/* data */

static inline const char *
dl_iocname(int cmd)
{
	switch (cmd) {
	default:
		return ("(unknown)");
	}
}

static inline const char *
dl_primname(dl_ulong prim)
{
	switch (prim) {
	case DL_INFO_REQ:
		return ("DL_INFO_REQ");
	case DL_INFO_ACK:
		return ("DL_INFO_ACK");
	case DL_ATTACH_REQ:
		return ("DL_ATTACH_REQ");
	case DL_DETACH_REQ:
		return ("DL_DETACH_REQ");
	case DL_BIND_REQ:
		return ("DL_BIND_REQ");
	case DL_BIND_ACK:
		return ("DL_BIND_ACK");
	case DL_UNBIND_REQ:
		return ("DL_UNBIND_REQ");
	case DL_OK_ACK:
		return ("DL_OK_ACK");
	case DL_ERROR_ACK:
		return ("DL_ERROR_ACK");
	case DL_SUBS_BIND_REQ:
		return ("DL_SUBS_BIND_REQ");
	case DL_SUBS_BIND_ACK:
		return ("DL_SUBS_BIND_ACK");
	case DL_SUBS_UNBIND_REQ:
		return ("DL_SUBS_UNBIND_REQ");
	case DL_ENABMULTI_REQ:
		return ("DL_ENABMULTI_REQ");
	case DL_DISABMULTI_REQ:
		return ("DL_DISABMULTI_REQ");
	case DL_PROMISCON_REQ:
		return ("DL_PROMISCON_REQ");
	case DL_PROMISCOFF_REQ:
		return ("DL_PROMISCOFF_REQ");
	case DL_UNITDATA_REQ:
		return ("DL_UNITDATA_REQ");
	case DL_UNITDATA_IND:
		return ("DL_UNITDATA_IND");
	case DL_UDERROR_IND:
		return ("DL_UDERROR_IND");
	case DL_UDQOS_REQ:
		return ("DL_UDQOS_REQ");
	case DL_CONNECT_REQ:
		return ("DL_CONNECT_REQ");
	case DL_CONNECT_IND:
		return ("DL_CONNECT_IND");
	case DL_CONNECT_RES:
		return ("DL_CONNECT_RES");
	case DL_CONNECT_CON:
		return ("DL_CONNECT_CON");
	case DL_TOKEN_REQ:
		return ("DL_TOKEN_REQ");
	case DL_TOKEN_ACK:
		return ("DL_TOKEN_ACK");
	case DL_DISCONNECT_REQ:
		return ("DL_DISCONNECT_REQ");
	case DL_DISCONNECT_IND:
		return ("DL_DISCONNECT_IND");
	case DL_RESET_REQ:
		return ("DL_RESET_REQ");
	case DL_RESET_IND:
		return ("DL_RESET_IND");
	case DL_RESET_RES:
		return ("DL_RESET_RES");
	case DL_RESET_CON:
		return ("DL_RESET_CON");
	case DL_DATA_ACK_REQ:
		return ("DL_DATA_ACK_REQ");
	case DL_DATA_ACK_IND:
		return ("DL_DATA_ACK_IND");
	case DL_DATA_ACK_STATUS_IND:
		return ("DL_DATA_ACK_STATUS_IND");
	case DL_REPLY_REQ:
		return ("DL_REPLY_REQ");
	case DL_REPLY_IND:
		return ("DL_REPLY_IND");
	case DL_REPLY_STATUS_IND:
		return ("DL_REPLY_STATUS_IND");
	case DL_REPLY_UPDATE_REQ:
		return ("DL_REPLY_UPDATE_REQ");
	case DL_REPLY_UPDATE_STATUS_IND:
		return ("DL_REPLY_UPDATE_STATUS_IND");
	case DL_XID_REQ:
		return ("DL_XID_REQ");
	case DL_XID_IND:
		return ("DL_XID_IND");
	case DL_XID_RES:
		return ("DL_XID_RES");
	case DL_XID_CON:
		return ("DL_XID_CON");
	case DL_TEST_REQ:
		return ("DL_TEST_REQ");
	case DL_TEST_IND:
		return ("DL_TEST_IND");
	case DL_TEST_RES:
		return ("DL_TEST_RES");
	case DL_TEST_CON:
		return ("DL_TEST_CON");
	case DL_PHYS_ADDR_REQ:
		return ("DL_PHYS_ADDR_REQ");
	case DL_PHYS_ADDR_ACK:
		return ("DL_PHYS_ADDR_ACK");
	case DL_SET_PHYS_ADDR_REQ:
		return ("DL_SET_PHYS_ADDR_REQ");
	case DL_GET_STATISTICS_REQ:
		return ("DL_GET_STATISTICS_REQ");
	case DL_GET_STATISTICS_ACK:
		return ("DL_GET_STATISTICS_ACK");
	case DL_NOTIFY_REQ:
		return ("DL_NOTIFY_REQ");
	case DL_NOTIFY_ACK:
		return ("DL_NOTIFY_ACK");
	case DL_NOTIFY_IND:
		return ("DL_NOTIFY_IND");
	case DL_AGGR_REQ:
		return ("DL_AGGR_REQ");
	case DL_AGGR_IND:
		return ("DL_AGGR_IND");
	case DL_UNAGGR_REQ:
		return ("DL_UNAGGR_REQ");
	case DL_CAPABILITY_REQ:
		return ("DL_CAPABILITY_REQ");
	case DL_CAPABILITY_ACK:
		return ("DL_CAPABILITY_ACK");
	case DL_CONTROL_REQ:
		return ("DL_CONTROL_REQ");
	case DL_CONTROL_ACK:
		return ("DL_CONTROL_ACK");
	case DL_PASSIVE_REQ:
		return ("DL_PASSIVE_REQ");
	case DL_INTR_MODE_REQ:
		return ("DL_INTR_MODE_REQ");
	case DL_HP_PPA_REQ:
		return ("DL_HP_PPA_REQ");
	case DL_HP_PPA_ACK:
		return ("DL_HP_PPA_ACK");
	case DL_HP_MULTICAST_LIST_REQ:
		return ("DL_HP_MULTICAST_LIST_REQ");
	case DL_HP_MULTICAST_LIST_ACK:
		return ("DL_HP_MULTICAST_LIST_ACK");
	case DL_HP_RAWDATA_REQ:
		return ("DL_HP_RAWDATA_REQ");
	case DL_HP_RAWDATA_IND:
		return ("DL_HP_RAWDATA_IND");
	case DL_HP_HW_RESET_REQ:
		return ("DL_HP_HW_RESET_REQ");
	case DL_HP_INFO_REQ:
		return ("DL_HP_INFO_REQ");
	case DL_HP_INFO_ACK:
		return ("DL_HP_INFO_ACK");
	case DL_HP_SET_ACK_TO_REQ:
		return ("DL_HP_SET_ACK_TO_REQ");
	case DL_HP_SET_P_TO_REQ:
		return ("DL_HP_SET_P_TO_REQ");
	case DL_HP_SET_REJ_TO_REQ:
		return ("DL_HP_SET_REJ_TO_REQ");
	case DL_HP_SET_BUSY_TO_REQ:
		return ("DL_HP_SET_BUSY_TO_REQ");
	case DL_HP_SET_SEND_ACK_TO_REQ:
		return ("DL_HP_SET_SEND_ACK_TO_REQ");
	case DL_HP_SET_MAX_RETRIES_REQ:
		return ("DL_HP_SET_MAX_RETRIES_REQ");
	case DL_HP_SET_ACK_THRESHOLD_REQ:
		return ("DL_HP_SET_ACK_THRESHOLD_REQ");
	case DL_HP_SET_LOCAL_WIN_REQ:
		return ("DL_HP_SET_LOCAL_WIN_REQ");
	case DL_HP_SET_REMOTE_WIN_REQ:
		return ("DL_HP_SET_REMOTE_WIN_REQ");
	case DL_HP_CLEAR_STATS_REQ:
		return ("DL_HP_CLEAR_STATS_REQ");
	case DL_HP_SET_LOCAL_BUSY_REQ:
		return ("DL_HP_SET_LOCAL_BUSY_REQ");
	case DL_HP_CLEAR_LOCAL_BUSY_REQ:
		return ("DL_HP_CLEAR_LOCAL_BUSY_REQ");
	default:
		return ("(unknown)");
	}
}

static inline const char *
dl_statename(dl_ulong state)
{
	switch (state) {
	case DL_UNATTACHED:
		return ("DL_UNATTACHED");
	case DL_ATTACH_PENDING:
		return ("DL_ATTACH_PENDING");
	case DL_DETACH_PENDING:
		return ("DL_DETACH_PENDING");
	case DL_UNBOUND:
		return ("DL_UNBOUND");
	case DL_BIND_PENDING:
		return ("DL_BIND_PENDING");
	case DL_UNBIND_PENDING:
		return ("DL_UNBIND_PENDING");
	case DL_IDLE:
		return ("DL_IDLE");
	case DL_UDQOS_PENDING:
		return ("DL_UDQOS_PENDING");
	case DL_OUTCON_PENDING:
		return ("DL_OUTCON_PENDING");
	case DL_INCON_PENDING:
		return ("DL_INCON_PENDING");
	case DL_CONN_RES_PENDING:
		return ("DL_CONN_RES_PENDING");
	case DL_DATAXFER:
		return ("DL_DATAXFER");
	case DL_USER_RESET_PENDING:
		return ("DL_USER_RESET_PENDING");
	case DL_PROV_RESET_PENDING:
		return ("DL_PROV_RESET_PENDING");
	case DL_RESET_RES_PENDING:
		return ("DL_RESET_RES_PENDING");
	case DL_DISCON8_PENDING:
		return ("DL_DISCON8_PENDING");
	case DL_DISCON9_PENDING:
		return ("DL_DISCON9_PENDING");
	case DL_DISCON11_PENDING:
		return ("DL_DISCON11_PENDING");
	case DL_DISCON12_PENDING:
		return ("DL_DISCON12_PENDING");
	case DL_DISCON13_PENDING:
		return ("DL_DISCON13_PENDING");
	case DL_SUBS_BIND_PND:
		return ("DL_SUBS_BIND_PND");
	case DL_SUBS_UNBIND_PND:
		return ("DL_SUBS_UNBIND_PND");
	default:
		return ("(unknown)");
	}
}

static inline int const char *
dl_errname(dl_long error)
{
	switch (error) {
	case DL_ACCESS:
		return ("DL_ACCESS");
	case DL_BADADDR:
		return ("DL_BADADDR");
	case DL_BADCORR:
		return ("DL_BADCORR");
	case DL_BADDATA:
		return ("DL_BADDATA");
	case DL_BADPPA:
		return ("DL_BADPPA");
	case DL_BADPRIM:
		return ("DL_BADPRIM");
	case DL_BADQOSPARAM:
		return ("DL_BADQOSPARAM");
	case DL_BADQOSTYPE:
		return ("DL_BADQOSTYPE");
	case DL_BADSAP:
		return ("DL_BADSAP");
	case DL_BADTOKEN:
		return ("DL_BADTOKEN");
	case DL_BOUND:
		return ("DL_BOUND");
	case DL_INITFAILED:
		return ("DL_INITFAILED");
	case DL_NOADDR:
		return ("DL_NOADDR");
	case DL_NOTINIT:
		return ("DL_NOTINIT");
	case DL_OUTSTATE:
		return ("DL_OUTSTATE");
	case DL_SYSERR:
		return ("DL_SYSERR");
	case DL_UNSUPPORTED:
		return ("DL_UNSUPPORTED");
	case DL_UNDELIVERABLE:
		return ("DL_UNDELIVERABLE");
	case DL_NOTSUPPORTED:
		return ("DL_NOTSUPPORTED");
	case DL_TOOMANY:
		return ("DL_TOOMANY");
	case DL_NOTENAB:
		return ("DL_NOTENAB");
	case DL_BUSY:
		return ("DL_BUSY");
	case DL_NOAUTO:
		return ("DL_NOAUTO");
	case DL_NOXIDAUTO:
		return ("DL_NOXIDAUTO");
	case DL_NOTESTAUTO:
		return ("DL_NOTESTAUTO");
	case DL_XIDAUTO:
		return ("DL_XIDAUTO");
	case DL_TESTAUTO:
		return ("DL_TESTAUTO");
	case DL_PENDING:
		return ("DL_PENDING");
	default:
		if (error < 0)
			return ("DL_SYSERR");
		return ("(unknown)");
	}
}

static inline const char *
dl_strerror(dl_long error)
{
	switch (error) {
	case DL_ACCESS:
		return ("Improper permissions for request.");
	case DL_BADADDR:
		return ("DLSAP addr in improper format or invalid.");
	case DL_BADCORR:
		return ("Seq number not from outstand DL_CONN_IND.");
	case DL_BADDATA:
		return ("User data exceeded provider limit.");
	case DL_BADPPA:
		return ("Specified PPA was invalid.");
	case DL_BADPRIM:
		return ("Primitive received not known by provider.");
	case DL_BADQOSPARAM:
		return ("QOS parameters contained invalid values.");
	case DL_BADQOSTYPE:
		return ("QOS structure type is unknown/unsupported.");
	case DL_BADSAP:
		return ("Bad LSAP selector.");
	case DL_BADTOKEN:
		return ("Token used not an active stream.");
	case DL_BOUND:
		return ("Attempted second bind with dl_max_conind.");
	case DL_INITFAILED:
		return ("Physical Link initialization failed.");
	case DL_NOADDR:
		return ("Provider couldn't allocate alt. address.");
	case DL_NOTINIT:
		return ("Physical Link not initialized.");
	case DL_OUTSTATE:
		return ("Primitive issued in improper state.");
	case DL_SYSERR:
		return ("UNIX system error occurred.");
	case DL_UNSUPPORTED:
		return ("Requested serv. not supplied by provider.");
	case DL_UNDELIVERABLE:
		return ("Previous data unit could not be delivered.");
	case DL_NOTSUPPORTED:
		return ("Primitive is known but not supported.");
	case DL_TOOMANY:
		return ("Limit exceeded.");
	case DL_NOTENAB:
		return ("Promiscuous mode not enabled.");
	case DL_BUSY:
		return ("Other streams for PPA in post-attached state.");
	case DL_NOAUTO:
		return ("Automatic handling of XID and TEST response not supported. .");
	case DL_NOXIDAUTO:
		return ("Automatic handling of XID not supported.");
	case DL_NOTESTAUTO:
		return ("Automatic handling of TEST not supported.");
	case DL_XIDAUTO:
		return ("Automatic handling of XID response.");
	case DL_TESTAUTO:
		return ("Automatic handling of TEST response.");
	case DL_PENDING:
		return ("pending outstanding connect indications.");
	case -EPERM:
		return ("Permission for the control request is incorrect.");
	case -ENXIO:
		return ("Invalid device.");
	case -ENOMEM:
		return ("Insufficient memory.");
	case -EBUSY:
		return ("Already enabled or driver unable to handle request.");
	case -EINVAL:
		return ("One of the parameters has an invalid value.");
	case -ENOBUFS:
		return ("Inadequate buffers were available.");
	case -ENOTSUP:
		return ("Unsupported request, event or call.");
	case -ENOLINK:
		return ("The interface is not connected.");
	case -EFAULT:
		return ("Segmentation fault.")
	default:
		if (error < 0)
			return ("Linux system error occured.");
		return ("(unknown)");
	}
}

static dl_ulong
dl_get_state(struct dl *dl)
{
	return (dl->state);
}

static dl_ulong
dl_set_state(struct dl *dl, dl_ulong newstate)
{
	dl_ulong oldstate = dl->state;

	if (newstate != oldstate) {
		dl->state = newstate;
		dl->proto.info.dl_current_state = newstate;
		mi_strlog(dl->oq, STRLOGST, SL_TRACE < "%s <- %s", dl_statename(newstate),
			  dl_statename(oldstate));
	}
	return (newstate);
}

static dl_ulong
dl_save_state(struct dl *dl)
{
	return ((dl->oldstate = dl_get_state(dl)));
}

static dl_ulong
dl_restore_state(struct dl *dl)
{
	return (dl_set_state(dl, dl->oldstate));
}

/*
 * --------------------------------------------------------------------------
 *
 * HDLC SENT MESSAGES
 *
 * --------------------------------------------------------------------------
 */

/*
 * I-Frames:
 * I		Information.
 *
 * S-Frames:
 * RR		Receive ready.
 * RNR		Receive not ready.
 * REJ		Reject.
 * SREJ		Selective reject.
 *
 * U-Frames:
 * SNRM		Set normal response mode.
 * SNRME	Set normal response mode extended.
 * SARM		Set asyncrhonous response mode.
 * SARME	Set asyncrhonous response mode extended.
 * SABM		Set asynchronous balanced mode.
 * SABME	Set asynchronous balanced mode extended.
 * UP		Unnumbered poll.
 * UI		Unnumbered information.
 * UA		Unnumbered acknowledgement.
 * RD		Request disconnect.
 * DISC		Disconnect.
 * ADM		Asynchronous disconnect mode.
 * NDM		Normal disconnect mode.
 * RIM		Request information mode.
 * SIM		Set initialization mode.
 * RSET		Reset.
 * XID		Exchange ID.
 * FRMR		Frame reject.
 * SM		Set Mode.
 * TEST		Test.
 */

//              /* I-Frames */  /* RRRP SSS0 */
#define DL_MT_I		0x00	/* 0000 0000 */	/* cmd rsp */

//              /* S-Frames */  /* RRRP XX01 */
#define DL_MT_RR	0x01	/* 0000 0001 */	/* cmd rsp */
#define DL_MT_REJ	0x09	/* 0000 1001 */	/* cmd rsp */
#define DL_MT_RNR	0x05	/* 0000 0101 */	/* cmd rsp */
#define DL_MT_SREJ	0x0d	/* 0000 1101 */	/* cmd rsp */

//              /* U-Frames */  /* XXXP XX11 */ /* cmd rsp */
#define DL_MT_UI	0x03	/* 0000 0011 */	/* cmd rsp */
#define DL_MT_SRNM	0x83	/* 1000 0011 */	/* cmd - */
#define DL_MT_DISC	0x43	/* 0100 0011 */	/* cmd - */
#define DL_MT_RD	0x43	/* 0100 0011 */	/* - rsp */
#define DL_MT_SM	0xc3	/* 1100 0011 */	/* cmd - */
#define DL_MT_UP	0x23	/* 0010 0011 */	/* cmd - */
#define DL_MT_UA	0x63	/* 0110 0011 */	/* - rsp */
#define DL_MT_TEST	0xe3	/* 1110 0011 */	/* cmd rsp */

#define DL_MT_SIM	0x07	/* 0000 0111 */	/* cmd - */
#define DL_MT_RIM	0x07	/* 0000 0111 */	/* - rsp */
#define DL_MT_FRMR	0x87	/* 1000 0111 */	/* - rsp */

#define DL_MT_SARM	0x0f	/* 0000 1111 */	/* cmd - */
#define DL_MT_ADM	0x0f	/* 0000 1111 */	/* - rsp */
#define DL_MT_RSET	0x8f	/* 1000 1111 */	/* cmd - */
#define DL_MT_SARME	0x4f	/* 0100 1111 */	/* cmd - */
#define DL_MT_SRNME	0xcf	/* 1100 1111 */	/* cmd - */
#define DL_MT_SABM	0x2f	/* 0010 1111 */	/* cmd - */
#define DL_MT_NDM	0x2f	/* 0010 1111 */	/* - rsp */
#define DL_MT_XID	0xaf	/* 1010 1111 */	/* cmd rsp */
#define DL_MT_SABME	0x6f	/* 0110 1111 */	/* cmd - */

/**
 * trimb: - trim and free leading message blocks
 * @mp: chain message to free from
 * @bp: block beyond which remains in the message
 *
 * Trim and free mp and any trailing message blocks that do not begin with bp.
 * This has the affect of trimming and freeing all of the leading message
 * blocks in mp that are ahead of dp.  When dp is NULL, trim and free all the
 * blocks in mp.
 */
static inline void
trimb(mblk_t *mp, mblk_t *bp)
{
	register mblk_t *b, *b_cont = mp;

	while ((b = b_cont) != NULL && b != bp) {
		b_cont = unlinkb(b);
		freeb(b);
	}
}

/*
 * --------------------------------------------------------------------------
 *
 * DLPI PROVIDER OPERATIONS
 *
 * --------------------------------------------------------------------------
 */
static fastcall noinline __unlikely int
dl_sap_check_lan(struct dl *dl, dl_ulong sap)
{
	/* 
	 * Non-ethernet:
	 * 1.  SAP >= 1536 Ethertype except 0xAAAA implies that a SNAP header is required.
	 * 2.  SAP == 0xAAAA need subsequent bind for SNAP OUI
	 * 3.  SAP == 0xAA need subsequent bind for SNAP OUI
	 * 4.  SAP <= 255 exclude odd SAPs, 0x00, 0xfc, 0xff
	 * 5.  SAP other value is an error.
	 */
	if (sap > 0x0000ffff)
		goto badsap;
	if (sap != 0x0000AAAA && sap != 0x000000AA) {
		if (sap <= 1536 && sap > 255)
			goto badsap;
		if (sap <= 255) {
			if (sap & 0x00000001)
				goto badsap;
			if (sap == 0x00000000)
				goto badsap;
			if (sap == 0x000000fc)
				goto badsap;
			if (sap == 0x000000ff)
				goto badsap;
		}
	}
	return (0);
      badsap:
	return (DL_BADSAP);
}
static fastcall noinline __unlikely int
dl_sap_check_llc(struct dl *dl, dl_ulong sap)
{
	/* 
	 * 802.2 only
	 * 1.  SAP <= 255 exclude odd SAPs, 0x00, 0xfc, 0xff
	 * 2.  SAP other value is an error.
	 */
	if (sap > 255)
		goto badsap;
	if (sap & 0x01)
		goto badsap;
	if (sap == 0x00)
		goto badsap;
	if (sap == 0xfc)
		goto badsap;
	if (sap == 0xff)
		goto badsap;
	return (0);
      badsap:
	return (DL_BADSAP);
}
static fastcall noinline __unlikely int
dl_sap_check_eII(struct dl *dl, dl_ulong sap)
{
	/* 
	 * Ethernet:
	 * 1.  SAP >= 1536 Ethertype (even 0xAAAA)
	 * 2.  SAP other value is an error
	 */
	if (sap > 0x0000ffff)
		goto badsap;
	if (sap <= 1536)
		goto badsap;
	return (0);
      badsap:
	return (DL_BADSAP);
}
static fastcall noinline __unlikely int
dl_sap_check_eth(struct dl *dl, dl_ulong sap)
{
	/* 
	 * Ethernet/802.2:
	 * 1.  SAP >= 1536 Ethertype (even 0xAAAA)
	 * 2.  SAP <= 255 exclude odd SAPs, 0x00, 0xfc, 0xff
	 * 3.  SAP other value is an error.
	 */
	if (sap > 0x0000ffff)
		goto badsap;
	if (sap <= 1536 && sap > 255)
		goto badsap;
	if (sap <= 255) {
		if (sap & 0x00000001)
			goto badsap;
		if (sap == 0x00000000)
			goto badsap;
		if (sap == 0x000000fc)
			goto badsap;
		if (sap == 0x000000ff)
			goto badsap;
	}
	return (0);
      badsap:
	return (DL_BADSAP);
}
static fastcall noinline __unlikely int
dl_sap_check_hdlc(struct dl *dl, dl_ulong sap)
{
	/* 
	 * ISO HDLC (includes SDLC):
	 * high order byte:
	 * (SAP>>24) == 0, slave
	 * (SAP>>24) == 1, master
	 * (SAP>>24) == 2, extended slave
	 * (SAP>>24) == 3, extended master
	 * low order 3 bytes:
	 * (SAP&0xffffff) <= 0x0000ff, non-extended address
	 * (SAP&0xffffff) <= 0x00007f, extended 1 octet address
	 * (SAP&0xffffff) <= 0x003fff, extended 2 octet address
	 * (SAP&0xffffff) <= 0x1fffff, extended 3 octet address
	 * (SAP&0xffffff) <= 0xffffff, extended 4 octet address
	 */
	if ((sap >> 24) > 3)
		goto badsap;
	if (((sap >> 24) & 0x20) == 0)
		if ((sap & 0xffffff) > 0xff)
			goto badsap;
	return (0);
      badsap:
	return (DL_BADSAP);
}

static fastcall noinline __unlikely int
dl_sap_check_lapb(struct dl *dl, dl_ulong sap)
{
	/* 
	 * X25/X75/ISO 7776 LAPB:
	 * high order byte
	 * (SAP>>24) == 0, DTE
	 * (SAP>>24) == 1, DCE
	 * low order 3 bytes:
	 * (SAP&0xffffff) == 0x00000000 (SLP DTE/DXE/STE)
	 * (SAP&0xffffff) == 0x00000001 (SLP DCE/DXE/STE)
	 * (SAP&0xffffff) == 0x00000011 (MLP DTE/DXE/STE)
	 * (SAP&0xffffff) == 0x00000111 (MLP DCE/DXE/STE)
	 * (SAP&0xffffff) == 0x00100000 (SLP DTE/DXE/STE) (D-channel)
	 * (SAP&0xffffff) == 0x00100001 (SLP DCE/DXE/STE) (D-channel)
	 */
	if (((sap >> 24) & 0x0ff) > 1)
		goto badsap;
	switch (sap & 0xffffff) {
	case 0:		/* SLP DTE/DXE/STE */
	case 1:		/* SLP DCE/DXE/STE */
	case 3:		/* MLP DTE/DXE/STE */
	case 7:		/* MLP DCE/DXE/STE */
	case 32:		/* SLP DTE/DXE/STE D-channel */
	case 33:		/* SLP DTE/DXE/STE D-channel */
		break;
	default:
		goto badsap;
	}
	return (0);
      badsap:
	return (DL_BADSAP);
}

static fastcall noinline __unlikely int
dl_sap_check_lapd(struct dl *dl, dl_ulong sap)
{
	/* 
	 * Q.921 LAPD:               |TTTTTTTE|SSSSSSCE|
	 * high order byte:
	 * (SAP>>24) == 0, user (extended implied)
	 * (SAP>>24) == 1, ntwk (extended implied)
	 * (SAP>>24) == 2, user (extended)
	 * (SAP>>24) == 3, ntwk (extended)
	 * (SAP>>24) = 0x00, User LAPD first octet 000000X0
	 * (SAP>>24) = 0x02, Ntwk LAPD first octet 000000X0
	 * (SAP>>24) = 0xfc, User MGMT first octet 111111X0
	 * (SAP>>24) = 0xfe, Ntwk MGMT frist octet 111111X0
	 * low order 3 bytes:
	 * (SAP&0x00007f) == 0x00 (user LAPD) 000000C0
	 * (SAP&0x00007f) == 0x01 (ntwk LAPD) 000000C0
	 * (SAP&0x00007f) == 0x7e (user MGMT) 111111C0
	 * (SAP&0x00007f) == 0x7f (ntwk MGMT) 111111C0
	 * ((SAP>>7)&0x01ffff) <= 0x7f TEI (valid)
	 * ((SAP>>7)&0x01ffff) == 0x7f TEI (broadcast)
	 */
	if ((p->dl_sap >> 24) > 3)
		goto badsap;
	return (0);
      badsap:
	return (DL_BADSAP);
}

static fastcall noinline __unlikely int
dl_sap_check_lapf(struct dl *dl, dl_ulong sap)
{
	/* 
	 * Q.922 LAPF:               |DDDDFBDE|DDDDDDCE|
	 * high order byte:
	 * (SAP>>24) == 2, extended
	 * lower order 3 bytes (just DLCI part)
	 * (SAP&0xffffff) <= 0x0003ff, DLCI, 2 or 3 (DL-CORE) octet address
	 * (SAP&0xffffff) <= 0x00ffff, DLCI, 3 or 4 (DL-CORE) octet address
	 * (SAP&0xffffff) <= 0x01ffff, DLCI, 4 or 4 (DL-CORE) octet address
	 * (SAP&0xffffff) <= 0x7fffff, DLCI, 4 octet address
	 */
	switch ((sap >> 24)) {
	case 0:		/* extended implied */
	case 2:		/* extended */
	default:
		goto badsap;
	}
	if ((sap & 0xffffff) > 0x7fffff)
		goto badsap;
	return (0);
      badsap:
	return (DL_BADSAP);
}

static fastcall noinline __unlikely int
dl_sap_check_loop(struct dl *dl, dl_ulong sap)
{
	return (0);
      badsap:
	return (DL_BADSAP);
}

static fastcall noinline __unlikely int
dl_sap_check(struct dl *dl, dl_ulong sap)
{
	switch (dl->proto.info.dl_mac_type) {
	case DL_TPR:
	case DL_100VGTPR:
		/* Note: 8-byte MAC */
	case DL_TPB:
	case DL_METRO:
	case DL_FDDI:
	case DL_ATM:
		return dl_sap_check_lan(dl, sap);
	case DL_CSMACD:
		return dl_sap_check_llc(dl, sap);
	case DL_ETHER:
	case DL_100VG:
	case DL_100BT:
		return dl_sap_check_eII(dl, sap);
	case DL_ETH_CSMA:
		return dl_sap_check_eth(dl, sap);
	case DL_HDLC:
	case DL_ASYNC:
		return dl_sap_check_hdlc(dl, sap);
		break;
	case DL_X25:
		return dl_sap_check_lapb(dl, sap);
	case DL_ISDN:
		return dl_sap_check_lapd(dl, sap);
	case DL_FRAME:
		return dl_sap_check_lapf(dl, sap);
	case DL_LOOP:
		return dl_sap_check_loop(dl, sap);
	default:
	case DL_CTCA:		/* not used */
	case DL_IPATM:		/* not used *//* XXX: why not? */
	case DL_IPX25:		/* not used *//* XXX: why not? */
	case DL_MPFRAME:	/* not used *//* XXX: why not? */
	case DL_FC:		/* not used */
	case DL_HIPPI:		/* not used */
	case DL_IB:		/* not used */
	case DL_CHAR:		/* not used */
	case DL_OTHER:		/* not used */
#ifdef _SUN_SOURCE
	case DL_IPV4:		/* not used */
	case DL_IPV6:		/* not used */
	case DL_VNI:
	case DL_WIFI:		/* not used */
#endif				/* _SUN_SOURCE */
		return (DL_BADSAP);
	}
}

/**
 * dl_bind: - bind a SAP to a data link interface
 * @dl: data link private structure
 * @q: active queue
 * @sap: SAP to bind
 *
 * This function takes an pre-allocated orphan SAP structure, adds a Linux
 * packet handler, and links the SAP structure into the list of bound SAPs
 * associated with the data link provider structure.  This function can return
 * an error if registering the packet handler with the Linux kernel fails,
 * otherwise the function always succeeds.
 */
static fastcall noinline __unlikely int
dl_bind(struct dl *dl, queue_t *q, struct sap *sap)
{
	int err;
	pl_t pl;

	dev_add_pack(&sap->pt);

	pl = RW_WRLOCK(&dl_mux_lock, plstr);
	list_add(&sap->sap_list, &dl_sap_list);
	RW_UNLOCK(&dl_mux_lock, pl);

	list_add(&sap->dl_list, &dl->sap_list);
	sap->dl = dl;
	return (0);
}

static fastcall noinline __unlikely int
dl_subs_bind(struct dl *dl, queue_t *q, caddr_t sap_ptr, size_t sap_len, dl_ulong sap_cls)
{
}

static fastcall noinline __unlikely int
dl_subs_unbind(struct dl *dl)
{
	struct sap *sap = dl->sap.curr;
	pl_t pl;

	dev_remove_pack(&sap->pt);

	pl = RW_WRLOCK(&dl_mux_lock, plstr);
	list_del(&sap->sap_list);
	RW_UNLOCK(&dl_mux_lock, pl);

	list_del(&sap->dl_list);
	sap->dl = NULL;
	return (0);
}
static fastcall noinline __unlikely int
dl_unbind(struct dl *dl)
{
	pl_t pl;

	dev_remove_pack(&sap->pt);

	pl = RW_WRLOCK(&dl_mux_lock, plstr);
	list_del(&sap->sap_list);
	RW_UNLOCK(&dl_mux_lock, pl);

	list_del(&sap->dl_list);
	sap->dl = NULL;
	return (0);
}
static fastcall noinline int
dl_setbusy_llc2(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong prim)
{
	int err;

	switch (prim) {
	case DL_HP_SET_LOCAL_BUSY_REQ:
		if (dl->busy.loc == 0) {
			/* FIXME: send RNR. */
			dl->busy.loc = 1;
		}
		err = 0;
		break;
	case DL_HP_CLEAR_LOCAL_BUSY_REQ:
		if (dl->busy.loc != 0) {
			/* FIXME: send RR. */
			dl->busy.loc = 0;
		}
		err = 0;
		break;
	default:
		err = -EFAULT;
		break;
	}
	freemsg(msg);
	return (0);
}

/**
 * dl_snddata: - send canonical or raw data
 * @dl: data link private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @prim: primitive (either DL_DATA_REQ or DL_HP_RAWDATA_REQ)
 * @dp: the data
 */
static fastcall inline __hot int
dl_snddata(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong prim, mblk_t *dp)
{
	int err;

	switch (prim) {
	case 0: /* DL_DATA_REQ */
		trimb(msg, dp);
		freemsg(dp);		/* FIXME: really send data */
		err = 0;
		break;
	case DL_HP_RAWDATA_REQ:
		trimb(msg, dp);
		freemsg(dp);		/* FIXME: really send data */
		err = 0;
		break;
	default:
		freemsg(msg);
		err = -EFAULT;
		break;
	}
	return (err);
}

/**
 * dl_attach: - attach a stream to a PPA
 * @dl: data link private structure
 * @q: active queue (might require allocation)
 * @ppa: the PPA to which to attach
 *
 * This is generic for all upper streams.
 */
static fastcall noinline __unlikely int
dl_attach(struct dl *dl, queue_t *q, dl_ulong ppa)
{
	/* FIXME: really attach */
	return (0);
}

/**
 * dl_detach_xxx: - detach a stream from a PPA
 * @dl: data link private structure
 *
 * This is PPA-specific.
 */
static fastcall noinline __unlikely int
dl_detach(struct dl *dl)
{
	/* FIXME: really detach */
	return (0);
}

/*
 * --------------------------------------------------------------------------
 *
 * DLPI PROVIDER TO DLPI USER ISSUED PRIMITIVES
 *
 * --------------------------------------------------------------------------
 * All these functions take a pointer to the data link private structure, a
 * pointer to the active queue (whichever queue that is), and a pointer to a
 * message block to free upon success (or NULL).  If data is also included, the
 * data will be detached from the msg before it is freed.
 */

static fastcall noinline __unlikely int
m_error(struct dl *dl, queue_t *q, mblk_t *msg, uchar rerr, uchar werr)
{
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, 2, BPRI_MED)))) {
		DB_TYPE(mp) = M_ERROR;
		*mp->b_wptr++ = rerr;
		*mp->b_wptr++ = werr;
		freemsg(msg);
		mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- M_ERROR");
		putnext(dl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
m_error_reply(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong prim, dl_long error)
{
	switch (error) {
	case 0:
	case -EAGAIN:
	case -EBUSY:
	case -ENOMEM:
	case -ENOBUFS:
	case -ENOSR:
	case -EDEADLK:
		return (error);
	}
	mi_strlog(dl->oq, 0, SL_TRACE, "%s: %s: %s", dl_primname(prim), dl_errname(error),
		  dl_strerror(error));
	error = (error > 0) ? EPROTO : -error;
	return m_error(dl, q, msg, error, error);
}
static fastcall noinline __unlikely int
m_hangup(struct dl *dl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, 0, BPRI_MED)))) {
		DB_TYPE(mp) = M_HANGUP;
		freemsg(msg);
		mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- M_HANGUP");
		putnext(dl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
m_hangup_reply(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong prim, dl_long error)
{
	switch (error) {
	case 0:
	case -EAGAIN:
	case -EBUSY:
	case -ENOMEM:
	case -ENOBUFS:
	case -EDEADLK:
	case -ENOSR:
		return (error);
	}
	mi_strlog(dl->oq, 0, SL_TRACE, "%s: %s: %s", dl_primname(prim), dl_errname(error),
		  dl_strerror(error));
	return m_hangup(dl, q, msg);
}
static fastcall inline __hot int
dl_data_ind(struct dl *dl, queue_t *q, mblk_t *msg, mblk_t *mp)
{
	if (likely(bcanputnext(dl->oq, mp->b_band))) {
		trimb(msg, mp);
		mi_strlog(dl->oq, STRLOGDA, SL_TRACE, "<- DL_DATA_IND");
		putnext(dl->oq, mp);
		return (0);
	}
	return (-EBUSY);
}
static fastcall noinline __unlikely int
dl_info_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_info_ack_t *p;
	mblk_t *mp;
	caddr_t qos_ptr, qor_ptr, add_ptr, brd_ptr;
	size_t qos_len, qor_len, add_len, brd_len, msg_len;

	if (dl->proto.info.dl_service_mode & DL_CODLS) {
		qos_len = sizeof(dl->proto.co.qos);
		qos_ptr = (caddr_t) (&dl->proto.co.qos);
		qor_len = sizeof(dl->proto.co.qor);
		qor_ptr = (caddr_t) (&dl->proto.co.qor);
	} else if (dl->proto.info.dl_service_mode & (DL_CLDLS | DL_ACLDLS)) {
		qos_len = sizeof(dl->proto.cl.qos);
		qos_ptr = (caddr_t) (&dl->proto.cl.qos);
		qor_len = sizeof(dl->proto.cl.qor);
		qor_ptr = (caddr_t) (&dl->proto.cl.qor);
	} else {
		qos_len = 0;
		qos_ptr = NULL;
		qor_len = 0;
		qor_ptr = NULL;
	}
	add_len = dl->proto.info.dl_addr_length;
	brd_len = dl->proto.info.dl_brdcst_addr_length;
	add_ptr = add_len ? (caddr_t) &dl->proto.add : NULL;
	brd_ptr = brd_len ? (caddr_t) &dl->proto.brd : NULL;
	msg_len = qos_len + qor_len + add_len + brd_len;

	if (likely(!!(mp = mi_allocb(q, msg_len, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_INFO_ACK;
		p->dl_max_sdu = dl->proto.info.dl_max_sdu;
		p->dl_min_sdu = dl->proto.info.dl_min_sdu;
		p->dl_addr_length = add_len;
		p->dl_mac_type = dl->proto.info.dl_mac_type;
		p->dl_reserved = dl->proto.info.dl_reserved;
		p->dl_current_state = dl->proto.info.dl_current_state;
		p->dl_sap_length = dl->proto.info.dl_sap_length;
		p->dl_service_mode = dl->proto.info.dl_service_mode;
		p->dl_qos_length = qos_len;
		p->dl_qos_offset = qos_len ? sizeof(*p) : 0;
		p->dl_qos_range_length = qor_len;
		p->dl_qos_range_offset = qor_len ? sizeof(*p) + qos_len : 0;
		p->dl_provider_style = dl->proto.info.dl_provider_style;
		p->dl_addr_offset = add_len ? sizeof(*p) + qos_len + qor_len : 0;
		p->dl_version = dl->proto.info.dl_version;
		p->dl_brdcst_addr_length = brd_len;
		p->dl_brdcst_addr_offset = brd_len ? sizeof(*p) + qos_len + qor_len + add_len : 0;
		p->dl_growth = 0;
		mp->b_wptr += sizeof(*p);
		bcopy(qos_ptr, mp->b_wptr, qos_len);
		mp->b_wptr += qos_len;
		bcopy(qor_ptr, mp->b_wptr, qor_len);
		mp->b_wptr += qor_len;
		bcopy(add_ptr, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
		bcopy(brd_ptr, mp->b_wptr, brd_len);
		mp->b_wptr += brd_len;
		freemsg(msg);
		mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_INFO_ACK");
		putnext(dl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
dl_bind_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_bind_ack_t *p;
	mblk_t *mp = NULL;
	size_t add_len = dl->proto.info.dl_addr_length;
	caddr_t add_ptr = (caddr_t) &dl->proto.add;
	int err;

	/* maximum address length is 20 bytes */
	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + add_len, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_BIND_ACK;
		p->dl_sap = dl->sap->lsap;
		p->dl_addr_length = add_len;
		p->dl_addr_offset = add_len ? sizeof(*p) : 0;
		p->dl_max_conind = dl->conmax;
		p->dl_xidtest_flg = dl->xidtest & (DL_AUTO_XID | DL_AUTO_TEST);
		mp->b_wptr += sizeof(*p);
		bcopy(add_ptr, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
		if (unlikely((err = dl->ops->dl_bind(dl, q, dl->sap))))
			goto error;
		freemsg(msg);
		dl_set_state(dl, DL_IDLE);
		mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_BIND_ACK");
		putnext(dl->oq, mp);
		return (0);
	      error:
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
dl_ok_ack(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong prim, union DL_primitives *p)
{
	dl_ok_ack_t *p;
	mblk_t *mp;
	int err;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_OK_ACK;
		p->dl_correct_primitive = prim;
		mp->b_wptr += sizeof(*p);
		switch (dl_get_state(dl)) {
		case DL_ATTACH_PENDING:
			if ((err = dl_attach(dl, q, p->attach_req.dl_ppa)))
				goto error;
			dl_set_state(dl, DL_UNBOUND);
			break;
		case DL_DETACH_PENDING:
			if ((err = dl_op_detach(dl)))
				goto error;
			dl_set_state(dl, DL_UNATTACHED);
			break;
		case DL_UNBIND_PENDING:
			/* FIXME: if we are an ether type, delete the packet handler for the
			   current SAP (dl->sap) remote and delete the SAP structure to complete
			   the unbind operation.  If there are additional bound SAPs that were
			   bound with DL_SUBS_BIND_REQ, delete the packet handlers for those and
			   delete them as well. */
			if ((err = dl->ops->dl_unbind(dl)))
				goto error;
			dl_set_state(dl, DL_UNBOUND);
			break;
		case DL_UDQOS_PENDING:
			dl_set_state(dl, DL_DATAXFER);
			break;
		case DL_CONN_RES_PENDING:
			err = dl->ops->dl_accept(dl, q, p->connect_res.dl_correlation,
						 p->connect_res.dl_resp_token,
						 (caddr_t) p + p->dl_qos_offset, p->dl_qos_length);
			if (err)
				goto error;
			if (p->connect_res.dl_resp_token == 0)
				dl_set_state(dl, DL_DATAXFER);
			else
				dl_set_state(dl, dl->conind > 0 ? DL_INCON_PENDING : DL_IDLE);
			break;
		case DL_RESET_RES_PENDING:
			dl_set_state(dl, DL_DATAXFER);
			break;
		case DL_DISCON8_PENDING:
		case DL_DISCON9_PENDING:
		case DL_DISCON11_PENDING:
		case DL_DISCON12_PENDING:
		case DL_DISCON13_PENDING:
			err = dl->ops->dl_disconnect(dl, q, p->disconnect_req.dl_reason,
						     p->disconnect_req.dl_correlation);
			if (err)
				goto error;
			dl_set_state(dl, DL_IDLE);
			break;
		case DL_SUBS_BIND_PND:
			err = dl_op_subs_bind(dl, q, (caddr_t) p +
					      p->subs_bind_req.dl_subs_sap_offset,
					      p->subs_bind_req.dl_subs_sap_length,
					      p->dl_subs_bind_class);
			if (err)
				goto error;
			dl_set_state(dl, DL_IDLE);
			break;
		case DL_SUBS_UNBIND_PND:
			/* FIXME: if we are an ether type, delete the packet handler for the
			   current SAP (dl->sap.curr) remove and delete the SAP structure to
			   complete the sap unbind operation */
			err = dl_op_subs_unbind(dl, (caddr_t) p +
						p->subs_unbind_req.dl_subs_sap_offset,
						p->subs_unbind_req.dl_subs_sap_length);
			if (err)
				goto error;
			dl_set_state(dl, DL_IDLE);
			break;
		default:
			/* This is not an error.  There are many primitives that require
			   acknowledgement that do not change state. */
			switch (prim) {
			case DL_ENABMULTI_REQ:
				/* FIXME: finalize multicast address enable */
				err = dl_op_multicast(dl, prim, (caddr_t) p +
						      p->enabmulti_req.dl_addr_offset,
						      p->enabmulti_req.dl_addr_length);
				break;
			case DL_DISABMULTI_REQ:
				/* FIXME: finalize multicast address disable */
				err = dl_op_multicast(dl, prim, (caddr_t) p +
						      p->disabmulti_req.dl_addr_offset,
						      p->disabmulti_req.dl_addr_length);
				break;
			case DL_PROMISCON_REQ:
				err = dl_op_promisc(dl, prim, p->promiscon_req.dl_level);
				break;
			case DL_PROMISCOFF_REQ:
				err = dl_op_promisc(dl, prim, p->promiscoff_req.dl_level);
				break;
			case DL_UDQOS_REQ:
				err = dl->ops->dl_udqos(dl, q, (caddr_t) p +
							p->udqos_req.dl_qos_offset,
							p->udqos_req.dl_qos_length);
				break;
			case DL_SET_PHYS_ADDR_REQ:
				err = dl->ops->dl_setphys(dl, q, (caddr_t) p +
							  p->set_phys_addr_req.dl_addr_offset,
							  p->set_phys_addr_req.dl_addr_length);
				break;
			case DL_INTR_MODE_REQ:
				err = 0;
				break;
			case DL_INTR_MODE_REQ:
				err = 0;
				break;
			case DL_AGGR_REQ:
				err = 0;
				break;
			case DL_UNAGGR_REQ:
				err = 0;
				break;
			case DL_PASSIVE_REQ:
				err = 0;
				break;
			case DL_INTR_MODE_REQ:
				err = 0;
				break;
			case DL_HP_HW_RESET_REQ:
				err = 0;
				break;
			case DL_HP_SET_ACK_TO_REQ:
				dl->proto.hp_info.dl_ack_to = p->hp_set_ack_to_req.dl_ack_to * 100;
				err = 0;
				break;
			case DL_HP_SET_P_TO_REQ:
				dl->proto.hp_info.dl_p_to = p->hp_set_p_to_req.dl_p_to * 100;
				err = 0;
				break;
			case DL_HP_SET_REJ_TO_REQ:
				dl->proto.hp_info.dl_rej_to = p->hp_set_rej_to_req.dl_rej_to * 100;
				err = 0;
				break;
			case DL_HP_SET_BUSY_TO_REQ:
				dl->proto.hp_info.dl_busy_to = p->hp_set_busy_to_req.dl_busy_to * 100;
				err = 0;
				break;
			case DL_HP_SET_SEND_ACK_TO_REQ:
				dl->proto.hp_info.dl_send_ack_to = p->hp_set_send_ack_to_req.dl_send_ack_to * 100;
				err = 0;
				break;
			case DL_HP_SET_MAX_RETRIES_REQ:
				dl->proto.hp_info.dl_max_retries =
					p->hp_set_max_retries_req.dl_max_retries;
				err = 0;
				break;
			case DL_HP_SET_ACK_THRESHOLD_REQ:
				dl->proto.hp_info.dl_ack_thres = p->hp_set_ack_threshold_req.dl_ack_thres;
				err = 0;
				break;
			case DL_HP_SET_LOCAL_WIN_REQ:
				dl->proto.hp_info.dl_local_win =
					p->hp_set_local_win_req.dl_local_win;
				err = 0;
				break;
			case DL_HP_SET_REMOTE_WIN_REQ:
				dl->proto.hp_info.dl_remote_win =
					p->hp_set_remote_win_req.dl_remote_win;
				err = 0;
				break;
			case DL_HP_CLEAR_STATS_REQ:
				dl->proto.hp_info.dl_mem_fails = 0;
				dl->proto.hp_info.dl_queue_fails = 0;
				dl->proto.hp_info.dl_ack_to_cnt = 0;
				dl->proto.hp_info.dl_p_to_cnt = 0;
				dl->proto.hp_info.dl_rej_to_cnt = 0;
				dl->proto.hp_info.dl_busy_to_cnt = 0;
				dl->proto.hp_info.dl_i_pkts_in = 0;
				dl->proto.hp_info.dl_i_pkts_oos = 0;
				dl->proto.hp_info.dl_i_pkts_in_drop = 0;
				dl->proto.hp_info.dl_i_pkts_out = 0;
				dl->proto.hp_info.dl_s_pkts_in = 0;
				dl->proto.hp_info.dl_s_pkts_out = 0;
				dl->proto.hp_info.dl_u_pkts_in = 0;
				dl->proto.hp_info.dl_u_pkts_out = 0;
				dl->proto.hp_info.dl_bad_pkts = 0;
				dl->proto.hp_info.dl_retry_cnt = 0;
				dl->proto.hp_info.dl_max_retry_cnt = 0;
				dl->proto.hp_info.dl_remote_busy_cnt = 0;
				dl->proto.hp_info.dl_hw_req_fails = 0;
				err = 0;
				break;
			case DL_HP_SET_LOCAL_BUSY_REQ:
				err = dl_op_setbusy(dl, q, prim);
				break;
			case DL_HP_CLEAR_LOCAL_BUSY_REQ:
				err = dl_op_setbusy(dl, q, prim);
				break;
			default:
				err = 0;
				break;
			}
			if (err)
				goto error;
			break;
		}
		freemsg(msg);
		mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_OK_ACK");
		putnext(dl->oq, mp);
		return (0);
	      error:
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
dl_error_ack(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong prim, dl_long error)
{
	dl_error_ack_t *p;
	mblk_t *mp;
	int err;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_ERROR_ACK;
		p->dl_error_primitive = prim;
		p->dl_errno = error < 0 ? DL_SYSERR : error;
		p->dl_unix_errno = error < 0 ? -error : 0;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		dl_restore_state(dl);
		mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_ERROR_ACK");
		putnext(dl->oq, mp);
		return (0);
	      error:
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
dl_error_reply(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong prim, dl_long error)
{
	switch (error) {
	case 0:
	case -EAGAIN:
	case -EBUSY:
	case -ENOMEM:
	case -ENOBUFS:
	case -ENOSR:
	case -EDEADLK:
		return (error);
	}
	mi_strlog(dl->oq, 0, SL_TRACE, "%s: %s: %s", dl_primname(prim), dl_errname(error),
		  dl_strerror(error));
	return dl_error_ack(dl, q, msg, prim, error);
}
static fastcall noinline __unlikely int
dl_subs_bind_ack(struct dl *dl, queue_t *q, mblk_t *msg, caddr_t sap_ptr, size_t sap_len,
		 dl_ulong sap_cls)
{
	dl_subs_bind_ack_t *p;
	mblk_t *mp;
	int err;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + sap_len, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_SUBS_BIND_ACK;
		p->dl_subs_sap_offset = sap_len ? sizeof(*p) : 0;
		p->dl_subs_sap_length = sap_len;
		mp->b_wptr += sizeof(*p);
		bcopy(sap_ptr, mp->b_wptr, sap_len);
		mp->b_wptr += sap_len;
		if ((err = dl->ops->dl_subs_bind(dl, q, msg, sap_ptr, sap_len, sap_cls)))
			goto error;
		dl_set_state(dl, DL_IDLE);
		mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_SUBS_BIND_ACK");
		putnext(dl->oq, mp);
		return (0);
	      error:
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}
static fastcall inline __hot int
dl_unitdata_ind(struct dl *dl, queue_t *q, mblk_t *msg, struct strbuf *dst, struct strbuf *src,
		dl_ulong grp, mblk_t *dp)
{
	dl_unitdata_ind_t *p;
	mblk_t *mp;
	caddr_t dst_ptr = dst ? (caddr_t) dst->buf : NULL;
	size_t dst_len = dst ? (size_t) dst->len : 0;
	caddr_t src_ptr = src ? (caddr_t) src->buf : NULL;
	size_t src_len = src ? (size_t) src->len : 0;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + dst_len + src_len, BPRI_MED)))) {
		if (likely(bcanputnext(dl->oq, dp->b_band))) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = dp->b_band;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_UNITDATA_IND;
			p->dl_dest_addr_length = dst_len;
			p->dl_dest_addr_offset = dst_len ? sizeof(*p) : 0;
			p->dl_src_addr_length = src_len;
			p->dl_src_addr_offset = src_len ? sizeof(*p) + dst_len : 0;
			p->dl_group_address = grp;
			mp->b_wptr += sizeof(*p);
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
			bcopy(src_ptr, mp->b_wptr, src_len);
			mp->b_wptr += src_len;
			mp->b_cont = dp;
			trimb(msg, dp);
			mi_strlog(dl->oq, STRLOGDA, SL_TRACE, "<- DL_UNITDATA_IND");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
dl_uderror_ind(struct dl *dl, queue_t *q, mblk_t *msg, caddr_t dst_ptr, size_t dst_len,
	       dl_long error, mblk_t *dp)
{
	dl_uderror_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + dst_len, BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_UDERROR_IND;
			p->dl_dest_addr_length = dst_len;
			p->dl_dest_addr_offset = dst_len ? sizeof(*p) : 0;
			p->dl_unix_errno = error < 0 ? -error : 0;
			p->dl_errno = error < 0 ? DL_SYSERR : error;
			mp->b_wptr += sizeof(*p);
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
			mp->b_cont = dp;
			trimb(msg, dp);
			mi_strlog(dl->oq, STRLOGDA, SL_TRACE, "<- DL_UDERROR_IND");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
dl_uderror_reply(struct dl *dl, queue_t *q, mblk_t *msg, caddr_t dst_ptr, size_t dst_len, dl_long
		 error, mblk_t *dp)
{
	switch (error) {
	case 0:
	case -EAGAIN:
	case -EBUSY:
	case -ENOMEM:
	case -ENOBUFS:
	case -ENOSR:
	case -EDEADLK:
		return (error);
	}
	mi_strlog(dl->oq, 0, SL_TRACE, "DL_UNITDATA_REQ: %s: %s", dl_errname(error),
		  dl_strerror(error));
	return dl_uderror_ind(dl, q, msg, dst_ptr, dst_len, error, dp);
}
static fastcall noinline __unlikely int
dl_connect_ind(struct dl *dl, queue_t *q, mblk_t *msg, mblk_t *seq, struct
	       strbuf *cld, struct strbuf *clg, struct strbuf *qos)
{
	dl_connect_ind_t *p;
	mblk_t *mp;

	caddr_t cld_ptr = cld ? (caddr_t) cld->buf : NULL;
	caddr_t clg_ptr = clg ? (caddr_t) clg->buf : NULL;
	caddr_t qos_ptr = qos ? (caddr_t) qos->buf : NULL;
	size_t cld_len = cld ? (size_t) cld->len : 0;
	size_t clg_len = clg ? (size_t) clg->len : 0;
	size_t qos_len = qos ? (size_t) qos->len : 0;
	size_t msg_len = sizeof(*p) + cld_len + clg_len + qos_len;

	if (likely(!!(mp = mi_allocb(q, msg_len, BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_CONNECT_IND;
			p->dl_correlation = (dl_ulong) (ulong) seq;
			p->dl_called_addr_length = cld_len;
			p->dl_called_addr_offset = cld_len ? sizeof(*p) + qos_len : 0;
			p->dl_calling_addr_length = clg_len;
			p->dl_calling_addr_offset = clg_len ? sizeof(*p) + qos_len + cld_len : 0;
			p->dl_qos_length = qos_len;
			p->dl_qos_offset = qos_len ? sizeof(*p) : 0;
			p->dl_growth = 0;
			mp->b_wptr += sizeof(*p);
			bcopy(cld_ptr, mp->b_wptr, cld_len);
			mp->b_wptr += cld_len;
			bcopy(clg_ptr, mp->b_wptr, clg_len);
			mp->b_wptr += clg_len;
			bcopy(qos_ptr, mp->b_wptr, qos_len);
			mp->b_wptr += qos_len;
			freemsg(mp);
			bufq_queue(&dl->conq, seq);
			dl->conind++;
			dl_set_state(dl, DL_INCON_PENDING);
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_CONNECT_IND");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
dl_connect_con(struct dl *dl, queue_t *q, mblk_t *msg, struct strbuf *res, struct strbuf *qos)
{
	dl_connect_con_t *p;
	mblk_t *mp;

	caddr_t res_ptr = res ? (caddr_t) res->buf : NULL;
	caddr_t qos_ptr = qos ? (caddr_t) qos->buf : NULL;
	size_t res_len = res ? (size_t) res->len : 0;
	size_t qos_len = qos ? (size_t) qos->len : 0;
	size_t msg_len = sizeof(*p) + res_len + qos_len;

	if (likely(!!(mp = mi_allocb(q, msg_len, BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_CONNECT_CON;
			p->dl_resp_addr_length = res_len;
			p->dl_resp_addr_offset = res_len ? sizeof(*p) + qos_len : 0;
			p->dl_qos_length = qos_len;
			p->dl_qos_offset = qos_len ? sizeof(*p) : 0;
			p->dl_growth = 0;
			mp->b_wptr += sizeof(*p);
			bcopy(res_ptr, mp->b_wptr, res_len);
			mp->b_wptr += res_len;
			bcopy(qos_ptr, mp->b_wptr, qos_len);
			mp->b_wptr += qos_len;
			freemsg(msg);
			dl_set_state(dl, DL_DATAXFER);
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE < "<- DL_CONNECT_CON");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
dl_token_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_token_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_TOKEN_ACK;
		p->dl_token = (dl_ulong) (ulong) (caddr_t) RD(dl->oq);
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		putnext(dl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
dl_disconnect_ind(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong orig, dl_ulong reason,
		  dl_ulong seq)
{
	dl_disconnect_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_DISCONNECT_IND;
			p->dl_originator = orig;
			p->dl_reason = reason;
			p->dl_correlation = seq;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			dl_set_state(dl, DL_IDLE);
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_DISCONNECT_IND");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
dl_reset_ind(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong orig, dl_ulong reason)
{
	dl_reset_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_RESET_IND;
			p->dl_originator = orig;
			p->dl_reason = reason;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			dl_set_state(dl, DL_PROV_RESET_PENDING);
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_RESET_IND");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
dl_reset_con(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_reset_con_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_RESET_CON;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			dl_set_state(dl, DL_DATAXFER);
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_RESET_CON");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

#if 0
static fastcall inline __hot int
dl_data_ack_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
}
static fastcall inline __hot int
dl_data_ack_status_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
}
static fastcall inline __hot int
dl_reply_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
}
static fastcall inline __hot int
dl_reply_status_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
}
static fastcall inline __hot int
dl_reply_update_status_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
}
#endif
static fastcall noinline __unlikely int
dl_xid_ind(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong flag, caddr_t dst_ptr, size_t dst_len,
	   caddr_t src_ptr, size_t src_len, mblk_t *dp)
{
	dl_xid_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + dst_len + src_len, BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_XID_IND;
			p->dl_flag = flag;
			p->dl_dest_addr_length = dst_len;
			p->dl_dest_addr_offset = dst_len ? sizeof(*p) : 0;
			p->dl_src_addr_length = src_len;
			p->dl_src_addr_offset = src_len ? sizeof(*p) + dst_len : 0;
			mp->b_wptr += sizeof(*p);
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
			bcopy(src_ptr, mp->b_wptr, src_len);
			mp->b_wptr += src_len;
			mp->b_cont = dp;
			trimb(msg, dp);
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_XID_IND");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
dl_xid_con(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong flag, caddr_t dst_ptr, size_t dst_len,
	   caddr_t src_ptr, size_t src_len, mblk_t *dp)
{
	dl_xid_con_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + dst_len + src_len, BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_XID_CON;
			p->dl_flag = flag;
			p->dl_dest_addr_length = dst_len;
			p->dl_dest_addr_offset = dst_len ? sizeof(*p) : 0;
			p->dl_src_addr_length = src_len;
			p->dl_src_addr_offset = src_len ? sizeof(*p) + dst_len : 0;
			mp->b_wptr += sizeof(*p);
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
			bcopy(src_ptr, mp->b_wptr, src_len);
			mp->b_wptr += src_len;
			mp->b_cont = dp;
			trimb(msg, dp);
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_XID_CON");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
dl_test_ind(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong flag, caddr_t dst_ptr, size_t dst_len,
	    caddr_t src_ptr, size_t src_len, mblk_t *dp)
{
	dl_test_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + dst_len + src_len, BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_TEST_IND;
			p->dl_flag = flag;
			p->dl_dest_addr_length = dst_len;
			p->dl_dest_addr_offset = dst_len ? sizeof(*p) : 0;
			p->dl_src_addr_length = src_len;
			p->dl_src_addr_offset = src_len ? sizeof(*p) + dst_len : 0;
			mp->b_wptr += sizeof(*p);
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
			bcopy(src_ptr, mp->b_wptr, src_len);
			mp->b_wptr += src_len;
			mp->b_cont = dp;
			trimb(msg, dp);
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_TEST_IND");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
dl_test_con(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong flag, caddr_t dst_ptr, size_t dst_len,
	    caddr_t src_ptr, size_t src_len, mblk_t *dp)
{
	dl_test_con_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + dst_len + src_len, BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_TEST_CON;
			p->dl_flag = flag;
			p->dl_dest_addr_length = dst_len;
			p->dl_dest_addr_offset = dst_len ? sizeof(*p) : 0;
			p->dl_src_addr_length = src_len;
			p->dl_src_addr_offset = src_len ? sizeof(*p) + dst_len : 0;
			mp->b_wptr += sizeof(*p);
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
			bcopy(src_ptr, mp->b_wptr, src_len);
			mp->b_wptr += src_len;
			mp->b_cont = dp;
			trimb(msg, dp);
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_TEST_CON");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
dl_phys_addr_ack(struct dl *dl, queue_t *q, mblk_t *msg, caddr_t add_ptr, size_t add_len)
{
	dl_phys_addr_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + add_len, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_PHYS_ADDR_ACK;
		p->dl_addr_length = add_len;
		p->dl_addr_offset = add_len ? sizeof(*p) : 0;
		mp->b_wptr += sizeof(*p);
		bcopy(add_ptr, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
		freemsg(msg);
		mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_PHYS_ADDR_ACK");
		putnext(dl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
dl_get_statistics_ack(struct dl *dl, queue_t *q, mblk_t *msg, caddr_t sta_ptr, size_t sta_len)
{
	dl_get_statistics_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + sta_len, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_GET_STATISTICS_ACK;
		p->dl_stat_length = sta_len;
		p->dl_stat_offset = sta_len ? sizeof(*p) : 0;
		mp->b_wptr += sizeof(*p);
		bcopy(sta_ptr, mp->b_wptr, sta_len);
		mp->b_wptr += sta_len;
		freemsg(msg);
		mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_GET_STATISTICS_ACK");
		putnext(dl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
dl_notify_ack(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong flags)
{
	dl_notify_ack_t *p;
	mblk_t *mp;
	int err;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_NOTIFY_ACK;
			p->dl_notifications = dl->notifications.supp;
			mp->b_wptr += sizeof(*p);
			if ((err = dl->ops->dl_notify(dl, flags)))
				goto error;
			freemsg(msg);
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_NOTIFY_ACK");
			putnext(dl->oq, mp);
			return (0);
		}
		err = -EBUSY;
		goto error;
	      error:
		freeb(mp);
		return (err);

	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
dl_notify_ind(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong flag, dl_ulong data, caddr_t add_ptr,
	      size_t add_len, mblk_t *dp)
{
	dl_notify_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + add_len, BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_NOTIFY_IND;
			p->dl_notification = flag;
			p->dl_data = data;
			p->dl_addr_length = add_len;
			p->dl_addr_offset = add_len ? sizeof(*p) : 0;
			mp->b_wptr += sizeof(*p);
			bcopy(add_ptr, mp->b_wptr, add_len);
			mp->b_wptr += add_len;
			mp->b_cont = dp;
			trimb(msg, dp);
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_NOTIFY_IND");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
dl_aggr_ind(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong key, dl_ulong port, caddr_t add_ptr,
	    size_t add_len)
{
	dl_aggr_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + add_len, BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_AGGR_IND;
			p->dl_key = key;
			p->dl_port = port;
			p->dl_addr_length = add_len;
			p->dl_addr_offset = add_len ? sizeof(*p) : 0;
			mp->b_wptr += sizeof(*p);
			bcopy(add_ptr, mp->b_wptr, add_len);
			mp->b_wptr += add_len;
			freemsg(msg);
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_AGGR_IND");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
dl_capability_ack(struct dl *dl, queue_t *q, mblk_t *msg, caddr_t sub_ptr, size_t sub_len)
{
	dl_capability_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + sub_len, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_CAPABILITY_ACK;
		p->dl_sub_length = sub_len;
		p->dl_sub_offset = sub_len ? sizeof(*p) : 0;
		mp->b_wptr += sizeof(*p);
		bcopy(sub_ptr, mp->b_wptr, sub_len);
		mp->b_wptr += sub_len;
		freemsg(msg);
		mi_strlog(dl->oq, STRLOGTX, SL_TRACE < "<- DL_CAPABILITY_ACK");
		putnext(dl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
dl_control_ack(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong operation, dl_ulong type,
	       caddr_t key_ptr, size_t key_len, caddr_t dat_ptr, size_t dat_len)
{
	dl_control_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + key_len + dat_len, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_CONTROL_ACK;
		p->dl_operation = operation;
		p->dl_type = type;
		p->dl_key_length = key_len;
		p->dl_key_offset = key_len ? sizeof(*p) : 0;
		p->dl_data_length = dat_len;
		p->dl_data_offset = dat_len ? sizeof(*p) + key_len : 0;
		mp->b_wptr += sizeof(*p);
		bcopy(key_ptr, mp->b_wptr, key_len);
		mp->b_wptr += key_len;
		bcopy(dat_ptr, mp->b_wptr, dat_len);
		mp->b_wptr += dat_len;
		freemsg(msg);
		mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_CONTROL_ACK");
		putnext(dl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
dl_hp_ppa_ack(struct dl *dl, queue_t *q, mblk_t *msg, caddr_t inf_ptr, size_t inf_cnt)
{
	dl_hp_ppa_ack_t *p;
	mblk_t *mp;
	size_t inf_len = inf_cnt * sizeof(dl_hp_ppa_info_t);

	/* FIXME: we should lock down (read-lock) the global PPA list and determine the number of
	   PPA structures necessary at this point. */
	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + inf_len, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_HP_PPA_ACK;
		p->dl_length = inf_len;
		p->dl_count = inf_cnt;
		p->dl_offset = inf_len ? sizeof(*p) : 0;
		mp->b_wptr += sizeof(*p);
		/* FIXME: we should actually fill out the individual PPA structures here after
		   locking down (read-lock) the global PPA list. */
		bcopy(inf_ptr, mp->b_wptr, inf_len);
		mp->b_wptr += inf_len;
		/* FIXME: must unlock the global PPA list. */
		freemsg(msg);
		mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_HP_PPA_ACK");
		putnext(dl->oq, mp);
		return (0);
	}
	/* FIXME: must unlock the global PPA list. */
	return (-ENOBUFS);
}
static fastcall noinline __unlikely int
dl_hp_multicast_list_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_hp_multicast_list_ack_t *p;
	mblk_t *mp;

	/* FIXME: determine the number of multicast list items to transfer. */
	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + inf_len, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_HP_MULTICAST_LIST_ACK;
		p->dl_length = inf_len;
		p->dl_count = inf_cnt;
		p->dl_offset = inf_len ? sizeof(*p) : 0;
		mp->b_wptr += sizeof(*p);
		/* FIXME: we should actually fill out the individual multicast address structure
		   here. */
		bcopy(inf_ptr, mp->b_wptr, inf_len);
		mp->b_wptr += inf_len;
		freemsg(msg);
		mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_HP_MULTICAST_LIST_ACK");
		putnext(dl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static fastcall inline __hot int
dl_hp_rawdata_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
}
static fastcall noinline __unlikely int
dl_hp_info_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_hp_info_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_HP_INFO_ACK;
		p->dl_mem_fails = dl->proto.hp_info.dl_mem_fails;
		p->dl_queue_fails = dl->proto.hp_info.dl_queue_fails;
		/* NOTE: timeouts are always stored in milliseconds, but must be returned in
		   deciseconds. */
		p->dl_ack_to = (dl->proto.hp_info.dl_ack_to + 99) / 100;
		p->dl_p_to = (dl->proto.hp_info.dl_p_to + 99) / 100;
		p->dl_rej_to = (dl->proto.hp_info.dl_rej_to + 99) / 100;
		p->dl_busy_to = (dl->proto.hp_info.dl_busy_to + 99) / 100;
		p->dl_send_ack_to = (dl->proto.hp_info.dl_send_ack_to + 99) / 100;
		p->dl_ack_to_cnt = dl->proto.hp_info.dl_ack_to_cnt;
		p->dl_p_to_cnt = dl->proto.hp_info.dl_p_to_cnt;
		p->dl_rej_to_cnt = dl->proto.hp_info.dl_rej_to_cnt;
		p->dl_busy_to_cnt = dl->proto.hp_info.dl_busy_to_cnt;
		p->dl_local_win = dl->proto.hp_info.dl_local_win;
		p->dl_remote_win = dl->proto.hp_info.dl_remote_win;
		p->dl_i_pkts_in = dl->proto.hp_info.dl_i_pkts_in;
		p->dl_i_pkts_oos = dl->proto.hp_info.dl_i_pkts_oos;
		p->dl_i_pkts_in_drop = dl->proto.hp_info.dl_i_pkts_in_drop;
		p->dl_i_pkts_out = dl->proto.hp_info.dl_i_pkts_out;
		p->dl_i_pkts_retrans = dl->proto.hp_info.dl_i_pkts_retrans;
		p->dl_s_pkts_in = dl->proto.hp_info.dl_s_pkts_in;
		p->dl_s_pkts_out = dl->proto.hp_info.dl_s_pkts_out;
		p->dl_u_pkts_in = dl->proto.hp_info.dl_u_pkts_in;
		p->dl_u_pkts_out = dl->proto.hp_info.dl_u_pkts_out;
		p->dl_bad_pkts = dl->proto.hp_info.dl_bad_pkts;
		p->dl_retry_cnt = dl->proto.hp_info.dl_retry_cnt;
		p->dl_max_retry_cnt = dl->proto.hp_info.dl_max_retry_cnt;
		p->dl_max_retries = dl->proto.hp_info.dl_max_retries;
		p->dl_ack_thres = dl->proto.hp_info.dl_ack_thres;
		p->dl_remote_busy_cnt = dl->proto.hp_info.dl_remote_busy_cnt;
		p->dl_hw_req_fails = dl->proto.hp_info.dl_hw_req_fails;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_HP_INFO_ACK");
		putnext(dl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 * --------------------------------------------------------------------------
 *
 * DLS USER TO DLS PROVIDER PRIMITIVES (RECEIVED FROM ABOVE)
 *
 * --------------------------------------------------------------------------
 */
static fastcall inline __hot int
dl_data_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	int err;
	size_t dlen;

	if (!(dl->proto.info.dl_service_mode & (DL_CODLS | DL_HP_RAWDLS)))
		goto unsupported;
	switch (dl_get_state(dl)) {
	case DL_DATAXFER:
		break;
	case DL_IDLE:
	case DL_PROV_RESET_PENDING:
		goto discard;
	default:
		goto outstate;
	}
	if (dl->ops == NULL || dl->ops->dl_snddata == NULL)
		goto notsupported;
	dlen = msgdsize(mp);
	if (dlen < dl->proto.info.dl_min_sdu || dlen > dl->proto.info.dl_max_sdu)
		goto baddata;
	if ((err = dl_op_snddata(dl, q, NULL, 0, mp)))
		goto merror;
	return (0);
      baddata:
	err = DL_BADDATA;
	goto merror;
      notsupported:
	err = DL_NOTSUPPORTED;
	goto merror;
      outstate:
	err = DL_OUTSTATE;
	goto merror;
      discard:
	freemsg(mp);
	return (0);
      unsupported:
	err = DL_UNSUPPORTED;
	goto merror;
      merror:
	return m_error_reply(dl, q, mp, 0, err);
}
static fastcall noinline __unlikely int
dl_info_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_info_ack(dl, q, mp);
}
static fastcall noinline __unlikely int
dl_attach_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_attach_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto fault;
	if (dl->proto.info.dl_provider_style != DL_STYLE2)
		goto unsupported;
	if (dl_get_state(dl) != DL_UNATTACHED)
		goto outstate;
	dl_set_state(dl, DL_ATTACH_PENDING);
	if ((err = dl_ok_ack(dl, q, mp, DL_ATTACH_REQ, (union DL_primitives *) p)))
		goto error;
	return (0);
      outstate:
	err = DL_OUTSTATE;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      fault:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_ATTACH_REQ, err);
}
static fastcall noinline __unlikely int
dl_detach_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	int err;

	if (dl->proto.info.dl_provider_style != DL_STYLE2)
		goto unsupported;
	if (dl_get_state(dl) != DL_UNBOUND)
		goto outstate;
	dl_set_state(dl, DL_DETACH_PENDING);
	if ((err = dl_ok_ack(dl, q, mp, DL_DETACH_REQ, (union DL_primitives *) p)))
		goto error;
	return (0);
      outstate:
	err = DL_OUTSTATE;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_DETACH_REQ, err);
}
static fastcall noinline __unlikely int
dl_bind_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_bind_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto fault;
	if (dl_get_state(dl) != DL_UNBOUND)
		goto outstate;
	switch (p->dl_service_mode) {
	case DL_CODLS:
	case DL_CLDLS:
	case DL_CODLS | DL_CLDLS:	/* special AIX mode */
	case DL_ACLDLS | DL_CLDLS:	/* special AIX mode */
	case DL_HP_RAWDLS:
		if ((dl->proto.info.dl_service_mode ^ p->dl_service_mode) & p->dl_service_mode)
			goto unsupported;
		break;
	default:
		goto unsupported;
	}
	switch (p->dl_xidtest_flg & (DL_AUTO_XID | DL_AUTO_TEST)) {
	case 0:
		break;
	case DL_AUTO_XID:
		if ((dl->xidtest ^ p->dl_xidtest_flg) & p->dl_xidtest_flg)
			goto noxidauto;
		break;
	case DL_AUTO_TEST:
		if ((dl->xidtest ^ p->dl_xidtest_flg) & p->dl_xidtest_flg)
			goto notestauto;
		break;
	case (DL_AUTO_XID | DL_AUTO_TEST):
		if ((dl->xidtest ^ p->dl_xidtest_flg) & p->dl_xidtest_flg)
			goto noauto;
		break;
	}
	if (!(p->dl_service_mode & DL_CODLS)) {
		p->dl_max_conind = 0;	/* ignore this field */
		p->dl_conn_mgmt = 0;	/* ignore this field */
	}
	/* connection management streams must be responders */
	if (p->dl_conn_mgmt != 0 && p->dl_max_conind == 0)
		goto inval;
	if (p->dl_conn_mgmt != 0)
		p->dl_sap = 0;	/* ignore this field */
	/* check sap for validity */
	if (dl->ops->dl_sap_check == NULL)
		goto fault;
	if ((err = dl->ops->dl_sap_check(dl, p->dl_sap)))
		goto error;
	dl->conmax = p->dl_max_conind;
	dl->conind = 0;
	dl_set_state(dl, DL_BIND_PENDING);
	if ((err = dl_bind_ack(dl, q, mp)))
		goto error;
	return (0);
      inval:
	err = -EINVAL;
	goto error;
      noauto:
	err = DL_NOAUTO;
	goto error;
      notestauto:
	err = DL_NOTESTAUTO;
	goto error;
      noxidauto:
	err = DL_NOXIDAUTO;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      fault:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_BIND_REQ, err);
}
static fastcall noinline __unlikely int
dl_unbind_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	int err;

	if (dl_get_state(dl) != DL_IDLE)
		goto outstate;
	dl_set_state(dl, DL_UNBIND_PENDING);
	if ((err = dl_ok_ack(dl, q, mp, DL_UNBIND_REQ, (union DL_primitives *) NULL)))
		goto error;
	return (0);
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_UNBIND_REQ, err);
}
static fastcall noinline __unlikely int
dl_subs_bind_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_subs_bind_req_t *p = (typeof(p)) mp->b_rtpr;
	int err;
	caddr_t sap_ptr;
	size_t sap_len;
	dl_ulong sap_cls;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto fault;
	if (!MBLKIN(mp, p->dl_subs_sap_offset, p->dl_subs_sap_length))
		goto badaddr;
	switch ((sap_cls = p->dl_subs_bind_class)) {
	case DL_PEER_BIND:
	case DL_HIERARCHICAL_BIND:
		break;
	default:
		goto unsupported;
	}
	if (dl_get_state(dl) != DL_IDLE)
		goto outstate;
	dl_set_state(dl, DL_SUBS_BIND_PND);
	sap_ptr = (caddr_t) (mp->b_rptr + p->dl_subs_sap_offset);
	sap_len = (size_t) (p->dl_sap_length);
	if ((err = dl_subs_bind_ack(dl, q, mp, sap_ptr, sap_len, sap_cls)))
		goto error;
	return (0);
      outstate:
	err = DL_OUTSTATE;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      badaddr:
	err = DL_BADADDR;
	goto error;
      fault:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_SUBS_BIND_REQ, err);
}
static fastcall noinline __unlikely int
dl_subs_unbind_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_subs_unbind_req_t *p = (typeof(p)) mp->b_rptr;
	int err;
	caddr_t sap_ptr;
	size_t sap_len;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto fault;
	if (!MBLKIN(mp, p->dl_subs_sap_offset, p->dl_subs_sap_length))
		goto badaddr;
	if (dl_get_state(dl) != DL_IDLE)
		goto outstate;
	if (dl->ops == NULL || dl->ops->dl_subs_unbind == NULL)
		goto notsupported;
	sap_ptr = (caddr_t) (mp->b_rptr + p->dl_subs_sap_offset);
	sap_len = (size_t) (p->dl_sap_length);
	dl_set_state(dl, DL_SUBS_UNBIND_PND);
	if ((err = dl_ok_ack(dl, q, mp, DL_SUBS_UNBIND_REQ, (union DL_primitives *) p)))
		goto error;
	return (0);
      notsupported:
	err = DL_NOTSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badaddr:
	err = DL_BADADDR;
	goto error;
      fault:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_SUBS_UNBIND_REQ, err);
}
static fastcall noinline __unlikely int
dl_enabmulti_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_enabmulti_req_t *p = (typeof(p)) mp->b_rptr;
	int err;
	caddr_t add_ptr;
	size_t add_len;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto fault;
	if (dl_get_state(dl) == DL_UNATTACHED)
		goto outstate;
	if (!MBLKIN(mp, p->dl_addr_offset, p->dl_addr_length))
		goto badaddr;
	if (dl->ops == NULL || dl->ops.dl_multicast == NULL)
		goto notsupported;
	add_ptr = (caddr_t) (mp->b_rptr + p->dl_addr_offset);
	add_len = (size_t) (p->dl_addr_length);
	if (add_len == 0 || add_len != dl->maclen)
		goto badaddr;
	if ((err = dl_ok_ack(dl, q, mp, DL_ENABMULTI_REQ, (union DL_primitives *) p)))
		goto error;
	return (0);
      notsupported:
	err = DL_NOTSUPPORTED;
	goto error;
      badaddr:
	err = DL_BADADDR;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      fault:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_ENABMULTI_REQ, err);
}
static fastcall noinline __unlikely int
dl_disabmulti_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_disabmulti_req_t *p = (typeof(p)) mp->b_rptr;
	int err;
	caddr_t add_ptr;
	size_t add_len;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto fault;
	if (dl_get_state(dl) == DL_UNATTACHED)
		goto outstate;
	if (!MBLKIN(mp, p->dl_addr_offset, p->dl_addr_length))
		goto badaddr;
	if (dl->ops == NULL || dl->ops.dl_multicast == NULL)
		goto notsupported;
	add_ptr = (caddr_t) (mp->b_rptr + p->dl_addr_offset);
	add_len = (size_t) (p->dl_addr_length);
	if ((err = dl_ok_ack(dl, q, mp, DL_DISABMULTI_REQ, (union DL_primitives *) p)))
		goto error;
	return (0);
      notsupported:
	err = DL_NOTSUPPORTED;
	goto error;
      badaddr:
	err = DL_BADADDR;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      fault:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_DISABMULTI_REQ, err);
}
static fastcall noinline __unlikely int
dl_promiscon_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_promiscon_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto fault;
	if (dl_get_state(dl) == DL_UNATTACHED)
		goto outstate;
	if (dl->ops == NULL || dl->ops->dl_promisc == NULL)
		goto notsupported;
	switch (p->dl_level) {
	case DL_PROMISC_PHYS:
	case DL_PROMISC_SAP:
	case DL_PROMISC_MULTI:
		break;
	default:
		goto unsupported;
	}
	if ((err = dl_ok_ack(dl, q, mp, DL_PROMISCON_REQ, (union DL_primitives *) p)))
		goto error;
	return (0);
      notsupported:
	err = DL_NOTSUPPORTED;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      fault:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_PROMISCON_REQ, err);

}
static fastcall noinline __unlikely int
dl_promiscoff_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_promiscoff_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto fault;
	if (dl_get_state(dl) == DL_UNATTACHED)
		goto outstate;
	if (dl->ops == NULL || dl->ops->dl_promisc == NULL)
		goto notsupported;
	switch (p->dl_level) {
	case DL_PROMISC_PHYS:
	case DL_PROMISC_SAP:
	case DL_PROMISC_MULTI:
		break;
	default:
		goto unsupported;
	}
	if ((err = dl_ok_ack(dl, q, mp, DL_PROMISCOFF_REQ, (union DL_primitives *) p)))
		goto error;
	return (0);
      notsupported:
	err = DL_NOTSUPPORTED;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      fault:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_PROMISCOFF_REQ, err);
}
static fastcall inline __hot int
dl_unitdata_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_unitdata_req_t *p = (typeof(p)) mp->b_rptr;
	int err;
	caddr_t dst_ptr = NULL;
	size_t dst_len = 0, dlen;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto fault;
	if (!(dl->proto.info.dl_service_mode & DL_CLDLS))
		goto unsupported;
	if (dl_get_state(dl) != DL_IDLE)
		goto outstate;
	if (dl->ops == NULL || dl->ops->dl_unitdata == NULL)
		goto notsupported;
	if (!MBLKIN(mp, p->dl_dest_addr_offset, p->dl_dest_addr_length))
		goto badaddr;
	dst_ptr = (caddr_t) p + p->dl_dest_addr_offset;
	dst_len = (size_t) p->dl_dest_addr_length;
	dlen = msgdsize(mp);
	if (dlen < dl->proto.info.dl_min_sdu || dlen > dl->proto.info.dl_max_sdu)
		goto baddata;
	if ((err = dl->ops->dl_unitdata(dl, q, mp, dst_ptr, dst_len, mp->b_cont)))
		goto error;
	return (0);
      baddata:
	err = DL_BADDATA;
	goto error;
      badaddr:
	err = DL_BADADDR;
	goto error;
      notsupported:
	err = DL_NOTSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      fault:
	err = -EFAULT;
	goto error;
      error:
	return dl_uderror_reply(dl, q, mp, dst_ptr, dst_len, err, mp->b_cont);
}
static fastcall inline __hot int
dl_udqos_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_udqos_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto fault;
	if (!(dl->proto.info.dl_service_mode & DL_CLDLS))
		goto unsupported;
	if (dl_get_state(dl) != DL_IDLE)
		goto outstate;
	if (dl->ops == NULL || dl->ops->dl_udqos == NULL)
		goto notsupported;
	if (!MBLKIN(mp, p->dl_qos_offset, p->dl_qos_length))
		goto badqostype;
	if (p->dl_qos_length < sizeof(dl_ulong))
		goto badqostype;
	if ((err = dl_ok_ack(dl, q, mp, DL_UDQOS_REQ, (union DL_primitives *) p)))
		goto error;
	return (0);
      badqostype:
	err = DL_BADQOSTYPE;
	goto error;
      notsupported:
	err = DL_NOTSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      fault:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_UDQOS_REQ, err);
}
static fastcall noinline __unlikely int
dl_connect_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_connect_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto fault;
	if (!(dl->proto.info.dl_service_mode & DL_CODLS))
		goto unsupported;
	if (dl_get_state(dl) != DL_IDLE)
		goto outstate;
	if (dl->ops == NULL || dl->ops->dl_connect == NULL)
		goto notsupported;
	if (!MBLKIN(mp, p->dl_dest_addr_offset, p->dl_dest_addr_length))
		goto badaddr;
	if (p->dl_dest_addr_length < dl->maclen)
		goto badaddr;
	if (!MBLKIN(mp, p->dl_qos_offset, p->dl_qos_length))
		goto badqostype;
	if (p->dl_qos_offset > 0 && p->dl_qos_offset < sizeof(dl_ulong))
		goto badqostype;
	dl_set_state(dl, DL_OUTCON_PENDING);
	err = dl->ops->dl_connect(dl, q, mp, (caddr_t) p + p->dl_dest_addr_offset,
				  p->dl_dest_addr_length, (caddr_t) p + p->dl_qos_offset,
				  p->dl_qos_length);
	if (err)
		goto error;
	return (0);
      badqostype:
	err = DL_BADQOSTYPE;
	goto error;
      badaddr:
	err = DL_BADADDR;
	goto error;
      notsupported:
	err = DL_NOTSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      fault:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_CONNECT_REQ, err);
}
static fastcall noinline __unlikely int
dl_connect_res(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_connect_res_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto fault;
	if (!(dl->proto.info.dl_service_mode & DL_CODLS))
		goto unsupported;
	if (dl_get_state(dl) != DL_INCON_PENDING)
		goto outstate;
	if (dl->ops == NULL || dl->ops->dl_accept == NULL)
		goto notsupported;
	if (!MBLKIN(mp, p->dl_qos_offset, p->dl_qos_length))
		goto badqostype;
	if (p->dl_qos_length > 0 && p->dl_qos_length < sizeof(dl_ulong))
		goto badqostype;
	if (p->dl_correlation == 0)
		goto badcorr;
	if (p->dl_resp_token == 0 && dl->conind > 1)
		goto pending;
	dl_set_state(dl, DL_CONN_RES_PENDING);
	if ((err = dl_ok_ack(dl, q, mp, DL_CONNECT_RES, (union DL_primitives *) p)))
		goto error;
	return (0);
      pending:
	err = DL_PENDING;
	goto error;
      badcorr:
	err = DL_BADCORR;
	goto error;
      badqostype:
	err = DL_BADQOSTYPE;
	goto error;
      notsupported:
	err = DL_NOTSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      fault:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_CONNECT_RES, err);
}
static fastcall noinline __unlikely int
dl_token_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_token_ack(dl, q, mp);
}
static fastcall noinline __unlikely int
dl_disconnect_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_disconnect_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto fault;
	if (!(dl->proto.info.dl_service_mode & DL_CODLS))
		goto unsupported;
	if (dl->ops == NULL || dl->ops->dl_disconnect == NULL)
		goto notsupported;
	switch (dl_get_state(dl)) {
	case DL_INCON_PENDING:
		if (p->dl_correlation == 0)
			goto badcorr;
		break;
	case DL_OUTCON_PENDING:
	case DL_DATAXFER:
	case DL_USER_RESET_PENDING:
	case DL_PROV_RESET_PENDING:
		if (p->dl_correlation != 0)
			goto badcorr;
		break;
	default:
		goto outstate;
	case DL_IDLE:
		goto discard;
	}
	switch (dl_get_state(dl)) {
	case DL_INCON_PENDING:
		dl_set_state(dl, DL_DISCON9_PENDING);
		break;
	case DL_OUTCON_PENDING:
		dl_set_state(dl, DL_DISCON8_PENDING);
		break;
	case DL_DATAXFER:
		dl_set_state(dl, DL_DISCON11_PENDING);
		break;
	case DL_USER_RESET_PENDING:
		dl_set_state(dl, DL_DISCON12_PENDING);
		break;
	case DL_PROV_RESET_PENDING:
		dl_set_state(dl, DL_DISCON13_PENDING);
		break;
	}
	if ((err = dl_ok_ack(dl, q, mp, DL_DISCONNECT_REQ, (union DL_primitives *) p)))
		goto error;
	return (0);
      discard:
	freemsg(mp);
	return (0);
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badcorr:
	err = DL_BADCORR;
	goto error;
      notsupported:
	err = DL_NOTSUPPORTED;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      fault:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_DISCONNECT_REQ, err);
}
static fastcall noinline __unlikely int
dl_reset_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	int err;

	if (!(dl->proto.info.dl_service_mode & DL_CODLS))
		goto unsupported;
	if (dl->ops == NULL || dl->ops->dl_reset == NULL)
		goto notsupported;
	if (dl_get_state(dl) == DL_IDLE)
		goto discard;
	if (dl_get_state(dl) != DL_DATAXFER)
		goto outstate;
	dl_set_state(dl, DL_USER_RESET_PENDING);
	if ((err = dl->ops->dl_reset(dl)))
		goto error;
	return (0);
      outstate:
	err = DL_OUTSTATE;
	goto error;
      discard:
	freemsg(mp);
	return (0);
      notsupported:
	err = DL_NOTSUPPORTED;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_RESET_REQ, err);
}
static fastcall noinline __unlikely int
dl_reset_res(struct dl *dl, queue_t *q, mblk_t *mp)
{
	int err;

	if (!(dl->proto.info.dl_service_mode & DL_CODLS))
		goto unsupported;
	if (dl->ops == NULL || dl->ops->dl_resume == NULL)
		goto notsupported;
	if (dl_get_state(dl) != DL_PROV_RESET_PENDING)
		goto outstate;
	dl_set_state(dl, DL_RESET_RES_PENDING);
	if ((err = dl_ok_ack(dl, q, mp, DL_RESET_RES, (union DL_primitives *) NULL)))
		goto error;
	return (0);
      outstate:
	err = DL_OUTSTATE;
	goto error;
      notsupported:
	err = DL_NOTSUPPORTED;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_RESET_RES, err);
}
static fastcall noinline __unlikely int
dl_data_ack_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_reply(dl, q, mp, DL_DATA_ACK_REQ, DL_UNSUPPORTED);
}
static fastcall noinline __unlikely int
dl_reply_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_reply(dl, q, mp, DL_DATA_ACK_REQ, DL_UNSUPPORTED);
}
static fastcall noinline __unlikely int
dl_reply_update_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_reply(dl, q, mp, DL_DATA_ACK_REQ, DL_UNSUPPORTED);
}
static fastcall noinline __unlikely int
dl_xid_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_xid_req_t *p = (typeof(p)) mp->mp_b_rptr;
	int err;
	size_t dlen;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto fault;
	switch (dl_get_state(dl)) {
	case DL_IDLE:
	case DL_DATAXFER:
		break;
	default:
		goto outstate;
	}
	if (dl->ops == NULL || dl->ops->dl_xidtest == NULL)
		goto notsupported;
	if (!MBLKIN(mp, p->dl_dest_addr_offset, p->dl_dest_addr_length))
		goto badaddr;
	if (p->dl_dest_addr_length != dl->dadsaplen)
		goto badaddr;
	dlen = msgdsize(mp);
	if (dlen < dl->proto.info.dl_min_sdu || dlen > dl->proto.info.dl_max_sdu)
		goto baddata;
	switch (p->dl_flag) {
	case 0:
	case DL_POLL_FINAL:
		break;
	default:
		goto unsupported;
	}
	err = dl->ops->dl_xidtest(dl, q, mp, p->dl_primitive, p->dl_flag,
				  (caddr_t) p + p->dl_dest_addr_offset, p->dl_dest_addr_length,
				  mp->b_cont);
	if (err)
		goto error;
	return (0);
      baddata:
	err = DL_BADDATA;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      badaddr:
	err = DL_BADADDR;
	goto error;
      notsupported:
	err = DL_NOTSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      fault:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_XID_REQ, err);
}
static fastcall noinline __unlikely int
dl_xid_res(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_xid_res_t *p = (typeof(p)) mp->mp_b_rptr;
	int err;
	size_t dlen;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto fault;
	switch (dl_get_state(dl)) {
	case DL_IDLE:
	case DL_DATAXFER:
		break;
	default:
		goto outstate;
	}
	if (dl->ops == NULL || dl->ops->dl_xidtest == NULL)
		goto notsupported;
	if (!MBLKIN(mp, p->dl_dest_addr_offset, p->dl_dest_addr_length))
		goto badaddr;
	if (p->dl_dest_addr_length != dl->dadsaplen)
		goto badaddr;
	dlen = msgdsize(mp);
	if (dlen < dl->proto.info.dl_min_sdu || dlen > dl->proto.info.dl_max_sdu)
		goto baddata;
	switch (p->dl_flag) {
	case 0:
	case DL_POLL_FINAL:
		break;
	default:
		goto unsupported;
	}
	err = dl->ops->dl_xidtest(dl, q, mp, p->dl_primitive, p->dl_flag,
				  (caddr_t) p + p->dl_dest_addr_offset, p->dl_dest_addr_length,
				  mp->b_cont);
	if (err)
		goto error;
	return (0);
      baddata:
	err = DL_BADDATA;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      badaddr:
	err = DL_BADADDR;
	goto error;
      notsupported:
	err = DL_NOTSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      fault:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_XID_RES, err);
}
static fastcall noinline __unlikely int
dl_test_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_test_req_t *p = (typeof(p)) mp->mp_b_rptr;
	int err;
	size_t dlen;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto fault;
	switch (dl_get_state(dl)) {
	case DL_IDLE:
	case DL_DATAXFER:
		break;
	default:
		goto outstate;
	}
	if (dl->ops == NULL || dl->ops->dl_xidtest == NULL)
		goto notsupported;
	if (!MBLKIN(mp, p->dl_dest_addr_offset, p->dl_dest_addr_length))
		goto badaddr;
	if (p->dl_dest_addr_length != dl->dadsaplen)
		goto badaddr;
	dlen = msgdsize(mp);
	if (dlen < dl->proto.info.dl_min_sdu || dlen > dl->proto.info.dl_max_sdu)
		goto baddata;
	switch (p->dl_flag) {
	case 0:
	case DL_POLL_FINAL:
		break;
	default:
		goto unsupported;
	}
	err = dl->ops->dl_xidtest(dl, q, mp, p->dl_primitive, p->dl_flag,
				  (caddr_t) p + p->dl_dest_addr_offset, p->dl_dest_addr_length,
				  mp->b_cont);
	if (err)
		goto error;
	return (0);
      baddata:
	err = DL_BADDATA;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      badaddr:
	err = DL_BADADDR;
	goto error;
      notsupported:
	err = DL_NOTSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      fault:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_TEST_REQ, err);
}
static fastcall noinline __unlikely int
dl_test_res(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_test_res_t *p = (typeof(p)) mp->mp_b_rptr;
	int err;
	size_t dlen;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto fault;
	switch (dl_get_state(dl)) {
	case DL_IDLE:
	case DL_DATAXFER:
		break;
	default:
		goto outstate;
	}
	if (dl->ops == NULL || dl->ops->dl_xidtest == NULL)
		goto notsupported;
	if (!MBLKIN(mp, p->dl_dest_addr_offset, p->dl_dest_addr_length))
		goto badaddr;
	if (p->dl_dest_addr_length != dl->dadsaplen)
		goto badaddr;
	dlen = msgdsize(mp);
	if (dlen < dl->proto.info.dl_min_sdu || dlen > dl->proto.info.dl_max_sdu)
		goto baddata;
	switch (p->dl_flag) {
	case 0:
	case DL_POLL_FINAL:
		break;
	default:
		goto unsupported;
	}
	err = dl->ops->dl_xidtest(dl, q, mp, p->dl_primitive, p->dl_flag,
				  (caddr_t) p + p->dl_dest_addr_offset, p->dl_dest_addr_length,
				  mp->b_cont);
	if (err)
		goto error;
	return (0);
      baddata:
	err = DL_BADDATA;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      badaddr:
	err = DL_BADADDR;
	goto error;
      notsupported:
	err = DL_NOTSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      fault:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_TEST_RES, err);
}
static fastcall noinline __unlikely int
dl_phys_addr_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_phys_addr_req_t *p = (typeof(p)) mp->b_rptr;
	int err;
	caddr_t add_ptr;
	size_t add_len;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto fault;
	if (dl_get_state(dl) == DL_UNATTACHED)
		goto outstate;
	switch (p->dl_addr_type) {
	case DL_FACT_PHYS_ADDR:
		add_ptr = (caddr_t) dl->fact.buf;
		add_len = (size_t) dl->fact.len;
		break;
	case DL_CURR_PHYS_ADDR:
		add_ptr = (caddr_t) dl->curr.buf;
		add_len = (size_t) dl->curr.len;
		break;
	default:
		goto unsupported;
	}
	if ((err = dl_phys_addr_ack(dl, q, mp, add_ptr, add_len)))
		goto error;
	return (0);
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      fault:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_PHYS_ADDR_REQ, err);
}
static fastcall noinline __unlikely int
dl_set_phys_addr_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_set_phys_addr_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto fault;
	if (dl_get_state(dl) == DL_UNATTACHED)
		goto outstate;
	if (dl->ops == NULL || dl->ops->dl_setphys == NULL)
		goto notsupported;
	if (p->dl_addr_length == 0)
		goto badaddr;
	if ((err = dl_ok_ack(dl, q, mp, DL_SET_PHYS_ADDR_REQ, (union DL_primitives *) p)))
		goto error;
	return (0);
      badaddr:
	err = DL_BADADDR;
	goto error;
      notsupported:
	err = DL_NOTSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      fault:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_SET_PHYS_ADDR_REQ, err);
}
static fastcall noinline __unlikely int
dl_get_statistics_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	int err;
	caddr_t sta_ptr = (caddr_t) dl->sta.buf;
	size_t sta_len = (size_t) dl->sta.len;

	if ((err = dl_get_statistics_ack(dl, q, mp, sta_ptr, sta_len)))
		goto error;
	return (0);
      error:
	return dl_error_reply(dl, q, mp, DL_GET_STATISTICS_REQ, err);
}
static fastcall noinline __unlikely int
dl_notify_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_notify_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto fault;
	switch (dl_get_state(dl)) {
	case DL_UNATTACHED:
	case DL_ATTACH_PENDING:
	case DL_DETACH_PENDING:
		break;
	default:
		goto outstate;
	}
	if (p->dl_timelimit != 0)
		goto unsupported;
	if (dl->ops == NULL || dl->ops->dl_notify == NULL)
		goto notsupported;
	if ((err = dl_notify_ack(dl, q, mp, p->dl_notifications)))
		goto error;
	return (0);
      notsupported:
	err = DL_NOTSUPPORTED;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      fault:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_NOTIFY_REQ, err);
}
static fastcall noinline __unlikely int
dl_aggr_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_reply(dl, q, mp, DL_AGGR_REQ, DL_NOTSUPPORTED);
}
static fastcall noinline __unlikely int
dl_unaggr_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_reply(dl, q, mp, DL_UNAGGR_REQ, DL_NOTSUPPORTED);
}
static fastcall noinline __unlikely int
dl_capability_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_capability_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto fault;
	if (!MBLKIN(mp, p->dl_sub_offset, p->dl_sub_length))
		goto fault;
	if (jj(err = dl

	goto notsupported;
notsupported:
	err = DL_NOTSUPPORTED;
	goto error;
fault:
	err = -EFAULT;
	goto error;
error:
	return dl_error_reply(dl, q, mp, DL_CAPABILITY_REQ, err);
}
static fastcall noinline __unlikely int
dl_control_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_reply(dl, q, mp, DL_CONTROL_REQ, DL_NOTSUPPORTED);
}
static fastcall noinline __unlikely int
dl_passive_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_ok_ack(dl, q, mp, DL_PASSIVE_REQ, NULL);
}
static fastcall noinline __unlikely int
dl_intr_mode_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_reply(dl, q, mp, DL_INTR_MODE_REQ, DL_NOTSUPPORTED);
}
static fastcall noinline __unlikely int
dl_hp_ppa_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_hp_ppa_ack(dl, q, mp);
}
static fastcall noinline __unlikely int
dl_hp_multicast_list_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	int err;

	if (dl_get_state(dl) == DL_UNATTACHED)
		goto outstate;
	if ((err = dl_hp_multicast_list_ack(dl, q, mp)))
		goto error;
	return (0);
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_HP_MULTICAST_LIST_REQ, err);
}
static fastcall inline __hot int
dl_hp_rawdata_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	int err;
	size_t dlen;

	if (!(dl->proto.info.dl_service_mode & DL_HP_RAWDLS))
		goto unsupported;
	switch (dl_get_state(dl)) {
	case DL_DATAXFER:
		break;
	case DL_PROV_RESET_PENDING:
	case DL_IDLE:
		goto discard;
	default:
		goto outstate;
	}
	if (dl->ops == NULL || dl->ops->dl_snddata == NULL)
		goto notsupported;
	if (mp->b_cont == NULL)
		goto baddata;
	dlen = msgdsize(mp->b_cont);
	if (dlen < dl->proto.info.dl_min_sdu || dlen > dl->proto.info.dl_max_sdu)
		goto baddata;
	if ((err = dl_op_snddata(dl, q, mp, p->dl_primitive, mp->b_cont)))
		goto merror;
	return (0);
      baddata:
	err = DL_BADDATA;
	goto merror;
      notsupported:
	err = DL_NOTSUPPORTED;
	goto merror;
      outstate:
	err = DL_OUTSTATE;
	goto merror;
      discard:
	freemsg(mp);
	return (0);
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_HP_RAWDATA_REQ, err);
      merror:
	return m_error_reply(dl, q, mp, DL_HP_RAWDATA_REQ, err);
}
static fastcall noinline __unlikely int
dl_hp_hw_reset_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	int err;

	switch (dl_get_state(dl)) {
	case DL_IDLE:
	case DL_UNBOUND:
		break;
	default:
		goto outstate;
	}
	if ((err = dl_hw_reset(dl)))
		goto error;
	return dl_ok_ack(dl, q, mp, DL_HP_HW_RESET_REQ, (union DL_primitives *) NULL);
      outstate:
	err = DL_OUTSTATE;
	break;
      error:
	return dl_error_reply(dl, q, mp, DL_HP_HW_RESET_REQ, err);
}
static fastcall noinline __unlikely int
dl_hp_info_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	int err;

	if (!(dl->proto.info.dl_service_mode & DL_CODLS))
		goto unsupported;
	switch (dl_get_state(dl)) {
	case DL_IDLE:
	case DL_OUTCON_PENDING:
	case DL_INCON_PENDING:
	case DL_USER_RESET_PENDING:
	case DL_PROV_RESET_PENDING:
		break;
	default:
		goto outstate;
	}
	return dl_hp_info_ack(dl, q, mp);
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_HP_INFO_REQ, err);
}
static fastcall noinline __unlikely int
dl_hp_set_ack_to_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_hp_set_ack_to_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto efault;
	if (!(dl->proto.info.dl_service_mode & DL_CODLS))
		goto unsupported;
	if (dl_get_state(dl) == DL_UNATTACHED)
		goto outstate;
	if (p->dl_ack_to == 0)
		goto einval;
	if (p->dl_ack_to > (INT_MAX / 100))
		goto einval;
	dl->proto.hp_info.dl_ack_to = p->dl_ack_to * 100;
	return dl_ok_ack(dl, q, mp, DL_HP_SET_ACK_TO_REQ, (union DL_primitives *) p);
      einval:
	err = -EINVAL;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      efault:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_HP_SET_ACK_TO_REQ, err);

}
static fastcall noinline __unlikely int
dl_hp_set_p_to_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_hp_set_p_to_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto efault;
	if (!(dl->proto.info.dl_service_mode & DL_CODLS))
		goto unsupported;
	if (dl_get_state(dl) == DL_UNATTACHED)
		goto outstate;
	if (p->dl_p_to == 0)
		goto einval;
	if (p->dl_p_to > (INT_MAX / 100))
		goto einval;
	dl->proto.hp_info.dl_p_to = p->dl_p_to * 100;
	return dl_ok_ack(dl, q, mp, DL_HP_SET_P_TO_REQ, (union DL_primitives *) p);
      einval:
	err = -EINVAL;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      efault:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_HP_SET_P_TO_REQ, err);
}
static fastcall noinline __unlikely int
dl_hp_set_rej_to_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_hp_set_rej_to_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto efault;
	if (!(dl->proto.info.dl_service_mode & DL_CODLS))
		goto unsupported;
	if (dl_get_state(dl) == DL_UNATTACHED)
		goto outstate;
	if (p->dl_rej_to == 0)
		goto einval;
	if (p->dl_rej_to > (INT_MAX / 100))
		goto einval;
	dl->proto.hp_info.dl_rej_to = p->dl_rej_to * 100;
	return dl_ok_ack(dl, q, mp, DL_HP_SET_REJ_TO_REQ, (union DL_primitives *) p);
      einval:
	err = -EINVAL;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      efault:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_HP_SET_REJ_TO_REQ, err);
}
static fastcall noinline __unlikely int
dl_hp_set_busy_to_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_hp_set_busy_to_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto efault;
	if (!(dl->proto.info.dl_service_mode & DL_CODLS))
		goto unsupported;
	if (dl_get_state(dl) == DL_UNATTACHED)
		goto outstate;
	if (p->dl_busy_to == 0)
		goto einval;
	if (p->dl_busy_to > (INT_MAX / 100))
		goto einval;
	dl->proto.hp_info.dl_busy_to = p->dl_busy_to * 100;
	return dl_ok_ack(dl, q, mp, DL_HP_SET_BUSY_TO_REQ, (union DL_primitives *) p);
      einval:
	err = -EINVAL;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      efault:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_HP_SET_BUSY_TO_REQ, err);
}
static fastcall noinline __unlikely int
dl_hp_set_send_ack_to_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_hp_set_send_ack_to_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto efault;
	if (!(dl->proto.info.dl_service_mode & DL_CODLS))
		goto unsupported;
	if (dl_get_state(dl) == DL_UNATTACHED)
		goto outstate;
	if (p->dl_send_ack_to == 0)
		goto einval;
	if (p->dl_send_ack_to > (INT_MAX / 100))
		goto einval;
	dl->proto.hp_info.dl_send_ack_to = p->dl_send_ack_to * 100;
	return dl_ok_ack(dl, q, mp, DL_HP_SET_SEND_ACK_TO_REQ, (union DL_primitives *) p);
      einval:
	err = -EINVAL;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      efault:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_HP_SET_SEND_ACK_TO_REQ, err);
}
static fastcall noinline __unlikely int
dl_hp_set_max_retries_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_hp_set_max_retries_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto efault;
	if (!(dl->proto.info.dl_service_mode & DL_CODLS))
		goto unsupported;
	if (dl_get_state(dl) == DL_UNATTACHED)
		goto outstate;
	if (p->dl_max_retries == 0)
		goto einval;
	if (p->dl_max_retries > INT_MAX)
		goto einval;
	dl->proto.hp_info.dl_max_retries = p->dl_max_retries;
	return dl_ok_ack(dl, q, mp, DL_HP_SET_MAX_RETRIES_REQ, (union DL_primitives *) p);
      einval:
	err = -EINVAL;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      efault:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_HP_SET_MAX_RETRIES_REQ, err);
}
static fastcall noinline __unlikely int
dl_hp_set_ack_threshold_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_hp_set_ack_threshold_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto efault;
	if (!(dl->proto.info.dl_service_mode & DL_CODLS))
		goto unsupported;
	if (dl_get_state(dl) == DL_UNATTACHED)
		goto outstate;
	if (p->dl_ack_thres == 0)
		goto einval;
	if (p->dl_ack_thres > INT_MAX)
		goto einval;
	dl->proto.hp_info.dl_ack_thres = p->dl_ack_thres;
	return dl_ok_ack(dl, q, mp, DL_HP_SET_ACK_THRESHOLD_REQ, (union DL_primitives *) p);
      einval:
	err = -EINVAL;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      efault:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_HP_SET_ACK_THRESHOLD_REQ, err);
}
static fastcall noinline __unlikely int
dl_hp_set_local_win_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_hp_set_local_win_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto efault;
	if (!(dl->proto.info.dl_service_mode & DL_CODLS))
		goto unsupported;
	if (dl_get_state(dl) == DL_UNATTACHED)
		goto outstate;
	if (p->dl_local_win == 0)
		goto einval;
	if (p->dl_local_win > INT_MAX)
		goto einval;
	dl->proto.hp_info.dl_local_win = p->dl_local_win;
	return dl_ok_ack(dl, q, mp, DL_HP_SET_LOCAL_WIN_REQ, (union DL_primitives *) p);
      einval:
	err = -EINVAL;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      efault:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_HP_SET_LOCAL_WIN_REQ, err);
}
static fastcall noinline __unlikely int
dl_hp_set_remote_win_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_hp_set_remote_win_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto efault;
	if (!(dl->proto.info.dl_service_mode & DL_CODLS))
		goto unsupported;
	if (dl_get_state(dl) == DL_UNATTACHED)
		goto outstate;
	if (p->dl_remote_win == 0)
		goto einval;
	if (p->dl_remote_win > INT_MAX)
		goto einval;
	dl->proto.hp_info.dl_remote_win = p->dl_remote_win;
	return dl_ok_ack(dl, q, mp, DL_HP_SET_REMOTE_WIN_REQ, (union DL_primitives *) p);
      einval:
	err = -EINVAL;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      efault:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_HP_SET_REMOTE_WIN_REQ, err);
}
static fastcall noinline __unlikely int
dl_hp_clear_stats_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	int err;

	if (!(dl->proto.info.dl_service_mode & DL_CODLS))
		goto unsupported;
	switch (dl_get_state(dl)) {
	case DL_IDLE:
	case DL_OUTCON_PENDING:
	case DL_INCON_PENDING:
	case DL_DATAXFER:
	case DL_USER_RESET_PENDING:
	case DL_PROV_RESET_PENDING:
		break;
	default:
		goto outstate;
	}
	dl->proto.hp_info.dl_mem_fails = 0;
	dl->proto.hp_info.dl_queue_fails = 0;
	dl->proto.hp_info.dl_ack_to_cnt = 0;
	dl->proto.hp_info.dl_p_to_cnt = 0;
	dl->proto.hp_info.dl_rej_to_cnt = 0;
	dl->proto.hp_info.dl_busy_to_cnt = 0;
	dl->proto.hp_info.dl_i_pkts_in = 0;
	dl->proto.hp_info.dl_i_pkts_oos = 0;
	dl->proto.hp_info.dl_i_pkts_in_drop = 0;
	dl->proto.hp_info.dl_i_pkts_out = 0;
	dl->proto.hp_info.dl_s_pkts_in = 0;
	dl->proto.hp_info.dl_s_pkts_out = 0;
	dl->proto.hp_info.dl_u_pkts_in = 0;
	dl->proto.hp_info.dl_u_pkts_out = 0;
	dl->proto.hp_info.dl_bad_pkts = 0;
	dl->proto.hp_info.dl_retry_cnt = 0;
	dl->proto.hp_info.dl_max_retry_cnt = 0;
	dl->proto.hp_info.dl_remote_busy_cnt = 0;
	dl->proto.hp_info.dl_hw_req_fails = 0;
	return dl_ok_ack(dl, q, mp, DL_HP_CLEAR_STATS_REQ, (union DL_primitives *) p);
      outstate:
	err = DL_OUTSTATE;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_HP_CLEAR_STATS_REQ, err);
}
static fastcall noinline __unlikely int
dl_hp_set_local_busy_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	int err;

	if (!(dl->proto.info.dl_service_mode & DL_CODLS))
		goto unsupported;
	switch (dl_get_state(dl)) {
	case DL_IDLE:
	case DL_OUTCON_PENDING:
	case DL_INCON_PENDING:
	case DL_DATAXFER:
	case DL_USER_RESET_PENDING:
	case DL_PROV_RESET_PENDING:
		break;
	default:
		goto outstate;
	}
	if (dl->ops == NULL || dl->ops->dl_setbusy == NULL)
		goto notsupported;
	return dl_ok_ack(dl, q, mp, DL_HP_SET_LOCAL_BUSY_REQ, NULL);
      notsupported:
	err = DL_NOTSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_HP_SET_LOCAL_BUSY_REQ, err);
}
static fastcall noinline __unlikely int
dl_hp_clear_local_busy_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	int err;

	if (!(dl->proto.info.dl_service_mode & DL_CODLS))
		goto unsupported;
	switch (dl_get_state(dl)) {
	case DL_IDLE:
	case DL_OUTCON_PENDING:
	case DL_INCON_PENDING:
	case DL_DATAXFER:
	case DL_USER_RESET_PENDING:
	case DL_PROV_RESET_PENDING:
		break;
	default:
		goto outstate;
	}
	if (dl->ops == NULL || dl->ops->dl_setbusy == NULL)
		goto notsupported;
	return dl_ok_ack(dl, q, mp, DL_HP_CLEAR_LOCAL_BUSY_REQ, NULL);
      notsupported:
	err = DL_NOTSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_HP_CLEAR_LOCAL_BUSY_REQ, err);
}

/*
 All primitives:

	DL_INFO_REQ
	DL_INFO_ACK
	DL_ATTACH_REQ
	DL_DETACH_REQ
	DL_BIND_REQ
	DL_BIND_ACK
	DL_UNBIND_REQ
	DL_OK_ACK
	DL_ERROR_ACK
	DL_SUBS_BIND_REQ
	DL_SUBS_BIND_ACK
	DL_SUBS_UNBIND_REQ
	DL_ENABMULTI_REQ
	DL_DISABMULTI_REQ
	DL_PROMISCON_REQ
	DL_PROMISCOFF_REQ
	DL_UNITDATA_REQ
	DL_UNITDATA_IND
	DL_UDERROR_IND
	DL_UDQOS_REQ
	DL_CONNECT_REQ
	DL_CONNECT_IND
	DL_CONNECT_RES
	DL_CONNECT_CON
	DL_TOKEN_REQ
	DL_TOKEN_ACK
	DL_DISCONNECT_REQ
	DL_DISCONNECT_IND
	DL_RESET_REQ
	DL_RESET_IND
	DL_RESET_RES
	DL_RESET_CON
	DL_DATA_ACK_REQ
	DL_DATA_ACK_IND
	DL_DATA_ACK_STATUS_IND
	DL_REPLY_REQ
	DL_REPLY_IND
	DL_REPLY_STATUS_IND
	DL_REPLY_UPDATE_REQ
	DL_REPLY_UPDATE_STATUS_IND
	DL_XID_REQ
	DL_XID_IND
	DL_XID_RES
	DL_XID_CON
	DL_TEST_REQ
	DL_TEST_IND
	DL_TEST_RES
	DL_TEST_CON
	DL_PHYS_ADDR_REQ
	DL_PHYS_ADDR_ACK
	DL_SET_PHYS_ADDR_REQ
	DL_GET_STATISTICS_REQ
	DL_GET_STATISTICS_ACK
	DL_NOTIFY_REQ
	DL_NOTIFY_ACK
	DL_NOTIFY_IND
	DL_AGGR_REQ
	DL_AGGR_IND
	DL_UNAGGR_REQ
	DL_CAPABILITY_REQ
	DL_CAPABILITY_ACK
	DL_CONTROL_REQ
	DL_CONTROL_ACK
	DL_PASSIVE_REQ
	DL_INTR_MODE_REQ
	DL_HP_PPA_REQ
	DL_HP_PPA_ACK
	DL_HP_MULTICAST_LIST_REQ
	DL_HP_MULTICAST_LIST_ACK
	DL_HP_RAWDATA_REQ
	DL_HP_RAWDATA_IND
	DL_HP_HW_RESET_REQ
	DL_HP_INFO_REQ
	DL_HP_INFO_ACK
	DL_HP_SET_ACK_TO_REQ
	DL_HP_SET_P_TO_REQ
	DL_HP_SET_REJ_TO_REQ
	DL_HP_SET_BUSY_TO_REQ
	DL_HP_SET_SEND_ACK_TO_REQ
	DL_HP_SET_MAX_RETRIES_REQ
	DL_HP_SET_ACK_THRESHOLD_REQ
	DL_HP_SET_LOCAL_WIN_REQ
	DL_HP_SET_REMOTE_WIN_REQ
	DL_HP_CLEAR_STATS_REQ
	DL_HP_SET_LOCAL_BUSY_REQ
	DL_HP_CLEAR_LOCAL_BUSY_REQ
*/
