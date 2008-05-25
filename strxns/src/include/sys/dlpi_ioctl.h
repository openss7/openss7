/*****************************************************************************

 @(#) $Id: dlpi_ioctl.h,v 0.9.2.1 2008-05-25 12:46:56 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation; version 3 of the License.

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

 Last Modified $Date: 2008-05-25 12:46:56 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: dlpi_ioctl.h,v $
 Revision 0.9.2.1  2008-05-25 12:46:56  brian
 - added manual pages, libraries, utilities and drivers

 *****************************************************************************/

#ifndef __SYS_DLPI_IOCTL_H__
#define __SYS_DLPI_IOCTL_H__

#ident "@(#) $RCSfile: dlpi_ioctl.h,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

#ifdef _SUN_SOURCE

#define DLIOC		('D' << 8)
#define DLIOCRAW	(DLIOC|1)	/* M_DATA "raw" mode */
#define DLIOCNATIVE	(DLIOC|2)	/* Native traffic mode */
#define DLIOCMARGININFO	(DLIOC|3)	/* margin size info */
#define DLIOCHDRINFO	(DLIOC|10)	/* IP fast-path */

#define DL_IOC_HDR_INFO	DLIOCHDRINFO

#endif				/* _SUN_SOURCE */

#ifdef _HPUX_SOURCE

#include <sys/dlpi.h>

#define DLIOC		('D' << 8)
#define DL_IOC_DRIVER_OPTIONS		(DLIOC|9)
#define DL_IOC_HDR_INFO			(DLIOC|10)
#define DL_HP_SET_DRV_PARAM_IOCTL	(DLIOC|11)
#define DL_HP_GET_DRV_PARAM_IOCTL	(DLIOC|12)
#define DLPI_SET_NOLOOPBACK		(DLIOC|13)

typedef struct {
	uint32_t driver_ops_type;
	uint32_t driver_ops_type_1;
	uint32_t driver_ops_type_2;
} driver_ops_t;

#define DRIVER_CKO_IN		(1UL<<0)	/* Supports inbound IPv4/IPv6 CKO.  DLPI
						   differentiates between IPv4 and IPv6. */
#define DRIVER_CKO_OUT		(1UL<<1)	/* Supports outbound IPv4/IPv6 CKO.  DLPI
						   differentiates between IPv4 and IPv6. */
#define DRIVER_CKO_OUT_DF	(1UL<<2)	/* Supports CKO for fragmented packets. */
#define DRIVER_COW		(1UL<<3)	/* Not supported by HP-UX. */
#define DRIVER_LFP		(1UL<<4)	/* Not supported by HP-UX. */
#define DRIVER_LNP		(1UL<<5)	/* Not supported by HP-UX. */
#define DRIVER_PRI		(1UL<<6)	/* Reserved for future use. */
#define DRIVER_TRAIN		(1UL<<7)	/* Supports packet trains. */
#define DRIVER_LLC		(1UL<<8)	/* Reserved for future use. */
#define DRIVER_FASTPATH_IN	(1UL<<9)	/* Supports inbound fast-path. */

/*
 * The following structure and definitions are used by the
 * DL_HP_SET_DRV_PARAM_IOCTL and DL_HP_GET_DRV_PARAM_IOCTL input-output
 * controls.
 */

typedef struct dl_hp_set_drv_param_ioctl {
	uint32_t dl_request;
	uint32_t dl_speed;
	uint32_t dl_duplex;
	uint32_t dl_autoneg;
	uint32_t dl_mtu;
	uint64_t dl_value1;
	uint64_t dl_value2;
	uint64_t dl_value3;
	uint32_t dl_reserved1[2];
	uint32_t dl_reserved2[2];
} dl_hp_set_drv_param_ioctl_t;

#define DL_HP_DRV_SPEED		(1UL<< 0)	/* dl_speed is valid. */
#define DL_HP_DRV_DUPLEX	(1UL<< 1)	/* dl_duplex is valid. */
#define DL_HP_DRV_AUTONEG	(1UL<< 2)	/* dl_autoneg is valid. */
#define DL_HP_DRV_MTU		(1UL<< 3)	/* dl_mtu is valid. */
#define DL_HP_DRV_RESET_MTU	(1UL<< 4)	/* Reset MTU. */
#define DL_HP_DRV_VALUE1	(1UL<< 5)	/* dl_value1 is valid. */
#define DL_HP_DRV_VALUE2	(1UL<< 6)	/* dl_value2 is valid. */
#define DL_HP_DRV_VALUE3	(1UL<< 7)	/* dl_value3 is valid. */
#define DL_HP_DRV_RESERVED1	(1UL<< 8)	/* dl_reserved1[0] is valid. */
#define DL_HP_DRV_RESERVED2	(1UL<< 9)	/* dl_reserved1[1] is valid. */
#define DL_HP_DRV_RESERVED3	(1UL<<10)	/* dl_reserved2[0] is valid. */
#define DL_HP_DRV_RESERVED4	(1UL<<11)	/* dl_reserved2[1] is valid. */
#define DL_HP_SERIALIZE		(1UL<<30)	/* Serialize request. */

#define DL_HP_HALF_DUPLIX	(1UL<<0)	/* Half-Duplex. */
#define DL_HP_FULL_DUPLIX	(1UL<<1)	/* Full-Duplex. */

#define DL_HP_AUTONEG_SENSE_ON	(1UL<<0)	/* Autonegotiate/Auto-sense on. */
#define DL_HP_AUTONEG_SENSE_OFF	(1UL<<1)	/* Autonegotiate/Auto-sense off. */

#endif				/* _HPUX_SOURCE */

#ifdef _AIX_SOURCE

/* should probably be OR'ed with DD_DLC */
#define DLC_ENABLE_SAP		 1	/* enable SAP */
#define DLC_DISABLE_SAP		 2	/* disable SAP */
#define DLC_START_LS		 3	/* start LS */
#define DLC_HALT_LS		 4	/* halt LS */
#define DLC_TRACE		 5	/* trace debugging */
#define DLC_CONTACT		 6	/* contact remote LS */
#define DLC_TEST		 7	/* test remote SAP */
#define DLC_ALTER		 8	/* alter LS configuration */
#define DLC_QUERY_SAP		 9	/* query SAP statistics */
#define DLC_QUERY_LS		10	/* query LS statistics */
#define DLC_ENTER_LBUSY		11	/* enter local-busy mode */
#define DLC_EXIT_LBUSY		12	/* exit local-busy mode */
#define DLC_ENTER_SHOLD		13	/* enter short-hold mode */
#define DLC_EXIT_SHOLD		14	/* exit short-hold mode */
#define DLC_GET_EXCEP		15	/* get exceptions */
#define DLC_ADD_GRP		16	/* add address */
#define DLC_ADD_FUNC_ADDR	17	/* add functional address */
#define DLC_DEL_FUNC_ADDR	18	/* delete functional address */
#define DLC_DEL_GRP		19	/* delete address */
#define IOCINFO			/* see /usr/include/sys/ioct.h */

#define DLC_MAX_DIAG	16	/* maximum diagnostic tag length */
#define DLC_MAX_NAME	20	/* maximum name length */
#define DLC_MAX_ADDR	 8	/* maximum address (MAC) length */
#define DLC_MAX_GSAPS	 7	/* maximum number of group SAPs */
#define DLC_MAX_ROUT	20	/* maximum size of routing info */

/* used with DLC_ADD_FUNC_ADDR, DLC_DEL_FUNC_ADDR */
struct dlc_func_addr {
	uint32_t gdlc_sap_corr;		/* GDLC SAP correlator */
	uint32_t len_func_addr_mask;	/* length of functional address mask */
	uint8_t func_addr_mask[DLC_MAX_ADDR];	/* functional address mask */
};

/* used with DLC_ADD_GRP, DLC_DEL_GRP */
struct dlc_add_grp {
	uint32_t gdlc_sap_corr;		/* GDLC SAP correlator */
	uint32_t grp_addr_len;		/* group address length */
	uint8_t grp_addr[DLC_MAX_ADDR];	/* grp address to be added */
};

/* used with DLC_ALTER */

#define DLC_ALT_RTO	(1UL<< 0)	/* new repoll_time */
#define DLC_ALT_AKT	(1UL<< 1)	/* new ack_time */
#define DLC_ALT_ITO	(1UL<< 2)	/* new inact_time */
#define DLC_ALT_FHT	(1UL<< 3)	/* new force_time */
#define DLC_ALT_MIF	(1UL<< 4)	/* new maxif */
#define DLC_ALT_XWIN	(1UL<< 5)	/* new xmit_wnd */
#define DLC_ALT_MXR	(1UL<< 6)	/* new max_repoll */
#define DLC_ALT_RTE	(1UL<< 7)	/* new routing */
#define DLC_ALT_SM1	(1UL<< 8)	/* set primary SDLC control mode */
#define DLC_ALT_SM2	(1UL<< 9)	/* set secondary SDLC control mode */
#define DLC_ALT_IT1	(1UL<<10)	/* set notification on inactivity */
#define DLC_ALT_IT2	(1UL<<11)	/* set halt on inactivity */

struct dlc_alter_arg {
	uint32_t gdlc_sap_corr;		/* GDLC SAP correlator */
	uint32_t gdlc_ls_corr;		/* GDLC link station correlator */
	uint32_t flags;			/* alter flags */
	uint32_t repoll_time;		/* new repoll timeout */
	uint32_t ack_time;		/* new acknowledge timeout */
	uint32_t inact_time;		/* new inactivity timeout */
	uint32_t force_time;		/* new force timeout */
	uint32_t maxif;			/* new maximum I-frame size */
	uint32_t xmit_wind;		/* new transmit value */
	uint32_t max_repoll;		/* max repoll value */
	uint32_t routing_len;		/* routing length */
	uint8_t routing[DLC_MAX_ROUT];	/* new routing data */
	uint32_t result_flags;		/* returned flags */
};

/* used with DLC_CONTACT, DLC_DISABLE_SAP, DLC_ENTER_LBUSY, DLC_ENTER_SHOLD,
 * DLC_EXIT_LBUSY, DLC_EXIT_SHOLD, DLC_HALT_LS, DLC_TEST */
struct dlc_corr_arg {
	uint32_t gdlc_sap_corr;		/* GDLC SAP correlator */
	uint32_t gdlc_ls_corr;		/* GDLC link station correlator */
};

/* used with DLC_ENABLE_SAP */
struct dlc_esap_arg {
	uint32_t gldc_sap_corr;
	uint32_t user_sap_corr;
	uint32_t len_func_addr_mask;
	uint8_t func_addr_mask[DLC_MAX_ADDR];
	uint32_t leng_grp_addr;
	uint8_t grp_addr[DLC_MAX_ADDR];
	uint32_t max_ls;
	uint32_t flags;
	uint32_t len_laddr_name;
	uint8_t laddr_name[DLC_MAX_NAME];
	uint8_t num_grp_saps;
	uint8_t grp_sap[DLC_MAX_GSAPS];
	uint8_t res1[3];
	uint8_t local_sap;
};

/* used with DLC_GET_EXCEP */

/*
 * Result indicators for use with the result_ind field of the dlc_getx_arg
 * structure.
 */
#define DLC_TEST_RES	1	/* test complete */
#define DLC_SAPE_RES	2	/* SAP enabled */
struct dlc_sape_res {
	uint32_t max_net_send;		/* maximum write network data length */
	uint32_t lport_addr_len;	/* local port network address length */
	uint8_t lport_addr[DLC_MAX_ADDR];	/* local port address */
};

#define DLC_SAPD_RES	3	/* SAP disabled */
#define DLC_STAS_RES	4	/* LS started */
struct dlc_stas_res {
	uint32_t maxif;			/* maximum user data size */
	uint32_t rport_addr_len;	/* remote port network address length */
	uint8_t rport_addr[DLC_MAX_ADDR];	/* remote port address */
	uint32_t rname_len;		/* remote port network name length */
	uint8_t rname[DLC_MAX_NAME];	/* remote network name */
	uint8_t res[3];			/* reserved for alignment */
	uint8_t rsap;			/* remote SAP */
	uint32_t max_data_off;		/* maximum network data offset for sends */
};

#define DLC_STAH_RES	5	/* LS halted */
struct dlc_stah_res {
	uint32_t conf_ls_corr;		/* conflicting link station correlator */
};

#define DLC_DIAL_RES	6	/* can dial phone */
#define DLC_IWOT_RES	7	/* inactivity without termination */
#define DLC_IEND_RES	8	/* inactivity ended */
#define DLC_CONT_RES	8	/* contact */
#define DLC_RADD_RES	8	/* remote address or name change */
struct dlc_radd_res {
	uint32_t rname_len;		/* remote network name/addr length */
	uint8_t rname[DLC_MAX_NAME];	/* remote network name/addr */
};

union dlc_ext_res {
	struct dlc_sape_res sape_res;
	struct dlc_stas_res stas_res;
	struct dlc_stah_res stah_res;
	struct dlc_radd_res radd_res;
};

#define DLC_MAX_EXT (sizeof(union dlc_ext_res))

/*
 * Result codes for use with the result_code field of the dlc_getx_arg
 * structure.  When the least significant but is clear it indicates a
 * non-catastrophic error requiring only a reset of the LS.  When the least
 * significant bit is set, it indicates a catastrophic error requiring at
 * least a reset of the SAP.
 */
#define DLC_SUCCESS	( 0UL<<1)	/* The result indicated was successful */
#define DLC_PROT_ERR	( 1UL<<1)	/* protocol error */
#define DLC_BAD_DATA	( 2UL<<1)	/* a bad data compare on TEST */
#define DLC_NO_RBUF	( 3UL<<1)	/* no remote buffering on test */
#define DLC_RDISC	( 4UL<<1)	/* remote initiated discontact */
#define DLC_DISC_TO	( 5UL<<1)	/* dicontact abort timeout */
#define DLC_INACT_TO	( 6UL<<1)	/* inactivity timeout */
#define DLC_MSESS_RE	( 7UL<<1)	/* mid-session reset */
#define DLC_NO_FIND	( 8UL<<1)	/* cannot find remote name */
#define DLC_INV_RNAME	( 9UL<<1)	/* invalid remote name */
#define DLC_SESS_LIM	(10UL<<1)	/* session limit exceeded */
#define DLC_LST_IN_PRGS	(11UL<<1)	/* listen already in progress */
#define DLC_LS_NT_COND	(12UL<<1)	/* LS unusual network condition */
#define DLC_LS_ROUT	(13UL<<1)	/* LS resource outage */
#define DLC_REMOTE_BUSY	(14UL<<1)	/* remote station found, but busy */
#define DLC_REMOTE_CONN	(15UL<<1)	/* specified remote is already connected */
#define DLC_NAME_IN_USE	(16UL<<1)	/* local name already in use */
#define DLC_INV_LNAME	(17UL<<1)	/* invalid local name */
#define DLC_SAP_NT_COND	(18UL<<1)	/* SAP unsual network condition */
#define DLC_SAP_ROUT	(19UL<<1)	/* SAP resource outage */
#define DLC_USR_INTRF	(20UL<<1)	/* user interface error */
#define DLC_ERR_CODE	(21UL<<1)	/* error in the code has been detected */
#define DLC_SYS_ERR	(22UL<<1)	/* system error */

struct dlc_getx_arg {
	uint32_t user_sap_corr;		/* user SAP corr - RETURNED */
	uint32_t user_ls_corr;		/* user ls corr - RETURNED */
	uint32_t result_ind;		/* the flags identifying the type of excep */
	uint32_t result_code;		/* the manner of exception */
	uint8_t result_ext[DLC_MAX_EXT];	/* exception specific extension */
};

/* used with DLC_QUERY_LS */
struct dlc_ls_counters {
	uint32_t test_cmds_sent;	/* test commands sent */
	uint32_t test_cmds_fail;	/* test commands failed */
	uint32_t test_cmds_rec;		/* test command received */
	uint32_t data_pkt_sent;		/* sequenced data packets sent */
	uint32_t data_pkt_resent;	/* sequenced data packets resent */
	uint32_t max_cont_resent;	/* maximum contiguous resendings */
	uint32_t data_pkt_rec;		/* data packets received */
	uint32_t inv_pkt_rec;		/* invalid packets received */
	uint32_t adp_rec_err;		/* data detected receive errors */
	uint32_t adp_send_err;		/* adata detected transmit errors */
	uint32_t rec_inact_to;		/* received inactivity timeouts */
	uint32_t cmd_polls_sent;	/* command polls sent */
	uint32_t cmd_repolls_sent;	/* command repolls sent */
	uint32_t cmd_cont_repolls;	/* continuous repolls sent */
};

/* for use in ls_state and sap_state field */
#define DLC_OPENING	1	/* SAP or LS opening */
#define DLC_OPENED	2	/* SAP or LS opened */
#define DLC_CLOSING	3	/* SAP or LS closing */
#define DLC_INACTIVE	4	/* LS inactive */

/* for use in ls_sub_state field */
#define DLC_CALLING	(1UL<<0)	/* LS is calling */
#define DLC_LISTENING	(1UL<<1)	/* LS is listening */
#define DLC_CONTACTED	(1UL<<2)	/* LS is contacted into sequenced data mode */
#define DLC_LOCAL_BUSY	(1UL<<3)	/* local LS is currently busy */
#define DLC_REMOTE_BUSY	(1UL<<4)	/* remote LS is currently busy */

#define
struct dlc_qls_arg {
	uint32_t gdlc_sap_corr;		/* GDLC SAP correlator */
	uint32_t gdlc_ls_corr;		/* GDLC LS correlator */
	uint32_t user_sap_corr;		/* user's SAP correlator - RETURNED */
	uint32_t user_ls_corr;		/* user's LS correlator - RETURNED */
	uint8_t ls_diag[DLC_MAX_DIAG];	/* the character name of the LS */
	uint32_t ls_state;		/* current ls state */
	uint32_t ls_sub_state;		/* further clarification of state */
	struct dlc_ls_counters counters;	/* statistics */
	uint32_t protodd_len;		/* protocol dependent data byte length */
	/* followed by protocol-specific area */
};

/* used with DLC_QUERY_SAP */
struct dlc_qsap_arg {
	uint32_t gdlc_sap_corr;		/* GDLC SAP correlator */
	uint32_t user_sap_corr;		/* user's SAP correlator - RETURNED */
	uint32_t sap_state;		/* state of the SAP, returned by kernel */
	uint8_t dev[DLC_MAX_DIAG];	/* returned device handler's device name */
	uint32_t devdd_len;		/* device driver dependent data byte length */
	/* followed by device driver-specific area */
};

/* flags for use with DLC_START_LS and DLC_TRACE */
#define DLC_TRCO	(1UL<<0)	/* link trace on */
#define DLC_TRCL	(1UL<<1)	/* link trace long */

/* flags for use with DLC_START_LS */
#define DLC_SLS_STAT	(1UL<<2)	/* station type: 1 primary, 0 secondary */
#define DLC_SLS_NEGO	(1UL<<3)	/* negotiate station type: 1 yes, 0 no */
#define DLC_SLS_HOLD	(1UL<<4)	/* hold link on inactivity: 1 yes, 0 no */
#define DLC_SLS_LSVC	(1UL<<5)	/* virtual call: 1 initiate, 0 respond */
#define DLC_SLS_ADDR	(1UL<<6)	/* address type: 1 address, 0 name */

/* used with DLC_START_LS */
struct dlc_sls_arg {
	uint32_t gdlc_ls_corr;		/* GDLC user link station correlator */
	uint8_t ls_diag[DLC_MAX_DIAG];	/* the char name of the ls */
	uint32_t gdlc_sap_corr;		/* GDLC SAP correlator */
	uint32_t user_ls_corr;		/* user's LS correlator */
	uint32_t flags;			/* start link station flags */
	uint32_t trace_chan;		/* trace channel (rc of trcstart) */
	uint32_t len_raddr_name;	/* length of remote name/addr */
	uint32_t raddr_name[DLC_MAX_NAME];	/* remote addr/name */
	uint32_t maxif;			/* maximum byits in an I-field */
	uint32_t rcv_wind;		/* maximum size of receive window */
	uint32_t xmit_wind;		/* maximum size of transmit window */
	uint8_t rsap;			/* remote SAP value */
	uint8_t rsap_low;		/* remote SAP low range value */
	uint8_t rasp_high;		/* remote SAP high range value */
	uint8_t res1;			/* reserved */
	uint32_t max_repoll;		/* maximum repoll count */
	uint32_t repoll_time;		/* repoll timeout value */
	uint32_t ack_time;		/* time to delay transmission of an ack */
	uint32_t inact_time;		/* time before inactivity times out */
	uint32_t force_time;		/* time before a forced disconnect */
};

/* used with DLC_TRACE */
struct dlc_trace_arg {
	uint32_t gdlc_sap_corr;		/* GDLC SAP correlator */
	uint32_t gdlc_ls_corr;		/* GDLC LS correlator */
	uint32_t trace_chan;		/* trace channel (rc of trcstart) */
	uint32_t flags;			/* trace flags */
};

#endif				/* _AIX_SOURCE */

#endif				/* __SYS_DLPI_IOCTL_H__ */
